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

#include "camtypes.h"
#include "filtimag.h"

#include "filters.h"
#include "oilfltrs.h"

//#include "resource.h"		// for _R(IDS_OUT_OF_MEMORY)

#include "camfiltr.h"		// for BaseCamelotFilter progress update
#include "app.h"			// Camelot

//#include "resimmap.h"		// Imagemap resources

//#include "fimagdlg.h"		//Imagemap dialog
#include "filtimag.h"

#include "docview.h"		//For DocView::GetCurrent
#include "clipint.h"		//For InternalClipboard::CopyText
#include "helpuser.h"		//For GetNextMsgHelpContext

#include "sgliboil.h"		//For file utility functions to do with temp files
#include "fileutil.h"		//For file utility functions to do with temp files

#include "strlist.h"		//For class StringListItem 

//#include "osrndrgn.h"		//For OSRenderRegion class to convert DocRect coords to WinRect -matt 24/08/2000
//#include "htmlexp.h"		//For WriteNumber
//#include "dialogop.h"		//For DialogOp::GetLongGadgetValue
//#include "imagdlg.h"		//_R(IDC_IMAGEMAP_HEIGHT)
//#include "filtimop.h"



CC_IMPLEMENT_MEMDUMP(ImagemapFilter, Filter)

#define new CAM_DEBUG_NEW

TCHAR ImagemapFilter::ms_strApprox[]	= _T("ExportImagemap\\CurveApproximation");
TCHAR ImagemapFilter::ms_strAllRectangles[] = _T("ExportImagemap\\AllRectangles");
TCHAR ImagemapFilter::ms_strClipboard[] = _T("ExportImagemap\\Clipboard");
TCHAR ImagemapFilter::ms_strFile[] = _T("ExportImagemap\\File");
TCHAR ImagemapFilter::ms_strInsert[] = _T("ExportImagemap\\Insert");

ImagemapFilterOptions ImagemapFilter::ms_Options=ImagemapFilterOptions();

/********************************************************************************************

  Constructors and initialiser

*********************************************************************************************/

/********************************************************************************************

>	ImagemapFilter::ImagemapFilter()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	Default constructor

********************************************************************************************/
ImagemapFilter::ImagemapFilter()
{
	//First set up the members of the filter class
	ExportMsgID =		_R(IDS_EXPORTMSG_IMAGEMAP);		

	Flags.CanImport					= FALSE;
	Flags.CanExport					= TRUE;

	Flags.CanExportMultipleImages	= FALSE;
	Flags.ShowFilter				= TRUE;

	FilterID = FILTERID_IMAGEMAP;

	//Set the pointer to our render region as NULL
	m_pRegion=NULL;

	//And set up our options to a set of default values
	ImagemapFilter::ms_Options=ImagemapFilterOptions();

	BackgroundRedrawStateSet=FALSE;
	BackgroundRedrawState=FALSE;
}

/********************************************************************************************

>	BOOL ImagemapFilter::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if the filter was initialized ok
				FALSE otherwise
	Purpose:	Initializes the ImagemapFilter.

********************************************************************************************/
BOOL ImagemapFilter::Init()
{
	// Set up our associated oil filter
	pOILFilter = new ImagemapOILFilter(this);

	//If we failed to set it up, return FALSE
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDS_FILTERNAME_IMAGEMAP));
	FilterInfo.Load(_R(IDS_FILTERINFO_IMAGEMAP));

	//And declare those variables which we want to save in the registry
	if (DeclareFilterPreferenceSection())
	{
		Camelot.DeclarePref( NULL, ms_strApprox, (INT32*) &ms_Options.m_ffApprox,  FF_VERYCLOSELY, FF_NOTATALL);
		Camelot.DeclarePref( NULL, ms_strAllRectangles, &ms_Options.m_fAllRectangles, FALSE, TRUE);
		Camelot.DeclarePref( NULL, ms_strClipboard, &ms_Options.m_fClipboard, FALSE, TRUE);
		Camelot.DeclarePref( NULL, ms_strFile, &ms_Options.m_fFile, FALSE, TRUE);
		Camelot.DeclarePref( NULL, ms_strInsert, &ms_Options.m_fInsert, FALSE, TRUE);
	}

	//Initialise the ExportOptions dialog
	ImagemapDlg::Init();
	
	//And return
	return TRUE;
}

/********************************************************************************************

>	ImagemapFilter::~ImagemapFilter()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	Destructor. Deletes the render region pointer.

********************************************************************************************/
ImagemapFilter::~ImagemapFilter()
{
	//Delete our render region pointer if it's pointing to something
	m_pRegion=NULL;

}


/********************************************************************************************

  Filter entry points

*********************************************************************************************/

/********************************************************************************************

>	BOOL ImagemapFilter::DoExport(Operation* pOp, 
									CCLexFile* pFile, 
									PathName* pPath, 
									Document* pDoc,
									BOOL ShowOptions);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if the export was OK
				FALSE if there were any errors

	Purpose:	This function is called when the user has gone to the
				Export... dialog box, chosen the imagemap filter and
				clicked the Export button.

				This function simply calls three member functions in turn
				in order to:

				- Put up a dialog box to get some options from the user
				- Set those options as current within the filter
				- Export the file using those options

********************************************************************************************/

BOOL ImagemapFilter::DoExport(Operation* pOp, 
							  CCLexFile* pFile, 
							  PathName* pPath, 
							  Document* pDoc,
							  BOOL ShowOptions)
{
	//First we need to get some ImagemapFilterOptions from the user
	//To do this, create a blank set of ImagemapFilterOptions
	ImagemapFilterOptions ifoNew=ms_Options;

	//And specify within those options that we want to
	//export to a file
	ifoNew.m_fFile=TRUE;

	//And specify the name of that file as being the path we have been given
	ifoNew.m_pthFile=*pPath;

	//And specify a pointer to that file
	ifoNew.m_pfileFile=pFile;

	//And remember we're reporting errors
	ifoNew.m_fReportErrors=TRUE;
	
	//And call up the dialog box to fill them in
	//If this function returns FALSE, the user has cancelled the dialog
	if (GetOptionsFromUser(&ifoNew, pPath, pFile))
	{
		
		//Set those options as current within the filter
		SetFilterOptions(ifoNew);

		//And export to the file using those options
		return PrepareAndWriteData(pDoc);
	}
	else
		//If the user has cancelled the dialog, return TRUE anyway
		return TRUE;
}


