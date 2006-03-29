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
//
// Implementation of the view document component classes

/*
*/

#include "camtypes.h"
#include "viewcomp.h"
#include "app.h"
//#include "webparam.h"
//#include "xarprefs.h"
//#include "webprefs.h"
#include "bitfilt.h"		// For standard bounds calc routines.
#include "quality.h"
#include "docview.h"
#include "spread.h"
#include "page.h"

#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"

#include "camfiltr.h"
#include "rechview.h"

#include "zoomops.h"
//#include "ralphvw.h"

CC_IMPLEMENT_DYNAMIC(ViewComponentClass,DocComponentClass)
CC_IMPLEMENT_DYNAMIC(ViewComponent,		DocComponent)
CC_IMPLEMENT_DYNAMIC(ViewRecordHandler,CamelotRecordHandler)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// This is the default view quality for the view records in a v2 file
#define DEFAULT_VIEW_QUALITY 110

//---------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL ViewComponentClass::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the view document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL ViewComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	ViewComponentClass *pClass = new ViewComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL ViewComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pDocument - the document to add the component to.
	Returns:	TRUE if the print was added ok;
				FALSE if not.
	Purpose:	Add a view component to the specified document.
	Errors:		Out of memory
	SeeAlso:	ViewComponentClass

********************************************************************************************/

BOOL ViewComponentClass::AddComponent(BaseDocument *pDocument)
{
	// Check to see if this document already has a view component; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(ViewComponent)) != NULL)
		return TRUE;

	// No view component - try to create a new one for this document.

	// create the view component
	ViewComponent *pComponent = new ViewComponent;

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);

	return TRUE;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//----------------------------

/********************************************************************************************

>	ViewComponent::ViewComponent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Purpose:	Construct a view component.
				Does nothing at the mo
	SeeAlso:	ViewComponent

********************************************************************************************/

ViewComponent::ViewComponent()
{
}

/********************************************************************************************

>	ViewComponent::~ViewComponent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Purpose:	The destructor - does nowt at the mo
	Errors:		-
	SeeAlso:	ViewComponent

********************************************************************************************/

ViewComponent::~ViewComponent()
{
}



