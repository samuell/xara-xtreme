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
// Handles the bitmap effects dialog

/*
*/

#include "camtypes.h"
//#include "bfxrc.h"
#include "bfxdlg.h"
#include "bitmap.h"
#include "bmplist.h"
#include "oilprog.h"

#include "ccdc.h"		// specific #includes needed for kernel-rendered dialogues
#include "dlgcol.h"
#include "fillval.h"
#include "grnddib.h"

#include "app.h"
#include "range.h"
#include "nodebmp.h"
#include "bitmpinf.h"
//#include "bmplist.h"
#include "bmpcomp.h"
#include "fillattr.h"
#include "bitmapfx.h"
#include "undoop.h"
#include "progress.h"
#include "document.h"
#include "chapter.h"
#include "page.h"
#include "spread.h"
#include "trans2d.h"
//#include "rikdlg.h"
//#include "resource.h"
#include "docview.h"
//#include "barsdlgs.h"			// for the bitmap effects bar control
#include "wrkrect.h"
#include "bubbleid.h"

//#include "will2.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNAMIC(BfxMsg, Msg)
CC_IMPLEMENT_DYNCREATE(BfxDlg, DialogTabOp)
CC_IMPLEMENT_DYNCREATE(BfxOp, UndoableOperation)   
CC_IMPLEMENT_MEMDUMP(BfxOpParam, OpParam)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

const CDlgMode BfxDlg::Mode = MODELESS; // Mode of the dialog  
const UINT32 BfxDlg::IDD = _R(IDD_BFXDLG); // Used to uniquely identify the tabbed dialog
												 // there is no actual resource associated with this ID.
BOOL BfxDlg::InteractiveBCC = TRUE;

/********************************************************************************************
>	BfxDlg::BfxDlg(): DialogTabOp(BfxDlg::IDD, BfxDlg::Mode) 
					 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Purpose:	The Constructor of the DialogTabOp derived class simply sets modality of the
				dialog, and it's ID.

********************************************************************************************/



BfxDlg::BfxDlg(): DialogTabOp(BfxDlg::IDD, BfxDlg::Mode) 
{
	OpenPage = 0;
	pOriginal = NULL;
	pDestination = NULL;
	pPrevOriginal=NULL;
	pOrigInfo = NULL;
	Clean = FALSE;
	pFX = NULL;
	SetParameters();
	for (INT32 i = 0; i<=8; i++) Matrix[i]=(i==4)?1.0:0.0;
}        

/********************************************************************************************
>	BfxDlg::~BfxDlg()
					 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Purpose:	Destructor

********************************************************************************************/

BfxDlg::~BfxDlg()
{
	if (pFX) delete pFX;
	if (pDestination) {TRACEUSER( "Alex", _T("Destructor deleting destination\n"));delete pDestination;}
	if (pOrigInfo) delete pOrigInfo;
}        

/*******************************************************************************************

>	BOOL BfxDlg::OriginalChanged()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Invalidates area for original bitmap, marks no destination as yet
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL BfxDlg::OriginalChanged()
{
	if (pPrevOriginal==pOriginal) return TRUE;
	pPrevOriginal=pOriginal;

	if (!pOrigInfo)
	{
		pOrigInfo = new BitmapInfo;
		if (!pOrigInfo) return FALSE; // error already set.
	}

	pOrigInfo->PixelWidth = 100;
	pOrigInfo->PixelHeight = 100;
	pOrigInfo->PixelDepth = 24;
	if (pOriginal && pOrigInfo) // Should always be true
	{
		pOriginal->ActualBitmap->GetInfo(pOrigInfo);
	}

	if (SetDirty() && (OpenPage != 0))
	{
		InvalidateGadget(_R(IDC_BFXDLG_ORIGREDRAW));
	}

	BROADCAST_TO_CLASS(DialogMsg(WindowID, DIM_OUT_OF_SPACE, _R(IDC_BFXDLG_ORIGREDRAW),
					   0, OpenPage) ,DialogOp);  // Pretty horrible bodge

	return FixOriginalComboBox();
}

/*******************************************************************************************

>	BOOL BfxDlg::SetEffect(CCRuntimeClass * Effect)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		Effect = runtime class of effect to be used, or none.
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Choses the bitmap effect for processing
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL BfxDlg::SetEffect(CCRuntimeClass * Effect)
{
	if (pFX)
	{
		if (pFX->GetRuntimeClass() == Effect) return TRUE;
		delete pFX;
		pFX = NULL;
	}
	if (Effect)
	{
		pFX = (AccusoftBitmapEffect *) (Effect->CreateObject());
		if (!pFX) return FALSE;
		return pFX->SetParameters(Param1, Param2, Param3, Matrix);
	}
	else return TRUE;
}

/*******************************************************************************************

>	BOOL BfxDlg::SetDirty(BOOL Redraw=TRUE)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Clears the clean flag and deletes the effect
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL BfxDlg::SetDirty(BOOL Redraw)
{
	if (!ClearDestination(Redraw)) return FALSE;
	Clean = FALSE;
	if (pFX)
	{
		delete pFX;
		pFX = NULL;
	}
	return TRUE;
}

/*******************************************************************************************

>	BOOL BfxDlg::DestinationChanged()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Invalidates area for destination bitmap, marks no destination as yet
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL BfxDlg::DestinationChanged()
{
	if (OpenPage)
	{
		InvalidateGadget(_R(IDC_BFXDLG_ORIGREDRAW2));
	}
	return TRUE;
}

/*******************************************************************************************

>	BOOL BfxDlg::ClearDestination(BOOL Redraw=TRUE)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Marks no dest bitmap
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL BfxDlg::ClearDestination(BOOL Redraw)
{
	if (pDestination)
	{
		delete pDestination;
		pDestination = NULL; //TRACEUSER( "Alex", _T("ClearDestination deleting dest\n"));
		return ((!Redraw) || (DestinationChanged()));
	}
	return TRUE;
}

/*******************************************************************************************

>	BOOL BfxDlg::FindBitmap()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	pOriginal set
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Set pOriginal to point at a bitmap
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL BfxDlg::FindBitmap()
{
//	pOriginal = NULL;

	Range Sel(*(GetApplication()->FindSelection()));

	Node* FirstSelectedNode = Sel.FindFirst(); 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{
	    Node* CurrentNode = FirstSelectedNode;       
	    Node* NextCurrent; 
	    
	    // Do all bitmaps. OK this should pick up the fill as well. Never mind
		while (CurrentNode != NULL)
		{
			NextCurrent = Sel.FindNext(CurrentNode);
	       	if  ( (CurrentNode->IsSelected()) && (CurrentNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBitmap)) ) 
	       	{         
				pOriginal = ((NodeBitmap *)(CurrentNode))->GetBitmap();
				return CheckOriginalBitmap();
	       	}
			CurrentNode = NextCurrent; 
		}

	} 

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapColourFill)))
		{
			pOriginal = pAttrNode->GetBitmap();
			return CheckOriginalBitmap();
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	return CheckOriginalBitmap();
}

/*******************************************************************************************

>	BOOL BfxDlg::CheckOriginalBitmap()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	pOriginal set
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Ensures pOriginal points to a valid bitmap in this document
	Errors:		None at present
	SeeAlso:	-


*******************************************************************************************/

