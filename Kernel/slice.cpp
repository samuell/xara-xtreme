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
// code to perform image slicing when selected from the menu

#include "camtypes.h"

//#include "barsdlgs.h"		//For _R(IDD_BARCONTROLSTORE)
//#include "resimmap.h"		//For _R(IDS_HTMLIMPORT_FILEDOWNLOAD)
#include "slice.h"

#include "ngcore.h"
#include "ngitem.h"

#include "xshelpid.h"		//For the help ID
//#include "helppath.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "simon.h"			// the string r%dc%d is in here
#include "progress.h"
#include "osrndrgn.h"

#include "giffiltr.h"		// for TI_GIFFilter
#include "bmpfiltr.h"
#include "pngfiltr.h"
#include "extfilts.h"

#include "exjpeg.h"			// jpeg export options

#include "page.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "mrhbits.h"		// for the bitmap render in to test if a slice is empty
#include "selall.h"			// for the select all op in the test if a slice is empty

#include "slicehelper.h"
//#include "sliceres.h"

#include "bmapprev.h"		// for setting the export type in the preview

#include "ngprop.h"
#include "ngsentry.h"

#include "webattr.h"		// for the URL attrib

// for the use of wix temple attribs
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"
#include "tmpltatr.h"

#include "desnotes.h"
#include <io.h>				// for _taccess()
#include "helpuser.h"		// for the SetNextMsgHelpContext function
//#include "andy.h"			//For _R(IDM_OVERWRITE)
//#include "resource.h"		//For _R(IDS_CANCEL)

#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodetext.h"

//#include "mario.h"			// For _R(IDE_NOMORE_MEMORY) - Matt 12/11/2000


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(OpSlice, OpMenuImport);

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	CSlice::CSlice()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Returns:	
	Purpose:	Creates a slice object either empty or pre-filled
	Errors:		-

********************************************************************************************/
CSlice::CSlice()
{
	top = bottom = left = right = 0;
	row = colm = -1;
	rowspan = colmspan = 1;
	rowheight = colmwidth = 0;
	name.Empty();
	FileTypeExtStr = "gif";
	deleteme = FALSE;
	IsNamedSlice = FALSE;
	ButtonNumber = 0;
	IsEmpty = FALSE;
	HasURL = FALSE;
	pFrameText = NULL;
	ExistsOnLayerState[0] = 0;
	ExistsOnLayerState[1] = 0;
	ExistsOnLayerState[2] = 0;
	ExistsOnLayerState[3] = 0;
}

CSlice::CSlice(INT32 Ax, INT32 Ay, INT32 Cx, INT32 Cy, String_256 ThisName, String_16 FileExt, BOOL Named)
{
	top = min(Ay, Cy);
	left = min(Ax, Cx);
	right = max(Ax, Cx);
	bottom = max(Ay, Cy);
	name = ThisName;
	row = colm = -1;
	rowspan = colmspan = 1;
	rowheight = colmwidth = 0;
	FileTypeExtStr = FileExt;
	deleteme = FALSE;
	IsNamedSlice = Named;
	ButtonNumber = 0;
	IsEmpty = FALSE;
	HasURL = FALSE;
	pFrameText = NULL;
	ExistsOnLayerState[0] = 0;
	ExistsOnLayerState[1] = 0;
	ExistsOnLayerState[2] = 0;
	ExistsOnLayerState[3] = 0;
}

/********************************************************************************************

>	void CSlice::SwapSpreadAndSliceCoords()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Returns:	
	Purpose:	Reverses the y-axis to convert between spread co-ords and slice co-ords
				Call it again to swap back between co-ord systems.
	Errors:		-

********************************************************************************************/
void CSlice::SwapSpreadAndSliceCoords()
{
	INT32 temp = top;
	top = -bottom;
	bottom = -temp;
}

/********************************************************************************************

>	BOOL OpSlice::Init()

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares the OpDescriptor
	Errors:		-

********************************************************************************************/
BOOL OpSlice::Init()
{
	return RegisterOpDescriptor(
			0, 							// Tool ID
			_R(IDS_IMAGESLICE),					// String resource ID
			CC_RUNTIME_CLASS(OpSlice), 	// Runtime class
			OPTOKEN_IMAGESLICE,			// Token string
			GetState,					// GetState function
			_R(IDH_Command_Import_from_Web),// help ID GTODO: Is this needed?
			_R(IDBBL_IMAGESLICE),			// bubble help
			_R(IDD_BARCONTROLSTORE),		// resource ID
			_R(IDC_IMAGESLICE),				// control ID
			SYSTEMBAR_FILE,				// Bar ID
			TRUE,						// Receive system messages
			FALSE,						// Smart duplicate operation
			TRUE,						// Clean operation
			0,		// String for one copy only error
			(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 		);
	
}

/********************************************************************************************

>	void OpSlice::Do(OpDescriptor*)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Purpose:	Exports the current document as slices defined by named selections
	Errors:		-

********************************************************************************************/
void OpSlice::Do(OpDescriptor*)
{
	// there needs to be a spread
	Spread* pSelSpread = Document::GetSelectedSpread();
	if (pSelSpread == NULL)
		return;

	//  Let's set a variable in BmapPrevDlg which will be used to determine
	//  which of the tab controls should be enabled or disabled.
	BmapPrevDlg::m_bSlicingImage = TRUE;

	m_ErrorStr = "none";
	m_HTMLext = "htm";

	m_NumberOfButtons = 0;

	// init the export path - which probably isn't needed
	// but better safe than sorry etc...
	// set to .non so that any slices that are defined from this can be
	// found and redefined later. see UpdateSliceFileExts() - sjk
	m_PathName.SetPathName("c:\\untitled.non");

	m_InjectHTML = FALSE; // don't inject without thinking about it - you know it makes sense!

	m_ExportedHTMLOK = TRUE; // asumes the best :-)

	// start a slow job
	Progress LongJob;
	LongJob.Start();

	// test if we use design notes
	m_UsesDesignNotes = UsesDesignNotes();

	// calculate the background colour
	DocColour dcol = Page::GetPageColour();
	m_lRed = m_lGreen = m_lBlue = 255;

	BOOL HasBitmapBackground = FALSE;

	Layer * pLayer = pSelSpread->FindFirstPageBackgroundLayer();
	if (pLayer)
	{
		Node * pNode = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(AttrFlatColourFill));
		if (pNode)
			dcol = *(((AttrFlatFill *)pNode)->GetStartColour());
		else
		{
			if (SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(AttrBitmapColourFill)))
				HasBitmapBackground = TRUE;
		}
	}

	dcol.GetRGBValue(&m_lRed, &m_lGreen, &m_lBlue);

	
	// calculate the bounds of the entire image
	// init the mosaic list which stores all the parts of the image

	CList <CSlice *, CSlice *> MosaicList;
	MosaicList.RemoveAll();

	// the bounds of the drawing
	DocRect SpreadBounds = BaseBitmapFilter::GetSizeOfDrawing(pSelSpread); // returns in spread co-ords ignoring silly layers
	
	// get the selection
	Range Sel(*(GetApplication()->FindSelection()));

	// set the range flags so it includes shadow and bevel manager nodes
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);
	BOOL UseWholeDrawing = TRUE;
	
	// get the size of the selection
	// if there is a selection use this as the area to export rather than the drawing area
	// which we will use if there is no selection made
	if (!Sel.IsEmpty())
	{
		SpreadBounds = Sel.GetBoundingRect();
		UseWholeDrawing = FALSE;
	}

	PixelAlignedInflate(&SpreadBounds); // pixel align the starting rect

	// find the defined slices
	NameGallery* pNameGallery = NameGallery::Instance();
	SGUsedNames* pNames = pNameGallery?NameGallery->GetUsedNames():NULL;

	if (!pNames)
	{
		InformWarning( _R(IDS_NOSLICES) );
		//  Reset:
		BmapPrevDlg::m_bSlicingImage = FALSE;
		return;
	}

	// make sure we are dealing with the most uptodate information
	pNameGallery->FastUpdateNamedSetSizes();

	// for each defined slice in the name gallery
	BOOL NoProblems = TRUE;
	BOOL AddedASlice = FALSE;
	BOOL OutsideSpread = FALSE;

	INT32 Attempts = 0; // first try with the large rects then if that fails try the smaller ones
	String_256 strName(TEXT("Empty"));
	String_256 Slice1ErrorStr = "";
	String_256 Slice2ErrorStr = "";
	INT32 SlicesDefinedInDrawing = 0;

	do
	{
		NoProblems = TRUE;
		AddedASlice = FALSE;
		OutsideSpread = FALSE;
		SlicesDefinedInDrawing = 0;

		// the original mosaic piece is the whole bounded image
		// which will be broken up
		TidyMosaicList(&MosaicList);
		MosaicList.AddHead(new CSlice(SpreadBounds.lox, SpreadBounds.loy, SpreadBounds.hix, SpreadBounds.hiy));

		SGNameItem* pItem = (SGNameItem*) pNames->GetChild();

		while (pItem != 0 && NoProblems)
		{
			// is the tick set in the name gallery to use this slice?
			BOOL UseThisSlice = TRUE;
			NamedTickboxProp * pSliceTick = (NamedTickboxProp *) pItem->GetProperty(1); // 1 is slices
			if (pSliceTick)
			{
				UseThisSlice = pSliceTick->GetState();
				if (UseThisSlice)
					SlicesDefinedInDrawing++;
			}

			// dont use an unselected slice if it is not the whole drawing being exported
			if (!UseWholeDrawing && pItem->IsNoneSelected())
				UseThisSlice = FALSE;

			if (UseThisSlice)
			{
				DocRect r;
				r = pItem->GetSetBounds();			// (returned in spread co-ords)

				pItem->GetNameText(&strName);

				// expand for actual text story visible size
				ScanTextStorySliceBounds(strName, r);

				if (Attempts == 0)
				{
					DocRect rLarge = ScanLargeSliceBounds(strName);
					r = r.Union(rLarge); // use the large rects
				}

				// r must be in the spread bounds
				// if it is outside the bounds it should be ignored
				PixelAlignedInflate(&r);		// pixel align the slice

				// trim anything that just strays over the bounds
				r = r.Intersection(SpreadBounds);

				// cut the existing mosaic pieces by the edges of this new piece
				// which of course creates even more pieces
				if (r.IsValid() && r.Height() > 0 && r.Width() > 0)
				{
					NoProblems = NoProblems && AddSlice(r.lox, r.loy, r.hix, r.hiy, &MosaicList, strName);
					AddedASlice = TRUE;
				}
				else
				{
					TRACEUSER( "SimonK", _T("Slice outside bounds of the drawing!!!\n"));
				}
			}

			if (NoProblems) pItem = (SGNameItem*) pItem->GetNext();
		}

		Attempts++;
		if (!NoProblems)
		{
			Slice1ErrorStr = m_ErrorStr;
			Slice2ErrorStr = strName;
		}

	} while (!NoProblems && Attempts < 2); // try twice , first with the max size slices, then with the smaller slices

