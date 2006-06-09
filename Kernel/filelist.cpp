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
// The operation to handle the Recent File List

/*
*/

#include "camtypes.h"
#include "filelist.h"

#include "camelot.h"
#include "camdoc.h"
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bubbleid.h"

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNCREATE(FileListOp, Operation)

// This will get Camelot to display the filename and linenumber of any memory allocations
#define new CAM_DEBUG_NEW




// Here is a macro to make the ::Init function a little neater in this class.
#define REG_FILELISTOP(a) RegisterOpDescriptor(0, _R(IDS_FILE_MRU_FILE), CC_RUNTIME_CLASS(FileListOp), (a),	FileListOp::GetState, 0, _R(IDBBL_FILEOP), 0)




/*********************************************************************************************

	Preference:	MaxFileListSize
	Section:	Recent File List
	Range:		1-9
	Purpose:	Describes how many files will be displayed in the Recent file list.

**********************************************************************************************/ 

INT32 FileListOp::MaxFileListSize = 4;




/********************************************************************************************

>	FileListOp::FileListOp()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	Constructor for the File List Operation.

********************************************************************************************/

FileListOp::FileListOp()
{
}


/********************************************************************************************

>	static BOOL FileListOp::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Returns:	TRUE if it inited OK, FALSE if not.
	Purpose:	Initialises the FileList Operation. It tries to create several OpDescriptors
				that will be associated with each of the menu items on the recent file list.
				They will all invoke this operation and the operation will try to work out
				which one invoked it.

********************************************************************************************/

BOOL FileListOp::Init()
{
	// Register some preferences
	if (Camelot.DeclareSection(TEXT("Recent File List"), 3))
	{
		Camelot.DeclarePref(NULL, TEXT("FileListSize"), &MaxFileListSize, 1, 9);
	}

	// Try and create an OpDescriptor. This uses a macro that is define at the top of this file.
	BOOL Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE1);

	// See if it worked
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// And do the same for all the others
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE2);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 3
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE3);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 4
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE4);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 5
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE5);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 6
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE6);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 7
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE7);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 8
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE8);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// File 9
	Worked = REG_FILELISTOP(OPTOKEN_MRU_FILE9);
	ERROR1IF(!Worked, FALSE, _R(IDE_NOMORE_MEMORY));

	// All worked
	return TRUE;
}



/********************************************************************************************

>	OpState FileListOp::GetState(String_256*, OpDescriptor*)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Purpose:	Descides if this operation shold be available or not
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState FileListOp::GetState(String_256* Title, OpDescriptor* pOp)
{
	OpState State;

	// Get the first 2 chars of the opdescriptors description as this holds the
	// file number of the recent file.
	TCHAR FileNum[3];
	camStrncpy(FileNum, pOp->Token, 2);
	FileNum[2] = 0;

	// make sure that it contains digits.
	if (isdigit(FileNum[0]) && isdigit(FileNum[1]))
	{
		// Find out which number was chosen
		TCHAR *pszMark;
		INT32 Index = camStrtol( FileNum, &pszMark, 10 );
		
		// Only the first 'MaxFileListSize' items will appear
		if (Index > MaxFileListSize)
			State.RemoveFromMenu = TRUE;
		else
		{
			// Change the menu string to whatever it needs to be
			CCamApp* pApp = (CCamApp*)&AfxGetApp();
			if (!pApp->GetRecentFileText(Index-1, Title))
			{
				State.Greyed = TRUE;

				// if this is the first item in the list, then it wants to stay in the menu, only greyed
				if (Index!=1)
					State.RemoveFromMenu = TRUE;
			}
		}
	}

	return State;
}



/********************************************************************************************

>	void FileListOp::Do(OpDescriptor* pOpDesc)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/95
	Inputs:		pOpDesc - The opdescriptor that invoked this operation.
	Purpose:	Attempts to load a file that was in the Recent file list.

********************************************************************************************/

void FileListOp::Do(OpDescriptor* pOpDesc)
{
	// Get the first 2 chars of the opdescriptors description as this holds the
	// file number of the recent file.
	TCHAR FileNum[3];
	BOOL WorkedOK = FALSE;
	camStrncpy(FileNum, pOpDesc->Token, 2);
	FileNum[2] = 0;

	TRACEUSER( "jlh92", _T("Op = %s\n"), FileNum );

	// make sure that it contains digits.
	if (isdigit(FileNum[0]) && isdigit(FileNum[1]))
	{
		// Find out which number was chosen
		TCHAR *pszMark;
		INT32 RecentFileNum = camStrtol( FileNum, &pszMark, 10 );

		TRACEUSER( "jlh92", _T("Recent = %d\n"), RecentFileNum );

		// Ask the app to load the file specified (it will look up the file name etc)
		WorkedOK = AfxGetApp().OnRecentFile(RecentFileNum-1);
		CCamDoc::EnableRemoveUntouchedDocs();
	}

	// See if it worked
	if (!WorkedOK)
	{
		// At this point Current is NULL, so we should put it back
		// we have to do this as the creation of a new document went wrong, leaving the pointers upset
		Document* pDoc = Document::GetSelected();
		if (pDoc==NULL)
			Document::SetNoCurrent();
		else
			pDoc->SetCurrent();

		// Same for the view
		DocView* pView = DocView::GetSelected();
		if (pView==NULL)
			DocView::SetNoCurrent();
		else
			pView->SetCurrent();

		// and fail
		FailAndExecute();
	}

	// End the operations.
	End();
}


