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

#ifndef INC_CMXTREE
#define INC_CMXTREE

//#include "dialogop.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "timdlg.h"

//#include "cxfrec.h"
//#include "cxfrech.h"
//#include "cdrfiltr.h"
#include "rifffile.h"

// -----------------------------------------------------------------------------------------
// CMXTreeDlg for displaying the Camelot v2 file format
 
#define OPTOKEN_CMXTREEDLG _T("CMXTreeDlg")    

class CMXTreeDlg;
class DebugTreeInfo;
class CCFile;
//class RIFFFile;

/* class CMXFileRecordHandler : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP( CMXFileRecordHandler )
public:

	CMXFileRecordHandler() {}
	~CMXFileRecordHandler() {}
}; */

class CMXFileRecord : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP( CMXFileRecord )
public:

	CMXFileRecord() {}
	CMXFileRecord(RIFFObjectType ObjectType, FOURCC ChunkType, UINT32 ChunkSize, UINT32 ObjectLevel)
		{
			m_ObjectType = ObjectType;
			m_ChunkType = ChunkType;
			m_ChunkSize = ChunkSize;
			m_ObjectLevel = ObjectLevel;
		}

	~CMXFileRecord() {}

	RIFFObjectType	GetObjectType()		{ return m_ObjectType; }
	FOURCC			GetChunkType()		{ return m_ChunkType; }
	UINT32			GetChunkSize()		{ return m_ChunkSize; }
	UINT32			GetObjectLevel()	{ return m_ObjectLevel; }

	virtual	INT32  GetRecordNumber() { return m_RecordNumber; }
	virtual	void  SetRecordNumber(UINT32 n) { m_RecordNumber = n; }

protected:
	RIFFObjectType	m_ObjectType;
	FOURCC	m_ChunkType;
	UINT32	m_ChunkSize;
	UINT32	m_ObjectLevel;

	INT32	m_RecordNumber;
};


class CMXNode : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP( CMXNode )
public:

	CMXNode() {}
	CMXNode(CMXFileRecord* pThisRecord);
	~CMXNode();

	void SetNext(CMXNode * pRecord)		{ pNext		= pRecord; }
	void SetPrevious(CMXNode* pRecord)	{ pPrevious = pRecord; }
	void SetParent(CMXNode* pRecord)	{ pParent	= pRecord; }
	void SetChild(CMXNode* pRecord)		{ pChild	= pRecord; }

	CMXNode* GetNext()			{ return pNext;		}
	CMXNode* GetPrevious()		{ return pPrevious; }
	CMXNode* GetParent()		{ return pParent;	}
	CMXNode* GetChild()			{ return pChild;	}

	CMXFileRecord* GetCMXFileRecord() { return pRecord; }

	RIFFObjectType	GetObjectType();
	FOURCC	GetChunkType();
	UINT32	GetChunkSize();
	UINT32	GetObjectLevel();

	//UINT32 GetTag();
	//UINT32 GetSize();
	void  ResetReadPos();

	BOOL HasChildren()			 { return pChild != NULL; }
	BOOL ShowChildren()			 { return DoShowChildren; }
	void SetShowChildren(BOOL b) { DoShowChildren = b; }

private:

	CMXFileRecord* pRecord;

	CMXNode* pNext;
	CMXNode* pPrevious;
	CMXNode* pParent;
	CMXNode* pChild;

	BOOL DoShowChildren;
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CMXTreeDlgRecordHandler : public CC_CLASS_MEMDUMP //CMXFileRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_MEMDUMP( CMXTreeDlgRecordHandler )
	//CC_DECLARE_DYNAMIC(CMXTreeDlgRecordHandler);

public:
	CMXTreeDlgRecordHandler(CMXTreeDlg* pDlg, CCFile* pCMXile);
	~CMXTreeDlgRecordHandler() {}

	virtual UINT32*	GetTagList() { return NULL; }
	virtual BOOL	HandleRecord(CMXFileRecord* pCMXFileRecord);
	virtual void	IncProgressBarCount(UINT32 n) {}

	virtual void	GetTagText(FOURCC ChunkType, String_256& Str);
	virtual void	GetRecordDescriptionText(CMXFileRecord* pCMXFileRecord,StringBase* Str);

private:
	CMXTreeDlg* pCMXDlg;
	CCFile* pCMXile;
};
    
//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

/********************************************************************************************

>	class CMXNodeInfo : public CC_CLASS_MEMDUMP

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Purpose:	Hold information on a node displayed in the tree dialog.
	SeeAlso:	CMXTreeDlg

********************************************************************************************/

class CMXNodeInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP( CMXNodeInfo  )
public:
	CMXNode* pNode;
};


