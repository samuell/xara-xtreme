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
// The Bezier Edit Tool
// Created by Jim on 31/3/94
// Based on Freehand.cpp by Rik

/*
*/

#include "camtypes.h"
#include "beztool.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bezbar.h"
#include "blobs.h"
#include "csrstack.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "finfodlg.h"
#include "keypress.h"
//#include "jim.h"
#include "lineattr.h"
//#include "mario.h"
//#include "markn.h"
//#include "msg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ndtxtpth.h"
//#include "node.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodedoc.h"
#include "nodershp.h"
#include "nodetxts.h"
#include "oilfiles.h"
#include "oilmods.h"
#include "opbreak.h"
#include "opbezier.h"
//#include "opdesc.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opnudge.h"
#include "pathedit.h"
#include "pathndge.h"
#include "pathops.h"
//#include "peter.h"
#include "progress.h"
//#include "resource.h"
//#include "rik.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "viewrc.h"
#include "vkextra.h"
#include "nodeblnd.h"
#include "ndbldpth.h"
#include "blndtool.h"
//#include "justin2.h"
//#include "will2.h"

#include "lattrops.h"
#include "nbevcont.h"
#include "nodecntr.h"
#include "nodeshad.h"
#include "slicehelper.h"
#include "opbevel.h"
#include "nodebev.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_MEMDUMP( BezierTool, Tool_v1 )
CC_IMPLEMENT_DYNCREATE( BezToolInfoBarOp, InformationBarOp )

// Better memory tracking please
#define new CAM_DEBUG_NEW

// These are still TCHAR* while we wait for resource technology to be developed for modules
TCHAR* BezierTool::FamilyName = _T("Drawing Tools");
TCHAR* BezierTool::ToolName = _T("Bezier Tool");
TCHAR* BezierTool::Purpose = _T("To Draw arbitrary lines");
TCHAR* BezierTool::Author = _T("Jim (latterly Peter)");

BezToolInfoBarOp*	BezierTool::pBezToolInfoBarOp = NULL;
BOOL BezierTool::CreateCurve = TRUE;
BOOL BezierTool::CreateCusp	= FALSE;

const INT32 NumberIconIDs = 18;
INT32 AllIconIDs[NumberIconIDs] =  
		{_R(IDC_PATH_EDIT_ENDPOINTX), _R(IDC_PATH_EDIT_ENDPOINTY), _R(IDC_PATH_BUMP_ENDX_LESS),
		 _R(IDC_PATH_BUMP_ENDX_MORE), _R(IDC_PATH_BUMP_ENDY_LESS), _R(IDC_PATH_BUMP_ENDY_MORE),
		 _R(IDC_PATH_EDIT_FIRSTX), _R(IDC_PATH_EDIT_FIRSTY), _R(IDC_PATH_BUMP_FIRSTX_LESS),
		 _R(IDC_PATH_BUMP_FIRSTX_MORE), _R(IDC_PATH_BUMP_FIRSTY_LESS), _R(IDC_PATH_BUMP_FIRSTY_MORE),
		 _R(IDC_PATH_EDIT_SECONDX), _R(IDC_PATH_EDIT_SECONDY), _R(IDC_PATH_BUMP_SECONDX_LESS),
		 _R(IDC_PATH_BUMP_SECONDX_MORE), _R(IDC_PATH_BUMP_SECONDY_LESS), _R(IDC_PATH_BUMP_SECONDY_MORE)};


/********************************************************************************************

>	BezierTool::BezierTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Dump Constructor - It does nothing. All the real initialisation is done
				in BezierTool::Init which is called by the Tool Manager
	SeeAlso:	BezierTool::Init

********************************************************************************************/

BezierTool::BezierTool()
{
	StartSpread = NULL;
	FloatingEndpoint = FALSE;
	MoveToDoc = NULL;
	MoveToSpread = NULL;
	pSmooth = NULL;
	RetroFlag = FALSE;
	DontDrawOnClearMoveTo = FALSE;
	CurrentToolMode = New;
}



/********************************************************************************************

>	BezierTool::~BezierTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

BezierTool::~BezierTool()
{
	// Destroy any resident retro smooth object
	if (pSmooth != NULL)
	{
		delete pSmooth;
		pSmooth = NULL;
	}

	pBezToolInfoBarOp->pBezTool = NULL;
}



/********************************************************************************************

>	BOOL BezierTool::Init( INT32 Pass )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	BezierTool::BezierTool

\********************************************************************************************/

BOOL BezierTool::Init()
{
	CCResTextFile 		file;				// Resource File
	BezToolInfoBarOpCreate BarCreate;		// Object that creates InfoBarOp objects

	#if 0
	BOOL 				ok;
	ok = file.open(_R(IDM_BEZTOOL_BAR), _R(IDT_INFO_BAR_RES));			// Open resource
	if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
	if (ok) file.close();									 	// Close resource

	ERROR2IF(!ok, FALSE, "Unable to load bezbar.ini from resource\n"); 

	// Info bar now exists.  Now get a pointer to it
	String_32 str = String_32(_R(IDS_BEZTOOL_INFOBARNAME));
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);
	
	ERROR2IF(pDialogBarOp==NULL, FALSE, "Line tool infobar not found\n");

	ERROR3IF(!(pDialogBarOp->IS_KIND_OF(BezToolInfoBarOp)), "Line tool infobar wrong type");
	#endif

	pBezToolInfoBarOp = new BezToolInfoBarOp();
	if (!pBezToolInfoBarOp)
	{
		ERROR3("Could not load bezier tool info bar");
		return FALSE;
	}	

	FloatingEndpoint = FALSE;
	MoveToDoc = NULL;
	MoveToSpread = NULL;

	pBezToolInfoBarOp->pBezTool = this;				// Set a pointer from the op to this tool
	pBezToolInfoBarOp->CurrentIndex = -1;
	pBezToolInfoBarOp->CurrentNodePath = NULL;
	pBezToolInfoBarOp->CurrentInkPath = NULL;
	pBezToolInfoBarOp->CurrentSpread = NULL;
	pBezToolInfoBarOp->IgnoreNextUpdate = FALSE;
	pBezToolInfoBarOp->LastShownPrevLength = 0;
	pBezToolInfoBarOp->LastShownNextLength = 0;

	// Read in the preference settings from the file
	GetApplication()->DeclareSection(_T("ShapeEditorTool"),2);
	GetApplication()->DeclarePref(_T("ShapeEditorTool"),_T("CreateCusp"), &CreateCusp, 0, 1);
	GetApplication()->DeclarePref(_T("ShapeEditorTool"),_T("CreateCurved"), &CreateCurve, 0, 1);

	// create a retro smooth regions object
	if (pSmooth != NULL)
	{
		ERROR3("Initialisation failure in class BezierTool: smooth ptr not NULL");
		return FALSE;
	}

	pSmooth = new RetroSmooth;
	if (pSmooth == NULL)
		return FALSE;

	if (!pSmooth->Initialise())
		return FALSE;

	// Initialise operations in opbezier.cpp
	if (!( OpSelectPathPoints::Declare() && OpInsertFloater::Declare() && OpRemoveFloater::Declare() ) )
		return FALSE;

	return TRUE;
}





/********************************************************************************************

>	void BezierTool::Describe(void *InfoPtr)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void BezierTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info -> InfoVersion = 1;
	
	Info -> InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info -> Version = 1;
	Info -> ID      = GetID();
	Info -> TextID  = _R(IDS_BEZIER_TOOL);

	Info -> Family  = FamilyName;
	Info -> Name    = ToolName;
	Info -> Purpose = Purpose;
	Info -> Author  = Author;

	Info -> InfoBarDialog = _R(IDD_BEZTOOLBAR);

	Info -> BubbleID = _R(IDBBL_LINETOOL);
	Info -> StatusID = _R(IDS_LINETOOL);
}



/********************************************************************************************
>	virtual void BezierTool::SelectChange(BOOL isSelected)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the tool is selected or deselected.  Creates and pushes the
				tool's cursor; pops and destroys it.
	Errors:		If creating any cursor fails, all are deleted and the user is told about
				lack of memory
	SeeAlso:	-
********************************************************************************************/

void BezierTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// This tool has just been selected.  Create an appropriate cursor, and push it
		// onto the top of the cursor stack so it'll appear when the pointer moves into
		// our window.
		
		MyCurrentCursor = NULL;		// in case we couldn't create one!
		
		pcMoveBezCursor = new Cursor(this, _R(IDC_MOVEBEZIERCURSOR));
		if (!pcMoveBezCursor || !pcMoveBezCursor->IsValid())
		{
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
			return;										 
		}

		pcReshapeLineCursor = new Cursor(this, _R(IDC_RESHAPECURSOR));
		if (!pcReshapeLineCursor || !pcReshapeLineCursor->IsValid())
		{
			delete pcMoveBezCursor;
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
			return;
		}

		pcNewPathCursor = new Cursor(this, _R(IDC_NEWPATHCURSOR));
		if (!pcNewPathCursor || !pcNewPathCursor->IsValid())
		{
			delete pcMoveBezCursor;
			delete pcReshapeLineCursor;
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
			return;
		}

		pcClosePathCursor = new Cursor(this, _R(IDC_CLOSEPATHCURSOR));
		if (!pcClosePathCursor || !pcClosePathCursor->IsValid())
		{
			delete pcMoveBezCursor;
			delete pcReshapeLineCursor;
			delete pcNewPathCursor;
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
			return;
		}

		pcAddPathCursor = new Cursor(this, _R(IDC_ADDPATHCURSOR));
		if (!pcAddPathCursor || !pcAddPathCursor->IsValid())
		{
			delete pcMoveBezCursor;
			delete pcReshapeLineCursor;
			delete pcNewPathCursor;
			delete pcClosePathCursor;
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
			return;
		}

		CurrentCursorID = CursorStack::GPush(pcNewPathCursor, FALSE);
		MyCurrentCursor = pcNewPathCursor;

		// Which blobs do I want displayed
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs we will display
			BlobStyle MyBlobs;
			MyBlobs.Object = TRUE;

			// Tell the blob manager
			BlobMgr->ToolInterest(MyBlobs);
		}
		
		if (FloatingEndpoint)
			BlobMgr->RenderToolBlobsOn(this, MoveToSpread,NULL);

		pBezToolInfoBarOp->Create();
		SetModeFlag();
		RetroFlag = FALSE;
		ResetRetroSlider();

		//Alias all the nudge ops to the path specific ones.
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP1);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeUp1),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP5);  		   
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeUp5),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUP10);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeUp10),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPFIFTH);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeUpFifth),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL1);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeUpPixel1),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEUPPIXEL10);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeUpPixel10),NULL,0);


		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN1);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeDown1),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN5);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeDown5),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWN10);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeDown10),NULL,0);  
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNFIFTH);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeDownFifth),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL1);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeDownPixel1),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGEDOWNPIXEL10);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeDownPixel10),NULL,0);


		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT1);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeLeft1),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT5);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeLeft5),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFT10);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeLeft10),NULL,0);  
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTFIFTH);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeLeftFifth),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL1);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeLeftPixel1),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGELEFTPIXEL10);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeLeftPixel10),NULL,0);


		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT1);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeRight1),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT5);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeRight5),NULL,0);  
 		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHT10);  
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeRight10),NULL,0);  
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTFIFTH);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeRightFifth),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL1);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeRightPixel1),NULL,0);
		pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NUDGERIGHTPIXEL10);
		pOpDesc->AliasOperation(CC_RUNTIME_CLASS(OpPathNudgeRightPixel10),NULL,0);

		// If there is are TextStories or Blends with paths selected then select the paths
		// (PS. This is a rather disgusting way of doing things, and one you can't blame on me - MarkN 17-5-99)
		// Now its made even more disgusting - since we wan't to do this for bevels, contours and shadows as well! (CGS 18/9/2000)
		Range SelRng(*(GetApplication()->FindSelection()));
		Node* pNode = SelRng.FindFirst();
	
		while (pNode != NULL)
		{
			if (IS_A(pNode, TextStory))
			{
				TextStory* pStory = (TextStory*)pNode;
				if (pStory->GetTextPath() != NULL)
					pStory->GetTextPath()->Select(TRUE);
			}

			else if (IS_A(pNode, NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pNode;
				UINT32 NBPCounter = 0;
				NodeBlendPath* pNodeBlendPath = pNodeBlend->GetNodeBlendPath(NBPCounter++);
				while (pNodeBlendPath != NULL)
				{
					pNodeBlendPath->Select(TRUE);
					pNodeBlendPath = pNodeBlend->GetNodeBlendPath(NBPCounter++);
				}
			}

			else if ((IS_A (pNode, NodeContour)) || (IS_A (pNode, NodeBevel)) || (IS_A (pNode, NodeShadow)))
			{
				Node* pControl = pNode->FindParent ();

				Node* pNode = SliceHelper::FindNextOfClass(pControl, pControl, CC_RUNTIME_CLASS (NodePath));

				while (pNode)
				{
					((NodeRenderable*) pNode)->Select (TRUE);
					pNode = (AttrFillGeometry*) SliceHelper::FindNextOfClass(pNode, pControl, CC_RUNTIME_CLASS (NodePath));
				}

				pNode = SliceHelper::FindNextOfClass(pControl, pControl, CC_RUNTIME_CLASS (NodeRegularShape));

				while (pNode)
				{
					((NodeRenderable*) pNode)->Select (TRUE);
					pNode = (AttrFillGeometry*) SliceHelper::FindNextOfClass(pNode, pControl, CC_RUNTIME_CLASS (NodeRegularShape));
				}
			}
			
			pNode = SelRng.FindNext(pNode);
		}
	}
	else
	{
		// Deselection - destroy the tool's cursor, if there is one.
		if (MyCurrentCursor)
		{
			CursorStack::GPop(CurrentCursorID);
			MyCurrentCursor = NULL;
			CurrentCursorID = 0;
			delete pcNewPathCursor;
			delete pcMoveBezCursor;
			delete pcReshapeLineCursor;
			delete pcClosePathCursor;
			delete pcAddPathCursor;
		}
		
		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		// Clear the floating endpoint condition if it exists
		// If camelot is shutting down then dont do it as the operation history has already been
		// vaped from the document!
		if (FloatingEndpoint && !GetApplication()->CamelotIsDying())
			RemoveFloater(&MoveToPoint, MoveToSpread, Document::GetSelected());
		
		pBezToolInfoBarOp->Delete();
	}
}

/********************************************************************************************

>	MsgResult BezToolInfoBarOp::Message(Msg* Message) 

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Inputs:		Message: The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Bezier tool info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult BezToolInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		// Message aimed at out infobar
		MsgResult TempResult = BezToolInfoBarOp::InfobarMessage(Message);
		if (TempResult != OK)
			return TempResult;		
	}
	else if (MESSAGE_IS_A(Message,OpMsg)) 	// Check for undo/redo
	{
		if ( (((OpMsg*)Message)->MsgType == OpMsg::AFTER_UNDO) ||
										(((OpMsg*)Message)->MsgType == OpMsg::AFTER_REDO) )
		{
			if (pBezTool != NULL)
			{
				pBezTool->SetModeFlag();
				pBezTool->ResetRetroSlider();
			}
		}
	}
	else if (MESSAGE_IS_A(Message,SelChangingMsg)) 
	{
		if ( (((SelChangingMsg*)Message)->State == SelChangingMsg::SELECTIONCHANGED) ||
						 (((SelChangingMsg*)Message)->State == SelChangingMsg::NODECHANGED) )
		{
			if (pBezTool != NULL)
			{
				pBezTool->SetModeFlag();
				BezToolInfoBarOp::UpdateState();
				pBezTool->ResetRetroSlider();
			}
		}
	}
	else if (MESSAGE_IS_A(Message,UnitMsg)) 	// Update the bar as someone has changd a unit
	{
		if (pBezTool != NULL)
		{
			pBezTool->SetModeFlag();
			BezToolInfoBarOp::UpdateState();
		}
	}
	else if (MESSAGE_IS_A(Message,DocChangingMsg)) 
	{													  						
		// Remove the floating endpoint from the old document
		DocChangingMsg*	msg = (DocChangingMsg*)Message;
		if ((msg->State == DocChangingMsg::SELCHANGED) && pBezTool!=NULL && pBezTool->FloatingEndpoint)
		{
			if ((msg->pOldDoc != NULL) && (msg->pNewDoc != msg->pOldDoc) && !GetApplication()->CamelotIsDying())
			{
				// Clear the floating endpoint condition
				pBezTool->DontDrawOnClearMoveTo = TRUE;
				pBezTool->RemoveFloater(&(pBezTool->MoveToPoint), pBezTool->MoveToSpread, msg->pOldDoc);
				pBezTool->DontDrawOnClearMoveTo = FALSE;
			}
		}
	}
	else if (MESSAGE_IS_A(Message,DocViewMsg)) 
	{													  						
		DocViewMsg* msg = (DocViewMsg*) Message;
		BlobManager* pBlobManager = GetApplication()->GetBlobManager();
		ENSURE(pBlobManager, "Can't get BlobManager");

		// Render the floating endpoint off the old view just before it changes
		if (msg->State == DocViewMsg::SELABOUTTOCHANGE)
		{
			if (msg->pNewDocView!=NULL && pBezTool!=NULL)
				pBlobManager->RenderToolBlobsOff(pBezTool, pBezTool->MoveToSpread, NULL);
		}

		// Render the floating endpoint onto the new view
		if (msg->State == DocViewMsg::SELCHANGED)
		{
			if (msg->pNewDocView!=NULL && pBezTool!=NULL)
				pBlobManager->RenderToolBlobsOn(pBezTool, pBezTool->MoveToSpread, NULL);
		}
	}
	else if (MESSAGE_IS_A(Message,NewPathCreatedMsg)) 
	{
		// When this message arrives the OpNewPath is still active so we can insert an
		// InsertFloaterAction into its undo record.
		DocCoord OldPos;
		Spread*  OldpSpread;
		Document* OldpDoc;
		NewPathCreatedMsg* Msg = (NewPathCreatedMsg*)Message;

		if (pBezTool != NULL)
		{
			if (pBezTool->GetMoveTo(&OldpSpread, &OldPos, &OldpDoc))
			{
				ERROR3IF((OldpDoc != Document::GetCurrent()),"Current doc was not doc with the floating endpoint!");
				if (!InsertFloaterAction::DoInsert(Msg->CurrentOp, Msg->UndoActs, pBezTool, &OldPos, OldpSpread))
				{
					FailAndExecute();
				}
			}
			pBezTool->ClearMoveTo();
		}
	}
	else if (MESSAGE_IS_A(Message,PathEditedMsg)) 
	{
		// We can use this message to update the edit fields in the infobar
		PathEditedMsg* Msg = (PathEditedMsg*)Message;
		CurrentSpread = Msg->EditSpread;
		CurrentNodePath = NULL;			// We will rely on updating on the SelChange message at the end of the drag
		CurrentInkPath = Msg->EditPath;
		CurrentIndex = Msg->EndPoint;
		UpdatePositionFields(TRUE, FALSE);
		
		/*
		// DY 27/9/99 if we are editing a blend on a path then update the number of blend steps
		NodeBlend* pNodeBlend = GetBlendOnCurve();
		if (pNodeBlend != NULL)
		{
			
			if (pNodeBlend->GetEditState() == EDIT_DISTANCE)
			{
				UINT32 NumSteps = 0;
				double StepDistance = pNodeBlend->GetStepDistance();
				BOOL Valid = pNodeBlend->GetNumStepsFromDistance(StepDistance, &NumSteps);
				if (Valid)
				{
					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLENDSTEPS);
					if (pOpDesc != NULL)
					{
						OpParam Param((INT32)NumSteps,0);
						pOpDesc->Invoke(&Param);
					}
							
				}
			}
			else
				pNodeBlend->UpdateStepDistance();
				
		} */
