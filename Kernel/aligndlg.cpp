// $Id: aligndlg.cpp 662 2006-03-14 21:31:49Z alex $
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

/*
*/

#include "camtypes.h"

DECLARE_SOURCE("$Revision: 662 $");

#include "camelot.h"
#include "dlgtypes.h" 
#include "aligndlg.h" 
//#include "mario.h"
//#include "ed.h"
//#include "eddlg.h"
#include "document.h"
#include "app.h"
#include "trans2d.h"
#include "fixmem.h"
#include "rnddlgs.h"
#include "ccdc.h"
#include "progress.h"
#include "grnddib.h"
#include "fillval.h"
#include "spread.h"
#include "page.h"
#include "dialogop.h"
#include "selmsg.h"
#include "qualattr.h"
#include "dlgcol.h"
//#include "barsdlgs.h"
#include "keypress.h"
#include "bubbleid.h"

CC_IMPLEMENT_DYNCREATE(ArrangeAlignment, DialogOp)   
CC_IMPLEMENT_DYNCREATE(OpAlign, TransOperation)   
#define new CAM_DEBUG_NEW

const CDlgMode ArrangeAlignment::Mode=MODELESS;
const UINT32     ArrangeAlignment::IDD =_R(IDD_ALIGNDIALOG);

// lists of grouped radio buttons
const CGadgetID TargetArea[]={_R(IDC_ALIGNDIALOG_TOSELECTION),	// default
							  _R(IDC_ALIGNDIALOG_TOPAGE),		
					//	WEBSTER-ranbirr-13/11/96
					#ifndef WEBSTER
							  _R(IDC_ALIGNDIALOG_TOSPREAD),		
					#endif //webster
							  NULL};

// string ID lists for combo boxes (null terminated) 
// >>>> NB. must be in same order as AlignType enum <<<<
const INT32 HAlignIDS[]={_R(IDS_HALIGNNONE),
						_R(IDS_HALIGNLEFT),
						_R(IDS_HALIGNCENTRE),
						_R(IDS_HALIGNRIGHT),
						_R(IDS_HDISTRIBUTELEFT),
						_R(IDS_HDISTRIBUTECENTRE),
						_R(IDS_HDISTRIBUTERIGHT),
						_R(IDS_HDISTRIBUTEEQUI),
						NULL};
const INT32 VAlignIDS[]={_R(IDS_VALIGNNONE),
						_R(IDS_VALIGNBOTTOM),
						_R(IDS_VALIGNCENTRE),
						_R(IDS_VALIGNTOP),
						_R(IDS_VDISTRIBUTEBOTTOM),
						_R(IDS_VDISTRIBUTECENTRE),
						_R(IDS_VDISTRIBUTETOP),
						_R(IDS_VDISTRIBUTEEQUI),
						NULL};

// constants defining the size and position of the rectangles on the align dialog
const INT32 DiagWidth =100;
const INT32 DiagHeight=100;
const INT32  DiagRectWidth[DiagRects]={6,14,22,30};
const INT32 DiagRectHeight[DiagRects]={6,30,14,22};
const INT32 DiagRectOrderX[DiagRects]={1,3,0,2};
const INT32 DiagRectOrderY[DiagRects]={0,2,1,3};
const INT32 DiagRectGapX[DiagRects-1]={13,6,8};
const INT32 DiagRectGapY[DiagRects-1]={13,6,8};
LineData   ArrangeAlignment::DiagRectX[8][DiagRects];
LineData   ArrangeAlignment::DiagRectY[8][DiagRects];

// initial state of dialog
AlignParam ArrangeAlignment::Align={AlignNone,AlignNone,ToSelection};
			  
/******************************************************************************
>	ArrangeAlignment::ArrangeAlignment() :
		DialogOp(ArrangeAlignment::IDD, ArrangeAlignment::Mode) 

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	ArrangeAlignment constructor
******************************************************************************/

ArrangeAlignment::ArrangeAlignment():
	DialogOp(ArrangeAlignment::IDD, ArrangeAlignment::Mode) 
{
}       	   

/******************************************************************************
>	BOOL ArrangeAlignment::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a ArrangeAlignment dialog
******************************************************************************/

