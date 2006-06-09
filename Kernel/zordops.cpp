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

//Revision 1.13  1994/04/29  12:41:05  Simon
//Changed all operations so that they correctly work across layers
//
//Revision 1.12  1994/04/14  18:13:00  Tim
//Major change to Node class hierarchy.
//
//Revision 1.11  1994/03/25  17:59:32  Simon
//New messaging system changes + Moved  certain Op flags into opDescriptors
//
//Revision 1.10  1994/03/14  10:24:00  Simon
//Changed all ops so they use the new high level Do functions 
//
//Revision 1.9  1994/02/25  15:34:10  JustinF
//Added bubble help resource ID's to all uses of OpDescriptor
//
//Revision 1.8  1994/01/26  17:01:59  Rik
//Fixed Blob and bounding rectangle problems
//
//Revision 1.7  1993/12/23  10:49:39  Tim
//New Spread based coordinate system
//
//Revision 1.6  1993/11/25  12:17:20  Simon
//Added operation disabled reason strings to all operations 
//
//Revision 1.5  1993/11/19  17:53:58  Simon
//No change 
//
//Revision 1.4  1993/10/20  16:14:25  Simon
//Made all Z operations undoable
//
//Revision 1.3  1993/10/11  17:34:33  Mario
//Changed prototype of GetState() function to use String_256*
//
//Revision 1.2  1993/10/11  12:18:21  Simon
//Added Bring forwards and Move backwards operations
//
//Revision 1.1  1993/10/01  18:17:16  Simon
//Initial revision
//

#include "camtypes.h" 
#include "zordops.h"
//#include "simon.h" 
//#include "errors.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h"    - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
//#include "markn.h"
#include "bubbleid.h"

//#include "prevwres.h"	// _R(IDS_COMBINELAYERSTOFRAMELAYER)
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "layergal.h"	// LayerStateAction
#include "sprdmsg.h"	// SpreadMsg::SpreadReason::LAYERCHANGES
#include "layermsg.h"	// UPDATE_ACTIVE_LAYER
//#include "bmpsdlgr.h"
//#include "opbevel.h"
#include "slicehelper.h"
#include "ophist.h"
      
DECLARE_SOURCE("$Revision$");  

CC_IMPLEMENT_DYNCREATE(OpBringToFront, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpPutToBack, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpMoveForwards, SelOperation)  
CC_IMPLEMENT_DYNCREATE(OpMoveBackwards, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpMoveToLyrInFront, SelOperation)  
CC_IMPLEMENT_DYNCREATE(OpMoveToLyrBehind, SelOperation)
CC_IMPLEMENT_DYNAMIC(FrameInFrontOpDescriptor, OpDescriptor)
CC_IMPLEMENT_DYNAMIC(FrameBehindOpDescriptor, OpDescriptor)

//CC_IMPLEMENT_DYNCREATE(OpCombineLayersToFrameLayer, SelOperation)

#define new CAM_DEBUG_NEW    

// ------------------------------------------------------------------------------------------
// WEBSTER Z-ORDER OPS (markn 2/2/97)
//
// As Webster doesn't have a concept of layers, the z-order operations have been rewritten so
// that they function across layers.  I.e. when an object has its Z position changed, it is not
// localised to the selected object's layer.  It will move across layers when necessary.
//
// The ops "Move to layer infront" and "Move to layer behind" have been removed as they are
// obsolete in WEBSTER
//
// Neville 23/6/97
// The ops "Move to layer infront" and "Move to layer behind" are now back in but perform
// across frame layers.

#ifdef WEBSTER
// ------------------------------------------------------------------------------------------
// Help funcs - WEBSTER - markn 2/2/97

// static Node* FindLastObject(Layer* pLayer)
//
// Finds the last object on a layer, but only if the layer is one that can be edited.
// Returns NULL if conditions aren't right
static Node* FindLastObject(Layer* pLayer)
{
	if (pLayer == NULL)
		return NULL;

	if (pLayer->IsLocked() || pLayer->IsGuide())
		return NULL;

	return pLayer->FindLastChild(CC_RUNTIME_CLASS(NodeRenderableInk));
}

// static Node* FindFirstObject(Layer* pLayer)
//
// Finds the first object on a layer, but only if the layer is one that can be edited.
// Returns NULL if conditions aren't right
static Node* FindFirstObject(Layer* pLayer)
{
	if (pLayer == NULL)
		return NULL;

	if (pLayer->IsLocked() || pLayer->IsGuide())
		return NULL;

	return pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
}
#endif // WEBSTER

// ------------------------------------------------------------------------------------------
// OpBringToFront methods
            
/********************************************************************************************

>	OpBringToFront::OpBringToFront() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpBringToFront constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpBringToFront::OpBringToFront(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpBringToFront::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpBringToFront initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpBringToFront::Init()
{
	return (RegisterOpDescriptor(0,
 								_R(IDS_BRINGTOFRONTOP),
								CC_RUNTIME_CLASS(OpBringToFront),
 								OPTOKEN_BRINGTOFRONT,
 								OpBringToFront::GetState,
 								0,	/* help ID */
 								_R(IDBBL_BRINGTOFRONTOP),
 								0,	
								0,
								SYSTEMBAR_ILLEGAL,			// For now !
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								)); 

}               

