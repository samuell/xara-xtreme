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

// SGTEMPLATE.cpp - TEMPLATE SuperGallery classes - TEMPLATESGallery and SGDisplayTEMPLATE

// You must do the following things:
//	1 Copy this file into your own code file, and screw that up, not this! ;-)
//	2 Strip back the topmost line to say '// #Header#' (where # are dollar signs), and
//    clean up the log comments below to just '#Log#' (# are dollars again).
//	3 Change the #include marked below to include your new header file
//	3 Do a GLOBAL, CASE SENSITIVE replace of 'TEMPLATE' with your gallery name (eg 'Bitmap')
//	4 Do a CASE SENSITIVE replace of 'DATATYPE' with your gallery type (eg 'KernelBitmap')
//	  (This means a DisplayItem will hold a (KernelBitmap *) pointer, etc - search for
//	  DATATYPE before replacing to see what I mean)
//	5 Do a global CASE SENSITIVE replace of 'AUTHOR' with your name
//	6 Update bars.ini
//	7 Remember to add this new file to all.lst and kernel.mak
//	8 Delete these instructional comment lines
//	9 Take a look at the 'TO DO' comments to see what code you should fill in, and then
//	  go back and try instruction 8 again, becaus eyou've obviously skipped it! ;-)


/*
*/


DECLARE_SOURCE("$Revision$");


#include "camtypes.h"

#include "app.h"		// For GetApplication()
//#include "galstr.h"
#include "sgbase.h"		// **** INCLUDE YOUR HEADER HERE! ****

#include "ccdc.h"		// For render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"

//#include "richard2.h"	// For _R(IDS_SGBASE_)... example resource strings - remove this after making your own...

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(TEMPLATESGallery, SuperGallery)
CC_IMPLEMENT_DYNAMIC(SGDisplayDATATYPE, SGDisplayItem)
CC_IMPLEMENT_DYNCREATE(OpDisplayTEMPLATEGallery, Operation);


// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW




/***********************************************************************************************

>	SGDisplayDATATYPE::SGDisplayDATATYPE()

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Purpose:	SGDisplayDATATYPE constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGDisplayDATATYPE::SGDisplayDATATYPE()
{
	ERROR3("Illegal call on default SGDisplayDATATYPE constructor - call the other one!");
	TheDATATYPE = NULL;
}



/***********************************************************************************************

>	SGDisplayDATATYPE::SGDisplayDATATYPE(DATATYPE *DATATYPEToDisplay)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		DATATYPEToDisplay - The DATATYPE this item will display

	Purpose:	SGDisplayDATATYPE constructor

***********************************************************************************************/

SGDisplayDATATYPE::SGDisplayDATATYPE(DATATYPE *DATATYPEToDisplay)
{
	ERROR3IF(DATATYPEToDisplay == NULL,
				"SGDisplayDATATYPE - trying to construct me with a NULL parameter is bad");

	TheDATATYPE = DATATYPEToDisplay;
}



/***********************************************************************************************

>	virtual void SGDisplayDATATYPE::CalculateMyRect(SGFormatInfo *FormatInfo,
												SGMiscInfo *MiscInfo)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo - As usual, the useful misc info struct

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this DATATYPE item's display area. This is dependent upon the current display
				mode and format state

				FormatInfo will be updated as a result of the formatting operation

	Purpose:	Shared code for DATATYPE items to calculate where they will appear in the
				grand scheme of things

	Notes:		DATATYPEs supply only one display mode ("full info")

	Scope:		private (for use of SGDisplayDATATYPE class only)

***********************************************************************************************/

void SGDisplayDATATYPE::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	INT32 XSize = SG_InfiniteWidth;
	INT32 YSize = SG_DefaultLargeIcon;

	// TO DO
	// Fill in code to calculate the size you need for your display item, in millipoints.
	// Use the SG_ constants defined in sgtree.h where possible, to retain gallery consistency
	// This may depend upon the current display mode, as in:
	//	switch(MiscInfo->DisplayMode)
	//	{
	//		case 0:		// Large icons mode
	//			XSize = SG_InfiniteWidth;
	//			YSize = 100000;
	//	}

	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}



