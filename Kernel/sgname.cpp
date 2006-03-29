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
// sgname.cpp - the NameGallery implementation.

/*
	$Log: /Camelot/kernel/sgname.cpp $
 * 
 * 2     11/02/04 10:58a Alex
 * Support for HTML Help
 * 
 * 2     16/07/99 18:06 Justinf
 * Now shows little circular blobs for adding sets to / subtracting sets
 * from the selection.
 * 
 * 1     24/06/99 16:23 Justinf
 * Name Gallery
*/

#include "camtypes.h"

#include "app.h"
#include "ink.h"
#include "attrmgr.h"
#include "ccdc.h"
#include "fillval.h"
#include "grnddib.h"

#include "cxfrech.h"
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"

#include "grndbmp.h"
#include "wbitmap.h"
#include "dragmgr.h"
#include "scrvw.h"
#include "docview.h"

//#include "resource.h"
//#include "galstr.h"
//#include "galres.h"
#include "sgmenu.h"
//#include "barsdlgs.h"
//#include "mario.h"
//#include "justin3.h"
//#include "richard2.h"

//#include "xshelpid.h"
#include "helpuser.h"

#include "sginit.h"
#include "sgname.h"

DECLARE_SOURCE("$Revision: 662 $");

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(NameGallery, SuperGallery)
CC_IMPLEMENT_DYNAMIC(SGNameItem, SGDisplayItem)
CC_IMPLEMENT_DYNCREATE(OpDisplayNameGallery, Operation);
CC_IMPLEMENT_DYNAMIC(GalleryNameDragInfo, BitmapDragInformation);
CC_IMPLEMENT_DYNAMIC(SGNameDragTarget, SGListDragTarget);
CC_IMPLEMENT_DYNCREATE(NameObjectsDlg, DialogOp)   

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW



/***********************************************************************************************
>	SGNameItem::SGNameItem(const String_256& strName)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		NameItemToDisplay - The NameItem this item will display
	Purpose:	SGNameItem constructor
***********************************************************************************************/

SGNameItem::SGNameItem(const String_256& strName)
  :	m_strSetName(strName),
	m_eIntersect(AttributeSets::Intersect::NONE)
{
//	TRACEUSER( "JustinF", _T("SGNameItem %s\n"), (LPCTSTR) m_strSetName);
}



/***********************************************************************************************
>	virtual void SGNameItem::CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct
	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this NameItem item's display area. This is dependent upon the current display
				mode and format state. FormatInfo will be updated as a result of the formatting
				operation.
	Purpose:	Shared code for NameItem items to calculate where they will appear in the
				grand scheme of things
	Notes:		NameItems supply only one display mode ("full info")
***********************************************************************************************/

void SGNameItem::CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo)
{
	CalculateFormatRect(pFormatInfo, pMiscInfo,	SG_InfiniteWidth, SG_DefaultSmallIcon);
}



/********************************************************************************************
>	virtual void SGNameItem::GetNameText(String_256* pResult)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95 (base generated in sgbase.cpp)
	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the name text associated with this item (if any)
	Purpose:	To determine a name string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose names match
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.
	Notes:		**** TO DO ****
				Modify this method to return the correct text. You may also want to
				add an override for the GetFullInfoText() method if you can provide
				a full-info display mode.
	SeeAlso:	SGDisplayNode::GetNameText
********************************************************************************************/

void SGNameItem::GetNameText(String_256* pResult)
{
	ERROR3IF(pResult == 0, "SGNameItem::GetNameText: illegal null param");
	*pResult = m_strSetName;
}



/********************************************************************************************
>	virtual BOOL SGNameItem::GetBubbleHelp(DocCoord *pMousePos, String_256 *pResult)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		pMousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.
	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a bubble help string for this item
	Returns:	TRUE if it filled in the string, FALSE if it did not
	Purpose:	Called by the parent gallery when bubble help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.
	SeeAlso:	SGDisplayNode::GetStatusLineHelp
********************************************************************************************/

BOOL SGNameItem::GetBubbleHelp(DocCoord*, String_256* pResult)
{
	ERROR3IF(pResult == 0, "SGNameItem::GetBubbleHelp: invalid null params");

	// Work out the intersection with the user selection.
	AttributeSets::Intersect eState =
			GetDocument()->GetAttributeSets()->IntersectSelectedObj(m_strSetName);

	// Create the bubble help text.
	String_256 strName;
	GetNameText(&strName);
	return pResult->MakeMsg(_R(IDBBL_NAMEGAL_ITEM),
							&String_32(_R(IDBBL_NAMEGAL_NONE) + INT32(eState)), &strName);
}


	
/********************************************************************************************
>	virtual BOOL SGNameItem::GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Inputs:		pMousePos - The current mouse position. This will generally be expected
				to lie inside this item's FormatRect. With it, this item can provide
				help on specific areas of an item.
	Outputs:	On exit, if the return value is TRUE, the string pointed at by Result
				will contain a status line help string for this item
	Returns:	TRUE if it filled in the string, FALSE if it did not			
	Purpose:	Called by the parent gallery when status line help is needed. The parent
				gallery will do a hit test to determine which node contains the pointer,
				and will then ask that node to supply bubble/status-line help.			
	Notes:		The base class returns FALSE (i.e. provides no help)
				If you can provide help, then override the base class method to do so.
	SeeAlso:	SGDisplayNode::GetBubbleHelp
********************************************************************************************/

BOOL SGNameItem::GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult)
{
	// Work out the intersection with the user selection.
	AttributeSets::Intersect eState =
			GetDocument()->GetAttributeSets()->IntersectSelectedObj(m_strSetName);

	// Create the bubble help text.
	String_256 strName;
	GetNameText(&strName);
	return pResult->MakeMsg(_R(IDS_NAMEGAL_ITEM_STATUS),
							&String_32(_R(IDBBL_NAMEGAL_NONE) + INT32(eState)),
							&strName);
}



/***********************************************************************************************
>	virtual void SGNameItem::HandleRedraw(SGRedrawInfo* pRedrawInfo,
										  SGFormatInfo* pFormatInfo)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure
				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item
	Purpose:	SGNameItem item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.
***********************************************************************************************/

void SGNameItem::HandleRedraw(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo)
{
	// Intersect the item's set with the user selection.
	String_256 strName;
	GetNameText(&strName);

	// Set the foreground and background colours according to whether this item is
	// gallery selected or not.
	RenderRegion* pRenderer = pRedrawInfo->Renderer;
	DocColour dcolForegnd, dcolBackgnd;
	if (Flags.Selected)
	{
		dcolForegnd = pRedrawInfo->SelForeground;
		dcolBackgnd = pRedrawInfo->SelBackground;
	}
	else
	{
		// Unselected items are in the 'selected' colour if the user selection includes
		// any of their members.
		dcolBackgnd = pRedrawInfo->Background;
		dcolForegnd = pRedrawInfo->Foreground;
	}

	// Work out the bounds of the bitmap and the label.
	DocRect drBmp(FormatRect), drTxt(FormatRect);
	drBmp.hi.x = drBmp.lo.x + SG_DefaultSmallIcon;
	drTxt.lo.x = drBmp.hi.x + SG_GapBeforeText;
	GridLockRect(pMiscInfo, &drBmp);
	GridLockRect(pMiscInfo, &drTxt);

	// Render the background.
	pRenderer->SetLineWidth(0);
	pRenderer->SetLineColour(pRedrawInfo->Transparent);
	pRenderer->SetFillColour(pRedrawInfo->Background);
	pRenderer->DrawRect(&drBmp);
	pRenderer->SetFillColour(dcolBackgnd);
	pRenderer->DrawRect(&drTxt);

	// Render the foreground.
	pRenderer->SetFixedSystemTextColours(&dcolForegnd, &dcolBackgnd);
	pRenderer->DrawFixedSystemText(&strName, drTxt);				

	// Work out which bitmap to render beside it.
	UINT32 idBmp;
	m_eIntersect = GetDocument()->GetAttributeSets()->IntersectSelectedObj(strName);
	switch (m_eIntersect)
	{
	case AttributeSets::Intersect::NONE:
		idBmp = _R(IDB_NAMEGAL_NONESEL);
		break;

	case AttributeSets::Intersect::SOME:
		idBmp = _R(IDB_NAMEGAL_SOMESEL);
		break;

	case AttributeSets::Intersect::ALL:
		idBmp = _R(IDB_NAMEGAL_ALLSEL);
		break;
	}

	// Render the bitmap.
	DrawBitmap(pRenderer, &drBmp, idBmp);
}



