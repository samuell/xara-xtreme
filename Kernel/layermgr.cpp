// $Id: layermgr.cpp 662 2006-03-14 21:31:49Z alex $
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

/* Implementation of the following classes
	LyrDetails
	LayerManager
	OpLayerChange
*/ 

// Include files 
#include "camtypes.h"
#include "layermgr.h"
#include "fixst256.h"
#include "ensure.h"
#include "mainfrm.h" 
#include "infobar.h" 
//#include "simon.h"
//#include "mario.h"
#include "errors.h"
#include "document.h"
#include "docview.h"
#include "spread.h"
#include "ink.h"
#include "bubbleid.h"
//#include "sliceres.h" // for the resource id move to active layer
#include "objchge.h" // for the allow op flags

//#include "will2.h"
#include "selop.h"

DECLARE_SOURCE("$Revision: 662 $");

// Declare class for memory tracking etc
CC_IMPLEMENT_DYNCREATE(OpLayerChange, UndoableOperation) 
CC_IMPLEMENT_DYNCREATE(ChangeLyrAttribAction, Action)
CC_IMPLEMENT_MEMDUMP(LyrDetails, ListItem)
CC_IMPLEMENT_DYNCREATE(OpMoveToLayer, SelOperation) 
#define new CAM_DEBUG_NEW    

/********************************************************************************************

>	LyrDetails::LyrDetails()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LyrDetails constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LyrDetails::LyrDetails()
{
}

/********************************************************************************************

>	LayerManager::LayerManager()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerManager Constructor, initially the LayerManager has no CurrentSpread. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LayerManager::LayerManager()
{
	CurrentSpread = NULL; // There is no current spread 
}

/********************************************************************************************

>	LayerManager::~LayerManager()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerManager destructor 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LayerManager::~LayerManager()
{
	DeleteLayerDetList(); // Delete the layer details list 	
}

/********************************************************************************************

>	 String_256 LayerManager::CreateUniqueLayerID()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	A LayerID which is unique for the current spread. 
	Purpose:	Used to generate a LayerID which is unique for the current spread. The name 
				generated is normally of the form "Layer n", where n is the number of layers 
				in the LyrDetList + 1. If "Layer n" already exists on the current spread then 
				"Layer m" is generated where m is the smallest integer greater than n such 
				that the id is unique.     
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256 LayerManager::CreateUniqueLayerID()
{
	// We can't generate a unique id if there is no current spread 
	ENSURE(CurrentSpread != NULL, "There is no current spread"); 

	String_256 UniqueLayerID; 
	INT32 NumLayers = 0; 

	ListItem* Lyr = LyrDetList.GetHead(); 

	// Calculate how many layers are on the current spread, we must ignore layers 
	// which have been deleted. 
	while (Lyr != NULL)
	{
		if (!((LyrDetails*)Lyr)->Deleted)	 // Don't count deleted layers 
		{
			NumLayers++; 	
		}
		Lyr = LyrDetList.GetNext(Lyr); 
	}

	ENSURE(NumLayers != 0, "The current spread has no layers"); 


	INT32 NewLayerNum = NumLayers+1; 

	BOOL Unique;  // Flag used to indicate if the id generated is unique or not 
	do 
	{
		// Construct a first attempt at a unique layer id 'Layer n' 
		// where n = the number of layers on the current spread + 1
	 
		UniqueLayerID.MakeMsg(_R(IDS_LAYERMGR_LAYERNAME),NewLayerNum);   
	
		// Check that UniqueLayerID is indeed unique 
		Unique = TRUE; // Until we know better 
		Lyr = LyrDetList.GetHead(); 
		while (Lyr != NULL)
		{	
			if (!((LyrDetails*)Lyr)->Deleted)	// Ignore deleted layers 
			{
               	if (UniqueLayerID == ((LyrDetails*)Lyr)->Status.StringLayerID)
				{
					Unique = FALSE; 

					// UniqueLayerID is not unique so increment NewLayerNum and try again 
					NewLayerNum++;      
					break; 
				}
			}
			Lyr = LyrDetList.GetNext(Lyr); // Get the next layer  
		}
		
	} while (!Unique); 
	return(UniqueLayerID); 
}

/********************************************************************************************

>	BOOL LayerManager::CommitLayerChanges()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns TRUE if successful, else InformError gets called and FALSE returned. 

	Purpose:	If the layer details have changed then performs a LayerChangeOp which 
				modifies the Camelot tree to reflect the changes made to the LyrDetList. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL LayerManager::CommitLayerChanges()
{
	if (LayerDetailsChanged()) // The layer details have changed so we need to perform 
							   // a LayerChangeOp to commit the changes.  
	{
		// Invoke the layer change operation 
		// Obtain a pointer to the op descriptor for the Layer change operation 
		OpDescriptor* OpDesc = 
			OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpLayerChange)); 
		// Invoke the operation. 
		OpDesc->Invoke();
	}
	if (RefreshLayerDetails()) // We need to refresh the layer details even if the layers were 
						   	   // not changed because a new layer could have been deleted  
	{
		return TRUE; 
	}
	else return FALSE; // Failed to refresh layer details (InformError has been called)
} 				

/********************************************************************************************

>	BOOL LayerManager::NewCurrentSpread(Spread* NewCurrentSpread) 				

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		NewCurrentSpread
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets a new current spread. 
	Errors:		InformError gets called and FALSE is returned if we run out of memory
	SeeAlso:	-

********************************************************************************************/