/********************************************************************************************

>	OpState	OpBringToFront::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn for WEBSTER - 2/2/97)
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpBringToFront operation
	Purpose:	For finding OpBringToFront's state. 

  				See note at the top of file about WEBSTER z-order ops
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpBringToFront::GetState(String_256*  UIDescription, OpDescriptor*)
{
#ifdef WEBSTER
	OpState OpSt;
	String_256 DisableReason; 

	// DMc changed for select inside on bevels etc
	
	SelRange Sel(*( GetApplication()->FindSelection());
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	Node* pSelNode = Sel.FindFirst();
	while (pSelNode != NULL)
	{
		Node* pNode = pSelNode;
		while (pNode != NULL)
		{
			Node* pOldNode = pNode;		// Remember this node

			if (pNode != NULL)
				pNode = pNode->FindNext();	// Get the node after this node

			// If the next node	is an unselected object, then we can move this object to the front
			if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
				goto End;

			if (pNode == NULL)
			{
				// Find the parent layer
				Layer* pLayer = (Layer*)pOldNode->FindParent(CC_RUNTIME_CLASS(Layer));

				// Find the last object on the first editable layer we can find that follows this layer
				do
				{
					pLayer = pLayer->FindNextLayer(TRUE);
					pNode = FindFirstObject(pLayer);

				} while (pLayer != NULL && pNode == NULL);

				// If the next node	is an unselected object, then we can move this object to the front
				if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
					goto End;
			}
		} 

		// Try the next selected node.
		pSelNode = Sel.FindNext(pSelNode);
	}

	OpSt.Greyed = TRUE;
 
	// Determine which reason string to return
	if (Sel.Count() > 1)
	{
		DisableReason = String_256(_R(IDS_ALREADY_AT_FRONTP)); // A plural description
	}
	else
	{
		DisableReason = String_256(_R(IDS_ALREADY_AT_FRONTS));
	}
	*UIDescription = DisableReason;	                                      

	End:												 
	return(OpSt);

#else
// WEBSTER - markn 2/2/97
// Original code

	OpState OpSt;
	String_256 DisableReason;

	// DMc changed for select inside on bevels etc
	SelRange Sel (*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// If all selected objects are already at the front of their layers then the operation should be
	// disabled. 
	Node* Current = Sel.FindFirst(); 
	Node* LastCurrent;
	MODE PresentMode = FRAME; // In case Current is NULL

	// Determine the correct document mode.
	if(Current)
	{
		Layer* CurrentLayer = (Layer*) (Current->FindParent(CC_RUNTIME_CLASS(Layer)));
		ERROR2IF(CurrentLayer == NULL, OpSt, "Cannot find layer of first selected object"); 

		// The mode of this document - Frame/Layer.
		PresentMode = (MODE)CurrentLayer->IsFrame();
	}

	BOOL bSelected = FALSE;
	
	while(Current != NULL) // Loop for all layers
	{	
		// If an unselected object is found on this layer with a higher z-order position than
		// Current then the operation can be performed.
		do
		{
			// DMc revision 4/6/99
			if (Current->ShouldITransformWithChildren())
			{
				// check to see if any of its children are selected
				RangeControl rg(TRUE, FALSE);
				Range rng(Current, Current, rg);

				if (rng.IsEmpty())
				{
					bSelected = FALSE;
				}
				else
				{
					bSelected = TRUE;
				}
			}
			else
			{
				bSelected = Current->IsSelected();
			}
			
			if ((Current->IsAnObject()) && (!(bSelected)))
			{
				goto End; // The operation can be performed
			}
			LastCurrent = Current;
			Current = Current->FindNext(); 
		} while (Current != NULL);
		// Obtain the next selected object (on the next layer)
		Current = Sel.FindNext(LastCurrent); 
	}
	// All selected objects are already at the front of their layers
	OpSt.Greyed = TRUE; 

	if(PresentMode == FRAME)
	{
		// Determine which reason string to return
		if (Sel.Count() > 1)
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_FRONTP_FRAME)); // A plural description
		}
		else
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_FRONTS_FRAME));
		}
	}
	else if (PresentMode == LAYER)
	{
		// Determine which reason string to return
		if (Sel.Count() > 1)
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_FRONTP)); // A plural description
		}
		else
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_FRONTS));
		}
	}
	else
		ERROR3("OpBringToFront::GetState - Bad mode!");

   	*UIDescription = DisableReason;	                                      
	End:												 
	return(OpSt);   

#endif // WEBSTER
}

/********************************************************************************************

>	void OpBringToFront::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn for WEBSTER - 2/2/97)
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BringToFront operation, this moves all selected objects to the 
				front of their layers.
				
				See note at the top of file about WEBSTER z-order ops
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpBringToFront::Do(OpDescriptor*)		   
{  
#ifdef WEBSTER
	OpState OpSt;
	String_256 DisableReason; 

	SelRange Sel (*( GetApplication()->FindSelection());
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	Node* pSelNode = Sel.FindLast();

	// If there are selected objects, record undo stuff about the selection
	if (pSelNode != NULL)
	{
		// Try to record the selection state , don't render the blobs though
		// the invalidate will take care of this. 
		if (!DoStartSelOp(FALSE,FALSE))
			goto EndOperation;  

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*Sel, TRUE))
			goto EndOperation; 
	}

	while (pSelNode != NULL)
	{
		Node* pContextNode = NULL;

		Node* pNode = pSelNode;
		while (pNode != NULL)
		{
			Node* pOldNode = pNode;		// Remember this node

			if (pNode != NULL)
				pNode = pNode->FindNext();	// Get the node after this node

			// If the next node	is an unselected object, then we can move this object to the front
			if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
				pContextNode = pNode;

			if (pNode == NULL)
			{
				// Find the parent layer
				Layer* pLayer = (Layer*)pOldNode->FindParent(CC_RUNTIME_CLASS(Layer));

				// Find the last object on the first editable layer we can find that follows this layer
				do
				{
					pLayer = pLayer->FindNextLayer(TRUE);
					pNode = FindFirstObject(pLayer);

				} while (pLayer != NULL && pNode == NULL);

				// If the next node	is an unselected object, then we can move this object to the front
				if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
					pContextNode = pNode;
			}
		} 

		pNode = pSelNode;
		// Try the next selected node.
		pSelNode = Sel.FindPrev(pSelNode);

		if (pContextNode != NULL)
		{
			if (!DoMoveNode(pNode, pContextNode, NEXT))
				goto EndOperation;

			pNode->SetSelected(TRUE);
		}
	}

	EndOperation:	 
	End(); // End of operation

#else
// WEBSTER - markn 2/2/97
// Original code

	// Obtain the current selections
	// Copy SelRange into a range so that it cannot change during the operation
	// Note: the cache is freshened by this 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);


	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
 
	// The first selected node should not ever be NULL. The GetState fn should have seen
	// to this.
	ENSURE(FirstSelectedNode != NULL, 
		"The OpBringToFront's GetState fn has not done a very good job"); 
	
	// lets be defensive about it anyway
	if (FirstSelectedNode != NULL) 
	{
		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state 
		{
			goto EndOperation;  
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}
		
		Spread* pSpread; 
		Node* Tail;
		Node* NextSelNode;  

		// We will need to scan the layers one at a time so create a Range control to
		// acheive this. 
//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE };  // Selected + don't cross layers  
		Range LayerRange; 
	   	
	   	// Find the spread that the selection lies on
		pSpread = FirstSelectedNode->FindParentSpread();
		
		Node* CurrentNode = FirstSelectedNode;     
        Node* LastNodeOnLayer;

		// Loop for all layers
		while(CurrentNode != NULL)
		{

			// Create a range of selected nodes on this layer
			Range temp(CurrentNode, 
						 	   NULL, 
						       RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );
			LayerRange = temp;
//			LayerRange = Range(CurrentNode, 
//						 	   NULL,
//						       RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );

			FirstSelectedNode = CurrentNode; 
		    
			// Find the last ink object on this layer
			LastNodeOnLayer = CurrentNode->FindParent(CC_RUNTIME_CLASS(Layer))->FindLastChild();
			if (LastNodeOnLayer->IsAnObject())
			{
				Tail = LastNodeOnLayer;
			}
			else
			{
				Tail = LastNodeOnLayer->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));
			}
	            
			// If the first node selected is already at the front then we don't really want
			// to move it. In this situation we know that there can be no other selected nodes 
			// so we need not do anything                    
			if (FirstSelectedNode != Tail)
			{    
				do 
				{   
					ENSURE (CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), 
					"Selected node not a NodeRenderableBounded");
          		
					// Find the next selected node on the current layer
	        		NextSelNode = LayerRange.FindNext(CurrentNode); 
			
					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					if (!DoMoveNode(CurrentNode, Tail, NEXT)) 
					{
						goto EndOperation;
					}

					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					CurrentNode->SetSelected(TRUE);
					 
					Tail = CurrentNode; 
					CurrentNode = NextSelNode;     
			
	    	    	// Stop when we hit the first node which was moved. 
		        	// or when there are no more selected nodes (This can only happen if there is only
		        	// one node selected). 
				} while ((CurrentNode != FirstSelectedNode) && (CurrentNode != NULL)); 
		 
			} 
			// Get the first selected node of the next layer
			CurrentNode = Sel.FindNext(Tail); 
			ENSURE(LayerRange.FindNext(Tail) == NULL, 
				"Current Node is not the last selected node on the layer"); 
		} 
		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}
	}

	EndOperation:
	GetApplication()->UpdateSelection();
	
	End(); // End of operation

#endif // WEBSTER
} 

// ------------------------------------------------------------------------------------------
// OpPutToBack methods
            
/********************************************************************************************

>	OpPutToBack::OpPutToBack() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpPutToBack constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpPutToBack::OpPutToBack(): SelOperation()								
{                              
}

/********************************************************************************************

>	OpState	OpPutToBack::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn for WEBSTER - 2/2/97)
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPutToBack operation
	Purpose:	For finding OpPutToBack's state. 

  				See note at the top of file about WEBSTER z-order ops
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpPutToBack::GetState(String_256*  UIDescription, OpDescriptor*)
{
#ifdef WEBSTER

	OpState OpSt;
	String_256 DisableReason; 

	SelRange Sel(*( GetApplication()->FindSelection()));

	Node* pSelNode = Sel.FindFirst();
	while (pSelNode != NULL)
	{
		Node* pNode = pSelNode;
		while (pNode != NULL)
		{
			Node* pOldNode = pNode;		// Remember this node

			if (pNode != NULL)
				pNode = pNode->FindPrevious();	// Get the node before this node

			// If the previous node	is an unselected object, then we can move this object back
			if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
				goto End;

			if (pNode == NULL)
			{
				// Find the parent layer
				Layer* pLayer = (Layer*)pOldNode->FindParent(CC_RUNTIME_CLASS(Layer));

				// Find the last object on the first editable layer we can find that preceeds this layer
				do
				{
					pLayer = pLayer->FindPrevLayer(TRUE);
					pNode = FindLastObject(pLayer);

				} while (pLayer != NULL && pNode == NULL);

				// If the previous node	is an unselected object, then we can move this object back
				if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
					goto End;
			}
		} 

		// Try the next selected node.
		pSelNode = Sel.FindNext(pSelNode);
	}

	OpSt.Greyed = TRUE;
 
	// Determine which reason string to return
	if (Sel.Count() > 1)
	{
		DisableReason = String_256(_R(IDS_ALREADY_AT_BACKP)); // A plural description
	}
	else
	{
		DisableReason = String_256(_R(IDS_ALREADY_AT_BACKS));
	}
	*UIDescription = DisableReason;	                                      

	End:												 
	return(OpSt);

#else

// WEBSTER - markn 30/1/97
// Original code

	OpState OpSt;
	String_256 DisableReason; 

	// DMc
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 

	// If all selected objects are already at the back then the operation should be
	// disabled
	Node* Current = FirstSelectedNode; 
	MODE PresentMode = LAYER;

	Spread* pSpread = (Spread*) (Current->FindParent (CC_RUNTIME_CLASS (Spread)));
	
	// Determine the correct document mode.
	if (Current && pSpread)
	{
		Layer* CurrentLayer = (Layer*) (Current->FindParent(CC_RUNTIME_CLASS(Layer)));
		ERROR2IF(CurrentLayer == NULL, OpSt, "Cannot find layer of first selected object"); 

		// The mode of this document - Frame/Layer.
		PresentMode = (MODE)CurrentLayer->IsFrame();
	}

	// now scan each layer ....

	BOOL allowed = FALSE;

	Layer* pNode = (Layer*) SliceHelper::FindNextOfClass(pSpread, pSpread, CC_RUNTIME_CLASS (Layer));

	// If an unselected object is found on this layer with a lower z-order position than
	// LastSelCurrent then the operation can be performed.

	while (pNode)
	{
		Node* lastSelected = NULL;
		BOOL currentLayerAllow = FALSE;				// so that we can detect an individual layer NOT allowing
													// the op, where previous have
		
		if (Current->FindParent (CC_RUNTIME_CLASS (Layer)) == Sel.FindLast ()->FindParent (CC_RUNTIME_CLASS (Layer)))
		{
			lastSelected = Sel.FindLast ();
		}
		else
		{
			// scan for the last selected node on this layer ....

			Node* Scan = pNode->FindLastChild ();

			while (Scan != NULL)
			{
				if (Scan->IsAnObject() && Scan->IsSelected())
				{
					lastSelected = Scan;
					break; 

				}
				Scan = Scan->FindPrevious();
			}
		}

		// -----------------------------------------------------------------------------
		// If an unselected object is found on this layer with a lower z-order position than
		// LastSelCurrent then the operation can be performed.

		Node* Current = lastSelected;

		if (Current)
		{
			do
			{
				// DMc revision 4/6/99
				BOOL bSelected = FALSE;
				if (Current->ShouldITransformWithChildren())
				{
					// check to see if any of its children are selected
					RangeControl rg(TRUE, FALSE);
					Range rng(Current, Current, rg);

					if (rng.IsEmpty())
					{
						bSelected = FALSE;
					}
					else
					{
						bSelected = TRUE;
					}
				}
				else
				{
					bSelected = Current->IsSelected();
				}
					
				if ((Current->IsAnObject()) && (!(bSelected)))
				{
					currentLayerAllow = TRUE;	// weve set this layer
					allowed = TRUE;				// The operation can be performed
				}
					
				Current = Current->FindPrevious(); 
			} while (Current != NULL);
		}
		else
		{
			currentLayerAllow = TRUE;		// nothing on this layer, we should still allow the op
		}

		// wo there, if one layer returned TRUE, and this one returned FALSE, then we MUST disable the op!

		if (allowed && !currentLayerAllow)
		{
			goto Disable;
		}
	
		pNode = (Layer*) SliceHelper::FindNextOfClass(pNode, pSpread, CC_RUNTIME_CLASS (Layer));
	}

	if (allowed)
	{
		goto End;
	}

	Disable:

	OpSt.Greyed = TRUE;

	if(PresentMode == LAYER)
	{
		// Determine which reason string to return
		if (Sel.Count() > 1)
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_BACKP)); // A plural description
		}
		else
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_BACKS));
		}
	}
	else if(PresentMode == FRAME)
	{
		// Determine which reason string to return
		if (Sel.Count() > 1)
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_BACKP_FRAME)); // A plural description
		}
		else
		{
			DisableReason = String_256(_R(IDS_ALREADY_AT_BACKS_FRAME));
		}
	}
	else
		ERROR3("OpBringToFront::GetState - Bad mode!");

	*UIDescription = DisableReason;	                                      

	End:												 
	return(OpSt);   

#endif // WEBSTER
}
 /********************************************************************************************

>	BOOL OpPutToBack::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPutToBack initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/


BOOL OpPutToBack::Init()
{
	return (RegisterOpDescriptor(0,
								_R(IDS_PUTTOBACKOP),
								CC_RUNTIME_CLASS(OpPutToBack),
								OPTOKEN_PUTTOBACK,
								OpPutToBack::GetState,
								0,	/* help id */
								_R(IDBBL_PUTTOBACKOP),
								0,	/* bitmap ID */
								0,
								SYSTEMBAR_ILLEGAL,			// For now !
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								)); 
}               



