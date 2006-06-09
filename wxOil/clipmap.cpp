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
// External clipboard interface - classes for encapsulating clipboard data mappings
// and for managing data transfers between the internal and external clipboards

/*
*/


#include "camtypes.h"

// -- clipboard includes
#include "clipext.h"
#include "clipint.h"
#include "clipmap.h"

// --- text includes  **** !!!! BODGE
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "layer.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "nodetxts.h"
#include "unicdman.h"

// --- Bitmap Mapping includes
#include "bitmpinf.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "jason.h"
#include "nodebmp.h"
#include "wbitmap.h"
#include "scrcamvw.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]

// ---
//#include "ccfile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "nev.h"

#include "cmxifltr.h"

// This class is defined in kernel\cliptype.h
CC_IMPLEMENT_MEMDUMP(InternalClipboardFormat, CC_CLASS_MEMDUMP)

CC_IMPLEMENT_DYNCREATE(ClipboardMapping, ListItem)

CC_IMPLEMENT_DYNCREATE(BodgeTextClipMap, ClipboardMapping)
CC_IMPLEMENT_DYNCREATE(BodgeUnicodeClipMap, ClipboardMapping)

CC_IMPLEMENT_DYNAMIC(CMXClipMap, ClipboardMapping)
CC_IMPLEMENT_DYNCREATE(CMX16ClipMap, CMXClipMap)
CC_IMPLEMENT_DYNCREATE(CMX32ClipMap, CMXClipMap)

#ifdef _DEBUG
CC_IMPLEMENT_DYNCREATE(RTFClipMap, ClipboardMapping)
#endif

CC_IMPLEMENT_DYNCREATE(BitmapClipMap, ClipboardMapping)
//CC_IMPLEMENT_DYNCREATE(PaletteClipMap, ClipboardMapping)
CC_IMPLEMENT_DYNCREATE(DIBClipMap, ClipboardMapping)

#if FALSE
CC_IMPLEMENT_DYNCREATE(QuarkPictureClipMap, ClipboardMapping)
#endif

#define new CAM_DEBUG_NEW




/********************************************************************************************

>	ClipboardMapping::ClipboardMapping()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Constructor

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	ClipboardMapping::CreateAndRegister

********************************************************************************************/

ClipboardMapping::ClipboardMapping()
{
	ERROR3("You can't directly construct an ClipboardMapping - Call CreateAndRegister");
}



/********************************************************************************************

>	ClipboardMapping::~ClipboardMapping()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Destructor

********************************************************************************************/

ClipboardMapping::~ClipboardMapping()
{
	RemoveTempFile();
}



/********************************************************************************************

>	ClipboardMapping::ClipboardMapping(ClipboardMappingType TheType, Filter *TheFilter,
										InternalClipboardFormat &TheInternalDataType,
										UINT32 TheExternalDataType,
										UINT32 ThePriority)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				TheFilter - The filter which can apply this conversion

				TheInternalDatatType - A class defining the internal data type
				(see cliptype.h)

				TheExternalDataType - A Windows CF_ constant defining the external
				clipboard data type which will be imported/exported.

				ThePriority - An integer indicating the priority of this mapping.
				The highest available priority mapping will be used in order to
				retain as much information in the data as possible. See
				docs\howtouse\ExtClip.doc for details of the existing mappings.

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	ClipboardMapping::CreateAndRegister; InternalClipboardFormat

********************************************************************************************/

ClipboardMapping::ClipboardMapping(ClipboardMappingType TheType, Filter *TheFilter,
									InternalClipboardFormat &TheInternalDataType,
									UINT32 TheExternalDataType,
									UINT32 ThePriority)
{
	Type		= TheType;
	pFilter		= TheFilter;

	InternalDataType.SetFormatID(TheInternalDataType.GetFormatID());
	ExternalDataType = RealExternalType = TheExternalDataType;

	Priority	= ThePriority;

	Available	= FALSE;

	tempfilename = NULL;
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	The OLE clipboard
//

#if (_OLE_VER >= 0x200)

/********************************************************************************************
>	void ClipboardMapping::SetRenderMemory(HGLOBAL hMem, DWORD cbMemSize)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/96
	Inputs:		hMem		---		the global memory to delay-render into, or null if
									the rendering functions themselves should allocate
									it.
				cbMemSize	---		the size of the given memory block.
	Purpose:	Sets the memory that the delay-render functions will render into.
	SeeAlso:	ExternalClipboard::SetRenderMemory; OpClipboardExport::DoWithParam
********************************************************************************************/

void ClipboardMapping::SetRenderMemory(HGLOBAL hMem, DWORD cbMemSize)
{
	// Record the handle and its size.
	m_hMem = hMem;
	m_cbMemSize = cbMemSize;
}

#endif

/////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************

>	static void ClipboardMapping::CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
													InternalClipboardFormat &TheInternalDataType,
													UINT32 TheExternalDataType,
													UINT32 ThePriority)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				TheFilter - The filter which can apply this conversion

				TheInternalDatatType - An object defining the internal data type
				(see cliptype.h)

				TheExternalDataType - A Windows CF_ constant defining the external
				clipboard data type which will be imported/exported.

				ThePriority - An integer indicating the priority of this mapping.
				The highest available priority mapping will be used in order to
				retain as much information in the data as possible. See
				docs\howtouse\ExtClip.doc for details of the existing mappings.

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	SeeAlso:	InternalClipboardFormat

********************************************************************************************/

