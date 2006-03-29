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

#ifndef INC_CXFTREE
#define INC_CXFTREE

//#include "dialogop.h"
#include "list.h"
#include "listitem.h"
//#include "timdlg.h"

#include "cxfrec.h"
#include "cxfrech.h"

// -----------------------------------------------------------------------------------------
// CXFTreeDlg for displaying the Camelot v2 file format
 
#define OPTOKEN_CXFTREEDLG _T("CXFTreeDlg")    

class CXFTreeDlg;
class DebugTreeInfo;
class CXaraFileRecordHandler;
class CXaraFile;

class CXFNode : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP( CXFNode )
public:

	CXFNode() {}
	CXFNode(CXaraFileRecord* pThisRecord);
	~CXFNode();

	void SetNext(CXFNode * pRecord)		{ pNext		= pRecord; }
	void SetPrevious(CXFNode* pRecord)	{ pPrevious = pRecord; }
	void SetParent(CXFNode* pRecord)	{ pParent	= pRecord; }
	void SetChild(CXFNode* pRecord)		{ pChild	= pRecord; }

	CXFNode* GetNext()			{ return pNext;		}
	CXFNode* GetPrevious()		{ return pPrevious; }
	CXFNode* GetParent()		{ return pParent;	}
	CXFNode* GetChild()			{ return pChild;	}

	CXaraFileRecord* GetCXaraFileRecord() { return pRecord; }

	UINT32 GetTag();
	UINT32 GetSize();
	void  ResetReadPos();

	BOOL HasChildren()			 { return pChild != NULL; }
	BOOL ShowChildren()			 { return DoShowChildren; }
	void SetShowChildren(BOOL b) { DoShowChildren = b; }

private:

	CXaraFileRecord* pRecord;

	CXFNode* pNext;
	CXFNode* pPrevious;
	CXFNode* pParent;
	CXFNode* pChild;

	BOOL DoShowChildren;
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CXFTreeDlgRecordHandler : public CXaraFileRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(CXFTreeDlgRecordHandler);

public:
	CXFTreeDlgRecordHandler(CXFTreeDlg* pDlg,CXaraFile* pCXFile);
	~CXFTreeDlgRecordHandler() {}

	virtual UINT32*	GetTagList() { return NULL; }
	virtual BOOL	HandleRecord(CXaraFileRecord* pCXaraFileRecord);
	virtual BOOL	HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber);
	virtual void	IncProgressBarCount(UINT32 n) {}
	virtual BOOL	IsStreamed(UINT32 Tag);

private:
	CXFTreeDlg* pCXFDlg;
	CXaraFile* pCXFile;
};
    
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

/********************************************************************************************

>	class CXFNodeInfo : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/96
	Purpose:	Hold information on a node displayed in the tree dialog.
	SeeAlso:	CXFTreeDlg

********************************************************************************************/

class CXFNodeInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP( CXFNodeInfo  )
public:
	CXFNode* pNode;
};


/********************************************************************************************

>	class CXFNodeTypeStatistics : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/96
	Purpose:	Holds info relating to a node of a particular type (i.e. tag value)
	SeeAlso:	CXFTreeDlg

********************************************************************************************/

class CXFNodeTypeStatistics : public ListItem
{
	CC_DECLARE_MEMDUMP( CXFNodeTypeStatistics )
public:

	CXFNodeTypeStatistics(UINT32 ThisTag) { Tag = ThisTag; NumOccurrences = 0; TotalSize = 0; }

	void	IncNumOccurances()		{ NumOccurrences++; }
	void	AddToTotalSize(INT32 s)	{ TotalSize += s; }

	UINT32	GetTag()				{ return Tag; }
	INT32	GetNumOccurances()		{ return NumOccurrences; }
	INT32	GetTotalSize()			{ return TotalSize; }

private:
	UINT32	Tag;
	INT32	NumOccurrences;
	INT32	TotalSize;
};

