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
// The Layers gallery

/*
*/

#include "camtypes.h"
#include "layergal.h"

DECLARE_SOURCE("$Revision$");

//#include "ensure.h"
#include "sprdmsg.h"
//#include "markn.h"
//#include "simon.h"
#include "document.h"
#include "docview.h"
#include "layermgr.h"
#include "spread.h"
#include "insertnd.h"
#include "ink.h"
#include "app.h"
//#include "resource.h"
//#include "barsdlgs.h" 
#include "guides.h" 
#include "nodetxts.h"
#include "textops.h"
#include "bubbleid.h"

#include "sglayer.h"		// The LayerSGallery class
#include "sginit.h"

//#include "will2.h"
//#include "galstr.h"			// _R(IDS_DELETEPAGEBACKGROUND)
//#include "prevwres.h"		// _R(IDS_DISPLAY_FRAME_GALLERY) _R(IDD_ANIMATIONBAR) ...
#include "sgframe.h"		// FrameSGallery

#include "frameops.h"		// Frame related ops
#include "aprps.h"

#include "nodetxts.h"		//TextStory::SetFocusStory

#include "layermsg.h"		// LayerMsg::LayerReason::REDRAW_LAYER
#include "xshelpid.h"		//For the help id

#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

CC_IMPLEMENT_DYNCREATE(OpDeletePageBackground,OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpLayerGalChange,UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpDisplayLayerGallery,Operation);
CC_IMPLEMENT_DYNCREATE(OpDisplayFrameGallery,Operation);
CC_IMPLEMENT_DYNCREATE(LayerNameDlg, DialogOp)   
CC_IMPLEMENT_DYNCREATE(LayerStateAction,Action)
			  
const CDlgMode 	LayerNameDlg::Mode 	 		= MODAL;   // The dialog is modal
const INT32		LayerNameDlg::IDD			= _R(IDD_LAYER_NAME);
BOOL 			LayerNameDlg::IsOpen 		= FALSE;  // It's not open yet
LayerNameDlg*	LayerNameDlg::pLayerNameDlg = NULL;  // The layer name dlg that is open

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


//------------------------------------------------------------------------------------------
// Methods for the OpDeletePageBackground

/********************************************************************************************

>	OpDeletePageBackground::OpDeletePageBackground() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Purpose:	OpDeletePageBackground constructor (Creates an undoable operation)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpDeletePageBackground::OpDeletePageBackground()								
{                              
	// nothing to do yet
}

/********************************************************************************************

>	OpState	OpDeletePageBackground::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpDeletePageBackground operation
	Purpose:	For finding the OpDeletePageBackground's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDeletePageBackground::GetState(String_256* pUIDescription, OpDescriptor* pOpDesc)
{
	OpState OpSt;  

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is a background layer
		Layer* pLayer = pSpread->FindFirstPageBackgroundLayer();
		if (pLayer)
		{
			// Yes, so we are allowed to delete it
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}

 	return OpSt;   
}

/********************************************************************************************

>	void OpDeletePageBackground::Do(OpDescriptor* pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Finds and deletes the page background layer from the selected spread.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeletePageBackground::Do(OpDescriptor* pOpDesc)
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is a background layer
		Layer* pLayer = pSpread->FindFirstPageBackgroundLayer();
		if (pLayer)
		{
			OpLayerGalParam Param(PAGEBACKGROUND_DEFAULT, pSpread);
			Param.pLayer = pLayer;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	return;
}

//------------------------------------------------------------------------------------------
// Methods for the OpLayerGalChange

/********************************************************************************************

>	OpLayerGalChange::OpLayerGalChange() 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpLayerGalChange constructor (Creates an undoable operation)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpLayerGalChange::OpLayerGalChange(): UndoableOperation()								
{                              
	pSpread 				= NULL;
	UndoIDS 				= _R(IDS_NOTYPE);
	BroadcastLayerChanges 	= TRUE;
	pOpUndoLayer			= NULL;
	pOpRedoLayer			= NULL;
}

/********************************************************************************************

>	BOOL OpLayerGalChange::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpLayerGalChange initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerGalChange::Init()
{
	return (
			RegisterOpDescriptor(
	 							0,
	 							_R(IDS_LAYERCHANGEOP),
								CC_RUNTIME_CLASS(OpLayerGalChange),
	 							OPTOKEN_LAYERGALCHANGE,
	 							OpLayerGalChange::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_LAYERCHANGEOP),
	 							0	/* bitmap ID */
								) &&
			RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DELETEPAGEBACKGROUND),
								CC_RUNTIME_CLASS(OpDeletePageBackground),
	 							OPTOKEN_DELETEPAGEBACKGROUND,
	 							OpDeletePageBackground::GetState,
	 							0,			// help ID
	 							0,			// bubble help id
	 							0			// bitmap ID
								)
			// special frame operations
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_SELECTSTARTFRAME),			// String resource ID
								CC_RUNTIME_CLASS(OpSelectStartFrame),	// Runtime class
	 							OPTOKEN_FRAME_STARTFRAME,		// Token string
	 							OpSelectStartFrame::GetState,	// GetState function
	 							0,								// help ID
								_R(IDBBL_SELECTSTARTFRAME),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_PREVIEW_STARTFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_SELECTENDFRAME),				// String resource ID
								CC_RUNTIME_CLASS(OpSelectEndFrame),	// Runtime class
	 							OPTOKEN_FRAME_ENDFRAME,			// Token string
	 							OpSelectEndFrame::GetState,	// GetState function
	 							0,								// help ID
								_R(IDBBL_SELECTENDFRAME),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_PREVIEW_ENDFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_SELECTPREVFRAME),			// String resource ID
								CC_RUNTIME_CLASS(OpSelectPrevFrame),	// Runtime class
	 							OPTOKEN_FRAME_PREVFRAME,		// Token string
	 							OpSelectPrevFrame::GetState,	// GetState function
	 							0,								// help ID
								_R(IDBBL_SELECTPREVFRAME),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_PREVIEW_PREVFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_SELECTNEXTFRAME),			// String resource ID
								CC_RUNTIME_CLASS(OpSelectNextFrame),	// Runtime class
	 							OPTOKEN_FRAME_NEXTFRAME,		// Token string
	 							OpSelectNextFrame::GetState,	// GetState function
	 							0,								// help ID
								_R(IDBBL_SELECTNEXTFRAME),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_PREVIEW_NEXTFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_NEWFRAME),					// String resource ID
								CC_RUNTIME_CLASS(OpCreateNewFrame),	// Runtime class
	 							OPTOKEN_FRAME_NEWFRAME,			// Token string
	 							OpCreateNewFrame::GetState,		// GetState function
	 							0,								// help ID
								_R(IDBBL_NEWFRAME),					// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_NEWFRAME),				// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_COPYFRAME),					// String resource ID
								CC_RUNTIME_CLASS(OpCopyFrame),	// Runtime class
	 							OPTOKEN_FRAME_COPYFRAME,		// Token string
	 							OpCopyFrame::GetState,			// GetState function
	 							0,								// help ID
								_R(IDBBL_COPYFRAME),				// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_COPYFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
								&&
			RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_DELETEFRAME),				// String resource ID
								CC_RUNTIME_CLASS(OpDeleteFrame),// Runtime class
	 							OPTOKEN_FRAME_DELETEFRAME,		// Token string
	 							OpDeleteFrame::GetState,		// GetState function
	 							0,								// help ID
								_R(IDBBL_DELETEFRAME),				// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_DELETEFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								)
			);
}               
    
