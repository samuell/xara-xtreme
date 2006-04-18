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
// An input filter for importing text files.


#include "camtypes.h"
#include "textfltr.h"

#if BUILD_TEXT_FILTERS

//#include "filtrres.h"

// code headers
#include "app.h"
#include "clipmap.h"
#include "document.h"
#include "errors.h"
#include "fillval.h"
#include "fixmem.h"
#include "fontman.h"
#include "nodetxts.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "oilfltrs.h"
#include "page.h"
#include "pathname.h"
#include "progress.h"
#include "spread.h"
#include "textfuns.h"

// resource headers
//#include "mario.h"
//#include "peter.h"
//#include "tim.h"


DECLARE_SOURCE("$Revision$");


CC_IMPLEMENT_DYNAMIC(BaseTextFilter, TextFilter)


#if BUILD_OTHER_TEXT_FILTER

CC_IMPLEMENT_DYNAMIC(ANSITextFilter, BaseTextFilter)
CC_IMPLEMENT_DYNAMIC(UnicodeTextFilter, ANSITextFilter)
CC_IMPLEMENT_DYNAMIC(RTFTextFilter, BaseTextFilter)

// RTF implementation classes
CC_IMPLEMENT_DYNAMIC(StateStore, ListItem)

CC_IMPLEMENT_DYNAMIC(RTFColourTable, List);
CC_IMPLEMENT_DYNAMIC(RTFColourTableEntry, ListItem);
CC_IMPLEMENT_DYNAMIC(RTFFontTable, List);
CC_IMPLEMENT_DYNAMIC(RTFFontTableEntry, ListItem);

CC_IMPLEMENT_MEMDUMP(RTFControlAction, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(RTFEnterSkipDest, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFNewParaAction, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFColourTableAction, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFFontTableAction, RTFControlAction)

CC_IMPLEMENT_MEMDUMP(RTFPaperWidth, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFPaperHeight, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFLeftMargin, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFRightMargin, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFTopMargin, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFBottomMargin, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFResetCharAttrs, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFResetParaAttrs, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFTableCellAction, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFTableRowAction, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFTabAction, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFSingleCharAction, RTFControlAction)

CC_IMPLEMENT_MEMDUMP(RTFAttributeAction, RTFControlAction)
CC_IMPLEMENT_MEMDUMP(RTFBoldAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFTextSizeAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFTextColourAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFTextFontAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFItalicAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFNoscriptAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFSubscriptAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFSuperscriptAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFTWIPTrackingAttribute, RTFAttributeAction)
CC_IMPLEMENT_MEMDUMP(RTFQPTrackingAttribute, RTFAttributeAction)

CC_IMPLEMENT_MEMDUMP(RTFHexCharAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFEmDashAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFEnDashAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFEmSpaceAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFEnSpaceAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFBulletAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFSingleLeftQuoteAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFSingleRightQuoteAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFDoubleLeftQuoteAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFDoubleRightQuoteAction, RTFSingleCharAction)
CC_IMPLEMENT_MEMDUMP(RTFNonBreakSpaceAction, RTFSingleCharAction)

#endif	//#if BUILD_OTHER_TEXT_FILTER

#define new CAM_DEBUG_NEW



/********************************************************************************************
>	BaseTextFilter::BaseTextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Constructor.  Initialises member variables
********************************************************************************************/
BaseTextFilter::BaseTextFilter()
{
}



/********************************************************************************************
>	BaseTextFilter::~BaseTextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Destructor.  Frees memory and cleans up.
********************************************************************************************/
BaseTextFilter::~BaseTextFilter()
{

}



/********************************************************************************************
>	TextStory* BaseTextFilter::CreateImportStory(DocCoord Pos);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		Pos - the position of the top left of the text story
	Outputs:	-
	Returns:	Pointer to a created TextStory object.  NULL on error
	Purpose:	Creates an empty text story for importing characters into
	SeeAlso:	BaseTextFilter::AddCharToStory
********************************************************************************************/
TextStory* BaseTextFilter::CreateImportStory(DocCoord Pos)
{
	return TextStory::CreateTextObject(Pos);
}



/********************************************************************************************
>	VisibleTextNode* BaseTextFilter::AddCharToStory(TextStory* pStory, WCHAR NewChar);

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		pStory - pointer to the story to add character to
				NewChar - the character (Unicode value)
	Outputs:	-
	Returns:	Pointer to the new character, NULL for error
	Purpose:	Adds the given character to the story.  It is added as the last character on
				the last line.  The filters current attributes are applied to the new
				character.
	SeeAlso:	BaseTextFilter::AddNewLineToStory
********************************************************************************************/
VisibleTextNode* BaseTextFilter::AddCharToStory(TextStory* pStory, WCHAR NewChar)
{
	ERROR2IF(pStory == NULL, NULL, "NULL Story pointer");
	
	// Find the EOLNode on the last line
	VisibleTextNode* pLastNode = pStory->FindLastVTN();
	ERROR2IF(pLastNode == NULL, NULL, "Story was (very) empty");

	// Create a new character, inserting into the story at the same time
	VisibleTextNode* pNew = new TextChar(pLastNode, PREV, NewChar);

	if (pNew != NULL)
	{
		if (!AttributeManager::ApplyBasedOnDefaults(pNew, CurrentAttrs))
		{
	 		delete pNew;
			pNew = NULL;
		}
	}

	return pNew;
}



/********************************************************************************************
>	BOOL BaseTextFilter::AddNewLineToStory(TextStory* pStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		pStory - pointer to the story to add the line to
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Adds a new (empty) line to the end of the story
	SeeAlso:	BaseTextFilter::AddCharToStory
********************************************************************************************/
BOOL BaseTextFilter::AddNewLineToStory(TextStory* pStory)
{
	ERROR2IF(pStory == NULL, NULL, "NULL Story pointer");

	// Create the new line as the last child of the story
	TextLine* pNewLine = new TextLine(pStory, LASTCHILD);

	// Create an EOL node for the new line
	EOLNode* pEOL = NULL;
	if (pNewLine != NULL)
	{
		pEOL = new EOLNode(pNewLine, LASTCHILD);
		if ( (pEOL == NULL) || (!AttributeManager::ApplyBasedOnDefaults(pEOL, CurrentAttrs)) )
		{
			pNewLine->CascadeDelete();
			delete pNewLine;
			pNewLine = NULL;
		}
	}
	
	return ((pNewLine != NULL) && (pEOL != NULL));
}



/********************************************************************************************
>	BOOL BaseTextFilter::AddTabToStory(TextStory* pStory, MILLIPOINT TabLength = 50000)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pStory - pointer to the story to add the tab to
				TabLength - the length of the tab
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Adds a TAB character to the story.  As stories currently don't have tabs a
				kern character of the appropiate size is added instead
	SeeAlso:	BaseTextFilter::AddCharToStory
********************************************************************************************/
BOOL BaseTextFilter::AddTabToStory(TextStory* pStory, MILLIPOINT TabLength)
{
	ERROR2IF(pStory == NULL, FALSE, "NULL Story pointer");

	// Find the EOLNode on the last line
	VisibleTextNode* pLastNode = pStory->FindLastVTN();
	ERROR2IF(pLastNode == NULL, FALSE, "Story was (very) empty");

	// Create a new kern, inserting into the story at the same time
	KernCode* pNewKern = new KernCode(pLastNode, PREV, DocCoord(TabLength, 0));

	if (pNewKern != NULL)
		return AttributeManager::ApplyBasedOnDefaults(pNewKern, CurrentAttrs);
	else
		return FALSE;
}




#if BUILD_OTHER_TEXT_FILTER

/********************************************************************************************
>	ANSITextFilter::ANSITextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Filter constructor - this initialises the default attributes and the
				input buffer.
********************************************************************************************/
ANSITextFilter::ANSITextFilter()
{
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;

	// Set up filter descriptions.
	FilterName.Load(_R(IDT_TEXT_FILTERNAME));
	FilterInfo.Load(_R(IDT_TEXT_FILTERINFO));
	FilterID = FILTERID_TEXT_ASCII;

	BytesReadSoFar = 0;
	EscapeWasPressed = FALSE;
};



/********************************************************************************************
>	ANSITextFilter::~ANSITextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Destructor for the filter
********************************************************************************************/
ANSITextFilter::~ANSITextFilter()
{
}



/********************************************************************************************
>	BOOL ANSITextFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Returns:	TRUE for successful initialisation, FALSE if error occured
	Purpose:	Initialise the filter (attaches a TextOILFilter object).
	SeeAlso:	TextOILFilter
********************************************************************************************/
BOOL ANSITextFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new ANSITextOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// All ok
	return TRUE;
}



/********************************************************************************************
>	virtual BOOL ANSITextFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		pcszPathName - pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded.  This filter always returns TRUE as ANSI text files have no page
				information
	SeeAlso:	Filter::IsDefaultDocRequired
********************************************************************************************/
BOOL ANSITextFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	return TRUE;
}	



/********************************************************************************************
>	INT32 ANSITextFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	Compatibility: 0 => Not an ANSI text file; 10 => It is an ANSI text file
	Purpose:	Analyse a file to see if it contains ANSI text.
	SeeAlso:	TextOILFilter::HowCompatible
********************************************************************************************/
INT32 ANSITextFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (pOILFilter == NULL)
	{
		ERROR3("No oil filter!");
		return 0;
	}
	
	// check the file type against the type for text files.  If it matches only return 9 so the 
	// more specialised text files (eg Unicode) can claim it by returning 10.
	if (pOILFilter->DoesExtensionOfPathNameMatch(&Filename))
		return 9;

	// We can have a stab at loading any file as text as a last resort
	return 1;
}



/********************************************************************************************
>	BOOL ANSITextFilter::PrepareToImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Returns:	TRUE if it succeeds, FALSE if not.
	Purpose:	Sets up the filter so it can attempt to read in a file.
	SeeAlso:	ANSITextFilter::CleanUpAfterImport
********************************************************************************************/
BOOL ANSITextFilter::PrepareToImport()
{
	// Get a default set of attributes for ink objects.
	if (!SetUpCurrentAttrs())
		return FALSE;

	BytesReadSoFar = 0;
	EscapeWasPressed = FALSE;

	return TRUE;
}



