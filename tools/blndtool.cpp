// $Id: blndtool.cpp 668 2006-03-15 12:18:16Z alex $
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
// Implementation of the blend tool

/*
*/

#include "camtypes.h"
//#include "oilfiles.h"
#include "selop.h"
#include "csrstack.h"
//#include "markn.h"
#include "spread.h"
#include "nodepath.h"
#include "progress.h"
#include "nodeblnd.h"
#include "nodebldr.h"
//#include "simon.h"
#include "blobs.h"
//#include "blndres.h"
//#include "bevres.h"
#include "objchge.h"
//#include "resource.h"
//#include "will.h"
#include "filltool.h"
#include "bubbleid.h"
#include "becomea.h"
#include "attrmap.h"
#include "ndbldpth.h"
#include "pathedit.h"
#include "keypress.h"
#include "vkextra.h"

#include "blndtool.h"
//#include "ezmodule.h"
#include "opbevel.h"

//#include "will2.h"
#include "biasgdgt.h"
#include "opcntr.h"
//#include "cntrtool.h"
#include "shapeops.h"
#include "biasdlg.h"

//#include "will.h"


// for bevels & shadows
#include "nbevcont.h"
#include "nodecont.h"

#include "nodecntr.h"
#include "ncntrcnt.h"
#include "opbevel.h"
#include "layer.h"
#include "ophist.h"

DECLARE_SOURCE( "$Revision: 668 $" );
											 	
CC_IMPLEMENT_MEMDUMP(BlendTool,Tool_v1)
CC_IMPLEMENT_DYNCREATE(BlendInfoBarOp,InformationBarOp)
CC_IMPLEMENT_MEMDUMP(BlendToolRef,CC_CLASS_MEMDUMP)
CC_IMPLEMENT_DYNCREATE(OpBlendNodes,SelOperation)
CC_IMPLEMENT_DYNCREATE(OpRemoveBlend,SelOperation)
CC_IMPLEMENT_DYNCREATE(OpAddBlendPath,SelOperation)
CC_IMPLEMENT_DYNCREATE(OpDetachBlendPath,SelOperation)
CC_IMPLEMENT_DYNCREATE(OpChangeBlend,SelOperation)
CC_IMPLEMENT_DYNCREATE(OpBlendOneToOne,OpChangeBlend)
CC_IMPLEMENT_DYNCREATE(OpBlendAntialias,OpChangeBlend)
CC_IMPLEMENT_DYNCREATE(OpBlendTangential,OpChangeBlend)
CC_IMPLEMENT_DYNCREATE(OpChangeBlendSteps,SelOperation)
CC_IMPLEMENT_DYNCREATE(ChangeBlendAction,Action)
CC_IMPLEMENT_DYNCREATE(ChangeBlenderAction,Action)
CC_IMPLEMENT_DYNCREATE(ChangeBlendStepsAction,Action)
CC_IMPLEMENT_DYNCREATE(RemapBlendAction,Action)
CC_IMPLEMENT_DYNCREATE(InitBlendersAction,Action)
CC_IMPLEMENT_MEMDUMP(ChangeBlendOpParam,OpParam)
CC_IMPLEMENT_MEMDUMP(ChangeBlenderOpParam,OpParam)
CC_IMPLEMENT_DYNCREATE(InvalidateBoundsAction,Action)
CC_IMPLEMENT_DYNCREATE(OpChangeBlendDistance, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpEditBlendEndObject, SelOperation)
CC_IMPLEMENT_MEMDUMP(BlenderInfoItem,ListItem);



// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* BlendTool::FamilyName	= _T("Blend Tools");
TCHAR* BlendTool::ToolName 		= _T("Blend Tool");
TCHAR* BlendTool::Purpose 		= _T("Blend manipulation");
TCHAR* BlendTool::Author 		= _T("Mark Neves");

// Init those other useful static vars
BlendInfoBarOp*	BlendTool::pBlendInfoBarOp			= NULL;
BlendToolRef* 	BlendTool::pRefStart 				= NULL;
BlendToolRef* 	BlendTool::pRefEnd   				= NULL;
Cursor*			BlendTool::pcNormalCursor			= NULL;
Cursor*			BlendTool::pcBlendableCursor		= NULL;
Cursor*			BlendTool::pcBlendableBlobCursor	= NULL;
Cursor*			BlendTool::pcBlendableRemapCursor	= NULL;
Cursor*			BlendTool::pcCurrentCursor			= NULL;
INT32			BlendTool::CurrentCursorID			= 0;
UINT32			BlendTool::StatusID					= _R(IDS_BLENDSTATUS_FINDSTART);

//#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

#define SWAP(type,a,b) { type x=a; a=b; b=x; }

/********************************************************************************************

>	BlendTool::BlendTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Default Constructor.
				Other initialisation is done in BlendTool::Init which is called by the Tool Manager
	SeeAlso:	BlendTool::Init

********************************************************************************************/

BlendTool::BlendTool()
{
	pcCurrentCursor = NULL;
}

/********************************************************************************************

>	BlendTool::~BlendTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor.

********************************************************************************************/

BlendTool::~BlendTool()
{
	if (pRefStart != NULL)
	{
		delete pRefStart;
		pRefStart = NULL;
	}

	if (pRefEnd != NULL)
	{
		delete pRefEnd;
		pRefEnd = NULL;
	}
}


/********************************************************************************************

>	BOOL BlendTool::Init( INT32 Pass )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	BlendTool::BlendTool

********************************************************************************************/

BOOL BlendTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed

	BOOL ok = (	OpBlendNodes::Declare() 		&& 
				OpRemoveBlend::Declare() 		&& 
				OpAddBlendPath::Declare() 		&& 
				OpDetachBlendPath::Declare()	&& 
				OpChangeBlend::Declare() 		&& 
				OpBlendOneToOne::Declare()		&&
				OpBlendAntialias::Declare()		&&
				OpChangeBlendSteps::Declare());

	if (!ok) return FALSE;

	// We need two BlendToolRef objects
	BlendTool::pRefStart = new BlendToolRef;
	BlendTool::pRefEnd   = new BlendToolRef;

	ok = (BlendTool::pRefStart != NULL && BlendTool::pRefEnd != NULL);

	// This section reads in the infobar definition and creates an instance of
	// BlendInfoBarOp.  Also pBlendInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
		pBlendInfoBarOp = new BlendInfoBarOp();
		ok = (pBlendInfoBarOp != NULL);
#if 0
		CCResTextFile 		file;				// Resource File
		BlendInfoBarOpCreate BarCreate;			// Object that creates BlendInfoBarOp objects

		 		ok = file.open(_R(IDM_BLEND_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load blendbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_BLNDTOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

					ok = (pDialogBarOp != NULL);
			if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(BlendInfoBarOp));
			if (ok) pBlendInfoBarOp = (BlendInfoBarOp*)pDialogBarOp;

			ENSURE(ok,"Error finding the blend tool info bar");
		}
#endif
	}

	return (ok);
}


/********************************************************************************************

>	void BlendTool::Describe(void *InfoPtr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		InfoPtr - 	A pointer to a tool info block. It is passed cast to void* as
							the version of the tool is unknown at this point. Later versions 
							of the Tool class may have more items in this block, that this 
							tool will not use
	Outputs:	InfoPtr - 	The structure pointed to by InfoPtr will have had all the info
							that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void BlendTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_BLEND_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_BLEND_TOOLBOX);
}

/********************************************************************************************

>	virtual void BlendTool::SelectChange(BOOL isSelected)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		isSelected	- TRUE  = tool has been selected
							- FALSE = tool has been deselected
	Outputs:	-
	Returns:	-
	Purpose:	Starts up and closes down the blend tool
	Errors:		Debug warning if creating the cursor fails.
	SeeAlso:	-

********************************************************************************************/

void BlendTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		if (!CreateCursors()) return;
		CurrentCursorID = CursorStack::GPush(pcNormalCursor, FALSE);		// Push cursor but don't display now
		pcCurrentCursor = pcNormalCursor;

		// Create and display the tool's info bar
		pBlendInfoBarOp->Create();
		m_EditEndObject = FALSE;
		// Which blobs do I want displayed
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs we will display
			BlobStyle MyBlobs;
			
			MyBlobs.Object = TRUE;
			MyBlobs.Tiny = FALSE;

			BlobMgr->ToolInterest(MyBlobs);
		}

		Document* pDoc = Document::GetCurrent();
		if (pDoc != NULL)
			BlobMgr->RenderToolBlobsOn(this, pDoc->GetSelectedSpread(),NULL);
	}
	else
	{
		// Deselection - destroy the tool's cursors, if they exist.
		if (pcCurrentCursor != NULL)
		{
			CursorStack::GPop(CurrentCursorID);
			pcCurrentCursor = NULL;
			CurrentCursorID = 0;
		}
		DestroyCursors();

		// we need to close down any profile dialogs that are currently open ....
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX
		pBlendInfoBarOp->CloseProfileDialog (pBlendInfoBarOp->m_BiasGainGadgetPosition);
		pBlendInfoBarOp->CloseProfileDialog (pBlendInfoBarOp->m_BiasGainGadgetAttribute);
#endif
		// BEFORE we do the next call !!!! Cause otherwise pBlendInfoBarOp will
		// have been "deleted", and the above will access violate!

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		// Remove the info bar from view by deleting the actual underlying window
		pBlendInfoBarOp->Delete();

		Document* pDoc = Document::GetCurrent();
		if (pDoc != NULL)
			BlobMgr->RenderToolBlobsOff(this, pDoc->GetSelectedSpread(),NULL);
	}
}

/********************************************************************************************

>	BOOL BlendTool::CreateCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the blend tool cursors have been successfully created
	Purpose:	Creates all the blend tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL BlendTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalCursor 	  		= new Cursor(this, _R(IDC_BLENDNORMALCURSOR));
	pcBlendableCursor 		= new Cursor(this, _R(IDC_BLENDABLECURSOR));
	pcBlendableBlobCursor 	= new Cursor(this, _R(IDC_BLENDABLEBLOBCURSOR));
	pcBlendableRemapCursor 	= new Cursor(this, _R(IDC_BLENDABLEREMAPCURSOR));

	if ( pcNormalCursor       	==NULL || !pcNormalCursor->IsValid()    		||
	   	 pcBlendableCursor    	==NULL || !pcBlendableCursor->IsValid() 		||
	   	 pcBlendableBlobCursor	==NULL || !pcBlendableBlobCursor->IsValid()		||
	   	 pcBlendableRemapCursor	==NULL || !pcBlendableRemapCursor->IsValid()
	   )
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}

/********************************************************************************************

>	void BlendTool::DestroyCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the blend tool cursors
	SeeAlso:	-

********************************************************************************************/

void BlendTool::DestroyCursors()
{
	if (pcNormalCursor 	  	  	!= NULL) delete pcNormalCursor;
	if (pcBlendableCursor 	  	!= NULL) delete pcBlendableCursor;
	if (pcBlendableBlobCursor 	!= NULL) delete pcBlendableBlobCursor;
	if (pcBlendableRemapCursor 	!= NULL) delete pcBlendableRemapCursor;
}

/********************************************************************************************

>	BOOL BlendTool::OnKeyPress(KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle keypress events for the Blend Tool.

********************************************************************************************/

BOOL BlendTool::OnKeyPress(KeyPress* pKeyPress)
{
#if defined(_DEBUG) && 0
	if (pKeyPress == NULL)
		return FALSE;

	if (pKeyPress->IsRepeat())
		return FALSE;

	if (pKeyPress->IsRelease())
		return FALSE;

	AFp BiasDelta = 0.0;
	AFp GainDelta = 0.0;
	BOOL Reset = FALSE;
	if (*pKeyPress == KeyPress(CAMKEY(Z)))	{ BiasDelta = -0.1; TRACEUSER( "Markn", _T("Decrease Bias by 0.1\n"));}
	if (*pKeyPress == KeyPress(CAMKEY(X)))	{ BiasDelta =  0.1; TRACEUSER( "Markn", _T("Increase Bias by 0.1\n"));}
	if (*pKeyPress == KeyPress(CAMKEY(N)))	{ GainDelta = -0.1; TRACEUSER( "Markn", _T("Decrease Gain by 0.1\n"));}
	if (*pKeyPress == KeyPress(CAMKEY(M)))	{ GainDelta =  0.1; TRACEUSER( "Markn", _T("Increase Gain by 0.1\n"));}

	if (*pKeyPress == KeyPress(CAMKEY(R)))	{ Reset = TRUE;		TRACEUSER( "Markn", _T("Resetting Bias and Gain\n"));}

	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pNode = pSelRange->FindFirst();
	while (pNode)
	{
		if (IS_A(pNode,NodeBlend))
		{
			NodeBlend* pNodeBlend = (NodeBlend*)pNode;

			// This alters the Attribute profile, but can easily be modified to alter the Object profile if necessary
			CProfileBiasGain* pProfile = pNodeBlend->GetAttrProfile();
			if (pProfile)
			{
				AFp Bias = pProfile->GetBias() + BiasDelta;
				AFp Gain = pProfile->GetGain() + GainDelta;
				if (Reset)
					Bias = Gain = 0.0;

				if (Bias < -0.9)	Bias = -0.9;
				if (Bias >  0.9)	Bias =  0.9;
				if (Gain < -0.9)	Gain = -0.9;
				if (Gain >  0.9)	Gain =  0.9;

				pProfile->SetBiasGain(Bias,Gain);
			}
		}
		// Now find the next selected node
		pNode = pSelRange->FindNext(pNode);
	}
#endif // _DEBUG

	return FALSE;
}

/********************************************************************************************

>	void BlendTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Blend Tool.  If the click is over the 
				central blob of a blend on a path then start a EditEnd operation, otherwise
				start a createblend operation.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BlendTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

//#pragma message( __LOCMSG__ "BlendTool::OnClick - do nothing" )
//	TRACE( _T("Warning - BlendTool::OnClick called\n") );

	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

	if (Click == CLICKTYPE_DRAG)
	{
		// are we over the middle blob of a blend on a curve?
		BOOL MoveEndObject = EditBlendEnd(pSpread, PointerPos);

		// Diccon  9/99, do we wish to create a blend or move an object?
		if (!MoveEndObject)  // create a blend
		{
			UpdateRef(pRefStart,pSpread,PointerPos);
			UpdateRef(pRefEnd  ,pSpread,PointerPos);
			CheckNodeRemapping(pRefStart,pRefEnd);
			UpdateCursorAndStatus();

			if (pRefStart->pNode != NULL)
			{
				// Start a drag
				OpBlendNodes* pOpBlendNodes = new OpBlendNodes;
				if (pOpBlendNodes != NULL)
				{
					// Start the drag operation and pass in the Anchor Point to the push operation
					if  (!pOpBlendNodes->DoDrag(this))
						delete pOpBlendNodes;
				}
			}
		}
		else  // drag end object
		{	
			OpEditBlendEndObject* pEditEnd = new OpEditBlendEndObject(this);

			if (pEditEnd != NULL)
			{
				StatusID = _R(IDS_BLENDSTATUS_MOVEEND);
				DisplayStatusBarHelp(StatusID);
				pEditEnd->DoDrag(PointerPos, pSpread);
			}
		}
	}
	
	// call the base class ....
	
	DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
}

/********************************************************************************************

>	void BlendTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Blend Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void BlendTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

	pRefEnd->Reset();
	UpdateRef(pRefStart,pSpread,PointerPos,FALSE);
	// Diccon added - check first to see if there is a hit on an edit end blob
	m_EditEndObject = EditBlendEndAndUpdateCursor(pSpread, PointerPos);
	// if not then check for new blend/remapping etc.
	if (m_EditEndObject == FALSE)
		UpdateCursorAndStatus();
}


/********************************************************************************************

>	BOOL BlendTool::EditBlendEndAndUpdateCursor(Spread* pSpread, DocCoord PointerPos)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
	Returns:	TRUE if we can edit and end object, FALSE otherwise
	Purpose:	To determine whether or not the pointer is the central blob of an end object
				of a blend on a curve. If so then we change the cursor and set a flag that 
				allows us to begin an OpEditBlendEndobject if the user clicks and drags.
	SeeAlso:	OnMouseMove, OnClick

********************************************************************************************/

BOOL BlendTool::EditBlendEndAndUpdateCursor(Spread* pSpread, DocCoord PointerPos)
{
	Cursor* pcNewCursor = pcNormalCursor;

	List BlendList;
	// make a list of selected nodes
	BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	if (ok)
	{
		ok = FALSE;
		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		while (pListItem != NULL)
		{
			
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
			Node* pNode = NULL;
			// check to see if there is a hit
			ok = pNodeBlend->HitOnEndDragBlob(PointerPos, &pNode);
			if (ok)
			{
				pcNewCursor = pcBlendableBlobCursor;
				StatusID = _R(IDS_BLENDSTATUS_EDITENDS);						
				break;
			}
			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
		}
	}
	BlendList.DeleteAll();

	if (pcCurrentCursor != pcNewCursor)
	{
		// Current cursor has changed
		CursorStack::GSetTop(pcNewCursor, CurrentCursorID);
		pcCurrentCursor = pcNewCursor;
	}
	
	return ok;
}


/********************************************************************************************

>	BOOL BlendTool::EditBlendEnd(Spread* pSpread, DocCoord PointerPos)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
	Returns:	TRUE if we can edit and end object, FALSE otherwise
	Purpose:	To determine whether or not the pointer is the central blob of an end object
				of a blend on a curve. If so then we change the cursor and set a flag that 
				allows us to begin an OpEditBlendEndobject if the user clicks and drags.
	SeeAlso:	OnMouseMove, OnClick

********************************************************************************************/

BOOL BlendTool::EditBlendEnd(Spread* pSpread, DocCoord PointerPos)
{
	List BlendList;
	// make a list of selected nodes
	BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	if (ok)
	{
		ok = FALSE;
		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		while (pListItem != NULL)
		{
			
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
			Node* pNode = NULL;
			// check to see if there is a hit
			ok = pNodeBlend->HitOnEndDragBlob(PointerPos, &pNode);
			if (ok)
				break;
			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
		}
	}
	BlendList.DeleteAll();

	return ok;
}



/********************************************************************************************
>	void BlendTool::UpdateRef(	BlendToolRef* pRef,
								Spread* pSpread, 
								DocCoord PointerPos,
								BOOL CheckNodeUnderPoint = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		pRef 				= ptr to a reference struct to update
				pSpread  			= ptr to spread click occurred
				PointerPos 			= the DocCoord of the click
				CheckNodeUnderPoint = TRUE to revert to click-detection search for underlying node
	Outputs:	The members of pRef are updated.
	Returns:	-
	Purpose:	This will update the blend tool ref depending on the current pointer pos.
				If CheckNodeUnderPoint is TRUE, then the routine will revert to a click-detection search
				to find out which node lies under the given point.  This is potentially very time-consuming.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void BlendTool::UpdateRef(BlendToolRef* pRef,Spread* pSpread, DocCoord PointerPos,BOOL CheckNodeUnderPoint)
{
	ERROR3IF_PF(pRef   ==NULL,("pRef is NULL"));
	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

	// Reset the reference
	pRef->Reset();

	// See if point is over a blob of a selected path
	// if IsPointOverPathBlob() fails, it inits pNode to NULL and Index to -1
	if (!IsPointOverPathBlob(&PointerPos,pRef))
	{
		// Reset the reference in case above call altered it
		pRef->Reset();

		// See if point is over a blob is a selected blend
		// if IsPointOverBlendBlob() fails, it inits pNode & pNodeBlend to NULL and Index to -1
		if (!IsPointOverBlendBlob(&PointerPos,pRef))
		{
			// Reset the reference in case above call altered it
			pRef->Reset();

			if (CheckNodeUnderPoint)
			{
				// See if we are over an object
				NodeRenderableInk* pNodeUnderPoint = FindObject(pSpread,PointerPos);

				if (pNodeUnderPoint != NULL)
				{
					BecomeA TestBecomeA(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
					// We are over an object. Can we blend it?
					if (pNodeUnderPoint->CanBecomeA(&TestBecomeA))
					{
						// We are over a NodePath or a node that can become a NodePath
						pRef->pNode = pNodeUnderPoint;
					}
				}
			}
		}
	}

	// Set the spread and pointer pos members
	pRef->pSpread    = pSpread;
	pRef->PointerPos = PointerPos;
}

/********************************************************************************************
>	void BlendTool::UpdateCursorAndStatus()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This will update the cursor and status line text depending on the data in the 
				two blend tool references within BlendTool.

				Copes with the following scenarios:
					Pointer over an object
					Dragging to another object
					Pointer over a selected path's blob
					Dragging to another blob in a selected path
					Pointer over a blend blob
					Dragging to a corresponding blend blob for remapping

	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void BlendTool::UpdateCursorAndStatus()
{
	ERROR3IF_PF(pRefStart==NULL,("pRefStart is NULL"));
	ERROR3IF_PF(pRefEnd  ==NULL,("pRefEnd   is NULL"));

	// Default to standard pointer and status line text
	Cursor* pcNewCursor = pcNormalCursor;
	StatusID = _R(IDS_BLENDSTATUS_FINDSTART);	// StatusID is a member var

	if (pRefStart->RemapRef > 0)
	{
		// The pointer is either over a blend blob, or user started a drag over a blend blob
		pcNewCursor = pcBlendableBlobCursor;
		StatusID = _R(IDS_BLENDSTATUS_REMAPSTART);

		if (pRefStart->RemapRef == pRefEnd->RemapRef && pRefStart->AStartNode != pRefEnd->AStartNode)
		{
			// Dragged to a corresponding blob in the other blend
			StatusID = _R(IDS_BLENDSTATUS_REMAPEND);
			pcNewCursor = pcBlendableRemapCursor;
		}
	}
	else if (pRefStart->pNode != NULL)
	{
		if (pRefEnd->pSpread == NULL)
		{
			if (pRefStart->Index >= 0)
			{
				// We are over a blob of a selected path
				pcNewCursor = pcBlendableBlobCursor;
				StatusID 	= _R(IDS_BLENDSTATUS_OVERBLOB);
			}
			else
			{
				// We are over a blendable node
				pcNewCursor = pcBlendableCursor;
				StatusID 	= _R(IDS_BLENDSTATUS_FINDEND);
			}
		}
		else if (pRefStart->pSpread == pRefEnd->pSpread)
		{
			// Dragging, and start and end are in the same spread
			if (pRefStart->pNode != pRefEnd->pNode && pRefEnd->pNode != NULL)
			{
				StatusID = _R(IDS_BLENDSTATUS_OVEREND);

				if (pRefEnd->Index >= 0 && pRefStart->Index >= 0)
					// We are over a blob of a selected path, for both start & end objects
					// so display the remap cursor
					pcNewCursor = pcBlendableRemapCursor;
				else
					// We are over a blendable node
					pcNewCursor = pcBlendableCursor;
			}
			else
				StatusID = _R(IDS_BLENDSTATUS_FINDEND);
		}
	}

	if (pcCurrentCursor != pcNewCursor)
	{
		// Current cursor has changed
		CursorStack::GSetTop(pcNewCursor, CurrentCursorID);
		pcCurrentCursor = pcNewCursor;
	}

	// Always update the status bar text
	DisplayStatusBarHelp(StatusID);
}

/********************************************************************************************

>	virtual BOOL BlendTool::GetStatusLineText(	String_256* ptext, 
												Spread* pSpread,
												DocCoord DocPos, 
												ClickModifiers ClickMods)
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pStr		- ptr to string to place text in
				pSpread		- ptr to the spread in question
				DocPos    	- position of mouse in doc (in spread coords)
				ClickMods 	- mouse click modifiers
	Outputs:	*pStr - text for status line
	Returns:	TRUE if outputting valid text
	Purpose:	generate up-to-date text for the status line (called on idles)

********************************************************************************************/

BOOL BlendTool::GetStatusLineText(String_256* pStr,Spread* pSpread,DocCoord DocPos,ClickModifiers ClickMods)
{
	*pStr = String_256(StatusID);
	return TRUE;
}


/********************************************************************************************

>	void BlendTool::UpdateInfobar()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Asks the infobar to update itself
	SeeAlso:	-

********************************************************************************************/

void BlendTool::UpdateInfobar()
{
	pBlendInfoBarOp->UpdateInfoBarState();
}




/********************************************************************************************

>	BOOL BlendTool::IsPointOverPathBlob(DocCoord* pPointerPos,BlendToolRef* pRef)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		pPointerPos	= ptr to position of mouse in DocCoords
				pRef		= ptr to blend tool ref to update
	Outputs:	Updates pRef->pNode and pRef->pIndex if a path blob was found.
				Also, if blob found, *pPointerPos is snapped to coord of blob
	Returns:	TRUE if the coord is over a blob of a selected path
	Purpose:	Scans for selected paths.  If the coord is over a selected path's blob,
				it returns TRUE and pRef->pNode points to the path, and pRef->Index contains
				the element index of the blob.
				Also, *pPointerPos is snapped to the coord of the centre of the blob, if found.

********************************************************************************************/

BOOL BlendTool::IsPointOverPathBlob(DocCoord* pPointerPos,BlendToolRef* pRef)
{
	if (pRef == NULL) return FALSE;

	// Find the selected range of objects
	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pNode = pSelRange->FindFirst();

	BOOL BlobFound = FALSE;

	// Scan the selection for NodePath objects
	while (!BlobFound && pNode != NULL && pNode->FindParent() != NULL)
	{
		// Only look at selected NodePaths that are NOT selected inside another node.
		if (IS_A(pNode,NodePath) && IS_A(pNode->FindParent(),Layer))
		{
			NodePath* pNodePath = (NodePath*)pNode;

			if (pNodePath->GetUnionBlobBoundingRect().ContainsCoord(*pPointerPos))
			{
				// Get a pointer to the Path object within the NodePath
				Path* pPath = &(pNodePath->InkPath);

				// Is it over a blob? (Only check end points. Forget about control points)
				BlobFound = pPath->FindNearestPoint(*pPointerPos,POINTFLAG_ENDPOINTS,&(pRef->Index));

				// If a blob is found, store ptr to the node
				if (BlobFound)
				{
					pRef->pNode = pNodePath;
					pPath->SetPathPosition(pRef->Index);
					*pPointerPos = pPath->GetCoord();
				}
			}
	 	}

		// Now find the next selected node
		pNode = pSelRange->FindNext(pNode);
	}

	return BlobFound;
}

/********************************************************************************************

>	BOOL BlendTool::IsPointOverBlendBlob(DocCoord* pPointerPos,NodeRenderableInk** ppNodePath,INT32* pIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		pPointerPos	= ptr to position of mouse in DocCoords
				pRef		= ptr to blend tool ref to update
	Outputs:	Updates pRef->pNode pRef->pNodeBlend and pRef->Index if a blend blob was found.
				Also, if found, *pPointerPos = centre of blob
	Returns:	TRUE if the coord is over a blob of a selected blend
	Purpose:	Scans for selected blends.  If the coord is over a selected blend's blob,
				it returns TRUE and pRef->pNode points to the path, pRef->pNodeBlend points to the blend
				containing the path, and *pIndex contains the element index of the blob.
				Also, if found, *pPointerPos = centre of blob.

********************************************************************************************/

BOOL BlendTool::IsPointOverBlendBlob(DocCoord* pPointerPos,BlendToolRef* pRef)
{
	if (pRef == NULL) return FALSE;

	// Find the selected range of objects
	SelRange* pSelRange = GetApplication()->FindSelection();
	Node* pNode = pSelRange->FindFirst();

	BOOL BlobFound = FALSE;

	// Scan the selection for NodePath objects
	while (pNode != NULL && !BlobFound)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBlend))
		{
			NodeBlend* pNodeBlend = (NodeBlend*)pNode;

			if (pNodeBlend->GetUnionBlobBoundingRect().ContainsCoord(*pPointerPos))
			{
				BlobFound = pNodeBlend->IsPointOverBlob(pPointerPos,
														&(pRef->pBlendPath),
														&(pRef->Index),
														&(pRef->AStartNode),
														&(pRef->RemapRef));

				if (BlobFound)
				{
					pRef->pNode      = pNodeBlend;
					pRef->pNodeBlend = pNodeBlend;
//					*pPointerPos = pRef->pBlendPath->GetPathCoord(pRef->Index);
				}
			}
		}

		// Now find the next selected node
		pNode = pSelRange->FindNext(pNode);
	}

	return BlobFound;
}

/********************************************************************************************

>	void BlendTool::CheckNodeRemapping(BlendToolRef* pRefStart, BlendToolRef* pRefEnd)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		pRefStart = ptr to start ref
				pRefEnd   = ptr to end ref
	Outputs:	Potentailly updates pRefStart and pRefEnd, so that if they represent a remapping of nodes,
				they have the same remap reference
	Returns:	-
	Purpose:	This checks to see if the two references actually represent a node remapping within a blend.
				In order to cope with multi-stage blends, both references have to be looked at the same time.

				This is not the neatest way of doing it, but it's the quickest and safest method, given that
				it has to work for the gamma release it a few days time.

********************************************************************************************/

void BlendTool::CheckNodeRemapping(BlendToolRef* pRefStart, BlendToolRef* pRefEnd)
{
	ERROR3IF(pRefStart == NULL,"pRefStart == NULL");
	ERROR3IF(pRefEnd   == NULL,"pRefEnd == NULL");
	if (pRefStart == NULL || pRefEnd == NULL)
		return;

	NodeBlend* pNodeBlend = pRefStart->pNodeBlend;

	if (pNodeBlend == NULL || pNodeBlend != pRefEnd->pNodeBlend)
		return;

	Node* pNode = pNodeBlend->FindFirstChild();
	while (pNode != NULL)
	{
		if (IS_A(pNode,NodeBlender))
		{
			NodeBlender* pNodeBlender = (NodeBlender*)pNode;

			BOOL StartFound = pNodeBlender->IsPointOverBlob(&(pRefStart->PointerPos),
															&(pRefStart->pBlendPath),
															&(pRefStart->Index),
															&(pRefStart->AStartNode));

			BOOL EndFound   = pNodeBlender->IsPointOverBlob(&(pRefEnd  ->PointerPos),
															&(pRefEnd  ->pBlendPath),
															&(pRefEnd  ->Index),
															&(pRefEnd  ->AStartNode));

			if (StartFound && EndFound && (pRefStart->AStartNode != pRefEnd->AStartNode))
			{
				pRefStart->RemapRef = pNodeBlender->GetTag();
				pRefEnd  ->RemapRef = pNodeBlender->GetTag();
				return;
			}
		}
		pNode = pNode->FindNext();
	}
}

/********************************************************************************************
>	NodeRenderableInk* BlendTool::FindObject(Spread* pSpread, DocCoord PointerPos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/94
	Inputs:		pSpread  = ptr to spread click occurred
				PointerPos = the DocCoord of the click
	Outputs:	-
	Returns:	-
	Purpose:	Looks for a (possibly grouped) node(s) in the given spread at the
				given mouse-click position.  Convenient shorthand for the hit-testing
				functions.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

NodeRenderableInk* BlendTool::FindObject(Spread* pSpread, DocCoord PointerPos)
{
	return NodeRenderableInk::FindCompoundAtPoint(pSpread,PointerPos,NULL);
}


/********************************************************************************************

>	void BlendTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pSpread  = ptr to a spread
				pDocRect = ptr to DocRect of spread to render in
	Returns:	-
	Purpose:	Handles the RenderToolBlobs method.
				Renders the tool's blobs into the current doc view.
	SeeAlso:	

********************************************************************************************/

void BlendTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)
{
	// Render into the current doc view
	DocView* pDocView = DocView::GetCurrent();
		
	if (pDocView != NULL && pSpread != NULL)
	{
		SelRange* pSel = GetApplication()->FindSelection();
		if (pSel == NULL) return;
	
		List BlendList;
		BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
		if (ok)
		{	
			// Get a render region on the spread.  We need to render EORd stuff on top of the current view
			RenderRegion* pRender = pDocView->RenderOnTop(pDocRect,pSpread,UnclippedEOR);
		
			// two possibilities here: either we have a selected blend in which case we can 
			// collect it in the list that we compile, or we have a selected inside object which is
			// a member of a blend. 
			while (pRender != NULL)
			{	
				NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		
				while (pListItem != NULL)
				{		
					NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
					pNodeBlend->RenderBlendBlobs(pRender);
					pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
				}
			
				Node *pNode = pSel->FindFirst();
				while (pNode != NULL)
				{
					Node* pParent = pNode->FindParentOfSelected();

					if ((pParent != NULL) && (IS_A(pParent, NodeBlend)))
					{
						RenderSelectInsideBlobs(pRender, (NodeRenderableInk*)pNode);
					}
					// get next in selection
					pNode = pSel->FindNext(pNode);
				}
					// get the next render region
				pRender = pDocView->GetNextOnTop(pDocRect);
			}
		}
		BlendList.DeleteAll();
	}
}



/********************************************************************************************

>	BOOL BlendTool::RenderSelectInsideBlobs(RenderRegion* pRender, NodeRenderableInk* pNode)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Inputs:		pRender - region to render into
				pNode - node to render the blobs on
	Outputs:	-
	Returns:	TRUE if all went well, otherwise FALSE
	Purpose:	To render blobs on selected-inside nodes whilst in the blend tool.
				The exact blob depends on whether or not the node is a text story.
	SeeAlso:	Blendtool::RenderToolBlobs()

********************************************************************************************/

BOOL BlendTool::RenderSelectInsideBlobs(RenderRegion* pRender, NodeRenderableInk* pNode)
{
	// some checks
	if (pRender == NULL)
	{
		ERROR3("No render region");
		return FALSE;
	}

	if (pNode == NULL)
	{
		ERROR3("node is NULL");
		return FALSE;
	}


	// if the node is a text node we want a red blob, otherwise a black cross
	if (pNode->IsABaseTextClass())
	{
		pRender->SetLineColour(COLOUR_BEZIERBLOB);
		pRender->SetFillColour(COLOUR_BEZIERBLOB);
	}
	else
	{
		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
	}
						
	// Draw a blob at the centre point
	DocRect BlobSize;
	BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr != NULL)
	{
		DocRect BoundingRect = ((NodeRenderableBounded*)pNode)->GetBoundingRect();
		DocCoord Point = BoundingRect.Centre();
		pBlobMgr->GetBlobRect(Point, &BlobSize);
		
		if (pNode->IsABaseTextClass())
		{
			pRender->DrawBlob(Point, BT_UNSELECTED);
		}
		else
		{
			pRender->DrawLine(DocCoord(BlobSize.hi.x, BlobSize.hi.y), DocCoord(BlobSize.lo.x, BlobSize.lo.y));
			pRender->DrawLine(DocCoord(BlobSize.lo.x, BlobSize.hi.y), DocCoord(BlobSize.hi.x, BlobSize.lo.y));
			pRender->DrawPixel(DocCoord(BlobSize.hi.x, BlobSize.lo.y));
			pRender->DrawPixel(DocCoord(BlobSize.lo.x, BlobSize.lo.y));
		}
		return TRUE;
	}

	ERROR3("Couldn't get BlobManager");
	return FALSE;
}




