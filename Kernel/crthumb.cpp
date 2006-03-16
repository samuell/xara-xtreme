// $Id: crthumb.cpp 662 2006-03-14 21:31:49Z alex $
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
// The "Create Font Thumbnails..." debug menu item

#include "camtypes.h"

#include "pathname.h"
//#include "barsdlgs.h"		//For _R(IDD_BARCONTROLSTORE)

#include "crthumb.h"
#include "sgfonts.h"		//For FontGallery

#include "urlimp.h"
#include "fileutil.h"

//#include "resimmap.h"		//For _R(IDS_HTMLIMPORT_FILEDOWNLOAD)
#include "urldlg.h"			//For URLImportDlg

#include "gendwnd.h"		//For the download operation
//#include "webster.h"

#include "filedlgs.h"

#include "pngfiltr.h"

#include "sgindgen.h"

#include "sgfonts.h"	//class SGTTFItem
#include "camelot.h"
#include "menucmds.h"

extern CCamApp NEAR theApp;

#ifdef _DEBUG
CC_IMPLEMENT_DYNCREATE(OpCreateFontThumbnails, OpMenuImport)
CC_IMPLEMENT_DYNCREATE(OpConvertFileFormats, Operation)
BOOL OpConvertFileFormats::active = FALSE;

/********************************************************************************************

	How to use this code to create font thumbnails given a folder of TTF fonts

********************************************************************************************/


/********************************************************************************************

>	BOOL OpCreateFontThumbnails::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares the OpDescriptor. This uses some IDs it shouldn't, but it doesn't
				matter much because this is a debug command.
	Errors:		-

********************************************************************************************/

BOOL OpCreateFontThumbnails::Init()
{
	return RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_CREATEFONTTHUMBNAILS),					// String resource ID
									CC_RUNTIME_CLASS(OpCreateFontThumbnails), 	// Runtime class
									OPTOKEN_CREATEFONTTHUMBNAILS,			// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_CREATEFONTTHUMBNAILS),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_FILEURLIMPORT),				// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Receive system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,		// String for one copy only error
									0
 								);
	
}

BOOL OpConvertFileFormats::Init()
{
	return RegisterOpDescriptor(
									0, 							// Tool ID
									_R(IDS_CONVERTFILEFORMATS),					// String resource ID
									CC_RUNTIME_CLASS(OpConvertFileFormats), 	// Runtime class
									OPTOKEN_CONVERTFILEFORMATS,			// Token string
									GetState,					// GetState function
									0,							// help ID
									_R(IDBBL_CONVERTFILEFORMATS),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_FILEURLIMPORT),				// control ID
									SYSTEMBAR_FILE,				// Bar ID
									TRUE,						// Receive system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,		// String for one copy only error
									0
 								);
	
}

/********************************************************************************************

>	void OpCreateFontThumbnails::Do(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	Imports a URL into the current document
	Errors:		-

********************************************************************************************/

