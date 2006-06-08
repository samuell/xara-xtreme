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
// Implementation of the blend tool

/*
*/

#include "camtypes.h"
#include "oilfiles.h"
#include "csrstack.h"
#include "docview.h"
//#include "markn.h"
#include "spread.h"
#include "nodepath.h"
#include "progress.h"
#include "nodeblnd.h"
#include "nodebldr.h"
//#include "simon.h"
#include "blobs.h"
//#include "blndres.h"
#include "objchge.h"
//#include "resource.h"
//#include "barsdlgs.h"
//#include "will.h"
#include "filltool.h"
#include "bubbleid.h"
#include "becomea.h"
#include "attrmap.h"
#include "ndbldpth.h"
#include "pathedit.h"
#include "keypress.h"
#include "vkextra.h"
#include "ezmodule.h"
#include "opbevel.h"
//#include "will2.h"
//#include "biasres.h"
#include "opcntr.h"
#include "cntrtool.h"
#include "ppbevel.h"
#include "nodetxts.h"	// for TextStory.
//#include "camvw.h"
#include "layer.h"
#include "ophist.h"
// the resources headers
//#include "cntres.h"
#include "gclips.h"

// for bevels & shadows
#include "nodecont.h"
#include "nodecntr.h"
#include "ncntrcnt.h"
//#include "rikdlg.h"
#include "nodemold.h"
#include "ndclpcnt.h"
#include "biasdlg.h"
#include "helpuser.h"
//#include "xshelpid.h"
//#include "helppath.h"
#include "dragmgr.h"
#include "lineattr.h"
#include "effects_stack.h"

DECLARE_SOURCE( "$Revision$" );
											 	
CC_IMPLEMENT_MEMDUMP(ContourTool,Tool_v1)
CC_IMPLEMENT_DYNCREATE(ContourInfoBarOp,InformationBarOp)
CC_IMPLEMENT_DYNCREATE(OpContourNodes,SelOperation)

// Must come after the last CC_IMPLEMENT.. macro
#define new CAM_DEBUG_NEW     

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* ContourTool::FamilyName	= _T("Contour Tools");
TCHAR* ContourTool::ToolName 	= _T("Contour Tool");
TCHAR* ContourTool::Purpose 	= _T("Contour manipulation");
TCHAR* ContourTool::Author 		= _T("David Mc");

// Init those other useful static vars
ContourInfoBarOp*	ContourTool::pContourInfoBarOp			= NULL;
BlendToolRef* 	ContourTool::pRefStart 				= NULL;
BlendToolRef* 	ContourTool::pRefEnd   				= NULL;
Cursor*			ContourTool::pcNormalCursor			= NULL;
Cursor*			ContourTool::pcOverBlob				= NULL;
Cursor*			ContourTool::pcCurrentCursor			= NULL;
INT32			ContourTool::CurrentCursorID			= 0;
UINT32			ContourTool::StatusID					= _R(IDS_CONTOURDRAGHELP);

OpContourNodes		*ContourTool::m_pOpContourNodes = NULL;

// maximum & minimum values for the contour slider
#define CONTOUR_DEPTH_MIN 10
#define CONTOUR_DEPTH_MAX 250000

//#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

#define SWAP(type,a,b) { type x=a; a=b; b=x; }

//////////////////////////////////////////////////////////////
// Flatness settings
const INT32 ContourToolFlatness=2048;

//////////////////////////////////////////////////////////////
// Profile for the logarithmic width slider
CProfileBiasGain SliderProfile((AFp)0.7, (AFp)0.0);


/********************************************************************************************

>	ContourTool::ContourTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Default Constructor.
				Other initialisation is done in ContourTool::Init which is called by the Tool Manager
	SeeAlso:	ContourTool::Init

********************************************************************************************/

ContourTool::ContourTool()
{
	pcCurrentCursor = NULL;
	m_bDisableBlobRenderingFlag = FALSE;
}

/********************************************************************************************

>	ContourTool::~ContourTool()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Destructor.

********************************************************************************************/

ContourTool::~ContourTool()
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

	if (m_pOpContourNodes)
	{
		delete m_pOpContourNodes;
		m_pOpContourNodes = NULL;
	}
}


/********************************************************************************************

>	BOOL ContourTool::Init( INT32 Pass )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	ContourTool::ContourTool

********************************************************************************************/

BOOL ContourTool::Init()
{
	// Declare all your ops here and only succeed if all declarations succeed
	BOOL ok =	OpContourNodes::Declare() &&
		OpChangeContourToInner::Declare() &&
		OpChangeContourToOuter::Declare() &&
		OpRemoveContour::Declare() &&
		OpChangeContourSteps::Declare() &&
		OpChangeContourColourType::Declare() &&
		OpChangeContourAttributeProfile::Declare() &&
		OpChangeContourObjectProfile::Declare() &&
		OpChangeContourStepDistance::Declare() &&
		OpToggleContourInsetPath::Declare();
	
	if (!ok) return FALSE;

	m_pOpContourNodes = new OpContourNodes;

	if (!m_pOpContourNodes)
		return FALSE;

	// We need two BlendToolRef objects
	ContourTool::pRefStart = new BlendToolRef;
	ContourTool::pRefEnd   = new BlendToolRef;

	ok = (ContourTool::pRefStart != NULL && ContourTool::pRefEnd != NULL);

	// This section reads in the infobar definition and creates an instance of
	// ContourInfoBarOp.  Also pContourInfoBarOp, the ptr to the tool's infobar, is set up
	// after the infobar is successfully read and created.
	if (ok)
	{
#if 0		
		CCResTextFile 		file;				// Resource File
		ContourInfoBarOpCreate BarCreate;			// Object that creates ContourInfoBarOp objects

//		 		ok = file.open(_R(IDM_BLEND_BAR), _R(IDT_INFO_BAR_RES));		// Open resource
				ok = file.open( _R(IDM_CONTOUR_BAR), _R(IDT_INFO_BAR_RES) );
		if (ok) ok = DialogBarOp::ReadBarsFromFile(file,BarCreate);	// Read and create info bar
		if (ok) file.close();									 	// Close resource

		ENSURE(ok,"Unable to load blendbar.ini from resource\n"); 

		if (ok)
		{
			// Info bar now exists.  Now get a pointer to it
			String_32 str = String_32(_R(IDS_CONTOURTOOL_INFOBARNAME));
			DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(str);

					ok = (pDialogBarOp != NULL);
			if (ok) ok = pDialogBarOp->IsKindOf(CC_RUNTIME_CLASS(ContourInfoBarOp));
			if (ok) pContourInfoBarOp = (ContourInfoBarOp*)pDialogBarOp;

			ENSURE(ok,"Error finding the blend tool info bar");
		}
#endif
		pContourInfoBarOp = new ContourInfoBarOp();
		ok = (pContourInfoBarOp != NULL);

	}

	return (ok);
}


/********************************************************************************************

>	void ContourTool::Describe(void *InfoPtr)

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

void ContourTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1 *Info = (ToolInfo_v1 *) InfoPtr;

	Info->InfoVersion = 1;
	
	Info->InterfaceVersion = GetToolInterfaceVersion();  // You should always have this line.
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_CONTOUR_TOOL);

	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;

	Info->BubbleID = _R(IDBBL_CONTOUR_TOOLBOX);
}

/********************************************************************************************

>	virtual void ContourTool::SelectChange(BOOL isSelected)

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

void ContourTool::SelectChange(BOOL isSelected)
{
// Stub out this function if the tool isn't wanted
#ifndef NO_ADVANCED_TOOLS
	if (isSelected)
	{
		if (!CreateCursors()) return;
		CurrentCursorID = CursorStack::GPush(pcNormalCursor, FALSE);		// Push cursor but don't display now
		pcCurrentCursor = pcNormalCursor;

		// Create and display the tool's info bar
		pContourInfoBarOp->Create();

		// Which blobs do I want displayed
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			// Decide which blobs we will display
			BlobStyle MyBlobs;
			MyBlobs.Object = FALSE;
			MyBlobs.Tiny = TRUE;
			// Tell the blob manager
			BlobMgr->ToolInterest(MyBlobs);
		}

		Document* pDoc = Document::GetCurrent();
		SetupToolBlobs();

		if (pDoc != NULL)
			RenderToolBlobs(pDoc->GetSelectedSpread(),NULL);

		pContourInfoBarOp->m_pTool = this;
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

PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
		pContourInfoBarOp->CloseProfileDialog (pContourInfoBarOp->m_BiasGainObjectGadget);
		pContourInfoBarOp->CloseProfileDialog (pContourInfoBarOp->m_BiasGainAttrGadget);
#endif

		// Remove the info bar from view by deleting the actual underlying window
		pContourInfoBarOp->Delete();

		// ensure any tool object blobs are removed.
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		if (BlobMgr != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			BlobMgr->RemoveInterest(bsRemoves);
		}

		Document* pDoc = Document::GetCurrent();
		if (pDoc != NULL)
			RenderToolBlobs(pDoc->GetSelectedSpread(),NULL);
	}
#endif	// NO_ADVANCED_TOOLS
}

/********************************************************************************************

>	void ContourTool::SetupToolBlobs()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	
	Purpose:	Sets up the tool blobs for the current selection.

				Karim 28/07/2000
				Contour tool blobs are laid out like so relative to the selection:

						2	4	3

						6	*	7

						0	5	1

				with corner blobs positioned 1 pixel out from the bounds rect
				and middle blobs positioned 2 pixels out from the bounds rect.

	SeeAlso:	-

********************************************************************************************/
void ContourTool::SetupToolBlobs()
{
	// reset the blob positions.
	for (INT32 i = 0 ; i < 8; i++)
	{
		m_BlobPoints[i].x = 0;
		m_BlobPoints[i].y = 0;
	}

	// give up if we there is no selection or we can't get the info we need.
	if (GetApplication()->FindSelection() == NULL)
		return;

	if (GetApplication()->FindSelection()->IsEmpty())
		return;

	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr == NULL)
		return;

	// figure out the blob offsets, in millipoints.
	INT32 BlobGap = 0;
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		BlobGap = pDocView->GetScaledPixelWidth().MakeLong();

	// ok, get the bounds of the selection, including contours.
	SelRange Sel(*(GetApplication()->FindSelection()));
	DocRect drBounds;
	Node* pN = Sel.FindFirst();
	while (pN != NULL)
	{
		if (pN->IsBounded())
		{
			Node* pParent = pN->FindParent();
			while (pParent != NULL && pParent->IS_KIND_OF(NodeContourController))
			{
				pN = pParent;
				pParent = pN->FindParent();
			}

			drBounds = drBounds.Union( ((NodeRenderableBounded*)pN)->GetBoundingRect() );
		}
		pN = Sel.FindNext(pN);
	}

	// lets set up those blobs!
	DocCoord dc;
	drBounds.Inflate(BlobGap);
	INT32 BlobSize = pBlobMgr->GetBlobSize();

	dc.x = drBounds.lo.x - BlobSize;
	dc.y = drBounds.lo.y - BlobSize;
	m_BlobPoints[0] = dc;

	dc.x = drBounds.hi.x + BlobSize;
	dc.y = drBounds.lo.y - BlobSize;
	m_BlobPoints[1] = dc;

	dc.x = drBounds.lo.x - BlobSize;
	dc.y = drBounds.hi.y + BlobSize;
	m_BlobPoints[2] = dc;

	dc.x = drBounds.hi.x + BlobSize;
	dc.y = drBounds.hi.y + BlobSize;
	m_BlobPoints[3] = dc;

	dc.x = (drBounds.lo.x + drBounds.hi.x) / 2;
	dc.y = drBounds.hi.y + BlobSize + BlobGap;
	m_BlobPoints[4] = dc;

	dc.x = (drBounds.lo.x + drBounds.hi.x) / 2;
	dc.y = drBounds.lo.y - BlobSize - BlobGap;
	m_BlobPoints[5] = dc;

	dc.x = drBounds.lo.x - BlobSize - BlobGap;
	dc.y = (drBounds.lo.y + drBounds.hi.y) / 2;
	m_BlobPoints[6] = dc;

	dc.x = drBounds.hi.x + BlobSize + BlobGap;
	dc.y = (drBounds.lo.y + drBounds.hi.y) / 2;
	m_BlobPoints[7] = dc;
}



/********************************************************************************************

>	BOOL ContourTool::CreateCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if all the blend tool cursors have been successfully created
	Purpose:	Creates all the blend tool cursors
	SeeAlso:	-

********************************************************************************************/

BOOL ContourTool::CreateCursors()
{
	// This tool has just been selected.  Create the cursors.
	pcNormalCursor 	  		= new Cursor(this, _R(IDC_POINTER_CONTOUR));
	pcOverBlob				= new Cursor(this, _R(IDC_SELECT_CONTOUR));
	
	if ( pcNormalCursor       	==NULL || !pcNormalCursor->IsValid() ||
		pcOverBlob== NULL || !pcOverBlob->IsValid()
	   )
	{
		DestroyCursors();
		return FALSE;
	}
	else
		return TRUE;
}

/********************************************************************************************

>	BOOL ContourTool::IsPointOverBlob(DocCoord &Point, DocRect * pBlobRect)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		A point in document coordinates, and the doc rect variable to
				put the blob rect into
	Outputs:	-
	Returns:	TRUE if the point is over a blob, false otherwise
	Purpose:	Creates all the blend tool cursors
	SeeAlso:	-

********************************************************************************************/
BOOL ContourTool::IsPointOverBlob(DocCoord &Point, DocRect * pBlobRect)
{
	// check to see if I'm over a blob
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	INT32 BlobSize = 0;

	// Karim 18/09/2000
	// The contour blobs are about twice the size of normal blobs,
	// so I'm doubling the value of BlobSize when checking for them.
	if (pBlobMgr)
		BlobSize = pBlobMgr->GetBlobSize();
//		BlobSize = pBlobMgr->GetBlobSize()/2;

	for (INT32 i = 0; i < 8; i++)
	{
		DocRect dr(	m_BlobPoints[i].x - BlobSize,
					m_BlobPoints[i].y - BlobSize,
					m_BlobPoints[i].x + BlobSize,
					m_BlobPoints[i].y + BlobSize);

		if (dr.ContainsCoord(Point))
		{
			if (pBlobRect)
				*pBlobRect = dr;
			return TRUE;
		}
	}

	return FALSE;
}

/********************************************************************************************

>	void ContourTool::DestroyCursors()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destroys all the blend tool cursors
	SeeAlso:	-

********************************************************************************************/

void ContourTool::DestroyCursors()
{
	if (pcNormalCursor 	  	  	!= NULL) delete pcNormalCursor;
	if (pcOverBlob				!= NULL) delete pcOverBlob;

	pcNormalCursor = NULL;
	pcOverBlob = NULL;
}