/********************************************************************************************

>	OpState	OpLayerGalChange::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpLayerGalChange operation
	Purpose:	For finding the OpLayerGalChange's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpLayerGalChange::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  
 	return(OpSt);   
}

/********************************************************************************************

>	void OpLayerGalChange::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		OpDescriptor (unused)
				pOpParam - this is really a ptr to an OpLayerGalParam
	Outputs:	-
	Returns:	-
	Purpose:	Performs the layer change operation defined by ((OpLayerGalParam*)pOpParam)->Reason.
				It handles the creation, deletion, changing, and moving of the layer provided.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/
// The bounding rectangle of the current layer must be invalidated in all the following cases. 

// If the visible status has changed 
// If the Quality level has changed 
// If the layer has been deleted 
// If the z-order of the layer has changed

			
void OpLayerGalChange::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{   
	OpLayerGalParam* pOpLayerGalParam = (OpLayerGalParam*)pOpParam;

	pSpread = pOpLayerGalParam->GetParentSpread();
	Reason  = pOpLayerGalParam->GetReason();

	Layer* 				pLayer 		= pOpLayerGalParam->pLayer;
	Node*				pContextNode= pOpLayerGalParam->pContextNode;
	AttachNodeDirection	AttDir		= pOpLayerGalParam->AttDir;
	LayerStatus			NewStatus 	= pOpLayerGalParam->Status;

	BOOL FrameLayer = FALSE;

	switch (Reason)
	{
		//--------------------------------------------------------------------------------------------
		// Select a Frame layer
		case (FRAME_SELECT):
			// Check for being given a null layer or an already active layer
			if (pLayer == NULL || pLayer->IsActive())
				goto EndOperation;

			// Make the layer we have been passed the active one
			FrameSGallery::MakeActiveLayer(pLayer);
			UndoIDS = _R(IDS_FRAME_UNDO_SELECTNEW);
			break;
		//--------------------------------------------------------------------------------------------
		// Create a new layer
		case (FRAME_NEW):
			FrameLayer = TRUE;
		case (LAYER_NEW):
			{
				// Attempt to create a new layer node 
				ALLOC_WITH_FAIL(pLayer, (new Layer()), this); 	      
				if (pLayer == NULL)
					goto EndOperation; // We were unable to create a new layer so 
									   // abort the operation 

				// Set the new layer's status  
				pLayer->SetLayerStatus(NewStatus); 

				// Set the frame layer flag according to the operation type i.e FRAME_NEW = set flag
				if (FrameLayer)
				{
					pLayer->SetFrame(TRUE);
					pLayer->SetEdited(TRUE);
				}

				// Insert new layer in the tree
				// Rule: if there is a guide layer at the top, then place the new layer
				// beneath this layer, otherwise, put the new layer at the top.
				Layer* pLastLayer = pSpread->FindLastLayer();
				if(pLastLayer && pLastLayer->IsGuide())
				{
					// this layer is a guide layer - get the next one
					pLastLayer = pLastLayer->FindPrevLayer();
				}
				Node* pNodeToInsert;
				if(pLastLayer)
				{
					pNodeToInsert = pLastLayer;
				}
				else
				{
					// what?! can't find a Last Layer to use?
					pNodeToInsert = pContextNode;
				}

				if (DoInsertNewNode(pLayer, pLastLayer, NEXT, FALSE))
				{
					if (FrameLayer)
						FrameSGallery::MakeActiveLayer(pLayer);
					else
						LayerSGallery::MakeActiveLayer(pLayer);

					// Note the layer inserted so we can clear any selection on the layer when this op is undone
					pOpUndoLayer = pLayer;
					pLayer->EnsureUniqueLayerID();
				}
			}

			if (FrameLayer)
				UndoIDS = _R(IDS_FRAME_UNDO_NEW);
			else
				UndoIDS = _R(IDS_LAYER_UNDO_NEW);
			break;

		//--------------------------------------------------------------------------------------------
		// Delete a layer
		case (FRAME_DELETE):
			FrameLayer = TRUE;
		case (LAYER_DELETE):
		case (PAGEBACKGROUND_DEFAULT):
			{
				// This doesn't seem to be an important ensure as it works when the layer
				// is not active
				// Disabled 10/4/97
				//ERROR3IF(pLayer->IsActive(),"Deleting layer, yet it is not active");

				// We need to find 'prev' and 'next' before we delete the layer
				Layer* pPrevLayer = pLayer->FindPrevLayer();
				Layer* pNextLayer = pLayer->FindNextLayer();

				if (pLayer->IsVisible())
				{
					if (!DoInvalidateLayerRegion(pLayer))
						goto EndOperation;
				}

				NodeRenderableInk::DeselectAllOnLayer(pLayer); 

				if (!DoHideNode(pLayer,TRUE)) // Hide the node 
					goto EndOperation;

				// Note the layer inserted so we can clear any selection on the layer when this op is redone
				// i.e. deleted again after a redo
				pOpRedoLayer = pLayer;

				// If the last layer has been deleted, create a new one and make it the active layer
				if (pSpread->FindFirstLayer() == NULL)
				{
					Layer* pFirstLayer;

					ALLOC_WITH_FAIL(pFirstLayer, (new Layer()), this); 	      
					if (pFirstLayer == NULL)
						goto EndOperation; // We were unable to create a new layer so 
										   // abort the operation 

					// Set the new layer's status and ID  
					pFirstLayer->SetActive(TRUE);
					pFirstLayer->GetLayerStatus().StringLayerID = LayerSGallery::CreateUniqueLayerID(pSpread);

					// Make it the last child of the spread
					if (DoInsertNewNode(pFirstLayer,pSpread,LASTCHILD,FALSE,TRUE,TRUE,TRUE))
					{
						// Note the layer inserted so we can clear any selection on the layer when this op is undone
						// i.e. deleted after an undo
						pOpUndoLayer = pLayer;
					}
				}
				else
				{
					// if we've deleted the active layer, make one of the others active
					if (pLayer->IsActive())
					{
						Layer* pNewActiveLayer = pNextLayer;

						if (pNewActiveLayer == NULL)
							pNewActiveLayer = pPrevLayer;

						if (pNewActiveLayer != NULL)
						{
							if (FrameLayer)
								FrameSGallery::MakeActiveLayer(pNewActiveLayer);
							else
								LayerSGallery::MakeActiveLayer(pNewActiveLayer);
						}

						ERROR3IF(pNewActiveLayer == NULL,"Can't find another layer to make active");

						// Make sure that when it comes back (through an undo) it is not flagged as active
						pLayer->SetActive(FALSE);
					}
				}
			}

			if (FrameLayer)
				UndoIDS = _R(IDS_FRAME_UNDO_DELETE);
			else
			{
				if (Reason == LAYER_DELETE)
					UndoIDS = _R(IDS_LAYER_UNDO_DELETE);
				else
					UndoIDS = _R(IDS_UNDO_DEFAULTBACKGROUND);
			}
			break;

		//--------------------------------------------------------------------------------------------
		// Change a layer's status (i.e. it's layer attributes)
		case (LAYER_CHANGE_NAME):
			UndoIDS = _R(IDS_LAYER_UNDO_CHANGENAME);
			// Fall through!!!!
//		case (LAYER_CHANGE):
			{
				// if UndoIDS has not been changed via the LAYER_CHANGE_NAME entry point,
				// change it to something relevent
				if (UndoIDS == _R(IDS_NOTYPE)) UndoIDS = _R(IDS_LAYER_UNDO_CHANGE);

				// Try to create an action to restore the attribute changes we 
				// are about to make 
			   	ChangeLyrAttribAction* UndoAttribChngAction;                          

				if ( ChangeLyrAttribAction::Init(this,                    
								 	      	  &UndoActions,
									      	  sizeof(InvalidateRegionAction),  
											  pLayer->GetLayerStatus(),
											  pLayer, 
							 			  	  ( Action**)(&UndoAttribChngAction))
							 			  	  != AC_FAIL) 
				{
					LayerStatus OldStatus = pLayer->GetLayerStatus();

					// Get the state of the layer before the change
					BOOL PrevVisible = pLayer->IsVisible();
					BOOL PrevLocked  = pLayer->IsLocked();
					BOOL PrevOutline = pLayer->IsOutline();

					// This is the real state of the visible flag before the change
					BOOL PrevVisibleReal = pLayer->GetVisibleFlagState();

					// change the layer's status info
					pLayer->SetLayerStatus(NewStatus);

					// Get the state of the layer after the change
					BOOL PostVisible = pLayer->IsVisible();
					BOOL PostLocked  = pLayer->IsLocked();
					BOOL PostOutline = pLayer->IsOutline();

					// This is the real state of the visible flag after the change
					BOOL PostVisibleReal = pLayer->GetVisibleFlagState();

					// If the visibility flag has changed, or if visible and the outline state has changed,
					// then do a redraw.
					if ((PrevVisibleReal != PostVisibleReal) ||	(PostVisible && (PrevOutline != PostOutline)))
					{
						if (!DoInvalidateLayerRegion(pLayer))
							goto EndOperation;
					} 

					// If become invisible or become locked, then remove the selections on the layer 
					if ( ((PrevVisible != PostVisible) && !PostVisible) ||
						 ((PrevLocked  != PostLocked)  && PostLocked) )
					{
						NodeRenderableInk::DeselectAllOnLayer(pLayer); 
					} 
					pLayer->EnsureUniqueLayerID();
				}
				else 
					goto EndOperation; // We have failed 

			}
			break;

		//--------------------------------------------------------------------------------------------
		// Move the layer somewhere else
		case (FRAME_MOVE):
			FrameLayer = TRUE;
		case (LAYER_MOVE):
			// Move the layer to the new pos
			if(!DoMoveNode(pLayer, pContextNode, AttDir))
				goto EndOperation; 

			GetApplication()->UpdateSelection();

			// Horrible bodge that ensures that the layer that moves remains the active layer.
			// The DoMoveNode() call above causes a hidden node to be generated that references a layer.
			// This in turn generates a LAYERSCHANGED spread message that causes the layer gallery to do
			// a validity check to ensure that there's an active layer on the spread.
			// If there isn't an active layer (which there isn't in this case because it's just been
			// hidden) the check selects the first layer in the spread.
			// Not easy to fix properly, but easy to bodge it.
			if (FrameLayer)
			{
				// Ensure that the frame layer flag is on
				pLayer->SetFrame(TRUE);
				pLayer->SetEdited(TRUE);
				FrameSGallery::MakeActiveLayer(pLayer);
			}
			else
				LayerSGallery::MakeActiveLayer(pLayer);

			pOpUndoLayer = pLayer;
			pOpRedoLayer = pLayer;

			// Invalidate the region if the layer is visible
			if (pOpLayerGalParam->MoveRedraw)
			{
				if (!DoInvalidateLayerRegion(pLayer))
					goto EndOperation;
			}

			if (pOpLayerGalParam->ToggleBackground)
				LayerStateAction::Init(this,&UndoActions,*pOpLayerGalParam);

			if (FrameLayer)
				UndoIDS = _R(IDS_FRAME_UNDO_MOVE);
			else
				UndoIDS = _R(IDS_LAYER_UNDO_MOVE);
			break;

		//--------------------------------------------------------------------------------------------
		// Copy the layer 
		case (FRAME_COPY):
			FrameLayer = TRUE;
		case (LAYER_COPY):
			{
				BOOL ok = TRUE;
				Layer* pCopyOfLayer = NULL;

				if (pLayer == NULL)
					goto EndOperation;


				// The following lump of code makes sure that the layer being copied does not 
				// contain the insertion node, otherwise it will be copied in the new layer,
				// resulting in the doc having two insertion nodes. (Markn 17/10/96)
				/*
				{
					BaseDocument* pBaseDoc = pLayer->FindOwnerDoc();

					if (pBaseDoc != NULL && IS_A(pBaseDoc,Document))
					{
						Document* pDoc = (Document*)pBaseDoc;
						InsertionNode* pInsertNode = pDoc->GetInsertionPosition();
						if (pInsertNode != NULL)
						{
							Layer* pInsertLayer = (Layer*)pInsertNode->FindParent(CC_RUNTIME_CLASS(Layer));
							if (pInsertLayer == pLayer)
							{
								pInsertNode->UnlinkNodeFromTree();
								delete pInsertNode;
								pInsertNode = NULL;
							}
						}
					}
				}
				*/

				// Copy the layer and all its children, without placing the copy in the tree
				CALL_WITH_FAIL(pLayer->NodeCopy((Node**)&pCopyOfLayer), this, ok);
				if (!ok) goto EndOperation; 

				//Graham 1/8/97: Clear the text tool's internal record of the focus story
				//This is a bug fix
				TextStory::SetFocusStory(NULL);

				// Set the new layer's status  
				pCopyOfLayer->SetLayerStatus(NewStatus); 

				// Set the frame layer flag according to the operation type i.e FRAME_NEW = set flag
				if (FrameLayer)
				{
					pCopyOfLayer->SetFrame(TRUE);
					pCopyOfLayer->SetEdited(TRUE);
				}

				// Insert layer and its children in the tree somewhere
				// Rule: if there is a guide layer at the top, then place the new layer
				// beneath this layer, otherwise, put the new layer at the top.
				Layer* pLastLayer = pSpread->FindLastLayer();
				if(pLastLayer && pLastLayer->IsGuide())
				{
					// this layer is a guide layer - get the next one
					// (there will (ought to be) only one guide layer...)
					pLastLayer = pLastLayer->FindPrevLayer();
				}
				Node* pNodeToInsert;
				if(pLastLayer)
				{
					pNodeToInsert = pLastLayer;
				}
				else
				{
					// what?! can't find a Last Layer to use?
					pNodeToInsert = pContextNode;
				}

				if (!DoInsertNewNode(pCopyOfLayer, pNodeToInsert, NEXT, FALSE))
				{
					// Tidy up if we fail
					pCopyOfLayer->CascadeDelete(); 
					delete pCopyOfLayer; 
					goto EndOperation; 
				} 

				// Call PostDuplicate so things sort their lives out.
				Node* pCurrent = pCopyOfLayer->FindFirstDepthFirst();
				while (pCurrent!=NULL && ok)
				{
					ok = pCurrent->PostDuplicate(this);
					pCurrent = pCurrent->FindNextDepthFirst(pCopyOfLayer);
				}
				if (!ok) goto EndOperation; 
			
				if (!DoInvalidateLayerRegion(pCopyOfLayer))
					goto EndOperation;

				// Note the layer inserted so we can clear any selection on the layer when this op is undone
				// i.e. when the copied layer is removed after an undo
				pOpUndoLayer = pCopyOfLayer;

				// Make it the active layer
				if (FrameLayer)
					FrameSGallery::MakeActiveLayer(pCopyOfLayer);
				else
					LayerSGallery::MakeActiveLayer(pCopyOfLayer);

				NodeRenderableInk::DeselectAllOnLayer(pLayer);
				pCopyOfLayer->EnsureUniqueLayerID();
			}

			if (FrameLayer)
				UndoIDS = _R(IDS_FRAME_UNDO_COPY);
			else
				UndoIDS = _R(IDS_LAYER_UNDO_COPY);
			break;

		//--------------------------------------------------------------------------------------------
		// Toggle the layer's background flag
		case (LAYER_TOGGLEBACKGROUND):
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_LAYER_UNDO_MOVE);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's visibility
		case (LAYER_VISIBLE):
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_LAYER_UNDO_VISIBLE);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's locked state
		case (LAYER_LOCKED):
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_LAYER_UNDO_LOCKED);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's doc's 'all visible' state
		case (FRAME_ALLVISIBLE):
			FrameLayer = TRUE;
		case (LAYER_ALLVISIBLE):
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			if (FrameLayer)
				UndoIDS = _R(IDS_FRAME_UNDO_ALLVISIBLE);
			else
				UndoIDS = _R(IDS_LAYER_UNDO_ALLVISIBLE);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's doc's 'multilayer' state
		case (FRAME_MULTILAYER):
			FrameLayer = TRUE;
		case (LAYER_MULTILAYER):
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			if (FrameLayer)
				UndoIDS = _R(IDS_FRAME_UNDO_MULTILAYER);
			else
				UndoIDS = _R(IDS_LAYER_UNDO_MULTILAYER);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's name, visibility & editability, all on one go
		case (LAYER_CHANGE):
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			//BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_OPCHANGELAYERPROPERTIES);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's solid state
		case (LAYER_SOLID):