BOOL BfxDlg::CheckOriginalBitmap()
{
	Document * pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
	{
		pOriginal = NULL;
		return OriginalChanged();
	}

	KernelBitmap * pBmp = (KernelBitmap *)(Bitmaps->GetHead());

	while (pBmp != NULL)
	{
		if (pBmp == pOriginal) return OriginalChanged(); // OK we've found it.
		pBmp = (KernelBitmap *) Bitmaps->GetNext(pBmp);
	}

	pOriginal = (KernelBitmap *)(Bitmaps->GetHead()); // possibly NULL but certainly on the list
	return OriginalChanged();
}

/*******************************************************************************************

>	BOOL BfxDlg::FixOriginalComboBox()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Makes the combo box reflect the bitmaps within the document
	Errors:		None at present
	SeeAlso:	-

This routine is heavilly copied from one within the fill tool

*******************************************************************************************/

BOOL BfxDlg::FixOriginalComboBox()
{
	// Find the doccomponent with the list of bitmaps in it
	Document * pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return TRUE;

	String_256 Str;

	if (OpenPage)
	{
		DeleteAllValues(_R(IDC_BFXDLG_ORIGCOMBO));
	}

	BOOL DoneMain = FALSE;
	INT32 SelIndex = 0;

	if (Bitmaps->GetCount() > 0)
	{
		INT32 Index = 0;

		ListItem* pBmp = Bitmaps->GetHead();

		while (pBmp != NULL)
		{
			Str = ((KernelBitmap*)pBmp)->ActualBitmap->GetName();
  			if (OpenPage)
  			{
				if (((KernelBitmap*)(pBmp)) == pOriginal)
				{
					SetStringGadgetValue(_R(IDC_BFXDLG_ORIGCOMBO),&Str, FALSE, -1);
					DoneMain = TRUE;
					SelIndex=Index;
				}
  				SetStringGadgetValue(_R(IDC_BFXDLG_ORIGCOMBO),&Str,FALSE, Index++);
			}
			pBmp = Bitmaps->GetNext(pBmp);
		}
	}
	else
	{
		Str.Load(_R(IDS_K_BFXDLG_DEFAULT));
		if (OpenPage) SetStringGadgetValue(_R(IDC_BFXDLG_ORIGCOMBO),&Str,TRUE, 0);
	}

	if (OpenPage)
	{
		Str.Load(_R(IDS_K_BFXDLG_DEFAULT));
		SetComboListLength(_R(IDC_BFXDLG_ORIGCOMBO));
		if (!DoneMain)
		{
			SetStringGadgetValue(_R(IDC_BFXDLG_ORIGCOMBO),&Str, FALSE, -1);
			SetSelectedValueIndex(_R(IDC_BFXDLG_ORIGCOMBO), 0);
		}
		else
		{
			SetSelectedValueIndex(_R(IDC_BFXDLG_ORIGCOMBO), SelIndex);
		}
	}
	return TRUE;
}


/*******************************************************************************************

>	BOOL BfxDlg::ReadOriginalComboBox()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Chooses a different bitmap from the combo box
	Errors:		None at present
	SeeAlso:	-

This routine is heavilly copied from one within the fill tool

*******************************************************************************************/

BOOL BfxDlg::ReadOriginalComboBox()
{
	if (!OpenPage) return FALSE;

	Document * pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return TRUE;

	ListItem* pBmp = Bitmaps->GetHead();

	WORD Index;
	GetValueIndex(_R(IDC_BFXDLG_ORIGCOMBO),&Index); 

	for (INT32 i = 0; i < Index; i++)
	{
		if (pBmp == NULL)
			break;

		pBmp = Bitmaps->GetNext(pBmp);
	}

	pOriginal = (KernelBitmap*)pBmp; // Might be NULL (conceivably)
	return CheckOriginalBitmap();
}

/********************************************************************************************

>	BOOL BfxDlg::RenderBitmap(ReDrawInfoType* ExtraInfo, KernelBitmap * BitmapToUse)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		ExtraInfo - The structure that has the extra data we need to start rendering
				BitmapToUse - The bitmap to render
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Fills a renderable control with a bitmap
	Errors:		None at present
	SeeAlso:	-

********************************************************************************************/

BOOL BfxDlg::RenderBitmap(ReDrawInfoType* ExtraInfo, KernelBitmap * BitmapToUse)
{
	// Go get a render region
	DocRect VirtualSize(0, 0, ExtraInfo->dx, ExtraInfo->dy);
	RenderRegion* pRender = CreateGRenderRegion(&VirtualSize, ExtraInfo);
	if (pRender!=NULL)
	{
		DialogColourInfo RedrawColours;		// Get a supplier for default dlg colours

		// A Grey colour [...hmmm, it's not a very grey grey any more... oragnge more like]
		DocColour Grey(255,200,0);

		// Render the attributes and the a rectangle
		pRender->SaveContext();
		pRender->SetLineColour(COLOUR_TRANS);

			// Draw a rectangle to fill in the background - Fill with Dialogue Background colour
		pRender->SetFillColour(RedrawColours.DialogBack());
		pRender->DrawRect(&VirtualSize);
	
		if (BitmapToUse)
		{

			NodeBitmap NB;
			NodeBitmap * pNodeBitmap = &NB;
		
			if (!((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12))))
			{
				// Get a new bitmap object for this node.
				pNodeBitmap->GetBitmapRef()->SetBitmap(BitmapToUse);
						
				ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");
		
				DocRect BitmapSize(VirtualSize);
				BitmapInfo BMInfo;
				BitmapToUse->ActualBitmap->GetInfo(&BMInfo);
				if (BMInfo.PixelWidth==0) BMInfo.PixelWidth=1;
				if (BMInfo.PixelHeight==0) BMInfo.PixelHeight=1;

				double BMAspect = ((double)(BMInfo.PixelWidth))/((double)(BMInfo.PixelHeight));
				double RedrawAspect = ((double)(ExtraInfo->dx))/((double)(ExtraInfo->dy));

				if (BMAspect>RedrawAspect)
				{
					// grey at top and bottom
					INT32 NewHeight = (INT32)( 0.5 + ((double)(VirtualSize.Width()))/BMAspect);
					if (NewHeight<1) NewHeight = 1;
					BitmapSize.loy = (VirtualSize.Height()-NewHeight)/2;
					BitmapSize.hiy = BitmapSize.loy + NewHeight;
				}
				else
				{
				 	// grey on left and right
					INT32 NewWidth = (INT32)( 0.5 + ((double)(VirtualSize.Height()))*BMAspect);
					if (NewWidth<1) NewWidth = 1;
					BitmapSize.lox = (VirtualSize.Width()-NewWidth)/2;
					BitmapSize.hix = BitmapSize.lox + NewWidth;
				}

				// And set this in our bitmap node
				pNodeBitmap->CreateShape(BitmapSize);
					
				pNodeBitmap->Render(pRender);
			}
		}

		pRender->RestoreContext();

		// Get rid of the render region
		DestroyGRenderRegion(pRender);
	}
	return TRUE;
}

