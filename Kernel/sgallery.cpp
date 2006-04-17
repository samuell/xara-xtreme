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
// SGallery.h - the SuperGallery class

#include "camtypes.h"

#include "app.h"
#include "camelot.h"
#include "dragmsg.h"
#include "fixmem.h"
//#include "galstr.h"
//#include "galres.h"
//#include "jason.h"
#include "keypress.h"
#include "camframe.h"
#include "progress.h"
//#include "resource.h"	// For _R(IDS_OK) (dammit!)
#include "sgallery.h"
#include "sgdrag.h"
#include "sgmenu.h"
#include "sgtree.h"
#include "thumbmsg.h"
//#include "sglinepr.h"

#include "ccdc.h"		// For render-into-dialogue support
#include "dlgcol.h"
#include "fillval.h"
#include "grnddib.h"

#include "sgindgen.h"	// For ReplaceCharacters function (which should be in stringbase)
#include "sglib.h"		// For Searching stuff, which will be removed shortly
//#include "sglbase.h"	// For searching stuff
//#include "sglcart.h"	// For searching stuff
//#include "sglfills.h"	// For searching stuff

//#include "richard3.h"	// Few extra text resources

#include "brushmsg.h"   // for the screen change message

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(SuperGallery, DialogOp)
CC_IMPLEMENT_DYNCREATE(SGalleryOptionsDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(SGallerySortDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(SGallerySearchDlg, DialogOp)

// OpParam class for opening the options and search dlgs - local to this .cpp file
class GalDlgParam : public OpParam
{
CC_DECLARE_MEMDUMP(GalDlgParam)
public:
	GalDlgParam(SuperGallery *Parent) : OpParam(0, 0) { ParentGal = Parent; };
	SuperGallery *ParentGal;
};

CC_IMPLEMENT_MEMDUMP(GalDlgParam, OpParam)




// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


// Static variables
SuperGallery *SuperGallery::CurrentSortGallery = NULL;	// Points at the gallery doing the current sort
														// Used inside the qsort comparator function


// Preferences
INT32 SuperGallery::UseFixedColourSet = FALSE;			// Use Fixed colour set (or read 
														// redraw colours from Windows settings)


#define MyIdlePriority (IDLEPRIORITY_LOW)


/********************************************************************************************

>	void SuperGallery::InitData(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Purpose:	Internal method to initialise data mambers. Shared code for constructors.

********************************************************************************************/

void SuperGallery::InitData(void)
{
	GallerySize			= wxSize(0,0);

	DisplayTree			= NULL;
	DisplayMode			= 0;

	FormatIsCached		= FALSE;

	LastWindowDX = LastWindowDY = 0;

	AmShaded			= FALSE;
	AmRedrawing			= FALSE;

	LastSelectedNode	= NULL;

	CurrentOptionsDlg			= NULL;
	CurrentSortDlg				= NULL;
	CurrentSearchDlg			= NULL;
	CurrentLinePropertiesDlg	= NULL;

	DblClickPending		= FALSE;		// We aren't in the middle of a double click

	PendingRedraws		= 0;
	LastBGNode			= NULL;

	for (INT32 i = 0; i < MaxSGSortKeys; i++)
	{
		SortKeys[i].SortKey  = 0;
		SortKeys[i].Reversed = FALSE;
	}
}



/********************************************************************************************

>	SuperGallery::SuperGallery(CCRuntimeClass *Class = CC_RUNTIME_CLASS(DialogOp)): DialogOp(_R(IDD_BLANKBAR), MODELESS,0,SAFECLASS(Class))
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Purpose:	SuperGallery constructor (Identical to DialogOp)

********************************************************************************************/

SuperGallery::SuperGallery(CCRuntimeClass *Class): DialogOp(_R(IDD_BLANKBAR), MODELESS,0,SAFECLASS(Class)) 
{
	String_32 str = String_32(_R(IDS_K_BARS_NONAME));
	Name=str;
	InitData();
} 



/********************************************************************************************

>	SuperGallery::SuperGallery(String_32 &NewName,CCRuntimeClass *Class = CC_RUNTIME_CLASS(DialogOp): 
	DialogOp(NewName, Class) 

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Purpose:	SuperGallery constructor (Identical to DialogOp)

********************************************************************************************/

SuperGallery::SuperGallery(String_32 &NewName,CCRuntimeClass *Class)
		: DialogOp(_R(IDD_BLANKBAR), MODELESS,0,SAFECLASS(Class))
{
	String_32 str = String_32(_R(IDS_K_BARS_NONAME));
	Name=str;
	InitData();
} 



/********************************************************************************************

>	SuperGallery::~SuperGallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Purpose:	SuperGallery destructor.

********************************************************************************************/

SuperGallery::~SuperGallery()
{
	if (DisplayTree != NULL)
	{
		DisplayTree->DestroySubtree();	// Destroy the tree, destructing all nodes except root
		delete DisplayTree;				// And delete the root as well
		DisplayTree = NULL;
	}

	CloseOwnedDialogs();

	// Remove any idle event processor we had registered for BG redraws
	GetApplication()->RemoveIdleProcessor(MyIdlePriority, this);
}



/********************************************************************************************

>	void SuperGallery::FillInMiscInfo(SGMiscInfo *MiscInfo,
										ReDrawInfoType *DlgRedrawInfo = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		DlgRedrawInfo - If you happen to have one of these lying around, pass
				it in to save me having to find this out again.

	Outputs:	MiscInfo  - will be filled in, ready to be passed to SG event handlers

	Purpose:	Sets up a MiscInfo structure, ready for use.

	Scope:		private (Internal shared code for SuperGallery)

********************************************************************************************/

void SuperGallery::FillInMiscInfo(SGMiscInfo *MiscInfo, ReDrawInfoType *DlgRedrawInfo)
{
	// If the RedrawInfo was not passed in for us, go and get it for ourselves
	ReDrawInfoType TempDlgRedrawInfo;
	if (DlgRedrawInfo == NULL)
	{
		DlgRedrawInfo = &TempDlgRedrawInfo;
		GetKernelRenderedGadgetInfo(GetListGadgetID(), DlgRedrawInfo);
	}

	// Fill in the MiscInfo
	MiscInfo->DisplayMode	= DisplayMode;
	MiscInfo->PixelSize		= 72000 / DlgRedrawInfo->Dpi;
	MiscInfo->MaxWidth		= DlgRedrawInfo->dx;		// Window/virtualsize width
	MiscInfo->WindowHeight	= DlgRedrawInfo->dy;		// Window height
}



/********************************************************************************************

>	void SuperGallery::BroadcastEvent(SGEventType EventType, void *EventInfo,
										SGMiscInfo *MiscInfo = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		EventType - the type of the event to broadcast
				EventInfo - the information to pass in 'EventInfo' to all event handlers
							(FormatInfo/RedrawInfo/MouseInfo, etc, depending upon EventType)
				MiscInfo  - the MiscInfo to pass to the event handlers. If this is passed in
							as NULL, a MiscInfo structure will be set up automatically and
							passed in (as MiscInfo must *always* be supplied)

	Purpose:	Passes the given event through the DisplayTree.

	SeeAlso:	SGDisplayNode::HandleEvent

********************************************************************************************/

void SuperGallery::BroadcastEvent(SGEventType EventType, void *EventInfo,
									SGMiscInfo *MiscInfo)
{
	if (DisplayTree == NULL)
		return;

	SGMiscInfo TheMiscInfo;

	if (MiscInfo == NULL && IsVisible())
	{
		FillInMiscInfo(&TheMiscInfo);
		MiscInfo = &TheMiscInfo;
	}

	ERROR3IF(MiscInfo == NULL && (EventType != SGEVENT_THUMBMSG && EventType != SGEVENT_BGFLUSH), 
			"Attempt to broadcast an event when the gallery is closed could be dangerous. Tell Jason");

	DisplayTree->HandleEvent(EventType, EventInfo, MiscInfo);
}



/********************************************************************************************

>	virtual MsgResult SuperGallery::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Inputs:		Message - The message to handle

	Purpose:	A standard message handler, really. If you override this in a derived
				class, you must pass any unhandled event on to this base class method
				after handling the messages you are interested in.

	SeeAlso:	DialogOp::Message

********************************************************************************************/

MsgResult SuperGallery::Message(Msg* Message)
{
	if (MESSAGE_IS_A(Message,DeathMsg)) 	// We must destroy the dialog if the application is dying
	{
		// Destroy the entire DisplayList (which should be empty by now),
		// just to be on the safe side, in case a derived gallery forgot to do it
		if (DisplayTree != NULL)
		{
			DisplayTree->DestroySubtree();	// Destroy the display tree, destructing all nodes
			delete DisplayTree;
			DisplayTree = NULL;
		}

		// Ensure that all doc/view pointers for this Op are NULL to stop
		// 'FlushRedraw' access violations occurring.
		pOurDoc = NULL;
		pOurView = NULL;

		End();
		return(OK);
	}

		
	if ( Message->IsKindOf(CC_RUNTIME_CLASS(DialogMsg)) &&
			((DialogMsg*)Message)->DlgMsg == DIM_BAR_DEATH )
	{
		DialogOp::Message(Message);
		return(OK);
	}

	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		BOOL WasRightButton = FALSE;		// What type of click was it (if any)
										// (allows code-sharing in the case statement)

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				break;

			case DIM_CANCEL:			// We're closing - close our owned dlgs
				CloseOwnedDialogs();

				// Ensure all items pending background redraws are flushed
				FlushBackgroundRedraws();
				
				// tell everyone the screen will be changing
				TRACEUSER( "Diccon", _T("Gallery closing\n"));
				BROADCAST_TO_ALL(ScreenChangeMsg());
				break;

			case DIM_MOUSEWHEEL_UP:
				{
					//MouseWheel Scrolled Upwards...
					TRACEUSER( "Matt", _T("SuperGallery::Message() Received MouseWheel UPWARDS Message!\n"));

					ReDrawInfoType DlgRedrawInfo;
					GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

					// Make a SGMiscInfo with, well, MiscInfo in it...
					SGMiscInfo MiscInfo;
					FillInMiscInfo(&MiscInfo, &DlgRedrawInfo);

					//Get the old scroll offset, adjust it accordingly and set it to be the current one...
					INT32 newScrollOffset = DisplayTree->GetScrollOffset() - (750 * 30);
					DisplayTree->SetScrollOffset(newScrollOffset, &MiscInfo);
				}
				break;

			case DIM_MOUSEWHEEL_DOWN:
				{
					//MouseWheel Scrolled Downwards...
					TRACEUSER( "Matt", _T("SuperGallery::Message() Received MouseWheel DOWNWARDS Message!\n"));

					ReDrawInfoType DlgRedrawInfo;
					GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

					// Make a SGMiscInfo with, well, MiscInfo in it...
					SGMiscInfo MiscInfo;
					FillInMiscInfo(&MiscInfo, &DlgRedrawInfo);

					//Get the old scroll offset, adjust it accordingly and set it to be the current one...
					INT32 newScrollOffset = DisplayTree->GetScrollOffset() + (750 * 30);
					DisplayTree->SetScrollOffset(newScrollOffset, &MiscInfo);
				}
				break;

			case DIM_RGT_BN_DOWN:
				WasRightButton = TRUE;
				// (Note: We treat a right click like a left click now. DIM_RGT_BN_UP is used
				// to pop up the menu when the button is released)

				// Drop through to LFT_BN_DOWN handler...

			case DIM_LFT_BN_DOWN:
				if ((Msg->GadgetID == GetListGadgetID()) && Msg->DlgMsgParam)
				{
					// Get our kernel-rendered-dlg info, and calculate our virtual coord space
					ReDrawInfoType *DlgRedrawInfo = (ReDrawInfoType*) Msg->DlgMsgParam;

					// Determine where the mouse was clicked, within the virtual coord space
					SGMouseInfo ClickInfo;

					// Was it a ctrl-click? (Adjust/right clicks now pop up the menu)
					ClickInfo.Adjust = KeyPress::IsGalleryCtrlPressed();

					// Was it a click to extend the selection? (shift-click)
					ClickInfo.Extend = KeyPress::IsGalleryShiftPressed();

					// Was it a pop-up-menu (right button) click?
					ClickInfo.MenuClick = WasRightButton;

					// Is it the second installment in a double-click?
					ClickInfo.DoubleClick = FALSE;
					if (DblClickPending && !WasRightButton)
					{
						// Double-click Distance (millipoints). If the second click is more than this
						// distance away from the forst click point, it is not regarded as a dble click.
						const INT32 DblClickDist = 12000;		// **** !!!! Read dbl-click-dist preference
						INT32 ClickDistX = ABS(LastClickPos.x - DlgRedrawInfo->pMousePos->x);
						INT32 ClickDistY = ABS(LastClickPos.y - DlgRedrawInfo->pMousePos->y);
						
						if (ClickDistX <= DblClickDist && ClickDistY <= DblClickDist)
						{
							const INT32 DblClickTime = 400;		// **** !!!! Read OS dbl-click time!

							if (!LastClickTime.Elapsed(DblClickTime))
								ClickInfo.DoubleClick= TRUE;
						}
					}
					LastClickPos = *(DlgRedrawInfo->pMousePos);	// Remember last click position

					if (ClickInfo.DoubleClick)
						DblClickPending = FALSE;	// No longer pending double click
					else
					{
						DblClickPending = TRUE;		// Double click is now pending
						LastClickTime.Sample();		// Start the "double-click timeout" timer
					}

					ClickInfo.Position = *(DlgRedrawInfo->pMousePos);
					ConvertToVirtualCoords(DlgRedrawInfo, &ClickInfo.Position);

					// And request that the tree decides what to do with the event
					BroadcastEvent(SGEVENT_MOUSECLICK, &ClickInfo);
				}
				break;


			case DIM_RGT_BN_UP:
				{
					// Give the gallery a chance to do something before popping up the context menu
					// At present, used by the bitmap gallery to ensure that the plug-ins have all
					// been parsed.
					PreContextMenu();
					// Right button has been released - pop up a context-sensitive menu
					GalleryContextMenu *TheMenu = new GalleryContextMenu(SGMENU_OVERITEM, this);
					if (TheMenu != NULL)
						TheMenu->Show();
				}
				break;


			case DIM_LFT_BN_CLICKED:
			{
				if (Msg->GadgetID == _R(IDC_GALLERY_NEW))
				{
					ApplyAction(SGACTION_CREATE);
				}
				else if (Msg->GadgetID == _R(IDC_GALLERY_APPLY))
				{
					// NB. changed to use Control key here, to be consistent with the
					// way Adjust works when clicking on items.
					if (KeyPress::IsGalleryCtrlPressed())
					{
						// If the gallery doesn't respond to Adjust-click then try a plain
						// click instead.
						if (!ApplyAction(SGACTION_APPLYADJUST)) ApplyAction(SGACTION_APPLY);
					}
					else
					{
						ApplyAction(SGACTION_APPLY);
					}
				}
				else if (Msg->GadgetID == _R(IDC_GALLERY_REDEFINE))
				{
					ApplyAction(SGACTION_REDEFINE);
				}
				else if (Msg->GadgetID == _R(IDC_GALLERY_EDIT))
				{
					ApplyAction(SGACTION_EDIT);
				}
				else if (Msg->GadgetID == _R(IDC_GALLERY_DELETE))
				{
					ApplyAction(SGACTION_DELETE);
				}
				else if (Msg->GadgetID == _R(IDC_GALLERY_MENU))
				{
					// Pop up the options menu/dialogue
//					SGalleryOptionsDlg::InvokeDialog(this);
					GalleryContextMenu *TheMenu = new GalleryContextMenu(SGMENU_OPTIONS, this);
					if (TheMenu != NULL)
						TheMenu->Show();
				}
				break;
			}
			case DIM_RGT_BN_CLICKED:
				if (Msg->GadgetID == _R(IDC_GALLERY_APPLY))
				{
					if (!ApplyAction(SGACTION_APPLYADJUST))
						ApplyAction(SGACTION_APPLY);
				}
				break;


			case DIM_REDRAW:
				if (Msg->GadgetID == _R(IDC_GALLERY_LISTBOX))		// Render the list box
				{
					RenderListBox((ReDrawInfoType*) Msg->DlgMsgParam);
				}
				break;
			
			default:
				break;
		}
	}
	else if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *Msg = (DocChangingMsg *) Message;
		switch (Msg->State)
		{
			case DocChangingMsg::TITLECHANGED:
				if (DisplayTree != NULL && IsVisible())
				{
					// If we have any 'categories' which represent this document,
					// ask them to redraw themselves to update their titles.
					SGDisplayNode *Category =
						DisplayTree->FindSubtree(this, Msg->pChangingDoc, NULL);

					if (Category != NULL)
						Category->ForceRedrawOfMyself();
				}
				break;


			case DocChangingMsg::ABOUTTODIE:		// Document dying - remove from tree
				if (DisplayTree != NULL)
				{
					// If the gallery has a subtree for this document, we'll scrap it for 'em
					SGDisplayNode *Ptr = DisplayTree->FindSubtree(this, Msg->pChangingDoc, NULL);
					if (Ptr != NULL)
					{
						Ptr->DestroySubtree();
						ForceRedrawOfList();
					}
				}
				break;


			case DocChangingMsg::SELCHANGED:
				if (Msg->pNewDoc == NULL)
				{
					// There is no selected doc - this can only mean there are no docs
					// at all, so we had better shade the gallery
					ShadeGallery(TRUE);
				}
				break;

			default:
				break;
		}
	}

	else if (MESSAGE_IS_A(Message, DragMessage) && IsVisible() && DisplayTree != NULL)
	{
		// If a drag starting message comes around, pass it on to the tree
		DragMessage *Msg = (DragMessage *) Message;
		if (Msg->State == DragMessage::DRAGSTARTED)
		{
			BroadcastEvent(SGEVENT_DRAGSTARTED, (SGEventInfo *) Msg);

			// And then call our virtual function to add a drag target (if appropriate)
			// for the entire gallery listbox.
			HandleDragStart(Msg);
		}
	}

	else if (MESSAGE_IS_A(Message, ThumbMessage) && DisplayTree != NULL)
	{
		// If a library Thumb message comes around, pass it on to the tree
		// (do this even if the gallery is closed, as libraries may cache stuff even when closed)
		BroadcastEvent(SGEVENT_THUMBMSG, (SGEventInfo *) Message);
	}
	
	// Pass the call down to the base class
	return(DialogOp::Message(Message));

	return OK;
}    