/********************************************************************************************

>	BOOL ImagemapFilter::PrepareAndWriteData(Document* pDoc)


	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if the export was OK
				FALSE if there were any errors

	Purpose:	Calculates an imagemap based on the file pDoc and the current
				set of ImagemapFilterOptions. Then writes this out to
				either a file or the clipboard (as specified in the options).

				This function is called by both ImagemapFilter::DoExport
				and the BaseBitmapFilter class.

********************************************************************************************/

BOOL ImagemapFilter::PrepareAndWriteData(Document* pDoc)
{
	//This variable will keep track of any problems
	BOOL ok=TRUE;

	//First set up the data to export
	ok=PrepareData(pDoc);

	// if there was a problem, or the region is empty
	if (!ok || m_pRegion->IsEmpty())
	{
		//delete the render region
		if (m_pRegion)
			delete m_pRegion;
		m_pRegion = NULL;

		//If there was a problem, return FALSE
		if (!ok)
			return FALSE;

		//If there is no data, and we are reporting errors, put up a message box to say so
		if (ms_Options.m_fReportErrors)
		{
			ERROR1(TRUE, _R(IDE_IMAGEMAP_NODATA));
		}
		else
			return TRUE;
	}

	//Otherwise, export the data
	ok=WriteData();

	//And delete the render region
	if (m_pRegion)
		delete m_pRegion;
	m_pRegion = NULL;

	return ok;

}

/********************************************************************************************

>	BOOL ImagemapFilter::PrepareData(Document* pdocToExport)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Returns:	TRUE if successful

	Purpose:	Sets up the imagemap data in the render region.

				Does not export that data to the file.

********************************************************************************************/

BOOL ImagemapFilter::PrepareData(Document* pdocToExport)
{
	//Set up the render region
	BOOL ok=PrepareRenderRegion(pdocToExport);

	//If that worked...
	if (ok)
	{
		//Render into our ImagemapRenderRegion
		//to set the data
		ExportRender(m_pRegion);

		//Restore the settings of the render region
		RestoreBackgroundRedraw(pView);
	}

	//And return
	return ok;
}

/********************************************************************************************

>	BOOL ImagemapFilter::WriteData()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if the export was OK
				FALSE if there were any errors

	Purpose:	Writes the data in the ImagemapRenderRegion either to a file
				or to the clipboard, as specified in the ImagemapFilterOptions.

				
    
********************************************************************************************/
BOOL ImagemapFilter::WriteData()
{
	//And this variable will keep track of errors
	BOOL ok=TRUE;

	//Now, should we be writing to a file?
	if (ms_Options.m_fFile)
	{
		//Yes, we should. So we should have been passed a path name
		//and a pointer to that file. So retrieve them.
		PathName pthExportFile=ms_Options.m_pthFile;
		CCLexFile* pfileExportFile=ms_Options.m_pfileFile;

		//Now, does this path name refer to an existing file? And if 
		//it does refer to an existing file, then have we been told to insert
		//our imagemap into that file?
		if (SGLibOil::FileExists(&pthExportFile) && ms_Options.Insert())
		{
			//Yes. So insert the data into the file
			ok=WriteDataToExistingFile(pfileExportFile, &pthExportFile);
		}
		else
		{
			//No, either it's a new file or we haven't been told to
			//insert our data into the file. So write to a new file
			//(and if the file does exist, it will be overwritten).
			ok=WriteDataToNewFile(pfileExportFile, &pthExportFile);
		}
	}
	else
	{
		//No. So should we be writing to the clipboard?
		if (ms_Options.m_fClipboard)
		{
			//Yes. So do that now.
			ok=WriteDataToNewFile(NULL, NULL);
		}
	}

	//And delete the render region
	if (m_pRegion)
		delete m_pRegion;
	m_pRegion=NULL;

	//And return
	return ok;
}

/********************************************************************************************

>	virtual BOOL ImagemapFilter::WillAcceptExistingFile(PathName pthToReplace)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/97
	Returns:	TRUE if this filter is willing to accept this existing file
				FALSE otherwise
	Purpose:	Checks if the user wants to export to this existing file.

				To do this, we put up a message box with three options: Insert,
				Overwrite and Cancel.

********************************************************************************************/

BOOL ImagemapFilter::WillAcceptExistingFile(PathName pthToReplace)
{
	//So first get the truncated path name
	String_256 strToReplace =  pthToReplace.GetTruncatedPath(50);

	//Now create a string to put our error message in
	String_256 strError;

	//And make the error message up using the file name
	strError.MakeMsg(_R(IDM_IMAGEMAP_INSERT), &strToReplace);
												
	//Now, set that error message as the next one to display
	Error::SetError( 0, strError, 0 );

	//And set up the message help context
	SetNextMsgHelpContext(_R(IDM_IMAGEMAP_INSERT));

	//Now set up the buttons
	ErrorInfo Info;
	Info.Button[0] = _R(IDB_IMAGEMAP_INSERT);
	Info.Button[1] = _R(IDB_IMAGEMAP_REPLACE);
	Info.Button[2] = _R(IDS_CANCEL);
										 	
	//Make Insert the default
	//and Cancel the cancel button
	Info.OK = 1;
	Info.Cancel = 3;

	//And put the error box up
	switch (AskQuestion( &Info ))
	{
		case _R(IDB_IMAGEMAP_INSERT):
			ms_Options.m_fInsert=TRUE;	
			return TRUE;

		case _R(IDB_IMAGEMAP_REPLACE):
			ms_Options.m_fInsert=FALSE;
			return TRUE;

		case _R(IDS_CANCEL):
			return FALSE;

		default:
			return TRUE;
	}

}

