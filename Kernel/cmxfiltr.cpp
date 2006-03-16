// $Id: cmxfiltr.cpp 662 2006-03-14 21:31:49Z alex $
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
// CMX (Corel Metafile) Filter

/*
*/


#include "camtypes.h"
#include "cmxfiltr.h"
#include "oilfltrs.h"
//#include "filtrres.h"
#include "ccfile.h"
#include "cmxexdc.h"
#include "cmxrendr.h"
#include "page.h"
#include "spread.h"
//#include "tim.h"
#include "docview.h"
#include "nodepath.h"
//#include "cmxres.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(CMXFilter, VectorFilter)
CC_IMPLEMENT_DYNAMIC(CMXFilter16, CMXFilter)
CC_IMPLEMENT_DYNAMIC(CMXFilter32, CMXFilter)

/********************************************************************************************

>	CMXFilter::CMXFilter()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		None
	Returns:	None
	Purpose:	Constructor for class CMXFilter
	SeeAlso:	class VectorFilter

********************************************************************************************/

CMXFilter::CMXFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_CMXFILTER_FILTERNAME));
	FilterInfo.Load(_R(IDT_CMXFILTER_FILTERINFO));
	FilterID = FILTERID_NONE;

	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;

#ifdef DO_EXPORT
	//WEBSTER-Martin-27/01/97
#ifndef WEBSTER
	Flags.CanExport = TRUE;
#endif //WEBSTER
#endif
	
	ImportMsgID = _R(IDT_IMPORTMSG_CMX);

	ExportDC = NULL;
	ExportRegion = NULL;
	ExportMsgID = _R(IDT_EXPORTMSG_CMX);

	IsDoingClipboardExport = FALSE;
}

/********************************************************************************************

>	CMXFilter16::CMXFilter16()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		None
	Returns:	None
	Purpose:	Constructor for class CMXFilter16
	SeeAlso:	class CMXFilter

********************************************************************************************/

CMXFilter16::CMXFilter16()
{
	// Set up filter descriptions.
	FilterID = FILTERID_CMX16;
}

/********************************************************************************************

>	CMXFilter32::CMXFilter32()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		None
	Returns:	None
	Purpose:	Constructor for class CMXFilter16
	SeeAlso:	class CMXFilter

********************************************************************************************/

CMXFilter32::CMXFilter32()
{
	// Set up filter descriptions.
	FilterID = FILTERID_CMX32;
}

/********************************************************************************************

>	BOOL CMXFilter16::Init()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		None
	Returns:	BOOL, propably saying whether we succeeded or not, buts it not documented
	Purpose:	Initialiser for class CMXFilter
	SeeAlso:	Filter

********************************************************************************************/