/********************************************************************************************
>	void ANSITextFilter::CleanUpAfterImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Delete dynamic objects used in the import process.
	SeeAlso:	ANSITextFilter::PrepareToImport; ANSITextFilter::DoImport
********************************************************************************************/
void ANSITextFilter::CleanUpAfterImport()
{
	DeleteCurrentAttrs();
}



/********************************************************************************************
>	BOOL ANSITextFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
								   Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		Op - pointer to the operation that this read process is associated with.
				pFile - The file that we are trying to import
				DestDoc - pointer to the document to insert the file data into.
				AutoChosen -
				Pos - pointer to the position to insert the text file at
	Returns:	TRUE if the read was successful, FALSE if not.
	Purpose:	Import the named text file into the specified document.
				If the read is successful, the End() function of the 'Op' operation is
				called, otherwise it is not called.
	Errors:		Unable to open file, corrupted document tree found.
********************************************************************************************/
BOOL ANSITextFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
							   Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos)
{
	ERROR2IF((Op == NULL) || (pFile == NULL) || (DestDoc == NULL), FALSE, "NULL Parameter");

	// Let's get ready
	if (!PrepareToImport()) return FALSE;

	// Get spread and import position
	Spread *pSpread = NULL;
	DocCoord ImportPos(0,0);
	if (Pos == NULL)
	{
		pSpread = GetFirstSpread(DestDoc);
		ERROR2IF(pSpread == NULL, FALSE, "No spread in document");
		Page* pPage = pSpread->FindFirstPageInSpread();
		ERROR2IF(pPage == NULL, FALSE, "No page in spread");
		ImportPos = DocCoord(pPage->GetPageRect().lox, pPage->GetPageRect().hiy);
	}
	else
	{
		pSpread = Pos->pSpread;
		ImportPos = Pos->Position;
	}

	// Create a text story object to add the characters into.
	TextStory* pStory = CreateImportStory(ImportPos);
	if (pStory == NULL)
		return FALSE;
	
	//FlagChangedByOp() modified to mModifiedByOp()
	pStory->mModifiedByOp();
	if (!DestDoc->GetAttributeMgr().ApplyCurrentAttribsToNode(pStory))
	{
		pStory->CascadeDelete();
		delete pStory;
		return FALSE;
	}
	
	// Initialise our handling of the file
	pFile->SetReportErrors(FALSE);
	pFile->SetThrowExceptions(FALSE);

	// Call the virtual import function
	BOOL Success = DoInternalImport(pStory, pFile);
	
	// Clean up after importing
	EndSlowJob();
	CleanUpAfterImport();

	// Insert the new sub-tree into the document
	if (!EscapeWasPressed)
	{
		if (Success)
			Success = pStory->FormatAndChildren();
		if (Success)
			Success = Op->DoInsertNewNode(pStory, pSpread, TRUE);
	}
	
	// If the load failed for any reason, delete the subtree we have created
	if (!Success || EscapeWasPressed)
	{
		pStory->CascadeDelete();
		delete pStory;
	}

	return Success;
}



/********************************************************************************************
>	BOOL ANSITextFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		As base class version.
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Exports the document as an ANSI text file.
	SeeAlso:	Filter::DoExport
********************************************************************************************/
BOOL ANSITextFilter::DoExport(Operation*, CCLexFile*, PathName* pPath, Document*)
{
	ERROR3("ANSITextFilter::DoExport is not implemented");
	return FALSE;
}



/********************************************************************************************
>	virtual BOOL ANSITextFilter::DoInternalImport(TextStory* pImportStory, CCLexFile* pFile)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pImportStory - the TextStory to import into
				pFile - the file to read the characters from
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Imports ANSI and multi-byte characters from the file into the text story.
	SeeAlso:	ANSITextFilter::DoImport
********************************************************************************************/
BOOL ANSITextFilter::DoInternalImport(TextStory* pImportStory, CCLexFile* pFile)
{
	ERROR2IF((pImportStory == NULL) || (pFile == NULL), FALSE, "NULL entry parameter");
	UINT32 FileSize = pFile->Size();
		
	// Set the progress indicator for a nice percentage display
	String_64 ImportMessage(_R(IDT_IMPORTMSG_TEXT));
	ImportMessage = GetImportProgressString(pFile, _R(IDT_IMPORTMSG_TEXT));
	BeginSlowJob(FileSize, TRUE, &ImportMessage);

	// Skip over any header that may be on the file
	if (!SkipHeader(pFile))
		return FALSE;

	// Process the file character by character, adding it to the story each time
	WCHAR PrevChar = ' ';
	WCHAR NewChar = ' ' ;
	BOOL ok = TRUE;
	do 
	{
		// Read and process each character
		PrevChar = NewChar;
		NewChar = GetNextCharacter(pFile);

		if (NewChar < ' ')
		{
			switch (NewChar)
			{
				case 0x00:	// Error in reading
					ok = FALSE;
					break;
				case 0x09:	// Tab
					ok = AddTabToStory(pImportStory);
					break;
				case 0x0A:	// NewLine
					if (PrevChar != 0x0D)
						ok = AddNewLineToStory(pImportStory);
					break;
				case 0x0D:	// CR
					if (PrevChar != 0x0A)
						ok = AddNewLineToStory(pImportStory);
					break;
				default:
					;		// Ignore the character
			}
		}
		else
			ok = (AddCharToStory(pImportStory, NewChar) != NULL);

		if (!ContinueSlowJob(BytesReadSoFar))
		{
			EscapeWasPressed = TRUE;
			return TRUE;
		}
	}
	while ((BytesReadSoFar < FileSize) && ok && pFile->good());

	return pFile->good() && ok;
}



/********************************************************************************************
>	WCHAR ANSITextFilter::GetNextCharacter(CCLexFile* pFile)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pImportStory - the TextStory to import into
				pFile - the file to read the characters from
	Returns:	Next character from file.  0 if error
	Purpose:	Gets the next character from the import file
	SeeAlso:	ANSITextFilter::DoInternalImport
********************************************************************************************/
WCHAR ANSITextFilter::GetNextCharacter(CCLexFile* pFile)
{
	ERROR2IF((pOILFilter == NULL), 0, "No Text OIL filter");

	UINT32 BytesRead = 0;
	WCHAR ReadChar = ((ANSITextOILFilter*)pOILFilter)->ReadMultiByteChar(pFile, &BytesRead);
	
	BytesReadSoFar += BytesRead;

	return ReadChar;
}



/********************************************************************************************
>	virtual BOOL ANSITextFilter::SkipHeader(CCLexFile* pFile)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pFile - the file to read the characters from
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Does nothing as ANSI text files have no header
********************************************************************************************/
BOOL ANSITextFilter::SkipHeader(CCLexFile* pFile)
{
	return TRUE;
}








/********************************************************************************************
>	UnicodeTextFilter::UnicodeTextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Filter constructor - this initialises the default attributes and the
				input buffer.
********************************************************************************************/
UnicodeTextFilter::UnicodeTextFilter()
{
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;

	// Set up filter descriptions.
	FilterName.Load(_R(IDT_TEXT_FILTERNAME));
	FilterInfo.Load(_R(IDT_TEXT_FILTERINFO));
	FilterID = FILTERID_TEXT_UNICODE;
};



/********************************************************************************************
>	UnicodeTextFilter::~UnicodeTextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Destructor for the filter - this deallocates the input buffer.
********************************************************************************************/

UnicodeTextFilter::~UnicodeTextFilter()
{
}



/********************************************************************************************
>	BOOL UnicodeTextFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Returns:	TRUE for successful initialisation, FALSE if error occured
	Purpose:	Initialise the filter (attaches an OILFilter object).
	SeeAlso:	UnicodeTextOILFilter
********************************************************************************************/
BOOL UnicodeTextFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new UnicodeTextOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// All ok
	return TRUE;
}



/********************************************************************************************
>	INT32 UnicodeTextFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	Compatibility: 0 => Not a Unicode text file; 10 => It is a Unicode text file
	Purpose:	Analyse a file to see if it contains Unicode text.
	SeeAlso:	ANSITextFilter::HowCompatible
********************************************************************************************/
INT32 UnicodeTextFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (pOILFilter == NULL)
	{
		ERROR3("No oil filter!");
		return 0;
	}
	
	// To load Unicode text the extension must match
	if (pOILFilter->DoesExtensionOfPathNameMatch(&Filename))
	{
		// The first two bytes must be 0xFFFE too
		if ( (*HeaderStart == 0xFF) && (*(HeaderStart+1) == 0xFE) )
			return 10;
	}

	return 0;
}



/********************************************************************************************
>	WCHAR UnicodeTextFilter::GetNextCharacter(CCLexFile* pFile)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pImportStory - the TextStory to import into
				pFile - the file to read the characters from
	Returns:	Next character from file.  0 if error
	Purpose:	Gets the next character from the import file
	SeeAlso:	UnicodeTextFilter::DoInternalImport
********************************************************************************************/
WCHAR UnicodeTextFilter::GetNextCharacter(CCLexFile* pFile)
{
	ERROR2IF((pOILFilter == NULL), 0, "No Text OIL filter");

	BytesReadSoFar += 2;
	return ((UnicodeTextOILFilter*)pOILFilter)->ReadUnicodeChar(pFile);
}



/********************************************************************************************
>	virtual BOOL UnicodeTextFilter::SkipHeader(CCLexFile* pFile)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pFile - the file to read the characters from
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Skips over the header on Unicode files
********************************************************************************************/
BOOL UnicodeTextFilter::SkipHeader(CCLexFile* pFile)
{
	BYTE Dummy[2];

	pFile->read(Dummy, 2);
	BytesReadSoFar += 2;

	return pFile->good();
}



/********************************************************************************************
>	StateStore::StateStore()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/95
	Purpose:	Constructor - initialises member variables
********************************************************************************************/
StateStore::StateStore()
{
	DestinationState = rdsNorm;
	InternalState = risNorm;
	InterCharGap = 0;
}

StateStore::~StateStore() {}
	
StateStore& StateStore::operator=(const StateStore& rhs)
{
	this->DestinationState = rhs.DestinationState;
	this->InternalState = rhs.InternalState;
	this->InterCharGap = rhs.InterCharGap;

	return *this;
}