//		IgnoreNextUpdate = TRUE;
	}

	// Pass the message on
	return (DialogBarOp::Message(Message));
}    



/********************************************************************************************

>	void BezToolInfoBarOp::UpdateState()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/94
	Purpose:	Overrides the empty UpdateState function provided by InformationBarOp
				making a call to the function in DialogBarOp.

*********************************************************************************************/

void BezToolInfoBarOp::UpdateState()
{
	DialogBarOp::UpdateState();
	UpdateTextIndicator();	
	UpdateLineButton();
	UpdateCurveButton();
	UpdateReversePathsButton ();
	UpdateSmoothButton();
	UpdateCuspButton();
	UpdatePositionFields();
	UpdateStartArrowButton();
	UpdateEndArrowButton();

}



/********************************************************************************************

>	NodeBlend* BezInfoBarOp::GetBlendOnCurve()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    
	returns:    pointer to the first nodeblend in the selection if successful, 
				NULL otherwise
	Purpose:	To determine if the current selection is a blend on a curve

	NOTE this function exists for the purposes of getting the editing of paths of
	blends on a curve to work.  it should probably be located elsewhere but for the
	time being...
	

*********************************************************************************************/

NodeBlend* BezToolInfoBarOp::GetBlendOnCurve()
{
	// first get the selection
	SelRange Sel(*( GetApplication()->FindSelection()));
	NodeBlend* pNodeBlend = NULL;
	NodeBlendPath* pNodeBlendPath = NULL;
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlendPath))
			{
				pNodeBlendPath = (NodeBlendPath*)pNode;
				break;
			}
			pNode = Sel.FindNext(pNode);
		}
	}

	if (pNodeBlendPath == NULL)  // if no blend node then there won't be a blender
		return NULL;
	else
		pNodeBlend = (NodeBlend*)pNodeBlendPath->FindParent();
	
	ERROR2IF(!(pNodeBlend->IS_KIND_OF(NodeBlend)), FALSE, "Node is a not a NodeBlend");
	return pNodeBlend;

}








/********************************************************************************************

>	void BezierTool::OnClick( DocCoord PointerPos, ClickType Click, 
								ClickModifiers ClickMods, Spread *pSpread )

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com> (via Rik)
	Created:	21/4/94
	Inputs:		PointerPos - The Coords (in spread coords) of the point where the mouse 
				button was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
				pSpread - the spread in which the click happened
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Click event for the Bezier Tool. It starts up a Bezier 
				Operation.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BezierTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	// Have a flag telling me what we have to do (either click on an existing point, 
	// create a new point on a path, create an all new path or reshape an existing line)
	// clickeffect is defined in the class header.

	INT32 PathPosition;		// index into the path of interesting elements
	NodeRenderableInk* WhichNode = NULL;
	INT32	NumPts;
	INT32	NumPaths;

	// Set the infobars spread pointer so if any kernel op wants to force edit field values
	// there is a spread to get units from.
	pBezToolInfoBarOp->CurrentSpread = pSpread;

	clickeffect WhatToDo = DetermineClickEffect(PointerPos, pSpread,&WhichNode, &PathPosition, &NumPaths, &NumPts);

	// So, like, WhatToDo tells me what to do, WhichNode points at the node that's involved
	// and PathPosition is the index into WhichNode of the element we are dealing with.
	// if WhatToDo = OnPoint we have to pass the click onto the path
	// if WhatToDo = AddSegment we have to add a curve or line segment to the given path.
	// we should also remember to do things with the selected points.
	// if WhatToDo = NewPath there wasn't a suitable selected path, so we have to start a new
	// path. A stupid person (i.e. me) might create a new path, but I think this could be done
	// better. When they click in this mode, I'll start an operation which drags out a single
	// line/curve element, but if they release immediately I won't insert anything - I'll set
	// the FloatingEndpoint flag and set the MoveToPoint coords to that point.

	// Take note of the else ifs - this should help to ensure that only one event is ever triggered
	// by an incoming click event. Care should be taken in ordering these clauses so that the correct 
	// results ensue from each click.

	// Special case: WhatToDo = ClosePath and the Adjust button is used. Instead of closing the path
	// we should select the point. Thus, we change it to OnPoint

	if ( WhatToDo == ClosePath && ClickMods.Adjust)
		WhatToDo = OnPoint;
	
	// Ordinary click from OnPoint
	if ( Click == CLICKTYPE_SINGLE && WhatToDo == OnPoint )
	{
		WhichNode->OnClick(PointerPos, Click, ClickMods, pSpread);

		// Need to clear any exiting floating endpoint condition
		if (FloatingEndpoint)
			RemoveFloater(&MoveToPoint, MoveToSpread, NULL);
	}

	// If the click is AddSegment, I should add a segment to the end of the given path (WhichNode,
	// returned by DetermineClickEffect(). Unlike the clicking on existing nodes, it is the tool's
	// job to initiate this operation

	else if (Click == CLICKTYPE_SINGLE && WhatToDo == AddSegment && !ClickMods.Adjust)
	{
		OpNodePathAddEndpoint* pOpAddEnd = new OpNodePathAddEndpoint;
		if (!pOpAddEnd)
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		else
			pOpAddEnd->DoStartDragEdit((NodePath*) WhichNode, PointerPos, pSpread, ClickMods, PathPosition, CreateCurve, FALSE, !CreateCusp);
	}

	else if (Click == CLICKTYPE_SINGLE && (WhatToDo == NewPath || (WhatToDo == AddSegment && ClickMods.Adjust)))
	{
		if (FloatingEndpoint)
		{	
			if (ClickMods.Adjust)
			{
				// Clear the floating endpoint condition
				RemoveFloater(&MoveToPoint, MoveToSpread, NULL);
			}
			else
			{
				// Handle adding of new path
				OpNewPath* pOpNew = new OpNewPath;
				if (!pOpNew)
					InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
				else
				{
					pOpNew->DoStartDragEdit(MoveToPoint, PointerPos, MoveToSpread, ClickMods, CreateCurve, !CreateCusp);

						
				
				}
			}
		}
		else
		{
			if (!OpSelectPathPoints::DragInProgress())
			{
				// Start a drag operation to marquee select points
				OpSelectPathPoints* pOpSelect = new OpSelectPathPoints;
				if (!pOpSelect)
				{
					InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
				}
				else
				{
					// Start the drag, giving a pointer back to this tool so that if
					// the drag was not really a drag, we can insert a floater.
					pOpSelect->DoDrag( PointerPos, pSpread, ClickMods.Adjust , this, ClickMods);
				}		
			} 
		}
	}

	else if ( Click == CLICKTYPE_DOUBLE && WhatToDo == OnPoint && !ClickMods.Adjust)
	{
		WhichNode->OnClick(PointerPos, Click, ClickMods, pSpread);
//		DialogBarOp::SetSystemStateChanged();		// Inform the system that greying needs updating
	}

	else if ( Click == CLICKTYPE_SINGLE && WhatToDo == ClosePath)
	{
		// Create the same operation as for adding an endpoint, but call it using
		// the flag that tells the operation to close the path.

		OpNodePathAddEndpoint* pOpAddEnd = new OpNodePathAddEndpoint;
		if (!pOpAddEnd)
		{
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		}
		else
		{
			// Start dragging the path. The TRUE flag at the end tells the operation 
			// to close the path
			// I have to flip the PathPosition index from the clicked-on point to the opposite
			// point so that the right segment is inserted
			((NodePath*)WhichNode)->InkPath.SetPathPosition(PathPosition);
			((NodePath*)WhichNode)->InkPath.FindStartOfSubPath();
			if (PathPosition == ((NodePath*)WhichNode)->InkPath.GetPathPosition())
			{
				((NodePath*)WhichNode)->InkPath.FindEndOfSubPath();
				PathPosition = ((NodePath*)WhichNode)->InkPath.GetPathPosition();
			}
			else
				PathPosition = ((NodePath*)WhichNode)->InkPath.GetPathPosition();

			pOpAddEnd->DoStartDragEdit((NodePath*) WhichNode, PointerPos, pSpread, ClickMods, PathPosition, CreateCurve, TRUE, !CreateCusp);
		}
	}
	else if (Click == CLICKTYPE_SINGLE && WhatToDo == ReshapeLine)
	{
		// Create the same operation as for adding an endpoint, but call it using
		// the flag that tells the operation to close the path.

		double pdist;
		INT32 tempel;
		((NodePath*) WhichNode)->InkPath.SqrDistanceToPoint(PointerPos, &tempel, &pdist);

		OpReshapeOrAddPoint* pOpReshape = new OpReshapeOrAddPoint;
		if (!pOpReshape)
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		else
			pOpReshape->DoStartDragEdit((NodePath*) WhichNode, PointerPos, pSpread, PathPosition, pdist);
		
	}
/*
	else if (Click == CLICKTYPE_DRAG && WhatToDo == NewPath && FloatingEndpoint && pSpread == MoveToSpread)
	{
		OpNodePathStartNew* pOpStartNew = new OpNodePathStartNew;
		if (!pOpStartNew)
		{
			if (IsUserName("Jim"))
			{
				TRACE( _T("Unable to create operation to start new path in Beztool\n"));
			}
			
			// Inform the user that we are out of memory
			InformError( _R(IDS_OUT_OF_MEMORY), _R(IDS_OK) );
		}
		else
		{
			// Start dragging the path
			pOpStartNew->DoStartDragEdit( PointerPos, pSpread );
		}
	}
*/

	else if ( Click == CLICKTYPE_DRAG && WhatToDo == OnPoint && (WhichNode != NULL) )
	{
		WhichNode->OnClick(PointerPos, Click, ClickMods, pSpread);
//		((NodeRenderableInk*)LastPathClicked)->OnClick(LastPointClicked,Click, ClickMods, LastSpreadClicked);
//		DialogBarOp::SetSystemStateChanged();		// Inform the system that greying needs updating
	}

	// Now update the cursor and status bar
	OnMouseMove(PointerPos, pSpread, ClickMods);
	SetModeFlag();
	ResetRetroSlider();
}


/********************************************************************************************

>	BOOL BezierTool::OnKeyPress(KeyPress* pKeyPress)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle keypress events for the Bezier Tool. If it is a keypress that it
				know about it starts up an appropiate operation.
				THIS IS BADLY WRONG AT THE MOMENT - WHEN THERE IS THE TECHNOLOGY FOR USER
				DEFINABLE KEY-SHORTCUTS THIS FUNCTION WILL NEED FIXING	

********************************************************************************************/

BOOL BezierTool::OnKeyPress(KeyPress* pKeyPress)
{
	// We don't want to know about key release or character events
	if (!pKeyPress->IsPress())
		return FALSE;

	// Comparing keypresses is exceeding greif!

	if (*pKeyPress == KeyPress(CAMKEY(TAB)))
	{
		CyclePathPoints(TRUE);
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(TAB), TRUE))
	{
		CyclePathPoints(FALSE);
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(RETURN)))
	{
		if (!AutoClosePaths())
			InformError();
		goto UsedKey;
	} 

	if ((pKeyPress->GetVirtKey() == CAMKEY(HOME)) && !pKeyPress->IsAdjust() &&
											!pKeyPress->IsConstrain() && !pKeyPress->IsAlternative())
	{
		HomePathPoints();
		goto UsedKey;
	} 

	if ((pKeyPress->GetVirtKey() == CAMKEY(END)) && !pKeyPress->IsAdjust() &&
											!pKeyPress->IsConstrain() && !pKeyPress->IsAlternative())
	{
		EndPathPoints();
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(Z)))
	{
		pBezToolInfoBarOp->HandleClickOnCuspButton(FALSE);
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(S)))
	{
		pBezToolInfoBarOp->HandleClickOnSmoothButton();
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(L)))
	{
		pBezToolInfoBarOp->HandleClickOnLineButton();
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(C)))
	{
		pBezToolInfoBarOp->HandleClickOnCurveButton();
		goto UsedKey;
	} 

	if (*pKeyPress == KeyPress(CAMKEY(B)))
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpBreakAtPoints));
		String_256		UIDesc;
		if (pOpDesc != NULL)
		{
			OpState	State = OpBreakAtPoints::GetState(&UIDesc, pOpDesc);
			if (!State.Greyed)
			{
				pOpDesc->Invoke();
				goto UsedKey;
			}
		}
	} 

	if ((pKeyPress->GetVirtKey() == CAMKEY(DELETE)) || (*pKeyPress == KeyPress(WXK_BACK)) ) 
	{
		// Run a delete selected points operation
		String_256		UIDesc;
		OpState	State = OpDeletePoints::GetState(&UIDesc, NULL);
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpDeletePoints));
		if ((!State.Greyed) && (pOpDesc != NULL))
		{
			pOpDesc->Invoke();
	 		goto UsedKey;
		}
	} 

	// If ESCAPE is pressed then clear the floating endpoint, but don't claim the keypress
	// so the selection is also cleared.
	if ((*pKeyPress == KeyPress(CAMKEY(ESCAPE))) && FloatingEndpoint)
		RemoveFloater(&MoveToPoint, MoveToSpread, Document::GetSelected());

	// If we get this far then the keypress wasn't handled
	return FALSE;

UsedKey:
	pBezToolInfoBarOp->IgnoreNextUpdate = FALSE;
	return TRUE;
}



/**************************************************************************************************************

> clickeffect BezierTool::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread,
											NodeRenderableInk** ReturnNode, INT32* ReturnPosition,
											INT32* NumSelectedPaths, INT32* NumSelectedPoints )

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/94
	Inputs:		PointerPos is the mouse position
				pSpread is a pointer to the spread containing the mouse position
	Outputs:	ReturnNode returns a pointer to the node the mouse would click on
				ReturnPosition returns the index into the nodepath of the affected point
				NumSelectedPaths is the number of paths in the selection
				NumSelectedPoints is the total number of selected points (on all paths)
	Returns:	The effect of clicking - one of NewPath, AddSegment, OnPoint or ReshapeLine
	Purpose:	Used when the cursor moves and when single clicking. This routine determines
				what effect a click will have. In this tool, clicking will either select a point
				on an already selected line, reshape a line segment (a la Corel), add a segment to
				the end of a line, or start a new path entirely.

***************************************************************************************************************/

