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

// The Camelot debug tree dialog.


#include "camtypes.h"

#include "dbugtree.h"
#include "node.h"
#include "document.h"
//#include "simon.h"
#include "oildbug.h"
#include "nodedoc.h"
#include "app.h"
#include "ccobject.h"
#include "nodetext.h"
#include "userattr.h"
#include "ngsentry.h"

#include "clipint.h"
#include "keypress.h"
#include "vkextra.h"

#include "insertnd.h"
#include "bubbleid.h"

#if DEBUG_TREE

CC_IMPLEMENT_DYNCREATE(DebugTreeDlg, DialogOp)   

BOOL DebugTreeDlg::s_bShowHiddenNodes = TRUE;
BOOL DebugTreeDlg::s_bSelectionOnly = FALSE;
BOOL DebugTreeDlg::s_bExpandClicked = FALSE;

/********************************************************************************************

>	DebugTreeDlg::DebugTreeDlg()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/94
	Purpose:	Create a debug tree dialog operation. Sets up the string used to hold
				node details.

********************************************************************************************/

DebugTreeDlg::DebugTreeDlg()
  : DialogOp(DebugTreeDlg::IDD(), MODELESS)
{      
	m_pstrDetails = new StringBase;
	if (!m_pstrDetails || !m_pstrDetails->Alloc(4096)) return;	// 4K is plenty for any node's details
	s_bExpandClicked = FALSE;
}     


/********************************************************************************************

>	DebugTreeDlg::~DebugTreeDlg()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/94
	Purpose:	Cleans up a debug tree dialog operation.

********************************************************************************************/

DebugTreeDlg::~DebugTreeDlg()     
{
	// Deallocate the node details string.
	delete m_pstrDetails;         
}



/********************************************************************************************

>	MsgResult DebugTreeDlg::Message( Msg* Message)                               

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/94
	Inputs:		Msg - the message to handle.
	Returns:	-
	Purpose:	Handles messages for this dialog.

********************************************************************************************/

MsgResult DebugTreeDlg::Message( Msg* Message)
{

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*) Message;

		if ((Msg->DlgMsg == DIM_COMMIT || Msg->DlgMsg == DIM_CANCEL)) 
		{
			DeInit();
			Close(); // Hide the dialog   
			End(); 
		}          
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_EXPAND))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Expand the tree  
			// Used by refresh
			s_bExpandClicked = TRUE;
			ExpandNewTree();
		}    
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_DUMP))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Dump the tree
			DumpTreeTree();
		}    
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_REFRESH))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Refresh the tree
			Refresh(); 
		}    
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_HIDDEN))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{
			// Update the 'show hidden nodes' flag
			BOOL Valid;
			INT32 Value = GetLongGadgetValue(_R(IDC_DEBUGTREE_HIDDEN), 0, 1, 0, &Valid);

			if (Valid)
				s_bShowHiddenNodes = (Value == 1);

			// Refresh the new tree control
			RefreshNewTree();
		}
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_SELNONLY))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Update the 'show selection' flag
			BOOL Valid;
			INT32 Value = GetLongGadgetValue(_R(IDC_DEBUGTREE_SELNONLY), 0, 1, 0, &Valid);

			if (Valid)
				s_bSelectionOnly = (Value == 1);

			// Refresh the new tree control
			RefreshNewTree();
		}    
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_TREE)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
			// Get the node out of the item
			CTreeItemID hItem = GetTreeGadgetFirstSelectedItem(_R(IDC_DEBUGTREE_TREE));
			if (hItem.IsOk())
			{
				CCObject* pNode = GetTreeGadgetItemData(_R(IDC_DEBUGTREE_TREE), hItem);
				if (pNode)
					ShowNodeDebugInfoForNode((Node*)pNode);
			}
   		}

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
	return OK; 
}



/********************************************************************************************

>	void DebugTreeDlg::Do(OpDescriptor*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		Ignored
	Purpose:	The 'Do' function for the debug tree dialog. Just creates and opens the
				dialog.

********************************************************************************************/

void DebugTreeDlg::Do(OpDescriptor*)		// "Do" function        
{  
	Create();
	Open();
}



/********************************************************************************************

>	BOOL DebugTreeDlg::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	TRUE if intialised ok;
				FALSE if not.
	Purpose:	Initialise the preferences, and register the OpDescriptor for this dialog.

********************************************************************************************/

