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
/*
	$Header: /wxCamelot/Kernel/ngcore.cpp 57    17/11/05 15:31 Luke $
	Attribute gallery core object
*/

#include "camtypes.h"

//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ngcore.h"
#include "ngdialog.h"
#include "ngitem.h"
#include "ngprop.h"
#include "ngscan.h"
#include "ngsentry.h"
#include "ngsetop.h"

#include "comattrmsg.h"

#include "keypress.h"
#include "sginit.h"
#include "sgmenu.h"

//#include "galres.h"
//#include "galstr.h"

#include "helpuser.h"
//#include "xshelpid.h"
//#include "helppath.h"
//#include "mario.h"

//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"
#include "tmpltatr.h"
#include "layer.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"
#include "zoomops.h"
#include "transop.h"
//#include "slicetool.h"
//#include "sliceres.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opscale.h"
#include "opsquash.h"
#include "attrappl.h" // Matt - 12/02/2001 - For OpApplyAttribToSelected
#include "ophist.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(NameGallery, SuperGallery);

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

#define TIMER_ELAPSE 150

// The one and only NameGallery object.
NameGallery* NameGallery::m_pInstance = 0;


/********************************************************************************************
>	NameGallery::NameGallery()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Constructs a NameGallery object.
********************************************************************************************/

NameGallery::NameGallery()
  :	m_pUsedNames(0),
	m_nPropertyIndex(NamedExportProp::nIndex),
	m_nRefresh(0),
	m_nHiddenUpdates(0),
	m_fMenusCreated(FALSE),
	m_fChildChanges(FALSE),
	m_Timer(this)
{
	DlgResID = _R(IDD_NAMESGALLERY);
	ERROR3IF(m_pInstance != 0, "NameGallery::NameGallery: instance already exists");
	m_pInstance = this;
	m_BarToIgnoreTargetsOf = "";
	CreateDisplayTree();
}



/********************************************************************************************
>	NameGallery::~NameGallery()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Destroys a NameGallery object.
********************************************************************************************/

NameGallery::~NameGallery()
{
	m_pInstance = 0;
}



/********************************************************************************************
>	static NameGallery* NameGallery::Instance()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Returns:	The one and only Name gallery object.
********************************************************************************************/

NameGallery* NameGallery::Instance()
{
//	ERROR3IF(m_pInstance == 0, "NameGallery::Instance: no gallery");
	return m_pInstance;
}



/********************************************************************************************
>	SGNameGroup* NameGallery::GetFirstGroup()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Returns:	The first group within the Name Gallery.
********************************************************************************************/

SGNameGroup* NameGallery::GetFirstGroup()
{
	return (SGNameGroup*) (DisplayTree != 0 ? DisplayTree->GetChild() : 0);
}



/********************************************************************************************
>	SGUsedNames* NameGallery::GetUsedNames()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Returns:	The 'Used Names' group within the Name Gallery.
********************************************************************************************/

SGUsedNames* NameGallery::GetUsedNames()
{
	if (DisplayTree == 0) m_pUsedNames = 0;
	return m_pUsedNames;
}



/********************************************************************************************
>	INT32 NameGallery::GetPropertyIndex() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Returns:	The index of the properties currently shown in the gallery.
	SeeAlso:	NameGallery::SetPropertyIndex
********************************************************************************************/

INT32 NameGallery::GetPropertyIndex() const
{
	return m_nPropertyIndex;
}



/********************************************************************************************
>	INT32 NameGallery::SetPropertyIndex(INT32 nNewIndex)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		nNewIndex	---		the new index of the properties to show.
				fRedraw		---		whether to redraw the gallery to show the new properties
	Returns:	The old value of the index.
	SeeAlso:	NameGallery::GetPropertyIndex
********************************************************************************************/

INT32 NameGallery::SetPropertyIndex(INT32 nNewIndex)
{
	ERROR3IF(0 > nNewIndex || nNewIndex >= SGNameProp::nPropertyCount,
								"NameGallery::GetPropertyIndex: index out of range");
	INT32 n = m_nPropertyIndex;
	m_nPropertyIndex = nNewIndex;
	DisplayDirty();
	return n;
}



/********************************************************************************************
>	void NameGallery::GetHighlightCount(INT32* pnTotalItems, INT32* pnJustNames)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Outputs:	pnTotalItems	---		how many items in total are highlighted
				pnJustNames		---		how many 'Used Names' are highlighted
	Purpose:	Outputs the highlighted status of items in the Attribute gallery.  Used
				by various GetState() functions to  determine the 'grey' status of their
				associated operations.
	SeeAlso:	various GetState() functions in kernel\ngsetop.cpp
********************************************************************************************/

void NameGallery::GetHighlightCount(INT32* pnTotalItems, INT32* pnJustNames)
{
	// At least one output is mandatory (and sane).
	ERROR3IF(pnTotalItems == 0 && pnJustNames == 0,
					"NameGallery::GetHighlightCount: null arguments");

	// Count how many SGNameItems are highlighted in all groups.
	if (pnTotalItems != 0)
		*pnTotalItems = GetSelectedItemCount();
	
	// Count how many SGNameItems are highlighted in the SGUsedNames group.
	if (pnJustNames != 0)
		*pnJustNames = (GetUsedNames() != 0) ? GetUsedNames()->GetSelectedItemCount() : 0;
}



/********************************************************************************************
>	void NameGallery::ForceUpdate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Forces an immediate redraw of the Attribute gallery.  Useful when making
				changes to the status of sets within the document and querying their
				new status before the gallery redraws in the normal course of (idle)
				events.
	SeeAlso:	NameGallery::DisplayDirty; NameGallery::OnIdleEvent
********************************************************************************************/

void NameGallery::ForceUpdate()
{
	DisplayDirty();
	//OnIdleEvent();
	//OnTimerEvent();
}



/***********************************************************************************************
>   BOOL NameGallery::PreTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam,
									 Range* pRange)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> Modified sjk
	Created:    27/6/99 - 3/4/00
	Inputs:		pOp		---		UndoableOperation context for edits to triggers
				pParam	---		for passing to AllowOp calls
				pRange	---		objects being edited.
	Returns:	TRUE if all targets of triggers affected by the edit successfully AllowOp.
	Purpose:	AllowOp now called in the PostTrigger bit where all the extending is done.
				The PreTrigger just needs to record the Op ptr so that the extending actions
				can be added to this ops action lists.
	SeeAlso:	NameGallery::PostTriggerEdit
***********************************************************************************************/

BOOL NameGallery::PreTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam, Range* pRange)
{
	// allowops now called for extending in the PostTriggerEdit
	m_fChildChanges = TRUE;
	m_LastOpUsed = pOp;
	m_TouchedBar = -1;
	return TRUE;
}



/***********************************************************************************************
>   BOOL NameGallery::PreTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam,
									 Node* pNode)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> Modified sjk
	Created:    27/6/99 - 3/4/00
	Inputs:		pOp		---		UndoableOperation context for edits to triggers
				pParam	---		for passing to AllowOp calls
				pNode	---		object being edited.
	Returns:	TRUE if all targets of triggers affected by the edit successfully AllowOp.
	Purpose:	AllowOp now called in the PostTrigger bit where all the extending is done.
				The PreTrigger just needs to record the Op ptr so that the extending actions
				can be added to this ops action lists.
	SeeAlso:	NameGallery::PostTriggerEdit
***********************************************************************************************/

BOOL NameGallery::PreTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam, Node* pNode)
{
	// allowops now called for extending in the PostTriggerEdit
	m_fChildChanges = TRUE;
	m_LastOpUsed = pOp;
	m_TouchedBar = -1;
	return TRUE;
}



/***********************************************************************************************
>   BOOL NameGallery::PostTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/6/99
	Inputs:		pOp		---		UndoableOperation context for edits to triggers
				pParam	---		for passing to AllowOp calls
	Returns:	TRUE if all targets of triggers affected by the edit successfully update.
	Purpose:	Does a quick test to see if there are any targets defined.
				If so there may be some stretching to do, so it calls FastApplyStretchScan() to do it.
				Note that FastApplyStretchScan() calls the NoStretchUpdateChangedNodes() version
				of UpdateChangedNodes().
	SeeAlso:	NameGallery::PreTriggerEdit
	Notes:		This should be called AFTER the UpdateChangedNodes call for the nodes
				directly affected by the given Operation.
***********************************************************************************************/

BOOL NameGallery::PostTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam)
{
	// If there are no triggers/targets affected then there's nothing to do.
	if (!m_fChildChanges || (!Document::GetCurrent()->GetSetSentinel()->TargetsExist() && m_TouchedBar == -1) )
		return TRUE;

	// Scan for trigger sets which have changed bounds, and stretch/extend their target
	// sets appropiately.
	BOOL ok = FastApplyStretchScan(*pParam);

	// reset these variables since we have just completed any scans and stretches
	// and everything should now be in the right place
	m_fChildChanges = FALSE;
	m_BarToIgnoreTargetsOf = "";

	return ok;
}




/********************************************************************************************
>	virtual BOOL NameGallery::PreCreate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise
	Purpose:	The NameGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very beginning of the
				SuperGallery::Create method, before the window has been created.
	Notes:		As this is called before the window is open, it must not attempt to touch
				any of the button gadgets in the window, or force redraws, etc. Also,
				events cannot be passed to the tree, as the tree formatting relies on
				knowing the window size - however, the tree will be reformatted and
				redrawn automatically when the window is opened - this will happen shortly.
********************************************************************************************/

BOOL NameGallery::PreCreate()
{
	// Try to scan through the tree creating items for each set.
	CreateDisplayTree();
	return CreateDisplayScan().Scan();
}



/********************************************************************************************
>	BOOL NameGallery::CreateDisplayTree()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/00
	Purpose:	Initialises the gallery's display tree with the initial groups.
	SeeAlso:	NameGallery::FastUpdateNamedSetSizes
********************************************************************************************/

BOOL NameGallery::CreateDisplayTree()
{
	// If there's no 'selected' document then just destroy the display.
	if (Document::GetSelected() == 0 && DisplayTree != 0)
	{
		DisplayTree->DestroySubtree();
		delete DisplayTree;
		DisplayTree = 0;
		m_pUsedNames = 0;
	}

	// Create the display tree, if necessary.
	if (DisplayTree == 0)
	{
		DisplayTree = new SGDisplayRootScroll(this);
		ERRORIF(DisplayTree == 0, _R(IDE_NOMORE_MEMORY), FALSE);
			
		m_pUsedNames = new SGUsedNames;
		ERRORIF(m_pUsedNames == 0, _R(IDE_NOMORE_MEMORY), FALSE);
		DisplayTree->AddItem(m_pUsedNames);
		
		SGUsedFonts* pFonts = new SGUsedFonts;
		ERRORIF(pFonts == 0, _R(IDE_NOMORE_MEMORY), FALSE);
		DisplayTree->AddItem(pFonts);

		SGUsedBitmaps* pBitmaps = new SGUsedBitmaps;
		ERRORIF(pBitmaps == 0, _R(IDE_NOMORE_MEMORY), FALSE);
		DisplayTree->AddItem(pBitmaps);

		SGUsedColours* pColours = new SGUsedColours;
		ERRORIF(pColours == 0, _R(IDE_NOMORE_MEMORY), FALSE);
		DisplayTree->AddItem(pColours);
	}

	return TRUE;
}



