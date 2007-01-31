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
// Implementation of the FileInfo class - copied from AboutDlg

/*
*/

#include "camtypes.h"

DECLARE_SOURCE("$Revision$");

#include "camelot.h"
//#include "dlgtypes.h"  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "finfodlg.h" 

//#include "mario.h"				// for string resource
//#include "peter.h"				// for string resources
//#include "reshlpid.h"			// for help resources
//#include "peterdlg.h"			// for dialogue resources

//#include "document.h"  			// for reading and setting document attributes - in camtypes.h [AUTOMATICALLY REMOVED]
#include "localenv.h"			// for locale string settings
//#include "node.h"				// for tree scanning calls - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h"				// for Spread - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"				// for Page
//#include "group.h"				// for NodeGroup - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"			// for NodeBitmap
#include "bitmpinf.h"			// for BitmapInfo
//#include "app.h"				// for GetApplication - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "convert.h"			// for StringToBytes and BytesToString  - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetxts.h"			// for TextStory node.
#include "fontlist.h"			// for the document font list
#include "progress.h"
//#include "barsdlgs.h"			// for the document info file bar controls
#include "fontman.h"
#include "bubbleid.h"
//#include "customlist.h"
//#include "richard.h"

//#include "will2.h"
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ophist.h"

// Required for the document font combo box handling
#include "fontman.h"
#include "fontdrop.h"
#include "fontbase.h"

#include "nodeliveeffect.h"

CC_IMPLEMENT_DYNCREATE(FileInfo, DialogOp)   
CC_IMPLEMENT_DYNCREATE(RefItem, ListItem)

#define new CAM_DEBUG_NEW


const CDlgMode FileInfo::Mode = MODELESS; 	// The dialog is modal (for the moment.)  
const UINT32 FileInfo::IDD = _R(IDD_FINFODLG);
FileInfo * FileInfo::CurrentDialog = NULL;			  

DocumentFontDropDown	*FileInfo::NameDropDown = NULL;		// Font name drop-down list which shows document fonts

// Timing constants
#define DURATION_FASTJOB 500		// Half a second
#define DURATION_SLOWJOB 10*1000	// Ten seconds


/*******************************************************************************************

>	FileInfo::FileInfo(): DialogOp(FileInfo::IDD, FileInfo::Mode) 


	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	FileInfo constructor
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
FileInfo::FileInfo(): DialogOp(FileInfo::IDD, FileInfo::Mode) 
{
}       



/*******************************************************************************************

>	FileInfo::~FileInfo()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/09/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	FileInfo destructor - deallocate the BitmapList space
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
FileInfo::~FileInfo() 
{
	BitmapList.DeleteAll();

	CurrentDialog = NULL;

	// Jim, 5/8/96
	// Delete a Drop Down list if it's been created (via a static, of all things!)
	// This removes a memory leak. (And should have been here all the time.)
	if (NameDropDown != NULL)
	{
		delete NameDropDown;
		NameDropDown = NULL;
	}
} 



/*******************************************************************************************

>	MsgResult FileInfo::Message(Msg* Message);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the File info dialog's messages 
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
MsgResult FileInfo::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
		
		if (Msg->DlgMsg == DIM_CREATE)
		{
			OnDimCreate();
		}

		if ((Msg->DlgMsg == DIM_COMMIT) || (Msg->DlgMsg == DIM_SOFT_COMMIT))
		{
			Document*	WorkDoc = Document::GetSelected();
			if (WorkDoc != NULL)
			{
				String_256 NewComment = GetStringGadgetValue(_R(IDC_FILEINFO_COMMENT), NULL, -1);
				String_256 OldComment = WorkDoc->GetComment();
				if (NewComment != OldComment)
				{
					WorkDoc->SetComment(&NewComment);	
					WorkDoc->SetModified(TRUE);
				}
			}
		}

		if ((Msg->DlgMsg == DIM_COMMIT) || (Msg->DlgMsg == DIM_CANCEL))
		{
			Close(); // Close and destroy the dialog 
			End();
			return (DLG_EAT_IF_HUNGRY(Msg));  	 
		}

		if (Msg->DlgMsg == DIM_TIMER)
		{
			SetDocInfo(FALSE, timerSlowJob.Elapsed(DURATION_SLOWJOB, TRUE));				// Don't update comment field
		}
	}

	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg*	DocMsg = (DocChangingMsg*) Message;
		if (DocMsg->State == DocChangingMsg::SELCHANGED)
		{
			if (DocMsg->pOldDoc != NULL)
			{
				String_256 NewComment = GetStringGadgetValue(_R(IDC_FILEINFO_COMMENT), NULL, -1);
				String_256 OldComment = DocMsg->pOldDoc->GetComment();
				if (NewComment != OldComment)
				{
					DocMsg->pOldDoc->SetComment(&NewComment);	
					DocMsg->pOldDoc->SetModified(TRUE);
				}
			}
			SetDocInfo(TRUE,TRUE);
		}
	}

//	return OK;  
	return DialogOp::Message(Message);
}  



/*******************************************************************************************

>	BOOL FileInfo::OnDimCreate()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the contents of the File Info dialog when it is first created
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
BOOL FileInfo::OnDimCreate()
{
	// Setup callback timer
	this->SetTimer(42, DURATION_FASTJOB);				// Ten seconds between updates
	timerSlowJob.Sample();

	
	// Setting up the external resources list control.
	if (!m_lstctrlExternalResInfo.Init(WindowID, _R(IDC_FINFO_REFSLIST)))
		return FALSE;
		
	m_lstctrlExternalResInfo.AddColumn(_T("Status"), 14);
	m_lstctrlExternalResInfo.AddColumn(_T("Item"), 180);
	m_lstctrlExternalResInfo.AddColumn(_T("Details"), 760);

	// Setup list columns
	// Why do we do this here when we have to use a custom AddItem function (AddRefsItem)
	// to populate the list?
/*	CCustomList* pListGadget = CCustomList::GetGadget(GetReadWriteWindowID(), _R(IDC_FINFO_REFSLIST));
	ERROR2IF(pListGadget==NULL, FALSE, "No refs list gadget");
//	pListGadget->ShowColumnHeaders("", "Item", "Details");
	pListGadget->SetColumnWidth(0, 14);		// Status indicators
	pListGadget->SetColumnWidth(1, 80);		// Item name
	pListGadget->SetColumnWidth(2, 760);	// Details
*/
	SetDocInfo(TRUE, TRUE);

	return TRUE;
}

		 	