/********************************************************************************************

>	BOOL ContourTool::OnKeyPress(KeyPress* pKeyPress)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		pKeyPress - pointer to a keypress object
	Returns:	TRUE if it handled the keypress, FALSE otherwise
	Purpose:	To handle keypress events for the Blend Tool.

********************************************************************************************/

BOOL ContourTool::OnKeyPress(KeyPress* pKeyPress)
{
#ifdef _DEBUG
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

>	void ContourTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse button was clicked
				Click 		-	Describes the type of click that was detected. 
				ClickMods 	-	Indicates which buttons caused the click and which modifers were
								pressed at the same time
				pSpread 	- 	The spread in which the click happened
	Returns:	-
	Purpose:	To handle a Mouse Click event for the Blend Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void ContourTool::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
						Spread* pSpread )
{
// Stub out this function if the tool isn't wanted
#ifndef NO_ADVANCED_TOOLS
	if (ClickMods.Menu) return;							// Don't do anything if the user clicked the Menu button

	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

//	NodeCompound * pCompound = NULL;

	DocRect BlobRect;

	switch (Click)
	{
		// if a drag was started, we alter or create a contour.
		case CLICKTYPE_DRAG:
		{
			if (m_pOpContourNodes)
			{
				m_pOpContourNodes->DoDrag(this, pContourInfoBarOp, PointerPos,
					IsPointOverBlob(PointerPos, &BlobRect), &BlobRect);
				return;
			}
		}
		break;

		// we ignore all other click types, but note that they aren't drag-clicks.
		case CLICKTYPE_SINGLE:
		{
			// check for bevels existing
			List BevelList;
			BevelTools::BuildListOfSelectedNodes(&BevelList, CC_RUNTIME_CLASS(NodeBevelController));
			
			if (!BevelList.IsEmpty())
			{
				BevelList.DeleteAll();
				
				DocRect BlobRect;

				// disable the drag
				if (IsPointOverBlob(PointerPos, &BlobRect))
				{
					DocView * pView = DocView::GetCurrent();

					if (pView)
						pView->EndDrag(NULL);				
					
					InformWarningBevelExistsInSelection();
				}
			}

			// call the base class ....

			DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
		}
		break;
		
		default:
			// call the base class ....
			
			DragTool::OnClick (PointerPos, Click, ClickMods, pSpread);
		break;
	}
#endif	// NO_ADVANCED_TOOLS
}

/********************************************************************************************

>	void ContourTool::InformWarningBevelExistsInSelection()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/2000
	Purpose:	Brings up the associated warning dialog
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/
void ContourTool::InformWarningBevelExistsInSelection()
{
	String_256 QueryString(_R(IDS_CANTCONTOURBEVEL));

	Error::SetError(0, QueryString, 0);				
	// The only way of bringing up a box with a string in it
	INT32 DlgResult = InformError(_R(IDS_CANTCONTOURBEVEL),
		_R(IDS_OK), _R(IDS_HELP));
				
	if (DlgResult == 2)
	{
		HelpUserTopic(_R(IDS_HELPPATH_Message__Bevel_already_applied));
	}
}


/********************************************************************************************

>	void ContourTool::OnMouseMove( DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		PointerPos 	- 	The DocCoord of the point where the mouse has moved to
				pSpread 	- 	The spread in which the move occurred
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	To handle a Mouse Move event for the Blend Tool.
	SeeAlso:	Tool::MouseClick; ClickType; ClickModifiers

********************************************************************************************/

void ContourTool::OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods)
{
// Stub out this function if the tool isn't wanted
#ifndef NO_ADVANCED_TOOLS
	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

//	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();
//	INT32 BlobSize = pBlobMgr->GetBlobSize();

	if (IsPointOverBlob(PointerPos))
	{
		// change the cursor
		if (pcCurrentCursor != pcOverBlob )
		{
			if (pcCurrentCursor)
			{
				CursorStack::GPop(CurrentCursorID);
			}
			
			CurrentCursorID = CursorStack::GPush(pcOverBlob, TRUE);
			pcCurrentCursor = pcOverBlob;
			DisplayStatusBarHelp(_R(IDS_CONTOUROVERBLOBHELP));
		}
	}
	else
	{
		// change the cursor back
		if (pcCurrentCursor != pcNormalCursor )
		{
			if (pcCurrentCursor)
			{
				CursorStack::GPop(CurrentCursorID);
			}
			
			CurrentCursorID = CursorStack::GPush(pcNormalCursor, TRUE);
			pcCurrentCursor = pcNormalCursor;
			DisplayStatusBarHelp(_R(IDS_CONTOURDRAGHELP));
		}
	}
#endif	// NO_ADVANCED_TOOLS
}

/********************************************************************************************
>	void ContourTool::UpdateRef(	BlendToolRef* pRef,
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

void ContourTool::UpdateRef(BlendToolRef* pRef,Spread* pSpread, DocCoord PointerPos,BOOL CheckNodeUnderPoint)
{
	ERROR3IF_PF(pRef   ==NULL,("pRef is NULL"));
	ERROR3IF_PF(pSpread==NULL,("pSpread is NULL"));

	// Set the spread and pointer pos members
	pRef->pSpread    = pSpread;
	pRef->PointerPos = PointerPos;
}

/********************************************************************************************
>	void ContourTool::UpdateCursorAndStatus()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This will update the cursor and status line text depending on the data in the 
				two blend tool references within ContourTool.

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

void ContourTool::UpdateCursorAndStatus()
{
	ERROR3IF_PF(pRefStart==NULL,("pRefStart is NULL"));
	ERROR3IF_PF(pRefEnd  ==NULL,("pRefEnd   is NULL"));

	DisplayStatusBarHelp(StatusID);
}

/********************************************************************************************

>	virtual BOOL ContourTool::GetStatusLineText(	String_256* ptext, 
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

BOOL ContourTool::GetStatusLineText(String_256* pStr,Spread* pSpread,DocCoord DocPos,ClickModifiers ClickMods)
{
	if (StatusID != 0)
	{
		*pStr = String_256(StatusID);
	}
	else
	{
		*pStr = String_256(_R(IDS_CONTOURDRAGHELP));
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL ContourTool::IsPointOverPathBlob(DocCoord* pPointerPos,BlendToolRef* pRef)

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

BOOL ContourTool::IsPointOverPathBlob(DocCoord* pPointerPos,BlendToolRef* pRef)
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

>	BOOL ContourTool::IsPointOverBlendBlob(DocCoord* pPointerPos,NodeRenderableInk** ppNodePath,INT32* pIndex)

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

BOOL ContourTool::IsPointOverBlendBlob(DocCoord* pPointerPos,BlendToolRef* pRef)
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

>	void ContourTool::CheckNodeRemapping(BlendToolRef* pRefStart, BlendToolRef* pRefEnd)

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

void ContourTool::CheckNodeRemapping(BlendToolRef* pRefStart, BlendToolRef* pRefEnd)
{
	ERROR3IF(pRefStart == NULL,"pRefStart == NULL");
	ERROR3IF(pRefEnd   == NULL,"pRefEnd == NULL");
}

/********************************************************************************************
>	NodeRenderableInk* ContourTool::FindObject(Spread* pSpread, DocCoord PointerPos)

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

NodeRenderableInk* ContourTool::FindObject(Spread* pSpread, DocCoord PointerPos)
{
	return NodeRenderableInk::FindCompoundAtPoint(pSpread,PointerPos,NULL);
}

/********************************************************************************************

>	void ContourTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/2000
	Inputs:		pSpread		ptr to a spread.
				pDocRect	ptr to DocRect of spread to render in.
	Returns:	-
	Purpose:	Handles the RenderToolBlobs method.
				Renders the tool's blobs into the current doc view.

				In case you were wondering, this is how contour blobs are arranged:

					2	4	3

					6	*	7

					0	5	1
	SeeAlso:	

********************************************************************************************/

void ContourTool::RenderToolBlobs(Spread* pSpread,DocRect* pDocRect)
{
//	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	if (m_bDisableBlobRenderingFlag)
		return;

	// check the blobs to see if they're valid
	BOOL bZeroBlobs = TRUE;

	for (UINT32 i = 0 ; i < 8; i++)
	{
		if (m_BlobPoints[i].x != 0 ||
			m_BlobPoints[i].y != 0)
		{
			bZeroBlobs = FALSE;
			break;
		}
	}

	if (bZeroBlobs)
		return;
	
	DocView* pDocView = DocView::GetCurrent();

	// Get a render region on the spread.  We need to render EORd stuff on top of the current view
	RenderRegion* pRender = pDocView->RenderOnTop(pDocRect,pSpread,UnclippedEOR);

	while (pRender != NULL)
	{
		pRender->SaveContext();

		// left and right.
		pRender->DrawBitmapBlob(m_BlobPoints[6], _R(IDBMP_CONTOUR_LEFTRIGHT));
		pRender->DrawBitmapBlob(m_BlobPoints[7], _R(IDBMP_CONTOUR_LEFTRIGHT));

		// up and down.
		pRender->DrawBitmapBlob(m_BlobPoints[4], _R(IDBMP_CONTOUR_UPDOWN));
		pRender->DrawBitmapBlob(m_BlobPoints[5], _R(IDBMP_CONTOUR_UPDOWN));

		// acute accent (bl to tr).
		pRender->DrawBitmapBlob(m_BlobPoints[0], _R(IDBMP_CONTOUR_ACUTE));
		pRender->DrawBitmapBlob(m_BlobPoints[3], _R(IDBMP_CONTOUR_ACUTE));

		// grave accent (tl to br).
		pRender->DrawBitmapBlob(m_BlobPoints[2], _R(IDBMP_CONTOUR_GRAVE));
		pRender->DrawBitmapBlob(m_BlobPoints[1], _R(IDBMP_CONTOUR_GRAVE));

		pRender->RestoreContext();

		// get the next render region
		pRender = pDocView->GetNextOnTop(pDocRect);
	}
}

/********************************************************************************************

>	static void ContourTool::DisplayStatusBarHelp(UINT32 StatusID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		StatusID = ID of status help string
	Outputs:	-
	Returns:	-
	Purpose:	Displays the given status help string in the status bar
	SeeAlso:	-

********************************************************************************************/

void ContourTool::DisplayStatusBarHelp(UINT32 StatusIDX)
{
	String_256 StatusMsg("");
	StatusMsg.Load(StatusIDX);
	GetApplication()->UpdateStatusBarText(&StatusMsg);
	ContourTool::StatusID = StatusIDX;
}

/********************************************************************************************

>	static BOOL ContourTool::IsCurrent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the blend tool is the current tool
	Purpose:	Is the blend tool the current one? Call this to find out.
	SeeAlso:	-

********************************************************************************************/

BOOL ContourTool::IsCurrent()
{
	return (Tool::GetCurrentID() == TOOLID_CONTOURTOOL);
}

/********************************************************************************************

>	static INT32 ContourTool::CalculateContourWidth(DocRect &br, DocCoord &PointerPos)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/99
	Inputs:		The doc rect to calculate the width from, and the pointer position
	Outputs:	-
	Returns:	The width of the contour
	Purpose:	Calculates the contour width for a given rect
	SeeAlso:	-

********************************************************************************************/
INT32 ContourTool::CalculateContourWidth(DocRect &br, DocCoord &PointerPos)
{
	DocCoord Centre = br.Centre();

	INT32 Width = 0;

	INT32 OffsetX = 0;
	INT32 OffsetY = 0;

	double dOffsetX = 0;
	double dOffsetY = 0;

	// try to work out the width of the contour
	// first, are we dragging inside or outside the bounding rect ?
	if (!br.ContainsCoord(PointerPos))
	{
		// outside the bounding rect
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
		
		Width = -Width;
	}
	else
	{
		// inside the bounding rect then

		// work out which quadrant to access
		dOffsetX = ((double)(PointerPos.x - Centre.x));
		dOffsetY = ((double)(PointerPos.y - Centre.y));

		INT32 RWidth = br.Width()/2;
		INT32 RHeight = br.Height()/2;

		if (RWidth > RHeight)
		{
			INT32 DistX = RWidth - RHeight;

			if (PointerPos.x < Centre.x + DistX &&
				PointerPos.x > Centre.x - DistX)
			{
				if (dOffsetY > 0)
				{
					Width = br.hi.y - PointerPos.y;
				}
				else
				{
					Width = PointerPos.y - br.lo.y;
				}
			}
			else if (PointerPos.x > Centre.x + DistX)
			{
				dOffsetX -= (double)DistX;

				if (dOffsetY > 0)
				{
					if (dOffsetX > dOffsetY)
					{
						Width = br.hi.x - PointerPos.x;
					}
					else
					{
						Width = br.hi.y - PointerPos.y;
					}
				}
				else
				{
					if (dOffsetX > -dOffsetY)
					{
						Width = br.hi.x - PointerPos.x;
					}
					else
					{
						Width = PointerPos.y - br.lo.y;
					}
				}
			}
			else
			{
				dOffsetX += (double)DistX;
				dOffsetX = -dOffsetX;

				if (dOffsetY > 0)
				{
					if (dOffsetX > dOffsetY)
					{
						Width = PointerPos.x - br.lo.x;
					}
					else
					{
						Width = br.hi.y - PointerPos.y;
					}
				}
				else
				{
					if (dOffsetX > -dOffsetY)
					{
						Width = PointerPos.x - br.lo.x;
					}
					else
					{
						Width = PointerPos.y - br.lo.y;
					}
				}
			}
		}
		else
		{
			INT32 DistY = RHeight - RWidth;

			if (PointerPos.y < Centre.y + DistY &&
				PointerPos.y > Centre.y - DistY)
			{
				if (dOffsetX > 0)
				{
					Width = br.hi.x - PointerPos.x;
				}
				else
				{
					Width = PointerPos.x - br.lo.x;
				}
			}
			else if (PointerPos.y > Centre.y + DistY)
			{
				dOffsetY -= (double)DistY;

				if (dOffsetX > 0)
				{
					if (dOffsetY > dOffsetX)
					{
						Width = br.hi.y - PointerPos.y;
					}
					else
					{
						Width = br.hi.x - PointerPos.x;
					}
				}
				else
				{
					if (dOffsetY > -dOffsetX)
					{
						Width = br.hi.y - PointerPos.y;
					}
					else
					{
						Width = PointerPos.x - br.lo.x;
					}
				}
			}
			else
			{
				dOffsetY += (double)DistY;
				dOffsetY = -dOffsetY;

				if (dOffsetX > 0)
				{
					if (dOffsetY > dOffsetX)
					{
						Width = PointerPos.y - br.lo.y;
					}
					else
					{
						Width = br.hi.x - PointerPos.x;
					}
				}
				else
				{
					if (dOffsetY > -dOffsetX)
					{
						Width = PointerPos.y - br.lo.y;
					}
					else
					{
						Width = PointerPos.x - br.lo.x;
					}
				}
			}
		}
	}

	// check for max's and min's
	if (Width > 0)
	{
		if (Width > CONTOUR_DEPTH_MAX)
			Width = CONTOUR_DEPTH_MAX;

		if (Width < CONTOUR_DEPTH_MIN)
			Width = CONTOUR_DEPTH_MIN;
	}
	else
	{
		if (Width < -CONTOUR_DEPTH_MAX)
			Width = -CONTOUR_DEPTH_MAX;

		if (Width > -CONTOUR_DEPTH_MIN)
			Width = -CONTOUR_DEPTH_MIN;
	}
	
	return Width;
}




