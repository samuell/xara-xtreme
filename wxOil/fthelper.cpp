// $Id$
// Implementation of the FlareTemplateHelper class
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

#include "fthelper.h"
#include "fileutil.h"
#include "ccfile.h"
#define NO_CAMTYPES
#include "templman.h"	// Scary bits

#include "txtmacro.h"	// MFC/ATL text converion macros
//#include "resource.h"	// out of memory error
//#include "filtrres.h"

extern "C"
{
//	const IID IID_IFTManipulator = {0x31A6E651,0xEBA2,0x11D0,{0x8C,0x33,0x00,0xA0,0x24,0x72,0x9D,0xDE}};
	const IID IID_IFTManipulator = {0x7B14A5DD,0xAA72,0x403b,{0xB0,0x1C,0x12,0x56,0xCA,0xF6,0x9D,0x67}};
//	const CLSID CLSID_FTManipulator = {0x31A6E652,0xEBA2,0x11D0,{0x8C,0x33,0x00,0xA0,0x24,0x72,0x9D,0xDE}};
	const CLSID CLSID_FTManipulator = {0x424C099D,0x6A26,0x4fea,{0xA5,0x66,0x45,0xCD,0xB0,0xEF,0x06,0x94}};
}


//-----------------------------------------------

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	CCLexFile* FlareTemplateHelper::ConvertFile(CCLexFile* pFile)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Purpose:	Default constructor
	SeeAlso:	-

********************************************************************************************/

CCLexFile* FlareTemplateHelper::ConvertFile(CCLexFile* pFile)
{
	ERROR2IF(pFile == NULL, FALSE, "NULL file in FTHelper::ConvertFile");

	String_256 InputFile = pFile->GetPathName().GetPath();

	if (InputFile.IsEmpty())
		return(NULL);

	BSTR pInputName = T2BSTR((TCHAR*)InputFile);
	if (pInputName == NULL)
	{
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		return(NULL);
	}

	HRESULT hr = S_OK;
	CCDiskFile* pNewFile = NULL;

	IFTManipulator* pTemplMan = CreateTemplMan();
	if (pTemplMan != NULL)
	{
		// Initialise the manipulator object
		HRESULT hr = pTemplMan->Init(pInputName);
		if (SUCCEEDED(hr))
		{
			PathName TempPath = FileUtil::GetTemporaryPathName();
			String_256 OutFile = TempPath.GetPath();
			if (!OutFile.IsEmpty())
			{
				// Get file name as a BSTR
				BSTR pOutputName = T2BSTR((TCHAR*)OutFile);
				// And process it (will fail if NULL)
				hr = pTemplMan->ProcessTemplateToFile(pOutputName);
				if (SUCCEEDED(hr))
				{
					// Create a default diskfile
					pNewFile = new CCDiskFile();
					if (pNewFile != NULL)
					{
						if (!pNewFile->open(TempPath, ios::in | ios::binary))
						{
							// If we failed then delete the file
							// and set the pointer to NULL for return
							delete pNewFile;
							pNewFile = NULL;
						}
					}
				}
				SysFreeString(pOutputName);
			}
		}
		// Release the interface
		pTemplMan->Release();
		pTemplMan = NULL;
	}

	if (FAILED(hr))
	{
		SetHResultError(hr);		// set the error
		ERROR3IF(pNewFile != NULL, "ConvertFile failed but created new file");
	}

	// Free the input BSTR
	SysFreeString(pInputName);

	return(pNewFile);
}


/********************************************************************************************

>	static IFTManipulator* FlareTemplateHelper::CreateTemplMan(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Returns:	IFTManipulator pointer or NULL if error
	Purpose:	Creates a FlareTemplateManipulator object
	Errors:		

********************************************************************************************/

IFTManipulator* FlareTemplateHelper::CreateTemplMan(void)
{
	IFTManipulator* pTemplMan = NULL;
		
	// Create an FTManipulator object
	HRESULT hr = CoCreateInstance(CLSID_FTManipulator,
									NULL,
									CLSCTX_INPROC_SERVER,
									IID_IFTManipulator,
									(void**)&pTemplMan);

	if (SUCCEEDED(hr))
	{
		return(pTemplMan);
	}
	else
	{
		SetHResultError(hr);
		return(NULL);
	}
}

/********************************************************************************************

>	static void FlareTemplateHelper::SetHResultError(HRESULT hr)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Purpose:	Sets an error from a TemplMan error HRESULT

********************************************************************************************/

void FlareTemplateHelper::SetHResultError(HRESULT hr)
{
	if (SUCCEEDED(hr))
		return;

	INT32 ErrorID = _R(IDS_TEMPLMAN_BAD_TEMPLATE);	// Default error
	INT32 PostFixID = 0;

	switch (hr)
	{
		case REGDB_E_CLASSNOTREG:
			ErrorID = _R(IDS_TEMPLMAN_NOT_INSTALLED);
			break;
	
		case E_OUTOFMEMORY:
			ErrorID = _R(IDS_OUT_OF_MEMORY);
			break;

		case E_BADTEMPLATE:
			break;

		case E_BADRECORD:
			{
				PostFixID = _R(IDS_TEMPLMAN_BAD_RECORD);
				break;
			}
		case E_BADDATAITEM:
			{
				PostFixID = _R(IDS_TEMPLMAN_BAD_DATA_ITEM);
				break;
			}
		default:
			{
				Error::SetError(_R(IDS_NOT_A_FLARETEMPLATE_FILE));
				return;
			}
	}

	String_256 strError;

	if (!strError.Load(ErrorID))			// Try to load the string
	{
		Error::SetError(ErrorID);			// Just set the error if it fails
		return;
	}

	if (PostFixID != 0)
	{
		String_256 strPostFix;
		if (!strPostFix.Load(PostFixID))	// Load the postfix string
		{
			Error::SetError(ErrorID);		// Just set the error if it fails
			return;
		}
		strError += strPostFix;				// and append it
	}

	// And set the error
	Error::SetError(ErrorID, strError, 0);		// Why doesn't the module param default to 0
}
