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

#include "cmxtree.h"
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "simon.h"
#include "oildbug.h"
#include "nodedoc.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetext.h"
#include <ios.h>
#include "bubbleid.h"

//#include "filtrres.h"

#include "rifffile.h"
#include "cdrfiltr.h"
#include "cmxform.h"

//#include "cxftree.h"
//#include "cxfile.h"
//#include "cxftfile.h"
//#include "cxftags.h"
//#include "camfiltr.h"

// new tree control stuff
#include "clipint.h"
#include "keypress.h"
#include "vkextra.h"
#include "insertnd.h"

#if DEBUG_TREE

CC_IMPLEMENT_DYNCREATE(CMXTreeDlg, DialogOp)
//CC_IMPLEMENT_DYNAMIC(CMXTreeDlgRecordHandler,CMXFileRecordHandler)

CC_IMPLEMENT_MEMDUMP(CMXTreeDlgRecordHandler, CC_CLASS_MEMDUMP)
//CC_IMPLEMENT_MEMDUMP( CMXFileRecordHandler, CC_CLASS_MEMDUMP )
CC_IMPLEMENT_MEMDUMP( CMXFileRecord, CC_CLASS_MEMDUMP )

CC_IMPLEMENT_MEMDUMP( CMXNodeInfo, CC_CLASS_MEMDUMP )
CC_IMPLEMENT_MEMDUMP( CMXNode, CC_CLASS_MEMDUMP )

CC_IMPLEMENT_MEMDUMP(CMXNodeTypeStatistics,ListItem)
CC_IMPLEMENT_MEMDUMP(CMXNodeTypeStatisticsList,List)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

CMXTreeDlg* CMXTreeDlg::pCurrentCMXTreeDlg = NULL;

/********************************************************************************************

>	CMXTreeDlg::CMXTreeDlg()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Purpose:	Create a debug tree dialog operation. Sets up the string used to hold
				node details.

********************************************************************************************/

CMXTreeDlg::CMXTreeDlg(): DialogOp(CMXTreeDlg::IDD, MODELESS) 
{      
	pRoot			= NULL;
	pContextNode	= NULL;
	EndOfFile		= FALSE;
	AddNextAsChild	= FALSE;
	Level			= 0;
	TreeInfo		= NULL;
	MaxIndex		= 0;

	EditStr = new StringBase;
	if (!EditStr || !EditStr->Alloc(0x100000)) return;

	CMXTreeDlg::pCurrentCMXTreeDlg = this;

	m_pRIFF = NULL;
	m_pHandler = NULL;
}     


/********************************************************************************************

>	CMXTreeDlg::~CMXTreeDlg()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Purpose:	Cleans up a debug tree dialog operation.

********************************************************************************************/

CMXTreeDlg::~CMXTreeDlg()     
{
	Delete(pRoot);
	DeleteTreeInfo();

	// Deallocate the node details string.
	delete EditStr;         

	CMXTreeDlg::pCurrentCMXTreeDlg = NULL;

	if (m_pRIFF != NULL)
		delete m_pRIFF;

	if (m_pHandler != NULL)
		delete m_pHandler;
}



/********************************************************************************************

>	void CMXTreeDlg::Do(OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Inputs:		Ignored
	Purpose:	The 'Do' function for the debug tree dialog. Just creates and opens the
				dialog.

********************************************************************************************/

void CMXTreeDlg::Do(OpDescriptor*)		// "Do" function        
{  
	Create();
	Open();
}



/********************************************************************************************

>	BOOL CMXTreeDlg::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Returns:	TRUE if intialised ok;
				FALSE if not.
	Purpose:	Initialise the preferences, and register the OpDescriptor for this dialog.

********************************************************************************************/

BOOL CMXTreeDlg::Init()                     
{
	// Register our OpDescriptor
	return (RegisterOpDescriptor(
	 							 0,
	 							 _R(IDS_CMXTREEDLG), 
								 CC_RUNTIME_CLASS(CMXTreeDlg),
	 							 OPTOKEN_CMXTREEDLG,
	 							 CMXTreeDlg::GetState,
	 							 0,	/* help ID */
	 							 _R(IDBBL_DEBUGTREEDLG),
	 							 0	/* bitmap ID */));
}

/********************************************************************************************

>	BOOL CMXTreeDlg::Create()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Returns:	TRUE if setup ok;
				FALSE if not.
	Purpose:	Create and setup the debug tree dialog.

********************************************************************************************/