/********************************************************************************************

>	static INT32 ContourTool::CalculateContourWidth(Node * pNode, DocCoord &PointerPos)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/99
	Inputs:		-
	Outputs:	-
	Returns:	The width of the contour
	Purpose:	Calculates the contour width for a given node given a pointer position
	SeeAlso:	-

********************************************************************************************/

INT32 ContourTool::CalculateContourWidth(Node * pNode, DocCoord &PointerPos)
{
	// get the bounding rect of the node to be contoured
	DocRect br = ((NodeRenderableBounded *)pNode)->GetBoundingRect();

	Node * pParent = pNode;

	BOOL bContourExists = FALSE;

	while (pParent && !bContourExists)
	{
		if (pParent->IsKindOf(CC_RUNTIME_CLASS(NodeContourController)))
		{
			pNode = pParent;
			bContourExists = TRUE;

			br = ((NodeContourController *)pNode)->GetInsideBoundingRect();
		}

		pParent = pParent->FindParent();
	}

	return CalculateContourWidth(br, PointerPos);

	
}

//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
BOOL ContourInfoBarOp::CheckForBevels()
{
	List BevelList;
	BevelTools::BuildListOfSelectedNodes(&BevelList, CC_RUNTIME_CLASS(NodeBevelController));
	
	if (!BevelList.IsEmpty())
	{
		BevelList.DeleteAll();
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	MsgResult ContourInfoBarOp::Message(Msg* Message) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94  changed 13/9/99 Diccon Yamanaka
	Inputs:		Message = The message to handle
	Outputs:	-
	Returns:	-
	Purpose:	Blend info bar dialog message handler
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult ContourInfoBarOp::Message(Msg* Message) 
{
	CProfileBiasGain Profile;
//	BOOL bMany = FALSE;
	UINT32 Width = 0;
	INT32 LWidth = 0;
	
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{

			case DIM_CREATE:
				m_BetweenViews = FALSE;
				// Initialise the infobar controls here
				// This is sent when you create the infobar in your tool startup code
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
				m_BiasGainObjectGadget.LinkControlButton ( this, _R(IDC_CONTOUROBJECTBIASGAIN), _R(IDBBL_CONTOUROBJECTBIASGAIN), _R(IDS_CONTOUROBJECTBIASGAIN) );
				m_BiasGainAttrGadget.LinkControlButton ( this, _R(IDC_CONTOURATTRBIASGAIN), _R(IDBBL_CONTOURATTRBIASGAIN), _R(IDS_CONTOURATTRBIASGAIN) );
				m_BiasGainAttrGadget.ToggleFillProfile ();
#endif
				DisallowInteractiveProfiles ();
				
				SetGadgetHelp(_R(IDC_BTN_CONTOURSTEPS), _R(IDBBL_CONTOURSTEPSEDIT), _R(IDS_CONTOURSTEPSEDIT));

				// set the slider bitmap
				SetGadgetBitmaps(_R(IDC_CONTOURSLIDER), _R(IDB_SLIDERBASE), _R(IDB_SLIDERSLIDER));
				
				// these two buttons different buttons for their selected and 
				// SELECTED states, set them here.
				UpdateState();
				break;

			case DIM_LFT_BN_CLICKED:
			{
				if (FALSE) {}
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
				else if (Msg->GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN))
				{
					HandleProfileButtonClick (m_BiasGainObjectGadget, _R(IDC_CONTOUROBJECTBIASGAIN));
				}
				else if (Msg->GadgetID == _R(IDC_CONTOURATTRBIASGAIN))
				{
					HandleProfileButtonClick (m_BiasGainAttrGadget, _R(IDC_CONTOURATTRBIASGAIN));
				}
#endif
				else if (Msg->GadgetID == _R(IDC_REMOVECONTOUR))
				{
					OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_REMOVECONTOUR);
					if (pOpDesc != NULL) pOpDesc->Invoke();
				}
				/* DY 13/9/99 buttons to select whether blend steps or blend distance
					is to be edited in the edit box */
				else if (Msg->GadgetID == _R(IDC_BTN_CONTOURDISTANCE))
				{
					// set the steps control	
					SetLongGadgetValue(_R(IDC_BTN_CONTOURSTEPS), FALSE);
					m_EditBlendSteps = FALSE;
					UpdateState();      // tell the infobar something has changed
				}
				else if (Msg->GadgetID == _R(IDC_BTN_CONTOURSTEPS))
				{	
					// set the distance control
					SetLongGadgetValue(_R(IDC_BTN_CONTOURDISTANCE), FALSE);
					m_EditBlendSteps = TRUE;
					UpdateState();  
				}
				else if (Msg->GadgetID == _R(IDC_BTN_CONTOUROUTER))
				{
					// deal with the button click
					DealWithContourOuterClick();
				}
				else if (Msg->GadgetID == _R(IDC_BTN_CONTOURINNER))
				{
					DealWithContourInnerClick();
				}
				else if (Msg->GadgetID == _R(IDC_BTN_INSETPATH))
				{
					DealWithInsetPathClick();
				}
				else if (Msg->GadgetID == _R(IDC_CONTOURJOINTYPEMITRE))
				{
					if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
					{
						HandleJoinTypeMitreClicked();
					}
				}
				else if (Msg->GadgetID == _R(IDC_CONTOURJOINTYPEROUND))
				{
					if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
					{
						HandleJoinTypeRoundClicked();
					}
				}
				else if (Msg->GadgetID == _R(IDC_CONTOURJOINTYPEBEVEL))
				{
					if (Msg->DlgMsg == DIM_LFT_BN_CLICKED)
					{
						HandleJoinTypeBevelClicked();
					}
				}
			}
			break;

			case DIM_SLIDER_POS_SET:
			{
				if (Msg->GadgetID == _R(IDC_CONTOURSLIDER))
				{
					// slider has been dropped therefore invoke the op
					Width = GetSliderLogValue();

					// get rid of the drag blobs
					ContourTool::m_pOpContourNodes->RenderDragBlobs(m_LastDragWidth, 
							Document::GetSelectedSpread(), TRUE);	
					
					m_bDragging = FALSE;

					DealWithWidthChange(Width);
				}
			}
			break;
			
			case DIM_SLIDER_POS_CHANGING:

				// deal with the slider being dragged about
				if (Msg->GadgetID == _R(IDC_CONTOURSLIDER))
				{
					// slider changed !
					// get the slider value & put it into the edit box
					Width = GetSliderLogValue();

					SetSliderValue(Width, FALSE);

					if (ContourTool::m_pOpContourNodes)
					{
						if(m_bOuterIsSelected)
							Width = -Width;

						if (!m_bDragging)
						{
							// set up the drag information for the blob rendering
							ContourTool::m_pOpContourNodes->SetupDragInfo();
							m_bDragging = TRUE;
						}
						else
						{
							// render drag blobs off
							ContourTool::m_pOpContourNodes->RenderDragBlobs(m_LastDragWidth,Document::GetSelectedSpread());
						}

						// render drag blobs with the new width
						ContourTool::m_pOpContourNodes->RenderDragBlobs(Width,Document::GetSelectedSpread());

						// set the last drag width
						m_LastDragWidth = Width;
					}
				}
				break;
			
			case DIM_SELECTION_CHANGED:
			{
				if (FALSE) {}
				else if (Msg->GadgetID == _R(IDC_CONTOURSTEPS))
				{
					BOOL Valid = TRUE;

					// get the correct number of steps depending on the buttons which are
					// activated (either step width or number of steps)
					if (GetBoolGadgetSelected(_R(IDC_BTN_CONTOURSTEPS)))
					{						
						INT32 NumSteps = GetLongGadgetValue(_R(IDC_CONTOURSTEPS),0,999,_R(IDS_BLENDSTEPS_INVALID),&Valid);

						if (NumSteps <= 0)
							NumSteps = 1;

						NumSteps --;

						if (Valid)
						{
							OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOURSTEPS);
							if (pOpDesc != NULL)
							{
								OpParam Param(NumSteps,0);
								pOpDesc->Invoke(&Param);
							}
						}
					}
					else if (GetBoolGadgetSelected(_R(IDC_BTN_CONTOURDISTANCE)))
					{
						// convert to units
						INT32 Dist = 0;
						Valid = ConvertStringToValue(_R(IDC_CONTOURSTEPS), Dist);

						// Make sure the value is suitable, ie. greater than a pixel or 750 MILLIPOINTS
						if(Dist <= 0)
						{
							InformWarning(_R(IDS_CONTOUR_DISTANCE_WARNING));
						}
						else if(Valid)
						{
							OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOURSTEPDISTANCE);
							if (pOpDesc != NULL)
							{
								OpParam Param(Dist,0);
								pOpDesc->Invoke(&Param);
							}
						}				
					}
				}
				else if (Msg->GadgetID == _R(IDC_EFFECT))
				{
					WORD Index;
					GetValueIndex(_R(IDC_EFFECT),&Index); 
					ColourBlendType ColBlendType = (ColourBlendType)Index;

					if (ColBlendType <= COLOURBLEND_ALTRAINBOW)
					{
						OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOURCOLOURTYPE);
						if (pOpDesc != NULL)
						{
							OpParam Param;
							Param.Param1 = (UINT32)ColBlendType;
							pOpDesc->Invoke(&Param);
						}
					}
				}
				else if (Msg->GadgetID == _R(IDC_CONTOURSLIDEREDIT))
				{
					// get the value from the editable control
					BOOL Valid = ConvertStringToValue(_R(IDC_CONTOURSLIDEREDIT), LWidth);

					// make sure that the specified contour width is a valid value.
					// Make the lowest value a pixel or 750 MILLIPOINTs
					if(LWidth <= 0)
					{
						InformWarning(_R(IDS_CONTOUR_WIDTH_WARNING));
					}
					else if(Valid)
					{
						// ok, deal with this change
						if (LWidth > 0)
						{
							Width = (UINT32)LWidth;

							DealWithWidthChange(Width);
						}
					}
				}
			}
			break;

			default:
				break;

		}

		if (( Msg->GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN) ) || ( Msg->GadgetID == _R(IDC_CONTOURATTRBIASGAIN) ))
		{  
			ProfileSelectionChange( Msg, Msg->GadgetID );
		}
	}
	else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		UpdateState();
		Spread* pSelected = Document::GetSelectedSpread();

		// render the tool blobs off
		if (m_pTool && pSelected && ContourTool::IsCurrent() && !m_BetweenViews)
		{
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE(BlobMgr, "Can't get BlobManager");
			BlobMgr->RenderToolBlobsOff(m_pTool, pSelected, NULL);
			m_pTool->SetupToolBlobs();
			BlobMgr->RenderToolBlobsOn(m_pTool, pSelected, NULL);

			TRACEUSER( "MarkH", _T("Selected Spread = 0x%x\n"),pSelected);
		}

PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
		HandleProfileSelChangingMsg (m_BiasGainObjectGadget, _R(IDC_CONTOUROBJECTBIASGAIN));
		HandleProfileSelChangingMsg (m_BiasGainAttrGadget, _R(IDC_CONTOURATTRBIASGAIN));
#endif
	}
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		DocViewMsg* msg = (DocViewMsg*) Message;

		// Render the tool blobs off the old view just before it changes
		if ((msg->State == DocViewMsg::SELABOUTTOCHANGE || msg->State == DocViewMsg::SELCHANGED ) 
			&& msg->pOldDocView && m_pTool && ContourTool::IsCurrent() && !GetApplication()->CamelotIsDying())
		{
			// Do additional checks here to avoid ensures!
			CCamView* pOilView = msg->pOldDocView->GetConnectionToOilView();
			BlobManager* BlobMgr = GetApplication()->GetBlobManager();
			ENSURE(BlobMgr, "Can't get BlobManager");

			PORTNOTE("other", "Removed IsScreenCamView test");
			if (pOilView!=NULL /*&& pOilView->IsScreenCamView()*/)
			{
				// handle the view will/has changed msgs and do NOT process and selection changes msgs between times (sjk 22/11/00)
				if (msg->State == DocViewMsg::SELABOUTTOCHANGE)
				{
					m_pTool->SetupToolBlobs();
					BlobMgr->RenderToolBlobsOff(m_pTool, msg->pOldDocView->GetVisibleSpread(), NULL);
					//TRACE( _T("View about to change\n"));
					m_BetweenViews = TRUE;
				}
				else
				{
					m_pTool->SetupToolBlobs();
					BlobMgr->RenderToolBlobsOn(m_pTool, msg->pOldDocView->GetVisibleSpread(), NULL);
					//TRACE( _T("View has just changed\n"));
					m_BetweenViews = FALSE;
				}
			}

		}
	}
	else if (MESSAGE_IS_A(Message,OpMsg)) 	// Check for undo/redo
	{
		// update the gadgets
		UpdateState();
		BlobManager* BlobMgr = GetApplication()->GetBlobManager();
		ENSURE(BlobMgr, "Can't get BlobManager");
		
		if ( (((OpMsg*)Message)->MsgType == OpMsg::BEFORE_UNDO) ||
										(((OpMsg*)Message)->MsgType == OpMsg::BEFORE_REDO))
		{
			// render the tool blobs off
			if (m_pTool && Document::GetSelectedSpread() && ContourTool::IsCurrent())
			{
				BlobMgr->RenderToolBlobsOff(m_pTool, Document::GetSelectedSpread(), NULL);
				m_pTool->SetBlobRendering(FALSE);
			}
		}
		else if ( (((OpMsg*)Message)->MsgType == OpMsg::AFTER_UNDO) ||
										(((OpMsg*)Message)->MsgType == OpMsg::AFTER_REDO))
		{
			// render the tool blobs back on
			if (m_pTool && Document::GetSelectedSpread() && ContourTool::IsCurrent())
			{
				m_pTool->SetBlobRendering(TRUE);
				m_pTool->SetupToolBlobs();
				BlobMgr->RenderToolBlobsOn(m_pTool, Document::GetSelectedSpread(), NULL);
			}
		}
	}

	// Pass the message on to the immediate base class
	return (InformationBarOp::Message(Message));
}



/********************************************************************************************

>	void ContourInfoBarOp::HandleJoinTypeMitreClicked()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000

	Outputs:	We ensure that all objects in the selection have a mitre join type.

	Purpose:	Handles the user clicking on the mitre join-type button on our infobar.

	Notes:		This method assumes that an object must have a join type;
				ie	if none of the other join types apply to the selection,
					then the whole selection must have this join type, so there
					is no point in reapplying it in this case.

********************************************************************************************/
void ContourInfoBarOp::HandleJoinTypeMitreClicked()
{
	BOOL fRound = GetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEROUND));
	BOOL fBevel = GetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEBEVEL));

	// only bother changing to a mitre join type if any selected objects
	// use the other join types.
	if (fRound || fBevel)
		ChangeJoinType(MitreJoin);
	else
		SetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEMITRE), TRUE);
}



