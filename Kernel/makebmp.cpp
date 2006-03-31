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

/*
*/

#include "camtypes.h"
#include "makebmp.h"
//#include "resource.h"
#include "app.h"
#include "document.h"
#include "page.h"
#include "spread.h"
#include "nodebmp.h"
#include "bitmpinf.h"
#include "bmpfiltr.h"
//#include "simon.h"
//#include "will3.h"
#include "docview.h"
#include "clikmods.h"
#include "nodetxtl.h"
#include "grndbmp.h"
#include "wrkrect.h"
#include "colplate.h"
#include "colcontx.h"
#include "bmapprev.h"
#include "rrcaps.h"

//#include "resimmap.h"	//_R(IDE_EXPORT_NODRAWING)

#include "palman.h"		// PaletteManager::Get4bppBrowserPalette

DECLARE_SOURCE("$Revision$");  

CC_IMPLEMENT_DYNCREATE(OpConvertToBitmap, SelOperation);
CC_IMPLEMENT_DYNAMIC(MakeBitmapFilter, TI_GIFFilter);
CC_IMPLEMENT_DYNCREATE(MakeBitmapExportOptions, MaskedFilterExportOptions)

#define	new	CAM_DEBUG_NEW

DPI			MakeBitmapExportOptions::g_Dpi 			= 96.0;
BMP_DEPTH	MakeBitmapExportOptions::g_Depth		= 8;
DITHER		MakeBitmapExportOptions::g_Dither		= XARADITHER_ERROR_DIFFUSION;
PALETTE		MakeBitmapExportOptions::g_Palette		= PAL_OPTIMISED;
BOOL		MakeBitmapExportOptions::g_TranspBkGnd	= TRUE;
INT32 		MakeBitmapFilter::FilterType			= MAKE_BITMAP_FILTER;

//-----------------------------------------------------------------------------------------------
// OpConvertToBitmap - Convert one or more objects into a bitmap

/********************************************************************************************

>	OpConvertToBitmap::OpConvertToBitmap() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpConvertToBitmap constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
OpConvertToBitmap::OpConvertToBitmap(): SelOperation()								
{                              
}

/********************************************************************************************

>	BOOL OpConvertToBitmap::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpConvertToBitmap initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/
BOOL OpConvertToBitmap::Init()
{
	MakeBitmapFilter::Initialise();

	return (RegisterOpDescriptor(
	 							0,
	 							_R(IDS_CONVERTTOBITMAPOP),
								CC_RUNTIME_CLASS(OpConvertToBitmap),
	 							OPTOKEN_CONVERTTOBITMAP,
	 							OpConvertToBitmap::GetState,
	 							0,	/* help ID */
	 							0,
	 							0	/* bitmap ID */));
}               
    
