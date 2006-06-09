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

#include "camtypes.h"
#include "bmpexprw.h"
#include "prvwmenu.h" // the context menu for the dialog
#include "ccdc.h"		// specific #includes needed for kernel-rendered dialogues
#include "dlgcol.h"		// DialogColourInfo
#include "grnddib.h"
//#include "exprwres.h"
#include "nodebmp.h"	// NodeBitmap
#include "dlgview.h"	// DialogView
#include "keypress.h"	// For Adjust key pressed test
#include "csrstack.h"
//#include "attr.h"		// for DM_EORPEN - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fileutil.h"
#include "bitfilt.h"	// the BaseBitmapFilter
//#include "nev.h"
//#include "tim.h"
#include "statline.h"	// for the status line
//#include "app.h"		// for Document - in camtypes.h [AUTOMATICALLY REMOVED]
#include "giffiltr.h"	// for TI_GIFFilter
#include "exjpeg.h"		// for JPEGExportFilter
#include "progress.h"
#include "sgliboil.h"
#include "backgrnd.h"
#include "bmpexdoc.h"
#include "prvwflt.h"
//#include "bmpreres.h"
#include "bmapprev.h"
//#include "accures.h"
//#include "oilfltrs.h"	// for find filter ext - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bitmpinf.h"	// for BitmapInfo

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(BitmapExportPreviewDialog, DialogOp)

#define new CAM_DEBUG_NEW

// define the fixed zoom factors (in percents) for click-zoom
const UINT32 MIN_ZOOM = 1;
const UINT32 MAX_ZOOM = 25601;
const UINT32 ZOOMS[] = {MIN_ZOOM,10,25,50,75,100,200,500,1000,2000,4000,8000,16000,MAX_ZOOM};
const INT32 NUM_ZOOMS = 14;

// Set the static vars of the render dialog
const CDlgMode BitmapExportPreviewDialog::Mode = MODELESS; //MODAL
const UINT32 BitmapExportPreviewDialog::IDD = _R(IDD_BITMAPPREVIEWDIALOG);

#define BUBBLE_TIME 800	

/*******************************************************************************************
********************************************************************************************/
// Initialise our statics

BitmapExportPreviewDialog * BitmapExportPreviewDialog::m_pBitmapExportPreviewDialog = NULL;


/*******************************************************************************************

>	BitmapExportPreviewDialog::BitmapExportPreviewDialog() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Purpose:	Constructs a Preview Dialog

*******************************************************************************************/

BitmapExportPreviewDialog::BitmapExportPreviewDialog(CWindowID ParentWnd)  
														: DialogOp(BitmapExportPreviewDialog::IDD, 
																	BitmapExportPreviewDialog::Mode, 
																	0,0,0,CC_RUNTIME_CLASS(DialogOp),-1,
																	ParentWnd) 
{
	m_pBitmapExportPreviewDialog = this;
	
	m_pRender = NULL;
	m_Width = 1;
	m_Height = 1;
	m_CurrentPos.x = 0;
	m_CurrentPos.y = 0;
	m_CurrentTool = PREVIEW_PUSH_TOOL;
	m_Dragging = FALSE;
	m_ZoomRectRender = FALSE;
	m_CurID = -1;
	m_ActiveBitmap = 0;

	m_pBubbleWnd = NULL;
	m_LastCursorOverControlID = 0;
	m_BubbleHelpPending = TRUE;
	m_LastAdjustState = FALSE;
	m_MouseOperationStarted = FALSE;
	m_PreviousWithFocus = NULL;
	m_pBmpFilter = NULL;
	m_pParentDlg = NULL;

	m_Scaling = 1.0;
	m_ZoomFactor = 100.0;
	m_bWantCursorRedrawn = FALSE;

	pOriginalBitmap[0] = NULL;
	pOriginalBitmap[1] = NULL;
	m_bNeedBitmapCopied[0] = TRUE;
	m_bNeedBitmapCopied[1] = TRUE;
}       



/*******************************************************************************************

>	BitmapExportPreviewDialog::~BitmapExportPreviewDialog()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Purpose:	Destructor

*******************************************************************************************/

BitmapExportPreviewDialog::~BitmapExportPreviewDialog()
{
	// clean out the render region, if allocated
	if (m_pRender)
	{
		// Delete the render region and its (dialog) view
		// Do it by hand as StopRender should have been called already
		delete m_pRender->GetRenderView();
		delete m_pRender;
//		DestroyGRenderRegion(m_pRender);
		m_pRender = NULL;
	}

	// check for the same bitmap
	if (BitmapData[0].m_pBitmap == BitmapData[1].m_pBitmap)
	{
		// set the first object's flag so the common data gets deleted
		BitmapData[0].m_bIsSameBitmap = FALSE;
	}

	// delete the cursor if one is still left
	DeleteCurrentCursor();
	
	// if there is a bubble help window from a previous call, delete it
	if (m_pBubbleWnd != NULL)
	{
		delete m_pBubbleWnd;
		m_pBubbleWnd = NULL;
	}

	// delete the background bitmap file, if one was created
	if (BitmapPreviewData::pPagePath != NULL)
	{
		FileUtil::DeleteFile(BitmapPreviewData::pPagePath);
		delete BitmapPreviewData::pPagePath;
		BitmapPreviewData::pPagePath = NULL;
	}

	// kill the static link to this dialog box
	m_pBitmapExportPreviewDialog = NULL;

	//  Get rid of the dynamically allocated KernelBitmap memory, if
	//  there has been any.
	if( pOriginalBitmap[0] )
	{
		delete pOriginalBitmap[0];
		pOriginalBitmap[0] = NULL; 
	}
	if( pOriginalBitmap[1] )
	{
		delete pOriginalBitmap[1];
		pOriginalBitmap[1] = NULL; 
	}

	// delete the non active export options but keep the active one
	// since the active one will be used outside this dlg
	// but the inactive one was stored by this dlg and used only here
	// See what is active by comparing it with the ptr to the one we
	// know we will use "BmapPrevDlg::m_pExportOptions"
	if (BitmapData[0].m_pOptions != BmapPrevDlg::m_pExportOptions && BitmapData[0].m_pOptions != NULL)
	{
		delete BitmapData[0].m_pOptions;
		BitmapData[0].m_pOptions = NULL;
	}
	if (BitmapData[1].m_pOptions != BmapPrevDlg::m_pExportOptions && BitmapData[1].m_pOptions != NULL)
	{
		delete BitmapData[1].m_pOptions;
		BitmapData[1].m_pOptions = NULL;
	}
} 


/***********************************************************************************************

> 	void FixZoomFactor (INT32 *pZoomFactor)

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/01/97
	Inputs:		pZoomFactor - a pointer to the zoom factor to be amended
    Purpose:    Searches the list of the pre-defined zoom factors for the one nearest to 
				the current one. If the difference between them is less then 2% the current zoom 
				factor is replaced by the pre-defined one. This is done to avoid things like 
				zooming in from 99% to 100%, and also fixes problems with rounding errors
				in the greater zoom factors.
	Returns:	None.
                 			                                     
***********************************************************************************************/

void FixZoomFactor (UINT32 *pZoomFactor)
{
	UINT32 ZoomFactor = *pZoomFactor;

	if (ZoomFactor < MIN_ZOOM)
		*pZoomFactor = MIN_ZOOM;
	else
	if (ZoomFactor > ZOOMS[NUM_ZOOMS - 1])
		*pZoomFactor = ZOOMS[NUM_ZOOMS - 1];
	else
	for (INT32 i=0; i<NUM_ZOOMS; i++)
	{
		// the calculation is done times 1000 decause of the integer arithmetic
		INT32 Error = 50 * ZoomFactor;
		if ((1000 * ZoomFactor - Error  < 1000 * ZOOMS[i]) &&
			(1000 * ZoomFactor + Error  > 1000 * ZOOMS[i]))
		{
			*pZoomFactor = ZOOMS[i];
			return;
		}
	}
}



/***********************************************************************************************

> 	double CPluginWindow::FindNearestZoom(BOOL Up)

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/01/97
	Inputs:		Up	-	Indicates whether we are looking for zoom factor greater or smaller then 
				the current one.
    Purpose:    Searches the list of the pre-defined zoom factors and finds the one nearest to 
				the current one, but above it (Up == TRUE), or below it (Up = FALSE). Current 
				zoom factor <= the smallest (10%) returns new zoom factor of 10%. Similarly 
				current zoom factor <= the greatest(25601%) returns 25601
	Returns:	One of the pre-defined zoom factors.
                 			                                     
***********************************************************************************************/

double BitmapExportPreviewDialog::FindNearestZoom(BOOL Up)
{
	UINT32 ZoomFactor;

	// get the zoom factor of the current bitmap
	if (m_ActiveBitmap == 1)
		ZoomFactor = (UINT32) m_ZoomFactor;
	else if (m_ActiveBitmap == 2)
		ZoomFactor = (UINT32) (m_ZoomFactor * m_Scaling);
	else
		return m_ZoomFactor;

	INT32 pos=NUM_ZOOMS - 1;

	// try to make it equal to one of the predefined zooms
	FixZoomFactor(&ZoomFactor);

	// find the position of the zoom factor in the list of predefined oness
	for (INT32 i=0;i<NUM_ZOOMS;i++)
		if (ZoomFactor <= ZOOMS[i])
		{
			// found a greater one, so remember the position
			pos=i;
			break;
		}

	if (Up)
	{
		// set it to the nearest one above it
		if ((ZoomFactor == ZOOMS[pos]) && (pos < NUM_ZOOMS - 1))
			ZoomFactor = ZOOMS[pos+1];
		else
			ZoomFactor = ZOOMS[pos];
	}
	else
	{
		// set it to the nearest one below it
		if (pos > 0)
			ZoomFactor = ZOOMS[pos-1];
		else
			ZoomFactor = ZOOMS[0];
	}

	// return a new value for the zoom factor, depending on the current bitmap
	if (m_ActiveBitmap == 2)
		return ZoomFactor / m_Scaling;
	else
		return ZoomFactor;
}


/*******************************************************************************************

>	void BitmapExportPreviewDialog::DeleteCurrentCursor()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	07/5/97
	Purpose:	Sets the cursor depending on the current tool mode.

*******************************************************************************************/

void BitmapExportPreviewDialog::DeleteCurrentCursor()
{
	// pop the cursor from the stack
	if (m_CurID != -1)
		delete CursorStack::GPop(m_CurID);
	m_CurID = -1;
}


/*******************************************************************************************

>	void BitmapExportPreviewDialog::SetCurrentCursor(UINT32 id)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	07/5/97
	Inputs:		id - the IDof the control which asked for the cursor change
	Purpose:	Sets the cursor depending on the current tool mode and the control

*******************************************************************************************/

void BitmapExportPreviewDialog::SetCurrentCursor(UINT32 id)
{
	// get he state of the Adjust(Shift) key	
	BOOL AdjustState = KeyPress::IsAdjustPressed();

	// if no change - return
	if ((m_CurID != -1) && (m_LastAdjustState == AdjustState))
	{
		//  Return, unless we want to have the cursor redrawn.
		if ( !m_bWantCursorRedrawn )
			return;
		else
			m_bWantCursorRedrawn = FALSE;
	}

	// remember the Adjust state
	m_LastAdjustState = AdjustState;

	// delete the old cursor
	DeleteCurrentCursor();

	Cursor *pCursor = NULL;

	// change the active bitmap if necessary
	if (((id == _R(IDC_REDRAW1)) && (m_ActiveBitmap != 1)) ||
		((id == _R(IDC_REDRAW2)) && (m_ActiveBitmap != 2)))
	{
		// over the unselected control
		pCursor = new Cursor(_R(IDC_ARROW));
	}
	else
	{
		// set the proper cursor
		if (m_CurrentTool == PREVIEW_ZOOM_TOOL)
		{
			if (AdjustState)
				pCursor = new Cursor(_R(IDC_ZOOMOUT));
			else
				pCursor = new Cursor(_R(IDC_ZOOMIN));
		}
		else if (m_CurrentTool == PREVIEW_PUSH_TOOL)
		{
			pCursor = new Cursor(_R(IDC_PICKHAND));
		}									
		else if( m_CurrentTool == PREVIEW_COLOUR_SELECTOR_TOOL )
		{
			//  Set the cursor for the colour selector. 
			pCursor = new Cursor( _R(IDC_COLOURSELECTOR) );
		}
		else
			pCursor = new Cursor( _R(IDC_COLOURSELECTOR) );
	}
	
	// push (and display) the cursor
	m_CurID = CursorStack::GPush(pCursor);
}