/********************************************************************************************

>	virtual ViewComponent::StartImport(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pFilter = ptr to the filter to import from
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Gives the component a chance to prepare for import.
				Does nothing at the mo
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::StartImport(BaseCamelotFilter *pFilter)
{
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL ViewComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to import from
				Success = TRUE if the import was successful, FALSE otherwise
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Gives the component a chance to clean up after import.
				Does nothing at the mo
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::EndImport(BaseCamelotFilter *pFilter, BOOL Success)
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL ViewComponent::StartExport(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Gives the component a chance to export info before the main document tree
				Does nothing at the mo
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::StartExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT

	ERROR2IF(pFilter == NULL,FALSE,"Null filter ptr");

	if (pFilter->IsWebFilter())
		return StartWebExport(pFilter);
	else
		return StartNativeExport(pFilter);

#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	virtual BOOL ViewComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
				Success = TRUE if the import was successful, FALSE otherwise
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Gives the component a chance to export info after the main document tree
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
#ifdef DO_EXPORT

	ERROR2IF(pFilter == NULL,FALSE,"Null filter ptr");

	if (!Success)
		return TRUE;

	if (pFilter->IsWebFilter())
		return EndWebExport(pFilter);
	else
		return EndNativeExport(pFilter);

#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	BOOL ViewComponent::StartWebExport(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports web stuff before the doc tree has been exported
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::StartWebExport(BaseCamelotFilter* pFilter)
{
	// In web format we need a view port record
	ViewComponent::ExportViewPortRecord(pFilter);

	return TRUE;
}

/********************************************************************************************

>	BOOL ViewComponent::StartNativeExport(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports native stuff before the doc tree has been exported
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::StartNativeExport(BaseCamelotFilter *pFilter)
{
	// In native format we need a view port record
	ViewComponent::ExportViewPortRecord(pFilter);

	return TRUE;
}

/********************************************************************************************

>	BOOL ViewComponent::EndWebExport(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports web stuff after the doc tree has been exported
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::EndWebExport(BaseCamelotFilter *pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL ViewComponent::EndNativeExport(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports native stuff after the doc tree has been exported
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::EndNativeExport(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	// save out DocView records for all available views
	BOOL ok = ExportAllViews(pFilter);

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	static BOOL ViewComponent::ExportViewPortRecord(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports the view port record to the filter
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::ExportViewPortRecord(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"Null filter ptr");

	DocRect Rect;

	// Find the document & spread ptrs
	Document* pDocument = pFilter->GetDocument();
	Spread* pSpread = pFilter->GetSpread();

	ERROR2IF(pDocument == NULL,FALSE,"No document dude!");
	ERROR2IF(pSpread == NULL,FALSE,"Can't find a spread");

	// Find the selection type selected by the user.
	// Default to DRAWING if there is no cached export options object
	SelectionType SelType = DRAWING;
	WebPrefsDlgParam* pPrefs = pFilter->GetCachedExportOptions();
	if (pPrefs != NULL)
		SelType = pPrefs->GetViewportSel();

	// WEBSTER - markn 15/2/97
	// Bug fix for selection type save option
#ifdef WEBSTER
	SelType = pFilter->GetSelType();
#endif // WEBSTER

	switch (SelType)
	{
		case SELECTION:
		{
			// Get the bounding rectangle for the selection
			SelRange* pSelection = GetApplication()->FindSelection();
			if (pSelection)
				Rect = pSelection->GetBoundingRect();

			// If the sel rect is empty, default to the drawing bounds
			if (Rect.IsEmpty())
				Rect = BaseBitmapFilter::GetSizeOfDrawing(pSpread);
		}
		break;

		case DRAWING: 
			Rect = BaseBitmapFilter::GetSizeOfDrawing(pSpread);
			break;

			//Graham 26/7/97: Took out spread option
		/*case SPREAD:
			Rect = BaseBitmapFilter::GetSizeOfSpread(pSpread);
			break;*/

		default :
			ERROR3("Unknown selection type in web export options");
			Rect = BaseBitmapFilter::GetSizeOfDrawing(pSpread);
			break;
	}

	// Write out the rectangle in the guise of a View Port record.
	CamelotFileRecord Rec(pFilter,TAG_VIEWPORT,TAG_VIEWPORT_SIZE);
	BOOL    ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Rect.lo);
	if (ok) ok = Rec.WriteCoord(Rect.hi);
	if (ok) ok = pFilter->Write(&Rec);

	// Find out the quality value of the selected DocView
	// Default to the maximum quality, if there isn't a selected doc view
	Quality ViewQuality(Quality::QualityMax);
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		ViewQuality.SetQuality(pDocView->RenderQuality.GetQuality());

	// The View Port record must always have a View Quality record after it
	if (!ExportViewQualityRecord(pFilter,&ViewQuality))
		return FALSE;

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	static BOOL ViewComponent::ExportViewQualityRecord(BaseCamelotFilter *pFilter,Quality* pQuality)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96
	Input:		pFilter	 = ptr to the filter to export to
				pQuality = ptr to Quality object that contains the quality value to write out
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports the view quality record to the filter

				NOTE!!!!!
				If the view quality is equal to DEFAULT_VIEW_QUALITY, then no record is saved out.
				The file format specifies that if no view qual record is encountered, assume the default setting,
				so we don't need to save a record out in the case, saving space.

	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::ExportViewQualityRecord(BaseCamelotFilter* pFilter, Quality* pQuality)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"Null filter ptr");
	ERROR2IF(pQuality == NULL,FALSE,"Null quality ptr");

	ERROR3IF(UINT32(pQuality->GetQuality()) > 255,"Can't store the view quality in a byte");

	// Don't save out a record if it defines the default value (saves space)
	if (pQuality->GetQuality() == DEFAULT_VIEW_QUALITY)
		return TRUE;

	// Write out the rectangle in the guise of a View Port record.
	CamelotFileRecord Rec(pFilter,TAG_VIEWQUALITY,TAG_VIEWQUALITY_SIZE);
	BOOL    ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(pQuality->GetQuality());
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	BOOL ViewComponent::ExportAllViews(BaseCamelotFilter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports all the current views as individual definition records to the filter
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::ExportAllViews(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"Null filter ptr");

	Document* pDocument = pFilter->GetDocument();
	ERROR2IF(pDocument == NULL,FALSE,"ExportViewDefinition no document pointer!");

	// Don't try saving views if it's the clipboard document or hidden (no views).
	if (pDocument->IsAClipboard() || pDocument->IsAHiddenDoc())
	{
		TRACEUSER( "JustinF", _T("Skipping document 0x%p in ViewComponent::ExportAllViews\n"),
					(LPVOID) pDocument);
		return TRUE;
	}

	BOOL ok = TRUE;

	// Find the first DocView (if there is one)
	DocView* pView = pDocument->GetFirstDocView();
	//if (pView != NULL && ok)
	while (pView != NULL && ok)
	{
		ok = ExportViewDefinition(pFilter, pView);
		pView = pDocument->GetNextDocView(pView);
	}

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	BOOL ViewComponent::ExportViewDefinition(BaseCamelotFilter* pFilter, DocView * pView)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Input:		pFilter = ptr to the filter to export to
				pView	= view to be exported
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports a single view definition record to the filter
	SeeAlso:	ViewComponent

********************************************************************************************/

BOOL ViewComponent::ExportViewDefinition(BaseCamelotFilter* pFilter, DocView * pView)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"ExportViewDefinition Null filter ptr");
	ERROR2IF(pView == NULL,FALSE,"ExportViewDefinition Null View ptr");

	// Find the spread ptrs
	Spread* pSpread = pFilter->GetSpread();
	ERROR2IF(pSpread == NULL,FALSE,"ExportViewDefinition no spread pointer");

	WorkCoord ScrollOffsets = pView->GetScrollOffsets();
	//INT32 XOffset			= ScrollOffsets.x.MakeLong();
	//INT32 YOffset			= ScrollOffsets.y.MakeLong();
	//WorkRect WorkViewRect	= pView->GetViewRect();
	
	// Get the current viewing rectangle in document coordinates
	DocRect ViewRect		= pView->GetDocViewRect(pSpread);
	// Convert this to spread coords. I know, I know it says DocCoords but image it is SpreadCoords
	DocRect  SpreadViewRect(ViewRect.ToSpread(pSpread, pView)); // ie from    DocCoords to SpreadCoords

	FIXED16 ScaleFactor		= pView->GetViewScale();

	// Find all the State flags
	BOOL ForeBackMode		= pView->GetForeBackMode();
	BOOL ShowGrid			= pView->GetShowGridState();
	BOOL SnapToGrid			= pView->GetSnapToGridState();
	BOOL SnapToObjects		= pView->GetSnapToObjectsState();
	BOOL SnapToMagObjects	= pView->GetSnapToMagObjectsState();
	BOOL ShowPrintBorders	= pView->GetShowPrintBorders();
	BOOL SnapToGuides		= pView->GetSnapToGuidesState();
	BOOL ShowGuides			= pView->GetShowGuidesState();
	BOOL ShowScrollBars		= pView->AreScrollersVisible();
	BOOL ShowRulers			= pView->AreRulersVisible();

	UINT32 ViewFlags = (SnapToObjects ? 8 : 0) | (SnapToGrid ? 4 : 0) | (ShowGrid ? 2 : 0) | (ForeBackMode ? 1 : 0);
	ViewFlags	   |= (( (ShowGuides ? 8 : 0) | (SnapToGuides ? 4 : 0) | (ShowPrintBorders ? 2 : 0) | (SnapToMagObjects ? 1 : 0) ) << 8);
	ViewFlags	   |= (( (ShowRulers ? 2 : 0) | (ShowScrollBars ? 1 : 0) ) << 16);

	//BOOL Multilayer			= IsMultilayer();
	//BOOL AllLayersVisible	= IsAllVisible();

	// Write out the rectangle in the guise of a View Port record.
	CamelotFileRecord Rec(pFilter,TAG_DOCUMENTVIEW, TAG_DOCUMENTVIEW_SIZE);
	BOOL    ok = Rec.Init();
	if (ok) ok = Rec.WriteFIXED16(ScaleFactor);
	if (ok) ok = Rec.WriteCoord(SpreadViewRect.lo);	// bottom left of work area
	if (ok) ok = Rec.WriteCoord(SpreadViewRect.hi);	// top right of work area
	if (ok) ok = Rec.WriteUINT32(ViewFlags);
	if (ok) ok = pFilter->Write(&Rec);

	// Find out the quality value of the DocView
	// Default to the maximum quality, if there isn't a doc view
	Quality ViewQuality(Quality::QualityMax);
	if (pView != NULL)
		ViewQuality.SetQuality(pView->RenderQuality.GetQuality());

	// The Doc View record must always have a View Quality record after it
	if (!ExportViewQualityRecord(pFilter,&ViewQuality))
		return FALSE;

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


