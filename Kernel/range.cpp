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
    
/* Implementation of class Range */     
    
/*
*/ 
 
#include "camtypes.h" 

#include "app.h"
#include "document.h"
#include "docmsgs.h"
#include "docview.h"
#include "ensure.h" 
#include "node.h"
#include "range.h"   
#include "rndrgn.h"
//#include "simon.h"
#include "strings.h" 
#include "ink.h"
#include "spread.h"
//#include "dlgbar.h"
#include "group.h"
#include "sprdmsg.h"
#include "layer.h"
#include "fillattr.h"
#include "attrmgr.h"
//#include "peter.h"
#include "nodetext.h"
#include "undoop.h"
#include "nodetxtl.h"
#include "nodetxts.h"
#include "nodecont.h"
#include "nodeshad.h"
#include "objreg.h"
#include "attrmap.h"
#include "toolmsg.h"
#include "comattrmsg.h"
#include "qualattr.h"
#include "ngcore.h"		// NameGallery, for stretching functionality
#include "objchge.h"	// for ues of ObjChangeParam
//#include "scrvw.h"
//#include "will2.h"
//#include "justin2.h"
//#include "resimmap.h"	//For _R(IDS_SINGLELAYER_INSIDE)
//#include "bmpsdlgr.h"	// _R(IDS_ON_FRAME_OUTSIDE_NOREF)
#include "effects_stack.h"
#include "bars.h"

DECLARE_SOURCE( "$Revision$" );    

CC_IMPLEMENT_DYNAMIC(Range, CCObject)   
CC_IMPLEMENT_DYNAMIC(NodeListItem, ListItem)

CC_IMPLEMENT_DYNAMIC(SelRange, Range)   
CC_IMPLEMENT_DYNAMIC(SelChangingMsg, Msg)

CC_IMPLEMENT_DYNAMIC(ListRange, Range)   

CC_IMPLEMENT_DYNAMIC(SelRangeMessageHandler, MessageHandler)

CC_IMPLEMENT_DYNAMIC(CommonAttrsChangedMsg, Msg);


// This line MUST occur after all CC_IMPLEMENT lines
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// Enable Fast XOR drag blob rendering - see below
// Set this value to 0 (FALSE) to disable, or 1 (TRUE) to enable fast XORing
#define ENABLE_FAST_XOR 1


// Define _DEBUG_LISTCHECKS to enable list checking in ListRange
#ifdef _DEBUG
//	#define _DEBUG_LISTCHECKS
#endif


/*****************************************************************************************

>   RangeControl::RangeControl(	BOOL Sel				= FALSE,
								BOOL Unsel				= FALSE,
								BOOL Cross				= FALSE,
								BOOL IgLock				= FALSE,
								BOOL Rendr				= FALSE,
								BOOL IgInvisible		= FALSE,
								BOOL SibsOnly			= FALSE,
								BOOL Promot				= FALSE )
    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	26/01/95

    Inputs:		-
    Outputs:	-
    Returns:	-
	Purpose:	RangeControl constructor. Sets the flags to safe defaults!
				See RangeControl definition for details of flags.

	Errors:		-

******************************************************************************************/

RangeControl::RangeControl( BOOL Sel, BOOL Unsel, BOOL Cross,
							BOOL IgLock, BOOL Rendr, BOOL IgInvisible, BOOL SibsOnly,
							BOOL Promot )
{
	Selected = Sel;
	Unselected = Unsel;
	CrossLayer = Cross;
	IgnoreLockedLayers = IgLock;
	IgnoreNoneRenderable = Rendr;
	IgnoreInvisibleLayers = IgInvisible;
	SiblingsOnly = SibsOnly;
	PromoteToParent = Promot;
}




/********************************************************************************************

>	Range::Range(Node* First, Node* Last, RangeControl RangeControlFlgs)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/93
	Inputs:		RangeControlFlags: Specifies the selected status of the nodes to be included
								   in the range.
								   
					               If RangeControlFlgs.Selected = TRUE
						 				All nodes with a selected status are included as 
						 				members of the range. 
						 				
						 		   If RangeControlFlgs.Unselected = TRUE
						 		   		All nodes with an unselected status are included as
						 		   		members in the range. 
						 		   		
						 		   If both these flags are TRUE then all nodes from First to Last
						 		   are included in the range. 

								   If RangeControlFlgs.CrossLayer = TRUE
										The range may cross several layers; when the end of a layer
										is reached, the search will continue at the start of the
										following layer.

						 		   If RangeControlFlgs.IgnoreLockedLayers = TRUE
						 		   		locked layers are included in the search.
						 		   		Defaults to FALSE so locked layers ignored.
								   
						 		   If RangeControlFlgs.IgnoreNoneRenderable = TRUE
						 		   		non-renderable nodes are included in the search.
						 		   		Defaults to FALSE so non-redenderable nodes ignored.
								   
						 		   If RangeControlFlgs.IgnoreInvisibleLayers = TRUE
						 		   		Invisable layers are included in the search.
						 		   		Defaults to FALSE so invisible layers ignored.
								   
				First:	The node from which to commence searching for members of a range.  
				Last:	The final node to search (must be a right sibling of First, or NULL). 
						NULL specifies that all nodes from First to the end of First's sibling 
						list (or end of the tree, if CrossLayer is TRUE) are to be searched for
						members of the range. 
							
								    
	Outputs:	-
	Returns:	-
	Purpose:	The purpose of this function is to create a node range. 
	Errors:		-

********************************************************************************************/
  
Range::Range(Node* First, Node* Last, RangeControl RangeControlFlgs)
{
	// When the First node in a range is NULL the range is assumed to lie directly under
	// the surface of layers...
/*
	if (First!=NULL)
	{
		ERROR3IF( RangeControlFlgs.Unselected
					&& !First->FindParent()->IsLayer()
					&& First != Last,
					"Attempt to create an illegal range!"
				 );
	}
*/

	RangeControlFlags = RangeControlFlgs; 
	FirstNode = First; 
	LastNode = Last;
	pCommonAttribCache = NULL;			   // The common attribute cache only comes into
										   // existance if the FindCommonAttribute functions
										   // are called on the range.
 
	// Initialise the XOR rendering stuff
	NodeToRender = NULL;
	ResetXOROutlineRenderer(TRUE);
} 




/********************************************************************************************

>	Range::Range()


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates an uninitialised range 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Range::Range()
{
	FirstNode = LastNode = NULL;
	pCommonAttribCache = NULL;			   // The common attribute cache only comes into
										   // existance if the FindCommonAttribute functions
}




Range::~Range()
{
	// Destroy the CommonAttribSet if one exists
	if (pCommonAttribCache)
	{
		delete pCommonAttribCache; 
	}
}




/********************************************************************************************

>	Range::Range(const Range& Range); 	

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/94
	Inputs:		Rng: 	The Range to copy 
	Outputs:	-
	Returns:	-
	Purpose:	Range copy constructor 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Range::Range(Range& Rng)
{
	// If the range is a sel range we must be sure that the range has been cached 
	Rng.FreshenCache();

	RangeControlFlags = Rng.RangeControlFlags;  
	FirstNode = Rng.FirstNode; 
	LastNode = Rng.LastNode;
	pCommonAttribCache = NULL; 		
}	   




/********************************************************************************************

>	Range& Range::operator=(Range& Range) 	           

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/3/94
	Inputs:		Range: The range to copy 
	Outputs:	
	Returns:	The Range
	Purpose:	Range = operator
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Range& Range::operator=(Range& Rng)
{
	// If the range is a sel range we must be sure that the range has been cached 
	Rng.FreshenCache(); 

	RangeControlFlags = Rng.RangeControlFlags;  
	FirstNode = Rng.FirstNode; 
	LastNode = Rng.LastNode; 
	pCommonAttribCache = NULL; 		
	return *this; 		
}




/********************************************************************************************

>	void Range::SetRangeControl(RangeControl RangeControlFlgs)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/93 
	
	Inputs:		RangeControlFlags: Specifies the selected status of the nodes to be included
								   in the range.
	Outputs:	-
	Returns:	-
	Purpose:	To set the range control for the range.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
							
void Range::SetRangeControl(RangeControl RangeControlFlgs)
{
	RangeControlFlags = RangeControlFlgs;
}   




/*********************************************************************************************

>    Node* Range::FindFirst(BOOL AndChildren = FALSE)

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (Changed by Phil, 30/11/94)
     Created:   25/6/93
     
     Returns:   If the range contains any members then
					A pointer to the first node in the range is returned
				Else
					NULL is returned 
			
     Purpose:   The purpose of this function is to find the first node in a range. 

     SeeAlso:   Range::FindNext    
     
     Errors:    An assertion failure will occur if:

     			There are no more nodes to search and the last node to search was not found.

**********************************************************************************************/
                                    
Node* Range::FindFirst(BOOL AndChildren)
{
	// Preconditions...
	ERROR2IF(this==NULL,NULL,"FindFirst called on NULL range");

	// Locals...
	Node* pNode = FirstNode;


// **** !!!! BODGE - Traps bug in message broadcast from document.cpp to try to
// keep the program stable.
	if (Document::GetSelectedSpread() != NULL)
	{
		if (!Document::SpreadBelongsToDoc(Document::GetSelected(),
											Document::GetSelectedSpread()))
		{
			TRACE( _T("SelRange: Selected Spread is NOT in the Selected Document!\n"));
			return(NULL);
		}
	}
// **** !!!! BODGE

	// Implementation...
	// If First is NULL then the caller expects to get a range of nodes
	// in the child lists of layers. So find the first layer on the
	// current spread.
	if (pNode == NULL)
	{
		Spread* pSelectedSpread = Document::GetSelectedSpread();
		if (pSelectedSpread==NULL)
		{
			// The range does not have a FirstNode. Normally it would find one
			// automatically based on the SelectedSpread but there is no SelectedSpread
			// so we have no choice but to return an empty range to the caller...
			TRACEUSER( "Phil", _T("Warning! Range is NULL because there's no selected spread\n"));
			return NULL;
		}
		
		// Oh well, we will use the first layer instead
		pNode = pSelectedSpread->FindFirstLayer();

		// If there is no layer, then give up and return NULL
		if (pNode==NULL)
			return NULL;
	}

	// The first node might be a valid member of the range so test for that...
	// (Set the special FirstNode flag TRUE so that the logic treats this node is
	//	if it hasn't yet been met in the scan.)
	pNode = SmartFindNext(pNode, AndChildren, TRUE);
	return pNode;
}                       


     

/*********************************************************************************************

>    Node* Range::FindNext(Node* Prev, BOOL AndChildren = FALSE)
     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>  (Changed by Phil, 30/11/94)
     Created:   25/6/93
     Inputs: 	Prev: The previous node in the range (usually returned from a 
     				   Range::FindFirst, or a previous Range::FindNext call). 
     Outputs:    - 
     Returns:   If the range contains any more members then
					A pointer to the next node in the range is returned
				Else
					NULL is returned 
     Purpose:   The purpose of this function is to find the next node in the range after Prev 
     SeeAlso:   Range::FindFirst    
     Errors:    An assertion failure will occur if:
     			There are no more nodes to search and the last node to search was not found.

**********************************************************************************************/

Node* Range::FindNext(Node* pPrevious, BOOL AndChildren)
{
	// Preconditions
	// No need to check that "this" is NULL because that's already been done
	// in FindFirst.
	ERROR2IF(pPrevious == NULL, NULL, "NULL pointer passed to Range::FindNext");

	Node* pNode = SmartFindNext(pPrevious, AndChildren);

	// DMc
	// if the output is the same as the input, and promote to parent is set,
	// the this should never happen !
	if (pNode == pPrevious && RangeControlFlags.PromoteToParent)
	{
		return NULL;
	}
		
	return pNode;
}



/*********************************************************************************************

>    Node* Range::FindPrev(Node* pNode, BOOL AndChildren = FALSE)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/01/95
     Inputs: 	pNode: The node which you want the previous one in the range to... 
     Outputs:    - 
     Returns:   If the range contains any members before the input node then
					A pointer to the prev node in the range is returned
				Else
					NULL is returned (Note NULL returned if input node was not in the range)
     Purpose:   The purpose of this function is to find the previous node in the range before
     			the input node.
				NOTE! THIS ROUTINE IS SLOW! It scans through the range in forwards order
				until it meets the input node, then returns the previous node to that!!!
     SeeAlso:   Range::FindFirst; Range::FindNext    
     Errors:    -

**********************************************************************************************/

Node* Range::FindPrev(Node* pNode, BOOL AndChildren)
{
	// Preconditions
	// No need to check that "this" is NULL because that's already been done
	// in FindFirst.
	ERROR2IF(pNode == NULL, NULL, "NULL pointer passed to Range::FindNext");

	Node* pPrev = NULL;
	Node* pCurrent = FindFirst(AndChildren);
	while (pCurrent)								// While there's a node to check
	{
		if (pCurrent == pNode)						// See if it's the one we're looking for
			return pPrev;							// If it is return the previous node!

		pPrev = pCurrent;							// Else, remember this node
		pCurrent = FindNext(pCurrent, AndChildren);	// and find the next one in the range
	}

	return NULL;
}



/*********************************************************************************************

>    Node* Range::FindLast()

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   25/6/93
     
     Returns:   If the range contains any members then
					A pointer to the last node in the range is returned
				Else
					NULL is returned 
			
     Purpose:   The purpose of this function is to find the last node in a range. 
	 			If the range was constructed with a NULL last node specifier then
				the range is scanned until the last node is found. If a non NULL last node
				was specified however the value of last is simply returned. It's existance
				is not verified !. 

     SeeAlso:   Range::FindFirst
     			Range::FindNext    
     
     Errors:    An assertion failure will occur if:

     			There are no more nodes to search and the last node to search was not found.

**********************************************************************************************/

Node* Range::FindLast()
{
	if (LastNode == NULL)
	{
		// We need to search for the last node in the range
		Node* Scout = FindFirst();
		Node* n = NULL; 
		while (Scout != NULL)
		{
			n = Scout;
			Scout = FindNext(Scout); // Get the next value in the range
		}
		return n; // If the range is empty NULL will be returned
	}
	else
	{
		// get the parent of the last node (if necessary to promote)
		Node * pContext = LastNode;

		if (pContext != NULL)
		{
			Node *pParent = pContext->FindParent();
		
			if (RangeControlFlags.PromoteToParent)
			{
				while (pParent)
				{
					if (pParent->ShouldITransformWithChildren())
					{
						pContext = pParent;
					}
				
					pParent = pParent->FindParent();
				}
			}
		}

		return pContext; 
	}
}




/******************************************************************************************

>	void Range::Update(BOOL TellWorld = FALSE)

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   03/10/94	(Was an inline function prior to this)

     Inputs: 	BOOL TellWorld: When TRUE the function will inform the world that the 
	 			selection has changed. Normally	the selection is changed in an operation
				and it is unneccesary to broadcast a SelChangingMsg at this point because
				this will occur at the end of the operation. However If the selection is
				changed outside of an operation then the flag should be set to TRUE.  
     Outputs:   -
     Returns:   -

     Purpose:   To inform the SelRange that the selection has changed
				This invalidates the SelRange's selection-info cache so it will be
				recached when info is next requested.

******************************************************************************************/

void  Range::Update(BOOL TellWorld)
{
	Camelot.FindSelection()->Update(TellWorld); 	
}	




/*********************************************************************************************

>    UINT32 Range::Count()
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/2/94
     Inputs: 	-
     Outputs:   -
     Returns:   The number of nodes described by the Range.
     Purpose:   To count the number of nodes in a range. Currently this routine always scans
     			the range to count the number of nodes. In some future incarnation it may 
     			keep the count cached for quicker access.
     SeeAlso:   Range::FindFirst
     SeeAlso:   Node::FindNextInRange
     Errors:    -

**********************************************************************************************/

UINT32 Range::Count()
{
	UINT32 count = 0;
	Node* pNode;

	pNode = FindFirst();
	while (pNode)
	{
		count++;
		pNode = FindNext(pNode);
	}

	return(count); 
}


/*********************************************************************************************

>    BOOL Range::Contains(Node* pLookFor, BOOL bAndChildren)

     Author:    Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
     Created:   3/5/95
     Inputs: 	pLookFor = a pointer to a node to look for.
				bAndChildren - Test for node being in subtree of member of range
     Outputs:   -
     Returns:   TRUE if the range contains this particular node
     			FALSE if the node is not held within this range
     Purpose:   To determin whether a particular node is actually in the range list.
     Errors:    -

**********************************************************************************************/

BOOL Range::Contains(Node* pLookFor, BOOL bAndChildren)
{
	Node* pNode=FindFirst();
	while (pNode)
	{
		if (pNode==pLookFor)
			return TRUE;

		if (bAndChildren && pNode->IsNodeInSubtree(pLookFor))
			return TRUE;

		pNode=FindNext(pNode);
	}
	return FALSE;
}



/*********************************************************************************************

>	BOOL Range::ContainsSelectInside()
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if Selection inside occurs in this range one or more times.
				FALSE otherwise.
				(Also returns FALSE when the Range is empty)
	Purpose:	To find out whether Select-inside is going on in this range. I.e. are any
				members of this range children of nodes which would normally be selected.
	Errors:		-

**********************************************************************************************/

BOOL Range::ContainsSelectInside()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Preconditions
	ERROR2IF(this==NULL,FALSE,"ContainsSelectInside called on NULL pointer");

	// Implementation
	Node* pNodeInRange = FindFirst();
	Node* pParent;
	while (pNodeInRange)								// While there's a node in the range
	{
		pParent = pNodeInRange->FindParent();			// Look at it's parent node
		
		// DMc - added promotehittestonchildrentome test to eliminate the 'inside' setting
		// for nodes should as shadows, contours, bevels etc
		while (pParent)
		{
			if (pParent->IsAnObject() && pParent->IsParentOfSelected() &&
				pParent->PromoteHitTestOnChildrenToMe())	// If that's marked parent of sel
				return TRUE;								// Then this node is "inside" so return TRUE

			pParent = pParent->FindParent();
		}

		pNodeInRange = FindNext(pNodeInRange);			// Else, keep scanning members of range
	}
#endif
	return FALSE;										// If no more in range, none were "inside"
}




/*********************************************************************************************

>    Node* Range::SmartFindNext(Node* pContext, BOOL AndChildren = FALSE, BOOL FirstCall = FALSE) const
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   24/11/94
     Inputs:	pContext	Pointer to node in range to find next range member from
	 						NOTE! pContext MUST be in the range!!!
				AndChildren	Flag specifying whether to return children of nodes in
							the range.
     			Last		A pointer to the final node to search or NULL if all possible
     						members of the range are to be searched.   
	            Flags  		Define the nodes which are to be members of the range 
		      		 Flags.Selected   : Selected nodes in the range
		     	     Flags.Unselected : Unselected nodes in the range
					 Flags.CrossLayer : Crossing into the next layer is allowed
		        If Flags.Selected and Flags.Unselected are both set, then all nodes from
		        this node to the Last node (or end of the layer, whichever comes first,
		        if CrossLayer is clear) are included in the range.
     Returns:   If the range contains any members following pContext then
					A pointer to the next node in the range is returned
				Else
					A NULL is returned 
		        If no flags are set then there can be no members in the range
			    and NULL is returned.
     Purpose:   This function looks for the next node in a range following this node
     			and ending at the node specified by Last (or the last node in this layer
     			if the CrossLayer flag is clear).
     			Last must be a right sibling of this node, or NULL if all siblings are
     			to be searched.     			
     SeeAlso:   Node::FindFirstInRange; Node::FindNextInRange    
     Errors:    There are no more nodes to search and Last was not found
	 Scope:		Protected

**********************************************************************************************/
/*	Technical Notes:

	This routine is just a souped up depth-first traversal of the whole tree with subtrees
	carefully pruned off to make the routine efficient.

**********************************************************************************************/
Node* Range::SmartFindNext(Node* pContext, BOOL AndChildren, BOOL FirstCall) const
{
	// Preconditions
	ERROR2IF(this==NULL,NULL,"SmartFindNext called on NULL range");
	ERROR2IF(pContext==NULL,NULL,"SmartFindNext not given a context node to start from");
	ERROR2IF(RangeControlFlags.Unselected && AndChildren,NULL,"Range can't cope with unsel and children");
	ERROR2IF(pContext->IsPaper() && !pContext->IsLayer(),NULL,"SmartFindNext called above layers surface");

	// If the PromoteToParent flag is set then the context node might be a "promoted parent"
	if (RangeControlFlags.PromoteToParent && !FirstCall && !AndChildren)
	{
		// Using PromoteToParent, it is possible for the context node to jump *above* the
		// last node in the Range, causing us to iterate outside of it. We must therefore
		// check whether the context node is a parent of the last node, in which case
		// we have finished iteration.
		if (pContext->IsNodeInSubtree(LastNode))
			return NULL;

		// If the context node's parents wish to do so, then switch them in instead of it.
		// Go up the tree until this is no longer the case.
		Node* pParent = pContext->FindParent();
		while (pParent != NULL && pParent->ShouldITransformWithChildren())
		{
			pContext = pParent;
			pParent = pContext->FindParent();
		}
	}

	// Implementation
	// Loop, looking for suitable nodes and testing whether they're in the range or not.
	// Traverses the tree in basically depth-first order...
	do
	{
		// If context node from previous findnext was last node then there can be no more...
		if (!FirstCall && LastNode!=NULL && pContext==LastNode) return NULL;

		// Do the normal depth-first thing; assume subtree we're given has already been
		// met and so try to find lowest leaf of next subtree...
		// (If this is the first call, from FindFirst, then treat the node we've got
		//	as if it hasn't been scanned yet. 'Cos it hasn't!)
		if (pContext->FindNext() || FirstCall)
		{
			// Here, either have a sibling or FirstCall is TRUE
			// So, if firstcall is NOT TRUE, must be here due to sibling...
			if (!FirstCall)
				pContext = pContext->FindNext();					// Find top of next subtree
			FirstCall = FALSE;

			// In a normal depth-first traversal of the tree we would find the deepest first child
			// in the subtree. However, by choosing not to go down certain child links
			// we can omit whole subtrees from the scan.
			// The following tests optimise away much of the work of a true depth-first
			// search by pruning subtrees that don't need to be traversed.

			//---------------------------------------------------------------------------//
			// If above layers, skip on down towards one...
			while (pContext->FindFirstChild() && !pContext->IsLayer() && pContext->IsPaper())
				pContext = pContext->FindFirstChild();
			// If we've not made it to a layer and we're still in paper then prune whatever
			// subtree we have because this routine is interested in children of layers and
			// their subtrees.
			if (pContext->IsPaper() && !pContext->IsLayer())
				goto PruneSubtree;

			//---------------------------------------------------------------------------//
			// We know now we're either at a layer node or underneath one...
			// If this layer has children go down to that level because that surface has
			// a special meaning to this routine. Most ranges are found in the child lists
			// of Layers.
			if (pContext->IsLayer())
			{
				// Don't allow Ranges into invisible layers at all!
				// New range control flag allows this to be overriden, defaults to FALSE
				if ( !RangeControlFlags.IgnoreInvisibleLayers && !((Layer*)pContext)->IsVisible() )
					goto PruneSubtree;

				// Don't allow SelRanges into locked layers at all!
				// New range control flag allows this to be overriden, defaults to FALSE
				if ( RangeControlFlags.IgnoreLockedLayers && ((Layer*)pContext)->IsLocked() )
					goto PruneSubtree;

				// If layer has children go find first child and continue pruning tests
				if (pContext->FindFirstChild())
					pContext = pContext->FindFirstChild();
				// Otherwise don't try to process empty layers at all!
				else
					goto PruneSubtree;
			}

			//---------------------------------------------------------------------------//
			// Descend subtree further if we come across select-inside (flagged by ParentOfSelected)
			// If looking for selected nodes (and not unselected nodes - Ed 1/5/95)
			// AND this node is the parent of selected nodes
			// Then go down this subtree as far as the parent of selected
			// flags last out...
			if (pContext->IsParentOfSelected() && RangeControlFlags.Selected && !RangeControlFlags.Unselected)
			{
				while (pContext->IsParentOfSelected() && pContext->FindFirstChild())
				{
					pContext = pContext->FindFirstChild();
				}
				ERROR3IF(pContext->IsParentOfSelected(),"Parent of selected has no children!");
			}

// Logically, if promotetoparent is not set we shouldn't return any node which would
// be the target of promotetoparent activities...
if (!RangeControlFlags.PromoteToParent && RangeControlFlags.Selected && RangeControlFlags.Unselected)
{
	while (pContext->ShouldITransformWithChildren() && pContext->FindFirstChild())
	{
		pContext = pContext->FindFirstChild();
	}
	ERROR3IF(pContext->ShouldITransformWithChildren(), "Parent of selected has no children!");
}

			//---------------------------------------------------------------------------//
			// Finally, there's no choice any more, we've got to descend into the subtree IF
			// the AndChildren flag is set and the node is a Primary member of the range
			// Then we are either already scanning a subtree and so we must continue
			// or else we're starting to scan a subtree because it meets the range
			// criteria and the range specifies that all child nodes of primary nodes
			// should be returned.
			// So, look for lowest leaf of subtree...
			if (!RangeControlFlags.SiblingsOnly && AndChildren)
			{
				Node* pChild = pContext->FindFirstChild();
				while (pChild)
				{
					if (InRange(pChild, AndChildren))
					{
						pContext=pChild;
						pChild=pChild->FindFirstChild();
					}
					else
						pChild=pChild->FindNext();
				}
			}

			//---------------------------------------------------------------------------//
			// If control gets here then the root of the "next" subtree is being returned for
			// consideration because we have judged that its subtree doesn't need scanning!
PruneSubtree:;

		}
		else
		{
			// No next subtree, so go up to parent...
			// If no parent then we must have scanned the entire tree (by definition of
			// depth-first traversal) so return NULL to caller.
			pContext = pContext->FindParent();

			// If we're not promoting to parent and we find our parent is the type of object we would normally
			// promote to, DON'T return it, keep going up!
			// (It doesn't make sense to return a node and it's parent controller)
			// This change made so that a shadowed node on the clipboard will return a set of sensible
			// common attributes when FindCommonAttributes is called on the clipboard range.
			// Test by using Paste Attributes
// NOPE, THIS DOESN'T WORK!
//			while (pContext && !AndChildren && !RangeControlFlags.PromoteToParent && pContext->ShouldITransformWithChildren())
//			{
//				pContext = pContext->FindParent();
//			}

			if (pContext==NULL)
				return NULL;

			// Ranges cannot exist across spreads so if we reach a Spread node we can stop...
			if (pContext->IsSpread())
				return NULL;

			if (pContext->IsLayer() && !RangeControlFlags.CrossLayer)
				// Can't cross layers so signal end of Range...
				return NULL;
		}
	}
	while (pContext!=NULL && (pContext->IsLayer() || pContext->IsPaper() || !InRange(pContext, AndChildren)));

	// Finally, check for parents wanting to be included instead of the children
	if (pContext != NULL && RangeControlFlags.PromoteToParent && !AndChildren)
	{
		Node *pParent = pContext->FindParent();
		while (pParent != NULL && pParent->ShouldITransformWithChildren())
		{
			pContext = pParent;
			pParent = pContext->FindParent();
		}
	}

	return pContext;
}




