// $Id: prevwdlg.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "prevwdlg.h"

#include "ccdc.h"		// specific #includes needed for kernel-rendered dialogues
#include "dlgcol.h"		// DialogColourInfo
//#include "fillval.h"
#include "grnddib.h"

//#include "prevwres.h"
#include "impexpop.h"	// BitmapExportParam
#include "nodebmp.h"	// NodeBitmap
#include "gifutil.h"	// GDM_BACKTOBACK
#include "dlgview.h"	// DialogView

//#include "rikdlg.h"		// _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER)
#include "app.h"
#include "ctrlhelp.h"
#include "docvmsg.h"

#include "spread.h"		// GetFirstFrameLayer()

#include "xshelpid.h"	//For the help ID

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(AnimatedBitmapItem, ListItem)
CC_IMPLEMENT_DYNCREATE(PreviewDialog, DialogOp)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


// Set the static vars of the render dialog
const CDlgMode	PreviewDialog::Mode = MODELESS; 
const UINT32		PreviewDialog::IDD = _R(IDD_PREVIEWDIALOG);
	  BOOL		PreviewDialog::m_AnimationPropertiesChanged = FALSE;
			  
typedef struct
{
	CGadgetID	Gadget;			// Gadget ID
	UINT32		BubbleID;		//Bubble help ID for this gadget
} GadgetBubbleHelp;


// If extra controls are added to the Preview dialog and
// they require Bubble help, add them to this table.
static GadgetBubbleHelp BubbleHelp[] = 
{

	{	_R(IDC_PREVIEW_PLAY),		 _R(IDS_PREVIEWBH_PLAY)		 },
	{	_R(IDC_PREVIEW_STOP),		 _R(IDS_PREVIEWBH_STOP)	     },
	{	_R(IDC_PREVIEW_NEXTFRAME),	 _R(IDS_PREVIEWBH_NEXTFRAME) },
	{	_R(IDC_PREVIEW_PREVFRAME),	 _R(IDS_PREVIEWBH_PREVFRAME) },	
	{	_R(IDC_PREVIEW_SLIDER),		 _R(IDS_PREVIEWBH_SLIDER)	 },
	{	NULL, NULL										 }	

};

BOOL PreviewDialog::viaBitmapGallery = FALSE;




/********************************************************************************************

>	AnimatedBitmapItem::AnimatedBitmapItem(KernelBitmap *pEntry, Layer * pLayer = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		pEntry	- The bitmap to add to the list of frames
				pLayer	- The layer that the bitmap is associated with (defaults to NULL)
	Purpose:	To add a bitmap to the list of frames in the animation sequence. If it has
				an associated layer then add this at the same time.

********************************************************************************************/

AnimatedBitmapItem::AnimatedBitmapItem(KernelBitmap * pEntry, Layer * pLayer)
{
	m_pBitmap = pEntry;
	m_pLayer = pLayer;
}

/********************************************************************************************

>	AnimatedBitmapItem::AnimatedBitmapItem()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Default contructor.

********************************************************************************************/

AnimatedBitmapItem::AnimatedBitmapItem()
{
	ERROR3("AnimatedBitmapItem::AnimatedBitmapItem - call the other constructor");
	m_pBitmap = NULL;
	m_pLayer = NULL;
}

/********************************************************************************************

>	AnimatedBitmapItem::~AnimatedBitmapItem()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Default destructor

********************************************************************************************/

AnimatedBitmapItem::~AnimatedBitmapItem()
{
	// Do not attempt to delete the pointers that we have as these are references
	// rather than anything that we have allocated
}



/*******************************************************************************************
********************************************************************************************/
// Initialise our statics

PreviewDialog * PreviewDialog::m_pPreviewDialog = NULL;
UINT32			PreviewDialog::m_Delay = 0; 


/*******************************************************************************************

>	PreviewDialog::PreviewDialog() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Constructs a Preview Dialog

*******************************************************************************************/

PreviewDialog::PreviewDialog() : DialogOp(PreviewDialog::IDD, PreviewDialog::Mode) 
{
	m_BitmapListSize			= 0;
	m_CurrentItem				= 0;
	m_pPreviewDialog			= this;
	m_pRender					= NULL;
	m_PlayAnimation				= TRUE;
	m_DisplayNextFrame			= FALSE;
	m_DisplayPreviousFrame		= FALSE;
	m_SliderPosition			= m_BitmapListSize;
	m_DocumentAboutToDie		= FALSE;
	m_SetPreviewDlgToIdleState	= FALSE;
	m_GeneratedFrameLayers		= FALSE;
	m_DisposalMethod			= GDM_BACKTOBACK;
	m_CurrentDisposalMethod		= GDM_BACKTOBACK;

	m_AnimationRect.MakeEmpty();

	// Reset the loop counts
	m_CurrentLoopCount			= 0;	// The loop that we are currently on (default to start)
	m_RequiredLoopCount			= 0;	// The loop count that we must reach (default for ever)
	
	//viaBitmapGallery			= FALSE;		// THIS MUST ALREADY HAVE BEEN SET TO THE CORRECT
												// VALUE !!!!!
	
	// Get the input focus for bubble help.
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_HIGH, this);
}       

/*******************************************************************************************

>	PreviewDialog::~PreviewDialog()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Destructor

*******************************************************************************************/

PreviewDialog::~PreviewDialog()
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
	
	// Clean out the list of bitmaps that we were animating
	m_BitmapList.DeleteAll();

	// kill the static link to this dialog box
	m_pPreviewDialog = NULL;

	// The dialog has been destroyed, remove the input focus.
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);
} 



/*******************************************************************************************

>	MsgResult PreviewDialog::Message(Msg* Message)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Handles all the Preview dialog's messages 

*******************************************************************************************/

MsgResult PreviewDialog::Message(Msg* Message)
{
	MsgResult Result;

	BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog

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
				// As it is a modal dialog box we are sent a message to say the dialog box
				// is being created, so we have a chance to set the initial control values.
			//	if (Mode == MODAL)
			{
					InitDialog();
					SetUpCallBack();
					
					INT32 i = 0;
					while (BubbleHelp[i].Gadget != NULL)						
					{
						SetGadgetHelp(BubbleHelp[i].Gadget, BubbleHelp[i].BubbleID, BubbleHelp[i].BubbleID);
						i++;
					}
				break;
			}
			//	break;

			// Close and destroy the dialog 
			case DIM_COMMIT :
			case DIM_CANCEL :
			{
				EndDialog = TRUE;						// Flag to close and end
				break;
			}
			//case DIM_SLIDER_POS_SET:
			case DIM_SLIDER_POS_CHANGING:
			{
				// Messages to all the controls, handled individually
				switch (Msg->GadgetID)
				{
					case _R(IDC_PREVIEW_SLIDER):								
					{
						// While the animation is playing, if this button is selected,
						// the play button is clicked out, the stop button is clicked in,
						// and the next/previous frame in the animation is displayed. 
						PreviewDlgStop();
						// Kill any bubble help just in case the user has bubble help up
						// over the slider
						ControlHelper::BubbleHelpDisable();

						SetSliderPosition();												
													
						// Force the gadget to be repainted, 
						// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
						InvalidateGadget(_R(IDC_REDRAW_ME));
					}
					break;
				}
				break;	//DIM_SLIDER_POS_CHANGING:
			}

			case DIM_LFT_BN_CLICKED :
			{
				// See which button was pressed
				switch (Msg->GadgetID)
				{
					case _R(IDC_PREVIEW_PLAY) :
						{
							// Play the animation.
							PreviewDlgPlay();
							
							// Force the gadget to be repainted, 
							// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
							InvalidateGadget(_R(IDC_REDRAW_ME));
							break;
						}

					case _R(IDC_PREVIEW_STOP) :
						{
							// Stop the animation.
							PreviewDlgStop();

							// Force the gadget to be repainted, 
							// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
							InvalidateGadget(_R(IDC_REDRAW_ME));
							break;
						}

					case _R(IDC_PREVIEW_NEXTFRAME) :
						{
							//  While the animation is playing, if this button is selected,
							//	the play button is clicked out, the stop button is clicked in,
							//	and the next frame in the animation is displayed. 
							PreviewNextFrame();
													
							// Force the gadget to be repainted, 
							// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
							InvalidateGadget(_R(IDC_REDRAW_ME));

							// Only show one frame at a time, therefore set this flag back to false. 
							m_DisplayNextFrame = FALSE;

							break;
						}

					case _R(IDC_PREVIEW_PREVFRAME) :
						{
							// While the animation is playing, if this button is selected,
							// the play button is clicked out, the stop button is clicked in,
							// and the previous frame in the animation is displayed. 
							PreviewPreviousFrame();
							
							// Force the gadget to be repainted, 
							// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
							InvalidateGadget(_R(IDC_REDRAW_ME));

							// Only show one frame at a time, therefore, set this flag to false. 
							m_DisplayPreviousFrame = FALSE;
							break;
						}

					case _R(IDC_PREVIEW_REFRESH) :
						{
							// Has the refresh button been clicked, while the dlg is in an idle state.
							if (m_SetPreviewDlgToIdleState)
								SetDlgStateAfterDocChange();
							else
								SetDlgStateAfterRefresh();								
						}
						break;

					/* case _R(IDC_PREVIEW_STARTFRAME) :
					{
							m_DisplayListOfBitmaps = TRUE;

							// Find the first frame in our list.
							m_pPreviewDialog->SelectFirstBitmap();

							// Force the gadget to be repainted, 
							// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
							m_pPreviewDialog->InvalidateGadget(_R(IDC_REDRAW_ME));
							m_DisplayListOfBitmaps = FALSE;

							break;
						
						}*/

					/* case _R(IDC_PREVIEW_ENDFRAME) :
						{
						m_DisplayListOfBitmaps = TRUE;
						// Find the first frame in our list.
						SelectLastBitmap();

						// Force the gadget to be repainted, 
						// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
						m_pPreviewDialog->InvalidateGadget(_R(IDC_REDRAW_ME));
						m_DisplayListOfBitmaps = FALSE;
					}
					{
						BOOL Valid = FALSE;
						BOOL State = GetLongGadgetValue(Msg->GadgetID, 0, 1, NULL, &Valid);
						//if (State)
						//	SetLongGadgetValue(Msg->GadgetID, !State);
						break;
					} */
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
						if(m_SetPreviewDlgToIdleState)
							// Set the Preview dialog to an idle state.
							SetPreviewDialogToIdleState((ReDrawInfoType*) Msg->DlgMsgParam);
						else
							// Render this control.
							RenderControl((ReDrawInfoType*) Msg->DlgMsgParam);												
					}
					break;

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
	}
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;
		switch (Msg->State)
		{
			case DocChangingMsg::DocState::ABOUTTODIE:
			/*{
				if (Msg->pOldDoc == NULL)
				{
					//Stop playing the animation.
					m_DocumentAboutToDie = TRUE;

					// Since the document is about to die, no frame layers will exist.
					m_GeneratedFrameLayers = FALSE;

					// Reset the disposal method
					m_DisposalMethod = GDM_BACKTOBACK;
					m_CurrentDisposalMethod = GDM_BACKTOBACK;
				
					// End the dialog
					// EndDialog = TRUE;						// Flag to close and end
				}
				break;
			}*/
			case DocChangingMsg::DocState::SELCHANGED:
			case DocChangingMsg::DocState::BORN:
			/*{
				// called when new is used,thereafter called when no docs. are around.
				if (Msg->pNewDoc == NULL)
				{
					SetDialogToIdleState();

					// End the dialog
					//EndDialog = TRUE;						// Flag to close and end
				}
				// There is an active doucment around.
				else
				{
					SetDialogToIdleState();
				}
				break;
			}*/
				EndDialog = TRUE;
			break;
		}
	}

	// Allow the base class access to the message, it will do the
	// DLG_EAT_IF_HUNGRY(Msg) for us
	// Must do this before the Close and End
	Result = DialogOp::Message(Message);

	// End dialog here
	if (EndDialog) 
	{
		Close();				// Hide the dialog box
		End();					// Finish the operation
	}

	// The message was for our dialog box so return that we have handled it, if necessary
	//return (DLG_EAT_IF_HUNGRY(Msg)); 
	return Result;
}  