/*******************************************************************************************

>	void BitmapExportPreviewDialog::ResetInfo(UINT32 id)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	07/5/97
	Inputs:		id - the ID of the control the mouse is over
	Purpose:	Resets the bubble help system and displays status text for the control the 
				mouse is over. Called on a mouse operation.

*******************************************************************************************/

void BitmapExportPreviewDialog::ResetInfo(UINT32 id)
{
	// if we've moved since the last time
	if (id != m_LastCursorOverControlID)
	{
		// start the timer for the bubble help
		BubbleTimer.Sample();
		
		// remember the control the mouse is over
		m_LastCursorOverControlID = id;

		// delete the bubble help window
		delete m_pBubbleWnd;
		m_pBubbleWnd = NULL;

		m_BubbleHelpPending = TRUE;

		// now do the status line messages
		
		UINT32 StatusTextID = 0;		
		// find the status text ID from the Gadget ID
		switch (id)
		{
			case _R(IDC_ZOOM_TOOL):
					StatusTextID = _R(IDS_ZOOM_TOOL_STATUS_TEXT);
					break;
			
			case _R(IDC_PUSH_TOOL):
					StatusTextID = _R(IDS_PUSH_TOOL_STATUS_TEXT);
					break;
			
			case _R(IDC_ZOOM_TO_FIT):
					StatusTextID = _R(IDS_ZOOMTOFIT_STATUS_TEXT);
					break;

			case _R(IDC_100PERCENT):
					StatusTextID = _R(IDS_ZOOMTO100_STATUS_TEXT);
					break;

			case _R(IDC_1TO1):
					StatusTextID = _R(IDS_1TO1_STATUS_TEXT);
					break;

			case _R(IDC_COLOUR_SELECTOR):
					StatusTextID = _R(IDS_COLOUR_SELECTOR_STATUS_TEXT);
					break;

			case _R(IDC_REDRAW1):
			case _R(IDC_REDRAW2):
				{
					// over one of the draw controls

					if (((id == _R(IDC_REDRAW1)) && (m_ActiveBitmap != 1)) ||
						((id == _R(IDC_REDRAW2)) && (m_ActiveBitmap != 2)))
					{
						// over the unselected bitmap
						StatusTextID = _R(IDS_UNSELECTED_STATUS_TEXT);
					}
					else
					{
						// over the selected bitmap - display the current tool options
						if (m_CurrentTool == PREVIEW_ZOOM_TOOL)
							StatusTextID = _R(IDS_ZOOM_MODE_STATUS_TEXT);
						else if (m_CurrentTool == PREVIEW_PUSH_TOOL)
							StatusTextID = _R(IDS_PUSH_MODE_STATUS_TEXT);
						else if( m_CurrentTool == PREVIEW_COLOUR_SELECTOR_TOOL )
							StatusTextID = _R(IDS_COLOUR_SELECTOR_MODE_STATUS_TEXT);
					}
					break;
				}
		
			default:
				StatusTextID = _R(IDS_BITMAPPREVIEWDIALOG);
		}
		
		// put up some status line help
		StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
		if (pStatusLine != NULL)
			pStatusLine->UpdateText(&String_256(StatusTextID),FALSE);
	}
}




/********************************************************************************************

>	void BitmapExportPreviewDialog::DoBubbleHelp()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays bubble help (if any) for the control the mouse is over

********************************************************************************************/

void BitmapExportPreviewDialog::DoBubbleHelp()
{
	// if there is a bubble help window from a previous call, delete it
	if (m_pBubbleWnd != NULL)
	{
		delete m_pBubbleWnd;
		m_pBubbleWnd = NULL;
	}
	
	// check if we are over a control
	if (!m_LastCursorOverControlID)
		return;

	UINT32 BubbleID = 0;

	// find the bubble text ID from the Gadget ID
	switch (m_LastCursorOverControlID)
	{
		case _R(IDC_ZOOM_TOOL):
			BubbleID = _R(IDS_PREVIEW_ZOOM_TOOL);
			break;

		case _R(IDC_PUSH_TOOL):
				BubbleID = _R(IDS_PREVIEW_PUSH_TOOL);
				break;

		case _R(IDC_ZOOM_TO_FIT):
			BubbleID = _R(IDS_PREVIEW_ZOOM_TO_FIT);
			break;

		case _R(IDC_100PERCENT):
			BubbleID = _R(IDS_PREVIEW_ZOOM_TO_100);
			break;

		case _R(IDC_1TO1):
			BubbleID = _R(IDS_PREVIEW_1TO1);
			break;

		case _R(IDC_COLOUR_SELECTOR):
			BubbleID = _R(IDS_PREVIEW_COLOUR_SELECTOR);
			break;

		default:
			m_LastCursorOverControlID = 0;
	}

	// no bubble found
	if (BubbleID == 0)
		return;
	

	////try to load the text
	String_256 BubbleText(BubbleID);

	// Make a new bubble help window
	TRY
	{
		m_pBubbleWnd = new BubbleHelpWnd;
	}
	CATCH(CMemoryException, e)
	{
		TRACEALL( _T("Unable to create bubble help window!\n"));
		return;
	}
	END_CATCH

	// Create the actual window
	if (!m_pBubbleWnd->Create())
	{
		TRACEALL( _T("Could not Init bubble help window\n"));
		return;
	}

	// set the text
	if (!m_pBubbleWnd->SetText(BubbleText))
		return;  // no bubble help for this item

	// display the window
	m_pBubbleWnd->Show();

}