/******************************************************************************************

>	BOOL Range::InRange(Node* pNode, BOOL AndChildren) const

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   24/11/94
	 Rewritten:	08/12/2004
     Inputs: 	pNode			Pointer to node to test for inclusion in the range
	 			AndChildren		Children of the primary range are allowed to be returned
     Outputs:	-
     Returns:   TRUE if node is in this range (ignoring CrossLayer flag)
	 			FALSE if not
     Purpose:   Private helper function for SmartFindNext.
				Tests whether the presented node is in the range according to the
				RangeControlFlags and two other locally presented flags.

				Version 2, 08/12/2004
				Simplified and adjusted to deal with selection inside Compound Parents
				Controllers and PostProcessors

				This function should ONLY be called on nodes that /could/ be members of Range
				Not Layers or Paper nodes!

	 Scope:		Protected
     SeeAlso:   Range::SmartFindNext
     Errors:    When attempting to return children and finds no parent link.

******************************************************************************************/

BOOL Range::InRange(Node* pNode, BOOL AndChildren) const
{
	// Preconditions
	ERROR2IF(this==NULL, FALSE, "InRange called on NULL range");
	ERROR2IF(pNode==NULL, FALSE, "InRange given a NULL node");
	ERROR2IF(pNode->IsPaper(), FALSE, "InRange asked to decide on Paper");
	ERROR2IF(pNode->IsLayer(), FALSE, "InRange asked to decide on Layer");
	ERROR2IF(RangeControlFlags.Unselected && !RangeControlFlags.Selected && FirstNode==NULL, FALSE, "InRange can't detect unselected nodes without a FirstNode");

	//------------------------------------------------------------------------------------
	// Test for Primary members of the range...
	//
	// If an op has asked this node for permission to apply the op to it, and the node said no, return.
	if (pNode->GetOpPermission() == PERMISSION_DENIED)
		return FALSE;

	// if trying to ignore none renderable nodes then check the node
	SubtreeRenderState state = pNode->RenderSubtree(NULL);
	if (RangeControlFlags.IgnoreNoneRenderable && !(state==SUBTREE_ROOTONLY || state==SUBTREE_ROOTANDCHILDREN))
		return FALSE;

	// If looking for selected nodes and node is selected, OK
	if (RangeControlFlags.Selected && pNode->IsSelected())
		return TRUE;

// If we're not promoting to parents but we seem to have found one such parent
// Then deny this node
if (!AndChildren && !RangeControlFlags.PromoteToParent && pNode->ShouldITransformWithChildren())
	return FALSE;

	// If looking for all nodes but not going into child subtrees, OK whether node is selected or not
	if (!AndChildren && RangeControlFlags.Selected && RangeControlFlags.Unselected)
		return TRUE;

	// If node isn't paper then it ought to have a parent node which we will need later...
	Node* pParent = pNode->FindParent();
	ERROR2IF_PF(pParent==NULL, FALSE, ("OOER! Range found an ink node %lx without any parents!",pNode));

	Node* pFirstParent = NULL;
	if (FirstNode)
		pFirstParent = FirstNode->FindParent();

	// If looking for immediate siblings only then check that this ndoe has the same parent as
	// the first node (we've already checked pParent to be non-NULL)
	if (RangeControlFlags.SiblingsOnly)
	{
		if (pFirstParent!=pParent)
			return FALSE;
	}
	
	// If looking for unselected nodes then we need to check whether this node is unselected
	// and in the same level of the tree as the first node...
	while (!pNode->IsSelected() && RangeControlFlags.Unselected && pFirstParent && pParent)
	{
		if (pFirstParent == pParent)
			return TRUE;

		pFirstParent = pFirstParent->FindParent();
		pParent = pParent->FindParent();
	}

	//------------------------------------------------------------------------------------
	// Test for Secondary members of the range by recursion...
	//
	// If looking for children of nodes in the primary range specification
	// or looking for selected nodes (which may be "selected-inside")
	// Then we must check that our parent is in the range. If he is then we are!
	if (AndChildren && !pParent->IsLayer() && !pParent->IsPaper())
		return InRange(pParent, AndChildren);

	return FALSE;
}








/*****************************************************************************************/

/********************************************************************************************

>	List* Range::MakeListOfNodes(BOOL AndChildren = FALSE)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	Pointer to a list of NodeListItems representing the current range
	Purpose:	This function will create and return a pointer to a list representing
				the objects in the range. It will return NULL if it couldn't create the
				list due to lack of memory.
				WARNING! This function creates objects which must be deleted by the calling
				routine. The last thing we need right now is another bunch of listitems
				appearing on shutdown.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

List* Range::MakeListOfNodes(BOOL AndChildren)
{
	// Create the list object first
	List* RangeList = new List;
	
	// only build the list if we found enough memory
	if (RangeList)
	{
		// Get the first object in the range to start the loop
		Node* pNode = FindFirst(AndChildren);
		BOOL Failed = FALSE;			// Will be TRUE if we ran out of memory

		// loop round all the objects in the range
		while (pNode)
		{
			if (!pNode->IsNodeHidden())	// DON'T record hidden nodes because they might be deleted!
			{
				// Mark Howitt 13/12/00 - This is a bodge fix for the release version. As the ClipViewAttribute is
				// only used by the new bevel code, it`s safe (Bodge) to make it not includable in any selections.
				// This will need to be fix for future versions if we decide to start using the attribute correctly!
				if(!pNode->IsAClipViewAttr())
				{
					NodeListItem* pItem;
					if ((pItem = new NodeListItem(pNode))!=NULL)
					{
						// Creation successful, so add it to the list
						RangeList->AddTail(pItem);
					}
					else
					{
						// Creation failed, so flag an error and break the loop
						Failed = TRUE;
						break;
					}
				}
			}

			// Go to the next node in the range
			pNode = FindNext(pNode, AndChildren);
		}
		// Handle an error if it occurred
		if (Failed)
		{
			while (!RangeList->IsEmpty())
			{
				NodeListItem* pItem = (NodeListItem*)(RangeList->RemoveHead());
				delete pItem;
			}
			delete RangeList;
			return NULL;
		}

	}
	return RangeList;
}


/********************************************************************************************

>	void Range::UpdateParentBoundsOfSelection(BOOL AndChildren = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/94
	Purpose:	This function updates the parent bounds of all selected objects. It is a 
				lot more efficient than calling ChangeBounds on every node in the selection
				(cos it uses an algorithm!)
	SeeAlso:	NodeRenderableBounded::ChangeBounds

********************************************************************************************/

void Range::UpdateParentBoundsOfSelection(BOOL AndChildren)
{
	// Find the first node in the selection
	Node* pNode = FindFirst(AndChildren);
	
	// loop through the selection, invalidating all the bounding rects
	while (pNode!=NULL)
	{
		// If the node was bounded, then invalidate its bounding rect
		if (pNode->IsBounded())
			((NodeRenderableBounded*)pNode)->InvalidateBoundingRect();

		// Get the next node in the selection
		pNode = FindNext(pNode, AndChildren);
	}

// Below is the old version of this function, that used to use change bounds
// Since this no longer exists, a new bit of code has been created
#if 0
	Node* Current = FindFirst(); 
	Node* UpdatedDaddy = NULL;
	Node* CurrentDaddy;

	NodeRenderableBounded* CurrentBound; 

	// For each selected node
	while(Current != NULL)
	{
		if (Current->IsAnObject())
		{
			// Find out Current's dad
			CurrentDaddy = Current->FindParent(); 
		
			CurrentBound = (NodeRenderableBounded*)Current;

			if (UpdatedDaddy != CurrentDaddy)
			{
				// We need to update CurrentDaddy
				CurrentBound->InvalidateBoundingRect();

				// Remember that CurrentDaddy has been updated
				UpdatedDaddy = CurrentDaddy;   	
			}
		}
		Current = FindNext(Current); 		
	}
#endif
} 




/********************************************************************************************

>	BOOL Range::MakeAttributeComplete(BOOL CheckForDuplicates = TRUE,
									  BOOL AndChildren = FALSE,
									  BOOL IgnoreComplex = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/94

	Inputs:		CheckForDuplicates: When TRUE a check is made to ensure that duplicate
									attribiutes are not added to the subtree
				AndChildren:
				IgnoreComplex:		When TRUE do not make attributes complete on complexcopy
									nodes.
				 	
	Outputs:	-
	Returns:	TRUE if successful
				FALSE if we run out of memory, the children of the node will remain unchanged

	Purpose:	This function calls NodeRenderableInk::MakeAttributeComplete on every Object
				in the selection. If we run out of memory then all changes are undone. 
				  
	SeeAlso:	NodeRenderableInk::MakeAttributeComplete
	SeeAlso:	Range::NormaliseAttributes

********************************************************************************************/

BOOL Range::MakeAttributeComplete(BOOL CheckForDuplicates, 
								  BOOL AndChildren, 
								  BOOL IgnoreComplex)
{
	BOOL MakeComplete;
	Node* Current = FindFirst(AndChildren); // First item in range
	while (Current != NULL)
	{
		if (Current->IsAnObject())
		{
			NodeRenderableInk* pInkObj = (NodeRenderableInk*)Current;
			MakeComplete = TRUE;

			if (IgnoreComplex)
				if ((pInkObj->GetCopyType()) == COMPLEXCOPY)
					MakeComplete=FALSE;

			// complete the attributes on this node if necessary
			if (MakeComplete)
			{
				if (!(pInkObj->MakeAttributeComplete(NULL, CheckForDuplicates)))
				{
					// Note that at this point Current's children have not been changed
					goto Abort; 
				}
			}
		}
		Current = FindNext(Current, AndChildren); 
	}
	return TRUE; // Success
	
	Abort:
	// We need to tidyup any changes that have been made
	BOOL Normalise;
	Node* Sentinal = Current; 
	Current = FindFirst(AndChildren); 
	while (Current != Sentinal )
	{
		ERROR3IF(Current == NULL, "Could not find sentinal"); 
		if (Current->IsAnObject())
		{
			NodeRenderableInk* pInkObj = (NodeRenderableInk*)Current;
			Normalise = TRUE;

			if (IgnoreComplex)
				if ((pInkObj->GetCopyType()) == COMPLEXCOPY)
					Normalise=FALSE;

			// complete the attributes on this node if necessary
			if (Normalise)
				pInkObj->NormaliseAttributes(); 		
		}

		Current = FindNext(Current, AndChildren);
	}
	return FALSE; 
}




/********************************************************************************************

>	void Range::NormaliseAttributes(BOOL AndChildren = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/94
	Inputs		AndChildren:	Normalise the attributes on children as well
				IgnoreComplex:	When TRUE do not make attributes complete on complexcopy
								nodes.

	Outputs:	-
	Returns:	-

	Purpose:	This function calls NodeRenderableInk::NormaliseAttributes on every Object
				in the selection.  
				  
	SeeAlso:	NodeRenderableInk::NormaliseAttributes
	SeeAlso:	Range::MakeAttributeComplete

********************************************************************************************/

void Range::NormaliseAttributes(BOOL AndChildren,
								BOOL IgnoreComplex)
{
	BOOL Normalise;
	Node* Current = FindFirst(AndChildren); // First item in range
	while (Current != NULL)
	{
		if (Current->IsAnObject())
		{
			NodeRenderableInk* pInkObj = (NodeRenderableInk*)Current;
			Normalise = TRUE;

			if (IgnoreComplex)
				if ((pInkObj->GetCopyType()) == COMPLEXCOPY)
					Normalise=FALSE;

			// complete the attributes on this node if necessary
			if (Normalise)
				pInkObj->NormaliseAttributes(); 		

		}
		Current = FindNext(Current, AndChildren);
	}
}





/********************************************************************************************

>	BOOL Range::CopyComponentDataToDoc(BaseDocument* pSrcDoc,
									   BaseDocument* pDestDoc,
									   BOOL AndChildren = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/94
	Inputs:		pSrcDoc: the document from where all nodes in the range were copied
				pDestDoc - where to copy all the nodes to.
	Returns:	FALSE if we are unable to copy the data accross. In this situation 
				AbortComponentCopy is called on the CurrentDocument. 
	Purpose:	Asks all objects in the range (including their children) to copy across their
				component data to the specified destination document.
				The range must be in the destination doc.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Range::CopyComponentDataToDoc(BaseDocument* pSrcDoc,
								   BaseDocument* pDestDoc,
								   BOOL AndChildren)
{
	// Scan all nodes in the range
	Node* Scan = FindFirst(AndChildren);
	while (Scan != NULL)
	{
		Node* Root = Scan;
		Node* Current = Root->FindFirstDepthFirst();
		// This depth first search was written BEFORE the AndChildren flag was passed in.
		// It could probably be removed now...
		while (Current != NULL)
		{
			// Ask the current node if it would copy it's data to the relevant DocComponents
			if (Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderable)))
			{
				if (!((NodeRenderable*)Current)->CopyComponentData(pSrcDoc, pDestDoc))
				{
					// No luck
					pDestDoc->AbortComponentCopy(); // Cancel all data which has been copied
					return FALSE; 
				}
			}

			Current = Current->FindNextDepthFirst(Root); 
		}
		Scan = FindNext(Scan, AndChildren);
	}
	return TRUE; // Success			
} 




/********************************************************************************************

>	virtual DocRect	Range::GetBoundingRect()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	The bounding rectangle of all objects in the range
	Purpose:	To determine the bounds of all objects in the range 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocRect	Range::GetBoundingRect()
{
	DocRect Bounds; 
	// Calculate the union of all bounded nodes within the range 
	Node* pNode = FindFirst(FALSE);
	while (pNode != NULL)
	{
		if (pNode->IsBounded())
			Bounds = Bounds.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect());

		pNode = FindNext(pNode, FALSE);
	} 
	return Bounds; 

}

/********************************************************************************************

>	virtual DocRect	Range::GetBoundingRectForEorDragging()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/2000
	Inputs:		-
	Outputs:	-
	Returns:	The bounding rectangle of all objects in the range
	Purpose:	Same as for GetBoundingRect() but calls GetDragEorBoundingRect for all
				nodes			
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocRect	Range::GetBoundingRectForEorDragging()
{
	DocRect Bounds; 
	// Calculate the union of all nodes within the range 
	NodeRenderableBounded* pNode = (NodeRenderableBounded*) FindFirst(FALSE);
	while (pNode != NULL)
	{
		Bounds = Bounds.Union(pNode->GetEorDragBoundingRect()); 
		pNode =(NodeRenderableBounded*)FindNext(pNode, FALSE);
	} 
	return Bounds; 

}



/////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of NodeListItem
/////////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	NodeListItem::NodeListItem()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	default constructor. Initialises pNode = NULL
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeListItem::NodeListItem()
{
	pNode = NULL;
}

/********************************************************************************************

>	NodeListItem::NodeListItem(Node* WhichNode)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for NodeListItem which sets the pNode variable to WhichNode
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeListItem::NodeListItem(Node* WhichNode)
{
	pNode = WhichNode;
}













//------------------------------------------------------------------------------------------
// Shared XOR-redraw-of-selection methods (Used by Scale, rotate, select tools)


/********************************************************************************************

>	void Range::ResetXOROutlineRenderer(BOOL FlushTheCache = TRUE);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		FlushTheCache - This should ALWAYS be TRUE in external calls

	Purpose:	Shared code used by the rotate, select, and scale tools to XOR the object(s)
				being dragged on/off screen. This function is called before starting to XOR
				the objects onto screen to reset the count of objects which have been rendered
				(for interruptible blob redraw) and other pertinent information.
				This should be called once before starting an outline drag, and under normal
				circumstances need never be called again, as it is called internally by
				RenderXOROutlinesOff at appropriate times.

	Notes:		For example client code, see the selector and rotate tools.
				If you fail to call this before starting outline dragging, the cached drag
				information used by related calls will be wrong, and it'll break.

	SeeAlso:	SelRange::RenderXOROutlinesOn; SelRange::RenderXOROutlinesOff;
				SelRange::RenderXOROutlinesToCatchUp; OpSelectorMove

********************************************************************************************/

typedef enum			// Rendering pass order (a simple state machine mechanism)
{
	RENDER_START,		// About to start rendering passes from the beginning
	RENDER_HIGH,		// Render all high-priority objects
	RENDER_MEDIUM,		// Render all medium-priority objects
	RENDER_LOW,			// Render all low-priority objects (anything that's left!)
	RENDER_END			// Have rendered all available objects
} XORRenderPasses;


void Range::ResetXOROutlineRenderer(BOOL FlushTheCache)
{
	NumObjectsDrawn = 0;
	RenderingPass	= RENDER_START;

	if (FlushTheCache)			// Ensure info cache is cleared     
	{
		XORInfo.PrimeObject = NULL;
		XORInfo.SigRegion.MakeEmpty();
		XORInfo.SigRegionIsScreen = TRUE;
		XORInfo.Cached = FALSE;
	}

	
}



/********************************************************************************************

>	BOOL Range::CacheXOROutlineInfo(Spread *pSpread, Node *ClickedNode);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/94

	Inputs:		Spread - The spread in which the drag is occurring
				ClickedNode - The renderable node which the user clicked on to start the drag.
				(This object will always be drawn first. You may pass in NULL, but this will
				reduce the interactiveness of the dragging system)

	Purpose:	This function is called before starting to XOR the objects onto screen to
				precalculate information pertaining to object significance (the order in which
				objects are drawn)

	Notes:		See the selector and rotate tools for example code using this system.

	Scope:		private
	SeeAlso:	SelRange::RenderXOROutlinesOn; SelRange::RenderXOROutlinesOff;
				SelRange::RenderXOROutlinesToCatchUp

********************************************************************************************/