BOOL DebugTreeDlg::Init()                     
{
	// Register our preferences
	Camelot.DeclareSection(_T("DebugFlags"), 2);
	Camelot.DeclarePref(_T("DebugFlags"), _T("ShowHiddenNodes"),	&s_bShowHiddenNodes);
	Camelot.DeclarePref(_T("DebugFlags"), _T("SelectionOnly"),		&s_bSelectionOnly);

	// Register our OpDescriptor
	return (RegisterOpDescriptor(
	 							 0,
	 							 _R(IDS_DEBUGTREEDLG), 
								 CC_RUNTIME_CLASS(DebugTreeDlg),
	 							 OPTOKEN_DEBUGTREEDLG,
	 							 DebugTreeDlg::GetState,
	 							 0,	/* help ID */
	 							 _R(IDBBL_DEBUGTREEDLG),
	 							 0	/* bitmap ID */));
}


/********************************************************************************************

>	BOOL DebugTreeDlg::Create()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Returns:	TRUE if setup ok;
				FALSE if not.
	Purpose:	Create and setup the debug tree dialog.

********************************************************************************************/

BOOL DebugTreeDlg::Create()
{    
	if (DialogOp::Create())
	{
		Initialise();  
	}     
	return TRUE; 
}



/********************************************************************************************

>	OpState DebugTreeDlg::GetState(String_256*, OpDescriptor*)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Purpose:	The usual.

********************************************************************************************/

OpState DebugTreeDlg::GetState(String_256*, OpDescriptor*)
{  
	OpState OpSt;
	return(OpSt);
}	

/********************************************************************************************

>	INT32 DebugTreeDlg::GetTreeDepth(Node *pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/94
	Inputs:		pNode - the node to find out the depth of.
	Returns:	Depth of the node in range 0...n
	Purpose:	Find out the depth of a node in the document tree; the document node at the
				top of the tree has a depth of 0, its children are at depth 1, and so on.

********************************************************************************************/

INT32 DebugTreeDlg::GetTreeDepth(Node *pNode)
{
	INT32 Depth = 0;

	do
	{
		pNode = pNode->FindParent();
		Depth++;
	} while (pNode != NULL);

	return Depth - 1;
}

/********************************************************************************************

>	void DebugTreeDlg::ShowNodeDebugInfo(INT32 ListIndex)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/94
	Inputs:		ListIndex - the list item to display information on.
	Purpose:	Display the debug information for the given node in the debug tree dialog's
				scrolly window.

********************************************************************************************/

void DebugTreeDlg::ShowNodeDebugInfoForNode(Node *pNode)
{  
	// Get the Node's class name.
	String_256 NodeDesc = String_256( (TCHAR *)( pNode->GetRuntimeClass()->GetClassName() ) ); 

	// Fill in the string.
	m_pstrDetails->Empty();
	*m_pstrDetails += NodeDesc;
	TCHAR NodeAddress[64];
	camSnprintf( NodeAddress, 64, _T(" Data at 0x%p (%lu bytes)\n\n"), pNode, pNode->GetNodeSize() );
	*m_pstrDetails += NodeAddress;
	pNode->GetDebugDetails(m_pstrDetails);

	// Copy this string into the edit control.
	SetStringGadgetValue(_R(IDC_DEBUGTREE_DETAILS), *m_pstrDetails); 
}  

/********************************************************************************************

>	void DebugTreeDlg::Refresh()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/97
	Purpose:	Refreshes the tree.

********************************************************************************************/

void DebugTreeDlg::Refresh()
{
	// Vape the old tree
	DeleteAllValues(_R(IDC_DEBUGTREE_TREE));

	// Create a new tree
	GetTree();

	// Select the first item in the tree and display its debug info.
	CTreeItemID hItem = GetTreeGadgetRootItem(_R(IDC_DEBUGTREE_TREE));
	SelectTreeGadgetItem(_R(IDC_DEBUGTREE_TREE), hItem);
	CCObject* pNode = GetTreeGadgetItemData(_R(IDC_DEBUGTREE_TREE), hItem);
	if (pNode && pNode->IsKindOf(CC_RUNTIME_CLASS(Node) ))
		ShowNodeDebugInfoForNode((Node*)pNode);

	// Expand the tree ?
	if (s_bExpandClicked)
		ExpandNewTree();
} 


