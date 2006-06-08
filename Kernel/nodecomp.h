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

#ifndef NODECOMPOUND_H
#define NODECOMPOUND_H

#include "ink.h"

class NodeCompound;
class BlendNodeParam;
class NodeBlend;
class NodeGroup;


/***********************************************************************************************

>	class CompoundNodeTreeFactory : public CCObject

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Encapsulates all data needed to create a new compound node of a particular type
				Abstract base class to be inherited from

***********************************************************************************************/
class CompoundNodeTreeFactory : public ListItem
{
	CC_DECLARE_DYNCREATE(CompoundNodeTreeFactory);

public:

	// pass in the list of nodes to apply the compound node to - WITHOUT CHILDREN !
	// these nodes will appear as first children of the new compound node
	// returns TRUE for success

	virtual NodeCompound * CreateNode(List *pList, UndoableOperation * pOp = NULL) { return NULL;}
} ;




/***********************************************************************************************

>	enum ListType

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	When?
	Purpose:	Identifies rampant lists of temporary nodes - URGH!

***********************************************************************************************/

enum ListType
{
	LT_SHADOWSLIST, LT_BEVELSLIST, LT_CONTOURSLIST, 
	LT_BECOMEA_SHADOWSLIST, LT_BECOMEA_BEVELSLIST, LT_BECOMEA_CONTOURSLIST
};


							
							
/***********************************************************************************************

>	class NodeCompound : public NodeRenderableInk

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Encapsulates everything to do with compound nodes

***********************************************************************************************/

class NodeCompound : public NodeRenderableInk
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeCompound);

public:
	NodeCompound(); 
	~NodeCompound(); 
	
	NodeCompound(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
			  );
	
	// does the regen code !
	virtual ChangeCode OnChildChange(ObjChangeParam* pParam);

	// special selection functions are required for some types of compound node.
	virtual void SetSelected(BOOL Status);
	virtual void Select(BOOL ReDraw);
	virtual void DeSelect(BOOL ReDraw, BOOL bDeselectChildren = FALSE);

	virtual BOOL IsCompound() const { return TRUE; }		// See also BaseTextClass
	virtual BOOL IsCompoundClass() const { return TRUE; }

	virtual INT32 EstimateNodeComplexity(OpParam* details);
	virtual BOOL HasVisibleChildren() const;

	virtual BOOL AllowOp_AccountForCompound(ObjChangeParam* pParam,
											BOOL SetOpPermissionState = TRUE,
											BOOL DoPreTriggerEdit = TRUE);

	// dpi for rendering
	void SetDPI(double dpi) { m_DPI = dpi; }
	double GetDPI() { return m_DPI; }

	// tells the node that we are printing
	void SetPrinting(BOOL bPrinting) { m_bPrinting = bPrinting; }
	BOOL IsPrinting() { return m_bPrinting; }

	// returns FALSE if I shouldn't regenerate before printing or export
	virtual BOOL RegenerateForPrinting() { return TRUE; }

	// gets the rectangle of all nodes under me without their 'NeedsParent' set
	virtual DocRect GetInsideBoundingRect();

	// in blending we need to get the blending node out of this node
	virtual NodeRenderableInk * GetNodeToBlend();

	virtual NodeCompound* GetParentController() const {return NULL;}

	// creates a tree wrapping up the given node
	virtual NodeRenderableInk * CreateTreeFromNodeToBlend(NodeRenderableInk * pNode,
				CCAttrMap * pAttrMap) { return NULL; }

	void SetStartBlendGroupNode(BOOL bFlag=TRUE) { m_bBlendStartNode = bFlag; }
	void SetEndBlendGroupNode(BOOL bFlag=TRUE)   { m_bBlendEndNode   = bFlag; }

	BOOL IsStartBlendGroupNode() { return m_bBlendStartNode; }
	BOOL IsEndBlendGroupNode() { return m_bBlendEndNode; }

	void SetBlendCreatedByNode(NodeCompound * pNode) { m_pBlendCreatedByNode = pNode; }
	NodeCompound * GetBlendCreatedByNode() { return m_pBlendCreatedByNode; }

	// whether attributes assignments should be promoted to me, except for
	// the 'needs parent' nodes
	// ie if dragndrop of a colour happens to the original nodes, then the
	// attribute assignment is promoted to the controller node if this returns TRUE
	// but if the attribute is applied to a NeedsParent node, then it isn't
	virtual BOOL PromoteAttributeApplicationToMe(CCRuntimeClass *pAttrClass) const { return FALSE; }

	// Routines to get/set the names of groups
	virtual String_32& GetName(); 
	virtual void SetName(String_32& GroupName); 

	// Functions for converting nodes into other types
  	virtual BOOL CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);
	virtual NodeGroup* BecomeAGroup(UndoableOperation* pUndoOp);

	virtual DocRect				GetChildBoundingRect(BOOL bIncludeAttrs = TRUE);
	virtual BOOL				HasEffectAttrs() const;
	virtual BOOL				ContainsEffects() const;
	virtual void				TransformEffectAttrs(TransformBase& Trans);

	// Karim MacDonald 25/05/2000
	// Extend functions.
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
public:
//	virtual BOOL IsTypeExtendible() const { return TRUE; }
//	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
//	virtual void Extend(const ExtendParams& ExtParams);

	// virtual override of NodeRenderableBounded::GetExtendTargetBounds().
	virtual DocRect GetExtendTargetBounds(const ExtendParams& ExtParams);

	// DY get the ink node underneath a contour, bevel, shadow, or whatever, though
	// for this class it will return NULL.
	virtual NodeRenderableInk* GetInkNodeFromController() const;