BOOL Range::CacheXOROutlineInfo(Spread *pSpread, Node *ClickedObject)
// Caches information which is used to determine object significance (drawing
// priority) in the DragBlob XORing routines. If called again, will return
// directly, as it has cached the information. Call ResetRenderingDragBlobs
// to flush the cache ready for re-caching.
{

#if !defined(EXCLUDE_FROM_RALPH)

	if (XORInfo.Cached)		// Have already cached the info
		return TRUE;

	BOOL bPromote = RangeControlFlags.PromoteToParent;
	RangeControlFlags.PromoteToParent = TRUE;

	ERROR2IF(pSpread == NULL,FALSE, "NULL Spread pointer passed to SelRange::CacheXOROutlineInfo");
	ERROR2IF(DocView::GetSelected() == NULL,FALSE, "DocView::GetSelected is NULL in SelRange::CacheXOROutlineInfo");

	DocView *pDocView = DocView::GetSelected();
	if (pDocView == NULL)
	{
		RangeControlFlags.PromoteToParent = bPromote;
		return FALSE;
	}

	XORInfo.SigRegion = pDocView->GetDocViewRect(pSpread);
	
	// Take screenrect or source bounding rect, whichever is smaller
	// (if the source rect passed in was NULL, just use screen coords)
	DocRect SourceRegion = GetBoundingRectForEorDragging();
	if (Count() > 0 &&
		XORInfo.SigRegion.Width() + XORInfo.SigRegion.Height() >
		SourceRegion.Width() + SourceRegion.Height())
	{
		XORInfo.SigRegion = SourceRegion;
		XORInfo.SigRegionIsScreen = FALSE;
	}
	else
	{
		// Convert SigRgn from Document to Spread coords
		pSpread->DocCoordToSpreadCoord(&XORInfo.SigRegion);
		XORInfo.SigRegionIsScreen = TRUE;
	}


	XORInfo.PrimeObject = NULL;
	if (ClickedObject != NULL)
	{
		// And remember which object must always be drawn first (the prime object)
		//	--but only if it's a renderable object that is part of the selection!
		BOOL Selected = ClickedObject->IsSelected();

		if (ClickedObject->ShouldITransformWithChildren())
		{
			Selected = TRUE;
		}

		if (!Selected)
		{
			// The object isn't selected, but perhaps it is in a selected group.
			// Search up through all parent nodes until we have determined if it
			// really is selected.

			Node *TheNode = (Node *) ClickedObject;
			do
			{
				Selected = (TheNode->IsSelected());
				TheNode = TheNode->FindParent();
			}
			while (!Selected && TheNode != NULL);
		}
	
		if (Selected && ClickedObject->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
			XORInfo.PrimeObject = ClickedObject;
	}
	else
	{
		
	}

	XORInfo.Cached = TRUE;

	RangeControlFlags.PromoteToParent = bPromote;

	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL Range::FindNextXOROutlineNode(Node **OriginalNode, DocRect *pClipRect);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/94
	Inputs:		OriginalNode - A handle on the current Node. Will be filled in with NULL or a
				pointer to the next node to be drawn, or NULL if there is nothing more to draw

				pClipRect - The current rendering clip rectangle - nodes outside this area
				will not be considered for redrawing.

	Purpose:	Shared code used by the rotate, select, and scale tools to XOR the object(s)
				being dragged on/off screen. This function is used to calculate the next
				object to be drawn; it attempts to draw the most significant objects first,
				so that minute detail is only drawn if there is enough time spare. This
				significantly enhances interactiveness of dragging.

	Notes:		See the selector and rotate tools for example code using this system.

	Scope:		private

	SeeAlso:	SelRange::RenderXOROutlinesOn; SelRange::RenderXOROutlinesOff;
				SelRange::RenderXOROutlinesToCatchUp

********************************************************************************************/

BOOL Range::FindNextXOROutlineNode(Node **OriginalNode, DocRect *pClipRect)
// Finds the next selected NodeRedrawable for the XOR dragblobs code to draw.
// Now returns objects in the following order, based upon their significance:
//	1) Prime object (the object on whic the user clicked to start the drag)
//	2) High priority object(s) - any:	screen/2 < object_size < screen
//	3) Medium priority - 				screen/6 < object_size < screen/2
//	4) Low priority - anything left over (<screen/6 or >screen)
//
// If the selection was larger than the screen, the significance of each object
// which was initially visible on screen is promoted (by doubling it) to increase
// the chance of visible things being drawn before offscreen things.
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL bPromote = RangeControlFlags.PromoteToParent;
	RangeControlFlags.PromoteToParent = FALSE;

	Node * pParent = NULL;

	BOOL DoRender = FALSE;
	const INT32 MaxSignificance = XORInfo.SigRegion.Width() + XORInfo.SigRegion.Height();
	const INT32 MediumSignificance	= MaxSignificance / 6;
	const INT32 HighSignificance		= MaxSignificance / 2;

	NodeRenderable *PrimeObject = (NodeRenderableInk *)XORInfo.PrimeObject;

	ERROR2IF(OriginalNode == NULL,FALSE, "Range::FindNextXOROutlineNode - NULL parameter is illegal");
	NodeRenderableBounded *CurrentNode = (NodeRenderableBounded *) *OriginalNode;

	if (CurrentNode == NULL)			// Can't continue from here, so start fresh
		RenderingPass = RENDER_START;

	if (CurrentNode == PrimeObject)
		CurrentNode = NULL;				// Start at beginning of selectn for next pass
	else
	{
		if (RenderingPass == RENDER_START && PrimeObject != NULL)
		{
			SubtreeRenderState state = PrimeObject->RenderSubtree(NULL);
			if (state==SUBTREE_ROOTONLY || state==SUBTREE_ROOTANDCHILDREN)
			{
				*OriginalNode = (Node *) PrimeObject;	// Just starting, so
				RangeControlFlags.PromoteToParent = bPromote;
				return TRUE;							// render prime object now
			}
		}
	}

	do
	{
		do		// Search for next renderable object
		{
			if (CurrentNode == NULL)
			{
				// Have run out of objects to draw - Start again on the next pass
				if (++RenderingPass >= RENDER_END)
				{
					*OriginalNode = NULL;
					return TRUE;			// Everything drawn (all passes done)
				}

				CurrentNode = (NodeRenderableBounded *) FindFirst(TRUE);

				// now, see if this node has a PromoteToParent node
				if (CurrentNode)
				{
					pParent = CurrentNode->FindParent();
				}
				else
				{
					pParent = NULL;
				}
				
				// DMc insertion - to cope with compound nodes
				// in the cases they take care of their own blob rendering
				if (pParent && pParent->IsAnObject())
				{
					if (((NodeRenderableInk *)pParent)->ChildrenAreEorDragRenderedByMe())
					{
						CurrentNode = (NodeRenderableBounded *)pParent;
					}

					pParent = pParent->FindParent();
				}
			}
			else
			{
				// DMc insertion - to cope with compound nodes
				// in the cases they take care of their own blob rendering
				CurrentNode = (NodeRenderableBounded *) FindNext(CurrentNode, TRUE);

				// now, see if this node has a PromoteToParent node
				if (CurrentNode)
				{
					pParent = CurrentNode->FindParent();
				}
				else
				{
					pParent = NULL;
				}
				
				while (pParent && pParent->IsBounded())
				{
					if (pParent->IsAnObject())
					{
						if (((NodeRenderableInk *)pParent)->ChildrenAreEorDragRenderedByMe())
						{
							CurrentNode = (NodeRenderableBounded *)pParent;
						}
					}

					pParent = pParent->FindParent();
				}				
			}
				
//				CurrentNode = (NodeRenderableBounded *) FindNextAndChild(CurrentNode);

		} while (CurrentNode == NULL			||
					CurrentNode == PrimeObject	||
					!CurrentNode->IsBounded() );
					
		// Now, determine priority, and return this object if priority is right
		DocRect	BBox = CurrentNode->GetBoundingRect();
		INT32 Significance = BBox.Width() + BBox.Height();

		if (XORInfo.SigRegionIsScreen &&
			Significance < HighSignificance &&
			XORInfo.SigRegion.ContainsRect(BBox))
		{
			// If selection is larger than the screen, and this is not
			// large enough to be a high-significance object, but it was 
			// visible on-screen at the start of the drag, promote its
			// significance so it has a greater chance of being drawn soon.
			Significance *= 2;
		}

		switch(RenderingPass)
		{
			case RENDER_HIGH:			// High significance - draw ASAP
				DoRender = Significance >= HighSignificance &&
							Significance < MaxSignificance;
				break;

			case RENDER_MEDIUM:			// Medium Significance - draw second
				DoRender = Significance >= MediumSignificance &&
							Significance < HighSignificance;
				break;
	
			case RENDER_LOW:			// Low significance - draw last
				DoRender = (Significance >= MaxSignificance) ||
							(Significance < MediumSignificance);
				break;

		 	default:
		 		ERROR3_PF( ("** FindNextDragBlobNode - Illegal rendering pass %d. Tell Jason", RenderingPass) );
 				break;
		}
	} while (!DoRender);	

	*OriginalNode = (Node *) CurrentNode;

	RangeControlFlags.PromoteToParent = bPromote;

	return TRUE;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	static void DrawXORBoundingRect(RenderRegion *pRender, DocRect &TheRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/95

	Inputs:		pRender - a render region to render into
				TheRect - the current bounding rectanglew of the XOR outline objects

	Purpose:	Shared code which renders the bounding rectangle of the XOR outline objects
				in the current colours. It is rendered as a path so that the rect

				Basically just a veneer for DrawDragBounds

	SeeAlso:	OSRenderRegion::DrawDragBounds

********************************************************************************************/

static inline void DrawXORBoundingRect(RenderRegion *pRender, DocRect &TheRect)
{
	if (!TheRect.IsValid() || TheRect.IsEmpty() || !DocView::OutlineShowBounds)
		return;
	
#if !defined(EXCLUDE_FROM_RALPH)
	pRender->SaveContext();

	pRender->SetLineColour(COLOUR_XORSELECT);
	pRender->DrawDragBounds(&TheRect);

	pRender->RestoreContext();
#endif
}



/********************************************************************************************

>	void Range::RenderXOROutlinesToCatchUp(DocRect* ClipRect, Spread *Spread,
												Matrix *Transform);

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (moved here by Jason) (recoded and renamed by Jason)
	Created:	27/10/93 (new lease of life 04/03/94)

	Inputs:		ClipRect - NULL to just clip to the spread/window bounds, or a pointer to
				a clipping rectangle into which the xored stuff will be clipped.
				Spread - The spread in which the drag is occurring
				Transform - A transformation matrix indicating how the blobs should
				be rendered - this allows use of this one routine for translation, scaling,
				and rotation operations.

	Purpose:	Shared code used by the rotate, select, and scale tools to XOR the object(s)
				being dragged onto screen. This function is used when an area is redrawn during
				a drag (e.g. scrolling the window forces a redraw) to 'catch up' to the XOR
				outlines already present in other areas of the screen (i.e. it redraws
				everything that should already be on screen in unchanged areas into the
				invalidated area, so that XOR dragging can continue without leaving mess behind.

	Notes:		See the selector and rotate tools for example code using this system.
				This function is identical to RenderXOROutlinesOff, only it preserves the
				current state of background redraw, so that XOR state will not be corrupted.

	SeeAlso:	SelRange::RenderXOROutlinesOn; SelRange::RenderXOROutlinesOff

********************************************************************************************/

void Range::RenderXOROutlinesToCatchUp(DocRect *ClipRect, Spread *pSpread, Matrix *Transform)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL bPromote = RangeControlFlags.PromoteToParent;
	RangeControlFlags.PromoteToParent = TRUE;

	INT32 OldRenderingPass = RenderingPass;	// Preserve previous rendering pass

	// Start up a bit of a RenderOnTop loop type of thing to do all the rendering
	RenderRegion* pRegion = DocView::RenderOnTop( ClipRect, pSpread, ClippedEOR );
	while ( pRegion )
	{
		// Need to tell the render region to transform the objects a bit first.
		Matrix OldRenderMatrix = pRegion->ConcatenateMatrix( *Transform );

		DocRect Bounds = GetBoundingRectForEorDragging();
		DrawXORBoundingRect(pRegion, Bounds);

		// Set the line colour and fill colour
		pRegion -> SetFillColour( COLOUR_TRANS );
		pRegion -> SetLineColour( COLOUR_XORDRAG );

		// Render the first NumObjectsDrawn objects
		Node *CurrentNode = NULL;
		RenderingPass = RENDER_START;

		for (INT32 i=0; i<NumObjectsDrawn; i++)
		{
			// Find and draw the next object
			FindNextXOROutlineNode(&CurrentNode, ClipRect);
			if (CurrentNode == NULL)
				break;				// Shouldn't happen, but best to be safe

			if (CurrentNode->IsRenderable() || ForceRenderEORAll()) ((NodeRenderableInk*)CurrentNode)->RenderEorDrag(pRegion);
		}

		// OK, we have finished with this render region, so set the matrix
		// back to how we found it and get the next one
		pRegion -> SetMatrix( OldRenderMatrix );
		pRegion = DocView::GetNextOnTop( ClipRect );
	}

	RenderingPass = OldRenderingPass; 	// Restore previous rendering pass info

	RangeControlFlags.PromoteToParent = bPromote;

#endif
}


/********************************************************************************************

>	void Range::RenderXOROutlinesOff(DocRect* ClipRect, Spread *Spread,
												Matrix *Transform);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/94

	Inputs:		ClipRect - NULL to just clip to the spread/window bounds, or a pointer to
				a clipping rectangle into which the xored stuff will be clipped.
				Spread - The spread in which the drag is occurring
				Transform - A transformation matrix indicating how the blobs should
				be rendered - this allows use of this one routine for translation, scaling,
				and rotation operations.

	Purpose:	Shared code used by the rotate, select, and scale tools to XOR the object(s)
				being dragged onto screen. This function is used to remove XORed object outline
				blobs from the screen (obviously they will only be truly removed if you call it
				with the screen in the correct state). After the outlines have been rendered,
				the XOROutline renderer state is reset, so it is ready to begin a fresh pass 
				of rendering blobs back on to the screen.

	Notes:		See the selector and rotate tools for example code using this system.

	SeeAlso:	SelRange::RenderXOROutlinesOn; SelRange::RenderXOROutlinesToCatcvhUp

********************************************************************************************/

void Range::RenderXOROutlinesOff(DocRect *ClipRect, Spread *pSpread, Matrix *Transform)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// If called in the correct state, the following will un-XOR the existing drag blobs
	// and then reset the rendering state (as we should now have a 'blank' screen, so 
	// there is no way we should try to un-XOR this stuff again)
	RenderXOROutlinesToCatchUp(ClipRect, pSpread, Transform);
	ResetXOROutlineRenderer(FALSE);		// Reset, but don't discard cached info
#endif
}


/********************************************************************************************

>	void Range::RenderXOROutlinesOn(DocRect* ClipRect, Spread *Spread,
										Matrix *Transform, Node *ClickedObject);

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com> (moved here by Jason) (recoded by Jason)
	Created:	27/10/93

	Inputs:		ClipRect - NULL to just clip to the spread/window bounds, or a pointer to
				a clipping rectangle into which the xored stuff will be clipped.
				Spread - The spread in which the drag is occurring
				Transform - A transformation matrix indicating how the blobs should
				be rendered - this allows use of this one routine for translation, scaling,
				and rotation operations.
				ClickedObject - NULL, or a pointer to the node object which needs to be redrawn
				first (the prime object). This allows super-interactive dragging, by always
				redrawing the object which the user clicked upon to start the drag first, so
				a specific thing can be easily positioned without waiting hours for redraw.
				Note that the 'ClickedObject' is cached during the first call, so it need only
				be passed in the first time you call this function.

	Purpose:	Shared code used by the rotate, select, and scale tools to XOR the object(s)
				being dragged on/off screen. This function will do background rendering to 
				XOR outlines onto screen (Successive calls will render more of the selection
				until it has all been rendered - it must therefore be called during mouse idle
				events to complete redraws)

	Notes:		See the selector and rotate tools for example code using this system.

	SeeAlso:	SelRange::RenderXOROutlinesOff; SelRange::RenderXOROutlinesToCatcvhUp

********************************************************************************************/

void Range::RenderXOROutlinesOn(DocRect *ClipRect, Spread *pSpread,
								   	Matrix *Transform, Node *ClickedObject)
{
	// DMc
	// promote the clicked object to any parents which take care of their own eor drag
	// rendering
	if (ClickedObject)
	{
		Node * pLastParent = ClickedObject;
		Node * pParent = ClickedObject->FindParent();
		
		while (pParent)
		{
			if (pParent->IsAnObject())
			{
				if (((NodeRenderableInk *)pParent)->ChildrenAreEorDragRenderedByMe())
				{
					pLastParent = pParent;
				}
			}
			
			pParent = pParent->FindParent();
		}
		
		ClickedObject = pLastParent;
	}

	BOOL bPromote = RangeControlFlags.PromoteToParent;
	RangeControlFlags.PromoteToParent = TRUE;

#if !defined(EXCLUDE_FROM_RALPH)
	CacheXOROutlineInfo(pSpread, ClickedObject);

		// If we have drawn nothing so far, find the first object
	if (NumObjectsDrawn == 0)
		NodeToRender = NULL;
	else
	{
		if (RenderingPass == RENDER_END)
		{
			RangeControlFlags.PromoteToParent = bPromote;
			return;	// No more to be drawn
		}
	
		if (NodeToRender == NULL)		// If starting fresh redraw, ensure counter zeroed
			NumObjectsDrawn = 0;
	}

#if ENABLE_FAST_XOR
	// The following significantly speeds up XOR redrawing
	// The problem is that we want to multitask while drawing blobs on, so
	// we must draw each object into ALL render regions. The code below works,
	// but has to create a new render region for each invalid region for each
	// node, so is *very* slow.
	// However, in the case where there is only one render region (often the case
	// when dragging in a topped window, and apparently also the case under Windows,
	// which gives you an all-encompassing rectangle and does the clipping to the
	// arbitrary invalid region itself), we can just select that region, draw
	// all the objects we can, and then deselect the region. Much faster

	// First, count the number of render regions with a dummy rendering loop...
	INT32 RegionCount = 0;
	RenderRegion* pRegion = DocView::RenderOnTop( ClipRect, pSpread, ClippedEOR );
	while ( pRegion )
	{
		RegionCount++;
		pRegion = DocView::GetNextOnTop( ClipRect );
	}

	if (RegionCount == 1)		// There is ony one render region, so we can draw much faster
	{
		MonotonicTime RenderClock;
		RenderRegion* pRegion = DocView::RenderOnTop( ClipRect, pSpread, ClippedEOR );

		// Need to tell the render region to transform the objects a bit first.
		Matrix OldRenderMatrix = pRegion->ConcatenateMatrix( *Transform );

		// Draw the original bounding box in it's rotated/translated position, but only
		// on the first pass - we don't want the box flashing on and off!
		if (NumObjectsDrawn == 0)
		{
			DocRect Bounds = GetBoundingRectForEorDragging();

			DrawXORBoundingRect(pRegion, Bounds);
		}
		
		// Set the line colour and fill colour
		pRegion -> SetFillColour( COLOUR_TRANS );
		pRegion -> SetLineColour( COLOUR_XORDRAG );

		while (!RenderClock.Elapsed( 90 ))
		{
			FindNextXOROutlineNode(&NodeToRender, ClipRect);

			if (NodeToRender == NULL)
				break;		// Nothing more to render - exit

			// Render the next node that needs rendering
			if (NodeToRender->IsRenderable() || ForceRenderEORAll()) ((NodeRenderableInk*)NodeToRender) -> RenderEorDrag( pRegion );

			// Keep the count of what we have drawn
			NumObjectsDrawn++;
		}

		// OK, we have finished with this render region, so set the matrix
		// back to how we found it and get the next one
		pRegion -> SetMatrix( OldRenderMatrix );
		pRegion = DocView::GetNextOnTop( ClipRect );

		ERROR3IF(pRegion != NULL, "FastXor drawing failed - There is another render region!");

		RangeControlFlags.PromoteToParent = bPromote;
		return;
	}
	
	//... else drop through to the normal slow rendering loop

#endif


	// Start a clock to tell us how long we have been rendering
	MonotonicTime RenderClock;
	while ( !RenderClock.Elapsed( 90 ) )
	{
		FindNextXOROutlineNode(&NodeToRender, ClipRect);
		if (NodeToRender == NULL)
		{
			RangeControlFlags.PromoteToParent = bPromote;
			return;				// Nothing else to draw, may as well stop
		}

		// Start up a bit of a RenderOnTop loop type of thing to do all the rendering
		RenderRegion* pRegion = DocView::RenderOnTop( ClipRect, pSpread, ClippedEOR );
		while ( pRegion )
		{
			// Need to tell the render region to transform the objects a bit first.
			Matrix OldRenderMatrix = pRegion->ConcatenateMatrix( *Transform );

			// Draw the original bounding box in it's rotated/translated position, but only
			// on the first pass - we don't want the box flashing on and off!
			if (NumObjectsDrawn == 0)
			{
				DocRect Bounds = GetBoundingRectForEorDragging();
				DrawXORBoundingRect(pRegion, Bounds);
			}
			
			// Set the line colour and fill colour
			pRegion -> SetFillColour( COLOUR_TRANS );
			pRegion -> SetLineColour( COLOUR_XORDRAG );

			// Render the next node that needs rendering
			if (NodeToRender->IsRenderable() || ForceRenderEORAll()) ((NodeRenderableInk*)NodeToRender) -> RenderEorDrag( pRegion );

			// OK, we have finished with this render region, so set the matrix
			// back to how we found it and get the next one
			pRegion -> SetMatrix( OldRenderMatrix );
			pRegion = DocView::GetNextOnTop( ClipRect );
		}
		
		// Keep the count of what we have drawn
		NumObjectsDrawn++;
	}
#endif

	RangeControlFlags.PromoteToParent = bPromote;

}



/********************************************************************************************

> 	virtual BOOL Range::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
														BOOL DoPreTriggerEdit = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 19/01/2000
	Created:	6/02/95
	Inputs:		pParam				 = 	describes the way an op wants to change the node
				SetOpPermissionState = 	if TRUE the Op permission state of this node (and its parents) will be set 
										according to the outcome of the call
				DoPreTriggerEdit	 =	if TRUE then calls NameGallery::PreTriggerEdit.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Outputs:	-
	Returns:	TRUE if one or more of the nodes in the range will allow the op to happen, FALSE otherwise
	Purpose:	Calls AllowOp() on all nodes in the range.
				If SetOpPermissionState is TRUE, the op permission state of all nodes in the range, plus all parents
				of these nodes, will have their op permission state changed.

	SeeAlso:	Node::AllowOp(),Node::GetOpPermission(),Node::SetOpPermission()

********************************************************************************************/

BOOL Range::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState, BOOL DoPreTriggerEdit)
{
	Node* pNode = FindFirst();
	BOOL result = FALSE,allowed;

	// we call AllowOp on all Nodes in the range, but never ask for a PreTriggerEdit on
	// each one, as if it is necessary then we deal with them en masse next.
	while (pNode != NULL)
	{
		Node* pNextNode = FindNext(pNode);
		allowed = pNode->AllowOp(pParam, SetOpPermissionState, FALSE);
		result  = result || allowed;
		pNode = pNextNode;
	}

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (result && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			result = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}

	return result;
}

/********************************************************************************************

>	BOOL Range::GetCompoundObjectSet(ObjectSet* pCompoundObjectSet, 
									 BOOL ExcludeTextObjects = FALSE); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/95
	Inputs:		-
	Outputs:	pCompoundObjectSet: A set of all the parent compound nodes of the items in the
									range. This set can already contain items on entry to this
									function, and these items will not be deleted. 

				ExcludeTextObjects: When TRUE TextStory compound objects will be excluded from 
									the set.


	Returns:	TRUE if successful
				FALSE if we run out of memory. In this situation we do NOT delete any items
				already added to the set. 

	Purpose:	This function traverses the range, and whenever it finds an object which is
				the child of a compound node it adds the compound node to the set.

				This is useful if you want to process the compound parents of the range 
				after they have been hidden, eg. when we need to factor out 
				attributes after a complex deletion.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Range::GetCompoundObjectSet(ObjectSet* pCompoundObjectSet, BOOL ExcludeTextObjects /* = FALSE*/) 
{
	Node *pParent;

	// Scan the range
	Node* pCurrent = FindFirst();
	while (pCurrent)
	{
		pParent = pCurrent->FindParent();
		ERROR3IF(!pParent, "Object in range has no parent !"); 
		if (pParent && (pParent->IsCompound()))
		{
			if (!(ExcludeTextObjects && ((IS_A(pParent, TextLine)) || (IS_A(pParent, TextStory)))) )
			{

				// A compound parent has been found, add it to the set
				if (!(pCompoundObjectSet->AddToSet(((NodeRenderableInk*)pParent))))
				{
					return FALSE;
				}
			}
		}

		pCurrent = pCurrent->FindNext(); 
	}
	return TRUE;
}


 /********************************************************************************************

>	BOOL Range::FindCommonAttributes(CommonAttrSet* CommonAttributeSet, 
									 BOOL CompoundAccuracy = FALSE )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94

	Inputs:		CommonAttributeSet:An OPTIONAL subset of attribute types to find common attributes 
				for. Use the CommonAttrSet::AddTypeToSet fn to build up a subset of attribute types. 
				If this set is empty on entry then the function will find common 
				attributes for ALL attribute types.

				Handy hint: If you repeatedly need to find common attribute values for a specific
				set of attribute types eg. for a tool's InfoBar,  then you only need to create 
				the CommmonAttributeSet once (during initialisation). 

				CompoundAccuracy: TRUE => Accurate results with compound objects. This requires
								  more work. This is important in some cases eg. PasteAttributes. 				 

	Outputs:	The CommonAttributeSet containing a CommonAttributeItem for each attribute Type 
				(or a subset of attribute types - see above). These define the status of each 
				attribute type (see below). Use CommonAttrSet::FindAttrItem to locate a particular 
				attribute type in this set; the order of the items in the set will have changed.

	Returns:	TRUE if successful, FALSE otherwise (out of memory) 
	Purpose:	Finds all attributes which are common to the objects in the selection. There is a
				CommonAttributeItem for every attribute type (common or not). The Status field
				in this structure specifies if the attribute is common. 

				Status field values

				ATTR_COMMON:There is a common attribute 
							pAttr will point to an instance of it 

				ATTR_NONE:	There is no selection, or none of the selected objects require the 
							attribute. 
							If there is a document
								pAttr will point to the current attribute for the 
								selected tool, this will be the value which is usually 
								displayed in this situation.
							else
								pAttr will be NULL
								No value will usually be displayed in this situation
						 
				ATTR_MANY:	There is no common attribute. In this situation 'Many' will usually
							be displayed in this situation.


	Errors:		-
	Scope:		private
	SeeAlso:	Range::FindCommonAttribute
	SeeAlso:	InternalClipboard::GetCommonAttributes
	SeeAlso:	CommonAttrSet
	SeeAlso:	CommonAttrSet::FindAttrItem

********************************************************************************************/

BOOL Range::FindCommonAttributes(CommonAttrSet* CommonAttributeSet, 
								 BOOL CompoundAccuracy /* = FALSE */)
{
#if !defined(EXCLUDE_FROM_RALPH)

	ERROR3IF(!CommonAttributeSet, "Common Attribute Set is NULL"); 
	
	BOOL DelCommonAttrSet = FALSE; // If we run out of memory whilst processing then 
								   // we want to leave the CommonAttributeSet with 
								   // the same number of items it entered with.
			  
	// Has the user specified a subset of attributes ?
	if (CommonAttributeSet->IsEmpty())
	{
		// Place all attributes in the CommonAttributeSet
		if (!(CommonAttributeSet->AddAllTypesToSet()))
		{
			return FALSE; // ERROR already called 
		}
		DelCommonAttrSet = TRUE; // Delete all items in set if we fail
	}

	// Initialise all CommonAttributeItems. Remember this function has been written so that 
	// the CommonAttributeSet can be reused. So the Status and pAttr fields may contain
	// garbage on entry to the function.
	CommonAttributeSet->ClearResults();
	
	INT32 NumValuesToFind; // The number of attribute types we need to find values for.
						 // This value will decrease as more values are found.
	INT32 NumNewValues;
	 
	// One, Many, or all common attribute values may already be cached
	CommonAttrSet* pCommonAttrCache = GetCommonAttrCache(); 

 	if (pCommonAttrCache)
	{
		// We only need to consider the first NumValuesToFind attributes in the list
		NumValuesToFind = CommonAttributeSet->UpdateKnownValues(pCommonAttrCache);
	}
	else
	{
		// We need to find all values
	 	NumValuesToFind = CommonAttributeSet->GetCount();
	}

	NumNewValues = NumValuesToFind;

	if (!NumValuesToFind)
		return TRUE; 

	// This is a recent optimisation. We create a map of Attribute types to applied attrs. This 
	// allows us to scan up the tree looking for attributes applied to each object once only.
	CCAttrMap* pAttrMap = new CCAttrMap;  // Create it dynamically as it could be quite big
	if (!pAttrMap)
	{
		if (DelCommonAttrSet) CommonAttributeSet->DeleteAll(); 
		return FALSE; // out of memory
	}

	// A special type of attribute map which is used for Compound objects only. It only gets 
	// built if and when we need it. When we find a default attribute applied to a compound
	// object, we only know that this attribute is a common attribute if at least one object
	// in the compound requires the attribute AND the attribute does not make an appearance 
	// anywhere as a child of the compound.	The attrmap maps an attribute type which is known
	// to be a default onto an ObjectAttrUsage value.
	BOOL CompoundAttrMapBuilt;  
	CMapPtrToWord* pCompoundAttrMap = new CMapPtrToWord;
	if (!pCompoundAttrMap)
	{
		delete pAttrMap;
		if (DelCommonAttrSet) CommonAttributeSet->DeleteAll(); 
		return FALSE;	// out of memory 
	}
	
	// Scan all objects in the range,
	Node* pNode = NULL;
	NodeRenderableInk* CurrentObject;

	pNode = FindFirst(); 
	// Stop when we have found all attribute values (all ATTR_MANY), or there are no more objects
	// to process.
	while ((pNode != NULL) && (NumValuesToFind))
	{
		if (pNode->IsAnObject())
		{
			CurrentObject = (NodeRenderableInk*)pNode;

			pAttrMap->RemoveAll(); 			// Remove any items that may have been in the map
			pCompoundAttrMap->clear(); 	// Ditto
			CompoundAttrMapBuilt = FALSE; 	// Cos we only build this when it becomes required

			// Find all attributes applied to CurrentObject, let's worry about compounds a bit later 
			CurrentObject->FindAppliedAttributes(pAttrMap); 

			// Scan attributes in the CommonAttributeSet
			CommonAttributeItem* CommonAttrItem;
			CommonAttributeItem* NxtCommonAttrItem;

			INT32 NumValuesToProcess = NumValuesToFind;
			
	   		CommonAttrItem = (CommonAttributeItem*)CommonAttributeSet->GetHead();
			while (NumValuesToProcess)
			{
				ERROR3IF(CommonAttrItem==NULL || CommonAttrItem->CheckMemory(FALSE)==FALSE, "CommonAttrItem should not be NULL"); 
				// Hand over hand, as the item will be moved to the tail of the list if 
				// it's value becomes known
				NxtCommonAttrItem = (CommonAttributeItem*)CommonAttributeSet->GetNext(CommonAttrItem); 
				if (CurrentObject->RequiresAttrib(CommonAttrItem->AttrType, CompoundAccuracy)) 
				{
					// Find the attribute which is applied to the CurrentObject
					void* vpAttr=NULL;
					NodeAttribute* pAttrNode;
					pAttrMap->Lookup(CommonAttrItem->AttrType, vpAttr);
					pAttrNode = (NodeAttribute*)vpAttr;
					ERROR3IF(pAttrNode == NULL, "Could not find attribute of type AttrType"); 
					if (pAttrNode != NULL)	// For safety
					{
						BOOL AttrCommonInCompound = TRUE;
						BOOL AttrReqd = TRUE;
						// Special Compound object processing
						if (CurrentObject->IsCompound())
						{
							// In this situation pAttrNode will be a common attribute if it is 
							// not a default attribute. i.e. if it's parent is not a NodeDocument
							if (pAttrNode->IsADefaultAttr())
							{
								// ok we are messing with a default attribute here.
								if (!CompoundAttrMapBuilt)
								{
									// If we haven't already got ourselves a CompoundAttrMap
									// construct one, adding all attribute types that are
									// known to be defaults.
									if (!CommonAttributeSet->BuildAttrDetailsMap(CurrentObject, 
																			NumValuesToFind,
																			pAttrMap, 
																			pCompoundAttrMap))
									{
										if (DelCommonAttrSet) CommonAttributeSet->DeleteAll();
										delete pAttrMap; delete pCompoundAttrMap; 
										return FALSE; 
									}
									CompoundAttrMapBuilt = TRUE;
								}
//								WORD wCompoundAttrUsage;
//								pCompoundAttrMap->Lookup(CommonAttrItem->AttrType, wCompoundAttrUsage);
								CMapPtrToWord::iterator iter = pCompoundAttrMap->find(CommonAttrItem->AttrType);
								if (pCompoundAttrMap->end() != iter)
								{
									CommonAttrSet::ObjectAttrUsage CompoundAttrUsage = 
										(CommonAttrSet::ObjectAttrUsage)iter->second;
									// if the attribute type has been found as a child of the compound then we know
									// it's not a common attribute of the compound, and so the Selection.
									if (CompoundAttrUsage == CommonAttrSet::FOUND)
										AttrCommonInCompound = FALSE;
									else if (CompoundAttrUsage == CommonAttrSet::NOT_REQUIRED)
										AttrReqd = FALSE;
									// else CompoundAttrUsage == REQUIRED (attr is common so continue)
								}
							}
						}
						if (AttrReqd)  
						{
							// Ok it has an attribute of this type
							if (CommonAttrItem->pAttr == NULL)
							{
								// The attribute becomes a common attribute
								CommonAttrItem->Status = Range::ATTR_COMMON;  // up to this point anyway
								CommonAttrItem->pAttr = pAttrNode;
							}						
							// not an else (need to catch Compound case
							if (!AttrCommonInCompound || (!((*pAttrNode)==(*(CommonAttrItem->pAttr)))) )
							{
								// Attributes have different values. 
								CommonAttrItem->Status = Range::ATTR_MANY;
								CommonAttrItem->pAttr = NULL;
								// This item's value has been found it will not cahnge so move it to the
								// end of the list.	
								CommonAttributeSet->RemoveItem(CommonAttrItem); 
								CommonAttributeSet->AddTail(CommonAttrItem);
								NumValuesToFind--; // One less to find for the next object
							}
						}
					} // if
				} // if
				NumValuesToProcess--;
				CommonAttrItem = NxtCommonAttrItem;
			} // Attribute scan
		}
		// Get the next object
		pNode = FindNext(pNode);
	} // Range scan


	// Post processing: For every new value found 
	// 		Each ATTR_NONE item should point at the current attribute associated with the selected tool
	//		The new value should be added to the cache
	Document* SelectedDoc = Document::GetSelected();

	CommonAttributeItem* CommonAttrItem;

	AttributeManager& pAttrMgr = SelectedDoc->GetAttributeMgr();
	INT32 Count;
	for (CommonAttrItem = (CommonAttributeItem*)CommonAttributeSet->GetHead(), Count = 0;
		 Count != NumNewValues;
		 CommonAttrItem = (CommonAttributeItem*)CommonAttributeSet->GetNext(CommonAttrItem), Count++)
	{
		ERROR3IF(!CommonAttrItem, "CommonAttrItem should not be NULL"); 
		if (SelectedDoc && CommonAttrItem->Status == Range::ATTR_NONE)
		{
			CommonAttrItem->pAttr = pAttrMgr.GetSelToolCurrentAttribute(CommonAttrItem->AttrType);
		}
		// Add the item to the cache.
		if (pCommonAttrCache)
		{
			CommonAttributeItem* pNewAttrItem = new CommonAttributeItem(CommonAttrItem->AttrType);
			if (pNewAttrItem)
			{
				// Add the attribute type results to the cache, taking a copy of the attribute
				if (!(pNewAttrItem->InitSafeAttrItem(CommonAttrItem->AttrType, 
													 CommonAttrItem->pAttr, 
													 CommonAttrItem->Status)))
				{
					delete pNewAttrItem;
				}
				else
				{
					pCommonAttrCache->AddTail(pNewAttrItem); // Add to the cache	
				}
			} // else we failed to allocate memory for the item, we can live with this.
		}
	}

	// else pAttr will be NULL for all items

	delete pAttrMap;
	delete pCompoundAttrMap; 

	return TRUE; // Job done
#else
	return FALSE;
#endif
}