/***********************************************************************************************
>	virtual BOOL SGNameItem::HandleEvent(SGEventType nEventType, void* pEventInfo,
										 SGMiscInfo* pMiscInfo)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		nEventType  - An enumerated value describing what type of event is to be processed
				pEventInfo - A structure describing the event (may be 0). The exact thing
							 pointed at by this pointer depends upon the event type:
					
					SGEVENT_FORMAT		0
					SGEVENT_REDRAW		(SGRedrawInfo*)
					SGEVENT_BGREDRAW	0
					SGEVENT_BGFLUSH		0					- May have 0 MiscInfo
					SGEVENT_MOUSECLICK	(SGMouseInfo*)
					SGEVENT_DRAGSTARTED	(DragMessage*)
					SGEVENT_CLAIMPOINT	(SGMouseInfo*)
					SGEVENT_THUMBMSG	(ThumbMessage*)		- May have 0 MiscInfo

				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				pMiscInfo - almost always provided. Contains a few useful bits of info that may be
				needed for all event types. This may be 0 for special event types (see sgtree.h
				for the enum and information on which ones may pass 0 MiscInfo - currently
				this is _THUMBMSG and _BGFLUSH, neither of which should concern you - so as long
				as you only reference MiscInfo once you know the event type, you will be safe)

	Outputs:	FormatInfo is updated as appropriate
	Returns:	TRUE if the event was handled successfully
				FALSE if it was not
	Purpose:	Handles a SuperGallery DisplayTree event
	Notes:		VERY IMPORTANT: The rendering must be enclosed by calls to StartRendering
				and StopRendering, to ensure that rendering works properly (in the future
				we may change the redraw system to use a GRenderRegion for each individual
				item, rather than one global one for the window, for which these calls will
				be essential)
	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayItem::HandleEvent
***********************************************************************************************/

BOOL SGNameItem::HandleEvent(SGEventType nEventType, void* pEventInfo, SGMiscInfo* pMiscInfo)
{
	switch (nEventType)
	{
		case SGEVENT_FORMAT:
		{
			SGFormatInfo* pFormatInfo = GetFormatInfo(nEventType, pEventInfo);
			CalculateMyRect(pFormatInfo, pMiscInfo);
			break;
		}
		case SGEVENT_REDRAW:
		{
			DocRect MyRect(FormatRect);		// Rely on FormatRect being cached from above
			SGRedrawInfo* pRedrawInfo = GetRedrawInfo(nEventType, pEventInfo);
			if (IMustRedraw(pRedrawInfo))
			{
				StartRendering(pRedrawInfo, pMiscInfo);
				HandleRedraw(pRedrawInfo, pMiscInfo);
				StopRendering(pRedrawInfo, pMiscInfo);
			}
			break;
		}
		case SGEVENT_MOUSECLICK:
		{
			// Work out the bounds of the bitmap and the label.
			DocRect drBmp(FormatRect), drTxt(FormatRect);
			drBmp.hi.x = drBmp.lo.x + SG_DefaultSmallIcon;
			drTxt.lo.x = drBmp.hi.x + SG_GapBeforeText;
			GridLockRect(pMiscInfo, &drBmp);
			GridLockRect(pMiscInfo, &drTxt);

			// Check if the click is on the text label or the bitmap button beside it.
			SGMouseInfo* pMouse = GetMouseInfo(nEventType, pEventInfo);
			if (drTxt.ContainsCoord(pMouse->Position))
			{
				// It's a click on the text label.  Work out what to do.
				// No drag, so move on to selection click handling.  Note that
				// Adjust-Double-Click does _not_ close the Name gallery.
				if (pMouse->DoubleClick)
					DefaultClickHandler(pMouse, pMiscInfo, FALSE, FALSE);
				else
				{
					// See if the base class can handle the mouse event.
					DefaultPreDragHandler(pMouse, pMiscInfo);
					
					// Make an appropriate attribute and try to start a drag.
					String_256 strName;
					GetNameText(&strName);
					TemplateAttribute* pAttrib =
						new TemplateAttribute(String_256(TEXT("ObjectName")),
											  NullString, strName);
					if (pAttrib != 0)
					{
						GalleryNameDragInfo* pInfo = 
							new GalleryNameDragInfo(this, pMouse, pMiscInfo, pAttrib,
													pMouse->MenuClick);
						if (pInfo != 0) DragManagerOp::StartDrag(pInfo);
					}
				}
					
				// Claim this event - nobody else can own this click.
				return TRUE;
			}
			else if (drBmp.ContainsCoord(pMouse->Position))
			{
				// It's a click on the selection bitmap button.
				String_256 strName;
				GetNameText(&strName);
				INT32 nChanged = 0;
				switch (m_eIntersect)
				{
				case AttributeSets::Intersect::NONE:
				case AttributeSets::Intersect::SOME:
					nChanged = GetDocument()->GetAttributeSets()->ChangeObjSelect(strName, TRUE);
					break;

				case AttributeSets::Intersect::ALL:
					nChanged = GetDocument()->GetAttributeSets()->ChangeObjSelect(strName, FALSE);					
					break;
				}

				// Make sure the selection is updated if something happened.
				if (nChanged != 0) GetApplication()->FindSelection()->Update(TRUE);

				// Claim the event.
				return TRUE;
			}
			break;
		}
		default:
			// Let the base class handle any events we don't know about.
			// This includes things like hit testing (CLAIMPOINT) etc
			return SGDisplayItem::HandleEvent(nEventType, pEventInfo, pMiscInfo);
	}

	// Default return value: we do not claim this event, so it will be passed on to others.
	return FALSE;
}



/********************************************************************************************
>	NameGallery::NameGallery()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	Initialises a NameGallery object, registering an idle processor for it.
********************************************************************************************/

NameGallery::NameGallery()
  :	m_fMenusCreated(FALSE),
	m_fIdleProc(FALSE)
{
	// Empty.
}



/********************************************************************************************
>	NameGallery::~NameGallery()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	Destroys a NameGallery object, deregistering its idle processor.
********************************************************************************************/

NameGallery::~NameGallery()
{
	// Stop sending idle events if we are still claiming them.
	if (m_fIdleProc) GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);
}



/********************************************************************************************
>	BOOL NameGallery::PreCreate()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
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
	// If there isn't already one, create a DisplayTree.
	if (GetDisplayTree() == 0)
	{
		DisplayTree = new SGDisplayRootScroll(this);
		ERRORIF(DisplayTree == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	}

	// For each document already present, create a display subtree (possibly replacing
	// a previous displaytree for it if we had one earlier)
	Document* pDoc = (Document*) GetApplication()->Documents.GetTail();
	while (pDoc != 0)
	{
		// Find the existing subtree, if any, and create the items for it.
		SGNameGroup* pGroup = (SGNameGroup*) GetDisplayTree()->FindSubtree(this, pDoc, 0);
		if (!CreateItems(pDoc, pGroup)) return FALSE;
		pDoc = (Document*) GetApplication()->Documents.GetPrev(pDoc);
	}

	// Success.
	return TRUE;
}



/********************************************************************************************
>	virtual SGNameGroup* NameGallery::CreateItems(Document* pParentDoc, SGNameGroup* pGroup)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		pParentDoc - The document to create a display subtree for
				pGroup - NULL (creates a new group for this document), or
				a pointer to the existing group-node for this document (in which case
				it clears all displayitems from the group and rebuilds it in place - this
				stops the display group moving around the tree at random!)
	Returns:	Pointer to the group the items were created in, or null if it fails.
	Purpose:	Internal call. This takes the NameItem list of the given document and 
				creates a DisplayTree subtree from it. This subtree is then added to
				the DisplayTree. Note that this does not force a redraw of the list - 
				after making this call, you should also call ForceRedrawOfList
	Notes:		Passing in a NULL parent document pointer results in an ERROR3 -
				the function returns without doing anything in retail builds
	SeeAlso:	SuperGallery::ForceRedrawOfList
********************************************************************************************/