/********************************************************************************************
>	RTFTextFilter::RTFTextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Filter constructor
********************************************************************************************/
RTFTextFilter::RTFTextFilter()
{
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;

	// Set up filter descriptions.
	FilterName.Load(_R(IDT_RTF_FILTERNAME));
	FilterInfo.Load(_R(IDT_RTF_FILTERINFO));
	FilterID = FILTERID_TEXT_RTF;

	// Init member vars to safe values
	EscapeWasPressed = FALSE;
	pImportStory = NULL;
	BytesReadSoFar = 0;
	ImportHasFinished = FALSE;
};



/********************************************************************************************
>	RTFTextFilter::~RTFTextFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Destructor for the filter
********************************************************************************************/
RTFTextFilter::~RTFTextFilter()
{
}



/********************************************************************************************
>	BOOL RTFTextFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Returns:	TRUE for successful initialisation, FALSE if error occured
	Purpose:	Initialise the filter (attaches a TextOILFilter object).
	SeeAlso:	RTFTextOILFilter
********************************************************************************************/
BOOL RTFTextFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new RTFTextOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	return ((RTFTextOILFilter*)pOILFilter)->RegisterWithClipboard(this);
}



/********************************************************************************************
>	virtual BOOL RTFTextFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded.  This filter always returns TRUE as RTF files have no page
				information
	SeeAlso:	Filter::IsDefaultDocRequired
********************************************************************************************/
BOOL RTFTextFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	return TRUE;
}	



/********************************************************************************************
>	INT32 RTFTextFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	Compatibility: 0 => Not a RTF file; 10 => It is a RTF file
	Purpose:	Analyse a file to see if it contains RTF information
********************************************************************************************/
INT32 RTFTextFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (pOILFilter == NULL)
	{
		ERROR3("No oil filter!");
		return 0;
	}

	// File extension must be RTF
	if (pOILFilter->DoesExtensionOfPathNameMatch(&Filename))
	{
		// File must start "{\rtf"
		if ((*HeaderStart == '{') && (*(HeaderStart+1) == '\\') && (*(HeaderStart+2) == 'r')
					&& (*(HeaderStart+3) == 't') && (*(HeaderStart+4) == 'f') )
			return 10;
	}

	return 0;
}



/********************************************************************************************
>	BOOL RTFTextFilter::PrepareToImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Returns:	TRUE if it succeeds, FALSE if not.
	Purpose:	Sets up the filter so it can attempt to read in a file.
	SeeAlso:	RTFTextFilter::CleanUpAfterImport
********************************************************************************************/
BOOL RTFTextFilter::PrepareToImport()
{
	// Get a default set of attributes for ink objects.
	if (!SetUpCurrentAttrs())
		return FALSE;

	// Set the fill colour to black and the outline to none
	SetFillColour(DocColour(COLOUR_BLACK));
	SetLineColour(DocColour(COLOUR_TRANS));

	BytesReadSoFar = 0;
	pImportStory = NULL;
	EscapeWasPressed = FALSE;
	ImportHasFinished = FALSE;

	// Ensure the various stacks and tables are empty
	if (!StateStack.IsEmpty())
	{
		ERROR3("State stack was not empty when import started");
		StateStack.DeleteAll();
	}
	if (!PreviousAttributes.GetList()->IsEmpty())
	{
		ERROR3("Attribute stack was not empty when import started");
		PreviousAttributes.GetList()->DeleteAll();
	}
	if (!ColourTable.IsEmpty())
	{
		ERROR3("Colour table not empty when import started");
		ColourTable.DeleteAll();
	}
	
	InitialiseInfo();
	cbBin = 0;
	fSkipDestIfUnk = FALSE;

	return TRUE;
}



/********************************************************************************************
>	void RTFTextFilter::CleanUpAfterImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Delete dynamic objects used in the import process.
	SeeAlso:	RTFTextFilter::PrepareToImport; RTFTextFilter::DoImport
********************************************************************************************/
void RTFTextFilter::CleanUpAfterImport()
{
	DeleteCurrentAttrs();

	// Ensure the various stacks and tables are empty and memory freed
	StateStack.DeleteAll();
	PreviousAttributes.GetList()->DeleteAll();
	ColourTable.DeleteAll();
	FontTable.DeleteAll();
}



/********************************************************************************************
>	BOOL RTFTextFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
								 Document *DestDoc, BOOL AutoChosen, ImportPosition *Pos)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/93
	Inputs:		Op - pointer to the operation that this read process is associated with.
				pFile - The file that we are trying to import
				DestDoc - pointer to the document to insert the file data into.
				AutoChosen -
				Pos - pointer to the position to insert the text file at
	Returns:	TRUE if the read was successful, FALSE if not.
	Purpose:	Import the named text file into the specified document.
				If the read is successful, the End() function of the 'Op' operation is
				called, otherwise it is not called.
	Errors:		Unable to open file, corrupted document tree found.
********************************************************************************************/
BOOL RTFTextFilter::DoImport(SelOperation *Op, CCLexFile* pFile, 
							 Document* DestDoc, BOOL AutoChosen, ImportPosition* Pos)
{
	ERROR2IF((Op == NULL) || (pFile == NULL) || (DestDoc == NULL), FALSE, "NULL Parameter");

	if (!PrepareToImport())
		return FALSE;

	// Get spread and import position
	Spread *pSpread = NULL;
	DocCoord ImportPos(0,0);
	if (Pos == NULL)
	{
		pSpread = GetFirstSpread(DestDoc);
		ERROR2IF(pSpread == NULL, FALSE, "No spread in document");
		Page* pPage = pSpread->FindFirstPageInSpread();
		ERROR2IF(pPage == NULL, FALSE, "No page in spread");
		ImportPos = DocCoord(pPage->GetPageRect().lox, pPage->GetPageRect().hiy);
	}
	else
	{
		pSpread = Pos->pSpread;
		ImportPos = Pos->Position;
	}

	// Create a text story object to add the characters into.
	pImportStory = CreateImportStory(ImportPos);
	if (pImportStory == NULL)
		return FALSE;
	
	// Initialise our handling of the file
	pFile->SetReportErrors(FALSE);
	pFile->SetThrowExceptions(FALSE);
	pImportFile = pFile;

	// Call the virtual import function
	BOOL Success = DoInternalImport();
	
	// Clean up after importing
	EndSlowJob();
	CleanUpAfterImport();

	// Insert new sub-tree into the document
	if (!EscapeWasPressed && Success)
	{
		if (Success)
			Success = pImportStory->OptimiseAttributes();		// Optimise up to story level
		if (Success)
			Success = pImportStory->FormatAndChildren();		// Initial format to setup char bounds
		pImportStory->SetImportFormatWidth(PaperWidth - LeftMargin - RightMargin);
		if (Success)
			Success = pImportStory->FormatToWidth();			// split paras to story width
		if (Success)
			Success = pImportStory->FormatAndChildren();		// Format again for new lines
		if (Success)
			Success = Op->DoInsertNewNode(pImportStory, pSpread, TRUE);
	}

	// If the load failed for any reason, delete the subtree we have created
	if (!Success || EscapeWasPressed)
	{
		pImportStory->CascadeDelete();
		delete pImportStory;
	}

	return Success;
}



/********************************************************************************************
>	BOOL RTFTextFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Inputs:		As base class version.
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Exports the document as an ANSI text file.
	SeeAlso:	Filter::DoExport
********************************************************************************************/
BOOL RTFTextFilter::DoExport(Operation*, CCLexFile*, PathName*, Document*)
{
	ERROR3("RTFTextFilter::DoExport is not implemented");
	return FALSE;
}



/********************************************************************************************
>	virtual BOOL RTFTextFilter::DoInternalImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Parses the RTF file for the text and the control parts
	SeeAlso:	RTFTextFilter::DoImport
********************************************************************************************/
BOOL RTFTextFilter::DoInternalImport()
{
	ERROR2IF((pImportStory == NULL) || (pImportFile == NULL), FALSE, "NULL entry parameter");
	UINT32 FileSize = pImportFile->Size();
		
	// Set the progress indicator for a nice percentage display
	String_64 ImportMessage(_R(IDT_IMPORTMSG_TEXT));
	ImportMessage = GetImportProgressString(pImportFile, _R(IDT_IMPORTMSG_TEXT));
	BeginSlowJob(FileSize, TRUE, &ImportMessage);
	BOOL ok = TRUE;

	CurrentState.InternalState = StateStore::risNorm;
	CurrentState.DestinationState = StateStore::rdsNorm;

    TCHAR ReadChar;
    while (pImportFile->good() && (BytesReadSoFar <= FileSize-1) && !ImportHasFinished)
    {
		// Update the percentage, allows users to press escape
		if (!ContinueSlowJob(BytesReadSoFar))
		{
			EscapeWasPressed = TRUE;
			return TRUE;
		}
		
		if (!ReadCharFromFile(&ReadChar))
			return FALSE;

		if (CurrentState.InternalState == StateStore::risBin)	// if we're parsing binary data, handle it directly
		{
			ERROR3("BinaryData not yet handled");
            if (!ParseChar(ReadChar))
               return FALSE;
		}
		else
		{
			switch (ReadChar)
			{
				case '{':
					if (!PushRTFState())
						return FALSE;
					break;
				case '}':
					if (!PopRTFState())
						return FALSE;
					break;
				case '\\':
					if (!ParseRTFKeyword())
						return FALSE;
					break;
				default:
					ERROR2IF(CurrentState.InternalState != StateStore::risNorm, FALSE, "In unknown parsing state");
					if (!ParseChar(ReadChar))
						return FALSE;
					break;
			}       // switch
		}           // else (ris != risBin)
	}               // while

	if (!StateStack.IsEmpty())
		ERROR1(FALSE, _R(IDE_RTF_DUFFFILE));

	// Delete any empty lines at the end of the story
	if (ok)
	{
		VisibleTextNode* pLastNode = pImportStory->FindLastVisibleTextNode();
		BOOL LastLineEmpty = FALSE;
		if (pLastNode != NULL)
			LastLineEmpty = (pLastNode->FindPrevious() == NULL);

		while (LastLineEmpty)
		{
			Node* pParent = pLastNode->FindParent();
			if (pParent != NULL)
			{
				pParent->CascadeDelete();
				delete pParent;

				pLastNode = pImportStory->FindLastVisibleTextNode();
				if (pLastNode != NULL)
					LastLineEmpty = (pLastNode->FindPrevious() == NULL);
			}
			else
				LastLineEmpty = FALSE;
		}
	}

	return pImportFile->good() && ok;
}