BOOL ArrangeAlignment::Init()
{
	// init array holding coords of diagram rects on dialog in all alignments
	ArrangeAlignment::CalcDiagramRectsOneAxis(ArrangeAlignment::DiagRectX,DiagRectWidth,
		DiagRectOrderX,DiagRectGapX,DiagWidth-1);
	ArrangeAlignment::CalcDiagramRectsOneAxis(ArrangeAlignment::DiagRectY,DiagRectHeight,
		DiagRectOrderY,DiagRectGapY,DiagHeight-1);

	return RegisterOpDescriptor(
		0,									// Tool ID
 		_R(IDS_ARRANGEALIGNMENT),				// String resource ID
		CC_RUNTIME_CLASS(ArrangeAlignment),	// Runtime class
 		OPTOKEN_ALIGNDLG,					// Token string
 		ArrangeAlignment::GetState,			// GetState function
		0,									// Help ID
		_R(IDBBL_ALIGNEMENT),					// Bubble ID
		_R(IDD_BARCONTROLSTORE),				// Resource ID
		_R(IDC_ALIGNEMENT),						// Control ID
		SYSTEMBAR_EDIT,						// Bar ID
		TRUE,								// Recieve system messages
		FALSE,								// Smart duplicate operation
		TRUE,								// Clean operation
		0,									// No vertical counterpart
		_R(IDS_ARRANGEALIGNMENT_ONE),			// String for one copy only
		(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
	);
}   

/******************************************************************************
>	void	ArrangeAlignment::CalcDiagramRectsOneAxis(
				LineData    x[8][DiagRects],
				const INT32 width[DiagRects],
				const INT32 order[DiagRects],
				const INT32 gap[DiagRects-1]
			)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/94
	Purpose:	calculate the x positions of the diagram rectangles
				Written as for x but applys equally to y with different params!
******************************************************************************/

void ArrangeAlignment::CalcDiagramRectsOneAxis(
	LineData x[8][DiagRects],
	const INT32 width[DiagRects], 
	const INT32 order[DiagRects],
	const INT32 gap[DiagRects-1],
	const INT32 DiagWidth
)
{
	INT32   i;
	INT32  sum=0;
	for (i=0; i<DiagRects; i++)
		sum+=width[i];
	float first=(float)width[order[0]];
	float last =(float)width[order[DiagRects-1]];
	float PosnLeft=(float)0;
	float PosnCntr=first/2;
	float PosnRght=first;
	float PosnEqui=(float)0;
	float GapLeft =((float)DiagWidth-last)/3;
	float GapCntr =((float)DiagWidth-first/2-last/2)/3;
	float GapRght =((float)DiagWidth-first)/3;
	float GapEqui =(float)(DiagWidth-sum)/3;
	for (i=0; i<DiagRects; i++)
	{
		INT32 j=order[i];
		x[AlignNone       ][j].lo = i ? x[AlignNone][order[i-1]].hi+gap[i-1] : 0;
		x[AlignLow        ][j].lo = 0;
		x[AlignCentre     ][j].lo =(DiagWidth-width[j])/2;
		x[AlignHigh       ][j].lo = DiagWidth-width[j];
		x[DistributeLow   ][j].lo = (INT32)PosnLeft;
		x[DistributeCentre][j].lo = (INT32)(PosnCntr-width[j]/2);
		x[DistributeHigh  ][j].lo = (INT32)(PosnRght-width[j]);
		x[DistributeEqui  ][j].lo = (INT32)PosnEqui;

		PosnLeft+=GapLeft;
		PosnCntr+=GapCntr;
		PosnRght+=GapRght;
		PosnEqui+=GapEqui+width[j];

		INT32 w=width[j];
		x[AlignNone       ][j].hi = x[AlignNone       ][j].lo + w;
		x[AlignLow        ][j].hi = x[AlignLow        ][j].lo + w;
		x[AlignCentre     ][j].hi = x[AlignCentre     ][j].lo + w;
		x[AlignHigh       ][j].hi = x[AlignHigh       ][j].lo + w;
		x[DistributeLow   ][j].hi = x[DistributeLow   ][j].lo + w;
		x[DistributeCentre][j].hi = x[DistributeCentre][j].lo + w;
		x[DistributeHigh  ][j].hi = x[DistributeHigh  ][j].lo + w;
		x[DistributeEqui  ][j].hi = x[DistributeEqui  ][j].lo + w;
	}
}

/******************************************************************************
>	OpState	ArrangeAlignment::GetState(String_256* pHelpString, OpDescriptor*)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Returns the OpState of the ArrangeAlignment dialogue operation
				ie. whether or not it is greyed on the menu etc!!!
******************************************************************************/

OpState	ArrangeAlignment::GetState(String_256* pHelpString, OpDescriptor*)
{
	static OpState DialogState;
	DialogState.Greyed=FALSE;
//	DialogState.Greyed=GetApplication()->FindSelection()->Count()==0;
//	if (DialogState.Greyed)
//		pHelpString->MakeMsg(_R(IDS_ARRANGEALIGNMENT_GREY));
	return DialogState;
}

/******************************************************************************
>	void ArrangeAlignment::Do(OpDescriptor*)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94 
	Purpose:	Creates and shows a ArrangeAlignment dialog
******************************************************************************/

void ArrangeAlignment::Do(OpDescriptor*)
{
	Create();
	Open();
}

/******************************************************************************
>	MsgResult ArrangeAlignment::Message(Msg* Message)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Handles all the ArrangeAlignment dialog's messages 
******************************************************************************/

MsgResult ArrangeAlignment::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg=(DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:				// dialog created - restore its previous state
				BuildIDSDropList(_R(IDC_ALIGNDIALOG_HORIZONTAL), HAlignIDS, Align.h);
				BuildIDSDropList(_R(IDC_ALIGNDIALOG_VERTICAL),   VAlignIDS, Align.v);
				SetRadioGroup(TargetArea, TargetArea[Align.target]);
				UpdateState();
				break;

			case DIM_LFT_BN_CLICKED:		// mouse clicks may change greyed state of buttons
			case DIM_SELECTION_CHANGED:		// mouse clicks may change greyed state of buttons
				UpdateState();
				break;

			case DIM_LFT_BN_DOWN:			// Handle clicks in the diagram control
				if (Msg->GadgetID == _R(IDC_ALIGNDIALOG_DIAGRAM))
					DiagramClicked((ReDrawInfoType*) Msg->DlgMsgParam);
				break;

			case DIM_REDRAW:				// handle redraw request for diagram gadget
				if (Msg->GadgetID == _R(IDC_ALIGNDIALOG_DIAGRAM))
					RedrawDiagram((ReDrawInfoType*)Msg->DlgMsgParam);
				break;

			case DIM_COMMIT:				// handle left hand OK click
				DialogOKed();
				break;

			case DIM_CANCEL:				// handle cancel
				Close();
				End();
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		// if selection has changed, update state of dialog
		if ( ((SelChangingMsg*)Message)->State == SelChangingMsg::SELECTIONCHANGED )
			UpdateState();
	}

	// Pass all unhandled messages to base class for default processing!
	return DialogOp::Message(Message);  
}  

