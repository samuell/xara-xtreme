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
#include "document.h"
//#include "filtrres.h"
#include "oilfltrs.h"
#include "htmlfltr.h"
#include "htmllist.h"
#include "backgrnd.h"
//#include "htmldwnd.h"
#include "fileutil.h"
#include "fixmem.h"		// CCMalloc, CCFree
#include "spread.h"		// See FindFirstImportPosition below
#include "cutop.h"		// To get the duplication distance
#include "webaddr.h"	// class WebAddress
#include "docview.h"	//For DocView::FindTopLeftImportPosition

//#include "resimmap.h"	//For some resources


DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(HTMLFilter, BaseTextFilter)


#define new CAM_DEBUG_NEW

//#define MAXTAGLENGTH	255
#define MAXTAGLENGTH	10240	// 10Ko	

// HTML tags
#define HTMLTAG_UNKNOWN		-1		// unknown tag
#define HTMLTAG_UNUSED		0		// known tags but unused during parsing

#define HTMLTAG_IMGSRC		1
#define HTMLTAG_BODY		2
#define HTMLTAG_BODYEND		3
#define HTMLTAG_LINK		4
#define HTMLTAG_LINKEND		5
#define HTMLTAG_INPUT		6
#define HTMLTAG_SELECT		7
#define HTMLTAG_HTML		8
#define HTMLTAG_HTMLEND		9
#define HTMLTAG_TITLE		10
#define HTMLTAG_TITLEEND	11
#define HTMLTAG_HEAD		12
#define HTMLTAG_HEADEND		13
#define HTMLTAG_CENTER		14
#define HTMLTAG_CENTEREND	15
#define HTMLTAG_EMBED		16
////// add more TAGS here //////

// how many tags ?
#define HTMLNUM_TAGS		16

BOOL HTMLFilter::ImportingHTML = FALSE;

// list of all tags
static struct {
	TCHAR *tag;		
	HTMLTAG_TYPE Id;		
} HTML_Table[] = {	{_T("img"),	HTMLTAG_IMGSRC},
					{_T("body"),	HTMLTAG_BODY},
					{_T("input"),	HTMLTAG_INPUT},
					{_T("center"),	HTMLTAG_CENTER},
					{_T("/center"),HTMLTAG_CENTEREND},
					{_T("embed"),	HTMLTAG_EMBED},

					// The following tags are not used during the parsing,
					// but are used during HowCompatible()
					{_T("a href"),	HTMLTAG_UNUSED},
					{_T("select"),	HTMLTAG_UNUSED},
					{_T("/body"),	HTMLTAG_UNUSED},
					{_T("/a"),		HTMLTAG_UNUSED},
					{_T("html"),	HTMLTAG_UNUSED},
					{_T("/html"),	HTMLTAG_UNUSED},
					{_T("title"),	HTMLTAG_UNUSED},
					{_T("/title"),	HTMLTAG_UNUSED},
					{_T("head"),	HTMLTAG_UNUSED},
					{_T("/head"),	HTMLTAG_UNUSED},
									
					////// add more TAGS here //////
					///// just add the FIRST element of the HTML tag ///////
					///// see doc about the parser for more details /////
};


// Lookup for a token in the list of known HTML tags
static INT32 FindTagId(const TCHAR* Token)
{
	//Changed by Graham 29/5/97 so the comparison is case insensitive
	for (INT32 i=0;i<HTMLNUM_TAGS;i++)
		if (camStricmp(HTML_Table[i].tag,Token) == 0) return (HTML_Table[i].Id);

	return (HTMLTAG_UNKNOWN);
}

// reverse a string
static BOOL ReverseString(String_256& In, char* OutString)
{
	if (In.IsEmpty() && OutString == NULL)
		return FALSE;

	char* InString = (TCHAR*)In;

	//INT32 length = strlen(InString)-1;
	INT32 length = In.Length()-1;

	char* OutStringTmp = OutString;
	
	while ( length != -1 )
	{
		*OutStringTmp = *(InString+length);
		OutStringTmp++;
		length--;
	}

	*OutStringTmp = '\0';

	return TRUE;
}



/********************************************************************************************
>	HTMLFilter::HTMLFilter()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/03/97
	Purpose:	Constructor.  Initialises member variables and load ressources
********************************************************************************************/
HTMLFilter::HTMLFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDS_FILTERNAME_HTML));
	FilterInfo.Load(_R(IDS_FILTERINFO_HTML));
	FilterID = FILTERID_HTML;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

	m_WarningsString.Empty();
}

