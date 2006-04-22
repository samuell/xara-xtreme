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
// SGColour.cpp - Colour SuperGallery classes - ColourSGallery and SGDisplayColour

#include "camtypes.h"

#include "attrmgr.h"
#include "app.h"
#include "ccfile.h"
#include "colcomp.h"
#include "collist.h"
#include "colourix.h"
#include "colormgr.h"
//#include "colpick.h"
#include "comattrmsg.h"
#include "docview.h"
#include "dragcol.h"
#include "dragmgr.h"
#include "fileutil.h"
#include "fillattr.h"
#include "filters.h"
//#include "galres.h"
//#include "galstr.h"
//#include "isetres.h"
//#include "jason.h"
#include "lineattr.h"
//#include "markn.h"
//#include "newcol.h"
//#include "resdll.h"
//#include "resource.h"	// For _R(IDS_CANCEL)
#include "sgcolour.h"
#include "sgdrag.h"
#include "sginit.h"
#include "sgliboil.h"
#include "sgmenu.h"

#include "ccdc.h"		// For render-into-dialogue support
#include "fillval.h"
#include "grnddib.h"
//#include "richard2.h"	// resources to remove naughty inline strings
#include "ccolbar.h"	// EnsureLibraryColoursPresent
#include "backgrnd.h"	// OpBackground

// WEBSTER - markn 31/1/97
#include "helpuser.h"
//#include "xshelpid.h"
//#include "helppath.h"

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNCREATE(ColourSGallery, SuperGallery)
CC_IMPLEMENT_DYNAMIC(SGDisplayColour, SGDisplayItem)
CC_IMPLEMENT_DYNAMIC(SGDisplayLibColour, SGDisplayColour)
CC_IMPLEMENT_DYNAMIC(SGDisplayColourGroup, SGDisplayGroup)
CC_IMPLEMENT_DYNAMIC(SGDisplayLibColGroup, SGDisplayColourGroup)
CC_IMPLEMENT_DYNCREATE(OpDisplayColourGallery, Operation)
CC_IMPLEMENT_DYNCREATE(GalleryColourDragInfo, ColourDragInformation)
CC_IMPLEMENT_DYNAMIC(SGColourDragTarget, SGListDragTarget);
CC_IMPLEMENT_DYNCREATE(ColourNameDlg, DialogOp)
CC_IMPLEMENT_MEMDUMP(ColourNameParam, OpParam)


// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW


// Nasty PANTONE bodge - after any change of ordering in any library, the 
// Pantone library no longer puts line breaks after every 9th item.
static BOOL LibraryHasBeenSorted = FALSE;

const INT32 SG_ColourNameWidth = 127000;		// Width of colour name field (just long enough
											// for "Pantone Process Magenta CV"!!)

// This always points to the colour gallery, or NULL if it doesn't exist.
ColourSGallery* ColourSGallery::m_pTheGallery = NULL;

/********************************************************************************************

	Preference:		AutoScrollColGal
	Section:		Displays
	Range:			TRUE or FALSE
	Purpose:		If TRUE, the colour gallery will automatically scroll its display list
					to show the currently selected line and fill colours whenever the
					selection changes. If FALSE, these colours will still become selected,
					but the gallery will not be scrolled.

					This is because it can be annoying that the gallery keeps scrolling
					away from a colour which you were about to apply to the selection (though
					you'd be better off dragging the colour, rather than selecting different
					objects in this case)

	Notes:			Normal preference, available from the options dialogue.
	SeeAlso:		ColourSGallery

********************************************************************************************/

#ifdef WEBSTER
// In Webster don't scroll to the selected colour by default
INT32 ColourSGallery::AutoScrollSelection = FALSE;
#else
// In Camelot do scroll to the selected colour by default
INT32 ColourSGallery::AutoScrollSelection = TRUE;
#endif


/********************************************************************************************

	Preference:		ColourDisplayMode
	Section:		Displays
	Range:			0..2
	Purpose:		Memory of the current colour gallery display mode, saved between
					sessions for convenience. Not shown in the options dialogue.
	SeeAlso:		ColourSGallery

********************************************************************************************/

INT32 ColourSGallery::DefaultDisplayMode = 0;

/********************************************************************************************

	Preference:		ShowDocumentColours
	Section:		ColourLine
	Range:			TRUE, FALSE (0,1)
	Purpose:		Memory of the current state of showing document colours in the colour
					line.
	SeeAlso:		ColourSGallery

********************************************************************************************/

// Webster and Camelot's default should be the same (True).
BOOL ColourSGallery::ShowDocumentColours = TRUE;


/********************************************************************************************

	Preference:		ShowNetscapeColours
	Section:		ColourLine
	Range:			TRUE, FALSE (0,1)
	Purpose:		Memory of the current state of showing Netscape colours in the colour
					line.
					Webster needs to default to True.
	SeeAlso:		ColourSGallery

********************************************************************************************/

// True if want Netscape colours being shown in colour line
// Webster only at present
BOOL ColourSGallery::ShowNetscapeColours = TRUE;

/********************************************************************************************

	Preference:		Path
	Section:		Palette
	Range:			0 .. 256 characters
	Purpose:		The path to load the palettes from. Defaults to blank which means alongside
					the exe.
	SeeAlso:		ColourSGallery

********************************************************************************************/

String_256 ColourSGallery::PalettePath = TEXT("");


/********************************************************************************************

>	SGColourDragTarget::SGColourDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL)
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		TheDialog - The kernel dialog in which the target exists
				TheGadget - The gadget within that dialogue which is the target

	Purpose:	Constructor

********************************************************************************************/

SGColourDragTarget::SGColourDragTarget(DialogOp *TheDialog, CGadgetID TheGadget)
					: SGListDragTarget(TheDialog, TheGadget)
{
	ERROR3IF(!TheDialog->IsKindOf(CC_RUNTIME_CLASS(ColourSGallery)),
			"You can only use SGColourDragTargets with ColourSGallery dialogues!");
}



/********************************************************************************************

	BOOL SGColourDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
										OilCoord *pMousePos, KeyPress* pKeyPress)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95
	Inputs:		Event - Indicates what has happened
				pDragInfo - points to drag information describing this drag. This
				should be a ColourDragInformation or derivation thereof
				pMousePos - points to information on the current mouse position, in OIL coords
				pKeyPress - NULL, or if for a keypress event, keypress information

	Returns:	TRUE to claim the event, FALSE to let it through to other targets

	Purpose:	Event Handler for SuperGallery listitem drag events. Overrides the
				base class handler to enable it to sort out the node being dragged
				for colour drags.

********************************************************************************************/

BOOL SGColourDragTarget::ProcessEvent(DragEventType Event, DragInformation *pDragInfo,
									OilCoord *pMousePos, KeyPress* pKeyPress)
{
	if (!pDragInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
		return(FALSE);

	SGDisplayNode *DraggedNode = NULL;
	BOOL IsSimpleColourDrag = FALSE;

	if (IS_A(pDragInfo, ColourDragInformation))
	{
		ColourDragInformation *ColourDragInfo = (ColourDragInformation *) pDragInfo;

		SuperGallery *ParentGallery = (SuperGallery *)TargetDialog;

		IsSimpleColourDrag = TRUE;

		// Search the display tree for an item which displays the dragged IndexedColour
		IndexedColour *DraggedColour = ColourDragInfo->GetInitiallyDraggedColour();

		// If the colour is NULL, it's "no colour" (or possibly some weird library colour which
		// we didn't start the drag of, because we'd use a GalleryColourDragInfo), so ignore it
		if (DraggedColour == NULL)
			return(FALSE);

		SGDisplayRoot *DisplayTree = ParentGallery->GetDisplayTree();
		if (DisplayTree == NULL)		// No tree?!
			return(FALSE);

		// Scan the display tree for the document in which the colour resides, to find
		// the display item that references the colour. If we weren't given a parent
		// document, then we'll scan the entire thing.
		// If we find it, we exit this bit with DraggedNode pointing at the relevant
		// SGDisplayNode item for it, and drop through to the regular handler
		BOOL Found = FALSE;

		SGDisplayNode *TheGroup = NULL;		// Find an appropriate group to start searching
		if (ColourDragInfo->GetParentDoc() != NULL)
		{
			TheGroup = DisplayTree->FindSubtree(ParentGallery,
								((ColourDragInformation *) pDragInfo)->GetParentDoc(), NULL);
		}
		else
			TheGroup = DisplayTree->GetChild();

		DocColour *TheDocCol;
		while (TheGroup != NULL && !Found)
		{
			DraggedNode = TheGroup->GetChild();
			while (DraggedNode != NULL)
			{
				TheDocCol = ((SGDisplayColour *)DraggedNode)->GetDisplayedColour();
				if (TheDocCol != NULL && TheDocCol->FindParentIndexedColour() == DraggedColour)
				{
					Found = TRUE;
					break;
				}

				DraggedNode = DraggedNode->GetNext();
			}

			TheGroup = TheGroup->GetNext();
		}

		if (!Found)			// No colour matching that description is in the gallery
			return(FALSE);
	}
	else if (IS_A(pDragInfo, GalleryColourDragInfo))
	{
		DraggedNode = ((GalleryColourDragInfo *)pDragInfo)->GetDraggedColour();
	}
	// else ignore the drag (as DraggedNode will still be NULL)

	if (DraggedNode != NULL)
	{
		switch(Event)
		{
			case DRAGEVENT_COMPLETED:
				HandleDragCompleted((SuperGallery *) TargetDialog,
									DraggedNode, pMousePos, IsSimpleColourDrag);
				return(TRUE);


			case DRAGEVENT_MOUSESTOPPED:
			case DRAGEVENT_MOUSEMOVED:
			case DRAGEVENT_MOUSEIDLE:
				// Call a subroutine to work out and set our current cursor shape
				return(DetermineCursorShape((SuperGallery *) TargetDialog,
											DraggedNode, pMousePos));
			default:
				break;
		}
	}

	// Otherwise, we aren't interested in the event, so we don't claim it
	return(FALSE);
}









/********************************************************************************************

>	void GalleryColourDragInfo::GalleryColourDragInfo() 
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95		  

	Purpose:	Default constructor - do not call this constructor

********************************************************************************************/

GalleryColourDragInfo::GalleryColourDragInfo()
{
	ERROR3("Default GalleryColourDragInfo constructor called");	
}



/********************************************************************************************

>	GalleryColourDragInfo::GalleryColourDragInfo(Document *pDocument, SGDisplayColour *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust = FALSE)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95		  

	Inputs:		pDocument	- The document in which the colour lives
				pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust drag

	Purpose:	Constructor - use this one for "document colours" - ones that already live
				in a document. The Display item's colour MUST reference an IndexedColour.

********************************************************************************************/

GalleryColourDragInfo::GalleryColourDragInfo(Document *pDocument, SGDisplayColour *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust)
					  : ColourDragInformation(pSourceItem->GetDisplayedColour()->FindParentIndexedColour(),
												IsAdjust, pDocument)
{
	ERROR3IF(pSourceItem == NULL || pMouseInfo == NULL || pMiscInfo == NULL,
				"GalleryColourDragInfo constructor: Illegal NULL param");

	SourceItem	= pSourceItem;	// Copy the source item pointer

	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}



/********************************************************************************************

>	GalleryColourDragInfo::GalleryColourDragInfo(SGDisplayColour *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust = FALSE)
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95		  

	Inputs:		pSourceItem - The gallery item from which the drag originated
				pMouseInfo	- The mouse info which made the item start the drag
				pMiscInfo	- The MiscInfo which accompanied the mouse event
				IsAdjust	- TRUE if this is an adjust drag

	Purpose:	Constructor - use this for drags of "library colours" (ones with
				no parent document)

********************************************************************************************/

GalleryColourDragInfo::GalleryColourDragInfo(SGDisplayColour *pSourceItem,
											SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 											BOOL IsAdjust)
					  : ColourDragInformation(pSourceItem->GetDisplayedColour(), IsAdjust,
												NULL, pSourceItem->IsASpotColour())
{
	ERROR3IF(pSourceItem == NULL || pMouseInfo == NULL || pMiscInfo == NULL,
				"GalleryColourDragInfo constructor: Illegal NULL param");

	// Fill in the base class' name member, which we passed NULL for in the construction above
	String_256 Bob;
	pSourceItem->GetNameText(&Bob);
	Bob.Left(&ColourName, 63);

	SourceItem	= pSourceItem;	// Copy the source item pointer

	MouseInfo	= *pMouseInfo;	// Duplicate the structures (they may cease to exist
	MiscInfo	= *pMiscInfo;	// soon after the drag is started)
}



/********************************************************************************************

>	virtual void GalleryColourDragInfo::OnClick(INT32 Flags, POINT Point) 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. It calls back the SourceItem SGDisplayColour, to get it
				to handle the click. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void GalleryColourDragInfo::OnClick(INT32 Flags, POINT Point)
{
	if (SourceItem != NULL)
		SourceItem->DragWasReallyAClick(&MouseInfo, &MiscInfo);
}









/***********************************************************************************************

>	SGDisplayColour::SGDisplayColour()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Purpose:	SGDisplayColour constructor
				DON'T call this constructor. Call the other constructor

***********************************************************************************************/

SGDisplayColour::SGDisplayColour()
{
	// We don't like people calling this constructor, but we have to put up with it
	// now that we have a derived class.
}



/***********************************************************************************************

>	SGDisplayColour::SGDisplayColour(IndexedColour *ColourToDisplay)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/94

	Inputs:		ColourToDisplay - The colour this item will display

	Purpose:	SGDisplayColour constructor

***********************************************************************************************/

SGDisplayColour::SGDisplayColour(IndexedColour *ColourToDisplay)
{
	TheColour.MakeRefToIndexedColour(ColourToDisplay);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayColour::CompareTo(SGDisplayNode *Other, INT32 SortKey)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95

	Inputs:		Other - the Colour to compare this Colour to
				SortKey - An integer identifying how to compare the items
					0 = No sorting (always returns 0)
					1 = Sort-by-name
					2 = Sort-by-hue
					3 = Sort-by-intensity
					4 = Sort-by-model

	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)

	Purpose:	Compares this Colour to the 'other' Colour, to determine their relative positions
				in the display tree. Returns a value which usually indicates that the other
				Colour should be inserted before (-1, or 0) or after (+1) this item.

	SeeAlso:	SGDisplayNode::CompareTo

***********************************************************************************************/

const INT32 SGSORTKEY_BYHUE = 2;
const INT32 SGSORTKEY_BYINTENSITY = 3;
const INT32 SGSORTKEY_BYMODEL = 4;

INT32 SGDisplayColour::CompareTo(SGDisplayNode *Other, INT32 SortKey)
{
	ERROR3IF(Other == NULL, "Illegal NULL parameter");

	ERROR3IF(!Other->IsKindOf(CC_RUNTIME_CLASS(SGDisplayColour)),
				"SGDisplayColour::CompareTo - The other item's not a colour!");

	DocColour *OtherCol = ((SGDisplayColour *)Other)->GetDisplayedColour();
	DocColour *ThisCol  = GetDisplayedColour();

	switch (SortKey)
	{
		case SGSORTKEY_BYNAME:
			{
				IndexedColour *OtherColIx = OtherCol->FindParentIndexedColour();
				IndexedColour *ThisColIx  = ThisCol->FindParentIndexedColour();

				// Safety check - if either colour is NULL, then return "equal"
				if (OtherColIx == NULL || ThisColIx == NULL)
					return(0);

				// We override the base class name comparison, because it is forced to
				// actually copy the names first! This one just compares them in-place
				return(ThisColIx->GetName()->CompareTo( (TCHAR *) (*(OtherColIx->GetName())) ) );
			}
			break;


		case SGSORTKEY_BYHUE:
		{
			ColourContextHSVT	*ColContext;
			ColourHSVT	ResultCol;
			ColourValue	Hue1;

			Document *ParentDoc = ((SGDisplayGroup *)GetParent())->GetParentDocument();

			// Get a default context, or if ParentDoc != NULL, the current context for the doc.
			ColContext = (ColourContextHSVT *) ColourManager::GetColourContext(COLOURMODEL_HSVT, ParentDoc);
			if (ColContext == NULL)
				return(0);

			// Convert the 2 colours to HSV and compare their Hue values
			ColContext->ConvertColour(ThisCol, (ColourGeneric *) &ResultCol);
			Hue1 = ResultCol.Hue;

			ColContext->ConvertColour(OtherCol, (ColourGeneric *) &ResultCol);

			if (Hue1 < ResultCol.Hue)
				return(-1);

			return((Hue1 == ResultCol.Hue) ? 0 : 1);
		}


		case SGSORTKEY_BYINTENSITY:
		{
			ColourContextGreyT	*ColContext;
			ColourGreyT			ResultCol;
			ColourValue			Intensity1;

			Document *ParentDoc = ((SGDisplayGroup *)GetParent())->GetParentDocument();

			// Get a default context, or if ParentDoc != NULL, the current context for the doc.
			ColContext = (ColourContextGreyT *) ColourManager::GetColourContext(COLOURMODEL_GREYT, ParentDoc);
			if (ColContext == NULL)
				return(0);

			// Convert the 2 colours to Greyscale and compare their intensity values
			ColContext->ConvertColour(ThisCol, (ColourGeneric *) &ResultCol);
			Intensity1 = ResultCol.Intensity;

			ColContext->ConvertColour(OtherCol, (ColourGeneric *) &ResultCol);

			if (Intensity1 < ResultCol.Intensity)
				return(-1);

			return((Intensity1 == ResultCol.Intensity) ? 0 : 1);
		}


		case SGSORTKEY_BYMODEL:
			return((INT32)ThisCol->GetColourModel() - (INT32)OtherCol->GetColourModel());
	}

	// No sorting (SGSORTKEY_NONE - 0), or unknown
	return(SGDisplayItem::CompareTo(Other, SortKey));
}



/********************************************************************************************

>	virtual void SGDisplayColour::GetNameText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the name text associated with this item (if any)

	Purpose:	To determine a name string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose names match
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	SeeAlso:	SGDisplayColour::GetFullInfoText

********************************************************************************************/

void SGDisplayColour::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	IndexedColour *ParentCol = TheColour.FindParentIndexedColour();
	if (ParentCol != NULL)
		*Result = *(ParentCol->GetName());
	else
		Result->MakeMsg(_R(IDS_UNNAMEDCOLOUR));
}



/********************************************************************************************

>	virtual void SGDisplayColour::GetFullInfoText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the full-information text associated with this item (if any)

	Purpose:	To determine a full-info string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose info matches
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	SeeAlso:	SGDisplayColour::GetNameText

********************************************************************************************/