/********************************************************************************************

>	ViewRecordHandler::ViewRecordHandler()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96
	Purpose:	Default constructor
				Inits the view record handler
	SeeAlso:	-

********************************************************************************************/

ViewRecordHandler::ViewRecordHandler()
{
	pLastDocView = NULL;
}

/********************************************************************************************

>	BOOL ViewRecordHandler::BeginImport()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Informs the handler that importing is about to begin
	SeeAlso:	-

********************************************************************************************/

BOOL ViewRecordHandler::BeginImport()
{
	pLastDocView = NULL;
	ViewCount = 0;
	return TRUE;
}

/********************************************************************************************

>	virtual UINT32* ViewRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* ViewRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_VIEWPORT,
								TAG_VIEWQUALITY,
								TAG_DOCUMENTVIEW,
								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL ViewRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL ViewRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_VIEWPORT:
			ok = HandleViewPortRecord(pCXaraFileRecord);
			break;

		case TAG_VIEWQUALITY:
			ok = HandleViewQualityRecord(pCXaraFileRecord);
			break;

		case TAG_DOCUMENTVIEW:
			ok = HandleDocumentViewRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL ViewRecordHandler::HandleViewPortRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the view port record.

				Current, all this does is set the coord origin to be the centre of the view port rect,
				but only if we are importing.

				I'm 28 today.  Late twenties are depressing because you realise that you're not going
				to change any more in any great way.  You have grown up.  When you're younger, there's that
				unspoken hope that things will get better, but now you know you are what you are, so you'd
				better make the most of it.  I kind of expected things to get more fun, but you can't expect it.

	SeeAlso:	-

********************************************************************************************/

