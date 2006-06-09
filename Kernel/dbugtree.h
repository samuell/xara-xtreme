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

#ifndef INC_DBUGTREE
#define INC_DBUGTREE

//#include "dialogop.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// -----------------------------------------------------------------------------------------
// DebugTreeDlg for displaying the Camelot tree
 
#define OPTOKEN_DEBUGTREEDLG _T("DebugTreeDlg")    
    
class Document;
    
class DebugTreeDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(DebugTreeDlg)
public:

	DebugTreeDlg();      
	~DebugTreeDlg(); 

	MsgResult Message( Msg* Message); 
	void Do(OpDescriptor*);		// "Do" function        
	static BOOL Init();                        
	BOOL Create(); 
	static OpState GetState(String_256*, OpDescriptor*);

	static void TweeDump(BaseDocument *DocToDump = NULL);
	static void DumpSubTree(Node* pSubTree, INT32 FromParent=0, INT32 MaxDepth=9999);

	static CDlgResID IDD() {return _R(IDD_NEWDEBUGTREE);}

private:
	INT32 DisplayNodesChildren(Node *pNode, INT32 ListIndex, BOOL AddOnly = FALSE);
	INT32 GetTreeDepth(Node *pNode);
	void ShowNodeDebugInfo(INT32 ListIndex); 
	void ShowNodeDebugInfoForNode(Node *pNode);
    void ExpandTree();
	void DumpTree();
    void Refresh();
	void Initialise();
	void GetTree();
	void DeInit();

	BOOL ShouldBeHidden(Node*);
	BOOL IsChildOfHiddenNode(Node*);
	BOOL HasChildren(Node *pNode);
	void AddNode(Node *pNode, INT32 Index, INT32 Depth,
				 BOOL HasChildren, BOOL ChildrenAreDisplayed, 
				 BOOL IsAHiddenNode, BOOL IsHidden);
	void RemoveNode(INT32 Index);
	void UpdateNode(INT32 Index, StringBase *NewText);
	BOOL IsAHiddenNode(Node *pNode);

	void ShowHidden(BOOL);
	INT32 DeleteNode(INT32 Index, BOOL Recurse = FALSE, INT32 Count = 0);
	
	// New TreeView stuff
	CTreeItemID AddOneItem(CTreeItemID hParent, const StringBase& str, CTreeItemID hInsAfter, INT32 iImage, Node *pNode);
	CTreeItemID AddItemToNewTreeControl(CTreeItemID hParentItem, CTreeItemID hInsAfter, Node *pNodeToAdd, BOOL bAddChildren = TRUE);
	BOOL InitialiseNewTreeControl(void);
	INT32 GetImageForNode(Node *pNode);
	BOOL GetInfoFromTreeItem(CTreeItemID hItem, Node** pNode, size_t* pChildren);
	void DumpTreeTree();
	void ExpandNewTree();
	void RefreshNewTree();

	// Member vars
	StringBase*		m_pstrDetails;
	static BOOL		s_bShowHiddenNodes;
	static BOOL 	s_bSelectionOnly;
	static BOOL 	s_bExpandClicked;

	// Pictures for tree view
	INT32					m_idxNode;
	INT32					m_idxAttribute;
	INT32					m_idxRenderableInk;
	INT32					m_idxRenderablePaper;
	INT32					m_idxInsertionNode;
	INT32					m_idxHiddenNode;
	INT32					m_idxSentinelNode;
	INT32					m_idxUserAttr;
	INT32					m_idxSetProperty;
};

#endif