void ClipboardMapping::CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
										 InternalClipboardFormat &TheInternalDataType,
										 UINT32 TheExternalDataType,
										 UINT32 ThePriority)
{
	ClipboardMapping *Mapping = new ClipboardMapping(TheType, TheFilter,
														TheInternalDataType, TheExternalDataType,
														ThePriority);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************

>	BOOL ClipboardMapping::ImportFromTempFile(TCHAR *filename, SelOperation *Caller,
												InternalClipboard *Dest)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		filename - the file name of the temporary scratch file to be loaded
				Dest - the internal clipboard document to import into


	Purpose:	Internal helper function. Invokes pFilter::DoImport into the Dest doc,
				using the given file as a source.

	Notes:		You must remove() the tem file yourself afterwards.

********************************************************************************************/

BOOL ClipboardMapping::ImportFromTempFile(TCHAR *filename, SelOperation *Caller,
											InternalClipboard *Dest)
{
	PathName FileToLoad(filename);
	
	CCLexFile* pFile = new CCDiskFile(1024, FALSE, TRUE);

	// See if we have a valid file
	if (pFile == NULL)
		return FALSE;

	// we have to try and open the file
	TRY
	{
		if (!((CCDiskFile*)pFile)->open(FileToLoad, ios::in | ios::binary))
		{
			// Failed to open the file...
			delete pFile;
			pFile = NULL;
			ERROR2RAW("Failed to open temp file for import into clipboard");
			return(FALSE);
		}

		// Found the Filter, so ask it to import the file please

		// Provide a current view for the filter to chomp on; We stack the current
		// view so that we do not corrupt it.
		View *OldCurrentView = View::GetCurrent();
		ClipboardView ImportView;
		if (!ImportView.Init())
		{
			delete pFile;
			pFile = NULL;
			return(FALSE);
		}

		ImportView.SetCurrent();

		if (!pFilter->DoImport(Caller, pFile, InternalClipboard::Instance()))
		{
			// Something went a bit wrong - tell the user what it was.
			// Only tell them if not special user cancelled error message
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
				InformError();

				// If the file is open, then close it
			if (pFile->isOpen())
				pFile->close();

			// and die a bit
			delete pFile;
			pFile = NULL;

			if (OldCurrentView != NULL)
				OldCurrentView->SetCurrent();
			return FALSE;
		}

		if (OldCurrentView != NULL)
			OldCurrentView->SetCurrent();

		// If the file is open, then close it
		if (pFile->isOpen())
			pFile->close();
	}

	// See if there was a file I/O error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			InformError();

		// Make sure that the file is closed
		TRY
		{
			// Close the file
			if (pFile->isOpen())
				pFile->close();
		}
		CATCH(CFileException, e)
		{
			// Not a lot we can do really...
		}
		END_CATCH

		// and fail
		delete pFile;
		pFile = NULL;
		return FALSE;
	}
	END_CATCH

	// Make sure that we have got rid of the file
	delete pFile;

	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL ClipboardMapping::HandleImport(SelOperation *Caller,
												HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.

	Returns:	TRUE for success

	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.

	Notes:		This base-class default treats the clipboard data as a file, and makes
				the associated filter import it directly. (Note: It is currently
				implemented by slaving a temp file to disc, in order to avoid CCMemFiles
				which are not currently good enough for us to use)

********************************************************************************************/

BOOL ClipboardMapping::HandleImport(SelOperation *Caller,
									HANDLE ClipboardData, InternalClipboard *Dest)
{
	BOOL ok = TRUE;

	// ---
	// Get a scratch file - if TMP isn't set, this will try for c:\temp.
	// The filename will have XS as a prefix
	char *tempname = GetTempFileName(); //_ttempnam("C:\temp", "XS~");
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(FALSE);
	}

	PathName FullPath(tempname);

	// ---
	// Save the clipboard data into our tempfile
	OFSTRUCT OpenBuf;
	OpenBuf.cBytes = sizeof(OpenBuf);
	HFILE OutFile = OpenFile(tempname, &OpenBuf, OF_CREATE);

	// Did it work ok?
	if (OutFile == HFILE_ERROR)
	{
		ERROR2RAW("Import tempfile couldn't be opened");
		InformError();
		free(tempname);
		return(FALSE);
	}

	// And write it to the tempfile
	LPCSTR lpbuffer = (LPCSTR) GlobalLock(ClipboardData);
	if (lpbuffer != NULL)
	{
		DWORD DataSize = GlobalSize(ClipboardData);

		if (DataSize != 0)
		{
			UINT32 BytesWritten = _lwrite(OutFile, lpbuffer, DataSize);

			if (BytesWritten != DataSize)
			{
				ERROR2RAW("Import tempfile save seems to have failed");
				InformError();
				ok = FALSE;
			}
		}
		else
		{
			ERROR3("No data in clipboard!");
			ok = FALSE;
		}

		GlobalUnlock(ClipboardData);
	}
	else
	{
		ERROR3("Clipboard memory chunk couldn't be locked!");
		ok = FALSE;
	}

	_lclose(OutFile);


	// ---
	// Now, import the temp file using our 'parent' filter
	if (ok)
		ok = ImportFromTempFile(tempname, Caller, InternalClipboard::Instance());


	// Delete the temp file, and deallocate the tempname memory
	_tremove(tempname);
	free(tempname);
	return(ok);
}



/********************************************************************************************

>	BOOL ClipboardMapping::ExportToTempFile(TCHAR *filename, 
										Operation *Caller, InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		filename - the file name of the temporary scratch file to be loaded
				Caller   - the operation we're being called from 
				Source   - the internal clipboard document to export from

	Returns:	TRUE if it succeeds. 

	Purpose:	Internal helper function. Invokes pFilter::DoExport from the Source doc,
				using the given file as a destination.

	Notes:		You must delete the temp file afterwards - use 'remove()'

********************************************************************************************/

BOOL ClipboardMapping::ExportToTempFile(TCHAR *filename, 
										Operation *Caller, InternalClipboard *Source)
{
	// Provide a current view for the filter to chomp on; We stack the current
	// view so that we do not corrupt it.
	View *OldCurrentView = View::GetCurrent();
	ClipboardView ExportView;
	if (!ExportView.Init())
		return(FALSE);

	ExportView.SetCurrent();

	PathName FullPath(filename);

	// Try and open the temporary file
	CCDiskFile DiskFile(1024, FALSE, TRUE);
	BOOL AllOK = TRUE;

	TRY
	{
		if (!DiskFile.open(FullPath, ios::out | ios::binary))
		{
			if (OldCurrentView != NULL)
				OldCurrentView->SetCurrent();
			return(FALSE);		// Failed to open the export file
		}

		// Tell the filter we would like a Preview Bitmap please
		if (pFilter->CanIncludePreviewBmp())
			pFilter->IncludePreviewBmp(TRUE);

		// Try and export the internal clipboard document
		if (!pFilter->DoExport(Caller, &DiskFile, &FullPath, InternalClipboard::Instance()))
		{
			// Something went a bit wrong - tell the user what it was.
			// Suppress the error if it was the 'user has cancelled one'
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			{
				InformError();
				// Caller should clean up the temp file
			}
			else
				Error::ClearError();	// otherwise remove the error so it won't get reported
			
			// Set the error
			AllOK = FALSE;
		}

		// Close the file
		if (DiskFile.isOpen())
			DiskFile.close();
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
		{
			InformError();
		}
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported

		// Make sure that the file is closed
		TRY
		{
			// Close that file
			if (DiskFile.isOpen())
				DiskFile.close();
		}
		CATCH(CFileException, e)
		{
			// Not a lot we can do really...
		}
		END_CATCH

		// And restore the previous current view
		if (OldCurrentView != NULL)
			OldCurrentView->SetCurrent();

		return(FALSE);
	}
	END_CATCH

	// Tell the filter we would NOT like a Preview Bitmap ready for next time
	if (pFilter->CanIncludePreviewBmp())
		pFilter->IncludePreviewBmp(FALSE);

	// And restore the previous current view
	if (OldCurrentView != NULL)
		OldCurrentView->SetCurrent();

	return(TRUE);
}

		
/********************************************************************************************

>	virtual BOOL ClipboardMapping::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard. Usually this just involves calling Filter::DoExport
				for the parent filter, and then returning the handle to the global memory
				block to be placed onto the external clipboard.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE ClipboardMapping::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	ERROR3("Base class ClipboardMapping::HandleExport called");

/*
	PathName FullPath;

	char *tempname = _ttempnam("C:\temp", "XS");
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(NULL);
	}

 	if (!ExportToTempFile(tempname, Caller, Source))
	{
		remove(tempname);
		free(tempname);
		return(NULL);
	}


	// In the switch statement below, set this handle to the global mem handle
	// of the data you want to place on the clipboard. If you don't want to
	// put anything on, leave this handle NULL.
	HANDLE hGlobalMem = NULL;


	// **** !!!! Export the file

	// Delete the temp file, and return the result
	remove(tempname);
	free(tempname);
	return(hGlobalMem);
*/
	return(NULL);
}



/********************************************************************************************

>	char *ClipboardMapping::GetTempFileName(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/95

	Returns:	NULL, or a a filename to be used as a temporary file

	Purpose:	Finds a temporary file
				Remove it with ClipboardMapping::RemoveTempFile

	SeeAlso:	ClipboardMapping::RemoveTempFile

********************************************************************************************/

char *ClipboardMapping::GetTempFileName(void)
{
	RemoveTempFile();	// Try to ensure any previous tempfile is gone

	tempfilename = _ttempnam("C:\temp", "XS~");

	return(tempfilename);
}



/********************************************************************************************

>	void ClipboardMapping::RemoveTempFile(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/95

	Purpose:	Removes the last temporary file 'allocated' with GetTempFilename

				This function is automatically called when the clipboard is wiped,
				as usually we had to leave the tempfile around while it was "on" the
				clipboard.

	SeeAlso:	ClipboardMapping::GetTempFilename

********************************************************************************************/

void ClipboardMapping::RemoveTempFile(void)
{
	if (tempfilename != NULL)
	{
		_tremove(tempfilename);
		free(tempfilename);
		tempfilename = NULL;
	}
}
















BodgeTextClipMap::BodgeTextClipMap()
{
	ERROR3("Please don't press that button again");
}


/********************************************************************************************

>	BodgeTextClipMap::BodgeTextClipMap(ClipboardMappingType TheType, UINT32 ClaimType = 0)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the
				windows clipboard - that is, create 3 of these mappings, specifying
				CF_TEXT, CF_UNICODETEXT, and CF_OEMTEXT, and they will all ask the
				clipboard for UNICODE text when they actually go to import. This allows
				us to detect the 3 implicitly-converted clipboard formats, and map them
				all to UNICODE, i.e. use the UNICODE mapping for all 3 available formats.
				If ClaimType == 0, UNICODE is assumed

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	BodgeTextClipMap::CreateAndRegister

********************************************************************************************/

BodgeTextClipMap::BodgeTextClipMap(ClipboardMappingType TheType, UINT32 ClaimType)
				: ClipboardMapping(TheType, NULL, InternalClipboardFormat(CLIPTYPE_TEXT),
									CF_TEXT, 50)
{
	if (ClaimType != 0)
		ExternalDataType = ClaimType;
}



/********************************************************************************************

>	static void BodgeTextClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the
				windows clipboard - that is, create 3 of these mappings, specifying
				CF_TEXT, CF_UNICODETEXT, and CF_OEMTEXT, and they will all ask the
				clipboard for UNICODE text when they actually go to import. This allows
				us to detect the 3 implicitly-converted clipboard formats, and map them
				all to UNICODE, i.e. use the UNICODE mapping for all 3 available formats.
				If ClaimType == 0, UNICODE is assumed

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/

void BodgeTextClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	BodgeTextClipMap *Mapping = new BodgeTextClipMap(TheType, ClaimType);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************
>	virtual BOOL BodgeTextClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.
	Returns:	TRUE for success
	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.
				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.
********************************************************************************************/

BOOL BodgeTextClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest)
{
	char *buff=(char*)GlobalLock(ClipboardData);
	if (buff != NULL && buff[0] != 0)
	{
		// position arbitarily on clipboard - when pasted the clip contents are centered anyway
		TextStory* TheStory=TextStory::CreateFromChars(DocCoord(100,100), buff, NULL,
													   InternalClipboard::Instance(), NULL, TRUE);

		// Unlock the clipboard data, as we won't need it again
		GlobalUnlock(ClipboardData);

		if (TheStory!=NULL)
		{
			// OK, attach the new story to the clipboard document
			TheStory->AttachNode(InternalClipboard::GetInsertionLayer(), LASTCHILD);
			TheStory->NormaliseAttributes();

			// And make it reformat itself properly
			TheStory->FormatAndChildren(NULL,FALSE,FALSE);
			return(TRUE);
		}

		// We failed (no text story), so report and ensure the clipboard is 'clean'
		InformError();
		InternalClipboard::Clear();
	}

	// We failed
	return(FALSE);
}

