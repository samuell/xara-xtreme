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
// The "Import from URL..." menu item

#include "camtypes.h"

#include "pathname.h"
//#include "barsdlgs.h"		//For _R(IDD_BARCONTROLSTORE)

#include "urlimp.h"
#include "fileutil.h"

//#include "resimmap.h"		//For _R(IDS_HTMLIMPORT_FILEDOWNLOAD)
#include "urldlg.h"			//For URLImportDlg

#include "gendwnd.h"		//For the download operation
//#include "webster.h"

#include "xshelpid.h"		//For the help ID

CC_IMPLEMENT_DYNCREATE(OpURLImport, OpMenuImport)

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL OpURLImport::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares the OpDescriptor
	Errors:		-

********************************************************************************************/

BOOL OpURLImport::Init()
{
	return RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_URLIMPORT),					// String resource ID
									CC_RUNTIME_CLASS(OpURLImport), 	// Runtime class
									OPTOKEN_URLIMPORT,			// Token string
									GetState,					// GetState function
									_R(IDH_Command_Import_from_Web),							// help ID GTODO: Is this needed?
									_R(IDBBL_URLIMPORT),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_FILEURLIMPORT),				// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Receive system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,		// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
 								);
	
}

/********************************************************************************************

>	void OpURLImport::Do(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	Imports a URL into the current document
	Errors:		-

********************************************************************************************/

void OpURLImport::Do(OpDescriptor*)
{
	//First display our Import from URL dialog and get a filename from it
	WebAddress urlToGet;
	
	BOOL ok=URLImportDlg::OpenAndGetURL(&urlToGet);

	//And if the user clicked OK
	if (ok)
	{

		//So, is the URL we've got back a local filename?
		if (urlToGet.IsLocalFile())
		{
			//Yes. So get it as a pathname
			PathName pthToGet=urlToGet.GetPathName();

			//And first get a pointer to the All filter
			Filter* pAllFilter=GetAllFilter();

			//And if we found it
			if (pAllFilter)
			{
				//Then import the pathname
				String_256 strToGet=pthToGet.GetPath();

				DoImport(pthToGet, pAllFilter, &strToGet);
			}

		}
		else
		{
			//No, it's a URL.

			// Added by Craig Hamilton 30/8/00.
			// Add some error checking to find whether the user has entered a valid URL

			PathName pthToGet = urlToGet.GetPathName();
			String_256 str256 = pthToGet.GetLocation(FALSE);
			CString s = str256;

			if((s == ""))
			{
				// ERROR
				MessageBox(NULL,"There was a problem downloading from the specified URL. Make sure the URL is spelled correctly.","Error from Xara X",MB_OK | MB_ICONWARNING);
				return;
			}
			// End added.

			//So download it
			ImportURL(urlToGet);
		}
	}





}

/********************************************************************************************

>	OpURLImport::OpURLImport()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	Default constructor

********************************************************************************************/

OpURLImport::OpURLImport()
{
	OpFlags.HasOwnTimeIndicator = TRUE; // The OpURLImport op has its own time indicator
}

/********************************************************************************************

>	OpState OpURLImport::GetState(String_256*, OpDescriptor*)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	This item is always available, so long as a document is visible.

********************************************************************************************/

OpState OpURLImport::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	return OpSt;
}

/*****************************************************************

  Toolkit functions

  ****************************************************************/

 /********************************************************************************************

>	BOOL OpURLImport::ImportURL(WebAddress urlToGet)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Inputs:		urlToGet	The URL to import
	Purpose:	Imports the URL given

********************************************************************************************/

BOOL OpURLImport::ImportURL(WebAddress urlToGet)
{
	//First, set up some variables. We must do this now because I'm 
	//going to use a goto in a moment
	GenericDownloadParam* Param;
	OpDescriptor* pOpDesc;
	PathName pthTemp;

	//We want to import urlToGet using the best available filter

	//To get the best available filter, we simply download to the "All" filter
	//So get a pointer to the All filter
	Filter* pAllFilter=GetAllFilter();

	//Check we found it
	if (pAllFilter==NULL)
		goto ReturnError;

	//Now, get ready to download the file. This is rather complex.

	//First create a new set of download parameters
	//This object will be deleted when the download finishes
	Param = new GenericDownloadParam;
	
	//Now find the generic download OpDescriptor
	pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_OPGENERICDOWNLOAD);

	//And check we found it
	if (pOpDesc==NULL)
		goto ReturnError;
  
	//Now, create a temporary file to download to
	pthTemp=FileUtil::GetTemporaryPathName();

	//And now fill in our parameters
	Param->type = TYPE_HTML;
	Param->priority = AsynchDownload::PRIORITY_NORMAL;
	Param->strDescription = String_256(_R(IDS_HTMLIMPORT_FILEDOWNLOAD));
	Param->file = pthTemp.GetPath();

	Param->strURL = urlToGet.GetWebAddress();
				
	Param->Output = NULL;

	Param->m_pFilter = pAllFilter;
	Param->m_Op = this;						
	Param->pDoc = Document::GetCurrent();

	//And, finally, start the download going, if we've got a URL to download
	if (!Param->strURL.IsEmpty())
		pOpDesc->Invoke((OpParam*) Param);
	
	//And return TRUE
	return TRUE;

ReturnError:
	if (Param)
		delete Param;

	return FALSE;
}

 /********************************************************************************************

>	Filter* OpURLImport::GetAllFilter()

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/97
	Inputs:		-
	Purpose:	Gets a pointer to the All filter.
    
********************************************************************************************/

Filter* OpURLImport::GetAllFilter()
{
	//So get a pointer to the All filter
	return Filter::FindFilterFromID(FILTERID_GENERIC);
		
}