void SGDisplayColour::GetFullInfoText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	*Result = TEXT("");		// Set a safe default

	// Generate an info string of the form:
	// <ColModel> (<components>) [, <tint/link/spot information>] [, not in use]
	// e.g.		"HSV(324, 80, 75), Linked to Red"

	String_256 Info(TEXT(""));

	ColourContext *cc = ColourContext::GetGlobalDefault(TheColour.GetColourModel());
	ERROR3IF(cc == NULL, "Default colour context is NULL!?");

	ColourGeneric col;
	TheColour.GetSourceColour(&col);
	cc->GetModelName(&Info);

	String_128	TempStr(TEXT("("));
	String_16	CompStr;
	if (cc->GetComponentName(1, NULL))
	{
		double CompVal;
		if (TheColour.GetColourModel() == COLOURMODEL_HSVT)
			CompVal = (col.Component1.MakeDouble()) * 360.0;
		else
			CompVal = (col.Component1.MakeDouble()) * 100.0;

		// Convert it to a string rounded to 1 decimal place accuracy
		Convert::DoubleToString(CompVal, &CompStr, 1);

		TempStr += CompStr;
	}

	if (cc->GetComponentName(2, NULL))
	{
		Convert::DoubleToString(col.Component2.MakeDouble() * 100.0, &CompStr, 1);
		TempStr += TEXT(", ");
		TempStr += CompStr;
	}

	if (cc->GetComponentName(3, NULL))
	{
		Convert::DoubleToString(col.Component3.MakeDouble() * 100.0, &CompStr, 1);
		TempStr += TEXT(", ");
		TempStr += CompStr;
	}

	if (cc->GetComponentName(4, NULL))
	{
		Convert::DoubleToString(col.Component4.MakeDouble() * 100.0, &CompStr, 1);
		TempStr += TEXT(", ");
		TempStr += CompStr;
	}

	TempStr	+= TEXT(")");
	Info	+= TempStr;

	IndexedColour *ParentCol = TheColour.FindParentIndexedColour();		// If this refs an IndexedColour...
	if (ParentCol != NULL)
	{
		switch(ParentCol->GetType())
		{
			case COLOURTYPE_SPOT:
				TempStr.MakeMsg(_R(IDS_COLGAL_ISPOT));
				Info += TempStr;
				break;

			case COLOURTYPE_TINT:
				{
					ERROR3IF(ParentCol->FindLinkedParent() == NULL,
							"A tint without a parent colour? Oh bums!");

					if (ParentCol->TintIsShade())
					{
						TempStr.MakeMsg(_R(IDS_COLGAL_ISHADE),
										(TCHAR *) *(ParentCol->FindLinkedParent()->GetName()));
					}
					else
					{
						TempStr.MakeMsg(_R(IDS_COLGAL_ITINT),
										(INT32) ((ParentCol->GetTintValue().MakeDouble() + 0.005)*100),
										(TCHAR *) *(ParentCol->FindLinkedParent()->GetName()));
					}
					Info += TempStr;
				}
				break;

			case COLOURTYPE_LINKED:
				{
					ERROR3IF(ParentCol->FindLinkedParent() == NULL,
							"A linked colour without a parent colour? Oh bums!");

					TempStr.MakeMsg(_R(IDS_COLGAL_ILINKED),
						(TCHAR *) *(ParentCol->FindLinkedParent()->GetName()));
					Info += TempStr;
				}
				break;

			default:
				break;
		}


		SGDisplayGroup *DocumentGroup = (SGDisplayGroup *) GetParent();
		ERROR3IF(DocumentGroup == NULL, "SGallery DisplayTree linkage corruption detected");

		Document *ScopeDoc = DocumentGroup->GetParentDocument();
		ERROR3IF(ScopeDoc == NULL, "SGallery group is not for a document! Unimplemented! Eek!");

		ColourList *ParentColList = ScopeDoc->GetIndexedColours();
		ERROR3IF(ParentColList == NULL, "A document with no colour list? Now I've seen it all");

		if (!ParentColList->IsColourInUseInDoc(ParentCol, TRUE))
		{
			// Add ' (not in use)' for any colour not currently used in the
			// document, or as a linked-colour parent
			TempStr.MakeMsg(_R(IDS_COLGAL_INOTUSED));
			Info += TempStr;
		}
	}

	*Result = Info;
}



/***********************************************************************************************

>	virtual BOOL SGDisplayColour::IsALibraryColour(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Inputs:		-
	Returns:	FALSE if this is a base-class SGDisplayColour (IndexedColour)
				TRUE  if this is a derived class SGDisplayLibColour (library DocColour)

	Purpose:	Proper virtual-function method for differentiating between IndexedColour
				and library DocColour gallery display items.

	Notes:		Always returns FALSE

***********************************************************************************************/

BOOL SGDisplayColour::IsALibraryColour(void) const
{
	return(FALSE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayColour::IsASpotColour(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96

	Inputs:		-
	Returns:	FALSE if this is a normal colour
				TRUE  if this is a spot colour

	Purpose:	Determine if the displayed colour is a normal or spot colour

***********************************************************************************************/

BOOL SGDisplayColour::IsASpotColour(void)
{
	if (TheColour.FindParentIndexedColour() != NULL)
	{
		if (TheColour.FindParentIndexedColour()->IsSpotOrTintOfSpot())
			return(TRUE);
	}

	return(FALSE);
}



/***********************************************************************************************

>	void SGDisplayColour::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95

	Inputs:		FormatInfo - The formatting info from which to calculate my position/size
				MiscInfo	- The MiscInfo, as usual

	Outputs:	member variable FormatRect - is returned filled in with the size/position of
				this colour item's display area. This is dependent upon the current display
				mode and format state

	Purpose:	Shared code for colour items to calculate where they will appear in the
				grand scheme of things

	Scope:		private (to sgcolour.cpp, for use of SGDisplayColour class only)

***********************************************************************************************/

void SGDisplayColour::CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo)
{
	const INT32 SGC_FullInfoWidth = SG_InfiniteWidth; //300000;

	INT32 XSize;
	INT32 YSize;

	// Note: The text is usually something like 9 point, so a minimum Y size to fit text
	// in is going to be around 12 point (12000 millipoints)

	switch (MiscInfo->DisplayMode)
	{
		case 1:	  					// 1 - Full Info
			XSize = GridLock(MiscInfo, SGC_FullInfoWidth);
			YSize = GridLock(MiscInfo, SG_DefaultSmallIcon);
			break;

		case 2:						// 2 - Icon only, no text at all
			XSize = YSize = GridLock(MiscInfo, SG_DefaultLargeIcon);
			break;

		default:					// 0 (Default) - Small icon with name
			XSize = GridLock(MiscInfo, SG_ColourNameWidth);
			YSize = GridLock(MiscInfo, SG_DefaultSmallIcon);
			break;
	}

	CalculateFormatRect(FormatInfo, MiscInfo, XSize, YSize);
}


/***********************************************************************************************

>	void DrawCircle(RenderRegion *Renderer, DocRect &BoundRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/96

	Inputs:		Renderer - The render region to render into
				BoundRect - The bounding rectangle to draw the circle into

	Purpose:	Draws a circle filling the width of the given rectangle,
				using the current render region attributes.

	Scope:		private

***********************************************************************************************/

void DrawCircle(RenderRegion *Renderer, DocRect &BoundRect)
{
	// WEBSTER - markn 23/1/97
	// No Circles required.
	//
#ifndef WEBSTER
	const INT32 CircleSize = BoundRect.Width() / 2;
	const INT32 CPDist = (const INT32) ( ((double)CircleSize) * 0.552 );

	Path Circle;
	Circle.Initialise(12, 12);
	Circle.FindStartOfPath();

	DocCoord Center(BoundRect.lo.x + BoundRect.Width()/2,
					BoundRect.lo.y + BoundRect.Height()/2);

	Circle.InsertMoveTo(DocCoord(Center.x+CircleSize, Center.y));
	Circle.InsertCurveTo(DocCoord(Center.x+CircleSize, Center.y+CPDist),
							DocCoord(Center.x+CPDist, Center.y+CircleSize),
							DocCoord(Center.x, Center.y+CircleSize));
	Circle.InsertCurveTo(DocCoord(Center.x-CPDist, Center.y+CircleSize),
							DocCoord(Center.x-CircleSize, Center.y+CPDist),
							DocCoord(Center.x-CircleSize, Center.y));
	Circle.InsertCurveTo(DocCoord(Center.x-CircleSize, Center.y-CPDist),
							DocCoord(Center.x-CPDist, Center.y-CircleSize),
							DocCoord(Center.x, Center.y-CircleSize));
	Circle.InsertCurveTo(DocCoord(Center.x+CPDist, Center.y-CircleSize),
							DocCoord(Center.x+CircleSize, Center.y-CPDist),
							DocCoord(Center.x+CircleSize, Center.y));

	Circle.IsFilled = TRUE;

	Renderer->DrawPath(&Circle);				// and draw it
#endif // WEBSTER
}



/***********************************************************************************************

>	virtual void SGDisplayColour::HandleRedraw(SGRedrawInfo *RedrawInfo,
												SGFormatInfo *FormatInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/1/95

	Inputs:		RedrawInfo	- The information on the kernel-rendered redraw area
				FormatInfo	- The formatting information structure

				member variable FormatRect should be set up (before calling this method)
				to be the rectangle in which to draw this item

	Purpose:	SGDisplayColour item redraw method - removed from the main HandleEvent
				method merely to make the code tidier.

	Scope:		private

***********************************************************************************************/

void SGDisplayColour::HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo)
{
	StartRendering(RedrawInfo, MiscInfo);

	DocRect MyRect(FormatRect);

	RenderRegion *Renderer = RedrawInfo->Renderer;

	INT32 OnePixel  = (INT32) DevicePixels(MiscInfo, 1);
	INT32 TwoPixels = (INT32) DevicePixels(MiscInfo, 2);

	DocRect IconRect(MyRect);		// Create a square icon at the left end of our rectangle
	IconRect.hi.x = IconRect.lo.x + IconRect.Height();
	MyRect.lo.x = IconRect.hi.x + TwoPixels;

	// Redraw the icon
	GridLockRect(MiscInfo, &IconRect);	

	Renderer->SetLineWidth(0);
	Renderer->SetLineColour(RedrawInfo->Transparent);

	// If selected, we have a 3-pixel-thick selection border
	// Otherwise (as is now the case) we have a single-pixel black border
	// We draw border "lines" using rectangle fills, else it screws pixel alignment
	if (!Flags.Selected)
		IconRect.Inflate(-TwoPixels);

	IndexedColour *IxCol = TheColour.FindParentIndexedColour();

	// Draw (selection border and) border
	// The patch is normally a square (process colour), but for spot colours
	// is drawn as a circle, in order to make the distinction more obvious
	//
	// WEBSTER - markn 23/1/97
	// No Circles required.
	//
	DocColour black(COLOUR_BLACK);
	Renderer->SetFillColour(black);
#ifndef WEBSTER
	if (IsASpotColour())
	{
		IconRect.Inflate(-OnePixel);
		DrawCircle(Renderer, IconRect);
	}
	else
#endif // WEBSTER
		Renderer->DrawRect(&IconRect);				// And draw it

	// Now deflate further, to get inside the selection & border...
	IconRect.Inflate((Flags.Selected)  ? -(TwoPixels+OnePixel) : -OnePixel);

	// Draw the colour patch (icon)
	// Save and restore the context while using TheColour to fill with. This is so that
	// the renderer no longer references the IndexedColour, so that GetFullInfoText will
	// put the text '(not in use)' on the end of colours which aren't used in the document!
	Renderer->SaveContext();
	Renderer->SetFillColour(TheColour);			// Fill with colour to display

	// The patch is normally a square (process colour), but for spot colours
	// is drawn as a circle, in order to make the distinction more obvious
	//
	// WEBSTER - markn 23/1/97
	// No Circles required.
	//
#ifndef WEBSTER
	if (IsASpotColour())
		DrawCircle(Renderer, IconRect);
	else
#endif // WEBSTER
		Renderer->DrawRect(&IconRect);				// And draw it
	Renderer->RestoreContext();

	// Redraw the name text, if there is room to the right of the icon
	if (MyRect.lo.x < MyRect.hi.x)
	{
		GridLockRect(MiscInfo, &MyRect);

//		IndexedColour *ParentCol = TheColour.FindParentIndexedColour();

		if (Flags.Selected)
		{
			// Fill the entire background with the 'selected' colour, so we don't
			// get gaps between bits of text or uneven rectangles in multiple selections
			Renderer->SetFillColour(RedrawInfo->SelBackground);
			Renderer->DrawRect(&MyRect);

			Renderer->SetFixedSystemTextColours(&RedrawInfo->SelForeground, &RedrawInfo->SelBackground);

			if (IxCol != NULL)	// Only do this for IndexedColours (i.e. not derived class Library colours)
			{
				// If this item represents the current line colour, then place a vertical line between the icon
				// and the text to indicate this fact.
				DocColour *LineCol;
				ColourManager::GetCurrentLineAndFillColours(&LineCol, NULL);

				if (LineCol != NULL && LineCol->FindParentIndexedColour() == TheColour.FindParentIndexedColour())
				{
					// This item is selected and is the current line colour. Put a mark in to indicate this
					DocColour black(COLOUR_BLACK);
					Renderer->SetLineColour(black);
					Renderer->SetLineWidth(TwoPixels);
					Renderer->DrawLine(MyRect.lo, DocCoord(MyRect.lo.x, MyRect.hi.y));
				}
			}
		}
		else
			Renderer->SetFixedSystemTextColours(&RedrawInfo->Foreground, &RedrawInfo->Background);

		MyRect.lo.x += SG_GapBeforeText;	// Leave a small gap before text begins

		String_256 Name;
		GetNameText(&Name);					// Get the name string

		if (MiscInfo->DisplayMode == 1)		// If in Full Info mode...
		{
			DocRect TextRect(MyRect);

			// Reserve space for the name (clip it if it won't fit)
			if (TextRect.hi.x - TextRect.lo.x > SG_ColourNameWidth)
				TextRect.hi.x = TextRect.lo.x + SG_ColourNameWidth;

			// Draw the name text next to the icon
			Renderer->DrawFixedSystemText(&Name, TextRect);
			
			// Move to the right, to fill the remaining space with information
			// (Leave a 6-pixel gap in case the name didn't all fit in the available space)
			TextRect.lo.x = TextRect.hi.x + DevicePixels(MiscInfo, 6);
			TextRect.hi.x = MyRect.hi.x;
			if (TextRect.lo.x < TextRect.hi.x)		// If any room left for info...
			{
				// Get the full info description, and render it
				GetFullInfoText(&Name);
				Renderer->DrawFixedSystemText(&Name, TextRect);
			}
		}
		else
		{
			// Not full-info mode, so we only render the colour name next to the patch
			Renderer->DrawFixedSystemText(&Name, MyRect);
		}
	}

	StopRendering(RedrawInfo, MiscInfo);
}




/***********************************************************************************************

>	virtual void SGDisplayColour::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95

	Inputs:		Mouse - The mouse info passed to the original click handler
				MiscInfo - The misc info passed to the original click handler

	Purpose:	Handles a mouse click event. This is a callback function - drags of
				colours from galleries will call this function back if the drag turns
				out to just be a click.

	SeeAlso:	SGDisplayColour::HandleEvent; GalleryColourDragInfo::OnClick

***********************************************************************************************/

void SGDisplayColour::DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo)
{
	// Just get default selection action to be applied for this click.
	// The TRUE indicates that this is a drag-click, and we previously called 
	// the DefaultPreDragHandler - we don't want it to do those same actions twice!
	DefaultClickHandler(Mouse, MiscInfo, TRUE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayColour::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGDisplayColour::HandleEvent(SGEventType EventType, void *EventInfo,
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

				if (IMustRedraw(RedrawInfo))
					HandleRedraw(RedrawInfo, MiscInfo);
			}
			break;		// exit and return FALSE to pass the redraw event on


		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					// If the colour is in the selected document, then it is safe to
					// do a colour drag - for now, it will only allow drags for the
					// selected doc.
					// Otherwise, the normal click action takes place.
					// If the drag fails (turns into a click) then the normal click action
					// takes place, passed on by the GalleryColourDragInfo::OnClick handler
					SGDisplayGroup *Parent = (SGDisplayGroup *) GetParent();

					if (Mouse->DoubleClick) // || Parent->GetParentDocument() != Document::GetSelected())
					{
						// Use the default click handler.
						// However, because we assign a very special meaning to adjust-double-click (apply
						// line colour) we don't want the default action (apply & close gallery) to take
						// effect, so we pass in FALSE as the last parameter.
						DefaultClickHandler(Mouse, MiscInfo, FALSE, FALSE);
					}
					else
					{
						DefaultPreDragHandler(Mouse, MiscInfo);

						// Start a drag, but only if there are selected documents around
						if (Document::GetSelected() != NULL)
						{
							GalleryColourDragInfo *DragCol;
							DragCol = new GalleryColourDragInfo(Parent->GetParentDocument(),
																this, Mouse, MiscInfo,
																Mouse->MenuClick);

							if (DragCol != NULL)
								DragManagerOp::StartDrag(DragCol);
						}
					}

					return(TRUE);		// Claim this event - nobody else can own this click
				}
			}
			break;


		default:
			return(SGDisplayItem::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return(FALSE);
}



/***********************************************************************************************

>	virtual void SGDisplayColour::MoveAfter(SGDisplayNode *NodeToMove)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item after *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayColour::InsertAfter; SGDisplayColour::AddItem

***********************************************************************************************/

void SGDisplayColour::MoveAfter(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");

	if (NodeToMove == this)
		return;

	if (IsALibraryColour())
		LibraryHasBeenSorted = TRUE;

	DocColour *DocColToMove = ((SGDisplayColour *) NodeToMove)->GetDisplayedColour();
	ERROR3IF(DocColToMove == NULL, "NULL displayed colour?!");

	IndexedColour *ColToMove = DocColToMove->FindParentIndexedColour();

	IndexedColour *TargetColour = TheColour.FindParentIndexedColour();
	if (ColToMove == NULL || TargetColour == NULL)
	{
		// If either colour is a library colour, then no need to move colours in the
		// document colour list, so just move the display nodes appropriately and return.
		SGDisplayNode::MoveAfter(NodeToMove);
		return;
	}

	Document *ScopeDoc = ((SGDisplayGroup *) GetParent())->GetParentDocument();
	ERROR3IF(ScopeDoc == NULL, "No parent document?!");

	if ( ((SGDisplayGroup *) ( ((SGDisplayColour *)NodeToMove)->GetParent() ) )->GetParentDocument() !=
			ScopeDoc)
	{
		ERROR2RAW("Attempt to MOVE a colour between documents!");
		InformError();
		return;
	}

	ColourList *ColList = ScopeDoc->GetIndexedColours();
	ERROR3IF(ColList == NULL, "A document with no colour list?!");
	
	ColList->RemoveItem(ColToMove);
	ColList->InsertAfter(TargetColour, ColToMove);

	SGDisplayNode::MoveAfter(NodeToMove);

// We do not need to do the following - the parent gallery will do it when it is called
// with SuperGallery::AllItemsCopied. This allows us to be called many times in a row
// without generating more than one broadcast.

//	// Inform the world, and lock out changes in the colour gallery
//	ColourSGallery *ParentGallery = (ColourSGallery *)GetParentGallery();
//	BOOL OldSentState = ParentGallery->ISentTheMessage;
//	ParentGallery->ISentTheMessage = TRUE;
//	
//	ColourManager::ColourListHasChanged(ColList);
//	
//	ParentGallery->ISentTheMessage = OldSentState;
}