//BOOL BodgeTextClipMap::HandleImport(SelOperation *Caller,
//									HANDLE ClipboardData, InternalClipboard *Dest)
//{
//	char *buff = (char *) GlobalLock(ClipboardData);
//	if (buff != NULL && buff[0] != 0)
//	{
//		DWORD Index = 0;
//  
//		TextStory *pStory = new TextStory(Dest->GetInsertionLayer(), LASTCHILD);
//		if (pStory == NULL)
//			goto Abort;
//  
//		// Lob in some default attributes (black fill, no line colour, line width = 250
//		Node *pNode;
//		FlatFillAttribute *pAttrFill = new FlatFillAttribute(DocColour(COLOUR_BLACK));
//		if (pAttrFill == NULL)
//			goto Abort;
//		pNode = pAttrFill->MakeNode();
//		if (pNode == NULL)
//			goto Abort;
//		pNode->AttachNode(pStory, LASTCHILD, FALSE);
//  
//		StrokeColourAttribute *pAttrLine = new StrokeColourAttribute(DocColour(COLOUR_TRANS));
//		if (pAttrLine == NULL)
//			goto Abort;
//		pNode = pAttrLine->MakeNode();
//		if (pNode == NULL)
//			goto Abort;
//		pNode->AttachNode(pStory, LASTCHILD, FALSE);
//
//		LineWidthAttribute *pAttrLineWidth = new LineWidthAttribute(250);
//		if (pAttrLineWidth == NULL)
//			goto Abort;
//		pNode = pAttrLineWidth->MakeNode();
//		if (pNode == NULL)
//			goto Abort;
//		pNode->AttachNode(pStory, LASTCHILD, FALSE);
//  
//		// Set the top left position for the text area - this doesn't matter because
//		// the absolute position on the clipboard is ignored when we paste
//		Matrix StoryMatrix(1000, 1000);
//		pStory->SetStoryMatrix(StoryMatrix);
//  
//		TextLine *pLine = new TextLine(pStory, LASTCHILD);
//		if (pLine == NULL)
//			goto Abort;
//  
//		Node *pChar = NULL;
//
//		while(buff[Index] != 0)
//		{
//			if (buff[Index] == 0x0D)
//			{
//				if (buff[Index+1] == 0x0A)	// Skip LF if it's a CRLF pair
//					Index++;
//  
//				pChar = new EOLNode(pLine, LASTCHILD);
//				if (pChar == NULL)
//					goto Abort;
//  
//				// Make sure if this is the last char exported, we don't leave
//				// this next TextLine without an EOLNode in it!
//				pChar = NULL;
//
//				pLine = new TextLine(pStory, LASTCHILD);
//				if (pLine == NULL)
//					goto Abort;
//			}
//			else
//			{
//				// Ignore control chars like tabs and suchlike for now
//				if (buff[Index] > 31)
//				{
//					pChar = new TextChar(pLine, LASTCHILD, (WCHAR) buff[Index]);
//					if (pChar == NULL)
//						goto Abort;
//				}
//			}
//			
//			Index++;
//		}
//  
//		if (pChar == NULL || !pChar->IsKindOf(CC_RUNTIME_CLASS(EOLNode)))
//		{
//			// no terminating newline, so add one
//			pChar = new EOLNode(pLine, LASTCHILD);
//			if (pChar == NULL)
//				goto Abort;
//		}
//  
//		// And lob a caret in, 'cos it seems to get upset if this isn't in there!
//		pChar = new CaretNode(pChar, PREV);
//		if (pChar == NULL)
//			goto Abort;
//  
//		// And finally, format the text, else the chars all stack on top of each other!
//		pStory->FormatAndChildren();
//
//		GlobalUnlock(ClipboardData);
//		return(TRUE);
//	}
//
//	return(FALSE);
//
//Abort:
//	ERROR2RAW("Text import failed, probably due to lack of memory");
//	InformError();
//
//	GlobalUnlock(ClipboardData);
//	Dest->ClearClipboard();	// We failed, so ensure the doc is wiped
//	return(FALSE);
//}

		
/********************************************************************************************

>	virtual BOOL BodgeTextClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE BodgeTextClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	const DWORD Increment = 1024 * sizeof(char);
	DWORD CurrentSize	= 0;
	DWORD MaxSize		= Increment;

#if (_OLE_VER >= 0x200)

	// If necessary allocate the memory.
	BOOL fDidAlloc = FALSE;
	HANDLE hGlobalMem = m_hMem;
	if (hGlobalMem)
	{
		// We have some already.  Set the size.
		MaxSize = m_cbMemSize;
	}
	else
	{
		// We don't have any, so try to allocate and remember that we did so.
		hGlobalMem = m_hMem = GlobalAlloc(GHND, MaxSize);
		if (!hGlobalMem) return 0;
		fDidAlloc = TRUE;
	}

#else

	// Allocate a block of global memory to store the text
	HANDLE hGlobalMem = GlobalAlloc(GHND, MaxSize);

	if (hGlobalMem == NULL)
		return(NULL);

#endif

	// Get a UNICODE text buffer
	char* buff = (char*) GlobalLock(hGlobalMem);
	if (buff == NULL)
	{
		return(NULL);
	}

	// Scan the entire document tree, and lob every TextChar we find into the export buffer
	// NOTE that the Global{Re}Alloc zeros memory, so we don't need to worry about 0-termination.
	Node *pSubtree = InternalClipboard::GetInsertionLayer();
	Node *pNode = pSubtree->FindFirstDepthFirst();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(TextChar)) || pNode->IsKindOf(CC_RUNTIME_CLASS(EOLNode)))
		{
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(TextChar)))
			{
				UINT32 ComposedChar = UnicodeManager::UnicodeToMultiByte(((TextChar *) pNode)->GetUnicodeValue());
				BYTE LeadByte = 0;
				BYTE TrailByte = 0;
				UnicodeManager::DecomposeMultiBytes(ComposedChar, &LeadByte, &TrailByte);

				if (LeadByte != 0)
					buff[CurrentSize++] = LeadByte;

				buff[CurrentSize++] = TrailByte;
			}
			else
			{
				if (!((EOLNode*)pNode)->IsVirtual())
				{
					buff[CurrentSize++] = (char) 0x0D;		// CRLF - newline
					buff[CurrentSize++] = (char) 0x0A;
				}
			}

			// Check if we've overrun the buffer - if so, we need to make the buffer bigger
			// (Allow 3 entries at the end (1 to guarantee we have a zero terminator, and 2 more
			// to allow room to fit the 2-char newline sequence in!)
			if ( (CurrentSize * sizeof(char)) >= MaxSize - 3)
			{
			#if (_OLE_VER >= 0x200)
				// If we didn't allocate the block then we can't resize it.
				if (!fDidAlloc) return 0;
				m_hMem = 0;
			#endif
				
				GlobalUnlock(hGlobalMem);

				MaxSize += Increment;
				HANDLE hNewMem = GlobalReAlloc(hGlobalMem, MaxSize, GHND);
				if (hNewMem == NULL)
				{
					GlobalFree(hGlobalMem);
					return(NULL);
				}

				hGlobalMem = hNewMem;
				buff = (char *)GlobalLock(hGlobalMem);

				if (buff == NULL)
				{
					GlobalFree(hGlobalMem);
					return(NULL);
				}

			#if (_OLE_VER >= 0x200)
				// Remember this block.
				m_hMem = hNewMem;
			#endif
			}
		}
		pNode = pNode->FindNextDepthFirst(pSubtree);
	}

	// We must unlock the block before giving it to the clipboard
	GlobalUnlock(hGlobalMem);

	return(hGlobalMem);
}










BodgeUnicodeClipMap::BodgeUnicodeClipMap()
{
	ERROR3("Please don't press that button again");
}


/********************************************************************************************

>	BodgeUnicodeClipMap::BodgeUnicodeClipMap(ClipboardMappingType TheType, UINT32 ClaimType = 0)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the
				windows clipboard - that is, create 3 of these mappings, specifying
				CF_TEXT, CF_UNICODETEXT, and CF_OEMTEXT, and they will all ask the
				clipboard for UNICODE text when they actually go to import. This allows
				us to detect the 3 implicitly-converted clipboard formats, and map them
				all to UNICODE, i.e. use the UNICODE mapping for all 3 available formats.
				If ClaimType == 0, UNICODE is assumed

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	BodgeUnicodeClipMap::CreateAndRegister

********************************************************************************************/

BodgeUnicodeClipMap::BodgeUnicodeClipMap(ClipboardMappingType TheType, UINT32 ClaimType)
				: ClipboardMapping(TheType, NULL, InternalClipboardFormat(CLIPTYPE_TEXT),
									CF_UNICODETEXT, 55)
{
	if (ClaimType != 0)
		ExternalDataType = ClaimType;
}



/********************************************************************************************

>	static void BodgeUnicodeClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the
				windows clipboard - that is, create 3 of these mappings, specifying
				CF_TEXT, CF_UNICODETEXT, and CF_OEMTEXT, and they will all ask the
				clipboard for UNICODE text when they actually go to import. This allows
				us to detect the 3 implicitly-converted clipboard formats, and map them
				all to UNICODE, i.e. use the UNICODE mapping for all 3 available formats.
				If ClaimType == 0, UNICODE is assumed

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/

void BodgeUnicodeClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	BodgeUnicodeClipMap *Mapping = new BodgeUnicodeClipMap(TheType, ClaimType);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************
>	virtual BOOL BodgeUnicodeClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95
	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.
	Returns:	TRUE for success
	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.
				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.
********************************************************************************************/

BOOL BodgeUnicodeClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest)
{
	WCHAR *buff=(WCHAR*)GlobalLock(ClipboardData);
	if (buff != NULL && buff[0] != 0)
	{
		// position arbitarily on clipboard - when pasted the clip contents are centered anyway
		TextStory* TheStory=TextStory::CreateFromChars(DocCoord(100,100), NULL, buff,
													   InternalClipboard::Instance(), NULL, TRUE);

		// Unlock the clipboard data, as we won't need it again
		GlobalUnlock(ClipboardData);

		if (TheStory!=NULL)
		{
			// OK, attach the new story to the clipboard document
			TheStory->AttachNode(InternalClipboard::GetInsertionLayer(), LASTCHILD);
			TheStory->NormaliseAttributes();

			// And make it reformat itself properly
			TheStory->FormatAndChildren(NULL,FALSE,FALSE);
			return(TRUE);
		}

		// We failed (no text story), so report and ensure the clipboard is 'clean'
		InformError();
		InternalClipboard::Clear();
	}

	// We failed
	return(FALSE);
}