/*******************************************************************************************

>	OpState	FileInfo::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns the OpState of the file info dialogue operation
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
OpState	FileInfo::GetState(String_256*, OpDescriptor*)
{
	return OpState(FALSE, (Document::GetSelected() == NULL));
}

		 

/*******************************************************************************************

>	BOOL FileInfo::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a file info dialog
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
BOOL FileInfo::Init()
{  
	return RegisterOpDescriptor(
								0,								// Tool ID
 								_R(IDS_FILE_INFO), 					// String resouirce ID
								CC_RUNTIME_CLASS(FileInfo),		// Runtime class
 								OPTOKEN_FINFODLG,				// Token string
 								FileInfo::GetState,				// GetState function
								0,								// Help ID
								_R(IDBBL_FILEINFO),					// Bubble ID
								_R(IDD_BARCONTROLSTORE),			// Resource ID
								_R(IDC_FILEINFO),					// Control ID
								SYSTEMBAR_FILE,					// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_FILEINFO_ONE),				// String for one copy only
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
}   
 

         
/*******************************************************************************************

>	void FileInfo::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94 
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates and shows a file info dialog
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
void FileInfo::Do(OpDescriptor*)
{
	Create();
	Open();
	CurrentDialog = this;
}

/*******************************************************************************************

>	void FileInfo::Refresh()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/95 
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Can be called by anyone to force an update	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
void FileInfo::Refresh()
{
	if (CurrentDialog)
		CurrentDialog->SetDocInfo(TRUE,TRUE);

}


/*******************************************************************************************

>	BOOL FileInfo::UpdateFontList(Document*	WorkDoc)				  

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/11/95
	Inputs:		WorkDoc		pointer to the document we are working on, NULL if no document
	Outputs:	-
	Returns:	True if completed ok, False otherwise.
	Purpose:	Updates the font name list with the current list of fonts in the selected
				document. If no fonts are present then shows this by using a blank list and
				putting a suitable message in the top item. If the specified document is
				null then will just show a blank list with a dashed and greyed top item.
	Errors:		-
	SeeAlso:	SetDocInfo; FontDropDown; DocumentFontDropDown;

*******************************************************************************************/