/***********************************************************************************************

>	virtual void SGDisplayColour::MoveBefore(SGDisplayNode *NodeToMove)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		NodeToMove - the node to move

	Purpose:	MOVES the given node (to a different position in the DisplayTree) as the
				previous (left) sibling of this node. If the node is not linked into
				a tree, it is effectively just inserted.

	Notes:		This base class method simply delinks the item and relinks it elsewhere
				in the display tree. However, derived classes will override this method
				so that moving display items can have a further effect of also rearranging
				the displayed "real" items. Before/After moving the real item, the
				derived class can then call this baseclass method to complete the action.
		
				Take care when moving items between groups (e.g. if an item is "moved"
				from one docuemnt to another, it could be a bad thing, so be very
				careful in derived classes to take appropriate action)

				Any attempt to move an item before *itself* is queitly ignored

	Errors:		ERROR3 and quiet exit if NodeToMove == NULL

	SeeAlso:	SuperGallery; SGDisplayColour::InsertBefore; SGDisplayColour::AddItem

***********************************************************************************************/

void SGDisplayColour::MoveBefore(SGDisplayNode *NodeToMove)
{
	ERROR3IF(NodeToMove == NULL, "Illegal NULL param");

	if (NodeToMove == this)
		return;

	if (IsALibraryColour())
		LibraryHasBeenSorted = TRUE;

	DocColour *DocColToMove = ((SGDisplayColour *) NodeToMove)->GetDisplayedColour();
	ERROR3IF(DocColToMove == NULL, "NULL displayed colour?!");
	IndexedColour *ColToMove = DocColToMove->FindParentIndexedColour();

	IndexedColour *TargetColour = TheColour.FindParentIndexedColour();
	if (ColToMove == NULL || TargetColour == NULL)
	{
		// If either colour is a library colour, then no need to move colours in the
		// document colour list, so just move the display nodes appropriately and return.
		SGDisplayNode::MoveBefore(NodeToMove);
		return;
	}

	Document *ScopeDoc = ((SGDisplayGroup *) GetParent())->GetParentDocument();
	ERROR3IF(ScopeDoc == NULL, "No parent document?!");

	if ( ((SGDisplayGroup *) ( ((SGDisplayColour *)NodeToMove)->GetParent() ) )->GetParentDocument() !=
			ScopeDoc)
	{
		ERROR2RAW("Attempt to MOVE a colour between documents!");
		InformError();
		return;
	}

	ColourList *ColList = ScopeDoc->GetIndexedColours();
	ERROR3IF(ColList == NULL, "A document with no colour list?!");
	
	ColList->RemoveItem(ColToMove);
	ColList->InsertBefore(TargetColour, ColToMove);

	SGDisplayNode::MoveBefore(NodeToMove);

// We do not need to do the following - the parent gallery will do it when it is called
// with SuperGallery::AllItemsCopied. This allows us to be called many times in a row
// without generating more than one broadcast.

//	// Inform the world, and lock out changes in the colour gallery
//	ColourSGallery *ParentGallery = (ColourSGallery *)GetParentGallery();
//	BOOL OldSentState = ParentGallery->ISentTheMessage;
//	ParentGallery->ISentTheMessage = TRUE;
//	
//	ColourManager::ColourListHasChanged(ColList);
//	
//	ParentGallery->ISentTheMessage = OldSentState;
}



/********************************************************************************************

>	virtual BOOL SGDisplayColour::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
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

BOOL SGDisplayColour::GetBubbleHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	return(FALSE);
}


	
/********************************************************************************************

>	virtual BOOL SGDisplayColour::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		MousePos - The current mouse position. This will generally be expected
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

BOOL SGDisplayColour::GetStatusLineHelp(DocCoord *MousePos, String_256 *Result)
{
	ERROR3IF(MousePos == NULL || Result == NULL, "Invalid NULL params");

	*Result = TEXT("");			// Empty the string, just to be safe
	GetNameText(Result);		// Start the string with the colour name
	*Result += TEXT("; ");

	String_64 Temp;
	if (IsSelected())
		Temp = _R(IDS_SGCOLOUR_CTRL_CLICK);			// TEXT("Ctrl-click to deselect;");
	else
		Temp = _R(IDS_SGCOLOUR_CLICK);				// TEXT("Click to select;");

	*Result += Temp;
	*Result += String_64(_R(IDS_SGCOLOUR_DOUBLE_CLICK)); // TEXT(" Double-click to apply this colour; Or drag and drop");

	return(TRUE);
}













/***********************************************************************************************

>	SGDisplayLibColour::SGDisplayLibColour()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Purpose:	SGDisplayLibColour constructor
				DON'T call this constructor. It ERROR3's. Call the other constructor

***********************************************************************************************/

SGDisplayLibColour::SGDisplayLibColour()
{
	ERROR3("Illegal call on default SGDisplayLibColour constructor - call the other one!");
	Flags.Prefix = 0;
	Flags.NewlineAfter = FALSE;
	Flags.SpotColour = FALSE;
}



/***********************************************************************************************

>	SGDisplayLibColour::SGDisplayLibColour(DocColour *ColourToDisplay,
										StringBase *Name, PalettePrefix Prefix,
										BOOL NewlineAfter = FALSE, IsSpotColour = FALSE);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Inputs:		ColourToDisplay - The definition of the colour this item will display
				Name	- The name (suffix) of this colour
				Prefix	- ID of a prefix to be prepended to the name
				NewlineAfter - TRUE if this item is the last in a "run" of colours
						and should cause a new-line in the displayed list to produce a
						tidier on screen display. This is only used in "icons only" mode.
				IsSpotColour - FALSE for a normal colour, TRUE if this is a spot colour

	Purpose:	SGDisplayLibColour constructor

***********************************************************************************************/

SGDisplayLibColour::SGDisplayLibColour(DocColour *ColourToDisplay,
										StringBase *Name, PalettePrefix Prefix,
										BOOL NewlineAfter, BOOL IsSpotColour)
{
	ERROR3IF((INT32)Prefix < 0 || (INT32)Prefix > 0x3f, "Out of range prefix value");
	Flags.Prefix = ((UINT32) Prefix) & 0x3f;
	Flags.NewlineAfter	= NewlineAfter;
	Flags.SpotColour	= IsSpotColour;

	ERROR3IF(ColourToDisplay == NULL || Name == NULL, "Illegal NULL params");
	TheColour = *ColourToDisplay;

	// Copy the colour name. Note that we use a StringBase object an explicitly
	// allocate exactly the right number of characters, in order to minimise
	// memory usage (libraries hold thousands of colours, so even one byte less
	// saves kilobytes of storage over an entire library)
//	INT32 Length = Name->Length();
	ColourName.Alloc(Name->Length());
	ColourName = *Name;
}



/********************************************************************************************

>	virtual void SGDisplayLibColour::GetNameText(String_256 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Outputs:	On exit, the string pointed at by Result will contain either a blank
				string, or the name text associated with this item (if any)

	Purpose:	To determine a name string for this node. Generally, this is used for
				a simple mechanism which searches for display items whose names match
				given search parameters in some way. It is also used in libraries to
				provide default redraw methods.

	SeeAlso:	SGDisplayLibColour::GetFullInfoText

********************************************************************************************/