/* Dont ask the question Asumme that the user will choose to have the shadows cut if they do overlap (sjk 20/12/00)
	if (Attempts == 2 && NoProblems && AddedASlice)
	{	// worked with the smaller rects but not with the larger ones

		// I need to force this message to reappear again if they click 'Help' - Matt 10/11/2000
		INT32 result = 3;
		while (result == 3)
		{
			result = AskQuestion(_R(IDS_SLICES_MAY_OVERLAP), _R(IDS_YES), _R(IDS_NO), _R(IDS_HELP));
			if (result == 2)
			{
				NoProblems = FALSE; // it is a problem if you say it is
				m_ErrorStr = Slice1ErrorStr; // restore the shapes that were causing problems for the first case
				strName = Slice2ErrorStr;
			}
			if (result == 3)
			{
				HelpUserTopic(_R(IDH_Alert_Exclude_peripheral_elements));
			}
		}
	}
*/
	POSITION Test = NULL;

	if (NoProblems && AddedASlice) 
	{
		// sort the mosaic using SortSlices()
		// it also merges small bits back together if it can
		SortSlices(&MosaicList, SpreadBounds);

		// mark which states exist
		m_FoundRolloverStates = ScanForRolloverStates(&MosaicList);

		BOOL ReplaceFiles = TRUE;
		BOOL FilesOverwriten = FALSE;
		do // loop to ask where to save to which may be asked more than once if they dont want
			// to overwrite files
		{

			// ask where to save the files
			// and find out which slice export type to use
			if (!SaveFileDlg())
			{
				// tidy up and leave if cancelled
				TidyMosaicList(&MosaicList);
				Error::ClearError();
				//  Reset:
				BmapPrevDlg::m_bSlicingImage = FALSE;
				return;
			}


			// cope with things being .html, which the BmapPrevDlg doesn't set correctly as its not 8.3
			String_256 temp = m_PathName.GetFileName(FALSE);
			INT32 dot = temp.FindNextChar(TCHAR ('.'));
			if (dot > 0)
			{
				String_256 end = "";
				temp.Split(&temp, &end, dot, FALSE);
				if (end.CompareTo(".html", FALSE) == 0)
				{
					m_HTMLext = "html";
					m_PathName.SetFileName(temp);
				}
			}

			CString ClickedFile(temp);
			ClickedFile += "." + m_HTMLext;

			ReplaceFiles = FileExists(ClickedFile);
			FilesOverwriten = FALSE;

			if (!ReplaceFiles)
			{
				// test if these names we are creating are already in existance
				Test = MosaicList.GetHeadPosition();
				while (Test && !FilesOverwriten)
				{
					CSlice* pTestSlice = MosaicList.GetNext(Test);
					CString TestName (pTestSlice->name);

					// we dont know exactly what file type for each element will be
					// but it is best to warn the user that they may be overwriting their files
					FilesOverwriten = FileExists(TestName + ".gif");
					if (!FilesOverwriten) FilesOverwriten = FileExists(TestName + ".jpg");
					if (!FilesOverwriten) FilesOverwriten = FileExists(TestName + ".jpeg");
					if (!FilesOverwriten) FilesOverwriten = FileExists(TestName + ".png");
					if (!FilesOverwriten) FilesOverwriten = FileExists(TestName + ".bmp");
				}
			}

			if (FilesOverwriten && !ReplaceFiles)
			{
				// ask the user "This folder contains a graphic file with a conflicting name.
				// Do you want to overwrite any conflicting files in this folder?"
				if (InformWarning(_R(IDS_CONFLICTING_SLICE_FILENAMES), _R(IDS_OVERWRITE_GRAPHIC), _R(IDS_SAVE_ELSEWHERE)) == 1)
					ReplaceFiles = TRUE;
			}

		} while (FilesOverwriten && !ReplaceFiles); // end of overwrite loop


		// The preview dlg wants to know the path of the image being
		// exported so give it the m_PathName
		BmapPrevDlg::m_pthExport = m_PathName;

		// set up the export options
		BitmapExportOptions * pExportOptions = NULL;
		BOOL ok = TRUE;

		ShowRolloverLayer(ALL_LAYERS);

		// WARNING SetUpExportOptions() actually 'new's the ExportOptions
		// so you will have to delete them or pass them to a function
		// that deletes them for you
		String_256 strExt = m_PathName.GetType();
		strExt.toLower();
		if (strExt.CompareTo("jpg") == 0)
		{
			JPEGExportFilter f;
			ok = f.SetUpExportOptions(&pExportOptions);
		}
		else
		if (strExt.CompareTo("bmp") == 0)
		{
			BMPFilter f;
			ok = f.SetUpExportOptions(&pExportOptions);
		}
		else
		if (strExt.CompareTo("png") == 0)
		{
			PNGFilter f;
			ok = f.SetUpExportOptions(&pExportOptions);
		}
		else
		//if (strExt == "gif")
		{
			TI_GIFFilter f;
			ok = f.SetUpExportOptions(&pExportOptions);
		}

		// the export options may have changed types
		// so ask the export dlg for the actual export options
		pExportOptions = BmapPrevDlg::m_pExportOptions;
		// take responsibility for the export options away from the bmp preview dlg
		BmapPrevDlg::m_pExportOptions = 0;

		// Cancelled from the export options dlg
		if (!ok)
		{
			if (pExportOptions)
			{
				delete pExportOptions;
			}
			// tidy up and leave
			TidyMosaicList(&MosaicList);
			Error::ClearError();
			//  Reset:
			BmapPrevDlg::m_bSlicingImage = FALSE;
			return;
		}

		// read the m_PathName back since
		// setting up the export options could
		// have changed this value
		m_PathName = BmapPrevDlg::m_pthExport;


		// clear the screen by calling an idle
		(DocView::GetSelected())->ForceRedraw();
		GetApplication()->OnIdle(TRUE);				// Phil 2/7/2004 YUK! What's wrong with ServiceRendering?

		// change the non-user defined slices to use the graphic type
		// just picked by the user in the SaveFileDlg
		UpdateSliceFileExts(&MosaicList);

		// mark any "empty" slices in the list
		// but not if we have a background bitmap
		if (!HasBitmapBackground)
			MarkEmptySlices(&MosaicList, SpreadBounds, pExportOptions);

		// export each section of the mosaic as a seperate graphic
		Test = MosaicList.GetHeadPosition();
		while (Test)
		{
			CSlice* pTestSlice = MosaicList.GetNext(Test);

			if (m_FoundRolloverStates && pTestSlice->IsNamedSlice)
				// export this slice as a rollover
				ExportRollOverSlice(pTestSlice, pExportOptions);
			else
				// do a normal slice
			{
				// only export the graphic for a none-empty slice
				if (!pTestSlice->IsEmpty)
					ExportSliceGraphic(pTestSlice, pExportOptions, pTestSlice->name);
			}

			// This function calls DoExportWithParams() which used to
			// delete the export options (and not zero the pointer - tusk!)
			// Behaviour now changed by using DontLetFilterDeleteMyPointer()
			// so we can keep using the same export options, but of course we
			// must then delete them ourselves.
		}

		// tidy up our generated export options
		if (pExportOptions != 0)
		{
			delete pExportOptions;
			pExportOptions = 0;
		}

		// And clean up the image export options in the bitmap dialogue.
//		BmapPrevDlg::CancelTransparency ();

		// export the HTML to display the mosaic put back together
		// the name is that typed in but with the htm extension

		// build up the list of link names with each button
		// rather than just going on adding a number to the file
		// users want to not have to edit HTML, do it all for them!
		m_UsedShimGraphic = FALSE;
		PathName HTMLPath = m_PathName;
		HTMLPath.SetType(m_HTMLext);
		m_pLinkName = NULL;
		INT32 i = 0;
		String_256 LaunchString = HTMLPath.GetFileName();

		INT32 LinkEntries = max (m_NumberOfButtons, 1);
		m_pLinkName = new String_256[LinkEntries];

		if (m_pLinkName)
		{
			m_pLinkName[0] = HTMLPath.GetPath();

			INT32 FileNumber = 0;
			// fill the link names with default names if there are no URLs found
			for (i = 0; i < m_NumberOfButtons; i++)
			{
				m_pLinkName[i] = m_PathName.GetFileName(FALSE);

				CSlice * pTestSlice = GetButtonNumber (&MosaicList, i+1);
				if (pTestSlice && pTestSlice->ExistsOnLayerState[SELECTED])
				{
					FileNumber++;

					if (m_FoundRolloverStates && m_RolloverState[SELECTED].Exists && FileNumber > 1)
					{
						String_256 NewName;
						NewName.MakeMsg(_R(IDS_HTML_NAME_SPEC), (LPCSTR) m_PathName.GetFileName(FALSE), FileNumber);
						m_pLinkName[i] = NewName;
					}
				}

				m_pLinkName[i] += ".";
				m_pLinkName[i] += m_HTMLext;
			}

			// scan the tree for any URLs used associated with the buttons in the Mosaic list
			// this will overwrite the defaults already set in m_pLinkName
			URLScan(m_pLinkName, &MosaicList);

			FileNumber = 0;

			// export each HTML file required
			if (m_RolloverState[SELECTED].Exists)
				for (i = 0; i < LinkEntries; i++)
				{
					String_256 NewName = m_pLinkName[i];
					String_256 Temp = "";

					// dont try to create a file http://www.stuff...
					NewName.Left(&Temp,5);
					BOOL isURL = (Temp.CompareTo("http:", FALSE) == 0);
					if (!isURL)
					{
						NewName.Right(&Temp,1);
						isURL = (Temp.CompareTo("/") == 0);
					}

					CSlice * pTestSlice = GetButtonNumber (&MosaicList, i+1);
					if (pTestSlice && pTestSlice->ExistsOnLayerState[SELECTED])
					{
						FileNumber++;

						if (isURL)
						{
							NewName.MakeMsg(_R(IDS_HTML_NAME_SPEC), (LPCSTR) m_PathName.GetFileName(FALSE), FileNumber);
							NewName += ".";
							NewName += m_HTMLext;
						}

						HTMLPath.SetPathName(NewName);

						if (!ExportImageSliceHTML(&MosaicList, HTMLPath.GetPath(), i+1))
							FailledToExportHTML (HTMLPath.GetPath());

						// set file to launch if changed by the URL
						if (FileNumber == 1)
						{
							LaunchString = HTMLPath.GetFileName();
						}
					}

				}
			else
				if (!ExportImageSliceHTML(&MosaicList, HTMLPath.GetPath()))
					FailledToExportHTML (HTMLPath.GetPath());

			delete [] m_pLinkName;
			m_pLinkName = NULL;
		}

		// Export out the required shim
		if (m_UsedShimGraphic)
		{
			// also needs shim.gif exported
			// set up the path of where to save it
			PathName ShimPath = m_PathName;

			ShimPath.SetFileName("shim");
			ShimPath.SetType("gif");

			CCDiskFile File;
			if(File.open(ShimPath.GetPath(), (ios::in | ios::out | ios::binary)))
			{
				BYTE buf[] =   {0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x01, 0x00,
								0x01, 0x00, 0x80, 0x00, 0x00, 0xFF, 0xFF, 0xFF,
								0x00, 0x00, 0x00, 0x21, 0xF9, 0x04, 0x01, 0x14,
								0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00,
								0x01, 0x00, 0x01, 0x00, 0x00, 0x02, 0x02, 0x44,
								0x01, 0x00, 0x3B, 0x00	};

				// make the shim of the correct colour
				buf[13] = (BYTE) m_lRed;
				buf[14] = (BYTE) m_lGreen;
				buf[15] = (BYTE) m_lBlue;

				// write the file
				File.write((BYTE *)buf, 44);
				File.close();
			}
		}
		
		// display the export HTML now?
		if (m_ExportedHTMLOK && AskQuestion(_R(IDS_SHOW_HTML_NOW), _R(IDS_YES), _R(IDS_NO)) == 1)
		{
			// shell execute the browser
			HINSTANCE hChild = ShellExecute(HWND_DESKTOP, "open", LaunchString, NULL, NULL ,SW_SHOW);

			// If the function (ShellExecute()) fails, then an error value that is less than or equal to 32 is returned. 
			INT32 Result = (INT32)hChild;
			// problems?
			if (Result <= 32)
			{
				InformWarning(_R(IDS_NO_BROWSER));
			}
		}
	
	}
	else if (!AddedASlice) // user forgot to define any slices
	{
		if (SlicesDefinedInDrawing == 0)
			InformWarning( _R(IDS_NOSLICES) );
		else
			InformWarning( _R(IDS_NOSLICES_SELECTED) );
	}
	else // some error occured such as intersecting named slices
	{
		if (OutsideSpread)
		{
			// An invisible slice is outside the bounds of the rendered spread
			InformWarning(_R(IDS_SLICE_OUT_OF_BOUNDS));
		}
		else
		{
			// should mention that it is pTestSlice->name that is overlapping		
			String_256 temp(TEXT(""));
			temp.MakeMsg(_R(IDS_SLICES_OVERLAP), (LPCTSTR) m_ErrorStr, (LPCTSTR) strName);

			ErrorInfo Info;
			Info.ErrorMsg = 0;
			Info.Button[0] = _R(IDS_OK);
			Info.Button[1] = _R(IDS_HELP);
			Info.OK = 1;
			Info.Help = 2;
			Info.Title = _R(IDBBL_IMAGESLICE);

			BOOL Again;
			do
			{
				// Set the error message
				Error::SetError( 0, temp, 0 );

				Again = FALSE;
				switch (AskQuestion(&Info))
				{
					case _R(IDS_HELP):
						HelpUserTopic(_R(IDS_HELPPATH_Alert_Named_Objects_Overlap));
						Again = TRUE;
						break;
				}
			} while (Again);
		}
	}

	// tidey up before leaving
	TidyMosaicList(&MosaicList);

	// end a slow job
	// done through the progress class being destroyed

	//  Reset:
	BmapPrevDlg::m_bSlicingImage = FALSE;
}