/********************************************************************************************

>	static void BlendTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void BlendTool::DisplayStatusBarHelp(UINT32 StatusID)
{
	String_256 StatusMsg("");
	StatusMsg.Load(StatusID);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
}

/********************************************************************************************

>	static BOOL BlendTool::IsCurrent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the blend tool is the current tool
	Purpose:	Is the blend tool the current one? Call this to find out.
	SeeAlso:	-

********************************************************************************************/

BOOL BlendTool::IsCurrent()
{
	return (Tool::GetCurrentID() == TOOLID_BLEND);
}



/********************************************************************************************

>	BOOL BlendTool::SelectedBlendIsOnCurve()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if there is a blend in the selection that is on a curve
	Purpose:    To find out if any selected blends are on a curve
	SeeAlso:	Used in BlendTool::SelectChange to determine which blobs to render

********************************************************************************************/

BOOL BlendTool::SelectedBlendIsOnCurve()
{
	List BlendList;

	BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	if (ok)
	{
		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		while (pListItem != NULL)
		{
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
//			Node* pNode = NULL;
			if (pNodeBlend->IsOnACurve())
			{
				BlendList.DeleteAll();
				return TRUE;
			}

			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
		}
	}
	BlendList.DeleteAll();
	return FALSE;
}



//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------

/********************************************************************************************

>	MsgResult BlendInfoBarOp::Message(Msg* Message) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94  changed 13/9/99 Diccon Yamanaka
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Blend info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult BlendInfoBarOp::Message(Msg* Message) 
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
//		BOOL SetProfile = FALSE;   // these must be declared here even though they are only used in WM_CREATE
//		INT32 ProfileIndex = 1;
		switch (Msg->DlgMsg)
		{	
			

			case DIM_CANCEL:
				// Check if the message is a CANCEL
				Close(); // Close the dialog 
				break;

			case DIM_CREATE:
			{

PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX
				// code added Diccon Yamanaka 9/99
				/// init profile gadgets
				{
					m_BiasGainGadgetPosition.LinkControlButton ( this, _R(IDC_BLENDOBJECTBIASGAIN), _R(IDBBL_BLENDOBJECTBIASGAIN), _R(IDS_BLENDOBJECTBIASGAIN)	);
					m_BiasGainGadgetAttribute.LinkControlButton ( this, _R(IDC_BLENDATTRBIASGAIN),	_R(IDBBL_BLENDATTRBIASGAIN),   _R(IDS_BLENDATTRBIASGAIN)	);
					m_BiasGainGadgetAttribute.ToggleFillProfile ();
				}
#endif
				SetGadgetHelp(_R(IDC_BTN_BLENDSTEPS), _R(IDBBL_BLENDSTEPSEDIT), _R(IDS_BLENDSTEPSEDIT));

				// these two buttons need different bitmaps for their selected and 
				// unselected states, set them here.
				SetBitmapButtonIndexes(_R(IDC_ADDREMOVEBLENDPATH), 79, 80);
				SetBitmapButtonIndexes(_R(IDC_TANGENTIAL), 81, 84);
				
				// if we were editing a blend on a path then make the blend selected
				CheckSelectionAndSet();  

				// solve the sticky button problem
				if (GetBlendOnCurve() != NULL)
					m_BlendedOnCurve = TRUE;
				else
					m_BlendedOnCurve = FALSE;
				
				// ensure we have the correct edit button depressed 
				NodeBlend* pNodeBlend = GetNodeBlend();
				if (pNodeBlend != NULL)
				{
					if (pNodeBlend->GetEditState() == EDIT_STEPS)
					{
						m_EditBlendSteps = TRUE;
						SetLongGadgetValue(_R(IDC_BTN_BLENDDISTANCE), FALSE);
					}
					else
					{
						m_EditBlendSteps = FALSE;
						SetLongGadgetValue(_R(IDC_BTN_BLENDDISTANCE), FALSE);
					}
				}

				UpdateInfoBarState();
			}
			break;

			case DIM_LFT_BN_CLICKED:
			{
				if (FALSE) {}
				else if (Msg->GadgetID == _R(IDC_ADDREMOVEBLENDPATH))
				{
					// DY 13/9/99 if button is down then pop it up 
					BOOL OnPath = (GetBlendOnCurve() != NULL);
					m_BlendedOnCurve = OnPath;
					ChangeBitmapButtonState(_R(IDC_ADDREMOVEBLENDPATH), &m_BlendedOnCurve);

					OpDescriptor* pOpDesc = NULL;

					if (!m_BlendedOnCurve)
						pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_DETACHBLENDPATH);
					else
						pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_ADDBLENDPATH);
				
					if (pOpDesc != NULL) 
						pOpDesc->Invoke();			
				}
				else if (Msg->GadgetID ==_R(IDC_REMOVEBLEND))
				{
					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_REMOVEBLEND);
					if (pOpDesc != NULL) 
						pOpDesc->Invoke();
				
					m_BlendedOnCurve = FALSE;

				}
				else if (Msg->GadgetID == _R(IDC_BLENDOBJECTBIASGAIN))
				{
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX
					HandleProfileButtonClick (m_BiasGainGadgetPosition, _R(IDC_BLENDOBJECTBIASGAIN));
#endif
				}
				else if (Msg->GadgetID == _R(IDC_BLENDATTRBIASGAIN))
				{
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX
					HandleProfileButtonClick (m_BiasGainGadgetAttribute, _R(IDC_BLENDATTRBIASGAIN));
#endif
				}
				else if (Msg->GadgetID == _R(IDC_BTN_ONETOONE))
				{
					// DY 13/9/99 if button is down then pop it up 
					ChangeBitmapButtonState(_R(IDC_BTN_ONETOONE), &m_OneToOne);	

					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLEND);
					if (pOpDesc != NULL)
					{
						ChangeBlendOpParam Param;
						Param.ChangeType  = CHANGEBLEND_ONETOONE;
						Param.NewOneToOne = GetBoolGadgetSelected(_R(IDC_BTN_ONETOONE));
						pOpDesc->Invoke(&Param);
					}
				}
				else if (Msg->GadgetID == _R(IDC_BTN_BLENDANTIALIAS))
				{
					// DY 13/9/99 if button is down then pop it up 
					ChangeBitmapButtonState(_R(IDC_BTN_BLENDANTIALIAS), &m_BlendAntiAlias);
						
					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLEND);
					if (pOpDesc != NULL)
					{
						ChangeBlendOpParam Param;
						Param.ChangeType   = CHANGEBLEND_ANTIALIAS;
						Param.NewAntiAlias = GetBoolGadgetSelected(_R(IDC_BTN_BLENDANTIALIAS));
						pOpDesc->Invoke(&Param);
					}
				}
				else if (Msg->GadgetID == _R(IDC_TANGENTIAL))
				{
					// DY 13/9/99 if button is down then pop it up 
					ChangeBitmapButtonState(_R(IDC_TANGENTIAL), &m_Tangential);				
							
					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLEND);
					if (pOpDesc != NULL)
					{
						ChangeBlendOpParam Param;
						Param.ChangeType  = CHANGEBLEND_TANGENTIAL;
						Param.NewTangential = GetBoolGadgetSelected(_R(IDC_TANGENTIAL));
						pOpDesc->Invoke(&Param);
					}
				}
				// DY 13/9/99 buttons to select whether blend steps or blend distance
				// is to be edited in the edit box
				else if (Msg->GadgetID == _R(IDC_BTN_BLENDDISTANCE))
				{
					// set the steps control	
					SetLongGadgetValue(_R(IDC_BTN_BLENDSTEPS), FALSE);
					m_EditBlendSteps = FALSE;
					SetBlendEditState(EDIT_DISTANCE);  // let the blend know we are editing distance
					UpdateInfoBarState();      // tell the infobar something has changed
					
				}
				else if (Msg->GadgetID == _R(IDC_BTN_BLENDSTEPS))
				{	
					// set the distance control
					SetLongGadgetValue(_R(IDC_BTN_BLENDDISTANCE), FALSE);
					m_EditBlendSteps = TRUE;
					SetBlendEditState(EDIT_STEPS);   // let the blend know we are editing state
					UpdateInfoBarState();                    // update toolbar
				}

			}
			break;

			case DIM_SELECTION_CHANGED:
			{
				if (FALSE) {}
				else if (Msg->GadgetID == _R(IDC_BLENDSTEPS))
				{
					BOOL Valid = FALSE;
					INT32 NumSteps = 0;
					
					// either change the number of blend steps or
					// the distance between steps
					if (m_EditBlendSteps)
					{
						Valid = GetNumSteps(1, 999, &NumSteps);
						if (Valid)
						{
							OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLENDSTEPS);
							if (pOpDesc != NULL)
							{
								OpParam Param(NumSteps,0);
								pOpDesc->Invoke(&Param);
							}
						}
						UpdateInfoBarState();
					}
					else
					{
						double Distance = 0.0;
						Valid = GetDistanceEntered( &Distance);
						if (Valid)
						{
							
							Valid = IsStepDistanceValid(1, 999, Distance);
							if (Valid)
							{
								OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLENDDISTANCE);
								if (pOpDesc != NULL)
								{
									OpParam Param((INT32)Distance, 0);
									pOpDesc->Invoke(&Param);
								}
							}
						}
						UpdateInfoBarState();
					}
					
				}
				else if (Msg->GadgetID == _R(IDC_EFFECT))
				{
					WORD Index;
					GetValueIndex(_R(IDC_EFFECT),&Index); 
					ColourBlendType ColBlendType = (ColourBlendType)Index;

					if (ColBlendType <= COLOURBLEND_ALTRAINBOW)
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLEND);
						if (pOpDesc != NULL)
						{
							ChangeBlendOpParam Param;
							Param.ChangeType      = CHANGEBLEND_COLOURBLENDTYPE;
							Param.NewColBlendType = ColBlendType;
							pOpDesc->Invoke(&Param);
						}
					}
				}
			}
			break;

			default:
				break;
		}


		if (( Msg->GadgetID == _R(IDC_BLENDOBJECTBIASGAIN)) || (Msg->GadgetID == _R(IDC_BLENDOBJECTBIASGAIN)))
		{  
			ProfileSelectionChange( Msg, Msg->GadgetID );
		}

	}
	// do we have a selection change message? 
	if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		UpdateInfoBarState();
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX
		HandleProfileSelChangingMsg (m_BiasGainGadgetPosition, _R(IDC_BLENDOBJECTBIASGAIN));
		HandleProfileSelChangingMsg (m_BiasGainGadgetAttribute, _R(IDC_BLENDATTRBIASGAIN));
#endif
	}

	// Pass the message on to the immediate base class
	return (InformationBarOp::Message(Message));
}    
		
/********************************************************************************************

>	void BlendInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> Snook
	Created:	17/9/99
	Purpose:	See InformationBarOp::ChangeProfile () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfile ()

*********************************************************************************************/

void BlendInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
				
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGEBLEND);
	if (pOpDesc != NULL)
	{
		BOOL FireOp = TRUE;

		// we only want to generate one bit of undo information - so decided whether
		// we have to fire the above op, or whether we just 'pump' the values into
		// our nodes (thereby nolonger generating infinite undo information) ....

		Operation* pLastOp = NULL;

		if (Profile->GetGeneratesInfiniteUndo ())	// only do if they didn't select a preset profile
		{
			pLastOp = Document::GetSelected()->GetOpHistory().FindLastOp();
		}
		
		ChangeBlendOpParam Param;
		
		if (GadgetID == _R(IDC_BLENDOBJECTBIASGAIN))
		{
			if (pLastOp)
			{
				if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeBlend))
				{
					OpChangeBlend* pLastBlendOp = (OpChangeBlend*) pLastOp;
					
					if (pLastBlendOp->GetChangeType () == CHANGEBLEND_OBJECTPROFILE)
					{
						if (Profile->GetGeneratesInfiniteUndo ())
						{
							FireOp = FALSE;
						}
					}
				}
			}
			
			if (FireOp == TRUE)
			{
				Param.ChangeType  = CHANGEBLEND_OBJECTPROFILE;
				Param.NewObjectProfile = *Profile;
			}
			else
			{	
				// we don't need/want any undo information - so just change the value ....

				ChangeBlendAction Action;
				Action.ChangeObjectProfileWithNoUndo (*Profile);
			}
		}
		else if (GadgetID == _R(IDC_BLENDATTRBIASGAIN))
		{
			if (pLastOp)
			{
				if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeBlend))
				{
					OpChangeBlend* pLastBlendOp = (OpChangeBlend*) pLastOp;
					
					if (pLastBlendOp->GetChangeType () == CHANGEBLEND_ATTRPROFILE)
					{
						FireOp = FALSE;
					}
				}
			}
			
			if (FireOp == TRUE)
			{
				Param.ChangeType  = CHANGEBLEND_ATTRPROFILE;
				Param.NewAttrProfile = *Profile;
			}
			else
			{	
				// we don't need/want any undo information - so just change the value ....
			
				ChangeBlendAction Action;
				Action.ChangeAttributeProfileWithNoUndo (*Profile);
			}
		}
		else
			return;  // shome mishtake shurely, lets go
		
		if (FireOp == TRUE)
		{
			pOpDesc->Invoke(&Param);
		}
	}				
}

/********************************************************************************************

>	void BlendInfoBarOp::ChangeProfileOnIdle(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/2000
	Purpose:	See InformationBarOp::ChangeProfile () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfile ()

*********************************************************************************************/

void BlendInfoBarOp::ChangeProfileOnIdle(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
	if (GadgetID == _R(IDC_BLENDOBJECTBIASGAIN))
	{	
		// we don't need/want any undo information - so just change the value ....

		ChangeBlendAction Action;
		Action.ChangeObjectProfileWithNoUndo (*Profile, TRUE);
	}

	// NOTE: not doing anything for the attribute profiles here, since it does not appear as
	// though we have to ....
}

		
/********************************************************************************************

>	BOOL BlendInfoBarOp::GetNumSteps( UINT32 MinValue, UINT32 MaxValue, INT32* LNumSteps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/9/99
	Inputs:		pointer to INT32 to store the result, minimum and maximum valid values
	Outputs:    returns TRUE if value can be retrieved and is within the valid range.
				This value will be assigned to LNumSteps, if a number cannot be retreived
				or it is invalid then FALSE.
	Purpose:	to retrieve an INT32 value for the number of steps and 
				check its validity. Strips out any chars following the numerical value

*********************************************************************************************/

BOOL BlendInfoBarOp::GetNumSteps( UINT32 MinValue, UINT32 MaxValue, INT32 *NumSteps)
{
	BOOL Valid = TRUE;  // the return value
	// first get the string from the gadget
	String_256 StrNumSteps = GetStringGadgetValue(_R(IDC_BLENDSTEPS), &Valid);
	
	
	if (Valid)
	{	
		INT32 Position = 0;
		double DNumSteps = 0.0;

		// get the numerical value from the string
		Valid = Convert::ReadNumber(StrNumSteps, &Position, &DNumSteps);

		if (Valid)
		{
			// check to see if it is in bounds, if not send a warning
			if (DNumSteps < MinValue || DNumSteps > MaxValue)
			{
				InformWarning(_R(IDS_BLENDSTEPS_INVALID));
				Valid = FALSE;
			}
			else
			{
				*NumSteps = (INT32)DNumSteps;  // success
				return Valid;
			}
		}
	}

	// if we reach here we have failed
	NumSteps = 0;
	return Valid;						
		
}
		
/********************************************************************************************

>	BOOL BlendInfoBarOp::GetDistanceEntered( INT32* LNumSteps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Inputs:		-
	Outputs:	the distance entered in millipoints 
	returns:    TRUE if a distance value can be retrieved 
				
	Purpose:	retrieve what the user entered in the edit field

*********************************************************************************************/

BOOL BlendInfoBarOp::GetDistanceEntered(double* Distance)

{

	SelRange* pSel = GetApplication()->FindSelection();
	BOOL Valid = FALSE;
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();  // only used to pass to the gadget function
		
		double UnitValue = 0.0;
		UnitType TheUnit;
		Valid = GetDoubleAndUnitGadgetValue(Distance, &UnitValue, &TheUnit, _R(IDC_BLENDSTEPS), pNode);
		if (!Valid)
			InformWarning(_R(IDS_INVALIDDISTANCE));
	}
	return Valid;
}

		
/********************************************************************************************

>	BOOL BlendInfoBarOp::GetNumStepsFromDistance( UINT32 MinValue, UINT32 MaxValue, INT32* LNumSteps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	Inputs:		pointer to INT32 to store the result, minimum and maximum valid values
	Outputs:    returns TRUE if value can be retrieved and is within the valid range.
				This value will be assigned to NumSteps, if a number cannot be retreived
				or it is invalid then FALSE.
	Purpose:	When the Distance between steps in a blend is edited we need to recalculate
				the number of steps.  This function retrieves the distance value from the edit
				box, converts to Mps and works out the number of steps needed to get closest
				to that distance value.
				If the specified distance leads to an invalid number of steps then the user
				is warned.

*********************************************************************************************/

BOOL BlendInfoBarOp::GetNumStepsFromDistance(UINT32 MinValue, UINT32 MaxValue, INT32* NumSteps)
{
	
	SelRange* pSel = GetApplication()->FindSelection();
	BOOL Valid = FALSE;
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();  // only used to pass to the gadget function
		double MPValue = 0.0;
		double UnitValue = 0.0;
		UnitType TheUnit;
		Valid = GetDoubleAndUnitGadgetValue(&MPValue, &UnitValue, &TheUnit, _R(IDC_BLENDSTEPS), pNode);
	
		if (Valid)
		{
		
				if (pNode->IS_KIND_OF(NodeBlend))
				{
					double Distance = 0.0;
					Valid = ((NodeBlend*)pNode)->GetBlendDistance(FALSE, &Distance);
					if (Valid)
					{
						UINT32 TempNumSteps = (UINT32)(Distance/MPValue);
						if (TempNumSteps < MinValue)
						{
							InformWarning(_R(IDS_BLENDDISTANCE_TOOBIG));
							Valid = FALSE;
						}
						else if (TempNumSteps > MaxValue)
						{
							InformWarning(_R(IDS_BLENDDISTANCE_TOOSMALL));
							Valid = FALSE;
						}
						else
						{
							*NumSteps = TempNumSteps;
						}
					}
					else
						InformWarning(_R(IDS_BLENDDISTANCE_INVALID));
				}
				else
					InformWarning(_R(IDS_BLENDDISTANCE_INVALID));
			}
		}
		return Valid;
}

/********************************************************************************************

>	void BlendInfoBarOp::ApplyOneToOne(BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		State = state of one to one flag (TRUE for set, FALSE for clear)
	Purpose:	This scans the selected blends setting the one-to-one flag state to "State"

*********************************************************************************************/
/*
void BlendInfoBarOp::ApplyOneToOne(BOOL State)
{
	DocView* pDocView = DocView::GetCurrent();			// Get ptr to current doc view
	Spread*  pSpread  = Document::GetSelectedSpread();	// Get ptr to selected spread

	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				NodeBlend* pNodeBlend = ((NodeBlend*)pNode);

				pNodeBlend->SetOneToOne(State);

				if (pDocView != NULL && pSpread != NULL)
					pDocView->ForceRedraw(pSpread,pNodeBlend->GetBlobBoundingRect());
			}

			pNode = pSel->FindNext(pNode);
		}
	}
}
*/
/********************************************************************************************

>	void BlendInfoBarOp::ApplyAntialias(BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		State = state of anitalias flag (TRUE for set, FALSE for clear)
	Purpose:	This scans the selected blends setting the antialias flag state to "State"

*********************************************************************************************/
/*
void BlendInfoBarOp::ApplyAntialias(BOOL State)
{
	DocView* pDocView = DocView::GetCurrent();			// Get ptr to current doc view
	Spread*  pSpread  = Document::GetSelectedSpread();	// Get ptr to selected spread

	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				NodeBlend* pNodeBlend = ((NodeBlend*)pNode);

				pNodeBlend->SetNotAntialiased(!State);

				if (pDocView != NULL && pSpread != NULL)
					pDocView->ForceRedraw(pSpread,pNodeBlend->GetBlobBoundingRect());
			}

			pNode = pSel->FindNext(pNode);
		}
	}
}
*/


/********************************************************************************************

>	void BlendInfoBarOp::UpdateState()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94, changed 13/9/99 Diccon Yamanaka
	Purpose:	Overrides the empty UpdateState function provided by InformationBarOp
				making a call to the function in DialogBarOp.

*********************************************************************************************/

void BlendInfoBarOp::UpdateState()
{
	// NOTE:  all of the functionality of updatestate has been moved to 
	// UpdateInfoBarState.  This is because we were getting flickering whenver
	// you used a scroll bar.
}

/********************************************************************************************

>	void BlendInfoBarOp::UpdateInfoBarState()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94, changed 13/9/99 Diccon Yamanaka
	Purpose:	Overrides the empty UpdateState function provided by InformationBarOp
				making a call to the function in DialogBarOp.

*********************************************************************************************/

void BlendInfoBarOp::UpdateInfoBarState()
{
	if(this== NULL)
		return;
 	if(!this->HasWindow())
		return ;

	if (!BlendTool::IsCurrent()) return;

	DialogBarOp::UpdateState();		// This updates all controls that have OpDescs attached

	BOOL OneToOne = FALSE;
	BOOL Antialias = NodeBlend::GetDefaultAntialias();
	INT32 NumSteps = 0;
	UINT32 NumSelBlends = 0;
	ColourBlendType ColBlendType = COLOURBLEND_FADE;

	BOOL OnlyBlendsSelected			= TRUE;
	BOOL AllSelectedBlendsAreOnCurve= TRUE;
//	BOOL AtLeastOneBlendIsOnCurve	= FALSE;
	BOOL Tangential					= FALSE;
	BOOL EditSteps					= TRUE;
	UINT32 NumBlendsOnCurve			= 0;
	BOOL NonLinearProfile			= FALSE;
	BOOL MultiStageBlendOnCurve     = FALSE;
	List BlendList;

	BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	if (!ok)
	{
		OnlyBlendsSelected = FALSE;
		AllSelectedBlendsAreOnCurve = FALSE;
	}
	else
	{
		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		
		while (pListItem != NULL)
		{
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
			// exclude blends that are part of a group as we don't want to put 
			// them on a path
			if (!pNodeBlend->IsChildOfGroup())
			{
				NumSelBlends++;	

				if (pNodeBlend->GetEditState() == EDIT_DISTANCE)
					EditSteps = FALSE;
				
				UINT32 			ThisNumSteps 	= pNodeBlend->GetNumBlendSteps();
				ColourBlendType ThisColBlendType= pNodeBlend->GetColourBlendType();

				if (NumSelBlends == 1)
				{
					NumSteps 	 = ThisNumSteps;
					ColBlendType = ThisColBlendType;
				}
				else
				{
					if (NumSteps >= 0 && NumSteps != INT32(ThisNumSteps))
						NumSteps = -1;
					if (ColBlendType != ThisColBlendType)
							ColBlendType = COLOURBLEND_NONE;
				}

				if (!OneToOne && pNodeBlend->IsOneToOne())
					OneToOne = TRUE;

				if (Antialias && pNodeBlend->IsNotAntialiased())
					Antialias = FALSE;

				if (!Tangential && pNodeBlend->IsTangential())
					Tangential = TRUE;

				if (pNodeBlend->GetNodeBlendPath(0) != NULL)
					NumBlendsOnCurve++;
				else
					AllSelectedBlendsAreOnCurve = FALSE;	

				if (pNodeBlend->NonLinearObjectProfile())
					NonLinearProfile = TRUE;
				
				if (pNodeBlend->GetNumNodeBlendPaths() > 1)
					MultiStageBlendOnCurve = TRUE;
			}

			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
			
		}
	}
	BlendList.DeleteAll();

	OnlyBlendsSelected = !NonBlendsSelected();
	
	EnableBlendSelectedGadgets(NumSelBlends > 0);

	// if we have a non-linear profile it makes no sense to enable distance editing so 
	// send a message to switch it off
	// also as we cannot currently deal with distance editing for blends with multiple 
	// curves then do the same thing
	// Likewise if we are not on a curve, and our operations are not allowed to move the end
	// objects then it makes no sense to allow this
	if (NonLinearProfile == TRUE || MultiStageBlendOnCurve == TRUE || NumBlendsOnCurve == 0)
	{
		EnableGadget(_R(IDC_BTN_BLENDDISTANCE), FALSE);
		// only send the message once
		if (m_EditBlendSteps == FALSE)
			BROADCAST_TO_CLASS( DialogMsg(NULL, DIM_LFT_BN_CLICKED, _R(IDC_BTN_BLENDSTEPS), 0, 0 ), DialogOp);
	}
	else
		EnableGadget(_R(IDC_BTN_BLENDDISTANCE), TRUE);

	EnableGadget(_R(IDC_BTN_BLENDSTEPS),	TRUE);
	EnableGadget(_R(IDC_ADDREMOVEBLENDPATH), !(NumSelBlends == 0 || (OnlyBlendsSelected && NumBlendsOnCurve==0)));
	EnableGadget(_R(IDC_TANGENTIAL),	(NumSelBlends > 0) && (NumBlendsOnCurve > 0));

	SetBoolGadgetSelected(_R(IDC_BTN_ONETOONE),OneToOne);
	SetBoolGadgetSelected(_R(IDC_BTN_BLENDANTIALIAS),Antialias);
	SetBoolGadgetSelected(_R(IDC_TANGENTIAL), Tangential); 

	LoadStringsIntoEffectCombo();

	if (m_EditBlendSteps != EditSteps)
	{
		if (EditSteps)
		{
			SetLongGadgetValue(_R(IDC_BTN_BLENDDISTANCE), FALSE);	
		}
		else
		{
			SetLongGadgetValue(_R(IDC_BTN_BLENDSTEPS), FALSE);
		}
	}

	if (NumSelBlends != GetApplication()->FindSelection()->Count())
	{
		EnableGadget (_R(IDC_BLENDOBJECTBIASGAIN), FALSE);
		EnableGadget (_R(IDC_BLENDATTRBIASGAIN), FALSE);
	}

	if (NumSelBlends == 0)
	{
		// make the effect combo and the blendsteps text blank
		String_64 Str;
		Str = _T("");
		SetStringGadgetValue(_R(IDC_BLENDSTEPS),&Str);
		SetStringGadgetValue(_R(IDC_EFFECT),&Str, FALSE, -1);
	}
	else
	{
		ShowEffectComboString(ColBlendType);
		// determine whether to display number of steps or distance
		// in the edit box and display appropriate value
		if (NumSelBlends == 1)
		{	
			if (m_EditBlendSteps)
				SetBlendStepsEditText((INT32)NumSteps);
			else
				SetBlendDistanceEditText((INT32)NumSteps);
		}

		else if (NumSelBlends > 1)
		{
			if (m_EditBlendSteps)
			{
				if (AllBlendsHaveSameNumSteps())
					SetBlendStepsEditText((INT32)NumSteps);
				else
					SetStringGadgetValue(_R(IDC_BLENDSTEPS),_R(IDS_MANY));
			}
			else 
			{ 
				if (AllBlendsHaveSameDistance())
					SetBlendDistanceEditText((INT32)NumSteps);
				else
					SetStringGadgetValue(_R(IDC_BLENDSTEPS),_R(IDS_MANY));
			}
		}
	}


	// For some reason the add blend to curve button doesn't like
	// to stay depressed, so force it.
	if (NumSelBlends > 0)
	{
		if (NumBlendsOnCurve > 0)
		{
			SetLongGadgetValue(_R(IDC_ADDREMOVEBLENDPATH), TRUE);
			if (Tangential)
			{
				SetLongGadgetValue(_R(IDC_TANGENTIAL), TRUE);
			}
		}
	
	}

	// depress either the edit steps or edit distance button and
	// change the bubble strings displayed in the edit box according
	// to which button is currently selected
	if (m_EditBlendSteps == TRUE)
	{
		SetLongGadgetValue(_R(IDC_BTN_BLENDSTEPS), TRUE);
		SetGadgetHelp(_R(IDC_BLENDSTEPS), _R(IDBBL_BLENDSTEPSEDITVALUE), _R(IDS_BLENDSTEPSEDITVALUE));
	}
	else
	{
		SetLongGadgetValue(_R(IDC_BTN_BLENDDISTANCE), TRUE);
		SetGadgetHelp(_R(IDC_BLENDSTEPS), _R(IDBBL_BLENDDISTANCEEDITVALUE), _R(IDS_BLENDDISTANCEEDITVALUE));
	}
}

/********************************************************************************************

>	void BlendInfoBarOp::ShowEffectComboString(ColourBlendType Type)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Inputs:		Type - determines which to show
	Outputs:	-
	Returns:	-
	Purpose:	Determines which string to show in the combo
*********************************************************************************************/

void BlendInfoBarOp::ShowEffectComboString(ColourBlendType Type)
{
	String_64 Str;
	switch (Type)
	{
		case COLOURBLEND_FADE:			Str.Load(_R(IDS_FILLTOOL_FADE));		break;
		case COLOURBLEND_RAINBOW:		Str.Load(_R(IDS_FILLTOOL_RAINBOW));		break;
		case COLOURBLEND_ALTRAINBOW:	Str.Load(_R(IDS_FILLTOOL_ALTRAINBOW));	break;
		case COLOURBLEND_NONE:			Str.Load(_R(IDS_MANY));					break;

		default:ERROR3("Unknown colour blend type"); Str.Load(_R(IDS_FILLTOOL_FADE)); break;
	}
	SetStringGadgetValue(_R(IDC_EFFECT),&Str, FALSE, -1);
}

/********************************************************************************************

>	void BlendInfoBarOp::LoadStringsIntoEffectCombo()
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Loads the correct strings into the combo box
*********************************************************************************************/

void BlendInfoBarOp::LoadStringsIntoEffectCombo()
{
	String_64 Str;
	DeleteAllValues(_R(IDC_EFFECT));
	Str.Load(_R(IDS_FILLTOOL_FADE));
	SetStringGadgetValue(_R(IDC_EFFECT),&Str,FALSE, FEMENU_FADE);
	Str.Load(_R(IDS_FILLTOOL_RAINBOW));
	SetStringGadgetValue(_R(IDC_EFFECT),&Str,FALSE, FEMENU_RAINBOW);
	Str.Load(_R(IDS_FILLTOOL_ALTRAINBOW));
	SetStringGadgetValue(_R(IDC_EFFECT),&Str,TRUE, FEMENU_ALTRAINBOW);

	SetComboListLength(_R(IDC_EFFECT));
}

/********************************************************************************************

>	void BlendInfoBarOp::EnableBlendSelectedGadgets(BOOL Enable)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Inputs:		Enable, TRUE or FALSE
	Outputs:	-
	Returns:	-
	Purpose:	All of these controls require at least one blend to be in the current selection
				if they are to be enabled. 
*********************************************************************************************/

void BlendInfoBarOp::EnableBlendSelectedGadgets(BOOL Enable)
{
	EnableGadget(_R(IDC_BLENDOBJECTBIASGAIN), Enable);
	EnableGadget(_R(IDC_BLENDATTRBIASGAIN), Enable);
	EnableGadget(_R(IDC_BLENDSTEPS),	Enable);
	EnableGadget(_R(IDC_BTN_ONETOONE),		Enable);
	EnableGadget(_R(IDC_BTN_BLENDANTIALIAS),Enable);
	EnableGadget(_R(IDC_EFFECT),		Enable);
}

/********************************************************************************************

>	void BlendInfoBarOp::ChangeBitmapButtonState(CGadget GadgetID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Purpose:	 utility used by BlendInfoBarOp::Message, if a button is depressed 
				and clicked again then un-depress it  For some reason the bitmap button custom
				control does not do this by itself..

*********************************************************************************************/

void BlendInfoBarOp::ChangeBitmapButtonState(CGadgetID GadgetID, BOOL* CurrentState)
{
	/*BOOL test =*/  GetBoolGadgetSelected(GadgetID);
	if (*CurrentState == FALSE)
	{
		*CurrentState = TRUE;
	}
	else
	{
		*CurrentState = FALSE;
		SetLongGadgetValue(GadgetID, FALSE);
	}
	
}

/********************************************************************************************

>	void BlendInfoBarOp::SetBlendStepsEditText(INT32 NumSteps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/99
	inputs :    number of steps in the currently selected blend
	Purpose:	converts the number of steps into a string and concatenates it with 
				the string "steps" before writing it into the control
*********************************************************************************************/

void BlendInfoBarOp::SetBlendStepsEditText(INT32 NumSteps)
{
		String_256 StepString(_R(IDS_BLENDSTEPS_POSTFIX));	// = " Steps";
		String_256 StrNumSteps = _T("");
		Convert::LongToString(NumSteps, &StrNumSteps);
		StrNumSteps += StepString;

		/*BOOL ok =*/ SetStringGadgetValue(_R(IDC_BLENDSTEPS), &StrNumSteps);
		//  error code to go here
}

/********************************************************************************************

>	void BlendInfoBarOp::SetBlendDistanceEditText(INT32 NumSteps, UINT32 OnCurve)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/99
	inputs :    NumSteps - number of steps in the currently selected blend 
	Purpose:	works out the distance between each blend step in the selected blend
				concatenates with the current units used in the spread and writes it
				to the edit box
*********************************************************************************************/

void BlendInfoBarOp::SetBlendDistanceEditText(INT32 NumSteps)
{	
	double Distance = 0.0;
	
	BOOL Valid = GetMeanBlendDistance(&Distance);

	if (!Valid)
		return;

	double DistanceBetweenSteps = Distance/NumSteps;

	// get the current spread 
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel == NULL)
		return;
	Spread* pCurrentSpread = pSel->FindFirst()->FindParentSpread();

	if (pCurrentSpread != NULL)
	{
		// get the unit type
		DimScale* pDimScale = DimScale::GetPtrDimScale((Node*)pCurrentSpread);
		if (pDimScale == NULL)
			return;

		String_256 Str;
		pDimScale->ConvertToUnits(DistanceBetweenSteps, &Str, TRUE, 4);
		
		SetStringGadgetValue(_R(IDC_BLENDSTEPS), &Str);// set it
	}
	
	return;
}

