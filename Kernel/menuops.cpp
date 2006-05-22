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

/********************************************************************************************

	menuops.cpp - This module provides a set of menu Operation classes which are used in the
	OIL Menu Module when the Menu System is being built. 

********************************************************************************************/


#include "camtypes.h"

//#import "WebFileUpdater_1_1.dll"
//using namespace WEBFILEUPDATERLib;

//#import "XMUpload_1_3.dll"
#define	E_XM_CONNECTCANCELLED	( 0x80041000 )
//using namespace XMUPLOADLib;


#include "camelot.h"
//#include "mainfrm.h"
#include "document.h"
//#include "camdoctp.h"
#include "camdoc.h"

#include "menuops.h"
#include "menucmds.h"
#include "ensure.h"
#include "errors.h"
#include "tool.h"
#include "toollist.h"
#include "selop.h"

//#include "mario.h"
#include "helpids.h"
#include "bubbleid.h"
//#include "barsdlgs.h"
#include "dlgmgr.h"
#include "combshps.h"
//#include "markn.h"
#include "shapeops.h"
//#include "peter.h"
//#include "will.h"
//#include "justin.h"
#include "app.h"
#include "csrstack.h"
//#include "tooldlg.h"
#include "progress.h"
//#include "ollie.h"
//#include "justin3.h"
//#include "camnet.h"

//#include "resimmap.h"	//For new template stuff
#include "sgliboil.h"	//For file exists function
#include "fileutil.h"	//For file util function
//#include "rik.h"		//For _R(IDS_DEFAULTDOCNAME)

#include "group.h"		// for blends
//#include "simon.h"		// for the sub menu stuff

//#include "webdialog.h"	// specials/update dialogs
//#include "webdialogdlg.h"	// specials/update dialogs
//#include "registerdialog.h"	// register dialog
//#include "resdll.h"			// resource / locale methods
#include "product.h"
//#include "helpdownload.h"
//#include "phil.h"
//#include "registry.h"
//#include "register.h"
//#include "liveeffectres.h"


#if (_OLE_VER >= 0x200)
	#include "srvritem.h"
#endif

//#ifdef WEBSTER
#define INC_WEB_MENU_ITEMS
//#endif // WEBSTER

//Webster_Ranbir_12\11\96
#ifdef INC_WEB_MENU_ITEMS
BOOL HelpOps::CDHelpOn = FALSE;
#endif //INC_WEB_MENU_ITEMS

BOOL ViewOps::DoFullScreen = FALSE;

#if !defined(EXCLUDE_FROM_RALPH)

/******************************************************************************************

>       void DUMMYOP(TEXT,TOKEN)

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        29/03/94
	Purpose:        MACRO
				Construct an OpDescriptor for a menu item which does not do anything.
				I.E. It's just a way of creating menu items before they have any
				Operations to connect to...
				
******************************************************************************************/

#define DUMMYOP(TEXT,TOKEN) {\
	BOOL blobby = RegisterOpDescriptor(\
										0,\
										TEXT,\
										CC_RUNTIME_CLASS(DummyOp),\
										TOKEN,\
										DummyOp::GetState,\
										0,      /* help ID */\
										0,  /* bubble help ID */\
										0,      /* resource ID */\
										0       /* control ID */\
										);\
	ERRORIF(!blobby, _R(IDE_NOMORE_MEMORY), FALSE);\
	}




/******************************************************************************************

>       void REGOP(MENU,NAME,CLASS)

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        20/04/94
	Purpose:        MACRO
				DON'T USE THIS MACRO! Use either BTNOP or CTRLOP from opdesc.h instead.
				Construct a complete OpDescriptor given three simple parameters. The
				parameters are used to build all the various resource IDs required by the
				call to RegisterOpDescriptor. Each use of this macro replaces the following
				code;
				
				BOOL Blobby = RegisterOpDescriptor(
													0, 
													_R(IDS_FILE_OPEN),
													CC_RUNTIME_CLASS(DocOps), 
													OPTOKEN_FILEOPEN,
													DocOps::GetState,
													HID_FILE_OPEN,
													_R(IDBBL_FILEOPENOP),
													_R(IDD_BARCONTROLSTORE),
													_R(IDC_BTN_FILEOPEN),
													SYSTEMBAR_FILE,
													TRUE // ReceiveMessages
													);
				ERRORIF(!Blobby, _R(IDE_NOMORE_MEMORY), FALSE);
	SeeAlso:        BTNOP, CTRLOP

******************************************************************************************/

#define REGOP(MENU,NAME,OPCLASS)\
	{\
		BOOL Blobby = RegisterOpDescriptor(\
						0,\
						_R(IDS_ ## MENU ## _ ## NAME),		/*NORESOURCEFIX*/ \
						CC_RUNTIME_CLASS(OPCLASS),\
						OPTOKEN_ ## MENU ## NAME,\
						OPCLASS::GetState,\
						HID_ ## MENU ## _ ## NAME,\
						_R(IDBBL_ ## MENU ## NAME ## OP),	/*NORESOURCEFIX*/ \
						_R(IDD_BARCONTROLSTORE),\
						_R(IDC_BTN_ ## MENU ## NAME)		/*NORESOURCEFIX*/,\
						SYSTEMBAR_ ## MENU,\
						TRUE\
					  );\
		ERRORIF(!Blobby, _R(IDE_NOMORE_MEMORY), FALSE);\
	}

/********************************************************************************************

	Preference:		DefaultAnimationTemplate
	Section:		Templates
	Range:			String
	Purpose:		The path to the default animation template. By default, this
					is the EXE path with "\templates\animation.xar" added to the end
	SeeAlso:		-

********************************************************************************************/

String_256 DocOps::ms_strDefaultAnimationTemplate = _T("");

/********************************************************************************************

	Preference:		DefaultDrawingTemplate
	Section:		Templates
	Range:			String
	Purpose:		The path to the default drawing template. By default, this
					is the EXE path with "\templates\drawing.xar" added to the end
	SeeAlso:		-

********************************************************************************************/

String_256 DocOps::ms_strDefaultDrawingTemplate = _T("");



CC_IMPLEMENT_DYNCREATE  (DocOps,                Operation)
CC_IMPLEMENT_DYNCREATE  (MenuPopupOps,  Operation)
CC_IMPLEMENT_DYNCREATE  (HelpOps,               Operation)
CC_IMPLEMENT_DYNCREATE  (WindowOps,             Operation)
CC_IMPLEMENT_DYNCREATE  (ViewOps,               Operation)
CC_IMPLEMENT_DYNCREATE  (OpExit,                Operation)
CC_IMPLEMENT_DYNCREATE  (OpUpdate,              Operation)
CC_IMPLEMENT_DYNCREATE  (OpRegister,            Operation)
CC_IMPLEMENT_DYNCREATE  (DummyOp,               Operation)
#endif

CC_IMPLEMENT_DYNCREATE  (OpToolSelect,  Operation)


//********************************************************************************************
//                                                                      TOOL SELECTION OPERATION
/********************************************************************************************

>       OpToolSelect::OpToolSelect(): Operation()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        20/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new OpToolSelect object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
OpToolSelect::OpToolSelect(): Operation()
{                 
}


/********************************************************************************************

>       BOOL OpToolSelect::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        16/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create OpDescriptors for each of the Tools available
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL OpToolSelect::Init()
{
	// Get the first tool in the tool list
	ToolListItem* ToolItem = Tool::GetFirstTool();

	ToolInfo *Info = new ToolInfo;
	if (Info == NULL)
		return FALSE;

    while (ToolItem != NULL)
    {
		Tool* ATool = ToolItem->m_pTool;
		
		//Get Tool Info
		Info->BubbleID=0;
		ATool->Describe(Info);
	
	// Set up OpToken
	String OpToken;
	camSnprintf( OpToken, 16, _T("TOOL%u"), (Info->ID) );
	
	// Create an instance of the ToolOpDescriptor
		ToolOpDescriptor* ToolOp = new ToolOpDescriptor(
														Info->ID,
														Info->TextID,
														CC_RUNTIME_CLASS(OpToolSelect), 
														OpToken,
														OpToolSelect::GetState,
														0,                                              /* Help ID */
														Info->BubbleID
														);

		ERRORIF(!ToolOp, _R(IDE_NOMORE_MEMORY), FALSE);
		
		//Get the next item in the tool list
		ToolItem = Tool::GetNextTool(ToolItem);
    }

	delete Info;

	return TRUE;
}

/********************************************************************************************

>       OpState OpToolSelect::GetState(String_256* UIDescription, OpDescriptor*  ToolOp)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        16/8/93
	Inputs:         String* pointer to a string description of menu item
				OpDescriptor* that describes a Tool
	Outputs:        None
	Returns:        OpState describing the state of the Tool.
	Purpose:        Is called to update the state of tool represented by
				OpDescriptor.
	Errors:         None

********************************************************************************************/

OpState OpToolSelect::GetState(String_256* UIDescription, OpDescriptor* ToolOp)
{ 
	OpState mState;
	
	UINT32 ToolId = ToolOp->GetToolID();

	ENSURE(Tool::Find(ToolId) != NULL, "Tool Id does not exist!");
		
    Tool* thisTool = ((Tool::Find(ToolId))->m_pTool);
    
    if (thisTool ==((Tool::Find(ToolId))->m_pTool)->GetCurrent())
	mState.Ticked = TRUE;
    else
	mState.Ticked = FALSE;

	mState.Greyed = FALSE;
	
	return mState; 
}