void OpCreateFontThumbnails::Do(OpDescriptor*)
{
	//First we put up a CFileDialog to allow the user to enter
	//the path to the index file
	CFileDialog dlgIndex(TRUE);

	dlgIndex.DoModal();

	PathName pthIndex=(const TCHAR*) dlgIndex.GetPathName();

	//Now we're going to open that file
	CCDiskFile fileIndex(1024, FALSE, TRUE);

	fileIndex.open(pthIndex, ios::in | ios::binary);

	//And get it ready for lexical reading...
	fileIndex.InitLexer();

	fileIndex.SetDelimiters("\r\n");		
	fileIndex.SetCommentMarker('#');
	fileIndex.SetWhitespace("\t");
	
	//This variable will point to the buffer that has been read in
	const TCHAR* TokenBuf = fileIndex.GetTokenBuf();

	//And this indicates the sort of token that has been read in
	LexTokenType tokType=TOKEN_NORMAL;

	//Now, start reading from the file until we reach the "START" token
	//or we reach the end of the file
	while (tokType != TOKEN_EOF && _tcscmp(TokenBuf, "START")!=0)
	{
		//Get a token frm the file
		fileIndex.GetToken();

		//Find out what sort of token it is
		tokType = fileIndex.GetTokenType();
	}

	//Now read until we reach the end of the file
	while (tokType!=TOKEN_EOF)
	{
		//Get a token frm the file
		fileIndex.GetToken();

		//Find out what sort of token it is
		tokType = fileIndex.GetTokenType();

		//Right, now let's find the first comma in the string
		TCHAR* pcFindPtr=_tcschr(TokenBuf, ',');

		//If we've found one
		if (pcFindPtr)
		{
			//Then move the string pointer on by two characters
			pcFindPtr=_tcsninc(pcFindPtr, 2);

			//And if we didn't go past the end of the string
			if (pcFindPtr && *pcFindPtr!='\0')
			{
				//Then pcFindPtr should now be pointing to the font name

				//Then find a pointer to the next comma in the string
				TCHAR* pcSecondComma=_tcschr(pcFindPtr, ',');

				if (!pcSecondComma)
					break;

				//And copy everything from the pcFindPtr (inclusive) to that second comma
				//(exclusive) into a new string
				String_256 strFontName;

				while (pcFindPtr && pcFindPtr!=pcSecondComma && *pcFindPtr!='\0')
				{
					strFontName+=(*pcFindPtr);
					pcFindPtr=_tcsinc(pcFindPtr);
				}

				//Now we want to find the ID of the font. That will occur after the
				//second comma. So set pcFindPtr so it points to two characters
				//after the second comma
				pcFindPtr=_tcsninc(pcSecondComma, 2);

				//And copy everything from the pcFindPtr (inclusive) to the third comma
				//(exclusive) into a new string
				String_256 strFontID;

				while (pcFindPtr && *pcFindPtr!=',' && *pcFindPtr!='\0')
				{
					strFontID+=(*pcFindPtr);
					pcFindPtr=_tcsinc(pcFindPtr);
				}

				//And translate that string into a number
				INT32 lFontID=atol(strFontID);

				//Now find the font item with that name
				SGDisplayPreviewFonts* pFontItem=FindFontFromName(strFontName);

				//And if there is a font with that name
				if (pFontItem)
				{
					//Then create three bitmap thumbnails from that font and
					//save them in the same directory as the index
					CreateThumbnailFiles(pFontItem, lFontID, pthIndex);
				}
			}
		}
	}
}



/********************************************************************************************

>	void OpConvertFileFormats::Do(OpDescriptor*)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/2000
	Purpose:	Converts all files of a one format into another format (e.g.  *.art => *.xar)
	Errors:		-

	NOTE:  The first three lines of this function are REALLY important!  You should do as their
	comments say ....  ALSO the code only works for a single directory (i.e.  it does NOT scan
	sub directories).  To do this, you will have to ammend this routine.

********************************************************************************************/

void OpConvertFileFormats::Do(OpDescriptor*)
{
	ERROR3 ("Please read the following comments BEFORE using this op!");	// comment this out to use the op

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// set up the search path, file util allows us just to pass the path/*.xar
	String_256 SearchPath = TEXT ("d:\\clipart\\xara\\");		// YOU MUST CHANGE THIS PATH
	
	String_256 FileSpecifier = SearchPath;						// YOU MUST CHANGE THIS EXTENSION
				FileSpecifier += TEXT("*.art");
	String_256 FileSpecifier2 = TEXT("xar");					// YOU MUST CHANGE THIS EXTENSION

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

	BOOL ok = FileUtil::StartFindingFiles(&FileSpecifier);
	active = TRUE;
	String_256 Filename;
	while (ok)
	{
		// fileutil will give us the next filename
		ok = FileUtil::FindNextFile(&Filename);
		String_256 FilePath = SearchPath;
		FilePath += Filename;
		PathName Path (FilePath);

		if (ok)
		{
			CDocument* pDoc = theApp.OpenDocumentFile((TCHAR*)FilePath);
			if (pDoc!=NULL)
			{
				// Make sure that the files name is sensible
				theApp.MakeDocumentNative(pDoc, &Path);
				
				Path.SetType (FileSpecifier2);
				String_256 newPath = Path.GetPath ();
				pDoc->DoSave ((TCHAR*) newPath, TRUE);

				FileCloseAction ();
			}
		}
	}
	active = FALSE;
	FileUtil::StopFindingFiles ();
}



/********************************************************************************************

>	void OpCreateFontThumbnails::CreateThumbnailFiles(SGDisplayPreviewFonts* pFontItem, INT32 lFontID, PathName pthIndex)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	Creates the thumbnail files
	Errors:		-
	Notes:		Karim 08/12/00 - modified to take SGDisplayPreviewFonts* instead, for ATM.

********************************************************************************************/

