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
// The Selector Tool

/*
*/

#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "stockcol.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "csrstack.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "oilkeys.h"
#include "oilfiles.h"
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodedoc.h"
//#include "selmsg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docvmsg.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "selector.h"
#include "selinfo.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
#include "keypress.h"
#include "vkextra.h"
#include "insertnd.h"
#include "nodetxts.h"

#include "tranlate.h"
#include "opscale.h"
#include "opscale2.h"
#include "opsquash.h"
#include "opshear.h"
#include "opflip.h"
#include "oprotate.h"
#include "guides.h"
#include "layer.h"
//#include "basebar.h"

//#include "resource.h"
//#include "viewrc.h"
//#include "justin.h"
//#include "markn.h"
//#include "mario.h"
//#include "rik.h"
//#include "selstr.h"

//#include "will2.h"
//#include "oleprefs.h"
#include "menuops.h"

// knowledge of these classes is required by tab-selection.
// remove these if/when the Iterate...() methods are virtual-ised.
#include "nodebldr.h"


//Matt 11/11/00 - I only wanted a function from the next line...
#include "slicehelper.h"	//For helper functions

//But I had to include all of these to get it to work!!...
//#include "cxfrech.h"		//For CamelotRecordHandler - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"		//For UserAttr
#include "tmpltatr.h"		//For TemplateAttribute

#include "cartprov.h"


DECLARE_SOURCE( "$Revision$" );

// Karim 09/08/2000 - enable the new tab selection-iteration code.
#define NEW_SELECTION_TAB_ITERATION

#define MIN(a, b)	((a) < (b) ? (a) : (b))
#define MAX(a, b)	((a) < (b) ? (b) : (a))
#define CURSORID_UNSET 5000

// Macro defining whether text should come from resources or should be direct...
#define T(res,string) res
//#define T(res,string) string	//<<<<

// These are still char* while we wait for resource technology to be developed for modules
TCHAR* SelectorTool::FamilyName = _T("Selection Tools");
TCHAR* SelectorTool::ToolName   = _T("Selector Tool");
TCHAR* SelectorTool::Purpose    = _T("Selecting objects and editing the selection");
TCHAR* SelectorTool::Author     = _T("Justin Flude & Phil Martin");

SelectorInfoBarOp* SelectorTool::pInfoBarOp = NULL;
BOOL SelectorTool::fSelectorIsCaching = FALSE;

INT32 SelectorTool::CursorStackID= CURSORID_UNSET;
BOOL SelectorTool::bNormalClickCheckProfileDialog = FALSE;
BOOL SelectorTool::bGlineSAllowed = TRUE;


/*********************************************************************************************

	Preference:	BlobBorder
	Section:	Selection Blob Sizes
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		0 ... 10 (millimetres)
	Purpose:	Determines how wide a border, in millimetres, to inflate the bounding
				rectangle of the selected object, when drawing blobs around it.  The
				default is 4 mm.

**********************************************************************************************/

INT32 SelectorTool::nBlobBorder = 4;

// This defines the number of millipoints in a millimetre.
const MILLIPOINT nMPperMM = 2835L;



/*********************************************************************************************

	Preference:	InitialBlobs
	Section:	Selector Tool
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		0 ... UINT_MAX
	Purpose:	Determines which blobs should be initially displayed.  This should really be
				done by the blob manager!

**********************************************************************************************/

unsigned SelectorTool::fBlobPref = 1;		// by default, show only bounds/rotate blobs



/*********************************************************************************************

	Preference:	AllowCorelToggleClick
	Section:	Selector Tool
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		FALSE ... TRUE
	Purpose:	If TRUE (the default) allows a single click on a selected object to toggle
				between bounds blobs & rotate blobs, for compatibility with Corel Draw.  This
				toggling option not allowed when Object blobs are shown, it would be a pain.

**********************************************************************************************/

BOOL SelectorTool::fAllowCorelToggleClick = TRUE;



/*********************************************************************************************

	Preference:	SelectUnderLikeArtWorks
	Section:	Selector Tool
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Range:		FALSE ... TRUE
	Purpose:	If TRUE then the select-under feature, Alt-Click, works exactly like ArtWorks,
				so that an Alt-Click on an unselected object selects that object rather than
				the object underneath.  If FALSE then Alt-Click always selects an object
				underneath the clicked object, irrespective of whether the clicked object
				was selected or not.  The default is FALSE, as this is consistent with the
				way that select-inside works.

**********************************************************************************************/

BOOL SelectorTool::fSelectUnderLikeArtWorks = FALSE;



/*********************************************************************************************

	Preference:	SlaveLineWidthToButton

	Section:	Selector Tool
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/96

	Range:		Boolean
	Purpose:	Controls whether attributes (line widths mainly) are considered when 
				calculating bounding rectangles for objects. This is used when displaying
				and editing width/height information in the infobar.

				If TRUE, the "Scale Line Widths" button will also control the behaviour
				of the function SelectorInfoBarOp::DontConsiderAttrs() - if the button is
				depressed, Attrs will not be considered, and vice versa.

				If FALSE, the behaviour will be fixed, and controlled by the preference
				"ConsiderLineWidths" in the "Selector Tool" section.

				The default setting (TRUE) is to determine the state from the button setting.

	Notes:		This preference is read by our associated SelectorInfoBarOp
				This preference is not currently written by camelot

**********************************************************************************************/

BOOL SelectorTool::fSlaveLineWidthToButton = TRUE;



/*********************************************************************************************

	Preference:	ConsiderLineWidths

	Section:	Selector Tool
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/96

	Range:		Boolean
	Purpose:	Controls whether attributes (line widths mainly) are considered when 
				calculating bounding rectangles for objects. This is used when displaying
				and editing width/height information in the infobar.

				This preference is IGNORED if the "SlaveLineWidthToButton" pref is TRUE.

				The default setting (FALSE) is to ignore line widths in these displays.

	Notes:		This preference is read by our associated SelectorInfoBarOp
				This preference is not currently written by camelot

**********************************************************************************************/

BOOL SelectorTool::fConsiderLineWidths = FALSE;



/*********************************************************************************************

	Preference:	fUseScalingFix

	Section:	Selector Tool
	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/11/97

	Range:		Boolean
	Purpose:	Controls whether my scaling fix is used

				The default setting (FALSE) is to *not* use the new scaling fix

	Notes:		This preference is read by our associated SelectorInfoBarOp
				This preference is not currently written by camelot

**********************************************************************************************/

BOOL SelectorTool::fUseScalingFix = FALSE;



/*********************************************************************************************

	Preference:	bPageDoubleClickOpenFile

	Section:	Selector Tool
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/2005

	Range:		Boolean
	Purpose:	Controls whether double-clicking on the page will show the Open dialog

**********************************************************************************************/

BOOL SelectorTool::bPageDoubleClickOpenFile = FALSE;



// This is our own permanent pointer to the blob manager.
BlobManager* SelectorTool::pBlobManager = NULL;

// Run-time type checking etc etc
CC_IMPLEMENT_DYNCREATE(OpSelectorDragBox, Operation)
CC_IMPLEMENT_DYNCREATE(OpDragRotateCentre, Operation)
CC_IMPLEMENT_MEMDUMP(SelectorTool, DragTool)

// report memory line info
#define new	 CAM_DEBUG_NEW



/********************************************************************************************

>	SelectorTool::SelectorTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Dummy Constructor - sets member pointers to NULL and initialises the
				record of which blobs are displayed (initially only "tiny" blobs).
	SeeAlso:	SelectorTool::Init

********************************************************************************************/

SelectorTool::SelectorTool()
  :	//pInfoBarOp(NULL),
	fRotateCentreIsValid(FALSE),
	eCurrentBlobs(BOUNDS_BLOBS),
	fShowToolBlobs(TRUE),
	fValidSelectedBlob(FALSE)
{
	// initialise all our member pointers to NULL.

	SelectionSpread	= NULL;
	SelectRange		= NULL;
	StartSpread		= NULL;

	pClickSimpleNode	= NULL;
	pClickCompoundNode	= NULL;
	pLastClickNode		= NULL;
	pPreProcClickNode	= NULL;

	pNormalCursor		= NULL;
	pAdjustCursor		= NULL;
	pUnderCursor		= NULL;
	pInsideCursor		= NULL;
	pUnderAdjustCursor	= NULL;
	pInsideAdjustCursor	= NULL;
	pLeafCursor			= NULL;
	pLeafAdjustCursor	= NULL;

	pALLCursor			= NULL;
	pNWSECursor			= NULL;
	pNESWCursor			= NULL;
	pNSCursor			= NULL;
	pWECursor			= NULL;
	pGradFillCursor		= NULL;
	pDragRotateCursor	= NULL;
	pHorzGuideCursor	= NULL;
	pVertGuideCursor	= NULL;

	bGlineSAllowed		= TRUE;
	m_bComputeAreaDetails	= FALSE;

	// Pre-load the resource we're going to use
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_6) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_3) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_1) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_8) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_45) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_45) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_27) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_27) );
	(CamArtProvider::Get())->FindBitmap( _R(IDBMP_HANDLE_CENTRE) );
}



/********************************************************************************************

>	virtual SelectorTool::~SelectorTool()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Purpose:	Destructor (Virtual). Does nothing.

********************************************************************************************/

SelectorTool::~SelectorTool()
{
	// empty
}



/********************************************************************************************

>	virtual BOOL SelectorTool::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Returns:	FALSE if it does not want to be created, TRUE otherwise
	Purpose:	Used to check if the Tool was properly constructed
	SeeAlso:	SelectorTool::SelectorTool

********************************************************************************************/

BOOL SelectorTool::Init()
{
/*	// Claim the left Alt for our tool.
	ToolKeyClaimDetails Mods;
	Mods.Value = 0;
	Mods.Keys.Alternative1 = TRUE;
	ClaimToolKey((Tool*) this, Mods);
*/
	// Read in the preferences.
	if (!ReadPrefs())
	{
		TRACE( _T("Couldn't read Selector tool's preferences!\n"));
		return FALSE;
	}

	// Declare the OpDescriptors of each transform.
	if (!OpTranslateTrans::Declare() ||
		!OpRotateTrans::Declare() ||
		!OpScaleTrans::Declare() ||
//		!OpScale2Trans::Declare() ||
		!OpShearTrans::Declare() ||
		!OpSquashTrans::Declare() ||
		!OpFlipTrans::Declare() ||
		!OpSelectorDragBox::Declare() ||
		!OpDragRotateCentre::Declare())
	{
		ENSURE(FALSE, "Couldn't 'declare' all OpDescriptors in SelectorTool::Init");
		return FALSE;
	}

	// This should be set to NULL by default. It will be set properly below, if
	// everthing is working as it should
	pInfoBarOp = new SelectorInfoBarOp;
	
	// In the debug version make sure we can create a bar.
	ENSURE(pInfoBarOp != NULL, "Failed to create selector tool info-bar");
	if (pInfoBarOp == NULL) return FALSE;

	// Get a permanent pointer to the blob manager.
	pBlobManager = GetApplication()->GetBlobManager();
	ENSURE(pBlobManager != NULL, "Selector tool: couldn't get blob manager");
	if (pBlobManager == NULL) return FALSE;

	// Get a permanent pointer to the application's SelRange object.
	SelectRange = GetApplication()->FindSelection();

	// set the initial state of the blob buttons
	bsBlobStyle.Object   = (fBlobPref & 2) != 0;
	bsBlobStyle.Fill 	 = (fBlobPref & 4) != 0;
	bsBlobStyle.Artistic = (fBlobPref & 8) != 0;
	bsBlobStyle.ToolObject = TRUE;
	bsBlobStyle.Effect  = FALSE;

	// Object blobs & Tiny blobs are mutually exclusive.
	bsBlobStyle.Tiny 	 = !bsBlobStyle.Object;
	
	// If we are displaying tool blobs then decide whether they are bounds or rotate blobs.
	fShowToolBlobs = (fBlobPref & 1) != 0;
	eCurrentBlobs = (pInfoBarOp->InRotateMode()) ? ROTATE_BLOBS : BOUNDS_BLOBS;
	
	// Success!!
	return TRUE;
}



/********************************************************************************************

>	static BOOL SelectorTool::ReadPrefs()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful.
	Purpose:	Reads settings concerning the selector tool from the .INI file.
	Errors:		-
	SeeAlso:	SelectorTool::Init

********************************************************************************************/

BOOL SelectorTool::ReadPrefs()
{
	return	Camelot.DeclareSection(TEXT("Selector Tool"), 10) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("BlobBorder"),
								&nBlobBorder, 0, 10) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("LockAspect"),
								&SelectorInfoBarOp::fLockAspect, FALSE, TRUE) &&
//		   Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("LeaveCopy"),
//		   					   &SelectorInfoBarOp::fLeaveCopy,  FALSE, TRUE) &&
//		   Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("TransFills"),
//		   					   &SelectorInfoBarOp::fTransFills, FALSE, TRUE) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("ScaleLines"),
								&SelectorInfoBarOp::fScaleLines, FALSE, TRUE) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("InitialBlobs"),
								&SelectorTool::fBlobPref, 0, UINT_MAX) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("AllowCorelToggleClick"),
								&SelectorTool::fAllowCorelToggleClick, FALSE, TRUE) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("SelectUnderLikeArtWorks"),
								&SelectorTool::fSelectUnderLikeArtWorks, FALSE, TRUE) &&

			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("SlaveLineWidthToButton"), &fSlaveLineWidthToButton) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("ConsiderLineWidths"), &fConsiderLineWidths) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("UseScalingFix"), &fUseScalingFix) &&
			Camelot.DeclarePref(TEXT("Selector Tool"), TEXT("PageDoubleClickOpenFile"), &bPageDoubleClickOpenFile)
			;
}



/********************************************************************************************

>	virtual void SelectorTool::Describe(void *InfoPtr)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		InfoPtr - A pointer to a tool info block. It is passed cast to void* as
				the version of the tool is unknown at this point. Later versions of the
				Tool class may have more items in this block, that this tool will not use
	Outputs:	InfoPtr - The structure pointed to by InfoPtr will have had all the info
				that this version of the Tool knows about
	Purpose:	Allows the tool manager to extract information about the tool

********************************************************************************************/

void SelectorTool::Describe(void *InfoPtr)
{
	// Cast structure into the latest one we understand.
	ToolInfo_v1* Info = (ToolInfo_v1*) InfoPtr;
	Info->InfoVersion = 1;
	
	// You should always have this line.
	Info->InterfaceVersion = GetToolInterfaceVersion();  
		
	// These are all arbitrary at present.
	Info->Version = 1;
	Info->ID      = GetID();
	Info->TextID  = _R(IDS_SELECTOR_TOOL);
        
	Info->Family  = FamilyName;
	Info->Name    = ToolName;
	Info->Purpose = Purpose;
	Info->Author  = Author;
//	Info->ToolBoxBitmap = _R(IDB_SELR_TOOLBOX);
//	Info->ToolBoxBitmapActive = _R(IDB_SELR_TOOLBOX_ACTIVE);
	
	Info->InfoBarDialog = 0;
	Info->BubbleID = _R(IDBBL_SEL_TOOLICON);
}



/********************************************************************************************

>	virtual void SelectorTool::SelectChange(BOOL isSelected)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		isSelected	-	TRUE if this tool is becoming current, FALSE
								otherwise
	Purpose:	Called whenever this tool is selected or deselected.

				Performs jobs related to changing tools, eg changing the cursor to a
				selector cursor, changing the displayed tool blobs to selector tool blobs,
				changing the preferences for BlobManager-rendered blobs.

	Errors:		Debug warning if creating the cursor fails.

********************************************************************************************/

void SelectorTool::SelectChange(BOOL isSelected)
{
	if (isSelected)
	{
		// Load the grad-fill cursor etc.
		pGradFillCursor     = new Cursor(this, _R(IDCSR_SEL_GRADPOINT));
		pNormalCursor       = new Cursor(this, _R(IDCSR_SEL_NORMAL));
		pAdjustCursor       = new Cursor(this, _R(IDCSR_SEL_ADJUST));
		pUnderCursor        = new Cursor(this, _R(IDCSR_SEL_UNDER));
		pInsideCursor       = new Cursor(this, _R(IDCSR_SEL_INSIDE));
		pUnderAdjustCursor  = new Cursor(this, _R(IDCSR_SEL_UNDERADJUST));
		pInsideAdjustCursor = new Cursor(this, _R(IDCSR_SEL_INSIDEADJUST));
		pALLCursor  	   	= new Cursor(this, _R(IDCSR_SEL_GRADPOINT));
		pLeafCursor			= new Cursor(this, _R(IDCSR_SEL_LEAF));
		pLeafAdjustCursor	= new Cursor(this, _R(IDCSR_SEL_LEAFADJUST));
		pNWSECursor 	   	= new Cursor(this, _R(IDCSR_SIZENWSE));
		pNESWCursor 	   	= new Cursor(this, _R(IDCSR_SIZENESW));
		pNSCursor   	   	= new Cursor(this, _R(IDCSR_SIZENS));
		pWECursor   	   	= new Cursor(this, _R(IDCSR_SIZEWE));
		pDragRotateCursor	= new Cursor(this, _R(IDCSR_SEL_GRADPOINT));
		pHorzGuideCursor	= new Cursor(this, _R(IDCSR_SEL_HGUIDE));
		pVertGuideCursor	= new Cursor(this, _R(IDCSR_SEL_VGUIDE));
		
		// This tool has just been selected.  Push an appropriate cursor.
		CursorStackID = CursorStack::GPush(pNormalCursor);

		// Initially we know of no selection, above or below.
		SelectionSpread = NULL;
		pClickSimpleNode = NULL;
		pClickCompoundNode = NULL;
		pLastClickNode = NULL;
		pPreProcClickNode = NULL;

		// Reset these flags used within the click logic etc.
		fIsBlobDrag = fPossibleToggleClick = fIgnoreSelChange = FALSE;
		fAllowIdleProcessing = fMouseHasMoved = TRUE;

		// Create and display my info bar please
		if (pInfoBarOp != NULL)
		{
			pInfoBarOp->pSelectorTool = this;
			if(pInfoBarOp->WindowID == NULL)
			{
				// set its pointer to me and call its create() function to get it to set its
				// controls
				pInfoBarOp->Create();
			}
			else
			{
			 	pInfoBarOp->Open();
				pInfoBarOp->MakeCurrent();
			}
        }

		///////////////////////////////////////////////////////////////////////////////
		// Version 1.0 of Camelot doesn't have ArtLines, so make sure their blobs
		// are always OFF.
		bsBlobStyle.Artistic = FALSE;
		bsBlobStyle.Effect  = FALSE;
		///////////////////////////////////////////////////////////////////////////////
		
		// Which blobs do I want displayed
// >>>> Commented on advice from Rik
//		if (Document::GetCurrent() != NULL)
//		{
			// Tell the blob manager about our blobs.
			if (pBlobManager != NULL)
				pBlobManager->ToolInterest(bsBlobStyle);
//		}

		// Note that whenever we switch to the selector we always start in bounds
		// rather than rotate mode.
		eCurrentBlobs = BOUNDS_BLOBS;
		SelectorInfoBarOp::fRotateMode = FALSE;
		if (pInfoBarOp != NULL)
		{
			// Make sure the info-bar reflects the state of this.
			pInfoBarOp->SetRotateMode(SelectorInfoBarOp::fRotateMode);
		}

		// Get the initial selection, if any, and render the blobs.
		if (UpdateSelectionInfo()) pBlobManager->RenderToolBlobsOn(this, SelectionSpread, NULL);
	}
	else
	{
		// Deselection - pop the tool's cursor(s).
		CursorStack::GPop(CursorStackID);
		CursorStackID=CURSORID_UNSET;

		// Delete the grad-fill cursor etc.
		delete pGradFillCursor;
		delete pNormalCursor;
		delete pInsideCursor;
		delete pAdjustCursor;
		delete pUnderCursor;
		delete pUnderAdjustCursor;
		delete pInsideAdjustCursor;
		delete pLeafCursor;
		delete pLeafAdjustCursor;
		delete pALLCursor;
		delete pNWSECursor;
		delete pNESWCursor;
		delete pNSCursor;
		delete pWECursor;
		delete pDragRotateCursor;
		delete pHorzGuideCursor;
		delete pVertGuideCursor;
		
		// Use this flag to avoid an unsightly redraw when simply changing tools.
		fSelectorIsCaching = TRUE;
		
		// Hide and destroy the info bar.
		if (pInfoBarOp != NULL)
		{
			// Make sure that the info bars pointer here is set to null and close itself down.
			// But NOT if the application is on the way out
			if (pInfoBarOp->HasWindow() && !GetApplication()->CamelotIsDying() )
				pInfoBarOp->Close();
			pInfoBarOp->pSelectorTool = NULL;
			pInfoBarOp->Delete();
		}

		fSelectorIsCaching = FALSE;
		
		// ensure any tool object blobs are removed.
		if (pBlobManager != NULL)
		{
			BlobStyle bsRemoves;
			bsRemoves.ToolObject = TRUE;
			pBlobManager->RemoveInterest(bsRemoves);
		}

		// If there is a spread with the selection on it, there is still a view,
		// and the tool blobs are being displayed, then render them off
		pBlobManager->RenderToolBlobsOff(this, SelectionSpread, NULL);
	}
}

/********************************************************************************************

>	static void SelectorTool::UnCacheInfoBar(BOOL Force)

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		Force Flag - will be deleted even if current
	Outputs:	-
	Returns:	-
	Purpose:	deletes the cached infobar and sets it's ID to NULL -this is normally called
				when the selector is not the current tool - 
				it can be forced however(on close down etc..)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::UnCacheInfoBar(BOOL Force)
{
	if (pInfoBarOp)
	{
		// normally we would only uncache the infobar if we were not the current tool
		// but there are occasions when we might need to force it
		if (pInfoBarOp->pSelectorTool == NULL || Force)
		{ 
			pInfoBarOp->Delete();	
			pInfoBarOp->WindowID = NULL;
		}
	}
}




/********************************************************************************************

>	virtual void SelectorTool::OnMouseMove(DocCoord dcPos, Spread* pSpread, ClickModifiers mods)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		dcPos		the current position of the mouse (spread coord)
				pSpread		the spread containing the mouse
				mods		the mouse modifiers, eg. Adjust, Constrain etc
	Outputs:	-
	Returns:	-
	Purpose:	Called by the tool system for the current tool when the mouse is moved
				outside of a drag.  Checks if the mouse is over one of the tool's blobs,
				changing the cursor appropriately if it is.
	Errors:		-
	SeeAlso:	SelectorTool::ChangeCursor; SelectorTool::IsNearBlob

********************************************************************************************/

void SelectorTool::OnMouseMove(DocCoord dcPos, Spread* pSpread, ClickModifiers mods)
{
	// Note that the mouse has moved so we know to update the status-bar text on the next
	// idle event.
	fMouseHasMoved = TRUE;

	// Make sure the cursor reflects which keys are down.  It is possible for the modifier
	// keys to change state without key-events being generated, so we must check this on
	// every mouse move.
//	ResetCursorNow();
	// Make sure the cursor reflects which keys are down.  It is possible for the modifier
	// keys to change state without key-events being generated, so we must check this on
	// every mouse move.
	// (That means prepare the global vars for us by GetClickModifiers in FigureUserFeedback)
//	StartSpread = pSpread;
//	ClickMods = mods;
//	ClickStart = dcPos;

	// If there isn't any selection, or it's in a different spread, then do nothing.
//	if (SelectionSpread == NULL || SelectionSpread != pSpread)
//		return;
	
	String_256 str;
	Cursor* pPointerShape;

	FigureUserFeedback(pSpread, dcPos, mods, FALSE, &str, &pPointerShape);

	if (!(str.IsEmpty()))
		SetStatusText(&str);

	if (CursorStackID!=CURSORID_UNSET)
		CursorStack::GSetTop(pPointerShape,CursorStackID);
}