BOOL LayerManager::NewCurrentSpread(Spread* NewCurrentSpread) 				
{
	ENSURE(NewCurrentSpread != NULL, "The new current spread is NULL"); 
	BOOL ok = TRUE;
	
	if (CurrentSpread != NULL)	// There is already a current spread 
	{
		
		// The LayerDetList should never be empty 
		ENSURE(!LyrDetList.IsEmpty(), "Empty Layer details list"); 

		DeleteLayerDetList();  
	
	}
	
	// The LayerDetList should be empty 
	ENSURE(LyrDetList.IsEmpty(), "Empty Layer details list"); 

	CurrentSpread = NewCurrentSpread; 

	// Read the current spread's layer details into the LyrDetList
	ok = ReadSpreadLayerDetails();

	return (ok);
}

/********************************************************************************************

>	Spread* LayerManager::GetCurrentSpread()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:		-
	Outputs:	-
	Returns:	The LayerManagers current spread
	Purpose:	For finding the current spread
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Spread* LayerManager::GetCurrentSpread()
{
	return CurrentSpread; 
}

/********************************************************************************************

>	void LayerManager::UpdateInterface()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Updates the interface after layers have changed. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void LayerManager::UpdateInterface()
{
//	BroadcastNewCurrentSpread(); 
}

/********************************************************************************************

>	void LayerManager::BroadcastNewCurrentSpread()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This private function gets called from NewCurrentSpread. It informs all 
				subsystems (that need to know) that the Current Spread	has changed. The 
				LyrDetList contains the new current spread's details before this function is 
				called.  

	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

/*
void LayerManager::BroadcastNewCurrentSpread()
{
	// The InfoBar needs to know that the current spread has changed so that it can update
	// the layer list gadget. 

//	4/5/94 - Markn changed this as infobar can now be NULL
//	ENSURE(CMainFrame::pInfoBar != NULL, "Infobar pointer is NULL in main frame");  
//	CMainFrame::pInfoBar->NewCurrentSpread(); 

	if (CMainFrame::pInfoBar != NULL)
		CMainFrame::pInfoBar->NewCurrentSpread();
}
*/

/********************************************************************************************

>	BOOL LayerManager::CancelLayerChanges()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Causes the changes made to the LyrDetList to be discarded, and new values read. 
	Errors:		If memory runs out then InformError is called and the function returns FALSE
	SeeAlso:	-

********************************************************************************************/

BOOL LayerManager::CancelLayerChanges()
{
	DeleteLayerDetList(); 	  // Delete details including any changes whioch may have been made
	if (ReadSpreadLayerDetails()) // Read new values 
	{
		return TRUE; 
	}
	else 
	{
		return FALSE; 
	}
} 