BOOL FileInfo::UpdateFontList(Document*	WorkDoc)
{
//	CCustomList* pListGadget = CCustomList::GetGadget(GetReadWriteWindowID(), _R(IDC_FINFO_REFSLIST));
//	ERROR2IF(pListGadget == NULL, FALSE, "No list gadget?!");

	List ItemList;

	// Build the font list for the specified document
	// This will NOT be alphabetical
	FontListItem* FontItem = NULL;
	if (WorkDoc)
	{
		FontList DocFonts;
		DocFonts.Build(WorkDoc);
		FontItem = DocFonts.GetFirstItem();

		// fill up the list
		while (FontItem != NULL)
		{
			// get the name
			UINT32 idBitmap = 0;
			String_64 strName = FontItem->GetFontName();
			String_256 strDetails;
			
			WORD Handle = FONTMANAGER->GetFontHandle(&strName);

			// check the style
			INT32 Style = FontItem->GetFontStyle();
			if (Style & 1)
				strName += _T(" -Bold");
			if (Style & 2)
				strName += _T(" -Italic");
			
			if (Handle > 0)
			{
				if (FONTMANAGER->IsFontReplaced(Handle))
					strDetails = String(_R(IDS_FINFO_FONTNOTINSTALLED));
			}
			FontClass Type = FontItem->GetFontClass();
			switch(Type)
			{
				case FC_TRUETYPE:
					idBitmap = _R(IDB_TTF_SYMBOL);
					break;

				case FC_ATM:
					idBitmap = _R(IDB_ATM_SYMBOL);
					break;

				default:
					idBitmap = 0;
					break;
			}

			AddToSortedList(ItemList, idBitmap, strName, strDetails);
			
			FontItem = DocFonts.GetNextItem(FontItem);

			ContinueSlowJob();
		}
	}

	// Put the sorted items in the list gadget
	RefItem* pRefItem = (RefItem*)ItemList.GetHead();
	while (pRefItem)
	{
		// Add the font in the list to the list box
//		pListGadget->AddRefsItem(pRefItem->idBitmap, pRefItem->strItemName, pRefItem->strDetails);
		CFileInfoList::CListRow oNewRow = m_lstctrlExternalResInfo.AddRow();
		oNewRow.SetBitmap(pRefItem->idBitmap);
		oNewRow.SetText(1, pRefItem->strItemName);
		oNewRow.SetText(2, pRefItem->strDetails);
		

		pRefItem = (RefItem*)ItemList.GetNext(pRefItem);
	}

	ItemList.DeleteAll();

	return TRUE;
}


/*******************************************************************************************

>	BOOL FileInfo::UpdateEffectsList(Document*	pWorkDoc)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/2005
	Inputs:		WorkDoc		pointer to the document we are working on, NULL if no document
	Outputs:	-
	Returns:	True if completed ok, False otherwise.
	Purpose:	Updates the effects list showing referred and missing effects
	Errors:		-
	SeeAlso:	SetDocInfo; FontDropDown; DocumentFontDropDown;

*******************************************************************************************/