clickeffect BezierTool::DetermineClickEffect(DocCoord PointerPos, Spread* pSpread,
											NodeRenderableInk** ReturnNode, INT32* ReturnPosition,
											INT32* NumSelectedPaths, INT32* NumSelectedPoints )
{
	clickeffect WhatToDo = NewPath;	// tells me what effect the click would have
	INT32 PathPosition = 0;			// temp var for return value
	INT32 TempIndex = 0;				// Temporary index into a path
	INT32 NumSelPaths = 0;			// Number of selected paths
	double Distance = 0;			// gets the returned distance from the nearest point
	double Nearest = 0;				// distance of current nearest point
	NodeRenderableInk* WhichNode = NULL;	// Saves me using a pointer to a pointer

	// Scan through the selected paths, and see if any of them want the click

	DocRect BlobRect;
	DocView* pDocView = DocView::GetCurrent();
	ERROR2IF( pDocView==NULL, NewPath, "BezierTool::DetermineClickEffect: Can't find current DocView");

	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();
	Node* pNode = Selected->FindFirst();

	INT32 NumSelectedEndpoints = 0;		// Count selected points in paths

	if (pNode != NULL)
	{
		Spread* NodeSpread = pNode->FindParentSpread();
		if (NodeSpread == pSpread)
		{
			// On the same spread, so see if the pointer is over an endpoint
			while ((pNode != NULL) && WhatToDo!=OnPoint)
			{
				NodePath* pActNode = FindEditablePath(pNode);
				if (pActNode)
				{

					// Now we know it's a NodePath, get a pointer to the Path object within it, so
					// we can find any endpoints
				
					Path* ThisPath = &((pActNode)->InkPath);

					// Increment the number of selected paths
					NumSelPaths++;
					
					// count the number of selected points on the path (excluding control points)
					INT32 NumCoords = ThisPath->GetNumCoords();
					PathFlags* Flags = ThisPath->GetFlagArray();

					for (INT32 i=0; i<NumCoords; i++)
					{
						if (Flags[i].IsSelected && Flags[i].IsEndPoint)
							NumSelectedEndpoints++;
					}

					if (ThisPath->FindStartOfPath())
					{
						// First, check to see if this click occurs on a selected point
						INT32 tempPos;
						if (ThisPath->FindNearestPoint(PointerPos,
													   POINTFLAG_ENDPOINTS |
													   POINTFLAG_CONTROLPOINTS |
													   POINTFLAG_ENDSFIRST,
													   &tempPos)
						   )
						{
							// The click occurred on one of the points on the line
							// so remember this path, the position, and the fact that
							// the user clicked on a point

							WhatToDo = OnPoint;
							WhichNode =	(NodeRenderableInk*)pActNode;
							PathPosition = tempPos;
						}
						else if (WhatToDo != OnPoint && ThisPath->PointCloseToLine(PointerPos, &tempPos))
						{
							WhatToDo = ReshapeLine;
							WhichNode =	(NodeRenderableInk*)pActNode;
							PathPosition = tempPos;
						}
						else if ((WhatToDo == AddSegment || WhatToDo == NewPath) && ThisPath->ClosestSelectedEndpoint(PointerPos,&TempIndex,&Distance))
						{
							// TempIndex is the index into the path for the closest selected endpoint
							// Distance is the distance
							if (WhatToDo == NewPath)
							{
								WhatToDo = AddSegment;
								WhichNode =	(NodeRenderableInk*)pActNode;
								Nearest = Distance;
								PathPosition = TempIndex;
							}
							else if (Nearest > Distance)
							{
								Nearest = Distance;
								WhichNode = (NodeRenderableInk*)pActNode;
								PathPosition = TempIndex;
							}
						}
					}
            	}
				// Now find the next selected node
				pNode = Selected->FindNext(pNode);
			}
		}
	}

	// WhatToDo tells us what the action will be
	// WhichNode points to the node we are dealing with
	// PathPosition is the index into that path of the element we are using

	// If WhatToDo == OnPoint and it's the end of a subpath, and the opposite
	// end is selected, and the path isn't closed we should change WhatToDo 
	// to be ClosePath

	if (WhatToDo == OnPoint && NumSelectedEndpoints == 1)
	{
		Path* ThisPath = &(((NodePath*)WhichNode)->InkPath);
		PathFlags* Flags = ThisPath->GetFlagArray();
		PathVerb* Verbs = ThisPath->GetVerbArray();
//		DocCoord* Coords = ThisPath->GetCoordArray();
		INT32 NumCoords = ThisPath->GetNumCoords();
		if (Verbs[PathPosition] == PT_MOVETO)			// Start of subpath
		{
			INT32 i = PathPosition;
			ThisPath->FindEndElOfSubPath(&i);				// i = index to end element
			
			if ((Flags[i].IsSelected) && !(Verbs[i] & PT_CLOSEFIGURE))
				WhatToDo = ClosePath;
		}
		else if (PathPosition+1 == NumCoords || Verbs[PathPosition+1] == PT_MOVETO)
		{
			if (!(Verbs[PathPosition] & PT_CLOSEFIGURE))
			{
				INT32 i = PathPosition;
				ThisPath->FindStartOfSubPath(&i);
				if (Flags[i].IsSelected)
					WhatToDo = ClosePath;
			}
		}
		// Now we must detect the case of an attempted closepath on a path consisting
		// of a moveto and one segment
		if ((WhatToDo == ClosePath) && ( (((NodePath*)WhichNode)->InkPath.GetNumCoords() == 2) ||
			((((NodePath*)WhichNode)->InkPath.GetNumCoords() == 4) && (Verbs[3] == PT_BEZIERTO)) ) )
		{
			WhatToDo = OnPoint;
		}
	}


	if ((WhatToDo == AddSegment || WhatToDo == ClosePath) && NumSelectedEndpoints > 1)
		WhatToDo = NewPath;

	if (WhatToDo == AddSegment || WhatToDo == ReshapeLine)
	{
		// If we're pointing at a curve, make sure we're pointing to the first element
		Path* ThisPath = &(((NodePath*)WhichNode)->InkPath);
		PathFlags* flags = ThisPath->GetFlagArray();
		while(!(flags[PathPosition].IsEndPoint ))
			PathPosition++;
		ThisPath->SetPathPosition(PathPosition);
		if (ThisPath->GetVerb() == PT_BEZIERTO)
			PathPosition-=2;
	}

	*ReturnPosition = PathPosition;
	*ReturnNode = WhichNode;
	*NumSelectedPoints = NumSelectedEndpoints;
	*NumSelectedPaths = NumSelPaths;
	return (WhatToDo);
}



/********************************************************************************************

>	NodePath* BezierTool::FindEditablePath(Node* pSelected)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		pSelected = A pointer to the selected node to check
	Outputs:	-
	Returns:	A pointer to an editable node path (or derived) object
				NULL if there isn't one.
	Purpose:	Finds the node to edit (if it exists). This node could actually be the
				selected node passed as a parameter or one of its children which the
				node wants to be editable.

********************************************************************************************/

NodePath* BezierTool::FindEditablePath(Node* pSelected)
{
 	if (pSelected->IsNodePath())
 		return ((NodePath*)pSelected);

 	return ((NodePath*)pSelected->HasEditableChild(CC_RUNTIME_CLASS(NodePath), NULL));
}




/********************************************************************************************

>	void BezierTool::OnMouseMove(DocCoord coord, Spread* pSpread, ClickModifiers mods)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/3/94
	Inputs:		coordinate of mouse move, pointer to spread containing coord, mouse modifiers
	Outputs:	-
	Returns:	-
	Purpose:	This routine is called whenever the mouse moves while we're in the freehand 
				tool. it sees what is under the pointer, and flips the cursor if clicking 
				will have a different effect. The rules are:

				Over space, no selection		- freehand cursor
				Over selected endpoint			- add freehand to line

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void BezierTool::OnMouseMove(DocCoord coord, Spread* pSpread, ClickModifiers mods)
{
	// Call DetermineClickEffect to see what a click will do at this position

	INT32 PathPosition;			// Needed to receive a return, not used otherwise
	NodeRenderableInk* node;	// again, only temporary
	INT32 NumPts;				// Number of selected points
	INT32 NumPaths;				// Number of selected paths
	clickeffect WhatToDo = DetermineClickEffect(coord, pSpread, &node, &PathPosition, &NumPaths, &NumPts);
	
	// Generate the status line help string
	String_256 status("");
	GenerateStatusLineText(&status, pSpread, coord, mods);
	GetApplication()->UpdateStatusBarText(&status);

	// Now change the cursor
	switch (WhatToDo)
	{
		case AddSegment:
			ChangeCursor(pcAddPathCursor);
			break;
		case NewPath:
			ChangeCursor(pcNewPathCursor);
			break;
		case OnPoint:
			ChangeCursor(pcMoveBezCursor);
			break;
		case ReshapeLine:
			ChangeCursor(pcReshapeLineCursor);
			break;
		case ClosePath:
			ChangeCursor(pcClosePathCursor);
			break;
	}
}


/********************************************************************************************

>	void BezierTool::ChangeCursor(Cursor* cursor)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/94
	Inputs:		ID of the cursor you want to flip to
	Outputs:	-
	Returns:	-
	Purpose:	Changes to the specified cursor. Will only change the cursor if it isn't already
				this cursor, so it doesn't flicker.
	Errors:		can fail if the cursor cannot be created - the cursor code will fail.
	SeeAlso:	-

********************************************************************************************/

void BezierTool::ChangeCursor(Cursor* cursor)
{
	// only change if this cursor is different from the current cursor
	if (cursor != MyCurrentCursor)
	{
		// set this cursor as the current cursor and immediately display it
		CursorStack::GSetTop(cursor, CurrentCursorID);
		// remember this is our current cursor
		MyCurrentCursor = cursor;
	}

}

/********************************************************************************************

>	void BezierTool::SetMoveTo(DocCoord MovePos, Spread* pSpread, Document* pDoc)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		MovePos is the coordinate of the virtual moveto
				pSpread points at the spread containing the coordinate
				pDoc pointer to the document that contains the endpoint
	Outputs:	-
	Returns:	-
	Purpose:	This function is called from an operation which tells the tool that there
				should be a virtual moveTo coordinate at the given point. This virtual coordinate
				looks much like the old single moveto used to in ArtWorks, with the added advantage
				that it doesn't really exist, so it can't cock up the tree like they used
				to do in ArtWorks.
	Errors:		-
	SeeAlso:	BezierTool::ClearMoveTo

********************************************************************************************/

void BezierTool::SetMoveTo(DocCoord MovePos, Spread* pSpread, Document* pDoc)
{
	// Set the member variables of the tool
	MoveToDoc = pDoc;
	MoveToPoint = MovePos;
	MoveToSpread = pSpread;
	FloatingEndpoint = TRUE;

	// Now display the blob
	if (IsCurrent())
	{
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr) BlobMgr->RenderToolBlobsOn(this, pSpread, NULL);
	}
}



/********************************************************************************************

>	void BezierTool::ClearMoveTo()

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Clears the floating endpoint condition.
	Errors:		-
	SeeAlso:	BezierTool::SetMoveTo

********************************************************************************************/

void BezierTool::ClearMoveTo()
{
	if (FloatingEndpoint && IsCurrent() && !DontDrawOnClearMoveTo)
	{
		BlobManager* pBlobManager = GetApplication()->GetBlobManager();
		ENSURE(pBlobManager, "Can't get BlobManager");
		pBlobManager->RenderToolBlobsOff(this, MoveToSpread,NULL);
	}
	FloatingEndpoint = FALSE;
}



/********************************************************************************************

>	BOOL BezierTool::GetMoveTo(Spread** ppSpread, DocCoord* pCoord)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/94
	Inputs:		ppSpread - pointer to a pointer to a spread
				pCoord - pointer to a DocCoord
	Outputs:	*ppSpread is set to point to the spread the floating endpoint is on
				*pCoord is set to the location of the floating endpoint
	Returns:	TRUE if there was a floating endpoint, FALSE if not (in this case there is 
				no outputs)
	Purpose:	Read the state of the floating endpoint
	Errors:		-
	SeeAlso:	BezierTool::SetMoveTo, BezierTool::ClearMoveTo

********************************************************************************************/

BOOL BezierTool::GetMoveTo(Spread** ppSpread, DocCoord* pCoord, Document** ppDoc)
{
	if (FloatingEndpoint)
	{
		*ppSpread = MoveToSpread;
		*pCoord = MoveToPoint;
		*ppDoc = MoveToDoc;
		return TRUE;
	}
	else
		return FALSE;
}



/********************************************************************************************

>	void BezierTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		Jim_Lynn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pSpread - The spread that the blob is to appear on
				pClipRect - Pointer to the rect that contains the blobs
	Purpose:	Renders the Tools Blobs. The only blob this tool renders is the floating endpoint
				which only appears the the flag is set.

********************************************************************************************/

void BezierTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
	// Can only draw the path if there is a path to draw
	if (FloatingEndpoint)
	{
		RenderRegion* pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
		while (pRegion)
		{
			// Draw a Cross Hair
			pRegion->SetLineColour(COLOUR_BEZIERBLOB);
			pRegion->SetFillColour(COLOUR_TRANS);
			pRegion->DrawBlob(MoveToPoint,BT_SELECTED);

			// Get the next region in the list
			pRegion = DocView::GetNextOnTop(pClipRect);
		}
	}	
}



/********************************************************************************************

>	void BezierTool::SetModeFlag()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/94
	Inputs:		-
	Outputs:	CurrentToolMode is set the the current tool mode
	Purpose:	Sets the current tool mode (new/add/change) and gets the infobar to redraw
				the text item that shows it.
				We are in New mode if there are no selected lines.  We are in Add mode if there
				is just one endpoint on the end od a path selected.  Otherwise we are in Change mode.

********************************************************************************************/

void BezierTool::SetModeFlag()
{
	SelRange*	Selected = GetApplication()->FindSelection();
	Node*	pNode = Selected->FindFirst();
//	CurrentMode OldMode = CurrentToolMode;
	CurrentToolMode = New;

	while ((pNode != NULL) && (CurrentToolMode != Change))
	{
		NodePath* pSelected = FindEditablePath(pNode);
		
		if (pSelected)
		{
			Path* ThisPath = &(pSelected->InkPath);
			PathFlags* Flags = ThisPath->GetFlagArray();
			INT32 UsedSlots = ThisPath->GetNumCoords();

			// Go through the points in this path summing the selected endpoints
			for (INT32 i=0; i<UsedSlots; i++)
			{	
				if (Flags[i].IsSelected && Flags[i].IsEndPoint)
				{
					if ( (i!=0) && (i!=(UsedSlots-1)) )
					{
						CurrentToolMode = Change;
						break;
					}
					else
					{
						if (CurrentToolMode == Add)
						{
							CurrentToolMode = Change;
							break;
						}
						else
							CurrentToolMode = Add;
					}
				}
			}
		}
		pNode = Selected->FindNext(pNode);
	}
	
	DialogBarOp::SetSystemStateChanged();
}




/********************************************************************************************

>	INT32 BezToolInfoBarOp::GetCurrentIndex()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/94
	Inputs:		-
	Returns:	The index of the Endpoint currently being displayed in the infobar.
	Purpose:	To enable other classes to see which endpoint is currently selected for display.

********************************************************************************************/
INT32 BezToolInfoBarOp::GetCurrentIndex()
{
	return CurrentIndex;
}



/********************************************************************************************

>	void BezToolInfoBarOp::UpdateTextIndicator()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/10/94
	Inputs:		-
	Purpose:	Updates the text in the "Add/Change/New" field.  If there is a floating
				endpoint then the field says "New", if the first or last point on an open
				path is selected then it says "Add", otherwise it says "Change"

********************************************************************************************/
void BezToolInfoBarOp::UpdateTextIndicator()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	String_16 DisplayWord;

	switch (pBezTool->CurrentToolMode)
	{
		case BezierTool::New:
			DisplayWord.Load(_R(IDS_BEZTOOL_NEW),Tool::GetModuleID(pBezTool->GetID()));
			break;
		case BezierTool::Add:
			DisplayWord.Load(_R(IDS_BEZTOOL_ADD),Tool::GetModuleID(pBezTool->GetID()));
			break;
		case BezierTool::Change:
			DisplayWord.Load(_R(IDS_BEZTOOL_CHANGE),Tool::GetModuleID(pBezTool->GetID()));
			break;
		default:
			ERROR3("Unknown tool mode encountered");
	}

	SetStringGadgetValue(_R(IDC_BEZTOOLADDCHANGE), DisplayWord);
	PaintGadgetNow(_R(IDC_BEZTOOLADDCHANGE));
}



/********************************************************************************************

>	void BezToolInfoBarOp::UpdateCurveButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		-
	Purpose:	Sets the grey and select status of the curve button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateCurveButton()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		SetLongGadgetValue(_R(IDC_BTN_MAKECURVE), pBezTool->CreateCurve, FALSE);
		EnableGadget(_R(IDC_BTN_MAKECURVE), TRUE);
	}
	else
	{
		OpState	State = OpMakeSegmentsCurves::GetState(NULL, NULL);

		EnableGadget(_R(IDC_BTN_MAKECURVE), !State.Greyed);
		SetLongGadgetValue(_R(IDC_BTN_MAKECURVE), (State.Ticked==FALSE ? 0 : 1), FALSE);
	}
}



/********************************************************************************************

>	void BezToolInfoBarOp::UpdateLineButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		-
	Purpose:	Sets the grey and select status of the line button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateLineButton()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		SetLongGadgetValue(_R(IDC_BTN_MAKELINE), !pBezTool->CreateCurve, FALSE);
		EnableGadget(_R(IDC_BTN_MAKELINE), TRUE);
	}
	else
	{
		OpState	State = OpMakeSegmentsLines::GetState(NULL, NULL);

		EnableGadget(_R(IDC_BTN_MAKELINE), !State.Greyed);
		SetLongGadgetValue(_R(IDC_BTN_MAKELINE), (State.Ticked==FALSE ? 0 : 1) , FALSE);
	}
}



/********************************************************************************************

>	void BezToolInfoBarOp::UpdateReversePathsButton ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/2000
	Inputs:		-
	Purpose:	Sets the grey and select status of the reverse paths button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateReversePathsButton ()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	/*if ((pBezTool->CurrentToolMode != BezierTool::Change) && (pBezTool->CurrentToolMode != BezierTool::Add))
	{
		SetLongGadgetValue(_R(IDC_BTN_BEZ_REVERSE_PATH), !pBezTool->CreateCurve, FALSE);
		EnableGadget(_R(IDC_BTN_BEZ_REVERSE_PATH), FALSE);
	}
	else
	{*/
		OpState	State = OpReversePath::GetState(NULL, NULL);

		EnableGadget(_R(IDC_BTN_BEZ_REVERSE_PATH), !State.Greyed);
	//	SetLongGadgetValue(_R(IDC_BTN_BEZ_REVERSE_PATH), (State.Ticked==FALSE ? 0 : 1) , FALSE);
	//}
}



