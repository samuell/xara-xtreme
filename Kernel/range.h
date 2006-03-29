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
     
/* Declaration of class Range */      
     
/*
*/


#ifndef INC_RANGE
#define INC_RANGE

#include "docrect.h"
#include "pump.h"

class NodeAttribute;
class BaseDocument;
class ObjChangeParam;
class ObjectSet;
class CommonAttributeItem;
class CCAttrMap;
class TransformBase;
class KernelBitmap;
class KernelBitmapRef;
class NodeBitmap;
class AttrFillGeometry;
class EffectsStack;
class ListRange;

// Define this if you want Phil's new SelRange area and perimeter features
//#define SELECTION_AREA_FEATURE

// Macros for scanning ranges of nodes...
//

/********************************************************************************************

>	void SCANRANGE(Range *pRange, NodeMethod func)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
+	Created:	?
	Purpose:	MACRO
				Applies the given method to every node in the given Range.
				i.e.
				MonoOn
					for (every item 'Node' in pRange)
						Node->\bfunc\b0;
				MonoOff
				
********************************************************************************************/


#define SCANRANGE(pRange,func) {\
								Node* pNode = pRange->FindFirst();\
								while (pNode)\
									{\
										pNode->func;\
										pNode = pRange->FindNext(pNode);\
									}\
								}


/********************************************************************************************

>	void SCANRANGEFORCLASS(Range *pRange, NodeMethod func, RunTimeClass RunTimeClass)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/03/94
	Purpose:	MACRO
				Applies the given method to every node of type 'RunTimeClass' in
				the given Range, i.e.
				MonoOn
					for (every item 'Node' in pRange)
						if (Node->IsKindOf(CC_RUNTIME_CLASS(\bRunTimeClass\b0)))
							Node->\bfunc\b0;
				MonoOff
				
********************************************************************************************/

#define SCANRANGEFORCLASS(pRange, func, RuntimeClass)						\
				{															\
					Node *Node = pRange->FindFirst();						\
					while (Node)											\
					{														\
						if (Node->IsKindOf(CC_RUNTIME_CLASS(RuntimeClass)))	\
							((RuntimeClass *)Node)->func;					\
						Node = pRange->FindNext(Node);						\
					}														\
				}



/********************************************************************************************

>	void SCANSELECTION(NodeMethod func)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?
	Purpose:	MACRO
				Applies the given method to every node in the Selection
				i.e.
				MonoOn
					for (every item 'Node' in the Selection)
						Node->\bfunc\b0;
				MonoOff
				
********************************************************************************************/

#define SCANSELECTION(func) {\
							SelRange* pSel = GetApplication()->FindSelection();\
						   	Node* pNode = pSel->FindFirst();\
						   	while (pNode)\
						   		{\
						   			pNode->func;\
						   			pNode = pSel->FindNext(pNode);\
						   		}\
						   	}


// The RangeControl type is required by Node's range functions. It is used to define the 
// nodes which are members of a range.  
//
struct CCAPI RangeControl
{
	RangeControl(BOOL Sel = FALSE,
						 BOOL Unsel = FALSE,
						 BOOL Cross = FALSE,
						 BOOL IgLock = FALSE,
						 BOOL Rendr = FALSE,
						 BOOL IgInvisible = FALSE,
						 BOOL SibsOnly = FALSE,
						 BOOL PromoteToParent = FALSE);		// Default constructor

	BOOL	Selected	:1;			// Selected nodes are included in the range
	BOOL	Unselected	:1;			// Unselected nodes are included in the range
	BOOL	CrossLayer	:1;			// Range can cross layers
	BOOL	IgnoreLockedLayers:1;	// Range will not enter locked layers
	BOOL	IgnoreNoneRenderable:1; // Dont include nodes whose NeedsToRender is false
	BOOL	IgnoreInvisibleLayers:1;// Range will not include invisible layers
	BOOL	SiblingsOnly:1;			// Only siblings from the first node are examined
	BOOL	PromoteToParent:1;		// Promotes nodes to the nodes with ShouldITransformWithChildren()
									// set to TRUE. Allows controller nodes to insert themselves
									// into the selection if a transformation on their children
									// needs to be applied through them.
}; 


// The description format type is used to specify the format of the description generated
// by the describe range function. 

enum DescriptionFormat {MENU, STATUS_BAR}; 


		    
class Node;     