protected:
	// causes the children to regenerate themselves
	// virtual BOOL RegenChildren(UndoableOperation * pOp);

	// recursive function for RegenChildren
	// BOOL RegenerateNode(Node * pNode, UndoableOperation * pOp);

	double m_DPI;

	BOOL m_bPrinting;

	// informs all parent nodes that a regeneration is about to occur (through
	// the child change message) or has occured
	virtual void PreInformParentsOfRegenerate();
	virtual void PostInformParentsOfRegenerate();

	// blend variables to tell me whether I'm a start of a blend path group or
	// the end of a blend path group
	BOOL m_bBlendStartNode;
	BOOL m_bBlendEndNode;

	// finds the blend node associated with myself in the end blend tree
	NodeCompound* FindAssociatedBlendNode(BlendNodeParam * pParam);

	// the node which created this node when blending
	NodeCompound* m_pBlendCreatedByNode;

	// The name of the group
	String_32 CompoundName;

// -----------------------------------------------------------------------------------------------
protected:
	/* CGS (16/6/2000)

		When a blend exists between a couple of group blends and we perform convert to editable
		shapes on it ....
		  
		If the internal (generated) compound nodes are shadowed, we MUST
		get our shadow to 'go' with the relevant intermediate compound node.  shadowThisNode
		allows us to do this.  DO NOT USE IT FOR ANYTHING ELSE !!!!
	*/
	
	static NodeCompound* shadowThisNode;
	
	/* GCS (3/7/2000)

		Compounds that are generated as intermediate nodes need to be deleted - otherwise we
		waste vast amounts of memory!  Although shadows, bevels and countours all delete themselves,
		in the case that we have a shadowed bevel (or contour); then the shadow MUST delete the
		bevel (or contour) after it has finished using it.  shadowDeleteThis allows us to achieve
		this requirement.  DO NOT USE IT FOR ANYTHING ELSE !!!!
	*/
	
	static NodeCompound* shadowDeleteThis;

	// CGS - support for the above NodeGroup pointers ....

	NodeCompound* GetShadowDeleteThisNode()				{ return (shadowDeleteThis); }
	NodeCompound* GetShadowThisNode()						{ return (shadowThisNode); }

	void SetShadowThisNode(NodeCompound* thisNode)		{ shadowThisNode = thisNode; } 
	void SetShadowDeleteThisNode(NodeCompound* thisNode)	{ shadowDeleteThis = thisNode; }

	// CGS (20/6/2000)
	// when blending a grouped blend to another grouped blend .....
	// some nodes have BECOMEA_PASSBACK called on them.  Now, since I am creating a copy
	// of the relevant node (controllers and their children); I need to be able to delete
	// them.  The only problem is that if you delete them just after creating them (as I do
	// elsewhere, BlendAttributes () access violates!  To overcome this problem,
	// I have found it necessary to insert them into a list; and delete them all at the
	// very end.
	// CGS: this is also now TRUE for all blend rendering code (no matter how complex the blend)
	// do NOT use this member variable for anything else!

	static List* blndConsBecomeAShadows;	// list of shadow controllers that were generated when blending (internally via PASSBACK)
	static List* blndConsBecomeABevels;		// list of bevel controllers that were generated when blending (internally via PASSBACK)
	static List* blndConsBecomeAContours;	// list of contour controllers that were generated when blending (internally via PASSBACK)

	NodeBlend* blenderNode;					// ptr to the node blend with which *this* node is associated with
											// this is required so that BecomeA lists are only cleared
											// for the relevant nodes (otherwise all manner of access
											// violations break out) ....  Take a look at NodeBlender::Deinit()
											// for an example of its usage.

	static BOOL isForPrinting;				// we need to flag nodes that are inserted when printing,
											// so that we may delete them without affecting others

public:

	// CGS:  support functions for the above ....

	static BOOL AllocBlendConsList(ListType listType);
	static BOOL AllocatedBlendConsList (ListType listType);

	static BOOL BlendConsListInsert(ListType listType, Node* insertNode);

	static BOOL KillBlendConsListItem(ListType listType, NodeBlend* NodeBlend);
	
	static BOOL KillBlendConsList(ListType listType, BOOL killAll, BOOL isPrinting);

	static BOOL KillAllBlendBecomeAConsLists(BOOL killAll = FALSE, BOOL isPrinting = FALSE);
	static BOOL KillAllBlendBecomeAConsListItem(NodeBlend* associatedWith);

	static void SetIsForPrinting(BOOL newVal) { isForPrinting = newVal; }

	NodeBlend* GetBlenderNode() { return (blenderNode); }
	void SetBlenderNode(NodeBlend* blender) { blenderNode = blender; }

private:

	static List* InitListPtr(ListType listType);

#ifdef _DEBUG

public:

	static INT32 shadowLastID;
	INT32 myShadowID;
	static INT32 bevelLastID;
	INT32 myBevelID;
	static INT32 contourLastID;
	INT32 myContourID;

	// for DoBecomeA stuff ....

	static INT32 shadowLastBecomeAID;
	INT32 myShadowBecomeAID;
	static INT32 bevelLastBecomeAID;
	INT32 myBevelBecomeAID;
	static INT32 contourLastBecomeAID;
	INT32 myContourBecomeAID;

#endif

};



#endif