/********************************************************************************************

>	void OpPutToBack::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn for WEBSTER - 2/2/97)
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the PutToBack operation 

  				See note at the top of file about WEBSTER z-order ops
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpPutToBack::Do(OpDescriptor*)		   
{   
#ifdef WEBSTER

	SelRange* Sel = GetApplication()->FindSelection();

	Node* pSelNode = Sel->FindFirst();

	// If there are selected objects, record undo stuff about the selection
	if (pSelNode != NULL)
	{
		// Try to record the selection state , don't render the blobs though
		// the invalidate will take care of this. 
		if (!DoStartSelOp(FALSE,FALSE))
			goto EndOperation;  

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*Sel, TRUE))
			goto EndOperation; 
	}

	while (pSelNode != NULL)
	{
		Node* pContextNode = NULL;

		Node* pNode = pSelNode;
		while (pNode != NULL)
		{
			Node* pOldNode = pNode;		// Remember this node

			if (pNode != NULL)
				pNode = pNode->FindPrevious();	// Get the node before this node

			// If the previous node	is an unselected object, then we can move this object back
			if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
				pContextNode = pNode;

			if (pNode == NULL)
			{
				// Find the parent layer
				Layer* pLayer = (Layer*)pOldNode->FindParent(CC_RUNTIME_CLASS(Layer));

				// Find the last object on the first editable layer we can find that preceeds this layer
				do
				{
					pLayer = pLayer->FindPrevLayer(TRUE);
					pNode = FindLastObject(pLayer);

				} while (pLayer != NULL && pNode == NULL);

				// If the previous node	is an unselected object, then we can move this object back
				if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
					pContextNode = pNode;
			}
		} 

		pNode = pSelNode;
		// Try the next selected node.
		pSelNode = Sel->FindNext(pSelNode);

		if (pContextNode != NULL)
		{
			if (!DoMoveNode(pNode, pContextNode, PREV))
				goto EndOperation;

			pNode->SetSelected(TRUE);
		}
	}

	EndOperation:	 
	End(); // End of operation

#else

// WEBSTER - markn 2/2/97
// Original code

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
 
	// The first selected node should not ever be NULL
	ENSURE(FirstSelectedNode != NULL, 
		"The OpPutToBack's GetState fn has not done a very good job"); 

	if (FirstSelectedNode != NULL)  
	{   
		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't
										 // render the blobs though, the invalidate
										 // will take care of this. 
		{
			goto EndOperation;  
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}

		// Find the spread that the selection lies on
//		Spread* pSpread = FirstSelectedNode->FindParentSpread();

//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE }; 
		Range LayerRange; 

		Node* FirstSelectedNextLayer; 

		// Loop for all layers
		while (FirstSelectedNode != NULL)
		{
			Range temp(FirstSelectedNode,
							   NULL, 
							   RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) ); // Selected + don't cross layers
			LayerRange = temp;
//			LayerRange = Range(FirstSelectedNode, 
//							   NULL, 
//							   RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) ); // Selected + don't cross layers  

	        // Find the last selected node on the current layer
	        Node* LastSelectedNode=NULL;  
	        Node* Scout = FirstSelectedNode; // First selected node on layer 

	        // Loop until we find the last selected node
	        while (Scout != NULL)
	        {
	        	LastSelectedNode = Scout; 
	        	Scout = LayerRange.FindNext(Scout); 	
	        }

	        Node* CurrentNode = LastSelectedNode; 

			// Find the first selected node of the next layer
			FirstSelectedNextLayer = Sel.FindNext(CurrentNode);

			// Find the head of the selection list
			Node* Head = FirstSelectedNode->FindParent()->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

			// If the last node selected is already at the back then we don't really want
			// to move it. In this situation we know that there can be no other selected nodes
			// so we need not do anything.
			if (LastSelectedNode != Head)
			{
				Node* PrevSelNode;
				do
				{
					ENSURE (CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)),
							"Selected node not a NodeRenderableBounded");

					// (ChrisG 15/2/2001) Since we're using a SelRange to determine what's selected,
					//	we really should use this to find the previous selected node, rather than
					//	relying on "Selected" flags, as controllers, etc... are never selected, but are
					//	included in selection ranges.

	        		// Find the previous selected node before CurrentNode moves. 
					PrevSelNode = LayerRange.FindPrev (CurrentNode, FALSE);

					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					if (!DoMoveNode(CurrentNode, Head, PREV))
					{
						goto EndOperation;
	        		}

					// (ChrisG 15/2/2001) This isn't really all that good, as it will select objects
					//	that weren't previously selected, as promote to parent will select the parent
					//	object, and SetSelected will then set all the children to selected. Which is
					//	fine, unless not all of the children were previously selected.
					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					CurrentNode->SetSelected(TRUE);

					Head = CurrentNode;
					CurrentNode = PrevSelNode;
				
		        // Stop when we hit the first node which was moved.
		        // or when there are no more selected nodes (This can only happen if there is only
		        // one node selected).
				} while ((CurrentNode != LastSelectedNode) && (CurrentNode != NULL));
        	
	        	// Invalidate the region which bounds all selected nodes
				if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE))
				{
					goto EndOperation;
				}
			}
				 
			// (ChrisG - 15/02/2001) Moved outside of if "is object at back of layer already", as this
			//	could cause an infinite loop if there were selected objects on two (or more) layers, 
			//	but the objects on the first layer were already at the back.

			// Now process next layer
			FirstSelectedNode = FirstSelectedNextLayer;  
		}
	}
	EndOperation:	 
	GetApplication()->UpdateSelection();
	
	End(); // End of operation

#endif // WEBSTER
}  

// ------------------------------------------------------------------------------------------
// OpMoveForwards methods
            
/********************************************************************************************

>	OpMoveForwards::OpMoveForwards() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpMoveForwards constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpMoveForwards::OpMoveForwards(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpMoveForwards::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMoveForwards initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpMoveForwards::Init()
{
	return(RegisterOpDescriptor(
								0,
								_R(IDS_MOVEFORWARDSOP),
								CC_RUNTIME_CLASS(OpMoveForwards),
								OPTOKEN_MOVEFORWARDS,
								OpBringToFront::GetState,	// MoveForwards uses the getstate
															// fn of OpBringToFront
								0,	/* help ID */
								_R(IDBBL_MOVEFORWARDSOP),
								0,	/* bitmap ID */
								0,
								SYSTEMBAR_ILLEGAL,			// For now !
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								)); 

}               

/********************************************************************************************

>	void OpMoveForwards::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn for WEBSTER - 2/2/97)
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the MoveForwards operation 

  				See note at the top of file about WEBSTER z-order ops
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpMoveForwards::Do(OpDescriptor*)		   
{     
#ifdef WEBSTER

	OpState OpSt;
	String_256 DisableReason; 

	SelRange* Sel = GetApplication()->FindSelection();

	Node* pSelNode = Sel->FindLast();

	// If there are selected objects, record undo stuff about the selection
	if (pSelNode != NULL)
	{
		// Try to record the selection state , don't render the blobs though
		// the invalidate will take care of this. 
		if (!DoStartSelOp(FALSE,FALSE))
			goto EndOperation;  

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*Sel, TRUE))
			goto EndOperation; 
	}

	while (pSelNode != NULL)
	{
		Node* pContextNode = NULL;
		BOOL SelNodeFound = FALSE;

		Node* pNode = pSelNode;
		while (pNode != NULL && pContextNode == NULL && !SelNodeFound)
		{
			Node* pOldNode = pNode;		// Remember this node

			if (pNode != NULL)
				pNode = pNode->FindNext();	// Get the node after this node

			// If the next node	is an unselected object, then we can move this object to the front
			if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
				pContextNode = pNode;

			SelNodeFound = (pNode != NULL && pNode->IsAnObject() && pNode->IsSelected());

			if (pNode == NULL)
			{
				// Find the parent layer
				Layer* pLayer = (Layer*)pOldNode->FindParent(CC_RUNTIME_CLASS(Layer));

				// Find the last object on the first editable layer we can find that follows this layer
				do
				{
					pLayer = pLayer->FindNextLayer(TRUE);
					pNode = FindFirstObject(pLayer);

				} while (pLayer != NULL && pNode == NULL);

				// If the next node	is an unselected object, then we can move this object to the front
				if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
					pContextNode = pNode;
			}
		} 

		pNode = pSelNode;
		// Try the next selected node.
		pSelNode = Sel->FindPrev(pSelNode);

		if (pContextNode != NULL && !SelNodeFound)
		{
			if (!DoMoveNode(pNode, pContextNode, NEXT))
				goto EndOperation;

			pNode->SetSelected(TRUE);
		}
	}

	EndOperation:	 
	End(); // End of operation

#else

// WEBSTER - markn 2/2/97
// Original code

	// Obtain the current selections
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);


	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
 
	// The first selected node should not ever be NULL
	ENSURE(FirstSelectedNode != NULL, 
		"The OpPutToBack's GetState fn has not done a very good job"); 
	
	// Make sure we are dealing with a NodeRenderable
	ENSURE (FirstSelectedNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), 
			"Selected node not a NodeRenderableBounded");  
	 
	// We can only do something if there are any selected nodes  
	if (FirstSelectedNode != NULL)  
	{                                                           
		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't
										 // render the blobs though, the invalidate
										 // will take care of this. 
		{
			goto EndOperation;  
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}

		// Find the spread that the selection lies in
//		Spread* pSpread = FirstSelectedNode->FindParentSpread();

//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE }; // Selected + don't cross layers
		Range LayerRange;

		Node* NextLayerFirstSelectedNode; 
		
		// Loop for all layers
		// FirstSelectedNode points to the first node in the range on the layer we are currently
		// processing
		while (FirstSelectedNode != NULL)
		{
			// Create a range of selected nodes on this layer
			Range temp(FirstSelectedNode, 
							   NULL, 
							   RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );
			LayerRange = temp;
//			LayerRange = Range(FirstSelectedNode, 
//							   NULL,
//							   RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );

	        // Find the last selected node on the layer
	        Node* TempSelNode = FirstSelectedNode; 
	        Node* LastSelectedNode = NULL; 
	        while (TempSelNode != NULL)
	        {
	        	LastSelectedNode = TempSelNode;                          // At end of loop will hold 
	        															 // the last node selected. 
	        	TempSelNode = LayerRange.FindNext(TempSelNode); 	
	        }
         
			// Obtain the first node in the range of the next layer,
			//  store it away until it's required
			NextLayerFirstSelectedNode = Sel.FindNext(LastSelectedNode); 

	        // We are going to start at the tail of the list i.e. at the node with the highest 
	        // Z-order.   
	        Node* CurrentNode = LastSelectedNode;             
        
	        Node* PrevSelNode = NULL;  // We never ever move a node further forward than the previously 
	        						   // processed node (This is to maintain the relative Z-Order of the 
	        						   // nodes).    
        						   
	        Node* NextSelNode; 		   	
        
	        Node* SrchNode;            // Used for searching for nodes which overlap
         
	        BOOL Intersects;           // Flag which indicates if there is a node which overlaps the
	        						   // current node. 
	        BOOL PassedPrevSel;        
       
            
	        // loop until there are no more selected nodes						 
	        while (CurrentNode != NULL) 
	        {   
	        	// Make sure we are dealing with a NodeRenderableBounded
				ENSURE (CurrentNode->IsAnObject(), 
						"Selected node not a NodeRenderableInk");
					
				// The current node will probably be moving very shortly, so lets find out the next node 
				// which needs to be processed now. We could do this after the node has been moved but 
				// this would be less than optimal.  
				NextSelNode = LayerRange.FindPrev (CurrentNode, FALSE);

				// We first need to decide where the CurrentNode is to be moved to, if it is to be 
	        	// moved at all. 
        	
				// -------------------------------------------------------------------------
	        	// See if there is a node which is overlapping the current node 
             
	            Intersects = FALSE;        
	            PassedPrevSel = FALSE;    
             
	        	SrchNode = CurrentNode->FindNext(); 	
	        	DocRect CurrentBounds = ((NodeRenderableBounded*)CurrentNode)->GetBoundingRect();
	        	while (SrchNode != NULL)           
	        	{   
	        		if (SrchNode->IsAnObject()) // Ignore hidden nodes/ Insertion node etc.
	        		{           
		        		if (SrchNode == PrevSelNode)
		        		{   
							PassedPrevSel = TRUE; 					
							// carry on searching 
		        		}
		        		if (((NodeRenderableBounded*)SrchNode)->GetBoundingRect().IsIntersectedWith
		        			(CurrentBounds))
		        		{         
		        			Intersects = TRUE;           
		        			break; 
		        		}
	        		}
	        		SrchNode = SrchNode->FindNext(); 	
	        	}

				// -------------------------------------------------------------------------
        	      
	        	// If there is a node which overlaps: then either insert the node as a next sibling of this 
	        	// node, or as a previous sibling of the previously processed node (whichever is lower). 
        	      
	        	if (Intersects) 
	        	{     
	        		if (PassedPrevSel) // We cannot insert the node in front of the node which 
	        						   // overlaps it. This would destroy the relative
	        						   // Z order !
	        		{
	        			if (PrevSelNode->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk))  != CurrentNode) 
	        			{
	        				// Insert the node as a previous sibling of the previously processed node
							((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
							if (!DoMoveNode(CurrentNode, PrevSelNode, PREV)) 
								goto EndOperation; 

							((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
							CurrentNode->SetSelected(TRUE);
	        			}	                                           
	        			// else the node is already in the correct position behind the previously
	        			// processed node. 
	        		}
	        		else
	        		{
	        			// Insert the node after the node which intersects it. 
						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						if (!DoMoveNode(CurrentNode, SrchNode, NEXT)) 
							goto EndOperation; 

						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						CurrentNode->SetSelected(TRUE);
	        		}
	        	}
	        	else // No node intersects with the current node 
	        	{ 
	        		// There is no overlapping node, so move the node one position, or do not move it 
	        		// at all if the next node is the previously moved node, or if the node is 
	        		// already at the front of the list.      

					Node* NextObjAfterCurrent = CurrentNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
	        			
					if (NextObjAfterCurrent != PrevSelNode) 
	        		{   
						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
	        			if (!DoMoveNode(CurrentNode,NextObjAfterCurrent, NEXT))
							goto EndOperation;   

						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						CurrentNode->SetSelected(TRUE);
	        		} 
					// else either
					// -Current node is the previous selected sibling of PrevSelNode so don't move it. 
					// -or the current node is already at the head of the list so don't move it 
				}                                     
			 
				PrevSelNode = CurrentNode;  
				CurrentNode = NextSelNode; // Get next node  
		
			}
			// Process the next layer
			FirstSelectedNode = NextLayerFirstSelectedNode;    
		}
		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}
	} 
	EndOperation:
	GetApplication()->UpdateSelection();
	
	End(); // End of operation

#endif // WEBSTER
}
// ------------------------------------------------------------------------------------------
// OpMoveBackwards methods
            
/********************************************************************************************

>	OpMoveForwards::OpMoveBackwards() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpMoveBackwards constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpMoveBackwards::OpMoveBackwards(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpMoveBackwards::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMoveBackwards initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpMoveBackwards::Init()
{
	return (RegisterOpDescriptor(0,
 								_R(IDS_MOVEBACKWARDSOP),
								CC_RUNTIME_CLASS(OpMoveBackwards),
 								OPTOKEN_MOVEBACKWARDS,
 								OpPutToBack::GetState,
 								0,	/* help ID */
 								_R(IDBBL_MOVEBACKWARDSOP),
 								0,	/* bitmap ID */
 								0,
								SYSTEMBAR_ILLEGAL,			// For now !
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								)); 
}               