/********************************************************************************************

>	void BezToolInfoBarOp::UpdateSmoothButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		-
	Purpose:	Sets the grey and select status of the smooth button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateSmoothButton()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		SetLongGadgetValue(_R(IDC_BTN_SMOOTHPOINT), !pBezTool->CreateCusp, FALSE);
		EnableGadget(_R(IDC_BTN_SMOOTHPOINT), TRUE);
	}
	else
	{
		BOOL	Smooth;
		BOOL	Cusp;

		if (ScanPointsForJoins(&Smooth, &Cusp))
		{
			SetLongGadgetValue(_R(IDC_BTN_SMOOTHPOINT), Smooth, FALSE);
			EnableGadget(_R(IDC_BTN_SMOOTHPOINT), TRUE);
		}
		else
		{
			SetLongGadgetValue(_R(IDC_BTN_SMOOTHPOINT), FALSE, FALSE);
			EnableGadget(_R(IDC_BTN_SMOOTHPOINT), FALSE);
		}
	}
}


/********************************************************************************************

>	void BezToolInfoBarOp::UpdateStartArrowButton()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		-
	Purpose:	Sets the grey and select status of the arrow extents button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateStartArrowButton()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;
#ifdef ARROWHEADS

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		SetLongGadgetValue(_R(IDC_BTN_BEZ_START_ARROW), FALSE, FALSE);
		EnableGadget(_R(IDC_BTN_BEZ_START_ARROW), TRUE);
	}
	else
	{
		SetLongGadgetValue(_R(IDC_BTN_BEZ_START_ARROW), FALSE, FALSE);
		EnableGadget(_R(IDC_BTN_BEZ_START_ARROW), TRUE);	
	}

#endif
}

/********************************************************************************************

>	void BezToolInfoBarOp::UpdateStartArrowButton()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		-
	Purpose:	Sets the grey and select status of the arrow extents button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateEndArrowButton()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;
#ifdef ARROWHEADS
	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		SetLongGadgetValue(_R(IDC_BTN_BEZ_END_ARROW), FALSE, FALSE);
		EnableGadget(_R(IDC_BTN_BEZ_END_ARROW), TRUE);
	}
	else
	{
		SetLongGadgetValue(_R(IDC_BTN_BEZ_END_ARROW), FALSE, FALSE);
		EnableGadget(_R(IDC_BTN_BEZ_END_ARROW), TRUE);	
	}
#endif
}

/********************************************************************************************

>	void BezToolInfoBarOp::UpdateCuspButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/94
	Inputs:		-
	Purpose:	Sets the grey and select status of the cusp button on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::UpdateCuspButton()
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
 		SetLongGadgetValue(_R(IDC_BTN_CUSPPOINT), pBezTool->CreateCusp, FALSE);
		EnableGadget(_R(IDC_BTN_CUSPPOINT), TRUE);
	}
	else
	{
		BOOL	Smooth;
		BOOL	Cusp;

		if (ScanPointsForJoins(&Smooth, &Cusp))
		{
			SetLongGadgetValue(_R(IDC_BTN_CUSPPOINT), Cusp, FALSE);
			EnableGadget(_R(IDC_BTN_CUSPPOINT), TRUE);
		}
		else
		{
			SetLongGadgetValue(_R(IDC_BTN_CUSPPOINT), FALSE, FALSE);
			EnableGadget(_R(IDC_BTN_CUSPPOINT), FALSE);
		}
	}
}



/********************************************************************************************

>	void BezToolInfoBarOp::UpdatePositionFields(BOOL ForceUpdate = FALSE, BOOL ScanForCurrent = TRUE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/1994
	Inputs:		ScanForCurrent - TRUE to scan the SelRange to update CurrentPath, CurrentIndex, etc..
								 FALSE to use the existing values
	Purpose:	Sets the grey and select status of the point position icons on the infobar.
				If there is one selected point on a path then the fields are enabled, in
				all other cases they are greyed.

********************************************************************************************/

void BezToolInfoBarOp::UpdatePositionFields(BOOL ForceUpdate, BOOL ScanForCurrent)
{
	// Exit now if there is no infobar
	if (WindowID==NULL || pBezTool==NULL)
		return;

	INT32	SelectedPoints = 0;
	if (ScanForCurrent)
	{
		SelRange* Selection = GetApplication()->FindSelection();
		Node* pNode = Selection->FindFirst();

		while ( (pNode != NULL) && (SelectedPoints<2) )
		{
			NodePath* pSelected = pBezTool->FindEditablePath(pNode);

			if (pSelected)
			{
				Path*	ThisPath = &(pSelected->InkPath);
				INT32	UsedSlots = ThisPath->GetNumCoords();
				PathFlags*	Flags = ThisPath->GetFlagArray();
				PathVerb*	Verbs = ThisPath->GetVerbArray();

				// Now scan the path and count the number of selected points
				for (INT32 i=0; i<UsedSlots; i++)
				{
					if (Flags[i].IsSelected && Flags[i].IsEndPoint)
					{
						// if it's a closed path the first point is selected
						// as well as the last. We have to account for this.
						if (! ((Verbs[i] == PT_MOVETO) && ThisPath->IsSubPathClosed(i)) )
						{
							SelectedPoints++;
							CurrentSpread = pSelected->FindParentSpread();
							CurrentNodePath = pSelected;
							CurrentInkPath = &(CurrentNodePath->InkPath);
							CurrentIndex = i;
						}
					}
				}
			}
			pNode = Selection->FindNext(pNode);
		}
	}

	String_8 NullString = _T("");
	BOOL EnableArray[NumberIconIDs];
	for (INT32 loop = 0; loop < NumberIconIDs; loop++)
		EnableArray[loop] = TRUE;

	if (IgnoreNextUpdate)
		IgnoreNextUpdate = FALSE;
	else
	{
		if ((SelectedPoints == 1) || !ScanForCurrent)
		{	// Enable and update things
			UpdateEditFieldsFromPath(CurrentInkPath, CurrentSpread, CurrentIndex, ForceUpdate, EnableArray);

			for (INT32 i = 0; i < NumberIconIDs; i++)
			{
				EnableGadget(AllIconIDs[i], EnableArray[i]);
			}
		}
		else
		{	// Disable and clear everything
			CurrentNodePath = NULL;
			CurrentInkPath = NULL;
			for (INT32 loop = 0; loop < NumberIconIDs; loop++)
				EnableArray[loop] = FALSE;
			SetStringGadgetValue(_R(IDC_PATH_EDIT_ENDPOINTX), NullString);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_ENDPOINTY), NullString);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_FIRSTX), NullString);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_FIRSTY), NullString);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_SECONDX), NullString);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_SECONDY), NullString);
			SetStringGadgetValue(_R(IDC_PATH_STATIC_FIRSTX), NullString);
			SetStringGadgetValue(_R(IDC_PATH_STATIC_FIRSTY), NullString);
			SetStringGadgetValue(_R(IDC_PATH_STATIC_SECONDX), NullString);
			SetStringGadgetValue(_R(IDC_PATH_STATIC_SECONDY), NullString);

			for (INT32 i = 0; i < NumberIconIDs; i++)
			{
				EnableGadget(AllIconIDs[i], FALSE);//EnableArray[i]);
			}
		}
	}
}



/********************************************************************************************
>	BOOL BezToolInfoBarOp::SetEdit(CGadgetID gid, INT32 nValue, Spread* pUnitSpread, BOOL PaintNow = FALSE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	25/1/95
	Inputs:		gid				gadget identifier
				nValue			the value to set in the edit field (in millipoints
				pUnitSpread		the spread containing the distance
				PaintNow - TRUE if the edit filed should be redrawn immediatly.
	Outputs:	-
	Returns:	TRUE if everything suceeded, FALSE if an error occured
	Purpose:	Converts the given value to the appropriate units, as contained in the
				spread, converts it to text and writes in into the edit-field.
	Errors:		-
	SeeAlso:	BezToolInfoBarOp::SetEditPosition
	
********************************************************************************************/

BOOL BezToolInfoBarOp::SetEditLength(CGadgetID gid, INT32 nValue, Spread* pSpread, BOOL PaintNow)
{
	ERROR2IF(pSpread == NULL, FALSE, "Spread was NULL in SetEditLength");

	// Get the dimension scaling object (units) associated with the given spread
	// and convert to its units.
	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) pSpread);
	ERROR2IF (pDimScale == NULL, FALSE, "Null DimScale* in SelectorInfoBarOp::SetEdit");

	// Convert to units & text and display.
	String_256 str;
	pDimScale->ConvertToUnits((INT32) nValue, &str);

	if (!UpdateStringGadgetValue(gid, &str))
		PaintNow = FALSE;

	if (PaintNow)
		PaintGadgetNow(gid);

	return TRUE;
}




/********************************************************************************************
>	BOOL BezToolInfoBarOp::SetEditPosition(CGadgetID gid, INT32 nValue, Spread* pUnitSpread, BOOL PaintNow = FALSE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (copied from the selector tool by JustinF)
	Created:	31/8/94
	Inputs:		gid				gadget identifier
				nValue			the value to set in the edit field (in millipoints
				pUnitSpread		the spread containing the object (so its units can be
								extracted).  If this parameter is NULL then no unit
								conversion is performed.
				PaintNow - TRUE if the edit filed should be redrawn immediatly.
	Outputs:	-
	Returns:	TRUE if everything suceeded, FALSE if an error occured
	Purpose:	Converts the given value to the appropriate units, as contained in the
				spread, converts it to text and writes in into the edit-field.
	Errors:		-
	SeeAlso:	SelectorInfoBarOp::SetEdit
	
********************************************************************************************/

BOOL BezToolInfoBarOp::SetEditPosition(CGadgetID gid, INT32 nValue, Spread* pSpread, BOOL PaintNow)
{
 	ERROR2IF(pSpread == NULL, FALSE, "Current spread was NULL");

	DocCoord	FakePoint(nValue, 0);
	String_256	XText;
	String_256	YText;

	pSpread->SpreadCoordToText(&XText, &YText, FakePoint);
	if (!UpdateStringGadgetValue(gid, &XText))
		PaintNow = FALSE;

	if (PaintNow)
		PaintGadgetNow(gid);

	return TRUE;
}



/********************************************************************************************
>	BOOL BezToolInfoBarOp::SetEditPosition(CGadgetID gidX, CGadgetID gidY, DocCoord loc,
														Spread* pSpread, BOOL PaintNow = FALSE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	25/1/95
	Inputs:		gidX - X coordinate gadget identifier
				gidY - Y coordinate gadget identifier
				loc - the SpreadCoord to show in the edit fields
				pUnitSpread - the spread containing the object (so its units can be extracted).
				PaintNow - TRUE if the edit field should be redrawn immediatly.
	Outputs:	-
	Returns:	TRUE if everything suceeded, FALSE if an error occured
	Purpose:	Converts the given coordinate to the appropriate units, as contained in the
				spread, converts it to text and writes in into the edit-fields.
	Errors:		-
	SeeAlso:	-
	
********************************************************************************************/

BOOL BezToolInfoBarOp::SetEditPosition(CGadgetID gidX, CGadgetID gidY, DocCoord loc, Spread* pSpread, BOOL PaintNow)
{
 	ERROR2IF(pSpread == NULL, FALSE, "Current spread was NULL");

	String_256	XText;
	String_256	YText;

	pSpread->SpreadCoordToText(&XText, &YText, loc);
	BOOL UpdatedX = UpdateStringGadgetValue(gidX, &XText) ;
	BOOL UpdatedY = UpdateStringGadgetValue(gidY, &YText) ;

	if (PaintNow)
	{
		if (UpdatedX)
			PaintGadgetNow(gidX);
		if (UpdatedY)
			PaintGadgetNow(gidY);
	}

	return TRUE;
}



/********************************************************************************************

>	void BezToolInfoBarOp::HandleClickOnCurveButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		-
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the curve button on the
				infobar.

********************************************************************************************/

void BezToolInfoBarOp::HandleClickOnCurveButton()
{
	if (pBezTool==NULL)
		return;
	
	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		pBezTool->CreateCurve = TRUE;
	}
	else
	{

		SelRange* Selected = GetApplication()->FindSelection();
		if (Selected != NULL)
		{
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpMakeSegmentsCurves));
			if (pOpDesc != NULL)
			{
				OpState State = OpMakeSegmentsCurves::GetState(NULL,pOpDesc);
				if (!State.Greyed)
					pOpDesc->Invoke();
			}
		}

		if (AttributeManager::LastAttrAppliedBecomesCurrent)
			pBezTool->CreateCurve = TRUE;
	}
	UpdateLineButton();
	UpdateCurveButton();
	UpdateCuspButton();
	UpdateSmoothButton();
}

/********************************************************************************************

>	void BezToolInfoBarOp::HandleClickOnStartArrowButton()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		-
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the arrow extends button
				on the infobar.

********************************************************************************************/
void BezToolInfoBarOp::HandleClickOnStartArrowButton()
{
	// invoke the op
#ifdef ARROWHEADS
	OpDescriptor * pOpDesc = 
		OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpChangeStartArrowExtendsDesc));
		
	if (pOpDesc)
	{
		pOpDesc->Invoke();
	}
#endif
}

/********************************************************************************************

>	void BezToolInfoBarOp::HandleClickOnEndArrowButton()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		-
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the arrow extends button
				on the infobar.

********************************************************************************************/
void BezToolInfoBarOp::HandleClickOnEndArrowButton()
{
	// invoke the op
#ifdef ARROWHEADS
	OpDescriptor * pOpDesc = 
		OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpChangeEndArrowExtendsDesc));
		
	if (pOpDesc)
	{
		pOpDesc->Invoke();
	}
#endif
}
	
/********************************************************************************************

>	void BezToolInfoBarOp::HandleClickOnLineButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		-
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the line button on the
				infobar.

********************************************************************************************/
void BezToolInfoBarOp::HandleClickOnLineButton()
{
	if (pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		pBezTool->CreateCurve = FALSE;
	}
	else
	{
		SelRange* Selected = GetApplication()->FindSelection();
		if (Selected != NULL)
		{
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpMakeSegmentsLines));
			if (pOpDesc != NULL)
			{
				OpState State = OpMakeSegmentsLines::GetState(NULL,pOpDesc);
				if (!State.Greyed)
					pOpDesc->Invoke();
			}
		}

		if (AttributeManager::LastAttrAppliedBecomesCurrent)
			pBezTool->CreateCurve = FALSE;
	}

	UpdateLineButton();
	UpdateCurveButton();
	UpdateCuspButton();
	UpdateSmoothButton();
}



/********************************************************************************************

>	void BezToolInfoBarOp::HandleClickOnReversePathButton ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/2000
	Inputs:		-
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the reverse path button
				on the infobar.

********************************************************************************************/

void BezToolInfoBarOp::HandleClickOnReversePathButton ()
{
	if (pBezTool==NULL)
	{
		return;
	}

/*	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		pBezTool->CreateCurve = FALSE;
	}
	else
	{*/

	// if this function is called - then the button MUST be enabled (thereby proving that we
	// have a selection).  BUT lets just make sure of this fact ....

		SelRange* Selected = GetApplication()->FindSelection();
		
		if (Selected != NULL)
		{
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor (CC_RUNTIME_CLASS (OpReversePath));
			
			if (pOpDesc != NULL)
			{
				OpState State = OpReversePath::GetState (NULL, pOpDesc);
				
				if (!State.Greyed)
				{
					pOpDesc->Invoke ();
				}
			}
		}

	/*	if (AttributeManager::LastAttrAppliedBecomesCurrent)
		{
			pBezTool->CreateCurve = FALSE;
		}*/
	//}
}



/********************************************************************************************

>	void BezToolInfoBarOp::HandleClickOnSmoothButton()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		-
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the smooth button on the
				infobar.

********************************************************************************************/
void BezToolInfoBarOp::HandleClickOnSmoothButton()
{
	if (pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		pBezTool->CreateCusp = FALSE;
	}
	else
	{
		BOOL	Smooth;
		BOOL	Cusp;

		if (ScanPointsForJoins(&Smooth, &Cusp))
			ScanPointsForToggleSmooth(TRUE, FALSE);

		if (AttributeManager::LastAttrAppliedBecomesCurrent)
			pBezTool->CreateCusp = FALSE;
	}
	UpdateCuspButton();
	UpdateSmoothButton();
}



/********************************************************************************************
>	void BezToolInfoBarOp::HandleClickOnCuspButton(BOOL WithConstrain)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		WithConstrain - TRUE if the control points should not be moved
	Returns:	-
	Purpose:	Called by the message handler to handle clicks on the cusp button on the
				infobar.
********************************************************************************************/
void BezToolInfoBarOp::HandleClickOnCuspButton(BOOL WithConstrain)
{
	if (pBezTool==NULL)
		return;

	if (pBezTool->CurrentToolMode != BezierTool::Change)
	{
		pBezTool->CreateCusp = TRUE;
	}
	else
	{
		BOOL Smooth = TRUE;
		BOOL Cusp = TRUE;

		if (ScanPointsForJoins(&Smooth, &Cusp))
			ScanPointsForToggleSmooth(FALSE, !WithConstrain);	// invert WithConstrain

		if (AttributeManager::LastAttrAppliedBecomesCurrent)
			pBezTool->CreateCusp = TRUE;
	}
	UpdateCuspButton();
	UpdateSmoothButton();
}