/********************************************************************************************

>	BOOL SuperGallery::Create(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Returns:	TRUE if the window was successfully created
				FALSE => ERROR2
	Purpose:	The SuperGallery Create method
				This method has been overridden to de-specialize the DialogOp. 

	Notes:		Before doing anything else, the PreCreate handler is called.
				Secondly, the InitMenuCommands() method is called to init any
				required gallery menu items.
				Then, the gallery window is created and initialised
				The last thing done is to invoke the PostCreate method
				(these upcalls should be overridden by derived classes to do stuff)

				This finally calls SelectionHasChanged() to ensure the buttons etc
				are updated appropriately to the selection state.

********************************************************************************************/

BOOL SuperGallery::Create(void)
{
	if (!DialogOp::Create())
		return(FALSE);

	// Call derived class PreCreate handler (to create display tree, usually)
	if (!PreCreate())
		return(FALSE);

	// Call the derived class Menu creation handler, to create any needed menu items
	if (!InitMenuCommands())
		return(FALSE);

	// Ensure any items pending background-redraws are 'flushed'
	FlushBackgroundRedraws();

#ifndef EXCLUDE_FROM_XARALX
PORTNOTE("galleries", "Removed docking stuff")
	// Bar initialisation and creation
	SetDockBarType(Dock);
	SetSlot(Slot);
	SetOffset(Offset);
	SetFloatingCPoint(FloatPos);
#endif

//	if (!DialogOp::Create())
//		return(FALSE);

	AmShaded = FALSE;

	// Call derived class PostCreate handler
	if (!PostCreate())
		return(FALSE);

	SelectionHasChanged();		// Ensure we update appropriately for the selection state

	return(TRUE);
}



/********************************************************************************************

>	virtual void SuperGallery::SetVisibility(BOOL Open)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		Open - 	if TRUE then open SuperGallery in its last pos if it is
						currently closed.
						if FALSE, it is closed

	Purpose:	Used by the tool bar customize system to open and close a tool bar.

				SuperGallery overrides this virtual function so that it can be aware
				of when its window is closed/hidden, so that it may remove its owned
				dialogues (sort/search/properties).

				It always calls the base class in order to show/hide the gallery

********************************************************************************************/

void SuperGallery::SetVisibility(BOOL Open)
{
	if (!Open)
	{
		// If we're being hidden, make sure that any owned dialogues are closed
		CloseOwnedDialogs();
	}	

	// Now call the base class to show/hide ourselves
	DialogOp::SetVisibility(Open);
}


/********************************************************************************************

>	virtual void SuperGallery::CloseOwnedDialogs(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/03/97

	Purpose:	Checks and closes the various dialogs which can be owned by galleries.

	SeeAlso:	SuperGallery::SetVisibility, SuperGallery::~SuperGallery()
				SuperGallery::Message(Msg* Message)

********************************************************************************************/

void SuperGallery::CloseOwnedDialogs(void)
{
	if (CurrentOptionsDlg != NULL)
	{
		CurrentOptionsDlg->Close();
		CurrentOptionsDlg->End();
	}

	if (CurrentSortDlg != NULL)
	{
		CurrentSortDlg->Close();
		CurrentSortDlg->End();
	}

	if (CurrentSearchDlg != NULL)
	{
		CurrentSearchDlg->Close();
		CurrentSearchDlg->End();
	}

PORTNOTE("dialogs", "Removed CurrentLinePropertiesDlg from SuperGallery::CloseOwnedDialogs(void)")
#ifndef EXCLUDE_FROM_XARALX
	if (CurrentLinePropertiesDlg != NULL)
	{
		CurrentLinePropertiesDlg->Close();
		CurrentLinePropertiesDlg->End();
	}
#endif
}

/********************************************************************************************

>	virtual BOOL SuperGallery::PreCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94
	Returns:	TRUE if the gallery was successfully initialised

	Purpose:	A call to be overridden by the derived class.
				This is called BEFORE the SuperGallery::Create function tries to create
				the gallery window.
				Derived galleries then initialise any unintialised data, and return
				TRUE if it is safe to continue creating the gallery window.

	Notes:		Generally speaking, most initialisation code should go into the post create
				handler method. Override this method with care.

	SeeAlso:	SuperGallery::PostCreate

********************************************************************************************/

BOOL SuperGallery::PreCreate(void)
{
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL SuperGallery::PostCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94
	Returns:	TRUE if the gallery was successfully initialised
	Purpose:	A call to be overridden by the derived class.
				This is called AFTER the SuperGallery::Create function has succeeded.
				Derived galleries can then initialise any unintialised data, and return
				TRUE if it is safe to open the gallery window.

	SeeAlso:	SuperGallery::PreCreate

********************************************************************************************/

BOOL SuperGallery::PostCreate(void)
{
	return(TRUE);
}



/********************************************************************************************

>	void SuperGallery::ShadeGallery(BOOL ShadeIt)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		ShadeIt - TRUE to shade, FALSE to un-shade, the gallery

	Purpose:	Shades (disables) the SuperGallery window.
				This calls the DoShadeGallery upcall to allow derived classes to take
				special action for any extra controls they may own, etc.

				If un-shading the gallery, it also calls SelectionHasChanged method to
				ensure that the buttons are in an appropriate state for the selection.

	Notes:		If the gallery is already in the requested state, or if it is closed
				(not visible on screen) then nothing happens (other than that the shaded
				flag is updated to reflect the desired state)

	SeeAlso:	SuperGallery::DoShadeGallery; SuperGallery::SelectionHasChanged

********************************************************************************************/

void SuperGallery::ShadeGallery(BOOL ShadeIt)
{
	if (AmShaded != ShadeIt && IsVisible())
	{
		EnableGadget(_R(IDC_GALLERY_APPLY),		!ShadeIt);
		EnableGadget(_R(IDC_GALLERY_NEW),		!ShadeIt);
		EnableGadget(_R(IDC_GALLERY_DELETE),	!ShadeIt);
		EnableGadget(_R(IDC_GALLERY_REDEFINE),	!ShadeIt);
		EnableGadget(_R(IDC_GALLERY_EDIT),		!ShadeIt);
//		EnableGadget(_R(IDC_GALLERY_UNDO),		!ShadeIt);
		EnableGadget(_R(IDC_GALLERY_MENU),		!ShadeIt);
		EnableGadget(_R(IDC_GALLERY_NAME),		!ShadeIt);

		DoShadeGallery(ShadeIt);
	}

	AmShaded = ShadeIt;

	if (!AmShaded)					// If unshading, ensure the buttons are updated
		SelectionHasChanged();		// to reflect the new selection state
}



/********************************************************************************************

>	virtual void SuperGallery::DoShadeGallery(BOOL ShadeIt);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		ShadeIt - TRUE to shade, FALSE to un-shade, the gallery

	Purpose:	Shades (disables) the SuperGallery window.
				This calls the DoShadeGallery upcall to allow derived classes to take
				special action for any extra controls they may own, etc.

	Notes:		This base class method does nothing - it is expected to be overridden if
				derived classes need to do anything when shading state changes

	SeeAlso:	SuperGallery::DoShadeGallery

********************************************************************************************/

void SuperGallery::DoShadeGallery(BOOL ShadeIt)
{
	// The base class does nothing
}



/********************************************************************************************

>	virtual void SuperGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag which is just starting.

	Purpose:	Checks a DragMessage to see if it is a gallery list-drag. If so, it
				adds a new drag target to handle drops on the gallery list box.

	Notes:		This method should be overridden by derived galleries to:
				a) Stop the gallery from allowing recieving of these drags, or
				b) Add handlers for specific drag types you're interested in - e.g.
				the colour gallery will use less generic colour drags to handle
				gallery reorganisation, so that you can also drag colours out of the
				gallery.

********************************************************************************************/

void SuperGallery::HandleDragStart(DragMessage *DragMsg)
{
	// If this is a gallery list-reorganising drag which originated from this gallery,
	// then create a target for the gallery listbox.
	if (DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(SGListDragInfo)) &&
		((SGListDragInfo *)DragMsg->pInfo)->GetParentGallery() == this)
	{
		// Note this sort of auto-attaches it seems - AMB
		/*SGListDragTarget *NewTarget = */new SGListDragTarget(this, GetListGadgetID());
	}
}



/********************************************************************************************

>	virtual BOOL SuperGallery::ApplyAction(SGActionType Action)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		Action - Indicates what action to apply

	Returns		FALSE (Derived classes will return TRUE to indicate successful handling
				of the action, or FALSE to indicate failure)

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	Notes:		This base-class method should be overridden by derived classes. It
				does nothing by default, and returns FALSE.

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL SuperGallery::ApplyAction(SGActionType Action)
{
	return(FALSE);
}



/********************************************************************************************

>	static INT32 __cdecl SuperGallery::SortComparator(const void *Item1, const void *Item2)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Inputs:		Item1, Item2 - the display nodes to be compared

	Returns:	a negative, zero, or positive result of comparing the items with the
				current gallery multi-key sort mode.

	Purpose:	'qsort' comparator function, used when quicksorting the display list

	Notes:		Relies on the static var. 'CurrentSortGallery' to contain a pointer to
				the gallery which requested this sort.

	SeeAlso:	SuperGallery::ApplySortNow

********************************************************************************************/

INT32 __cdecl SuperGallery::SortComparator(const void *Item1, const void *Item2)
{
	SGDisplayNode *Node1 = *((SGDisplayNode **)Item1);
	SGDisplayNode *Node2 = *((SGDisplayNode **)Item2);

	INT32 Result = Node1->CompareTo(Node2, CurrentSortGallery->SortKeys[0].SortKey);
	if (CurrentSortGallery->SortKeys[0].Reversed)
		Result = -Result;

	if (Result == 0)
	{
		Result = Node1->CompareTo(Node2, CurrentSortGallery->SortKeys[1].SortKey);
		if (CurrentSortGallery->SortKeys[1].Reversed)
			Result = -Result;
	}

	return(Result);
}



/********************************************************************************************

>	virtual void SuperGallery::ApplySortNow(BOOL ApplyToEntireList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Inputs:		ApplyToEntireList - TRUE to sort the entire DisplayList,
				FALSE to sort only groups which contain a selection

	Purpose:	Applies the current gallery sort mode (member array SortKeys) to the
				display list.

	SeeAlso:	SuperGallery::SortComparator; ::qsort

********************************************************************************************/

void SuperGallery::ApplySortNow(BOOL ApplyToEntireList)
{
	if (SortKeys[0].SortKey == 0 || DisplayTree == NULL)
		return;
	
	CurrentSortGallery = this;

	SGDisplayNode *CurrentGroup = DisplayTree->GetChild();
	if (CurrentGroup == NULL)	// Nothing to sort
		return;

	SGDisplayNode *Ptr = CurrentGroup;

	// Pre-run progress indicator
	String_64 ProgMsg(_R(IDS_GALLERY_PREPARE_FOR_SORT));
	BeginSlowJob(-1, FALSE, &ProgMsg);

	// For library galleries we need to set the Quiet button status ready to un-supress errors
	SetQuietStatus(FALSE);

	// Count the number of available items to sort
	BOOL HasSelection = FALSE;
	INT32 NumItemsToSort = 0;
	while (Ptr != NULL)
	{
		if(ApplyToEntireList || ((SGDisplayGroup *)Ptr)->IsSelected())
			if(((SGDisplayGroup *)Ptr)->IsVirtualised())
				((SGDisplayGroup *)Ptr)->DeVirtualise();

		if (ApplyToEntireList || ((SGDisplayGroup *)Ptr)->FindNextSelectedItem(NULL) != NULL)
		{
			SGDisplayNode *Item = Ptr->GetChild();	// Count number of items to sort in this group
			INT32 ItemsInThisGroup = 0;
			while (Item != NULL)
			{
				ItemsInThisGroup++;
				if (Item->Flags.Selected)
					HasSelection = TRUE;
				Item = Item->GetNext();
			}

			if (ItemsInThisGroup > 1 && (ApplyToEntireList || HasSelection))
				NumItemsToSort += ItemsInThisGroup;
		}
		Ptr = Ptr->GetNext();
	}

	EndSlowJob();

	// If there is no point in trying to sort, abort now
	if (NumItemsToSort < 2)
		return;

	// Start progress indicators, with a percentage based upon the number of items.
	// We will update twice for each group (after qsort and shuffle-items stages)
	String_64 Description(_R(IDS_SGOPTS_SORTING));
	BeginSlowJob(NumItemsToSort * 2, FALSE, &Description);
	NumItemsToSort = 0;

	INT32 NumItems;
	INT32 i;
	BOOL GroupHasChanged = FALSE;

	while (CurrentGroup != NULL)
	{
		// Don't sort virtualised-out groups ! (they should have all been devirtualised before)
		if(!((SGDisplayGroup *)CurrentGroup)->IsVirtualised())
		{
			HasSelection = FALSE;
			Ptr = CurrentGroup->GetChild();
			ERROR3IF(Ptr != NULL && !Ptr->IsKindOf(CC_RUNTIME_CLASS(SGDisplayItem)),
						"Sort hasn't found items! Heinous failure imminent!" );
			// Count the number of items we have to sort
			NumItems = 0;
			while (Ptr != NULL)
			{
				NumItems++;
				if (Ptr->Flags.Selected)
					HasSelection = TRUE;
				Ptr = Ptr->GetNext();
			}

			// If there are 2 or more items, and we are either applying to all groups, or this
			// group contains selected item(s), then we will sort it..
			if (NumItems > 1 && (ApplyToEntireList || HasSelection))
			{
				// Get memory for an array of pointers to these items
				SGDisplayNode **SortArray = (SGDisplayNode **)CCMalloc(NumItems * sizeof(SGDisplayNode *));
				if (SortArray == NULL)
				{
					CurrentSortGallery = NULL;
					InvalidateCachedFormat();
					EndSlowJob();
					InformError();
					return;
				}

				// Fill in the array with pointers to display items to sort
				i = 0;
				Ptr = CurrentGroup->GetChild();
				while (Ptr != NULL)
				{
					SortArray[i++] = Ptr;
					Ptr = Ptr->GetNext();
				}

				// Sort the array of pointers
				qsort(SortArray, NumItems, sizeof(SGDisplayNode *), SuperGallery::SortComparator);

				NumItemsToSort += NumItems;
				if (!ContinueSlowJob(NumItemsToSort)) // Update percentage complete for the number of items processed
				{
					// User has cancelled by pressing escape
					CCFree(SortArray);
					break;
				}


				GroupHasChanged = FALSE;

				// Now, take the sorted array, and rearrange the display items to be in that order		
				// Special case the first item
				if (SortArray[0]->GetPrevious() != NULL)
				{
					SortArray[1]->MoveBefore(SortArray[0]);
					GroupHasChanged = TRUE;
				}

				// Then whip through the rest of the items
				for (i = 1; i < NumItems; i++)
				{
					if (SortArray[i]->GetPrevious() != SortArray[i-1])
					{
						SortArray[i-1]->MoveAfter(SortArray[i]);
						GroupHasChanged = TRUE;
					}
				}

				// Free our temporary workspace
				CCFree(SortArray);

				// Tell the derived gallery that the items in this group have been moved about
				if (GroupHasChanged)
					AllItemsCopied((SGDisplayGroup *)CurrentGroup);

				NumItemsToSort += NumItems;
				if (!ContinueSlowJob(NumItemsToSort))
					break; 	// Update percentage complete for the number of items processed, and quit if aborted
			}
		}
		
		// And go on to the next group
		CurrentGroup = CurrentGroup->GetNext();
	}

	CurrentSortGallery = NULL;

	EndSlowJob();
	
	InvalidateCachedFormat();
	ReformatAndRedrawIfNecessary();
}


/********************************************************************************************

>	virtual BOOL SuperGallery::GetQuietStatus(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if Quiet has been pressed (SetQuiet status called with TRUE)
	Purpose:	Get the Quiet status of the gallery - used when a load of indexes are being
				added to the gallery and there are problems...

********************************************************************************************/

BOOL SuperGallery::GetQuietStatus(void)
{
	return FALSE;
}

/********************************************************************************************

>	virtual void SuperGallery::SetQuietStatus(BOOL Status)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Inputs:		Status - Set to TRUE and call if Quiet has just been pressed on an 'adding
						 indexes' dialog
	Purpose:	Set the Quiet status of the gallery - used when a load of indexes are being
				added to the gallery and there are problems...

********************************************************************************************/
		
void SuperGallery::SetQuietStatus(BOOL Status)
{
}

/********************************************************************************************

>	virtual BOOL SuperGallery::IsLibraryGallery(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if the SuperGallery is a LibraryGallery
	Purpose:	To check if we're a library gallery

********************************************************************************************/