/********************************************************************************************

>	class CXFNodeTypeStatisticsList : public List

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/96
	Purpose:	Class that maintains statistics on the nodes added to the tree
	SeeAlso:	CXFTreeDlg

********************************************************************************************/

class CXFNodeTypeStatisticsList : public List
{
	CC_DECLARE_MEMDUMP( CXFNodeTypeStatisticsList )
public:

	void Update(CXFNode* pNode);
	void Dump(CXFTreeDlg* pDlg);

private:
	CXFNodeTypeStatistics*	FindNodeType(UINT32 Tag);
	void					Add(CXFNodeTypeStatistics* pNodeType);
	CXFNodeTypeStatistics*	GetHead();
	CXFNodeTypeStatistics*	GetNext(CXFNodeTypeStatistics* pItem);
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------




//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CXFTreeDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE( CXFTreeDlg )
public:

	CXFTreeDlg();      
	~CXFTreeDlg(); 
	MsgResult Message( Msg* Message); 
	void Do(OpDescriptor*);		// "Do" function        
	static BOOL Init();                        
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	void SetFileName(String_256& FileName);

	void SetEndOfFile(BOOL b) { EndOfFile = b; }

	enum { IDD = _R(IDD_NEWDEBUGTREE) };  

	void AddNode(CXFNode *pNode);

	static CXFTreeDlg* GetCurrentCXFTreeDlg() { return pCurrentCXFTreeDlg; }
	void ShowFile(char* pFileName);

	void GetTagText(UINT32 Tag,String_256& Str);

private:
	void DeInit();
	void Delete(CXFNode* pNode);

	void DeleteTreeInfo();

	void CreateTree();
	void DisplayTree(BOOL ExpandAll);
	INT32 AddDisplayNode(CXFNode* pNode,INT32 Index,INT32 Depth,BOOL ExpandAll);
	void ShowNodeDebugInfo(INT32 ListIndex);
	void ShowNodeDebugInfoForNode(CXFNode *pNode);
	CXFNodeInfo* GetInfo(INT32 Index);
	CXaraFileRecordHandler* CXFTreeDlg::FindHandler(UINT32 Tag);
	void GetTagText(CXFNode* pNode,String_256& Str);

	CXFNode* pRoot;
	CXFNode* pContextNode;
	BOOL AddNextAsChild;
	INT32 Level;
	INT32 MaxIndex;

	String_256 FileName;
	BOOL EndOfFile;

	DebugTreeInfo* TreeInfo;
	StringBase* EditStr;

	CXFNodeTypeStatisticsList ListOfNodeTypeStats;

	static CXFTreeDlg* pCurrentCXFTreeDlg;


	// New TreeView stuff
	HTREEITEM AddOneItem(HTREEITEM hParent, TCHAR *pText, HTREEITEM hInsAfter, INT32 iImage, CXFNode *pNode);
	HTREEITEM AddItemsToNewTreeControl(HTREEITEM hParentItem, CXFNode *pNodeToAdd);
	BOOL InitialiseNewTreeControl(void);
	INT32 AddBitmapResourceToImageList(HIMAGELIST hList, UINT32 ResID);
	INT32 GetImageForNode(CXFNode *pNode);
	BOOL GetInfoFromHTREEITEM(HTREEITEM hItem, CXFNode **pNode, INT32 *pChildren);
	void ExpandNewTree();

	// Pictures for tree view
	HIMAGELIST hNewTreeControlImageList;
	INT32 m_idxGeneralTag;
	INT32 m_idxDefineBitmap;
	INT32 m_idxDefineColour;
	INT32 m_idxCompression;
	INT32 m_idxDown;
	INT32 m_idxUp;
	INT32 m_idxGroup;
	INT32 m_idxPage;
	INT32 m_idxAttribute;
	INT32 m_idxShape;
	INT32 m_idxSentinelNode;
	INT32 m_idxSetProperty;
};    

#endif // INC_CXFTREE

