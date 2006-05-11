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
// The Native file format for Camelot first version.
// This should be the true all singing replacement for the bodge EPS form.

/*
*/

#include "camtypes.h"
#include "xpfilter.h"
//#include "xpfres.h"
#include "xpoilflt.h"
#include "xpfrgn.h"
#include "xpfcaps.h"
#include "cxftags.h"
#include "chapter.h"
#include "spread.h"
//#include "filtrres.h"
#include "nodedoc.h"
#include "progress.h"

#include "camprocess.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(PluginNativeFilter, CamelotNativeFilter);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	PluginNativeFilter::PluginNativeFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/96
	Purpose:	Constructor for the Native save/load filter.

********************************************************************************************/

PluginNativeFilter::PluginNativeFilter()
{
	m_pNewTree = NULL;
	m_ProgressOffset = 0;
	m_bSaveXPEBitmaps = TRUE;	// For now we will default this to saving out the bitmaps
	m_BoundsLevel = BWL_NONE;
	m_BitmapCount = 0;
}




/********************************************************************************************

>	PluginNativeFilter::~PluginNativeFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/96
	Purpose:	Tidies up when the filter is destroyed.

********************************************************************************************/

PluginNativeFilter::~PluginNativeFilter()
{
}

/********************************************************************************************

>	BOOL PluginNativeFilter::Init(const CLSID& rCLSID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Initalises the Filter ready for use. Will fail if it can not get enough
				memory to work with.

********************************************************************************************/

BOOL PluginNativeFilter::Init( xmlNode* pFilterNode )
{
	// Get the OILFilter object
	PluginOILFilter* pPluginOILFilter = new PluginOILFilter(this);
	pOILFilter = pPluginOILFilter;
	if (pOILFilter == NULL)
		return FALSE;

	if (!pPluginOILFilter->Init( pFilterNode ))
		return(FALSE);

	Flags.CanImport = pPluginOILFilter->IsImport();
	Flags.CanExport = pPluginOILFilter->IsExport();

	// Load the description strings
	FilterName = pPluginOILFilter->FilterName;
	FilterInfo = _T("Temp Info String");

	BOOL ok = CreateRecordHandlers();

/*	if (Camelot.DeclareSection("Filters", 10))
	{
		// Preference to turn native file compression on or off
		Camelot.DeclarePref( NULL, "CompressV2Format", &BaseCamelotFilter::CompressNative, 0, 1 );
		// Preference to turn xpe bitmap saving on or off
		Camelot.DeclarePref( NULL, "SaveXPEBitmaps", &BaseCamelotFilter::SaveXPEBitmaps, 0, 1 );
		// Preference to decide what bitmap format to use for preview bitmaps 
		Camelot.DeclarePref( NULL, "PreviewBitmapFilterType", &BaseCamelotFilter::PreviewBitmapFilterType, 0, 4 );
		// Preference for the optional export as web pathname when native saving
		Camelot.DeclarePref( NULL, "DefaultWebExportFilterPath", &BaseCamelotFilter::DefaultExportPath);
		// Preference to turn native file checking for similar paths on or off
		Camelot.DeclarePref( NULL, "NativeCheckSimilarPaths", &BaseCamelotFilter::NativeCheckSimilarPaths, 0, 1 );
	}*/

	return ok;
}


/********************************************************************************************

>	virtual INT32 PluginNativeFilter::HowCompatible( PathName& Filename, ADDR  HeaderStart,
													UINT32 HeaderSize,
													UINT32 FileSize )
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/2005
	Inputs:		FileName	The name of the file being imported
				HeaderStart	The address of the start of the data from the file which needs checking
				HeaderSize	The size of this data that we should check
				FileSize	The total size of the file
	Returns:	10 if the file is fully recognised or 0 otherwise
	Purpose:	returns value in range 0 to 10 which indicates how much we like this file:
					0 => "Definitely Not"
					10 => "Yes Sir, that's my file format"

********************************************************************************************/