// Cannot do this as this causes the gallery items to be destructed whilst handling
// clicks on one of the items.
//			// Check that all visible layers are actually frame layers
//			FrameSGallery::EnsureFrameLayerIntegrity(pSpread);
			// Set up the action
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_LAYER_UNDO_SOLID);
			break;
		//--------------------------------------------------------------------------------------------
		// Change the layer's solid state
		case (LAYER_OVERLAY):
//			// Check that all visible layers are actually frame layers
//			FrameSGallery::EnsureFrameLayerIntegrity(pSpread);
			// Set up the action
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_LAYER_UNDO_OVERLAY);
			break;

		//--------------------------------------------------------------------------------------------
		// Change the layer's active state
		case (LAYER_ACTIVE):
			// Set up the action
			LayerStateAction::Init(this, &UndoActions, *pOpLayerGalParam);
			BroadcastLayerChanges = FALSE;
			UndoIDS = _R(IDS_LAYER_UNDO_ACTIVE);
			break;

		// Trap any other reasons here
		default:
			ERROR3("Unknown OpLayerGalChange reason code");
			break;
	}

	if (BroadcastLayerChanges)
	{
		BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
	}

 	EndOperation:

	ERROR3IF(UndoIDS == 0,"The undo ID string is zero. No too useful!");
 	End(); 
}