BOOL ViewRecordHandler::HandleViewPortRecord(CXaraFileRecord* pCXaraFileRecord)
{
	TRACEUSER( "Simon", _T("D1 Picked up ViewRecord")); 
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_VIEWPORT,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;
//#ifndef RALPH

	DocRect ViewPortRect;
	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&ViewPortRect.lo,0,0);
	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&ViewPortRect.hi,0,0);

	if (ok && IsOpeningMinimalWebFormat())
	{
		Spread* pSpread = GetSpread();

		if (pSpread != NULL)
		{
			MILLIPOINT Width = 0;
			MILLIPOINT Height = 0;
			MILLIPOINT Margin = 0;
			MILLIPOINT Bleed = 0;
			BOOL Dps = TRUE;
			BOOL ShowDropShadow = TRUE;

#ifdef RALPH
			MILLIPOINT Inflate = 0;
#else
			MILLIPOINT Inflate = (max(ViewPortRect.Width(),ViewPortRect.Height())*5)/100;
#endif
			DocRect NewPageRect = ViewPortRect;
			NewPageRect.Inflate(Inflate);

			if (ok) ok = pSpread->GetPageSize(&Width,				&Height,				&Margin, &Bleed, &Dps, &ShowDropShadow);
			if (ok) ok = pSpread->SetPageSize(NewPageRect.Width(),	NewPageRect.Height(),	Margin,   Bleed,  Dps,  ShowDropShadow);

			Page *pPage = pSpread->FindFirstPageInSpread(); 
			ERROR2IF(pPage == NULL,FALSE,"Spread has no pages"); 
			DocRect PageRect = pPage->GetPageRect();

			// align the bottom-left of the viewport with the bottom-left of the page, but
			// add 'Inflate' to give it a comfortable distance away from the page's edge
			DocCoord Origin = PageRect.lo;
			Origin.x += (Inflate - ViewPortRect.lo.x);
			Origin.y += (Inflate - ViewPortRect.lo.y);
			SetCoordOrigin(Origin);

			ViewPortRect.Translate(-ViewPortRect.lo.x,-ViewPortRect.lo.y);
		}
	}

	if (ok && IsImportingAtPosition())
	{
		pCXaraFileRecord->ResetReadPos();

		DocRect ViewPortRect;
		if (ok) ok = pCXaraFileRecord->ReadCoord(&ViewPortRect.lo);
		if (ok) ok = pCXaraFileRecord->ReadCoord(&ViewPortRect.hi);

		DocCoord Centre = ViewPortRect.lo;
		Centre.x += ViewPortRect.Width()/2;
		Centre.y += ViewPortRect.Height()/2;

		DocCoord Origin = GetCoordOrigin();

		Origin.x -= Centre.x - Origin.x;
		Origin.y -= Centre.y - Origin.y;

		SetCoordOrigin(Origin);
	}