/********************************************************************************************

>	virtual BOOL SelectorTool::OnKeyPress(KeyPress* pKey)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pKey			pointer to a key-press object representing a keystroke.
	Outputs:	-
	Returns:	TRUE if the key-event is handled, FALSE if not.
	Purpose:	Called when a key is pressed of released.  If the key is a "click modifier"
				such as the ALT or CTRL key then the cursor is changed to indicate whatever
				the tool can do with that modifier.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::OnKeyPress(KeyPress* pKey)
{
	// Find the current state of the "click" keyboard modifiers...
	ClickMods = ClickModifiers::GetClickModifiers();

	switch (pKey->GetVirtKey())
	{

// >>>> BODGE! These cases are only here to prevent the messages
//				being passed on to other key handlers (very slow!)
	case CAMKEY(CC_MOD_ADJUST):						// bit 0 of fKeyStates (SHIFT)
		break;

	case CAMKEY(CC_MOD_ALTERNATIVE):					// bit 1 of fKeyStates (ALT)
		break;

	case CAMKEY(CC_MOD_CONSTRAIN):					// bit 2 of fKeyStates (CONTROL)
		break;
// >>>> End

	case CAMKEY(TAB):								// moves selection to next rendered node
		if( pKey->IsConstrain() )
			return FALSE;			// We have other uses for Ctrl+Tab (switch document)

		if (pKey->IsPress()) HandleTabKey(ClickMods.Adjust);
		break;

	case CAMKEY(1):									// toggle tool blobs
		if (pKey->IsModified()) 
		{
			// if Ctrl/Alt/Shift is down, pass the message on to another handler
			// instead of 'absorbing' the keypress
			return FALSE;
		}
		if (pKey->IsPress() && !pKey->IsModified())
		{
			BoundsButtonChange();
			if (pInfoBarOp != NULL)
			{
				pInfoBarOp->SetLongGadgetValue(_R(IDC_SEL_SHOWBOUNDSBLOBS), fShowToolBlobs, FALSE);
			}
		}
		break;

	case CAMKEY(2):									// toggle edit blobs
		if (pKey->IsModified()) 
		{
			return FALSE;
		}
		if (pKey->IsPress() && !pKey->IsModified())
		{
			BlobStyle bs(TRUE);
			SelectionBlobChange(bs);
			if (pInfoBarOp != NULL)
			{
				pInfoBarOp->SetLongGadgetValue(_R(IDC_SEL_SHOWOBJECTBLOBS), bsBlobStyle.Object, FALSE);
			}
		}
		break;

	case CAMKEY(3):									// toggle fill blobs
		if (pKey->IsModified()) 
		{
			return FALSE;
		}
		if (pKey->IsPress() && !pKey->IsModified())
		{
			BlobStyle bs(FALSE, FALSE, TRUE);
			SelectionBlobChange(bs);
			if (pInfoBarOp != NULL)
			{
				pInfoBarOp->SetLongGadgetValue(_R(IDC_SEL_SHOWFILLBLOBS), bsBlobStyle.Fill, FALSE);
			}
		}
		break;

	case CAMKEY(HOME):								// select first object in render order
		if (pKey->IsPress())
		{
			if (SelectionSpread != NULL) NodeRenderableInk::DeselectAll();
			HandleTabKey(FALSE);
		}
		break;

	case CAMKEY(END):								// select last object in render order
		if (pKey->IsPress())
		{
			if (SelectionSpread != NULL) NodeRenderableInk::DeselectAll();
			HandleTabKey(TRUE);
		}
		break;

	case CAMKEY(4):									// toggle bounds/rotate blobs
		if (pKey->IsModified()) 
		{
			return FALSE;
		}
		if (pKey->IsPress() && !pKey->IsModified())
		{
			RotateButtonChange(SelectorInfoBarOp::fRotateMode = !SelectorInfoBarOp::fRotateMode);
			if (pInfoBarOp != NULL)
			{
				pInfoBarOp->SetLongGadgetValue(_R(IDC_SEL_ROTATEBUTTON), SelectorInfoBarOp::fRotateMode,
											   FALSE);
			}
		}
		break;

	default:									// not interested in processing this
		return FALSE;
	}	

	// If we processed a click modifier then update the cursor and return that we processed
	// the keystroke.
		SetKeyDownCursor(ClickMods);

	// Yes, we processed this key-event.
	return TRUE;
}



/********************************************************************************************

>	void SelectorTool::SetKeyDownCursor(ClickModifiers cmods)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Decodes the bit-field fKeyStates, indicating which combination of modifier
				keys are down, and sets the cursor appropriately.
	Errors:		-
	SeeAlso:	SelectorTool::OnKeyPress; SelectorTool::OnMouseMove

********************************************************************************************/

void SelectorTool::SetKeyDownCursor(ClickModifiers cmods)
{
	// Get current position information for call to change pointer shape...
	Spread*  pSpread;
	DocCoord dcMousePos;
	if( DocView::GetCurrentMousePos( &pSpread, &dcMousePos ) &&
		Tool::GetCurrentID()== TOOLID_SELECTOR )
PORTNOTE("other", "Removed bar drag usage")
#ifndef EXCLUDE_FROM_XARALX
		&& !BaseBar::IsDragging())
#endif
	{
//		StartSpread = pSpread;
//		ClickStart = dcMousePos;
		// Call nice central routine to figure out what pointer shape to show...
		// (Set the status bar text while we're at it.)
		String_256 Str;
		Cursor* pPtr;
		FigureUserFeedback(pSpread, dcMousePos, cmods, TRUE, &Str, &pPtr);
		if (CursorStackID!=CURSORID_UNSET)
			CursorStack::GSetTop(pPtr,CursorStackID);
		if (!(Str.IsEmpty()))
			SetStatusText( &Str );
	}
}



/********************************************************************************************

>	void SelectorTool::ResetCursorNow()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when a mouse button-up event or end-of-drag event occurs.  It resets
				the cursor according to which modifier keys are up or down, without a key
				event having to take place.
	Errors:		-
	SeeAlso:	SelectorTool::SetKeyDownCursor; SelectorTool::HandleDragClick;
				SelectorTool::HandleButtonUp; SelectorTool::OnMouseMove

********************************************************************************************/

void SelectorTool::ResetCursorNow()
{
	// Make sure the cursor reflects which keys are down.  It is possible for the modifier
	// keys to change state without key-events being generated, so we must check this on
	// every mouse move.
	SetKeyDownCursor(ClickModifiers::GetClickModifiers());
}



/********************************************************************************************

>	static void SelectorTool::AllowIdleWork(BOOL fIsAllowed)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Inputs:		fIsAllowed			if TRUE then the tool will act on idle events, if FALSE
									the tool will ignore idle events.
	Outputs:	-
	Returns:	-
	Purpose:	Turns idle processing on or off.  During idle events the selector tool
				updates the status bar, so if a drag operation, for instance, wants to
				place its own text in the status bar while it is running it should call
				this function at its start and at its end.
	Errors:		-
	SeeAlso:	SelectorTool::OnIdle

********************************************************************************************/

void SelectorTool::AllowIdleWork(BOOL fIsAllowed)
{
	// Make sure the selector is the current tool.
	ERROR3IF(Tool::GetCurrent() == NULL || Tool::GetCurrent()->GetID() != TOOLID_SELECTOR,
				"Selector isn't current tool in SelectorTool::AllowIdleWork");

	if (Tool::GetCurrent() != NULL && Tool::GetCurrent()->GetID() == TOOLID_SELECTOR)
	{
		// Set the flag.
#ifndef SELECTION_AREA_FEATURE
		((SelectorTool*) Tool::GetCurrent())->fAllowIdleProcessing = fIsAllowed;
#else
		((SelectorTool*) Tool::GetCurrent())->fAllowIdleProcessing = fIsAllowed;
//		((SelectorTool*) Tool::GetCurrent())->fAllowIdleProcessing = (fIsAllowed || m_bComputeAreaDetails);
#endif
	}
// >>>>
//((SelectorTool*) Tool::GetCurrent())->fAllowIdleProcessing = FALSE;
}



/********************************************************************************************

>	virtual BOOL SelectorTool::OnIdle()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if idle events are still required (currently always returns TRUE).
	Purpose:	Called on idle events.  Performs hit-detection on the current mouse position
				and updates the status-bar text accordingly.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::OnIdle()
{
#ifdef SELECTION_AREA_FEATURE
	if (m_bComputeAreaDetails)
	{
		BOOL bNeedsMoreTime = SelectRange->GetAreaDetailsWhileIdle(&xlSelectionArea, &xlSelectionPerim);
		if (!bNeedsMoreTime)
		{
			pInfoBarOp->SetEdit_OnSelectionChange();
			pInfoBarOp->UpdateAllRecords();
			m_bComputeAreaDetails = FALSE;
		}
	}
#endif

return FALSE;
/*	// If the cursor is already over a blob then do nothing, as OnMouseMove will have
	// handled it already.  We also have a "global" flag to turn idle processing on or off,
	// particularly useful during the running of drag Operations.
//	if (!fMouseHasMoved || !fAllowIdleProcessing || fCursorOverBlob || fCursorOverGradFillBlob)
		return TRUE;

	// Clear this flag, so that we remember to check again next time.
	fMouseHasMoved = FALSE;

	// Make sure the selected Doc is current as well.  If there isn't one then what are we
	// doing here?
	Document* pdoc = Document::GetSelected();
	if (pdoc != NULL) pdoc->SetCurrent(); else return TRUE;

	// Check if the mouse is within the "selected" DocView.
	Spread*  pSpread;
	DocCoord dcMousePos;
	if (DocView::GetCurrentMousePos(&pSpread, &dcMousePos))
	{
		// Call nice central routine to figure out what status text to show...
		// Set the status bar text.
		String_256 Str;
		Cursor* pPtr;
		FigureUserFeedback(pSpread, dcMousePos, TRUE, &Str, &pPtr);
		SetStatusText( &Str );
	}

	// We always want lots more idle time, please.
	return TRUE;
*/
}




/********************************************************************************************

>	void SelectorTool::FigureUserFeedback(Spread* pSpread,
											DocCoord dcPos,
											ClickModifiers cmods,
											BOOL DoSlowTests,
											String_256* pStr,
											Cursor** ppPointerShape
											)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		?
	Outputs:	?
	Returns:	-
	Purpose:	This routine computes the status line help and the pointer shape for any
				given mouse position in the Selector tool. It does NOT set either of these
				values but returns them both to the caller. It's up to the caller to decide
				whether to use these values or not.
				The tests done to figure out which pointer shapes, status help (and maybe 
				other stuff one day) is quite complex so it's most efficient to compute them
				all at once and then let the caller decide which ones to actually use.
				The precedence of pointer shapes, status messages and other user feedback is
				determined by the order of the checks within this function (not by flags any
				more!).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::FigureUserFeedback(Spread* pSpread,
											DocCoord dcPos,
											ClickModifiers cmods,
											BOOL DoSlowTests,
											String_256* pStr,
											Cursor** ppPointerShape
											)
{
	// Initialise the status text string and pointer shape to be "null".
	pStr->Empty();
	*ppPointerShape = pNormalCursor;

	if (Tool::GetCurrentID()!=TOOLID_SELECTOR)
		return;

	//-------------------------------------------------------------------------
	// Blob cursors are top priority
	//
	// Check with the fill tool if the cursor is over one of its blobs (it's smart enough
	// to check if fill blobs are being displayed or not).
	UINT32 nStatusTextID;
	if (AttrFillGeometry::CheckForFillControlHit(dcPos, &nStatusTextID))
	{
		*ppPointerShape = pGradFillCursor;
		*pStr = String_256(nStatusTextID);
		return;
	}

	// If we aren't displaying our own tool blobs then don't do anything.
	if (fShowToolBlobs && !(SelectionRect.IsEmpty()))
	{
		// First check if we've hit the rotation centre.
		if (IsRotateCentreClicked(dcPos))
		{
			ChangeCursorAndText(9, pStr, ppPointerShape);
			return;
		}

		// Check if we hit any other bounds/rotate blob.
		INT32 nBlob = BoundsBlobHitTest(dcPos);
		if (nBlob != 0)
		{
			ChangeCursorAndText(nBlob, pStr, ppPointerShape);
			return;
		}
	}

	BOOL DescribeDrag = TRUE;

	//-------------------------------------------------------------------------
	// Next in priority order is the setting of the pointer shape due to
	// key modifiers
	if (!cmods.Adjust && !cmods.Constrain && !cmods.Alternative1)
	{
		*ppPointerShape = pNormalCursor;
		Append(pStr,T(_R(IDS_SEL_MODE0),"Normal select mode:"));
		goto FigureModeSet;
	}

	if (cmods.Adjust && !cmods.Constrain && !cmods.Alternative1)
	{
		*ppPointerShape = pAdjustCursor;
		Append(pStr,T(_R(IDS_SEL_MODE1),"Adjust select:"));
		goto FigureModeSet;
	}

	if (!cmods.Adjust && IsSelectUnderClick(cmods))
	{
		*ppPointerShape = pUnderCursor;
		Append(pStr,T(_R(IDS_SEL_MODE2),"Select under:"));
		goto FigureModeSet;
	}

	if (cmods.Adjust && IsSelectUnderClick(cmods))
	{
		*ppPointerShape = pUnderAdjustCursor;
		Append(pStr,T(_R(IDS_SEL_MODE3),"Adjust select under:"));
		goto FigureModeSet;
	}

	if (!cmods.Adjust && IsSelectMemberClick(cmods))
	{
		*ppPointerShape = pInsideCursor;
		Append(pStr,T(_R(IDS_SEL_MODE4),"Force drag/Select member:"));
		goto FigureModeSet;
	}

	if (cmods.Adjust && IsSelectMemberClick(cmods))
	{
		*ppPointerShape = pInsideAdjustCursor;
		Append(pStr,T(_R(IDS_SEL_MODE5),"Force drag/Adjust select member:"));
		goto FigureModeSet;
	}

	if (!cmods.Adjust && IsSelectLeafClick(cmods))
	{
		*ppPointerShape = pLeafCursor;
		Append(pStr,T(_R(IDS_SEL_MODE6),"Select inside:"));
		goto FigureModeSet;
	}

	if (cmods.Adjust && IsSelectLeafClick(cmods))
	{
		*ppPointerShape = pLeafAdjustCursor;
		Append(pStr,T(_R(IDS_SEL_MODE7),"Adjust select inside:"));
		goto FigureModeSet;
	}

	ERROR3("Out-of-range modifier combination in SelectorTool::FigureUserFeedback?!?");
	return;

FigureModeSet:
	// Check for direct drag mode and say something about it...
	if (IsTranslateShortcut(cmods))
	{
		if (SelectRange->FindFirst())
		{
			*ppPointerShape = pALLCursor;
			Append(pStr,T(_R(IDS_SELINSIDE4),"Drag to move the selected objects"));
		}
		DescribeDrag = FALSE;
	}


		// See if there's a non-selectable object (guideline) under the mouse
		NodeRenderableInk* pPreProNode = FindPreProcessClickNode(pSpread,dcPos,TRUE);
		if (pPreProNode)
		{
			if (IS_A(pPreProNode,NodeGuideline))
			{	
				NodeGuideline* pGuideline = (NodeGuideline*)pPreProNode;
				if (pGuideline->GetType()==GUIDELINE_HORZ)
				{
					*ppPointerShape = pHorzGuideCursor;
					Append(pStr,T(_R(IDS_SELHORZGUIDE),"Drag up or down to move guideline; Drag onto top ruler to delete it"));
					return;
				}

				if (pGuideline->GetType()==GUIDELINE_VERT)
				{
					*ppPointerShape = pVertGuideCursor;
					Append(pStr,T(_R(IDS_SELVERTGUIDE),"Drag left or right to move guideline; Drag onto left ruler to delete it"));
					return;
				}
			}
		}

	//-------------------------------------------------------------------------
	// If we've got time to do the slow tests then go ahead and do them...
	if (DoSlowTests)
	{
		// Perform a hit-test at the mouse position and set the status bar text
		// accordingly.
		// Allow the hit-test to be interrupted if the mouse moves!
		Node* pInterruptedNode = NULL;
		NodeRenderableInk* pSimple = NodeRenderableInk::FindSimpleAtPoint(pSpread, 
																		  dcPos, 
																		  NULL, 
																		  &pInterruptedNode);
		// If hit-test was interrupted then don't say anything about what's under the pointer!
		if (pInterruptedNode!=NULL)
			return;

		NodeRenderableInk* pCompound = NodeRenderableInk::FindCompoundFromSimple(pSimple);

		// If the SelRange thinks it knows what the last selected node was
		// we'll go along with that, otherwise we'll use our own record...
		Node* pLastSelNode = SelectRange->GetLastSelectedNode();
		if ( pLastSelNode==NULL || !pLastSelNode->IsAnObject() )
		{
			pLastSelNode = pLastClickNode;
		}

		// Find out what we should do with the click...
		ClickActionCode action = CLICKACTION_NONE;
		NodeRenderableInk* pActionNode = NULL;
		action = DetermineClickAction(&pActionNode,(NodeRenderableInk*)pLastSelNode,
										pSimple,pCompound,pSpread,dcPos,cmods);

		// Act upon the information...
		switch (action)
		{
			//-------------------------------------------------//
			// No action required...
			case CLICKACTION_NONE:
				break;

			//-------------------------------------------------//
			// A Bounds handle was clicked on...
			case CLICKACTION_BOUNDTOPLEFT:
			case CLICKACTION_BOUNDTOP:
			case CLICKACTION_BOUNDTOPRIGHT:
			case CLICKACTION_BOUNDLEFT:
			case CLICKACTION_BOUNDRIGHT:
			case CLICKACTION_BOUNDBOTTOMLEFT:
			case CLICKACTION_BOUNDBOTTOM:
			case CLICKACTION_BOUNDBOTTOMRIGHT:
			case CLICKACTION_BOUNDTRANSFORMORIGIN:
				break;

			//-------------------------------------------------//
			// Anything that's selected must be deselected...
			case CLICKACTION_SELNONE:
				if (!cmods.Adjust)
				{
					// If there are selected objects append message about clearing.
					if (SelectRange && SelectRange->FindFirst())
						Append(pStr,T(_R(IDS_SELNONE1),"Click to clear the selection"));
					// Message about marquee drag
					if (DescribeDrag)
						Append(pStr,T(_R(IDS_SELNONE4),"Drag to marquee select objects"));
					Append(pStr,T(_R(IDS_SELNONE2),"Move pointer over object to select"));
				}
				else
				{
					// Adjust is held down so describe marquee add.
					if (DescribeDrag)
						Append(pStr,T(_R(IDS_SELNONE5),"Drag to marquee select objects to add to selection"));
					Append(pStr,T(_R(IDS_SELNONE3),"Move pointer over object to add/remove from selection"));
				}
				break;

			//-------------------------------------------------//
			// The action node must be selected or toggled...
			case CLICKACTION_SELNODE:
				if (fShowToolBlobs && !cmods.Adjust && pActionNode->IsSelected())
					if (SelectorInfoBarOp::fRotateMode)
						Append(pStr,T(_R(IDS_SELNODE1),"Click to change to scale & stretch bounds mode; Drag to move the selection"));
					else
						Append(pStr,T(_R(IDS_SELNODE2),"Click to change to rotate & skew bounds mode; Drag to move the selection"));
				else
					Append(pStr,cmods,
								T(_R(IDS_SELNODE3),"Click to select this #1%S alone; Drag to move it"),
								T(_R(IDS_SELNODE4),"Click to select this #1%S"),
								T(_R(IDS_SELNODE5),"Click to deselect this #1%S"),
								pActionNode);
				break;
			case CLICKACTION_SELUNDER:
				Append(pStr,cmods,
							T(_R(IDS_SELUNDER1),"Click to select the #1%S under the last selected object alone"),
							T(_R(IDS_SELUNDER2),"Click to select the #1%S under the last selected object"),
							T(_R(IDS_SELUNDER3),"Click to deselect the #1%S under the last selected object"),
							pActionNode);
				break;
			case CLICKACTION_SELUNDERCYCLE:
				Append(pStr,cmods,
							T(_R(IDS_SELUNDERCYCLE1),"Click to select the top #1%S alone; (Reached the bottom)"),
							T(_R(IDS_SELUNDERCYCLE2),"Click to select the top #1%S; (Reached the bottom)"),
							T(_R(IDS_SELUNDERCYCLE3),"Click to deselect the top #1%S; (Reached the bottom)"),
							pActionNode);
				break;
			case CLICKACTION_SELUNDERFAIL:
				Append(pStr,cmods,
							T(_R(IDS_SELUNDERFAIL1),"Click to select the #1%S alone; (Nothing under the last selected object)"),
							T(_R(IDS_SELUNDERFAIL2),"Click to select the #1%S; (Nothing under the last selected object)"),
							T(_R(IDS_SELUNDERFAIL3),"Click to deselect the #1%S; (Nothing under the last selected object)"),
							pActionNode);
				break;
			case CLICKACTION_SELUNDERFAIL2:
				Append(pStr,cmods,
							T(_R(IDS_SELUNDERFAIL21),"Click to select the #1%S alone; (The last selected object is not under the pointer)"),
							T(_R(IDS_SELUNDERFAIL22),"Click to select the #1%S; (The last selected object is not under the pointer)"),
							T(_R(IDS_SELUNDERFAIL23),"Click to deselect the #1%S; (The last selected object is not under the pointer)"),
							pActionNode);
				break;
			case CLICKACTION_SELINSIDE:
//				Append(pStr,T(_R(IDS_SELINSIDE4),"Drag to move selected objects")); <<<<
				Append(pStr,cmods,
							T(_R(IDS_SELINSIDE1),"Click to select the #1%S member of the last selected object alone"),
							T(_R(IDS_SELINSIDE2),"Click to select the #1%S member of the last selected object"),
							T(_R(IDS_SELINSIDE3),"Click to deselect the #1%S member of the last selected object"),
							pActionNode);
				break;
			case CLICKACTION_SELINSIDECYCLE:
//				Append(pStr,T(_R(IDS_SELINSIDECYCLE4),"Drag to move selected objects")); <<<<
				Append(pStr,cmods,
							T(_R(IDS_SELINSIDECYCLE1),"Click to select the top #1%S alone; (Reached the simplest object)"),
							T(_R(IDS_SELINSIDECYCLE2),"Click to select the top #1%S; (Reached the simplest object)"),
							T(_R(IDS_SELINSIDECYCLE3),"Click to deselect the top #1%S; (Reached the simplest object)"),
							pActionNode);
				break;
			case CLICKACTION_SELINSIDEFAIL:
//				Append(pStr,T(_R(IDS_SELINSIDEFAIL2),"Drag to move selected objects")); <<<<
				Append(pStr,T(_R(IDS_SELINSIDEFAIL1),"Nothing inside this object"));
				break;
			case CLICKACTION_SELINSIDEFAIL2:
//				Append(pStr,T(_R(IDS_SELINSIDEFAIL24),"Drag to move selected objects")); <<<<
				Append(pStr,cmods,
							T(_R(IDS_SELINSIDEFAIL21),"Click to select the #1%S alone; (The pointer is not over a member of the last selected object)"),
							T(_R(IDS_SELINSIDEFAIL22),"Click to select the #1%S; (The pointer is not over a member of the last selected object)"),
							T(_R(IDS_SELINSIDEFAIL23),"Click to deselect the #1%S; (The pointer is not over a member of the last selected object)"),
							pActionNode);
				break;
			case CLICKACTION_SELLEAF:
				Append(pStr,cmods,
							T(_R(IDS_SELLEAF1),"Click to select this #1%S alone"),
							T(_R(IDS_SELLEAF2),"Click to select this #1%S"),
							T(_R(IDS_SELLEAF3),"Click to deselect this #1%S"),
							pActionNode);
				break;
			//-------------------------------------------------//
			default:
				ERROR3("Unknown Click action code!");
				break;
		};

		// If we're in normal click mode (no modifiers down) then remind the user
		// that they can use the modifer keys...
		if (!cmods.Adjust && !cmods.Constrain && !cmods.Alternative1)
			Append(pStr,_R(IDS_SELOPTIONS));
	}
}




/********************************************************************************************

>	BOOL SelectorTool::Append(String_256* pStr, ClickModifiers cmods, UINT32 resID, NodeRenderableInk* pActionNode = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SelectorTool::Append(String_256* pStr, ClickModifiers cmods,
											UINT32 SelectID,
											UINT32 AddID,
											UINT32 RemoveID,
											NodeRenderableInk* pActionNode)
{
	ERROR2IF(SelectID==0,FALSE,"Asked to append a string resource with a null ID");

	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SEL_SEPARATOR));

	String_256 Message;
	UINT32 TemplateID = 0;
	Message.Empty();

	if (!cmods.Adjust || pActionNode==NULL)
		TemplateID = SelectID;
	else
	{
		if (!pActionNode->IsSelected())
			TemplateID = AddID;
		else
			TemplateID = RemoveID;
	}

	if (TemplateID==0)
		TemplateID = SelectID;

	if (pActionNode==NULL)
		*pStr += String_256(TemplateID);
	else
	{
		String_256 sTemplateID(TemplateID);
		String_256 NodeDesc = pActionNode->Describe(FALSE);
		Message._MakeMsg( (TCHAR*) sTemplateID, &NodeDesc);
		*pStr += Message;
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL SelectorTool::Append(String_256* pStr, UINT32 resID)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SelectorTool::Append(String_256* pStr, UINT32 StringID)
{
	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SEL_SEPARATOR));
	*pStr += String_256(StringID);

	return TRUE;
}




/********************************************************************************************

>	BOOL SelectorTool::Append(String_256* pStr, String_256 String)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SelectorTool::Append(String_256* pStr, String_256 String)
{
	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SEL_SEPARATOR));
	*pStr += String;

	return TRUE;
}




/********************************************************************************************

>	BOOL SelectorTool::Append(String_256* pStr,
							  String_256 SelectTemplate,
							  String_256 AddTemplate,
							  String_256 RemoveTemplate,
							  NodeRenderableInk* pActionNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	-
	Errors:		-

********************************************************************************************/