INT32 PluginNativeFilter::HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
						   UINT32 FileSize )
{
	// We ignore the passed in buffer and just pass the filename to the external object
	INT32 HowCompatible = 0;

	// If we don't have a filename then don't bother for now	
	String_256 sFilename = Filename.GetPath();
	if (!sFilename.IsEmpty())
	{
		PluginOILFilter* pPluginOILFilter = (PluginOILFilter*)pOILFilter;
		HowCompatible = pPluginOILFilter->HowCompatible(Filename);
	}

	// Return the found value to the caller.
	TRACEUSER( "Gerry", _T("PluginNativeFilter::HowCompatible returning = %d\n"), HowCompatible);
	return HowCompatible;
}



/********************************************************************************************

>	virtual BOOL PluginNativeFilter::DoImport(SelOperation* pOp, CCLexFile* pFile, Document* pDestDoc, 
					  BOOL AutoChosen, ImportPosition* pPos,
					  KernelBitmap** ppImportedBitmap,
					  DocCoord* pPosTranslate, String_256* URL)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/2005
	Inputs:		Op - pointer to the operation that this input process is associated with.
				pFile - The file that we should load the document from
				DestDoc - The Document object which should hold the data read in from
						  the file.
				AutoChosen - 
				pPos - 
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
	Returns:	TRUE if the input operation worked ok, FALSE if not.
	Purpose:	Read the data from the specified file. 
	Errors:		Fails (returns FALSE) if the document structure is incorrect, or if there
				is a problem with the file.
				Assumes the caller will fail and end the operation if False is returned.

********************************************************************************************/

BOOL PluginNativeFilter::DoImport(SelOperation* pOp, CCLexFile* pFile, Document* pDestDoc, 
								 BOOL AutoChosen, ImportPosition* pPos,
								 KernelBitmap** ppImportedBitmap,
								 DocCoord* pPosTranslate, String_256* URL)
{
	ERROR2IF(FALSE, pOp == NULL,"PluginNativeFilter::DoImport null operation!");
	ERROR2IF(FALSE, pFile == NULL,"PluginNativeFilter::DoImport null file!");
	ERROR2IF(FALSE, pDestDoc == NULL,"PluginNativeFilter::DoImport null document!");

	TRACEUSER( "Gerry", _T("PluginNativeFilter::DoImport"));

	PluginOILFilter* pPluginOILFilter = (PluginOILFilter*)pOILFilter;

	// Make sure the OIL filter gets cleaned up on exit
	AutoCleanOILFilter TidyFilter(pPluginOILFilter);

	// Call OIL layer to create a new CCLexFile derived object containing the 
	// translated data
	CCLexFile* pNewFile = NULL;
	if (!pPluginOILFilter->GetImportFile(pFile, &pNewFile))
	{
		TRACEUSER( "Gerry", _T("Failed to get import file\n"));
		return(FALSE);
	}

	// Make sure the new file object gets deleted on exit
	AutoDeleteCCObject TidyFile(pNewFile);

	TRACEUSER( "Gerry", _T("Calling CamelotNativeFilter::DoImport\n"));

	// Call the baseclass DoImport method passing the new file
	if (!CamelotNativeFilter::DoImport(pOp, pNewFile, pDestDoc, AutoChosen, pPos, ppImportedBitmap, pPosTranslate, URL))
	{
		TRACEUSER( "Gerry", _T("Failed in baseclass DoImport\n"));
		return(FALSE);
	}

	return(TRUE);
}


/********************************************************************************************

>	virtual BOOL PluginNativeFilter::DoExport(Operation *pOp, CCLexFile* pFile,
											   PathName * pPath, Document *pDoc)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/2005
	Inputs:		pOp - the operation that started the export off
				pFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
				ShowOptions - Are the export options needed?
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Exports the current document out to file. Uses the virtual functions of this
				class. Do not override unless really necessary.
	SeeAlso:	GetExportOptions; PrepareToExport; CleanUpAfterExport;

********************************************************************************************/