/********************************************************************************************

>	INT32 OpSlice::TidyMosaicList(CList <CSlice *, CSlice *> * pMosaicList)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/99
	Purpose:	Deletes all the slices in the list now that we are done with them.
				So there will be no memory leeks.

********************************************************************************************/
INT32 OpSlice::TidyMosaicList(CList <CSlice *, CSlice *> * pMosaicList)
{
	// tidy the mosaic list by deleting all its entries
	INT32 Pieces = 0;
	POSITION Test = pMosaicList->GetHeadPosition();
	while (Test)
		{
			CSlice * pTestSlice = pMosaicList->GetNext(Test);
			pMosaicList->RemoveHead();
			delete pTestSlice;
			Pieces++;
		}

	return Pieces;
}

/********************************************************************************************

>	void OpSlice::UpdateSliceFileExts(CList <CSlice *, CSlice *> * pMosaicList)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/99
	Purpose:	Changes all non set file types to be the type the user picks from the file dlg.

********************************************************************************************/
void OpSlice::UpdateSliceFileExts(CList <CSlice *, CSlice *> * pMosaicList)
{
	POSITION Test = pMosaicList->GetHeadPosition();
	while (Test)
		{
			CSlice * pTestSlice = pMosaicList->GetNext(Test);
			if (pTestSlice->FileTypeExtStr.CompareTo("non") == 0) // using non as a standard we haven't set this yet
				pTestSlice->FileTypeExtStr = m_PathName.GetType();
		}

}

/********************************************************************************************

>	OpState OpSlice::GetState(String_256*, OpDescriptor*)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/99
	Purpose:	This item is always available, so long as a document is visible.
				Not really sure if I even need this!!!

********************************************************************************************/
OpState OpSlice::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	// if we don't allow it
	OpSt.Greyed = TRUE;
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread && !pSpread->FindActiveLayer()->IsFrame())
		OpSt.Greyed = FALSE;

	return OpSt;
}

/********************************************************************************************

>BOOL OpSlice::AddSlice (INT32 Ax, INT32 Ay, INT32 Cx, INT32 Cy, CList <CSlice *, CSlice *> * pMosaicList, String_256 SliceName)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/99
	Purpose:	
				 slice rect ABCD
				   A---B
				   |   |
				   D---C
				 pMosaicList should start with one entry which is the whole drawing extent
				 that is to be split up into sections.
				 The passed in slice (AC) is added to the list of pieces which is a named user
				 defined slice. Existing pieces that are occupy the same area are cut into
				 smaller parts or absorbed.

	Params:		Axy, Cxy defines the new slice
				pMosaic is this list of the slices
				SliceName is the name of the new slice

	Returns:	TRUE if it worked - FALSE if something failled

********************************************************************************************/	
BOOL OpSlice::AddSlice (INT32 Ax, INT32 Ay, INT32 Cx, INT32 Cy, CList <CSlice *, CSlice *> * pMosaicList, String_256 SliceName)
{
	// ignore empty slices as we could hardly export them
	// but since slices containing nothing could exist we should cater
	// for the possibility
	if (Ax == Cx || Ay == Cy) return TRUE;

	// if the inflate to pixel aligned rect has swapped over two named slices
	// set them to being on the same boundry
	NudgeSliceIfClose(Ax, Ay, Cx, Cy, pMosaicList);

	BOOL ok = TRUE;
	POSITION pos = pMosaicList->GetHeadPosition();

	while (pos)
	{
		POSITION thispos = pos;
		CSlice * pRect = pMosaicList->GetNext(pos);
		BOOL RemoveMe = TRUE;
		
		if (Ax == pRect->left && Cx == pRect->right && Ay == pRect->top && Cy == pRect->bottom)
		{
			// remove existing identical slices
			// keep the name if it has one and the replacement doesn't.
			if (!pRect->name.IsEmpty())
			{
				TRACE( _T("Identically named slices\n"));
				if (SliceName.IsEmpty())
					SliceName = pRect->name;
			}
		}
		else
		if (InRect(pRect, Ax, Ay)) // A in rect
			ok = Slice (pRect, Ax, Ay, Cx, Ay, pMosaicList); // slice AB
		else
		if (InRect(pRect, Cx, Ay)) // B in rect
			ok = Slice (pRect, Ax, Ay, Cx, Ay, pMosaicList); // slice AB
		else
		if (CutsRect(pRect, Ax, Ay, Cx, Ay)) // cuts AB
			ok = Slice (pRect, Ax, Ay, Cx, Ay, pMosaicList); // slice AB
		else
		if (InRect(pRect, Cx, Cy)) // C in rect
			ok = Slice (pRect, Cx, Ay, Cx, Cy, pMosaicList); // slice BC
		else
		if (CutsRect(pRect, Cx, Ay, Cx, Cy)) // cuts BC
			ok = Slice (pRect, Cx, Ay, Cx, Cy, pMosaicList); // slice BC
		else
		if (InRect(pRect, Ax, Cy)) // D in rect
			ok = Slice (pRect, Ax, Ay, Ax, Cy, pMosaicList); // slice AD
		else
		if (CutsRect(pRect, Ax, Ay, Ax, Cy)) // cuts AD
			ok = Slice (pRect, Ax, Ay, Ax, Cy, pMosaicList); // slice AD
		else
		if (CutsRect(pRect, Ax, Cy, Cx, Cy)) // cuts DC
			ok = Slice (pRect, Ax, Cy, Cx, Cy, pMosaicList); // slice DC
		else
		if (pRect->left >= Ax && pRect->right <= Cx && pRect->top >= Ay && pRect->bottom <= Cy) // rect in ABCD
			//remove rect from list
		{
			TRACE( _T("remove sub rect\n"));
			// NB doing nothing will result in it being removed
		}
		else // this piece doesn't interact with the slice at all
			RemoveMe = FALSE; // so don't let it be taken from us

		if (!ok)
			return FALSE; // an error occurred
		
		pos = thispos;
		// reget the next item to check that we haven't extended the list
		pRect = pMosaicList->GetNext(pos);

		if (RemoveMe)
		{
			pMosaicList->RemoveAt(thispos);
			delete pRect;
		}
	}

	// add the slice passed in
	// remember to give it the users individually selected graphic type
	// which isn't yet implimented
	RemoveIlligalFileAndJavaChars(SliceName);
	pMosaicList->AddHead(new CSlice(Ax, Ay, Cx, Cy, SliceName, m_PathName.GetType(), TRUE));

	return ok;
}

/********************************************************************************************

>	BOOL OpSlice::InRect(CSlice * pRect, INT32 x, INT32 y)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/99
	Purpose:	Tests if the point X,Y is in the rectangle defined by the slice
	Returns:	TRUE if x,y is in the rect

********************************************************************************************/
BOOL OpSlice::InRect(CSlice * pRect, INT32 x, INT32 y)
{
	if (x > pRect->left && x < pRect->right
		&& y > pRect->top && y < pRect->bottom)
		return TRUE;
		
	return FALSE;
}

/********************************************************************************************

>	BOOL OpSlice::Slice (CSlice * pRect, INT32 Ax, INT32 Ay, INT32 Bx, INT32 By, CList <CSlice *, CSlice *> * pMosaicList, POSITION pos)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/99
	Purpose:	Split the Slice pRect into smaller rectangles along the line AB
	Returns:	TRUE if it performed a slice
	Errors:		On overlapped named slices

********************************************************************************************/
BOOL OpSlice::Slice (CSlice * pRect, INT32 Ax, INT32 Ay, INT32 Bx, INT32 By, CList <CSlice *, CSlice *> * pMosaicList)
{
	if (!pRect->name.IsEmpty()) // Overlapping named slices
	{
		m_ErrorStr = pRect->name; // set the error string to be the name 
								  // of the offending overlapping one
		return FALSE;
	}

	BOOL ok = FALSE;
	
	if (Ax == Bx) // Vert slice
	{
		pMosaicList->AddTail(new CSlice(pRect->left, pRect->top, Ax, pRect->bottom, "", m_PathName.GetType()));
		pMosaicList->AddTail(new CSlice(Ax, pRect->top, pRect->right, pRect->bottom, "", m_PathName.GetType()));
		ok = TRUE;
	}
	else
	if (Ay == By) // Horiz slice
	{
		pMosaicList->AddTail(new CSlice(pRect->left, pRect->top, pRect->right, Ay, "", m_PathName.GetType()));
		pMosaicList->AddTail(new CSlice(pRect->left, Ay, pRect->right, pRect->bottom, "", m_PathName.GetType()));
		ok = TRUE;
	}
	
	return ok;
}

/********************************************************************************************

>	BOOL OpSlice::CutsRect(CSlice * pRect, INT32 Ax, INT32 Ay, INT32 Bx, INT32 By)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/99
	Purpose:	Tests to see if the line AB cuts the rectangle pRect
	Returns:	TRUE if it does cut

********************************************************************************************/
BOOL OpSlice::CutsRect(CSlice * pRect, INT32 Ax, INT32 Ay, INT32 Bx, INT32 By)
{
	if (Ax == Bx) // Vert cut
	{
		// top & bottom outside rect
		// while the vertical line cuts through the rect
		if (Ay <= pRect->top && By >= pRect->bottom
			&& Ax < pRect->right && Ax > pRect->left)
			return TRUE;
	}
	else
	if (Ay == By) // Horiz cut
	{
		// left & right outside rect
		// while the horizontal line cuts through the rect
		if (Ax <= pRect->left && Bx >= pRect->right
			&& Ay < pRect->bottom && Ay > pRect->top)
			return TRUE;
	}
	
	return FALSE;
}

// moves all edges slightly from one location to another keeping them all in line
void ReAlignEdge ( INT32 OldEdge, INT32 NewEdge, BOOL Horiz, CList <CSlice *, CSlice *> * pMosaicList)
{
	POSITION pos = pMosaicList->GetHeadPosition();

	while (pos)
	{
		CSlice * pRect = pMosaicList->GetNext(pos);

		if (Horiz)
		{
			if (pRect->left == OldEdge)
				pRect->left = NewEdge;
			else
			if (pRect->right == OldEdge)
				pRect->right = NewEdge;
		}
		else
		{
			if (pRect->bottom == OldEdge)
				pRect->bottom = NewEdge;
			else
			if (pRect->top == OldEdge)
				pRect->top = NewEdge;
		}
	}
}

/********************************************************************************************

>	void OpSlice::NudgeSliceIfClose(INT32 &Ax, INT32 &Ay, INT32 &Cx, INT32 &Cy, CList <CSlice *, CSlice *> * pMosaicList)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/99
	Purpose:	Moves the new slice over a bit if it just overlaps with an
				existing slice due to the pixel infation.
				It does this by adjusting the AC rectangle passed in
	Returns:	nothing

********************************************************************************************/
void OpSlice::NudgeSliceIfClose(INT32 &Ax, INT32 &Ay, INT32 &Cx, INT32 &Cy, CList <CSlice *, CSlice *> * pMosaicList)
{
	INT32 OldAx = Ax;
	INT32 OldAy = Ay;
	INT32 OldCx = Cx;
	INT32 OldCy = Cy;

	BOOL done = FALSE;

	// allow how many pixels to be fudged?
	for (INT32 size = 750 ; size <= 1500 && !done ; size += 750)
	{
		POSITION pos = pMosaicList->GetHeadPosition();

		while (pos)
		{
			CSlice * pRect = pMosaicList->GetNext(pos);
			if (!pRect->name.IsEmpty())
			{
				if (pRect->bottom - size == OldAy  && OldCy != pRect->bottom)
				{
					if (size == 750)
					{
						Ay = pRect->bottom;
						done = TRUE;
					}
					else
					{
					// anything in the whole slice list that equals pRect->bottom should be reduced by 750
					// and the same for all the others
						//Ay = pRect->bottom - 750;
						ReAlignEdge ( pRect->bottom, Ay, FALSE, pMosaicList);
						done = TRUE;
					}
				}
				if (pRect->top + size == OldCy && OldAy != pRect->top)
				{
					if (size == 750)
					{
						Cy = pRect->top;
						done = TRUE;
					}
					else
					{
						//Cy = pRect->top + 750;
						ReAlignEdge ( pRect->top, Cy, FALSE, pMosaicList);
						done = TRUE;
					}
				}
				if (pRect->left + size == OldCx && OldAx != pRect->left)
				{
					if (size == 750)
					{
						Cx = pRect->left;
						done = TRUE;
					}
					else
					{
						//Cx = pRect->left + 750;
						ReAlignEdge ( pRect->left, Cx, TRUE, pMosaicList);
						done = TRUE;
					}
				}
				if (pRect->right - size == OldAx && OldCx != pRect->right)
				{
					if (size == 750)
					{
						Ax = pRect->right;
						done = TRUE;
					}
					else
					{
						//Ax = pRect->right - 750;
						ReAlignEdge ( pRect->right, Ax, TRUE, pMosaicList);
						done = TRUE;
					}
				}
			}
		}
	}
}