void OpCreateFontThumbnails::CreateThumbnailFiles(SGDisplayPreviewFonts* pFontItem, INT32 lFontID, PathName pthIndex)
{
	//We want to create three thumbnails: large, medium and small
	
	//So for each of the three display modes in the gallery...
	for (INT32 iDMode=0; iDMode<3; iDMode++)
	{
		//Set the display mode into the gallery
		SGDisplayPreviewFonts::DMode=iDMode;

		//Now create the thumbnail as a kernel bitmap
		KernelBitmap* pThisBitmap=NULL;

		pFontItem->CreateThumbnail(&pThisBitmap);

		//Now let's get a pointer to the PNG export filter.
		PNGFilter* pPNGFilter=(PNGFilter*) Filter::FindFilterFromID(FILTERID_PREVIEW_PNG);
				
		if (pPNGFilter==NULL)
			return;

		//Now create a file for the thumbnail
		CCDiskFile fileThumb(1024, FALSE, TRUE);

		//And get a path to save the thumbnail into
		TCHAR buf[256];

		switch (iDMode)
		{
				case 0:
					wsprintf(buf, TEXT("F%05liL.png"), lFontID);
					break;
				case 2:
					wsprintf(buf, TEXT("F%05liS.png"), lFontID);
					break;
				case 1:
				case 3:
				default:
					wsprintf(buf, TEXT("F%05liM.png"), lFontID);
					break;
		}
						
		PathName pthThumb=pthIndex;

		String_256 strThumbFile=buf;

		pthThumb.SetFileNameAndType(strThumbFile);

		//Now export the bitmap to that file
		pPNGFilter->DoExportBitmap(this, &fileThumb, &pthThumb, pThisBitmap);

		//And delete the bitmap
		if (pThisBitmap)
			delete pThisBitmap;
	}
}


/********************************************************************************************

>	SGDisplayPreviewFonts* OpCreateFontThumbnails::FindFontFromName(String_256& strName)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	Gets a pointer to the first font with the same name as that passed

				Note that to find the font with the same name as the 
	Errors:		-

********************************************************************************************/

/*
Clarifying note: 29/9/2000 DY.  I've just spent the day trying to make some font thumnails so I
include here the notes about what to do:

A quick guide to creating thumbnails for True Type fonts, using a debug
version of CorelXARA.

The first point to make is: this system is hacked. My brief in writing this
system was to get it done as quickly as possible and not worry about people
using it in the future. For "people using it in the future", read "you".
8-)

But, briefly speaking, this is how you do it.

1. First get a folder of TTF files (that is, font files). Don't install
them: instead, just put them in a separate folder somewhere.

2. Now you must add these files into the font gallery. This is not easy and
works as follows.

a. Go to winoil\sgindgen.cpp line 51, which reads:

BOOL GenerateIndexFile::CreateFontIndexes = FALSE;

 And change the FALSE to TRUE. Build a debug version of CorelXARA

b. Now run your debug version and open the Font Gallery. Click "Disc
fonts..." and locate your folder. You should get the error message:

"The folder 'D:\Fonts' doesn't have a Font index.Would you like to create a
new index for the folder?..."

If you haven't followed step 2a, you won't get this message.

Click "Create".

Your fonts will be added into the font gallery. However, when you open the
folder, you will see a series of grey boxes instead of the preview bitmaps.

3. Now to generate the preview bitmaps.

a. Open the font gallery and open the folder that contains the fonts for
which you want to create bitmaps.

b. Install all the new fonts so they appear in your Installed fonts folder.

c. Now go to the Debug menu and choose "Create Font Thumbnails". An open
dialog will appear. Go to the folder with the fonts in and open the
XaraInfo directory within it. Open XaraFont.txt within that directory.

d. Now go back to the Fonts Gallery and look at the folder of your fonts
again. The preview bitmaps should now be showing.

e. Scroll through and make sure the preview bitmaps are showing for every
font.

If they are, you've finished. If not, follow step 4.

4. The reason some preview bitmaps have not been created is because the
real font names are not the names that the galleries use for the fonts. For
example, the galleries refer to "Calmari Italic" as "CalmarItal".

I did warn you this was a hack.

a. Open the file kernel\crthumb.cpp and go to the function
OpCreateFontThumbnails::FindFontFromName. Scroll down until you see the
words "ADD NAMES OF NEW FONTS HERE", at about line 319.

Take a moment to look at the code and see how it works. This function works
as follows: a name of a font is passed in and a pointer to that font in the
fonts gallery is returned. However, first the name is corrected.

You're going to have to add two lines of code similar to the following:

if (strCorrectedName==String_256("CalmarItal"))
		strCorrectedName="CalmariItal";

to correct the name that the function is passed to the name that the
gallery stores.

b. To find the name that the function is passed (after it's been corrected
for things like spaces and "Italic" has been shortened to "Ital", stick a
breakpoint on line 320:

if (strCorrectedName==String_256("CacheExtraBold"))
		strCorrectedName="CacheExtra";

Then create some font thumbnails. You'll eventually see the name of your
problem font coming in.

c. Now stick a breakpoint on line 354:

String_256 strDescription=pLogFont->lfFaceName;

And eventually you'll see the name that your problem font is given in the
gallery.

d. Now actually add the two lines of code to correct the name of the font.

5. Repeat step 3, debugging the code until all the preview bitmaps are
created.

*/