/********************************************************************************************

>	void ImagemapFilter::SearchFileForImagemaps(const PathName& pthSearch, List* plstToAdd)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/7/97
	Inputs:		pthSearch	The path of the file to search
				plstToAdd	Pointer to a list to add the map names to
				
	Returns:	-

	Purpose:	Goes through the file pointed to by pthSearch.

				Each time it finds an imagemap tag <MAP NAME=xxx>, it adds
				the value of the NAME parameter into the list pointed to by
				plstToAdd.

				It does this all by calling the function BuildMapNamesList.

  See Also:		ImagemapFilter::BuildMapNamesList(); 
    
********************************************************************************************/

void ImagemapFilter::SearchFileForImagemaps(const PathName& pthSearch, List* plstToAdd)
{
	//First create a disk file
	CCDiskFile fileOpen(1024, FALSE, TRUE);

	//Create a copy of the path name we've been given
	PathName pthOpen=pthSearch;

	//And call BuildMapNamesList to do all the work
	BuildMapNamesList(&fileOpen, &pthOpen, plstToAdd);
	
}







/********************************************************************************************

  Access functions

  Functions to set and get the values of member variables

*********************************************************************************************/


/********************************************************************************************

>	void ImagemapFilter::SetFilterOptions(ImagemapFilterOptions ifoOptions)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		ifoOptions	The options to set as current within the filter
	Returns:	-

	Purpose:	Sets ifoOptions as current within the filter

 ********************************************************************************************/

void ImagemapFilter::SetFilterOptions(ImagemapFilterOptions ifoOptions)
{
	//Simply copy the options into our static member variable
	ms_Options=ifoOptions;
}

/********************************************************************************************

>	ImagemapFilterOptions ImagemapFilter::GetFilterOptions()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		-
	Returns:	The current set of imagemap filter options

	Purpose:	Returns the current set of imagemap filter options

 ********************************************************************************************/

ImagemapFilterOptions ImagemapFilter::GetFilterOptions()
{
	//Return the filter options
	return ms_Options;
}


/********************************************************************************************

  Action functions

  Functions which are called by the entry point functions to do things

*********************************************************************************************/

/********************************************************************************************

	BOOL ImagemapFilter::GetOptionsFromUser(ImagemapFilterOptions* pifoDefault, 
					PathName* pPath, CCLexFile* pFile)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		pifoDefault	A set of ImagemapFilterOptions that
							will appear in the dialog box when it starts
				pPath		Path to the file we are writing to
				pFile		The file we are writing to
	Outputs:	A set of ImagemapFilterOptions from the user
	Returns:	FALSE if the dialog was cancelled

	Purpose:	Puts up the Export Options... dialog box to get a 
				set of ImagemapFilterOptions from the user.

 ********************************************************************************************/

BOOL ImagemapFilter::GetOptionsFromUser(ImagemapFilterOptions* pifoDefault, PathName* ppthFile, CCLexFile* pFile)
{
	ERROR2IF (pifoDefault==NULL, FALSE, "ImagemapFilter::GetOptionsFromUser - NULL parameter");

	//Now, if the file we are writing to exists already, and if
	//we are writing to that file
	if (SGLibOil::FileExists(ppthFile) && ms_Options.Insert())
	{
		//Then we want to create a list of all the imagemap names in the existing file
		//This variable will hold the list
		List lstNames;

		//Then go through the file and get a list of all the imagemap names
		//in the file
		BuildMapNamesList(pFile, ppthFile, &lstNames);

		//Then put up the modal dialog with that list of names shown
		//in the map name combo box
		BOOL ok=ImagemapDlg::InvokeDialog(pifoDefault, &lstNames);

		//Delete the list of names
		lstNames.DeleteAll();

		//And return
		return ok;
	}
	else
	{
		//Put up the dialog box with nothing shown in the map name
		//combo box
		//The dialog will start with the inital values of pifoReturn
		//in it. When the dialog closes, it will put the values that the user
		//chose back into *pifoDefault
		return ImagemapDlg::InvokeDialog(pifoDefault);
	}

}	


/********************************************************************************************

  Toolkit functions

  Protected functions which are called by other functions as helpers.

*********************************************************************************************/

/********************************************************************************************

>	BOOL ImagemapFilter::PrepareRenderRegion(Document* pDoc)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/4/97
	Inputs:		pDoc - The document to export
	Returns:	TRUE if successful
				FALSE if there was an error
	Purpose:	a. Creates a new render region with the current set of options
				b. Attaches a device to that render region

 ********************************************************************************************/

BOOL ImagemapFilter::PrepareRenderRegion(Document* pDoc)
{
	// First, create the render region from our current options
	if (m_pRegion)
		delete m_pRegion;

	m_pRegion = new ImagemapRenderRegion(ms_Options);

	//Now we must attach a device to our render region
	//For this, we must find the current view and spread

	pView = DocView::GetCurrent();
	Spread* pSpread = Filter::GetFirstSpread(pDoc);

	ERROR2IF(pView==NULL, FALSE, "ImagemapFilter::PrepareToExport - no current view!");
	ERROR2IF(pSpread==NULL, FALSE, "ImagemapFilter::PrepareToExport - no current spread!");

	//And attach the device to our render region
	//The middle parameter is the device context, which we set as NULL
	m_pRegion->AttachDevice(pView, NULL, pSpread);

	//We also need to switch background rendering off in the view
	ForceBackgroundRedrawOff(pView);
		
	return TRUE;
}

/********************************************************************************************

>	void ImagemapFilter::ForceBackgroundRedrawOff(DocView* pView)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/97 - Britain deserves better!
	Inputs:		pView 
	Returns:	-
	Purpose:	Switches background rendering off in pView

 ********************************************************************************************/

void ImagemapFilter::ForceBackgroundRedrawOff(DocView* pView)
{
	// We wan't to make sure we only remember the old state once ...
	if (!BackgroundRedrawStateSet)
	{
		BackgroundRedrawState = pView->GetForeBackMode();
		BackgroundRedrawStateSet = TRUE; 	// So we don't do this more than once
	}

	pView->SetForeBackMode(FALSE);
}

