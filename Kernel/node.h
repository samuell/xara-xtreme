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

// Declaration of the following classes:
/*
 	Node                           (Implementation in node.cpp)    
 	
		NodeRenderable             (Implementation in noderend.cpp)
			NodeRenderableInk
			NodeRenderablePaper     
		NodeHidden
		NodeGroup
*/ 


// Check in comments
/*
*/    
      
      
#ifndef INC_NODE
#define INC_NODE

// All include files here
#include <assert.h>
//#include "binds.h"
#include "release.h"

class CCAPI NodeRenderablePaper;
class NodeRenderableInk;
class CCAPI Chapter;  
class CCAPI Layer;
class RenderRegion;
class NodeAttribute;
class Spread;
class List;
class TransformBase;
class UndoableOperation;
class BaseDocument;
class BecomeA;
class ObjChangeParam;
class Filter;
class KernelBitmap;
class NodeHidden;
class InternalClipboardFormat;
class NodePath;
class BaseCamelotFilter;
class ExtendParams;
class NodeCompound;

#if _RELEASE
#define	DEBUG_TREE	0
#else
#define	DEBUG_TREE	1
#endif

#ifdef RALPH
#undef DEBUG_TREE
#define	DEBUG_TREE	0
#endif


// The AttachNode direction type specifies a direction in which one node is to be attached
// to another. 
enum AttachNodeDirection { PREV, NEXT, FIRSTCHILD, LASTCHILD }; 

// A set of flags used to determine which nodes should be rendered.
// See Find{First,Next}ForClippedInkRender functions.
typedef struct
{
	unsigned Required   : 1;
	unsigned Attribute  : 1;
	unsigned Renderable : 1;
	unsigned Intersects : 1;
} ClippedInkFlags;


/***********************************************************************************************

>	typedef enum ChangeCode

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/01/95
	Purpose:	Define type of change results which can be returned by OnChildChange and
				WarnParentOfChange

***********************************************************************************************/

typedef enum ChangeCode
{
	CC_OK,
	CC_NORECORD,
	CC_FAIL
};


/***********************************************************************************************

>	typedef enum SubtreeRenderState

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Date:		22/06/2004
	Purpose:	Enumerates the different actions that RenderTree can take when given the root
				node of a subtree to be rendered
				(Return values for the RenderSubtree function and stored in SubRenderContext)

***********************************************************************************************/
typedef enum SubtreeRenderState
{
	SUBTREE_NORENDER = 0,
	SUBTREE_ROOTONLY,
	SUBTREE_ROOTANDCHILDREN,
	SUBTREE_JUMPTO,
	SUBTREE_RUNTO
};

// OpPermissionState
//
// This is to do with the mechanism used to allow ops to determine whether a node can have an op applied to
// them, which may change depending on the node's parent (e.g. blends don't allow child nodes to be deleted)
//
// This gives readable meanings to the combined states of the OpPermission1 and OpPermission2 flags in Node.  These
// flags get set up by Ops that want to work correctly on nodes that are selected inside other nodes.
//
// An operation should NOT be applied to a node if the permission value is PERMISSION_DENIED.  The other two
// states will allow ops to happen to a node.
//
// The default state of a node is PERMISSION_UNDEFINED, which allows all ops to work as they do currently
//
// This permission state is used by the Range class.  Ranges will not return nodes that have values of PERMISSION_DENIED.
//
// Use Get/SetOpPermission() funcs to access a node's op permission state.
//
// Markn 3/2/95

enum OpPermissionState 	{ 	PERMISSION_UNDEFINED,
							PERMISSION_DENIED,
							PERMISSION_ALLOWED
					  	};


/***********************************************************************************************

>	typedef enum CopyType

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/5/95
	Purpose:	Define the copy type a node supports. the Node class defaults to SIMPLECOPY
				which means its SimpleCopy() function will be used during object copy. This
				gives the default action of making a straight copy of the node. COMPLEXCOPY
				is used by nodes which cannot exist on their own. It may be necessary for
				them to return a tree of nodes when one node is asked to be copied.	This type
				is specifically used for complex group objects
				
***********************************************************************************************/

typedef enum CopyType
{ 
	SIMPLECOPY, 
	COMPLEXCOPY
};

typedef enum CopyStage
{ 
	COPYOBJECT, 
	COPYFINISHED 
};

typedef enum CopyControlFlags
{
	ccALL,
	ccLOCKED,
	ccMANGLED,
	ccMARKED,
	ccSELECTED,
	ccRENDERABLE
};


/********************************************************************************************

class AttrTypeItem: public ListItem

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Purpose:	AttrTypeItem's are held in AttrTypeSets
	SeeAlso:	AttrTypeSet
	SeeAlso:	Node::AppliedAttrSet

********************************************************************************************/

class AttrTypeItem: public ListItem
{
public:
	CCRuntimeClass* AttributeType; 	
};

/********************************************************************************************

>	class AttrTypeSet: public List

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Purpose:	A set of AttrTypeitems
	SeeAlso:	AttrTypeItem

********************************************************************************************/

class AttrTypeSet: public List
{
public:
	
	// A function to return a copy of the AppliedAttrSet
	AttrTypeSet* CopySet();
	
	// A simple function to add attr types to the AppliedAttrSet (Chiecks for duplicates)
	virtual BOOL AddToSet(CCRuntimeClass* AttrType);

	virtual BOOL AddToSet(List& AttributeList);
						   
	// A function to determine if AttrType is in the set
	BOOL InSet(CCRuntimeClass* AttrType);
	
	~AttrTypeSet() { DeleteAll(); };
  		  
};