/******************************************************************************************

>	BOOL PreviewDialog::SetDialogToIdleState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/97
	Returns:	TRUE if successful, else FALSE
	Purpose:	Ensures that the the dialog is set into an idle state showing a cross in
				the preview section.

******************************************************************************************/

BOOL PreviewDialog::SetDialogToIdleState()
{
	// Stop playing the animation.
	m_DocumentAboutToDie = TRUE;
	m_PlayAnimation = FALSE;
	
	// Set the Preview dialog to an idle state.
	m_SetPreviewDlgToIdleState = TRUE;

	// Since the document has changed no frame layers will exist.
	// They will require generating.
	m_GeneratedFrameLayers = FALSE;

	// Reset the disposal methods
	m_DisposalMethod = GDM_BACKTOBACK;
	m_CurrentDisposalMethod = GDM_BACKTOBACK;

	// SetDlgState(TRUE);

	// Clean the list of bitmaps that we were animating.
	m_BitmapList.DeleteAll();
	m_BitmapListSize			= 0;
	m_CurrentItem				= 0;
	m_SliderPosition			= m_BitmapListSize;
	m_AnimationRect.MakeEmpty();

	// Reset the loop counts
	m_CurrentLoopCount			= 0;	// The loop that we are currently on (default to start)
	m_RequiredLoopCount			= 0;	// The loop count that we must reach (default for ever)

	// Force the gadget to be repainted, 
	InvalidateGadget(_R(IDC_REDRAW_ME));

	return TRUE;
}

/******************************************************************************************

>	BOOL PreviewDialog::InitDialog()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial dialog values 

******************************************************************************************/

BOOL PreviewDialog::InitDialog()
{
	SetGadgetBitmaps(_R(IDC_PREVIEW_SLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
	INT32 StepValue = 100;
	if (m_BitmapListSize > 0)
		StepValue = 100/m_BitmapListSize;

	// Set the range of the slider control plus the step value.  
	SetGadgetRange(_R(IDC_PREVIEW_SLIDER), 1, m_BitmapListSize, StepValue);	
	SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), m_BitmapListSize - m_CurrentItem);

	// Find the correct bitmap to display.						
	UINT32 BitmapNum = (m_BitmapListSize - 1);
	SelectCurrentBitmap(BitmapNum);
	
	// Set up the bitmaps for the play controls
	// Uses the title defined in the rc file so do not specify any bitmaps
	SetGadgetBitmaps(_R(IDC_PREVIEW_PLAY), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_PREVIEW_STOP), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_PREVIEW_NEXTFRAME), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_PREVIEW_PREVFRAME), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_PREVIEW_STARTFRAME), NULL, NULL);
	SetGadgetBitmaps(_R(IDC_PREVIEW_ENDFRAME), NULL, NULL);

	// Reset the loop count that we are currently on
	m_CurrentLoopCount = 0;

	// Are we in single bitmap mode?
	if (m_BitmapListSize == 1)
	{
		// Single bitmap mode

		// Enable the play button = grab all frames and play
		if (viaBitmapGallery)
		{
			EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);//TRUE);

			// Set the title of the dialog to "Preview single bitmap"
			String_256 title(_R(IDS_PREVIEWTITLE_SINGLEBITMAP));
			SetTitlebarName(&title); 

			// change the name field from saying "Frame:" to "Image:"
			String_32 Name(_R(IDS_PREVIEW_FRAMENAMETYPESTR));
			SetStringGadgetValue(_R(IDC_PREVIEW_FRAMENAMETYPE), &Name);
		}
		else
		{
			EnableGadget(_R(IDC_PREVIEW_PLAY), TRUE);

			// Set the title of the dialog to "Preview single frame"
			String_256 title(_R(IDS_PREVIEWTITLE_SINGLE));
			SetTitlebarName(&title); 
		}

		// disable all the other controls
		EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
		EnableGadget(_R(IDC_PREVIEW_NEXTFRAME), FALSE);
		EnableGadget(_R(IDC_PREVIEW_PREVFRAME), FALSE);

		// Disable the slider
		EnableGadget(_R(IDC_PREVIEW_SLIDER), FALSE);

		// Disable the refresh button
		EnableGadget (_R(IDC_PREVIEW_REFRESH), FALSE);

		m_PlayAnimation = FALSE;
	}
	else
	{
		// No, in multiple bitmap mode

		if (viaBitmapGallery)
		{
			// Set the title of the dialog to "Preview multiple images"
			String_256 title(_R(IDS_PREVIEWTITLE_MULTIPLEBITMAP));
			SetTitlebarName(&title);

			// change the name field from saying "Frame:" to "Image:"
			String_32 Name(_R(IDS_PREVIEW_FRAMENAMETYPESTR));
			SetStringGadgetValue(_R(IDC_PREVIEW_FRAMENAMETYPE), &Name);
		}
		else
		{
			// Set the title of the dialog to "Preview all frames"
			String_256 title(_R(IDS_PREVIEWTITLE_MULTIPLE));
			SetTitlebarName(&title); 
		}

		// Disable the Play buttton. Enable the Stop buttton.
		EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);
		EnableGadget(_R(IDC_PREVIEW_STOP), TRUE);

		// Enable the slider
		EnableGadget(_R(IDC_PREVIEW_SLIDER), TRUE);

		// although I would like to do this here, viaBitmapGallery has not been set yet
		// so instead we do this within ToggleViaBitmapGallery ()
		
		if (viaBitmapGallery)
		{
			EnableGadget (_R(IDC_PREVIEW_REFRESH), FALSE);
			SetPlayAnimation (TRUE);
		}
	}

/*	SetGadgetBitmaps(_R(IDC_PREVIEW_PLAY), _R(IDB_PREVIEW_PLAY), _R(IDB_PREVIEW_PLAY));
	SetGadgetBitmaps(_R(IDC_PREVIEW_STOP), _R(IDB_PREVIEW_STOP), _R(IDB_PREVIEW_STOP));
	SetGadgetBitmaps(_R(IDC_PREVIEW_NEXTFRAME), _R(IDB_PREVIEW_NEXT), _R(IDB_PREVIEW_NEXT));
	SetGadgetBitmaps(_R(IDC_PREVIEW_PREVFRAME), _R(IDB_PREVIEW_PREVIOUS), _R(IDB_PREVIEW_PREVIOUS));
	SetGadgetBitmaps(_R(IDC_PREVIEW_STARTFRAME), _R(IDB_PREVIEW_START), _R(IDB_PREVIEW_START));
	SetGadgetBitmaps(_R(IDC_PREVIEW_ENDFRAME), _R(IDB_PREVIEW_END), _R(IDB_PREVIEW_END)); */

	// Set up the frame/layer name control
	String_32 BlankName(TEXT(""));
	SetStringGadgetValue(_R(IDC_PREVIEW_FRAMENAME), &BlankName);

	return TRUE;
}

void PreviewDialog::ToggleViaBitmapGallery (BOOL val)
{
	viaBitmapGallery = val;//TRUE;

/*	if (viaBitmapGallery)
	{
//		EnableGadget (_R(IDC_PREVIEW_REFRESH), FALSE);
//		m_Delay = 100;								// lets make the frames tick slower ....
	}
	else
	{
		EnableGadget (_R(IDC_PREVIEW_REFRESH), TRUE);
	}*/
}


/******************************************************************************************

>	BOOL PreviewDialog::ReInitDialog()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Ranbir code which was in OpGrabFrame::SetFullBitmapList
	Created:	17/6/97
	Returns:	TRUE if successful, else FALSE
	Purpose:	Will reset the initial dialog values. Should be used on an already open
				dialog box.

******************************************************************************************/

BOOL PreviewDialog::ReInitDialog()
{
	if (GetDlgState())
	{
		SetDocumentAboutToDie(FALSE);
		SetDlgState(FALSE);
		// Set the correct state of the controls.
	}

	// Reset the disposal method
	m_DisposalMethod = GDM_BACKTOBACK;
	m_CurrentDisposalMethod = GDM_BACKTOBACK;

	// Reset the loop count that we are currently on
	m_CurrentLoopCount = 0;

	// Is there more than one bitmap in the animation?
	INT32	BitmapListSize = GetBitmapListSize();
	if (BitmapListSize == 1)
	{
		// Set the title of the dialog to "Preview single frame"
		String_256 title(_R(IDS_PREVIEWTITLE_SINGLE));
		SetTitlebarName(&title); 

		// Enable the play button = grab all frames and play
		EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);//TRUE);

		// Disable all the other controls
		EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
		EnableGadget(_R(IDC_PREVIEW_NEXTFRAME), FALSE);
		EnableGadget(_R(IDC_PREVIEW_PREVFRAME), FALSE);

		// Disable the slider
		EnableGadget(_R(IDC_PREVIEW_SLIDER), FALSE);

		// Disable the refresh button
		EnableGadget (_R(IDC_PREVIEW_REFRESH), FALSE);

		// A single frame layer has been generated.
		SetGeneratedFrameLayers(FALSE);
	}
	else
	{
		// Set the title of the dialog to "Preview all frames"
		String_256 title(_R(IDS_PREVIEWTITLE_MULTIPLE));
		SetTitlebarName(&title); 

		// Enable the slider
		EnableGadget(_R(IDC_PREVIEW_SLIDER), TRUE);

		if (viaBitmapGallery)
		{
			EnableGadget (_R(IDC_PREVIEW_REFRESH), FALSE);
			SetPlayAnimation (TRUE);
		}

		// Is the animation playing.
		if (GetPlayAnimation())
		{
			// Set the correct state of the controls.
			EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);
			EnableGadget(_R(IDC_PREVIEW_STOP), TRUE);
			EnableGadget(_R(IDC_PREVIEW_NEXTFRAME), TRUE);
			EnableGadget(_R(IDC_PREVIEW_PREVFRAME), TRUE);

			// A document change will stop the timer, therefore re-start it.
			RestartTimer();					
		}
		else
		{
			// Set the correct state of the controls.
			EnableGadget(_R(IDC_PREVIEW_PLAY), TRUE);
			EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
			EnableGadget(_R(IDC_PREVIEW_NEXTFRAME), TRUE);
			EnableGadget(_R(IDC_PREVIEW_PREVFRAME), TRUE);
		}

		// Set the correct state of the dlg.
		// Ensure that the timer does not get stopped after we restarted it as it thinks
		// that either the animation properties have changed or the document is about to die
		SetDlgState(FALSE);
		SetDocumentAboutToDie(FALSE);
		SetGeneratedFrameLayers(TRUE);
		SetAnimationPropertiesChanged(FALSE);
	}

	// If there is only a single layer, set the slider to the end.
	m_SliderPosition = m_BitmapListSize;
	SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), m_BitmapListSize);
	
	// Since the dialog is modeless, the user can add extra layers,
	// therefore reset the step value of the slider.
	INT32 Step = 100/ m_BitmapListSize;
	SetGadgetRange(_R(IDC_PREVIEW_SLIDER), 1, m_BitmapListSize, Step);
	SetGadgetBitmaps(_R(IDC_PREVIEW_SLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));

	// Find the correct bitmap to display.						
	//if (m_SliderPosition >= m_BitmapListSize)
	//	m_SliderPosition = 0;
	UINT32 BitmapNum = m_BitmapListSize - m_SliderPosition;
	SelectCurrentBitmap(BitmapNum);

	// Force the gadget to be repainted, 
	// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
	InvalidateGadget(_R(IDC_REDRAW_ME));

	// Set up the frame/layer name control
	String_32 BlankName(TEXT(""));
	SetStringGadgetValue(_R(IDC_PREVIEW_FRAMENAME), &BlankName);

	return TRUE;
}