void SGDisplayLibColour::GetNameText(String_256 *Result)
{
	ERROR3IF(Result == NULL, "Illegal NULL param");

	switch((PalettePrefix) Flags.Prefix)
	{
		case PalettePrefix_Pantone:
			*Result = TEXT("PANTONE ");		// NOTE - all these string constants are
			*Result += ColourName;			// the same in all languages and changing the
//			*Result += TEXT(" PC");			// exact string used here could affect the
			return;							// Pantone licence

		case PalettePrefix_PantoneSpot:
			*Result = TEXT("PANTONE ");		// NOTE - all these string constants are
			*Result += ColourName;			// the same in all languages and changing the
			*Result += TEXT(" PC");			// exact string used here could affect the
			return;							// Pantone licence

		case PalettePrefix_Focoltone:
			*Result = TEXT("Focoltone ");
			break;

		case PalettePrefix_Trumatch:
			*Result = TEXT("Trumatch ");
			break;

		case PalettePrefix_RGB:
			*Result = TEXT("RGB ");
			break;

		default:
			*Result = TEXT("");
			break;
	}

	*Result += ColourName;
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLibColour::HandleEvent(SGEventType EventType, void *EventInfo,
											 SGMiscInfo *MiscInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Inputs:		EventType - An enumerated value describing what type of event is to be processed

				EventInfo - A structure describing the event (may be NULL). The exact thing
							pointed at by this pointer depends upon the event type:

							MonoOn
							Event				Thing EventInfo points at
							SGEVENT_FORMAT		(SGFormatInfo *)
							SGEVENT_REDRAW		(SGRedrawInfo *)
							SGEVENT_MOUSECLICK	(SGMouseInfo *)
							MonoOff
				Use the provided SGDisplayNode::Get[Format]Info() inlines to retrieve this
				information - they provide useful error/type checking, and hide the cast

				MiscInfo - always provided. Contains a few useful bits of info that may be
				needed for all event types.

	Outputs:	FormatInfo is updated as appropriate

	Returns:	TRUE if the event was handled successfully
				FALSE if it was not

	Purpose:	Handles a SuperGallery DisplayTree event

	Notes:		This overrides the pure virtual SGDisplayNode::HandleEvent method

				A node need not handle a specific event - if it does not handle it, it
				should return FALSE.

				Redraw and Formatting handlers should never return TRUE, as this will
				prevent the event from continuing through the tree.

				Non-leaf-nodes must call SGDisplayNode::GiveEventToMyChildren in order
				to pass the event dow the tree. THIS node is a leaf-node, so it doesn't.

	SeeAlso:	SGDisplayNode::HandleEvent

***********************************************************************************************/

BOOL SGDisplayLibColour::HandleEvent(SGEventType EventType, void *EventInfo,
								  SGMiscInfo *MiscInfo)
{
	switch (EventType)
	{
		case SGEVENT_FORMAT:
			{
				SGFormatInfo *FormatInfo = GetFormatInfo(EventType, EventInfo);
				CalculateMyRect(FormatInfo, MiscInfo);		// Cache our FormatRect for later use

				// If in display mode 2, and this is the end of a pantone "page", then
				// we special case it to do a "newline" at the end of the line, to
				// keep pages together nicely
				if (!LibraryHasBeenSorted && MiscInfo->DisplayMode == 2)
				{
					if (Flags.NewlineAfter)
						NewLine(FormatInfo, MiscInfo);
				}
			}
			break;

		case SGEVENT_MOUSECLICK:
			{
				SGMouseInfo *Mouse = GetMouseInfo(EventType, EventInfo);

				if (FormatRect.ContainsCoord(Mouse->Position))
				{
					// If the colour is in the selected document, then it is safe to
					// do a colour drag - for now, it will only allow drags for the
					// selected doc.
					// Otherwise, the normal click action takes place.
					// If the drag fails (turns into a click) then the normal click action
					// takes place, passed on by the GalleryColourDragInfo::OnClick handler
//					SGDisplayGroup *Parent = (SGDisplayGroup *) GetParent();

					if (Mouse->DoubleClick) // || Parent->GetParentDocument() != Document::GetSelected())
					{
						// Use the default click handler.
						// However, because we assign a very special meaning to adjust-double-click (apply
						// line colour) we don't want the default action (apply & close gallery) to take
						// effect, so we pass in FALSE as the last parameter.
						DefaultClickHandler(Mouse, MiscInfo, FALSE, FALSE);
					}
					else
					{
						DefaultPreDragHandler(Mouse, MiscInfo);

						// Don't start drags when there are no documents about
						if (Document::GetSelected() != NULL)
						{
							GalleryColourDragInfo *DragCol;
							DragCol = new GalleryColourDragInfo(this, Mouse, MiscInfo, Mouse->MenuClick);

							if (DragCol != NULL)
								DragManagerOp::StartDrag(DragCol);
						}
					}

					return(TRUE);		// Claim this event - nobody else can own this click
				}
			}
			break;


		default:
			return(SGDisplayColour::HandleEvent(EventType, EventInfo, MiscInfo));
	}

	// Default return value: We do not claim this event, so it will be passed on to others
	return(FALSE);
}



/***********************************************************************************************

>	virtual INT32 SGDisplayLibColour::CompareTo(SGDisplayNode *Other, INT32 SortKey)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Inputs:		Other - the Colour to compare this Colour to
				SortKey - An integer identifying how to compare the items
					0 = No sorting (always returns 0)
					1 = Sort-by-name
					2 = Sort-by-hue
					3 = Sort-by-intensity
					4 = Sort-by-model

	Returns:	negative (I am lesser), 0 (we are equal), or positive (I am greater)

	Purpose:	Compares this Colour to the 'other' Colour, to determine their relative positions
				in the display tree. Returns a value which usually indicates that the other
				Colour should be inserted before (-1, or 0) or after (+1) this item.

	Notes:		Overrides the base Colour class compare for colour names, but otherwise
				relegates control back to the base class function.

	SeeAlso:	SGDisplayNode::CompareTo; SGDisplayColour::CompareTo

***********************************************************************************************/

INT32 SGDisplayLibColour::CompareTo(SGDisplayNode *Other, INT32 SortKey)
{
	ERROR3IF(Other == NULL, "Illegal NULL parameter");

	ERROR3IF(!Other->IsKindOf(CC_RUNTIME_CLASS(SGDisplayLibColour)),
				"SGDisplayLibColour::CompareTo - The other item's not a colour!");

	switch (SortKey)
	{
		case SGSORTKEY_BYNAME:
			{
				// We override the base class version to use the specialised name strings that
				// Library colours hold in them
				SGDisplayLibColour *pOther = (SGDisplayLibColour *)Other;

				return(ColourName.CompareTo( (TCHAR *) pOther->ColourName) );
			}
			break;
	}

	// Apart from sort-by-name, all sorting can be achieved by the base class code
	return(SGDisplayColour::CompareTo(Other, SortKey));
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLibColour::IsALibraryColour(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Inputs:		-
	Returns:	FALSE if this is a base-class SGDisplayColour (IndexedColour)
				TRUE  if this is a derived class SGDisplayLibColour (library DocColour)

	Purpose:	Proper virtual-function method for differentiating between IndexedColour
				and library DocColour gallery display items.

	Notes:		Always returns TRUE

***********************************************************************************************/

BOOL SGDisplayLibColour::IsALibraryColour(void) const
{
	return(TRUE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLibColour::IsASpotColour(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96

	Returns:	FALSE if this is a normal colour
				TRUE  if this is a spot colour

	Purpose:	Determine if the display colour is a normal or spot colour

***********************************************************************************************/

BOOL SGDisplayLibColour::IsASpotColour(void)
{
	// If spot colours are all forced to be process, this colour is a process colour!
	if (IndexedColour::AreSpotsForcedToProcess())
		return(FALSE);

	return(Flags.SpotColour);
}




/***********************************************************************************************

>	SGDisplayLibColGroup::SGDisplayLibColGroup(SuperGallery *ParentGal,
											   Document *ParentDoc = NULL, Library *ParentLib = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/97
	Inputs:		ParentGal - The parent gallery of this group
				ParentDoc - The parent document
				ParentLib - The parent library
	Purpose:	Constructor

***********************************************************************************************/

SGDisplayColourGroup::SGDisplayColourGroup(SuperGallery *ParentGal,
										   Document *ParentDoc, Library *ParentLib)
					 : SGDisplayGroup(ParentGal, ParentDoc, ParentLib)
{
}

/***********************************************************************************************

>	virtual BOOL SGDisplayColourGroup::DisplayInColourLine()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/97
	Returns:	The current state of the ShowOnColourLine flag.
	Purpose:	To find out the current state of the showing on Colour Line flag.

***********************************************************************************************/

BOOL SGDisplayColourGroup::DisplayInColourLine()
{
	return ColourSGallery::ShowDocumentColours;
}

/***********************************************************************************************

>	virtual BOOL SGDisplayColourGroup::SetDisplayInColourLine(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/97
	Inputs:		New state for the ShowOnColourLine flag.
	Returns:	The old state of the ShowOnColourLine flag.
	Purpose:	To set a new current state of the ShowOnColourLine flag.

***********************************************************************************************/

BOOL SGDisplayColourGroup::SetDisplayInColourLine(BOOL NewState)
{
	BOOL OldState = ColourSGallery::ShowDocumentColours;
	ColourSGallery::ShowDocumentColours = NewState;
	return OldState;
}

/***********************************************************************************************

>	virtual BOOL SGDisplayColourGroup::ToggleDisplayInColourLine()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/97
	Returns:	The old state of the ShowOnColourLine flag.
	Purpose:	To toggle the current state of the ShowOnColourLine flag.

***********************************************************************************************/

BOOL SGDisplayColourGroup::ToggleDisplayInColourLine()
{
	BOOL OldState = ColourSGallery::ShowDocumentColours;
	if (ColourSGallery::ShowDocumentColours)
		ColourSGallery::ShowDocumentColours = FALSE;
	else
		ColourSGallery::ShowDocumentColours = TRUE;

	return OldState;
}




/***********************************************************************************************

>	SGDisplayLibColGroup::SGDisplayLibColGroup(SuperGallery *ParentGal, PathName *LibraryFile)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Inputs:		ParentGal - The parent gallery of this group
				LibraryFile - The library palette file to be displayed by this group

	Purpose:	Constructor

***********************************************************************************************/

SGDisplayLibColGroup::SGDisplayLibColGroup(SuperGallery *ParentGal, PathName *LibraryFile)
					 : SGDisplayColourGroup(ParentGal)
{
	ERROR3IF(LibraryFile == NULL, "Illegal NULL param");

	// Copy the group name into the base class member variable
	String_256 TheFile = LibraryFile->GetFileName(FALSE);

	String_256 UpperFile(TheFile);		// Force the name to uppercase to compare, to be safe
	UpperFile.toUpper();

	if (UpperFile == String_256(TEXT("PANTSPOT")))
		TitleText.MakeMsg(_R(IDS_PANTONESPOTLIB));						// "PANTONE Spot Colours"
	else if (UpperFile == String_256(TEXT("PPROCESSU")))
		TitleText.MakeMsg(_R(IDS_PANTONEPROCESSU));						// "PANTONE Color Name-Uncoated"
	else if (UpperFile == String_256(TEXT("PPROCESSC")))
		TitleText.MakeMsg(_R(IDS_PANTONEPROCESSC));						// "PANTONE Color Name-Coated"
	else if (UpperFile == String_256(TEXT("WEB")))
		TitleText.MakeMsg(_R(IDS_WEBLIBRARY));							// "Web Colours"
	else
		TitleText.MakeMsg(_R(IDS_COLOURLIBRARY), (TCHAR *)TheFile);		// "Colour library Fabby.pal"

	// Copy the filename into our member variable
	Filename = *LibraryFile;

	Flags.Folded		= TRUE;		// Always default to being folded
	Flags.Virtualised	= TRUE;		// And virtualised (we demand-load when we're unfolded)

	Flags.CanSelect		= TRUE;		// The group is selectable (why not?)

	Flags.ReadOnly		= TRUE;		// The group is read-only (won't allow items to be dropped into it)

	// Illegal default value to determine if value is present
	m_DisplayInColourLine = -1;

	// read a preference value for that named gallery
	/*BOOL ReadOk =*/ Camelot.GetPrefDirect(TEXT("ColourLine"), UpperFile, &m_DisplayInColourLine);
	if (m_DisplayInColourLine == -1)
	{
		// If the value wasn't present then default to TRUE for "WEB" and FALSE for others
		m_DisplayInColourLine = (UpperFile == String_256(TEXT("WEB")));
	}
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLibColGroup::CanVirtualise(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Returns:	TRUE

	Purpose:	To deterimine if this group can be virtualised to disk

				Colour libraries can be virtualised

***********************************************************************************************/

BOOL SGDisplayLibColGroup::CanVirtualise(void)
{
	return(TRUE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLibColGroup::CanVirtualise(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Returns:	TRUE for success

	Purpose:	Virtualises this library out to disk (or rather, to be more accurate,
				chucks this library out of memory. Colour libraries are totally read-only,
				although while loaded we let the user reorganise them for convenience)

***********************************************************************************************/

BOOL SGDisplayLibColGroup::Virtualise(void)
{
	if(IsVirtualised())
		return TRUE;

	// Simply destroy the entire subtree from (but not including- FALSE) this node down
	DestroySubtree(FALSE);
	SetVirtualisedState(TRUE);

	return(TRUE);
}



/***********************************************************************************************

>	virtual BOOL SGDisplayLibColGroup::DeVirtualise(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Returns:	TRUE for success

	Purpose:	De-Virtualises this library - loads it back in from disk

***********************************************************************************************/

BOOL SGDisplayLibColGroup::DeVirtualise(void)
{
	if(!IsVirtualised())
		return TRUE;

	// Try and find the generic palette filter to load whatever type of palette file we're using
	Filter *pFilter = Filter::GetFirst();
	while (pFilter != NULL && pFilter->FilterID != FILTERID_PALETTE)
		pFilter = Filter::GetNext(pFilter);

	CCDiskFile Infile;
	if (pFilter == NULL || !Infile.open(Filename, ios::in | ios::binary))
		return(FALSE);

	((ColourSGallery *)GetParentGallery())->SetCurrentImportGroup(this);

	if (!pFilter->DoImport(NULL, &Infile, NULL))
		InformError();

	((ColourSGallery *)GetParentGallery())->SetCurrentImportGroup(NULL);

	if (Infile.isOpen())
		Infile.close();

	SetVirtualisedState(FALSE);
	return(TRUE);
}



/***********************************************************************************************

>	virtual void SGDisplayLibColGroup::ReadGroupTitle(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Purpose:	Reads the title for this group.
				This method overrides the base class functionality merely to stop
				it sitting on the TitleText member variable, in which our title is
				always cached.

***********************************************************************************************/

void SGDisplayLibColGroup::ReadGroupTitle(void)
{
	// Simply override the function so that it leaves the TitleText member variable alone!
}



/***********************************************************************************************

>	INT32 SGDisplayLibColGroup::CountChildren()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Returns:	The number of children
	Purpose:	Returns the number of children of this group node

***********************************************************************************************/

INT32 SGDisplayLibColGroup::CountChildren()
{
	INT32 total = 0;

	SGDisplayNode *pChild = GetChild();
	while (pChild)
	{
		total ++;
		pChild = pChild->GetNext();
	}

	return total;
}

/***********************************************************************************************

>	DocColour *SGDisplayLibColGroup::GetItemColour(UINT32 Index, SGDisplayLibColour **ppLibColour = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		The index of the item required
				An optional SGDisplayLibColour pointer, defaults to null.
	Returns:	The DocColour associated with the specified item or NULL 
	Purpose:	Returns the DocColour associated with the specified item in this group or NULL.
				If the SGDisplayLibColour pointer is provided it is updated with the found item
				THe called can then interogate the item if so desired.

***********************************************************************************************/

DocColour *SGDisplayLibColGroup::GetItemColour(UINT32 Index, SGDisplayLibColour **ppLibColour)
{
	UINT32 item = 0;

	SGDisplayLibColour *pChild = (SGDisplayLibColour *)GetChild();
	while (pChild)
	{
		if (item == Index)
		{
			// If the caller desired it, return the lib colour item
			if (ppLibColour)
				*ppLibColour = pChild;
			return pChild->GetDisplayedColour();
		}
		
		item ++;
		pChild = (SGDisplayLibColour *)pChild->GetNext();
	}

	if (ppLibColour)
		*ppLibColour = NULL;

	// we found nothing
	return NULL;
}

/***********************************************************************************************

>	virtual BOOL SGDisplayLibColGroup::SetDisplayInColourLine(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/97
	Inputs:		New state for the ShowOnColourLine flag.
	Returns:	The old state of the ShowOnColourLine flag.
	Purpose:	To set a new current state of the ShowOnColourLine flag.

***********************************************************************************************/

BOOL SGDisplayLibColGroup::SetDisplayInColourLine(BOOL NewState)
{
	BOOL OldState = m_DisplayInColourLine;
	m_DisplayInColourLine = NewState;
	return OldState;
}

/***********************************************************************************************

>	virtual BOOL SGDisplayLibColGroup::ToggleDisplayInColourLine()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/97
	Returns:	The old state of the ShowOnColourLine flag.
	Purpose:	To toggle the current state of the ShowOnColourLine flag.

***********************************************************************************************/

BOOL SGDisplayLibColGroup::ToggleDisplayInColourLine()
{
	BOOL OldState = m_DisplayInColourLine;
	if (m_DisplayInColourLine)
		m_DisplayInColourLine = FALSE;
	else
		m_DisplayInColourLine = TRUE;

	// If in Camelot mode then save the new state in the preferences
	// as this is the only chance that we will get
	String_256 TheFile = Filename.GetFileName(FALSE);
	TheFile.toUpper();
	// read a preference value for that named gallery
	/*BOOL SetOk =*/ Camelot.SetPrefDirect(TEXT("ColourLine"), TheFile, &m_DisplayInColourLine);

	return OldState;
}












/********************************************************************************************

>	ColourSGallery::ColourSGallery()
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Purpose:	ColourSGallery default constructor

********************************************************************************************/

ColourSGallery::ColourSGallery()
{
	DlgResID = _R(IDD_COLOURSGALLERY);

	ISentTheMessage = FALSE;
	CurrentColComp  = NULL;
	CurrentTarget	= NULL;

	// Set the display mode up from the default setting
	DisplayMode = DefaultDisplayMode;
	if (DisplayMode < 0 || DisplayMode > 2)
		DisplayMode = 0;

	CurrentImportGroup = NULL;

	// WEBSTER - markn 9/12/96 - Martin 16/07/97 same as Camelot now
	// Default gallery size
	//CSize Size(256, 256);
	//SetGallerySize(Size);

	// Remember a pointer to the global instance of this gallery.
	ERROR3IF(m_pTheGallery != NULL, "Gallery already exists in ColourSGallery::ColourSGallery?");
	m_pTheGallery = this;
}



/********************************************************************************************

>	ColourSGallery::~ColourSGallery()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94
	Purpose:	ColourSGallery destructor.

********************************************************************************************/

ColourSGallery::~ColourSGallery()
{
	ERROR3IF(CurrentColComp != NULL, "Destructed colour gallery is still doing a component copy?!");

	// Remember the display mode as the default setting for next time we're created (preference)
	DefaultDisplayMode = DisplayMode;

	// Make sure nothing is seriously screwed-up.
	ERROR3IF(m_pTheGallery == NULL, "No gallery in ColourSGallery::~ColourSGallery?");
	m_pTheGallery = NULL;
}



/********************************************************************************************

>	static BOOL ColourSGallery::Init(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/95

	Purpose:	ColourSGallery initialisation - declares preferences etc

********************************************************************************************/

BOOL ColourSGallery::Init(void)
{
	// Declare preferences
	GetApplication()->DeclareSection(TEXT("Displays"), 8);
	GetApplication()->DeclarePref(TEXT("Displays"), TEXT("AutoScrollColours"), &AutoScrollSelection);
	GetApplication()->DeclarePref(TEXT("Displays"), TEXT("ColourDisplayMode"), &DefaultDisplayMode);

	GetApplication()->DeclareSection(TEXT("ColourLine"), 4);
	GetApplication()->DeclarePref(TEXT("ColourLine"), TEXT("ShowDocumentColours"), &ShowDocumentColours);

	GetApplication()->DeclareSection(TEXT("Palettes"), 2);
	GetApplication()->DeclarePref(TEXT("Palettes"), TEXT("Path"), &PalettePath);

	OpBackground::Init();

	return(TRUE);
}

/********************************************************************************************

>   static ColourSGallery* ColourSGallery::GetInstance()

	Author:     Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/3/97
	Inputs:     -
	Outputs:    -
	Returns:    A pointer to the program's colour gallery if it exists, or NULL.
	Purpose:    Public access function to the colour gallery.
	Errors:     -
	SeeAlso:    -

********************************************************************************************/

ColourSGallery* ColourSGallery::GetInstance()
{
	return m_pTheGallery;
}


/********************************************************************************************

>	void ColourSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayColourGroup *ExistingGroup)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94

	Inputs:		ParentDoc - The document to create a display subtree for
				ExistingGroup - NULL (creates a new group for this document), or
				a pointer to the existing group-node for this document (in which case
				it clears all displayitems from the group and rebuilds it in place - this
				stops the display group moving around the tree at random!)

	Purpose:	Internal call. This takes the colour list of the given document and 
				creates a DisplayTree subtree from it. This subtree is then added to
				the DisplayTree. Note that this does not force a redraw of the list - 
				after making this call, you should also call ForceRedrawOfList

	Notes:		Passing in a NULL parent document pointer results in an ERROR3 -
				the function returns without doing anything in retail builds

				It now scans the existinggroup (if any), and takes no action if it
				exactly matches the parent colour list.

	SeeAlso:	SuperGallery::ForceRedrawOfList

********************************************************************************************/

void ColourSGallery::CreateNewSubtree(Document *ParentDoc, SGDisplayColourGroup *ExistingGroup)
{
	ERROR3IF(ParentDoc == NULL, "ColourSGallery::CreateNewSubtree - illegal NULL parameter");
	if (ParentDoc == NULL || DisplayTree == NULL)
		return;

	// Don't add subtrees for clipboards!
	if (ParentDoc->IsAClipboard() || ParentDoc->IsAHiddenDoc())
		return;

	SGDisplayColourGroup  *DisplayDocument = NULL;
	SGDisplayColour *DisplayColour	 = NULL;

	if (ExistingGroup != NULL)
	{
		ERROR3IF(ExistingGroup->GetParentDocument() != ParentDoc, 
				 "This group is not for that document! What's going down, dude?");
		DisplayDocument = ExistingGroup;							// Use existing group

		// Scan the ExistingGroup, to see if we really need to do anything
		BOOL DisplayCorrect = TRUE;
		ColourList *ColList = ParentDoc->GetIndexedColours();
		IndexedColour *TheCol = ColList->GetUndeletedHead();
		DisplayColour = (SGDisplayColour *) DisplayDocument->GetChild();

		while (TheCol != NULL && DisplayColour != NULL && DisplayCorrect)
		{
			if (TheCol != DisplayColour->GetDisplayedColour()->FindParentIndexedColour())
				DisplayCorrect = FALSE;

			TheCol = ColList->GetUndeletedNext(TheCol);
			DisplayColour = (SGDisplayColour *) DisplayColour->GetNext();
		}

		if (DisplayCorrect && TheCol == NULL && DisplayColour == NULL)
		{
			// The display is exactly right- we don't need to do anything
			return;
		}

		DisplayDocument->DestroySubtree(FALSE);		// Wipe any existing colour display items
	}
	else
	{
		DisplayDocument = new SGDisplayColourGroup(this, ParentDoc, NULL);// Create new Group

		if (DisplayDocument == NULL)								// Total failure - abort
		{
			InformError();
			return;
		}

		DisplayDocument->Flags.CanSelect = TRUE;					// Group is selectable

		// Make sure that any library groups stay at the bottom of the gallery
		SGDisplayNode *InsertionPos = DisplayTree->GetChild();
		while (InsertionPos != NULL)
		{
			if (InsertionPos->IsKindOf(CC_RUNTIME_CLASS(SGDisplayLibColGroup)))
				break;

			InsertionPos = InsertionPos->GetNext();
		}

		// And add the group to the tree
		if (InsertionPos != NULL)
			InsertionPos->InsertBefore(DisplayDocument);		// Insert in front of libraries
		else
			DisplayTree->AddItem(DisplayDocument);				// No libraries - just AddTail

		// And ensure it is folded/unfolded as appropriate (but don't bother redrawing)
		DisplayDocument->SetFoldedState((ParentDoc != Document::GetSelected()), FALSE);
	}

	ColourList *ColList = ParentDoc->GetIndexedColours();
	IndexedColour *CurColour = (ColList == NULL) ? NULL : (IndexedColour *) ColList->GetHead();

	while (CurColour != NULL)										// For each colour in the doc...
	{
//		if (!CurColour->IsDeleted() && CurColour->IsNamed())		// If alive & named...

// WEBSTER - markn 5/12/96
// New func that decides weather the colour should be added to the gallery
// called CanAddColourToGallery()

		if (CanAddColourToGallery(CurColour,ColList))		// If alive & named...
		{
			DisplayColour = new SGDisplayColour(CurColour);
			if (DisplayColour == NULL)
			{
				InformError();
				return;
			}

			DisplayDocument->AddItem(DisplayColour);				// Create & add item for it
		}

		CurColour = (IndexedColour *) ColList->GetNext(CurColour);	// Next colour
	}
}


/********************************************************************************************

>	static BOOL ColourSGallery::CanAddColourToGallery(IndexedColour *pCol,ColourList* pColList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/12/96

	Inputs:		pCol = ptr to the index colour in question
				pColList = ptr to the Colour List for the document

	Returns:	TRUE if the colour should be displayed in the gallery
				FALSE otherwise

	Purpose:	Central point where it decides whether the colour should be displayed in the gallery

				Added for WEBSTER

********************************************************************************************/

BOOL ColourSGallery::CanAddColourToGallery(IndexedColour *pCol,ColourList* pColList)
{
	ERROR2IF(pCol == NULL,FALSE,"NULL colour ptr");
	ERROR2IF(pColList == NULL,FALSE,"NULL colour list ptr");

//#ifndef WEBSTER
	return (!pCol->IsDeleted() && pCol->IsNamed());		// v1.5 implementation
//#else
//	if (!pCol->IsDeleted() && pCol->IsNamed())
//	{
//		if (pCol->HasLinkedChildren() && pCol->FindLinkedParent() == NULL)
//		{
//			// OK, the colour has a child yet no parent, so it must be a top-level parent colour?
//			// Well, not quite.
//			//
//			// If the colour editor has been opened, it may have created an editing colour
//			// and linked it to one of the colours in the colour list *just in case* you wish to link
//			// it to another colour
//			//
//			// This linking is done so that it can make an intelligent choice on which colour you wish
//			// to link to.
//			//
//			// The following loop checks to see if there is another *real* colour (i.e. one in the document)
//			// that is linked to this colour
//
//			IndexedColour *pColInList = (IndexedColour *) pColList->GetUndeletedHead();
//			while (pColInList != NULL)
//			{
//				if (pColInList != pCol && pColInList->IsADescendantOf(pCol))
//					return TRUE;
//		
//				pColInList = pColList->GetUndeletedNext(pColInList);
//			}
//		}
//	}
//
//	return FALSE;
//#endif // WEBSTER
}


/********************************************************************************************

>	BOOL ColourSGallery::PreCreate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/94

	Returns:	TRUE if the Gallery initialised successfully
				FALSE if it should not be opened due to a failure to initialise

	Purpose:	The ColourSGallery PreCreate handler. This overrides the base class
				PreCreate function. It is called at the very start of the
				SuperGallery::Create method, before the window has been created.

********************************************************************************************/

BOOL ColourSGallery::PreCreate(void)
{
	// If there isn't already one, create a DisplayTree
	if (DisplayTree == NULL)
	{
		DisplayTree = new SGDisplayRootScroll(this);	// New root node, with a scrollbar
		if (DisplayTree == NULL)
			return(FALSE);
	}

	// We're opening the window, so we fold all groups except that for the Selected document
	SGDisplayGroup *Group = (SGDisplayGroup *) DisplayTree->GetChild();
	while (Group != NULL)
	{
		// Set the group's folded state, but don't bother trying to redraw, because we know
		// that the window is not currently visible, and that we are doing this to multiple groups.
		Document *pDoc = Group->GetParentDocument();

		if (pDoc != NULL)	// Leave library groups alone
			Group->SetFoldedState((pDoc != Document::GetSelected()), FALSE);

		Group = (SGDisplayGroup *) Group->GetNext();
	}

	// And for each document already present, (re)create a display subtree (possibly replacing
	// a previous displaytree for it if we had one earlier)
	Document *ParentDoc = (Document *) GetApplication()->Documents.GetTail();
	while (ParentDoc != NULL)
	{
		SGDisplayColourGroup *ParentGroup = (SGDisplayColourGroup *)DisplayTree->FindSubtree(this, ParentDoc, NULL);

		CreateNewSubtree(ParentDoc, ParentGroup);
		ParentDoc = (Document *) GetApplication()->Documents.GetPrev(ParentDoc);
	}

	// Only add the library groups if they're not already in the gallery
	SGDisplayNode *Ptr = DisplayTree->GetChild();
	while (Ptr != NULL)
	{
		if (Ptr->IsKindOf(CC_RUNTIME_CLASS(SGDisplayLibColGroup)))
			break;

		Ptr = Ptr->GetNext();
	}

PORTNOTE("other", "Colour gallery needs to use the resource system for finding palettes when no path is specified");
#ifndef EXCLUDE_FROM_XARALX
	if (Ptr == NULL)
	{
		SGDisplayLibColGroup *Bob;

		String_256 SearchPath;
		// If the user has set a pathname in the preference then use this, otherwise
		// use a Palettes directory which is sitting alongside the exe.
		if (PalettePath.IsEmpty())
		{
			// Find the directory path of the .exe file
			CResDll::GetExecutablePath((TCHAR *) SearchPath);

			SearchPath += TEXT("\\Palettes\\");	// This is a fixed (non-internationalised) string
		}
		else
		{
			SearchPath = PalettePath;
			// Add a trailing slash if it hasn't got one
			SGLibOil::AppendSlashIfNotPresent(&SearchPath);
		}
			
		// Make up the search string to use so that we see all files.
		String_256 SearchSpec(SearchPath);
		SearchSpec += TEXT("*.*");	// This is a fixed (non-internationalised) string

		if (FileUtil::StartFindingFiles(&SearchSpec))
		{
			String_256 LeafName;

			while (FileUtil::FindNextFile(&LeafName))
			{
				String_256 FullPath = SearchPath;
				//FullPath += TEXT("\\Palettes\\");
				FullPath += LeafName;

				PathName Name(FullPath);
				Bob = new SGDisplayLibColGroup(this, &Name);
				if (Bob != NULL)
					DisplayTree->AddItem(Bob);
			}
			FileUtil::StopFindingFiles();
		}
	}
#endif

	// Ensure we know we have to reformat before the next redraw
	InvalidateCachedFormat();

	return(TRUE);
}

/********************************************************************************************

>	BOOL ColourSGallery::MakeSureGroupsHaveBeenCreated()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		-
	Returns:	True if everything went ok, False otherwise.
	Purpose:	This will create the gallery groups (unless they're already there)
				and return TRUE if ok.

********************************************************************************************/
 
BOOL ColourSGallery::MakeSureGroupsHaveBeenCreated()
{
	// We will use the DisplayTree presence or absence to dictate whether the groups are
	// there or not. If it is there just return TRUE as everything should be fabby.
	if (DisplayTree != NULL)
		return TRUE;

	// If not then just call PreCreate to ensure that they are present.
	return PreCreate();
}


/********************************************************************************************

>	SGDisplayLibColGroup * ColourSGallery::GetNextLibGroup(SGDisplayLibColGroup * pLibGroup)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		-
	Returns:	NULL or pointer to the next LibGroup in the list.
	Purpose:	Gets the first colour library group node in the colour gallery.

********************************************************************************************/

SGDisplayLibColGroup * ColourSGallery::GetFirstLibGroup()
{
	return GetNextLibGroup(NULL);
}

/********************************************************************************************

>	SGDisplayLibColGroup * ColourSGallery::GetNextLibGroup(SGDisplayLibColGroup * pLibGroup)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/97
	Inputs:		-
	Returns:	NULL or pointer to the next LibGroup in the list.
	Purpose:	Gets the next colour library group node in the colour gallery.

********************************************************************************************/

SGDisplayLibColGroup * ColourSGallery::GetNextLibGroup(SGDisplayLibColGroup * pLibGroup)
{
	SGDisplayNode *pGroup = NULL;
	// If we are passed a null pointer then somebody must want the first item in the list
	if (pLibGroup == NULL)
	{
		if (DisplayTree == NULL)
			return NULL;

		// Find the first library groups in the gallery
		pGroup = DisplayTree->GetChild();
	}
	else
		pGroup = pLibGroup->GetNext();

	while (pGroup != NULL)
	{
		if (pGroup->IsKindOf(CC_RUNTIME_CLASS(SGDisplayLibColGroup)))
		{
			return (SGDisplayLibColGroup*)pGroup;
		}

		pGroup = pGroup->GetNext();
	}

	// none found
	return NULL;
}




/********************************************************************************************

>	void ColourSGallery::EditColour(ColourList *ParentList, IndexedColour *TheColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		ParentList - The ColourList in which the given colour resides
				TheColour  - the colour to edit, or NULL to just open the editor

	Purpose:	Opens the colour editor, and sets it to edit the given colour

	SeeAlso:	ColourPicker

********************************************************************************************/

void ColourSGallery::EditColour(ColourList *ParentList, IndexedColour *TheColour)
{
PORTNOTE("other", "disabled colour picker");
#ifndef EXCLUDE_FROM_XARALX
	ColourPicker ColPicker;
	ColPicker.EditColour(ParentList, TheColour);
#endif
}



/********************************************************************************************

>	BOOL ColourSGallery::OKToDeleteSelection(SGDisplayGroup *DocumentGroup)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		DocumentGroup - The subtree in which the selection lies
				This is expected to be the subtree for the selected document

	Returns:	TRUE if it is OK to go ahead and delete, or
				FALSE if the delete action has been cancelled

	Purpose:	To determine if it is OK to go ahead and delete the selected colours,
				and to possibly modify the selection if some colours should not be deleted.

				This checks with the user - they can either delete all colours, delete
				only those which are not 'in use', or cancel the delete. In the first case,
				this method does nothing (returns TRUE); in the second, it removes in-use
				colours from the selection and returns TRUE; in the third case, it does
				nothing, but returns FALSE.

	SeeAlso:	ColourSGallery::DeleteSelection

********************************************************************************************/

BOOL ColourSGallery::OKToDeleteSelection(SGDisplayGroup *DocumentGroup)
{
	ERROR3IF(DocumentGroup == NULL, "Illegal NULL param");

	SGDisplayColour *Item = (SGDisplayColour *) DocumentGroup->FindNextSelectedItem(NULL);
	IndexedColour *RefColour;
	INT32 ItemsInUse = 0;
	INT32 TotalItems = 0;
	INT32 DeletableItems = 0;

	Document *ParentDoc = DocumentGroup->GetParentDocument();
	if (ParentDoc == NULL)
		return(FALSE);

	ColourList *ParentColList = ParentDoc->GetIndexedColours();
	if (ParentColList == NULL)
		return(FALSE);

	// Determine how many items are selected, and how many of these are in use
	while (Item != NULL)
	{
		if (!Item->IsALibraryColour())
		{
			DeletableItems++;				// Increment number of deletable (non-library) items

			// Check if the colour is in use. NOTE that because it is used in this gallery
			// display item, it's usage count will be 1 greater than the 'real' usage
			// (i.e. when we delete it, the usage count will be decremented once, so
			// it is considered not-in-use if the usagcount is <= 1)
			RefColour = Item->GetDisplayedColour()->FindParentIndexedColour();
			if (RefColour != NULL && ParentColList->IsColourInUseInDoc(RefColour, TRUE))
				ItemsInUse++;
		}

		TotalItems++;						// Increment number of found items

		Item = (SGDisplayColour *) DocumentGroup->FindNextSelectedItem(Item);
	}

	if (DeletableItems == 0)
		return(FALSE);

	// If some are in use, determine if the user really wants to delete them
	INT32 ButtonPressed = 2;
	if (ItemsInUse != 0)
	{
		ButtonPressed = AskQuestion(_R(IDS_COLGAL_COLINUSE),
									_R(IDS_COLGAL_KILL), _R(IDS_COLGAL_NOKILL), _R(IDS_CANCEL), 0,
									2, 3);	// NOKILL=default, CANCEL=cancel

		if (ButtonPressed == 3)	// Delete was cancelled - return FALSE
			return(FALSE);
	}

	if (ButtonPressed == 1)		// Force-delete all items, so go ahead and delete
		return(TRUE);

	if (ButtonPressed == 2 && DeletableItems - ItemsInUse <= 0)
		return(FALSE);			// No items left to delete (all in use) so abort

	// Remove all in-use items from the selection so we only delete 'free' colours
	Item = (SGDisplayColour *) DocumentGroup->FindNextSelectedItem(NULL);

	while (Item != NULL)
	{
		RefColour = Item->GetDisplayedColour()->FindParentIndexedColour();
		if (RefColour != NULL && ParentColList->IsColourInUseInDoc(RefColour, TRUE))
			Item->SetSelected(FALSE);

		Item = (SGDisplayColour *) DocumentGroup->FindNextSelectedItem(Item);
	}

	return(TRUE);
}



/********************************************************************************************

>	BOOL ColourSGallery::DeleteSelection(SGDisplayGroup *DocumentGroup,
										 ColourList *ParentList)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		DocumentGroup - The subtree in which the selection lies
				This is expected to be the subtree for the selected document

				ParentList - The ColourList in which all these colours reside

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Forcibly deletes all selected colours
				Should be used after OKToDeleteSelection (which confirms the action)

	SeeAlso:	ColourSGallery::OKToDeleteSelection

********************************************************************************************/

BOOL ColourSGallery::DeleteSelection(SGDisplayGroup *DocumentGroup, ColourList *ParentList)
{
	if (ParentList == NULL)
		return(TRUE);

	INT32 TotalItems = DocumentGroup->GetSelectedItemCount();
	if (TotalItems == 0)
		return(TRUE);

	IndexedColour **KillList = new IndexedColourPtr[TotalItems+1];
	if (KillList == NULL)
		return(FALSE);

	DWORD KillIndex = 0;
	SGDisplayColour *Item = (SGDisplayColour *) DocumentGroup->FindNextSelectedItem(NULL);
	SGDisplayColour *NextItem;
	IndexedColour *RefColour;

	while (Item != NULL)
	{
		// Remember the NextItem item, cos we're about to delink/destroy this one
		NextItem = (SGDisplayColour *) DocumentGroup->FindNextSelectedItem(Item);

		RefColour = Item->GetDisplayedColour()->FindParentIndexedColour();
		if (RefColour != NULL)
		{
			// Add the colour to the kill list
			KillList[KillIndex++] = RefColour;

			// Annihilate this item, so that it no longer references the IndexedColour
			Item->DestroySubtree();
		}

		// And move on to the NextItem item
		Item = NextItem;
	}
	
	KillList[KillIndex] = NULL;		// NULL terminate the list

	// Now cause a redraw of the tree in its new state, before the message broadcast goes around
	InvalidateCachedFormat();
	ReformatAndRedrawIfNecessary();

	// Delete (hide, with undo actually) the given colours
	// Our message handler will cope with redrawing the new list (we must do it this
	// way in case the deletion causes other changes to the list, e.g. a new 'black'
	// being created if we delete the default one)
	ColourManager::HideColours(ParentList, KillList, TRUE);

	// HideColours has made a copy of this list for itself, so we are responsible
	// for deleting our original list
	delete [] KillList;


	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL ColourSGallery::ApplyAction(SGActionType Action)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95

	Inputs:		Action - Indicates what action to apply

	Returns:	TRUE to indicate successful handling of the action, or
				FALSE to indicate failure

	Purpose:	Applies certain conventional gallery actions (usually associated with
				gallery buttons, for new, edit, delete, etc)

	SeeAlso:	SGActionType

********************************************************************************************/

BOOL ColourSGallery::ApplyAction(SGActionType Action)
{
	// No display tree? Or worse, no active docuemnts? Better forget about it then!
	if (DisplayTree == NULL || Document::GetSelected() == NULL)
		return(FALSE);

	SGDisplayColour *FirstSelected = (SGDisplayColour *)
										DisplayTree->FindNextSelectedItem(NULL);

	SGDisplayGroup	*DocumentGroup	= NULL;
	Document		*ScopeDoc		= NULL;
	ColourList		*ColList		= NULL;
	IndexedColour	*SelectedColour	= NULL;
	DocColour		*TheColour		= NULL;

	if (FirstSelected != NULL)
	{
		DocumentGroup = (SGDisplayGroup *) FirstSelected->GetParent();
		ERROR3IF(DocumentGroup == NULL, "SGallery DisplayTree linkage corruption detected");

		ScopeDoc = DocumentGroup->GetParentDocument();
		if (ScopeDoc == NULL)		// Must be a library item, so it will affect the selected doc
			ScopeDoc = Document::GetSelected();
		ERROR3IF(ScopeDoc == NULL, "No scope document for colour gallery operation!");

		ColList = ScopeDoc->GetIndexedColours();
		ERROR3IF(ColList == NULL, "A document with no colour list? Now I've seen it all");

		TheColour = FirstSelected->GetDisplayedColour();
		ERROR3IF(TheColour == NULL, "SGDisplayColour has no colour in it?");
		
		SelectedColour = TheColour->FindParentIndexedColour();
	}


	BOOL Adjust = FALSE;

	// Now, process the action
	switch(Action)
	{
		case SGACTION_CREATE:
			ColList = Document::GetSelected()->GetIndexedColours();
			ERROR3IF(ColList == NULL, "A document with no colour list? Now I've seen it all");
			if (ColList != NULL)
			{
				BOOL OldSentState = ISentTheMessage;
				ISentTheMessage = TRUE;

				IndexedColour *NewCol = NULL;

				if (FirstSelected == NULL || !FirstSelected->IsALibraryColour() || TheColour == NULL)
					NewCol = ColourManager::GenerateNewNamedColour(ColList, SelectedColour);
				else
				{
					// A library doccolour - we must copy it into the document before trying to apply it
					// Make a temporary IndexedColour from the library colour
					NewCol = new IndexedColour(*TheColour);

					if (NewCol != NULL)
					{
						SGDisplayLibColour *LibCol = (SGDisplayLibColour *) FirstSelected;

						// Set the colour's name to the same as the library item (but make sure it's unique)
						String_256 Buffer;
						LibCol->GetNameText(&Buffer);
						NewCol->SetName(Buffer);
						if (ColList->GenerateUniqueColourName(NewCol->GetName(), (String_64 *) &Buffer))
							NewCol->SetName(Buffer);

						// If it's a spot colour, make it so
						if (LibCol->IsASpotColour())
							NewCol->SetLinkedParent(NULL, COLOURTYPE_SPOT);

						// Copy the colour into the destination document (merging it with existing
						// colours so we won't keep creating new copies of the same colour as it's applied)
						DocColour ColourToApply;
						ColourToApply.MakeRefToIndexedColour(NewCol);
						ColourManager::EnsureColourIsInDocument(NULL, Document::GetSelected(), &ColourToApply);

						// Delete the temporary IndexedColour we used
						delete NewCol;

						// And remember the new colour we've just made
						NewCol = ColourToApply.FindParentIndexedColour();
					}
				}

				// If NewCol == NULL, either an error occured, or more likely, the user cancelled

				ISentTheMessage = OldSentState;

				if (NewCol != NULL)
				{
					// Bring up the colour editor on the new colour
					EditColour(ColList, NewCol);

					// And make the new item the selected one in the list
					SelectItems(FALSE);			// Deselect everything else

					// Find the item and select it
					SGDisplayGroup *DocGroup = DisplayTree->FindSubtree(this,
														Document::GetSelected(), NULL);
					if (DocGroup != NULL)
					{
						// We've found the group for the selected document
						SGDisplayColour *Ptr = new SGDisplayColour(NewCol);
						if (Ptr != NULL)
						{
							// Add the item to the group using the last applied sort mode
							DocGroup->AddItem(Ptr, SortKeys);

							// And rearrange the real colour item to the same position by inserting
							// it after the colour which is previous in the display list. If there
							// is no display-list-previous-item, then we insert at the head.
							ColourList *ColList = ColourManager::GetColourList();
							ColList->RemoveItem(NewCol);

							if (ColList->GetHead() == NULL)
							{
								// Thanks, Mario. If you try to add an item when the list is empty
								// he just ignores you (no ensure, no useful default behaviour)
								ColList->AddItem(NewCol);
							}
							else
							{
								if (Ptr->GetPrevious() == NULL)
									ColList->InsertBefore(ColList->GetHead(), NewCol);
								else
									ColList->InsertAfter(((SGDisplayColour *)Ptr->GetPrevious())->
														GetDisplayedColour()->FindParentIndexedColour(), NewCol);
							}

							ForceGroupFolded(DocGroup, FALSE);	// Ensure group is unfolded and tree reformatted

							// Select the new node, and remember it as the new selection anchor
							Ptr->SetSelected(TRUE);
							SetLastSelectedNode(Ptr);

							// Ensure the format is recalculated as a result of adding the new item
							ReformatAndRedrawIfNecessary();

							// And now we can get its format rect, and scroll to show it
							DocRect ItemFormatRect;
							Ptr->GetFormatRect(&ItemFormatRect);
							ScrollToShow(&ItemFormatRect);
						}
					}
					else
					{
						ERROR3("Colour DisplayGroup not found for Selected Document!");
					}

					SelectionHasChanged();
				}
			}
			break;


		case SGACTION_APPLYADJUST:
			Adjust = TRUE;
			// Drop through to SGACTION_APPLY handler

		case SGACTION_APPLY:
			if (TheColour != NULL && (SelectedColour != NULL || FirstSelected->IsALibraryColour()))
			{
				DocColour ColourToApply;

				if (!FirstSelected->IsALibraryColour())
				{
					// Normal IndexedColour item - just apply it
					ColourToApply.MakeRefToIndexedColour(SelectedColour);

					// Ensure this colour (or an equivalent) is available in the selected doc
					// (which is where the apply will take place), and modify ColourToApply
					// (if necessary) to reference the safe colour for the dest doc.
					ColourManager::EnsureColourIsInDocument(ScopeDoc, Document::GetSelected(), &ColourToApply);
				}
				else
				{
					// A library doccolour - we must copy it into the document before trying to apply it
					// Make a temporary IndexedColour from the library colour
					IndexedColour *NewCol = new IndexedColour(*TheColour);

					if (NewCol != NULL)
					{
						SGDisplayLibColour *LibCol = (SGDisplayLibColour *) FirstSelected;

						// Set the colour's name to the same as the library item
						String_256 Buffer;
						LibCol->GetNameText(&Buffer);
						NewCol->SetName(Buffer);

						// If it's a spot colour, make it so
						if (LibCol->IsASpotColour())
							NewCol->SetLinkedParent(NULL, COLOURTYPE_SPOT);

						// Copy the colour into the destination document (merging it with existing
						// colours so we won't keep creating new copies of the same colour as it's applied)
						ColourToApply.MakeRefToIndexedColour(NewCol);
						ColourManager::EnsureColourIsInDocument(NULL, Document::GetSelected(), &ColourToApply);

						// And delete the temporary IndexedColour we used
						delete NewCol;
						NewCol = NULL;
					}
				}

				if (ColourToApply.FindParentIndexedColour() != NULL)
				{
					// We've got a colour to apply, so apply it

					NodeAttribute *Attrib;
					if (Adjust)
					{
						// Adjust-ed, so set the line colour
						Attrib = new AttrStrokeColourChange;
						if (Attrib == NULL)
						{
							InformError();
							return(FALSE);
						}

						((AttrStrokeColourChange *)Attrib)->SetStartColour(&ColourToApply);
					}
					else
					{
						// Select-ed so set the fill colour
						Attrib = new AttrColourChange;
						if (Attrib == NULL)
						{
							InformError();
							return(FALSE);
						}

						((AttrColourChange *)Attrib)->SetStartColour(&ColourToApply);
					}

					// AttributeSelected knows what to do with a selected attribute
					AttributeManager::AttributeSelected(NULL, Attrib);
				}
			}
			break;


		case SGACTION_REDEFINE:
			if (SelectedColour != NULL && TheColour != NULL)
			{
				// First, try to find an interesting colour to redefine from
				DocColour SourceColour;
				ColourList *SrcColList;

				ColourManager::FindColourOfInterestToUser(&SourceColour, &SrcColList);

				if (SourceColour.FindParentIndexedColour() == SelectedColour)
				{
					// You can't redefine a colour from itself! Don't be daft!
					InformError(_R(IDE_COLGAL_SAMEITEM));
					return(FALSE);
				}

				// Copy the colour
				IndexedColour *NewCol = new IndexedColour(SourceColour);
				if (NewCol == NULL)
					return(FALSE);

				if (SrcColList != ColList || NewCol->IsADescendantOf(SelectedColour))
				{
					// We're redefining from a colour in a different document, or attempting to make a
					// recursive parent-pointer loop. Remove the parent linkage to make NewCol a standalone
					// copy of its previous parent.
					NewCol->SetLinkedParent(NULL,
							(NewCol->GetType() == COLOURTYPE_SPOT) ? COLOURTYPE_SPOT : COLOURTYPE_NORMAL);
				}
  
				// Compose a new, unique, name for it - e.g. "Red (Redefined)"
				String_128 NewName(_R(IDS_UNNAMEDCOLOUR));
				if (SelectedColour->IsNamed())
					NewName.MakeMsg(_R(IDS_REDEFINEDCOLOUR), (TCHAR *) (*(SelectedColour->GetName())) );

				// Ensure the new name is unique within the destination colour list
				String_64 UniqueNewName;
				ColList->GenerateUniqueColourName(&NewName, &UniqueNewName);
				NewCol->SetName(UniqueNewName);

				// And finally, apply the colour change, with UNDO
				// (Note that NewCol is put into the undo system, so we must not delete it)
				return(ColourManager::ChangeColour(ColList, NewCol, SelectedColour));
			}
			break;


		case SGACTION_EDIT:
			if (ScopeDoc != Document::GetSelected())
			{
				// **** BODGE! TO DO !!!!
// This should make the appropriate document selected by chucking its window to the front, and
// then invoke the colour editor on the selected colour

				InformMessage(_R(IDS_COLGAL_NOTSELDOC), _R(IDS_CANCEL));
				break;
			}

			// Always show the editor, and if there is a selection, edit the
			// first selected colour
			EditColour(ColList, SelectedColour);
			break;


		case SGACTION_DELETE:
			{
				while (DocumentGroup != NULL)
				{
					// For each document, if there are selected items in the document, then
					// delete them (after asking the user for each document) TODO : Clean up!!!!

					ScopeDoc = DocumentGroup->GetParentDocument();
					if (ScopeDoc != NULL)
					{
						ColList = ScopeDoc->GetIndexedColours();
						ERROR3IF(ColList == NULL,
								"A document with no colour list? Now I've seen it all");

						if (DocumentGroup->GetSelectedItemCount() > 0)
						{
							if (OKToDeleteSelection(DocumentGroup))
							{
								if (!DeleteSelection(DocumentGroup, ColList))
									return(FALSE);
							}
						}
					}

					DocumentGroup = (SGDisplayGroup *) DocumentGroup->GetNext();
				}

				SelectionHasChanged();
			}
			break;


		case SGACTION_SETOPTIONS:	// Set values in the options/sort dialogue as it is opened
			if (CurrentOptionsDlg != NULL)
			{																	// Display Modes
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_SMALL));		// 0
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_FULLINFO));	// 1
				CurrentOptionsDlg->AddDisplayModeName(_R(IDS_GALLERYDM_ICONONLY));	// 2
			}
			break;

		case SGACTION_SETSORTMODE:
			if (CurrentSortDlg != NULL)
			{																// Sort Modes (0 = none)
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_NAME));			// 1 (sort by name)
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_HUE));				// 2
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_INTENSITY));		// 3
				CurrentSortDlg->AddSortKeyName(_R(IDS_SORTBY_MODEL));			// 4
			}
			break;


		case SGACTION_DISPLAYMODECHANGED:
			// Ensure that the display mode is one of the supported values
			if (DisplayMode < 0 || DisplayMode > 2)
				DisplayMode = 0;
			break;


		default:
			return(FALSE);
	}

	return(TRUE);
}



/********************************************************************************************

>	void ColourSGallery::RedrawColourDisplay(Document *TheDocument, IndexedColour *TheColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95

	Inputs:		TheDocument - The document containing the colour
				TheColour - The colour which needs to be re-displayed

	Purpose:	Redraws the display item for one specific colour, and all of its linked
				descendants (liked/tint colours) (if any) in a given document.

	Notes:		If the specified colo9ur is unnamed, nothing is done

	SeeAlso:	SuperGallery::RedrawDocumentDisplay

********************************************************************************************/

void ColourSGallery::RedrawColourDisplay(Document *TheDocument, IndexedColour *TheColour)
{
	if (TheDocument == NULL || TheColour == NULL)
		return;

	SGDisplayGroup *Group = DisplayTree->FindSubtree(this, TheDocument, NULL);

	if (Group == NULL)		// Can't find a group for this document
		return;

	// Now search the tree for the specific items to redraw
	SGDisplayColour *Item = (SGDisplayColour *) Group->GetChild();

	// WEBSTER - markn 28/1/97
	// This can be NULL in webster when you edit a non-editable colour (e.g. by double-clicking
	// on a colour in the web palette folder)
	// In Camelot it expects a colour to be added to the folder containing the doc's colours,
	// but for Webster that is not the case. It is now (mab) Martin 15/07/97
	if (Item != NULL)
	{
		ERROR3IF(Item->GetChild() != NULL, "My leaf-item isn't a leaf-item! Help!");

		IndexedColour *RealColour;

		while (Item != NULL)
		{
			RealColour = Item->GetDisplayedColour()->FindParentIndexedColour();
			if (RealColour != NULL && RealColour->IsADescendantOf(TheColour))
				Item->ForceRedrawOfMyself();				// Found it, or a descendant - redraw

			Item = (SGDisplayColour *) Item->GetNext();
		}
	}
}



/********************************************************************************************

>	void ColourSGallery::SetSelectionFromDocument(BOOL AlwaysScroll = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95 (copied from old ColourGallery, previously created 11/10/94)

	Inputs:		AlwaysScroll - TRUE if you always want to autoscroll, regardless of the
				preference setting. (This is used when opening the gallery, as there is no
				point in not auto-scrolling somewhere useful)

	Purpose:	Sets the colour gallery listbox selection state based upon the currently
				selected object(s) in the document (to show the current fill/stroke colours)

	Scope:		private

********************************************************************************************/

void ColourSGallery::SetSelectionFromDocument(BOOL AlwaysScroll)
{
	if (DisplayTree == NULL || !IsVisible())
		return;
	
	DocColour *SelColour[2];
	BOOL AreCurrentAttrs =
			ColourManager::GetCurrentLineAndFillColours(&SelColour[0], &SelColour[1]);

	SGDisplayGroup *DocGroup = NULL;
	if (Document::GetSelected() != NULL)
		DocGroup = DisplayTree->FindSubtree(this, Document::GetSelected(), NULL);
	SelectItems(FALSE);					// Clear the current selection

	DocRect ScrollToRect;
	BOOL HaveSelected = FALSE;
	BOOL ScrollToBottom = FALSE;	// TRUE if it is more important that the bottom of
									// ScrollToRect, rather than the top, is visible

	if (DocGroup != NULL)
		ForceGroupFolded(DocGroup, FALSE);	// Ensure group is unfolded

	ReformatNow(TRUE);				// Ensure the tree format information is up to date

	if (!AreCurrentAttrs && DocGroup != NULL)
	{
		// The line/fill colours returned are those of the selection, not the current attrs
		IndexedColour *Parent;

		for (INT32 i = 0; i < 2; i++)			// For each colour (line, fill)
		{
			if (SelColour[i] != NULL)		// If there is a single selected colour
			{
				Parent = SelColour[i]->FindParentIndexedColour();		// Find the named IxColour
				if (Parent != NULL && Parent->IsNamed())				// (if any)
				{
					// See if we can find a display item for this colour, and select it
					SGDisplayColour *Ptr = (SGDisplayColour *)DocGroup->GetChild();
					while (Ptr != NULL)
					{
						if (Ptr->GetDisplayedColour()->FindParentIndexedColour() == Parent)
						{
							// determine the rectangle to try to show on screen (the
							// union of both the selected items)
							if (!HaveSelected)
								Ptr->GetFormatRect(&ScrollToRect);
							else
							{
								DocRect TempRect;
								Ptr->GetFormatRect(&TempRect);

								// We must be adding the fill colour. If this is below the
								// line colour, then we'd prefer to scroll to show the
								// bottom of the rectangle, as it is more important that
								// the fill rather than line is visible if won't both fit.
								ScrollToBottom = (TempRect.hi.y <= ScrollToRect.lo.y);

								ScrollToRect = ScrollToRect.Union(TempRect);
							}								

							// And select the item, and update the selection-anchor
							Ptr->SetSelected(TRUE);
							SetLastSelectedNode(Ptr);

							HaveSelected = TRUE;
							break;
						}

						Ptr = (SGDisplayColour *) Ptr->GetNext();
					}
				}
			}
		}
	}

	if (AutoScrollSelection || AlwaysScroll)
	{
		if (AlwaysScroll && !HaveSelected && DocGroup != NULL)
		{
			// We didn't select anything in the gallery. In that case, scroll to the top
			// of the document display
			DocRect TempRect;
			DocGroup->GetFormatRect(&TempRect);
			DocGroup->GetChildArea(&ScrollToRect);
			ScrollToRect = ScrollToRect.Union(TempRect);
			HaveSelected = TRUE;
			ScrollToBottom = FALSE;
		}

		if (HaveSelected)
			ScrollToShow(&ScrollToRect, ScrollToBottom);
	}
												// scroll to make seln visible on screen

	SelectionHasChanged();						// Update for the new selection
}



/********************************************************************************************

>	virtual MsgResult ColourSGallery::Message(Msg* Message)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/10/94
	Inputs:		Message - The message to handle

	Purpose:	A standard message handler, really.

	Notes:		Any messages that this does not handle must be passed down to the
				SuperGallery base class message handler.

	SeeAlso:	SuperGallery::Message

********************************************************************************************/

MsgResult ColourSGallery::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				SGInit::UpdateGalleryButton(_R(OPTOKEN_DISPLAYCOLOURGALLERY), TRUE);
				SetSelectionFromDocument(TRUE);
				break;

			case DIM_CANCEL:
				SGInit::UpdateGalleryButton(_R(OPTOKEN_DISPLAYCOLOURGALLERY), FALSE);
				break;

			case DIM_LFT_BN_CLICKED:
				if (DisplayTree == NULL || !IsVisible())
					break;

				if (FALSE) {}
				else if (Msg->GadgetID == _R(IDC_GALLERY_NAME))
				{
					SGDisplayColour *FirstSelected = (SGDisplayColour *)
										DisplayTree->FindNextSelectedItem(NULL);
					if (FirstSelected != NULL)
					{
						IndexedColour *ColToEdit = FirstSelected->GetDisplayedColour()->
														FindParentIndexedColour();

						SGDisplayGroup *DocumentGroup = (SGDisplayGroup *) FirstSelected->GetParent();
						ERROR3IF(DocumentGroup == NULL, "SGallery DisplayTree linkage corruption detected");

						Document *ScopeDoc = DocumentGroup->GetParentDocument();
						ERROR3IF(ScopeDoc == NULL, "SGallery group is not for a document! Unimplemented! Eek!");

						ColourList *ColList = ScopeDoc->GetIndexedColours();
						ERROR3IF(ColList == NULL, "A document with no colour list? Now I've seen it all");

						if (ColToEdit != NULL)
							ColourNameDlg::InvokeDialog(ColList, ColToEdit);
					}
				}
				else if (Msg->GadgetID == _R(IDC_GALLERY_HELP))
				{
					TRACEUSER( "Markn", _T("Hello. Do you want help on this gallery?\n"));
					HelpUserTopic(_R(IDS_HELPPATH_Gallery_Colour));
				}
				else if (Msg->GadgetID == _R(IDC_COLGAL_BACKGROUND))	// Set a background layer with the selected colour
				{
					ApplySelectedAsBackground();
				}
				break;

			default:
				break;
		}

		return(SuperGallery::Message(Message));
	}


	// If we have no displaytree, then we have not been shown, or something terrible has
	// happened, so we don't bother handling any of these messages.
	if (DisplayTree == NULL || !IsVisible())
		return(SuperGallery::Message(Message));


	if (MESSAGE_IS_A(Message, DocChangingMsg))
	{
		DocChangingMsg *TheMsg = (DocChangingMsg *) Message;

		switch ( TheMsg->State )
		{
			case DocChangingMsg::BORN:			// New document - add to display tree
				{
					CreateNewSubtree(TheMsg->pChangingDoc);	// Add a subtree for this doc
					ShadeGallery(FALSE);

					InvalidateCachedFormat();				// And redraw what is necessary
					ReformatAndRedrawIfNecessary();
				}
				break;		// Pass this message on to the base class as well

//			case DocChangingMsg::DocState::ABOUTTODIE:		// Document dying - remove from tree
//			case DocChangingMsg::DocState::KILLED:
//			case DocChangingMsg::DocState::SELCHANGED:
//
//				==== NOTE ====
//				The base sgallery class handles these messages to automatically remove
//				the subtree for the dying doc, and/or shade the gallery if no docs are left
			default:
				break;

		}
	}
	else if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *TheMsg = (ColourChangingMsg *) Message;

		switch (TheMsg->State)
		{
			case ColourChangingMsg::LISTUPDATED:
				// A document colour list has been changed. We must recreate its display subtree
				if (!ISentTheMessage && TheMsg->NewColourList != NULL)
				{
					// First, try to find the document which owns the changed list
					Document *ScopeDoc = (Document *)TheMsg->NewColourList->GetParentDocument();

					// Just check that the bi-directional link is sane!
					ERROR3IF(ScopeDoc == NULL ||
							 ScopeDoc->GetIndexedColours() != TheMsg->NewColourList,
							 "A Document colour list is somehow screwed up!");

					// If we found it, recreate the display subtree for it
					if (ScopeDoc != NULL)
					{
						SGDisplayNode *Ptr = DisplayTree->FindSubtree(this, ScopeDoc, NULL);
						CreateNewSubtree(ScopeDoc, (SGDisplayColourGroup *) Ptr);

						// And force a redraw of the entire list
						//ForceRedrawOfList();
						InvalidateCachedFormat();
						ReformatAndRedrawIfNecessary();
					}
				}				
				break;


			case ColourChangingMsg::COLOURUPDATED:
			case ColourChangingMsg::COLOURUPDATEDINVISIBLE:
				if (TheMsg->NewColourList != NULL)
				{
					Document *ScopeDoc = (Document *)TheMsg->NewColourList->GetParentDocument();
					RedrawColourDisplay(ScopeDoc, TheMsg->ChangedColour);
				}
				break;

			case ColourChangingMsg::SELVIEWCONTEXTCHANGE:
				// Colour separation/correction options for the selected view have
				// changed, so redraw the entire colour list using the new options.
				ForceRedrawOfList();
				break;

			default:
				break;
		}
	}
	// The new CommonAttrsChanged msg replaces this messages. It handles 
	// a change in the Current Attribute group associated with a tool, and a change in the
	// value of a current attribute.
	/*else if (MESSAGE_IS_A(Message, SelChangingMsg))
	{
		SelChangingMsg *Msg = (SelChangingMsg *) Message;
		switch ( Msg->State )
		{
			case SelChangingMsg::COLOURATTCHANGED:
			case SelChangingMsg::SELECTIONCHANGED:
			case SelChangingMsg::NODECHANGED:
				if (!AmShaded)			// If we're open & active, set listbox selection
					SetSelectionFromDocument();
				break;
		}
		// Drop through so the Gallery base class gets to see this message too
	}
	*/
	else if (MESSAGE_IS_A(Message, CommonAttrsChangedMsg))
	{
		if (!AmShaded)			// If we're open & active, set listbox selection
			SetSelectionFromDocument();

	}
	else if (MESSAGE_IS_A(Message, DocViewMsg))
	{
		DocViewMsg *Msg = (DocViewMsg *) Message;

		if (Msg->State == DocViewMsg::SELCHANGED)
		{
			// Selected DocView is changing - redraw to use the new DocView's colour context
			BOOL DoRedraw = TRUE;
			if (Msg->pOldDocView != NULL && Msg->pNewDocView != NULL)
			{
				// If we know the old & new views, then see if they have the same colour
				// context attached - if they do, there's no need to redraw. This eliminates
				// flicker when swapping normal views (the most common view-swap action)
				// We only check the RGB context because we assume the screen is always RGB
				ColourContext *OldCC = Msg->pOldDocView->GetColourContext(COLOURMODEL_RGBT, TRUE);
				ColourContext *NewCC = Msg->pNewDocView->GetColourContext(COLOURMODEL_RGBT, TRUE);

				if (OldCC == NewCC)
					DoRedraw = FALSE;
			}

			if (DoRedraw)
				ForceRedrawOfList();
		}
	}

	return(SuperGallery::Message(Message));
}