/*******************************************************************************************

>	MsgResult BitmapExportPreviewDialog::Message(Msg* Message)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Purpose:	Handles all the Preview dialog's messages 

*******************************************************************************************/
MsgResult BitmapExportPreviewDialog::Message(Msg* Message)
{
	// See if it is for us
	if (IS_OUR_DIALOG_MSG(Message))
	{
		// it is
		DialogMsg* Msg = (DialogMsg*)Message;

		// Main handler for dialog type messages
		BOOL EndDialog = FALSE;						// TRUE if we should quit the dialog

		// decide what to do
		switch (Msg->DlgMsg)
		{
			// Create message
			case DIM_CREATE:
			{
				// Force the arrow cursor on to the top of the cursor stack
				m_TopLevelCursor = CursorStack::GPush(Cursor::Arrow);

				// As it is a modal dialog box we are sent a message to say the dialog box
				// is being created, so we have a chance to set the initial control values.
				if (Mode == MODAL)
					InitDialog();

				// set up the timer
				SetTimer(1000,100);

				break;
			}

			// Close and destroy the dialog 
			case DIM_COMMIT:
			case DIM_CANCEL:
			{
				// remove the allocated cursor
				DeleteCurrentCursor();

				CursorStack::GPop(m_TopLevelCursor);

				KillTimer(1000);

				// delete the unused disk file
				if ((m_ActiveBitmap == 1) || (m_ActiveBitmap == 2))
				{
					// delete the temp file for the non-active bitmap
					if (BitmapData[2-m_ActiveBitmap].m_bIsSameBitmap != TRUE) // not same bitmap
					{
						// delete the file
						FileUtil::DeleteFile(&(BitmapData[2-m_ActiveBitmap].m_pOptions->GetPathName()));
					}

					// delete the temp file for the active bitmap, but only if Cancel was chosen
					if (Msg->DlgMsg == DIM_CANCEL)
						FileUtil::DeleteFile(&(BitmapData[m_ActiveBitmap-1].m_pOptions->GetPathName()));
				}

				// if there is a bubble help window from a previous call, delete it
				if (m_pBubbleWnd != NULL)
				{
					delete m_pBubbleWnd;
					m_pBubbleWnd = NULL;
				}

				EndDialog = TRUE;
				//Close();
				//End();
				break;
			}

			case DIM_TIMER:
			{
				// check for bubble help pending
				if (m_BubbleHelpPending)
					if (BubbleTimer.Elapsed(BUBBLE_TIME))
					{
						m_BubbleHelpPending = FALSE;
						DoBubbleHelp();
					}
				break;
			}

			case DIM_LFT_BN_DOWN :
			{
				switch (Msg->GadgetID)
				{
					case _R(IDC_REDRAW1) :
					case _R(IDC_REDRAW2) :
					{
						// get the extra info
						ReDrawInfoType*param = (ReDrawInfoType*) Msg->DlgMsgParam;
						
						// sanity check
						ERROR3IF((param == NULL)  || (param->pMousePos == NULL), "Invalid mouse position passed");
						
						// remember the current pos
						m_CurrentPos = *(param->pMousePos);

						// initialize for zoom to rect
						m_StartPos = m_CurrentPos;

						// indicate start of mouse operation
						m_MouseOperationStarted = TRUE;

						//  If we are doing image slicing, then return now
						if( BmapPrevDlg::m_bSlicingImage )
							break;

						// If the 'Colour Selector' is selected, the file format supports a palette
						// and the message is for the active window more things have to be done.
						if(	m_CurrentTool == PREVIEW_COLOUR_SELECTOR_TOOL &&
							m_pParentDlg->m_pExportOptions->GetSupportsPalette() &&
							(
								((Msg->GadgetID==_R(IDC_REDRAW1)) && (m_ActiveBitmap==1)) ||
								((Msg->GadgetID==_R(IDC_REDRAW2)) && (m_ActiveBitmap==2))
							)
						  )
						{
							//  The user has just clicked on a pixel in one of the preview images
							//  Need to get the palette inidex of the colour that this pixel uses.
							INT32 PaletteIndex = GetPaletteIndexOfColour( param, Msg->GadgetID );
							//  If -1 was returned, then something went wrong, so just leave.
							if( PaletteIndex == -1 )
								break;
							m_pParentDlg->SetPaletteSelection(PaletteIndex);
						}
						break;
					}
				}
				break;
			}


			case DIM_LFT_BN_UP :
			{
				switch (Msg->GadgetID)
				{
					case _R(IDC_REDRAW1) :
					case _R(IDC_REDRAW2) :
					{
						// if the press of the button was outside the control, or ESC was pressed
						if (!m_MouseOperationStarted)
						{						
							// delete the cursor so new one can be displayed
							DeleteCurrentCursor();

							break;
						}
						
						// end of the mouseop
						m_MouseOperationStarted = FALSE;

						// change the active bitmap if necessary
						if (((Msg->GadgetID == _R(IDC_REDRAW1)) && (m_ActiveBitmap != 1)) ||
							((Msg->GadgetID == _R(IDC_REDRAW2)) && (m_ActiveBitmap != 2)))
						{
							// change the selected bitmap
							ChangeActiveBitmap(1 + (m_ActiveBitmap == 1));
						}
						else
						{
							//perform zoom/pan

							// get the extra info
							ReDrawInfoType*param = (ReDrawInfoType*) Msg->DlgMsgParam;
							
							// sanity check
							ERROR3IF((param == NULL)  || (param->pMousePos == NULL), 
													"Invalid mouse position passed");
							
							// remember the current position
							m_CurrentPos = *(param->pMousePos);

							if (m_CurrentTool == PREVIEW_ZOOM_TOOL)
							{
								if (!m_Dragging) // click-zoom
									CalcViewRect(FindNearestZoom(!KeyPress::IsAdjustPressed()),
											param->pMousePos);
								else // zoom to rectangle
									ZoomToRect();
							}

							m_Dragging = FALSE;
						}
	
						// delete the cursor so new one can be displayed
						DeleteCurrentCursor();

						break;
					}

					// Any other controls on the 1st image, swap to that image
					case _R(IDC_ACTIVE1) :
//					case _R(IDC_ACTIVEFRAME1) :
					case _R(IDC_IMAGESIZE1) :
					case _R(IDC_FILESIZE1) :
					case _R(IDC_FILEDIFF1) :
					case _R(IDC_FILE_TYPE_LIST):
					{
						// change the active bitmap if necessary
						if (m_ActiveBitmap != 1)
						{
							// change the selected bitmap
							ChangeActiveBitmap(1 + (m_ActiveBitmap == 1));

							//  Don't want to respond to any mouse messages until this piece has
							//  been finished. 
//							m_pParentDlg->SetDontWantMouseMessage( TRUE );
//							BmapPrevDlg::m_bNeedPaletteUpdated = TRUE;
						}
						break;
					}

					// Any other controls on the second image, swap to that image
					case _R(IDC_ACTIVE2) :
//					case _R(IDC_ACTIVEFRAME2) :
					case _R(IDC_IMAGESIZE2) :
					case _R(IDC_FILESIZE2) :
					case _R(IDC_FILEDIFF2) :
					case _R(IDC_FILE_TYPE_LIST2):
					{
						// change the active bitmap if necessary
						if (m_ActiveBitmap != 2)
						{
							// change the selected bitmap
							ChangeActiveBitmap(1 + (m_ActiveBitmap == 1));

						}
						break;
					}
				}
				break;
			}

			case DIM_MOUSE_MOVE :
			{
				ResetInfo(Msg->GadgetID);
				
				if (Msg->GadgetID != _R(IDC_REDRAW1) && Msg->GadgetID != _R(IDC_REDRAW2))
				{
					DeleteCurrentCursor();					// pop the cursor from the stack
					ReleaseMouse(Msg->GadgetID);			// release the mouse capture
					if (m_PreviousWithFocus != NULL)		// restore the keyboard input focus to
						::SetFocus(m_PreviousWithFocus);	// the last window with the focus
					m_PreviousWithFocus = NULL; // reset it
				}
				else
				{
					// mouse move messages are handled only so that we can capture 
					// the mouse. This also allows us to set a cursor, because the	
					// cursor doesn't get displayed, when the window is not a top one, 
					// unless the mouse is captured. Thus by capturing the mouse
					// we can both check for the mouse pointer inside the control
					// and set the proper cursor.
					
					// get the extra info
					ReDrawInfoType*param = (ReDrawInfoType*) Msg->DlgMsgParam;
					
					// sanity check
					ERROR3IF((param == NULL)  || (param->pMousePos == NULL), 
						"Invalid mouse position passed");
					
					// if the mouse leaves the control, release the capture
					if ((param->pMousePos->x < 0) || (param->pMousePos->x > param->dx) ||
						(param->pMousePos->y < 0) || (param->pMousePos->y > param->dy))

					{
						// release the mouse capture
						ReleaseMouse(Msg->GadgetID);
						
						// restore the keyboard input focus to the last window with the focus
						if (m_PreviousWithFocus != NULL)
							::SetFocus(m_PreviousWithFocus);
						m_PreviousWithFocus = NULL; // reset it
						
						// pop the cursor from the stack
						DeleteCurrentCursor();
					}
					else
					{
						// get the hwnd of the control
						CWindowID hChild = ::GetDlgItem(GetReadWriteWindowID(), Msg->GadgetID);
						
						// set the keyboard input focus to our control
						HWND hWnd = ::SetFocus(hChild);
						
						// remember the last window with the focus
						if (m_PreviousWithFocus == NULL)
							m_PreviousWithFocus = hWnd; 
						
						//capture the mouse pointer
						CaptureMouse(Msg->GadgetID);
						
						// the mouse is moved inside the control, so update the cursor
						SetCurrentCursor(Msg->GadgetID);
						
						//	If we are doing image slicing, then return now
						if( BmapPrevDlg::m_bSlicingImage )
							break;
						
						//	If the 'Colour Selector' is selected, then some more things have to be done.
						if( m_CurrentTool == PREVIEW_COLOUR_SELECTOR_TOOL &&
							m_pParentDlg->m_pExportOptions->GetSupportsPalette() &&
							(
							((Msg->GadgetID==_R(IDC_REDRAW1)) && (m_ActiveBitmap==1)) ||
							((Msg->GadgetID==_R(IDC_REDRAW2)) && (m_ActiveBitmap==2))
							)
							)
						{
							//	The user has just clicked on a pixel in one of the preview images
							//	Need to get the palette index of the colour that this pixel uses.
							INT32 PaletteIndex = GetPaletteIndexOfColour( param, Msg->GadgetID );

							if( PaletteIndex != -1 )
								m_pParentDlg->SetPaletteHighlight(PaletteIndex);
						}
					}
				}
				break;
			}

			case DIM_MOUSE_DRAG :
			{
				ResetInfo(Msg->GadgetID);

				switch (Msg->GadgetID)
				{
					case _R(IDC_REDRAW1) :
					case _R(IDC_REDRAW2):
					{
						// check whether the mouse button was first pressed inside our window
						if (!m_MouseOperationStarted)
							break;

						// check for a drag in the unselected control
						if (((Msg->GadgetID == _R(IDC_REDRAW1)) && (m_ActiveBitmap != 1)) ||
							((Msg->GadgetID == _R(IDC_REDRAW2)) && (m_ActiveBitmap != 2)))
							break;

						if (KeyPress::IsEscapePressed())
						{
							// cancel the operation
							m_MouseOperationStarted = FALSE;
							
							// if we are zooming to a rectangle, hide the rectangle
							if (m_Dragging && (m_CurrentTool == PREVIEW_ZOOM_TOOL))
								DrawZoomRect();

							break;
						}

						// get the extra info
						ReDrawInfoType*param = (ReDrawInfoType*) Msg->DlgMsgParam;
						
						// sanity check
						ERROR3IF((param == NULL)  || (param->pMousePos == NULL), "Invalid mouse position passed");
						
						m_Dragging = TRUE;

						// put up some status line help
						StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
						if (pStatusLine != NULL)
						{
							UINT32 StatusTextID= 0;
							if (m_CurrentTool == PREVIEW_PUSH_TOOL)
								StatusTextID = _R(IDS_PUSH_DRAG_STATUS_TEXT);
							else if (m_CurrentTool == PREVIEW_ZOOM_TOOL)
								StatusTextID = _R(IDS_ZOOM_DRAG_STATUS_TEXT);

							// display the text
							if (StatusTextID != 0)
								pStatusLine->UpdateText(&String_256(StatusTextID),FALSE);
						}

						// perform the drag operation
						if (m_CurrentTool == PREVIEW_PUSH_TOOL)
							DoPush(param->pMousePos);
						else if (m_CurrentTool == PREVIEW_ZOOM_TOOL)
							DoZoom(param->pMousePos);
						break;
					}
				}
				break;
			}

		case DIM_RGT_BN_UP :
			{
				// See which button was pressed
				switch (Msg->GadgetID)
				{
					case _R(IDC_REDRAW1) :
					case _R(IDC_REDRAW2) :
					{
						// change the active bitmap if necessary
						if (((Msg->GadgetID == _R(IDC_REDRAW1)) && (m_ActiveBitmap != 1)) ||
							((Msg->GadgetID == _R(IDC_REDRAW2)) && (m_ActiveBitmap != 2)))
						{
							// change the selected bitmap
							ChangeActiveBitmap(1 + (m_ActiveBitmap == 1));
//							BmapPrevDlg::m_bNeedPaletteUpdated = TRUE;
						}
						else
						{
							// initialise the popup menu
							OpPreviewPopupCommand::Init();

							// create the right-click menu
							PreviewContextMenu *menu = new PreviewContextMenu;

							// display the menu
							if (menu != NULL)
								menu->Show();
							else
								ERROR3("Can't create PreviewContextMenu");
						}

						// delete the cursor, so new one can be set
						DeleteCurrentCursor();

						break;
					}
				}
				break;
			}

		
		case DIM_LFT_BN_CLICKED :
			{
				// See which button was pressed
				switch (Msg->GadgetID)
				{
					case _R(IDC_ZOOM_TOOL) :
					{
						OnZoomTool();
						break;
					}

					case _R(IDC_PUSH_TOOL) :
					{
						OnPushTool();
						break;
					}

					case _R(IDC_ZOOM_TO_FIT) :
					{
						// first check if there is any bitmap
						if (m_ActiveBitmap != 0)
							CalcViewRect(-1, NULL);
						break;
					}

					case _R(IDC_1TO1):
					{
						On1to1();
						break;
					}

					case _R(IDC_100PERCENT):
					{
						On100Percent();
						break;
					}

					case _R(IDC_COLOUR_SELECTOR):
					{
						OnColourSelectorTool();
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
					case _R(IDC_REDRAW1) :
					case _R(IDC_REDRAW2) :
					{
						RenderControl((ReDrawInfoType*) Msg->DlgMsgParam, Msg->GadgetID);
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

			//  The user made a selection from one of the file type drop-lists
			case DIM_SELECTION_CHANGED:
			{
				//  Make sure that the message came from one of the file type drop-lists
				//  before calling the handling function.
				switch( Msg -> GadgetID )
				{
				case _R(IDC_FILE_TYPE_LIST):	
					HandleFileTypeListChange( 0 );
					break;
				case _R(IDC_FILE_TYPE_LIST2):
					HandleFileTypeListChange( 1 );
					break;
				}
				break;
			}

		}

		// Allow the base class access to the message, it will do the
		// DLG_EAT_IF_HUNGRY(Msg) for us
		// Must do this before the Close and End
		MsgResult Result = DialogOp::Message(Message);

		// End dialog here
		if (EndDialog) 
		{
			Close();				// Hide the dialog box
			End();					// Finish the operation

			// Free up any unused bitmaps in the global list
			// (just deleting the KernelBitmaps doesn't seem to do it)
			Camelot.GetGlobalBitmapList()->DeleteAllUnusedBitmaps();
		}
		
		// Return
		// The message was for our dialog box so return that we have handled it, if necessary
		//return (DLG_EAT_IF_HUNGRY(Msg)); 
		return Result;
	}

	return DialogOp::Message(Message);
}  


/******************************************************************************************
>	INT32 BitmapExportPreviewDialog::GetPaletteIndexOfColour( ReDrawInfoType* param )
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/99
	Inputs:		param - Holds the mouse information needed
	Returns:	-1 if something went wrong, otherwise the relevant palette index.
	Purpose:	When the user clicks in, or moves the mouse over a pixel in one of the 
				preview images, this function is called to work out the actual palette 
				entry of the colour that the pixel uses.
******************************************************************************************/
INT32 BitmapExportPreviewDialog::GetPaletteIndexOfColour( ReDrawInfoType* param, CGadgetID Gadget )
{
	//  How big is a pixel in millipoints? - need this for conversion.
	INT32 pixelSize = 72000 / param->Dpi;

	//  Get the current mouse position.
	INT32 MouseX = param->pMousePos->x;
	INT32 MouseY = param->pMousePos->y;

	//  The values above are mouse positions in the preview window.
	//  To get the actual values in the bitmap view rectangle, we
	//  have to make an adjustment.
	//  ( Add 1 to avoid problems at the top and right-hand edges of the bitmap).
	MouseX -= m_BitmapView.lo.x + 1;
	MouseY -= m_BitmapView.lo.y + 1;

	//  We now have to scale these values from values in the bitmap
	//  view rectangle to values in the actual bitmap. The zoom
	//  factor and scaling factor contains the information we need for this.
	double ConversionFactor = 0.0;
	if( m_ActiveBitmap == 1 )
	{
		ConversionFactor = m_ZoomFactor;
	}
	else if( m_ActiveBitmap == 2 )
	{
		ConversionFactor = m_ZoomFactor * m_Scaling;
	}
	MouseX = ( INT32 )( MouseX * 100.0 / ConversionFactor );
	MouseY = ( INT32 )( MouseY * 100.0 / ConversionFactor );

	//  If the pointer is inside the bitmap preview control
	//  but outside the bitmap, ( can easily happen when
	//  'Zoom to fit' is clicked ), then we don't want
	//  to do anything else.
	if ( ( MouseX < 0) || ( MouseX >= m_Width) ||
		 ( MouseY < 0) || ( MouseY >= m_Height) )
	{
		return -1;
	}

	//  Convert our mouse position into pixels.
	MouseX /= pixelSize;
	MouseY /= pixelSize;

	//  We now have the position of the pixel in the bitmap
	//  that the user has put the mouse over.
	//  We now need to get the palette index of the 
	//  corresponding colour.

	KernelBitmap * pBitmapToUse;
	if ( Gadget == _R(IDC_REDRAW1) )
		pBitmapToUse = BitmapData[0].m_pBitmap;
	else if ( Gadget == _R(IDC_REDRAW2) )
		pBitmapToUse = BitmapData[1].m_pBitmap;
	else
		pBitmapToUse = NULL;

	//  If there is no bitmap being previewed, then finish here.
	if( pBitmapToUse == NULL )
		return -1;

	NodeBitmap NB;
	NodeBitmap * pNodeBitmap = &NB;

	pNodeBitmap->GetBitmapRef()->SetBitmap( pBitmapToUse );
	ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");

	KernelBitmap* pBit = pNodeBitmap->GetBitmap();
	
	//  Need to alter the mouse position to take account of any change in bitmap size 
	//  or dpi which is not transmitted to this class.
	BitmapInfo BmapInfo;
	pBit->ActualBitmap->GetInfo( &BmapInfo );
	INT32 Width	= BmapInfo.PixelWidth;
	INT32 Height	= BmapInfo.PixelHeight;

	INT32 NewWidth	= pBit->GetRecommendedWidth();
	INT32 NewHeight	= pBit->GetRecommendedHeight();
	NewWidth		/= pixelSize;
	NewHeight		/= pixelSize;

	MouseX = MouseX * ( Width ) / NewWidth;
	MouseY = MouseY * ( Height ) / NewHeight;

	// we don't want to be subtracting one from the values above anymore
	// (to allow the indexed colour to correspond to the eye dropper location)
	// however, this leads us to a few problems - since it is now possible to
	// access invalid regions within the bitmap.  SOLUTION:  if were at the extents
	// of the bitmap, then allow us to subtract 1 (we can't do much else)
	if (MouseX >= Width)	{ MouseX = Width-1; }
	if (MouseY >= Height)	{ MouseY = Height-1; }

	//  Get the palette from the BmapPrevDlg class.
	//  Don't need to do this every time the mouse moves.
	ExtendedPalette* pPal = BmapPrevDlg::m_pExportOptions->GetExtendedPalette();
	INT32 PaletteIndex = pBit->ReturnPaletteIndexUsed( MouseX, MouseY );

	return PaletteIndex;
}



/********************************************************************************************

  >	BOOL BitmapExportPreviewDialog::DoBrowserPreview(BrowserPreviewOptions BrowserOptions)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Inputs:		BrowserOptions - options to be used when generating the html page
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Browser previewing. Simply calls GenerateHTMLStub function in the bitmap data 
				object to do the work

********************************************************************************************/

BOOL BitmapExportPreviewDialog::DoBrowserPreview(BrowserPreviewOptions BrowserOptions)
{
	if ((m_ActiveBitmap == 1) || (m_ActiveBitmap == 2))
		return BitmapData[m_ActiveBitmap - 1].GenerateHTMLStub(BrowserOptions);
	else
		return FALSE;
}



/********************************************************************************************

>	void BitmapExportPreviewDialog::OnZoomTool()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Selects the zoom tool and displays the zoom tool button pressed (and the push 
				tool one) released.

********************************************************************************************/

void BitmapExportPreviewDialog::OnZoomTool()
{
	BOOL Valid = FALSE;
	BOOL State = GetLongGadgetValue(_R(IDC_ZOOM_TOOL), 0, 1, NULL, &Valid);
	if (!State)
	{
		SetLongGadgetValue(_R(IDC_ZOOM_TOOL), TRUE);
		SetLongGadgetValue(_R(IDC_PUSH_TOOL), FALSE);
		m_CurrentTool = PREVIEW_ZOOM_TOOL;

		//  Turn the colour selector tool off.
		SetLongGadgetValue( _R(IDC_COLOUR_SELECTOR), FALSE );
	}
}

/******************************************************************************************

>	void BitmapExportPreviewDialog::OnColourSelectorTool()

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	20/07/99

	Inputs:		-

	Returns:	-

	Purpose:	This function is called when the user clicks on the Colour Selector.

******************************************************************************************/


void BitmapExportPreviewDialog::OnColourSelectorTool()
{
	//  When the Colour Selector is clicked, make sure that the Zoom and Push
	//  tools are deselected.
	SetLongGadgetValue( _R(IDC_ZOOM_TOOL), FALSE );
	SetLongGadgetValue( _R(IDC_PUSH_TOOL), FALSE );
	//  Select the Colour Selector
	SetLongGadgetValue( _R(IDC_COLOUR_SELECTOR), TRUE );

	//  Set this as the current tool
	m_CurrentTool = PREVIEW_COLOUR_SELECTOR_TOOL;
}

/********************************************************************************************

>	void BitmapExportPreviewDialog::OnPushTool()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Selects the push tool and displays the push tool button pressed (and the zoom
				tool one) released.

********************************************************************************************/

void BitmapExportPreviewDialog::OnPushTool()
{
	BOOL Valid = FALSE;
	BOOL State = GetLongGadgetValue(_R(IDC_PUSH_TOOL), 0, 1, NULL, &Valid);
	if (!State)
	{
		SetLongGadgetValue(_R(IDC_PUSH_TOOL), TRUE);
		SetLongGadgetValue(_R(IDC_ZOOM_TOOL), FALSE);
		m_CurrentTool = PREVIEW_PUSH_TOOL;

		//  Turn the Colour Selector off
		SetLongGadgetValue( _R(IDC_COLOUR_SELECTOR), FALSE );
	}
}


/********************************************************************************************
>	void BitmapExportPreviewDialog::On1to1()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com> (from Stefan)
	Created:	14/12/2000
	Purpose:	Calculates and displays new view so one pixel in the final image is equal
				to one pixel in the preview
********************************************************************************************/
void BitmapExportPreviewDialog::On1to1()
{
	// get the control size
	ReDrawInfoType param;
	GetKernelRenderedGadgetInfo(_R(IDC_REDRAW1), &param);
	MILLIPOINT w = param.dx;
	MILLIPOINT h = param.dy;

	ERROR3IF(!BitmapData[0].m_pBitmap, "No bitmap");
	ERROR3IF(BitmapData[0].m_pBitmap->GetHorizontalDPI() != BitmapData[0].m_pBitmap->GetVerticalDPI(),
					"Different horizontal and vertiacl DPIs might cause problems");

	// calculate zoom factor so that the whole bitmap fits in 
	m_ZoomFactor = (BitmapData[0].m_pBitmap->GetHorizontalDPI() / double(param.Dpi)) * 100.0;
	if (m_ZoomFactor <= 0) 
		m_ZoomFactor = MIN_ZOOM;

	// scale the bitmap width and height
	MILLIPOINT Width  = (MILLIPOINT)((m_Width  * m_ZoomFactor) / 100);
	MILLIPOINT Height = (MILLIPOINT)((m_Height * m_ZoomFactor) / 100);

	m_BitmapView.lo.x = w/2 - Width / 2;
	m_BitmapView.hi.x = w/2 + Width / 2;
	m_BitmapView.lo.y = h/2 - Height / 2;
	m_BitmapView.hi.y = h/2 + Height / 2;

	INT32 PixelSize = 72000 / param.Dpi;

	// make sure one of the corners is aligned at a pixel

	// get the pixel size in millipoints
	MILLIPOINT PixSize = 0;
	DocUnitList* pDocUnitList =	DocUnitList::GetCurrentDocUnitList();
	ERROR3IF(pDocUnitList == NULL, "BmpPrefsDlg::InitDialog() - no pDocUnitList!")
	if (pDocUnitList != NULL)
	{
		Unit* pPixelUnit = pDocUnitList->FindUnit(PIXELS);
		ERROR3IF(pPixelUnit == NULL, "BmpPrefsDlg::InitDialog() - no pixel units!")
		if (pPixelUnit != NULL)
			PixSize = (MILLIPOINT) pPixelUnit->GetMillipoints();
	}

	// shift the rectangle, so that the lower left corner is aligned at a pixel boundary
	if (PixSize != NULL)
	{
		MILLIPOINT ShiftX = m_BitmapView.lo.x % PixSize;
		MILLIPOINT ShiftY = m_BitmapView.lo.y % PixSize;

		m_BitmapView.Translate(-ShiftX, -ShiftY);
	}


	// display the new zoom factor
	DisplayZoomFactor();
	
	// Render the controls
	InvalidateGadget(_R(IDC_REDRAW1));
	InvalidateGadget(_R(IDC_REDRAW2));
}

/********************************************************************************************
>	void BitmapExportPreviewDialog::On100Percent()
	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/2000
	Purpose:	Calculates and displays new view, so that the selected bitmap is visible at 100%
********************************************************************************************/
void BitmapExportPreviewDialog::On100Percent()
{
	if (m_ActiveBitmap == 1)
		CalcViewRect(100, NULL);
	else if (m_ActiveBitmap == 2)
		CalcViewRect(100 / m_Scaling, NULL);
}


/******************************************************************************************

>	BOOL BitmapExportPreviewDialog::InitDialog()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL BitmapExportPreviewDialog::InitDialog()
{
	// Set up the bitmaps for the play controls
	// Uses the title defined in the rc file so do not specify any bitmaps
	SetGadgetBitmaps(_R(IDC_ZOOM_TOOL), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_PUSH_TOOL), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_ZOOM_TO_FIT), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_1TO1), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_100PERCENT), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_COLOUR_SELECTOR), NULL, NULL );

	// default to push button
	SetLongGadgetValue(_R(IDC_PUSH_TOOL), TRUE);

	DisplayZoomFactor();

	//  Initialise the file type drop-list
	InitFileTypeList();

	return TRUE;
}


/******************************************************************************************

>	void BitmapExportPreviewDialog::InitFileTypeList()

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	06/10/99

	Inputs:		-

	Returns:	-

	Purpose:	This function is called to initialise the file type drop lists which enables
				the user to switch the preview image between different file types
				Also initialises the file type variable in BmapPrevDlg

******************************************************************************************/
void BitmapExportPreviewDialog::InitFileTypeList()
{
	// Make sure the list is empty
	DeleteAllValues( _R(IDC_FILE_TYPE_LIST) );

	//  Add the necessary strings to the list. 
	//  This is the complete collection of them.
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST), _R(IDS_FILE_TYPE_GIF), FALSE, 0 );
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST), _R(IDS_FILE_TYPE_PNG), FALSE, 1 );
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST), _R(IDS_FILE_TYPE_BMP), FALSE, 2 );
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST), _R(IDS_FILE_TYPE_JPG), FALSE, 3 );

	//  Repeat what is necessary for the second drop list.
	DeleteAllValues( _R(IDC_FILE_TYPE_LIST2) );

	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST2), _R(IDS_FILE_TYPE_GIF), FALSE, 0 );
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST2), _R(IDS_FILE_TYPE_PNG), FALSE, 1 );
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST2), _R(IDS_FILE_TYPE_BMP), FALSE, 2 );
	SetStringGadgetValue( _R(IDC_FILE_TYPE_LIST2), _R(IDS_FILE_TYPE_JPG), FALSE, 3 );

	switch(BmapPrevDlg::m_pExportOptions->GetFilterNameStrID())
	{
	case _R(IDN_FILTERNAME_GIF): // its a windows bitmap bmp type
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST), 0 );
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST2), 0 );
		break;
	default:
	case _R(IDS_FILTERNAME_PNG): // its a png
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST), 1 );
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST2), 1 );
		break;
	case _R(IDT_FILTERNAME_BMP): // its a windows bitmap bmp type
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST), 2 );
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST2), 2 );
		break;
	case _R(IDS_JPG_EXP_FILTERNAME): // its a jpeg type
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST), 3 );
		SetSelectedValueIndex( _R(IDC_FILE_TYPE_LIST2), 3 );
		break;
	}
	
	//  Set the lists to the correct length
	SetComboListLength( _R(IDC_FILE_TYPE_LIST) );
	SetComboListLength( _R(IDC_FILE_TYPE_LIST2) );

	EnableGadget( _R(IDC_FILE_TYPE_LIST),	!BmapPrevDlg::m_bIsCreateBitmap );
	EnableGadget( _R(IDC_FILE_TYPE_LIST2),	!BmapPrevDlg::m_bIsCreateBitmap );
}