/********************************************************************************************

> struct XOROutlineInfoStr

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>

	Purpose:	Contains information used by the XOR Outline rendering functions in the
				Range. THis includes a pointer to the "Prime Object" (which is the
				object that the user clicked on to start their drag), a rectangle
				which is used when comparing object sizes to determine how significant
				they are in the current drag (more significant objects are drawn first)
				and a couple of flags indicating if the significance region is based
				on the entire screen size or not, and whether the information is
				currently cached correctly.

********************************************************************************************/

typedef struct
{
	Node	*PrimeObject;					// The first object to draw (the one Mr. user clicked)
	DocRect	SigRegion;						// Region on which significance calculations are based
	BOOL	SigRegionIsScreen;				// The above region was whole visible screen area
	BOOL	Cached;							// TRUE if the above info has been cached
} XOROutlineInfoStr;



/********************************************************************************************

>	class Range : public CCObject

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/93
	Purpose:	This class represents a range of sibling nodes with a specified selected
				status. The range is specified by supplying a First and Last node (where
				Last must be a right sibling of First), and range control flags 
				which specify which nodes from First to Last inclusive, are to be
				included in the range. There are two range control flags, Selected
				and Unselected. When Selected is TRUE all selected nodes are included in the 
				range, when Unselected is TRUE all unselected nodes are included in the range, 
				and when both flags are TRUE all nodes from First to Last are included. 
				
				As an example consider the following sibling node list in a document tree 
				(where (S) indicates that the node has selected status). 
				
				N1(S)->N2->N3(S)->N4->N5->N6(S)               
				
				If First = N1, Last = N6, RangeControl.Selected = TRUE, and 
				RangeControl.Unselected = FALSE, then nodes N1, N3 and N6 will be included 
				in the range.
				
				The new CrossLayer flag enables the range to cross from one layer to the
				next in the tree. This only works if the range exists at the level which
				the user can directly edit - i.e. in the sibling lsist of layers.
				
********************************************************************************************/
class CommonAttrSet;
class Range: public CCObject
{  
public: 
	CC_DECLARE_DYNAMIC( Range )  
	Range(); 								   // Creates a NULL range 
	virtual ~Range();

	Range(Node*First, Node*Last, RangeControl RangeControlFlgs);    
	Range(Range& Range); 	
	Range& operator=(Range& Range); 	
	BOOL operator==(const Range&) const;
	BOOL operator!=(const Range&) const;

	void SetRangeControl(RangeControl RangeControlFlgs); 
	virtual Node* FindFirst(BOOL AndChildren = FALSE);
	virtual Node* FindLast();
	virtual Node* FindNext(Node* Previous, BOOL AndChildren = FALSE);
	virtual Node* FindPrev(Node* pNode, BOOL AndChildren = FALSE);
	virtual void  Update(BOOL TellWorld = FALSE);
	virtual void  FreshenCache() {};

	RangeControl GetRangeControlFlags() const { return RangeControlFlags; }

	BOOL ForceRenderEORAll() const { return TRUE; }

	UINT32 Count();
	BOOL Contains(Node* pNode, BOOL bAndChildren = FALSE);
	BOOL ContainsSelectInside();

	BOOL ConsistsOf(CCRuntimeClass* pccrtSearch, BOOL fIncludeDerivedClasses=TRUE);
	BOOL IsEmpty();

	List* MakeListOfNodes(BOOL AndChildren = FALSE);

	void UpdateParentBoundsOfSelection(BOOL AndChildren = FALSE);
	BOOL MakeAttributeComplete(	BOOL CheckForDuplicates = TRUE, 
								BOOL AndChildren = FALSE,
								BOOL IgnoreComplex = FALSE);
	void NormaliseAttributes(BOOL AndChildren = FALSE,
							 BOOL IgnoreComplex = FALSE);

	BOOL CopyComponentDataToDoc(BaseDocument* pSrcDoc,
								BaseDocument* pDestDoc,
								BOOL AndChildren = FALSE);
	
	// Helpful functions to XOR-redraw the selection, given a matrix by which 
	// objects are to be transformed (used by scale, rotate, selector, etc
	// tools as shared drag-redraw code).
	// See selector and rotate tools for examples of use

	void	ResetXOROutlineRenderer(BOOL FlushTheCache = TRUE);