/***********************************************************************************************

>    Class Node: public CCObject

     Author:       Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Date:         19/4/93
     Base Classes: CCObject       
     
     Purpose:      This abstract class describes the fundamental type of object which can
			  	   be linked into a camelot tree. All other objects which are linked into
			   	   the tree are derived from this class.   
			   
				   IMPORTANT: When producing derived classes of Node, the virtual functions 
				   SimpleCopy and ShowDebugTreeDetails() must be redefined.  

     SeeAlso:      CCObject

***********************************************************************************************/
						
class CCAPI Node: public CCObject
{                    
	CC_DECLARE_DYNAMIC( Node ) 
	
public:                       

	// Node Constructors
	Node();
	Node(Node* ContextNode,  
		 AttachNodeDirection Direction, 
		 BOOL Locked=FALSE, 
		 BOOL Mangled=FALSE,  
		 BOOL Marked=FALSE, 
		 BOOL Selected=FALSE, 
		 BOOL Renderable=FALSE); 
              
    // Destructor
    virtual ~Node();

	virtual void Render( RenderRegion* pRender );
	void RenderTreeAtomic(RenderRegion* pRender);

	virtual BOOL NeedsToRender(RenderRegion* pRender);
	virtual void PreExportRender( RenderRegion* pRender ); 
	virtual BOOL ExportRender( RenderRegion* pRender ); 
	virtual BOOL NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE,
							   BOOL CheckSelected = FALSE);
	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE);

	// Functions for handling child nodes changing inside compound node,
	virtual ChangeCode 	OnChildChange(ObjChangeParam* pParam);

	// Access functions to the node's op permission state
	virtual OpPermissionState	GetOpPermission();
	virtual void				SetOpPermission(OpPermissionState NewState,BOOL AndParents = FALSE);

	// Permission enquiry functions.
	virtual BOOL AllowOp(ObjChangeParam* pParam,BOOL SetOpPermissionState = TRUE,
												BOOL DoPreTriggerEdit = TRUE);
	virtual BOOL AllowOp_AccountForCompound(ObjChangeParam* pParam,
											BOOL SetOpPermissionState=TRUE,
											BOOL DoPreTriggerEdit = TRUE);

    // Node deletion methods
	void CascadeDelete(void);

	// Rendering PreChild function - See Function Declaration in node.cpp for more info
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual void RenderAfterSubtree(RenderRegion* pRender) {;}

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WritePostChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePostChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL CanWriteChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL CanWriteChildrenNative(BaseCamelotFilter* pFilter);

	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);

	virtual BOOL ReadPostChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL ReadPostChildrenNative(BaseCamelotFilter* pFilter);

	// Progressive rendering & the new file format
	virtual	BOOL AreYouSafeToRender();

	// Node copy and move methods
	BOOL CopyNode(Node* DestinationNode, 
				  AttachNodeDirection Direction);
	virtual BOOL NodeCopy(Node** ppNodeCopy);
	virtual BOOL CloneNode(Node** ppNoeCopy, BOOL bLightweight);
	BOOL CopyComplexRange(Range& RangeToCopy);
	
	BOOL CopyChildrenTo(Node* DestinationNode, CopyControlFlags CopyFlags=ccALL); 

	void MoveNode(Node* DestinationNode, 
				  AttachNodeDirection Direction);
				   
	void AttachNode(Node* ContextNode, 
					AttachNodeDirection Direction,
					BOOL fCheckTransparent = TRUE,
					BOOL InvalidateChildren = TRUE);

	// function to insert a chain of siblings, but does little more than alter pointers
    void InsertChainSimple(Node* ContextNode, AttachNodeDirection Direction);
	
	virtual BOOL PostImport();			// function called after import
	virtual BOOL PostDuplicate(UndoableOperation* pOp);		// function called after node has been copied.

	UINT32 GetTag() const;     
    
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
	virtual CopyType GetCopyType();
   	UINT32 GetSubtreeSize(); 					// Returns size of subtree in bytes 
	BOOL IsUnder(Node* pTestNode, BOOL bFirstCall = TRUE) const;		// Find whether this is under the test node in render order

    
    // Query flag's status methods
	BOOL IsLocked() const;  
	BOOL IsMangled() const; 
	BOOL IsMarked() const;  
	BOOL IsRenderable() const;
	BOOL IsParentOfSelected() const;
	BOOL IsSelected() const;
	BOOL IsChildOfSelected();

	virtual BOOL ShouldBeRendered() const;
	virtual BOOL NeedsTransparency() const;
	virtual INT32 EstimateNodeComplexity (OpParam* details) { return (1); }

	// Fast type detection functions
	// whether this node is a complex compound eg bevel, shadow, contour etc
	// Implementing them here tells the compiler that they should be inlined if possible
	virtual BOOL IsAnObject() const {return FALSE;}
	virtual BOOL IsAnAttribute() const {return FALSE;}
	virtual BOOL IsKindOfTextAttribute() const { return FALSE; }
	virtual BOOL IsPaper() const {return FALSE;}
	virtual BOOL IsLayer() const {return FALSE;}
	virtual BOOL IsSpread() const {return FALSE;}
	virtual BOOL IsChapter() const {return FALSE;}
	virtual BOOL IsNodeDocument() const {return FALSE;}
	virtual BOOL IsNodeRenderableClass() const {return FALSE;}
	virtual BOOL IsNodeHidden() const {return FALSE;}
	virtual BOOL IsNodePath() const {return FALSE;}
	virtual BOOL IsARegularShape() const {return FALSE;}
	virtual BOOL IsOrHidesAnAttribute() const {return FALSE;}
	virtual BOOL IsABitmap() const {return FALSE;}
	virtual BOOL IsAFillAttr() const {return FALSE;}
	virtual BOOL IsABitmapFill() const {return FALSE;}
	virtual BOOL IsABitmapColourFill() const {return FALSE;}
	virtual BOOL IsABitmapTranspFill() const {return FALSE;}
	virtual BOOL IsAFeatherAttr() const { return FALSE; }
	virtual BOOL IsAClipViewAttr() const { return FALSE; }
	virtual BOOL IsBounded() const {return FALSE;}
	virtual BOOL IsDragged() const {return FALSE; }
	virtual BOOL IsController() { return FALSE;}
	virtual BOOL IsABlend () { return FALSE; }
	virtual BOOL IsCompound() const {return FALSE;} 
	virtual BOOL IsCompoundClass() const {return FALSE;} 
	virtual BOOL IsAVisibleTextNode() const {return FALSE;}
	virtual BOOL IsAnAbstractTextChar() const {return FALSE;}
	virtual BOOL IsATextChar() const {return FALSE;}
	virtual BOOL IsABaseTextClass() const {return FALSE;}
	virtual BOOL IsATypeface() const {return FALSE;}
	virtual BOOL IsAnObjectName() const {return FALSE;}
	virtual BOOL IsUserAttribute() const {return FALSE;}
	virtual BOOL IsSetCandidate() const {return FALSE;}
	virtual BOOL IsCacheCompound() const { return FALSE; } 
	virtual BOOL IsABevel() const { return FALSE; } 
	virtual BOOL IsABevelController() const { return FALSE; } 
	virtual BOOL IsAContour() const { return FALSE; } 
	virtual BOOL IsAContourController() const { return FALSE; } 
	virtual BOOL IsAShadow() const { return FALSE; } 
	virtual BOOL IsAShadowController() const { return FALSE; } 
	virtual BOOL IsABrush() const {return FALSE;}
	virtual BOOL IsEffect() const {return FALSE;}
	virtual BOOL IsBitmapEffect() const {return FALSE;}
	virtual BOOL IsRenderedAsInk() const {return FALSE;}

	BOOL IsBrushed() const;// does this node have a brush attribute?

	virtual NodePath* IsAnEditablePath();

	// virtual ID fn's for ClipView nodes.
	virtual BOOL IsANodeClipViewController() const { return FALSE; }
	virtual BOOL IsANodeClipView() const { return FALSE; }

	// virtual ID fn for a group node.
	virtual BOOL IsAGroup() const { return FALSE; }

	// virtual ID fn for an insertion node.
	virtual BOOL IsAnInsertionNode() const { return FALSE; }

	// virtual ID fn for a NodeMould.
	virtual BOOL IsANodeMould() const {return FALSE;}

	// Function for interrogation by clipboard to determine exportable data types
	virtual BOOL SupportsClipboardFormat(InternalClipboardFormat *Format) const;

	// This virtual was added because we need to know about nodes which can discard their child
	// attributes, an example is the CaretNode. If an attribute gets applied to such a node 
	// then no undo info can be generated, the base class function returns FALSE
	virtual BOOL DiscardsAttributeChildren() const; 

	// Function to allow nodes to provide flexible edit surfaces.
	virtual Node* HasEditableChild(CCRuntimeClass* ChildClass, Node* pPrevNode);

	// This function returns NULL in the base class.
	virtual KernelBitmap* EnumerateBitmaps(UINT32 Count);

	// This function returns the special value 1e9 to indicate no effective minimum
	virtual double GetEffectiveBitmapMinDPI(KernelBitmap* pBitmap) { return(1e9); }

	// Replace a specific bitmap with another one
	virtual BOOL ReplaceBitmap(KernelBitmap* pOrigBitmap, KernelBitmap* pNewBitmap) { return(FALSE); }

	// Find out the optoken of the default operation for this node. (See EditSelectionOp and Return hotkey.)
	virtual TCHAR* GetDefaultOpToken() {return NULL;}
		
	// Set Flag's status methods
	void SetLocked(BOOL Status);       
	void SetMangled(BOOL Status);
	void SetMarker(BOOL Status); 
	virtual void SetSelected(BOOL Status); 
	void SetRender(BOOL Status, BOOL bAndChildren=FALSE);
	void SetParentOfSelected(BOOL Status);

	// Support functions for SetSelected
	BOOL HasSelectedChildren() const;

	// Fast way of clearing all selected flags (Selected and SelectedChildren) of the node, and only this node.
	// Note: this is NOT a substitute for SetSelected(FALSE). Only use if you know what you're doing
	void ClearSelection(BOOL AndChildren=FALSE);

	// Used when adding a sub-tree to a document, any part of which may contain
	// transparency.
	BOOL ChildrenNeedTransparency();

    // Find Node pointer methods   
    // These routines should be made inline as they are called oodles of times during rendering.
	Node* FindNext(void) const;
	Node* FindPrevious(void) const;
	Node* FindParent(void) const;        
	Node* FindFirstChild(void) const;
	Node* FindLastChild(BOOL ExcludeInsertionNode = FALSE) const;  

	Node* FindParent(CCRuntimeClass* ParentClass) const;
	Node* FindNext(CCRuntimeClass* Class) const; 
	Node* FindPrevious(CCRuntimeClass* Class) const; 
	Node* FindFirstChild(CCRuntimeClass* Class) const;
	Node* FindLastChild(CCRuntimeClass* Class) const;

	NodeRenderableInk* FindNextInk() const;
	NodeRenderableInk* FindPreviousInk() const;
	NodeRenderableInk* FindFirstChildInk() const;
	NodeRenderableInk* FindLastChildInk() const;

	BaseDocument *FindOwnerDoc() const;

	// check all our parents for a node of the given type, which is,
	// or has as one of _its_ parents, the given node.
	BOOL IsFamily(CCRuntimeClass* ccrcType, Node* pAncestor) const;

	// These functions have a very special purpose. They are very Dangerous to use. So don't use it them unless
	// you are very sure what you are doing.
	void SetParentDangerous(Node* NewParent); 
	void SetNextDangerous(Node* NewNext);
	void SetPreviousDangerous(Node* NewPrev);
	void SetChildDangerous(Node* NewChild);

	// The following routines are provided to access the nodes in the tree without being 
	// passed back pointers to hidden nodes. They are slower that the Node pointer methods
	// and should therefore only be used where neccesary.

	Node* FindNextNonHidden(void) const; 
	Node* FindPrevNonHidden(void) const; 

	// Tree traversal methods.

	Spread* FindParentSpread();

	// These are the old FindFirstDepthFirst and FindNextDepthFirst routines 
    static Node* DocFindFirstDepthFirst(BaseDocument *pDocToSearch);
    Node* DocFindNextDepthFirst(void);

	Node* FindFirstDepthFirst();
	Node* FindNextDepthFirst(Node* Subtree);
	Node* FindFirstPreorder();
	Node* FindNextPreorder(Node* pRoot = NULL, BOOL bSkipSubtree = FALSE);

	static Chapter* FindFirstChapter(BaseDocument* pDocToSearch);
	Chapter* FindNextChapter(void); 

    NodeRenderablePaper* FindNextForClippedPaperRender(void);

	// ---------------------------------------------------------------------
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// These functions are deprecated and nearly ready to be removed!
	// DO NOT USE THEM!
	// Use RenderTree instead to enable the more intelligent tree scanning
	// needed by Cacheing, Effects and Effect attributes
    Node* FindFirstForClippedInkRender(DocRect* pClipRect,
    								   RenderRegion* pRender,
    								   BOOL ExcludeLockedLayers = FALSE,
									   Node* pStopNode = NULL);

	Node* FindNextForClippedInkRender(DocRect* pClipRect, 
									  RenderRegion *pRender, 
									  BOOL ExcludeLockedLayers = FALSE,
									  Node* pStopNode = NULL);

	Node* FindFirstForUnclippedInkRender(RenderRegion* pRender);
	Node* FindNextForUnclippedInkRender(RenderRegion *pRender);
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// ---------------------------------------------------------------------

	Chapter* FindEnclosingChapter(DocCoord* ChapterPos, XLONG* ChapterDepth); 

	// Ilan - Support the new extensions to the attribute system (ie geometry linked attributes)
	NodeAttribute* FindFirstGeometryLinkedAttr();
	NodeAttribute* FindNextGeometryLinkedAttr();

	virtual void UnlinkNodeFromTree(BaseDocument *pOwnerDoc = NULL);

	// Select-inside functions
	Node* FindParentOfSelected();
	virtual NodeCompound* GetParentController() const {return NULL;}	// Implement if your node was created by a PostProcessor of some sort

	// These two functions are used to convert to shapes, and are implemented in mkshapes.cpp
	virtual BOOL CanBecomeA(BecomeA* pBecomeA); 
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);