/********************************************************************************************
>	BOOL RTFTextFilter::PushRTFState(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Pushes the current state RTF state onto the state stack
	SeeAlso:	RTFTextFilter::PopRTFState
********************************************************************************************/
BOOL RTFTextFilter::PushRTFState(void)
{
	// Create a new object to store the state in
	StateStore* pState = new StateStore;
	if (pState == NULL)
	    return FALSE;
	*pState = CurrentState;
	StateStack.Push(pState);

	// Store the current attribute state
	if (!PreviousAttributes.SaveContext(CurrentAttrs))
		return FALSE;

	// Reset the current state
	CurrentState.InternalState = StateStore::risNorm;

	return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::PopRTFState(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Pops the state off the top of the RTF stack and set the current state to it.
	SeeAlso:	RTFTextFilter::PushRTFState
********************************************************************************************/
BOOL RTFTextFilter::PopRTFState(void)
{
	StateStore* pState = (StateStore*)StateStack.Pop();
	ERROR2IF(pState == NULL, FALSE, "State stack was empty");

	// Are we going to change destination on this state change?
	if (CurrentState.DestinationState != pState->DestinationState)
	{
	    if (!EndGroupAction(CurrentState.DestinationState))
	        return FALSE;
	}

	CurrentState = *pState;

	delete pState;

	// Set current state to stored state
	if (!PreviousAttributes.RestoreContext(CurrentAttrs))
		return FALSE;

	if (StateStack.IsEmpty())
		ImportHasFinished = TRUE;

	return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::ParseRTFKeyword()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Reads and proccesses a RTF control word and its parameter
	SeeAlso:	RTFTextFilter::TranslateKeyword
********************************************************************************************/
BOOL RTFTextFilter::ParseRTFKeyword()
{
	String_256 Keyword = "";
	INT32 Parameter = 0;
	BOOL ParameterUsed = FALSE;

	if (RTFTextFilter::ReadKeyword(&Keyword, &Parameter, &ParameterUsed))
		return TranslateKeyword(&Keyword, Parameter, ParameterUsed);
	else
		return FALSE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::ParseChar(WCHAR ch)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/95
	Inputs:		ch - character
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Process a single character from the RTF stream
	SeeAlso:	RTFTextFilter::ParseRTFKeyword, RTFTextFilter::PrintChar
********************************************************************************************/
BOOL RTFTextFilter::ParseChar(WCHAR ch)
{
    // Have we reached the end of the binary skip?
	if (CurrentState.InternalState == StateStore::risBin && --cbBin <= 0)
		CurrentState.InternalState = StateStore::risNorm;

	switch (CurrentState.DestinationState)
	{
		case StateStore::rdsSkip:
			// Toss this character (away 8-).
			return TRUE;
		case StateStore::rdsNorm:
			// Insert a character into the story
			if (ch >= 32)
			{
//				PATRACE( _T("Adding char %c\n"), ch);
				
				// Before we insert the char we need to setup some attributes that depend on other
				// attributes, eg tracking.

				// Get the current size of an Em
				if (CurrentState.InterCharGap != 0)
				{
					MILLIPOINT Em = 0;
					if (!((RTFTextOILFilter*)pOILFilter)->GetCurrentEmSize(this, &Em))
						return FALSE;
					else
						SetTextTracking((INT32)((CurrentState.InterCharGap/(double)Em)*1000));
				}
				else
					SetTextTracking(0);

				return (AddCharToStory(pImportStory, ch) != NULL);
			}
		default:
			// handle other destinations....
			return TRUE;
	}
}



/********************************************************************************************
>	BOOL RTFTextFilter::TranslateKeyword(StringBase* pKeyword, INT32 param, BOOL fParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/95
	Inputs:		pKeyword - the keyword read from the file
				param - the keywords parameter (0 if no parameter)
				fParam - TRUE if the keyword uses the parameter, FALSE if unused
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to translate a keyword read from the file into an internal object
				that can be acted upon
	SeeAlso:	RTFTextFilter::ParseRTFKeyword
********************************************************************************************/
BOOL RTFTextFilter::TranslateKeyword(StringBase* pKeyword, INT32 param, BOOL fParam)
{
	// Test the keyword against all the known control handlers
	BOOL Success = TRUE;
	// Bold
	if (RTFBoldAttribute::IsThisAction(pKeyword))
		Success = RTFBoldAttribute::CreateAndExecute(param, fParam, this);
	// Italic
	else if(RTFItalicAttribute::IsThisAction(pKeyword))
		Success = RTFItalicAttribute::CreateAndExecute(param, fParam, this);
	// Size
	else if(RTFTextSizeAttribute::IsThisAction(pKeyword))
		Success = RTFTextSizeAttribute::CreateAndExecute(param, fParam, this);
	// Super/Sub script off
	else if(RTFNoscriptAttribute::IsThisAction(pKeyword))
		Success = RTFNoscriptAttribute::CreateAndExecute(this);
	// Superscript
	else if(RTFSuperscriptAttribute::IsThisAction(pKeyword))
		Success = RTFSuperscriptAttribute::CreateAndExecute(this);
	// Subscript
	else if(RTFSubscriptAttribute::IsThisAction(pKeyword))
		Success = RTFSubscriptAttribute::CreateAndExecute(this);
	// Colour
	else if(RTFTextColourAttribute::IsThisAction(pKeyword))
		Success = RTFTextColourAttribute::CreateAndExecute(param, fParam, this);
	// Font
	else if(RTFTextFontAttribute::IsThisAction(pKeyword))
		Success = RTFTextFontAttribute::CreateAndExecute(param, fParam, this);
	// Tracking in twips
	else if(RTFTWIPTrackingAttribute::IsThisAction(pKeyword))
		Success = RTFTWIPTrackingAttribute::CreateAndExecute(param, fParam, this);
	// Tracking in quarter points
	else if(RTFQPTrackingAttribute::IsThisAction(pKeyword))
		Success = RTFQPTrackingAttribute::CreateAndExecute(param, fParam, this);
	
	// New line
	else if(RTFNewParaAction::IsThisAction(pKeyword))
		Success = RTFNewParaAction::CreateAndExecute(this);
	// Tab
	else if(RTFTabAction::IsThisAction(pKeyword))
		Success = RTFTabAction::CreateAndExecute(this);
	// Start skipping text
	else if(RTFEnterSkipDest::IsThisAction(pKeyword))
		Success = RTFEnterSkipDest::CreateAndExecute(this);

	// Single character definied as two hex digits
	else if(RTFHexCharAction::IsThisAction(pKeyword))
		Success = RTFHexCharAction::CreateAndExecute(this);
	// An Em dash
	else if(RTFEmDashAction::IsThisAction(pKeyword))
		Success = RTFEmDashAction::CreateAndExecute(this);
	// An en dash
	else if(RTFEnDashAction::IsThisAction(pKeyword))
		Success = RTFEnDashAction::CreateAndExecute(this);
	// An em space
	else if(RTFEmSpaceAction::IsThisAction(pKeyword))
		Success = RTFEmSpaceAction::CreateAndExecute(this);
	// An en space
	else if(RTFEnSpaceAction::IsThisAction(pKeyword))
		Success = RTFEnSpaceAction::CreateAndExecute(this);
	// A bullet symbol
	else if(RTFBulletAction::IsThisAction(pKeyword))
		Success = RTFBulletAction::CreateAndExecute(this);
	// Single left sexed quote
	else if(RTFSingleLeftQuoteAction::IsThisAction(pKeyword))
		Success = RTFSingleLeftQuoteAction::CreateAndExecute(this);
	// Single right sexed quote
	else if(RTFSingleRightQuoteAction::IsThisAction(pKeyword))
		Success = RTFSingleRightQuoteAction::CreateAndExecute(this);
	// Double left sexed quote
	else if(RTFDoubleLeftQuoteAction::IsThisAction(pKeyword))
		Success = RTFDoubleLeftQuoteAction::CreateAndExecute(this);
	// Double right sexed quote
	else if(RTFDoubleRightQuoteAction::IsThisAction(pKeyword))
		Success = RTFDoubleRightQuoteAction::CreateAndExecute(this);
	// Non-breaking space
	else if(RTFNonBreakSpaceAction::IsThisAction(pKeyword))
		Success = RTFNonBreakSpaceAction::CreateAndExecute(this);

	// New cell in table
	else if(RTFTableCellAction::IsThisAction(pKeyword))
		Success = RTFTableCellAction::CreateAndExecute(this);
	// New row in table
	else if(RTFTableRowAction::IsThisAction(pKeyword))
		Success = RTFTableRowAction::CreateAndExecute(this);
	// Reset character attributes
	else if(RTFResetCharAttrs::IsThisAction(pKeyword))
		Success = RTFResetCharAttrs::CreateAndExecute(this);
	// Reset paragraph attributes
	else if(RTFResetParaAttrs::IsThisAction(pKeyword))
		Success = RTFResetParaAttrs::CreateAndExecute(this);
	// Colour table
	else if(RTFColourTableAction::IsThisAction(pKeyword))
		Success = RTFColourTableAction::CreateAndExecute(this);
	// Font table
	else if(RTFFontTableAction::IsThisAction(pKeyword))
		Success = RTFFontTableAction::CreateAndExecute(this);

	// Paper width
	else if(RTFPaperWidth::IsThisAction(pKeyword))
	{
		ERROR1IF(!fParam, FALSE, _R(IDE_RTF_DUFFFILE));
		Success = RTFPaperWidth::CreateAndExecute(this, param);
	}
	// Paper height
	else if(RTFPaperHeight::IsThisAction(pKeyword))
	{
		ERROR1IF(!fParam, FALSE, _R(IDE_RTF_DUFFFILE));
		Success = RTFPaperHeight::CreateAndExecute(this, param);
	}
	// Left margin
	else if(RTFLeftMargin::IsThisAction(pKeyword))
	{
		ERROR1IF(!fParam, FALSE, _R(IDE_RTF_DUFFFILE));
		Success = RTFLeftMargin::CreateAndExecute(this, param);
	}
	// Right margin
	else if(RTFRightMargin::IsThisAction(pKeyword))
	{
		ERROR1IF(!fParam, FALSE, _R(IDE_RTF_DUFFFILE));
		Success = RTFRightMargin::CreateAndExecute(this, param);
	}
	// Top margin
	else if(RTFTopMargin::IsThisAction(pKeyword))
	{
		ERROR1IF(!fParam, FALSE, _R(IDE_RTF_DUFFFILE));
		Success = RTFTopMargin::CreateAndExecute(this, param);
	}
	// Bottom margin
	else if(RTFBottomMargin::IsThisAction(pKeyword))
	{
		ERROR1IF(!fParam, FALSE, _R(IDE_RTF_DUFFFILE));
		Success = RTFBottomMargin::CreateAndExecute(this, param);
	}
	else
	{
		// if this is a new destination skip the destination, else just discard it
		if (fSkipDestIfUnk)				
			CurrentState.DestinationState = StateStore::rdsSkip;	
		fSkipDestIfUnk = FALSE;
	}

	return Success;
}



/********************************************************************************************
>	BOOL RTFTextFilter::EndGroupAction(StateStore::DestinationStateTypes rds)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/11/95
	Inputs:		rds - the current destination
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called at the end of a group to allow cleanup
	SeeAlso:	-
********************************************************************************************/
BOOL RTFTextFilter::EndGroupAction(StateStore::DestinationStateTypes rds)
{
    return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::SetSuperscript()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Set the filters current attributes to have superscript on
	SeeAlso:	RTFTextFilter::SetSubscript()
********************************************************************************************/
BOOL RTFTextFilter::SetSuperscript()
{
	return SetTextScript((INT32)(Text_SuperScriptOffset*GetTextSize()), (INT32)(Text_SuperScriptSize*GetTextSize()));
}


/********************************************************************************************
>	BOOL RTFTextFilter::SetSubscript()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Set the filters current attributes to have subscript on
	SeeAlso:	RTFTextFilter::SetSuperscript()
********************************************************************************************/
BOOL RTFTextFilter::SetSubscript()
{
	return SetTextScript((INT32)(Text_SubScriptOffset*GetTextSize()), (INT32)(Text_SubScriptSize*GetTextSize()));
}



/********************************************************************************************
>	BOOL RTFTextFilter::ReadKeyword(StringBase* pKeyword, INT32* pParam, BOOL* pParamUsed)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Inputs:		pKeyword - return pointer for the keyword string
				pParam - return pointer for the parameter
				pParamUsed - return pointer for the flag saying wether the parameter is used.
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Reads an RTF keyword and its optional parameter
	SeeAlso:	-
********************************************************************************************/
BOOL RTFTextFilter::ReadKeyword(StringBase* pKeyword, INT32* pParam, BOOL* pParamUsed)
{
	TCHAR ch = 0;
	BOOL fNegative = FALSE;
	*pParam = 0;
	*pParamUsed = FALSE;
	*pKeyword = "";
	String_256 Parameter = "";

	// Read the first character of the control word
	if (!ReadCharFromFile(&ch))
		return FALSE;

	// If it's a control symbol there is no delimiter, its just one character
	if (!camIsalpha(ch))
	{
		*pKeyword = ch;
//		PATRACE( _T("Control symbol \\%s\n"), (TCHAR*)(*pKeyword));
		return TRUE;
	}

	// Read in the rest of the control word
	while (camIsalpha(ch))
	{
		*pKeyword += ch;
		if (!ReadCharFromFile(&ch))
			return FALSE;
	}
//	PATRACE( _T("Control word \\%s\n"), (TCHAR*)(*pKeyword));

    // If the delimeter was '-' then the following parameter is negative
    if (ch == '-')
    {
        fNegative = TRUE;
		if (!ReadCharFromFile(&ch))
			return FALSE;
    }

    // Read the numeric parameter (if there is one)
	if (camIsdigit(ch))
	{
		*pParamUsed = TRUE;
		while (camIsdigit(ch))
		{
			Parameter += ch;
			if (!ReadCharFromFile(&ch))
				return FALSE;
		}

		TCHAR* pChar = (TCHAR*)Parameter;
		*pParam = camStrtol(pChar);		// BODGE - use string class conversions
		if (fNegative)
			*pParam = -*pParam;
//		PATRACE( _T("Parameter %d\n"), *pParam);
	}

	// If the delimeter was non-space then this character is part of the following text!
	if (ch != ' ')
		return SkipBackChar();
	else
		return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::ReadColourTable()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Reads the RTF colour table, storing it within the filter for future use
	SeeAlso:	-
********************************************************************************************/
BOOL RTFTextFilter::ReadColourTable()
{
	// Ensure the colour table is empty before we start
	if (!ColourTable.IsEmpty())
	{
		ERROR3("Color table was not empty");
		ColourTable.DeleteAll();
	}

	TCHAR CurrentChar=0;	//TCHAR CurrentChar = 0;- adapted for DBCS
	if (!ReadCharFromFile(&CurrentChar))
		return FALSE;

	String_256 Keyword = "";
	INT32 Parameter = 0;
	BOOL ParamUsed = FALSE;
	while (CurrentChar != '}')
	{
		// Create a new entry for the colour table
		RTFColourTableEntry* pNew = new RTFColourTableEntry;
		if (pNew == NULL)
			return FALSE;

 		if (CurrentChar == _T(';'))	//if (CurrentChar == ';')-adapted for DBCS
		{
			// Default colour required				
			pNew->Colour = DocColour(COLOUR_BLACK);
		}
		else
		{
			BOOL TableError = FALSE;
			if (CurrentChar == _T('\\'))	//if (CurrentChar ==';')-adapted for DBCS
			{
				// read colour definition
				INT32 Red = 0;
				INT32 Blue = 0;
				INT32 Green = 0;

				// read Red, Green and Blue values (will be in that order).
				if (!ReadKeyword(&Keyword, &Parameter, &ParamUsed))
					return FALSE;
				if ((Keyword == String_8("red")) && ParamUsed)
					Red = Parameter;
				else
					TableError = TRUE;
				// Read slash at start of next keyword
				if (!ReadCharFromFile(&CurrentChar) || (CurrentChar != _T('\\')))
					TableError = TRUE;

				if (!TableError)
				{
					if (!ReadKeyword(&Keyword, &Parameter, &ParamUsed))
						return FALSE;
					if ((Keyword == String_8("green")) && ParamUsed)
						Green = Parameter;
					else
						TableError = TRUE;
					if (!ReadCharFromFile(&CurrentChar) || (CurrentChar != _T('\\')))
						TableError = TRUE;
				}

				if (!TableError)
				{
					if (!ReadKeyword(&Keyword, &Parameter, &ParamUsed))
						return FALSE;
					if ((Keyword == String_8("blue")) && ParamUsed)
						Blue = Parameter;
					else
						TableError = TRUE;
					if (!ReadCharFromFile(&CurrentChar) || (CurrentChar != _T(';')))
						TableError = TRUE;
				}

				pNew->Colour = DocColour(Red, Green, Blue);
			}
			else
				TableError = TRUE;

			ERROR3IF(TableError, "Error in colour table");
			ERROR1IF(TableError, FALSE, _R(IDE_RTF_DUFFFILE));
		}

		// Add the colour to the table
		ColourTable.AddTail(pNew);

		// Read in the next char
		if (!ReadCharFromFile(&CurrentChar))
			return FALSE;
	}

	// Put the '}' back into the input stream
	return SkipBackChar();
}



/*********************************************************************************************
>	BOOL RTFTextFilter::ReadCharFromFile(TCHAR* pCh, BOOL SkipCR = TRUE)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Inputs:		pCh - return pointer
				SkipCR - TRUE if CRs and LFs should be skipped over
	Outputs:	pCh contains the read character
	Returns:	TRUE id character read ok, FALSE if file error occured
	Purpose:	Reads a character from the input file and increments the bytes read counter.
**********************************************************************************************/  
BOOL RTFTextFilter::ReadCharFromFile(TCHAR* pCh, BOOL SkipCR)
{
	do
	{
		pImportFile->read(pCh);
		BytesReadSoFar++;

		if (pImportFile->bad())
		{
			Error::SetError(_R(IDE_FILE_READ_ERROR));
			return FALSE;
		}
	} while (SkipCR && ((*pCh==13) || (*pCh==10)) );

	return TRUE;
}



/*********************************************************************************************
>	BOOL RTFTextFilter::SkipBackChar(INT32 Count = 1)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Inputs:		Count - number of bytes to skip back
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Skips the import files position back a number of characters
**********************************************************************************************/  
BOOL RTFTextFilter::SkipBackChar(INT32 Count)
{
	FilePos NewPos = pImportFile->tellIn() - Count;
	pImportFile->seekIn(NewPos);
	BytesReadSoFar -= Count;

	return pImportFile->good();
}



/********************************************************************************************
>	BOOL RTFTextFilter::ReadFontTable()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Reads the RTF font table, storing it within the filter for future use
	SeeAlso:	RTFFilter::ReadOneFontFromTable
********************************************************************************************/
BOOL RTFTextFilter::ReadFontTable()
{
	// Ensure the font table is empty before we start
	if (!FontTable.IsEmpty())
	{
		ERROR3("Font table was not empty");
		FontTable.DeleteAll();
	}

	TCHAR CurrentChar = 0;
	if (!ReadCharFromFile(&CurrentChar))
		return FALSE;

	if (CurrentChar == _T('\\'))
	{
		// one entry in the font table
		// BODGE - (just to get your attention - Test one item font tables!)
		if (!ReadOneFontFromTable())
			return FALSE;
	}
	else
	{
		if (CurrentChar != _T('{'))
		{
			ERROR3("Error in font table");
			ERROR1(FALSE, _R(IDE_RTF_DUFFFILE));
		}

		// multiple entries in font table
		while (CurrentChar != _T('}'))
		{
			if (CurrentChar == _T('{'))
			{
				if (!ReadCharFromFile(&CurrentChar))
					return FALSE;
			}
			
			if (!ReadOneFontFromTable())
				return FALSE;

			// now eat whitespace until we hit either '}' (end of font group) or '{' (another font item)
			do {
				if (!ReadCharFromFile(&CurrentChar))
					return FALSE;
			} while ( (CurrentChar != _T('}')) && (CurrentChar != _T('{')) );
		}
	}

	// Put the close group symbol back into the input stream
	return SkipBackChar();
}



/********************************************************************************************
>	BOOL RTFTextFilter::ReadOneFontFromTable()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Reads one font item from the font table in the RTF font table.  When called
				the file must be at the f of the 'f' (fontnum) keyword
	SeeAlso:	RTFFilter::ReadFontTable
********************************************************************************************/
BOOL RTFTextFilter::ReadOneFontFromTable()
{
	// Optional data that may not be present in the file
	INT32 CharacterSet = -1;
	INT32 CodePage = -1;

	String_256 Keyword = "";
	TCHAR ReadChar;
	INT32 Parameter = 0;
	BOOL ParamUsed = FALSE;

	// Read the font index (must be specified)
	if (!ReadKeyword(&Keyword, &Parameter, &ParamUsed))
		return FALSE;
	if ((Keyword != String_8("f")) || !ParamUsed)
		ERROR1(FALSE, _R(IDE_RTF_DUFFFILE));
	INT32 FontIndex = Parameter;

	// Read the font family (must be specified)
	RTFFontTableEntry::FontFamilyType FontFamily = RTFFontTableEntry::FamNil;
	if (!ReadCharFromFile(&ReadChar))
		return FALSE;
	if (ReadChar != _T('\\'))
		ERROR1(FALSE, _R(IDE_RTF_DUFFFILE));
	if (!ReadKeyword(&Keyword, &Parameter, &ParamUsed))
		return FALSE;
	if (Keyword == String_8("fnil"))
		FontFamily = RTFFontTableEntry::FamNil;
	else if (Keyword == String_16("froman"))
		FontFamily = RTFFontTableEntry::FamRoman;
	else if (Keyword == String_16("fswiss"))
		FontFamily = RTFFontTableEntry::FamSwiss;
	else if (Keyword == String_16("fmodern"))
		FontFamily = RTFFontTableEntry::FamModern;
	else if (Keyword == String_16("fscript"))
		FontFamily = RTFFontTableEntry::FamScript;
	else if (Keyword == String_16("fdecor"))
		FontFamily = RTFFontTableEntry::FamDecor;
	else if (Keyword == String_16("ftech"))
		FontFamily = RTFFontTableEntry::FamTech;
	else if (Keyword == String_16("fbidi"))
		FontFamily = RTFFontTableEntry::FamBiDi;
	else
	{
		ERROR3("Unknown font family type");
		FontFamily = RTFFontTableEntry::FamNil;
	}

	// Now (possibly) comes some optional keyword before the fontname
	if (!ReadCharFromFile(&ReadChar))
		return FALSE;
	while (ReadChar == _T('\\'))
	{
		if (!ReadKeyword(&Keyword, &Parameter, &ParamUsed))
			return FALSE;

		if (Keyword == String_16("fcharset"))
		{
			if (ParamUsed)
				CharacterSet = Parameter;
			else
				ERROR1(FALSE, _R(IDE_RTF_DUFFFILE));
		}
		else if (Keyword == String_16("cpg"))
		{
			if (ParamUsed)
				CodePage = Parameter;
			else
				ERROR1(FALSE, _R(IDE_RTF_DUFFFILE));
		}

		if (!ReadCharFromFile(&ReadChar))
			return FALSE;
	}

	// Now comes the font name, terminated by either a close brace or a slash or a semi-colon
	String_64 FontName = "";
	while ( (ReadChar != _T('}')) && (ReadChar != _T('\\')) && (ReadChar != _T(';')) )
	{
		FontName += ReadChar;
		if (!ReadCharFromFile(&ReadChar))
			return FALSE;
	}

	// Munch the remaining control words down to the close brace
	while (ReadChar != '}')
	{
		if (!ReadCharFromFile(&ReadChar))
			return FALSE;
	}

	// Create the font entry and put it into the font table
	RTFFontTableEntry* pNewFont = new RTFFontTableEntry;
	if (pNewFont == NULL)
		return FALSE;

	pNewFont->RealFontName = FontName;
	pNewFont->ApplyFontName = FontName;
	pNewFont->FontFamily = FontFamily;
	pNewFont->FontIndex = FontIndex; 
	if (CharacterSet != -1)
		pNewFont->CharacterSet = CharacterSet;
	if (CodePage != -1)
		pNewFont->CodePage = CodePage;

	FontTable.AddTail(pNewFont);

	return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::InitialiseInfo()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Initialises the RTF document values to their defaults
	SeeAlso:	-
********************************************************************************************/
BOOL RTFTextFilter::InitialiseInfo()
{
	PaperWidth = 12240*50;
	PaperHeight = 15840*50;
	LeftMargin = 1800*50;
	RightMargin = 1800*50;
	TopMargin = 1440*50;
	BottomMargin =1440*50;

	CurrentState.InterCharGap = 0;

	return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextFilter::ResetCharacterAttributes()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Resets all the character attributes back to their defaults
	SeeAlso:	-
********************************************************************************************/
BOOL RTFTextFilter::ResetCharacterAttributes()
{
	BOOL ok = TRUE;
	
	if (ok)
		ok = SetTextBold(FALSE);
	if (ok)
		ok = SetTextItalic(FALSE);
	if (ok)
		ok = SetTextSize(12000);
	if (ok)
		ok = SetTextColour(DocColour(COLOUR_BLACK));
	if (ok)
		ok = SetFont(&DefaultAttrs.FontName);

	return ok;
}



/********************************************************************************************
>	BOOL RTFTextFilter::ResetParagraphAttributes()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Resets all the paragraph attributes back to their defaults
	SeeAlso:	-
********************************************************************************************/
BOOL RTFTextFilter::ResetParagraphAttributes()
{
	return TRUE;
}






/*********************************************************************************************
>	DocColour RTFColourTable::GetColourAt(INT32 TableIndex)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/95
	Inputs:		TableIndex - the index into the RTF filter's colour table
	Outputs:	-
	Returns:	A DocColour describing the colour at that position in the colour table
	Purpose:	Gets a colour from the colour table.  If the index into the table is not valid
				then the colour black is returned.  (It's not an error)
**********************************************************************************************/  
DocColour RTFColourTable::GetColourAt(INT32 TableIndex)
{
	RTFColourTableEntry* pItem = (RTFColourTableEntry*)FindItem(TableIndex);

	if (pItem == NULL)
		return DocColour(COLOUR_BLACK);
	else
		return pItem->Colour;
}



/*********************************************************************************************
>	BOOL RTFFontTable::GetIndexedFont(INT32 TableIndex, String_64* pFontName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Inputs:		TableIndex - the index into the RTF filter's font table
				pFontName - return pointer for the name of the font at that index.
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets a font from the font table.  If the index into the table is not valid
				then a default font is returned
**********************************************************************************************/  
BOOL RTFFontTable::GetIndexedFont(INT32 TableIndex, String_64* pFontName)
{
	ERROR2IF(pFontName == NULL, FALSE, "Null entry param");

	// We have to look through all the items in the table to find one with a matching index
	RTFFontTableEntry* pItem = (RTFFontTableEntry*)GetHead();

	while (pItem != NULL)
	{
		if (pItem->FontIndex == TableIndex)
		{
			*pFontName = pItem->ApplyFontName;
			return TRUE;
		}

		pItem = (RTFFontTableEntry*)GetNext(pItem);
	}

	return TRUE;
}



/*********************************************************************************************
>	RTFFontTableEntry::RTFFontTableEntry()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Purpose:	Constructor - initialises member variables to sensible values
**********************************************************************************************/  
RTFFontTableEntry::RTFFontTableEntry()
{
	RealFontName = "Times New Roman";		// BODGE - get default from font manager
	ApplyFontName = RealFontName;
	FontFamily = FamNil;
	CharacterSet = 0;
	CodePage = 819;
	FontIndex = 0;
}


/*********************************************************************************************
>	RTFFontTableEntry::~RTFFontTableEntry()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Purpose:	Destructor
**********************************************************************************************/  
RTFFontTableEntry::~RTFFontTableEntry()
{
}






/*********************************************************************************************
>	RTFControlAction::RTFControlAction(INT32 Param, RTFTextFilter* pFilt)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Base constructor of all RTF control handlers
**********************************************************************************************/  
RTFControlAction::RTFControlAction(RTFTextFilter* pFilt, INT32 Param)
{
	Parameter = Param;
	pFilter = pFilt;
}



/*********************************************************************************************
>	BOOL RTFControlAction::ExecuteControlCarefully(RTFControlAction* pAction)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Inputs:		pAction - pointer to the created action (may be NULL)
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Common code for executing a newly created control handler.
**********************************************************************************************/  
BOOL RTFControlAction::ExecuteControlCarefully(RTFControlAction* pAction)
{
	if (pAction != NULL)
	{
		BOOL Success = pAction->ExecuteControl();
		delete pAction;
		return Success;
	}
	else
		return FALSE;		// Error will have already been set!
}


/*********************************************************************************************
>	RTFAttributeAction::RTFAttributeAction(RTFTextFilter* pFilt, INT32 Param) : RTFControlAction(pFilt, Param)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Base constructor of all RTF attribute control handlers
**********************************************************************************************/  
RTFAttributeAction::RTFAttributeAction(RTFTextFilter* pFilt, INT32 Param) : RTFControlAction(pFilt, Param)
{
}




/*********************************************************************************************
>	RTFBoldAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the bold character property in importing RTF
				Parameter is 0 to turn bold off, otherwise turn it on
**********************************************************************************************/  
RTFBoldAttribute::RTFBoldAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFBoldAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("b"));
}

BOOL RTFBoldAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->SetBold(Parameter);
}

BOOL RTFBoldAttribute::CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFBoldAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFBoldAttribute(pFilter, Param);
	else
		pNew = new RTFBoldAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFItalicAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the italic character property in importing RTF
				Parameter is 0 to turn italic off, otherwise turn it on
**********************************************************************************************/  
RTFItalicAttribute::RTFItalicAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFItalicAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("i"));
}

BOOL RTFItalicAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->SetItalic(Parameter);
}

BOOL RTFItalicAttribute::CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFItalicAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFItalicAttribute(pFilter, Param);
	else
		pNew = new RTFItalicAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFTextSizeAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the text size character property in importing RTF.
				Parameter if the new text size (defined in half-point units)
**********************************************************************************************/  
RTFTextSizeAttribute::RTFTextSizeAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFTextSizeAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("fs"));
}