/********************************************************************************************

>	BOOL BlendInfoBarOp::GetMeanBlendDistance(double* Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    
	outputs:    double value of length of blend, zero if fails
	returns:    TRUE if successful, FALSE otherwise 
	Purpose:	To find the mean length of the currently selected blend.  That is to say if
				this is a multistage blend then it returns the mean blend stage length.

*********************************************************************************************/

BOOL BlendInfoBarOp::GetMeanBlendDistance(double* Distance)
{
	BOOL Valid = FALSE; // the return value
	*Distance = 0.0;
	NodeBlend* pNodeBlend = GetNodeBlend();

	if (pNodeBlend == NULL)
		return FALSE;

	UINT32 NumBlenders = pNodeBlend->GetNumBlenders();
	if (NumBlenders == 0)
	{
		ERROR3("This blend has zero blenders");
		return FALSE;
	}
	double TempDistance = 0.0;

	Valid = pNodeBlend->GetBlendDistance(FALSE, &TempDistance);
	if (Valid)
	{
		TempDistance = TempDistance / NumBlenders;
		*Distance = TempDistance;
	}

	return Valid;
}

/********************************************************************************************

>	BOOL BlendInfoBarOp::AllBlendsHaveSameNumSteps()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    
	returns:    TRUE if all blends in selection have the same number of steps,
				FALSE otherwise
	Purpose:	find out if all selected blends have the same number of steps

*********************************************************************************************/

BOOL BlendInfoBarOp::AllBlendsHaveSameNumSteps()
{
	UINT32 NumSteps = 0;
	BOOL FoundFirst = FALSE;
	BOOL SameNumber = FALSE;
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))	
				if (!FoundFirst)
				{
					NumSteps = ((NodeBlend*)pNode)->GetNumBlendSteps();
					FoundFirst = TRUE;
					SameNumber = TRUE;
				}
				else
				{
					if (((NodeBlend*)pNode)->GetNumBlendSteps() != NumSteps)
						return FALSE;
				}
				pNode = pSel->FindNext(pNode);
		}
	}
	return SameNumber;
}

/********************************************************************************************

>	BOOL BlendInfoBarOp::AllBlendsHaveSameDistance()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    
	returns:    TRUE if all blends in selection have the same distance between steps
				FALSE otherwise
	Purpose:	find out if all selected blends have the same distance between steps

*********************************************************************************************/

BOOL BlendInfoBarOp::AllBlendsHaveSameDistance()
{
	double FirstDistance = 0.0;
	double FirstStepDistance = 0.0;
	
	BOOL FoundFirst = FALSE;
	BOOL SameNumber = FALSE;
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))	
				if (!FoundFirst)
				{
					if (!((NodeBlend*)pNode)->GetBlendDistance(FALSE, &FirstDistance))
						return FALSE;
					UINT32 NumSteps = ((NodeBlend*)pNode)->GetNumBlendSteps();
					FirstStepDistance = FirstDistance/NumSteps;
					FoundFirst = TRUE;
					SameNumber = TRUE;
				}
				else
				{
					double NextDistance;
					if (!((NodeBlend*)pNode)->GetBlendDistance(FALSE, &NextDistance))
						return FALSE;
					
					UINT32 NumSteps = ((NodeBlend*)pNode)->GetNumBlendSteps();
					double ThisStepDistance = FirstDistance/NumSteps;

					if (ThisStepDistance != FirstStepDistance)
						return FALSE;
				}
				pNode = pSel->FindNext(pNode);
		}
	}
	return SameNumber;
}

/********************************************************************************************

>	NodeBlender* BlendInfoBarOp::GetBlender()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    pointer to the first nodeblender node in the current selection
				or NULL if there isn't one
	Purpose:	To find the first nodeblender node in a given blend

*********************************************************************************************/

NodeBlender* BlendInfoBarOp::GetBlender()
{
	// first get the selection
	SelRange Sel(*( GetApplication()->FindSelection()));
	NodeBlend* pNodeBlend = NULL;
	
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				 pNodeBlend = (NodeBlend*)pNode;
				break;
			}
			pNode = Sel.FindNext(pNode);
		}
	}

	if (pNodeBlend == NULL)  // if no blend node then there won't be a blender
		return NULL;

	//  now get the blender
	NodeBlender* pBlender = NULL;

	Node* pNode = pNodeBlend->FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IS_KIND_OF(NodeBlender))
		{
			pBlender = (NodeBlender*)pNode;
			break;
		}
		pNode = pNode->FindNext();
	}
	
	return pBlender;
}

/********************************************************************************************

>	NodeBlend* BlendInfoBarOp::GetNodeBlend()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/99
	inputs :    -
	outputs:    pointer to the first nodeblend node in the current selection
				or NULL if there isn't one
	Purpose:	To find the first nodeblend node in the selection

*********************************************************************************************/

NodeBlend* BlendInfoBarOp::GetNodeBlend()
{
	SelRange* pSel = GetApplication()->FindSelection();
	NodeBlend* pNodeBlend = NULL;
	
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();

		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				pNodeBlend = (NodeBlend*)pNode;
				return pNodeBlend;
			}
			pNode = pSel->FindNext(pNode);
		}
	}
	return pNodeBlend;
}

/********************************************************************************************

>	NodeBlend* BlendInfoBarOp::GetBlendOnCurve()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    -
	outputs:    
	returns:    pointer to the first nodeblend in the selection if successful, 
				NULL otherwise
	Purpose:	To determine if the current selection is a blend on a curve

*********************************************************************************************/

NodeBlend* BlendInfoBarOp::GetBlendOnCurve()
{
	// first get the selection
	SelRange Sel(*( GetApplication()->FindSelection()));
	NodeBlend* pNodeBlend = NULL;
	
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				 pNodeBlend = (NodeBlend*)pNode;
				break;
			}
			pNode = Sel.FindNext(pNode);
		}
	}

	if (pNodeBlend == NULL)  // if no blend node then there won't be a blender
		return NULL;
	
	if (pNodeBlend->IsOnACurve())
		return pNodeBlend;
	else 
		return NULL;

}

/********************************************************************************************

>	void BlendInfoBarOp::SetBlendEditState(EditState State)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    edit state to set (EDIT_STEPS or EDIT_DISTANCE)
	outputs:    -
	Purpose:	to set the edit state of all selected blends to the given value

*********************************************************************************************/

void BlendInfoBarOp::SetBlendEditState(EditState State)
{
	// first get the selection
	SelRange Sel(*( GetApplication()->FindSelection()));
//	NodeBlend* pNodeBlend = NULL;
	
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				((NodeBlend*)pNode)->SetEditState(State);
			}
			pNode = Sel.FindNext(pNode);
		}
	}
}

/********************************************************************************************

>	void BlendInfoBarOp::IsBlendDistanceValid(UINT32 MinValue, UINT32 Maxvalue, double Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    the step distance to set, values for the minimum and maximum valid number of 
				steps that are allowed
	outputs:    -
	returns:    TRUE if this distance can be set, FALSE if the distance will cause an invalid number of
				blend steps 
	Purpose:	to see if this step distance will give us a valid number of blend steps

*********************************************************************************************/

BOOL BlendInfoBarOp::IsStepDistanceValid(UINT32 MinValue, UINT32 MaxValue, double Distance)
{
	// first get the selection
	SelRange Sel(*( GetApplication()->FindSelection()));
//	NodeBlend* pNodeBlend = NULL;
	
	if (Distance <= 0)
	{
		InformWarning(_R(IDS_BLENDDISTANCE_TOOSMALL));
		return FALSE;
	}
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				// check to make sure this distance will not cause
				// an illegal number of steps
				UINT32 NumSteps = 0;
				BOOL Valid = ((NodeBlend*)pNode)->GetNumStepsFromDistance(Distance, &NumSteps);
				if (Valid)
				{
					if (NumSteps < MinValue)
					{
						InformWarning(_R(IDS_BLENDDISTANCE_TOOBIG));
						return FALSE;
					}
					else if (NumSteps > MaxValue)
					{
						InformWarning(_R(IDS_BLENDDISTANCE_TOOSMALL));
						return FALSE;
					}
				}
				else
					return FALSE;  
			}
			pNode = Sel.FindNext(pNode);
		}
	}
	return TRUE;
}

/********************************************************************************************

>	virtual CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, INT32* Index, BOOL* bAllSameType)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Inputs:		The GadgetID of the CBiasGainGadget that we are dealing with.
	Outputs:	bMany - returned as TRUE if we have MANY profiles selected.
				bAllSameType - returned as TRUE if objects within selection are all of the
				same type.
	returns		Ptr to common CProfileBiasGain, or NULL if there is NOT one.
	Purpose:	See InformationBarOp::GetProfileFromSelection () for a description of this
				function.

*********************************************************************************************/

CProfileBiasGain* BlendInfoBarOp::GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType)
{
	BOOL ok = ((GadgetID == _R(IDC_BLENDOBJECTBIASGAIN)) || (GadgetID == _R(IDC_BLENDATTRBIASGAIN)));

	ERROR2IF(ok==FALSE, FALSE, "Invalid gadgetID passed");

	//BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	// NOTE:  we could make use of the above function (and base the code around that); BUT since
	// this code works fine - why bother changing it?  If you wan't to change it, then take a look
	// at SoftShadowInfoBarOp::GetProfileFromSelection () ....

	SelRange Sel(*( GetApplication()->FindSelection()));
	
	NodeBlend* pFirstNodeBlend = NULL;
	CProfileBiasGain* pFirstProfile = NULL;
	
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)						// code for comment 1) ....
		{
			if (pNode->IS_KIND_OF(NodeBlend))		// part of condition 3 check
			{
				if (pFirstNodeBlend == NULL)		// code for comment 2) ....
				{
					pFirstNodeBlend = (NodeBlend*) pNode;

					if (GadgetID == _R(IDC_BLENDOBJECTBIASGAIN))
					{
						pFirstProfile = pFirstNodeBlend->GetObjectProfile();
					}
					else if (GadgetID == _R(IDC_BLENDATTRBIASGAIN))
					{
						pFirstProfile = pFirstNodeBlend->GetAttrProfile();
					}
				}
				else
				{
					// code for comment 3) ....
					// condition 1/2 check ....
					// taking into account multiple profile controls ....

					if (GadgetID == _R(IDC_BLENDOBJECTBIASGAIN))
					{
						if (*pFirstProfile == *((NodeBlend*) pNode)->GetObjectProfile())
						{
							// all ok
						}
						else
						{
							*bMany = TRUE;
						}
					}
					else if (GadgetID == _R(IDC_BLENDATTRBIASGAIN))
					{
						if (*pFirstProfile == *((NodeBlend*) pNode)->GetAttrProfile())
						{
							// all ok
						}
						else
						{
							*bMany = TRUE;
						}
					}
				}
			}
			else	// part of condition 3 check
			{
				*bAllSameType = FALSE;
				return (NULL);
			}
			pNode = Sel.FindNext(pNode);
		}
	}

	if (*bMany == TRUE)
	{
		return (NULL);
	}
	else
	{
		return (pFirstProfile);
	}
}

// a big hack: make sure this is fixed..

INT32 BlendInfoBarOp::AttributeIndexModifier(INT32 CurrentIndex)
{
	INT32 retval = 1;

	switch (CurrentIndex)
	{
	case 1:
		retval = 1;
		break;
	case 2:
		retval=  3;
		break;
	case 3:
		retval = 2;
		break;
	case 4:
		retval=  5;
		break;
	case 5:
		retval = 4; 
		break;
	case 6:
		retval = 6;
		break;
	}
	return retval;
}

/********************************************************************************************

>	NodeBlend* BlendInfoBarOp::GetCurrentNodeBlend()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/99
	inputs :    -
	outputs:    -
	returns:    the first nodeblend in the current selection, or null if there isn't one
	Purpose:	utility function to retrieve the first nodeblend in the current selection

*********************************************************************************************/

NodeBlend* BlendInfoBarOp::GetCurrentNodeBlend()
{
	// first get the selection
	SelRange Sel(*( GetApplication()->FindSelection()));
	NodeBlend* pNodeBlend = NULL;
	
	// get the node blend
	if (!Sel.IsEmpty())
	{
		Node* pNode = Sel.FindFirst();
		while (pNode != NULL)
		{
			if (pNode->IS_KIND_OF(NodeBlend))
			{
				 pNodeBlend = (NodeBlend*)pNode;
				break;
			}
			pNode = Sel.FindNext(pNode);
		}
	}

	return pNodeBlend;
}

/********************************************************************************************

>	BOOL BlendInfoBarOp::CheckSelectionAndSet()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/99
	inputs :    -
	outputs:    -
	returns:    TRUE if the selection is changed, FALSE otherwise
	Purpose:	If we have a situation where there is a blend on a path and the user
				changes to the Bezier or FreeHand tool in order to edit the path, 
				if they return to the Blend we wish to have that blend reselected. 
				This is accomplished by checking to see whether the currently selected 
				node is a NodeBlendPath, if it is then we reset the selection to its parent.
				A bit hacky but what can you do...

*********************************************************************************************/

BOOL BlendInfoBarOp::CheckSelectionAndSet()
{
	SelRange* pSel = GetApplication()->FindSelection();

	if (pSel == NULL)
		return FALSE;

	Node* pNode = pSel->FindFirst();
	
	if (pNode != NULL)
	{
		if (pNode->IS_KIND_OF(NodeBlendPath))
		{
			Node* pParent = pNode->FindParent();

			if (pParent->IS_KIND_OF(NodeBlend))
			{	
				NodeRenderableInk::DeselectAll();
				((NodeRenderable*)pParent)->Select(TRUE);
				pSel->Update();
			
				return TRUE;
			}
		}
	}
	return FALSE;

}

/********************************************************************************************

>	BOOL BlendInfoBarOp::NonBlendsSelected()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/99
	inputs :    
	Returns:    TRUE if there are not only blends in the current selection, otherwise FALSE
	Purpose:	To find out if there is anything in the selection apart from blends
*********************************************************************************************/

BOOL BlendInfoBarOp::NonBlendsSelected()
{
	SelRange* pSel = GetApplication()->FindSelection();

	if (pSel == NULL)
		return FALSE;

	Node* pNode = pSel->FindFirst();
	while (pNode != NULL)
	{
		if (pNode->IsAnObject())
		{
			
			if (pNode->IsCompound())
			{
				if (!pNode->IS_KIND_OF(NodeBlend))
				{
					// look inside bevels, contours, etc.
					if (pNode->IS_KIND_OF(NodeBevelController) || 
						pNode->IS_KIND_OF(NodeShadowController) ||
						pNode->IS_KIND_OF(NodeContourController))
					{
						Node* pChild = pNode->FindFirstChild(CC_RUNTIME_CLASS(NodeBlend));
						if (pChild == NULL)
							return TRUE;
					}
				}
			}
			else 
				return TRUE;
		}
		pNode = pSel->FindNext(pNode);
	}
	return FALSE;

}


//////////////////////////////////////////////////////////////////////////////////////////
//  OpBlendNodes
//
// This operation is responsible for creating and editing



/********************************************************************************************

>	OpBlendNodes::OpBlendNodes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Constructor. 

********************************************************************************************/

OpBlendNodes::OpBlendNodes()
{
	pRefStart = NULL;
	pRefEnd   = NULL;
	OpType    = BLENDOPTYPE_NONE;

	pNodeBlend 		= NULL;
	pNodeBlendStart = NULL;
	pNodeBlendEnd	= NULL;
}

/********************************************************************************************

>	OpBlendNodes::~OpBlendNodes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Destructor.

********************************************************************************************/

OpBlendNodes::~OpBlendNodes()
{
}


/********************************************************************************************

>	BOOL OpBlendNodes::DoDrag()
    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This starts a drag that may lead to a blend.
				The DragFinished() method will do the hard work of blending if it can be done.

********************************************************************************************/

BOOL OpBlendNodes::DoDrag(BlendTool* pBlendTool)
{
	// DMc
	// find out the active tool for the drag
	if (Tool::GetCurrent()->GetID() == TOOLID_BLEND)
	{
		pRefStart = BlendTool::GetPtrRefStart();
		pRefEnd   = BlendTool::GetPtrRefEnd();
	}
		
	ERROR2IF_PF(pRefStart == NULL || pRefEnd == NULL,FALSE,("Blend tool refs are NULL"));

	pRefEnd->PointerPos = pRefStart->PointerPos;
	RenderMyDragBlobs();
	m_pBlendTool = pBlendTool;

	// Tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_AUTOSCROLL );

	return TRUE;
}




/********************************************************************************************

>	void OpBlendNodes::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods  - Which key modifiers are being pressed
				pSpread    - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position and calculates the new dragged outline of the EORd
				bounding box
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpBlendNodes::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
	// First Rub out the old box
	RenderMyDragBlobs();

	// Make sure that start and end refer to the same spread
	if (pSpread != pRefStart->pSpread)
		PointerPos = MakeRelativeToSpread(pRefStart->pSpread, pSpread, PointerPos);

	// DMc
	// find out the active tool for the drag
	if (Tool::GetCurrent()->GetID() == TOOLID_BLEND)
	{
		BlendTool::UpdateRef(pRefEnd,pRefStart->pSpread,PointerPos,FALSE);
		BlendTool::CheckNodeRemapping(pRefStart,pRefEnd);
		BlendTool::UpdateCursorAndStatus();
	}
	
	// Render the new drag box
	RenderMyDragBlobs();
}



/********************************************************************************************

>	void OpBlendNodes::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Ends the drag.
				Either creates a new grid or resizes GridClicked depending on the state of affairs
				when the drag started
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpBlendNodes::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	ERROR3IF(pRefStart == NULL,"pRefStart is NULL");
	ERROR3IF(pRefEnd   == NULL,"pRefEnd is NULL");
	if ((pRefStart == NULL) || (pRefEnd == NULL)) return;

	// First Rub out the old box
	RenderMyDragBlobs();

	if (Success)
	{
		// Make sure that start and end refer to the same spread
		if (pSpread != pRefStart->pSpread)
			PointerPos = MakeRelativeToSpread(pRefStart->pSpread, pSpread, PointerPos);

		if (Tool::GetCurrent()->GetID() == TOOLID_BLEND)
		{
			BlendTool::UpdateRef(pRefEnd,pRefStart->pSpread,PointerPos);
			BlendTool::CheckNodeRemapping(pRefStart,pRefEnd);
		}
		else
		{
			FailAndExecute();
			EndDrag();
			delete this;
			return;
		}
	
		String_32 ProgString = _T("Calculating blend, please wait..");
		Progress Hourglass(&ProgString, -1, FALSE);
		Node* pNodeStart = pRefStart->pNode;
		Node* pNodeEnd   = pRefEnd  ->pNode;

		// make the start & end nodes point to any 'needs parent' compounds
		while (pNodeStart)
		{
			if (!pNodeStart->PromoteHitTestOnChildrenToMe() && pNodeStart->IsAnObject())
			{
				pRefStart->pNode = (NodeRenderableInk *)pNodeStart;
			}

			pNodeStart = pNodeStart->FindParent();
		}

		while (pNodeEnd)
		{
			if (!pNodeEnd->PromoteHitTestOnChildrenToMe() && pNodeEnd->IsAnObject())
			{
				pRefEnd->pNode = (NodeRenderableInk *)pNodeEnd;
			}

			pNodeEnd = pNodeEnd->FindParent();
		}


		BOOL ok = FALSE;

		if (pRefStart->pNode == NULL || pRefEnd->pNode == NULL)
		{
			// can't blend so exit
			FailAndExecute();
			EndDrag();
			delete this;
			return;
		}
		else if (pRefStart->pNode == pRefEnd->pNode)
		{
			if (pRefStart->RemapRef == pRefEnd->RemapRef && pRefStart->RemapRef > 0)
			{
				// User's tried to remap two blend blobs

				if (pRefStart->AStartNode != pRefEnd->AStartNode)
					// The blobs lie on opposite ends of a blend
					ok = DoRemapBlend();
			}
			else
			{
				// can't blend so exit
				FailAndExecute();
				EndDrag();
				delete this;
				return;
			}
		}
		else if (IS_A(pRefStart->pNode,NodeBlend) &&
		   		 IS_A(pRefEnd  ->pNode,NodeBlend))
		{
			// Blending a blend to a blend
			ok = DoBlendBlendAndBlend();
		}
		else if (IS_A(pRefStart->pNode,NodeBlend) ||
		   		 IS_A(pRefEnd  ->pNode,NodeBlend))
		{
			// Blending an object to a blend, or a blend to an object
			ok = DoBlendBlendAndObject();
		}
		else
		{
			ok = DoBlendObjects();
		}

		if (!ok) 
			FailAndExecute();

		// Do this at the end so that the status bar text doesn't get wiped before the op begins
		if (Tool::GetCurrent()->GetID() == TOOLID_BLEND)
		{
			BlendTool::UpdateCursorAndStatus();
		}
		else
		{
			
		}
	}
	else
		FailAndExecute();


	// End the drag and the op
	EndDrag();
	End();
}

/********************************************************************************************

>	BOOL OpBlendNodes::DoContourNode(Node * pNode, DocCoord PointerPos, UINT32 Steps )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Inputs:		Node to contour
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	Applies a contour to the given node

********************************************************************************************/
BOOL OpBlendNodes::DoContourNode(Node * pNode, DocCoord PointerPos, UINT32 Steps)
{
	/*
	List NodeList;

	// get the bounding rect of the node to be contoured
	DocRect br = ((NodeRenderableBounded *)pNode)->GetBoundingRect();

	UINT32 Width = 0;

	INT32 OffsetX = 0;
	INT32 OffsetY = 0;

	// try to work out the width of the contour
	if (PointerPos.x < br.lo.x && PointerPos.y >= br.lo.y &&
		PointerPos.y <= br.hi.y)
	{
		Width = br.lo.x - PointerPos.x;
	}
	else if (PointerPos.x > br.hi.x && PointerPos.y >= br.lo.y &&
		PointerPos.y <= br.hi.y)
	{
		Width = PointerPos.x - br.hi.x;
	}
	else if (PointerPos.x >= br.lo.x && PointerPos.x <= br.hi.x &&
		PointerPos.y < br.lo.y)
	{
		Width = br.lo.y - PointerPos.y;
	}
	else if (PointerPos.x >= br.lo.x && PointerPos.x <= br.hi.x &&
		PointerPos.y > br.hi.y)
	{
		Width = PointerPos.y - br.hi.y;
	}
	else if (PointerPos.x < br.lo.x && PointerPos.y < br.lo.y)
	{
		OffsetX = br.lo.x - PointerPos.x;
		OffsetY = br.lo.y - PointerPos.y;

		if (OffsetX > OffsetY)
		{
			Width = OffsetX;
		}
		else
		{
			Width = OffsetY;
		}
	}
	else if (PointerPos.x > br.hi.x && PointerPos.y < br.lo.y)
	{
		OffsetX = PointerPos.x - br.hi.x ;
		OffsetY = br.lo.y - PointerPos.y;

		if (OffsetX > OffsetY)
		{
			Width = OffsetX;
		}
		else
		{
			Width = OffsetY;
		}
	}
	else if (PointerPos.x > br.hi.x && PointerPos.y > br.hi.y)
	{
		OffsetX = PointerPos.x - br.hi.x ;
		OffsetY = PointerPos.y - br.hi.y;

		if (OffsetX > OffsetY)
		{
			Width = OffsetX;
		}
		else
		{
			Width = OffsetY;
		}
	}
	else if (PointerPos.x < br.lo.x && PointerPos.y > br.hi.y)
	{
		OffsetX = br.lo.x - PointerPos.x ;
		OffsetY = PointerPos.y - br.hi.y;

		if (OffsetX > OffsetY)
		{
			Width = OffsetX;
		}
		else
		{
			Width = OffsetY;
		}
	}

	// find top level of the node to contour
	Node * pParent = pNode->FindParent();

	BOOL bContourExists = FALSE;
	
	while (pParent)
	{
		if (pParent->IsKindOf(CC_RUNTIME_CLASS(NodeContourController)))
		{
			pNode = pParent;
			bContourExists = TRUE;
		}

		pParent = pParent->FindParent();
	}

	NodeListItem * pItem = new NodeListItem(pNode);

	NodeList.AddTail(pItem);

	// if a contour exists then change its width
	if (!bContourExists)
	{
		CreateContourParam Param(&NodeList, Steps, Width);

		OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateContour));

		if (pOpDesc && Width > 0)
		{
			pOpDesc->Invoke(&Param);
		}
	}
	else
	{
		ChangeContourWidthParam Param(&NodeList, Width, FALSE);

		OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpChangeContourWidth));

		if (pOpDesc && Width > 0)
		{
			pOpDesc->Invoke(&Param);
		}
	}

	NodeList.DeleteAll();
	*/
	
	return TRUE;

}

/********************************************************************************************

>	BOOL OpBlendNodes::DoBlendBlendAndBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		- (data is taken from pRefStart and pRefEnd member vars)
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This merges to blend nodes together, so that the last object in the first blend
				is blended with the first of the second blend.

********************************************************************************************/

BOOL OpBlendNodes::DoBlendBlendAndBlend()
{
	// Get ptr to the NodeBlends in question, and put them in the member vars pNodeBlendStart & pNodeBlendEnd
	pNodeBlendStart = (NodeBlend*)pRefStart->pNode;
	pNodeBlendEnd   = (NodeBlend*)pRefEnd  ->pNode;

	// DY changed to allow for new blending to and from a blend on a path
	if (pNodeBlendStart->IsOnACurve() || pNodeBlendEnd->IsOnACurve())
	{
		OpType = BLENDOPTYPE_BLENDBLENDONPATH;
		pNodeBlendStart->SetBlendedOnCurve(TRUE);
	}
	else
		OpType = BLENDOPTYPE_BLENDANDBLEND;


	if (pNodeBlendStart == NULL || pNodeBlendEnd == NULL || pNodeBlendStart == pNodeBlendEnd) return FALSE;

	ERROR3IF(!IS_A(pNodeBlendStart,NodeBlend) || !IS_A(pNodeBlendEnd,NodeBlend),"Either start or end is not a NodeBlend");
	if (!IS_A(pNodeBlendStart,NodeBlend) || !IS_A(pNodeBlendEnd,NodeBlend)) return FALSE;

	// Get a record of all the blenders in the blend nodes
	List BlenderListStart,BlenderListEnd;
	if (!RecordBlenderInfo(BlenderListStart,pNodeBlendStart) || !RecordBlenderInfo(BlenderListEnd,pNodeBlendEnd))
	{
		// Tidy up if we fail to get blender info on the two blend nodes
		BlenderListStart.DeleteAll();
		BlenderListEnd  .DeleteAll();
		return FALSE;
	}

	//BlenderInfoItem*   pItemStart = (BlenderInfoItem*)BlenderListStart.GetTail();
	//BlenderInfoItem*   pItemEnd   = (BlenderInfoItem*)BlenderListEnd  .GetHead();
	NodeRenderableInk* pNodeStart = NULL;  //pItemStart->pNodeEnd;
	NodeRenderableInk* pNodeEnd	  = NULL;  //pItemEnd  ->pNodeStart;

	BOOL ok = GetNodeClosestToPoint(&pNodeStart, TRUE);
	if (ok) ok = GetNodeClosestToPoint(&pNodeEnd, FALSE);
	ERROR3IF(pNodeStart == NULL,"The start node ptr is NULL");
	ERROR3IF(pNodeEnd   == NULL,"The end node ptr is NULL");
	if (pNodeStart == NULL || pNodeEnd == NULL)	return FALSE;

	BlobManager* pBlobManager = GetApplication()->GetBlobManager();
	ENSURE(pBlobManager, "Can't get BlobManager");

	ok = DeterminBlendObjectsProcessorHit ();

	// Now we are ready to do the actual blend

	// Firstly, record the selection state
	if (ok) ok = DoStartSelOp(TRUE,TRUE);

	if (ok) ok = (Tool::GetCurrent() != NULL);
	if (ok) pBlobManager->RenderToolBlobsOff(Tool::GetCurrent(), pRefStart->pSpread,NULL);
	if (ok)	NodeRenderableInk::DeselectAll(FALSE);

	// Select the start blend node
	if (ok) pNodeBlendStart->SetSelected(TRUE);

	// Deinit all the blenders in the end blend.
	if (ok) ok = DoDeinitBlenders(BlenderListEnd);

	// Localise the attributes in the start blend
	if (ok) ok = DoLocaliseCommonAttributes(pNodeBlendStart);

	// Localise the attributes in the end blend
	if (ok) ok = DoLocaliseCommonAttributes(pNodeBlendEnd);

	// Hide the end blend node
	NodeHidden* pNodeHidden;
	if (ok) ok = DoHideNode(pNodeBlendEnd,TRUE,&pNodeHidden);

	if (ok)
	{
		// DY update blendpath indexes in existing blenders before we
		// move them
		INT32 FirstBlendNumPaths = pNodeBlendStart->GetNumNodeBlendPaths();
		NodeBlender* pBlender = pNodeBlendEnd->FindFirstBlender();
		while (pBlender != NULL)
		{
			INT32 CurrentIndex = pBlender->GetNodeBlendPathIndex();
			if (CurrentIndex > -1)
			{
				ChangeBlenderOpParam Param;
				Param.m_NewNodeBlendPathIndex = (CurrentIndex + FirstBlendNumPaths);
				Param.m_ChangeType = CHANGEBLENDER_NBPINDEX;
				ok = ChangeBlenderAction::Init(this, &UndoActions, pBlender, Param);
			}
			pBlender =  pNodeBlendEnd->FindNextBlender(pBlender);
		}
	}

	// Now move all the end blend's children (except hidden nodes) into the start blend.
	if (ok)
	{
		Node* pNode = pNodeBlendEnd->FindFirstChild();
		while (pNode != NULL && ok)
		{
			Node* pNext = pNode->FindNext();
	 		if (!pNode->IS_KIND_OF(NodeHidden))
	 			ok = DoMoveNode(pNode,pNodeBlendStart,LASTCHILD);
			
			// DY keep track of which nodeblendpath this is
			//if (pNode->IS_KIND_OF(NodeBlendPath))
			//	((NodeBlendPath*)pNode)->SetPathIndex();
			pNode = pNext;
		}
	}

	// Create a blender node to blend the two nodes
	if (ok)	ok = DoCreateBlender(pNodeStart,-1,pNodeEnd,-1,pNodeStart,NEXT);

	// Reinit the end blenders using the same start and end pointers they were using before the blend
	if (ok) ok = DoReinitBlenders(BlenderListEnd);

	// Factor out any common attrs that might have been copied during the blend
	if (ok) ok = DoFactorOutCommonChildAttributes(pNodeBlendStart);

	// Invalid the whole of the start blend
	if (ok)	ok = DoInvalidateNodeRegion(pNodeBlendStart,TRUE);

	// Clear out the lists (we don't want memory leaks, do we).
	BlenderListStart.DeleteAll();
	BlenderListEnd  .DeleteAll();

	// Throw away the selection cache
	GetApplication()->UpdateSelection();

	return (ok);
}

/********************************************************************************************

>	BOOL OpBlendNodes::DoBlendBlendAndObject()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		- (data is taken from pRefStart and pRefEnd member vars)
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This blends a blend object with another non-blend object.
				This is like a concatination of the the two nodes, resulting a single blend with an extra
				blend stage. E.g. blending a blend containing two paths (i.e. a single blend of two objects) with
				another path.  The result will be a single blend object, blending the first path to the second,
				and the second to the third.

				The order in which the two objects are concatinated depends on which is is the start
				of the blend and which is the end.

********************************************************************************************/

BOOL OpBlendNodes::DoBlendBlendAndObject()
{
	ERROR3IF(!IS_A(pRefStart->pNode,NodeBlend) && !IS_A(pRefEnd->pNode,NodeBlend),"Neither start or end are NodeBlends");
	if (!IS_A(pRefStart->pNode,NodeBlend) && !IS_A(pRefEnd->pNode,NodeBlend)) return FALSE;

	OpType = BLENDOPTYPE_BLENDANDOBJECT;
	// BlendIsStart == TRUE if the blend node was the start of the blend and not the end
	BOOL BlendIsStart = IS_A(pRefStart->pNode,NodeBlend);

	// Get ptr to the NodeBlend in question, and put it in the member var pNodeBlend
	pNodeBlend = NULL;
	if (BlendIsStart) 
		pNodeBlend = (NodeBlend*)pRefStart->pNode; 
	else
		pNodeBlend = (NodeBlend*)pRefEnd->pNode; 

	if (pNodeBlend == NULL) return FALSE;

	// Get a record of all the blenders in the blend node
	List BlenderList;
	if (!RecordBlenderInfo(BlenderList,pNodeBlend))
		return FALSE;

	NodeRenderableInk* pNodeStart = NULL;
	NodeRenderableInk* pNodeEnd = NULL;
	Node* pContextNode;
	AttachNodeDirection AttachDir;
	BOOL ok = FALSE;
	if (BlendIsStart)
	{
//		BlenderInfoItem* pItem = (BlenderInfoItem*)BlenderList.GetTail();
//		pNodeBlendStart = pNodeBlend;
		ok              = GetNodeClosestToPoint(&pNodeStart, TRUE);
		pNodeEnd   		= pRefEnd->pNode;
		pContextNode	= pNodeEnd;
		AttachDir 		= PREV;
	}
	else
	{
//		BlenderInfoItem* pItem = (BlenderInfoItem*)BlenderList.GetHead();
//		pNodeStart 		= pRefStart->pNode;
		pNodeBlendEnd   = pNodeBlend;
		ok              = GetNodeClosestToPoint(&pNodeEnd, FALSE);
		pContextNode 	= pNodeStart;
		AttachDir 		= NEXT;
	}

	ERROR3IF(pNodeBlend == NULL,"Neither start or end are NodeBlends");
	ERROR3IF(pNodeStart == NULL,"The start node ptr is NULL");
	ERROR3IF(pNodeEnd   == NULL,"The end node ptr is NULL");
	if (pNodeBlend == NULL || pNodeStart == NULL || pNodeEnd == NULL)
		return FALSE;

	BlobManager* pBlobManager = GetApplication()->GetBlobManager();
	ENSURE(pBlobManager, "Can't get BlobManager");

	ok = DeterminBlendObjectsProcessorHit ();

	// Firstly, record the selection state
	if (ok) ok = DoStartSelOp(TRUE,TRUE);

	if (ok) ok = (Tool::GetCurrent() != NULL);
	if (ok) pBlobManager->RenderToolBlobsOff(Tool::GetCurrent(), pRefStart->pSpread,NULL);
	if (ok)	NodeRenderableInk::DeselectAll(FALSE);

	// Select th blend node
	if (ok) pNodeBlend->SetSelected(TRUE);

	// Deinit all the blenders in the blend.
	if (ok) ok = DoDeinitBlenders(BlenderList);

	// Localise the attributes in the blend before we start moving node around
	if (ok) ok = DoLocaliseCommonAttributes(pNodeBlend);

	// If blending to a blend, move the start node, and make sure it's deselected
	// Also find the number of paths that will be passed back (for the progress display)
	if (ok && !BlendIsStart) 	ok = DoMoveNode(pNodeStart,pNodeBlend,FIRSTCHILD);
	if (ok && !BlendIsStart) 	ok = DoDeselectNode(pNodeStart);

	// If blending from a blend, move the end node, and make sure it's deselected
	// Also find the number of paths that will be passed back (for the progress display)
	if (ok &&  BlendIsStart) 	ok = DoMoveNode(pNodeEnd,pNodeBlend,LASTCHILD);
	if (ok &&  BlendIsStart) 	ok = DoDeselectNode(pNodeEnd);

	// Create a blender node to blend the two nodes
	if (ok)	ok = DoCreateBlender(pNodeStart,-1,pNodeEnd,-1,pContextNode,AttachDir);

	// Factor out any common attrs that might have been copied during the blend
	if (ok) ok = DoFactorOutCommonChildAttributes(pNodeBlend);

	// Invalidate the whole blend
	if (ok)	ok = DoInvalidateNodeRegion(pNodeBlend,TRUE);

	// Reinit the blenders using the same start and end pointers they were using before the blend
	if (ok) ok = DoReinitBlenders(BlenderList);

	// Clear out the list (we don't want memory leaks, do we).
	BlenderList.DeleteAll();

	// Throw away the selection cache
	GetApplication()->UpdateSelection();

	return (ok);
}

