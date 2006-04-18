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
/********************************************************************************************

>	rechtext.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Purpose:	Text record handler for the new file format

********************************************************************************************/

#include "camtypes.h"

//#include "ccpanose.h"

#include "txtattr.h"

#include "nodetext.h"
#include "nodetxtl.h"
#include "nodetxts.h"

#include "cxfdefs.h"
#include "cxftags.h"
#include "cxfrec.h"
#include "cxfrech.h"

#include "impstr.h"
#include "rechtext.h"

#include "fontman.h"
#include "doccomp.h"
#include "fontcomp.h"

// to match the declare in the header file
CC_IMPLEMENT_DYNAMIC(TextObjRecordHandler, CamelotRecordHandler);
CC_IMPLEMENT_DYNAMIC(TextAttrRecordHandler, CamelotRecordHandler);
CC_IMPLEMENT_DYNAMIC(FontDefRecordHandler, CamelotRecordHandler);

/********************************************************************************************

>	BOOL TextObjRecordHandler::BeginImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Informs the handler that importing is about to begin.

				Inits vars prior to importing.

********************************************************************************************/

BOOL TextObjRecordHandler::BeginImport()
{
	pLastInsertedTextStory = NULL;

	return TRUE;
}

/********************************************************************************************

>	UINT32 *TextObjRecordHandler::GetTagList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		-
	Returns:	UINT32* to an array of records handled by this class.
	Purpose:	Returns an array of records handled by this class.

********************************************************************************************/

UINT32 *TextObjRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_TEXT_STORY_SIMPLE,
								TAG_TEXT_STORY_COMPLEX,
								TAG_TEXT_STORY_SIMPLE_START_LEFT,
								TAG_TEXT_STORY_SIMPLE_START_RIGHT,
								TAG_TEXT_STORY_SIMPLE_END_LEFT,
								TAG_TEXT_STORY_SIMPLE_END_RIGHT,
								TAG_TEXT_STORY_COMPLEX_START_LEFT,
								TAG_TEXT_STORY_COMPLEX_START_RIGHT,
								TAG_TEXT_STORY_COMPLEX_END_LEFT,
								TAG_TEXT_STORY_COMPLEX_END_RIGHT,
								TAG_TEXT_STORY_WORD_WRAP_INFO,
								TAG_TEXT_STORY_INDENT_INFO,
								TAG_TEXT_LINE,
								TAG_TEXT_STRING,
								TAG_TEXT_CHAR,
								TAG_TEXT_EOL,
								TAG_TEXT_KERN,
								TAG_TEXT_CARET,
								TAG_TEXT_LINE_INFO,
								CXFRH_TAG_LIST_END};
	return TagList;
}

/********************************************************************************************

>	BOOL TextObjRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pCXaraFileRecord - pointer to a CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Handles reading a text attribute record from a CXaraFile.

********************************************************************************************/