/********************************************************************************************

>	OpState	OpConvertToBitmap::GetState(String_256*, OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Returns:	The state of the OpConvertToBitmap operation
	Purpose:	For finding the OpConvertToBitmap's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
OpState	OpConvertToBitmap::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	String_256 DisableReason;

	// Ensure that a document exists
	if (Document::GetSelected() == NULL)
	{
		// There is no selected document
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_DOC));
	   	*UIDescription = DisableReason;	     
	   	return OpSt;                                 
	}

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	  
	// This operation is disabled if there are no nodes selected, or if none of the selected 
	// objects can become a NodePath. 

	Node* FirstSelected = Sel.FindFirst();  
	
	OpSt.Greyed = FALSE; 
	if (FirstSelected == NULL) 
	{
		OpSt.Greyed = TRUE; 
		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_OBJECTS_SELECTED));
	   	*UIDescription = DisableReason;	        
	}
	
	return(OpSt);   
}

/********************************************************************************************

>	void OpConvertToBitmap::Do(OpDescriptor*)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Purpose:	Convert the selection into a bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void OpConvertToBitmap::Do(OpDescriptor*)
{   
	// We want to include bevel/contour/shadow controller nodes in our bitmap-copy
	// deliberations, so set PromoteToParent on a copy of the app's SelRange.
	SelRange Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Find the first node which is selected 
	Node* CurrentNode = Sel.FindFirst(); 
	
	ERROR3IF(CurrentNode == NULL, "Called ConvertToBitmap with no nodes selected"); 
	
	// In the retail build it is best to do nothing if we find there are no selected nodes 
	if (CurrentNode != NULL) // No nodes selected so End
	{   
		// We need to invalidate the region
		if (!DoInvalidateNodesRegions(Sel, TRUE))
			goto EndOperation; 

		if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't
			goto EndOperation;  		 // render the blobs though 
             
        // Create a special Bitmap filter, that creates a bitmap
        // and puts it in the tree   
		MakeBitmapFilter* pBitmapFilter = new MakeBitmapFilter();
		if (pBitmapFilter == NULL)
		{
			FailAndExecute();
			goto EndOperation;  
		}

		KernelBitmap* pBitmapToCreate = NULL;

		if (!pBitmapFilter->DoCreateBitmap(this, Document::GetSelected(), &pBitmapToCreate))
		{
			delete pBitmapFilter;
			FailAndExecute();
			goto EndOperation;  
		}

		if (!pBitmapFilter->InsertBitmapIntoDocument(this, pBitmapToCreate, Document::GetSelected()))
		{
			delete pBitmapToCreate;
			delete pBitmapFilter;
			FailAndExecute();
			goto EndOperation;  
		}

		delete pBitmapFilter;

		if (!DoInvalidateNodesRegions(*(GetApplication()->FindSelection()), TRUE))
		{
			// We need to invalidate the region
			FailAndExecute();
			goto EndOperation; 
		} 
	}                   

 	EndOperation:

	SelRange* pRng = GetApplication()->FindSelection();

	End();
}			

/********************************************************************************************

>	static BOOL MakeBitmapExportOptions::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Will)
	Created:	29/10/96
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
	Purpose:	To declare preferences associated with these export options

********************************************************************************************/
BOOL MakeBitmapExportOptions::Declare()
{
	if (Camelot.DeclareSection("Filters", 10))
	{
		Camelot.DeclarePref( NULL, "CreateBitmapDPI", &g_Dpi, 1, 3000 );
		Camelot.DeclarePref( NULL, "CreateBitmapDepth", &g_Depth, 1, 32 );
		Camelot.DeclarePref( NULL, "CreateBitmapDither", (INT32*)&g_Dither, 0, XARADITHER_MAX );
		Camelot.DeclarePref( NULL, "CreateBitmapPalette", (INT32*)&g_Palette, 0, 1 );
		Camelot.DeclarePref( NULL, "CreateBitmapBkGnd", &g_TranspBkGnd, FALSE, TRUE );
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	MakeBitmapExportOptions::MakeBitmapExportOptions(	const FILTER_ID FilterID, 
														const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor for a MakeBitmapExportOptions object (mainly to allow
				CreateBitmapCopy to store its own defaults)

********************************************************************************************/
MakeBitmapExportOptions::MakeBitmapExportOptions(	const FILTER_ID FilterID, 
													const StringBase* pFilterName) : 
						MaskedFilterExportOptions(_R(IDD_EXPORTBMPOPTS), FilterID, pFilterName)
{
}

/********************************************************************************************

>	BitmapExportOptions *MakeBitmapExportOptions::MakeCopy()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Stefan code
	Created:	2/7/97
	Purpose:	Makes a copy object of this object
	See Also:	BitmapExportOptions::CopyFrom(); BitmapExportOptions::MakeCopy();

********************************************************************************************/
BitmapExportOptions *MakeBitmapExportOptions::MakeCopy()
{
	// Get the runtime class info on this object
	CCRuntimeClass *pCCRuntimeClass = GetRuntimeClass();

	// Create another object of the same type
	MakeBitmapExportOptions *temp = (MakeBitmapExportOptions *) pCCRuntimeClass->CreateObject();

	if (temp != NULL)
		temp->CopyFrom(this); // copy the contents accross
	
	return temp;
}

/********************************************************************************************

>	virtual BOOL MakeBitmapExportOptions::CopyFrom(BitmapExportOptions *pSource) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Stefan code
	Created:	2/7/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
BOOL MakeBitmapExportOptions::CopyFrom(BitmapExportOptions *pSource) 
{
	// copy the base class first
	if (!MaskedFilterExportOptions::CopyFrom(pSource))
		return FALSE;

	// must be the same class, otherwise the base class  function above returns FALSE
	MakeBitmapExportOptions *pOther = (MakeBitmapExportOptions *)pSource;

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL MakeBitmapExportOptions::Equal(BitmapExportOptions *pSource) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from Stefan code
	Created:	2/7/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Compares the contents of this and pOther objects
	Returns:	TRUE, if objects are equal, FALSE otherwise
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
/*
BOOL MakeBitmapExportOptions::Equal(BitmapExportOptions *pSource) 
{
	BOOL ok = TRUE;

	// compare the base classes first
	ok = MaskedFilterExportOptions::Equal(pSource);

	// must be the same class, otherwise the base class  function above returns FALSE
	MakeBitmapExportOptions *pOther = (MakeBitmapExportOptions *)pSource;

	// Compare any variables unique to our class here
	return ok;
}
*/

/********************************************************************************************

>	virtual BOOL MakeBitmapExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BitmapExportInfo for interface details: gets GIF specific preferences

********************************************************************************************/
BOOL MakeBitmapExportOptions::RetrieveDefaults()
{
	BOOL ok = FALSE;

	if (MaskedFilterExportOptions::RetrieveDefaults())
	{
		SetDPI(g_Dpi);
		SetDepth(g_Depth);
		SetDither(g_Dither);
		ok = TRUE;
	}

	return ok;
}


/********************************************************************************************

>	virtual BOOL MakeBitmapExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation to set GIF specific info as defaults
	See Also:	BitmapExportInfo::SetAsDefaults()

********************************************************************************************/
BOOL MakeBitmapExportOptions::SetAsDefaults() const
{
	BOOL ok = FALSE;

	if (MaskedFilterExportOptions::SetAsDefaults())
	{
		g_Dpi			= GetDPI();
		g_Depth			= GetDepth();
		g_Dither		= GetDither();
		g_TranspBkGnd	= WantTransparent();
		ok = TRUE;
	}

	return ok;
}


//-----------------------------------------------------------------------------------------------
// MakeBitmapFilter - Dummy filter, used to render the selection into a bitmap

/********************************************************************************************

>	MakeBitmapFilter::MakeBitmapFilter()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Constructor for an MakeBitmapFilter object.  The object should be initialised
				before use.
	SeeAlso:	MakeBitmapFilter::Init

********************************************************************************************/
MakeBitmapFilter::MakeBitmapFilter() : TI_GIFFilter ()
{
	pTheBitmap = NULL;
}

/********************************************************************************************

>	static BOOL MakeBitmapFilter::Initialise()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an MakeBitmapFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/
BOOL MakeBitmapFilter::Initialise()
{
	if (!MakeBitmapExportOptions::Declare())
		return FALSE;

	if (Camelot.DeclareSection("Filters", 10))
	{
		Camelot.DeclarePref( NULL, "DoTwoColourRounding", 
							 &GRenderOptPalette::DoTwoColourRoundingToPrimary, FALSE, TRUE );
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL MakeBitmapFilter::DoCreateBitmap(Operation *pOp, Document *pDoc, KernelBitmap** ppBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Exports the current selection as a bitmap, via the virtual fns of the
				inherited class.
	Returns:	TRUE if worked, FALSE if failed.
	SeeAlso:	GetExportOptions; PrepareToExport; ExportRenderNodes; CleanUpAfterExport;

********************************************************************************************/
BOOL MakeBitmapFilter::DoCreateBitmap(Operation *pOp, Document *pDoc, KernelBitmap** ppBitmap)
{
	ERROR3IF(ppBitmap == NULL, "NULL bitmap pointer passed to MakeBitmapFilter::DoCreateBitmap");
	if (ppBitmap == NULL)
		return FALSE;

	pTheBitmap = NULL;
	*ppBitmap = NULL;

	// Set the bitmap pointer to null just in case, usually only used by DoExportBitmap
	pExportBitmap = NULL;

	// Get pointer to the spread to export.
	pSpread = GetFirstSpread(pDoc);

	// remember the document in the class variable
	TheDocument = pDoc;

	// We must now check if there is a selection present so that we can set up whether the
	// user gets the choice of exporting the selection, drawing or spread if there is a 
	// selection present OR just a choice between the spread or drawing if no selection is
	// present.
	// If have a caret selected in a text story then the selection will be almost zero so trap
	// this case as well. 
	RangeControl rg = GetApplication()->FindSelection()->GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	GetApplication()->FindSelection()->Range::SetRangeControl(rg);
	SelRange Rng(*(GetApplication()->FindSelection()));

	// now, run through the selection selecting all nodes under all compound nodes
	// if we don't do this then all compound nodes aren't rendered correctly with transparent
	// bitmaps
	Node * pNode = Rng.FindFirst(FALSE);

	while (pNode)
	{
		pNode->SetSelected(FALSE);
		pNode->SetSelected(TRUE);
		pNode = Rng.FindNext(pNode, FALSE);
	}

	rg.PromoteToParent = FALSE;
	GetApplication()->FindSelection()->Range::SetRangeControl(rg);
	GetApplication()->UpdateSelection();

	DocRect ClipRect = GetApplication()->FindSelection()->GetBoundingRect();
	SelectionType Selection = DRAWING;
 	if ( ClipRect.IsEmpty() || ClipRect.Width() < MinExportSize ||
		 ClipRect.Height() < MinExportSize)
		Selection = DRAWING;		// no selection present, so choose drawing by default
	else
		Selection = SELECTION;		// selection present, so choose this by default

	if (Selection != SELECTION)
	{
		BOOL UseDrawingBounds = TRUE;
		
		SelRange* pSel = GetApplication()->FindSelection();
		if (pSel && pSel->Count()==1)
		{
			// Only one thing selected ... Is it the Text Caret per chance ?
			Node* pSelNode = pSel->FindFirst();
			if (pSelNode && pSelNode->IsAVisibleTextNode())
			{
				VisibleTextNode* pTextNode = (VisibleTextNode*)pSelNode;
				if (pTextNode->IsACaret())
				{
				 	// Aha! It's the Caret that's selected.
					// We'll use the bounds of the parent text line instead then ...
					Node* pTextLine = pTextNode->FindParent();
					ERROR3IF(!IS_A(pTextLine, TextLine), "Caret doesn't have a parent text line in DoCreateBitmap");
					
					// Get the bounds of the text line
					ClipRect = ((TextLine*)pTextLine)->GetBoundingRect();				

					Selection = SELECTION;
					UseDrawingBounds = FALSE;
				}		 		
			}
		}

		if (UseDrawingBounds)
		{
			// Work out the size of the rectangle encompassing the drawing (visible layers only)
			ClipRect = GetSizeOfDrawing(pSpread);
		}
	}

	// Create somewhere to put the user options and fill them up
	MakeBitmapExportOptions* pMakeBmpExportOptions = (MakeBitmapExportOptions*)CreateExportOptions();
	if (pMakeBmpExportOptions == NULL)
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		return FALSE;
	}

	pMakeBmpExportOptions->RetrieveDefaults();
	pMakeBmpExportOptions->SetDepth(32); // create bmp copies shoulda always default to being 32 bit
	pMakeBmpExportOptions->SetSelectionType(Selection);

	// Set the BaseBitmapFilter member so we can use base class functionality - blurghh
	SetExportOptions(pMakeBmpExportOptions);

	BOOL ok = GetExportOptions( (BitmapExportOptions*)pMakeBmpExportOptions );

	BOOL IsAnimation = BmapPrevDlg::m_pExportOptions == NULL;
	BaseBitmapFilter * pNewFilter = this;
	BitmapExportOptions * pExportOptions = BmapPrevDlg::m_pExportOptions;
	if (!IsAnimation)
	{
		pNewFilter = pExportOptions->FindBitmapFilterForTheseExportOptions();
		// from the exporting point of view we should always have been using png options
		// set them to the png options for the export part
		if (pNewFilter)
			pNewFilter->SetExportOptions(pExportOptions);
		// gets the real export options that the prevdlg has set up for us
		BmapPrevDlg::m_pExportOptions = NULL; // take responsibility for this data
	}
	else
	{
		// the animation export didn't bring up the dlg so never set BmapPrevDlg::m_pExportOptions
		// so continue with these options
		pExportOptions = (BitmapExportOptions*)pMakeBmpExportOptions;
	}

	SetExportOptions(pExportOptions);
	

	if (!ok)
	{
		SetExportOptions(NULL); // deletes the filter ptr and nulls it
		delete pExportOptions;
		return FALSE;									// if cancelled
	}


//////////////// SMFIX

	pExportOptions->MarkValid();
	SetDepthToRender(pExportOptions->GetDepth());

	// Set up device context and render region for this export.
	// This will show a progress hourglass for the objects being rendered
	// THis will now also write the data out to file via our ExportRenderNodes function
	if (!PrepareToExport(pSpread, pExportOptions->GetDepth(), pExportOptions->GetDPI(), 
							pExportOptions->GetSelectionType(), pExportOptions->GetDither()))
	{
		pNewFilter->SetExportOptions(NULL);
		SetExportOptions(NULL);
		delete pExportOptions;
		CleanUpAfterExport();
		return FALSE;
	}

	RenderInStrips = FALSE;
	
	if (!ExportRender(ExportRegion))
	{
		pNewFilter->SetExportOptions(NULL); // deletes the filter ptr and nulls it
		SetExportOptions(NULL);
		delete pExportOptions;
		CleanUpAfterExport();
		return FALSE;
	}

	// Now get the converted 32Bit BMP
	pTheBitmap = GetTheBitmap(pExportOptions);

	// if we`ve got a transparency index AND we`ve got a 32 Bit Render region with alpha channel info
	// then we can quickly do
	if(pExportOptions->GetDepth() <= 8 && pExportOptions->GetTransparencyIndex() != -1 && pTheBitmap)
	{
		// What we do now is to go throught the bitmap setting the relavent pixels to transparent
		// depending on the alpha channel info held in the 32 bit version.
		if(!ApplyTransparentColoursToBitmap(pTheBitmap,pExportOptions))
		{
			pNewFilter->SetExportOptions(NULL); // deletes the filter ptr and nulls it
			return FALSE;
		}

		// Now make sure the bitmap knows that it has a transparent colour!
		pTheBitmap->SetTransparencyIndex(pExportOptions->GetTransparencyIndex());
	}

	// Set pointer to the bitmap we have created
	*ppBitmap = pTheBitmap;

	pNewFilter->SetExportOptions(NULL); // deletes the filter ptr and nulls it
	SetExportOptions(NULL);
	delete pExportOptions;
	CleanUpAfterExport();

	return *ppBitmap != NULL;

//////////////// SMFIX
}

/********************************************************************************************

>	virtual KernelBitmap* MakeBitmapFilter::GetTheBitmap ()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/96
	Purpose:	Creates a kernel bitmap from the Export RenderRegion
	Returns:	Ptr to the bitmap, or NULL if error

********************************************************************************************/
KernelBitmap* MakeBitmapFilter::GetTheBitmap(BitmapExportOptions *pExportOptions)
{
	// SMFIX dont use member vars of the class to store stuff like the palette
	// take all the info from the export options ptr
	OILBitmap* pOilBitmap = ExportRegion->ExtractBitmapCopy ( pExportOptions->GetLogicalPalette(), pDestBMInfo,
															  pDestBMBytes, pExportOptions->GetTransparencyIndex());
	if (pOilBitmap == NULL)
	{
		CleanUpAfterExport ();
		return NULL;
	}

	// Err... Lets make sure that the DPI is set up correctly form the options!
	LPBITMAPINFOHEADER pInfoHeader = pOilBitmap->GetBitmapInfoHeader();
	if(pInfoHeader)
	{
		INT32 NewPPM = (INT32)((10000.0 * pExportOptions->GetDPI()) / 254.0);
		pInfoHeader->biXPelsPerMeter = NewPPM;
		pInfoHeader->biYPelsPerMeter = NewPPM;
	}

	// Make a kernel bitmap from the oily one
	return KernelBitmap::MakeKernelBitmap ( pOilBitmap );
}

/********************************************************************************************

>	BOOL MakeBitmapFilter::InsertBitmapIntoDocument(UndoableOperation *pOp, KernelBitmap* KernelBmp, Document* DestDoc)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Exports the current selection as a bitmap, via the virtual fns of the
				inherited class.
	Returns:	TRUE if worked, FALSE if failed.
	SeeAlso:	GetExportOptions; PrepareToExport; ExportRenderNodes; CleanUpAfterExport;

********************************************************************************************/
BOOL MakeBitmapFilter::InsertBitmapIntoDocument(UndoableOperation *pOp, KernelBitmap* KernelBmp, Document* DestDoc)
{
	Spread *pSpread;
	DocCoord Origin;

	// Remember the selection rect, before we change it
	DocRect SelRect = GetApplication()->FindSelection()->GetBoundingRect();

	// For now, position Draw objects on 1st page of spread 1
	pSpread = GetFirstSpread(DestDoc);
	Page *pPage = (Page *) pSpread->FindFirstPageInSpread();
	ERROR3IF(!pPage->IsKindOf(CC_RUNTIME_CLASS(Page)),
		   "MakeBitmapFilter::InsertBitmapIntoDocument: Could not find first Page");

	// Use bottom left of page as origin
	DocRect PageRect = pPage->GetPageRect();
	Origin.x = PageRect.lo.x;
	Origin.y = PageRect.hi.y;

	// Get a new NodeBitmap object to import into.
	NodeBitmap *pNodeBitmap = new NodeBitmap;
	if ((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12)))
		return FALSE;

	// Attach the Imported Bitmap to our Node
	pNodeBitmap->GetBitmapRef()->Attach(KernelBmp, DestDoc); //GetDocument());
	if (pNodeBitmap->GetBitmap() != KernelBmp)
		delete KernelBmp; // It didn't use the bitmap we gave it, so we can delete it

	// Import worked - try to add the bitmap object into the tree.
	// First, set the rectangle to the right size for the bitmap...
	BitmapInfo Info;
	pNodeBitmap->GetBitmap()->ActualBitmap->GetInfo(&Info);

	DocRect BoundsRect;
	BoundsRect.lo = Origin;
	BoundsRect.hi.x = BoundsRect.lo.x + Info.RecommendedWidth;
	BoundsRect.hi.y = BoundsRect.lo.y + Info.RecommendedHeight;

	// And set this in our bitmap node
	pNodeBitmap->CreateShape(BoundsRect);

	// Make sure that there is a layer to put the bitmap onto
	if (!MakeSureLayerExists(DestDoc))
	{
		// There is no layer and one could not be made, so we will have to fail
		delete pNodeBitmap;
		return FALSE;
	}

	// Set the default attrs
	// This MUST be done before the NodeBitmap is Inserted into the tree
	if (!pNodeBitmap->ApplyDefaultBitmapAttrs(pOp))
		return FALSE;

	// Insert the node, but don't invalidate its region
	if (!pOp->DoInsertNewNode(pNodeBitmap, pSpread, FALSE))
	{
		// It didn't work - delete the sub-tree we just created, and report error.
		delete pNodeBitmap;
		return FALSE;
	}

	// bitmap is currently positioned so its bottom left hand
	// corner is at the top left of the page

	// By default we'll move it down so the top-left of the bitmap is on the top-left of the page
	INT32 XTranslate = 0;
	INT32 YTranslate = -Info.RecommendedHeight;

	ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();

	if (ClickMods.Adjust && !SelRect.IsEmpty())
	{
		// If shift is down, then we'll try and place the bitmap exactly on top of the selection
		DocCoord SelectionCentre(SelRect.lo.x + (SelRect.Width()/2), SelRect.lo.y + (SelRect.Height()/2));

		XTranslate = SelectionCentre.x - Origin.x - (Info.RecommendedWidth/2);
		YTranslate = SelectionCentre.y - Origin.y - (Info.RecommendedHeight/2);
	}
	else
	{
		// Otherwise we'll try and centre it within the current view
		Spread* pCurrentSpread;
		DocCoord ViewCentre;

		if (FindCentreInsertionPosition(&pCurrentSpread, &ViewCentre))
		{
/*	Karim 19/01/2000
 *	Ensure that the bmp is pixel-aligned as close to the centre as possible.
 *
			XTranslate = ViewCentre.x - Origin.x - (Info.RecommendedWidth/2);
			YTranslate = ViewCentre.y - Origin.y - (Info.RecommendedHeight/2);
/**/
			DocCoord PixAlignBmpOrigin(	ViewCentre.x - Info.RecommendedWidth/2,
										ViewCentre.y - Info.RecommendedHeight/2 );
			INT32			FullDPI		= GRenderRegion::GetDefaultDPI();
			MILLIPOINT	PixWidth	= (INT32)((IN_MP_VAL / (double)FullDPI) + 0.5);
			PixAlignBmpOrigin.x -= PixAlignBmpOrigin.x % PixWidth;
			PixAlignBmpOrigin.y -= PixAlignBmpOrigin.y % PixWidth;
			XTranslate = PixAlignBmpOrigin.x - Origin.x;
			YTranslate = PixAlignBmpOrigin.y - Origin.y;
		}
	}

	Trans2DMatrix Xlate(XTranslate, YTranslate);
	pNodeBitmap->Transform(Xlate);

	// Ensure Sel Bounds are correct after translation
	GetApplication()->UpdateSelection();

	return TRUE;
}

/********************************************************************************************

>	BOOL MakeBitmapFilter::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (from Simon)
	Created:	12/6/96
	Inputs:		-
	Outputs:	Spread:  The spread to place the clipboard objects on
				Position:The centre of the view (Spread coords)
	Purpose:	Finds the centre insertion position for clipboard objects

********************************************************************************************/
BOOL MakeBitmapFilter::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)
{
	// ---------------------------------------------------------------------------------
	// Find out which spread is in the centre of the view 
	// this is the spread that the pasted objects will be placed on

	// Obtain the current DocView
	DocView* CurDocView = DocView::GetCurrent();

	ENSURE(CurDocView != NULL, "The current DocView is NULL"); 
	if (CurDocView == NULL)
		return FALSE; // No DocView

	// Get the view rect
	WorkRect WrkViewRect = CurDocView->GetViewRect();

	if (WrkViewRect.IsEmpty() || (!WrkViewRect.IsValid()) )
		return FALSE; // Defensive
	
	// Determine the centre of the view
	WorkCoord WrkCentreOfView; 
	WrkCentreOfView.x = WrkViewRect.lox	+ (WrkViewRect.Width()/2); 
	WrkCentreOfView.y = WrkViewRect.loy	+ (WrkViewRect.Height()/2);
	
	// FindEnclosing spread requires an OilCoord
	OilCoord OilCentreOfView = WrkCentreOfView.ToOil(CurDocView->GetScrollOffsets()); 

	// Find out which spread to insert the pasteboard objects onto
	(*Spread) = CurDocView->FindEnclosingSpread(OilCentreOfView);
	if ((*Spread) == NULL)
		return FALSE; // There is no spread

	// Phew
	// ---------------------------------------------------------------------------------
	// Now lets find the spread coordinate of the centre of the view
	DocRect DocViewRect = CurDocView->GetDocViewRect(*Spread);
	
	if ( DocViewRect.IsEmpty() || (!DocViewRect.IsValid()) )
	{
		ERROR3("DocViewRect is invalid");
		return FALSE; // Defensive
	}

	// Find the centre of the DocViewRect
   	DocCoord DocCentreOfView; 
	DocCentreOfView.x = DocViewRect.lox	+ (DocViewRect.Width()/2); 
	DocCentreOfView.y = DocViewRect.loy	+ (DocViewRect.Height()/2);

	// --------------------------------------------------------------------------------
	// Now convert from DocCoords to spread coords

	DocRect PhysSpreadRect = (*Spread)->GetPasteboardRect();
	
	(*Position).x = DocCentreOfView.x - PhysSpreadRect.lo.x; 
	(*Position).y = DocCentreOfView.y - PhysSpreadRect.lo.y;
	
	return TRUE;  
}

