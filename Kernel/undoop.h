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

#ifndef INC_UNDOOP
#define INC_UNDOOP         

#include "ops.h" 
#include "node.h"				// for classas and AttachNodeDirection
#include "pathtype.h"

class NodeRenderableInk;
class NodePath;
class Path;
class AttrBrushType;


/********************************************************************************************

class ObjectItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Purpose:	ObjectItems are held in ObjectSets
	SeeAlso:	ObjectSet
	SeeAlso:	Node::AppliedAttrSet

********************************************************************************************/

class ObjectItem: public ListItem
{
public:
	NodeRenderableInk* pObject;
};


/********************************************************************************************

>	class ObjectSet: public List

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	A set of NodeRenderableInk objects
	SeeAlso:	ObjectItem

********************************************************************************************/

class ObjectSet: public List
{
public:
	
	// A function to return a copy of the ObjectSet
	ObjectSet* CopySet();
	
	// A simple function to add attr types to the AppliedAttrSet (Chiecks for duplicates)
	virtual BOOL AddToSet(NodeRenderableInk* pObject);
						   
	// A function to determine if pObject is in the set
	BOOL InSet(NodeRenderableInk* pObject); 
	
	~ObjectSet() { DeleteAll(); };
 		  
};



/********************************************************************************************

>	class PreDelFactorOutInfoItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Purpose:	PreDelFactorOutInfoItems are used by the GetPreDelFactorOutInfo and 
  				DoFactorOutAfterDeletion functions. See these for a description
				of its purpose.

	SeeAlso:	-

********************************************************************************************/

class PreDelFactorOutInfoItem: public ListItem
{
	public:
		NodeRenderableInk* pCompound;
		AttrTypeSet AttrTypesToFactorOut;
};


/********************************************************************************************

>	class UndoableOperation: public Operation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/94
	Purpose:	All Undoable operations should be derived from this class. 
	SeeAlso:	Operation

********************************************************************************************/


class UndoableOperation: public Operation
{
	CC_DECLARE_DYNCREATE( UndoableOperation ) 

public:	
	// -------------------------------------------------------------------------------------
	// The all new Do functions 
	
	// These functions are the operation's high level interface, a shiny veneer protecting
	// you from such beasties as system integrity, and attribute optimisation. Each Do
	// function performs a 'bit' of an operation and generates the actions neccessary to 
	// undo/redo the damage it performs. The functions each return a BOOL
	// Success/Failure status. 
	
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Whenever a DO function fails the operation should tidyup
	// and then call End(). It should not under any circumstance call any other Do functions 
	// or create any other actions. 
	
	// Do functions which operate on nodes normally come in two flavours. The generally
	// more useful flavour takes a range of nodes as its input (usually the range
	// of selected nodes). Also provided is a flavour which operates on a single node. 

	// Some Do functions have out of necessity to create an action for every node in the
	// range which is supplied to it. An example of such a function is DoMoveNodes which 
	// needs to record the old position of every node which moves in the tree. 
	//    Other Do functions like DoInvalidateNodesRegions need only to create a single 
	// action which stores the nodes it must operate on compactly in a range. If you are
	// to perform an action (for want of a better word) on a range of nodes, then you 
	// would be extremely silly to call the single node flavour for each node in the 
	// range, (they are very memory greedy). Even if you know that a function has 
	// currently to create an action for each range member it is far better to call the
	// range flavour because it could all change in the future. 

	// Operation History size integrity
	// --------------------------------

	// In Camelot it is important that the size of the operation history accurately reflects 
	// the amount of memory which has been allocated for undo and redo information.

	// Most of the time you are shielded from worrying about such things. However some of the 
	// lower level do functions require a bit of information to help them to calculate the 
	// size of the undo/redo information.  

	// A good example of this is the DoHideNode function which takes in an IncludeSubtreeSize 
	// flag. This indicates if the size of the subtree being hidden should be considered as 
	// 'in' the operation history or not.

	// DoHideNode is used whenever we need to remove an object from the tree. In this 
	// circumstance	we would set IncludeSubtreeSize = TRUE, because the subtree which has 
	// been removed, should be considered 'in' the Operation History. 