/********************************************************************************************
>	HTMLFilter::~HTMLFilter()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/03/97
	Purpose:	Destructor.  Frees memory and cleans up.
********************************************************************************************/
HTMLFilter::~HTMLFilter()
{
	//DeInit1();
}


/********************************************************************************************

>	INT32 HTMLFilter::Init()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/03/97
	Returns:	TRUE if init ok
	Purpose:	Initialisation of the filter

********************************************************************************************/

INT32 HTMLFilter::Init()
{
	// allocate memory for member variables
	//if (!Init1())
	//	return FALSE;

	// Get the OILFilter object
	pOILFilter = new HTMLOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// init the Bitmap download operation
	 if(!OpBitmapDownload::Init())
		 return FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL HTMLFilter::PreHowCompatible()

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/03/97
	Returns:	True if it it worked ok.
	Purpose:	Allows a filter or set of filters to take some action or set up variables
				before being called by HowCompatible. A group of filters which share common
				code may require some variable set up before being asked to check the type.  
				Base class does nothing and so works for all filters which do not require it.
	SeeAlso:	HowCompatible

********************************************************************************************/

BOOL HTMLFilter::PreHowCompatible()
{
	return TRUE;
}

/********************************************************************************************

>	INT32 HTMLFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,UINT32 FileSize)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/03/97
	Returns:	a value <= 10 depending on how we like the file. 10 mean it's HTML, 0 is definetely
				not HTML.
	Purpose:	check whether the file is HTML or not
				I've considered that the file is HTML as soon as there is one HTML tags.
	SeeAlso:	

********************************************************************************************/

INT32 HTMLFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (FileSize == 0 || pOILFilter == NULL)
		return 0;

	INT32 IsCompatible = 0;
	INT32 MaxCompatible = 0;

	// If this file doesn't have the right extension, then claim with a value lower than 10.
	// If we don't check the extension, this function will return 10 if it finds any HTML tag ID
	// within the buffer, even if the string is not contained within "<>"
	//
	// The header could be the text part on an EPS file (e.g. v1.1 native file format), so without
	// the extension match, this filter may try and import it, instead of leaving it to the EPS filter.
	//
	// Really, this function should be vamped up so that it spots real HTML tags, rather than just the tag ID
	// string. (Markn & Neville 7/8/97)
	// Reduced the 6 to 4 as most EPS filters return 5 if the %!Adobe is at the start of the file
	// Neville 27/10/97
	if (pOILFilter->DoesExtensionOfPathNameMatch(&Filename))
		MaxCompatible = 10;
	else
		MaxCompatible = 4;

	// Convert to lower case
	// CAN NOT do this on the main buffer as this would screw up all filters in the
	// chain below us. So take a copy of the buffer and then lower case that
	char * CopyFilterBuf = (char *) CCMalloc(HeaderSize);
	if (CopyFilterBuf == NULL)
		return 0;
	
	// Take a copy of the buffer
	memcpy(CopyFilterBuf, HeaderStart, HeaderSize);
	// and now finally, lower case it
	CopyFilterBuf = _strlwr(CopyFilterBuf);

	// all known HTML tags
	for (INT32 i=0;i<HTMLNUM_TAGS;i++)
	{
/*		TCHAR* Tag = new TCHAR[camStrlen(HTML_Table[i].tag)+3];
		*Tag = '\0';
		camStrcat(Tag, "<");
		camStrcat(Tag, HTML_Table[i].tag);
*/

		// is there an occurence of the string 'HTML_Table[i].tag' in the string 'HeaderStart'
		if ( SearchStr(CopyFilterBuf, HTML_Table[i].tag) != NULL )
		{
			IsCompatible = MaxCompatible;
//			delete [] Tag;
			break;

		}
//		delete [] Tag;
	}

	CCFree(CopyFilterBuf);

	return IsCompatible;
}

/********************************************************************************************

>	BOOL HTMLFilter::DoImport(SelOperation *Op, CCLexFile* pFile, Document *DestDoc,
							 		BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL, String_256* URL)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/03/97
	Inputs:		Op - pointer to the operation that this input process is associated with.
				pFile - The file that we should load
				DestDoc - 
				URL - URL of the original imported file
	Returns:	TRUE if the input operation worked ok, FALSE if not.
	Purpose:	
	Errors:		Fails (returns FALSE) if the document structure is incorrect, or if there
				is a problem with the bitmap.

********************************************************************************************/