/********************************************************************************************

>	void ContourInfoBarOp::HandleJoinTypeRoundClicked()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000

	Outputs:	We ensure that all objects in the selection have a round join type.

	Purpose:	Handles the user clicking on the round join-type button on our infobar.

	Notes:		This method assumes that an object must have a join type;
				ie	if none of the other join types apply to the selection,
					then the whole selection must have this join type, so there
					is no point in reapplying it in this case.

********************************************************************************************/
void ContourInfoBarOp::HandleJoinTypeRoundClicked()
{
	BOOL fMitre = GetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEMITRE));
	BOOL fBevel = GetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEBEVEL));

	// only bother changing to a mitre join type if any selected objects
	// use the other join types.
	if (fMitre || fBevel)
		ChangeJoinType(RoundJoin);
	else
		SetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEROUND), TRUE);
}



/********************************************************************************************

>	void ContourInfoBarOp::HandleJoinTypeBevelClicked()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08 September 2000

	Outputs:	We ensure that all objects in the selection have a bevel join type.

	Purpose:	Handles the user clicking on the bevel join-type button on our infobar.

	Notes:		This method assumes that an object must have a join type;
				ie	if none of the other join types apply to the selection,
					then the whole selection must have this join type, so there
					is no point in reapplying it in this case.

********************************************************************************************/
void ContourInfoBarOp::HandleJoinTypeBevelClicked()
{
	BOOL fRound = GetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEROUND));
	BOOL fMitre = GetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEMITRE));

	// only bother changing to a mitre join type if any selected objects
	// use the other join types.
	if (fRound || fMitre)
		ChangeJoinType(BevelledJoin);
	else
		SetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEBEVEL), TRUE);
}



/********************************************************************************************

>	void ContourInfoBarOp::ChangeJoinType(JointType jt)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 September 2000
	Inputs:		jt	the join type to apply to the selection.

	Outputs:	An attribute describing the given join type is applied to the selection.

	Purpose:	Apply the given join type to the selection.

	Errors:		Shows a warning if we run out of memory.

	See also:	AttributeManager::AttributeSelected().

********************************************************************************************/
void ContourInfoBarOp::ChangeJoinType(JointType jt)
{
	// Create the appropriate join type attribute.
	AttrJoinType* pJoinAttr = new AttrJoinType;
	if (pJoinAttr == NULL)
	{
		InformWarning(_R(IDS_UNDO_MEMORY_FAILURE), _R(IDS_OK));
		return;
	}
	pJoinAttr->Value.JoinType = jt;

	// Let the attribute manager apply it to the selection.
	AttributeManager::AttributeSelected(pJoinAttr);
}



/********************************************************************************************

>	void ContourInfoBarOp::UpdateJoinTypeControls()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11 September 2000

	Outputs:	The join type buttons on the infobar are updated to reflect the join types
				contained in the selection.

	Purpose:	Update our infobar join type buttons.
				This method is currently called only in response to a SelChanging message.

********************************************************************************************/
void ContourInfoBarOp::UpdateJoinTypeControls()
{
// DEBUG:
//	TRACEUSER( "Karim", _T("ContourInfoBarOp::UpdateJoinTypeControls\n"));

	// the check for join types is possibly time-consuming, as we may need to scan the
	// selection if there are multiple join types in use on selected objects.

	// Determine if there is a common attribute.
	NodeAttribute* pAttr;
	SelRange::CommonAttribResult eResult;
	eResult = GetApplication()->
				 FindSelection()->
					 FindCommonAttribute(CC_RUNTIME_CLASS(AttrJoinType), &pAttr);

	// initialise all button-states to FALSE - they will be set to TRUE appropriately below.
	BOOL fMitre = FALSE;
	BOOL fRound = FALSE;
	BOOL fBevel = FALSE;

	switch (eResult)
	{
		// uncheck all buttons.
		case SelRange::ATTR_NONE:
			break;

		// only check the common join type.
		case SelRange::ATTR_COMMON:
			if (pAttr != NULL)
			{
				fMitre = ( ((AttrJoinType*)pAttr)->Value.JoinType == MitreJoin );
				fRound = ( ((AttrJoinType*)pAttr)->Value.JoinType == RoundJoin );
				fBevel = ( ((AttrJoinType*)pAttr)->Value.JoinType == BevelledJoin );
			}
			break;

		// test the selection for all common line attr types.
		case SelRange::ATTR_MANY:
			{
				// scan the selection, quitting as soon as all three states get set.
				SelRange* pSel = GetApplication()->FindSelection();
				for (Node*	pSelNode =	pSel->FindFirst();
							pSelNode !=	NULL && !(fMitre && fRound && fBevel);
							pSelNode =	pSel->FindNext(pSelNode))
				{
					if (!pSelNode->IsAnObject())
						continue;

					((NodeRenderableInk*)pSelNode)->
						FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), &pAttr);
					if (pAttr != NULL)
					{
						fMitre |= ( ((AttrJoinType*)pAttr)->Value.JoinType == MitreJoin );
						fRound |= ( ((AttrJoinType*)pAttr)->Value.JoinType == RoundJoin );
						fBevel |= ( ((AttrJoinType*)pAttr)->Value.JoinType == BevelledJoin );
					}
				}
			}
			break;

		// error - unrecognised return option from FindCommonAttribute().
		default:
			ERROR3("ContourInfoBarOp::UpdateJoinTypeControls; invalid result from FindCommonAttribute()");
			break;
	}

	SetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEMITRE), fMitre);
	SetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEROUND), fRound);
	SetBoolGadgetSelected(_R(IDC_CONTOURJOINTYPEBEVEL), fBevel);
}



/********************************************************************************************

>	void ContourInfoBarOp::EnableDisableJoinTypeControls()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12 September 2000
	Purpose:	Enable/disable the join type buttons on the info bar,
				according to the contents of the selection.

********************************************************************************************/
void ContourInfoBarOp::EnableDisableJoinTypeControls()
{
	// our join type buttons act only upon the selection.
	SelRange* pSel = GetApplication()->FindSelection();
	if (pSel == NULL || pSel->IsEmpty())
	{
		EnableGadget(_R(IDC_CONTOURJOINTYPEMITRE), FALSE);
		EnableGadget(_R(IDC_CONTOURJOINTYPEROUND), FALSE);
		EnableGadget(_R(IDC_CONTOURJOINTYPEBEVEL), FALSE);
	}
	else
	{
		EnableGadget(_R(IDC_CONTOURJOINTYPEMITRE), TRUE);
		EnableGadget(_R(IDC_CONTOURJOINTYPEROUND), TRUE);
		EnableGadget(_R(IDC_CONTOURJOINTYPEBEVEL), TRUE);
	}
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

CProfileBiasGain* ContourInfoBarOp::GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType)
{
	BOOL ok = ((GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN)) || (GadgetID == _R(IDC_CONTOURATTRBIASGAIN)));

	ERROR2IF(ok==FALSE, FALSE, "Invalid gadgetID passed");

	UINT32 TotalNumberSelected = (GetApplication()->FindSelection()->Count ());

	// get the list of all the shadows
	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));

	NodeContour* pFirstNodeCountour = NULL;
	CProfileBiasGain* pFirstProfile = NULL;

	/*if (ContourList.IsEmpty())
	{
		// the list is empty - er trick the code into closing the dialog
		*bAllSameType = FALSE;
		ContourList.DeleteAll();
		return (NULL);
	}*/

	if (TotalNumberSelected != (UINT32) ContourList.GetCount ())
	{
		// totals differ - so the user MUST have selected someother type of node as well
		*bAllSameType = FALSE;
		ContourList.DeleteAll();
		return (NULL);
	}

	NodeListItem * pItem = (NodeListItem *)ContourList.GetHead();

	// many flag
	*bMany = FALSE;

	while (pItem)
	{
		if (pFirstNodeCountour == NULL)
		{
			pFirstNodeCountour = (NodeContour*) pItem->pNode;
			
			if (GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN))
			{
				pFirstProfile = ((NodeContourController *)pItem->pNode)->GetObjectProfilePtr();
			}
			else if (GadgetID == _R(IDC_CONTOURATTRBIASGAIN))
			{
				pFirstProfile = ((NodeContourController *)pItem->pNode)->GetAttrProfilePtr();
			}
		}
		else
		{
			if (GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN))
			{
				CProfileBiasGain* pOtherProfile = NULL;

				pOtherProfile = ((NodeContourController *)pItem->pNode)->GetObjectProfilePtr();

				if (pOtherProfile)
				{
					if (*pFirstProfile == *pOtherProfile)
					{
						// all ok
					}
					else
					{
						*bMany = TRUE;
					}
				}
			}
			else if (GadgetID == _R(IDC_CONTOURATTRBIASGAIN))
			{
				CProfileBiasGain* pOtherProfile = NULL;

				pOtherProfile = ((NodeContourController *)pItem->pNode)->GetAttrProfilePtr();
				
				if (pOtherProfile)
				{
					if (*pFirstProfile == *pOtherProfile)
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

		pItem = (NodeListItem *)ContourList.GetNext(pItem);
	}

	ContourList.DeleteAll();
	
	if (*bMany == TRUE)
	{
		return (NULL);
	}
	else
	{
		return (pFirstProfile);
	}
}



/********************************************************************************************

>	void ContourInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/2000
	Purpose:	See InformationBarOp::ChangeProfile () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfile ()

*********************************************************************************************/

void ContourInfoBarOp::ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
	OpDescriptor* pOpDesc = NULL;

	BOOL FireOp = TRUE;

	// we only want to generate one bit of undo information - so decided whether
	// we have to fire the above op, or whether we just 'pump' the values into
	// our nodes (thereby nolonger generating infinite undo information) ....

	Operation* pLastOp = NULL;

	if (Profile->GetGeneratesInfiniteUndo ())	// only do if they didn't select a preset profile
	{
		pLastOp = Document::GetSelected()->GetOpHistory().FindLastOp();
	}

	if (GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN))
	{
		if (pLastOp)
		{
			if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeContourObjectProfile))
			{
				FireOp = FALSE;
			}
		}
			
		if (FireOp == TRUE)
		{
			pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOUROBJPROFILE);
		}
		else
		{	
			// we don't need/want any undo information - so just change the value ....

			ChangeContourObjectProfileAction Action;
			Action.ChangeObjectProfileWithNoUndo (*Profile);
		}
	}
	else if (GadgetID == _R(IDC_CONTOURATTRBIASGAIN))
	{
		if (pLastOp)
		{
			if (pLastOp->GetRuntimeClass() == CC_RUNTIME_CLASS(OpChangeContourAttributeProfile))
			{	
				FireOp = FALSE;
			}
		}

		if (FireOp == TRUE)
		{
			pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOURATTRPROFILE);
		}
		else
		{
			ChangeContourAttributeProfileAction Action;
			Action.ChangeAttributeProfileWithNoUndo (*Profile);
		}
	}
	else		// some mistake surely !!!!
	{
		return;
	}
	
	if ((pOpDesc != NULL) && (FireOp == TRUE))
	{
		ChangeContourProfileParam Param (*Profile);
		pOpDesc->Invoke(&Param);
	}
}



/********************************************************************************************

>	void ContourInfoBarOp::ChangeProfileOnIdle(CProfileBiasGain* Profile, CGadgetID GadgetID)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/2000
	Purpose:	See InformationBarOp::ChangeProfileOnIdle () for an explanation of this function.
	See Also:	InformationBarOp::ChangeProfileOnIdle ()

*********************************************************************************************/

void ContourInfoBarOp::ChangeProfileOnIdle(CProfileBiasGain* Profile, CGadgetID GadgetID)
{
	if (GadgetID == _R(IDC_CONTOUROBJECTBIASGAIN))
	{
		// we don't need/want any undo information - so just change the value ....
		
		ChangeContourObjectProfileAction Action;
		Action.ChangeObjectProfileWithNoUndo (*Profile);
	}
	else if (GadgetID == _R(IDC_CONTOURATTRBIASGAIN))
	{
		// we don't need/want any undo information - so just change the value ....
		
		ChangeContourAttributeProfileAction Action;
		Action.ChangeAttributeProfileWithNoUndo (*Profile);
	}
	else
	{
		return;		// what planet are you on?
	}
}



/********************************************************************************************

>	BOOL ContourInfoBarOp::SetSliderLogValue(MILLIPOINT Value)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	7/12/99
	Inputs:		The new width to set the slider to
	Outputs:	-
	Returns:	-
	Purpose:	Sets the position of the slider given the value
				The slider is logarithmic - i.e. it uses the profile to set & retreive
				its values
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL ContourInfoBarOp::SetSliderLogValue(MILLIPOINT Value)
{
	// make the value appear between 0 and 1
	if (Value > CONTOUR_DEPTH_MAX)
		Value = CONTOUR_DEPTH_MAX;

	if (Value < CONTOUR_DEPTH_MIN)
		Value = CONTOUR_DEPTH_MIN;

	double dValue = Value - CONTOUR_DEPTH_MIN;
	dValue /= (double)(CONTOUR_DEPTH_MAX - CONTOUR_DEPTH_MIN);

	// change the profile
	double dSliderValue = SliderProfile.MapZeroToOne((AFp)(dValue));
//	dSliderValue = 1.0 - dSliderValue;
		
	dSliderValue *= (double)CONTOUR_DEPTH_MAX;
	dSliderValue += CONTOUR_DEPTH_MIN;

	BOOL ok = SetGadgetRange(_R(IDC_CONTOURSLIDER), CONTOUR_DEPTH_MIN, CONTOUR_DEPTH_MAX);

	if (ok)
		ok = SetLongGadgetValue(_R(IDC_CONTOURSLIDER), (INT32)dSliderValue);

	return ok;
}

/********************************************************************************************

>	MILLIPOINT ContourInfoBarOp::GetSliderLogValue()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	7/12/99
	Inputs:		-
	Outputs:	The slider width in millipoints
	Returns:	-
	Purpose:	Sets the position of the slider given the value
				The slider is logarithmic - i.e. it uses the profile to set & retreive
				its values
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
MILLIPOINT ContourInfoBarOp::GetSliderLogValue()
{
	SetGadgetRange(_R(IDC_CONTOURSLIDER), CONTOUR_DEPTH_MIN, CONTOUR_DEPTH_MAX);
	MILLIPOINT Value = GetLongGadgetValue(_R(IDC_CONTOURSLIDER), CONTOUR_DEPTH_MIN,
		CONTOUR_DEPTH_MAX);
	
	// make the value appear between 0 and 1
	if (Value > CONTOUR_DEPTH_MAX)
		Value = CONTOUR_DEPTH_MAX;

	double dValue = Value - CONTOUR_DEPTH_MIN;
	dValue /= (double)(CONTOUR_DEPTH_MAX - CONTOUR_DEPTH_MIN);
//	dValue = 1.0 - dValue;

	// change the profile
	SliderProfile.SetBias(-SliderProfile.GetBias());
	double dSliderValue = SliderProfile.MapZeroToOne((AFp)(dValue));
	SliderProfile.SetBias(-SliderProfile.GetBias());
		
	dSliderValue *= (double)(CONTOUR_DEPTH_MAX - CONTOUR_DEPTH_MIN);
	dSliderValue += CONTOUR_DEPTH_MIN;

	return (INT32)dSliderValue;	
}


/********************************************************************************************

>	BOOL ContourInfoBarOp::ConvertValueToString(String_256 &In, const INT32 value, UnitType type, String_256 &unit)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/96
	Inputs:		The value to convert to a string (In)
	Outputs:	-
	Returns:	-
	Purpose:	TRUE if the function was able to convert the INT32 to a string

********************************************************************************************/