	void	RenderXOROutlinesOn(DocRect *ClipRect, Spread *pSpread,
								Matrix *Transform, Node *ClickedObject = NULL);
	void	RenderXOROutlinesOff(DocRect *ClipRect, Spread *pSpread, Matrix *Transform);

	void	RenderXOROutlinesToCatchUp(DocRect *ClipRect, Spread *pSpread, Matrix *Transform);

	Node	*NodeToRender;					// The node that we are up to
											// (for rendering more significant objects first)
	
	virtual DocRect GetBoundingRect();

	// DMc gets the bounding rect to render when eor dragging
	virtual DocRect GetBoundingRectForEorDragging();

	// Func to call AllowOp() on all nodes in the range.
	virtual BOOL AllowOp(ObjChangeParam* pParam,BOOL SetOpPermissionState = TRUE,
												BOOL DoPreTriggerEdit = TRUE);

	// This function returns a set of the compound parent nodes of every item in the range 
	BOOL GetCompoundObjectSet(ObjectSet* pCompoundObjectSet, BOOL ExcludeTextObjects = FALSE); 
	// This fn is for BODGE purposes
	BOOL AddTextStoryCompoundsForDel(ObjectSet* pCompoundObjectSet);

	// CommonAttribResult is returned by Range::FindCommonAttribute()
	enum CommonAttribResult { ATTR_COMMON, ATTR_NONE, ATTR_MANY };

	// For finding a common attributes in the range. 
	Range::CommonAttribResult FindCommonAttribute(CCRuntimeClass* AttribClass, 
								    		 	  NodeAttribute** pAttribute,
												  BOOL bRangeIsEffectsLevel = FALSE);

	BOOL FindCommonAttributes(CommonAttrSet* CommonAttributeSet, BOOL CompoundAccuracy = FALSE);

	Range::CommonAttribResult FindCommonAttributeType(CCRuntimeClass* AttribType, 
												 		  	CCRuntimeClass** pCommonType,
															BOOL ForceAttribType = FALSE,
															   BOOL bRangeIsEffectsLevel = FALSE);

	Range::CommonAttribResult FindCommonNonOptimisingAttr(	CCRuntimeClass* pAttribClass,
															NodeAttribute** ppAttr );

	// Find a bitmap in this range
	BOOL FindBitmap(KernelBitmap** ppFoundBitmap = NULL,
					KernelBitmapRef** ppFoundBitmapRef = NULL,
					NodeBitmap** ppFoundNode = NULL,
					AttrFillGeometry** ppFoundFillAttribute = NULL);

	// This function gets called to inform the range that attributes applied to objects in 
	// the range have changed. It deletes the CommonAttrCache if one exists.
	virtual void AttrsHaveChanged(); 
		 
	virtual void SetRenderable(BOOL bNewVis = FALSE);
	virtual Range* CloneNodes(UINT32 timeLimit, BOOL bCloneOnTop = FALSE, BOOL bLightweight = FALSE);
	virtual void DeleteNodes();
	virtual void ForceRedrawView(DocView* pDocView, BOOL bReleaseCache = FALSE, BOOL bUseBlobRects = FALSE, BOOL bReleaseParentsOnly = FALSE);
	virtual BOOL TransformNodes(TransformBase* Trans);
	virtual void SetDraggedNodes(BOOL bNewState = FALSE);
	virtual void CopyNodesContents(Range* pDestRange, BOOL bSetDragged = FALSE, BOOL bSetRenderable = FALSE);

	virtual BOOL SetPromoteToParent(BOOL bNewValue) {BOOL b = RangeControlFlags.PromoteToParent; RangeControlFlags.PromoteToParent = bNewValue; return b;}

protected:
	// This fn will return a pointer to the common attribute cache. If it does not exist then
	// one will be created
	CommonAttrSet* GetCommonAttrCache(); 

	// The common attribute cache. This list will only ever exist
    // if the FindCommonAttribute, or FindCommonAttributes functions
  	// are called on the range. The list contains CommonAttributeItems. 
	CommonAttrSet* pCommonAttribCache; 


	BOOL	FindNextXOROutlineNode(Node **OriginalNode, DocRect *pClipRect);
	BOOL	CacheXOROutlineInfo(Spread *pSpread, Node *ClickedObject = NULL);

protected:  
	RangeControl RangeControlFlags; 