BOOL HTMLFilter::DoImport(SelOperation *Op, CCLexFile* pFile, Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos,
						  KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	ERROR2IF(Op == NULL, FALSE, 		"HTMLFilter::DoImportNULL operation supplied");
	ERROR2IF(DestDoc == NULL, FALSE, 		"HTMLFilter::DoImportNULL document supplied");

	// Blank our warnings string
	m_WarningsString.Empty();
	
	HTMLFileList* HTMLList = new HTMLFileList;

	// Parse the file and add bitmap names in the list
	BOOL ok = ParseHTMLFile(pFile, HTMLList);

	//Now let's get the path to the file we are importing
	PathName pthImport=pFile->GetPathName();

	if (URL == NULL)
	{
		String_256 URLTemp = "";
		// import each bitmap
		ok = ScanListAndImport(HTMLList, &URLTemp, DestDoc, Op, pthImport);
	}
	else
		ok = ScanListAndImport(HTMLList, URL, DestDoc, Op, pthImport);
	
	// empty the list 
	if ( HTMLList->GetCount())
		HTMLList->DeleteAll();
	delete HTMLList;

	// Now check the warning about problem data string
	// If it has anything in then warn the user about the problem
	if (ok && !m_WarningsString.IsEmpty())
	{
		Error::SetError(0, m_WarningsString, 0);
		InformWarning();		
	}

	// All ok
	return ok;
}

/********************************************************************************************

>	BOOL HTMLFilter::ScanListAndImport(HTMLFileList* HTMLList, String_256 &Address, Document *DestDoc, PathName pthImportFile)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/97
	Inputs:		HTMLList - the list of bitmap
				Address - the original URL 
				DestDoc -
				pthImportFile - the path name of the HTML file that's being imported
	Returns:	TRUE 
	Purpose:	scan the list and call DoImportBitmap()
	
********************************************************************************************/