/********************************************************************************************

>	void ImagemapFilter::RestoreBackgroundRedraw(DocView* pView)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/97 - Tough on bugs, tough on the causes of bugs
	Inputs:		pView 
	Returns:	-
	Purpose:	Restores pView's background redraw state

 ********************************************************************************************/


void ImagemapFilter::RestoreBackgroundRedraw(DocView* pView)
{
	// We wan't to make sure we only remember the old state once ...
	pView->SetForeBackMode(BackgroundRedrawState);
}

/********************************************************************************************

>	BOOL ImagemapFilter::OpenImagemapFile(CCLexFile* pFile, PathName* pPath)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/97
	Inputs:		pFile, pPath - the file to export to
	Returns:	TRUE if successful
				FALSE if there was an error
	Purpose:	Opens the export file

 ********************************************************************************************/

BOOL ImagemapFilter::OpenImagemapFile(CCLexFile* pFile, PathName* pPath)
{
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
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in ImagemapFilter::DoExport\n"));
			return FALSE;
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL ImagemapFilter::CloseImagemapFile(CCLexFile* pFile)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/97
	Inputs:		pFile - the file to close
	Returns:	TRUE if successful (no way of erroring at present)
				
	Purpose:	Closes the export file

 ********************************************************************************************/

BOOL ImagemapFilter::CloseImagemapFile(CCLexFile* pFile)
{
	if (pFile->isOpen())
		pFile->close();

	return TRUE;
	
}

/********************************************************************************************

  >	void ImagemapFilter::Write(CCLexFile* pfileToWrite, TCHAR** ppcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pfileToWrite - The file to write to
				
				This may be NULL

	Purpose:	Writes out the imagemap to a file or a text buffer or both. This
				works as follows:

				If pfileToWrite is not NULL, this function will write the
				imagemap to that file.

				If ms_Options.fClipboard is TRUE, this function will also
				set up a text buffer, write the imagemap to that text buffer
				and then copy the text buffer to the clipboard.

				This function calls our toolkit function WriteHelper to do
				most of the work.

				But what is actually written out?

				The only thing that will ever be written to the text buffer
				is the imagemap itself:

				<MAP>
					<AREA SHAPE=RECTANGLE ...>
					<AREA SHAPE=POLYGON ...>
				</MAP>

				What is written to the file depends on whether we have
				told that this imagemap is associated with a particular bitmap
				file. In other words, it depends on whether the Path To Bitmap
				File member variable of our ImagemapFilterOptions member variable
				is valid.

				If we have been given a bitmap file, then an entire HTML file 
				embedding the bitmap will be written to the imagemap file:

				<HTML>
				...
					<IMG SRC="c:\dir\bitmap.gif">
				</HTML>

				<MAP>
				...
				</MAP>

				If not, only the portion between the <MAP> tags will be written.
				
  SeeAlso:		ImagemapFilter::WriteHelper()
	
********************************************************************************************/

void ImagemapFilter::Write(CCLexFile* pfileToWrite)
{					
	//If we need to copy the imagemap to the clipboard, this variable
	//will point to a text buffer to store the string in.
	TCHAR* pBuffer=NULL;

	//Now, do we need to copy the imagemap to the clipboard?
	if (ms_Options.m_fClipboard)
	{
		//Yes. So we need to set up a text buffer.

		//First find out the length of this buffer by calling our helper
		//function with no parameters
		INT32 lBufferLength=WriteHelper();

		//And set up a buffer of that length + length of hint line (determined by
		//calling WritePreamble()) - Matt 31/08/2000
		if (lBufferLength>0)
		{
			lBufferLength += WritePreamble(NULL);

			pBuffer=new TCHAR[lBufferLength+1];

			//And NULL terminate the buffer at the first character
			*pBuffer=0;

			//If we have a text buffer, then write to it
			if (pBuffer)
			{
				//Write to the buffer our hint...
				WritePreamble(pBuffer);
			}
		}
		
	}

	//And write the imagemap out to both the file and, if necessary, the
	//text buffer
	WriteHelper(pfileToWrite, pBuffer);
	
	//And finally, if we have a text buffer
	if (pBuffer)
	{
		//Copy it to the clipboard
		CopyToClipboard(pBuffer);

		//Then delete it
		delete pBuffer;
	}

}


/*************************************************

  Toolkit functions

  ***********************************************/


/********************************************************************************************

 >	INT32 ImagemapFilter::WriteHelper(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pfileToWrite - The file to write to
				pcBuffer	 - Pointer to a text buffer to write to

				Either of the above may be NULL.

  Returns:		The number of TCHARs written.

  Purpose:		This function will be called twice:

				a. First with all parameters NULL, simply to calculate how
				   INT32 the text buffer for the imagemap must be

				b. Secondly with the parameters pointing to a file, or a text buffer
				   or both, to actually write the imagemap out.

				It's assumed that the text buffer is long enough for the imagemap.

				This function simply iterates through the list and calls on
				the members to write themselves out.
	
  SeeAlso:		ImagemapFilter::WriteHelper()
	
********************************************************************************************/

INT32 ImagemapFilter::WriteHelper(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	//Set up a variable to remember how many chars we have written
	INT32 lCharsWritten=0;
	
	//Write the imagemap HTML
	lCharsWritten+=WriteImagemapHTML(pfileToWrite, pcBuffer);

	//And return the number of characters written
	return lCharsWritten;				 											  
		
}

/********************************************************************************************

 >	INT32 ImagemapFilter::WriteBitmapHTML(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pfileToWrite - The file to write to
				pcBuffer	 - Pointer to a text buffer to write to

				Either of the above may be NULL.

  Returns:		The number of TCHARs written.

  Purpose:		Writes the bitmap HTML.
	
  SeeAlso:		ImagemapRenderRegion::WriteHelper()
	
********************************************************************************************/
/*
INT32 ImagemapFilter::WriteBitmapHTML(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	//Set up a variable to remember how many chars we have written
	INT32 lCharsWritten=0;
	
	//Now, first write <HTML> followed by an end of line
	lCharsWritten+=HTMLExportFilter::WriteTag(_R(IDS_HTMLEXPORT_HTML), pfileToWrite, pcBuffer);
	lCharsWritten+=HTMLExportFilter::WriteEOL(pfileToWrite, pcBuffer);

	//Put the bitmap path in a variable
	String_256 strBitmapPath=ms_Options.m_pthBitmapFile.GetWebAddress();
	
	//Now make the HEAD portion from our resource string
	String_256 strHead;
	strHead.MakeMsg(_R(IDS_HTMLEXPORT_IMAGEMAP_HEAD), (TCHAR*) strBitmapPath);

	//And write it out
	lCharsWritten+=HTMLExportFilter::Write(strHead, pfileToWrite, pcBuffer);

	//And make up the BODY portion
	String_256 strBody;
	strBody.MakeMsg(_R(IDS_HTMLEXPORT_IMAGEMAP_BODY), (TCHAR*) strBitmapPath, (TCHAR*) ms_Options.m_strName);

	//And write it out
	lCharsWritten+=HTMLExportFilter::Write(strBody, pfileToWrite, pcBuffer);

	//Write the HTML closing tag
	lCharsWritten+=HTMLExportFilter::WriteCloseTag(_R(IDS_HTMLEXPORT_HTML), pfileToWrite, pcBuffer);
	lCharsWritten+=HTMLExportFilter::WriteEOL(pfileToWrite, pcBuffer);

	//And return the number of characters written
	return lCharsWritten;				 	
		
} */

/********************************************************************************************

 >	INT32 ImagemapFilter::WriteImagemapHTML(CCLexFile* pfileToWrite=NULL, TCHAR* pcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pfileToWrite - The file to write to
				pcBuffer	 - Pointer to a text buffer to write to

				Either of the above may be NULL.

  Returns:		The number of TCHARs written.

  Purpose:		Writes the imagemap HTML.
	
	  SeeAlso:		ImagemapFilter::WriteHelper(), ImagemapRenderRegion::Write()
	
********************************************************************************************/

INT32 ImagemapFilter::WriteImagemapHTML(CCLexFile* pfileToWrite, TCHAR* pcBuffer)
{
	//Keep a count of the characters we write out
	INT32 lCharsWritten=0;

	//Do we have an imagemap render region member?
	if (m_pRegion)
	{
		//Yes. So we write out the imagemap

		//First write out <MAP NAME="MyMap">
		lCharsWritten+=WriteStartOfTag(_R(IDS_HTMLEXPORT_MAP), pfileToWrite, pcBuffer);

		lCharsWritten+=WriteParameterInQuotes(_R(IDS_HTMLEXPORT_NAME), ms_Options.m_strName, pfileToWrite, pcBuffer);

		lCharsWritten+=WriteEndOfTag(pfileToWrite, pcBuffer);

		lCharsWritten+=WriteEOL(pfileToWrite,pcBuffer);

		//And write the imagemap itself
		lCharsWritten+=m_pRegion->Write(pfileToWrite, pcBuffer);

		//Then write out </MAP>
		lCharsWritten+=WriteCloseTag(_R(IDS_HTMLEXPORT_MAP), pfileToWrite, pcBuffer);

		//And a new line
		//lCharsWritten+=WriteEOL(pfileToWrite, pcBuffer);
	}
	
	
	//And return the number of characters written
	return lCharsWritten;
}

/********************************************************************************************

 >	void ImagemapFilter::CopyToClipboard(TCHAR* pcBuffer=NULL)
										   
	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		pcBuffer	The text to put on the clipboard

    Purpose:	Puts the text on the clipboard
	
********************************************************************************************/

void ImagemapFilter::CopyToClipboard(TCHAR* pcBuffer)
{
	//Put the text on the clipboard
	InternalClipboard::CopyText(pcBuffer);
}

/********************************************************************************************

>	BOOL ImagemapFilter::WriteDataToExistingFile(CCLexFile* pFile, PathName* pPath)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if the export was OK
				FALSE if there were any errors

	Purpose:	Writes the data in the imagemap region into an existing file and/or
				to the clipboard.
    
********************************************************************************************/

BOOL ImagemapFilter::WriteDataToExistingFile(CCLexFile* pFile, PathName* pPath)
{
	//Check parameters
	ERROR2IF (pFile==NULL || pPath==NULL, FALSE, "ImagemapFilter::WriteDataToExistingFile - null parameter");

	//Now let's create a new Xara temporary file
	PathName pthTemp=FileUtil::GetTemporaryPathName();

	//And open that temporary file for writing
	CCDiskFile fileTemp(1024, FALSE, TRUE);

	OpenImagemapFile(&fileTemp, &pthTemp);

	//Now write our existing file across to our temporary file,
	//parsing it as we do so, and inserting the imagemap data
	//in the appropriate position
	WriteExistingDataHelper(&fileTemp, pFile, pPath);

	//And close the files
	fileTemp.close();
	
	//Finally, delete our existing file
	BOOL ok=SGLibOil::FileDelete(pPath);

	//Copy our temporary file across
	if (ok)
		SGLibOil::FileCopy(&pthTemp, pPath);

	//Then delete our temporary file
	if (ok)
		ok=SGLibOil::FileDelete(&pthTemp);

	//And return
	return ok;
}

/********************************************************************************************

>	BOOL ImagemapFilter::WriteDataToNewFile(CCLexFile* pFile, PathName* pPath)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Returns:	TRUE if the export was OK
				FALSE if there were any errors

	Purpose:	Writes the data in the imagemap region into a new file and/or
				to the clipboard.
    
********************************************************************************************/

BOOL ImagemapFilter::WriteDataToNewFile(CCLexFile* pFile, PathName* pPath)
{
	//Have we been passed a file to write to?
	if (pFile!=NULL)
	{
		//Yes. So open it.
		if (!OpenImagemapFile(pFile, pPath))
			return FALSE;
	}


	// add a header to the new file so that if a
	// dumb user looks at it it will show the image
	// this isn't in the "replace an image map" code
	// since that will already have an image file indicated
	//  Note that it is valid for this function to be called with 2 NULL arguments ( exporting
	//  to the clipboard only ). So, no use in testing for NULL arguments.
//	ASSERT(pPath);
	if( pFile != NULL )	//  Only write information out to pFile if it exists.
	{
		String_256 Temp = "<img src=\"file:///";
		*pFile << Temp;
		*pFile << ms_Options.m_GraphicPath.GetPath(); // the path and file name of the graphic
		Temp = "\"";
		*pFile << Temp;

/*********************************************************************************************
		// matt - 25/08/2000 - added the following to ensure HTML has height and width data
		// matt - 04/09/2000 - OK, this only works when the image is not resized during export

		// Get the dimensions of the image map area
		DocRect rectDocRect;
		rectDocRect = ms_Options.GetSizeOfExportArea(ms_Options.m_stExportArea);

		// use the matrix to work out the pixel values of height and width
		Matrix Identity;
		WinRect	rectWinRect = OSRenderRegion::BitmapDocRectToWin( Identity, rectDocRect, 96.0 );

		// save the width and height data
		Temp = " width=\"";
		*pFile << Temp;
		INT32 lToWrite = rectWinRect.right - rectWinRect.left;
		HTMLExportFilter::WriteNumber(lToWrite, pFile, NULL);
		Temp = "\"";
		*pFile << Temp;
		Temp = " height=\"";
		*pFile << Temp;
		lToWrite = rectWinRect.bottom - rectWinRect.top;
		HTMLExportFilter::WriteNumber(lToWrite, pFile, NULL);
		Temp = "\"";
		*pFile << Temp;
**************************************************************************************************/

		
		//Now write the remaining details
		
		Temp = " usemap=\"#";
		*pFile << Temp;
		*pFile << ms_Options.GetImagemapName();
		Temp = "\" border=\"0\">\r\n";
		*pFile << Temp;

	}

	//Write the imagemap to the file and/or to the clipboard
	Write(pFile);

	//If we have been passed a file to write to
	if (pFile!=NULL)
	{
		//Then close it.
		CloseImagemapFile(pFile);
	}
			
	//Delete the render region
	delete m_pRegion;
	m_pRegion=NULL;
	
	return TRUE;
}

/********************************************************************************************

>	BOOL ImagemapFilter::WriteExistingDataHelper(CCLexFile* pfileTo, CCLexFile* pfileFrom, PathName* ppathFrom)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/97
	Inputs:		pfileTo		Ptr to the file to copy to
				pfileFrom	Ptr to the file to copy from
				ppathFrom	Ptr to a pathname of the file to copy from

	Returns:	TRUE if OK
				FALSE if there were any errors

	Purpose:	This function does most of the hard work in writing an 
				imagemap into an existing file.

				It takes the data from pfileFrom, parses it and writes it
				to pfileTo, but inserting the imagemap data in an appropriate place.

				The appropriate place is, in order of preference:

				1. In place of an imagemap in pfileFrom that has the same
				   map name as the imagemap we are writing
				2. Before the </BODY> tag of pfileFrom has been written
				3. After all the data from pfileFrom has been written
    
********************************************************************************************/

BOOL ImagemapFilter::WriteExistingDataHelper(CCLexFile* pfileTo, CCLexFile* pfileFrom, PathName* ppathFrom)
{
	ERROR2IF(pfileTo == NULL,FALSE,"Bad params - pfileTo");
	ERROR2IF(pfileFrom == NULL,FALSE,"Bad params - pfileFrom");
	ERROR2IF(ppathFrom == NULL,FALSE,"Bad params - ppathFrom");

	//Set up some strings which we will search for
	String_256 strMap("MAP");
	String_256 strCloseMap("/MAP");
	String_256 strName("NAME");
	String_256 strCloseBody("/BODY");

	String_256 strImagemapName=ms_Options.GetImagemapName();
	strImagemapName.toUpper();

	//First we're going to do a first pass through the file

	//This will determine two things: one, if the file contains an existing image map
	//with the same name as the one we were about to write out. If it does,
	//we will overwrite it

	//Two, if the file contains a closing </BODY> tag. If it does, and if 
	//there is no existing image map in the file with the same name
	//as the one we were about to write out, we will write the imagemap
	//before that closing tag

	//Start off by assuming we're not going to overwrite an existing imagemap
	//and we're not going to insert the imagemap before the closing body tag
	BOOL fOverwriteExistingImageMap=FALSE;
	BOOL fInsertBeforeBodyTag=FALSE;
		
	//So open the file for reading
	CCDiskFile* pdiskfileFrom=(CCDiskFile*) pfileFrom;

	pdiskfileFrom->open(*ppathFrom, ios::in | ios::binary);
		
	//Initialise the HTML Lexer
	pfileFrom->InitLexer();

	//And until we reach the end of the file
	while(!pfileFrom->IsEndOfHTMLFile())
	{
		//Get the next HTML token in the file
		//This will either be a tag <xxxxxx>
		//or text (where text is in between tags)
		pfileFrom->GetHTMLToken(FALSE, FALSE);

		//Is that string a <MAP> tag with the same name as the
		//imagemap we are about to write out?
		if (pfileFrom->GetHTMLTagName()==strMap 
			&& pfileFrom->GetHTMLParameterValue(strName)==strImagemapName)
		{
			//Yes. So remember that we want to overwrite the
			//existing image map
			fOverwriteExistingImageMap=TRUE;
		}

		//Is that string a </BODY> tag?
		if (pfileFrom->GetHTMLTagName()==strCloseBody)
		{
			//Yes. So remember that we want to insert before the
			//closing body tag
			fInsertBeforeBodyTag=TRUE;
		}
	}

	//Finally, close the file and deinit the lexer
	pdiskfileFrom->close();
	pfileFrom->DeinitLexer();

	//Now go through the file again, and insert the imagemap in the correct place
							  
	//So open the file for reading
	pdiskfileFrom->open(*ppathFrom, ios::in | ios::binary);
		
	//Initialise the HTML Lexer
	pfileFrom->InitLexer();

	//And until we reach the end of the file
	while(!pfileFrom->IsEndOfHTMLFile())
	{
		//Get the next HTML token in the file
		//This will either be a tag <xxxxxx>
		//or text (where text is in between tags)
		pfileFrom->GetHTMLToken(FALSE, FALSE);

		//Now, if that tag is a closing BODY tag, and we should be inserting
		//the imagemap before that closing body tag, and we are not replacing
		//an existing imagemap
		if (pfileFrom->GetHTMLTagName()==strCloseBody 
			&& fInsertBeforeBodyTag
			&& !fOverwriteExistingImageMap) 
		{
			//Then write out our imagemap & image details now - Matt 31/08/2000
			String_256 Temp = "<img src=\"file:///";
			*pfileTo << Temp;
			*pfileTo << ms_Options.m_GraphicPath.GetPath(); // the path and file name of the graphic
			Temp = "\"";
			*pfileTo << Temp;

/*****************************************************************************************************
			// Get the dimensions of the image map area
			DocRect rectDocRect;
			rectDocRect = ms_Options.GetSizeOfExportArea(ms_Options.m_stExportArea);

			// use the matrix to work out the pixel values of height and width
			Matrix Identity;
			WinRect	rectWinRect = OSRenderRegion::BitmapDocRectToWin( Identity, rectDocRect, 96.0 );

			// save the width and height data
			Temp = " width=\"";
			*pfileTo << Temp;
			INT32 lToWrite = rectWinRect.right - rectWinRect.left;
			HTMLExportFilter::WriteNumber(lToWrite, pfileTo, NULL);
			Temp = "\"";
			*pfileTo << Temp;
			Temp = " height=\"";
			*pfileTo << Temp;
			lToWrite = rectWinRect.bottom - rectWinRect.top;
			HTMLExportFilter::WriteNumber(lToWrite, pfileTo, NULL);
			Temp = "\"";
			*pfileTo << Temp;
*************************************************************************************************/

			//Now write the remaining details
			Temp = " usemap=\"#";
			*pfileTo << Temp;
			*pfileTo << ms_Options.GetImagemapName();
			Temp = "\" border=\"0\">\r\n";
			*pfileTo << Temp;

			
			
			Write(pfileTo);
		}

		//If the tag is a MAP tag with the same
		//name as the map we are about to write out, and we should
		//be writing the imagemap in place of the existing imagemap
		if (pfileFrom->GetHTMLTagName()==strMap 
			&& pfileFrom->GetHTMLParameterValue(strName)==strImagemapName
			&& fOverwriteExistingImageMap)
		{
			//Then write out our imagemap
			Write(pfileTo);
			
			//Now we want to carry on copying the file we are reading from, 
			//starting from the closing </MAP> tag. So we must find that closing map
			//tag.

			//So read HTML tokens from the file until either we
			//find a </MAP> tag or we reach the end of the file
			while (pfileFrom->GetHTMLTagName()!=strCloseMap && !pfileFrom->IsEndOfHTMLFile())
			{
				//Get the next HTML token
				pfileFrom->GetHTMLToken(FALSE, FALSE);
			}

			//And if we haven't reached the end of the file, get another token,
			//which we will write out
			if (!pfileFrom->IsEndOfHTMLFile())
				pfileFrom->GetHTMLToken(FALSE, FALSE);
		}

		//Then get a pointer to the last HTML token we read
		TCHAR* pcBufferToWrite=pfileFrom->GetHTMLTokenBuffer();

		//And write it out
		if (pcBufferToWrite)
			pfileTo->write(pcBufferToWrite, _tcslen(pcBufferToWrite));
	}

	//Now, if we haven't inserted an imagemap before a closing body tag,
	//and we haven't replaced an existing image map, then we must write
	//out the imagemap now
	if (!fOverwriteExistingImageMap && !fInsertBeforeBodyTag)
	{
		//Then write out the imagemap now
		String_256 Temp = "<img src=\"file:///";
		*pfileTo << Temp;
		*pfileTo << ms_Options.m_GraphicPath.GetPath(); // the path and file name of the graphic
		Temp = "\"";
		*pfileTo << Temp;

/*****************************************************************************************************
		// Get the dimensions of the image map area
		DocRect rectDocRect;
		rectDocRect = ms_Options.GetSizeOfExportArea(ms_Options.m_stExportArea);

		// use the matrix to work out the pixel values of height and width
		Matrix Identity;
		WinRect	rectWinRect = OSRenderRegion::BitmapDocRectToWin( Identity, rectDocRect, 96.0 );

		// save the width and height data
		Temp = " width=\"";
		*pfileTo << Temp;
		INT32 lToWrite = rectWinRect.right - rectWinRect.left;
		HTMLExportFilter::WriteNumber(lToWrite, pfileTo, NULL);
		Temp = "\"";
		*pfileTo << Temp;
		Temp = " height=\"";
		*pfileTo << Temp;
		lToWrite = rectWinRect.bottom - rectWinRect.top;
		HTMLExportFilter::WriteNumber(lToWrite, pfileTo, NULL);
		Temp = "\"";
		*pfileTo << Temp;
******************************************************************************************************/

		//Now write the remaining details
		Temp = " usemap=\"#";
		*pfileTo << Temp;
		*pfileTo << ms_Options.GetImagemapName();
		Temp = "\" border=\"0\">\r\n";
		*pfileTo << Temp;



		Write(pfileTo);
	}

	//And finally close the file and deinit the lexer
	pdiskfileFrom->close();
	pfileFrom->DeinitLexer();

	
	return TRUE;
}

/********************************************************************************************

>	void ImagemapFilter::BuildMapNamesList(CCLexFile* pfileSearch, PathName* ppthSearch, List* plstToAdd)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/97
	Inputs:		plstToAdd	Pointer to the list to add the names to
				pfileSearch	Pointer to the file to search
				ppthSearch	Pointer to the path of the file to search

				
	Returns:	-

	Purpose:	Goes through the file pointed to by pfileSearch.

				Each time it finds an imagemap tag <MAP NAME=xxx>, it adds
				the value of the NAME parameter into the list pointed to by
				plstToAdd.

				This may seem a strange function to be an "entry point" to the filter,
				but it's here so that the file parsing code is available to 
				BaseBitmapFilter.
    
********************************************************************************************/

void ImagemapFilter::BuildMapNamesList(CCLexFile* pfileSearch, PathName* ppthSearch, List* plstToAdd)
{
	//First we must open the file for reading
	CCDiskFile* pDiskFile=(CCDiskFile*) pfileSearch;

	pDiskFile->open(*ppthSearch, ios::in | ios::binary);

	//Initialise the HTML lexer
	pfileSearch->InitLexer();

	//Set up two strings which we will search for
	String_256 strMap("MAP");
	String_256 strName("NAME");
	
	//And until we reach the end of the file
	while(!pfileSearch->IsEndOfHTMLFile())
	{
		//Get the next HTML token in the file
		pfileSearch->GetHTMLToken(TRUE, FALSE);

		//And if the token is an HTML MAP tag
		if (pfileSearch->GetHTMLTagName()==strMap)
		{
			//Then add the value of the NAME parameter to the list
			String_256 strToAdd=pfileSearch->GetHTMLParameterValue(strName, FALSE);

			StringListItem* psliToAdd=new StringListItem(strToAdd);

			plstToAdd->AddTail(psliToAdd);
		}
				
	}

	//And deinitialise the lexer
	pfileSearch->DeinitLexer();

	//Then close the file
	pDiskFile->close();
}





/********************************************************************************************

>	INT32 ImagemapFilter::WritePreamble(TCHAR* pcBuffer)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/2000
	Inputs:		pcBuffer		The text buffer to write to (may be NULL)

	Returns:	The number of TCHARs written to the text buffer
	Purpose:	Helper for writing imagemaps - this function will write the 'preamble' to
				pcBuffer. The 'preamble' consists of the image source, dimensions, which
				map it is bound to, and a tag indicating the border has 0 width.

				If this function is called with pcBuffer as NULL, it will return the length
				of text buffer required to write the current preamble information

	Notes:		pcBuffer is assumed to be null terminated

********************************************************************************************/

INT32 ImagemapFilter::WritePreamble(TCHAR* pcBuffer)
{
	String_256 temp;
	INT32 nChars = 0;

	temp = "<!-- HINT : The next line describes the file used for this image map. It can be deleted if you have already included this information in your HTML page. -->\r\n";

	if (pcBuffer)
	{
		//Hint line for novice users
		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), temp);

		//Image Path and filename...
		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T("<img src=\"file:///"));

		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(ms_Options.m_GraphicPath.GetPath())); // the path and file name of the graphic

		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T("\""));