BOOL SuperGallery::IsLibraryGallery(void)
{
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL SuperGallery::IsLibraryGalleryWithNonLibraryGroups(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Returns:	TRUE if the SuperGallery is a LibraryGallery, yet also can contain non-library
				groups
	Purpose:	To check if we're a dedicated library gallery, or a half+half (like the font
				gallery)

********************************************************************************************/

BOOL SuperGallery::IsLibraryGalleryWithNonLibraryGroups(void)
{
	return FALSE;
}



/********************************************************************************************

>	void SuperGallery::CalcVirtualSize(ReDrawInfoType *DlgRedrawInfo,
										DocRect *VirtualSize, INT32 *PixelSize)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95

	Inputs:		DlgRedrawInfo - The kernel-rendered-dilaogue info struct passed into your
				redraw or mouse handler.

	Outputs:	VirtualSize - Returned containing the virtual coordinate rectangle for
				the visible portion of the gallery displaylist.

				PixelSize - Returned containing the size of an output device pixel, as
				it maps into millipoints in the virtual space.

	Purpose:	Shared internal code to determine the virtual coordinate space of the region
				of the display list which is visible n screen.

	SeeAlso:	SuperGallery::RenderListBox

********************************************************************************************/

void SuperGallery::CalcVirtualSize(ReDrawInfoType *DlgRedrawInfo,
									DocRect *VirtualSize, INT32 *PixelSize)
{
	ERROR3IF(DlgRedrawInfo == NULL || VirtualSize == NULL || PixelSize == NULL,
				"SuperGallery::CalcVirtualSize - NULL parameters are illegal");

	// Calculate the size of a screen display pixel in terms of virtual space coords
	*PixelSize = 72000 / DlgRedrawInfo->Dpi;

	// Create our viewport. We specify 0,0 at the top left corner of the window, with
	// x increasing to the right, and y increasing upwards. This, all y coordinates 
	// within the redrawn area are actually negative.
	// Coordinates are in millipoints, and no scaling is desired (i.e. if we say 20pt,
	// we mean 20pt on screen).
	*VirtualSize = DocRect(0, -DlgRedrawInfo->dy, DlgRedrawInfo->dx, 0);

	// Now, shift the coordinate space down to the correct scroll offset position, so
	// we map the correct part of the window extent into the visible portion of the window
	if (DisplayTree != NULL)
		VirtualSize->Translate(0, -DisplayTree->GetScrollOffset());
}



/********************************************************************************************

>	void SuperGallery::ConvertToVirtualCoords(ReDrawInfoType *DlgRedrawInfo,
												DocCoord *SourceAndResult)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		DlgRedrawInfo   - The kernel-rendered-dialogue information
				SourceAndResult - The coordinate to be converted

	Ouputs:		SourceAndResult - The resulting coordinate

	Purpose:	Converts a standard DocCoord, as passed in for kernel-rendered dialogues,
				into a standard Display-list virtual coordinate, as used by Display items.

********************************************************************************************/

void SuperGallery::ConvertToVirtualCoords(ReDrawInfoType *DlgRedrawInfo,
											DocCoord *SourceAndResult)
{
	ERROR3IF(SourceAndResult == NULL || DlgRedrawInfo == NULL,
				"SuperGallery::ConvertToVirtualCoords - NULL parameters are illegal");

	INT32 ScrollOffset = 0;
	if (DisplayTree != NULL)
		ScrollOffset = DisplayTree->GetScrollOffset();

	// x has a 1:1 mapping into our virtual space, so we don't touch it
	SourceAndResult->y = (SourceAndResult->y - DlgRedrawInfo->dy) - ScrollOffset;
}



/********************************************************************************************

>	void SuperGallery::ConvertToVirtualCoords(SGMiscInfo *MiscInfo,
												DocCoord *SourceAndResult)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/95

	Inputs:		MiscInfo   - The kernel-rendered-dialogue information passed to SG
				Display Tree event handlers
				SourceAndResult - The coordinate to be converted

	Ouputs:		SourceAndResult - The resulting coordinate

	Purpose:	Converts a standard DocCoord, as passed in for kernel-rendered dialogues,
				into a standard Display-list virtual coordinate, as used by Display items.

********************************************************************************************/

void SuperGallery::ConvertToVirtualCoords(SGMiscInfo *MiscInfo,
											DocCoord *SourceAndResult)
{
	ERROR3IF(SourceAndResult == NULL || MiscInfo == NULL,
				"SuperGallery::ConvertToVirtualCoords - NULL parameters are illegal");

	INT32 ScrollOffset = 0;
	if (DisplayTree != NULL)
		ScrollOffset = DisplayTree->GetScrollOffset();

	// x has a 1:1 mapping into our virtual space, so we don't touch it
	SourceAndResult->y = (SourceAndResult->y - MiscInfo->WindowHeight) - ScrollOffset;
}


/********************************************************************************************

>	BOOL SuperGallery::ConvertFromVirtualCoords(SGMiscInfo *MiscInfo, DocRect *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95

	Inputs:		MiscInfo   - The kernel-rendered-dialogue information passed to SG
				Display Tree event handlers
				SourceAndResult - The rect to be converted

	Ouputs:		SourceAndResult - The resulting rect. This will NOT be clipped to the visible
				area of the display list.

	Returns:	TRUE if the rectangle is at least partially visible.

	Purpose:	Converts a standard Display-list virtual rectangle, as used by Display items,
				into a rect as used in kernel-rendered dialogues.
				The returned rectangle may be outside the visible display area of the
				gadget.

********************************************************************************************/

BOOL SuperGallery::ConvertFromVirtualCoords(SGMiscInfo *MiscInfo, DocRect *SourceAndResult)
{
	ERROR3IF(SourceAndResult == NULL || MiscInfo == NULL,
				"SuperGallery::ConvertToVirtualCoords - NULL parameters are illegal");

	INT32 ScrollOffset = 0;
	if (DisplayTree != NULL)
		ScrollOffset = DisplayTree->GetScrollOffset();

	// x has a 1:1 mapping into our virtual space, so we don't touch it
	SourceAndResult->lo.y = (SourceAndResult->lo.y + MiscInfo->WindowHeight) + ScrollOffset;	
	SourceAndResult->hi.y = (SourceAndResult->hi.y + MiscInfo->WindowHeight) + ScrollOffset;	

	// And determine if the y extent of the coords lie within the 0..WindowHeight range
	return(SourceAndResult->hi.y >= 0 && SourceAndResult->lo.y <= MiscInfo->WindowHeight);
}



/********************************************************************************************

>	virtual RenderRegion *SuperGallery::CreateRenderRegion(DocRect *VirtualSize,
															ReDrawInfoType *DlgRedrawInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Purpose:	An overridable veneer to the DialogOp CreateOSRenderRegion method.
				This can be overriden to use a different type of renderregion for
				all of your rendering.

				If you override this, you MUST also override DestroyRenderRegion!

	SeeAlso:	SuperGallery::DestroyRenderRegion

********************************************************************************************/

RenderRegion *SuperGallery::CreateRenderRegion(DocRect *VirtualSize, ReDrawInfoType *DlgRedrawInfo)
{
	return(CreateOSRenderRegion(VirtualSize, DlgRedrawInfo));
}



/********************************************************************************************

>	virtual void SuperGallery::DestroyRenderRegion(RenderRegion *pRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Purpose:	An overridable veneer to the DialogOp DestroyOSRenderRegion method.
				This can be overriden to use a different type of renderregion for
				all of your rendering.

				If you override this, you MUST also override CreateRenderRegion!

	SeeAlso:	SuperGallery::CreateRenderRegion

********************************************************************************************/

void SuperGallery::DestroyRenderRegion(RenderRegion *pRender)
{
	DestroyOSRenderRegion(pRender);
}



/********************************************************************************************

>	void SuperGallery::RenderListBox(ReDrawInfoType *DlgRedrawInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/94
	Purpose:	Renders the displayed list of items into the 'list box' control

********************************************************************************************/

void SuperGallery::RenderListBox(ReDrawInfoType *DlgRedrawInfo)
{
	if (AmRedrawing)
	{
		ERROR3("Re-entrant redraw detected in SuperGallery::RenderListBox! (I'll ignore it)");
		return;
	}

	AmRedrawing = TRUE;		// LOCK to ensure we don't do this in a re-entrant manner

	// Initialise the MiscInfo and formatting information structure
	SGMiscInfo MiscInfo;
	FillInMiscInfo(&MiscInfo, DlgRedrawInfo);
	
	// If Format Info is not cached by the tree, or if the window size has changed,
	// then ask the tree to reformat its display items and cache the result
	if (DisplayTree != NULL)
	{
		if (!FormatIsCached ||
			LastWindowDX != DlgRedrawInfo->dx || LastWindowDY != DlgRedrawInfo->dy)
		{
			SGFormatInfo FormatInfo;
			DisplayTree->InitFormatInfo(&FormatInfo, &MiscInfo);

			// Recalculate and cache formatting information
			BroadcastEvent(SGEVENT_FORMAT, &FormatInfo, &MiscInfo);

			// And remember that the cache is now valid
			FormatIsCached = TRUE;

			// And remember new window size
			LastWindowDX = DlgRedrawInfo->dx;
			LastWindowDY = DlgRedrawInfo->dy;
		}
	}

	// Now calculate the virtual size - note that this depends upon the scroll offset
	// which may be altered by a reformat, which is why we did the Reformat above.	
	INT32	PixelSize;
	DocRect	VirtualSize;
	CalcVirtualSize(DlgRedrawInfo, &VirtualSize, &PixelSize);

	RenderRegion* pRender = CreateRenderRegion(&VirtualSize, DlgRedrawInfo);
	if (pRender!=NULL)
	{
		pRender->SaveContext();

		DialogColourInfo RedrawColours;

		if (DisplayTree == NULL)
		{
			DocColour White(COLOUR_WHITE);

			if (UseFixedColourSet)
			{
				pRender->SetFillColour(White);
				pRender->SetLineColour(White);
			}
			else
			{
				pRender->SetFillColour(RedrawColours.TextBack());
				pRender->SetLineColour(RedrawColours.TextBack());
			}
			pRender->DrawRect(&VirtualSize);	// No display tree! Fill the area with white
		}
		else
		{
			// Set up the redraw information (Note: This must be left until after formatting,
			// as the scroll offset etc may be changed by a reformat)
			// Set the bounding rectangle of the region to be redrawn from the clip rectangle,
			// so that only items that need redrawing do redraw. We then have to convert these
			// into display list virtual coordinates
			SGRedrawInfo RedrawInfo;
			RedrawInfo.Bounds = *(DlgRedrawInfo->pClipRect);
			ConvertToVirtualCoords(DlgRedrawInfo, &RedrawInfo.Bounds.lo);
			ConvertToVirtualCoords(DlgRedrawInfo, &RedrawInfo.Bounds.hi);

			RedrawInfo.Renderer = pRender;

			if (UseFixedColourSet)
			{
				// Use fixed colour scheme
				RedrawInfo.Foreground	 = DocColour(COLOUR_BLACK);
				RedrawInfo.Background	 = DocColour(COLOUR_WHITE);
				RedrawInfo.SelForeground = DocColour(COLOUR_WHITE);
				RedrawInfo.SelBackground = DocColour(0L, 0L, 128L);
			}
			else
			{
				// Read the operating-system preferences for the colour scheme
				RedrawInfo.Foreground	 = RedrawColours.TextFore();
				RedrawInfo.Background	 = RedrawColours.TextBack();
				RedrawInfo.SelForeground = RedrawColours.HighlightedTextFore();
				RedrawInfo.SelBackground = RedrawColours.HighlightedTextBack();
			}

			RedrawInfo.Transparent	 = DocColour(COLOUR_TRANS);

			// And render the tree
			BroadcastEvent(SGEVENT_REDRAW, &RedrawInfo, &MiscInfo);
		}

		pRender->RestoreContext();
		DestroyRenderRegion(pRender);
	}

	AmRedrawing = FALSE;		// And unlock the redraw once more
}


/********************************************************************************************

>	INT32 SuperGallery::GetDisplayExtent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Returns:	The minimum Y coordinate of the display list virtual coordinate space
				(The extent of the list - it will always be a negative number as the
				list starts at 0 at the top of the window, and descends)

	Purpose:	To determine the length of the displayed list, in MILLIPOINTS; actually,
				in terms of the mimimum displayed Y coordinate, which is really -(Length).

	Notes:		It will return 0 if the gallery is not visible on screen (because the
				display extent is determined by formatting the display into a given window,
				and if the window is closed, its sixze is unknown)

********************************************************************************************/

INT32 SuperGallery::GetDisplayExtent(void)
{
	if (DisplayTree == NULL || !IsVisible())
		return(0);

	SGMiscInfo	 MiscInfo;
	FillInMiscInfo(&MiscInfo);

	SGFormatInfo FormatInfo;
	DisplayTree->InitFormatInfo(&FormatInfo, &MiscInfo);

	return(DisplayTree->CalculateListExtent(&FormatInfo, &MiscInfo));
}



/********************************************************************************************

>	void SuperGallery::ForceGroupFolded(SGDisplayGroup *GroupToFold, BOOL FoldedState)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95

	Inputs:		GroupToFold - The Group to fold/unfold
				FoldedState - TRUE to fold, FALSE to unfold the group
				
	Purpose:	Ensures that the given group is in an {un}folded state.
				If it is not oin the desired state, the state is changed, and the tree
				is reformatted and invalidated for redraw.

	Notes:		This can be used to set the folded state of a group, even if it is not
				yet linked into the tree. 

	SeeAlso:	SGDisplayGroup::SetFoldedState

********************************************************************************************/

void SuperGallery::ForceGroupFolded(SGDisplayGroup *GroupToFold, BOOL FoldedState)
{
	ERROR3IF(GroupToFold == NULL, "SuperGallery::ForceGroupFolded- NULL parameter is illegal");

	// Ensure the group is in the requested state, and reformat/redraw if necessary
	GroupToFold->SetFoldedState(FoldedState);
}



/********************************************************************************************

>	void SuperGallery::ScrollToShow(DocRect *Area, BOOL PreferToSeeBottom = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95

	Inputs:		Area - The area to scroll to, in list virtual coordinates

				PreferToSeeBottom - Indicates whether you're a leg man or not... er
				I mean... set this to TRUE if the things at the bottom of the rect
				are more important than the things at the top - if the entire rect
				won't fit into the visible area of the window, then the bottom portion
				rather than the top portion will be shown.
				
	Purpose:	Determines if the given rectangle is entirely visible in the display
				window. If it is not, the window is scrolled to show the rectangle.
				Used to scroll the display to ensure a single item or set of items is
				visible on screen.

	Notes:		Calls ReformatNow(FALSE) to reformat if the cached format is known to be
				invalid before it attempts to scroll.

				If the rectangle is bigger than the displayable area, we'll scroll to
				the top (or bottom) of it.

				If the rectangle is smaller than the display area, we'll scroll so that
				it is centered in the display.

				May be called when there is no displaylist or when the gallery is closed

	SeeAlso:	SuperGallery::ForceGroupFolded; SGDisplayRoot::SetScrollOffset

********************************************************************************************/

void SuperGallery::ScrollToShow(DocRect *Area, BOOL PreferToSeeBottom)
{
	if (DisplayTree == NULL || !IsVisible())
		return;

	ReformatNow(FALSE);		// Check that the formatting information is up to date

	DocRect Window;
	INT32 PixelSize;

	ReDrawInfoType DlgRedrawInfo;
	GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

	CalcVirtualSize(&DlgRedrawInfo, &Window, &PixelSize);

	// If the rectangle is entirely visible, don't bother scrolling to it
	if (Window.lo.y <= Area->lo.y && Window.hi.y >= Area->hi.y)
		return;

	// OK. Scroll the top into view...
	INT32 NewScrollOffset = ABS(Area->hi.y);

	if (Area->Height() <= Window.Height())
		NewScrollOffset -= (Window.Height() - Area->Height()) / 2;
	else if (PreferToSeeBottom)
		NewScrollOffset = (ABS(Area->lo.y)) - Window.Height();

	if (NewScrollOffset != DisplayTree->GetScrollOffset())
	{
		SGMiscInfo MiscInfo;
		FillInMiscInfo(&MiscInfo, &DlgRedrawInfo);

		DisplayTree->SetScrollOffset(NewScrollOffset, &MiscInfo);
	}
}



/********************************************************************************************

>	void SuperGallery::ScrollArea(DocRect *Area, DocCoord *ScrollBy)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/1/95

	Inputs:		Area - The area to scroll, in WINDOW coordinates. Ask Jason. I've got a cold
				and I can't be naffed explaining this today. See the SeeAlso

				ScrollBy - Millipoint offsets to scroll by.

	Outputs:	ScrollBy will be returned containing the actual millipoint scrolled values
				(they are gridlocked to the ouptut pixel grid to ensure it works OK)

	Purpose:	To scroll the list gadget in the super gallery. Basically just a veneer
				function for sgdisplayrootscroll which can't tidily access the DialogOp
				method...

	Notes:		On return, use the updated ScrollBy values to change your scroll offset,
				and then call PaintListNow (or else the bit that 'scrolled into view' will
				not be redrawn)

				May be called when the gallery is closed

	SeeAlso:	DialogOp::ScrollKernelRenderedGadget; SuperGallery::PaintListNow

********************************************************************************************/

void SuperGallery::ScrollArea(DocRect *Area, DocCoord *ScrollBy)
{
	if (IsVisible())
		ScrollKernelRenderedGadget(GetListGadgetID(), Area, ScrollBy);
}



/********************************************************************************************

>	void SuperGallery::PaintListNow(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/95
	Purpose:	Direct veneer function for DialogOp::PaintGadgetNow for the ListBox gadget.
				Allows SGDisplayNodes to force an immediate update

	Notes:		There is a special lock which prevents this method from starting a redraw
				immediately if we are already inside the rendering code (this prevents
				render regions being created while already using a render region, which
				is a bad situation to get into). In this case, the paint will be done later
				when the message gets through to our normal message processor.

				May be called when the gallery is closed

********************************************************************************************/

void SuperGallery::PaintListNow(void)
{
	if (IsVisible() && !AmRedrawing)
		PaintGadgetNow(GetListGadgetID());
}



/********************************************************************************************

>	void SuperGallery::ReformatNow(BOOL ForceReformat)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95

	Inputs:		ForceFormat - TRUE to force an immediate reformat even if we think the format
				is correctly cached; FALSE to only reformat if we are sure it's necessary.

	Purpose:	Immediately reformats the display tree if necessary.
				NOTE: This is done automatically on any redraw if the cached format is known
				to be out of date. However, sometimes (e.g. when unfolding a group and then
				scrolling to it) you may want to chnage the display tree format and then
				cause the formatting to be recalculated before it is needed for the next
				step (scrolling) of your operation.

				May be called when the gallery is closed

********************************************************************************************/

void SuperGallery::ReformatNow(BOOL ForceReformat)
{
	if (DisplayTree == NULL || !IsVisible())
		return;

	if (ForceReformat || !FormatIsCached)
	{
		SGFormatInfo FormatInfo;
		SGMiscInfo MiscInfo;

		FillInMiscInfo(&MiscInfo);
		DisplayTree->InitFormatInfo(&FormatInfo, &MiscInfo);

		// Recalculate and cache formatting information
		BroadcastEvent(SGEVENT_FORMAT, &FormatInfo, &MiscInfo);

		// And remember that the cache is now valid
		FormatIsCached = TRUE;
	}
}



/********************************************************************************************

>	void SuperGallery::ForceRedrawOfList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94
	Purpose:	Reformats, and then redraws the entire displayed list

	Notes:		May be called when the gallery is closed

********************************************************************************************/

void SuperGallery::ForceRedrawOfList(void)
{
	InvalidateCachedFormat();			// Force the formatting info to be re-cached

	if (IsVisible())
		InvalidateGadget(GetListGadgetID());
}



/********************************************************************************************

>	void SuperGallery::ForceRedrawOfArea(DocRect *Area)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Inputs:		Area - The area to redraw, in SuperGallery virtual coordinates

	Purpose:	Redraws the given region of the list. The area specified is a MILLIPOINT
				offset within the SuperGallery Virtual Coordinate Space.

	Notes:		May be called when the gallery is closed
	
	SeeAlso:	SuperGallery::CalcVirtualSize; SGDisplayNode::ForceRedrawMyself

********************************************************************************************/

void SuperGallery::ForceRedrawOfArea(DocRect *Area)
{
	if (DisplayTree == NULL)
		return;

	if (IsVisible())
	{
		// Get the dialogue info (dx, dy, and dpi values for the list box gadget)
		ReDrawInfoType DlgRedrawInfo;
		GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

		DocRect RealArea(*Area);
		INT32 ScrollOffset = DisplayTree->GetScrollOffset();

		// Convert the area into kernel-dialogue coords
		RealArea.lo.y = DlgRedrawInfo.dy + (RealArea.lo.y + ScrollOffset);
		RealArea.hi.y = DlgRedrawInfo.dy + (RealArea.hi.y + ScrollOffset);

		// Clip values within the visible screen area. This is important because
		// the stupid winoil routines don't do this, and some cheap OSes (Win95/Win32s)
		// get upset at being passed huge numbers.
		if (RealArea.lo.y < 0)
			 RealArea.lo.y = 0;

		if (RealArea.hi.y > DlgRedrawInfo.dy)
			RealArea.hi.y = DlgRedrawInfo.dy;

		// If this translates to a visible area of the window, force a redraw of it
		if (RealArea.hi.y > 0 && RealArea.lo.y < DlgRedrawInfo.dy)
			InvalidateGadget(GetListGadgetID(), &DlgRedrawInfo, &RealArea);
	}
}



/********************************************************************************************

>	void SuperGallery::RedrawEverythingBelow(INT32 TopYCoord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		TopYCoord - A Millipoint diaplay list coordinate

	Purpose:	To redraw the display list from a given positio downwards (e.g. if a document
				list changes, you'll need to redraw its display, plus any displays below it
				which may have moved around)

	Notes:		May be called when the gallery is closed

				This method always forces a redraw of the scrollbar (if any) to ensure that
				the scrollbar is updated appropriately for the new extent.
	
********************************************************************************************/

void SuperGallery::RedrawEverythingBelow(INT32 TopYCoord)
{
#if FALSE
/*
	if (DisplayTree == NULL || !IsVisible())
		return;

	ReDrawInfoType DlgRedrawInfo;
	GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

	// Get the visible list area in list virtual coordinates
	DocRect Area(0, 0, DlgRedrawInfo.dx, DlgRedrawInfo.dy);
	ConvertToVirtualCoords(&DlgRedrawInfo, &Area.lo);
	ConvertToVirtualCoords(&DlgRedrawInfo, &Area.hi);

	// If the area to redraw is not offscreen (scrolled down out of view), redraw it
	if (TopYCoord > Area.lo.y)
		ForceRedrawOfArea(&Area);

	SGMiscInfo MiscInfo;
	FillInMiscInfo(&MiscInfo);
	DisplayTree->RedrawScrollBar(&MiscInfo);	// Ensure the Scrollbar is updated
*/
#else
	ReformatAndRedrawIfNecessary();		// This should do a better job than the above code!
#endif
}



/********************************************************************************************

>	void SuperGallery::RedrawDocumentDisplay(Document *TheDocument, BOOL Reformat = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		TheDocument - The docuemnt for which the display needs redrawing

	Purpose:	To redraw the entire display list for the given document.
				If the cached formatting information is known to be compromised, a reformat
				will occur, and the necessary regions will be redrawn. Otherwise, just
				the document's display will be redrawn.

	Notes:		If TheDocument is NULL or the gallery is closed, nothing happens
	
	SeeAlso:	SuperGallery::InvalidateCachedFormat;
				SuperGallery::ReformatAndRedrawIfNecessary

********************************************************************************************/

void SuperGallery::RedrawDocumentDisplay(Document *TheDocument)
{
	if (TheDocument == NULL || DisplayTree == NULL || !IsVisible())
		return;

	SGDisplayGroup *Group = DisplayTree->FindSubtree(this, TheDocument, NULL);

	// Reformat, and update any areas that have changed
	ReformatAndRedrawIfNecessary();

	// If we found it, redraw the group and its contents (the window manager should
	// sort out redraw rectangle merging and stuff for us)
	if (Group != NULL)
		Group->ForceRedrawOfMyselfAndChildren();
}



/********************************************************************************************

>	void SuperGallery::ReformatAndRedrawIfNecessary(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Purpose:	If the cached format information is thought to be out of date (eg after
				someone has called InvalidateCachedFormat) then this method will cause
				a reformatting operation to be carried out, and will then redraw any
				areas which are affected (have changed due to the reformatting).

	SeeAlso:	SuperGallery::InvalidateCachedFormat

********************************************************************************************/

void SuperGallery::ReformatAndRedrawIfNecessary(void)
{
	if (DisplayTree == NULL || !IsVisible())
		return;

	ReDrawInfoType DlgRedrawInfo;
	GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

	SGMiscInfo MiscInfo;
	FillInMiscInfo(&MiscInfo, &DlgRedrawInfo);

	if (!FormatIsCached ||
		LastWindowDX != DlgRedrawInfo.dx || LastWindowDY != DlgRedrawInfo.dy)
	{
		SGFormatInfo FormatInfo;
		DisplayTree->InitFormatInfo(&FormatInfo, &MiscInfo);
		FormatInfo.AccumulateBounds = TRUE;

		// Recalculate and cache formatting information
		BroadcastEvent(SGEVENT_FORMAT, &FormatInfo, &MiscInfo);

		// And remember that the cache is now valid
		FormatIsCached = TRUE;

		// And remember new window size
		LastWindowDX = DlgRedrawInfo.dx;
		LastWindowDY = DlgRedrawInfo.dy;

		// If this caused anything to move, redraw the display
		if (FormatInfo.InvalidBounds.hi.y <= 0 && IsVisible())
			ForceRedrawOfArea(&FormatInfo.InvalidBounds);
	}
}



/********************************************************************************************

>	void SuperGallery::SelectItems(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95

	Inputs:		SelectThem - TRUE to select the given items, FALSE to deselect them

				Exclusive  - TRUE to apply this action to all items *outside* the given
				range, FALSE to apply it to all items *inside* the range. (i.e. it acts
				as a logical 'NOT' on the inside-this-document/library test)

				Document - NULL, or the document which defines the range of items to affect

				Library - NULL, or the library which defines the range of items to affect

	Purpose:	To select/deselect groups of display items (or all display items) in this
				Gallery display.
				All items whose state changes will force redraw themselves automatically

	Notes:		To select all items in a range, and deselect all items outside the range,
				you need to use 2 calls to this method.

				To select/deselect all items in the display, pass FALSE, NULL, NULL to
				the last 3 parameters. (If Doc/Lib are both NULL, 'Exclusive' has no effect)

				e.g.
				SelectItems(TRUE, FALSE, MyDocument);
					will select all items which are related to the given document.

				SelectItems(TRUE, TRUE, MyDocument); 
					will select all items which are NOT related to the given document.

				SelectItems(FALSE);
					will deselect ALL items in the gallery

				SelectItems(TRUE, FALSE, MyDocument);
				SelectItems(FALSE, TRUE, MyDocument); 
					This combination will select all items in this document, ensuring that
					nothing is selected anywhere else in the gallery.

********************************************************************************************/

void SuperGallery::SelectItems(BOOL SelectThem, BOOL Exclusive,
								Document *ParentDocument, Library *ParentLibrary)
{
	if (DisplayTree != NULL)
		DisplayTree->SelectItems(SelectThem, Exclusive, ParentDocument, ParentLibrary);
}


/********************************************************************************************

>	void SuperGallery::SelectGroups(BOOL SelectThem, BOOL Exclusive = FALSE,
						Document *ParentDocument = NULL, Library *ParentLibrary = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/95

	Inputs:		SelectThem - TRUE to select the given groups, FALSE to deselect them

				Exclusive  - TRUE to apply this action to all groups *outside* the given
				range, FALSE to apply it to all groups *inside* the range. (i.e. it acts
				as a logical 'NOT' on the inside-this-document/library test)

				Document - NULL, or the document which defines the range of groups to affect

				Library - NULL, or the library which defines the range of groups to affect

	Purpose:	To select/deselect groups of display groups (or all display groups) in this
				Gallery display.
				All groups whose state changes will force redraw themselves automatically

	Notes:		To select all groups in a range, and deselect all groups outside the range,
				you need to use 2 calls to this method.

				To select/deselect all groups in the display, pass FALSE, NULL, NULL to
				the last 3 parameters. (If Doc/Lib are both NULL, 'Exclusive' has no effect)

	SeeAlso:	SuperGallery::SelectItems for examples

********************************************************************************************/

void SuperGallery::SelectGroups(BOOL SelectThem, BOOL Exclusive,
								Document *ParentDocument, Library *ParentLibrary)
{
	if (DisplayTree != NULL)
		DisplayTree->SelectGroups(SelectThem, Exclusive, ParentDocument, ParentLibrary);
}



/********************************************************************************************

>	void SuperGallery::SelectRange(SGDisplayNode *PrimeNode, SGDisplayNode *AnchorNode);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		PrimeNode - The node which MUST be selected. May NOT be NULL.
				AnchorNode - The other node, specifying a range of sibling nodes to
				be selected. May be NULL, in which case only PrimeNode is selected

	Purpose:	Selects the PrimeNode, and if possible, all sibling nodes between it and
				the Anchor node. If Anchor == NULL or is not found, only PrimeNode is
				selected. Does not deselect any nodes - you should call SelectItems first
				to clear the seln.

********************************************************************************************/

void SuperGallery::SelectRange(SGDisplayNode *PrimeNode, SGDisplayNode *AnchorNode)
{
#if 0
	BOOL Items = FALSE;
	BOOL Groups = FALSE;

	if(PrimeNode->IS_KIND_OF(SGDisplayItem) && (AnchorNode == NULL || AnchorNode->IS_KIND_OF(SGDisplayItem)))
		Items = TRUE;
	else if(PrimeNode->IS_KIND_OF(SGDisplayGroup) && (AnchorNode == NULL || AnchorNode->IS_KIND_OF(SGDisplayGroup)))
		Groups = TRUE;

	if(Items)
		DisplayTree->SelectRange(PrimeNode, AnchorNode);
		SelectRangeItems((SGDisplayItem *)PrimeNode, (SGDisplayItem *)AnchorNode);
	else if(Groups)
		SelectRangeGroups((SGDisplayGroup *)PrimeNode, (SGDisplayGroup *)AnchorNode);
	else
		ERROR3("SGDisplayNode::SelectRange given opposing node types");
#endif

	if (DisplayTree != NULL)
		DisplayTree->SelectRange(PrimeNode, AnchorNode);
}



/********************************************************************************************

>	INT32 SuperGallery::GetSelectedItemCount(Document *TheDocument = NULL,
											Library *TheLibrary = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		TheDocument- NULL, or the document group to scan for
				TheLibrary - NULL, or the library group to scan for

	Purpose:	Counts the number of selected items in the display tree or a subtree.

	Notes:		If both parameters are NULL, will give a count for the entire display tree.

				Otherwise, the document or library specified will be found (if possible)
				in the tree, and the return value will be the selection count within that
				subtree.
				
				If the desired {sub}tree does not exist, 0 is returned.

********************************************************************************************/

INT32 SuperGallery::GetSelectedItemCount(Document *TheDocument, Library *TheLibrary)
{
	if (DisplayTree == NULL)
		return(0);

	if (TheDocument != NULL || TheLibrary != NULL)
	{
		SGDisplayGroup *TheGroup = DisplayTree->FindSubtree(this, TheDocument, TheLibrary);

		if (TheGroup == NULL)
			return(0);

		return(TheGroup->GetSelectedItemCount());
	}
	
	return(DisplayTree->GetSelectedItemCount());
}

/********************************************************************************************

>	INT32 SuperGallery::GetSelectedGroupCount(void)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95

	Inputs:

	Purpose:	Counts the number of selected groups in the display tree

********************************************************************************************/

INT32 SuperGallery::GetSelectedGroupCount(void)
{
	if (DisplayTree == NULL)
		return(0);

	return(DisplayTree->GetSelectedGroupCount());
}


/********************************************************************************************

>	virtual void SuperGallery::SelectionHasChanged(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95

	Purpose:	To inform the gallery that the selection has changed in some way.
				The base gallery class will do the following things:

				MonoOn
				- If the selection is not exactly one item, the following standard
				buttons are shaded:
					Apply, Redefine, Edit

				- If the selection is greater than 1 item, the New button will shade

				- If the selection is zero items, then the Delete button will shade
				MonoOff

	Notes:		If this default action is unsuitable, derived galleries may override it.
				If they add their own special buttons, they may need to override this
				method to ensure that their buttons also behave correctly on sel change.

				This method is called after creation of the window (immediately after
				the PostCreate upcall) to ensure the window state is correct

********************************************************************************************/

void SuperGallery::SelectionHasChanged(void)
{
	if (DisplayTree == NULL || AmShaded || !IsVisible())
		return;

	INT32 Count = DisplayTree->GetSelectedItemCount();

	EnableGadget(_R(IDC_GALLERY_APPLY),		(Count == 1));	// Apply/Redefine/edit/name need exactly 1 item
	EnableGadget(_R(IDC_GALLERY_REDEFINE),	(Count == 1));
	EnableGadget(_R(IDC_GALLERY_EDIT),		(Count == 1));
	EnableGadget(_R(IDC_GALLERY_NAME),		(Count == 1));

	EnableGadget(_R(IDC_GALLERY_NEW),		(Count <= 1));	// New needs 0 or 1 items

	EnableGadget(_R(IDC_GALLERY_DELETE),	(Count > 0));	// Delete needs 1 or more items

	EnableGadget(_R(IDC_GALLERY_UNDO),		TRUE);			// Undo/menu always available
	EnableGadget(_R(IDC_GALLERY_MENU),		TRUE);
}



/********************************************************************************************

>	void SuperGallery::SetLastSelectedNode(SGDisplayNode *JustSelected)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Inputs:		JustSelected - The node which has just been made selected. This 
				param may be NULL (as is the case when a node dies or is deselected)

	Purpose:	To set which node, if any, was last selected in this gallery.
				Used by the display tree to determine the anchor point for selection
				extension exercises.

	Scope:		public, but it's a private interface between display tree and parent gallery

********************************************************************************************/

void SuperGallery::SetLastSelectedNode(SGDisplayNode *JustSelected)
{
	LastSelectedNode = JustSelected;
}



/********************************************************************************************

>	SGDisplayNode *SuperGallery::GetLastSelectedNode(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95

	Returns:	A pointer to the last selected node - this may well be NULL

	Purpose:	To determine which node, if any, was last selected in this gallery.
				Used by the display tree to determine the anchor point for selection
				extension exercises.

	Notes:		For added security, it would be helpful if you do not actually use
				the object that this returns (i.e. all existing code compares the
				pointer to others, but does not actually dereference the pointer as such)

********************************************************************************************/

SGDisplayNode *SuperGallery::GetLastSelectedNode(void)
{
	return(LastSelectedNode);
}



/********************************************************************************************

>	virtual SGDisplayGroup *SuperGallery::AddLibraryGroup(Library *LibraryToDisplay,
															INT32 NumItems)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95

	Inputs:		LibraryToDisplay - points to a Library object which needs to be displayed
				in this gallery.

				NumItems - The expected number of items that will be added to this group.

	Returns:	NULL, or the newly created group

	Purpose:	Creates a blank group in the displaytree representing the given library.
				(If a suitable group already exists, its contents are wiped in preparation
				for being filled in again).

	Notes:		This is a callback function called by a newly created Library as a result
				of that library having been opened by this gallery.

	SeeAlso:	SuperGallery::RemoveLibraryGroup; SuperGallery::AddLibraryItem

********************************************************************************************/

SGDisplayGroup *SuperGallery::AddLibraryGroup(Library *LibraryToDisplay, INT32 NumItems)
{
	ERROR3IF(LibraryToDisplay == NULL, "SuperGallery::AddLibraryGroup - NULL parameter is illegal");

	if (DisplayTree == NULL)
	{
		ERROR3("SuperGallery::AddLibraryGroup called before the DisplayTree was initialised!");
		return(NULL);
	}

	SGDisplayGroup *TheGroup = DisplayTree->FindSubtree(this, NULL, LibraryToDisplay);

	if (TheGroup == NULL)
	{
		// No existing group for that library, so create a new one
		TheGroup = new SGDisplayGroup(this, NULL, LibraryToDisplay);
		if (TheGroup == NULL)				// Failed!
			return(NULL);

		// And add it to our display tree
		DisplayTree->AddItem(TheGroup);
	}
	else
		TheGroup->DestroySubtree(FALSE);	// Delete all items in the group

	return(TheGroup);

}



/********************************************************************************************

>	virtual void SuperGallery::RemoveLibraryGroup(Library *DisplayedLibrary)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95

	Inputs:		DisplayedLibrary - A pointer to the library you suspect may be on display
				in this gallery.

	Purpose:	Ensures that there are no display groups representing this library in
				the DisplayTree- deletes any related group from the tree if necessary.
				Used when Library objects are destructed to ensure no references to them
				are held by their parent gallery.

	Notes:		May be called if there is no such group, or even no display tree - it
				quietly ensures we are not referencing the given library.

	SeeAlso:	SuperGallery::AddLibraryGroup; SuperGallery::AddLibraryItem

********************************************************************************************/

void SuperGallery::RemoveLibraryGroup(Library *DisplayedLibrary)
{
	ERROR3IF(DisplayedLibrary == NULL, "SuperGallery::RemoveLibraryGroup - NULL parameter is illegal");

	if (DisplayTree == NULL || DisplayedLibrary == NULL)
		return;

	SGDisplayGroup *TheGroup = DisplayTree->FindSubtree(this, NULL, DisplayedLibrary);

	if (TheGroup != NULL)
		TheGroup->DestroySubtree();		// Destroy subtree, including the Group node
}



/********************************************************************************************

>	virtual SGDisplayItem *SuperGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup,
												Library *ParentLib,
												LibraryIndex ItemIndex)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/95

	Inputs:		LibraryGroup - The group to add the item into
				ParentLib - (For cross checking inputs) the library you allege the above
				group is for.
				ItemIndex - The Library generated index for this item

	Returns:	NULL, or a pointer to the created item

	Purpose:	Called by the Library class to create a display item for every item in 
				a newly-scanned library file. It is essentially a callback to the gallery
				which requested that the library be scanned.
				
	Notes:		This method MUST BE OVERRIDDEN by the derived gallery that opens the library,
				in order to create appropriate SGDisplayItem-derived nodes for the things
				in the library (e.g. a clipart library gallery will have to create items
				that display clipart thumbnails)

	SeeAlso:	SuperGallery::AddLibraryGroup; SuperGallery::RemoveLibraryGroup

********************************************************************************************/

SGDisplayItem *SuperGallery::AddLibraryItem(SGDisplayGroup *LibraryGroup, Library *ParentLib,
											LibraryIndex ItemIndex, BOOL bNew)
{
	ERROR3IF(LibraryGroup == NULL || ParentLib == NULL,
				"SuperGallery::AddLibraryItem - NULL params are illegal");

	ERROR3IF(LibraryGroup->GetParentLibrary() != ParentLib,
				"SuperGallery::AddLibraryitem - The DisplayGroup is not for the same library!");

	// The base class cannot add library items - derived classes must override this method
	// in order to provide the necessary functionality (as they must create items of the
	// correct type in order to display things from the given library
	ERROR3("Illegal attempt to add a library item to a gallery which doesn't support it!");	

	return(NULL);
}




/********************************************************************************************

>	CGadgetID SuperGallery::GetListGadgetID(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/95

	Returns:	The Gadget ID of the list box gadget in the SuperGallery window.

	Purpose:	To allow outside entities to determine the list box gadget ID. Meant only
				for gallery drags to determine their target area.

********************************************************************************************/

CGadgetID SuperGallery::GetListGadgetID(void)
{
	return(_R(IDC_GALLERY_LISTBOX));
}


/********************************************************************************************

>	virtual BOOL SuperGallery::PreContextMenu(void)
												 
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/97
	Returns:	TRUE for success
	Purpose:	Give the gallery a chance to do something before popping up the context menu.
				Called before the context menu is popped up on a right click.
				At present, used by the bitmap gallery to ensure that the plug-ins have all
				been parsed.
	Notes:		The base class method does nothing. Override it in galleries which require this
				assistance.

********************************************************************************************/

BOOL SuperGallery::PreContextMenu(void)
{
	// The base class does nothing
	return(TRUE);
}

/********************************************************************************************

>	virtual BOOL SuperGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		The base class method does nothing. Override it in galleries which will
				need command menus, in order to init your menu items. See the colour gallery
				code for an example of this.

				Note: Should only create the menu commands once - further calls in the future
				should return TRUE immediately without trying to re-init the ops.

	SeeAlso:	ColourSGallery::InitMenuCommands; SuperGallery::InitMenuCommand

********************************************************************************************/

BOOL SuperGallery::InitMenuCommands(void)
{
	// The base class does nothing - no menu
	return(TRUE);
}



/********************************************************************************************

>	BOOL SuperGallery::InitMenuCommand(StringBase *CommandID, UINT32 MenuTextID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - The command identifier string (max 31 chars INT32)

				MenuTextID - A string resource ID which references the text to be used in
				the menu item. Note that to get 2 different wordings for the same menu item
				you will need 2 separate MenuCommands, from which you add the appropriate one
				when building the menu.

	Returns:	TRUE if it succeeded

	Purpose:	To initialise a menu command for future use. This should be called for any
				potential menu commands *on startup* - it essentially adds OpDescriptors
				to the system, and they must be initialised and ready to go right from
				startup of the application.

				i.e. only call this from your Init() menthod.

********************************************************************************************/

BOOL SuperGallery::InitMenuCommand(StringBase *CommandID, UINT32 MenuTextID)
{
	String_256 OpToken = GetName();
	// With the new GIF Animation frame gallery, this can mean that the OpToken can easily
	// overrun its String_32 allocation. Therefore, restrict the name to the first 16 characters.
	// Unfortunately, we then use the name to find the gallery to send the command to!
	// So we match that only using the GalleryBarNameLimitSize (16) characters
	String_256 OpTokenT = OpToken;
	OpToken.Left(&OpTokenT, GalleryBarNameLimitSize);

	OpTokenT += TEXT(":");
	OpTokenT += *CommandID;

	return(OpGalleryCommand::InitPolymorphicCommand(&OpTokenT, MenuTextID));
}



/********************************************************************************************

>	BOOL SuperGallery::AddCommand(GalleryContextMenu *TheMenu, StringBase *CommandID,
									BOOL Separator = FALSE, , MenuItem* pRootMenu = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				CommandID - The command to add to the menu - May not be NULL
				Separator - TRUE to follow this item with a separator
				pRootMenu	Pointer to root menu to add this to or NULL for main root

	Returns:	TRUE if it succeeded

	Purpose:	To add a gallery command (or separator) to the end of a menu currently being
				built. Call this from your overridden BuildCommandMenu method to add each
				appropriate command to the menu. NOTE that this command must have been
				registered on startup by calling InitMenuCommand.
	
	SeeAlso:	SuperGallery::BuildCommandMenu; SuperGallery::InitMenuCommand

********************************************************************************************/

BOOL SuperGallery::AddCommand(GalleryContextMenu *TheMenu, StringBase *CommandID, BOOL Separator,
							  MenuItem* pRootMenu)
{
	String_256 OpToken = GetName();
	// With the new GIF Animation frame gallery, this can mean that the OpToken can easily
	// overrun its String_32 allocation. Therefore, restrict the name to the first 16 characters.
	// Unfortunately, we then use the name to find the gallery to send the command to!
	// So we match that only using the GalleryBarNameLimitSize (16) characters
	String_256 OpTokenT = OpToken;
	OpToken.Left(&OpTokenT, GalleryBarNameLimitSize);

	OpTokenT += TEXT(":");
	OpTokenT += *CommandID;

	return(TheMenu->BuildCommand(OpTokenT, Separator, pRootMenu));
}



/********************************************************************************************

>	virtual BOOL SuperGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		The base class does nothing. Override in order to provide a menu.

********************************************************************************************/

BOOL SuperGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
	return(FALSE);
}