/******************************************************************************
>	void ArrangeAlignment::DiagramClicked(ReDrawInfoType *Info)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/96
	Purpose:	Handles clicks on the diagram on the align dialog
				There are 3 zones in each of the X, Y axes. An unmodified
				click will adjust both axes. Holding down ctrl will constrain
				the X axis to none while holding down shift will constrain
				the Y axis to none. (Ctrl+shift = reset both axes)
******************************************************************************/
void ArrangeAlignment::DiagramClicked(ReDrawInfoType *Info)
{
	// We'll default to resetting any axis not changed below
	Align.h = Align.v = AlignNone;

	// We only do Y direction if constrain (ctrl) is depressed
	if (!KeyPress::IsConstrainPressed())
	{
		if (Info->pMousePos->x < Info->dx/3)			// Bottom row
			Align.h = AlignLow;
		else if (Info->pMousePos->x < (2 * Info->dx)/3)	// Middle row
			Align.h = AlignCentre;
		else											// Top row
			Align.h = AlignHigh;
	}

	// We only do X direction if adjust (shift) is depressed
	if (!KeyPress::IsAdjustPressed())
	{
		if (Info->pMousePos->y < Info->dy/3)			// Left column
			Align.v = AlignLow;
		else if (Info->pMousePos->y < (2 * Info->dy)/3)	// Middle column
			Align.v = AlignCentre;
		else											// Right column
			Align.v = AlignHigh;
	}

	// Update the drop-down lists
	SetSelectedValueIndex(_R(IDC_ALIGNDIALOG_HORIZONTAL), Align.h);
	SetSelectedValueIndex(_R(IDC_ALIGNDIALOG_VERTICAL),   Align.v);
	UpdateState();
}



/******************************************************************************
>	void ArrangeAlignment::RedrawDiagram(ReDrawInfoType* ExtraInfo)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/94
	Purpose:	Redraws the diagram on the align dialog
	Note:		Code to render objects with grad-fills commented out
				As it is allegedly too slow (but this decision will probably be reversed)
******************************************************************************/