/********************************************************************************************

>	BOOL PreviewDialog::SelectCurrentFrame(const UINT32 Position)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/97
	Inputs:		The frame to be selected (0 .. max frames - 1)
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to set the current frame to be displayed and ensure that associated
				items are set e.g. the slider.

********************************************************************************************/

BOOL PreviewDialog::SelectCurrentFrame(const UINT32 Position)
{
	INT32 NewPosition = (INT32)Position;
	// check if the requested position is beyond the end of the list
	if (NewPosition >= m_BitmapListSize)
		NewPosition = 0;
	
	// Set the current selection to be correct
	SelectCurrentBitmap(NewPosition);

	// Set the slider position REM slider works backwards.
	m_SliderPosition = m_BitmapListSize - NewPosition;
	SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), m_SliderPosition);

	// Force the main display gadget to be repainted, 
	// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
	InvalidateGadget(_R(IDC_REDRAW_ME));

	return TRUE;
}

/********************************************************************************************

>	BOOL PreviewDialog::StopPreviewing()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Puts the dialog into a stopped state ready to grab the frames or frame.

********************************************************************************************/

BOOL PreviewDialog::StopPreviewing()
{
	// Stop the animation.
	PreviewDlgStop();

	// Force the gadget to be repainted, 
	// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
	InvalidateGadget(_R(IDC_REDRAW_ME));

	// Reset the loop count that we are currently on
	m_CurrentLoopCount = 0;

	return TRUE;
}

/********************************************************************************************

>	void PreviewDialog::RenderControl(ReDrawInfoType* pExtraInfo)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		pExtraInfo - The structure that has the extra data we need to start rendering
	Purpose:	Renders the current bitmap into the control. This actually consists of:-
					Render slabbed in border
					Render items before present bitmap in list to set up background
					Render current bitmap

********************************************************************************************/

void PreviewDialog::RenderControl(ReDrawInfoType* pExtraInfo)
{
	if (pExtraInfo == NULL)
		return;
	
	// Go get a render region
	DocRect VirtualSize(0, 0, pExtraInfo->dx, pExtraInfo->dy);
	DocRect WholeRedrawSize = VirtualSize;
	
	// If we haven't already created our render region then go and create it
	BOOL StartedOk = TRUE;
	if (m_pRender == NULL)
	{
		// This will call start render for us
		m_pRender = CreateGRenderRegion(&VirtualSize, pExtraInfo);
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

		GridLockRect(&VirtualSize, PixelSize);

		// Render the attributes and then a rectangle
		m_pRender->SaveContext();

		// Get the current bitmap and try and display it
		KernelBitmap * pBitmapToUse = GetCurrentBitmap();
		if (pBitmapToUse)
		{
			// Using this method we don't get flicker on the backdrop as we draw the background rectangle
			// followed by the white new background
			// Draw single pixel black line left and top and single pixel background colour 1 pixel in
			m_pRender->SetLineWidth(0);
			m_pRender->SetFillColour(RedrawColours.DialogBack());
			m_pRender->SetLineColour(COLOUR_BLACK); //RedrawColours.ButtonShadow());
			m_pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.hi.y - PixelSize),
								DocCoord(VirtualSize.hi.x, VirtualSize.hi.y - PixelSize));
			m_pRender->DrawLine(DocCoord(VirtualSize.lo.x, VirtualSize.hi.y),
								DocCoord(VirtualSize.lo.x, VirtualSize.lo.y));
			m_pRender->SetLineColour(RedrawColours.DialogBack());
			m_pRender->DrawLine(DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.hi.y - 2 * PixelSize),
								DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.hi.y - 2 * PixelSize));
			m_pRender->DrawLine(DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.lo.y),
								DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.hi.y - 2 * PixelSize));

			// Draw single pixel hightligh line right and bottom and single pixel background colour 1 pixel in
			m_pRender->SetLineColour(RedrawColours.ButtonHighlight());
			m_pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.hi.y - PixelSize),
								DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y));
			m_pRender->DrawLine(DocCoord(VirtualSize.hi.x - PixelSize, VirtualSize.lo.y),
								DocCoord(VirtualSize.lo.x + PixelSize, VirtualSize.lo.y));

			m_pRender->SetLineColour(RedrawColours.DialogBack());
			m_pRender->DrawLine(DocCoord(VirtualSize.hi.x - 2 * PixelSize, VirtualSize.hi.y - 2 * PixelSize),
								DocCoord(VirtualSize.hi.x - 2 * PixelSize, VirtualSize.lo.y + 1 * PixelSize));
			m_pRender->DrawLine(DocCoord(VirtualSize.hi.x - 2 * PixelSize, VirtualSize.lo.y + 1 * PixelSize),
								DocCoord(VirtualSize.lo.x + 2 * PixelSize, VirtualSize.lo.y + 1 * PixelSize));

			// And deflate the rect by 2 pixels
			VirtualSize.Inflate(-PixelSize * 2);

			// Ensure that we cannot redraw into the border that we have just redrawn
//			m_pRender->SetClipRect(VirtualSize);

			// Code stolen from BfxDlg::RenderBitmap
			m_pRender->SetLineColour(COLOUR_TRANS);

			// If we are playing then assume we need the quickest redraw and we will
			// be overlaying the correct frames.
/*			if (m_PlayAnimation)
			{
				// Check the GIFDisposalMethod for the previous bitmap. If necessary redraw the background.
				if (m_DisposalMethod == GDM_BACKTOBACK)
				{
					// Draw a rectangle to fill in the background - Fill with Dialogue Background colour
					//m_pRender->SetFillColour(RedrawColours.DialogBack()); // COLOUR_WHITE); //
					m_pRender->SetFillColour(COLOUR_WHITE); 
					m_pRender->DrawRect(&VirtualSize);
				}
				// This is REDRAW code and so noting the redraw method is very bad.
				// Should be done in the code which moves between frames.
				//// Note the disposal method for this bitmap for the next time around
				//m_DisposalMethod = pBitmapToUse->GetAnimationRestoreType();

				// redraw the specified bitmap into the render region
				RedrawBitmap(pBitmapToUse, &VirtualSize);
			}
			else */
			{
				// Try just redrawing everything we need to

				// Not playing so we must redraw everything that is required to make up
				// this frame. If the previous frame is restore background then we just
				// need to redraw this frame. If the previous frame is anything else then
				// we need to move back through the frames until we find the start or a
				// frame with restore background and then draw from this point.
				// If we encounter a bitmap which is restore previous and it is not this
				// one then we need do nothing.

				// Draw a rectangle to fill in the background
				m_pRender->SetFillColour(COLOUR_WHITE); 
				m_pRender->DrawRect(&VirtualSize);

				INT32 ThisIndex = m_CurrentItem;
				INT32 StartIndex = m_CurrentItem;
				BOOL StartFrameFound = FALSE;
				// First, search out the first frame to redraw
				while (!StartFrameFound && StartIndex > 0)
				{
					// Move to the previous item in the list
					StartIndex--;
					KernelBitmap * pBitmap = GetBitmapItem(StartIndex);
					// Check the GIFDisposalMethod for the previous bitmap.
					if (pBitmap == NULL)
					{
						// No previous frame so stop the search now
						StartFrameFound = TRUE;
						StartIndex++;
					}
					else if (pBitmap->GetAnimationRestoreType() == GDM_BACKTOBACK)
					{
						// Its a restore background so we have found our first frame
						// to redraw, its StartIndex + 1
						StartFrameFound = TRUE;
						StartIndex++;
					}
				}

				// I'm not sure why they were originally doing this, BUT I've taken it out
				// because it was causing problems with my new stuff ....

				// Now redraw all the bitmaps from the start frame onwards
				// up to the frame before the current one
		/*		for (INT32 i = StartIndex; i < ThisIndex; i++)
				{
					KernelBitmap * pBitmap = GetBitmapItem(i);
					if (pBitmap && pBitmap->GetAnimationRestoreType() != GDM_PREVIOUS)
					{
						RedrawBitmap(pBitmap, &VirtualSize);
					}
				}*/

				// Lastly, redraw the present bitmap into the render region
				RedrawBitmap(pBitmapToUse, &VirtualSize);
			}
		}
		else
		{
			// draw a cross to show we do not have a bitmap
			RedrawCross(&VirtualSize, &RedrawColours, PixelSize);
		}

		// Now that we have drawn everything, reset the cliprect back to the whole
		// size, otherwise it seems to stop redrawing the border.
		m_pRender->SetClipRect(WholeRedrawSize);

		m_pRender->RestoreContext();

		// Get rid of the render region, now done in the destructor
		//DestroyGRenderRegion(m_pRender);
		
		// Blit to the screen
		m_pRender->StopRender();
	}
}

/*******************************************************************************************

>	BOOL PreviewDialog::RedrawBitmap(KernelBitmap * pBitmapToUse, DocRect * pVirtualSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/97
	Inputs:		pBitmap				the bitmap to redraw
				pVirtualSize		the size of the area to redraw into
	Purpose:	Redraws the specified bitmap into the render region.

*******************************************************************************************/