/********************************************************************************************

>   CommonAttribResult Range::FindCommonAttribute(RuntimeClass* AttribType, 
													 NodeAttribute** pAttribute,
													 BOOL bRangeIsEffectsLevel = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/94
	
	Inputs:		AttribType: The Type of attribute to find - defined in the virtual 
							GetAttributeType method	of NodeAttribute

	Outputs:	pAttribute: Depends on the return value, see below
											 

	Returns:	ATTR_COMMON:There is a common attribute 
							pAttribute will point to an instance of it 

				ATTR_NONE:	There is no selection, or none of the selected objects require the 
							attribute. 
							If there is a document
								pAttribute will point to the current attribute for the 
								selected tool, this will be the value which is usually 
								displayed in this situation.
							else
								pAttribute will be NULL
								No value will usually be displayed in this situation
						 
				ATTR_MANY:	There is no common attribute, 
							pAttribute will be NULL
						 
		
	Purpose:	To determine if all objects in the range share an attribute with the 
				same value. You should only use this function if you need to find a 
				common attribute value for a single attribute type. If you need to find
				common values for many types then FindCommonAttributes is much more efficient.

	Errors:		-
	Scope:		private
	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/

Range::CommonAttribResult Range::FindCommonAttribute(CCRuntimeClass* AttribType, 
												 	 NodeAttribute** pAttribute,
													 BOOL bRangeIsEffectsLevel)
{
#if !defined(EXCLUDE_FROM_RALPH)
	CommonAttribResult Result;
	*pAttribute = NULL;

	// Before we go away and do any hard work, let's see if the value that the user
	// is after is already cached.
	CommonAttrSet* pCommonAttrCache = GetCommonAttrCache(); 
	if (pCommonAttrCache)
	{
		// are the AttrTypes common details cached 
		CommonAttributeItem* pAttrItem = pCommonAttrCache->FindAttrItem(AttribType);
		if (pAttrItem)
		{
			// Aren't we lucky
			*pAttribute = pAttrItem->pAttr;
			return (pAttrItem->Status);
		} 	
	}
	// else the cache could not be created. We shall just have to work without one 

	// Scan each selected node until we get a result
	NodeRenderableInk* n = NULL;
	NodeRenderableInk* pn = (NodeRenderableInk*)FindFirst(); 
	
	NodeAttribute* AppliedAttr; // The attribute applied and used by n.

	while (pn != NULL)
	{
		n = pn;
		if (bRangeIsEffectsLevel)
			n = pn->GetObjectToApplyTo(AttribType);

		AppliedAttr = NULL;
		if (n->IsCompound())
		{
			// Determine if all objects within the  compound object share a common attribute
			
			ERROR3IF(!n->IsAnObject(), "Item in range is not a NodeRenderableInk"); 

			// We may sometime later decide that this attribute is not applied to the 
			// compound.
			AppliedAttr = ((NodeRenderableInk*)n)->FindAppliedAttribute(AttribType);

			if (AppliedAttr->IsADefaultAttr() && !((NodeRenderableInk*)n)->IsValidEffectAttr(AppliedAttr))	 // else AppliedAttr will be correct
			{
				// This can mean one of three things
				// a. All nodes in the compound which require the attribute share the default 
				//    attribute
				// b. Nodes requiring attribute have more than one attribute value. i.e. 
				//    the default attribute is not a common attribute.
				// c. No nodes in the group require the attribute

				// Flag which is set to TRUE when we know that AttribType is required by the 
				// the compound
				BOOL AttribRequiredInCompound = FALSE; 

				// Scan the compound depth first
				Node* Current = n->FindFirstDepthFirst();
									
				while(Current != n)	 // Stop when we hit the compound (used to be NULL, this was a bug)
				{
					if (!AttribRequiredInCompound)
					{
						// Determine if the attribute is required by the current node
						if (Current->IsAnObject())
						{
							AttribRequiredInCompound = ((NodeRenderableInk*)Current)
								->RequiresAttrib(AttribType); 	
						}
					}
					if (Current->IsAnAttribute())
					{
						if (((NodeAttribute*)Current)->GetAttributeType() == AttribType)
						{
							// This means that the compound must have more than one
							// attribute value, because if this attribute was common to
							// all objects requiring the attribute in the compound then
							// it would have been found in the child attribute block !
							(*pAttribute) = NULL; // Nothing useful to return here
							Result = ATTR_MANY;
							goto End;		 
						}
					}
					Current = Current->FindNextDepthFirst(n); 		
				}
				if (!AttribRequiredInCompound)
				{
					AppliedAttr = NULL; // Compound has no applied attribute !
				}
				// else attribute not required in compound so ignore it. 
			}
		}
		else
		{
			// Does n require attribute AttribType to render itself ?
			if (n->RequiresAttrib(AttribType))
			{
				// n needs to have attribute AttribType defined so that it can render properly
				// scan from the last child of n, up the tree in reverse render order until we 
				// find an instance of an AttribType attribute.
				AppliedAttr = n->FindAppliedAttribute(AttribType);
				
			}
			// else n does not require the attribute so it is ignored
		}
		
		// Determine if the attribute is common

		if (AppliedAttr != NULL)
		{

			if ((*pAttribute) == NULL) // This is the first instance of the attribute found
			{
				(*pAttribute) = AppliedAttr;	
			}
			else
			{
				// If the attribute found is not the same as all others found then 
				// we know that there is no common attribute, so there is no point in
				// searching any further
			
				if (!((**pAttribute) == (*AppliedAttr)))
				{
					(*pAttribute) = NULL; // Nothing useful to return here
					Result = ATTR_MANY;
					goto End;
				}
				// else the Applied attribute is the same as all others found so carry
				// on searching.
			}
		}
		 
		pn = (NodeRenderableInk*)FindNext(pn); // Obtain the next selected object
	}

	// Ok weve finished the scan, so either no objects require the attribute, or there
	// is a common attribute value shared by all objects that do require the attribute
	if ((*pAttribute) != NULL)
	{
		Result = ATTR_COMMON;
		// pAttribute will point to an instance of the common attribute
	}
	else
	{
		Result = ATTR_NONE;

		Document* SelectedDoc = Document::GetSelected();
	
		// If there is a selected document then pAttribute is set to the current attribute
		// for the selected tool.
		if (SelectedDoc != NULL)
		{
			(*pAttribute) = SelectedDoc->GetAttributeMgr().GetSelToolCurrentAttribute(AttribType);
		}
		// else pAttribute will be NULL	
	}

	End:

	// Add the result to the Common attribute cache
	if (pCommonAttrCache)
	{
		CommonAttributeItem* pNewAttrItem = new CommonAttributeItem(AttribType);
		if (pNewAttrItem)
		{
			// Add the attribute type results to the cache, taking a copy of the attribute
			if (!(pNewAttrItem->InitSafeAttrItem(AttribType, *pAttribute, Result)))
				delete pNewAttrItem;
			else
				pCommonAttrCache->AddTail(pNewAttrItem); // Add to the cache	
		} // else we failed to allocate memory for the item, we can live with this.
	}
	// If we have a Common
	return Result;
#else
	return ATTR_NONE;
#endif
}




/********************************************************************************************

>   Range::CommonAttribResult Range::FindCommonAttributeType(CCRuntimeClass* AttribType, 
												 		       CCRuntimeClass** pCommonType,
															   BOOL ForceAttribType,
															   BOOL bRangeIsEffectsLevel = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	
	Inputs:		AttribType: The Type of attribute to find - defined in the virtual 
							GetAttributeType method	of NodeAttribute

	Outputs:	pCommonType: Depends on the return value, see below

	Returns:	ATTR_COMMON:There is a common attribute type
							pCommonType will point to its runtime class 

				ATTR_NONE:	There is no selection, or none of the selected objects require the 
							attribute. 
							If there is a document
								pCommonType will point to the type of thecurrent attribute 
								for the selected tool, this will be the value which is usually 
								displayed in this situation.
							else
								pCommonType will be NULL
								No value will usually be displayed in this situation
						 
				ATTR_MANY:	There is no common attribute type, 
							pCommonType will be NULL
						 
		
	Purpose:	To determine if all objects in the selection share attributes with the 
				same type. 

	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

Range::CommonAttribResult Range::FindCommonAttributeType(CCRuntimeClass* AttribType, 
												 		 CCRuntimeClass** pCommonType,
														 BOOL ForceAttribType,
														 BOOL bRangeIsEffectsLevel)
{
#if !defined(EXCLUDE_FROM_RALPH)
	CommonAttribResult Result;
	NodeAttribute *pAttribute = NULL;
	*pCommonType = NULL;

	// Scan each selected node until we get a result
	NodeRenderableInk* n = NULL;
	NodeRenderableInk* pn = (NodeRenderableInk*)FindFirst(); 
	
	NodeAttribute* AppliedAttr; // The attribute applied to n

	while (pn != NULL)
	{
		n = pn;
		if (bRangeIsEffectsLevel)
			n = pn->GetObjectToApplyTo(AttribType);

		AppliedAttr = NULL;
		if (n->IsCompound())		// TODO: Don't do this clause if n would allow effect attrs of the given type???
		{
			// Determine if all objects within the  compound object share a common attribute
			
			// (Check the child attribute block)
			// If we find the attribute defined as a left child of the compound object (before 
			// other, non attribute nodes) then we can be sure that this attribute is 
			// shared by all objects requiring the attribute within the compound.

			ERROR3IF(!n->IsAnObject(), "Item in range is not a NodeRenderableInk"); 

			AppliedAttr = ((NodeRenderableInk*)n)->GetChildAttrOfType(AttribType);
			
			if (!AppliedAttr)
			{
				// This can mean one of three things
				// a. All nodes in the compound which require the attribute share the default 
				//    attribute
				// b. Nodes requiring attribute have more than one attribute value
				// c. No nodes in the group require the attribute

				// Flag which is set to TRUE when we know that AttribType is required by the 
				// the compound
				BOOL AttribRequiredInCompound = FALSE; 

				// Scan the compound depth first
				Node* Current = n->FindFirstDepthFirst();
				
				while (Current != NULL)
				{
					if (!AttribRequiredInCompound)
					{
						// Determine if the attribute is required by the current node
						if (Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
						{
							AttribRequiredInCompound = ((NodeRenderableInk*)Current)
								->RequiresAttrib(AttribType); 	
						}
					}
					if (Current->IsAnAttribute())
					{
						if (((NodeAttribute*)Current)->GetAttributeType() == AttribType)
						{
							// This means that the compound must have more than one
							// attribute value, because if this attribute was common to
							// all objects requiring the attribute in the compound then
							// it would have been found in the child attribute block !
							
							if (!ForceAttribType)
							{
								(*pCommonType) = NULL; // Nothing useful to return here
								return ATTR_MANY;		  // 						*RETURN
							}
							else		// we want to try and force a common attribute ....
										// e.g.  if we have two shapes with a different linear
										// fill each and then group these, it is desirable to
										// still show these to the user as linear, and NOT change
										// (by default) to showing many ....
							{
								AppliedAttr = (NodeAttribute*)Current;
								
								if ((pAttribute) == NULL) // This is the first instance of the attribute found
								{
									(pAttribute) = AppliedAttr;	
								}
								else
								{
									// If the attribute found is not the same as all others found then 
									// we know that there is no common attribute, so there is no point in
									// searching any further
			
									if (! ((pAttribute->GetRuntimeClass()) == (AppliedAttr->GetRuntimeClass())) )
									{
										(*pCommonType) = NULL; // Nothing useful to return here
										return ATTR_MANY;		  // 									*RETURN
									}
	
									// Complicated by this special case.
									// If it's a Radial Fill Geometry, then we need to see if it's
									// circular or elliptical

									if ( pAttribute->IsKindOf(CC_RUNTIME_CLASS(AttrRadialFill)) )
									{
										if (! (((AttrRadialFill*)pAttribute)->IsElliptical()) == 
											(((AttrRadialFill*)AppliedAttr)->IsElliptical()) )
										{
											(*pCommonType) = NULL; // Nothing useful to return here
											return ATTR_MANY;	   // 									*RETURN
										}
									}

									// else the Applied attribute is the same as all others found so carry
									// on searching.
								}
							}
						}
					}
					Current = Current->FindNextDepthFirst(n); 		
				}
				if ((AttribRequiredInCompound) && (!ForceAttribType))
				{
					// The group uses an attribute further up the tree
					// scan left then up the tree in reverse render order until we 
					// find an instance of an AttribClass attribute.
					AppliedAttr = n->FindAppliedAttribute(AttribType, TRUE);	// bExcludeChildAttrs
				}
				// else attribute not required in compound so ignore it. 
			}
		}
		else
		{
			// Does n require attribute AttribType to render itself ?
			if (n->RequiresAttrib(AttribType))
			{
				// n needs to have attribute AttribType defined so that it can render properly
				// scan from the last child of n, up the tree in reverse render order until we 
				// find an instance of an AttribType attribute.
				AppliedAttr = n->FindAppliedAttribute(AttribType);
				
			}
			// else n does not require the attribute so it is ignored
		}
		
		// Determine if the attribute is common

		if (AppliedAttr != NULL)
		{

			if ((pAttribute) == NULL) // This is the first instance of the attribute found
			{
				(pAttribute) = AppliedAttr;	
			}
			else
			{
				// If the attribute found is not the same as all others found then 
				// we know that there is no common attribute, so there is no point in
				// searching any further
			
				if (! ((pAttribute->GetRuntimeClass()) == (AppliedAttr->GetRuntimeClass())) )
				{
					(*pCommonType) = NULL; // Nothing useful to return here
					return ATTR_MANY;		  // 									*RETURN
				}
	
				// Complicated by this special case.
				// If it's a Radial Fill Geometry, then we need to see if it's
				// circular or elliptical

				if ( pAttribute->IsKindOf(CC_RUNTIME_CLASS(AttrRadialFill)) )
				{
					if (! (((AttrRadialFill*)pAttribute)->IsElliptical()) == 
							(((AttrRadialFill*)AppliedAttr)->IsElliptical()) )
					{
						(*pCommonType) = NULL; // Nothing useful to return here
						return ATTR_MANY;	   // 									*RETURN
					}
				}

				// else the Applied attribute is the same as all others found so carry
				// on searching.
			}
		}
		 
		pn = (NodeRenderableInk*)FindNext(pn); // Obtain the next selected object
	}

	// Ok weve finished the scan, so either no objects require the attribute, or there
	// is a common attribute value shared by all objects that do require the attribute
	if ((pAttribute) != NULL)
	{
		*pCommonType = pAttribute->GetRuntimeClass();

		// If it's one of these silly circle things then
		// we'll have to return this 'virtual' class type.
		if ( pAttribute->IsKindOf(CC_RUNTIME_CLASS(AttrRadialColourFill)) )
		{
			if ( ((AttrRadialColourFill*)pAttribute)->IsCircular() )
				*pCommonType = CC_RUNTIME_CLASS(AttrCircularColourFill);
		}
		else if ( pAttribute->IsKindOf(CC_RUNTIME_CLASS(AttrRadialTranspFill)) )
		{
			if ( ((AttrRadialTranspFill*)pAttribute)->IsCircular() )
				*pCommonType = CC_RUNTIME_CLASS(AttrCircularTranspFill);
		}

		Result = ATTR_COMMON;
		// pAttribute will point to an instance of the common attribute
	}
	else
	{
		Result = ATTR_NONE;

		Document* SelectedDoc = Document::GetSelected();
	
		// If there is a selected document then pAttribute is set to the current attribute
		// for the selected tool.
		if (SelectedDoc != NULL)
		{
			*pCommonType = (SelectedDoc->GetAttributeMgr().GetSelToolCurrentAttribute(AttribType))->GetRuntimeClass();
		}
		// else pAttribute will be NULL	
	}
	return Result;
#else
	return ATTR_NONE;
#endif
}

/********************************************************************************************

>	CommonAttrSet* Range::GetCommonAttrCache()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Returns:	A pointer to the common attribute cache. NULL is returned if one could not
				be created because we ran out of memory.
	Purpose:	This fn should be called to get a pointer to the range's common attribute cache.
				If the cache does not exist in the range then one is first created.
	Scope:		private

********************************************************************************************/

CommonAttrSet* Range::GetCommonAttrCache()
{
	FreshenCache();			   // Cache may not be valid
	if (!pCommonAttribCache)
	{
		// Let's try and create one
		pCommonAttribCache = new CommonAttrSet(); 
	}
	FreshenCache(); 		   // Could delete the pCommonAttribCache if not cached
	return pCommonAttribCache; // Will be NULL if we ran out of memory
} 



/********************************************************************************************

>	Range::CommonAttribResult Range::FindCommonNonOptimisingAttr(CCRuntimeClass* pAttribClass,
																 NodeAttribute** ppAttr )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/2000
	Inputs:		pAttribClass
				ppAttr

	Outputs:	If returning ATTR_COMMON, then ppAttr is filled with a ptr to one of the
				common attributes, so that its values may be read by whoever called us.

	Returns:	ATTR_MANY	if we find more than one attribute of the given class, and they
							are different.
				ATTR_COMMON	if we find one or more attributes, and they are the same.
				ATTR_NONE	if we find no attributes of the given class.

	Purpose:	Do a search on the range, for non-optimising attrs.
				Because they are not optimised, we don't need to search up and down the tree
				for these attributes - we just look at the children of each node in the range.

	Notes:		This method *does* search all children of a node, so that it will detect
				multiple different attributes of the same type attached to one node.

	Errors:		ERROR2 with ATTR_NONE if either parameter is NULL.
	See also:	FindCommonAttribute().

********************************************************************************************/
Range::CommonAttribResult Range::FindCommonNonOptimisingAttr(	CCRuntimeClass* pAttribClass,
																NodeAttribute** ppAttr )
{
	ERROR2IF(ppAttr == NULL || pAttribClass == NULL, ATTR_NONE,
			"Range::FindCommonNonOptimisingAttr; NULL input parameter!");

	NodeAttribute* pAttr	= NULL;
	NodeAttribute* pFirstAttr	= NULL;

	Node* pNode = FindFirst();
	while (pNode != NULL)
	{
		if (pNode->IsAnObject())	// sanity check.
		{
			pAttr = (NodeAttribute*)pNode->FindFirstChild(pAttribClass);
			while (pAttr != NULL)
			{
				// remember the first match we have, so we can check for differences.
				if (pFirstAttr == NULL)
					pFirstAttr = pAttr;

				// if we have multiple matches, then check each one for differences.
				else if (pAttr->IsDifferent(pFirstAttr))
					return ATTR_MANY;

				pAttr = (NodeAttribute*)pAttr->FindNext(pAttribClass);
			}
		}

		pNode = FindNext(pNode);
	}

	if (pFirstAttr != NULL)
	{
		*ppAttr = pFirstAttr;
		return ATTR_COMMON;
	}
	else
	{
		return ATTR_NONE;
	}
}



/********************************************************************************************

>	BOOL Range::AddTextStoryCompoundsForDel(ObjectSet* pCompoundObjectSet)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function is really a BODGE. When we are deleting a range which contains 
				text chars, the deletion code may/may-not hide unselected TextLines as well. If 
				this occurs then we will not be able to globally localise/factor out attributes 
				on these TextLines. 

				So what this function does is to add all parent TextStory compounds of 
				any selected TextChars to the set, so that we can factor out on these. 

				Really any deletion of this sort should occur after the operation ends
				similar to group deletion in the no children case.But we have got to ship
				this product some time.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Range::AddTextStoryCompoundsForDel(ObjectSet* pCompoundObjectSet)
{
	Node* pCurrent = FindFirst();
	Node* pParent;
	while (pCurrent)
	{
		pParent = pCurrent->FindParent();
		ERROR3IF(!pParent, "object in range has no parent"); 
		if (pParent)
		{ 
			if (IS_A(pParent, TextLine))
			{
				pParent = pParent->FindParent(); 
				ERROR3IF(!pParent, "parent of object in range has no parent"); 
				ERROR3IF(!(IS_A(pParent, TextStory)), "Parent of TextLine is not a TextStory");
				if (IS_A(pParent, TextStory))
				{
					// Add the story to the set
					if (!(pCompoundObjectSet->AddToSet((NodeRenderableInk*)pParent)))
						return FALSE;  
				}
			}
		}
		pCurrent = pCurrent->FindNext(); 
 	}
	return TRUE; 
} 

/********************************************************************************************

	void Range::AttrsHaveChanged()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	This function gets called to inform the range that attributes applied to its 
				objects have, or may have changed. It deletes the Common attribute cache if 
				one exists.
	SeeAlso:	SelRange::AttrsHaveChanged

********************************************************************************************/

void Range::AttrsHaveChanged()
{
	// if we are currently caching current attrs delete them 
	if (pCommonAttribCache)
	{
		delete pCommonAttribCache;
		pCommonAttribCache = NULL;
	}
}

/********************************************************************************************

	BOOL Range::ConsistsOf(CCRuntimeClass* pccrtSearch, BOOL fIncludeDerivedClasses=TRUE)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/97
	Inputs:		pccrtSearch		The type of object to search for
				fIncludeDerivedClasses
								If FALSE, the region must consist only of objects of
								class pccrtSearch.

								If TRUE, the region must consist only of objects
								of pccrtSearch and derived classes.
				
	Purpose:	Determines whether the range consists entirely of the given
				class of object - or of derived classes of the given sort of object.

				This is used by the WebAddressDlg, which needs to know whether
				the selection consists entirely of text.


	SeeAlso:	Range::FindFirst, Range::FindNext, SelRange::FindFirst,
				Range::FindNext

********************************************************************************************/

BOOL Range::ConsistsOf(CCRuntimeClass* pccrtSearch, BOOL fIncludeDerivedClasses)
{
	//Find the first node in the range, not including children
	Node* pThisNode=FindFirst(FALSE);

	//Now scan until we get to the end of the range
	while (pThisNode!=NULL)
	{
		BOOL ok=TRUE;

		//Is the object allowed to be a derived class of pccrtSearch?
		if (fIncludeDerivedClasses)
			//Yes. So test to see whether the node is a derived class of pccrtSearch
			ok=pThisNode->IsKindOf(pccrtSearch);
		else
			//No. So test whether the node is exactly the same class as pccrtSearch
			ok=(pThisNode->GetRuntimeClass()==pccrtSearch);

		//If the node wasn't of the appropriate class, return FALSE now.
		if (!ok)
			return FALSE;

		//Get the next object to test
		pThisNode=FindNext(pThisNode, FALSE);
	}

	//We've reached the end of the range and all the nodes were of the class we wanted.
	//So return TRUE
	return TRUE;

}

/********************************************************************************************

	BOOL Range::IsEmpty()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/97
	Inputs:		-
	Returns:	TRUE if the range is empty
				
	Purpose:	Determines whether the range is empty


	SeeAlso:	-

********************************************************************************************/

BOOL Range::IsEmpty()
{
	return (FindFirst()==NULL);
}




/*********************************************************************************************

>    void Range::SetRenderable(BOOL bNewVis = FALSE)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   11/12/2003
     Inputs: 	bNewVis - New state of visibility flag on the nodes in this range
     Outputs:   -
     Returns:   -
     Purpose:   To mark the range as visible or invisible to the screen renderer
     SeeAlso:   Range::FindFirst
     SeeAlso:   Node::FindNextInRange
     Errors:    -

**********************************************************************************************/

void Range::SetRenderable(BOOL bNewVis)
{
	Node* pNode;

	pNode = FindFirst();
	while (pNode)
	{
		pNode->SetRender(bNewVis, TRUE);
//		if (pNode->IsAnObject()) ((NodeRenderableInk*)pNode)->ReleaseCached();

		pNode = FindNext(pNode);
	}
}




/*********************************************************************************************

>    Range* Range::CloneNodes(UINT32 timeLimit, BOOL bCloneOnTop, BOOL bLightweight)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   11/12/2003
     Inputs: 	timelimit		- if the clone doesn't complete within timelimit, back out
				bCloneOnTop		- Make the clones at the current insertpoint rather than next
								  to the originals
				bLightweight	- Don't do FULL copies of the nodes, assuming they will be
								  deleted rather than retained in the tree
     Outputs:   -
     Returns:   -
     Purpose:   Make a simple copy of the range, without making any undo actions for use
				during solid dragging.
     SeeAlso:   Range::FindFirst
     SeeAlso:   Node::FindNextInRange
     Errors:    -

**********************************************************************************************/

Range* Range::CloneNodes(UINT32 timeLimit, BOOL bCloneOnTop, BOOL bLightweight)
{
	Node* pNode;
	ListRange* pCloneRange = new ListRange();
	MonotonicTime timeClone;

	pNode = FindFirst();
	while (pNode && !timeClone.Elapsed(timeLimit))
	{
		// Make a copy of the current node
		Node* pCopy = NULL;
		BOOL bOK = FALSE;

//		bOK = pNode->CloneNode(&pCopy, bLightweight);
		bOK = pNode->NodeCopy(&pCopy);
		if (!bOK)
			goto CloneError;

		// make sure that it is bounded
		ERROR2IF(!pCopy->IsBounded(), FALSE, "Object being pasted is not a NodeRenderableBounded"); 
//		NodeRenderableBounded* BoundCopy = (NodeRenderableBounded*)pCopy;

		// Make sure the copy is not selected
		pCopy->SetSelected(FALSE);

		if (!bCloneOnTop)
		{
			// Insert the copied node right alongside the original
			pCopy->AttachNode(pNode, NEXT, FALSE, FALSE);
		}
		else
		{
			Node* pTail = pNode->FindParent(CC_RUNTIME_CLASS(Layer))->FindLastChild(TRUE);
			pCopy->AttachNode(pTail, NEXT, FALSE, FALSE);
		}

		// Add the copied node to the output ListRange
		pCloneRange->AddNode(pCopy);

		pNode = FindNext(pNode);
	}

//	If we ran through all the nodes we needed to then pNode will be NULL and we can exit normally
	if (pNode==NULL)
		return pCloneRange;
//	Else we aborted early and we must fall through to the tidy up routine and return NULL

CloneError:
	pNode = pCloneRange->FindLast();
	while (pNode)
	{
		pNode->CascadeDelete();
		delete pNode;

		pNode = pCloneRange->FindPrev(pNode);
	}

	delete pCloneRange;

	return NULL;
}



/*********************************************************************************************

>    void Range::DeleteNodes()
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/12/2003
     Inputs: 	-
     Outputs:   -
     Returns:   -
     Purpose:   Delete the nodes in the range, without making any undo actions for use
				during solid dragging.
     SeeAlso:   Range::FindFirst
     SeeAlso:   Node::FindNextInRange
     Errors:    -

**********************************************************************************************/

void Range::DeleteNodes()
{
	Node* pNode;
	Node* pNext;

	pNode = FindFirst();
	while (pNode)
	{
		pNext = FindNext(pNode);

		pNode->CascadeDelete();
		delete pNode;

		pNode = pNext;
	}
}




/*********************************************************************************************

>    void Range::TransformNodes()
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/12/2003
     Inputs: 	-
     Outputs:   -
     Returns:   -
     Purpose:   Transform the nodes in the range, without making any undo actions for use
				during solid dragging.
     SeeAlso:   Range::FindFirst
     SeeAlso:   Node::FindNextInRange
     Errors:    -

**********************************************************************************************/