/********************************************************************************************
>	virtual MsgResult NameGallery::Message(Msg* pMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Inputs:		pMessage - The message to handle
	Purpose:	A standard message handler, really.
	Notes:		Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.

				NOTE WELL that the SuperGallery base class handler does some funky things
				for us - see SuperGallery::Message - such as deleting our display subtree
				for any document which dies (which, uncannily, would explain why they go
				away like that when you close documents ;-), and shading the gallery when
				there are no documents present. [To override this behaviour in these cases,
				you should respond to the message, and return OK rather than calling the
				base class message handler]

	SeeAlso:	SuperGallery::Message; NameGallery::UpdateList
********************************************************************************************/

MsgResult NameGallery::Message(Msg* pMessage)
{
	// Handle clicks on buttons associated with this gallery.
	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* pMsg = (DialogMsg*) pMessage;
		switch (pMsg->DlgMsg)
		{
		case DIM_CREATE:
			SGInit::UpdateGalleryButton( _R(OPTOKEN_DISPLAY_NAME_GALLERY), TRUE);
			break;

		case DIM_CANCEL:
			SGInit::UpdateGalleryButton( _R(OPTOKEN_DISPLAY_NAME_GALLERY), FALSE);
			break;

		case DIM_LFT_BN_CLICKED:
			if( _R(IDC_GALLERY_HELP) == pMsg->GadgetID )
			{
				HelpUserTopic(_R(IDS_HELPPATH_Gallery_Name)); break;
			}
			else
			if( _R(IDC_GALLERY_NEW)		== pMsg->GadgetID ||
				_R(IDC_NAMEGAL_RENAME)	== pMsg->GadgetID ||
				_R(IDC_NAMEGAL_SELECT)	== pMsg->GadgetID ||
				_R(IDC_NAMEGAL_INTERSECT) == pMsg->GadgetID ||
				_R(IDC_GALLERY_DELETE)	== pMsg->GadgetID ||
				_R(IDC_LIBGAL_REMOVE)	== pMsg->GadgetID ||
				_R(IDC_NAMEGAL_EXPORT)	== pMsg->GadgetID )
			{
				// By-pass default base class handling for these buttons so the default
				// bar implementation can automatically invoke the operations defined
				// for them in bars.ini.
				return DialogOp::Message(pMessage);
			}
			if( _R(IDC_GALLERY_APPLY)	== pMsg->GadgetID ||
				_R(IDC_GALLERY_REDEFINE) == pMsg->GadgetID )
			{
				// Do as above, but afterwards check to ensure that (if any of the sets
				// affected is involved in a stretch) no NodeRegularShapes are involved
				MsgResult tempMsg = DialogOp::Message(pMessage);


				// For every SGNameItem currently selected, check if it is involved in a stretch...
				NameGallery *pNameGallery = NameGallery::Instance();
				if (!pNameGallery)	{ return tempMsg;	}

				SGUsedNames *pNames = pNameGallery->GetUsedNames();
				if (!pNames)	{ return tempMsg;	}

				SGNameItem *pNameGalleryItem = (SGNameItem*) pNames->GetChild();
				while (pNameGalleryItem)
				{
					if ((SGDisplayNode *)((SGDisplayItem *)pNameGalleryItem)->Flags.Selected)
					{
						NodeSetProperty* pPropNode = pNameGalleryItem->GetPropertyNode();
						if (pPropNode)
						{
							NamedStretchProp* pProp = (NamedStretchProp*) pPropNode->GetProperty(NamedStretchProp::nIndex);
							if (pProp && pProp->GetState())
							{
								// OK, we should have the NamedStretchProp for the current selected SGNameItem
								// AND we have checked to see if it is flagged as being stretched by something...
								// Therefore, we want to check if it contains a NodeRegularShape...
								if (!pProp->ValidateStretchingObjects(pNameGalleryItem))
								{
									InformWarning(_R(IDE_SGNODEREGULARSHAPESDETECTED));
									return tempMsg;
								}
							}
						}

					}

					pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
				}

				return tempMsg;
			}
			break;
			
		default:
			break;
		}
	}

	// If we've changed to a different document then refresh everything.
	else if (MESSAGE_IS_A(pMessage, DocChangingMsg))
	{
		DocChangingMsg* pMsg = (DocChangingMsg*) pMessage;
		if (pMsg->State == DocChangingMsg::SELCHANGED)
			DisplayDirty();
		else if (pMsg->State == DocChangingMsg::BORN)
		{
			// set everything into a clean state for the new doc
			if (DisplayTree != 0)
			{
				DisplayTree->DestroySubtree();
				delete DisplayTree;
				DisplayTree = 0;
				m_pUsedNames = 0;
			}

			CreateDisplayTree();
		}
	}
	
	// Redraw after an undo, a redo or a successful undoable operation. We use this
	// rather blanket approach because there isn't a message sent around to notify us
	// of all the possible changes to attributes and objects that interest us.
	else if (MESSAGE_IS_A(pMessage, OpMsg))
	{
		OpMsg* pMsg = (OpMsg*) pMessage;
		switch (pMsg->MsgType)
		{
		case OpMsg::BEGIN:
			// An operation is about to be invoked, so clear out the "affected" and
			// "child change" flags etc, ready for calls to PreTriggerEdit etc.
			m_fChildChanges = FALSE;
			break;

		case OpMsg::END:
			if (!pMsg->pOp->IS_KIND_OF(UndoableOperation)) break;
			// fall through

		case OpMsg::AFTER_UNDO:
		case OpMsg::AFTER_REDO:
			DisplayDirty();
			break;
		
		default:
			break;
		}			
	}

	// Redraw after changes to the common attributes.  This will catch changes to
	// the currently selected objects.
	else if (MESSAGE_IS_A(pMessage, CommonAttrsChangedMsg))
		DisplayDirty();

	// Always pass messages on for base class handling.
	return SuperGallery::Message(pMessage);
}



/********************************************************************************************
>	virtual void NameGallery::DisplayDirty()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Marks a gallery as needing to be refreshed/redrawn on the next idle
				event sent to the NameGallery.
	SeeAlso:	NameGallery::Message; NameGallery::OnIdleEvent
********************************************************************************************/

// the timer event adds the idle processor
// which adds to the  latency. Ie we wait 150 ms, before waiting for a free cpu cycle (or 2) to do the scan.
// So why do we have a timer in there too? Well the msgs that the name gallery reacts to tend to come in bursts
// so putting a timer in hopes to minimise unneeded scans that are likely to occur when the msg burst occurs.
// The idles were previously being called more optimistically than hoped. (sjk 6/10/00)
BOOL NameGallery::OnTimerEvent()
{
	if (m_nRefresh != 0)
	{
		GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_LOW, this);
		m_Timer.Stop();
	}

	return TRUE;
}

 

void NameGallery::DisplayDirty()
{
	// If the gallery display exists and isn't visible then just remember that it needs
	// refreshing when it next shows.

	if (!IsVisible())
	{
		// needs updating later
		m_nHiddenUpdates++;
		return;
	}

	// If going from the clean to the dirty state then send the gallery high
	// priority idle events.
	// needs updating very soon now
	if (m_nRefresh++ == 0)
		m_Timer.Start(TIMER_ELAPSE, TRUE);
}



/********************************************************************************************
>	virtual BOOL NameGallery::OnIdleEvent()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Called on idle events.  Updates the selection if it's been changed, and
				recreates and redraws SGNameGroups if the named objects in their
				associated Documents have been changed.
	Notes:		We bypass the standard SuperGallery OnIdleEvent background redraw code as
				we may have to destroy SGNameItems during the refresh operation, which
				cannot be done during the HandleEvent processing.
	SeeAlso:	NameGallery::Message; NameGallery::OnIdleEvent; SGNameGroup::SetRefresh;
				Application::RegisterIdleProcessor
********************************************************************************************/

BOOL NameGallery::OnIdleEvent()
{
	// If they're out of date, recreate the items in this group and redraw them.
	if (m_nRefresh != 0)
	{
		// Reconstruct and redraw the display tree.
		PreCreate();
		if (IsVisible())
			ForceRedrawOfList();

		// Kill idle events now the update has been done.
		GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_LOW, this);
		m_nRefresh = 0;

		// Inform the zoom combo that it needs to be updated.
		if (DocView::GetCurrent() != 0)
			OpZoomComboDescriptor::Update(TRUE);
	}

	// Don't call the base class handler.
	return FALSE;	// we don't want more idles - we've either removed ourself or nothing is out of date
}



/********************************************************************************************
>	virtual OpState NameGallery::GetCommandState(StringBase* pstrCommandID,
												 String_256* pstrShade)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Inputs:		pstrCommandID	---		the ID of the command
	Outputs:	pstrShade		---		if returning (OpState.Greyed == TRUE) then this 
										is set to the reason that the item is shaded/greyed.
	Returns:	An OpState indicating the current menu item state.
	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's
				GetState).
	Notes:		Override this method to provide state info for your special commands
				Call the base class for unknown commands.

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine;
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;
********************************************************************************************/

OpState NameGallery::GetCommandState(StringBase* pstrCommandID, String_256* pstrShade)
{
	// Delegate to the corresponding operations or base class.
	const TCHAR* pTok;
		 if (*pstrCommandID == SGCmd_New)			pTok = OPTOKEN_NAME_OBJECTS_DLG;
	else if (*pstrCommandID == SGCmd_Rename)		pTok = OPTOKEN_RENAME_OBJECTS_DLG;
	else if (*pstrCommandID == SGCmd_Select)		pTok = OPTOKEN_SELECT_UNION_SETS;
	else if (*pstrCommandID == SGCmd_Intersect)		pTok = OPTOKEN_SELECT_INTERSECT_SETS;
	else if (*pstrCommandID == SGCmd_Apply)			pTok = OPTOKEN_APPLY_NAMES_TO_SEL;
	else if (*pstrCommandID == SGCmd_RemoveNames)	pTok = OPTOKEN_REMOVE_NAMES_FROM_SEL;
	else if (*pstrCommandID == SGCmd_Redefine)		pTok = OPTOKEN_REDEFINE_NAMES_AS_SEL;
	else if (*pstrCommandID == SGCmd_Delete)		pTok = OPTOKEN_DELETE_NAMES_FROM_ALL;
	else if (*pstrCommandID == SGCmd_Export)		pTok = OPTOKEN_EXPORT_SETS;
	else
		return SuperGallery::GetCommandState(pstrCommandID, pstrShade);

	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor((TCHAR*) pTok);
	ERROR3IF(pDesc == 0, "NameGallery::GetCommandState: can't find descriptor");
	return pDesc->GetOpsState(pstrShade);
}