//BOOL BodgeUnicodeClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest)
//{
//	WCHAR *buff = (WCHAR *) GlobalLock(ClipboardData);
//	if (buff != NULL && buff[0] != 0)
//	{
//		DWORD Index = 0;
//
//		TextStory *pStory = new TextStory(Dest->GetInsertionLayer(), LASTCHILD);
//		if (pStory == NULL)
//			goto Abort;
//
//		// Lob in some default attributes (black fill, no line colour, line width = 250
//		Node *pNode;
//		FlatFillAttribute *pAttrFill = new FlatFillAttribute(DocColour(COLOUR_BLACK));
//		if (pAttrFill == NULL)
//			goto Abort;
//		pNode = pAttrFill->MakeNode();
//		if (pNode == NULL)
//			goto Abort;
//		pNode->AttachNode(pStory, LASTCHILD, FALSE);
//
//		StrokeColourAttribute *pAttrLine = new StrokeColourAttribute(DocColour(COLOUR_TRANS));
//		if (pAttrLine == NULL)
//			goto Abort;
//		pNode = pAttrLine->MakeNode();
//		if (pNode == NULL)
//			goto Abort;
//		pNode->AttachNode(pStory, LASTCHILD, FALSE);
//
//		LineWidthAttribute *pAttrLineWidth = new LineWidthAttribute(250);
//		if (pAttrLineWidth == NULL)
//			goto Abort;
//		pNode = pAttrLineWidth->MakeNode();
//		if (pNode == NULL)
//			goto Abort;
//		pNode->AttachNode(pStory, LASTCHILD, FALSE);
//
//		// Set the top left position for the text area - this doesn't matter because
//		// the absolute position on the clipboard is ignored when we paste
//		Matrix StoryMatrix(1000, 1000);
//		pStory->SetStoryMatrix(StoryMatrix);
//  
//		TextLine *pLine = new TextLine(pStory, LASTCHILD);
//		if (pLine == NULL)
//			goto Abort;
//
//		Node *pChar = NULL;
//
//		while(buff[Index] != 0)
//		{
//			if (buff[Index] == 0x000D)
//			{
//				if (buff[Index+1] == 0x000A)	// Skip LF if it's a CRLF pair
//					Index++;
//
//				pChar = new EOLNode(pLine, LASTCHILD);
//				if (pChar == NULL)
//					goto Abort;
//
//				// Make sure if this is the last char exported, we don't leave
//				// this next TextLine without an EOLNode in it!
//				pChar = NULL;
//							
//				pLine = new TextLine(pStory, LASTCHILD);
//				if (pLine == NULL)
//					goto Abort;
//			}
//			else
//			{
//				// Ignore control chars like tabs and suchlike for now
//				if (buff[Index] > 31)
//				{
//					pChar = new TextChar(pLine, LASTCHILD, buff[Index]);
//					if (pChar == NULL)
//						goto Abort;
//				}
//			}
//			
//			Index++;
//		}
//
//		if (pChar == NULL || !pChar->IsKindOf(CC_RUNTIME_CLASS(EOLNode)))
//		{
//			// no terminating newline, so add one
//			pChar = new EOLNode(pLine, LASTCHILD);
//			if (pChar == NULL)
//				goto Abort;
//		}
//
//		// And lob a caret in, 'cos it seems to get upset if this isn't in there!
//		pChar = new CaretNode(pChar, PREV);
//		if (pChar == NULL)
//			goto Abort;
//
//		// And finally, format the text, else the chars all stack on top of each other!
//		pStory->FormatAndChildren();
//
//		GlobalUnlock(ClipboardData);
//		return(TRUE);
//	}
//
//	return(FALSE);
//
//Abort:
//	ERROR2RAW("Text import failed, probably due to lack of memory");
//	InformError();
//
//	GlobalUnlock(ClipboardData);
//	Dest->ClearClipboard();	// We failed, so ensure the doc is wiped
//	return(FALSE);
//}

		
/********************************************************************************************

>	virtual BOOL BodgeUnicodeClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE BodgeUnicodeClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	const DWORD Increment = 1024 * sizeof(WCHAR);
	DWORD CurrentSize	= 0;
	DWORD MaxSize		= Increment;

#if (_OLE_VER >= 0x200)

	// If necessary allocate the memory.
	BOOL fDidAlloc = FALSE;
	HANDLE hGlobalMem = m_hMem;
	if (hGlobalMem)
	{
		// We have some already.  Set the size.
		MaxSize = m_cbMemSize;
	}
	else
	{
		// We don't have any, so try to allocate and remember that we did so.
		hGlobalMem = m_hMem = GlobalAlloc(GHND, MaxSize);
		if (!hGlobalMem) return 0;
		fDidAlloc = TRUE;
	}

#else

	// Allocate a block of global memory to store the text
	HANDLE hGlobalMem = GlobalAlloc(GHND, MaxSize);

	if (hGlobalMem == NULL)
		return(NULL);

#endif

	// Get a UNICODE text buffer
	WCHAR *buff = (WCHAR *)GlobalLock(hGlobalMem);
	if (buff == NULL)
	{
		return(NULL);
	}

	// Scan the entire document tree, and lob every TextChar we find into the export buffer
	// NOTE that the Global{Re}Alloc zeros memory, so we don't need to worry about 0-termination.
	Node *pSubtree = InternalClipboard::GetInsertionLayer();
	Node *pNode = pSubtree->FindFirstDepthFirst();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(TextChar)) || pNode->IsKindOf(CC_RUNTIME_CLASS(EOLNode)))
		{
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(TextChar)))
			{
				buff[CurrentSize] = ((TextChar *) pNode)->GetUnicodeValue();
				CurrentSize++;
			}
			else
			{
				buff[CurrentSize++] = 0x000D;		// CRLF - newline
				buff[CurrentSize++] = 0x000A;
			}

			// Check if we've overrun the buffer - if so, we need to make the buffer bigger
			// (Allow 3 entries at the end (1 to guarantee we have a zero terminator, and 2 more
			// to allow room to fit the 2-char newline sequence in!)
			if ( (CurrentSize * sizeof(WCHAR)) >= MaxSize - 3)
			{
			#if (_OLE_VER >= 0x200)
				// If we didn't allocate the block then we can't resize it.
				if (!fDidAlloc) return 0;
				m_hMem = 0;
			#endif
				
				GlobalUnlock(hGlobalMem);

				MaxSize += Increment;
				HANDLE hNewMem = GlobalReAlloc(hGlobalMem, MaxSize, GHND);
				if (hNewMem == NULL)
				{
					GlobalFree(hGlobalMem);
					return(NULL);
				}

				hGlobalMem = hNewMem;
				buff = (WCHAR *)GlobalLock(hGlobalMem);

				if (buff == NULL)
				{
					GlobalFree(hGlobalMem);
					return(NULL);
				}

			#if (_OLE_VER >= 0x200)
				// Remember this block.
				m_hMem = hNewMem;
			#endif
			}
		}
		pNode = pNode->FindNextDepthFirst(pSubtree);
	}

	// We must unlock the block before giving it to the clipboard
	GlobalUnlock(hGlobalMem);

	return(hGlobalMem);
}

BitmapClipMap::BitmapClipMap()
{
	ERROR3("Please don't press that button again");
}


/********************************************************************************************

>	BitmapClipMap::BitmapClipMap(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - CF_DIB for straight import, CF_BITMAP for an alias

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	BitmapClipMap::CreateAndRegister

********************************************************************************************/

BitmapClipMap::BitmapClipMap(ClipboardMappingType TheType, UINT32 ClaimType)
				: ClipboardMapping(TheType, NULL, InternalClipboardFormat(CLIPTYPE_BITMAP),
									CF_BITMAP, 75)
{
	if (ClaimType != 0)
		ExternalDataType = ClaimType;
}



/********************************************************************************************

>	static void BitmapClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - CF_DIB for straight import, CF_BITMAP for an alias

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/

void BitmapClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	BitmapClipMap *Mapping = new BitmapClipMap(TheType, ClaimType);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************

>	static BOOL ConvertBMPToDIB(HBITMAP hSourceBmp, HPALETTE hSourcePal,
								BITMAPINFOHEADER **ResultHeader, LPBYTE *ResultBits)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/95

	Inputs:		hSourceBmp - The handle of the source bitmap GDI object
				hSourcePal - NULL, or a handle of the palette to use

	Outputs:	ResultHeader - Will be NULL or point at the resulting DIB header
				ResultBits	 - Will be NULL or point at the resulting DIB body

	Returns:	TRUE for success

	Purpose:	Converts a Windows HBITMAP GDI DDB into a DIB

	Notes:		Both returned pointers will be NULL unless the return value is TRUE.
				These pointers are CCMalloc'd blocks as used by other DIBUtil calls
				and WBitmaps.

				If hSourcePal is not supplied, a default palette will be used

	SeeAlso:	DibUtil::AloocDIB; DIBUtil::FreeDIB

********************************************************************************************/