BOOL HTMLFilter::ScanListAndImport(HTMLFileList* HTMLList, String_256* URL, Document *DestDoc, SelOperation *Op, PathName pthImportFile)
{
	ERROR2IF(DestDoc == NULL, FALSE, 		"HTMLFilter::ScanListAndImport NULL document supplied");
	ERROR2IF(HTMLList == NULL, FALSE, 		"HTMLFilter::ScanListAndImport NULL list supplied");
	ERROR2IF(URL == NULL, FALSE, 		"HTMLFilter::ScanListAndImport NULL URL supplied");

	// make sure there is at least one file in the list
	INT32 FileNumber = HTMLList->GetCount();

	//If there were no files in the list, tell the user
	ERROR1IF(FileNumber==0, TRUE, _R(IDE_HTMLIMPORT_NOIMAGESINFILE));

	// current file
	INT32 FileNum = 0;
	//Get the first position at which to import a bitmap
	DocCoord PosTranslate=GetImportPosition(DestDoc, FileNum);
	

	// first, get a pointer to the first element
	HTMLFileListItem* ListBitmaps = (HTMLFileListItem*)HTMLList->GetHead();
	String_256 NewName;

	// scan the list
	while(ListBitmaps != NULL)
	{
		// get the Tag ID associated with the bitmap
		HTMLTAG_TYPE TagId = ListBitmaps->GetTagId();

		//Get the name of the bitmap
		NewName= ListBitmaps->GetFileName();

		// if only it's a web address then I call the bitmap download operation
		if (WebAddress::IsHTTP(*URL) || WebAddress::IsHTTP(ListBitmaps->GetFileName()))
		{
		
			// create the webaddress according base and relative URL
			WebAddress WebFileName(ListBitmaps->GetFileName());
			WebFileName.Combine(*URL);							// combine the existing URL with the filename

			// reconstitute the web address
			String_256 WebPath = WebFileName.GetWebAddress();

			// call the bitmap download op
			//Graham 30/5/97: When the asynchronous download op completes,
			//it will delete this object
			BitmapDownloadParam* Param=new BitmapDownloadParam;
	
			OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_OPBITMAPDOWNLOAD);

			if (pOpDesc != NULL)
			{	
				// get the temporary dir
				TCHAR TempDir[MAX_PATH];
				FileUtil::GetTemporaryPath(MAX_PATH,TempDir);
				// get temp file name
				TCHAR TempFileName[256];
				TCHAR Prefix[] = "BMP"; // the first three letters of the new file name
				FileUtil::GetTemporaryFileName(TempDir,Prefix,0,TempFileName);

				// extract the original filename
				INT32 lg = WebPath.Length()+1;
				char* TmpPath = new char[lg];
				char* TmpPath2=TmpPath;
				ReverseString(WebPath, TmpPath);
				char* RevFileName = new char [lg];
				char* FileNameTmp = RevFileName;
				while (*TmpPath2!='\\' && *TmpPath2!='/' && *TmpPath2!=NULL)
					*(FileNameTmp++) = *(TmpPath2++);
				*FileNameTmp = '\0';

				// we download only if there is a file to download
				if (*RevFileName != '\0')
				{
					String_256 StrRevFileName(RevFileName);
					char* FileName = new char[lg];
					ReverseString(StrRevFileName, FileName);
					//delete [] OutPath;
					String_256 StrFileName(TempDir);
					StrFileName += String_256(FileName);

					// fill in the struct (minimum necessary information)
					Param->type = TYPE_BITMAP;
					Param->priority = AsynchDownload::PRIORITY_NORMAL;
					Param->file = String_256(TempFileName);					// temporary file name
					//Param->file = StrFileName;									// temporary file name
					Param->strURL = WebPath;
					//Param->strDescription = "Bitmap file Download";
					BOOL ok = TRUE;
					Param->Output = &ok;

					// other information relative to the filter
					//Param->m_Op = Op;								// I need the Op
					Param->m_FileName = FileName;					// original filename
					Param->m_DestDoc = DestDoc;						// I also need the document
					Param->m_TagId = (INT32)TagId;					// Id of tag associated with the bitmap
					Param->m_FileNum = FileNum;						// current file
					Param->m_FileNumber = FileNumber;				// number of files
					Param->m_PosTranslate = PosTranslate;			// coordinates of the bitmap in the document
					Param->m_IsCenter = FALSE;			

					// Invoke the operation
					pOpDesc->Invoke((OpParam*) Param);

					//Graham 2/6/97: Delete FileName
					delete [] FileName;
				}

				//Graham 2/6/97: And delete everything we need to delete
				delete [] TmpPath;
				delete [] RevFileName;

			}
			else	
				return FALSE;
		}
		else		// we can directly call the DoImportBitmap as the file is not a URL
		{
			//Rewritten by Graham 2/6/97

			//First get the name of the file we're going to import
			String_256 szFileName = ListBitmaps->GetFileName();

			//Then make a copy of the path of the file we are importing
			PathName Path=pthImportFile;

			//And set the file name of that path to the file name we are going
			//to import.
			Path.SetFileNameAndType(szFileName);

			//And import the bitmap
			if (!DoImportBitmap(TagId, Op, DestDoc, Path, FileNum, FileNumber, PosTranslate, FALSE, &NewName))
			{
				// Make up an error string adding a default 'Problem loading images. This includes' and
				// then add the pathname.
				if (m_WarningsString.IsEmpty())
				{
					m_WarningsString = String_256(_R(IDE_HTMLIMPORT_BADIMAGESINFILE));
				}

				String_256 Sep(_R(IDS_TAG_ERROR_LIST_SEP)); 
				String_256 Temp = Path.GetPath();
				// Only add it if there is enough room to fit it all in
				if (Sep.Length() + Temp.Length() + m_WarningsString.Length() < m_WarningsString.MaxLength())
				{
					m_WarningsString += Sep;
					m_WarningsString += Temp;
				}
				else
				{
					// Try using the filename plus extension only
					Temp = Path.GetFileName();
					if (Sep.Length() + Temp.Length() + m_WarningsString.Length() < m_WarningsString.MaxLength())
					{
						m_WarningsString += Sep;
						m_WarningsString += Temp;
					}
				}
			}
		}
		
		FileNum++;

		// Update the import position translation
		PosTranslate=GetImportPosition(DestDoc, FileNum);
		
		// Get the next element in the list
		ListBitmaps = (HTMLFileListItem*)HTMLList->GetNext(ListBitmaps);
	}

	return TRUE;
}


/********************************************************************************************

>	static BOOL HTMLFilter::DoImportBitmap(HTMLTAG_TYPE TagId, SelOperation *Op, Document *DestDoc, 
								PathName Path, INT32 FileNum, 
								INT32 FileNumber, DocCoord& PosTranslate, BOOL IsCentre=FALSE)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/97
	Inputs:		TagId - tag associated with the bitmap
				Op - pointer to the operation that this input process is associated with.		
				DestDoc - 
				ppImportedBitmap - KernelBitmap for bitmap background
				Path - Path of the file
				FileNum - current file
				FileNumer - number of files
	Returns:	TRUE 
	Purpose:	how to import (bitmap background or formatting bitmap)
	
********************************************************************************************/