/********************************************************************************************
>	MsgResult BfxDlg::Message( Msg* Message)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Message: The message 

	
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. 

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers. 

	Purpose:	This is the message handler for the BfxDlg
	 
********************************************************************************************/


MsgResult BfxDlg::Message( Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = ((DialogMsg*) Message); 
		
		BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE; 	// TRUE if we should commit the dialog values

		CDlgResID ThePage = Msg->PageID;

		if (ThePage == 0) switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				OpenPage=0;
				pOriginal=NULL;
				TalkToPage(0);
				FindBitmap();
				FixOriginalComboBox();
				SetDirty();
				break;
			
			case DIM_COMMIT:		// Want to commit and quit
				EndDialog = TRUE;
				CommitValues = TRUE; 
				ThePage = OpenPage;
				break;

			case DIM_SOFT_COMMIT:	// Want to commit
				CommitValues = TRUE; 
				ThePage = OpenPage;
				break; 

			case DIM_CANCEL:		// Want to quit
				ThePage = OpenPage;
				EndDialog = TRUE;
				break;

		}

		// These are common to all pages, and get passed on to the pages themselves
		switch (Msg->DlgMsg)
		{
			case DIM_SET_ACTIVE:
				OpenPage = Msg->PageID;
				TalkToPage(Msg->PageID);
				CheckOriginalBitmap();
				FixOriginalComboBox();
				SetDirty();
				break;

			case DIM_REDRAW :
				// This is where all the redrawing is done
				// Which control in the window is sending the redraw message (if there are many
				// grdraw controls you can tell which is which from the Gadget ID
				TalkToPage(Msg->PageID);
				switch (Msg->GadgetID)
				{
					// Draw the redraw_me control in here
					case _R(IDC_BFXDLG_ORIGREDRAW) :
					{
						RenderBitmap((ReDrawInfoType*) Msg->DlgMsgParam, pOriginal);
						break;
					}
					case _R(IDC_BFXDLG_ORIGREDRAW2) :
					{
						RenderBitmap((ReDrawInfoType*) Msg->DlgMsgParam, pDestination);
						break;
					}
				}
				break;
			
			case DIM_SELECTION_CHANGED:
				TalkToPage(Msg->PageID);
				switch (Msg->GadgetID)
				{
					// Draw the redraw_me control in here
					case _R(IDC_BFXDLG_ORIGCOMBO) :
					{
						ReadOriginalComboBox();
						break;
					}
				}
				break;											       

		}

		// Determine from what page the message originated
		if (ThePage) switch (ThePage)
		{
			case _R(IDD_BFXDLG_PG1):
				HandleFlipAndRotateMsg(Msg, &CommitValues, &EndDialog); 
				break;
			case _R(IDD_BFXDLG_PG2):
			    HandleResizeMsg(Msg, &CommitValues, &EndDialog); 
				break;
			case _R(IDD_BFXDLG_PG3):
				HandleBrightnessAndContrastMsg(Msg, &CommitValues, &EndDialog); 
				break;
			case _R(IDD_BFXDLG_PG4):
				HandleDitherMsg(Msg, &CommitValues, &EndDialog); 
				break;
			case _R(IDD_BFXDLG_PG5):
				HandleSpecialEffectsMsg(Msg, &CommitValues, &EndDialog); 
				break;
			case _R(IDD_BFXDLG_PG6):
				HandleAlchemistMsg(Msg, &CommitValues, &EndDialog); 
				break;
			default:
				ERROR3("Message from unknown tab dialog page"); 
		}

		// Commit values here
		if (CommitValues)
		{
			if (pOriginal)
			{
				if (pFX)
				{
					if (!Clean)
					{
						ClearDestination();
						String_64 ProcessMessage(_R(IDS_BFXDLG_PROCESSING));
						// Bodge to avoid progress bar on interactive B&C
						if (Msg->DlgMsg==DIM_COMMIT || Msg->DlgMsg==DIM_SOFT_COMMIT)
						{
							BeginSlowJob(100, FALSE, &ProcessMessage);
							ContinueSlowJob();
						}
						pFX->SetParameters(Param1, Param2, Param3, Matrix);
						pDestination = pFX->GetProcessedBitmap(pOriginal);
						if (Msg->DlgMsg==DIM_COMMIT || Msg->DlgMsg==DIM_SOFT_COMMIT)
						{
							ContinueSlowJob(99);
							EndSlowJob();
						}
						if (!pDestination) InformError(); else
						{
							DestinationChanged();
							Clean=TRUE;
						}
					}
				}
				else
				{
					TRACEUSER( "Alex", _T("Wierd - he didn't actually ask for an effect..."));
				}
			}
		}

		// End dialog here
		if (EndDialog)	// Dialog communication over 
		{	
			if (pOriginal && CommitValues)
			{
				OpenPage = 0;
				TalkToPage(0);

				BfxOpParam param;
				param.ppBitmap = &pDestination;
			
				OpDescriptor* pOpDesc=OpDescriptor::FindOpDescriptor("BfxOp");
				ERROR3IF_PF(pOpDesc==NULL,("Couldn't find OPTOKEN_OPBFXOP op descriptor"));
				pOpDesc->Invoke((OpParam*)&param);
				// NULLs destination on success
			}

			ClearDestination();

			Close(); 		   
//			TRACEUSER( "Alex", _T("Calling End()\n"));
			End(); 	 	// End of dialog 
	   	}
		//return (DLG_EAT_IF_HUNGRY(Msg));   // I return EAT_MSG unless the message needs to be sent to all dialogs 
	} else if (MESSAGE_IS_A(Message, SelChangingMsg))		// Selection changed - use new bitmap
	{
		if (OpenPage) TalkToPage(OpenPage);
		SelChangingMsg *Msg = (SelChangingMsg *) Message;
		switch ( Msg->State )
		{
			case SelChangingMsg::SelectionState::SELECTIONCHANGED:
			case SelChangingMsg::SelectionState::NODECHANGED:
//				pOriginal=NULL;
				FindBitmap();
				break;
		}
	} else if (MESSAGE_IS_A(Message, BitmapListChangedMsg))		// Selection changed - use new bitmap
	{
		if (OpenPage) TalkToPage(OpenPage);
		CheckOriginalBitmap();
		FixOriginalComboBox();
	} else if (MESSAGE_IS_A(Message, BfxMsg))
	{
		if (OpenPage) TalkToPage(OpenPage);
		pOriginal=((BfxMsg*)(Message))->pBitmap;
		CheckOriginalBitmap();
	}

	return DialogTabOp::Message(Message);
} 


