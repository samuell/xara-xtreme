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
// DragCol.cpp - Colour DragInformation base class

/*
*/


#include "camtypes.h"

#include "app.h"
#include "ccolbar.h"
#include "coldlog.h"
#include "colourix.h"
#include "colormgr.h"
#include "devcolor.h"
#include "docview.h"
#include "dragcol.h"
#include "dragmgr.h"
#include "keypress.h"
#include "camframe.h"
//#include "resource.h"
#include "sgcolour.h"
#include "camview.h"
#include "attrmgr.h"
#include "fillattr.h"
#include "ink.h"
#include "lineattr.h"

#include "palman.h"

#include "backgrnd.h"	// OpBackground
#include "keypress.h"	// KeyPress
//#include "galstr.h"		// _R(IDS_DRAGCOL_DROP_SETPAGEBACKGROUND)

CC_IMPLEMENT_DYNCREATE(ColourDragInformation, DragInformation)



const INT32 CellSize = 16;		// X/Y size of the solid-dragged colour square



// Default to non-transparent dragging
INT32 ColourDragInformation::DragTransparency = 0;



/********************************************************************************************

>	void ColourDragInformation::InitObject(void)
	 
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Purpose:	Shared code for initialising the object - called by all constructors

********************************************************************************************/

void ColourDragInformation::InitObject(void)
{
	// Init member variables
	TheColour = DocColour(COLOUR_TRANS);
	pParentDoc = NULL;
	ColourName = TEXT("");
	LibColIsSpot = FALSE;

	// Set up a few things about this drag
	DoesSolidDrag = TRUE;
	
	// offset of colour swatch from the pointer
	SolidDragOffset.x = -14;
	SolidDragOffset.y = 4;	   
	
	// size of the solid drag redraw
	SolidDragSize.x = CellSize;
	SolidDragSize.y = CellSize;
}



/********************************************************************************************

>	void ColourDragInformation::ColourDragInformation() 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  

	Purpose:	Conmstructor - do not call this default constructor.

********************************************************************************************/

ColourDragInformation::ColourDragInformation()
{
	ERROR3("Default ColourDragInformation constructor called");	
	InitObject();
}



/********************************************************************************************

>	void ColourDragInformation::ColourDragInformation(IndexedColour *Colour, BOOL IsAdjust,
							 							Document* pParent) 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	
	Inputs:		Colour		- The IndexedColour being dragged
							  May not be NULL

				IsAdjust	- TRUE if this is an adjust-drag, FALSE if a select-drag

				pParent		- Points to the parent document for the dragged colour.

	Purpose:	Constructor
				Use this for "docuemnt colours" i.e. IndexedColours which already exist
				inside a document. See the other constructor for library colours

********************************************************************************************/

ColourDragInformation::ColourDragInformation(IndexedColour *Colour,BOOL IsAdjust,
												Document* pParent)
						: DragInformation(IsAdjust)
{
	ERROR3IF(pParent == NULL, "Illegal NULL param");

	InitObject();

	// The IndexedColour we are dragging - if it's NULL, we're dragging "no colour"
	if (Colour != NULL)
		TheColour.MakeRefToIndexedColour(Colour);

	// The Parent document of the colour we are dragging
	pParentDoc = pParent;
}



/********************************************************************************************

>	ColourDragInformation::ColourDragInformation(DocColour *Colour, BOOL IsAdjust,
													StringBase *pColourName, BOOL IsSpot)
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	
	Inputs:		Colour		- The Colour being dragged
							  May not be NULL

				IsAdjust	- TRUE if this is an adjust-drag, FALSE if a select-drag

				pColourName - Should not be NULL (but it won't ERROR3, because
								of GalleryColourDragInfo constructor technical reasons)
								The name of the colour being dragged

				IsSpot		- TRUE if this library colour is a spot colour

	Purpose:	Constructor
				Use this for "library colours" i.e. Colours which DON'T already exist
				inside a document. See the other constructor for document colours.

				When the drag finishes, the user will ask this object for a colour,
				at which point, an IndexedColour will be created for them from the
				given DocColour and Name string. See GetLibraryColourForDocument()

********************************************************************************************/

