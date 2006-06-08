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
// Defines a class that will become a parameter to the Node::DoBecomeA() function

#ifndef INC_BECOMEA
#define INC_BECOMEA

class Node;
class NodeRenderableInk;
class UndoableOperation;
class CCAttrMap;

//-----------------

enum BecomeAReason
{
	BECOMEA_REPLACE,		// How to handle this reason in your DoBecomeA() function:
							//
							// 		Make a new node of type GetClass();
							//		Remove yourself from the tree  (in an undoable way)
							//		Put the new node in your place (in an undoable way)
							//
							// GetUndoOp() should not return NULL. If it does, error!
							//
							// When trying to replace a node with one of the same type,
							// DON'T DO ANYTHING!!
							//
							// E.g. when trying to replace a NodePath with a NodePath,
							// there is nothing to do because it is pointless replacing
							// your node with an exact copy of yourself in the tree.


	BECOMEA_PASSBACK,		// How to handle this reason in your DoBecomeA() function:
							//
							// 		Make a new node of type GetClass()
							//		Pass it on via PassBack()
							//
							// With this reason, ALWAYS create a new node.  If your node
							// is one of the same type, use SimpleCopy() to create a copy
							// of yourself, and pass the copy back via PassBack()
							//
							// 	BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode)
							//
							// The node you've just created is the pNewNode param and a pointer to your 
							// node (the 'this' pointer) is the pCreatedByNode param.
							//
							// Once the new node is passed back, it is no longer your concern, ie. you
							// don't have to worry about deleting it at a later stage. This should all be
							// done by the derived instance of the give BecomeA class.
							//
							// Remember that PassBack() returns a BOOL, so if FALSE is returned, you must
							// error and recover as normal.
							//
							// It is possible for GetUndoOp() to return NULL with this reason
							//
							// Karim 12/06/2000
							// Note to BecomeA implementors - realise that although it is forbidden
							// for someone else to pass you a node linked into the main tree via
							// PassBack(), it is *perfectly* fine for them to return you an isolated SUBTREE.
							// Therefore, use code similar to the following when disposing of the returned
							// node (assuming it's non-NULL!):
							//										pNewNode->CascadeDelete();
							//										delete pNewNode;
							//										pNewNode = NULL;

	BECOMEA_TEST			// This is only used when calling CanBecomeA and is just a dummy
							// value used where it's not appropriate to pretend to be a
							// replace or passback. (BecomeA reasons are generally ignored
							// by CanBecomeA functions.)
};


/********************************************************************************************

>	class BecomeA : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Purpose:	Class that encapsulates the params needed by virtual Node::DoBecomeA()

********************************************************************************************/