BOOL SelectorTool::Append(String_256* pStr,
						  ClickModifiers cmods,
						  String_256 SelectTemplate,
						  String_256 AddTemplate,
						  String_256 RemoveTemplate,
						  NodeRenderableInk* pActionNode)
{
	ERROR2IF(SelectTemplate.IsEmpty(),FALSE,"Asked to Append an empty message");

	// Append a message to the string, preceding it with a separator if there was something
	// already in the string...
	if (!pStr->IsEmpty())
		*pStr += String_256(_R(IDS_SEL_SEPARATOR));

	String_256 Message;
	String_256* pTemplate;
	Message.Empty();

	if (!cmods.Adjust || pActionNode==NULL)
		pTemplate = &SelectTemplate;
	else
	{
		if (!pActionNode->IsSelected())
			pTemplate = &AddTemplate;
		else
			pTemplate = &RemoveTemplate;
	}

	if (pTemplate->IsEmpty())
		pTemplate = &SelectTemplate;

	if (pActionNode==NULL)
		*pStr += *pTemplate;
	else
	{
		String_256 NodeDesc = pActionNode->Describe(FALSE);
		Message._MakeMsg( (TCHAR*) *pTemplate, &NodeDesc );
		*pStr += Message;
	}

	return TRUE;
}




/********************************************************************************************

>	void SelectorTool::ChangeCursorAndText(INT32 nBlobID, String_256* pStr, Cursor** ppPointerShape)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		nBlobID		the numeric ID of the blob the mouse is over.
	Outputs:	-
	Returns:	-
	Purpose:	Changes the cursor shape according to thew nBlobID parameter.  Each blob
				has its own cursor shape.
	Errors:		-
	SeeAlso:	SelectorTool::OnMouseMove

********************************************************************************************/

void SelectorTool::ChangeCursorAndText(INT32 nBlobID, String_256* pStr, Cursor** ppPointerShape)
{
	// So we can use just one switch table, just for this function, we give bounds and rotate
	// blobs distinct numeric ID's.
	if (eCurrentBlobs == ROTATE_BLOBS) nBlobID += 10;

	// Choose an appropriate cursor.  If the aspect ratio is locked then the cursor over corner
	// blobs is different.
	Cursor* pNewCursor;
	UINT32 nStringID;
	BOOL fLocked = pInfoBarOp->IsAspectLocked();
	switch (nBlobID)
	{
		// Bounds: top-left & bottom-right.
		case 1: case 8:
			pNewCursor = fLocked ? pNWSECursor : pALLCursor;
			nStringID = _R(IDS_SEL_BOUNDSTEXT_1836);
			break;

		// Bounds: top-middle & bottom-middle.
		case 2: case 7:
			pNewCursor = pNSCursor;
			nStringID = _R(IDS_SEL_BOUNDSTEXT_2745);
			break;

		// Bounds: top-right & bottom-left.
		case 3:	case 6:
			pNewCursor = fLocked ? pNESWCursor : pALLCursor;
			nStringID = _R(IDS_SEL_BOUNDSTEXT_1836);
			break;

		// Bounds: left-middle & right-middle.
		case 4: case 5:
			pNewCursor = pWECursor;
			nStringID = _R(IDS_SEL_BOUNDSTEXT_2745);
			break;

		// Rotate: top-left & bottom-right.
		case 11: case 18:
			pNewCursor = pNESWCursor;
			nStringID = _R(IDS_SEL_ROTATETEXT_1836);
			break;

		// Rotate: top-middle & bottom-middle.
		case 12: case 17:
			pNewCursor = pWECursor;
			nStringID = _R(IDS_SEL_ROTATETEXT_2745);
			break;

		// Rotate: top-right & bottom-left.
		case 13: case 16:
			pNewCursor = pNWSECursor;
			nStringID = _R(IDS_SEL_ROTATETEXT_1836);
			break;

		// Rotate: left-middle & right-middle.
		case 14: case 15:
			pNewCursor = pNSCursor;
			nStringID = _R(IDS_SEL_ROTATETEXT_2745);
			break;

		// Rotate: centre of rotation.
		case 9: case 19:
			pNewCursor = pDragRotateCursor;
			nStringID = _R(IDS_SEL_ROTATETEXT_CENTRE);
			break;

		default:
			ENSURE(FALSE, "Bad blob ID in SelectorTool::ChangeCursor");
			return;
	}

	// Set the new cursor and the status bar text.
	*pStr = String_256(nStringID);
	*ppPointerShape = pNewCursor;

}



/********************************************************************************************

>	static void SelectorTool::SetStatusText(String_256* pStr)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Inputs:		pStr			pointer to the string to display
	Outputs:	-
	Returns:	-
	Purpose:	Sets the status bar text to the given string.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::SetStatusText(String_256* pStr)
{
	GetApplication()->UpdateStatusBarText(pStr);
}



/********************************************************************************************

>	static void SelectorTool::SetStatusText(UINT32 nStringID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		nStringID			the numeric identifier of the string resource
	Outputs:	-
	Returns:	-
	Purpose:	Sets the status bar text to the given string.  Alternative to loading the
				string yourself and calling the other SetStatusText function.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::SetStatusText(UINT32 nStringID)
{
	String_256 str(nStringID);
	SetStatusText(&str);
}




/********************************************************************************************

>	BOOL SelectorTool::GetStatusLineText(String_256* ptext, Spread* pSpread,
												DocCoord DocPos, ClickModifiers ClickMods)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/95
	Inputs:		ptext		Pointer to text string to fill in
				pSpread		Pointer to spread containing mouse position
				DocPos		Mouse position within spread
				ClickMods	Click modifiers
	Outputs:	-
	Returns:	-
	Purpose:	Figure out what the status text for the Selector tool is at the given position
				on the given spread with the given click modifiers.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers cmods)
{
	ERROR2IF(ptext==NULL,FALSE,"ptext is NULL!");

//	StartSpread = pSpread;
//	ClickStart = DocPos;
//	ClickMods = cmods;

	Cursor* pPtr;		// Dummy to hold unused pointer shape computed by FigureUserFeedback
	FigureUserFeedback(pSpread, DocPos, cmods, TRUE, ptext, &pPtr);

	if (CursorStackID!=CURSORID_UNSET)
		CursorStack::GSetTop(pPtr,CursorStackID);

	return TRUE;
}




/********************************************************************************************

>	virtual void SelectorTool::OnClick(DocCoord dcPos, ClickType ctType,
									   ClickModifiers cmMods, Spread* pSpread)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		dcPos			the position of the mouse when the click event happened
				ctType			the type of mouse click, eg. CLICKTYPE_DRAG
				cmMods			which buttons & keys were down
				pSpread			points to the spread that was clicked
	Outputs:	-
	Returns:	-
	Purpose:	Dispatches mouse events to the proper handler.  The tool is given a chance
				to handle an event first, which allows it to override the usual behaviour
				if necessary.  If it doesn't handle the event the selected object(s) are
				given a chance.  Finally, any events still not handled are given to the
				selector tool to deal with.
	Errors:		-
	SeeAlso:	SelectorTool::PreProcessClick; SelectorTool::ProcessObjectClick;
				SelectorTool::PostProcessClick

********************************************************************************************/

void SelectorTool::OnClick(DocCoord dcPos, ClickType ctType, ClickModifiers cmMods, Spread* pSpread)
{
	// This divides the trace output into "click events".
/*	if (IsUserName("Phil"))
	{
		TRACE( _T("===========================================================================\n"));
		TRACE( _T("OnClick - Modifiers: "));
		if (cmMods.Adjust) 			TRACE( _T("Adjust  "));
		if (cmMods.Constrain) 		TRACE( _T("Constrain  "));
		if (cmMods.Menu) 			TRACE( _T("Menu  "));
		if (cmMods.Alternative1) 	TRACE( _T("Alt1  "));
		if (cmMods.Alternative2) 	TRACE( _T("Alt2  "));
		TRACE( _T("\n"));
	}
*/
	// We mustn't allow nested drags!
	if (Operation::GetCurrentDragOp() != NULL)
	{
//		TRACEUSER( "JustinF", _T("Drag already running in SelectorTool::OnClick\n"));
		return;
	}

	// If this event marks the beginning of a possible drag then remember the position of the
	// mouse etc.
	if (ctType == CLICKTYPE_SINGLE)
	{
		ClickStart = dcPos;
		StartSpread = pSpread;
	}

	// Save the click modifiers etc to save passing them as parameters to lower-level routines.
	ClickMods = cmMods;
	TypeOfClick = ctType;
	SelectRange = GetApplication()->FindSelection();

	// Clicks are processed in three stages.  If either of these routines return TRUE then we
	// have no further processing to do.
	if (PreProcessClick()) return;
	if (ProcessObjectClick()) return;

	// Here we do the processing normally specific to the selector tool.
	PostProcessClick();
}



/********************************************************************************************

>	BOOL SelectorTool::PreProcessClick()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it handled a mouse event, FALSE if it didn't and the event needs
				further handling.
	Purpose:	This allows a click to be handled before going through the standard selector
				tool click handling code

				At the momemt it sees if there is a node that would like the chance to handle
				the click, and if there is one, it's OnClick() handler is called.
	Errors:		-
	SeeAlso:	SelectorTool::OnClick

********************************************************************************************/

BOOL SelectorTool::PreProcessClick()
{
	// This isn't very nice but at least it's quick!
	// We have to check blob hits BEFORE we allow the Click PreProcessor
	// to look for it's hits because they take precedence.
	UINT32 nStatusTextID;
	if (AttrFillGeometry::CheckForFillControlHit(ClickStart, &nStatusTextID))
		return FALSE;

	// If we aren't displaying our own tool blobs then don't do anything.
	if (fShowToolBlobs && !(SelectionRect.IsEmpty()))
	{
		// First check if we've hit the rotation centre.
		if (IsRotateCentreClicked(ClickStart))
			return FALSE;

		// Check if we hit any other bounds/rotate blob.
		INT32 nBlob = BoundsBlobHitTest(ClickStart);
		if (nBlob != 0)
			return FALSE;
	}

	// Find a node that might be interested on the click before the selector tool handles it
	// in the standard way
	pPreProcClickNode = FindPreProcessClickNode(StartSpread,ClickStart);

	if (pPreProcClickNode != NULL)
	{
		if (bGlineSAllowed)
		{
			// If we have a node that's interested, give 'em a call
			if (pPreProcClickNode->OnClick(ClickStart, TypeOfClick, ClickMods, StartSpread))
				return TRUE;
		}
		else
		{
			if (!IS_A (pPreProcClickNode, NodeGuideline))
			{
				// If we have a node that's interested, give 'em a call
				if (pPreProcClickNode->OnClick(ClickStart, TypeOfClick, ClickMods, StartSpread))
					return TRUE;
			}
		}
	}

	// No preprocess node, or the preprocess node didn't use the click.
	return FALSE;
}


/********************************************************************************************

>	NodeRenderableInk* SelectorTool::FindPreProcessClickNode(Spread* pSpread,
															 DocCoord ClickPos,
															 BOOL Interruptible = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Inputs:		pSpread  = ptr to spread
				ClickPos = point clicked within given spread
				Interruptible = Flag specifying whether function can be aborted by mouse movement
	Outputs:	-
	Returns:	Ptr to a node that requires a chance to process the click itself
				OR NULL if no node is interested
	Purpose:	This scans the nodes in the spread to see if any of them are interested in the click
				before it is processed in the normal way.

				It uses member vars set up in SelectorTool::OnClick()

				Currently, only NodeGuidelines can possibly be interested in this type of click
	Errors:		-
	SeeAlso:	SelectorTool::PreProcessClick(), SelectorTool::OnClick()

********************************************************************************************/
/* 	Implementation notes

	This func makes some assumptions at the moment.
	It assumes that NodeGuideline nodes are the only nodes that will be interested in this sort of click.  
	It also assumes that these nodes will only live as top-level children in guide layers.

	A more general version would scan the entire tree, and call a virtual node func called PreProcClick().

	It also assumes that guidelines can only be obscured by objects on other layers.
	All in all, this is rather specific code written quickly in order to get this ready for v1.1
*/

NodeRenderableInk* SelectorTool::FindPreProcessClickNode(Spread* pSpread,DocCoord ClickPos,BOOL Interruptible)
{
	ERROR2IF(pSpread == NULL,NULL,"pSpread is NULL");

	// Init the ptr to the node found that wants to deal with a pre process click
	NodeRenderableInk* pFoundNode = NULL;
	BOOL Found = FALSE;	// Not found a node yet

	// Start looking for layers in the spread
	Layer* pLayer = pSpread->FindFirstLayer();
	
	while (pLayer != NULL && !Found)
	{
		if (pLayer->IsGuide() && !pLayer->IsLocked() && pLayer->IsVisible())
		{
			// We have a layer that's also a guide layer
			// Now look for the guidelines in this layer

			Node* pNodeInLayer = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
			while (pNodeInLayer != NULL && !Found)
			{
				pFoundNode = (NodeGuideline*)pNodeInLayer;

				// We have found a guideline
				// Get the hit test bounds
				DocRect Rect = pFoundNode->GetBoundingRect(FALSE,TRUE);
				
				// If the click lies within its bounds, then we have found an interested node
				Found = (Rect.ContainsCoord(ClickPos));

				pNodeInLayer = pNodeInLayer->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
			}
		}

		pLayer = pLayer->FindNextLayer();
	}

	if (Found)
	{
		// OK, we have found a node interested in a preprocess click
		// We now have to ensure that it's not being obscured visually by another node
		NodeRenderableInk* pNode = NULL;
		if (Interruptible)
		{
			// Allow the hit-test to be interrupted if the mouse moves!
			Node* pInterruptedNode = NULL;
			pNode = NodeRenderableInk::FindSimpleAtPoint(	pSpread, 
															ClickPos, 
															NULL, 
															&pInterruptedNode);
			// If hit-test was interrupted then don't say anything about what's under the pointer!
			if (pInterruptedNode!=NULL)
				return NULL;
		}
		else
		{
			// Can't be interrupted by mouse movement so just go for it...
			pNode = NodeRenderableInk::FindSimpleAtPoint(pSpread,ClickPos);
		}

		if (pNode)
		{
			// Find out whether the hit node is in front of the guideline or not.
			// If it is, then clear the Found flag.
			Layer* pLowerLayer = (Layer*) pFoundNode->FindParent(CC_RUNTIME_CLASS(Layer));	// The guideline layer
			Layer* pNodeLayer = (Layer*) pNode->FindParent(CC_RUNTIME_CLASS(Layer));		// The layer containing the node
			// Make sure GuideLayer comes after NodeLayer
			do
			{
				pLowerLayer = pLowerLayer->FindNextLayer();		// Find the layer above the last one tested
			}
			while (pLowerLayer && pLowerLayer!=pNodeLayer);		// Keep going while there is a layer
																// and that layer isn't the one we're looking for
			// Get here when either we've run out of layers or we've found the layer we want
			if (pLowerLayer && pLowerLayer==pNodeLayer)			// If found layer above guide layer
				Found=FALSE;									// Then flag that the guideline is obscured
		}
	}

	if (!Found)
		pFoundNode = NULL;

	return pFoundNode;
}


/********************************************************************************************

>	BOOL SelectorTool::ProcessObjectClick()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if an object handled a mouse event, FALSE if no object did and the
				event needs further handling.
	Purpose:	Iterates over the selector objects, calling the OnClick method of each
				NodeRenderableInk and AttrFillGeometry.
	Errors:		-
	SeeAlso:	SelectorTool::OnClick; NodeRenderableInk::OnClick;
				AttrFillGeometry::OnClick

********************************************************************************************/

BOOL SelectorTool::ProcessObjectClick()
{
	// Test for a click on a fill attribute first, as they aren't NodeRenderableInks, and so
	// won't be included in the tests below.
	if (AttrFillGeometry::CheckAttrClick(ClickStart, TypeOfClick, ClickMods, StartSpread))
	{
//		TRACEUSER( "JustinF", _T("Click handled by AttrFillGeometry\n"));
		return TRUE;
	}

	// Look for a selected object to pass the click to.
	Node* pNode;
	if (SelectRange == NULL || (pNode = SelectRange->FindFirst()) == NULL)
	{
//		TRACEUSER( "JustinF", _T("No selected object to handle click\n"));
		return FALSE;
	}

	// If the click is in a different spread to the selection we don't handle it here.
	Spread* pSpread = pNode->FindParentSpread();
	if (pSpread != StartSpread)
	{
//		TRACEUSER( "JustinF", _T("Click on unselected spread\n"));
		return FALSE;
	}

	// Check if the click is on the rotation centre, so that has priority over all other
	// blob actions.
	if (IsRotateCentreClicked(ClickStart))
	{
		// Someone clicked on the rotate centre, that's not going to do anything...
		return FALSE;
	}

	// Next, check if the click is on one of the other tool blobs. Strictly speaking this
	// isn't needed because by definition the bound handles will always be outside the edit
	// handles
	if (BoundsBlobHitTest(ClickStart) != 0)
	{
		// Someone clicked on a bounds handle, that's not going to do anything...
		return FALSE;
	}
	
	if (IsCurrent ())	// only do this processing if we are the current tool!
	{
		// Providing we are displaying object blobs or tool object blobs, try passing the click
		// on to each object in the selection.
		BlobManager* pBlobs = GetApplication()->GetBlobManager();
		if ((pBlobs && pBlobs->GetCurrentInterest().Object) ||
			(pBlobs && pBlobs->GetCurrentInterest().ToolObject) )
		{
			// For all selected objects . . .
			while (pNode != NULL)
			{
				// Is this node ink-renderable?
				if (pNode->IsAnObject())
				{
					// Yes.  Does it want the click?
					if (((NodeRenderableInk*) pNode)->OnClick(ClickStart, TypeOfClick,
															  ClickMods, StartSpread))
					{
						// An object processed the click, so indicate that there's no more to do.
	//					TRACEUSER( "JustinF", _T("Object processed click\n"));
						return TRUE;
					}
				}

				// Try the next object in the selection.
				pNode = SelectRange->FindNext(pNode);
			}
		}

		// If after all that we still haven't processed the click then someone else had better
		// have a go.
	}

	return FALSE;
}



/********************************************************************************************

>	void SelectorTool::PostProcessClick()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Last-chance handler for a mouse event.  Calls HandleSingleClick for a
				CLICKTYPE_SINGLE and HandleDragClick for a CLICKTYPE_DRAG etc.
	Errors:		-
	SeeAlso:	SelectorTool::OnClick; SelectorTool::HandleSingleClick;
				SelectorTool::HandleDoubleClick(); SelectorTool::HandleDragClick

********************************************************************************************/

void SelectorTool::PostProcessClick()
{
	switch (TypeOfClick)
	{
	case CLICKTYPE_SINGLE:
		HandleSingleClick();
		break;

	case CLICKTYPE_DOUBLE:
		HandleDoubleClick();
		break;

	case CLICKTYPE_DRAG:
		HandleDragClick();
		break;

	case CLICKTYPE_UP:
		HandleButtonUp();
		break;

	default:
		break;
	}
}



/********************************************************************************************

>	virtual void SelectorTool::HandleSingleClick()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selector tool must process a button-down mouse event.
	Errors:		-
	SeeAlso:	SelectorTool::PostProcessClick; SelectorTool::HandleSingleClick;
				SelectorTool::HandleButtonUp;
				class OpTranslateTrans; class OpSelectorDragBox

********************************************************************************************/

void SelectorTool::HandleSingleClick()
{
	// Make sure the mouse cursor reflects which modifiers are active.  The keyboard handler
	// receives all key events such as Ctrl and SHIFT and sets the cursor appropriately.  Here
	// we have to check if the user is using the right button, which also means Adjust, but
	// which will not have been detected by the key handler.
//	if (ClickMods.Adjust)
//	{
		// "Fake" a call of the key handler to do the work.
		SetKeyDownCursor(ClickMods);
//	}

	// Find out which object, if any, was clicked on.  We hit-detect both the simple node
	// that was clicked and any top-level compound object it may be part of.
	pClickSimpleNode = NodeRenderableInk::FindSimpleAtPoint(StartSpread, ClickStart);
	pClickCompoundNode = NodeRenderableInk::FindCompoundFromSimple(pClickSimpleNode);

	// NOTE! All click handling takes place in HandleButtonUp using the information cached
	// here. This allows drags to be distinguished from clicks in a very clear way.
}