BOOL CMXFilter16::Init()
{
 	// Get the OILFilter object
	pOILFilter = new CMX16OILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	BOOL CMXFilter32::Init()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		None
	Returns:	BOOL, propably saying whether we succeeded or not, buts it not documented
	Purpose:	Initialiser for class CMXFilter
	SeeAlso:	Filter

********************************************************************************************/

BOOL CMXFilter32::Init()
{
 	// Get the OILFilter object
	pOILFilter = new CMX32OILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	
	return TRUE;
}

/********************************************************************************************

>	INT32 CMXFilter::HowCOmpatible(PathName& Filename,
							ADDR HeaderStart,
							UINT32 HeaderSize,
							UINT32 FileSize);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		Filename - filename of file to check
				HeaderStart, HeaderSize, - infomation on header of file loaded into memory
				FileSize - length of file (bytes)
	Returns:	0 to 10. 0 means I know nothing about this file, 10 means its definitly mine
	Purpose:	Looks at the start of the file to see if we recognise it as one of ours
	SeeAlso:	class Filters, class CDRFilter

********************************************************************************************/

INT32 CMXFilter::HowCompatible(PathName & FileName, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
	return 0;
}

/********************************************************************************************

>	BOOL CMXFilter::DoImport(SelOperation * Op, CCLexFile * pDiskFile,
							Document * DestDoc, BOOL AutoChoosen , ImportPosition * Pos,
							KernelBitmap** ppImportedBitmap,DocCoord* pPosTranslate, String_256* URL)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		Op - pointer to the operatioj that this input process is associated with,
				pDiskFile - the CMX file to be loaded
				DestDoc - the document object whcih should hold the data read in from this file
				URL - original URL of the imported file
	Returns:	TRUE is the input operation worked, FALSE if not
	Purpose:	Read an CMX format file and convert it into Camelot tree data structures. When
				the file is loaded we force a redraw for the area affected by the new data
				At the moment the data is just put on the first layer of the first spread
				of the first chapter in the document, and this is a bodge.
				The operation is terminated (i.e. its End() function is called) if the
				operation completed successfully.
	Errors:		Fails (returns FALSE) if the document structure is incorrect, if the CMX file
				contains a syntax error, of the CMX file cannot be found/opened or if its
				not actually an CMX file.
	SeeAlso:	CMXFilter::HowCompatible

********************************************************************************************/

BOOL CMXFilter::DoImport(SelOperation * Op, CCLexFile * pDiskFile,
							Document * DestDoc, BOOL AutoChoosen , ImportPosition * Pos,
							KernelBitmap** ppImportedBitmap,DocCoord* pPosTranslate, String_256* URL)
{
	return FALSE;
}

/********************************************************************************************

>	BOOL CMXFilter::DoExport(Operation *, CCLexFile *, PathName *, Document * );

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		
	Returns:	
	Purpose:	does the exportness
	SeeAlso:	

********************************************************************************************/

BOOL CMXFilter::DoExport(Operation* pOp, CCLexFile* pFile, PathName* pPath, Document* pDoc,
						 BOOL ShowOptions)
{
#ifdef DO_EXPORT
	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already.
	BOOL ok;
	if (!pFile->isOpen())
	{
		if (pFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{
			ok = OpenExportFile((CCDiskFile*) pFile, pPath);
			if (!ok) return FALSE;
		}
		else
		{
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in CMXFilter::DoExport\n"));
			return FALSE;
		}
	}

	// Set up document pointer
	TheDocument = pDoc;

	// set a nice flag
	DisplaySquishyWarning = FALSE;

	// Get pointer to the spread to export.
	Spread *pSpread = GetFirstSpread(pDoc);

	// Set up device context and render region for this export.
	if (!PrepareToExport(pFile, pSpread))
	{
		CleanUpAfterExport();
		return FALSE;
	}

	// Graeme (15-2-00) - Ben's original code used the spread's bounding box, which appears
	// to line in the range +/- 10,000,000. This was giving the wrong results, so I've
	// now modified the code to create the PageBBox from the page's bounding box.
	Page *pPage = ( Page * ) pSpread->FindFirstPageInSpread ();
	DocRect PageBBox = pPage->GetPageRect ();

	ExportDC->SetInfo(pOp, pPath, pDoc);

	// write the header
	if(!ExportDC->CreateHeader(&PageBBox))
		return FALSE;

	// preview bitmap...
	if(!IsDoingClipboardExport)
	{
		if(!ExportDC->WritePreviewBitmap())
			return FALSE;
	}

	// start the page
	if(!ExportDC->StartPage(&PageBBox))
		return FALSE;

	// Export the data to the file
	if (!ExportRender(ExportRegion))
	{
		CleanUpAfterExport();
		return FALSE;
	}

	// end the page
	if(!ExportDC->EndPage())
		return FALSE;

	// write the reference list
	if(!ExportDC->WriteReferenceList())
		return FALSE;

	// write the description sections
	if(!ExportDC->WriteDescriptionSections())
		return FALSE;

	// write any other sections
	if(!ExportDC->WriteReferSections())
		return FALSE;

	// write the index
	if(!ExportDC->WriteIndex())
		return FALSE;

	if(!FinishExport())
		return FALSE;

	// All done - deallocate dynamic objects and return success.
	CleanUpAfterExport();

	// Detach document
	TheDocument = NULL;

	if(DisplaySquishyWarning)
	{
		Error::SetError(_R(IDT_CMXFILTER_SQUISHWARNING), 0);
		InformWarning();
		Error::ClearError();
	}

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	Matrix CMXFilter16::GetExportTransform ( Spread *pSpread )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/00
	Inputs:		pSpread	- A pointer to the spread that is being exported.
	Returns:	Matrix	- A transformation matrix to covert Camelot co-ordinate spaces into
						  CMX co-ordinate spaces.
	Purpose:	Creates a transformation matrix that provides the mapping between a Camelot
				document's co-ordinate system, and that used within the CMX file.
	SeeAlso:	

********************************************************************************************/

Matrix CMXFilter16::GetExportTransform(Spread *pSpread)
{
#ifdef DO_EXPORT

	// Work out where the origin is...
	Page *pPage = ( Page * ) pSpread->FindFirstPageInSpread ();
	
	// Use the centre of the page as the origin.
	DocCoord CMXOrigin = ( pPage->GetPageRect () ).Centre ();

	// CMXOrigin is the origin of the page in CMX coords...
	Matrix Mat ( -CMXOrigin.x, -CMXOrigin.y );

	// Fiddle with the scale factor
	Mat *= Matrix ( FIXED16 ( CAMCOORD_SCALEFACTOR16 ), FIXED16 ( CAMCOORD_SCALEFACTOR16 ) );

	return Mat;
#else
	return Matrix ();
#endif
}


/********************************************************************************************

>	Matrix &CMXFilter32::GetExportTransform(Spread *pSpread)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		none
	Returns:	whether it fell over or not, really.
	Purpose:	get everything ready for the old exporting business -- basically
				set up one of our nice render regions, write a little header
				and get everything under way.
	SeeAlso:	

********************************************************************************************/

Matrix CMXFilter32::GetExportTransform(Spread *pSpread)
{
#ifdef DO_EXPORT
	// Work out where the origin is...
	Page *pPage = ( Page * ) pSpread->FindFirstPageInSpread ();
	
	// Use the centre of the page as the origin.
	DocCoord CMXOrigin = ( pPage->GetPageRect () ).Centre ();

	// CMXOrigin is the origin of the page in CMX coords...
	Matrix Mat ( -CMXOrigin.x, -CMXOrigin.y );

	// Fiddle with the scale factor
	Mat *= Matrix(FIXED16(CAMCOORD_SCALEFACTOR32), FIXED16(CAMCOORD_SCALEFACTOR32));

	return Mat;
#else
	return Matrix();
#endif
}


/********************************************************************************************

>	BOOL CMXFilter::PrepareToExport(CCLexFile*, Spread* pSpread)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		disk file to pop things out to, spread to do funky things to
	Returns:	whether it fell over or not, really.
	Purpose:	get everything ready for the old exporting business -- basically
				set up one of our nice render regions, write a little header
				and get everything under way.
	SeeAlso:	

********************************************************************************************/

BOOL CMXFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)
{
#ifdef DO_EXPORT
	// Get the position of the first page, and use this to set the origin.
	Page *pPage = pSpread->FindFirstPageInSpread(); 
	ENSURE(pPage != NULL, "Spread has no pages");
	ERRORIF(pPage == NULL, _R(IDT_DOC_BADSTRUCTURE), FALSE);

	// Create a new render region to export to:
	Matrix Mat = GetExportTransform(pSpread);
	ExportRegion = CreateRenderRegion(&Mat);

	if (ExportRegion == NULL)
		return FALSE;

	// Created the 'file' DC for rendering and try to open the specified file.
	ExportDC = CreateExportDC();

	if (ExportDC == NULL)
		return FALSE;

	TRY
	{
		ExportDC->Init(pFile, ExportRegion, IS_A(this, CMXFilter16)?FALSE:TRUE, IsDoingClipboardExport);
	}
	CATCH(CFileException, e)
	{
		// Could not open the export file - return error
		return FALSE;
	}
	END_CATCH

	// Attach to the right device.
	View* pView = View::GetCurrent();
	ERROR2IF(!pView, FALSE, "No current View to attach in CMXFilter::PrepareForExport");
	ExportRegion->AttachDevice(pView, ExportDC, pSpread);
		
	// All ok
	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL CMXFilter::FinishExport(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/96
	Inputs:		nothin'
	Returns:	success
	Purpose:	Finish off the export operation -- finishes header and all that
	SeeAlso:	

********************************************************************************************/

BOOL CMXFilter::FinishExport(void)
{
#ifdef DO_EXPORT
	if(!ExportDC->PolishOffThatFile())
		return FALSE;
	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	void CMXFilter::CleanUpAfterExport()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		nothin'
	Returns:	nothin'
	Purpose:	Sorts everything out when something went wrong, or we finished normally
	SeeAlso:	

********************************************************************************************/

void CMXFilter::CleanUpAfterExport()
{
#ifdef DO_EXPORT
	ExportDC->CleanUp();

	// get rid of that render region
	delete ExportRegion;
#endif
}


/********************************************************************************************

>	CMXExportDC* CMXFilter::CreateExportDC();

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Inputs:		nothin'
	Returns:	the device context to use when exporting CMX files
	Purpose:	Get a device context up and running.
	SeeAlso:	

********************************************************************************************/

CMXExportDC* CMXFilter::CreateExportDC()
{
#ifdef DO_EXPORT
	// Create and return a DC
	return new CMXExportDC(this);
#else
	return NULL;
#endif
}


/********************************************************************************************

>	CMXRenderRegion *CMXFilter::CreateRenderRegion(Matrix *pM)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/96
	Inputs:		nothin'
	Returns:	render region or 0
	Purpose:	create a 32 bit render region for CMX export.
	SeeAlso:	

********************************************************************************************/

CMXRenderRegion *CMXFilter::CreateRenderRegion(Matrix *pM)
{
#ifdef DO_EXPORT
	// Don't care about clip regions when exporting - create a null region.
	DocRect NullClipRect;
	NullClipRect.MakeEmpty();

	// Don't use view scale; set to 1
	FIXED16 Scale(1);

	// Create the region
	return new CMXRenderRegion(NullClipRect, *pM, Scale);
#else
	// Ne yuk yuk... No cmx export for you pesky ralphs...
	return NULL;
#endif
}