//#else
#ifdef RALPH
/*	DocRect Rect;
	ok = pCXaraFileRecord->ReadCoord(&Rect.lo);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&Rect.hi);
	*/
	if (ok)
	{
		TRACEUSER( "Simon", _T("D1 Before FakeZoomToRect")); 
		
		DocRect RalphViewPortRect = ViewPortRect;

		Document * pDoc = Document::GetCurrent();
		RalphDocument * pRalphDoc = NULL;
		RalphView * pRalphView = NULL;
		if(pDoc)
		{
			if(pDoc->IsARalphDoc())
			{
				pRalphDoc=pDoc->GetRalphDoc();
				if(pRalphDoc)
				{
					
					// Store the viewport away until its safe to zoom.
					pRalphDoc->SetViewportRect(RalphViewPortRect); 
				}
			}
		}
		// We used to do this !!, which was bad cos we had multiple threads piddling with RenderRegion lists.
		//	OpZoomDescriptor::FakeZoomToRect(&Rect);
//		DocView::GetCurrent()->SetScrollOffsets(WorkCoord(0,0), FALSE);
	}

#endif
	TRACEUSER( "Simon", _T("D1 End Handle View Record")); 

	return ok;
}

/********************************************************************************************

>	BOOL ViewRecordHandler::HandleViewQualityRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the view quality record.
				It reads a view quality value to apply to the last view port & doc view read in

				**** implementation not complete ****
	SeeAlso:	-

********************************************************************************************/

BOOL ViewRecordHandler::HandleViewQualityRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_VIEWQUALITY,FALSE,"I don't handle this tag type");

	BYTE QualVal;
	BOOL ok = pCXaraFileRecord->ReadBYTE(&QualVal);

	if (pLastDocView != NULL)
		pLastDocView->RenderQuality.SetQuality(QualVal);

	return ok;
}

/********************************************************************************************

>	BOOL ViewRecordHandler::HandleDocumentViewRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the document view definition record.
				All this does is set the current view quality (applied to subsequent view port & doc view records)
	SeeAlso:	-

********************************************************************************************/

