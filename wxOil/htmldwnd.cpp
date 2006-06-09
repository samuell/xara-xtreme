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
#include "htmldwnd.h"
#include "htmlfltr.h"
//#include "webster.h"
//#include "docmsgs.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "resimmap.h"	//For _R(IDS_HTML_DOWNLOADFAILED)


DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(OpBitmapDownload, DownloadOp) 

#define new CAM_DEBUG_NEW


///////////////////////////////////////////////////////////////////////////////
/////////////////				Bitmap download			///////////////////////
///////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	BitmapDownloadParam::BitmapDownloadParam()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/97
	Inputs:		
	Returns:	
	Purpose:	Default constructor. Do nothing
	Errors:		

********************************************************************************************/

BitmapDownloadParam::BitmapDownloadParam()
{
}


/********************************************************************************************

>	BitmapDownloadParam::~BitmapDownloadParam()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/97
	Inputs:		
	Returns:	
	Purpose:	Destructor. Do nothing
	Errors:		

********************************************************************************************/

BitmapDownloadParam::~BitmapDownloadParam()
{
}

/********************************************************************************************

>	BOOL BOOL OpBitmapDownload::Init()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		
	Returns:	TRUE if the Init worked ok
	Purpose:	
	Errors:		

********************************************************************************************/

BOOL OpBitmapDownload::Init()
{
	return RegisterOpDescriptor(
		0,									// Tool ID
 		_R(IDS_OPBITMAPDOWNLOAD),				// String resource ID
		CC_RUNTIME_CLASS(OpBitmapDownload),	// Runtime class
 		OPTOKEN_OPBITMAPDOWNLOAD,			// Token string
 		OpBitmapDownload::GetState,			// GetState function
		0,									// Help ID
		0,									// Bubble ID
		0,									// Resource ID
		0									// Control ID
	);
}


/********************************************************************************************

>	OpState OpBitmapDownload::GetState(String_256*, OpDescriptor*)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		
	Returns:	
	Purpose:	
	Errors:		

********************************************************************************************/

OpState OpBitmapDownload::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;
	return(OpSt);
}


/********************************************************************************************

>	BOOL OpBitmapDownload::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		pChangingDoc -
				State -
	Returns:	
	Purpose:	End (abort) the operation when for instance the current document has been destroyed
				while downloading.
	Errors:		

********************************************************************************************/

BOOL OpBitmapDownload::OnDocChangingMsg(Document* pChangingDoc, DocChangingMsg::DocState State)
{
	BitmapDownloadParam* pBMPParam = (BitmapDownloadParam*) pParam;

	if (pParam != NULL)
	{
		if (State == DocChangingMsg::ABOUTTODIE && pChangingDoc == pBMPParam->m_DestDoc)
		{
			FailAndExecute(); 
			End();
		}
	}

	return TRUE;
}

/********************************************************************************************

>	void OpBitmapDownload::OnDownloadSuccess()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/04/97
	Inputs:		
	Returns:	
	Purpose:	Start the bitmap import
	Errors:		

********************************************************************************************/

void OpBitmapDownload::OnDownloadSuccess()
{
	// get a pointer to the OpParam so that I can retrieve some useful information
	BitmapDownloadParam* pBMPParam = (BitmapDownloadParam*) pParam;

	String_256 BitmapFile = (pBMPParam->file).GetFileName();
	if (IsUserName("Olivier"))
		TRACE1("OpBitmapDownload::OnDownloadSuccess(), file = %s\n", (TCHAR*)BitmapFile);

	INT32 TagId = pBMPParam->m_TagId;
	//SelOperation *Op = pBMPParam->m_Op;
	Document *DestDoc = pBMPParam->m_DestDoc;
	PathName Path = pBMPParam->file;
	INT32 FileNum = pBMPParam->m_FileNum;
	INT32 FileNumber = pBMPParam->m_FileNumber;
	DocCoord PosTranslate = pBMPParam->m_PosTranslate;
	BOOL IsCenter = pBMPParam->m_IsCenter;
	String_256 NewName = pBMPParam->m_FileName;

	// set the current document
	DestDoc->SetCurrent();

	// create a new undoable operation 
	// we can't import a bitmap without having a undoable op pointer
	SelOperation *Op = new SelOperation();

	// Tell the filter we're importing an HTML page
	HTMLFilter::ImportingHTML=TRUE;

	//Import the bitmap
	HTMLFilter::DoImportBitmap(TagId, Op, DestDoc, Path, FileNum, FileNumber, PosTranslate, IsCenter, &NewName);

	//And tell the filter we're no longer importing an HTML page
	HTMLFilter::ImportingHTML=FALSE;
	
	// End the op
	Op->End();

	// remove the temporary bitmap file
	remove((TCHAR*) (String_256) Path.GetPath());

}

/******************************************************************************************
>	virtual void DownloadOp::OnDownloadFail()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/01/97
	Inputs:		-
	Outputs:	- 
						
	Returns:	- 
	Purpose:	This function is called if the file download fails.

				It puts up a message that says "Webster could not download from the URL
				you typed"

******************************************************************************************/

void OpBitmapDownload::OnDownloadFail()
{
	//First get a pointer to the parameters
	BitmapDownloadParam* pBMPParam = (BitmapDownloadParam*) pParam;

	if (pBMPParam==NULL)
	{
		ERROR2RAW("OpBitmapDownload::OnDownloadFail - no download parameters");
		return;
	}

	//Get the URL which the user typed
	String_256 strURL=pBMPParam->strURL;

	//And put it up as a message
	String_256 strMessage;
	strMessage.MakeMsg(_R(IDS_HTML_DOWNLOADFAILED), &strURL);
	Error::SetError(0, strMessage, 0);
	InformError();
}