// -----------------------------------------------------------------------------------------
// Message handlers for individual pages

/********************************************************************************************

>	void BfxDlg::HandleFlipAndRotateMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BfxDlg::Message

********************************************************************************************/


void BfxDlg::HandleFlipAndRotateMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)
{
	TalkToPage(_R(IDD_BFXDLG_PG1));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			SetLongGadgetValue(_R(IDC_BFXDLG_FLIPX), TRUE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_FLIPY), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_ROTATE90), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_ROTATE180), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_ROTATE270), FALSE); 
			break;

		case DIM_LFT_BN_CLICKED:
		case DIM_SELECTION_CHANGED:
			{
				switch (Msg->GadgetID)
				{
					case _R(IDC_BFXDLG_FLIPX):
					case _R(IDC_BFXDLG_FLIPY):
					case _R(IDC_BFXDLG_ROTATE90):
					case _R(IDC_BFXDLG_ROTATE180):
					case _R(IDC_BFXDLG_ROTATE270):
						SetDirty();
						break;
					default:
						break;
				}
			}
			break;

		case DIM_COMMIT:
		case DIM_SOFT_COMMIT:
			{
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_FLIPX))) SetEffect(CC_RUNTIME_CLASS(ABFXFlipX));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_FLIPY))) SetEffect(CC_RUNTIME_CLASS(ABFXFlipY));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_ROTATE90) )) SetEffect(CC_RUNTIME_CLASS(ABFXRotate90 ));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_ROTATE180))) SetEffect(CC_RUNTIME_CLASS(ABFXRotate180));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_ROTATE270))) SetEffect(CC_RUNTIME_CLASS(ABFXRotate270));
			}
			break;

	}
}

/********************************************************************************************

>	void BfxDlg::HandleResizeMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BfxDlg::Message

********************************************************************************************/


void BfxDlg::HandleResizeMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)
{
	static BOOL ReEntered = FALSE;
	BOOL RescanSize = FALSE;
	TalkToPage(_R(IDD_BFXDLG_PG2));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_SET_ACTIVE:
			ReEntered = FALSE; // Just in case we get out of sync
			break;

		case DIM_CREATE:
			FindBitmap();
			SetLongGadgetValue(_R(IDC_BFXDLG_INTERP), TRUE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_ASPECT), TRUE); 
			RescanSize=TRUE;
			break;

		case DIM_OUT_OF_SPACE: // BODGE!!
			if (Msg->GadgetID == _R(IDC_BFXDLG_ORIGREDRAW))
				RescanSize=TRUE;
			break;

		case DIM_SELECTION_CHANGED:
		case DIM_SLIDER_POS_SET:
		//case DIM_SLIDER_POS_CHANGING:
		case DIM_LFT_BN_CLICKED:
		case DIM_TEXT_CHANGED:	 		   // Text changed 
			SetDirty();
			if ((GetLongGadgetValue(_R(IDC_BFXDLG_ASPECT),0,1)!=0) && !ReEntered)
			{
				INT32 NewX = GetLongGadgetValue(_R(IDC_BFXDLG_NEWXSIZE),0,10000000);
				INT32 NewY = GetLongGadgetValue(_R(IDC_BFXDLG_NEWYSIZE),0,10000000);
				switch (Msg->GadgetID)
				{
					case _R(IDC_BFXDLG_NEWXSIZE):
						if (pOrigInfo->PixelWidth)
						{
							ReEntered = TRUE;
							SetLongGadgetValue(_R(IDC_BFXDLG_NEWYSIZE), (INT32)(0.5+ (
								((double)(pOrigInfo->PixelHeight))/((double)(pOrigInfo->PixelWidth))*((double)(NewX)) )));
							ReEntered = FALSE;
						}
						break;
				
					case _R(IDC_BFXDLG_NEWYSIZE):
						if (pOrigInfo->PixelHeight)
						{
							ReEntered = TRUE;
							SetLongGadgetValue(_R(IDC_BFXDLG_NEWXSIZE), (INT32)(0.5+ (
								((double)(pOrigInfo->PixelWidth))/((double)(pOrigInfo->PixelHeight))*((double)(NewY)) )));
							ReEntered = FALSE;
						}
						break;

					default:
						break;
				}
			}
			break;
		case DIM_COMMIT:
		case DIM_SOFT_COMMIT:
			{
				SetEffect(CC_RUNTIME_CLASS(ABFXResize));
				INT32 NewX = GetLongGadgetValue(_R(IDC_BFXDLG_NEWXSIZE),0,10000000);
				INT32 NewY = GetLongGadgetValue(_R(IDC_BFXDLG_NEWYSIZE),0,10000000);
				if (NewX<2) NewX = 2;
				if (NewY<2) NewY = 2;
				SetParameters(NewX, NewY, (INT32)(BOOL)(GetLongGadgetValue(_R(IDC_BFXDLG_INTERP),0,1)!=0));
				}
			break;
	}

	if (RescanSize && pOrigInfo)
	{
		ReEntered = TRUE;
		SetLongGadgetValue(_R(IDC_BFXDLG_ORIGXSIZE), pOrigInfo->PixelWidth);
		SetLongGadgetValue(_R(IDC_BFXDLG_NEWXSIZE),  pOrigInfo->PixelWidth);
		SetLongGadgetValue(_R(IDC_BFXDLG_ORIGYSIZE), pOrigInfo->PixelHeight);
		SetLongGadgetValue(_R(IDC_BFXDLG_NEWYSIZE),  pOrigInfo->PixelHeight);
		ReEntered = FALSE;
	}
}
/********************************************************************************************

>	void BfxDlg::HandleBrightnessAndContrastMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BfxDlg::Message

********************************************************************************************/


void BfxDlg::HandleBrightnessAndContrastMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)
{
	TalkToPage(_R(IDD_BFXDLG_PG3));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			SetGadgetRange(_R(IDC_BFXDLG_BRIGHTNESS), -127, 127, 1);
			SetGadgetBitmaps(_R(IDC_BFXDLG_BRIGHTNESS), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
			SetGadgetRange(_R(IDC_BFXDLG_CONTRAST), -127, 127, 1);
			SetGadgetBitmaps(_R(IDC_BFXDLG_CONTRAST), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
			SetGadgetRange(_R(IDC_BFXDLG_COLOUR), -127, 127, 1);
			SetGadgetBitmaps(_R(IDC_BFXDLG_COLOUR), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
			SetLongGadgetValue(_R(IDC_BFXDLG_BRIGHTNESS), 0); 
			SetLongGadgetValue(_R(IDC_BFXDLG_CONTRAST), 0); 
			SetLongGadgetValue(_R(IDC_BFXDLG_COLOUR), 0); 
			break;
//		case DIM_SLIDER_POS_SET:
//		case DIM_SLIDER_POS_IDLE:
		case DIM_LFT_BN_CLICKED:
		case DIM_SELECTION_CHANGED:
			{
				switch (Msg->GadgetID)
				{
					case _R(IDC_BFXDLG_BRIGHTNESS):
					case _R(IDC_BFXDLG_CONTRAST):
					case _R(IDC_BFXDLG_COLOUR):
						SetDirty();
						break;
					default:
						break;
				}
			}
			break;

		case DIM_SLIDER_POS_SET:
		case DIM_SLIDER_POS_IDLE:
		case DIM_COMMIT:
		case DIM_SOFT_COMMIT:
			{
				INT32 Brightness = -GetLongGadgetValue(_R(IDC_BFXDLG_BRIGHTNESS),-127,127);
				INT32 Contrast = -GetLongGadgetValue(_R(IDC_BFXDLG_CONTRAST),-127,127);
				INT32 Colour = -GetLongGadgetValue(_R(IDC_BFXDLG_COLOUR),-127,127);
				if (Msg->DlgMsg == DIM_SLIDER_POS_SET || Msg->DlgMsg == DIM_SLIDER_POS_IDLE)
				{
					if (InteractiveBCC)
					{					
						if ((!pFX) || (!pFX->IsKindOf(CC_RUNTIME_CLASS(ABFXBrightnessContrast))) ||
							(Brightness!=Param1) || (Contrast!=Param2) || (Colour!=Param3))
						{
							*pCommitValues = TRUE;
							*pEndDialog = FALSE;
							SetDirty(FALSE); // no redraw right now
						}
						else break;
					}
					else
					{
						SetDirty();
						break;
					}
				}
				SetEffect(CC_RUNTIME_CLASS(ABFXBrightnessContrast));
				SetParameters(Brightness, Contrast, Colour);
			}
			break;

	}
}
/********************************************************************************************

>	void BfxDlg::HandleDitherMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BfxDlg::Message

********************************************************************************************/


void BfxDlg::HandleDitherMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)
{
	TalkToPage(_R(IDD_BFXDLG_PG4));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			SetLongGadgetValue(_R(IDC_BFXDLG_BAYERMONO), TRUE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_HALFTONEMONO), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_DIFFUSIONMONO), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_BAYERCOLOUR), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_DIFFUSIONCOLOUR), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_POPULARITY), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_MAKEGREYSCALE), FALSE); 
			SetLongGadgetValue(_R(IDC_BFXDLG_OCTREECOLOUR), FALSE); 
			
//EnableGadget(_R(IDC_BFXDLG_DIFFUSIONCOLOUR), FALSE); // Accusoft library crashes
//EnableGadget(_R(IDC_BFXDLG_OCTREECOLOUR), FALSE); // Accusoft library crashes
			
			break;

		case DIM_LFT_BN_CLICKED:
		case DIM_SELECTION_CHANGED:
			{
				switch (Msg->GadgetID)
				{
					case _R(IDC_BFXDLG_BAYERMONO):
					case _R(IDC_BFXDLG_HALFTONEMONO):
					case _R(IDC_BFXDLG_DIFFUSIONMONO):
					case _R(IDC_BFXDLG_BAYERCOLOUR):
					case _R(IDC_BFXDLG_DIFFUSIONCOLOUR):
					case _R(IDC_BFXDLG_POPULARITY):
					case _R(IDC_BFXDLG_MAKEGREYSCALE):
					case _R(IDC_BFXDLG_OCTREECOLOUR):
						SetDirty();
						break;
					default:
						break;
				}
			}
			break;

		case DIM_COMMIT:
		case DIM_SOFT_COMMIT:
			{
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_BAYERMONO)      )) SetEffect(CC_RUNTIME_CLASS(ABFXBayerMono));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_HALFTONEMONO)   )) SetEffect(CC_RUNTIME_CLASS(ABFXHalftoneMono));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_DIFFUSIONMONO)  )) SetEffect(CC_RUNTIME_CLASS(ABFXDiffusionMono));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_BAYERCOLOUR)    )) SetEffect(CC_RUNTIME_CLASS(ABFXBayerColour));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_DIFFUSIONCOLOUR))) SetEffect(CC_RUNTIME_CLASS(ABFXDiffusionColour));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_POPULARITY)     )) SetEffect(CC_RUNTIME_CLASS(ABFXPopularityColour));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_MAKEGREYSCALE)  )) SetEffect(CC_RUNTIME_CLASS(ABFXMakeGreyscale));
				if (GetBoolGadgetSelected(_R(IDC_BFXDLG_OCTREECOLOUR)      )) SetEffect(CC_RUNTIME_CLASS(ABFXOctree));
			}
			break;

	}
}

/********************************************************************************************

>	void BfxDlg::HandleSpecialEffectsMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BfxDlg::Message

********************************************************************************************/