BOOL FileInfo::UpdateEffectsList(Document* pWorkDoc)
{
	// Make a list of all unique effects used in the doc
	// Add each list item to the listbox, marking it if the referred effect can not
	// be found in the installed effect list

//	CCustomList* pListGadget = CCustomList::GetGadget(GetReadWriteWindowID(), _R(IDC_FINFO_REFSLIST));
//	ERROR2IF(pListGadget == NULL, FALSE, "No list gadget?!");

	if (pWorkDoc == NULL)	// Don't try to fill in anything when the window is shaded
	{
		return TRUE;
	}

	List ItemList;

PORTNOTE("other", "Disabled bitmap effects")
#ifndef EXCLUDE_FROM_XARALX
	ListRange* pEffectList = pWorkDoc->GetEffectsList(500000);	// Arbitrary large number
	if (pEffectList)
	{
		// Now add listed items to the ListBox
		INT32 index = 0;
		Node* pNode = pEffectList->FindFirst();
		while (pNode)
		{
			ENSURE(pNode->IsBitmapEffect(), "How can this node not be a BitmapEffect?");

			if (pNode->IsBitmapEffect())
			{
				NodeBitmapEffect* pEffect = (NodeBitmapEffect*)pNode;

				String_64 strEffectName = pEffect->GetDisplayName();
				String_256 strDetails;
				UINT32 idBitmap = 0;

				if (pEffect->IsLockedEffect())
				{
					// Destructive/locked effects have their own bitmap stored in them
					// So we don't need to test whether their handler is installed on this machine
					String_64 strFoundDisplayName;
					BOOL bFound = XPEHost::GetEffectDetails(pEffect->GetPostProcessorID(), &strFoundDisplayName);
					if (bFound)
					{
						strDetails = String(_R(IDS_FINFO_EFFECTOK));
						idBitmap = _R(IDB_FINFO_OKEFFECT);
					}
					else
					{
						strDetails = String(_R(IDS_FINFO_LOCKEDEFFECT));
						idBitmap = _R(IDB_FINFO_LOCKEDEFFECT);
					}
				}
				else
				{
					// Live effects need to regenerate their bitmaps so we must
					// test whether the handler is present and warn the user if not
					String_64 strFoundDisplayName;
					BOOL bFound = XPEHost::GetEffectDetails(pEffect->GetPostProcessorID(), &strFoundDisplayName);
					if (bFound)
					{
						strDetails = String(_R(IDS_FINFO_EFFECTOK));
						idBitmap = _R(IDB_FINFO_OKEFFECT);
					}
					else
					{
						strDetails = String(_R(IDS_FINFO_NOTFOUND));
						strDetails += String(" [");
						strDetails += pEffect->GetPostProcessorID();
						strDetails += String("]");
						idBitmap = _R(IDB_FINFO_MISSINGEFFECT);
					}
				}
				AddToSortedList(ItemList, idBitmap, strEffectName, strDetails);

				ContinueSlowJob();
			}

			pNode = pEffectList->FindNext(pNode);
		}

		delete pEffectList;
		pEffectList = NULL;
	}
#endif
	// Put the sorted items in the list gadget
	RefItem* pRefItem = (RefItem*)ItemList.GetHead();
	while (pRefItem)
	{
		// Add the font in the list to the list box
//		pListGadget->AddRefsItem(pRefItem->idBitmap, pRefItem->strItemName, pRefItem->strDetails);
		CFileInfoList::CListRow oNewRow = m_lstctrlExternalResInfo.AddRow();
		oNewRow.SetBitmap(pRefItem->idBitmap);
		oNewRow.SetText(1, pRefItem->strItemName);
		oNewRow.SetText(2, pRefItem->strDetails);

		pRefItem = (RefItem*)ItemList.GetNext(pRefItem);
	}

	ItemList.DeleteAll();

	return TRUE;
}