ColourDragInformation::ColourDragInformation(DocColour *Colour, BOOL IsAdjust,
												StringBase *pColourName, BOOL IsSpot)
						: DragInformation(IsAdjust)
{
	ERROR3IF(Colour == NULL, "Illegal NULL param");

	InitObject();

	TheColour = *Colour;
	LibColIsSpot = IsSpot;
	
	if (pColourName != NULL)
		ColourName = *pColourName;
}



/********************************************************************************************

>	static BOOL ColourDragInformation::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Returns:	-
	Purpose:	Initialise any Colour Drag stuff. (eg. ini file preferences).
	SeeAlso:	-

********************************************************************************************/

BOOL ColourDragInformation::Init()
{
	// Setup preference for drag transparency
	if (GetApplication()->DeclareSection(_T("Dragging"), 1))
		GetApplication()->DeclarePref( NULL, _T("ColourDragTransparency"), &DragTransparency, 0, 100);

	return TRUE;
}



/********************************************************************************************

>	BOOL ColourDragInformation::IsLibraryColour(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Returns:	TRUE if this is a "library colour" (has no parent document), or
				FALSE if the dragged colour resides in a document

	Purpose:	To determine if the dragged colour is from a library or a document

	SeeAlso:	ColourDragInformation::GetParentDoc

********************************************************************************************/

BOOL ColourDragInformation::IsLibraryColour(void)
{
	return(GetParentDoc() == NULL);
}



/********************************************************************************************

>	Document *ColourDragInformation::GetParentDoc(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Returns:	NULL (if this is a "library colour" (has no parent document)), or a pointer
				to the Source Document from which the colour has been dragged.

	Purpose:	To determine the document (if any) from which a colour is being dragged

	SeeAlso:	ColourDragInformation::IsLibraryColour

********************************************************************************************/

Document *ColourDragInformation::GetParentDoc(void)
{
	return(pParentDoc);
}



/********************************************************************************************

>	IndexedColour *ColourDragInformation::GetColourForDocument(Document *pDestDoc = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96

	Inputs:		pDestDoc -	NULL, to copy the colour into the Selected document, or
						a pointer to the document to copy the colour into

	Returns:	NULL if the colour is transparent (no colour), or
				A pointer to a safe colour to be used in the given document

	Purpose:	To copy/merge the dragged colour into the given document. The returned
				pointer is a colour which is contained in the given document, and
				can be safely used within that document.

	SeeAlso:	ColourDragInformation::IsLibraryColour

********************************************************************************************/

IndexedColour *ColourDragInformation::GetColourForDocument(Document *pDestDoc)
{
	// If no document supplied, assume the selected docuemnt is the destination
	if (pDestDoc == NULL)
		pDestDoc = Document::GetSelected();

	// If there is no destination document, or if the colour is transparent, return NULL
	if (pDestDoc == NULL || TheColour.IsTransparent())
		return(NULL);

	// If it's a document colour and already in the destination document, return it
	if (pParentDoc == pDestDoc)
		return(TheColour.FindParentIndexedColour());

	// Copy the dragged colour into a temporary DocColour
	DocColour Temp = TheColour;
	IndexedColour *TempIxCol = NULL;

	// Check if we've got an IndexedColour to use...
	if (Temp.FindParentIndexedColour() == NULL)
	{
		// It's a library colour, so we must create a new IndexedColour to use
		TempIxCol = new IndexedColour(Temp);
		if (TempIxCol == NULL)
			return(NULL);			// Failure - return "no colour"

		TempIxCol->SetName(ColourName);

		if (LibColIsSpot)			// If a spot colour, make it so
			TempIxCol->SetLinkedParent(TempIxCol->FindLastLinkedParent(), COLOURTYPE_SPOT);

		// And make the DocColour reference it
		Temp.MakeRefToIndexedColour(TempIxCol);
	}

	// Ensure the colour (or a safe equivalent) is in the dest doc, by copying/merging it
	ColourManager::EnsureColourIsInDocument(pParentDoc, pDestDoc, &Temp);

	// And delete any temporary IndexedColour we created
	if (TempIxCol != NULL)
		delete TempIxCol;

	// And finally, return the resulting colour
	return(Temp.FindParentIndexedColour());
}