/********************************************************************************************

>	virtual OpState SuperGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - TheString ID of the command
	Outputs:	ShadeReason - If you return (OpState.Greyed == TRUE) then this should be filled
				ion with the reason that the item is shaded/greyed.

	Returns:	An OpState indicating the current menu item state.

	Purpose:	To determine the state of a given menu item. This method is an exact
				parallel to an Op's GetState method (in fact, it is called by an Op's GetState)
	
	Notes:		Override this method to provide state info for your special commands
				Call the base class for unknown commands to allow it to handle them for you

				Default handling is provided for the following commands. These commands
				need only be added to your menu in BuildCommandMenu in order to become
				available. Override the default behaviour if it is unsuitable:
				MonoOn
					Properties		}
					Sort			} - Always available
					Find			}

					New				Always available
					Apply			Available if one item selected
					Edit			Available if one item selected
					Delete			Available if one or more items salected
					Redefine		Available if one item selected

					NextGroup		Available if there is a next group
					PrevGroup		Available if there is a previous group
					FoldGroup		Always available if there is a selected group
					UnfoldGroup		Always available if there is a selected group
				MonoOff

********************************************************************************************/

OpState SuperGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	// Find/Sort/Properties are pretty much always available
	if (*CommandID == SGCmd_Sort || *CommandID == SGCmd_Find ||
		*CommandID == SGCmd_Properties || *CommandID == SGCmd_New)
		return(State);

	// Find the selected group for this command to act on
	SGDisplayGroup *ThisGroup = FindCommandGroup();

	if (ThisGroup == NULL)									// --- No group to apply command to - abort
	{
		State.Greyed = TRUE;
		ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
	}
	else if (*CommandID == SGCmd_Apply		||				// --- Apply, Edit, Redefine, Rename
			 *CommandID == SGCmd_Edit		||
			 *CommandID == SGCmd_Redefine	||				// 		(These options are given default behaviour)
			 *CommandID == SGCmd_Rename)
		{
		if (GetSelectedItemCount() != 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLE));
		}
	}
	else if (*CommandID == SGCmd_Delete)					// --- Delete (applies only to selected *items*)
	{
		if (GetSelectedItemCount() < 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_NOSEL));
		}
	}
	else if (*CommandID == SGCmd_NextGroup)					// --- Next Group
	{
		ThisGroup = (SGDisplayGroup *) ThisGroup->GetNext();
		if (ThisGroup == NULL)		// No next group
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_LASTGROUP));
		}
	}
	else if (*CommandID == SGCmd_PrevGroup)					// --- Previous Group
	{
		ThisGroup = (SGDisplayGroup *) ThisGroup->GetPrevious();
		if (ThisGroup == NULL)		// No next group
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_FIRSTGROUP));
		}
	}

	return(State);
}