BOOL PreviewDialog::RedrawBitmap(KernelBitmap * pBitmapToUse, DocRect * pVirtualSize)
{
	// If we haven't got a render region or bitmap then give up now
	// No errors as we are in redraw code
	if (m_pRender == NULL || pBitmapToUse == NULL || pVirtualSize == NULL)
		return FALSE;

	// Ensure that we cannot redraw into the border that we have just redrawn
	m_pRender->SetClipRect(*pVirtualSize);

	NodeBitmap NB;
	NodeBitmap * pNodeBitmap = &NB;

	if (!((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12))))
	{
		// Get a new bitmap object for this node.
		pNodeBitmap->GetBitmapRef()->SetBitmap(pBitmapToUse);
		ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");

		DocRect BitmapSize(*pVirtualSize);

		MILLIPOINT Width = pBitmapToUse->GetRecommendedWidth();
		MILLIPOINT Height = pBitmapToUse->GetRecommendedHeight();
		if (Width < 1) Width = 1;
		if (Height < 1) Height = 1;

		UINT32 LeftOffset = pBitmapToUse->GetLeftOffset();
		UINT32 TopOffset = pBitmapToUse->GetTopOffset();
		UINT32 HDpi = pBitmapToUse->GetHorizontalDPI();
		UINT32 VDpi = pBitmapToUse->GetVerticalDPI();
		MILLIPOINT LeftOffsetMP = ((LeftOffset * 72000)/HDpi);
		MILLIPOINT TopOffsetMP = ((TopOffset * 72000)/VDpi);

		// Plot bitmap 1:1 with no scaling
		// Try to centralise the image inside the available width
		// Otherwise
		// Plot the bitmap from the top left of the redraw area using
		// the offsets stored in the bitmap
		MILLIPOINT FullWidth = m_AnimationRect.Width();
		if (FullWidth < pVirtualSize->Width())
		{
			MILLIPOINT diff = pVirtualSize->Width() - FullWidth;
			BitmapSize.lo.x = pVirtualSize->lo.x + diff/2 + LeftOffsetMP;
			BitmapSize.hi.x = BitmapSize.lo.x + Width;
		}
		else
		{
			BitmapSize.lo.x = pVirtualSize->lo.x + LeftOffsetMP;
			BitmapSize.hi.x = BitmapSize.lo.x + Width;
		}

		// Try to centralise the image inside the available height
		MILLIPOINT FullHeight = m_AnimationRect.Height();
		if (FullHeight < pVirtualSize->Height())
		{
			MILLIPOINT diff = pVirtualSize->Height() - FullHeight;
			BitmapSize.hi.y = pVirtualSize->hi.y - diff/2 - TopOffsetMP;
			BitmapSize.lo.y = BitmapSize.hi.y - Height;
		}
		else
		{
			BitmapSize.hi.y = pVirtualSize->hi.y - TopOffsetMP;
			BitmapSize.lo.y = BitmapSize.hi.y - Height;
		}

		// If the bitmap is bigger than the size we have available then
		// the nodebitmap will render into the slabbed in region on the right
		// hand side. So set the clipping rectangle to be the size we have
		// available to us i.e. the inside of the slabbed in area.
		// Remember, NodeBitmaps has the bitmap applied to fill the entire shape
		// and so the NodeBitmap must be the size of the bitmap to fill 1:1.
		//m_pRender->SetClipRect(*pVirtualSize);
		// Done in the caller now

		// And set this in our bitmap node
		pNodeBitmap->CreateShape(BitmapSize);
			
		pNodeBitmap->Render(m_pRender);
	}

	return TRUE;
}


/*******************************************************************************************

>	BOOL PreviewDialog::RedrawCross(DocRect * pVirtualSize, DialogColourInfo * pRedrawColours,
									INT32 PixelSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/97
	Inputs:		pVirtualSize		the size of the area to redraw into
				pRedrawColours		a default dialog colour supplier object
				PixelSize			the size of a pixel on the redraw area
	Purpose:	Redraws the specified bitmap into the render region.

*******************************************************************************************/

BOOL PreviewDialog::RedrawCross(DocRect * pVirtualSize, DialogColourInfo * pRedrawColours,
								INT32 PixelSize)
{
	// If we haven't got a render region then give up now
	// No errors as we are in redraw code
	if (m_pRender == NULL || pVirtualSize == NULL || pRedrawColours == NULL)
		return FALSE;

	// Draw a backgound making sure we blat anything previously there with the dialog
	// background colour
	m_pRender->SetLineWidth(0);
	m_pRender->SetFillColour(pRedrawColours->DialogBack());
	m_pRender->SetLineColour(COLOUR_BLACK); //RedrawColours.ButtonShadow());
	m_pRender->DrawRect(pVirtualSize);
	m_pRender->DrawLine(DocCoord(pVirtualSize->lo.x, pVirtualSize->hi.y), pVirtualSize->hi);

	m_pRender->SetLineWidth(0);
	m_pRender->SetLineColour(pRedrawColours->ButtonHighlight());
	m_pRender->DrawLine(DocCoord(pVirtualSize->hi.x - PixelSize, pVirtualSize->hi.y - PixelSize),
						DocCoord(pVirtualSize->hi.x - PixelSize, pVirtualSize->lo.y));
	m_pRender->DrawLine(DocCoord(pVirtualSize->hi.x - PixelSize, pVirtualSize->lo.y + PixelSize),
						DocCoord(pVirtualSize->lo.x + PixelSize, pVirtualSize->lo.y + PixelSize));

	// Draw the no bitmap present cross i.e. vertical
	// And Deflate the rect by 1 pixels so we draw to the outer lines
	DocRect InnerSize(*pVirtualSize);
	InnerSize.Inflate(-PixelSize);
	INT32 Width = InnerSize.Width();
	INT32 Height = InnerSize.Height();
	m_pRender->SetLineWidth(0);
	m_pRender->SetLineColour(COLOUR_BLACK); //RedrawColours.ButtonShadow());
	m_pRender->DrawLine(DocCoord(InnerSize.lo.x + Width/2, InnerSize.lo.y),
						DocCoord(InnerSize.lo.x + Width/2, InnerSize.hi.y));
	m_pRender->DrawLine(DocCoord(InnerSize.lo.x, InnerSize.lo.y + Height/2),
						DocCoord(InnerSize.hi.x, InnerSize.lo.y + Height/2));

	return TRUE;
}

/********************************************************************************************

>	inline INT32 PreviewDialog::HalfGridLock(INT32 Position, INT32 GridSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Jason code in coldlog.cpp
	Created:	28/7/97
	Inputs:		Position - The X/Y position, in millipoints
				GridSize - the size of the grid to lock to, in millipoints
	Returns:	Position, locked (by rounding) to a grid of the given size, and offset by
				half a grid.
	Purpose:	Grid-locks a given plotting poisition to lie over a grid (usually the
				output device-pixel grid). The result is shifted by half the grid size to
				grid-lock to the _center_ of a pixel rather than the edges.
	Note:		stolen from ColourEditDlg rendering routines

********************************************************************************************/

inline INT32 PreviewDialog::HalfGridLock(INT32 Position, INT32 GridSize)
{
	// By truncating down to the nearest grid point, and adding half the grid value,
	// we achieve rounding to the nearest offset-grid position.	

	// NOTE:
	// The original algorithm incorrectly rounded negative numbers towards
	// zero. Negative numbers should be rounded towards negative infinity.
	// The algorithm has been corrected by always rounding a positive number
	// and restoring the original sign of the number after rounding.

	BOOL bNegative = FALSE;				// Assume the number is positive

	if (Position < 0)					// If the number if not positive
	{									// note the fact and make positive
		bNegative	= TRUE;
		Position	= -Position;
	}

	Position -= Position % GridSize;
	Position += GridSize / 2;

	if (bNegative)						// If the number was negative
		Position = -Position;			// restore the sign

	return (Position);
}



/********************************************************************************************

>	void PreviewDialog::GridLockRect(DocRect *TheRect, INT32 GridSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Jason code in coldlog.cpp
	Created:	28/7/97
	Inputs:		TheRect - A MILLIPOINT DocRect to be grid-locked
				GridSize - the size of the grid to lock to, in millipoints
	Outputs:	TheRect is updated as appropriate
	Purpose:	GridLocks all 4 coordinates of the rectangle using HalfGridLock
	Note:		stolen from ColourEditDlg rendering routines

********************************************************************************************/

void PreviewDialog::GridLockRect(DocRect *TheRect, INT32 GridSize)
{
	TheRect->lo.x = HalfGridLock(TheRect->lo.x, GridSize);
	TheRect->lo.y = HalfGridLock(TheRect->lo.y, GridSize);
	TheRect->hi.x = HalfGridLock(TheRect->hi.x, GridSize);
	TheRect->hi.y = HalfGridLock(TheRect->hi.y, GridSize);
}

/*******************************************************************************************

>	Layer * PreviewDialog::GetCurrentLayer()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Purpose:	Returns the current layer

*******************************************************************************************/

Layer * PreviewDialog::GetCurrentLayer()
{
	// If beyond the end of the list then wrap around to the start
	if (m_CurrentItem >= m_BitmapListSize)
		m_CurrentItem = 0;

	// get the item associated with this index
	return GetLayerItem(m_CurrentItem); 
}

/*******************************************************************************************

>	Layer * PreviewDialog::GetLayerItem(INT32 Index)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Inputs:		Index	the index of the layer in the list to get
	Purpose:	Returns the specified layer

*******************************************************************************************/

Layer * PreviewDialog::GetLayerItem(INT32 Index)
{
	// If beyond the end of the list or the start then return nothing found
	if (Index >= m_BitmapListSize || Index < 0)
		return NULL;

	// Search through the list for the nth item
	AnimatedBitmapItem * pCurrentItem = (AnimatedBitmapItem *)m_BitmapList.GetHead();
	INT32 Count = 0;
	while (pCurrentItem)
	{
		if (Count == Index)
		{
			return pCurrentItem->GetLayer();
		}

		Count ++;

		// Get the next item in the list
		pCurrentItem = (AnimatedBitmapItem *)m_BitmapList.GetNext(pCurrentItem);
	}

	// Nothing found
	return NULL;
}

/*******************************************************************************************

>	KernelBitmap * PreviewDialog::GetBitmap(INT32 Index)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/97
	Inputs:		Index	the index of the bitmap in the list to get
	Purpose:	Returns the specified bitmap

*******************************************************************************************/

KernelBitmap * PreviewDialog::GetBitmapItem(INT32 Index)
{
	// If beyond the end of the list or the start then return nothing found
	if (Index >= m_BitmapListSize || Index < 0)
		return NULL;

	// Search through the list for the nth item
	AnimatedBitmapItem * pCurrentItem = (AnimatedBitmapItem *)m_BitmapList.GetHead();
	INT32 Count = 0;
	while (pCurrentItem)
	{
		if (Count == Index)
		{
			return pCurrentItem->GetBitmap();
		}

		Count ++;

		// Get the next item in the list
		pCurrentItem = (AnimatedBitmapItem *)m_BitmapList.GetNext(pCurrentItem);
	}

	// Nothing found
	return NULL;
}

/*******************************************************************************************

>	KernelBitmap * PreviewDialog::GetCurrentBitmap(BOOL GetDetails = TRUE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Returns the current bitmap which we should be using

*******************************************************************************************/