/********************************************************************************************

>	virtual void SGDisplayDATATYPE::GetNameText(String_256 *Result)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
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

void SGDisplayDATATYPE::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	*Result = _R(IDS_SGBASE_EXAMPLE_TEXT); // TEXT("Example Item");
}



/***********************************************************************************************

>	virtual void SGDisplayDATATYPE::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGFormatInfo *FormatInfo)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure

				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item

	Purpose:	SGDisplayDATATYPE item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.

	Notes:		**** TO DO **** This template method will draw an item as a red splodge
				with a constant text string ("Example Item") next to it. You need to replace
				this with proper redraw code. To see how different display modes are done
				take a look at the bitmap, layer, and colour supergalleries.

				VERY IMPORTANT: The rendering must be enclosed by calls to StartRendering
				and StopRendering, to ensure that rendering works properly (in the future
				we may change the redraw system to use a GRenderRegion for each individual
				item, rather than one global one for the window, for which these calls will
				be essential)

	Scope:		private

***********************************************************************************************/

void SGDisplayDATATYPE::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	// First, inform the system that we are about to start rendering this item
	StartRendering(RedrawInfo, MiscInfo);

	DocRect MyRect(FormatRect);		// Get my redraw position from the cached FormatRect

	RenderRegion *Renderer = RedrawInfo->Renderer;

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);
	INT32 TwoPixels = (INT32) DevicePixels(MiscInfo, 2);

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	// First, render the icon at the left end of our rectangle
	DocRect IconRect(MyRect);
	IconRect.hi.x = IconRect.lo.x + IconRect.Height();	// Make it a square
	MyRect.lo.x = IconRect.hi.x + TwoPixels;			// And exclude it from 'MyRect'

	// Redraw the icon
	GridLockRect(MiscInfo, &IconRect);			// Ensure it maps exactly to specific pixels
	IconRect.Inflate(-OnePixel, -OnePixel);		// Leave a bit of space around the edge

	Renderer->SetFillColour(DocColour(COLOUR_RED));
	Renderer->DrawRect(&IconRect);

	GridLockRect(MiscInfo, &MyRect);			// Ensure the new 'MyRect' is pixel-grid-aligned

	// Set up the colours for rendering our text, and fill the background if selected
	if (Flags.Selected)
	{
		// Fill the entire background with the 'selected' colour, so we don't
		// get gaps between bits of text or uneven rectangles in multiple selections
		Renderer->SetFillColour(RedrawInfo->SelBackground);
		Renderer->DrawRect(&MyRect);
		Renderer->SetFixedSystemTextColours(&RedrawInfo->SelForeground, &RedrawInfo->SelBackground);
	}
	else
		Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);

	MyRect.lo.x += SG_GapBeforeText;	// Leave a small gap before text begins

	// And render the text
	String_256 MyText;
	GetNameText(&MyText);
	Renderer->DrawFixedSystemText(&MyText, MyRect);				

	// Finally, inform the system that we have completed rendering this item
	StopRendering(RedrawInfo, MiscInfo);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayDATATYPE::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

					MonoOn
					SGEVENT_FORMAT		NULL
					SGEVENT_REDRAW		(SGRedrawInfo *)
					SGEVENT_BGREDRAW	NULL
					SGEVENT_BGFLUSH		NULL					- May have NULL MiscInfo
					SGEVENT_MOUSECLICK	(SGMouseInfo *)
					SGEVENT_DRAGSTARTED	(DragMessage *)
					SGEVENT_CLAIMPOINT	(SGMouseInfo *)
					SGEVENT_THUMBMSG	(ThumbMessage *)		- May have NULL MiscInfo
					MonoOff

				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - almost always provided. Contains a few useful bits of info that may be
				needed for all event types. This may be null for special event types (see sgtree.h
				for the enum and information on which ones may pass NULL MiscInfo - currently
				this is _THUMBMSG and _BGFLUSH, neither of which should concern you - so as long
				as you only reference MiscInfo once you know the event type, you will be safe)

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return by calling the base class HandleEvent method, so that
				default actions for new/unknown events can be supplied by the base class.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

	SeeAlso:	SGDisplayNode::HandleEvent; SGDisplayItem::HandleEvent