/********************************************************************************************
>	BOOL BezToolInfoBarOp::HandleBumpClick(INT32 GadgetID)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/95
	Inputs:		GadgetID - the gadget ID of the clicked bump button
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called by the message handler to handle clicks on any of the bump buttons.
	Errors:		If an invalid gadget code is detected or if current path is NULL
********************************************************************************************/
BOOL BezToolInfoBarOp::HandleBumpClick(CGadgetID GadgetID)
{
	ERROR2IF(((CurrentNodePath == NULL) || (CurrentInkPath == NULL) || (&(CurrentNodePath->InkPath) != CurrentInkPath)), FALSE, "HandleClickOnBump found CurrentPath(s) was NULL");	
//	PathVerb*	Verbs = CurrentInkPath->GetVerbArray();
//	DocCoord*	Coords = CurrentInkPath->GetCoordArray();

	UINT32 ChangeField = 0;			// ID of edit field with value
	BOOL IsCoordChange = FALSE;		// TRUE if is coord change
	BOOL IsLengthChange = FALSE;	// TRUE if is line length change
	BOOL IsAngleChange = FALSE;		// TRUE if is line angle change
	INT32 ChangeValue = 0;			// Amount of change

	//	switch (GadgetID)
	if (
		// Bumping the currently selected path endpoint
		(GadgetID == _R(IDC_PATH_BUMP_ENDX_LESS)) ||
		(GadgetID == _R(IDC_PATH_BUMP_ENDX_MORE)) ||
		(GadgetID == _R(IDC_PATH_BUMP_ENDY_LESS)) ||
		(GadgetID == _R(IDC_PATH_BUMP_ENDY_MORE))
		)
	{ 
		ERROR2IF(CurrentIndex == -1, FALSE, "Attempt to nudge unspecified endpoint");	
		IsCoordChange = TRUE;
		// Set the coord nudge ammount
		if ((GadgetID == _R(IDC_PATH_BUMP_ENDY_LESS)) || (GadgetID == _R(IDC_PATH_BUMP_ENDX_MORE)))
			ChangeValue = OpNudge::GetNudgeStep();
		else
			ChangeValue = -OpNudge::GetNudgeStep();
		// Set the edit field
		if ((GadgetID == _R(IDC_PATH_BUMP_ENDX_LESS)) || (GadgetID == _R(IDC_PATH_BUMP_ENDX_MORE)))
			ChangeField = _R(IDC_PATH_EDIT_ENDPOINTX);
		else
			ChangeField = _R(IDC_PATH_EDIT_ENDPOINTY);
	}
	else if (
		// Bumping the previous endpoint
		(GadgetID == _R(IDC_PATH_BUMP_FIRSTX_LESS)) ||
		(GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE)) ||
		(GadgetID == _R(IDC_PATH_BUMP_FIRSTY_LESS)) ||
		(GadgetID == _R(IDC_PATH_BUMP_FIRSTY_MORE))
		)
	{
		if (CurrentInkPath->FindPrevControlPoint(CurrentIndex) != -1)
		{
			// Bumping control point position
			IsCoordChange = TRUE;
			// Set the coord nudge ammount
			if ((GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE)) || (GadgetID == _R(IDC_PATH_BUMP_FIRSTY_LESS)))
				ChangeValue = OpNudge::GetNudgeStep();
			else
				ChangeValue = -OpNudge::GetNudgeStep();
			// Set the edit field
			if ((GadgetID == _R(IDC_PATH_BUMP_FIRSTX_LESS)) || (GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE)))
				ChangeField = _R(IDC_PATH_EDIT_FIRSTX);
			else
				ChangeField = _R(IDC_PATH_EDIT_FIRSTY);
		}
		else
		{
			if ((GadgetID == _R(IDC_PATH_BUMP_FIRSTX_LESS)) || (GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE)))
			{
				// Change the length of the line by the bump amount		
				IsLengthChange = TRUE;
				// Set the length nudge ammount
				if (GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE))
					ChangeValue = OpNudge::GetNudgeStep();
				else
					ChangeValue = -OpNudge::GetNudgeStep();
				ChangeField = _R(IDC_PATH_EDIT_FIRSTX);
			}
			else
			{
				// Change the angle of the line by the bump amount		
				ERROR3IF((GadgetID != _R(IDC_PATH_BUMP_FIRSTY_LESS)) && (GadgetID != _R(IDC_PATH_BUMP_FIRSTY_MORE)),"What's that gadget?");
				IsAngleChange = TRUE;
				// Set the angle nudge ammount
				if (GadgetID == _R(IDC_PATH_BUMP_FIRSTY_MORE))
					ChangeValue = 5;
				else
					ChangeValue = -5;
				ChangeField = _R(IDC_PATH_EDIT_FIRSTY);
			}
		}
	}
	else if (
	// Bumping the next endpoint
		(GadgetID == _R(IDC_PATH_BUMP_SECONDX_LESS)) ||
		(GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE)) ||
		(GadgetID == _R(IDC_PATH_BUMP_SECONDY_LESS)) ||
		(GadgetID == _R(IDC_PATH_BUMP_SECONDY_MORE))
		)
	{
		if (CurrentInkPath->FindNextControlPoint(CurrentIndex) != -1)
		{
			// Bumping control point position
			IsCoordChange = TRUE;
			// Set the coord nudge ammount
			if ((GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE)) || (GadgetID == _R(IDC_PATH_BUMP_SECONDY_LESS)))
				ChangeValue = OpNudge::GetNudgeStep();
			else
				ChangeValue = -OpNudge::GetNudgeStep();
			// Set the edit field
			if ((GadgetID == _R(IDC_PATH_BUMP_SECONDX_LESS)) || (GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE)))
				ChangeField = _R(IDC_PATH_EDIT_SECONDX);
			else
				ChangeField = _R(IDC_PATH_EDIT_SECONDY);
		}
		else
		{
			if ((GadgetID == _R(IDC_PATH_BUMP_SECONDX_LESS)) || (GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE)))
			{
				// Change the length of the line by the bump amount		
				IsLengthChange = TRUE;
				// Set the length nudge ammount
				if (GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE))
					ChangeValue = OpNudge::GetNudgeStep();
				else
					ChangeValue = -OpNudge::GetNudgeStep();
				ChangeField = _R(IDC_PATH_EDIT_SECONDX);
			}
			else
			{
				// Change the angle of the line by the bump amount		
				ERROR3IF((GadgetID != _R(IDC_PATH_BUMP_SECONDY_LESS)) && (GadgetID != _R(IDC_PATH_BUMP_SECONDY_MORE)),"What's that gadget?");
				IsAngleChange = TRUE;
				// Set the angle nudge ammount
				if (GadgetID == _R(IDC_PATH_BUMP_SECONDY_MORE))
					ChangeValue = 5;
				else
					ChangeValue = -5;
				ChangeField = _R(IDC_PATH_EDIT_SECONDY);
			}
		}
	}

	// Now update the edit field
	BOOL Valid = TRUE;
	if (IsCoordChange || IsLengthChange)
	{
		// Read the current field value
		ERROR2IF(CurrentSpread == NULL, FALSE, "HandleClickOnBump found CurrentSpread was NULL");	
		DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
		ERROR2IF(pDimScale == NULL, FALSE, "NULL DimScalePtr");
		String_32 FieldContents = GetStringGadgetValue(ChangeField, &Valid);
		MILLIPOINT CurrentPos = 0;

		if (Valid)
			Valid = pDimScale->ConvertToMillipoints(FieldContents, &CurrentPos);
		if (Valid)
		{
			CurrentPos += ChangeValue;
			if (IsLengthChange && (CurrentPos<0))
				CurrentPos = 0;

			// Put this new value back into the edit field.
			SetEditLength(ChangeField, CurrentPos, CurrentSpread, TRUE);
		}
	}
	else if (IsAngleChange)
	{
		// Read the current field value
		double RequiredAngle = GetDoubleGadgetValue(ChangeField, -360.0, 360.0, _R(IDE_INVALID_ANGLE), &Valid);
		if (Valid)
		{
			RequiredAngle += ChangeValue;

			// Get the angle to lie between -180 and 180
			while (RequiredAngle < -180)
				RequiredAngle += 360;
			while (RequiredAngle > 180)
				RequiredAngle -= 360;

			SetDoubleGadgetValue(ChangeField, RequiredAngle);
		}
	}

	return TRUE;
}



/********************************************************************************************

>	void BezToolInfoBarOp::HandleEditFieldCommit(CGadgetID CommitField)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/1994
	Inputs:		CommitField is the gadget code of the field that was committed
	Outputs:	-
	Returns:	TRUE if the fields were read sucessfully, FALSE if they wern't
	Purpose:	Called by the message handler when RETURN is pressed in one of the point
				position edit fields.
	Errors:		Checks if CurrentPath and CurrentSpread are not NULL.

********************************************************************************************/
BOOL BezToolInfoBarOp::HandleEditFieldCommit(CGadgetID CommitField)
{
	ERROR2IF(((CurrentNodePath == NULL) || (CurrentInkPath == NULL)), FALSE, "HandleEditFieldCommit found CurrentPath was NULL");
	ERROR2IF(CurrentSpread == NULL, FALSE, "HandleEditFieldCommit found CurrentSpread was NULL");
	
	// Get path info
//	const INT32 NumCoords = CurrentInkPath->GetNumCoords();
	const INT32 FirstIndex = CurrentInkPath->FindPrevControlPoint(CurrentIndex);
	const INT32 SecondIndex = CurrentInkPath->FindNextControlPoint(CurrentIndex);
	DocCoord*	Coords = CurrentInkPath->GetCoordArray();

	INT32 LineAnglePrevIndex = GetPrevLineIndex(CurrentInkPath, CurrentIndex);
	INT32 LineAngleNextIndex = GetNextLineIndex(CurrentInkPath, CurrentIndex);
	if (LineAnglePrevIndex==-2 || LineAngleNextIndex==-2)
		return FALSE;

	BOOL Valid = TRUE;
	INT32	NumChanges = 0;
	ElementCoord NewPos[3];

	// Check the centre edit fields which is always the coordinate of the selected endpoint
	BOOL EndPointMoved = FALSE;
 	DocCoord NewEndPoint;
	DocCoord OldEndPoint = Coords[CurrentIndex];
	if (!HandleCoordFieldCommit(_R(IDC_PATH_EDIT_ENDPOINTX), _R(IDC_PATH_EDIT_ENDPOINTY), CurrentIndex, &NewEndPoint, &Valid))
		return FALSE;
	if (Valid && (OldEndPoint != NewEndPoint))
	{
		NewPos[NumChanges].Element = CurrentIndex;
		NewPos[NumChanges].Coordinate = NewEndPoint;
		NumChanges ++;
		EndPointMoved = TRUE;
	}

	// Now check the left hand pair, either the previous control point OR length & angle to previous line
	if (Valid && 0==NumChanges)
	{
		if (FirstIndex != -1)
		{
		 	DocCoord NewLeftPoint;
			DocCoord OldLeftPoint = Coords[FirstIndex];
			if (!HandleCoordFieldCommit(_R(IDC_PATH_EDIT_FIRSTX), _R(IDC_PATH_EDIT_FIRSTY), FirstIndex, &NewLeftPoint, &Valid))
				return FALSE;
			if (Valid && (OldLeftPoint != NewLeftPoint))
			{
				NewPos[NumChanges].Element = FirstIndex;
				NewPos[NumChanges].Coordinate = NewLeftPoint;
				NumChanges ++;
			}
		}
		else
		{
			if (LineAnglePrevIndex!=-1)
			{
				DocCoord OtherEnd = Coords[LineAnglePrevIndex];
				DocCoord NewLoc = OldEndPoint;
				if (!HandleLAFieldCommit(_R(IDC_PATH_EDIT_FIRSTX), _R(IDC_PATH_EDIT_FIRSTY), CommitField, OldEndPoint, OtherEnd, &NewLoc))
					return FALSE;
														 
				if (NewLoc != Coords[CurrentIndex])
				{
					NewPos[NumChanges].Element = CurrentIndex;
					NewPos[NumChanges].Coordinate = NewLoc;
					NumChanges++;
				}
			}
		}
	}

	// Now check the right hand pair
	if (Valid && 0==NumChanges)
	{
		if (SecondIndex != -1)
		{
		 	DocCoord NewRightPoint;
			DocCoord OldRightPoint = Coords[SecondIndex];
			if (!HandleCoordFieldCommit(_R(IDC_PATH_EDIT_SECONDX), _R(IDC_PATH_EDIT_SECONDY), SecondIndex, &NewRightPoint, &Valid))
				return FALSE;
			if (Valid && (OldRightPoint != NewRightPoint))
			{
				NewPos[NumChanges].Element = SecondIndex;
				NewPos[NumChanges].Coordinate = NewRightPoint;
				NumChanges ++;
			}
		}
		else
		{
			if (LineAngleNextIndex!=-1)
			{
				DocCoord OtherEnd = Coords[LineAngleNextIndex];
				DocCoord NewLoc = OldEndPoint;
				if (!HandleLAFieldCommit(_R(IDC_PATH_EDIT_SECONDX), _R(IDC_PATH_EDIT_SECONDY), CommitField, OldEndPoint, OtherEnd, &NewLoc))
					return FALSE;

				if (NewLoc != Coords[CurrentIndex])
				{
					NewPos[NumChanges].Element = CurrentIndex;
					NewPos[NumChanges].Coordinate = NewLoc;
					NumChanges++;
				}
			}
		}
	}

	// Now fire off a MovePoints operation to do the movement with undo.
	if (Valid)
	{
		if (NumChanges > 0)
		{
			// Accumulate the bounds of the changed point[s]
			DocRect NewBounds(NewPos[0].Coordinate.x, NewPos[0].Coordinate.y,
								NewPos[0].Coordinate.x, NewPos[0].Coordinate.y);

			for (INT32 loop = 1; loop < NumChanges; loop++)
				NewBounds.IncludePoint(NewPos[loop].Coordinate);

			// Expand the pasteboard as necessary to include any moved points
			// If this doesn't work, we'll tell the user below that the points still fall off
			// the available pasteboard area.
			BOOL AllOK = CurrentSpread->ExpandPasteboardToInclude(NewBounds);

			// Check that the new coords are on the spread
			DocRect SpreadBounds = CurrentSpread->GetPasteboardRect(FALSE);
			SpreadBounds = SpreadBounds.ToSpread(CurrentSpread, DocView::GetSelected());

			if (AllOK)
			{
				MovePointsParams MoveParams(CurrentNodePath, NewPos, NumChanges);

				OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpMovePathPoint));
				if (pOpDesc != NULL)
				{
					// Invoke the operation
					pOpDesc->Invoke(&MoveParams);

					// If the endpoint has been moved then scroll to show it
					if (EndPointMoved == TRUE)
					{
						DocView* pDocView = DocView::GetSelected();
						if (pDocView != NULL)
						{
							pDocView->ScrollToShowWithMargin(&NewEndPoint);
						}
					}
				}
			}
			else
			{
				Error::SetError(_R(IDE_MOVEPOINTSOFFSPREAD));
				InformError();
				UpdatePositionFields();
			}
		}
	}
	else
	{
		// Positions were not correct.  Inform user and reset
		Error::SetError(_R(IDE_PATHMOVE_DUFFFIELD));
		InformError();
		UpdatePositionFields();
	}
	return TRUE;
}



/********************************************************************************************
>	void BezToolInfoBarOp::ScanPointsForToggleSmooth(BOOL MakeSmooth, BOOL WithConstrain)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		MakeSmooth - TRUE if selected cusp points should be made smooth.
							 FALSE if selected smooth points should be made cusps.
				WithConstrain - TRUE if the control points should not be moved when making
								smooth cusp
	Returns:	-
	Purpose:	Called by the click handlers for the smooth and cusp buttons.  Calls 
				DoTogglePoint to do the work.
	SeeAlso:	OpToggleSmooth::DoTogglePoint
********************************************************************************************/
void BezToolInfoBarOp::ScanPointsForToggleSmooth(BOOL MakeSmooth, BOOL WithConstrain)
{
	OpToggleSmooth* pOpToggle = new OpToggleSmooth;
	if (!pOpToggle)
	{	// Inform the user that we are out of memory
		InformError();
	}
	else
	{	// Call the function that actually does something
		pOpToggle->DoTogglePoint(NULL, -1, NULL, MakeSmooth, WithConstrain);
	}
}



/********************************************************************************************

>	BOOL BezToolInfoBarOp::ScanPointsForJoins(BOOL* AllSmooth, BOOL* AllCusp)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		AllSmooth - pointer to a BOOL.
				AllCusp - pointer to a BOOL.
	Returns:	*AllSmooth is set to TRUE if all selected points are smooth.
				*AllCusp is set to TRUE if all selected points are not smooth.
				FALSE if there are no selected objects, otherwise true
	Purpose:	Scan the selected points on all paths to see the type of the points.

********************************************************************************************/