/********************************************************************************************

>	virtual UINT32 ColourDragInformation::ApplyColourToSelection()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95

	Purpose:	Applies the dragged colour to the current selection
				(including copying/merging the colour into the destination document
				if necessary)

	Notes:		Assumes that the selection will be in the Selected Document!
   
********************************************************************************************/

void ColourDragInformation::ApplyColourToSelection()
{
// WEBSTER-ranbirr-12/11/96 No colour bar stuff in webster
// Neville 23/5/97 Now has a Browser palette in the colour line
	IndexedColour *ColToUse = GetColourForDocument(NULL);

	// ColourToApply now references a safe colour to use, or NULL (in which case it is
	// used as 'no colour' by the colour bar, which is exactly what we wanted. Fabby)
	CColourBar::TheColourBar->ApplyColour(ColToUse, IsAdjustDrag);
}



/********************************************************************************************

>	void ColourDragInformation::GetCursorID(DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Purpose:	Set cursor over this target


********************************************************************************************/

UINT32 ColourDragInformation::GetCursorID(DragTarget* pDragTarget)
{
	if (pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

//		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
		BOOL IsInside = ClickMods.Constrain;

//	 	Removed by Will 23/1/96
//  	This code was needed originally, but is no longer needed as the
//		colour drop testing code is much smarter and copes with the case
//		that this code is testing for.

//		if (!IsInside && pObjectHit && pObjectHit->IsCompound())
//		{
//			TargetHit = MANY_TARGET;
//		}

		switch (TargetHit)
		{
			case FILL_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONFILL) : _R(IDC_CANDROPONFILL);
			case LINE_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONLINE) : _R(IDC_CANDROPONLINE);
			case STARTCOL_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONFILLSTART) : _R(IDC_CANDROPONFILLSTART);
			case ENDCOL_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONFILLEND) : _R(IDC_CANDROPONFILLEND);
			case ENDCOL2_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONFILLEND2) : _R(IDC_CANDROPONFILLEND2);
			case ENDCOL3_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONFILLEND3) : _R(IDC_CANDROPONFILLEND3);
			case MANY_TARGET:
				return IsInside ? _R(IDC_DROPINSIDEONFILL) : _R(IDC_CANDROPONFILL);

			case NO_TARGET:
				// colour drag - drop = page fill, CTRL + drop = set current fill
				return IsInside ? _R(IDC_CANFILLPAGE) : _R(IDC_CANDROPONPAGE);
				//return _R(IDC_CANDROPONPAGE);
		};

		return _R(IDC_CANDROPONPAGE);
	}

	return 0;
}



/********************************************************************************************

>	virtual BOOL ColourDragInformation::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95
	Returns:	Whether String is valid
	Purpose:	provide status line text for this target
   
********************************************************************************************/