static BOOL ConvertBMPToDIB(HBITMAP hSourceBmp, HPALETTE hSourcePal,
							LPBITMAPINFO *ResultHeader, LPBYTE *ResultBits)
{
	ERROR3IF(hSourceBmp == NULL || ResultHeader == NULL ||
				ResultBits == NULL, "Illegal NULL params");

	// Safe results if we fail
	*ResultHeader	= NULL;
	*ResultBits		= NULL;
	
	BITMAPINFOHEADER *Header;
	LPBYTE Bits;

	// See if we can get a palette from the clipboard - if not, grab a default one
	if (hSourcePal == NULL)
		hSourcePal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Fill in the BITMAP info from the source bitmap
	BITMAP bm;
	GetObject(hSourceBmp, sizeof(bm), (LPSTR) &bm);

	UINT32 Depth = bm.bmPlanes * bm.bmBitsPixel;
	if (Depth > 8)		// DDBs can be 16bpp, but DIBs cannot -> convert it to 24bpp
		Depth = 24;

	INT32 PaletteSize = DIBUtil::CalcPaletteSize(Depth, FALSE);

	Header = (BITMAPINFOHEADER *) CCMalloc(sizeof(BITMAPINFOHEADER) + PaletteSize);
	if (Header == NULL)
		return(FALSE);		

	INT32 BitmapSize = DIBUtil::ScanlineSize(bm.bmWidth, Depth) * bm.bmHeight;
	if (BitmapSize == 0)	// Make sure we correctly handle zero width/height
		BitmapSize = 4;

	Header->biSize               = sizeof(BITMAPINFOHEADER);
	Header->biWidth              = bm.bmWidth;
	Header->biHeight             = bm.bmHeight;
	Header->biPlanes             = 1;
	Header->biBitCount           = Depth;
	Header->biCompression        = BI_RGB;
	Header->biSizeImage          = BitmapSize;
	Header->biXPelsPerMeter      = 0;
	Header->biYPelsPerMeter      = 0;
	Header->biClrUsed            = 0;
	Header->biClrImportant       = 0;

	// Add padding for Gavin rampancy etc
	BitmapSize += EXTRA_GAVIN_BYTES + DIBUtil::ScanlineSize(bm.bmWidth, Depth);

	Bits = (LPBYTE) CCMalloc(BitmapSize);
	if (Bits == NULL)
	{
		CCFree(Header);
		return(FALSE);
	}

	// Get a screen DC, and select in the palette
	HDC hDC = GetDC(NULL);
	if (Depth <= 8)
	{
		hSourcePal = SelectPalette(hDC, hSourcePal, FALSE);
		RealizePalette(hDC);
	}

	// Call GetDIBits to blit the bitmap data across
	INT32 result = GetDIBits(hDC, hSourceBmp,	0, (WORD)Header->biHeight,
							Bits, (LPBITMAPINFO) Header, DIB_RGB_COLORS);

	// Restore the screen DC
	if (Depth <= 8)
		SelectPalette(hDC, hSourcePal, FALSE);
	ReleaseDC(NULL, hDC);

	// Result will be zero for failure, else the number of scanlines copied
	if (result == 0)
	{
		CCFree(Bits);
		CCFree(Header);
		return(FALSE);
	}

	if (Depth <= 8)
	{
		// If either of these fields are 0, they mean "The maximum number of colours"
		// and this doesn't export quite right, so we "fix" the value (eg 8bpp=>256 cols)
		if (Header->biClrUsed == 0)
			Header->biClrUsed = 1<<Depth;

		if (Header->biClrImportant == 0)
			Header->biClrImportant = 1<<Depth;
	}

	// And return successfully
	*ResultHeader	= (LPBITMAPINFO) Header;
	*ResultBits		= Bits;
	return(TRUE);
}



/********************************************************************************************

>	virtual BOOL BitmapClipMap::HandleImport(SelOperation *Caller,
												HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.

	Returns:	TRUE for success

	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

	SeeAlso:	DIBClipMap::HandleImport

********************************************************************************************/

BOOL BitmapClipMap::HandleImport(SelOperation *Caller,
									HANDLE ClipboardData, InternalClipboard *Dest)
{
	LPBYTE Bits = NULL;
	LPBITMAPINFO BmpInfo = NULL;
	WinBitmap *WinBmp = NULL;
	KernelBitmap *KernelBmp = NULL;
//	NodeBitmap *pNode = NULL;

	// Try to retrieve the palette from the clipboard - if there is not one there,
	// we'll get a NULL, which ConvertBMPToDIB is quite happy with.
	HPALETTE hPalette = (HPALETTE) GetClipboardData(CF_PALETTE);
	if (!ConvertBMPToDIB((HBITMAP)ClipboardData, hPalette, /* TO */ &BmpInfo, &Bits))
		goto Abort;

	WinBmp = new WinBitmap(BmpInfo, Bits);
	if (WinBmp == NULL)
		goto Abort;

	KernelBmp = new KernelBitmap(WinBmp);
	if (KernelBmp == NULL)
		goto Abort;

#if FALSE
/*
	// And now, lob a bitmap node for it into the clipboard
	pNode = new NodeBitmap();
	if (pNode == NULL || pNode->GetBitmapRef() == NULL || !(pNode->SetUpPath(12,12)))
		goto Abort;

	// Attach this bitmap reference to the new bitmap, and put in the clipboard document
	pNode->GetBitmapRef()->Attach(KernelBmp, Dest);

	// And set up the node's bounds appropriately
	BitmapInfo Info;
	pNode->GetBitmap()->ActualBitmap->GetInfo(&Info);

	{
		DocRect BoundsRect;
		BoundsRect.lo.x = 0;
		BoundsRect.lo.y = 0;
		BoundsRect.hi.x = Info.RecommendedWidth/2;
		BoundsRect.hi.y = Info.RecommendedHeight/2;

		pNode->CreateShape(BoundsRect);
	}

	pNode->AttachNode(Dest->GetInsertionLayer(), LASTCHILD, FALSE);
*/
#else
	{
		// Compile information on where to put the imported NodeBitmap
		PageDropInfo DropInfo;

		DropInfo.pDoc		= InternalClipboard::Instance();
		DropInfo.pDocView	= DocView::GetCurrent();
		DropInfo.pSpread	= InternalClipboard::Instance()->FindFirstSpread();	//GetInsertionLayer()->FindParent();
		DropInfo.DropPos	= DocCoord(100,100);		// Position is unimportant as it is centered when pasted
		DropInfo.TargetHit	= NO_TARGET;
		DropInfo.pObjectHit	= NULL;

		// Invoke an Op to create a NodeBitmap for us
		OpDescriptor* OpCreateNodeBmp = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(OpCreateNodeBitmap));
		OpCreateNodeBmp->Invoke(&OpParam((INT32)KernelBmp,(INT32)&DropInfo));
	}
#endif

	return(TRUE);

Abort:
//	ERROR2RAW("Bitmap import failed, probably due to lack of memory");
	InformError();

	if (KernelBmp != NULL)
		delete KernelBmp;

	if (WinBmp != NULL)
		delete WinBmp;
	else if (BmpInfo != NULL || Bits != NULL)
		FreeDIB(BmpInfo, Bits);

	InternalClipboard::Clear();	// We failed, so ensure the doc is wiped

	return(FALSE);
}



/********************************************************************************************

>	static HPALETTE CreateBMPPalette(LPBITMAPINFOHEADER lpbi)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:		lpbi - A pointer to your DIB's header

	Returns:	NULL (failed, or no palette), or a windows handle of the GDI palette data

	Purpose:	Given a DIB, creates a GDI HPALETTE based on the DIB's palette.
				If the DIB is 24 or 32 bit or whatever, this will return NULL, so it is a
				quite normal return value.

********************************************************************************************/

static HPALETTE CreateBMPPalette(LPBITMAPINFOHEADER lpbi)
{
	ERROR3IF(lpbi == NULL, "Illegal NULL param");

	// Is it a windows DIB? If not, we can't get a palette out of it
	if (lpbi->biSize != sizeof(BITMAPINFOHEADER))
		return NULL;

	// Get a pointer to the colour table and the number of colours in it
	RGBQUAD *pRGB = (RGBQUAD *)((LPSTR)lpbi + (WORD)lpbi->biSize);
	INT32 NumColours = DIBUtil::CalcPaletteSize(lpbi->biBitCount, lpbi->biCompression == BI_BITFIELDS) / sizeof(RGBQUAD);

	HPALETTE hpal = NULL;

	if (NumColours > 0)
	{
		// Allocate temporary memory
		LOGPALETTE *pPal = (LOGPALETTE *) LocalAlloc(LPTR, sizeof(LOGPALETTE) + NumColours * sizeof(PALETTEENTRY));
		if (pPal == NULL)
			return NULL;

		// Set up the LOGPALETTE structure, copying the pal entries across
		pPal->palNumEntries = NumColours;
		pPal->palVersion    = 0x300;		// Palette structure Windows version number

		for (INT32 i = 0; i < NumColours; i++)
		{
			pPal->palPalEntry[i].peRed   = pRGB[i].rgbRed;
			pPal->palPalEntry[i].peGreen = pRGB[i].rgbGreen;
			pPal->palPalEntry[i].peBlue  = pRGB[i].rgbBlue;
			pPal->palPalEntry[i].peFlags = (BYTE)0;
		}
		hpal = CreatePalette(pPal);
		LocalFree((HANDLE)pPal);
	}

	return hpal;
}



/********************************************************************************************

>	static KernelBitmap *FindBitmapToExport(InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Inputs:		Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a KernelBitmap to be exported

	Purpose:	Scans the clipboard for bitmap objects that can be exported, and
				returns the first one found.

				Used by the Bitmap, DIB, and Palette exporters to find the export data

********************************************************************************************/

static KernelBitmap *FindBitmapToExport(InternalClipboard *Source)
{
	KernelBitmap *KernelBmp = NULL;

	// Scan the entire document tree, looking for a bitmap to export
	Node *pSubtree = InternalClipboard::GetInsertionLayer();
	Node *pNode = pSubtree->FindFirstDepthFirst();

	while (pNode != NULL && KernelBmp == NULL)
	{
		// Is this node a NodeBitmap?
		if (pNode->IsAnObject() && pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBitmap))
		{
			NodeBitmap* pNodeBmp = (NodeBitmap*)pNode;
			KernelBitmapRef *BmpRef = pNodeBmp->GetBitmapRef();
			if (BmpRef != NULL)
				KernelBmp = BmpRef->GetBitmap();
		}
/*
		// How about a bitmap fill?
		else if (pNode->IsAnAttribute())
		{
			if (((NodeAttribute*)pNode)->IsABitmapFill())
			{
				// We've found either a Bitmap or Fractal fill
				AttrFillGeometry* pBmpFill = (AttrFillGeometry*)pNode;
				KernelBitmapRef *BmpRef = pBmpFill->GetBitmapRef();
				if (BmpRef != NULL)
					KernelBmp = BmpRef->GetBitmap();
			}
		}
*/
		pNode = pNode->FindNextDepthFirst(pSubtree);
	}

	return(KernelBmp);
}