/********************************************************************************************

>       void OpToolSelect::Do(OpDescriptor* ToolOp)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        16/8/93
	Inputs:         OpDescriptor representing a Tool
	Purpose:        Sets the currently active tool based on the tool OpDescriptor passed in.
	Errors:         None


********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void OpToolSelect::Do(OpDescriptor* ToolOp)
{
	UINT32 ToolId = ToolOp->GetToolID();

	ENSURE(Tool::Find(ToolId) != NULL, "Tool Id does not exist!");
	ToolListItem * pItem = NULL;
	pItem = Tool::Find(ToolId);
	if(pItem)
	  pItem->m_pTool->SetCurrent();
	
	//((Tool::Find(ToolId))->pTool)->SetCurrent();
	
	End();
}

#if !defined(EXCLUDE_FROM_RALPH)


//*****************************************************************************************
//                                                                      EXIT OPERATION

/********************************************************************************************

>       OpState DocOps::GetState(String_256* UIDescription, OpDescriptor* pOp)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Outputs:        -
	Returns:        -
	Purpose:        Find the state of the various DocOps operations.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

OpState DocOps::GetState(String_256* UIDescription, OpDescriptor* pOp)
{
	// File/New and File/Open are always available.
	if (pOp->Token == String(OPTOKEN_FILENEW) ||
		pOp->Token == String(OPTOKEN_FILEOPEN))
	{
		return OpState(FALSE, FALSE);
	}

	//Graham 20/10/97: New template commands
	if (pOp->Token == String(OPTOKEN_FILENEW_DRAWING))
	{
		//We need to change the name of the command to the name of the
		//default template file, converted so the first character
		//is upper case and the rest is lower
		PathName pathDefaultDrawingTemplate=GetDefaultDrawingTemplate();

		String_256 strToReturn=pathDefaultDrawingTemplate.GetFileName(FALSE);

		if (strToReturn.IsEmpty())
			strToReturn.Load(_R(IDS_NEWTEMPLATES_DEFAULTNAMEFORDRAWINGMENUITEM));

		strToReturn.toTitle();

		*UIDescription=strToReturn;
		
		return OpState(FALSE, FALSE);
	}

	if (pOp->Token == String(OPTOKEN_FILENEW_ANIMATION))
		
	{
		//Does the default animation template exist?
PORTNOTE("other", "Remove template existance check - too annoying while it's not there!")
#ifndef EXCLUDE_FROM_XARALX
		PathName pathDefaultAnimationTemplate=GetDefaultAnimationTemplate();
		BOOL fFileExists=SGLibOil::FileExists(&pathDefaultAnimationTemplate);

		String_256 strToReturn=pathDefaultAnimationTemplate.GetFileName(FALSE);

		if (strToReturn.IsEmpty())
			strToReturn.Load(_R(IDS_NEWTEMPLATES_DEFAULTNAMEFORANIMATIONMENUITEM));

		strToReturn.toTitle();

		*UIDescription=strToReturn;
#else
		BOOL fFileExists = FALSE;
#endif

		//If not, grey the menu item
		return OpState(FALSE, !fFileExists);
	}

	if (pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE1) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE2) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE3) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE4) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE5) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE6) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE7) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE8) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE9) ||
		pOp->Token == String(OPTOKEN_FILENEW_TEMPLATE10))
	{
		//First find the default template path
		Application* pApplication=GetApplication();

		PathName pathTemplates=pApplication->GetTemplatesPath();

		//Now search that path for templates
		//Start by setting the leaf name to *.xar
		String_256 strSearchFilename(_R(IDS_NEWTEMPLATES_DEFAULTTEMPLATEEXTENSION));

		pathTemplates.SetFileNameAndType(strSearchFilename);

		//Now find out which template in that path we want
		//(This code isn't particularly pleasant)
		String_256 strNumberOfTemplate;

		INT32 iPositionOfFCharacter=pOp->Token.FindNextChar( 'F', 0 );
		TRACEUSER( "jlh92", _T("Token = %s(%d)\n"), PCTSTR(pOp->Token), iPositionOfFCharacter );
		
		pOp->Token.Left(&strNumberOfTemplate, iPositionOfFCharacter);

		TCHAR* pszTmp;
		INT32 iNumberOfTemplate = camStrtol( (TCHAR *)strNumberOfTemplate, &pszTmp, 10 );

		//And search the path for xar files that are
		//NOT the default animation or drawing templates
		String_256 strTemplates=pathTemplates.GetPath(FALSE);
		if (FileUtil::StartFindingFiles(&strTemplates))
		{
			String_256 strNameOfFile;
			PathName pathOfFile=pathTemplates;

			TRACEUSER( "jlh92", _T("SFF = true\n") );

			String_256 strPathOfDrawingTemplate=GetDefaultDrawingTemplate().GetPath(FALSE);
			String_256 strPathOfAnimationTemplate=GetDefaultAnimationTemplate().GetPath(FALSE);
			String_256 strPathOfFile;

			TRACEUSER( "jlh92", _T("DefPath = %s, %s\n"), PCTSTR(strPathOfDrawingTemplate),
				PCTSTR(strPathOfAnimationTemplate) );

			std::set<String_256>	setSortFilename;

			while( FileUtil::FindNextFile( &strNameOfFile ) )
			{
				pathOfFile.SetFileNameAndType(strNameOfFile);
				strPathOfFile=pathOfFile.GetFileName(TRUE);

				if( 0 != strPathOfFile.CompareTo( strPathOfDrawingTemplate, FALSE ) &&
					0 != strPathOfFile.CompareTo( strPathOfAnimationTemplate, FALSE ) )
				{
					setSortFilename.insert( strPathOfFile );
					TRACEUSER( "jlh92", _T("Curr = %s\n"), PCTSTR(strPathOfFile) );
				}
			}
			FileUtil::StopFindingFiles();

			if( iNumberOfTemplate >= setSortFilename.size() )
			{
				// Don't allow any errors set while searching to propagate outside this scope
				Error::ClearError();
				return OpState(FALSE, FALSE, TRUE);
			}

			std::set<String_256>::iterator iter = setSortFilename.begin();
			for( INT32 i = 1; i < iNumberOfTemplate; ++i, ++iter )
			{ /*Do nothing!*/ }

			strNameOfFile = *iter;
			TRACEUSER( "jlh92", _T("Final(%d) = %s\n"), iNumberOfTemplate, PCTSTR(strPathOfFile) );

			//We've found a file. So strip the .xar from the name, as follows...
			pathTemplates.SetFileNameAndType(strNameOfFile);
			*UIDescription=pathTemplates.GetFileName(FALSE);

			UIDescription->toTitle();

			// Don't allow any errors set while searching to propagate outside this scope
			Error::ClearError();

			return OpState(FALSE, FALSE, FALSE);
		}

		// Don't allow any errors set while searching to propagate outside this scope
		Error::ClearError();
	}
	
	// File/SaveAll is only available if there is a document that is "dirty".
	if (pOp->Token == String(OPTOKEN_FILESAVEALL))
	{
		// Are any open non-embedded document's modified?
		wxDocManager   *pDocManager = AfxGetApp().GetDocumentManager();
		wxList		   &lstDocuemnt = pDocManager->GetDocuments();
		wxNode		   *pNode = lstDocuemnt.GetFirst();			
		while( NULL != pNode )
		{
			CCamDoc	   *pDoc = (CCamDoc *)pNode->GetData();

PORTNOTE("other","Removed OLE usage")
#ifndef EXCLUDE_FROM_XARALX
#if (_OLE_VER >= 0x200)
			// Don't consider embedded documents for file-saving commands.
			if (pDoc->IsEmbedded()) continue;
#endif
#endif
	
			// Is this document "dirty" (and not embedded)?
			if( pDoc->IsModified() )
				return OpState(FALSE, FALSE);

			// Get the next document template.
			pNode = pNode->GetNext();
		}

		// If we get here there are no dirty documents.
		UIDescription->Load(_R(IDS_FILE_SAVE_ALL_NONE_CHANGED_GREYED));
		return OpState(FALSE, TRUE);
	}

	// Get the selected document(s).
	Document* pKernelDoc = Document::GetSelected();
	CCamDoc* pOilDoc = (pKernelDoc) ? pKernelDoc->GetOilDoc() : 0;
	
	// If there isn't one then none of the other File options are available.
	if (!pOilDoc)
	{
		// This knocks out the rest when there are no docs open.
		UIDescription->Load(_R(IDS_FILE_NO_DOC_GREYED));
		return OpState(FALSE, TRUE);
	}

	// Work out the state of File/Save.
	if (pOp->Token == String(OPTOKEN_FILESAVE))
	{
		// By default this is available.
		BOOL fGreyed = FALSE;
		pOp->EnableHotKey();
#if (_OLE_VER >= 0x200)
		// If we're embedding a document, replace "Save..." with "Update..." and disable that HotKey
		if (pOilDoc->IsEmbedded())
		{
			UIDescription->MakeMsg(_R(IDS_FILE_UPDATE), (LPCTSTR) pOilDoc->GetContainerName());
			pOp->DisableHotKey();
		}
		else
#endif
		// Disabled because we're not modified?
		if (!pOilDoc->IsModified())
		{
			fGreyed = TRUE;
			UIDescription->Load(_R(IDS_FILE_SAVE_NOT_CHANGED_GREYED));
		}

		// Return state.
		return OpState(FALSE, fGreyed);
	}

	// Work out the state of File/Close.
	if (pOp->Token == String(OPTOKEN_FILECLOSE))
	{
		// By default this is available.
		BOOL fGreyed = FALSE;

	#if (_OLE_VER >= 0x200)
		// Work out what to show for OLE server File/Close.
		if (pOilDoc->IsEmbedded())
		{
			// The doc is embedded.  If we are in server mode then the Close
			// option should be greyed.  If we aren't in server mode then
			// change the text to "Close And Return To ...".
			if (!AfxOleGetUserCtrl())
			{
				// In server mode.  Disable the menu option.
				fGreyed = TRUE;
				UIDescription->Load(_R(IDS_FILE_CLOSE_SERVER_MODE_GREYED));
			}
			else
			{
				// User control.  Enable the option and change the text of the
				// Close option to show return.
				UIDescription->MakeMsg(_R(IDS_FILE_CLOSE_AND_RETURN),
									   (LPCTSTR) pOilDoc->GetContainerName());
			}
		}
	#endif

		// Not embeddding - use the default text for the menu option.
		return OpState(FALSE, fGreyed);
	}
	
	// Work out File/SaveAs (SaveCopyAs), which is always available (if there's a
	// selected document).
#if (_OLE_VER >= 0x200)
	if (pOp->Token == String(OPTOKEN_FILESAVEAS))
	{
		// If we are enabled and embedding then change the menu to Save Copy As.
		if (pOilDoc->IsEmbedded()) UIDescription->Load(_R(IDS_FILE_SAVE_COPY_AS));
	}
#endif

	// If we get here we're available.
	return OpState(FALSE, FALSE);
}                                    



/********************************************************************************************
>       OpState OpExit::GetState(String_256* UIDescription, OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Returns:        The OpState describing this Exit Operation.
	Purpose:        Find the state of the OpExit operation.
********************************************************************************************/

OpState OpExit::GetState(String_256* UIDescription, OpDescriptor*)
{
PORTNOTE("other","Removed OLE usage")
#ifndef EXCLUDE_FROM_XARALX
#if (_OLE_VER >= 0x200)

	// Get the user's document, if any.
	Document* pKernelDoc = Document::GetSelected();
	CCamDoc* pOilDoc = (pKernelDoc != 0) ? pKernelDoc->GetOilDoc() : 0;

	// Get the associated OpDescriptor.
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_EXIT);
	ERROR3IF(!pDesc, "Null OpDescriptor* in OpExit::GetState");

	// Is the selected document embedded in an OLE container etc?
	if (pOilDoc && pOilDoc->IsServer())
	{
		// The selected document is embedded, we know the specific container.
		UIDescription->MakeMsg(_R(IDS_FILE_EXIT_AND_RETURN),
								(LPCTSTR) pOilDoc->GetContainerName());
	}
	else if (CCamSrvrItem::HowMany() > 0)
	{
		// Are there any embedded docs?
		POSITION posTpl = AfxGetApp()->GetFirstDocTemplatePosition();
		CDocTemplate* pTemplate = AfxGetApp()->GetNextDocTemplate(posTpl);
		POSITION posDoc = pTemplate->GetFirstDocPosition();
		while (posDoc)
		{
			// Skip docs that aren't active OLE items.
			CCamDoc* pDoc = (CCamDoc*) pTemplate->GetNextDoc(posDoc);
			if (pDoc->IsServer())
			{
				// The selected document is not embedded but some other is,
				// we know only the word 'host' for this ...
				UIDescription->MakeMsg(_R(IDS_FILE_EXIT_AND_RETURN),
									   (LPCTSTR) String_64(_R(IDS_DEFAULT_HOST_NAME)));
				break;
			}
		}
			
	}