SGNameGroup* NameGallery::CreateItems(Document* pParentDoc, SGNameGroup* pGroup)
{
	ERROR3IF(pParentDoc == 0, "NameSGallery::CreateNewSubtree: no document");

	// If there isn't already one, create a DisplayTree.
	if (GetDisplayTree() == 0)
	{
		DisplayTree = new SGDisplayRootScroll(this);
		ERRORIF(DisplayTree == 0, _R(IDE_NOMORE_MEMORY), 0);
	}

	// Set up a sort key ("by name") for both items and groups.
	SGSortKey sk[2] = { { SGSORTKEY_BYNAME, FALSE }, { SGSORTKEY_NONE, FALSE } };

	// Use either the provided group or create a new one.
	if (pGroup != 0)
	{
		ERROR3IF(pGroup->GetParentDocument() != pParentDoc, 
							"NameGallery::CreateItems: Group/Document mismatch");
		pGroup->DestroySubtree(FALSE);
	}
	else
	{
		pGroup = new SGNameGroup(this, pParentDoc);
		ERRORIF(pGroup == 0, _R(IDE_NOMORE_MEMORY), 0);
		GetDisplayTree()->AddItem(pGroup, sk);
	}

	// Iterate over all the name in the set, creating a DisplayItem for
	// each one and inserting it in the DisplayGroup.
	String_256 strName;
	AttributeSets* pSets = pParentDoc->GetAttributeSets();
	BOOL fOK = pSets->GetFirstSet(&strName);
	while (fOK)
	{
		SGNameItem* pNewItem = new SGNameItem(strName);
		ERRORIF(pNewItem == 0, _R(IDE_NOMORE_MEMORY), 0);
		pGroup->AddItem(pNewItem, sk);
		fOK = pSets->GetNextSet(&strName);
	}

	// Success.
	return pGroup;
}



/********************************************************************************************
>	virtual MsgResult NameGallery::Message(Msg* pMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
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
	// User interface?
	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* pMsg = (DialogMsg*) pMessage;
		switch (pMsg->DlgMsg)
		{
		case DIM_CREATE:
			SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_NAME_GALLERY, TRUE);
			break;

		case DIM_CANCEL:
			SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_NAME_GALLERY, FALSE);
			break;

		// Handle custom buttons.
		case DIM_LFT_BN_CLICKED:
			switch(pMsg->GadgetID)
			{
			case _R(IDC_NAMEGAL_RENAME):
				ApplyAction(SGACTION_EDIT);
				break;

			case _R(IDC_NAMEGAL_UNNAME):
				ApplyAction((SGActionType) SGACTION_UNNAME);
				break;

			case _R(IDC_GALLERY_HELP):
				HelpUserTopic(_R(IDS_HELPPATH_Gallery_Name));
				break;
			}
			break;
		}
	}

		// A new document or a change in focus?
	else if (MESSAGE_IS_A(pMessage, DocChangingMsg))
	{
		DocChangingMsg* pMsg = (DocChangingMsg*) pMessage;
		switch (pMsg->State)
		{
		case DocChangingMsg::DocState::BORNANDSTABLE:
			{
				// A new document has been opened, create a new group and items for it.
				MILLIPOINT nExtent = GetDisplayExtent();
				if (!CreateItems(pMsg->pChangingDoc, 0)) return FAIL;
				ShadeGallery(FALSE);
				InvalidateCachedFormat();
				RedrawEverythingBelow(-nExtent);
			}
			break;

		case DocChangingMsg::DocState::SELCHANGED:
			// Unhighlight all the items in the old document's group.
			if (pMsg->pOldDoc != 0) SelectItems(FALSE, FALSE, pMsg->pOldDoc);
			break;
		}
	
		// Make sure the button states are updated.
		SelectionHasChanged();
	}		

	// New intersection to show?
	else if (MESSAGE_IS_A(pMessage, SelChangingMsg))
	{
		SelChangingMsg* pMsg = (SelChangingMsg*) pMessage;
		if (pMsg->State == SelChangingMsg::SelectionState::SELECTIONCHANGED)
			RefreshDocument(Document::GetCurrent());
	}

	// An message that a Document's AttributeSets has changed?
	else if (MESSAGE_IS_A(pMessage, AttrSetChangeMsg))
	{
		AttrSetChangeMsg* pMsg = (AttrSetChangeMsg*) pMessage;
		RefreshDocument(pMsg->GetDocument());
	}

	// Always pass messages on for base class handling.
	return SuperGallery::Message(pMessage);
}



/********************************************************************************************
>	virtual void NameGallery::RefreshDocument(Document* pDoc)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		pDoc	---		the Document whose gallery items should be refreshed 
				on the next idle event.
	Purpose:	Marks a gallery as needing to be refreshed/redrawn on the next idle
				event sent to the NameGallery.
	SeeAlso:	NameGallery::Message; NameGallery::OnIdleEvent; SGNameGroup::SetRefresh
********************************************************************************************/

void NameGallery::RefreshDocument(Document* pDoc)
{
	// Ignore messages sent before there's a DisplayTree, or for Documents which don't
	// have an associated group.
	if (GetDisplayTree() == 0) return;
	SGNameGroup* pGroup = (SGNameGroup*) GetDisplayTree()->FindSubtree(this, pDoc, 0);
	if (pGroup != 0)
	{
		// Mark the group as needing to be refreshed and if there isn't one already,
		// register an idle event handler for this gallery.
		pGroup->SetRefresh();
		if (!m_fIdleProc)
		{
			// Set at high priority so the gallery UI is quick.  The proc will be
			// deregistered after one event so it won't hog the idle time.
			GetApplication()->RegisterIdleProcessor(IDLEPRIORITY_HIGH, this);
			m_fIdleProc = TRUE;
		}
	}
}



/********************************************************************************************
>	virtual BOOL NameGallery::OnIdleEvent()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
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
	// If there's no display tree or we're shaded or closed then there's nothing
	// more to do.
	BOOL fChanged = FALSE;
	if (GetDisplayTree() != 0 && !AmShaded && IsVisible())
	{
		// Iterate over all the groups, recreating and redrawing each that is marked
		// for refresh.
		SGNameGroup* pGroup = (SGNameGroup*) GetDisplayTree()->GetChild();
		while (pGroup != 0)
		{
			if (pGroup->NeedsRefresh())
			{
				// Recreate the items in this group and redraw everything.
				CreateItems(pGroup->GetParentDocument(), pGroup);
				fChanged = TRUE;
			}

			pGroup = (SGNameGroup*) pGroup->GetNext();
		}
	}
	
	// If we changed something then update the buttons and the display.
	if (fChanged)
	{
		SelectionHasChanged();
		ForceRedrawOfList();
	}

	// Kill the idle event now the update has been done.
	ERROR3IF(!m_fIdleProc, "NameGallery::OnIdleEvent: lost track of idle event proc");
	GetApplication()->RemoveIdleProcessor(IDLEPRIORITY_HIGH, this);
	m_fIdleProc = FALSE;

	// Don't call the SuperGallery handler as this gallery doesn't use the
	// background redraw facility and works on a lower priority level.
	return TRUE;	
}



/********************************************************************************************
>   virtual void NameGallery::SelectionHasChanged()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    7/4/95
	Purpose:    Called by the base-class SuperGallery whenever the user clicks on an item.
				This version overrides the default bahaviour so that selecting more than
				one item does NOT grey the "Apply" button.
********************************************************************************************/