/********************************************************************************************

>	virtual void SelectorTool::HandleDoubleClick()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selector tool must process a mouse double-click.  Currently
				does nothing.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::HandleDoubleClick()
{
	if (SelectorTool::bPageDoubleClickOpenFile && pClickSimpleNode==NULL && pClickCompoundNode==NULL)
	{
		// The user has double-clicked on the page
		// Interpret this as a request to open a file
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor( OPTOKEN_FILEOPEN ) ;
		if (pOpDesc)
			pOpDesc->Invoke();
		return;
	}

	// Treat double clicks like two single clicks, for now.
	HandleSingleClick();
}



/********************************************************************************************

>	virtual void SelectorTool::HandleDragClick()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selector tool must process a mouse drag.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::HandleDragClick()
{
	// First check to see if the user is accidentally dragging with the menu button
	// If so then exit immediately!
// Removed by Phil to allow dragging with the menu button
//	if (ClickMods.Menu)
//		return;

	// Karim 12/07/2000
	// If the Adjust modifier is set (means the Shift key is pressed), then we'll forget
	// our click info, which is the trigger for our marquee-select behaviour.
	// This lets you start a marquee select by dragging from on top of an object, which
	// previously resulted in the object being dragged instead.
	if (ClickMods.Adjust)
	{
		pClickSimpleNode = NULL;
		pClickCompoundNode = NULL;
	}

	// Next check for the translate keyboard shortcut.  If we detect it run the translate
	// operation, provided there is something selected.
	if (IsTranslateShortcut(ClickMods))
	{		
		if (SelectionSpread != NULL)
		{
			// There is a selection, so start a translate drag...
			nClickedBoundsBlob = 0;
			GetUserParams();
			DoTranslate();
		}
		return;
	}
	
	// Check if the click is on the rotation centre, so that has priority over all other
	// blob actions.
	if (IsRotateCentreClicked(ClickStart))
	{
		// We are trying to drag the rotation centre blob.
		DoDragRotateCentre();
		return;
	}

	// Next, check if the click is on one of the other tool blobs.	Set the transforms'
	// parameters according to the way the user has set the info-bar.  Code within the
	// DoXXXX functions may override fields set here.  NB. the call to GetUserParams() 
	// must come AFTER nClickedBoundsBlob is set.
	nClickedBoundsBlob = BoundsBlobHitTest(ClickStart);
	GetUserParams();
	if (nClickedBoundsBlob != 0)
	{
		SelectRange->MakePartialSelectionWhole();

		// Run the appropriate transform for the kind of blob clicked on.
		switch (nClickedBoundsBlob)
		{
			// Corner blob.
			case 1: case 3: case 6: case 8:
				if (pInfoBarOp->InRotateMode()) DoRotate(); else DoScale();
				break;
			
			// Edge blob.
			case 2: case 4: case 5: case 7:
				if (pInfoBarOp->InRotateMode()) DoShear(); else DoSquash();
				break;

			// That's enough blobs for now.
			default:
				ERROR3("Bad blob number in SelectorTool::HandleSingleClick");
				break;
		}

		// Clicks on blobs should have nothing more to do.
		return;
	}

	// Right, that's the blobs dealt with.  If we get here we clicked on either an object
	// (possibly already selected) or blank paper.  First, check if there is an object at
	// the click position.
	if (pClickCompoundNode == NULL)
	{
		// There is only blank paper at the click point, so the user must be trying to start
		// a marquee selection drag.  Run the drag-box operation.
		DoDragBox();
		return;
	}

	// Is the clicked object already selected?  Note that the user may be trying to click on
	// a simple object selected inside a group that itself is not selected, so we must be
	// careful to check the state of the right nodes here.
	//
	// The logic here is:
	// If the clicked simple node or any of it's parents are selected
	// Then DON'T alter the selection to reflect the clicked object!
	//
	BOOL SimpleInSelected = FALSE;				// So far haven't encountered any selected nodes
	Node* pNode = pClickSimpleNode;			// Make a working pointer and initialise it
	do
	{
		if (pNode->IsSelected())				// If the working node is selected
		{
			SimpleInSelected = TRUE;			// Then the simple node or one of its parents are
			break;								// selected so we don't need to change the selection!
		}
		pNode = pNode->FindParent();			// Else check the working node's parent
	}
	while (pNode != pClickCompoundNode->FindParent());// until we've reached the compound node's parent
												// (Allows the compound node itself to be checked)


	if (!SimpleInSelected)
	{
		// No.  If the click was with the left button we must deselect all other objects.
		// If the clicked object isn't selected, we can't run a transform on it.  Hence we must
		// select it, but we prevent it being redrawn as selected (with its little blobs).
		if (!ClickMods.Adjust)
		{
			// Normal click, so deslect everything before we select the clicked node...
			NodeRenderableInk::DeselectAll();
			ResetDefaults();
		}
		else
		{
			// We clicked with the right button, but if the click was in a different spread to
			// the selection we have to deselect everything anyway.
			Node* pNode = SelectRange->FindFirst();
			if (pNode != NULL && pNode->FindParentSpread() != SelectionSpread)
			{
				// Clicked node in a different spread from previous selection, so clear selection
				NodeRenderableInk::DeselectAll();
			}
		}

		// Now that the selection state of all other objects is dealt with, make sure the
		// clicked object is selected.
		pClickCompoundNode->Select(TRUE);

		// Force a broadcast etc of the changing selection, so that our SelectionHasChanged
		// function gets called.  If it doesn't then SelectionSpread will remain NULL and
		// the info-bar won't be able to dynamically update as the object is dragged.
		SelectRange->Update(TRUE);
	}

	// Finally, run a transform on the selected object(s).  Of course, the user may not be
	// trying to drag the selection - if so the button will come up before a significant
	// drag has occurred and we can take it from there.
	SelectRange->MakePartialSelectionWhole();
	// NOTE! pNode may no longer be selected after MakePartialSelectionWhole!

	// MRH 16/5/00 - This is a worthwhile check as doing the DoTranslate function with an
	// empy bounding rect results in a major error and screws up camelots selection tool
	// from here on!
	DocRect SelRect = GetSelectionBounds(pInfoBarOp->DontConsiderAttrs());
	ERROR3IF(SelRect.IsEmpty(), "We've got an Empty Bounding Rect in HandleDragClick!");

	if (!SelRect.IsEmpty())
	{
		DoTranslate();
	}
}



/********************************************************************************************

>	virtual void SelectorTool::HandleButtonUp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selector tool must process a mouse button-up event.  Note
				that such events are NOT sent if an Operation etc has run a drag in response
				to a CLICKTYPE_DRAG.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::HandleButtonUp()
{
	// If the SelRange thinks it knows what the last selected node was
	// we'll go along with that, otherwise we'll use our own record...
	Node* pLastSelNode = SelectRange->GetLastSelectedNode();
	if ( pLastSelNode==NULL || !pLastSelNode->IsAnObject() )
	{
		pLastSelNode = pLastClickNode;
	}

	// Find out what we should do with the click...
	ClickActionCode action = CLICKACTION_NONE;
	NodeRenderableInk* pActionNode = NULL;
	action = DetermineClickAction(&pActionNode,(NodeRenderableInk*)pLastSelNode,
									pClickSimpleNode,pClickCompoundNode,StartSpread,ClickStart,ClickMods);

	// Act upon the information...
	switch (action)
	{
		//-------------------------------------------------//
		// No action required...
		case CLICKACTION_NONE:
			break;

		//-------------------------------------------------//
		// A Bounds handle was clicked on...
		case CLICKACTION_BOUNDTOPLEFT:
		case CLICKACTION_BOUNDTOP:
		case CLICKACTION_BOUNDTOPRIGHT:
		case CLICKACTION_BOUNDLEFT:
		case CLICKACTION_BOUNDRIGHT:
		case CLICKACTION_BOUNDBOTTOMLEFT:
		case CLICKACTION_BOUNDBOTTOM:
		case CLICKACTION_BOUNDBOTTOMRIGHT:
		case CLICKACTION_BOUNDTRANSFORMORIGIN:
			break;

		//-------------------------------------------------//
		// Anything that's selected must be deselected...
		case CLICKACTION_SELNONE:
		{
			// Don't clear selection if was an adjust click
			// (Change requested by Charles and Alan Burns)
			if (!ClickMods.Adjust)
			{
				// Markn 29/9/95: We ignore this if the click happend on a guideline.
				// If another type of node needs to behave like this, then a virt func in node
				// will be required instead of the hideous IS_A() clause in the 'if' statement
				if (pPreProcClickNode == NULL || !IS_A(pPreProcClickNode,NodeGuideline))
				{
					// If the selector is not the current tool, get the tool to render its blobs BEFORE the selection changes
					RenderOtherToolBlobs();


						NodeRenderableInk::DeselectAll();
						ResetDefaults();
						pLastClickNode = NULL;
	InvalidateRotationCentre();

					// If the selector is not the current tool, get the tool to render its blobs AFTER the selection changes
					RenderOtherToolBlobs();

					// Make sure the selection range stuff is updated and sends notifications.
					SelectRange->Update(TRUE);
				}
			}
			break;
		}

		//-------------------------------------------------//
		// The action node must be selected or toggled...
		case CLICKACTION_SELNODE:
		case CLICKACTION_SELUNDER:
		case CLICKACTION_SELUNDERCYCLE:
		case CLICKACTION_SELUNDERFAIL:
		case CLICKACTION_SELUNDERFAIL2:
		case CLICKACTION_SELINSIDE:
		case CLICKACTION_SELINSIDECYCLE:
		case CLICKACTION_SELINSIDEFAIL:
		case CLICKACTION_SELINSIDEFAIL2:
		case CLICKACTION_SELLEAF:
		{
			ERROR3IF(pActionNode == NULL,"Action and ActionNode don't agree!");

			// If the selector is not the current tool, get the tool to render its blobs BEFORE the selection changes
			RenderOtherToolBlobs();
			BOOL ChangedSelection = FALSE;

			// Alter final selection behaviour based on state of Adjust modifier...
			if (ClickMods.Adjust)
			{
				// If Adjust is applied, toggle the state of the action node.
				if (pActionNode->IsSelected())
					pActionNode->DeSelect(TRUE);
				else
					pActionNode->Select(TRUE);
				ChangedSelection = TRUE;
			}
			else
			{
				// Special case:
				// If this is a menu click on a selected node then don't upset the selection...
				if (pActionNode->IsSelected()
					&& ClickMods.Menu)
				{
					// Menu-click on a selected node
					// So don't change anything!
				}

				// Special case:
				// Check for clicks on a selected node in the Selector tool with bounds blobs showing...
				else if (pActionNode->IsSelected()
					&& fShowToolBlobs
					&& Tool::GetCurrentID()==TOOLID_SELECTOR
					&& !ClickMods.Menu)
				{
					// Click on a selected node
					// So toggle the scale/rotation mode...
					BOOL fNewState = !SelectorInfoBarOp::fRotateMode;
					if (pInfoBarOp != NULL)
						pInfoBarOp->SetRotateMode(fNewState);
					else
						SelectorInfoBarOp::fRotateMode = fNewState;
					// Redraw blobs and update flags etc.
					RotateButtonChange(fNewState);
					// Didn't change the selection!
				}

				// Fell through all the special cases so it's a normal click on a normal object
				// Deselect any other selected objects and select the clicked object alone
				else
				{
					bNormalClickCheckProfileDialog = TRUE;		// need to check the profile dialog
					
					NodeRenderableInk::DeselectAll();
//InvalidateRotationCentre();
					ResetDefaults();
					pActionNode->Select(TRUE);
					ChangedSelection = TRUE;
				}
			}

			// If the selector is not the current tool, get the tool to render its blobs AFTER the selection changes
			RenderOtherToolBlobs();

			// If the logic resulted in the selection actually being changed then update things...
			if (ChangedSelection)
			{
				// Record this node as being the last one clicked on.
				pLastClickNode = pActionNode;
				// Make sure the selection range stuff is updated and sends notifications.
				SelectRange->Update(TRUE);

				bNormalClickCheckProfileDialog = FALSE;		// ensure this is updated
			}

			break;
		}
		//-------------------------------------------------//
		default:
			ERROR3("Unknown Click action code!");
			break;
	}; // switch (action)

	// Make sure the cursor reflects which keys are down, now that the mouse button has
	// been released.
	ResetCursorNow();
}




/********************************************************************************************

>	ClickActionCode SelectorTool::DetermineClickAction(NodeRenderableInk** ppActionNode,
														NodeRenderableInk* pLastClickNode,
														NodeRenderableInk* pClickSimpleNode,
														NodeRenderableInk* pClickCompoundNode,
														Spread* pStartSpread,
														DocCoord ClickStart,
														ClickModifiers ClickMods)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		-
	Outputs:	pNodeToSelect	Pointer to pointer to node to select or NULL
	Returns:	ActionCode describing what the click should do
	Purpose:	Determine what action needs to be taken inresponse to a click.
	Errors:		-
	SeeAlso:	SelectorTool::HandleButtonUp

********************************************************************************************/

SelectorTool::ClickActionCode SelectorTool::DetermineClickAction(NodeRenderableInk** ppActionNode,
																 NodeRenderableInk* pLastClickNode,
																 NodeRenderableInk* pClickSimpleNode,
																 NodeRenderableInk* pClickCompoundNode,
																 Spread* pStartSpread,
																 DocCoord ClickStart,
																 ClickModifiers ClickMods)
{
	*ppActionNode = NULL;

	//--------------------------------------
	// Did we click on the transform origin?
	if (IsRotateCentreClicked(ClickStart))
	{
		return CLICKACTION_BOUNDTRANSFORMORIGIN;
	}

	// Did we click on a bounds handle?
	INT32 BlobID = BoundsBlobHitTest(ClickStart);
	if (BlobID!=0)
	{
		return (SelectorTool::ClickActionCode)((INT32)CLICKACTION_BOUNDTOPLEFT+BlobID);
	}

	//--------------------------------------
	// Test "leaf" modifier...
	if (IsSelectLeafClick(ClickMods))
	{
		// Go directly to leaf nodes!
		if (pClickSimpleNode!=pClickCompoundNode)
		{
			*ppActionNode = pClickSimpleNode;
			
			// <<<<< Inclusion by Mike 11/01/96
			// this stuff is to check whether any parent is responding to
			// AllowSelectInside() and returning FALSE. Selections will not go down
			// into these objects if so.

			Node* pParentNode = pClickSimpleNode->FindParent();
			while (pParentNode)
			{
				if (pParentNode->IsLayer())
					break;
				if (pParentNode->IsAnObject())
				{
					if ( (!pParentNode->AllowSelectInside()) && 
						 ((NodeRenderableInk*)pParentNode)->CanSelectAsCompoundParent()
					   )
					{
						*ppActionNode = (NodeRenderableInk*)(pParentNode);
					}
				}
				if (pParentNode==pClickCompoundNode)
					break;
				pParentNode = pParentNode->FindParent();
			}

			// <<<<< End of inclusion
		}

		// If we still haven't found what we're looking for
		// Cycle round to the top again...
		return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELLEAF,CLICKACTION_SELNODE);
	}

	//--------------------------------------
	// Test "under" modifier...
	if (IsSelectUnderClick(ClickMods))
	{
		// Try to perform a select under
		// First check that the context node is still under the pointer
		// If not then all we can do is a normal click operation...
		if (!ValidateLastClickUnder(pLastClickNode,pStartSpread,ClickStart))
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELUNDERFAIL2,CLICKACTION_SELUNDERFAIL2);

		// Find the leaf node at the click position, but only search nodes
		// before the last clicked node.
		*ppActionNode = NodeRenderableInk::FindSimpleAtPoint(pStartSpread,ClickStart,pLastClickNode);
		// Then find a compound node containing the leaf, preferably a sibling
		// of the last clicked node.
		*ppActionNode = NodeRenderableInk::FindCompoundFromSimple(*ppActionNode,pLastClickNode);

		// If the "under" node turns out to be the node we started from
		// return a failure code but go ahead and re-select it...
		// (If we failed to find anything under the last node, and the last node is the top node)
		if (*ppActionNode==NULL && pLastClickNode == pClickCompoundNode)
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELUNDERFAIL,CLICKACTION_SELUNDERFAIL);

		// If we still haven't found what we're looking for
		// Cycle round to the top again...
		return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELUNDER,CLICKACTION_SELUNDERCYCLE);
	}

	//--------------------------------------
	// Test "member" modifier...
	if (IsSelectMemberClick(ClickMods))
	{
		// See if the clicked simple node is a descendent of the last clicked node
		if (!ValidateLastClickInside(pLastClickNode,pClickSimpleNode))
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELINSIDEFAIL2,CLICKACTION_SELINSIDEFAIL2);

		// If the node we're going to look inside is not compound and it's the top node
		// return a failure code but go ahead and re-select it...
		if (pLastClickNode && !pLastClickNode->IsCompound() && pLastClickNode == pClickCompoundNode)
			return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELINSIDEFAIL,CLICKACTION_SELINSIDEFAIL);

		// Try to perform a select inside
		*ppActionNode = NodeRenderableInk::FindInnerCompound(pClickSimpleNode,pLastClickNode);

		// If we still haven't found what we're looking for
		// Cycle round to the top again...
		return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_SELINSIDE,CLICKACTION_SELINSIDECYCLE);
	}

	//--------------------------------------
	// OK, so no modifiers are currently down
	// Just try to do a normal click action...
	return CycleClickAction(ppActionNode,pClickCompoundNode,CLICKACTION_NONE,CLICKACTION_SELNODE);
}




/********************************************************************************************

>	BOOL SelectorTool::ValidateLastClickUnder(NodeRenderableInk* pLastClickNode, DocCoord ClickStart)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/95
	Inputs:		-
	Outputs:	pLastClickNode	Pointer to last node selected or NULL
	Returns:	TRUE if last click node is still under the pointer (somewhere)
				FALSE otherwise
	Purpose:	Validate that the last click node is still under the pointer
				Note! This routine can be slow depending on how deep it has to look in the
				tree for the last selected object.
	Errors:		-
	SeeAlso:	SelectorTool::HandleButtonUp

********************************************************************************************/

BOOL SelectorTool::ValidateLastClickUnder(NodeRenderableInk* pLastClickNode, Spread* pStartSpread, DocCoord ClickStart)
{
	NodeRenderableInk* pSearchNode = NULL;
	do
	{
		pSearchNode = NodeRenderableInk::FindSimpleAtPoint(pStartSpread,ClickStart,pSearchNode);
	}
	while (pSearchNode && pLastClickNode!=NodeRenderableInk::FindCompoundFromSimple(pSearchNode,pLastClickNode));

	return (pSearchNode!=NULL);
}




/********************************************************************************************

>	BOOL SelectorTool::ValidateLastClickInside(NodeRenderableInk* pLastClickNode,NodeRenderableInk* pClickSimpleNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/95
	Inputs:		-
	Outputs:	pLastClickNode	Pointer to last node selected or NULL
	Returns:	TRUE if the simple clicked node is inside the last clicked node somewhere
				FALSE otherwise
	Purpose:	Validate that the simple node is inside the last clicked node
	Errors:		-
	SeeAlso:	SelectorTool::HandleButtonUp

********************************************************************************************/

BOOL SelectorTool::ValidateLastClickInside(NodeRenderableInk* pLastClickNode,NodeRenderableInk* pClickSimpleNode)
{
	Node* pSearchNode = pClickSimpleNode;
	while (pSearchNode && pSearchNode!=pLastClickNode)
		pSearchNode = pSearchNode->FindParent();
	return (pSearchNode!=NULL);
}




/********************************************************************************************

>	ClickActionCode SelectorTool::CycleClickAction(	NodeRenderableInk** ppActionNode,
													NodeRenderableInk* pClickCompoundNode,
													ClickActionCode foundAction,
													ClickActionCode cycleAction
													)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/95
	Inputs:		-
	Outputs:	pNodeToSelect	Pointer to pointer to node to select or NULL
	Returns:	ActionCode describing what the click should do
	Purpose:	Determine what action needs to be taken inresponse to a click.
	Errors:		-
	SeeAlso:	SelectorTool::HandleButtonUp

********************************************************************************************/

SelectorTool::ClickActionCode SelectorTool::CycleClickAction(NodeRenderableInk** ppActionNode,
															 NodeRenderableInk* pClickCompoundNode,
															 ClickActionCode foundAction,
															 ClickActionCode cycleAction
															 )
{
	// If we have found a node then return the specified action code...
	if (*ppActionNode)
	{
		return foundAction;
	}
	// Else no suitable node so see whether the click occurred over a compound node
	else
	{
		// If click occurred over a compound node then we can return that
		// along with the alternative action code...
		if (pClickCompoundNode)
		{
			*ppActionNode = pClickCompoundNode;
			return cycleAction;
		}
		else
		// Else if there wasn't even a compound node we must return the information that
		// the click occurred over white space...
		{
			*ppActionNode = NULL;
			return CLICKACTION_SELNONE;
		}
	}
}




/********************************************************************************************

>	void SelectorTool::RenderOtherToolBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selector tool changes the selection, and yet may not be the *current* tool.
				This can happen nowadays with the right mouse button popping up the menu and changing the selection
				whilst in any tool.
	Errors:		-
	SeeAlso:	SelectorTool::OnKeyPress

********************************************************************************************/