BOOL PluginNativeFilter::DoExport ( Operation* pOp, CCLexFile* pFile, PathName* pPath,
								   Document* pDoc, BOOL ShowOptions )
{
	ERROR2IF(pOp == NULL, FALSE,"PluginNativeFilter::DoExport no export operation");
	ERROR2IF(pFile == NULL, FALSE,"PluginNativeFilter::DoExport no file to export to");
	ERROR2IF(pPath == NULL, FALSE,"PluginNativeFilter::DoExport no export pathname");
	ERROR2IF(pDoc == NULL, FALSE,"PluginNativeFilter::DoExport no document to export");

	// Set up document pointer in the baseclass
	// This is required so that GetExportNode will work
	TheDocument = pDoc;

	PluginOILFilter* pPluginOILFilter = (PluginOILFilter*)pOILFilter;

	// Make sure the OIL filter gets cleaned up on exit
	AutoCleanOILFilter TidyFilter(pPluginOILFilter);

	// Call OIL layer to get a new CCLexFile derived object that the Xar data
	// must be exported to
	CCLexFile* pNewFile = NULL;
	if (!pPluginOILFilter->GetExportFile(pPath, &pNewFile))
	{
		TRACEUSER( "Gerry", _T("Failed to get an output file\n"));
		return(FALSE);
	}

	// Make sure the new file object gets deleted on exit
	AutoDeleteCCObject TidyFile(pNewFile);

	CapabilityTree PlugCaps;

	// Call OIL layer to set up the capabilities control object
	if (!pPluginOILFilter->GetCapabilities(pFile, pPath, &PlugCaps))
	{
		TRACEUSER( "Gerry", _T("Failed to get capabilities object\n"));
		return(FALSE);
	}

	// Make sure the bitmap list is empty
	m_BitmapList.DeleteAll();

	// Update our bounds write level
	m_BoundsLevel = PlugCaps.GetBoundsLevel();

	KernelBitmap::SetCreateTracker(&m_BitmapList);

	// Convert the document according to the capabilites
	BOOL ok = GenerateExportData(&PlugCaps);

	KernelBitmap::SetCreateTracker(NULL);

	if (!ok)
	{
		TRACEUSER( "Gerry", _T("Failed to generate export data\n"));

		// Clean up any partial tree we have created
		if (m_pNewTree)
		{
			m_pNewTree->CascadeDelete();
			delete m_pNewTree;
			m_pNewTree = NULL;
		}
		return(FALSE);
	}

	// Reset the max progress bar limit to an approximation of the size of the Xar data
	SetTotalProgressBarCount(256*1024);

	ForceStatusExportMessage(_R(IDS_SAVING_XAR_DATA));

	// Call the baseclass DoExport method passing the new file object
	// This will call the GetExportData virtual function to get the 
	// document tree to be exported
	ok = CamelotNativeFilter::DoExport(pOp, pNewFile, pPath, pDoc, FALSE);

	UnforceStatusExportMessage();

	// Now cleanup the copy of the document
	if (m_pNewTree)
	{
		m_pNewTree->CascadeDelete();
		delete m_pNewTree;
		m_pNewTree = NULL;
	}

	// Now we must clean up the bitmap list
	// First we remove the head item
	KernelBmpListItem* pItem = (KernelBmpListItem*)m_BitmapList.RemoveHead();
	while (pItem)
	{
		// Delete the KernelBitmap to remove it from the document's list
		// and remove the reference on the OILBitmap
		if (pItem->m_pBmp)
			delete pItem->m_pBmp;

		// Delete the ListItem
		delete pItem;

		// Remove the next item from the list
		pItem = (KernelBmpListItem*)m_BitmapList.RemoveHead();
	}

	// Free up any unused bitmaps in the global list
	// (just deleting the KernelBitmaps doesn't seem to do it)
	Camelot.GetGlobalBitmapList()->DeleteAllUnusedBitmaps();

	// Make sure this is closed
	if (pNewFile->isOpen())
		pNewFile->close();

	if (!ok)
	{
		TRACEUSER( "Gerry", _T("Failed in baseclass DoExport\n"));
		return(FALSE);
	}

	String_64 Str(_R(IDS_EXPORTING_DOCUMENT));
	StartProgressBar(&Str);
	SetTotalProgressBarCount(100);

	// Call OIL layer to do the translation
	if (!pPluginOILFilter->DoExport(pNewFile, pPath))
	{
		TRACEUSER( "Gerry", _T("Failed to translate exported data\n"));
		return(FALSE);
	}

	EndProgressBar();

	return(TRUE);
}