BOOL Range::TransformNodes(TransformBase* Trans)
{
	// If the Trans is invertible then create a Transform node action to undo the transform
	// Note at this point Trans is not the inverse Transformation, but it will be soon !!
//	Node* pParent = NULL;
//	Node* pNodeToTransform = NULL;

	// Assume that the transformation is invertible
	// If we test for it below, the test explodes when the matrix scale factor is zero.
//	if (Trans->IsInvertable()) 
	{
		// Scan the range and transform each node 
		Node* Current = FindFirst();
		Node* Next; 
//		Node* Parent = NULL;
//		BOOL bTransform = FALSE;
		while (Current != NULL)
		{
			Next = FindNext(Current); 

			if (Current->IsNodeRenderableClass())
			{
				Trans->bHaveTransformedCached = TRUE;
				Trans->bHaveTransformedChildren = TRUE;

				((NodeRenderable*)Current)->Transform(*Trans);

				ERROR3IF(!Trans->bHaveTransformedCached && !Trans->bHaveTransformedChildren,
						 "Node::Transform has failed to either transform its children or transform its cached data"
						);

				ERROR3IF(Trans->bTransformYourChildren && !Trans->bHaveTransformedChildren,
						 "Node::Transform was told to transform its children but says it hasn't done so"
						);

				Trans->bHaveTransformedAllCached = Trans->bHaveTransformedAllCached && Trans->bHaveTransformedCached;
				Trans->bHaveTransformedAllChildren = Trans->bHaveTransformedAllChildren && Trans->bHaveTransformedChildren;

			}
			
			Current = Next;  
		}

	}
//	else // The Trans is not invertable
//	{
//		ERROR2(FALSE, "Range::TransformNodes can't do non-invertible transforms yet");
//	}

	return (TRUE); // Success 
}   




/*********************************************************************************************

>    void Range::SetDraggedNodes(BOOL bNewState)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/12/2003
     Inputs: 	bNewState - TRUE to mark nodes as currently being dragged
     Outputs:   -
     Returns:   -
     Purpose:   Set the Dragged flag on all the Bounded nodes in the range so that
				they can be treated differently than normal objects by routines which
				need to know.
     SeeAlso:   Range::FindFirst
     SeeAlso:   Node::FindNextInRange; CSnap::TryToSnapToObject
     Errors:    -

**********************************************************************************************/

void Range::SetDraggedNodes(BOOL bNewState)
{
	Node* pNode;

	pNode = FindFirst();
	while (pNode)
	{
//		if (pNode->IsBounded())
//			((NodeRenderableBounded*)pNode)->SetDraggedState(bNewState, TRUE);
		if (pNode->IsNodeRenderableClass())
			((NodeRenderable*)pNode)->SetDraggedState(bNewState, TRUE);

		pNode = FindNext(pNode);
	}
}




/*********************************************************************************************

>	void Range::CopyNodesContents(Range* pDestRange, BOOL bSetDragged = FALSE, BOOL bSetRenderable = FALSE)
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/2003
	Inputs: 	pDestRange - The destination range whose nodes must match those in this range
				EXACTLY!
	Outputs:	-
	Returns:	-
	Purpose:	Copy node contents from one range to another.
	SeeAlso:	Range::FindFirst
	SeeAlso:	Node::FindNextInRange
	Errors:		-

**********************************************************************************************/
void Range::CopyNodesContents(Range* pDestRange, BOOL bSetDragged, BOOL bSetRenderable)
{
	Node* pSrcNode;
	Node* pDestNode;
	NodeRenderable* pSrcRendNode;
	NodeRenderable* pDestRendNode;

//	ENSURE(GetCount()==pDestRange->GetCount(), "Ranges differ too much");

	pSrcNode = FindFirst(FALSE);
	pDestNode = pDestRange->FindFirst(FALSE);
	while (pSrcNode && pDestNode)
	{
		ENSURE(pSrcNode->GetRuntimeClass() == pDestNode->GetRuntimeClass(), "Ranges differ unexpectedly");
		if (pSrcNode->IsBounded() && pSrcNode->GetRuntimeClass() == pDestNode->GetRuntimeClass())
		{
			pSrcRendNode = (NodeRenderable*)pSrcNode;
			pDestRendNode = (NodeRenderable*)pDestNode;

			// Do the children because PolyCopyNodeContents can't sensibly be recursive
			Node* pSrcSubNode = pSrcRendNode->FindFirstDepthFirst();
			while (pSrcSubNode && (pSrcSubNode->IsNodeHidden() || pSrcSubNode->IsDragged()))					// URGH
				pSrcSubNode = pSrcSubNode->FindNextDepthFirst(pSrcRendNode);	// URGH

			Node* pDestSubNode = pDestRendNode->FindFirstDepthFirst();
			while (pDestSubNode && (pDestSubNode->IsNodeHidden() || !pDestSubNode->IsDragged()))				// URGH
				pDestSubNode = pDestSubNode->FindNextDepthFirst(pDestRendNode);	// URGH

			while (pSrcSubNode && pDestSubNode)
			{
				if (pSrcSubNode->IsNodeRenderableClass()
					&& pDestSubNode->IsNodeRenderableClass())
				{
					ENSURE(pSrcSubNode->GetRuntimeClass() == pDestSubNode->GetRuntimeClass(), "Ranges differ unexpectedly");
					((NodeRenderable*)pSrcSubNode)->PolyCopyNodeContents((NodeRenderable*)pDestSubNode);
					pDestSubNode->SetSelected(FALSE);
					pDestSubNode->SetParentOfSelected(FALSE);
				}

				pSrcSubNode = pSrcSubNode->FindNextDepthFirst(pSrcRendNode);
				while (pSrcSubNode && (pSrcSubNode->IsNodeHidden() || pDestSubNode->IsDragged()))					// URGH
					pSrcSubNode = pSrcSubNode->FindNextDepthFirst(pSrcRendNode);	// URGH

				pDestSubNode = pDestSubNode->FindNextDepthFirst(pDestRendNode);
				while (pDestSubNode && (pDestSubNode->IsNodeHidden() || !pDestSubNode->IsDragged()))				// URGH
					pDestSubNode = pDestSubNode->FindNextDepthFirst(pDestRendNode);	// URGH
			}

//			if (bSetDragged && pDestRendNode->IsBounded())
//				((NodeRenderableBounded*)pDestRendNode)->SetDraggedState(TRUE, TRUE);
			if (bSetDragged)
				pDestRendNode->SetDraggedState(TRUE, TRUE);
			if (bSetRenderable)
				pDestRendNode->SetRender(TRUE, TRUE);

		}

		pSrcNode = FindNext(pSrcNode, FALSE);
		pDestNode = pDestRange->FindNext(pDestNode, FALSE);
	}
}




/*****************************************************************************************
//
//	SelRange class implementation...
*/


/********************************************************************************************

>	static BOOL SelRange::DeclarePrefs()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/08/95
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Declares any preferences that the SelRange class needs to declare

********************************************************************************************/

BOOL SelRange::DeclarePrefs()
{
	BOOL ok = TRUE;

	// No preferences today, thank you!

	return ok;
}




/*****************************************************************************************

>   SelRange::SelRange()

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	16/2/94

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	SelRange constructor. Defines a SelRange to be a range containing selected 
				objects and that range is allowed to cross layers.

	Errors:		-

******************************************************************************************/

SelRange::SelRange() :	Range(NULL, NULL, RangeControl(TRUE,FALSE,TRUE,TRUE))	// Call Range constructor
{
	ScopeDocument = NULL;
	Cached = FALSE;
	IsGagged = FALSE;

	CachedBounds = FALSE; 		// Bounds not cached
	CachedBoundsNoAttrs = FALSE;	// Nor are bounds-without-attributes

	InformedSelChanged = FALSE; 
	CachedCount = 0;			// Number of objects in range is unknown
	LastFindNextNode = NULL;
//	SelectedSpread = NULL; 
	// There is no need to initialise CachedBBox, CachedBlobBBox, or CachedBBoxNoBounds

	pLastSelectedNode = NULL;	// We don't know of any last selected node yet
	pLastSelectedDoc  = NULL;

	MessageHandler = new SelRangeMessageHandler;

	m_pEffectsStack		= NULL;
	m_pEffectClassRange	= NULL;
	m_bEffectClassNodesOnly = FALSE;
	m_pAreaNode			= NULL;
}

/*****************************************************************************************

>   SelRange::SelRange()

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	16/2/94

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	SelRange constructor. Defines a SelRange to be a range containing selected 
				objects and that range is allowed to cross layers.

	Errors:		-

******************************************************************************************/
SelRange::SelRange(SelRange &Copy) : Range(Copy)
{
	ScopeDocument = Copy.ScopeDocument;
	Cached = Copy.Cached;
	IsGagged = Copy.IsGagged;

	CachedBounds = Copy.CachedBounds; 		// Bounds not cached
	CachedBoundsNoAttrs = Copy.CachedBoundsNoAttrs;	// Nor are bounds-without-attributes

	InformedSelChanged = Copy.InformedSelChanged; 
	CachedCount = Copy.CachedCount;			// Number of objects in range is unknown
	LastFindNextNode = Copy.LastFindNextNode;
//	SelectedSpread = NULL; 
	// There is no need to initialise CachedBBox, CachedBlobBBox, or CachedBBoxNoBounds

	pLastSelectedNode = Copy.pLastSelectedNode;	// We don't know of any last selected node yet
	pLastSelectedDoc  = Copy.pLastSelectedDoc;

	MessageHandler = new SelRangeMessageHandler;

	m_pEffectsStack			= NULL;
	m_pEffectClassRange		= NULL;
	m_bEffectClassNodesOnly	= FALSE;
	m_pAreaNode				= NULL;
}





SelRange::~SelRange()
{
	if (MessageHandler)
	{
		delete MessageHandler;
	}
}




/******************************************************************************************

>	void SelRange::SetRangeControl(RangeControl RangeControlFlgs)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/02/94
	
	Inputs:		RangeControlFlags: Specifies the properties of the nodes to be included
								   in the range. Or it would if we didn't ignore it.
	Outputs:	-
	Returns:	-
	Purpose:	To prevent the range controls of the SelRange subclass being altered.
				A call to this function has no effect (other than a TRACE warning msg)
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

void SelRange::SetRangeControl(RangeControl RangeControlFlgs)
{
	TRACE( _T("Warning: Calls to SelRange::SetRangeControl have no effect"));
}




/******************************************************************************************

>    Node* SelRange::FindFirst(BOOL AndChildren = FALSE)

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (& Jason) (Changed by Phil, 30/11/94)
     Created:   18/02/94
     
     Returns:   If the range contains any members then
					A pointer to the first node in the range is returned
				Else
					NULL is returned 
			
     Purpose:   The purpose of this function is to find the first node in a range. 

	 Notes:		This function, in combination with SelRange::FindNext, will attempt
				to cache useful information on the fly. This will occur if you scan the
				range in sequential order from start to finish.

     SeeAlso:   SelRange::FindNext; SelRange::FindLast

******************************************************************************************/

Node* SelRange::FindFirst(BOOL AndChildren)
{
	if (Document::GetSelected() == NULL)
	{
		Cached = FALSE;
		CachedBounds = FALSE;
		CachedBoundsNoAttrs = FALSE;
		if (pCommonAttribCache)
		{
			delete pCommonAttribCache;
			pCommonAttribCache = NULL; 
		}
		return(NULL);		 				    // No document so no selection
	}

	// If we're in a Document SelChanged message broadcast, somebody might ask us
	// for info before we get the msg: In this case, we flush the cache, so that
	// we don't return any out of date info
	if (Cached && Document::GetSelected() != ScopeDocument)
	{
		Update(FALSE, NULL);
		pLastSelectedNode = NULL;
	}

	// Call base class FindFirst function instead of more specialised and less flexible
	// one in Node. (When FirstNode is already known, calling this func instead of just
	// returning FirstNode allows children of it to be returned.)
	Node* pNode = Range::FindFirst(AndChildren);
	// Don't set FirstNode or LastNode yet, we'll set FirstNode below only if it's
	// not already been cached. LastNode will only be set when SelRange::FindNext
	// reaches the end of the range.

	if (!Cached || CachedCount < 0)			// If cached count is invalid/incomplete
	{
		Cached = FALSE;
		if (pCommonAttribCache)
		{
			delete pCommonAttribCache;
			pCommonAttribCache = NULL; 
		}

		CachedBounds = FALSE; 
		if (pNode == NULL)
		{
			// No objects in range. Note that in this case					
			// the BBoxes will be recalculated whenever   									
			// requested, so we don't need to touch them
			CachedCount = 0;
			FirstNode = NULL;
			LastNode = NULL;
		}
		else
		{
			// Range contains at least some members!
			//
			// Only allow primary members of the range to be counted and have their bounds
			// accumulated by setting the AndChildren flag FALSE on the InRange test...
			if ( InRange(pNode,FALSE) )
			{
				FirstNode = pNode;
				CachedBBox = ((NodeRenderableBounded*)FirstNode)->GetBoundingRect();
				CachedCount = -1;
			}
			else
				CachedCount = 0;
		}
	}

	return(LastFindNextNode = pNode); 
}




/******************************************************************************************

>    Node* SelRange::FindNext(Node* Prev, BOOL AndChildren = FALSE)

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (& Jason) (Changed by Phil, 30/11/94)
     Created:   18/02/94
    
     Inputs: 	Prev: The previous node in the range (usually returned from a 
     					Range::FindFirst, or a previous Range::FindNext call).
     Outputs:    - 
    
     Returns:   If the range contains any more members then
					A pointer to the next node in the range is returned
					(NOTE this does not return members of a selected group object)
				Else
					NULL is returned 

     Purpose:   The purpose of this function is to find the next node in the range after
     			Prev.

	 Notes:		This function, in combination with SelRange::FindFirst, will attempt
				to cache useful information on the fly. This will occur if you scan the
				range in sequential order from start to finish.

     SeeAlso:   SelRange::FindFirst; Range::findNext
     
     Errors:    An assertion failure will occur if:
     				There are no more nodes to search and the last node to search was not found.
					The pointer passed in is NULL

	 Technical Notes:
				This function attempts to cache as much useful information as possible.
				After reaching the last node of a range, it has cached the first and last
				pointers. If the client progressed through the range in a sequential fashion
				(each 'Previous' pointer passed in was returned by the previous FindNext call)
				then a cached count of the number of objects is also recorded. If this is not
				the case, then the count will be set to zero, which forces the Count() function
				to re-scan the entire range to determine the count. Note that during this process
				CachedCount is zero if the count is invalid, or a NEGATIVE accumulated count of
				number of objects counted so far. If the value is positive, it is already cached.
				While the CachedCount is valid, an overall Bounding Box will also be cached.

******************************************************************************************/

Node *SelRange::FindNext(Node *Previous, BOOL AndChildren)
{
	// Preconditions
//	ERROR2IF(Previous==NULL,NULL,"NULL pointer passed to SelRange::FindNext");

	NodeRenderableBounded* pNext;

	if (Previous == NULL)
		return(LastFindNextNode = NULL);

	if (CachedCount < 0 && Previous != LastFindNextNode)	// If accumulating a count, and 
		CachedCount = 0;									// 	not progressing sequentially through
															// 	the range, abandon the cached count.

	// Call base class FindNext...
	pNext = (NodeRenderableBounded*) Range::FindNext(Previous, AndChildren);

	if (!Cached)
	{
		if (pNext != NULL)						// 	If there is another node
		{
			if (CachedCount < 0 && InRange(pNext,FALSE))
			{
				CachedCount--;					// 	and if count still valid, accumulate it
				CachedBBox = CachedBBox.Union(pNext->GetBoundingRect());
			}
		}
		else
		{
			if (CachedCount < 0)
			{									// If there were no more selected nodes
				Cached = TRUE;					// then Last is now true last object ptr
				CachedCount = -CachedCount;		// and count (if any) is now valid
				LastNode = Previous;			// Know that Previous was last, so record that info
			}
		}
	}

	return ( LastFindNextNode = (Node *) pNext );	// Return, remembering the return value
}


/******************************************************************************************

>    Node* SelRange::FindLast()

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   02/03/94
     
     Returns:   If the range contains any members then
					A pointer to the last node in the range is returned
				Else
					NULL is returned 
			
     Purpose:   The purpose of this function is to find the last node in a SelRange. 

	 Notes:		If the LastNode value is not cached, then this will necessitate a scan
				of the entire selction range to determine (and cache) the LastNode.

     SeeAlso:   SelRange::FindFirst;
     
******************************************************************************************/

Node* SelRange::FindLast()
{
	FreshenCache();						// Ensure the cache is valid
	return(LastNode); 					// Return cached LastNode value
}



/******************************************************************************************

>    Node* SelRange::FreshenCache()

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (with tattered remnants of Phil's code in there somewhere)
     Created:   02/03/94
    
     Inputs: 	-
     Outputs:   - 
     Returns:   - 

     Purpose:	Internal call for caching useful information
     			Determines if the SelRange information cache is valid, and if not, scans
				the entire range to accumulate that information, and freshens the cache.
										    
	 Notes:		After a call to this function, all possible SelRange information will have
				been cached. The cache is invalidated when the selection changes.
				This routine just makes repeated calls to SelRange::FindFirst and FindNext
				because they will cache any missing information.

     SeeAlso:   SelRange::FindFirst; SelRange::FindNext
     
     Errors:    -

******************************************************************************************/

void SelRange::FreshenCache()
{
	BOOL NoDocument = (Document::GetSelected() == NULL);
	Node* pNode = NULL;

	if (NoDocument || (Cached && Document::GetSelected() != ScopeDocument))
	{
		Cached = FALSE;
		
		if (pCommonAttribCache)
		{
			delete pCommonAttribCache;
			pCommonAttribCache = NULL; 
		}

		CachedBounds = FALSE; 
		FirstNode = LastNode = NULL;
		CachedCount = 0;
		CachedBBox.MakeEmpty();
		CachedBlobBBox.MakeEmpty();
		pLastSelectedNode = NULL;
		pLastSelectedDoc  = NULL;
	}

	if (!Cached || CachedCount <= 0 || FirstNode==NULL)		// If count not cached, or is invalid
	{
		BOOL bOldValue = RangeControlFlags.PromoteToParent;
		RangeControlFlags.PromoteToParent = TRUE;

		if ((!Cached && !NoDocument) || FirstNode==NULL)	// If FirstNode not cached, find it
			pNode = FindFirst();			// Call own find first in SelRange function

		if (FirstNode != NULL)
		{
			while (pNode)						// Scan the range...
			{
				pNode = FindNext(pNode);		// by calling own find next in SelRange function
			}
		}
		else
		{
			// No objects - Ensure that the bounding boxes are set to 'empty'
			CachedBBox.MakeEmpty();
			CachedBlobBBox.MakeEmpty();

			CachedBoundsNoAttrs = FALSE;
			CachedBBoxNoAttrs.MakeEmpty();

			LastNode = NULL;
		}

		RangeControlFlags.PromoteToParent = bOldValue;

		if (!NoDocument && FirstNode!=NULL)
		{
			Cached = TRUE;			// All useful info possible has been cached
			CachedBounds = TRUE; 
		}
	}


// This seems to serve no useful purpose other than bringing up an Ensure box
// in the middle of some perfectly valid situations.
//#ifdef _DEBUG
//	if (NoDocument)
//		ERROR3("SelRange called when there is no SelectedDocument!");
//#endif
}




/******************************************************************************************

>    UINT32 SelRange::Count()

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   18/02/94
     
     Inputs: 	-
     Outputs:   -
     
     Returns:   The number of nodes described by the SelRange (may be zero)

     Purpose:   To count the number of nodes in a range.

	 Notes:		After a call to this function, all possible SelRange information will have
				been cached. The cache is invalidated when the selection changes.

     Errors:    -

******************************************************************************************/

UINT32 SelRange::Count()
{
	// DMc 18/5/99
	// if PromoteToParent is set, then I think we need to recalc this
	Node * pNode = FindFirst();
	
	INT32 count = 0;
	
	while (pNode)
	{
		count ++;
		pNode = FindNext(pNode);
	}
	
	CachedCount = count;
	
	return(CachedCount);		// Return the cached count
}



/********************************************************************************************

>	void SelRange::FreshenBoundsCache()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function ensures that the 	CachedBBox and CachedBlobBBox are up-to-date
				
	Scope:		private
	Errors:		-
	SeeAlso:	SelRange::UpdateBounds

********************************************************************************************/

void SelRange::FreshenBoundsCache()
{
	FreshenCache();		// Ensure the cache is valid
	if (!CachedBounds)
	{
		// Scan the range and calculate the bounds
		CachedBBox.MakeEmpty();
		CachedBlobBBox.MakeEmpty();

		BOOL bOldValue = RangeControlFlags.PromoteToParent;
		RangeControlFlags.PromoteToParent = TRUE;
		Node* Current = FindFirst();
		while (Current != NULL)
		{
			CachedBBox = CachedBBox.Union(((NodeRenderableBounded*)Current)->GetBoundingRect());
			Current = FindNext(Current);
		}
		RangeControlFlags.PromoteToParent = bOldValue;
		CachedBounds = TRUE;
	}
}



/******************************************************************************************

>    DocRect SelRange::GetBoundingRect(BOOL bPromoteToControllers = FALSE);

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   02/03/94
         
     Inputs: 	-
     Outputs:   -
     
     Returns:   The bounding rectangle of the current selection. If nothing is selected this
				will be an empty rectangle.
				Note that rather than using DocRect::IsEmpty, it may be slightly more efficient
				to check the empty case with  if (SelRange::Count() == 0),  as the SelRange
				caching mechanism results in this being an efficient call. This may also make
				your code more readable.

     Purpose:   To find the bounding box of all objects in a selection range.

	 Notes:		After a call to this function, all possible SelRange information will have
				been cached. The cache is invalidated when the selection changes.

     SeeAlso:   SelRange::GetBlobBoundingRect

     Errors:    -

******************************************************************************************/

DocRect SelRange::GetBoundingRect(BOOL bPromoteToControllers)
{
	FreshenBoundsCache();		// Ensure the cache is valid

// This strange stuff is needed because FreshenBoundsCache doesn't necessarilly
// loop through the node in this range
// Phil 27/10/2004
//
	BOOL bOldValue = SetPromoteToParent(bPromoteToControllers);

	// Calculate the union of all nodes within the range 
	DocRect Bounds; 
	NodeRenderableBounded* pNode = (NodeRenderableBounded*) FindFirst(FALSE);
	while (pNode != NULL)
	{
		Bounds = Bounds.Union(pNode->GetBoundingRect());
		pNode =(NodeRenderableBounded*)FindNext(pNode, FALSE);
	} 

	CachedBBox = Bounds;
	
	SetPromoteToParent(bOldValue);

	return(CachedBBox);			// Return cached bounding box
}




/******************************************************************************************

>	DocRect SelRange::GetBoundsWithoutAttrs(void)

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   23/02/95
         
     Returns:   The bounds of the selection, not including the effects of attributes
				(e.g. line widths, etc)

     Purpose:   To find the bounding box of all objects in a selection range, not
				including the effects of any attributes applied thereto.

				This is used (mainly in the selector tool) to provide things like
				the width and height of objects, exclusive of line width.

	Notes:		The returned bounding box will be "Empty" if there is no selection.

******************************************************************************************/

DocRect SelRange::GetBoundsWithoutAttrs(void)
{
	// If we happen to have already got this information, then it'll be cached,
	// but otherwise, we'll have to go and find out.
	if (!CachedBoundsNoAttrs)
	{
		// Set a reasonable default state in case of failure
		CachedBBoxNoAttrs.MakeEmpty();

		// Scan the range, accumulating the bounds. GetBoundingRect is called with a TRUE
		// parameter in order to get the bounds exclusive of attributes.
		Node *pNode = FindFirst();

		if (pNode != NULL)
		{
			// Set the bounds to the bounds of the first object - who can tell what we'll
			// get if we union an "empty" rectangle with another.
			CachedBBoxNoAttrs = ((NodeRenderableBounded*)pNode)->GetBoundingRect(TRUE);
			pNode = FindNext(pNode);

			while (pNode != NULL)
			{
				CachedBBoxNoAttrs = CachedBBoxNoAttrs.Union( ((NodeRenderableBounded*)pNode)->GetBoundingRect(TRUE) );
				pNode = FindNext(pNode);
			}
		}

		// And remember that we now have a cached set of bounds
		CachedBoundsNoAttrs = TRUE;
	}

	return(CachedBBoxNoAttrs);
}



/******************************************************************************************

>    DocRect SelRange::GetBlobBoundingRect(void);

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   03/03/94
         
     Inputs: 	-
     Outputs:   -
     
     Returns:   The bounding rectangle of the current selection, including the selection blobs
     			If nothing is selected this will be an empty rectangle. 
				Note that rather than using DocRect::IsEmpty, it may be slightly more efficient
				to check the empty case with  if (SelRange::Count() == 0), as the SelRange
				caching mechanism results in this being an efficient call. This may also make
				your code more readable.

     Purpose:   To find the bounding box of all objects in a selection range, including their
     			selection blobs.

	 Notes:		After a call to this function, all possible SelRange information will have
				been cached. The cache is invalidated when the selection changes.

     SeeAlso:   SelRange::GetBoundingRect

     Errors:    -

******************************************************************************************/

DocRect SelRange::GetBlobBoundingRect(void)
{
	// Get some rectangles
	DocRect BlobRect;
	DocRect RealRect;

	// Make sure they are empty
	BlobRect.MakeEmpty();
	RealRect.MakeEmpty();

	// Loop through the selection
	BOOL bOldValue = RangeControlFlags.PromoteToParent;
	RangeControlFlags.PromoteToParent = TRUE;

	Node* Current = FindFirst();
	while (Current != NULL)
	{
		ERROR3IF(!Current->IsBounded(), "Non-bounded node in selection in GetBlobBoundingRect\n");
		if (Current->IsBounded())
		{
			NodeRenderableBounded* pBounded = (NodeRenderableBounded*)Current;

			// Get the real bounding rect, since this is worked out anyway
			RealRect = RealRect.Union(pBounded->GetBoundingRect());

			// Get the blob bounding rect
			BlobRect = BlobRect.Union(pBounded->GetBlobBoundingRect());
		}

		// Get the next node in the list
		Current = FindNext(Current);
	}

	// Make sure the basic selection's blob bounds are included (without doing
	// fully recursive scan of the selection)
	RangeControlFlags.PromoteToParent = FALSE;

	Current = FindFirst();
	while (Current != NULL)
	{
		ERROR3IF(!Current->IsBounded(), "Non-bounded node in selection in GetBlobBoundingRect\n");
		if (Current->IsBounded())
		{
			NodeRenderableBounded* pBounded = (NodeRenderableBounded*)Current;

			// Get the blob bounding rect
			BlobRect = BlobRect.Union(pBounded->GetBlobBoundingRect());
		}

		// Get the next node in the list
		Current = FindNext(Current);
	}

	RangeControlFlags.PromoteToParent = bOldValue;

	// It the cached bounds was out of date, we can use the one we have just built
	if (!CachedBounds)
	{
		// Update the cached rectangle and make it as valid
		CachedBBox = RealRect;
		CachedBounds = TRUE;
	}

	// return the blob bounding rect
	return BlobRect;
}