/********************************************************************************************

>	BOOL OpLayerGalChange::DoInvalidateLayerRegion(Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/95
	Inputs:		pLayer = ptr to layer
	Outputs:	-
	Returns:	TRUE for success, FALSE otherwise
	Purpose:	This invalidates the areas of the screen covered by the given layer
				This copes with guidelines if the given layer is a guides layer

				We have to do guidelines separately, because guidelines don't get included in the bounding
				rect of the layer, hence DoInvalidateNodeRegion(pLayer) won't work
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerGalChange::DoInvalidateLayerRegion(Layer* pLayer)
{
	ERROR2IF(pLayer  == NULL,FALSE, "pLayer is NULL");
	ERROR2IF(pSpread == NULL,FALSE,"pSpread is NULL");

	BOOL ok = TRUE;

	if (pLayer->IsGuide())
	{
		Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
		while (pNode != NULL && ok)
		{
			DocRect Rect = ((NodeGuideline*)pNode)->GetRenderRect();
			ok = DoInvalidateRegion(pSpread,Rect);

			pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
		}
	}

	if (ok) ok = DoInvalidateNodeRegion(pLayer, TRUE);

	return ok;
}

/********************************************************************************************

>	void OpLayerGalChange::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
					
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpLayerGalChange::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(UndoIDS);
}  


/********************************************************************************************

>	void OpLayerGalChange::Undo() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The overloaded Undo operation refreshes the layer details in the layer 
				manager   
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerGalChange::Undo() 
{
	// Do some stuff before the undo takes place, relevent to the operation being undone
	switch (Reason)
	{
		// Only LAYER_NEW and LAYER_COPY need to worry if pOpUndoLayer is NULL
		// LAYER_DELETE op only creates a pOpUndoLayer ptr after the last layer is deleted and
		// automatically replaced by a new blank layer.

		case FRAME_NEW:
		case FRAME_COPY:
		case LAYER_NEW:
		case LAYER_COPY:

			ERROR3IF(pOpUndoLayer == NULL,"The op's undo layer ptr is NULL");

			// **** Fall through...

		case FRAME_DELETE:
		case LAYER_DELETE:
		case PAGEBACKGROUND_DEFAULT:

			// If undoing, make sure there are no selected nodes on the layer before
			// it disappears
			if (pOpUndoLayer != NULL && pOpUndoLayer->FindParent() != NULL)
				NodeRenderableInk::DeselectAllOnLayer(pOpUndoLayer);

			// **** Fall through...

		case FRAME_MOVE:
		case LAYER_MOVE:

			// All the above reasons generate hidden nodes, so the HideNode and ShowNode code
			// will broadcast layer changes on undo and redo (hopefully!)
			BroadcastLayerChanges = FALSE;
			break;
	}

	BOOL Result = Operation::Undo(); 
	if (Result)
	{
		// Do some stuff after the undo takes place, relevent to the operation being undone
		switch (Reason)
		{
			// When moving layers about, we need to make sure the layer moved remains the active layer
			case FRAME_MOVE:
			case LAYER_MOVE:
				GetApplication()->UpdateSelection();
				if (pOpUndoLayer != NULL)
				{
					if (Reason == FRAME_MOVE)
						FrameSGallery::MakeActiveLayer(pOpUndoLayer);
					else
						LayerSGallery::MakeActiveLayer(pOpUndoLayer);
					BroadcastLayerChanges = TRUE;
				}
				break;
		}		

		if (BroadcastLayerChanges)
		{
			BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
		}
	}
	return (Result); 
}		

/********************************************************************************************

>	void OpLayerGalChange::Redo()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The overloaded Redo operation refreshes the layer details in the layer
				manager 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerGalChange::Redo()
{
	// Do some stuff before the redo takes place, relevent to the operation being redone
	switch (Reason)
	{
		case FRAME_DELETE:
		case LAYER_DELETE:
		case PAGEBACKGROUND_DEFAULT:

			ERROR3IF(pOpRedoLayer == NULL,"The op's redo layer ptr is NULL");

			// If redoing a layer delete, make sure there are no selected nodes on the layer before
			// it disappears
			if (pOpRedoLayer != NULL && pOpRedoLayer->FindParent() != NULL)
				NodeRenderableInk::DeselectAllOnLayer(pOpRedoLayer);

			// **** Fall through...

		case FRAME_NEW:
		case FRAME_COPY:
		case FRAME_MOVE:
		case LAYER_NEW:
		case LAYER_COPY:
		case LAYER_MOVE:

			// All the above reasons generate hidden nodes, so the HideNode and ShowNode code
			// will broadcast layer changes on undo and redo (hopefully!)
			BroadcastLayerChanges = FALSE;
			break;
	}

	BOOL Result = Operation::Redo(); 
	if (Result)
	{
		// Do some stuff after the redo takes place, relevent to the operation being redone
		switch (Reason)
		{
			// When moving layers about, we need to make sure the layer moved remains the active layer
			case FRAME_MOVE:
			case LAYER_MOVE:
				GetApplication()->UpdateSelection();
				if (pOpRedoLayer != NULL)
				{
					if (Reason == FRAME_MOVE)
						FrameSGallery::MakeActiveLayer(pOpRedoLayer);
					else
						LayerSGallery::MakeActiveLayer(pOpRedoLayer);
					BroadcastLayerChanges = TRUE;
				}
				break;
		}		

 		if (BroadcastLayerChanges)
		{
 			BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
		}
	}
	return (Result); 
}

/********************************************************************************************

>	static BOOL OpLayerGalChange::IsFrameMode()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the document is frame based, else FALSE.
	Purpose:	
	
********************************************************************************************/