#endif
#endif

	// This option is always available.
	return OpState(FALSE, FALSE);
}



/******************************************************************************************

>       OpExit::OpExit()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        20/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new opExit object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

******************************************************************************************/

OpExit::OpExit(): Operation()
{
	// We don't show the progress indicator cos it may have been destroyed
	OpFlags.HasOwnTimeIndicator = TRUE;     
}



/******************************************************************************************

>       void OpExit::Do(OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        12/8/93
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Exit Application operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void OpExit::Do(OpDescriptor*)
{

	// CGS:  we need to clear out all intermediate blend steps ....
	// NOTE:  this clears out all 'built-up' intermediate data
	// it clears out both 'render' and 'becomea'

	NodeGroup::KillAllBlendBecomeAConsLists (TRUE);

	GetApplication ()->ResetRealDownload ();	// how dare they!
	
	// This op is not related to a document.
	pOurDoc = NULL;

PORTNOTE("other","Removed InternetManager usage")
#ifndef EXCLUDE_FROM_XARALX
//	InternetManager::OnExitInstance(); // this should be called as early as possible
#endif

	ExitAction(); 

	End();
}



/********************************************************************************************

>       BOOL OpExit::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create an OpDescriptor for the EXIT operation
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL OpExit::Init()
{   
	return (RegisterOpDescriptor(0, 
								_R(IDS_APP_EXIT),
								CC_RUNTIME_CLASS(OpExit), 
								OPTOKEN_EXIT,
								OpExit::GetState,
								HID_APP_EXIT,
								_R(IDBBL_EXITOP),
								_R(IDD_BARCONTROLSTORE),    /* resource ID */
								_R(IDC_BTN_FILEEXIT)                /* control ID */
								)
			);
}