	Node* FirstNode; 
	Node* LastNode; 

	virtual Node* SmartFindNext(Node* pContext, BOOL AndChildren = FALSE, BOOL FindFirst = FALSE) const;
	virtual BOOL InRange(Node* pNode, BOOL AndChildren) const;
	
   	INT32	NumObjectsDrawn;				// A count of the number of objects rendered so far
	INT32		RenderingPass;					// Which pass the rendering is up to

	// Cache of information for XOROutline renderer functions...
	XOROutlineInfoStr XORInfo;				// (See above for defn of this struct)

};




/********************************************************************************************

>	class NodeListItem : public ListItem

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Purpose:	Stores a pointer to a Node. Used to build a list of nodes in the current range.
				The actual node can be accessed through the public member variable pNode.
	SeeAlso:	Range::MakeListOfNodes

********************************************************************************************/

class NodeListItem : public ListItem
{
public:
	CC_DECLARE_DYNAMIC( NodeListItem )
	NodeListItem();					// will contain a NULL pointer
	NodeListItem(Node* WhichNode);	// initialise pNode to be WhichNode
	~NodeListItem() {}				// Nothing to destruct
	
	Node* pNode;					// pointer to a node
};




/********************************************************************************************

>	class SelRangeMessageHandler : public MessageHandler

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/94
	Purpose:	Intercepts messages for the SelRange.
				(Basically sits there and flushes the Selection's information cache
				whenever it thinks the selection cache has become invalid)
				
********************************************************************************************/

class SelRangeMessageHandler : public MessageHandler
{
public:
	CC_DECLARE_DYNAMIC( SelRangeMessageHandler ) 
	SelRangeMessageHandler();				// Constructor

protected:
	MsgResult Message(Msg* Msg);			// Overridden message reciever
};




/********************************************************************************************

>	class SelRange : public Range

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com> & Jason
	Created:	28/11/94 (Date this comment added!!! Actually written long before this)
	Purpose:	This class represents a range of selected nodes directly under the
				Selection Surface.
				The Selection Surface is the highest surface in the tree at which the user
				can select nodes. It is usually the child list of Layers but the user can
				push the Selection Surface lower, into groups, using Select Inside.

				The SelRange also offers a set of cached information pertinent to
				the selection, such as the bounds, number of objects included, etc.
				This cache generally makes access to such information much more efficient
				than a brute force treesearch.

********************************************************************************************/

class SelRange: public Range
{
	CC_DECLARE_DYNAMIC( SelRange )  
	friend class SelRangeMessageHandler; 

public:
	SelRange();
	~SelRange();
	
	// DMc
	SelRange(SelRange &From);

	static BOOL DeclarePrefs();
	void FreshenCache();

// New functions specifically for the selection range object...
	virtual void Update(BOOL TellWorld = FALSE, Node *SelectedNode = NULL);
					// Inform a SelRange object that the selection has changed (FLUSHES the cache)

	void UpdateBounds(); // This fn get's called when the range's bounds have been changed

	virtual void BroadcastAnyPendingMessages(void);
					// Ensures that any pending SelChangingMsg broadcasts are sent

	virtual void AttrsHaveChanged(); // This function gets called whenever attributes are applied to the 
							  		// range. It invalidates the common attribute cache.

private:
	void FreshenBoundsCache();


public:
	virtual DocRect	GetBoundingRect(BOOL bPromoteToControllers = FALSE);		// Get bounds of the entire selection
	DocRect	GetBlobBoundingRect();			// Get bounds, including selection blobs
	virtual DocRect GetBoundsWithoutAttrs(void);	// Get bounds excluding line widths and other attrs
#ifdef SELECTION_AREA_FEATURE
	virtual BOOL GetAreaDetails(XLONG* pxlArea, XLONG* pxlPerim);
	virtual BOOL GetAreaDetailsWhileIdle(XLONG* pxlArea, XLONG* pxlPerim);
#endif

	Node*	FindLast();						// Find the last item in the range

// Overridden functions inherited from Range...
	void	SetRangeControl(RangeControl RangeControlFlgs); 
	Node*	FindFirst(BOOL AndChildren = FALSE);				// Find first object in the selection
	Node*	FindNext(Node* Previous, BOOL AndChildren = FALSE);	// Find the next object in the selection
	UINT32	Count();											// How many primary objects are in the selection