/********************************************************************************************

>	virtual void SuperGallery::DoCommand(StringBase *CommandID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - The String ID of the command

	Purpose:	To apply a given command when it is chosen from the menu.
	
	Notes:		Override this method to provide handling for your special commands.
				Call the base class if you don't recognise the command, so that it can
				handle standard commands.

				Default handling is provided for the following commands. These commands
				need only be added to your menu in BuildCommandMenu in order to become
				available. Override the default behaviour if it is unsuitable.
				MonoOn
					Properties*		Shows properties (options) dialogue
					Sort*			Shows Sort dialogue
					Find*			Shows find dialogue

					New				Calls ApplyAction(SGACTION_CREATE)
					Apply			Calls ApplyAction(SGACTION_APPLY)
					Edit			Calls ApplyAction(SGACTION_EDIT)
					Delete			Calls ApplyAction(SGACTION_DELETE)
					Redefine		Calls ApplyAction(SGACTION_REDEFINE)

					NextGroup		Scrolls to show next group
					PrevGroup		Scrolls to show previous group
					FoldGroup		Forces the selected group folded
					UnfoldGroup		Forces the seletced group unfolded
				MonoOff

				* These items should always be provided if you provide an options menu.
				  They should be shaded when unavailable.

********************************************************************************************/

void SuperGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Properties)										// --- Properties
		SGalleryOptionsDlg::InvokeDialog(this);
	else if (*CommandID == SGCmd_Sort)										// -- Sort
		SGallerySortDlg::InvokeDialog(this);
	else if (*CommandID == SGCmd_Find)										// --- Search
		SGallerySearchDlg::InvokeDialog(this);
	else if (*CommandID == SGCmd_New)										// --- New (default)
		ApplyAction(SGACTION_CREATE);
	else
	{
		// All remaining commands require a selected group, so abort if no group
		SGDisplayGroup *ThisGroup = FindCommandGroup();
		if (ThisGroup == NULL)		// No group to apply command to - abort
			return;

		if (*CommandID == SGCmd_Apply)										// --- Apply (default)
			ApplyAction(SGACTION_APPLY);
		else if (*CommandID == SGCmd_Edit)									// --- Edit (default)
			ApplyAction(SGACTION_EDIT);
		else if (*CommandID == SGCmd_Delete)								// --- Delete (default)
			ApplyAction(SGACTION_DELETE);
		else if (*CommandID == SGCmd_Redefine)								// --- Redefine (default)
			ApplyAction(SGACTION_REDEFINE);
		else if (*CommandID == SGCmd_NextGroup)								// --- Next Group
		{
			ThisGroup = (SGDisplayGroup *) ThisGroup->GetNext();
			if (ThisGroup == NULL)				// No next group
				return;

			// Select the group we are scrolling to
			SelectItems(FALSE);
			SelectGroups(FALSE);
			ThisGroup->SetSelected();
			SelectionHasChanged();

			// And scroll (if necessary) to make that group visible on screen
			DocRect ScrollToRect;
			ThisGroup->GetFormatRect(&ScrollToRect);
			ScrollToShow(&ScrollToRect);
		}
		else if (*CommandID == SGCmd_PrevGroup)								// --- Previous Group
		{
			ThisGroup = (SGDisplayGroup *) ThisGroup->GetPrevious();
			if (ThisGroup == NULL)				// No prev group
				return;

			// Select the group we are scrolling to
			SelectItems(FALSE);
			SelectGroups(FALSE);
			ThisGroup->SetSelected();
			SelectionHasChanged();

			// And scroll (if necessary) to make that group visible on screen
			DocRect ScrollToRect;
			ThisGroup->GetFormatRect(&ScrollToRect);
			ScrollToShow(&ScrollToRect);
		}
		else if (*CommandID == SGCmd_FoldGroup)								// --- Fold Group
		{
			ForceGroupFolded(ThisGroup, TRUE);

			// And make sure that we scroll back to the start of the group - we may have
			// just shot off the bottom of the list now that it is several miles shorter!
			// Also, we select the group, so we don't leave a selected item inside a folded group
			SelectItems(FALSE);
			SelectGroups(FALSE);
			ThisGroup->SetSelected();
			SelectionHasChanged();

			DocRect ScrollToRect;
			ThisGroup->GetFormatRect(&ScrollToRect);
			ScrollToShow(&ScrollToRect);
		}
		else if (*CommandID == SGCmd_UnfoldGroup)							// --- Unfold Group
			ForceGroupFolded(ThisGroup, FALSE);
		else
		{
			ERROR3("SuperGallery::DoCommand - base class called for unknown command");
		}
	}
}



/********************************************************************************************

>	SGDisplayGroup *SuperGallery::FindCommandGroup(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Returns:	NULL, or a pointer to the first selected item in the gallery

	Purpose:	Determines which group will be affected by a menu command, if any.
				Esentially finds the first selected group in the gallery, or the parent
				group of the first selected item.

********************************************************************************************/

SGDisplayGroup *SuperGallery::FindCommandGroup(void)
{
	SGDisplayGroup *ThisGroup = DisplayTree->FindNextSelectedGroup();

	if (ThisGroup == NULL)
	{
		SGDisplayNode *Item = DisplayTree->FindNextSelectedItem();
		if (Item == NULL)		// No selected group or item - abort
			return(NULL);

		ThisGroup = (SGDisplayGroup *) Item->GetParent();
	}

	return(ThisGroup);
}



/********************************************************************************************

>	void SuperGallery::InsertCopiedItem(SGDisplayItem *CopiedItem,
							SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		CopiedItem - The item to insert into the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to insert at the end of the sibling list), or
				points to the item BEFORE which the CopiedItem will be inserted.

	Purpose:	Inserts a given copied-node into the tree in an appropriate fashion.
				Used for re-organising the tree at the conclusion of a drag.
				Intended to be called only from overridden CopyDisplayItem() methods, to
				take all the work out of the process of copying display items around.

	Notes:		CopiedItem must NOT already be linked into the tree (ERROR3 if it is)
				
				This is used when dragging between groups, where typically items are
				copied rather than moved. The derived class CopyDisplayItem determines
				if a copy or move is necessary, and if the former, copies the item and
				creates a new DisplayItem, then calls this method to add the copied
				item to the display. Otherwise it will call MoveItem as appropriate

	SeeAlso:	SuperGallery::CopyDisplayItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter

********************************************************************************************/

void SuperGallery::InsertCopiedItem(SGDisplayItem *CopiedItem,
						SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	ERROR3IF(CopiedItem == NULL || DestGroup == NULL, "Illegal NULL param");
	if (CopiedItem == NULL || DestGroup == NULL)
		return;

	if (TargetPosition != NULL)
		TargetPosition->InsertBefore(CopiedItem);
	else
		DestGroup->AddItem(CopiedItem);
}



/********************************************************************************************

>	virtual SGDisplayItem *SuperGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
													SGDisplayGroup *DestGroup,
													SGDisplayItem *TargetPosition = NULL)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		SourceItem - The item to copy elsewhere in the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to add the copied item to the end of the sibling
				list), or points to an item BEFORE which the copied item will be inserted.

	Returns:	NULL (failed) or a pointer to the new (copied) display item

	Purpose:	"Copies" the existing node in the tree in an appropriate fashion.
				
				This method is normally called when a gallery-organising drag completes,
				and it is discovered that the dragged item(s) have been dragged to a 
				different display group.

	Notes:		This method should be overridden by derived galleries to provide
				appropriate behaviour (some galleries (e.g colour) will copy the real-item
				that the given display-item references to the new group (document), while
				other galleries (layer) may just move the item after all).

				Note the handy InsertCopiedItem and MoveBefore/After methods which
				are available to take all of the hard work out of copying/moving items!

				See the body of this method in the source code for example pseudocode.
				For real code, see the Colour Gallery (sgcolour.cpp)

	SeeAlso:	SuperGallery::InsertCopiedItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; ColourSGallery::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *SuperGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	ERROR3IF(SourceItem == NULL || DestGroup == NULL, "Illegal NULL param");

	ERROR3("Unimplemented code - SuperGallery::CopyDisplayItem must be overridden!");

/**** Example psuedocode ****

	// Find the real item (Thingy) to be moved/copied
	Thingy *ThingyToCopy = ((SGDisplayThingy *)SourceItem)->GetDisplayedThingy();

	if (we really want to copy the items, rather than just move them)
	{
		// Just copy the items...
		
		// First, copy the real-item associated with the display item
		ThingyType *NewThingy = new ThingyType(ThingyToCopy);

		// Add the copied item to your 'real item' list or whatever.
		DocThingyList = DestGroup->GetParentDocument()->FindThingyList();

		if (TargetPosition == NULL)
			DocThingyList->AddTail(NewThingy);
		else
		{
			Thingy *TargetPos = ((SGDisplayThingy *)TargetPosition)->GetDisplayedThingy();
			if (TargetPos == NULL)
				DocThingyList->AddTail(NewThingy);
			else
				DocThingyList->InsertBefore(TargetPos, NewThingy);
		}

		// Now, create a new display item for the copied Thingy, and call the
		// base class helper method to insert it as appropriate.
		SGDisplayThingy *NewThingyDisplayItem = new SGDisplayThingy(NewThingy);
		if (NewThingyDisplayItem != NULL)
			SuperGallery::InsertCopiedItem(NewThingyDisplayItem, DestGroup, TargetPosition);

		return(NewThingyDisplayItem);
	}
	else
	{
		// Just move the items
		
		// This code will be much the same as the above, but instead of copying the
		// real-item, you will have to move it as appropriate. Instead of making
		// a new DisplayItem to pass on to InsertCopiedItem, simply delink it from the
		// DisplayTree, and then pass it to InsertCopiedItem as follows:

		SourceItem->RemoveFromTree();		// Delink from current displaytree position
		SuperGallery::InsertCopiedItem(SourceItem, DestGroup, TargetPosition);

		return(SourceItem);
	}

****/

	return(NULL);
}



/********************************************************************************************

>	virtual void SuperGallery::AllItemsCopied(SGDisplayGroup *DestGroup)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Inputs:		DestGroup - Indicates where the copy/move took place.

	Purpose:	This upcall method is called after one or more items have been copied or
				moved by a gallery drag. It allows the gallery to do update things like
				broadcasting a message or getting a related window (eg colour line) to redraw
				at the end of a move/copy operation. This allows thousands of items to be
				moved/copied with only a single redraw/message-broadcast, etc at the very end.

				An example where this methodis used is the colour gallery.

	Notes:		The caller automatically causes the appropriate things to be redrawn, so we
				just need to update other things.
				e.g. The colour gallery broadcasts a ColourChangingMsg to make sure that the
				colour line and other interested parties all update appropriately.

	SeeAlso:	SuperGallery::CopyDisplayItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; ColourSGallery::AllItemsCopied

********************************************************************************************/

void SuperGallery::AllItemsCopied(SGDisplayGroup *DestGroup)
{
	// The base class does nothing
}