BOOL TextObjRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord==NULL");
	
	BOOL ok;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_TEXT_STORY_SIMPLE				: ok = ReadTextStorySimple(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_COMPLEX				: ok = ReadTextStoryComplex(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_SIMPLE_START_LEFT	: ok = ReadTextStorySimpleStartLeft(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_SIMPLE_START_RIGHT	: ok = ReadTextStorySimpleStartRight(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_SIMPLE_END_LEFT		: ok = ReadTextStorySimpleEndLeft(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_SIMPLE_END_RIGHT	: ok = ReadTextStorySimpleEndRight(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_COMPLEX_START_LEFT	: ok = ReadTextStoryComplexStartLeft(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_COMPLEX_START_RIGHT	: ok = ReadTextStoryComplexStartRight(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_COMPLEX_END_LEFT	: ok = ReadTextStoryComplexEndLeft(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_COMPLEX_END_RIGHT	: ok = ReadTextStoryComplexEndRight(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_WORD_WRAP_INFO		: ok = ReadTextStoryWordWrapInfo(pCXaraFileRecord); break;
		case TAG_TEXT_STORY_INDENT_INFO			: ok = ReadTextStoryIndentInfo(pCXaraFileRecord); break;

		case TAG_TEXT_LINE						: ok = ReadTextLine(pCXaraFileRecord); break;
		case TAG_TEXT_STRING					: ok = ReadTextString(pCXaraFileRecord); break;
		case TAG_TEXT_CHAR						: ok = ReadTextChar(pCXaraFileRecord); break;
		case TAG_TEXT_EOL						: ok = ReadTextEOL(pCXaraFileRecord); break;
		case TAG_TEXT_KERN						: ok = ReadTextKernCode(pCXaraFileRecord); break;
		case TAG_TEXT_CARET						: ok = ReadTextCaret(pCXaraFileRecord); break;
		case TAG_TEXT_LINE_INFO					: ok = ReadTextLineInfo(pCXaraFileRecord); break;
		default									: ok = ReadTextInvalid(pCXaraFileRecord); break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL TextObjRecordHandler::InsertTextStoryNode(TextStory* pTextStory)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Informs the handler that importing is about to begin.

				Inits vars prior to importing.

********************************************************************************************/

BOOL TextObjRecordHandler::InsertTextStoryNode(TextStory* pTextStory)
{
	ERROR2IF(pTextStory == NULL,FALSE,"NULL text story ptr");

	pLastInsertedTextStory = pTextStory;

	return InsertNode(pTextStory);
}


/********************************************************************************************

>	BOOL TextObjRecordHandler::ReadTextStorySimple(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStoryComplex(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStorySimpleStartLeft(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStorySimpleStartRight(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStorySimpleEndLeft(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler;:ReadTextStorySimpleEndRight(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStoryComplexStartLeft(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStoryComplexStartRight(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextStoryComplexEndLeft(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler;:ReadTextStoryComplexEndRight(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextString(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextLine(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextChar(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextEOL(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextKern(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextCaret(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextObjRecordHandler::ReadTextInvalid(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pCXaraFileRecord	- CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Handles reading a text object from the filter

********************************************************************************************/

BOOL TextObjRecordHandler::ReadTextStorySimple(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	DocCoord MyCoord;
	Matrix MyMatrix;
	
	BOOL ok = TRUE;
	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadCoord(&MyCoord);
	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );
	MyMatrix = Matrix(MyCoord);
	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryComplex(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	Matrix MyMatrix;

	BOOL ok = TRUE;
	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&MyMatrix);
	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStorySimpleStartLeft(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	DocCoord MyCoord;
	Matrix MyMatrix;
	
	BOOL ok = TRUE;
	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadCoord(&MyCoord);
	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	MyMatrix = Matrix(MyCoord);
	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStorySimpleStartRight(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	DocCoord MyCoord;
	Matrix MyMatrix;
	
	BOOL ok = TRUE;
	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadCoord(&MyCoord);
	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	MyMatrix = Matrix(MyCoord);
	pStory->SetStoryMatrix(MyMatrix);

	pStory->SetCharsScale(-1.0);

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStorySimpleEndLeft(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	DocCoord MyCoord;
	Matrix MyMatrix;
	
	BOOL ok = TRUE;
	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadCoord(&MyCoord);
	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	MyMatrix = Matrix(MyCoord);
	pStory->SetStoryMatrix(MyMatrix);

	pStory->ReverseTextOnPath();

	pStory->SetCharsScale(-1.0);

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStorySimpleEndRight(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	DocCoord MyCoord;
	Matrix MyMatrix;
	
	BOOL ok = TRUE;
	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadCoord(&MyCoord);
	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	MyMatrix = Matrix(MyCoord);
	pStory->SetStoryMatrix(MyMatrix);

	pStory->ReverseTextOnPath();

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryComplexStartLeft(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	Matrix MyMatrix;
	ANGLE MyRotation;
	ANGLE MyShear;

	BOOL ok = TRUE;

	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&MyMatrix);
	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyRotation);
	pStory->SetCharsRotation(MyRotation);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyShear);
	pStory->SetCharsShear(MyShear);

	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryComplexStartRight(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	Matrix MyMatrix;
	ANGLE MyRotation;
	ANGLE MyShear;

	BOOL ok = TRUE;

	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&MyMatrix);
	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyRotation);
	pStory->SetCharsRotation(MyRotation);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyShear);
	pStory->SetCharsShear(MyShear);

	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	pStory->SetCharsScale(-1.0);

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryComplexEndLeft(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	Matrix MyMatrix;
	ANGLE MyRotation;
	ANGLE MyShear;

	BOOL ok = TRUE;

	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&MyMatrix);
	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyRotation);
	pStory->SetCharsRotation(MyRotation);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyShear);
	pStory->SetCharsShear(MyShear);

	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	pStory->SetCharsScale(-1.0);
	pStory->ReverseTextOnPath();

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryComplexEndRight(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	Matrix MyMatrix;
	ANGLE MyRotation;
	ANGLE MyShear;

	BOOL ok = TRUE;

	TextStory *pStory = new TextStory();

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&MyMatrix);
	pStory->SetStoryMatrix(MyMatrix);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyRotation);
	pStory->SetCharsRotation(MyRotation);

	if (ok) ok = pCXaraFileRecord->ReadANGLE(&MyShear);
	pStory->SetCharsShear(MyShear);

	if (ok) ok = ReadAutoKern ( pCXaraFileRecord, pStory );

	pStory->ReverseTextOnPath();

	if (ok) ok = InsertTextStoryNode(pStory);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryWordWrapInfo(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;

	if (pLastInsertedTextStory != NULL)
	{
		INT32 Width;
		BYTE WordWrapOn;

		if (ok) ok = pCXaraFileRecord->ReadINT32(&Width);
		if (ok) ok = pCXaraFileRecord->ReadBYTE(&WordWrapOn);

// Never word wrap in Ralph builds
// This is to prevent text stories overflowing when a font has been substituted.
// It was decided that it is better for each individual line to extend past the format width, rather than
// letting the story potentially create extra lines due to formatting width characters into the same width
#if defined(EXCLUDE_FROM_RALPH) || defined(EXCLUDE_FROM_XARALX)
		WordWrapOn = FALSE;
#endif

		if (ok)
		{
			pLastInsertedTextStory->SetStoryWidth(Width);
			pLastInsertedTextStory->SetWordWrapping(WordWrapOn != 0);
		}
	}

	return ok;
}

BOOL TextObjRecordHandler::ReadTextStoryIndentInfo(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;

	if (pLastInsertedTextStory != NULL)
	{
		INT32 LeftIndent;
		INT32 RightIndent;

		if (ok) ok = pCXaraFileRecord->ReadINT32(&LeftIndent);
		if (ok) ok = pCXaraFileRecord->ReadINT32(&RightIndent);

		if (ok)
		{
			pLastInsertedTextStory->SetLeftIndent(LeftIndent);
			pLastInsertedTextStory->SetRightIndent(RightIndent);
		}
	}

	return ok;
}

BOOL TextObjRecordHandler::ReadTextLine(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	TextLine *pLine = new TextLine();

	if (ok) ok = InsertNode(pLine);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextLineInfo(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");
	return TRUE;
}

BOOL TextObjRecordHandler::ReadTextString(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	WCHAR MyWChar;

	// Text strings are not converted into N text chars here.
	// Instead, the first char in the string is inserted into the tree.
	// This marks where the rest of the string should appear.
	// A ptr to the resultant char, and a copy of the string record, is placed inside
	// a ImportedString object.  This object is added to the string list for this text story.
	//
	// After this record is read in, any subsequent child attr recs that apply to the whole string will
	// get applied to the first text char we've just added.
	//
	// On post-import, the text story runs through it's list of imported strings, expanding them
	// into a series of text chars.  As it does this, it copies the child attrs of the first char
	// (i.e. the char inserted in this func) to all the rest of the text chars in the string.
	// This ensures that all the chars in the string will look the same.


	// Create a TextChar to hold the first char in the string. Then insert it into the tree
	TextChar* pChar = new TextChar();
	if (ok) ok = (pChar != NULL);
	if (ok) ok = pCXaraFileRecord->ReadWCHAR(&MyWChar);
	if (ok) pChar->SetUnicodeValue(MyWChar);
	if (ok) ok = InsertNode(pChar);

	if (ok)
	{
		// Keep a ptr to the inserted TextChar, and copy of the string, so that the rest of the
		// string can be expanded after all the relevant attrs have come in.

		ImportedString* pImportedString = new ImportedString(pChar,pCXaraFileRecord);
		TextStory* pTextStory = pChar->FindParentStory();
		if (pTextStory != NULL && pImportedString != NULL)
			pTextStory->AddImportedString(pImportedString);
		else
			ok = FALSE;
	}

	return ok;
}
	
BOOL TextObjRecordHandler::ReadTextChar(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	WCHAR MyWChar;

	TextChar *pChar = new TextChar();

	if (ok) ok = pCXaraFileRecord->ReadWCHAR(&MyWChar);
	pChar->SetUnicodeValue(MyWChar);
	if (ok) ok = InsertNode(pChar);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextEOL(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	EOLNode *pEOL = new EOLNode();

	if (ok) ok = InsertNode(pEOL);

	return ok;
}

BOOL TextObjRecordHandler::ReadTextKernCode(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter pCXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	DocCoord MyKern;
	KernCode *pKern = new KernCode();

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MyKern,0,0);
	pKern->SetValue(MyKern);
	if (ok) ok = InsertNode(pKern);

	return ok;
}
	
BOOL TextObjRecordHandler::ReadTextCaret(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter pCXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	CaretNode *pCaret = new CaretNode();

	if (ok) ok = InsertNode(pCaret);

	return ok;	
}

BOOL TextObjRecordHandler::ReadTextInvalid(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR3("Attempt to read an invalid text object.");
	return FALSE;
}

/********************************************************************************************

>	BOOL TextObjRecordHandler::ReadAutoKern ( CXaraFileRecord	*pCXaraFileRecord,
											  TextStory			*pStory )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/00
	Inputs:		pCXaraFileRecord	- A pointer to a file record.
				pStory				- The text story being read in from the file.
	Returns:	TRUE				- The text story was read OK.
				FALSE				- A file error occured.
	Purpose:	Reads an AutoKern record from the file. This class is necessary because
				there is no ReadBOOLnoError function. Using a ReadINT32noError without the
				if statement will cause a number of compiler warnings to be thrown, so this
				way cleans things up somewhat.

********************************************************************************************/

BOOL TextObjRecordHandler::ReadAutoKern ( CXaraFileRecord	*pCXaraFileRecord,
										  TextStory			*pStory )
{
	INT32 AutoKern;

	// Attempt to read the autokern record from the .xar file.
	if ( pCXaraFileRecord->ReadINT32noError ( &AutoKern ) )
	{
		// Is the auto kern flag set?
		if ( AutoKern )
		{
			// Yes - set autokerning to be TRUE.
			pStory->SetAutoKerning ( TRUE );
		}
		else
		{
			// Nope - so set the flag to FALSE.
			pStory->SetAutoKerning ( FALSE );
		}

		// The file record was read OK.
		return TRUE;
	}

	// There was a file I/O error.
	return FALSE;
}

/********************************************************************************************

>	void TextObjRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeStoryVanilla(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeStoryStartLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeStoryStartRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeStoryEndLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeStoryEndRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordhandler::DescribeLine(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeString(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeChar(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeEOL(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeKern(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeCaret(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextObjRecordHandler::DescribeInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/07/96
	Inputs:		pCXaraFileRecord	- CXaraFileRecord to describe
				pStr				- string to add description to
	Returns:	-	
	Purpose:	Functions to describe text object records for debugging purposes

********************************************************************************************/

#if XAR_TREE_DIALOG
void TextObjRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr)
{
	if (pCXaraFileRecord!=NULL || pStr!=NULL)
	{
		GetRecordInfo(pCXaraFileRecord, pStr);

		switch (pCXaraFileRecord->GetTag())
		{
		case TAG_TEXT_STORY_SIMPLE				: DescribeStorySimple(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_COMPLEX				: DescribeStoryComplex(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_SIMPLE_START_LEFT	: DescribeStorySimpleStartLeft(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_SIMPLE_START_RIGHT	: DescribeStorySimpleStartRight(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_SIMPLE_END_LEFT		: DescribeStorySimpleEndLeft(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_SIMPLE_END_RIGHT	: DescribeStorySimpleEndRight(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_COMPLEX_START_LEFT	: DescribeStoryComplexStartLeft(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_COMPLEX_START_RIGHT	: DescribeStoryComplexStartRight(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_COMPLEX_END_LEFT	: DescribeStoryComplexEndLeft(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_COMPLEX_END_RIGHT	: DescribeStoryComplexEndRight(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_WORD_WRAP_INFO		: DescribeStoryWordWrapInfo(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STORY_INDENT_INFO			: DescribeStoryIndentInfo(pCXaraFileRecord, pStr); break;

		case TAG_TEXT_LINE						: DescribeLine(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_STRING					: DescribeString(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_CHAR						: DescribeChar(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_EOL						: DescribeEOL(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_KERN						: DescribeKern(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_CARET						: DescribeCaret(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_LINE_INFO					: DescribeLineInfo(pCXaraFileRecord, pStr); break;
		default									: DescribeInvalid(pCXaraFileRecord, pStr); break;
		}
	}
}

void TextObjRecordHandler::DescribeStorySimple(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story (Simple)\r\n");
}

void TextObjRecordHandler::DescribeStoryComplex(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story (Complex)\r\n");
}

void TextObjRecordHandler::DescribeStorySimpleStartLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - Start Left (Simple)\r\n");
}

void TextObjRecordHandler::DescribeStorySimpleStartRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - Start Right (Simple)\r\n");
}

void TextObjRecordHandler::DescribeStorySimpleEndLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - End Left (Simple)\r\n");
}

void TextObjRecordHandler::DescribeStorySimpleEndRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - End Right (Simple)\r\n");
}

void TextObjRecordHandler::DescribeStoryComplexStartLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - Start Left (Complex)\r\n");
}

void TextObjRecordHandler::DescribeStoryComplexStartRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - Start Right (Complex)\r\n");
}

void TextObjRecordHandler::DescribeStoryComplexEndLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - End Left (Complex)\r\n");
}

void TextObjRecordHandler::DescribeStoryComplexEndRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Story - End Right (Complex)\r\n");
}

void TextObjRecordHandler::DescribeStoryWordWrapInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	(*pStr) += _T("Text Story Word Wrap Infor\r\n");

	INT32 Width;
	BYTE WordWrapOn;

	pCXaraFileRecord->ReadINT32(&Width);
	pCXaraFileRecord->ReadBYTE(&WordWrapOn);

	camSprintf(s, _T("Width : \t%ld\r\n"),Width);
	(*pStr) += s;
	camSprintf(s, _T("Word Wrap On : \t%ld\r\n"),WordWrapOn);
	(*pStr) += s;
}


void TextObjRecordHandler::DescribeStoryIndentInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	(*pStr) += _T("Text Story Indent Info\r\n");

	INT32 LeftIndent;
	INT32 RightIndent;

	pCXaraFileRecord->ReadINT32(&LeftIndent);
	pCXaraFileRecord->ReadINT32(&RightIndent);

	camSprintf(s, _T("Left indent : \t%ld\r\n"),LeftIndent);
	(*pStr) += s;
	camSprintf(s, _T("Right indent : \t%ld\r\n"),RightIndent);
	(*pStr) += s;
}


void TextObjRecordHandler::DescribeLine(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Line\r\n");
}

void TextObjRecordHandler::DescribeLineInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	INT32 Width,Height,DistFromPreviousLine;

	pCXaraFileRecord->ReadINT32(&Width);
	pCXaraFileRecord->ReadINT32(&Height);
	pCXaraFileRecord->ReadINT32(&DistFromPreviousLine);

	(*pStr) += _T("Text Line info\r\n");
	camSprintf(s, _T("Width : \t%ld\r\n"),Width);
	(*pStr) += s;
	camSprintf(s, _T("Height: \t%ld\r\n"),Height);
	(*pStr) += s;
	camSprintf(s, _T("Dist from previous line: \t%ld\r\n"),DistFromPreviousLine);
	(*pStr) += s;

	(*pStr) += _T("\r\n'Dist from previous line' is the y distance between\r\n");
	(*pStr) += _T("this line's base line and the previous line's base line\r\n");
}

void TextObjRecordHandler::DescribeString(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	UINT32 RecSize = pCXaraFileRecord->GetSize();
	WCHAR MyWChar;
	UINT32 i;

	(*pStr) += _T("Text String\r\nString = ");

	for (i=0; i<RecSize/2; i++)
	{
		pCXaraFileRecord->ReadWCHAR(&MyWChar);
		camSprintf(s, _T("%lc"), MyWChar);
		(*pStr) += s;
	}

	(*pStr) += _T("\r\n");
}

void TextObjRecordHandler::DescribeChar(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	WCHAR MyWChar;

	pCXaraFileRecord->ReadWCHAR(&MyWChar);

	camSprintf(s, _T("Text Char\r\nChar = %lc\r\n"), MyWChar);
	(*pStr) += s;
}

void TextObjRecordHandler::DescribeEOL(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text EOL\r\n");
}

void TextObjRecordHandler::DescribeKern(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	INT32 KernX, KernY;
	pCXaraFileRecord->ReadINT32(&KernX);
	pCXaraFileRecord->ReadINT32(&KernY);

	camSprintf(s, _T("Text Kern Code\r\nKern Code = DocCoord(%ld, %ld)"), KernX, KernY);
	(*pStr) += s;
}

void TextObjRecordHandler::DescribeCaret(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Caret\r\n");
}

void TextObjRecordHandler::DescribeInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	(*pStr) += _T("Text Object - Invalid\r\n");
}

#endif

/********************************************************************************************

>	UINT32 *TextAttrRecordHandler::GetTagList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		-
	Returns:	UINT32* to an array of records handled by this class.
	Purpose:	Returns an array of records handled by this class.

********************************************************************************************/

UINT32 *TextAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_TEXT_LINESPACE_RATIO,
								TAG_TEXT_LINESPACE_ABSOLUTE,
								TAG_TEXT_JUSTIFICATION_LEFT,
								TAG_TEXT_JUSTIFICATION_CENTRE,
								TAG_TEXT_JUSTIFICATION_RIGHT,
								TAG_TEXT_JUSTIFICATION_FULL,
								TAG_TEXT_FONT_SIZE,
								TAG_TEXT_FONT_TYPEFACE,
								TAG_TEXT_BOLD_ON,
								TAG_TEXT_BOLD_OFF,
								TAG_TEXT_ITALIC_ON,
								TAG_TEXT_ITALIC_OFF,
								TAG_TEXT_UNDERLINE_ON,
								TAG_TEXT_UNDERLINE_OFF,
								TAG_TEXT_SCRIPT_ON,
								TAG_TEXT_SCRIPT_OFF,
								TAG_TEXT_SUPERSCRIPT_ON,
								TAG_TEXT_SUBSCRIPT_ON,
								TAG_TEXT_TRACKING,
								TAG_TEXT_ASPECT_RATIO,
								TAG_TEXT_BASELINE,
								CXFRH_TAG_LIST_END};
	return TagList;
}

/********************************************************************************************

>	BOOL TextAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pCXaraFileRecord - pointer to a CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Handles reading a text attribute record from a CXaraFile.

********************************************************************************************/

BOOL TextAttrRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord==NULL");
	
	BOOL ok;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_TEXT_LINESPACE_RATIO		: ok = ReadTextLineSpaceRatio(pCXaraFileRecord); break;
		case TAG_TEXT_LINESPACE_ABSOLUTE	: ok = ReadTextLineSpaceAbsolute(pCXaraFileRecord); break;
		case TAG_TEXT_JUSTIFICATION_LEFT	: ok = ReadTextJustificationLeft(pCXaraFileRecord); break;
		case TAG_TEXT_JUSTIFICATION_CENTRE	: ok = ReadTextJustificationCentre(pCXaraFileRecord); break;
		case TAG_TEXT_JUSTIFICATION_RIGHT	: ok = ReadTextJustificationRight(pCXaraFileRecord); break;
		case TAG_TEXT_JUSTIFICATION_FULL	: ok = ReadTextJustificationFull(pCXaraFileRecord); break;
		case TAG_TEXT_FONT_SIZE				: ok = ReadTextFontSize(pCXaraFileRecord); break;
		case TAG_TEXT_FONT_TYPEFACE			: ok = ReadTextFontTypeface(pCXaraFileRecord); break;
		case TAG_TEXT_BOLD_ON				: ok = ReadTextBoldOn(pCXaraFileRecord); break;
		case TAG_TEXT_BOLD_OFF				: ok = ReadTextBoldOff(pCXaraFileRecord); break;
		case TAG_TEXT_ITALIC_ON				: ok = ReadTextItalicOn(pCXaraFileRecord); break;
		case TAG_TEXT_ITALIC_OFF			: ok = ReadTextItalicOff(pCXaraFileRecord); break;
		case TAG_TEXT_UNDERLINE_ON			: ok = ReadTextUnderlineOn(pCXaraFileRecord); break;
		case TAG_TEXT_UNDERLINE_OFF			: ok = ReadTextUnderlineOff(pCXaraFileRecord); break;
		case TAG_TEXT_SCRIPT_ON				: ok = ReadTextScriptOn(pCXaraFileRecord); break;
		case TAG_TEXT_SCRIPT_OFF			: ok = ReadTextScriptOff(pCXaraFileRecord); break;
		case TAG_TEXT_SUPERSCRIPT_ON		: ok = ReadTextSuperscriptOn(pCXaraFileRecord); break;
		case TAG_TEXT_SUBSCRIPT_ON			: ok = ReadTextSubscriptOn(pCXaraFileRecord); break;
		case TAG_TEXT_TRACKING				: ok = ReadTextTracking(pCXaraFileRecord); break;
		case TAG_TEXT_ASPECT_RATIO			: ok = ReadTextAspectRatio(pCXaraFileRecord); break;
		case TAG_TEXT_BASELINE				: ok = ReadTextBaseline(pCXaraFileRecord); break;
		default								: ok = ReadTextInvalid(pCXaraFileRecord); break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL TextAttrRecordHandler::ReadTextStoryVanilla(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextString(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextKernCode(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextCaret(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextLineSpaceRatio(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextLineSpaceAbsolute(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextJustificationLeft(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextJustificationCentre(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextJustificationRight(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextJustificationFull(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextFontSize(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextFontTypeface(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextBoldOn(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextBoldOff(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextItalicOn(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextItalicOff(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextUnderlineOn(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextUnderlineOff(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextScriptOn(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextScriptOff(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextSuperscriptOn(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextSubscriptOn(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextTracking(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextAspectRatio(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextBaseline(CXaraFileRecord *pCXaraFileRecord)
	BOOL TextAttrRecordHandler::ReadTextInvalid(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pCXaraFileRecord - pointer to a CXaraFileRecord to load
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Read text attributes for the new file format

********************************************************************************************/

BOOL TextAttrRecordHandler::ReadTextLineSpaceRatio(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtLineSpace *pAttr = new AttrTxtLineSpace();
	FIXED16 Ratio;

	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Ratio);

	pAttr->Value.IsRatio = TRUE;
	pAttr->Value.Value = 0;
	pAttr->Value.Ratio = Ratio;
	
	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextLineSpaceAbsolute(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtLineSpace *pAttr = new AttrTxtLineSpace();
	MILLIPOINT Value;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&Value);

	pAttr->Value.IsRatio = FALSE;
	pAttr->Value.Value = Value;
	pAttr->Value.Ratio = 0;
	
	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextJustificationLeft(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtJustification *pAttr = new AttrTxtJustification();

	pAttr->Value.justification = JLEFT;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextJustificationCentre(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtJustification *pAttr = new AttrTxtJustification();

	pAttr->Value.justification = JCENTRE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextJustificationRight(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtJustification *pAttr = new AttrTxtJustification();

	pAttr->Value.justification = JRIGHT;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextJustificationFull(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtJustification *pAttr = new AttrTxtJustification();

	pAttr->Value.justification = JFULL;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextFontSize(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtFontSize *pAttr = new AttrTxtFontSize();
	MILLIPOINT Size;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&Size);

	pAttr->Value.FontSize = Size;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextFontTypeface(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter pCXaraFileRecord==NULL.");

	BOOL ok = TRUE;
	
	INT32 RecordNumber;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&RecordNumber);

	FontComponent *pFontComponent = GetFontComponent();
	
	WORD FontHandle;
	BOOL IsBold;
	BOOL IsItalic;

	if (ok) ok = pFontComponent->GetFontInformation(RecordNumber, &FontHandle, &IsBold, &IsItalic);

	if (ok)
	{
		AttrTxtFontTypeface *pAttr = new AttrTxtFontTypeface();

		pAttr->Value.HTypeface = FontHandle;
		pAttr->Value.IsBold = IsBold;
		pAttr->Value.IsItalic = IsItalic;

		ok = InsertNode(pAttr);
	}

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextBoldOn(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtBold *pAttr = new AttrTxtBold();

	pAttr->Value.BoldOn = TRUE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextBoldOff(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtBold *pAttr = new AttrTxtBold();

	pAttr->Value.BoldOn = FALSE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextItalicOn(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtItalic *pAttr = new AttrTxtItalic();

	pAttr->Value.ItalicOn = TRUE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextItalicOff(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtItalic *pAttr = new AttrTxtItalic();

	pAttr->Value.ItalicOn = FALSE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextUnderlineOn(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtUnderline *pAttr = new AttrTxtUnderline();

	pAttr->Value.Underlined = TRUE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextUnderlineOff(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtUnderline *pAttr = new AttrTxtUnderline();

	pAttr->Value.Underlined = FALSE;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextScriptOn(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtScript *pAttr = new AttrTxtScript();
	FIXED16 Offset;
	FIXED16 Size;

	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Offset);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Size);

	pAttr->Value.Offset = Offset;
	pAttr->Value.Size = Size;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextScriptOff(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtScript *pAttr = new AttrTxtScript();

	pAttr->Value.Offset = 0;
	pAttr->Value.Size = 1;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextSuperscriptOn(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtScript *pAttr = new AttrTxtScript();

	pAttr->Value.Offset = FIXED16(Text_SuperScriptOffset);
	pAttr->Value.Size = FIXED16(Text_SuperScriptSize);

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextSubscriptOn(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtScript *pAttr = new AttrTxtScript();

	pAttr->Value.Offset = FIXED16(Text_SubScriptOffset);
	pAttr->Value.Size = FIXED16(Text_SubScriptSize);

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextTracking(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtTracking *pAttr = new AttrTxtTracking();
	MILLIPOINT Tracking;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&Tracking);

	pAttr->Value.Tracking = Tracking;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextAspectRatio(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtAspectRatio *pAttr = new AttrTxtAspectRatio();
	FIXED16 AspectRatio;

	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&AspectRatio);

	pAttr->Value.AspectRatio = AspectRatio;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextBaseline(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord == NULL.");

	BOOL ok = TRUE;
	AttrTxtBaseLine *pAttr = new AttrTxtBaseLine();
	MILLIPOINT BaseLine;

	if (ok) ok = pCXaraFileRecord->ReadINT32(&BaseLine);

	pAttr->Value.Value = BaseLine;

	if (ok) ok = InsertNode(pAttr);

	return ok;
}

BOOL TextAttrRecordHandler::ReadTextInvalid(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR3("Attempt to read an invalid text attribute.");
	return FALSE;
}

/********************************************************************************************

>	void TextAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeLineSpaceRatio(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeLineSpaceAbsolute(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeJustificationLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeJustificationCentre(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeJustificationRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeJustificationFull(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeFontSize(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeFontTypeface(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeBoldOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeBoldOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeItalicOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeItalicOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeUnderlineOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeUnderlineOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeScriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeScriptOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeSuperscriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeSubscriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeTracking(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeAspectRatio(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeBaseline(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void TextAttrRecordHandler::DescribeInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pCXaraFileRecord	- CXaraFileRecord to describe
				pStr				- string to append text to
	Returns:	-
	Purpose:	Function to describe the text attribute records

********************************************************************************************/

#if XAR_TREE_DIALOG
void TextAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase* pStr)
{
	if (pCXaraFileRecord!=NULL || pStr!=NULL)
	{
		GetRecordInfo(pCXaraFileRecord, pStr);

		switch (pCXaraFileRecord->GetTag())
		{
		case TAG_TEXT_LINESPACE_RATIO		: DescribeLineSpaceRatio(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_LINESPACE_ABSOLUTE	: DescribeLineSpaceAbsolute(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_JUSTIFICATION_LEFT	: DescribeJustificationLeft(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_JUSTIFICATION_CENTRE	: DescribeJustificationCentre(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_JUSTIFICATION_RIGHT	: DescribeJustificationRight(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_JUSTIFICATION_FULL	: DescribeJustificationFull(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_FONT_SIZE				: DescribeFontSize(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_FONT_TYPEFACE			: DescribeFontTypeface(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_BOLD_ON				: DescribeBoldOn(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_BOLD_OFF				: DescribeBoldOff(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_ITALIC_ON				: DescribeItalicOn(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_ITALIC_OFF			: DescribeItalicOff(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_UNDERLINE_ON			: DescribeUnderlineOn(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_UNDERLINE_OFF			: DescribeUnderlineOff(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_SCRIPT_ON				: DescribeScriptOn(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_SCRIPT_OFF			: DescribeScriptOff(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_SUPERSCRIPT_ON		: DescribeSuperscriptOn(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_SUBSCRIPT_ON			: DescribeSubscriptOn(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_TRACKING				: DescribeTracking(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_ASPECT_RATIO			: DescribeAspectRatio(pCXaraFileRecord, pStr); break;
		case TAG_TEXT_BASELINE				: DescribeBaseline(pCXaraFileRecord, pStr); break;
		default								: DescribeInvalid(pCXaraFileRecord, pStr); break;
		}
	}
}

void TextAttrRecordHandler::DescribeLineSpaceRatio(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	FIXED16 Value;
	pCXaraFileRecord->ReadFIXED16(&Value);

	_stprintf(s, "Text Attribute - Line Spacing (Ratio)\r\n");
	(*pStr) += s;
	_stprintf(s, "Line spacing = %f\r\n", Value.MakeDouble());
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeLineSpaceAbsolute(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	INT32 Value;
	pCXaraFileRecord->ReadINT32(&Value);

	_stprintf(s, "Text Attribute - Line Spacing (Absolute)\r\n");
	(*pStr) += s;
	_stprintf(s, "Line spacing = %ld\r\n", Value);
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeJustificationLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Justification (Left)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeJustificationCentre(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Justification (Centre)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeJustificationRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Justification (Right)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeJustificationFull(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Justification (Full)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeFontSize(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	MILLIPOINT Value;
	pCXaraFileRecord->ReadINT32(&Value);
	
	_stprintf(s, "Text Attribute - Font Size\r\n");
	(*pStr) += s;
	_stprintf(s, "Font size = %ld\r\n", Value);
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeFontTypeface(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	INT32 RecordNumber;

	pCXaraFileRecord->ReadINT32(&RecordNumber);

	_stprintf(s, "Text Attribute - Typeface\r\n");
	(*pStr) += s;

	_stprintf(s, "Record Number = %ld\r\n", RecordNumber);
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeBoldOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Bold (On)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeBoldOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Bold (Off)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeItalicOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Italic (On)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeItalicOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Italic (Off)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeUnderlineOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Underline (On)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeUnderlineOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Underline (Off)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeScriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	FIXED16 Offset;
	FIXED16 Size;
	pCXaraFileRecord->ReadFIXED16(&Offset);
	pCXaraFileRecord->ReadFIXED16(&Size);

	_stprintf(s, "Text Attribute - Script (On)\r\n");
	(*pStr) += s;
	_stprintf(s, "Offset = %f\r\n", Offset.MakeDouble());
	(*pStr) += s;
	_stprintf(s, "Offset = %f\r\n", Size.MakeDouble());
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeScriptOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Script (Off)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeSuperscriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Superscript (On)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeSubscriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Text Attribute - Subscript (On)\r\n");
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeTracking(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	MILLIPOINT Value;
	pCXaraFileRecord->ReadINT32(&Value);

	_stprintf(s, "Text Attribute - Tracking\r\n");
	(*pStr) += s;
	_stprintf(s, "Tracking = %ld\r\n", Value);
	(*pStr) += s;
}
	
void TextAttrRecordHandler::DescribeAspectRatio(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	FIXED16 Value;
	pCXaraFileRecord->ReadFIXED16(&Value);

	_stprintf(s, "Text Attribute - Aspect Ratio\r\n");
	(*pStr) += s;
	_stprintf(s, "Aspect Ratio = %f\r\n", Value.MakeDouble());
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeBaseline(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	MILLIPOINT Value;
	pCXaraFileRecord->ReadINT32(&Value);

	_stprintf(s, "Text Attribute - Baseline Shift\r\n");
	(*pStr) += s;
	_stprintf(s, "Baseline shift = %ld\r\n", Value);
	(*pStr) += s;
}

void TextAttrRecordHandler::DescribeInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s,"Record Number = %d\r\n", pCXaraFileRecord->GetRecordNumber());
	(*pStr) += s;
	_stprintf(s, "Tag = %d\r\n", pCXaraFileRecord->GetTag());
	(*pStr) += s;
	_stprintf(s, "Size = %d\r\n\r\n", pCXaraFileRecord->GetSize());
	(*pStr) += s;

	_stprintf(s, "Text Attribute - Invalid\r\n");
	(*pStr) += s;
}

#endif	// _DEBUG

/********************************************************************************************

>	UINT32 *FontDefRecordHandler::GetTagList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		-
	Returns:	UINT32* to an array of records handled by this class.
	Purpose:	Returns an array of records handled by this class.

********************************************************************************************/

UINT32 *FontDefRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_FONT_DEF_TRUETYPE,
								TAG_FONT_DEF_ATM,
								CXFRH_TAG_LIST_END};
	return TagList;
}

/********************************************************************************************

>	BOOL FontDefRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Inputs:		pCXaraFileRecord - pointer to a CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Handles reading a text attribute record from a CXaraFile.

********************************************************************************************/

BOOL FontDefRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord==NULL, FALSE, "Parameter CXaraFileRecord==NULL");
	
	BOOL ok;

	FontComponent *pFontComponent = GetFontComponent();

	ok = pFontComponent->ReadFontDefinition(pCXaraFileRecord);

	return ok;
}

/********************************************************************************************

>	void FontDefRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr)
	void FontDefRecordHandler::DescribeFontDefTrueType(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void FontDefRecordHandler::DescribeFontDefATM(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
	void FontDefRecordHandler::DescribeFontDefInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Inputs:		pCXaraFileRecord	- CXaraFileRecord to describe
				pStr				- string to append text to
	Returns:	-
	Purpose:	Function to describe the text attribute records

********************************************************************************************/

#if XAR_TREE_DIALOG
void FontDefRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr)
{
	if (pCXaraFileRecord!=NULL || pStr!=NULL)
	{
		GetRecordInfo(pCXaraFileRecord, pStr);

		switch (pCXaraFileRecord->GetTag())
		{
		case TAG_FONT_DEF_TRUETYPE		: DescribeFontDefTrueType(pCXaraFileRecord, pStr); break;
		case TAG_FONT_DEF_ATM			: DescribeFontDefATM(pCXaraFileRecord, pStr); break;
		default							: DescribeFontDefInvalid(pCXaraFileRecord, pStr); break;
		}
	}
}

void FontDefRecordHandler::DescribeFontDefTrueType(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR buf[256];
	TCHAR FontName[64];
	TCHAR FaceName[32];
	CCPanose MyCCPanose;
	
	pCXaraFileRecord->ReadUnicode(FontName, 64);
	pCXaraFileRecord->ReadUnicode(FaceName, 32);
	pCXaraFileRecord->ReadCCPanose(&MyCCPanose);

	_stprintf(buf, "Font Definition - Truetype\r\n");
	(*pStr) += buf;
	_stprintf(buf, "Full font name - %s\r\n", FontName);
	(*pStr) += buf;
	_stprintf(buf, "TypeFace name - %s\r\n", FaceName);
	(*pStr) += buf;

	MyCCPanose.GetDebugDetails(pStr);
	
}

void FontDefRecordHandler::DescribeFontDefATM(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR buf[256];
	TCHAR FontName[64];
	TCHAR FaceName[32];
	CCPanose MyCCPanose;
	
	pCXaraFileRecord->ReadUnicode(FontName, 64);
	pCXaraFileRecord->ReadUnicode(FaceName, 32);
	pCXaraFileRecord->ReadCCPanose(&MyCCPanose);

	_stprintf(buf, "Font Definition - ATM\r\n");
	(*pStr) += buf;
	_stprintf(buf, "Full font name - %s\r\n", FontName);
	(*pStr) += buf;
	_stprintf(buf, "TypeFace name - %s\r\n", FaceName);
	(*pStr) += buf;

	MyCCPanose.GetDebugDetails(pStr);
	
}

void FontDefRecordHandler::DescribeFontDefInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr)
{
	TCHAR s[256];

	_stprintf(s, "Font Definition - Invalid\r\n");
	(*pStr) += s;
}

#endif	// _DEBUG