KernelBitmap * PreviewDialog::GetCurrentBitmap(BOOL GetDetails)
{
	KernelBitmap * pBitmapToUse = NULL;

	// If beyond the end of the list then wrap around to the start
	if (m_CurrentItem >= m_BitmapListSize)
		m_CurrentItem = 0;

	// Search through the list for the nth item
	AnimatedBitmapItem * pCurrentItem = (AnimatedBitmapItem *)m_BitmapList.GetHead();
	INT32 Count = 0;
	while (pCurrentItem)
	{
		if (Count == m_CurrentItem)
		{
			pBitmapToUse = pCurrentItem->GetBitmap();
			break;
		}

		Count ++;

		// Get the next item in the list
		pCurrentItem = (AnimatedBitmapItem *)m_BitmapList.GetNext(pCurrentItem);
	}

	// Note the disposal method for this bitmap for the next time around
	if (pBitmapToUse && GetDetails)
	{
		m_CurrentDisposalMethod = pBitmapToUse->GetAnimationRestoreType();

		// Set up the frame/layer name control to be the bitmap name
		String_256 Name = pBitmapToUse->GetName();
		SetStringGadgetValue(_R(IDC_PREVIEW_FRAMENAME), &Name);
	}

	return pBitmapToUse;
}


/*******************************************************************************************

>	void PreviewDialog::IncrementCurrentBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Moves the current item to be the next bitmap in the list. 
				If we reach the end	of the list, it then becomes the first item in the list.

*********************************************************************************************/

void PreviewDialog::IncrementCurrentBitmap()
{
	// Local flag to test whether to increment the slider position.
	BOOL SliderPositionReset = FALSE;

	// Increment the Bitmap if the user has clicked either Play or NextFrame.
	if (m_PlayAnimation || m_DisplayNextFrame )
	{
		m_CurrentItem++;

		// Moving forward so note the current disposal method as the new one
		m_DisposalMethod = m_CurrentDisposalMethod;

		// Only activate the slider if we have more than 1 bitmap.
		if(m_BitmapListSize > 1)
		{
			// Decrement the slider.
			m_SliderPosition--;

			// If the slider is set to the end, place it back at the start.
			if (m_SliderPosition <= 0)
				m_SliderPosition = m_BitmapListSize;	
		}	

		// Set the slider to the correct position.
		SetSliderPosition(m_SliderPosition);
	}

	// If beyond the end of the list then wrap around to the start.
	if (m_CurrentItem >= m_BitmapListSize)
	{
		// We have looped again so update our loop count
		// Only do though if we haven't got a loop forever state
		if (m_RequiredLoopCount <= 0)
			m_CurrentItem = 0;
		else
		{
			m_CurrentLoopCount++;
			if (m_CurrentLoopCount >= m_RequiredLoopCount)
			{
				// We have looped enough so stop the animation
				// Stops the animation and sets the play/stop buttons to the play state
				PreviewDlgStop();
			}
			else
			{
				// Need to loop again so reset to the start of the loop
				m_CurrentItem = 0;
			}
		}
	}

	TRACEUSER( "JustinF", _T("\t-- current frame is %ld\n"), (INT32) m_CurrentItem);		
}

/*******************************************************************************************

>	void PreviewDialog::SelectFirstBitmap()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Purpose:	Selects the first bitmap in our list.

*******************************************************************************************/

void PreviewDialog::SelectFirstBitmap()
{
	// select the first bitmap from our list.
	m_CurrentItem = 0;
}

/***********************************************************************************************

>	void PreviewDialog::SelectLastBitmap()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Purpose:	Selects the last bitmap in our list.

***********************************************************************************************/

void PreviewDialog::SelectLastBitmap()
{
	// select the last bitmap from our list.
	m_CurrentItem = (m_BitmapListSize -1);
}

/***********************************************************************************************

>	void PreviewDialog::SelectCurrentBitmap(UINT32 Position)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		Position - THe position on the slider.
	Outputs:	None
	Purpose:	Moves the current item to where the slider has been positioned. 
				
*************************************************************************************************/
void PreviewDialog::SelectCurrentBitmap(UINT32 Position)
{
	m_CurrentItem = Position;
}

/***********************************************************************************************

>	void PreviewDialog::DecrementCurrentBitmap()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Purpose:	Moves the current item to be the previous bitmap in the list.
				If we reach the start of the list, it then becomes the last item in the list.

*************************************************************************************************/

void PreviewDialog::DecrementCurrentBitmap()
{
	//  Local flag to test whether to increment the slider position.
	BOOL SliderPositionReset = FALSE;

	//  Decrement the bitmap if the user has eitther clicked Play or Previous frame.
	if(m_PlayAnimation || m_DisplayPreviousFrame)
	{
		m_CurrentItem--;
		
		// Moving backwards so do what?????
		m_DisposalMethod = GDM_BACKTOBACK;

		// Activate the slider if we have more than 1 bitmap.
		if(m_BitmapListSize > 1)
		{
			// Increment the slider.
			m_SliderPosition++;

			//	If the slider is at the start, then set it to the end.	
			if(m_SliderPosition >= m_BitmapListSize + 1)
				m_SliderPosition = 1;		
		}

		// Set the slider to the correct position.
		SetSliderPosition(m_SliderPosition);
	}

	// If we reach the start of the list then wrap around to the end.
	if (m_CurrentItem < 0)
		m_CurrentItem = (m_BitmapListSize-1);
}

/*******************************************************************************************

>	void SetSliderPosition(INT32 Position)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		The position at which the slider is to be set.
	Purpose:	Set the slider to its correct position.

******************************************************************************************/
void PreviewDialog::SetSliderPosition(INT32 Position)
{
	if (Position == 0)
		Position = m_BitmapListSize;
		
	SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), Position);
}

/*************************************************************************************************************

>	BOOL PreviewDialog::SetRegeneratedBitmapInList(GIFAnimationExportParam* pParam, UINT32 BitmapNumber)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		pParam - GIF Export Options
				BitmapNumber - A reference to the bitmap which will be replaced with our regenerated bitmap.
	Otutputs	-
	REturns		TRUE if successful, else FALSE.
	Purpose:	If we need to regenerate a SINGLE bitmap while the Preview dialog is open, 
				this function allows us to update our Bitmap list.				

***************************************************************************************************************/

BOOL PreviewDialog::SetRegeneratedBitmapInList(GIFAnimationExportParam* pParam)
{
	if (pParam == NULL)
		ERROR2RAW("PreviewDialog::SetRegeneratedBitmapInList bad params");

	// For the regenerated bitmap, retrieve it's position in the list. 
	UINT32 BitmapNumber = pParam->GetRegeneratedBitmapPosition();
	UINT32 BitmapCount = pParam->GetBitmapCount();

	ERROR3IF(BitmapCount != 1, "BitmapCount specifid an illegal value");

	// Ensure that a reference to this bitmap will exist.
	DWORD Count = m_BitmapList.GetCount();

	if (BitmapNumber > Count - 1)
		return FALSE;

	// 	Find the first bitmap in the list.
	AnimatedBitmapItem* pList = (AnimatedBitmapItem*) m_BitmapList.GetHead();

	if (!pList) 
		return FALSE;

	// Loop counter.
	UINT32 Num = 0;
 
	// Find the bitmap corresponding to BitmapNumber.
	while (Num != BitmapNumber)
	{
		pList = (AnimatedBitmapItem*) m_BitmapList.GetNext(pList);
		Num++;
	}

	// Find the previous bitmap in the list.
	BOOL ListHead = FALSE;
	AnimatedBitmapItem * pPrev = (AnimatedBitmapItem*) m_BitmapList.GetPrev(pList);

	if (!pPrev)
	{
		//This must be the head of the list.
		ListHead = TRUE;
	}

	// Remove the bitmap corresponding to BitmapNumber from the list.
		m_BitmapList.RemoveItem(pList);

	// Retrieve the regenerated bitmap from GIFAnimationExportParam.
	KernelBitmap* pRegeneratedBitmap = pParam->GetBitmap(0);
	Layer * pLayer = pParam->GetLayer(0);

	if (pRegeneratedBitmap)
	{
		AnimatedBitmapItem * pNewItem = new AnimatedBitmapItem(pRegeneratedBitmap, pLayer);
		if (pNewItem)
		{
			// Add this bitmap to our list.
			if(ListHead)
				m_BitmapList.AddHead(pNewItem);
			else
				m_BitmapList.InsertAfter(pPrev, pNewItem);
		}	
	}

	return TRUE;
}

/*******************************************************************************************

>	BOOL PreviewDialog::SetBitmapList(BitmapExportParam* pParam, BOOL SetSlider = TRUE)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		pParam		- GIF Export options
				SetSlider	- True if want the slider position to be set (Default)
	Outputs		-
	Returns		TRUE if successful, else FALSE.
	Purpose:	If we need to regenerate our bitmaps while the Preview dialog is open,
				this function allows us to update our list.				

*******************************************************************************************/

BOOL PreviewDialog::SetBitmapList(BitmapExportParam* pParam, BOOL SetSlider)
{
	if (pParam == NULL)
			ERROR2RAW("PreviewDialog::SetBitmapList bad params");

	// Clean the list of bitmaps that we were animating.
	m_BitmapList.DeleteAll();

	// Store the bitmaps which have been passed to us.
	m_BitmapListSize = pParam->GetBitmapCount();

	if (m_BitmapListSize == 0)
		ERROR2RAW("PreviewDialog::SetBitmapList no bitmaps to preview");
	
	KernelBitmap* pTheBitmap = NULL;
	Layer * pLayer = NULL;
	MILLIPOINT MaxWidth = 0;
	MILLIPOINT MaxHeight = 0;
	for (INT32 i = 0; i < m_BitmapListSize; i++)
	{
		pTheBitmap = pParam->GetBitmap(i);
		pLayer = pParam->GetLayer(i);

		if (pTheBitmap)
		{
			AnimatedBitmapItem * pNewItem = new AnimatedBitmapItem(pTheBitmap, pLayer);
			if (pNewItem)
			{
				// Add this item to our list.
				m_BitmapList.AddTail(pNewItem);

				MILLIPOINT Width = pTheBitmap->GetRecommendedWidth();
				MILLIPOINT Height = pTheBitmap->GetRecommendedHeight();
				if (Width > MaxWidth)
					MaxWidth = Width;
				if (Height > MaxHeight)
					MaxHeight = Height;
			}
		}
	}

	// Work out the bounding box of the animation and the loop count
	if (pParam->IS_KIND_OF(GIFAnimationExportParam))
	{
		GIFAnimationExportParam * pGIFParam = (GIFAnimationExportParam*)pParam;
		// Get the bounding rect that we have been passed in
		m_AnimationRect = pGIFParam->GetBoundingRect();

		// Get the loop count that we have been passed in
		m_RequiredLoopCount = pGIFParam->GetAnimLoop();
	}
	else
	{
		// Look through all the bitmaps to find the biggest size
		// But we have already done this above, so use it
		m_AnimationRect = DocRect(0, 0, MaxWidth, MaxHeight);

		// Set the loop count to its default of loop forever
		m_RequiredLoopCount = 0;
	}

	// reset the current loop count to zero
	m_CurrentLoopCount = 0;

	// Reset the disposal methods
	m_DisposalMethod = GDM_BACKTOBACK;
	m_CurrentDisposalMethod = GDM_BACKTOBACK;

	if (SetSlider)
	{
		// Set the range of the slider control plus the step value.  
		INT32 StepValue = 100/ m_BitmapListSize;
		SetGadgetRange(_R(IDC_PREVIEW_SLIDER), 1, m_BitmapListSize, StepValue);
	}

	return TRUE;
}