#ifdef SELECTION_AREA_FEATURE
/******************************************************************************************

>    BOOL SelRange::GetAreaDetails(XLONG* pxlArea, XLONG* pxlPerim)

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/03/2005
     Inputs: 	-
     Outputs:   -
     Returns:   The area and perimiter length of the current selection. If nothing is selected these
				will be zero.
     Purpose:   To find the area details of all objects in a selection range.
     SeeAlso:   SelRange::GetBoundingRect
     Errors:    -

******************************************************************************************/

BOOL SelRange::GetAreaDetails(XLONG* pxlArea, XLONG* pxlPerim)
{
	FreshenBoundsCache();		// Ensure the cache is valid

// This strange stuff is needed because FreshenBoundsCache doesn't necessarilly
// loop through the node in this range
// Phil 27/10/2004
//
	BOOL bOK = TRUE;
	XLONG xlArea = 0; 
	XLONG xlPerim = 0;
	BOOL bOldValue = SetPromoteToParent(FALSE);

	// Calculate the union of all nodes within the range 
	Node* pNode = FindFirst(FALSE);
	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			NodeRenderableInk* pInkNode = (NodeRenderableInk*)pNode;
			bOK = pInkNode->GetAreaDetails(&xlArea, &xlPerim);
			*pxlArea += xlArea;
			*pxlPerim += xlPerim;
		}

		pNode = FindNext(pNode, FALSE);
	} 

	SetPromoteToParent(bOldValue);

	return TRUE;
}




/******************************************************************************************

>    BOOL SelRange::GetAreaDetailsWhileIdle(XLONG* pxlArea, XLONG* pxlPerim)

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   28/03/2005
     Inputs: 	-
     Outputs:   -
     Returns:   The area and perimiter length of the current selection. If nothing is selected these
				will be zero.
     Purpose:   To find the area details of all objects in a selection range.
     SeeAlso:   SelRange::GetBoundingRect
     Errors:    -

******************************************************************************************/

BOOL SelRange::GetAreaDetailsWhileIdle(XLONG* pxlArea, XLONG* pxlPerim)
{
	FreshenBoundsCache();		// Ensure the cache is valid

// This strange stuff is needed because FreshenBoundsCache doesn't necessarilly
// loop through the node in this range
// Phil 27/10/2004
//
	BOOL bOK = TRUE;
	XLONG xlArea = 0; 
	XLONG xlPerim = 0;
	BOOL bOldValue = SetPromoteToParent(FALSE);
	MonotonicTime Slice;							// Sample time now

	// Calculate the union of all nodes within the range
	Node* pNode = m_pAreaNode;
	if (pNode==NULL)
	{
		pNode = FindFirst(FALSE);
		*pxlArea = 0;
		*pxlPerim = 0;
	}

	while (pNode && !Slice.Elapsed(50))
	{
		if (pNode->IsAnObject())
		{
			NodeRenderableInk* pInkNode = (NodeRenderableInk*)pNode;
			bOK = pInkNode->GetAreaDetails(&xlArea, &xlPerim);
			*pxlArea += xlArea;
			*pxlPerim += xlPerim;
		}

		pNode = FindNext(pNode, FALSE);
	}

	m_pAreaNode = pNode;

	SetPromoteToParent(bOldValue);

	return (pNode!=NULL);
}
#endif




/******************************************************************************************

>	Node *SelRange::GetLastSelectedNode(void)

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   04/01/95
         
     Inputs: 	-
     Outputs:   -
     
     Returns:   NULL, or a pointer to the last node that was selected

     Purpose:   To find the last node that was selected. This may be only a short-term
	 			memory (i.e. any update of the selection cache will force us to forget
				the node, so the info may only be available for a short time after the
				node is selected).

				This information is used by entities such as the colour editor (which
				needs to know the last object selected when it recieves a SelChanged
				message, so it can make an intelligent guess at a good colour to edit)

     SeeAlso:   SelRange::Update

     Errors:    -

******************************************************************************************/

Node *SelRange::GetLastSelectedNode(void)
{
	if (pLastSelectedNode != NULL)							// If we know a last-selected node
	{
		if (!pLastSelectedNode->IsSelected() ||				// ...& is no longer selected...
			pLastSelectedDoc != Document::GetSelected())	// ...or was in a different doc...
		{			
			pLastSelectedNode = NULL;						// ...then forget about it
			pLastSelectedDoc  = NULL;
		}
	}

	return(pLastSelectedNode);
}



/******************************************************************************************

>	void SelRange::Update(BOOL TellWorld = FALSE, Node *LastSelectedNode)

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Changed by Phil, 30/11/94)
     Created:   18/5/94	(Was an inline function prior to this)

     Inputs: 	BOOL TellWorld: When TRUE the function will inform the world that the 
	 			selection has changed. Normally	the selection is changed in an operation
				and it is unneccesary to broadcast a SelChangingMsg at this point because
				this will occur at the end of the operation. However If the selection is
				changed outside of an operation then the flag should be set to TRUE.  

				LastSelectedNode - NULL (the default), or a pointer to the node being
				selected. This is remembered for return by GetLastSelectedNode, which
				is used by entities such as the colour editor to try to show a sensible
				colour for editing.
     Outputs:   -
     Returns:   -

     Purpose:   To inform the SelRange that the selection has changed
				This invalidates the SelRange's selection-info cache so it will be
				recached when info is next requested.

	 SeeAlso:	SelRange::GetLastSelectedNode; SelRange::UpdateBounds

******************************************************************************************/

void SelRange::Update(BOOL TellWorld, Node *LastSelectedNode)
{
	Cached 			 	= FALSE;	
	CachedCount			= 0;
	InformedSelChanged	= FALSE; 
	FirstNode			= NULL;
	LastNode			= NULL;
	CachedBounds 		= FALSE;
	CachedBoundsNoAttrs	= FALSE;

	LastFindNextNode	= NULL;		// Ensure all on-the-fly FindNext caching is wiped

	// destroy the CommonAttribute cache, a new one will be created the next time
	// a user calls the 
	if (pCommonAttribCache)
	{
		delete (pCommonAttribCache); 
		pCommonAttribCache = NULL;
	}

	// Get rid of any postprocessor stack that has been created
	// Note that this means that people using the PostPro stack at times when Update is likely to be called
	// must make local copies and NOT rely on this cached copy!
	// And there's no such thing as a "Fog Chicken"
	if (m_pEffectsStack)
	{
		delete m_pEffectsStack;
		m_pEffectsStack = NULL;
	}
	if (m_pEffectClassRange)
	{
		delete m_pEffectClassRange;
		m_pEffectClassRange = NULL;
	}

	m_pAreaNode = NULL;

	// Remember the last node which was selected (or if it's NULL, forget the last
	// node, because we cannot any longer guarantee (a) that it is still selected, or
	// (far more importantly) (b) if it still exists
	if (LastSelectedNode != NULL)
	{
		pLastSelectedNode = LastSelectedNode;
		pLastSelectedDoc  = Document::GetSelected();
	}

	// Inform the DialogBars that the system state has changed so that they can refresh
	// themselves
	DialogBarOp::SetSystemStateChanged(); 

	if (TellWorld)
	{
		// Broadcast message/ update status bar etc.
		SelChangedCommit(); 		
	}
}

/********************************************************************************************

>	EffectsStack* SelRange::GetEffectsStack(BOOL bGetCopy = FALSE, BOOL bEscapeDerived = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2004
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

EffectsStack* SelRange::GetEffectsStack(BOOL bGetCopy, BOOL bEscapeDerived)
{
	if (bGetCopy)
		return EffectsStack::GetEffectsStackFromSelection(this, bEscapeDerived);

	if (m_pEffectsStack==NULL)
		m_pEffectsStack = EffectsStack::GetEffectsStackFromSelection(this, bEscapeDerived);

	return m_pEffectsStack;
}


/********************************************************************************************

>	ListRange* SelRange::GetTopClassRange(CCRuntimeClass* pClass, BOOL bClassOnly, BOOL bIgnorePassThroughEffects, Node** ppMasterNode, INT32* piStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/2005
	Inputs:		pClass - pointer to class descriptor for class we're looking for
	Outputs:	pMasterNode - pointer to master node (sample node from consistent range) or NULL
				iStackPos - index into effect stack or STACKPOS_INVALID if range not valid in stack
	Returns:	-
	Purpose:	-
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ListRange* SelRange::GetTopClassRange(CCRuntimeClass* pClass, BOOL bClassOnly, BOOL bIgnorePassThroughEffects, Node** ppMasterNode, INT32* piStackPos)
{
	CCRuntimeClass* pEffectClass = NULL;
	Node* pFirstEffectNode = NULL;

	// First get the type of the nodes in the cached range
	if (m_pEffectClassRange)
		pFirstEffectNode = m_pEffectClassRange->FindFirst();
	if (pFirstEffectNode)
		pEffectClass = pFirstEffectNode->GetRuntimeClass();

	if (m_pEffectClassRange==NULL || pClass!=pEffectClass || m_bEffectClassNodesOnly!=bClassOnly)
	{
		if (m_pEffectClassRange)
			delete m_pEffectClassRange;

		m_pEffectClassRange = EffectsStack::GetTopClassRange(pClass, bClassOnly, bIgnorePassThroughEffects, ppMasterNode, piStackPos, this, !bIgnorePassThroughEffects);
		m_bEffectClassNodesOnly = bClassOnly;
	}
	else
	{
		if (ppMasterNode)
			*ppMasterNode = pFirstEffectNode;
		if (piStackPos)
			*piStackPos = STACKPOS_INVALID;
	}

	return m_pEffectClassRange;
}


/********************************************************************************************

>	void SelRange::UpdateBounds()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	To inform the SelRange that its bounds have changed
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelRange::UpdateBounds()
{
	CachedBounds		= FALSE;
	CachedBoundsNoAttrs	= FALSE;

// Removed by Jason, 23/10/95
// After discussion with Simon, this has been removed, as we reckon it's completely
// rampant. The selection has not been changed, but the bounds of the selected item(s)
// have changed. Perhaps there should be a notification broadcast, but it should NOT
// be the SelChangingMsg::SELECTIONCHANGED message that this flag triggers!
//	InformedSelChanged	= FALSE; 


#if !defined(EXCLUDE_FROM_RALPH)
	// Inform the DialogBars that the system state has changed so that they can refresh
	// themselves

	DialogBarOp::SetSystemStateChanged(); 
#endif
}



/********************************************************************************************

>	void SelRange::BroadcastAnyPendingMessages(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/95

	Purpose:	Ensures that any pending SelChangingMsg broadcasts are sent immediately.
				This should be called by any code which calls SelRange::Update with the
				default FALSE (don't broadcast yet) flag parameter, to ensure that soon
				after the change the delayed message broadcast occurs.

	Note:		Any pending message is automatically sent when an Op ends. Thus, you
				only need to call this code directly when you update the selection
				outside an Op. (e.g. when the selected document changes, etc)

	SeeAlso:	SelRange::Update()

********************************************************************************************/

void SelRange::BroadcastAnyPendingMessages(void)
{
	// If we're not gagged, and we have any pending message broadcast, do it
	if (!IsGagged && !InformedSelChanged)
		SelChangedCommit();
}



/********************************************************************************************

>	void SelRange::SelChangedCommit(); 


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This private fn gets called to inform the world that the selection has changed
				/ update the status bar etc.
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

void SelRange::SelChangedCommit() 
{
	BOOL DifferentDoc = (ScopeDocument != Document::GetSelected());

	// Tell the world that the selection has changed
	if (!IsGagged)
	{
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::SELECTIONCHANGED, DifferentDoc));
		InformedSelChanged = TRUE;  

		// Update the status bar description of the selection
		DescribeSelectionInStatusBar(); 

	}

	// Update our idea of the current scope document
	ScopeDocument = Document::GetSelected();

	Node* pNode = FindFirst();
	if (pNode != NULL)
	{
		// Set the selected spread to be the parent spread of the first node in the selection.
		pNode = pNode->FindParent(CC_RUNTIME_CLASS(Spread));
		ERROR3IF(pNode == NULL,"first node in selection doesn't have a parent spread!");

		// Ensure selected Doc, DocView, and Spread are legal
		// (If pNode is NULL, it will try to sort out our life for us)
		Document::SetSelectedViewAndSpread(ScopeDocument, NULL, (Spread *)pNode);
	}
}



/********************************************************************************************
>	BOOL SafeAppendString(StringBase* pstrAppendum, const StringBase& strAppendee,
						  BOOL fAddSep = TRUE)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/95
	Inputs:		pstrAppendum		pointer to the string to append to
				strAppendee			the string to append
				fAddSep				if TRUE (the default) will append a space after
									strAppendee string.
	Outputs:	pstrAppendum		the appended string
	Purpose:	Safe way to append one string to another without risking overflow.  If
				an overflow would occur the appending isn't done.
	Returns:	TRUE if it was trivial to append, FALSE if the string to be appended had
				to be truncated.
	SeeAlso:	SelRange::LayerDescription
********************************************************************************************/

BOOL SafeAppendString(StringBase* pstrAppendum, const StringBase& strAppendee, BOOL fAddSep = TRUE)
{
	INT32 nTotal = pstrAppendum->Length() + strAppendee.Length() + (fAddSep != 0);
	INT32 nMax = pstrAppendum->MaxLength();
	BOOL fNoTrunc= (nTotal < nMax);
	if (fNoTrunc)
	{
		// We can append without fear of overflow . . .
		*pstrAppendum += strAppendee;
		if (fAddSep) *pstrAppendum += TEXT(" ");
	}
	else
	{
		// We have to truncate the string to append, to avoid overflow . . .
		INT32 nTruncCount = strAppendee.Length() - (nTotal - nMax);
		if (nTruncCount > 0)
		{
			// We have some room for more text, so append the truncated string.
			String_256 strTrunc;
			strAppendee.Left(&strTrunc, nTruncCount);
			*pstrAppendum += strTrunc;
			if (fAddSep) *pstrAppendum += TEXT(" ");
		}
	}

	// Return FALSE if we had to truncate.
	return fNoTrunc;
}




/***********************************************************************************************

>	String_256 SelRange::Describe(DescriptionFormat Format) const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93     
	    
	Inputs:		DescriptFormat: The format of the description generated. Currently there are 
		        two types of description which can be generated. 
		        
		        When DescriptFormat = MENU 
		        
		        	A description suitable for displaying on a menu is generated. The 
			        	description will have the form:
		        	
		        	"rounded rectangle"  // For a single rounded rectangle object 
		        						 //	in the range. 
		        	"rounded rectangles" // For multiple rounded rectangles in the range. 
		        	"objects"			 // For multiple objects of different types 
		        						 // in the range. 

					If the range conatains only group nodes	which all have the same name
					then the Group name is used for the description i.e

					"bannana"			 // For a single group with the name bananna
					"bannanas"			 // For multiple groups all with the name bananna
		        	
		        When DescriptionFormat = INFOBAR
		        
		        	A description suitable for the infobar is generated. The 
		        	description will be the same as that generated by the MENU 
		        	DescriptFormat, except that the descriptions will be prefixed by 
		        	the number of objects in the range, and will contain a layer description

					"1 rounded rectangle on layer Blobby"
					"5 rounded rectangles on layer Blobby 
		        	"5 bannanas on 4 layers"

					if one item is selected, and is inside, then the description will look like
					this

					"1 bannana 'inside' on layer Blobby"

					If there is more than one item selected and at least one item is inside
					then the description will have the word 'inside' appended to it, like this

		        	"5 bannanas on 4 layers ('inside')"

	Outputs:	-
	Returns:	If there are any members in the range then a description of the members
				  in the range is returned, else an empty string is returned. 
				  
	Purpose:    This routine returns a description of a range of objects.             
	
	Errors:	  	A resource exception will be thrown if a problem occurs when loading a
				string resource. 
	
	SeeAlso:	-

***********************************************************************************************/

String_256 SelRange::Describe(DescriptionFormat Format) 
{
	String_256 str = ObjectDescription(Format);
	SafeAppendString(&str, LayerDescription(Format), FALSE);
	return str;
}




/********************************************************************************************

>	String_64 SelRange::ObjectDescription(DescriptionFormat Format)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns a string describing the objects in the selection, see 
				SelRange::Describe for a description of the format
	Errors:		-
	Scope:		private
	SeeAlso:	SelRange::Describe
	SeeAlso:	SelRange::LayerDescription

********************************************************************************************/

String_64 SelRange::ObjectDescription(DescriptionFormat Format)
{
	String_256 Description;
	Node* Current = FindFirst();

	// Implementation
	if (Current != NULL)                                      // There are objects in range
	{                                            
		// Use shortended description for Menus
		BOOL Verbose = (Format != MENU);

		Node* FirstInRange = Current; 

		// Simon - 17/08/95. Pre Quickshapes the NodeType was the runtime class of the object
		// we now use description strings. This should still be quick cos most will be 
		// identifiable by the first char.
		String NodeType = Current->Describe(FALSE, Verbose); // Use singular  
		
		// --------------------------------------------------------------------------------
		// Determine if there is more than one type of object selected

		BOOL OneTypeOfObject = TRUE;                          // TRUE if range contains only
															  // one type of object. 

		while ((Current = FindNext(Current)) != NULL)
		{   
			if (Current->Describe(FALSE, Verbose) != NodeType)	      // Range contains more than one
															  // type of object
			{
				// Current object is not the same type as the first so we know there is more
				// than one type of object.
				OneTypeOfObject = FALSE;
				break;
			}
		}     
		
		UINT32 NumObjectsInRange = Count(); 					 // Count is cached
		ERROR3IF(NumObjectsInRange <= 0, "No objects selected"); 

		if (OneTypeOfObject)
		{
			// All objects in the selection are of the same type

			String_32 Name;
			BOOL UseClassToDescribe	= TRUE;

			NodeGroup* FirstGroup = NULL; 

			if (FirstInRange->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeGroup))
			{
				UseClassToDescribe = FALSE; 
				// If all selected groups have the same name then the name is used in the 
				// description
				FirstGroup = (NodeGroup*)FirstInRange;
				NodeGroup* CurrentGroup = FirstGroup;
				Name = CurrentGroup->GetName(); 
				if (!(Name.IsEmpty()))
				{
					do
					{
						CurrentGroup=(NodeGroup*)FindNext(CurrentGroup); 
				
						if (CurrentGroup != NULL)
						{
							if (CurrentGroup->GetName() != Name)
							{
								// A group has been found with a different name. Stop the search and
								// use the class to describe the selection.
								UseClassToDescribe = TRUE; 
								break;					
							}
						}

					} while (CurrentGroup != NULL); 
				}
				else
				{
					UseClassToDescribe = TRUE;
				}
			}
			if (UseClassToDescribe)
			{
				Description = (FirstInRange->Describe(NumObjectsInRange>1, Verbose));                                   
			}
			else 
			{
				// Use the group name to describe the selection
				ERROR3IF(FirstGroup == NULL, "FirstGroup should be the first selected group"); 
				if (NumObjectsInRange > 1)
				{
					Description = Pluralise(FirstGroup->GetName()); 
				}
				else
				{
					Description = FirstGroup->GetName();
				}
			}
		}
		else
		{
			// There must be more than one type of selected objects
			// ERROR3IF(NumObjectsInRange <= 1, "Only a single object is in range"); 
			// Cos there are multiple objects of different types we describe them as 'objects'
			Description = String(_R(IDS_OBJECTS)); 	
		}

		if (Format == STATUS_BAR)
		{      
			// If the description is to be displayed on the status bar, then it needs to be prefixed 
			// with the number of objects in the range
			Description._MakeMsg( _T("#1%lu #2%S"),NumObjectsInRange, &Description); 
		}
	}
	else // No objects are selected
	{
		// Only show "Nothing selected:" if there are documents open.
PORTNOTE("other","Removed ScreenView usage")
#ifndef EXCLUDE_FROM_XARALX
		if (ScreenView::HowMany() > 0) Description = String_32(_R(IDS_NO_OBJECTS_SEL));
#endif
	}	                                  
	
	return (Description);
}



/********************************************************************************************
>	String_256 SelRange::LayerDescription(DescriptionFormat nFormat)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Inputs:		nFormat			an enum describing how this description will be used,
								eg. STATUS_BAR (anything else returns empty string).
	Returns:	A string containing a text description of what is selected, if anything,
				on the selected layer(s).
	Purpose:	Builds the text displayed by the status-bar code.  Rewritten to be
				internationally portable.
********************************************************************************************/

String_256 SelRange::LayerDescription(DescriptionFormat nFormat)
{
	// Check if there's nothing to do.
	String_256 strDescription;
	if (nFormat != STATUS_BAR || FindFirst() == NULL ||
		FindLast() == NULL) return strDescription;

	// Do we need to describe the selection as "inside"?
	BOOL fIsInside = ContainsSelectInside() && !(Count() == 1 && IS_A(FindFirst(), CaretNode));

	// DMc change
	Layer* pLastLayer = NULL;

	pLastLayer = (Layer*) (FindLast()->FindParent(CC_RUNTIME_CLASS(Layer)));
	
	//Graham 8/7/97: Now, get the document
	Document* pDocument = pLastLayer->FindDocument();
	if (pDocument == NULL)
	{
		// No document yet. So return an empty string
		return String_256("");
	}

	//Is there only one layer in the document?
	if (pDocument->GetNumLayers()==1)
	{
		//Yes. So we needn't put the layer name into the description string.
		//So, is the selection "inside"?
		if(fIsInside)
		{
			//Yes. So return "(inside)" as our description string
			return String_256(_R(IDS_SINGLELAYER_INSIDE));
		}
		else
		{
			//No. So return an empty string
			return String_256("");
		}
	}
	

	// Count how many layers contain select objects if more than one object is selected.
//	INT32 nLayers=0;
	INT32 nLayersWithSelected = 1;
	if (Count() > 1)
	{
		// Loop over all the layers in the doc (?)
		Layer* pLayer = (Layer*) (FindFirst()->FindParent(CC_RUNTIME_CLASS(Layer)));
		while (pLayer != pLastLayer)
		{
			// Over-run?
			ERROR3IF(pLayer == NULL, "Couldn't find the last layer in SelRange::LayerDescription");

			// Determine if pLayer has any selected objects.
			Node* pObj = pLayer->FindFirstChild();
			while (pObj != NULL)
			{
				// Is selected or has selected children?
				if (pObj->IsSelected() || pObj->IsParentOfSelected())
				{
					// Yes, go on to next layer.
					nLayersWithSelected++;
					break;
				}

				// No, consider the next sibling.
				pObj = pObj->FindNext();
			}

			// Go on to the next sibling layer.
			pLayer = (Layer*) (pLayer->FindNext(CC_RUNTIME_CLASS(Layer)));
		}
	}

	// Is there more than one layer with selected objects?
	if (nLayersWithSelected > 1)
	{
		// Yes, so we want a string with one of these formats:-
		// 'on <no. of layers> layers' or 'on <no. of layers> layers (inside)'
		UINT32 nID = (fIsInside) ? _R(IDS_ON_MANY_LAYERS_INSIDE)			// " on #1%d layers (inside)"
							   : _R(IDS_ON_MANY_LAYERS);				// " on #1%d layers"
		strDescription.MakeMsg(nID, (INT32) nLayersWithSelected);
	}
	else
	{
		// Only one layer with a selection.  Decide whether to use 'inside' as a prefix or suffix.
		BOOL fShouldSuffix = (Count() != 1);

		// Does the name of the layer contain the word 'layer'/'Frame' in whatever language?)
		BOOL fLayerReferredTo = FALSE;

// RanbirR - 28/10/97 - Changed for Camelot v2, Frame/Layer integration. 

#ifdef WEBSTER

		// Choose which format string to use.  This is the proper way to construct portable strings!
		static UINT32 nFormat[8] =
		{
			_R(IDS_ON_FRAME_OUTSIDE_NOREF),				// " on frame #1%S"
			_R(IDS_ON_FRAME_OUTSIDE_NOREF),
			_R(IDS_ON_FRAME_OUTSIDE_REF),				// " on #1%S"
			_R(IDS_ON_FRAME_OUTSIDE_REF),
			_R(IDS_ON_FRAME_INSIDE_NOREF_PREFIX),		// " inside on frame #1%S"
			_R(IDS_ON_FRAME_INSIDE_NOREF_SUFFIX),		// " on frame #1%S (inside)"
			_R(IDS_ON_FRAME_INSIDE_REF_PREFIX),			// " inside on #1%S"
			_R(IDS_ON_FRAME_INSIDE_REF_SUFFIX)			// " on #1%S (inside)"
		};

		String_256 strLayerID = pLastLayer->GetLayerID();
		strLayerID.toUpper();
		fLayerReferredTo = (strLayerID.Sub(String_64(_R(IDS_LOCAL_LAYER_NOUN))) != -1);

#else
		static UINT32 nFormat[8];

		// Determine the document mode.
		BOOL IsFrame = pLastLayer->IsFrame();

		// Frame Mode.
		if(IsFrame)
		{
			nFormat[0] =	_R(IDS_ON_FRAME_OUTSIDE_NOREF);				// " on frame #1%S"
			nFormat[1] =	_R(IDS_ON_FRAME_OUTSIDE_NOREF);
			nFormat[2] =	_R(IDS_ON_FRAME_OUTSIDE_REF);				// " on #1%S"
			nFormat[3] =	_R(IDS_ON_FRAME_OUTSIDE_REF);
			nFormat[4] =	_R(IDS_ON_FRAME_INSIDE_NOREF_PREFIX);		// " inside on frame #1%S"
			nFormat[5] =	_R(IDS_ON_FRAME_INSIDE_NOREF_SUFFIX);		// " on frame #1%S (inside)"
			nFormat[6] =	_R(IDS_ON_FRAME_INSIDE_REF_PREFIX);			// " inside on #1%S"
			nFormat[7] =	_R(IDS_ON_FRAME_INSIDE_REF_SUFFIX);			// " on #1%S (inside)"

			// Is the Word 'frame' already included in the text name.
			String_256 strLayerID = pLastLayer->GetLayerID();
			strLayerID.toUpper();
			fLayerReferredTo = (strLayerID.Sub(String_64(_R(IDS_LOCAL_FRAME_NOUN))) != -1);
		}
		// Layer Mode.
		else

		{
			nFormat[0] =	_R(IDS_ON_LAYER_OUTSIDE_NOREF);				// " on layer #1%S"
			nFormat[1] =	_R(IDS_ON_LAYER_OUTSIDE_NOREF);
			nFormat[2] =	_R(IDS_ON_LAYER_OUTSIDE_REF);				// " on #1%S"
			nFormat[3] =	_R(IDS_ON_LAYER_OUTSIDE_REF);
			nFormat[4] =	_R(IDS_ON_LAYER_INSIDE_NOREF_PREFIX);		// " inside on layer #1%S"
			nFormat[5] =	_R(IDS_ON_LAYER_INSIDE_NOREF_SUFFIX);		// " on layer #1%S (inside)"
			nFormat[6] =	_R(IDS_ON_LAYER_INSIDE_REF_PREFIX);			// " inside on #1%S"
			nFormat[7] =	_R(IDS_ON_LAYER_INSIDE_REF_SUFFIX);			// " on #1%S (inside)"
		
			// Is the Word 'layer' already included in the text name.
			String_256 strLayerID = pLastLayer->GetLayerID();
			strLayerID.toUpper();
			fLayerReferredTo = (strLayerID.Sub(String_64(_R(IDS_LOCAL_LAYER_NOUN))) != -1);
		}
#endif
		
		UINT32 nID = nFormat[(fIsInside << 2) | (fLayerReferredTo << 1) | fShouldSuffix];

		// Substitute the layer's name into the correct format string.
		strDescription.MakeMsg(nID, &(pLastLayer->GetLayerID()));
	}

	// OK, return the resulting description.
	return strDescription;
}