void SelectorTool::RenderOtherToolBlobs()
{
	// Get the current tool
	Tool* pTool = Tool::GetCurrent();

	// Get the tool to remove all its blobs before we deselect the nodes.
	// Only do this if the current tool is NOT the selector tool,
	// because the selector handles the selection itself through sel changed messages.
	if (pTool!=NULL && StartSpread!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
		pBlobManager->RenderToolBlobsOff(pTool, StartSpread, NULL);
}



/********************************************************************************************

>	virtual void SelectorTool::HandleTabKey(BOOL fIsShifted)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/95
	Inputs:		fIsShifted			if TRUE then the shift key is down, which generally
									means cycle in the opposite direction.
	Outputs:	-
	Returns:	-
	Purpose:	Called when the selector tool receives a TAB keystroke.  Moves the
				selection to the next object in render-order.
	Errors:		-
	SeeAlso:	SelectorTool::OnKeyPress

********************************************************************************************/
void SelectorTool::HandleTabKey(BOOL fIsShifted)
{
#ifdef NEW_SELECTION_TAB_ITERATION
//[
	// Rules:
	//	1.	if Shift is pressed, then we move forward through the tree,
	//		otherwise we move backward.
	//	2.	if nothing is selected, then we select the backmost or foremost
	//		object on the spread, depending on whether Shift is pressed or not,
	//		and taking into account locked/invisible layers.
	//	3.	we iterate according to the rules defined by our Iterate...() methods.

	// ignore invisible and locked layers please, and reset our starting layer to NULL.
	BOOL fIgnoreInvisibleLayers	= TRUE;
	BOOL fIgnoreLockedLayers	= TRUE;
	m_pIterStartLayer			= NULL;

	BOOL fMoveForward = fIsShifted;
	IterateFlags iterFlags(fIgnoreInvisibleLayers, fIgnoreLockedLayers);

	// tell the selrange to match our settings.
	RangeControl rc = SelectRange->GetRangeControlFlags();
	RangeControl oldrc = rc;
	rc.IgnoreInvisibleLayers	= !iterFlags.IgnoreInvisibleLayers;
	rc.IgnoreLockedLayers		= iterFlags.IgnoreLockedLayers;
	SelectRange->Range::SetRangeControl(rc);

	Node* pThisNode = NULL;
	NodeRenderableInk* pInkNode = NULL;

	// if there is a selection, then we start on its first or last member.
	if (SelectionSpread != NULL)
	{
		// assumes that SelectRange has PromoteToParent FALSE, and will return a valid ink-node.
		if (fMoveForward)
		{
			pThisNode = SelectRange->FindLast();
			pThisNode = IteratePreProcess(pThisNode);
			pInkNode  = IterateNextInk(pThisNode, iterFlags);
		}
		else
		{
			pThisNode = SelectRange->FindFirst();
			pThisNode = IteratePreProcess(pThisNode);
			pInkNode  = IteratePrevInk(pThisNode, iterFlags);

		}

		// we must remove the current selection before we select our chosen node.
		if (pInkNode != NULL)
			NodeRenderableInk::DeselectAll();
	}

	// no selection, so we'll give the iteration code a bad guess (NULL) in the right spot
	// (the first or last layer in the spread), and let it use that guess to choose the node.
	else
	{
		Document* pDoc = Document::GetSelected();
PORTNOTE("spread", "Multi-spread warning!")
		Spread* pSpread = pDoc->FindFirstSpread();
		if (pSpread != NULL)
		{
			if (fMoveForward)
			{
				pThisNode = pSpread->FindLastLayer();
				pInkNode  = IteratePostProcessNext(NULL, pThisNode, iterFlags);
			}
			else
			{
				pThisNode = pSpread->FindFirstLayer();
				pInkNode  = IteratePostProcessPrev(NULL, pThisNode, iterFlags);
			}
		}
	}

	// ensure the RangeControl of the selrange is reset correctly.
	SelectRange->Range::SetRangeControl(oldrc);

	// Select the given node, if it exists.
	if (pInkNode != NULL)
	{
		pInkNode->Select(TRUE);
		SelectRange->Update(TRUE);
	}
//]
#else
//[
	// To begin, make a range that covers the entire document tree but ignores locked layers.
	Range rng(NULL, NULL, RangeControl(TRUE, TRUE, TRUE, TRUE));

	// We have two different courses of action, depending on whether there is anything
	// currently selected.  If neither work then pNode will remain NULL and we do nothing.
	NodeRenderableInk* pInkNode = NULL;
	if (SelectionSpread != NULL)
	{
		// Something is selected.  Are we tabbing forwards or backwards?
		if (!fIsShifted)
		{
			// Tab - move backwards.
			Node* pFirstNode = SelectRange->FindFirst();
			if (pFirstNode != NULL)
			{
				// Ensure we're in the normal selection surface (not "inside")
				pFirstNode = FindFrom((NodeRenderableInk*) pFirstNode);

				// Find the next node that could be selected.
				pFirstNode = rng.FindPrev(pFirstNode);
				pFirstNode = EnsureInkPrev(&rng, pFirstNode);

				// If we've run out of "prev" nodes then start from the end again
				if (pFirstNode == NULL)
				{
					pFirstNode = rng.FindLast();
					pFirstNode = EnsureInkPrev(&rng, pFirstNode);
				}

				if (pFirstNode != NULL) pInkNode = (NodeRenderableInk*) pFirstNode;
				else TRACEUSER( "JustinF", _T("nothing in tree ?!?\n"));
			}
		}
		else
		{
			// Shift tab - move forwards.
			Node* pLastNode = SelectRange->FindLast();
			if (pLastNode != NULL)
			{
				// Ensure we're in the normal selection surface (not "inside")
				pLastNode = FindFrom((NodeRenderableInk*) pLastNode);

				// Find the next node that could be selected.
				pLastNode = rng.FindNext(pLastNode);
				pLastNode = EnsureInkNext(&rng, pLastNode);

				// If we've run out of "next" nodes then start from the beginning again
				if (pLastNode == NULL)
				{
					pLastNode = rng.FindFirst();
					pLastNode = EnsureInkNext(&rng, pLastNode);
				}

				if (pLastNode != NULL) pInkNode = (NodeRenderableInk*) pLastNode;
				else TRACEUSER( "JustinF", _T("nothing in tree ?!?\n"));
			}
		}

		// Before we select the node we have found, deselect everything else.
		if (pInkNode != NULL) NodeRenderableInk::DeselectAll();
	}
	else
	{
		// Nothing is selected.  Forwards or backwards?
		if (!fIsShifted)
		{
			// Tab: Backwards - find the frontmost object.
			pInkNode = (NodeRenderableInk*) rng.FindLast();
			pInkNode = EnsureInkPrev(&rng, pInkNode);
		}
		else
		{
			// Shift-tab: Forwards - find the rearmost object.
			pInkNode = (NodeRenderableInk*) rng.FindFirst();
			pInkNode = EnsureInkNext(&rng, pInkNode);
		}
	}


	// Select the given node, if it exists.
	if (pInkNode != NULL)
	{
		pInkNode->Select(TRUE);
		SelectRange->Update(TRUE);
	}

//]
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Karim 07/08/2000
//	New iteration code, added for tab-selection because certain compound nodes - bevels,
//	shadows, contours - seriously interfered with the old range-based iteration process.
//
//	A new feature with this code is that the user can tab through the contents of a group,
//	which was previously impossible under CorelXara.
//
//	For clarity, iteration no longer uses Range code. Instead, an intelligent node-to-node
//	iteration is performed. This is in three(ish) parts.
//		1.	PreProcess	-	Ensures that the iteration starts off on the correct node.
//							For instance, if the iteration begins on a text character, then
//							it must actually behave as if it is beginning on that character's
//							enclosing text-story.
//
//		2.	Next/Prev	-	Moves on to the next/previous ink-node, regardless of whether
//							that node is actually the correct node to move to.
//
//		3.	PostProcess		Post-processes the choice made in step 2, and if necessary,
//							recursively returns to step 2, to ensure the correct node is
//							found.
//
//	Below is an example tree, with the expected iteration order.
//	(in reality, Shadow wouldn't have more than one sibling ink).
//
//	Group
//	  |
//	Shape1--ShadowController--Shape2
//				|
//			Shadow--BevelController--Shape3
//						|
//						Bevel--Shape4
//
//	forward iteration order:	Shape1, Shadow, Bevel, Shape4, Shape3, Shape2, Shape1, etc.
//

/********************************************************************************************

>	Node* SelectorTool::IteratePreProcess(Node* pCurrent)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000

	Inputs:		pCurrent	the node which you _think_ we should start from.
	Returns:	the node which we should _actually_ start from :o)

	Purpose:	Iteration pre-processing. If we're asked to iterate over the selection,
				and we happen to be on a node, eg a text-character, which is unsuitable
				for iterating from, then we need to move to a more suitable node.

	Notes:		Could be made into a virtual function based in Node or NodeRenderableInk.
				Could also possibly be called from IterateNextInk()/IteratePrevInk().

	Errors:		No errors, but if you give me a NULL node, I'll give itcha right back ;o)

********************************************************************************************/
Node* SelectorTool::IteratePreProcess(Node* pCurrent)
{
	if (pCurrent == NULL)
		return NULL;

	if (pCurrent->IsABaseTextClass())
		pCurrent = ((BaseTextClass*)pCurrent)->FindParentStory();

	return pCurrent;
}



/********************************************************************************************

>	NodeRenderableInk* SelectorTool::IterateNextInk(Node* pCurrent,
													IterateFlags iterFlags)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000
	Inputs:		pCurrent	the node which we're gonna start iterating from.
				iterFlags	some flags which tell us how to iterate.
	Returns:	the next node in the iteration, or
				NULL if the given node or its parent is NULL.

	Purpose:	Iterate over the document. See notes up above for more info.
	Notes:		* Could be made into a non-virtual function in Node or NodeRenderableInk.
				* Recursively calls IteratePostProcessNext().

********************************************************************************************/
NodeRenderableInk* SelectorTool::IterateNextInk(Node* pCurrent, IterateFlags iterFlags)
{
	Node* pParent	= (pCurrent == NULL) ? NULL : pCurrent->FindParent();
	Node* pNext		= (pCurrent == NULL) ? NULL : pCurrent->FindNextInk();
	return IteratePostProcessNext(pNext, pParent, iterFlags);
}



/********************************************************************************************

>	NodeRenderableInk* SelectorTool::IteratePostProcessNext(Node* pCurrent, Node* pParent,
															IterateFlags iterFlags)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/08/2000
	Inputs:		pCurrent	the node to post-process.
				pParent		the parent of the node to post-process.
				iterFlags	some flags which tell us how to iterate.
	Returns:	the correct node in the iteration, based on this guess.

	Purpose:	IterateNextInk() is brainless - it picks the next sibling of a node.
				This function makes an informed decision, based on that chosen node,
				to get the selection-tabbing order correct. See notes above for more info.

	Notes:		* Could be made into a virtual function based in Node or NodeRenderableInk.
				* Recursively calls itself and IterateNextInk().

********************************************************************************************/
NodeRenderableInk* SelectorTool::IteratePostProcessNext(Node* pCurrent, Node* pParent,
														IterateFlags iterFlags)
{
	// return NULL if we are given no parent.
	if (pParent == NULL)
		return NULL;

	// we're NULL, so post-processing is based on our parent.
	if (pCurrent == NULL)
	{
		if (pParent->ShouldITransformWithChildren())
			return IterateNextInk(pParent, iterFlags);

		else if (pParent->IsLayer())
		{
			Layer* pNextLayer = ((Layer*)pParent);

			pNextLayer = pNextLayer->FindNextLayer(	iterFlags.IgnoreInvisibleLayers,
													iterFlags.IgnoreLockedLayers );
			if (pNextLayer == NULL)
			{
				Spread* pSpread = pParent->FindParentSpread();
				if (pSpread != NULL)
				{
					pNextLayer = pSpread->FindFirstLayer();
					if ( (iterFlags.IgnoreInvisibleLayers && !pNextLayer->IsVisible()) ||
						 (iterFlags.IgnoreLockedLayers && pNextLayer->IsLocked()) )
						pNextLayer = pNextLayer->FindNextLayer(	iterFlags.IgnoreInvisibleLayers,
																iterFlags.IgnoreLockedLayers );
				}
			}

			if (pNextLayer == NULL || pNextLayer == m_pIterStartLayer)
				return NULL;

			if (m_pIterStartLayer == NULL)
				m_pIterStartLayer = pNextLayer;

			pCurrent = pNextLayer->FindFirstChildInk();
			return IteratePostProcessNext(pCurrent, pNextLayer, iterFlags);
		}

		else
		{
			pCurrent = pParent->FindFirstChildInk();
			if (pCurrent != NULL)
				pCurrent = IteratePostProcessPrev(pCurrent, pParent, iterFlags);

			return (NodeRenderableInk*)pCurrent;
		}
	}

	// we're non-NULL, so post-processing is based on us.
	else
	{
		if (pCurrent->ShouldITransformWithChildren())
		{
			pParent = pCurrent;
			pCurrent = pCurrent->FindFirstChildInk();
			return IteratePostProcessNext(pCurrent, pParent, iterFlags);
		}
		else if (pCurrent->IsANodeClipView())
		{
			return IterateNextInk(pCurrent, iterFlags);
		}
		else if (pCurrent->IS_KIND_OF(NodeBlender))
		{
			return IterateNextInk(pCurrent, iterFlags);
		}
		else
			return (NodeRenderableInk*)pCurrent;
	}
}



/********************************************************************************************

>	NodeRenderableInk* SelectorTool::IteratePrevInk(Node* pCurrent,
													IterateFlags iterFlags)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/2000
	Inputs:		pCurrent	the node which we're gonna start iterating from.
				iterFlags	some flags which tell us how to iterate.
	Returns:	the previous node in the iteration, or
				NULL if the given node or its parent is NULL.

	Purpose:	Iterate over the document. See notes up above for more info.
	Notes:		* Could be made into a non-virtual function in Node or NodeRenderableInk.
				* Recursively calls IteratePostProcessPrev().

********************************************************************************************/
NodeRenderableInk* SelectorTool::IteratePrevInk(Node* pCurrent, IterateFlags iterFlags)
{
	Node* pParent	= (pCurrent == NULL) ? NULL : pCurrent->FindParent();
	Node* pPrev		= (pCurrent == NULL) ? NULL : pCurrent->FindPreviousInk();
	return IteratePostProcessPrev(pPrev, pParent, iterFlags);
}



/********************************************************************************************

>	NodeRenderableInk* SelectorTool::IteratePostProcessPrev(Node* pCurrent, Node* pParent,
															IterateFlags iterFlags)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/08/2000
	Inputs:		pCurrent	the node to post-process.
				pParent		the parent of the node to post-process.
				iterFlags	some flags which tell us how to iterate.
	Returns:	the correct node in the iteration, based on this guess.

	Purpose:	IteratePrevInk() is brainless - it picks the previous sibling of a node.
				This function makes an informed decision, based on that chosen node,
				to get the selection-tabbing order correct. See notes above for more info.

	Notes:		* Could be made into a virtual function based in Node or NodeRenderableInk.
				* Recursively calls itself and IteratePrevInk().

********************************************************************************************/
NodeRenderableInk* SelectorTool::IteratePostProcessPrev(Node* pCurrent, Node* pParent,
														IterateFlags iterFlags)
{
	// return NULL if we are given no parent.
	if (pParent == NULL)
		return NULL;

	// we're NULL, so post-processing is based on our parent.
	if (pCurrent == NULL)
	{
		if (pParent->ShouldITransformWithChildren())
			return IteratePrevInk(pParent, iterFlags);

		else if (pParent->IsLayer())
		{
			Layer* pPrevLayer = ((Layer*)pParent);

			pPrevLayer = pPrevLayer->FindPrevLayer(	iterFlags.IgnoreInvisibleLayers,
													iterFlags.IgnoreLockedLayers );
			if (pPrevLayer == NULL)
			{
				Spread* pSpread = pParent->FindParentSpread();
				if (pSpread != NULL)
				{
					pPrevLayer = pSpread->FindLastLayer();
					if ( (iterFlags.IgnoreInvisibleLayers && !pPrevLayer->IsVisible()) ||
						 (iterFlags.IgnoreLockedLayers && pPrevLayer->IsLocked()) )
						pPrevLayer = pPrevLayer->FindPrevLayer(	iterFlags.IgnoreInvisibleLayers,
																iterFlags.IgnoreLockedLayers );
				}
			}

			if (pPrevLayer == NULL || pPrevLayer == m_pIterStartLayer)
				return NULL;

			if (m_pIterStartLayer == NULL)
				m_pIterStartLayer = pPrevLayer;

			pCurrent = pPrevLayer->FindLastChildInk();
			return IteratePostProcessPrev(pCurrent, pPrevLayer, iterFlags);
		}

		else
		{
			pCurrent = pParent->FindLastChildInk();
			if (pCurrent != NULL)
				pCurrent = IteratePostProcessPrev(pCurrent, pParent, iterFlags);

			return (NodeRenderableInk*)pCurrent;
		}
	}

	// we're non-NULL, so post-processing is based on us.
	else
	{
		if (pCurrent->ShouldITransformWithChildren())
		{
			pParent = pCurrent;
			pCurrent = pCurrent->FindLastChildInk();
			return IteratePostProcessPrev(pCurrent, pParent, iterFlags);
		}
		else if (pCurrent->IsANodeClipView())
		{
			return IteratePrevInk(pCurrent, iterFlags);
		}
		else if (pCurrent->IS_KIND_OF(NodeBlender))
		{
			return IteratePrevInk(pCurrent, iterFlags);
		}
		else
			return (NodeRenderableInk*)pCurrent;
	}
}



/********************************************************************************************

>	BOOL SelectorTool::IsRotateCentreClicked(DocCoord ClickStart) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the current mouse click is on the rotation centre blob.
	Purpose:	Decides whether the user has clicked the rotation centre blob, correctly
				handling the case where rotate blobs aren't visible.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::IsRotateCentreClicked(DocCoord ClickStart) const
{
	return fShowToolBlobs && eCurrentBlobs==ROTATE_BLOBS
			&& !(SelectionRect.IsEmpty()) && IsNearBlob(dcRotateCentre, ClickStart);
}




/********************************************************************************************

>	BOOL SelectorTool::IsTranslateShortcut(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the translate shortcut keys are detected.
	Purpose:	Detects whether the current mouse click modifiers denote the translate
				drag operation shortcut.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::IsTranslateShortcut(ClickModifiers cmods) const
{
	return cmods.Constrain && cmods.Alternative1;
}




/********************************************************************************************

>	BOOL SelectorTool::IsClickModified(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the current click is "modified".
	Purpose:	Tests whether any of the modifiers, eg. Constrain, Adjust etc, apply to
				the current mouse click (as received by the OnClick function).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::IsClickModified(ClickModifiers cmods) const
{
	return cmods.Adjust || cmods.Constrain || cmods.Alternative1 || cmods.Menu;
}




/********************************************************************************************

>	BOOL SelectorTool::IsSelectUnderClick(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the current click signifies the object below the clicked object
				should be selected, FALSE otherwise.
	Purpose:	Decides whether the current click is modified to be an "under" click or not
	Errors:		-
	SeeAlso:	SelectorTool::HandleSingleClick

********************************************************************************************/

BOOL SelectorTool::IsSelectUnderClick(ClickModifiers cmods) const
{
	return (!cmods.Constrain && cmods.Alternative1);
}




/********************************************************************************************

>	BOOL SelectorTool::IsSelectMemberClick(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the last click indicated "select-inside".
	Purpose:	Reports whether the current mouse click meant the user wanted to "select-
				inside" or not.
	Errors:		-
	SeeAlso:	SelectorTool::HandleButtonUp

********************************************************************************************/

BOOL SelectorTool::IsSelectMemberClick(ClickModifiers cmods) const
{
	return (cmods.Constrain && cmods.Alternative1);
}



/********************************************************************************************

>	BOOL SelectorTool::IsSelectLeafClick(ClickModifiers cmods) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the last click indicated "select-leaf".
	Purpose:	Reports whether the current mouse click meant the user wanted to "select-
				leaf" or not.
	Errors:		-
	SeeAlso:	SelectorTool::HandleButtonUp

********************************************************************************************/

BOOL SelectorTool::IsSelectLeafClick(ClickModifiers cmods) const
{
	return (cmods.Constrain && !cmods.Alternative1);
}



/********************************************************************************************

>	NodeRenderableInk* SelectorTool::FindFrom(NodeRenderableInk* pSimpleNode) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Inputs:		pSimpleNode			the simple node to begin searching from
	Outputs:	-
	Returns:	The compound object the simple node is part of, if any, or pSimpleNode
				if it isn't.
	Purpose:	Front-end short-hand for NodeRenderableInk::FindCompoundFromSimple
	Errors:		-
	SeeAlso:	NodeRenderableInk::FindCompoundFromSimple

********************************************************************************************/

NodeRenderableInk* SelectorTool::FindFrom(NodeRenderableInk* pSimpleNode) const
{
	return NodeRenderableInk::FindCompoundFromSimple(pSimpleNode);
}




/********************************************************************************************

>	NodeRenderableInk* SelectorTool::EnsureInkNext(Range* range, Node* pNode) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95
	Inputs:		pNode	Pointer to node which may or may not be ink (may even be NULL).
	Outputs:	-
	Returns:	A pointer to an ink node or NULL.
	Purpose:	Ensure that the node passed is an ink node and if not find the next object
				in the range that IS.
	Errors:		-

********************************************************************************************/

NodeRenderableInk* SelectorTool::EnsureInkNext(Range* range, Node* pNode) const
{
	while ( pNode && !pNode->IsAnObject() )
	{
		pNode = range->FindNext(pNode);
	}
	return (NodeRenderableInk*) pNode;
}




/********************************************************************************************

>	NodeRenderableInk* SelectorTool::EnsureInkPrev(Range* range, Node* pNode) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95
	Inputs:		pNode	Pointer to node which may or may not be ink (may even be NULL).
	Outputs:	-
	Returns:	A pointer to an ink node or NULL.
	Purpose:	Ensure that the node passed is an ink node and if not find the prev object
				in the range that IS.
	Errors:		-

********************************************************************************************/

NodeRenderableInk* SelectorTool::EnsureInkPrev(Range* range, Node* pNode) const
{
	while ( pNode && !pNode->IsAnObject() )
	{
		pNode = range->FindPrev(pNode);
	}
	return (NodeRenderableInk*) pNode;
}




/********************************************************************************************

>	void SelectorTool::DragMove(TransformBoundingData* pBoundingData)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/94
	Inputs:		pBoundingData - Pointer to the current size, position etc of the selection
	Purpose:	This function is called by the transform operations every time the mouse
				moves, so that the selector tool can keep the fields in its info bar up
				to date.

********************************************************************************************/

void SelectorTool::DragMove(TransformBoundingData* pBoundingData)
{
	// Debug check - WHY DO WE GET CALLS TO THIS FUNCTION WHEN THERE IS NOTHING SELECTED?!?
	if (SelectionSpread == NULL)
	{
//		TRACEUSER( "JustinF", _T("Null SelectionSpread in SelectorTool::DragMove ?\n"));
		return;
	}

	// Delegate handling of this to the info-bar.
	if (pInfoBarOp != NULL) pInfoBarOp->SetEdit_OnDrag(pBoundingData, SelectionSpread);
}



/********************************************************************************************

>	virtual BOOL SelectorTool::DragFinished(DragEndType det)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95
	Inputs:		det				an code indicating how the drag was ended, eg. did the
								user hit ESCAPE and cancel it?
	Outputs:	-
	Returns:	A boolean which if FALSE means "cancel that there drag, sonny".
	Purpose:	Called by the tool base-class when a drag is finished.  In the selector
				tool this resets the cursor and always returns success.  It also makes
				sure that the info-bar text reflects the selection, even if the drag was
				cancelled with the ESCAPE key.
	Errors:		-
	SeeAlso:	SelectorTool::ResetCursorNow

********************************************************************************************/

BOOL SelectorTool::DragFinished(DragEndType det)
{
	// Restore our normal non-drag cursor.
	ResetCursorNow();

	// If the drag was cancelled then we must manually force an update of the info-bar,
	// which will show incorrect bounds for the selection.
	if (det == DT_CANCELLED) UpdateSelectionInfo();

	// We always leave cancelling the drag to a capricious user.
	return TRUE;
}



/********************************************************************************************

>	void SelectorTool::GetUserParams()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets up the transformation data structure with default values read from
				the tool's info-bar, as set by the user.  Individual transformations can
				override these settings later.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::GetUserParams()
{
	// Get the button settings.  NB. ScaleLines should be FALSE for all except the scale
	// transform, or it affects lines of thickness > 0.
	tdParams.ScaleLines    = FALSE;
	tdParams.LockAspect    = pInfoBarOp->IsAspectLocked();
	tdParams.StartBlob     = nClickedBoundsBlob;

//	tdParams.LeaveCopy     = pInfoBarOp->ShouldLeaveCopy();
//	tdParams.TransFills    = pInfoBarOp->ShouldTransFills();
	tdParams.LeaveCopy	   = FALSE;
	tdParams.TransFills	   = TRUE;

	// Set the default transform centre to the centre of the select object(s), if any.
/*	if (SelectionSpread != NULL && !SelectionRect.IsEmpty())
	{
		DocRect dr = InflatedSelRect();
		tdParams.CentreOfTrans.x = (dr.lo.x + dr.hi.x) / 2;
		tdParams.CentreOfTrans.y = (dr.lo.y + dr.hi.y) / 2;
	}															*/

	tdParams.CentreOfTrans = dcRotateCentre;
	tdParams.pRange = 0;
}



/********************************************************************************************

>	void SelectorTool::DoScale()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a scale transformation drag of the selection.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoScale()
{
	// Get the setting of the "Scale Lines" button, which should only be effective in a
	// scale transform.
	tdParams.ScaleLines = pInfoBarOp->ShouldScaleLines();

	// Set the centre of the transform to the opposite blob.
	tdParams.CentreOfTrans = GetSelPosNearBlob(9 - nClickedBoundsBlob);

	// Run the drag.
	StartXformDrag(new OpScaleTrans, DRAGTYPE_AUTOSCROLL);
}



/********************************************************************************************

>	void DoSquash()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a squash transformation drag on the selection.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoSquash()
{
	tdParams.CentreOfTrans = GetSelPosNearBlob(9 - nClickedBoundsBlob);
	StartXformDrag(new OpSquashTrans, DRAGTYPE_AUTOSCROLL);
}



/********************************************************************************************

>	void SelectorTool::DoShear()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a shear transformation drag on the selection.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoShear()
{
	tdParams.CentreOfTrans = GetSelPosNearBlob(9 - nClickedBoundsBlob);
	StartXformDrag(new OpShearTrans, DRAGTYPE_AUTOSCROLL);
}



/********************************************************************************************

>	void SelectorTool::DoTranslate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a translation transformation drag on the selection.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoTranslate()
{
	// Override the default transform parameters.
	tdParams.CentreOfTrans = ClickStart;

	// Set drag pointer shape.
	if (CursorStackID!=CURSORID_UNSET)
		CursorStack::GSetTop(pALLCursor,CursorStackID);

	// Start the translation.
// WEBSTER - markn 14/2/97
// Disable OLE drags in Webster
#ifndef WEBSTER
#if (_OLE_VER >= 0x200)
	if (OLEPrefs::DoOLEDrags())
		StartXformDrag(new OpTranslateTrans, DRAGTYPE_OLESCROLL);
	else
#endif
#endif // WEBSTER
		StartXformDrag(new OpTranslateTrans, DRAGTYPE_AUTOSCROLL);
}



/********************************************************************************************

>	void SelectorTool::DoRotate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a rotation transformation drag on the selection.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoRotate()
{
	// Set the rotation centre.
//	tdParams.CentreOfTrans = dcRotateCentre;

	// Do the rotation.
	StartXformDrag(new OpRotateTrans, DRAGTYPE_NOSCROLL);
}



/********************************************************************************************

>	void SelectorTool::DoDragBox()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a selector tool drag-box operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoDragBox()
{
	OpSelectorDragBox* pOpDragBox = new OpSelectorDragBox;
	if (pOpDragBox == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
	}
	else
	{
		pOpDragBox->StartDragBox(StartSpread, ClickStart, ClickMods);
	}
}



/********************************************************************************************

>	void SelectorTool::DoDragRotateCentre()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Runs a drag operation for moving the centre of rotation blob around.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoDragRotateCentre()
{
	OpDragRotateCentre* pOp = new OpDragRotateCentre;
	if (pOp == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
	}
	else
	{
		// Note that we have to pass the rotate centre position as the point where the
		// drag was initiated, to ensure that it is removed cleanly.
		pOp->StartDragCentreBlob(StartSpread, dcRotateCentre, ClickMods);
	}
}



/********************************************************************************************

>	BOOL SelectorTool::StartXformDrag(TransOperation* pXformDragOp, DragType dragtype)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pXformDragOp		pointer to a transformation mouse-drag operation
				dragtype			what kind of drag to do, eg. DRAGTYPE_AUTOSCROLL
	Outputs:	-
	Returns:	TRUE if the drag is successful.
	Purpose:	Fills in any remaining fields of the transformation data structure(s) and
				calls the Tranform virtual function that initiates a mouse drag.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::StartXformDrag(TransOperation* pXformDragOp, DragType dragtype)
{
	// Couldn't allocate the op?
	if (pXformDragOp == NULL)
	{
		InformError(_R(IDS_OUT_OF_MEMORY), _R(IDS_OK));
		return FALSE;
	}

	// Get the current DocView (there must be one or we wouldn't be here).
	DocView* pDocView = DocView::GetCurrent();
	ERROR2IF(pDocView == NULL, FALSE, "Null current DocView in SelectorTool::StartXformDrag");

	ERROR3IF(tdParams.StartBlob < 0 || tdParams.StartBlob > 8, "Bad blob number in SelectorTool::StartXformDrag");


	// Calculate the "offset" from a bounds blob to the corresponding point on a selected object.
	// When we're using rectangles including effects of attributes, this is easy, as the offset
	// is a constant from the "inclusive" rectangle. However, when using the "exclusive" rect,
	// we have to factor in the distance from the appropriate edge of the exclusive rect to
	// the same edge of the inclusive rect. (If there is an object with large line thickness on
	// one side of the selection, and one with a small line width on the other side, the
	// gap between the two rectangles is different on each side!)

	// Get the distance of the gap between the selected object (inclusive rect) and its blobs.
	INT32 nOffset = GetBlobBorderSize();

	// Get the inside and outside rectangles. The DontConsiderAttrs flag may be FALSE, in which case,
	// the 2 rectangles will be identical. Thus the code in the switch statements below is generic,
	// and will do nothing extra if we're using the inclusive bounds.
	DocRect Small = GetSelectionBounds(pInfoBarOp->DontConsiderAttrs());
	DocRect Large = GetSelectionBounds(FALSE);

	DocCoord dcOffset(0, 0);		// Default to offsets of 0

	// Adjust the sign of x and/or y according to the blob number in the transform
	// parameters.
	switch (tdParams.StartBlob)		// Pass 1: Adjust the X offset if necessary
	{
		// Cases 0 (no blob), 2, 7 all leave x as zero

		case 1:		// Left side
		case 4:
		case 6:
			dcOffset.x = nOffset + (Small.lo.x - Large.lo.x);
			break;

		case 3:		// Right side
		case 5:
		case 8:
			dcOffset.x = -nOffset + (Small.hi.x - Large.hi.x);
			break;
	}

	switch (tdParams.StartBlob)		// Pass 2: Adjust the Y offset if necessary
	{
		// Cases 0 (no blob), 4, and 5 all leave y as zero

		case 1:		// Top edge
		case 2:
		case 3:
			dcOffset.y = -nOffset + (Small.hi.y - Large.hi.y);
			break;

		case 6:		// Bottom edge
		case 7:
		case 8:
			dcOffset.y = nOffset + (Small.lo.y - Large.lo.y);
			break;
	}

	// Fill the Transform Bounding Data structure up here
	DocRect SelRect = GetSelectionBounds(pInfoBarOp->DontConsiderAttrs());

	BoundingData.x 		  = SelRect.lo.x;
	BoundingData.y 		  = SelRect.lo.y;
	BoundingData.Width    = SelRect.Width();
	BoundingData.Height   = SelRect.Height();
	BoundingData.XScale   = (FIXED16) 1;
	BoundingData.YScale   = (FIXED16) 1;
	BoundingData.Rotation = (ANGLE) 0;
	BoundingData.Shear 	  = (ANGLE) 0;

	// Run the transformation drag operation and return success code.
	pXformDragOp->DragStarted(&tdParams, this, &BoundingData, ClickStart,
							  StartSpread, ClickMods, dcOffset, NULL, dragtype);
	return TRUE;
}



/********************************************************************************************

>	BOOL SelectorTool::StartXformImmediate(const char* chOpToken, void* pvParam2)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		chOpToken		the token of the OpDescriptor associated with the
								transformation Operation to be run.
				pvParam2		a parameter to pass on to the Operation, if needed.
	Outputs:	-
	Returns:	TRUE if the transformation is successful.
	Purpose:	Runs a tranformation Operation in response to the user changing settings
				on the tool's info-bar.  This works through the traditional 'Invoke'
				method.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SelectorTool::StartXformImmediate(const TCHAR* chOpToken, void* pvParam2)
{
	// Try to find the OpDescriptor associated with the given token.
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor( const_cast<TCHAR *>(chOpToken) );
	
	// Did we find it?
	if (pOpDesc == NULL)
	{
//		TRACEUSER( "JustinF", _T("Couldn't find token %s\n"), (LPCTSTR) chOpToken);
		ERROR3("Couldn't find OpDescriptor in Selector::StartXformImmediate");
		return FALSE;
	}

	// Run the transform.
	OpParam opparam(&tdParams, pvParam2);
	pOpDesc->Invoke( &opparam );
	
	return TRUE;
}



/********************************************************************************************

>	void SelectorTool::RotationCentreDragged(const DocCoord& dcNewPos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		dcNewPos		the new centre of rotation (in spread coords)
	Outputs:	-
	Returns:	-
	Purpose:	Updates the centre of rotation, marking it as now 'valid'.  This is
				called by the drag operation on successful completion.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::RotationCentreDragged(const DocCoord& dcNewPos)
{
	dcRotateCentre = dcNewPos;
	fRotateCentreIsValid = TRUE;
	fValidSelectedBlob = FALSE;
}



/********************************************************************************************

>	void SelectorTool::InvalidateRotationCentre()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Tells the selector tool that it should position the centre of rotation
				in the middle of the selection's bounds, the next time there is a
				selection.
	Errors:		-
	SeeAlso:	OpSelectorDragBox::DragFinished

********************************************************************************************/

void SelectorTool::InvalidateRotationCentre()
{
	fRotateCentreIsValid = FALSE;
}



/********************************************************************************************

>	void SelectorTool::BoundsButtonChange()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called by the info-bar when the user has clicked on the 'Show Bounds/Rotate
				Blobs' button.  Handles keeping a record of the setting, changing the
				blobs on-screen etc.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::BoundsButtonChange()
{
	if (fShowToolBlobs)
	{
		// Need to hide the tool's bounds/rotate blobs
		pBlobManager->RenderToolBlobsOff(this, SelectionSpread, NULL);

		// Record that we aren't showing our tool blobs.
		fBlobPref &= ~1;
		fShowToolBlobs = FALSE;
	}
	else
	{
		// Record which blobs are being displayed.
		fBlobPref |= 1;
		eCurrentBlobs = (pInfoBarOp->InRotateMode()) ? ROTATE_BLOBS : BOUNDS_BLOBS;
		fShowToolBlobs = TRUE;

		// Need to draw the tool blobs on if they are needed
		pBlobManager->RenderToolBlobsOn(this, SelectionSpread, NULL);
	}
}



/********************************************************************************************

>	void SelectorTool::RotateButtonChange(BOOL fNewState)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fNewState		TRUE if the button is down, FALSE if it is up.
	Outputs:	-
	Returns:	-
	Purpose:	Called by the info-bar when the user clicks on the "Rotate Blobs" button,
				that switches between rotate blobs and bounds blobs.  Handles updating
				on-screen blobs etc.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::RotateButtonChange(BOOL fNewState)
{
	// Hide the current blobs.
	pBlobManager->RenderToolBlobsOff(this, SelectionSpread, NULL);

	// If we aren't currently displaying tool blobs then switch them on.
	if (!fShowToolBlobs)
	{
		// We "fake" a click on the bounds button.  Note that BoundsButtonChange() will render
		// the new blobs and update eCurrentBlobs correctly.
		if (pInfoBarOp != NULL)
		{
			pInfoBarOp->SetLongGadgetValue(_R(IDC_SEL_SHOWBOUNDSBLOBS), TRUE, FALSE);
		}
		BoundsButtonChange();
		return;
	}

	// Update our records of which blobs should be visible.
	eCurrentBlobs = (fNewState) ? ROTATE_BLOBS : BOUNDS_BLOBS;

	// Draw the new blobs.
	pBlobManager->RenderToolBlobsOn(this, SelectionSpread, NULL);
}



/********************************************************************************************

>	void SelectorTool::FlipButtonChange(BOOL fIsVertical)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		fIsVertical		TRUE if it is a vertical flip, FALSE if it is horizontal
	Outputs:	-
	Returns:	-
	Purpose:	Called by the info-bar when the user clicks one of the "flip" buttons.
				Gets default transformation parameters and starts an "immediate"
				transformation operation.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::FlipButtonChange(BOOL fIsVertical)
{
	if (SelectionSpread != NULL)
	{
		Range* Selection = GetApplication()->FindSelection();
		RangeControl TransFlags = Selection->GetRangeControlFlags();
		TransFlags.IgnoreNoneRenderable=TRUE;
		TransFlags.IgnoreInvisibleLayers = TRUE;
		Selection->SetRangeControl(TransFlags);
		SliceHelper::ModifySelectionToContainWholeButtonElements();


		// Get the info-bar settings.
		GetUserParams();

		// Horizontal flips have StartBlob = 2 or 7, vertical flips have StartBlob = 4 or 5.
		tdParams.StartBlob = (fIsVertical) ? 4 : 2;
		StartXformImmediate( OPTOKEN_FLIP, NULL );


		SliceHelper::RestoreSelection();
	}
}



/********************************************************************************************

>	void SelectorTool::SetRotateCentre(INT32 nBlob)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nBlob		the bounds blob identifier which indicates the new position of
							the rotation centre around the selected object(s).  If zero then
							the rotation centre is set to the centre of the selection.
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a click on the "telephone keypad" by moving the centre of
				rotation to the given position, as described by a blob number.  The blob
				itself will be redrawn if necessary.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::SetRotateCentre(INT32 nBlob)
{
	// If there is nothing selected then ignore this call (the "telephone keypad" should
	// be disabled.
	if (SelectionSpread == NULL) return;

	// Remember which blob was chosen as the rotation centre.  This can be useful for other
	// transformations.
	nLastSelectedBlob = nBlob;
	fValidSelectedBlob = TRUE;

	// Test if the rotate centre blob is currently being displayed.  If it is then hide it.
	if (eCurrentBlobs==ROTATE_BLOBS && fShowToolBlobs)
	{
		// Get an XOR render region (may be the first of many).
		DocRect drClip = OpDragRotateCentre::CalcBlobClipRect(dcRotateCentre);
		RenderRegion* pRegion = DocView::RenderOnTop(&drClip, SelectionSpread, ClippedEOR);
		while (pRegion != NULL)
		{
			// Draw the blob in each render-region.
			RenderRotateCentre(pRegion, dcRotateCentre);
			pRegion = DocView::GetNextOnTop(&drClip);
		}
	}

	// Move to the given edge/side of the selected object(s) (or the center blob if nBlob==0)
	dcRotateCentre = GetSelPosNearBlob(nBlob);

	// Mark the rotation centre as "valid", ie. positively set by the user.
	fRotateCentreIsValid = TRUE;

	// Now redraw it if it was previously being displayed.
	if (eCurrentBlobs==ROTATE_BLOBS && fShowToolBlobs)
	{
		// Get an XOR render region (may be the first of many).
		DocRect drClip = OpDragRotateCentre::CalcBlobClipRect(dcRotateCentre);
		RenderRegion* pRegion = DocView::RenderOnTop(&drClip, SelectionSpread, ClippedEOR);
		while (pRegion != NULL)
		{
			// Draw the blob in each render-region.
			RenderRotateCentre(pRegion, dcRotateCentre);
			pRegion = DocView::GetNextOnTop(&drClip);
		}
	}
}



/********************************************************************************************

>	void SelectorTool::DoTranslateImmediate(MILLIPOINT nXpos, MILLIPOINT nYpos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nXpos			new position of selection, in millipoints
				nYpos			
	Outputs:	-
	Returns:	-
	Purpose:	Runs a translation of the selected object(s) initiated from the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoTranslateImmediate(MILLIPOINT nXpos, MILLIPOINT nYpos)
{
	TRACEUSER( "Matt", _T("Translate\n"));

//	SliceHelper::ModifySelectionToContainWholeButtonElements();

	// Check here if the selection would be moved off the pasteboard?

	// Package parameters and transform.
	nClickedBoundsBlob = 0;
	GetUserParams();

	DocRect SelRect = GetSelectionBounds(pInfoBarOp->DontConsiderAttrs());

	DocCoord coord (nXpos - SelRect.lo.x, nYpos - SelRect.lo.y);
	StartXformImmediate( OPTOKEN_TRANSLATE, &coord);


//	SliceHelper::RestoreSelection();
}



/********************************************************************************************

>	void SelectorTool::DoScaleImmediate(MILLIPOINT nWidth, MILLIPOINT nHeight)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nWidth			new dimensions of the selection, in millipoints
				nHeight
	Outputs:	-
	Returns:	-
	Purpose:	Scales the selected object(s) as initiated from the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoScaleImmediate(MILLIPOINT nWidth, MILLIPOINT nHeight )
{
//	TRACEUSER( "JustinF", _T("Scale\n"));

	// Work out the relative change in scale.
	DocRect SelRect = GetSelectionBounds(pInfoBarOp->DontConsiderAttrs());
	double fpXscale = double(nWidth) / SelRect.Width();
	double fpYscale = double(nHeight) / SelRect.Height();

	// Check if the scale will under- or over-flow the fixed-point numbers used in the
	// transform system.
	if (BeyondFixedRange(fpXscale) || BeyondFixedRange(fpYscale))
	{
		TRACEUSER( "JustinF", _T("Under/overflow in SelectorTool::DoScaleImmediate\n"));
		pInfoBarOp->ReportEditError(_R(IDS_SEL_ERROR_TOO_SMALL_OR_LARGE));
		return;
	}

	// Work out the centre of the transform.
	nClickedBoundsBlob = (fValidSelectedBlob) ? nLastSelectedBlob : 0;
	GetUserParams();

	// Read the scale lines setting from the info bar, overriding the default setting of
	// FALSE.
	tdParams.ScaleLines = pInfoBarOp->ShouldScaleLines();

	// Build up the transform's parameter block and do it.
	FIXED16 fxParams[2];
	fxParams[0] = fpXscale;
	fxParams[1] = fpYscale;


	StartXformImmediate( OPTOKEN_SCALE, &fxParams);
}



/********************************************************************************************

>	void SelectorTool::DoScaleImmediate(MILLIPOINT nWidth, MILLIPOINT nHeight,  BOOL Other)

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/11/97
	Inputs:		nWidth		new dimensions of the selection, in millipoints
				nHeight
				BOOL		extra param to distinguish this function from the above
	Outputs:	-
	Returns:	-
	Purpose:	As the previous DoScaleImmediate() ( Scales the selected object(s) as 
				initiated by the info-bar ), except omitting calculation of the 
				relative change in scale, and filling the transform's parameter block
				with the width & height instead, and sending a different optoken.
	Errors:		-
	SeeAlso:	DoScaleImmediate(MILLIPOINT nWidth, MILLIPOINT nHeight)

********************************************************************************************/

/*void SelectorTool::DoScaleImmediate(MILLIPOINT nWidth, MILLIPOINT nHeight,  BOOL)
{

	// Work out the centre of the transform.
	nClickedBoundsBlob = (fValidSelectedBlob) ? nLastSelectedBlob : 0;
	GetUserParams();


	// Read the scale lines setting from the info bar, overriding the default setting of
	// FALSE.
	tdParams.ScaleLines = pInfoBarOp->ShouldScaleLines();


	// Build up the transform's parameter block
	MILLIPOINT	fxParams[2];
	fxParams[0]  =  nWidth;
	fxParams[1]  =  nHeight;


	StartXformImmediate(OPTOKEN_SCALE2, &fxParams);

}
*/



/********************************************************************************************

>	void SelectorTool::DoScalePercentImmediate(double nWpercent, double nHpercent)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nWpercent			the new dimensions of the selection as a percentage of
				nHpercent			the old dimensions. Note that these values may be
									negative, in which case the object will be flipped
									in the relevant axis as well as scaled. Values near
									to 0 (less than 0.01) will be clipped to +/- 0.01
	Outputs:	-
	Returns:	-
	Purpose:	Scales the selection as initiated from the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoScalePercentImmediate(double nWPercent, double nHPercent)
{
//	TRACEUSER( "JustinF", _T("ScalePercent\n"));

	// Work out the relative change in scale and check if its in (reasonable) range.
	double fpXscale = nWPercent / 100.0;
	double fpYscale = nHPercent / 100.0;
	if (BeyondFixedRange(fpXscale) || BeyondFixedRange(fpYscale) ||
		fpXscale > 100 || fpYscale > 100)
	{	
		TRACEUSER( "JustinF", _T("Under/overflow in SelectorTool::DoScalePercentImmediate\n"));
		pInfoBarOp->ReportEditError(_R(IDS_SEL_ERROR_TOO_SMALL_OR_LARGE));
		return;
	}

	// Work out the centre of the transform.
	nClickedBoundsBlob = (fValidSelectedBlob) ? nLastSelectedBlob : 0;
	GetUserParams();

	// Read the scale lines setting from the info bar, overriding the default setting of
	// FALSE.
	tdParams.ScaleLines = pInfoBarOp->ShouldScaleLines();

	// Package up the transformation parameters and do it.
	FIXED16 fxParams[2];
	fxParams[0] = fpXscale;
	fxParams[1] = fpYscale;

	StartXformImmediate( OPTOKEN_SCALE, fxParams);
}



/********************************************************************************************

>	void SelectorTool::DoRotateImmediate(ANGLE nAngle)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nAngle		the angle (in degrees) to rotate the selection by
	Outputs:	-
	Returns:	-
	Purpose:	Rotates the selection as initiated from the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoRotateImmediate(ANGLE nAngle)
{
//	TRACEUSER( "JustinF", _T("Rotate\n"));

	nClickedBoundsBlob = 0;
	GetUserParams();

	StartXformImmediate( OPTOKEN_ROTATE, &nAngle);
}



/********************************************************************************************

>	void SelectorTool::DoShearImmediate(ANGLE nAngle)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		nAngle		the amount (in degrees) to shear the selection by
	Outputs:	-
	Returns:	-
	Purpose:	Shears the selection, as initiated from the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::DoShearImmediate(ANGLE nAngle)
{
//	TRACEUSER( "JustinF", _T("Shear\n"));

	if (fValidSelectedBlob &&
		(nLastSelectedBlob == 2 ||
		 nLastSelectedBlob == 4 ||
		 nLastSelectedBlob == 5 ||
		 nLastSelectedBlob == 7))
	{
		nClickedBoundsBlob = nLastSelectedBlob;
	}
	else
	{
		nClickedBoundsBlob = 7;					// TEMP: until we work out an axis for shearing?
	}

	GetUserParams();
	StartXformImmediate( OPTOKEN_SHEAR, &nAngle);
}



/********************************************************************************************

>	void SelectorTool::PublicDoTranslate()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 May 2000
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	Errors:		
	See also:	

********************************************************************************************/
void SelectorTool::PublicDoTranslate()
{
	// only do a drag if there is a selection.
	if (SelectionSpread != NULL)
	{
		nClickedBoundsBlob = 0;
		GetUserParams();
		DoTranslate();
	}
}



/********************************************************************************************

>	void SelectorTool::ResetDefaults()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called after a click or drag event if the tool's options should be set back
				to certain defaults.  For example, if the selection is changed to a new
				object then rotate blobs should be switched back to bounds blobs and
				the "Leave Copy" button should be OFF.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void SelectorTool::ResetDefaults()
{
	// Karim 16/10/2000
	// We don't want to worry about resetting blobs and stuff
	// unless we're actually the current tool in use.
	if (Tool::GetCurrentID() != TOOLID_SELECTOR)
		return;

	// If necessary switch back to bounds blobs.
	if (eCurrentBlobs != BOUNDS_BLOBS)
	{
		// "Fake" a click on the rotate toggle.
		pInfoBarOp->SetRotateMode(FALSE);
		RotateButtonChange(FALSE);
	}

/*
	// If necessary switch the "Leave Copy" button to OFF.
	if (pInfoBarOp->ShouldLeaveCopy())
	{
		// "Fake" a click on the LeaveCopy button.
		pInfoBarOp->SetLeaveCopy(FALSE);
	}
*/
}