void NameGallery::SelectionHasChanged()
{
	// Nothing to do if we are unavailable.
	if (AmShaded || !IsVisible()) return;

	// Work out if the user has selected any objects in the selected Document.
	BOOL fSelectionExists = Document::GetSelected() != 0 &&
							!GetApplication()->FindSelection()->IsEmpty();
	
	// Work out how items are highlighted in the gallery.
	INT32 n = GetSelectedItemCount();

	// New button available when there is something selected in the Document.
	EnableGadget(_R(IDC_GALLERY_NEW), fSelectionExists);

	// Apply and Delete buttons available when at least one item is highlighted.
	EnableGadget(_R(IDC_GALLERY_APPLY), n >= 1);
	EnableGadget(_R(IDC_GALLERY_DELETE), n >= 1);

	// Rename button available when only one item is highlighted.
	EnableGadget(_R(IDC_NAMEGAL_RENAME), n == 1);

	// Redefine button available when at least one item is highlighted and something
	// is selected in the Document.
	EnableGadget(_R(IDC_GALLERY_REDEFINE), n >= 1 && fSelectionExists);

	// Unname button available when at least one item is highlighted and something
	// is selected in the Document.
	EnableGadget(_R(IDC_NAMEGAL_UNNAME), n >= 1 && fSelectionExists);
}



/********************************************************************************************
>	virtual BOOL NameGallery::ApplyAction(SGActionType nAction)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		nAction - Indicates what action to apply
	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure
	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)
	SeeAlso:	SGActionType
********************************************************************************************/

BOOL NameGallery::ApplyAction(SGActionType nAction)
{
	// No display tree? Better forget about it then!
	if (GetDisplayTree() == 0) return FALSE;

	// Determine useful info - this is usually needed for most actions, so always get it
	Document* pDoc = Document::GetSelected();
	ERROR3IF(pDoc == 0, "NameGallery::ApplyAction: no selected Document");
	SGNameGroup* pGroup = (SGNameGroup*) GetDisplayTree()->FindSubtree(this, pDoc, 0);
	ERROR3IF(pGroup == 0, "NameGallery::ApplyAction: no Group for selected Document");

	// Call the appropriate handler.
	switch (nAction)
	{
	case SGACTION_APPLY:
		return OnApply(pGroup, FALSE);

	case SGACTION_APPLYADJUST:
		return OnApply(pGroup, TRUE);

	case SGACTION_CREATE:
		return OnCreate(pGroup);

	case SGACTION_EDIT:
		return OnRename(pGroup);

	case SGACTION_DELETE:
		return OnDelete(pGroup);

	case SGACTION_REDEFINE:
		return OnRedefine(pGroup);

	case SGACTION_UNNAME:
		return OnUnname(pGroup);

	default:
		TRACEUSER( "JustinF", _T("NameGallery::ApplyAction: unimplemented action\n"));
		break;
	}

	// Successful processing.
	return TRUE;
}



/***********************************************************************************************
>	virtual BOOL NameGallery::OnApply(SGNameGroup* pGroup, BOOL fIsAdjust)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup		---		the group whose items are being applied
				fIsAdjust	---		if TRUE then this is an "adjust" action, eg. the
									user ran the action with the Control button as a
									click modifier.
	Returns:	TRUE if successful.
	Purpose:	Selects (not "adjust") or toggles ("adjust") the selection status of all
				the objects in the associated attribute set of each highlighted
				SGNameItem.
	Notes:		The Application's SelRange object is updated _after_ the SuperGallery
				event has been processed, as calling SelRange::Update during an SG event
				may destroy the SGNameItems which are processing it.
	SeeAlso:	NameGallery::ApplyAction; AttributeSets::ChangeObjSelect
***********************************************************************************************/

BOOL NameGallery::OnApply(SGNameGroup* pGroup, BOOL fIsAdjust)
{
	// If not Adjust then deselect all other objects first.  If Adjust then _don't_ change
	// the selection of other objects.  Don't broadcast a SelChangingMsg or the items will
	// be deleted before the iteration.
	if (!fIsAdjust) NodeRenderableInk::DeselectAll(TRUE, FALSE);

	// Work out what the new selection status of the objects will be.  If Adjust then
	// toggle the selection states, otherwise set the selection states.
	BOOL fState = (fIsAdjust) ? -1 : TRUE;

	// Iterate over all the SGNameItems in pGroup which are selected.
	INT32 nChanged = 0;
	SGNameItem* pItem = (SGNameItem*) pGroup->FindNextSelectedItem(0); 
	while (pItem != 0)
	{
		SGNameItem* pNextItem = (SGNameItem*) pGroup->FindNextSelectedItem(pItem);	

		// Change the selection status of every object in the sets.  Afterwards
		// NameGallery::Message will update the app's object selection.
		String_256 strName;
		pItem->GetNameText(&strName);
		nChanged += pItem->GetDocument()->GetAttributeSets()->ChangeObjSelect(strName, fState);

		pItem = pNextItem;
	}

	// Inform the application that the selection has changed if any objects were affected.
	if (nChanged != 0) GetApplication()->FindSelection()->Update(TRUE);
	return TRUE;
}



/***********************************************************************************************
>	virtual BOOL NameGallery::GetNewName(SGNameGroup* pGroup, UINT32 nDialogTemplateID,
										 String_256* pstrName)	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup				---		the group that a new name is being prompted for
				nDialogTemplateID	---		the template ID to use, ie. _R(IDD_NAMEOBJ_NAMEDLG)
											or _R(IDD_NAMEOBJ_RENAMEDLG)
	Outputs:	
	Returns:	TRUE if successful and *pstrName is valid, FALSE if the user cancelled the
				dialog or something else went wrong.
	Purpose:	Runs either the "Name selected objects" or "Rename objects" dialogs,
				returning a new, unique name chosen by the user.
	SeeAlso:	NameGallery::OnCreate; NameGallery::OnRename; class NameObjectsDlg
***********************************************************************************************/

BOOL NameGallery::GetNewName(SGNameGroup* pGroup, UINT32 nDialogTemplateID, String_256* pstrName)
{
	// Nightmarish pointer gubbins.
	ERROR3IF(pstrName == 0, "NameGallery::GetNewName: no output parameter");
	ERROR3IF(pGroup == 0 || pGroup->GetParentDocument() == 0,
					"NameGallery::GetNewName: no Group or Document");

	AttributeSets* pSets = pGroup->GetParentDocument()->GetAttributeSets();
	OpDescriptor* pDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_NAME_OBJECTS_DLG);
	ERROR3IF(pDesc == 0, "NameGallery::OnCreate: can't find OPTOKEN_NAME_OBJECTS_DLG");
	
	// Prompt the user until successfully ok-ed or cancelled.
	for (;;)
	{
		// Run the given dialog and get its result.
		BOOL fOk;
		pDesc->Invoke(&OpParam((INT32) pstrName, (INT32) nDialogTemplateID, &fOk));

		// Did the user cancel the dialog or OK it with a valid entry?
		if (!fOk) return FALSE;
		if (!pSets->IsSet(*pstrName)) break;
		
		// Explain to the user that new names must be different and prompt again.
		InformError(_R(IDE_NAMEOBJ_NAME_EXISTS));
	}

	// Success.
	return TRUE;
}



/***********************************************************************************************
>	virtual BOOL NameGallery::OnCreate(SGNameGroup* pGroup)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup		---		the group in which a new name is to be created
	Purpose:	Prompts the user for a new name for the selected objects in the
				group's associated document	and creates a new attribute set with
				that name.
	Returns:	TRUE if successful.
	SeeAlso:	NameGallery::ApplyAction; NameGallery::GetNewName;
				AttributeSets::CreateSetOfSelectedObj
***********************************************************************************************/

BOOL NameGallery::OnCreate(SGNameGroup* pGroup)
{
	ERROR3IF(pGroup == 0 || pGroup->GetParentDocument() == 0,
				"NameGallery::OnCreate: no Group or Document");
	
	// Run the "Name selected objects" dialog and get its result back.  If the user
	// cancels the dialog we've nothing more to do.
	String_256 strName = GetApplication()->FindSelection()->Describe(STATUS_BAR);
	if (!GetNewName(pGroup, _R(IDD_NAMEOBJ_NAMEDLG), &strName)) return TRUE;

	// Ask the Document's AttributeSets object to make a set of the given name of all
	// the selected objects.
	ERROR3IF(pGroup->GetParentDocument() != Document::GetSelected(),
		"NameGallery::OnCreate: can't CreateSetOfSelectedObj as Document not selected");
	return pGroup->GetParentDocument()->GetAttributeSets()->CreateSetOfSelectedObj(strName);
}