SGDisplayPreviewFonts* OpCreateFontThumbnails::FindFontFromName(String_256& strName)
{
	//First get a pointer to the fonts gallery
	FontsSGallery* pGallery=FontsSGallery::ThisGallery;

	if (!pGallery)
		return NULL;

	//And a pointer to the display tree
	SGDisplayRoot* pTree=pGallery->GetDisplayTree();

	if (!pTree)
		return NULL;

	//Welcome to Hack City! Have a nice day...

	//Now we need to remove all the spaces from strName...it's a long story why
	String_256 strCorrectedName=strName;

	INT32 iSpace=strCorrectedName.FindNextChar(' ');

	while(iSpace>=0)
	{
		strCorrectedName.Remove(iSpace, 1);
		iSpace=strCorrectedName.FindNextChar(' ');
	}

	//And let's all replace all instances of the word italic with "Ital". Another long story.
	INT32 iItalic=strCorrectedName.Sub(String_256("Italic"));

	while(iItalic>=0)
	{
		strCorrectedName.Remove(iItalic, 6);
		strCorrectedName.Insert(&String_256("Ital"), iItalic);
		iItalic=strCorrectedName.Sub(String_256("Italic"));
	}

	//Now let's check for two particular fonts that have strange description strings
	if (strCorrectedName==String_256("CacheExtraBold"))
		strCorrectedName="CacheExtra";

	else if (strCorrectedName==String_256("CalmarItal"))
			 strCorrectedName="CalmariItal";

	else if (strCorrectedName==String_256("SerpentineDBolItal"))
			 strCorrectedName="SerpentineDBol";

	else if (strCorrectedName==String_256("TimesNewRoman"))
			 strCorrectedName="Times New Roman";

	else if (strCorrectedName==String_256("AbbessRegular"))
			 strCorrectedName= "Abbess";

	else if (strCorrectedName==String_256("BalthazarRegular"))
			 strCorrectedName= "Balthazar";

	else if (strCorrectedName==String_256("KlingBoldBold"))
			 strCorrectedName= "KlingBold";

	else if (strCorrectedName==String_256("MargaretRegular"))
			 strCorrectedName= "Margaret";

	else if (strCorrectedName==String_256("PhrasticMediumRegular"))
			 strCorrectedName= "PhrasticMedium";

	else if (strCorrectedName==String_256("TarzanRegular"))
			 strCorrectedName= "Tarzan";

	else if (strCorrectedName==String_256("BorealisRegular"))
			 strCorrectedName= "Borealis";

	else if (strCorrectedName.Sub(String_256("FontBank")) == 0)
			 strCorrectedName.Remove(0, 8);

	else if (strCorrectedName==String_256("GaramondAmericanRegular"))
			 strCorrectedName= "GaramondAmerican";

	//Select everything in the gallery - (bodge!)
	pGallery->SelectItems(TRUE);

	//Get the first selected item
	SGDisplayNode* pThisItem = pTree->FindNextSelectedItem(NULL);		

	static BOOL skipall = FALSE;

	//And while we are looking at an item
	while (pThisItem)
	{
		
		if (pThisItem->IsKindOf(CC_RUNTIME_CLASS(SGTTFItem)))
		{
			//Then cast it into a True Type font pointer
			SGTTFItem* pTTFItem=(SGTTFItem*) pThisItem;
		
			//Now we need to get the cached font description for that
			//item
			LOGFONT* pLogFont=pTTFItem->CachedLogFont;

			if (!pLogFont)
				continue;

			String_256 strDescription=pLogFont->lfFaceName;

			//And if that description is the one we're looking for
			if (_tcsncmp(strDescription, strCorrectedName, strCorrectedName.Length())==0)
			{
				//Return that item
				return pTTFItem;
			}

			if (!skipall)
			{
				// possible match up - does our name of the font start with the font's own description.
				if (strCorrectedName.Sub(strDescription) == 0)
				{
					// we must only match up GaramondAmerican with itself, not with plain Garamond.
					if (strCorrectedName==String_256("GaramondAmerican"))
					{
						if (strDescription==String_256("GaramondAmerican"))
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// is this a bold-italic font?
					else if (_tcsstr(strCorrectedName, "BoldItal") != 0)
					{
						if (_tcsstr(strDescription, "BoldItal") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// is this a heavy-italic font?
					else if (_tcsstr(strCorrectedName, "HeavyItal") != 0)
					{
						if (_tcsstr(strDescription, "HeavyItal") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// is this a heavy font?
					else if (_tcsstr(strCorrectedName, "Heavy") != 0)
					{
						if (_tcsstr(strDescription, "Heavy") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// is this a bold font?
					else if (_tcsstr(strCorrectedName, "Bold") != 0)
					{
						if (_tcsstr(strDescription, "Bold") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// is this an italic font?
					else if (_tcsstr(strCorrectedName, "Ital") != 0)
					{
						if (_tcsstr(strDescription, "Ital") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// is this an engraved font?
					else if (_tcsstr(strCorrectedName, "Engraved") != 0)
					{
						if (_tcsstr(strDescription, "Engraved") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pTTFItem;
						}
					}

					// not italic or engraved.
					else
					{
						TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
						return pTTFItem;
					}
				}
			}
		}

		else if (pThisItem->IsKindOf(CC_RUNTIME_CLASS(SGATMItem)))
		{
			//Then cast it into an ATM font pointer
			SGATMItem* pATMItem=(SGATMItem*) pThisItem;
		
			//Now we need to get the cached font description for that
			//item
			LOGFONT* pLogFont=pATMItem->CachedLogFont;

			if (!pLogFont)
				continue;

			String_256 strDescription=pLogFont->lfFaceName;

			//And if that description is the one we're looking for
			if (_tcsncmp(strDescription, strCorrectedName, strCorrectedName.Length())==0)
			{
				//Return that item
				return pATMItem;
			}

			if (!skipall)
			{
				// possible match up - does our name of the font start with the font's own description.
				if (strCorrectedName.Sub(strDescription) == 0)
				{
					// we must only match up GaramondAmerican with itself, not with plain Garamond.
					if (strCorrectedName==String_256("GaramondAmerican"))
					{
						if (strDescription==String_256("GaramondAmerican"))
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// is this a bold-italic font?
					else if (_tcsstr(strCorrectedName, "BoldItal") != 0)
					{
						if (_tcsstr(strDescription, "BoldItal") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// is this a heavy-italic font?
					else if (_tcsstr(strCorrectedName, "HeavyItal") != 0)
					{
						if (_tcsstr(strDescription, "HeavyItal") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// is this a heavy font?
					else if (_tcsstr(strCorrectedName, "Heavy") != 0)
					{
						if (_tcsstr(strDescription, "Heavy") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// is this a bold font?
					else if (_tcsstr(strCorrectedName, "Bold") != 0)
					{
						if (_tcsstr(strDescription, "Bold") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// is this an italic font?
					else if (_tcsstr(strCorrectedName, "Ital") != 0)
					{
						if (_tcsstr(strDescription, "Ital") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// is this an engraved font?
					else if (_tcsstr(strCorrectedName, "Engraved") != 0)
					{
						if (_tcsstr(strDescription, "Engraved") != 0)
						{
							TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
							return pATMItem;
						}
					}

					// not italic or engraved.
					else
					{
						TRACEALL( _T("Matched %s with %s\n"), (TCHAR*)strCorrectedName, (TCHAR*)strDescription);
						return pATMItem;
					}
				}
			}
		}

		//And move on to the next item
		pThisItem = pTree->FindNextSelectedItem(pThisItem);		

	}

	//We haven't found anything. So return NULL.
	return NULL;
}



/********************************************************************************************

>	OpCreateFontThumbnails::OpCreateFontThumbnails()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	Default constructor

********************************************************************************************/

OpCreateFontThumbnails::OpCreateFontThumbnails()
{
	//
}

OpConvertFileFormats::OpConvertFileFormats()
{
	//
}



/********************************************************************************************

>	OpState OpCreateFontThumbnails::GetState(String_256*, OpDescriptor*)

	Author: 	Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/97
	Purpose:	This item is always available, so long as a document is visible.

********************************************************************************************/

OpState OpCreateFontThumbnails::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	OpSt.Greyed=FALSE; 

	return OpSt;
}

OpState OpConvertFileFormats::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	OpSt.Greyed=FALSE; 

	return OpSt;
}

#endif //DEBUG