/******************************************************************************************

>	void BitmapExportPreviewDialog::HandleFileTypeListChange( INT32 Index )

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	06/10/99

	Inputs:		An index telling us which of the 2 drop-lists being referred to.
				Equal to 0 for the first list, and 1 for the second.

	Returns:	-

	Purpose:	When the user makes a selection from the file type drop-lists, this function
				carries out any necessary changes required.

******************************************************************************************/

void BitmapExportPreviewDialog::HandleFileTypeListChange( INT32 Index )
{
	//  Get the user's selection
	String_16 FileTypeSelected;
	if( Index == 0 )
		FileTypeSelected = GetStringGadgetValue( _R(IDC_FILE_TYPE_LIST) );
	else if( Index == 1 )
		FileTypeSelected = GetStringGadgetValue( _R(IDC_FILE_TYPE_LIST2) );

	BitmapExportOptions * pNewOptions = NULL;

	//  Did the user select 'Gif'?
	if( FileTypeSelected.IsIdentical( String_16( _R(IDS_FILE_TYPE_GIF) ) ) )
	{
		//  Was 'Gif' previously selected?
		//  If so, there is nothing to do, so return now.
		if (BmapPrevDlg::m_pExportOptions->GetFilterNameStrID() == _R(IDN_FILTERNAME_GIF))
			return;
		pNewOptions = new GIFExportOptions();
	}
	//  Did the user select 'Png'?
	else if( FileTypeSelected.IsIdentical( String_16( _R(IDS_FILE_TYPE_PNG) ) ) )
	{
		//  Was 'Png' previously selected?
		//  If so, there is nothing to do, so return now.
		if (BmapPrevDlg::m_pExportOptions->GetFilterNameStrID() == _R(IDS_FILTERNAME_PNG))
			return;
		pNewOptions = new PNGExportOptions();
	}
	//  Did the user select 'Bmp'?
	else if( FileTypeSelected.IsIdentical( String_16( _R(IDS_FILE_TYPE_BMP) ) ) )
	{
		//  Was 'Bmp' previously selected?
		//  If so, there is nothing to do, so return now.
		if (BmapPrevDlg::m_pExportOptions->GetFilterNameStrID() == _R(IDT_FILTERNAME_BMP))
			return;
		pNewOptions = new BMPExportOptions();
	}
	//  Did the user select 'Jpg'?
	else if( FileTypeSelected.IsIdentical( String_16( _R(IDS_FILE_TYPE_JPG) ) ) )
	{
		//  Was 'Jpg' previously selected?
		//  If so, there is nothing to do, so return now.
		if (BmapPrevDlg::m_pExportOptions->GetFilterNameStrID() == _R(IDS_JPG_EXP_FILTERNAME))
			return;
		pNewOptions = new JPEGExportOptions();
	}
	
	// failled to create some new options
	if (!pNewOptions)
		return;

	// copy as much as we can from the old options
	pNewOptions->RetrieveDefaults();
	pNewOptions->CopyFrom(BitmapData[Index].m_pOptions);

	// Bodge fix so BMPs don't gain background transparency as they do not support it
	if (FileTypeSelected.IsIdentical(String_16(_R(IDS_FILE_TYPE_BMP))))
		pNewOptions->SetBackgroundTransparency(FALSE);

	// juggle the export objects so we dont hold on to too many
	// It appears that BmapPrevDlg::m_pExportOptions is either
	// NULL
	// Or holds a pointer to the [0] options but doesn't own it
	// Or holds a pointer to the [1] options but doesn't own it
	// Or holds a poitner to an objects object that it does own
	// ...
	if (BmapPrevDlg::m_pExportOptions != NULL &&
		BmapPrevDlg::m_pExportOptions != BitmapData[0].m_pOptions &&
		BmapPrevDlg::m_pExportOptions != BitmapData[1].m_pOptions
		)
	{
		// The static options object is not either of the preview objects, so delete it
		delete BmapPrevDlg::m_pExportOptions;
		BmapPrevDlg::m_pExportOptions = NULL;
	}
	BmapPrevDlg::m_pExportOptions = NULL;

	// Delete the old options (if no-one else owns them)
	if (BitmapData[0].m_pOptions != BitmapData[1].m_pOptions)
	{
		delete BitmapData[Index].m_pOptions;
		BitmapData[Index].m_pOptions = NULL;
	}

	// set the new current export options
	BitmapData[Index].m_pOptions = pNewOptions;
	BmapPrevDlg::m_pExportOptions = pNewOptions;

	// set the filter to match the new options
	m_pBmpFilter = pNewOptions->FindBitmapFilterForTheseExportOptions();

	// set this as the active bitmap
	ChangeActiveBitmap(Index+1);

	// tell the tab part of the dlg to update the tabs and the preview
	if (m_pParentDlg)
	{
		//BROADCAST_TO_CLASS( DialogMsg(m_pParentDlg->WindowID, DIM_SOFT_COMMIT, 0, NULL, 0), DialogOp );
		CDlgResID page = m_pParentDlg->GetCurrentPageID();
		m_pParentDlg->DoPreview();
		m_pParentDlg->TalkToPage(page);
		m_pParentDlg->UpdateCurrentTab();
	}
}