/********************************************************************************************

>	BOOL LayerManager::RefreshLayerDetails()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns TRUE if successful, else calls InformError and returns FALSE
	Purpose:	Reads in new layer details and updates the interface. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL LayerManager::RefreshLayerDetails()
{
	// Make sure that no changes have been made 
	DeleteLayerDetList(); 	  // Delete details including any changes whioch may have been made
	if (ReadSpreadLayerDetails()) // Read new values 
	{
		UpdateInterface(); 	
		return TRUE; 	
	}
	else return FALSE; 	 // InformError has been called 
}


/********************************************************************************************

>	void LayerManager::DeleteLayerDetList()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the contents of the LyrDetList 
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

void LayerManager::DeleteLayerDetList()
{ 
	// Delete all current spread details 
	ListItem* Current = LyrDetList.GetHead(); 
	while (Current != NULL)
	{
		delete (LyrDetList.RemoveItem(Current)); 
		Current = LyrDetList.GetHead(); 
	}				 
}


/********************************************************************************************

>	BOOL LayerManager::ReadSpreadLayerDetails()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Read the current spread's layer details into the LyrDetList. If 
				memory runs out then InformError is called and FALSE is returned. 

	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL LayerManager::ReadSpreadLayerDetails() 
{
	ENSURE(CurrentSpread != NULL, "Trying to read the details of a NULL current spread"); 
	ENSURE(LyrDetList.IsEmpty(), "Layer details list not empty"); 

	LyrDetails* LayerDet; 

	Layer* Current = CurrentSpread->FindFirstLayer(); 
	while (Current != NULL) 
	{
		// Found a layer so record the details 
		LayerDet = new LyrDetails(); 
		if (LayerDet == NULL)
		{
			// Inform an error - The error will already be set by new
			InformError();
			return FALSE; 
		}
		LayerDet->Status = (Current)->GetLayerStatus(); // record the layer status  
		LayerDet->Layer = Current; 
		LayerDet->New = FALSE; 
		LayerDet->Deleted = FALSE; 

		LyrDetList.AddTail(LayerDet); // Add the layers details to the tail of the layer 
									   // detail list, so preserving the correct z-order. 
		Current = Current->FindNextLayer(); 
	}
	ENSURE(!LyrDetList.IsEmpty(), "No layers found on the current spread"); 
	return TRUE; 
}

/********************************************************************************************

>	BOOL LayerManager::LayerDetailsChanged()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the LayerDetails list has changed 
	Purpose:	This function compares the current spread's layers against the Layer details 
				list. If the layers have changed in any way, then the function returns TRUE. 
				Otherwise the function returns FALSE. 
				
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL LayerManager::LayerDetailsChanged()
{
	LyrDetails* CurrentLyrDet = (LyrDetails*)LyrDetList.GetHead(); 	  
	Layer* CurrentLyr = CurrentSpread->FindFirstLayer(); // Get the first layer node 
	
	ENSURE(CurrentLyrDet != NULL, "There are no layer details on the layer details list"); 
	ENSURE(CurrentLyr != NULL, "The LayerManager's current spread has no layers"); 
	
	while (CurrentLyrDet != NULL) 
	{
		// We ignore deleted new layers because they don't change anything 
		if (!(CurrentLyrDet->New && CurrentLyrDet->Deleted)) 	
		{		
			if (CurrentLyr != CurrentLyrDet->Layer) // z-order change 
			{
				return TRUE; 
			}									
			else if ((CurrentLyrDet->New) || (CurrentLyrDet->Deleted)) 
			{
				return TRUE; // New layer created or existing layer deleted 
			}
			else if (!(CurrentLyrDet->Status == CurrentLyr->GetLayerStatus()))
			{	
				// The layer's status has changed 
				return TRUE; 
			}  
			CurrentLyrDet =  (LyrDetails*)(LyrDetList.GetNext(CurrentLyrDet));
			CurrentLyr = (CurrentLyr->FindNextLayer()); 
		}	
		else 	
			CurrentLyrDet = (LyrDetails*)(LyrDetList.GetNext(CurrentLyrDet)); 
	}	   
	// Sanity check 
	ENSURE (CurrentLyr == NULL, "CurrentLyr ptr should be NULL"); 
	return FALSE; // Nothing has changed 	
}

// Methods for the OpLayerChange

/********************************************************************************************

>	OpLayerChange::OpLayerChange() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpLayerChange constructor (Creates an undoable operation)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpLayerChange::OpLayerChange(): UndoableOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpLayerChange::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpLayerChange initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerChange::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_LAYERCHANGEOP),
								CC_RUNTIME_CLASS(OpLayerChange),
	 							OPTOKEN_LYRCHNG,
	 							OpLayerChange::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_LAYERCHANGEOP),
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpLayerChange::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpLayerChange operation
	Purpose:	For finding the OpLayerChange's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpLayerChange::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  
 	return(OpSt);   
}

/********************************************************************************************

>	void OpLayerChange::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the layer change operation 
				this operation reads the layer details list of the current documents layer 
				manager, and changes the document tree to reflect any changes which have 
				been made. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
// The bounding rectangle of the current layer must be invalidated in all the following cases. 

// If the visible status has changed 
// If the Quality level has changed 
// If the layer has been deleted 
// If the z-order of the layer has changed

			
void OpLayerChange::Do(OpDescriptor*)
{   
	ENSURE(FALSE,"This shouldn't have been called");
/*
	Spread* pSpread = Document::GetCurrent()->GetLayerMgr().GetCurrentSpread(); 
	// Find the first layer on the spread. All siblings of the first layer node should
	// be layer nodes
	Node* CurrentTreeLayer = pSpread->FindFirstLayer(); // skips over page nodes 

	ENSURE(CurrentTreeLayer->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer), 
			"A next sibling of a layer node is not a layer"); 


	// Get the first layer details record 
	LyrDetails* CurLyrDet = (LyrDetails*)
		(Document::GetCurrent()->GetLayerMgr()).LyrDetList.GetHead(); 
	
	BOOL InvalidateLayersRgn; // Flag used to decide if we should invalidate
							  // a layers region
	BOOL RemoveSelections; 	  // Flag used to indicate if we should remove all
							  // selections from the layer 

    // loop while there are more changes to be made 
	while (CurLyrDet != NULL) 
	{
		InvalidateLayersRgn = FALSE;
		RemoveSelections = FALSE; 

		// We can ignore all new layers which have been deleted 
		if (!((CurLyrDet->New) && (CurLyrDet->Deleted)))
		{
			// Is the layer a new layer ? 
			if (CurLyrDet->New)
			{
				// Attempt to create a new layer node 
				Layer* NewLyr; 
				ALLOC_WITH_FAIL(NewLyr, (new Layer()), this); 	      
				if (NewLyr == NULL)
				{
					goto EndOperation; // We were unable to create a new layer so 
									   // abort the operation 
				}
				// Set the new layer's status  
				NewLyr->SetLayerStatus(CurLyrDet->Status); 
		   
			    // Create a hide node action to hide the new node when we undo/redo
			    HideNodeAction* UndoHideNodeAction; 
			    // ** Change !!!    
				if (!HideNodeAction::Init(this,                    
							 			  &UndoActions,
										  NewLyr, 
										  TRUE, 
							  			  ( Action**)(&UndoHideNodeAction))      
							  			  != AC_FAIL)
				{
					delete NewLyr;     // We won't be needing this 
					goto EndOperation; 		
				}
				// All is well 
				if (CurrentTreeLayer != NULL)
				{
					// Add the new layer to the tree as a previous sibling of 
					// the CurrentTreeLayer 
					NewLyr->AttachNode(CurrentTreeLayer, PREV); 
				}
				else 
				{
					// Add the new layer as a last child of the spread 
					NewLyr->AttachNode(Document::GetCurrent()->
						GetLayerMgr().GetCurrentSpread(), LASTCHILD); 
				}
			}

			// Has the layer been deleted 
			else if (CurLyrDet->Deleted)
			{
				if ( CurLyrDet->Layer == CurrentTreeLayer )
				{
					// We are about to hide the CurrentTreeLayer so we need to find the 
					// next layer before we do this 
					CurrentTreeLayer = ((Layer*)CurrentTreeLayer)->FindNextLayer(); 
				}
				
				// If a layer has been deleted then we ignore all attribute changes 
				// which may have been made prior to the layer being deleted. 
				// Change 
				if (!DoHideNode(CurLyrDet->Layer, 
								TRUE				// Include subtree size 
								)) // Hide the node 
					goto EndOperation; 
				InvalidateLayersRgn = TRUE; // We will need to invalidate the hidden 
											// layers bounding region. 
				RemoveSelections = TRUE;    
			}
			else 
			{
				// Have the attributes of the layer changed 
				if ( !(CurLyrDet->Status == CurLyrDet->Layer->GetLayerStatus()) )	
				{
		
					// Determine if the attribute changes mean that the layer's 
					// bounding rectangle must be invalidated. 
					LayerStatus Old = CurLyrDet->Layer->GetLayerStatus(); 
					LayerStatus New = CurLyrDet->Status; 
					if (
						 (New.Flags.Visible != Old.Flags.Visible) || 
					   	 (New.LayerQualityLevel != Old.LayerQualityLevel)  
					   )  
					{
						InvalidateLayersRgn = TRUE; 
					} 

					// Determine if the attribute changes mean that we should remove
					// the selections on the layer 
					if ( 
						((New.Flags.Visible != Old.Flags.Visible) && 
						 (!New.Flags.Visible)
						) || 
						((New.Flags.Locked != Old.Flags.Locked) && 
						 (New.Flags.Locked)
						)
					   )
					{
						RemoveSelections = TRUE;  
					} 
			
					// Try to create an action to restore the attribute changes we 
					// are about to make 
				   	ChangeLyrAttribAction* UndoAttribChngAction;                          

					if ( ChangeLyrAttribAction::Init(this,                    
									 	      	  &UndoActions,
										      	  sizeof(InvalidateRegionAction),  
												  CurLyrDet->Layer->GetLayerStatus(),
												  CurLyrDet->Layer, 
								 			  	  ( Action**)(&UndoAttribChngAction))
								 			  	  != AC_FAIL) 
					{
						// Change the layer's attributes 
						CurLyrDet->Layer->SetLayerStatus(CurLyrDet->Status); 	
					}
					else 
						goto EndOperation; // We have failed 
			
				}
				// Has the current layers z-position changed ? 
				if (CurLyrDet->Layer != CurrentTreeLayer)
				{
					// I don't think the CurrentTreeLayer can ever be NULL in this situation !
					ENSURE(CurrentTreeLayer != NULL, "The current tree layer is NULL"); 
					// Move the node to its correct tree position 
					// *** Change 
					if(!DoMoveNode(CurLyrDet->Layer, 
						   		   CurrentTreeLayer, 
						    	   PREV 			  // The correct tree position 
								   ))
					{
						goto EndOperation; 
					}				  
				}
				else // The layer is in the correct position in the	tree
				{
					CurrentTreeLayer = ((Layer*)CurrentTreeLayer)->FindNextLayer(); 
				}
			}
			// Do we want to remove the layer's selections 
			if (RemoveSelections)
			{
				NodeRenderableInk::DeselectAllOnLayer(CurLyrDet->Layer); 
			}

			// Do we want to invalidate the bounding rectangle of the layer ? 
			if (InvalidateLayersRgn) 
			{
				DocRect Bounds = CurLyrDet->Layer->GetBoundingRect();
				// Don't bother if the bounding rectangle is empty 
				if (!Bounds.IsEmpty())
				{													  
				 	// Create an InvalidateRegionAction  
					if (!DoInvalidateNodeRegion(CurLyrDet->Layer, FALSE))
					{
						goto EndOperation; 
					}
				}
			}
        }
		// Process the next layer 
		CurLyrDet = (LyrDetails*)
			(Document::GetCurrent()->GetLayerMgr()).LyrDetList.GetNext(CurLyrDet); 
	}
 	EndOperation:
*/
 	End(); 
}