/********************************************************************************************

>	void OpMoveBackwards::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Markn for WEBSTER - 2/2/97)
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the MoveBackwards operation 

				See note at the top of file about WEBSTER z-order ops
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpMoveBackwards::Do(OpDescriptor*)		   
{   
#ifdef WEBSTER

	SelRange* Sel = GetApplication()->FindSelection();

	Node* pSelNode = Sel->FindFirst();

	// If there are selected objects, record undo stuff about the selection
	if (pSelNode != NULL)
	{
		// Try to record the selection state , don't render the blobs though
		// the invalidate will take care of this. 
		if (!DoStartSelOp(FALSE,FALSE))
			goto EndOperation;  

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*Sel, TRUE))
			goto EndOperation; 
	}

	while (pSelNode != NULL)
	{
		Node* pContextNode = NULL;
		BOOL SelNodeFound = FALSE;

		Node* pNode = pSelNode;
		while (pNode != NULL && pContextNode == NULL && !SelNodeFound)
		{
			Node* pOldNode = pNode;		// Remember this node

			if (pNode != NULL)
				pNode = pNode->FindPrevious();	// Get the node before this node

			// If the previous node	is an unselected object, then we can move this object back
			if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
				pContextNode = pNode;

			SelNodeFound = (pNode != NULL && pNode->IsAnObject() && pNode->IsSelected());

			if (pNode == NULL)
			{
				// Find the parent layer
				Layer* pLayer = (Layer*)pOldNode->FindParent(CC_RUNTIME_CLASS(Layer));

				// Find the last object on the first editable layer we can find that preceeds this layer
				do
				{
					pLayer = pLayer->FindPrevLayer(TRUE);
					pNode = FindLastObject(pLayer);

				} while (pLayer != NULL && pNode == NULL);

				// If the previous node	is an unselected object, then we can move this object back
				if (pNode != NULL && pNode->IsAnObject() && !pNode->IsSelected())
					pContextNode = pNode;
			}
		} 

		pNode = pSelNode;
		// Try the next selected node.
		pSelNode = Sel->FindNext(pSelNode);

		if (pContextNode != NULL && !SelNodeFound)
		{
			if (!DoMoveNode(pNode, pContextNode, PREV))
				goto EndOperation;

			pNode->SetSelected(TRUE);
		}
	}

	EndOperation:	 
	End(); // End of operation

#else

// WEBSTER - markn 2/2/97
// Original code

	// Obtain the current selections
	Range Sel(*(GetApplication()->FindSelection()));

	// DMc
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
 
	// The first selected node should not ever be NULL
	ENSURE(FirstSelectedNode != NULL, 
		"The OpPutToBack's GetState fn has not done a very good job"); 
	
	// Make sure we are dealing with a NodeRenderable
	ENSURE (FirstSelectedNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), 
			"Selected node not a NodeRenderableBounded");  
	 
	// We can only do something if there are any selected nodes  
	if (FirstSelectedNode != NULL)  
	{                                                           
		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't
										 // render the blobs though, the invalidate
										 // will take care of this. 
		{
			goto EndOperation;  
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}
		
		Node* CurrentNode; 
		CurrentNode = FirstSelectedNode;

 		// Find the spread that the selection lies on
//		Spread* pSpread = CurrentNode->FindParentSpread();

//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE }; // Selected + don't cross layers
		
		Range LayerRange;

		Node* PrevSelNode;  

        Node* NextSelNode; 		   	
        
        Node* SrchNode;            // Used for searching for nodes which overlap
                                       
        BOOL Intersects;           // Flag which indicates if there is a node which intersects the
        						   // current node. 
        BOOL PassedPrevSel;        
     
	  	Node* LastSelectedOnLayer=NULL; 

		// Loop for all layers
		while (CurrentNode != NULL)
		{

			// Create a range of the selected nodes on this layer
			Range temp(CurrentNode,
							NULL,
							RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );
			LayerRange = temp;
//			LayerRange = Range(CurrentNode,
//							   NULL,
//							   RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );

		
			PrevSelNode = NULL;    // We never ever move a node further backward than the previously 
								// processed node (This is to maintain the relative Z-Order of the 
								// nodes).    
			
			// loop until there are no more selected nodes on this layer					 
			while (CurrentNode != NULL) 
			{   
				// Make sure we are dealing with a NodeRenderableBounded
				ENSURE (CurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), 
						"Selected node not a NodeRenderableBounded");      
					
				// The current node will probably be moving very shortly, so lets find out the next node 
				// which needs to be processed now. We could do this after the node has been moved but 
				// this would be less than optimal.  
				NextSelNode = LayerRange.FindNext(CurrentNode); 
			
				// We first need to decide where the CurrentNode is to be moved to, if it is to be 
				// moved at all. 
			
				// ------------------------------------------------------------------------- 
				// See if there is a node which is intersecting the current node 
			
				Intersects = FALSE;     
				PassedPrevSel = FALSE; 
				
				DocRect CurrentsBounds = ((NodeRenderableBounded*)CurrentNode)->GetBoundingRect();   
			
				SrchNode = CurrentNode->FindPrevious(); 	
				while (SrchNode != NULL)           
				{   
					if (SrchNode->IsAnObject())		// Ignore Hidden/Insertion nodes
					{           
						if (SrchNode == PrevSelNode)
						{   
							PassedPrevSel = TRUE; 					
							// carry on searching 
	    	    		}
	        			if (((NodeRenderableBounded*)SrchNode)->GetBoundingRect().IsIntersectedWith
	        				(CurrentsBounds))
	        			{         
	        				Intersects = TRUE;           
		        			break; 
	    	    		}
	        		}
        			SrchNode = SrchNode->FindPrevious(); 
	        	}
        	    
        	    // -------------------------------------------------------------------------
        	      
    	    	// If there is a node which intersects: Then either insert the node as previous 
        		// sibling of this node, or as a next sibling of the previously processed node 
        		// (whichever is higher). 
        	     
        	    Node* ObjPrevCurrent = CurrentNode->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));   
	        	if (Intersects) 
    	    	{     
        			if (PassedPrevSel) // We cannot insert the node behind the node which 
        							   // it overlaps. This would destroy the relative
        							   // Z order !
	        		{
    	    			if (PrevSelNode  != ObjPrevCurrent) 
        				{
        					// Insert the node as a next sibling of the previously processed node
							((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
							if (!DoMoveNode((NodeRenderableBounded*)CurrentNode, PrevSelNode, NEXT))
								goto EndOperation;   

							((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
							CurrentNode->SetSelected(TRUE);
	        			}	                                           
    	    			// else the node is already in the correct position as a next sibling of the
        				// previously processed node. 
        			}
	        		else
    	    		{   
        				// Insert the node as a previous sibling of the node which it currently overlaps.
						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						if (!DoMoveNode((NodeRenderableBounded*)CurrentNode, SrchNode, PREV))
							goto EndOperation;   

						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						CurrentNode->SetSelected(TRUE);
				
					}
				}
				else // No node intersects with the current node 
				{ 
					// There is no overlapping node, so move the node one position, or do not move it 
					// at all if the prev node is the previously moved node, or if the node is 
					// already at the tail of the list.      
				
					if (PrevSelNode  != ObjPrevCurrent) 
    	    		{   
						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						if (!DoMoveNode((NodeRenderableBounded*)CurrentNode, ObjPrevCurrent, PREV))
							goto EndOperation;   

						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						CurrentNode->SetSelected(TRUE);
					} 
					// else either
					// - Current node is the next selected sibling of PrevSelNode so don't move it. 
					// - or current node is already at the tail of the list so don't move it 
				}                                     
				PrevSelNode = CurrentNode;  
				LastSelectedOnLayer = CurrentNode; 
				CurrentNode = NextSelNode; // Get next node  
			}

			// Process the next layer
			ENSURE(LayerRange.FindNext(LastSelectedOnLayer) == NULL, 
				"Current should be the last selected node on the layer");

			CurrentNode = Sel.FindNext(LastSelectedOnLayer);
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}

	}   
	EndOperation:
	GetApplication()->UpdateSelection();
	
	End(); // End of operation

#endif // WEBSTER
}  

// ------------------------------------------------------------------------------------------
// OpMoveToLyrInFront methods



/********************************************************************************************

>	OpMoveToLyrInFront::OpMoveToLyrInFront() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpMoveToLyrInFront constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpMoveToLyrInFront::OpMoveToLyrInFront(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpMoveToLyrInFront::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMoveToLyrInFront initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

**********************************************************************************************/