/********************************************************************************************
>	virtual void NameGallery::DoCommand(StringBase* pstrCommandID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Inputs:		pstrCommandID	---		the ID of the command
	Purpose:	Overrides the default virtual function in the SuperGallery base class so
				as to handle the custom SGACTION_UNNAME command etc.
********************************************************************************************/

void NameGallery::DoCommand(StringBase* pstrCommandID)
{
	// Deletegate to the ops or the base class.
	const TCHAR* pTok;
		 if (*pstrCommandID == SGCmd_New)			pTok = OPTOKEN_NAME_OBJECTS_DLG;
	else if (*pstrCommandID == SGCmd_Rename)		pTok = OPTOKEN_RENAME_OBJECTS_DLG;
	else if (*pstrCommandID == SGCmd_Select)		pTok = OPTOKEN_SELECT_UNION_SETS;
	else if (*pstrCommandID == SGCmd_Intersect)		pTok = OPTOKEN_SELECT_INTERSECT_SETS;
	else if (*pstrCommandID == SGCmd_Apply)			pTok = OPTOKEN_APPLY_NAMES_TO_SEL;
	else if (*pstrCommandID == SGCmd_RemoveNames)	pTok = OPTOKEN_REMOVE_NAMES_FROM_SEL;
	else if (*pstrCommandID == SGCmd_Redefine)		pTok = OPTOKEN_REDEFINE_NAMES_AS_SEL;
	else if (*pstrCommandID == SGCmd_Delete)		pTok = OPTOKEN_DELETE_NAMES_FROM_ALL;
	else if (*pstrCommandID == SGCmd_Export)		pTok = OPTOKEN_EXPORT_SETS;
	else
	{
		SuperGallery::DoCommand(pstrCommandID);
		return;
	}

	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor((TCHAR*) pTok);
	ERROR3IF(pDesc == 0, "NameGallery::DoCommand: can't find descriptor");
	pDesc->Invoke();
}



/********************************************************************************************
>	virtual BOOL NameGallery::InitMenuCommands()
												 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/99
	Returns:	TRUE if successful.
	Purpose:	Initialises the pop-up menu commands used by the gallery.
********************************************************************************************/

BOOL NameGallery::InitMenuCommands()
{
	// Menus are only initialised once.
	if (m_fMenusCreated) return TRUE;
	return m_fMenusCreated =
				InitMenuCommand((StringBase*) &SGCmd_Find,			_R(IDS_SGMENU_FIND))
			 && InitMenuCommand((StringBase*) &SGCmd_Apply,			_R(IDS_SGMENU_APPLY))
			 && InitMenuCommand((StringBase*) &SGCmd_RemoveNames,	_R(IDS_SGMENU_REMOVE_NAMES))
			 && InitMenuCommand((StringBase*) &SGCmd_Redefine,		_R(IDS_SGMENU_REDEFINE))
			 &&	InitMenuCommand((StringBase*) &SGCmd_Select,		_R(IDS_SGMENU_SELECT))
			 &&	InitMenuCommand((StringBase*) &SGCmd_Intersect,		_R(IDS_SGMENU_INTERSECT))
			 && InitMenuCommand((StringBase*) &SGCmd_New,			_R(IDS_SGMENU_NEW))
			 && InitMenuCommand((StringBase*) &SGCmd_Rename,		_R(IDS_SGMENU_RENAME))
			 && InitMenuCommand((StringBase*) &SGCmd_Delete,		_R(IDS_SGMENU_DELETE))
			 && InitMenuCommand((StringBase*) &SGCmd_Export,		_R(IDS_SGMENU_EXPORT))
			 && InitMenuCommand((StringBase*) &SGCmd_FoldGroup,		_R(IDS_SGMENU_FOLD))
			 && InitMenuCommand((StringBase*) &SGCmd_UnfoldGroup,	_R(IDS_SGMENU_UNFOLD))
			 && InitMenuCommand((StringBase*) &SGCmd_PrevGroup,		_R(IDS_SGMENU_PREVGROUP))
			 && InitMenuCommand((StringBase*) &SGCmd_NextGroup,		_R(IDS_SGMENU_NEXTGROUP));
}



/********************************************************************************************
>	virtual BOOL NameGallery::BuildCommandMenu(GalleryContextMenu* pMenu, SGMenuID id)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Inputs:		pMenu	---		the menu to add commands to
				id		---		the menu type (over-list or options-button) to create
	Returns:	TRUE if successful.
	Purpose:	Builds the pop-menu for gallery items.
********************************************************************************************/

BOOL NameGallery::BuildCommandMenu(GalleryContextMenu* pMenu, SGMenuID id)
{
	// Only handle pop-ups over display items.
	if (id == SGMENU_OVERITEM)
	{
		SGNameGroup* pGroup = (SGNameGroup*) FindCommandGroup();
		return AddCommand(pMenu, (StringBase*) &SGCmd_Find, TRUE)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Apply)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_RemoveNames)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Redefine, TRUE)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Select)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Intersect, TRUE)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_New)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Rename)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Delete, TRUE)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_Export, TRUE)
			&& AddCommand(pMenu, (StringBase*) ((pGroup == 0 || !pGroup->Flags.Folded)
													? &SGCmd_FoldGroup : &SGCmd_UnfoldGroup))
			&& AddCommand(pMenu, (StringBase*) &SGCmd_PrevGroup)
			&& AddCommand(pMenu, (StringBase*) &SGCmd_NextGroup);
	}

	// For all other menus just do the default.
	return SuperGallery::BuildCommandMenu(pMenu, id);
}



/********************************************************************************************
>	virtual BOOL NameGallery::ApplyAction(SGActionType nAction)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Inputs:		nAction		---		the SGACTION_XXX to perform.
	Returns:	TRUE if successful.
	Purpose:	Default handler for standard SGACTION_XXX commands.  Only implements
				SGACTION_APPLY(ADJUST), as required by SGDisplayNode::DefaultClickHandler.
				All Attribute gallery buttons (except for Help) are in fact proper
				stand-alone Camelot operations attached to particular button gadgets
				and hence their clicking bypasses the default gallery code.
********************************************************************************************/

BOOL NameGallery::ApplyAction(SGActionType nAction)
{
	switch (nAction)
	{
	case SGACTION_APPLY:
	case SGACTION_APPLYADJUST:
		{
			OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_APPLY_NAMES_TO_SEL);
			ERROR3IF(pDesc == 0, "NameGallery::ApplyAction: no descriptor");
			String_256 str;
			if (!pDesc->GetOpsState(&str).Greyed)
				pDesc->Invoke();
		}
		return TRUE;
		
	default:
		break;
	}

	return SuperGallery::ApplyAction(nAction);
}



/********************************************************************************************
>	virtual void NameGallery::SelectionHasChanged()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Purpose:	Overrides default SuperGallery button state handling to do nothing.
********************************************************************************************/

void NameGallery::SelectionHasChanged()
{
	// Do nothing except notify the system that its UI state has changed - the state of
	// all gallery buttons is determined by the operations they are attached to, allowing
	// them to be stand-alone in the original Camelot style.  The base class model is only
	// used for the display tree redraw and click handling.
	DialogBarOp::SetSystemStateChanged(TRUE);
}



/********************************************************************************************
>	virtual void NameGallery::SetVisibility(BOOL fOpen)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/99
	Purpose:	Overrides default SuperGallery hide/show handling to force an update to
				the gallery when 
********************************************************************************************/

void NameGallery::SetVisibility(BOOL fOpen)
{
	SuperGallery::SetVisibility(fOpen);
	if (fOpen && m_nHiddenUpdates != 0)
	{
		m_nHiddenUpdates = 0;
		DisplayDirty();
	}
}