/********************************************************************************************

>	class CMXNodeTypeStatistics : public ListItem

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Purpose:	Holds info relating to a node of a particular type (i.e. tag value)
	SeeAlso:	CMXTreeDlg

********************************************************************************************/

class CMXNodeTypeStatistics : public ListItem
{
	CC_DECLARE_MEMDUMP( CMXNodeTypeStatistics )
public:

	CMXNodeTypeStatistics(FOURCC ThisChunk) { m_ChunkType = ThisChunk; NumOccurrences = 0; TotalSize = 0; }

	void	IncNumOccurances()		{ NumOccurrences++; }
	void	AddToTotalSize(INT32 s)	{ TotalSize += s; }

	FOURCC	GetChunkType()			{ return m_ChunkType; }
	//	UINT32	GetTag()				{ return Tag; }
	INT32	GetNumOccurances()		{ return NumOccurrences; }
	INT32	GetTotalSize()			{ return TotalSize; }

private:
	FOURCC	m_ChunkType;
//	UINT32	Tag;
	INT32	NumOccurrences;
	INT32	TotalSize;
};

/********************************************************************************************

>	class CMXNodeTypeStatisticsList : public List

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Purpose:	Class that maintains statistics on the nodes added to the tree
	SeeAlso:	CMXTreeDlg

********************************************************************************************/

class CMXNodeTypeStatisticsList : public List
{
	CC_DECLARE_MEMDUMP( CMXNodeTypeStatisticsList )
public:

	void Update(CMXNode* pNode);
	void Dump(CMXTreeDlg* pDlg);

private:
	CMXNodeTypeStatistics*	FindNodeType(UINT32 Tag);
	void					Add(CMXNodeTypeStatistics* pNodeType);
	CMXNodeTypeStatistics*	GetHead();
	CMXNodeTypeStatistics*	GetNext(CMXNodeTypeStatistics* pItem);
};

//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------




//---------------------------------------------------------------------
//---------------------------------------------------------------------
//---------------------------------------------------------------------

class CMXTreeDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE( CMXTreeDlg )
public:

	CMXTreeDlg();      
	~CMXTreeDlg(); 

	MsgResult Message( Msg* Message); 
	void Do(OpDescriptor*);		// "Do" function        
	static BOOL Init();                        
	BOOL Create();
	static OpState GetState(String_256*, OpDescriptor*);

	void SetFileName(String_256& FileName);

	void SetEndOfFile(BOOL b) { EndOfFile = b; }

	enum { IDD = _R(IDD_NEWDEBUGTREE) };  

	void AddNode(CMXNode *pNode);

	static CMXTreeDlg* GetCurrentCMXTreeDlg() { return pCurrentCMXTreeDlg; }
	void ShowFile(char* pFileName);

	void GetTagText(FOURCC ChunkType,String_256& Str);

protected:
	BOOL ParseFile(CCFile * pFile);

private:
	void DeInit();
	void Delete(CMXNode* pNode);

	void DeleteTreeInfo();

	void CreateTree();
	void DisplayTree(BOOL ExpandAll);
	INT32 AddDisplayNode(CMXNode* pNode,INT32 Index,INT32 Depth,BOOL ExpandAll);
	void ShowNodeDebugInfo(INT32 ListIndex);
	void ShowNodeDebugInfoForNode(CMXNode *pNode);
	CMXNodeInfo* GetInfo(INT32 Index);
	CMXTreeDlgRecordHandler* CMXTreeDlg::FindHandler(FOURCC ChunkType);
	void GetTagText(CMXNode* pNode,String_256& Str);

	CMXNode* pRoot;
	CMXNode* pContextNode;
	BOOL AddNextAsChild;
	INT32 Level;
	INT32 MaxIndex;

	String_256 FileName;
	BOOL EndOfFile;

	DebugTreeInfo* TreeInfo;
	StringBase* EditStr;

	CMXNodeTypeStatisticsList ListOfNodeTypeStats;

	static CMXTreeDlg* pCurrentCMXTreeDlg;


	// New TreeView stuff
	HTREEITEM AddOneItem(HTREEITEM hParent, TCHAR *pText, HTREEITEM hInsAfter, INT32 iImage, CMXNode *pNode);
	HTREEITEM AddItemsToNewTreeControl(HTREEITEM hParentItem, CMXNode *pNodeToAdd);
	BOOL InitialiseNewTreeControl(void);
	INT32 AddBitmapResourceToImageList(HIMAGELIST hList, UINT32 ResID);
	INT32 GetImageForNode(CMXNode *pNode);
	BOOL GetInfoFromHTREEITEM(HTREEITEM hItem, CMXNode **pNode, INT32 *pChildren);
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

	RIFFFile * m_pRIFF;
	CMXTreeDlgRecordHandler * m_pHandler;
};    

#endif // INC_CMXTREE