// The depth in the tree of selected nodes.
const INT32 SelectionDepth = 4;


/********************************************************************************************
>	static void DebugTreeDlg::TweeDump(BaseDocument *DocToDump = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Inputs:		DocToDump - the document that should be dumped out.
				If NULL then the selected document is dumped.
	Purpose:	Writes a textual description of the tree to the debug output.
	SeeAlso:	DebugTreeDlg::DumpTree
********************************************************************************************/

void DebugTreeDlg::TweeDump(BaseDocument* DocToDump)
{
	TCHAR				StrBuff[256];
	String_256			Details;

	if (DocToDump == NULL)
		DocToDump = Document::GetSelected();
	if (DocToDump==NULL)
	{
		DebugTreeInfo::OutputDebug( _T("TweeDump: No Selected Document!\n") );
		return;
	}
	else
	{
		camSnprintf( StrBuff, 256, _T("Tweedump: Document %p\n"), DocToDump);
		DebugTreeInfo::OutputDebug((TCHAR*) StrBuff);
	}

	Node* pNode = DocToDump->GetFirstNode();
	INT32 depth = 0;
	INT32 i;
	while (pNode)
	{
		// Fill in the details string for this node...
		Details.Empty();
		for(i=0; i<depth; i++)
		{
			camSnprintf( StrBuff, 256, _T("  ") );
			Details += StrBuff;
		}
		camSnprintf( StrBuff, 256, _T("%p  "), pNode);
		Details += StrBuff;
		camSnprintf( StrBuff, 256, _T("%c%c%c  "),	
										(pNode->IsRenderable() ? 'R':'-'),
										(pNode->IsSelected() ? 'S':'-'),
										(pNode->IsParentOfSelected() ? 'P':'-')
				);
		Details += StrBuff;
		Details += String_256((TCHAR*)(pNode->GetRuntimeClass()->GetClassName())); 
		camSnprintf( StrBuff, 256, _T("\n"), pNode);
		Details += StrBuff;
		DebugTreeInfo::OutputDebug((TCHAR*)Details);

		// Find any children we may have and show them...
		if (pNode->FindFirstChild())
		{
			depth+=1;
			pNode = pNode->FindFirstChild();
		}
		else
		{
			// Now find the next subtree...
			while ( pNode && pNode->FindNext()==NULL)
			{
				pNode = pNode->FindParent();
				depth-=1;
			}
			if (pNode)
				pNode = pNode->FindNext();
		}
	}

	DebugTreeInfo::OutputDebug( _T("\n") );
}


/********************************************************************************************
>	static void DebugTreeDlg::DumpSubTree(Node* pSubTree, INT32 FromParent=0, INT32 MaxDepth=9999)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com> (tweaked version of Phil's amusingly named 'TweeDump')
	Created:	7/2/96
	Inputs:		pSubTree   - pointer to subtree to dump
				FromParent - levels to ascend before dumping sub tree
				MaxDepth   - depth to which to descend
********************************************************************************************/