/***********************************************************************************************
>	virtual BOOL NameGallery::OnRename(SGNameGroup* pGroup)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup		---		the group containing the item to be renamed
	Returns:	TRUE if successful.
	Purpose:	Changes the existing name of objects.
	SeeAlso:	NameGallery::ApplyAction; NameGallery::GetNewName; AttributeSets::RenameSet
***********************************************************************************************/

BOOL NameGallery::OnRename(SGNameGroup* pGroup)
{
	ERROR3IF(pGroup == 0 || pGroup->GetParentDocument() == 0,
				"NameGallery::OnRename: no Group or Document");
	
	// Extract the old name from the item.
	String_256 strName;
	SGNameItem* pItem = (SGNameItem*) pGroup->FindNextSelectedItem(0);
	pItem->GetNameText(&strName);

	// Run the "Name selected objects" dialog and get its result back.  If the user
	// cancels the dialog we've nothing more to do.
	String_256 strNewName = strName;
	if (!GetNewName(pGroup, _R(IDD_NAMEOBJ_RENAMEDLG), &strNewName)) return TRUE;

	// Ask the Document's AttributeSets object to make a set of the given name of all
	// the selected objects.
	return pGroup->GetParentDocument()->GetAttributeSets()->RenameSet(strName, strNewName);
}



/***********************************************************************************************
>	virtual BOOL NameGallery::OnDelete(SGNameGroup* pGroup)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup		---		the group containing the items to be deleted
	Returns:	TRUE if successful.
	Purpose:	Deletes the given set from the group's document's AttributeSets.  
	SeeAlso:	NameGallery::ApplyAction
***********************************************************************************************/

BOOL NameGallery::OnDelete(SGNameGroup* pGroup)
{
	ERROR3IF(pGroup == 0 || pGroup->GetParentDocument() == 0,
				"NameGallery::OnDelete: no Group or Document");
	
	// Allocate an array of name parameters at least as big as all the number of
	// selected items.  Set the last entry to null.
	INT32 nEntries = GetSelectedItemCount();
	typedef const StringBase* PSTRBASE;
	PSTRBASE* pstrNames = new PSTRBASE[nEntries + 1];
	ERRORIF(pstrNames == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	pstrNames[nEntries] = 0;

	// Iterate over all the SGNameItems in pGroup which are selected, extracting their
	// names and allocating a copy for them.
	PSTRBASE* pstrIter = pstrNames;
	SGNameItem* pItem = (SGNameItem*) pGroup->FindNextSelectedItem(0); 
	while (pItem != 0)
	{
		SGNameItem* pNextItem = (SGNameItem*) pGroup->FindNextSelectedItem(pItem);	
		
		String_256 strName;
		pItem->GetNameText(&strName);
		*pstrIter = new String_256(strName);
		if (*pstrIter++ == 0)
		{
			delete[] pstrNames;
			Error::SetError(_R(IDE_NOMORE_MEMORY), 0);
			return FALSE;
		}

		pItem = pNextItem;
	}

	// Destroy the attribute sets.
	BOOL fResult = pGroup->GetParentDocument()->GetAttributeSets()->DestroySets(pstrNames);

	// Deallocate the parameter array and return the result.
	pstrIter = pstrNames;
	while (*pstrIter != 0) delete *pstrIter++;
	delete[] pstrNames;
	return fResult;
}



/***********************************************************************************************
>	virtual BOOL NameGallery::OnRedefine(SGNameGroup* pGroup)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup		---		the group containing the items to be redefined
	Returns:	TRUE if successful.
	SeeAlso:	NameGallery::ApplyAction
***********************************************************************************************/

BOOL NameGallery::OnRedefine(SGNameGroup* pGroup)
{
	ERROR3IF(pGroup == 0 || pGroup->GetParentDocument() == 0,
				"NameGallery::OnRedefine: no Group or Document");
	
	// Allocate an array of name parameters at least as big as all the number of
	// selected items.  Set the last entry to null.
	INT32 nEntries = GetSelectedItemCount();
	typedef const StringBase* PSTRBASE;
	PSTRBASE* pstrNames = new PSTRBASE[nEntries + 1];
	ERRORIF(pstrNames == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	pstrNames[nEntries] = 0;

	// Iterate over all the SGNameItems in pGroup which are selected, extracting their
	// names and allocating a copy for them.
	PSTRBASE* pstrIter = pstrNames;
	SGNameItem* pItem = (SGNameItem*) pGroup->FindNextSelectedItem(0); 
	while (pItem != 0)
	{
		SGNameItem* pNextItem = (SGNameItem*) pGroup->FindNextSelectedItem(pItem);	
		
		String_256 strName;
		pItem->GetNameText(&strName);
		*pstrIter = new String_256(strName);
		if (*pstrIter++ == 0)
		{
			delete[] pstrNames;
			Error::SetError(_R(IDE_NOMORE_MEMORY), 0);
			return FALSE;
		}

		pItem = pNextItem;
	}

	// Destroy the attribute sets.
	BOOL fResult =
		pGroup->GetParentDocument()->GetAttributeSets()->RedefineSetsAsSelectedObj(pstrNames);

	// Deallocate the parameter array and return the result.
	pstrIter = pstrNames;
	while (*pstrIter != 0) delete *pstrIter++;
	delete[] pstrNames;
	return fResult;
}



/***********************************************************************************************
>	virtual BOOL NameGallery::OnUnname(SGNameGroup* pGroup)
	
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/99
	Inputs:		pGroup		---		the group containing the items to be removed from the 
									selected objects
	Returns:	TRUE if successful.
	Purpose:	Removes the given sets from Document's currently selected objects.
	SeeAlso:	NameGallery::ApplyAction
***********************************************************************************************/

BOOL NameGallery::OnUnname(SGNameGroup* pGroup)
{
	ERROR3IF(pGroup == 0 || pGroup->GetParentDocument() == 0,
				"NameGallery::OnUnname: no Group or Document");
	
	// Allocate an array of name parameters at least as big as all the number of
	// selected items.  Set the last entry to null.
	INT32 nEntries = GetSelectedItemCount();
	typedef const StringBase* PSTRBASE;
	PSTRBASE* pstrNames = new PSTRBASE[nEntries + 1];
	ERRORIF(pstrNames == 0, _R(IDE_NOMORE_MEMORY), FALSE);
	pstrNames[nEntries] = 0;

	// Iterate over all the SGNameItems in pGroup which are selected, extracting their
	// names and allocating a copy for them.
	PSTRBASE* pstrIter = pstrNames;
	SGNameItem* pItem = (SGNameItem*) pGroup->FindNextSelectedItem(0); 
	while (pItem != 0)
	{
		SGNameItem* pNextItem = (SGNameItem*) pGroup->FindNextSelectedItem(pItem);	
		
		String_256 strName;
		pItem->GetNameText(&strName);
		*pstrIter = new String_256(strName);
		if (*pstrIter++ == 0)
		{
			delete[] pstrNames;
			Error::SetError(_R(IDE_NOMORE_MEMORY), 0);
			return FALSE;
		}

		pItem = pNextItem;
	}

	// Destroy the attribute sets.
	BOOL fResult =
		pGroup->GetParentDocument()->GetAttributeSets()->RemoveSelectedObjFromSets(pstrNames);

	// Deallocate the parameter array and return the result.
	pstrIter = pstrNames;
	while (*pstrIter != 0) delete *pstrIter++;
	delete[] pstrNames;
	return fResult;
}



/********************************************************************************************
>	virtual BOOL NameGallery::InitMenuCommands()
												 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95
	Returns:	TRUE if successful.
	Purpose:	Initialises any menu commands that the Name gallery needs.
********************************************************************************************/

BOOL NameGallery::InitMenuCommands()
{
	// Menus are only initialised once.
	return (m_fMenusCreated)
				? TRUE
				: m_fMenusCreated =
						InitMenuCommand((StringBase*) &SGCmd_Find, _R(IDS_SGMENU_FIND))
					 &&	InitMenuCommand((StringBase*) &SGCmd_Apply, _R(IDS_SGMENU_APPLY))
					 && InitMenuCommand((StringBase*) &SGCmd_New, _R(IDS_SGMENU_NEW))
					 && InitMenuCommand((StringBase*) &SGCmd_Unname, _R(IDS_SGMENU_UNNAME))
					 && InitMenuCommand((StringBase*) &SGCmd_Delete, _R(IDS_SGMENU_DELETE))
					 && InitMenuCommand((StringBase*) &SGCmd_Rename, _R(IDS_SGMENU_RENAME))
					 && InitMenuCommand((StringBase*) &SGCmd_Redefine, _R(IDS_SGMENU_REDEFINE))
					 && InitMenuCommand((StringBase*) &SGCmd_FoldGroup, _R(IDS_SGMENU_FOLD))
					 && InitMenuCommand((StringBase*) &SGCmd_UnfoldGroup, _R(IDS_SGMENU_UNFOLD))
					 && InitMenuCommand((StringBase*) &SGCmd_PrevGroup, _R(IDS_SGMENU_PREVGROUP))
					 && InitMenuCommand((StringBase*) &SGCmd_NextGroup, _R(IDS_SGMENU_NEXTGROUP));
}



/********************************************************************************************
>	virtual BOOL NameGallery::BuildCommandMenu(GalleryContextMenu* pMenu, SGMenuID id)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95
	Inputs:		pMenu	---		the menu to add commands to
				id		---		the menu type (over-list or options-button) to create
	Returns:	TRUE if successful.
	Purpose:	To build a menu of commands to be popped up over the gallery.
	Notes:		The Name gallery provides only a simple pop-up menu for items.
********************************************************************************************/

BOOL NameGallery::BuildCommandMenu(GalleryContextMenu* pMenu, SGMenuID id)
{
	// Only handle pop-ups over display items.
	if (id != SGMENU_OVERITEM) return TRUE;
	SGNameGroup* pGroup = (SGNameGroup*) FindCommandGroup();
	return AddCommand(pMenu, (StringBase*) &SGCmd_Find, TRUE) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_Apply) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_New) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_Unname) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_Delete) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_Rename) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_Redefine, TRUE) &&
		   ((pGroup == 0 || !pGroup->Flags.Folded)
				? AddCommand(pMenu, (StringBase*) &SGCmd_FoldGroup)
				: AddCommand(pMenu, (StringBase*) &SGCmd_UnfoldGroup)) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_PrevGroup) &&
		   AddCommand(pMenu, (StringBase*) &SGCmd_NextGroup);
}