BOOL CMXTreeDlg::Create()
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

>	OpState CMXTreeDlg::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Purpose:	The usual.

********************************************************************************************/

OpState CMXTreeDlg::GetState(String_256*, OpDescriptor*)
{  
	OpState OpSt;
	return(OpSt);
}	
/********************************************************************************************

>	void CMXTreeDlg::ShowFile(char* pFile)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Input:		pFile = ptr to file name
	Purpose:	Set up the initial debug tree display
	SeeAlso:	CMXTreeDlg::GetTree

********************************************************************************************/

void CMXTreeDlg::ShowFile(char* pFile)
{
	String_256 s = pFile;
	SetFileName(s);

	CreateTree();
	DisplayTree(FALSE);
}  


void CMXTreeDlg::DeInit()
{
	HWND hwndTree = ::GetDlgItem(WindowID, _R(IDC_DEBUGTREE_TREE));
	TreeView_DeleteAllItems(hwndTree);

	Delete(pRoot);
	pRoot = NULL;
} 


void CMXTreeDlg::Delete(CMXNode* pNode)
{
	if (pNode == NULL)
		return;

	Delete(pNode->GetChild());
	Delete(pNode->GetNext());

	delete pNode;
}

void CMXTreeDlg::SetFileName(String_256& ThisFileName)
{
	FileName = ThisFileName;
}


void CMXTreeDlg::CreateTree()
{
	// Init vars before reading the file
	AddNextAsChild = FALSE;
	Level = 0;
	Delete(pRoot);
	pRoot = NULL;
	pContextNode = NULL;
	
	String_256 NameOfType;

	ListOfNodeTypeStats.DeleteAll();

	PathName* pPath = new PathName(FileName);

	if (pPath != NULL && pPath->IsValid())
	{
		CCDiskFile* pFile = new CCDiskFile(*pPath,ios::in | ios::binary);

		if (pFile != NULL)
		{
			EndOfFile = FALSE;

			//CCFile* pCMXile = NULL;
			/*char Buf[1024];
			pFile->read(Buf, 1024);
			// reset to the begining
			pFile->seek(0); */

			// sort out the RIFF file
			m_pRIFF = new RIFFFile();

			FOURCC FormType = m_pRIFF->Init(pFile);
			BOOL FileOk = TRUE;			
			switch (FormType)
			{
				case cmxRIFFFORMTYPE_CMX:
					NameOfType = "CMX";
					break;
				case cmxRIFFFORMTYPE_CDR5:
					NameOfType = "CDR5";
					break;
				case cmxRIFFFORMTYPE_CDT5:
					NameOfType = "CDRT5";
					break;
				case cmxRIFFFORMTYPE_CDR6:
					NameOfType = "CDR6";
					break;
				case cmxRIFFFORMTYPE_CDT6:
					NameOfType = "CDT6";
					break;

				default:
					FileOk=FALSE;
					break;
			}

			if (FileOk)
			{
				ParseFile(pFile);
			}

			delete pFile;
		}
	}

	if (pPath != NULL)
		delete pPath;

	ListOfNodeTypeStats.Dump(this);
	ListOfNodeTypeStats.DeleteAll();

	AddOneItem(NULL, (TCHAR *)FileName, TVI_LAST, m_idxPage, NULL);
	AddOneItem(NULL, (TCHAR*)NameOfType,TVI_LAST, m_idxPage, NULL);
	AddItemsToNewTreeControl(NULL, pRoot);
}