/********************************************************************************************

>	static String_256 SelRange::Pluralise(String_256 Noun)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Inputs:		Noun: The word to pluralise
	Outputs:	-
	Returns:	The plural of Noun
	Purpose:	Converts the Noun string to a plural
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256 SelRange::Pluralise(String_256 Noun)
{
	// A bit simple for now
	return (Noun += String_8(_R(IDS_K_RANGE_PLURAL))); 
}




/********************************************************************************************

>	void SelRange::DescribeSelectionInStatusBar()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function should get called whenever the selection description needs 
				to be displayed. The description is cached

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelRange::DescribeSelectionInStatusBar()
{
	String_256			strNull("");
	GetApplication()->UpdateStatusBarText( &strNull ); 
}





/********************************************************************************************

>	SelRangeMessageHandler::SelRangeMessageHandler()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Inputs:		Message: The message
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a SelRangeMessageHandler, an object which, surprisingly,
				handles messages for its parent SelRange.
	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

SelRangeMessageHandler::SelRangeMessageHandler()
  : MessageHandler(CC_RUNTIME_CLASS(SelRangeMessageHandler), TRUE)
{
}




/********************************************************************************************

>	virtual MsgResult SelRangeMessageHandler::Message(Msg* Message)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/94
	Inputs:		Message: The message
	Outputs:	-
	Returns:	-
	Purpose:	Process messages sent to the SelRange. 
	
				Note that a SelChangingMsg is only broadcast at the end of an operation which 
				has changed the selection.

	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

MsgResult SelRangeMessageHandler::Message(Msg* Message)
{
	SelRange* Sel = GetApplication()->FindSelection();

	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;

		// ***** Fix for Alpha release 15/8/94 Markn
		// Put a check in for ABOUTTODIE and KILLED doc messages because we only want to update
		// the selection once when the document dies (i.e. when ~Document() is called)
		// This solution makes hugh asumptions on the messages that are generated on document
		// destruction, and could go wrong in the future.
		// *****
		// Fixed again by Jason, 5/1/95
		// This ABSOLUTELY UNEQUIVOCALLY MUST flush the cache!
		// So if we don't want the msg broadcast, we just pass FALSE to Update() 
		// *****
		if (Sel != NULL)
		{
			if (Msg->State != DocChangingMsg::ABOUTTODIE &&
				Msg->State != DocChangingMsg::KILLED )
			{
				// Ensure selection cache is flushed.
				// The TRUE param. makes this broadcast a SelChangingMsg to inform the world
				
				// But don't bother doing a flush if it's only a title-changed notification
				if (Msg->State != DocChangingMsg::TITLECHANGED)
					Sel->Update(TRUE);
			}
			else
			{
				// Ensure the selection cache is flushed, but don't broadcast a msg!
				Sel->Update(FALSE);
			}
		}
	}
	else if (MESSAGE_IS_A(Message, OpMsg))
	{
		if (((OpMsg*)Message)->MsgType == OpMsg::END)
		{
			// Make sure that any delayed message broadcast is now sent out.
			// This is used to allow Ops to make multiple changes to the selection,
			// yet only cause one SelChangingMsg broadcast (when they End).
			GetApplication()->FindSelection()->BroadcastAnyPendingMessages();
		}
	}
	else if (MESSAGE_IS_A(Message, ToolsCurrentAttrGroupChangedMsg) ||
			 MESSAGE_IS_A(Message, CurrentAttrChangedMsg) ||
			 MESSAGE_IS_A(Message, SelChangingMsg)
			)
	{
		// This convenient message gets sent out whenever a user needs to 
		// reflect a change in the common attributes. Usually the user will
		// call Range::FindCommonAttribute\s is receipt of this message.
		// The FindCommonAttribute fns return Current Attributes in certain
		// cases which is why the message is broadcast when there is a 
		// change in these as well.
		
		Sel->AttrsHaveChanged(); // The CommonAttributeCache is no longer valid. 
		BROADCAST_TO_ALL(CommonAttrsChangedMsg); 
	}
	/**********
		Taken out because when the selected spread changes, NodeRenderableInk::DeselectAll() is called
		which will generate a selection changed message if it needs to.
		   Markn reporting on behalf of the Alpha release bug fixing team, 16/8/94
		   And now back to the studio

	else if (MESSAGE_IS_A(Message, SpreadMsg))
	{
		SpreadMsg*	pSpreadMsg = (SpreadMsg*) Message;

		switch (pSpreadMsg->Reason)
		{
			case (SpreadMsg::SELCHANGED) :
				GetApplication()->FindSelection()->Update(TRUE);
				break;
		}
	}
	*/



	return OK; 
}      



/********************************************************************************************

>	BOOL SelRange::FindObject(CCRuntimeClass* pClass, Node** pOutput = NULL)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95

	Inputs:		pClass  = A runtime class
				pOutput	= A pointer to a pointer to a node (NULL if none)

	Outputs:	pOutput (if non-NULL) is updated to point to the first node of type
				pClass in the selection. If there is no object of this type, this
				is returned containing NULL.

	Returns:	TRUE if there is an object of this type in the selection
				FALSE if not

	Purpose:	Scans the selection hunting for a selected object of type pClass. If an
				object is found, the function will return true, and set pOutput to point
				at the object (if it was not NULL on entry).

********************************************************************************************/

BOOL SelRange::FindObject(CCRuntimeClass* p, Node** pOutput)
{
	BOOL Sel = FALSE;

	if (pOutput != NULL)		// Set a reasonable return value if we fail
		*pOutput = NULL;

	Node *pNode = FindFirst();
	while ((pNode!=NULL) && (!Sel))
	{
		Sel = (pNode->GetRuntimeClass() == p);
		if (!Sel)
			pNode = FindNext(pNode);
	}

	if (Sel && pOutput != NULL)
		*pOutput = pNode;

	return Sel;
}



/********************************************************************************************
>	void SelRange::SetGag(BOOL NewState)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/95
	Inputs:		NewState - TRUE to gag the SelRange - FALSE to ungag it 
	Outputs:	-
	Returns:	-
	Purpose:	Allows operations to gag the selrange in order to stop it sending SelChanged
				messages.  THIS IS VERY SCARY!  The only op that does this it the moment is
				when the text caret is moving left/right.  Whilst the cursor key is down the
				SelRange is gagged so the cursor movement is not slowed by constant SelChanged
				messages.
********************************************************************************************/
void SelRange::SetGag(BOOL NewState)
{
	IsGagged = NewState;																	  
}



/********************************************************************************************
>	BOOL SelRange::IsSelRangeGagged()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the SelRange is gagged, FALSE if it isn't
	Purpose:	For getting the current gag state of the SelRange.
********************************************************************************************/
BOOL SelRange::IsSelRangeGagged()
{
	return IsGagged;																	  
}



/********************************************************************************************
>	Node* SelRange::FindFirstObject(CCRuntimeClass* pClass)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/95
	Inputs:		pClass  = A runtime class
	Outputs:	-
	Returns:	A pointer to the first selected object that is the class specified.
				NULL if none found
	Purpose:	Finds the first selected object of the required class.  For finding derived
				classes use FindFirstDerivedObject
	SeeAlso:	SelRange::FindObject, SelRange::FindFirstDerivedObject
********************************************************************************************/
Node* SelRange::FindFirstObject(CCRuntimeClass* pClass)
{
	Node* pNode = FindFirst();

	while ((pNode != NULL) && (pNode->GetRuntimeClass() != pClass))	 
	{
		pNode = FindNext(pNode);
	}

	return pNode;
}



/********************************************************************************************
>	Node* SelRange::FindNextObject(CCRuntimeClass* pClass, Node* Previous)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/95
	Inputs:		pClass - A runtime class
				Previous - the node to scan from
	Outputs:	-
	Returns:	A pointer to the next selected object that is the class specified.
				NULL if none found
	Purpose:	Finds the next selected object of the required class.  For finding derived
				classes use FindNextDerivedObject
	SeeAlso:	SelRange::FindNext, SelRange::FindNextDerivedObject
********************************************************************************************/
Node* SelRange::FindNextObject(CCRuntimeClass* pClass, Node* Previous)
{
	ERROR3IF(Previous == NULL, "Previous node pointer was NULL");

	if (Previous != NULL)
		Previous = FindNext(Previous);

	while ((Previous != NULL) && (Previous->GetRuntimeClass() != pClass))
	{
		Previous = FindNext(Previous);
	}

	return Previous;
}



/********************************************************************************************
>	Node* SelRange::FindFirstDerivedObject(CCRuntimeClass* pClass)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/95
	Inputs:		pClass  = A runtime class
	Outputs:	-
	Returns:	A pointer to the first selected object derived from the class specified.
				NULL if none found
	Purpose:	Finds the first selected object of the required derived class.  For finding
				extact class matches uses FindFirstObject
	SeeAlso:	SelRange::FindObject, SelRange::FindFirstObject
********************************************************************************************/
Node* SelRange::FindFirstDerivedObject(CCRuntimeClass* pClass)
{
	Node* pNode = FindFirst();

	while ((pNode != NULL) && !pNode->IsKindOf(pClass))	 
	{
		pNode = FindNext(pNode);
	}

	return pNode;
}



/********************************************************************************************
>	Node* SelRange::FindNextDerivedObject(CCRuntimeClass* pClass, Node* Previous)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/05/95
	Inputs:		pClass - A runtime class
				Previous - the node to scan from
	Outputs:	-
	Returns:	A pointer to the next selected object that is derived from the class specified.
				NULL if none found
	Purpose:	Finds the next selected object of the required derived class.  For finding
				exact class matches use FindNextObject
	SeeAlso:	SelRange::FindNext, SelRange::FindNextObject 
********************************************************************************************/

Node* SelRange::FindNextDerivedObject(CCRuntimeClass* pClass, Node* Previous)
{
	ERROR3IF(Previous == NULL, "Previous node pointer was NULL");

	if (Previous != NULL)
		Previous = FindNext(Previous);

	while ((Previous != NULL) && !Previous->IsKindOf(pClass))
	{
		Previous = FindNext(Previous);
	}

	return Previous;
}



/********************************************************************************************

	void SelRange::AttrsHaveChanged()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	This function gets called to inform the range that attributes applied to its 
				objects have, or may have changed. It deletes the Common attribute cache if 
				one exists.
	SeeAlso:	Range::AttrsHaveChanged

********************************************************************************************/

void SelRange::AttrsHaveChanged()
{
// Removed by Jason, 23/10/95
// After discussion with Simon, this has been removed, as we reckon it's completely
// rampant. The selection has not been changed, but the attrs of the selected item(s)
// have changed. Perhaps there should be a notification broadcast, but it should NOT
// be the SelChangingMsg::SELECTIONCHANGED message that this flag triggers!
// In fact, all the current callers (in AttrMgr.cpp) send relevant notification
// broadcasts around, so setting us up for another broadcast is silly.
//	InformedSelChanged	= FALSE; 

#if !defined(EXCLUDE_FROM_RALPH)
	// Inform the DialogBars that the system state has changed so that they can refresh
	// themselves
	DialogBarOp::SetSystemStateChanged(); 
#endif

PORTNOTE("other","SelRange::AttrsHaveChanged - EffectsStack usage removed")
	if (m_pEffectsStack)
	{
		TRACE( _T("Warning - SelRange::AttrsHaveChanged - m_pEffectsStack->AttrsHaveChanged NOT called") );
#ifndef EXCLUDE_FROM_XARALX
		m_pEffectsStack->AttrsHaveChanged();
#endif
	}

	if (m_pEffectClassRange)
		m_pEffectClassRange->AttrsHaveChanged();

	Range::AttrsHaveChanged(); // Call base class fn
}



/********************************************************************************************

>	BOOL SelRange::MakePartialSelectionWhole(BOOL TellWorld = TRUE, BOOL bTextOnly = FALSE, BOOL bPathInText = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> (from Karim MacDonald)
	Created:	21/04/2005 (25 August 2000)

	Inputs:		TellWorld	Whether or not to tell everyone if the selection changes.
							Note that if this function is called from within an Op, this flag
							can be set to FALSE as the selection will be updated on 
							completion of the Op.

	Outputs:	If any text characters are selected, then their parent text stories will be
				selected instead.

	Returns:	TRUE if successful, FALSE otherwise.

	Purpose:	Run through the selection and make sure that no children of TextStories
				are selected - only whole TextStories themselves.

********************************************************************************************/
BOOL SelRange::MakePartialSelectionWhole(BOOL TellWorld, BOOL bTextOnly, BOOL bPathInText)
{
	BOOL bChangedSelection = FALSE;
	Node* pNode = FindFirst();

	while (pNode)
	{
		// Push any text sub-selection up to the selected story
 		if (pNode->IsAnAbstractTextChar())
		{
			((NodeRenderableInk*)pNode)->DeSelect(TRUE);
			TextStory* pStory = ((AbstractTextChar*)pNode)->FindParentStory();
			if (pStory != NULL)
			{
				pStory->Select(TRUE);
			 	bChangedSelection = TRUE;
			}
		}

		// Push subselection of fit-text-to-curve node up to the selected story
		if (bPathInText && pNode->IsNodePath() && pNode->FindParent()->IsKindOf(CC_RUNTIME_CLASS(TextStory)))
		{
			((TextStory*)pNode->FindParent())->Select(TRUE);
			bChangedSelection = TRUE;
		}

		// Push subselection of fit-text-to-curve node up to the selected story
		if (bPathInText && pNode->IsNodePath() && pNode->FindParent()->IsKindOf(CC_RUNTIME_CLASS(TextStory)))
		{
			((TextStory*)pNode->FindParent())->Select(TRUE);
			bChangedSelection = TRUE;
		}

		// Push subselection of fit-text-to-curve node up to the selected story
		if (bPathInText && pNode->IsNodePath() && pNode->FindParent()->IsKindOf(CC_RUNTIME_CLASS(TextStory)))
		{
			((TextStory*)pNode->FindParent())->Select(TRUE);
			bChangedSelection = TRUE;
		}

		// Push subselection of fit-text-to-curve node up to the selected story
		if (bPathInText && pNode->IsNodePath() && pNode->FindParent()->IsKindOf(CC_RUNTIME_CLASS(TextStory)))
		{
			((TextStory*)pNode->FindParent())->Select(TRUE);
			bChangedSelection = TRUE;
		}

		if (!bTextOnly)
		{
			// Do the same thing for sub-selection inside controller nodes
			NodeCompound* pController = pNode->GetParentController();
			if (pController && pNode->NeedsParent(pController))
			{
				((NodeRenderableInk*)pNode)->DeSelect(TRUE);
				NodeRenderableInk* pInkNode = pController->GetInkNodeFromController();
				if (pInkNode)
					pInkNode->Select(TRUE);
				bChangedSelection = TRUE;
			}
		}

		pNode = FindNext(pNode);
	}

	if (bChangedSelection)
		Update(TellWorld);

	return TRUE;
}



// -------------------------------------------------------------------------------------------
// CommonAttrSet methods

/********************************************************************************************

>	BOOL CommonAttrSet::AddTypeToSet(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Inputs:		AttrType: The type to add to the set
	Returns:	FALSE if we run out of memory
	Purpose:	Call this function to build up a subset of attribute types to find common
				attribute values for. It creates an CommonAttributeItem and adds it to the set.

	Errors:		In a debug build a check is made to ensure that AttrType does not initially exist
				in the set. 

	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/
   
BOOL CommonAttrSet::AddTypeToSet(CCRuntimeClass* AttrType)
{
	ERROR3IF(FindAttrItem(AttrType), "Trying to add a duplicate item to a CommonAttrSet"); 
	CommonAttributeItem* CommonAttrItem;
	CommonAttrItem = new CommonAttributeItem(AttrType);
	if (CommonAttrItem == NULL)
	{
		// Oh no we've run out of memory
		return FALSE; 
	}
	AddTail(CommonAttrItem); 
	return TRUE;
}

/********************************************************************************************

>	BOOL CommonAttrSet::AddAllTypesToSet()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Returns:	FALSE if we run out of memory. In this situation the CommonAttrSet is left empty
	Purpose:	This function adds all attribute types to the CommonAttrSet. You will probably
				never need to call this function because Range::FindCommonAttributes will
				call it for you if you pass in an empty CommonAttrSet.
	Errors:		On entry to this function the CommonAttrSet must be empty
	SeeAlso:	CommonAttrSet::AddTypeToSet
	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/

BOOL CommonAttrSet::AddAllTypesToSet()
{
	// If the CommonAttributeSet is not empty on entry then something is probably not right !
	ERROR3IF(!IsEmpty(), 
			 "CommonAttributeSet not empty on entry to AddAllTypesToSet");

	// Obtain a list of all required attributes
	List* ReqdAttribList = ObjectRegistry::GetRequiredAttribs(NULL);
	
	// Now create a CommonAttributeItem for each item in the list
	NodeAttributeClassItem* ReqdAttrib = (NodeAttributeClassItem*)ReqdAttribList->GetHead();
 
	while(ReqdAttrib != NULL)
	{
		if (!(ReqdAttrib->AttributeClass == CC_RUNTIME_CLASS(AttrQuality)))
		{
			if (!AddTypeToSet(ReqdAttrib->AttributeClass))
			{
				DeleteAll(); 
				return FALSE;
			}
		}
		ReqdAttrib = (NodeAttributeClassItem*)ReqdAttribList->GetNext(ReqdAttrib);
	}
	return TRUE; // Success
}

/********************************************************************************************

>	CommonAttributeItem* CommonAttrSet::FindAttrItem(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Inputs:		AttrType: The attribute type to search for
	Returns:	A pointer to the CommonAttributeItem with type AttrType.
				NULL if the item does not exist.
	Purpose:	To find an item in the CommonAttrSet

********************************************************************************************/

CommonAttributeItem* CommonAttrSet::FindAttrItem(CCRuntimeClass* AttrType)
{
	CommonAttributeItem* pAttrItem;
	pAttrItem = (CommonAttributeItem*)(GetHead()); 
	while (pAttrItem)
	{
		if (pAttrItem->AttrType == AttrType)
			break;

		pAttrItem = (CommonAttributeItem*)(GetNext(pAttrItem)); 	
	}
	return pAttrItem; 	 
}

/********************************************************************************************

>	BOOL CommonAttrSet::FindAttrDetails(CCRuntimeClass* AttrType, 
									 NodeAttribute** pAttr, 
									 Range::CommonAttribResult* Status)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Inputs:		AttrType: The attribute type to search for
	
	Outputs:	See Range::FindCommonAttributes for the semantics of these output values
	
				pAttr: 	Attribute pointer (or NULL)
				Status: The attribute's status

	Returns:	FALSE if the attribute does not exist
	Purpose:	To find an item in the CommonAttrSet
	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/

BOOL CommonAttrSet::FindAttrDetails(CCRuntimeClass* AttrType, 
								 	NodeAttribute** pAttr, 
								 	Range::CommonAttribResult* Status)
{
	CommonAttributeItem* pAttrItem;
	pAttrItem = (CommonAttributeItem*)(GetHead()); 
	while (pAttrItem)
	{
		if (pAttrItem->AttrType == AttrType)
		{
			*pAttr = pAttrItem->pAttr;
			*Status = pAttrItem->Status;
			return TRUE; 
		}
		pAttrItem = (CommonAttributeItem*)(GetNext(pAttrItem)); 	
	}
	return FALSE; 	 
}


/********************************************************************************************

>	BOOL CommonAttrSet::IsAnyItemCommon()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95
	Purpose:	Returns TRUE if any item in the CommonAttrSet is a common attribute. 
	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/


BOOL CommonAttrSet::IsAnyItemCommon()
{
	// Scan the set
	CommonAttributeItem* pAttrItem;
	pAttrItem = (CommonAttributeItem*)(GetHead());
	while (pAttrItem)
	{
		if (pAttrItem->Status == Range::ATTR_COMMON)
			return TRUE;
		pAttrItem = (CommonAttributeItem*)(GetNext(pAttrItem)); 	
 	}
	return FALSE;
} 


/********************************************************************************************

>	void CommonAttrSet::ClearResults();  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	This function initialises the CommonAttrSet ready for finding common attributes
				Each CommonAttributeItem in the set has its Status set to ATTR_NONE and its 
				pAttr set to NULL. You will probably never need to call this function as
				Range::FindCommonAttributes makes a call to it prior to looking for
				common attributes. 

	Errors:		The CommonAttrSet must not be empty
				The AttrType field in each CommonAttributeItem must be a valid attribute type
	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/

void CommonAttrSet::ClearResults()
{
	ERROR3IF(IsEmpty(), "CommonAttrSet::ClearResults called on an empty CommonAttrSet");
	CommonAttributeItem* pAttrItem;
	pAttrItem = (CommonAttributeItem*)(GetHead()); 
	while (pAttrItem)
	{
		ERROR3IF(!pAttrItem->AttrType, "CommonAttrSet contains invalid attr type"); 
		pAttrItem->ClearResults(); 
		pAttrItem = (CommonAttributeItem*)(GetNext(pAttrItem)); 	
	}
}


/********************************************************************************************

>	INT32 CommonAttrSet::UpdateKnownValues(CommonAttrSet* pCache); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Inputs:		pCache: A pointer to CommonAttrSet which contains known Status and pAttr values.
	Outputs:	-
	Returns:	The number of items which have not been updated (n). If this number is 0 then 
				all items in the set have values. This number is  useful because all
				updated items get moved to the end of the list, and so the first n items in 
				the list are those that you need to find values for. This makes the list 
				scanning much more efficient.

	Purpose:	This function is provided so that a CommonAttrSet can be updated from a cache
				of known values. Each AttrType in this set which has values in the pCache is
				updated. 

				It is called by Range::FindCommonAttributes to reduce the number of attribute
				types to find common attributes for. 

	SeeAlso:	Range::FindCommonAttributes

********************************************************************************************/

INT32 CommonAttrSet::UpdateKnownValues(CommonAttrSet* pCache)
{
	CommonAttributeItem* pMyItem;
	CommonAttributeItem* pCachedItem;
	CommonAttributeItem* pFirstKnownItem = NULL;
	INT32 UncachedItems = GetCount();

	// Scan thru the cache
	pCachedItem = (CommonAttributeItem*)(pCache->GetHead());

	while (pCachedItem && UncachedItems)
	{
		pMyItem = (CommonAttributeItem*)GetHead(); 
		while (pMyItem != pFirstKnownItem)
		{
			if (pMyItem->AttrType == pCachedItem->AttrType)
			{
				// Item is cached 
				pMyItem->pAttr = pCachedItem->pAttr;
				pMyItem->Status = pCachedItem->Status;
				UncachedItems--;
				// Move the item to the tail of the list, so that we can stop when we
				// hit the first known item.
				RemoveItem(pMyItem); 
				AddTail(pMyItem);
				if (!pFirstKnownItem)
					pFirstKnownItem = pMyItem;
				break;
			}
			pMyItem = (CommonAttributeItem*)(GetNext(pMyItem)); 	
		}
		pCachedItem = (CommonAttributeItem*)(pCache->GetNext(pCachedItem)); 	
	}
 	return UncachedItems;
}  

/********************************************************************************************

	BOOL CommonAttrSet::BuildAttrDetailsMap(NodeRenderableInk* CompoundObject, 
									   		INT32 FirstN, 
											CCAttrMap* pAppliedAttrMap, 
									   		CMapPtrToWord* pCompoundAttrMap)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:		CompoundObject: The compound we wish to generate the map for

				FirstN:			Specifies how many attribute types to add to the map.
								Useful because as we find CommonAttributeItem values we
								move them to the end of the list.

				pAppliedAttrMap: A map from an attribute type to the attribute value which is
								 'applied' to the CompoundObject. Only AttrTypes which have
								 default attribute values are added to the AttrDetailsMap

	Outputs:	A map from an AttrType to an ObjectAttrUsage value
	Returns:	FALSE if we run out of memory. All items in the map are deleted.
	Purpose:	This method is used by FindCommonAttributes to determine if a default attribute
				is common to a CompoundObject.

				The routine creates a map from the FirstN items in the CommonAttrSet,  to 
				ObjectAttrUsage values. 
				
				For Child read Child, or Child of Child etc.

				If an AttrType maps to a REQUIRED value, this means that at least
				one Child of the compound requires the attribute, but an attribute of this
				type does not appear as a Child of the compound.

				If it maps to a FOUND value then this means that the attribute 
				is required by at least one child object and has been found as a Child

				if it maps to a NOT_REQUIRED value then this means that the attribute
				is not required by any child of the compound.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CommonAttrSet::BuildAttrDetailsMap(NodeRenderableInk* Object, 
								   INT32 FirstN,
								   CCAttrMap* pAppliedAttrMap, 
								   CMapPtrToWord* pCompoundAttrMap)
{
  
	// This will probably indicate a bug
	ERROR3IF(FirstN == 0, "Called CommonAttrSet::BuildAttrDetailsMap with nothing to do");
	 
	// I can't imagine why anyone would want to call this fn on a non-compound object
	ERROR3IF(!(Object->IsCompound()), "BuildAttrDetailsMap called on a non-compound"); 

	if (FirstN == 0)
		return TRUE;  // Nothing to do

	// We want to scan this list fast, and we don't want to change the order of items 
	// within it. So create a ListIndex
	List IdxIntoAttrSet;
	if (!CreateIndex(&IdxIntoAttrSet))
	{
		return FALSE; 
	}
	
	// Place required attribute types into the map, and remove index items we are not interested
	// in.
	ListItemIdx* ppAttrItem = (ListItemIdx*)(IdxIntoAttrSet.GetHead());
	CommonAttributeItem* pAttrItem = (CommonAttributeItem*)GetHead();
	ListItemIdx* ppNxtAttrItem; // Hand over hand
	INT32 Count = 0; 
	while(pAttrItem && ppAttrItem)
	{
		ppNxtAttrItem = (ListItemIdx*)(IdxIntoAttrSet.GetNext(ppAttrItem)); 

		if  (Count != FirstN) // We are interested in this item, add it to the map
		{
			// Is the applied AttrType attribute a default ?
			void* pvAttrValue = NULL;
			NodeAttribute* pAttrValue;
			pAppliedAttrMap->Lookup(pAttrItem->AttrType, pvAttrValue);
			pAttrValue = (NodeAttribute*)pvAttrValue;
			if (pAttrValue->IsADefaultAttr()) 
			{
				(*pCompoundAttrMap)[pAttrItem->AttrType] = (WORD)CommonAttrSet::NOT_REQUIRED; 
			}
			else
			{
				// Remove item from index - we are not interested in non-defaults
			    delete(IdxIntoAttrSet.RemoveItem(ppAttrItem));				
			}
			pAttrItem = (CommonAttributeItem*)GetNext(pAttrItem);
			Count++; 
		}
		else  // We are not interested in this item remove it from our index
		{
			delete(IdxIntoAttrSet.RemoveItem(ppAttrItem));				
		}
		ppAttrItem = ppNxtAttrItem;
	}

	// Scan the compound depth first
	Node* Current = Object->FindFirstDepthFirst();
	
	INT32 UnknownValues = IdxIntoAttrSet.GetCount();  // When this equals 0 we can stop
	
	while((Current != Object) && UnknownValues) // stop when we hit the compound
	{
		if (Current->IsAnObject())
		{
			ppAttrItem = (ListItemIdx*)(IdxIntoAttrSet.GetHead());
			while (ppAttrItem)
			{
				ppNxtAttrItem = (ListItemIdx*)(IdxIntoAttrSet.GetNext(ppAttrItem)); 
	
				CCRuntimeClass* AttrType = ((CommonAttributeItem*)(ppAttrItem->pItem))->AttrType; 
				if (((NodeRenderableInk*)Current)->
					 RequiresAttrib(AttrType))
				{
					CommonAttrSet::ObjectAttrUsage AttrUsage;
					CMapPtrToWord::iterator iter = pCompoundAttrMap->find( AttrType );
					if( pCompoundAttrMap->end() != iter )
					{
						AttrUsage = (CommonAttrSet::ObjectAttrUsage)iter->second;
						if( AttrUsage != CommonAttrSet::FOUND ) 
						{
							(*pCompoundAttrMap)[AttrType] = (WORD)CommonAttrSet::REQUIRED;
						}
					}

					// We don't need to consider this item again, so remove it from the
					// index. 
					delete(IdxIntoAttrSet.RemoveItem(ppAttrItem)); 
				}
				ppAttrItem = ppNxtAttrItem;
			}

		}
		else if (Current->IsAnAttribute())
		{
			CCRuntimeClass* AttrType = ((NodeAttribute*)Current)->GetAttributeType(); 

			CommonAttrSet::ObjectAttrUsage AttrUsage;
			CMapPtrToWord::iterator iter = pCompoundAttrMap->find( AttrType );
			if( pCompoundAttrMap->end() != iter )
			{
				AttrUsage = (CommonAttrSet::ObjectAttrUsage)iter->second;
			
				if (AttrUsage != CommonAttrSet::FOUND) 	// Has not been found before !
				{
					(*pCompoundAttrMap)[AttrType] = (WORD)CommonAttrSet::FOUND;
					UnknownValues--; // This AttrUsage value is not going to change
				}
			} // else it's an attribute we're not interested in !	
		}
		Current = Current->FindNextDepthFirst(Object); 		
	}

	IdxIntoAttrSet.DeleteAll(); // Tidyup

	return TRUE;
} 



#if 0

/*
	JCF says: this has been rewritten so that the strings it constructs can be
			  easily trasnslated.
*/