/********************************************************************************************

>	Spread* SelectorTool::GetSelectionSpread() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A spread pointer, ho ho, or NULL.
	Purpose:	Returns a pointer to the spread containing the currently selected object(s).
				Used internally by the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Spread* SelectorTool::GetSelectionSpread() const
{
	return SelectionSpread;
}



/********************************************************************************************

>	const DocRect& SelectorTool::GetSelectionBounds(BOOL WithNoAttrs = FALSE) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		WithNoAttrs - TRUE if you want the bounds of the selection, exclusive
				of attribute effects such as line width
				FALSE (the default) if you want the full bounding box including attributes.
	Outputs:	-
	Returns:	A rectangle.
	Purpose:	Returns a reference to the bounds of the selected object(s).  This will be
				empty if there is no current selection.  Used internally by the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

const DocRect& SelectorTool::GetSelectionBounds(BOOL WithNoAttrs) const
{
	if (WithNoAttrs)
		return(SelectionRectNoAttr);

	return SelectionRect;
}



/********************************************************************************************

>	const BOOL SelectorTool::GetAreaDetails(BOOL WithNoAttrs, XLONG* pxlArea, XLONG* pxlPerim) const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/03/2005
	Inputs:		WithNoAttrs - TRUE if you want the area exclusive
				of attribute effects such as line width
				FALSE if you want the area including attributes
	Outputs:	xlArea - area in millipoints
				xlPerim - length of perimeter in millipoints
	Returns:	TRUE if eveyrthing went OK
	Purpose:	Returns a reference to the bounds of the selected object(s).  This will be
				empty if there is no current selection.  Used internally by the info-bar.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

const BOOL SelectorTool::GetAreaDetails(BOOL WithNoAttrs, XLONG* pxlArea, XLONG* pxlPerim) const
{
	*pxlArea = xlSelectionArea;
	*pxlPerim = xlSelectionPerim;
	return TRUE;
}



/********************************************************************************************

>	void SelectorTool::SelectionHasChanged()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94

	Purpose:	Called when the selection changes, allowing the selector tool to do things
				like update its tools blobs etc

********************************************************************************************/