/********************************************************************************************

>	virtual void ColourSGallery::HandleDragStart(DragMessage *DragMsg)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		DragMsg - The DRAGSTARTED message that we've just recieved, indicating
				the type of drag being started

	Purpose:	Checks a DragMessage to see if it is a colour drag.
				If it is, then it creates a drag target for this gallerys listbox.

	Notes:		Overrides the default base-class action. Calls down to the base class
				if it is not a colour drag, so that dragging of gallery groups is allowed

********************************************************************************************/

void ColourSGallery::HandleDragStart(DragMessage *DragMsg)
{
	// If it's a colour drag, add a target for our window. If not, let the base class
	// have a look at it (to see if it is a group being dragged)
	if (DragMsg->pInfo->IsKindOf(CC_RUNTIME_CLASS(ColourDragInformation)))
	{
		/*SGColourDragTarget *NewTarget =*/ new SGColourDragTarget(this, GetListGadgetID());
	}
	else
		SuperGallery::HandleDragStart(DragMsg);
}



/********************************************************************************************

>	virtual SGDisplayItem *ColourSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
									SGDisplayGroup *DestGroup,
									SGDisplayItem *TargetPosition = NULL)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Inputs:		SourceItem - The item to copy elsewhere in the tree (see below)

				DestGroup - The group into which the item should be inserted

				TargetPosition - NULL (to insert at the end of the sibling list), or
				points to an item BEFORE which SourceItem will be copied.

	Returns:	NULL (failed) or a pointer to the new (copied) display item

	Purpose:	"Copies" the existing node in the tree in an appropriate fashion.
				
				This method is normally called when a gallery-organising drag completes,
				and it is discovered that the dragged item(s) have been dragged to a 
				different display group.

	Notes:		This derived class override copies colours between documents.
				
				Currently, this just errors to the user, as copying between docs is
				not possible, due to undo going into the selected doc. **** !!!!

	SeeAlso:	ColourSGallery::InsertCopiedItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; SGColour::CopyDisplayItem

********************************************************************************************/