BOOL CMXTreeDlg::ParseFile(CCFile * pFile)
{
	ERROR2IF(pFile == NULL || m_pRIFF == NULL,FALSE,"Bad params");

	m_pHandler = new CMXTreeDlgRecordHandler(this, pFile);
	
	if (m_pHandler == NULL)
		return FALSE;

	// loop through looking at top level lists, and dispatch them to various handlers
	BOOL finished = FALSE;
	// store the level we're working on
	UINT32 StartLevel = m_pRIFF->GetObjLevel();

	// get the next object in the file
	if (!m_pRIFF->NextObject())
		finished = TRUE;

	RIFFObjectType	ObjectType		= m_pRIFF->GetObjType();
	FOURCC			ChunkType		= m_pRIFF->GetObjChunkType();
	UINT32			Size			= m_pRIFF->GetObjSize();
	UINT32			Level			= m_pRIFF->GetObjLevel();
	INT32			RecordNumber	= 0;

	while (ObjectType != RIFFOBJECTTYPE_FILEEND && !finished)
	{
		CMXFileRecord* pCMXFileRecord = new CMXFileRecord(ObjectType, ChunkType, Size, Level);
		
		RecordNumber ++;	

		if (pCMXFileRecord != NULL)
		{
			pCMXFileRecord->SetRecordNumber(RecordNumber);
			m_pHandler->HandleRecord(pCMXFileRecord);
		}
		else
			finished = TRUE;
	
		// get the next object in the file
		if (!m_pRIFF->NextObject())
			finished = TRUE;

		ObjectType	= m_pRIFF->GetObjType();
		ChunkType	= m_pRIFF->GetObjChunkType();
		Size		= m_pRIFF->GetObjSize();
		Level		= m_pRIFF->GetObjLevel();
	}

	return TRUE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------

void CMXTreeDlg::AddNode(CMXNode* pNode)
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
		INT32 NewLevel = pNode->GetObjectLevel();
		if (NewLevel > Level)
		{
			AddNextAsChild = TRUE;
			Level++;
		}
		else if (NewLevel < Level)
		{
			AddNextAsChild = FALSE;
			Level--;
		}

		/* switch (pNode->GetObjectType())
		{
			case RIFFOBJECTTYPE_LISTEND:
			// case TAG_UP:
				AddNextAsChild = FALSE;
				Level--;
				break;

			case RIFFOBJECTTYPE_LISTSTART:
			//case TAG_DOWN:
				AddNextAsChild = TRUE;
				Level++;
				break;
		} */

		if (pContextNode != NULL)
		{
			if (AddNextAsChild)
			{
				CMXNode* pChildNode = pContextNode->GetChild();
				if (pChildNode != NULL)
				{
					TRACEUSER( "Neville from Markn code", _T("Take cover - context node has already got children"));

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

				//if (pNode->GetTag() == TAG_UP)
				//	pContextNode = pNode->GetParent();
				//else
					pContextNode = pNode;
			}
		}
	}
}

/********************************************************************************************

>	CMXFileRecordHandler* CCFile::FindHandler(UINT32 Tag)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Inputs:		Tag = the tag value of a record
	Returns:	-
	Purpose:	Finds the record handler that's been registered to handle a record with the given tag value
	SeeAlso:	ReadNextRecord()
	Scope: 		-

********************************************************************************************/

CMXTreeDlgRecordHandler* CMXTreeDlg::FindHandler(FOURCC ChunkType)
{
/*	List* pRecordHandlerList = BaseCamelotFilter::GetRecordHandlers();

	ERROR3IF(pRecordHandlerList == NULL,"pRecordHandlerList is NULL");

	if (pRecordHandlerList != NULL)
	{
		CMXFileRecordHandler* pHandler = (CMXFileRecordHandler*) pRecordHandlerList->GetHead();
		while (pHandler != NULL)
		{
			if (pHandler->IsTagInList(Tag))
				return pHandler;

			pHandler = (CMXFileRecordHandler*) pRecordHandlerList->GetNext(pHandler);
		}
	}
*/

	// return the default handler
	return m_pHandler;
}

void CMXTreeDlg::GetTagText(CMXNode* pNode,String_256& Str)
{
	if (pNode != NULL)
		GetTagText(pNode->GetChunkType(),Str);
}

void CMXTreeDlg::GetTagText(FOURCC ChunkType, String_256& Str)
{
	CMXTreeDlgRecordHandler* pHandler = FindHandler(ChunkType);

	if (pHandler != NULL)
		pHandler->GetTagText(ChunkType, Str);
}