void SelectorTool::SelectionHasChanged(BOOL DontUpdate)
{
	// If necessary remove the blobs.
	pBlobManager->RenderToolBlobsOff(this, SelectionSpread, NULL);

	// After interrogating the selection manager, is there now a selection?
	if (UpdateSelectionInfo())
	{
		// Yes.  Draw the tool blobs back in again, if there's a selection.
		pBlobManager->RenderToolBlobsOn(this, SelectionSpread, NULL);
	}

	// Was the cursor over a blob?  If so we must reset it.
	// Check if the mouse is within the "selected" DocView.
	Spread*  pSpread;
	DocCoord dcMousePos;
	if (DocView::GetCurrentMousePos(&pSpread, &dcMousePos))
	{
		// Call nice central routine to figure out what status text to show...
		// Set the status bar text.
//		StartSpread = pSpread;
//		ClickStart = dcMousePos;
		String_256 Str;
		Cursor* pPtr;
		FigureUserFeedback(pSpread, dcMousePos, ClickModifiers::GetClickModifiers(), TRUE, &Str, &pPtr);
		if (CursorStackID!=CURSORID_UNSET)
			CursorStack::GSetTop(pPtr,CursorStackID);
	}

	// Give the info-bar a chance to update its records of the contents of the
	// edit-fields.
	pInfoBarOp->UpdateAllRecords();

}



/********************************************************************************************
>	void SelectorTool::AttributeHasChanged()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/6/95
	Purpose:	Responds to an attribute-changing message by forcing the selection range to
				update its bounds, then forces a recalc and redraw of the tool's bounds
				blobs.
	SeeAlso:	SelectorTool::SelectionHasChanged; SelectorInfoBarOp::Message
********************************************************************************************/

void SelectorTool::AttributeHasChanged()
{
	// If there's no SelRange then we're a bit stuck.
	if (SelectRange == NULL) return;
	
	// Make sure the SelRange has updated bounds.
	SelectRange->UpdateBounds();

	// Force a recalc and redraw of the bounds blobs.
	SelectionHasChanged();
}



/********************************************************************************************
>	void SelectorTool::SelectionBlobChange(BlobStyle ChangingBlobs)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		ChangingBlobs - The blob types that are changing
	Purpose:	Called when the types of blob that are being displayed is to be changed
				(ie, someone pressed one of the buttons on the info bar). This function
				tells the blob manager what it wants to know and will display or
				remove the Tool blobs as appropriate. You should only have one of the
				blobs types set to TRUE at a time. ie, only change one type of blob at
				once.
********************************************************************************************/

void SelectorTool::SelectionBlobChange(BlobStyle ChangingBlobs)
{
	// Get a pointer to the BlobManager
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	if (BlobMgr == NULL) return;

	// The Object Blobs
	if (ChangingBlobs.Object==TRUE)
	{
		if (bsBlobStyle.Object==TRUE)
		{
			// Remove the Object blobs from the world
			BlobMgr->RemoveInterest(ChangingBlobs);
			ChangingBlobs.Object = bsBlobStyle.Object = FALSE;
			fBlobPref &= ~2;

			// Add Tiny blobs.
			ChangingBlobs.Tiny = bsBlobStyle.Tiny = TRUE;
			BlobMgr->AddInterest(ChangingBlobs);
		}
		else
		{
			// draw the Object blobs in
			BlobMgr->AddInterest(ChangingBlobs);
			bsBlobStyle.Object = TRUE;
			fBlobPref |= 2;

			// Remove the tiny blobs
			ChangingBlobs.Object = FALSE;
			ChangingBlobs.Tiny = TRUE;
			BlobMgr->RemoveInterest(ChangingBlobs);
			bsBlobStyle.Tiny = FALSE;
		}
	}

	// The fill blobs
	if (ChangingBlobs.Fill==TRUE)
	{
		if (bsBlobStyle.Fill==TRUE)
		{
			// Remove the Fill blobs from the world
			BlobMgr->RemoveInterest(ChangingBlobs);
			bsBlobStyle.Fill = FALSE;
			fBlobPref &= ~4;
		}
		else
		{
			// draw the Fill blobs in
			BlobMgr->AddInterest(ChangingBlobs);
			bsBlobStyle.Fill = TRUE;
			fBlobPref |= 4;
		}
	}

	// The Artistic Blobs
	if (ChangingBlobs.Artistic==TRUE)
	{
		if (bsBlobStyle.Artistic==TRUE)
		{
			// Remove the Artistic blobs from the world
			BlobMgr->RemoveInterest(ChangingBlobs);
			bsBlobStyle.Artistic = FALSE;
			fBlobPref &= ~8;
		}
		else
		{
			// draw the Artistic blobs in
			BlobMgr->AddInterest(ChangingBlobs);
			bsBlobStyle.Artistic = TRUE;
			fBlobPref |= 8;
		}
	}
}



/********************************************************************************************
>	void SelectorTool::ViewChanged(const DocViewMsg& msg)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/94
	Inputs:		A constant reference to a DocView message that notified us of this event.
	Purpose:	When views and documents are changing the selector tool needs to get its
				selection data correct for the old and new views so that its tool blobs
				can be removed. This function allows the info bar to get the tool to
				re-calculate this information and draw the blobs accordingly. The blobs
				are only drawn once here as this functions real purpose it to get rid of the
				blobs from the old view and the draw them in on the new view. As a result
				the info bar calls this function twice after setting up the appropriate
				docview info.
********************************************************************************************/

void SelectorTool::ViewChanged(const DocViewMsg& msg)
{
	switch (msg.State)
	{
		case DocViewMsg::SELABOUTTOCHANGE:
		{
			// Draw the blobs onto the new view
			if (msg.pNewDocView != NULL)
			{
				// The view is about to change, so get rid of the blobs in this view now
				pBlobManager->RenderToolBlobsOff(this, SelectionSpread, NULL);
///////////////////////////////////////////////////////////////////////////////////////////
				// Ignore Selection Changed messages until the view has actually changed
				// JCF: this has been moved to here so that this flag does *not* get set
				// if there is no new view to switch to, otherwise it remains set and the
				// info-bar's gadgets don't get disabled.  This appears to be due to a
				// bug in the messages sent when the Document is closed - the
				// SELABOUTTOCHANGE message gets sent *twice*, once before the SELCHANGED
				// message and once after!?!?
				fIgnoreSelChange = TRUE;
///////////////////////////////////////////////////////////////////////////////////////////
			}

			// Ignore Selection Changed messages until the view has actually changed
//			fIgnoreSelChange = TRUE;
			break;
		}

		case DocViewMsg::SELCHANGED:
		{
			// This record the document's associated with the old and new views.
			Document* pOldViewsDoc = NULL;
			Document* pNewViewsDoc = NULL;

			// Find out about the document associated with the old view
			if (msg.pOldDocView != NULL)
				pOldViewsDoc = msg.pOldDocView->GetDoc();

			// Find out about the document associated with the new view
			if (msg.pNewDocView != NULL)
				pNewViewsDoc = msg.pNewDocView->GetDoc();

			// If the document associated with the new view is different to the document associated
			// with the old view, then mark the rotation centre's position as needing updating.
			if (pOldViewsDoc != pNewViewsDoc)
				fRotateCentreIsValid = FALSE;

			// Draw the blobs onto the new view
			if (msg.pNewDocView != NULL)
			{
// [Removed by Jason]
//				SelectRange->Update();
// There is no need to update the SelRange in this manner - we should now only call this
// method WHEN we HAVE CHANGED the selection. I believe this line was added when the
// SelRange didn't correctly cope with Doc{View} changes; it's unnecessary (bad) these days.

				// There is a new view so draw the bounds blobs on it.
				if (UpdateSelectionInfo())
					pBlobManager->RenderToolBlobsOn(this, SelectionSpread, NULL);
			}

			// Stop ignoring the selection changed messages again
			fIgnoreSelChange = FALSE;
			break;
		}

		default:
			break;
	}
}



/********************************************************************************************
>	BlobStyle SelectorTool::GetBlobStyle() const

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Returns:	BlobStyle - The selector tools ideas about which blobs are being displayed
				and which are not
	Purpose:	To find out what blobs the selector tool is displaying
********************************************************************************************/

BlobStyle SelectorTool::GetBlobStyle() const
{
	return bsBlobStyle;
}



/********************************************************************************************
>	SelectorTool::BlobType SelectorTool::GetCurrentToolBlobType() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	A blob-type enumeration.
	Purpose:	Returns the type of tool blobs currently being displayed, either None,
				Bounds, or Rotate blobs.  Used internally by the info-bar.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

SelectorTool::BlobType SelectorTool::GetCurrentToolBlobType() const
{
	return eCurrentBlobs;
}



/********************************************************************************************
>	void SelectorTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		pSpread - The spread that needs redrawing
				pClipRect - The clipping rectangle that needs redrawing
	Purpose:	Draws the blobs specific to the selector tool. This can include the
				Bounds blobs that surround the current selection, or the Rotate/Shear
				blobs that also surround the selection. 
********************************************************************************************/

void SelectorTool::RenderToolBlobs(Spread* pSpread, DocRect* pClipRect)
{
	// Draw the bounds blobs if there is anything to render in a view on the given spread.
	if (fShowToolBlobs && DocView::GetSelected() != NULL &&
		SelectionSpread != NULL && SelectionSpread == pSpread)
	{
		// Calculate the inflated selection rectangle.
		DocRect drBlobRect = InflatedSelRect();

		// Render the appropriate blobs.
		switch (eCurrentBlobs)
		{
			case BOUNDS_BLOBS:
				RenderBoundBlobs(pSpread, pClipRect, drBlobRect);
				break;

			case ROTATE_BLOBS:
				RenderRotateBlobs(pSpread, pClipRect, drBlobRect);
				break;

			default:
				ENSURE(FALSE, "Unknown blob type in SelectorTool::RenderToolBlobs");
				return;
		}
	}
}



/********************************************************************************************
>	BOOL SelectorTool::UpdateSelectionInfo()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if there is a current selection, FALSE if no object is selected.
	Purpose:	Updates the selector tool's record of the spread and bounding rectangle
				of the current selection.
	Errors:		-
	SeeAlso:	SelectorTool::SelectChange; SelectorTool::SelectionHasChanged
********************************************************************************************/

BOOL SelectorTool::UpdateSelectionInfo()
{
	// Reset all our infomation about the selection to "no selection".
	SelectionSpread = NULL;
	SelectionRect.MakeEmpty();
	SelectionRectNoAttr.MakeEmpty();
	xlSelectionArea = 0;
	xlSelectionPerim = 0;

	// Go find the first node in the selection, so that we can find out about its spread
	Node* pFirstNode = SelectRange->FindFirst();
	if (pFirstNode != NULL)
	{
		// Find the spread that the selection lives on, if any, and its bounds.
		SelectionSpread = pFirstNode->FindParentSpread();
		if (SelectionSpread != NULL)
		{
			// Update the bounding rectangle of the selection.
			SelectionRect = SelectRange->GetBoundingRect();
			SelectionRectNoAttr = SelectRange->GetBoundsWithoutAttrs();

#ifdef SELECTION_AREA_FEATURE
			// This needs to be done in the background
			m_bComputeAreaDetails = TRUE;
//			SelectRange->GetAreaDetails(&xlSelectionArea, &xlSelectionPerim);
#endif

//			ENSURE(!SelectionRect.IsEmpty(),"Empty selection bounds in SelectorTool::UpdateSelectionInfo");
			if (SelectionRect.IsEmpty())
				SelectionRect.Inflate(1);

			if (SelectionRectNoAttr.IsEmpty())
				SelectionRectNoAttr.Inflate(1);

			// If any previous routine has indicated the rotation centre should be moved back
			// to the centre of the selection then recalc its position.
			if (!fRotateCentreIsValid)
			{
				// Default rotation centre is the centre of the object. Call GetSelPosNearBlob
				// to calculate the position for us. 0 means blob 0, the center blob.
				dcRotateCentre = GetSelPosNearBlob(0);

				nLastSelectedBlob = 0;
				fValidSelectedBlob = TRUE;
//				fRotateCentreIsValid = TRUE;
//				TRACEUSER( "JustinF", _T("Reseting rotation centre to middle of selection\n"));
			}
		}
/*	#ifdef _DEBUG
		else
		{
			TRACEUSER( "JustinF", _T("Nothing selected cos first node has no parent spread\n"));
		}
	#endif
*/	}			/*
#ifdef _DEBUG
	else
	{
		TRACEUSER( "JustinF", _T("Nothing selected cos no first node in SelRange\n"));
	}
#endif
*/
	// Update the info-bar with the selection's extent.
	pInfoBarOp->SetEdit_OnSelectionChange();

	// Return TRUE if there is a selection.
	return SelectionSpread != NULL;
}



/********************************************************************************************
>	INT32 SelectorTool::BoundsBlobHitTest(const DocCoord& dcMousePos) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		dcMousePos	-	the position of the mouse (in spread coords).
	Outputs:	-
	Returns:	0 if no blob is hit, a blob number (1 - 8) if a blob is hit.  Blob numbers
				follow this pattern:-
					1-----2-----3
					|           |
					4           5
					|           |
					6-----7-----8
	Purpose:	Checks if the given coordinate is over or very near a blob.
	Errors:		-
	SeeAlso:	SelectorTool::IsNearBlob
********************************************************************************************/

INT32 SelectorTool::BoundsBlobHitTest(const DocCoord& dcMousePos) const
{
	// If there isn't a selected object or no blobs at all we can't have clicked on a blob.
	if (SelectionRect.IsEmpty() || !fShowToolBlobs) return 0;

	// Calculate the rectangle describing where the blobs are on the screen, and the
	// mid-points of its horizontal and vertical sides.
	DocRect drBlobRect = InflatedSelRect();
	INT32 nMidX = (drBlobRect.lo.x + drBlobRect.hi.x) / 2;
	INT32 nMidY = (drBlobRect.lo.y + drBlobRect.hi.y) / 2;

	// Check each blob in turn to see if it is near enough to the given point.
	     if (IsNearBlob(dcMousePos, DocCoord(drBlobRect.lo.x, drBlobRect.hi.y)))				
				return 1;		// top-left
	else if (IsNearBlob(dcMousePos, DocCoord(nMidX, drBlobRect.hi.y)))				
				return 2;		// top-mid
	else if (IsNearBlob(dcMousePos, drBlobRect.hi))
				return 3;		// top-right				
	else if (IsNearBlob(dcMousePos, DocCoord(drBlobRect.lo.x, nMidY)))				
				return 4;		// mid-left
	else if (IsNearBlob(dcMousePos, DocCoord(drBlobRect.hi.x, nMidY)))				
				return 5;		// mid-right
	else if (IsNearBlob(dcMousePos, drBlobRect.lo))		 		
		 		return 6;		// bottom-left
	else if (IsNearBlob(dcMousePos, DocCoord(nMidX, drBlobRect.lo.y)))				
				return 7;		// bottom-mid
	else if (IsNearBlob(dcMousePos, DocCoord(drBlobRect.hi.x, drBlobRect.lo.y))) 				
				return 8;		// bottom-right

	// It's not near any of them!
	return 0;
}



/********************************************************************************************
>	static BOOL SelectorTool::IsNearBlob(const DocCoord& dcBlobOrigin, 
						  	  	  		 const DocCoord& dcTestPoint)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		dcBlobOrigin ---  the centre of the blob to be checked
				dcTestPoint  ---  the point that is being tested
	Outputs:	-
	Returns:	TRUE if the point is over or "near" the given blob.
	Purpose:	Private helper function for SelectorTool::BlobHitTest
	Errors:		-
	SeeAlso:	SelectorTool::BoundsBlobHitTest
********************************************************************************************/

BOOL SelectorTool::IsNearBlob(const DocCoord& dcBlobOrigin, const DocCoord& dcTestPoint)
{
	// Get the click radius from the preferences.
	DocView* pDocView = DocView::GetCurrent();
	ENSURE(pDocView != NULL, "Null current DocView in SelectorTool::IsNearBlob");
	MILLIPOINT nNearRadius = OSRenderRegion::GetHitTestRadius(pDocView);

	// Calculate the blob's rectangle, given it's centre, and inflate it by the
	// "is near enough" radius.
	DocRect drBlobRect;
	pBlobManager->GetBlobRect(DocCoord(dcBlobOrigin), &drBlobRect);
	drBlobRect.Inflate(nNearRadius);

	// Return TRUE if the test point is within the blob's rectangle (a hit).
	return drBlobRect.ContainsCoord(dcTestPoint);
}



/********************************************************************************************
>	DocCoord SelectorTool::GetBoundsBlobPos(INT32 nBlob) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/94
	Inputs:		nBlob		the numerical ID of the blob (1-8)
	Outputs:	-
	Returns:	The position of the blob, in Spread coords.
	Purpose:	Calculates the position of a given blob within the document.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

DocCoord SelectorTool::GetBoundsBlobPos(INT32 nBlob) const
{
	// Get the rectangle describing the position of the bounds blobs, and the
	// mid-points along its sides.
	DocRect drBlobRect = InflatedSelRect();
	INT32 nMidX = (drBlobRect.lo.x + drBlobRect.hi.x) / 2;
	INT32 nMidY = (drBlobRect.lo.y + drBlobRect.hi.y) / 2;

	DocCoord dc(0,0);
	switch (nBlob)
	{
		case 0:
			dc = DocCoord(nMidX, nMidY); break;
		case 1:
			dc = DocCoord(drBlobRect.lo.x, drBlobRect.hi.y); break;
		case 2:
			dc = DocCoord(nMidX, drBlobRect.hi.y); break;
		case 3:
			dc = drBlobRect.hi; break;
		case 4:
			dc = DocCoord(drBlobRect.lo.x, nMidY); break;
		case 5:
			dc = DocCoord(drBlobRect.hi.x, nMidY); break;
		case 6:
			dc = drBlobRect.lo;	break;
		case 7:
			dc = DocCoord(nMidX, drBlobRect.lo.y); break;
		case 8:
			dc = DocCoord(drBlobRect.hi.x, drBlobRect.lo.y); break;
		default:
			ENSURE(FALSE, "Bad blob number in SelectorTool::GetBlobPos"); break;
	}

	return dc;
}



/********************************************************************************************
>	DocCoord SelectorTool::GetSelPosNearBlob(INT32 nBlob) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		nBlob		---		the blob's number
	Outputs:	-
	Returns:	A position on the bounds of the selected object(s).
	Purpose:	This returns the nearest point to the given blob on the selected object(s)
				bounding rectangle.

	Notes:		The "point on the object" is actually a point on the object's bounding
				box. The bounding box used will include or exclude effects of attributes
				such as line width depending on the pInfoBarOp->DontConsiderAttrs() flag.

********************************************************************************************/

DocCoord SelectorTool::GetSelPosNearBlob(INT32 nBlob) const
{
	// Use the selection bounds. If we should include line widths, then we use a large rectangle.
	// This means we'll scale the object either so that the edge of the line doesn't move,
	// or so that the edge of the object (ignoring line width) doesn't move, depending on
	// the mode we're in. This will also affect exactly which bit of the object will snap
	// to the grid.
	DocRect SelRect = GetSelectionBounds(pInfoBarOp->DontConsiderAttrs());

	// This function should never need to be called if the SelRect is empty.
	ERROR3IF(SelRect.IsEmpty(), "Empty SelRect in SelectorTool::InflatedSelRect");

	// Calculate the midpoints of the sides of the rectangle.
	INT32 nMidX = (SelRect.lo.x + SelRect.hi.x) / 2;
	INT32 nMidY = (SelRect.lo.y + SelRect.hi.y) / 2;

	// Here we go . . .
	DocCoord dc(0,0);
	switch (nBlob)
	{
		case 0:			
			dc = DocCoord(nMidX, nMidY); break;
		case 1:
			dc = DocCoord(SelRect.lo.x, SelRect.hi.y); break;
		case 2:
			dc = DocCoord(nMidX, SelRect.hi.y); break;
		case 3:
			dc = SelRect.hi; break;
		case 4:
			dc = DocCoord(SelRect.lo.x, nMidY); break;
		case 5:
			dc = DocCoord(SelRect.hi.x, nMidY); break;
		case 6:
			dc = SelRect.lo;	break;
		case 7:
			dc = DocCoord(nMidX, SelRect.lo.y); break;
		case 8:
			dc = DocCoord(SelRect.hi.x, SelRect.lo.y); break;
		default:
			ENSURE(FALSE, "Bad blob number in SelectorTool::GetSelPosNearBlob"); break;
	}
	return dc;
}



/********************************************************************************************
>	DocRect SelectorTool::InflatedSelRect() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		-
	Outputs:	-
	Returns:	The bounding rectangle of the selection (which must not be empty)
				inflated by 4 (visible) millimetres, or whatever the preference is set to.
	Purpose:	Calculates the position of the blobs around a selected object.

	Notes:		Note that the Inflated SelRect as used for blob plotting is always based
				on the selection bounding rect INCLUDING Attributes. This means the blobs
				always sit outside the object. This differs from all the rectangles
				which are used to read/set object values, which may/may not include
				the effects of attributes such as line width in them.

	SeeAlso:	SelectorTool::InflateByBlobBorder; SelectorTool::RenderToolBlobs
********************************************************************************************/

DocRect SelectorTool::InflatedSelRect() const
{
	DocRect dr = SelectionRect;
	InflateByBlobBorder(&dr);
	return dr;
}



/********************************************************************************************

>	static INT32 SelectorTool::GetBlobBorderSize(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Moved code of Justin's here to centralise the calculation)
	Created:	27/2/96

	Returns:	The size of the border (in millipoints)

	Purpose:	Determines the width of the "border" between a selected object and the bounds
				blobs around it.  NB. there must be a selected DocView for this function to
				work (it relies on the selected view's scale factor).

	SeeAlso:	SelectorTool::InflateByBlobBorder

********************************************************************************************/

INT32 SelectorTool::GetBlobBorderSize(void)
{
	// We can only do this if there is a current DocView
	DocView* pDocView = DocView::GetSelected();

	// Make sure we are not passed in a load of junk
	if (pDocView == NULL)
	{
		ERROR3("No Selected DocView in SelectorTool::InflateByBlobBorder");
		return(0);
	}

	// We have to inflate the rectangle by (nBlobBorder) mm, so we must calculate how much
	// that is in DocCoord units, accounting for the view's scale factor.
	INT32 nBorder = (nMPperMM * nBlobBorder) << F16SHIFT;
	nBorder /= pDocView->GetViewScale().GetRawLong();

	return(nBorder);
}



/********************************************************************************************

>	static void SelectorTool::InflateByBlobBorder(DocRect* pdrRect)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Inputs:		pdrRect		---		pointer to the rectangle (in doc or spread coords) to
									inflate. May not be NULL.
	Outputs:	The inflated rectangle.

	Purpose:	Useful public function that adjusts a bounding rectangle by the "border"
				between a selected object and the bounds blobs around it.  NB. there must
				be a selected DocView for this function to work (it relies on the selected
				view's scale factor).

	SeeAlso:	SelectorTool::GetBlobBorderSize;
				SelectorTool::InflatedSelRect;
				OpZoomFitSelectedDescriptor::AdjustRect

********************************************************************************************/

void SelectorTool::InflateByBlobBorder(DocRect* pdr)
{
	ERROR3IF(pdr->IsEmpty(), "Empty rectangle passed to SelectorTool::InflateByBlobBorder");

	if (pdr != NULL)
		pdr->Inflate(GetBlobBorderSize());
}



/********************************************************************************************
>	void SelectorTool::RenderBoundBlobs(Spread* pSpread, DocRect* pClipRect,
										const DocRect& drcBlobRect)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		pSpread - The spread that needs redrawing
				pClipRect - The clipping rectangle that needs redrawing
				drcBlobRect - where to draw the blobs
	Purpose:	This function is called from the RenderToolBlobs function. If the Bounds
				Blobs should be displayed (ie the user has switch them on) then this function
				is used to draw them. The blobs appear as a small rect at each corner of the
				selection rectangle as well as one at the midpoint of each of the sides of
				the rectangle. This function should not be called directly, but only through
				RenderToolBlobs()
********************************************************************************************/