BOOL RTFTextSizeAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");

	return pFilter->SetSize(Parameter);
}

BOOL RTFTextSizeAttribute::CreateAndExecute(TWIP Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFTextSizeAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFTextSizeAttribute(pFilter, Param*500);
	else
		pNew = new RTFTextSizeAttribute(pFilter, 24*500);		// 12 point default

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFSubscriptAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the subscript character property in importing RTF.
				No parameter
**********************************************************************************************/  
RTFSubscriptAttribute::RTFSubscriptAttribute(RTFTextFilter* pFilt) : RTFAttributeAction(pFilt, 0)
{
}

BOOL RTFSubscriptAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("sub"));
}

BOOL RTFSubscriptAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->SetSubscript();
}

BOOL RTFSubscriptAttribute::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFSubscriptAttribute* pNew = NULL;

	pNew = new RTFSubscriptAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFSuperscriptAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the superscript character property in importing RTF.
				No parameter
**********************************************************************************************/  
RTFSuperscriptAttribute::RTFSuperscriptAttribute(RTFTextFilter* pFilt) : RTFAttributeAction(pFilt, 0)
{
}

BOOL RTFSuperscriptAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("super"));
}

BOOL RTFSuperscriptAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->SetSuperscript();
}

BOOL RTFSuperscriptAttribute::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFSuperscriptAttribute* pNew = NULL;
	
	pNew = new RTFSuperscriptAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFSuperscriptAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the no superscript/subscript character property in importing RTF.
				No parameter