#ifdef _DEBUG
	void DT();										// dump doc tree  containing this node
	void DST();										// dump a subtree from this node
	void DST1(INT32 FormParent=0);	                // dump a subtree containing this node
	void DST2(INT32 FormParent=0, INT32 MaxDepth=9999);	// dump a subtree containing this node
#endif

	// This function is used to inform parents of a child change
	ChangeCode WarnParentOfChange(ObjChangeParam* pParam, BOOL AllParents=TRUE);

	virtual BOOL AllowSelectInside() const { return TRUE; }

	// The ShowDebugTree method is only required in the debug version                  
#if !_RELEASE
	#ifdef _DEBUG
	
	void ShowDebugTree() const;  
	virtual void ShowDebugTreeDetails() const; 
	
	#endif    
	
	virtual void GetDebugDetails(StringBase* Str);   // This is used by the Debug Tree dialog
													 // It will probably be deleted when we ship !.
#endif

	// Methods to increment and decrement the Hidden node refference count 												 
	void IncHiddenCnt(void); 
	void DecHiddenCnt(void); 

	UINT32 GetHiddenCnt(void);

	#ifdef _DEBUG
		const TCHAR* Name() const;
	#endif 

	BOOL OptimiseAttributes();

	// Functions called when the node is hidden or re-shown
	virtual BOOL HidingNode();
	virtual BOOL ShowingNode();
	virtual INT32 ComplexHide(UndoableOperation* pOp, Node* pNextInRange);

	// Functions to managed node marks
	void MarkNodeAndChildren();
	void MarkChildren();
	void ClearMarks();
	void ClearChildMarks();