BOOL BezToolInfoBarOp::ScanPointsForJoins(BOOL* AllSmooth, BOOL* AllCusp)
{
	ERROR2IF(pBezTool==NULL, FALSE, "No tool pointer");

	SelRange*	Selected = GetApplication()->FindSelection();
	Node*		pNode = Selected->FindFirst();
	BOOL		FoundSome = FALSE;

	*AllSmooth = TRUE;
	*AllCusp = TRUE;
	
	while (pNode != NULL)
	{
		NodePath* pSelected = pBezTool->FindEditablePath(pNode);

		if (pSelected)
		{
			Path*	ThisPath = &(pSelected->InkPath);
			INT32	UsedSlots = ThisPath->GetNumCoords();
			PathFlags*	Flags = ThisPath->GetFlagArray();

			for (INT32 i=0; i<UsedSlots; i++)
			{
				if (Flags[i].IsSelected && Flags[i].IsEndPoint)
				{
					if (Flags[i].IsRotate)
						*AllCusp = FALSE;
					else
						*AllSmooth = FALSE;
					FoundSome = TRUE;
				}
			}
		}
		pNode = Selected->FindNext(pNode);
	}
	if (!FoundSome)
	{
		*AllSmooth = FALSE;
		*AllCusp = FALSE;
		return FALSE;
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL BezierTool::CyclePathPoints(BOOL Fowards)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Inputs:		Fowards - TRUE if the point selection status are to move fowards, otherwise
				they move backwards.
	Returns:	TRUE if all went well, FALSE if an error occured
	Purpose:	Performs the cycle the selected points task.  The selection status of each
				endpoint on selected paths is applied to the next endpoint.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BezierTool::CyclePathPoints(BOOL Fowards)
{   
	// Obtain the current selections and the first node in the selection
	SelRange*	Selected = GetApplication()->FindSelection();
	Node*		pNode = Selected->FindFirst();
	NodePath*	ThisPath;

	// Holds the coords of the last selected point we find on our travels,
	// we then call DocView::ScrolToShowWithMargin to ensure that this point is visible
	DocCoord LastSelectedPoint;
	BOOL LastSelectedPointValid = FALSE;	// does LastSelectedPoint hold a valid value?

	while (pNode != NULL)
	{	// we're only interested in NodePaths which have selected points
		NodePath* pSelected = FindEditablePath(pNode);
		if (pSelected)
		{
				// for convenience, cast the pointer to a pointer to a NodePath
				ThisPath = pSelected;

				// First get pointers to the arrays
				PathFlags*	Flags = ThisPath->InkPath.GetFlagArray();
				PathVerb*	Verbs = ThisPath->InkPath.GetVerbArray();
				DocCoord*	Coords = ThisPath->InkPath.GetCoordArray();
				const INT32 NumCoords = ThisPath->InkPath.GetNumCoords();
			
			if (pSelected->InkPath.IsSubSelection())
			{	
				// Render off the current selection blobs
				ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());

				if (Fowards)
				{	
					INT32 Current = 0;
					INT32 Next = 0;
					BOOL MoreEndpoints = TRUE;
					BOOL PrevSelected = Flags[0].IsSelected;

					while (MoreEndpoints)
					{
						// Find the next endpoint
						MoreEndpoints = ThisPath->InkPath.FindNextEndPoint(&Next);	
						if (MoreEndpoints && (Verbs[Next] & PT_CLOSEFIGURE))
							MoreEndpoints = ThisPath->InkPath.FindNextEndPoint(&Next);
						if (MoreEndpoints)
						{
							BOOL temp = Flags[Next].IsSelected;
							Flags[Next].IsSelected = PrevSelected;
							if (Flags[Next].IsSelected)
							{
							LastSelectedPoint = Coords[Next];
							LastSelectedPointValid = TRUE;
							}
							PrevSelected= temp;
							Current = Next;
						}
						else
						{
							Flags[0].IsSelected = PrevSelected;
							if (Flags[0].IsSelected)
							{
								LastSelectedPoint = Coords[0];
								LastSelectedPointValid = TRUE;
							}
						}
					}

					// Tidy up the selection state
					ThisPath->InkPath.EnsureSelection(TRUE);
				}
				else														
				{
					INT32 Current = NumCoords-1;
					INT32 Next = Current;
					BOOL MoreEndpoints = TRUE;
					BOOL PrevSelected = Flags[Current].IsSelected;
					BOOL ClosedPath = (Verbs[Next] & PT_CLOSEFIGURE);

					while (MoreEndpoints)
					{
						// Find the previous endpoint
						MoreEndpoints = ThisPath->InkPath.FindPrevEndPoint(&Next);	
						if (MoreEndpoints && ClosedPath && (Verbs[Next] == PT_MOVETO))
							MoreEndpoints = ThisPath->InkPath.FindPrevEndPoint(&Next);
						if (MoreEndpoints)
						{
							BOOL temp = Flags[Next].IsSelected;
							Flags[Next].IsSelected = PrevSelected;
							if (Flags[Next].IsSelected)
							{
								LastSelectedPoint = Coords[Next];
								LastSelectedPointValid = TRUE;
							}
							PrevSelected = temp;
							Current = Next;
						}
						else
						{
							Flags[NumCoords-1].IsSelected = PrevSelected;
							if (Flags[NumCoords-1].IsSelected)
							{
								LastSelectedPoint = Coords[NumCoords-1];
								LastSelectedPointValid = TRUE;
							}
						}
					}

					// Tidy up the selection state
					ThisPath->InkPath.EnsureSelection(FALSE);
				}

			}
			// If no SubSelection
			else
			{
				if (Fowards) 
				{
					Flags[0].IsSelected = TRUE;
					ThisPath->InkPath.EnsureSelection(TRUE);
				}
				else
					Flags[NumCoords-1].IsSelected = TRUE; 
					ThisPath->InkPath.EnsureSelection(FALSE);
		    }
	
			// Render on the new selection blobs
			ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());
		}
		pNode = Selected->FindNext(pNode);
	}
	DialogBarOp::SetSystemStateChanged();		
	if (LastSelectedPointValid)
	{
		DocView* pDocView = DocView::GetSelected();
		if (pDocView != NULL)
		{
			pDocView->ScrollToShowWithMargin(&LastSelectedPoint);
		}
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL BezierTool::HomePathPoints()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/94
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if an error occured
	Purpose:	Moves the selected endpoints back along the selected path(s) so that the first
				point is selected.  The pattern of selection remains the same.
	Errors:		-
	SeeAlso:	BezierTool::EndPathPoints

********************************************************************************************/
BOOL BezierTool::HomePathPoints()
{   
	// Obtain the current selections and the first node in the selection
	SelRange*	Selected = GetApplication()->FindSelection();
	Node*		pNode = Selected->FindFirst();
	NodePath*	ThisPath;

	// Variable to hold the position of the last 'first point'. This is updated as we
	// loop through the selection until it holds the first point of the last line
	// in the selection with subselection.
	DocCoord LastFirstPoint;
	BOOL LastFirstPointValid = FALSE;

	while (pNode != NULL)
	{	// we're only interested in NodePaths which have selected points
		NodePath* pSelected = FindEditablePath(pNode);
		if (pSelected) 
		{
			// for convenience, cast the pointer to a pointer to a NodePath
			ThisPath = pSelected;

			// First get pointers to the arrays
			PathFlags*	Flags = ThisPath->InkPath.GetFlagArray();
			PathVerb*	Verbs = ThisPath->InkPath.GetVerbArray();
			DocCoord*	Coords = ThisPath->InkPath.GetCoordArray();
			INT32	NumCoords = ThisPath->InkPath.GetNumCoords()-1;
			// We only need to bother if the first endpoint is not selected!
			if (pSelected->InkPath.IsSubSelection())		
			{
				if (!Flags[0].IsSelected)
				{
					// Render off the current selection blobs
					ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());

					// Find the first selected point
					INT32 Offset = 0;
					while ((Offset <= NumCoords) && !(Flags[Offset].IsSelected && Flags[Offset].IsEndPoint))
					{
						Offset ++;
					}

					ERROR3IF(Offset > NumCoords,"No selected endpoint found when there was one");
				
					// Now we can move the selection back to the start
					BOOL MoreEndpoints = TRUE;
					INT32 Current = 0;

					while (MoreEndpoints)
					{
						Flags[Current].IsSelected = Flags[Offset].IsSelected;
						MoreEndpoints = ThisPath->InkPath.FindNextEndPoint(&Offset);	
						if (Verbs[Offset] & PT_CLOSEFIGURE)
							MoreEndpoints = ThisPath->InkPath.FindNextEndPoint(&Offset);
						if (MoreEndpoints)
						{
							ThisPath->InkPath.FindNextEndPoint(&Current);	
							if (Verbs[Current] & PT_CLOSEFIGURE)
							ThisPath->InkPath.FindNextEndPoint(&Current);	
						}
						else
							Current ++;
					}

					// Unselect the remaining endpoints from Current onwards
					while (Current <= NumCoords)
					{
						Flags[Current++].IsSelected = FALSE;
					}

					// Now fix up the selection status of the path
					ThisPath->InkPath.EnsureSelection(TRUE);

					// Render on the new selection blobs
					ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());
				}

				// current pNode is selected and has a seubselection, so update FirstPointCoord to
				// point to the first end point of this path
				LastFirstPoint = Coords[0];
				LastFirstPointValid = TRUE;
			}		
			else
			{
				Flags [0].IsSelected = TRUE;
				// Now fix up the selection status of the path
				ThisPath->InkPath.EnsureSelection(TRUE);
				// Render on the new selection blobs
				ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());	
			}
		}
		pNode = Selected->FindNext(pNode);
	}
	DialogBarOp::SetSystemStateChanged();		

	// scroll to show the last first point (if it exists) at the coordinates we remembered earlier
	if (LastFirstPointValid)
	{
		DocView* pDocView = DocView::GetSelected();
		if (pDocView != NULL)
		{
			pDocView->ScrollToShowWithMargin(&LastFirstPoint);
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL BezierTool::EndPathPoints()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/1/94
	Inputs:		-
	Returns:	TRUE if all went well, FALSE if an error occured
	Purpose:	Moves the selected endpoints foward along the selected path(s) so that the last
				point is selected.  The pattern of selection remains the same.
	Errors:		-
	SeeAlso:	BezierTool::HomePathPoints

********************************************************************************************/

BOOL BezierTool::EndPathPoints()
{   
	// Obtain the current selections and the first node in the selection
	SelRange*	Selected = GetApplication()->FindSelection();
	Node*		pNode = Selected->FindFirst();
	NodePath*	ThisPath;

	// Variable to hold the position of the last 'end point'. This is updated as we
	// loop through the selection until it holds the end point of the last line
	// in the selection with subselection.
	DocCoord LastEndPoint;
	BOOL LastEndPointValid = FALSE;

	while (pNode != NULL)
	{	// we're only interested in NodePaths which have selected points
			NodePath* pSelected = FindEditablePath(pNode);
			if (pSelected)
			{
				// for convenience, cast the pointer to a pointer to a NodePath
				ThisPath = pSelected;

				// First get pointers to the arrays
				PathFlags*	Flags = ThisPath->InkPath.GetFlagArray();
				PathVerb*	Verbs = ThisPath->InkPath.GetVerbArray();
				DocCoord*	Coords = ThisPath->InkPath.GetCoordArray();
				INT32	NumCoords = ThisPath->InkPath.GetNumCoords()-1;
				if (pSelected->InkPath.IsSubSelection() ) 
				{
					// We only need to bother if the last endpoint is not selected!
					if (!Flags[NumCoords].IsSelected)
					{
						// Render off the current selection blobs
						ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());

						// Find the last selected point
						INT32 Offset = NumCoords;
						while ((Offset > -1) && !(Flags[Offset].IsSelected && Flags[Offset].IsEndPoint))
					
						{
							Offset--;
						}

						ERROR3IF(Offset == -1,"No selected endpoint found when there was one");

						// Now we can move the selection on to the end
						INT32 Current = NumCoords;
						BOOL MoreEndpoints = TRUE;

						while (MoreEndpoints)
						{
							Flags[Current].IsSelected = Flags[Offset].IsSelected;
							MoreEndpoints = ThisPath->InkPath.FindPrevEndPoint(&Offset);	
							if (MoreEndpoints && (Verbs[Offset] & PT_CLOSEFIGURE))
								MoreEndpoints = ThisPath->InkPath.FindPrevEndPoint(&Offset);
							if (MoreEndpoints)
							{
								ThisPath->InkPath.FindPrevEndPoint(&Current);	
								if (Verbs[Current] & PT_CLOSEFIGURE)
									ThisPath->InkPath.FindPrevEndPoint(&Current);	
							}	
							else
								Current --;
						}

						// Unselect the remaining endpoints
						while (Current > -1)
						{
							Flags[Current--].IsSelected = FALSE;
						}

						// Now fix up the selections so if the starts of subpaths are selected then so are the ends
						ThisPath->InkPath.EnsureSelection(FALSE);
	
						// Render on the new selection blobs
						ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());
					}
				
					// this path is in the selection, and has a subselection, so remember its end point
					// position, as it may be the last end point
					LastEndPoint = Coords[NumCoords];
					LastEndPointValid = TRUE;
			}

			else
			{
					Flags[NumCoords].IsSelected = TRUE;
					// Now fix up the selections so if the starts of subpaths are selected then so are the ends
					ThisPath->InkPath.EnsureSelection(FALSE);
					// Render on the new selection blobs
					ThisPath->InkPath.RenderPathSelectedControlBlobs(pNode->FindParentSpread());		
			}								
		}
		pNode = Selected->FindNext(pNode);
	}
	DialogBarOp::SetSystemStateChanged();		

	// scroll to show the last end point (if it exists) at the coordinates we remembered earlier
	if (LastEndPointValid)
	{
		DocView* pDocView = DocView::GetSelected();
		if (pDocView != NULL)
		{
			pDocView->ScrollToShowWithMargin(&LastEndPoint);
		}
	}

	return TRUE;
}



/********************************************************************************************

>	BezierTool::RetroSmoothChanging(double smooth)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		smooth = smoothness level to use
	Returns:
	Purpose:	Scan the selection for a path to smooth. If we find only one path object
				selected then we should tell the retro smooth code to begin smoothing.
				Arh!, actually we need to be able to keep track of the selected object?
				possibly.... Surely its fast enough already to find the selected
				objects, there must be some caching going on somewhere. Ok, best thing to
				do is to make sure theres only one selected object and ignore the slider
				change if there isn't
	
********************************************************************************************/

void BezierTool::RetroSmoothChanging(double smooth)
{

	if (!RetroFlag)
	{
		Node* pNode = OneNodePathSelected();
		if (pNode == NULL)
			return;
	
		Spread* pSpread = pNode->FindParentSpread();
		if (pSpread == NULL)
			return;

		RetroFlag = TRUE;
		pRetroNode = pNode;
		pRetroSpread = pSpread;

	}

	// ok, at this stage we should have a path to work with.
	pSmooth->Changing((NodePath*)pRetroNode, pRetroSpread, smooth);
}



/********************************************************************************************

>	BezierTool::RetroSmoothFinished()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		
	Returns:
	Purpose:	Calls the retro smooth finalisation code to perform the completed
				smoothing action
	
********************************************************************************************/

void BezierTool::RetroSmoothFinished()
{
	// ok, at this stage we should have a path to work with.
	if (RetroFlag)
	{
		pSmooth->Finished();
		RetroFlag = FALSE;
	}
}


/********************************************************************************************

>	Node* BezierTool::OneNodePathSelected()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Returns:	a pointer to the node if there is a single node path object selected
				NULL if no nodepath or more than one object is selected
	Purpose:	Checks the selection and determins whether exactly one nodepath object
				is selected.

********************************************************************************************/

Node* BezierTool::OneNodePathSelected()
{
	// Find the selected range of objects
	SelRange* Selected = GetApplication()->FindSelection();

	Node* pNode = Selected->FindFirst();
	if (pNode == NULL)
		return NULL;
	Node* qNode = Selected->FindNext(pNode);
	if (qNode != NULL)
		return NULL;

	// ok there's only one selected object so what kind is it?
	if (pNode->GetRuntimeClass() != CC_RUNTIME_CLASS(NodePath))
		return NULL;

	return pNode;
}


/********************************************************************************************

	void BezierTool::ResetRetroSlider()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Returns:
	Purpose:	There has been a change in the selection so we need to update the retro
				slider state

********************************************************************************************/

void BezierTool::ResetRetroSlider()
{
	Node* pNode = OneNodePathSelected();
	Path* pPath = NULL;

	if (pNode)
		pPath = &(((NodePath*)pNode)->InkPath);
	
	if (pPath && (!pPath->IsSubSelection()))
		pPath=NULL;

	UpdateRetroSlider(pPath); 
}



/********************************************************************************************

 	void BezierTool::UpdateRetroSlider(Path* pPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pPath = a path pointer, if NULL	then the slider is set to zero
	Purpose:	Sets the retro smooth slider position for a given path.

********************************************************************************************/

void BezierTool::UpdateRetroSlider(Path* pPath)
{
	if (pSmooth && pPath)
	{
		double acc = pSmooth->ReturnCachedAccuracy(pPath);
		INT32 lacc = (INT32) (acc+0.5);
		RetroSmoothSet(lacc, TRUE);
	}
	else
		RetroSmoothSet(0, FALSE);
}



/********************************************************************************************

	void BezierTool::RetroSmoothSet(INT32 percent, BOOL Enabled)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		percent	= the value to set the smooth slider to	(0..100)
	Returns:
	Purpose:	Set the position of the retro smooth slider, given a percentage value.
				The function will also update the percent text field.

********************************************************************************************/

void BezierTool::RetroSmoothSet(INT32 percent, BOOL Enabled)
{
	// Set the pip position
	if (pBezToolInfoBarOp)
	{
		if (pBezToolInfoBarOp->IsOpen())
		{
			pBezToolInfoBarOp->SetLongGadgetValue(_R(IDC_SMOOTHSLIDER), percent);
			
			// Set the percentage string
			TCHAR Str[32];
			String_32 jcf(_R(IDS_PERCENT_FORMAT));
			camSnprintf(Str, 31, jcf, (INT32) percent);
			String_32 PercentStr(Str);
			pBezToolInfoBarOp->SetStringGadgetValue(_R(IDC_SMOOTHPERCENT), PercentStr);

			pBezToolInfoBarOp->EnableGadget(_R(IDC_SMOOTHSLIDER), Enabled);
			pBezToolInfoBarOp->EnableGadget(_R(IDC_SMOOTHPERCENT), Enabled);

		}
	}
}




/********************************************************************************************

>	void BezierTool::RetroSmoothInvalidate()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		-
	Returns:
	Purpose:	Invalidate the retro smooth cached information.

********************************************************************************************/

void BezierTool::RetroSmoothInvalidate()
{
	// set the slider to full
	RetroSmoothSet(100, TRUE);
	// and tell the reto smooth op.
	if (pSmooth != NULL)
		pSmooth->Invalidate();
}



/*****************************************************************************

>	virtual BOOL BezierTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
											 DocCoord DocPos, ClickModifiers ClickMods);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)
	Errors:		ERROR3 if ptext is NULL

*****************************************************************************/

BOOL BezierTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods)
{
	ERROR2IF(ptext==NULL,FALSE,"ptext was NULL");

	*ptext = "";

	GenerateStatusLineText(ptext, pSpread, DocPos, ClickMods);

	return TRUE;
}



/*****************************************************************************

>	void BezierTool::GenerateStatusLineText(String_256* ptext, Spread* pSpread,
											 DocCoord DocPos, ClickModifiers ClickMods);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/94
	Inputs:		pSpread   - pointer to spread under mouse (else NULL)
				DocPos    - position of mouse in doc (in spread coords)
				ClickMods - mouse click modifiers
	Outputs:	ptext - text for status line
	Returns:	-
	Purpose:	To find the string to display on the status line given the
				current mouse position
	Errors:		-

*****************************************************************************/