/********************************************************************************************
>	BOOL NameGallery::FastUpdateNamedSetSizes(BOOL PropagateChanges = TRUE)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/3/00
	Inputs:		PropagateChanges - 
	Purpose:	The scans are too slow and this replaces the ForceRedraw function when in the
				middle of an op which just updates the sizes of the named sets and NOTHING ELSE.
				No node is looked at twice, and retests for "most likely" name gallery item first.
********************************************************************************************/
BOOL NameGallery::FastUpdateNamedSetSizes(BOOL PropagateChanges)
{
	TRACEUSER( "GerryX", _T("FastUpdateNamedSetSizes(%s)\n"), PropagateChanges ? _T("TRUE") : _T("FALSE") );

	// scan the tree looking for name attribs
	// find one and the size of the parent should be added to the bounds of the set
	Spread * pSpread =  Document::GetSelectedSpread();
	if (!pSpread)
		return FALSE;

	Node *pParent = NULL;
	SGUsedNames* pNames = GetUsedNames();
	if (!pNames)
		return FALSE;

	SGNameItem* pNameGalleryItem = NULL; 

	// reset all the names
	pNameGalleryItem = (SGNameItem*) pNames->GetChild();

	while (pNameGalleryItem)
	{
		pNameGalleryItem->Reset(PropagateChanges);
		pNameGalleryItem->m_BarNumber = -1;
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	String_256 str;
	String_256 AttrStr;
	String_256 BarName;

	// scan each layer
	Node * pTop = pSpread;
	Node * pNode = NULL;
	BOOL CheckLastAgainQuickly = TRUE;

	if (pTop)
	{
		// scan from the first layer all through the layers since they are brothers of this layer
		pNode = SliceHelper::FindNextNameNode(pTop, pTop);

		// check all the name gallery items
		while (pNode)
		{
			// if I have a last looked at gallery item, its a fair bet for the next one along
			CheckLastAgainQuickly = TRUE;
			if (!pNameGalleryItem)
			{
				pNameGalleryItem = (SGNameItem*) pNames->GetChild();
				CheckLastAgainQuickly = FALSE;
			}

			AttrStr = ((TemplateAttribute *)pNode)->GetParam();
			pParent = pNode->FindParent();

			// which set is it part of?
			while (pNameGalleryItem)
			{
				pNameGalleryItem->GetNameText(&str);
				// if the name matches its your man
				if (str.CompareTo(AttrStr) == 0)
				{
					pNameGalleryItem->Include(pParent);
					BarName = SliceHelper::GetBarName((TemplateAttribute *)pNode);
					if (BarName[0] && pNameGalleryItem->m_BarNumber == -1)
					{
						pNameGalleryItem->m_BarNumber = SliceHelper::GetBarNumberFromBarName(BarName);
//						TRACEUSER( "GerryX", _T("found bar %d\n"), pNameGalleryItem->m_BarNumber);
					}
					break; // shortcut out
				}

				// no then try the next name set?
				if (CheckLastAgainQuickly)
				{
					CheckLastAgainQuickly = FALSE;
					pNameGalleryItem = (SGNameItem*) pNames->GetChild();
				}
				else
					pNameGalleryItem = (SGNameItem*) pNameGalleryItem->GetNext();
			}

			// add this new name to the list
			if (!pNameGalleryItem)
			{
				pNameGalleryItem = pNames->RegisterMember(pParent, AttrStr);

				// and what bar is it associated with?
				if (pNameGalleryItem)
				{
					BarName = SliceHelper::GetBarName((TemplateAttribute *)pNode);
					if (BarName[0])
						pNameGalleryItem->m_BarNumber = SliceHelper::GetBarNumberFromBarName(BarName);
					else
						pNameGalleryItem->m_BarNumber = -1;
				}
			}

			pNode = SliceHelper::FindNextNameNode(pNode, pTop);
		}
	}

	TRACEUSER( "GerryX", _T("Named Sets\n"));
	pNameGalleryItem = (SGNameItem*) pNames->GetChild();
	while (pNameGalleryItem)
	{
		pNameGalleryItem->GetNameText(&str);
		TRACEUSER( "GerryX", _T("Item %s Bar = %d  Nodes = %d\n"), (TCHAR*)str, pNameGalleryItem->m_BarNumber, 
			pNameGalleryItem->GetObjectCount());
		DocRect Rect;
		Rect = pNameGalleryItem->GetSetBounds();
		TRACEUSER( "GerryX", _T("Bounds     = (%d, %d) (%d, %d)\n"), Rect.lo.x, Rect.lo.y, Rect.hi.y, Rect.hi.y);
		Rect = pNameGalleryItem->GetOldSetBounds();
		TRACEUSER( "GerryX", _T("Old Bounds = (%d, %d) (%d, %d)\n"), Rect.lo.x, Rect.lo.y, Rect.hi.y, Rect.hi.y);
		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	return TRUE;
}

// checks if the rects have the same height and width but ignores location
inline BOOL RectsAreTheSameSize(const DocRect & r1, const DocRect & r2)
{
	return ((r1.hi.x - r1.lo.x == r2.hi.x - r2.lo.x) && (r1.hi.y - r1.lo.y == r2.hi.y - r2.lo.y));
}


// extension to the NodeListItem to store an index too which I use to refer to an array of data
class NodeListItemWithIndex : public NodeListItem
{
public:
	NodeListItemWithIndex(Node* WhichNode, INT32 i) {pNode = WhichNode; Index = i;};	// initialise pNode to be WhichNode
	INT32 Index;
};

/********************************************************************************************
>	BOOL NameGallery::FastApplyStretchScan(ObjChangeParam & ObjChange)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/00
	Returns:	TRUE if it performs the extending ok
				FALSE if no extending was performed for any reason like none were defined
	Param:		ObjChange	The Objchange Param from the op that spawned this extend.
							This doesn't get altered in any way by this function.
	Purpose:	To do the exending once it has decided that it needs to be done.
				This requires a fast tree scan to find what has changed dimensions (see FastUpdateNamedSetSizes)
				Analysis of the name gallery items to work out which trigger has changed and which target
				should extend. Then it calls the extend bit on each node that needs it.
	SeeAlso:	NameGallery::FastUpdateNamedSetSizes
********************************************************************************************/
BOOL NameGallery::FastApplyStretchScan(ObjChangeParam & ObjChange)
{
	TRACEUSER( "GerryX", _T("FastApplyStretchScan\n"));

	Spread * pSpread =  Document::GetSelectedSpread();
	if (!pSpread || m_LastOpUsed == NULL)
		return FALSE;

	SGUsedNames* pNames = GetUsedNames();
	if (!pNames)
	{
		PreCreate(); // set up the name gallery
		pNames = GetUsedNames();
		if (!pNames)
			return FALSE;
	}

	BOOL ExtendOk = TRUE; // did it all extend ok?
	BOOL Recurse = FALSE;

	m_bResetARelationship = FALSE; // nothing reset yet

	// fill this array with data and it is all we will need to extend items when we scan the tree
	struct ExtendStructType
	{
		SGNameItem * pTriggerSet;
		SGNameItem * pTargetSet;
		NamedStretchProp * pTargetStretchProp;
		DocRect CleanTargetRect;
		DocRect	CombinedTriggerRect;
		DocRect	OldCombinedTriggerRect;
		BOOL PerformedExtendOK;
		DocRect TotalExtend;
	} ExtendStruct[MAX_SIM_EXTENDS]; // surely no more than 50 simaltainious extends going on!

	Node * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel(); // the sentinel - basically avoid it!
	SGNameItem* pNameGalleryItem = NULL;

	INT32 TimesInLoop = 0;

	// Added this line because OpCut (and presumably others) causes an OpPageResize
	// to happen which gets remembered as the last op and we crash at this point 
	// because the op has been deleted
	m_LastOpUsed = ObjChange.GetOpPointer();

	// list the ops that would allow us to pull a button apart
	BOOL IsPossiblePullApartOp =  m_LastOpUsed->IS_KIND_OF(TransOperation) && !IS_A(m_LastOpUsed, OpScaleTrans) && !IS_A(m_LastOpUsed, OpSquashTrans);
	BOOL IsAChangePropertyOp = IS_A(m_LastOpUsed, OpChangeBarProperty);
	BOOL IsAButtonNoChangingOp =
PORTNOTE("other", "Remove OpDuplicateBar and OpShortenBar")
#ifndef EXCLUDE_FROM_XARALX
									IS_A(m_LastOpUsed, OpDuplicateBar) || 
									IS_A(m_LastOpUsed, OpShortenBar) || 
#endif
									IS_A(m_LastOpUsed, OpRenameAll);

	// force recursion for these two ops
	Recurse = IsAButtonNoChangingOp;

	ObjChange.GetSettableChangeFlags()->RegenerateNode = TRUE;

	// which op is being called
	TRACEUSER( "GerryX", _T("%s\n"), (LPCTSTR) m_LastOpUsed->GetRuntimeClass()->m_lpszClassName);

	do // this do loop is used for the recursive bit of A extends B which extends C (NB it never gets to D though)
	{
		TimesInLoop++;

		// get the size of each set
		if (!FastUpdateNamedSetSizes())
			return FALSE; // scan says there was nothing there at all so don't bother to extend it mate!

		NodeSetProperty* pPropNode = NULL;
		NamedStretchProp* pProp = NULL;
		NodeBarProperty * pNodeBarProperty = (NodeBarProperty*) ((NodeSetSentinel *)pNodeSetSentinel)->FindBarProperty();

		// zero the array of bar sizes and other cached bar data
		memset (m_BarSize,0, sizeof(m_BarSize));

		INT32 TriggeredBar = -1;
		if (m_TouchedBar >= 0)
		{
			if (!SetBSTData(m_TouchedBar, 0, 0, 1, 0))
			{
				ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
				return FALSE;
			}
			TriggeredBar = m_TouchedBar;

			// adding a bar member is added over button1
			// so first we have to shuffle the button into the correct place
			// otherwise the back bar wont be able to expand around it
PORTNOTE("other", "Removed OpDuplicateBar");
#ifndef EXCLUDE_FROM_XARALX
			if (IS_A(m_LastOpUsed, OpDuplicateBar) && pNodeBarProperty->Bar(TriggeredBar).IsLive)
			{
			INT32 barDirection = pNodeBarProperty->Bar(TriggeredBar).IsHorizontal ? 1 : 2;
				if (!pNodeBarProperty->Bar(TriggeredBar).RequiresShuffle)
					barDirection = 0;

				ShuffleBar(	TriggeredBar,
							pNodeBarProperty->Bar(TriggeredBar).Spacing,
							barDirection,
							pNames,
							pSpread,
							ObjChange);

				FastUpdateNamedSetSizes(); // record the new positions
			}
#endif
		}

	//INT32 TriggersFound = 0; // not needed in the calculation any more but left as it can be useful for debugging

		/**** FIND OUT WHICH TRIGGERS HAVE BEEN FIRED ***/
		// find which triggers have changed
		// and set the affected trigger bool in the set item
		// which is doesn't require scanning for like it does in the stretch properties if set there
		pNameGalleryItem = (SGNameItem*) pNames->GetChild();
		while (pNameGalleryItem)
		{
			// init this the first time round only
			if (TimesInLoop == 1)
			{
				pNameGalleryItem->m_IsPartOfThisStretch = FALSE;
				pNameGalleryItem->ResetCachedPropertyNode(); // dont rely on a cached value find the actual value in the GetPropertyNode() call bellow!
			}

			pPropNode = pNameGalleryItem->GetPropertyNode();
			if (pPropNode)
				pProp = (NamedStretchProp*) pPropNode->GetProperty(NamedStretchProp::nIndex);

			if (pProp && pPropNode && (pNameGalleryItem->GetSetBounds() != pNameGalleryItem->GetOldSetBounds()
				|| (TimesInLoop > 1 && pNameGalleryItem->m_IsPartOfThisStretch)
				))
			{
				// require that this is a TRIGGER
				// ie it has targets set on it which should be stated in from the add stretches gallery
//				TriggersFound++;
				pNameGalleryItem->m_SetIsAffectedTrigger = TRUE;
			}
			else
				pNameGalleryItem->m_SetIsAffectedTrigger = FALSE;

			pNameGalleryItem->m_IsATrigger = FALSE; // blank this now to fill in later

			pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
		}

		/*** CALC THE LARGEST TRIGGER SIZE PER BAR ***/
		// work out the size of the largest trigger of each bar
		DocRect TempTriggerRect;
		SGNameItem* pTempTriggerSet = NULL;

		// find out the largest bar button size
		pNameGalleryItem = (SGNameItem*) pNames->GetChild();
		while (pNameGalleryItem)
		{
			TempTriggerRect.MakeEmpty();

			// scan each set that has a trigger associated
			pPropNode = pNameGalleryItem->GetPropertyNode();
			if (pPropNode)
			{
				pProp = (NamedStretchProp*) pPropNode->GetProperty(NamedStretchProp::nIndex);

				// union these triggers together and check if any of them have been fired (from the above scan)
				if (pProp && pProp->GetState() && !pProp->GetTriggers().empty())
				{
					// loop around each trigger of this target
					for (std::list<TriggerSet>::iterator pt = pProp->GetTriggers().begin();
						pt != pProp->GetTriggers().end();
						pt++)
					{
						pTempTriggerSet = SliceHelper::LookupNameGalleryItem(pt->m_strSet);

						if (pTempTriggerSet)
						{
							// union the trigger bounds together
							TempTriggerRect = TempTriggerRect.Union(pTempTriggerSet->GetSetBounds());

							// this is in a trigger list so is a trigger
							// this info can be used from now on
							pTempTriggerSet->m_IsATrigger = TRUE;

							// mark this bar as an active bar
							if (pTempTriggerSet->m_SetIsAffectedTrigger && pNameGalleryItem->m_BarNumber >= 0)
							{
								if (!SetBSTData(pNameGalleryItem->m_BarNumber, 0, 0, 1, 0))
								{
									ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
									return FALSE;
								}
								TriggeredBar = pNameGalleryItem->m_BarNumber;
							}
						}
					}
				}
			}

			// work out the largest unioned item in the bar
			// this is used as the size of the virtual trigger
			// which all triggers in a bar are assumed to be this size if the bar
			// is of equal size
			if (pNameGalleryItem->m_BarNumber >= 0 && pNameGalleryItem->m_BarNumber < MAX_BARS && !pNameGalleryItem->IsABackBar())
			{
				if (!SetBSTData(pNameGalleryItem->m_BarNumber,
					max( DWORD(TempTriggerRect.Width()), m_BarSize[pNameGalleryItem->m_BarNumber].MaxWidth ),
					max( DWORD(TempTriggerRect.Height()), m_BarSize[pNameGalleryItem->m_BarNumber].MaxHeight ), 0, 0 ) )
				{
					ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
					return FALSE;
				}
			}

			// mark which bars have backbars
			if (pNameGalleryItem->IsABackBar() && TimesInLoop == 1 && pNameGalleryItem->m_BarNumber >= 0)
			{
				if (!SetBSTData(pNameGalleryItem->m_BarNumber, 0, 0, 0, 1))
				{
					ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
					return FALSE;
				}
			}

			pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
		}

		// We know which triggers have fired, but we need to work out what targets these
		// triggers are affecting
	INT32 ExtendsFound = 0;

		/*** LOOK AT EACH TARGET IT IS EITHER 1) EXTENDING 2) RESETING OR 3) DOING NOTHING ***/
		
		pNameGalleryItem = (SGNameItem*) pNames->GetChild();
		while (pNameGalleryItem)
		{
			pPropNode = pNameGalleryItem->GetPropertyNode();
			if (pPropNode && pNodeBarProperty->Bar(pNameGalleryItem->m_BarNumber).IsLive)
			{
				// calc the size of the targets triggers
				pProp = (NamedStretchProp*) pPropNode->GetProperty(NamedStretchProp::nIndex);

				// union these triggers together and check if any of them have been fired (from the above scan)
				if (pProp && pProp->GetState() && !pProp->GetTriggers().empty())
				{
					ExtendStruct[ExtendsFound].CombinedTriggerRect.MakeEmpty();
					ExtendStruct[ExtendsFound].OldCombinedTriggerRect.MakeEmpty();

					// loop around each trigger of this target
					for (std::list<TriggerSet>::iterator pt = pProp->GetTriggers().begin();
						pt != pProp->GetTriggers().end();
						pt++)
					{
						ExtendStruct[ExtendsFound].pTriggerSet = SliceHelper::LookupNameGalleryItem(pt->m_strSet);

						// the trigger exists and it is a valid trigger
						if (ExtendStruct[ExtendsFound].pTriggerSet)
						{
							// union the trigger bounds together
							ExtendStruct[ExtendsFound].CombinedTriggerRect = ExtendStruct[ExtendsFound].CombinedTriggerRect.Union(ExtendStruct[ExtendsFound].pTriggerSet->GetSetBounds());
							ExtendStruct[ExtendsFound].OldCombinedTriggerRect = ExtendStruct[ExtendsFound].OldCombinedTriggerRect.Union(ExtendStruct[ExtendsFound].pTriggerSet->GetOldSetBounds());

							// this is a trigger that has been fired
							if (ExtendStruct[ExtendsFound].pTriggerSet->m_SetIsAffectedTrigger)
							{
								// mark it as part of this stretch
								ExtendStruct[ExtendsFound].pTriggerSet->m_IsPartOfThisStretch = TRUE;
							}
						}
					}

					// add on the virtual trigger size if it is a bar
					// if it is part of a bar and the bar same sizing is turned on then
					// expand the CombinedTriggerRect Around the center by the bar dims
					if (pNameGalleryItem->m_BarNumber >= 0 && // is part of a bar
						GetBSTTriggeredBar(pNameGalleryItem->m_BarNumber) && // and that bar has had a trigger fire in it
						ExtendStruct[ExtendsFound].pTriggerSet && // and it has a trigger associated
						!pNameGalleryItem->IsABackBar()) // and it is not a back bar
					{
						if (pNodeBarProperty->Bar(pNameGalleryItem->m_BarNumber).SameSize < 3 
							&& ( ExtendStruct[ExtendsFound].pTriggerSet->GetSetBounds().Height() != 0 || ExtendStruct[ExtendsFound].pTriggerSet->GetSetBounds().Width() != 0))
						{
							// expand up the combined and old combined trigger rects
							ExpandVirtualTriggers(	pNodeBarProperty->Bar(pNameGalleryItem->m_BarNumber).SameSize,
													pNameGalleryItem->m_BarNumber,
													ExtendStruct[ExtendsFound].CombinedTriggerRect);
						}
					}

					/*** DO WE EXTEND, RESET OR DO NOTHING? ***/

					// calc the dimensions of the trigger and the target
					// testing if these dimensions have changed is important to
					// work out if a stretch has occurred.
					BOOL TriggerSameSize = RectsAreTheSameSize(ExtendStruct[ExtendsFound].OldCombinedTriggerRect, ExtendStruct[ExtendsFound].CombinedTriggerRect);
					BOOL TargetSameSize = RectsAreTheSameSize(pNameGalleryItem->GetSetBounds(), pNameGalleryItem->GetOldSetBounds());

					// trigger changed in size & target hasn't => extend
					// trigger and target both unchanged in size => do nothing
					// otherwise reset the relationships
					BOOL DoExtend = (!TriggerSameSize || // either the trigger has changed size OR
						( pNameGalleryItem->m_BarNumber >= 0 && // is part of a bar
						GetBSTTriggeredBar(pNameGalleryItem->m_BarNumber)));// it is a triggered bar

					 // and it isn't a trans op on this button's bits
					DoExtend = DoExtend && !(IsPossiblePullApartOp && pNameGalleryItem->GetSelectedCount() > 0);

					// has the clean target changed size - should be the same size for an extend
					if (DoExtend && !TargetSameSize && pNameGalleryItem->GetSelectedCount() > 0 && ExtendStruct[ExtendsFound].pTriggerSet)
					{
						if (RectsAreTheSameSize(pNameGalleryItem->GetSetBounds(),
								SliceHelper::ScanForSetSizeExcluding(*(pNameGalleryItem->GetNameTextPtr()), *(ExtendStruct[ExtendsFound].pTriggerSet->GetNameTextPtr()))))
							DoExtend = FALSE;
					}

					BOOL DoReset = ((pNameGalleryItem->GetSetBounds() != pNameGalleryItem->GetOldSetBounds() ||
						ExtendStruct[ExtendsFound].OldCombinedTriggerRect != ExtendStruct[ExtendsFound].CombinedTriggerRect));

					// Matt - 12/02/2001
					// OK, here's my contribution to this super-huge function...
					// If you change the text alignment - it causes a translation without being flagged as a TransOp!
					// Therefore, we will be flagged as !DoExtend and DoReset which will give us the wrong effect...
					// It will not move any targets of the trigger, but will instead reset the relationships between them!
					if (!DoExtend && DoReset && (ObjChange.GetOpPointer())->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttribToSelected)))
					{
						DoExtend = TRUE;
						DoReset = FALSE;
					}

					if (pNameGalleryItem->m_BarNumber != -1 &&
						IsAChangePropertyOp &&
						((OpChangeBarProperty*)m_LastOpUsed)->m_MakingLive &&
						((OpChangeBarProperty*)m_LastOpUsed)->m_BarIndex == pNameGalleryItem->m_BarNumber)
					{
						// override extend, force a reset for this bar has just gone live
						DoExtend = FALSE;
						DoReset = TRUE;
						TRACEUSER( "GerryX", _T("Bar gone live\n"));
					}
					
					/*** DO AN EXTEND ***/
					if (DoExtend)
					{
#ifdef DEBUG
						String_256 debug = "";
						pNameGalleryItem->GetNameText(&debug);
						TRACEUSER( "GerryX", _T("%s : Extended\n"), (TCHAR*) (debug));
#endif
						// fill in the rest of the extend details
						// this is used in the later half of this function to actaully do the extending
						ExtendStruct[ExtendsFound].pTargetSet = pNameGalleryItem;
						ExtendStruct[ExtendsFound].pTargetStretchProp = pProp;
						ExtendStruct[ExtendsFound].CleanTargetRect.MakeEmpty();
						ExtendStruct[ExtendsFound].PerformedExtendOK = FALSE;
						ExtendStruct[ExtendsFound].TotalExtend.MakeEmpty();

						// ready the extends found to accept the next item and count this one
						if (ExtendsFound < MAX_SIM_EXTENDS-1)
							ExtendsFound++;

						// set the bar being triggered
						if (pNameGalleryItem->m_BarNumber != -1)
						{
							TriggeredBar = pNameGalleryItem->m_BarNumber;
							if (!SetBSTData(TriggeredBar, 0, 0, 1, 0))
							{
								ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
								return FALSE;
							}
						}

						// if this target is also a trigger then we may have recursion going on
						if (pNameGalleryItem->m_IsATrigger)
						{
							Recurse = TRUE;
						}

						pNameGalleryItem->m_IsPartOfThisStretch = TRUE;
					}
					// if either the trigger or the target have changed at all and it is not an extend
					// then it must be a reset
					else if (DoReset)
					{
						if (m_LastOpUsed && m_LastOpUsed->OpStatus == DO)
						{
							/*** DO A RESET ***/
#ifdef DEBUG
							String_256 debug = "";
							pNameGalleryItem->GetNameText(&debug);
							TRACEUSER( "GerryX", _T("%s : Reset\n"), (TCHAR*) (debug));
#endif

							if (pNameGalleryItem->m_BarNumber >= 0 && // is part of a bar
								!GetBSTTriggeredBar(pNameGalleryItem->m_BarNumber) /*m_BarSize[pNameGalleryItem->m_BarNumber].TriggeredBar*/ && // and that bar has had a trigger fire in it
								ExtendStruct[ExtendsFound].pTriggerSet && // and it has a trigger associated
								!pNameGalleryItem->IsABackBar()) // and it is not a back bar
							{
								if (pNodeBarProperty->Bar(pNameGalleryItem->m_BarNumber).SameSize < 3 
									&& ( ExtendStruct[ExtendsFound].pTriggerSet->GetSetBounds().Height() != 0 || ExtendStruct[ExtendsFound].pTriggerSet->GetSetBounds().Width() != 0))
								{
									// expand up the combined and old combined trigger rects
									ExpandVirtualTriggers(	pNodeBarProperty->Bar(pNameGalleryItem->m_BarNumber).SameSize,
															pNameGalleryItem->m_BarNumber,
															ExtendStruct[ExtendsFound].CombinedTriggerRect);
								}
							}

							ResetRelationshipRects(pNameGalleryItem,
													pProp,
													NULL,
													&(ExtendStruct[ExtendsFound].CombinedTriggerRect)
													);
						}
						else
						{
#ifdef DEBUG
							String_256 debug = "";
							pNameGalleryItem->GetNameText(&debug);
							TRACEUSER( "GerryX", _T("%s : Op Invalid for Reset\n"), (TCHAR*) (debug));
#endif
						}

						// set the bar being triggered
						if (pNameGalleryItem->m_BarNumber != -1)
						{
							TriggeredBar = pNameGalleryItem->m_BarNumber;
							if (!SetBSTData(TriggeredBar, 0, 0, 1, 0))
							{
								ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
								return FALSE;
							}
						}
					}
					else
					{
#ifdef DEBUG
						String_256 debug = "";
						pNameGalleryItem->GetNameText(&debug);
						TRACEUSER( "GerryX", _T("%s : No action\n"), (TCHAR*) (debug));
#endif
					}
				}
			}

			// look at the next posible target
			pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
		}

		// bar creation should make the bar shuffle as we know no extending is likely to go on
		if (TriggeredBar == -1)
		{
PORTNOTE("other", "Removed OpBarCreation");
#ifndef EXCLUDE_FROM_XARALX
			if (IS_A(m_LastOpUsed, OpBarCreation))
			{
				TriggeredBar = ((OpBarCreation *)m_LastOpUsed)->GetBarNumber();
			}
#endif
		}
		
		// no target found to extend, so give up now!
		if (ExtendsFound == 0 && !IsAButtonNoChangingOp)
		{
			if (m_TouchedBar != -1)
				TriggeredBar = m_TouchedBar;

			if (TriggeredBar != -1)
			{
				// get the size of each set
				// with reseting the old size of anything
				FastUpdateNamedSetSizes();

				if (!SetBSTData(TriggeredBar, 0, 0, 1, 0))
				{
					ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
					return FALSE;
				}

			for (INT32 i = 0; i < MAX_BARS; i++)
				{
					if (GetBSTTriggeredBar(i) == 1 && pNodeBarProperty->Bar(i).IsLive)
					{
						// but before we just leave does the bar need a shuffle?
						// shuffle the bar if it has been touched but no extends went on
					INT32 barDirection = pNodeBarProperty->Bar(i).IsHorizontal ? 1 : 2;
						if (!pNodeBarProperty->Bar(i).RequiresShuffle)
							barDirection = 0;

						ShuffleBar(	i,
									pNodeBarProperty->Bar(i).Spacing,
									barDirection,
									pNames,
									pSpread,
									ObjChange);

						// only recurse if the shuffled bar has a back bar
						if (GetBSTHasABackBar(i) == 1)
							Recurse = TRUE; // its backbar if it has one will need extending
					}
				}

				m_TouchedBar = -1;
				m_LastOpUsed->NoStretchUpdateChangedNodes(&ObjChange, Document::GetCurrent());
				// reseting a rel may cause an extend like affect when
				// infact it is just the above NoStretchUpdateChangedNodes that wiggled
				// recalcing the positions now removes this wiggle
//				if (m_bResetARelationship)
//					FastUpdateNamedSetSizes();

				// We will always update the set sizes here as otherwise there can be small
				// discrepancies between the current and old sizes at the end of the operation
				// This causes the next operation to go wrong (things get reset rather than 
				// extended)
				FastUpdateNamedSetSizes();
			}

			if (!Recurse)
				return FALSE; // no extends so go away!!!
		}

		/*** WE KNOW WHAT IS TO EXTEND SO GO DO IT ***/

		// scan to build list of all nodes that have affected targets

		Node * pTop = pSpread->FindFirstLayer()->FindParent(); // start point of the scan
		Node * pNode = NULL;	// general node in the scan
		Node * pTempNode = NULL; // a tempory use for a node
		Node * pParent = NULL; // the parent of pNode

		// list holds the nodes to extend
		List ExtendNodeList;

	INT32 i = 0;
		BOOL CleanTarget = TRUE;
		BOOL StretchAsSingularItem = FALSE;

		if (pTop)
		{
			// scan entire tree for the Template Attribs
			pNode = SliceHelper::FindNextNameNode(pTop, pTop);

			// check for all the name gallery items
			while (pNode)
			{
				pNameGalleryItem = (SGNameItem*) pNames->GetChild();

				// of course it is the parent of the attrib that is the node that needs extending
				pParent = pNode->FindParent();

				if (pParent == pNodeSetSentinel)
					break; // gone too far

				// add it to the list of nodes to extend if it is a target of a fired trigger
				if (pParent)
				{
					for (i = 0; i < ExtendsFound; i++)
					{
						if (ExtendStruct[i].pTargetSet->GetNameTextPtr()->
							CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
						{
							CleanTarget = TRUE;
							StretchAsSingularItem = FALSE;
							// exclude it if it is the trigger node aswell as a target
							pTempNode = pParent->FindFirstChild(CC_RUNTIME_CLASS(TemplateAttribute));
							while (pTempNode && CleanTarget)
							{
								if (ExtendStruct[i].pTriggerSet && ExtendStruct[i].pTriggerSet->GetNameTextPtr()->
									CompareTo(((TemplateAttribute *)pTempNode)->GetParam()) == 0)
								{
									CleanTarget = FALSE;
									// nodes that are not clean
									// ie the target contains the trigger too
									// need to use the data of the bounds before the trigger
									// changed the size of the traget by being part of it.
									// note of it is clean or not
								}

								pTempNode = pTempNode->FindNext(CC_RUNTIME_CLASS(TemplateAttribute));
							}

							// check it is not part of the bar we wish to ignore and is clean
							if (CleanTarget && (!m_BarToIgnoreTargetsOf[0] || m_BarToIgnoreTargetsOf.CompareTo(((TemplateAttribute *)pNode)->GetQuestion()) != 0))
							{
								if (!pParent->IsSelected() || !IsPossiblePullApartOp)
								{
									// if all is well add it to the list
									NodeListItemWithIndex * pItem = new NodeListItemWithIndex(pParent, i);
									ExtendNodeList.AddTail(pItem);
									ExtendStruct[i].PerformedExtendOK = TRUE;
								}
								// keep record of the clean target rect as we will need this and the usual
								// scan will not seperate the clean and dirty parts of it for us
								ExtendStruct[i].CleanTargetRect = ExtendStruct[i].CleanTargetRect.Union(SliceHelper::BoundingNodeSize(pParent));
							}
						}
					}
				}

				pNode = SliceHelper::FindNextNameNode(pNode, pTop);
			}
		}

		// allow op and allow extend them all?
		ExtendOk = TRUE; // did it all extend ok?
//		DocRect TotalExtend; // total bounds for the extender so the button all extends the same amount
		DocRect rDiff; // temp variable

//		TotalExtend.MakeEmpty(); 
		rDiff.MakeEmpty();

		NodeListItemWithIndex * pNodeListItem = (NodeListItemWithIndex *)ExtendNodeList.GetHead();

		// loop around the list of nodes that are extending
		while (pNodeListItem && ExtendOk)
		{
			if (pNodeBarProperty->Bar(ExtendStruct[pNodeListItem->Index].pTargetSet->m_BarNumber).GroupsStretch || !IS_A(pNodeListItem->pNode, NodeGroup))
			{
				// check if we are allowed to extend
				rDiff =	Extender::CheckValidExtend((NodeRenderableInk*) (pNodeListItem->pNode),
												ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetStretchType(), 
												ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetRefUnionTriggerBounds(),
												ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetRefTargetBounds(),
												ExtendStruct[pNodeListItem->Index].CombinedTriggerRect,
												ExtendStruct[pNodeListItem->Index].OldCombinedTriggerRect,
												ExtendStruct[pNodeListItem->Index].CleanTargetRect,
												&ExtendOk,
												ExtendStruct[pNodeListItem->Index].pTargetSet->m_BarNumber >= 0);

				if (!ExtendOk)
					break;

				if (ExtendStruct[pNodeListItem->Index].TotalExtend.IsEmpty())
					ExtendStruct[pNodeListItem->Index].TotalExtend=rDiff;

				// Minimise the "difference" for any stretches that cannot be fully performed.
				if (ExtendStruct[pNodeListItem->Index].TotalExtend.lo.x > rDiff.lo.x) ExtendStruct[pNodeListItem->Index].TotalExtend.lo.x = rDiff.lo.x;
				if (ExtendStruct[pNodeListItem->Index].TotalExtend.lo.y > rDiff.lo.y) ExtendStruct[pNodeListItem->Index].TotalExtend.lo.y = rDiff.lo.y;
				if (ExtendStruct[pNodeListItem->Index].TotalExtend.hi.x > rDiff.hi.x) ExtendStruct[pNodeListItem->Index].TotalExtend.hi.x = rDiff.hi.x;
				if (ExtendStruct[pNodeListItem->Index].TotalExtend.hi.y > rDiff.hi.y) ExtendStruct[pNodeListItem->Index].TotalExtend.hi.y = rDiff.hi.y;

				// Karim 11/01/2000
				// Ok, this is a fix for a yukky bug.
				// Bug:	most of the time, shrinks are symmetric. However, sometimes they're not, in
				//		which case it is possible for an object to swap sides in the target rect.
				//		This is bad, as extension behaviour is side-dependent.
				// Fix:	if an asymmetric shrink is about to happen, force it to be symmetric.
				//		we do this by ensuring that TotalExtend.lo.x and .hi.y are both set
				//		to be the lesser of the two values. Same for top and bottom.
				if (ExtendStruct[pNodeListItem->Index].TotalExtend.lo.x < ExtendStruct[pNodeListItem->Index].TotalExtend.hi.x)
					ExtendStruct[pNodeListItem->Index].TotalExtend.hi.x = ExtendStruct[pNodeListItem->Index].TotalExtend.lo.x;
				else
					ExtendStruct[pNodeListItem->Index].TotalExtend.lo.x = ExtendStruct[pNodeListItem->Index].TotalExtend.hi.x;
					
				if (ExtendStruct[pNodeListItem->Index].TotalExtend.lo.y < ExtendStruct[pNodeListItem->Index].TotalExtend.hi.y)
					ExtendStruct[pNodeListItem->Index].TotalExtend.hi.y = ExtendStruct[pNodeListItem->Index].TotalExtend.lo.y;
				else
					ExtendStruct[pNodeListItem->Index].TotalExtend.lo.y = ExtendStruct[pNodeListItem->Index].TotalExtend.hi.y;
					
				// check allow op
				ExtendOk = pNodeListItem->pNode->AllowOp(&ObjChange);
			}

			pNodeListItem = (NodeListItemWithIndex *)ExtendNodeList.GetNext(pNodeListItem);
		}

		// if ok call extend on them all and remove them from the list
		pNodeListItem = (NodeListItemWithIndex *)ExtendNodeList.GetHead();
		NodeListItemWithIndex * pNodeListItemToDel = NULL;

		while (pNodeListItem)
		{
			if (ExtendOk && (pNodeBarProperty->Bar(ExtendStruct[pNodeListItem->Index].pTargetSet->m_BarNumber).GroupsStretch || !IS_A(pNodeListItem->pNode, NodeGroup)))
			{
				TRACEUSER( "GerryX", _T("Extending %s at 0x%08x\n"), pNodeListItem->pNode->GetRuntimeClass()->m_lpszClassName, pNodeListItem->pNode);

				// do the extend
				Extender::Extend((NodeRenderableInk*) (pNodeListItem->pNode),
					ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetStretchType(), 
					ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetRefUnionTriggerBounds(),
					ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetRefTargetBounds(),
					ExtendStruct[pNodeListItem->Index].CombinedTriggerRect,
					ExtendStruct[pNodeListItem->Index].OldCombinedTriggerRect,
					ExtendStruct[pNodeListItem->Index].CleanTargetRect,
					&(ExtendStruct[pNodeListItem->Index].TotalExtend),
					ExtendStruct[pNodeListItem->Index].pTargetSet->m_BarNumber >= 0,
					m_LastOpUsed
					);
			}
			else if (ExtendOk) // move items that do not stretch but are part of the button
			{
				TRACEUSER( "GerryX", _T("Moving %s at 0x%08x\n"), pNodeListItem->pNode->GetRuntimeClass()->m_lpszClassName, pNodeListItem->pNode);
				ExtendParams EPS;
				EPS.pOp = m_LastOpUsed;
				Extender::CalculateExtendParams(&EPS,
					ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetStretchType(), 
					ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetRefUnionTriggerBounds(),
					ExtendStruct[pNodeListItem->Index].pTargetStretchProp->GetRefTargetBounds(),
					ExtendStruct[pNodeListItem->Index].CombinedTriggerRect,
					ExtendStruct[pNodeListItem->Index].OldCombinedTriggerRect,
					ExtendStruct[pNodeListItem->Index].CleanTargetRect,
					&(ExtendStruct[pNodeListItem->Index].TotalExtend),
					ExtendStruct[pNodeListItem->Index].pTargetSet->m_BarNumber >= 0
					);

				if (m_LastOpUsed)
					m_LastOpUsed->DoInvalidateNodeRegion((NodeRenderableBounded*) pNode, TRUE, FALSE);
				((NodeRenderable*) (pNodeListItem->pNode))->NodeRenderable::TransformTranslateObject(EPS);
				((NodeRenderable*) (pNodeListItem->pNode))->NodeRenderable::TransformStretchObject(EPS);
				if (m_LastOpUsed)
					m_LastOpUsed->DoInvalidateNodeRegion((NodeRenderableBounded*) pNode, TRUE, FALSE);
			}

			// delete the item from the list and tidy the list
			pNodeListItemToDel = pNodeListItem;
			pNodeListItem = (NodeListItemWithIndex*)ExtendNodeList.GetNext(pNodeListItem);

			if (pNodeListItemToDel)
			{
				ExtendNodeList.RemoveItem((NodeListItemWithIndex*)pNodeListItemToDel);
				delete pNodeListItemToDel;
			}
		}

		// shuffle the bar
		if (TriggeredBar >= 0 && TimesInLoop == 1 && pNodeBarProperty->Bar(TriggeredBar).RequiresShuffle)
		{
			// get the size of each set
			// with reseting the old size of anything
			FastUpdateNamedSetSizes();

			if (!SetBSTData(TriggeredBar, 0, 0, 1, 0))
			{
				ERROR3("Couldn't Set Triggered Bar -> Index may be out of range!");
				return FALSE;
			}

		for (i = 0; i < MAX_BARS; i++)
			{
				if (GetBSTTriggeredBar(i) == 1 && pNodeBarProperty->Bar(i).IsLive)
					// but before we just leave does the bar need a shuffle?
					// shuffle the bar if it has been touched but no extends went on
				{
					INT32 barDirection = pNodeBarProperty->Bar(i).IsHorizontal ? 1 : 2;
					if (!pNodeBarProperty->Bar(i).RequiresShuffle)
						barDirection = 0;

					ShuffleBar(	i,
								pNodeBarProperty->Bar(i).Spacing,
								barDirection,
								pNames,
								pSpread,
								ObjChange);

					// only recurse if the shuffled bar has a back bar
					if (GetBSTHasABackBar(i) == 1)
						Recurse = TRUE; // its backbar if it has one will need extending
				}
			}
		}

		// remove any trace that there was a touched bar, so it doesn't cause recursion etc
		m_TouchedBar = -1;

		if (Recurse && (ExtendOk || IsAButtonNoChangingOp) && TimesInLoop < 2)
			TRACEUSER( "GerryX", _T("Looping\n"));

	} while(Recurse && (ExtendOk || IsAButtonNoChangingOp) && TimesInLoop < 2); // recursive loop for A extends B extends C

	TRACEUSER( "GerryX", _T("After Loop\n"));

	// tidy up after a delete or a shorten bar op has occured to remove some of the properties
	// from bars that still exist
	if (m_LastOpUsed->OpStatus == DO && (
PORTNOTE("other", "Removed OpShortenBar")
#ifndef EXCLUDE_FROM_XARALX											
											IS_A(m_LastOpUsed, OpShortenBar) ||
#endif
											IS_A(m_LastOpUsed, OpDelete)))
	{
		pNameGalleryItem = (SGNameItem*) pNames->GetChild();
		while (pNameGalleryItem)
		{
			// purge the use of this named set if it has just been deleted somehow
			if (pNameGalleryItem->IsEmpty())
			{
				String_256 TempSetName = "";
				pNameGalleryItem->GetNameText(&TempSetName);

				SliceHelper::PurgeUseOfSetName(TempSetName, m_LastOpUsed);
				// remove all properties of the deleted node
				Node * pNode = pNameGalleryItem->GetPropertyNode();
				if (pNode)
					m_LastOpUsed->DoHideNode(pNode, FALSE);

				// deleted bar members should be got rid of completely
				// remove the attribs from the sentinel
				pNode = Document::GetSelected()->GetSetSentinel()->GetNameAttr(TempSetName);
				if (pNode)
					m_LastOpUsed->DoHideNode(pNode, FALSE);
			}

			pNameGalleryItem = (SGNameItem*)pNameGalleryItem->GetNext();
		}
	}

	m_LastOpUsed->NoStretchUpdateChangedNodes(&ObjChange, Document::GetCurrent());

	// reseting a rel may cause an extend like affect when
	// infact it is just the above NoStretchUpdateChangedNodes that wiggled
	// recalcing the positions now removes this wiggle
//	if (m_bResetARelationship)
//	{
//		FastUpdateNamedSetSizes();
//		TRACEUSER( "GerryX", _T("Done UpdateSetSizes after reset\n"));
//	}

	// We will always update the set sizes here as otherwise there can be small
	// discrepancies between the current and old sizes at the end of the operation
	// This causes the next operation to go wrong (things get reset rather than 
	// extended)
	FastUpdateNamedSetSizes();

	m_TouchedBar = -1;

	TRACEUSER( "GerryX", _T("FastApplyStretchScan returning %s\n"), ExtendOk ? _T("TRUE") : _T("FALSE") );

	return ExtendOk;
}


/********************************************************************************************
>	void NameGallery::ResetRelationshipRects(SGNameItem * pTarget, NamedStretchProp * pProp, DocRect * pCleanTargetRect, DocRect * pKnownTriggerRect)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/00
	Purpose:	Does what it says on the tin. It takes ptrs to the target set and the properties
				of this target set and updates the relationship rectangles to be exactly as they
				are currently.

********************************************************************************************/
void NameGallery::ResetRelationshipRects(SGNameItem * pTarget, NamedStretchProp * pProp, DocRect * pCleanTargetRect, DocRect * pKnownTriggerRect)
{
	String_256 debug = "";
	pTarget->GetNameText(&debug);
	TRACEUSER( "GerryX", _T("ResetRelationshipRects %s\n"), (TCHAR*) (debug));

	if (pTarget->GetSetBounds().Height() != 0 && pTarget->GetSetBounds().Width() != 0
		&& m_LastOpUsed && m_LastOpUsed->OpStatus == DO)
	{
		DocRect TempCombinedTriggerRect;
		DocRect TempCleanTargetRect;

		// what is the target bounds - use the clean bounds
		if (pCleanTargetRect && pCleanTargetRect->Height() > 0 && pCleanTargetRect->Width() > 0)
			TempCleanTargetRect = *pCleanTargetRect;
		else
		{
			/*** This appears to be working without the more expensive call to ScanForSetSizeExcluding()
				 but will have to test to make sure we can always get away without it (sjk) ***/
			// use the first extender as the thing to clean it from (as this works for most buttons)
			if (pProp && !pProp->GetTriggers().empty())
				TempCleanTargetRect = SliceHelper::ScanForSetSizeExcluding(*pTarget->GetNameTextPtr(), pProp->GetTriggers().front().m_strSet);
			else
				TempCleanTargetRect = pTarget->GetSetBounds();
		}

		// either use the known trigger rects or calculate them here
		if (pKnownTriggerRect)
			TempCombinedTriggerRect = *pKnownTriggerRect;
		else
		{
			TempCombinedTriggerRect.MakeEmpty();
			SGNameItem * pTempNameItem = NULL;

			// loop around each trigger of this target
			for (std::list<TriggerSet>::iterator pt = pProp->GetTriggers().begin();
				pt != pProp->GetTriggers().end();
				pt++)
			{
				pTempNameItem = SliceHelper::LookupNameGalleryItem(pt->m_strSet);
				// union the trigger bounds together
				if (pTempNameItem)
					TempCombinedTriggerRect = TempCombinedTriggerRect.Union(pTempNameItem->GetSetBounds());
			}
		}

		BOOL ok = TempCombinedTriggerRect.IsValid();

		// if it is ok and this will make a difference
		if (ok 
			&& (TempCleanTargetRect != pProp->GetRefTargetBounds() || TempCombinedTriggerRect != pProp->GetRefUnionTriggerBounds()) 
			)
		{
#ifdef DEBUG
			String_256 debug = "";
			pTarget->GetNameText(&debug);
			TRACEUSER( "GerryX", _T("Reset %s\n"), (TCHAR*) (debug));
#endif
			// copy the old bar data into the new bar data as the new is a copy of the old
			NodeSetProperty* pCopy;
			ALLOC_WITH_FAIL(pCopy, ((NodeSetProperty*) pTarget->GetPropertyNode()->SimpleCopy()), m_LastOpUsed);
			if (pCopy)
			{
				// change the data in the tree to do this undoably
				NamedStretchProp* pNewProp = (NamedStretchProp*) pCopy->GetProperty(NamedStretchProp::nIndex);
				pNewProp->SetRefTargetBounds(TempCleanTargetRect);
				pNewProp->SetRefUnionTriggerBounds(TempCombinedTriggerRect);

				Node * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel(); // the sentinel
				pCopy->AttachNode(pNodeSetSentinel, LASTCHILD);
				// Create a hide node action to hide the node when we undo 
				HideNodeAction* UndoHideNodeAction;
				HideNodeAction::Init(m_LastOpUsed,                    
					  				 m_LastOpUsed->GetUndoActions(), //&UndoActions,
									 pCopy, 
									 FALSE, 		 // Include subtree size 
					  				 ( Action**)(&UndoHideNodeAction));
				
				m_LastOpUsed->DoHideNode(pTarget->GetPropertyNode(), FALSE);
				pTarget->ResetCachedPropertyNode();

				m_bResetARelationship = TRUE;
			}
		}
	}
}



/********************************************************************************************
>	void NameGallery::ShuffleBar(INT32 BarNumber, UINT32 Spacing, INT32 BarDirection,
								 SGUsedNames* pNames, Spread* pSpread)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/00
	Purpose:	Moves the buttons of a bar about according to the principles of the bar
				such as the spacing and the bar direction
	SeeAlso:	NameGallery::FastUpdateNamedSetSizes
********************************************************************************************/
void NameGallery::ShuffleBar(INT32 BarNumber, INT32 Spacing, INT32 BarDirection,
							 SGUsedNames* pNames, Spread* pSpread, ObjChangeParam& ObjChange)
{
	TRACEUSER( "GerryX", _T("ShuffleBar(%d)\n"), BarNumber);

	DocCoord NextPos(0,0);
	DocCoord FirstButtonPos(0,0);
	DocCoord FirstButtonSubPixPos(0,0);
	DocRect	rBounds;
	
	SGNameItem * Order[MAX_BUTTONS_IN_A_BAR];
	INT32 num = 0;

	if (BarDirection != 1 && BarDirection != 2)
		return;

	// reset the translations and fill in the Order array of items to be shuffled
	SGNameItem* pNameGalleryItem = (SGNameItem*) pNames->GetChild();
	while (pNameGalleryItem)
	{
		if (pNameGalleryItem->m_BarNumber == BarNumber)
		{
			// default is to not move the shape
			pNameGalleryItem->m_Translation.x = 0;
			pNameGalleryItem->m_Translation.y = 0;

			// ignore the back bar in these calcs
			if (!pNameGalleryItem->IsABackBar())
			{
				Order[num] = pNameGalleryItem;
				num++;
			}
		}

		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}

	
	// sort them by location
	INT32 i;
	INT32 j;
	if (BarDirection == 1) // horiz
		for (i = 0; i < num; i++)
		{
			for (j = i+1; j < num; j++)
			{
				if ((Order[j]->GetSetBounds().lo.x < Order[i]->GetSetBounds().lo.x)
					|| (Order[j]->GetSetBounds().lo.x == Order[i]->GetSetBounds().lo.x && Order[j]->GetSetBounds().hi.y > Order[i]->GetSetBounds().hi.y) )
				{
					pNameGalleryItem = Order[j];
					Order[j] = Order[i];
					Order[i] = pNameGalleryItem;
				}
			}
		}
	else 
	if (BarDirection == 2) // vert
		for (i = 0; i < num; i++)
		{
			for (j = i+1; j < num; j++)
			{
				if ((Order[j]->GetSetBounds().hi.y > Order[i]->GetSetBounds().hi.y)
					|| (Order[j]->GetSetBounds().hi.y == Order[i]->GetSetBounds().hi.y && Order[j]->GetSetBounds().lo.x < Order[i]->GetSetBounds().lo.x) )
				{
					pNameGalleryItem = Order[j];
					Order[j] = Order[i];
					Order[i] = pNameGalleryItem;
				}
			}
		}

	/* this loop works out what translations need to be performed on each button in the bar */
	for (i = 0; i < num; i++)
	{
		rBounds = Order[i]->GetSetBounds();

		if (i == 0)
		{
			// staple the horz and vertical bars at the top left corner
			// but let user positioned bars and manually positioned buttons expand around the text
			if (BarDirection == 1 || BarDirection == 2)
			{
				Order[i]->m_Translation.x = Order[i]->GetOldSetBounds().lo.x - rBounds.lo.x;
				Order[i]->m_Translation.y = Order[i]->GetOldSetBounds().hi.y - rBounds.hi.y;
			}
			FirstButtonPos.x = rBounds.lo.x + Order[i]->m_Translation.x;
			FirstButtonPos.y = rBounds.hi.y + Order[i]->m_Translation.y;
			FirstButtonSubPixPos.x = - (FirstButtonPos.x % 750);
			FirstButtonSubPixPos.y = FirstButtonPos.y % 750;
		}
		else
		{	// move button from bar relative to last position
			// storing the translation in the name gallery item
			Order[i]->m_Translation.x = NextPos.x - rBounds.lo.x;
			Order[i]->m_Translation.y = NextPos.y - rBounds.hi.y;

			if (BarDirection == 1)
				Order[i]->m_Translation.x += Spacing;
			else if (BarDirection == 2)
				Order[i]->m_Translation.y -= Spacing;

		}

		switch (BarDirection)
		{
		case 1: // horizontal
			if (Spacing == 0 || Spacing >= 3750)
				NextPos.x = ((rBounds.hi.y + Order[i]->m_Translation.x)/750)*750 + FirstButtonSubPixPos.x;
			else
				NextPos.x = rBounds.hi.y + Order[i]->m_Translation.x;
			NextPos.y = FirstButtonPos.y;
			break;

		case 2: // vertical
			NextPos.x = FirstButtonPos.x;
			if (Spacing == 0 || Spacing >= 3750)
				NextPos.y = ((rBounds.lo.y + Order[i]->m_Translation.y)/750 +1)*750 + FirstButtonSubPixPos.y;
			else
				NextPos.y = rBounds.lo.y + Order[i]->m_Translation.y;
			break;
		}

		TRACEUSER( "GerryX", _T("Moved Bar %d, %d\n"), Order[i]->m_Translation.x, Order[i]->m_Translation.y);
	}


	/* *** This next loop scans the tree translating buttons in the bar *** */

	Node * pTop = pSpread->FindFirstLayer()->FindParent(); // start point of the scan
	Node * pNode = NULL;	// general node in the scan
	Node * pParent = NULL; // the parent of pNode
	Node * pNodeSetSentinel = Document::GetSelected()->GetSetSentinel(); // the sentinel - basically avoid it!

	BOOL UpdateSelection = FALSE;

	Trans2DMatrix Transformer;

	if (pTop)
	{
		// scan entire tree for the Template Attribs
		pNode = SliceHelper::FindNextNameNode(pTop, pTop);

		// check for all the name gallery items
		while (pNode)
		{
			pNameGalleryItem = SliceHelper::LookupNameGalleryItem(((TemplateAttribute *)pNode)->GetParam());

			if (pNameGalleryItem && pNameGalleryItem->m_BarNumber == BarNumber && 
				(pNameGalleryItem->m_Translation.x || pNameGalleryItem->m_Translation.y))
			{
				// of course it is the parent of the attrib that is the node that needs extending
				pParent = pNode->FindParent();

				if (pParent == pNodeSetSentinel)
					break;

				if (pParent)
				{
					Transformer.SetTransform(pNameGalleryItem->m_Translation.x, pNameGalleryItem->m_Translation.y);

					if (m_LastOpUsed) m_LastOpUsed->DoInvalidateNodeRegion((NodeRenderableBounded*) pParent, TRUE, FALSE);
					if (pParent->AllowOp(&ObjChange))
					{
						((NodeRenderableBounded *)pParent)->Transform(Transformer);
						if (pParent->IsSelected())
							UpdateSelection = TRUE;
					}
					if (m_LastOpUsed) m_LastOpUsed->DoInvalidateNodeRegion((NodeRenderableBounded*) pParent, TRUE, FALSE);
				}
			}

			pNode = SliceHelper::FindNextNameNode(pNode, pTop);
		}
	}

	if (UpdateSelection)
	{
		// the selection will have moved - make sure the blobs are up to date
		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::NONCOLOURATTCHANGED));
		GetApplication()->UpdateSelection();
	}
}