BOOL HTMLFilter::DoImportBitmap(HTMLTAG_TYPE TagId, SelOperation *Op, Document *DestDoc, 
								PathName Path, INT32 FileNum,	INT32 FileNumber, 
								DocCoord& PosTranslate, BOOL IsCentre, String_256* NewName)
{
	BOOL ok = FALSE;
	KernelBitmap* pKernelBitmap = NULL;

	if (TagId == HTMLTAG_BODY)
	{
		// if it's body background then
		if (!ImportBitmap(Op, Path, &pKernelBitmap, NULL, FileNum+1, FileNumber, NewName))
			return FALSE;
		
		Spread *pSpread = NULL;
		pSpread = GetFirstSpread(DestDoc);

		// can't be NULL
		if (!pSpread && !pKernelBitmap)
			return FALSE;

		// call the background op
		OpBackground::MakePageBackground(Op, pSpread, pKernelBitmap);
		return TRUE;
	}
	else
	{
				//Taken out by Graham 12/6/97
		/*if (IsCentre)
		{
			// formatting, arbitrary position :-(
			PosTranslate.x =  288000;
			//PosTranslate.y = -(72000*0.2)*i;
			
		} */
		
		//Modified by Graham 30/5/97 so it *doesn't* leak every bitmap it tries
		//to import! 8-)
		if (!ImportBitmap(Op, Path, &pKernelBitmap, &PosTranslate, FileNum+1, FileNumber, NewName))
			return FALSE;

	}

	//And finally, set the name of the bitmap to our new name
	if (pKernelBitmap && NewName)
		pKernelBitmap->SetName(*NewName);

	return TRUE;
}


/********************************************************************************************

>	static BOOL HTMLFilter::ParseHTMLFile(CCLexFile* pFile, HTMLFileList* HTMLList)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com> (rewritten by Graham)
	Created:	30/04/97
	Inputs:		Op - pointer to the operation that this input process is associated with.
				pFile - The file that we should load
				DestDoc - 
	Returns:	TRUE if the input operation worked ok, FALSE if not.
	Purpose:	Parse the html file then create the list of bitmaps and finally call ImportBitmap
	Errors:		Fails (returns FALSE) if the document structure is incorrect, or if there
				is a problem with the bitmap.

********************************************************************************************/


BOOL HTMLFilter::ParseHTMLFile(CCLexFile* pFile, HTMLFileList* HTMLList)
{
	//First check our parameters
	ERROR2IF(pFile == NULL, FALSE, 	"HTMLFilter::ParseHTMLFile NULL diskfile supplied");
	ERROR2IF(HTMLList == NULL, FALSE, "HTMLFile::PArseHTMLFile NULL HTMLList supplied");

	// Set up some variables
	BOOL IsCenter = FALSE;	// this is used for formatting
	TCHAR* pTagName = new TCHAR[MAXTAGLENGTH];
	TCHAR* pTagAttrib = new TCHAR[MAXTAGLENGTH];
	TCHAR* pTokenBuf = new TCHAR[MAXTAGLENGTH];

	//First initialise the lexer
	pFile->InitLexer(FALSE, TRUE);

	HTMLTAG_TYPE Token = HTMLTAG_UNKNOWN;

	BOOL ok = TRUE;

	//And now do the parsing until we reach the end of the file
	while (!pFile->IsEndOfHTMLFile())
	{
		//First get an HTML token. This token will either be
		//a tag "<xxxx>" or text "xxxx".
		//The two parameters mean that we want to ignore any EOL characters
		//but we do not want the case to be corrected to upper case
		pFile->GetHTMLToken(TRUE, FALSE);

		//Now, if the HTML token is a tag
		if (pFile->IsHTMLTag())
		{
			//Then get the name of the tag
			String_256 szTagName=pFile->GetHTMLTagName();

			//Now try and find our token in the HTML table
			Token = FindTagId(szTagName);

			switch (Token)
			{
				case HTMLTAG_IMGSRC:
				{	
					//It's an IMG tag.
					//Get the value of the SRC parameter. The "FALSE" indicates we shouldn't
					//correct the case of the value
					String_256 szValue=pFile->GetHTMLParameterValue(String_256("SRC"), FALSE);
					
					//And, if we found an SRC parameter value
					if (!szValue.IsEmpty())
					{
						//Create a new HTMLFileListItem
						HTMLFileListItem* NewItem = new HTMLFileListItem((TCHAR*) szValue, Token, FALSE);

						//And try to add it to the list
						//If it doesn't get added, delete it
						if (!HTMLList->Add(NewItem))
							delete NewItem;
					}
					
				}
				break;

			case HTMLTAG_EMBED:
				{	
					//It's an EMBED tag.
					//Get the value of the SRC parameter. The "FALSE" indicates we shouldn't
					//correct the case of the value
					String_256 szValue=pFile->GetHTMLParameterValue(String_256("SRC"), FALSE);
					
					//And, if we found an SRC parameter value
					if (!szValue.IsEmpty())
					{
						//Create a new HTMLFileListItem
						HTMLFileListItem* NewItem = new HTMLFileListItem((TCHAR*) szValue, Token, FALSE);

						//And try to add it to the list
						//If it doesn't get added, delete it
						if (!HTMLList->Add(NewItem))
							delete NewItem;
					}
					
				}
				break;

			case HTMLTAG_BODY:
				{
					// It's a BODY tag

					//Get the value of the BACKGROUND parameter, if any
					String_256 szValue=pFile->GetHTMLParameterValue(String_256("BACKGROUND"), FALSE);

					//And if we found a value
					if (!szValue.IsEmpty())
					{
						//Create a new HTMLFileListItem
						HTMLFileListItem* NewItem = new HTMLFileListItem((TCHAR*) szValue, Token, FALSE);
						
						//And try to add it to the list
						//If it doesn't get added, delete it
						if (!HTMLList->Add(NewItem))
							delete NewItem;
					}
				}
				break;

			case HTMLTAG_INPUT:	// e.g. : <input type=image src="image.gif">
				{
					//It's an input tag

					//Get the value of the TYPE parameter, if any
					String_256 szTypeValue=pFile->GetHTMLParameterValue(String_256("TYPE"), FALSE);

					//And if we found a TYPE parameter and its value was
					//IMAGE
					if (camStricmp(szTypeValue, "image")==0)
					{
						// Then look for an "SRC" parameter
						String_256 szSRCValue=pFile->GetHTMLParameterValue(String_256("SRC"), FALSE);

						//And if we found an SRC parameter value
						if (!szSRCValue.IsEmpty())
						{
							//Then create a new HTMLFileListItem
							HTMLFileListItem* NewItem = new HTMLFileListItem((TCHAR*) szSRCValue, Token, FALSE);

						//And try to add it to the list
						//If it doesn't get added, delete it
						if (!HTMLList->Add(NewItem))
							delete NewItem;
						}
					}
				}
				break;

			case HTMLTAG_UNUSED:
			case HTMLTAG_UNKNOWN:
			default:
				break;
			}
		}

		Token = HTMLTAG_UNKNOWN;
	}

	// We are now finished with the lexer
	pFile->DeinitLexer();

	delete [] pTagName;
	delete [] pTagAttrib;
	delete [] pTokenBuf;

	// All ok
	return TRUE;
}