void DebugTreeDlg::DumpSubTree(Node* pSubTree, INT32 FromParent, INT32 MaxDepth)
{
	if (FromParent<0)
	{
		DebugTreeInfo::OutputDebug( _T("DebugTreeDlg::DumpSubTree() - FromParent<0!\n") );
		return;
	}
	if (MaxDepth<0)
	{
		DebugTreeInfo::OutputDebug( _T("DebugTreeDlg::DumpSubTree() - MaxDepth<0!\n") );
		return;
	}

	Node* pStartNode = pSubTree;
	while (FromParent--!=0 && pStartNode!=NULL)
		pStartNode = pStartNode->FindParent();

	if (pStartNode==NULL)
	{
		DebugTreeInfo::OutputDebug( _T("DebugTreeDlg::DumpSubTree() - pSubTree==NULL!\n") );
		return;
	}

	TCHAR				StrBuff[256];
	camSnprintf( StrBuff, 256, _T("DebugTreeDlg::DumpSubTree() - pStartNode = %p\n"), pStartNode );
	DebugTreeInfo::OutputDebug((TCHAR*)StrBuff);

	INT32        depth = 0;
	String_256 Details;
	Node* pNode = pStartNode;
	while (pNode && depth>=0)
	{
		// Fill in the details string for this node...
		camSnprintf( StrBuff, 256, _T("  ") );
		Details.Empty();
		for(INT32 i=0; i<depth; i++)
			Details += StrBuff;
		camSnprintf( StrBuff, 256, _T("%p  "), pNode);
		Details += StrBuff;
		camSnprintf( StrBuff, 256, _T("%c%c%c  "),
									(pNode->IsRenderable()       ? 'R':'-'),
									(pNode->IsSelected()         ? 'S':'-'),
									(pNode->IsParentOfSelected() ? 'P':'-') );
		Details += StrBuff;
		Details += String_256((TCHAR*)(pNode->GetRuntimeClass()->GetClassName()));
		if (IS_A(pNode,TextChar))
		{
			Details += String_8( _T(" '") );
			TCHAR ptch[2] =  { (TCHAR)(((TextChar*)pNode)->GetUnicodeValue()), '\0' };
			Details += String_8((TCHAR*)ptch);
			Details += String_8( _T("'") );
		}
		Details += String_8( _T("\n") );
		DebugTreeInfo::OutputDebug((TCHAR*)Details);

		// Find any children we may have and show them...
		if (pNode->FindFirstChild() && depth<MaxDepth)
		{
			depth += 1;
			pNode  = pNode->FindFirstChild();
		}
		else
		{
			// Now find the next subtree...
			while ( pNode && pNode->FindNext()==NULL)
			{
				pNode = pNode->FindParent();
				depth-=1;
			}
			if (pNode==pStartNode)
				pNode = NULL;	// end
			if (pNode)
				pNode = pNode->FindNext();
		}
	}

	DebugTreeInfo::OutputDebug( _T("\n") );
}


/********************************************************************************************

>	BOOL DebugTreeDlg::ShouldBeHidden(Node *pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		pNode - the node to check.
	Returns:	TRUE if the Node should be hidden; 
				FALSE if it should be displayed.
	Purpose:	Determine whether or not a given node should be displayed or not,
				according to the current preferences.
	SeeAlso:	DebugTreeDlg::ShowHidden

********************************************************************************************/

BOOL DebugTreeDlg::ShouldBeHidden(Node *pNode)
{
	if (!s_bShowHiddenNodes && pNode->IsKindOf(CC_RUNTIME_CLASS(NodeHidden)))
		// This is a hidden node and the user does not want hidden nodes.
		return TRUE;

	if (s_bSelectionOnly && 
		(GetTreeDepth(pNode) == SelectionDepth) &&
		!pNode->IsSelected())
	{
		// This is an unselected node and the user only wants to see the selection
		return TRUE;
	}

	// Otherwise, we want to see this node
	return FALSE;
}


/********************************************************************************************

>	BOOL DebugTreeDlg::IsChildOfHiddenNode(Node *pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		pNode - the node to check.
	Returns:	TRUE if the node is the child of a hidden node;
				FALSE if not.
	Purpose:	Determine if a given node is 'hidden' (in the Camelot sense), i.e. it is the 
				child of a hidden node.

********************************************************************************************/

BOOL DebugTreeDlg::IsChildOfHiddenNode(Node *pNode)
{
	if (pNode->FindParent() != NULL)
		// Can't be the child of a hidden node
		return FALSE;

	// It's either a Hidden node's child, or a Document node...
	return (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)));
}


/********************************************************************************************

>	BOOL DebugTreeDlg::HasChildren(Node *pNode)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/06/94
	Inputs:		pNode - the node to check.
	Returns:	TRUE if the node has displayable children;
				FALSE if not.
	Purpose:	Determine if a node has any children that can be displayed in the tree
				(irrespective of user preferences).  Any node that has a normal child
				can be displayed, unless it is the child of a Hidden node.
				Hidden nodes also have displayable children, via their 'HiddenNd' field.
	SeeAlso:	DebugTreeDlg::IsChildOfHiddenNode

********************************************************************************************/

BOOL DebugTreeDlg::HasChildren(Node *pNode)
{
	if (pNode->FindFirstChild() != NULL)
	{
		// It has children, but we only display them if this node is not hanging
		// off a NodeHidden.
		return (IsChildOfHiddenNode(pNode) == FALSE);
	}

	if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeHidden)))
		// NodeHiddens have a pseudo-child
		return TRUE;

	// Otherwise, no children
	return FALSE;
}