INT32 CMXTreeDlg::AddDisplayNode(CMXNode* pNode,INT32 Index,INT32 Depth,BOOL ExpandAll)
{
	while (pNode != NULL)
	{
		if (ExpandAll)
			pNode->SetShowChildren(TRUE);

		// Set up the info record for this node.
		CMXNodeInfo* pInfo = new CMXNodeInfo;
		
		if (pInfo != NULL)
			pInfo->pNode = pNode;
		else
			return Index;

		// Create the indentation string, based of the depth value
		char indent[256];
		indent[0] = 0;
		for (INT32 i=0;i<Depth;i++)
			camStrcat(indent,"       ");

		if (pNode->HasChildren())
		{
			if (pNode->ShowChildren())
				camStrcat(indent,"-");
			else
				camStrcat(indent,"+");
		}

		// Get the textual desc of the tag
		String_256 TagText;
		GetTagText(pNode,TagText);

		// Construct the string for the node
		String_256 ListBoxText = indent;
		ListBoxText += TagText;

		// Add the item to the list
		SetStringGadgetValue(_R(IDC_DEBUGTREE_LIST), &ListBoxText, FALSE, Index); 

		// Store the debug info for this item
		if (TreeInfo != NULL)
			TreeInfo->AddDebugInfo(Index, pInfo);

		MaxIndex = Index;
		Index++;

		if (pNode->ShowChildren())
			Index = AddDisplayNode(pNode->GetChild(),Index,Depth+1,ExpandAll);

//		dont recurse the next value, stick it in a while loop you prat
//		Index = AddDisplayNode(pNode->GetNext(),Index,Depth);

		pNode = pNode->GetNext();

	}

	return Index;
}

void CMXTreeDlg::DeleteTreeInfo()
{
	if (TreeInfo != NULL)
	{
		for (INT32 i=0;i<=MaxIndex;i++)
		{
			CMXNodeInfo *pNodeInfo = GetInfo(i);

			TreeInfo->AddDebugInfo(i, NULL);

			if (pNodeInfo != NULL)
				delete pNodeInfo;
		}

		delete TreeInfo;
	}

	TreeInfo = NULL;
	MaxIndex = 0;
}


void CMXTreeDlg::DisplayTree(BOOL ExpandAll)
{
	// Get handle of list box
	HWND hListBox = ::GetDlgItem(WindowID, _R(IDC_DEBUGTREE_LIST));
	ENSURE(hListBox != NULL, "Can't find list box in debug tree dialog!");

	DeleteTreeInfo();

	TreeInfo = new DebugTreeInfo(hListBox);

	DeleteAllValues(_R(IDC_DEBUGTREE_LIST));
	SetStringGadgetValue(_R(IDC_DEBUGTREE_LIST), &FileName, FALSE, 0); 
	AddDisplayNode(pRoot,1,0,ExpandAll);
}

/********************************************************************************************

>	void CMXTreeDlg::ShowNodeDebugInfo(INT32 ListIndex)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Inputs:		ListIndex - the list item to display information on.
	Purpose:	Display the debug information for the given node in the debug tree dialog's
				scrolly window.

********************************************************************************************/

void CMXTreeDlg::ShowNodeDebugInfo(INT32 ListIndex)
{  
	if (ListIndex < 1)
	{
		String_256 Str = "";
		SetStringGadgetValue(_R(IDC_DEBUGTREE_DETAILS), &Str); 
		return;
	}

	// Get the Node
	CMXNode *pNode = GetInfo(ListIndex)->pNode;

	if (pNode != NULL)
	{
		ShowNodeDebugInfoForNode(pNode);
	}
}  

void CMXTreeDlg::ShowNodeDebugInfoForNode(CMXNode *pNode)
{  
	if (pNode != NULL)
	{
		(*EditStr).Empty();

		pNode->ResetReadPos();

		CMXTreeDlgRecordHandler* pHandler = FindHandler(pNode->GetChunkType());
		if (pHandler != NULL)
			pHandler->GetRecordDescriptionText(pNode->GetCMXFileRecord(),EditStr);
		else
		{
			TCHAR s[256];
			FOURCC ChunkType = pNode->GetChunkType();
			canSprintf(s,_T("Default handler (%d)\r\n"), ChunkType);
			(*EditStr) += s;
		}

		// Copy this string into the edit control.
		SetStringGadgetValue(_R(IDC_DEBUGTREE_DETAILS), EditStr); 
	}
}  

CMXNodeInfo *CMXTreeDlg::GetInfo(INT32 Index)
{
	ERROR2IF(TreeInfo==NULL,NULL,"TreeInfo is NULL");
	return (CMXNodeInfo *) TreeInfo->GetDebugInfo(Index);
}

/********************************************************************************************

>	MsgResult CMXTreeDlg::Message( Msg* Message)                               

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Inputs:		Msg - the message to handle.
	Returns:	-
	Purpose:	Handles messages for this dialog.

********************************************************************************************/