void BezierTool::GenerateStatusLineText(String_256* ptext, Spread* pSpread, DocCoord coord, ClickModifiers mods)
{
	// Call DetermineClickEffect to see what a click will do at this position

	INT32 PathPosition;			// Needed to receive a return, not used otherwise
	NodeRenderableInk* node;	// again, only temporary
	INT32 NumPts;				// Number of selected points
	INT32 NumPaths;				// Number of selected paths
	clickeffect WhatToDo = DetermineClickEffect(coord, pSpread, &node, &PathPosition, &NumPaths, &NumPts);

	switch (WhatToDo)
	{
	case AddSegment:
		if (CreateCurve)
			ptext->Load(_R(IDS_ADD_SEGMENT),Tool::GetModuleID(GetID()));
		else
			ptext->Load(_R(IDS_ADDLINESEGMENT),Tool::GetModuleID(GetID()));
		break;
	case NewPath:
		if (FloatingEndpoint)
		{
			if (CreateCurve)
			{
				if (NumPts == 0)
					ptext->Load(_R(IDS_CLICK_TO_MAKE_NEW),Tool::GetModuleID(GetID()));
				else
					ptext->Load(_R(IDS_CLICK_TO_MAKE_NEW),Tool::GetModuleID(GetID()));
			}
			else
			{
				ptext->Load(_R(IDS_CLICKMAKENEWLINE),Tool::GetModuleID(GetID()));
			}
		}
		else
		{
			if (CreateCurve)
			{
				if (NumPts == 0)
					ptext->Load(_R(IDS_STARTNEWCURVENOSEL),Tool::GetModuleID(GetID()));
				else
					ptext->Load(_R(IDS_CREATE_NEW_PATH),Tool::GetModuleID(GetID()));
			}
			else
			{
				if (NumPts == 0)
					ptext->Load(_R(IDS_STARTNEWLINENOSEL),Tool::GetModuleID(GetID()));
				else
					ptext->Load(_R(IDS_STARTNEWLINE),Tool::GetModuleID(GetID()));
			}
		}
		break;
	case OnPoint:
		// There are various different combinations of selected/unselected, which require
		// different status line messages
		{
			NodePath* ThisPath = (NodePath*)node;
//			PathVerb* Verbs = ThisPath->InkPath.GetVerbArray();
			PathFlags* Flags = ThisPath->InkPath.GetFlagArray();
			INT32 ptype = 0;
			if (Flags[PathPosition].IsSelected)
				ptype |= 1;
			if (Flags[PathPosition].IsEndPoint)
				ptype |= 2;
			if (Flags[PathPosition].IsRotate)
				ptype |= 4;

			// Now ptype holds a number from 0 to 7 which completely describes what type of
			// point this is, so we can use that to select the correct string
			switch(ptype)
			{
				case 0:		//Unselected, unsmoothed control point
				case 4:		// UnSelected, smoothed control point 
				case 1:		// Selected, unsmoothed control point
				case 5:		// Selected, smoothed control point
					ptext->Load(_R(IDS_SELCONTROL),Tool::GetModuleID(GetID()));
					break;
				case 2:		// UnSelected, unsmoothed end point
					ptext->Load(_R(IDS_UNSELCUSP),Tool::GetModuleID(GetID()));
					break;
				case 3:		// Selected, unsmoothed end point
					ptext->Load(_R(IDS_SELCUSP),Tool::GetModuleID(GetID()));
					break;
				case 6:		// UnSelected, smoothed end point
					ptext->Load(_R(IDS_UNSELSMOOTH),Tool::GetModuleID(GetID()));
					break;
				case 7:		// Selected, smoothed end point
					ptext->Load(_R(IDS_SELSMOOTH),Tool::GetModuleID(GetID()));
					break;
			}
		}
		break;
	case ReshapeLine:
		ptext->Load(_R(IDS_RESHAPE_LINE),Tool::GetModuleID(GetID()));
		break;
	case ClosePath:
		ptext->Load(_R(IDS_CLOSEPATH),Tool::GetModuleID(GetID()));
		break;
	}
}



/*****************************************************************************

>	void BezToolInfoBarOp::UpdateEditFieldsFromPath(Path*	pPath,
													Spread*	pSpread,
													INT32	Index
													BOOL	ForceUpdate,
													BOOL	EnableData[]);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Inputs:		pPath - points to the path to display the data from
				pSpread - pointer to spread to use in corordinate display
				Index - the index of the endpoint in the path being edited
				ForceUpdate - TRUE if the changed gadgets should be immediatly redraw
				EnableData - Points to an array of BOOLs to gadget enabling
							 Can be NULL
	Outputs:	Items in EnableData are set to FALSE if they should be greyed.
	Returns:	-
	Purpose:	Updates the contents of the edit fields in the infobar from
				the positions in the path
	Errors:		-

*****************************************************************************/
void BezToolInfoBarOp::UpdateEditFieldsFromPath(Path* pPath, Spread* pSpread, INT32 Index, BOOL ForceUpdate,  BOOL EnableData[NumberIconIDs])
{
	// Error Checks
	if ((WindowID == NULL) || (pPath == NULL) || (pSpread == NULL) || (pBezTool == NULL))
		return;

	INT32 LineAnglePrevIndex = GetPrevLineIndex(pPath, Index);
	INT32 LineAngleNextIndex = GetNextLineIndex(pPath, Index);
	if (LineAnglePrevIndex==-2 || LineAngleNextIndex==-2)
	{
		InformError();
		return;
	}

//	PathVerb* Verbs = pPath->GetVerbArray();
	DocCoord* Coords = pPath->GetCoordArray();
//	INT32 UsedSlots = pPath->GetNumCoords();
	String_8 NullString(_T(""));
	String_8 UpperString(_T(""));
	String_8 LowerString(_T(""));

	SetEditPosition(_R(IDC_PATH_EDIT_ENDPOINTX), _R(IDC_PATH_EDIT_ENDPOINTY), Coords[Index], pSpread, ForceUpdate);

	INT32 FirstIndex = pPath->FindPrevControlPoint(Index);
	INT32 SecondIndex = pPath->FindNextControlPoint(Index);

	if (FirstIndex != -1)
	{
		// It's a curve
		SetEditPosition(_R(IDC_PATH_EDIT_FIRSTX), _R(IDC_PATH_EDIT_FIRSTY), Coords[FirstIndex], CurrentSpread, ForceUpdate);

		// Set the field indicators to XY
		UpperString.Load(_R(IDS_LINE_X_FIELD),Tool::GetModuleID(pBezTool->GetID()));
		LowerString.Load(_R(IDS_LINE_Y_FIELD),Tool::GetModuleID(pBezTool->GetID()));
		// Set the bubble and status help on the fields
		SetGadgetHelp(_R(IDC_PATH_EDIT_FIRSTX), _R(IDBBL_PATH_PREVCONTROLPOINT), _R(IDS_PATH_PREVCONTROLPOINT_X)); 
		SetGadgetHelp(_R(IDC_PATH_EDIT_FIRSTY), _R(IDBBL_PATH_PREVCONTROLPOINT), _R(IDS_PATH_PREVCONTROLPOINT_Y)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTX_LESS), _R(IDBBL_PATH_BUMPLOWER_X), _R(IDS_PATH_BUMPCOORDLOWER_X)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTX_MORE), _R(IDBBL_PATH_BUMPHIGHER_X), _R(IDS_PATH_BUMPCOORDHIGHER_X)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTY_MORE), _R(IDBBL_PATH_BUMPLOWER_Y), _R(IDS_PATH_BUMPCOORDLOWER_Y)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTY_LESS), _R(IDBBL_PATH_BUMPHIGHER_Y), _R(IDS_PATH_BUMPCOORDHIGHER_Y)); 
	}
	else
	{
		// It's a line
		if (LineAnglePrevIndex!=-1)
		{
			const double Distance = Coords[Index].Distance(Coords[LineAnglePrevIndex]);
			const double Angle = GetAngle(Coords[Index], Coords[LineAnglePrevIndex])*(180/PI);

			SetEditLength(_R(IDC_PATH_EDIT_FIRSTX), (INT32)Distance, pSpread, ForceUpdate);
			String_16 strX = GetStringGadgetValue(_R(IDC_PATH_EDIT_FIRSTX));
			DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
			if (pDimScale != NULL)
				pDimScale->ConvertToMillipoints(strX, &LastShownPrevLength); 
			SetDoubleGadgetValue(_R(IDC_PATH_EDIT_FIRSTY), Angle, FALSE);

			if ((Distance == 0.0) && (EnableData != NULL))
				EnableData[8] = FALSE;

			if (ForceUpdate)
				PaintGadgetNow(_R(IDC_PATH_EDIT_FIRSTY));

			// Set the field indicators to LA
			UpperString.Load(_R(IDS_LINE_L_FIELD),Tool::GetModuleID(pBezTool->GetID()));
			LowerString.Load(_R(IDS_LINE_A_FIELD),Tool::GetModuleID(pBezTool->GetID()));
			// Set the bubble and status help on the fields
			SetGadgetHelp(_R(IDC_PATH_EDIT_FIRSTX), _R(IDBBL_PATH_LINELENGTH), _R(IDS_PATH_PREVLINELENGTH)); 
			SetGadgetHelp(_R(IDC_PATH_EDIT_FIRSTY), _R(IDBBL_PATH_LINEANGLE), _R(IDS_PATH_PREVLINEANGLE)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTX_LESS), _R(IDBBL_PATH_BUMPLOWER_X), _R(IDS_PATH_BUMPLINELENGTHDOWN)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTX_MORE), _R(IDBBL_PATH_BUMPHIGHER_X), _R(IDS_PATH_BUMPLINELENGTHUP)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTY_MORE), _R(IDBBL_PATH_BUMPLOWER_Y), _R(IDS_PATH_BUMPLINEANGLEDOWN)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTY_LESS), _R(IDBBL_PATH_BUMPHIGHER_Y), _R(IDS_PATH_BUMPLINEANGLEUP)); 
		}
		else
		{
			SetStringGadgetValue(_R(IDC_PATH_EDIT_FIRSTX), NullString, FALSE);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_FIRSTY), NullString, FALSE);
			if (EnableData != NULL)
			{
				EnableData[8] = FALSE;
				EnableData[9] = FALSE;
				EnableData[10] = FALSE;
				EnableData[11] = FALSE;
			}
			if (ForceUpdate)
			{
				PaintGadgetNow(_R(IDC_PATH_EDIT_FIRSTX));
				PaintGadgetNow(_R(IDC_PATH_EDIT_FIRSTY));
			}

			// Clear the field indicators
			UpperString.Empty();
			LowerString.Empty();
			SetGadgetHelp(_R(IDC_PATH_EDIT_FIRSTX), 0, 0); 
			SetGadgetHelp(_R(IDC_PATH_EDIT_FIRSTY), 0, 0); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTX_LESS), 0, 0); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTX_MORE), 0, 0); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTY_MORE), 0, 0); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_FIRSTY_LESS), 0, 0); 
		}
	}

	// Redraw the left hand text indicators
	SetStringGadgetValue(_R(IDC_PATH_STATIC_FIRSTX), UpperString);
	SetStringGadgetValue(_R(IDC_PATH_STATIC_FIRSTY), LowerString);
	if (ForceUpdate)
	{
		PaintGadgetNow(_R(IDC_PATH_STATIC_FIRSTX));
		PaintGadgetNow(_R(IDC_PATH_STATIC_FIRSTY));
	}

	// Now do the right hand edit fields
	if (SecondIndex != -1)
	{
		SetEditPosition(_R(IDC_PATH_EDIT_SECONDX), _R(IDC_PATH_EDIT_SECONDY), Coords[SecondIndex], pSpread, ForceUpdate);

		// Set the field indicators to XY
		UpperString.Load(_R(IDS_LINE_X_FIELD),Tool::GetModuleID(pBezTool->GetID()));
		LowerString.Load(_R(IDS_LINE_Y_FIELD),Tool::GetModuleID(pBezTool->GetID()));
			// Set the bubble and status help on the fields
		SetGadgetHelp(_R(IDC_PATH_EDIT_SECONDX), _R(IDBBL_PATH_NEXTCONTROLPOINT), _R(IDS_PATH_NEXTCONTROLPOINT_X)); 
		SetGadgetHelp(_R(IDC_PATH_EDIT_SECONDY), _R(IDBBL_PATH_NEXTCONTROLPOINT), _R(IDS_PATH_NEXTCONTROLPOINT_Y)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDX_LESS), _R(IDBBL_PATH_BUMPLOWER_X), _R(IDS_PATH_BUMPCOORDLOWER_X)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDX_MORE), _R(IDBBL_PATH_BUMPHIGHER_X), _R(IDS_PATH_BUMPCOORDHIGHER_X)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDY_MORE), _R(IDBBL_PATH_BUMPLOWER_Y), _R(IDS_PATH_BUMPCOORDLOWER_Y)); 
		SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDY_LESS), _R(IDBBL_PATH_BUMPHIGHER_Y), _R(IDS_PATH_BUMPCOORDHIGHER_Y)); 
	}
	else
	{
		// It's a line
		if (LineAngleNextIndex!=-1)
		{
			const double Distance = Coords[Index].Distance(Coords[LineAngleNextIndex]);
			const double Angle = GetAngle(Coords[Index], Coords[LineAngleNextIndex])*(180/PI);

			SetEditLength(_R(IDC_PATH_EDIT_SECONDX), (INT32)Distance, pSpread, ForceUpdate);
			SetDoubleGadgetValue(_R(IDC_PATH_EDIT_SECONDY), Angle, FALSE);

			String_16 strX = GetStringGadgetValue(_R(IDC_PATH_EDIT_SECONDX));
			DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
			if (pDimScale != NULL)
				pDimScale->ConvertToMillipoints(strX, &LastShownNextLength); 

			if ((Distance == 0.0) && (EnableData != NULL))
				EnableData[14] = FALSE;

			if (ForceUpdate)
				PaintGadgetNow(_R(IDC_PATH_EDIT_SECONDY));

			// Set the field indicators to LA
			UpperString.Load(_R(IDS_LINE_L_FIELD),Tool::GetModuleID(pBezTool->GetID()));
			LowerString.Load(_R(IDS_LINE_A_FIELD),Tool::GetModuleID(pBezTool->GetID()));
			// Set the bubble and status help on the fields
			SetGadgetHelp(_R(IDC_PATH_EDIT_SECONDX), _R(IDBBL_PATH_LINELENGTH), _R(IDS_PATH_NEXTLINELENGTH)); 
			SetGadgetHelp(_R(IDC_PATH_EDIT_SECONDY), _R(IDBBL_PATH_LINEANGLE), _R(IDS_PATH_NEXTLINEANGLE)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDX_LESS), _R(IDBBL_PATH_BUMPLOWER_X), _R(IDS_PATH_BUMPLINELENGTHDOWN)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDX_MORE), _R(IDBBL_PATH_BUMPHIGHER_X), _R(IDS_PATH_BUMPLINELENGTHUP)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDY_MORE), _R(IDBBL_PATH_BUMPLOWER_Y), _R(IDS_PATH_BUMPLINEANGLEDOWN)); 
			SetGadgetHelp(_R(IDC_PATH_BUMP_SECONDY_LESS), _R(IDBBL_PATH_BUMPHIGHER_Y), _R(IDS_PATH_BUMPLINEANGLEUP)); 
		}
		else
		{
			SetStringGadgetValue(_R(IDC_PATH_EDIT_SECONDX), NullString, FALSE);
			SetStringGadgetValue(_R(IDC_PATH_EDIT_SECONDY), NullString, FALSE);
			if (EnableData != NULL)
			{
				EnableData[14] = FALSE;
				EnableData[15] = FALSE;
				EnableData[16] = FALSE;
				EnableData[17] = FALSE;
			}
			if (ForceUpdate)
			{
				PaintGadgetNow(_R(IDC_PATH_EDIT_SECONDX));
				PaintGadgetNow(_R(IDC_PATH_EDIT_SECONDY));
			}

			// Clear the field indicators
			UpperString.Empty();
			LowerString.Empty();
		}
	}

	// Redraw the right hand text indicators
	SetStringGadgetValue(_R(IDC_PATH_STATIC_SECONDX), UpperString);
	SetStringGadgetValue(_R(IDC_PATH_STATIC_SECONDY), LowerString);
	if (ForceUpdate)
	{
		PaintGadgetNow(_R(IDC_PATH_STATIC_SECONDX));
		PaintGadgetNow(_R(IDC_PATH_STATIC_SECONDY));
	}

	CurrentIndex = Index;
	CurrentSpread = pSpread;
}