BOOL ViewRecordHandler::HandleDocumentViewRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"HandleDocumentViewRecord pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_DOCUMENTVIEW,FALSE,"HandleDocumentViewRecord I don't handle this tag type");

	// If we are just importing data from this document into an existing one,
	// then don't import this data.
	if (IsImporting())
		return TRUE;
	
	// Read in all the information
	FIXED16 ScaleFactor;
	DocCoord BottomLeft;
	DocCoord TopRight;
	UINT32 ViewFlags = 0L;
	
	BOOL ok = pCXaraFileRecord->ReadFIXED16(&ScaleFactor);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&BottomLeft);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&TopRight);
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&ViewFlags);

	Document* pDocument = GetDocument();
	ERROR2IF(pDocument == NULL,FALSE,"HandleDocumentViewRecord no document pointer!");

	Spread* pSpread = GetSpread();
	ERROR2IF(pSpread == NULL,FALSE,"HandleDocumentViewRecord no spread pointer");

	// Find the first DocView (if there is one)
	DocView* pView = pDocument->GetFirstDocView();
	// Increment our view count
	ViewCount ++;
	// Check that we should restore the view on import
	BOOL Restore = pDocument->GetRestoreViewOnImport();
			
	// If everything was read in ok and we have a view and this is the first view record
	// then apply the data
	if (ok && pView && ViewCount == 1 && Restore)
	{
		// Set the view for the following code.  The zoom ops work on the current
		// view, as do the coordinate conversion functions.
		ERROR3IF(pView != DocView::GetCurrent(),"ViewRecordHandler::HandleDocumentViewRecord DocView::Current is screwed");
		pView->SetCurrent();

		// Set the view scale and positioning
		// First set the correct view scale factor as otherwise some of the conversions may go wrong
		pView->SetViewScale(ScaleFactor);
		// The top left corner of this rectangle is the same as	the value returned by GetScrollOffsets().
		// First take our saved version which is in SpreadCoords.
		// I know, I know just imagine DocCoord = SpreadCoord at this point.
		DocCoord SpreadScrollOffsets(BottomLeft.x, TopRight.y);
		// Convert this to DocCoords
		DocCoord DocScrollOffsets(SpreadScrollOffsets.ToDoc(pSpread, pView));
		// And now to WorkCoords, which the scroll offsets require
		WorkCoord ScrollOffsets(DocScrollOffsets.ToWork(pSpread, pView));

		// Check the scroll offsets bounds as we can't have scroll offsets anywhere.
		if (ScrollOffsets.x < 0) ScrollOffsets.x = 0;
		if (ScrollOffsets.y > 0) ScrollOffsets.y = 0;

		pView->SetScrollOffsets(ScrollOffsets, TRUE);

		// Find all the State flags
		BOOL ForeBackMode		= FALSE;
		BOOL ShowGrid			= FALSE;
		BOOL SnapToGrid			= FALSE;
		BOOL SnapToObjects		= FALSE;
		BOOL SnapToMagObjects	= FALSE;
		BOOL ShowPrintBorders	= FALSE;
		BOOL SnapToGuides		= FALSE;
		BOOL ShowGuides			= FALSE;
		BOOL ShowScrollBars		= FALSE;
		BOOL ShowRulers			= FALSE;

// WEBSTER - markn 14/1/97
// Various view flags should be off in Webster, so I've commented out
// the 'if' statements that can set them to TRUE
#ifndef WEBSTER
		if (ViewFlags & 8)
			SnapToObjects = TRUE;
#endif // WEBSTER

		if (ViewFlags & 4)
			SnapToGrid = TRUE;
		if (ViewFlags & 2)
			ShowGrid = TRUE;
		if (ViewFlags & 1)
			ForeBackMode = TRUE;
		
#ifndef WEBSTER
		if (ViewFlags & (8 << 8)) 
			ShowGuides = TRUE;
		if (ViewFlags & (4 << 8))
			SnapToGuides = TRUE;
		if (ViewFlags & (2 << 8))
			ShowPrintBorders = TRUE;
		if (ViewFlags & (1 << 8))
			SnapToMagObjects = TRUE;

		if (ViewFlags & (2 << 16))
			ShowRulers = TRUE;
#endif // WEBSTER

#ifndef WEBSTER
		// In Camelot use the saved value
		if (ViewFlags & (1 << 16))
			ShowScrollBars = TRUE;
#else
		// In Webster the user has no control over the scroll bars and so it must be on
		ShowScrollBars = TRUE;
#endif
		pView->SetForeBackMode(ForeBackMode);
		pView->SetShowGridState(ShowGrid);
		pView->SetSnapToGridState(SnapToGrid);
		pView->SetSnapToObjectsState(SnapToObjects);
		pView->SetSnapToMagObjectsState(SnapToMagObjects);
		pView->SetShowPrintBorders(ShowPrintBorders);
		pView->SetSnapToGuidesState(SnapToGuides);
		pView->SetShowGuidesState(ShowGuides);
		pView->ShowViewScrollers(ShowScrollBars);
		pView->ShowViewRulers(ShowRulers);

		// The set the default view quality for the view
		pView->RenderQuality.SetQuality(DEFAULT_VIEW_QUALITY);
		
		// Update the button bar
#ifndef RALPH
		OpZoomComboDescriptor::Update();
#endif
		
		pLastDocView = pView;
	}

	return ok;
}