BOOL ContourInfoBarOp::ConvertValueToString(String_256 &In, const INT32 value, UnitType type)
{
	BOOL Converted = FALSE;	// True when a value has been converted
	
	Spread * CurrentSpread = Document::GetSelectedSpread();

	if (CurrentSpread)
	{
		// Get the dimension scaling object (units) associated with the given spread
		// and convert to its units.
		DimScale* pDimScale = DimScale::GetPtrDimScale((Node*) CurrentSpread);
		pDimScale->ConvertToUnits((INT32)value , &In);
		Converted = TRUE;
	}

	if (In.Length() == 0)
	{
		In = _T("0");
	}

	return Converted;
		
}


/********************************************************************************************

>	BOOL ContourInfoBarOp::ConvertStringToValue(CGadgetID ID, INT32 &Value)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/99
	Inputs:		The value to convert to a string (In)
	Outputs:	-
	Returns:	-
	Purpose:	TRUE if the function was able to convert the INT32 to a string
	See Also:	ShadowBarOp::ConvertStringToValue()

********************************************************************************************/

BOOL ContourInfoBarOp::ConvertStringToValue(CGadgetID ID, INT32 &Value)
{
	BOOL Valid = FALSE;
	
	DimScale *pDimScale = DimScale::GetPtrDimScale(Document::GetSelectedSpread());
	String_128 FieldContents = GetStringGadgetValue(ID, &Valid);
	Valid = pDimScale->ConvertToMillipoints(FieldContents, &Value);

	return Valid;
}

/********************************************************************************************

>	void ContourInfoBarOp::UpdateState()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94, changed 13/9/99 Diccon Yamanaka
	Purpose:	Overrides the empty UpdateState function provided by InformationBarOp
				making a call to the function in DialogBarOp.

*********************************************************************************************/

void ContourInfoBarOp::UpdateState()
{
	if(this == NULL)
		return;

 	if(!this->HasWindow() || !m_bUpdateState || !ContourTool::IsCurrent())
		return ;

	DialogBarOp::UpdateState();		// This updates all controls that have OpDescs attached

//	BOOL OneToOne = FALSE;
//	BOOL Antialias = NodeBlend::GetDefaultAntialias();
	INT32 NumSteps = 0;
	UINT32 NumSelBlends = 0;
	ColourBlendType ColBlendType = COLOURBLEND_FADE;
	UINT32 StepDistance = 0;
	BOOL OnlyBlendsSelected			= TRUE;
//	BOOL AllSelectedBlendsAreOnCurve= TRUE;
//	BOOL AtLeastOneBlendIsOnCurve	= FALSE;
//	BOOL Tangential					= FALSE;
//	BOOL EditSteps					= TRUE;
//	UINT32 NumBlendsOnCurve			= 0;
	CProfileBiasGain Profile;
//	BOOL bMany						= FALSE;
	UINT32 NumInsets = 0;
	UINT32 NumNonInsets = 0;

	// build a list of all contour nodes in the selection
	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController),TRUE);
	NodeListItem * pItem = (NodeListItem *)ContourList.GetHead();
	Node* pNode = NULL;
	
	if (pItem)
		pNode = pItem->pNode;

	// booleans to say whether there are inner or outer (or both) contours in 
	// the selection
	BOOL bHasInner = FALSE;
	BOOL bHasOuter = TRUE;
	BOOL bManySteps = FALSE;
	BOOL bFirst = TRUE;
	UINT32 LastNumSteps = 0;
	BOOL bManyDistances = FALSE;
	UINT32 LastStepDistance = FALSE;
	UINT32 ThisWidth = 0;
	BOOL bManyWidths = FALSE;
	UINT32 ThisNumSteps = 0;
	UINT32 ThisStepDistance = 0;

	NodeContourController* pControl = NULL;
	Node* pParent = NULL;

	// We need to check for contours in Blends! If we have then we need to disable the Inset Path function!
	BOOL DoEnableInsetPaths = TRUE;
	
	// If there`s no contours in the list then we must also check to see if there any nodes selected
	// inside blends as the inset path function will crash if there is!
	if(pNode == NULL)
	{
		// Get the current selection
		SelRange* pSelection = GetApplication()->FindSelection();
		Node* pSelected = pSelection->FindFirst();

		while(pSelected && DoEnableInsetPaths)
		{
			pParent = pSelected->FindParent();

			while(pParent && pParent->IsCompound() && DoEnableInsetPaths)
			{
				DoEnableInsetPaths = !pParent->IsABlend();
				pParent = pParent->FindParent();
			}

			pSelected = pSelection->FindNext(pSelected);
		}
	}

	// run through the list, setting variables
	while (pNode != NULL)
	{
		if (pNode->IsAContourController())
		{
			pControl = (NodeContourController *)pNode;

			// Find out if this contour is sitting inside a blend
			if(DoEnableInsetPaths)
			{
				pParent = pControl->FindParent();
				while(pParent && pParent->IsCompound() && DoEnableInsetPaths)
				{
					DoEnableInsetPaths = !pParent->IsABlend();
					pParent = pParent->FindParent();
				}
			}

			NumSelBlends++;
			
			ThisNumSteps = pControl->GetNumberOfSteps();
			ColourBlendType ThisColBlendType = pControl->GetColourBlendType();

			if (pControl->GetInsetPathFlag())
				NumInsets ++;
			else
				NumNonInsets ++;

			ThisStepDistance = (UINT32)(abs(pControl->GetWidth()) / (pControl->GetNumberOfSteps() + 1));

			// check for many's
			if (bFirst)
			{
				bFirst = FALSE;
				LastNumSteps = ThisNumSteps;
				LastStepDistance = ThisStepDistance;
				ThisWidth = abs(pControl->GetWidth());
			}
			else
			{
				if (LastNumSteps != ThisNumSteps)
					bManySteps = TRUE;

				if (LastStepDistance != ThisStepDistance)
					bManyDistances = TRUE;

				if (ThisWidth != (UINT32)abs(pControl->GetWidth()))
					bManyWidths = TRUE;
			}
			
			NumSteps 	 = ThisNumSteps;
			ColBlendType = ThisColBlendType;
			StepDistance = ThisStepDistance;
			
			if (pControl->GetWidth() < 0)
				bHasOuter = TRUE;

			if (pControl->GetWidth() > 0)
				bHasInner = TRUE;
		}
		else
			OnlyBlendsSelected = FALSE;
		
		pItem = (NodeListItem *)ContourList.GetNext(pItem);

		if (pItem)
			pNode = pItem->pNode;
		else
			pNode = NULL;
	}

	ContourList.DeleteAll();

	BOOL HasBevels = CheckForBevels();

	if ((NumInsets == 0 || (NumInsets > 0 && NumNonInsets > 0) || (NumInsets == 0 && NumNonInsets == 0)) && !HasBevels)
	{
		EnableGadget(_R(IDC_BTN_CONTOURDISTANCE), NumSelBlends > 0);
		EnableGadget(_R(IDC_BTN_CONTOURSTEPS), NumSelBlends > 0);
		EnableGadget(_R(IDC_CONTOUROBJECTBIASGAIN), NumSelBlends > 0);
		EnableGadget(_R(IDC_CONTOURATTRBIASGAIN), NumSelBlends > 0);
		EnableGadget(_R(IDC_CONTOURSTEPS), NumSelBlends > 0);
PORTNOTE("other", "_R(IDC_BTN_CONTOURANTIALIAS) removed as not in the resources")
#ifndef EXCLUDE_FROM_XARALX
		EnableGadget(_R(IDC_BTN_CONTOURANTIALIAS),NumSelBlends > 0);
#endif
		EnableGadget(_R(IDC_EFFECT), NumSelBlends > 0);

		if (NumSelBlends > 0)
		{
			EnableGadget(_R(IDC_BTN_INSETPATH), DoEnableInsetPaths);
			SetBoolGadgetSelected(_R(IDC_BTN_INSETPATH), m_bInsetPath);
		}
		else
		{
			SetBoolGadgetSelected(_R(IDC_BTN_INSETPATH), m_bInsetPath);
			EnableGadget(_R(IDC_BTN_INSETPATH), DoEnableInsetPaths);
		}
	}
	else
	{
		EnableGadget(_R(IDC_BTN_CONTOURDISTANCE), FALSE);
		EnableGadget(_R(IDC_BTN_CONTOURSTEPS), FALSE);
		EnableGadget(_R(IDC_CONTOUROBJECTBIASGAIN), FALSE);
		EnableGadget(_R(IDC_CONTOURATTRBIASGAIN), FALSE);
		EnableGadget(_R(IDC_CONTOURSTEPS), FALSE);
PORTNOTE("other", "_R(IDC_BTN_CONTOURANTIALIAS) removed as not in the resources")
#ifndef EXCLUDE_FROM_XARALX
		EnableGadget(_R(IDC_BTN_CONTOURANTIALIAS),FALSE);
#endif
		EnableGadget(_R(IDC_EFFECT), FALSE);
		EnableGadget(_R(IDC_BTN_INSETPATH), DoEnableInsetPaths);
		SetBoolGadgetSelected(_R(IDC_BTN_INSETPATH), TRUE);
	}

	if (GetApplication()->FindSelection())
	{
		if (GetApplication()->FindSelection()->IsEmpty() || HasBevels)
		{
			EnableGadget(_R(IDC_CONTOURSLIDER), FALSE);
			EnableGadget(_R(IDC_CONTOURSLIDEREDIT), FALSE);
		}
		else
		{
			EnableGadget(_R(IDC_CONTOURSLIDER), TRUE);
			EnableGadget(_R(IDC_CONTOURSLIDEREDIT), TRUE);
		}
	}
	
	String_64 Str;

	DeleteAllValues(_R(IDC_EFFECT));

	Str.Load(_R(IDS_FILLTOOL_FADE));
	SetStringGadgetValue(_R(IDC_EFFECT),Str,FALSE, FEMENU_FADE);
	Str.Load(_R(IDS_FILLTOOL_RAINBOW));
	SetStringGadgetValue(_R(IDC_EFFECT),Str,FALSE, FEMENU_RAINBOW);
	Str.Load(_R(IDS_FILLTOOL_ALTRAINBOW));
	SetStringGadgetValue(_R(IDC_EFFECT),Str,TRUE, FEMENU_ALTRAINBOW);

	SetComboListLength(_R(IDC_EFFECT));

	if (NumSelBlends == 0)
	{
		Str = _T("");
		SetStringGadgetValue(_R(IDC_CONTOURSTEPS),Str);
	}
	else
	{
		if (GetBoolGadgetSelected(_R(IDC_BTN_CONTOURSTEPS)))
		{
			if (NumSteps >= 0 && !bManySteps)
			{
				SetLongGadgetValue(_R(IDC_CONTOURSTEPS),NumSteps+1);
				PaintGadgetNow(_R(IDC_CONTOURSTEPS));
			}
			else if (bManySteps)
			{
				String_256 ManyString;
				ManyString.Load(_R(IDS_MANY));
				SetStringGadgetValue(_R(IDC_CONTOURSTEPS),ManyString);
				PaintGadgetNow(_R(IDC_CONTOURSTEPS));
			}
		}
		else if (GetBoolGadgetSelected(_R(IDC_BTN_CONTOURDISTANCE)))
		{
			if (!bManyDistances)
			{
				// do the conversion to whatever the document intends
				String_256 DistString;
				ConvertValueToString(DistString, StepDistance);
				SetStringGadgetValue(_R(IDC_CONTOURSTEPS), DistString);

			}
			else
			{
				String_256 ManyString;
				ManyString.Load(_R(IDS_MANY));
				SetStringGadgetValue(_R(IDC_CONTOURSTEPS),ManyString);
				PaintGadgetNow(_R(IDC_CONTOURSTEPS));
			}
		}

		switch (ColBlendType)
		{
			case COLOURBLEND_FADE:			Str.Load(_R(IDS_FILLTOOL_FADE));		break;
			case COLOURBLEND_RAINBOW:		Str.Load(_R(IDS_FILLTOOL_RAINBOW));		break;
			case COLOURBLEND_ALTRAINBOW:	Str.Load(_R(IDS_FILLTOOL_ALTRAINBOW));	break;
			case COLOURBLEND_NONE:			Str.Load(_R(IDS_MANY));					break;

			default:ERROR3("Unknown colour blend type"); Str.Load(_R(IDS_FILLTOOL_FADE)); break;
		}
		
		SetStringGadgetValue(_R(IDC_EFFECT),Str, FALSE, -1);
		
		// ensure immediate update
		PaintGadgetNow(_R(IDC_EFFECT));
	}

	// depress the profile buttons (or NOT) as the case may be ....
	if (NumSelBlends != GetApplication()->FindSelection()->Count())
	{
		EnableGadget (_R(IDC_CONTOUROBJECTBIASGAIN), FALSE);
		EnableGadget (_R(IDC_CONTOURATTRBIASGAIN), FALSE);
	}

	// depress either the edit steps or edit distance button
	if (m_EditBlendSteps == TRUE)
	{
		SetLongGadgetValue(_R(IDC_BTN_CONTOURSTEPS), TRUE);
		SetGadgetHelp(_R(IDC_CONTOURSTEPS), _R(IDBBL_CONTOURSTEPSEDITVALUE), _R(IDS_CONTOURSTEPSEDITVALUE));
	}
	else
	{
		SetLongGadgetValue(_R(IDC_BTN_CONTOURDISTANCE), TRUE);
		SetGadgetHelp(_R(IDC_CONTOURSTEPS), _R(IDBBL_CONTOURDISTANCEEDITVALUE), _R(IDS_CONTOURDISTANCEEDITVALUE));
	}

	// now, lets do the inner & outer buttons
	if (!bHasInner && bHasOuter)
	{
		EnableGadget(_R(IDC_BTN_CONTOUROUTER), TRUE);
		EnableGadget(_R(IDC_BTN_CONTOURINNER), TRUE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOUROUTER), TRUE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOURINNER), FALSE);
		m_bOuterIsSelected = TRUE;
		m_bInnerIsSelected = FALSE;
	}
	else if (bHasInner && !bHasOuter)
	{
		EnableGadget(_R(IDC_BTN_CONTOUROUTER), TRUE);
		EnableGadget(_R(IDC_BTN_CONTOURINNER), TRUE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOUROUTER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOURINNER), TRUE);
		m_bOuterIsSelected = FALSE;
		m_bInnerIsSelected = TRUE;
	} 
	else if (bHasInner && bHasOuter)
	{
		EnableGadget(_R(IDC_BTN_CONTOUROUTER), TRUE);
		EnableGadget(_R(IDC_BTN_CONTOURINNER), TRUE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOUROUTER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOURINNER), FALSE);
		m_bOuterIsSelected = FALSE;
		m_bInnerIsSelected = FALSE;
	}
	else if (!bHasInner && !bHasOuter)
	{
		EnableGadget(_R(IDC_BTN_CONTOUROUTER), FALSE);
		EnableGadget(_R(IDC_BTN_CONTOURINNER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOUROUTER), FALSE);
		SetBoolGadgetSelected(_R(IDC_BTN_CONTOURINNER), FALSE);
		m_bOuterIsSelected = FALSE;
		m_bInnerIsSelected = FALSE;
	}

	// Mark, do something similar here as well for _R(IDC_CONTOURJOINTYPEMITRE), ROUND, BEVEL....
	SetSliderValue(ThisWidth, bManyWidths);	

	// update the state of our join-type controls.
	UpdateJoinTypeControls();
	EnableDisableJoinTypeControls();

	UpdateGadgetHelp();
}

