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

// Implementation of the following classes:
/*
 	
		SelectionState 
		SelNdRng
		SelNd
*/ 

// Check in comments
/*
*/
      

#include "camtypes.h"
#include "selstate.h"  
#include "ensure.h"  
#include "docview.h"
#include "range.h" 
#include "app.h" 
#include "node.h"                    
#include "ink.h"
#include "blobs.h"
#include "spread.h"
#include "fillattr.h"	// for AttrFillGeometry::LastRenderedMesh
#include "tool.h"
#include "nodetext.h"

DECLARE_SOURCE("$Revision: 662 $");     


CC_IMPLEMENT_MEMDUMP(SelNdRng, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(SelectionState, CC_CLASS_MEMDUMP)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW    
                         
// -----------------------------------------------------------------------------------------                         
// SelectionState methods   
   
/********************************************************************************************

>	SelectionState::SelectionState()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	SelectionState constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
     
SelectionState::SelectionState()
{
	SelNdRngList = NULL; 
	SelNdList = NULL; 
	NumNdRng = 0; 
	NumNd = 0; 
} 

// helper function to determine if two nodes are contiguous. could usefully be moved to Node?
BOOL AreContiguous(Node* left,Node* right)
{
	BOOL contiguous = FALSE ;
	if( right!=NULL && left != NULL)
	{
		contiguous  = (left->FindNextNonHidden() == right);
	}
	return contiguous ;
}

// Records the current selected state 

/********************************************************************************************

>	BOOL SelectionState::Record() 	     	 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if Out of memory
				ERRORIF does not get called because this method is called by operations
				which handle out of memory differently to the rest of the system. 
				
	Purpose:	Traverses the tree and records all selected nodes. 
	Errors:		- 
	SeeAlso:	-

********************************************************************************************/

BOOL SelectionState::Record() 	     	 
{   
	// Make sure that Record has not been called before     
	ENSURE(((SelNdList == NULL) && (SelNdRngList == NULL)), "SelectionState::Record called twice"); 
	
	// Find the current selections 
	SelRange* pSel; 
	pSel = GetApplication()->FindSelection();

	Node* StartRange;

	UINT32 SelNdRngListIndex = 0; 
	UINT32 SelNdListIndex = 0; 

	UINT32 NumSelectedNodes = pSel->Count(); 

	if (NumSelectedNodes != 0)
	{

		// At this point we don't know exactly how much memory to allocate for the SelNdRngList
		// and SelNdList. So we allocate two temporary arrays which are more than big enough to
		// store the selection state. When we have completed recording the selection state into 
		// these temporary arrays, we know how big the SelNdRngList and SelNdList should
		// be. so we can allocate memory for these and then copy the data from the temporary 
		// arrays into them.  

		SelNdRng* SelNdRngListTmp = new SelNdRng[NumSelectedNodes];  
	

		if (SelNdRngListTmp == NULL)
		{
			return FALSE; 
		}
	 
		Node** SelNdListTmp = new Node*[NumSelectedNodes]; 		  

		if (SelNdListTmp == NULL)
		{
			delete[] SelNdRngListTmp; 	// Tidy up 
			return FALSE; 
		}

		// Get the first selected node in the tree
		Node * Current = pSel->FindFirst(); 
		Node* Last; 

		BYTE NumAdjacentSel; 			  // Number of contiguous selected nodes   
	
		#ifdef _DEBUG   
		UINT32 NumSel = 0; 
		#endif
	
		// always use the selection object to determine next node to store... this fixes bug #10775 whereby
		// selected Bevel,Contour & Shadow objects were not being restored on Undo
		while (Current != NULL)
		{   
			// At this point Current will always point to the next selected node which needs 
			// recording.      
			// Only NodeRenderableInk nodes should be selected
			ENSURE(Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)), 
					"A non NodeRenderableInk node is selected"); 
		
			NumAdjacentSel = 1;    
		
			#ifdef _DEBUG   
			NumSel++;  
			#endif
		        
			StartRange = Current; 
			Last = Current;  
			Current = pSel->FindNext(Current); //next from selection
		
			// if we have at least two contiguous nodes, store as a range
			if (Current && AreContiguous(Last, Current) && !IS_A(Last, CaretNode) && !IS_A(Current, CaretNode))
			{
				SelNdRngListTmp[SelNdRngListIndex].FirstNode = StartRange;   
				do
				{
					if (IS_A(Current, CaretNode))
					{
						// Give Caret it's own selection record but don't break the contiguous range
						SelNdListTmp[SelNdListIndex] = Current;
						SelNdListIndex++; 
						#ifdef _DEBUG   
						NumSel++;  
						#endif
					}
					else
					{
						NumAdjacentSel++; 
						#ifdef _DEBUG   
						NumSel++;  
						#endif
					}
					Last = Current; 
					Current = pSel->FindNext(Current);
	                if (Current == NULL)
	                   	break; 
				} while ((AreContiguous(Last,Current)) && (NumAdjacentSel < 255));      
				// Either there are no more contiguous selected nodes or 
				// we have hit the maximum number of selected nodes that a SelNdRng can 
				// represent.  
				SelNdRngListTmp[SelNdRngListIndex].NumSelected = NumAdjacentSel; 
				SelNdRngListIndex++;
			}
			else  // Store node in the SelNdLst 
			{
				ERROR3IF(StartRange==NULL, "Trying to add NULL pointer to SelNdList\n");
				SelNdListTmp[SelNdListIndex] = StartRange; 
				SelNdListIndex++; 
			}    
		}       
		ERROR3IF(NumSel!=NumSelectedNodes,"Incorrect selection state stored!");
	
		NumNd = SelNdListIndex; 
		NumNdRng = SelNdRngListIndex; 

		if (SelNdRngListIndex != 0) 
		{
			// We have created at least one SelNdRange
	 		SelNdRngList = new SelNdRng[NumNdRng];  
			if (SelNdRngList == NULL)			// Out of memory 
			{
				// Delete the two temporary lists
				delete [] SelNdRngListTmp; 
				delete [] SelNdListTmp; 
				return FALSE; 
			}
		
			// Copy the SelNdRngListTmp to the SelNdRngList  
			memcpy(SelNdRngList, SelNdRngListTmp, sizeof(SelNdRng)*NumNdRng); 
		}

		delete[] SelNdRngListTmp; // No longer required
	
	
		if (SelNdListIndex != 0)
		{
			SelNdList = new Node*[NumNd]; 
			if (SelNdList == NULL)				 // Out of memory 
			{
				delete [] SelNdListTmp; 
				if (SelNdRngList != NULL)		 // We allocated the SelNdRng list
				{
					delete [] SelNdRngList; 
				}
				return FALSE; 
			}
			// copy the SelNdListTmp to the SelNdList 
			memcpy(SelNdList, SelNdListTmp, sizeof(Node*)*NumNd); 
		} 

		delete[] SelNdListTmp; // No longer required
	 
		#ifdef _DEBUG   
		//if (IsUserName("Simon"))
			//TRACE( _T("Number of nodes selected = %lu\n"), NumSel); 
		#endif
	}
	return (TRUE);  
}     