/********************************************************************************************

>	void DebugTreeDlg::GetTree()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/94
	Purpose:	Initialises the tree display to contain the top level of the tree.
	SeeAlso:	DebugTreeDlg::AddNodesChildren

********************************************************************************************/

void DebugTreeDlg::GetTree()  
{
	// Add the main document node.
	Node *pNodeDoc = Document::GetSelected()->GetFirstNode()->FindNext();

	if (KeyPress::IsKeyPressed(CAMKEY(C)))
		pNodeDoc = InternalClipboard::Instance()->GetFirstNode()->FindNext();

	ENSURE(pNodeDoc->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)),
		   "Bad document node in tree");

	AddItemToNewTreeControl( CTreeItemID(), CTreeItemID(), pNodeDoc );
} 


/********************************************************************************************

>	void DebugTreeDlg::Initialise()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/94
	Purpose:	Set up the initial debug tree display
	SeeAlso:	DebugTreeDlg::GetTree

********************************************************************************************/

void DebugTreeDlg::Initialise()
{
	// Got to do this before adding items to the new tree control
	InitialiseNewTreeControl();

	GetTree();

	// Select the first item in the tree and display its debug info.
	CTreeItemID hItem = GetTreeGadgetRootItem(_R(IDC_DEBUGTREE_TREE));
	SelectTreeGadgetItem(_R(IDC_DEBUGTREE_TREE), hItem);
	CCObject* pNode = GetTreeGadgetItemData(_R(IDC_DEBUGTREE_TREE), hItem);
	if (pNode && pNode->IsKindOf(CC_RUNTIME_CLASS(Node) ))
		ShowNodeDebugInfoForNode((Node*)pNode);

	// Set the check boxes according to preferences
	if (s_bShowHiddenNodes)
		SetLongGadgetValue(_R(IDC_DEBUGTREE_HIDDEN), 1);
	else
		SetLongGadgetValue(_R(IDC_DEBUGTREE_HIDDEN), 0);

	if (s_bSelectionOnly)
		SetLongGadgetValue(_R(IDC_DEBUGTREE_SELNONLY), 1);
	else
		SetLongGadgetValue(_R(IDC_DEBUGTREE_SELNONLY), 0);
}  


BOOL DebugTreeDlg::IsAHiddenNode(Node *pNode)
{
	return (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeHidden));
}


void DebugTreeDlg::DeInit()
{
	DeleteAllValues(_R(IDC_DEBUGTREE_TREE));
} 




/********************************************************************************************

>	CTreeItemID DebugTreeDlg::AddOneItem(CTreeItemID hParent,
										 const StringBase& str,
										 CTreeItemID hInsAfter,
										 INT32 iImage,
										 Node *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	This function fills out the TV_ITEM and TV_INSERTSTRUCT structures 
				and adds the item to the tree view control.

********************************************************************************************/

CTreeItemID DebugTreeDlg::AddOneItem(CTreeItemID hParent,
									 const StringBase& str,
									 CTreeItemID hInsAfter,
									 INT32 iImage,
									 Node* pNode)
{
	return SetTreeGadgetItem(_R(IDC_DEBUGTREE_TREE), hParent, str, hInsAfter, iImage, pNode);
}


/********************************************************************************************

>	INT32 DebugTreeDlg::GetImageForNode(Node *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Ah well... This should really be up to the nodes themselves... This has
				been left as an exercise to the reader...

********************************************************************************************/

INT32 DebugTreeDlg::GetImageForNode(Node *pNode)
{
	INT32 iImage = m_idxNode;

	if (pNode->IS_KIND_OF(AttrUser))
		iImage = m_idxUserAttr;
	else if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeHidden)))
		iImage = m_idxHiddenNode;
	else if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)))
		iImage = m_idxAttribute;
	else if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableInk)))
		iImage = m_idxRenderableInk;
	else if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderablePaper)))
		iImage = m_idxRenderablePaper;
	else if (pNode->IsKindOf(CC_RUNTIME_CLASS(InsertionNode)))
		iImage = m_idxInsertionNode;
	else if (pNode->IS_KIND_OF(NodeSetSentinel))
		iImage = m_idxSentinelNode;
	else if (pNode->IS_KIND_OF(NodeSetProperty))
		iImage = m_idxSetProperty;

	return iImage;
}