void ArrangeAlignment::RedrawDiagram(ReDrawInfoType* ExtraInfo)
{
	// objects drawn in the render region must be relatively large in the given coord space
	// else Gavin's curve flattening results in visible straight lines
	// so every dimension is scaled by scale
	INT32 scale=1000;

	// make a render region
	DocRect VirtRendRect;
	VirtRendRect.lox=-1*scale;
	VirtRendRect.loy=-2*scale;
	VirtRendRect.hix=(DiagWidth +1)*scale;
	VirtRendRect.hiy=(DiagHeight+2)*scale;
	RenderRegion* pRender=CreateGRenderRegion(&VirtRendRect,ExtraInfo);

	if (pRender!=NULL)
	{
		pRender->SaveContext();

		// currently this must be set here before any colour tables calculated
		Quality             AntiAliasQuality(Quality::QualityMax);
		QualityAttribute    AntiAliasQualityAttr(AntiAliasQuality);
		pRender->SetQuality(&AntiAliasQualityAttr,FALSE);

		// Render the background rectangle
		DialogColourInfo RedrawColours;
		pRender->SetLineColour(RedrawColours.DialogBack());
		pRender->SetFillColour(RedrawColours.DialogBack());
		pRender->DrawRect(&VirtRendRect);

		// declared at this scope else RestoreContext() dies!
		RadialFillAttribute Fill;		

		// set up some defaults used by all objects
		Fill.MakeElliptical();
		Fill.Colour=DocColour(255,255,255);
		pRender->SetLineColour(BLACK);
		pRender->SetLineWidth(0);

		for (INT32 i=0; i<DiagRects; i++)
		{
			// reverse order in which objets are rendered (now filled!)
			INT32 j=DiagRects-1-i;

			// set fill colour of each object
			switch (j)
			{
				case  0: Fill.EndColour=DocColour(255,255,0); break;
				case  1: Fill.EndColour=DocColour(0,0,255);   break;
				case  2: Fill.EndColour=DocColour(255,0,0);   break;
				case  3: Fill.EndColour=DocColour(0,160,0);   break;
				default: Fill.EndColour=DocColour(0,0,0);     break;
			}

			// get bound rect of object to be drawn
			INT32 x=DiagRectX[Align.h][j].lo*scale;
			INT32 w=DiagRectX[Align.h][j].hi*scale-x;
			INT32 y=DiagRectY[Align.v][j].lo*scale;
			INT32 h=DiagRectY[Align.v][j].hi*scale-y;

			// create shape and fill geometries
			Path shape;
			shape.Initialise(16,8);
			shape.IsFilled=TRUE;
			shape.FindStartOfPath();
			switch (j)
			{
				case 0:
				{
					// create a rectangle
					shape.InsertMoveTo(DocCoord(x,y));
					shape.InsertLineTo(DocCoord(x,y+h));
					shape.InsertLineTo(DocCoord(x+w,y+h));
					shape.InsertLineTo(DocCoord(x+w,y));
					shape.InsertLineTo(DocCoord(x,y));

//					// create a radial fill
//					Fill.StartPoint=DocCoord(x+w*3/16,y+h*3/4);
//					Fill.EndPoint  =DocCoord(x+w*3/8,y+h/2);
//					Fill.EndPoint2 =DocCoord(x+w*3/8,y+h);
					break;
				}

				case 1:
				{
					// create a pseudo ellipse
					shape.InsertMoveTo( DocCoord(x,y+h/2));
					shape.InsertCurveTo(DocCoord(x,y+h*3/4),  DocCoord(x+w/4,y+h),  DocCoord(x+w/2,y+h));
					shape.InsertCurveTo(DocCoord(x+w*3/4,y+h),DocCoord(x+w,y+h*3/4),DocCoord(x+w,y+h/2));
					shape.InsertCurveTo(DocCoord(x+w,y+h/4),  DocCoord(x+w*3/4,y),  DocCoord(x+w/2,y));
					shape.InsertCurveTo(DocCoord(x+w/4,y),    DocCoord(x,y+h/4),    DocCoord(x,y+h/2));

// 					// create a radial fill
//					Fill.StartPoint=DocCoord(x+w*3/8,y+h*5/8);
//					Fill.EndPoint  =DocCoord(x+w*6/8,y+h/4);
//					Fill.EndPoint2 =DocCoord(x+w*6/8,y+h);
					break;
				}

				default:
				{
					// create a rounded rectangle
					shape.InsertMoveTo( DocCoord(x,y+h/2));
					shape.InsertCurveTo(DocCoord(x,y+h),  DocCoord(x,y+h),  DocCoord(x+w/2,y+h));
					shape.InsertCurveTo(DocCoord(x+w,y+h),DocCoord(x+w,y+h),DocCoord(x+w,y+h/2));
					shape.InsertCurveTo(DocCoord(x+w,y),  DocCoord(x+w,y),  DocCoord(x+w/2,y));
					shape.InsertCurveTo(DocCoord(x,y),    DocCoord(x,y),    DocCoord(x,y+h/2));

// 					// create a radial fill
//					Fill.StartPoint=DocCoord(x+w*3/16,y+h*3/4);
//					Fill.EndPoint  =DocCoord(x+w*3/8,y+h/2);
//					Fill.EndPoint2 =DocCoord(x+w*3/8,y+h);
					break;
				}

			}
//			pRender->SetFillGeometry(&Fill,FALSE);
			pRender->SetFillColour(Fill.EndColour);
			pRender->DrawPath(&shape);
		}

		pRender->RestoreContext();
		DestroyGRenderRegion(pRender);	// also blt's to screen
	}
}