/*******************************************************************************************

>	UINT32 PreviewDialog::GetDelayForCurrentBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Returns:	Delay in centiseconds.
	Purpose:	Returns the delay in centiseconds for the current bitmap.

*******************************************************************************************/

UINT32 PreviewDialog::GetDelayForCurrentBitmap()
{
	if (!viaBitmapGallery)
	{
		UINT32 Delay = 0;
		KernelBitmap * pBitmap = GetCurrentBitmap(FALSE);
		if (pBitmap)
		{
			// Ask the bitmap for its stored delay
			Delay = pBitmap->GetDelay();
			// If the delay specified is zero then use the default
			if (Delay == 0)
				Delay = 10;
		}

		return Delay;
	}
	else
	{
		return (100);		// we wan't a longer delay ....
	}
}


/*******************************************************************************************

>	OpState	PreviewDialog::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Purpose:	Returns the OpState of the Preview dialogue operation

*******************************************************************************************/

OpState	PreviewDialog::GetState(String_256*, OpDescriptor*)
{
	return OpState(FALSE, FALSE);
}
		 

/*******************************************************************************************

>	BOOL PreviewDialog::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a Preview Dialog

*******************************************************************************************/

BOOL PreviewDialog::Init()	
{  
	return RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_PREVIEWDIALOG), 				// String resource ID
								CC_RUNTIME_CLASS(PreviewDialog),// Runtime class
 								OPTOKEN_PREVIEWDIALOG,			// Token string
 								PreviewDialog::GetState,		// GetState function
								_R(IDH_Command_Preview_All_Frames),	// Help ID
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