BOOL PluginNativeFilter::GenerateExportData(CapabilityTree* pPlugCaps)
{
	BOOL ok = TRUE;

	// Reset the count for GetNewBitmapName
	m_BitmapCount = 0;

	// Disable bitmap caching but remember the old state so we can set it back
	ScopedFlagSetter fsBitmapCacheing(NodeRenderableBounded::bEnableCacheing, FALSE);

	String_64 Str(_R(IDS_CONVERTING_DOCUMENT));
	StartProgressBar(&Str);
	SetTotalProgressBarCount(500);
	m_ProgressOffset = 0;

//	UINT32 ThisPassCount = 0;
//	UINT32 ThisPassTotal = 0;
//	UINT32 NextPassCount = 0;

	// First we create a copy of the chapter node
	Node* pChapter = CamelotNativeFilter::GetExportNode();
	ERROR2IF(!IS_A(pChapter, Chapter), FALSE, "Export node is not a Chapter");

	Node* pParent = pChapter->FindParent();
	ERROR2IF(!IS_A(pParent, NodeDocument), FALSE, "Parent is not a NodeDocument");

	// Copy the NodeDocument as the root of our new tree
	m_pNewTree = pParent->PublicCopy();

	// Copy all of the children upto pChapter
	Node* pNode = pParent->FindFirstChild();
	while (pNode && pNode != pChapter)
	{
		pNode->CopyNode(m_pNewTree, LASTCHILD);
		pNode = pNode->FindNext();
	}

	Node* pNewChapter = pChapter->PublicCopy();
	if (pNewChapter)
		pNewChapter->AttachNode(m_pNewTree, LASTCHILD);

	XPFView* pXPFView = new XPFView();
	if (!pXPFView || !pXPFView->Init(pPlugCaps->GetRasteriseDPI()))
	{
		TRACEUSER( "Gerry", _T("Failed to create or init XPFView\n"));
		return(FALSE);
	}

	// Remember the current view and set the new view as current
	View* pOldView = View::GetCurrent();
	if (!pXPFView->SetCurrent())
	{
		TRACEUSER( "Gerry", _T("Failed to set XPFView as current\n"));
		return(FALSE);
	}

	View *pView = View::GetCurrent();
	if (pView)
	{
		// Must force background rendering off as otherwise we might get a partly
		// rendered drawing or object if it is complex.
//		ForceBackgroundRedrawOff(pView);
	}
	else
	{
		ERROR2(FALSE, "PluginNativeFilter::GenerateExportData no current view");
	}

	// Then we loop through each spread
//	BOOL bFirstSpread = TRUE;
	Node* pChild = pChapter->FindFirstChild();
	while (pChild)
	{
		if (IS_A(pChild, Spread))
		{
			Spread* pSpread = (Spread*)pChild;
//			Spread* pNewSpread = NULL;

			// If spreads are being converted to bitmap then render a bitmap
			// and build a NodeBitmap to represent it

			// If spreads are being done natively then handle via XPFRenderRegion

			// Create and set up a new XPFRenderRegion
			XPFRenderRegion XPFRegion(this, pPlugCaps);

			// Attach a device to the scanning render region
			// Since this rr does no real rendering, it does not need a Device context
			XPFRegion.AttachDevice(pView, NULL, pSpread);

			// Get it ready to render
//			XPFRegion.SetMatrix(ViewTrans);
//			XPFRegion.SetClipRect(ClipRect);
			
			// Start the render region and return if it fails
			if (XPFRegion.StartRender())
			{			
				XPFRenderCallback XPFCallback(this, &XPFRegion, pPlugCaps, 1);
				XPFCallback.SetAttachContext(pNewChapter, LASTCHILD);

				// Call RenderTree to do the rendering
				XPFRegion.RenderTree(pSpread, FALSE, FALSE, &XPFCallback);

				// Thats all the nodes rendered, so stop rendering
				XPFRegion.StopRender();
			}
		}
		else
		{
			ok = pChild->CopyNode(pNewChapter, LASTCHILD);
		}

		// Get the next child node of the chapter
		pChild = pChild->FindNext();
	}

	if (ok)
	{
		// Now handle conversion pass 2 for the stroked conversion type
		m_ProgressOffset = 100;
		SetProgressBarCount(0);
		ok = DoConversionPassN(pPlugCaps, 2);
	}

	if (ok)
	{
		// Now handle conversion pass 3 for the bitmapfill conversion type
		m_ProgressOffset = 200;
		SetProgressBarCount(0);
		ok = DoConversionPassN(pPlugCaps, 3);
	}

	if (ok)
	{
		// Now handle conversion pass 4 for the bitmap conversion type
		m_ProgressOffset = 300;
		SetProgressBarCount(0);
		ok = DoConversionPassN(pPlugCaps, 4);
	}

	if (ok)
	{
		// Now handle conversion pass 5 for the bitmapspan conversion type
		m_ProgressOffset = 400;
		SetProgressBarCount(0);
		ok = DoConversionPassN(pPlugCaps, 5);
	}

	SetProgressBarCount(100);

	EndProgressBar();					// Kill progess bar

	// Reset this so that the progress bar works for the rest of the export
	m_ProgressOffset = 0;

	if (pOldView)
		pOldView->SetCurrent();

	delete pXPFView;

	return(ok);
}