//********************************************************************************************
//                                                      MENU POPUP OPERATIONS
/********************************************************************************************

>       MenuPopupOps::MenuPopupOps(): Operation()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        20/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new MenuPopupOps object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
MenuPopupOps::MenuPopupOps(): Operation()
{                 
}

/********************************************************************************************

>       void MenuPopupOps::Do(OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        12/8/93
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Exit Application operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void MenuPopupOps::Do(OpDescriptor* WhichOp)
{
	End();
}


/********************************************************************************************

>       OpState MenuPopupOps::GetState(String_256* UIDescription, OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Outputs:        -
	Returns:        -
	Purpose:        Find the state of the MenuPopupOps operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

OpState MenuPopupOps::GetState(String_256* UIDescription, OpDescriptor*)
{
	return OpState(FALSE, FALSE);
}                                    


/********************************************************************************************

>       BOOL MenuPopupOps::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create OpDescriptors for the each submenu operation in here.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL MenuPopupOps::Init()
{
	// The following Opdescriptors do not need to be created using the usual
	// RegisterOpDescriptor function because they cannot be invoked by the user and they
	// are not connected to controls on the button bars.
	OpDescriptor* FileOp = new OpDescriptor(
											0, 
											_R(IDS_FILE_MENU),
											CC_RUNTIME_CLASS(MenuPopupOps), 
											OPTOKEN_FILE_MENU,
											MenuPopupOps::GetState,
											0,      /* help ID */
											_R(IDBBL_FILEOP),
											0       /* bitmap ID */);
	ERRORIF(!FileOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* EditOp = new OpDescriptor(
											0, 
											_R(IDS_EDIT_MENU),
											CC_RUNTIME_CLASS(MenuPopupOps), 
											OPTOKEN_EDIT_MENU,
											MenuPopupOps::GetState,
											0,      /* help ID */
											_R(IDBBL_EDITOP),
											0       /* bitmap ID */);
	ERRORIF(!EditOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* AttrOp = new OpDescriptor(
											0, 
											_R(IDS_ATTRIBUTE_MENU),
											CC_RUNTIME_CLASS(MenuPopupOps), 
											OPTOKEN_ATTRIBUTE_MENU,
											MenuPopupOps::GetState,
											0,      /* help ID */
											0,
											0       /* bitmap ID */);
	ERRORIF(!AttrOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* HelpOp = new OpDescriptor(
											0, 
											_R(IDS_HELP_MENU),
											CC_RUNTIME_CLASS(MenuPopupOps), 
											OPTOKEN_HELP_MENU,
											MenuPopupOps::GetState,
											0,      /* help ID */
											_R(IDBBL_HELPOP),
											0       /* bitmap ID */);
	ERRORIF(!HelpOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* WindowOp = new OpDescriptor(
												0, 
												_R(IDS_WINDOW_MENU),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_WINDOW_MENU,
												MenuPopupOps::GetState,
												0,      /* help ID */
												_R(IDBBL_WINDOWOP),
												0       /* bitmap ID */);
	ERRORIF(!WindowOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* UtilsOp = new OpDescriptor(
											0, 
											_R(IDS_UTILS_MENU),
											CC_RUNTIME_CLASS(MenuPopupOps), 
											OPTOKEN_UTILS_MENU,
											MenuPopupOps::GetState,
											0,      /* help ID */
											0,
											0       /* bitmap ID */);
	ERRORIF(!UtilsOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* ArrangeOp = new OpDescriptor(     
												0, 
												_R(IDS_ARRANGE_MENU),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_ARRANGE_MENU,
												MenuPopupOps::GetState,
												0,      /* help ID */
												0,  /* Bubble help ID */
												0       /* bitmap ID */);
	ERRORIF(!ArrangeOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* DebugOp = new OpDescriptor(       
												0, 
												_R(IDS_DEBUG_MENU),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_DEBUG_MENU,
												MenuPopupOps::GetState,
												0,      /* help ID */
												0,  /* Bubble help ID */
												0       /* bitmap ID */);
	ERRORIF(!DebugOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* QualityOp = new OpDescriptor(     
												0, 
												_R(IDS_QUALITY_MENU),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_QUALITY_MENU,
												MenuPopupOps::GetState,
												0,      /* help ID */
												0,  /* Bubble help ID */
												0       /* bitmap ID */);
	ERRORIF(!QualityOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* BarsOp = new OpDescriptor(        
												0, 
												_R(IDS_BARS_CONFIG),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_BARS_MENU,
												MenuPopupOps::GetState,
												0,      /* help ID */
												0,  /* Bubble help ID */
												0       /* bitmap ID */);
	ERRORIF(!BarsOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* CombineOp = new OpDescriptor(     
												0, 
												_R(IDS_COMBINE_MENU),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_COMBINESHAPES,
												MenuPopupOps::GetState,
												0,      /* help ID */
												0,  /* Bubble help ID */
												0       /* bitmap ID */);
	ERRORIF(!CombineOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* NumSidesOp = new OpDescriptor(    
												0, 
												_R(IDS_QUICKSHAPES_NUMBERSIDES),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_QUICKSHAPE_NUMBERSIDES,
												MenuPopupOps::GetState,
												0,      /* help ID */
												0,  /* Bubble help ID */
												0       /* bitmap ID */);
	ERRORIF(!NumSidesOp, _R(IDE_NOMORE_MEMORY), FALSE);


PORTNOTE("other","Removed bitmap effects usage")
#ifndef EXCLUDE_FROM_XARALX
	OpDescriptor* EditEffectOp = new OpDescriptor(     
												0, 
												_R(IDS_EDITEFFECT_MENU),
												CC_RUNTIME_CLASS(MenuPopupOps), 
												OPTOKEN_EDITEFFECTS,
												MenuPopupOps::GetState,
												0,      // help ID
												0,		// Bubble help ID
												0 );	// bitmap ID
	ERRORIF(!EditEffectOp, _R(IDE_NOMORE_MEMORY), FALSE);
#endif
	return TRUE;
}

//********************************************************************************************
//                                                      FILE OPEN, CLOSE, NEW, SAVE OPERATION

/********************************************************************************************

>       DocOps::DocOps(): Operation()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        20/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new DocOps object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
DocOps::DocOps(): Operation()
{                 
}

/********************************************************************************************

>       void DocOps::Do(OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        12/8/93
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Exit Application operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void DocOps::Do(OpDescriptor* WhichOp)
{
	if ((WhichOp->Token) ==  String(OPTOKEN_FILEOPEN))
		FileOpenAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW))
		FileNewAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_DRAWING))
		FileNewAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_ANIMATION))
		FileNewAnimationAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE1))
		FileNewTemplateAction(1); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE2))
		FileNewTemplateAction(2); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE3))
		FileNewTemplateAction(3); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE4))
		FileNewTemplateAction(4); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE5))
		FileNewTemplateAction(5); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE6))
		FileNewTemplateAction(6); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE7))
		FileNewTemplateAction(7); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE8))
		FileNewTemplateAction(8); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE9))
		FileNewTemplateAction(9); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILENEW_TEMPLATE10))
		FileNewTemplateAction(10); 
	else if ((WhichOp->Token) == String(OPTOKEN_FILECLOSE))
	{
		// Close the file
		FileCloseAction();

		// Always set the working doc to NULL, otherwise we try to indirect it in End().
		// (This is safe even if the document doesn't close (e.g. modified) because we
		// don't do much clean-up after a successful or failed document close op - we
		// basically just delete the operation.
		pOurDoc = NULL;
	}
    else if ((WhichOp->Token) == String(OPTOKEN_FILESAVE))
		FileSaveAction(); 
    else if ((WhichOp->Token) == String(OPTOKEN_FILESAVEAS))
		FileSaveAsAction();
	else if ((WhichOp->Token) == String(OPTOKEN_FILESAVEALL))
	{
		// Save all modified documents
		FileSaveAllAction();
	}
	End();
}



/********************************************************************************************

>       BOOL DocOps::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create OpDescriptors for the each file operation
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL DocOps::Init()
{
	REGOP(FILE,OPEN,DocOps)
	REGOP(FILE,CLOSE,DocOps)
	REGOP(FILE,NEW,DocOps)
	REGOP(FILE,SAVE,DocOps)
	REGOP(FILE,SAVEAS,DocOps)

	REGOP(FILE,NEW_DRAWING,DocOps)
	REGOP(FILE,NEW_ANIMATION,DocOps)
	REGOP(FILE,NEW_TEMPLATE1,DocOps)
	REGOP(FILE,NEW_TEMPLATE2,DocOps)
	REGOP(FILE,NEW_TEMPLATE3,DocOps)
	REGOP(FILE,NEW_TEMPLATE4,DocOps)
	REGOP(FILE,NEW_TEMPLATE5,DocOps)
	REGOP(FILE,NEW_TEMPLATE6,DocOps)
	REGOP(FILE,NEW_TEMPLATE7,DocOps)
	REGOP(FILE,NEW_TEMPLATE8,DocOps)
	REGOP(FILE,NEW_TEMPLATE9,DocOps)
	REGOP(FILE,NEW_TEMPLATE10,DocOps)
	
	BOOL ok = RegisterOpDescriptor(
					0,
					_R(IDS_FILE_SAVEALL),
					CC_RUNTIME_CLASS(DocOps),
					OPTOKEN_FILESAVEALL,
					DocOps::GetState,
					0,                                      // Help
					_R(IDBBL_FILESAVEALL),      // Bubble ID
					_R(IDD_BARCONTROLSTORE),
					_R(IDC_FILESAVEALL),        // Button ID
					SYSTEMBAR_FILE,         // Bar ID
					TRUE                            // Recieve system messages
					);
	ERRORIF(!ok, _R(IDE_NOMORE_MEMORY), FALSE);

	//Graham 20/10/97
	if (Camelot.DeclareSection( _T("NewTemplates"), 10))
	{
		Camelot.DeclarePref( _T("NewTemplates"), _T("DefaultAnimationFile"), &ms_strDefaultAnimationTemplate);
		Camelot.DeclarePref( _T("NewTemplates"), _T("DefaultDrawingFile"), &ms_strDefaultDrawingTemplate);
	}

PORTNOTETRACE("other","DocOps::Init - remove code to setup paths");
#if !defined(EXCLUDE_FROM_XARALX)
	if (ms_strDefaultAnimationTemplate==String_256(_T("")))
	{
		//Then assume it's the exe path with \templates\animation.xar on the end
		TCHAR Pathname[MAX_PATH];

		if(GetModuleFileName(NULL, Pathname, MAX_PATH) == 0) //Should be in the winoil really
			return FALSE;

		// Put the path name into a string
		String_256 strPathOfExe(Pathname);
		PathName pathPathOfExe(strPathOfExe);

		strPathOfExe = pathPathOfExe.GetLocation(TRUE);

		//And add "templates\" to the end
		String_256 strTemplatesDirectory(_R(IDS_NEWTEMPLATES_RELATIVEPATH));
		String_256 strNameOfAnimationTemplate(_R(IDS_NEWTEMPLATES_DEFAULTANIMATIONFILE));

		String_256 strPathOfTemplate=strPathOfExe;
		strPathOfTemplate+=strTemplatesDirectory;
		strPathOfTemplate+=strNameOfAnimationTemplate;

		ms_strDefaultAnimationTemplate=strPathOfTemplate;
	}

	if (ms_strDefaultDrawingTemplate==String_256(_T("")))
	{
		//Then assume it's the exe path with \templates\drawing.xar on the end
		TCHAR Pathname[MAX_PATH];

		if(GetModuleFileName(NULL, Pathname, MAX_PATH) == 0) //Should be in the winoil really
			return FALSE;

		// Put the path name into a string
		String_256 strPathOfExe(Pathname);
		PathName pathPathOfExe(strPathOfExe);

		strPathOfExe = pathPathOfExe.GetLocation(TRUE);

		//And add "templates\" to the end
		String_256 strTemplatesDirectory(_R(IDS_NEWTEMPLATES_RELATIVEPATH));
		String_256 strNameOfDrawingTemplate(_R(IDS_DEFAULTDOCNAME));

		String_256 strPathOfTemplate=strPathOfExe;
		strPathOfTemplate+=strTemplatesDirectory;
		strPathOfTemplate+=strNameOfDrawingTemplate;

		ms_strDefaultDrawingTemplate=strPathOfTemplate;
	}
#else
	if (ms_strDefaultAnimationTemplate==String_256(_T("")))
	{
		ms_strDefaultAnimationTemplate=String_256(_T("animation.xar"));
	}

	if (ms_strDefaultDrawingTemplate==String_256(_T("")))
	{
		ms_strDefaultDrawingTemplate=String_256(_T("default.xar"));
	}
#endif
	
	return TRUE;
}

/********************************************************************************************

>   PathName DocOps::GetDefaultAnimationTemplate()

	Author:         Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:        23/10/97
	Inputs:         -
	Outputs:        -
	Returns:        The path of the default animation template
	Purpose:        As above
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

PathName DocOps::GetDefaultAnimationTemplate()
{
	PathName pathToReturn=ms_strDefaultAnimationTemplate;

	return pathToReturn;
}

/********************************************************************************************

>   PathName DocOps::GetDefaultDrawingTemplate()

	Author:         Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:        23/10/97
	Inputs:         -
	Outputs:        -
	Returns:        The path of the default drawing template
	Purpose:        As above
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

PathName DocOps::GetDefaultDrawingTemplate()
{
	PathName pathToReturn=ms_strDefaultDrawingTemplate;

	return pathToReturn;
}




//********************************************************************************************
//                                                      HELP USING & HELP INDEX OPERATION

/********************************************************************************************

>       HelpOps::HelpOps(): Operation()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        23/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new HelpOps object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
HelpOps::HelpOps(): Operation()
{                 
}

/********************************************************************************************

>       void HelpOps::Do(OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        12/8/93
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Exit Application operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void HelpOps::Do(OpDescriptor* WhichOp)
{
	if ((WhichOp->Token) ==  String(OPTOKEN_HELPUSING))
		HelpUsingAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_HELPINDEX))
		HelpIndexAction();
	else if (WhichOp->Token == String(OPTOKEN_HELPTOOLS))
		HelpToolsAction();
	else if (WhichOp->Token == String(OPTOKEN_HELPGALLERIES))
		HelpGalleriesAction();
	else if (WhichOp->Token == String(OPTOKEN_HELPDEMOS))
	{
	#ifndef WEBSTER
		HelpDemosAction();
	#else	// webster
		WebsterDemos();
	#endif //webster
	}
	else if (WhichOp->Token == String(OPTOKEN_HELPTECHSUPPORT))
		HelpTechSupportAction();
//Webster_Ranbir_12\11\96
#ifdef INC_WEB_MENU_ITEMS
	else if (WhichOp->Token == String(OPTOKEN_XARAX_FORUM))
		HelpXaraForumAction();
	else if (WhichOp->Token == String(OPTOKEN_WEBSTER_HOME_PAGE))
		HelpWebsterHomePage();
	else if (WhichOp->Token == String(OPTOKEN_XARA_HOME_PAGE))
		HelpXaraHomePage();
	else if (WhichOp->Token == String(OPTOKEN_HINTS_AND_TIPS_PAGES))
		WebsterHelpPages();
	else if (WhichOp->Token == String(OPTOKEN_XARA_PURCHASE_PAGE))
		HelpXaraPurchasePage();
//Webster_Ranbir_12\11\96
#endif //INC_WEB_MENU_ITEMS

#ifdef INC_WEB_MENU_ITEMS
	else if (WhichOp->Token == String(OPTOKEN_WEBLINK))
		if(GetCDHelpOn())
			SetWebsterReg(FALSE);
		else
			SetWebsterReg(TRUE);
#endif	//INC_WEB_MENU_ITEMS	

#ifdef STANDALONE
	else if ((WhichOp->Token) == String(OPTOKEN_HELPSPEC))
		HelpSpecAction(); 
#endif
	
	End();
}                 


/********************************************************************************************

>       OpState HelpOps::GetState(String_256* UIDescription, OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Outputs:        -
	Returns:        -
	Purpose:        Find the state of the HelpOps operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

OpState HelpOps::GetState(String_256* UIDescription, OpDescriptor* OpDesc)
{
	//Webster_Ranbir_12\11\96
#ifdef INC_WEB_MENU_ITEMS
	if (OpDesc->Token == String(OPTOKEN_WEBLINK))		
	{
		OpState Opst;
		BOOL RegistryFlag = IsCDROMOn();
		if(RegistryFlag)
		{
			Opst.Ticked = TRUE;
			SetCDHelpOn(TRUE);
		}
		else
		{
			Opst.Ticked = FALSE;
			SetCDHelpOn(FALSE);
		}
 		return Opst;
	}
	else
#endif //INC_WEB_MENU_ITEMS
		return OpState(FALSE, FALSE);
}                                    


/********************************************************************************************

>       BOOL HelpOps::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create OpDescriptors for the each file operation
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

#define HID_HELP_TOOLS                          (0)
#define HID_HELP_GALLERIES                      (0)
#define HID_HELP_DEMOS                          (0)
#define HID_HELP_TECHSUPPORT            (0)

BOOL HelpOps::Init()
{
	REGOP(HELP,USING,HelpOps)
	/* OpDescriptor* UsingOp = new OpDescriptor(
											0, 
											_R(IDS_HELP_USING),
											CC_RUNTIME_CLASS(HelpOps), 
											OPTOKEN_HELPUSING,
											HelpOps::GetState,
											HID_HELP_USING,
											_R(IDBBL_HELPUSINGOP),
											0 );

	ERRORIF(!UsingOp, _R(IDE_NOMORE_MEMORY), FALSE);*/

	// REGOP(HELP, INDEX, HelpOps)
	OpDescriptor* IndexOp = new OpDescriptor(
												0, 
												_R(IDS_HELP_TOPICS),
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_HELPINDEX,
												HelpOps::GetState,
												HID_HELP_INDEX,
												_R(IDBBL_HELPINDEXOP),
												0 );

	ERRORIF(!IndexOp, _R(IDE_NOMORE_MEMORY), FALSE);


	// REGOP(HELP, DEMOS, HelpOps);
	OpDescriptor* DemosOp = new OpDescriptor(
												0, 
												_R(IDS_HELP_MOVIES),
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_HELPDEMOS,
												HelpOps::GetState,
												HID_HELP_DEMOS,
												_R(IDBBL_HELPDEMOSOP),
												0 );

	ERRORIF(!DemosOp, _R(IDE_NOMORE_MEMORY), FALSE);

	//Webster_Ranbir_12\11\96
#ifdef INC_WEB_MENU_ITEMS

	OpDescriptor* WebsOp = new OpDescriptor(
												0, 
												_R(IDS_XARA_WEB),				
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_XARA_WEB_MENU,		
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!WebsOp, _R(IDE_NOMORE_MEMORY), FALSE);


	OpDescriptor* ForumOp = new OpDescriptor(
												0,
												_R(IDS_XARAX_FORUM),
												CC_RUNTIME_CLASS(HelpOps),
												OPTOKEN_XARAX_FORUM,
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!ForumOp, _R(IDE_NOMORE_MEMORY), FALSE);


	OpDescriptor* WebsHomePageOp = new OpDescriptor(
												0, 
												_R(IDS_WEBSTER_HOME_PAGE),   				
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_WEBSTER_HOME_PAGE,
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!WebsHomePageOp, _R(IDE_NOMORE_MEMORY), FALSE);


	OpDescriptor* XaraHomePageOp = new OpDescriptor(
												0, 
												_R(IDS_XARA_HOME_PAGE),   				
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_XARA_HOME_PAGE,
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!XaraHomePageOp, _R(IDE_NOMORE_MEMORY), FALSE);


	OpDescriptor* XaraPurchasePageOp = new OpDescriptor(
												0, 
												_R(IDS_XARA_PURCHASE_PAGE),   				
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_XARA_PURCHASE_PAGE,
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!XaraPurchasePageOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* HintsAndTipsPagesOp = new OpDescriptor(
												0, 
												_R(IDS_HINTS_AND_TIPS_PAGES),   				
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_HINTS_AND_TIPS_PAGES,
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!HintsAndTipsPagesOp, _R(IDE_NOMORE_MEMORY), FALSE);



	OpDescriptor* WebLink = new OpDescriptor(
												0, 
												_R(IDS_WEBLINK),   				
												CC_RUNTIME_CLASS(HelpOps), 
												OPTOKEN_WEBLINK,
												HelpOps::GetState,
												0,
												0,
												0 );

	ERRORIF(!WebLink, _R(IDE_NOMORE_MEMORY), FALSE);

#endif // INC_WEB_MENU_ITEMS

	REGOP(HELP, TOOLS, HelpOps);
	REGOP(HELP, GALLERIES, HelpOps);

	REGOP(HELP, TECHSUPPORT, HelpOps);

#ifdef STANDALONE
	REGOP(HELP,SPEC,HelpOps)
#endif

	return TRUE;
}

//********************************************************************************************
//                                                      WINDOW MENU OPERATIONS

/********************************************************************************************

>       WindowOps::WindowOps(): Operation()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        23/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new WindowOps object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
WindowOps::WindowOps(): Operation()
{                 
}

/********************************************************************************************

>       void WindowOps::Do(OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        12/8/93
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Exit Application operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void WindowOps::Do(OpDescriptor* WhichOp)
{

	PORTNOTETRACE("other","WindowOps::Do - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if ((WhichOp->Token) ==  String(OPTOKEN_WINDOWNEWVIEW))
		WindowNewAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_WINDOWARRANGE))
		WindowArrangeAction(); 
    else if ((WhichOp->Token) == String(OPTOKEN_WINDOWCASCADE))
		WindowCascadeAction(); 
    else if ((WhichOp->Token) == String(OPTOKEN_WINDOWTILE))
		WindowTileHorzAction(); 
//    else if ((WhichOp->Token) == String(OPTOKEN_WINDOWTILE_H))
//              WindowTileHorzAction(); 
//    else if ((WhichOp->Token) == String(OPTOKEN_WINDOWTILE_V))
//              WindowTileVertAction(); 
#endif
	End();
}                 


/********************************************************************************************

>       OpState WindowOps::GetState(String_256* UIDescription, OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Outputs:        -
	Returns:        -
	Purpose:        Find the state of the WindowOps operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

OpState WindowOps::GetState(String_256* UIDescription, OpDescriptor*)
{
	PORTNOTETRACE("other","WindowOps::GetState - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	return WindowCmdState();
#else
	return OpState();
#endif
}                                    


/********************************************************************************************

>       BOOL WindowOps::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create OpDescriptors for the each file operation
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL WindowOps::Init()
{
PORTNOTETRACE("other","WindowOps::Do - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	REGOP(WINDOW,NEWVIEW,WindowOps)
	/*OpDescriptor* NewOp = new OpDescriptor (
											0, 
											_R(IDS_WINDOW_NEW_VIEW),
											CC_RUNTIME_CLASS(WindowOps), 
											OPTOKEN_WINDOWNEWVIEW,
											WindowOps::GetState,
											HID_WINDOW_NEW,
											_R(IDBBL_WINDOWNEWOP),
											0 );

	ERRORIF(!NewOp, _R(IDE_NOMORE_MEMORY), FALSE);*/

	REGOP(WINDOW,ARRANGE,WindowOps)
	/*OpDescriptor* ArrangeOp = new OpDescriptor(
												0, 
												_R(IDS_WINDOW_ARRANGE),
												CC_RUNTIME_CLASS(WindowOps), 
												OPTOKEN_WINDOWARRANGE,
												WindowOps::GetState,
												HID_WINDOW_ARRANGE,
												_R(IDBBL_WINDOWARRANGEOP),
												0);

	ERRORIF(!ArrangeOp, _R(IDE_NOMORE_MEMORY), FALSE);*/

	REGOP(WINDOW,CASCADE,WindowOps)
	/*OpDescriptor* CascadeOp = new OpDescriptor(
											0, 
											_R(IDS_WINDOW_CASCADE),
											CC_RUNTIME_CLASS(WindowOps), 
											OPTOKEN_WINDOWCASCADE,
											WindowOps::GetState,
											HID_WINDOW_CASCADE,
											_R(IDBBL_WINDOWCASCADEOP),
											0);

	ERRORIF(!CascadeOp, _R(IDE_NOMORE_MEMORY), FALSE);*/

	REGOP(WINDOW,TILE,WindowOps)
	/*OpDescriptor* TileOp = new OpDescriptor(
												0, 
												_R(IDS_WINDOW_TILE),
												CC_RUNTIME_CLASS(WindowOps), 
												OPTOKEN_WINDOWTILE,
												WindowOps::GetState,
												HID_WINDOW_TILE_VERT,
												_R(IDBBL_TILEVERTICALOP),
												0);
	ERRORIF(!TileOp, _R(IDE_NOMORE_MEMORY), FALSE);*/
#endif

	return TRUE;
}

//********************************************************************************************
//                                                      VIEW MENU OPERATIONS

/********************************************************************************************

>       ViewOps::ViewOps(): Operation()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        23/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new ViewOps object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
ViewOps::ViewOps(): Operation()
{                 
}

/********************************************************************************************

>       virtual BOOL ViewOps::OnIdleEvent(void)

	Author:         Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:        4/6/95
	Inputs:         -
	Returns:        FALSE if it's OK to call low-priority idles (i.e. if I'm not busy this turn)

	Purpose:        Delays going into full screen mode until we are out of the broadcast loop
				makes BarOp deletion safe

********************************************************************************************/

BOOL ViewOps::OnIdleEvent(void)
{
	if(DoFullScreen)
	{
		Progress::Start(FALSE);         // Start an hourglass with no delay

PORTNOTE("other","Removed ViewFullScreenAction usage")
#ifndef EXCLUDE_FROM_XARALX
		ViewFullScreenAction(); 
#endif
			// Deregister for idles, because we should not be getting them now
		GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);

		End();
		DoFullScreen = FALSE;
		
#ifndef STANDALONE
PORTNOTE("other","Removed ToolbarDlg usage")
#ifndef EXCLUDE_FROM_XARALX
		// no ToolbarDlg on standalone versions
		ToolbarDlg * pToolbarDlg = ToolbarDlg::GetToolbarDlg();
		if(pToolbarDlg)
			pToolbarDlg->ShowToolbarList();
#endif
#endif          
		Progress::Stop();                       // Stop the hourglass again
	}
	return(FALSE);          // And let the low-priority idle handlers do their stuff
}




/********************************************************************************************

>       void ViewOps::Do(OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        12/8/93
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Exit Application operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
/*
Technical Notes:
********************************************************************************************/

void ViewOps::Do(OpDescriptor* WhichOp)
{
	PORTNOTETRACE("other","ViewOps::Do - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	DoFullScreen = FALSE;
	if ((WhichOp->Token) == String(OPTOKEN_VIEWFULLSCREEN))
	{
		// make sure there isn't a processor registered
		GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);
		// register the idle handler
		GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_HIGH, this);
		DoFullScreen = TRUE;
		// if we End() now the idle loop will fall over.
		return;                                        // DONT END() !!!! 
	}

	if ((WhichOp->Token) ==  String(OPTOKEN_VIEWTOOLBAR))
		ViewToolBarAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWSTATUSBAR))
		ViewStatusBarAction(); 
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWCOLOURBAR))
		ViewColourBarAction();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWSCROLLBARS))
		ViewScrollBarsAction();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWRULERS))
		ViewRulersAction();