/********************************************************************************************

>	virtual BOOL BitmapClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

				Note that when a bitmap is available on the clipabord, a palette will
				also be available - see the SeeAlso

	SeeAlso:	PaletteClipMap::HandleExport; DIBClipMap::HandleExport

********************************************************************************************/

HANDLE BitmapClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	ERROR3("Bitmap export is unsupported on the grounds that it won't work thanks to Windows");
	return(NULL);

/*
	// Scan the entire document tree, looking for a bitmap to export
	KernelBitmap *KernelBmp = FindBitmapToExport(Source);
	if (KernelBmp == NULL)		// Didn't find a bitmap
		return(NULL);

	OILBitmap *pOilBitmap = KernelBmp->ActualBitmap;
	ERROR3IF(pOilBitmap == NULL, "Unattached kernel bitmap found!");
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	
	// Convert the DIB into GDI BITMAP format
	HDC hDCScreen = GetDC(NULL);
	HBITMAP hBitmap = NULL;
	if (hDCScreen != NULL)
	{
//		HGDIOBJ OldPal = ::SelectObject(hDCScreen, CMainFrame::MainPalette.GetSafeHandle());
//		RealizePalette(hDCScreen);

		HPALETTE hPal = CreateBMPPalette(&Info->bmiHeader);
		HPALETTE hOldPal = NULL;
		
#if _DEBUG
		if (IsUserName("Jason"))
			ERROR3IF(hPal == NULL, "Couldn't create a palette for that image");
#endif

		if (hPal != NULL)
		{
			hOldPal = SelectPalette(hDCScreen, hPal, FALSE);
			RealizePalette(hDCScreen);
		}

		hBitmap = CreateDIBitmap(hDCScreen, &Info->bmiHeader, CBM_INIT, Bytes, Info, DIB_RGB_COLORS);
		SetClipboardData(CF_PALETTE, hPal);

//		if (OldPal != NULL)
//			::SelectObject(hDCScreen, OldPal);
		

		if (hPal != NULL)
		{
			SelectPalette(hDCScreen, hOldPal, FALSE);
			RealizePalette(hDCScreen);
		}

		ReleaseDC(NULL, hDCScreen);
	}

	// And return the bitmap handle, which is put on the clipboard for the CF_BITMAP data
	return((HANDLE) hBitmap);
 */
/*
	// Scan the entire document tree, looking for a bitmap to export
	KernelBitmap *KernelBmp = FindBitmapToExport(Source);
	if (KernelBmp == NULL)		// Didn't find a bitmap
		return(NULL);

	OILBitmap *pOilBitmap = KernelBmp->ActualBitmap;
	ERROR3IF(pOilBitmap == NULL, "Unattached kernel bitmap found!");
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	
	// Convert the DIB into GDI BITMAP format
//	HDC hDCScreen = GetDC(NULL);
	HBITMAP hBitmap = NULL;
//	if (hDCScreen != NULL)
	{
//		hBitmap = CreateDIBitmap(hDCScreen, &Info->bmiHeader, CBM_INIT, Bytes, Info, DIB_RGB_COLORS);
		hBitmap = CreateBitmap(Info->bmiHeader.biWidth, Info->bmiHeader.biHeight,
								1, Info->bmiHeader.biBitCount, NULL);//Bytes);

		// Copy the DIB bits into the bitmap
		SetDIBits(NULL, hBitmap, 0, Info->bmiHeader.biHeight, Bytes, Info, NULL);//DIB_RGB_COLORS);

		HPALETTE hPal = CreateBMPPalette(&Info->bmiHeader);
		SetClipboardData(CF_PALETTE, hPal);

//		ReleaseDC(NULL, hDCScreen);
	}

	// And return the bitmap handle, which is put on the clipboard for the CF_BITMAP data
	return((HANDLE) hBitmap);
*/
}









#if FALSE

// Palettes are only needed when exporting DDB Bitmaps, and we don't do this because they
// almost never work. Also, fabbily, some programs out there stupidly use the palette we
// supply with any DIB that we put on the clipboard (dang!) so it's generally a bad thing to
// even try exporting a bitmap with palette.

/********************************************************************************************

//>	PaletteClipMap::PaletteClipMap()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to export
				palette data from bitmaps to a windows clipboard in CF_PALETTE format.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	PaletteClipMap::CreateAndRegister

********************************************************************************************/
PaletteClipMap::PaletteClipMap()
				: ClipboardMapping(CLIPMAP_EXPORTONLY, NULL,
									InternalClipboardFormat(CLIPTYPE_BITMAP),
									CF_PALETTE, 70)
{
}



/********************************************************************************************

//>	static void PaletteClipMap::CreateAndRegister(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to export
				data to a windows clipboard in CF_PALETTE format

********************************************************************************************/

void PaletteClipMap::CreateAndRegister(void)
{
	PaletteClipMap *Mapping = new PaletteClipMap();
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************

//>	virtual BOOL PaletteClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE PaletteClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
ERROR3("Palette not exported\n");
return(NULL);

/*
	KernelBitmap *KernelBmp = FindBitmapToExport(Source);
	if (KernelBmp == NULL)		// Didn't find a bitmap
		return(NULL);

	OILBitmap *pOilBitmap = KernelBmp->ActualBitmap;
	ERROR3IF(pOilBitmap == NULL, "Unattached kernel bitmap found!");
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;

	// Generate a palette for the bitmap
	ERROR3IF(pWBitmap->BMInfo == NULL, "Incomplete winoil bitmap");
	HPALETTE hPal = CreateBMPPalette(&(pWBitmap->BMInfo->bmiHeader));
	if (hPal != NULL)
		return(hPal);

	// It failed (probably a 24 or 32 bpp BMP) so give them the mainframe's palette
	return(CMainFrame::MainPalette.GetSafeHandle());
*/
}
#endif











DIBClipMap::DIBClipMap()
{
	ERROR3("Please don't press that button again");
}


/********************************************************************************************

>	DIBClipMap::DIBClipMap(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - CF_DIB for straight import, CF_BITMAP for an alias

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	DIBClipMap::CreateAndRegister

********************************************************************************************/

DIBClipMap::DIBClipMap(ClipboardMappingType TheType, UINT32 ClaimType)
				: ClipboardMapping(TheType, NULL, InternalClipboardFormat(CLIPTYPE_BITMAP),
									CF_DIB, 80)
{
	if (ClaimType != 0)
		ExternalDataType = ClaimType;
}



/********************************************************************************************

>	static void DIBClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - CF_DIB for straight import, CF_BITMAP for an alias

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/

void DIBClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	DIBClipMap *Mapping = new DIBClipMap(TheType, ClaimType);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************

>	virtual BOOL DIBClipMap::HandleImport(SelOperation *Caller,
												HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.

	Returns:	TRUE for success

	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

********************************************************************************************/