BOOL ColourDragInformation::GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");

	String_256 DragString(_R(IDS_DRAGCOL_DRAGGING_COLOUR)); // "Dragging colour '");

	if (TheColour.IsTransparent())
		DragString = _R(IDS_DRAGCOL_DRAGGING_NO_COLOUR);	// "Dragging 'no colour' : ";
	else
	{
		IndexedColour *TheIxCol = GetInitiallyDraggedColour();
		if (TheIxCol != NULL)
			DragString += *(TheIxCol->GetName());		// Document IndexedColour
		else
			DragString += ColourName;					// Library colour

		DragString += String_16(_R(IDS_DRAGCOL_DRAGGING_COLOUR_RIGHT_BIT)); // "' : ";
	}

	if (pDragTarget->IS_KIND_OF(ViewDragTarget))
	{
		PageDropInfo PageDropInfo;
		((ViewDragTarget*)pDragTarget)->GetDropInfo(&PageDropInfo);

		NodeRenderableInk* pObjectHit 	= PageDropInfo.pObjectHit;
		ObjectDragTarget TargetHit 		= PageDropInfo.TargetHit;

		String_256 ObjectDesc = _R(IDS_DRAGCOL_THIS_OBJECT); //" this object";

		if (pObjectHit)
		{
			ObjectDesc = pObjectHit->Describe(FALSE);
		}

		ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
		BOOL IsInside = ClickMods.Constrain;

		if (!IsInside && pObjectHit && pObjectHit->IsCompound())
		{
			TargetHit = MANY_TARGET;
		}

		switch (TargetHit)
		{
			case FILL_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_FILL)); // "Drop to set the fill colour of this ";
				DragString += ObjectDesc;
				if (IsInside)
					DragString += String_64(_R(IDS_DRAGCOL_ALONE)); // " alone";
				break;
			case LINE_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_LINE)); //"Drop to set the line colour of this ";
				DragString += ObjectDesc;
				if (IsInside)
					DragString += String_64(_R(IDS_DRAGCOL_ALONE)); // " alone";
				break;
			case STARTCOL_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_FILL_START)); //"Drop to set the fill start colour of this ";
				DragString += ObjectDesc;
				break;
			case ENDCOL_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_FILL_END)); //"Drop to set the fill end colour of this ";
				DragString += ObjectDesc;
				break;
			case ENDCOL2_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_FILL_END2)); //"Drop to set the fill end colour of this ";
				DragString += ObjectDesc;
				break;
			case ENDCOL3_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_FILL_END3)); //"Drop to set the fill end colour of this ";
				DragString += ObjectDesc;
				break;
			case MANY_TARGET:
				DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_FILL)); //"Drop to set the fill colour of this ";
				DragString += ObjectDesc;
				DragString += String_64(_R(IDS_DRAGCOL_PRESS_CONTROL)); // "; Press 'Control' to drop 'Inside'";
				break;

			case NO_TARGET:
				if (IsInside)
				{
					DragString += String_256(_R(IDS_DRAGCOL_DROP_SETPAGEBACKGROUND)); // "Drop to set the fill colour of the page background";
				}
				else
				{
					DragString += String_64(_R(IDS_DRAGCOL_DROP_SET_CURRENT_FILL_COLOUR)); // "Drop to set the 'current fill colour' attribute";
				}
				break;
		};

	 	*TheText = DragString;
		return TRUE;
	}
 	
	return FALSE;
}


/********************************************************************************************

>	virtual UINT32 ColourDragInformation::GetCursorID()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95
	Returns:	a cursor ID to set during this drag
	Purpose:	Determines what cursor to display during a colour drag
   
********************************************************************************************/

UINT32 ColourDragInformation::GetCursorID()
{
	return(_R(IDC_DRAGGING_COLOUR));
}



/********************************************************************************************

>	virtual BOOL ColourDragInformation::GetStatusLineText(String_256 * TheText)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/95

	Outputs:	TheText - If it returns TRUE, this is filled in with status line text
	Returns:	Whether String is valid

	Purpose:	provide status line text for this drag
   
********************************************************************************************/

BOOL ColourDragInformation::GetStatusLineText(String_256 *TheText)
{
	ERROR2IF(TheText==NULL,FALSE,"NULL string in GetStatusLineText()");
 	String_32 LineString(_R(IDS_DRAGCOL_DRAGGING_LINE_COLOUR)); // "Dragging line colour : ");
	String_32 FillString(_R(IDS_DRAGCOL_DRAGGING_FILL_COLOUR)); // "Dragging fill colour : ");
	String_256 DragString;

	if(IsAdjustDrag)
		DragString = LineString;
	else
		DragString = FillString;

	if (TheColour.IsTransparent())
		DragString = _R(IDS_DRAGCOL_DRAGGING_NO_COLOUR);	//"Dragging 'no colour'";
	else
	{
		IndexedColour *TheIxCol = GetInitiallyDraggedColour();
		if (TheIxCol != NULL)
			DragString += *(TheIxCol->GetName());		// Document IndexedColour
		else
			DragString += ColourName;					// Library colour
	}

 	*TheText = DragString;

	return TRUE;
}



/********************************************************************************************

>	void ColourDragInformation::OnClick(INT32 Flags,POINT Point) 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		

	Purpose:	This is called if a drag was attempted but never started because it was a 
				click all along. In the base class, the default action is to make the
				colour bar think that the dragged colour has been clicked.

********************************************************************************************/