#endif
	End();
}                 


/********************************************************************************************

>       OpState ViewOps::GetState(String_256* UIDescription, OpDescriptor*)

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Outputs:        -
	Returns:        -
	Purpose:        Find the state of the ViewOps operation.
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

OpState ViewOps::GetState(String_256* UIDescription, OpDescriptor* WhichOp)
{
	PORTNOTETRACE("other","ViewOps::GetState - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if ((WhichOp->Token) ==  String(OPTOKEN_VIEWTOOLBAR))
		return ViewToolBarState();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWSTATUSBAR))
		return ViewStatusBarState();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWCOLOURBAR))
		return ViewColourBarState();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWFULLSCREEN))
		return ViewFullScreenState();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWSCROLLBARS))
		return ViewScrollBarsState();
	else if ((WhichOp->Token) == String(OPTOKEN_VIEWRULERS))
		return ViewRulersState();
#endif
	return OpState(FALSE, FALSE);
}                                    


/********************************************************************************************

>       BOOL ViewOps::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create OpDescriptors for the each file operation
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL ViewOps::Init()
{
PORTNOTETRACE("other","ViewOps::Init - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	/*OpDescriptor* ToolOp = new OpDescriptor(
											0, 
											_R(IDS_VIEW_TOOLBAR),
											CC_RUNTIME_CLASS(ViewOps), 
											OPTOKEN_VIEWTOOLBAR,
											ViewOps::GetState,
											HID_VIEW_TOOLBAR,
											_R(IDBBL_VIEWTOOLBAROP),
											0 );

	ERRORIF(!ToolOp, _R(IDE_NOMORE_MEMORY), FALSE);*/

	REGOP(VIEW,STATUSBAR,ViewOps)
	OpDescriptor* ColourOp = new OpDescriptor(
												0, 
												_R(IDS_VIEW_COLOURBAR),
												CC_RUNTIME_CLASS(ViewOps), 
												OPTOKEN_VIEWCOLOURBAR,
												ViewOps::GetState,
												0,
												0,
												0,0,TRUE );

	ERRORIF(!ColourOp, _R(IDE_NOMORE_MEMORY), FALSE);