BOOL DIBClipMap::HandleImport(SelOperation *Caller,
								HANDLE ClipboardData, InternalClipboard *Dest)
{
	LPBYTE Bits = NULL;
	LPBITMAPINFO BmpInfo = NULL;
	WinBitmap *WinBmp = NULL;
	KernelBitmap *KernelBmp = NULL;
	CCMemFile MemFile;

	BITMAPINFO *buff = (BITMAPINFO *) GlobalLock(ClipboardData);
	if (buff != NULL)
	{
		String_64 ProgressMsg(_R(IDS_CLIPBOARD_IMPORTDIB));
		MemFile.open(buff, GlobalSize(ClipboardData), CCMemRead);
		if (!DIBUtil::ReadFromFile(&MemFile, &BmpInfo, &Bits, FALSE, &ProgressMsg))
			goto Abort;

		MemFile.close();
		GlobalUnlock(ClipboardData);

		WinBmp = new WinBitmap(BmpInfo, Bits);
		if (WinBmp == NULL)
			goto Abort;

		KernelBmp = new KernelBitmap(WinBmp);
		if (KernelBmp == NULL)
			goto Abort;

		// And now, lob a bitmap node for it into the clipboard
		NodeBitmap *pNode = new NodeBitmap();
		if (pNode == NULL || pNode->GetBitmapRef() == NULL || !(pNode->SetUpPath(12,12)))
			goto Abort;

		// Attach this bitmap reference to the new bitmap, and put in the clipboard document
		pNode->GetBitmapRef()->Attach(KernelBmp, InternalClipboard::Instance());
		if (pNode->GetBitmap() != KernelBmp)
		{
			// It didn't use the bitmap we gave it, so we can delete it
			delete KernelBmp;
		}

		// And set up the node's bounds appropriately
		BitmapInfo Info;
		pNode->GetBitmap()->ActualBitmap->GetInfo(&Info);

		DocRect BoundsRect;
		BoundsRect.lo.x = 0;
		BoundsRect.lo.y = 0;
		BoundsRect.hi.x = Info.RecommendedWidth/2;
		BoundsRect.hi.y = Info.RecommendedHeight/2;

		pNode->CreateShape(BoundsRect);

		// Apply the default bitmap attributes
		pNode->ApplyDefaultBitmapAttrs(NULL);

		pNode->AttachNode(InternalClipboard::GetInsertionLayer(), LASTCHILD, FALSE);
		return(TRUE);
	}

	return(FALSE);

Abort:
//	ERROR2RAW("Bitmap import failed, probably due to lack of memory");
//	InformError();

	GlobalUnlock(ClipboardData);

	if (MemFile.isOpen())
		MemFile.close();

	if (KernelBmp != NULL)
		delete KernelBmp;

	if (WinBmp != NULL)
		delete WinBmp;
/*	else if (BmpInfo != NULL || Bits != NULL)
		FreeDIB(BmpInfo, Bits);
*/
	InternalClipboard::Clear();	// We failed, so ensure the doc is wiped
	InformError();
	return(FALSE);
}


		
/********************************************************************************************

>	virtual BOOL DIBClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE DIBClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	// Scan the entire document tree, looking for a bitmap to export
	KernelBitmap *KernelBmp = FindBitmapToExport(InternalClipboard::Instance());
	if (KernelBmp == NULL)		// Didn't find a bitmap
		return(NULL);

	OILBitmap *pOilBitmap = KernelBmp->ActualBitmap;
	ERROR3IF(pOilBitmap == NULL, "Unattached kernel bitmap found!");
	WinBitmap* pWBitmap = (WinBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;

	ERROR3IF(Info->bmiHeader.biSizeImage == 0, "Bitmap data size is zero - I'm about to screw up the export");

	// Neville 14/10/97
	// Don't just use the number of colours in the colour depth. If we have set biClrUsed to
	// something between 0 and max number of colours for that colour depth then that is
	// the size of the palette that is expected. We set biClrUsed when exporting an
	// optimised palette with a user specified number of colours.
	// If we don't make sure that this matches the the destination program will skew the
	// bitmap. 
	UINT32 PalSize = DIBUtil::CalcPaletteSize(pWBitmap->GetBPP(), Info->bmiHeader.biCompression == BI_BITFIELDS,
		Info->bmiHeader.biClrUsed);
	DWORD HeaderSize = Info->bmiHeader.biSize +	PalSize;
	DWORD BitmapSize = Info->bmiHeader.biSizeImage;

#if (_OLE_VER >= 0x200)

	// Get the memory block's handle.
	BOOL fDidAlloc = FALSE;
	HANDLE hGlobalMem = m_hMem;
	if (!hGlobalMem)
	{
		// We don't have any memory, so try to allocate some and remember that we did so.
		fDidAlloc = TRUE;
		hGlobalMem = m_hMem = GlobalAlloc(GHND, HeaderSize + BitmapSize);
		if (hGlobalMem == NULL)
		{
			ERROR3("Couldn't get enough memory to export bitmap");
			return(NULL);
		}
	}

#else

	// Allocate a block of global memory to store the DIB
	HANDLE hGlobalMem = GlobalAlloc(GHND, HeaderSize + BitmapSize);

	if (hGlobalMem == NULL)
	{
		ERROR3("Couldn't get enough memory to export bitmap");
		return(NULL);
	}

#endif

	char* buff = (char*) GlobalLock(hGlobalMem);
	if (buff == NULL)
	{
		ERROR3("Couldn't get clipboard memory");
#if (_OLE_VER >= 0x200)
		if (fDidAlloc) GlobalFree(hGlobalMem);
#else
		GlobalFree(hGlobalMem);
#endif
		return(NULL);
	}

	// Copy the DIB onto the clipboard (copy the header and body separately as they
	// may not lie in contiguous memory)
	memcpy(buff, Info, HeaderSize);
	memcpy(buff + HeaderSize, Bytes, BitmapSize);

	// We must unlock the block before giving it to the clipboard
	GlobalUnlock(hGlobalMem);
	return(hGlobalMem);
}








#if FALSE

// Experimental code to attempt to load EPSF files copied from Quark XPress
// It doesn't work very well, and it's highly specific, so not worth using.

/********************************************************************************************

>	QuarkPictureClipMap::QuarkPictureClipMap()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - CF_DIB for straight import, CF_BITMAP for an alias

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	QuarkPictureClipMap::CreateAndRegister

********************************************************************************************/

QuarkPictureClipMap::QuarkPictureClipMap()
{
	ERROR3("Nope. Wrong constructor dude");
}

QuarkPictureClipMap::QuarkPictureClipMap(UINT32 MyFormat)
				: ClipboardMapping(CLIPMAP_IMPORTONLY, NULL,
									InternalClipboardFormat(CLIPTYPE_VECTOR),
									MyFormat, 110)
{
}



/********************************************************************************************

>	static void QuarkPictureClipMap::CreateAndRegister(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		This also registers a custom clipboard format with Windows

********************************************************************************************/

void QuarkPictureClipMap::CreateAndRegister(void)
{
#if _DEBUG
	// Register or determine the Format ID for the Quack XPress custom clipboard format
	UINT32 MyFormat = RegisterClipboardFormat("QuarkXPress Picture Data");

	if (MyFormat == 0)
	{
		ERROR3("Couldn't register QuarkXPress clipboard handler");
		return;
	}

	QuarkPictureClipMap *Mapping = new QuarkPictureClipMap(MyFormat);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
#endif
}



typedef struct
{
	BYTE dunno1[2];			// Yep, I've really sussed their format now! ;-)
	char ID[4];
	BYTE dunno2[40];
	char Type[4];
	BYTE dunno3[28];
	char Pathname[260];		// Zero terminated, with a bit of crap on the end
} QUACKDATA;


/********************************************************************************************

>	virtual BOOL QuarkPictureClipMap::HandleImport(SelOperation *Caller,
												HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/95

	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.

	Returns:	TRUE for success

	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.

				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.

********************************************************************************************/

BOOL QuarkPictureClipMap::HandleImport(SelOperation *Caller,
								HANDLE ClipboardData, InternalClipboard *Dest)
{
#if _DEBUG
	QUACKDATA *buff = (QUACKDATA *) GlobalLock(ClipboardData);

	if (buff != NULL)
	{
		if (IsUserName("Jason"))
		{
			FILE *fp = _tfopen("c:\\jason.tmp", "wb");
			size_t Size = (size_t)GlobalSize(ClipboardData);
			fwrite(buff, 1, Size, fp);
			fclose(fp);
		}

		if (camStrncmp(buff->ID, "XPR3", 4))
		{
			ERROR3("Not a Quark XPress 3 clipboard thingo");
			goto Abort;
		}

		if (camStrncmp(buff->Type, "EPSF", 4))
		{
			ERROR3("Doesn't appear to be EPSF");
			goto Abort;
		}

		// Attempt to load the EPS data from the given pathname - BODGE! ******* !!!!!!!!!!!
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile(buff->Pathname);

		GlobalUnlock(ClipboardData);
		return(TRUE);
	}
#endif

	return(FALSE);

Abort:
	GlobalUnlock(ClipboardData);
	return(FALSE);
}

#endif


#ifdef _DEBUG
/********************************************************************************************
>	RTFClipMap::RTFClipMap(ClipboardMappingType TheType, Filter *TheFilter,
										InternalClipboardFormat *TheInternalDataType,
										UINT32 TheExternalDataType,
										UINT32 ThePriority)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		TheType - 1 (import), 2 (export), or 3 (import and export)
				TheFilter - The filter which can apply this conversion
				TheInternalDatatType - An object defining the internal data type
				(see cliptype.h)
				TheExternalDataType - A Windows CF_ constant defining the external
				clipboard data type which will be imported/exported.
				ThePriority - An integer indicating the priority of this mapping.
				The highest available priority mapping will be used in order to
				retain as much information in the data as possible. See
				docs\howtouse\ExtClip.doc for details of the existing mappings.
	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.
	Notes:		DON'T call the constructor - call CreateAndRegister
	SeeAlso:	RTFClipMap::CreateAndRegister
********************************************************************************************/

RTFClipMap::RTFClipMap(ClipboardMappingType TheType, Filter *TheFilter,
									InternalClipboardFormat &TheInternalDataType,
									UINT32 TheExternalDataType,
									UINT32 ThePriority)
				: ClipboardMapping(TheType, TheFilter, TheInternalDataType,
									TheExternalDataType, ThePriority)
{
}



/********************************************************************************************
>	static void RTFClipMap::CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
												InternalClipboardFormat &TheInternalDataType,
												UINT32 TheExternalDataType,
												UINT32 ThePriority)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		ImportOrExport - TRUE == Import, FALSE == Export

				TheFilter - The filter which can apply this conversion

				TheInternalDatatType - An object defining the internal data type
				(see cliptype.h)

				TheExternalDataType - A Windows CF_ constant defining the external
				clipboard data type which will be imported/exported.

				ThePriority - An integer indicating the priority of this mapping.
				The highest available priority mapping will be used in order to
				retain as much information in the data as possible. See
				docs\howtouse\ExtClip.doc for details of the existing mappings.

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.
********************************************************************************************/
void RTFClipMap::CreateAndRegister(ClipboardMappingType TheType, Filter *TheFilter,
										InternalClipboardFormat &TheInternalDataType,
										UINT32 TheExternalDataType,
										UINT32 ThePriority)
{
	RTFClipMap *Mapping = new RTFClipMap(TheType, TheFilter,
													TheInternalDataType,
													TheExternalDataType,
													ThePriority);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}



/********************************************************************************************
>	virtual BOOL RTFClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.
	Returns:	TRUE for success
	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.
********************************************************************************************/
BOOL RTFClipMap::HandleImport(SelOperation *Caller,	HANDLE ClipboardData, InternalClipboard *Dest)
{
	CCMemFile MemFile;
	BOOL ok = TRUE;

	// Get a lock on the clipboard data
	BYTE* buff = (BYTE*) GlobalLock(ClipboardData);
	ERROR1IF(buff == NULL, FALSE, "_R(IDE_NOMORE_MEMORY)");

	// Provide a current view for the filter to chomp on; We stack the current
	// view so that we do not corrupt it.
	View *OldCurrentView = NULL;
	ClipboardView ImportView;
	if (ok)
		OldCurrentView = View::GetCurrent();
	if (ok)
		ok = ImportView.Init();
	if (ok)
		ImportView.SetCurrent();

	// Open a memfile onto that data
	if (ok)
		ok = MemFile.open(buff, GlobalSize(ClipboardData)-1, CCMemRead);

	// Attempt to import the file
	if (ok)
		ok = pFilter->DoImport(Caller, &MemFile, InternalClipboard::Instance());

	// Clean up behind messy importers
	if (MemFile.isOpen())
		MemFile.close();

	if (OldCurrentView != NULL)
		OldCurrentView->SetCurrent();

	GlobalUnlock(ClipboardData);

	return ok;
}


		
/********************************************************************************************
>	virtual BOOL RTFClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported
	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.
	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard. Usually this just involves calling Filter::DoExport
				for the parent filter, and then returning the handle to the global memory
				block to be placed onto the external clipboard.
	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.
********************************************************************************************/
HANDLE RTFClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	return NULL;
}
#endif