MsgResult CMXTreeDlg::Message( Msg* Message)
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
			DisplayTree(TRUE); 
			ExpandNewTree();
		}    
/*
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_DUMP))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Dump the tree
			DumpTree();

		}    
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_HIDDEN))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Update the 'show hidden nodes' flag
			BOOL Valid;
			INT32 Value = GetLongGadgetValue(_R(IDC_DEBUGTREE_HIDDEN), 0, 1, 0, &Valid);

			if (Valid)
				ShowHiddenNodes = (Value == 1);

			// Update the tree display according to this value.
			ShowHidden(ShowHiddenNodes);
		}    
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_SELNONLY))
			 && (Msg->DlgMsg == DIM_LFT_BN_CLICKED))
		{   
			// Update the 'show selection' flag
			BOOL Valid;
			INT32 Value = GetLongGadgetValue(_R(IDC_DEBUGTREE_SELNONLY), 0, 1, 0, &Valid);

			if (Valid)
				SelectionOnly = (Value == 1);

			// Update the tree display according to this value.
			ShowHidden(!SelectionOnly);
		}    
		*/
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_LIST)) && 
				 (Msg->DlgMsg == DIM_SELECTION_CHANGED_COMMIT))
		{
			WORD Index; 
			GetValueIndex(_R(IDC_DEBUGTREE_LIST), &Index);
			if (Index > 0)
			{
				CMXNode *pNode = GetInfo(Index)->pNode;
				if (pNode != NULL)
				{
					pNode->SetShowChildren(!pNode->ShowChildren());
					DisplayTree(FALSE);
					SetSelectedValueIndex(_R(IDC_DEBUGTREE_LIST), Index);
				}
			}
		} 
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_LIST)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED)) 
		{
			WORD Index; 
			GetValueIndex(_R(IDC_DEBUGTREE_LIST), &Index);
			ShowNodeDebugInfo(Index);
		}  
		else if ((Msg->GadgetID == _R(IDC_DEBUGTREE_TREE)) && (Msg->DlgMsg == DIM_SELECTION_CHANGED))
		{
			NM_TREEVIEW *pTV = (NM_TREEVIEW *)Msg->DlgMsgParam;
			
			if(pTV != NULL)
			{
				// Get the newly selected TV_ITEM
				TV_ITEM Item = pTV->itemNew;
				{
					// Get the node out of the item
					CMXNode *pNode = (CMXNode *)Item.lParam;
					ShowNodeDebugInfoForNode(pNode);
				}
			}
   		} 

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}
	return OK; 
}



//------------------------------------------------------------------

CMXTreeDlgRecordHandler::CMXTreeDlgRecordHandler(CMXTreeDlg * pThisDlg,CCFile* pThisCMXile)
{
	ERROR3IF(pThisDlg    == NULL,"Null pThisDlg");
	ERROR3IF(pThisCMXile == NULL,"Null pThisCMXile");

	pCMXDlg = pThisDlg;
	pCMXile = pThisCMXile;
}

//------------------------------------------------------------------

BOOL CMXTreeDlgRecordHandler::HandleRecord(CMXFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL,FALSE,"NULL record given!");
	ERROR2IF(pCMXDlg == NULL,FALSE,"NULL pCMXDlg!");
	ERROR2IF(pCMXile == NULL,FALSE,"NULL pCMXile!");

	CMXNode* pNode = new CMXNode(pRecord);

	pCMXDlg->AddNode(pNode);

/*	switch (pRecord->GetTag())
	{
		case TAG_STARTCOMPRESSION:
			// Ask the compression system to be turned on
			pCMXile->SetCompression(TRUE);
			break;
		case TAG_ENDCOMPRESSION:
			// Ask the compression system to be turned off
			pCMXile->SetCompression(FALSE);
			break;

		case TAG_ENDOFFILE:
			pCMXDlg->SetEndOfFile(TRUE);
			break;
	} */

	return TRUE;
}

void CMXTreeDlgRecordHandler::GetTagText(FOURCC ChunkType, String_256& Str)
{
	// Convert the ChunkType from the DWORD into a description;
	TCHAR c = ' ';
	
	c = (TCHAR)((ChunkType) & 0xFF);
	Str += c;
	c = (TCHAR)((ChunkType >> 8) & 0xFF);
	Str += c;
	c = (TCHAR)((ChunkType >> 16) & 0xFF);
	Str += c;
	c = (TCHAR)((ChunkType >> 24) & 0xFF);
	Str += c;
}