BOOL PluginNativeFilter::DoConversionPassN(CapabilityTree* pPlugCaps, INT32 ConvertPass)
{
	TRACE( _T("DoConversionPassN(%d)\n"), ConvertPass);

	// Run a render loop and get the render callback to 
	// check the attrs in the render region for each renderable object 
	// and add any that require the bitmapfill conversion 

	View *pView = View::GetCurrent();
	ERROR2IF(pView == NULL, FALSE, "No current DocView in DoConversionPassN");

	Node* pChapter = m_pNewTree->FindFirstChild(CC_RUNTIME_CLASS(Chapter));
	ERROR2IF(pChapter == NULL, FALSE, "No chapter node in DoConversionPassN");

	// Loop through each spread
	Node* pChild = pChapter->FindFirstChild();
	while (pChild)
	{
		if (IS_A(pChild, Spread))
		{
			Spread* pSpread = (Spread*)pChild;

			// Create and set up a new XPFRenderRegion
			XPFRenderRegion XPFRegion(this, pPlugCaps);

			// Attach a device to the scanning render region
			// Since this rr does no real rendering, it does not need a Device context
			XPFRegion.AttachDevice(pView, NULL, pSpread);

			// Start the render region and return if it fails
			if (XPFRegion.StartRender())
			{			
				// Create a render callback object to do the tricky stuff
				XPFRenderCallback XPFCallback(this, &XPFRegion, pPlugCaps, ConvertPass);

				// Call RenderTree to do the rendering
				XPFRegion.RenderTree(pSpread, FALSE, FALSE, &XPFCallback);

				// Thats all the nodes rendered, so stop rendering
				XPFRegion.StopRender();

				// Now we need to convert all the nodes in the list
				if (!XPFCallback.ConvertNodes())
				{
					return(FALSE);
				}
			}
			else
			{
				ERROR2(FALSE, "StartRender failed");
			}
		}

		// Get the next child node of the chapter
		pChild = pChild->FindNext();
	}

	return(TRUE);
}