void BfxDlg::HandleSpecialEffectsMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)
{
	TalkToPage(_R(IDD_BFXDLG_PG5));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			{
				SetLongGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO), 0); 
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM1), (INT32)(Matrix[0]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM2), (INT32)(Matrix[1]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM3), (INT32)(Matrix[2]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM4), (INT32)(Matrix[3]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM5), (INT32)(Matrix[4]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM6), (INT32)(Matrix[5]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM7), (INT32)(Matrix[6]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM8), (INT32)(Matrix[7]+0.5));
				SetLongGadgetValue(_R(IDC_BFXDLG_CUSTOM9), (INT32)(Matrix[8]+0.5));
				SetGadgetRange(_R(IDC_BFXDLG_AMOUNT), 0, 127, 1);
				SetGadgetBitmaps(_R(IDC_BFXDLG_AMOUNT), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT), 64); 

				DeleteAllValues(_R(IDC_BFXDLG_EFFECTCOMBO));

				String_256 Str0(_R(IDS_BFXDLG_EFFECTMENU0));
				String_256 Str1(_R(IDS_BFXDLG_EFFECTMENU1));
				String_256 Str2(_R(IDS_BFXDLG_EFFECTMENU2));
				String_256 Str3(_R(IDS_BFXDLG_EFFECTMENU3));
				String_256 Str4(_R(IDS_BFXDLG_EFFECTMENU4));
				String_256 Str5(_R(IDS_BFXDLG_EFFECTMENU5));
				String_256 Str6(_R(IDS_BFXDLG_EFFECTMENU6));
				String_256 Str7(_R(IDS_BFXDLG_EFFECTMENU7));
				String_256 Str8(_R(IDS_BFXDLG_EFFECTMENU8));
				String_256 Str9(_R(IDS_BFXDLG_EFFECTMENU9));
				String_256 Str10(_R(IDS_BFXDLG_EFFECTMENU10));
				String_256 Str11(_R(IDS_BFXDLG_EFFECTMENU11));
				String_256 Str12(_R(IDS_BFXDLG_EFFECTMENU12));
				String_256 Str13(_R(IDS_BFXDLG_EFFECTMENU13));
				String_256 Str14(_R(IDS_BFXDLG_EFFECTMENU14));
				String_256 Str15(_R(IDS_BFXDLG_EFFECTMENU15));
				String_256 Str16(_R(IDS_BFXDLG_EFFECTMENU16));
				String_256 Str17(_R(IDS_BFXDLG_EFFECTMENU17));
				String_256 Str18(_R(IDS_BFXDLG_EFFECTMENU18));
				String_256 Str19(_R(IDS_BFXDLG_EFFECTMENU19));
		  	
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str0,FALSE, 0);
			  	SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str1,FALSE, 1);
			  	SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str2,FALSE, 2);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str3,FALSE, 3);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str4,FALSE, 4);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str5,FALSE, 5);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str6,FALSE, 6);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str7,FALSE, 7);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str8,FALSE, 8);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str9,FALSE, 9);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str10,FALSE, 10);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str11,FALSE, 11);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str12,FALSE, 12);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str13,FALSE, 13);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str14,FALSE, 14);
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str15,FALSE, 15);
				if (IsUserName("Alex"))
				{
					SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str16,FALSE, 16);
					SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str17,FALSE, 17);
					SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str18,FALSE, 18);
					SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str19,FALSE, 19);
				}			
				
				
				SetStringGadgetValue(_R(IDC_BFXDLG_EFFECTCOMBO),&Str11, FALSE, -1);
				SetComboListLength(_R(IDC_BFXDLG_EFFECTCOMBO));
				SetSelectedValueIndex(_R(IDC_BFXDLG_EFFECTCOMBO), 11);
			}
			//break; RUN THROUGH

		case DIM_SLIDER_POS_SET:
		//case DIM_SLIDER_POS_CHANGING:
		case DIM_LFT_BN_CLICKED:
		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:	 		   // Text changed 
			{
				SetDirty();
				INT32 Choice = GetSelectedValueIndex(_R(IDC_BFXDLG_EFFECTCOMBO));
				BOOL GreyAmount = (Choice <= 9);
				BOOL GreyCustom = (Choice != 0);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM1), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM2), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM3), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM4), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM5), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM6), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM7), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM8), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_CUSTOM9), !GreyCustom);
				EnableGadget(_R(IDC_BFXDLG_AMOUNT), !GreyAmount);
				EnableGadget(_R(IDC_BFXDLG_AMOUNTTXT), !GreyAmount);
			}
			break;

		case DIM_COMMIT:
		case DIM_SOFT_COMMIT:
			{
				INT32 Choice = GetSelectedValueIndex(_R(IDC_BFXDLG_EFFECTCOMBO));
				Matrix[0]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM1),-255.0,255.0);
				Matrix[1]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM2),-255.0,255.0);
				Matrix[2]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM3),-255.0,255.0);
				Matrix[3]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM4),-255.0,255.0);
				Matrix[4]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM5),-255.0,255.0);
				Matrix[5]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM6),-255.0,255.0);
				Matrix[6]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM7),-255.0,255.0);
				Matrix[7]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM8),-255.0,255.0);
				Matrix[8]=GetDoubleGadgetValue(_R(IDC_BFXDLG_CUSTOM9),-255.0,255.0);

				switch (Choice)
				{
					case 0:
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
						SetEffect(CC_RUNTIME_CLASS(ABFXSpecialEffect));
						SetParameters((INT32) Choice);
						break;
					case 10:
						SetEffect(CC_RUNTIME_CLASS(ABFXSharpenBlur));
						SetParameters(((INT32) 127-GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127)), 1);
						break;
					case 11:
						SetEffect(CC_RUNTIME_CLASS(ABFXSharpenBlur));
						SetParameters(((INT32) 127-GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127)), 2);
						break;
					case 12:
						SetEffect(CC_RUNTIME_CLASS(ABFXSharpenBlur));
						SetParameters(((INT32) 127-GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127)), 3);
						break;
					case 13:
						SetEffect(CC_RUNTIME_CLASS(ABFXSharpenBlur));
						SetParameters(((INT32) GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127))-127, 1);
						break;
					case 14:
						SetEffect(CC_RUNTIME_CLASS(ABFXSharpenBlur));
						SetParameters(((INT32) GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127))-127, 2);
						break;
					case 15:
						SetEffect(CC_RUNTIME_CLASS(ABFXSharpenBlur));
						SetParameters(((INT32) GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127))-127, 3);
						break;
					case 16:
					case 17:
					case 18:
					case 19:
						SetEffect(CC_RUNTIME_CLASS(ABFXRemoveDither));
						SetParameters((127-((INT32) GetLongGadgetValue(_R(IDC_BFXDLG_AMOUNT),0,127)))<<1, 30, Choice-16);
						break;
				}											
			}

	}
}

/********************************************************************************************

>	void BfxDlg::HandleSpecialEffectsMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Msg: The message sent from page 1 of the dialog
	Outputs:	-
	Returns:	-
	Purpose:	All messages generated from the tabbed dialog's page 1 get processed here
	Errors:		-
	Scope:		private
	SeeAlso:	BfxDlg::Message

********************************************************************************************/