/********************************************************************************************

>	BOOL OpBlendNodes::DoDeinitBlenders(List& BlenderList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		BlenderList = list of blenders to deinit.
	Outputs:	-
	Returns:	-
	Purpose:	This calls DeinitBlenders() on the given list, and creates an action that
				will call ReinitBlenders() on undo, and DeinitBlenders() on redo

********************************************************************************************/

BOOL OpBlendNodes::DoDeinitBlenders(List& BlenderList)
{
	InitBlendersAction* pAction;
	BOOL ok = (InitBlendersAction::Init(this,&UndoActions,&BlenderList,TRUE,&pAction) != AC_FAIL);

	if (ok)	DeinitBlenders(BlenderList);

	return (ok);
}

/********************************************************************************************

>	BOOL OpBlendNodes::DoReinitBlenders(NodeBlend* pNodeBlend)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		BlenderList = list of blenders to deinit.
	Outputs:	-
	Returns:	-
	Purpose:	This calls ReinitBlenders() on the given list, and creates an action that
				will call DeinitBlenders() on undo, and ReinitBlenders() on redo

********************************************************************************************/

BOOL OpBlendNodes::DoReinitBlenders(List& BlenderList)
{
	InitBlendersAction* pAction;
	BOOL ok = (InitBlendersAction::Init(this,&UndoActions,&BlenderList,FALSE,&pAction) != AC_FAIL);

	if (ok)
	{
		ok = ReinitBlenders(BlenderList);
		if (!ok) FailAndExecuteAllButLast();
	}

	return ok;
}

/********************************************************************************************

>	void OpBlendNodes::DeinitBlenders(List& BlenderList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		BlenderList = reference to a list of blender info items
	Outputs:	-
	Returns:	-
	Purpose:	This scans the blenders, calling the Deinit() method for each of them.

********************************************************************************************/

void OpBlendNodes::DeinitBlenders(List& BlenderList)
{
	BlenderInfoItem* pItem = (BlenderInfoItem*) BlenderList.GetHead();

	while (pItem != NULL)
	{
		pItem->pNodeBlender->Deinit();
		pItem = (BlenderInfoItem*) BlenderList.GetNext(pItem);
	}
}

/********************************************************************************************

>	BOOL OpBlendNodes::ReinitBlenders(List& BlenderList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		BlenderList = reference to a list of blender info items
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This scans the blenders, calling the Reinit() method for each of them.
				It uses the start and end node pointers gathered by RecordBlenderInfo() when
				calling Reinit().

********************************************************************************************/

BOOL OpBlendNodes::ReinitBlenders(List& BlenderList)
{
	BOOL ok = TRUE;
	BlenderInfoItem* pItem = (BlenderInfoItem*) BlenderList.GetHead();

	while (pItem != NULL && ok)
	{
		ok = pItem->pNodeBlender->Reinit(pItem->pNodeStart,pItem->pNodeEnd);
		pItem = (BlenderInfoItem*) BlenderList.GetNext(pItem);
	}

	return (ok);
}

/********************************************************************************************

>	BOOL OpBlendNodes::RecordBlenderInfo(List& BlenderList,NodeBlend* pNodeBlend)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Inputs:		BlenderList = reference to a list to put all the blender info items on
				pNodeBlend  = the blend containing the blenders of interested
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This scans the blenders in the given blend node, creating a BlenderInfoItem for each
				NodeBlender.  Each BlenderInfoItem is placed on BlenderList

********************************************************************************************/

BOOL OpBlendNodes::RecordBlenderInfo(List& BlenderList,NodeBlend* pNodeBlend)
{
	ERROR3IF(pNodeBlend == NULL,"pNodeBlend == NULL");
	if (pNodeBlend == NULL) return FALSE;

 	BOOL ok = TRUE;

	Node* pNode = pNodeBlend->FindFirstChild();
	while (pNode != NULL && ok)
	{
		if (IS_A(pNode,NodeBlender))
		{
			NodeBlender* pNodeBlender = (NodeBlender*)pNode;
			BlenderInfoItem* pItem = new BlenderInfoItem;
			ok = (pItem != NULL);

			if (ok)
			{
				pItem->pNodeBlender = pNodeBlender;
				pItem->pNodeStart   = pNodeBlender->GetNodeStart();
				pItem->pNodeEnd     = pNodeBlender->GetNodeEnd();

				ok = (pItem->pNodeStart != NULL && pItem->pNodeEnd != NULL);
			}

			if (ok)	BlenderList.AddTail(pItem);
		}
		pNode = pNode->FindNext();
	}

	return ok;
}

/********************************************************************************************

>	BOOL OpBlendNodes::DoRemapBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/11/94
	Inputs:		- (data is taken from pRefStart and pRefEnd member vars)
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This tries to remap a blend using the data held in pRefStart && pRefEnd.
				It asks the pNodeBlend object to try and remap the two paths by rotating
				the path elements so that the element at Index is the first element in the path.

********************************************************************************************/

BOOL OpBlendNodes::DoRemapBlend()
{
	// What type of blend op are we doing?
	OpType = BLENDOPTYPE_REMAP;

	// Are we going mad? Better check our onions...
	ERROR3IF(pRefStart        		== NULL,"pRefStart == NULL");
	ERROR3IF(pRefEnd          		== NULL,"pRefEnd == NULL");
	ERROR3IF(pRefStart->pNode 		== NULL,"pRefStart->pNode == NULL");
	ERROR3IF(pRefEnd  ->pNode 	  	== NULL,"pRefEnd->pNode == NULL");
	ERROR3IF(pRefStart->pNodeBlend 	== NULL,"pRefStart->pNodeBlend == NULL");
	ERROR3IF(pRefEnd  ->pNodeBlend 	== NULL,"pRefEnd->pNodeBlend == NULL");
	ERROR3IF(pRefStart->pNodeBlend  != pRefEnd->pNodeBlend,"Start and end blend nodes not equal");
	ERROR3IF(pRefStart->RemapRef 	!= pRefEnd->RemapRef,"Start and end remap refs not equal");
	ERROR3IF(pRefStart->AStartNode 	== pRefEnd->AStartNode,"Start and end AStartNode vals equal");

	BOOL ok = TRUE;
	RemapBlendAction* pAction;
	NodeBlend* 	pNodeBlend 	= pRefStart->pNodeBlend;
	UINT32 		RemapRef 	= pRefStart->RemapRef;

	DocCoord	PosStart	= pRefStart->PointerPos;
	DocCoord	PosEnd		= pRefEnd  ->PointerPos;

	if (!pRefStart->AStartNode)
	{
		// Swap the coords around
		DocCoord Temp = PosStart;
		PosStart = PosEnd;
		PosEnd = Temp;
	}

	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,pNodeBlend,this);
	ok = pNodeBlend->AllowOp(&ObjChange);

	if (ok)	ok = DoInvalidateNodeRegion(pRefStart->pNodeBlend,TRUE);

	if (ok) ok = RemapBlendAction::Init(this,&UndoActions,
										pNodeBlend,RemapRef,PosStart,PosEnd,
										&pAction) != AC_FAIL;

	if (ok)
	{
		ObjChange.Define(OBJCHANGE_FINISHED,cFlags,pNodeBlend,this);
		UpdateChangedNodes(&ObjChange);
	}

	return (ok);
}

/********************************************************************************************

>	BOOL OpBlendNodes::RemoveCompoundNodes(CompoundNodeTreeFactoryList * pList)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/99
	Inputs:		The list to return the class factories necessary to regenerate any
				compound nodes which have been removed
	Outputs:	TRUE if all when well, FALSE otherwise
	Purpose:	Removes all compound nodes

********************************************************************************************/
BOOL OpBlendNodes::RemoveCompoundNodes(CompoundNodeTreeFactoryList * pList)
{
/*	Node * pNewStartNode = NULL;
	Node * pNewEndNode = NULL;
	
	if (!RemoveCompoundNodesFromNode(pRefStart->pNode, &pNewStartNode, NULL))
	{
	ERROR3("OpBlendNodes::RemoveCompoundNodes - Removal of compound nodes\n\
		from start node has failed");
	}
	
	if (!RemoveCompoundNodesFromNode(pRefEnd->pNode, &pNewEndNode, pList))
	{
	ERROR3("OpBlendNodes::RemoveCompoundNodes - Removal of compound nodes\n\
		from end node has failed");
	}

	pRefStart->pNode = (NodeRenderableInk *)pNewStartNode;
	pRefEnd->pNode   = (NodeRenderableInk *)pNewEndNode;

	GetApplication()->UpdateSelection();*/
	
	
	return TRUE;
}

/********************************************************************************************

>	BOOL OpBlendNodes::RemoveCompoundNodesFromNode(Node * pNode, Node ** pRetnNode,
								CompoundNodeTreeFactoryList * pList)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/99
	Inputs:		The node to remove all compound nodes from, and the return node to be
				used in the blend as well as the list of compound node factories needed
				to regenerate the compounds nodes removed
	Outputs:	The return node to use in the blend
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	Removes all compound nodes from the given node (i.e. unshadows and unbevels 
				nodes)

********************************************************************************************/
BOOL OpBlendNodes::RemoveCompoundNodesFromNode(Node * pNode, Node ** pRetnNode,
											   CompoundNodeTreeFactoryList * pList)
{
	/*if (!pNode)
		return FALSE;

	if (!pRetnNode)
		return FALSE;

	// first, get the first parent in the hierarchy which has a PromoteHitTestOnChildrenToMe() set
	Node * pParent = pNode->FindParent();

	Node * pPromoteParent = NULL;

	*pRetnNode = pNode;

	while (pParent)
	{
		if (!pParent->PromoteHitTestOnChildrenToMe())
		{
			pPromoteParent = pParent;
		}

		pParent = pParent->FindParent();
	}

	// if we haven't found one - this means that there is one under me
	if (!pPromoteParent)
	{
		pPromoteParent = pNode;
	}

	List CompoundNodeList;

	// get the compound node list for the whole subtree
	BevelTools::GetAllNodesUnderNode(pPromoteParent, &CompoundNodeList, CC_RUNTIME_CLASS(NodeCompound));

	NodeListItem * pNodeToGroupItem = (NodeListItem *)CompoundNodeList.GetHead();
	Node		 * pChildNode		= NULL;
	Node		 * pNextChildNode	= NULL;
	NodeHidden   * pHidden			= NULL;
	Node * pChildNode2 = NULL;

	BOOL ok = TRUE;

	// first, invalidate all the compound node's regions
	while (pNodeToGroupItem && ok)
	{
		if (ok)
			ok = DoInvalidateNodeRegion((NodeRenderableBounded *)pNodeToGroupItem->pNode, TRUE);				

		pNodeToGroupItem = (NodeListItem *)CompoundNodeList.GetNext(pNodeToGroupItem);
	}			

	// now, run through all the compound nodes turning them into groups
	pNodeToGroupItem = (NodeListItem *)CompoundNodeList.GetHead();

	while (pNodeToGroupItem && ok)
	{
		if (pNodeToGroupItem->pNode->ShouldITransformWithChildren())
		{
			
			// find out how many children we have -
			// if there are more than 1 that don't need their parents then we need to group them
			// otherwise, just promote them
			INT32 NumChildren = 0;

			pChildNode2 = pNodeToGroupItem->pNode->FindFirstChild();

			while (pChildNode2)
			{
				if (!pChildNode2->NeedsParent(pNodeToGroupItem->pNode) &&
					!pChildNode2->IsAnAttribute()					   &&
					!pChildNode2->IsNodeHidden()					   &&
					pChildNode2->IsAnObject())
				{
					NumChildren ++;
				}

				pChildNode2 = pChildNode2->FindNext();
			}

			// localise the common attributes underneath the compound node
			if (ok)
				ok = DoLocaliseCommonAttributes((NodeRenderableInk *)pNodeToGroupItem->pNode,
				TRUE);				
			
			if (NumChildren > 1)
			{
				// make a new group node and transfer all child nodes which don't need their
				// parents into this group node
				NodeGroup * pGroup = NULL;
				ALLOC_WITH_FAIL(pGroup, new NodeGroup, this);
				
				*pRetnNode = pGroup;
				
				// get the first child
				pChildNode = pNodeToGroupItem->pNode->FindFirstChild();
				
				while (pChildNode && ok)
				{
					pNextChildNode = pChildNode->FindNext();
					
					if (!pChildNode->NeedsParent(pNodeToGroupItem->pNode) &&
						pChildNode->IsAnObject() && !pChildNode->IsNodeHidden())
					{
						if (ok) 
							ok = DoMoveNode(pChildNode, pGroup, LASTCHILD);
					}
					// continuity check
					else if (pChildNode->ShouldITransformWithChildren())
					{
						ERROR3("Found a child node which is a compound node !");
					}
					
					pChildNode = pNextChildNode;
				}
				
				// insert the group node into the tree, and hide the original node
				
				if (ok)
				{
					ok = DoInsertNewNode(	pGroup,
						pNodeToGroupItem->pNode,
						NEXT,
						FALSE);
				}
			}
			else
			{
				// if there's only one node, then move it
				// find the first node that NeedParent returns FALSE
				pChildNode = pNodeToGroupItem->pNode->FindFirstChild();

				while (pChildNode->NeedsParent(pNodeToGroupItem->pNode)  ||
					   pChildNode->IsAnAttribute()						 ||
					   pChildNode->IsNodeHidden()						 ||
					   !pChildNode->IsAnObject()
					   )
				{
					pChildNode = pChildNode->FindNext();
				}

				// ok, move the node & set the passback variable
				if (ok)
				{
					ok = DoMoveNode(pChildNode, pNodeToGroupItem->pNode, NEXT);

					*pRetnNode = pChildNode;
				}
			}

			// before we hide the original, add its class factory to the list
			if (pList)
			{
				CompoundNodeTreeFactory * pFactory = NULL;
				
				((NodeCompound *)pNodeToGroupItem->pNode)->CreateCompoundNodeTreeFactory(&pFactory);
				
				if (pFactory)
				{
					pList->AddItem(pFactory);
				}
			}
				
			// hide the original
			if (ok)
			{
				ok = DoHideNode	( pNodeToGroupItem->pNode,
								  TRUE,
								  &pHidden,
								  TRUE);
			}
		}

		pNodeToGroupItem = (NodeListItem *)CompoundNodeList.GetNext(pNodeToGroupItem);
	}

	CompoundNodeList.DeleteAll();
	*/

	return TRUE;
}



/********************************************************************************************

>	BOOL OpBlendNodes::DoBlendObjects()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		(data is taken from pRefStart and pRefEnd member vars)
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This blends two objects together to form a new blend object in the tree.
				All the data that's required is stored in pRefStart (for the start object)
				and pRefEnd (for the end object).
				It assumes that neither the start or end objects are blends, so no clever
				merging of blend objects will take place here.				

********************************************************************************************/

BOOL OpBlendNodes::DoBlendObjects()
{
	// DMc - are the nodes to be blended shadowed or bevelled ?
//	BOOL bIsShadowed = FALSE;
//	BOOL bIsBevelled = FALSE;

	// What type of blend op are we doing?
	OpType = BLENDOPTYPE_NEW;

	// Are we going mad? Better check our onions...
	ERROR3IF_PF(pRefStart        == NULL,("pRefStart == NULL"));
	ERROR3IF_PF(pRefEnd          == NULL,("pRefEnd == NULL"));
	ERROR3IF_PF(pRefStart->pNode == NULL,("pRefStart->pNode == NULL"));
	ERROR3IF_PF(pRefEnd  ->pNode == NULL,("pRefEnd->pNode == NULL"));

	BOOL ok = DeterminBlendObjectsProcessorHit ();

	if (!ok)
	{
		return (FALSE);
	}

	ok = DoStartSelOp(TRUE,TRUE);

	CompoundNodeTreeFactoryList CFList;

	RemoveCompoundNodes(&CFList);

	NodeBlend* pNodeBlend;

	// DMc
	// move the end nodes upwards to include compound nodes
	Node * pParent = pRefStart->pNode;

	while (pParent)
	{
		if (!pParent->PromoteHitTestOnChildrenToMe() && pParent->IsAnObject())
		{
			pRefStart->pNode = (NodeRenderableInk *)pParent;
		}

		pParent = pParent->FindParent();
	}

	pParent = pRefEnd->pNode;

	while (pParent)
	{
		if (!pParent->PromoteHitTestOnChildrenToMe() && pParent->IsAnObject())
		{
			pRefEnd->pNode = (NodeRenderableInk *)pParent;
		}

		pParent = pParent->FindParent();
	}

	BlobManager* pBlobManager = GetApplication()->GetBlobManager();
	ENSURE(pBlobManager, "Can't get BlobManager");

	if (ok) ok = (Tool::GetCurrent() != NULL);
	if (ok) pBlobManager->RenderToolBlobsOff(Tool::GetCurrent(), pRefStart->pSpread,NULL);
	if (ok)	NodeRenderableInk::DeselectAll(FALSE,FALSE);

	if (ok) ALLOC_WITH_FAIL(pNodeBlend,new NodeBlend,this);
	if (ok) ok = (pNodeBlend != NULL);

	if (ok) pRefStart->pNode->SetSelected(FALSE);
	if (ok) pRefEnd  ->pNode->SetSelected(FALSE);

	if (ok) ok = DoInsertNewNode(pNodeBlend,pRefEnd->pNode,NEXT,FALSE,FALSE,FALSE,FALSE);
	if (ok) ok = DoMoveNode(pRefStart->pNode,pNodeBlend,LASTCHILD);
	if (ok) ok = DoMoveNode(pRefEnd  ->pNode,pNodeBlend,LASTCHILD);
	if (ok) ok = DoCreateBlender(pRefStart->pNode,pRefStart->Index,pRefEnd->pNode,pRefEnd->Index,pRefStart->pNode,NEXT);
	if (ok)	ok = DoInvalidateNodeRegion(pNodeBlend,TRUE);

	// Simon - Factor out the blend's common attributes
	if (ok) ok = DoFactorOutCommonChildAttributes(pNodeBlend);


	if (ok)
		pNodeBlend->SetSelected(TRUE);		// Select the new node

	// Throw away the selection cache
	GetApplication()->UpdateSelection();

	return (ok);
}



/********************************************************************************************

>	BOOL OpBlendNodes::DeterminBlendObjectsProcessorHit ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if should continue, FALSE otherwise
	Purpose:	This function provides a quick (and rough) estimate on the amount of work
				that will have to be done to create the blend.  This estimate is based upon
				the total length of all paths that exist within the start and end nodes.
				An attempt is also made at altering the 'threshold' based upon the CPU type.

				This system could probably be enhanced - but for now it seems to work fine.

********************************************************************************************/

BOOL OpBlendNodes::DeterminBlendObjectsProcessorHit ()
{
	Node* pStart = pRefStart->pNode;
	Node* pEnd = pRefEnd->pNode;

	INT32 complexityEstimate1 = pStart->EstimateNodeComplexity (NULL);
	INT32 complexityEstimate2 = pEnd->EstimateNodeComplexity (NULL);

	INT32 generatedPathsEstimate = 3*complexityEstimate1 + 2*complexityEstimate2;

	static BOOL foundCPU = FALSE;
	static INT32 pathsBeforeAsk = 0;

	if (!foundCPU)
	{
/*		SYSTEM_INFO systemInfo;
		GetSystemInfo (&systemInfo);

		if (systemInfo.dwProcessorType == PROCESSOR_INTEL_386)
		{
			pathsBeforeAsk = 6000;
		}
		else if (systemInfo.dwProcessorType == PROCESSOR_INTEL_486)
		{
			pathsBeforeAsk = 12000;
		}
		else if (systemInfo.dwProcessorType == PROCESSOR_INTEL_PENTIUM)
		{
			pathsBeforeAsk = 20000;
		}
		else */
		{
			// assume its faster than a pentium .... (in 2005 this will alwys be the case!)
			pathsBeforeAsk = 24000;
		}
		foundCPU = TRUE;
	}

	if (generatedPathsEstimate > pathsBeforeAsk)
	{
		// Load and build the question text.
		String_256 QueryString(_R(IDS_ASKLONGJOB));
						
		// The only way of bringing up a box with a string in it
		Error::SetError(0, QueryString, 0);
		INT32 DlgResult = InformMessage(0, _R(IDS_YES), _R(IDS_NO));
		Error::ClearError();

		switch (DlgResult)
		{
			case 1:		// YES
				return (TRUE);
			break;
			case 2:		// NO
				return (FALSE);			// break out of this stuff!
		}
	}

	return (TRUE);
}

/********************************************************************************************

>	BOOL OpBlendNodes::DoCreateBlender(	NodeRenderableInk* pNodeStart,INT32 PathIndexStart,
										NodeRenderableInk* pNodeEnd,  INT32 PathIndexEnd,
										Node* pContextNode, AttachNodeDirection AttachDir)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94 
	Inputs:		pNodeStart 		= the start node to blend
				PathIndexStart 	= index into start path, if applicable (-1 means not applicable)
				pNodeEnd   		= the end node to blend
				PathIndexEnd   	= index into end   path, if applicable (-1 means not applicable)
				pContextNode	= ptr to context for insertion of the blender
				AttachDir		= how to attach the blender to the tree in relation to pContextNode
				
	Outputs:	-
	Returns:	TRUE if all when well, FALSE otherwise
	Purpose:	This forms a blender object in the tree that can blend pRefStart->pNode
				to pRefEnd->pNode.

********************************************************************************************/

BOOL OpBlendNodes::DoCreateBlender(	NodeRenderableInk* pNodeStart,INT32 PathIndexStart,
									NodeRenderableInk* pNodeEnd,  INT32 PathIndexEnd,
									Node* pContextNode, AttachNodeDirection AttachDir)
{
	ERROR2IF(pNodeStart == NULL,FALSE,"pNodeStart is NULL");
	ERROR2IF(pNodeEnd   == NULL,FALSE,"pNodeEnd   is NULL");

	// Work out how many paths will be passed back
	// The sum will be used to determine the final count for the progress bar
	BecomeA TestStart(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
	BecomeA TestEnd(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
	TestStart.ResetCount();
	TestEnd.ResetCount();
	pNodeStart->CanBecomeA(&TestStart);
	pNodeEnd  ->CanBecomeA(&TestEnd);

	// Create the hourglass with the correct final count.
	Progress Hourglass(_R(IDS_BLENDING), INT32(TestStart.GetCount() + TestEnd.GetCount()) );

	BOOL ok = FALSE;
	NodeBlender* pNodeBlender;

	ALLOC_WITH_FAIL(pNodeBlender,new NodeBlender,this);
	ok = (pNodeBlender != NULL);

	// If either of the indexes are -ve, make sure both are -ve
	if (PathIndexStart < 0 || PathIndexEnd < 0)
		PathIndexStart = PathIndexEnd = -1;

	if (ok) ok = DoInsertNewNode(pNodeBlender,pContextNode,AttachDir,FALSE,FALSE,FALSE,FALSE);
	if (ok) ok = pNodeBlender->Initialise(pNodeStart,pNodeEnd,PathIndexStart,PathIndexEnd,this,&Hourglass,FALSE);
	
	pRefStart->pNode = pNodeStart;
	pRefEnd->pNode = pNodeEnd;
	return (ok);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

/********************************************************************************************

>	virtual void OpBlendNodes::RenderMyDragBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Rect 	- The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed rectangle defined by AnchorPoint and DragPoint

********************************************************************************************/

void OpBlendNodes::RenderMyDragBlobs()
{
	INT32 x0 = pRefStart->PointerPos.x;
	INT32 y0 = pRefStart->PointerPos.y;
	INT32 x1 = pRefEnd  ->PointerPos.x;
	INT32 y1 = pRefEnd  ->PointerPos.y;

	DocRect Rect = DocRect(	min(x0,x1),min(y0,y1),max(x0,x1),max(y0,y1));

	RenderDragBlobs(Rect, pRefStart->pSpread, FALSE);
}



/********************************************************************************************

>	void OpBlendNodes::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Rect 	- The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed rectangle defined by AnchorPoint and DragPoint

********************************************************************************************/

void OpBlendNodes::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
	if (pRefStart->PointerPos == pRefEnd->PointerPos)
		return;

	// Get the scaled pixel size for the view.
	FIXED16 ScaledPixelWidth,
			ScaledPixelHeight;
	GetWorkingView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

	MILLIPOINT LineWidth = ScaledPixelWidth.MakeLong()*2;
	StockColour Colour = COLOUR_BLACK;

	if (pRefStart->RemapRef == pRefEnd->RemapRef && pRefStart->RemapRef > 0 &&
		pRefStart->AStartNode != pRefEnd->AStartNode)
	{
		Colour = COLOUR_RED;
		LineWidth += LineWidth;
	}
	else if ((pRefStart->pNode != NULL) &&
			 (pRefEnd  ->pNode != NULL) &&
			 (pRefStart->pNode != pRefEnd->pNode) &&
			 (pRefStart->Index >= 0) &&
			 (pRefEnd  ->Index >= 0))
	{
		Colour = COLOUR_RED;
		LineWidth += LineWidth;
	}

	// Inflate for the width of the line
	Rect.Inflate(LineWidth);

	RenderRegion* pRegion = DocView::RenderOnTop( &Rect, pSpread, UnclippedEOR );

	while ( pRegion != NULL )
	{
		// Set the line colour and Draw the rect
		pRegion->SetLineColour(Colour);

		// Draw the line
		pRegion->SetLineWidth(LineWidth);
		pRegion->DrawLine(pRefStart->PointerPos,pRefEnd->PointerPos);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop( &Rect );
	}
}



/********************************************************************************************

>	BOOL OpBlendNodes::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpBlendNodes::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_BLEND_TOOL),
								CC_RUNTIME_CLASS(OpBlendNodes), 
								OPTOKEN_BLENDNODES,
								OpBlendNodes::GetState,
								0,      	/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpBlendNodes::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpBlendNodes::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	
	return State;
}

/********************************************************************************************

>	virtual void OpBlendNodes::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		OpName = ptr to str to place op name in
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpBlendNodes::GetOpName(String_256* OpName) 
{ 
	switch (OpType)
	{
		case BLENDOPTYPE_NEW:
		case BLENDOPTYPE_BLENDANDOBJECT:
		case BLENDOPTYPE_BLENDANDBLEND:
			*OpName = String_256(_R(IDS_BLEND_UNDO));
			break;

		case BLENDOPTYPE_REMAP:
			*OpName = String_256(_R(IDS_REMAP_UNDO));
			break;

		default:
			break;
	}
}  

/********************************************************************************************

>	virtual BOOL OpBlendNodes::Undo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	Gets called whenever this operation is undone.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpBlendNodes::Undo()
{
	// Pre-undo stuff
	switch (OpType)
	{
		case BLENDOPTYPE_NEW:
		case BLENDOPTYPE_BLENDANDBLEND:
		case BLENDOPTYPE_REMAP:
		case BLENDOPTYPE_BLENDANDOBJECT:
		case BLENDOPTYPE_BLENDBLENDONPATH:
			break;
		default:
			ERROR3_PF(("Unknown blend OpType (%d)",OpType));
			break;
	}

	// Do the Undo
	SelOperation::Undo();

	// Post-undo stuff
	switch (OpType)
	{
		case BLENDOPTYPE_NEW:
		case BLENDOPTYPE_REMAP:
			break;

		case BLENDOPTYPE_BLENDANDBLEND:
		/*
			// Check that we have a blend object
			ERROR3IF(pNodeBlendStart==NULL,"pNodeBlendStart == NULL");
			ERROR3IF(pNodeBlendEnd  ==NULL,"pNodeBlendEnd == NULL");
			if (pNodeBlendStart == NULL || pNodeBlendEnd == NULL) return TRUE;
			DeinitAndReinitBlend(pNodeBlendStart);
			DeinitAndReinitBlend(pNodeBlendEnd);
		*/
			break;

		case BLENDOPTYPE_BLENDANDOBJECT:
		/*
			// Check that we have a blend object
			ERROR3IF(pNodeBlend==NULL,"pNodeBlend == NULL");
			if (pNodeBlend == NULL) return TRUE;
			DeinitAndReinitBlend(pNodeBlend);
		*/
			break;

		default:
			break;
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL OpBlendNodes::Redo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	Gets called whenever this operation is Redone.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpBlendNodes::Redo()
{
	// Pre-Redo stuff
	switch (OpType)
	{
		case BLENDOPTYPE_NEW:
		case BLENDOPTYPE_BLENDANDBLEND:
		case BLENDOPTYPE_REMAP:
		case BLENDOPTYPE_BLENDANDOBJECT:
		case BLENDOPTYPE_BLENDBLENDONPATH:
			break;
		default:
			ERROR3_PF(("Unknown blend OpType (%d)",OpType));
			break;
	}
	SelRange* pSel = GetApplication()->FindSelection();
	/*Node* pNode =*/ pSel->FindFirst();

	// Do the Redo
	SelOperation::Redo();

	// Post-Redo stuff
	switch (OpType)
	{
		case BLENDOPTYPE_NEW:
		case BLENDOPTYPE_REMAP:
			break;

		case BLENDOPTYPE_BLENDANDBLEND:
		/*
			// Check that we have a blend object
			ERROR3IF(pNodeBlendStart==NULL,"pNodeBlendStart == NULL");
			if (pNodeBlendStart == NULL) return TRUE;
			DeinitAndReinitBlend(pNodeBlendStart);
		*/
			break;

		case BLENDOPTYPE_BLENDANDOBJECT:
		/*
			// Check that we have a blend object
			ERROR3IF(pNodeBlend==NULL,"pNodeBlend == NULL");
			if (pNodeBlend == NULL) return TRUE;
			DeinitAndReinitBlend(pNodeBlend);
		*/
			break;

		default:
			break;
	}

	return TRUE;
}


/********************************************************************************************

>	void OpBlendNodes::GetStartAndEndNodes(NodeRenderableink** pInk,BOOL StartBlend)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/99
	Inputs:	    -
	Outputs;    ppInk, the starting node of the blend
				StartBlend - do we wish to find the start or end 
	Returns:    TRUE if successful , FALSE otherwise, or if we are not blending two blends. 
	Purpose:    when blending from two blends, this retrieves the node closest to the actual 
				point where the blend began or ended
********************************************************************************************/

BOOL OpBlendNodes::GetNodeClosestToPoint(NodeRenderableInk** ppInk, BOOL StartBlend)
{
//	ERROR3IF_PF(pNodeBlendStart == NULL; "Nodeblend start is NULL");
//	ERROR3IF_PF(pNodeBlendEnd == NULL; "Nodeblend end is NULL");

	DocCoord Point;
//	Node* pStart = NULL;
//	Node* pEnd = NULL;
//	BOOL ok = FALSE;
	NodeBlend* pBlend = NULL;
	
	if (StartBlend)
	{
		Point = pRefStart->PointerPos;
		//ok = pNodeBlendStart->GetStartAndEndNodes(&pStart, &pEnd);
		pBlend = pNodeBlendStart;
	}
	else
	{
		Point = pRefEnd->PointerPos;
		//ok = pNodeBlendEnd->GetStartAndEndNodes(&pStart, &pEnd);
		pBlend = pNodeBlendEnd;
	}
	
	
	double ClosestDistance = 9999999999999.9;
	Node* pClosestNode = NULL;
	Node* pNode = pBlend->FindFirstChild();
	while (pNode != NULL)
	{
		// we are looking for inks but don't want blenders or nodeblendpaths
		// could do with some virtual functions here
		if (pNode->IS_KIND_OF(NodeRenderableInk) && 
			(!pNode->IS_KIND_OF(NodeBlender)) && 
			(!pNode->IS_KIND_OF(NodeBlendPath)))
		{
			DocRect Rect = ((NodeRenderableInk*)pNode)->GetBoundingRect();
			Coord Centre = (Coord)(Rect.Centre());
			double Distance = Point.Distance(Centre);
		
			if (Distance < ClosestDistance)
			{
				pClosestNode = pNode;
				ClosestDistance = Distance;
			}
		}
		pNode = pNode->FindNext();
	}
	if (pClosestNode != NULL)
	{
		*ppInk = (NodeRenderableInk*)pClosestNode;
		return TRUE;
	}
	else
	{
		ERROR3("Couldn't find a node close to point");
		return FALSE;
	}
}



/********************************************************************************************

>	BOOL OpBlendNodes::DeinitAndReintBlend(NodeBlend* pThisNodeBlend)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/94
	Inputs:		pThisNodeBlend = ptr to a blend node
	Outputs:	-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This gets a list of all the blenders in pNodeBlend.
				The it calls Deinit() followed by Reinit() on the blenders.
				The Reinit() is sent the actual object pointers to the objects they blended.				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

/*BOOL OpBlendNodes::DeinitAndReinitBlend(NodeBlend* pThisNodeBlend)
{
	BOOL ok = FALSE;
	List BlenderList;
	if (RecordBlenderInfo(BlenderList,pThisNodeBlend))
	{
		DeinitBlenders(BlenderList);
		ok = ReinitBlenders(BlenderList);
	}
	BlenderList.DeleteAll();

	return ok;
}
*/


class BlendPathBecomeA : public BecomeA
{
public:
	BlendPathBecomeA(UndoableOperation* pOp) : BecomeA(BECOMEA_PASSBACK,CC_RUNTIME_CLASS(NodePath),pOp)
	{
		m_pNodePath			= NULL;
		m_pAttrMap			= NULL;
	}

	~BlendPathBecomeA()
	{
		if (m_pNodePath != NULL)
			delete m_pNodePath;

		if (m_pAttrMap != NULL)
		{
			m_pAttrMap->DeleteAttributes();
			delete m_pAttrMap;
		}
	}

	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap=NULL)
	{
		ERROR2IF(pCreatedByNode == NULL,FALSE,"pCreatedByNode == NULL");
		ERROR2IF(pNewNode == NULL,FALSE,"pNewNode == NULL");
		ERROR2IF(!pNewNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)),FALSE,"pNewNode not a kind of NodePath");

		BOOL ok = FALSE;
		if (m_pNodePath == NULL)
		{
			m_pNodePath = new NodePath;
			if (m_pNodePath)
			{
				if (!m_pNodePath->SetUpPath())
				{
					delete m_pNodePath;
					m_pNodePath = NULL;
				}

			}
		}

		if (m_pNodePath)
		{
			Path& PassedPath = ((NodePath*)pNewNode)->InkPath;
			INT32 Len = PassedPath.GetNumCoords();

					ok = m_pNodePath->InkPath.MakeSpaceInPath(Len);
			if (ok) ok = m_pNodePath->InkPath.MergeTwoPaths(PassedPath);
		}

		if (m_pAttrMap == NULL)
		{
			if (pAttrMap == NULL)
			{
				CCAttrMap* pTempAttrMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
				if (pTempAttrMap)
				{
					m_pAttrMap = pTempAttrMap->Copy();
					delete pTempAttrMap;
				}
			}
			else
				m_pAttrMap = pAttrMap;
		}
		else
		{
			if (pAttrMap != NULL)
			{
				pAttrMap->DeleteAttributes();
				delete pAttrMap;
				pAttrMap = NULL;
			}
		}

		if (pNewNode)
		{
			pNewNode->CascadeDelete();
			delete pNewNode;
			pNewNode = NULL;
		}

		return ok;
	}

	NodePath*	GetNodePath()	{ return m_pNodePath; }
	CCAttrMap*	GetAttrMap()	{ return m_pAttrMap; }