>	virtual void PreviewDialog::DoWithParam(OpDescriptor* pOpDesc,OpParam* pParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		pOpDesc		The OpDescriptor being invoked
				pParam		The parameters being passed in
	Purpose:	Creates and shows a Preview dialog

*******************************************************************************************/

void PreviewDialog::DoWithParam(OpDescriptor* pOpDesc,OpParam* pParam)
{
	TRACEUSER( "Neville", _T("PreviewDialog::DoWithParam\n"));

	// Check out the parameter that we should be being passed
	if (pParam == NULL)
	{
		ERROR2RAW("OpMenuExport::DoWithParam bad params");
		FailAndExecute();
		End();
		return;
	}
	
	BitmapExportParam* pExportParam = (BitmapExportParam*) pParam;
	ERROR3IF(!pExportParam->IS_KIND_OF(BitmapExportParam), "OpParam passed is not a BitmapExportParam");

	if (pExportParam->GetBitmapCount() == 0)
	{
		ERROR2RAW("OpMenuExport::DoWithParam no bitmaps to preview");
		FailAndExecute();
		End();
		return;
	}				

	// Take a copy of the list of bitmaps passed in
	// Don't set the slider position as we haven't constructed the dialog yet!
	SetBitmapList(pExportParam, FALSE);

	// Has more than one frame layer been generated.
	if(m_BitmapListSize > 1)
		m_GeneratedFrameLayers = TRUE;

	// Force the dialog box to be created.
	// If it is non-modal it must be opened if the create works ok.
	// If it is modal then don't call the open
	if (Create())
	{
		// If dialog is not Modal do this now
		// Otherwise set up happens in the message handler
		if (Mode == MODELESS)
		{
			// Try to resize the dialog box so that the current animation fits
			ResizeDialogToFitAnimation();

			// Set the initial control values 
			InitDialog();

			// If modeless then call open Open()
			Open();
		
			// Set up the call back so we get timer events
			SetUpCallBack();
		}
	}
	else
	{
		TRACE( _T("Failed to create Render Demo Dialog\n"));
		// Could not create the dialog box so call inform error 
		InformError();						// Show user the error
  		End(); 		   						// End the operation 
	}

	return;
}

/*******************************************************************************************

>	BOOL PreviewDialog::ResizeDialogToFitAnimation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to resize the dialog to fit the animation in the preview control

*******************************************************************************************/

BOOL PreviewDialog::ResizeDialogToFitAnimation()
{
	// Clean out the render region, if allocated, otherwise redraws may be funny
	// after the resize
	if (m_pRender)
	{
		// Delete the render region and its (dialog) view
		DestroyGRenderRegion(m_pRender);
		m_pRender = NULL;
	}
	
	// Get the handle of the dialog box. This is really an HWND.
	CWindowID hWnd = GetReadWriteWindowID();

	RECT RedrawRect;
	if (!GetGadgetPosition(_R(IDC_REDRAW_ME), &RedrawRect))
		return FALSE;

	// Work out the required size for the redraw icon
	// REMEMBER m_AnimationRect is in MILLIPOINTS 
	// Read from the dpi of the screen from the OS via the dialog manager
	INT32 ScreenDpi = GetScreenDpi();
	if (ScreenDpi <= 0)
		ScreenDpi = 96;

	// Work out the size of an output pixel in millipoints
	const INT32 PixelSize = 72000 / ScreenDpi;
	// And inflate the rect by 6 pixels to take account of the slab in
	ERROR3IF(m_AnimationRect.IsEmpty(),"m_AnimationRect not set up!");
	DocRect RequiredSize = m_AnimationRect;
	RequiredSize.Inflate(PixelSize * 6);

	INT32 Width = RequiredSize.Width() / PixelSize; 
	INT32 Height = RequiredSize.Height() / PixelSize; 
	
	// Read from the size of the screen from the OS via the dialog manager
	INT32 MaxWidth = 800;
	INT32 MaxHeight = 600;
	GetScreenSize(&MaxWidth, &MaxHeight);

	// Work out the size of redraw icon that is required
	INT32 CurrentWidth = RedrawRect.right - RedrawRect.left;
	INT32 NewWidth = CurrentWidth;
	if (Width > NewWidth)
		NewWidth = Width;
	if (NewWidth > MaxWidth)
		NewWidth = MaxWidth;
	INT32 DiffWidth = NewWidth - CurrentWidth;

	INT32 CurrentHeight = RedrawRect.bottom - RedrawRect.top;
	INT32 NewHeight = CurrentHeight;
	if (Height > NewHeight)
		NewHeight = Height;
	if (NewHeight > MaxHeight)
		NewHeight = MaxHeight;
	INT32 DiffHeight = NewHeight - CurrentHeight;

	// No changes required
	if (DiffHeight == 0 && DiffWidth == 0)
		return TRUE;

	// *************************************************
	// Make the redraw icon the required size
	RedrawRect.right  = RedrawRect.right + DiffWidth;
	RedrawRect.bottom = RedrawRect.bottom + DiffHeight;
	SetGadgetPosition(_R(IDC_REDRAW_ME), RedrawRect);
	
	// *************************************************
	RECT Rect;
	if (GetGadgetPosition(_R(IDC_3DFRAME), &Rect))
	{
		Rect.right  = Rect.right + DiffWidth;
		Rect.bottom = Rect.bottom + DiffHeight;
		SetGadgetPosition(_R(IDC_3DFRAME), Rect);
	}

	// *************************************************
	// Get the size and position of the dialog box itself
	if (GetWindowPosition(&Rect))
	{
		Rect.right  = Rect.right + DiffWidth;
		Rect.bottom = Rect.bottom + DiffHeight;
		SetWindowPosition(Rect);
	}

	// Force a redraw of the items that we have changed
	// causes a DIM_REDRAW\_R(IDC_REDRAW_ME) message to be sent.
	InvalidateGadget(_R(IDC_3DFRAME));
	InvalidateGadget(_R(IDC_REDRAW_ME));

	return TRUE;
}

/*******************************************************************************************

>	BOOL PreviewDialog::SetUpCallBack()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Sets up the call back for this dialog box so that we get timer events and
				so can animate the bitmap on display.

*******************************************************************************************/

BOOL PreviewDialog::SetUpCallBack()
{
	// set the delay for the first bitmap that we are going to display
	m_Delay = GetDelayForCurrentBitmap();
	return m_Timer.Start(m_Delay * 10, TimerProc, FALSE);
}



/*******************************************************************************************
>	static BOOL PreviewDialog::TimerProc(HiResTimer* pThisTimer, INT32 nElapsed, void* pvData)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/97
	Inputs:		(see HiResTimer::Callback)
	Purpose:	This callback function is called when the new frame in the GIF should be
				displayed.  It resets the timer to generate an event for the subsequent
				frame.
*******************************************************************************************/

BOOL PreviewDialog::TimerProc(HiResTimer* pThisTimer, INT32 nElapsed, void* /* pvData */)
{
	if (m_pPreviewDialog != NULL)
	{
		TRACEUSER( "JustinF", _T("%ld msec elapsed\n"), (INT32) nElapsed);

		// Stop the animation if the properties are changed or the document is closd.
		if(m_AnimationPropertiesChanged || m_pPreviewDialog->m_DocumentAboutToDie)
		{
			// Stop any pending timer events.
			if (pThisTimer->IsRunning())
				pThisTimer->Stop();
			return TRUE;
		}
	
		// Move to the next bitmap in the list
		m_pPreviewDialog->IncrementCurrentBitmap();

		// Force the gadget to be repainted
		m_pPreviewDialog->InvalidateGadget(_R(IDC_REDRAW_ME));
		m_pPreviewDialog->PaintGadgetNow(_R(IDC_REDRAW_ME));

		// Get the delay until we need to show the next frame
		m_Delay = m_pPreviewDialog->GetDelayForCurrentBitmap();

		// Reset the timer.
		pThisTimer->Start(m_Delay * 10, TimerProc, FALSE);
	}

	// Please continue to send events to us.
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL PreviewDialog::OnIdleEvent(void)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	21/05/97
	Inputs:		-
	Returns:	TRUE if it does not want low-priority idle handlers to be called this time
				FALSE if it's OK to call low-priority idles.
	Purpose:	Used to detect when the mouse has stopped moving 
				Therefore allows us to implement bubble help for the Preview Dialog.

********************************************************************************************/

BOOL PreviewDialog::OnIdleEvent(void)
{
	UpdateBubbleHelp();
		return FALSE;
};

/********************************************************************************************

>	void PreviewDialog::UpDateBubbleHelp(void)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason code)
	Created:	21/05/97
	Inputs:		-
	Purpose:	Called for Idle events to provide bubble help for the Preview Dialog.

********************************************************************************************/
void PreviewDialog::UpdateBubbleHelp(void)
{
	PreviewDialog *pPrvDlg = m_pPreviewDialog;
	if (pPrvDlg == NULL)
		return;

	static HWND TheWindow = NULL;
	
	if (TheWindow == NULL)
		TheWindow = (HWND)pPrvDlg->WindowID;

	if (TheWindow == NULL)
		return;

	static MonotonicTime LastUpdate;
	static UINT32 MousePos = NULL;

	if (LastUpdate.Elapsed(100))
	{
		// Remember when we last checked
		LastUpdate.Sample();

		// Default to the mouse being "nowhere special" again
		TheWindow = (HWND)pPrvDlg->WindowID;
		MousePos = NULL;

		POINT MouseScreenPos;
		if (::GetCursorPos(&MouseScreenPos))
		{
			// Only continue processing if the window under the pointer is the Preview Dialog
			
			HWND WindowUnder = ::WindowFromPoint(MouseScreenPos);

			if (WindowUnder != NULL &&
				(WindowUnder == TheWindow || ::GetParent(WindowUnder) == TheWindow))
			{
				POINT TempPos;
				TempPos.x = MouseScreenPos.x;
				TempPos.y = MouseScreenPos.y;

				// Convert to client coords in the main window
				::ScreenToClient(TheWindow, &TempPos);

				CPoint Pos(TempPos);
				HWND WindowUnderPointer = ::ChildWindowFromPoint(TheWindow, Pos);
				if (WindowUnderPointer != NULL)// && IsWindowVisible(WindowUnderPointer))
				{
					// Make sure that hidden windows do not provide status help!
					INT32 WindowStyle = ::GetWindowLong(WindowUnderPointer, GWL_STYLE);
					if ((WindowStyle & WS_VISIBLE) != 0)
					{
						HWND hGadget;
						INT32 i = 0;
						while (BubbleHelp[i].Gadget != NULL && MousePos == NULL)
						{
							hGadget = GetDlgItem(TheWindow, BubbleHelp[i].Gadget);
							if (WindowUnderPointer == hGadget)
							{
								MousePos = (UINT32)BubbleHelp[i].Gadget;
								TheWindow = hGadget;
							}
							i++;
						}
					}
				}
			}
		}
	}
	// Tell the bubble help system what help we want.
	if (MousePos != NULL)
	{
		// Set up our callback handler to show the bubble help. 
		ControlHelper::DoBubbleHelpOn(TheWindow, MousePos, PreviewDialog::HelpCallbackHandler, NULL);
	}
}

/********************************************************************************************

>	TCHAR* PreviewDialog::HelpCallbackHandler(HWND Window, UINT32 Item, void *UserData)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (Based onn Jason code)
	Created:	21/05/97
	Inputs:		Window	 - The window to get help for
				Item	 - The control to get help for
				UserData - User-supplied data (not used)
	Returns:	The bubble help string to use, or NULL if no help is available.
	Purpose:	Help callback handler to provide bubble help for the POreview Diaolg.
	
********************************************************************************************/

TCHAR *PreviewDialog::HelpCallbackHandler(HWND Window, UINT32 Item, void *UserData)
{
	static String_256 HelpStringStore;
	BOOL ReturnVal = FALSE;

	PreviewDialog* pPreviewDlg = m_pPreviewDialog;
	if (pPreviewDlg == NULL)
		return(NULL);

	switch (Item)
	{
		case _R(IDC_PREVIEW_PLAY):
			HelpStringStore = String_256(_R(IDS_PREVIEWBH_PLAY));
			ReturnVal = TRUE;
		break;
			
		case _R(IDC_PREVIEW_STOP):		 	     
			HelpStringStore = String_256(_R(IDS_PREVIEWBH_STOP));
			ReturnVal = TRUE;
		break;

		case _R(IDC_PREVIEW_NEXTFRAME):		 	     
			HelpStringStore = String_256(_R(IDS_PREVIEWBH_NEXTFRAME));
			ReturnVal = TRUE;
		break;
			
		case _R(IDC_PREVIEW_PREVFRAME):
			HelpStringStore = String_256(_R(IDS_PREVIEWBH_PREVFRAME));
			ReturnVal = TRUE;
		break;	

		case _R(IDC_PREVIEW_SLIDER):
			HelpStringStore = String_256(_R(IDS_PREVIEWBH_SLIDER));
			ReturnVal = TRUE;
		break;	
	}

	if (ReturnVal)
		return((TCHAR *) HelpStringStore);

	return(NULL);
	
}


/********************************************************************************************

>	BOOL PreviewDialog::GetStatusLineText(String_256 *Result)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (Based on Jason code)
	Created:	21/05/97
	Inputs:		*Result - The String to recieve the status line text.
	Outputs		The Status Line string to use, or NULL if not available.
	Returns		TRUE if successful, else false.	
		
********************************************************************************************/

BOOL PreviewDialog::GetStatusLineText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");
		
	PreviewDialog* pPreviewDlg = m_pPreviewDialog;

	if (pPreviewDlg == NULL)
		return(FALSE);

	// Find the main editor window
	HWND TheWindow = (HWND)pPreviewDlg->WindowID;
	if (TheWindow == NULL)
		return(FALSE);

	// Get the cursor position in screen coords
	POINT TempPos;
	if (!::GetCursorPos(&TempPos))
		return(FALSE);

	// Convert to client coords in the main window
	ScreenToClient(TheWindow, &TempPos);

	// See if this is over the main window or any child, get it's HWND
	CPoint Pos(TempPos);	
	HWND WindowUnderPointer = ::ChildWindowFromPoint(TheWindow, Pos);
	if (WindowUnderPointer == NULL)
		return(FALSE);

	// Make sure that hidden windows do not provide status help!
	INT32 WindowStyle = ::GetWindowLong(WindowUnderPointer, GWL_STYLE);
	if ((WindowStyle & WS_VISIBLE) == 0)
		return(FALSE);

	// If the pointer is over main window background, return standard help
	if (WindowUnderPointer == TheWindow)
		return(GetStatusLineText(pPreviewDlg, 0, Result));

	// Otherwise, see if we can find help for the specific gadget
	HWND hGadget;
	INT32 i = 0;
	while (BubbleHelp[i].Gadget != NULL)
	{
		hGadget = GetDlgItem(TheWindow, BubbleHelp[i].Gadget);
		if (WindowUnderPointer == hGadget && GetStatusLineText(pPreviewDlg, BubbleHelp[i].Gadget, Result))
			return(TRUE);
		i++;
	}
	return FALSE;
}

/************************************************************************************************

>	static BOOL PreviewDialog::GetStatusLineText(PreviewDialog *pPrvDlg, UINT32 GadgetID,
													String_256 *Result)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/05/97
	Inputs:		pPrvDlg - Points to the current Preview Dialog.
				GadgetID - indicates the gadget for which you want status bar help.
	Outputs:	If return value is TRUE, Result is updated with an appropriate help string.
	Returns:	-
	
**************************************************************************************************/

BOOL PreviewDialog::GetStatusLineText(PreviewDialog *pPrvDlg, UINT32 GadgetID,String_256 *Result)
{
	switch (GadgetID)
	{
		case _R(IDC_PREVIEW_PLAY):
			*Result = String_256(_R(IDS_PREVIEWST_PLAY));
			break;

		case _R(IDC_PREVIEW_STOP):
			*Result = String_256(_R(IDS_PREVIEWST_STOP));
			break;

		case _R(IDC_PREVIEW_NEXTFRAME):
			*Result = String_256(_R(IDS_PREVIEW_NEXTFRAME));
			break;

		case _R(IDC_PREVIEW_PREVFRAME):
			*Result = String_256(_R(IDS_PREVIEW_PREVFRAME));
			break;			
	}
	return TRUE;
}

/***********************************************************************************************

>	BOOL PreviewDialog::DoRegenerateSingleFrame()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/05/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	While previewing an animation, if the Reset button is clicked we need to regrab 
				all the frames.	This function invokes the correct Op.

*************************************************************************************************/
BOOL PreviewDialog::DoRegenerateFrames()
{
	// When in stop mode, we should try and regrab the frame being displayed
	// When in play mode, we should grab the entire animation
	// If we were in stand by mode then grab the active frame.

	// Return false if the document is empty.
	String_256 ShadeReason;
	OpState State = OpGrabAllFrames::GetState(&ShadeReason, NULL);
	if (State.Greyed)
	{
		if(!m_SetPreviewDlgToIdleState)
			//Set the dlg to an idle state.
			m_SetPreviewDlgToIdleState = TRUE;

		// Force the gadget to be repainted.
		m_pPreviewDialog->InvalidateGadget(_R(IDC_REDRAW_ME));
		return FALSE;			
	}
	
	// Temp flags to remember whether the animation was playing/How may layers existed
	// before all the frames were regenerated.
	BOOL AnimationPlaying = FALSE;
	//UINT32 PreRefreshNumberOfLayers = m_BitmapListSize;
	BOOL IdleState = m_SetPreviewDlgToIdleState;
	
	// If we are not in an idle state already then ensure that we are playing we are stopped
	if (!m_SetPreviewDlgToIdleState)
	{
		// Stop the animation.
		if (m_PlayAnimation)
		{
			StopPreviewing();

			AnimationPlaying = TRUE;
		}
	}

	// Set the dlg idle flag back to false.
	m_SetPreviewDlgToIdleState = FALSE;
	
	// Are we in single bitmap mode?
	BOOL SingleBitmapMode = (m_BitmapListSize == 1);

	// If we were playing or idle then grab all frames
	//if (AnimationPlaying || IdleState)
	// If we are in an idle state or we are in multiple bitmap mode then
	// refresh = grab all frames
	if (IdleState || !SingleBitmapMode)
	{
		// If we were playing or idle then ensure that we restart in play mode
		if (AnimationPlaying || IdleState)
			m_PlayAnimation	= TRUE;
		else
			m_PlayAnimation	= FALSE;

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABALLFRAMES); 
		if (pOpDesc != NULL)
			pOpDesc->Invoke();
		else
		{
			ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
		}
	}
	else
	{
		// We were showing a single frame so try and refresh this layer
		// and show it again.
		Layer * pLayerToRefresh = NULL;
		KernelBitmap * pBitmap = GetCurrentBitmap();
		if (pBitmap != NULL)
		{
			// Search through the layers for this bitmap and then try and regenerate it
			// If we don't find it then regnerate the active layer
			Spread * pSpread = Document::GetSelectedSpread();
			if (pSpread)
			{
				Layer * pLayer = pSpread->FindFirstFrameLayer();
				BOOL Found = FALSE;
				while (pLayer != NULL && !Found)
				{
					KernelBitmap * pLayerBitmap = pLayer->GetGeneratedBitmap();
					if (pLayerBitmap == pBitmap)
					{
						pLayerToRefresh = pLayer;
						Found = TRUE;
					}
					
					pLayer = pLayer->FindNextFrameLayer();
				}
			}
		}

		// Ensure that we restart in stop mode
		m_PlayAnimation	= FALSE;

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABFRAME); 
		// Pass in pLayerToRefresh as a possible layer to refresh
		if (pOpDesc != NULL)
		{
			OpParam Param;
			Param.Param1 = (INT32)pLayerToRefresh;
			pOpDesc->Invoke(&Param);
		}
		else
		{
			ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
		}
	}

	// The ops should call us back to set everything up.

	// We will just ensure that these are set correctly
	// Set the flags to the correct state.
	m_DocumentAboutToDie		=	FALSE;
	m_SetPreviewDlgToIdleState	=	FALSE;

	// Reset the loop counts
	m_CurrentLoopCount			= 0;	// The loop that we are currently on (default to start)
	m_RequiredLoopCount			= 0;	// The loop count that we must reach (default for ever)

	// Through a document change, the timer may not be ticking.
	// If it has been stopped, then re-start.
	RestartTimer();

	return TRUE;
}