/********************************************************************************************

>	BOOL OpSlice::SortSlices (CList <CSlice *, CSlice *> * pMosaicList)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/99
	Purpose:	Sorts the slices into order.
				Merges together suitable slices.
				Fills in colm & row positions, spans and width/heights
				for defining an HTML table.
				Gives names to nameless slices in the form r*c*

	Returns:	FALSE if some error occured

********************************************************************************************/
BOOL OpSlice::SortSlices (CList <CSlice *, CSlice *> * pMosaicList, const DocRect & Bounds)
{
	POSITION ToDo = pMosaicList->GetHeadPosition();

	// go to Slice Co-ords from spread co-ords
	while (ToDo)
	{
		CSlice * pTopSlice = pMosaicList->GetNext(ToDo);
		pTopSlice->SwapSpreadAndSliceCoords();
	}

	// sort the Slices into order top left to bottom right

	ToDo = pMosaicList->GetHeadPosition();

	// ToDo moves through the whole mosaic list
	while (ToDo)
	{
		CSlice * pTopSlice = pMosaicList->GetNext(ToDo);
		POSITION Test = ToDo;

		// Test marks the iteration through the unsorted mosaic list
		// Is always between ToDo and the end of the list
		while (Test && !pTopSlice->deleteme)
		{
			CSlice * pTestSlice = pMosaicList->GetNext(Test);

			if (!pTestSlice->deleteme)
			{
				BOOL merge = FALSE;

				// test for merging the two rects together again
				if (pTestSlice->name.IsEmpty() && pTopSlice->name.IsEmpty()) // don't merge selected rects!
				{
					// same height and y position
					if (pTestSlice->top == pTopSlice->top && pTestSlice->bottom == pTopSlice->bottom)
					{
						if (pTestSlice->left == pTopSlice->right)
						{
							pTopSlice->right = pTestSlice->right;
							merge = TRUE;
						}
						else if (pTestSlice->right == pTopSlice->left)
						{
							pTopSlice->left = pTestSlice->left;
							merge = TRUE;
						}
					}
					if (pTestSlice->left == pTopSlice->left && pTestSlice->right == pTopSlice->right)
					{
						if (pTestSlice->top == pTopSlice->bottom)
						{
							pTopSlice->bottom = pTestSlice->bottom;
							merge = TRUE;
						}
						else if (pTestSlice->bottom == pTopSlice->top)
						{
							pTopSlice->top = pTestSlice->top;
							merge = TRUE;
						}
					}
				}
				
				if (merge)
				{
					//TRACE( _T("merged slices\n"));
					// remove the test slice
					// it has been absorbed
					pTestSlice->deleteme = TRUE;
				}
			}
		}
	}

	// delete marked slices
	ToDo = pMosaicList->GetHeadPosition();

	// Remove any slices marked for deletion
	// since they have been merged with another slice
	while (ToDo)
	{
		POSITION Test = ToDo;
		CSlice * pTestSlice = pMosaicList->GetNext(ToDo);
		if (pTestSlice->deleteme)
		{
			pMosaicList->RemoveAt(Test);
			delete pTestSlice;
		}
	}



	// sort the Slices into order top left to bottom right

	ToDo = pMosaicList->GetHeadPosition();

	// TopPosition == ToDo-1 marks the start of the unsorted mosaic list
	while (ToDo)
	{
		POSITION TopPosition = ToDo;
		CSlice * pTopSlice = pMosaicList->GetNext(ToDo);
		POSITION Test = ToDo;


		// LastTest == Test-1 marks the iteration through the unsorted mosaic list
		while (Test)
		{
			POSITION LastTest = Test;
			CSlice * pTestSlice = pMosaicList->GetNext(Test);
			
			// swap the two entries to sort them
			// ordering on the position of the top corner first
			// then if these are equal which is left most goes first
			if (pTestSlice->top < pTopSlice->top || 
			(pTestSlice->top == pTopSlice->top && pTestSlice->left < pTopSlice->left) )
			{
				// swap the order of these two slices in the mosaic list
				pMosaicList->SetAt(LastTest, pTopSlice);
				pMosaicList->SetAt(TopPosition, pTestSlice);

				// change what the top slice is for future compares
				// during the lifetime of pTopSlice
				pTopSlice = pTestSlice;
			}
		}
	}

	/*** WARNING *** Use of INT32_MIN assumes that the Slice rects and the DocRect/Spread coords
					 that defined them are in the range of a INT32.
					 This is a pretty safe asumption though */
	
	// set the row position
	INT32 LowestUnSetRow = INT32_MIN+1, LastLowestUnSetRow = INT32_MIN+1;
	INT32 RowHeight = 0;
	INT32 RowNumber = 1;
	while (LowestUnSetRow != INT32_MIN) 
	{
		// find the lowest unset row
		LowestUnSetRow = INT32_MIN;
		POSITION Test = pMosaicList->GetHeadPosition();
		while (Test)
			{
				CSlice * pTestSlice = pMosaicList->GetNext(Test);
				if (pTestSlice->row == -1 &&
					(LowestUnSetRow == INT32_MIN || pTestSlice->top < LowestUnSetRow )
					)
				{
					LowestUnSetRow = pTestSlice->top;
				}
			}

		// calc the row height
		RowHeight = LowestUnSetRow - LastLowestUnSetRow;
		LastLowestUnSetRow = LowestUnSetRow;

		// set the lowest rows
		Test = pMosaicList->GetHeadPosition();
		while (Test)
			{
				CSlice * pTestSlice = pMosaicList->GetNext(Test);

				// set the row number if it hasn't been set and it is the lowest
				if (pTestSlice->row == -1 && pTestSlice->top == LowestUnSetRow )
				{
					pTestSlice->row = RowNumber;
					pTestSlice->rowheight = RowHeight;
				}

				// calculate the rows used by this slice
				if (pTestSlice->top < LowestUnSetRow && pTestSlice->bottom > LowestUnSetRow /*&& pTestSlice->row != -1*/)
				{
					pTestSlice->rowspan++;
				}
			}
		RowNumber ++;
	}


	// set the colm position	
	INT32 LowestUnSetColm = INT32_MIN + 1, LastLowestUnSetColm = Bounds.lox;
	INT32 ColmWidth = 0;
	INT32 ColmNumber = 1;
	while (LowestUnSetColm != INT32_MIN)
	{
		LowestUnSetColm = INT32_MIN;
		POSITION Test = pMosaicList->GetHeadPosition();

		// find the least left most point that doesn't have a set colm
		while (Test)
			{
				CSlice * pTestSlice = pMosaicList->GetNext(Test);
				if (pTestSlice->colm == -1 &&
					(LowestUnSetColm == INT32_MIN || pTestSlice->left < LowestUnSetColm )
					)
				{
					LowestUnSetColm = pTestSlice->left;
				}
			}

		// calc the colm widths
		// extra bit of fiddling to get the last width by using the 
		// image bounding rect
		if (LowestUnSetColm != INT32_MIN)
			ColmWidth = (LowestUnSetColm - LastLowestUnSetColm);
		else
			ColmWidth = Bounds.hix - LastLowestUnSetColm;

		LastLowestUnSetColm = LowestUnSetColm;

		// Set the colm numbers
		Test = pMosaicList->GetHeadPosition();
		while (Test)
			{
				CSlice * pTestSlice = pMosaicList->GetNext(Test);

				// set the colm number if it hasn't been set and it is the lowest
				if (pTestSlice->colm == -1 && pTestSlice->left == LowestUnSetColm )
				{
					pTestSlice->colm = ColmNumber;
					pTestSlice->colmwidth = ColmWidth;
					
					// set the slice name if it doesn't have one
					String_256 temp = "";
					temp.MakeMsg(_R(IDS_SLICENAME), pTestSlice->row, pTestSlice->colm);
					if (pTestSlice->name.IsEmpty())
						pTestSlice->name = temp;
				}
				
				// calculate the colms used by this slice
				if (pTestSlice->left < LowestUnSetColm && pTestSlice->right > LowestUnSetColm /*&& pTestSlice->colm != -1*/)
				{
					pTestSlice->colmspan++;
				}
			}
		ColmNumber ++;
	}

	
	// go from Slice Co-ords back to spread co-ords
	// so we can happily use a standard co-ord system again other places in Camelot
	m_NumberOfButtons = 0;
	ToDo = pMosaicList->GetHeadPosition();
	while (ToDo)
	{
		CSlice * pTestSlice = pMosaicList->GetNext(ToDo);
		pTestSlice->SwapSpreadAndSliceCoords();

		// and while we are here set the button number
		if (pTestSlice->IsNamedSlice)
		{
			m_NumberOfButtons++;
			pTestSlice->ButtonNumber = m_NumberOfButtons;
		}
	}

	return TRUE;
}

// search the list of slices for the button numbered buttonno and return a ptr to it
CSlice * OpSlice::GetButtonNumber (CList <CSlice *, CSlice *> * pMosaicList, INT32 buttonno)
{
	POSITION ToDo = pMosaicList->GetHeadPosition();
	while (ToDo)
	{
		CSlice * pTestSlice = pMosaicList->GetNext(ToDo);

		// and while we are here set the button number
		if (pTestSlice->IsNamedSlice && pTestSlice->ButtonNumber == buttonno)
		{
			return pTestSlice;
		}
	}

	return NULL;
}

