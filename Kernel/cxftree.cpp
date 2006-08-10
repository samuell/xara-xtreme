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

#include "cxftree.h"
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "simon.h"
#include "oildbug.h"
#include "nodedoc.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetext.h"
//#include <ios.h>
#include "bubbleid.h"

//#include "filtrres.h"

//#include "cxftree.h"
#include "cxfile.h"
#include "cxftfile.h"
#include "cxftags.h"
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// new tree control stuff
#include "clipint.h"
#include "keypress.h"
#include "vkextra.h"
#include "insertnd.h"

#ifdef XAR_TREE_DIALOG

CC_IMPLEMENT_DYNCREATE(CXFTreeDlg, DialogOp)
CC_IMPLEMENT_DYNAMIC(CXFTreeDlgRecordHandler,CXaraFileRecordHandler)

CC_IMPLEMENT_DYNAMIC(CXFNode, CCObject)

CC_IMPLEMENT_MEMDUMP(CXFNodeTypeStatistics,ListItem)
CC_IMPLEMENT_MEMDUMP(CXFNodeTypeStatisticsList,List)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

CXFTreeDlg* CXFTreeDlg::pCurrentCXFTreeDlg = NULL;
BOOL CXFTreeDlg::s_bExpandClicked = FALSE;

/********************************************************************************************

>	CXFTreeDlg::CXFTreeDlg()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Purpose:	Create a debug tree dialog operation. Sets up the string used to hold
				node details.

********************************************************************************************/

CXFTreeDlg::CXFTreeDlg(): DialogOp(CXFTreeDlg::IDD(), MODELESS) 
{
	pRoot			= NULL;
	pContextNode	= NULL;
	EndOfFile		= FALSE;
	AddNextAsChild	= FALSE;
	Level			= 0;
	MaxIndex		= 0;

	EditStr = new StringBase;
	if (!EditStr || !EditStr->Alloc(0x100000)) return;

	CXFTreeDlg::pCurrentCXFTreeDlg = this;
}


/********************************************************************************************

>	CXFTreeDlg::~CXFTreeDlg()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Purpose:	Cleans up a debug tree dialog operation.

********************************************************************************************/

CXFTreeDlg::~CXFTreeDlg()     
{
	Delete(pRoot);
	pRoot = NULL;

	// Deallocate the node details string.
	delete EditStr;

	CXFTreeDlg::pCurrentCXFTreeDlg = NULL;
}



/********************************************************************************************

>	void CXFTreeDlg::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		Ignored
	Purpose:	The 'Do' function for the debug tree dialog. Just creates and opens the
				dialog.

********************************************************************************************/

void CXFTreeDlg::Do(OpDescriptor*)
{
	Create();
	Open();
}



/********************************************************************************************

>	BOOL CXFTreeDlg::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Returns:	TRUE if intialised ok;
				FALSE if not.
	Purpose:	Initialise the preferences, and register the OpDescriptor for this dialog.

********************************************************************************************/

BOOL CXFTreeDlg::Init()
{
	// Register our OpDescriptor
	return (RegisterOpDescriptor(
								 0,
								 _R(IDS_CXFTREEDLG), 
								 CC_RUNTIME_CLASS(CXFTreeDlg),
								 OPTOKEN_CXFTREEDLG,
								 CXFTreeDlg::GetState,
								 0,	/* help ID */
								 _R(IDBBL_DEBUGTREEDLG),
								 0	/* bitmap ID */));
}

/********************************************************************************************

>	BOOL CXFTreeDlg::Create()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Returns:	TRUE if setup ok;
				FALSE if not.
	Purpose:	Create and setup the debug tree dialog.

********************************************************************************************/

BOOL CXFTreeDlg::Create()
{
	if (DialogOp::Create())
	{
		EnableGadget(_R(IDC_DEBUGTREE_EXPAND),  TRUE);
		EnableGadget(_R(IDC_DEBUGTREE_DUMP),    FALSE);
		EnableGadget(_R(IDC_DEBUGTREE_HIDDEN),  FALSE);
		EnableGadget(_R(IDC_DEBUGTREE_SELNONLY),FALSE);
		EnableGadget(_R(IDC_DEBUGTREE_REFRESH), FALSE);

		InitialiseNewTreeControl();
	}

	return TRUE; 
}