/********************************************************************************************

>	String_64 SelRange::LayerDescription(DescriptionFormat Format)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/94
	Inputs:		Format:	The format of the description
	Outputs:	-
	Returns:	-
	Purpose:	Returns a string describing the layer part of the selection Description.
	Errors:		-
	Scope:		private
	SeeAlso:	SelRange::Describe
	SeeAlso:	SelRange::ObjectDescription

********************************************************************************************/

String_64 SelRange::LayerDescription(DescriptionFormat Format)
{
	String_64 Description;
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

	if ((Format == STATUS_BAR) && (FindFirst() != NULL))
	{
		// Determine if the selection contains any 'inside' objects
		BOOL SelInside = ContainsSelectInside();

		// If the selected object is the caret, then we don't want to display the 'inside' part of
		// the description.
		BOOL CaretSelected = ((Count()==1) && (IS_A((FindFirst()), CaretNode)));

		// InsideDesc will be set to an empty string or the word 'inside'
		String_16 InsideDesc;
		String_16 InsideBracketDesc; // Same as InsideDesc except enclosed in backets - "('inside')"
		if (SelInside && !CaretSelected)
		{
			InsideDesc._MakeMsg(" #1%S",&String_16(_R(IDS_INSIDE)));
			InsideBracketDesc._MakeMsg("(#1%S)",&String_16(_R(IDS_INSIDE))); 
		}
	
		UINT32 NumLayersWithSel = 1; 						  // Number of layers with selected
															  // objects.

		Layer* LastLayer = (Layer*) FindLast()->FindParent(CC_RUNTIME_CLASS(Layer)); 

		// ---------------------------------------------------------------------------------
		// Determine the number of layers containing selected objects
	
		if (Count() > 1)
		{
			Layer* CurrentLayer = (Layer*)(FindFirst()->FindParent(CC_RUNTIME_CLASS(Layer)));
			Node* CurrentObj;
			while (CurrentLayer != LastLayer) 
			{
				ERROR3IF(CurrentLayer == NULL, "Could not find last layer"); 
				// Determine if the current layer has any selected objects IsParentOfSelected cannot
				// be called on a layer at the moment.
				CurrentObj = CurrentLayer->FindFirstChild();
				while(CurrentObj != NULL)
				{
					if (CurrentObj->IsSelected() || CurrentObj->IsParentOfSelected())
					{
						NumLayersWithSel++;		
						break;
					}
					CurrentObj = CurrentObj->FindNext();
				}

				CurrentLayer =  (Layer*)(CurrentLayer->FindNext(CC_RUNTIME_CLASS(Layer)));
			} 
		}
		if (NumLayersWithSel > 1)
		{
			// The format is 'On <NumLayers> Layers' with an optional ('inside') string if there is at 
			// least one object selected inside.
			Description._MakeMsg(" #1%S #2%lu #3%S #4%S",
					  &String_8(_R(IDS_ON)),NumLayersWithSel,&String_8(_R(IDS_LAYERS)),&InsideBracketDesc); 

		}
		else 
		{
			// Inside handling
			String_16 Prefix;
			String_16 Suffix; 
 
			if (Count() == 1)
			{
				Prefix._MakeMsg("#1%S", &InsideDesc); 	   // ' inside' prefix
			}
			else
			{
				Suffix._MakeMsg("#1%S", &InsideBracketDesc); 	   // ' inside' prefix
			}


			// The format is 'On Layer <LayerName>'
			// if LayerName contains the word layer however then the format is
			// On <LayerName>

			String_256 TempLayerID = LastLayer->GetLayerID(); 
			TempLayerID.toUpper();
			if (TempLayerID.Sub(String_8(_R(IDS_LAYER_MASK))) != -1)   // does TempLayer contain the string 'LAYER" ?
			{
				// 'LAYER' found in layer id
/*				Description._MakeMsg("#1%S #2%S #3%S #4%S",
									  &Prefix, &String_8(_R(IDS_ON)), &(LastLayer->GetLayerID()), &Suffix);
*/
				SafeAppendString(&Description, Prefix);
				SafeAppendString(&Description, String_8(_R(IDS_ON)));
				SafeAppendString(&Description, LastLayer->GetLayerID());
				SafeAppendString(&Description, Suffix, FALSE);
			} 
			else
			{ 
/*				Description._MakeMsg("#1%S #2%S #3%S #4%S",
									  &Prefix, &String_8(_R(IDS_ON_LAYER)), &(LastLayer->GetLayerID()), &Suffix);
*/
				SafeAppendString(&Description, Prefix);
				SafeAppendString(&Description, String_8(_R(IDS_ON_LAYER)));
				SafeAppendString(&Description, LastLayer->GetLayerID());
				SafeAppendString(&Description, Suffix, FALSE);
			}
		}
	}	
#endif
	return Description;
}

#endif



/*********************************************************************************************
>	BOOL Range::operator==(const Range& R) const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/96
	Inputs:		R - the range to compare against.
	Outputs:	- 
	Returns:	TRUE if R describes the same range as this range.
	Purpose:	Test for equality of two ranges.  Ranges are considered equal if they have
				the same flags and the same first & last nodes.
	SeeAlso		Range::operator!=
	Errors:		None.
**********************************************************************************************/  
BOOL Range::operator==(const Range& R) const
{
	return (R.RangeControlFlags.Selected == RangeControlFlags.Selected &&
			R.RangeControlFlags.Unselected == RangeControlFlags.Unselected &&
			R.RangeControlFlags.CrossLayer == RangeControlFlags.CrossLayer &&
			R.RangeControlFlags.IgnoreLockedLayers == RangeControlFlags.IgnoreLockedLayers &&
			R.RangeControlFlags.IgnoreNoneRenderable == RangeControlFlags.IgnoreNoneRenderable &&
			R.RangeControlFlags.IgnoreInvisibleLayers == RangeControlFlags.IgnoreInvisibleLayers &&
			R.FirstNode == FirstNode &&
			R.LastNode == LastNode);
}



/*********************************************************************************************
>	BOOL Range::operator!=(const Range& R) const

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/96
	Inputs:		R - the range to compare against.
	Outputs:	- 
	Returns:	TRUE if R dosen't describe the same range as this range.
	Purpose:	Test for non-equality of two ranges.  Ranges are considered equal if they have
				the same flags and the same first & last nodes.
	SeeAlso		Range::operator==
	Errors:		None.
**********************************************************************************************/  
BOOL Range::operator!=(const Range& R) const
{
	return !(*this==R);
}




/*****************************************************************************************
	ListRange class
*/
/*****************************************************************************************

>   ListRange::ListRange()

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	11/12/2003

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	ListRange constructor.

	Errors:		-

******************************************************************************************/

ListRange::ListRange() :	Range(NULL, NULL, RangeControl(TRUE,FALSE,TRUE,TRUE))	// Call Range constructor
{
	pLastReturnedItem = NULL;
}




/*****************************************************************************************

>   ListRange::ListRange(const List& srcList)

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	11/12/2003

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	SelRange constructor.

	Errors:		-

******************************************************************************************/
ListRange::ListRange(const List& srcList) : Range()
{
	ERROR2RAW("Unimplemented!");
}





/*****************************************************************************************

>   ListRange::ListRange(const ListRange& srcRange)

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/02/2005

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	SelRange constructor.

	Errors:		-

******************************************************************************************/
ListRange::ListRange(const ListRange& srcRange) : Range()
{
	BOOL bOK = TRUE;
	NodeListItem* pNodeItem = (NodeListItem*)srcRange.nodelist.GetHead();
	while (pNodeItem)
	{
		NodeListItem* pNewNodeItem = new NodeListItem(pNodeItem->pNode);
		if (pNewNodeItem==NULL)
		{
			bOK = FALSE;
			break;
		}
		nodelist.AddTail(pNewNodeItem);

		pNodeItem = (NodeListItem*)srcRange.nodelist.GetNext(pNodeItem);
	}

	if (!bOK)
	{
		// Get rid of all NodeListItems in the nodelist
		while (nodelist.GetHead())
			delete nodelist.RemoveHead();
	}
	pLastReturnedItem = NULL;
}




/*****************************************************************************************

>   ListRange::ListRange(const ListRange* pSrcRange)

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	14/02/2005

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	SelRange constructor.

	Errors:		-

******************************************************************************************/
ListRange::ListRange(const ListRange* pSrcRange) : Range()
{
	if (pSrcRange==NULL)
		return;

	BOOL bOK = TRUE;
	NodeListItem* pNodeItem = (NodeListItem*)pSrcRange->nodelist.GetHead();
	while (pNodeItem)
	{
		NodeListItem* pNewNodeItem = new NodeListItem(pNodeItem->pNode);
		if (pNewNodeItem==NULL)
		{
			bOK = FALSE;
			break;
		}
		nodelist.AddTail(pNewNodeItem);

		pNodeItem = (NodeListItem*)pSrcRange->nodelist.GetNext(pNodeItem);
	}

	if (!bOK)
	{
		// Get rid of all NodeListItems in the nodelist
		while (nodelist.GetHead())
			delete nodelist.RemoveHead();
	}
	pLastReturnedItem = NULL;
}




/*****************************************************************************************

>   ListRange::~ListRange()

    Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:	11/12/2003

    Inputs:		-
    Outputs:	-
    Returns:	-

	Purpose:	SelRange destructor.

	Errors:		-

******************************************************************************************/

ListRange::~ListRange()
{
	// Get rid of all NodeListItems in the nodelist
	while (nodelist.GetHead())
		delete nodelist.RemoveHead();

	pLastReturnedItem = NULL;
}




/*********************************************************************************************

>    Node* ListRange::FindFirst(BOOL AndChildren = FALSE)

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> 11/12/2003
     Created:   25/6/93
     
     Returns:   If the range contains any members then
					A pointer to the first node in the range is returned
				Else
					NULL is returned 
			
     Purpose:   The purpose of this function is to find the first node in a range. 

     SeeAlso:   Range::FindNext    
     
     Errors:    An assertion failure will occur if:

     			There are no more nodes to search and the last node to search was not found.

**********************************************************************************************/
                                    
Node* ListRange::FindFirst(BOOL AndChildren)
{
	// Preconditions...
	ERROR2IF(this==NULL,NULL,"FindFirst called on NULL range");
//	ERROR2IF(AndChildren, NULL, "ListRange can't honour AndChildren requests yet");

	NodeListItem* pNodeItem = (NodeListItem*)nodelist.GetHead();
	pLastReturnedItem = pNodeItem;
	if (pNodeItem)
	{
		if (AndChildren)
			return pNodeItem->pNode->FindFirstDepthFirst();
		else
			return pNodeItem->pNode;
	}

	return NULL;
}


     

/*********************************************************************************************

>    Node* ListRange::FindNext(Node* Prev, BOOL AndChildren = FALSE)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> 11/12/2003
     Created:   25/6/93
     Inputs: 	Prev: The previous node in the range (usually returned from a 
     				   Range::FindFirst, or a previous Range::FindNext call). 
     Outputs:    - 
     Returns:   If the range contains any more members then
					A pointer to the next node in the range is returned
				Else
					NULL is returned 
     Purpose:   The purpose of this function is to find the next node in the range after Prev 
     SeeAlso:   Range::FindFirst    
     Errors:    An assertion failure will occur if:
     			There are no more nodes to search and the last node to search was not found.

**********************************************************************************************/

Node* ListRange::FindNext(Node* pPrevious, BOOL AndChildren)
{
	// Preconditions
	// No need to check that "this" is NULL because that's already been done
	// in FindFirst.
	ERROR2IF(pPrevious == NULL, NULL, "NULL pointer passed to Range::FindNext");
//	ERROR2IF(AndChildren, NULL, "ListRange can't honour AndChildren requests yet");

	BOOL found = FALSE;
	NodeListItem* pNodeItem = (NodeListItem*)nodelist.GetHead();

	// ------------------
	// Optimisation
	if (pLastReturnedItem)
	{
		if (pLastReturnedItem->pNode == pPrevious)
			pNodeItem = pLastReturnedItem;
		else if (AndChildren && pLastReturnedItem->pNode->IsNodeInSubtree(pPrevious))
			pNodeItem = pLastReturnedItem;
	}
	// ------------------

	while (pNodeItem && !found)
	{
		found = (pNodeItem->pNode == pPrevious);
		if (!found && AndChildren)
		{
			// If this list item contains the previous context node
			// Then we are still traversing the subtree of that list item
			// So get the next item in the subtree
			if (pNodeItem->pNode->IsNodeInSubtree(pPrevious))
			{
				pLastReturnedItem = pNodeItem;
				return pPrevious->FindNextDepthFirst(pNodeItem->pNode);
			}
		}

		pNodeItem = (NodeListItem*)nodelist.GetNext(pNodeItem);
	}

	pLastReturnedItem = pNodeItem;

	if (pNodeItem && found)
		if (AndChildren)
			return pNodeItem->pNode->FindFirstDepthFirst();
		else
			return pNodeItem->pNode;

	return NULL;
}




/*********************************************************************************************

>    Node* ListRange::FindPrev(Node* pNode, BOOL AndChildren = FALSE)
     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> 11/12/2003
     Created:   19/01/95
     Inputs: 	pNode: The node which you want the previous one in the range to... 
     Outputs:    - 
     Returns:   If the range contains any members before the input node then
					A pointer to the prev node in the range is returned
				Else
					NULL is returned (Note NULL returned if input node was not in the range)
     Purpose:   The purpose of this function is to find the previous node in the range before
     			the input node.
				until it meets the input node, then returns the previous node to that!!!
     SeeAlso:   Range::FindFirst; Range::FindNext    
     Errors:    -

**********************************************************************************************/

Node* ListRange::FindPrev(Node* pNode, BOOL AndChildren)
{
	// Preconditions
	// No need to check that "this" is NULL because that's already been done
	// in FindFirst.
	ERROR2IF(pNode == NULL, NULL, "NULL pointer passed to Range::FindNext");
	ERROR2IF(AndChildren, NULL, "ListRange::FindPrev can't honour AndChildren requests yet");

	BOOL found = FALSE;
	NodeListItem* pNodeItem = (NodeListItem*)nodelist.GetTail();

	// ------------------
	// Optimisation
	if (pLastReturnedItem && pLastReturnedItem->pNode == pNode)
		pNodeItem = pLastReturnedItem;
	// ------------------

	while (pNodeItem && !found)
	{
		found = (pNodeItem->pNode == pNode);

		pNodeItem = (NodeListItem*)nodelist.GetPrev(pNodeItem);
	}

	pLastReturnedItem = pNodeItem;

	if (pNodeItem && found)
		return pNodeItem->pNode;

	return NULL;
}




/*********************************************************************************************

>    Node* ListRange::FindLast()

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   25/6/93
     
     Returns:   If the range contains any members then
					A pointer to the last node in the range is returned
				Else
					NULL is returned 
			
     Purpose:   The purpose of this function is to find the last node in a range. 
	 			If the range was constructed with a NULL last node specifier then
				the range is scanned until the last node is found. If a non NULL last node
				was specified however the value of last is simply returned. It's existance
				is not verified !. 

     SeeAlso:   Range::FindFirst
     			Range::FindNext    
     
     Errors:    An assertion failure will occur if:

     			There are no more nodes to search and the last node to search was not found.

**********************************************************************************************/

Node* ListRange::FindLast()
{
	ERROR2IF(this==NULL,NULL,"FindFirst called on NULL range");

//	BOOL found = FALSE;
	NodeListItem* pNodeItem = (NodeListItem*)nodelist.GetTail();
	pLastReturnedItem = pNodeItem;
	if (pNodeItem)
		return pNodeItem->pNode;

	return NULL;
}




/**********************************************************************************************
**********************************************************************************************/
void ListRange::AddNode(Node* pNode)
{
#ifdef _DEBUG_LISTCHECKS
	ERROR3IF(InRange(pNode, FALSE), "Attempt to add duplicate node to ListRange!\n");
#endif

	NodeListItem* pNodeItem = new NodeListItem(pNode);
	nodelist.AddTail(pNodeItem);
}




/**********************************************************************************************
**********************************************************************************************/
Node* ListRange::RemoveNode(Node* pNode)
{
	BOOL found = FALSE;
	Node* pFoundNode = NULL;
	NodeListItem* pNodeItem = (NodeListItem*)nodelist.GetHead();
	while (pNodeItem && pFoundNode==NULL)
	{
		found = (pNodeItem->pNode == pNode);

		if (found)
			pFoundNode = pNode;
		else
			pNodeItem = (NodeListItem*)nodelist.GetNext(pNodeItem);
	}

	if (pNodeItem && pFoundNode)
		nodelist.RemoveItem(pNodeItem);

	pLastReturnedItem = NULL;

	return pFoundNode;
}




/**********************************************************************************************
**********************************************************************************************/
BOOL ListRange::InRange(Node* pNode, BOOL AndChildren) const
{
	ERROR2IF(AndChildren, FALSE, "ListRange can't honour AndChildren requests yet");

	BOOL found = FALSE;
	NodeListItem* pNodeItem = (NodeListItem*)nodelist.GetHead();
	while (pNodeItem && !found)
	{
		found = (pNodeItem->pNode == pNode);

		pNodeItem = (NodeListItem*)nodelist.GetNext(pNodeItem);
	}

//	pLastReturnedItem = pNodeItem;

	return found;
}




/**********************************************************************************************
**********************************************************************************************/
void ListRange::Clear()
{
	// Get rid of all NodeListItems in the nodelist
	while (nodelist.GetHead())
		delete nodelist.RemoveHead();

	pLastReturnedItem = NULL;
}




/********************************************************************************************

>	BOOL ListRange::MatchesSelectionEffectLevel(INT32 iStackPos)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/2005
	Inputs:		iStackPos - index into effects stack
	Outputs:	-
	Returns:	TRUE if this range matches the specifed level in the Effects stack
				FALSE otherwise
	Purpose:	Get the current edit context for the feather UI controls
	Notes:		VERY IMPORTANT!
				The node pointers held in this range could be out of date - the nodes
				could have been deleted. So it's important that the pointers are
				only used AFTER we have verified that they actually point to something!

	More:		The implementation assumes that the nodes will be stored in the lists
				in the same order. This is not a good idea!
				A better algorithm, avoiding O(n^2) problems, would be to check the two
				lists are the same length, then go through them both clearing a marker flag
				on every node, then scan one list setting the marker flag, finally scan
				the other list checking whether all nodes are marked.
				Er, but what about the possible dead pointers in one of the lists?

********************************************************************************************/
BOOL ListRange::MatchesSelectionEffectLevel(INT32 iStackPos)
{
	if (iStackPos==STACKPOS_INVALID)
		return FALSE;

	SelRange* pSelRange = GetApplication()->FindSelection();
	EffectsStack* pStack = pSelRange->GetEffectsStack();		// From cache
	if (pStack==NULL || pStack->IsEmpty())
		return FALSE;

	INT32 iPos = iStackPos;
	Range* pLevelRange = pStack->GetLevelRange(&iPos);
	if (pLevelRange==NULL)
		return FALSE;

	Node* pNode = pLevelRange->FindFirst();
	Node* pEditNode = FindFirst();
	while (pNode && pEditNode)
	{
		// If the pointers are the same then we can reasonably assume
		// the the pointer stored in s_pEditRange is still valid...
		if (pNode!=pEditNode)
			return FALSE;

		// Now we know the pointer at least points to SOME kind of node
		// Check whether Another node has been allocated into the same
		// address as a deleted node...
		if (pNode->GetTag() != pEditNode->GetTag())
			return FALSE;

		pNode = pLevelRange->FindNext(pNode);
		pEditNode = FindNext(pEditNode);
	}

	// If either pointer is non-null then we stopped without matching every case
	// So must return NULL
	if (pNode || pEditNode)
		return FALSE;

	return TRUE;
}


/**********************************************************************************************
**********************************************************************************************/
void Range::ForceRedrawView(DocView* pDocView, BOOL bReleaseCache, BOOL bUseBlobRects, BOOL bReleaseParentsOnly)
{
	BOOL bOldPromote = SetPromoteToParent(TRUE);

	NodeRenderableBounded* CurrentNode = (NodeRenderableBounded*)FindFirst(); 

	if (CurrentNode != NULL && pDocView!=NULL)
	{
		Spread* pSpread = (Spread*)CurrentNode->FindParent(CC_RUNTIME_CLASS(Spread));
		Node* pBackmost = CurrentNode;
		DocRect Bounds(0,0,0,0);

		while (CurrentNode)
		{
			if (bReleaseCache && CurrentNode->IsAnObject()) ((NodeRenderableInk*)CurrentNode)->ReleaseCached(TRUE, !bReleaseParentsOnly, !bReleaseParentsOnly, !bReleaseParentsOnly);
			if (CurrentNode->IsBounded()) Bounds = Bounds.Union( ((NodeRenderableBounded*)CurrentNode)->GetBoundingRect() );
			if (CurrentNode->IsBounded() && bUseBlobRects) Bounds = Bounds.Union( ((NodeRenderableBounded*)CurrentNode)->GetBlobBoundingRect() );
			if (CurrentNode->IsUnder(pBackmost)) pBackmost = CurrentNode;

			CurrentNode = (NodeRenderableBounded*)FindNext(CurrentNode);
		}

		pDocView->ForceRedraw(pSpread, Bounds, TRUE, pBackmost);
	}

	SetPromoteToParent(bOldPromote);
}




/*******************************************************************************************

>	BOOL Range::FindBitmap( KernelBitmap * pFoundBitmap = NULL,
							KernelBitmapRef ** ppFoundBitmapRef = NULL,
							NodeBitmap ** ppFoundNode = NULL,
							AttrFillGeometry ** ppFoundFillAttribute = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/1/97
	Changed:	Phil, 20/01/2004: Moved from PlugInUndoOp static to be member function of range
									!!!!!!!!!
	Inputs:		None
	Outputs:	if supplied, it will return a pointer to the bitmap that is in the selection
				if supplied, it will return a pointer to the bitmap ref that is in the selection
				if supplied, it will return a pointer to the node bitmap that is in the selection
				if supplied, it will return a pointer to the fill geometry that is in the selection
	Returns:	TRUE if succeeded in finding a bitmap, FALSE if not
	Purpose:	Find out if there is a valid bitmap in the selection and return useful information
				to the caller.
	SeeAlso:	-

*******************************************************************************************/

BOOL Range::FindBitmap( KernelBitmap** ppFoundBitmap,
						KernelBitmapRef** ppFoundBitmapRef,
						NodeBitmap** ppFoundNode,
						AttrFillGeometry** ppFoundFillAttribute)
{
	KernelBitmap * pBitmap = NULL;
	KernelBitmapRef* pBitmapRef = NULL;

	// Assume nothing found
	if (ppFoundBitmap)			*ppFoundBitmap = NULL;
	if (ppFoundBitmapRef)		*ppFoundBitmapRef = NULL;
	if (ppFoundNode)			*ppFoundNode = NULL;
	if (ppFoundFillAttribute)	*ppFoundFillAttribute = NULL;

	Node* pCurrentNode = FindFirst(); 

	if (pCurrentNode != NULL) // No nodes selected so End
	{
	    // Do all bitmaps. OK this should pick up the fill as well. Never mind
		while (pCurrentNode != NULL)
		{
	       	if  (pCurrentNode->IsABitmap())
	       	{
				NodeBitmap* pNodeBmp = (NodeBitmap*)pCurrentNode;
				pBitmap = pNodeBmp->GetBitmap();
				pBitmapRef = pNodeBmp->GetBitmapRef();
				// If the caller wanted it, return the found bitmap to them
				if (ppFoundBitmap)
					*ppFoundBitmap = pBitmap;
				// ditto found bitmap ref
				if (ppFoundBitmapRef)
					*ppFoundBitmapRef = pBitmapRef;
				// ditto found node
				if (ppFoundNode)
					*ppFoundNode = pNodeBmp;
				// ditto found fill attribute
				if (ppFoundFillAttribute)
					*ppFoundFillAttribute = NULL;

				return TRUE;
	       	}

			pCurrentNode = FindNext(pCurrentNode);
		}

	} 

	if (ppFoundFillAttribute)
	{
		// Find the first Fill Attribute in the selection
		AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

		while (pAttrNode != NULL)
		{
			if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapColourFill)))
			{
				pBitmap = pAttrNode->GetBitmap();
				pBitmapRef = pAttrNode->GetBitmapRef();
				// If the caller wanted it, return the found bitmap to them
				if (ppFoundBitmap)
					*ppFoundBitmap = pBitmap;
				// ditto found bitmap ref
				if (ppFoundBitmapRef)
					*ppFoundBitmapRef = pBitmapRef;
				// ditto found node
				if (ppFoundNode)
					*ppFoundNode = NULL;
				// ditto found fill attribute
				if (ppFoundFillAttribute)
					*ppFoundFillAttribute = pAttrNode;

				return TRUE;
			}

			// Check the next fill
			pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
		}
	}

	return FALSE;
}