SGDisplayItem *ColourSGallery::CopyDisplayItem(SGDisplayItem *SourceItem, 
						SGDisplayGroup *DestGroup, SGDisplayItem *TargetPosition)
{
	ERROR3IF(SourceItem == NULL || DestGroup == NULL, "Illegal NULL param");

	if (DestGroup->Flags.ReadOnly)
	{
		ERROR2RAW("Trying to copy a colour into a read-only (library) group");
		InformError();
		return(NULL);		// Sanity check - disallow copy into a library group!
	}

	Document *DestDoc = DestGroup->GetParentDocument();
	if (DestDoc == NULL)
	{
		ERROR2RAW("No dest doc?!");
		InformError();
		return(NULL);
	}

	// Normal colour drag between 2 documents
	DocColour *DocColToCopy = ((SGDisplayColour *) SourceItem)->GetDisplayedColour();
	ERROR3IF(DocColToCopy == NULL, "NULL displayed colour?!");

	IndexedColour *ColToCopy = NULL;
	BOOL ColIsTemporary = FALSE;

	if (((SGDisplayColour *) SourceItem)->IsALibraryColour())
	{
		// The dragged item is not resident in any document - it's in a library
		// Create a temporary IndexedColour to be copied into the doc
		ColToCopy = new IndexedColour(*DocColToCopy);
		ColIsTemporary = TRUE;

		if (ColToCopy != NULL)
		{
			// Set the colour's name to the same as the library item
			String_256 Buffer;
			((SGDisplayLibColour *) SourceItem)->GetNameText(&Buffer);
			ColToCopy->SetName(Buffer);

			if (((SGDisplayLibColour *) SourceItem)->IsASpotColour())
				ColToCopy->SetLinkedParent(NULL, COLOURTYPE_SPOT);
		}
		else
		{
			ERROR3("No memory for copied colour");
		}
	}
	else	
	{	
		ColToCopy = DocColToCopy->FindParentIndexedColour();
		ERROR3IF(ColToCopy == NULL, "NULL displayed colour?!");
	}

	if (ColToCopy == NULL)
		return(NULL);

	// If we haven't already started a component copy, set one up
	if (CurrentColComp == NULL)
	{
		CurrentColComp = (ColourListComponent *)
								DestDoc->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));
		if (CurrentColComp == NULL)
		{
			if (ColIsTemporary)
				delete ColToCopy;

			ERROR2RAW("Couldn't find a colour list component for the destination document!");
			InformError();
			return(NULL);
		}

		if (!CurrentColComp->StartComponentCopy())	// If this fails, it reports the error itself
		{
			if (ColIsTemporary)
				delete ColToCopy;

			return(NULL);
		}

		CurrentTarget = TargetPosition;
	}

	// Create a new display item, referencing the colour to be copied
	SGDisplayColour *NewDisplayItem = new SGDisplayColour(ColToCopy);
	if (NewDisplayItem == NULL)
	{
		if (ColIsTemporary)
			delete ColToCopy;

		InformError();
		return(NULL);
	}

	// Remap the display item to reference a safe colour for the destination document
	// NOTE that we use a special TRUE flag here to indicate that we know the colour
	// is always unique - this stops the ColComp from caching a pointer to our temporary
	// colour (which then is reallocated for the next colour, which then causes
	// all the colour mapping to get very very confused).
	if (CurrentColComp->CopyColourAcross(NewDisplayItem->GetDisplayedColour(), TRUE) != CCCOPY_NEWCOLOUR)
	{
		// Either the copy failed (we have no colour to display), or it was merged with
		// an existing colour (in which case there will already be a display item for it)
		delete NewDisplayItem;
		
		if (ColIsTemporary)
			delete ColToCopy;

		return(NULL);
	}

	SuperGallery::InsertCopiedItem(NewDisplayItem, DestGroup, TargetPosition);

	// If we copied a library item, then delete the temporary IndexedColour we created
	if (ColIsTemporary)
	{
		delete ColToCopy;
		ColToCopy = NULL;
	}

	// And update the item selection states, so that the copied item is the selected one
	// We poke directly at the selected flag to avoid it trying to redraw itself when we know
	// it hasn't had a chance to format itself yet.