BOOL OpMoveToLyrInFront::Init()
{
/*
	return (RegisterOpDescriptor(0,
 								_R(IDS_MOVELAYERINFRONT),
								CC_RUNTIME_CLASS(OpMoveToLyrInFront),
								OPTOKEN_MOVELAYERINFRONT,
 								OpMoveToLyrInFront::GetState,
 								0,	
 								_R(IDBBL_MOVELAYERINFRONT),
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_MOVEUPAFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE, 							// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								)); 
*/
	// Changed for Camelot2, Layer/Frame integration - RanbirR.

	FrameInFrontOpDescriptor* pOpDesc = new FrameInFrontOpDescriptor(OPTOKEN_MOVELAYERINFRONT, 
																	 CC_RUNTIME_CLASS(OpMoveToLyrInFront),
																	 OpMoveToLyrInFront::GetState);
	if(pOpDesc)
		return TRUE;

	return FALSE;
}               

/********************************************************************************************

>	OpState	OpMoveToLyrInFront::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpMoveToLyrInFront operation
	Purpose:	For finding OpMoveToLyrInFront's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpMoveToLyrInFront::GetState(String_256*  UIDescription, OpDescriptor*)
{
	OpState OpSt;

	String_256 DisableReason;

	SelRange Sel(*( GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);


	Node* pNode = Sel.FindFirst();

	while (pNode != NULL)
	{
		Node* pLayer = pNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer != NULL && ((Layer*)pLayer)->IsGuide())
		{
		   	*UIDescription = String_256(_R(IDS_CANT_MOVE_OFF_GUIDE_LAYER));
			OpSt.Greyed = TRUE;
			return OpSt;
		}

		pNode = Sel.FindNext(pNode);
	}

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 

	// The operation is disabled if the first object which is selected is already on the highest
	// visible layer.
	BOOL HighestVisible = TRUE; 
	Layer* CurrentLayer = (Layer*) (FirstSelectedNode->FindParent(CC_RUNTIME_CLASS(Layer)));
	ERROR2IF(CurrentLayer == NULL, OpSt, "Cannot find layer of first selected object"); 
// WEBSTER - markn 2/2/97
// Not needed in Webster
// Neville 23/6/97 Now needed but must work in Frame mode in Webster
// RanbirR - 28/10/97 - Changed for Camelot v2, Frame/Layer integration. 

#ifdef WEBSTER
	// Find the next frame layer
	do
	{
		CurrentLayer = CurrentLayer->FindNextFrameLayer();
		if (CurrentLayer != NULL)
		{
			// Is the layer not a guide or a page background layer?
			if (!CurrentLayer->IsGuide() && !CurrentLayer->IsPageBackground())
			{
				HighestVisible = FALSE; // The first node selected is not on the highest visible layer
										// so the operation can be performed. 
				break;
			}
		} 
	} while (CurrentLayer != NULL);   

#else
	// Are we in frame or layer mode.
	MODE PresentMode = (MODE)CurrentLayer->IsFrame();

	if (PresentMode == FRAME)
	{
		do
		{
			CurrentLayer = CurrentLayer->FindNextFrameLayer();
			if (CurrentLayer != NULL)
			{
				// Is the layer not a guide or a page background layer?
				if (!CurrentLayer->IsGuide() && !CurrentLayer->IsPageBackground())
				{
					HighestVisible = FALSE; // The first node selected is not on the highest visible layer
											// so the operation can be performed. 
					break;
				}
			} 
		} while (CurrentLayer != NULL);   
	}
	else if (PresentMode == LAYER)
	{
		do
		{
			CurrentLayer = CurrentLayer->FindNextLayer();
			if (CurrentLayer != NULL)
			{
				// Is the layer visible
				if (CurrentLayer->IsVisible() && !CurrentLayer->IsLocked() && !CurrentLayer->IsGuide())
				{
					HighestVisible = FALSE; // The first node selected is not on the highest visible layer
											// so the operation can be performed. 
					break;
				}
			} 
		} while (CurrentLayer != NULL);   
	}
	else
		ERROR3("OpMoveToLyrInFront::GetState - Bad mode!");

#endif // WEBSTER

	if (HighestVisible)
	{
		// The operation must be disabled
		OpSt.Greyed = TRUE;

		#ifdef WEBSTER

			if (Sel.FindNext(FirstSelectedNode) == NULL)
			{
				// There is only a single selected node
				DisableReason = String_256(_R(IDS_ALREADY_ON_TOP_FRMS)); 
			}
			else
			{
				// There is more than one selected node
				DisableReason = String_256(_R(IDS_ALREADY_ON_TOP_FRM)); 
			}
		#else
			if(PresentMode == LAYER)
			{
				if (Sel.FindNext(FirstSelectedNode) == NULL)
				{
					// There is only a single selected node
					DisableReason = String_256(_R(IDS_ALREADY_ON_TOP_LYRS)); 
				}
				else
				{
					// There is more than one selected node
					DisableReason = String_256(_R(IDS_ALREADY_ON_TOP_LYRP)); 
				}
			}
			else if(PresentMode == FRAME)
			{
				if (Sel.FindNext(FirstSelectedNode) == NULL)
				{
					// There is only a single selected node
					DisableReason = String_256(_R(IDS_ALREADY_ON_TOP_FRMS)); 
				}
				else
				{
					// There is more than one selected node
					DisableReason = String_256(_R(IDS_ALREADY_ON_TOP_FRM)); 
				}
			}
			else
				ERROR3("OpMoveToLyrInFront::Do - Bad mode!");
		#endif
	}

   	*UIDescription = DisableReason;	 

	return(OpSt);   
}

/********************************************************************************************

>	void OpMoveToLyrInFront::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the MoveToLayerInFront operation. This moves those selected objects
				which are not on the highest visible layer to the lowest z-order position of the next 
				visible layer in front. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpMoveToLyrInFront::Do(OpDescriptor*)		   
{  
	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
 
	// The first selected node should not ever be NULL
	ENSURE(FirstSelectedNode != NULL, "The OpMoveToLyrInFront's GetState fn has not done a very good job"); 
	
	if (FirstSelectedNode != NULL) 
	{
		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't
										 // render the blobs though 
		{
			goto EndOperation;  
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}

		Node*	CurrentNode			= FirstSelectedNode; 
		
//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE };  // Selected + don't cross layers  
		Range	LayerRange; 
	   	Layer*	NxtLyr				= NULL;
		Node*	NextNode			= NULL; 
		Node*	ThisLayer			= NULL;
		Node*	FirstSelectedNxtLyr	= NULL;
		Node*	LastObjectMoved		= NULL; 

		BOOL	OldVisible			= FALSE;
		BOOL	OldLocked			= FALSE;

#ifndef WEBSTER
		MODE PresentMode			= LAYER; 
#endif
		// Loop for all layers
		// Current Node is the first selected object on the layer we are currently processing
		while (CurrentNode != NULL)
		{
			ENSURE(CurrentNode->IsAnObject(), "Non ink node in OpMoveToLyrInFront");

			ThisLayer = CurrentNode->FindParent(CC_RUNTIME_CLASS(Layer));

			if (ThisLayer != NULL)
				((Layer*)ThisLayer)->InvalidateBoundingRect();

// WEBSTER - markn 2/2/97
// Not needed in Webster
// Neville 23/6/97 Now needed but must work in Frame mode in Webster
// RanbirR - 28/10/97 - Changed for Camelot v2, Frame/Layer integration. 

#ifdef WEBSTER
			// ----------------------------------------------------------------------------
			// Find the next visible frame
			NxtLyr = (Layer*)ThisLayer;
			do
			{
				NxtLyr = NxtLyr->FindNextFrameLayer();
				if (NxtLyr != NULL)
				{
					// Is the layer not a guide or a page background layer?
					if (!NxtLyr->IsGuide() && !NxtLyr->IsPageBackground())
					{
						// found
						break;
					}
				}
				
			} while (NxtLyr != NULL); 

			if (NxtLyr != NULL)
			{
				// Make the layer visible and editable so we don't loose the selection
				// do it blatently as it will be fixed up properly latter
				OldVisible = NxtLyr->IsVisible();
				OldLocked = NxtLyr->IsLocked();
				NxtLyr->SetVisible(TRUE);
				NxtLyr->SetLocked(FALSE);
			}
			
#else
			// ----------------------------------------------------------------------------

			// Determine the Document Mode.
			NxtLyr = (Layer*)ThisLayer;
			PresentMode = (MODE)NxtLyr->IsFrame();
			
			if(PresentMode == LAYER)
			{
				do
				{
					NxtLyr = NxtLyr->FindNextLayer();
					if (NxtLyr != NULL)
					{
						if (NxtLyr->IsVisible() && !NxtLyr->IsLocked() && !NxtLyr->IsGuide())
						{
							// found
							break;
						}
					}
					
				} while (NxtLyr != NULL); 

				if (NxtLyr != NULL)
				{
					// Make the layer visible and editable so we don't loose the selection
					// do it blatently as it will be fixed up properly latter
					OldVisible = NxtLyr->IsVisible();
					OldLocked = NxtLyr->IsLocked();
					NxtLyr->SetVisible(TRUE);
					NxtLyr->SetLocked(FALSE);
				}
			}
			else if (PresentMode == FRAME)
			{
				// Find the next visible frame
				do
				{
					NxtLyr = NxtLyr->FindNextFrameLayer();
					if (NxtLyr != NULL)
					{
						// Is the layer not a guide or a page background layer?
						if (!NxtLyr->IsGuide() && !NxtLyr->IsPageBackground())
						{
							// found
							break;
						}
					}
				
				} while (NxtLyr != NULL); 

				if (NxtLyr != NULL)
				{
					// Make the layer visible and editable so we don't loose the selection
					// do it blatently as it will be fixed up properly latter
					OldVisible = NxtLyr->IsVisible();
					OldLocked = NxtLyr->IsLocked();
					NxtLyr->SetVisible(TRUE);
					NxtLyr->SetLocked(FALSE);
				}
			}
			else
				ERROR3("OpMoveToLyrInFront::Do - Bad mode!");
#endif 

			if (NxtLyr == NULL)
			{
				ENSURE(CurrentNode != FirstSelectedNode, "OpMoveToLyrInFront called with nothing to do"); 
				
				// There is no layer in front so the operation is complete
				goto EndOperation;
			} 

			NxtLyr->InvalidateBoundingRect();

			// Create a range of selected nodes on this layer, we will move these to the layer
			// in front.
			Range temp(CurrentNode, 
						 	   NULL, 
						       RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );
			LayerRange = temp;
//			LayerRange = Range(CurrentNode, 
//						 	   NULL, 
//						       RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );
			
			// Find the first selected node on the layer in front of the layer we are currently 
			// processing. 

			FirstSelectedNxtLyr = CurrentNode;
			
			do 
			{
				if (FirstSelectedNxtLyr->FindParent(CC_RUNTIME_CLASS(Layer)) != ThisLayer)
				{
					break; // Found		
				}
				FirstSelectedNxtLyr = Sel.FindNext(FirstSelectedNxtLyr);

			} while (FirstSelectedNxtLyr != NULL);
			
			// We haven't moved any objects from this layer yet
			LastObjectMoved = NULL; 

			// Shift all selected nodes on this layer to the lowest z-order position on the layer in front
			// maintaining the objects relative order 
			do
			{
				NextNode = LayerRange.FindNext(CurrentNode); // Hand over hand

				if (LastObjectMoved == NULL)
				{
					// No previous objects have been copied from this layer to the NxtLayer, therefore
					// Attach the node as a first child of the next layer
					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					if (!DoMoveNode(CurrentNode, NxtLyr, FIRSTCHILD)) 
					{
						goto EndOperation;
					}

					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					CurrentNode->SetSelected(TRUE);
				}
				else
				{
					// Copy the rest of the objects after the first object
					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					if (!DoMoveNode(CurrentNode, LastObjectMoved, NEXT)) 
					{
						goto EndOperation;
					}

					((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
					CurrentNode->SetSelected(TRUE);
				}

				LastObjectMoved = CurrentNode; 	// Remember the node we have just moved
				CurrentNode = NextNode; 
			
			} while (NextNode != NULL);

//#ifdef WEBSTER	Included in Camelot2 for Frame/Layer integration - RanbirR 03/11/97	
#ifdef WEBSTER
			if (NxtLyr != NULL)
#else
			if (NxtLyr != NULL && PresentMode == FRAME)
#endif
			{
				// Put back the old states so we get the redraw correct
				NxtLyr->SetVisible(OldVisible);
				//NxtLyr->SetLocked(OldLocked);
			}
//#endif	
			// Get the first selected node of the next layer
			CurrentNode = FirstSelectedNxtLyr; 
		} 

//#ifdef WEBSTER	Included in Camelot2 for Frame/Layer integration - RanbirR 03/11/97	

		// Switch to viewing the destination layer
		// Otherwise, the user will see the objects dissappear and will be worried
		// Also, the range blows up as the destination is invisible
#ifdef WEBSTER
		if (NxtLyr != NULL)
#else
		if (NxtLyr != NULL && PresentMode == FRAME)
#endif
		{
PORTNOTETRACE("gallery", "Removed used of FrameSGallery from OpMoveToLyrInFront");
#if !defined(EXCLUDE_FROM_XARALX)
			FrameSGallery::MakeActiveLayer(NxtLyr);
			BROADCAST_TO_ALL(LayerMsg(NxtLyr,LayerMsg::UPDATE_ACTIVE_LAYER));
#endif
		}

//#endif // WEBSTER		Included in Camelot2 for Frame/Layer integration - RanbirR 03/11/97	
		
		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}
	
		// Invalidate the bounding rects
		if (FirstSelectedNode->IsBounded())
		{
			NodeRenderableBounded* pNodeRB = (NodeRenderableBounded*)FirstSelectedNode;
			pNodeRB->InvalidateBoundingRect();
		}
	}

	EndOperation:

	End(); // End of operation
} 


// ------------------------------------------------------------------------------------------
// OpMoveToLyrBehind methods


/********************************************************************************************

>	OpMoveToLyrBehind::OpMoveToLyrBehind() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpMoveToLyrBehind constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpMoveToLyrBehind::OpMoveToLyrBehind(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpMoveToLyrBehind::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpMoveToLyrBehind initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpMoveToLyrBehind::Init()
{
/*
	return (RegisterOpDescriptor(0,
 								_R(IDS_MOVELAYERBEHIND),
								CC_RUNTIME_CLASS(OpMoveToLyrBehind),
 								OPTOKEN_MOVELAYERBEHIND,
 								OpMoveToLyrBehind::GetState,
 								0,	// help ID
 								_R(IDBBL_MOVELAYERBEHIND),
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_MOVEDOWNAFRAME),		// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE, 							// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)

 								));
*/

	FrameBehindOpDescriptor* pOpDesc = new FrameBehindOpDescriptor(OPTOKEN_MOVELAYERBEHIND, 
																   CC_RUNTIME_CLASS(OpMoveToLyrBehind),
																   OpMoveToLyrBehind::GetState);

	if(pOpDesc)
		return TRUE;
	
	return FALSE;
}               