/********************************************************************************************

>	OpState CXFTreeDlg::GetState(String_256*, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Purpose:	The usual.

********************************************************************************************/

OpState CXFTreeDlg::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}
/********************************************************************************************

>	void CXFTreeDlg::ShowFile(TCHAR* pFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Input:		pFile = ptr to file name
	Purpose:	Set up the initial debug tree display
	SeeAlso:	CXFTreeDlg::GetTree

********************************************************************************************/

void CXFTreeDlg::ShowFile(TCHAR* pFile)
{
	String_256 s = pFile;
	SetFileName(s);

	CreateTree();
}  


void CXFTreeDlg::DeInit()
{
	DeleteAllValues(_R(IDC_DEBUGTREE_TREE));

	Delete(pRoot);
	pRoot = NULL;
} 


void CXFTreeDlg::Delete(CXFNode* pNode)
{
	if (pNode == NULL)
		return;

	while (pNode)
	{
		CXFNode* pNext = pNode->GetNext();

		CXFNode* pChild = pNode->GetChild();
		if (pChild)
		{
			Delete(pChild);
		}

		delete pNode;
		pNode = pNext;
	}
}

void CXFTreeDlg::SetFileName(String_256& ThisFileName)
{
	FileName = ThisFileName;
}


void CXFTreeDlg::CreateTree()
{
	// Delete the items from the tree control and also delete our tree
	DeleteAllValues(_R(IDC_DEBUGTREE_TREE));
	Delete(pRoot);
	pRoot = NULL;

	// Init vars before reading the file
	AddNextAsChild = FALSE;
	Level = 0;
	pContextNode = NULL;

	ListOfNodeTypeStats.DeleteAll();

	PathName* pPath = new PathName(FileName);

	if (pPath != NULL && pPath->IsValid())
	{
		CCDiskFile* pFile = new CCDiskFile(*pPath,ios::in | ios::binary);

		if (pFile != NULL)
		{
			EndOfFile = FALSE;
			CXaraFile* pCXFile = NULL;

PORTNOTE("other", "Removed wix reading - uses COM object")
#if FALSE
			char Buf[10];
			pFile->read(Buf, 10);
			// reset to the begining
			pFile->seek(0);

			if ((memcmp(Buf, "FLARETEXT", 9) == 0) &&
				((Buf[9] == 13) || (Buf[9] == 10)))
			{
				pCXFile = new CXaraTemplateFile();		// template so use new class
			}
			else
#endif
			{
				pCXFile = new CXaraFile();				// use original class
			}

			if (pCXFile != NULL)
			{
				CXaraFileRecordHandler* pHandler = new CXFTreeDlgRecordHandler(this,pCXFile);
				if (pHandler != NULL)
				{
					pCXFile->SetDefaultRecordHandler(pHandler);

					if (pCXFile->OpenToRead(pFile))
					{
						BOOL ok = TRUE;

						while (!EndOfFile && ok)
							ok = pCXFile->ReadNextRecord();

						pCXFile->Close();
					}

					delete pHandler;
					pHandler = NULL;
				}
				delete pCXFile;
			}
			delete pFile;
		}
	}

	if (pPath != NULL)
		delete pPath;

//	ListOfNodeTypeStats.Dump(this);
	ListOfNodeTypeStats.DeleteAll();

	CTreeItemID hThisItem = AddOneItem(CTreeItemID(), FileName, CTreeItemID(), m_idxPage, NULL);

	// AddItemsToNewTreeControl doesn't add siblings now so we must loop
	CTreeItemID hContext;
	CXFNode* pNode = pRoot;
	while (pNode)
	{
		hContext = AddItemToNewTreeControl(hThisItem, hContext, pNode, TRUE);
		pNode = pNode->GetNext();
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------

void CXFTreeDlg::AddNode(CXFNode* pNode)
{
	if (pNode == NULL)
		return;

	ListOfNodeTypeStats.Update(pNode);

	if (pRoot == NULL)
	{
		pRoot = pNode;
		return;
	}

	if (pContextNode == NULL)
		pContextNode = pRoot;

	if (pContextNode != NULL)
	{
		switch (pNode->GetTag())
		{
			case TAG_UP:
				AddNextAsChild = FALSE;
				Level--;
				break;

			case TAG_DOWN:
				AddNextAsChild = TRUE;
				Level++;
				break;
		}

		if (pContextNode != NULL)
		{
			if (AddNextAsChild)
			{
				CXFNode* pChildNode = pContextNode->GetChild();
				if (pChildNode != NULL)
				{
					TRACEUSER( "Markn", _T("Take cover - context node has already got children"));

					while (pChildNode->GetNext() != NULL)
						pChildNode = pChildNode->GetNext();

					pChildNode->SetNext(pNode);
					pNode->SetPrevious(pChildNode);
					pNode->SetParent(pChildNode->GetParent());
				}
				else
				{
					pContextNode->SetChild(pNode);
					pNode->SetParent(pContextNode);
				}

				AddNextAsChild = FALSE;
				pContextNode = pNode;
			}
			else
			{
				pContextNode->SetNext(pNode);
				pNode->SetPrevious(pContextNode);
				pNode->SetParent(pContextNode->GetParent());

				if (pNode->GetTag() == TAG_UP)
					pContextNode = pNode->GetParent();
				else
					pContextNode = pNode;
			}
		}
	}
}

/********************************************************************************************

>	CXaraFileRecordHandler* CXaraFile::FindHandler(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		Tag = the tag value of a record
	Returns:	-
	Purpose:	Finds the record handler that's been registered to handle a record with the given tag value
	SeeAlso:	ReadNextRecord()
	Scope: 		-

********************************************************************************************/

CXaraFileRecordHandler* CXFTreeDlg::FindHandler(UINT32 Tag)
{
	List* pRecordHandlerList = BaseCamelotFilter::GetRecordHandlers();

	ERROR3IF(pRecordHandlerList == NULL,"pRecordHandlerList is NULL");

	if (pRecordHandlerList != NULL)
	{
		CXaraFileRecordHandler* pHandler = (CXaraFileRecordHandler*) pRecordHandlerList->GetHead();
		while (pHandler != NULL)
		{
			if (pHandler->IsTagInList(Tag))
				return pHandler;

			pHandler = (CXaraFileRecordHandler*) pRecordHandlerList->GetNext(pHandler);
		}
	}

	return NULL;
}

void CXFTreeDlg::GetTagText(CXFNode* pNode,String_256& Str)
{
	if (pNode != NULL)
		GetTagText(pNode->GetTag(),Str);
}

void CXFTreeDlg::GetTagText(UINT32 Tag,String_256& Str)
{
	CXaraFileRecordHandler* pHandler = FindHandler(Tag);

	if (pHandler != NULL)
		pHandler->GetTagText(Tag,Str);
}


void CXFTreeDlg::ShowNodeDebugInfoForNode(CXFNode *pNode)
{  
	if (pNode != NULL)
	{
		(*EditStr).Empty();

		pNode->ResetReadPos();

		CXaraFileRecordHandler* pHandler = FindHandler(pNode->GetTag());
		if (pHandler != NULL)
			pHandler->GetRecordDescriptionText(pNode->GetCXaraFileRecord(),EditStr);
		else
		{
			TCHAR s[256];
			camSprintf(s,_T("CAN'T FIND HANDLER FOR TAG (%d)\r\nMissing from a handler's tag list perhaps?\r\n"),pNode->GetTag());
			(*EditStr) += s;
		}

		// Copy this string into the edit control.
		SetStringGadgetValue(_R(IDC_DEBUGTREE_DETAILS), *EditStr);
	}
}  


/********************************************************************************************

>	MsgResult CXFTreeDlg::Message( Msg* Message)                               

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/96
	Inputs:		Msg - the message to handle.
	Returns:	-
	Purpose:	Handles messages for this dialog.

********************************************************************************************/

MsgResult CXFTreeDlg::Message( Msg* Message)
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
			ExpandNewTree();
		}
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_TREE)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
			// Get the node out of the item
			CTreeItemID hItem = GetTreeGadgetFirstSelectedItem(_R(IDC_DEBUGTREE_TREE));
			if (hItem.IsOk())
			{
				CCObject* pNode = GetTreeGadgetItemData(_R(IDC_DEBUGTREE_TREE), hItem);
				if (pNode)
					ShowNodeDebugInfoForNode((CXFNode*)pNode);
			}
		}

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
	return OK; 
}