/********************************************************************************************

>	static BOOL HTMLFilter::ImportBitmap(SelOperation *Op, PathName &Path, KernelBitmap** ppImportedBitmap, 
								DocCoord* pPosTranslate, INT32 FileNum, INT32 FileNumber, String_256* NewName)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/97
	Inputs:		Op - 
				Path = the path of the file to import
				ppImportedBitmap - The kernelBitmap I need to set the bitmap background up
				pPosTranslate - used for formatting
				FileNum - number of the file presently imported
				NewName - name of the bitmap
	Returns:	TRUE if import ok 
	Purpose:	
	Errors:		Fails (returns FALSE) if can't import the bitmap
				
********************************************************************************************/

BOOL HTMLFilter::ImportBitmap(SelOperation *Op, PathName &Path, KernelBitmap** ppImportedBitmap, 
							  DocCoord* pPosTranslate, INT32 FileNum, INT32 FileNumber, String_256* NewName)
{
	BOOL ok = FALSE;

	if (!SGLibOil::FileExists(&Path))
		return FALSE;

	// which filter to use ?
	Filter *pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		// what kind of bitmap is it ?
		UINT32 Size = 1024;
		INT32 FileSize;
		ADDR FilterBuf = pFilter->LoadInitialSegment(Path, &Size, &FileSize);

		// If there has been a problem in the load initial segment then fail now.
		if (FilterBuf == NULL)
		{
			ok = FALSE;
			break;
		}

		// Inform any filters that we are about to do a HowCompatible call.
		// This would allow a set of filters which have common functionality hidden in a
		// filter that cannot import and cannot export handle this call and hence set
		// itself up. This would allow it to maybe cache a result which should only be
		// checked by the first filter in the group. 
		pFilter->PreHowCompatible();
			
		// Change this to be less than 8 as the filters like the Accusoft forms return
		// 8 and 9 to make sure that they are last in the chain.
		if (pFilter->HowCompatible(Path, FilterBuf, Size, FileSize) > 8)
		{
			ok = TRUE;
			CCFree(FilterBuf);
			// This is the filter!
			break;
		}

		//Free the initial segment
		CCFree(FilterBuf);

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// filter found ?
	if (pFilter == NULL || !ok)
		return FALSE;		// no filter found

	// now import bitmap

	// First off, we have to try and open the file
	CCDiskFile DiskFile(1024, FALSE, TRUE);

	// Get pointer to current doc 'cos we'll need it several times...
	Document* pCurDoc = Document::GetCurrent();

	// open the file
	if (!DiskFile.open(Path, ios::in | ios::binary))
		return FALSE;

	//Graham 15/7/97: Now, we want to import into the top left hand corner of the view
	DocView* pDocView=DocView::GetCurrent();

	ERROR2IF(pDocView==NULL, FALSE, "HTMLFilter::DoImport - no current view");

	ImportPosition posImport=pDocView->GetCentreImportPosition();

	//And add on the amount by which to translate the bitmap
	if (pPosTranslate)
	{
		posImport.Position.x+=pPosTranslate->x;
		posImport.Position.y+=pPosTranslate->y;
	}

	// then import the file
	if (!pFilter->DoImport(Op, &DiskFile, pCurDoc, FALSE, NULL, ppImportedBitmap, pPosTranslate, NULL))
	{
		// Close the file again
		if (DiskFile.isOpen())
			DiskFile.close();

		Error::ClearError();	// remove the error so it won't get reported

		return FALSE;
	}

	// Close the file again
	if (DiskFile.isOpen())
		DiskFile.close();

	return TRUE;
}