	// DoHideNode is also used whenever we move a subtree from one position in the document
	// tree to another position in the tree. In this circumstance we set  
	// IncludeSubtreeSize = FALSE, because the subtree which is being hidden is in the 
	// document still, not in the operation history. 


	// *************************************************************************************
	// Low level Do functions
	// A Low level Do function is a function which usually gets gets called by higher level 
	// Do functions. It generally creates a single action. 

	// -------------------------------------------------------------------------------------
	// Functions for invalidating regions 

	// The reason these functions require a spread parameter is that very often the node 
	// that needs invalidating is hidden. 
	BOOL DoInvalidateNodesRegions(Range NodeRange, 
							 	  BOOL IncludeBlobs,
							 	  BOOL UndoBlobs = FALSE,
							 	  BOOL IfBgRedraw = FALSE,
								  BOOL bForceRecache = TRUE);

	BOOL DoInvalidateNodeRegion(NodeRenderableBounded* Node,
						   		BOOL IncludeBlobs,
							 	BOOL UndoBlobs = FALSE,
						 	    BOOL IfBgRedraw = FALSE,
								BOOL bForceRecache = TRUE);

	// Function to invalidate an arbitary region on a spread
	BOOL DoInvalidateRegion (Spread *pSpread, const DocRect &InvalidRegion );

	// -------------------------------------------------------------------------------------
	// These functions save a copy of the specified nodes. The copies are restored when we undo. 

	BOOL DoSaveCopyOfNode(NodeRenderable* Node); 
	BOOL DoSaveCopyOfNodes(Range NodeRange); 

	// -------------------------------------------------------------------------------------
	// functions for hiding subtrees

	// This function must be used if a pointer to the NodeHidden hiding the NodeToHide is 
	// required
	BOOL DoHideNode(Node* NodeToHide, BOOL IncludeSubtreeSize, NodeHidden** nodeHidden = NULL, BOOL TellSubtree = TRUE); 	 
	
	BOOL DoHideNodes(Range NodeRange, BOOL IncludeSubtreeSize, BOOL TellSubtree = TRUE); 
	BOOL DoHideComplexRange(Range& RangeToHide);

	// *************************************************************************************
	// High level Do functions 

	// This function inserts a new node onto the active layer of a spread 
	BOOL DoInsertNewNode(NodeRenderableBounded* NewNode, 
					 	 Spread *pSpread, 
					 	 //DocRect Bounds, 
					 	 BOOL InvalidateRgn,
					 	 BOOL ClearSelection = TRUE, 
					 	 BOOL SelectNewObject = TRUE, 
						 BOOL NormaliseAttributes = TRUE); 
			
	// This function inserts a new node attached to ContextNode in the direction specified 	

	BOOL DoInsertNewNode(NodeRenderableBounded* NewNode, 
					 	 Node* ContextNode,
					 	 AttachNodeDirection Direction, 
					 	 //DocRect Bounds, 
					 	 BOOL InvalidateRegion,
					 	 BOOL ClearSelection = TRUE, 
					 	 BOOL SelectNewObject = TRUE, 
						 BOOL NormaliseAttributes = TRUE);

	// --------------------------------------------------------------------------------------
	// Functions to move a node to another position in the document tree

	BOOL DoMoveNode(Node* MoveNode, 
  					Node* Destination, 
					AttachNodeDirection Direction); 
					
	BOOL DoMoveNodes(Range NodeRange, 
					 Node* Destination, 
					 AttachNodeDirection Direction);   
				
	// This static BOOL is a bit of a bodge.
	// It is set while a node is being moved, so that the undo ops know not
	// to call the 'HidingNode'	function, which should only be called when
	// a node is 'really' being hidden.
	static BOOL MovingNode;
	
	// --------------------------------------------------------------------------------------
	// Functions to transform objects 

	BOOL DoTransformNode(NodeRenderableInk* NodeToTransform, 
						 TransformBase* T);

	BOOL DoTransformNodes(Range NodeRange, 		   
						 TransformBase* T);

	// --------------------------------------------------------------------------------------