/********************************************************************************************

>	void ContourInfoBarOp::UpdateGadgetHelp()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Purpose:	Updates the help in this info bar

*********************************************************************************************/
void ContourInfoBarOp::UpdateGadgetHelp()
{

}



/********************************************************************************************

>	void ContourInfoBarOp::SetSliderValue(UINT32 Value, BOOL bMany)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Purpose:	Sets the slider & edit controls in the info bar

*********************************************************************************************/
void ContourInfoBarOp::SetSliderValue(UINT32 Value, BOOL bMany)
{
	// now do the slider
	SetGadgetRange(_R(IDC_CONTOURSLIDER), CONTOUR_DEPTH_MIN, CONTOUR_DEPTH_MAX);

	// slider goes from 0 on the right, so invert the width
//	UINT32 SliderValue = (CONTOUR_DEPTH_MAX - (Value - CONTOUR_DEPTH_MIN));
	
	BOOL ok = SetSliderLogValue(Value);

	String_256 WidthStr;
	
	// now do the edit control
	if (bMany)
	{
		WidthStr.Load(_R(IDS_MANY));

		if (ok)
			ok = SetStringGadgetValue(_R(IDC_CONTOURSLIDEREDIT), WidthStr);
		
		if (ok)
		{
			PaintGadgetNow(_R(IDC_CONTOURSLIDER));
			PaintGadgetNow(_R(IDC_CONTOURSLIDEREDIT));
		}
	}
	else
	{
		// convert to whatever value we're dealing with
		if (ok)
			ok = ConvertValueToString(WidthStr, Value);
		
		if (ok)
			ok = SetStringGadgetValue(_R(IDC_CONTOURSLIDEREDIT), WidthStr);

		if (ok)
		{
			PaintGadgetNow(_R(IDC_CONTOURSLIDER));
			PaintGadgetNow(_R(IDC_CONTOURSLIDEREDIT));
		}
	}
}



/********************************************************************************************

>	void ContourInfoBarOp::ChangeBitmapButtonState(CGadget GadgetID)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/99
	Purpose:	 utility used by ContourInfoBarOp::Message, if a button is depressed 
				and clicked again then un-depress it  For some reason the bitmap button custom
				control does not do this by itself..

*********************************************************************************************/

void ContourInfoBarOp::ChangeBitmapButtonState(CGadgetID GadgetID, BOOL* CurrentState)
{
	/*BOOL test =*/  GetBoolGadgetSelected(GadgetID);
	if (!*CurrentState)
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

>	void ContourInfoBarOp::DealWithContourOuterClick()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Purpose:	Deals with the contour outer button click message

*********************************************************************************************/
void ContourInfoBarOp::DealWithContourOuterClick()
{
	// do nothing if the button is already selected
	if (m_bOuterIsSelected)
		return;

	// invoke the operation
	OpDescriptor* pOpDesc = NULL;

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOUR_OUTER);

	if (pOpDesc)
	{
		pOpDesc->Invoke();
	}						
}

/********************************************************************************************

>	void ContourInfoBarOp::DealWithInsetPathClick()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Purpose:	Deals with the contour outer button click message

*********************************************************************************************/
void ContourInfoBarOp::DealWithInsetPathClick()
{
	// do we have any contours at the moment ?
	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContour));

	if (ContourList.IsEmpty())
	{
		// then just flip the flag
		if (m_bInsetPath)
		{
			m_bInsetPath = FALSE;
			SetBoolGadgetSelected(_R(IDC_BTN_INSETPATH), FALSE);
		}
		else
		{
			m_bInsetPath = TRUE;
			SetBoolGadgetSelected(_R(IDC_BTN_INSETPATH), TRUE);
		}

		return;
	}

	ContourList.DeleteAll();
		
	// invoke the operation
	OpDescriptor* pOpDesc = NULL;

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_TOGGLEINSETPATH);

	BOOL bSet = FALSE;

	if (GetBoolGadgetSelected(_R(IDC_BTN_INSETPATH)))
	{
		bSet = TRUE;
	}
	else
	{
		bSet = FALSE;
	}

	OpParam Param;
	Param.Param1 = bSet;

	if (pOpDesc)
	{
		pOpDesc->Invoke(&Param);
	}						
}


/********************************************************************************************

>	void ContourInfoBarOp::DealWithContourInnerClick()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Purpose:	Deals with the contour inner button click message

*********************************************************************************************/
void ContourInfoBarOp::DealWithContourInnerClick()
{
	// do nothing if the button is already selected
	if (m_bInnerIsSelected)
		return;

	// invoke the operation
	OpDescriptor* pOpDesc = NULL;

	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOUR_INNER);

	if (pOpDesc)
	{
		pOpDesc->Invoke();
	}						
}

/********************************************************************************************

>	void ContourInfoBarOp::DealWithWidthChange(UINT32 Width)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/99
	Purpose:	Deals with the contour width changes without changing the direction of
				the contours

*********************************************************************************************/
void ContourInfoBarOp::DealWithWidthChange(UINT32 Width)
{
	// make the list of nodes out of the selection
	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));

	if (!ContourList.IsEmpty())
	{	
		ChangeContourWidthParam Param(&ContourList, Width, TRUE);
		
		OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpChangeContourWidth));
		
		if (pOpDesc)
		{
			pOpDesc->Invoke(&Param);
		}	
		
		ContourList.DeleteAll();
	}
	else
	{
		// build the creation list
		SelRange* pSel = GetApplication()->FindSelection();

		if (pSel)
		{
			if (!pSel->IsEmpty())
			{
				EffectsStack* pStack = pSel->GetEffectsStack();
				ENSURE(pStack, "Argh!");
				Range* pSelList = pStack->GetBaseLevelRange();
				ENSURE(pSelList, "Argh!");

				Node* pNode = pSelList->FindFirst();
				NodeListItem* pItem = NULL;
				while (pNode)
				{
					pItem = new NodeListItem(pNode);
					ContourList.AddTail(pItem);
					
					pNode = pSelList->FindNext(pNode);
				}

				CreateContourParam Param(&ContourList, 3, -Width, m_bInsetPath);

				OpDescriptor * pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateContour));
		
				if (pOpDesc)
				{
					pOpDesc->Invoke(&Param);
				}	
		
				ContourList.DeleteAll();
			}
		}
	}
}


//////////////////////////////////////////////////////////////////////////////////////////
//  OpContourNodes
//
// This operation is responsible for creating and editing



/********************************************************************************************

>	OpContourNodes::OpContourNodes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Constructor. 

********************************************************************************************/

OpContourNodes::OpContourNodes()
{
	m_pBarOp = NULL;

	m_bHasDragged = FALSE;

	m_pPathList = NULL;
	m_NumPaths = 0;
	m_pSetList = NULL;
	m_NumSets = 0;
	m_pPathOuterList = NULL;
	m_pPathJoinTypeList = NULL;
	m_OriginalWidth = 0;
	m_StartDragWidth = 0;
	m_pTool = NULL;
	m_JoinType = MitreJoin;
}

/********************************************************************************************

>	OpContourNodes::~OpContourNodes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Destructor.

********************************************************************************************/

OpContourNodes::~OpContourNodes()
{
	if (m_pPathList)
	{
		delete [] m_pPathList;
	}	

	if (m_pSetList)
	{
		delete [] m_pSetList;
	}

	if (m_pPathOuterList)
	{
		delete [] m_pPathOuterList;
	}

	if (m_pPathJoinTypeList)
	{
		delete [] m_pPathJoinTypeList;
		m_pPathJoinTypeList = NULL;
	}
}

/********************************************************************************************

>	BOOL OpContourNodes::SetupDragInfo()
    
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		
	Outputs:	TRUE for success
	Returns:	-
	Purpose:	Sets up the drag information necessary to render the drag blobs

********************************************************************************************/
BOOL OpContourNodes::SetupDragInfo()
{
	Range Sel((*GetApplication()->FindSelection()));

	if (Sel.IsEmpty())
		return FALSE;

	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.SetRangeControl(rg);
	
	// go through all nodes getting their inside bounding rects, and
	// building up the summed path
	CProfileBiasGain Profile;

	// find out how many paths we require

	Node * pNode = Sel.FindFirst();

	DocRect dr(0,0,0,0);

	// how many paths do we need ?
	m_NumPaths = 0;

	Node * pSubNode = NULL;

	// don't do needs parent nodes !!!
	// therefore, recurse through the subtree of every node in the selection
	// calling their can become a's individually
	m_NumSets = 0;

	// do we have any contour controller nodes in the selection ? if so,
	// we need to set a flag to tell ourselves to ignore all nodes which aren't contoured
	BOOL bContourNodesExist = FALSE;

	List ContourNodeList;
	BevelTools::BuildListOfSelectedNodes(&ContourNodeList, 
		CC_RUNTIME_CLASS(NodeContourController),
		TRUE);

	if (!ContourNodeList.IsEmpty())
	{
		bContourNodesExist = TRUE;
	}

	// do the flatness
	// get the current view's zoom factor
	DocView * pView = DocView::GetCurrent();

	double ZoomFactor = 1;

	BecomeA TestBecomeA(BECOMEA_TEST, CC_RUNTIME_CLASS(NodePath));
	TestBecomeA.ResetCount();

	if (pView)
	{
		ZoomFactor = 
			pView->GetScaledPixelWidth().MakeDouble() / pView->GetPixelWidth().MakeDouble();
	}

	// alter the flatness appropriately
	double dFlat = ContourToolFlatness;

	if (ZoomFactor < 1.0)	
		m_Flatness = (INT32)(dFlat * ZoomFactor);
	else
		m_Flatness = ContourToolFlatness;

	ContourNodeList.DeleteAll();

	Node* pLastController = NULL;
	Node* pParentController = NULL;

	// we do a depth first search of the selection, calling DoBecomeA on the appropriate nodes
	// we do this because
	// a) we need to neglect all 'needs parent' nodes, which means calling doBecomeA on
	// just the top level selected nodes is insufficient
	// we also count how many 'sets' we need
	// a set defines a range in the path list (i.e. between path 2 and path 6) of paths
	// which need to be merged together before rendering
	// in building this set, we need to take into account when the nodes switch from one
	// contour controller node to another, as this obviously indicates the start of a new set
	// if any contour controller nodes exist, then we must ignore all nodes which are in the
	// selection but not part of a contour node. This is because in this case, when dragging,
	// no new contour nodes are created - the existing ones widths are changed

	// MRH - We need to find the default or applied join type on the contour controller!
	m_JoinType = MitreJoin;
	AttrJoinType* pJoinType = NULL;

	while (pNode)
	{
		pSubNode = pNode->FindFirstDepthFirst();

		// set up the parent controller from this node
		if (bContourNodesExist)
		{
			pLastController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController));
		}
		
		while (pSubNode)
		{
			if(IS_A(pSubNode,NodeContour))
			{
				((NodeRenderableInk *)pSubNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)(&pJoinType));

				if (pJoinType)
				{
					m_JoinType = pJoinType->Value.JoinType;
				}
			}

			TestBecomeA.ResetCount();
			if (NodeCanBeDragContoured(pSubNode, pNode, bContourNodesExist) &&
				pSubNode->CanBecomeA(&TestBecomeA))
			{
				if (!bContourNodesExist ||
					pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController)) != NULL)
				{
					m_NumPaths += TestBecomeA.GetCount();
				}

				// check for the number of sets needing to be increased - i.e. if
				// the controller node of the new node is different to the last controller
				if (bContourNodesExist)
				{
					pParentController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController));

					if (pParentController)
					{
						if (pParentController != pLastController &&
							pLastController != NULL)
						{
							m_NumSets ++;
						}

						pLastController = pParentController;
					}
				}
			}

			pSubNode = pSubNode->FindNextDepthFirst(pNode);
		}

		m_NumSets ++;

		pNode = Sel.FindNext(pNode);
	}

	// increase the number of sets by 1
	m_NumSets++;

	ERROR2IF(m_NumPaths == 0, FALSE, "Nothing to contour");

	if (m_pPathList)
		delete [] m_pPathList;

	if (m_pSetList)
		delete [] m_pSetList;

	if (m_pPathOuterList)
		delete [] m_pPathOuterList;

	if (m_pPathJoinTypeList)
		delete [] m_pPathJoinTypeList;

	ALLOC_WITH_FAIL(m_pPathList, new Path[m_NumPaths], this);

	// each path has whether the object it came from was an inner or an outer contour
	// or not
	ALLOC_WITH_FAIL(m_pPathOuterList, new BOOL[m_NumPaths], this);
	ALLOC_WITH_FAIL(m_pPathJoinTypeList, new AttrJoinType*[m_NumPaths], this);
	UINT32 i;
	for (i = 0; i < m_NumPaths; i++)
	{
		m_pPathList[i].Initialise();
		m_pPathOuterList[i] = TRUE;
		m_pPathJoinTypeList[i] = NULL;
	}

	ALLOC_WITH_FAIL(m_pSetList, new UINT32[m_NumSets], this);

	// Make sure we have a valid pOurDoc as Contouring moulds with bitmap fills go bang!
	pOurDoc = Document::GetCurrent();

	// sum all paths together in the range
	ContourDragBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath), this, FALSE,
		m_pPathList, m_NumPaths);
	MyBecomeA.ResetCount();

	pNode = Sel.FindFirst();

	// don't do needs parent nodes !!!
	// therefore, recurse through the subtree of every node in the selection
	// calling their do become a's individually

	NodeContourController * pControl = NULL;

	AttrJoinType * pAttrJoin = NULL;
	Node * pNodePath = NULL;

	UINT32 SetCount = 0;