***********************************************************************************************/

BOOL SGDisplayDATATYPE::HandleEvent(SGEventType EventType, void *EventInfo,
								  SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_FORMAT:
			{
				SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);
				CalculateMyRect(FormatInfo, MiscInfo);		// Cache our FormatRect for later use
			}
			break;


		case SGEVENT_REDRAW:
			{
				DocRect MyRect(FormatRect);		// Rely on FormatRect being cached from above
				SGRedrawInfo *RedrawInfo = GetRedrawInfo(EventType, EventInfo);

				if (IMustRedraw(RedrawInfo))	// only redraw if we intersect the clip rect
					HandleRedraw(RedrawInfo, MiscInfo);
			}
			break;		// exit and return FALSE to pass the redraw event on


		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					// Test for drag...		**** !!!! ToDo !!!! ****
					// DefaultDragHandler(Mouse, MiscInfo);

					// No drag, so move on to normal selection click handling
					DefaultClickHandler(Mouse, MiscInfo);

					return(TRUE);		// Claim this event - nobody else can own this click
				}
			}
			break;

		default:
			// Let the base class handle any events we don't know about.
			// This includes things like hit testing (CLAIMPOINT) etc
			return(SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return(FALSE);
}
















/********************************************************************************************

>	TEMPLATESGallery::TEMPLATESGallery()
												 
	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	TEMPLATESGallery default constructor

********************************************************************************************/

TEMPLATESGallery::TEMPLATESGallery()
{
} 



/********************************************************************************************

>	TEMPLATESGallery::~TEMPLATESGallery()

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Purpose:	TEMPLATESGallery destructor.

********************************************************************************************/

TEMPLATESGallery::~TEMPLATESGallery()
{
}



/********************************************************************************************

>	void TEMPLATESGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		ParentDoc - The document to create a display subtree for
				ExistingGroup - NULL (creates a new group for this document), or
				a pointer to the existing group-node for this document (in which case
				it clears all displayitems from the group and rebuilds it in place - this
				stops the display group moving around the tree at random!)

	Purpose:	Internal call. This takes the DATATYPE list of the given document and 
				creates a DisplayTree subtree from it. This subtree is then added to
				the DisplayTree. Note that this does not force a redraw of the list - 
				after making this call, you should also call ForceRedrawOfList

	Notes:		Passing in a NULL parent document pointer results in an ERROR3 -
				the function returns without doing anything in retail builds

	SeeAlso:	SuperGallery::ForceRedrawOfList

********************************************************************************************/

void TEMPLATESGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup)
{
	ERROR3IF(ParentDoc == NULL, "TEMPLATESGallery::CreateNewSubtree - NULL parameter passed in");
	if (ParentDoc == NULL)
		return;

	SGDisplayGroup  *DisplayDocument;
	SGDisplayDATATYPE *DisplayDATATYPE;

	if (ExistingGroup != NULL)
	{
		ERROR3IF(ExistingGroup->GetParentDocument() != ParentDoc, 
				 "This group is not for that document! What's going down, dude?");
		DisplayDocument = ExistingGroup;							// Use existing group
		DisplayDocument->DestroySubtree(FALSE);	// Wipe any existing DATATYPE display items
	}
	else
	{
		DisplayDocument = new SGDisplayGroup(this, ParentDoc,NULL);	// Create new Group

		if (DisplayDocument == NULL)								// Total failure - abort
			return;

		DisplayTree->AddItem(DisplayDocument);						// Add new group to tree
	}


	// **** TO DO ****
	// Fill in some code here to generate SGDisplayDATATYPEs to be displayed.
	// e.g: (in PSEUDO code!! ;-)
	//
	// DATATYPE *Ptr = ParentDoc->GetDATATYPEList()->GetHead();
	// while (Ptr != NULL)
	// {
	//    DisplayDATATYPE = new SGDisplayDATATYPE(Ptr);
	//    if (DisplayDATATYPE != NULL)
	//       DisplayDocument->AddItem(DisplayDATATYPE);
	//
	//    Ptr = ParentDoc->GetDATATYPEList()->GetNext();
	// }
	//
	//
	// Default : create a few items to display
	// NOTE: The pointer faked up to pass in to the constructor will cause the thing
	// to blow up if you start actually trying to USE the data item! Replace this at
	// earliest convenience with proper tree construction code.
	for (INT32 Bob = 0; Bob < 7; Bob++)
	{
		DisplayDATATYPE = new SGDisplayDATATYPE( (DATATYPE *) 0xcdcdcdcd );
		if (DisplayDATATYPE != NULL)
			DisplayDocument->AddItem(DisplayDATATYPE);
	}
}