private:
	NodePath*	m_pNodePath;
	CCAttrMap*	m_pAttrMap;
};


///////////////////////////////////////////////////////////////////////////////////////////
//  OpAddBlendPath
//
// Creates a single path out of all the selected objects that aren't blends
// and applies them to all the selected blends.

/********************************************************************************************

>	void OpAddBlendPath::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/99
	Returns:	-
	Purpose:	This removes all the selected blend objects, leaving the original objects in the 
				tree.

********************************************************************************************/


void OpAddBlendPath::Do(OpDescriptor*)
{
	SelRange* pSel = GetApplication()->FindSelection();

	BOOL ok = (pSel != NULL);
	

	if (ok) ok = DoStartSelOp(FALSE,FALSE);
	
	/* DY 16/9 in order to render the BlendPath either on
	top of or below the rest of the blend we need to know where it was in the 
	tree before we began */
	BOOL BlendPathOnTop = FALSE;
	if (ok)
		 BlendPathOnTop = IsBlendPathOnTopOfBlend(pSel);

	if (ok)
	{
		// Get an ObjChangeParam ready so we can tell the selected objects what we hope to do 
		// to the them (i.e. effectively delete them and use their paths).
		ObjChangeFlags cFlags;
		cFlags.DeleteNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

		BlendPathBecomeA BPBecomeA(this);
		BecomeA TestPathBecomeA(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));

		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;
			pSelNode = pSel->FindNext(pSelNode);

			if (!pNode->IS_KIND_OF(NodeBlend) && pNode->IsAnObject() && pNode->CanBecomeA(&TestPathBecomeA))
			{
				// Will the blend node allow us to do the op to it?
						ok = pNode->AllowOp(&ObjChange);
				if (ok) ok = pNode->DoBecomeA(&BPBecomeA);
				if (ok) ok = DoInvalidateNodeRegion((NodeRenderableInk*)pNode,TRUE,FALSE);
				if (ok) ok = DoHideNode(pNode,TRUE);
			}
		}

		// Get an ObjChangeParam ready so we can tell the selected blends what we hope to do 
		// to the them (i.e. effectively add or change the blend path "attribute").
		ObjChangeFlags cBlendFlags;
		cBlendFlags.Attribute = TRUE;
		ObjChangeParam BlendChange(OBJCHANGE_STARTING,cBlendFlags,NULL,this);

		NodePath*  pBlendPath = BPBecomeA.GetNodePath();
		CCAttrMap* pAttrMap   = BPBecomeA.GetAttrMap();

		if (ok) ok = (pBlendPath != NULL);

		pSelNode = pSel->FindFirst();
		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;
			pSelNode = pSel->FindNext(pSelNode);

			if (pNode->IS_KIND_OF(NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pNode;
				
				
						ok = pNodeBlend->AllowOp(&BlendChange);
				if (ok) ok = pNodeBlend->BlendAlongThisPath(pBlendPath,pAttrMap,this, BlendPathOnTop);
				if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE);
				if (ok) 
				{
					// Diccon 9/99 added changeblend action
					ChangeBlendAction* pAction;
					ChangeBlendOpParam ChangeParam;
					ChangeParam.ChangeType = CHANGEBLEND_BLENDONPATH;
					ChangeParam.NewBlendedOnCurve = TRUE;
					ok = ChangeBlendAction::Init(this,&UndoActions,pNodeBlend,&ChangeParam,&pAction) != AC_FAIL;
				}
								     
					pNodeBlend->UpdateStepDistance();
			}
		}

		if (ok)
		{
			// update the effected parents after the change
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
			UpdateChangedNodes(&ObjChange);
		}
	}

	if (ok) 
	{
		pSel->Update();

	}
	else
		FailAndExecute();

	End();
}

/********************************************************************************************

>	BOOL OpAddBlendPath::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpAddBlendPath::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_ADDBLENDPATH),
								CC_RUNTIME_CLASS(OpAddBlendPath), 
								OPTOKEN_ADDBLENDPATH,
								OpAddBlendPath::GetState,
								0,      	/* help ID */
								_R(IDBBL_ADDBLENDPATH),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpAddBlendPath::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpAddBlendPath::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// Get an ObjChangeParam ready so we can tell the selected blend's parents what we hope to do 
	// to the blend (i.e. replace it with other nodes).
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	BOOL Denied = FALSE;
	SelRange* pSel = GetApplication()->FindSelection();

	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL && State.Greyed)
		{
			if (IS_A(pNode,NodeBlend))
			{
				// will the node allow the op to happen? (Don't set op permissions - we're only asking)
				if (pNode->AllowOp(&ObjChange,FALSE))
					State.Greyed = FALSE;		// Yes! we can ungrey
				else
					Denied = TRUE;				// Oooh! we've been denied by at least one selected blend
			}
			pNode = pSel->FindNext(pNode);
		}
	}

	UINT32 IDS = 0;
	if (State.Greyed)
	{
		// If we are greyed because we've been denied, get the reason for the denial
		if (Denied)
			IDS = ObjChange.GetReasonForDenial();
		else
			IDS = _R(IDS_ADDBLENDPATH_GREYED);
	}


	if (IDS == 0)
		IDS = _R(IDS_ADDBLENDPATH);

	*Description = String_256(IDS);

	return State;
}

/********************************************************************************************

>	virtual void OpAddBlendPath::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpAddBlendPath::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_ADDBLENDPATH_UNDO));
}  

//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
// OpDetachBlendPath

void OpDetachBlendPath::Do(OpDescriptor*)
{
	SelRange* pSel = GetApplication()->FindSelection();

	BOOL ok = (pSel != NULL);

	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	if (ok)
	{
		// Get an ObjChangeParam ready so we can tell the selected blends what we hope to do 
		// to the them (i.e. effectively remove the blend path "attribute").
		ObjChangeFlags cBlendFlags;
		cBlendFlags.Attribute = TRUE;
		ObjChangeParam BlendChange(OBJCHANGE_STARTING,cBlendFlags,NULL,this);

		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL && ok)
		{
			if (pSelNode->IS_KIND_OF(NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pSelNode;

						ok = pNodeBlend->AllowOp(&BlendChange);
				if (ok) ok = pNodeBlend->DetachNodeBlendPath(pNodeBlend,NEXT,this);
				if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE);
				if (ok)	ok = pNodeBlend->RotateBlendEndObjectsBack(this);
				if (ok) 
				{	
					// Diccon 9/99 added changeblend action to solve sticky button prob.
					ChangeBlendAction* pAction;
					ChangeBlendOpParam ChangeParam;
					ChangeParam.ChangeType = CHANGEBLEND_BLENDONPATH;
					ChangeParam.NewBlendedOnCurve = FALSE;
					ok = ChangeBlendAction::Init(this,&UndoActions,pNodeBlend,&ChangeParam,&pAction) != AC_FAIL;
				}
				if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE);
			}
			pSelNode = pSel->FindNext(pSelNode);
		}

		if (ok)
		{
			// update the effected parents after the change
			BlendChange.Define(OBJCHANGE_FINISHED,cBlendFlags,NULL,this);
			UpdateChangedNodes(&BlendChange);
		}
	}

	if (ok)
	{
		pSel->Update();
		// re-render the blobs on the path
		/*BlobManager* BlobMgr =*/ GetApplication()->GetBlobManager(); // AB - why does GetBlobManager rerender the paths?
	}
	else
		FailAndExecute();

	End();
}

/********************************************************************************************

>	BOOL OpAddBlendPath::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/99
	Returns:	TRUE if path to blend to is an older sibling to the blend, FALSE otherwise
	Purpose:    Little utility called by OpAddBlendPath::Do, does what the name suggests

********************************************************************************************/
BOOL OpAddBlendPath::IsBlendPathOnTopOfBlend(SelRange* pSel)
{
	Node* pNode = pSel->FindFirst();
	
	while (pNode != NULL)
	{
		if (pNode->IS_KIND_OF(NodeBlend))
			return TRUE;
		else if (pNode->IS_KIND_OF(NodePath))
			return FALSE;

		pNode = pSel->FindNext(pNode);
	}

	/* no way we should reach this point as there must be
	either one or the other in the selection for the op to be
	allowed, but just in case.. */
	return TRUE;
}


/********************************************************************************************

>	BOOL OpDetachBlendPath::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpDetachBlendPath::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_DETACHBLENDPATH),
								CC_RUNTIME_CLASS(OpDetachBlendPath), 
								OPTOKEN_DETACHBLENDPATH,
								OpDetachBlendPath::GetState,
								0,      	/* help ID */
								_R(IDBBL_DETACHBLENDPATH),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpDetachBlendPath::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpDetachBlendPath::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// Get an ObjChangeParam ready so we can tell the selected blend's parents what we hope to do 
	// to the blend (i.e. change it's "blend path" attribute")
	ObjChangeFlags cFlags;
	cFlags.Attribute = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	BOOL Denied = FALSE;
	SelRange* pSel = GetApplication()->FindSelection();

	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL && State.Greyed)
		{
			if (IS_A(pNode,NodeBlend))
			{
				// will the node allow the op to happen? (Don't set op permissions - we're only asking)
				if (pNode->AllowOp(&ObjChange,FALSE))
					State.Greyed = FALSE;		// Yes! we can ungrey
				else
					Denied = TRUE;				// Oooh! we've been denied by at least one selected blend
			}
			pNode = pSel->FindNext(pNode);
		}
	}

	UINT32 IDS = 0;
	if (State.Greyed)
	{
		// If we are greyed because we've been denied, get the reason for the denial
		if (Denied)
			IDS = ObjChange.GetReasonForDenial();
		else
			IDS = _R(IDS_DETACHBLENDPATH_GREYED);
	}


	if (IDS == 0)
		IDS = _R(IDS_DETACHBLENDPATH);

	*Description = String_256(IDS);

	return State;
}

/********************************************************************************************

>	virtual void OpDetachBlendPath::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDetachBlendPath::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_DETACHBLENDPATH_UNDO));
}  


///////////////////////////////////////////////////////////////////////////////////////////
//  OpRemoveBlend
//
// Removes the selected blend objects, leaving the original objects in the tree

/********************************************************************************************

>	void OpRemoveBlend::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Returns:	-
	Purpose:	This removes all the selected blend objects, leaving the original objects in the 
				tree.
	Notes:		Phil 12/09/2005
				This routine is somewhat perverse. It makes efforts to find all the blends
				in the selection surface and below by calling BuildListOfSelectedNodes but then
				all of the blends below the selection surface don't allow themselves to be
				"removed" during the AllowOp phase.
				Note that after AllowOp has been called on inner blends the subtree is marked
				as OPPERMISSION_DENIED and is no longer found by Range::SmartFindNext!
				What a mess! (Too dangerous to change it now, though)

********************************************************************************************/

void OpRemoveBlend::Do(OpDescriptor*)
{
	SelRange* pSel = GetApplication()->FindSelection();

	BOOL ok = (pSel != NULL);

	if (ok) ok = DoStartSelOp(FALSE,FALSE);
	List BlendList;
	if (ok) ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);


	if (ok)
	{
		// Get an ObjChangeParam ready so we can tell the selected blend's parents what we hope to do 
		// to the blend (i.e. replace it with other nodes).
		ObjChangeFlags cFlags;
		cFlags.MultiReplaceNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();

		
		while (pListItem != NULL)
		{
			
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;

			// We now have a selected NodeBlend node. Here's what we do:
			//		Invalidate the region
			// 		Localise common attributes
			//		Hide the NodeBlend
			//		Move all other nodes next to the place the NodeBlend node used to be (select them too)

			NodeHidden* pNodeHidden;

			// Bodge to overcome strange behaviour of this function when blends
			// are found inside selected blend
			// (After AllowOp, OPPERMISSION_DENIED is set on the blend and then
			// DoInvalidateRegion does nothing)
			pNodeBlend->ReleaseCached(TRUE, FALSE, FALSE, TRUE);

			// Will the blend node allow us to do the op to it?
			ok = pNodeBlend->AllowOp(&ObjChange);

			if(!ok)
			{
				UINT32 IDS = ObjChange.GetReasonForDenial();

				if (IDS == _R(IDS_CANT_REMOVE_BLEND_WHEN_BEVELLED) ||
					IDS == _R(IDS_CANT_REMOVE_BLEND_WHEN_CONTOURED) )
					InformWarning(IDS);
			}

			DocRect BRect = pNodeBlend->GetBoundingRect();
			Spread* pSpread = Document::GetSelectedSpread();
			
			if (ok) ok = DoInvalidateRegion(pSpread, BRect);
			// Invalidate the whole blend region				
			if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE,FALSE);

			if (ok)	ok = pNodeBlend->RotateBlendEndObjectsBack(this);

			// Invalidate the whole blend region again
			BRect = pNodeBlend->GetBoundingRect();
			if (ok) ok = DoInvalidateRegion(pSpread, BRect);
			// Localise the attributes 
			if (ok) ok = DoLocaliseCommonAttributes(pNodeBlend);

			// Firstly, hide the blend node, and deselect it
			if (ok) ok = DoHideNode(pNodeBlend,TRUE,&pNodeHidden,FALSE);
			if (ok) pNodeBlend->SetSelected(FALSE);

			if (ok)
			{
				// Diccon 9/99 add an action so we can restore multiple blends on curves
				NodeBlender* pBlender = pNodeBlend->FindFirstBlender();
				while (pBlender != NULL)
				{
					// set the nodeblendpathindex in an undoable way
					ChangeBlenderOpParam Param;
					Param.m_NewNodeBlendPathIndex = (-1);
					Param.m_ChangeType = CHANGEBLENDER_NBPINDEX;
					ok = ChangeBlenderAction::Init(this, &UndoActions, pBlender, Param);
					//pBlender = pNodeBlendFindNextBlender(pBlender);
					pBlender = pNodeBlend->FindNextBlender(pBlender);
				}
			}

			// Move all the child ink nodes up (leave blender nodes safe under the hidden blend parent node)
			Node* pNode = pNodeBlend->FindFirstChild();
			while (pNode != NULL && ok)
			{
				Node* pNext = pNode->FindNext();

				if (pNode->IsAnObject())
				{
					NodeRenderableInk* pInkNode = (NodeRenderableInk*)pNode;
					if (pInkNode->IS_KIND_OF(NodeBlendPath))
						ok = pNodeBlend->DetachNodeBlendPath(pNodeHidden,PREV,this);						
					else if (!pInkNode->IS_KIND_OF(NodeBlender))
					{
						ok = DoMoveNode(pInkNode,pNodeHidden,PREV);	// Move next to hidden node (where blend node used to be)
						if (ok) DoSelectNode(pInkNode);				// Select it, because it's nicer that way
					}
				}
				pNode = pNext;
			}
			
			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
		}
		BlendList.DeleteAll();
	

		if (ok)
		{
			// update the effected parents after the change
			ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
			UpdateChangedNodes(&ObjChange);
		}


		if (ok) 
			pSel->Update();
		else
			FailAndExecute();

		End();
	}
}

/********************************************************************************************

>	BOOL OpRemoveBlend::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpRemoveBlend::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_REMOVEBLEND),
								CC_RUNTIME_CLASS(OpRemoveBlend), 
								OPTOKEN_REMOVEBLEND,
								OpRemoveBlend::GetState,
								0,      	/* help ID */
								_R(IDBBL_REMOVEBLEND),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpRemoveBlend::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpRemoveBlend::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// Get an ObjChangeParam ready so we can tell the selected blend's parents what we hope to do 
	// to the blend (i.e. replace it with other nodes).
	ObjChangeFlags cFlags;
	cFlags.MultiReplaceNode = TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	BOOL Denied = FALSE;
	List BlendList;
	BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	if (ok)
	{
		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		
		while (pListItem != NULL)
		{
			
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
				// will the node allow the op to happen? (Don't set op permissions - we're only asking)
			if (pNodeBlend->AllowOp(&ObjChange,FALSE))
				State.Greyed = FALSE;		// Yes! we can ungrey
			else
				Denied = TRUE;				// Oooh! we've been denied by at least one selected blend

			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
		}
	}

	BlendList.DeleteAll();



	/*
	SelRange* pSel = GetApplication()->FindSelection();

	if (pSel != NULL)
	{
		RangeControl rg = pSel->GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		pSel->Range::SetRangeControl(rg);
	
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL && State.Greyed)
		{
			if (IS_A(pNode,NodeBlend))
			{
				// will the node allow the op to happen? (Don't set op permissions - we're only asking)
				if (pNode->AllowOp(&ObjChange,FALSE))
					State.Greyed = FALSE;		// Yes! we can ungrey
				else
					Denied = TRUE;				// Oooh! we've been denied by at least one selected blend
			}
			pNode = pSel->FindNext(pNode);
		}

		rg.PromoteToParent = FALSE;
		pSel->Range::SetRangeControl(rg);	
	}
	*/
	UINT32 IDS = 0;
	if (State.Greyed)
	{
		// If we are greyed because we've been denied, get the reason for the denial
		if (Denied)
			IDS = ObjChange.GetReasonForDenial();
		else
			IDS = _R(IDS_REMOVEBLEND_GREYED);
	}


	if (IDS == 0)
		IDS = _R(IDS_REMOVEBLEND);

	*Description = String_256(IDS);

	return State;
}

/********************************************************************************************

>	virtual void OpRemoveBlend::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpRemoveBlend::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_REMOVEBLEND_UNDO));
}  



///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeBlendSteps
//
// Changes the number of steps of all the selected blends


/********************************************************************************************

>	void OpChangeBlendSteps::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	-
	Purpose:	This changes all the selected blend objects to have pOpParam->Param1 number of steps

********************************************************************************************/

void OpChangeBlendSteps::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	NodeListItem *pItem = NULL;

	BOOL ok = !NodeList.IsEmpty();

	ok = DeterminBlendObjectsProcessorHit (pOpParam, &NodeList);

	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	if (ok)
	{
		// The new number of steps is in pOpParam->Param1 of the 
		UINT32 NewNumSteps = UINT32(pOpParam->Param1);
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}

		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}

			if (pNode->IS_KIND_OF(NodeBlend))
			{
				// We now have a selected blend node so:
				//	Invalidate the node's region
				//	Store the current number of blend steps in an undo actiom
				// 	Change the number of steps to NewNumSteps
			
				// Altered by DMC to account for contoured nodes
				BOOL bBlend = FALSE;

				if (pNode->IS_KIND_OF(NodeBlend))
				{
					bBlend = TRUE;
				}



				NodeRenderableInk * pInk = (NodeRenderableInk *)pNode;

				UINT32 NumSteps = 0;
				double DistanceEntered = 0.0;
				if (bBlend)
				{				
					NumSteps = ((NodeBlend *)pNode)->GetNumBlendSteps();
					DistanceEntered = ((NodeBlend *)pNode)->GetDistanceEntered(); 
				}
				
				ChangeBlendStepsAction* pAction;

				// Ask the node if it's ok to do the op
				ObjChangeFlags cFlags;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				ok = pInk->AllowOp(&ObjChange);

				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
				if (ok) ok = ChangeBlendStepsAction::Init(this,&UndoActions,pInk,NumSteps, DistanceEntered, &pAction) != AC_FAIL;
				
		
				if (ok)
				{
					if (bBlend)
					{				
						((NodeBlend *)pNode)->SetNumBlendSteps(NewNumSteps);
						
						// DY update the distance between steps variable
						((NodeBlend *)pNode)->UpdateStepDistance();
					}
					
				}
				
				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
			}
		}
	}

	NodeList.DeleteAll();

	if (ok) 
	{
		// Inform the effected parents of the change
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);
	}
	else
		FailAndExecute();

	End();
}

BOOL OpChangeBlendSteps::DeterminBlendObjectsProcessorHit (OpParam* pOpParam, List* NodeList)
{
	NodeListItem *pItem = NULL;
	INT32 total = 0;
	
	// The new number of steps is in pOpParam->Param1 of the 
	INT32				NewNumSteps = UINT32(pOpParam->Param1);
	pItem = (NodeListItem *)NodeList->GetHead();

	OpParam				pParam( NewNumSteps, INT32(0) );

	Node* pSelNode = NULL;

	if (pItem)
	{
		pSelNode = pItem->pNode;
	}

	while (pSelNode != NULL)
	{
		Node* pNode = pSelNode;

		pItem = (NodeListItem *)NodeList->GetNext(pItem);

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}
		else
		{
			pSelNode = NULL;
		}

		if (pNode->IS_KIND_OF(NodeBlend))
		{
			// We now have a selected blend node so:
			// add in its paths

			NodeBlend* pBlend = (NodeBlend*) pNode;
			Node* start = NULL, *end = NULL;

			pBlend->GetStartAndEndNodes (&start, &end);

			if ((start != NULL) && (end != NULL))
			{
				total += pNode->EstimateNodeComplexity ( &pParam );
			}
			else
			{
				return (FALSE);
			}
		}
	}

	static BOOL foundCPU = FALSE;
	static INT32 pathsBeforeAsk = 0;

	if (!foundCPU)
	{
/*		SYSTEM_INFO systemInfo;
		GetSystemInfo (&systemInfo);

		if (systemInfo.dwProcessorType == PROCESSOR_INTEL_386)
		{
			pathsBeforeAsk = 6000;
		}
		else if (systemInfo.dwProcessorType == PROCESSOR_INTEL_486)
		{
			pathsBeforeAsk = 12000;
		}
		else if (systemInfo.dwProcessorType == PROCESSOR_INTEL_PENTIUM)
		{
			pathsBeforeAsk = 20000;
		}
		else */
		{
			// assume its faster than a pentium .... (always true now in 2005!)
			pathsBeforeAsk = 24000;
		}
		foundCPU = TRUE;
	}

	if (total > pathsBeforeAsk)
	{
		// Load and build the question text.
		String_256 QueryString(_R(IDS_ASKLONGJOB));
						
		// The only way of bringing up a box with a string in it
		Error::SetError(0, QueryString, 0);
		INT32 DlgResult = InformMessage(0, _R(IDS_YES), _R(IDS_NO));
		Error::ClearError();

		switch (DlgResult)
		{
			case 1:		// YES
				return (TRUE);
			break;
			case 2:		// NO
				return (FALSE);			// break out of this stuff!
		}
	}
	
	return (TRUE);
}

/********************************************************************************************

>	BOOL OpChangeBlendSteps::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeBlendSteps::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeBlendSteps), 
								OPTOKEN_CHANGEBLENDSTEPS,
								OpChangeBlendSteps::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeBlendSteps::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeBlendSteps::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeBlend));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();	

	// DY awful hack to allow us to call this op from the bezier tool 
	// when we wish to edit the path of a blend on a path.
	Range * pSel = GetApplication()->FindSelection();

	if (pSel)
	{
		Node* pNode = pSel->FindFirst();
		if (pNode->IS_KIND_OF(NodeBlendPath))
		{
			State.Greyed = FALSE;
		}
	}
	
	if (State.Greyed)
		*Description = String_256(_R(IDS_REMOVEBLEND_GREYED));
	else
		*Description = String_256(_R(IDS_BLENDSTEPS));

	return State;
}

/********************************************************************************************

>	virtual void OpChangeBlendSteps::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeBlendSteps::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_BLENDSTEPS_UNDO));
}  



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeBlendStepsAction class

/********************************************************************************************

>	ChangeBlendStepsAction::ChangeBlendStepsAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeBlendStepsAction::ChangeBlendStepsAction()
{
	pNodeBlend  = NULL;
	OldNumSteps = 0;
}


/********************************************************************************************

>	ActionCode ChangeBlendStepsAction::Init( 	Operation* 	pOp,
												ActionList* pActionList,
												NodeBlend* 	pThisNodeBlend,
												UINT32 		NumSteps,
												ChangeBlendStepsAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pThisNodeBlend 	= ptr to NodeBlend to change 
				NumSteps	 	= Num steps to applied to pThisNodeBlend
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ChangeBlendStepsAction::Init(Operation* pOp,
										ActionList* pActionList,
										Node* pThisNodeBlend,
										UINT32 NumSteps,
										double DistanceEntered,
										ChangeBlendStepsAction** ppNewAction)
{
	UINT32 ActSize = sizeof(ChangeBlendStepsAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeBlendStepsAction),(Action**)ppNewAction);

	if (Ac != AC_FAIL)
	{
		(*ppNewAction)->pNodeBlend  = pThisNodeBlend;
		(*ppNewAction)->OldNumSteps = NumSteps;
		(*ppNewAction)->OldDistanceEntered = DistanceEntered;
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeBlendStepsAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeBlendStepsAction::Execute()
{
	ActionCode Act;
	ChangeBlendStepsAction* pAction;

	UINT32 NumSteps = 0;
	double DistanceEntered = 0.0;

	if (pNodeBlend->IsKindOf(CC_RUNTIME_CLASS(NodeBlend)))
	{
		NumSteps = ((NodeBlend *)pNodeBlend)->GetNumBlendSteps();
		DistanceEntered = ((NodeBlend *)pNodeBlend)->GetDistanceEntered();
	}
	Act = ChangeBlendStepsAction::Init(	pOperation, 
										pOppositeActLst,
										pNodeBlend,
										NumSteps,
										DistanceEntered,
										&pAction);
	if (Act != AC_FAIL)
	{
		if (pNodeBlend->IsKindOf(CC_RUNTIME_CLASS(NodeBlend)))
		{
			((NodeBlend *)pNodeBlend)->SetNumBlendSteps(OldNumSteps);
			((NodeBlend *)pNodeBlend)->SetDistanceEntered(OldDistanceEntered);
		}
		
	}

	return Act;
}

ChangeBlendStepsAction::~ChangeBlendStepsAction()
{
}


///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeBlendDistance
//
// Changes the distance between steps of all the selected blends


/********************************************************************************************

>	void OpChangeBlendDistance::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Returns:	-
	Purpose:	To change the distance between steps in the selected blends to that 
				given in OpParam->Param1.  The method differs depending on whether or
				not the blend is on a curve.  
				If the blend is on a curve then it is necessary to recalculate the number of steps 
				that can fit on the curve with the new length. If there is not an exact fit then 
				the last object must be moved along the curve to ensure correct distance is maintained.
				If the blend is not on a curve the it is necessary to calculate the direction of
				the blend and move the last object forwards or backwards to maintain the correct 
				distance.				
********************************************************************************************/

void OpChangeBlendDistance::DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	SelRange* pSel = GetApplication()->FindSelection();

	RangeControl rg;	

	if (pSel)
	{
		rg = pSel->GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		pSel->Range::SetRangeControl(rg);
	}

	BOOL ok = (pSel != NULL);

	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	if (ok)
	{
		List BlendList;
		// The new distance is in pOpParam->Param1 
		double NewStepDistance = double(pOpParam->Param1);
		ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
		if (ok)
		{
			NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
			while (pListItem != NULL)
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
	
				// We now have a selected blend node so:
				//	 - Invalidate the node's region
				//	 - Store the current number of blend steps in an undo actiom
				// 	 - Calculate the new number of blend steps and apply
				//   - Get the nodeblender child of the blend
				//   - Calculate the proportion along the path we have to set in order to
				//     achieve the correct number of steps at the exact distance entered.
				
				NodeRenderableInk * pInk = (NodeRenderableInk *)pNodeBlend;

				// Ask the node if it's ok to do the op
				ObjChangeFlags cFlags;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				ok = pInk->AllowOp(&ObjChange);
		
				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
				
				if (ok)
				{
					
//					NodeBlend* pCopyBlend = pNodeBlend;
					BOOL OnCurve = pNodeBlend->IsOnACurve();

					if (OnCurve)
					{
						// NewNumSteps calculated in InsertChangeStepsAction and passed back
						UINT32 NewNumSteps = 0;
						// NewDistances is used to store the new distances along the path for the start and end objects
						double NewDistances[2] = { 1.0, 0.0 };

						// calculate and perform the actions
								ok = InsertChangeStepsAction(pNodeBlend, NewStepDistance, &NewNumSteps, NewDistances);
						if (ok) ok = InsertTransformNodesAction(pNodeBlend, NewDistances[0], NewDistances[1]);
						if (ok) ok = InsertChangePathProportion(pNodeBlend, NewDistances[0], NewDistances[1]);
						if (ok)		
						{
							// set the member variables in the nodeblend
							pNodeBlend->SetNumBlendSteps(NewNumSteps);
							pNodeBlend->UpdateStepDistance();
							pNodeBlend->SetDistanceEntered(NewStepDistance);
						}
					}
					else
					{
						ok = InsertChangeLinearBlendActions(pNodeBlend,this, NewStepDistance);
					}
					pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
				
				} // end if ok
				
				if (ok) ok = DoInvalidateNodeRegion(pInk,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pInk,TRUE) != AC_FAIL);
			}
		}
		BlendList.DeleteAll();
	}

	rg.PromoteToParent = FALSE;
	pSel->Range::SetRangeControl(rg);

	if (ok) 
	{
		pSel->Update();

		// Inform the effected parents of the change
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);
	}
	else
		FailAndExecute();

	End();

}


/********************************************************************************************

>	BOOL OpChangeBlendDistance::InsertChangeStepsAction(NodeBlend* pNodeBlend,
														Operation* pOp,
														double StepDistance,
														UINT32* NewNumSteps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/99
	Inputs:     pNodeBlend   - the nodeblend which needs to have its number of steps changed
				pOp          - the Op which wants to use this action
				StepDistance - the new distance between steps of the blend
	Outputs:    NewNumSteps	 - the number of steps calculated
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	When the distance between blend steps is changed and the blend is on a curve it 
				is necessary to recalculate how many blend steps we can now fit on the curve.
				This function performs that operation and inserts a new action.

********************************************************************************************/

BOOL OpChangeBlendDistance::InsertChangeStepsAction(NodeBlend* pNodeBlend,double StepDistance, 
													UINT32* NewNumSteps, double* NewDistances)
{
	UINT32 OldNumSteps = 0;
	double BlendDistance = 0.0;
	
	OldNumSteps = pNodeBlend->GetNumBlendSteps();
	UINT32 TempNumSteps = OldNumSteps;
	BOOL ok = pNodeBlend->GetBlendDistance(TRUE, &BlendDistance);
	
	
	double StartProportion = 0.0;
	double EndProportion = 0.0;
	double StartDistance = 0.0;
	double EndDistance = 0.0; 

	if (ok)
	{
		// find out if the user has edited the end position
		switch (pNodeBlend->GetLastEdited())
		{
			case NONE:
				TempNumSteps = (UINT32)(BlendDistance / StepDistance);	
				EndDistance = BlendDistance - (TempNumSteps * StepDistance);
				StartDistance = 0.0;
			
			break;
			case FIRST:
				ok = pNodeBlend->GetStartAndEndProportions(&StartProportion, &EndProportion);
				if (ok)
				{
					StartDistance = StartProportion * BlendDistance;
					EndDistance = BlendDistance - (EndProportion * BlendDistance);
					ok = CalculateNewNumStepsAndPosition(OldNumSteps, BlendDistance, StepDistance,
														 &StartDistance, &EndDistance, &TempNumSteps);
				}
			
			break;
			case LAST:
			
				StartProportion = 0.0;
				EndProportion = 0.0;

				ok = pNodeBlend->GetStartAndEndProportions(&StartProportion, &EndProportion);
				if (ok)
				{
					StartDistance = StartProportion * BlendDistance;
					EndDistance = BlendDistance - (EndProportion * BlendDistance);
					ok = CalculateNewNumStepsAndPosition(OldNumSteps, BlendDistance, StepDistance,
														 &EndDistance, &StartDistance, &TempNumSteps);
				}
			break;
			default:
				break;
			
		
		}
		if ((TempNumSteps != OldNumSteps) && ok)
		{
			double DistanceEntered = pNodeBlend->GetDistanceEntered();
			ChangeBlendStepsAction* pStepAction;
			NodeRenderableInk * pInk = (NodeRenderableInk *)pNodeBlend;		
			ok = ChangeBlendStepsAction::Init(this,&UndoActions,pInk,OldNumSteps,DistanceEntered, &pStepAction) != AC_FAIL;
		}
	}

	if (ok)
	{
		*NewNumSteps = TempNumSteps;
		NewDistances[0] = StartDistance;
		NewDistances[1] = EndDistance;
	}
	else
		*NewNumSteps = OldNumSteps;   // restore the original number
									  // if something went wrong.

	return ok;
}