/********************************************************************************************

>	virtual void MakeBitmapFilter::CleanUpAfterExport()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Purpose:	Cleans up the memory allocated at the end of Exporting or when exporting has
				been aborted for some reason. Does its cleaning up and then calls the
				baseclass version to do its stuff,  - used
				when the import process ends, either normally or abnormally. Override if
				extra things are required.
	SeeAlso:	BaseBitmapFilter::PrepareToExport(); BaseBitmapFilter::CleanUpAfterExport();
	Scope: 		Protected

********************************************************************************************/
void MakeBitmapFilter::CleanUpAfterExport()
{
	// Call the baseclass version to do its stuff
	MaskedFilter::CleanUpAfterExport();
}

/********************************************************************************************

>	virtual BOOL MakeBitmapFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/96
	Purpose:	See BaseBitmapFilter for interface details

********************************************************************************************/
BOOL MakeBitmapFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "pOptions NULL");
	ERROR3IF(!pOptions->IS_KIND_OF(MakeBitmapExportOptions), "pOptions isn't");

	BOOL Ok = FALSE;

	OpDescriptor* pOpDes = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFTABDLG);

	if (pOpDes != NULL)
	{
		// Graeme (18-9-00)
		// We need to ensure that the correct file type is set up. The bitmap copy filter
		// uses the PNG settings, and so it's necessary to convince the dialogue that we
		// are exporting as a PNG to avoid relics from an earlier export to confuse matters.
		BmapPrevDlg::m_pthExport.SetType ( String_256 ( "png" ) );

		// set up the data for the export options dialog
		OpParam Param((INT32)pOptions, (INT32)this);

		// invoke the dialog
		pOpDes->Invoke(&Param);

		// SMFIX
		// we have brought the dlg up so get the options from the dlg as the graphic type may have changed
		pOptions = BmapPrevDlg::m_pExportOptions;
		Ok = BmapPrevDlg::m_bClickedOnExport;
	}
	else
	{	
		ERROR3("Unable to find OPTOKEN_BMAPPREVDLG");
	} 

	// Return whether or not it worked.
	return Ok;
}