/******************************************************************************
>	void ArrangeAlignment::UpdateState(void)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Updates the HState, VState, and Alignments vars in the class
				Also updates the grey state of all the dialog gadgets
******************************************************************************/

void ArrangeAlignment::UpdateState()
{
	// update target
	CGadgetID Target = ReadRadioGroup(TargetArea,TargetArea[0]);
	switch (Target)
	{
		case _R(IDC_ALIGNDIALOG_TOSELECTION): Align.target=ToSelection;	break;
		case _R(IDC_ALIGNDIALOG_TOPAGE):      Align.target=ToPage;      break;
	//	WEBSTER-ranbirr-13/11/96
	#ifndef WEBSTER
		case _R(IDC_ALIGNDIALOG_TOSPREAD):    Align.target=ToSpread;    break;
	#endif //webster
		default: ERROR3("ArrangeAlignment::UpdateState() - unknown target");
	}

	// read alignments from horizontal and vertical combo boxes
	Align.h = (AlignType)GetSelectedValueIndex(_R(IDC_ALIGNDIALOG_HORIZONTAL));
	Align.v = (AlignType)GetSelectedValueIndex(_R(IDC_ALIGNDIALOG_VERTICAL));
	
	// force the diagram to redraw
	InvalidateGadget(_R(IDC_ALIGNDIALOG_DIAGRAM));

	// set state of Apply button
	INT32 NumObjs = 0;
	if (Document::GetSelected())
		NumObjs = GetApplication()->FindSelection()->Count();
	BOOL ApplyButtonState = (NumObjs>0);
	if (NumObjs==1 && Target==_R(IDC_ALIGNDIALOG_TOSELECTION)) ApplyButtonState = FALSE;
	if (Align.h==AlignNone && Align.v==AlignNone)          ApplyButtonState = FALSE;
	EnableGadget(IDOK,ApplyButtonState);
}


/******************************************************************************
>	void ArrangeAlignment::DialogOKed(void)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Purpose:	invokes the Op
******************************************************************************/

void ArrangeAlignment::DialogOKed()
{
	if (Align.h!=AlignNone || Align.v!=AlignNone)
	{
		OpDescriptor* pOpDesc=OpDescriptor::FindOpDescriptor(OPTOKEN_OPALIGN);
		ERROR3IF_PF(pOpDesc==NULL,("Couldn't find OPTOKEN_OPALIGN op descriptor"));
		pOpDesc->Invoke((OpParam*)&Align);
	}
}

/******************************************************************************
>	void		ArrangeAlignment::BuildIDSDropList(
					const CGadgetID DropListID,
					const INT32*		IDSList,
					const INT32 		Default=0
				)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7th November 1994
	Purpose:	Builds a drop down list for a combo box from a null terminated 
				list of string IDs, selecting a default value
	Errors:		IF LIST NOT NULL TERMINATED => same prob as non-term strings!
******************************************************************************/

void ArrangeAlignment::BuildIDSDropList(const CGadgetID DropListID,
		const INT32* IDSList, INT32 Default)
{
	// Make sure the list is empty
	DeleteAllValues(DropListID);

	// build the list
	String_32 Str;
	INT32 i=0;
	while (IDSList[i])
	{
		Str.MakeMsg(IDSList[i]);
		SetStringGadgetValue(DropListID,&Str);
		i++;
	}
	SetComboListLength(DropListID);

	// select one of the entries in the list
	if (Default<0 || Default>=i)
	{
		ERROR3("BuildIDSDropList() passed a default outside the list");
		Default=0;
	}
	SetSelectedValueIndex(DropListID,Default);
}