void SelectorTool::RenderBoundBlobs(Spread* pSpread, DocRect* pClipRect,
									const DocRect& drcBlobRect)
{
	// Get an XOR render region (may be the first of many).
	RenderRegion* pRegion = DocView::RenderOnTop(pClipRect, pSpread, ClippedEOR);
	while (pRegion != NULL)
	{
		// Set the line and fill styles
		pRegion->SetLineColour(COLOUR_NONE);
		pRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);

		// Render a blob at each corner of the Selections bounds
		pRegion->DrawBlob(DocCoord(drcBlobRect.lo.x, drcBlobRect.lo.y), BT_SELECTED);
		pRegion->DrawBlob(DocCoord(drcBlobRect.hi.x, drcBlobRect.hi.y), BT_SELECTED);
		pRegion->DrawBlob(DocCoord(drcBlobRect.lo.x, drcBlobRect.hi.y), BT_SELECTED);
		pRegion->DrawBlob(DocCoord(drcBlobRect.hi.x, drcBlobRect.lo.y), BT_SELECTED);

		// Find the x- and y-ordinates of the mid-point of each side.
		INT32 xMid = (drcBlobRect.lo.x + drcBlobRect.hi.x) / 2;
		INT32 yMid = (drcBlobRect.lo.y + drcBlobRect.hi.y) / 2;

		// Render the blobs on each of the sides
		pRegion->DrawBlob(DocCoord(xMid, drcBlobRect.lo.y), BT_SELECTED);
		pRegion->DrawBlob(DocCoord(xMid, drcBlobRect.hi.y), BT_SELECTED);
		pRegion->DrawBlob(DocCoord(drcBlobRect.lo.x, yMid), BT_SELECTED);
		pRegion->DrawBlob(DocCoord(drcBlobRect.hi.x, yMid), BT_SELECTED);

		// Render the rotation centre.
//		RenderRotateCentre(pRegion, dcRotateCentre);

		// Go on to the next render region, if any.
		pRegion = DocView::GetNextOnTop(pClipRect);
	}
}



/********************************************************************************************
>	void SelectorTool::RenderRotateBlobs(Spread* pSpread, DocRect* pdrClip, const DocRect& drBlob)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/94
	Inputs:		pSpread - The spread that needs redrawing
				pdrClip - The clipping rectangle that needs redrawing
				drBlob 	- Where to draw the blobs.
	Purpose:	Draws the blobs that appear when the Rotate/Shear mode of the selector tool
				is entered. These blobs appear in the same places as the Bounds Blobs,
				around the edge of the given rectangle. The only difference is their
				appearance.  The blobs at the corner of the selection appear as small
				curved arrows to indicate rotation and the ones on the side of the
				selection appear as straight arrows	to indicate shearing.
	SeeAlso:	SelectorTool::RenderToolBlobs
********************************************************************************************/

void SelectorTool::RenderRotateBlobs(Spread* pSpread, DocRect* pdrClip, const DocRect& drBlob)
{
	// Get the size of a blob at this scale.
//	INT32 nSize = pBlobManager->GetBlobSize();

	// Get an XOR render region (may be the first of many).
	RenderRegion* pRegion = DocView::RenderOnTop(pdrClip, pSpread, ClippedEOR);
	while (pRegion != NULL)
	{
		// Render a blob at each corner of the Selections bounds
		// This is not the correct shape etc for these blobs, but it will do for now...
		pRegion->DrawBitmapBlob(DocCoord(drBlob.lo.x, drBlob.lo.y), _R(IDBMP_HANDLE_6));
		pRegion->DrawBitmapBlob(DocCoord(drBlob.hi.x, drBlob.hi.y), _R(IDBMP_HANDLE_3));
		pRegion->DrawBitmapBlob(DocCoord(drBlob.lo.x, drBlob.hi.y), _R(IDBMP_HANDLE_1));
		pRegion->DrawBitmapBlob(DocCoord(drBlob.hi.x, drBlob.lo.y), _R(IDBMP_HANDLE_8));

		// Find the x- and y-ordinates of the mid-point of each side.
		INT32 xMid = (drBlob.lo.x + drBlob.hi.x) / 2;
		INT32 yMid = (drBlob.lo.y + drBlob.hi.y) / 2;

		// Render the blobs on each of the sides
		pRegion->DrawBitmapBlob(DocCoord(xMid, drBlob.lo.y), _R(IDBMP_HANDLE_45));
		pRegion->DrawBitmapBlob(DocCoord(xMid, drBlob.hi.y), _R(IDBMP_HANDLE_45));
		pRegion->DrawBitmapBlob(DocCoord(drBlob.lo.x, yMid), _R(IDBMP_HANDLE_27));
		pRegion->DrawBitmapBlob(DocCoord(drBlob.hi.x, yMid), _R(IDBMP_HANDLE_27));

		// Render the rotation centre.
		RenderRotateCentre(pRegion, dcRotateCentre);

		// Go on to the next render region, if any.
		pRegion = DocView::GetNextOnTop(pdrClip);
	}
}



/********************************************************************************************
>	void SelectorTool::RenderRotateCentre(RenderRegion* pRegion, const DocCoord& dcPos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pRegion			the RenderRegion to draw into
				dcPos			the position of the rotation centre (spread coords)
	Outputs:	-
	Returns:	-
	Purpose:	Draws the rotation centre blob at the given position.  This is a separate
				function so that the drag code can call it as well.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void SelectorTool::RenderRotateCentre(RenderRegion* pRegion, const DocCoord& dcPos)
{
	pRegion->DrawBitmapBlob(dcPos, _R(IDBMP_HANDLE_CENTRE));
}



/********************************************************************************************
>	static BOOL SelectorTool::BeyondFixedRange(double fpFixedVal)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/96
	Inputs:		fpFixedVal		the double that is to be converted to fixed-point
	Returns:	TRUE if the number is beyond the range that fixed-piont numbers can
				represent, FALSE if its OK to convert.
	Purpose:	Checks for under/overflow in fixed-point calculations.
********************************************************************************************/

BOOL SelectorTool::BeyondFixedRange(double fpFixedVal)
{
	const double fpMinFixPt = 1.0 / (INT32(SHRT_MAX) + 1);		// 1/32768
	const double fpMaxFixPt = SHRT_MAX;							// 32767
	double fp = fabs(fpFixedVal);
	return fp < fpMinFixPt || fp > fpMaxFixPt;
}



/********************************************************************************************
>	OpSelectorDragBox::OpSelectorDragBox()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. This simply sets a few of the operation flags.
********************************************************************************************/

OpSelectorDragBox::OpSelectorDragBox()
{
	// Empty.
}



/********************************************************************************************
>	void OpSelectorDragBox::StartDragBox(DocCoord Anchor)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		Anchor - The position of the mouse at the start of the Drag
	Purpose:	This is called when a Drag operation has been detected
********************************************************************************************/

void OpSelectorDragBox::StartDragBox(Spread* pSpread, DocCoord Anchor, ClickModifiers ClickMods)
{
	// We had better take a note of the starting point of the drag
	StartSpread = pSpread;
	StartPoint = Anchor;
	LastMousePosition = Anchor;

	// Put some helpful text in the status bar.
	SelectorTool::AllowIdleWork(FALSE);
	SelectorTool::SetStatusText(_R(IDS_SEL_DRAGBOXTEXT));
	
	// And tell the Dragging system that we need drags to happen
	StartDrag(DRAGTYPE_AUTOSCROLL);
}




/********************************************************************************************
>	void OpSelectorDragBox::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The current position of the mouse in Doc Coords
				ClickMods - Which key modifiers are being pressed
	Purpose:	This is called every time the mouse moves, during a drag. All it really does
				is update the EORed bounding box
	SeeAlso:	ClickModifiers
********************************************************************************************/

void OpSelectorDragBox::DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods,
										Spread* pSpread, BOOL bSolidDrag)
{
	// First Rub out the old box
	DocRect Rect = DocRect(MIN(StartPoint.x, LastMousePosition.x),
						   MIN(StartPoint.y, LastMousePosition.y),
						   MAX(StartPoint.x, LastMousePosition.x),
						   MAX(StartPoint.y, LastMousePosition.y));
	RenderDragBlobs(Rect, StartSpread, bSolidDrag);

	// Update the box and draw in the new one
	if (pSpread != StartSpread)
	{
		PointerPos = MakeRelativeToSpread(StartSpread, pSpread, PointerPos);
	}

	LastMousePosition = PointerPos;
	Rect = DocRect(MIN(StartPoint.x, LastMousePosition.x),
				   MIN(StartPoint.y, LastMousePosition.y),
				   MAX(StartPoint.x, LastMousePosition.x),
				   MAX(StartPoint.y, LastMousePosition.y));
	RenderDragBlobs(Rect, StartSpread, bSolidDrag);
}



/********************************************************************************************
>	void OpSelectorDragBox::DragFinished(DocCoord PointerPos,
										 ClickModifiers ClickMods, BOOL Success, BOOL bSolidDrag)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Inputs:		PointerPos - The position of the mouse at the end of the drag
				ClickMods - the key modifiers being pressed
				Success - TRUE if the drag was terminated properly, FALSE if it
				was ended with the escape key being pressed
	Purpose:	This is called when a drag operation finishes. This removes the EORed drag
				rect from the screen and then selects all the objects that were in it
	SeeAlso:	ClickModifiers
********************************************************************************************/

void OpSelectorDragBox::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods,
									 Spread* Spread, BOOL Success, BOOL bSolidDrag)
{
	// Build the rectangle of the drag box at the end of the drag
	DocRect BoundingRect(MIN(StartPoint.x, LastMousePosition.x),
						 MIN(StartPoint.y, LastMousePosition.y),
						 MAX(StartPoint.x, LastMousePosition.x),
						 MAX(StartPoint.y, LastMousePosition.y));
								  
	// First Rub out the old box
	RenderDragBlobs(BoundingRect, StartSpread, bSolidDrag);

	// Put the hourglass up
	BeginSlowJob();

	// Go and try and select a few things
	if (Success)
	{
		// If we didn't drag with the right button then deselect everything prior to selecting
		// the "lasso-ed" objects, and remember to restore the rotation centre to the middle of
		// the selection.
		if (!ClickMods.Adjust)
		{
			NodeRenderableInk::DeselectAll();
			((SelectorTool*) Tool::GetCurrent())->InvalidateRotationCentre();
		}

		// Select the objects in the BoundingRect
		NodeRenderableInk::SelectAllInRect(BoundingRect, Spread,
										   NodeRenderableInk::SET);
/*
		// This old code makes right-button marquee consistent with the right-button selection toggle.
		// Unfortunately Charles & Burns don't like it (this week).
		NodeRenderableInk::SelectAllInRect(BoundingRect, Spread,
										   ClickMods.Adjust ? NodeRenderableInk::SelStateAction::TOGGLE
										   					: NodeRenderableInk::SelStateAction::SET);
*/
		// Reenable the tool's idle processing.
		SelectorTool::AllowIdleWork(TRUE);

		// End the Drag
		if (!EndDrag()) FailAndExecute();
	}
	else
	{
		// Reenable the tool's idle processing.
		SelectorTool::AllowIdleWork(TRUE);

		// Set up the flags that say it all went wrong.
		EndDrag();
		FailAndExecute();
	}

	// Final end of the operation.
	End();

}


/********************************************************************************************
>	void OpSelectorDragBox::RenderDragBlobs( DocRect Rect, Spread *pSpread, BOOL bSolidDrag )

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/93
	Purpose:	EORs a rectangle onto the screen to mark out the size of the current
				selection.
********************************************************************************************/

void OpSelectorDragBox::RenderDragBlobs(DocRect Rect, Spread *pSpread, BOOL bSolidDrag)
{
	RenderRegion* pRegion = DocView::RenderOnTop(&Rect, pSpread, ClippedEOR);
	while (pRegion)
	{
		// Set the line colour 
		pRegion->SetLineColour(COLOUR_XORSELECT);

		// And Draw the rect
		DocRect RubberBox = DocRect(MIN(StartPoint.x, LastMousePosition.x),
									MIN(StartPoint.y, LastMousePosition.y),
									MAX(StartPoint.x, LastMousePosition.x),
									MAX(StartPoint.y, LastMousePosition.y));
		
		// Draw the rectangle
		pRegion->DrawDragRect(&RubberBox);

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Rect);
	}
}



/********************************************************************************************
>	BOOL OpSelectorDragBox::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations
********************************************************************************************/

BOOL OpSelectorDragBox::Declare()
{	
	return RegisterOpDescriptor(0, _R(IDS_SELECTOR_BOX), CC_RUNTIME_CLASS(OpSelectorDragBox), 
								OPTOKEN_SELECTOR_DRAGBOX, OpSelectorDragBox::GetState);
}



/********************************************************************************************
>	OpState OpSelectorDragBox::GetState(String_256* Description, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Outputs:	Description - GetState fills this string with an approriate description
				of the current state of the selector tool
	Returns:	The state of the operation, so that menu items (ticks and greying can be
				done properly
	Purpose:	Find out the state of the operation at the specific time
********************************************************************************************/

OpState OpSelectorDragBox::GetState(String_256* Description, OpDescriptor*)
{
	OpState Blobby;
	return Blobby;
}



/********************************************************************************************
>	OpDragRotateCentre::OpDragRotateCentre()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs an Operation that allows the user to drag around the centre of
				rotation blobby.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

OpDragRotateCentre::OpDragRotateCentre()
{
	// Empty.
}



/********************************************************************************************
>	void OpDragRotateCentre::StartDragCentreBlob(Spread* pSpread, const DocCoord& dcPos,
											 	 ClickModifiers)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		pSpread			the spread containing the rotation centre blob
				dcPos			the initial position of the blob

				The ClickModifiers argument is currently unused.
	Outputs:	-
	Returns:	-
	Purpose:	Starts a drag of the rotation centre blobby.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpDragRotateCentre::StartDragCentreBlob(Spread* pSpread, const DocCoord& dcPos,
											 ClickModifiers)
{
	// Put some helpful text in the status bar and prevent the selector tool overwriting
	// it until we are done.
	SelectorTool::AllowIdleWork(FALSE);
	SelectorTool::SetStatusText(_R(IDS_SEL_DRAGROTCENTRE));

	// Remember the starting positions etc and initiate a drag.
	m_pStartSpread = pSpread;
	m_dcLastMousePos = m_dcLastPos = m_dcFirstPos = dcPos;
	StartDrag(DRAGTYPE_AUTOSCROLL);
}



/********************************************************************************************
>	void OpDragRotateCentre::DragPointerMove(DocCoord dcPos, ClickModifiers, Spread* pSpread, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		dcPos			the current mouse position
				pSpread			the spread containing the mouse
	Outputs:	-
	Returns:	-
	Purpose:	Called while the rotation centre blobby is being dragged around.  Erases it
				from its old position and redraws it at its new position.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpDragRotateCentre::DragPointerMove(DocCoord dcPos, ClickModifiers mods, Spread* pSpread, BOOL bSolidDrag)
{	
	// If the mouse has moved outside the spread the drag was started on then we must
	// account for this.
	if (pSpread != m_pStartSpread)
	{
		// Convert from one spread's coordinates to another's.
		dcPos = MakeRelativeToSpread(m_pStartSpread, pSpread, dcPos);
	}

	// Now remember this mouse position.
	m_dcLastMousePos = dcPos;

	// If the constrain key is down then "snap" to the nearest blob position (as if the
	// "telephone keypad" had been clicked).
	if (mods.Constrain)
	{
		// Find the current nearest blob.
		DocCoord dcNearest;
		GetNearestBlob(dcPos, &dcNearest, NULL);

		// If it is the same as the last snapped position do nothing, otherwise update
		// our records.
		if (dcNearest == m_dcLastPos) return;
		dcPos = dcNearest;
	}
	else
	{
		// If the constrain key ISN'T down then we will snap to the grid, if appropriate.
		DocView::SnapCurrent(pSpread, &dcPos);
	}

	// Erase the old centre blob and draw the new one.
	RenderDragBlobs(CalcBlobClipRect(m_dcLastPos), m_pStartSpread, bSolidDrag);
	m_dcLastPos = dcPos;
	RenderDragBlobs(CalcBlobClipRect(m_dcLastPos), m_pStartSpread, bSolidDrag);
}



/********************************************************************************************
>	virtual BOOL OpDragRotateCentre::DragKeyPress(KeyPress* pKey, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		pKey			pointer to the KeyPress object representing a key event
	Outputs:	-
	Returns:	TRUE if the key-press was handled, FALSE otherwise.
	Purpose:	Checks for the constrain key going up or down, snapping the dragged rotation
				centre to the nearest bounds blob when it goes down, freeing it to follow 
				the mouse when the key goes up.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL OpDragRotateCentre::DragKeyPress(KeyPress* pKey, BOOL bSolidDrag)
{
	// Is this the constrain key going down or up?  Note that we have to explicitly compare
	// against the virtual key-code as the keypress stuff has a bug in it - it won't have the
	// constrain bit set when the release bit is set as well (ie. it doesn't send CTRL key-up
	// events).
	if (pKey->GetVirtKey() == CAMKEY(CC_MOD_CONSTRAIN))
	{
		// Treat the keypress as it was a mouse move to get the right snapping behaviour...
		DragPointerMove(m_dcLastMousePos,ClickModifiers::GetClickModifiers(),m_pStartSpread, FALSE);
		
		// Yes, we did process this one.
		return TRUE;
	}

	// Nope, didn't process it.
	return FALSE;
}



/********************************************************************************************
>	void OpDragRotateCentre::DragFinished(DocCoord dcPos, ClickModifiers mods,
									  	  Spread* pSpread, BOOL fDragOK, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		dcPos		final position of the mouse
				pSpread		spread containing the mouse
				fDragOK		whether the drag was cancelled or not
	Outputs:	-
	Returns:	-
	Purpose:	Moves the rotation centre to its final resting place (he he) and ends the
				drag.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpDragRotateCentre::DragFinished(DocCoord dcPos, ClickModifiers mods,
									  Spread* pSpread, BOOL fDragOK, BOOL bSolidDrag)
{
	// Put the hourglass up
	BeginSlowJob();

	// Act on the termination status of the drag operation.
	if (fDragOK)
	{
		// Treat the drag finish as a mouse move to gett he right snapping behaviour...
		DragPointerMove(dcPos,mods,pSpread, bSolidDrag);

		// Tell the selector tool that the rotation centre has moved.
		((SelectorTool*) Tool::GetCurrent())->RotationCentreDragged(m_dcLastPos);	

		// Reenable the tool's idle processing.
		SelectorTool::AllowIdleWork(TRUE);

		// End the Drag
		if (!EndDrag()) FailAndExecute();
	}
	else
	{
		// Remove the last drawn rotation centre blob.
		RenderDragBlobs(CalcBlobClipRect(m_dcLastPos), m_pStartSpread, bSolidDrag);

		// Redraw it at its original position.
		RenderDragBlobs(CalcBlobClipRect(m_dcLastPos = m_dcFirstPos), m_pStartSpread, bSolidDrag);

		// Reenable the tool's idle processing.
		SelectorTool::AllowIdleWork(TRUE);

		// Set up the flags that say it all went wrong.
		EndDrag();
		FailAndExecute();
	}

	// Finished.
	End();
}



/********************************************************************************************
>	void OpDragRotateCentre::RenderDragBlobs(DocRect drClip, Spread* pSpread, BOOL bSolidDrag)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		drClip			clipping rectangle for output
				pSpread			the spread to render into
	Outputs:	-
	Returns:	-
	Purpose:	Draws the rotation centre blobby at the last-known position of the mouse
				during a drag.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

void OpDragRotateCentre::RenderDragBlobs(DocRect drClip, Spread* pSpread, BOOL bSolidDrag)
{
	RenderRegion* pRegion = DocView::RenderOnTop(&drClip, pSpread, ClippedEOR);
	while (pRegion != NULL)
	{
		// Set the line colour 
		pRegion->SetLineColour(COLOUR_XORSELECT);
		
		// Draw the rotation centre at the given position.
		((SelectorTool*) Tool::GetCurrent())->RenderRotateCentre(pRegion, m_dcLastPos);

		// Get the next render region.
		pRegion = DocView::GetNextOnTop(&drClip);
	}
}



/********************************************************************************************
>	BOOL OpDragRotateCentre::Declare()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the op is correctly registered.
	Purpose:	Registers the rotation centre drag Operation.
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL OpDragRotateCentre::Declare()
{
	return RegisterOpDescriptor(0, /* _R(IDS_DRAGROTATECENTREOP), */ 0,
								CC_RUNTIME_CLASS(OpDragRotateCentre),
								OPTOKEN_SELECTOR_DRAGCENTRE, GetState);
}



/********************************************************************************************
>	OpState OpDragRotateCentre::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns a default OpState.
	Purpose:	Controls whether the rotation-centre mouse drag operation is available or
				not.  Currently, is always available (the internal logic of the selector
				tool & info-bar handle this really).
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

OpState OpDragRotateCentre::GetState(String_256*, OpDescriptor*)
{
	OpState os;
	return os;
}



/********************************************************************************************
>	static DocRect OpDragRotateCentre::CalcBlobClipRect(const DocCoord& dcPos)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/94
	Inputs:		dcPos		the centre of the blob that the clipping rectangle will surround
	Outputs:	-
	Returns:	A clipping rectangle for use by RenderDragBlobs.
	Purpose:	Constructs a rectangle that surrounds the given point during a mouse
				drag.  Some rendering functions require a clipping rectangle that isn't
				always available (poor design or what) so this makes one up as necessary.
	Errors:		-
	SeeAlso:	OpDragRotateCentre::RenderDragBlobs
********************************************************************************************/

DocRect OpDragRotateCentre::CalcBlobClipRect(const DocCoord& dcPos)
{
	INT32 nSize = SelectorTool::pBlobManager->GetBlobSize() + 2;
	return DocRect(DocCoord(dcPos.x - nSize, dcPos.y - nSize),
				   DocCoord(dcPos.x + nSize, dcPos.y + nSize));
}



/********************************************************************************************
>	void OpDragRotateCentre::GetNearestBlob(const DocCoord& dcPos,
											DocCoord* pPos,
											INT32* pBlob) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		dcPos			the point for which the nearest blob is to be found
	Outputs:	pPos			if not NULL then is set to the position of the nearest blob
				pBlob			if not NULL then is set to the blob ID of the nearest blob
	Returns:	-
	Purpose:	Finds the nearest bounds blob to the given point.
	Errors:		-
	SeeAlso:	OpDragRotateCentre::DragKeyPress; OpDragRotateCentre::DragPointerMove
********************************************************************************************/

void OpDragRotateCentre::GetNearestBlob(const DocCoord& dcPos, DocCoord* pPos, INT32* pBlob) const
{
	// Find out the nearest blob position to dcPos.
	SelectorTool* pTool = (SelectorTool*) Tool::GetCurrent();
	DocCoord dcNearestBlob(0,0);

	// This needs to be done a little carefully, as INT32_MAX itself is not big enough.
	XLONG nNearestDistance = XLONG(INT32_MAX);
	nNearestDistance *= nNearestDistance;			// square it to be safe

	// Compare against every blob.
	INT32 i;
	for (i = 0; i < 9; i++)
	{
		// Find out the square of the distance, in millipoints, to the i'th blob.
		DocCoord dcBlob = pTool->GetSelPosNearBlob(i);
		XLONG dx  = XLONG(dcPos.x) - XLONG(dcBlob.x);
		XLONG dy  = XLONG(dcPos.y) - XLONG(dcBlob.y);
		XLONG dz2 = (dx * dx) + (dy * dy);

		// If it's nearer remember it.
		if (dz2 < nNearestDistance)
		{
			nNearestDistance = dz2;
			dcNearestBlob = dcBlob;
		}
	}

	// Set the output paramaters accordingly.
	if (pPos  != NULL) *pPos  = dcNearestBlob;
	if (pBlob != NULL) *pBlob = i;
}