**********************************************************************************************/  
RTFNoscriptAttribute::RTFNoscriptAttribute(RTFTextFilter* pFilt) : RTFAttributeAction(pFilt, 0)
{
}

BOOL RTFNoscriptAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_16("Nosupersub"));
}

BOOL RTFNoscriptAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->SetNoscript();
}

BOOL RTFNoscriptAttribute::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFNoscriptAttribute* pNew = NULL;
	
	pNew = new RTFNoscriptAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFTextColourAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the foreground colour character property in importing RTF.
				Parameter is the index into the colour tale of the new text colour
**********************************************************************************************/  
RTFTextColourAttribute::RTFTextColourAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFTextColourAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("cf"));
}

BOOL RTFTextColourAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");

	DocColour TableColour = pFilter->GetColourTable()->GetColourAt(Parameter);
	return pFilter->SetTextColour(TableColour);
}

BOOL RTFTextColourAttribute::CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFTextColourAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFTextColourAttribute(pFilter, Param);
	else
		pNew = new RTFTextColourAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFTextFontAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Purpose:	Deals with the font character property in importing RTF.
				Parameter is the index into the font tale of the new font
**********************************************************************************************/  
RTFTextFontAttribute::RTFTextFontAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFTextFontAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("f"));
}

BOOL RTFTextFontAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");

	String_64 FontName;
	if (!pFilter->GetFontTable()->GetIndexedFont(Parameter, &FontName))
		return FALSE;
	else
	{
		if ((FONTMANAGER->CacheNamedFont(&FontName, FC_TRUETYPE) != ILLEGALFHANDLE))
			return pFilter->SetFont(&FontName);
		else
			return FALSE;
	}
}