/********************************************************************************************
>	BOOL NameGallery::ExpandVirtualTriggers(INT32 ExpandType, INT32 BarNo, DocRect &r1)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/00
	Purpose:	Expands up the trigger defined by r1 according to the virtual trigger size
				defined in m_BarSize[] and the Expand type of the bar (Equal size, Equal but left aligned, etc.)
	SeeAlso:
********************************************************************************************/
BOOL NameGallery::ExpandVirtualTriggers(INT32 ExpandType, INT32 BarNo, DocRect &r1)
{
	DWORD bstMaxHeight = GetBSTMaxHeight(BarNo);
	DWORD bstMaxWidth  = GetBSTMaxWidth(BarNo);
	if (!bstMaxHeight || !bstMaxWidth)
	{
		ERROR3("Problems getting the max height and width -> Index may be out of range");
		return FALSE;
	}

	INT32 ysize = max( ( bstMaxHeight/*m_BarSize[BarNo].MaxHeight*/ - r1.Height() ) / 2, unsigned(0) ); // not a typo ysize is held as half of xsize
	INT32 xsize = max( ( bstMaxWidth/*m_BarSize[BarNo].MaxWidth*/ - r1.Width() ), unsigned(0) );

	switch (ExpandType)
	{
	case 1: //left align
		r1.Inflate(0, ysize);
		r1.hi.y += xsize;
		return TRUE;

	case 2: // right align
		r1.Inflate(0, ysize);
		r1.lo.x -= xsize;
		return TRUE;

	case 0: // centre
		r1.Inflate(xsize/2, ysize);
		return TRUE;
	}

	return FALSE;
}