   	BOOL DoMakeShapes(Range NodeRange); 
	BOOL DoFlattenRange(Range NodeRange); 

	BOOL DoCopyNodesToClipboard(Range NodeRange);

	// --------------------------------------------------------------------------------------

	// Hides all instances of nodes with class NodeClass from the subtree
	BOOL DoRemoveAttrTypeFromSubtree(Node* Subtree, CCRuntimeClass* NodeClass, Node* pExceptThis = NULL);

	// Functions that manipulate NodePath objects
	// Helper function to change selection with undo
	BOOL DoChangeSelection(NodePath* ThisNode, INT32 Index, BOOL NewState);
	BOOL DoDeletePathSection(NodePath* ThisNode, INT32 Index, INT32 NumElements, BOOL RedrawPath = TRUE);
	BOOL DoAlterPathElement(NodePath* ThisNode, 
							INT32 Index, 
							DocCoord NewCoord,
							PathFlags NewFlags,
							PathVerb NewVerb,
							BOOL RedrawPath = TRUE);
	BOOL DoInsertPathElement(	NodePath* ThisPath, 
								INT32 Index, 
								DocCoord NewCoord,
								PathFlags NewFlags,
								PathVerb NewVerb,
								BOOL RedrawPath = TRUE);
	BOOL DoReversePath (NodePath* ThisPath, BOOL RedrawPath = TRUE);
	BOOL DoSmoothNodePath(NodePath* pThisNode, double smoothacc);
	BOOL DoMakeNodeFromPath(NodePath* pParentNode,
							Path* pParentPath,
							AttachNodeDirection Direction,
							BOOL CopyAttributes);



	// --------------------------------------------------------------------------------------
	// Do functions for attribute optimisation

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// This function must get called prior to a new attribute being applied
	BOOL DoLocaliseForAttrChange(NodeRenderableInk* Object, 
								 AttrTypeSet* pAffectedAttrTypes, 
								 ObjectSet* pLocalisedCompounds);
								 
	BOOL DoLocaliseForAttrChange(NodeRenderableInk* Object, 
								 CCRuntimeClass* pAffectedAttrType, 
								 ObjectSet* pLocalisedCompounds);

	// versions of the function which localise all attributes in a range
	BOOL DoLocaliseForAttrChange(Range* pRange, 
								 AttrTypeSet* pAffectedAttrTypes, 
								 BOOL ExcludeTextObjects = FALSE);
								 
	BOOL DoLocaliseForAttrChange(Range* pRange, 
								 CCRuntimeClass* pAffectedAttrType);

 	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~										 
 	// This function must get called after an attribute has been applied
	// Useful when applying multiple attributes to a single object
 	BOOL DoFactorOutAfterAttrChange(NodeRenderableInk* Object,
									AttrTypeSet* pAffectedAttrTypes);

	// Useful when applying a single attribute to a single object
	BOOL DoFactorOutAfterAttrChange(NodeRenderableInk* Object,
									CCRuntimeClass* pAffectedAttrType);
	
	// versions of the function which factor out attributes on all nodes in the range

	// useful when applying multiple attributes to a range of objects
	BOOL DoFactorOutAfterAttrChange(Range* pRange,
									AttrTypeSet* pAffectedAttrTypes);

	// useful when applying a single attribute to a range of objects
	BOOL DoFactorOutAfterAttrChange(Range* pRange,
									CCRuntimeClass* pAffectedAttrType);

	// versions of the function which factor out attributes on all compounds in the 
	// pLocalisedCompounds set

	// Useful when applying attributes to a set of objects which don't fall into a range
	BOOL DoFactorOutAfterAttrChange(ObjectSet* pLocalisedCompounds, 
									AttrTypeSet* pAffectedAttrTypes);

	BOOL DoFactorOutAfterAttrChange(ObjectSet* pLocalisedCompounds,
									CCRuntimeClass* pAffectedAttrType);

	// These function should be called before deleting all objects in a range. It works
	// out the attributes which must be factored out after objects have been deleted.

	// This function perhaps belongs in the range cos this is what it operates on. 
	// However it seems more convenient for it to live here as it is designed 
	// to be used with the DoFactorOutAfterDeletion function.