/********************************************************************************************

>	BOOL OpChangeBlendDistance::InsertChangePathProportion(NodeBlend* pNodeBlend,
														Operation* pOp,
														UINT32* NewNumSteps)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/99
	Inputs:     pNodeBlend   - the nodeblend which needs to have its number of steps changed
				pOp          - the Op which wants to use this action
				StepDistance - the distance between steps of the blend
			    NewNumSteps	 - the number of steps calculated
	outputs:    -
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	To work out the proportion of the nodeblendpath that we need to use to set
				the given number of steps and step distance.  Once this is known we find the 
				point on the path corresponding to that proportion and transform the end node
				to that point
********************************************************************************************/

BOOL OpChangeBlendDistance::InsertChangePathProportion(NodeBlend* pNodeBlend, 
													   double StartDistance, double EndDistance)

{
	if (pNodeBlend == NULL)
	{
		ERROR3("NodeBlend is NULL");
		return FALSE;
	}

	NodeBlender* pNodeBlender = pNodeBlend->FindFirstBlender();

	if (pNodeBlender == NULL)
	{
		ERROR3("This blend has no nodeblenders");
		return FALSE;
	}

	// this will require changing when we implement blending from a 
	// blend on a curve to other objects
	double BlendDistance = 0.0;
	BOOL ok = pNodeBlend->GetBlendDistance(TRUE, &BlendDistance);

	if (ok)
	{
		double OldStartProp = 0.0;
		double OldEndProp   = 0.0;

		ok = pNodeBlend->GetStartAndEndProportions(&OldStartProp, &OldEndProp);
		if (ok)
		{
			// check to see if the proportions have changed
			double OldStartDist = BlendDistance * OldStartProp;
			if (OldStartDist != StartDistance)
			{
				double NewPathProportion = StartDistance / BlendDistance;
				ChangeBlenderOpParam BlenderParam;
				BlenderParam.m_ChangeType = CHANGEBLENDER_PATHSTART;
				BlenderParam.m_NewPathStart = NewPathProportion;
				ok = ChangeBlenderAction::Init(this, &UndoActions, pNodeBlender, BlenderParam);
			}

			double OldEndDist = BlendDistance - (BlendDistance * OldEndProp);
			
			if (OldEndDist != EndDistance)
			{
				double NewPathProportion = 1 - (EndDistance / BlendDistance);
				ChangeBlenderOpParam BlenderParam;
				BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
				BlenderParam.m_NewPathEnd = NewPathProportion;
				ok = ChangeBlenderAction::Init(this, &UndoActions, pNodeBlender, BlenderParam);
			}
		}
	}
	return ok;
}

/*
	double BlendDistance = 0.0;
	BOOL ValidDistance = pNodeBlend->GetBlendDistance(TRUE, &BlendDistance);
	if (ValidDistance = FALSE)
		return FALSE;
	// work out the proportion of the path in use
	double PathDistanceUsed = NumSteps * StepDistance;
	double PathProportion = PathDistanceUsed / BlendDistance;
	double PathDistanceUnused = BlendDistance - PathDistanceUsed;

	BOOL ok = FALSE;
	if (PathProportion != 1.0)
	{		
		//  We may have to deal with multiple blenders in which case
		// we only want to transform the last object of the final blender
		
		NodeBlender* pNodeBlender = pNodeBlend->FindFirstBlender();
		INT32 NumBlenders = pNodeBlend->GetNumBlenders();
						
		while (pNodeBlender != NULL)
		{
			NumBlenders--;
			if (NumBlenders ==0)
			{	
				ChangeBlenderOpParam BlenderParam;
				BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
				BlenderParam.m_NewPathEnd = PathProportion;
				ok = ChangeBlenderAction::Init(this, &UndoActions, pNodeBlender, BlenderParam);
				if (ok)
				{
					DocCoord NewPoint;
					double ExtraParam = 0.0;  //passed to the function but not used afterwards
					ok = pNodeBlender->GetPointOnNodeBlendPath(1.0,&NewPoint,&ExtraParam);

					if (ok)
					{
						NodeRenderableInk* pEnd = pNodeBlender->GetNodeEnd();
						ok = ((pEnd != NULL) && (pNodeBlend != NULL));
						if (ok) 
							ok = pNodeBlend->TransformNodeToPoint(pEnd,&NewPoint,this,ExtraParam);
					}
				}
			}	
			pNodeBlender = pNodeBlend->FindNextBlender(pNodeBlender);
		}

	} // end if (pathproportion

	return ok;

*/


/********************************************************************************************

>	BOOL OpChangeBlendDistance::InsertTransformNodesAction(NodeBlend* pNodeBlend, double StartDistance
															   double EndDistance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		NodeBlend to perform the actions on
				StartDistance - distance along the path for the start object
				EndDistance - distance along the path for the end object
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Determines the point along the nodeblendpath to which the end objects must be
				moved, then creates the actions

********************************************************************************************/

BOOL OpChangeBlendDistance::InsertTransformNodesAction(NodeBlend* pNodeBlend, double StartDistance,
													   double EndDistance)
{
	if (pNodeBlend == NULL)
	{
		ERROR3("NodeBlend is NULL");
		return FALSE;
	}

	NodeBlender* pNodeBlender = pNodeBlend->FindFirstBlender();

	if (pNodeBlender == NULL)
	{
		ERROR3("This blend has no nodeblenders");
		return FALSE;
	}

	// this will require changing when we implement blending from a 
	// blend on a curve to other objects
	double BlendDistance = 0.0;
	BOOL ok = pNodeBlend->GetBlendDistance(TRUE, &BlendDistance);

	if (ok)
	{
		double OldStartProp = 0.0;
		double OldEndProp   = 0.0;

		ok = pNodeBlend->GetStartAndEndProportions(&OldStartProp, &OldEndProp);
		if (ok)
		{
			// check to see if the proportions have changed
			double OldStartDist = BlendDistance * OldStartProp;
			if (OldStartDist != StartDistance)
			{
				// locate the point on the line corresponding to the new distance
				DocCoord NewPoint;
				double ExtraParam = 0.0;  //passed to the function but not used afterwards
				
				ok = pNodeBlender->GetPointFromDistance(StartDistance, &NewPoint, &ExtraParam);
				if (ok)
				{
					NodeRenderableInk* pEnd = pNodeBlender->GetNodeStart();
					ok = ((pEnd != NULL) && (pNodeBlend != NULL));
					if (ok) 
						ok = pNodeBlend->TransformNodeToPoint(pEnd,&NewPoint,this,ExtraParam);
				}
			}

			double OldEndDist = BlendDistance - (BlendDistance * OldEndProp);
			
			if (OldEndDist != EndDistance)
			{
				// locate the point on the line corresponding to the new distance
				DocCoord NewPoint;
				double ExtraParam = 0.0;  //passed to the function but not used afterwards
				double DistanceFromStart = BlendDistance - EndDistance;
				pNodeBlender = pNodeBlend->FindLastBlender();
				if (pNodeBlender != NULL)
				{
					ok = pNodeBlender->GetPointFromDistance(DistanceFromStart, &NewPoint, &ExtraParam);

					if (ok)
					{
						NodeRenderableInk* pEnd = pNodeBlender->GetNodeEnd();
						ok = ((pEnd != NULL) && (pNodeBlend != NULL));
						if (ok) 
							ok = pNodeBlend->TransformNodeToPoint(pEnd,&NewPoint,this,ExtraParam);
					}
				}
			}
		}
	}
	return ok;
}	





/********************************************************************************************

>	BOOL OpChangeBlendDistance::InsertChangeLinearBlendActions(NodeBlend* pNodeBlend, Operation* pOp
															   double StepDistance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		NodeBlend to perform the actions on
				Operation performing the actions
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Calculates the actions necessary to change a linear blend when the 
				distance between blend steps is edited.  This involves calculating the
				new length of the blend, transforming the last blend object accordingly
				and telling each blender to regenerate itself.

********************************************************************************************/

BOOL OpChangeBlendDistance::InsertChangeLinearBlendActions(NodeBlend* pNodeBlend, 
														   Operation* pOp,
														   double StepDistance)
{
	if (pNodeBlend->IsOnACurve())
	{
		ERROR3("Trying to perform linear blend actions to blend on a curve");
		return FALSE;
	}

	NodeBlender* pBlender = pNodeBlend->FindFirstBlender();
	const UINT32 NumSteps = pNodeBlend->GetNumBlendSteps();
	BOOL ok = TRUE;
	while (pBlender != NULL)
	{
		double OldDistance = pBlender->GetLinearDistance();
		double NewDistance = StepDistance * NumSteps;
		double DistanceRatio = NewDistance / OldDistance;
		DocCoord Start;
		DocCoord End;
	
		 ok = pBlender->GetBlendObjectCentres(&Start, &End);
		if (ok)
		{
			// bit of a hack this, when it comes to undoing we need to have the blender
			// regenerate itself AFTER it transforms the end node. So when first performing
			// this action it doesn't really do anything, but it has to be there for the Undo

			ChangeBlenderOpParam BlenderParam;
			BlenderParam.m_ChangeType = CHANGEBLENDER_REGEN;
			ok = ChangeBlenderAction::Init(pOp, &UndoActions, pBlender, BlenderParam);
			
			DocCoord NewPosition = DocCoord::PositionPointFromRatio(Start, End, DistanceRatio);
			
			// we need to cast to noderenderable ink in order to use in the
			// transform function
			NodeRenderableInk* pEnd = (NodeRenderableInk*)(pBlender->GetNodeEnd());

			if (pEnd != NULL)
			{
				UndoableOperation* pUndoOp = (UndoableOperation*)pOp;
				ok = pNodeBlend->TransformNodeToPoint(pEnd, &NewPosition, pUndoOp, 0.0);
				
				// tell the blender that it needs to recalculate itself
				pBlender->SetUninitialised();  
											
			}
		}
		pBlender = pNodeBlend->FindNextBlender(pBlender);
	}
	
	return ok;

}
		

/********************************************************************************************

>	BOOL OpChangeBlendDistance::CalculateNewNumStepsAndPosition(UINT32 OldNumSteps, 
																double BlendDistance,
																double StepDistance
																double* FixedPosition
																double* MoveablePosition
																UINT32* NewNumSteps)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		OldNumSteps - the number of steps in the blend before any changes were applied
				BlendDistance - the length of the NodeBlendPath
				StepDistance  - the distance between steps that we wish to apply.
				FixedDistance - the distance from the end of the path of the object that we would
				like to keep fixed.  Note that it may not always remain fixed.
				MoveableDistance - the distance from the end of the path of the object that we are
				allowing to move.
				NOTE: FixedDistance and MoveableDistance should be measured from the end that they 
				represent.  That is to say if you wish to keep the first blend object fixed then FixedDistance
				should be the distance from the start of the path to the first blend object, MoveableDistance will
				then be the distance FROM THE OTHER END to the end object.

	Outputs:    NewNumSteps - the new number of steps required
				MoveableDistance - the new position of the end that we don't care about
				FixedDistance - in some rare instances we may have adjust the fixed position
				in order to accomodate the desired step distance
				is the last object.			
	returns:	TRUE if all went well, FALSE if the step distance gives and invalid number of steps.
	Purpose:	Works out the number of steps needed to accomodate the new step distance, as well as 
				working out the new positions of the end objects.
				
	See Also:	
***********************************************************************************************/
	
BOOL OpChangeBlendDistance::CalculateNewNumStepsAndPosition(UINT32 OldNumSteps, double BlendDistance,
														   double StepDistance, double* FixedDistance,
														   double* MoveableDistance, UINT32* NewNumSteps)
{
	// First check parameters
	if (OldNumSteps < 0 || StepDistance < 0)
	{
		ERROR3("Invalid parameter");
		return FALSE;
	}
	if (BlendDistance < (*FixedDistance + *MoveableDistance))
	{
		ERROR3("Invalid distance parameter");
		return FALSE;
	}

	//initialise locals to zero 
	*NewNumSteps = 0;

	// get the distance currently occupied by the blend
	double DistanceUsed = BlendDistance - (*FixedDistance + *MoveableDistance);

	// ideally we'd like to blend distance to be as close as possible to 
	// what it was before editing
	UINT32 TempNumSteps = (UINT32)(DistanceUsed / StepDistance);
	
	// if this gives us a positive number of steps then we'll take it
	if (TempNumSteps > 0)
	{
		double NewDistance = TempNumSteps * StepDistance;
		*MoveableDistance = BlendDistance - (*FixedDistance + NewDistance);
		*NewNumSteps = TempNumSteps;
		//NewFixedDistance = *FixedDistance;
	}
	// if not then we'll try to use the whole length of the blend
	else
	{
		TempNumSteps = (UINT32)(BlendDistance / StepDistance);
		if (TempNumSteps > 0)
		{
			double NewDistance = TempNumSteps * StepDistance;
			*MoveableDistance = BlendDistance -  NewDistance;
			*FixedDistance = 0.0;
			*NewNumSteps = TempNumSteps;
		}
		else
		{
			// we've been passed a step distance that is too long, this should never happen due to the
			// checks in the UI
			ERROR3("Step distance is too long");
			return FALSE;
		}
	}



	// test to see that what we have is ok.
	if (*NewNumSteps != 0)
		return TRUE;
	else
		return FALSE;
}



/********************************************************************************************

>	BOOL OpChangeBlendDistance::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeBlendDistance::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeBlendDistance), 
								OPTOKEN_CHANGEBLENDDISTANCE,
								OpChangeBlendDistance::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeBlendDistance::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeBlendDistance::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	SelRange* pSel = GetApplication()->FindSelection();

	RangeControl rg = pSel->GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	pSel->Range::SetRangeControl(rg);
	if (pSel != NULL)
	{
		Node* pNode = pSel->FindFirst();
		while (pNode != NULL && State.Greyed)
		{
			State.Greyed = !(pNode->IS_KIND_OF(NodeBlend));
			pNode = pSel->FindNext(pNode);
		}
	}
	// if that fails then try this, which will catch any blends that are
	// inside bevels or contours - DY
	if (State.Greyed)
	{
		List BlendList;
		BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
		State.Greyed = !ok;
		BlendList.DeleteAll();
	}

	// DY awful hack to allow us to call this op from the bezier tool 
	// when we wish to edit the path of a blend on a path.
	Node* pNode = pSel->FindFirst();
	if (pNode->IS_KIND_OF(NodeBlendPath))
	{
		State.Greyed = FALSE;
	}

	rg.PromoteToParent = FALSE;
	pSel->Range::SetRangeControl(rg);


	if (State.Greyed)
		*Description = String_256(_R(IDS_REMOVEBLEND_GREYED));
	else
		*Description = String_256(_R(IDS_BLENDDISTANCEEDITVALUE));
	return State;
}

/********************************************************************************************

>	virtual void OpChangeBlendDistance::GetOpName(String_256* OpName) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeBlendDistance::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_STEPDISTANCEUNDO));
}  




//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The RemapBlendAction class

/********************************************************************************************

>	RemapBlendAction::RemapBlendAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

RemapBlendAction::RemapBlendAction()
{
	pNodeBlend 		= NULL;
	RemapRef		= 0;
}


/********************************************************************************************

>	ActionCode RemapBlendAction::Init( 	Operation* 	pOp,
										ActionList* pActionList,
										NodeBlend* pNodeBlend,
										UINT32 RemapRef,
										INT32 IndexStart,
										INT32 IndexEnd,
										RemapBlendAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		pOp  		= ptr to the operation to which this action belongs
				pActionList = ptr to action list to which this action should be added
				pNodeBlend 	= ptr to NodeBlend to change 
				RemapRef	= remap ref value to pass to pNodeBlend->Remap()
				IndexStart	= mapping to apply to the start node
				IndexEnd	= mapping to apply to the end  node
	Outputs:	ppNewAction = ptr to a ptr to an action, allowing the function to return
							  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually performs the remapping of pNodeBlend, using pNodeBlend->Remap().
				This is so that it will only function if it successfully creates the action.
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode RemapBlendAction::Init(	Operation* pOp,
									ActionList* pActionList,
									NodeBlend* 	pNodeBlend,
									UINT32 RemapRef,
									DocCoord PosStart,
									DocCoord PosEnd,
									RemapBlendAction** ppNewAction)
{
	UINT32 ActSize = sizeof(RemapBlendAction);

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(RemapBlendAction),(Action**)ppNewAction);

	if (Ac != AC_FAIL)
	{
		DocCoord InvPosStart,InvPosEnd;

		if (pNodeBlend->Remap(RemapRef,PosStart,PosEnd,&InvPosStart,&InvPosEnd))
		{
			(*ppNewAction)->pNodeBlend  	= pNodeBlend;
			(*ppNewAction)->RemapRef		= RemapRef;
			(*ppNewAction)->InvPosStart 	= InvPosStart;
			(*ppNewAction)->InvPosEnd   	= InvPosEnd;
		}
		else
		{
			ERROR3("pNodeBlend->Remap() failed");
			Ac = AC_FAIL;
		}
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode RemapBlendAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
				This remaps the blend node using the inverse mapping indexes it was initialised with,
				creating another RemapBlendAction to invert this mapping
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode RemapBlendAction::Execute()
{
	ActionCode Act;
	RemapBlendAction* pAction;
	Act = RemapBlendAction::Init(	pOperation, 
									pOppositeActLst,
									pNodeBlend,
									RemapRef,
									InvPosStart,
									InvPosEnd,
									&pAction);

	return Act;
}

RemapBlendAction::~RemapBlendAction()
{
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The InitBlendersAction class

/********************************************************************************************

>	InitBlendersAction::InitBlendersAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

InitBlendersAction::InitBlendersAction()
{
}


/********************************************************************************************

>	ActionCode InitBlendersAction::Init( 	Operation* 	pOp,
										ActionList* pActionList,
										List* pBlenderInfoList,
										BOOL Deinit,
										InitBlendersAction** 	ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to action list to which this action should be added
				pBlenderInfoList= ptr to a NodeBlend
				Deinit			= TRUE if deinit blend, else reinit
	Outputs:	ppNewAction 	= ptr to a ptr to an action, allowing the function to return
							  	a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				If Deinit == TRUE, then undo will reinit the blenders and redo will deinit them again.
				The reverse happens if Deinit == FALSE;
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode InitBlendersAction::Init(	OpBlendNodes* pOp,
										ActionList* pActionList,
										List* pBlenderInfoList,
										BOOL DeinitState,
										InitBlendersAction** ppNewAction)
{
	UINT32 ActSize = sizeof(InitBlendersAction)+(pBlenderInfoList->GetCount()*sizeof(BlenderInfoItem));

	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(InitBlendersAction),(Action**)ppNewAction);
	InitBlendersAction* pAction = *ppNewAction;

	if (Ac != AC_FAIL)
	{
		pAction->Deinit = DeinitState;

		// Copy items from the given list to the new action's list
		BlenderInfoItem *pItem = (BlenderInfoItem*)pBlenderInfoList->GetHead();
		while (pItem != NULL && Ac != AC_FAIL)
		{
			BlenderInfoItem* pNewItem = pItem->SimpleCopy();
			if (pNewItem != NULL)
			{
				pAction->BlenderInfoList.AddTail(pNewItem);
				pItem = (BlenderInfoItem*)pBlenderInfoList->GetNext(pItem);
			}
			else
				Ac = AC_FAIL;
		}
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode InitBlendersAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
				This remaps the blend node using the inverse mapping indexes it was initialised with,
				creating another InitBlendersAction to invert this mapping
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode InitBlendersAction::Execute()
{
	ActionCode Ac;
	InitBlendersAction* pAction;
	OpBlendNodes* pOp = (OpBlendNodes*)pOperation;

	// Negate the type of blender initialisation
	Deinit = !Deinit;

	Ac = InitBlendersAction::Init(	pOp,
									pOppositeActLst,
									&BlenderInfoList,
									Deinit,
									&pAction);

	if (Ac != AC_FAIL)
	{
		if (Deinit)
			pOp->DeinitBlenders(BlenderInfoList);
		else
		{
			if (!(pOp->ReinitBlenders(BlenderInfoList)))
				Ac = AC_FAIL;
		}
	}

	return Ac;
}

InitBlendersAction::~InitBlendersAction()
{
	BlenderInfoList.DeleteAll();
}

///////////////////////////////////////////////////////////////////////////////////////////
//  OpChangeBlend
//
// Changes one of the flags of all the selected blends


/********************************************************************************************

>	void OpChangeBlend::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Returns:	-
	Purpose:	This changes the flag specified in FlagType (given in pOpParam->Param1) in all the 
				selected blend objects to have the same as state of pOpParam->Param2.

********************************************************************************************/

void OpChangeBlend::DoWithParam(OpDescriptor*,OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL,"NULL OpParam ptr");
	if (pOpParam == NULL) return;

	ChangeBlendOpParam* pChangeParam = (ChangeBlendOpParam*)pOpParam;

	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	BOOL ok = !NodeList.IsEmpty();
	if (ok) ok = DoStartSelOp(FALSE,FALSE);

	NodeListItem * pItem = NULL;

	if (ok)
	{
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}

		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}

			if (pNode->IS_KIND_OF(NodeBlend))
			{
				// We now have a selected blend node so:
				//	Invalidate the node's region
				//	Store the current state of blend flag in an undo actiom
				// 	Change the flag to the setting in Param2

				ChangeBlendAction* pAction;
				NodeBlend* pNodeBlend = (NodeBlend*)pNode;

				// Ask the node if it's ok to do the op
				ObjChangeFlags cFlags;
				// Ilan 7/5/00
				// Ensure AllowOp passes messages on to children in compound (so geom linked attrs informed)
				cFlags.TransformNode = TRUE;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				ok = pNodeBlend->AllowOp(&ObjChange);

				// invalidate the blend's region
				if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE,FALSE);
				if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pNodeBlend,TRUE) != AC_FAIL);

				// change the blend in an undoable way
				if (ok) ok = ChangeBlendAction::Init(this,&UndoActions,pNodeBlend,pChangeParam,&pAction) != AC_FAIL;

				if (ok && pChangeParam->ChangeType == CHANGEBLEND_TANGENTIAL)
				{
					ok = pNodeBlend->TransformBlendEndObjects(this);
					// invalidate the blend's region again after transfrom
					if (ok) ok = DoInvalidateNodeRegion(pNodeBlend,TRUE,FALSE);
					if (ok)	ok = (InvalidateBoundsAction::Init(this,&UndoActions,pNodeBlend,TRUE) != AC_FAIL);
				}
			}
			
		}
	}

	// de-allocate the contents of NodeList.
	NodeList.DeleteAll();

	if (ok) 
	{
		// Inform the effected parents of the change
		ObjChangeFlags cFlags;
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,this);
		UpdateChangedNodes(&ObjChange);

		// Note the way the selected blends were changed
		ChangeType = pChangeParam->ChangeType;
	}
	else
		FailAndExecute();

	End();
}

/********************************************************************************************

>	BOOL OpChangeBlend::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpChangeBlend::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpChangeBlend), 
								OPTOKEN_CHANGEBLEND,
								OpChangeBlend::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpChangeBlend::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpChangeBlend::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE); // It's not ticked, but it is greyed by default
	
	// DMc - to test for bevels & contours
	// are there any contour nodes in the selection
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeBlend));

	if (!NodeList.IsEmpty())
	{
		State.Greyed = FALSE;
	}

	NodeList.DeleteAll();	

	// DY awful hack to allow us to call this op from the bezier tool 
	// when we wish to edit the path of a blend on a path.
	Range * pSel = GetApplication()->FindSelection();

	if (pSel)
	{
		Node* pNode = pSel->FindFirst();
		if (pNode->IS_KIND_OF(NodeBlendPath))
		{
			State.Greyed = FALSE;
		}
	}

	if (State.Greyed)
		*Description = String_256(_R(IDS_REMOVEBLEND_GREYED));
	else
		*Description = String_256(_R(IDS_BLENDSTEPS));

	return State;

}

/********************************************************************************************

>	virtual void OpChangeBlend::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpChangeBlend::GetOpName(String_256* OpName) 
{ 
	UINT32 IDS = _R(IDS_MARKN_EMPTY);

	switch (ChangeType)
	{
		case CHANGEBLEND_ONETOONE:			IDS = _R(IDS_ONETOONE_UNDO);  		break;
		case CHANGEBLEND_ANTIALIAS:			IDS = _R(IDS_BLENDANTIALIAS_UNDO); 	break;
		case CHANGEBLEND_COLOURBLENDTYPE:	IDS = _R(IDS_COLOURBLENDTYPE_UNDO);	break;
		case CHANGEBLEND_TANGENTIAL:		IDS = _R(IDS_TANGENTIAL_UNDO);  	break;
		case CHANGEBLEND_OBJECTPROFILE:		IDS = _R(IDS_POSITIONPROFILE_UNDO);	break;
		case CHANGEBLEND_ATTRPROFILE:		IDS = _R(IDS_ATTRPROFILE_UNDO);		break;
		default: ERROR3_PF(("Unknown flag type (%d)",ChangeType));  break;
	}

	*OpName = String_256(IDS);
}  


//-------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////
//  OpBlendOneToOne
//
// Changes the '1 to 1' state of all the selected blends


/********************************************************************************************

>	void OpBlendOneToOne::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/94
	Returns:	-
	Purpose:	Changes the 1 to 1 flag of the selected blend by toggling the flag.

********************************************************************************************/

void OpBlendOneToOne::Do(OpDescriptor* pOpDesc)
{
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL)
		{
			if (IS_A(pSelNode,NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pSelNode;

				ChangeBlendOpParam Param;
				Param.ChangeType  = CHANGEBLEND_ONETOONE;
				Param.NewOneToOne = !pNodeBlend->IsOneToOne();
				DoWithParam(pOpDesc,&Param);

				// Only call DoWithParam() once as this func acts upon the whole selection.
				// We only need to scan the selection here in order to find the first selected blend
				// node which is used to determine the state of the NewOneToOne flag
				return;
			}
			pSelNode = pSel->FindNext(pSelNode);
		}
	}
}

/********************************************************************************************

>	BOOL OpBlendOneToOne::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpBlendOneToOne::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDBBL_ONETOONE),
								//_R(IDS_ONETOONE),
								CC_RUNTIME_CLASS(OpBlendOneToOne), 
								OPTOKEN_BLENDONETOONE,
								OpBlendOneToOne::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpBlendOneToOne::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpBlendOneToOne::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE);	// not ticked & greyed by default

	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL && !State.Ticked)
		{
			if (IS_A(pSelNode,NodeBlend))
			{
				State.Ticked = ((NodeBlend*)pSelNode)->IsOneToOne();
				State.Greyed = FALSE;

				// We only need to scan the selection here in order to find the first selected blend
				// node which is used to determine the state of the Ticked flag
				return State;
			}
			pSelNode = pSel->FindNext(pSelNode);
		}
	}

	return State;
}

//-------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////
//  OpBlendAntialias
//
// Changes the antialias state of all the selected blends


/********************************************************************************************

>	void OpBlendAntialias::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/94
	Returns:	-
	Purpose:	Changes the antialias flag of the selected blend by toggling the flag.

********************************************************************************************/

void OpBlendAntialias::Do(OpDescriptor* pOpDesc)
{
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL)
		{
			if (IS_A(pSelNode,NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pSelNode;

				ChangeBlendOpParam Param;
				Param.ChangeType   = CHANGEBLEND_ANTIALIAS;
				Param.NewAntiAlias = pNodeBlend->IsNotAntialiased();
				DoWithParam(pOpDesc,&Param);

				// Only call DoWithParam() once as this func acts upon the whole selection.
				// We only need to scan the selection here in order to find the first selected blend
				// node which is used to determine the state of the NewAntialias flag
				return;
			}
			pSelNode = pSel->FindNext(pSelNode);
		}
	}
}

/********************************************************************************************

>	BOOL OpBlendAntialias::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpBlendAntialias::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDBBL_BLENDANTIALIAS),
								//_R(IDS_BLENDANTIALIAS),
								CC_RUNTIME_CLASS(OpBlendAntialias), 
								OPTOKEN_BLENDANTIALIAS,
								OpBlendAntialias::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpBlendAntialias::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpBlendAntialias::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE);	// not ticked & greyed by default

	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL && !State.Ticked)
		{
			if (IS_A(pSelNode,NodeBlend))
			{
				State.Ticked = !(((NodeBlend*)pSelNode)->IsNotAntialiased());
				State.Greyed = FALSE;

				// We only need to scan the selection here in order to find the first selected blend
				// node which is used to determine the state of the Ticked flag
				return State;
			}
			pSelNode = pSel->FindNext(pSelNode);
		}
	}

	return State;
}

//-------------------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////////////////
//  OpBlendTangential
//
// Changes the Tangential state of all the selected blends


/********************************************************************************************

>	void OpBlendTangential::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/99
	Returns:	-
	Purpose:	Changes the Tangential flag of the selected blend by toggling the flag.

********************************************************************************************/

void OpBlendTangential::Do(OpDescriptor* pOpDesc)
{
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL)
		{
			if (IS_A(pSelNode,NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pSelNode;

				ChangeBlendOpParam Param;
				Param.ChangeType   = CHANGEBLEND_TANGENTIAL;
				Param.NewTangential = !pNodeBlend->IsTangential();
				DoWithParam(pOpDesc,&Param);

				// Only call DoWithParam() once as this func acts upon the whole selection.
				// We only need to scan the selection here in order to find the first selected blend
				// node which is used to determine the state of the NewTangential flag
				return;
			}
			pSelNode = pSel->FindNext(pSelNode);
		}
	}
}

/********************************************************************************************

>	BOOL OpBlendTangential::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpBlendTangential::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDBBL_TANGENTIAL),
								CC_RUNTIME_CLASS(OpBlendTangential), 
								OPTOKEN_BLENDTANGENTIAL,
								OpBlendTangential::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpBlendTangential::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpBlendTangential::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE);	// not ticked & greyed by default

	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel != NULL)
	{
		Node* pSelNode = pSel->FindFirst();
		while (pSelNode != NULL && !State.Ticked)
		{
			if (IS_A(pSelNode,NodeBlend))
			{
				NodeBlend* pNodeBlend = (NodeBlend*)pSelNode;

				if (!State.Ticked)
					State.Ticked = pNodeBlend->IsTangential();

				if (State.Greyed)
					State.Greyed = (pNodeBlend->GetNodeBlendPath(0) != NULL);
			}
			pSelNode = pSel->FindNext(pSelNode);
		}
	}

	return State;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeBlendAction class

/********************************************************************************************

>	ChangeBlendAction::ChangeBlendAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for the action
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ChangeBlendAction::ChangeBlendAction()
{
	pNodeBlend 	= NULL;
}


/********************************************************************************************

>	ActionCode ChangeBlendAction::Init( 	Operation* 		pOp,
											ActionList* 	pActionList,
											NodeRenderableInk * pNodeBlend,
											ChangeBlendOpParam	*pChangeParam,
											ChangeBlendAction** ppNewAction);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	=  ptr to action list to which this action should be added
				pNodeBlend 		= ptr to NodeBlend to change 
				pChangeParam	= ptr to class that details how the blend should be changed.
	Outputs:	ppNewAction  	= ptr to a ptr to an action, allowing the function to return
								  a pointer to the created action
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blend node in a way specified in pChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ChangeBlendAction::Init(	Operation* 			pOp,
									ActionList* 		pActionList,
									NodeRenderableInk* 	pNodeBlend,
									ChangeBlendOpParam*	pChangeParam,
									ChangeBlendAction** ppNewAction)
{
	ERROR2IF(pNodeBlend   == NULL,AC_FAIL,"pNodeBlend is NULL");
	ERROR2IF(pChangeParam == NULL,AC_FAIL,"pChangeParam is NULL");

	UINT32 ActSize = sizeof(ChangeBlendAction);

	ChangeBlendAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeBlendAction),(Action**)&pNewAction);

	*ppNewAction = pNewAction;

	BOOL bNodeIsContour = FALSE;

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		if (!bNodeIsContour)
		{
			ChangeBlendType ChangeType = pChangeParam->ChangeType;
			
			pNewAction->pNodeBlend  		   = pNodeBlend;
			pNewAction->ChangeParam.ChangeType = ChangeType;
			pNewAction->ChangeParam.SetOldValues(pNodeBlend);
			
			switch (ChangeType)
			{
			case CHANGEBLEND_ONETOONE:			((NodeBlend *)pNodeBlend)->SetOneToOne(pChangeParam->NewOneToOne); 			break;
			case CHANGEBLEND_ANTIALIAS: 		((NodeBlend *)pNodeBlend)->SetNotAntialiased(!pChangeParam->NewAntiAlias); 	break;
			case CHANGEBLEND_COLOURBLENDTYPE:	((NodeBlend *)pNodeBlend)->SetColourBlendType(pChangeParam->NewColBlendType); 	break;
			case CHANGEBLEND_EDITEND:			((NodeBlend *)pNodeBlend)->SetLastEdited(pChangeParam->NewEndObject);		break;
			case CHANGEBLEND_TANGENTIAL:
			{
				// Only change the tangential flag of blends on a curve
				if (((NodeBlend *)pNodeBlend)->GetNodeBlendPath(0) != NULL)
					((NodeBlend *)pNodeBlend)->SetTangential(pChangeParam->NewTangential); 		
			}
			break;

			// Diccon Yamanaka 9/99 added profile code
			case CHANGEBLEND_OBJECTPROFILE:  
			{
				AFp BiasValue = (pChangeParam)->NewObjectProfile.GetBias();
				AFp GainValue = (pChangeParam)->NewObjectProfile.GetGain();
				CProfileBiasGain *pProfile = ((NodeBlend *)pNodeBlend)->GetObjectProfile();
				
				if (pProfile != NULL)
				{
					((NodeBlend*) pNodeBlend)->RequestObjectProfileProcessing (TRUE);
					pProfile->SetBiasGain(BiasValue, GainValue);
				}
				
			}
			break;
			case CHANGEBLEND_ATTRPROFILE:
			{
				AFp BiasValue = (pChangeParam)->NewAttrProfile.GetBias();
				AFp GainValue = (pChangeParam)->NewAttrProfile.GetGain();
				CProfileBiasGain *pProfile = ((NodeBlend *)pNodeBlend)->GetAttrProfile();
				
				if (pProfile != NULL)
				{
					// this may seem a little wierd - BUT we still need to do position
					// processing for the attribute ....
					((NodeBlend*) pNodeBlend)->RequestObjectProfileProcessing (TRUE);
					pProfile->SetBiasGain(BiasValue, GainValue);
				}

			}
			break;
			
			case CHANGEBLEND_BLENDONPATH:
			{
				((NodeBlend*)pNodeBlend)->SetBlendedOnCurve(pChangeParam->NewBlendedOnCurve);
	
				// increment/decrement the nodeblendpath counter
				if (pChangeParam->NewBlendedOnCurve)
					((NodeBlend*)pNodeBlend)->SetNumNodeBlendPaths(TRUE);
				else
					((NodeBlend*)pNodeBlend)->SetNumNodeBlendPaths(FALSE);

				NodeBlender* pNodeBlender = ((NodeBlend*)pNodeBlend)->FindFirstBlender();
				ERROR2IF((pNodeBlender == NULL), AC_FAIL, "Couldn't get blender");
				while (pNodeBlender != NULL)
				{
					if (pChangeParam->NewBlendedOnCurve)
						pNodeBlender->SetNodeBlendPathIndex(0);
					else
						pNodeBlender->SetNodeBlendPathIndex(-1);

					pNodeBlender = ((NodeBlend*)pNodeBlend)->FindNextBlender(pNodeBlender);
				}

				/*
				Spread* pSpread = pNodeBlend->FindParentSpread();
				if (pSpread != NULL)
				{
					RenderRegion* pOnTopRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
					while (pOnTopRegion)
					{
						((NodeBlend*)pNodeBlend)->RenderObjectBlobs(pOnTopRegion);

						// Go find the next region
						pOnTopRegion = DocView::GetNextOnTop(NULL);
					}
				}
				*/
			}
			break;

			default : ERROR2(AC_FAIL,"Unknown change blend type"); break;
			
			} // end
		} // end if (!bNode..
		else
		{
			ChangeBlendType ChangeType = pChangeParam->ChangeType;
			
			pNewAction->pNodeBlend  		   = pNodeBlend;
			pNewAction->ChangeParam.ChangeType = ChangeType;
			pNewAction->ChangeParam.SetOldValues(pNodeBlend);
			
			switch (ChangeType)
			{
			case CHANGEBLEND_COLOURBLENDTYPE:	((NodeContourController *)pNodeBlend)->SetColourBlendType(pChangeParam->NewColBlendType); 	break;
			default : ERROR2(AC_FAIL,"Unknown change blend type"); break;
			}
		} // end else


		pNewAction->ChangeParam.SetNewValues(pNodeBlend);
	} // end if (AC != ..

	return Ac;
}