/******************************************************************************
>	CGadgetID	ArrangeAlignment::ReadRadioGroup(
					const CGadgetID* IDCList,
					const CGadgetID IDCDefault=NULL
				)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13th October 1994
	Purpose:	Returns ID of the first set radio button in a group specified
				in a null terminated list (irespective of greyed state)
				If none are selected it returns the default ID (default NULL)
	Errors:		IF LIST NOT NULL TERMINATED => same prob as non-term strings!
******************************************************************************/

CGadgetID ArrangeAlignment::ReadRadioGroup(const CGadgetID* IDCList,
										   const CGadgetID IDCDefault)
{
	BOOL valid;
	INT32  i=0;
	while (IDCList[i])
	{
		if (GetLongGadgetValue(IDCList[i],0,1,0,&valid))
			return (IDCList[i]);
		i++;
	}
	return (IDCDefault);
}

/******************************************************************************
>	void	ArrangeAlignment::SetRadioGroup(
				const CGadgetID* IDCList,
				const CGadgetID IDC
			)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13th October 1994
	Purpose:	Sets the specified radio button in a group specified in	a null 
				terminated list (irespective of whether or not it is greyed)
				resetting all others in the group.
				If IDC is not in list, none are set.
	Errors:		IF LIST NOT NULL TERMINATED => same prob as non-term strings!
******************************************************************************/

void ArrangeAlignment::SetRadioGroup(const CGadgetID* IDCList, const CGadgetID IDC)
{
	INT32 i=0;
	while (IDCList[i])
	{
		SetLongGadgetValue(IDCList[i],IDCList[i]==IDC);
		i++;
	}
}

//////////////////////////// OpAlign class ... ////////////////////////////////

/******************************************************************************
>	OpAlign::OpAlign() : TransOperation()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	OpAlign constructor
******************************************************************************/

OpAlign::OpAlign() : TransOperation() 
{
}       

/******************************************************************************
>	BOOL OpAlign::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/9/94
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for an Align operation
******************************************************************************/

BOOL OpAlign::Init()
{  
	return RegisterOpDescriptor(
		0,							// Tool ID
 		_R(IDS_OPALIGN),				// String resource ID
		CC_RUNTIME_CLASS(OpAlign),	// Runtime class
 		OPTOKEN_OPALIGN,			// Token string
 		OpAlign::GetState,			// GetState function
		0,							// Help ID
		0,							// Bubble ID
		0,							// Resource ID
		0							// Control ID
//	needs	'GREY_WHEN_NO_CURRENT_DOC'
	);
}   

/******************************************************************************
>	OpState	OpAlign::GetState(String_256* pString, OpDescriptor* pOpDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Returns the OpState of the OpAlign dialogue operation
******************************************************************************/

OpState	OpAlign::GetState(String_256* pString, OpDescriptor* pOpDesc)
{
	return ArrangeAlignment::GetState(pString, pOpDesc);
}

/******************************************************************************
>	void OpAlign::DoWithParam(OpDescriptor* pOp, OpParam* pAlignParam)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94 
	Purpose:	performs the desired align operation
******************************************************************************/