/********************************************************************************************

>	void SelectionState::Restore(BOOL RestoreSelBlobs = FALSE, BOOL RemoveBlobs = TRUE) 	     	


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93

	Inputs:		RestoreBlobs: Redraw blobs on the restored selections (Default = FALSE)
							   
				RemoveBlobs:  Remove all blobs, this should be TRUE only when the selection
							  state is in sync with the blobs on the screen, i.e. at the start
							  of an undo/redo.
	Outputs:	-
	Returns:	-
	Purpose:	This function initially removes all node selections and their selection blobs
				It then restores the node selections. If the RestoreSelBlobs flag is TRUE 
				then blobs are drawn for the restored selections. 
		
				Very often it is better to redraw the blobs for the restored selected objects 
				by invalidating the bounding rectangle of the objects and their blobs. For
				this reason the default for RestoreSelBlobs is FALSE. 

	Errors:		-
	SeeAlso:	SelectionState::Record

********************************************************************************************/

void SelectionState::Restore(BOOL RestoreSelBlobs, BOOL RemoveBlobs) 	     	
{
	DeselectAll(RemoveBlobs); // Deselect all selections

	#ifdef _DEBUG
	UINT32 NumRestored = 0; 	 
	#endif

	 
    // ----------------------------------------------------------------------------------
	// Restore all nodes in the SelNdList
	
	UINT32 i; 

	for (i=0; i < NumNd; i++)
	{
		// Only NodeRenderableInk nodes should be selected
		ENSURE(SelNdList[i]->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)), 
			   "Node to be selected is not a NodeRenderableInk");
			    
		// Ensure node to be selected is not already selected                                
		//ENSURE(!(SelNdList[i]->IsSelected()), "Invalid selected node"); 

		// Select the node 
		SelNdList[i]->SetSelected(TRUE); 
		#ifdef _DEBUG
		NumRestored++; 	 
		#endif	
	}
	   
	// ----------------------------------------------------------------------------------
	// Restore all nodes in the SelNdRngList	  

	Node* Current; 
	   
	for (i=0; i < NumNdRng; i++) 
	{  
		// A SelNdRng shold represent at least two contiguous selected nodes
		ENSURE(SelNdRngList[i].NumSelected >= 2, "Number of nodes in SelNdRng < 2");    

		UINT32 NumNodesSelected = 0; 
		Current = SelNdRngList[i].FirstNode; // First node in range 
		
		// Select the next SelNdRng->NumSelected nodes
		do  
		{   
			ENSURE((Current != NULL), "Node in a SelNdRng is NULL, Has tree changed ?" );  
			// Only NodeRenderableInk nodes should be selected   
			
			ENSURE(Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)), 
				   "Selected node is not a NodeRenderableInk"); 

			// [Phil, 11/10/2005] The Current node may be a Caret node
			// because Carets are included in the selection, even when
			// a sub-selection of characters is present, so that attributes
			// optimise correctly within the text story
			// However, the Record function treats them as single selected
			// nodes, not part of a contiguous run of selected nodes to
			// allow for them moving around...
			// So we should ignore carets here
			if (!IS_A(Current, CaretNode))
			{
				// Ensure SelNode not already selected  
				ENSURE(!(Current->IsSelected()), "Invalid selected node");

				Current->SetSelected(TRUE); 	// Set the nodes selected flag
				NumNodesSelected++;
#ifdef _DEBUG
				NumRestored++; 	 
#endif	
			}
			Current = Current->FindNextNonHidden();  
		}
		while (NumNodesSelected != SelNdRngList[i].NumSelected); 
	}