class BecomeA : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(BecomeA);
public:
	BecomeA(BecomeAReason ThisReason,
			CCRuntimeClass* pClass,
			UndoableOperation* pOp = NULL,
			BOOL sel = TRUE,
			BOOL First = FALSE
			);

	virtual ~BecomeA() { }

	// Member variable access functions
	BecomeAReason		GetReason()		{ return Reason; }
	CCRuntimeClass*		GetClass() 		{ return pClassToBecome; }
	UndoableOperation*	GetUndoOp()		{ return pUndoOp; }			// Can return NULL
	BOOL				Reselect()		{ return Select; }
	BOOL				IsFirstNode()	{ return AmFirstNode; }
	BOOL				DoSilhouette()	{ return fSilhouette; }
	void				SetDoSilhouette(BOOL bEnable) { fSilhouette = bEnable; }
	BOOL				DoShadowSilhouettes() { return fShadowSilhouettes; }
	void				SetDoShadowSilhouettes(BOOL bEnable) { fShadowSilhouettes = bEnable; }

	void SetInsertComplexBlendStepsAsPaths (BOOL val) { insertComplexBlendStepsAsPaths = val; }
	BOOL GetInsertComplexBlendStepsAsPaths () { return (insertComplexBlendStepsAsPaths); }
	
	// DMc inclusion so that node using this becomeA can make a record of itself inside
	// of the passback mechanism for nodes in which the passback is called on to use
	Node			 *	GetCallNode()	{ return pCallNode; }
	void				SetCallNode(Node * pNode)	{ pCallNode = pNode; }

	// whether this is a become a from a blend or not
	virtual BOOL IsBlendBecomeA() { return FALSE; }
	// whether this is a become a from a compound blend or not
	virtual BOOL IsCompoundBlendBecomeA() { return FALSE; }
	// whether this is a become a from a combine or not
	virtual BOOL IsCombineBecomeA() { return FALSE; }

	virtual BOOL IsBrushBecomeAGroup() { return FALSE;}

	// This function should be called when Reason == BECOMEA_PASSBACK 
	// (see above for a description of this reason code)
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap=NULL);

	// Quick function to test the common case that pClassToBecome is a CC_RUNTIME_CLASS(NodePath)
	virtual BOOL BAPath() const;

	// Access functions for CanBecomeA counting
	virtual void AddCount(UINT32 iNum) {m_Count += iNum;}
	virtual UINT32 GetCount() const {return m_Count;}
	virtual void ResetCount() {m_Count = 0; m_bIsCounting = TRUE;}	// Call ResetCount to clear count and enable counting
	virtual BOOL IsCounting() const {return m_bIsCounting;}

	// Will the BecomeA move the results after they have been created or place them alongside their originating node?
	virtual BOOL ResultsStayInPlace() const {return m_bInPlace;}
	virtual void SetResultsStayInPlace(BOOL bInPlace) {m_bInPlace = bInPlace;}

	virtual BOOL IsSecondary() const {return m_bSecondary;}
	virtual void SetSecondary(BOOL bNewValue = TRUE) {m_bSecondary = bNewValue;}

	virtual BOOL OnlyNeedPaths() const {return m_bPathsOnly;}		// Don't need fill attributes of effects
	virtual void SetPathsOnly(BOOL bNewValue = TRUE) {m_bPathsOnly = bNewValue;}

private:
	BecomeAReason			Reason;
	CCRuntimeClass*			pClassToBecome;
	UndoableOperation*		pUndoOp;
	BOOL					Select;
	BOOL					AmFirstNode;
	Node					*pCallNode;
	BOOL					m_bIsCounting;
	BOOL					m_bBecomeANodePath;
	BOOL					m_bInPlace;
	BOOL					m_bSecondary;			// Set when path processor should not delete original
	BOOL					m_bPathsOnly;

	// Karim 14/06/2000
	// a flag used for PASSBACK BecomeA's, denoting whether the target node should merely
	// pass back enough information to determine its outline (including line width).
	// this is useful for ops like contouring and bevelling, which do not necessarily need
	// full path information, just enough to determine the silhouette of the shape.
protected:
	BOOL					fSilhouette;
	BOOL					fShadowSilhouettes;
	BOOL					insertComplexBlendStepsAsPaths;			// CGS (23/10/2000)
	UINT32					m_Count;

	// in-order for the slicing of compound blends to work, compound nodes MUST be inserted
	// into the camelot tree as paths - and NOT as complete compounds (e.g.  NodeBevelController .. NodeBevel .. etc.)
	// setting insertComplexBlendStepsAsPaths to TRUE allows this to happen.
	// The member variable could be used for controlling other such operations in the future
};


/***********************************************************************************************

>	class CopyBecomeA: public BecomeA

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/95
	Purpose:	This is the BecomeA class that is passed to other nodes when we need to make
				'pathified' copy of a subtree. The effect is to create a bunch of nodes
				which copy identically the specified tree.

***********************************************************************************************/

class CopyBecomeA : public BecomeA
{
	CC_DECLARE_MEMDUMP(CopyBecomeA);

	public:
		CopyBecomeA(BecomeAReason Reason,
					CCRuntimeClass* pClass,
					UndoableOperation* pOp) : BecomeA(Reason,pClass,pOp) {pContextNode=NULL;}

		// This function should be called when Reason == BECOMEA_PASSBACK 
		virtual BOOL PassBack(NodeRenderableInk* pNewNode,
							  NodeRenderableInk* pCreatedByNode,
							  CCAttrMap* pAttrMap);

		void SetContextNode(Node* pNode) { pContextNode = pNode; }

	private:
		// Where to hang the node.
		Node*	pContextNode;

};

#endif  // INC_BECOMEA