/********************************************************************************************

>	OpState	OpMoveToLyrBehind::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpMoveToLyrBehind operation
	Purpose:	For finding OpMoveToLyrBehind's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpMoveToLyrBehind::GetState(String_256*  UIDescription, OpDescriptor*)
{
	OpState OpSt;

	// In Camelot, work as before in layer mode
	// In Webster, work only on frame layers
	// This should never go off in Webster but we will leave it in just in case
	String_256 DisableReason;

	SelRange Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	Node* pNode = Sel.FindFirst();
	while (pNode != NULL)
	{
		Node* pLayer = pNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer != NULL && ((Layer*)pLayer)->IsGuide())
		{
		   	*UIDescription = String_256(_R(IDS_CANT_MOVE_OFF_GUIDE_LAYER));
			OpSt.Greyed = TRUE;
			return OpSt;
		}

		pNode = Sel.FindNext(pNode);
	}

// WEBSTER - markn 2/2/97
// Not needed in Webster. Now required but must be frame orientated
// RanbirR - 28/10/97 - Changed for Camelot v2, Frame/Layer integration. 

#ifdef WEBSTER
	// We need to work in Frame mode in Webster

	// The operation is disabled if there does not exist a selected node on a layer 
	// which is not the bottom-most layer.
	
	// Find the first node which is selected 
	Node* pLastSelectedNode = Sel.FindLast(); 
		 
	// The operation is disabled if the last selected object is already on the lowest
	// visible layer.
	BOOL LowestVisible = TRUE; 
	Layer* pCurrentLayer = (Layer*) (pLastSelectedNode->FindParent(CC_RUNTIME_CLASS(Layer)));
	ERROR2IF(pCurrentLayer == NULL, OpSt, "Cannot find layer of first selected object"); 
	do
	{
		pCurrentLayer = pCurrentLayer->FindPrevFrameLayer();
		if (pCurrentLayer != NULL)
		{
			// Is the layer not a guide or a page background layer?
			if (!pCurrentLayer->IsGuide() && !pCurrentLayer->IsPageBackground())
			{
				LowestVisible = FALSE; // The last node selected is not on the lowest visible frame
									   // so the operation can be performed. 
				break;
			}
		} 
	} while (pCurrentLayer != NULL);   

#else

	// Find the first node which is selected 
	Node* pLastSelectedNode = Sel.FindLast();
	
	// Karim 18/04/2000 - spot-check! - any NULL pointers??
	if (pLastSelectedNode == NULL)
	{
		// fail in the same way as the layer check below, but we'll only complaing in debug.
		ERROR3("OpMoveToLyrBehind::GetState; NULL returned as last node in Selection!");
		return OpSt;
	}

	// The operation is disabled if the last selected object is already on the lowest
	// visible layer.
	BOOL LowestVisible = TRUE; 
	Layer* pCurrentLayer = (Layer*) (pLastSelectedNode->FindParent(CC_RUNTIME_CLASS(Layer)));
	ERROR2IF(pCurrentLayer == NULL, OpSt, "Cannot find layer of first selected object"); 

	// Determine the Document mode. 
	MODE PresentMode = (MODE)pCurrentLayer->IsFrame();
	if (PresentMode == LAYER)
	{
		do
		{
			pCurrentLayer = pCurrentLayer->FindPrevLayer();
			if (pCurrentLayer != NULL)
			{
				// Is the layer visible
				if (pCurrentLayer->IsVisible() && !pCurrentLayer->IsLocked() && !pCurrentLayer->IsGuide())
				{
						LowestVisible = FALSE; // The last node selected is not on the lowest visible layer
										// so the operation can be performed. 
					break;
				}
			} 
		} while (pCurrentLayer != NULL);   
	}
	else if (PresentMode == FRAME)
	{
		do
		{
			pCurrentLayer = pCurrentLayer->FindPrevFrameLayer();
			if (pCurrentLayer != NULL)
			{
				// Is the layer not a guide or a page background layer?
				if (!pCurrentLayer->IsGuide() && !pCurrentLayer->IsPageBackground())
				{
					LowestVisible = FALSE; // The last node selected is not on the lowest visible frame
										   // so the operation can be performed. 
					break;
				}
			} 
		} while (pCurrentLayer != NULL);   
	}
	else
		ERROR3("OpMoveToLyrInFront::GetState - Bad mode!");
#endif // WEBSTER

	if (LowestVisible)
	{
		// The operation must be disabled
		OpSt.Greyed = TRUE;


		if (Sel.FindFirst() == pLastSelectedNode)
		{
			#ifdef WEBSTER
				// There is only a single selected node
				DisableReason = String_256(_R(IDS_ALREADY_ON_BOTTOM_FRMS)); 				
			#else
				if(PresentMode == FRAME)
					DisableReason = String_256(_R(IDS_ALREADY_ON_BOTTOM_FRMS)); 				
				else if(PresentMode == LAYER)
					DisableReason = String_256(_R(IDS_ALREADY_ON_BOTTOM_LYRS)); 
				else
					ERROR3("OpMoveToLyrInBehind::GetState - Bad mode!");
			#endif
		}
		else
		{
			#ifdef WEBSTER
				// There is more than one selected node
				DisableReason = String_256(_R(IDS_ALREADY_ON_BOTTOM_FRM)); 	
			#else
				if(PresentMode == FRAME)
				{
					// There is more than one selected node
					DisableReason = String_256(_R(IDS_ALREADY_ON_BOTTOM_FRM)); 						
				}
				else if (PresentMode == LAYER)
				{
					// There is more than one selected node
					DisableReason = String_256(_R(IDS_ALREADY_ON_BOTTOM_LYRP)); 
				}
				else
					ERROR3("OpMoveToLyrInFront::GetState - Bad mode!");
			#endif
		}
	}
	
   	*UIDescription = DisableReason;	 

	return(OpSt);   
}

/********************************************************************************************

>	void OpMoveToLyrBehind::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the OpMoveToLyrBehind operation. This moves all selected nodes 
				(which are not on the lowest visible layer) to the highest z-order position 
				on the next visible layer behind.
				
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
		
void OpMoveToLyrBehind::Do(OpDescriptor*)		   
{
	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);


	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
 
	// The first selected node should not ever be NULL
	ENSURE(FirstSelectedNode != NULL, 
		"The OpMoveToLyrInFront's GetState fn has not done a very good job"); 
	
	if (FirstSelectedNode != NULL) 
	{
		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't
										 // render the blobs though 
		{
			goto EndOperation;  
		}

		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE, FALSE, FALSE, FALSE))
		{
			goto EndOperation; 
		}

		Node*	CurrentNode				= FirstSelectedNode; 
		
//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE };  // Selected + don't cross layers  
		Range	LayerRange; 

	   	Node*	Destination				= NULL;	// Pointer to the last child of the previous layer.
												// We move all nodes to be next siblings of this node
		Node*	ThisLayer				= NULL;
		Layer*	PrevLayer				= NULL;
		Node*	LastSelectedThisLayer	= NULL;
		Node*	NextNode				= NULL;

		BOOL	OldVisible				= FALSE;
		BOOL	OldLocked				= FALSE;

#ifndef WEBSTER
		MODE PresentMode				= LAYER;
#endif
		// Loop for all layers
		while (CurrentNode != NULL)
		{
			ENSURE(CurrentNode->IsAnObject(), "Non ink node in OpMoveToLyrBehind");

			// Create a range of selected nodes on this layer
			Range temp(CurrentNode, 
						 	   NULL, 
						       RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );
			LayerRange = temp;
//			LayerRange = Range(CurrentNode, 
//						 	   NULL, 
//						       RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE) );

			// -----------------------------------------------------------------------------
			// Find the last child object of the previous visible layer, which is the destination for all
			// nodes moved from this layer.
			// - there may not be a previous layer
			// - there may not be any children on the previous layer
			Destination = NULL;

			ThisLayer = CurrentNode->FindParent(CC_RUNTIME_CLASS(Layer));

			if (ThisLayer != NULL)
				((Layer*)ThisLayer)->InvalidateBoundingRect();
						
// WEBSTER - markn 2/2/97
// Not needed in Webster.
// Neville 23/6/97 Now needed but must work in Frame mode in Webster
// RanbirR - 28/10/97 - Changed for Camelot v2, Frame/Layer integration. 

#ifdef WEBSTER
			// ----------------------------------------------------------------------------
			// Find the previous frame
			PrevLayer = (Layer*)ThisLayer;
			do
			{
				PrevLayer = PrevLayer->FindPrevFrameLayer();
				if (PrevLayer != NULL)
				{
					// Is the layer not a guide or a page background layer?
					if (!PrevLayer->IsGuide() && !PrevLayer->IsPageBackground())
					{
						// found
						break;
					}
				}
				
			} while (PrevLayer != NULL); 

			if (PrevLayer != NULL)
			{
				// Make the layer visible and editable so we don't loose the selection
				// do it blatently as it will be fixed up properly latter
				OldVisible = PrevLayer->IsVisible();
				OldLocked = PrevLayer->IsLocked();
				PrevLayer->SetVisible(TRUE);
				PrevLayer->SetLocked(FALSE);
			}

#else
			// -----------------------------------------------------------------------------

			// Determine the document mode.
			PrevLayer = (Layer*)ThisLayer;
			PresentMode = (MODE)PrevLayer->IsFrame();

			if(PresentMode == LAYER)
			{
				do
				{
					PrevLayer = (Layer*)(PrevLayer->FindPrevious(CC_RUNTIME_CLASS(Layer)));
					if (PrevLayer != NULL)
					{
						if (PrevLayer->IsVisible() && !PrevLayer->IsLocked() && !PrevLayer->IsGuide())
						{
							// found
							break;
						}
					}
					
				} while (PrevLayer != NULL); 

				if (PrevLayer != NULL)
				{
					// Make the layer visible and editable so we don't loose the selection
					// do it blatently as it will be fixed up properly latter
					OldVisible = PrevLayer->IsVisible();
					OldLocked = PrevLayer->IsLocked();
					PrevLayer->SetVisible(TRUE);
					PrevLayer->SetLocked(FALSE);
				}
			}
			else if (PresentMode == FRAME)
			{
				// Find the previous visible layer
				do
				{
					PrevLayer = PrevLayer->FindPrevFrameLayer();
					if (PrevLayer != NULL)
					{
						// Is the layer not a guide or a page background layer?
						if (!PrevLayer->IsGuide() && !PrevLayer->IsPageBackground())
						{
							// found
							break;
						}
					}
				
				} while (PrevLayer != NULL); 

				if (PrevLayer != NULL)
				{
					// Make the layer visible and editable so we don't loose the selection
					// do it blatently as it will be fixed up properly latter
					OldVisible = PrevLayer->IsVisible();
					OldLocked = PrevLayer->IsLocked();
					PrevLayer->SetVisible(TRUE);
					PrevLayer->SetLocked(FALSE);
				}
			}
			else
				ERROR3("OpMoveToLyrBehind::Do - Bad mode");
#endif
			if (PrevLayer != NULL)
			{
				PrevLayer->InvalidateBoundingRect();

				Destination = PrevLayer->FindLastChild();
				
				if (Destination != NULL)  // It will be NULL if there are no objects on the
											   // previous layer
				{
					// The Destination node could be an Insertion node, we certainly don't want to insert
					// after that
					if (!(Destination->IsAnObject()))
					{
						Destination = Destination->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableInk));	
					}

				}
			} 

			// If there is a previous layer then shift all selected nodes 
			// on this layer to the layer behind.
			do
			{
				NextNode = LayerRange.FindNext(CurrentNode); // Hand over hand

				if (PrevLayer != NULL)	 
				{
					if (Destination == NULL)
					{
						// The destination is NULL which means that the layer
						// has no existing objects
						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						if (!DoMoveNode(CurrentNode, PrevLayer, FIRSTCHILD)) 
						{
							goto EndOperation;
						}

						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						CurrentNode->SetSelected(TRUE);
					}
					else
					{
						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						if (!DoMoveNode(CurrentNode, Destination, NEXT)) 
						{
							goto EndOperation;
						}

						((NodeRenderableBounded*)CurrentNode)->ReleaseCached(TRUE, FALSE, FALSE, TRUE);
						CurrentNode->SetSelected(TRUE);
					}
					Destination = CurrentNode; 
				} 


				LastSelectedThisLayer = CurrentNode;  // Used to find first 
													  // selected on next layer

				CurrentNode = NextNode; 
			
			} while (CurrentNode != NULL);

// #ifdef WEBSTER //  Include in Camelot2 for Frame/Layer integration - RanbirR 03/11/97
#ifdef WEBSTER
			if (PrevLayer != NULL)
#else
			if (PrevLayer != NULL && PresentMode == FRAME)
#endif
			{
				// Put back the old states so we get the redraw correct
				PrevLayer->SetVisible(OldVisible);
				//PrevLayer->SetLocked(OldLocked);
			}

//#endif	
			// Obtain the first selected node of the next layer
			CurrentNode = Sel.FindNext(LastSelectedThisLayer); 
		} 
		
//#ifdef WEBSTER	Include in Camelot2 for Frame/Layer integration - RanbirR 03/11/97	

		// Switch to viewing the destination layer
		// Otherwise, the user will see the objects dissappear and will be worried
		// Also, the range blows up as the destination is invisible
#ifdef WEBSTER //  Include in Camelot2 for Frame/Layer integration - RanbirR 03/11/97
		if (PrevLayer != NULL)
#else
		if (PrevLayer != NULL && PresentMode == FRAME)
#endif
		{
PORTNOTETRACE("gallery", "Removed used of FrameSGallery from OpMoveToLyrBehind");
#if !defined(EXCLUDE_FROM_XARALX)
			FrameSGallery::MakeActiveLayer(PrevLayer);
			BROADCAST_TO_ALL(LayerMsg(PrevLayer,LayerMsg::UPDATE_ACTIVE_LAYER));
#endif
		}

//#endif // WEBSTER		
		
		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE))
		{
			goto EndOperation; 
		}

		// Call ChangeBounds() on the first node to ensure all parent node bounds contain
		// the newly moved nodes bounds
		if (FirstSelectedNode->IsBounded())
		{
			NodeRenderableBounded* pNodeRB = (NodeRenderableBounded*)FirstSelectedNode;
			pNodeRB->InvalidateBoundingRect();
		}
	}

EndOperation:
	GetApplication()->UpdateSelection();

	End(); // End of operation  

} 

/********************************************************************************************
/// OpCombineLayersToFrameLayer
********************************************************************************************/