#if !defined(EXCLUDE_FROM_RALPH)
	// Only restore the blobs if any selections have been restored
	if (((NumNdRng != 0) || (NumNd !=0)) && RestoreSelBlobs)	// We need to restore the selection blobs 
	{
		// Find the current selections 
		SelRange* pSel; 
		pSel = GetApplication()->FindSelection();
 
		Current = pSel->FindFirst(); // The first selected node 

		Spread *pSpread = NULL;
		if (Current != NULL)
			pSpread = Current->FindParentSpread();

//		ENSURE(pSpread != NULL, "First selected node does not have a parent spread");
		// It's a legal state to not find a selected node - the layers may all be locked.
		if (pSpread == NULL)
		{
			AttrFillGeometry::LastRenderedMesh = NULL;
			return;
		}

		// Go get the blob manager
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		ENSURE( BlobMgr!=NULL, "Blob Manger was not there when we needed him!");
		if (BlobMgr==NULL)
			return;

	    Current = pSel->FindFirst();   // The first selected node 

		while (Current != NULL)
		{
			ENSURE(Current->IsSelected(), "Node not selected"); 
			ENSURE(Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)), 
					"Selected Node not a NodeRenderable"); 

			// Tell the node to add selection blobs
			BlobMgr->RenderMyBlobsOn(NULL, pSpread, (NodeRenderable*)Current);
			
			Current = pSel->FindNext(Current); 	// Get next selected node 
		}

		Tool* pTool = Tool::GetCurrent();
			
		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSpread,NULL);

		AttrFillGeometry::LastRenderedMesh = NULL;
	}
#endif
	
	#ifdef _DEBUG
	//if (IsUserName("Simon"))
	//	TRACE( _T(" Num Restored = %lu\n"), NumRestored);  
	#endif	
	 
}  

/********************************************************************************************

>	void SelectionState::DeselectAll(BOOL RenderBlobs = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Inputs:		RenderBlobs: Should blobs be rendered?

	Purpose:	This is a brute force method of deselecting all the nodes in the tree.
				It scans ALL nodes in the selected spread, deselecting them all via
				Node::ClearSelection().

********************************************************************************************/

void SelectionState::DeselectAll(BOOL RenderBlobs)
{
	// Find the selected objects in the tree;
	SelRange* Selected = GetApplication()->FindSelection();
	ERROR3IF( Selected==NULL, "Selection object is null in DeselectAll()");

	// Get the selected spread
 	Spread* pSpread = Document::GetSelectedSpread();
	ERROR3IF(pSpread == NULL,"NULL selected spread");

	// Make sure that we have a spread and a selection
	if (pSpread == NULL || Selected == NULL)
		return;

	// Find first selected node

#if !defined(EXCLUDE_FROM_RALPH)
	Node* pFirstSelectedNode = Selected->FindFirst();
	// If there is a selection, EOR blobs off, deselect nodes, and inform everybody
	if (pFirstSelectedNode != NULL && RenderBlobs)
	{
		// Go though and render all the EOR blobs off the screen

		// Find the Blob Manager
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		ENSURE( BlobMgr!=NULL, "Blob Manager unexpectedly not there.");

		// Render all the blobs
		BlobMgr->RenderOff(NULL, pFirstSelectedNode->FindParentSpread());

		Tool* pTool = Tool::GetCurrent();
			
		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSpread,NULL);
	}