/********************************************************************************************

>	BOOL TEMPLATESGallery::PreCreate(void)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The TEMPLATESGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very beginning of the
				SuperGallery::Create method, before the window has been created.

	Notes:		As this is called before the window is open, it must not attempt to touch
				any of the button gadgets in the window, or force redraws, etc. Also,
				events cannot be passed to the tree, as the tree formatting relies on
				knowing the window size - however, the tree will be reformatted and
				redrawn automatically when the window is opened - this will happen shortly.

********************************************************************************************/

BOOL TEMPLATESGallery::PreCreate(void)
{
	// If there isn't already one, create a DisplayTree
	if (DisplayTree == NULL)
	{
		DisplayTree = new SGDisplayRootScroll(this);	// New root node, with a scrollbar
		if (DisplayTree == NULL)
			return(FALSE);
	}


	// **** TO DO ****
	// You may not need to adjust the following, but note that it re-creates the entire
	// display tree from scratch whenever the gallery window is opened - if this is
	// unsuitable for your needs, you'll have to change it (e.g. you could make it only
	// do it if the DisplayTree has not already been created, above)

	// And for each document already present, create a display subtree (possibly replacing
	// a previous displaytree for it if we had one earlier)
	Document *ParentDoc = (Document *) GetApplication()->Documents.GetTail();
	while (ParentDoc != NULL)
	{
		// Find the existing subtree, if any
		SGDisplayGroup *ParentGroup = DisplayTree->FindSubtree(this, ParentDoc, NULL);

		// Create a new subtree, replacing the old one if possible
		CreateNewSubtree(ParentDoc, ParentGroup);

		ParentDoc = (Document *) GetApplication()->Documents.GetPrev(ParentDoc);
	}

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL TEMPLATESGallery::ApplyAction(SGActionType Action)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL TEMPLATESGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Better forget about it then!
	if (DisplayTree == NULL)
		return(FALSE);

	// Determine useful info - this is usually needed for most actions, so always get it
	Document		*SelectedDoc = Document::GetSelected();
	SGDisplayGroup	*DocumentGroup = DisplayTree->FindSubtree(this, SelectedDoc, NULL);
	SGDisplayDATATYPE	*FirstSelected = NULL;
	
	if (DocumentGroup != NULL)
		FirstSelected = (SGDisplayDATATYPE *) DocumentGroup->FindNextSelectedItem(NULL);

	// Now, process the action	TO DO! - see Colour gallery for examples
	switch(Action)
	{
		case SGACTION_CREATE:
TRACE( _T("Unimplemented feature executed in TEMPLATE gallery"));
			break;

		case SGACTION_APPLY:
TRACE( _T("Unimplemented feature executed in TEMPLATE gallery"));
			break;

		case SGACTION_REDEFINE:
TRACE( _T("Unimplemented feature executed in TEMPLATE gallery"));
			break;

		case SGACTION_EDIT:
TRACE( _T("Unimplemented feature executed in TEMPLATE gallery"));
			break;

		case SGACTION_DELETE:
TRACE( _T("Unimplemented feature executed in TEMPLATE gallery"));
			break;

		case SGACTION_DISPLAYMODECHANGED:
TRACE( _T("Unimplemented feature executed in TEMPLATE gallery"));
			break;

		default:
			return(FALSE);
	}

	return(TRUE);
}



/********************************************************************************************

>	virtual MsgResult TEMPLATESGallery::Message(Msg* Message)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95 (base generated in sgbase.cpp)
	Inputs:		Message - The message to handle

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

	SeeAlso:	SuperGallery::Message

********************************************************************************************/