	// Find a node in the selection of a given type (returns TRUE if found)
	BOOL  FindObject(CCRuntimeClass* p, Node** pOutput = NULL);
	Node* FindFirstObject(CCRuntimeClass* p);
	Node* FindNextObject(CCRuntimeClass* p, Node* Previous);
	Node* FindFirstDerivedObject(CCRuntimeClass* p);
	Node* FindNextDerivedObject(CCRuntimeClass* p, Node* Previous);

public:
	String_256 Describe(DescriptionFormat Format);

	   
private:
	String_256 LayerDescription(DescriptionFormat Format);
	String_64 ObjectDescription(DescriptionFormat Format);

	void SelChangedCommit(); 

public:
	// This function gets called at the end of each operation, and other places where the selection
	// may have changed, it should not get called in response to Selection changed messages because
	// it is quite slow.
	void	DescribeSelectionInStatusBar(); 

	// A general fn to convert a singular to a plural (for English at least)
	static  String_256 Pluralise(String_256 Noun);


	BOOL	IsCached()	{ return Cached; }


	// Determine the last node that the SelRange can remember having been selected.
	// This is only a short-term memory - it may well return NULL
	Node *GetLastSelectedNode(void);

	// Ensure that the given node is not cached in our 'pLastSelectednode' member variable,
	// as it is being destructed, so it is no longer safe to keep this pointer
	// Called by Node::~Node() - shouldn't really be called by anyone else
	inline void ClearLastSelectedNodeIfItIs(Node *TheNodeToCheck);

	void SetGag(BOOL NewState);
	BOOL IsSelRangeGagged();

	BOOL MakePartialSelectionWhole(BOOL TellWorld = TRUE, BOOL bTextOnly = FALSE, BOOL bPathInText = FALSE);

	EffectsStack*	GetEffectsStack(BOOL bGetCopy = FALSE, BOOL bEscapeDerived = TRUE);
	ListRange*		GetTopClassRange(CCRuntimeClass* pClass, BOOL bClassOnly = TRUE, BOOL bIgnorePassThroughEffects = FALSE, Node** ppMasterNode = NULL, INT32* piStackPos = NULL);

private:
	Document *ScopeDocument;				// Points at the document we have cached this
											// information for.

	BOOL	Cached;							// TRUE when Range is directly usable.
											// FALSE when the tree has to be scanned

	BOOL	InformedSelChanged;				// Has the sel changed message been broadcast

	BOOL	IsGagged;						// Is the SelRange gagged from sending
											//  SelChanged messages

	BOOL	CachedBounds; 					// Set to FALSE in UpdateBounds function
											// used to indicate when the bounds of the 
											// selection are/are not cached

	DocRect	CachedBBox;						// Cache for the bounding box of the selection
											// Only valid when (Cached==TRUE && CachedCount>0)

	DocRect	CachedBlobBBox;					// As above, but includes selection blobs
											// Only valid when (Cached==TRUE && CachedCount>0)

	BOOL	CachedBoundsNoAttrs;			// BBox cache for non-attribute bounds, and
	DocRect CachedBBoxNoAttrs;				// its associated cached/not-cached flag

	INT32	CachedCount;					//  >0 : Cached number of objects in range
											// 		 This value is INVALID if Cached == FALSE
											// ==0 : Number of objects unknown (uncached)
											//		 or number of objects is 0 (See Cached flag)
											//  <0 : Contains count of number of objects
											//		 so far encountered during a FindNext()
											//		 range scan. At the end of such a scan,
											//		 the correct count will have been cached,
											//		 so long as the search was sequential.
											// Thus MUST be accessed through Count() function

	Node	*pLastSelectedNode;				// NULL, or points at the last node which was made
											// selected (usually the last object user clicked)

	Document *pLastSelectedDoc;				// The document (if any) in which the
											// LastSelectedNode resides.

	Node	*LastFindNextNode;				// Memory of last 'Previous' node passed to
											// FindNext - used to cache a count if possible
											// during a FindNext scan of the tree.

	SelRangeMessageHandler* MessageHandler;	// Object to intercept messages and flush our cache
											// if the selection has been changed

	EffectsStack*	m_pEffectsStack;		// Cached effect stack