BOOL RTFTextFontAttribute::CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFTextFontAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFTextFontAttribute(pFilter, Param);
	else
		pNew = new RTFTextFontAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFQPTrackingAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the old-style tracking character property in importing RTF.
				Parameter is tracking size in quarter-point units
**********************************************************************************************/  
RTFQPTrackingAttribute::RTFQPTrackingAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFQPTrackingAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("expnd"));
}

BOOL RTFQPTrackingAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetInterCharGap(Parameter);
	return TRUE;
}

BOOL RTFQPTrackingAttribute::CreateAndExecute(MILLIPOINT Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFQPTrackingAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFQPTrackingAttribute(pFilter, Param*250);		// Convert 1/4 point to millipoint
	else
		pNew = new RTFQPTrackingAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFTWIPTrackingAttribute

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the new-style tracking character property in importing RTF.
				Parameter is tracking size in twips
**********************************************************************************************/  
RTFTWIPTrackingAttribute::RTFTWIPTrackingAttribute(RTFTextFilter* pFilt, INT32 Param) : RTFAttributeAction(pFilt, Param)
{
}

BOOL RTFTWIPTrackingAttribute::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("expndtw"));
}

BOOL RTFTWIPTrackingAttribute::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetInterCharGap(Parameter);
	return TRUE;
}

BOOL RTFTWIPTrackingAttribute::CreateAndExecute(TWIP Param, BOOL UseParam, RTFTextFilter* pFilter)
{
	RTFTWIPTrackingAttribute* pNew = NULL;
	
	if (UseParam)
		pNew = new RTFTWIPTrackingAttribute(pFilter, Param*TwipToMillpoint);
	else
		pNew = new RTFTWIPTrackingAttribute(pFilter);

	return ExecuteControlCarefully(pNew);
}






/*********************************************************************************************
>	RTFEnterSkipDest

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals skipping text following unknown control words
				No parameter
**********************************************************************************************/  
RTFEnterSkipDest::RTFEnterSkipDest(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFEnterSkipDest::IsThisAction(StringBase* pControl)
{
	return ((*pControl == String_16("author"))		||
			(*pControl == String_16("buptim"))		||
			(*pControl == String_16("comment"))		||
			(*pControl == String_16("creatim"))		||
			(*pControl == String_16("doccomm"))		||
			(*pControl == String_16("footer"))		||
			(*pControl == String_16("footerf"))		||
			(*pControl == String_16("footerl"))		||
			(*pControl == String_16("footerr"))		||
			(*pControl == String_16("footnote"))	||
			(*pControl == String_16("ftncn"))		||
			(*pControl == String_16("ftnsep"))		||
			(*pControl == String_16("ftnsepc"))		||
			(*pControl == String_16("header"))		||
			(*pControl == String_16("headerf"))		||
			(*pControl == String_16("headerl"))		||
			(*pControl == String_16("headerr"))		||
			(*pControl == String_16("info"))		||
			(*pControl == String_16("keywords"))	||
			(*pControl == String_16("operator"))	||
			(*pControl == String_16("pict"))		||
			(*pControl == String_16("printim"))		||
			(*pControl == String_16("private1"))	||
			(*pControl == String_16("revtim"))		||
			(*pControl == String_16("rxe"))			||
			(*pControl == String_16("stylesheet"))	||
			(*pControl == String_16("subject"))		||
			(*pControl == String_16("tc"))			||
			(*pControl == String_16("title"))		||
			(*pControl == String_16("txe"))			||
			(*pControl == String_16("*"))			||
			(*pControl == String_16("xe")) );
}

BOOL RTFEnterSkipDest::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetSkipping();
	return TRUE;
}

BOOL RTFEnterSkipDest::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFEnterSkipDest* pNew = new RTFEnterSkipDest(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFNewParaAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with the text in the RTF file starting a new paragraph
				No parameter
**********************************************************************************************/  
RTFNewParaAction ::RTFNewParaAction(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFNewParaAction::IsThisAction(StringBase* pControl)
{
	return (*pControl == String_8("par")) || (*pControl == String_8("sect"));
}

BOOL RTFNewParaAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->StartNewLine();
}

BOOL RTFNewParaAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFNewParaAction* pNew = new RTFNewParaAction(pFilter);

	return ExecuteControlCarefully(pNew);
}



/*********************************************************************************************
>	RTFColourTableAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with reading the colour table from a RTF file
				No parameter
**********************************************************************************************/  
RTFColourTableAction::RTFColourTableAction(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFColourTableAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("colortbl");
}

BOOL RTFColourTableAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->ReadColourTable();
}

BOOL RTFColourTableAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFColourTableAction* pNew = new RTFColourTableAction(pFilter);

	return ExecuteControlCarefully(pNew);
}


/*********************************************************************************************
>	RTFFontTableAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Deals with reading the font table from a RTF file
				No parameter
**********************************************************************************************/  
RTFFontTableAction::RTFFontTableAction(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFFontTableAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("fonttbl");
}

BOOL RTFFontTableAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->ReadFontTable();
}

BOOL RTFFontTableAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFFontTableAction* pNew = new RTFFontTableAction(pFilter);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFPaperWidth : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Reads RTF document's paper width.  Parameter is width of paper in twips
********************************************************************************************/
RTFPaperWidth::RTFPaperWidth(RTFTextFilter* pFilt, MILLIPOINT Width) : RTFControlAction(pFilt, Width)
{
}

BOOL RTFPaperWidth::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("paperw");
}

BOOL RTFPaperWidth::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetPaperWidth(Parameter);
	return TRUE;
}

BOOL RTFPaperWidth::CreateAndExecute(RTFTextFilter* pFilter, TWIP Width)
{
	RTFPaperWidth* pNew = new RTFPaperWidth(pFilter, Width*TwipToMillpoint);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFPaperHeight : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Read RTF document's paper height
********************************************************************************************/
RTFPaperHeight::RTFPaperHeight(RTFTextFilter* pFilt, MILLIPOINT Height) : RTFControlAction(pFilt, Height)
{
}

BOOL RTFPaperHeight::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("paperh");
}

BOOL RTFPaperHeight::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetPaperHeight(Parameter);
	return TRUE;
}

BOOL RTFPaperHeight::CreateAndExecute(RTFTextFilter* pFilter, TWIP Height)
{
	RTFPaperHeight* pNew = new RTFPaperHeight(pFilter, Height*TwipToMillpoint);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFLeftMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the width of the left-hand page margin
********************************************************************************************/
RTFLeftMargin::RTFLeftMargin(RTFTextFilter* pFilt, MILLIPOINT Width) : RTFControlAction(pFilt, Width)
{
}

BOOL RTFLeftMargin::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("margl");
}