MsgResult TEMPLATESGallery::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYTEMPLATEGALLERY, TRUE);
				break;

			case DIM_CANCEL:
				SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYTEMPLATEGALLERY, FALSE);
				break;
		}

		return(SuperGallery::Message(Message));
	}

	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL)
		return(SuperGallery::Message(Message));


	// **** TO DO ****
	// Just another note: Most galleries ignore most messages when closed (!IsVisible())
	// This means that they must re-create themselves properly to get back up to date
	// when they are opened - this is done in the PostCreate handler. If you wish to 
	// respond to messages to keep up to date while closed, you should address the
	// default code in the PostCreate handler as well.

	if (IsVisible() && MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocChangingMsg::DocState::BORN:			// New document - add to display tree
				{
					INT32 Extent = GetDisplayExtent();		// Remember previous list extent

					CreateNewSubtree(TheMsg->pChangingDoc);	// Add a subtree for this doc
					ShadeGallery(FALSE);					// Unshade the gallery

					InvalidateCachedFormat();				// And redraw what is necessary
					RedrawEverythingBelow(-Extent);
				}
				break;		// Pass this message on to the base class as well
		}
	}

	return(SuperGallery::Message(Message));
}    
















/********************************************************************************************

>	BOOL OpDisplayTEMPLATEGallery::Init()

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayTEMPLATEGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayTEMPLATEGallery::Init()
{

	// **** TO DO ****
	// Ensure the IDS_ OPTOKEN_ and IDBBL_ constants are available and correct
	// You should really also uppercase these constant names.

	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_TEMPLATE_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayTEMPLATEGallery),
	 							OPTOKEN_DISPLAYTEMPLATEGALLERY,
	 							OpDisplayTEMPLATEGallery::GetState,
	 							0,	/* help ID */
	 							_R(IDBBL_DISPLAY_TEMPLATE_GALLERY),
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpDisplayTEMPLATEGallery::GetState(String_256*, OpDescriptor*)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayTEMPLATEGallery operation
	Purpose:	For finding the OpDisplayTEMPLATEGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayTEMPLATEGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  

	// If the gallery is currenty open, then the menu item should be ticked
	DialogBarOp* pDialogBarOp = FindGallery();
	if (pDialogBarOp != NULL)
		OpSt.Ticked = pDialogBarOp->IsVisible();

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == NULL);
 	return(OpSt);   
}



/********************************************************************************************

>	void OpDisplayTEMPLATEGallery::Do(OpDescriptor*)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the TEMPLATEs gallery
				Updates the button state for this Op (the button sticks down while the
				gallery is open)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayTEMPLATEGallery::Do(OpDescriptor*)
{
	DialogBarOp *pDialogBarOp = FindGallery();

	if (pDialogBarOp != NULL)
	{
		// Toggle the visible state of the gallery window
		pDialogBarOp->SetVisibility( !pDialogBarOp->IsVisible() );

		// And update the gallery button state
		SGInit::UpdateGalleryButton(OPTOKEN_DISPLAYTEMPLATEGALLERY, pDialogBarOp->IsVisible());
	}
	
	End();
}



/********************************************************************************************

>	static DialogBarOp *OpDisplayTEMPLATEGallery::FindGallery(void)

	Author:		Unattributed (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)

	Returns:	NULL or a pointer to the TEMPLATE gallery instance

	Purpose:	Finds the TEMPLATE gallery class instance

	Notes:		The bars system always keeps one TEMPLATE gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the TEMPLATESGallery class

********************************************************************************************/

DialogBarOp *OpDisplayTEMPLATEGallery::FindGallery(void)
{
	String_32 Name = _R(IDS_SGBASE_TEMPLATE_GALLERY); //"TEMPLATE gallery";
	DialogBarOp* pDialogBarOp = DialogBarOp::FindDialogBarOp(Name);

	if (pDialogBarOp != NULL)
	{
		if (pDialogBarOp->GetRuntimeClass() == CC_RUNTIME_CLASS(TEMPLATESGallery))
			return(pDialogBarOp);

		ERROR3("Got the TEMPLATE gallery but it's not of the TEMPLATESGallery class");
	}
	else
	{
		ERROR3("Can't find the TEMPLATE gallery in bars.ini!\n");
	}

	return(NULL);
}