	ListRange*		m_pEffectClassRange;	// Cached effect level (may not match a level in the effect stack)
											// This is stored in SelRange context so that it shares
											// the valid lifetime of SelRange and can be re-used by callers
											// instead of having to be rebuilt whenever it's needed
	BOOL			m_bEffectClassNodesOnly;// Record state of bClass flag along with m_pEffectClassRange
											// so that m_pEffectClassRange is only re-used when bClass is the same

	Node*				m_pAreaNode;		// Current context of idle area computer
};



/******************************************************************************************

>	inline void SelRange::ClearLastSelectedNodeIfItIs(Node *TheNodeToCheck)

     Author:    Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
     Created:   4/1/95

     Inputs: 	TheNodeToCheck - A pointer to a node being destructed.

     Purpose:   Ensures that the SelRange does not retain a pointer to a node
	 			(in pLastSelectedNode) which has been destructed.
				Called by Node::~Node() - not to be called by anyone else

	 SeeAlso:	SelRange::GetLastSelectedNode; Node::~Node

******************************************************************************************/

inline void SelRange::ClearLastSelectedNodeIfItIs(Node *TheNodeToCheck)
{
	if (TheNodeToCheck == pLastSelectedNode)
		pLastSelectedNode = NULL;
}



/********************************************************************************************

>	class CommonAttrSet: public List

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/95
	Purpose:	A CommonAttributeSet. See Range::FindCommonAttributes 
	SeeAlso:	Range::FindCommonAttributes
	SeeAlso:	CommonAttributeItem

********************************************************************************************/
class NodeRenderableInk;
class CommonAttrSet: public List
{
public:
	CommonAttrSet() { }; 						// Constructs an empty set
	virtual ~CommonAttrSet() { DeleteAll(); };	// Delete all items in the set before we leave
	
	// Methods for setting up the CommonAttrSet prior to a call to FindCommonAttributes
	BOOL AddTypeToSet(CCRuntimeClass* AttrType);
	BOOL AddAllTypesToSet();

	// Methods for accessing the CommonAttrSet 
	CommonAttributeItem* FindAttrItem(CCRuntimeClass* AttrType);
	BOOL FindAttrDetails(CCRuntimeClass* AttrType, 
					  NodeAttribute** pAttr, 
					  Range::CommonAttribResult* Status);

	BOOL IsAnyItemCommon(); 
	void ClearResults();

	INT32 UpdateKnownValues(CommonAttrSet* pCache);
	
	enum ObjectAttrUsage { REQUIRED, 	 		 // The attribute is required by at least one child object
												 // but does not appear as a child. => Default attr is 
												 // common
						   FOUND,   			 // The attribute is required by at least one child object
						   						 // and has been found as a child. => Default attr is not common 
						   NOT_REQUIRED };		 // Attribute is not required by any child.

	// Special fn used by FindCommonAttributes to determine if a default attribute is common to a compound
	// object.					    
	BOOL BuildAttrDetailsMap(NodeRenderableInk* CompoundObject, 
							 INT32 FirstN,
							 CCAttrMap* pAppliedAttrMap, 
						     CMapPtrToWord* pCompoundAttrMap); // Maps an attribute type to an ObjectAttrUsage value
};




/********************************************************************************************

>	class ListRange : public Range

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/12/2003
	Purpose:	This class represents a list of explicitly declared nodes (rather than
				the implicitly chosen nodes controlled by RangeControl flags).

********************************************************************************************/

class ListRange: public Range
{
	CC_DECLARE_DYNAMIC(ListRange)

public:
	ListRange();
	ListRange(const List& srcList);
	ListRange(const ListRange& srcRange);
	ListRange(const ListRange* pSrcRange);
	~ListRange();

	virtual Node* FindFirst(BOOL AndChildren = FALSE);
	virtual Node* FindLast();
	virtual Node* FindNext(Node* Previous, BOOL AndChildren = FALSE);
	virtual Node* FindPrev(Node* pNode, BOOL AndChildren = FALSE);

	virtual void AddNode(Node* pNode);
	virtual Node* RemoveNode(Node* pNode);
	virtual void Clear();
	virtual BOOL MatchesSelectionEffectLevel(INT32 iStackPos);

protected:  
	virtual BOOL InRange(Node* pNode, BOOL AndChildren) const;
	
private:
	List nodelist;
	NodeListItem*	pLastReturnedItem;
	
};




#endif