/********************************************************************************************

>	void BitmapExportPreviewDialog::RenderControl(ReDrawInfoType* pExtraInfo, UINT32 GadgetID)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Inputs:		pExtraInfo - The structure that has the extra data we need to start rendering
				GadgetID - specifies one of the two draw controls
	Purpose:	The redraw handling for the two draw controls in the dialog. Depending on the 
				zoom flag (m_ZoomRectRender) either the zoom rectangle, or the whole
				bitmap is drawn in the control.

********************************************************************************************/

void BitmapExportPreviewDialog::RenderControl(ReDrawInfoType* pExtraInfo, UINT32 GadgetID)
{
	//  Ok, this may not be the best place to do this, but it will do for the moment.
	//  If we are doing a 'Create Bitmap Copy...', then make sure that the file type 
	//  drop-lists are disabled.
//J	if( !( m_pParentDlg->WantToShowFileTypeDropLists() ) )
//J	{
//J		EnableGadget( _R(IDC_FILE_TYPE_LIST),	FALSE );
//J		EnableGadget( _R(IDC_FILE_TYPE_LIST2),	FALSE );
//J	}

	if (pExtraInfo == NULL)
		return;

	// Go get a render region
	DocRect VirtualSize(0, 0, pExtraInfo->dx, pExtraInfo->dy);

	// test for zoom rectangle request
	if ((m_ZoomRectRender) && (m_StartPos != m_CurrentPos)) 
	{
		// draw inverted rectangle only
		
		// create OSRenderRegion, because GRenderRegion doesn't handle EOR-ed draw
		RenderRegion *pRender = CreateOSRenderRegion(&VirtualSize, pExtraInfo);

		if (pRender == NULL)
			return;

		// Render the attributes and then a rectangle
		pRender->SaveContext();

		pRender->SetDrawingMode(DM_EORPEN);
		
		pRender->SetFillColour(COLOUR_TRANS);
		pRender->SetLineColour(COLOUR_XORSELECT); 

		// draw the rect
		DocRect rc;
		rc.lo.x = m_StartPos.x < m_CurrentPos.x ? m_StartPos.x : m_CurrentPos.x;
		rc.lo.y = m_StartPos.y < m_CurrentPos.y ? m_StartPos.y : m_CurrentPos.y;
		rc.hi.x = m_StartPos.x > m_CurrentPos.x ? m_StartPos.x : m_CurrentPos.x;
		rc.hi.y = m_StartPos.y > m_CurrentPos.y ? m_StartPos.y : m_CurrentPos.y;

		pRender->DrawRect(&rc);

		pRender->RestoreContext();

		// destroy the render region
		DestroyOSRenderRegion(pRender);

		return;
	}

	// normall redraw operation - draw our bitmap
	
	// If we haven't already created our render region then go and create it
	BOOL StartedOk = TRUE;
	if (m_pRender == NULL)
	{
		// This will call start render for us
		m_pRender = (GRenderRegion *)CreateGRenderRegion(&VirtualSize, pExtraInfo);
	}
	else
	{
		// and delete the render region and its (dialog) view
		View * pDialogView = m_pRender->GetRenderView();
		
		if (pDialogView)
		{
			// Try and create the bitmap etc
			StartedOk = m_pRender->AttachDevice(pDialogView, pExtraInfo->pDC, NULL);

			// Try and start the render region
			StartedOk = StartedOk && m_pRender->StartRender();
		}
		else
			StartedOk = FALSE;
	}

	// if we have a render region then go and use it
	if (m_pRender != NULL && StartedOk)
	{
		// Code stolen from ColourEditDlg::RenderControl
		DialogColourInfo RedrawColours;				// Get a supplier for default dlg colours
		INT32 PixelSize = 72000 / pExtraInfo->Dpi;	// Size of output pixel in millipoints

		// Render the attributes and then a rectangle
		m_pRender->SaveContext();

		// Get the current bitmap for the passed control
		KernelBitmap * pBitmapToUse;
		//  The ( 1-based ) number of the bitmap currently being rendered.
		UINT32 BitmapNumber = -1;
		if (GadgetID == _R(IDC_REDRAW1))
		{
			BitmapNumber = 1;
			pBitmapToUse = BitmapData[0].m_pBitmap;
		}
		else if (GadgetID == _R(IDC_REDRAW2))
		{
			BitmapNumber = 2;
			pBitmapToUse = BitmapData[1].m_pBitmap;
		}
		else
			pBitmapToUse = NULL;
		
		// Draw a backgound making sure we blat anything previously there with the dialog
		// background colour
		m_pRender->SetFillColour(RedrawColours.DialogBack());
		if (pBitmapToUse)
			m_pRender->SetLineColour(COLOUR_TRANS); 
		else
			m_pRender->SetLineColour(COLOUR_BLACK); 
		m_pRender->DrawRect(&VirtualSize);

		if (pBitmapToUse)
		{
			// set the dither origin, so its hooked to the top left corner of the bitmap
			// needs to be converted in pixels
			m_pRender->GetDrawContext()->SetHalftoneOrigin( m_BitmapView.lo.x / PixelSize, 
							- m_BitmapView.lo.y / PixelSize); // needs to negate the value in y 

			// Code stolen from BfxDlg::RenderBitmap
			m_pRender->SetLineColour(COLOUR_TRANS);

			NodeBitmap NB;
			NodeBitmap * pNodeBitmap = &NB;
		
//			if (!((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12))))
			if ((pNodeBitmap != NULL) && pNodeBitmap->SetUpPath(12,12))
			{
				// Get a new bitmap object for this node.
				pNodeBitmap->GetBitmapRef()->SetBitmap(pBitmapToUse);
				ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");
		
				// And set this in our bitmap node
				pNodeBitmap->CreateShape(m_BitmapView);

				//  Only do the render if we are not doing a double render which is required
				//  for maintaining the number of colours in the palette in certain circumstances
//				if( !( m_pParentDlg->m_bDoNotRenderThisTime | m_pParentDlg->m_bDoNotRenderThisTime2 ) )
					pNodeBitmap->Render(m_pRender);
			}
		}
		else
		{
			// Draw the no bitmap present cross i.e. vertical
			// And Deflate the rect by 1 pixels so we draw to the outer lines
			VirtualSize.Inflate(-PixelSize);
			INT32 Width = VirtualSize.Width();
			INT32 Height = VirtualSize.Height();
			m_pRender->SetLineWidth(0);
			m_pRender->SetLineColour(COLOUR_BLACK); //RedrawColours.ButtonShadow());
			m_pRender->DrawLine(DocCoord(VirtualSize.lo.x + Width/2, VirtualSize.lo.y),
								DocCoord(VirtualSize.lo.x + Width/2, VirtualSize.hi.y));
			m_pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.lo.y + Height/2),
								DocCoord(VirtualSize.hi.x, VirtualSize.lo.y + Height/2));
		}

		m_pRender->RestoreContext();

		// Blit to the screen
		m_pRender->StopRender();

		// Get rid of the render region, now done in the destructor
		DestroyGRenderRegion(m_pRender);
		m_pRender = NULL;

		//  May have to redraw cursor.
		m_bWantCursorRedrawn = TRUE;
	}
}


/*******************************************************************************************

>	OpState	BitmapExportPreviewDialog::GetState(String_256*, OpDescriptor*)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Purpose:	Returns the OpState of the Preview dialogue operation

*******************************************************************************************/