//	SourceItem->SetSelected(FALSE);
//	NewDisplayItem->Flags.Selected = TRUE;

	return(NewDisplayItem);
}



/********************************************************************************************

>	virtual void ColourSGallery::AllItemsCopied(SGDisplayGroup *DestGroup)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95

	Purpose:	This upcall method is called after one or more items have been copied or
				moved by a gallery drag. It allows the gallery to do update things like
				broadcasting a message or getting a related window (eg colour line) to redraw
				at the end of a move/copy operation. This allows thousands of items to be
				moved/copied with only a single redraw/message-broadcast, etc at the very end.

	Notes:		The caller automatically causes the appropriate things to be redrawn, so we
				just need to update other things.
				e.g. The colour gallery broadcasts a ColourChangingMsg to make sure that the
				colour line and other interested parties all update appropriately.

	SeeAlso:	SuperGallery::CopyDisplayItem; SGDisplayItem::MoveBefore;
				SGDisplayItem::MoveAfter; ColourSGallery::AllItemsCopied

********************************************************************************************/

void ColourSGallery::AllItemsCopied(SGDisplayGroup *DestGroup)
{
	ERROR3IF(DestGroup == NULL, "Illegal NULL param");

	BOOL OldSentState = ISentTheMessage;	// Make me ignore any messages triggered by this code
	ISentTheMessage = TRUE;

	// This can be called after MoveItems as well as CopyDisplayItem calls
	// We only have to do something if the CopyDisplayItem call has started a component copy...
	if (CurrentColComp != NULL)
	{
		// Merge colours, add undo record, etc. Any new colours will be inserted after the
		// given target position, and if any colours were merged, the user is informed
		IndexedColour *Target = NULL;
		if (CurrentTarget != NULL)
			Target = ((SGDisplayColour *)CurrentTarget)->GetDisplayedColour()->
														FindParentIndexedColour();

		CurrentColComp->EndComponentCopy(Target, TRUE);
		CurrentColComp = NULL;
		CurrentTarget  = NULL;

		Document *ParentDoc = DestGroup->GetParentDocument();
		if (ParentDoc != NULL)
		{
			CreateNewSubtree(ParentDoc, (SGDisplayColourGroup*)DisplayTree->FindSubtree(this, ParentDoc, NULL));
			InvalidateCachedFormat();
			ReformatAndRedrawIfNecessary();
		}
	}
	else
	{
		// Inform the rest of the world that this colour list order has been modified
		Document *ParentDoc = DestGroup->GetParentDocument();
		if (ParentDoc != NULL)
			ColourManager::ColourListHasChanged(ParentDoc->GetIndexedColours());
	}

	ISentTheMessage = OldSentState;
}


/********************************************************************************************

>	virtual RenderRegion *ColourSGallery::CreateRenderRegion(DocRect *VirtualSize,
															ReDrawInfoType *DlgRedrawInfo)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96

	Purpose:	An overridable veneer to the DialogOp CreateOSRenderRegion method.
				This can be overriden to use a different type of renderregion for
				all of your rendering.

				If you override this, you MUST also override DestroyRenderRegion!

	Notes:		This ovverrides the base class to do the same thing, but to make our
				RenderRegion use colour correction/separation options from the selected
				view, so that we display "document colours" correctly for the selview.

	SeeAlso:	ColourSGallery::DestroyRenderRegion

********************************************************************************************/

RenderRegion *ColourSGallery::CreateRenderRegion(DocRect *VirtualSize, ReDrawInfoType *DlgRedrawInfo)
{
	return(CreateOSRenderRegion(VirtualSize, DlgRedrawInfo, TRUE));
}



/********************************************************************************************

>	virtual void ColourSGallery::DestroyRenderRegion(RenderRegion *pRender)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/96

	Purpose:	An overridable veneer to the DialogOp DestroyOSRenderRegion method.
				This can be overriden to use a different type of renderregion for
				all of your rendering.

				If you override this, you MUST also override CreateRenderRegion!

	SeeAlso:	ColourSGallery::CreateRenderRegion

********************************************************************************************/

void ColourSGallery::DestroyRenderRegion(RenderRegion *pRender)
{
	DestroyOSRenderRegion(pRender);
}






/********************************************************************************************

>	virtual BOOL ColourSGallery::InitMenuCommands(void)
												 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/9/95

	Returns:	TRUE for success

	Purpose:	Initialises any menu commands that this gallery needs.

	Notes:		Will only create the menu commands once - further calls in the future
				will return TRUE immediately wihtout doing anything.

********************************************************************************************/

BOOL ColourSGallery::InitMenuCommands(void)
{
	static BOOL MenusInitialised = FALSE;

	BOOL ok = TRUE;

	if (!MenusInitialised)
	{
		// Edits to the pop-up menu on the colour gallery

		// Initialise menu command Ops

		// "Standard" entries for options menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Find, _R(IDS_SGMENU_FIND));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Sort, _R(IDS_SGMENU_SORT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Properties, _R(IDS_SGMENU_PROPERTIES));

		// "Special" entries for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_New, _R(IDS_SGMENU_NEW));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Edit, _R(IDS_SGMENU_EDIT));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Delete, _R(IDS_SGMENU_DELETE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Redefine, _R(IDS_SGMENU_REDEFINE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_Rename, _R(IDS_SGMENU_RENAME));

		// "Standard" commands for over-list menu
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_FoldGroup, _R(IDS_SGMENU_FOLD));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_UnfoldGroup, _R(IDS_SGMENU_UNFOLD));

		ok = ok && InitMenuCommand((StringBase *) &SGCmd_NextGroup, _R(IDS_SGMENU_NEXTGROUP));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_PrevGroup, _R(IDS_SGMENU_PREVGROUP));

		// Extra new group item
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_ShowInColourLine, _R(IDS_SGMENU_SHOWINCOLOURLINE));
		ok = ok && InitMenuCommand((StringBase *) &SGCmd_SetBackground, _R(IDS_SGMENU_SETBACKGROUND));

		MenusInitialised = TRUE;
	}

	return(ok);
}



/********************************************************************************************

>	virtual BOOL ColourSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		TheMenu - The menu to add commands to
				MenuID  - The type of menu (over-list or from-options-button) to create

	Returns:	TRUE if it succeeded

	Purpose:	To build a menu of commands to be popped up over the gallery.
	
	Notes:		Override this method to stop the default menus being built

********************************************************************************************/

BOOL ColourSGallery::BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID)
{
 	BOOL ok = TRUE;

	// Edits to the pop-up menu on the colour gallery

	if (MenuID == SGMENU_OPTIONS)
	{
		// Options menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Find);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Sort);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Properties);
	}
	else
	{
		// Over-list menu
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_New);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Edit);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Delete);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Redefine);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_Rename, TRUE);		// With separator

		SGDisplayGroup *TheGroup = FindCommandGroup();		// Fold or unfold as appropriate
		if (TheGroup == NULL || !TheGroup->Flags.Folded)
			ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_FoldGroup);
		else
			ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_UnfoldGroup);

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_PrevGroup);
		// add 'Show in colour line' menu item in Camelot
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_NextGroup, TRUE);		// With separator

		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_ShowInColourLine);
		ok = ok && AddCommand(TheMenu, (StringBase *) &SGCmd_SetBackground);
	}

	return(ok);
}



/********************************************************************************************

>	virtual OpState ColourSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)

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

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine;
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

OpState ColourSGallery::GetCommandState(StringBase *CommandID, String_256 *ShadeReason)
{
	OpState State;

	if (AmShaded)		// No commands available while the gallery is shaded
	{
		State.Greyed = TRUE;
		return(State);
	}

	// Determine if the entire selection consists only of library colours...
	BOOL AllSelectionIsLibrary = TRUE;
	SGDisplayColour *Col = (SGDisplayColour *) DisplayTree->FindNextSelectedItem(NULL);
	SGDisplayColour *pSelectedItem = Col;
	if (Col == NULL)
		AllSelectionIsLibrary = FALSE;

	while (Col != NULL)
	{
		if (!Col->IsALibraryColour())
			AllSelectionIsLibrary = FALSE;

		Col = (SGDisplayColour *) DisplayTree->FindNextSelectedItem(Col);
	}

	// determine the command state...
	if (*CommandID == SGCmd_New)											// --- New
	{
		if (GetSelectedItemCount() > 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLE));			
		}
	}
	else if (*CommandID == SGCmd_Rename)									// --- Rename
	{
		if (GetSelectedItemCount() != 1)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLECOL));
		}
		else if (AllSelectionIsLibrary)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_ITSALIBRARYCOLOUR));
		}
	}
	else if (*CommandID == SGCmd_Edit		||								// --- Edit, Redefine, Delete
			 *CommandID == SGCmd_Redefine	||
			 *CommandID == SGCmd_Delete)
	{
		// Make sure we disallow these options when the selection is a library colour
		if (AllSelectionIsLibrary)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_ITSALIBRARYCOLOUR));
		}
		else
			return(SuperGallery::GetCommandState(CommandID, ShadeReason));
	}
	else if (*CommandID == SGCmd_ShowInColourLine)
	{
		// Make sure we only allow these options when the selection is a library colour
		SGDisplayColourGroup *pGroup = (SGDisplayColourGroup *) DisplayTree->FindNextSelectedGroup(NULL);
		if (pGroup == NULL && pSelectedItem != NULL)
		{
			pGroup = (SGDisplayColourGroup *) pSelectedItem->GetParent();
		}
		if (pGroup)
		{
			// We have a group item selected so use it
			// Only do this on library colour groups at present
			//SGDisplayLibColGroup * pLibGroup = (SGDisplayLibColGroup*) pGroup;
			State.Ticked = pGroup->DisplayInColourLine();
		}
		else
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_ITSNOTALIBRARYCOLOUR));
		}
	}
	else if (*CommandID == SGCmd_SetBackground)	// Set the page background colour
	{
		// Ensure we have a single selected colour and a selected document
		Document * pDoc = Document::GetSelected();
		if (pSelectedItem == NULL || pDoc == NULL)
		{
			State.Greyed = TRUE;
			ShadeReason->MakeMsg(_R(IDS_SGSHADE_SINGLECOL));
		}
	}
	else
		return(SuperGallery::GetCommandState(CommandID, ShadeReason));		// Unknown command- pass to baseclass

	return(State);
}



/********************************************************************************************

>	virtual void ColourSGallery::DoCommand(StringBase *CommandID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/95

	Inputs:		CommandID - The String ID of the command

	Purpose:	To apply a given command when it is chosen from the menu.
	
	Notes:		Override this method to provide handling for your special commands.
				Call the base class if you don't recognise the command, so that it can
				handle standard commands.

				The base class handles all of these (maybe more - see the base class help)
					Properties, Sort, Find;
					New, Edit, Delete, Redefine; (it calls ApplyAction as appropriate)
					NextGroup, PrevGroup, FoldGroup, UnfoldGroup;

********************************************************************************************/

void ColourSGallery::DoCommand(StringBase *CommandID)
{
	if (*CommandID == SGCmd_Rename)
	{
		SGDisplayColour *FirstSelected = (SGDisplayColour *)		// Invoke name-colour dialogue
							DisplayTree->FindNextSelectedItem(NULL);
		if (FirstSelected != NULL && !FirstSelected->IsALibraryColour())
		{
			IndexedColour *ColToEdit = FirstSelected->GetDisplayedColour()->
											FindParentIndexedColour();

			SGDisplayGroup *DocumentGroup = (SGDisplayGroup *) FirstSelected->GetParent();
			ERROR3IF(DocumentGroup == NULL, "SGallery DisplayTree linkage corruption detected");

			Document *ScopeDoc = DocumentGroup->GetParentDocument();
			ERROR3IF(ScopeDoc == NULL, "SGallery group is not for a document! Unimplemented! Eek!");

			ColourList *ColList = ScopeDoc->GetIndexedColours();
			ERROR3IF(ColList == NULL, "A document with no colour list? Now I've seen it all");

			if (ColToEdit != NULL)
				ColourNameDlg::InvokeDialog(ColList, ColToEdit);
		}
	}
	else if (*CommandID == SGCmd_ShowInColourLine)					// Toggle the show in colour line state
	{
		SGDisplayColour *pFirstSelected = (SGDisplayColour *)DisplayTree->FindNextSelectedItem(NULL);
		SGDisplayColourGroup *pGroup = (SGDisplayColourGroup *) DisplayTree->FindNextSelectedGroup(NULL);
		if (pGroup == NULL && pFirstSelected != NULL)
		{
			pGroup = (SGDisplayColourGroup *) pFirstSelected->GetParent();
		}
		if (pGroup)
		{
			//SGDisplayLibColGroup * pLibGroup = (SGDisplayColourGroup*) pGroup;
			// We have a group item selected so use it
			// Only do this on library colour groups at present
			pGroup->ToggleDisplayInColourLine();

			// Ensure any colours libraries that are now required are loaded
			// This should also send a message to the colour line to force an update
			CColourBar::EnsureLibraryColoursPresent();
		}
	}
	else if (*CommandID == SGCmd_SetBackground)	// Set the page background colour
	{
		ApplySelectedAsBackground();
	}
	else
		SuperGallery::DoCommand(CommandID);		// Unknown command- pass to the base class
}

/********************************************************************************************

>	BOOL ColourSGallery::ApplySelectedAsBackground()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Outputs:	-		
	Returns:	True if worked correctly, False otherwise.
	Purpose:	Takes the selected item in the gallery and tries to apply this as a new
				background to the spread.

********************************************************************************************/

BOOL ColourSGallery::ApplySelectedAsBackground()
{
	SGDisplayColour *pFirstSelected = (SGDisplayColour *)DisplayTree->FindNextSelectedItem(NULL);
	if (pFirstSelected != NULL)
	{
		// Use the first selected colour and ask a page background to be made
		// using this.
		TRACEUSER( "NEVILLE", _T("SGCmd_SetBackground"));

		Document * pScopeDoc = NULL;
		DocColour ColourToApply;
		BOOL ok = GetDocColourToApply(pFirstSelected, &ColourToApply, &pScopeDoc);

		// Only apply if we found a valid document and the colour was ok
		if (ok && pScopeDoc)
		{
			OpBackgroundParam Param;
			Param.pDocColour = &ColourToApply;
			Param.pDoc = pScopeDoc;			
			
			// Obtain a pointer to the op descriptor for the create operation 
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BACKGROUND);

			// Invoke the operation, passing DocView and Pos as parameters
			pOpDesc->Invoke(&Param);		 
		
			return TRUE;
		}
		
		return FALSE;
	}
	
	return FALSE;
}

/********************************************************************************************

>	BOOL ColourSGallery::GetDocColourToApply(SGDisplayColour *pFirstSelected, DocColour * pColourToApply,
											 Document **ppDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		pFirstSelected	selected item in the gallery
				pColourToApply	the DocColour which needs setting up
	Outputs:	ppDoc			the document which we need to apply the colour to 			
	Returns:	True if worked ok, False otherwise.
	Purpose:	To create a doc colour which can then be used to apply the colour in the
				currently selected document. Copes with the simple case of the colour being
				a document colour and the more complex case of teh colour being a library
				colour. Whereapon, a new colour must be added to the selected document which
				is this selected library colour.

********************************************************************************************/