/********************************************************************************************

>BOOL OpSlice::ExportSliceGraphic(CSlice * pSlice)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/99
	Purpose:	Generates a graphic of a section of the whole image defined by the
				rectangular area of the slice.
	Params:		pSlice - the slice to export contains area rect and name
	Returns:	FALSE if some error occured

********************************************************************************************/
BOOL OpSlice::ExportSliceGraphic(CSlice * pSlice, BitmapExportOptions * pExportOptions, const String_256 & GraphicFileName)
{
	// This is the clip rect that is required to be in spread co-ords
	DocRect r(pSlice->left, pSlice->top, pSlice->right, pSlice->bottom);

	// Get the export filter for this slice if
	// it is a named slice and has been exported before in which case get
	// its type and its exportoptions (if the user wishes to use them)
	// and use those instead
	BitmapExportOptions * pOverRideExportOptions = pExportOptions;
	SGNameItem* pItem = SliceHelper::LookupNameGalleryItem(pSlice->name);
	if (!pItem && pSlice->IsNamedSlice)
	{
		// someone may have mundged the slice name due to javascript and filename requirements
		// will have to mundge each gallery name in turn since we didn't find a direct match
		NameGallery * pNameGallery = NameGallery::Instance();
		SGUsedNames* pNames = pNameGallery ? pNameGallery->GetUsedNames() : NULL;
		pItem = pNames ? (SGNameItem*) pNames->GetChild() : NULL;

		BOOL Found = FALSE;

		String_256 str;

		// check all the name gallery items
		while (pItem && !Found)
		{
			pItem->GetNameText(&str);

			// do the mundge
			RemoveIlligalFileAndJavaChars(str);

			// if the name matches its your man
			if (pSlice->name.CompareTo(str) == 0)
				Found = TRUE;
			else
				// no then try the next one?
				pItem = (SGNameItem *) pItem->GetNext();
		}
	}

	if (pItem)
	{
		NodeSetProperty* pPropNode = pItem->GetPropertyNode();
		if (pPropNode)
		{
			NamedExportProp * pProp = (NamedExportProp*) pPropNode->GetProperty(NamedExportProp::nIndex);
			if (pProp && pProp->GetOptions() && !pProp->IsVirgin())
			{	// update the slice to use this name and export type
				pOverRideExportOptions = pProp->GetOptions();

				//PathName TempPath = pOverRideExportOptions->GetPathName();
				//pSlice->FileTypeExtStr = TempPath.GetType();

				// have to use the ptr class type on the options to work out which filter to use (not very good really)
				if (IS_A(pOverRideExportOptions, JPEGExportOptions))
				{
					pSlice->FileTypeExtStr = "jpg";
				}
				else if (IS_A(pOverRideExportOptions, BMPExportOptions))
				{
					pSlice->FileTypeExtStr = "bmp";
				}
				else if (IS_A(pOverRideExportOptions, PNGExportOptions))
				{
					pSlice->FileTypeExtStr = "png";
				}
				else if (IS_A(pOverRideExportOptions, GIFExportOptions))
				{
					pSlice->FileTypeExtStr = "gif";
				}
				else // not a type we can deal with from here
					pOverRideExportOptions = pExportOptions;
			}
		}
	}

	// set up the path of where to save it
	PathName Path = m_PathName;
	Path.SetFileName(GraphicFileName);
	Path.SetType(pSlice->FileTypeExtStr);

	// use screen AA for all image slicing
	pOverRideExportOptions->SetAntiAliasing(MAINTAIN_SCREEN_AA);

	// since we have been messing around with the rect to export and the layers we
	// should assume that the palette we hold is no longer valid
	pOverRideExportOptions->BitmapSourceHasChanged();

	CCDiskFile File;
	if(!File.open(Path.GetPath(), ios::in | ios::out | ios::binary))
		return FALSE;		// error!

	// have to use the ptr class type on the options to work out which filter to use (not very good really)
	if (IS_A(pOverRideExportOptions, JPEGExportOptions))
	{
		JPEGExportFilter ExportFilter;
		ExportFilter.DoExportWithOptions(this, &File, &Path, 
										Document::GetCurrent(),
										(JPEGExportOptions*) pOverRideExportOptions, &r);
	}
	else if (IS_A(pOverRideExportOptions, BMPExportOptions))
	{
		BMPFilter ExportFilter;
		ExportFilter.DoExportWithOptions(this, &File, &Path, 
										Document::GetCurrent(),
										(BMPExportOptions*) pOverRideExportOptions, &r);
	}
	else if (IS_A(pOverRideExportOptions, PNGExportOptions))
	{
		PNGFilter ExportFilter;
		ExportFilter.DoExportWithOptions(this, &File, &Path, 
										Document::GetCurrent(),
										(PNGExportOptions*) pOverRideExportOptions, &r);
	}
	else if (IS_A(pOverRideExportOptions, GIFExportOptions))
	{
		TI_GIFFilter ExportFilter;
		ExportFilter.DoExportWithOptions(this, &File, &Path, 
										Document::GetCurrent(),
										(GIFExportOptions*) pOverRideExportOptions, &r);
	}

	File.close();

	// integrate with Dream Weaver by creating design notes for each exported graphic file
	if (m_UsesDesignNotes && pExportOptions->GetCanUseDesignNotes())
	{
		CString DocName(Document::GetCurrent()->GetLocation());	
		DocName += TEXT("\\") + Document::GetCurrent()->GetPathName();

		if (DocName == TEXT("\\"))
		{
			InformWarning (_R(IDS_NOTES_WARNING));
			m_UsesDesignNotes = FALSE;
		}
		else
		{
			CString GraphicFile(Path.GetPath());

			// create or modify a .mno forward source file
			CreateDesignNoteWithForwardSource(GraphicFile, DocName);
		}
	}


	return TRUE;
}


/********************************************************************************************

>	BOOL OpSlice::ExportImageSliceHTML(CList <CSlice *, CSlice *> * pMosaicList, String_256 HTMLFile)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/7/99
	Purpose:	Creates an HTML file with a table that re-weaves the sliced images back together
				Also if rollovers have been defined they are also incorperated.
	Returns:	FALSE if some error occured
	SideEffect:	On m_UsedShimGraphic to set it correctly - assuming it is FALSE to start with.

********************************************************************************************/
BOOL OpSlice::ExportImageSliceHTML(CList <CSlice *, CSlice *> * pMosaicList, const String_256 & HTMLFile, INT32 FileNumber)
{
	BOOL InjectFile = m_InjectHTML;

	// varibales used in the inject file and reinject file modes
	BYTE * pPreviousHTML = NULL;
	DWORD PreviousHTMLSize = 0;
	BYTE * pHeadEnd = NULL;
	BYTE * pBodyStart = NULL;
	BYTE * pBodyStartAbsolute = NULL;
	BYTE * pNavDefStart = NULL;
	BYTE * pNavDefEnd = NULL;
	BYTE * pNavTableStart = NULL;
	BYTE * pNavTableEnd = NULL;

	// file doesn't exist? then we cant inject!
	CString tempFile(HTMLFile);
	if (InjectFile && !FileExists(tempFile))
		InjectFile = FALSE;

	if (InjectFile)
	{
		// read the previous file into memory and identify important locations within it
		pPreviousHTML = LoadFileIntoMemory(HTMLFile, PreviousHTMLSize);
		if (!pPreviousHTML)
			return FALSE;

		// find where the <head> tag finishes
		pHeadEnd = FindNextOccuranceOf((BYTE *)&"</head>", pPreviousHTML);
		if (!pHeadEnd)
			return FALSE;

		// find the navbar definition by looking for the marker comment
		// this will be in the header part inside the dummy javascript comment
		pNavDefStart = FindNextOccuranceOf((BYTE *)&"<!-- Navbar def -->", pPreviousHTML);
		if (pNavDefStart)
		{
			// find the end of the navbar definition by looking for the marker comment
			pNavDefEnd = FindNextOccuranceOf((BYTE *)&"<!-- Navbar def end -->", pNavDefStart);
			if (pNavDefEnd)
				pNavDefEnd += 23;
		}

		// find where the <body> tag starts & ends
		pBodyStartAbsolute = FindNextOccuranceOf((BYTE *)&"<body", pHeadEnd);
		if (!pBodyStartAbsolute)
			return FALSE;
		pBodyStart = FindNextOccuranceOf((BYTE *)&">", pBodyStartAbsolute);
		if (!pBodyStart)
			return FALSE;

		// the table that holds the navbar is marked by having name tag of "XaraTable"
		// find this and the locate the start of the table tag
		pNavTableStart = FindNextOccuranceOf((BYTE *)&"name=\"XaraTable", pBodyStart);
		if (pNavTableStart)
		{
			BYTE * pTemp = FindPreviousChar('<', pNavTableStart, pBodyStart);
			BYTE * pTemp2 = FindNextOccuranceOf((BYTE *)&"<table", pTemp);

			if (pTemp == pTemp2) // is in start of <table>
			{
				pNavTableStart = pTemp;

				// find the end of the table tag
				pNavTableEnd = FindNextOccuranceOf((BYTE *)&"</table>", pNavTableStart);
				if (pNavTableEnd)
					pNavTableEnd += 8;
			}
			else
				pNavTableStart = NULL;
		}
	}

	// injecting HTML where we have injected before?
	// ie we have found the markers and can just replace these bits of code
	BOOL ReInject = pNavTableEnd ? TRUE : FALSE;

	// open the file to output the HTML
	ofstream f(HTMLFile);
	if ( !f.is_open() )
	{
		return FALSE;
	}

	POSITION pos = 0;

	if (ReInject)
	{
		// write the stuff till the start navbar marker
		if (pNavDefEnd)
			WriteText (f, pPreviousHTML, pNavDefStart - pPreviousHTML);
	}
	else
	{
		if (InjectFile)
		{
			// write everything in the file up to the <\head>
			WriteText(f, pPreviousHTML, pHeadEnd - pPreviousHTML);
		}
		else
		{
			// export HTML
			f << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 transitional//EN\">\n";
			f << "<html>\n<head>\n";
			f << "<title>Sliced Image</title>\n";
		}
	}

	// any javascript images go in here

	if (m_FoundRolloverStates)
	{
		// add the start java button marker
		f << "<!-- Navbar def -->\n";

		f << "<script language=\"JavaScript\">\n";
		f << "<!-- Dummy comment to hide code from non-JavaScript browsers.\n\n";
		f << "if (document.images) {\n";

		// iterate throught the slices for named slices to add their images to the document
		pos = pMosaicList->GetHeadPosition();
		while (pos)
		{
			CSlice * pSlice = pMosaicList->GetNext(pos);

			if (pSlice->IsNamedSlice)
			{
				for (INT32 i = 0; i < 3 ; i++) // not including SELECTED
					if (m_RolloverState[i].Exists && pSlice->ExistsOnLayerState[i])
					{
						f << pSlice->name;
						f << m_RolloverState[i].Name;
						f << " = new Image(); ";
						f << pSlice->name;
						f << m_RolloverState[i].Name;
						f << ".src = \"";
						f << pSlice->name;
						if (i > 0 && m_RolloverState[i].Exists)
							f << m_RolloverState[i].Name;

						f << "." << pSlice->FileTypeExtStr;
						f << "\"\n";
					}
			}
		}
		f << "}\n\n";

		// set up the list of graphics
		for (INT32 n = 0; n < 3; n++) // not including selected
			if (m_RolloverState[n].Exists)
			{
				f << "function turn" << m_RolloverState[n].Name;
				f << "(ImageName) {\n";
				f << "\tif (document.images != null) {\n";
				f << "\t\tdocument[ImageName].src = eval(ImageName + \"";
				f << m_RolloverState[n].Name;
				f << ".src\");\n\t}\n";
				//if (n == CLICKED)
				//	f << "return true;\n";
					
				f << "}\n\n";
			}

		f << "// End of dummy comment -->\n";
		f << "</script>\n";
		// mark the end of the java script buttons def
		f << "<!-- Navbar def end -->\n";

	} // end of m_FoundRolloverStates - adding rollover start javascript


	if (ReInject)
	{
			// write the stuff between the end of the navr bar def and
			// the begining of the table
		if (pNavDefEnd)
			WriteText(f, pNavDefEnd, pNavTableStart - pNavDefEnd);
		else
			// write the stuff from the begining of the old file to the table start marker
			WriteText(f, pPreviousHTML, pNavTableStart - pPreviousHTML);
	}
	else
	{

		f << "</head>\n\n";

		if (InjectFile)
		{
			// recover the exisitng body tag
			WriteText(f, pBodyStartAbsolute, pBodyStart - pBodyStartAbsolute + 1);
		}
		else
		{
			// set body tag something like this
			f << "<body bgcolor=\"";

			char col[8];
			wsprintf(col, "#%02x%02x%02x", (BYTE) m_lRed, (BYTE) m_lGreen, (BYTE) m_lBlue);
			f << col;

			f << "\" text=\"";

			// sort out the text colour
			if (m_lRed + m_lGreen + m_lBlue < 300)
				f << "white";
			else
				f << "black";
				
			// set the rest of the body tag
			f << "\" link=\"red\" vlink=\"blue\" alink=\"yellow\">\n";
		}
	}

	// Sliced images are set into a table
	f << "<table name=\"XaraTable\" border=\"0\" cellpadding=\"0\" cellspacing=\"0\">\n\n";

	BOOL RequireEndOfRowShims = FALSE;
	BOOL RequireShimRow = FALSE;
	pos = pMosaicList->GetHeadPosition();
	while (pos)
	{
		CSlice * pSlice = pMosaicList->GetNext(pos);
		if (pSlice->rowspan > 1)
			RequireEndOfRowShims = TRUE;
		if (pSlice->colmspan > 1)
			RequireShimRow = TRUE;
	}

	// add a shim row if there is any colm spanning
	if (RequireShimRow)
	{
		BOOL AddedShimRow = FALSE;
		pos = pMosaicList->GetHeadPosition();
		INT32 shimcolm = 2;	// they are all out of sinc by one so start at 2!?!
							// never mind they only have to be (nearly) all there and in order
		while (pos)
		{
			CSlice * pSlice = pMosaicList->GetNext(pos);

			if (pSlice->colm == shimcolm)
			{
				// add the shim row if required and we found something to put in it
				if (!AddedShimRow)
				{
					f << "<!-- shim row--><tr>";
					AddedShimRow = TRUE;
				}

				// output the HTML for the shim
				f << "<td><img src=\"shim.gif\" width=\"" << pSlice->colmwidth/750 << "\" height=\"1\" border=\"0\"></td>\n";
				shimcolm++;
				m_UsedShimGraphic = TRUE;
				// start looking for the next colm width from the begining of the list
				// since the x co-ord is not ordered directly through the list
				pos = pMosaicList->GetHeadPosition();
			}
		}

		// if we started the row - end it here
		if (AddedShimRow)
		{
			// add the last tag (unknown width, but then we dont need to know this width
			// just fill the cell in with a shim
			f << "<td><img src=\"shim.gif\"";
			if (RequireEndOfRowShims) // if we have end of row shims expand this shim into the top corner with a colm span of 2
				f << " colspan=\"2\"";
			f << " height=\"1\" border=\"0\"></td>\n</tr>\n\n";
		}
	}

	// loop round all entries in the list
	pos = pMosaicList->GetHeadPosition();
	
	INT32 CurrentRow = 0;

	while (pos)
	{
		CSlice * pSlice = pMosaicList->GetNext(pos);
		
		if (pSlice->row > CurrentRow)
		{
			// finish the last row off
			if (CurrentRow > 0)
			{
				// add a shim for the row height
				if (RequireEndOfRowShims)
				{
					f << "<td><img src=\"shim.gif\" width=\"1\" height=\"" << pSlice->rowheight/750 << "\" border=\"0\"></td>";
					m_UsedShimGraphic = TRUE;
				}

				// finish off the row
				f << "</tr>\n\n";
			}
				
			CurrentRow = pSlice->row;
			// start a new row
			f << "<!-- Row " << CurrentRow << " --><tr>\n";
		}

		// add the table entry for this slice
  		// open the table data entry
  		f << "<td";
		
		// add spanning data
		if (pSlice->rowspan > 1)
			{
			f << " rowspan=\"" << pSlice->rowspan << "\"";
			}
		
		if (pSlice->colmspan > 1)
			{
			f << " colspan=\"" << pSlice->colmspan << "\"";
			}
		// end the table definition start entry
		f << ">";

		// debug data
		//f << "<!-- ";
		//f << "(" << pSlice->top << "," << pSlice->left << "),(" << pSlice->bottom << "," << pSlice->right << ") ";
		//f << " row " << pSlice->row << " colm " << pSlice->colm << " ";
		//f << " --> ";

		// so we know when to close it
		BOOL AddedAnchor = FALSE;
		
		// Add the anchor data if this is a rollover
		// and it is not the selected state button
		if ( (m_FoundRolloverStates || pSlice->HasURL)
			&& pSlice->IsNamedSlice 
			&& (FileNumber != pSlice->ButtonNumber || !m_RolloverState[SELECTED].Exists))
		{
			//<a href="Index.htm" onmouseover="turnmouse('general_info')" onmouseout="turnoff('general_info')" onclick="turnon('general_info')" >
			f << "<a href = \"";
			AddedAnchor = TRUE;

			// link in destination of the link]
			if (m_pLinkName && pSlice->ButtonNumber > 0 && (pSlice->HasURL || pSlice->ExistsOnLayerState[SELECTED]))
			{
				f << m_pLinkName[pSlice->ButtonNumber - 1];
				f << "\"";

				if (pSlice->pFrameText && *(pSlice->pFrameText))
				{
					f << " TARGET=\"";
					f << pSlice->pFrameText;
					f << "\"";
				}
			}
			else
			{
				f << m_PathName.GetFileName(FALSE); // just the name stem of the HTML file please
				f << ".";
				f << m_HTMLext;
				f << "\"";
			}

			if (m_FoundRolloverStates)
			{
				if (m_RolloverState[MOUSE].Exists && pSlice->ExistsOnLayerState[MOUSE])
				{
					// add mouse state
					f << " onmouseover=\"turn";
					f << m_RolloverState[MOUSE].Name;
					f << "('";
					f << pSlice->name;
					f << "')\"";
				}

				if (pSlice->ExistsOnLayerState[DEFAULT])
				{
					// add default state state
					f << " onmouseout=\"turn";
					f << m_RolloverState[DEFAULT].Name;
					f << "('";
					f << pSlice->name;
					f << "')\"";
				}

				if (m_RolloverState[CLICKED].Exists && pSlice->ExistsOnLayerState[CLICKED])
				{
					// add click state
					f << " onmousedown=\"turn";
					f << m_RolloverState[CLICKED].Name;
					f << "('";
					f << pSlice->name;
					f << "')\"";
				}
			}

			f << ">";
		}

		// add the slice image
		f << "<img name=\"";
		f << pSlice->name;
		f << "\" src=\"";
			
		if (!pSlice->IsEmpty)
		{	
			f << pSlice->name;

			// if this is the selected page and the selected button
			// show the selected graphic not the default one
			// by adding the word "selected" after the name
			if (FileNumber == pSlice->ButtonNumber && m_RolloverState[SELECTED].Exists && pSlice->ExistsOnLayerState[SELECTED])
				f << m_RolloverState[SELECTED].Name;

			f << "." + pSlice->FileTypeExtStr;
		}
		else // it was just blank anyway so use the shim instead
		{
			f << "shim.gif";
			m_UsedShimGraphic = TRUE;
		}

		f << "\"";

		// add alternative text
		if (pSlice->IsNamedSlice)
		{
			f << " alt=\"";
			f << TurnUnderScoreIntoSpace(pSlice->name);
			f << "\"";
		}

		// use the matrix to work out the pixel values of height and width
		Matrix Identity;
		DocRect r(pSlice->left, pSlice->top, pSlice->right, pSlice->bottom);
		WinRect	Rect = OSRenderRegion::BitmapDocRectToWin( Identity, r, 96.0 );

		// bitmap width
		f << " width=\"" << Rect.right - Rect.left <<"\"";
		
		// bitmap height
		f << " height=\"" << Rect.bottom - Rect.top <<"\"";
		
		// finish the image tag
		f << " border=\"0\">";
		
		// chose the anchor tag if this is a rollover
		if (AddedAnchor)
		{
			f << "</a>";
		}

		// close the table definition entery
		f << "</td>\n";
	}

	// add a shim to the end of the last row if needed
	if (RequireEndOfRowShims)
	{
		f << "<td><img src=\"shim.gif\" width=\"1\" border=\"0\"></td>";
		m_UsedShimGraphic = TRUE;
	}

	// end the last row
	f << "</tr>\n";
	
	// end the table
	f << "</table>\n";

	if (ReInject)
	{
		// copy the data from the end of the table to the end of the file
		WriteText(f, pNavTableEnd, PreviousHTMLSize - (pNavTableEnd - pPreviousHTML));
	}
	else
	{
		if (InjectFile)
		{
			// write everything from after the <body> tag onwards
			WriteText(f, pBodyStart+1, PreviousHTMLSize - (pBodyStart+1 - pPreviousHTML));
		}
		else
		{
			// close the html file
			f << "<p>\n\n</body>\n</html>\n";
		}
	}

	// close the file
	f.close();
	
	// we are done with the old HTML file in memory
	if (pPreviousHTML)
		delete [] pPreviousHTML;

	return TRUE;
}