/*
	OpDescriptor* FullScreenOp = new OpDescriptor(
												0, 
												_R(IDS_VIEW_FULLSCREEN),
												CC_RUNTIME_CLASS(ViewOps), 
												OPTOKEN_VIEWFULLSCREEN,
												ViewOps::GetState,
												0,
												0,
												0,0,TRUE  );
*/
	BOOL FullScreenOp = RegisterOpDescriptor(
												0,
												_R(IDS_VIEW_FULLSCREEN),
												CC_RUNTIME_CLASS(ViewOps),
												OPTOKEN_VIEWFULLSCREEN,
												ViewOps::GetState,
												HID_VIEW_FULLSCREEN,
												_R(IDBBL_VIEWFULLSCREENOP),
												_R(IDD_BARCONTROLSTORE),
												_R(IDC_BTN_VIEWFULLSCREEN),
												SYSTEMBAR_VIEW,
												TRUE
											  );

//      REGOP(VIEW,FULLSCREEN,ViewOps)
	ERRORIF(!FullScreenOp, _R(IDE_NOMORE_MEMORY), FALSE);

	OpDescriptor* ScrollBarsOp = new OpDescriptor(
												0, 
												_R(IDS_VIEW_SCROLLBARS),
												CC_RUNTIME_CLASS(ViewOps), 
												OPTOKEN_VIEWSCROLLBARS,
												ViewOps::GetState,
												0,
												0,
												0,0,TRUE  );

	ERRORIF(!ScrollBarsOp, _R(IDE_NOMORE_MEMORY), FALSE);

	
	OpDescriptor* RulerOp = new OpDescriptor(
												0, 
												_R(IDS_VIEW_RULERS),
												CC_RUNTIME_CLASS(ViewOps), 
												OPTOKEN_VIEWRULERS,
												ViewOps::GetState,
												0,
												_R(IDBBL_PAGERULERS),
												_R(IDD_BARCONTROLSTORE),
												_R(IDC_PAGERULERS),
												SYSTEMBAR_VIEW,
												TRUE
												);

	ERRORIF(!RulerOp, _R(IDE_NOMORE_MEMORY), FALSE);
	//REGOP(VIEW,COLOURBAR,ViewOps)
#endif
	return TRUE;
}




//*****************************************************************************************
//                                                                      Dummy OPERATION

/******************************************************************************************

>       DummyOp::DummyOp()

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        29/03/94
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Constructs a new DummyOp object: setting default operation flags, and 
				adding it to the Live list. 
	Errors:         -
	SeeAlso:        -

********************************************************************************************/
     
DummyOp::DummyOp(): Operation()
{                 
}



/******************************************************************************************

>       void DummyOp::Do(OpDescriptor*)

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        29/03/94
	Inputs:         An Operation Descriptor
	Outputs:        -
	Returns:        -
	Purpose:        Actually "DO" a Dummy operation.
	Errors:         -
	SeeAlso:        -

******************************************************************************************/

void DummyOp::Do(OpDescriptor*)
{
	End();
}


/********************************************************************************************

>       OpState DummyOp::GetState(String_256* UIDescription, OpDescriptor*)

	Author:         Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:        29/03/94
	Inputs:         String - representing the operation description which may be updated
				OpDescriptor - To help update the state
	Outputs:        -
	Returns:        -
	Purpose:        Find the state of the DummyOp operation. This will always be greyed!
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

OpState DummyOp::GetState(String_256* UIDescription, OpDescriptor*)
{
	return OpState( FALSE, TRUE );  // Never ticked, always greyed
}

/********************************************************************************************

>       BOOL DummyOp::Init()

	Author:         Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:        11/8/93
	Inputs:         -
	Outputs:        -
	Returns:        -
	Purpose:        Create an OpDescriptor for the Dummy operation
	Errors:         -
	SeeAlso:        -

********************************************************************************************/

BOOL DummyOp::Init()
{
	DUMMYOP( _R(IDS_DO_NOTHING),				OPTOKEN_DO_NOTHING);

//      DUMMYOP(_R(IDS_FILE_SAVEALL),               OPTOKEN_FILESAVEALL);
	DUMMYOP(_R(IDS_FILE_MERGE),                 OPTOKEN_FILEMERGE);
//      DUMMYOP(_R(IDS_FILE_INFO),                  OPTOKEN_FILEINFO);
//      DUMMYOP(_R(IDS_FILE_OPTIONS),               OPTOKEN_FILEOPTIONS);
//      DUMMYOP(_R(IDS_UNITDLG),                    OPTOKEN_UNITDLG);
//      DUMMYOP(_R(IDS_EDIT_DELETE),                OPTOKEN_EDITDELETE);
//      DUMMYOP(_R(IDS_EDIT_SELECT_ALL),    OPTOKEN_EDITSELECTALL);
//      DUMMYOP(_R(IDS_EDIT_SELECT_NONE),   OPTOKEN_EDITSELECTNONE);
//      DUMMYOP(_R(IDS_EDIT_DUPLICATE),             OPTOKEN_EDITDUPLICATE);
//      DUMMYOP(_R(IDS_EDIT_CLONE),                 OPTOKEN_EDITCLONE);
	DUMMYOP(_R(IDS_EDIT_OBJECT_INFO),   OPTOKEN_EDITOBJECTINFO);
	DUMMYOP(_R(IDS_ATTR_FONT),                  OPTOKEN_ATTRFONT);
	DUMMYOP(_R(IDS_ATTR_LINE),                  OPTOKEN_ATTRLINE);
	DUMMYOP(_R(IDS_ATTR_FILL),                  OPTOKEN_ATTRFILL);
	DUMMYOP(_R(IDS_ATTR_STYLE),                 OPTOKEN_ATTRSTYLE);
//      DUMMYOP(_R(IDS_ARRANGE_UP_LAYER),   OPTOKEN_ARRANGEUPLAYER);
//      DUMMYOP(_R(IDS_ARRANGE_UP_TOP),             OPTOKEN_ARRANGEUPTOP);
//      DUMMYOP(_R(IDS_ARRANGE_UP_ONE),             OPTOKEN_ARRANGEUPONE);
//      DUMMYOP(_R(IDS_ARRANGE_DOWN_ONE),   OPTOKEN_ARRANGEDOWNONE);
//      DUMMYOP(_R(IDS_ARRANGE_DOWN_BOTTOM),OPTOKEN_ARRANGEDOWNBOTTOM);
//      DUMMYOP(_R(IDS_ARRANGE_DOWN_LAYER), OPTOKEN_ARRANGEDOWNLAYER);
//      DUMMYOP(_R(IDS_ARRANGE_ALIGNMENT),  OPTOKEN_ARRANGEALIGNMENT);
//      DUMMYOP(_R(IDS_ARRANGE_PULL_GRID),  OPTOKEN_ARRANGEPULLGRID);
	DUMMYOP(_R(IDS_MAKEMASK),   OPTOKEN_MAKEMASK);
	DUMMYOP(_R(IDS_MASKINSIDE), OPTOKEN_MASKINSIDE);
//      DUMMYOP(_R(IDS_ARRANGE_JOIN_SHAPES),        OPTOKEN_ARRANGEJOINSHAPES);
//      DUMMYOP(_R(IDS_ARRANGE_BREAK_SHAPES),       OPTOKEN_ARRANGEBREAKSHAPES);
//      DUMMYOP(_R(IDS_ARRANGE_MAKE_SHAPES),        OPTOKEN_ARRANGEMAKESHAPES);
//      DUMMYOP(_R(IDS_UTIL_COLOUR),                OPTOKEN_UTILCOLOUR);
	DUMMYOP(_R(IDS_UTIL_CUSTOMIZE),             OPTOKEN_UTILCUSTOMIZE);
//      DUMMYOP(_R(IDS_UTIL_OPTIONS),               OPTOKEN_UTILOPTIONS);
//      DUMMYOP(_R(IDS_WINDOW_NEW_VIEW),            OPTOKEN_WINDOWNEWVIEW);
	DUMMYOP(_R(IDS_WINDOW_EASY_MDI),            OPTOKEN_WINDOWEASYMDI);
//      DUMMYOP(_R(IDS_WINDOW_TOOLBARS),            OPTOKEN_WINDOWTOOLBARS);
	DUMMYOP(_R(IDS_WINDOW_RULERS),              OPTOKEN_WINDOWRULERS);
//      DUMMYOP(_R(IDS_WINDOW_GRIDS),                       OPTOKEN_WINDOWGRIDS);
//      DUMMYOP(_R(IDS_WINDOW_SNAP),                        OPTOKEN_WINDOWSNAP);
	DUMMYOP(_R(IDS_WINDOW_BORDERS),             OPTOKEN_WINDOWBORDERS);
	DUMMYOP(_R(IDS_WINDOW_QUALITY),             OPTOKEN_WINDOWQUALITY);
	DUMMYOP(_R(IDS_WINDOW_SEPARATION),  OPTOKEN_WINDOWSEPARATION);
//      DUMMYOP(_R(IDS_EDIT_CUT),                   OPTOKEN_EDITCUT);
//      DUMMYOP(_R(IDS_EDIT_COPY),                  OPTOKEN_EDITCOPY);
//      DUMMYOP(_R(IDS_EDIT_PASTE),                 OPTOKEN_EDITPASTE);
	DUMMYOP(_R(IDS_EDIT_PASTE_LINK),    OPTOKEN_EDITPASTESPECIAL);
	DUMMYOP(_R(IDS_EDIT_OLE_INSERT),    OPTOKEN_EDITOLEINSERT);
	DUMMYOP(_R(IDS_EDIT_OLE_LINKS),             OPTOKEN_EDITOLELINKS);
	DUMMYOP(_R(IDS_EDIT_OLE_OBJECT),    OPTOKEN_EDITOLEOBJECT);
//      DUMMYOP(_R(IDS_QUALITY_ANTI_ALIASED),       OPTOKEN_QUALITYANTIALIASED);
//      DUMMYOP(_R(IDS_QUALITY_NORMAL),                     OPTOKEN_QUALITYNORMAL);
//      DUMMYOP(_R(IDS_QUALITY_SIMPLE),                     OPTOKEN_QUALITYSIMPLE);
//      DUMMYOP(_R(IDS_QUALITY_OUTLINE),            OPTOKEN_QUALITYOUTLINE);

	DUMMYOP(_R(IDS_GALLERIES_SUB_OP),    OPTOKEN_GALLERIES_SUBMENU);
	DUMMYOP(_R(IDS_ANIMATION_SUB_OP),    OPTOKEN_ANIMATION_SUBMENU);
	return (TRUE);
}




String_256 OpUpdate::IndexBaseURL = _T("http://xara.xaraonline.com/XaraX2/WebFiles/");
String_32 OpUpdate::IndexLeafName = _T("index.xml");