BOOL OpLayerGalChange::IsFrameMode()
{
	// Flag variables.
	BOOL FramedDoc = FALSE;		// Flag to determine the document type.
	
	// Get a ptr to the selected spread
	Spread* pSpread = Document::GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			FramedDoc = TRUE;
	}

	return FramedDoc;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
//------------------------------------------------------------------------

// The layer stuff dialog
/********************************************************************************************

>	LayerNameDlg::LayerNameDlg(): DialogOp(LayerNameDlg::IDD, LayerNameDlg::Mode) 


	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerNameDlg constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				

LayerNameDlg::LayerNameDlg(): DialogOp(LayerNameDlg::IDD, LayerNameDlg::Mode) 
{   
	pLayer  = NULL;
	pSpread = NULL;
}        

/********************************************************************************************

>	MsgResult LayerNameDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the scale dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult LayerNameDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		BOOL EndDialog = FALSE;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				ShowLayerDetails();
				LayerNameDlg::IsOpen = TRUE;
				LayerNameDlg::pLayerNameDlg = this;
				SetKeyboardFocus(_R(IDC_LAYER_NAME));
				HighlightText (_R(IDC_LAYER_NAME));
				break;
				
			case DIM_COMMIT:
				EndDialog = TRUE;
				CommitDialogValues();
				break;

			case DIM_SOFT_COMMIT:
				//TRACE( _T("Rgt OK handling here\n"));
				break; 

			case DIM_CANCEL:
				EndDialog = TRUE;
				break;

		    case DIM_LFT_BN_CLICKED:
				break; 

			case DIM_TEXT_CHANGED:
				break;
		}

		if (EndDialog)	// Dialog communication over 
		{			   // Isn't this pretty?
			Close();  // Close the dialog 
			End(); 	 // Destroy dialog 

			LayerNameDlg::IsOpen 		= FALSE;	// We no longer have a current layer name dialog
			LayerNameDlg::pLayerNameDlg = NULL;		// so reset the static vars
	   	}
//		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
//	return OK; 

	// Pass everything on to the base class . . .
	return DialogOp::Message(Message);
}  


/********************************************************************************************

>	OpState	LayerNameDlg::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerNameDlg GetState method
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	LayerNameDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}
		 
/********************************************************************************************

>	BOOL LayerNameDlg::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerNameDlg Init method
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



BOOL LayerNameDlg::Init()
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_LAYERNAMEDLG),
								CC_RUNTIME_CLASS(LayerNameDlg),
								OPTOKEN_LAYERNAMEDLG,
								LayerNameDlg::GetState,
								0,	// help ID
								0   // bubble ID
								)
			); 
}   
 
/********************************************************************************************

>	static void LayerNameDlg::StartUp(LayerDlgParam* pParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		pParam - ptr to all the info the dlg needs in order to do its stuff
	Outputs:	-
	Returns:	-
	Purpose:	Starts up a LayerNameDlg using the OPTOKEN_LAYERNAMEDLG op token
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerNameDlg::StartUp(LayerDlgParam* pParam)
{
	if (!LayerNameDlg::IsOpen)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERNAMEDLG); 
		if (pOpDesc != NULL)
			pOpDesc->Invoke((OpParam*)pParam);
		else
		{
			ERROR3("Couldn't find OPTOKEN_LAYERNAMEDLG op descriptor");
		}
	}
	else if (LayerNameDlg::pLayerNameDlg != NULL)
		SetActiveWindow(LayerNameDlg::pLayerNameDlg->WindowID);
}

	   
/********************************************************************************************

>	BOOL LayerNameDlg::Create()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	LayerNameDlg create method 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

	   
BOOL LayerNameDlg::Create()
{
	if (DialogOp::Create())
	{
		//ShowLayerDetails();
		return TRUE;
	}
	else
		return FALSE;

}

/********************************************************************************************

>	void LayerNameDlg::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		pOpParam = ptr to a OpLayerGalReason struct, cast as an OpParam* 
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void LayerNameDlg::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	// If there's already one layer name dlg open, return.
	if (LayerNameDlg::IsOpen)
		return;

	LayerDlgParam* pParam = (LayerDlgParam*)pOpParam;

	// Why are you creating me. I need to know, God damn it!
	Reason = pParam->GetReason();

	// Extract the spread and layer in question
	pSpread = pParam->GetSpread();
	pLayer  = pParam->GetLayer();

	// If we have a layer but not a spread, find the spread via the given layer
	if (pLayer != NULL && pSpread == NULL)
		pSpread = pLayer->FindParentSpread();

	BOOL ok=FALSE;
	
	switch (Reason)
	{
		// If  we're changing the layer's name or copying it, we need both a layer and a spread
		case LAYER_CHANGE_NAME:
		case LAYER_COPY:
			ERROR3IF(pLayer  == NULL, "I don't have a layer");
			ERROR3IF(pSpread == NULL, "I dont' have a spread");
			ok = ((pLayer != NULL) && (pSpread != NULL));
			break;

		case LAYER_NEW:
			ERROR3IF(pSpread == NULL, "I dont' have a spread");
			ok = (pSpread != NULL);
			break;

		default:
			ERROR3("The layer name dialog can't process the given reason");
			ok = FALSE;
			break;
	}

	if (ok) ok = Create();
//	if (ok)	Open();

	if (!ok)			// Could not create 
	{
		InformError(0,_R(IDS_OK));
  		End(); 		   	// End the operation 
	}
//	else
//	{
//    	LayerNameDlg::IsOpen 		= TRUE;
//    	LayerNameDlg::pLayerNameDlg = this;
//	}
}


/********************************************************************************************

>	void LayerNameDlg::ShowLayerDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Shows layer details in the dialog
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerNameDlg::ShowLayerDetails()
{
	String_256 Name;

	switch (Reason)
	{
		case LAYER_CHANGE_NAME:
			Name = pLayer->GetLayerID();
			break;

		case LAYER_COPY:
			CreateUniqueLayerCopyName(pLayer->GetLayerID(), &Name);
			break;
			break;

		case LAYER_NEW:
			Name = LayerSGallery::CreateUniqueLayerID(pSpread);
			break;

		default:
			ERROR3("Unknown OpLayerGalReason reason");
			break;
	}

	SetStringGadgetValue(_R(IDC_LAYER_NAME),&Name);
}



/********************************************************************************************
>	static void LayerNameDlg::CreateUniqueLayerCopyName(const String_256& strName,
												 		String_256* pCopyName)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/95
	Inputs:		strName			the name of the layer to be copied			
	Outputs:	pCopyName		the name of copy of the layer, which is unique
	Purpose:	Generates a unique name for a layer to be copied.
	SeeAlso:	LayerNameDlg::ShowLayerDetails
********************************************************************************************/