//	UINT32 ObjectCount = 0;

	while (pNode)
	{
		pSubNode = pNode->FindFirstDepthFirst();

		// set up the parent controller from this node
		if (bContourNodesExist)
		{
			pLastController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController));
		}

		// start a new set
		m_pSetList[SetCount] = MyBecomeA.GetCount();
		SetCount ++;

		ERROR3IF(SetCount >= m_NumSets, "Number of sets doesn't match");		

		while (pSubNode)
		{
			if (NodeCanBeDragContoured(pSubNode, pNode, bContourNodesExist) &&
				pSubNode->CanBecomeA(&TestBecomeA))
			{
				// check for the number of sets needing to be increased - i.e. if
				// the controller node of the new node is different to the last controller
				if (bContourNodesExist)
				{
					pParentController = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController));

					if (pParentController)
					{
						if (pParentController != pLastController &&
							pLastController != NULL)
						{
							// start a new set
							m_pSetList[SetCount] = MyBecomeA.GetCount();
							SetCount ++;
						}

						pLastController = pParentController;
					}
				}
				
				// do the become a
				if (!bContourNodesExist ||
					pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController)) != NULL)
				{
					// get the start path count
					UINT32 StartCount = MyBecomeA.GetCount();
					
					// find out if we have a parent contour controller node or not
					pControl = 
						(NodeContourController *)pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController));

					// do the become A
					if (!pSubNode->IsNodePath())
					{
						pSubNode->DoBecomeA(&MyBecomeA);
					}
					else
					{
						// make the node out of the path first
						pNodePath = ((NodePath *)pSubNode)->MakeNodePathFromAttributes(m_Flatness);

						pNodePath->DoBecomeA(&MyBecomeA);

						delete pNodePath;
						pNodePath = NULL;
					}
					
					// find out if my parent controller is an inner or an outer,
					// and store the value for each path which has just been created
					if (pControl)
					{
						// find the applied join type attribute for this particular series of
						if (pControl->GetContourNode())
						{
							pControl->GetContourNode()->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),
								(NodeAttribute **)(&pAttrJoin));
						}
						
						for (UINT32 x = StartCount; x < MyBecomeA.GetCount(); x++)
						{
							if (pControl->GetWidth() > 0)
							{
								m_pPathOuterList[x] = FALSE;
							}
							
							m_pPathJoinTypeList[x] = pAttrJoin;					
						}
					}
					else
					{
						if (pSubNode->IsAnObject())
						{
							// find the selected parent of this sub node (if one exists !)
							Node * pSubNodeParentStep = pSubNode->FindParent();
							Node * pSubNodeParent     = NULL;
							pAttrJoin = NULL;

							while (pSubNodeParentStep)
							{
								if (pSubNodeParentStep->IsAnObject() &&
									pSubNodeParentStep->IsSelected())
								{
									pSubNodeParent = pSubNodeParentStep;
								}

								pSubNodeParentStep = pSubNodeParentStep->FindParent();
							}
							
							if (pSubNodeParent)
							{
								((NodeRenderableInk *)pSubNodeParent)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),
									(NodeAttribute **)(&pAttrJoin));
							}
							else if (pSubNode->IsAnObject())
							{
								((NodeRenderableInk *)pSubNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType),
									(NodeAttribute **)(&pAttrJoin));
							}

							for (UINT32 x = StartCount; x < MyBecomeA.GetCount(); x++)
							{
								m_pPathJoinTypeList[x] = pAttrJoin;					
							}
						}
					}
				}
			}

			pSubNode = pSubNode->FindNextDepthFirst(pNode);
		}

		pNode = Sel.FindNext(pNode);
	}

	// put the last set node in
	m_pSetList[SetCount] = MyBecomeA.GetCount();

	m_SelRect.lo.x = 0;
	m_SelRect.lo.y = 0;
	m_SelRect.hi.x = 0;
	m_SelRect.hi.y = 0;

	INT32 BlobSize = 0;
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();
	if (pBlobMgr)
		BlobSize = pBlobMgr->GetBlobSize();

	// calculate the rect from the paths
	for (i = 0 ; i < m_NumPaths; i++)
	{
		// Karim 19/04/2000
		// don't complain if a path has less than two coords, just ignore it.
		if (m_pPathList[i].GetNumCoords() < 2)
		{
//			ERROR3("Less than 2 points in path");
		}
		else
		{
//			m_SelRect = m_SelRect.Union(m_pPathList[i].GetBoundingRect()); MRH 19/5/00
			m_pPathList[i].GetTrueBoundingRect(&dr);
			m_SelRect = m_SelRect.Union(dr);
		}
	}

	// increase the size of the selection rect to take into account the blobs
	m_SelRect.Inflate(BlobSize);
	
	return TRUE;
}



/********************************************************************************************

>	BOOL OpContourNodes::NodeCanBeDragContoured(Node* pSubNode,
												Node* pNode,
												BOOL bContourNodesExist)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/05/2000
	Inputs:		pSubNode	the node which we are considering.
				pNode		the node on which the drag is actually occurring.
							note that this should be an ancestor of pSubNode in the tree.
				bContourNodesExist	TRUE if we're editting existing contours,
									FALSE if we're creating new contours.

	Returns:	TRUE if pSubNode should have its outline taken into account when the user
				creates or changes a contour by dragging.
				FALSE if not.

	Purpose:	Helper function for SetupDragInfo(), which encapsulates a test for whether
				a node's outline should be considered when dragging contours.
				There are a number of rules for exactly which nodes should be considered;
				examine the body of this function for details.
	Errors:		ERROR3 in DEBUG if any parameters are NULL.
				*Will* bomb out with AV's if NULL parameters are passed, so don't!
	See also:	SetupDragInfo()

********************************************************************************************/
BOOL OpContourNodes::NodeCanBeDragContoured(Node* pSubNode,
											Node* pNode,
											BOOL bContourNodesExist)
{
	// subnode must not require a parent node to exist.
	BOOL	ok = !pSubNode->NeedsParent(NULL);

	// subnode must be a NodeRenderableInk.
	if (ok) ok = pSubNode->IsAnObject();

	// subnode must not be any compound node other than the exceptions listed here.
	if (ok) ok = !pSubNode->IsCompound() || pSubNode->IsABaseTextClass() ||
					pSubNode->IS_KIND_OF(NodeMould) || pSubNode->IsANodeClipViewController();

	// subnode must not reside within a NodeMould.
	if (ok) ok = (pSubNode->FindParent(CC_RUNTIME_CLASS(NodeMould)) == NULL);

	// ClipView tests.
	if (ok)
	{
		// if contours do exist, then subnode must not reside within a ClipView group
		// which has, or whose parents have, a contour currently applied, and which is,
		// or lies beneath, the contour's object node.
		if (bContourNodesExist)
		{
			Node* pNCC = pSubNode->FindParent(CC_RUNTIME_CLASS(NodeContourController));
			if (pNCC != NULL)
			{
				Node* pNodeTest = pNCC;
				while (pNodeTest != pNode && pNodeTest != NULL)
					pNodeTest = pNodeTest->FindParent();
				if (pNodeTest != NULL)
				{
					// pNCC is, or is a child of, pNode.
					// now we just need to find out whether pSubNode and pNCC
					// have a NCVC in between.
					ok = !pSubNode->IsFamily(CC_RUNTIME_CLASS(NodeClipViewController), pNCC);
				}
			}
		}

		// if no contours exist yet, then subnode must not reside within a ClipView group
		// which either is, or lies beneath, the contour's object node.
		else
			ok = !pSubNode->IsFamily(CC_RUNTIME_CLASS(NodeClipViewController), pNode);
	}

	return ok;

/*
 *	This comment holds the unexpanded version of the above tests,
 *	apart from the ClipView test for when contours do exist.
 *
	return !pSubNode->NeedsParent(NULL) && 
			(!pSubNode->IsCompound() || pSubNode->IsABaseTextClass() || 
			pSubNode->IS_KIND_OF(NodeMould) || pSubNode->IsANodeClipViewController()) && 
			pSubNode->IsAnObject() &&
			pSubNode->FindParent(CC_RUNTIME_CLASS(NodeMould)) == NULL &&
			(bContourNodesExist || 
			!pSubNode->IsFamily(CC_RUNTIME_CLASS(NodeClipViewController), pNode))
			;
*/
}



/********************************************************************************************

>	BOOL OpContourNodes::DoDrag(ContourTool * pTool, ContourInfoBarOp * pBar, DocCoord &PointerPos,
								BOOL bDragOnBlob, DocRect * pBlobRect)
    
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		The info bar starting the drag, and the pointer position when the drag is 
				started.
				bDragOnBlob = TRUE if the drag started on a tool blob
				pBlobRect   = pointer to the rect of the blob which has been clicked on
	Outputs:	-
	Returns:	-
	Purpose:	This starts a drag that may lead to a contour.
				The DragFinished() method will do the hard work of contouring if it can be done.

********************************************************************************************/

BOOL OpContourNodes::DoDrag(ContourTool * pTool, ContourInfoBarOp * pBar, DocCoord &PointerPos, BOOL bDragOnBlob,
							DocRect *pBlobRect)
{
	// snap the pointer position
	DocView * pView = DocView::GetCurrent();
	Spread * pSpread = Document::GetSelectedSpread();

	if (pView && pSpread)
	{
		pView->Snap(pSpread, &PointerPos, FALSE, TRUE);
	}
	
	// if there's no selection then forget it !
	if (!GetApplication()->FindSelection())
		return FALSE;

	/////////////////////////////////////////////////////////////////////////////////////////
	//
	// This added by Karim MacDonald 17/12/1999, to fix a bug, whereby starting a contour
	// drag whilst a text char was sub-selected, then dragging with selector tool, caused
	// and error.
	//

	// if the drag was started on a blob, ensure any text in the selection is valid for this.
	if (bDragOnBlob)
		GetApplication()->FindSelection()->MakePartialSelectionWhole();

	// otherwise quit - can't do this drag unless it's from a blob.
	else
		return FALSE;
	//
	/////////////////////////////////////////////////////////////////////////////////////////

	m_pBarOp = pBar;
	m_pTool  = pTool;
	m_bHasDragged = FALSE;
	m_LastPointerPos = PointerPos;
	m_ThisPointerPos = PointerPos;	

	if (pBlobRect)
		m_BlobRect = *pBlobRect;
	else
		m_BlobRect = DocRect(0,0,0,0);
	
	m_bDragStartedOnBlob = bDragOnBlob;

	// decide what the start value for the drag will be
	List ContourList;
	BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));

	if (ContourList.IsEmpty())
	{
		m_OriginalWidth = 0;
	}
	else
	{
		NodeListItem * pItem = (NodeListItem *)ContourList.GetHead();

		m_OriginalWidth = ((NodeContourController *)(pItem->pNode))->GetWidth();
	}

	ContourList.DeleteAll();

	StartDrag( DRAGTYPE_AUTOSCROLL );	

	if (bDragOnBlob)
	{
		SetupDragInfo();

		if (pBlobRect)
		{
			DocCoord BlobCentre(pBlobRect->Centre());
	
			// calculate the width
			INT32 Width = ContourTool::CalculateContourWidth(m_SelRect, 
				BlobCentre);

			m_StartDragWidth = Width;

			m_LastPointerPos = BlobCentre;
			m_ThisPointerPos = BlobCentre;
		}
	}
	
	// Tell the Dragging system that we need drags to happen
	return TRUE;
}




/********************************************************************************************

>	void OpContourNodes::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/99
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods  - Which key modifiers are being pressed
				pSpread    - The spread that the mouse pointer is over
	Purpose:	Takes the pointer position and calculates the new dragged outline of the EORd
				bounding box
	SeeAlso:	ClickModifiers

********************************************************************************************/

void OpContourNodes::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, 
								   Spread* pSpread, BOOL bSolidDrag)
{
	ERROR3IF(!m_pBarOp, "No info bar op");

	// snap the pointer position
	DocView *pView = DocView::GetCurrent();

	if (pView && pSpread)
	{
		pView->Snap(pSpread, &PointerPos, FALSE, TRUE);
	}
/*
	// check the pointer pos for it being over the blob
	AlterPointerPosToAccountForBlob(&PointerPos);
*/
	if (PointerPos == m_LastPointerPos)
	{
		return;
	}

	if (!m_bDragStartedOnBlob)
	{
		m_bHasDragged = TRUE;
		return;
	}

	// render the drag blobs off first
	if (m_bHasDragged)
		RenderDragBlobs(m_SelRect, pSpread, bSolidDrag);

	m_bHasDragged = TRUE;
	
	m_LastPointerPos = PointerPos;
	m_ThisPointerPos = PointerPos;	

	ContourTool::DisplayStatusBarHelp(_R(IDS_CONTOURDRAGHELP));
	
	INT32 Width = (ContourTool::CalculateContourWidth(m_SelRect, PointerPos) - m_StartDragWidth) 
		+ m_OriginalWidth;

	RenderDragBlobs(Width, pSpread);

	if (Width < 0)
	{
		// outer contour
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_CONTOUROUTER), TRUE);
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_CONTOURINNER), FALSE);
	}
	else
	{
		// inner contour
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_CONTOUROUTER), FALSE);
		m_pBarOp->SetBoolGadgetSelected(_R(IDC_BTN_CONTOURINNER), TRUE);
	}

	m_pBarOp->PaintGadgetNow(_R(IDC_BTN_CONTOURINNER));
	m_pBarOp->PaintGadgetNow(_R(IDC_BTN_CONTOUROUTER));

	m_pBarOp->SetSliderValue(abs(Width), FALSE);
}