BOOL RTFLeftMargin::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetLeftMargin(Parameter);
	return TRUE;
}

BOOL RTFLeftMargin::CreateAndExecute(RTFTextFilter* pFilter, TWIP Width)
{
	RTFLeftMargin* pNew = new RTFLeftMargin(pFilter, Width*TwipToMillpoint);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFRightMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the width of the right-hand page margin
********************************************************************************************/
RTFRightMargin::RTFRightMargin(RTFTextFilter* pFilt, MILLIPOINT Width) : RTFControlAction(pFilt, Width)
{
}

BOOL RTFRightMargin::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("margr");
}

BOOL RTFRightMargin::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetRightMargin(Parameter);
	return TRUE;
}

BOOL RTFRightMargin::CreateAndExecute(RTFTextFilter* pFilter, TWIP Width)
{
	RTFRightMargin* pNew = new RTFRightMargin(pFilter, Width*TwipToMillpoint);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFTopMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the height of the margin at the top of the page
********************************************************************************************/
RTFTopMargin::RTFTopMargin(RTFTextFilter* pFilt, MILLIPOINT Height) : RTFControlAction(pFilt, Height)
{
}

BOOL RTFTopMargin::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("margt");
}

BOOL RTFTopMargin::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetTopMargin(Parameter);
	return TRUE;
}

BOOL RTFTopMargin::CreateAndExecute(RTFTextFilter* pFilter, TWIP Height)
{
	RTFTopMargin* pNew = new RTFTopMargin(pFilter, Height*TwipToMillpoint);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFBottomMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the height of the margin at the bottom of the page
********************************************************************************************/
RTFBottomMargin::RTFBottomMargin(RTFTextFilter* pFilt, MILLIPOINT Height) : RTFControlAction(pFilt, Height)
{
}

BOOL RTFBottomMargin::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("margb");
}

BOOL RTFBottomMargin::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->SetBottomMargin(Parameter);
	return TRUE;
}

BOOL RTFBottomMargin::CreateAndExecute(RTFTextFilter* pFilter, TWIP Height)
{
	RTFBottomMargin* pNew = new RTFBottomMargin(pFilter, Height*TwipToMillpoint);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFResetCharAttrs : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Dispatches the reset character attributes command.
				No parameter.
********************************************************************************************/
RTFResetCharAttrs::RTFResetCharAttrs(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFResetCharAttrs::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("plain");
}

BOOL RTFResetCharAttrs::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->ResetCharacterAttributes();
	return TRUE;
}

BOOL RTFResetCharAttrs::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFResetCharAttrs* pNew = new RTFResetCharAttrs(pFilter);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFResetParaAttrs : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Dispatches the reset paragraph attributes command.
				No parameter.
********************************************************************************************/
RTFResetParaAttrs::RTFResetParaAttrs(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFResetParaAttrs::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("pard");
}

BOOL RTFResetParaAttrs::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	pFilter->ResetParagraphAttributes();
	return TRUE;
}

BOOL RTFResetParaAttrs::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFResetParaAttrs* pNew = new RTFResetParaAttrs(pFilter);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFTableCellAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Purpose:	Dispatches a 'cell' command in a table
				No parameter.
********************************************************************************************/
RTFTableCellAction::RTFTableCellAction(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFTableCellAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("cell");
}

BOOL RTFTableCellAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->InsertTab();
}

BOOL RTFTableCellAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFTableCellAction* pNew = new RTFTableCellAction(pFilter);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFTableRowAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Purpose:	Dispatches a 'row' command in a table
				No parameter.
********************************************************************************************/
RTFTableRowAction::RTFTableRowAction(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFTableRowAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("row");
}

BOOL RTFTableRowAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->StartNewLine();
}

BOOL RTFTableRowAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFTableRowAction* pNew = new RTFTableRowAction(pFilter);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFTabAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/95
	Purpose:	Processes a tab character
********************************************************************************************/
RTFTabAction::RTFTabAction(RTFTextFilter* pFilt) : RTFControlAction(pFilt, 0)
{
}

BOOL RTFTabAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("tab");
}

BOOL RTFTabAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->InsertTab();
}

BOOL RTFTabAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFTabAction* pNew = new RTFTabAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFSingleCharAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/96
	Purpose:	Inserts a single 'special' character into the import story
				This class cannot be created and executed; only classes derived from it
********************************************************************************************/
RTFSingleCharAction::RTFSingleCharAction(RTFTextFilter* pFilt, WCHAR Character) : RTFControlAction(pFilt, Character)
{
}

BOOL RTFSingleCharAction::ExecuteControl()
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");
	return pFilter->InsertChar((WCHAR)Parameter);
}



/********************************************************************************************
>	class RTFHexCharAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/96
	Purpose:	Processes a character defined by two hex chars
********************************************************************************************/
RTFHexCharAction::RTFHexCharAction(RTFTextFilter* pFilt, WCHAR Character) : RTFSingleCharAction(pFilt, Character)
{
}

BOOL RTFHexCharAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("'");
}

BOOL RTFHexCharAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	ERROR2IF(pFilter == NULL, FALSE, "pFilter == NULL");

	// read the two hex digits defining this character
	BYTE Digits = 0;
	for (INT32 loop = 1; loop<=2; loop++)
	{
		Digits = Digits << 4;
		INT32 Base = 0;
		TCHAR ReadChar = 0;
		if (!pFilter->ReadCharFromFile(&ReadChar))
			return FALSE;

		if (ReadChar >= '0' && ReadChar <= '9')
			Base = '0';
		else
		{
			if (ReadChar >= 'a' && ReadChar <= 'f')
				Base =  'a' - 10;
			else
			{
				if (ReadChar >= 'A' && ReadChar <= 'F')
					Base =  'A' - 10;
				else
				{
					Error::SetError(_R(IDE_RTF_DUFFFILE));
					return FALSE;
				}
			}
		}

		Digits += ReadChar - Base;
	}

	WCHAR Character = TextManager::MultiByteToUnicode(Digits);

	RTFHexCharAction* pNew = new RTFHexCharAction(pFilter, Character);

	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFEmDashAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a em dash - a dash the width of an em
********************************************************************************************/
RTFEmDashAction::RTFEmDashAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x2014)
{
	// BODGE - check that number!
}

BOOL RTFEmDashAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("emdash");
}

BOOL RTFEmDashAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFEmDashAction* pNew = new RTFEmDashAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFEnDashAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a en dash - a dash the width of an en
********************************************************************************************/
RTFEnDashAction::RTFEnDashAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x2013)
{
	// BODGE - check that number!
}

BOOL RTFEnDashAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("endash");
}

BOOL RTFEnDashAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFEnDashAction* pNew = new RTFEnDashAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFEmSpaceAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes an em space - a space the width of an em
********************************************************************************************/
RTFEmSpaceAction::RTFEmSpaceAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x0020)
{
	// BODGE - prehaps use a kern to simulate an em space?
}

BOOL RTFEmSpaceAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("emspace");
}

BOOL RTFEmSpaceAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFEmSpaceAction* pNew = new RTFEmSpaceAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFEnSpaceAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes an en space - a space the width of an en
********************************************************************************************/
RTFEnSpaceAction::RTFEnSpaceAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x0020)
{
	// BODGE - prehaps use a kern to simulate an en space?
}

BOOL RTFEnSpaceAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("enspace");
}

BOOL RTFEnSpaceAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFEnSpaceAction* pNew = new RTFEnSpaceAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFBulletAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a bullet symbol
********************************************************************************************/
RTFBulletAction::RTFBulletAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x2022)
{
}

BOOL RTFBulletAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("bullet");
}

BOOL RTFBulletAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFBulletAction* pNew = new RTFBulletAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFSingleLeftQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes single left sexed quote
********************************************************************************************/
RTFSingleLeftQuoteAction::RTFSingleLeftQuoteAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x2018)
{
}

BOOL RTFSingleLeftQuoteAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("lquote");
}

BOOL RTFSingleLeftQuoteAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFSingleLeftQuoteAction* pNew = new RTFSingleLeftQuoteAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFSingleRightQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes single right sexed quote
********************************************************************************************/
RTFSingleRightQuoteAction::RTFSingleRightQuoteAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x2019)
{
}

BOOL RTFSingleRightQuoteAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("rquote");
}

BOOL RTFSingleRightQuoteAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFSingleRightQuoteAction* pNew = new RTFSingleRightQuoteAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFDoubleLeftQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes double left sexed quote
********************************************************************************************/
RTFDoubleLeftQuoteAction::RTFDoubleLeftQuoteAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x201c)
{
}

BOOL RTFDoubleLeftQuoteAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("ldblquote");
}

BOOL RTFDoubleLeftQuoteAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFDoubleLeftQuoteAction* pNew = new RTFDoubleLeftQuoteAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFDoubleRightQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes double right sexed quote
********************************************************************************************/
RTFDoubleRightQuoteAction::RTFDoubleRightQuoteAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x201d)
{
}

BOOL RTFDoubleRightQuoteAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("rdblquote");
}

BOOL RTFDoubleRightQuoteAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFDoubleRightQuoteAction* pNew = new RTFDoubleRightQuoteAction(pFilter);
	return ExecuteControlCarefully(pNew);
}



/********************************************************************************************
>	class RTFNonBreakSpaceAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes double right sexed quote
********************************************************************************************/
RTFNonBreakSpaceAction::RTFNonBreakSpaceAction(RTFTextFilter* pFilt) : RTFSingleCharAction(pFilt, (WCHAR)0x00a0)
{
}

BOOL RTFNonBreakSpaceAction::IsThisAction(StringBase* pControl)
{
	return *pControl == String_16("~");
}

BOOL RTFNonBreakSpaceAction::CreateAndExecute(RTFTextFilter* pFilter)
{
	RTFNonBreakSpaceAction* pNew = new RTFNonBreakSpaceAction(pFilter);
	return ExecuteControlCarefully(pNew);
}

#endif	// #if BUILD_OTHER_TEXT_FILTER

#endif	// #if BUILD_TEXT_FILTERS