/*****************************************************************************

>	void BezToolInfoBarOp::HandleLAFieldCommit(	CGadgetID LengthID,
												CGadgetID AngleID,
												CGadgetID CommitID,
												DocCoord SelectedPoint,
												DocCoord UnselectedPoint,
												DocCoord* Result)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		LengthID - gadget ID of the field with the new segment length
				AngleID - gagdet ID of the field with the new angle
				CommitID - gagdet ID of the field that was commited
				SelectedPoint - the coord of the selected endpoint
				UnselectedPoint - the coord of the unselected endpoint
				Result - pointer to the new position of the selected endpoint
	Outputs:	Result will hold the new location of the previous/next endpoint
	Returns:	-
	Purpose:	Common code for calculating the new position of the previous or
				next line endpoint from the given endpoint.
	Errors:		-

*****************************************************************************/
BOOL BezToolInfoBarOp::HandleLAFieldCommit(CGadgetID LengthID, CGadgetID AngleID, CGadgetID CommitID, 
					DocCoord SelectedPoint, DocCoord UnselectedPoint, DocCoord* Result)
{
	// Set return value to sensible value
	*Result = SelectedPoint;

	// Exit now if there is no infobar
	if (WindowID == NULL)
		return TRUE;

	// if the commited ID was not one of the two fields then exit
	if ((CommitID!=LengthID) && (CommitID!=AngleID))
		return TRUE;

	DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
	ERROR2IF(pDimScale == NULL, FALSE, "NULL DimScalePtr");

	// See what the edit fields were showing before the edit
	String_256 OldLength;
	String_256 OldAngle;
	const INT32 OldLengthVal = (INT32)(SelectedPoint.Distance(UnselectedPoint));
	const double OldAngleVal = GetAngle(UnselectedPoint, SelectedPoint)*(180/PI);
	pDimScale->ConvertToUnits((INT32) OldLengthVal, &OldLength);
	if (!Convert::DoubleToString(OldAngleVal, &OldAngle))
		return FALSE;

	// Get the current edit field strings
	INT32 RequiredLength = 0;
	double RequiredAngle = 0;
	BOOL Valid = TRUE;
	String_16 NewLength;
	String_16 NewAngle;
	if (Valid)
		NewLength = GetStringGadgetValue(LengthID, &Valid);
	if (Valid)
		NewAngle = GetStringGadgetValue(AngleID, &Valid);

	// Get the required length
	if (Valid && (NewLength != OldLength))
		Valid = pDimScale->ConvertToMillipoints(NewLength, &RequiredLength); 
	else
		RequiredLength = OldLengthVal;

	// Get the required angle
	if (Valid && (NewAngle != OldAngle))
	{
		RequiredAngle = GetDoubleGadgetValue(AngleID, -360.0, 360.0, _R(IDE_INVALID_ANGLE), &Valid);
		if (Valid)
		{
			// Get the angle to lie between -180 and 180
			while (RequiredAngle < -180)
				RequiredAngle += 360;
			while (RequiredAngle > 180)
				RequiredAngle -= 360;
		}

		// Flip the angle around
		RequiredAngle -= 180;
		if (RequiredAngle < -180)
			RequiredAngle += 360;
	}
	else
		RequiredAngle = OldAngleVal;


	// Generate a new position if fields have been edited
	if (Valid && ((RequiredLength != OldLengthVal) || (RequiredAngle != OldAngleVal)))
	{
		DocCoord NewLoc(0,0);
		RequiredAngle = RequiredAngle * (PI/180);

		NewLoc.x = (INT32)(RequiredLength*cos(RequiredAngle));
		NewLoc.y = (INT32)(RequiredLength*sin(RequiredAngle));
		*Result = UnselectedPoint + NewLoc;
	}

	return TRUE;
}



/*****************************************************************************

>	double BezToolInfoBarOp::GetAngle(DocCoord Origin, DocCoord Offset)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Inputs:		Origin - the point to calculate the angle around
				Offset - a point on the line to calculate the angle for
	Outputs:	-
	Returns:	The angle (in radians) from the positive x-axis and the line
	Purpose:	Common code for calculating the angle between two endpoints
	Errors:		-

*****************************************************************************/
double BezToolInfoBarOp::GetAngle(DocCoord Origin, DocCoord Offset)
{
	const DocCoord Outer = Offset - Origin;
	double Angle = atan2((double)Outer.y, (double)Outer.x);

	if (Angle == HUGE_VAL)
		return 0.0;
	else
		return Angle;
}



/*****************************************************************************
>	BOOL BezToolInfoBarOp::HandleCoordFieldCommit(UINT32 UpperID, UINT32 LowerID, INT32 Index, DocCoord* NewPos, BOOL* Valid)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/95
	Inputs:		UpperID - the resource ID of the upper edit field (X pos)
				LowerID - the resource IS of the lower edit field (Y pos)
				Index - the index of the cordinate being edited.
	Outputs:	NewPos - DocCoord set to the new coordinate
				Valid - TRUE/FALSE if the coord is valid (ie edit fields were ok)
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Common code for handling a commit in a pait of edit fields that
				contain a coordinate.  If the fields haven't been edited
				then no change is required
	Errors:		-
*****************************************************************************/
BOOL BezToolInfoBarOp::HandleCoordFieldCommit(UINT32 UpperID, UINT32 LowerID, INT32 Index,
																DocCoord* NewPos, BOOL* Valid)
{
	// Exit now if there is no infobar
	if (WindowID == NULL)
		return TRUE;

	// Check parameters and member variables
	ERROR2IF(((CurrentNodePath == NULL) || (CurrentInkPath == NULL)), FALSE, "HandleCoordFieldCommit found CurrentPath was NULL");
	ERROR2IF(&(CurrentNodePath->InkPath) != CurrentInkPath, FALSE, "HandleCoordFieldCommit found InkPath/NodePath mismatch");
	ERROR2IF(CurrentSpread == NULL, FALSE, "HandleCoordFieldCommit found CurrentSpread was NULL");
	ERROR2IF(Index < 0, FALSE, "-ve index supplied to HandleCoordFieldCommit");
	ERROR2IF(Index >= CurrentInkPath->GetNumCoords(), FALSE, "index supplied to HandleCoordFieldCommit was too large");
	
	DocCoord PreviousPos = CurrentInkPath->GetCoordArray()[Index]; 
	*NewPos = PreviousPos;
	BOOL FieldsValid = TRUE;

	// Work out what the fields showed before the edit
	String_256	OldXText;
	String_256	OldYText;
	if (!CurrentSpread->SpreadCoordToText(&OldXText, &OldYText, PreviousPos))
		return FALSE;

	// And what is actually in them now
	String_256	NewXText;
	String_256	NewYText;
	DocCoord NewPosition;
	NewXText = GetStringGadgetValue(UpperID, &FieldsValid);
	if (FieldsValid) 
		NewYText = GetStringGadgetValue(LowerID, &FieldsValid);
	if (FieldsValid)
		FieldsValid = CurrentSpread->TextToSpreadCoord(&NewPosition, &NewXText, &NewYText);

	// Update the coord
	if (FieldsValid)
	{
		if (OldXText != NewXText)
			NewPos->x = NewPosition.x;
		if (OldYText != NewYText)
			NewPos->y = NewPosition.y;
	}

	*Valid = FieldsValid;

	return TRUE;
}

BOOL BezToolInfoBarOp::CheckClassData ()
{
	if ((CurrentNodePath) && (CurrentInkPath) && (CurrentSpread))
	{
		return (TRUE);
	}
	return (FALSE);
}

/*****************************************************************************
>	MsgResult BezToolInfoBarOp::InfobarMessage(Msg* Message) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/95
	Inputs:		Msg - points to the message
	Outputs:	-
	Returns:	MsgResult code
	Purpose:	Deals with messages sent to the Shape editor tools infobar
	Errors:		-
	SeeAlso:	MsgResult
*****************************************************************************/

MsgResult BezToolInfoBarOp::InfobarMessage(Msg* Message) 
{
	ERROR2IF(!(IS_OUR_DIALOG_MSG(Message)), FAIL, "Message not sent to the shape editor infobar");
	ERROR2IF(pBezTool==NULL, FAIL, "NULL tool pointer");

	DialogMsg* Msg = (DialogMsg*)Message;

	if (Msg->DlgMsg == DIM_CANCEL)
	{
		Close();
	}
	else if (Msg->DlgMsg == DIM_CREATE)
	{
		// Set the range of the slider control
		SetGadgetRange(_R(IDC_SMOOTHSLIDER), 0, 100, 1);
		SetGadgetBitmaps(_R(IDC_SMOOTHSLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
		SetLongGadgetValue(_R(IDC_SMOOTHSLIDER), 0);
		
		// Set the percentage string
		TCHAR Str[32];
		String_32 jcf(_R(IDS_PERCENT_FORMAT));
		camSnprintf(Str, 31, jcf, (INT32) 0);
		String_32 PercentStr(Str);
		SetStringGadgetValue(_R(IDC_SMOOTHPERCENT), PercentStr);

		// Update all the buttons
		UpdateTextIndicator();	
		UpdateLineButton();
		UpdateCurveButton();
		UpdateReversePathsButton ();
		UpdateSmoothButton();
		UpdateCuspButton();
		UpdatePositionFields();
	}
	else if (Msg->DlgMsg == DIM_SELECTION_CHANGED)
	{
		if (CheckClassData())
		{
			if (!HandleEditFieldCommit(Msg->GadgetID))
				InformError();
		}
	}
	else if ((Msg->DlgMsg == DIM_LFT_BN_CLICKED) || (Msg->DlgMsg == DIM_LFT_BN_UP))
	{
		//switch (Msg->GadgetID)
	
		if (Msg->GadgetID == _R(IDC_BTN_MAKELINE))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				HandleClickOnLineButton();
		}
		else if (Msg->GadgetID == _R(IDC_BTN_MAKECURVE))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				HandleClickOnCurveButton();
		}
		else if (Msg->GadgetID == _R(IDC_BTN_SMOOTHPOINT))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				HandleClickOnSmoothButton();
		}
		else if (Msg->GadgetID == _R(IDC_BTN_CUSPPOINT))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
				HandleClickOnCuspButton(KeyPress::IsConstrainPressed());
		}
		else if (
			(Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTX_LESS)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTY_LESS)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTY_MORE)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_ENDX_LESS)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_ENDX_MORE)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_ENDY_LESS)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_ENDY_MORE)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDX_LESS)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDY_LESS)) ||
			(Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDY_MORE))
			)
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
			{
				if (!HandleBumpClick(Msg->GadgetID))
					InformError();
			}
			else
			{
				if (Msg->DlgMsg == DIM_LFT_BN_UP)
				{
					if (CheckClassData ())
					{
						// Pretend the user has pressed return in the field
						BOOL ok = TRUE;
						if ((Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTX_LESS)) || (Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTX_MORE)))
						{
							ok = HandleEditFieldCommit(_R(IDC_PATH_EDIT_FIRSTX));
						}
						else if ((Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTY_LESS)) || (Msg->GadgetID == _R(IDC_PATH_BUMP_FIRSTY_MORE)))
						{
							ok = HandleEditFieldCommit(_R(IDC_PATH_EDIT_FIRSTY));
						}
						else if ((Msg->GadgetID == _R(IDC_PATH_BUMP_ENDX_LESS)) || (Msg->GadgetID == _R(IDC_PATH_BUMP_ENDX_MORE)))
						{	
							ok = HandleEditFieldCommit(_R(IDC_PATH_EDIT_ENDPOINTX));
						}
						else if ((Msg->GadgetID == _R(IDC_PATH_BUMP_ENDY_LESS)) || (Msg->GadgetID == _R(IDC_PATH_BUMP_ENDY_MORE)))
						{
							ok = HandleEditFieldCommit(_R(IDC_PATH_EDIT_ENDPOINTY));
						}
						else if ((Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDX_LESS)) || (Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDX_MORE)))
						{
							ok = HandleEditFieldCommit(_R(IDC_PATH_EDIT_SECONDX));
						}
						else if ((Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDY_LESS)) || (Msg->GadgetID == _R(IDC_PATH_BUMP_SECONDY_MORE)))
						{
							ok = HandleEditFieldCommit(_R(IDC_PATH_EDIT_SECONDY));
						}

						if (!ok)
							InformError();
					}
				}
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_BEZ_REVERSE_PATH))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
			{
				HandleClickOnReversePathButton ();
			}
		}
#ifdef ARROWHEADS
		else if (Msg->GadgetID == _R(IDC_BTN_BEZ_END_ARROW))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
			{
				HandleClickOnEndArrowButton();
				UpdateEndArrowButton();
			}
		}
		else if (Msg->GadgetID == _R(IDC_BTN_BEZ_START_ARROW))
		{
			if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
			{
				HandleClickOnStartArrowButton();
				UpdateStartArrowButton();
			}
		}
#endif
	}
	else if (Msg->DlgMsg == DIM_SLIDER_POS_CHANGING)
	{
		// Messages to all the controls, handled individually
		if (Msg->GadgetID == _R(IDC_SMOOTHSLIDER))
		{
			// Find the current scrollers position
			TCHAR Str[32];
			BOOL Valid;
			INT32 Result = GetLongGadgetValue(_R(IDC_SMOOTHSLIDER), 0, 100, 0, &Valid);

			// Build the Percentage string and set it
			String_32 jcf(_R(IDS_PERCENT_FORMAT));
			camSnprintf(Str, 31, jcf, Result);
			String_32 PercentStr(Str);
			SetStringGadgetValue(_R(IDC_SMOOTHPERCENT), PercentStr);

			// Tell the bezier tool about the new smoothness
			if (Valid)
				// Set the smoothness and resmooth the points if possible
				pBezTool->RetroSmoothChanging(Result);
		}
	}
	else if (Msg->DlgMsg == DIM_SLIDER_POS_SET)
	{
		if (Msg->GadgetID == _R(IDC_SMOOTHSLIDER))
		{
			// Tell the tool that things are all finished with
			pBezTool->RetroSmoothFinished();
		}
	}

	return OK; 
}



/********************************************************************************************
>	BOOL BezierTool::AutoClosePaths()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all went well, FALSE if an error occured.
	Purpose:	Runs through all selected lines.  If the path is open and either of the ends
				are selected then the path is closed.
	SeeAlso:	-
********************************************************************************************/
BOOL BezierTool::AutoClosePaths()
{
	OpState Calcium = OpCloseNodePaths::GetState(NULL, NULL);

	if (!Calcium.Greyed)
	{
		OpParam Param(CreateCurve, !CreateCusp);
		OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCloseNodePaths));
		if (Apple != NULL)
			Apple->Invoke(&Param);
	}

	return TRUE;
}



/********************************************************************************************
>	INT32 BezToolInfoBarOp::GetPrevLineIndex(Path* pPath, INT32 Index)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/96
	Inputs:		pPath - points to the path
				Index - the index of the current endpoint
	Outputs:	-
	Returns:	The index of the previous line endpoint, -1 if there isn't one, -2 for error
	Purpose:	Gets the index of the 'other' end of the line ending at the current index,
				accounting for closed and sub-paths.
	SeeAlso:	INT32 BezToolInfoBarOp::GetNextLineIndex()
********************************************************************************************/
INT32 BezToolInfoBarOp::GetPrevLineIndex(Path* pPath, INT32 Index)
{
	if (pPath == NULL)
	{
		ERROR3("NULL path pointer");
		return -1;
	}

	// Get info on this subpath
	INT32 SubPathStart = Index;
	INT32 SubPathEnd = Index;
	pPath->FindStartOfSubPath(&SubPathStart);
	pPath->FindEndOfSubPath(&SubPathEnd);
	BOOL SubPathClosed = (pPath->GetVerbArray()[SubPathEnd] & PT_CLOSEFIGURE);

	// If the sub-path is closed and we're at the start then we must skip to the end
	if (SubPathClosed && (Index == SubPathStart) )
	{
		if ((pPath->GetVerbArray()[SubPathEnd] & ~PT_CLOSEFIGURE) == PT_LINETO)
			return SubPathEnd;
		else
			return -1;
	}

	// no worries about looping around ends of paths
	if ((Index > SubPathStart) && ((pPath->GetVerbArray()[CurrentIndex] & ~PT_CLOSEFIGURE) == PT_LINETO) )
		return Index-1;
	else
		return -1;
}



/********************************************************************************************
>	INT32 BezToolInfoBarOp::GetNextLineIndex()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/96
	Inputs:		pPath - points to the path
				Index - the index of the current endpoint
	Outputs:	-
	Returns:	The index of the next line endpoint, -1 if there isn't one
	Purpose:	Gets the index of the 'other' end of the line starting at the current index,
				accounting for closed and sub-paths.
	SeeAlso:	INT32 BezToolInfoBarOp::GetPrevLineIndex()
********************************************************************************************/
INT32 BezToolInfoBarOp::GetNextLineIndex(Path* pPath, INT32 Index)
{
	if (pPath == NULL)
	{
		ERROR3("NULL path pointer");
		return -1;
	}

	// Get info on this subpath
	INT32 SubPathStart = Index;
	INT32 SubPathEnd = Index;
	pPath->FindStartOfSubPath(&SubPathStart);
	pPath->FindEndOfSubPath(&SubPathEnd);
	BOOL SubPathClosed = (pPath->GetVerbArray()[SubPathEnd] & PT_CLOSEFIGURE);

	// If the sub-path is closed and we're at the end then we must skip to the start
	if (SubPathClosed && (Index == SubPathEnd) )
	{
		if ((pPath->GetVerbArray()[SubPathStart+1] & ~PT_CLOSEFIGURE) == PT_LINETO)
			return SubPathStart+1;
		else
			return -1;
	}

	// no worries about looping around ends of paths
	if ((Index < SubPathEnd) && ((pPath->GetVerbArray()[CurrentIndex+1] & ~PT_CLOSEFIGURE) == PT_LINETO) )
		return Index+1;
	else
		return -1;
}



/********************************************************************************************
>	void BezierTool::RemoveFloater(DocCoord* FloatPos, Spread* FloatSpread, Document* pDoc)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/96
	Inputs:		FloatPos - the current position of the floating endpoint
				FloatSpread - the spread it's on
				pDoc - points to document with the endpoint
	Outputs:	-
	Returns:	-
	Purpose:	Invokes the operation to remove the floating endpoint
	SeeAlso:	OpRemoveFloater
********************************************************************************************/
void BezierTool::RemoveFloater(DocCoord* FloatPos, Spread* FloatSpread, Document* pDoc)
{
	if (FloatingEndpoint)
	{	
		InsertFloaterParam Param(FloatPos, FloatSpread, this, pDoc);
		OpDescriptor* Apple = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpRemoveFloater));
		if (Apple != NULL)
			Apple->Invoke(&Param);
		else
			InformError();
	}
	else
	{
		ERROR3("Attempted to remove floating endpoint but it wasn't there!");
	}
}