void ColourDragInformation::OnClick(INT32 Flags, CNativePoint Point)
{
//	WEBSTER-ranbirr-27/03/97 No colour bar stuff in webster
// Neville 23/5/97 Now has a Browser palette in the colour line

	if (KeyPress::IsAdjustPressed())
		Flags = -1;

	wxRect RectMouseIsIn;
	Point = CColourBar::TheColourBar->ScreenToClient(Point);
	UINT32 MousePos = CColourBar::TheColourBar->WhereIsMouse(Point, &RectMouseIsIn);

 	CColourBar::TheColourBar->CellClicked(MousePos, (Flags < 0));
}



/********************************************************************************************

>	void ColourDragInformation::OnDrawSolidDrag(CRect DragRect,CDC * TheDC) 
	 
	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95		  
	Inputs:		TheDC - pointer to a DC ready to draw into.
	Outputs:	-
	Returns:	-
	Purpose:	allows the drag to draw into a Device Context - DragManager will look after
				painting the background	etc.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ColourDragInformation::OnDrawSolidDrag(wxPoint Origin, wxDC *TheDC)
{
	// set up gbrush
//	CPalette * OldPal = NULL;
//	if (PaletteManager::UsePalette())
//	{
//		OldPal = PaletteManager::StartPaintPalette( TheDC );
//	}

	GBrush GDrawBrush;
	GDrawBrush.Init(TheDC);
	GDrawBrush.Start();

	// get a gbrush brush
	Document *ScopeDoc = Document::GetSelected();
	ERROR2IF(ScopeDoc == NULL, FALSE, "No Selected Doc !");

	wxBrush* pDragBrush = new wxBrush;
	
	BOOL BrushCreate = TRUE;

	if (pDragBrush != NULL )
	{
		if (TheColour.IsTransparent())
		{
//			TheDC->SetBkColor(RGB(255,255,255));		// ensure white background
			pDragBrush->SetStyle(wxCROSSDIAG_HATCH);
			pDragBrush->SetColour(wxColour(0,0,0));		// Fill with black/white cross hatch
		}
		else
		{
			DWORD ScreenWord;

			ColourContextRGBT *CCrgbt = (ColourContextRGBT *)
						ColourManager::GetColourContext(COLOURMODEL_RGBT, DocView::GetSelected());
			ScreenWord = ConvertColourToScreenWord(CCrgbt, &TheColour);

			// create the Brush and	Pen
			if (GDrawBrush.Available())
			{
				GDrawBrush.GetLogBrush((COLORREF)ScreenWord, pDragBrush);
			}
			else
				pDragBrush->SetColour((COLORREF)ScreenWord);
		}

		wxPen MyPen(wxColour(0,0,0));

		// Set the brush origin so the dither (or Hatch) patterns
		// don't move around as you drag.
//		TheDC->SetBrushOrg(Origin.x, Origin.y);
		
		//the rectangle to draw into
		wxRect DrawRect(Origin.x, Origin.y, SolidDragSize.x-1, SolidDragSize.y-1);

		// select brushes and pens ..
		TheDC->SetBrush(*pDragBrush);
		TheDC->SetPen(MyPen);

		// draw the thingy. In this case, it's a colour patch, circular for a spot colour, else a square
#ifndef WEBSTER // don't distinguish spots in Webster Martin 15/07/97
		if (!TheColour.IsTransparent() && TheColour.GetSpotParent() != NULL)
 			TheDC->DrawEllipse(DrawRect);
 		else
#endif // WEBSTER
 			TheDC->DrawRectangle(DrawRect);
		    						
    	// clean up the dc
    	TheDC->SetBrush(wxNullBrush);
		TheDC->SetPen(wxNullPen);

    	// delete the objects
		delete pDragBrush;
	}
	else
		BrushCreate = FALSE;
	
	// Finish with GBrush
	GDrawBrush.Stop();
//	if (OldPal)
//		PaletteManager::StopPaintPalette(TheDC, OldPal);
	
	// problems !!!
	ERROR2IF(!BrushCreate, FALSE, "Failed to make a solid drag brush!");

	return TRUE;
}



/********************************************************************************************

>	virtual INT32 ColourDragInformation::GetDragTransparency()
 
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Returns:	-
	Purpose:	Specifies how transparent a drag should be.
				A value of 0, will cause a normal solid blit.
				Any other value (between 0 and 100), will cause a masked blit.
	SeeAlso:	-

********************************************************************************************/