/********************************************************************************************

>	OpUpdate::OpUpdate(): Operation()

	Author:         Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:        09/3/04
	Inputs:         
	Purpose:        OpUpdate constructor

********************************************************************************************/

OpUpdate::OpUpdate(): Operation()
{}

/********************************************************************************************

>	void OpUpdate::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:         Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:        09/3/04
	Inputs:			OpParam - (BOOL) Param1 Whether the request was from the user or automatic
	Purpose:        Show the Update or Special boards

********************************************************************************************/

void OpUpdate::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	PORTNOTETRACE("other","OpUpdate::DoWithParam - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL forcedByUser = (BOOL)pOpParam->Param1;

	if (!forcedByUser)
	{
		// This is an automatic (background) check
		CheckForNewFiles();

		// update takes priority over specials - don't show both

		if (!UpdateDontShowAgain)
		{
			CWebDialogDlg webdlg(TRUE, _T("UpgradeLX.web"), TRUE, NULL, 0, FALSE);
			webdlg.DoModal();
			UpdateDontShowAgain = webdlg.DontShowAgain();
		}
		else if (!SpecialsDontShowAgain)
		{
			CWebDialogDlg webdlg(TRUE, GetSpecialsFile());
			webdlg.DoModal();
			SpecialsDontShowAgain = webdlg.DontShowAgain();
		}
	}
	else
		Progress::Start(FALSE, _R(IDS_UPDATING_SPECIALS), -1);

	// kick off the web updater to find new versions of the files 
	// (to appear the next time we run)
	BOOL bOK = UpdateFiles(forcedByUser);

	if (forcedByUser)
		Progress::Stop();

	if (bOK && forcedByUser)
	{
		// This is a deliberate (foreground) check, so we must show the user something...
		CheckForNewFiles();

		// Don't show the "DontShowAgain" flag
		// Don't show the window's surround
		// Do show the "follow up" message
		CWebDialogDlg webdlg(FALSE, _T("UpgradeLX.web"), FALSE, NULL, 0, TRUE);
		webdlg.DoModal();
// Don't change the state of the DontShowAgain flag because we didn't show it...
//		UpdateDontShowAgain = webdlg.DontShowAgain();
	}

	// If invoked deliberately by the user then tag the HelpAndSupport downloads onto
	// the end of this operation...
	if (forcedByUser)
	{
		HelpDownloadOp::CheckForNewFiles(forcedByUser);
	} 
#endif
	End();
}

/********************************************************************************************

>	void OpUpdate::Do(OpDescriptor*)

	Author:         Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:        09/3/04
	Returns:        
	Purpose:        The default operation is called from the menu

********************************************************************************************/