/********************************************************************************************

>	SGDisplayNode *SuperGallery::FindNodeUnderPointer(DocCoord *ThePoint)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		ThePoint - A point, in normal kernel-rendered-dialogue coordinates
				within the gallery list-box control. (Millipoint offsets from the
				bottom left corner of the window)

	Returns:	NULL, or a pointer to the displaytree node under the pointer.
				This will usually be an SGDisplayGroup or SGDisplayItem - use IsKindOf
				to determine which type of node it is before you do something stupid!

	Purpose:	To allow outside entities to determine which display node lies under the
				(current mouse pointer) position.

	Notes:		If you pass a stupid point in (i.e one outside the bounds of the
				list box control bounds) you will get a stupid answer (no item, or
				possibly an item outside the bounds of the visible area)

				Ensures that the formatting is cached with ReformatNow(FALSE)

********************************************************************************************/

SGDisplayNode *SuperGallery::FindNodeUnderPointer(DocCoord *ThePoint)
{
	ERROR3IF(ThePoint == NULL, "Illegal NULL param");

	ReformatNow(FALSE);		// Ensure the formatting is cached

	// Get our kernel-rendered-dlg info
	ReDrawInfoType DlgRedrawInfo;
	GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

	// Fill in appropriate information
	SGClaimPointInfo PointInfo;
	PointInfo.Claimant = NULL;
	PointInfo.ClosestSoFar = 1024*1024;		// Don't bother detecting clicks
											// beyond approx 15inches away from items

	// Convert the point into gallery display-list coordinates
	PointInfo.Position = *ThePoint;
	ConvertToVirtualCoords(&DlgRedrawInfo, &PointInfo.Position);

	// Ask the tree to hit-test the point
	BroadcastEvent(SGEVENT_CLAIMPOINT, &PointInfo);

	// And return the node (if any) which claimed to 'own' the point
	return(PointInfo.Claimant);
}



/********************************************************************************************

>	void SuperGallery::AutoScrollForDrag(DocCoord *MousePosition)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/3/95

	Inputs:		MousePosition - A point, in normal kernel-rendered-dialogue coordinates
				within the gallery list-box control. (Millipoint offsets from the
				bottom left corner of the window).

	Purpose:	If the given point is near the top or bottom of the window, this
				will cause the supergallery to auto-scroll the window up/down.
				Intended to allow drags to autoscroll as necessary.

	Notes:		Does nothing if the window is closed, or there is no display tree

	SeeAlso:	SGListDragTarget::HandlEvent

********************************************************************************************/

void SuperGallery::AutoScrollForDrag(DocCoord *MousePosition)
{
	if (DisplayTree == NULL || !IsVisible())
		return;
	
	const INT32 AutoScrollDist = 28000/2;	// MILLIPOINT distance for autoscroll activation

	// Get our kernel-rendered-dlg info
	ReDrawInfoType DlgRedrawInfo;
	GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);

	// Find distance from top (or bottom) of the window, and determine a
	// scroll speed/direction factor to use below
	INT32 Dist = DlgRedrawInfo.dy - MousePosition->y;
	INT32 ScrollSpeed = -3;
	if (Dist < 0 || Dist > AutoScrollDist)
	{
		Dist = 	MousePosition->y - 0;
		ScrollSpeed = +3;
	}

	if (Dist > 0 && Dist < AutoScrollDist)
	{
		// We're within AutoScrollDist of the top/bottom - let's scroll
		Dist = (AutoScrollDist - Dist) * ScrollSpeed;

		SGMiscInfo MiscInfo;
		FillInMiscInfo(&MiscInfo, &DlgRedrawInfo);

		DisplayTree->SetScrollOffset(DisplayTree->GetScrollOffset() + Dist, &MiscInfo);
	}
}



/********************************************************************************************

>	virtual BOOL SuperGallery::CanSearchKeywords(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Returns:	FALSE

	Purpose:	Used to determine if this type of gallery supports keyword searching.
				By default galleries do not. Override and return TRUE if you do.

********************************************************************************************/

BOOL SuperGallery::CanSearchKeywords(void)
{
	return(FALSE);
}



/********************************************************************************************

>	void SuperGallery::IncrementPendingRedraws(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	Called by SGDisplayNode classes to handle background redraw.
				This increments our count of how many items are awaiting background
				redraw. While this counter is greater than zero, we use an idle processor
				to kick off background redraws every now and then.

	SeeAlso:	SuperGallery::DecrementPendingRedraws

********************************************************************************************/

void SuperGallery::IncrementPendingRedraws(void)
{
	if (PendingRedraws < 1)
	{
		PendingRedraws = 0;		// Just to be on the safe side!

		// Add an idle event processor to handle the bg redraws
		GetApplication()->RegisterIdleProcessor(MyIdlePriority, this);
	}

	PendingRedraws++;			// Increment count of pending redraws
}



/********************************************************************************************

>	void SuperGallery::DecrementPendingRedraws(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/95

	Returns:	FALSE

	Purpose:	Called by SGDisplayNode classes to handle background redraw.
				This increments our count of how many items are awaiting background
				redraw. While this counter is greater than zero, we use an idle processor
				to kick off background redraws every now and then.

	SeeAlso:	SuperGallery::IncrementPendingRedraws

********************************************************************************************/

void SuperGallery::DecrementPendingRedraws(void)
{
 	PendingRedraws--;			// Decrement count of pending redraws

	if (PendingRedraws < 1)
	{
		PendingRedraws = 0;		// Just to be on the safe side!

		// Remove any idle event processor we had registered
		GetApplication()->RemoveIdleProcessor(MyIdlePriority, this);
	}
}



/********************************************************************************************

>	virtual BOOL SuperGallery::OnIdleEvent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/95
	Inputs:		-
	Returns:	TRUE if it still requires Idle Events
	Purpose:	Processes idle events for the supergallery. This is used to provide
				backgrounded redraw of gallery items.

	SeeAlso:	SuperGallery::IncrementPendingRedraws

********************************************************************************************/

BOOL SuperGallery::OnIdleEvent(void)
{
	if (DisplayTree == NULL || !IsVisible() || PendingRedraws <= 0)
	{
		// Deregister for idles, because we should not be getting them now
		GetApplication()->RemoveIdleProcessor(MyIdlePriority, this);
		return(FALSE);		// I no longer need idles
	}

//	BroadcastEvent(SGEVENT_BGREDRAW, NULL);

	// This is no longer done with an event broadcast. We now call a method which returns
	// once a node has been redrawn. It returns the redrawn node, so that we may continue
	// the next pass from that point, rather than having to re-scan the entire tree from the
	// beginning again (with a 14,000 item clipart gallery, we're looking at a small but
	// very noticable extra delay to scan the entire tree)
	SGMiscInfo MiscInfo;
	FillInMiscInfo(&MiscInfo);

	if (LastBGNode == NULL)
		DisplayTree->DoBGRedrawPass(&MiscInfo);
	else
		LastBGNode->DoBGRedrawPass(&MiscInfo);

	return(TRUE);
}



/********************************************************************************************

>	void SuperGallery::FlushBackgroundRedraws(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95

	Purpose:	Resets the BG rendering system - flushes all pending bg redraws, and then
				ensures the system is in the default (inactive) state. 
				This has the useful side effect that if bg rendering has become confused
				it will reset the state and 'unconfuse' the renderer.

	Notes:		Called automatically whenever the gallery opens (just after PreCreate is
				called, before the window is shown), and closes (Messgae handler DIM_CANCEL)

	SeeAlso:	SuperGallery::IncrementPendingRedraws; SuperGallery::DecrementPendingRedraws

********************************************************************************************/

void SuperGallery::FlushBackgroundRedraws(void)
{
	BroadcastEvent(SGEVENT_BGFLUSH, NULL);

	ERROR3IF(PendingRedraws != 0, "Warning: PendingRedraws was non-zero after a flush");

	PendingRedraws = 0;
	LastBGNode = NULL;

	GetApplication()->RemoveIdleProcessor(MyIdlePriority, this);
}

/********************************************************************************************

>	BOOL SuperGallery::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95

	Purpose:	Gets status line help from the gallery.

********************************************************************************************/

BOOL SuperGallery::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	SGDisplayNode *TheNode = FindNodeUnderPointer(MousePos);
	if (TheNode != NULL)
	{
		// Convert MousePos into a display list coord and call the node
		DocCoord ListPos(*MousePos);
		ReDrawInfoType DlgRedrawInfo;
		GetKernelRenderedGadgetInfo(GetListGadgetID(), &DlgRedrawInfo);
		ConvertToVirtualCoords(&DlgRedrawInfo, &ListPos);

		return(TheNode->GetStatusLineHelp(&ListPos, Result));
	}

	return(FALSE);
}


/********************************************************************************************

>	BOOL SuperGallery::DeVirtualiseAllGroups(StringBase *ProgressBarMsg = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/96
	Inputs:		ProgressBarMsg	- 	Pass in a String if you want a progress bar (NULL if you
									don't
	Returns:	TRUE if all the groups are now not virtualised out (ie, the groups all have
				their items in the tree)

	Purpose:	Go through all the gallery groups and de-virtualise all virtual groups. A
				virtual group is a group with no associated items, de-virtualising will create
				all these items.

********************************************************************************************/

BOOL SuperGallery::DeVirtualiseAllGroups(StringBase *ProgressBarMsg)
{
	SGDisplayNode *Ptr = DisplayTree->GetChild();
	if (Ptr == NULL)	// No groups
		return TRUE;

	// Count groups in gallery (for progress bar)
	if(ProgressBarMsg != NULL)
	{
		INT32 NumGroups = 0;
		SGDisplayNode *Ptr2 = Ptr;
		while (Ptr2 != NULL)
		{
			NumGroups++;
			Ptr2 = Ptr2->GetNext();
		}
		String_64 SmallStr;
		ProgressBarMsg->Left(&SmallStr, 64);
		BeginSlowJob(NumGroups, FALSE, &SmallStr);
	}

	// Whizz through groups, devirtualising each one which needs it...

	BOOL ProblemsDeVirtualising = FALSE;
	INT32 Count = 0;

	// For library galleries we need to set the Quiet button status ready to un-supress errors
	SetQuietStatus(FALSE);

	while (Ptr != NULL)
	{
		if(((SGDisplayGroup *)Ptr)->IsVirtualised())
		{
			if (!((SGDisplayGroup *)Ptr)->DeVirtualise())
				ProblemsDeVirtualising = TRUE;
		}

		if(ProgressBarMsg != NULL)
			ContinueSlowJob(++Count);

		Ptr = Ptr->GetNext();
	}

	if(ProgressBarMsg != NULL)
		EndSlowJob();
 	
	return !ProblemsDeVirtualising;
}


/********************************************************************************************

>	static SuperGallery* SuperGallery::FindSuperGallery(String_32& SuperGalleryName, INT32 limit = -1)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/94
	Inputs:		SuperGalleryName = Name of op to find
				limit	if -1 then it means use the whole name for the check (default)
						if > 0 then limit the check to the limit number of characters
	Outputs:	-
	Returns:	ptr to SuperGallery
				NULL is returned if not found
	Purpose:	Looks for a given SuperGallery by using its name 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

SuperGallery* SuperGallery::FindSuperGallery(String_32& SuperGalleryName, INT32 limit)
{
	List*       	pList = MessageHandler::GetClassList(CC_RUNTIME_CLASS(DialogOp));
	SuperGallery*	pSuperGallery = (SuperGallery*)pList->GetHead();
	
	String_32		OpName;
	while (pSuperGallery != NULL)
	{
		if (pSuperGallery->IsKindOf(CC_RUNTIME_CLASS(SuperGallery)))
		{
			if (limit > 0)
				pSuperGallery->Name.Left(&OpName, limit);
			else
				OpName = pSuperGallery->Name;
			
			if (OpName == SuperGalleryName)
				return (pSuperGallery);
		}

		pSuperGallery = (SuperGallery*)pList->GetNext(pSuperGallery);
	}

	return NULL;
}






const CDlgMode SGalleryOptionsDlg::Mode = MODELESS; // Mode of the dialog  

const UINT32 SGalleryOptionsDlg::IDD = _R(IDD_SGDISPLAY);


/********************************************************************************************

>	SGalleryOptionsDlg::SGalleryOptionsDlg(): DialogOp(SGalleryOptionsDlg::IDD, SGalleryOptionsDlg::Mode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Constructor

********************************************************************************************/

SGalleryOptionsDlg::SGalleryOptionsDlg()
				   : DialogOp(SGalleryOptionsDlg::IDD, SGalleryOptionsDlg::Mode)
{
	ParentGallery = NULL;
}



/********************************************************************************************

>	SGalleryOptionsDlg::~SGalleryOptionsDlg()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	destructor

********************************************************************************************/

SGalleryOptionsDlg::~SGalleryOptionsDlg()
{
	if (ParentGallery != NULL)
		ParentGallery->CurrentOptionsDlg = NULL;
}



/********************************************************************************************

>	void SGalleryOptionsDlg::InitValues(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Initialises the gallery display dialogue, and sets its controls
	Scope:		private

	SeeAlso:	SGalleryOptionsDlg::SetControls

********************************************************************************************/

void SGalleryOptionsDlg::InitValues(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("SGalleryOptionsDlg MUST be given a valid parent pointer");
		return;
	}

	// Fill in the combo boxes etc with the current settings
	SetControls();
}



/********************************************************************************************

>	static void SGalleryOptionsDlg::InvokeDialog(SuperGallery *Parent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Parent - the parent (owner) of this display dialogue. This pointer
				is used to inform the parent of changes to display/sort modes.
				May NOT be NULL.

	Purpose:	Creates a new instance of this dialogue type, connects it to the given
				gallery, and opens the dialogue on screen.

********************************************************************************************/

void SGalleryOptionsDlg::InvokeDialog(SuperGallery *Parent)
{
	if (Parent->CurrentOptionsDlg != NULL)		// There's one already open!
	{
		// Bring it to the front of the window stack, then return
		Parent->CurrentOptionsDlg->BringToTop();
		return;
	}

	GalDlgParam GalOptInfo(Parent);
	OpDescriptor *Dlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(SGalleryOptionsDlg));

	ERROR3IF(Dlg == NULL, "I can't find the Dialog OpDescriptor");

	if (Dlg != NULL)
		Dlg->Invoke(&GalOptInfo);
}



/********************************************************************************************

>	void SGalleryOptionsDlg::AddDisplayModeName(UINT32 NameResourceID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		NameResourceID - The resource ID of the name string to use for the next item

	Purpose:	Called (multiple times) by derived gallery classes in response to
				a call we make (upon opening the dialogue) to
					ParentGallery->ApplyAction(SGACTION_SETOPTIONS)

	Notes:		Each call appends another display mode name to the display mode combo box
				list in the options window. When an option is chosen from this list, the
				index of the selected item (starting from 0 for the first item) will be
				used to set a new DisplayMode for the parent gallery.

********************************************************************************************/

void SGalleryOptionsDlg::AddDisplayModeName(UINT32 NameResourceID)
{
	SetStringGadgetValue(_R(IDC_SGDISPLAY_DMODE), NameResourceID, TRUE, 0);
}



/********************************************************************************************

>	void SGalleryOptionsDlg::AddDisplayModeName(StringBase *EntryName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		EntryName - The name to set as the next item

	Purpose:	Called (multiple times) by derived gallery classes in response to
				a call we make (upon opening the dialogue) to
					ParentGallery->ApplyAction(SGACTION_SETOPTIONS)

	Notes:		Each call appends another display mode name to the display mode combo box
				list in the options window. When an option is chosen from this list, the
				index of the selected item (starting from 0 for the first item) will be
				used to set a new DisplayMode for the parent gallery.

********************************************************************************************/

void SGalleryOptionsDlg::AddDisplayModeName(StringBase *EntryName)
{
	SetStringGadgetValue(_R(IDC_SGDISPLAY_DMODE), EntryName, TRUE, 0);
}



/********************************************************************************************

>	void SGalleryOptionsDlg::SetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	(Re)Initialises the colour manager dialogue controls
				(This simply sets the combo boxes etc up from the current settings)

********************************************************************************************/

void SGalleryOptionsDlg::SetControls(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("ParentGallery is NULL!");
		return;
	}

	// Set up the OK button to respond to Adjust (SOFT_COMMIT) clicks
	DualFunctionButton(wxID_OK);

	// Set values in our combo boxes - the gallery will call us back to do this
	DeleteAllValues(_R(IDC_SGDISPLAY_DMODE));
	GadgetRedraw(_R(IDC_SGDISPLAY_DMODE), FALSE);

	BOOL Result = ParentGallery->ApplyAction(SGACTION_SETOPTIONS);

	EnableGadget(_R(IDC_SGDISPLAY_DMODE), Result);
	GadgetRedraw(_R(IDC_SGDISPLAY_DMODE), TRUE);

	if (Result)
	{
		SetSelectedValueIndex(_R(IDC_SGDISPLAY_DMODE), (INT32) ParentGallery->GetDisplayMode());
		SetComboListLength(_R(IDC_SGDISPLAY_DMODE));
	}
}



/********************************************************************************************

>	void SGalleryOptionsDlg::Commit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Applies the current settings in the dialogue to its parent gallery.

********************************************************************************************/

void SGalleryOptionsDlg::Commit(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("No parent gallery!");
		return;
	}

	INT32 NewDisplayMode = (INT32) GetSelectedValueIndex(_R(IDC_SGDISPLAY_DMODE));
	INT32 OldDisplayMode = ParentGallery->GetDisplayMode();
	if (NewDisplayMode != OldDisplayMode)
	{
		SGMiscInfo MiscInfo;
		ParentGallery->FillInMiscInfo(&MiscInfo);

		DocCoord OldPos(10000, MiscInfo.WindowHeight - 1000);
		SGDisplayNode *TopLeftItem = ParentGallery->FindNodeUnderPointer(&OldPos);

		INT32 OldScrollPos = ParentGallery->DisplayTree->GetScrollOffset();

		// Set the new display mode
		ParentGallery->DisplayMode = NewDisplayMode;

		// Get the parent gallery to 'vet' the new display mode number and take any
		// appropriate action.
		ParentGallery->ApplyAction(SGACTION_DISPLAYMODECHANGED);

		// After the gallery has 'vetted' the display mode, if it has really changed,
		// we need to redraw the list, and ensure it is not scrolled too far (as the
		// list extent has probably just changed dramatically)
		if (ParentGallery->GetDisplayMode() != OldDisplayMode)
		{
			ParentGallery->ForceRedrawOfList();		// Invlaidate the entire listbox

			if (ParentGallery->DisplayTree != NULL)
			{
				// Re-cache the formatting
				ParentGallery->GetDisplayExtent();

				INT32 NewPos = OldScrollPos;
				if (TopLeftItem != NULL)
				{
					// If possible, find where the previous top-left item has moved to,
					// and set the scroll offset to show that item at the top left
					DocRect TheRect;
					TopLeftItem->GetFormatRect(&TheRect);
					NewPos = ABS(TheRect.hi.y);
				}

				// And scroll to the current scroll offset (it will automatically clip
				// the scroll offset back to a sensible place if necessary)
				SGMiscInfo MiscInfo;
				ParentGallery->FillInMiscInfo(&MiscInfo);
				ParentGallery->DisplayTree->SetScrollOffset(NewPos, &MiscInfo);
			}
		}
	}
}