/********************************************************************************************

>	OpCombineLayersToFrameLayer::OpCombineLayersToFrameLayer() 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/97
	Purpose:	OpCombineLayersToFrameLayer constructor

********************************************************************************************/
            
            
/* OpCombineLayersToFrameLayer::OpCombineLayersToFrameLayer(): SelOperation()								
{                              
} */

 /********************************************************************************************

>	BOOL OpCombineLayersToFrameLayer::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/97
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpCombineLayersToFrameLayer constructor
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

/* BOOL OpCombineLayersToFrameLayer::Init()
{
	return RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_COMBINELAYERSTOFRAMELAYER),	// String resource ID
								CC_RUNTIME_CLASS(OpCombineLayersToFrameLayer),	// Runtime class
	 							OPTOKEN_COMBINELAYERSTOFRAMELAYER,		// Token string
	 							OpCombineLayersToFrameLayer::GetState,	// GetState function
	 							0,								// help ID
								0,								// bubble help
								0,								// resource ID
								0,								// control ID
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								FALSE,							// Clean operation
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
							   );
}  */             

/********************************************************************************************

>	OpState	OpCombineLayersToFrameLayer::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpCombineLayersToFrameLayer operation
	Purpose:	For finding the OpCombineLayersToFrameLayer's state. 

********************************************************************************************/

/* OpState	OpCombineLayersToFrameLayer::GetState(String_256*  UIDescription, OpDescriptor*)
{
	// We are not a visible operation and so don't need to return reason strings

	OpState OpSt;

	// Assume operation is disabled by default
	OpSt.Greyed = TRUE;
	
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// See if we have frame layers present. If so, then do we have non-frame layers present?
		// The operation needs to ensure that all other non-guide and non-background layers are frame layers
		// We could be in the situation where we have a single non-frame layer present, so we must
		// include this
//		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();
//		if (pFrameLayer)
		UINT32 NonFrameLayersFound = 0;
		Layer* pCurrentLayer = pSpread->FindFirstLayer();
		while (pCurrentLayer != NULL)
		{
			if (!pCurrentLayer->IsBackground() && !pCurrentLayer->IsGuide() &&
				!pCurrentLayer->IsPageBackground() && !pCurrentLayer->IsFrame())
			{
				NonFrameLayersFound++;
			}

			pCurrentLayer = pCurrentLayer->FindNextLayer();
		}

		if (NonFrameLayersFound > 0)
		{
			// The operation is enabled
			OpSt.Greyed = FALSE;
		}
	}

	return OpSt;   
} */

/********************************************************************************************

>	virtual void OpCombineLayersToFrameLayer::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/97
	Inputs:		OpDescriptor (unused)
	Purpose:	Performs the OpCombineLayersToFrameLayer operation.
				This sees if we have frame layers present. If so, then it checks if there 
				are any non-frame layers present. If we have a mixture of frame and non-frame
				layers then combine all non-frame layers together onto one layer and make it
				a frame layer.

********************************************************************************************/
		
/* void OpCombineLayersToFrameLayer::Do(OpDescriptor* pOpDesc)		   
{
	TRACEUSER( "Neville", _T("OpCombineLayersToFrameLayer::Do\n"));
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		OpParam Param;
		Param.Param1 = (INT32)pSpread;
		DoWithParam(pOpDesc, &Param);
	}
} */