/********************************************************************************************

>	void OpLayerChange::Undo() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The overloaded Undo operation refreshes the layer details in the layer 
				manager   
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerChange::Undo() 
{
	BOOL Result = Operation::Undo(); 
	if (Result)
	{
		//Document::GetCurrent()->GetLayerMgr().RefreshLayerDetails(); 
	}
	return (Result); 
 
}		

/********************************************************************************************

>	void OpLayerChange::Redo()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	The overloaded Redo operation refreshes the layer details in the layer
				manager 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpLayerChange::Redo()
{
	BOOL Result = Operation::Redo(); 
	if (Result)
	{
		//Document::GetCurrent()->GetLayerMgr().RefreshLayerDetails(); 
	}
	return (Result); 
}

//------------------------------------------------------------------------------------------
//ChangeLyrAttribAction methods 

/********************************************************************************************

>	ChangeLyrAttribAction::ChangeLyrAttribAction()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ChangeLyrAttribAction constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
ChangeLyrAttribAction::ChangeLyrAttribAction()
{
}

/********************************************************************************************

>	ChangeLyrAttribAction::~ChangeLyrAttribAction() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	ChangeLyrAttribAction destructor 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



ChangeLyrAttribAction::~ChangeLyrAttribAction() 
{ 

}   


 
/********************************************************************************************

>	virtual ActionCode ChangeLyrAttribAction::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the ChangeLyrAttribAction 
	Errors:		-
	SeeAlso:	-
    
    
********************************************************************************************/
 