public:
	virtual BOOL IsDifferent(Node *pOther);
		// Determine if 2 nodes are considered to be equal.
		// NOTE: This may not be implemented fully for all derived classes, so for certain
		// types of node, the comparison will only be to see if they are of the same class!
		// (This was originally added for the StrokeComponent to be able to compare 2
		// subtrees to see if they are equal - a simple base-class implementation plus
		// overrides in NodeRenderable & NopdePath suffices to distinguish most Stroke clipart)

public:
	// Functions for examining and manipulating child attributes
	virtual void CountChildNodes(UINT32* pChildCount, UINT32* pChildAttrCount);
	virtual BOOL AreChildAttrsIdentical(Node* pOtherNode);
	virtual BOOL HasThisChildAttr(NodeAttribute* pAttr);
	virtual BOOL CopyChildAttrs(Node* pDestNode);
	
	// DMc - returns TRUE if the given node is in the subtree of this node
	virtual BOOL IsNodeInSubtree(Node * pNode);

	// DMc 11/5/99
	// Selection & regeneration functions
	
	// causes the node to regenerate itself (in an undoable way given an undoable operation)
	// bCacheRender is FALSE if the node is to regenerate itself now, or TRUE if it is to
	// regenerate itself on the next render click
	// Returns TRUE to indicate that the node was changed, FALSE if nothing changed
	virtual BOOL RegenerateNode(UndoableOperation * pOp = NULL,
								BOOL bCacheRender = FALSE,
								BOOL bInformParents = TRUE) { return FALSE; }
	
	// when the user clicks on a child node, this tells the hit testing code
	// whether to promote the hit test to this node
	virtual BOOL PromoteHitTestOnChildrenToMe() const { return TRUE; }

	// this indicates whether node should transform with its children
	virtual BOOL ShouldITransformWithChildren() const { return FALSE; }

	// this indicates that the node needs a parent node of a particular type
	virtual BOOL NeedsParent(Node * pClassNode) const { return FALSE; }

	// indicates whether this node should be ignored by marquee select
	virtual BOOL MarqueeSelectNode() const { return TRUE; }
	
	// calls the protected function SimpleCopy
	virtual Node* PublicCopy(); 

	void RemoveAttrTypeFromSubtree(CCRuntimeClass* AttrType, Node* pExceptThis = NULL);