INT32 ColourDragInformation::GetDragTransparency()
{
	return ColourDragInformation::DragTransparency;
}



/********************************************************************************************

>	virtual BOOL ColourDragInformation::OnPageDrop(ViewDragTarget* pDragTarget)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95		  
	Returns:	TRUE if it succeeds, FALSE if there is an error

	Purpose:	Called when a colour is dropped on an Object.

	Notes:		This routine assumes that the destination document is the SELECTED doc

********************************************************************************************/

BOOL ColourDragInformation::OnPageDrop(ViewDragTarget* pDragTarget)
{
	PageDropInfo ThePageDropInfo;
	((ViewDragTarget*)pDragTarget)->GetDropInfo(&ThePageDropInfo);

	DocCoord DragPos 			= ThePageDropInfo.DropPos;
	ObjectDragTarget TargetType = ThePageDropInfo.TargetHit;
	NodeRenderableInk* ThisNode = ThePageDropInfo.pObjectHit;

	// DMc 4/10/99
	// test for dragging & dropping onto a bevel/shadow/other compound node which
	// is grouped
	if (ThisNode)
	{
		if (ThisNode->FindParent())
		{
			if (!ThisNode->FindParent()->PromoteHitTestOnChildrenToMe())
			{
				// any compounds with their 'promotehittest' stuff set above me ?
				Node * pNode = ThisNode->FindParent();
				
				while (pNode)
				{
					if (pNode->IsAnObject())
					{
						if (pNode->PromoteHitTestOnChildrenToMe())
						{
							ThisNode = (NodeRenderableInk *)pNode;
						}
					}
					
					pNode = pNode->FindParent();
				}
			}
		}
	}

	NodeAttribute *Attrib = NULL;
	
	// Get a colour to apply to the Selected doc
	IndexedColour *IxColToApply = GetColourForDocument(NULL);

	// And make a DocColour to apply
	DocColour ColourToApply(COLOUR_TRANS);
	if (IxColToApply != NULL)
		ColourToApply.MakeRefToIndexedColour(IxColToApply);


 	if (TargetType == LINE_TARGET) // apply a stroke colour
	{
		// Apply a stroke colour to the object(s) we have been dropped onto
		Attrib = new AttrStrokeColourChange;
		if (Attrib == NULL)
			return FALSE;

		((AttrStrokeColourChange *)Attrib)->SetStartColour(&ColourToApply);
		AttributeManager::ApplyAttribToNode(ThisNode, Attrib);
	}
	else		  
	{
		// Apply a fill colour to the object(s) we have been dropped onto
		// This always used to be the case. If there were no objects then we
		// just set the current fill colour to be the new one.
		// Now, if no objects are the target then apply a page background colour
		// otherwise if control is held down set the current fill colour.
		
		BOOL Constrain = FALSE;

		// get object bounds
		DocRect pSimpleBounds;
		if (ThisNode)
			pSimpleBounds = ThisNode->GetBoundingRect();
		else
		{
			pSimpleBounds.MakeEmpty();
			// See if the control key is pressed
			Constrain = KeyPress::IsConstrainPressed();
		}

		// No node(s) are targetted so see if the constrain key is pressed or not
		if (ThisNode == NULL && Constrain)
		{
			// Use the colour to set the background
			// We should use the document given to us by the page info class
			OpBackgroundParam Param;
			Param.pDocColour = &ColourToApply;
			Param.pDoc = ThePageDropInfo.pDoc;			
			Param.pSpread = ThePageDropInfo.pSpread;
			
			// Obtain a pointer to the op descriptor for the create operation 
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_BACKGROUND);

			// Invoke the operation, passing in our parameters
			pOpDesc->Invoke(&Param);
		}
		else
		{
			// Apply the colour either to the targetted node(s) or as the current
			// fill colour, if no nodes are targetted.
			Attrib = new AttrColourDrop(DragPos, pSimpleBounds, ColourToApply);
			if (Attrib == NULL)
				return FALSE;

			if (ThisNode)
				((AttrColourDrop*)Attrib)->SetObjectDroppedOn(ThisNode);

			AttributeManager::ApplyAttribToNode(ThisNode, Attrib);
		}
	}

	return TRUE;
}