void BfxDlg::HandleAlchemistMsg(DialogMsg* Msg, BOOL *pCommitValues, BOOL *pEndDialog)
{
	TalkToPage(_R(IDD_BFXDLG_PG6));  // The Coords Page identifier
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:
			{
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_HUE_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_SATURATION_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_VALUE_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_RAINBOW_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_VARIATION_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_SHAPES_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_STYLE_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_OPACITY_CHECK), TRUE); 
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_GRAIN_CHECK), TRUE); 

				SetGadgetRange    (_R(IDC_BFXDLG_ALC_HUE_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_HUE_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_HUE_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_SATURATION_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_SATURATION_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_SATURATION_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_VALUE_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_VALUE_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_VALUE_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_RAINBOW_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_RAINBOW_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_RAINBOW_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_VARIATION_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_VARIATION_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_VARIATION_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_SHAPES_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_SHAPES_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_SHAPES_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_STYLE_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_STYLE_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_STYLE_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_OPACITY_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_OPACITY_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_OPACITY_SLIDER), 64); 
				SetGadgetRange    (_R(IDC_BFXDLG_ALC_GRAIN_SLIDER), 0, 127, 1);
				SetGadgetBitmaps  (_R(IDC_BFXDLG_ALC_GRAIN_SLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
				SetLongGadgetValue(_R(IDC_BFXDLG_ALC_GRAIN_SLIDER), 64); 

			}
			break; //RUN THROUGH

		case DIM_SLIDER_POS_SET:
		//case DIM_SLIDER_POS_CHANGING:
		case DIM_LFT_BN_CLICKED:
		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:	 		   // Text changed 
			{
				SetDirty();
			}
			break;

		case DIM_COMMIT:
		case DIM_SOFT_COMMIT:
			{
			}
			break;

	}
}


/********************************************************************************************

>	OpState	BfxDlg::GetState(String_256*, OpDescriptor*)


	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The standard GetState fn, does nothing
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


OpState	BfxDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}
		 
/********************************************************************************************

>	BOOL BfxDlg::Init()


	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The standard Init fn for the op. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL BfxDlg::Init()
{  
	GetApplication()->DeclareSection( "Bitmap Effects", 1);
	GetApplication()->DeclarePref( "Bitmap Effects", "Interactive Brightness & Contrast", &InteractiveBCC, FALSE, TRUE);
	
	return (RegisterOpDescriptor(
 								0,						   // Tool ID                    
 								_R(IDS_BFXDLGM), 			   // String resouirce ID        
								CC_RUNTIME_CLASS(BfxDlg),  // Runtime class              
 								OPTOKEN_BFXDLG,			   // Token string               
 								BfxDlg::GetState,		   // GetState function          
 								0,	/* help ID */		   // Help ID                    
 								_R(IDBBL_BITMAPEFFECTS),//_R(IDBBL_BFXDLG),   // Bubble ID                  
 								_R(IDD_BARCONTROLSTORE),	   // Resource ID                
 								_R(IDC_BITMAPEFFECTS),		   // Control ID                 
								SYSTEMBAR_UTILITIES,	   // Bar ID                     
								TRUE,					   // Recieve system messages    
								FALSE,					   // Smart duplicate operation  
								TRUE,					   // Clean operation            
								0,						   // No vertical counterpart
								_R(IDS_BFXDLG_ONE),		       // String for one copy only
								GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE
								)
								
		&& BfxOp::Init()); // HORRIBLE! But means I don't have to get a lock on main.cpp
} 

/********************************************************************************************

>	static void BfxMsg::OpenOrUse(KernelBitmap * pBitmap)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		pBitmap = bitmap to use
	Outputs:	-
	Returns:	-
	Purpose:	Open the bitmap effects dialog if it's not already open, else just uses that bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BfxMsg::OpenOrUse(KernelBitmap * pBitmap)
{
	// Obtain a pointer to the op descriptor for the create operation 
	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(BfxDlg));
	if (!OpDesc) return;

	// Only one live instance of the operation is allowed. It's probably a dialog 
	if (!MessageHandler::MessageHandlerExists(CC_RUNTIME_CLASS(BfxDlg)))
	{
		OpDesc->Invoke(&OpParam((INT32)pBitmap,(INT32)NULL));		 
	}
	else
	{
		BROADCAST_TO_CLASS(BfxMsg(pBitmap),DialogOp);	
	}	
	return;
}

/********************************************************************************************

>	void BfxDlg::Do(OpDescriptor*)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Like all Do functions invokes the BfxDlg
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BfxDlg::Do(OpDescriptor*)
{
	String_256 Title(_R(IDS_BFXDLG));
	SetName(&Title); 
	BOOL ok = Create(); // Create the TAB dialog please
	if (!ok)
	{
		/*InformError(blobby)*/;	// Display an error if this is a real dialog please
		OpenPage=0;
		End(); 
	}
	TalkToPage(0);
	SetStringGadgetValue(_R(ID_APPLY_NOW), _R(IDS_BFXDLG_TEST));
	SetStringGadgetValue(IDOK, _R(IDS_BFXDLG_INSERT));
	SetStringGadgetValue(IDCANCEL, _R(IDS_BFXDLG_CLOSE));
}

/********************************************************************************************

>	void BfxDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BfxOp for the first time
	Errors:		-
	SeeAlso:	-

The first parameter should be a kernel bitmap to use or null to just bring the dialog up

********************************************************************************************/

void BfxDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	Do(pOp);
	KernelBitmap * pBitmap = NULL;
	if (pParam) pBitmap = (KernelBitmap *)(void *)(pParam->Param1);
	if (pBitmap && pBitmap->IsKindOf(CC_RUNTIME_CLASS(KernelBitmap)))
	{
		pOriginal=pBitmap;
		TalkToPage(OpenPage);
		OriginalChanged();
	}
	return;
}


/********************************************************************************************

>	virtual BOOL BfxDlg::RegisterYourPagesInOrderPlease()


	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		-
	Outputs:	-
	Returns:	return TRUE if all calls to AddAPage returned TRUE
	Purpose:	This virtual function will get called to give you a chance to add pages to your
				DialogTabOp. This function will usually make multiple calls to AddAPage to 
				register the initial set of pages to be contained in the tab dialog. The pages
				should all be the same size and be registered in the left to right order in which
				you wish them to appear. 

				This function get's called directly from the Create method, Don't call it 
				explicitly yourself. If you return FALSE from this method then the Create 
				method will fail. 

	Errors:		-
	SeeAlso:	DialogTabOp::AddPage
	SeeAlso:	DialogOp::Create

********************************************************************************************/


BOOL BfxDlg::RegisterYourPagesInOrderPlease()
{
	BOOL ok = AddAPage(_R(IDD_BFXDLG_PG1)) &&	
			  AddAPage(_R(IDD_BFXDLG_PG2)) &&
			  AddAPage(_R(IDD_BFXDLG_PG3)) &&
			  AddAPage(_R(IDD_BFXDLG_PG4)) &&
			  AddAPage(_R(IDD_BFXDLG_PG5)) &&
#ifdef _DEBUG
			  ((!IsUserName("Alex")) || AddAPage(_R(IDD_BFXDLG_PG6))) &&
#endif
			  TRUE;
	return (ok); 
}
 
   