/********************************************************************************************
>	virtual OpState NameGallery::OpState GetCommandState(StringBase* pstrCommandID,
														 String_256* pstrShadeReason)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95
	Inputs:		pstrCommandID	---		the ID of the command
	Outputs:	pstrShadeReason ---		if returning (OpState.Greyed == TRUE) then this 
										is set to the reason that the item is shaded/greyed.
	Returns:	An OpState indicating the current menu item state.
	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's GetState)
	Notes:		Override this method to provide state info for your special commands
				Call the base class for unknown commands to allow it to handle them for you.

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine;
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;
********************************************************************************************/

OpState NameGallery::GetCommandState(StringBase* pstrCommandID, String_256* pstrShadeReason)
{
	// If the gallery is closed then return "greyed".
	OpState st(FALSE, TRUE);
	if (AmShaded) return st;

	// Work out if the user has selected any objects in the selected Document.
	BOOL fSelectionExists = Document::GetSelected() != 0 &&
							!GetApplication()->FindSelection()->IsEmpty();
	
	// Work out how items are highlighted in the gallery.
	INT32 n = GetSelectedItemCount();

	// Work out the state for each command.
	if (*pstrCommandID == SGCmd_New)
		// 'New' is greyed if there's no selected document or no objects are selected.
		st.Greyed = !fSelectionExists;
	
	else if (*pstrCommandID == SGCmd_Apply || *pstrCommandID == SGCmd_Delete)
		// 'Apply/Delete' are greyed when no items are highlighted in the gallery.
		st.Greyed = (n == 0);
	
	else if (*pstrCommandID == SGCmd_Rename)
		// 'Rename' is enabled when only one item is highlighted.
		st.Greyed = (n != 1);

	else if (*pstrCommandID == SGCmd_Redefine || *pstrCommandID == SGCmd_Unname)
		// 'Redefine' and 'Unname' are greyed when no items are highlighted or there are
		// no selected objects in the Document.
		st.Greyed = (n == 0) || !fSelectionExists;
	
	else
		// Call the base class for default handling of the other commands.
		st = SuperGallery::GetCommandState(pstrCommandID, pstrShadeReason);

	return st;
}



/********************************************************************************************
>	virtual void NameGallery::DoCommand(StringBase* pstrCommandID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95
	Inputs:		pstrCommandID	---		the ID of the command
	Purpose:	Overrides the default virtual function in the SuperGallery base class so
				as to handle the custom SGACTION_UNNAME command.
********************************************************************************************/

void NameGallery::DoCommand(StringBase* pstrCommandID)
{
	if (*pstrCommandID == SGCmd_Unname)
		ApplyAction((SGActionType) SGACTION_UNNAME);
	else
		SuperGallery::DoCommand(pstrCommandID);
}



/********************************************************************************************
>	SGNameDragTarget::SGNameDragTarget()
	 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	Default constructor, required for dynamic creation.
********************************************************************************************/

SGNameDragTarget::SGNameDragTarget()
  :	SGListDragTarget(0, 0)
{
	// Empty.
}



/********************************************************************************************
>	SGNameDragTarget::SGNameDragTarget(NameGallery* pGallery, CGadgetID idGadget = 0);
	 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pGallery	---		the instance of the Name gallery
				idGadget	---		the gadget within that dialogue which is the target
	Purpose:	Initialises a drag target for an item from the Name gallery.
********************************************************************************************/

SGNameDragTarget::SGNameDragTarget(NameGallery* pGallery, CGadgetID idGadget)
  :	SGListDragTarget(pGallery, idGadget)
{
	// Empty.
}



/********************************************************************************************
	BOOL SGNameDragTarget::ProcessEvent(DragEventType nEvent, DragInformation* pDragInfo,
										OilCoord* pMousePos, KeyPress* pKeyPress)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		nEvent		---		indicates what has happened
				pDragInfo	---		points to drag information describing this drag. This
									should be a BitmapDragInformation or derivation thereof
				pMousePos	---		points to information on the current mouse position,
									in OIL coords
				pKeyPress	---		NULL, or if for a keypress event, keypress information
	Returns:	TRUE to claim the event, FALSE to let it through to other targets
	Purpose:	Event handler for SGNameItem gallery drag events. Overrides the
				base class handler to enable it to sort out the node being dragged
				for Name drags.
********************************************************************************************/

BOOL SGNameDragTarget::ProcessEvent(DragEventType nEvent, DragInformation* pDragInfo,
									OilCoord* pMousePos, KeyPress* pKeyPress)
{
	ERROR3IF(!pDragInfo->IS_KIND_OF(GalleryNameDragInfo),
				"SGNameDragTarget::ProcessEvent: wrong kind of DragInfo");

	SGDisplayNode* pDraggedNode = ((GalleryNameDragInfo*) pDragInfo)->GetDraggedNameAttr();
	if (pDraggedNode == 0) return FALSE;

	switch (nEvent)
	{
	case DRAGEVENT_COMPLETED:
		// Drag just one item.
		HandleDragCompleted((NameGallery*) TargetDialog, pDraggedNode, pMousePos, TRUE);
		return TRUE;

	case DRAGEVENT_MOUSEMOVED:
	case DRAGEVENT_MOUSESTOPPED:
	case DRAGEVENT_MOUSEIDLE:
		return DetermineCursorShape((NameGallery*) TargetDialog, pDraggedNode, pMousePos);
	}

	// Not interested in any other event so don't claim it.
	return FALSE;
}