protected:    

	// The NodeFlags type is used to hold the status of a Node                
	struct CCAPI NodeFlags
	{
		BOOL Locked: 1;       // Not used currently
		BOOL Mangled: 1;	  // Used by the ArtWorks importer
		BOOL Marked: 1;		  // Used by CopyMarkedObjects

		// these should only be set in a NodeRenderable
		BOOL Selected: 1;			// This node has been selected by the user
		BOOL Renderable:1;			// This node is renderable
		BOOL SelectedChildren: 1;	// This node has children that have been selected by the user

		BOOL OpPermission1: 1;		// These two flags are used together to encode the node's tri-state
		BOOL OpPermission2: 1;		// op permission value.  Values are defined by enum OpPermissionState.
	};
   
	UINT32 Tag;         // Unique node identifier
	NodeFlags Flags;   // Flags indicating the objects status    
	
	 // Links to other nodes
	Node *Previous;    // Node's previous sibling
	Node *Next;        // Node's next sibling
	Node *Child;       // Node's first child
	Node *Parent;      // Node's parent   



	UINT32 HiddenRefCnt; // Reference count of the number of NodeHidden nodes which hide the node

	virtual Node* SimpleCopy(); 
	virtual INT32 ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput);
	void CopyNodeContents(Node* nodeCopy);      

	// Size of pixel in document coords
	static MILLIPOINT PixelWidth; 
	static MILLIPOINT PixelHeight;

	Node *FindChildToRender(DocRect *const pClipRect, RenderRegion *const pRender, 
							BOOL ExcludeLockedLayers,
							Node* pStopNode = NULL);

	Node *FindChildToRenderUnclipped(RenderRegion *const pRender);

private:
	// Support functions to attach a node to a context node
	void AttachAsPreviousNode(Node* ContextNode); 
	void AttachAsNextNode(Node* ContextNode);                                 
	void AttachAsFirstChildNode(Node* ContextNode);       
	void AttachAsLastChildNode(Node* ContextNode); 

	void SetTags(BaseDocument *pOwnerDoc);

public:
	void DeleteChildren(Node* FirstChild);

private:
	static BOOL HourglassOn;

public:
	static void StartHourglass() { HourglassOn = TRUE; };
	static void StopHourglass() { HourglassOn = FALSE; };
	static BOOL ShowHourglass() { return HourglassOn;} 

protected:
	BOOL CopyChildren(Node* FirstChild, 
					  Node* NewParent,
					  CopyControlFlags = ccALL);

private:	
	// Support function for ShowDebugTree method
#ifdef _DEBUG
	void ShowDebugTreeHelper(const Node* const Root, 
						     INT32 TreeLevel, 
							 BYTE* IndentString) const;
#endif

	// Select-inside functions
protected:
	BOOL DeSelectSubtree();

public:
	friend class NodeHidden;
	virtual BOOL SetParentLayerAsEdited();

public:
	// Returns the first attribute of the given class that applies to this node, or null
	// if there isn't one.  Follow with calls to NodeAttribute::FindNextAttrClass on the
	// returned attribute.
	typedef BOOL (Node::*TypeFunc)() const;
	NodeAttribute* FindFirstAttr(TypeFunc pfnTest) const;

	// Karim MacDonald 25/11/1999
	// Extend functions. Maybe these functions should really originate in NodeRenderable?
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
public:
	virtual BOOL IsTypeExtendible() const { return FALSE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);
};



/////////////////////////////////////////////////////////////////////////////////////////////
//
//  Inline versions of Node traversal functions.
//
//  Note that we only bother with small non-virtual functions which are likely to be called
//	in a loop.
//
/////////////////////////////////////////////////////////////////////////////////////////////


/***********************************************************************************************

>	UINT32 Node::GetTag() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/93
	Inputs:		-
	Outputs:	-
	Returns:	Tag field of the node 
	Purpose:    For finding a nodes TAG, which is a unique identifier for each node stored
				within a document. 
	Errors:		-

***********************************************************************************************/
       