/****************************************************************************************************

>	BOOL PreviewDialog::SetPreviewDialogToIdleState();

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (From Neville code)
	Created:	28/05/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise
	Purpose:	If the preview animation dialog is open, and the user does any of the following:-
				Changes the current doc.
				Closes the current doc.
				Opens a new doc.
				Then we stop playing the current animation, and set the dialog to a grey background
				with a cross in the centre.
			
*****************************************************************************************************/

void PreviewDialog::SetPreviewDialogToIdleState(ReDrawInfoType* pExtraInfo)
{
	if (pExtraInfo == NULL)
		return;

	// Go get a render region
	DocRect VirtualSize(0, 0, pExtraInfo->dx, pExtraInfo->dy);
	BOOL StartedOk = FALSE;

	// we should have a render region, but to be on the safe side.
	if (m_pRender == NULL)
	{	
		// This will call start render for us
		m_pRender = CreateGRenderRegion(&VirtualSize, pExtraInfo);
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
			
		// Draw a cross to signify we are in a halted or idle state
		RedrawCross(&VirtualSize, &RedrawColours, PixelSize);

		m_pRender->RestoreContext();

		// Blit to the screen
		m_pRender->StopRender();
	}

	// Set the correct state of the dlg controls.
	EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);
	EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
	EnableGadget(_R(IDC_PREVIEW_NEXTFRAME), FALSE);
	EnableGadget(_R(IDC_PREVIEW_PREVFRAME), FALSE);

	// Clean the list of bitmaps that we were animating.
	m_BitmapList.DeleteAll();
	m_BitmapListSize			= 0;
	m_CurrentItem				= 0;
	m_SliderPosition			= m_BitmapListSize;
	m_AnimationRect.MakeEmpty();

	// Set the slider back to a good state
	SetGadgetRange(_R(IDC_PREVIEW_SLIDER), 1, 1, 100);	
	SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), 1 );

	// Disable the slider
	EnableGadget(_R(IDC_PREVIEW_SLIDER), FALSE);
}

/*************************************************************************************************************

>	BOOL PreviewDialog::SetBitmapToListTail(GIFAnimationExportParam* pParam)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		pParam - GIF Export Options
	Outputs	-
	Returns		TRUE if successful, else FALSE.
	Purpose:	If we need to (re)generate asingle bitmap while the Preview dialog is open, 
				this function allows us to add the Bitmap to the Tail of the list.				

***************************************************************************************************************/

BOOL PreviewDialog::SetBitmapToListTail(GIFAnimationExportParam* pParam)
{
	if (pParam == NULL)
		ERROR2RAW("PreviewDialog::SetRegeneratedBitmapInList bad params");

	// Ensure we only have a single bitmap in our list.
	UINT32 BitmapCount = pParam->GetBitmapCount();
	ERROR3IF(BitmapCount != 1, "BitmapCount specifid an illegal value");

	// Increment the value of list size.
	m_BitmapListSize++; 

	// Retrieve the generated bitmap from GIFAnimationExportParam.
	KernelBitmap* pRegeneratedBitmap = pParam->GetBitmap(0);
	Layer * pLayer = pParam->GetLayer(0);

	if (pRegeneratedBitmap)
	{
		AnimatedBitmapItem * pNewItem = new AnimatedBitmapItem(pRegeneratedBitmap, pLayer);
		if (pNewItem)
		{
			// Add this bitmap to the tail of our list.
			m_BitmapList.AddTail(pNewItem);
		}
	}

	return TRUE;
}

/*************************************************************************************************************

>	BOOL PreviewDialog::SetDlgStateAfterDocChange()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE.
	Purpose:	If the Preview dialog is open before and after a doument change, then play the animation and
				set the correct state of the controls.

***************************************************************************************************************/
BOOL PreviewDialog::SetDlgStateAfterDocChange()
{
	// The document has changed, the refresh button has been clicked,
	// regenerate all the frames, set the controls to the correct state.
	// and play the animation.

	// All the setting up of the new state of the dialog will happen when the op
	// realises that the dialog is already open and calls ReInitDialog().
	if(!DoRegenerateFrames())
		return FALSE;

	return TRUE;
}

/*************************************************************************************************************

>	BOOL PreviewDialog::SetDlgStateAfterRefresh()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE.
	Purpose:	When the preview dlg is open, if the refresh button is clicked, this function 
				refreshes the current state of the dialog. 

***************************************************************************************************************/
BOOL PreviewDialog::SetDlgStateAfterRefresh()
{
	// Refresh the current state of the dialog:-
	// When in stop mode, we should try and regrab the frame being displayed
	// When in play mode, we should grab the entire animation
	// If we were in stand by mode then grab the active frame.

	// All the setting up of the new state of the dialog will happen when the op
	// realises that the dialog is already open and calls ReInitDialog().
	if(!DoRegenerateFrames())
		return FALSE;

	return TRUE;
}

/*************************************************************************************************************

>	void PreviewDialog::PreviewDlgPlay()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	After the play button has been clicked, this functions plays the animation and 
				sets the correct state of the controls.
				
***************************************************************************************************************/
void PreviewDialog::PreviewDlgPlay()
{
	// We should ensure that all the frames are up to date. The simplest way of doing this is
	// by calling the GrabAllFrames op. This should ensure that the frames are up to date
	// and force play to on. It should also rest the dialog to the way it should be.
	// Ensure that when we come back we are in play mode.
	m_PlayAnimation = TRUE;

	if (!viaBitmapGallery)
	{
		// we need to do this - so that we can call the followinf invoke ....
		//DialogBarOp::SetSystemStateChanged();

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_FRAME_GRABALLFRAMES); 
		if (pOpDesc != NULL)
			pOpDesc->Invoke();
		else
		{
			ERROR3("Couldn't find OPTOKEN_FRAME_GRABALLFRAMES op descriptor");
		}
	}
	else
	{
		//Disable the Play button.
		EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);

		//Enable the Stop button.
		EnableGadget(_R(IDC_PREVIEW_STOP), TRUE);

		// Set the correct state of the flags.
		m_DocumentAboutToDie = FALSE;
		m_GeneratedFrameLayers = TRUE;

		// After a document change/refresh, the bitmap list size may have changed.
		INT32 StepValue = 100/ m_BitmapListSize;
		SetGadgetRange(_R(IDC_PREVIEW_SLIDER), 1, m_BitmapListSize, StepValue);	
		SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), m_BitmapListSize - m_CurrentItem);

		// Through a document change, the timer may not be ticking.
		// If it has been stopped, then re-start.
		RestartTimer();
	}
	
	// The old way of doing it, jsut play the animation
/*	// Play the Animation.
	m_PlayAnimation = TRUE;

	//Disable the Play button.
	EnableGadget(_R(IDC_PREVIEW_PLAY), FALSE);

	//Enable the Stop button.
	EnableGadget(_R(IDC_PREVIEW_STOP), TRUE);

	// Set the correct state of the flags.
	m_DocumentAboutToDie = FALSE;
	m_GeneratedFrameLayers = TRUE;

	// After a document change/refresh, the bitmap list size may have changed.
	INT32 StepValue = 100/ m_BitmapListSize;
	SetGadgetRange(_R(IDC_PREVIEW_SLIDER), 1, m_BitmapListSize, StepValue);	
	SetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), m_BitmapListSize - m_CurrentItem);

	// Through a document change, the timer may not be ticking.
	// If it has been stopped, then re-start.
	RestartTimer(); */
}

/*************************************************************************************************************

>	void PreviewDialog::PreviewDlgStop()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	After the Stop button has been cllicked, this functions Stops the animation and 
				sets the correct state of the controls.
				
***************************************************************************************************************/
void PreviewDialog::PreviewDlgStop()
{
	// Only display the last bitmap when this button is clicked.
	m_PlayAnimation = FALSE;

	//Enable the Play button. 
	EnableGadget(_R(IDC_PREVIEW_PLAY), TRUE);

	// Disable the Stop button.
	EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
}

/*************************************************************************************************************

>	void PreviewDialog::PreviewNextFrame()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	After the preview next frame button is clicked, if the animation was playing this 
				function stops tha animation, and selects the next frame. 	
								
***************************************************************************************************************/
void PreviewDialog::PreviewNextFrame()
{
	// If the animation is playing, set the correct state of the controls.
	if (m_PlayAnimation)
	{
		EnableGadget(_R(IDC_PREVIEW_PLAY), TRUE);
		EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
	}

	// Stop the animation.
	m_PlayAnimation = FALSE;
	m_DisplayNextFrame = TRUE;

	// Find the correct bitmap to display.						
	BOOL Valid;
	m_SliderPosition = GetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), 0, m_BitmapListSize, 0, &Valid);
	UINT32 BitmapNum = m_BitmapListSize - m_SliderPosition;
	SelectCurrentBitmap(BitmapNum);

	// Move to the next bitmap in the list.
	IncrementCurrentBitmap();
}

/*************************************************************************************************************

>	void PreviewDialog::PreviewPreviousFrame()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	After the Preview previous frame button is cllicked, if the animation was playing this 
				function stops tha animation, and selects the previous frame. 	
								
***************************************************************************************************************/
void PreviewDialog::PreviewPreviousFrame()
{
	if (m_PlayAnimation)
	{
		EnableGadget(_R(IDC_PREVIEW_PLAY), TRUE);
		EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
	}

	m_PlayAnimation = FALSE;
	m_DisplayPreviousFrame = TRUE;

	// Find the correct bitmap to display.						
	BOOL Valid;
	m_SliderPosition = GetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), 0, m_BitmapListSize, 0, &Valid);
	UINT32 BitmapNum = m_BitmapListSize - m_SliderPosition;
	SelectCurrentBitmap(BitmapNum);
							
	// Move to the previous bitmap in the list.
	DecrementCurrentBitmap();
}

/*************************************************************************************************************

>	void PreviewDialog::SetSliderPosition()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This functiion deals with clicks on the Preview dialog slider control.
								
***************************************************************************************************************/
void PreviewDialog::SetSliderPosition()
{
	// Set the correct state of the controls.
	if (m_PlayAnimation && m_BitmapListSize > 1)
	{
		EnableGadget(_R(IDC_PREVIEW_PLAY), TRUE);
		EnableGadget(_R(IDC_PREVIEW_STOP), FALSE);
	}

	// Stop the animation. 
	m_PlayAnimation = FALSE;	
						
	// Get the new Slider position.
	BOOL Valid;
	m_SliderPosition = GetLongGadgetValue(_R(IDC_PREVIEW_SLIDER), 0, m_BitmapListSize, 0, &Valid);

	// Find the correct bitmap to display.						
	UINT32 BitmapNum = m_BitmapListSize - m_SliderPosition;
	SelectCurrentBitmap(BitmapNum);

	// Set the new slider position.
	SetSliderPosition(m_SliderPosition);
}

/*************************************************************************************************************

>	void PreviewDialog::RestartTimer()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/05/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	If for some reason the timer was stopped, this function will re-start it.
								
***************************************************************************************************************/
void PreviewDialog::RestartTimer()
{
	// Has the timer been stopped.
	if (!m_Timer.IsRunning())
	{
		// Re-start the timer.
		SetUpCallBack();
	}
}