/********************************************************************************************

>	MsgResult SGalleryOptionsDlg::Message( Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Msg - The dialogue manager message to handle

	Returns:	A MsgResult
	Purpose:	Standard DialogOp message handler, for the Gallery display dialogue

********************************************************************************************/

MsgResult SGalleryOptionsDlg::Message( Msg* Message)
{
	if (!(IS_OUR_DIALOG_MSG(Message))) return DialogOp::Message(Message);

	DialogMsg* TheMsg = (DialogMsg*)Message;

	switch(TheMsg->DlgMsg)
	{
		case DIM_COMMIT:						// OK clicked
			Commit();
			// Drop through to CANCEL to close the dlg...

		case DIM_CANCEL:						// Cancel clicked
			Close();
			End();
			break;

		case DIM_SOFT_COMMIT:					// OK "adjust clicked"
			Commit();
			break;

		default:
			break;
	}

	return DialogOp::Message(Message);
}



/********************************************************************************************

>	OpState	SGalleryOptionsDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get the state of the Colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	SGalleryOptionsDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}



/********************************************************************************************

>	BOOL SGalleryOptionsDlg::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SGalleryOptionsDlg::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_SGOPTIONSDLG),
								CC_RUNTIME_CLASS(SGalleryOptionsDlg),
 								OPTOKEN_SGOPTIONSDLG,
 								SGalleryOptionsDlg::GetState,
 								_R(IDST_GALLERY_MENU),	// Status line help
 								_R(IDBBL_GALLERY_MENU),	// Bubble help
 								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	BOOL SGalleryOptionsDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Returns:	FALSE if it fails
	Purpose:	Creates a gallery display dialogue box

********************************************************************************************/

BOOL SGalleryOptionsDlg::Create()
{
	ERROR3IF(ParentGallery == NULL, "My ParentGallery is NULL!");

	if (DialogOp::Create())
	{ 
		ParentGallery->CurrentOptionsDlg = this;

		InitValues();
		return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	void SGalleryOptionsDlg::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		OPDescriptor - the Opdescriptor for this op

	Purpose:	'Does' a gallery display dialogue op.
				DO NOT call this method - it must be invoked via DoWithParam

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGalleryOptionsDlg::InvokeDialog

********************************************************************************************/

void SGalleryOptionsDlg::Do(OpDescriptor*)
{
	ERROR3("SGalleryOptionsDlg - You must use DoWithParam (Call InvokeDialog)");
	End();
}



/********************************************************************************************

>	void SGalleryOptionsDlg::DoWithParam(OpDescriptor *Bob, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		Bob - the Opdescriptor for this op
				Param - The parameter. Must point at a GalDlgParam object

	Purpose:	'Does' a gallery display dialogue op. Shows the dialogue.

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGalleryOptionsDlg::InvokeDialog

********************************************************************************************/

void SGalleryOptionsDlg::DoWithParam(OpDescriptor*, OpParam *Param)
{
	ERROR3IF(Param == NULL, "Null parameters are illegal");

	ParentGallery = ((GalDlgParam *)Param)->ParentGal;

	ERROR3IF(ParentGallery == NULL, "SGalleryOptionsDlg needs a non-NULL parent gallery!");	

	if (ParentGallery != NULL && Create())
		Open();
	else
		End();
}








//**!!
const CDlgMode SGallerySortDlg::Mode = MODELESS; // Mode of the dialog  

const UINT32 SGallerySortDlg::IDD = _R(IDD_SGSORT);



// --- GallerySorts
// The GallerySorts array holds the current window state. Similar arrays (of
// SortKeys only) are held by each instantiation of Gallery, which define
// their current sort settings.

SGOptsSortInfo SGallerySortDlg::GallerySorts[] = 
{
	{ {0, FALSE}, _R(IDC_GALSORT_SORT1), _R(IDC_GALSORT_REVERSE1) },
	{ {0, FALSE}, _R(IDC_GALSORT_SORT2), _R(IDC_GALSORT_REVERSE2) }
};



/********************************************************************************************

>	SGallerySortDlg::SGallerySortDlg(): DialogOp(SGallerySortDlg::IDD, SGallerySortDlg::Mode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Constructor

********************************************************************************************/

SGallerySortDlg::SGallerySortDlg()
				   : DialogOp(SGallerySortDlg::IDD, SGallerySortDlg::Mode)
{
	ParentGallery = NULL;
}



/********************************************************************************************

>	SGallerySortDlg::~SGallerySortDlg()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	destructor

********************************************************************************************/

SGallerySortDlg::~SGallerySortDlg()
{
	if (ParentGallery != NULL)
		ParentGallery->CurrentSortDlg = NULL;
}



/********************************************************************************************

>	void SGallerySortDlg::InitValues(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Initialises the gallery display dialogue, and sets its controls
	Scope:		private

	SeeAlso:	SGallerySortDlg::SetControls

********************************************************************************************/

void SGallerySortDlg::InitValues(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("SGallerySortDlg MUST be given a valid parent pointer");
		return;
	}

	// Fill in the combo boxes etc with the current settings
	SetControls();
}



/********************************************************************************************

>	static void SGallerySortDlg::InvokeDialog(SuperGallery *Parent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Parent - the parent (owner) of this display dialogue. This pointer
				is used to inform the parent of changes to display/sort modes.
				May NOT be NULL.

	Purpose:	Creates a new instance of this dialogue type, connects it to the given
				gallery, and opens the dialogue on screen.

********************************************************************************************/

void SGallerySortDlg::InvokeDialog(SuperGallery *Parent)
{
	if (Parent->CurrentSortDlg != NULL)		// There's one already open!
	{
		// Bring it to the front of the window stack, then return
		Parent->CurrentSortDlg->BringToTop();
		return;
	}

	GalDlgParam GalOptInfo(Parent);
	OpDescriptor *Dlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(SGallerySortDlg));

	ERROR3IF(Dlg == NULL, "I can't find the Dialog OpDescriptor");

	if (Dlg != NULL)
		Dlg->Invoke(&GalOptInfo);
}



/********************************************************************************************

>	void SGallerySortDlg::AddSortKeyName(UINT32 NameResourceID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Inputs:		NameResourceID - The resource ID of the name string to use for the next item

	Purpose:	Called (multiple times) by derived gallery classes in response to
				a call we make (upon opening the dialogue) to
					ParentGallery->ApplyAction(SGACTION_SETOPTIONS)

	Notes:		Each call appends another sort key name to the sort key combo box lists
				in the options window. When 'apply' is clicked, if sorting is enabled by
				the user, the appropriate sort mode from the available ones will be used
				to sort the gallery. 

				The sort mode 0 (Do not sort) is always available.
				By default, sort mode 1 will be sort-by-name (but only if the base DisplayItem
				class is called for the comparison, and the parent gallery should call
				this method with 'sort by name' (_R(IDS_SORTBY_NAME)) first oin order to match
				up appropriate text to the sort mode.

	SeeAlso:	SuperGallery::ApplySortNow

********************************************************************************************/

void SGallerySortDlg::AddSortKeyName(UINT32 NameResourceID)
{
/*
	if (!SupportsSorting)
	{
		SupportsSorting = TRUE;

		// We now have evidence to prove that the gallery has supplied its own sort
		// modes, so remove 'None' from the first list
		DeleteAllValues(GallerySorts[0].ListControlID);
	}
*/
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
		SetStringGadgetValue(GallerySorts[i].ListControlID, NameResourceID, TRUE, 0);
}



/********************************************************************************************

>	void SGallerySortDlg::AddSortKeyName(StringBase *EntryName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Inputs:		EntryName - The name to set as the next item

	Purpose:	Called (multiple times) by derived gallery classes in response to
				a call we make (upon opening the dialogue) to
					ParentGallery->ApplyAction(SGACTION_SETOPTIONS)

	Notes:		Each call appends another sort key name to the sort key combo box lists
				in the options window. When 'apply' is clicked, if sorting is enabled by
				the user, the appropriate sort mode from the available ones will be used
				to sort the gallery. 

				The sort mode 0 (Do not sort) is always available.
				By default, sort mode 1 will be sort-by-name (but only if the base DisplayItem
				class is called for the comparison, and the parent gallery should call
				this method with 'sort by name' (_R(IDS_SORTBY_NAME)) first oin order to match
				up appropriate text to the sort mode.

	SeeAlso:	SuperGallery::ApplySortNow

********************************************************************************************/

void SGallerySortDlg::AddSortKeyName(StringBase *EntryName)
{
/*
	if (!SupportsSorting)
	{
		SupportsSorting = TRUE;

		// We now have evidence to prove that the gallery has supplied its own sort
		// modes, so remove 'None' from the first list
		DeleteAllValues(GallerySorts[0].ListControlID);
	}
*/
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
		SetStringGadgetValue(GallerySorts[i].ListControlID, EntryName, TRUE, 0);
}



/********************************************************************************************

>	void SGallerySortDlg::SetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/95

	Purpose:	Shades/unshaded the sort fields as appropriate to the button which 
				enables them.

********************************************************************************************/

void SGallerySortDlg::ShadeSortFields(void)
{
	// If the 'sort' button is on, then shade all the sort fields	
	BOOL AllowedOn = TRUE;

	// If we've made it unshaded and the first item is zero, we need to make it 1,
	// as the first field has no 'none' option.
	if (GallerySorts[0].SortKey.SortKey == 0)
		GallerySorts[0].SortKey.SortKey = 1;

	for (INT32 i = 0; i < MaxSGSortKeys; i++)
	{
		if (i > 0)
			AllowedOn = (GallerySorts[i-1].SortKey.SortKey != 0);

		EnableGadget(GallerySorts[i].ListControlID,  AllowedOn);
		EnableGadget(GallerySorts[i].CheckControlID, AllowedOn && GallerySorts[i].SortKey.SortKey != 0);
	}

	// And finally, the miscellaneous controls
	EnableGadget(_R(IDC_GALSORT_GROUP1), TRUE);
	EnableGadget(_R(IDC_GALSORT_GROUP2), TRUE);
	EnableGadget(_R(IDC_GALSORT_TOALL),  TRUE);
	EnableGadget(_R(IDC_GALSORT_TOSELONLY), TRUE);
}



/********************************************************************************************

>	void SGallerySortDlg::SetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	(Re)Initialises the sort dialogue controls
				(This simply sets the combo boxes etc up from the current settings)

********************************************************************************************/

void SGallerySortDlg::SetControls(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("ParentGallery is NULL!");
		return;
	}

	INT32 i;

	// Set up the OK button to respond to Adjust (SOFT_COMMIT) clicks
	DualFunctionButton(wxID_OK);

	String_32 NoSortName(_R(IDS_SORTBY_NONE));	// Add 'None' to all but the 1st combo box
	for (i = 0; i < MaxSGSortKeys; i++)
	{
		// Copy the last-used sort-mode information from the parent gallery
		GallerySorts[i].SortKey = ParentGallery->SortKeys[i];

		// Clear out the sort gadgets, and add 'None' to the head of each list
//		GadgetRedraw(GallerySorts[i].ListControlID, FALSE);
		DeleteAllValues(GallerySorts[i].ListControlID);
		if (i > 0)
			SetStringGadgetValue(GallerySorts[i].ListControlID, (StringBase *)&NoSortName, FALSE, 0);
	}

	BOOL Result = ParentGallery->ApplyAction(SGACTION_SETSORTMODE);

	for (i = 0; i < MaxSGSortKeys; i++)
	{
//		GadgetRedraw(GallerySorts[i].ListControlID, TRUE);
		SetComboListLength(GallerySorts[i].ListControlID);
	}

	if (Result)
	{
		// Set the Combo box selections and 'reversed' boxes to the current sort mode
		INT32 Selected;
		for (i = 0; i < MaxSGSortKeys; i++)
		{
			// And set the gadgets appropriately
			SetBoolGadgetSelected(GallerySorts[i].CheckControlID, GallerySorts[i].SortKey.Reversed);
			
			Selected = GallerySorts[i].SortKey.SortKey;
			if (i == 0 && Selected > 0)		// The first control has no 'none' option
				Selected--;
			SetSelectedValueIndex(GallerySorts[i].ListControlID, Selected);
		}
	}

	SetBoolGadgetSelected(_R(IDC_GALSORT_TOALL), TRUE);
	SetBoolGadgetSelected(_R(IDC_GALSORT_TOSELONLY), FALSE);

	// Shade/unshade the sort fields as appropriate to the setting of the 'sort by:' check
	// box, and the currently active sort key(s).
	ShadeSortFields();
}



/********************************************************************************************

>	void SGallerySortDlg::Commit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Applies the current settings in the dialogue to its parent gallery.

********************************************************************************************/

void SGallerySortDlg::Commit(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("No parent gallery!");
		return;
	}

	// Copy the new sort mode info into the parent gallery
	for (INT32 i = 0; i < MaxSGSortKeys; i++)
	{
		GallerySorts[i].SortKey.Reversed = GetBoolGadgetSelected(GallerySorts[i].CheckControlID);
		ParentGallery->SortKeys[i] = GallerySorts[i].SortKey;
	}

	ParentGallery->ApplySortNow(GetBoolGadgetSelected(_R(IDC_GALSORT_TOALL)));
}



/********************************************************************************************

>	MsgResult SGallerySortDlg::Message( Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Msg - The dialogue manager message to handle

	Returns:	A MsgResult
	Purpose:	Standard DialogOp message handler, for the Gallery display dialogue

********************************************************************************************/

MsgResult SGallerySortDlg::Message( Msg* Message)
{
	if (!(IS_OUR_DIALOG_MSG(Message))) return DialogOp::Message(Message);

	DialogMsg* TheMsg = (DialogMsg*)Message;

	switch(TheMsg->DlgMsg)
	{
		case DIM_COMMIT:						// OK clicked
			Commit();
			// Drop through to CANCEL to close the dlg...

		case DIM_CANCEL:						// Cancel clicked
			Close();
			End();
			break;

		case DIM_SOFT_COMMIT:					// OK "adjust clicked"
			Commit();
			break;

		case DIM_LFT_BN_CLICKED:
			if (TheMsg->GadgetID == _R(IDC_GALSORT_DOSORT))		// Shade/ushade the sort options
					ShadeSortFields();
			break;

		case DIM_SELECTION_CHANGED:
			{
				for (INT32 i = 0; i < MaxSGSortKeys; i++)
				{
					if (TheMsg->GadgetID == GallerySorts[i].ListControlID)
					{
						WORD Bob;
						GetValueIndex(GallerySorts[i].ListControlID, &Bob);
						GallerySorts[i].SortKey.SortKey = (INT32) Bob;

						// If it's the first sort key, then it doesn't have the 'none' option
						if (i == 0)
							GallerySorts[i].SortKey.SortKey++;
							
						ShadeSortFields();					// Ensure shading is appropriate
						break;
					}
				}
			}
			break;
		default:
			break;
	}

	return DialogOp::Message(Message);

	return OK;
}



/********************************************************************************************

>	OpState	SGallerySortDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get the state of the Colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	SGallerySortDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}



/********************************************************************************************

>	BOOL SGallerySortDlg::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SGallerySortDlg::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_SGSORTDLG),
								CC_RUNTIME_CLASS(SGallerySortDlg),
 								OPTOKEN_SGSORTDLG,
 								SGallerySortDlg::GetState,
 								_R(IDST_GALLERY_MENU),	// Status line help
 								_R(IDBBL_GALLERY_MENU),	// Bubble help
 								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	BOOL SGallerySortDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Returns:	FALSE if it fails
	Purpose:	Creates a gallery display dialogue box

********************************************************************************************/

BOOL SGallerySortDlg::Create()
{
	ERROR3IF(ParentGallery == NULL, "My ParentGallery is NULL!");

	if (DialogOp::Create())
	{ 
		ParentGallery->CurrentSortDlg = this;

		InitValues();
		return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	void SGallerySortDlg::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		OPDescriptor - the Opdescriptor for this op

	Purpose:	'Does' a gallery display dialogue op.
				DO NOT call this method - it must be invoked via DoWithParam

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGallerySortDlg::InvokeDialog

********************************************************************************************/

void SGallerySortDlg::Do(OpDescriptor*)
{
	ERROR3("SGallerySortDlg - You must use DoWithParam (Call InvokeDialog)");
	End();
}



/********************************************************************************************

>	void SGallerySortDlg::DoWithParam(OpDescriptor *Bob, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		Bob - the Opdescriptor for this op
				Param - The parameter. Must point at a GalDlgParam object

	Purpose:	'Does' a gallery display dialogue op. Shows the dialogue.

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGallerySortDlg::InvokeDialog

********************************************************************************************/

void SGallerySortDlg::DoWithParam(OpDescriptor*, OpParam *Param)
{
	ERROR3IF(Param == NULL, "Null parameters are illegal");

	ParentGallery = ((GalDlgParam *)Param)->ParentGal;

	ERROR3IF(ParentGallery == NULL, "SGallerySortDlg needs a non-NULL parent gallery!");	

	if (ParentGallery != NULL && Create())
		Open();
	else
		End();
}











const CDlgMode SGallerySearchDlg::Mode = MODELESS; // Mode of the dialog  

const UINT32 SGallerySearchDlg::IDD = _R(IDD_SGSEARCH);

String_64 SGallerySearchDlg::LastSearchText;
BOOL SGallerySearchDlg::LastSearchFullInfo = FALSE;


/********************************************************************************************

>	SGallerySearchDlg::SGallerySearchDlg(): DialogOp(SGallerySearchDlg::IDD, SGallerySearchDlg::Mode)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Constructor

********************************************************************************************/

SGallerySearchDlg::SGallerySearchDlg()
				   : DialogOp(SGallerySearchDlg::IDD, SGallerySearchDlg::Mode)
{
	ParentGallery = NULL;
}



/********************************************************************************************

>	SGallerySearchDlg::~SGallerySearchDlg()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	destructor

********************************************************************************************/

SGallerySearchDlg::~SGallerySearchDlg()
{
	if (ParentGallery != NULL)
		ParentGallery->CurrentSearchDlg = NULL;
}



/********************************************************************************************

>	void SGallerySearchDlg::InitValues(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	Initialises the gallery display dialogue, and sets its controls
	Scope:		private

	SeeAlso:	SGallerySearchDlg::SetControls

********************************************************************************************/

void SGallerySearchDlg::InitValues(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("SGallerySearchDlg MUST be given a valid parent pointer");
		return;
	}

	// Fill in the combo boxes etc with the current settings
	SetControls();
}



/********************************************************************************************

>	static void SGallerySearchDlg::InvokeDialog(SuperGallery *Parent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Parent - the parent (owner) of this display dialogue. This pointer
				is used to inform the parent of changes to display/sort modes.
				May NOT be NULL.

	Purpose:	Creates a new instance of this dialogue type, connects it to the given
				gallery, and opens the dialogue on screen.

********************************************************************************************/

void SGallerySearchDlg::InvokeDialog(SuperGallery *Parent)
{
	if (Parent->CurrentSearchDlg != NULL)		// There's one already open!
	{
		// Bring it to the front of the window stack, then return
		Parent->CurrentSearchDlg->BringToTop();
		return;
	}

	GalDlgParam GalSearchInfo(Parent);
	OpDescriptor *Dlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(SGallerySearchDlg));

	ERROR3IF(Dlg == NULL, "I can't find the Dialog OpDescriptor");

	if (Dlg != NULL)
		Dlg->Invoke(&GalSearchInfo);
}