/*******************************************************************************************

>	BOOL FileInfo::SetDocInfo(BOOL UpdateComment, BOOL UpdateFonts)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/08/94
	Inputs:		UpdateComment - TRUE if the comment field should be updated.  Normally
				FALSE for fast updates (otherwise the comment field looses newly entered
				text).
				UpdateFonts - TRUE if we want to update the font list avoid if possible 
				as this causes a rebuild of the font list and much flicker..
	Outputs:	-
	Returns:	-
	Purpose:	Sets the values in the fields of the document information dialogue.
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/
BOOL FileInfo::SetDocInfo(BOOL UpdateComment, BOOL UpdateFonts)
{
	Document*	WorkDoc = Document::GetSelected();

	if (UpdateFonts)
	{
		BeginSlowJob();
		GadgetRedraw(_R(IDC_FINFO_REFSLIST), FALSE);	// Disable redraw while updating

//		INT32 iSelectedItem = pListGadget->GetSelectedItemIndex();
//		INT32 iScrollPos = pListGadget->GetScrollPos(SB_VERT);
		
		m_lstctrlExternalResInfo.Clear();

		// Work out all the document based fonts again and recreate the drop down list
		CFileInfoList::CListRow oNewRow = m_lstctrlExternalResInfo.AddRow();
		oNewRow.SetText(1, String(_R(IDS_FINFO_FONTREFS_NAMEHEADER)));
		oNewRow.SetText(2, String(_R(IDS_FINFO_FONTREFS_DETAILSHEADER)));
		
		UpdateFontList(WorkDoc);

		// Show effects usage
//		pListGadget->AddRefsItem(0, String(_R(IDS_FINFO_EFFECTREFS_NAMEHEADER)), String(_R(IDS_FINFO_EFFECTREFS_DETAILSHEADER)));
		oNewRow = m_lstctrlExternalResInfo.AddRow();
		oNewRow.SetText(1, String(_R(IDS_FINFO_EFFECTREFS_NAMEHEADER)));
		oNewRow.SetText(2, String(_R(IDS_FINFO_EFFECTREFS_DETAILSHEADER)));
		
		UpdateEffectsList(WorkDoc);

//		pListGadget->SetSelectedItemIndex(iSelectedItem);
//		pListGadget->SetScrollPos(SB_VERT, iScrollPos, FALSE);

		GadgetRedraw(_R(IDC_FINFO_REFSLIST), TRUE);		// Enable redraw while updating
		InvalidateGadget(_R(IDC_FINFO_REFSLIST));		// Force redraw now that we're done
		EndSlowJob();
	}

	// If there is no selected document then we want to set the fields to be blank 
	if (WorkDoc == NULL)
	{
		String_8	DashString(_R(IDS_K_FINFODLG_DASH));
		String_8	NullString = _T("");
		UpdateStringGadgetValue(_R(IDC_FILEINFO_UNDOSIZE),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_UNDOSTEPS),		&DashString);
//		UpdateStringGadgetValue(_R(IDC_FILEINFO_SPREADS),			&DashString);
//		UpdateStringGadgetValue(_R(IDC_FILEINFO_PAGES),			&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_NUMOBJECTS),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_NUMSELOBJECTS),	&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_LOCATION),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_FILENAME),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_FILESIZE),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_NUMBITMAPS),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_BITMAPSIZE),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_FRACTALS),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_FRACTALSIZE),		&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_CREATIONDATE),	&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_LASTSAVEDATE),	&DashString);
		UpdateStringGadgetValue(_R(IDC_FILEINFO_COMMENT),			&NullString);
		return TRUE;
	}

	// Declare the general strings we use throughout
  	String_256	String;
  	String_256	String2;

	// Keep a list of the bitmaps that we have found in our scan so that we do not
	// count the same bitmap twice
	BitmapList.DeleteAll();

	// --
	// -- Display the undo and redo steps for the document
	// --

	// Now calculate the size of the undo buffer and if any records have been discarded
	Convert::BytesToString(&String, WorkDoc->GetOpHistory().GetSize());
	if (WorkDoc->GetOpHistory().IsReduced())
	{
		String2.MakeMsg(_R(IDS_FILEINFO_REDUCED));
		String += String2;
	}
	UpdateStringGadgetValue(_R(IDC_FILEINFO_UNDOSIZE), &String);

	// Now calculate the number of undo and redo steps
	UINT32	UndoSteps = WorkDoc->GetOpHistory().GetNumUndoSteps();
	UINT32	RedoSteps = WorkDoc->GetOpHistory().GetNumRedoSteps();
	String = "";
	if (UndoSteps != 0)
	{
		String.MakeMsg(_R(IDS_FILEINFO_UNDOSTEPS), UndoSteps);
		if (UndoSteps == 1)
			String2.MakeMsg(_R(IDS_FILEINFO_STEP));
		else
			String2.MakeMsg(_R(IDS_FILEINFO_STEPS));
		String += String2;
	}
	if (RedoSteps != 0)
	{
		String_256	String3;
		String2.MakeMsg(_R(IDS_FILEINFO_REDOSTEPS), RedoSteps); 
		if (String.CompareTo(_T("")) != 0)
			String += String_64(_R(IDS_K_FINFODLG_SEPERATOR));
		if (RedoSteps == 1)
			String3.MakeMsg(_R(IDS_FILEINFO_STEP));
		else
			String3.MakeMsg(_R(IDS_FILEINFO_STEPS));
		String += String2;
		String += String3;
	}
	if (String.CompareTo(_T("")) == 0)
	{
		String.MakeMsg(_R(IDS_FILEINFO_NOSTEPS)); 
		UpdateStringGadgetValue(_R(IDC_FILEINFO_UNDOSTEPS), &String);
	}
	else
	{
		UpdateStringGadgetValue(_R(IDC_FILEINFO_UNDOSTEPS), &String);
	}

	// --
	// -- Display the comment field for the current document
	// --

	// Update the comment field if we should
	if (UpdateComment)
	{
		String = WorkDoc->GetComment();
		UpdateStringGadgetValue(_R(IDC_FILEINFO_COMMENT), &String);
	}

	// Now scan the document for the various object counts
	BitmapInfo	BitInfo;
	Node*	CurrentNode = StartTreeScan(WorkDoc);
	UINT32	Spreads = 0;						  
	UINT32	Pages = 0;
	UINT32	Bitmaps = 0; 
	UINT32	BitmapSize = 0;
	UINT32	Objects = 0;
	UINT32	SelObjects = 0;
	UINT32 	Groups = 0;
	UINT32	Size = 0;
	BOOL	DontGetNext = TRUE;

	while (CurrentNode != NULL)
	{
		Size += CurrentNode->GetNodeSize();
		DontGetNext = TRUE;

		if (CurrentNode->IsAnObject())
		{			
			// If it's a renderable object then count it.
			Objects ++;

			// This is silly as there is already a function to do this (SelRange::Count)
			// and using this method we won't be counting selected inside items.
			//if (CurrentNode->IsSelected())
			//{
			//	SelObjects++;
			//}

			if (CurrentNode->IS_KIND_OF(NodeGroup) || IS_A(CurrentNode,TextStory))
			{	// Count objects inside groups, moulds and blends as one object
				// as these are renderable and would otherwise be included in the count.
				// Need to do the same with TextStories as these contain the renderable
				// text lines and text characters ...
				// Don't want to count objects inside groups (or blends) so sidestep
				Size += CurrentNode->GetSubtreeSize();
				CurrentNode = SkipSubtreeScan(CurrentNode);
				DontGetNext = FALSE;
				// Increment our group count if this is a real group rather than a blend
				// or a mould or a text story. 
				if (IS_A(CurrentNode,NodeGroup))
					Groups++;
			}
			else
			{
				//// If it's a renderable object then count it.
				//// But objects like blends and moulds are renderable but shouldn't be
				//// counted as they are not true objects.
				//Objects ++;
				
				if (IS_A(CurrentNode,NodeBitmap))
				{	// We've found a bitmap, so we need to see whether it is in our list of
					// previously found bitmaps.  If we have found it before then we can
					// ignore it.  Otherwise, we add it to the list and add its size to our
					// size count.
					Bitmaps ++;
					NodeListItem* CurrentBitmapNode = (NodeListItem*)BitmapList.GetHead();
					BOOL Found = FALSE;

					while (CurrentBitmapNode != NULL)
					{
						if ((((NodeBitmap*)CurrentBitmapNode->pNode)->GetBitmap()->ActualBitmap) == 
												(((NodeBitmap*)CurrentNode)->GetBitmap()->ActualBitmap))
						{
							Found = TRUE;
						}
						CurrentBitmapNode = (NodeListItem*)BitmapList.GetNext(CurrentBitmapNode);
					}
					if (!Found)
					{
						((NodeBitmap*)CurrentNode)->GetBitmap()->ActualBitmap->GetInfo(&BitInfo);
						BitmapSize += BitInfo.MemoryUsed;
						CurrentBitmapNode = new NodeListItem(CurrentNode);
						BitmapList.AddHead(CurrentBitmapNode);
					}
				}
			}
		}
		else
		{	// Increment specific non-renderable object counts.
			if (IS_A(CurrentNode,Spread))
			{
				Spreads ++;
			}
			else
			{
				if (IS_A(CurrentNode,Page))
				{
					Pages ++;
				}
			}
		}
		if ((CurrentNode != NULL) && DontGetNext)
		{
			CurrentNode = GetNextTreeNode(CurrentNode);
		}																 
	}

	// Now work out the number of objects that are currently selected
	// Find the application and hence the selection range
	Application* pApplication = GetApplication();
	if (pApplication)
	{
		// First, check if there is a selection present. ClipRect should be empty if not.
		SelRange* pSelection = pApplication->FindSelection();
		// If there is a selection then count it
		if (pSelection)
		{
			SelObjects = pSelection->Count();
		}
	}

	// --
	// -- Display number of pages and number of spreads in the document
	// --

// No longer required as only one spread in version 1.0
//	String._MakeMsg("#1%lu", Spreads);
//	UpdateStringGadgetValue(_R(IDC_FILEINFO_SPREADS), &String);
//	String._MakeMsg("#1%lu", Pages);
//	UpdateStringGadgetValue(_R(IDC_FILEINFO_PAGES), &String);

	// --
	// -- Display number of objects and number of selected objects
	// --

	// Show some of the information that we found
	String._MakeMsg(_T("#1%lu"), Objects);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_NUMOBJECTS), &String);
	String._MakeMsg(_T("#1%lu"), SelObjects);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_NUMSELOBJECTS), &String);

	// --
	// -- Display count and size of fractals in the document
	// --

	// Show the current number of fractals and bitmaps in this document 
	// First the fractals.
	Application * Camelot = GetApplication();	
	EnumFractalData FracData;
	if (Camelot)
	{
		GlobalFractalList * FracList = Camelot->GetGlobalFractalList();
		if (FracList)
			FracList->GetDocumentFractalData(WorkDoc,&FracData);
	}
	String._MakeMsg(_T("#1%lu"), FracData.Count);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_FRACTALS), &String);
	Convert::BytesToString(&String, FracData.Size);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_FRACTALSIZE), &String);

	// --
	// -- Display count and size of bitmaps in the document
	// --

	INT32 BitmapsCount = 0;
	INT32 BitmapsSize = 0;
	if (Camelot)
	{
		GlobalBitmapList * BmpList = Camelot->GetGlobalBitmapList();
		if (BmpList)
		{
			BitmapsCount = BmpList->GetDocumentBitmapCount(WorkDoc);
			BitmapsSize = BmpList->GetDocumentBitmapSize(WorkDoc);
		}
	}
	//String._MakeMsg("#1%lu", Bitmaps);
	String._MakeMsg(_T("#1%lu"), BitmapsCount);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_NUMBITMAPS), &String);
	//BytesToString(&String, BitmapSize);
	Convert::BytesToString(&String, BitmapsSize);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_BITMAPSIZE), &String);

	// Now show our final size 
	// Add in the number of bitmaps that we found as these will be saved with the
	// document. Don't include fractals as these are regenerated on loading. 
	// Current thinking seems to be that we should show the fractals as well as this
	// field is now memory used.
	Size += BitmapsSize;
	Size += FracData.Size;
	Convert::BytesToString(&String, Size);
	UpdateStringGadgetValue(_R(IDC_FILEINFO_FILESIZE), &String);

	// --
	// -- Display Location and Filename of document
	// --

	// Show the current pathname for the document
	// The number give sthe size that we have to fit the pathname into.
	String.Empty();
	//String = WorkDoc->GetPathName(45);
	String = WorkDoc->GetLocation(41);
	if ( String.IsEmpty() )
	{
		// No pathname yet, so put in a nice message
		String.MakeMsg(_R(IDS_FILEINFO_UNSAVED));
	}
	UpdateStringGadgetValue(_R(IDC_FILEINFO_LOCATION), &String);

	// Now show the current filename
	String.Empty();
	String = WorkDoc->GetTitle();
	UpdateStringGadgetValue(_R(IDC_FILEINFO_FILENAME), &String);

	// --
	// -- Display Creation and Save times for the document
	// --

	// Now do the creation and save times
	time_t	TimeData;
	TimeData = WorkDoc->GetCreationTime();
			
	String = asctime(localtime(&TimeData));
	
//	LocalEnvironment::SystemTimeToString(&String, &TimeData);
//	LocalEnvironment::SystemDateToString(&String2, &TimeData);
//	String += _T("  ");
//	String += String2;
	UpdateStringGadgetValue(_R(IDC_FILEINFO_CREATIONDATE), &String);

	TimeData = WorkDoc->GetLastSaveTime();
	if (TimeData != 0)
	{
//		LocalEnvironment::SystemTimeToString(&String, &TimeData);
//		LocalEnvironment::SystemDateToString(&String2, &TimeData);
//		String += _T("  ");
//		String += String2;
		String = asctime(localtime(&TimeData));
		
		UpdateStringGadgetValue(_R(IDC_FILEINFO_LASTSAVEDATE), &String);
	}
	else
	{
		String.MakeMsg(_R(IDS_FILEINFO_UNSAVED));
		UpdateStringGadgetValue(_R(IDC_FILEINFO_LASTSAVEDATE), &String);
	}

#ifdef STANDALONE
	EnableGadget(_R(IDC_FILEINFO_COMMENT),FALSE);
#endif

	return TRUE;
}
					

/*******************************************************************************************
>	Node* StartTreeScan(Document* pDoc);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31th August 1994
	Inputs:		Pointer to the document to scan.
	Returns:	Pointer to the first node.
	Purpose:	To scan a tree "not depth first".  Instead of skipping to the bottom of
				the tree and scanning back up I want to scan to the bottom and skip
				back up.  Initialises the scan of the document.
	See also:	GetNextTreeNode
*******************************************************************************************/
Node* StartTreeScan(Document* pDoc)
{
	if (pDoc == NULL)		   
	{
		ENSURE(FALSE,"NULL document pointer passed to StartTreeScan");
		return NULL;
	}
	return pDoc->GetFirstNode();
}	
							 

										
/*******************************************************************************************
>	Node* GetNextTreeNode(Node* Current);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31th August 1994
	Inputs:		The current node. 
	Returns:	Pointer to the next node.
	Purpose:	To scan a tree "not depth first".  Instead of skipping to the bottom of
				the tree and scanning back up I want to scan to the bottom and skip
				back up.  Gets the node after the current one.  Returns NULL if no more
	See also:	StartTreeScan, SkipSubTreeScan
*******************************************************************************************/
Node* GetNextTreeNode(Node* Current)
{
	if (Current == NULL)
	{
		ENSURE(FALSE,"NULL node pointer passed to GetNextTreeNode");
		return NULL;
	}
	// first try going down one level
	if (Current->FindFirstChild() != NULL)
	{
		return Current->FindFirstChild();
	}
	// now try going right one
	if (Current->FindNext() != NULL)
	{
		return Current->FindNext();
	}
	// if we get this far we need to go up and to the right.
 	Node* Temp = Current->FindParent();;
	while (Temp != NULL)
	{
		if (Temp->FindNext() != NULL)
		{
			return Temp->FindNext();
		}
		Temp = Temp->FindParent();
	}
	// no more nodes left in the document
	return NULL;
}	