/********************************************************************************************

>	virtual UINT32 MakeBitmapFilter::GetExportMsgID()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Overides the baseclass form of the function so that during the two stage
				export process it can change the message.
	SeeAlso:	DoExport;

********************************************************************************************/
UINT32 MakeBitmapFilter::GetExportMsgID()
{
	if (GeneratingOptimisedPalette())
		return _R(IDS_GENOPTPALMSGID);			// "Generating optimised palette..."
	else
		return _R(IDS_MAKEBITMAPMSGID);			// "Creating bitmap ..."
}

/********************************************************************************************

>	virtual BOOL MakeBitmapFilter::WriteToFile( BOOL End )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		End is TRUE if this is the last block of the file.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
				NOTE - ONLY COPES WITH End=TRUE currently
				Most of the work now is down by WriteDataToFile as this is a static form
				and there is a similar version in the Accusoft DLL. This can therefore be
				used to pass work onto the Accusoft DLL, for the cases we don't cope with. 
				End is ignored now and should always be set to TRUE.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/
BOOL MakeBitmapFilter::WriteToFile( BOOL End )
{
	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL MakeBitmapFilter::EndWriteToFile( )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Purpose:	Cleans up after writing the bitmap data out to a file. Inherited classes
				override this to write in different file formats.
				This is slightly different to most other bitmap filters in that it is here
				that the data actually gets written out to file, after doing the transparency
				translation, if required.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/
BOOL MakeBitmapFilter::EndWriteToFile( )
{
	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL MakeBitmapFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpConvertToBitmap::DoWithParam;

********************************************************************************************/
BOOL MakeBitmapFilter::IsThisBppOk(UINT32 Bpp)
{
	return (Bpp == 24 || Bpp == 8 || Bpp == 4 || Bpp == 1);
}

/********************************************************************************************

>	virtual BitmapExportOptions* MakeBitmapFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BaseBitmapFilter for details
	Notes:		Provides a MakeBitmapExportOptions class

********************************************************************************************/
BitmapExportOptions* MakeBitmapFilter::CreateExportOptions() const
{
	MakeBitmapExportOptions* pExportOptions = new MakeBitmapExportOptions(MAKE_BITMAP_FILTER, &FilterName);
	return (BitmapExportOptions*)pExportOptions;
}