/********************************************************************************************

>	void SGallerySearchDlg::SetControls(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Purpose:	(Re)Initialises the colour manager dialogue controls
				(This simply sets the combo boxes etc up from the current settings)

********************************************************************************************/

void SGallerySearchDlg::SetControls(void)
{
	if (ParentGallery == NULL)
	{
		ERROR3("ParentGallery is NULL!");
		return;
	}

	// Set the search-text field
	SetStringGadgetValue(_R(IDC_SGSEARCH_TEXT), (StringBase *) &LastSearchText);

	// Set the name-only/full-info radio buttons
	SetBoolGadgetSelected(_R(IDC_SGSEARCH_NAMEONLY), !(LastSearchFullInfo));
	SetBoolGadgetSelected(_R(IDC_SGSEARCH_FULLINFO),   LastSearchFullInfo);
}         



/********************************************************************************************

>	void SGallerySearchDlg::Search(BOOL FindFirst)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		FindFirst TRUE = Find first, FALSE = Find next

	Purpose:	Applies the current settings in the dialogue to its parent gallery.
				Searches for the first/next matching gallery item. Selects the matched item.
				Search for next searches from the first selected item onwards.
				Search is case insensitive.

	Notes:		Shows an hourglass during the search, and aborts if escape is pressed.

********************************************************************************************/

void SGallerySearchDlg::Search(BOOL FindFirst)
{
	if(ParentGallery == NULL)
	{						   
		ERROR3("No parent gallery!");
		return;
	}

	BOOL Valid = FALSE;
	String_256 SearchText = GetStringGadgetValue(_R(IDC_SGSEARCH_TEXT), &Valid);

	if(!Valid)	return;
	
	// Truncate the search tet into the last-search-text buffer
	SearchText.Left((StringBase *) &LastSearchText, 63);
	LastSearchText.toLower();

	if(LastSearchText.Length() < 1)
	{
		InformWarning(_R(IDE_SGSEARCH_BADTEXT), _R(IDS_OK));
		return;
	}

	// And remember the last used options for the radio buttons
	LastSearchFullInfo = GetBoolGadgetSelected(_R(IDC_SGSEARCH_FULLINFO));

	// Now, apply the search
	if(ParentGallery == NULL || ParentGallery->DisplayTree == NULL)
		return;

	// Should we use keywords ?
	BOOL UseKeywords = ParentGallery->CanSearchKeywords();

	// Need this if Devirtualising library indexes proves bad...
	ParentGallery->SetQuietStatus(FALSE);

	// Find the first selected item (if any) and get the next item to start searching from
	// If there is no selection, then get the first item
	SGDisplayItem *Item = SearchFindStartItem(FindFirst);

	// Work out how many groups we're dealing with...
	INT32 HourglassCount = -1;
	if (Item != NULL)
	{
		SGDisplayGroup *GroupCountItem = (SGDisplayGroup *)Item->GetParent();

		while(GroupCountItem != NULL)
		{
			GroupCountItem = (SGDisplayGroup *)GroupCountItem->GetNext();
			HourglassCount ++;
		}
	}

	// Start up the hourglass
	Progress Hourglass(_R(IDS_SGSEARCH_SEARCHING), HourglassCount, TRUE);

	// This really needs moving out to sglbase sometime soon...
	BOOL ItsALibrary = ParentGallery->IsLibraryGalleryWithNonLibraryGroups();

	// We shouldn't really check for escape for every item
	INT32 ProgressCounterRate = 30;
	INT32 ProgressCounter = ProgressCounterRate;
	
	// Actually loop through the items and compare each with the given search string
	while(Item != NULL)
	{
		// Possibly update progress indicator; if ESCAPE pressed, abort!
		if(!(--ProgressCounter))
			ProgressCounter = ProgressCounterRate;
		else
		if(ProgressCounter == 1 && !Hourglass.Update(Hourglass.GetCount(), TRUE))
		{
			EndSlowJob();
			InformMessage(_R(IDE_SGSEARCH_ABORTED), _R(IDS_OK));
			return;
		}

		if(DoComparison(Item, &LastSearchText, UseKeywords, ItsALibrary, LastSearchFullInfo))
			break;

		if(HourglassCount == -1)
			Item = SearchFindNextItem(Item, NULL);
		else
			Item = SearchFindNextItem(Item, &Hourglass);
	}

	if(Item == NULL)
	{
		InformMessage(_R(IDE_SGSEARCH_NOMATCH), _R(IDS_OK));
		return;
	}

	ParentGallery->SelectItems(FALSE);			// Clear existing selection
	Item->Flags.Selected = TRUE;				// And select this item
	Item->ForceRedrawOfMyself();				// Ensuring it redraws itself

	ParentGallery->ForceGroupFolded((SGDisplayGroup *)Item->GetParent(), FALSE);

	DocRect ScrollToRect;
	Item->GetFormatRect(&ScrollToRect);
	ParentGallery->ScrollToShow(&ScrollToRect);	// Scroll to make seln visible on screen

	ParentGallery->SelectionHasChanged();		// Update gallery for the new selection
}





/********************************************************************************************

>	SGDisplayNode *SGallerySearchDlg::SearchFindNextItem(SGDisplayNode *LastItem, Progress *Hourglass = NULL)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> / Jason
	Created:	30/1/96

	Inputs:		LastItem	- The last item we compared
				Hourglass	- Optional Progress bar which will be incremented when jumping
								groups

	Returns:	The next item to compare

	Purpose:	Get the next item to compare

********************************************************************************************/

SGDisplayItem *SGallerySearchDlg::SearchFindNextItem(SGDisplayItem *LastItem, Progress *Hourglass)
{
	SGDisplayNode *Item = LastItem;

	if (Item->GetNext() == NULL)	// Skip to next group
	{
		ERROR3IF(Item->GetParent() == NULL, "Tree linkage corruption");

		// Make Item = Next group
		if (Item->GetParent()->GetNext() != NULL)
			Item = Item->GetParent()->GetNext();
		else
			Item = NULL;

		BOOL DevirtualisedGroups = FALSE;

		// Skip through groups until we find one with items
		while(Item != NULL && Item->GetChild() == NULL)
		{
			BOOL DeVirted = FALSE;
			if(((SGDisplayGroup *)Item)->IsVirtualised())
				DeVirted = ((SGDisplayGroup *)Item)->DeVirtualise();
	
			// Update the progress bar for each group jumped
			if(Hourglass)
				Hourglass->GetCount(1);
			
			DevirtualisedGroups = TRUE;

			if(!DeVirted)
				Item = Item->GetNext();
		}

		if(!DevirtualisedGroups && Hourglass)
		{
			// We jumped a group, but didn't bother with devirtualising stuff, because
			// we didn't need to, so update the progress bar
			Hourglass->GetCount(1);
		}

		// Get first item in new group
		if(Item != NULL)
			Item = Item->GetChild(); // Always get the first item in a group
	}
	else
		Item = Item->GetNext();		// Get next sibling item

	return (SGDisplayItem *)Item;
}


/********************************************************************************************

>	SGDisplayItem *SGallerySearchDlg::SearchFindStartItem(BOOL FindFirst)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com> / Jason
	Created:	30/1/96

	Inputs:		FindFirst	- Are we doing a find first or a continue from present position search ?
	Returns:	The first item to compare

	Purpose:	Get the first item to start the search with

********************************************************************************************/

SGDisplayItem *SGallerySearchDlg::SearchFindStartItem(BOOL FindFirst)
{
	SGDisplayNode *Item = ParentGallery->DisplayTree->FindNextSelectedItem(NULL);

	if (Item == NULL || FindFirst)
	{
		Item = ParentGallery->DisplayTree->GetChild();		// Find the first leaf node

		// Skip through groups until we find one with items
		while(Item != NULL && Item->GetChild() == NULL)
		{
			BOOL DeVirted = FALSE;
			if(((SGDisplayGroup *)Item)->IsVirtualised())
				DeVirted = ((SGDisplayGroup *)Item)->DeVirtualise();
	
			if(!DeVirted)
				Item = Item->GetNext();
		}
		
		while (Item != NULL && Item->GetChild() != NULL)
			Item = Item->GetChild();
	}
	else
	{
		// Find next item, skipping to the next group if necessary
		if (Item->GetNext() == NULL)	// Skip to next group
		{
			ERROR3IF(Item->GetParent() == NULL, "Tree linkage corruption");

			// Make Item = Next group
			if (Item->GetParent()->GetNext() != NULL)
				Item = Item->GetParent()->GetNext();
			else
				Item = NULL;

			// Skip through groups until we find one with items
			while(Item != NULL && Item->GetChild() == NULL)
			{
				BOOL DeVirted = FALSE;
				if(((SGDisplayGroup *)Item)->IsVirtualised())
					DeVirted = ((SGDisplayGroup *)Item)->DeVirtualise();
		
				if(!DeVirted)
					Item = Item->GetNext();
			}

			// Get first item in new group
			if(Item != NULL)
				Item = Item->GetChild();
		}
		else
			Item = Item->GetNext();		// Get next sibling item
	}

	return (SGDisplayItem *)Item;
}


/********************************************************************************************

>	BOOL SGallerySearchDlg::DoComparison(SGDisplayItem *Item, StringBase *SearchText,
		BOOL UseKeywords = FALSE, BOOL ItsALibrary = FALSE, BOOL FullInfo = FALSE)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/96

	Inputs:		Item		- The item which we want comparing with the string
				SearchText	- The string which we want comparing with the item
				UseKeywords	- Should we check keywords too ?
				ItsALibrary	- Are we a library item ?
				FullInfo	- Is this q quick search, or a full info search ?

	Returns:	True if we have a match, false otherwise

	Purpose:	The search routine's main comparison point. All items will go through this,
				so it must be fairly speedy in operation.

********************************************************************************************/

BOOL SGallerySearchDlg::DoComparison(SGDisplayItem *Item, StringBase *SearchText, BOOL UseKeywords, BOOL ItsALibrary, BOOL FullInfo)
{
	String_256 ItemText;

	// Set these if we did a quick compare via TCHAR *'s...
	BOOL CheckedTitle = FALSE;
	BOOL CheckedFilename = FALSE;

#if 0
	// If we are a library, we can use SpeedSearch(TM) technology...
	// Note that this won't do wildcard searches, but if we don't match, we do normal, slow,
	// searching later on...
	CheckedTitle = CheckedFilename = FALSE;
	if (ItsALibrary)
	{
		TCHAR *pSpeedyResult;
		BOOL CouldGetPointer = ((SGLibDisplayItem *)Item)->GetNameTextPtr(&pSpeedyResult);
		if (CouldGetPointer)
		{
			// Compare two title TCHAR *'s...
			if(CompareString(LOCALE_USER_DEFAULT,
				(NORM_IGNORECASE|NORM_IGNOREKANATYPE|NORM_IGNOREWIDTH),
				pSpeedyResult, -1, (TCHAR *)LastSearchText, -1) == 2)
				return TRUE;
			CheckedTitle = TRUE;
		}

		CouldGetPointer = ((SGLibDisplayItem *)Item)->GetFileNamePtr(&pSpeedyResult);
		if(CouldGetPointer)
		{
			// Compare two filename TCHAR *'s...
			if(CompareString(LOCALE_USER_DEFAULT,
				(NORM_IGNORECASE|NORM_IGNOREKANATYPE|NORM_IGNOREWIDTH),
				pSpeedyResult, -1, (TCHAR *)LastSearchText, -1) == 2)
				return TRUE;
			CheckedFilename = TRUE;
		}
	}
#endif

	// Check normal name
	if(!CheckedTitle)
	{
		Item->GetNameText(&ItemText);
		ItemText.toLower();
		if (ItemText.Sub(*SearchText, 0, TCHAR('*')) >= 0)
			return TRUE;
	}

	// Check filenames if told to
	if (ItsALibrary && !CheckedFilename)
	{
		((SGLibDisplayItem *)Item)->GetFileName(&ItemText);
		ItemText.toLower();
		if (ItemText.Sub(*SearchText, 0, TCHAR('*')) >= 0)
			return TRUE;
	}

	// For galleries which support it, check keywords
	if (UseKeywords)
	{
		Item->GetKeyWords(&ItemText);
		ItemText.toLower();

		// Check if there's a possibility of the string matching before the absolute check
		if (ItemText.Sub(*SearchText, 0, TCHAR('*')) >= 0)
		{
			// ItemText will be a string such as 'bat |animal|bird| mammal'
			// 'LastSearchText' will be a string such as 'animal'
			// We need to check whole words, so that 'nima' won't provide a match

			// change ItemText to be: '|bat||animal|bird||mammal|', then search for '|animal|'

			if (GenerateIndexFile::ReplaceCharacters(&ItemText, ' ', '|'))
			{
				String_256 NewItemText;
				NewItemText = _T("|");
				NewItemText += ItemText;
				NewItemText += _T("|");

				String_256 NewSearchText;
				NewSearchText = _T("|");
				NewSearchText += *SearchText;
				NewSearchText += _T("|");

				if (NewItemText.Sub(NewSearchText, 0, TCHAR('*')) >= 0)
					return TRUE;
			}
		}
	}

	// Check text in full info modes
	if (FullInfo)
	{
		Item->GetFullInfoText(&ItemText);
		ItemText.toLower();
		if (ItemText.Sub(*SearchText, 0, TCHAR('*')) >= 0)
			return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	MsgResult SGallerySearchDlg::Message( Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Inputs:		Msg - The dialogue manager message to handle

	Returns:	A MsgResult
	Purpose:	Standard DialogOp message handler, for the Gallery display dialogue

********************************************************************************************/

MsgResult SGallerySearchDlg::Message( Msg* Message)
{
	if (!(IS_OUR_DIALOG_MSG(Message))) return DialogOp::Message(Message);

	DialogMsg* TheMsg = (DialogMsg*)Message;

	switch(TheMsg->DlgMsg)
	{
		case DIM_CREATE:
			SetKeyboardFocus (_R(IDC_SGSEARCH_TEXT)) ;
			HighlightText    (_R(IDC_SGSEARCH_TEXT)) ;
			break ;
		case DIM_COMMIT:						// OK clicked
		case DIM_SOFT_COMMIT:					// OK "adjust clicked"
			Search(FALSE);
			
			// The 'OK' button ('Find Next') does not close the dialogue - it will stay
			// open until the user explicitly chooses 'Close'.
			break;

		case DIM_CANCEL:						// Cancel clicked
			Close();
			End();
			break;

		case DIM_LFT_BN_CLICKED:
			if (TheMsg->GadgetID == _R(IDC_SGSEARCH_FINDFIRST))
					Search(TRUE);
			break;

		default:
			break;
	}

	return DialogOp::Message(Message);

	return OK;
}



/********************************************************************************************

>	OpState	SGallerySearchDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Get the state of the Colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	SGallerySearchDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}



/********************************************************************************************

>	BOOL SGallerySearchDlg::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises the colour sort dialogue op
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL SGallerySearchDlg::Init()
{
	return (RegisterOpDescriptor(
 								0,
 								_R(IDS_SGSEARCHDLG),
								CC_RUNTIME_CLASS(SGallerySearchDlg),
 								OPTOKEN_SGSEARCHDLG,
 								SGallerySearchDlg::GetState,
 								_R(IDST_GALLERY_MENU),	// Status line help		**** !!!!
 								_R(IDBBL_GALLERY_MENU),	// Bubble help			**** !!!!
 								0	/* bitmap ID */
								));
}



/********************************************************************************************

>	BOOL SGallerySearchDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Returns:	FALSE if it fails
	Purpose:	Creates a gallery display dialogue box

********************************************************************************************/

BOOL SGallerySearchDlg::Create()
{
	ERROR3IF(ParentGallery == NULL, "My ParentGallery is NULL!");

	if (DialogOp::Create())
	{ 
		ParentGallery->CurrentSearchDlg = this;

		InitValues();
		return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	void SGallerySearchDlg::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		OPDescriptor - the Opdescriptor for this op

	Purpose:	'Does' a gallery display dialogue op.
				DO NOT call this method - it must be invoked via DoWithParam

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGallerySearchDlg::InvokeDialog

********************************************************************************************/

void SGallerySearchDlg::Do(OpDescriptor*)
{
	ERROR3("SGallerySearchDlg - You must use DoWithParam (Call InvokeDialog)");
	End();
}



/********************************************************************************************

>	void SGallerySearchDlg::DoWithParam(OpDescriptor *Bob, OpParam *Param)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		Bob - the Opdescriptor for this op
				Param - The parameter. Must point at a GalDlgParam object

	Purpose:	'Does' a gallery display dialogue op. Shows the dialogue.

	Notes:		To show this dialogue you should call InvokeDialog
																  
	SeeAlso:	SGallerySearchDlg::InvokeDialog

********************************************************************************************/

void SGallerySearchDlg::DoWithParam(OpDescriptor*, OpParam *Param)
{
	ERROR3IF(Param == NULL, "Null parameters are illegal");

	ParentGallery = ((GalDlgParam *)Param)->ParentGal;

	ERROR3IF(ParentGallery == NULL, "SGallerySearchDlg needs a non-NULL parent gallery!");	

	if (ParentGallery != NULL && Create())
		Open();
	else
		End();
}