//------------------------------------------------------------------

CXFTreeDlgRecordHandler::CXFTreeDlgRecordHandler(CXFTreeDlg * pThisDlg,CXaraFile* pThisCXFile)
{
	ERROR3IF(pThisDlg    == NULL,"Null pThisDlg");
	ERROR3IF(pThisCXFile == NULL,"Null pThisCXFile");

	pCXFDlg = pThisDlg;
	pCXFile = pThisCXFile;
}

//------------------------------------------------------------------

BOOL CXFTreeDlgRecordHandler::HandleRecord(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"NULL record given!");
	ERROR2IF(pCXFDlg == NULL,FALSE,"NULL pCXFDlg!");
	ERROR2IF(pCXFile == NULL,FALSE,"NULL pCXFile!");

	CXFNode* pNode = new CXFNode(pRecord);

	pCXFDlg->AddNode(pNode);

	switch (pRecord->GetTag())
	{
		case TAG_STARTCOMPRESSION:
			// Ask the compression system to be turned on
			pCXFile->SetCompression(TRUE);
			break;
		case TAG_ENDCOMPRESSION:
			// Ask the compression system to be turned off
			pCXFile->SetCompression(FALSE);
			break;

		case TAG_ENDOFFILE:
			pCXFDlg->SetEndOfFile(TRUE);
			break;
	}

	return TRUE;
}