/********************************************************************************************

>	CTreeItemID DebugTreeDlg::AddItemToNewTreeControl(CTreeItemID hParentItem, CTreeItemID hInsAfterItem, Node *pNode, BOOL bAddChildren = TRUE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Recursively add new items to the tree control from given root node

********************************************************************************************/

CTreeItemID DebugTreeDlg::AddItemToNewTreeControl(CTreeItemID hParentItem, CTreeItemID hInsAfterItem, Node *pNode, BOOL bAddChildren)
{
	String_256 strNodeText( (TCHAR*)( pNode->GetRuntimeClass()->GetClassName() ) );
	INT32 iImage = GetImageForNode(pNode);

	CTreeItemID hThisItem = AddOneItem(hParentItem, strNodeText, hInsAfterItem, iImage, pNode);

	if (bAddChildren && HasChildren(pNode))
	{
		// Check to see if this node has any children
		Node *pChild;
		CTreeItemID hInsContext;

		// Get the first node
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeHidden))
		{
			pChild = ((NodeHidden*) pNode)->HiddenNd;

			// Add to the tree control but don't add children
			hInsContext = AddItemToNewTreeControl(hThisItem, hInsContext, pChild, FALSE);
		}
		else
		{
			pChild = pNode->FindFirstChild();

			while (pChild != NULL)
			{
				// Don't show certain kinds of nodes if the user doesn't want them
				if (!ShouldBeHidden(pChild))
				{
					// Add to the tree control
					hInsContext = AddItemToNewTreeControl(hThisItem, hInsContext, pChild);
				}

				pChild = pChild->FindNext();
			}
		}
	}

	return hThisItem;
}


/********************************************************************************************

>	BOOL DebugTreeDlg::InitialiseNewTreeControl(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Sets up the ImageList for the tree control

********************************************************************************************/

BOOL DebugTreeDlg::InitialiseNewTreeControl(void)
{
	// Load the bitmaps and add them to the image list.
	CGadgetImageList NewImageList(16, 16);
	m_idxNode 				= NewImageList.Add(_R(IDB_DEBUGTREE_NODE));
	m_idxAttribute  		= NewImageList.Add(_R(IDB_DEBUGTREE_ATTRIBUTE));
	m_idxRenderableInk 		= NewImageList.Add(_R(IDB_DEBUGTREE_RENDERABLE));
	m_idxRenderablePaper 	= NewImageList.Add(_R(IDB_DEBUGTREE_PAPER));
	m_idxInsertionNode 		= NewImageList.Add(_R(IDB_DEBUGTREE_INSERT));
	m_idxHiddenNode 		= NewImageList.Add(_R(IDB_DEBUGTREE_HIDDEN));
	m_idxSentinelNode 		= NewImageList.Add(_R(IDB_DEBUGTREE_SENTINEL));
	m_idxUserAttr 			= NewImageList.Add(_R(IDB_DEBUGTREE_USER));
	m_idxSetProperty 		= NewImageList.Add(_R(IDB_DEBUGTREE_PROPERTY));

	SetGadgetBitmaps(_R(IDC_DEBUGTREE_TREE), NewImageList);

PORTNOTE("other", "Removed use of GetGadgetImageCount")
//	INT32 Count = GetGadgetImageCount(_R(IDC_DEBUGTREE_TREE));
//
//	return (Count>0);
	return TRUE;
}

/********************************************************************************************

>	void DebugTreeDlg::DumpTreeTree()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	This does the equivalent of Tim's DumpTree function - dumps the tree view
				to the TRACE output - except it does it for	the tree control view. Tree
				controls were definitely not designed with this in mind...

********************************************************************************************/