/*******************************************************************************************
>	Node* SkipSubtreeScan(Node* Current);	
	
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31th August 1994
	Inputs:		The current node. 
	Returns:	Pointer to the next node.
	Purpose:	To skip the scan of the children of the current node.  The node returned
				is either a sibling or a parent of the current node.  Can return NULL.
	See also:	GetNextTreeNode
*******************************************************************************************/
Node* SkipSubtreeScan(Node* Current)
{
	if (Current == NULL)
	{
		ENSURE(FALSE,"NULL node pointer passed to SkipSubTreeScan");
		return NULL;
	}
	if (Current->FindNext() != NULL)
	{
		return Current->FindNext();
	}

	// now we need to go up and to the right.
 	Node* Temp = Current->FindParent();
	while (Temp != NULL)
	{
		if (Temp->FindNext() != NULL)
		{
			return Temp->FindNext();
		}
		Temp = Temp->FindParent();
	}
	return NULL;
}	






/*******************************************************************************************

>	BOOL FileInfo::AddToSortedList(List& ItemList, UINT32 idBitmap, String_64& strItemName, String_256& strDetails)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/01/2005
	Inputs:		List
				Bitmap ID
				Item name
				Item details
	Outputs:	-
	Returns:	-
	Purpose:	Create a new item in the list, insertion sorted alphabetically by item name
	Errors:		-
	SeeAlso:	-

*******************************************************************************************/