ActionCode ChangeLyrAttribAction::Execute()
{  
   ChangeLyrAttribAction ChngLyrAct; 
   ActionCode ActCode;  

     // Create an action to restore the changes we are about to make  
    if ((ActCode = ChangeLyrAttribAction::Init(pOperation,                    
								     		   pOppositeActLst,  
								     		   sizeof(ChangeLyrAttribAction),  
								     		   ChangeLayer->GetLayerStatus(),  // The current status
								     		   ChangeLayer,       
						 			 		   ( Action**)(&ChngLyrAct))) != AC_FAIL) 
	{ 
		// Remove the layers selection blobs if we are about to make the layer invisible,
		// or if we are about to lock the layer. 
		//if ((Status.Flags.Locked) || 
		//   (!(Status.Flags.Visible)))
		//{
		//	NodeRenderableInk::DeselectAllOnLayer( ChangeLayer );
		//}

		// Preserve the Active flag of the changing layer
		LayerStatus CurrentStatus = ChangeLayer->GetLayerStatus();
		//Status.Flags.Active = CurrentStatus.Flags.Active;

		// Change the layer's status  
		ChangeLayer->SetLayerStatus(Status); 
	}             
	return (ActCode);                
}     


/********************************************************************************************

>	static ActionCode ChangeLyrAttribAction::Init(Operation* const pOp, 
									   			  ActionList* pActionList, 	
						  			   			  UINT32 ActionSize, 
			 		  	    				 	  LayerStatus& Status, 
						   						  Layer* Layer,
						   			   			  Action** NewAction)	

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object
				
				ActionSize:  The size of the action in bytes. This should be the total  
							 size of the action (including any objects pointed to by the
							 action).  
				
				Status:		 The status which is restored when executed  
				Layer:		 The layer to change when we execute 
							 
							 
 	Outputs:    NewAction:   A pointer to the action if it could be allocated. 

	Returns:	AC_FAIL:     There was not enough room in the operation history for the 
							 action and the user did not wish to continue. Usually 
							 End() should be called in this situation. 
				         
				AC_NORECORD: There was not enough room in the operation history for
							 the action, but the user requested that he wished to 
							 continue without undo. 
				
				AC_OK      : The action was successfully initialised and added to the 
							 operation. 
				         
				           
	Purpose:	To check that there is sufficient room for the action in the operation
				history, and if there is, then to add the action to the operations 
				action list. 
				
				The function calls the Action::Init function passing the runtime class 
				of a ChangeLyrAttribAction.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/
   
ActionCode ChangeLyrAttribAction::Init(Operation* const pOp, 
										 ActionList* pActionList, 	
						  				 UINT32 ActionSize,  
				    				 	 LayerStatus& Status,  // The layer status which 
				    				 	 					   // should be restored 
					 				    					   // when we execute the action. 
						   				 Layer* Layer, 		   // The layer to change when we execute 
				
						  			 	 Action** NewAction)
{    

	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 ActionSize,
					 CC_RUNTIME_CLASS(ChangeLyrAttribAction), 
					 NewAction));
					     
	if (*NewAction != NULL) // The action has been allocated 
	{
		((ChangeLyrAttribAction*)(*NewAction))->ChangeLayer = Layer;
		((ChangeLyrAttribAction*)(*NewAction))->Status = Status; 
	}
	return (Ac); 
}
    




/********************************************************************************************

>	BOOL OpDelBar::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/00
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpDelBar initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpMoveToLayer::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_MOVE_SEL_TO_ACTIVE_LAYER),
								CC_RUNTIME_CLASS(OpMoveToLayer),
 								OPTOKEN_MOVE_SEL_TO_ACTIVE_LAYER,
 								OpMoveToLayer::GetState,
 								0,					// help ID 
 								0,					// bubble help
								0,					// resource ID
								0,					// control ID
								SYSTEMBAR_ILLEGAL,	// Bar ID
								TRUE, 				// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE) ));
}



/********************************************************************************************

>	OpState	OpMoveToLayer::GetState(String_256* UIDescription, OpDescriptor *Bob)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/5/00
	Purpose:	For finding the operations state.  

********************************************************************************************/
OpState	OpMoveToLayer::GetState(String_256* UIDescription, OpDescriptor *Bob)
{
	OpState OpSt;
	return(OpSt);   
}