BOOL CXFTreeDlgRecordHandler::HandleStreamedRecord(CXaraFile * pCXFile, UINT32 Tag,UINT32 Size,UINT32 RecordNumber)
{
	ERROR2IF(pCXFDlg == NULL,FALSE,"NULL pCXFDlg!");
	ERROR2IF(pCXFile == NULL,FALSE,"NULL pCXFile!");

	CXaraFileRecord* pRecord = new CXaraFileRecord(Tag,Size);
	if (pRecord == NULL || !pRecord->Init(TRUE))
		return TRUE;

	pRecord->SetRecordNumber(RecordNumber);

	CXFNode* pNode = new CXFNode(pRecord);		// Constructor makes a copy of the record
	delete pRecord;
	pRecord = NULL;

	pCXFDlg->AddNode(pNode);

	switch (Tag)
	{
		case TAG_STARTCOMPRESSION:
			// Ask the compression system to be turned on
			pCXFile->SetCompression(TRUE);
			break;
		case TAG_ENDCOMPRESSION:
			// Ask the compression system to be turned off
			pCXFile->SetCompression(FALSE);
			break;

		case TAG_ENDOFFILE:
			pCXFDlg->SetEndOfFile(TRUE);
			break;
	}

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL CXFTreeDlgRecordHandler::IsStreamed(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		The tag of the record
	Returns:	TRUE if this is a streamed record
				FALSE otherwise
	Purpose:	Function to find out if the record is streamed or not.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL CXFTreeDlgRecordHandler::IsStreamed(UINT32 Tag)
{
	switch (Tag)
	{
		case TAG_STARTCOMPRESSION:
		case TAG_ENDCOMPRESSION:
			return TRUE;
			break;
	}

	return FALSE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------

CXFNode::CXFNode(CXaraFileRecord* pThisRecord)
{
	pRecord		= NULL;
	pNext		= NULL;
	pPrevious	= NULL;
	pParent		= NULL;
	pChild		= NULL;

	DoShowChildren = FALSE;

	if (pThisRecord != NULL)
		pRecord = pThisRecord->GetCopy();
}

CXFNode::~CXFNode()
{
	if (pRecord != NULL)
		delete pRecord;
}

UINT32 CXFNode::GetTag()
{
	if (pRecord != NULL)
		return pRecord->GetTag();

	return UINT32(-1);
}

UINT32 CXFNode::GetSize()
{
	if (pRecord != NULL)
		return pRecord->GetSize();

	return UINT32(-1);
}

void CXFNode::ResetReadPos()
{
	if (pRecord != NULL)
		pRecord->ResetReadPos();
}


//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

void CXFNodeTypeStatisticsList::Update(CXFNode* pNode)
{
	CXFNodeTypeStatistics* pItem = FindNodeType(pNode->GetTag());

	if (pItem == NULL)
	{
		pItem = new CXFNodeTypeStatistics(pNode->GetTag());
		if (pItem != NULL)
			Add(pItem);
	}

	if (pItem != NULL)
	{
		pItem->IncNumOccurances();
		pItem->AddToTotalSize(pNode->GetSize());
	}
}

void CXFNodeTypeStatisticsList::Dump(CXFTreeDlg* pDlg)
{
	TRACE( _T("\nRecord stats:\n\n"));

	CXFNodeTypeStatistics* pItem = GetHead();

	String_256 Str;

	while (pItem != NULL)
	{
		pDlg->GetTagText(pItem->GetTag(),Str);

		INT32 o = pItem->GetNumOccurances();
		INT32 s = pItem->GetTotalSize();

		TRACE( _T("%s\t%d\t%d\t%d\n"),(TCHAR*)Str,o,s,(o*8)+s);

		pItem = GetNext(pItem);
	}
}


CXFNodeTypeStatistics* CXFNodeTypeStatisticsList::FindNodeType(UINT32 Tag)
{
	CXFNodeTypeStatistics* pItem = GetHead();
	while (pItem != NULL)
	{
		if (pItem->GetTag() == Tag)
			return pItem;

		pItem = GetNext(pItem);
	}

	return NULL;
}


void CXFNodeTypeStatisticsList::Add(CXFNodeTypeStatistics* pNodeType)
{
	AddTail(pNodeType);
}

CXFNodeTypeStatistics* CXFNodeTypeStatisticsList::GetHead()
{
	return (CXFNodeTypeStatistics*)List::GetHead();
}

CXFNodeTypeStatistics* CXFNodeTypeStatisticsList::GetNext(CXFNodeTypeStatistics* pItem)
{
	return (CXFNodeTypeStatistics*)List::GetNext(pItem);
}


/********************************************************************************************

New TreeView Control stuff by Richard - 14/3/97

********************************************************************************************/


/********************************************************************************************

>	CTreeItemID CXFTreeDlg::AddOneItem(CTreeItemID hParent, const StringBase& str, CTreeItemID hInsAfter,
																	 INT32 iImage, CXFNode *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	This function fills out the TV_ITEM and TV_INSERTSTRUCT structures 
				and adds the item to the tree view control.

********************************************************************************************/

CTreeItemID CXFTreeDlg::AddOneItem(CTreeItemID hParent, const StringBase& str, CTreeItemID hInsAfter, INT32 iImage, CXFNode *pNode)
{
	return SetTreeGadgetItem(_R(IDC_DEBUGTREE_TREE), hParent, str, hInsAfter, iImage, pNode);
}


/********************************************************************************************

>	INT32 CXFTreeDlg::GetImageForNode(CXFNode *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Return an imagelist item (icon) for the given node.

********************************************************************************************/

INT32 CXFTreeDlg::GetImageForNode(CXFNode *pNode)
{
	INT32 iImage = m_idxGeneralTag;

	if(pNode != NULL)
	{
		UINT32 Tag = pNode->GetTag();

		switch(Tag)
		{
			case TAG_UP:
				iImage = m_idxUp;
				break;

			case TAG_DOWN:
				iImage = m_idxDown;
				break;

			case TAG_STARTCOMPRESSION:
			case TAG_ENDCOMPRESSION:
				iImage = m_idxCompression;
				break;

			case TAG_DOCUMENT:
			case TAG_CHAPTER:
			case TAG_SPREAD:
			case TAG_LAYER:
			case TAG_PAGE:
			case TAG_SPREADINFORMATION:
				iImage = m_idxPage;
				break;

			case TAG_DEFINERGBCOLOUR:
			case TAG_DEFINECOMPLEXCOLOUR:
				iImage = m_idxDefineColour;
				break;

			case TAG_PREVIEWBITMAP_BMP:
			case TAG_PREVIEWBITMAP_GIF:
			case TAG_PREVIEWBITMAP_JPEG:
			case TAG_PREVIEWBITMAP_PNG:
			case TAG_PREVIEWBITMAP_TIFFLZW:
			case TAG_DEFINEBITMAP_BMP:
			case TAG_DEFINEBITMAP_GIF:
			case TAG_DEFINEBITMAP_JPEG:
			case TAG_DEFINEBITMAP_PNG:
			case TAG_DEFINEBITMAP_BMPZIP:
			case TAG_DEFINESOUND_WAV:
			case TAG_DEFINEBITMAP_JPEG8BPP:
			case TAG_NODE_BITMAP:
			case TAG_NODE_CONTONEDBITMAP:
				iImage = m_idxDefineBitmap;
				break;

			case TAG_GROUP:
			case TAG_COMPOUNDRENDER:
			case TAG_LIVE_EFFECT:
			case TAG_LOCKED_EFFECT:
			case TAG_FEATHER_EFFECT:
			case TAG_GROUPTRANSP:
			case TAG_CACHEDNODESGROUP:
				iImage = m_idxGroup;
				break;

			case TAG_PATH:
			case TAG_PATH_FILLED:
			case TAG_PATH_STROKED:
			case TAG_PATH_FILLED_STROKED:
			case TAG_BLEND:
			case TAG_BLENDER:
			case TAG_BLENDER_CURVEPROP:
			case TAG_BLENDER_CURVEANGLES:
			case TAG_BLEND_PATH:
			case TAG_MOULD_ENVELOPE:
			case TAG_MOULD_PERSPECTIVE:
			case TAG_MOULD_GROUP:
			case TAG_MOULD_PATH:
			case TAG_PATH_FLAGS:
			case TAG_GUIDELINE:
			case TAG_PATH_RELATIVE:
			case TAG_PATH_RELATIVE_FILLED:
			case TAG_PATH_RELATIVE_STROKED:
			case TAG_PATH_RELATIVE_FILLED_STROKED:
			case TAG_PATHREF_IDENTICAL:
			case TAG_PATHREF_TRANSFORM:
			case TAG_ELLIPSE_SIMPLE:
			case TAG_ELLIPSE_COMPLEX:
			case TAG_RECTANGLE_SIMPLE:
			case TAG_RECTANGLE_SIMPLE_REFORMED:
			case TAG_RECTANGLE_SIMPLE_STELLATED:
			case TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED:
			case TAG_RECTANGLE_SIMPLE_ROUNDED:
			case TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED:
			case TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED:
			case TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED:
			case TAG_RECTANGLE_COMPLEX:
			case TAG_RECTANGLE_COMPLEX_REFORMED:
			case TAG_RECTANGLE_COMPLEX_STELLATED:
			case TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED:
			case TAG_RECTANGLE_COMPLEX_ROUNDED:
			case TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED:
			case TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED:
			case TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED:
			case TAG_POLYGON_COMPLEX:
			case TAG_POLYGON_COMPLEX_REFORMED:
			case TAG_POLYGON_COMPLEX_STELLATED:
			case TAG_POLYGON_COMPLEX_STELLATED_REFORMED:
			case TAG_POLYGON_COMPLEX_ROUNDED:
			case TAG_POLYGON_COMPLEX_ROUNDED_REFORMED:
			case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED:
			case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED:
			case TAG_REGULAR_SHAPE_PHASE_1:
			case TAG_REGULAR_SHAPE_PHASE_2:
				iImage = m_idxShape;
				break;

			case TAG_FLATFILL:
			case TAG_LINECOLOUR:
			case TAG_LINEWIDTH:
			case TAG_LINEARFILL:
			case TAG_LINEARFILL3POINT:
			case TAG_LINEARFILLMULTISTAGE:
			case TAG_LINEARFILLMULTISTAGE3POINT:
			case TAG_CIRCULARFILL:
			case TAG_ELLIPTICALFILL:
			case TAG_CONICALFILL:
			case TAG_BITMAPFILL:
			case TAG_CONTONEBITMAPFILL:
			case TAG_FRACTALFILL:
			case TAG_FILLEFFECT_FADE:
			case TAG_FILLEFFECT_RAINBOW:
			case TAG_FILLEFFECT_ALTRAINBOW:
			case TAG_FILL_REPEATING:
			case TAG_FILL_NONREPEATING:
			case TAG_FILL_REPEATINGINVERTED:
			case TAG_FILL_REPEATING_EXTRA:
			case TAG_FLATTRANSPARENTFILL:
			case TAG_LINEARTRANSPARENTFILL:
			case TAG_LINEARTRANSPARENTFILL3POINT:
			case TAG_CIRCULARTRANSPARENTFILL:
			case TAG_ELLIPTICALTRANSPARENTFILL:
			case TAG_CONICALTRANSPARENTFILL:
			case TAG_BITMAPTRANSPARENTFILL:
			case TAG_FRACTALTRANSPARENTFILL:
			case TAG_LINETRANSPARENCY:
			case TAG_STARTCAP:
			case TAG_ENDCAP:
			case TAG_JOINSTYLE:
			case TAG_MITRELIMIT:
			case TAG_WINDINGRULE:
			case TAG_QUALITY:
			case TAG_TRANSPARENTFILL_REPEATING:
			case TAG_TRANSPARENTFILL_NONREPEATING:
			case TAG_TRANSPARENTFILL_REPEATINGINVERTED:
			case TAG_DASHSTYLE:
			case TAG_DEFINEDASH:
			case TAG_ARROWHEAD:
			case TAG_ARROWTAIL:
			case TAG_DEFINEARROW:
			case TAG_DEFINEDASH_SCALED:
			case TAG_USERVALUE:
			case TAG_WEBADDRESS:
			case TAG_WEBADDRESS_BOUNDINGBOX:
			case TAG_FLATFILL_NONE:
			case TAG_FLATFILL_BLACK:
			case TAG_FLATFILL_WHITE:
			case TAG_LINECOLOUR_NONE:
			case TAG_LINECOLOUR_BLACK:
			case TAG_LINECOLOUR_WHITE:
			case TAG_SQUAREFILL:
			case TAG_SQUARETRANSPARENTFILL:
			case TAG_THREECOLFILL:
			case TAG_THREECOLTRANSPARENTFILL:
			case TAG_FOURCOLFILL:
			case TAG_FOURCOLTRANSPARENTFILL:
			case TAG_VARIABLEWIDTHFUNC:
			case TAG_VARIABLEWIDTHTABLE:
			case TAG_STROKETYPE:
			case TAG_STROKEDEFINITION:
			case TAG_STROKEAIRBRUSH:
			case TAG_NOISEFILL:
			case TAG_NOISETRANSPARENTFILL:
			case TAG_WIZOP:
				iImage = m_idxAttribute;
				break;

			case TAG_SETSENTINEL:
				iImage = m_idxSentinelNode; break;

			case TAG_SETPROPERTY:
				iImage = m_idxSetProperty; break;
		}
	}

	return iImage;
}

/********************************************************************************************

>	CTreeItemID CXFTreeDlg::AddItemToNewTreeControl(CTreeItemID hParentItem, CTreeItemID hInsAfterItem, CXFNode *pNode, BOOL bAddChildren)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Recursively add this item and all its children to the tree control
				No longer adds siblings as this guarantees a stack overflow if there is 
				any very large sibling list.

********************************************************************************************/

CTreeItemID CXFTreeDlg::AddItemToNewTreeControl(CTreeItemID hParentItem, CTreeItemID hInsAfterItem, CXFNode *pNode, BOOL bAddChildren)
{
	if(pNode == NULL)
		return CTreeItemID();

	String_256 NodeText; //(((TCHAR*)(pNode->GetRuntimeClass()->m_lpszClassName)));

	GetTagText(pNode, NodeText);
	INT32 iImage = GetImageForNode(pNode);

	CTreeItemID hThisItem = AddOneItem(hParentItem, NodeText, hInsAfterItem, iImage, pNode);

	// Check to see if this node has any children
	CXFNode *pChild = pNode->GetChild();
	if (pChild && bAddChildren)
	{
		CTreeItemID hInsContext;

		while (pChild != NULL)
		{
			// Add to the tree control
			hInsContext = AddItemToNewTreeControl(hThisItem, hInsContext, pChild, bAddChildren);
			pChild = pChild->GetNext();
		}
	}

	return hThisItem;
}



/********************************************************************************************

>	BOOL CXFTreeDlg::InitialiseNewTreeControl(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Sets up the ImageList for the tree control

********************************************************************************************/

BOOL CXFTreeDlg::InitialiseNewTreeControl(void)
{
	CGadgetImageList NewImageList(16, 16);
	m_idxGeneralTag		= NewImageList.Add(_R(IDB_CXFTREE_GENERALTAG));
	m_idxDefineBitmap	= NewImageList.Add(_R(IDB_CXFTREE_DEFINEBITMAP));
	m_idxDefineColour	= NewImageList.Add(_R(IDB_CXFTREE_DEFINECOLOUR));
	m_idxCompression	= NewImageList.Add(_R(IDB_CXFTREE_COMPRESSION));
	m_idxDown			= NewImageList.Add(_R(IDB_CXFTREE_DOWN));
	m_idxUp				= NewImageList.Add(_R(IDB_CXFTREE_UP));
	m_idxGroup			= NewImageList.Add(_R(IDB_CXFTREE_GROUP));
	m_idxPage			= NewImageList.Add(_R(IDB_DEBUGTREE_RENDERABLEPAPER));
	m_idxAttribute		= NewImageList.Add(_R(IDB_DEBUGTREE_ATTRIBUTE));
	m_idxShape			= NewImageList.Add(_R(IDB_DEBUGTREE_RENDERABLEINK));
	m_idxSentinelNode	= NewImageList.Add(_R(IDB_DEBUGTREE_SENTINELNODE));
	m_idxSetProperty	= NewImageList.Add(_R(IDB_DEBUGTREE_SETPROPERTY));

	SetGadgetBitmaps(_R(IDC_DEBUGTREE_TREE), NewImageList);

	return(TRUE);
}


/********************************************************************************************

>	BOOL CXFTreeDlg::GetInfoFromTreeItem(CTreeItemID hItem, CXFNode **ppNode, INT32 *pChildren)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Obtains the Node pointer and children flag for the given HTREEITEM.

********************************************************************************************/

BOOL CXFTreeDlg::GetInfoFromTreeItem(CTreeItemID hItem, CXFNode **ppNode, INT32 *pChildren)
{
	CCObject* pNode = GetTreeGadgetItemData(_R(IDC_DEBUGTREE_TREE), hItem);
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(CXFNode)), "DebugTreeDlg found a non-CXFNode tree item");
	if (!pNode->IsKindOf(CC_RUNTIME_CLASS(CXFNode)))
		return FALSE;

	*ppNode		= (CXFNode*)pNode;
	*pChildren	= GetTreeGadgetChildrenCount(_R(IDC_DEBUGTREE_TREE), hItem, FALSE);

	return TRUE;
}


/********************************************************************************************

>	void CXFTreeDlg::ExpandNewTree()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/97
	Purpose:	Expands the tree display so that the whole tree is displayed in the control.

********************************************************************************************/

void CXFTreeDlg::ExpandNewTree()
{
	CTreeItemID hItem = GetTreeGadgetRootItem(_R(IDC_DEBUGTREE_TREE));
//	CTreeItemID hSelectedItem = GetTreeGadgetFirstSelectedItem(_R(IDC_DEBUGTREE_TREE));

	// Loop through the items in the tree control
	while (hItem.IsOk())
	{
		TreeGadgetExpandItem(_R(IDC_DEBUGTREE_TREE), hItem);
//		CTreeItemID hChild = GetTreeGadgetFirstChildItem(_R(IDC_DEBUGTREE_TREE), hItem);
//		if (hChild.IsOk())
//			hItem = hChild;

		// Get the next item
		hItem = GetTreeGadgetNextVisItem(_R(IDC_DEBUGTREE_TREE), hItem);
	}

//	if (hSelectedItem.IsOk())
//		SelectTreeGadgetItem(_R(IDC_DEBUGTREE_TREE), hSelectedItem, TRUE );
} 

#if FALSE
void CXFTreeDlg::ExpandTreeItem(CTreeItemID hItem, BOOL bChildren)
{
	if (hItem.IsOk())
		TreeGadgetExpandItem(_R(IDC_DEBUGTREE_TREE), hItem);

	if (bChildren)
	{
		CTreeItemID hChild = GetTreeGadgetFirstChildItem(_R(IDC_DEBUGTREE_TREE), hItem);

		// Loop through the child items
		while (hChild.IsOk())
		{
			ExpandTreeItem(hChild, bChildren);

			// Get the next item
			hItem = GetTreeGadgetNextItem(_R(IDC_DEBUGTREE_TREE), hChild);
		}
	}
} 
#endif

#endif	// XAR_TREE_DIALOG