/*******************************************************************************************************************************
		// Get the dimensions of the image map area
		DocRect rectDocRect;
		rectDocRect = ms_Options.GetSizeOfExportArea(ms_Options.m_stExportArea);

		// use the matrix to work out the pixel values of height and width
		Matrix Identity;
		WinRect	rectWinRect = OSRenderRegion::BitmapDocRectToWin( Identity, rectDocRect, 96.0 );

		//Image Width and Height...
		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(" width=\""));

		INT32 tempLong = rectWinRect.right - rectWinRect.left;
		nChars = _tcslen(pcBuffer);

		//Format the INT32 as a string
		temp.MakeMsg(_R(IDS_HTMLEXPORT_NUMBERFORMAT), tempLong);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(temp));

		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T("\""));


		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(" height=\""));

		tempLong = rectWinRect.bottom - rectWinRect.top;
		nChars = _tcslen(pcBuffer);

		//Format the INT32 as a string
		temp.MakeMsg(_R(IDS_HTMLEXPORT_NUMBERFORMAT), tempLong);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(temp));

		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T("\""));

************************************************************************************************************/

		//Now the remaining details...
		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(" usemap=\"#"));

		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T(ms_Options.GetImagemapName()));

		nChars = _tcslen(pcBuffer);
		_tcscpy(pcBuffer+nChars*sizeof(TCHAR), _T("\" border=\"0\">\r\n"));


		nChars = _tcslen(pcBuffer);

		return nChars;
	}

	//Hint line for novice users
	nChars += _tcslen(temp);

	//Image Path and filename...
	nChars += _tcslen(_T("<img src=\"file:///"));
	nChars += _tcslen(_T(ms_Options.m_GraphicPath.GetPath())); // the path and file name of the graphic
	nChars += _tcslen(_T("\""));