/********************************************************************************************

>	void OpMoveToLayer::Do(OpDescriptor*)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	3/5/00
	Purpose:	This takes the selection and moves it to the active layer. This is a more
				powerful operation than the move forward/backward a layer

********************************************************************************************/
void OpMoveToLayer::Do(OpDescriptor*)
{
	if (DoStartSelOp(FALSE,TRUE))
	{
		// get the selection
		Range Sel(*(GetApplication()->FindSelection()));

		// set the range flags so it includes shadow and bevel manager nodes
		RangeControl rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		Sel.Range::SetRangeControl(rg);

		// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
		ObjChangeFlags cFlags;
		cFlags.MoveNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

		// add items directly after the layer node as its first child
		Node * pTail = (Node *) (Document::GetCurrent()->GetInsertionPosition());
		AttachNodeDirection TailAttachDirection = PREV;
		Spread* pSpread = Document::GetSelectedSpread();

		if (!pTail)
		{
			if (pSpread)
			{
				pTail = pSpread->FindActiveLayer();
				AttachNodeDirection TailAttachDirection = LASTCHILD;
			}
			else
			{
				FailAndExecute();
				End();
				return; // nowhere to put the nodes
			}
		}


		// Mark nodes that will allow this to happen, and error if no nodes will let it happen
		if (!Sel.AllowOp(&ObjChange))
		{
			FailAndExecute();
			End();
			return; // op not allowed
		}

		// get a list from which to move the nodes (fixes job #10781 - the re-selection of 
		// moved nodes caused an infinite loop)

		List* pNodeList = Sel.MakeListOfNodes(FALSE);
		NodeListItem* CurItem = (NodeListItem*)(pNodeList->GetHead());
		while (CurItem)
		{
			Node* pNode = CurItem->pNode;

			// Make sure the current owner Layer is told about the changes
			// and given the chance to release any cached info it may be
			// holding about the selected object
			// (I know that the original position coincides with the destination position
			//  but this is hte simplest way to get the original parent layer uncached
			//  and to get a record in the undo history so that the layer cacheing will
			//  be dealt with properly during gundo/redo)
			DoInvalidateNodeRegion((NodeRenderableBounded*) pNode, TRUE, FALSE);

			// localise attribs for this node
			DoLocaliseForAttrChange((NodeRenderableInk*) pNode, (AttrTypeSet *)NULL, (ObjectSet*) NULL);
			DoMoveNode(pNode, pTail, TailAttachDirection);
			// factor out common attribs
			if (pNode->IsCompound())
				DoFactorOutCommonChildAttributes((NodeRenderableInk*) pNode);
			else
				DoFactorOutAfterAttrChange((NodeRenderableInk*) pNode, (AttrTypeSet *)NULL);

			// make the nodes region be redrawn
			DoInvalidateNodeRegion((NodeRenderableBounded*) pNode, TRUE, FALSE);

			CurItem = (NodeListItem*)(pNodeList->GetNext(CurItem));
		}
		// delete the the list objects 
		pNodeList->DeleteAll();
		delete pNodeList;

		// the selection will have changed - after all we just deleted it
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));
		GetApplication()->UpdateSelection();

		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);
		// end the op
		End();

		// update the bars
		DialogBarOp::SetSystemStateChanged();
		DialogBarOp::UpdateStateOfAllBars(); 
	}
	else
	{
		// give up and go home
		FailAndExecute();
		End();
	}
}