/********************************************************************************************

>	virtual void ViewRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the viewrecords.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void ViewRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	char s[256];

	//	 Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_VIEWPORT:
		{
			DocCoord lo,hi;

			pRecord->ReadCoord(&lo);
			pRecord->ReadCoord(&hi);

			_stprintf(s,"lo.x = %d\r\n",lo.x);	(*pStr) += s;
			_stprintf(s,"lo.y = %d\r\n",lo.y);	(*pStr) += s;
			_stprintf(s,"hi.x = %d\r\n",hi.x);	(*pStr) += s;
			_stprintf(s,"hi.y = %d\r\n",hi.y);	(*pStr) += s;
		}
		break;

		case TAG_VIEWQUALITY:
		{
			BYTE Quality = 0;
			pRecord->ReadBYTE(&Quality);

			_stprintf(s,"Quality = %d\r\n",Quality);
			(*pStr) += s;
		}
		break;

		case TAG_DOCUMENTVIEW:
		{
			FIXED16 ScaleFactor;
			DocCoord BottomLeft;
			DocCoord TopRight;
			UINT32 ViewFlags = 0L;
			pRecord->ReadFIXED16(&ScaleFactor);
			pRecord->ReadCoord(&BottomLeft);
			pRecord->ReadCoord(&TopRight);
			pRecord->ReadUINT32(&ViewFlags);

			(*pStr) += "Document view\r\n\r\n";
			_stprintf(s,"Scale factor \t= %f\r\n",ScaleFactor.MakeDouble());
			(*pStr) += s;
			_stprintf(s,"Bottom left \t= %d,%d\r\n",BottomLeft.x,BottomLeft.y);
			(*pStr) += s;
			_stprintf(s,"Top right \t\t= %d,%d\r\n",TopRight.x,TopRight.y);
			(*pStr) += s;
			// Show all the State flags
			if (ViewFlags & 8)
				(*pStr) += "SnapToObjects";
			if (ViewFlags & 4)
				(*pStr) += "SnapToGrid ";
			if (ViewFlags & 2)
				(*pStr) += "ShowGrid ";
			if (ViewFlags & 1)
				(*pStr) += "ForeBackMode ";
			(*pStr) += "\r\n";
			
			if (ViewFlags & (8 << 8)) 
				(*pStr) += "ShowGuides ";
			if (ViewFlags & (4 << 8))
				(*pStr) += "SnapToGuides ";
			if (ViewFlags & (2 << 8))
				(*pStr) += "ShowPrintBorders ";
			if (ViewFlags & (1 << 8))
				(*pStr) += "SnapToMagObjects ";
			(*pStr) += "\r\n";

			if (ViewFlags & (2 << 16))
				(*pStr) += "ShowRulers ";
			if (ViewFlags & (1 << 16))
				(*pStr) += "ShowScrollBars ";
			(*pStr) += "\r\n";

			//_stprintf(s,"Flags \t= %d\r\n",ViewFlags);
			//(*pStr) += s;
		}
		break;
	}
}
#endif