void OpAlign::DoWithParam(OpDescriptor* pOp, OpParam* pAlignParam)
{
	// DMc alterations so that this works with compound nodes	
	SelRange   Selection(*(GetApplication()->FindSelection()));

	RangeControl rg = Selection.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Selection.Range::SetRangeControl(rg);

	DocRect		SelRect   = Selection.GetBoundingRect();
	DocRect		TargetRect;
	TargetRect.MakeEmpty();
	INT32        NumObjs   = Selection.Count();
	AlignParam* pAlign    =(AlignParam*)pAlignParam;

	BOOL moved=FALSE;					// set to TRUE if any object is moved
	BeginSlowJob(-1,FALSE);
	BOOL OK=DoStartTransOp(FALSE);

	// find parent spread of first object in selection
	Node*   pFirstNode=NULL;
	Spread* pSpread   =NULL;
	if (OK)
	{
		pFirstNode=Selection.FindFirst();
		if (pFirstNode!=NULL)
			pSpread=pFirstNode->FindParentSpread();
		OK=(pSpread!=NULL);
		if (!OK)
			ERROR2RAW("OpAlign::DoWithParam() - could not find parent spread");
	}

	// Find the size of the target rectangle
	if (pAlign->target==ToSelection)
		TargetRect=SelRect;
	else
	{
		Page* pPage=pSpread->FindFirstPageInSpread();
		while (pPage)
		{
			DocRect PageRect=pPage->GetPageRect();
			if (pAlign->target==ToSpread || SelRect.IsIntersectedWith(PageRect))
				TargetRect=TargetRect.Union(PageRect);
			pPage=pPage->FindNextPage();
		}
	}

	// allocate all dynamic memory required
	Node**	 pObj=NULL;
	ObjInfo* x   =NULL;
	ObjInfo* y   =NULL;
	INT32*    dx  =NULL;
	INT32*    dy  =NULL;
	if (OK)			ALLOC_WITH_FAIL(pObj,(Node**)  CCMalloc(NumObjs*sizeof(Node*)),  this);
	if (pObj!=NULL)	ALLOC_WITH_FAIL(x,   (ObjInfo*)CCMalloc(NumObjs*sizeof(ObjInfo)),this);
	if (   x!=NULL) ALLOC_WITH_FAIL(y,   (ObjInfo*)CCMalloc(NumObjs*sizeof(ObjInfo)),this);
	if (   y!=NULL) ALLOC_WITH_FAIL(dx,  (INT32*)   CCMalloc(NumObjs*sizeof(INT32)),   this);
	if (  dx!=NULL) ALLOC_WITH_FAIL(dy,  (INT32*)   CCMalloc(NumObjs*sizeof(INT32)),   this);
	OK=(dy!=NULL);

	// if memory claimed OK and target rect not empty proceed with op
	// (ie. do nothing if 'within page(s)' when no object on a page)
	DocRect EmptyRect;
	if (OK && TargetRect!=EmptyRect)
	{
		// create an array of pointers to objects (nodes) to be affected
		Node* pNode=Selection.FindFirst();
		INT32  i=0;
		while (pNode!=NULL)
		{
			if (pNode->IsBounded() && !((NodeRenderableBounded*)pNode)->GetBoundingRect(TRUE).IsEmpty())
				pObj[i++]=pNode;
			pNode=Selection.FindNext(pNode);
		}
		NumObjs=i;

		// cache x & y info in separate arrays so they can be sorted separately
		XLONG SumObjWidths =0;
		XLONG SumObjHeights=0;
		for (i=0; i<NumObjs; i++)
		{
			DocRect ObjRect=((NodeRenderableBounded*)pObj[i])->GetBoundingRect();
			x[i].i=i;
			x[i].lo=ObjRect.lox;
			x[i].hi=ObjRect.hix;
			SumObjWidths+=ObjRect.hix-ObjRect.lox;
			y[i].i=i;
			y[i].lo=ObjRect.loy;
			y[i].hi=ObjRect.hiy;
			SumObjHeights+=ObjRect.hiy-ObjRect.loy;
		}

		// for each object, calculate the x and y displacements independently
		AlignOneAxis(pAlign->h,NumObjs,SumObjWidths, TargetRect.lox,TargetRect.hix,x,dx);
		AlignOneAxis(pAlign->v,NumObjs,SumObjHeights,TargetRect.loy,TargetRect.hiy,y,dy);

		// apply the x and y displacements simultaneously to each object
		for (i=0; i<NumObjs; i++)
			if (dx[i]!=0 || dy[i]!=0)
			{
				moved=TRUE;
				Trans2DMatrix* pMatrix=new Trans2DMatrix(dx[i],dy[i]);
				DoTransformNode((NodeRenderableInk*)pObj[i],pMatrix);
			}
	}

	// free up any memory which was allocated
	CCFree(dx);
	CCFree(dy);
	CCFree(x);
	CCFree(y);
	CCFree(pObj);

	if (moved)
	{
		Document::GetSelected()->ForceRedraw(pSpread, TargetRect);
		GetApplication()->UpdateSelection();
	}
	else
		FailAndExecute();
	End();
	EndSlowJob();
}

/******************************************************************************
 following routines used by the 'C' library qsort() in OpAlign::AlignOneAxis()
******************************************************************************/
extern "C" INT32 CompareObjInfoLow(const void* elem1, const void* elem2)
{
	ObjInfo* a=(ObjInfo*)elem1;
	ObjInfo* b=(ObjInfo*)elem2;
	if (a->lo == b->lo)
		return (a->i  < b->i  ? -1 : 1);	// sort by Z order
	else
		return (a->lo < b->lo ? -1 : 1);
}
/*****************************************************************************/
extern "C" INT32 CompareObjInfoHigh(const void* elem1, const void* elem2)
{
	ObjInfo* a=(ObjInfo*)elem1;
	ObjInfo* b=(ObjInfo*)elem2;
	if (a->hi == b->hi)
		return (a->i  < b->i  ? -1 : 1);	// sort by Z order
	else
		return (a->hi < b->hi ? -1 : 1);
}
/*****************************************************************************/
extern "C" INT32 CompareObjInfoCentre(const void* elem1, const void* elem2)
{
	ObjInfo* a=(ObjInfo*)elem1;
	ObjInfo* b=(ObjInfo*)elem2;
	INT32     d=(a->lo+a->hi)-(b->lo+b->hi);
	if (d==0)
		return (a->i < b->i ? -1 : 1);		// sort by Z order
	else
		return (   d < 0    ? -1 : 1);
}