inline UINT32 Node::GetTag() const
{ 
	return Tag; 
}       
 
 
/*********************************************************************************************

>    BOOL Node::IsLocked(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is locked, and FALSE if the node is not locked.  
     Purpose:   For finding the status of the nodes locked flag
     Errors:    

**********************************************************************************************/
       
inline BOOL Node::IsLocked(void) const 
{ 
	return Flags.Locked; 
} 


/*********************************************************************************************

>    BOOL Node::IsMangled(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is mangled, and FALSE if the node is not mangled.  
     Purpose:   For finding the status of the nodes mangled flag
     Errors:    

**********************************************************************************************/
       
inline BOOL Node::IsMangled(void) const                
{ 
	return Flags.Mangled; 
} 


/*********************************************************************************************

>    BOOL Node::IsMarked(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is marked, and FALSE if the node is not marked.  
     Purpose:   For finding the status of the nodes marked flag
     Errors:    

**********************************************************************************************/
       
inline BOOL Node::IsMarked(void) const         
{ 
	return Flags.Marked; 
} 


/*********************************************************************************************

>    BOOL Node::IsRenderable(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is renderable, and FALSE if the node is not renderable.  
     Purpose:   For finding the status of the nodes renderable flag
     Errors:    

**********************************************************************************************/
       
inline BOOL Node::IsRenderable() const
{ 
	return Flags.Renderable; 
}


/*********************************************************************************************

>    virtual BOOL Node::IsSelected() const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node is selected, and FALSE if the node is not selected.  
     Purpose:   For finding the status of the nodes selected flag
     Errors:    

**********************************************************************************************/
       
inline BOOL Node::IsSelected() const           
{ 
	return Flags.Selected; 
}  


/*********************************************************************************************

>    BOOL Node::IsParentOfSelected(void) const

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   07/10/94
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if the node *may* have selected children.
     			FALSE otherwise
     Purpose:   For finding the status of the nodes SelectedChildren flag
	 			Note: When this function returns TRUE it is not guaranteed that there will be
				selected children - there may be none!
     Errors:    

**********************************************************************************************/
       
inline BOOL Node::IsParentOfSelected() const           
{ 
	return Flags.SelectedChildren; 
}  


/*********************************************************************************************

>    void Node::SetLocked(BOOL Status) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    Status: Status of node's locked flag
     Outputs:   -
     Returns:   -
     Purpose:   To set the node's locked status (TRUE/FALSE)
     Errors:    -

**********************************************************************************************/
       
inline void Node::SetLocked( BOOL Status )        
{ 
	Flags.Locked = Status; 
}    
 
			     
/*********************************************************************************************

>    void Node::SetMangled(BOOL Status) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    Status: Status of node's mangled flag
     Outputs:   -
     Returns:   -
     Purpose:   To set Nodes mangled status (TRUE/FALSE)
     Errors:    -

**********************************************************************************************/
      
 
inline void Node::SetMangled( BOOL Status )  
{ 
	Flags.Mangled = Status; 
}  
     
     
/*********************************************************************************************

>    void Node::SetMarker(BOOL Status) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Inputs:    Status: Status of node's marked flag
     Outputs:   -
     Returns:   -
     Purpose:   To set Nodes marked status (TRUE/FALSE)
     Errors:    -

**********************************************************************************************/
       

inline void Node::SetMarker( BOOL Status )  
{
	Flags.Marked = Status; 
}  
  
  			     
/********************************************************************************************

>    Node* Node::FindNext(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Returns:   Pointer to the node's next sibling node 
     Purpose:   For finding the next sibling of a node

********************************************************************************************/
				   
					     
inline Node* Node::FindNext(void) const     
{
	return(Next);
}  
			      
	  
/********************************************************************************************

>   Node* Node::FindPrevious(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Returns:   Pointer to the node's previous sibling node 
     Purpose:   For finding the previous sibling of a node

********************************************************************************************/
	
			   
inline Node* Node::FindPrevious(void) const
{                 
	return(Previous);
}


/********************************************************************************************

>    Node* Node::FindParent(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Returns:   Pointer to the node's parent node 
     Purpose:   For finding the parent of a node        

********************************************************************************************/
	
inline Node* Node::FindParent(void) const
{         
	return(Parent);
}


/********************************************************************************************

>    Node* Node::FindFirstChild(void) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   19/4/93
     Returns:   Pointer to the node's first child  
     Purpose:   For finding the first child of a node

********************************************************************************************/
       
inline Node* Node::FindFirstChild(void) const
{                  
	return(Child); 
}                      


/*********************************************************************************************

>    void Node::SetParentOfSelected(BOOL Status) const

     Author:    Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   07/10/94                                                                                                                       
     Inputs:    Status: Required setting of nodes SelectedChildren flag
     Outputs:   -
     Returns:   -
     Purpose:   To set Nodes SelectedChildren status (TRUE/FALSE)
	 			This is used to implement select-inside
     Errors:    -

**********************************************************************************************/
  
inline void Node::SetParentOfSelected( BOOL Status )       
{
	Flags.SelectedChildren = Status;  
//	Camelot.UpdateSelection();			// This line not required because the function
											// does not itself modify the selection!
}     

			     
/*********************************************************************************************

>    void Node::ClearSelection(BOOL AndChildren=FALSE)

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   7/2/95
     Inputs:    AndChildren = a bool to check for selected children and clear their bits too
     Outputs:   -
     Returns:   -
     Purpose:   Clears all selection flags in this node. It now deselects all children if
	 			the AndChildren is TRUE (Defaults to FALSE)
     Errors:    -

**********************************************************************************************/
       