OpState	BitmapExportPreviewDialog::GetState(String_256*, OpDescriptor*)
{
	return OpState(FALSE, FALSE);
}

		 

/*******************************************************************************************

>	BOOL BitmapExportPreviewDialog::Init()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a Preview Dialog

*******************************************************************************************/

BOOL BitmapExportPreviewDialog::Init()
{  
	return RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_BITMAPPREVIEWDIALOG), 				// String resource ID
								CC_RUNTIME_CLASS(BitmapExportPreviewDialog),// Runtime class
 								OPTOKEN_BITMAPPREVIEWDIALOG,			// Token string
 								BitmapExportPreviewDialog::GetState,		// GetState function
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

>	void BitmapExportPreviewDialog::InitPreviewDialog(BmapPrevDlg *pParentDlg, Filter *pFilter)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on code by Neville Humphrys)
	Created:	11/4/97
	Inputs:		pFilter - pointer to the export filter to be used for creating temp files
	Purpose:	Creates and shows a Preview dialog

*******************************************************************************************/

void BitmapExportPreviewDialog::InitPreviewDialog(BmapPrevDlg *pParentDlg, Filter *pFilter)
{
	ERROR3IF(pFilter == NULL, "BitmapExportPreviewDialog::InitPreviewDialog - no filter");
	ERROR3IF(pParentDlg == NULL, "BitmapExportPreviewDialog::InitPreviewDialog - no filter");

	// remember the pointer to the parent
	m_pParentDlg = pParentDlg;

	// remember the filter
	m_pBmpFilter = (BaseBitmapFilter*)pFilter;

	if (pFilter == NULL)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		FailAndExecute(); 

		// Finished the operation
		End();
		return;
	}

	// give this object to the menu operation
	OpPreviewPopupCommand::m_pDialog = this;

	// close the progress bar indicator
	SmashSlowJob();

	// Force the dialog box to be created.
	// If it is non-modal it must be opened if the create works ok.
	// If it is modal then don't call the open
	if (Create())
	{
		// If dialog is not Modal do this now
		// Otherwise set up happens in the message handler
		if (Mode == MODELESS)
		{
			// Set the initial control values 
			InitDialog();

			// If modeless then call open Open()
			Open();
		}
	}
	else
	{
		TRACE( _T("Failed to create Render Preview Dialog\n"));
		// Could not create the dialog box so call inform error 
		InformError();						// Show user the error
		End(); 		   						// End the operation 
	}
}




/***********************************************************************************************

> 	void BitmapExportPreviewDialog::CalcViewRect(double NewZoomFactor, DocCoord *pos)

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		NewZoomFactor - the new zoom factor
				Pos - the click position, indicating the centre of the new view
    Purpose:    Calculates a new view rectangle using the zoom factor and the position. Pass 
				a negative value for 'Zoom to Fit'. If null is passed for pos, the centre of 
				the control is assumed.
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::CalcViewRect(double NewZoomFactor, DocCoord *pos)
{
	// get the control size
	ReDrawInfoType param;
	GetKernelRenderedGadgetInfo(_R(IDC_REDRAW1), &param);
	MILLIPOINT w = param.dx;
	MILLIPOINT h = param.dy;

	if (NewZoomFactor < 0) 		// zoom to fit
	{
		// calculate zoom factor so that the whole bitmap fits in 
		m_ZoomFactor = (double(w)/m_Width < double(h)/m_Height ? 
				(double(w)/m_Width) * 100 : (double(h)/m_Height) * 100);
		if (m_ZoomFactor <= 0)
			m_ZoomFactor = MIN_ZOOM;

		// scale the bitmap width and height
		MILLIPOINT Width  = (MILLIPOINT)((m_Width  * m_ZoomFactor) / 100);
		MILLIPOINT Height = (MILLIPOINT)((m_Height * m_ZoomFactor) / 100);

		m_BitmapView.lo.x = w/2 - Width / 2;
		m_BitmapView.hi.x = w/2 + Width / 2;
		m_BitmapView.lo.y = h/2 - Height / 2;
		m_BitmapView.hi.y = h/2 + Height / 2;

		INT32 PixelSize = 72000 / param.Dpi;
	}
	else 
	{
		double OldZoomFactor = m_ZoomFactor;

		// set the new zoom factor
		m_ZoomFactor = NewZoomFactor;

		// check for zoom factor > MAX_ZOOM
		
		// check for the first bitmap
		if (NewZoomFactor > (double)MAX_ZOOM) 
			m_ZoomFactor = (double) MAX_ZOOM;

		// now for the second one
		if (m_ZoomFactor * m_Scaling > (double)MAX_ZOOM)
			m_ZoomFactor = double(MAX_ZOOM) / m_Scaling;
			

		// check for zoom factor < MIN_ZOOM

		// first bitmap
		if (m_ZoomFactor < (double)MIN_ZOOM) 
			m_ZoomFactor = (double) MIN_ZOOM;

		// the second one
		if (m_ZoomFactor * m_Scaling < (double)MIN_ZOOM)
			m_ZoomFactor = double(MIN_ZOOM) / m_Scaling;
		

		// scale the bitmap width and height
		MILLIPOINT Width  = (MILLIPOINT)((m_Width  * m_ZoomFactor) / 100);
		MILLIPOINT Height = (MILLIPOINT)((m_Height * m_ZoomFactor) / 100);

		// calculate new view based on the last one

		// find the centre of the view
		DocCoord Centre;
		Centre.x = (m_BitmapView.hi.x + m_BitmapView.lo.x) / 2;
		Centre.y = (m_BitmapView.hi.y + m_BitmapView.lo.y) / 2;

		// scale view
		m_BitmapView.lo.x = Centre.x - Width / 2;
		m_BitmapView.hi.x = Centre.x + Width / 2;
		m_BitmapView.lo.y = Centre.y - Height/ 2;
		m_BitmapView.hi.y = Centre.y + Height/ 2;

		
		INT32 PixelSize = 72000 / param.Dpi;

		DocCoord Pos;
		// if no position passed assume click in the middle of the control
		if (pos == NULL)
		{
			Pos.x = w/2;
			Pos.y = h/2; 
		}
		else 
			Pos = *pos;

		// scale the passed point according to the new zoom
		DocCoord ScaledPos;
		
		// test to prevent 'divide by zero' exception
		if (OldZoomFactor == 0)
			OldZoomFactor = MIN_ZOOM;

		ScaledPos.x = (MILLIPOINT)(Centre.x + (Pos.x - Centre.x) * (m_ZoomFactor / OldZoomFactor));
		ScaledPos.y = (MILLIPOINT)(Centre.y + (Pos.y - Centre.y) * (m_ZoomFactor / OldZoomFactor));

		// translate the new view rectangle to be centered on the scaled position
		m_BitmapView.Translate(w/2 - ScaledPos.x, h/2 - ScaledPos.y);
	}

	// make sure one of the corners is aligned at a pixel

	// get the pixel size in millipoints
	MILLIPOINT PixSize = 0;
	DocUnitList* pDocUnitList =	DocUnitList::GetCurrentDocUnitList();
	ERROR3IF(pDocUnitList == NULL, "BmpPrefsDlg::InitDialog() - no pDocUnitList!")
	if (pDocUnitList != NULL)
	{
		Unit* pPixelUnit = pDocUnitList->FindUnit(PIXELS);
		ERROR3IF(pPixelUnit == NULL, "BmpPrefsDlg::InitDialog() - no pixel units!")
		if (pPixelUnit != NULL)
			PixSize = (MILLIPOINT) pPixelUnit->GetMillipoints();
	}

	// shift the rectangle, so that the lower left corner is aligned at a pixel boundary
	if (PixSize != NULL)
	{
		MILLIPOINT ShiftX = m_BitmapView.lo.x % PixSize;
		MILLIPOINT ShiftY = m_BitmapView.lo.y % PixSize;

		m_BitmapView.Translate(-ShiftX, -ShiftY);
	}


	// display the new zoom factor
	DisplayZoomFactor();
	
	// Render the controls
	InvalidateGadget(_R(IDC_REDRAW1));
	InvalidateGadget(_R(IDC_REDRAW2));
}



/***********************************************************************************************

> 	void BitmapExportPreviewDialog::DoPush(DocCoord *NewPos)

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		NewPos - the new mouse position (in millipoints)
	Purpose:    Pushes the bitmaps by translating the current view rectangle.
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::DoPush(DocCoord *NewPos)
{
	// translate the view 
	DocCoord diff(NewPos->x - m_CurrentPos.x, NewPos->y - m_CurrentPos.y);
	m_BitmapView.Translate(diff.x, diff.y);

	// remember the new position
	m_CurrentPos = *NewPos;

	// Render the controls
	InvalidateGadget(_R(IDC_REDRAW1));
	InvalidateGadget(_R(IDC_REDRAW2));
	PaintGadgetNow(_R(IDC_REDRAW1));
	PaintGadgetNow(_R(IDC_REDRAW2));
}



/***********************************************************************************************

> 	void BitmapExportPreviewDialog::DoZoom(DocCoord *NewPos)

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		NewPos - the new mouse position (in millipoints)
	Purpose:    Removes the last zoom rectangle and draws the new one as part of the zoom to 
				rectangle operation
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::DoZoom(DocCoord *NewPos)
{
	// remove the last rectangle
	DrawZoomRect();

	// remember and set the new rectangle coordinate
	m_CurrentPos = *NewPos;

	// draw a new rectangle
	DrawZoomRect();
}



/***********************************************************************************************

> 	void BitmapExportPreviewDialog::DrawZoomRect()

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		None.
	Purpose:    Draws the zoom rectangle, by setting the zoom flag and invalidating the controls.
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::DrawZoomRect()
{
	// ask the redraw funcion to draw the zoom rectangle
	m_ZoomRectRender = TRUE;
	InvalidateGadget(_R(IDC_REDRAW1));
	InvalidateGadget(_R(IDC_REDRAW2));
	PaintGadgetNow(_R(IDC_REDRAW1));
	PaintGadgetNow(_R(IDC_REDRAW2));
	m_ZoomRectRender = FALSE;
}



/***********************************************************************************************

> 	void BitmapExportPreviewDialog::ZoomToRect()

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		None.
	Purpose:    Implements zoom to rectangle. The rectangle is defined by m_CurrentPos and 
				m_StartPos. Very small rectangles are treated as click-zooms
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::ZoomToRect()
{
	// find the centre of the view
	DocCoord Centre;
	Centre.x = (m_CurrentPos.x + m_StartPos.x) / 2;
	Centre.y = (m_CurrentPos.y + m_StartPos.y) / 2;

	DocCoord size;
	size.x = abs(m_CurrentPos.x - m_StartPos.x);
	size.y = abs(m_CurrentPos.y - m_StartPos.y);

	// check for very small rectangle
	if ((size.x < 3000) && (size.y < 3000) )
	{
		// treat this as a click zoom
		CalcViewRect(FindNearestZoom(!KeyPress::IsAdjustPressed()),&Centre);
	}
	else
	{
		// get the control size
		ReDrawInfoType param;
		GetKernelRenderedGadgetInfo(_R(IDC_REDRAW1), &param);
		MILLIPOINT w = param.dx;
		MILLIPOINT h = param.dy;

		// calc scaling factor
		double Scale = max(double(size.x) / w, double(size.y) / h);

		// check for zero scale
		if (fabs(Scale) < 0.0001)
			return;

		// calculate the new zoom factor
		double NewZoomFactor = m_ZoomFactor / Scale;

		// calculate the new view
		CalcViewRect(NewZoomFactor, &Centre);
	}
}




/***********************************************************************************************

> 	void BitmapExportPreviewDialog::ChangeActiveBitmap(UINT32 Active)

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		Active - the active bitmap - either 1 or 2
	Purpose:    Sets new active bitmap and makes the frame around it visible (and the one around 
				the other bitmap invisible). The current zoom factor is also updated.
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::ChangeActiveBitmap(UINT32 Active)
{
	ERROR3IF((Active != 1) && (Active != 2),"Invalid active bitmap passed");

	if (m_ActiveBitmap == Active) // no change
		return; 

	if (BitmapData[Active - 1].m_pBitmap == NULL)
		return; // can't select non-existent bitmap

	// remember it
	m_ActiveBitmap = Active;

	// THE export options are the active side of the export dlg
	if (BitmapData[m_ActiveBitmap - 1].m_pOptions)
	{
		// In other places the static pointer is handled carefully - but not here!
		BmapPrevDlg::m_pExportOptions = BitmapData[m_ActiveBitmap - 1].m_pOptions;
		// the filter should match THE export options
		m_pBmpFilter = BmapPrevDlg::m_pExportOptions->FindBitmapFilterForTheseExportOptions();
		if (m_pBmpFilter)
			m_pBmpFilter->SetExportOptions(BmapPrevDlg::m_pExportOptions);
	}
	
	// define the active one
	BOOL ActiveFirst = (Active == 1);

	// The palette sort data is out of date so force it to be regenerated the next time it is used
	BitmapExportPaletteInterface::InvalidateSortedPalette();

	// update the tab to reflect the new active side of the dlg
	if (m_pParentDlg)
		m_pParentDlg->UpdateCurrentTab();

	// update the frame title string
	DisplayZoomFactor();

	// hide the frame around the previous active control and show it around the new active one
//	HideGadget(_R(IDC_ACTIVEFRAME1), !ActiveFirst);
//	HideGadget(_R(IDC_ACTIVEFRAME2), ActiveFirst);

	EnableGadget(_R(IDC_ACTIVE1), ActiveFirst);
	EnableGadget(_R(IDC_ACTIVE2), !ActiveFirst);
	
	//  Since the active preview image has changed, have to enable/disable the appropriate
	//  file type drop-lists.
	//  If we want to have both of the drop-lists disabled, then we can return now.
	// cant change the frame if we are a create bitmap option
	if (!BmapPrevDlg::m_bIsCreateBitmap)
	{
		if( Active == 1 )
		{
			//  If the user has clicked on the first image, need to enable the first list, and
			//  disable the second list.
			EnableGadget( _R(IDC_FILE_TYPE_LIST),	TRUE );
			EnableGadget( _R(IDC_FILE_TYPE_LIST2),	FALSE );
		}
		else if( Active == 2 )
		{
			EnableGadget( _R(IDC_FILE_TYPE_LIST),	FALSE );
			EnableGadget( _R(IDC_FILE_TYPE_LIST2),	TRUE );
		}
	}
}



/***********************************************************************************************

> 	void BitmapExportPreviewDialog::DisplayZoomFactor()

    Author: 	Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
    Created:	20/04/97
	Inputs:		None.
	Purpose:    Displays the zoom factor.
	Returns:	None.
                 			                                     
***********************************************************************************************/