void LayerNameDlg::CreateUniqueLayerCopyName(const String_256& strName, String_256* pCopyName)
{
	// First, check for junk inputs.
	ERROR3IF(pCopyName == NULL,
				"Null output String_256* in LayerNameDlg::CreateUniqueLayerCopyName");

	// The rest of this function is a complete bodge, because to do this properly would
	// mean a rewrite of the brain-damaged way that layers are named, copied, and
	// identified as unique.  So what I'm going to do is prepend a "Copy of " string
	// and make sure it doesn't overflow when I do.
	pCopyName->Load(_R(IDS_LAYERGAL_COPYOF));
	String_256 strNewName;
	if ((strName.Length() + pCopyName->Length()) >= pCopyName->MaxLength())
	{
		// We must truncate so it won't overflow.
		strName.Left(&strNewName, pCopyName->MaxLength() - pCopyName->Length());
	}
	else
	{
		// We don't need to truncate.
		strNewName = strName;
	}

	// Prepend and return the result.
	*pCopyName += strNewName;
}



/********************************************************************************************

>	void LayerNameDlg::CommitDialogValues()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Takes the values in the dialog and sets the Layer object
				accordingly.
				Called when the OK button is selected
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerNameDlg::CommitDialogValues()
{
	String_256 NewName;
	char* pD = NewName;
	BOOL Valid;

	// Get the string from the layer name dialog
	NewName = GetStringGadgetValue(_R(IDC_LAYER_NAME),&Valid);

	// If we're changing the layer's name, but the name hasn't changed, 
	// return without doing anything
	if (Reason == LAYER_CHANGE_NAME)
	{
		if (pLayer->GetLayerID() == NewName)
			return;
	}

	// Initialise the param structure
	OpLayerGalParam Param(Reason, pSpread);
	Param.pLayer = pLayer;

	// Change the name to the new name
	Param.Status.StringLayerID = NewName;

	// If we are copying or creating a new the layer, set up the context node 
	// and where to place it relative to the context node
	if (Reason == LAYER_COPY || Reason == LAYER_NEW)
	{
		Param.pContextNode  = pSpread;
		Param.AttDir		= LASTCHILD;
	}

	// Invoke the operation
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_LAYERGALCHANGE); 
	if (pOpDesc != NULL)
		pOpDesc->Invoke((OpParam*)&Param);
	else
	{
		ERROR3("Couldn't find OPTOKEN_LAYERGALCHANGE op descriptor");
	}
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

// OpDisplayLayerGallery - the operation that is used to display the layers gallery

/********************************************************************************************

>	BOOL OpDisplayLayerGallery::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayLayerGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayLayerGallery::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_LAYER_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayLayerGallery),
	 							OPTOKEN_DISPLAYLAYERGALLERY,
	 							OpDisplayLayerGallery::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_DISPLAY_LAYER_GALLERY),
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpDisplayLayerGallery::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayLayerGallery operation
	Purpose:	For finding the OpDisplayLayerGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayLayerGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	// The Layer gallery op has been modified so that 
	// it can only be opened on a layer based document.
	
	OpState OpSt;  

	// Flag variables.
	BOOL LayerDoc = TRUE;		// Flag to determine whether this is a layer document.

	// Get a ptr to the selected spread
	Spread* pSpread = Document::GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			LayerDoc = FALSE;
	}

	if(LayerDoc)
	{
		// If the gallery is currenty open, then the menu item should be ticked
		String_32 Name(_R(IDS_LAYERGAL_GALLNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

		if (pDialogBarOp != NULL)
		{
			if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(LayerSGallery))
			OpSt.Ticked = pDialogBarOp->IsVisible();
		}

		// If there are no open documents, you can't toggle the gallery
		OpSt.Greyed = (Document::GetSelected() == NULL);
	}
	else
	{
		// This is a frame document, so set the menu item to the correct state.
		OpSt.Ticked = FALSE;
		OpSt.Greyed	= TRUE;
		*UIDescription = String_256 (_R(IDS_NO_LAYERS));
	}

	return(OpSt);   
}

/********************************************************************************************

>	void OpDisplayLayerGallery::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the layers gallery
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayLayerGallery::Do(OpDescriptor*)
{
	String_32 Name(_R(IDS_LAYERGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(LayerSGallery))
		{
			// Toggle the visible state of the gallery window
			pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );
		}
		else
		{
			ERROR3("Got the layer gallery but it's not of the LayerSGallery class");
		}

		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYLAYERGALLERY, pDialogBarOp->IsVisible());
	}

	ERROR3IF(pDialogBarOp == NULL,"Couldn't find the layer gallery bar");

	End();
}
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------

// OpDisplayFrameGallery - the operation that is used to display the frame gallery

/********************************************************************************************

>	BOOL OpDisplayFrameGallery::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayFrameGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayFrameGallery::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_FRAME_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayFrameGallery),
	 							OPTOKEN_DISPLAYFRAMEGALLERY,
	 							OpDisplayFrameGallery::GetState,
	 							_R(IDH_Gallery_Frame),	/* help ID */
	 							_R(IDBBL_DISPLAY_FRAME_GALLERY),
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpDisplayFrameGallery::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayFrameGallery operation
	Purpose:	For finding the OpDisplayFrameGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayFrameGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	// The frame gallery op has been modified so that 
	// it can only be opened on a frame based document.

	OpState OpSt;