void DebugTreeDlg::DumpTreeTree()
{
PORTNOTETRACE("other", "Removed body of DumpTreeTree because it does nothing!");
/*
	CTreeItemID			hItem;
	Node*			    pNode = NULL;
	RECT				RootRect;
	BOOL				RootRectOK = FALSE;
	INT32				Children = 0;
	INT32				Indent = 0;
	String_256			ItemText;
	
	hItem = GetTreeGadgetRootItem(_R(IDC_DEBUGTREE_TREE));
	BOOL ok = GetInfoFromTreeItemId((hItem, &pNode, &Children, &Indent);

	// Loop through the items in the tree control, dumping each one to the trace output
	while(ok && hItem != NULL)
	{
		ItemText = TEXT("");

		// Suss the indentation level
		// This might need altering if bad indentation levels result - depends on your
		// desktop's font size...
		INT32 i = 0;
		for (i=0; i < Indent; i++)
		{
			ItemText += TEXT("  ");
		}
	
		// Add '+' to show children
		if (Children > 0)
			ItemText += TEXT("+ ");
		else
			ItemText += TEXT("  ");
			
		// Display the node name
		ItemText += (((TCHAR*)(pNode->GetRuntimeClass()->m_lpszClassName)));

		// Display the node address in brackets after the name...
		TCHAR AddrStr[32];
		_stprintf(AddrStr, " (%p)\n", pNode);
		ItemText += AddrStr;

		// Dump Item to debug output (this is a bit OILy...oops!)
//		TreeInfo->OutputDebugLine((TCHAR *) ItemText);

		// Get the next item
		hItem = GetTreeGadgetNextVisItem(_R(IDC_DEBUGTREE_TREE), hItem);
		ok = GetInfoFromTreeItem(hItem, &pNode, &Children, &Indent);
	}
*/
} 

/********************************************************************************************

>	BOOL DebugTreeDlg::GetInfoFromTreeItem(CNativeTreeItem hItem, Node **ppNode, INT32 *pChildren)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Obtains the Node pointer and children flag for the given CNativeTreeItem.

********************************************************************************************/

BOOL DebugTreeDlg::GetInfoFromTreeItem(CTreeItemID hItem, Node** ppNode, size_t* pChildren)
{
	CCObject* pNode = GetTreeGadgetItemData(_R(IDC_DEBUGTREE_TREE), hItem);
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(Node)), "DebugTreeDlg found a non-node tree item");
	if (!pNode->IsKindOf(CC_RUNTIME_CLASS(Node)))
		return FALSE;

	*ppNode		= (Node*)pNode;
	*pChildren	= GetTreeGadgetChildrenCount(_R(IDC_DEBUGTREE_TREE), hItem, FALSE);

	return TRUE;
}


/********************************************************************************************

>	void DebugTreeDlg::ExpandNewTree()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/97
	Purpose:	Expands the tree display so that the whole tree is displayed in the control.

********************************************************************************************/

void DebugTreeDlg::ExpandNewTree()
{
	CTreeItemID		hItem = GetTreeGadgetRootItem(_R(IDC_DEBUGTREE_TREE));
	CTreeItemID		hSelectedItem = GetTreeGadgetFirstSelectedItem(_R(IDC_DEBUGTREE_TREE));

	// Loop through the items in the tree control
	while (hItem.IsOk())
	{
		TreeGadgetExpandItem(_R(IDC_DEBUGTREE_TREE), hItem);
		CTreeItemID hChild = GetTreeGadgetFirstChildItem(_R(IDC_DEBUGTREE_TREE), hItem);
		if (hChild.IsOk())
			hItem = hChild;

		// Get the next item
		hItem = GetTreeGadgetNextVisItem(_R(IDC_DEBUGTREE_TREE), hItem);
	}

	SelectTreeGadgetItem(_R(IDC_DEBUGTREE_TREE), hSelectedItem, TRUE );
} 



/********************************************************************************************

>	void DebugTreeDlg::RefreshNewTree()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/97
	Purpose:	Kills and recreates the newtree... Bit nasty this...

********************************************************************************************/

void DebugTreeDlg::RefreshNewTree()
{
	// Vape the new tree
	DeleteAllValues(_R(IDC_DEBUGTREE_TREE));

	// Add the main document node.
	Node* pNodeDoc = Document::GetSelected()->GetFirstNode()->FindNext();

	if (KeyPress::IsKeyPressed(CAMKEY(C)))
		pNodeDoc = InternalClipboard::Instance()->GetFirstNode()->FindNext();

	ENSURE(pNodeDoc->IsKindOf(CC_RUNTIME_CLASS(NodeDocument)),
		   "Bad document node in tree");

	AddItemToNewTreeControl( CTreeItemID(), CTreeItemID(), pNodeDoc );

	// Expand the tree ?
	if (s_bExpandClicked)
		ExpandNewTree(); 
}

#endif //DEBUG_TREE