/************************************************************************************
	// Get the dimensions of the image map area
	DocRect rectDocRect;
	rectDocRect = ms_Options.GetSizeOfExportArea(ms_Options.m_stExportArea);

	// use the matrix to work out the pixel values of height and width
	Matrix Identity;
	WinRect	rectWinRect = OSRenderRegion::BitmapDocRectToWin( Identity, rectDocRect, 96.0 );

	//Image Width and Height...
	nChars += _tcslen(_T(" width=\""));
	INT32 tempLong = rectWinRect.right - rectWinRect.left;


	//Format our INT32
	temp.MakeMsg(_R(IDS_HTMLEXPORT_NUMBERFORMAT), tempLong);

	nChars += _tcslen(temp);
	nChars += _tcslen(_T("\""));

	nChars += _tcslen(_T(" height=\""));
	tempLong = rectWinRect.bottom - rectWinRect.top;


	//Format our INT32
	temp.MakeMsg(_R(IDS_HTMLEXPORT_NUMBERFORMAT), tempLong);

	nChars += _tcslen(temp);
	nChars += _tcslen(_T("\""));

************************************************************************************/

	//Now the remaining details...
	nChars += _tcslen(_T(" usemap=\"#"));
	nChars += _tcslen(_T(ms_Options.GetImagemapName()));
	nChars += _tcslen(_T("\" border=\"0\">\r\n"));

	return nChars*sizeof(TCHAR);
}