#endif

	DeselectAll(pSpread->FindFirstChild());

	// Selection cache is no longer valid, so update and tell everyone that it has changed

	// *Note*, This used to be 'Selected->Update(TRUE)', but I (Will) removed the TRUE, so
	// that a message is NOT broadcast.  This should only be called from an operation,
	// and the op will send a message when it ends.

	Selected->Update();
}


/********************************************************************************************

>	void SelectionState::DeselectAll(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Inputs:		pNode = ptr first node in sibling list to deselect

	Purpose:	This deselects the given node using Node::ClearSelection().
				It then calls ClearSelection() on the node's children, and all the 
				following sibling nodes, and their children, etc.

********************************************************************************************/

void SelectionState::DeselectAll(Node* pNode)
{
	while (pNode != NULL)
	{
		DeselectAll(pNode->FindFirstChild());
		pNode->ClearSelection();
		pNode = pNode->FindNext();
	}
}

/********************************************************************************************

>	UINT32 SelectionState::GetSize()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/94
	Inputs:		-
	Outputs:	-
	Returns:	The size of the selection state in bytes 
	Purpose:	For finding the size of the selection state 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 SelectionState::GetSize()
{
	return (sizeof(SelectionState) + (sizeof(SelNdRng)*NumNdRng) + (sizeof(Node*)*NumNd)); 
}  

/********************************************************************************************

>	SelectionState::~SelectionState(); 	 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	SelectionState destructor. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

SelectionState::~SelectionState() 	 
{    
	// delete selection lists
  	if (SelNdRngList != NULL)
	{
  		delete [] SelNdRngList; 
	}
	if (SelNdList != NULL)
	{
		delete [] SelNdList; 	
	}
}


/********************************************************************************************

>	BOOL SelectionState::operator==(SelectionState& SelState)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		SelState: The selection state to compare with
	Outputs:	-
	Returns:	TRUE if both SelectionStates contain the same set of objects
	Purpose:	Compares this SelectionState with SelState. If they contain the same objects 
				then TRUE is returned, else FALSE. 
				
				Notes:
				
				This is important: 

				We use object TAGs for the comparison
				
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectionState::operator==(SelectionState& SelState)
{
	if ((NumNd != SelState.NumNd) || (NumNdRng != SelState.NumNdRng))
    {
		// Selection states contain different numbers of objects
		return FALSE; 
	}

    // ----------------------------------------------------------------------------------
	// Check that the SelNdLists contain the same objects 
	
	UINT32 i; 

	for (i=0; i< NumNd; i++)
	{
		if (SelNdList[i]->GetTag() != SelState.SelNdList[i]->GetTag())
		{
			return FALSE; 
		} 
	}
	   
	// ----------------------------------------------------------------------------------
	// Check that the SelNdRngLists contain the same objects 	  
	   
	for (i=0; i < NumNdRng; i++) 
	{  
		if (SelNdRngList[i].NumSelected != SelState.SelNdRngList[i].NumSelected)
		{
			return FALSE; 
		}

		if (SelNdRngList[i].FirstNode != SelState.SelNdRngList[i].FirstNode)
		{
			return FALSE; 
		}
	}
	return TRUE; // The two SelectionsStates are equal
}


/********************************************************************************************

>	SelNdRng* SelectionState::GetSelectionList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		-
	Returns:    The array of selected ranges

	Purpose:	Access function gets you a pointer to the array of selected ranges. Probably
				not the safest thing in the world to be using if you want to make changes to your
				stored selection, but if you just want to find out what was there it can be useful.


********************************************************************************************/

SelNdRng* SelectionState::GetSelectionList()
{
	return SelNdRngList;
}


/********************************************************************************************

>	UINT32 SelectionState::GetNumRanges()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		-
	Returns:    The number of selected ranges in SelNdRngList
	Purpose:	as above
********************************************************************************************/

UINT32 SelectionState::GetNumRanges()
{
	return NumNdRng;
}


/********************************************************************************************

>	Node** SelectionState::GetNodeList()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		-
	Returns:    pointer to the array of stored nodes, 

	Purpose:	Access function gets you a pointer to the array of selected nodes. 

********************************************************************************************/

Node** SelectionState::GetNodeList()
{
	return SelNdList;
}



/********************************************************************************************

>	UINT32 SelectionState::GetNumNodes()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		-
	Returns:    The number of selected nodes in SelNdList
	Purpose:	as above
********************************************************************************************/

UINT32 SelectionState::GetNumNodes()
{
	return NumNd;
}