BOOL FileInfo::AddToSortedList(List& ItemList, UINT32 idBitmap, String_64& strItemName, String_256& strDetails)
{
	RefItem* pNewItem = new RefItem(idBitmap, strItemName, strDetails);
	if (pNewItem == NULL)
	{
		ERROR3("AddRefsItem couldn't create Item");
		return FALSE;
	}

	// Add the item into the list, in a sorted fashion...
	RefItem *pTheItem = (RefItem*)ItemList.GetHead();

	if (pTheItem == NULL)
	{
		ItemList.AddTail(pNewItem);
		return TRUE;
	}
	
	if (pTheItem->strItemName > strItemName)
	{
		ItemList.InsertBefore(pTheItem, pNewItem);
		return TRUE;
	}

	while (pTheItem != NULL)
	{
		RefItem* pTheNextItem = (RefItem*)ItemList.GetNext(pTheItem);

		if (pTheNextItem == NULL)
		{
			ItemList.InsertAfter(pTheItem, pNewItem);
			return TRUE;
		}

		if (pTheNextItem->strItemName > strItemName)
		{
			ItemList.InsertBefore(pTheNextItem, pNewItem);
			return TRUE;
		}

		// Try the next item
		pTheItem = pTheNextItem;
	}

	// Should never get here!
	ERROR3("Why are we here?");
	return FALSE;
}