#ifdef WEBSTER
		
	// If the gallery is currenty open, then the menu item should be ticked
	String_32 Name(_R(IDS_FRAMEGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(FrameSGallery))
			OpSt.Ticked = pDialogBarOp->IsVisible();
	}

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == NULL);

#else

	// Flag variables.
	BOOL FramedDoc = FALSE;		// Flag to determine whether this is a framelayer(animation) document.

	// Get a ptr to the selected spread
	Spread* pSpread = Document::GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			FramedDoc = TRUE;
	}

	// If this is an animation Doc, then set the correct menu item state.
	if(FramedDoc)
	{
		// If the gallery is currenty open, then the menu item should be ticked
		String_32 Name(_R(IDS_FRAMEGAL_GALLNAME));
		DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

		if (pDialogBarOp != NULL)
		{
			if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(FrameSGallery))
				OpSt.Ticked = pDialogBarOp->IsVisible();
		}

		// If there are no open documents, you can't toggle the gallery
		OpSt.Greyed = (Document::GetSelected() == NULL);
	}
	else
	{
		// This is a layer document, so the Frame Gallery isn't needed.
		//	Set up item up as greyed and give a reason.
		OpSt.Ticked = FALSE;
		OpSt.Greyed	= TRUE;
		*UIDescription = String_256 (_R(IDS_NO_FRAMES));
	}

#endif

	// Lets go back..
	return(OpSt);   
}