/********************************************************************************************

>	virtual void CMXTreeDlgRecordHandler::GetRecordDescriptionText(CMXFileRecord* pCMXFileRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Markn code
	Created:	7/11/97
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This is the base class record description generator.
				Adds the Tag & Size items to the string, so derived classes should call the base class version
				first.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CMXTreeDlgRecordHandler::GetRecordDescriptionText(CMXFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	String_256 TagText;
	GetTagText(pRecord->GetChunkType(),TagText);
	TCHAR* pTagText = TagText;

	RIFFObjectType ObjType = pRecord->GetObjectType();
	String_256 ObjText;
	TCHAR* pObjText = ObjText;
	switch (ObjType)
	{
		case RIFFOBJECTTYPE_NONE:
			ObjText = "None";
			break;
		case RIFFOBJECTTYPE_CHUNK:
			ObjText = "Chunk";
			break;
		case RIFFOBJECTTYPE_LISTSTART:
			ObjText = "List start";
			break;
		case RIFFOBJECTTYPE_LISTEND:
			ObjText = "List end";
			break;
		case RIFFOBJECTTYPE_FILEEND:
			ObjText = "List end";
			break;
	}

	TCHAR s[256];
	camSprintf(s,_T("Record Number = %d\r\nRIFF type = %s\r\nTag = %s\r\nSize = %d\r\n\r\n"),pRecord->GetRecordNumber(),pObjText,pTagText,pRecord->GetChunkSize());

	(*pStr) += s;
}

//------------------------------------------------------------------
//------------------------------------------------------------------

CMXNode::CMXNode(CMXFileRecord* pThisRecord)
{
	pRecord		= NULL;
	pNext		= NULL;
	pPrevious	= NULL;
	pParent		= NULL;
	pChild		= NULL;

	DoShowChildren = FALSE;

	if (pThisRecord != NULL)
		pRecord = pThisRecord; //->GetCopy();
}

CMXNode::~CMXNode()
{
	if (pRecord != NULL)
		delete pRecord;
}

RIFFObjectType	CMXNode::GetObjectType()
{
	if (pRecord != NULL)
		return pRecord->GetObjectType();

	return RIFFOBJECTTYPE_NONE;
}

FOURCC	CMXNode::GetChunkType()
{
	if (pRecord != NULL)
		return pRecord->GetChunkType();

	return cmxRIFFFORMTYPE_CMX;
}

UINT32	CMXNode::GetChunkSize()
{
	if (pRecord != NULL)
		return pRecord->GetChunkSize();

	return UINT32(-1);
}

UINT32	CMXNode::GetObjectLevel()
{
	if (pRecord != NULL)
		return pRecord->GetObjectLevel();

	return UINT32(-1);
}

void CMXNode::ResetReadPos()
{
//	if (pRecord != NULL)
//		pRecord->ResetReadPos();
}


//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------

void CMXNodeTypeStatisticsList::Update(CMXNode* pNode)
{
	CMXNodeTypeStatistics* pItem = FindNodeType(pNode->GetObjectType());

	if (pItem == NULL)
	{
		pItem = new CMXNodeTypeStatistics(pNode->GetObjectType());
		if (pItem != NULL)
			Add(pItem);
	}

	if (pItem != NULL)
	{
		pItem->IncNumOccurances();
		pItem->AddToTotalSize(pNode->GetChunkSize());
	}
}

void CMXNodeTypeStatisticsList::Dump(CMXTreeDlg* pDlg)
{
	TRACE( _T("\nRecord stats:\n\n"));

	CMXNodeTypeStatistics* pItem = GetHead();

	String_256 Str;

	while (pItem != NULL)
	{
		pDlg->GetTagText(pItem->GetChunkType(), Str);

		INT32 o = pItem->GetNumOccurances();
		INT32 s = pItem->GetTotalSize();

		TRACE( _T("%s\t%d\t%d\t%d\n"),(TCHAR*)Str,o,s,(o*8)+s);

		pItem = GetNext(pItem);
	}
}


CMXNodeTypeStatistics* CMXNodeTypeStatisticsList::FindNodeType(FOURCC ChunkType)
{
	CMXNodeTypeStatistics* pItem = GetHead();
	while (pItem != NULL)
	{
		if (pItem->GetChunkType() == ChunkType)
			return pItem;

		pItem = GetNext(pItem);
	}

	return NULL;
}


void CMXNodeTypeStatisticsList::Add(CMXNodeTypeStatistics* pNodeType)
{
	AddTail(pNodeType);
}

CMXNodeTypeStatistics* CMXNodeTypeStatisticsList::GetHead()
{
	return (CMXNodeTypeStatistics*)List::GetHead();
}

CMXNodeTypeStatistics* CMXNodeTypeStatisticsList::GetNext(CMXNodeTypeStatistics* pItem)
{
	return (CMXNodeTypeStatistics*)List::GetNext(pItem);
}


/********************************************************************************************

New TreeView Control stuff by Richard - 14/3/97

********************************************************************************************/