/***************************************************************************************

>	BOOL NameGallery::SetBSTData(INT32 Index, DWORD MaxWidth, DWORD MaxHeight, BYTE TriggeredBar, BYTE HasABackBar)

	Author		: Matt Priestley
	Created		: 06 February 2001
	Purpose		: 

	Returns		: BOOL
	Argument	: INT32 Index
	Argument	: DWORD MaxWidth
	Argument	: DWORD MaxHeight
	Argument	: BYTE TriggeredBar
	Argument	: BYTE HasABackBar

***************************************************************************************/

BOOL NameGallery::SetBSTData(INT32 Index, DWORD MaxWidth = NULL, DWORD MaxHeight = NULL, BYTE TriggeredBar = NULL, BYTE HasABackBar = NULL)
{
	if (Index > MAX_BARS || (!MaxWidth && !MaxHeight && !TriggeredBar && !HasABackBar))
	{
		ERROR3("Problem calling SetBSTData()");
		return FALSE;
	}

	if (MaxWidth)		{	m_BarSize[Index].MaxWidth		= MaxWidth;		}
	if (MaxHeight)		{	m_BarSize[Index].MaxHeight		= MaxHeight;	}
	if (TriggeredBar)	{	m_BarSize[Index].TriggeredBar	= TriggeredBar;	}
	if (HasABackBar)	{	m_BarSize[Index].HasABackBar	= HasABackBar;	}

	return TRUE;
}