/********************************************************************************************

>	void ChangeBlendAction::ChangeObjectProfileWithNoUndo (CProfileBiasGain &Profile, BOOL regenerateParents)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
				regenerateParents - do we need to regenerate the parents of the affected nodes?
	Purpose:	When applying blend (object) profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeBlend::DoWithParam ()).  This function is ONLY
				called from within BlendInfoBarOp::ChangeProfile () - after
				OpChangeBlend::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	BlendInfoBarOp::ChangeProfile (), OpChangeBlend::DoWithParam ().

********************************************************************************************/

void ChangeBlendAction::ChangeObjectProfileWithNoUndo (CProfileBiasGain &Profile, BOOL regenerateParents)
{
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	BOOL ok = !NodeList.IsEmpty();

	NodeListItem * pItem = NULL;

	if (ok)
	{
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}

		Document * pDoc = Document::GetCurrent();

		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}

			if (pNode->IS_KIND_OF(NodeBlend))
			{
				if (!regenerateParents)
				{
					// We now have a selected blend node so:
					// Change the profile in a non-undoable fashion
					// Invalidate the node's region (thereby causing a redraw)

					NodeBlend* pNodeBlend = (NodeBlend*)pNode;

					AFp BiasValue = Profile.GetBias ();
					AFp GainValue = Profile.GetGain ();
					CProfileBiasGain *pProfile = ((NodeBlend *)pNodeBlend)->GetObjectProfile();
					
					if (pProfile != NULL)
					{
						pNodeBlend->RequestObjectProfileProcessing (TRUE);
						pProfile->SetBiasGain(BiasValue, GainValue);
					}

					pDoc->ForceRedraw(pNodeBlend->FindParentSpread(), 
					pNodeBlend->GetBoundingRect(), FALSE, pNodeBlend);
				}
				else
				{
					// now the fun begins!  I need to persuade parents nodes to regenerate - BUT
					// I don't have an op to do it with!  Well lets not let a little thing like that
					// deter us ....

					NodeBlend* pNodeBlend = (NodeBlend*)pNode;
					
					// Ask the node if it's ok to do the op
					ObjChangeFlags cFlags;
					// Ilan 7/5/00
					// Ensure AllowOp passes messages on to children in compound (so geom linked attrs informed)
					cFlags.TransformNode = TRUE;
					ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);//this);
					ok = pNodeBlend->AllowOp(&ObjChange);
				}
			}
		}

		if (pDoc->GetOpHistory ().CanRedo ())
		{
			// then we need to clear out the redo information - since we are now 'before' it ....
			pDoc->GetOpHistory ().DeleteRedoableOps ();

			// and update the state of things ....
			DialogBarOp::SetSystemStateChanged();
			DialogBarOp::UpdateStateOfAllBars();
		}
	}

	// de-allocate the contents of NodeList.
	NodeList.DeleteAll();

	if (ok && regenerateParents)
	{
		// Inform the effected parents of the change
		ObjChangeFlags cFlags;
		cFlags.RegenerateNode = TRUE;
		ObjChangeParam ObjChange(OBJCHANGE_FINISHED,cFlags,NULL,NULL);
		
		UndoableOperation undoOp;
		
		undoOp.UpdateChangedNodes(&ObjChange);		// get compound nodes to regenerate themselves
		undoOp.FailAndDiscard ();					// Although we will have succeeded, we don't
													// want any of the overheads that would result
													// from succeeding - so simply fail
	}
}



/********************************************************************************************

>	void ChangeBlendAction::ChangeAttributeProfileWithNoUndo (CProfileBiasGain &Profile)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/2000
	Inputs:		Profile - the profile that is to be applied directly (i.e.  applied with no undo)
	Purpose:	When applying blend (attribute) profiles, we only want to generate one bit of undo information.
				This function allows us to meet this requirement (the one bit of undo information
				is generated via OpChangeBlend::DoWithParam ()).  This function is ONLY
				called from within BlendInfoBarOp::ChangeProfile () - after
				OpChangeBlend::DoWithParam () has been called.
	Errors:		-
	SeeAlso:	BlendInfoBarOp::ChangeProfile (), OpChangeBlend::DoWithParam ().

********************************************************************************************/

void ChangeBlendAction::ChangeAttributeProfileWithNoUndo (CProfileBiasGain &Profile)
{
	List NodeList;
	BevelTools::BuildListOfSelectedNodes(&NodeList, CC_RUNTIME_CLASS(NodeRenderableInk));

	BOOL ok = !NodeList.IsEmpty();

	NodeListItem * pItem = NULL;

	if (ok)
	{
		pItem = (NodeListItem *)NodeList.GetHead();

		Node* pSelNode = NULL;

		if (pItem)
		{
			pSelNode = pItem->pNode;
		}

		Document * pDoc = Document::GetCurrent();

		while (pSelNode != NULL && ok)
		{
			Node* pNode = pSelNode;

			pItem = (NodeListItem *)NodeList.GetNext(pItem);

			if (pItem)
			{
				pSelNode = pItem->pNode;
			}
			else
			{
				pSelNode = NULL;
			}

			if (pNode->IS_KIND_OF(NodeBlend))
			{
				// We now have a selected blend node so:
				// Change the profile in a non-undoable fashion
				// Invalidate the node's region (thereby causing a redraw)

				NodeBlend* pNodeBlend = (NodeBlend*)pNode;

				AFp BiasValue = Profile.GetBias ();
				AFp GainValue = Profile.GetGain ();
				CProfileBiasGain *pProfile = ((NodeBlend *)pNodeBlend)->GetAttrProfile();
				
				if (pProfile != NULL)
				{
					// this may seem a little wierd - BUT we still need to do position
					// processing for the attribute ....
					
					pNodeBlend->RequestObjectProfileProcessing (TRUE);
					pProfile->SetBiasGain(BiasValue, GainValue);
				}

				pDoc->ForceRedraw(pNodeBlend->FindParentSpread(), 
				pNodeBlend->GetBoundingRect(), FALSE, pNodeBlend);
			}
		}

		if (pDoc->GetOpHistory ().CanRedo ())
		{
			// then we need to clear out the redo information - since we are now 'before' it ....
			pDoc->GetOpHistory ().DeleteRedoableOps ();

			// and update the state of things ....
			DialogBarOp::SetSystemStateChanged();
			DialogBarOp::UpdateStateOfAllBars();
		}
	}

	// de-allocate the contents of NodeList.
	NodeList.DeleteAll();
}



/********************************************************************************************

>	ActionCode ChangeBlendAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  This will reset the num blend steps in pThisNodeBlend to OldNumSteps,
				after creating another action to record the current num steps of pThisNodeBlend
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeBlendAction::Execute()
{
	ChangeParam.SwapOldAndNew();

	ActionCode Act;
	ChangeBlendAction* pAction;
	Act = ChangeBlendAction::Init(pOperation,pOppositeActLst,pNodeBlend,&ChangeParam,&pAction);

	return Act;
}

ChangeBlendAction::~ChangeBlendAction()
{
}

//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

/********************************************************************************************

>	void ChangeBlendOpParam::SetOldValues(NodeBlend* pNodeBlend)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		pNodeBlend = ptr to a node blend object
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the blend node into this classes Old member vars
	Errors:		-

********************************************************************************************/

void ChangeBlendOpParam::SetOldValues(NodeRenderableInk* pNodeBlend)
{
	// DMc changed to include contours
	
	if (pNodeBlend->IsKindOf(CC_RUNTIME_CLASS(NodeBlend)))
	{
		OldAntiAlias 	  = !((NodeBlend*)pNodeBlend)->IsNotAntialiased();
		OldOneToOne  	  = ((NodeBlend *)pNodeBlend)->IsOneToOne();
		OldColBlendType	  = ((NodeBlend *)pNodeBlend)->GetColourBlendType();
		OldTangential     = ((NodeBlend *)pNodeBlend)->IsTangential();
		OldBlendedOnCurve = ((NodeBlend *)pNodeBlend)->IsOnACurve();
		OldEndObject     = ((NodeBlend *)pNodeBlend)->GetLastEdited();

		// get the object profile values
		CProfileBiasGain* pProfile = ((NodeBlend *)pNodeBlend)->GetObjectProfile();
		OldObjectProfile = *pProfile;
		pProfile = NULL;
	
		pProfile = ((NodeBlend *)pNodeBlend)->GetAttrProfile();
		OldAttrProfile = *pProfile;
		pProfile = NULL;
	}
}

/********************************************************************************************

>	void ChangeBlendOpParam::SetNewValues(NodeRenderabeleInk* pNodeBlend)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		pNodeBlend = ptr to a node blend object
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the blend node into this classes New member vars
	Errors:		-

********************************************************************************************/

void ChangeBlendOpParam::SetNewValues(NodeRenderableInk* pNodeBlend)
{
	if (pNodeBlend->IsKindOf(CC_RUNTIME_CLASS(NodeBlend)))
	{
		NewAntiAlias 	  = !((NodeBlend *)pNodeBlend)->IsNotAntialiased();
		NewOneToOne  	  = ((NodeBlend *)pNodeBlend)->IsOneToOne();
		NewColBlendType	  = ((NodeBlend *)pNodeBlend)->GetColourBlendType();
		NewTangential	  = ((NodeBlend *)pNodeBlend)->IsTangential();
		NewBlendedOnCurve = ((NodeBlend *)pNodeBlend)->IsOnACurve();
		NewEndObject     = ((NodeBlend *)pNodeBlend)->GetLastEdited();

		// DY profile code 9/99
		CProfileBiasGain* pProfile = ((NodeBlend *)pNodeBlend)->GetObjectProfile();
		NewObjectProfile = *pProfile;
		pProfile = NULL;
		pProfile = ((NodeBlend *)pNodeBlend)->GetAttrProfile();
		NewAttrProfile = *pProfile;
		pProfile = NULL;
	}
}

/********************************************************************************************

>	void ChangeBlendOpParam::SwapOldAndNew()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the old and new values of this class
	Errors:		-

********************************************************************************************/

void ChangeBlendOpParam::SwapOldAndNew()
{
	SWAP(BOOL,				OldAntiAlias,	   NewAntiAlias);
	SWAP(BOOL,				OldOneToOne,	   NewOneToOne);
	SWAP(ColourBlendType,	OldColBlendType,   NewColBlendType);
	SWAP(BOOL,				OldTangential,	   NewTangential);
	SWAP(BOOL,				OldBlendedOnCurve, NewBlendedOnCurve);
	SWAP(EndObject,			OldEndObject,      NewEndObject);
	// DY 9/99 profile code
	SwapProfiles(&OldObjectProfile, &NewObjectProfile);
	SwapProfiles(&OldAttrProfile, &NewAttrProfile);
}

/********************************************************************************************

>	void ChangeBlendOpParam::SwapProfiles()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/99
	Inputs:		pointers to the old and new CProfileBiasgain objects
	Outputs:	the same pointers, swapped
	Returns:	-
	Purpose:	Swaps the old and new profiles of this class by swapping the pointers
	Errors:		-

********************************************************************************************/

void ChangeBlendOpParam::SwapProfiles(CProfileBiasGain* pOldProfile, 
									  CProfileBiasGain* pNewProfile) 
{
	CProfileBiasGain TempProfile = *pOldProfile;
	*pOldProfile = *pNewProfile;
	*pNewProfile = TempProfile;
}



//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The ChangeBlenderAction class

/********************************************************************************************

>	ChangeBlenderAction::ChangeBlenderAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Purpose:	Constructor for the action

********************************************************************************************/

ChangeBlenderAction::ChangeBlenderAction()
{
	m_pNodeBlender = NULL;
}


/********************************************************************************************

>	ActionCode ChangeBlenderAction::Init( 	Operation* 				pOp,
											ActionList* 			pActionList,
											NodeBlender* 			pNodeBlender,
											ChangeBlenderOpParam&	ChangeParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to action list to which this action should be added
				pNodeBlender	= ptr to NodeBlender to change 
				ChangeParam		= class that details how the blender should be changed.
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

				This function actually changes the blender node in a way specified in ChangeParam
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode ChangeBlenderAction::Init(	Operation* 				pOp,
										ActionList* 			pActionList,
										NodeBlender* 			pNodeBlender,
										ChangeBlenderOpParam&	ChangeParam)
{
	ERROR2IF(pNodeBlender == NULL,AC_FAIL,"pNodeBlender is NULL");

	UINT32 ActSize = sizeof(ChangeBlenderAction);

	ChangeBlenderAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(ChangeBlenderAction),(Action**)&pNewAction);

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		ChangeBlenderType ChangeType = ChangeParam.m_ChangeType;

		pNewAction->m_pNodeBlender  		   = pNodeBlender;
		pNewAction->m_ChangeParam.m_ChangeType = ChangeType;

		pNewAction->m_ChangeParam.SetOldValues(pNodeBlender);
		switch (ChangeType)
		{
			case CHANGEBLENDER_ANGLESTART:			pNodeBlender->SetAngleStart(ChangeParam.m_NewAngleStart); break;
			case CHANGEBLENDER_ANGLEEND: 			pNodeBlender->SetAngleEnd(  ChangeParam.m_NewAngleEnd);	break;
			case CHANGEBLENDER_PATHSTART:			
			{
				pNodeBlender->SetProportionOfPathDistStart(ChangeParam.m_NewPathStart); 
				pNodeBlender->SetUninitialised();
			}
			break;
			case CHANGEBLENDER_PATHEND:				
			{
				pNodeBlender->SetProportionOfPathDistEnd(ChangeParam.m_NewPathEnd); 
				pNodeBlender->SetUninitialised();
			}	
			break;
			case CHANGEBLENDER_REGEN:
			{
				pNodeBlender->SetUninitialised();
			}
			break;
			case CHANGEBLENDER_NBPINDEX:
			{
				pNodeBlender->SetNodeBlendPathIndex(ChangeParam.m_NewNodeBlendPathIndex);
			}
			break;
			case CHANGEBLENDER_BLENDONCURVE:
			{
				pNodeBlender->SetBlendedOnCurve(ChangeParam.m_NewBlendedOnCurve);
			}
			break;
			case CHANGEBLENDER_SWAPENDS:
			{
				pNodeBlender->ReverseEnds();
				pNodeBlender->SetUninitialised();
			}
			break;	
			default : ERROR2(AC_FAIL,"Unknown change blend type"); break;
		}
		pNewAction->m_ChangeParam.SetNewValues(pNodeBlender);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode ChangeBlenderAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode ChangeBlenderAction::Execute()
{
	m_ChangeParam.SwapOldAndNew();

	ActionCode Act;
	Act = ChangeBlenderAction::Init(pOperation,pOppositeActLst,m_pNodeBlender,m_ChangeParam);

	return Act;
}

ChangeBlenderAction::~ChangeBlenderAction()
{
}

//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

/********************************************************************************************

>	void ChangeBlenderOpParam::SetOldValues(NodeBlender* pNodeBlender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		pNodeBlender = ptr to a node blend object
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the blender node into this classes Old member vars
	Errors:		-

********************************************************************************************/

void ChangeBlenderOpParam::SetOldValues(NodeBlender* pNodeBlender)
{
	m_OldAngleStart = pNodeBlender->GetAngleStart();
	m_OldAngleEnd   = pNodeBlender->GetAngleEnd();
	m_OldPathStart  = pNodeBlender->GetProportionOfPathDistStart();
	m_OldPathEnd    = pNodeBlender->GetProportionOfPathDistEnd();
	m_OldNodeBlendPathIndex = pNodeBlender->GetNodeBlendPathIndex();
	m_OldBlendedOnCurve = pNodeBlender->IsBlendedOnCurve();
	m_OldObjIndexEnd = pNodeBlender->GetObjIndexEnd();
	m_OldObjIndexStart = pNodeBlender->GetObjIndexStart();


}

/********************************************************************************************

>	void ChangeBlenderOpParam::SetNewValues(NodeBlender* pNodeBlender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		pNodeBlender = ptr to a node blend object
	Outputs:	-
	Returns:	-
	Purpose:	Copies the current state of the blender node into this classes New member vars
	Errors:		-

********************************************************************************************/

void ChangeBlenderOpParam::SetNewValues(NodeBlender* pNodeBlender)
{
	m_NewAngleStart = pNodeBlender->GetAngleStart();
	m_NewAngleEnd   = pNodeBlender->GetAngleEnd();
	m_NewPathStart  = pNodeBlender->GetProportionOfPathDistStart();
	m_NewPathEnd    = pNodeBlender->GetProportionOfPathDistEnd();
	m_NewNodeBlendPathIndex = pNodeBlender->GetNodeBlendPathIndex();
	m_NewBlendedOnCurve = pNodeBlender->IsBlendedOnCurve();
	m_NewObjIndexEnd = pNodeBlender->GetObjIndexEnd();
	m_NewObjIndexStart = pNodeBlender->GetObjIndexStart();
}

/********************************************************************************************

>	void ChangeBlenderOpParam::SwapOldAndNew()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Swaps the old and new values of this class
	Errors:		-

********************************************************************************************/

void ChangeBlenderOpParam::SwapOldAndNew()
{
	SWAP(double,m_NewAngleStart,m_OldAngleStart);
	SWAP(double,m_NewAngleEnd  ,m_OldAngleEnd);
	SWAP(double,m_NewPathStart, m_OldPathStart);
	SWAP(double,m_NewPathEnd,   m_OldPathEnd);
	SWAP(UINT32, m_NewNodeBlendPathIndex, m_OldNodeBlendPathIndex);
	SWAP(BOOL, m_OldBlendedOnCurve, m_NewBlendedOnCurve);
	SWAP(INT32, m_OldObjIndexStart, m_NewObjIndexStart);
	SWAP(INT32, m_OldObjIndexEnd, m_NewObjIndexEnd);
}

//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
// The InvalidateBoundsAction class

/********************************************************************************************

>	InvalidateBoundsAction::InvalidateBoundsAction()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/99
	Purpose:	Constructor for the action

********************************************************************************************/

InvalidateBoundsAction::InvalidateBoundsAction()
{
	m_pNode = NULL;
	m_IncludeChildren = FALSE;
}


/********************************************************************************************

>	ActionCode InvalidateBoundsAction::Init(Operation* 				pOp,
											ActionList* 			pActionList,
											NodeRenderableBounded*	pNode,
											BOOL					IncludeChildren)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/99
	Inputs:		pOp  			= ptr to the operation to which this action belongs
				pActionList 	= ptr to action list to which this action should be added
				pNodeBlender	= ptr to Node whos bounds invalidating
				IncludeChildren = invalidate the bounds of the children too
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	This is the function which creates an instance of this action. If there is no room 
				in the undo buffer (which is determined by the base class Init function called within)
				the function will either return AC_NO_RECORD which means the operation can continue, 
				but no undo information needs to be stored, or AC_OK which means the operation should
				continue AND record undo information. If the function returns AC_FAIL, there was not 
				enough memory to record the undo information, and the user has decided not to continue
				with the operation.

	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/



ActionCode InvalidateBoundsAction::Init(Operation* 				pOp,
										ActionList* 			pActionList,
										NodeRenderableBounded*	pNode,
										BOOL					IncludeChildren)
{
	ERROR2IF(pNode == NULL,AC_FAIL,"pNode is NULL");

	UINT32 ActSize = sizeof(InvalidateBoundsAction);

	InvalidateBoundsAction* pNewAction;
	ActionCode Ac = Action::Init(pOp,pActionList,ActSize,CC_RUNTIME_CLASS(InvalidateBoundsAction),(Action**)&pNewAction);

	if (Ac != AC_FAIL && pNewAction != NULL)
	{
		pNewAction->m_pNode  		   = pNode;
		pNewAction->m_IncludeChildren  = IncludeChildren;

		pNode->InvalidateBoundingRect(IncludeChildren);
	}

	return Ac;
}

/********************************************************************************************

>	ActionCode InvalidateBoundsAction::Execute();

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/99
	Inputs:		-
	Outputs:	-
	Returns:	ActionCode, one of AC_OK, AC_NO_RECORD or AC_FAIL
	Purpose:	Executes the action.  
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

ActionCode InvalidateBoundsAction::Execute()
{
	ActionCode Act;
	Act = InvalidateBoundsAction::Init(pOperation,pOppositeActLst,m_pNode,m_IncludeChildren);

	return Act;
}

InvalidateBoundsAction::~InvalidateBoundsAction()
{
}

//-------------------------------------------
//-------------------------------------------
//-------------------------------------------
// class CompoundNodeTreeFactoryList

/********************************************************************************************

>	CompoundNodeTreeFactoryList::~CompoundNodeTreeFactoryList()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	9/8/99
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	Deletes the list
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/

CompoundNodeTreeFactoryList::~CompoundNodeTreeFactoryList()
{
	CompoundNodeTreeFactoryListItem * pItem = (CompoundNodeTreeFactoryListItem *)GetHead();

	while (pItem)
	{
		delete pItem->pFactory;
		pItem->pFactory = NULL;

		pItem = (CompoundNodeTreeFactoryListItem *)GetNext(pItem);
	}
	
	List::DeleteAll();
}

/********************************************************************************************

>	void CompoundNodeTreeFactoryList::AddItem(CompoundNodeTreeFactory *pItem)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	9/8/99
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	Adds an item to the list - but won't add 2 items of the same kind !
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
void CompoundNodeTreeFactoryList::AddItem(CompoundNodeTreeFactory *pItem)
{
	// find out if any element in the list matches this type - if so, don't add it !
	BOOL bAdd = TRUE;

	CompoundNodeTreeFactoryListItem * pListItem = (CompoundNodeTreeFactoryListItem *)GetHead();

	while (pListItem && bAdd)
	{
		if (pListItem->pFactory->GetRuntimeClass() == pItem->GetRuntimeClass())
		{
			bAdd = FALSE;
		}

		pListItem = (CompoundNodeTreeFactoryListItem *)GetNext(pListItem);
	}

	if (bAdd)
	{
		pListItem = new CompoundNodeTreeFactoryListItem;
		pListItem->pFactory = pItem;
		AddTail(pListItem);
	}
}

/********************************************************************************************

>	CompoundNodeTreeFactory * CompoundNodeTreeFactoryList::GetItem(INT32 index)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	9/8/99
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	Adds an item to the list - but won't add 2 items of the same kind !
	Errors:		-
	SeeAlso:	Action::Init()

********************************************************************************************/
CompoundNodeTreeFactory * CompoundNodeTreeFactoryList::GetItem(INT32 index)
{
	if( index >= INT32(GetCount()) )
		return NULL;
	
	CompoundNodeTreeFactoryListItem * pItem = (CompoundNodeTreeFactoryListItem *)GetHead();

	for (INT32 i = 0 ; i < index; i++)
	{
		pItem = (CompoundNodeTreeFactoryListItem *)GetNext(pItem);
	}
	
	return pItem->pFactory;
}



/********************************************************************************************

>	OpEditBlendEndObject::OpEditBlendEndObject()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Returns:	
	Purpose:	default constructor

********************************************************************************************/

OpEditBlendEndObject::OpEditBlendEndObject(BlendTool* pBlendTool)
{
	if (pBlendTool == NULL)
	{
		ERROR3("No blend tool");
		return;
	}
	m_pTransMatrix = NULL;
	m_pNodeBlend = NULL;
	m_pRange = NULL;
	m_pNodeBlendPath = NULL;
	m_pBlendTool = pBlendTool;
}



/********************************************************************************************

>	OpEditBlendEndObject::~OpEditBlendEndObject()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Returns:	
	Purpose:	destructor, deletes pointers to dynamically created objects

********************************************************************************************/

OpEditBlendEndObject::~OpEditBlendEndObject()
{
	delete m_pTransMatrix;
	delete m_pSelState;
}


/********************************************************************************************

>	BOOL OpEditBlendEndObject::Declare()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpEditBlendEndObject::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpEditBlendEndObject), 
								OPTOKEN_EDITBLENDENDOBJECT,
								OpEditBlendEndObject::GetState,
								0,      	/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

 >	static OpState OpEditBlendEndObject::GetState(String_256* Description, OpDescriptor*)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the operation 
	Returns:	The state of the operation, so that menu items (ticks and greying) can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpEditBlendEndObject::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;

	return State;
}



/********************************************************************************************

>	BOOL OpEditBlendEndObject::DoDrag(DocCoord PointerPos, Spread* pSpread)
    
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		Position of the pointer to start the drag from.
				The current spread.
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This starts a drag to change the position of one of the end objects of a blend
				The DragFinished() method will do the hard work of recalculating the 
				blend etc.


********************************************************************************************/

/* IMPLEMENTATION NOTE:
   This operation does some very bad things, forced on me by failures in the Range code.
   Range::RenderXOROutlinesOn() does not work when you generate your own range which you want
   to XOR.  However it does work with the selection range.  The problem here is that we wish to
   XOR one element of the selection, therefore the method I have used is to temporarily change
   the selection for the duration of this drag so that the only object selected is the one we 
   are dragging.  The selection is restored in DragFinished().  This is a pretty poor way of 
   doing things but I see no alternative.
   */


BOOL OpEditBlendEndObject::DoDrag(DocCoord PointerPos, Spread* pSpread)
{
	GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_HIGH, this);

	List BlendList;
	Node* pNodeUnderPoint = NULL;
	BOOL ok = BevelTools::BuildListOfSelectedNodes(&BlendList, CC_RUNTIME_CLASS(NodeBlend), FALSE);
	if (ok)
	{
		NodeListItem* pListItem = (NodeListItem*)BlendList.GetHead();
		while (pListItem != NULL)
		{
			
			NodeBlend* pNodeBlend = (NodeBlend*)pListItem->pNode;
			
			ok = pNodeBlend->HitOnEndDragBlob(PointerPos, &pNodeUnderPoint);
			if (ok)
			{
				ObjChangeFlags cFlags;
				cFlags.TransformNode = TRUE;
				ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);
				if (pNodeBlend->AllowOp(&ObjChange, TRUE))
				{
					m_pNodeBlend = pNodeBlend;						
					break;
				}
			}
			pListItem = (NodeListItem*)BlendList.GetNext(pListItem);
		}
	}
	else
	{
		// that didn't work, but it may be selected inside so try this;
		pNodeUnderPoint = NodeRenderableInk::FindSimpleAtPoint(pSpread, PointerPos);
		
		if (pNodeUnderPoint == NULL)
		{
			// op shouldn't have been allowed as we are not above a node, lets quit
			FailAndExecute();
			End();
			BlendList.DeleteAll();
			return FALSE;
		}
		
		Node* pParent = pNodeUnderPoint->FindParentOfSelected();
		if (pParent == NULL || (!pParent->IS_KIND_OF(NodeBlend)))
		{
			FailAndExecute();
			End();
			BlendList.DeleteAll();
			return FALSE;
		}
		// its ok, we can continue
		m_pNodeBlend = (NodeBlend*)pParent;

	}

	BlendList.DeleteAll();
	// see if there is a node between the blend and the simple node, if so it may be a group
	NodeRenderableInk* pGroupUnderPoint = NodeRenderableInk::FindInnerCompound(pNodeUnderPoint, m_pNodeBlend);
	
	Node* pNodeToEdit = NULL;
	
	if (pGroupUnderPoint != NULL)
	{
		// if the group is the same as the nodeblend
		if (pGroupUnderPoint == m_pNodeBlend)
		{
			pNodeToEdit = (Node*) pNodeUnderPoint;
		}
		else
		{
			pNodeToEdit = (Node*)pGroupUnderPoint;
		}
	}
	else
	{
		FailAndExecute();
		End();
		return FALSE;
	}
	
	// save the selection state for restoring at the end of the drag
	ALLOC_WITH_FAIL(m_pSelState, new SelectionState, this);
	ok = m_pSelState->Record();

	if (!ok)
	{
		ERROR3("Could not record selection");
		FailAndExecute();
		End();
		return FALSE;
	}	


	m_pRange = GetApplication()->FindSelection(); 

	if (m_pRange == NULL)
	{
		ERROR3("No Selection");
		FailAndExecute();
		End();
		return FALSE;
	}

	// the bodging continues... When the TransformNodeAction gets called as part of
	// the undoing of this op it changes the selection so that only the transformed node is
	// selected.  Hence we need to insert an action that will occur after TransformNode in the 
	// undo list in order to have the selection as it started....nice
	Action* pRestoreAction;
	SelectionState* pCopySelState =  NULL;
	ALLOC_WITH_FAIL(pCopySelState, new SelectionState, this);  // gets destroyed by RestoreSelectionsAction
	pCopySelState->Record();  

	ok = RestoreSelectionsAction::Init(this, &UndoActions, pCopySelState,
										TRUE, TRUE, FALSE, TRUE, FALSE, FALSE,
										&pRestoreAction);
/*  the function proto
RestoreSelectionsAction::Init(Operation* const pOp, 
						   				 ActionList* pActionList, 	
						   				 SelectionState* SelState,
						   				 BOOL Toggle, 
						   				 BOOL ToggleStatus,
						   				 BOOL SelStateShared,
						   				 BOOL RenderStartBlobs, 
										 BOOL RenderEndBlobs, 
 						   				 BOOL StartRestore,  
						   				 Action** NewAction)       

*/
	
	// setting the selection so that only the node we wish to be XOR'd during the
	// drag is selected - ugh.
	if (ok)
	{
		NodeRenderableInk::DeselectAll();
		((NodeRenderable*)pNodeToEdit)->Select(TRUE);
	
		// we don't want to see those object blobs whilst dragging
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
				// Decide which blobs we will display
				BlobStyle MyBlobs;
			
				MyBlobs.Object = FALSE;
				MyBlobs.Tiny = FALSE;

				BlobMgr->ToolInterest(MyBlobs);
		}


		m_pRange->ResetXOROutlineRenderer();

		// work out where we need to translate to and create a matrix for it
		DocRect BRect = ((NodeRenderableBounded*)pNodeToEdit)->GetBoundingRect();
		m_StartCoord = BRect.Centre();

		ALLOC_WITH_FAIL(m_pTransMatrix, new Matrix, this)
		m_pTransMatrix->SetTranslation(PointerPos - m_StartCoord);

		m_pRange->RenderXOROutlinesOn(NULL, pSpread, m_pTransMatrix, pNodeUnderPoint);
	
		m_LastCoord = m_StartCoord;
		RenderSelectedObjectBlobs(pSpread);
		// And tell the Dragging system that we need drags to happen
		StartDrag( DRAGTYPE_AUTOSCROLL );
	}
	else
	{
		FailAndExecute();
		End();
	}
	return ok;
}





/********************************************************************************************

>	void OpEditBlendEndObject::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods  - Which key modifiers are being pressed
				pSpread    - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position and calculates the new dragged outline of the EORd
				bounding box.  The closest point on the NodeBlendpath to the actual mouseclick
				is used.
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpEditBlendEndObject::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag)
{
	// get rid of existing blobs
	RenderDragBlobs(pSpread, m_LastCoord, FALSE);
	if (m_pBlendTool != NULL)
		m_pBlendTool->DisplayStatusBarHelp(_R(IDS_BLENDSTATUS_MOVEEND));
	DocCoord PointOnLine;
	BOOL ValidPoint = GetClosestPointOnPath(PointerPos, &PointOnLine);
	if (!ValidPoint)
	{	
		EndDrag();	// not sure this is correct, should we make some attempt to 
		return;       // render the xor outlines off? 
	}
	// draw new blobs
	RenderDragBlobs(pSpread, PointOnLine, TRUE);
	//RenderAllBlobs(pSpread);
	
	m_LastCoord = PointOnLine;



}


/********************************************************************************************

>	void OpEditBlendEndObject::DragPointerIdle( DocCoord PointerPos,
												ClickModifiers ClickMods,
												Spread* pSpread, BOOL bSolidDrag)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				pSpread    - The spread that the mouse pointer is over
	Purpose:	If the pointer is idle take the opportunity to render a few more objects

********************************************************************************************/