/********************************************************************************************
>	GalleryNameDragInfo::GalleryNameDragInfo()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Purpose:	Default constructor, required for dynamic creation.
********************************************************************************************/

GalleryNameDragInfo::GalleryNameDragInfo()
  :	m_pSourceItem(0),
	m_pAttrib(0)
{
	// Empty.
}



/********************************************************************************************
>	GalleryNameDragInfo::GalleryNameDragInfo(SGNameItem* pSourceItem,
											 SGMouseInfo* pMouseInfo,
											 SGMiscInfo* pMiscInfo,
											 TemplateAttribute* pAttrib,
											 BOOL fIsAdjust = FALSE)
	Author:	 Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created: 10/4/95		  
	Inputs:	 pSourceItem	---		the gallery item from which the drag originated
			 pMouseInfo		---		the mouse info which made the item start the drag
			 pMiscInfo		---		the MiscInfo which accompanied the mouse event
			 pAttrib		---		the attribute associated with the SGNameItem
			 fIsAdjust		---		TRUE if this is an "adjust" (Ctrl) drag
********************************************************************************************/

GalleryNameDragInfo::GalleryNameDragInfo(SGNameItem* pSourceItem, SGMouseInfo* pMouseInfo,
										 SGMiscInfo* pMiscInfo, TemplateAttribute* pAttrib,
										 BOOL fIsAdjust)
  :	BitmapDragInformation(0, 100, 50, 0, 0, fIsAdjust),
	m_pSourceItem(pSourceItem),
	m_MouseInfo(*pMouseInfo),
	m_MiscInfo(*pMiscInfo),
	m_pAttrib(pAttrib)
{
	// Empty.
}



/********************************************************************************************
>	virtual GalleryNameDragInfo::~GalleryNameDragInfo()
 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Purpose:	Destructor.  Deletes the attribute associated with this object.
********************************************************************************************/

GalleryNameDragInfo::~GalleryNameDragInfo()
{
	if (m_pAttrib != 0)
	{
		delete m_pAttrib;
		m_pAttrib = 0;
	}
}



/********************************************************************************************
>	virtual void GalleryNameDragInfo::OnClick(INT32 nFlags, POINT ptClick) 
	 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Inputs:		nFlags		---		which keys are depressed during the drag
				ptClick		---		the position of the mosue click
	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. It calls back the SourceItem SGDisplayLine, to get it
				to handle the click.
********************************************************************************************/

void GalleryNameDragInfo::OnClick(INT32 nFlags, POINT ptClick)
{
	if (m_pSourceItem != 0) m_pSourceItem->DefaultClickHandler(&m_MouseInfo, &m_MiscInfo);
}



/********************************************************************************************
>	void GalleryNameDragInfo::GetCursorID(DragTarget* pDragTarget)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pDragTarget		---		the drag target to set the cursor for
	Purpose:	Set cursor over this target.
********************************************************************************************/

UINT32 GalleryNameDragInfo::GetCursorID(DragTarget* pDragTarget)
{
	// Only allow drops on the document view.
	if (pDragTarget == 0 || !pDragTarget->IS_KIND_OF(ViewDragTarget))
		return _R(IDC_CANTDROP);

	// Get information about what's below the cursor.
	PageDropInfo pdInfo;
	((ViewDragTarget*) pDragTarget)->GetDropInfo(&pdInfo);
	NodeRenderableInk* pObjectHit = pdInfo.pObjectHit;
//	ClickModifiers cmMods = ClickModifiers::GetClickModifiers();

	// If there's an object below the cursor then show the "can drop" cursor, otherwise
	// show the "can't drop" cursor.
	return (pObjectHit != 0) ? _R(IDC_CANDROPONPAGE) : _R(IDC_CANTDROP);
}



/********************************************************************************************
>	virtual BOOL GalleryNameDragInfo::GetStatusLineText(String_256* pText,
														DragTarget* pDragTarget)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		pText			---		where to output the status line text
				pDragTarget		---		the target of the drag (ie. the document view)
	Returns:	TRUE if the output string is valid.
	Purpose:	Works out status line text for this target of a Name gallery item drag.
********************************************************************************************/