/********************************************************************************************

>	BfxOp::BfxOp() 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	BfxOp constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BfxOp::BfxOp(): UndoableOperation()  
{                                
}
   
/********************************************************************************************

>	BOOL BfxOp::Init()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	BlobyOp initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL BfxOp::Init()
{  
	return (UndoableOperation::RegisterOpDescriptor(
				 							0,
				 							_R(IDS_BFXOP),
											CC_RUNTIME_CLASS(BfxOp),
				 							OPTOKEN_BFXOP,
				 							BfxOp::GetState,
				 							0,	/* help ID */
				 							0,  /* bubble help ID */
				 							0	/* bitmap ID */)); 
}

/********************************************************************************************

>	OpState	BfxOp::GetState(String_256*, OpDescriptor*)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the BfxOp
	Purpose:	For finding the BfxOp's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	BfxOp::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}
		
/********************************************************************************************

>	void BfxOp::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BfxOp for the first time
	Errors:		-
	SeeAlso:	-

Blatantly copied from BitFilt::DoImportBitmap

********************************************************************************************/

void BfxOp::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{               
	DocCoord DropPos;
	Spread * pSpread;
	BOOL ok=FALSE;
	
	if (!GetWorkingDoc()) return;
	
	if (!FindCentreInsertionPosition(&pSpread, &DropPos))
	{
		InformError();
		FailAndExecute();
		End();
		return;
	}

	// Get a new NodeBitmap object to import into.
	NodeBitmap *pNodeBitmap = new NodeBitmap;
	if ((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12)))
	{
		InformError();		
		FailAndExecute();
		End();
		return;
	}

	KernelBitmap* pKB = *(((BfxOpParam *)(pParam))->ppBitmap);
	if (pKB == NULL)
	{
		// Bitmap is NULL, so the op must have failed
		delete pNodeBitmap;
		FailAndExecute();
		End();
		return;
	}
	
	// Put up a progress display/hourglass (FALSE => no delay, show it NOW!)
	BeginSlowJob(100, FALSE);
	*(((BfxOpParam *)(pParam))->ppBitmap) = NULL; // The bitmap is now the responsibility of this routine

	// The bitmap above is a temp bitmap, so we'll extract the OILBitmap (which will delete the KBmp),
	// and create a 'proper' kbmp to attach to the node
	OILBitmap* pOILBmp = pKB->ExtractOILBitmap();
	pKB = KernelBitmap::MakeKernelBitmap(pOILBmp, GetWorkingDoc());

	pNodeBitmap->GetBitmapRef()->Attach(pKB);

	if (pNodeBitmap->GetBitmap() != pKB)
	{
		// It didn't use the bitmap we gave it, so we can delete it
		delete pKB;
	}

	// First, set the rectangle to the right size for the bitmap...
	BitmapInfo Info;
	pNodeBitmap->GetBitmap()->ActualBitmap->GetInfo(&Info);

	DocRect BoundsRect;
	BoundsRect.lo.x = DropPos.x - (Info.RecommendedWidth/2);
	BoundsRect.lo.y = DropPos.y - (Info.RecommendedHeight/2);
	BoundsRect.hi.x = BoundsRect.lo.x + Info.RecommendedWidth;
	BoundsRect.hi.y = BoundsRect.lo.y + Info.RecommendedHeight;

	// And set this in our bitmap node
	pNodeBitmap->CreateShape(BoundsRect);

	// Apply some default attrs for the bitmap
	// This Must be done before the NodeBitmap is inserted into the tree
	if (!pNodeBitmap->ApplyDefaultBitmapAttrs(this))
	{
		goto EndOp;
	}
		
	// Insert the node
	if (!DoInsertNewNode(pNodeBitmap, pSpread, TRUE))
	{
		// It didn't work - delete the sub-tree we just created.
		delete pNodeBitmap;
		goto EndOp;
	}

	ok = TRUE;

EndOp:
	if (!ok)
	{
		FailAndExecute();
		InformError();
	}

	EndSlowJob();
	End();
}

/********************************************************************************************

>	BOOL BfxOp::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	Spread:  The spread to place the clipboard objects on
				Position:The centre of the view (Spread coords)
	Returns:	TRUE if succeeded else FALSE
	Purpose:	Finds the centre insertion position for clipboard objects
	Errors:		ERROR2 in various situations
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL BfxOp::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)
{
	// Let's chuck in a smattering of defensive programming
	ERROR3IF(Spread == NULL || Position == NULL, "Illegal NULL params");
	*Spread = NULL;
	*Position = DocCoord(0,0);	// A default value if we fail

	// ---------------------------------------------------------------------------------
	// Find out which spread is in the centre of the view 
	// this is the spread that the pasted objects will be placed on

	ERROR2IF((!Spread)||(!Position),FALSE,"Invalid call to FindCentreInsertionPoint()");
	// Obtain the current DocView
	DocView* CurDocView = GetWorkingDocView();

	ERROR2IF(!CurDocView, FALSE, "The current DocView is NULL"); 

	WorkRect WrkViewRect = CurDocView->GetViewRect();

	ERROR2IF((WrkViewRect.IsEmpty() || (!WrkViewRect.IsValid()) ), FALSE, "Invalid WorkRect");
	
	// Determine the centre of the view
	WorkCoord WrkCentreOfView; 
	WrkCentreOfView.x = WrkViewRect.lox	+ (WrkViewRect.Width()/2); 
	WrkCentreOfView.y = WrkViewRect.loy	+ (WrkViewRect.Height()/2);
	
	// FindEnclosing spread requires an OilCoord
	OilCoord OilCentreOfView = WrkCentreOfView.ToOil(CurDocView->GetScrollOffsets()); 

	// Find out which spread to insert the pasteboard objects onto
	(*Spread) = CurDocView->FindEnclosingSpread(OilCentreOfView);
	ERROR2IF(!(*Spread), FALSE, "No spread?!");

	// Phew
	// ---------------------------------------------------------------------------------
	// Now lets find the spread coordinate of the centre of the view
	DocRect DocViewRect = CurDocView->GetDocViewRect(*Spread);

	ERROR2IF( ( (DocViewRect.IsEmpty()) || !(DocViewRect.IsValid())), FALSE, "DocViewRect is invalid" );

	// Find the centre of the DocViewRect
   	DocCoord DocCentreOfView; 
	DocCentreOfView.x = DocViewRect.lox	+ (DocViewRect.Width()/2); 
	DocCentreOfView.y = DocViewRect.loy	+ (DocViewRect.Height()/2);

	// Now convert from DocCoords to spread coords
	(*Spread)->DocCoordToSpreadCoord(&DocCentreOfView);

	// Finally, fill in the return value
	*Position = DocCentreOfView;
	
	return TRUE;  
}