/********************************************************************************************

>	CMXClipMap::CMXClipMap(ClipboardMappingType TheType, UINT32 ClaimType = 0)
	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/09/96

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the
				windows clipboard - that is, create 3 of these mappings, specifying
				CF_TEXT, CF_UNICODETEXT, and CF_OEMTEXT, and they will all ask the
				clipboard for UNICODE text when they actually go to import. This allows
				us to detect the 3 implicitly-converted clipboard formats, and map them
				all to UNICODE, i.e. use the UNICODE mapping for all 3 available formats.
				If ClaimType == 0, UNICODE is assumed

	Purpose:	Constructs a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

	Notes:		DON'T call the constructor - call CreateAndRegister

	SeeAlso:	BodgeTextClipMap::CreateAndRegister

********************************************************************************************/

CMXClipMap::CMXClipMap(ClipboardMappingType TheType, UINT32 ClaimType, UINT32 nPriority)
				: ClipboardMapping(TheType, NULL, InternalClipboardFormat(CLIPTYPE_VECTOR),
					ClaimType, nPriority)
{
	if (ClaimType != 0)
		ExternalDataType = ClaimType;
}

CMX16ClipMap::CMX16ClipMap()
{
	ERROR3("Please don't press that button again");
}

CMX32ClipMap::CMX32ClipMap()
{
	ERROR3("Please don't press that button again");
}

/********************************************************************************************

>	static void CMXClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType = 0)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/09/96

	Inputs:		TheType - indicates import / export / import & export

				ClaimType - specifies the text type that this mapping will claim from the

	Purpose:	Constructs and registers a clipboard mapping with the ExternalClipboard
				manager. This mapping info describes a filter which is able to import
				data from or export data to a windows clipboard in some way.

********************************************************************************************/

void CMX16ClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	CMXClipMap *Mapping = new CMX16ClipMap(TheType, ClaimType, 120);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}

void CMX32ClipMap::CreateAndRegister(ClipboardMappingType TheType, UINT32 ClaimType)
{
	// CMX32 should have a higher priority than CMX16!
	CMXClipMap *Mapping = new CMX32ClipMap(TheType, ClaimType, 121);
	if (Mapping == NULL)
		InformError();
	else
		ExternalClipboard::RegisterDataType(Mapping);
}


/********************************************************************************************

>	virtual BOOL CMXClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData,
												InternalClipboard *Dest)
	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/09/96
	Inputs:		Caller - The operation within which this method is being called
				ClipboardData - The result of calling GetClipboardData for your datatype
				Dest - The InternalClipboard (document) to import the data into.
	Returns:	TRUE for success
	Purpose:	Calls the parent filter as appropriate to import the given data from
				the external clipboard.
				This bodge mapping for text scans the docuemnt tree for text chars and
				strips them out by hand. Styles and suchlike are lost. It's a bodge.
********************************************************************************************/

BOOL CMXClipMap::HandleImport(SelOperation *Caller, HANDLE ClipboardData, InternalClipboard *Dest)
{
	BOOL ok = FALSE;

	// Get a scratch file - if TMP isn't set, this will try for c:\temp.
	// The filename will have XS as a prefix
	char *tempname = GetTempFileName();
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(FALSE);
	}

	void *pMem = GlobalLock(ClipboardData);
	ok = (pMem != NULL);

	BOOL Layers = Filter::ImportWithLayers;
	Filter::ImportWithLayers = FALSE;
	pFilter = new CMXImportFilter;
	ok = (pFilter != NULL);

	CCDiskFile File;
	if(ok)
	{
		// get a file and write to it
		PathName pathname(tempname);
		if(!File.open(pathname, (ios::out | ios::binary)))
			ok = FALSE;		// error!
	}

	if(ok)
	{
		File.write(pMem, GlobalSize(ClipboardData));

		File.close();
	}

	GlobalUnlock(ClipboardData);

	if(ok)
	{
		ok = ImportFromTempFile(tempname, Caller, InternalClipboard::Instance());
	}

	RemoveTempFile();

	if(pFilter != NULL)
	{
		delete pFilter;
		pFilter = NULL;
	}

	Filter::ImportWithLayers = Layers;

	return(ok);


/*	char *buff=(char*)GlobalLock(ClipboardData);
	if (buff != NULL && buff[0] != 0)
	{
		// get a filter to play with
		CMXImportFilter *pFilter = new CMXImportFilter;
		BOOL ok = (pFilter != NULL);

		// Provide a current view for the filter to chomp on; We stack the current
		// view so that we do not corrupt it.
		View *OldCurrentView = View::GetCurrent();
		ClipboardView ImportView;
		if(!ImportView.Init())
		{
			ok = FALSE;
		}
		else
		{
			ImportView.SetCurrent();
		}

		// position arbitarily on clipboard - when pasted the clip contents are centered anyway
		if(ok)
		{
			// make a memfile based on this thingy
			CCMemFile File(buff, GlobalSize(ClipboardData), CCMemRead, TRUE, TRUE);

			// run it through the filter
			ok = pFilter->DoImport(Caller, &File, Dest);

			if(File.isOpen())
				File.close();

			if (OldCurrentView != NULL)
				OldCurrentView->SetCurrent();
		}

		// Unlock the clipboard data, as we won't need it again
		GlobalUnlock(ClipboardData);

		if(ok)
			return TRUE;

		// We failed (no text story), so report and ensure the clipboard is 'clean'
		InformError();
		Dest->ClearClipboard();
	}

	// We failed
	return FALSE;*/
}




/********************************************************************************************

>	virtual BOOL CMXClipMap::HandleExport(Operation *Caller,
												InternalClipboard *Source)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/09/96

	Inputs:		Caller - the operation within which this method is being called
				Source - the internal clipboard (document) to be exported

	Returns:	NULL (if it failed), or a windows handle of the data to be placed on
				the clipboard.

	Purpose:	Invokes this mapping for exporting
				This takes the document tree of Source, and exports it to the external
				(windows) clipboard. Usually this just involves calling Filter::DoExport
				for the parent filter, and then returning the handle to the global memory
				block to be placed onto the external clipboard.

	Notes:		The returned handle should be the thing you'd pass to SetClipboardData
				if you were dealing with it directly. You must adhere to all the Windows
				rules for this - i.e. a global data block, unlocked, etc etc.

********************************************************************************************/

HANDLE CMXClipMap::HandleExport(Operation *Caller, InternalClipboard *Source)
{
	char *tempname = GetTempFileName();	//_ttempnam("C:\temp", "XS");
	if (tempname == NULL)
	{
		ERROR3("Couldn't get a temp filename");
		return(FALSE);
	}


	// make a new disk file of this name
	CCDiskFile File;
	PathName pathname(tempname);
	if(!File.open(pathname, (ios::in | ios::out | ios::binary)))
		return 0;		// error!
	
	// get a filter to export the thingy to
	CMXFilter* pFilter = CreateExportFilter();
	BOOL ok = (pFilter != NULL);

	if(ok)
	{
		// tell the filter not to do non clipboardy thing
		pFilter->SetIsDoingClipboardExport(TRUE);

		/////////////////////////////////////////////////////////////////////////////////////
		//
		// JustinF says: this is a ultra-late bodge so that we can render OLE presentation
		// data in CMX format.  The only app that requires this is CorelDRAW.  We are
		// tricking the CMX exporter to work with any kernel Document rather than only
		// the internal clipboard document.
		//

// WEBSTER - markn 12/2/97
#if (_OLE_VER >= 0x200)
		ok = pFilter->DoExport(Caller, &File, &pathname, m_pDoc, TRUE);
#else
		// export to the file
		ok = pFilter->DoExport(Caller, &File, &pathname, InternalClipboard::Instance());
#endif

		// reset clipboardy flag
		pFilter->SetIsDoingClipboardExport(FALSE);
	}

	delete pFilter;

	void* pMem;
	INT32 FileSize;
	HANDLE hGlobalMem;

	if (ok)
	{
		// find out how big the file is
		FileSize = File.Size();

	#if (_OLE_VER >= 0x200)
	
		// Is memory already allocated?
		if ((hGlobalMem = m_hMem) != 0)
		{
			// Is the file too big?
			if (UINT32(FileSize) > m_cbMemSize) return 0;
		}
		else
		{
			// No.  Allocate some.
			hGlobalMem = m_hMem = GlobalAlloc(GHND, FileSize);
		}

	#else
		
		// copy the file into the global block
		pMem = GlobalLock(hGlobalMem);
	
	#endif

		// Lock the block.
		pMem = GlobalLock(hGlobalMem);
		ok = (pMem != 0);
	}

	
	if (ok)
	{
		// seek to beginning of file
		File.seekIn(0);

		// load data..
		File.read(pMem, FileSize);
	}

	// close the file
	if(File.isOpen())
		File.close();

	// get rid of the temp file
	RemoveTempFile();

	// We must unlock the block before giving it to the clipboard
	GlobalUnlock(hGlobalMem);

	return hGlobalMem;
}
