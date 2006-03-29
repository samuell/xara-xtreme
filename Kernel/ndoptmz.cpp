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
/*
//*/

// The implementation of the NodeRenderableInk attribute optimisation routines 

#include "camtypes.h"
#include "node.h"
#include "nodeattr.h"
#include "list.h"
#include "listitem.h"
#include "errors.h"
#include "ensure.h"
#include "objreg.h"
#include "ndoptmz.h"
#include "ink.h"
#include "nodetext.h"
#include "nodedoc.h"
#include "ndmldgrp.h"
#include "nodemldr.h"
#include "attrmgr.h"

// for the template attribute
#include "cxfrech.h"
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h" 

#include "brshattr.h"
#include "dbugtree.h"


DECLARE_SOURCE("$Revision: 662 $");
CC_IMPLEMENT_DYNCREATE( FactorOutCommonChildAttrAct, Action )
CC_IMPLEMENT_DYNCREATE( LocaliseCommonAttrAct, Action )

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	BOOL NodeRenderableInk::MakeAttributeComplete(Node* Root = NULL,
												  BOOL CheckForDuplicates = TRUE,
												  AttrTypeSet* pAffectedAttrTypes = NULL, 
												  BOOL IncludeDefaults = FALSE,
												  BOOL bIncludeEffectAttrs = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/94

	Inputs:		Root: 				The search for attributes will stop at this node. The 
									NULL default value means 'don't stop until we reach the top'.
				CheckForDuplicates: When TRUE a check is made to ensure that duplicate
									attribiutes are not added to the subtree (this is the
									default). When localising a groups attributes this 
									check will be unneccessary.
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only add attributes which have a type
									in this set.
				IncludeDefaults:	When TRUE default attributes will be added.
				bIncludeEffectAttrs	Only used when forcing effect attrs to become normal attrs
									(see Ungroup) TRUE to force attr scanning to find effect
									attrs as well as normal ones...
				 	
	Outputs:	-
	Returns:	TRUE if successful
				FALSE if we run out of memory, the children of the node will remain unchanged

	Purpose:	This function should be called on a subtree prior to it being moved, either
				to a new position in the document, or to another document. It is also used 
				to localise attributes; during an ungroup for example.  
				 
				It finds ALL attributes which are required by the subtree and adds 
				them as first children. 
				  
				After the subtree has been moved to its new position, the function 
				NodeRenderableInk::NormaliseAttributes should be called.

	SeeAlso:	NodeRenderableInk::NormaliseAttributes
	SeeAlso:	Range::MakeAttributeComplete

********************************************************************************************/

BOOL NodeRenderableInk::MakeAttributeComplete(Node* Root, 
											  BOOL CheckForDuplicates, 		  /* = TRUE */
											  AttrTypeSet* pAffectedAttrTypes, /* = NULL */ 
											  BOOL IncludeDefaults, /* = FALSE */
											  BOOL bIncludeEffectAttrs /* = FALSE */)
{
	Node* Current = NULL;  // Pointer to the current node in the tree 

	NodeAttribute* CurAttr;
	CCRuntimeClass* AttrType;
	BOOL Exists; 

	Node* PreFirstChild = FindFirstChild(); // Remember the FirstChild of the node before we add
										    // any new attributes, this will come in handy if we
										    // need to abort.

	// Loop until all attributes are copied, we are not interested in the defaults cos these are the
	// same for all docs !.
	if (bIncludeEffectAttrs)
		Current = NodeAttribute::FindFirstAppliedAttr(this, Root);
	else
		Current = NodeAttribute::FindPrevAppliedAttr(this, Root);

	while (Current && (IncludeDefaults || (!(IS_A(Current->FindParent(), NodeDocument)))) )
	{
		// Find the next node, snaking up the tree
		if (Current->IsAnAttribute())
		{
			CurAttr = (NodeAttribute*)Current;
			if (CurAttr->CanBeAppliedToObject() && CurAttr->ShouldBeOptimized())
			{
				AttrType = CurAttr->GetAttributeType();

				BOOL Required = RequiresAttrib(AttrType) || this->IsCompound();
				// Is the attribute required ?
				if (Required && (!pAffectedAttrTypes || pAffectedAttrTypes->InSet(AttrType)))
				{
					Exists = FALSE; 
					if (CheckForDuplicates)
					{
						// triggers can have duplicates
						if(!CurAttr->CanBeMultiplyApplied())
						{
							// Does the node already have this child attribute
							Exists = (GetChildAttrOfType(AttrType) != NULL);
						}
					}

					#ifdef _DEBUG
					if (!CheckForDuplicates)
					{
						// If we feel there is no need to check for duplicates then there shouldn't be any !
						if (!CurAttr->CanBeMultiplyApplied())
						{
							NodeAttribute* pChildAttr = GetChildAttrOfType(AttrType);
							if ((pChildAttr != NULL))
							{
#if DEBUG_TREE
								DebugTreeDlg::DumpSubTree(this, 4);
#endif
								TRACE(_T("Duplicate Attr found at %x %s\n"), pChildAttr, pChildAttr->GetRuntimeClass()->m_lpszClassName);
							}
//							ERROR3IF((pChildAttr != NULL), "MakeAttributeComplete: Duplicate attr found !"); 
						}
					}
					#endif

					if (!Exists)
					{
						// Make a copy of the attribute
						NodeAttribute* NewAttr = (NodeAttribute*)CurAttr->SimpleCopy();

						if (NewAttr == NULL)
						{
							goto OutOfMemory; 
						}

						// Now add the attribute to this node
						NewAttr->AttachNode(this, FIRSTCHILD, TRUE, FALSE);
					}
				}
			}
		}
		Current = NodeAttribute::FindPrevAppliedAttr(Current, Root);

		// in order to copy brush ink nodes we need to break if the parent is NULL, else is violates
		if (Current!=NULL && Current->FindParent() == NULL)
			break;
	} 

	return TRUE;

OutOfMemory:
	
	// Delete any new attributes added to the node
	Current = FindFirstChild(); 
	Node* Next; 

	while (Current != PreFirstChild)
	{
		ENSURE(Current != NULL, "PreFirstChild could not be found");
		ENSURE(Current->IsAnAttribute(), "Should be a NodeAttribute"); 
		
		Next = Current->FindNext();
		// Delete the attribute
		Current->CascadeDelete(); 
		delete Current; 		    

		Current = Next; 
	}

	return FALSE; 
}

/********************************************************************************************

>	void NodeRenderableInk::NormaliseAttributes()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function should be called whenever an attribute-complete node is inserted
				into the tree (whether it's a new node, or a node which has been moved 
				from elsewhere). 
				
				It removes all child attributes of the node which are the same type and value 
				as attributes defined further up the tree eg. the defaults
			
	Errors:		-
	SeeAlso:	NodeRenderableInk::MakeAttributeComplete

********************************************************************************************/

void NodeRenderableInk::NormaliseAttributes()
{

	NodeAttribute* ChildAttribute;	   
	NodeAttribute* GlobalAttribute;

	Node* LocalScan;
	Node* NextLocal;  
	
	// Scan the child attribute block
	LocalScan = FindFirstChild();
	// Stop if we hit an object or if there are no more children 
	// NOTE: Stopping scanning when we find the first RendereblInk node prevents
	// Effect attributes being Normalised. THIS IS DELIBERATE!
	while((LocalScan != NULL) && (!LocalScan->IsAnObject())) 
	{
		// Hand over hand (LocalScan may get deleted)
		NextLocal = LocalScan->FindNext();
		if(LocalScan->IsAnAttribute() && ((NodeAttribute*)LocalScan)->ShouldBeOptimized())	// cos it could be a NodeHidden
		{
			ChildAttribute = (NodeAttribute*)LocalScan;  
			// We now need to search up the tree to see if ChildAttribute is redundant
//		   	Node* GlobalScan = FindPreviousEffectiveNode();
			Node* GlobalScan = NodeAttribute::FindPrevAppliedAttr(this);
			// Search until we can go no higher
			while (GlobalScan != NULL)
			{
				if (GlobalScan->IsAnAttribute())
				{
					GlobalAttribute = (NodeAttribute*)GlobalScan; 
					// An attribute has been found, is it the same class as ChildAttribute ?
					if(GlobalAttribute->GetRuntimeClass() ==
					   ChildAttribute->GetRuntimeClass())
					{
						// Yes it is, so let's check if they are equal
						if ((*GlobalAttribute)==(*ChildAttribute))
						{
							// They are equal so we can nuke the child object because it's 
							// redundant
							ChildAttribute->CascadeDelete();
							delete ChildAttribute;
						}
						break;	 // don't search any further
					}

				}
//				GlobalScan = GlobalScan->FindPreviousEffectiveNode(); // climb higher
				GlobalScan = NodeAttribute::FindPrevAppliedAttr(GlobalScan);
			} 
		}
		LocalScan = NextLocal; // Get the next child 
	}    

}

/********************************************************************************************

>	void NodeRenderableInk::RemoveSuperfluousAttribs()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This fn does not exist.
	
	
				This function removes attributes from the subtree that we consider to
				be superfluous. An attribute is classed as superfluous if -

				a. It is overridden by an attribute of the same class before any ink object
				   has been rendered. 

				   e.g. Select a red pen, select a green pen, draw a circle

				   Here picking up the red pen is a waste of effort and so can be removed.

				   A subtree which has had the MakeAttributeComplete method called on it
				   will probably have attributes like these.

				b. The attribute (i.e. same class, same value) has already been defined 
				   
				   e.g. Select a red pen, draw a circle, select a red pen, draw a square

				   Again the default attributes which will be added by the MakeAttributeComplete
				   method will fall into this category.
				 
	Scope:		protected
	Errors:		-
	SeeAlso:	NodeRenderableInk::NormaliseAttributes
	SeeAlso:	NodeRenderableInk::MakeAttributeComplete

********************************************************************************************/
/*
void NodeRenderableInk::RemoveSuperfluousAttribs()
{
	// Local Scan is used to perform a depth first traversal of the subtree
	Node* LocalScan = FindFirstDepthFirst(); 

	// Global Scan is used to find attributes defined in and above the subtree
	Node* GlobalScan; 

	Node* ToDie = NULL; 
	
	// Attribute override flag. This gets set to true when we start processing each 
	// local attribute. 
	BOOL AttribOver;  
	
	// Traverse the tree in render order
	while (LocalScan != NULL)
	{
		if (LocalScan->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)))
		{
			// Ok weve found a node attribute, lets see if it's superfluous
			// We need to search back up the tree in reverse render order to determine this

			GlobalScan = LocalScan;
			
			// This flag gets set to FALSE as soon as we find an ink node or we move 
			// to a parent node.
			AttribOver = TRUE;  

			while (GlobalScan != NULL)
			{
				// Find the next node 
				if (GlobalScan->FindPrevious() != NULL)
				{
					GlobalScan = GlobalScan->FindPrevious(); 	// Search leftmost siblings of 
																// the subtree first
				}
				else 
				{
					GlobalScan = GlobalScan->FindParent();   // then move to the parent
					AttribOver = FALSE;
				}

				if (ToDie != NULL)
				{
					// Set at (a)
					ToDie->UnlinkNodeFromTree(); 
					delete ToDie;
					ToDie = NULL;
				}

				if (GlobalScan != NULL)
				{
					if (AttribOver)
					{
						if (GlobalScan->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
						{
							AttribOver = FALSE; 
						}
					}
					else if (GlobalScan->GetRuntimeClass() == LocalScan->GetRuntimeClass())
					{
					#if _DEBUG
						if (IsUserName("Simon"))
						{
							TRACE( _T("Local attrib = %s, Global = %s"), LocalScan->Name(), GlobalScan->Name()); 
						}
					#endif

						if (AttribOver)
						{
							// The GlobalScan Attribute is redundant
							// (a)
							ToDie = GlobalScan;
							// continue the search
						}

						// An attribute of the same class is found, do they have the 
						// same value

						else if ( (*((NodeAttribute*)LocalScan)) == (*((NodeAttribute*)GlobalScan)))
						{
							// The attribute is superfluous so kill it 
							//(b)
							ToDie = LocalScan; 
							break;// Terminate the global scan  
						}
					
					}
				}
			}
		}
		// Get the next node in the subtree
		LocalScan = LocalScan->FindNextDepthFirst(this); 

		if (ToDie != NULL)
		{
			// Set at (b)
			ToDie->UnlinkNodeFromTree(); 
			delete ToDie;
			ToDie = NULL;
		} 
	}	
} 
*/


/********************************************************************************************

>	static BOOL NodeRenderableInk::FindCommonAttributesToFactorOut(CommonAttrSet* CommonAttributeSet)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Inputs:		-
	Outputs:	All items in the CommonAttributeSet are attributes common to all objects
				within the group. The attributes applied to objects which can discard their
				attributes (like the caret) are ignored

	Returns:	TRUE if successful, FALSE otherwise (out of memory) 
	Purpose:	Finds all attributes common to the compound node
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/
/*
Technical Notes:

(Method)

CommonAttributeSet = the set of all attributes 
Each attribute in the set has a NULL value.

Scan all grouped objects (Current)
	Scan each attribute in the common attribute set in turn (CommonA)
		If Current requires the attribute
			If Current has an attribute with the same type as CommonA then
				If CommonA == NULL
					CommonA = Currents attribute
				Else if (CommonA != Currents attribute)
					The attribute is not common so remove it from the common attribute set.
			else
				The attribute is not common so remove it from the common attribute set.	
	EndScan
	Current = Next object in group
EndScan
*/

BOOL NodeRenderableInk::FindCommonAttributesToFactorOut(CommonAttrSet* CommonAttributeSet)
{
	CommonAttributeItem* CommonAttrItem;
	CommonAttributeItem* NextCommonAttrItem;

	// Place all attributes in the CommonAttributeSet
	if (!CommonAttributeSet->AddAllTypesToSet())
	{
		return FALSE; // ERROR already called 
	}

	// Scan all grouped objects
	for (Node* CurrentObject = FindFirstChild();  
		 CurrentObject != NULL;
		 CurrentObject = CurrentObject->FindNext())
	{
		if (CurrentObject->IsAnObject())
		{
			
			// Scan all attributes in the CommonAttributeSet in turn
			CommonAttrItem = (CommonAttributeItem*)CommonAttributeSet->GetHead();

			while(CommonAttrItem != NULL)
			{
				// Hand over hand cos we may well delete the CommonAttrItem
				NextCommonAttrItem = (CommonAttributeItem*)(CommonAttributeSet->GetNext(CommonAttrItem));

				// Does CurrentObject require the attribute	to render
				if ( (((NodeRenderableInk*)CurrentObject)->RequiresAttrib(CommonAttrItem->AttrType)) || CurrentObject->IsCompound())
				{
					BOOL DeleteCommonAttr = FALSE; // Until we know better
            
					// Ok the current object requires the attribute
					// Does the CurrentObject have a child attribute of this type ?
					NodeAttribute* pAttrNode = 
						((NodeRenderableInk*)CurrentObject)->GetChildAttrOfType(CommonAttrItem->AttrType);
					if (pAttrNode != NULL && pAttrNode->ShouldBeOptimized())
					{
						// Ok it has an attribute of this type
						if (CommonAttrItem->pAttr == NULL)
						{
							// The attribute becomes a common attribute
							CommonAttrItem->pAttr = pAttrNode;
							CommonAttrItem->Status = Range::ATTR_COMMON; 
						}
						else if(CommonAttrItem->pAttr->GetRuntimeClass() ==
							    pAttrNode->GetRuntimeClass())
						{
							// Ok they are the same runtime class but are they equal
							if (!((*pAttrNode)==(*(CommonAttrItem->pAttr))))
							{
								// They are not equal so remove CommonAttrItem from the
								// common attribute set.
								DeleteCommonAttr = TRUE; 
							}
							// DY 12/5/2000 AttrBrushTypes cannot be factored because they 
							// may contain caches of pressure or timestamp data which apply
							// to a specific node only.
							// They no longer contain this data so factor them like normal!
							//if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBrushType)))
							//	DeleteCommonAttr = TRUE;
						}
						else 
						{
							// They cannot be the same value cos they are different runtime types
							DeleteCommonAttr = TRUE; 
						}
					}
					else 
					{
						// The CurrentObject does not have an attribute of this type so it
						// cannot be common
						DeleteCommonAttr = TRUE; 
					}

					if (DeleteCommonAttr)
					{
						delete(CommonAttributeSet->RemoveItem(CommonAttrItem)); 
					}
				}

				CommonAttrItem = NextCommonAttrItem;
			}
		}
		// Removed because there are circumstances where certain attributes have 
		// already been factored out eg. Corel filter
		//else 
		//{
		//	ENSURE(CurrentObject->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeHidden), 
		//				"It's not an object, it's not a hidden node, so what is it ??");
		//}
	} 	
	// Delete all NULL items in the CommonAttributeSet
	CommonAttrItem = (CommonAttributeItem*)CommonAttributeSet->GetHead();
	while (CommonAttrItem != NULL)
	{
		CommonAttributeItem* Next = (CommonAttributeItem*)CommonAttributeSet->GetNext(CommonAttrItem);
		if (CommonAttrItem->pAttr == NULL)
		{
			// Item is a non required attribute so zap it
			delete (CommonAttributeSet->RemoveItem(CommonAttrItem)); 
		}
		CommonAttrItem = Next; 
	}
	
	return TRUE; // Job done
}




/********************************************************************************************

>	static BOOL NodeRenderableInk::FactorOutCommonChildAttributes(BOOL Global = FALSE, 
																  AttrTypeSet* pAffectedAttrTypes = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94

	Inputs:		Global:	When TRUE we recursively factor out attributes in all parent compounds 
						of the CompoundObject (Bottom up)
								  	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider factoring out  those attributes
									which have a type which is in this set. 

	Outputs:	TRUE if successful, FALSE otherwise (out of memory)

	Returns:	This function finds all attributes which are common to all children
				of this compound object, adds the common attributes to this node and
				deletes them from its children. 

				If the compound contains a node which can discard its attributes (like the caret)
				then its attributes are not considered when factoring out.


	Purpose:	
	Scope:		public
	Errors:		-
	Document:	atroptmz.doc
	SeeAlso:	-

********************************************************************************************/

BOOL NodeRenderableInk::FactorOutCommonChildAttributes(BOOL Global, AttrTypeSet* pAffectedAttrTypes)
{
	// This function should only ever get called on a compound object
	ENSURE(IsCompound(), "FactorOutCommonChildAttributes called on a non compound object"); 
	

	// Try to factor out the neccessary attributes
	if (!FactorOutCommonChildAttrHelper(Global, pAffectedAttrTypes))
	{
		// Tidyup then return FAIL
		DeleteLocalisedAttributes(Global, pAffectedAttrTypes); // tidyup
		return FALSE; // Failed
	}
	else
	{
		// The only thing that remains now is to delete all remaining factored out attributes
		DeleteFactoredOutAttribs(Global, pAffectedAttrTypes); 
	}

	return TRUE; // Success
}

/********************************************************************************************

>	BOOL NodeRenderableInk::FactorOutCommonChildAttributesHelper(BOOL Global, 
																 AttrTypeSet* pAffectedAttrTypes)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/95
	Inputs:		Global:	When TRUE we recursively factor out attributes in all parent compounds 
						of the CompoundObject (Bottom up)
								  	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider factoring out  those attributes
									which have a type which is in this set. 
	Outputs:	-
	Returns:	-
	Purpose:	This is a helper function for FactorOutCommonChildAttributes. It finds all 
				attributes which are common to all children of this compound object and 
				adds the common attributes to this node. If Global is TRUE then it recursivly
				tries to factor out the attributes on all parent compounds. This function
				does not delete the attributes which are factored out, This is left to
				the calling function.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NodeRenderableInk::FactorOutCommonChildAttrHelper(BOOL Global, AttrTypeSet* pAffectedAttrTypes)
{
	// This function should only ever get called on a compound object
	ENSURE(IsCompound(), "FactorOutCommonChildAttributes called on a non compound object"); 

 	CommonAttrSet CommonAttributeSet; // A list of CommonAttributeItems
	
	if (!FindCommonAttributesToFactorOut(&CommonAttributeSet))	 // Ignores attr discard nodes
	{
		return FALSE;  
	} 
	
	NodeAttribute* pFactoredOutAttr;

	// Ok let's add the common attributes to the first child of the group 
	CommonAttributeItem* pCommonAttr; 
	for (pCommonAttr = (CommonAttributeItem*)CommonAttributeSet.GetHead();
		 pCommonAttr != NULL;
		 pCommonAttr = (CommonAttributeItem*)CommonAttributeSet.GetNext(pCommonAttr)) 
	{
		// Is the common attribute an attribute which should be factored out ?
		if (!pAffectedAttrTypes || (pAffectedAttrTypes->InSet(pCommonAttr->pAttr->GetAttributeType())) )
		{
			//pCommonAttr->pAttr->MoveNode(this, FIRSTCHILD);    
			// Take a copy of the node and insert it as a first child
			pFactoredOutAttr = (NodeAttribute*)(pCommonAttr->pAttr->SimpleCopy());
			if (!pFactoredOutAttr)
				return FALSE; 
			pFactoredOutAttr->AttachNode(this, FIRSTCHILD, TRUE, FALSE);
		}  
	}

	// The CommonAttributeSet is no longer required
	CommonAttributeSet.DeleteAll(); 

	// Do we need to factor out the parents attributes ?
	if (Global)
	{
		Node* pParent = FindParent();
		if (pParent && (pParent->IsCompound()))
		{
			// We need to localise the parent's attributes first  (Recursive bit)
			if (!(((NodeRenderableInk*)pParent)->FactorOutCommonChildAttrHelper(TRUE, pAffectedAttrTypes)))
			{
				return FALSE; // Failed
			} 
		}
	}
	return TRUE; // Success
}


/********************************************************************************************

>	BOOL NodeRenderableInk::LocaliseCommonAttributes(BOOL CheckForDuplicates = FALSE, 
													 BOOL Global = FALSE, 
													 AttrTypeSet* pAffectedAttrTypes = NULL,
													 BOOL RecursiveForChildren = FALSE); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/94
	
	Inputs:		CheckForDuplicates:	This should be TRUE if the children of this compound 
									node could have child attributes with the same
									type as the compound's attributes. This should never
									be the case.

				Global: TRUE indicates that we should localise the attributes on all
						parent compound nodes (top down), before localising the attributes for
						this compound.

	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider localising  those attributes
									which have a type which is in this set. 
				RecursiveForChildren: Passed onto the helper function, means that if we have children
									 who are compound then they will have LocaliseCommonAttrs called on
									 them also
	Outputs:	-
	Returns:	TRUE if successful
				FALSE if we run out of memory, in this situation the NodeRenderableInk is left unchanged

	Purpose:	This function is the opposite of the FactorOutCommonChildAttributes	function
				it copies all attributes common to the compound to each child object within the
				compound which requires each attribute. The groups common attributes are deleted.

				The routine does not localise attributes on objects which can discard their child
				attributes (like the caret)
			
	Errors:		-
	SeeAlso:	NodeRenderableInk::FactorOutCommonChildAttributes

********************************************************************************************/

BOOL NodeRenderableInk::LocaliseCommonAttributes(BOOL CheckForDuplicates,  
												 BOOL Global,
												 AttrTypeSet* pAffectedAttrTypes,
												 BOOL RecursiveForChildren) 
{
	// This function should only ever get called on a compound object
	ENSURE(IsCompound(), "LocaliseCommonAttributes called on a non compound object"); 
	
	// First of all let's try and localise the attributes. This is the bit that can fail
	if (!LocaliseCommonAttrHelper(CheckForDuplicates, Global, pAffectedAttrTypes, RecursiveForChildren))
	{
		// We ran out of memory so we must, delete all attributes which were localised
		DeleteFactoredOutAttribs(Global, pAffectedAttrTypes); 
		return FALSE; 
	}
	else
	{
		// We managed to localise all the attributes so it is now safe to delete
		// them from the compounds.
		DeleteLocalisedAttributes(Global, pAffectedAttrTypes); 
		return TRUE;
	}
}

/********************************************************************************************

>	BOOL NodeRenderableInk::LocaliseCommonAttrHelper(BOOL CheckForDuplicates, 
												 	 BOOL Global, 
												 	 AttrTypeSet* pAffectedAttrTypes,
													 BOOL RecursiveDownwards)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	
	Inputs:		CheckForDuplicates:	This should be TRUE if the children of this compound 
									node could have child attributes with the same
									type as the compound's attributes. This should never
									be the case.

				Global: TRUE indicates that we should localise the attributes on all
						parent compound nodes (top down), before localising the attributes for
						this compound.

	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider localising  those attributes
									which have a type which is in this set. 
				RecursiveDownwards: Passed on to the attribute helper, means that if we have a child
									who is also compound then we will also localise their attributes.
									Defaults to FALSE.

	Outputs:	-
	Returns:	TRUE if successful
				FALSE if we run out of memory, the localised attributes are not deleted
				in this function.

	Purpose:	This function copies all attributes common to the compound to each child object 
				within the compound which requires each attribute. 

				The routine does not localise attributes on objects which can discard their child
				attributes (like the caret)
				

	Scope:		private

	SeeAlso:	NodeRenderableInk::LocaliseCommonAttributes

********************************************************************************************/


BOOL NodeRenderableInk::LocaliseCommonAttrHelper(BOOL CheckForDuplicates,
												 BOOL Global,
												 AttrTypeSet* pAffectedAttrTypes,
												 BOOL RecursiveForChildren)
{
	// Do we need to globally optimise ?
	if (Global)
	{
		Node* pParent = FindParent(); 
		if (pParent && (pParent->IsCompound()))
		{
			// We need to localise the parent's attributes first  (Recursive bit)
			if (!(((NodeRenderableInk*)pParent)->LocaliseCommonAttrHelper(CheckForDuplicates, TRUE, pAffectedAttrTypes)))
			{
				return FALSE; // Failed
			} 
		}
	}
			   
	// add all necessary attrs to each child object in the group
	for (Node* Current=FindFirstChild(); Current!=NULL; Current=Current->FindNext())
	{
		if (RecursiveForChildren && Current->IsCompound())
			((NodeRenderableInk*)Current)->LocaliseCommonAttributes(FALSE, FALSE, NULL, TRUE);
	
		if (Current->IsAnObject())
		{
			NodeRenderableInk* pObject = (NodeRenderableInk*)Current;
			BOOL ChkForDups = CheckForDuplicates;
			Node* pRoot = RecursiveForChildren ? NULL : this;
			if(!pObject->MakeAttributeComplete(pRoot,ChkForDups,pAffectedAttrTypes)) 	
				return FALSE; 	// out of memory
		}
	}

	return TRUE; 
}

/********************************************************************************************

>	void NodeRenderableInk::DeleteLocalisedAttributes(BOOL Global = FALSE 
												  	  AttrTypeSet* pAffectedAttrTypes)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	
	Inputs:		Global: TRUE indicates that we should delete the attributes on all
						parent compound nodes as well as this node's attributes

	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider deleting  those attributes
									which have a type which is in this set.

	Scope:		private

	SeeAlso:	NodeRenderableInk::LocaliseCommonAttributes

********************************************************************************************/

void NodeRenderableInk::DeleteLocalisedAttributes(BOOL Global,
												  AttrTypeSet* pAffectedAttrTypes )
{
	// Do we need to delete any parent compound's attributes
	if (Global)
	{
		Node* pParent = FindParent(); 
		if (pParent && pParent->IsCompound())
		{
			// We need to localise the parent's attributes first  (Recursive bit)
			((NodeRenderableInk*)pParent)->DeleteLocalisedAttributes(Global, pAffectedAttrTypes); 
		}
	}


	// Delete all moved attributes
	Node* Next; 
	Node* Current = FindFirstChild();
	while (Current != NULL && !Current->IsAnObject())
	{
		Next = Current->FindNext(); 
		if (Current->IsAnAttribute())
		{
			BOOL DeleteAttr = TRUE;
			if (pAffectedAttrTypes)
			{
				// Only delete the attribute if it has been localised
				if (!(pAffectedAttrTypes->InSet(((NodeAttribute*)Current)->GetAttributeType())))
					DeleteAttr = FALSE;  
			}
			if (DeleteAttr)
			{
				// Die
				// Ilan 19/04/00
				// More generally speaking, all attributes which returned FALSE in their ShouldBeOptimized()
				// calls, will not have been localised to the child nodes during DoLocaliseCommonAttributes().
				// Hence don't want to delete these attributes, they must remain attached to the group node
				// which we are ungrouping so that they are present on an UNDO of a OpUngroup
				// Attributes in this category include Feather attibutes and TemplateAttributes at present
//				if (!Current->IsKindOf(CC_RUNTIME_CLASS(TemplateAttribute)))
				if( ((NodeAttribute*)Current)->ShouldBeOptimized())
				{
					Current->CascadeDelete(); 
					delete Current;
				}
			}	
		}
		Current = Next;	
	}
}
 
/********************************************************************************************

>	void NodeRenderableInk::DeleteFactoredOutAttribs(BOOL Global = FALSE 
												  	 AttrTypeSet* pAffectedAttrTypes)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	
	Inputs:		Global: TRUE indicates that we should delete the attributes on all
						parent compound nodes as well as this node's attributes

	
				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider deleting  those attributes
									which have a type which is in this set.


	Purpose:	Deletes all child attributes of objects within the compound, which are defined
				in the groups child attribute block. The attributes of objects which discard
				their child attributes (like the caret) are only deleted if they have the same value
	Scope:		private

********************************************************************************************/

void NodeRenderableInk::DeleteFactoredOutAttribs(BOOL Global, AttrTypeSet* pAffectedAttrTypes)
{
	Node* pGroupNode = FindFirstChild(); 
	while(pGroupNode!=NULL)
	{
		// Karim 30/08/2000
		// Non-optimisable attributes, like feathers and names,
		// must not be automatically deleted - only automatically delete optimisable ones.
		if (pGroupNode->IsAnAttribute() && ((NodeAttribute*)pGroupNode)->ShouldBeOptimized())
		{
			NodeAttribute* pGroupAttr = (NodeAttribute*)pGroupNode;
			CCRuntimeClass* GrouptAttrType = pGroupAttr->GetAttributeType();
			if (pAffectedAttrTypes==NULL || pAffectedAttrTypes->InSet(GrouptAttrType))
			{
				// delete this group attr type from all child objects of this group
				// BUT if obj discards child attrs only delete attr if it also has same value
				for (Node* pNode=FindFirstChild(); pNode!=NULL; pNode=pNode->FindNext())
				{
					if (pNode->IsAnObject())
					{
						NodeRenderableInk* pObject = (NodeRenderableInk*)pNode;
						NodeAttribute* pDeadAttr = pObject->GetChildAttrOfType(GrouptAttrType);
						if (pDeadAttr!=NULL)
						{
							// This code used to only test the attribute for equality if pObject
							// returned TRUE from DiscardsAttributeChildren, otherwise it would 
							// just assume that they are identical and delete it.
							// The DiscardAttributeChildren checks are now done elsewhere so 
							// this code now just assumes it can delete any attributes that have 
							// got this far.
							// This optimisation relies on the tree being in a "legal" state 
							// at the start (i.e. correctly optimised) and also helps to correct 
							// problems where attributes may have been incorrectly left on children
							// (though such "corrections" may change the appearance of the document).

							pDeadAttr->CascadeDelete(); 
							delete pDeadAttr;	
						}
					}
				}
			}
		}
		pGroupNode = pGroupNode->FindNext(); 		
	}

	// Do we need to delete any parent compound's attributes
	if (Global)
	{
		Node* pParent = FindParent(); 
		if (pParent && (pParent->IsCompound()))
		{
			// We need to delete the parent's attributes first  (Recursive bit)
			((NodeRenderableInk*)pParent)->DeleteFactoredOutAttribs(Global, pAffectedAttrTypes); 
		}
	}
} 


/********************************************************************************************

>	BOOL Node::OptimiseAttributes()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This function should be called on a subtree
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL Node::OptimiseAttributes()
{
	Node* Current= this->FindFirstDepthFirst(); 

	while (Current != NULL)
	{
		if (Current->IsCompound())
		{
			ENSURE(Current->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)), "Compound should be a NodeRenderableInk");
			// It's a compound node, so factor out common attributes
			// We don't want to attribute inside moulds (as yet)
			if ( !((IS_A(Current, NodeMouldGroup)) || (IS_A(Current, NodeMoulder))) )
			{
				if (!((NodeRenderableInk*)Current)->FactorOutCommonChildAttributes())
				{
					return FALSE; 
				}
			}
		}
		Current = Current->FindNextDepthFirst(this); 
	}

	return TRUE; 
}
	
//------------------------------------------------------------------------------------------
// FactorOutCommonChildAttrAct methods
    
/********************************************************************************************

>	FactorOutCommonChildAttrAct::FactorOutCommonChildAttrAct()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	FactorOutCommonChildAttrAct constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
    
FactorOutCommonChildAttrAct::FactorOutCommonChildAttrAct()
{
}   

/********************************************************************************************

>	ActionCode FactorOutCommonChildAttrAct::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the FactorOutCommonChildAttrAct which factors out all attributes 
				which are common to all children of the compound object. All common attributes 
				become first children of the compound object.
	Errors:		-
	SeeAlso:	LocaliseCommonAttrAct
                                                        
    
********************************************************************************************/

ActionCode FactorOutCommonChildAttrAct::Execute()
{
	LocaliseCommonAttrAct* NewAct;  
    ActionCode ActCode;  

    // Attempt to initialise the LocaliseCommonAttrAct action which will localise the attributes
    if ((ActCode = LocaliseCommonAttrAct::Init(pOperation,                    
								     		   pOppositeActLst,  
											   CompoundObj,
											   Global,  
											   pSetOfAffectedAttrTypes,
						 			 		   ( Action**)(&NewAct))) != AC_FAIL) 
	{ 
		if (!CompoundObj->FactorOutCommonChildAttributes(Global, pSetOfAffectedAttrTypes))
		{
			return AC_FAIL; 
		}  	                
 	}             
	return (ActCode);                   	 
}    

/********************************************************************************************

>	static ActionCode FactorOutCommonChildAttrAct::Init(Operation* const pOp, 
										   				ActionList* pActionList,
														NodeRenderableInk* pCompound,
														BOOL Global, 
														AttrTypeSet* pAffectedAttrTypes, 
										   				Action** NewAction); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object

				pCompound:	 The compound object

				Global:	When TRUE the attributes on all parent compounds are also
						factored out.

				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider factoring out  those attributes
									which have a type which is in this set. 


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
				of a FactorOutCommonChildAttrAct.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode FactorOutCommonChildAttrAct::Init(Operation* const pOp, 
						   ActionList* pActionList,
						   NodeRenderableInk* pCompound,
						   BOOL global, 
						   AttrTypeSet* pAffectedAttrTypes, 
						   Action** NewAction) 

{  
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(FactorOutCommonChildAttrAct), 
					 CC_RUNTIME_CLASS(FactorOutCommonChildAttrAct), 
					 NewAction)); 

	if (*NewAction != NULL) 
	{
		((FactorOutCommonChildAttrAct*)(*NewAction))->CompoundObj = pCompound;
		((FactorOutCommonChildAttrAct*)(*NewAction))->pSetOfAffectedAttrTypes = pAffectedAttrTypes;
		((FactorOutCommonChildAttrAct*)(*NewAction))->Global = global;
	}
				  
	return (Ac); 
} 

/********************************************************************************************

>	virtual void FactorOutCommonChildAttrAct::Slaughter()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor which gets called when an operation is deleted  
	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/

void FactorOutCommonChildAttrAct::Slaughter() 
{         
	// Destroy the set of attribute types
	if (pSetOfAffectedAttrTypes)
	{
		pSetOfAffectedAttrTypes->DeleteAll(); 
		delete pSetOfAffectedAttrTypes;
	}

	// call the base class to destroy the action
	Action::Slaughter();
}   


//------------------------------------------------------------------------------------------
// LocaliseCommonAttrAct methods
    
/********************************************************************************************

>	LocaliseCommonAttrAct::LocaliseCommonAttrAct()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LocaliseCommonAttrAct constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
    
    
LocaliseCommonAttrAct::LocaliseCommonAttrAct()
{
}   

/********************************************************************************************

>	ActionCode LocaliseCommonAttrAct::Execute()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		-
	Outputs:	ActionCode indicating if the action was successfully executed or not
	Returns:	-
	Purpose:	Executes the LocaliseCommonAttrAct which copies all attributes common to the 
				compound object to each child object within the group which requires each 
				attribute. The groups common attributes are deleted.
	Errors:		-
	SeeAlso:	FactorOutCommonChildAttrAct
                                                        
    
********************************************************************************************/

ActionCode LocaliseCommonAttrAct::Execute()
{
	FactorOutCommonChildAttrAct* NewAct;  
    ActionCode ActCode;  

    // Attempt to initialise the FactorOutCommonChildAttrAct action
    if ((ActCode = FactorOutCommonChildAttrAct::Init(pOperation,                    
								     		   pOppositeActLst,  
											   CompoundObj, 
											   Global,
											   pSetOfAffectedAttrTypes,
						 			 		   ( Action**)(&NewAct))) != AC_FAIL) 
	{ 
		if (!CompoundObj->LocaliseCommonAttributes(TRUE, 	// Always check for duplicates when undoing
												   Global, 
												   pSetOfAffectedAttrTypes))
		{
			return AC_FAIL; 
		}  	                
 	}             
	return (ActCode);                   	 
}    

/********************************************************************************************

>	static ActionCode LocaliseCommonAttrAct::Init(Operation* const pOp, 
										   				ActionList* pActionList,
														NodeRenderableInk* pCompound,
														BOOL Global, 
														AttrTypeSet* pAffectedAttrTypes, 
										   				Action** NewAction); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/93  
	
	Inputs:		pOp: The operation to which the action should be added
				
				pActionList: The action list in the operation object

				pCompound:	 The compound object

				
				Global: TRUE indicates that we should localise the attributes on all
						parent compound nodes (top down), before localising the attributes for
						this compound.


				pAffectedAttrTypes: An optional set of attribute types. If this is specified
									then we only consider localising those attributes
									which have a type which is in this set. 


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
				of a LocaliseCommonAttrAct.
	Errors:		-
	SeeAlso:	Action::Init

********************************************************************************************/

ActionCode LocaliseCommonAttrAct::Init(Operation* const pOp, 
						   ActionList* pActionList,
						   NodeRenderableInk* pCompound,
						   BOOL global, 
						   AttrTypeSet* pAffectedAttrTypes,
						   Action** NewAction) 

{  
	ActionCode Ac = (Action::Init(pOp,
					 pActionList,
					 sizeof(LocaliseCommonAttrAct), 
					 CC_RUNTIME_CLASS(LocaliseCommonAttrAct), 
					 NewAction)); 

	if (*NewAction != NULL) 
	{
		((LocaliseCommonAttrAct*)(*NewAction))->CompoundObj = pCompound;
		((LocaliseCommonAttrAct*)(*NewAction))->Global = global;
		((LocaliseCommonAttrAct*)(*NewAction))->pSetOfAffectedAttrTypes = pAffectedAttrTypes;

	}
				  
	return (Ac); 
}   


/********************************************************************************************

>	virtual void LocaliseCommonAttrAct::Slaughter()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/05/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	destructor which gets called when an operation is deleted  
	Errors:		-
	SeeAlso:	Action::Slaughter

********************************************************************************************/

void LocaliseCommonAttrAct::Slaughter() 
{         
	// Destroy the set of attribute types
	if (pSetOfAffectedAttrTypes)
	{
		pSetOfAffectedAttrTypes->DeleteAll(); 
		delete pSetOfAffectedAttrTypes;
	}

	// call the base class to destroy the action
	Action::Slaughter();
}   