	// These funs are now defunct 
	//BOOL GetPreDelFactorOutInfo(Range* pRange, List* pPreDelFactorOutInfoList); 

	// This function should get called after a range of objects have been deleted ('Hidden')
	//BOOL DoFactorOutAfterDeletion(List* pPreDelFactorOutInfoList); 
															
	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
								
	BOOL DoFactorOutCommonChildAttributes(NodeRenderableInk* CompoundObject, 			
										  BOOL Global = FALSE,
										  AttrTypeSet* pAffectedAttrTypes = NULL);

	BOOL DoFactorOutCommonAttributes(NodeRenderableInk* pRootNode,
									 BOOL bGlobal = FALSE,
									 AttrTypeSet* pAffectedAttrTypes = NULL);

	BOOL DoLocaliseCommonAttributes(NodeRenderableInk* CompoundObject,
										  BOOL CheckForDuplicates = FALSE, 
										  BOOL Global = FALSE,  
										  AttrTypeSet* pAffectedAttrTypes = NULL);


	// --------------------------------------------------------------------------------------	
	// Functions for selecting/deselecting nodes (these may not be required in future)

	BOOL DoSelectNode(NodeRenderableInk* NodeToSelect, Spread *Parent = NULL); 
	BOOL DoDeselectNode(NodeRenderableInk* NodeToDeselect, Spread *Parent = NULL);

	static BOOL RegisterOpDescriptor(
			 					     UINT32 toolID,       
			 					  	 UINT32 txID, 
			 					  	 CCRuntimeClass* RuntimeClass,        
				 					 TCHAR* tok,	
				 					 pfnGetState gs,	 
				 					 UINT32 helpId = 0, 
									 UINT32 bubbleID = 0,
									 UINT32 resourceID = 0, 
				 					 UINT32 controlID = 0,
				 					 SystemBarType GroupBarID  = SYSTEMBAR_ILLEGAL,	 
				 					 BOOL ReceiveMessages = TRUE, 
				 					 BOOL Smart = FALSE,
				 					 BOOL Clean = FALSE,
				 					 UINT32 OneOpenInstID =0,		
									 UINT32 AutoStateFlags = 0
									);
				 					 	
 
	  
	UndoableOperation();
	virtual ~UndoableOperation();  

	// This	virtual function gets called from the EndOp method after the op has successfully ended
	// and added to the OperationHistory. Override this function to perform operation merging. Note
	// that this function cannot fail (cos it's too scary). 
	virtual void PerformMergeProcessing() ; 
	void MergeWithPrevious();

	// --------------------------------------------------------------------------------------
	// Karim MacDonald 19/01/2000
	// An override for Operation's UpdateChangedNodes(ObjChangedaram*, Spread*) function,
	// as well as a NoStretchUpdateChangedNodes() fn, both required as part of the stretching
	// implementation.
	virtual BOOL UpdateChangedNodes(ObjChangeParam* pParam, Spread* pSpread = NULL);
	virtual BOOL NoStretchUpdateChangedNodes(ObjChangeParam* pParam, Document* pDoc);

	// Karim 20/01/2000
	// A virtual behaviour-ID function, used by the stretching/extension framework, which
	// denotes whether this node might result in the bounds of a named set changing. This
	// most often happens when a node changes its bounds, hence the function name.
	//
	// The rule of thumb is - if your Op might change the bounds of a node, then override
	// this function and return TRUE from it. examples: OpGroup - no; OpPaste - yes.
	// Do a search for 'MayChangeNodeBounds' for more examples.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }


	// --------------------------------------------------------------------------------------
	// Phil 16/02/2005
	//
	// Functions for applying attributes to nodes
	//
public:
	virtual BOOL GetStarted() const { return FALSE; }

private:
	// This var is a flag that ensures that this warning will only appear once for the lifetime of this op
	BOOL WarnInsertObjsOntoLockedLayers;

	// DMc
	// includes the parent nodes to transform in the given range
	Range * IncludeParentTransformNodesInRange(Range * Rng);
};

#endif