/****************************************************************************

>	virtual Node* PluginNativeFilter::GetExportNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/2005

	Returns:	NULL in times of grief
	Purpose:	Returns a pointer to the first node to export

****************************************************************************/

Node* PluginNativeFilter::GetExportNode()
{
	if (m_pNewTree)
	{
		Node* pChapter = m_pNewTree->FindFirstChild(CC_RUNTIME_CLASS(Chapter));
		if (pChapter)
			return(pChapter);
	}

	return(CamelotNativeFilter::GetExportNode());
}


/****************************************************************************

>	BOOL PluginNativeFilter::GetExportOptions( WebPrefsDlgParam *pPrefs )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/2005

	Inputs:		pPrefs		- pointer to a WebPrefsDlgParam 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Overrides base class so we can do our own thing

****************************************************************************/

BOOL PluginNativeFilter::GetExportOptions( WebPrefsDlgParam *pPrefs )
{
	// May want to call PrepareExport here
	
	return TRUE;
}


/********************************************************************************************

>	virtual INT32 PluginNativeFilter::GetBitmapCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the bitmap compression.
	Purpose:	Public access to the current bitmap compression which the user has requested
				to use when saving bitmaps in the web format.
				0 means maximum compression e.g. use JPEG filter at 0% lossy compression
				100 means minimum compression e.g. use JPEG filter at 100% lossy compression
				101 - 200 means lossless compression e.g. us PNG filter.
				Could use the no bitmap compression at all e.g. use BMP filter. Unused at present.
				(Very much like the JPEG compression percentage).
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

INT32 PluginNativeFilter::GetBitmapCompression()
{
	// Always use a lossless format such as PNG in native files
	return 200;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::GetPreviewBitmapExport()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the export preview bitmap.
	Purpose:	Public access to the current export preview bitmap which the user has requested
				to use when saving bitmaps in the web format.
				True means export a preview bitmap, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL PluginNativeFilter::GetPreviewBitmapExport()
{
	// Return TRUE if the plugin wants a preview bitmap and FALSE otherwise

	// For now I'll just return FALSE
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::GetConvertTextToOutlines()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the convert text to outlines.
	Purpose:	Public access to the current convert text to outlines which the user has requested
				to use when saving in the web format.
				True means convert text to outlines, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL PluginNativeFilter::GetConvertTextToOutlines()
{
	// In native files ensure text is not converted to outlines
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::GetRemoveInvisibleLayers()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the remove invisible layers.
	Purpose:	Public access to the current remove invisible layers which the user has requested
				to use when saving in the web format.
				True means remove invisible layers, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL PluginNativeFilter::GetRemoveInvisibleLayers()
{
	// In native files retain inivisible layers
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::GetRemoveUnusedColours()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the remove unused colours.
	Purpose:	Public access to the current remove unused colours which the user has requested
				to use when saving in the web format.
				True means remove unused colours, False means do not.
	Note:		This overrides the baseclass version to save in the proper native state

********************************************************************************************/