/********************************************************************************************

>	HTREEITEM CMXTreeDlg::AddOneItem(HTREEITEM hParent, TCHAR *pText, HTREEITEM hInsAfter,
																	 INT32 iImage, CMXNode *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	This function fills out the TV_ITEM and TV_INSERTSTRUCT structures 
				and adds the item to the tree view control.

********************************************************************************************/

HTREEITEM CMXTreeDlg::AddOneItem(HTREEITEM hParent, TCHAR *pText, HTREEITEM hInsAfter, INT32 iImage, CMXNode *pNode)
{
	HWND hwndTree = ::GetDlgItem(WindowID, _R(IDC_DEBUGTREE_TREE));

	HTREEITEM hItem;
	TV_ITEM tvI;
	TV_INSERTSTRUCT tvIns;

	// The pszText, iImage, and iSelectedImage members are filled out.
	tvI.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvI.pszText = pText;
	tvI.cchTextMax = camStrlen(pText);
	tvI.iImage = iImage;
	tvI.iSelectedImage = iImage;
	tvI.lParam = (LPARAM)pNode;

	tvIns.item = tvI;
	tvIns.hInsertAfter = hInsAfter;
	tvIns.hParent = hParent;

	// Insert the item into the tree.
	hItem = TreeView_InsertItem(hwndTree, &tvIns);

	return (hItem);
}


/********************************************************************************************

>	INT32 CMXTreeDlg::GetImageForNode(CMXNode *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Return an imagelist item (icon) for the given node.

********************************************************************************************/

INT32 CMXTreeDlg::GetImageForNode(CMXNode *pNode)
{
	INT32 iImage = m_idxGeneralTag;

	if (pNode != NULL)
	{

		RIFFObjectType Type = pNode->GetObjectType();
		switch(Type)
		{
			case RIFFOBJECTTYPE_LISTEND:
				iImage = m_idxUp;
				break;

			case RIFFOBJECTTYPE_LISTSTART:
				iImage = m_idxDown;
				break;

			case RIFFOBJECTTYPE_FILEEND:
				iImage = m_idxCompression;
				break;

			case RIFFOBJECTTYPE_CHUNK:
				iImage = m_idxGroup;
				/*
				iImage = m_idxPage;
				iImage = m_idxDefineColour;
				iImage = m_idxDefineBitmap;
				iImage = m_idxGroup;
				iImage = m_idxShape;
				iImage = m_idxAttribute; */
				break;
		}
	}

	return iImage;
}

/********************************************************************************************

>	HTREEITEM CMXTreeDlg::AddItemsToNewTreeControl(HTREEITEM hParentItem, CMXNode *pNode)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Recursively add new items to the tree control from given root node

********************************************************************************************/

HTREEITEM CMXTreeDlg::AddItemsToNewTreeControl(HTREEITEM hParentItem, CMXNode *pNode)
{
	if(pNode == NULL)
		return 0;

	String_256 NodeText; //(((TCHAR*)(pNode->GetRuntimeClass()->m_lpszClassName)));

	GetTagText(pNode, NodeText);
	INT32 iImage = GetImageForNode(pNode);

	HTREEITEM hThisItem = AddOneItem(hParentItem, (TCHAR *)NodeText, hParentItem, iImage, pNode);

	// Check to see if this node has any children
	CMXNode *pChild = pNode->GetChild();

	if(pChild != NULL)
	{
		// Add to the tree control
		AddItemsToNewTreeControl(hThisItem, pChild);
	}
  
	// Add the next nodes in
	CMXNode *pNext = pNode->GetNext();
	
	if(pNext != NULL)
	{
		// Add to the tree control
		AddItemsToNewTreeControl(hParentItem, pNext);
	}

	return hThisItem;
}