/********************************************************************************************

>	BOOL HTMLFilter::GetTag(TCHAR* pTokenBuf, TCHAR* pTagName, const TCHAR* TokenBuffer)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/97
	Inputs:		TokenBuffer: e.g. : 'img src="image.gif"'
	Returns:	TRUE if it had found another known HTML tag
	Purpose:	extract the TAGNAME from the tokenbuf 
				(e.g.: will extract 'img' from 'img src=image.gif')
	Errors:		

********************************************************************************************/

BOOL HTMLFilter::GetTag(TCHAR* pTokenBuf, TCHAR* pTagName, const TCHAR* TokenBuffer)
{
	if (TokenBuffer == NULL)
		return FALSE;

	// convert to lower case
	camStrcpy(pTokenBuf, TokenBuffer);
	pTokenBuf = _strlwr(pTokenBuf);

	// all known HTML tags
	for (INT32 i=0;i<HTMLNUM_TAGS;i++)
	{
		const TCHAR* TokenBufTmp = pTokenBuf;
		*pTagName = '\0';
		TCHAR* TagNameTmp = pTagName;
		TCHAR* HTML_Tag = HTML_Table[i].tag;

		while(*TokenBufTmp == *HTML_Tag && *TokenBufTmp!=NULL && *HTML_Tag!=NULL)
		{
			*(TagNameTmp++) = *(TokenBufTmp++);
			HTML_Tag++;
		}
		if (*HTML_Tag == NULL)
		{
			*TagNameTmp = '\0';
			break;
		}
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL HTMLFilter::GetAttribute(const TCHAR* AttrName, TCHAR* pTokenBuf, TCHAR* pTagAttrib)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/04/97
	Inputs:		AttrName - store the name of the tag attribute (e.g. : img, or src....)
	Returns:	TRUE if it had extracted the attribute
	Purpose:	Extract the attribute from the tag
	Errors:		

********************************************************************************************/

BOOL HTMLFilter::GetAttribute(const TCHAR* AttrName, const TCHAR* pTokenBuf, TCHAR* pTagAttrib)
{
	// they shouldn't be NULL
	if (AttrName == NULL && pTokenBuf == NULL && pTagAttrib == NULL)
		return FALSE;

	// e.g. : 'img src=image.gif'
	// we move the pointer until 'src'
	const TCHAR* TokenBufTmp = pTokenBuf;
	TCHAR* TagAttribute = pTagAttrib;
	while (TRUE)
	{
		const TCHAR* AttrNameTmp = AttrName;
		// find the first occurence of the first letter of AttrName
		TokenBufTmp = strchr(TokenBufTmp, (INT32)(*AttrName));
		if (TokenBufTmp == NULL)
			return FALSE;

		// extract "AttrName" from TokenBuf
		while ( *TokenBufTmp == *AttrNameTmp && *AttrNameTmp!=NULL && *TokenBufTmp!=NULL)
		{
			TokenBufTmp++;
			AttrNameTmp++;
		}
		if (*TokenBufTmp == NULL)
			return FALSE;
		if (*AttrNameTmp == NULL)
			break;
	}

	// move the pointer to reach the first letter
	while ( (*TokenBufTmp=='=' || *TokenBufTmp==' ' ||
			*TokenBufTmp=='\t' || *TokenBufTmp=='\"') && 
			*TokenBufTmp!=NULL)
	{
		TokenBufTmp++;
	}
	if (*TokenBufTmp == NULL)
		return FALSE;

	// extract the attribute
	while ( *TokenBufTmp!='\"' && *TokenBufTmp!=' ' && *TokenBufTmp!='\t' && *TokenBufTmp!=NULL)
	{
		*(TagAttribute++) = *(TokenBufTmp++);
	}

	*TagAttribute = '\0';

	return TRUE;
}



/********************************************************************************************

>	TCHAR* HTMLFilter::SearchStr(const char* A, const char* B)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/04/97
	Inputs:		A - string to search
				B- string to search for
				
	Returns:	a pointer to the first occurence of B
	Purpose:	Search for string B in string A
	Errors:		

********************************************************************************************/

TCHAR* HTMLFilter::SearchStr(const char* A, const char* B)
{
	if (A==NULL || B==NULL)
		return NULL;

	return ((TCHAR*)strstr(A, B));
}

/********************************************************************************************

>	DocCoord HTMLFilter::GetImportPosition(Document* pdocTarget, INT32 iPosNumber)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/6/97
	Inputs:		pdocTarget		- The document to import into
				iPosNumber		- The number of the position
				
	Returns:	The offset by which to offset bitmap number iPosNumber.
	Purpose:	Finds the offset by which to offset the iPosNumber-th bitmap
				imported.

				The first bitmap imported is offset by 0,0.
				
				Then the next bitmap is offset by the standard offset
				which the duplication operation uses.

				And so on: the offset of the Nth bitmap equals
				the offset of the (N-1)th bitmap plus the duplication offset.

				This carries on until the offset is such so that the offset
				of the Mth bitmap would place it off the spread.

				Instead, then, the offset of the Mth bitmap is the horizontal 
				component of the duplication offset. And then we go
				back to the general iterative rule.

				Not very well explained I'm afraid but I've got to get this done.

	Errors:		Null parameters

********************************************************************************************/

DocCoord HTMLFilter::GetImportPosition(Document* pdocTarget, INT32 iPosNumber)
{
	//First check our parameter
	ERROR2IF(pdocTarget==NULL, DocCoord(-1,-1), "HTMLFilter::GetFirstImportPosition - NULL parameter");

	//This variable will remember how many times we have returned the import position
	//back to the top left hand corner
	INT32 iReturned=0;

	//Now get the first spread in the document
	Spread* psprFirst=pdocTarget->FindFirstSpread();

	//Do some checking
	ERROR2IF(psprFirst==NULL, DocCoord(-1, -1), "HTMLFilter::GetFirstImportPosition - no spread in document");

	//And get its bounding rectangle
	DocRect rectBounding;
	psprFirst->GetPagesRect(&rectBounding);

	//Now translate that rectangle so that its top left hand corner is at
	//0,0
	DocCoord dcTopLeft(rectBounding.lo.x, rectBounding.hi.y);

	rectBounding.Translate(-dcTopLeft.x, -dcTopLeft.y);
	
	//Now, the first bitmap will be imported at 0,0
	DocCoord dcThisPosition(0, 0);

	//And this is the standard duplication offset
//	DocCoord dcOffset(OpDuplicate::DuplicatePlacementX, OpDuplicate::DuplicatePlacementY);
	DocCoord dcOffset = pdocTarget->GetDuplicationOffset();



	//Do the following a number of times equal to the number
	//of the bitmaps we are importing
	for (INT32 i=0; i<iPosNumber; i++)
	{
		//Increase dcThisPosition by dcOffset
		dcThisPosition.x=dcThisPosition.x+dcOffset.x;
		dcThisPosition.y=dcThisPosition.y+dcOffset.y;
		
		//Now, if that import position is outside the spread
		if (!rectBounding.ContainsRectCoord(dcThisPosition))
		{
			//Then set the import position back to the top left hand corner of the
			//spread
			dcThisPosition=DocCoord(rectBounding.lo.x, rectBounding.hi.y);

			//And increase our count of the number of times we have set the 
			//import position back to the top left hand corner
			iReturned++;

			//And offset the import position by the horizontal component
			//of the duplication distance, a number of times equal to 
			//the number of times we have reset the import position
			dcThisPosition.x+=dcOffset.x*iReturned;

			//Now, if *that* import position is outside the spread
			if (!rectBounding.ContainsRectCoord(dcThisPosition))
			{
				//Then go back to the very first import position
				//and set iReturned to zero
				dcThisPosition=DocCoord(rectBounding.lo.x, rectBounding.hi.y);

				iReturned=0;
			}
		}

	}

	//And return our calculated import position
	return dcThisPosition;
}