/********************************************************************************************

>	void OpSlice::PixelAlignedInflate(DocRect * r)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Purpose:	Infates a DocRect to the nearest pixel aligned boundary

********************************************************************************************/
void OpSlice::PixelAlignedInflate(DocRect * r)
{
	INT32 lox, loy, hix, hiy = 0;
	
	lox = ((r->lox)/750) * 750; // likely round down
	loy = ((r->loy)/750) * 750;
	hix = ((r->hix + 749)/750) * 750; // likely round up
	hiy = ((r->hiy + 749)/750) * 750;

	DocRect newr(lox, loy, hix, hiy);
	*r = newr;
}


/********************************************************************************************

>	BOOL OpSlice::SaveFileDlg()

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/99
	Purpose:	Brings up the save dlg to ask the user where to save the html
				and the slices too. Also has the filter selection to choose from
				when picking the default slice.
				Sets the path location, name and graphic type into the variable
				m_PathName
	Returns:	TRUE if the user clicks save, FALSE otherwise

********************************************************************************************/
BOOL OpSlice::SaveFileDlg()
{
	// Set up the dialog

	// list of filters available is a cut down version of the filter list
	// offering just the bitmap versions
	String_256 FilterString(_R(IDS_SLICE_USABLE_FILTERS));

	// Get the filename.
	String_256 Str("untitled.htm");

	// Build a pathname out of it
	PathName Path(Str);

	// Get some details, ready for the dialog
	String_256 LocString = Path.GetLocation(FALSE);
	String_256 NameString = Path.GetFileName();

	// create a dialog and use the path and name of the file if available, or
	// use the default path if not.
	String_256 DocName = Document::GetCurrent()->GetDocName();

	// Sorry about the INT32 names, but we are into OS Specific scary stuff here
	// Under Windows 95 the OnFileNameOK() function of CFileDialog does not work
	// As a Result we have to do the tests ourselves afterwards under win95
	// by default we do not need to put the dialog up
	BOOL WeNeedToPutSaveAsDialogUpAgain = FALSE;
	
	// try the dialog
	do
	{
		// Default to only putting the dialog up the once.
		WeNeedToPutSaveAsDialogUpAgain = FALSE;

		// Create and display prepare the dialog
		SliceSaveFileDlg /*SaveFileDialog*/ SaveDialog(FilterString, LocString, NameString, DocName);
		SaveDialog.PrepareDialog();
		CString temp;
		
		temp.LoadString(_R(IDS_SLICE_EXPORT_TITLE));
		SaveDialog.m_ofn.lpstrTitle = temp;

		// Display the dialog
		BOOL Result = SaveDialog.OpenAndGetFileName();
		m_InjectHTML = SaveDialog.m_Inject; // did we ask to inject the HTML?

		// If they did not click on OK then stop right now
		if (!Result)
			return FALSE;

		// Make sure that the file name is OK
		// we only need to do this under windows 95, as other OS's will have already done it
		// Disable for VC4.0/MFC 4.0 as they have fixed it.
	#if _MFC_VER < 0x400
		if (IsWin32c())
		{	
			//TRACE( _T("Windows 95, testing for same file name after the event\n"));
			if (!SaveDialog.IsValidFilename()) WeNeedToPutSaveAsDialogUpAgain = TRUE;
		}
	#endif

		// Get the filename.
		CString TempName(SaveDialog.m_ofn.lpstrFile);
		SaveDialog.AppendExtension(&TempName);
		Str = String_256(TempName);
		Path = PathName(Str);

		// read the filter type to use
		// if the user hasn't picked (or typed) one of this list
		// then make it a gif.
		// Otherwise set the Path Type to store the default type for each slice made
		if (Path.GetType() != "jpg" && Path.GetType() != "bmp" && Path.GetType() != "gif" && Path.GetType() != "png")
			switch(SaveDialog.GetSelectedFilterIndex())
			{
			default:
			case 1:
				Path.SetType("gif");
				break;
			case 2:
				Path.SetType("jpg");
				break;
			case 3:
				Path.SetType("bmp");
				break;
			case 4:
				Path.SetType("png");
				break;
			}

		// Extract directory name (minus terminating backslash) and remember for next time.
		SaveDialog.SetDefaultPath(Path.GetLocation(FALSE));

	} while(WeNeedToPutSaveAsDialogUpAgain);

	// Put the file name into the var set aside for it
	m_PathName = Path;

	return TRUE;
}