/********************************************************************************************

>	virtual void OpCombineLayersToFrameLayer::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/97
	Inputs:		OpDescriptor (unused)
				pParam->Param1	pointer to the spread to use
	Purpose:	Performs the OpCombineLayersToFrameLayer operation.
				This sees if we have frame layers present. If so, then it checks if there 
				are any non-frame layers present. If we have a mixture of frame and non-frame
				layers then combine all non-frame layers together onto one layer and make it
				a frame layer.

********************************************************************************************/

/* void OpCombineLayersToFrameLayer::DoWithParam(OpDescriptor* pOpDesc, OpParam* pParam)
{
	if (pParam == NULL)
	{
		ERROR3("OpCombineLayersToFrameLayer::DoWithParam Bad params!");
		End();
		return;
	}

	TRACEUSER( "Neville", _T("OpCombineLayersToFrameLayer::DoWithParam\n"));
	
	// Recover the passed in spread pointer
	Spread * pSpread = (Spread*)pParam->Param1;
	ERROR3IF(!pSpread->IS_KIND_OF(Spread), "OpParam->Param2 passed is not a Spread!");
	if (pSpread == NULL)
	{
		ERROR3("OpCombineLayersToFrameLayer::DoWithParam Bad spread pointer");
		End();
		return;
	}

	// Do something useful
	// Ensure that all other non-guide and non-background layers are frame layers
	UINT32 NonFrameLayersFound = 0;
	Layer* pCurrentLayer = pSpread->FindFirstLayer();
	while (pCurrentLayer != NULL)
	{
		if (!pCurrentLayer->IsBackground() && !pCurrentLayer->IsGuide() &&
			!pCurrentLayer->IsPageBackground() && !pCurrentLayer->IsFrame())
		{
			// Do an undoable form of pCurrentLayer->SetFrame(TRUE);
			OpLayerGalParam Param(LAYER_MAKEFRAME, pSpread);
			Param.pLayer = pCurrentLayer;
			LayerStateAction::Init(this, &UndoActions, Param);
			
			NonFrameLayersFound++;
		}

		pCurrentLayer = pCurrentLayer->FindNextLayer();
	}

	if (NonFrameLayersFound > 0)
	{
		TRACEUSER( "Neville", _T("++++++++++EnsureFrameLayerIntegrity found %d non-frame layers\n"),NonFrameLayersFound);

		// We need to update the display as one or more layer items have changed status
		// So tell ourselves about the change. (We are static and so cannot do it directly!)
		BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::SpreadReason::LAYERCHANGES));
	}

	End(); // End of operation  
	return;
} */

/**************************************************************************************************************************

>	FrameInFrontOpDescriptor::FrameInFrontOpDescriptor(	const TCHAR* pcszToken, 
														CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
														pfnGetState gs = PlugInOp::GetState)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pcszToken		the "OpToken" of the associated Operation
				pClass			the runtime class to use.
				gs				the GetState function to use.
	Outputs:	-
	Returns:	-
	Purpose:	Constructs the new FrameInFrontOpDescriptor
	Errors:		-
	SeeAlso:	-

***************************************************************************************************************************/

FrameInFrontOpDescriptor::FrameInFrontOpDescriptor(	const TCHAR* pcszToken, 
													CCRuntimeClass* pClass, pfnGetState gs)
  :	OpDescriptor(	0,															// tool ID
					_R(IDS_MOVELAYERINFRONT),										// String resource ID (use same for all)
					pClass,														//CC_RUNTIME_CLASS(PlugInOp), Runtime class
					(TCHAR*) pcszToken,											// OpToken
					gs,															// GetState function
					0,														// help ID
					_R(IDBBL_MOVELAYERINFRONT),										// bubble help
					_R(IDD_ANIMATIONBAR),											// resource ID
					_R(IDC_FRAME_MOVEUPAFRAME),										// control ID
					TRUE,														// Recieve system messages
					FALSE,														// Smart duplicate operation
					TRUE,														// Clean operation
					0,															// String for one copy only error
					(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)			// Auto state flags
				)
{}


/********************************************************************************************

>	static OpState FrameInFrontOpDescriptor::GetState(String_256* pDesc, OpDescriptor* pOpDesc)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		pDesc       --- A pointer to a String. GetState fills this with the
								appropriate details for the conditions arising eg. why
								"Previous Zoom" is greyed out.
				pOpDesc		--- A pointer to the OpDescriptor whose state is being
								queried.
	Returns:	An OpState containing the flags that show what is valid.
	Purpose:	Returns the state that this operation should appear in the menus 
				or as a buttom, for example - greyed out, or ticked.

********************************************************************************************/

OpState FrameInFrontOpDescriptor::GetState(String_256*, OpDescriptor* pOpDesc)
{
	// At present, this item is always available.
	OpState OpSt;
	return OpSt;
}


/********************************************************************************************

>	virtual void FrameInFrontOpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>	(From Mario code in base class.)
	Created:	21/10/97
	Inputs:		TextFlag - Identifies which text string to retrieve from the string resource
	Outputs:	Description - Operation description string if it is found or
				NULL otherwise.
	Returns:	TRUE if successfully retrieves the string and FALSE othersise.
	Purpose:	Depending upon the document Mode, (i.e Frame or Layer),
				this function will obtain the String resource describing an operation. 
				String resources may have one or more text descriptions in them, therefore,
				a TextFlag can be used to identify the the appropriate text required.
	
********************************************************************************************/

BOOL FrameInFrontOpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)
{	
	// Assume we are in layer mode.
	BOOL LayerMode = TRUE;
	TCHAR* ok;
	
	//Determine the document mode.
	Document* pDoc = Document::GetSelected();

	// Ensure a valid ptr.
	if(pDoc)
	{
		// Get a ptr to the selected spread
		Spread* pSpread = pDoc->GetSelectedSpread();
	
		// Ensure a valid spread ptr.
		if(pSpread)
		{

			// Are there any frame layers?
			Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

			//If a frame layer exists, then this is an animation doc.
			if (pFrameLayer)
				LayerMode = FALSE;
		}
	}

	if(LayerMode)
	{
		// In layer mode.
		String_256	ResourceText( _R(IDS_MOVELAYERINFRONT), ModuleID );

		// Explicitly cast return value from GetDescription from a TCHAR* to a String_256
		ok = GetDescription((TCHAR*) ResourceText, WhichText);
	}
	else
	{
		// In frame mode.
		String_256 ResourceText( _R(IDS_MOVEFRAMERINFRONT), ModuleID );

		// Explicitly cast return value from GetDescription from a TCHAR* to a String_256
		ok = GetDescription((TCHAR*) ResourceText, WhichText);
	}
	
	// if description is found then return true else return false
	if (ok)        
	{
		*Description = String_256(ok);
		return TRUE;
	}
	else
		return FALSE;

}


/**************************************************************************************************************************

>	FrameBehindOpDescriptor::FrameBehindOpDescriptor(	const TCHAR* pcszToken, 
														CCRuntimeClass* pClass = CC_RUNTIME_CLASS(PlugInOp),
														pfnGetState gs = PlugInOp::GetState)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/96
	Inputs:		pcszToken		the "OpToken" of the associated Operation
				pClass			the runtime class to use.
				gs				the GetState function to use.
	Outputs:	-
	Returns:	-
	Purpose:	Constructs the new FrameInFrontOpDescriptor
	Errors:		-
	SeeAlso:	-

***************************************************************************************************************************/

FrameBehindOpDescriptor::FrameBehindOpDescriptor(	const TCHAR* pcszToken, 
													CCRuntimeClass* pClass, pfnGetState gs)
  :	OpDescriptor(	0,															// tool ID
					_R(IDS_MOVELAYERBEHIND),										// String resource ID (use same for all)
					pClass,														//CC_RUNTIME_CLASS(PlugInOp), Runtime class
					(TCHAR*) pcszToken,											// OpToken
					gs,															// GetState function
					0,															// help ID
					_R(IDBBL_MOVELAYERBEHIND),										// bubble help
					_R(IDD_ANIMATIONBAR),											// resource ID
					_R(IDC_FRAME_MOVEDOWNAFRAME),									// control ID
					TRUE,														// Recieve system messages
					FALSE,														// Smart duplicate operation
					TRUE,														// Clean operation
					0,															// String for one copy only error
					(GREY_WHEN_NO_SELECTION | GREY_WHEN_NO_CURRENT_DOC)			// Auto state flags
				)
{}

/********************************************************************************************

>	static OpState FrameBehindOpDescriptor::GetState(String_256* pDesc, OpDescriptor* pOpDesc)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		pDesc       --- A pointer to a String. GetState fills this with the
								appropriate details for the conditions arising eg. why
								"Previous Zoom" is greyed out.
				pOpDesc		--- A pointer to the OpDescriptor whose state is being
								queried.
	Returns:	An OpState containing the flags that show what is valid.
	Purpose:	Returns the state that this operation should appear in the menus 
				or as a buttom, for example - greyed out, or ticked.

********************************************************************************************/

OpState FrameBehindOpDescriptor::GetState(String_256*, OpDescriptor* pOpDesc)
{
	// At present, this item is always available.
	OpState OpSt;
	return OpSt;
}

/********************************************************************************************

>	virtual void FrameBehindOpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>	(From Mario code in base class.)
	Created:	21/10/97
	Inputs:		TextFlag - Identifies which text string to retrieve from the string resource
	Outputs:	Description - Operation description string if it is found or
				NULL otherwise.
	Returns:	TRUE if successfully retrieves the string and FALSE othersise.
	Purpose:	Depending upon the document Mode, (i.e Frame or Layer),
				this function will obtain the String resource describing an operation. 
				String resources may have one or more text descriptions in them, therefore,
				a TextFlag can be used to identify the the appropriate text required.
	
********************************************************************************************/

BOOL FrameBehindOpDescriptor::GetText(String_256* Description, OpTextFlags WhichText)
{	
	// Assume we are in layer mode.
	BOOL LayerMode = TRUE;
	TCHAR* ok;
	
	//Determine the document mode.
	Document* pDoc = Document::GetSelected();

	// Ensure a valid ptr.
	if(pDoc)
	{
		// Get a ptr to the selected spread
		Spread* pSpread = pDoc->GetSelectedSpread();
	
		// Ensure a valid spread ptr.
		if(pSpread)
		{
			// Are there any frame layers?
			Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

			//If a frame layer exists, then this is an animation doc.
			if (pFrameLayer)
				LayerMode = FALSE;
		}
	}

	if(LayerMode)
	{
		// In layer mode.
		String_256	ResourceText( _R(IDS_MOVELAYERBEHIND), ModuleID );

		// Explicitly cast return value from GetDescription from a TCHAR* to a String_256
		ok = GetDescription((TCHAR*) ResourceText, WhichText);
	}
	else
	{
		// In frame mode.
		String_256 ResourceText( _R(IDS_MOVERFAMEBEHIND), ModuleID );

		// Explicitly cast return value from GetDescription from a TCHAR* to a String_256
		ok = GetDescription((TCHAR*) ResourceText, WhichText);
	}
	
	// if description is found then return true else return false
	if (ok)        
	{
		*Description = String_256(ok);
		return TRUE;
	}
	else
		return FALSE;

}