/********************************************************************************************

>	void OpContourNodes::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)

    
	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
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

void OpContourNodes::DragFinished( DocCoord PointerPos, ClickModifiers ClickMods, 
								Spread* pSpread, BOOL Success, BOOL bSolidDrag)
{
	// snap the pointer position
	DocView * pView = DocView::GetCurrent();
	
	if (pView && pSpread)
	{
		pView->Snap(pSpread, &PointerPos, FALSE, TRUE);
	}
	
	ContourTool * pTool = (ContourTool *)Tool::GetCurrent();
	
	// End the drag and the op
//	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	EndDrag();

	// added 20/12/1999 by Karim MacDonald.
	// if the drag did not start on a blob, we quit now.
	if (!m_bDragStartedOnBlob)
		return;

	// calculate the contour width & set the infobar
	INT32 Width = (ContourTool::CalculateContourWidth(m_SelRect, PointerPos) - m_StartDragWidth) 
		+ m_OriginalWidth;

	// if we've not dragged, then do nothing
	if (!m_bHasDragged)
		return;

	if (m_pBarOp)
		m_pBarOp->UpdateState();

	m_bHasDragged = FALSE;

	if (!pSpread)
		pSpread = Document::GetSelectedSpread();

	// render the drag blobs off
	RenderDragBlobs(m_SelRect, pSpread, bSolidDrag);	

	// create the contour, or change its width
	if (Success)
	{
		if (pTool)
		{
			pTool->RenderToolBlobs(pSpread, NULL);
			pTool->SetBlobRendering(FALSE);
		}
		
		List ContourList;
		BevelTools::BuildListOfSelectedNodes(&ContourList, CC_RUNTIME_CLASS(NodeContourController));

		if (ContourList.IsEmpty())
		{
			// make a list of the range, and then invoke the op
			EffectsStack* pStack = GetApplication()->FindSelection()->GetEffectsStack();
			ENSURE(pStack, "Argh!");
			Range* pSelList = pStack->GetBaseLevelRange();
			ENSURE(pSelList, "Argh!");

			Node* pNode = pSelList->FindFirst();
			NodeListItem* pItem = NULL;
			while (pNode)
			{
				pItem = new NodeListItem(pNode);
				ContourList.AddTail(pItem);
				
				pNode = pSelList->FindNext(pNode);
			}

			CreateContourParam Param(&ContourList, 3, Width, m_pBarOp->GetInsetPathFlag());
			
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CREATECONTOUR);

			if (pOpDesc)
				pOpDesc->Invoke(&Param);

			ContourList.DeleteAll();
			delete pSelList;
		}
		else
		{
			ChangeContourWidthParam Param(&ContourList, Width, FALSE);

			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_CHANGECONTOURWIDTH);

			if (pOpDesc)
				pOpDesc->Invoke(&Param);

			ContourList.DeleteAll();
		}

		// put the tool blobs back on
		if (pTool)
		{
			pTool->SetBlobRendering(TRUE);
			pTool->SetupToolBlobs();
			pTool->RenderToolBlobs(pSpread, NULL);
		}
	}

	delete [] m_pPathList;
	m_pPathList = NULL;

	delete [] m_pSetList;
	m_pSetList = NULL;

	delete [] m_pPathOuterList;
	m_pPathOuterList = NULL;

	delete [] m_pPathJoinTypeList;
	m_pPathJoinTypeList = NULL;
}

/********************************************************************************************

>	void OpContourNodes::AlterPointerPosToAccountForBlob(DocCoord * pPoint)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		The point to change
	Purpose:	Ensures that dragging back onto a blob returns you to the original
				contour value

********************************************************************************************/
void OpContourNodes::AlterPointerPosToAccountForBlob(DocCoord * pPoint)
{
	DocCoord PPos;

	DocCoord Offset;

	double Scale = 0;

	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	double BlobSize = 0;

	if (pBlobMgr)
		BlobSize = pBlobMgr->GetBlobSize()/2;
	else
		return;
	
	if (m_BlobRect.IsValid())
	{
		DocCoord Centre = m_BlobRect.Centre();
			
		if (m_BlobRect.ContainsCoord(*pPoint))
		{
			
			pPoint->x = Centre.x;
			pPoint->y = Centre.y;
		}
		else
		{
			// move the pointer appropriately so that the blob is always
			// the same value
			
			// first find the point which is the intersection between the line from
			// the centre of the blob to the point on the outside of the blob
			PPos.x = pPoint->x - Centre.x;
			PPos.y = pPoint->y - Centre.y;

			if (abs(PPos.x) > abs(PPos.y))
			{
				Scale = ((double)abs(PPos.y)) / ((double)abs(PPos.x));
				Scale *= BlobSize;

				if (PPos.x > 0)
				{
					Offset.x = (INT32)BlobSize;
				}
				else
				{
					Offset.x = (INT32)-BlobSize;
				}

				if (PPos.y > 0)
				{
					Offset.y = (INT32)Scale;
				}
				else
				{
					Offset.y = (INT32)-Scale;
				}
			}
			else
			{
				Scale = ((double)PPos.x) / ((double)PPos.y);
				Scale *= BlobSize;

				if (PPos.y > 0)
				{
					Offset.y = (INT32)BlobSize;
				}
				else
				{
					Offset.y = (INT32)-BlobSize;
				}

				if (PPos.x > 0)
				{
					Offset.x = (INT32)Scale;
				}
				else
				{
					Offset.x = (INT32)-Scale;
				}
			}

			pPoint->x -= Offset.x;
			pPoint->y -= Offset.y;
		}
	}
}


/********************************************************************************************

>	void OpContourNodes::RenderDragBlobs(MILLIPOINT Width,Spread* pSpread, BOOL bKeepDirections)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> H - rewritten Dave Mc code 06/06/00
	Created:	11/10/94
	Inputs:		Width   - The width of the contour to render the drag blobs of
				pSpread - The spread that the drawing will happen on
				bKeepDirections - Whether to maintian each set's direction (either inner
								  or outer) or to render the blobs with the signed value
								  passed in.
	Purpose:	Draws an EORed outline distance width away

********************************************************************************************/
void OpContourNodes::RenderDragBlobs(MILLIPOINT Width,Spread* pSpread, BOOL bKeepDirections)
{
	bool Outer = false;
	if(Width < 0)
		Outer = true;

	if (bKeepDirections)
		Width = abs(Width);

	if (/*m_pPathJoinTypeList == NULL || */m_pPathList == NULL || m_pSetList == NULL)
	{
		ERROR3("No list defined");
		return;
	}

	// Allocate Two Path Arrays to hold the outline information
	// SetPathList holds the all the paths inside compound nodes
	Path * pSetPathList = NULL;
	ALLOC_WITH_FAIL(pSetPathList, new Path[m_NumSets-1], this);

	// DrawPAthList holds the path that has been expanded by the width
	Path * pDrawPathList = NULL;
	ALLOC_WITH_FAIL(pDrawPathList, new Path[m_NumPaths], this);

	// Two local paths to hold intermediate path steps
	Path CopyPath;
	Path TempPath;
	CopyPath.Initialise();
	TempPath.Initialise();

	DocRect DrawBounds(0,0,0,0);
	DocRect dri(0,0,0,0);
	DocRect drj(0,0,0,0);
	UINT32 j = 0;

	// Set the flatness to be 1500 (2 pixels) at 96dpi
	INT32 Flatness = 1500;
	DocView * pView = DocView::GetCurrent();
	if (pView)
	{
		double ZoomFactor = pView->GetScaledPixelWidth().MakeDouble() / pView->GetPixelWidth().MakeDouble();
		Flatness = (INT32)(ZoomFactor * Flatness);
		if(Flatness <= 1)
		{
			Flatness = 2;
		}
	}

	//  Get the current Join Style
	// do the default join type for the document
//	AttrJoinType * pDefaultJoinType = NULL;
	JointType DefaultJT = m_JoinType;
	JoinStyles JoinS = (DefaultJT==MitreJoin) ? JOIN_MITER : (DefaultJT==RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;
	UINT32 i;
	// Loop trough the path list expanding them and saving the result into the DrawPathList
	for (i = 0 ; i < m_NumPaths; i++)
	{
		pDrawPathList[i].Initialise();

		// Make sure we use the correct function by checking to see if the path is closed or not!
		CapStyles CapS = CAPS_ROUND;
		BOOL IsPathClosed = m_pPathList[i].IsClosed();
		m_pPathList[i].InitializeContourValues(abs(Width)*2,JoinS,Outer,Flatness,IsPathClosed,IsPathClosed,CapS);
		m_pPathList[i].GetContourForStep(&pDrawPathList[i],1.0);
	}

	// merge all the paths in each set and keep an overall bounding rect going
	for (i = 0 ; i < m_NumSets - 1; i++)
	{
		pSetPathList[i].Initialise();
		pSetPathList[i].GetTrueBoundingRect(&dri);
		
		for (j = m_pSetList[i]; j < m_pSetList[i+1]; j++)
		{
			pDrawPathList[j].GetTrueBoundingRect(&drj);
			pSetPathList[i].MergeTwoPaths(pDrawPathList[j]);
		}

		if(m_NumSets > 1)
		{
			CopyPath.ClearPath(FALSE);
			CopyPath.CloneFrom(pSetPathList[i]);
			pSetPathList[i].ClearPath(FALSE);
			TempPath.ClipPathToPath(CopyPath,&pSetPathList[i],7|(1<<4),50,m_Flatness,m_Flatness);
		}

		pSetPathList[i].IsFilled = TRUE;
		pSetPathList[i].IsStroked = FALSE;

		DrawBounds = DrawBounds.Union(dri);
	}

	// increase the bounds slightly - DaveMc ?
	DrawBounds.Inflate(750);

	// Get the render region required
	RenderRegion* pRegion = DocView::RenderOnTop( NULL, pSpread, UnclippedEOR );

	while ( pRegion != NULL )
	{
		// Set the line colour and Draw the rect
		pRegion->SaveContext();

		pRegion->SetLineWidth(0);
		
		pRegion->SetWindingRule(PositiveWinding);
		
		pRegion->SetLineColour(COLOUR_RED);
		pRegion->SetFillColour(COLOUR_NONE);

		for (i = 0 ; i < m_NumSets-1; i++)
		{
			if (pSetPathList[i].GetNumCoords()>0)
				pRegion->DrawPath(&(pSetPathList[i]));
		}
		
		// Get the Next render region
		pRegion->RestoreContext();
		pRegion = DocView::GetNextOnTop( NULL );
	}

	// tidy up and finish!
	delete [] pSetPathList;
	delete [] pDrawPathList;
}


/********************************************************************************************

>	void OpContourNodes::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Inputs:		Rect 	- The region that needs the blobs to be drawn
				pSpread - The spread that the drawing will happen on
	Purpose:	Draws an EORed rectangle defined by AnchorPoint and DragPoint

********************************************************************************************/

void OpContourNodes::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
	if (m_pPathList == NULL || m_NumPaths == 0)
	{
		return;
	}
	
	// set up the bounds
	INT32 Width = (ContourTool::CalculateContourWidth(m_SelRect, m_ThisPointerPos) - m_StartDragWidth) 
		+ m_OriginalWidth;

	RenderDragBlobs(Width, pSpread);	
}

/********************************************************************************************

>	BOOL OpContourNodes::Declare()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Returns:	TRUE if all went OK, FALSE otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpContourNodes::Declare()
{
	return (RegisterOpDescriptor(
								0, 
								_R(IDS_CONTOUR_TOOL),
								CC_RUNTIME_CLASS(OpContourNodes), 
								OPTOKEN_CONTOURNODES,
								OpContourNodes::GetState,
								0,      	/* help ID */
								_R(IDBBL_NOOP),	/* bubble ID */
								0			/* bitmap ID */
								));
}


/********************************************************************************************

>	static OpState OpContourNodes::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the push tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time

********************************************************************************************/

OpState OpContourNodes::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	
	return State;
}

/********************************************************************************************

>	virtual void OpContourNodes::GetOpName(String_256* OpName) 

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

void OpContourNodes::GetOpName(String_256* OpName) 
{ 
	*OpName = String_256(_R(IDS_CREATECONTOUROP));
}  

/********************************************************************************************

>	BOOL ContourDragBecomeA::PassBack(NodeRenderableInk* pNewNode,
		NodeRenderableInk* pCreatedByNode,
		CCAttrMap* pAttrMap);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/12/99
	Inputs:		See base class
	Outputs:	TRUE for success
	Returns:	
	Purpose:	Passback function used to sum all paths together
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL ContourDragBecomeA::PassBack(NodeRenderableInk* pNewNode,
		NodeRenderableInk* pCreatedByNode,
		CCAttrMap* pAttrMap)
{
	if (m_Count >= m_NumPaths)
	{
		ERROR3("Number of paths doesn't match");
		return FALSE;
	}
	
	if (!pNewNode || !pNewNode->IsNodePath() || ((NodePath *)pNewNode)->InkPath.GetNumCoords() < 2)
		return FALSE;

	if (pCreatedByNode)
	{
		// only do non-needs parent nodes
		if (!pCreatedByNode->NeedsParent(NULL))
		{
			BOOL PathIsClosed = ((NodePath*)pNewNode)->InkPath.IsClosed();
			// check for an open path
			if ( !PathIsClosed )
			{		
				// make a new node path
				NodePath * pClosedPathNode = 
					((NodePath *)pNewNode)->MakeNodePathFromAttributes(ContourToolFlatness, pAttrMap, FALSE, m_IncludeLineWidths);

				m_pPathList[m_Count].ClearPath(FALSE);
				m_pPathList[m_Count].CloneFrom(((NodePath *)pNewNode)->InkPath);

				// delete the node
				pNewNode->DeleteChildren(pNewNode->FindFirstChild());
				
				delete pNewNode;
				pNewNode = pClosedPathNode;
			}

			NodePath* pStrokedPath = pCreatedByNode->GetVariableWidthStrokePath();
			BOOL bStroke = FALSE;
			
			if (pStrokedPath != NULL)
			{
				delete pNewNode;
				pNewNode = pStrokedPath;
				pStrokedPath = NULL;
				bStroke = TRUE;
			}

			// Karim 30/06/2000
			// the path-clipping code cannot cope with a straight line (two handles),
			// so we need to explicitly check for this case.
			if ( ((NodePath *)pNewNode)->InkPath.GetNumCoords() == 2 )
			{
				m_pPathList[m_Count].ClearPath(FALSE);
				m_pPathList[m_Count].CloneFrom(((NodePath *)pNewNode)->InkPath);
			}
			else
			{
				// clip the path to ensure its validity
				if (!bStroke)
				{
					Path BlankPath;
					BlankPath.Initialise();
					BlankPath.ClipPathToPath(((NodePath *)pNewNode)->InkPath, &(m_pPathList[m_Count]), 3);
				}
			}

			// has the clipping failed ? if so, just clone.
			if (m_pPathList[m_Count].GetNumCoords() < 2)
			{
				m_pPathList[m_Count].ClearPath(FALSE);
				m_pPathList[m_Count].CloneFrom(((NodePath *)pNewNode)->InkPath);
			}
			
			// take the path from the node, and add it to my path
			m_pPathList[m_Count++].TryToClose();
		}
	}
	
	// delete the node
	Node *pChild = pNewNode->FindFirstChild();
	Node *pNextNode = NULL;

	while (pChild)
	{
		pNextNode = pChild->FindNext();
		pChild->UnlinkNodeFromTree();
		
		delete pChild;
		pChild = pNextNode;
	}
	
	if (pAttrMap)
	{
		pAttrMap->DeleteAttributes();
		delete pAttrMap;
	}

	if(pNewNode)
		delete pNewNode;
	
	return TRUE;
}


/*
IMPLEMENT_SIMPLE_MODULE( ContourModule, MODULEID_BLEND, ContourTool,
							"Contour Tool", "To contour objects", "DavidMc" );
							*/