/********************************************************************************************

>	BOOL OpSlice::ScanForRolloverStates()

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/99
	Purpose:	Sets the m_RolloverState member variable by scanning the layers
				and finding out what specail named layers exist for the creation
				of rollovers.
	Returns:	TRUE if enough layers were defined to make a rollover
				FALSE otherwise
********************************************************************************************/
BOOL OpSlice::ScanForRolloverStates(CList <CSlice *, CSlice *> * pMosaicList)
{
	INT32 i = 0;
	for (i = 0; i < 4; i++)
	{
		m_RolloverState[i].Exists = FALSE;
		m_RolloverState[i].pLayer = NULL;
		m_RolloverState[i].WasVisibleToStartWith = FALSE;
	}

	m_RolloverState[DEFAULT].Name.Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	m_RolloverState[MOUSE].Name.Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	m_RolloverState[CLICKED].Name.Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	m_RolloverState[SELECTED].Name.Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";

	Spread* pSelSpread = Document::GetSelectedSpread();
	if (pSelSpread == NULL)
		return FALSE;

	Layer * pLayer = pSelSpread->FindFirstLayer();
	while (pLayer)
	{
		for (i = 0; i < 4 ; i++)
		{
			if (pLayer->GetLayerID().CompareTo(m_RolloverState[i].Name) == 0)
			{
				m_RolloverState[i].pLayer = pLayer;
				m_RolloverState[i].WasVisibleToStartWith = pLayer->IsVisible();

				// does this layer exist within the mosaic list?
				m_RolloverState[i].Exists = FALSE;

				// find a wix marker on this layer
				Node * pNode = SliceHelper::FindNextOfClass(pLayer, pLayer, CC_RUNTIME_CLASS(TemplateAttribute) );

				while (pNode/* && !m_RolloverState[i].Exists*/)
				{
					POSITION EachSlice = pMosaicList->GetHeadPosition();

					// having found a marker does it match with any of the slices in the mosaic list?
					while (EachSlice)
					{
						CSlice * pSlice = pMosaicList->GetNext(EachSlice);
						if (pSlice->IsNamedSlice && pSlice->name.CompareTo(((TemplateAttribute *)pNode)->GetParam(), FALSE) == 0)
						{
							m_RolloverState[i].Exists = TRUE; // if it does than we need to export this state
							pSlice->ExistsOnLayerState[i] = 1; // this slice exists on this state
						}
					}

					pNode = SliceHelper::FindNextOfClass(pNode, pLayer, CC_RUNTIME_CLASS(TemplateAttribute) );
				}
			}
		}

		pLayer = pLayer->FindNextLayer();
	}

	return (m_RolloverState[DEFAULT].Exists && 
			(m_RolloverState[MOUSE].Exists || m_RolloverState[CLICKED].Exists || m_RolloverState[SELECTED].Exists ));
	
}


/********************************************************************************************

>	BOOL OpSlice::ExportRollOverSlice(CSlice * pSlice, BitmapExportOptions * pExportOptions)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/99
	Purpose:	Exports all the rollover states of this given slice if they are defined.
	Returns:	FALSE if any of the exports failled
********************************************************************************************/
BOOL OpSlice::ExportRollOverSlice(CSlice * pSlice, BitmapExportOptions * pExportOptions)
{
	BOOL ok = TRUE;

	for (INT32 i = 0 ; ok && i < 4 ; i++)
	{

		if (m_RolloverState[i].Exists && (i == 0 || pSlice->ExistsOnLayerState[i]))
		{
			// hide other button layers and show this button layer
			ShowRolloverLayer(i);
			String_256 StateName = pSlice->name;

			if (i > 0)
				StateName += m_RolloverState[i].Name;

			// make sure this named object is fully selected
			if (pSlice->IsNamedSlice)
			{
				// do this by calling the select scan on it
				SGNameItem* pItem = SliceHelper::LookupNameGalleryItem(pSlice->name);
				if (pItem)
				{
					SelectScan scanner(pItem, SelectScan::SELECT);
					scanner.Scan();
				}
			}

			// export the graphic
			ok = ExportSliceGraphic(pSlice, pExportOptions, StateName);
		}
	}

	// restore layers to as they were
	ShowRolloverLayer(ORIGINAL_LAYERS);

	return ok;
}


/********************************************************************************************

>	void OpSlice::ShowRolloverLayer(INT32 ShowLayer)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/99
	Purpose:	Shows the layer number passed to it and hides the other of the rollover layers.
				Pass it DEFAULT, MOUSE, CLICKED, SELECTED, ORIGINAL_LAYERS or ALL_LAYERS

********************************************************************************************/
void OpSlice::ShowRolloverLayer(INT32 ShowLayer)
{
	Spread* pSelSpread = Document::GetSelectedSpread();
	if (pSelSpread == NULL)
		return;

	Layer * pLayer = pSelSpread->FindFirstLayer();
	while (pLayer)
	{
		INT32 i = 0;
		for (i = 0; i < 4 ; i++)
		{
			if (pLayer->GetLayerID().CompareTo(m_RolloverState[i].Name) == 0)
			{
				if (ShowLayer == ORIGINAL_LAYERS /* Restore all layers to starting visibilty*/)
					pLayer->SetVisible(m_RolloverState[i].WasVisibleToStartWith);
				else
				if (ShowLayer == ALL_LAYERS /* show all layers at once */)
					pLayer->SetVisible(TRUE);
				else
				if (i == ShowLayer)
					pLayer->SetVisible(TRUE);
				else
					pLayer->SetVisible(FALSE);
			}
		}

		pLayer = pLayer->FindNextLayer();
	}
}



/********************************************************************************************

>	void OpSlice::RemoveIlligalFileAndJavaChars(String_256 & Str)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Purpose:	Replaces any illigal chars in the Str as far as filenames
				and use as javascript variables for use in rollovers 
				is concerned.

********************************************************************************************/
void OpSlice::RemoveIlligalFileAndJavaChars(String_256 & Str)
{
	INT32 i = 0;

	while (Str[i])
	{
		char stri = Str[i];

		if ((stri < 'a' || stri > 'z') && (stri < 'A' || stri > 'Z') && (stri < '0' || stri > '9')
			&& (stri != '#' && stri != '$') )
		{
			*((TCHAR *) Str +i) = (TCHAR)'_';
		}

		i++;
	}
}

String_256 OpSlice::TurnUnderScoreIntoSpace(const String_256 & StartStr)
{
	INT32 i = 0;

	String_256 Str = StartStr;

	while (Str[i])
	{
		char stri = Str[i];

		if (stri == '_')
		{
			*((TCHAR *) Str +i) = (TCHAR)' ';
		}

		i++;
	}

	return Str;
}

/********************************************************************************************

>	void OpSlice::MarkEmptySlices(CList <CSlice *, CSlice *> * pMosaicList, DocRect &SelRect)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/99
	Purpose:	What it is meant to do is check if the drawing has anything that is
				visible within this slice. So if there is nothing visible don't
				bother generating a graphic for it.
				Any empty slices are set as empty in the list of slices.
				Method used is to render the image to be exported as a 32bit graphic
				and then check the alpha channel for anything non-transparent.
	Param		pMoasicList	-	List of the slices
				SelRect		-	The rect of the image being exported
	Returns		-
	Errors		Not putting a graphic into the HTML table may screw up the sizing
				of cells in the table if it was relying on this cell to get the 
				size information. So instead I re-use the shim graphic expanded to
				that size (see the ExportHTML fn).
********************************************************************************************/
void OpSlice::MarkEmptySlices(CList <CSlice *, CSlice *> * pMosaicList, DocRect &SelRect, BitmapExportOptions * pExportOptions)
{
	// get the selection
	SelRange Sel(*(GetApplication()->FindSelection()));

	// set the range flags so it includes shadow and bevel manager nodes
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	rg.CrossLayer = TRUE;
	Sel.Range::SetRangeControl(rg);
	Sel.Update(FALSE);

	BOOL RenderedBmp = FALSE;

	OILBitmap* pOilBmp = NULL;
	UINT32 Width = 0;
	UINT32 Height = 0;
	
	POSITION ToDo = pMosaicList->GetHeadPosition();

	while (ToDo)
	{
		CSlice * pTopSlice = pMosaicList->GetNext(ToDo);

		// we will want to export all named slices
		// don't export any none-named slices that are blank
		// these are marked as empty
		if (!pTopSlice->IsNamedSlice)
		{
			// create the bmp of the selection if we haven't done so
			if (!RenderedBmp)
			{
				SelectionState SelState;
				SelState.Record();

				// no selection implies exporting the lot
				if (Sel.IsEmpty() || (pExportOptions && !pExportOptions->IsBackgroundTransparent()) )
				{
					// select everything
					//NodeRenderableInk::SelectAllInRect(SelRect, Document::GetSelectedSpread());
					// select everything
					DocRect SpreadBounds = BaseBitmapFilter::GetSizeOfDrawing(Document::GetSelectedSpread()); // returns in spread co-ords ignoring silly layers
					NodeRenderableInk::SelectAllInRect(SpreadBounds, Document::GetSelectedSpread());

					SelRange NewSel(*(GetApplication()->FindSelection()));
					// set the range flags so it includes shadow and bevel manager nodes
					RangeControl rg = NewSel.GetRangeControlFlags();
					rg.PromoteToParent = TRUE;
					rg.CrossLayer = TRUE;
					NewSel.Range::SetRangeControl(rg);
					NewSel.Update(FALSE);

					// render a bitmap of the entire image being exported (all slices)
					pOilBmp = CBMPBits::RenderSelectionToBMP(&NewSel, 32, TRUE, &SelRect);
				}
				else
				{
					// render a bitmap of the entire image being exported (all slices)
					pOilBmp = CBMPBits::RenderSelectionToBMP(&Sel, 32, TRUE, &SelRect);
				}

				SelState.Restore();
				
				if (!pOilBmp)
					return;

				Width = pOilBmp->GetWidth();
				Height = pOilBmp->GetHeight();

				RenderedBmp = TRUE; // since we only want to do this once!

				/*
				// add the bitmap to the bitmap list
				KernelBitmap* PreConvOffscreenBitmap = new KernelBitmap(pOilBmp,TRUE);
				KernelBitmap* copy1 = DIBUtil::CopyKernelBitmap(PreConvOffscreenBitmap,FALSE);	// FALSE to ensure OIL gets added to globalbmp list
				OILBitmap* pOil1 = (OILBitmap*) copy1->GetActualBitmap();
				pOil1->SetName(String_256("PreConvertedBitmap"));
				copy1->Attach(Document::GetCurrent()->GetBitmapList());
				delete PreConvOffscreenBitmap;
				*/
			}

			BOOL FoundABit = FALSE;

			// calculate sizes and start positions for the scan

			UINT32 x = (pTopSlice->left - SelRect.lox) * Width / (SelRect.hix - SelRect.lox);
			UINT32 y = (pTopSlice->top - SelRect.loy) * Height / (SelRect.hiy - SelRect.loy);

			UINT32 StartX = x;

			UINT32 EndX = min( Width, (pTopSlice->right - SelRect.lox) * Width / (SelRect.hix - SelRect.lox));
			UINT32 EndY = min(Height, (pTopSlice->bottom - SelRect.loy) * Height / (SelRect.hiy - SelRect.loy));

			TRACE( _T("marking slice "));
			TRACE(pTopSlice->name);
			TRACE( _T("\nfrom (%d,%d) to (%d,%d)\n"), x,y, EndX, EndY);
			Pixel32bpp Val;

			// scan this slices part of the bitmap
			if (pOilBmp)
				while (!FoundABit && y < EndY)
				{
					x = StartX;

					while (!FoundABit && x < EndX)
					{
						Val = pOilBmp->ReadPixel32bpp(x,y);

						// check the alpha of each pixel in the 32bit bitmap of the slice
						if (Val.Alpha != 255)
							FoundABit = TRUE;

						x++;
					}
					y++;
				}

			// mark as empty
			if (!FoundABit)
			{
				pTopSlice->IsEmpty = TRUE;
				TRACE( _T("Is Empty\n"));
			}
		}
	}
}