inline void Node::ClearSelection(BOOL AndChildren)
{
	Flags.Selected 			= FALSE;
	Flags.SelectedChildren 	= FALSE;
	if (AndChildren)
		DeSelectSubtree();
}  
  
			     
/////////////////////////////////////////////////////////////////////////////////////////////
//
//  End of Inline versions of Node functions.
//
/////////////////////////////////////////////////////////////////////////////////////////////


/***********************************************************************************************

>	class NodeRenderable : public Node

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Date:		19/5/93
	Purpose:	This abstract class describes a node which is renderable.
				See the Node class for an important note about producing derived
				classes of Node.
	SeeAlso:	Node

********************************************************************************************/

class NodeRenderable : public Node
{       
 	CC_DECLARE_DYNAMIC(NodeRenderable)
 
public:
	// Constructors
	NodeRenderable();
	
	// Node renderable initialisation 
	NodeRenderable(Node* ContextNode,  
					AttachNodeDirection Direction,    
					BOOL Locked=FALSE, 
					BOOL Mangled=FALSE,  
					BOOL Marked=FALSE, 
					BOOL Selected=FALSE); 
	
	// Transformation functions
	virtual BOOL CanTransform();
	virtual void Transform(TransformBase&);
	virtual void TransformChildren(TransformBase&);
    virtual void PolyCopyNodeContents(NodeRenderable* NodeCopy);

	// Query functions
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual BOOL NeedsToExport(RenderRegion* pRender, BOOL VisibleLayersOnly = FALSE, BOOL CheckSelected = FALSE);
	virtual INT32 GetSizeOfExport(Filter*);
	virtual BOOL IsNodeRenderableClass() const;
	virtual BOOL ShouldBeRendered() const;

	// This is used by the Debug Tree dialog. It will probably be deleted when we ship!
	virtual void GetDebugDetails(StringBase* Str);

	// Blob related functions
	virtual void RenderFillBlobs(RenderRegion* pRender) {}
	virtual void RenderAppliedFillBlobs(RenderRegion* pRender) {}
	virtual void RenderArtisticBlobs(RenderRegion* pRender);
	virtual void RenderObjectBlobs(RenderRegion* pRender);
	virtual void RenderTinyBlobs(RenderRegion* pRender);
	virtual void RenderPenBlobs(RenderRegion* pRender);
	virtual void RenderToolObjectBlobs(RenderRegion* pRender);
	virtual void RenderEffectBlobs(RenderRegion* pRender);

	// Selection state related functions
	virtual void Select(BOOL ReDraw);
	virtual void DeSelect(BOOL ReDraw, BOOL bDeselectChildren = FALSE);

	virtual BOOL CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc); 

	// All renderable node can have blob bounding rects (though they may be empty)
	virtual DocRect GetBlobBoundingRect();
	virtual DocRect GetUnionBlobBoundingRect(BOOL bIncludeEffects = TRUE);

	virtual BOOL IsDragged() const;
	virtual void SetDraggedState(BOOL bNewState, BOOL bAndChildren);

// Karim 1/12/1999
	// Methods to perform a stretch or translate of this node, for use with Extension.
	// The base implementations of these methods do nothing - subclasses must add function.
	virtual void TransformStretchObject(const ExtendParams& ExtParams);
	virtual void TransformTranslateObject(const ExtendParams& ExtParams);
	virtual void TransformTranslateNoStretchObject(const ExtendParams& ExtParams);

	// Find the extend reference-centre of this Node, for use with Extension.
	// this base-class implementation returns origin coordinates - override to add function.
	virtual DocCoord FindExtendCentre() { return DocCoord(0, 0); }

	virtual Node* SimpleCopy(); 
    void CopyNodeContents(NodeRenderable* NodeCopy); 

	// Karim 13/09/2000
	// Test whether this node is see-through, ie if you can still see some of the background
	// after rendering the tree rooted at this node.
	// This test is useful if you're going to render into a bitmap, to be post-processed and
	// plonked on the document (eg Feathering). This way, you can hopefully avoid any
	// performance hit caused by unnecessarily rendering into an alpha-channel bitmap.
	//
	// This base class version simply returns FALSE.
	// It exists as the common base for fill attribute and certain ink nodes to override.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) { return FALSE; }

protected:
	// Flag indicating the currently dragged state of this node
	BOOL	Dragged;

};                 




/********************************************************************************************

>	class NodeRenderableBounded : public NodeRenderable

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Purpose:	As NodeRenderable, but with a bounding box describing the area this object
				occupies.  This class also includes functions for use with attribute 
				optimisation.
	SeeAlso:	NodeRendereable

********************************************************************************************/

class NodeRenderableBounded : public NodeRenderable
{
	CC_DECLARE_DYNAMIC( NodeRenderableBounded )  
 
public:
	// Constructors
	NodeRenderableBounded();
	
	// Node renderable bounded initialisation 
	NodeRenderableBounded(Node* ContextNode,  
						  AttachNodeDirection Direction,    
						  BOOL Locked=FALSE, 
						  BOOL Mangled=FALSE,  
						  BOOL Marked=FALSE, 
						  BOOL Selected=FALSE); 

	~NodeRenderableBounded();

	// Virtual functions
	virtual void GetDebugDetails(StringBase* Str);	
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	virtual void Transform(TransformBase&);
	virtual void TransformChildren(TransformBase&);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Function to handle attribute application (gives the node a chance to veto it).
	virtual BOOL CanTakeAttributeType(CCRuntimeClass*);

	// Functions that deal with bounding boxes of nodes
 	void 			ValidateBoundingRect();
	virtual void	InvalidateBoundingRect(BOOL InvalidateChildBounds = FALSE);
private:
	void 			InvalidateBoundingRectHelper(); // The recursive bit of InvalidateBoundingRect
public:
	virtual DocRect	GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual DocRect GetUnionBlobBoundingRect(BOOL bIncludeEffects = TRUE);
	