BOOL ColourSGallery::GetDocColourToApply(SGDisplayColour *pFirstSelected, DocColour * pColourToApply,
										 Document **pDoc)
{
	ERROR2IF(pFirstSelected == NULL || pColourToApply == NULL || pDoc == NULL,FALSE,"GetDocColourToApply bad params!");

	// Try to get the parent item of the selected item and ask it for the associated document
	SGDisplayGroup * DocumentGroup = (SGDisplayGroup *) pFirstSelected->GetParent();
	ERROR3IF(DocumentGroup == NULL, "SGallery DisplayTree linkage corruption detected");

	Document * pScopeDoc = DocumentGroup->GetParentDocument();
	if (pScopeDoc == NULL)		// Must be a library item, so it will affect the selected doc
		pScopeDoc = Document::GetSelected();
	
	// Return this to the caller
	*pDoc = pScopeDoc;
	
	// If we have no document to work with then nothing doing
	if (pScopeDoc == NULL)
	{
		ERROR3("No scope document for colour gallery operation!");
		return FALSE;
	}

	DocColour * pTheColour = pFirstSelected->GetDisplayedColour();
	ERROR3IF(pTheColour == NULL, "SGDisplayColour has no colour in it?");

	IndexedColour *pSelectedColour = pTheColour->FindParentIndexedColour();

	// Check that we have a minimal ensemble of required items
	if (pTheColour != NULL && (pSelectedColour != NULL || pFirstSelected->IsALibraryColour()))
	{
		if (!pFirstSelected->IsALibraryColour())
		{
			// Normal IndexedColour item - just apply it
			pColourToApply->MakeRefToIndexedColour(pSelectedColour);

			// Ensure this colour (or an equivalent) is available in the selected doc
			// (which is where the apply will take place), and modify ColourToApply
			// (if necessary) to reference the safe colour for the dest doc.
			ColourManager::EnsureColourIsInDocument(pScopeDoc, Document::GetSelected(), pColourToApply);
		}
		else
		{
			// A library doccolour - we must copy it into the document before trying to apply it
			// Make a temporary IndexedColour from the library colour
			IndexedColour *pNewCol = new IndexedColour(*pTheColour);

			if (pNewCol != NULL)
			{
				SGDisplayLibColour *pLibCol = (SGDisplayLibColour *) pFirstSelected;

				// Set the colour's name to the same as the library item
				String_256 Buffer;
				pLibCol->GetNameText(&Buffer);
				pNewCol->SetName(Buffer);

				// If it's a spot colour, make it so
				if (pLibCol->IsASpotColour())
					pNewCol->SetLinkedParent(NULL, COLOURTYPE_SPOT);

				// Copy the colour into the destination document (merging it with existing
				// colours so we won't keep creating new copies of the same colour as it's applied)
				pColourToApply->MakeRefToIndexedColour(pNewCol);
				ColourManager::EnsureColourIsInDocument(NULL, Document::GetSelected(), pColourToApply);

				// And delete the temporary IndexedColour we used
				delete pNewCol;
				pNewCol = NULL;
			}
		}
	
		return TRUE;
	}

	// something was a bit squiffy
	return FALSE;
}

/********************************************************************************************

>	virtual void ColourSGallery::SelectionHasChanged(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96

	Purpose:	To inform the gallery that the selection has changed in some way.

				The colour gallery does the same as the base class, but also ensures
				that the selection includes some non-library items for some of the
				"editing" commands (edit, redefine, rename, delete)

********************************************************************************************/

void ColourSGallery::SelectionHasChanged(void)
{
	if (DisplayTree == NULL || AmShaded || !IsVisible())
		return;

	INT32 Count = DisplayTree->GetSelectedItemCount();

	// Determine if the entire selection consists only of library colours...
	BOOL AllSelectionIsLibrary = TRUE;
	SGDisplayColour *Col = (SGDisplayColour *) DisplayTree->FindNextSelectedItem(NULL);
	if (Col == NULL)
		AllSelectionIsLibrary = FALSE;

	while (Col != NULL)
	{
		if (!Col->IsALibraryColour())
			AllSelectionIsLibrary = FALSE;

		Col = (SGDisplayColour *) DisplayTree->FindNextSelectedItem(Col);
	}

	EnableGadget(_R(IDC_GALLERY_APPLY),		(Count == 1));	// Apply needs exactly 1 item

	// Redefine, Edit, Name, all need exactly one item, and it cannot be a library item
	EnableGadget(_R(IDC_GALLERY_REDEFINE),	(Count == 1 && !AllSelectionIsLibrary));
	EnableGadget(_R(IDC_GALLERY_EDIT),		(Count == 1 && !AllSelectionIsLibrary));
	EnableGadget(_R(IDC_GALLERY_NAME),		(Count == 1 && !AllSelectionIsLibrary));

	EnableGadget(_R(IDC_GALLERY_NEW),		(Count <= 1));	// New needs 0 or 1 items

	// Delete needs 1 or more items, and some of them must be document colours
	EnableGadget(_R(IDC_GALLERY_DELETE),	(Count > 0 && !AllSelectionIsLibrary));

	EnableGadget(_R(IDC_GALLERY_UNDO),		TRUE);			// Undo/menu always available
	EnableGadget(_R(IDC_GALLERY_MENU),		TRUE);

	Document *pDoc = Document::GetSelected();
	EnableGadget(_R(IDC_COLGAL_BACKGROUND),	Count == 1 && pDoc != NULL);
}

/***********************************************************************************************

>	void ColourSGallery::DoShadeGallery(BOOL ShadeIt)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/97
	Inputs:		ShadeIt -	TRUE if the gallery is being shaded
							FALSE if the gallery is being unshaded
	Purpose:	Called by the base class whenever the shaded status of the gallery
				is changed, to allow derived galleries to shade/unshade any extra
				controls that they provide in the window. ONLY called if the gallery
				is actually open/visible.

	Notes:		Need not be overridden - the default action is to do nothing extra
				to the normal shade/unshade operation.
	SeeAlso:	Gallery::ShadeGallery; Gallery::UnshadeGallery

***********************************************************************************************/

void ColourSGallery::DoShadeGallery(BOOL ShadeIt)
{
	// Shade any non-standard buttons that we have on the gallery
	EnableGadget(_R(IDC_COLGAL_BACKGROUND),	!ShadeIt);
}



















/********************************************************************************************

>	BOOL OpDisplayColourGallery::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDisplayColourGallery initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDisplayColourGallery::Init()
{
	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_DISPLAY_COLOUR_GALLERY),
								CC_RUNTIME_CLASS(OpDisplayColourGallery),
	 							OPTOKEN_DISPLAYCOLOURGALLERY,
	 							OpDisplayColourGallery::GetState,
	 							_R(IDST_COLGAL_OPENGALLERY),
	 							_R(IDBBL_DISPLAY_COLOUR_GALLERY),
				 				_R(IDC_BTN_SGCOLOUR), // UINT32 resourceID = 0,	// resource ID
								_R(IDC_BTN_SGCOLOUR), // UINT32 controlID = 0,	// control ID
								SYSTEMBAR_ILLEGAL,	  // SystemBarType GroupBarID = SYSTEMBAR_ILLEGAL,	// group bar ID
				 				TRUE,	  // BOOL ReceiveMessages = TRUE,	// BODGE
				 				FALSE,	  // BOOL Smart = FALSE,
				 				TRUE,	  // BOOL Clean = TRUE,   
								NULL,	  // OpDescriptor *pVertOpDesc = NULL,
								0,	  // UINT32 OneOpenInstID = 0,		
								0,	  // UINT32 AutoStateFlags = 0,
								TRUE	  // BOOL fCheckable = FALSE
								)
								);

}               
    
/********************************************************************************************

>	OpState	OpDisplayColourGallery::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDisplayColourGallery operation
	Purpose:	For finding the OpDisplayColourGallery's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDisplayColourGallery::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;  

	// If the gallery is currenty open, then the menu item should be ticked
	SuperGallery* pSuperGallery = FindGallery();
	if (pSuperGallery != NULL)
		OpSt.Ticked = pSuperGallery->IsVisible();

	// If there are no open documents, you can't toggle the gallery
	OpSt.Greyed = (Document::GetSelected() == NULL);
 	return(OpSt);   
}



/********************************************************************************************

>	void OpDisplayColourGallery::Do(OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Displays the Colours gallery
				Updates the button state for this Op (the button sticks down while the
				gallery is open)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDisplayColourGallery::Do(OpDescriptor*)
{
	SuperGallery *pSuperGallery = FindGallery();

	if (pSuperGallery != NULL)
	{
		// Toggle the visible state of the gallery window
		pSuperGallery->SetVisibility( !pSuperGallery->IsVisible() );

		// And update the gallery button state
		SGInit::UpdateGalleryButton(_R(OPTOKEN_DISPLAYCOLOURGALLERY), pSuperGallery->IsVisible());
	}
	
	End();
}



/********************************************************************************************

>	static SuperGallery *OpDisplayColourGallery::FindGallery(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (base generated in sgbase.cpp)

	Returns:	NULL or a pointer to the Colour gallery instance

	Purpose:	Finds the Colour gallery class instance

	Notes:		The bars system always keeps one Colour gallery alive for us.
				If one is not found, this usually indicates that it can't be found
				in bars.ini: Check that the 'Name' string *exactly* matches the
				title string given in bars.ini.
				Also check that bars.ini indicates the bar is of the ColourSGallery class

********************************************************************************************/

SuperGallery *OpDisplayColourGallery::FindGallery(void)
{
	SuperGallery* pSuperGallery = SuperGallery::FindSuperGallery(_R(IDD_COLOURSGALLERY));

	if (!pSuperGallery) pSuperGallery = new ColourSGallery;

	if (pSuperGallery != NULL)
	{
		if (pSuperGallery->GetRuntimeClass() == CC_RUNTIME_CLASS(ColourSGallery))
			return(pSuperGallery);

		ERROR3("Got the Colour gallery but it's not of the ColourSGallery class");
	}
	else
	{
		ERROR3("Can't find the Colour gallery in bars.ini!\n");
	}

	return(NULL);
}











const INT32 ColourNameDlg::IDD = _R(IDD_COLOURNAMEDLG);
const CDlgMode ColourNameDlg::Mode = MODAL;

/********************************************************************************************

>	ColourNameDlg::ColourNameDlg(): DialogOp(ColourNameDlg::IDD, ColourNameDlg::Mode) 


	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	ColourNameDlg constructor.

********************************************************************************************/

ColourNameDlg::ColourNameDlg() : DialogOp(ColourNameDlg::IDD, ColourNameDlg::Mode) 
{
}        



/********************************************************************************************

>	MsgResult ColourNameDlg::Message( CDlgMessage DlgMsg, CGadgetID Gadget)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the scale dialog's messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MsgResult ColourNameDlg::Message(Msg* Message)
{
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = (DialogMsg*)Message;
//		BOOL EndDialog = FALSE;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				{
					String_256 Name;

					if (Info->TheName != NULL)			// Must be creating new colour
					{
						Name.MakeMsg(_R(IDS_COLNAME_CREATE));
						SetStringGadgetValue(_R(IDOK), _R(IDS_COLNAME_YCREATE));
						SetStringGadgetValue(_R(IDC_COLNAME_TEXT), *Info->TheName);
					}
					else if (Info->TheColour != NULL)	// Are renaming existing colour
					{
						Name.MakeMsg(_R(IDS_COLNAME_ALTER));
						SetStringGadgetValue(_R(IDOK), _R(IDS_COLNAME_YALTER));
						SetStringGadgetValue(_R(IDC_COLNAME_TEXT), *Info->TheColour->GetName());
					}

					SetTitlebarName(&Name);
					SetKeyboardFocus (_R(IDC_COLNAME_TEXT));
					HighlightText    (_R(IDC_COLNAME_TEXT));
				}
				break;
				
			case DIM_COMMIT:
			case DIM_SOFT_COMMIT:
				CommitName();
				break;

			case DIM_CANCEL:
				Close();
				End();
				break;

			case DIM_REDRAW:
				if (Msg->GadgetID == _R(IDC_COLNAME_PATCH))
				{
					ReDrawInfoType *RedrawInfo = (ReDrawInfoType*) Msg->DlgMsgParam;
					DocRect VirtualSize(0, 0, RedrawInfo->dx, RedrawInfo->dy);
					RenderRegion *pRender = CreateOSRenderRegion(&VirtualSize, RedrawInfo);
					if (pRender)
					{
						pRender->SaveContext();

						DocColour PatchCol(128L, 128L, 128L);
						if (Info->TheColour != NULL)
							PatchCol.MakeRefToIndexedColour(Info->TheColour);

						pRender->SetLineWidth(0);
						pRender->SetLineColour(PatchCol);
						pRender->SetFillColour(PatchCol);
						pRender->DrawRect(&VirtualSize);

						pRender->RestoreContext();

						DestroyOSRenderRegion(pRender);
					}
				}
				break;

			default:
				break;
		}

		return (DLG_EAT_IF_HUNGRY(Msg)); 
	}

	return OK; 
}  



/********************************************************************************************

>	void ColourNameDlg::CommitName(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95

	Purpose:	Commits to the current setting for the new name
				Reads the name into the supplied destination, sets the return value to
				TRUE and closes and ends the Dlg Op.

	Notes:		If the result is placed into TheColour, then a ColourChangingMsg broadcast
				will be sent. Thus, if a caller wants only to rename a colour, they need
				only call up the dialogue upon that colour - set and forget.

********************************************************************************************/

void ColourNameDlg::CommitName(void)
{
	Info->Result = TRUE;

	String_256 NewName = GetStringGadgetValue(_R(IDC_COLNAME_TEXT));

	if (NewName.Length() < 1)	// User gave NULL name. Use 'Unnamed' instead.
		NewName.MakeMsg(_R(IDS_UNNAMEDCOLOUR));

	if (Info->TheList != NULL && Info->TheColour != NULL)
	{
		// We must set the new name for the colour - first make sure it's unique
		String_64 UniqueName;
		Info->TheList->GenerateUniqueColourName((const StringBase *) &NewName, &UniqueName);

		IndexedColour *NewCol = new IndexedColour(*Info->TheColour);
		if (NewCol == NULL)
			InformError();
		else
		{
			NewCol->SetName(UniqueName);
			ColourManager::ChangeColour(Info->TheList, NewCol, Info->TheColour, TRUE);
		}
	}
	else if (Info->TheName != NULL)
	{
		// We just copy the string into the result string, truncating at 63 chars
		NewName.Left(Info->TheName, 63);
	}

	Close();
	End();
}



/********************************************************************************************

>	OpState	ColourNameDlg::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Purpose:	ColourNameDlg GetState method

********************************************************************************************/

OpState	ColourNameDlg::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}


		 
/********************************************************************************************

>	BOOL ColourNameDlg::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Purpose:	ColourNameDlg Init method. Called by sginit.cpp

********************************************************************************************/

BOOL ColourNameDlg::Init(void)
{  
	return (RegisterOpDescriptor(
								0,
								_R(IDS_COLOURNAMEDLG),
								CC_RUNTIME_CLASS(ColourNameDlg),
								OPTOKEN_COLOURNAMEDLG,
								ColourNameDlg::GetState,
								0,	// help ID
								0   // bubble ID
								)
			); 
}   
 


/********************************************************************************************

>	BOOL ColourNameDlg::Create()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Returns:	TRUE if successful, else FALSE
	Purpose:	ColourNameDlg create method 

********************************************************************************************/

BOOL ColourNameDlg::Create()
{
	return(DialogOp::Create());
}



/********************************************************************************************

>	void ColourNameDlg::DoWithParam(OpDescriptor*,OpParam* pOpParam)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95
	Inputs:		pOpParam = ptr to a ColourNameParam struct

	Purpose:	Creates then opens the dialog 

********************************************************************************************/


void ColourNameDlg::DoWithParam(OpDescriptor*, OpParam* pOpParam)
{
	ERROR3IF(pOpParam == NULL, "Come on, play by the rules");

	Info = (ColourNameParam *) pOpParam;

	if (Info->TheColour == NULL && Info->TheName == NULL)
	{
		ERROR3("ColourNameDlg parameters are unhelpful to say the least");
		End();
	}

	if (!Create())
	{
		InformError();
  		End();
	}
}



/********************************************************************************************

>	static BOOL ColourNameDlg::InvokeDialog(ColourList *ParentList,
											IndexedColour *ColourToName)

	vAuthor:	Jason
	Created:	1/4/95

	Returns:	TRUE if the OK button was clicked, FALSE if cancel was clicked

	Inputs:		ParentList	 - the list in which the colour resides (not NULL)
				ColourToName - the colour whose name should be changed (not NULL)

	Purpose:	Opens the gallery name dialogue on screen, elicits a response, 
				and returns having set the new name. This is a MODAL dlg.

				This variant renames an existing colour. The window indicates
				that renaming is going to occur, and has 'Apply' and 'Cancel'
				buttons.

				If the name is changed, the colour name will be set (to a name
				guaranteed to be unique within the colour list), and an appropriate
				ColourChangingMsg willbe broadcast (i.e. it's a set-and-forget mode)

********************************************************************************************/

BOOL ColourNameDlg::InvokeDialog(ColourList *ParentList, IndexedColour *ColourToName)
{
	ERROR3IF(ParentList == NULL || ColourToName == NULL, "Come on, play by the rules");

	ColourNameParam Info(ParentList, ColourToName);
	OpDescriptor *NameDlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(ColourNameDlg));

	ERROR3IF(NameDlg == NULL,
			"ColourNameDlg::InvokeDialog is unable to find the Dlg OpDescriptor");

	if (NameDlg != NULL)
		NameDlg->Invoke(&Info);

	return(Info.Result);
}



/********************************************************************************************

>	static BOOL ColourNameDlg::InvokeDialog(String_64 *NameToFill,
											IndexedColour *DisplayColour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/95

	Returns:	TRUE if the OK button was clicked, FALSE if cancel was clicked

	Inputs:		NameToFill - A String_64 containing the default name. This will be
				changed to the new nbame to use.

				DisplayColour - A colour to be displayed in the dialogue. This is just
				for the user to look at and say "oooh, pretty!"

	Purpose:	Opens the gallery name dialogue on screen, elicits a response, 
				and returns having set the new name. This is a MODAL Dlg.

				This variant gets the name of a new colour. The window indicates
				that a new colour is being created, and has 'Create' and 'Cancel'
				buttons. Only if it returns TRUE should you create the colour.

				Note that this variant is not 'set and forget'. You have to do
				something with the returned name.

	Notes:		DO NOT use this method with the result of a call to the
				IndexedColour::GetName() function! See the other variant to do this.

				If you SetName() a colour already in a colour list, (in which case
				you should be using the other variant of this method) then you MUST
				ensure that the name you set is guaranteed to be unique within the
				list (or export/import will fail).
	
********************************************************************************************/

BOOL ColourNameDlg::InvokeDialog(String_64 *NameToFill, IndexedColour *DisplayColour)
{
	ERROR3IF(NameToFill == NULL, "Come on, play by the rules");

	ColourNameParam Info(NameToFill, DisplayColour);
	OpDescriptor *NameDlg = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(ColourNameDlg));

	ERROR3IF(NameDlg == NULL,
			"ColourNameDlg::InvokeDialog is unable to find the Dlg OpDescriptor");

	if (NameDlg != NULL)
		NameDlg->Invoke(&Info);

	return(Info.Result);
}