void BitmapExportPreviewDialog::DisplayZoomFactor()
{
	// get the "Selected" string
	String_32 Sel(_R(IDS_PREVIEW_SELECTED));

	// for the first image
	String_32 Str1;
	Str1.MakeMsg(_R(IDS_PREVIEW_IMAGEA), (UINT32)(m_ZoomFactor + 0.5));
	if (m_ActiveBitmap == 1)
	{
		Str1 += Sel;
		EnableGadget(_R(IDC_ACTIVE1), TRUE);
	}
	else
		EnableGadget(_R(IDC_ACTIVE1), FALSE);

	SetStringGadgetValue(_R(IDC_ACTIVE1),&Str1);
	InvalidateGadget(_R(IDC_FILE_TYPE_LIST)); // so the outline is not drawn over the top of the drop down

	// for the second image
	Str1.MakeMsg(_R(IDS_PREVIEW_IMAGEB), UINT32(m_ZoomFactor * m_Scaling + 0.5));
	if (m_ActiveBitmap == 2)
	{
		Str1 += Sel;
		EnableGadget(_R(IDC_ACTIVE2), TRUE);
	}
	else
		EnableGadget(_R(IDC_ACTIVE2), FALSE);

	SetStringGadgetValue(_R(IDC_ACTIVE2),&Str1);
	InvalidateGadget(_R(IDC_FILE_TYPE_LIST2)); // so the outline is not drawn over the top of the drop down

	DisplayBitmapInfo();
}




/********************************************************************************************

>	void BitmapExportPreviewDialog::DisplayBitmapInfo()

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the file and image sizes and the file size difference between the 
				two bitmaps
********************************************************************************************/

void BitmapExportPreviewDialog::DisplayBitmapInfo()
{
	if ((BitmapData[0].m_pBitmap == NULL) || (BitmapData[1].m_pBitmap == NULL))
	{
		// no bitmap yet
		EnableGadget(_R(IDC_IMAGESIZE1), FALSE);
		EnableGadget(_R(IDC_IMAGESIZE2), FALSE);
		EnableGadget(_R(IDC_FILESIZE1), FALSE);
		EnableGadget(_R(IDC_FILESIZE2), FALSE);
		HideGadget(_R(IDC_FILEDIFF1), TRUE);
		HideGadget(_R(IDC_FILEDIFF2), TRUE);
		return;
	}

	EnableGadget(_R(IDC_IMAGESIZE1), m_ActiveBitmap == 1);
	EnableGadget(_R(IDC_IMAGESIZE2), m_ActiveBitmap == 2);
	EnableGadget(_R(IDC_FILESIZE1), m_ActiveBitmap == 1);
	EnableGadget(_R(IDC_FILESIZE2), m_ActiveBitmap == 2);

	String_64 ImageSize;
	String_64 FileSize;
	
	// for the first image
	BitmapData[0].GenerateBitmapInfoStrings(ImageSize, FileSize, TRUE);
	SetStringGadgetValue(_R(IDC_IMAGESIZE1), &ImageSize);
	SetStringGadgetValue(_R(IDC_FILESIZE1), &FileSize);

	// for the second image
	BitmapData[1].GenerateBitmapInfoStrings(ImageSize, FileSize, TRUE);
	SetStringGadgetValue(_R(IDC_IMAGESIZE2), &ImageSize);
	SetStringGadgetValue(_R(IDC_FILESIZE2), &FileSize);

	// now set the file difference

	// calc the file difference
	INT32 diff = BitmapData[0].m_FileSize - BitmapData[1].m_FileSize;

	// display only for the selected bitmap
	HideGadget(_R(IDC_FILEDIFF1), m_ActiveBitmap != 1);
	HideGadget(_R(IDC_FILEDIFF2), m_ActiveBitmap != 2);

	String_64 FileDiff(_R(IDS_FILE_DIFF));

	if (diff != 0)
	{
		String_32 s;
		s.MakeMsg(_R(IDS_FILE_DIFF_BYTES), abs(diff));
		FileDiff += s;
	}

	if (m_ActiveBitmap == 1)
	{
		if (diff > 0)
			FileDiff += String_16(_R(IDS_LARGER));
		else if (diff < 0)
			FileDiff += String_16(_R(IDS_SMALLER));
		else
			FileDiff += String_16(_R(IDS_SAMESIZE));

		SetStringGadgetValue(_R(IDC_FILEDIFF1),&FileDiff);
	}
	else
	{
		if (diff < 0)
			FileDiff += String_16(_R(IDS_LARGER));
		else if (diff > 0)
			FileDiff += String_16(_R(IDS_SMALLER));
		else
			FileDiff += String_16(_R(IDS_SAMESIZE));

		SetStringGadgetValue(_R(IDC_FILEDIFF2),&FileDiff);
	}
}

	
/********************************************************************************************

>	BOOL BitmapExportPreviewDialog::OnExport(BitmapExportOptions * pExportOptions)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		Num - in which preview box to set the bitmap (either 1, or 2)
	Outputs:	-
	Returns:	-
	Purpose:	Alows export of the document into a temp file to be requested on 
				pressing the Export button in the dialog.
********************************************************************************************/

BOOL BitmapExportPreviewDialog::OnExport(BitmapExportOptions * pExportOptions)
{
	// remember the active bitmap
	UINT32 Num = m_ActiveBitmap;

	// if first time call - set the first bitmap
	if (Num == 0)
		Num = 1;

	// check for bitmap filter
//	if (m_pBmpFilter == NULL)
//		return FALSE;

	// get the current options
	BitmapExportOptions *pCurrentOptions = BitmapData[Num-1].m_pOptions;

	if (pExportOptions == NULL)
		return FALSE;

	//  Compare the new options with the old ones. Also look at the m_bUseExistingPalette
	//  to see if we should use the existing palette.
/*	if ( ((pCurrentOptions != NULL)&& (pCurrentOptions->Equal(pExportOptions))) && !BmapPrevDlg::m_bUseExistingPalette 
		&& ( BmapPrevDlg::m_nTransColour ) < 0 )
	{
		// the same options => do nothing
		return TRUE;  
	}
*/
	// we want to keep a copy of the options, so make one
	//BitmapExportOptions *pOptions = pExportOptions->MakeCopy();

	// export and import back 
	//BOOL ok = DoWithExportOptions(pOptions, Num);

	BOOL ok = DoWithExportOptions(pExportOptions, Num);

	if (ok)
	{
		// everything went ok, so
		// store the temp file in the options passed to us
//		pExportOptions->CopyFrom(BitmapData[m_ActiveBitmap-1].m_pOptions);
	}
/*	else
	{
		// get rid of the temp options
		delete pOptions;
	}
*/
	return ok;
}


/******************************************************************************************

>	void BitmapExportPreviewDialog::InitialiseBitmapPaletteInformation( BitmapExportOptions* pExportOptions, 
				INT32 Num = 0 )

	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>

	Created:	17/08/99

	Inputs:		pExportOptions - The export options containing the palette information
				Num - If the default value of 0 is used here, then information is copied
				over to both preview images. If the value of Num is < 0, then only the 
				active bitmap is updated.

	Returns:	-

	Purpose:	Copies the palette information to the preview image information block. Depending
				on the value of 'Num' passed in, then either the currently active bitmap,
				or both the bitmaps are updated.

******************************************************************************************/

void BitmapExportPreviewDialog::InitialiseBitmapPaletteInformation( BitmapExportOptions* pExportOptions, INT32 Num )
{
	if( Num < 0 )
	{
		if( BitmapData[ m_ActiveBitmap - 1 ].m_pOptions )
			BitmapData[ m_ActiveBitmap - 1 ].m_pOptions->CopyFrom( pExportOptions );
	}
	else
	{
		if( BitmapData[ 0 ].m_pOptions )
			BitmapData[ 0 ].m_pOptions->CopyFrom( pExportOptions );
		if( BitmapData[ 1 ].m_pOptions )
			BitmapData[ 1 ].m_pOptions->CopyFrom( pExportOptions );
	}
}


/********************************************************************************************

>	BOOL BitmapExportPreviewDialog::DoWithExportOptions(BitmapExportOptions *pExportOptions, 
			UINT32 Num)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		pExportOptions - pointer to bitmap export options
				Num - in which preview box to set the bitmap (either 1, or 2)
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Receives bitmap export options, which are passed to the export filter to export 
				the document into a temporary disk file. This file is then imported back to be 
				displayed in one of the preview rectangles. 
********************************************************************************************/

BOOL BitmapExportPreviewDialog::DoWithExportOptions(BitmapExportOptions *pOptions, UINT32 Num)
{
	ERROR3IF(pOptions == NULL,"NULL Export options parameter passed");
	ERROR3IF((Num != 1) && (Num != 2), "Invalid num of preview rectangle passed");

/*	// set the filter to match the new options
	// if it hasn't been set for some reason

  if (m_pBmpFilter == NULL)
		m_pBmpFilter = pOptions->FindBitmapFilterForTheseExportOptions();
*/
	// test for bitmap filter
	if ((m_pBmpFilter == NULL) || (!m_pBmpFilter->IS_KIND_OF(BaseBitmapFilter)))
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		return FALSE;
	}

	// create a disk file
	CCDiskFile TempDiskFile(1024, FALSE, TRUE);
	
	// create a temp file 
	PathName TempPath = FileUtil::GetTemporaryPathName();