void OpUpdate::Do(OpDescriptor* od)
{
PORTNOTETRACE("other","OpUpdate::Do - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	this->DoWithParam(od,&OpParam((INT32) TRUE, NULL));
#endif
}

/********************************************************************************************

>	static variables used in preferences

********************************************************************************************/

BOOL		OpUpdate::UpdateDontShowAgain = TRUE;
BOOL		OpUpdate::SpecialsDontShowAgain = TRUE;
INT32			OpUpdate::UpdateFileDate = 0;
INT32			OpUpdate::SpecialsFileDate = 0;


/********************************************************************************************

>	BOOL OpUpdate::Init()

	Author:         Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:        09/3/04
	Returns:        TRUE / FALSE
	Purpose:        Initialise the Update operation and preferences

********************************************************************************************/

BOOL OpUpdate::Init()
{
PORTNOTETRACE("other","OpUpdate::Init - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (! 
		RegisterOpDescriptor(
			0, 
			_T( _R(IDS_APP_UPDATE) ),
			CC_RUNTIME_CLASS(OpUpdate), 
			OPTOKEN_UPDATE,
			OpUpdate::GetState
		)
	)
	{
		return FALSE;
	}
	if (!Camelot.DeclareSection(TEXT("Update"), 12)	// 12 because HelpDownloadOp also puts 6 entris here...
		|| !Camelot.DeclarePref(TEXT("Update"), TEXT("UpdateDontShowAgain"), &UpdateDontShowAgain)
		|| !Camelot.DeclarePref(TEXT("Update"), TEXT("SpecialsDontShowAgain"), &SpecialsDontShowAgain)
		|| !Camelot.DeclarePref(TEXT("Update"), TEXT("UpdateFileDate"), &UpdateFileDate)
		|| !Camelot.DeclarePref(TEXT("Update"), TEXT("SpecialsFileDate"), &SpecialsFileDate)
		|| !Camelot.DeclarePref(TEXT("Update"), TEXT("WebFilesIndexLeafName"), &IndexLeafName)

// Only allow the base URL to be configurable in DEBUG builds
// Thus the value assigned to IndexBaseURL above is hardwired into Release builds
#ifdef _DEBUG
		|| !Camelot.DeclarePref(TEXT("Update"), TEXT("WebFilesBaseURL"), &IndexBaseURL)
#endif
		)
	{
		return FALSE;
	}
#endif

	return TRUE;
}

/****************************************************************************

>	void OpUpdate::CheckForNewFiles()

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2004
	Purpose:	record change of version of update/specials in preferences data
				to be shown at next appropriate time

****************************************************************************/

void OpUpdate::CheckForNewFiles()
{
	PORTNOTETRACE("other","OpUpdate::CheckForNewFiles - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// *** Important note *** For updating this code to Win64, ensure that
	// the 64 bit time_t value is NOT truncated to (32bit) integer.

	String_256 sUpgradeFile = GetWebFilePath(_T("UpgradeLX.web"));
	String_256 sSpecialsFileName = GetSpecialsFile();
	String_256 sSpecialsFilePath = GetWebFilePath(sSpecialsFileName);

	CFileStatus status;

	if (CFile::GetStatus(sUpgradeFile, status))
	{
		if (status.m_mtime.GetTime() > UpdateFileDate)
		{
			// only force the update check if it is NOT the installed file
			if (UpdateFileDate != 0)
				UpdateDontShowAgain = FALSE;

			UpdateFileDate = status.m_mtime.GetTime();
		}
	}
	if (CFile::GetStatus(sSpecialsFilePath, status))
	{
		if (status.m_mtime.GetTime() > SpecialsFileDate)
		{
			SpecialsDontShowAgain = FALSE;
			SpecialsFileDate = status.m_mtime.GetTime();
		}
	}
#endif
}

/****************************************************************************

>	String_256 OpUpdate::GetSpecialsFile()

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/2004
	Purpose:	Get the specials file name, which varies according to
				whether the user is running the Free Trial or not.

****************************************************************************/

String_256 OpUpdate::GetSpecialsFile()
{
	String_256 SpecialsFile;

	SpecialsFile = _T("SpecialsLX.web");

	return SpecialsFile;
}


/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Purpose:	Returns the base .web file path 

****************************************************************************/

String_256 OpUpdate::GetWebFileRoot()
{
	String_256 exepath;
PORTNOTETRACE("other","OpUpdate::GetWebFileRoot - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	
PORTNOTE("other","Removed CResDll usage")
#ifndef EXCLUDE_FROM_XARALX
	CResDll::GetExecutablePath(exepath);
#endif
	return String_256(exepath) + _T("\\WebFiles");
#else
	return exepath;
#endif

/*
// Save into AppData folder in Documents and Settings:
	DWORD CSInfo = CSIDL_APPDATA|CSIDL_FLAG_CREATE;

	String_256 sPartialPath(_T(""),MAX_PATH);
	TCHAR* Path = sPartialPath.GetBuffer(MAX_PATH);
 	SHGetFolderPath(NULL, CSInfo, NULL, 0, Path);
	sPartialPath.ReleaseBuffer();
	String_256 sPath;
	sPath.Format("%s\\Xara\\%s\\%s", sPartialPath, PROGRAM_NAME, _T("\\WebFiles"));

	return sPath;
*/
}

/****************************************************************************

>	String_256 GetWebFilePath(TCHAR *ReturnBuffer)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2004
	Purpose:	Returns the full .web file path for a given .web file. Reverts
				to ENG default if locale scoped file not found.

****************************************************************************/

String_256 OpUpdate::GetWebFilePath(LPCTSTR filename)
{
	String_256			webfilepath;

	PORTNOTETRACE("other","OpUpdate::GetWebFilePath - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	CFileStatus			TempStatus;

	// the path to the locale scoped file  ...
	webfilepath = GetWebFileRoot() + _T("\\") + CResDll::GetCurrentLocale();
	webfilepath += _T("\\") + String_256(filename);

	// if it doesn't exist, revert to the ENG default folder
	if (!CFile::GetStatus(webfilepath, TempStatus))
	{
		webfilepath = GetWebFileRoot() + _T("\\ENG\\") + String_256(filename);
	}
	TRACEUSER( "Marc", _T("webfilepath = %s\n"),webfilepath);
#endif
	return webfilepath;
}


/****************************************************************************

>	BOOL OpUpdate::UpdateFiles() 

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2004
	Purpose:	Call the WebFileUpdater to download any new files from
				our servers

****************************************************************************/

BOOL OpUpdate::UpdateFiles(BOOL bForeground) 
{
	return UpdateFiles(bForeground, GetWebFileRoot(), GetUpdateURL(), (String_256)IndexLeafName, _T("UpdateMessages"));
}


/****************************************************************************

>	BOOL OpUpdate::UpdateFiles(BOOL bForeground,
								const String_256& strClientFolder,
								const String_256& strURL,
								const String_256& strIndexLeafName,
								const String_256& strRegKeyName
								)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2004
	Purpose:	Call the WebFileUpdater to download any new files from
				our servers

****************************************************************************/

BOOL OpUpdate::UpdateFiles(BOOL bForeground,
							const String_256& strClientFolder,
							const String_256& strURL,
							const String_256& strIndexLeafName,
							const String_256& strRegKeyName
							)
{
	PORTNOTETRACE("other","OpUpdate::UpdateFiles - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	WEBFILEUPDATERLib::IWebFileUpdaterPtr pWebFileUpdater;
	HRESULT hr = pWebFileUpdater.CreateInstance(__uuidof(WebFileUpdater));
	ERROR2IF(FAILED(hr), FALSE, "Failed To create WebFileUpdater object");

	pWebFileUpdater->put_ClientFolder(_bstr_t(strClientFolder));
	pWebFileUpdater->put_ServerURL(_bstr_t(strURL));
	pWebFileUpdater->put_IndexName(_bstr_t(strIndexLeafName));

#ifndef _DEBUG
	// Don't let the WFU know about it's stored state in debug builds
	// This forces it to download a file every time - good for debugging
	//
	// Must ensure that the key exists because WFU doesn't create it if it's missing
	// (And doesn't DO ANYTHING if it's missing!)
	HKEY hKey = CreateRegKey(hAppStateRegKey, strRegKeyName);
	if (hKey)
	{
		// If ServerCheck doesn't exist yet, GetRegDword returns 0
		if (GetRegDword(hKey, _T("ServerCheck"))==0)
			SetRegDword(hKey, _T("ServerCheck"), 0);
		CloseRegKey(hKey);

		String_256 strFullRegKeyPath = _T(PRODUCT_REGISTRYKEY_USERPATH) + strRegKeyName;
		pWebFileUpdater->put_RegistryKeyPath(_bstr_t(strFullRegKeyPath));
	}
#endif

	hr = pWebFileUpdater->raw_Update((BSTR)NULL, !bForeground);
	if (FAILED(hr))
	{
		UINT32 idMessage = 0;

//		if (hr==E_XM_CLIENTFOLDERREADONLY)
		if (hr==0x80041008)
			idMessage = _R(IDS_WARN_PROGRAMFOLDER_READONLY);
//		else if (hr==E_XM_CLIENTFOLDERUNAVAILABLE)
		else if (hr==0x80041009)
			idMessage = _R(IDS_WARN_PROGRAMFOLDER_UNAVAILABLE);
//		else if (hr==E_XM_COULDNOTWRITEINDEX)
		else if (hr==0x80041007)
			idMessage = _R(IDS_WARN_PROGRAMFOLDER_NOTWRITABLE);

		if (idMessage!=0)
		{
			if (bForeground)
				idMessage = idMessage+1;		// NOTE! Message IDs must be interleaved correctly!
			ErrorInfo error;
			error.ErrorMsg = idMessage;
			error.Button[0] = _R(IDS_CONTINUE);
			INT32 ret = InformWarning(&error);
		}
		else if (bForeground)
		{
//			String_256 strError(_T(""));
//			strError.Format(_T("Failed to contact xara.com or failed to read update information (0x%08x)."),e.Error());
//			::MessageBox(NULL, strError, PRODUCT_NAME, MB_OK);
			ErrorInfo error;
			error.ErrorMsg = _R(IDE_FAILEDTOUPDATEHANDS);
			error.Button[0] = _R(IDS_CONTINUE);
			INT32 ret = InformWarning(&error);
		}
		return FALSE;
	}
#endif
	return TRUE;
}


/****************************************************************************

>	String_256 OpUpdate::GetUpdateURL()

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/2004
	Purpose:	returns the Update URL. 
	Notes:		The update URL is scoped to the Major product version
				to minimise the number of update folders required

****************************************************************************/

String_256 OpUpdate::GetUpdateURL()
{
PORTNOTETRACE("other","OpUpdate::GetUpdateURL() - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	String_256 sTemp;
	sTemp = IndexBaseURL;
	sTemp += PRODUCT_RETAIL_VERSION_NO;
	LPCTSTR szURL = sTemp;
	String_256 strURLEnc(szURL);
	DWORD dwLen = camStrlen(szURL) * 3; // max buffer length for encoded URL

	// Since the version may include illegal URL characters, canonicalise it.
	InternetCanonicalizeUrl(szURL, strURLEnc.GetBuffer(dwLen), &dwLen, 0);
	strURLEnc.ReleaseBuffer();

	return strURLEnc;
#else
	return String_256(_T(""));
#endif
}

/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2004
	Purpose:	Constructor for OpRegister

****************************************************************************/

OpRegister::OpRegister(): Operation()
{}

BOOL	OpRegister::RegisteredOnline = FALSE;
PORTNOTE("other","OpRegister removed use of DATE type");
#ifndef EXCLUDE_FROM_XARALX
DATE	OpRegister::LastCheck = (DATE)1.0;
#endif
INT32		OpRegister::DaysBetweenAutoChecks = 5 ;

/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2004
	Inputs:		(BOOL)OpParam->Param1 - User requested registration 
	Purpose:	Either the user requested to register or it is an auto-check.

****************************************************************************/

void OpRegister::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
PORTNOTETRACE("other","OpRegister::DoWithParam - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL Requested = (BOOL)pOpParam->Param1;
	if(Requested)
	{
		if(RegisterOnline())
		{
			put_HasRegistered(TRUE);
		}
	}
	else
	{
		// an auto-check only prompts the user if it's long enough since we last nagged them
		if(!get_HasRegistered())
		{
			COleDateTimeSpan ts = COleDateTime::GetCurrentTime() - COleDateTime(LastCheck);
			INT32 DaysSinceLastCheck = ts.GetTotalDays();
			if( DaysSinceLastCheck >= DaysBetweenAutoChecks )
			{
				CRegisterDialog RegisterDlg;
				switch(RegisterDlg.DoModal())
				{
				case IDOK:
					if(RegisterOnline())
					{
						put_HasRegistered(TRUE);
					}
					break;
				case IDCANCEL:
				default:
					{
						LastCheck = COleDateTime::GetCurrentTime().m_dt;
					}
					break;
				}
			}
		}
	}
#endif
}

/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2004
	Purpose:	The default Register operation called from the menu

****************************************************************************/

void OpRegister::Do(OpDescriptor* od)
{
	OpParam temp((INT32) TRUE, (void *)NULL);
	this->DoWithParam(od, &temp);
}

/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2004
	Purpose:	Register an OpDescriptor

****************************************************************************/

BOOL OpRegister::Init()
{
PORTNOTETRACE("other","OpRegister::Init - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (! 
		RegisterOpDescriptor(
			0, 
			_R(IDS_REGISTER_MENUTEXT),
			CC_RUNTIME_CLASS(OpRegister), 
			OPTOKEN_REGISTER,
			OpRegister::GetState
		)
	)
	{
		return FALSE;
	}
	if (!Camelot.DeclareSection(TEXT("Register"), 2)	
		|| !Camelot.DeclarePref(TEXT("Register"), TEXT("LastCheck"), &LastCheck)
		)
	{
		return FALSE;
	}
#endif

	return TRUE;
}

/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2004
	Purpose:	returns the serial number stored in the registry

****************************************************************************/

String_256 OpRegister::GetSerialNumber()
{
	// We now use a registry key to hold serial number data, so use that if available. If not then
	// display nothing.
	String_256		serialNumber = "";

PORTNOTETRACE("other","OpRegister::getSerialNumber - do nothing");
#ifndef EXCLUDE_FROM_XARALX

	HKEY		hKey = NULL;
	INT32			i;
	char		unlockCode[32];
	DWORD		dwType = REG_SZ;
	DWORD		dwSize = sizeof(unlockCode);

	// Initialise unlock code buffer.
	for(i=0;i<32;i++)
	{
		unlockCode[i] = NULL;
	}

	// Open the key.
	hKey = OpenRegKey(HKEY_CLASSES_ROOT,SERIALNO_REGPATH);
	if(hKey != NULL)
	{
		GetRegValue(hKey,"ulc",&dwType,unlockCode,&dwSize);

		serialNumber = unlockCode;

		// Close the key.
		RegCloseKey(hKey);
	}
#endif

	return serialNumber;
}


/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/Aug/2005
	Purpose:	Read registry value to find out if the user has registered. This
				is no longer a Pref but stored with the serial number key so that 
				it persists when the product is uninstalled

****************************************************************************/

BOOL OpRegister::get_HasRegistered()
{
PORTNOTETRACE("other","OpRegister::get_HasRegistered - do nothing - always registered?");
#ifndef EXCLUDE_FROM_XARALX
	// Open the key.
	HKEY	hKey = OpenRegKey(HKEY_CLASSES_ROOT,SERIALNO_REGPATH);
	BOOL	bRegistered = GetRegBool(hKey,TEXT("HasRegistered"));
	CloseRegKey(hKey);
	return bRegistered;
#else
	return TRUE;
#endif
}

/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/Aug/2005
	Purpose:	Write registry value to find out if the user has registered. 

****************************************************************************/

void OpRegister::put_HasRegistered(BOOL bHasRegistered)
{
PORTNOTETRACE("other","OpRegister::get_HasRegistered - do nothing - always registered?");
#ifndef EXCLUDE_FROM_XARALX
	HKEY	hKey = OpenRegKey(HKEY_CLASSES_ROOT,SERIALNO_REGPATH);
	SetRegBool(hKey, TEXT("HasRegistered"), bHasRegistered);
	CloseRegKey(hKey);
#endif
}



/****************************************************************************

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2004
	Purpose:	Creates a StratusClient to actually perform online registration

****************************************************************************/

BOOL OpRegister::RegisterOnline()
{
PORTNOTETRACE("other","OpRegister::RegsiterOnline - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL RegSuccess = FALSE; 
	IStratusClient2Ptr pStratusClient;
	HRESULT hr = pStratusClient.CreateInstance(__uuidof(StratusClient_1_0_2)) ;
	ERROR2IF(FAILED(hr), FALSE, "Stratus Client didn't create") ;

	pStratusClient->put_ParentWindow((INT32)(GetMainFrame()->GetSafeHwnd()));
	pStratusClient->put_SerialNumber(_bstr_t(OpRegister::GetSerialNumber())) ;
	pStratusClient->put_RegistryKey(_bstr_t(_T("\\" PRODUCT_REGISTRYKEY_FULLPATH "Connect")));
	pStratusClient->put_HintText(_bstr_t(_T("")));// need to blank the default
	pStratusClient->put_ShowStorePassword(FALSE) ;
	pStratusClient->put_ConnectButtonText(_bstr_t(String_256((LPCSTR)_R(IDS_REGISTER_BUTTON))));
	pStratusClient->put_ConnectTitle(_bstr_t(String_256((LPCSTR)_R(IDS_REGISTER_TITLE))));
	pStratusClient->put_JoinTitle(_bstr_t(String_256((LPCSTR)_R(IDS_REGISTER_TITLE))));
	pStratusClient->put_ConnectText(_bstr_t(String_256((LPCSTR)_R(IDS_REGISTER_MAINTEXT))));
	pStratusClient->put_JoinText(_bstr_t(String_256((LPCSTR)_R(IDS_REGISTER_JOINTEXT))));

	try
	{
		// registration succeeds if the sign-in method is successfully invoked
		pStratusClient->SignIn((BSTR)NULL,(BSTR)NULL);
		RegSuccess = TRUE;
	}
	catch(_com_error e)
	{
		if (e.Error() != E_XM_CONNECTCANCELLED)
		{
			String_256 strError;
			strError.Format(String_256((LPCSTR)_R(IDS_REGISTER_CONNECTPROBLEM)),e.Error());
			::MessageBox(NULL, strError, PRODUCT_NAME, MB_OK);
		}
	}

	if(RegSuccess)
		::MessageBox(NULL, String_256((LPCSTR)_R(IDS_REGISTER_REGSUCCESS)), PRODUCT_NAME, MB_OK);

	return RegSuccess;
#else
	return TRUE;
#endif
}


#endif