BOOL GalleryNameDragInfo::GetStatusLineText(String_256* pText, DragTarget* pDragTarget)
{
	// Validate outputs and object state.
	ERROR3IF(pText == 0, "GalleryNameDragInfo::GetStatusLineText: no output parameter");
	if (TheBitmap == 0 || TheBitmap->ActualBitmap == 0) return FALSE;

	// Only provide status-line help for drops on the document view.
	if (pDragTarget == 0 || !pDragTarget->IS_KIND_OF(ViewDragTarget))
		return FALSE;

	// Get information about what the mouse is over in the document view.
	PageDropInfo pdInfo;
	((ViewDragTarget*) pDragTarget)->GetDropInfo(&pdInfo);
	NodeRenderableInk* pObjectHit = pdInfo.pObjectHit;

 	// Build up the status-line help text.
	*pText = _R(IDST_NAMEGAL_DRAG);							// "Dragging name"
	*pText += String_8(_R(IDS_SGDFONTS_STAT_COLON_SEP));	// " : "

	// Try to work out the specific description of the object the mouse is over.
	String_256 strObjectDesc = _R(IDS_SGLDRAG_THIS_OBJECT);	// " this object";
	if (pObjectHit != 0) strObjectDesc = pObjectHit->Describe(FALSE);

	*pText += String_256(_R(IDST_NAMEGAL_DROP_TO_APPLY));	// "Drop to apply this name to this ";
	*pText += strObjectDesc;
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL GalleryNameDragInfo::CanDropOnPage()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Returns:	TRUE if the a name item can be dropped on a page (ie. a document view).
	Purpose:	Called by the kernel dragging code to test whether a dragged name item
				can be dropped on a view onto a document.
********************************************************************************************/

BOOL GalleryNameDragInfo::CanDropOnPage()
{
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL GalleryNameDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Inputs:		pDragTarget		---		the drag target representing the view onto
										a document (the "page").
	Returns:	TRUE if successful.
	Purpose:	Called when a Name gallery item is dropped onto the page.
********************************************************************************************/

BOOL GalleryNameDragInfo::OnPageDrop(ViewDragTarget* pDragTarget)
{
	// Extract the address of the object dropped on and apply the dragged name
	// attribute to it.
	PageDropInfo pdInfo;
	pDragTarget->GetDropInfo(&pdInfo);
	NodeRenderableInk* pObjectHit = pdInfo.pObjectHit;
	if (pObjectHit != 0)
	{
		// This call always deletes the passed attribute, so make sure we don't as well.
		AttributeManager::ApplyAttribToNode(pObjectHit, m_pAttrib);
		m_pAttrib = 0;
	}
	return TRUE;
}



/********************************************************************************************
>	virtual INT32 GalleryNameDragInfo::GetDragTransparency()
 
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Returns:	A transparency value, in this case, 50%
	Purpose:	Specifies how transparent a drag should be.
********************************************************************************************/

INT32 GalleryNameDragInfo::GetDragTransparency()
{
	return 50;
}



/********************************************************************************************
>	virtual KernelBitmap* GalleryNameDragInfo::GetSolidDragMask()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95		  
	Returns:	A pointer to a KernelBitmap to use as a 1bpp mask for the dragged item,
				or 0 for failure.
	Purpose:	Renders an image of the dragged item into monochrome bitmap to use as an
				XOR mask for rendering the dragged Name gallery item.
********************************************************************************************/

KernelBitmap* GalleryNameDragInfo::GetSolidDragMask()
{
	// If there's no current View, or no Spread, then fail.
	DocView* pView = DocView::GetCurrent();
	if (pView == 0) return 0;
	Spread* pSpread = pView->FindEnclosingSpread(OilCoord(0, 0));
	if (pSpread == 0) return 0;

	// Create a device context for the display.
	CDC cdcDisplay;
	cdcDisplay.CreateDC("DISPLAY", 0, 0, 0); 

	// Calculate the size of the rendering and set up the rendering matrix etc.
	Matrix matConvert;
	FIXED16 fxScale = 1;
	DocRect drClip(0, 0, SG_DefaultNameText, SG_DefaultSmallIcon);

	// Create a render region with the given properties of the display etc.
	double dpi = (double) GetDeviceCaps(cdcDisplay.m_hDC, LOGPIXELSX);
	GRenderBitmap* pMaskRegion 	= new GRenderBitmap(drClip, matConvert, fxScale, 1, dpi);
	pMaskRegion->AttachDevice(pView, &cdcDisplay, pSpread);

	// Get the name of the item.
	String_256 strName;
	m_pSourceItem->GetNameText(&strName);

	// Render the blank background.
	pMaskRegion->StartRender();
	pMaskRegion->SetLineWidth(0);
	pMaskRegion->SetLineColour(COLOUR_BLACK);
	pMaskRegion->SetFillColour(COLOUR_BLACK);
	pMaskRegion->DrawRect(&drClip);

	// Render the text.
	DocColour dcText(COLOUR_WHITE), dcBack(COLOUR_BLACK);
	pMaskRegion->SetFixedSystemTextColours(&dcText, &dcBack);
	pMaskRegion->DrawFixedSystemText(&strName, drClip);				
	pMaskRegion->StopRender();

	// Create a kernel bitmap from the OIL bitmap and return it.
	OILBitmap* pOilMaskBmp = pMaskRegion->ExtractBitmap();
	delete pMaskRegion;
	return new KernelBitmap(pOilMaskBmp, TRUE);
}



/********************************************************************************************
>	NameObjectsDlg::NameObjectsDlg()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Purpose:	Create a NameObjectsDlg which is a modal DialogOp of template
				_R(IDD_NAMEOBJ_NAMEDLG) or _R(IDD_NAMEOBJ_RENAMEDLG).
********************************************************************************************/
																				

NameObjectsDlg::NameObjectsDlg()
  :	DialogOp(0, MODAL),				// template ID is passed via OpParam to DoWithParam()
	m_pstrOutputName(0),
	m_pfOkCancel(0)
{   
	// Empty.
}        



/********************************************************************************************
>	virtual void NameObjectsDlg::DoWithParam(OpDescriptor*, OpParam* pOpParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		pOpParam	---		pOpParam->Param1 is a pointer to the String_256 to
									set the committed value of the dialog to.
									pOpParam->Param2 is the dialog template to use
									ie. _R(IDD_NAMEOBJ_NAMEDLG) or _R(IDD_NAMEOBJ_RENAMEDLG).
									pOpParam->Output is a pointer to the BOOL to set to
									TRUE if the dialog is OK's, FALSE if it's cancelled.
	Purpose:	Creates then opens the dialog 
********************************************************************************************/


void NameObjectsDlg::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	// Remember the output parameters.
	ERROR3IF(pOpParam == 0, "NameObjectsDlg::DoWithParam: no OpParam");
	ERROR3IF(pOpParam->Param1 == 0 || pOpParam->Output == 0,
					"NameObjectsDlg::DoWithParam: no output parameters");

	// Extract the input and output parameters.
	m_pstrOutputName = (String_256*) (pOpParam->Param1);
	DlgResID = (CDlgResID) pOpParam->Param2;
	m_pfOkCancel = (BOOL*) (pOpParam->Output);

	// Try to run the operation.
	*m_pfOkCancel = FALSE;
	if (!Create())
	{
		InformError(0, _R(IDS_OK));
		End();
	}
}



/********************************************************************************************
>	MsgResult NameObjectsDlg::Message(Msg* pMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Purpose:	Handles kernel messages for the NameObjectsDlg dialog operation.
********************************************************************************************/

MsgResult NameObjectsDlg::Message(Msg* pMessage)
{
	// A message from the dialog or its gadgets?
	if (IS_OUR_DIALOG_MSG(pMessage))
	{
		DialogMsg* pMsg = (DialogMsg*) pMessage;
		switch (pMsg->DlgMsg)
		{
		case DIM_CREATE:
			SetStringGadgetValue(_R(IDC_NAMEOBJ_NAME), m_pstrOutputName);
			SetKeyboardFocus(_R(IDC_NAMEOBJ_NAME));
			HighlightText(_R(IDC_NAMEOBJ_NAME));
			break;
			
		case DIM_COMMIT:
			*m_pstrOutputName = GetStringGadgetValue(_R(IDC_NAMEOBJ_NAME), m_pfOkCancel);
			// nobreak;

		case DIM_CANCEL:
			Close();
			End();
			break;
		}
	}

	// Pass everything on to the base class . . .
	return DialogOp::Message(pMessage);
}  



/********************************************************************************************
>	static OpState NameObjectsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Purpose:	The usual default GetState function for the NameObjectsDlg dialog operation.
********************************************************************************************/

OpState	NameObjectsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return OpSt;
}



/********************************************************************************************
>	static BOOL NameObjectsDlg::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Purpose:	Register operation.
	SeeAlso:	SGInit::Init
********************************************************************************************/

BOOL NameObjectsDlg::Init()
{  
	return RegisterOpDescriptor(0,
								_R(IDS_NAMEOBJ_NAMEDLG),
								CC_RUNTIME_CLASS(NameObjectsDlg),
								OPTOKEN_NAME_OBJECTS_DLG,
								NameObjectsDlg::GetState,
								0,								// help ID
								0);								// bubble ID
}   



/********************************************************************************************
>	BOOL OpDisplayNameGallery::Init()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpDisplayNameGallery initialiser method.  Regsiters Operations.
********************************************************************************************/

BOOL OpDisplayNameGallery::Init()
{
	return RegisterOpDescriptor(0,
								_R(IDS_DISPLAY_NAME_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayNameGallery),
								OPTOKEN_DISPLAY_NAME_GALLERY,
								OpDisplayNameGallery::GetState,
								0,										// help id
								_R(IDBBL_DISPLAY_NAME_GALLERY),				// bubble help id
								0);										// bitmap id
}



/********************************************************************************************
>	OpState	OpDisplayNameGallery::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Returns:	The state of the OpDisplayNameGallery operation
	Purpose:	For finding the OpDisplayNameGallery's state. 
********************************************************************************************/

OpState	OpDisplayNameGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	// If the gallery is currently open, then the menu item should be ticked
	OpState OpSt;  
	DialogBarOp* pDialogBarOp = FindGallery();
	if (pDialogBarOp != 0) OpSt.Ticked = pDialogBarOp->IsVisible();

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == 0);
 	return OpSt;
}



/********************************************************************************************
>	void OpDisplayNameGallery::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Purpose:	Displays the Names gallery
				Updates the button state for this Op (the button sticks down while the
				gallery is open)
********************************************************************************************/

void OpDisplayNameGallery::Do(OpDescriptor*)
{
	DialogBarOp *pOp = FindGallery();
	if (pOp != 0)
	{
		// Toggle the visible state of the gallery window
		pOp->SetVisibility(!pOp->IsVisible());
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAY_NAME_GALLERY, pOp->IsVisible());
	}

	End();
}



/********************************************************************************************
>	static DialogBarOp* OpDisplayNameGallery::FindGallery()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Returns:	0 or a pointer to the Name gallery instance
	Purpose:	Finds the Name gallery class instance
	Notes:		The bars system always keeps one Name gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the NameGallery class
********************************************************************************************/

DialogBarOp* OpDisplayNameGallery::FindGallery()
{
	String_32 id(_R(IDS_SGNAME_GALLERY_NAME)); // "Name gallery";

	DialogBarOp* pOp = DialogBarOp::FindDialogBarOp(id);
	ERROR3IF(pOp == 0 || pOp->GetRuntimeClass() != CC_RUNTIME_CLASS(NameGallery), 
		"OpDisplayNameGallery::FindGallery: Can't find the gallery in bars.ini!\n");
	return pOp;
}