	// gets the bounding rect to use in Eor dragging this node
	virtual DocRect GetEorDragBoundingRect() { return GetBoundingRect(TRUE, FALSE); }

	virtual DocRect GetImagemapClickableRectangle()
	{
		return GetBoundingRect(FALSE, TRUE);
	}
	virtual DocRect				GetEffectStackBounds(BOOL* pbFoundEffects = NULL);

	virtual void IncludeChildrensBoundingRects(DocRect *);
	BOOL IsIntersectedWith( DocRect* pClipRect );

	// A function to save on expensive IsKindOf()
	virtual BOOL IsBounded() const {return TRUE;}

	// The natty Snap functions
	virtual BOOL Snap(DocCoord* pDocCoord);
	virtual BOOL Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);
	virtual BOOL SnapToCoords(DocCoord* pDocCoord);

	// Returns TRUE if this object has been made magnetic.  Points and rects will snap to
	// it if the "snap to magnetic objects" option is on
	virtual BOOL IsMagnetic();

	// Lets you set the magnetic state of the node
	virtual void SetMagneticState(BOOL state);

	// Function to declare the preferences for the magnetic stuff
	static BOOL Init();

	// All bounded renderable nodes have an angle of rotation
	virtual double GetRotationAngle();

	// Cacheing helper functions
	virtual BOOL RenderCached(RenderRegion* pRender);
	virtual BOOL CaptureCached(RenderRegion* pRender);
	virtual BOOL ReleaseCached( BOOL bAndParents = TRUE,
								BOOL bAndChildren = TRUE,
								BOOL bSelf = TRUE,
								BOOL bAndDerived = TRUE);
	virtual void CopyCached(NodeRenderableBounded* pCopyOfNode, double dRes, INT32 maxOption = 0);
	virtual void CopyCached(NodeRenderableBounded* pCopyOfNode);
	virtual void TransformCached(TransformBase& Trans, double dTestPixelWidth);
	virtual BOOL HasCachedDirectBitmap() {return FALSE;}

	// Check subtree for non-mix transparency
	BOOL ContainsNonMixTransparency(RenderRegion* pRegion);

	virtual void RenderEffectAttributes(RenderRegion* pRegion);

public:
	virtual BOOL IsDifferent(Node *pOther);
		// Determine if 2 nodes are considered to be equal.
		// NOTE: This may not be implemented fully for all derived classes, so for certain
		// types of node, the comparison will only be to see if they are of the same class!
		// (This was originally added for the StrokeComponent to be able to compare 2
		// subtrees to see if they are equal - a simple base-class implementation plus
		// overrides in NodeRenderable & NopdePath suffices to distinguish most Stroke clipart)

protected:
	// Functions to help with the magnetic stuff
	BOOL IsMagneticallyClose(DocCoord*, DocCoord*);

protected:
	// Node copying functions.
	virtual Node* SimpleCopy(); 
    void CopyNodeContents(NodeRenderableBounded* NodeCopy); 

// Karim 1/12/1999
public:
	// Find the extend reference-centre of this Node, for use with Extension.
	virtual DocCoord FindExtendCentre() { return GetBoundingRect().Centre(); }

	// Find the extend bounding rect of this node, for use with Extension.
	virtual DocRect GetExtendTargetBounds(const ExtendParams& ExtParams);

protected:
	// a flag to indicate if it is valid or not.
	BOOL	IsBoundingRectValid;

	// The renderable node bounding rectangle
    DocRect BoundingRectangle;

	// Flag indicating the magnetic state of this node
	BOOL	Magnetic;

	BOOL	MayBeCached;			// False when this node is definitely NOT cached
									// Prevents cache lookup for simple objects

public:
	// Preferences for the effective range of the magnetic mode
	static INT32 MagneticPointRadius;			// Magnetic range of a Coord
	static INT32 MagneticLineRadius; 			// Magnetic range of a line

	static BOOL bEnableCacheing;			// Enable bitmap cacheing
	static BOOL bThrottleCacheing;			// Prevent bitmap cacheing using more than 5% of rendering time
	static BOOL bShowCacheBlobs;			// Show special markers when cached groups are selected

};
 






/********************************************************************************************

>	class NodeHidden: public Node

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/93
	Purpose:	Nodes are generally never deleted from the tree, instead they are hidden. 
				To hide a node a NodeHidden is inserted into the tree at the old position of
				the hidden node, the HiddenNode holds a pointer to the hidden node. The static
				ShowNode function of NodeHidden will delete the NodeHidden and relink the 
				hidden node into the tree. 
	SeeAlso:	-

********************************************************************************************/

class NodeHidden: public Node
{       
public: 
	CC_DECLARE_DYNAMIC( NodeHidden ) 
	NodeHidden(Node* HiddenNode); 
	Node* ShowNode(); 
	Node* HiddenNd; //Pointer to the hidden node	

	// The simple copy method of a NodeHidden should never be called. Therefore the method causes 
	// an ENSURE failure if executed. 
	virtual Node* SimpleCopy();  
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual BOOL IsNodeHidden() const;
	virtual BOOL IsOrHidesAnAttribute() const;

// Karim MacDonald 25/11/1999
	// Extend functions.
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
	// Because a hidden node does nothing at all on an extend, it needn't
	// validate its children.
public:
	virtual BOOL IsTypeExtendible() const { return FALSE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams)
			{
				return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
			}
	virtual void Extend(const ExtendParams& ExtParams) {}
}; 


#endif