BOOL PluginNativeFilter::GetRemoveUnusedColours()
{
	// In native files retain unused colours
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::GetSaveXPEBitmaps()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/05
	Inputs:		-
	Returns:	The current state of the save XPE bitmaps.
	Purpose:	

********************************************************************************************/

BOOL PluginNativeFilter::GetSaveXPEBitmaps()
{
	// In native files retain unused colours
	return m_bSaveXPEBitmaps;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::SetSaveXPEBitmaps(BOOL NewSaveXPEBitmaps)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/05
	Inputs:		NewSaveXPEBitmaps	- New value
	Returns:	The previous state of the save XPE bitmaps.
	Purpose:	

********************************************************************************************/

BOOL PluginNativeFilter::SetSaveXPEBitmaps(BOOL NewSaveXPEBitmaps)
{
	BOOL bOld = m_bSaveXPEBitmaps;
	m_bSaveXPEBitmaps = NewSaveXPEBitmaps;
	return bOld;
}

/********************************************************************************************

>	virtual BaseCamelotFilter* PluginNativeFilter::CreateNULLFilter(Document* pDocument)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		pDocument = ptr to the document
	Returns:	ptr to a filter that can be used to count the number of exported bytes
				NULL if there's an error
	Purpose:	Creates the relevant NULL filter
	Scope: 		Protected.

********************************************************************************************/

//BaseCamelotFilter* PluginNativeFilter::CreateNULLFilter(Document* pDocument)
//{
//	return NULL;
//}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::BeginDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called just before document-related records are written out
				to the file.

				The native filter writes out a TAG_DOCUMENT record to signify the start of
				the document.

	SeeAlso:	CamelotNativeFilter::EndDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL PluginNativeFilter::BeginDocumentExport()
{
	BOOL ok = TRUE;

	// Writes out a document rec followed by a down rec
	// This will ensure that all doc-related records appear as a child of the doc record
	if (ok) ok = WriteZeroSizedRecord(TAG_DOCUMENT);
	if (ok) ok = WriteZeroSizedRecord(TAG_DOWN);

	return ok;
}

/********************************************************************************************

>	virtual BOOL PluginNativeFilter::EndDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called after document-related records have been are written out
				to the file.  BeginDocumentExport() is called before the doc records are saved

				This should be called for each document that is saved out to the file.

	SeeAlso:	CamelotNativeFilter::BeginDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL PluginNativeFilter::EndDocumentExport()
{
	BOOL ok = TRUE;

	// Writes out an up rec
	// This will ensure that all doc-related records appear as a child of the doc record
	// by matching the down rec written out by BeginDocumentExport()
	if (ok) ok = WriteZeroSizedRecord(TAG_UP);

	return ok;
}

INT32 PluginNativeFilter::HowCompatibleIsFileType(char* pFileType)
{
	if (strcmp(pFileType,GetExportFileType()) == 0)
		return 10;
	else
		return 9;
}

char* PluginNativeFilter::GetExportFileType()
{
	return EXPORT_FILETYPE_NATIVE;
}

double PluginNativeFilter::GetSimilarPathTolerance(NodePath* pPath)
{
	return 0.0;
}

/********************************************************************************************

>	INT32 PluginNativeFilter::GetMaxPathLookUp()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/96
	Inputs:		-
	Returns:	100 always
	Purpose:	The max number pf paths that should be looked at by the function FindSimilarPath().

	SeeAlso:	BaseCamelotFilter::GetMaxPathLookUp;
	Scope: 		Protected

********************************************************************************************/

INT32 PluginNativeFilter::GetMaxPathLookUp()
{
	return -1;//100;
}




/********************************************************************************************

>	BOOL PluginNativeFilter::WriteCurrentAttributes()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if worked, FALSE otherwise
	Purpose:	Write out all current attributes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL PluginNativeFilter::WriteCurrentAttributes()
{
	BOOL ok = TRUE;
	
	if (ok && TheDocument)
	{
		AttributeManager* pAttrMgr = &(TheDocument->GetAttributeMgr());
		if (pAttrMgr)
			ok = pAttrMgr->WriteCurrentAttributes(this);
	}

	return (ok);
}


BOOL PluginNativeFilter::SetProgressBarCount(UINT32 n)
{
//	TRACE( _T("PluginNativeFilter::SetProgressBarCount(%d)\n"), n);
	
	ProgressBarCount = 	m_ProgressOffset + n;

	if (pProgress != NULL && TotalProgressBarCount > 0)
	{
		if (ProgressBarCount > TotalProgressBarCount)
			ProgressBarCount = TotalProgressBarCount;

		INT32 Percentage = INT32((ProgressBarCount*100) / TotalProgressBarCount);
//		TRACE( _T("Setting progress to %d\n"), Percentage);
		EscapePressed = !pProgress->Update(Percentage);
		return !EscapePressed;
	}

	return TRUE;
}


String_256 PluginNativeFilter::GetNewBitmapName()
{
	String_256 Str;
	Str._MakeMsg(_T("%d"), ++m_BitmapCount);
	return(Str);
}