void OpEditBlendEndObject::DragPointerIdle(DocCoord PointerPos, ClickModifiers Clickmodifiers, Spread* pSpread, BOOL bSolidDrag)
{

	m_pRange->RenderXOROutlinesOn(NULL, pSpread, m_pTransMatrix, NULL);
}

/********************************************************************************************

>	void OpEditBlendEndObject::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				pSpread - The spread that the drag finished on
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	Ends the drag and recalculates the blend 
	SeeAlso:	ClickModifiers, DoDrag() for an explanation of whats going on with 
				the selection.

********************************************************************************************/

void OpEditBlendEndObject::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);
	
	DocCoord PointOnLine;
	
	BOOL ValidPoint = GetClosestPointOnPath(PointerPos, &PointOnLine);
	if (!ValidPoint)
	{	
		EndDrag();					// not sure this is correct, should we make some attempt to 
		return;       // render the xor outlines off? 
	}

	Matrix NewMatrix;
	NewMatrix.SetTranslation(PointOnLine - m_StartCoord);
	
	m_pRange->RenderXOROutlinesOff(NULL, pSpread, &NewMatrix);
	EndDrag();
	
	if (Success)
		Success = RecalculateBlend(PointOnLine);

	// bring back the object blobs
	// we don't want to see those object blobs whilst dragging
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	if (BlobMgr != NULL)
	{
			// Decide which blobs we will display
			BlobStyle MyBlobs;
			
			MyBlobs.Object = TRUE;
			MyBlobs.Tiny = FALSE;

			BlobMgr->ToolInterest(MyBlobs);
	}

	// restore the selection - very important
	m_pSelState->Restore();
	if (Success)
	{	
		SelRange* pSel = GetApplication()->FindSelection();
		pSel->Update();
	}
	else
		FailAndExecute();
	
	// Inform all changed nodes that we have finished
	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_FINISHED, cFlags, NULL, this);
	
	/*BOOL ok =*/ UpdateChangedNodes(&ObjChange);
	End();
	
	//TRACEUSER( "Diccon", _T("\nFinished dragging end object\n"));

}


/********************************************************************************************

>	BOOL OpEditBlendEndObject::RecalculateBlend(DocCoord PointerPos)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		PointerPos - The position to move the end object to
	Outputs:    
	returns:    TRUE if successful , FALSE otherwise
	Purpose:	To recalculate the number of steps in the blend or distance between steps 
				following the change of position of one of the end objects.  Having done this 
				it then creates the actions to perform the change.

***********************************************************************************************/

BOOL OpEditBlendEndObject::RecalculateBlend(DocCoord EndPosition)
{
	// check the inputs
	if (m_pNodeBlend == NULL) 
	{
		ERROR3("NodeBlend is not initialised");
		return FALSE;
	}
	// get a pointer to the path
	
	if (m_pNodeBlendPath == NULL)
	{
		ERROR3("NodeBlend is not on a path");
		return FALSE;
	}
	// find out how far the point is along the path
	double PathLength = m_pNodeBlendPath->GetPathLength();
	INT32 DistanceAlongPath = 0;
	BOOL Valid = m_pNodeBlendPath->InkPath.GetDistanceToPoint(EndPosition, &DistanceAlongPath);
	
	// Range::FindFirst() always returns NULL for some reason so I'm forced
	// to use FindLast(), given that there is only one this seems ok.
	Node* pNode = m_pRange->FindLast();
	NodeRenderableInk* pNodeToEdit= NULL;
	if (pNode == NULL)
	{
		ERROR3("Range is empty");
		Valid = FALSE;
	}
	else
		 pNodeToEdit = (NodeRenderableInk*)pNode;

	if (Valid)
	{
		// depending on which Edit mode the blend is in try to keep either
		// number of steps or distance between steps constant.
		double NewPathProportion = DistanceAlongPath / PathLength;
		if (m_pNodeBlend->GetEditState() == EDIT_STEPS)
		{	
			Valid = InsertChangeEndActions(NewPathProportion, EndPosition, pNode);
		}
		else
		{
			double StepDistance = m_pNodeBlend->GetDistanceEntered();
			Valid = InsertChangeEndActions(NewPathProportion, EndPosition, StepDistance, pNode);
		}

	}
	

	return Valid;

}



/********************************************************************************************

>	BOOL OpEditBlendEndObject::InsertChangeEndActions(double PathProportion, 
													  DocCoord NewPosition, Node* pNodeToEdit)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	17/9/99
	Inputs:		PathProportion - the new proportion along the path to move the end object to
				pNodeToEdit - the blend object that is being edited
				NewPosition - the new position as a coordinate
	Outputs:    pNodeToEdit - some members are altered by these actions			
	returns:    TRUE if all actions were correctly carried out, else FALSE
	Purpose:	To calculate and perform the actions necessary to transform pNode to its new PathProportion
				whilst maintaining a constant number of blend steps. 

   See Also:	Overridden version of this function for use with constant distance between blend steps
				

***********************************************************************************************/
	
BOOL OpEditBlendEndObject::InsertChangeEndActions(double NewPathProp, DocCoord NewPosition,
												  Node* pNodeToEdit)
{
	// couple of quick checks
	if (pNodeToEdit == NULL)
	{
		ERROR3("Node to edit is NULL");
		return FALSE;
	}

	if (NewPathProp < 0.0 || NewPathProp > 1.0)
	{
		ERROR3("Invalid path proportion");
		return FALSE;
	}

	// find out if this node is the first or last node of the blender on a curve
	BOOL NodeIsFirst;
	NodeBlender* pNodeBlender = GetBlenderAndPosition(pNodeToEdit, &NodeIsFirst);
	BOOL Valid = FALSE;		
	if (pNodeBlender == NULL)
	{
		ERROR3("No NodeBlender");
		return FALSE;
	}
	else
	{	
		Valid = DoInvalidateNodeRegion((NodeRenderableInk*)m_pNodeBlend,TRUE,FALSE);
		if (Valid)	Valid = (InvalidateBoundsAction::Init(this,&UndoActions,m_pNodeBlend,TRUE) != AC_FAIL);

		// change the proportion along the path to the new value
		if (Valid)
			Valid = InsertChangeProportion(NewPathProp, NodeIsFirst, pNodeBlender);
		
		double AngleChange = 0.0;
		// if we are rotated along the curve then calculate the new angle
		if (m_pNodeBlend->IsTangential() && Valid)
		{
			
			NodeBlendPath* pNodeBlendPath = pNodeBlender->GetNodeBlendPath();
			if (pNodeBlendPath != NULL)
			{
				double NewAngle = 0.0;
				double OldAngle = 0.0;
				double BlendRatio = 0.0;
				ChangeBlenderOpParam ChangeParam;
				if (NodeIsFirst)
				{
					OldAngle = pNodeBlender->GetAngleStart();
					ChangeParam.m_ChangeType = CHANGEBLENDER_ANGLESTART;
					BlendRatio = 0.0;
				}
				else
				{
					OldAngle = pNodeBlender->GetAngleEnd();
					ChangeParam.m_ChangeType = CHANGEBLENDER_ANGLEEND;
					BlendRatio = 1.0;
				}
//				MILLIPOINT PathDistance = (MILLIPOINT)(pNodeBlendPath->GetPathLength());
//				MILLIPOINT PointDistance = (MILLIPOINT)(NewPathProp * PathDistance);
				DocCoord Point;
				Valid = pNodeBlender->GetPointOnNodeBlendPath(BlendRatio, &Point, &NewAngle);
				if (Valid)
				{
					AngleChange = NewAngle - OldAngle;
					TRACEUSER( "Diccon", _T("Moved end: OldAngle: %f, NewAngle %f, Change %f\n"), OldAngle, NewAngle, AngleChange);
					// make it undoable		
					if (!NodeIsFirst)
						ChangeParam.m_NewAngleEnd = NewAngle;
					else
						ChangeParam.m_NewAngleStart = NewAngle;
					ActionCode Ac = ChangeBlenderAction::Init(	this, &UndoActions,
																pNodeBlender,ChangeParam);
					Valid = (Ac !=AC_FAIL);
				}

			}
		}
		
		// transform the edited node to its new location
		if (Valid) 
			Valid = m_pNodeBlend->TransformNodeToPoint((NodeRenderableInk*)pNodeToEdit,&NewPosition,this,AngleChange);
		
		// find out if this node is part of another blend, and if so then regenerate it
		if (Valid)
		{
			BOOL OtherEnd;
			NodeBlender* pOtherBlender = m_pNodeBlend->NodeIsPartOfBlender( pNodeToEdit, pNodeBlender, &OtherEnd); //!NodeIsFirst);
			if (pOtherBlender != NULL && pOtherBlender != pNodeBlender)
			{
				// if its not on a curve just ask it to reninitialise
				if (!pOtherBlender->IsBlendedOnCurve())
				{
					ChangeBlenderOpParam Param;
					Param.m_ChangeType = CHANGEBLENDER_REGEN;
					Valid = ChangeBlenderAction::Init(this, &UndoActions, pOtherBlender, Param);
				}
				// if it is on a curve work set the new position for the other blender
				else
				{
					ChangeBlenderOpParam ChangeParam;
					if (OtherEnd)
					{
						ChangeParam.m_ChangeType = CHANGEBLENDER_PATHSTART;
						ChangeParam.m_NewPathStart = NewPathProp;
					}
					else
					{
						ChangeParam.m_ChangeType = CHANGEBLENDER_PATHEND;
						ChangeParam.m_NewPathEnd = NewPathProp;
					}
					ActionCode Ac = ChangeBlenderAction::Init(	this, &UndoActions,
																pOtherBlender,ChangeParam);
					Valid = (Ac !=AC_FAIL);

				}
			}
		}
	}
	// regenerate any shadows, bevels or contours
	Node* pController = ((Node*)m_pNodeBlend)->FindParent(CC_RUNTIME_CLASS(NodeShadowController));
	if (pController != NULL)
		pController->RegenerateNode(this);
	else 
	{
		pController = ((Node*)m_pNodeBlend)->FindParent(CC_RUNTIME_CLASS(NodeBevelController));
		if (pController != NULL)
			pController->RegenerateNode(this);
		else
		{
			pController = ((Node*)m_pNodeBlend)->FindParent(CC_RUNTIME_CLASS(NodeContourController));
			if (pController != NULL)
				pController->RegenerateNode(this);
		}
	}

		
	if (Valid)
		Valid = DoInvalidateNodeRegion((NodeRenderableInk*)m_pNodeBlend,TRUE,FALSE);
	if (Valid)
		Valid = (InvalidateBoundsAction::Init(this,&UndoActions,m_pNodeBlend,TRUE) != AC_FAIL);
	
	return Valid;
}



/********************************************************************************************

>	BOOL OpEditBlendEndObject::InsertChangeEndActions(double PathProportion, DocCoord NewPosition,
													   double StepDistance, Node* pNodeToEdit)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	17/9/99
	Inputs:		PathProportion - the new proportion along the path to move the end object to
				pNode - the blend object that is being edited
				NewPosition - the new position as a coordinate
	Outputs:    pNodeToEdit - some members are altered by these actions			
	returns:    TRUE if all actions were correctly carried out, else FALSE
	Purpose:	To calculate and perform the actions necessary to transform pNode to its new PathProportion
				whilst maintaining a constant distance between blend steps. This requires calculating a new position for
				the node that is not being edited in order to maintain the correct distance, it may also require
				changing the number of steps in the blend. 

   See Also:	Overridden version of this function for use with constant number of blend steps
				

***********************************************************************************************/
	
BOOL OpEditBlendEndObject::InsertChangeEndActions(double NewPathProp, DocCoord NewPosition, 
												  double StepDistance, Node* pNodeToEdit)
{
	// couple of quick checks
	if (pNodeToEdit == NULL)
	{
		ERROR3("Node to edit is NULL");
		return FALSE;
	}
	if (NewPathProp < 0.0 || NewPathProp > 1.0)
	{
		ERROR3("Invalid path proportion");
		return FALSE;
	}
	if (m_pNodeBlend == NULL)
	{
		ERROR3("m_pNodeBlend is NULL");
		return FALSE;
	}
	// find out if this node is the first or last node of the blend
	BOOL NodeIsFirst;
	NodeBlender* pNodeBlender = GetBlenderAndPosition(pNodeToEdit, &NodeIsFirst);	
	if (pNodeBlender == NULL)
	{
		ERROR3("No NodeBlender");
		return FALSE;
	}
	// get the variables that we need to pass to CalculateNewNumSteps()
	double OtherDistance = 0.0;
	double ThisDistance = 0.0;
	double StartProp = 0.0;
	double EndProp = 1.0;
	UINT32 CurrentSteps = m_pNodeBlend->GetNumBlendSteps();
	UINT32 NewNumSteps = CurrentSteps;
	double BlendDistance = 0.0;
	
	BOOL Valid = m_pNodeBlend->GetBlendDistance(TRUE, &BlendDistance);
	if (Valid)
	{
		Valid = m_pNodeBlend->GetStartAndEndProportions(&StartProp, &EndProp);
		if (Valid)
		{
			// we must pass the distance of each object from their respective ends,
			// i.e. the distance of the end object is the distance from the object to
			// the end of the path.
			if (NodeIsFirst)
			{
				// have we reversed the order? If so then do it as an action
				if (NewPathProp >= EndProp)
				{	
					// Swap the ends
					Valid = InsertChangeProportion(NewPathProp, NodeIsFirst, pNodeBlender);
					// get the new proportions
					if (Valid) Valid = m_pNodeBlend->GetStartAndEndProportions(&StartProp, &EndProp);
					// work out the distances from the closest end to each object
					OtherDistance = BlendDistance * StartProp;
					ThisDistance = BlendDistance * (1-NewPathProp);
					// we have swapped ends
					NodeIsFirst = FALSE;
				}
				else
				{
					OtherDistance = BlendDistance * (1-EndProp);
					ThisDistance = BlendDistance * NewPathProp;
				}
			}
			else
			{
				if (NewPathProp <= StartProp)
				{
					Valid = InsertChangeProportion(NewPathProp, NodeIsFirst, pNodeBlender);
					if (Valid) Valid = m_pNodeBlend->GetStartAndEndProportions(&StartProp, &EndProp);	
					OtherDistance = BlendDistance * (1-EndProp);
					ThisDistance = BlendDistance * NewPathProp;
					NodeIsFirst = TRUE;
				}
				else
				{
					OtherDistance = BlendDistance * StartProp;
					ThisDistance = BlendDistance * (1-NewPathProp);
				}
			}
			Valid = CalculateNewNumStepsAndPosition(CurrentSteps, BlendDistance, 
													StepDistance, &ThisDistance, 
													&OtherDistance, &NewNumSteps);

			if (Valid)
			{
				double NewOtherProp = 0.0;
				
				// if we are editing the start node make the other distance go from the
				// start of the path
				if (NodeIsFirst)
					OtherDistance = BlendDistance - OtherDistance;
					
				NewOtherProp = OtherDistance / BlendDistance;

				Valid = DoInvalidateNodeRegion((NodeRenderableInk*)m_pNodeBlend,TRUE,FALSE);
				if (Valid)	Valid = (InvalidateBoundsAction::Init(this,&UndoActions,m_pNodeBlend,TRUE) != AC_FAIL);
				if (Valid)
				{
//					NodeBlender* pThisBlender = NULL;
					NodeBlender* pOtherBlender = NULL;
					Node* pOtherNode = NULL;
					if (NodeIsFirst)
					{
						pOtherBlender = m_pNodeBlend->FindLastBlender();
						if (pOtherBlender ==  NULL)
						{
							ERROR3("Couldn't find blender");
							return FALSE;
						}
						pOtherNode = pOtherBlender->GetNodeEnd();
					}
					else
					{
						pOtherBlender = m_pNodeBlend->FindFirstBlender();
						if (pOtherBlender ==  NULL)
						{
							ERROR3("Couldn't find blender");
							return FALSE;
						}
						pOtherNode = pOtherBlender->GetNodeStart();
					}

					if (pOtherNode == NULL)
					{
						ERROR3("Couldn't find other blend node");
						return FALSE;
					}
					// insert a change steps action if we need one
					if (NewNumSteps != CurrentSteps)
					{
						double DistanceEntered = m_pNodeBlend->GetDistanceEntered();
						ChangeBlendStepsAction* pStepAction;
						NodeRenderableInk * pInk = (NodeRenderableInk *)m_pNodeBlend;		
						Valid = ChangeBlendStepsAction::Init(this,&UndoActions,pInk,CurrentSteps,DistanceEntered, &pStepAction) != AC_FAIL;
						m_pNodeBlend->SetNumBlendSteps(NewNumSteps);
					}
					// insert change proportion for the node we edited
					Valid = InsertChangeProportion(NewPathProp, NodeIsFirst, pNodeBlender);
					
					// insert change for the node that was moved in order to retain constant step distance
					if (Valid) Valid = InsertChangeProportion(NewOtherProp, (!NodeIsFirst), pOtherBlender);
					
					// transform the edited node to its new position
					if (Valid) Valid = m_pNodeBlend->TransformNodeToPoint((NodeRenderableInk*)pNodeToEdit,&NewPosition,this,0.0);
					
					// transform the other node to its new position
					if (Valid)
					{	
						DocCoord NewPoint;
						double VoidParam;
						Valid = pOtherBlender->GetPointFromDistance(OtherDistance, &NewPoint, &VoidParam);
						if (Valid) Valid = m_pNodeBlend->TransformNodeToPoint((NodeRenderableInk*)pOtherNode, &NewPoint, this, 0.0);
					
						// find out if we are part of a bevel, shadow or contour, if so then we must regenerate
						Node* pController = m_pNodeBlend->GetParentController();
						if (pController != NULL)
							pController->RegenerateNode(this);

						if (Valid) Valid = DoInvalidateNodeRegion((NodeRenderableInk*)m_pNodeBlend,TRUE,FALSE);
						if (Valid) Valid = (InvalidateBoundsAction::Init(this,&UndoActions,m_pNodeBlend,TRUE) != AC_FAIL);
					
					}
						
				}
			}
		}
	}

	
	return Valid;


}


/********************************************************************************************

>	BOOL OpEditBlendEndObject::InsertChangeProportion(double NewProportion, BOOL First,
													Node* pNodeToEdit)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	17/9/99
	Inputs:		PathProportion - the new proportion along the path to move the end object to
				pNode - the blend object that is being edited
				
	Outputs:    pNodeToEdit - some members are altered by these actions			
	returns:    TRUE if all actions were correctly carried out, else FALSE
	Purpose:	inserts an action setting the proportion along the path variable to its new value 

***********************************************************************************************/
	
BOOL OpEditBlendEndObject::InsertChangeProportion(double NewProp, BOOL FirstNode, 
												  NodeBlender* pEditBlender)
{
	// insert the change blender action
	ChangeBlenderOpParam BlenderParam;
	ChangeBlendOpParam	 BlendParam;

	BOOL Valid;
	// lets see if we need to swap the ends
	if (FirstNode)
	{
		// if the new proportion of path is past the other end then 
		// we need to swap ends.
		if (NewProp > pEditBlender->GetProportionOfPathDistEnd())
		{
			BlenderParam.m_ChangeType = CHANGEBLENDER_SWAPENDS;
			Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
			
			if (Valid)
			{
				// set the new start proportion to the old end proportion
				BlenderParam.m_ChangeType = CHANGEBLENDER_PATHSTART;
				BlenderParam.m_NewPathStart = pEditBlender->GetProportionOfPathDistEnd();
				BlendParam.NewEndObject  = FIRST;
				Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
				
				if (Valid)
				{
					BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
					BlenderParam.m_NewPathEnd = NewProp;
					BlendParam.NewEndObject  = LAST;
					Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
				}
			}			

		}
		else
		{
			// just a regular end edit
			BlenderParam.m_ChangeType = CHANGEBLENDER_PATHSTART;
			BlenderParam.m_NewPathStart = NewProp;
			BlendParam.NewEndObject  = FIRST;
			Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
		}
	}
	else 
	{
		if (NewProp < pEditBlender->GetProportionOfPathDistStart())
		{
			BlenderParam.m_ChangeType = CHANGEBLENDER_SWAPENDS;
			Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);	
		
			if (Valid)
			{
				// set the new end proportion to the old start proportion
				BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
				BlenderParam.m_NewPathEnd = pEditBlender->GetProportionOfPathDistStart();
				BlendParam.NewEndObject  = LAST;
				Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
				
				if (Valid)
				{
					BlenderParam.m_ChangeType = CHANGEBLENDER_PATHSTART;
					BlenderParam.m_NewPathStart = NewProp;
					BlendParam.NewEndObject  = FIRST;
					Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
				}
			}			

		}
		else
		{
			// just a regular end edit
			BlenderParam.m_ChangeType = CHANGEBLENDER_PATHEND;
			BlenderParam.m_NewPathEnd = NewProp;
			BlendParam.NewEndObject  = LAST;
			Valid = ChangeBlenderAction::Init(this, &UndoActions, pEditBlender, BlenderParam);
		}
	}

	return Valid;
}




/********************************************************************************************

>	BOOL OpChangeBlendDistance::CalculateNewNumStepsAndPosition(UINT32 OldNumSteps, 
																double BlendDistance,
																double StepDistance
																double* FixedPosition
																double* MoveablePosition
																UINT32* NewNumSteps)
	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		OldNumSteps - the number of steps in the blend before any changes were applied
				BlendDistance - the length of the NodeBlendPath
				StepDistance  - the distance between steps that we wish to apply.
				FixedDistance - the distance from the end of the path of the object that we would
				like to keep fixed.  Note that it may not always remain fixed.
				MoveableDistance - the distance from the end of the path of the object that we are
				allowing to move.
				NOTE: FixedDistance and MoveableDistance should be measured from the end that they 
				represent.  That is to say if you wish to keep the first blend object fixed then FixedDistance
				should be the distance from the start of the path to the first blend object, MoveableDistance will
				then be the distance FROM THE OTHER END to the end object.

	Outputs:    NewNumSteps - the new number of steps required
				MoveableDistance - the new position of the end that we don't care about
				FixedDistance - in some rare instances we may have adjust the fixed position
				in order to accomodate the desired step distance
				is the last object.			
	returns:	TRUE if all went well, FALSE if the step distance gives and invalid number of steps.
	Purpose:	Works out the number of steps needed to accomodate the new step distance, as well as 
				working out the new positions of the end objects. 
				This is nearly identical to the function of the same name in OpChangeBlendDistance,
				the difference being that if the operation cannot be performed without moving the 
				fixed node then it will inform the user.
				
	See Also:	
***********************************************************************************************/
	
BOOL OpEditBlendEndObject::CalculateNewNumStepsAndPosition(UINT32 OldNumSteps, double BlendDistance,
														   double StepDistance, double* FixedDistance,
														   double* MoveableDistance, UINT32* NewNumSteps)
{
	// First check parameters
	if (OldNumSteps < 0 || StepDistance < 0)
	{
		ERROR3("Invalid parameter");
		return FALSE;
	}
	if (BlendDistance < (*FixedDistance + *MoveableDistance))
	{
		ERROR3("Invalid distance parameter");
		return FALSE;
	}

	//initialise to zero 
	*NewNumSteps = 0;

	// get the distance currently occupied by the blend
	double DistanceUsed = BlendDistance - (*FixedDistance + *MoveableDistance);

	// try to use all the space 
	UINT32 TempNumSteps = (UINT32)(DistanceUsed / StepDistance);

	if (TempNumSteps > 0)
	{
		*NewNumSteps = TempNumSteps;
		double NewDistance = TempNumSteps * StepDistance;
		*MoveableDistance = BlendDistance - (*FixedDistance + NewDistance);
	}
	// thats too long, try moving the other node all the way to the end
	else 
	{

		TempNumSteps = (UINT32)((BlendDistance - *FixedDistance)/ StepDistance);
		if (TempNumSteps > 0)
		{
			double NewDistance = TempNumSteps * StepDistance;
			*NewNumSteps = TempNumSteps;
			*MoveableDistance = BlendDistance - (*FixedDistance + NewDistance);
		}
		else
		{
			// still didn't work, just quit for now but maybe 
			// think about bringing up a dialog asking the user if they 
			// wish to go to edit steps mode.
			return FALSE;
		}
	}

	// test to see that what we have is ok.
	if (*NewNumSteps > 0)
		return TRUE;
	else
		return FALSE;
}





/********************************************************************************************

>	NodeBlender* OpEditBlendEndObject::GetBlenderAndPosition(Node* pNode, BOOL* pFirst)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		pNode - the blend object that is being edited
	Outputs:    pFirst -  TRUE if pNode is the first object of the blender, FALSE if it
				is the last object.			
	returns:    pointer to the nodeblender responsible for the blend being edited, or 
				NULL if invalid.
	Purpose:	To find the NodeBlender on a curve that uses pNode as either its first or last node, 
				and which position.  
				

***********************************************************************************************/
	


NodeBlender* OpEditBlendEndObject::GetBlenderAndPosition(Node* pEditNode, BOOL* pFirst)
{
	if (pEditNode == NULL)
	{
		ERROR3("Node is NULL");
		return NULL;
	}

	if (m_pNodeBlend == NULL)
	{
		ERROR3("m_pNodeBlend is NULL");
		return NULL;
	}

	NodeBlender* pBlender = m_pNodeBlend->FindFirstBlender();
	while (pBlender != NULL)
	{
		if (pBlender->IsBlendedOnCurve())
		{
			if (pBlender->GetNodeStart() == pEditNode)
			{
				*pFirst = TRUE;
				return pBlender;
			}
			else if (pBlender->GetNodeEnd() == pEditNode)
			{
				*pFirst = FALSE;
				return pBlender;
			}
			else
			{
				// also need to check for compound nodes ....

				if (pBlender->GetNodeStart ()->IsNodeInSubtree (pEditNode))
				{
					*pFirst = TRUE;
					return pBlender;
				}
				else	// check the end node
				{
					if (pBlender->GetNodeEnd ()->IsNodeInSubtree (pEditNode))
					{
						*pFirst = FALSE;
						return pBlender;
					}
				}
			}
		}
		pBlender = m_pNodeBlend->FindNextBlender(pBlender);
	}

	// we didn't find a nodeblender
	ERROR3("Couldn't find a nodeblender");
	return NULL;


}



/********************************************************************************************

>	BOOL OpEditBlendEndObject::GetClosetPointOnPath(DocCoord PointerPos, DocCoord* ClosestPoint)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
	Outputs:    ClosestPoint - the closest point on the nodeblendpath to pointerpos				
	returns:    TRUE if successful , FALSE otherwise
	Purpose:	To find the closest point on the nodeblend path to the point clicked or dragged on
				by the user.  The function gets the closest point on all the paths and takes the 
				closest. However this part is only done once as the nodeblendpath is then cached.

***********************************************************************************************/

BOOL OpEditBlendEndObject::GetClosestPointOnPath(DocCoord PointerPos, DocCoord* ClosestPoint)
{
	if (m_pNodeBlend == NULL)
	{
		ERROR3("There is no NodeBlend");
		return FALSE;
	}
	if (m_pNodeBlendPath == NULL)
	{
		UINT32 NBPCounter = 0;
		NodeBlendPath* pNodeBlendPath = m_pNodeBlend->GetNodeBlendPath(NBPCounter);
	
		if (pNodeBlendPath == NULL)
		{
			ERROR3("This blend has no nodeblendpath");
			return FALSE;
		}
	
		UINT32 ClosestIndex = 0;
		double ClosestDistance = 999999999999.;
		INT32 ClosestPath=0;
		double ClosestMu=0;
		while (pNodeBlendPath != NULL)
		{
			INT32 Index;
			double Mu;
			double SqrDistance = pNodeBlendPath->InkPath.SqrDistanceToPoint(PointerPos, &Index, &Mu);
			if (SqrDistance < ClosestDistance)
			{
				ClosestDistance = SqrDistance;
				ClosestPath = NBPCounter;
				ClosestIndex = Index;
				ClosestMu = Mu;
			}
			pNodeBlendPath = m_pNodeBlend->GetNodeBlendPath(++NBPCounter);
		}
		pNodeBlendPath = m_pNodeBlend->GetNodeBlendPath(ClosestPath);
		DocCoord PointOnLine = pNodeBlendPath->InkPath.ClosestPointTo(ClosestMu, ClosestIndex);
		// assign the point
		*ClosestPoint = PointOnLine;
		// assign the path to the member variable
		m_pNodeBlendPath = pNodeBlendPath; 
	}
	else
	{   // if we have already cached the nodeblendpath
		INT32 Index=0;
		double Mu=0;
		/*double SqrDistance = */m_pNodeBlendPath->InkPath.SqrDistanceToPoint(PointerPos, &Index, &Mu);
		DocCoord PointOnLine = m_pNodeBlendPath->InkPath.ClosestPointTo(Mu, Index);
		*ClosestPoint = PointOnLine;
	}

	return TRUE;



}
	



/********************************************************************************************

>	virtual void OpEditBlendEndObject::GetOpName(String_256* OpName) 

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	10/9/99
	Inputs:		OpName = ptr to str to place op name in
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpEditBlendEndObject::GetOpName(String_256 * OpName)
{
	String_256 test = "test string";


}



/********************************************************************************************

>	void OpEditBlendEndObjects::RenderDragBlobs(DocRect Rect,Spread* pSpread)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/99
	Inputs:		CentrePosition - the centre of the 'X' to be drawn
				pSpread - The spread that the drawing will happen on
				On - are we rendering the blobs on or off?
	Purpose:	Calls the XOR outline function and draws an 'X' in the position specified

********************************************************************************************/

void OpEditBlendEndObject::RenderDragBlobs(Spread* pSpread, DocCoord CentrePosition, BOOL On)
{
	if (pSpread == NULL)
	{
		ERROR3("No Spread");
		return;
	}
	if (m_pRange == NULL || m_pTransMatrix == NULL)
	{
		ERROR3("Null member variables");
		return;
	}

	Matrix NewMatrix;
	NewMatrix.SetTranslation(CentrePosition - m_StartCoord);
	
	if (On == FALSE)
		m_pRange->RenderXOROutlinesOff(NULL, pSpread, m_pTransMatrix);
	else
	{
		m_pRange->RenderXOROutlinesOn(NULL, pSpread, &NewMatrix, NULL);
		m_pTransMatrix->SetTranslation(CentrePosition - m_StartCoord);
	}
	
	//we'll render the 'X' directly so we need a render region
	
	RenderRegion* pRegion = DocView::RenderOnTop(NULL, pSpread, UnclippedEOR);
	while ( pRegion != NULL )
	{
		pRegion->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
	
					
		// Draw a blob at the centre point
		DocRect BlobSize;
		BlobManager* pBlobMgr = GetApplication()->GetBlobManager();
		if (pBlobMgr != NULL)
		{	
			pBlobMgr->GetBlobRect(CentrePosition, &BlobSize);
	
			pRegion->DrawLine(DocCoord(BlobSize.hi.x, BlobSize.hi.y), DocCoord(BlobSize.lo.x, BlobSize.lo.y));
			pRegion->DrawLine(DocCoord(BlobSize.lo.x, BlobSize.hi.y), DocCoord(BlobSize.hi.x, BlobSize.lo.y));
			pRegion->DrawPixel(DocCoord(BlobSize.hi.x, BlobSize.lo.y));
			pRegion->DrawPixel(DocCoord(BlobSize.lo.x, BlobSize.lo.y));
		}
		pRegion = DocView::GetNextOnTop(NULL);
	}
			
}


/********************************************************************************************

>	void OpEditBlendEndObjects::RenderSelectedObjectBlobs(Spread* pSpread)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/99
	Inputs:		pSpread - The spread that the drawing will happen on
				
	Purpose:	To render the object blobs of the objects that were in the selection during the
				drag op.  To dig myself out of the hole that I dug for myself in the implementaion
				of the drag op.  Basically if you read the comments in DoDrag() you will be
				aware that we've had to temporarily change the selection for the purposes of this drag.
				However this means that all the objects that should really be selected lose their 
				object blob.  in order to restore them we must go through the stored
				selection by hand and render them individually.  Awful, sorry.
	See also:   SelState.h/.cpp DoDrag()

********************************************************************************************/

void OpEditBlendEndObject::RenderSelectedObjectBlobs(Spread* pSpread)
{
	// variables we want so long as the selection state member is ok
	Node** pSelectionNodes = NULL;
	UINT32 NumNodes = 0; 
	SelNdRng* pSelectedRanges = NULL;
	UINT32 NumRanges = 0;

	// check out the member
	if (m_pSelState == NULL)
	{
		ERROR3("m_pSelState is NULL");
		return;
	}
	else
	{
		pSelectionNodes = m_pSelState->GetNodeList();
		NumNodes = m_pSelState->GetNumNodes();
		pSelectedRanges = m_pSelState->GetSelectionList();
		NumRanges = m_pSelState->GetNumRanges();
	}

	if ((NumNodes == 0 || pSelectionNodes[0] == NULL) && 
		(NumRanges == 0 || pSelectedRanges[0].FirstNode == NULL))
	{
		ERROR3("No selection nodes");
		EndDrag();
		return;
	}

	UINT32 i;
	for ( i = 0; i < NumRanges; i++)
	{	
		NodeRenderable* pNode = (NodeRenderable*)pSelectedRanges[i].FirstNode;
		for (UINT32 j = 0; j < pSelectedRanges[i].NumSelected; j++)
		{
			if (pNode == NULL)
			{
				ERROR3("Node is null");
				break;
			}

			RenderRegion* pOnTopRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
			while (pOnTopRegion)
			{
				pNode->RenderObjectBlobs(pOnTopRegion);

				// Go find the next region
				pOnTopRegion = DocView::GetNextOnTop(NULL);
			}
			pNode = (NodeRenderable*)pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
		}
	}


	for ( i = 0; i < NumNodes; i++)
	{	
		NodeRenderable* pNode = (NodeRenderable*)pSelectionNodes[i];
		RenderRegion* pOnTopRegion = DocView::RenderOnTop(NULL, pSpread, ClippedEOR);
		while (pOnTopRegion)
		{
			pNode->RenderObjectBlobs(pOnTopRegion);

			// Go find the next region
			pOnTopRegion = DocView::GetNextOnTop(NULL);
		}
	}

	
}



//IMPLEMENT_SIMPLE_MODULE( BlendModule, MODULEID_BLEND, BlendTool,
//							"Blend Tool", "To blend objects", "MarkN" );