/********************************************************************************************

>	void OpDisplayFrameGallery::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the frame gallery
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayFrameGallery::Do(OpDescriptor*)
{
	String_32 Name(_R(IDS_FRAMEGAL_GALLNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(FrameSGallery))
		{
			GIFAnimationPropertyTabs::SetFrameGalleryOpen(FALSE);

			// Toggle the visible state of the gallery window
			pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );
		}
		else
		{
			ERROR3("Got the frame gallery but it's not of the LayerSGallery class");
		}

		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYFRAMEGALLERY, pDialogBarOp->IsVisible());

		GIFAnimationPropertyTabs::SetFrameGalleryOpen(TRUE);
	}

	ERROR3IF(pDialogBarOp == NULL,"Couldn't find the frame gallery bar");

	End();
}

//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------

/********************************************************************************************

>	LayerStateAction::LayerStateAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LayerStateAction::LayerStateAction()
{
}


/********************************************************************************************

>	ActionCode LayerStateAction::Init(UndoableOperation* pOp,
										ActionList* pActionList,
										OpLayerGalParam EntryParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		pOp  		= ptr to the operation to which this action belongs
				pActionList = ptr to action list to which this action should be added
				Param		= Contains all the info needed to change the layer
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This changes a flag associated with the layer to the value in 'NewState' in an
				undoable way
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode LayerStateAction::Init(	UndoableOperation* pOp,
									ActionList* pActionList,
									OpLayerGalParam EntryParam)
{
	UINT32 ActSize = sizeof(LayerStateAction);

	LayerStateAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(LayerStateAction),(Action**)&pNewAction);

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		pNewAction->Param = EntryParam;
		OpLayerGalParam& Param = pNewAction->Param;

		switch (Param.GetReason())
		{
			case LAYER_VISIBLE:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");

				if (Param.pLayer != NULL)
					LayerSGallery::DoChangeVisible(Param.pLayer,Param.NewState);
				break;

			case LAYER_LOCKED:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");

				if (Param.pLayer != NULL)
					LayerSGallery::DoChangeLocked(Param.pLayer,Param.NewState);
				break;

			case LAYER_ALLVISIBLE:
			case FRAME_ALLVISIBLE:
				if (Param.pLayerSGal != NULL)
				{
					Param.pLayerSGal->DoChangeAllVisible(Param.NewState);
					Param.pLayerSGal->ForceRedrawOfList();
				}
				break;

			case LAYER_MULTILAYER:
			case FRAME_MULTILAYER:
				if (Param.pLayerSGal != NULL)
				{
					Param.pLayerSGal->DoChangeMultilayer(Param.NewState);
					Param.pLayerSGal->ForceRedrawOfList();
				}
				break;

			case FRAME_MOVE:
			case LAYER_MOVE:
			case LAYER_TOGGLEBACKGROUND:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");
				ERROR3IF(Param.pLayerSGal == NULL,"Param.pLayerSGal == NULL");

				if (Param.pLayer != NULL)
				{
					if (Param.GetReason() == FRAME_MOVE)
					{
						Param.pLayer->SetBackground(FALSE);
						Param.pLayer->SetPrintable(!Param.pLayer->IsPrintable());
					}
					else
					{
						Param.pLayer->SetBackground(!Param.pLayer->IsBackground());
						Param.pLayer->SetPrintable(!Param.pLayer->IsPrintable());
					}
					// Cause the layer gallery items to be recreated
					Param.pLayerSGal->CreateNewSubtree(Param.pLayer->FindDocument());
				}
				break;

			case LAYER_CHANGE:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");

				if (Param.pLayer != NULL)
				{
					Param.OldVisibleState 	= Param.pLayer->IsVisible();
					Param.OldLockedState 	= Param.pLayer->IsLocked();
					Param.OldName 			= Param.pLayer->GetLayerID();

					LayerSGallery::DoChangeVisible(Param.pLayer,Param.VisibleState);
					LayerSGallery::DoChangeLocked(Param.pLayer,Param.LockedState);
					Param.pLayer->SetLayerID(Param.NewName);
				}
				break;

			case LAYER_SOLID:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");

				if (Param.pLayer != NULL)
				{
					FrameSGallery::DoChangeSolid(Param.pLayer,Param.NewState);
					Param.pLayer->SetEdited(TRUE);
					Param.pLayer->SetFrame(TRUE);	// double check this
#ifdef _DEBUG
					// Tell the frame gallery to update its display of the frame
					BROADCAST_TO_ALL(LayerMsg(Param.pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
#endif
				}
				break;

			case LAYER_OVERLAY:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");

				if (Param.pLayer != NULL)
				{
					FrameSGallery::DoChangeOverlay(Param.pLayer,Param.NewState);
					Param.pLayer->SetEdited(TRUE);
					Param.pLayer->SetFrame(TRUE);	// double check this
#ifdef _DEBUG
					// Tell the frame gallery to update its display of the frame
					BROADCAST_TO_ALL(LayerMsg(Param.pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
#endif
				}
				break;

			case LAYER_MAKEFRAME:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");
				if (Param.pLayer != NULL)
				{
					Param.pLayer->SetFrame(Param.NewState);
				}
				break;

			case LAYER_ACTIVE:
				ERROR3IF(Param.pLayer == NULL,"Param.pLayer == NULL");
				if (Param.pLayer != NULL)
				{
					Param.pLayer->SetActive(Param.NewState);

					// Ensure that the gallery is updated with the new active layer details
					BROADCAST_TO_ALL(LayerMsg(Param.pLayer, LayerMsg::LayerReason::UPDATE_ACTIVE_LAYER));
				}
				break;

			default:
				ERROR3("Unknown Reason code");
				break;
		}
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode LayerStateAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/95
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
				Puts the action's layer's visible state to the opposite setting.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode LayerStateAction::Execute()
{
	ActionCode Ac = AC_FAIL;

	if (pOperation->IS_KIND_OF(UndoableOperation))
	{
		Param.NewState = !Param.NewState;

		// Swap LAYER_CHANGE vars
		Swap(Param.VisibleState,Param.OldVisibleState);
		Swap(Param.LockedState, Param.OldLockedState);
		String_256 Str 		= Param.OldName;
		Param.OldName 		= Param.NewName;
		Param.NewName 		= Str;

		Ac = LayerStateAction::Init((UndoableOperation*)pOperation,pOppositeActLst,Param);

		if (Ac != AC_FAIL)
		{
			LayerSGallery* 	pGal = Param.pLayerSGal;

			if (pGal != NULL)
			{
				pGal->ForceRedrawOfList();

				Document* pDoc = GetWorkingDoc();

				if (pDoc != NULL && pGal->IsVisible())
				{
					pGal->SetBoolGadgetSelected(_R(IDC_BTN_MULTILAYER),!(pDoc->IsMultilayer() == 0));
					pGal->SetBoolGadgetSelected(_R(IDC_BTN_ALLVISIBLE),!(pDoc->IsAllVisible() == 0));
				}
			}
		}
	}
	else
	{
		ERROR3("LayerStateAction::Execute() called with op that's not an undoable op");
	}

	return Ac;
}

LayerStateAction::~LayerStateAction()
{
}