/********************************************************************************************

>	BOOL OpSlice::URLScan(String_256 * pLinkName, CList <CSlice *, CSlice *> * pMosaicList)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/99
	Purpose:	Scans through the tree looking for URL tags.
				If it finds one it looks for a set name on the same node.
				If it finds a set name it scans the Mosaic list for the button with that set name
				and records the URL as the link name for that button number
	Param		pMoasicList	-	List of the slices
				pLinkName	-	The dynamic array holding the URLs for each button
	Returns		TRUE if it ran ok
********************************************************************************************/
BOOL OpSlice::URLScan(String_256 * pLinkName, CList <CSlice *, CSlice *> * pMosaicList)
{
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL || pLinkName == NULL || pMosaicList == NULL)
		return FALSE;

	Node * pNode = SliceHelper::FindNextOfClass(pSpread, pSpread, CC_RUNTIME_CLASS(AttrWebAddress));
	while (pNode)
	{
		String_256 URL = ((AttrWebAddress *)pNode)->Value.m_url.GetWebAddress(); // the URL set from the URL dlg
		TCHAR* pTargetFrame = ((AttrWebAddress *)pNode)->Value.m_pcFrame; // ptr to the frame string set from the URL dlg

		Node *pWix = pNode->FindParent()->FindFirstChild();
		while (pWix)
		{
			if (pWix->IsAnAttribute() && IS_A(pWix,TemplateAttribute))
				break;
			pWix = pWix->FindNext();
		}

		if (pWix)
		{
			POSITION ToDo = pMosaicList->GetHeadPosition();
			String_256 ButtonName = ((TemplateAttribute *)pWix)->GetParam();

			// do the mundge
			RemoveIlligalFileAndJavaChars(ButtonName);

			while (ToDo)
			{
				CSlice * pSlice = pMosaicList->GetNext(ToDo);

				if (pSlice->IsNamedSlice && pSlice->ButtonNumber > 0 && ButtonName.CompareTo(pSlice->name) == 0)
				{
					// no dot in the URL? Perhaps they mean a file name? Make it URL.htm (or.html) for them
					if (URL.FindNextChar('.') == -1)
					{
						URL += ".";
						URL += m_HTMLext;
					}

					// set this as the link name
					pLinkName[pSlice->ButtonNumber - 1] = URL;

					// set the target frame
					pSlice->pFrameText = pTargetFrame;
					pSlice->HasURL = TRUE;

					ToDo = NULL;
				}
			}
		}

		pNode = SliceHelper::FindNextOfClass(pNode, pSpread, CC_RUNTIME_CLASS(AttrWebAddress));
	}

	return TRUE;
}

/********************************************************************************************

>	BYTE * OpSlice::LoadFileIntoMemory(String_256 FileName, DWORD & Size)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Purpose:	Creates a copy of the file in memory and says how big it is
	Param		Filename	-	The file to open
				Size		-	Fills in this param with the size of the memory object created
	Returns		a ptr to the memory, or NULL if it failled
********************************************************************************************/
BYTE * OpSlice::LoadFileIntoMemory(String_256 FileName, DWORD & Size)
{
	Size = 0;
	CFile file (FileName, CFile::modeRead );
	DWORD BytesToRead = file.GetLength();

	if (!BytesToRead)
		return NULL;

	BYTE * pBits = new BYTE[BytesToRead + 1];

	if (!pBits)
		return NULL;

	Size = file.Read(pBits, BytesToRead) + 1;

	// set the last bit as zero so we can use string functions on the file!
	*(pBits + BytesToRead) = 0; 

	return pBits;
}

/********************************************************************************************

>	BYTE * OpSlice::FindNextOccuranceOf(BYTE * pSearch, BYTE * pFrom)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Purpose:	Finds the next occurance of a string in the other large 'string'.
				Pass in lowercase and it will match with uppercase letters
	Param		pSearch		-	The shorter string being searched for
				pFrom		-	The larger string to find it in
	Returns		a ptr to the memory where the search string was located nearest to the starting point,
				or NULL if it wasn't found.
********************************************************************************************/
BYTE * OpSlice::FindNextOccuranceOf(BYTE * pSearch, BYTE * pFrom)
{
	ASSERT(pFrom);
	ASSERT(pSearch);
	INT32 i = 0;
	while (*pFrom)
	{
		i = 0;
		while (*(pSearch+i) == *(pFrom + i) || *(pSearch+i) == *(pFrom + i) + 32 /*+ 'a' - 'A'*/)
		{
			i++;
			if (*(pSearch + i) == 0)
				return pFrom;
		}
		pFrom++;
	}

	return NULL;
}


BYTE * OpSlice::FindPreviousChar(char Search, BYTE * pFrom, BYTE * pLimit)
{
	BOOL found = FALSE;
	while (pFrom >= pLimit && !found)
	{
		if ((char)*pFrom == Search)
			found = TRUE;
		else
			pFrom--;
	}

	return found ? pFrom : NULL;
}
/********************************************************************************************

>	void OpSlice::WriteText(ofstream & f, BYTE * pData, INT32 NoOfBytes)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Purpose:	There appeared to be a slight problem when using the write function to
				output text to the ofstream that contained chars 10 & 13.
				This function neatens this up cutting out 10,13 & 0 and putting a nice \n in 
				the right places.
	Param		f			-	The output stream
				pData		-	the string to output
				NoOfBytes	-	Number of bytes to send to the stream
	Returns		
********************************************************************************************/
void OpSlice::WriteText(ofstream & f, BYTE * pData, INT32 NoOfBytes)
{
	while (NoOfBytes)
	{
		INT32 scan = 0;
		BYTE * pTemp = pData;
		while (NoOfBytes && *pTemp != 10 && *pTemp != 13 && *pTemp != 0)
		{
			scan++;
			pTemp++;
			NoOfBytes--;
		}

		f.write(pData, scan);
		if (*pTemp != 0)
			f << "\n";

		pData = pTemp;

		while (NoOfBytes && (*pData == 10 || *pData == 13 || *pData == 0))
		{
			pData++;
			NoOfBytes--;
		}
	}
}

/********************************************************************************************

>	void OpSlice::FailledToExportHTML (const String_256 & file)

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Purpose:	Puts up an error msg saying that it could export the HTML
				and marks it as unexported
	Param		file		-	The file it failled to export
	Returns		
********************************************************************************************/
void OpSlice::FailledToExportHTML (const String_256 & file)
{
	TCHAR prompt[256];
	if (!::LoadString( AfxGetResourceHandle(), _R(IDS_FAILED_EXPORT_HTML), prompt, 256 ) )
		camStrcpy( prompt, "%s?" );								// in case cannot load

	// fill in the %s field with the filename
	TCHAR ErrorMsg[512];
	wsprintf( ErrorMsg, prompt, (LPCTSTR) file );
	Error::SetError( 0, ErrorMsg, 0 );
	SetNextMsgHelpContext(_R(IDM_OVERWRITE));
	InformLastError();
	m_ExportedHTMLOK = FALSE; // since we don't want to ask the user to view it if we didn't export it do we?
}

/********************************************************************************************

>	BOOL SliceSaveFileDlg::IsValidFilename()

	Author: 	Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Purpose:	override for the virtual function in SaveFileDialog.
				That had a message "file exists : replace or cancel?"
				This extends that to a "file exists : replace, insert or cancel?"
				for the insertion of HTML into existing HTML
	Param		
	Returns		
********************************************************************************************/
BOOL SliceSaveFileDlg::IsValidFilename()
{
	// Get the pathname
	CString FullPath = GetPathName();
	AppendExtension(&FullPath);

	// Webster - RanbirR 17/02/97
	// Save the file name with our Filter extension, for use in our Message Box.
#ifdef WEBSTER
	CString FileName(FullPath);
#endif //webster

	// if doesn't exist then thats fine
	if (_taccess( FullPath, 0 )) // requires io.h
	{
		//	WEBSTER-ranbirr-27/03/97
		// Remove the extension from the file. Webster - RanbirR 11\02\97.
	#ifdef WEBSTER
		RemoveFileExtension(&FileName);
	#endif //webster
		return TRUE;
	}

	// Added for Webster - Ranbir 11\02\97	
	// Removes the exstion from a file name.
#ifdef WEBSTER
	RemoveFileExtension(&FileName);
#endif //webster

	// Get the filename into a Path object
#ifndef WEBSTER
	String_256 KernelFilename(FullPath);
	PathName FileMessage(KernelFilename);
	KernelFilename =  FileMessage.GetTruncatedPath(50);
#endif //webster
	
	// Webster - RanbirR 17/02/97 
	// Use "FileName", since it contains the Filter extension.
#ifdef WEBSTER
	String_256 KernelFilename(FileName);
	PathName FileMessage(KernelFilename);
	KernelFilename =  FileMessage.GetTruncatedPath(50);
#endif //webster

	const TCHAR* Name;
	Name = (TCHAR*)KernelFilename;

	TCHAR prompt[256];
	if (!::LoadString( AfxGetResourceHandle(), _R(IDM_OVERWRITE_INJECT), prompt, 256 ) )
		camStrcpy( prompt, "%s?" );								// in case cannot load

	// fill in the %s field with the filename
	TCHAR ErrorMsg[512];
	wsprintf( ErrorMsg, prompt, (const TCHAR*)Name );
	Error::SetError( 0, ErrorMsg, 0 );
	SetNextMsgHelpContext(_R(IDM_OVERWRITE_INJECT));

	// then ask the user
	ErrorInfo Info;
	Info.Button[0] = _R(IDB_OVERWRITE);
	Info.Button[1] = _R(IDS_INJECT);
	Info.Button[2] = _R(IDS_CANCEL);
	
	// This makes the cancel button the default one in this situation
	// as defaulting to replace is bad.
	Info.OK = Info.Cancel = 3;

	switch (AskQuestion( &Info ))
	{
		// if they cancel, then the name is not acceptable to them
		case _R(IDS_CANCEL):
			return FALSE;

		case _R(IDS_INJECT):
			m_Inject = TRUE;
		case _R(IDB_OVERWRITE):
		default:
			break;
	}

	return TRUE;
}

// shadows are not included in the bounds of the shape
// due to latency in generating the bounds and you may want the entire set of buttons shadowed.
// there are similar problems with bevels.
// so this quick scan gets the "large" size which includes the full bevel and shadow and the
// calling routine can use whichever best suits (sjk 7-4-00)
DocRect OpSlice::ScanLargeSliceBounds(const String_256 & Name)
{
	DocRect Ret;
	Ret.MakeEmpty();

	// scan each layer
	Node * pTop = Document::GetSelectedSpread();
	Node * pNode = NULL;
	Node * pParent = NULL;

	if (pTop)
	{
		// scan from the first layer all through the layers since they are brothers of this layer
		pNode = SliceHelper::FindNextOfClass(pTop, pTop, CC_RUNTIME_CLASS(TemplateAttribute));

		while (pNode)
		{
			if (Name.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
			{
				pParent = pNode->FindParent();
				if (pParent->IsBounded())
					Ret = Ret.Union(((NodeRenderableInk *)pParent)->GetBoundingRect(TRUE, FALSE));
			}

			pNode = SliceHelper::FindNextOfClass(pNode, pTop, CC_RUNTIME_CLASS(TemplateAttribute));
		}
	}

	return Ret;
}

// deflates any text stories in a slice to their visible bounds not the extending metrically worked out bounds
void OpSlice::ScanTextStorySliceBounds(const String_256 & Name, DocRect & Bounds)
{
	// scan each layer
	Node * pTop = Document::GetSelectedSpread();
	Node * pNode = NULL;
	Node * pParent = NULL;
	BOOL IncludesText = FALSE;
	DocRect TextModBounds;

	TextModBounds.MakeEmpty();

	if (pTop)
	{
		// scan from the first layer all through the layers since they are brothers of this layer
		pNode = SliceHelper::FindNextOfClass(pTop, pTop, CC_RUNTIME_CLASS(TemplateAttribute));

		while (pNode)
		{
			if (Name.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
			{
				pParent = pNode->FindParent();

				if (IS_A(pParent,TextChar) || IS_A(pParent,TextStory))
				{
					IncludesText= TRUE;

					// expand the rect by the bounding rect of the text story / text char
					// taking the intersection of the Metric method of size with the Bounding rect render method
					// The metric assures us that the overall bounds of the rect will not be outside the
					// complete scope of the text. The bounding rect method expands for ascenders and descenders
					// only where they appear but may be too large as it includes visible onscreen UI.
					// intersecting these two will easily give the correct answer 99.9% of the time
					// and always a safe answer.
					DocRect Temp = SliceHelper::BoundingNodeSize(pParent);
					Temp = Temp.Intersection(((NodeRenderableInk *)pParent)->GetBoundingRect());
					TextModBounds = TextModBounds.Union(Temp);
				}
				else // union in the usual way
					TextModBounds = TextModBounds.Union(SliceHelper::BoundingNodeSize(pParent));
			}

			pNode = SliceHelper::FindNextOfClass(pNode, pTop, CC_RUNTIME_CLASS(TemplateAttribute));
		}

		if (IncludesText)
			Bounds = TextModBounds;
	}
}