/********************************************************************************************

>	INT32 CMXTreeDlg::AddBitmapResourceToImageList(HIMAGELIST hList, UINT32 ResID)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Obtain the bitmap from the resources and add it to the imagelist.
				We use the old Magenta Mask trick for optimum viewtasticness...

********************************************************************************************/

INT32 CMXTreeDlg::AddBitmapResourceToImageList(HIMAGELIST hList, UINT32 ResID)
{
	HINSTANCE hMain = AfxGetResourceHandle();
	HBITMAP hBmp = ::LoadBitmap(hMain, MAKEINTRESOURCE(ResID));
	INT32 index = ImageList_AddMasked(hList, hBmp, RGB(255,0,255));
	return index;
}

/********************************************************************************************

>	BOOL CMXTreeDlg::InitialiseNewTreeControl(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Sets up the ImageList for the tree control

********************************************************************************************/

BOOL CMXTreeDlg::InitialiseNewTreeControl(void)
{
	HWND hwndTree = ::GetDlgItem(WindowID, _R(IDC_DEBUGTREE_TREE));

	INT32 BitmapWidth = 16;
	INT32 BitmapHeight = 16;

	// Need to create an image list
	hNewTreeControlImageList = ImageList_Create(BitmapWidth, BitmapHeight, ILC_MASK, 0, 0);

	// Load the bitmaps and add them to the image list.
	m_idxGeneralTag = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_GENERALTAG));
	m_idxDefineBitmap = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_DEFINEBITMAP));
	m_idxDefineColour = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_DEFINECOLOUR));
	m_idxCompression = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_COMPRESSION));
	m_idxDown = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_DOWN));
	m_idxUp = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_UP));
	m_idxGroup = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_CXFTREE_GROUP));
	m_idxPage = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_DEBUGTREE_RENDERABLEPAPER));
	m_idxAttribute = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_DEBUGTREE_ATTRIBUTE));
	m_idxShape = AddBitmapResourceToImageList(hNewTreeControlImageList, _R(IDB_DEBUGTREE_RENDERABLEINK));

	// Associate the image list with the tree view control.
	TreeView_SetImageList(hwndTree, hNewTreeControlImageList, m_idxGeneralTag);

	// Check list actually contains bitmaps
	INT32 Count = ImageList_GetImageCount(hNewTreeControlImageList);

	return(Count > 0);
}


/********************************************************************************************

>	BOOL CMXTreeDlg::GetInfoFromHTREEITEM(HTREEITEM hItem, CMXNode **ppNode, INT32 *pChildren)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/97
	Purpose:	Obtains the Node pointer and children flag for the given HTREEITEM.

********************************************************************************************/

BOOL CMXTreeDlg::GetInfoFromHTREEITEM(HTREEITEM hItem, CMXNode **ppNode, INT32 *pChildren)
{
	HWND hwndTree = ::GetDlgItem(WindowID, _R(IDC_DEBUGTREE_TREE));

	TV_ITEM Item;
	Item.hItem = hItem;
	Item.mask = TVIF_PARAM | TVIF_CHILDREN | TVIF_STATE;

	if(TreeView_GetItem(hwndTree, &Item))
	{
		*ppNode = (CMXNode *)Item.lParam;
		*pChildren = Item.cChildren;
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************

>	void CMXTreeDlg::ExpandNewTree()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/97
	Purpose:	Expands the tree display so that the whole tree is displayed in the control.

********************************************************************************************/

void CMXTreeDlg::ExpandNewTree()
{
	HTREEITEM hItem = 0;
	HWND hwndTree = ::GetDlgItem(WindowID, _R(IDC_DEBUGTREE_TREE));
	hItem = TreeView_GetRoot(hwndTree);

	HTREEITEM hSelectedItem = TreeView_GetSelection(hwndTree);

	// Loop through the items in the tree control, dumping each one to the trace output
	while(hItem != 0)
	{
		TreeView_Expand(hwndTree, hItem, TVE_EXPAND);	

		// Get the next item
		hItem = TreeView_GetNextItem(hwndTree, hItem, TVGN_NEXTVISIBLE);
	}

	TreeView_SelectItem(hwndTree, hSelectedItem);
} 

#endif //DEBUG_TREE