/******************************************************************************
>	void OpAlign::AlignOneAxis(
		AlignType Align,
		INT32	  NumObjs,
		XLONG	  SumObjWidths,
		INT32	  RectLow,
		INT32	  RectHigh,
		ObjInfo*  x,			// returned sorted (ascending) if distributing
		INT32*	  dx			// returns displacements for each object
	)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94 
	Purpose:	aligns the object in one axis at a time, in the specified way
				(written as though for x, but same algorithm applies to y)
				distributing a single object leaves it unchanged.
******************************************************************************/

void OpAlign::AlignOneAxis(AlignType Align, INT32 NumObjs, XLONG SumObjWidths,
						   INT32 RectLow, INT32 RectHigh, ObjInfo* x, INT32* dx)
{
	INT32 i;

	switch (Align)
	{
		case DistributeLow:
			if (NumObjs==1)
				dx[0]=0;
			else
			{
				qsort(x,NumObjs,sizeof(ObjInfo),CompareObjInfoLow);
				INT32 RectWidth=RectHigh-RectLow;
				INT32 LastObjWidth =x[NumObjs-1].hi-x[NumObjs-1].lo;
				INT32 gap=(RectWidth-LastObjWidth)/(NumObjs-1);
				INT32 pos=RectLow;
				for (i=0; i<NumObjs; i++)
				{															                                      
					dx[x[i].i]=pos-x[i].lo;
					pos+=gap;
				}
			}
			break;

		case DistributeHigh:
			if (NumObjs==1)
				dx[0]=0;
			else
			{
				qsort(x,NumObjs,sizeof(ObjInfo),CompareObjInfoHigh);
				INT32 RectWidth=RectHigh-RectLow;
				INT32 FirstObjWidth=x[0].hi-x[0].lo;
				INT32 gap=(RectWidth-FirstObjWidth)/(NumObjs-1);
				INT32 pos=RectLow+FirstObjWidth;
				for (i=0; i<NumObjs; i++)
				{
					dx[x[i].i]=pos-x[i].hi;
					pos+=gap;
				}
			}
			break;

		case DistributeCentre:
			if (NumObjs==1)
				dx[0]=0;
			else
			{
				qsort(x,NumObjs,sizeof(ObjInfo),CompareObjInfoCentre);
				INT32 RectWidth=RectHigh-RectLow;
				INT32 LastObjWidth =x[NumObjs-1].hi-x[NumObjs-1].lo;
				INT32 FirstObjWidth=x[0].hi-x[0].lo;
				INT32 gap=(RectWidth-(FirstObjWidth+LastObjWidth)/2)/(NumObjs-1);
				INT32 pos=RectLow+FirstObjWidth/2;
				for (i=0; i<NumObjs; i++)
				{
					dx[x[i].i]=pos-(x[i].hi+x[i].lo)/2;
					pos+=gap;
				}
			}
			break;

		case DistributeEqui:
			if (NumObjs==1)
				dx[0]=0;
			else
			{
				qsort(x,NumObjs,sizeof(ObjInfo),CompareObjInfoCentre);
				INT32 RectWidth=RectHigh-RectLow;
				INT32 gap=((XLONG)RectWidth-SumObjWidths)/(NumObjs-1);
				INT32 pos=RectLow;
				for (i=0; i<NumObjs; i++)
				{
					dx[x[i].i]=pos-x[i].lo;
					pos+=gap+x[i].hi-x[i].lo;
				}
			}
			break;
		 
		case AlignLow:
			for (i=0; i<NumObjs; i++)
				dx[i]=RectLow-x[i].lo;
			break;

		case AlignCentre:
			for (i=0; i<NumObjs; i++)
				dx[i]=(RectHigh-x[i].hi+RectLow-x[i].lo)/2;
			break;

		case AlignHigh:
			for (i=0; i<NumObjs; i++)
				dx[i]=RectHigh-x[i].hi;
			break;

		case AlignNone:
			for (i=0; i<NumObjs; i++)
				dx[i]=0;
			break;
	}
}