// Set flag telling exporter that we are NOT exporting a temporary file!
pOptions->SetTempFileFlag(TRUE);

	BOOL ok = TRUE;
		
	TRY  // to export the document to the temp file
	{
		//no need for preview bitmap
		m_pBmpFilter->IncludePreviewBmp(FALSE);

		// start the progress bar
		BeginSlowJob();
		
		// try to export the document
		BOOL ExportedOK = TRUE;

		if (pOptions->GetSelectionType() != ABITMAP)
		{
			// export the document
			ExportedOK = m_pBmpFilter->DoExportWithOptions(this, &TempDiskFile, &TempPath, 
													Document::GetCurrent(), pOptions);
		}
		else
		{
			ERROR3IF(!pOptions->IS_KIND_OF(JPEGExportOptions),"Bad bitmap export options type");
			JPEGExportOptions *pJPEGOptions = (JPEGExportOptions *)pOptions;

			// get the bitmap to export from the options
			KernelBitmap* pBitmap = pJPEGOptions->GetKernelBitmap();
			
			if (pBitmap != NULL)
			{
				// export the bitmap
				ExportedOK = ((JPEGExportFilter *)m_pBmpFilter)->DoExportBitmapWithOptions(this, 
												&TempDiskFile, &TempPath, pBitmap, pJPEGOptions);

				//restore the pointer to the bitmap, which got cleared during the export
				if (ExportedOK)
					pJPEGOptions->SetKernelBitmap(pBitmap);
			}
			else
			{
				ok = FALSE;
			}
		}

		if (!ExportedOK)
		{
			UINT32 ErrNo = Error::GetErrorNumber();
			// Something went a bit wrong - tell the user what it was.
			// Supress the error if it was the 'user has cancelled one'
			if ((ErrNo != _R(IDN_USER_CANCELLED)) && ( ErrNo != _R(IDW_CANCELEXPORT)))
			{
				InformError();
				m_pBmpFilter->DeleteExportFile(&TempDiskFile);
			}
			else
			{
		 		// otherwise remove the error so it won't get reported
			 	Error::ClearError();
			}

			EndSlowJob();

			return FALSE;
		}
	}
	// See if there was a file io error
	CATCH(CFileException, e)
	{
		UINT32 ErrNo = Error::GetErrorNumber();
		// Report the error if no one else did
		if ((ErrNo != _R(IDN_USER_CANCELLED)) && (ErrNo != _R(IDW_CANCELEXPORT)) && 
			(ErrNo != _R(IDW_CANCELLEDBMPEXPORT)))
		{
			InformError();
		}
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported

		EndSlowJob();

		// Make sure that the file is closed and deleted
		TRY
		{
			// First try and delete it (tries to close it first)
			if (m_pBmpFilter)
				m_pBmpFilter->DeleteExportFile(&TempDiskFile);

			// Double check to make sure it is closed.
			if (TempDiskFile.isOpen())
				TempDiskFile.close();
		}
		CATCH(CFileException, e)
		{
			// Failed to close the file - not much we can do about it really
		}
		END_CATCH

		return FALSE;
	}
	END_CATCH

	if (ok)
	{
		// try to import the temporary file

		// close it first 
		if (TempDiskFile.isOpen())
			TempDiskFile.close();

		// open it back for reading
		ok = TempDiskFile.open(TempPath, ios::in | ios::binary);

		// Make sure we have a generic filter!
		GenericFilter *pGenericFilter = Filter::GetGenericFilter();

		if (ok && pGenericFilter != NULL)
		{
			// remember the file size
			UINT32 FileSize = TempDiskFile.Size();

			// find the best filter for the import
			Filter *pImportFilter = pGenericFilter->GetBestFilter(&TempDiskFile);
			KernelBitmap *pKernelBitmap;

			// we only handle bitmap filters
			if (pImportFilter && !pImportFilter->IS_KIND_OF(BaseBitmapFilter))
			{
				// not a bitmap filter 
				pImportFilter = NULL;
			}

			// import the file
			if (pImportFilter != NULL) 
			{
				// Set the preview bitmap flag to avoid adding the bitmap to the global bitmap list
				((BaseBitmapFilter *)pImportFilter)->SetPreviewBitmap(TRUE);

				ok = pImportFilter->ImportBitmap(&TempDiskFile, &pKernelBitmap);

				// Set it back
				((BaseBitmapFilter *)pImportFilter)->SetPreviewBitmap(FALSE);

				// close the file
				TempDiskFile.close();

				// close the progress bar indicator
				EndSlowJob();

				if (ok)
				{
					// check if the bitmap is the same size, so the current view can be preserved
					BOOL NeedDefaultZoom = TRUE; 
					KernelBitmap *pBmp = BitmapData[Num - 1].m_pBitmap;

					if (pBmp != NULL)
					{
						if ((pKernelBitmap->GetRecommendedWidth() == pBmp->GetRecommendedWidth()) && 
							(pKernelBitmap->GetRecommendedHeight() == pBmp->GetRecommendedHeight()))
						NeedDefaultZoom = FALSE;
					}

					// if doing a "make bitmap copy" - use the memory instead of the file size
					if ((m_pParentDlg != NULL) && (m_pParentDlg->m_FilterType == MAKE_BITMAP_FILTER))
					{
						// get the OIL bitmap
						OILBitmap* pOILy = pKernelBitmap->GetActualBitmap();

						// extract the bitmap size
						if (pOILy != NULL)
							FileSize =pOILy->GetBitmapSize();
					}

					String_256 oldTempPath = pOptions->GetPathName().GetPath();
					if (pOptions->GetPathName().IsValid() && (oldTempPath != TempPath.GetPath()))
					{
						// Delete any old files that might be around
						FileUtil::DeleteFile(&(pOptions->GetPathName()));
					}

					// set the new path name in the options object
					pOptions->SetPathName(&TempPath);

					// set the bitmap 
					BitmapData[Num-1].SetNewBitmap(pKernelBitmap, FileSize, pOptions);

					//  If we need to, make a copy of one of the Kernel bitmaps before 
					//  transparency is turned on. This is done so that, when 1 or more colours
					//  have been made transparent, then moving the mouse over the pixel in the
					//  preview image highlights the colour 'underneath'
					if( m_pParentDlg->m_pExportOptions->GetTransparencyIndex())
					{
						//  Preview image 'Num' has been made transparent.
						//  If there is no KernelBitmap, then make one.
						if( pOriginalBitmap[ Num - 1 ] == NULL )
						{
							if( m_bNeedBitmapCopied[ Num - 1 ] )
							{
								pOriginalBitmap[ Num - 1 ] = new KernelBitmap( ( BitmapData[Num-1].m_pBitmap )->ActualBitmap );
								//  Make sure the following variable is set to the correct value.
								m_bNeedBitmapCopied[ Num - 1 ] = FALSE;
							}
						}
						else
						{
							//  There has been a bitmap before. To determine whether we can use this
							//  one or not, we look at the appropriate variable.
							if( m_bNeedBitmapCopied[ Num - 1 ] )
							{
								//  Need a new bitmap
								//  Get rid of the old one first.
								if( pOriginalBitmap[ Num - 1 ] )
								{
									delete pOriginalBitmap[ Num - 1 ];
									pOriginalBitmap[ Num - 1 ] = NULL;
								}
								pOriginalBitmap[ Num - 1 ] = new KernelBitmap( ( BitmapData[Num-1].m_pBitmap )->ActualBitmap );
								//  Don't need to do this again for the moment.
								m_bNeedBitmapCopied[ Num - 1 ] = FALSE;
							}
							else
							{
								//  It is ok to use the existing bitmap, so we don't have to do
								//  anything else.
							}
						}
					}
					
					// check if this is the first bitmap
					if (BitmapData[2-Num].m_pBitmap == NULL)
					{
						// initialise the export options for the other side
						// try to make them both the same and borrow the bitmap from the first side
						BitmapExportOptions * pNewOptions = NULL;

						switch(pOptions->GetFilterNameStrID())
						{
						case _R(IDN_FILTERNAME_GIF):
							pNewOptions = new GIFExportOptions();
							break;

						case _R(IDT_FILTERNAME_BMP):
							pNewOptions = new BMPExportOptions();
							break;

						default:
						case _R(IDS_FILTERNAME_PNG):
							pNewOptions = new PNGExportOptions();
							break;

						case _R(IDS_JPG_EXP_FILTERNAME):
							pNewOptions = new JPEGExportOptions();
							break;
						}


						KernelBitmap * pNewKBMP = new KernelBitmap(pKernelBitmap->ActualBitmap);

						if (pNewKBMP && pNewOptions)
						{
							pNewOptions->RetrieveDefaults();
							pNewOptions->CopyFrom(pOptions);

							// set the other bitmap data to be the same
							BitmapData[2-Num].SetNewBitmap(pNewKBMP, FileSize, pNewOptions);

							// clear the flags for same bitmap
							BitmapData[0].m_bIsSameBitmap = FALSE;
							BitmapData[1].m_bIsSameBitmap = FALSE;
						}
						else
							BitmapData[2-Num].SetNewBitmap(pKernelBitmap, FileSize, pOptions);

					}
					else
					{
						// clear the flags for same bitmap
						BitmapData[0].m_bIsSameBitmap = FALSE;
						BitmapData[1].m_bIsSameBitmap = FALSE;
					}

					if (BitmapData[0].m_pBitmap != NULL)
					{
						// remember some bitmap info
						m_Width  = BitmapData[0].m_pBitmap->GetRecommendedWidth();
						m_Height = BitmapData[0].m_pBitmap->GetRecommendedHeight();
						if (m_Width  < 1) m_Width  = 1;
						if (m_Height < 1) m_Height = 1;

						// remember the scaling between the two bitmaps 
						// (assuming the aspect ratio is the same)
						MILLIPOINT WidthB = BitmapData[1].m_pBitmap->GetRecommendedWidth();
						if (WidthB < 1) 
							WidthB = 1;
						m_Scaling = (double) m_Width / WidthB;

						// set the active bitmap
						ChangeActiveBitmap(Num);

						// display info for the bitmap
						DisplayBitmapInfo();

						// calculate new view if necessary
						if (NeedDefaultZoom)
						{
							// do the default zoom
							CalcViewRect(-1, NULL); // calculate initial view
							
							// zoom the new rectangle to 100% 
							if (Num == 1)
								CalcViewRect(100, NULL);
							else 
								CalcViewRect(100 / m_Scaling, NULL);
							PaintGadgetNow(_R(IDC_REDRAW1));
							PaintGadgetNow(_R(IDC_REDRAW2));
						}
						else
						{
							// Render the controls
							InvalidateGadget(_R(IDC_REDRAW1));
							InvalidateGadget(_R(IDC_REDRAW2));
							PaintGadgetNow( _R(IDC_REDRAW1) );
							PaintGadgetNow( _R(IDC_REDRAW2) );
						
						}

						return TRUE;
					}
				}

				if (!ok) // failed to import the file properly
				{
					UINT32 ErrNo = Error::GetErrorNumber();
					// Something went a bit wrong - tell the user what it was.
					// Supress the error if it was the 'user has cancelled one'
					if ((ErrNo != _R(IDN_USER_CANCELLED)) && (ErrNo != _R(IDW_CANCELLEDBMPIMPORT)))
					{
						InformError();
					}
					else
					{
		 				// otherwise remove the error so it won't get reported
			 			Error::ClearError();
					}
				}
			}
		}
	}

	// the operation failed
	return FALSE;
}


/********************************************************************************************

>	void BitmapExportPreviewDialog::DoCommand(StringBase *CommandID)

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/97
	Inputs:		CommandID - the token of the menu selected command 
	Outputs:	-
	Returns:	-
	Purpose:	Called when a menu command is selected to perform the respective operation
********************************************************************************************/

void BitmapExportPreviewDialog::DoCommand(StringBase *CommandID)
{
	if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_ZOOM_TOOL)
		OnZoomTool(); // "Zoom Tool"
	else if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_PUSH_TOOL)
		OnPushTool(); // "Push Tool"
	else if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_COLOUR_SELECTOR_TOOL)
		OnColourSelectorTool(); // 'Colour Selector'
	else if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_ZOOM_TO_FIT)
		CalcViewRect(-1, NULL); //"Zoom to Fit"
	else if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_ZOOM_TO_100)
		On100Percent(); // "Zoom to 100%"
	else if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_1TO1)
		On1to1(); // "Zoom to 100%"
	else if (*CommandID == (TCHAR *)OPTOKEN_PREVIEW_HELP)
	{
		// call the parent dialog to do the help 
		if (m_pParentDlg != NULL)
			m_pParentDlg->OnHelpButtonClicked();
	}
}
