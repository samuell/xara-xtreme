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
// ********* Text objects for CDR files

/*
*/

#include "camtypes.h"
#include "cdrfiltr.h"
#include "nodetxts.h"
#include "nodetxtl.h"
#include "nodetext.h"
#include "fixmem.h"
#include "txtattr.h"
#include "app.h"
#include "fontman.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW

static TCHAR *CDRDefaultFontName = "AvantGarde Bk BT";
static CDRTextStyle CDRDefaultTextStyle = {0, (72000*24)/72, FALSE, FALSE, FALSE, CDRSCRIPT_NONE,
			(1000*24)/72, 100, JLEFT};

/********************************************************************************************

>	BOOL CDRFilter::ConvertText(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	converts a text object to a node. If the object has an unexpected
				format, then FormatError is set and it return *TRUE* - the return value only
				indicated environmental errors such as lack of memory
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ConvertText(cdrfOffsetHeader *Header)
{
	// check to see if this has an offset for text on a path
	if(FindDataInObject(Header, cdrfOBJOFFSETTYPE_PATHTEXT1) != 0
			&& FindDataInObject(Header, cdrfOBJOFFSETTYPE_PATHTEXT2) != 0)
	{
		IsTextOnAPath = TRUE;
	}
	else
	{
		IsTextOnAPath = FALSE;
	}

	// hey, this is text!
	IsText = TRUE;
	IsTextStory = FALSE;

	// dispatch conversion to appropriate routine
	if(Version == CDRVERSION_3)
		return ConvertText3(Header);

	if(Version == CDRVERSION_4)
	{
		if(CDRDATA_WORD(Header->ObjectType) == cdrfOBJTYPE_TEXT)
		{
			return ConvertText4Art(Header);
		}
		else
		{
			return ConvertText4Para(Header);
		}
	}

	// if it's a text story we're fiddling with, see how big the frame is
	INT32 FrameX = 0, FrameY = 0;
	DocCoord Trans = DocCoord(0,0);
	if(Header->ObjectType == cdrfOBJTYPE_TEXTSTORY)
	{
		// find the size of the object
		cdrfTextStoryCoordData *Coords = (cdrfTextStoryCoordData *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);
	
		if(Coords == 0)
		{
			FormatError = TRUE;
			return TRUE;
		}

		FrameX = CDRDATA_WORD(Coords->XSize);
		FrameY = CDRDATA_WORD(Coords->YSize);
TRACEUSER( "Ben", _T("Initial frame dimensions: X = %d Y = %d\n"), FrameX, FrameY);

		IsTextStory = TRUE;
	}
	else
	{
		IsTextStory = FALSE;
	}

	// OK, the header supplied here is in a block of it's own so we can aquire a new RIFF block
	// for the text info

	// the RIFF object will be at a loda chunk, which is at a one higher level than that of
	// the txsm chunk - find level we don't want to seek over
	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK || RIFF->GetObjChunkType() != cdrT_loda,
		"CDFFilter::ConvertText called with RIFF on a non-loda chunk");
	UINT32 EndLevel = RIFF->GetObjLevel() - 1;

	// now stroll though the RIFF file
	// stop when we find a txsm chunk, and store any trfd chunks we might meet on the way
	BOOL Found = FALSE;
	do
	{
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK && RIFF->GetObjChunkType() == cdrT_txsm)
		{
			// found the text info thingy
			Found = TRUE;
		}
		else if(RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART && RIFF->GetObjChunkType() == cdrT_trfl)
		{
			// grab the list contents
			if(!RIFF->GetListContents(&TransformChunk, &TransformChunkSize))
				return FALSE;
		}

		UpdateProgress();

		if(!Found)
			if(!RIFF->NextObject())
				return FALSE;
	
	} while(Found == FALSE && (RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() >= EndLevel));

	if(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK || RIFF->GetObjChunkType() != cdrT_txsm)
	{
		FormatError = TRUE;
		return TRUE;
	}

	// get size of chunk
	INT32 TextInfoSize = RIFF->GetObjSize();
	
	// aquire the chunk data
	if(!RIFF->AquireChunkData())
		return FALSE;

	// and get a pointer to the 'nice' info chunk
	ADDR TextInfo;
	if((TextInfo = RIFF->GetAquiredData()) == 0)
	{
		return FALSE;
	}
	cdrfTextInfoHdr *TIH = (cdrfTextInfoHdr *)TextInfo;

	// make a new text story node
	TextStory *TSNode = new TextStory;

	if(TSNode == 0)
		return FALSE;
	
	// get the number of paragraphs
	UINT32 NParagraphs = CDRDATA_WORD(TIH->NParagraphs);

	// set up some variables
	INT32 Loc = sizeof(cdrfTextInfoHdr);
	INT32 FirstLineSpace = -1;
	INT32 InitialFirstLineSpace = -1;
	INT32 ThisLineSpace = 0;
	INT32 PreviousLineSpace = 0;
	INT32 MaximumLineX = 0;		// MaximumLineX and
	INT32 Lines = 1;				// Lines only used for artisitic text
	Justification Just;			// the justification

	// convert the paragraph
	UINT32 CurrentPara;
	INT32 LineX;
	for(CurrentPara = 0; CurrentPara < NParagraphs; CurrentPara++)
	{
		// set up a few things
		LineX = 0;			// current position in the line
		
		// check sizes
		if((Loc + (INT32)sizeof(cdrfTextInfoParaHdr)) > TextInfoSize)
			break;
	
		// get paragraph header info
		cdrfTextInfoParaHdr *ParaHdr = (cdrfTextInfoParaHdr *)(TextInfo + Loc);
		Loc += sizeof(cdrfTextInfoParaHdr);

		// sort out font style definitions
		INT32 NFontDefns = CDRDATA_WORD(ParaHdr->NFontDefns);

		if((Loc + ((INT32)sizeof(cdrfTextInfoFontDefn) * NFontDefns)) > TextInfoSize)
			break;
	
		cdrfTextInfoFontDefn *FontDefns = (cdrfTextInfoFontDefn *)(TextInfo + Loc);
		Loc += (sizeof(cdrfTextInfoFontDefn) * NFontDefns);

		// get some memory for the converted font styles
		if(NFontDefns <= 0)
			break;

		CDRTextStyle *Styles = new CDRTextStyle[NFontDefns];
		
		// convert the first definiton from the style
		if(!GetTextStyleFromCDRStyle(&Styles[0], ParaHdr->Style))
			goto NoMemory;

		// store the justificationness
		Just = Styles[0].Just;

		// just in case of random Corel behaviour...
		PreviousLineSpace = (Styles[0].FontSize * 12) / 10;

		// go through translating all the other styles
		for(INT32 l = 1; l < NFontDefns; l++)
		{
			if(!GetTextStyleFromDefn(&Styles[l], &FontDefns[l], &Styles[0]))
				return FALSE;
		}

		// get the paragraph info
		if((Loc + (INT32)sizeof(cdrfTextInfoParaInfo)) > TextInfoSize)
			break;
		
		cdrfTextInfoParaInfo *PInfo = (cdrfTextInfoParaInfo *)(TextInfo + Loc);

		Loc += sizeof(cdrfTextInfoParaInfo);
		
		INT32 NChars = CDRDATA_WORD(PInfo->NChars);

		// sort out the character array
		if((Loc + ((INT32)sizeof(cdrfTextInfoChar) * NChars)) > TextInfoSize)
			break;
		
		cdrfTextInfoChar *Chars = (cdrfTextInfoChar *)(TextInfo + Loc);

		Loc += (sizeof(cdrfTextInfoChar) * NChars);
	
		// make a new text line for this paragraph
		TextLine *CurrentLine = new TextLine(TSNode, LASTCHILD);
		if(CurrentLine == 0)
			goto NoMemory;

		// and run through adding all those nice characters
		INT32 CurrentChar;
		for(CurrentChar = 0; CurrentChar < NChars; CurrentChar++)
		{
			BOOL NewLine = FALSE;		// whether to create a new line here

			if(IsTextStory == FALSE && CDRDATA_WORD(Chars[CurrentChar].Code) == cdrfTEXT_NEWLINE)
			{
				// update a couple of artisitic text only variables
				Lines++;

				if(LineX > MaximumLineX)
					MaximumLineX = LineX;

				// create a new line
				NewLine = TRUE;
			}
			
			// find the style of the character
			INT32 Style = ((CDRDATA_WORD(Chars[CurrentChar].Info)) & cdrfTEXTINFOCHAR_INFO_DEFNMASK)
						>> cdrfTEXTINFOCHAR_INFO_DEFNMASKSBY;

			// create it
			if(CDRDATA_WORD(Chars[CurrentChar].Code) >= ' ')
			{
				TextChar *C = new TextChar(CurrentLine, LASTCHILD, CDRDATA_WORD(Chars[CurrentChar].Code));

				if(C == 0)
					goto NoMemory;

				if(Style < 0 || Style >= NFontDefns)
					Style = 0;

				// apply attributes
				if(!ApplyTextAttr(C, &Styles[Style], 0 /*&Styles[0]*/))		// for now, we can't base attributes on anything 'cos the optimiser can't hack it
					goto NoMemory;
			}

			// add the character width
			LineX += CDRDATA_SWORD(Chars[CurrentChar].XSize);

			// update this line's spacing thingy
			if(Styles[Style].LineSpace > ThisLineSpace)
				ThisLineSpace = Styles[Style].LineSpace;

			// sort out first line spaceing thing
			if(InitialFirstLineSpace == -1)
				InitialFirstLineSpace = Styles[Style].LineSpace;

			if(NewLine)
			{
				// apply the line spacing attribute - make a font size attribute
				if(ThisLineSpace < 512)
					ThisLineSpace = PreviousLineSpace;
				TxtLineSpaceAttribute Attr((FIXED16)(((double)Styles[0].ParaLineSpacePercent) /* * 1.2*/ / 100));

				// attach
				if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
					return FALSE;

				// add an EOL
				EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);
				if(EOL == 0)
					goto NoMemory;
				// make a new line
				CurrentLine = new TextLine(TSNode, LASTCHILD);
				if(CurrentLine == 0)
					goto NoMemory;

				// sort out first line spacing thing
				if(FirstLineSpace == -1)
					FirstLineSpace = ThisLineSpace;

				// reset things
				LineX = 0;
				PreviousLineSpace = ThisLineSpace;
				ThisLineSpace = 0;
			}
		}

		// sort out first line spaceing thing
		if(FirstLineSpace == -1)
			FirstLineSpace = ThisLineSpace;

		// apply the line spacing attribute - make a font size attribute
		if(ThisLineSpace < 512)
			ThisLineSpace = PreviousLineSpace;
		TxtLineSpaceAttribute Attr((FIXED16)(((double)Styles[0].ParaLineSpacePercent) /** 1.2*/ / 100));
	
		// attach
		if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
			return FALSE;

		// pop on an end of line node
		EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);

		if(EOL == 0)
			goto NoMemory;

		// get rid of the font styles
		delete [] Styles;

		PreviousLineSpace = ThisLineSpace;
	}

	// pop a caret at the end of the last line
	{
		Node *LastLine = TSNode->FindLastChild();

		if(LastLine == 0)
			goto FormError;

		Node *LastLineNode = LastLine->FindLastChild();

		ERROR3IF(LastLineNode == 0, "Line node doesn't have any children");
		ERROR3IF(!LastLineNode->IsKindOf(CC_RUNTIME_CLASS(EOLNode)), "Last entry of a line is not an EOL node");
		
		CaretNode *C = new CaretNode(LastLineNode, PREV);
		if(C == 0)
			goto NoMemory;
	}

	// set up maximumlinex - only valid for artisitic text
	if(LineX > MaximumLineX)
		MaximumLineX = LineX;

	// set up a bounding box for this text object and
	// if this is a text story, translate it down a bit
	// so that the top of the first line is at the top of the frame
	if(IsTextStory)
	{
		if(FirstLineSpace == -1)
			goto FormError;			// no first line? How strange...

		// work out the distance to move the text line down = the first line un 120%ed
		INT32 Down = (FirstLineSpace * 10) / 12;

		// work out the rightness shifting
		INT32 Right = 0;
		if(Just == JCENTRE)
		{
			Right = (FrameX / 2) * CDRCOORDS_TO_MILLIPOINTS;
		}
		else if(Just == JRIGHT)
		{
			Right = FrameX * CDRCOORDS_TO_MILLIPOINTS;
		}

		// transform the story
		TSNode->SetStoryMatrix(Matrix(Trans.x+Right, Trans.y-Down));

		// set the format width
		TSNode->SetImportFormatWidth(FrameX * CDRCOORDS_TO_MILLIPOINTS);
TRACEUSER( "Ben", _T("Import format width initially %d\n"), TSNode->GetImportFormatWidth());

		// bounding box for text story
		DocRect BBox = DocRect(Trans.x, Trans.y - (FrameY * CDRCOORDS_TO_MILLIPOINTS),
				Trans.x + (FrameX * CDRCOORDS_TO_MILLIPOINTS), Trans.y);

		TextBBoxes.Add(TSNode, &BBox, JLEFT);
	}
	else
	{
		// set the matrix of the story
		TSNode->SetStoryMatrix(Matrix(0,0));

		// bounding box for a text object
		DocRect BBox = DocRect(0, 0 - FirstLineSpace * Lines,
				(MaximumLineX * CDRCOORDS_TO_MILLIPOINTS), FirstLineSpace);

		TextBBoxes.Add(TSNode, &BBox, Just);

		// see if this is linked to a path
		CheckTextForLinks(TSNode, Header);
	}

	// set some essential variables
	ObjFilled = TRUE;
	ObjStroked = FALSE;

	// all done return...
	pMadeNode = TSNode;

	return TRUE;

NoMemory:
	TSNode->CascadeDelete();
	delete TSNode;
	
	return FALSE;

FormError:
	FormatError = TRUE;
	TSNode->CascadeDelete();
	delete TSNode;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ConvertText4Art(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	converts a version 4 artisitic text object
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ConvertText4Art(cdrfOffsetHeader *Header)
{
	// get the data from this object
	cdrfTextHeaderV4 *Hdr = (cdrfTextHeaderV4 *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// it has got some coords, hasn't it?
	if(Hdr == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}
	// find pointer to the characters
	cdrfTextCharV4 *ThisChar = (cdrfTextCharV4 *)(Hdr + 1);

	// get the base style
	CDRTextStyle BaseStyle;

	WORD *StyleRef = (WORD *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_STYLE);
	if(StyleRef == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	if(!GetTextStyleFromCDRStyle(&BaseStyle, CDRDATA_WORD(*StyleRef)))
		return FALSE;

	// make a new text story node
	TextStory *TSNode = new TextStory;

	if(TSNode == 0)
		return FALSE;

	// run through the text...
	INT32 NChars = CDRDATA_WORD(Hdr->NCharacters);
	INT32 CurrentChar;
	INT32 MaximumLineSpace = BaseStyle.LineSpace;
	INT32 Lines = 1;
	INT32 Depth = 0;

	// make a new text line
	TextLine *CurrentLine = new TextLine(TSNode, LASTCHILD);
	if(CurrentLine == 0)
		goto NoMemory;

	for(CurrentChar = 0; CurrentChar < NChars; CurrentChar++)
	{
		BOOL HasStyles = FALSE;

		if(ThisChar->Info != 0)
			HasStyles = TRUE;

		if(ThisChar->Code >= ' ')
		{
			TextChar *C = new TextChar(CurrentLine, LASTCHILD, ThisChar->Code);

			if(C == 0)
				goto NoMemory;

			// apply attributes
			if(HasStyles)
			{
				// it's got styles of it's own, find and apply them
				cdrfTextCharStyledV4 *SC = (cdrfTextCharStyledV4 *)ThisChar;

				CDRTextStyle ThisStyle;

				if(!GetTextStyleFromChar4(&ThisStyle, SC, &BaseStyle))
					return FALSE;

				if(!ApplyTextAttr(C, &ThisStyle, 0))
					goto NoMemory;
		
				if(ThisStyle.LineSpace > MaximumLineSpace)
					MaximumLineSpace = ThisStyle.LineSpace;
			}
			else
			{
				// just apply the normal attributes
				if(!ApplyTextAttr(C, &BaseStyle, 0))
					goto NoMemory;
			}
		}

		if(ThisChar->Code == cdrfTEXT_NEWLINE)
		{
			// create a new line...
			// apply the line spacing attribute - make a font size attribute
//			TxtLineSpaceAttribute Attr(MaximumLineSpace);
			TxtLineSpaceAttribute Attr((FIXED16)(((double)BaseStyle.ParaLineSpacePercent) /** 1.2*/ / 100));

			// attach
			if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
				return FALSE;

			// add an EOL
			EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);
			if(EOL == 0)
				goto NoMemory;

			// make a new line
			CurrentLine = new TextLine(TSNode, LASTCHILD);
			if(CurrentLine == 0)
				goto NoMemory;

			// set things
			Lines++;
			Depth += MaximumLineSpace;
			MaximumLineSpace = BaseStyle.LineSpace;
		}

		if(HasStyles)
		{
			// it's got more than normal bytes in it, so add a few more than usual
			ThisChar = (cdrfTextCharV4 *)(((cdrfTextCharStyledV4 *)ThisChar) + 1);
		}
		else
		{
			// just a normal character, so simply increment it
			ThisChar++;
		}
	}

	// pop a line space attribute on the last line
	{
//		TxtLineSpaceAttribute Attr(MaximumLineSpace);
		TxtLineSpaceAttribute Attr((FIXED16)(((double)BaseStyle.ParaLineSpacePercent) /** 1.2*/ / 100));

		// attach
		if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
			return FALSE;
	}

	Depth += MaximumLineSpace;

	// finish the last line with an end of line node
	{
		EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);

		if(EOL == 0)
			goto NoMemory;

		// pop a caret at the end of the last line
		Node *LastLine = TSNode->FindLastChild();

		if(LastLine == 0)
			goto FormError;

		Node *LastLineNode = LastLine->FindLastChild();

		ERROR3IF(LastLineNode == 0, "Line node doesn't have any children");
		ERROR3IF(!LastLineNode->IsKindOf(CC_RUNTIME_CLASS(EOLNode)), "Last entry of a line is not an EOL node");
		
		CaretNode *C = new CaretNode(LastLineNode, PREV);
		if(C == 0)
			goto NoMemory;
	}

	// set the matrix of the story
	{
		TSNode->SetStoryMatrix(Matrix(0,0));

		// bounding box for a artisitic text object - approximate it's width
		INT32 Width = (MaximumLineSpace * NChars) / (Lines * 3);

		DocRect BBox = DocRect(0, 0 - Depth, Width, MaximumLineSpace);

		TextBBoxes.Add(TSNode, &BBox, BaseStyle.Just);
	}

	// check it for paths...
	CheckTextForLinks(TSNode, Header);
	
	// set some essential variables
	ObjFilled = TRUE;
	ObjStroked = FALSE;
	IsTextStory = FALSE;

	// all done, return...
	pMadeNode = TSNode;

	return TRUE;

NoMemory:
	TSNode->CascadeDelete();
	delete TSNode;
	
	return FALSE;

FormError:
	FormatError = TRUE;
	TSNode->CascadeDelete();
	delete TSNode;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ConvertText4Para(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	converts a version 4 paragraph text object
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ConvertText4Para(cdrfOffsetHeader *Header)
{
	// get the data from this object
	cdrfParaTextHeaderV4 *Hdr = (cdrfParaTextHeaderV4 *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// set the var...
	IsTextStory = TRUE;

	// it has got some coords, hasn't it?
	if(Hdr == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}


	// find out what text item we need
	if(LinkTable == 0)
	{
		// no link table, oh dear
		FormatError = TRUE;
		return TRUE;
	}

	cdrfLinkTableHdr *th = (cdrfLinkTableHdr *)LinkTable;

	INT32 Entries = CDRDATA_WORD(th->Entries);
	BOOL Found = FALSE;
	WORD *Offsets = (WORD *)(LinkTable + CDRDATA_WORD(th->OffsetsOffset));
	cdrfLinkTableEntryTextV4 *En;
	for(INT32 l = 0; l < Entries; l++)
	{
		En = (cdrfLinkTableEntryTextV4 *)(LinkTable + CDRDATA_WORD(Offsets[l]));
	
		if(CDRDATA_WORD(En->Type) == cdrfLINKTABLEENTRYV4_TEXT &&
				CDRDATA_WORD(En->ObjectSerial) == SerialNumber)
		{
			Found = TRUE;
			break;
		}
	}

	if(Found == FALSE)
	{
		FormatError = TRUE;
		return TRUE;
	}
	
	// locate the text item with those attributes
	CDRVectorStoredItem *Item = (CDRVectorStoredItem *)TextV4.GetHead();
	
	// go through the list of items searching for the reference number given
	while(Item != 0)
	{
		if(Item->Reference == CDRDATA_WORD(En->TextID))
		{
			// OK, we found it...
			break;
		}

		Item = (CDRVectorStoredItem *)TextV4.GetNext(Item);
	}

	if(Item == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	Node *Text = Item->Objects;

	// find the base attributes
	CDRTextStyle BaseStyle;

	WORD *StyleRef = (WORD *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_STYLE);
	if(StyleRef == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}

	if(!GetTextStyleFromCDRStyle(&BaseStyle, CDRDATA_WORD(*StyleRef)))
		return FALSE;
	
	// get the maximum height of the first line
	ERROR2IF(!IS_A(Text, TextLine), FALSE, "Node in text list is not a text line");

	// apply attributes to the lines
	TextLine *CurrentLine = (TextLine *)Text;
	while(CurrentLine != 0)
	{
		if(IS_A(CurrentLine, TextLine))
		{
/*			Node *pNode = CurrentLine->FindFirstChild();

			while(pNode != 0)
			{
				if(IS_A(pNode, TextChar))
				{
					if(!ApplyTextAttr(pNode, &BaseStyle, 0))
						return FALSE;
				}

				pNode = pNode->FindNext();
			}
*/
			// attach the line spacing onto it
			TxtLineSpaceAttribute Attr((FIXED16)(((double)BaseStyle.ParaLineSpacePercent) /** 1.2*/ / 100));

			if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
				return FALSE;
		}
		
		CurrentLine = (TextLine *)CurrentLine->FindNext();
	}

	// add it to a text story
	TextStory *TSNode = new TextStory;
	if(TSNode == 0)
		return FALSE;

	Text->InsertChainSimple(TSNode, FIRSTCHILD);

	// remove references to the nodes now inserted from the item
	Item->Objects = 0;

	// set the matrix and add the bbox
	{
		INT32 FirstLineSpace = Item->BBox.hi.y;		// get the line spacing from the bbox defn

		// get the frame size
		INT32 FrameX = CDRDATA_WORD(Hdr->FrameX) * CDRCOORDS_TO_MILLIPOINTS;
		INT32 FrameY = CDRDATA_WORD(Hdr->FrameY) * CDRCOORDS_TO_MILLIPOINTS;

		// work out the distance to move the text line down = the first line un 120%ed
		INT32 Down = (FirstLineSpace * 10) / 12;

		// work out the shift...
		INT32 Right = 0;
		if(BaseStyle.Just == JCENTRE)
		{
			Right = FrameX / 2;
		}
		else if(BaseStyle.Just == JRIGHT)
		{
			Right = FrameX;
		}

		
		// transform the story
		TSNode->SetStoryMatrix(Matrix(Right,-Down));

		// bounding box for text story
		DocRect BBox = DocRect(0, 0 - FrameY, FrameX, 0);

		TextBBoxes.Add(TSNode, &BBox, JLEFT);

		// set it's width to be formatted to
		TSNode->SetImportFormatWidth(FrameX);
	}

	// delete the item
	delete TextV4.RemoveItem(Item);

	// set a few variables
	pMadeNode = TSNode;
	ObjFilled = TRUE;
	ObjStroked = FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::GetTextStyleFromChar4(CDRTextStyle *TS, cdrfTextCharStyledV4 *Char, CDRTextStyle *BasedOn)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	gets a text style from a styled version 4 text character
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::GetTextStyleFromChar4(CDRTextStyle *TS, cdrfTextCharStyledV4 *Char, CDRTextStyle *BasedOn)
{
	if(BasedOn != 0)
		*TS = *BasedOn;
	else
		*TS = CDRDefaultTextStyle;

	// font name
	if((CDRDATA_WORD(Char->Changes) & cdrfSTYLECHANGEV4_FONTNAME) != 0)
	{
/*		TCHAR *FontName = GetFontName(Char->FontRef);
		if(FontName != 0)
		{
			TS->FontName = FontName;
		}
*/
		TS->FontReference = CDRDATA_DWORD(Char->FontRef);
	}

	// font size
	if((CDRDATA_WORD(Char->Changes) & cdrfSTYLECHANGEV4_FONTSIZE) != 0)
	{
		if(CDRDATA_WORD(Char->FontSize) != 0)
		{
			TS->FontSize = CDRDATA_WORD(Char->FontSize) * CDRCOORDS_TO_MILLIPOINTS;
		}
	}

	// update line spacing
 	TS->LineSpace = (TS->FontSize * (12 * TS->ParaLineSpacePercent)) / 1000;

	// weight
	if((CDRDATA_WORD(Char->Changes) & cdrfSTYLECHANGEV4_WEIGHT) != 0)
	{
		switch(CDRDATA_WORD(Char->FontType))
		{
			case cdrfFONTTYPEV4_BOLD:
				TS->Bold = TRUE;
				TS->Italic = FALSE;
				break;

			case cdrfFONTTYPEV4_ITALIC:
				TS->Bold = FALSE;
				TS->Italic = TRUE;
				break;

			case cdrfFONTTYPEV4_BOLDITALIC:
				TS->Bold = TRUE;
				TS->Italic = TRUE;
				break;

			default:
				TS->Bold = FALSE;
				TS->Italic = FALSE;
				break;
		}
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL CDRFilter::ConvertText3(cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24 03 95
	Inputs:		none
	Returns:	error flag
	Purpose:	converts a version 3 text object to a node. If the object has an unexpected
				format, then FormatError is set and it return *TRUE* - the return value only
				indicated environmental errors such as lack of memory
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ConvertText3(cdrfOffsetHeader *Header)
{
	// is it a text story?
	if(Header->ObjectType == (cdrfOBJTYPE_TEXTSTORY - cdrfOBJTYPE_V3ADD))
		IsTextStory = TRUE;

	// get the data from this object
	cdrfTextHeaderV3 *Hdr = (cdrfTextHeaderV3 *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_COORDS);

	// it has got some coords, hasn't it?
	if(Hdr == 0)
	{
		FormatError = TRUE;
		return TRUE;
	}
//TRACEUSER( "Ben", _T("FrameX = %d, FrameY = %d\n"), Hdr->FrameX, Hdr->FrameY);

	INT32 FrameX = CDRDATA_WORD(Hdr->FrameX) * CDRCOORDS_TO_MILLIPOINTS;
	INT32 FrameY = CDRDATA_WORD(Hdr->FrameY) * CDRCOORDS_TO_MILLIPOINTS;

	// find pointer to the characters
	cdrfTextCharV3 *ThisChar = (cdrfTextCharV3 *)(Hdr + 1);

	// get the base style
	CDRTextStyle BaseStyle;

	if(!GetTextStyle3(&BaseStyle, CDRDATA_WORD(Hdr->BaseFontStyle)))
		return FALSE;

	// add the justifcation stuff to it
	TRACEUSER( "Ben", _T("Justificatoin V3 = %d\n"), Hdr->Justification);
	switch(Hdr->Justification)
	{
		case cdrfJUSTIFICATIONV3_LEFT:
		default:
			BaseStyle.Just = JLEFT;
			break;

		case cdrfJUSTIFICATIONV3_CENTRE:
			BaseStyle.Just = JCENTRE;
			break;

		case cdrfJUSTIFICATIONV3_RIGHT:
		 	BaseStyle.Just = JRIGHT;
			break;
	}
	
	// make a new text story node
	TextStory *TSNode = new TextStory;

	if(TSNode == 0)
		return FALSE;

	// run through the text...
	INT32 NChars = CDRDATA_WORD(Hdr->NCharacters);
	INT32 CurrentChar;
	INT32 MaximumLineSpace = BaseStyle.LineSpace;
	INT32 Lines = 1;
	INT32 Depth = 0;

	// make a new text line
	TextLine *CurrentLine = new TextLine(TSNode, LASTCHILD);
	if(CurrentLine == 0)
		goto NoMemory;

	for(CurrentChar = 0; CurrentChar < NChars; CurrentChar++)
	{
		BOOL HasStyles = FALSE;

		if(ThisChar->Info != 0)
			HasStyles = TRUE;

		if(ThisChar->Code >= ' ')
		{
			TextChar *C = new TextChar(CurrentLine, LASTCHILD, ThisChar->Code);

			if(C == 0)
				goto NoMemory;

			// apply attributes
			if(HasStyles)
			{
				// it's got styles of it's own, find and apply them
				cdrfTextCharStyledV3 *SC = (cdrfTextCharStyledV3 *)ThisChar;

				CDRTextStyle ThisStyle;

				if(!GetTextStyle3(&ThisStyle, CDRDATA_WORD(SC->Style), &BaseStyle))
					return FALSE;

				if(!ApplyTextAttr(C, &ThisStyle, 0))
					goto NoMemory;
		
				if(ThisStyle.LineSpace > MaximumLineSpace)
					MaximumLineSpace = ThisStyle.LineSpace;
			}
			else
			{
				// just apply the normal attributes
				if(!ApplyTextAttr(C, &BaseStyle, 0))
					goto NoMemory;
			}
		}

		if(ThisChar->Code == cdrfTEXT_NEWLINE)
		{
			// create a new line...
			// apply the line spacing attribute - make a font size attribute
//			TxtLineSpaceAttribute Attr(MaximumLineSpace);
			TxtLineSpaceAttribute Attr((FIXED16)(((double)BaseStyle.ParaLineSpacePercent) /** 1.2*/ / 100));

			// attach
			if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
				return FALSE;

			// add an EOL
			EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);
			if(EOL == 0)
				goto NoMemory;

			// make a new line
			CurrentLine = new TextLine(TSNode, LASTCHILD);
			if(CurrentLine == 0)
				goto NoMemory;

			// set things
			Lines++;
			Depth += MaximumLineSpace;
			MaximumLineSpace = BaseStyle.LineSpace;
		}

		if(HasStyles)
		{
//TRACEUSER( "Ben", _T("Char has styles, code %d (%c)\n"), ThisChar->Code, ThisChar->Code); 
			// it's got more than normal bytes in it, so add a few more than usual
			ThisChar = (cdrfTextCharV3 *)(((cdrfTextCharStyledV3 *)ThisChar) + 1);
		}
		else
		{
//TRACEUSER( "Ben", _T("Char, code %d (%c)\n"), ThisChar->Code, ThisChar->Code); 
			// just a normal character, so simply increment it
			ThisChar++;
		}
	}

	// pop a line space attribute on the last line
	{
//		TxtLineSpaceAttribute Attr(MaximumLineSpace);
		TxtLineSpaceAttribute Attr((FIXED16)(((double)BaseStyle.ParaLineSpacePercent) /** 1.2*/ / 100));

		// attach
		if(!ApplyTextAttrDoApply(CurrentLine, &Attr, FALSE))
			return FALSE;
	}

	Depth += MaximumLineSpace;

	// finish the last line with an end of line node
	{
		EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);

		if(EOL == 0)
			goto NoMemory;

		// pop a caret at the end of the last line
		Node *LastLine = TSNode->FindLastChild();

		if(LastLine == 0)
			goto FormError;

		Node *LastLineNode = LastLine->FindLastChild();

		ERROR3IF(LastLineNode == 0, "Line node doesn't have any children");
		ERROR3IF(!LastLineNode->IsKindOf(CC_RUNTIME_CLASS(EOLNode)), "Last entry of a line is not an EOL node");
		
		CaretNode *C = new CaretNode(LastLineNode, PREV);
		if(C == 0)
			goto NoMemory;
	}

	if(IsTextStory)
	{
		// get a first line height
		INT32 FirstLineHeight = (BaseStyle.LineSpace * 10) / 12;

		// work out the rightness shifting
		INT32 Right = 0;
		if(BaseStyle.Just == JCENTRE)
		{
			Right = FrameX / 2;
		}
		else if(BaseStyle.Just == JRIGHT)
		{
			Right = FrameX;
		}

		// set the matrix of the story
		TSNode->SetStoryMatrix(Matrix(Right, -FirstLineHeight));

		// bounding box for a text frame
		DocRect BBox = DocRect(0, 0 - FrameY, FrameX, 0);

		TextBBoxes.Add(TSNode, &BBox, JLEFT);
	
		// set it's width to be formatted to
		TSNode->SetImportFormatWidth(FrameX);
	}
	else
	{
		// set the matrix of the story
		TSNode->SetStoryMatrix(Matrix(0,0));

		// bounding box for a artisitic text object - approximate it's width
		INT32 Width = (MaximumLineSpace * NChars) / (Lines * 3);

		DocRect BBox = DocRect(0, 0 - Depth, Width, MaximumLineSpace);

		TextBBoxes.Add(TSNode, &BBox, BaseStyle.Just);
	
		// check it for path on a text
		CheckTextForLinks(TSNode, Header);
	}

	// set some essential variables
	ObjFilled = TRUE;
	ObjStroked = FALSE;

	// all done, return...
	pMadeNode = TSNode;

	return TRUE;

NoMemory:
	TSNode->CascadeDelete();
	delete TSNode;
	
	return FALSE;

FormError:
	FormatError = TRUE;
	TSNode->CascadeDelete();
	delete TSNode;
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::GetTextStyle3(CDRTextStyle *TS, WORD Style, CDRTextStyle *BasedOn = 0);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/05/95
	Inputs:		none
	Returns:	error flag
	Purpose:	gets a style for version 3 text from the global text styles table
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::GetTextStyle3(CDRTextStyle *TS, WORD Style, CDRTextStyle *BasedOn)
{
	// copy in the default text style
	if(BasedOn == 0)
		*TS = CDRDefaultTextStyle;
	else
		*TS = *BasedOn;

	// if the styles chunk isn't present, return now
	if(FontStylesV3 == 0)
		return TRUE;

	// find the font definition
	cdrfFontStyleTableHeaderV3 *Hdr = (cdrfFontStyleTableHeaderV3 *)FontStylesV3;

	if(Style >= CDRDATA_WORD(Hdr->NOffsets))
		return TRUE;		// if there aren't enough entries to find this style

	cdrfFontStyleTableEntryV3 *En = (cdrfFontStyleTableEntryV3 *)(FontStylesV3 + CDRDATA_WORD(Hdr->Offsets[Style]));

	// convert it

	// find the font name
/*	if(FontTableV3 == 0)
		return FALSE;

	cdrfFontTableEntryV3 *FEn = (cdrfFontTableEntryV3 *)FontTableV3;
	for(INT32 l = 0; l < FontTableEntriesV3; l++)
	{
		if(FEn[l].Reference == En->FontRef)		// don't need to bother with CDRDATA stuff
		{
			TS->FontName = (char *)FEn[l].Name;

			break;
		}
	}
*/
	// store the font reference
	TS->FontReference = CDRDATA_DWORD(En->FontRef);
TRACEUSER( "Ben", _T("Font reference = %d\n"), TS->FontReference);
	
	// font size
	TS->FontSize = CDRDATA_WORD(En->FontSize) * CDRCOORDS_TO_MILLIPOINTS;

	// style...
	INT32 S = CDRDATA_WORD(En->FontStyle) & cdrfFONTSTYLEV3_WEIGHT_MASK;
	switch(S)
	{
		case cdrfFONTSTYLEV3_WEIGHT_NORMAL:
		default:
			TS->Bold = FALSE;
			TS->Italic = FALSE;
			break;

		case cdrfFONTSTYLEV3_WEIGHT_BOLD:
			TS->Bold = TRUE;
			TS->Italic = FALSE;
			break;

		case cdrfFONTSTYLEV3_WEIGHT_ITALIC:
			TS->Bold = FALSE;
			TS->Italic = TRUE;
			break;

		case cdrfFONTSTYLEV3_WEIGHT_BOLDITALIC:
			TS->Bold = TRUE;
			TS->Italic = TRUE;
			break;
	}

	// super or subscriptness
	if((CDRDATA_WORD(En->FontStyle) & cdrfFONTSTYLEV3_FLAG_SUPERSCRIPT) != 0)
	{
		TS->Script = CDRSCRIPT_SUPER;
	}
	else if((CDRDATA_WORD(En->FontStyle) & cdrfFONTSTYLEV3_FLAG_SUPERSCRIPT) != 0)
	{
		TS->Script = CDRSCRIPT_SUB;
	}

	// the line spacing is a para wide thingy, so we need to reworkout the linespace value
 	// the line spacing value is a percentage of 120% of the font size
 	TS->LineSpace = (TS->FontSize * (12 * TS->ParaLineSpacePercent)) / 1000;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::GetTextStyleFromCDRStyle(CDRTextStyle *TS, WORD StyleRef, BOOL SearchForParent = TRUE)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		
	Returns:	error flag
	Purpose:	gets a text style from a CDR style. If it can't be found, you get the
				default text style. If SearchForParent is TRUE, the parent style of this
				style will be located
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::GetTextStyleFromCDRStyle(CDRTextStyle *TS, WORD StyleRef, BOOL SearchForParent)
{
	// copy in the default text style
	*TS = CDRDefaultTextStyle;

	// find the style
	INT32 Size;
	cdrfTextStyleHdr *Hdr = (cdrfTextStyleHdr *)Styles.Find(StyleRef, &Size);

	if(Hdr == 0)
		return TRUE;		// don't error if it can't be found

	// if necessary, search for the parent style...
	if(SearchForParent)
	{
		if(CDRDATA_WORD(Hdr->Hdr.ObjectType) < 0x7fff)
		{
			if(!GetTextStyleFromCDRStyle(TS, CDRDATA_WORD(Hdr->Hdr.ObjectType), FALSE))
				return FALSE;
		}
	}

	// find the font size and stuff.
	cdrfTextStyleFont *FontStyle = (cdrfTextStyleFont *)FindDataInObject(&Hdr->Hdr, cdrfTEXTSTYLE_OFFSETTYPE_FONT);
	if(FontStyle != 0)
	{	
		// set fontname
/*		TCHAR *FontName = GetFontName(CDRDATA_WORD(FontStyle->FontRef));
		if(FontName != 0)
			TS->FontName = FontName;
		else
			TS->FontName = CDRDefaultFontName;
*/
		// set font reference
		TS->FontReference = CDRDATA_WORD(FontStyle->FontRef);

		// set font size
		TS->FontSize = CDRDATA_WORD(FontStyle->FontSize) * CDRCOORDS_TO_MILLIPOINTS;
		
		// set font bold and italicness
		if(Version == CDRVERSION_5)
		{
			switch(CDRDATA_WORD(FontStyle->FontType))
			{
				case cdrfFONTTYPE_BOLD:
					TS->Bold = TRUE;
					TS->Italic = FALSE;
					break;

				case cdrfFONTTYPE_ITALIC:
					TS->Bold = FALSE;
					TS->Italic = TRUE;
					break;

				case cdrfFONTTYPE_BOLDITALIC:
					TS->Bold = TRUE;
					TS->Italic = TRUE;
					break;

				default:
					TS->Bold = FALSE;
					TS->Italic = FALSE;
					break;
			}
		}
		else
		{
			switch(CDRDATA_WORD(FontStyle->FontType))
			{
				case cdrfFONTTYPEV4_BOLD:
					TS->Bold = TRUE;
					TS->Italic = FALSE;
					break;

				case cdrfFONTTYPEV4_ITALIC:
					TS->Bold = FALSE;
					TS->Italic = TRUE;
					break;

				case cdrfFONTTYPEV4_BOLDITALIC:
					TS->Bold = TRUE;
					TS->Italic = TRUE;
					break;

				default:
					TS->Bold = FALSE;
					TS->Italic = FALSE;
					break;
			}
		}
	}

	// find the spacing info
	cdrfTextStyleSpacing *Spacing = (cdrfTextStyleSpacing *)FindDataInObject(&Hdr->Hdr, cdrfTEXTSTYLE_OFFSETTYPE_SPACING);
	if(Spacing != 0)
	{
		// set the paragraph line spacing
		TS->ParaLineSpacePercent = CDRDATA_SWORD(Spacing->LineSpacing);

	}

 	// work out the line spacing for this thingy
 	// the line spacing value is a percentage of 120% of the font size
 	TS->LineSpace = (TS->FontSize * (12 * TS->ParaLineSpacePercent)) / 1000;

	// find the alignment
	cdrfTextStyleAlignment *Align = (cdrfTextStyleAlignment *)FindDataInObject(&Hdr->Hdr, cdrfTEXTSTYLE_OFFSETTYPE_ALIGNMENT);
	if(Align != 0)
	{
		switch(CDRDATA_WORD(Align->Alignment))
		{
			case cdrfALIGNMENT_RIGHT:	TS->Just = JRIGHT;	break;
			case cdrfALIGNMENT_CENTRE:	TS->Just = JCENTRE;	break;
			case cdrfALIGNMENT_FULL:	TS->Just = JFULL;	break;
			default:					TS->Just = JLEFT;	break;
		}
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::GetTextStyleFromDefn(CDRTextStyle *TS, cdrfTextInfoFontDefn *FontDefn, const CDRTextStyle *BasedOn)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		
	Returns:	error flag
	Purpose:	gets a font style from a font definition stored in a txsm chunk. Anything
				not defined comes from the BasedOn style.
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::GetTextStyleFromDefn(CDRTextStyle *TS, cdrfTextInfoFontDefn *FontDefn, const CDRTextStyle *BasedOn)
{
	// set up the defaults
	if(BasedOn != 0)
		*TS = *BasedOn;
	else
		*TS = CDRDefaultTextStyle;

	// add in things which are changed...
	if((CDRDATA_WORD(FontDefn->Changes) & cdrfTEXTINFODEFNCHANGES_FONT) != 0)
	{
		// change the font reference
/*		TCHAR *NewName = GetFontName(CDRDATA_WORD(FontDefn->FontRef));

		if(NewName != 0)
			TS->FontName = NewName;
*/
		TS->FontReference = CDRDATA_WORD(FontDefn->FontRef);
	}

	if((CDRDATA_WORD(FontDefn->Changes) & cdrfTEXTINFODEFNCHANGES_SIZE) != 0)
	{
		// new font size
		TS->FontSize = CDRDATA_WORD(FontDefn->FontSize) * CDRCOORDS_TO_MILLIPOINTS;
	}

	if((CDRDATA_WORD(FontDefn->Changes) & cdrfTEXTINFODEFNCHANGES_ATTR) != 0)
	{
		// weightness of the font
		if(CDRDATA_WORD(FontDefn->FontType) != 0)
		{
			// set font bold and italicness
			switch(CDRDATA_WORD(FontDefn->FontType))
			{
				case cdrfFONTTYPE_BOLD:
					TS->Bold = TRUE;
					TS->Italic = FALSE;
					break;

				case cdrfFONTTYPE_ITALIC:
					TS->Bold = FALSE;
					TS->Italic = TRUE;
					break;

				case cdrfFONTTYPE_BOLDITALIC:
					TS->Bold = TRUE;
					TS->Italic = TRUE;
					break;

				default:
					TS->Bold = FALSE;
					TS->Italic = FALSE;
					break;
			}
		}
	
		// new attributes
		if(FontDefn->FontAttr.Underline != cdrfLINETYPE_NONE
				|| FontDefn->FontAttr.Overline != cdrfLINETYPE_NONE
				|| FontDefn->FontAttr.StrikeOut != cdrfLINETYPE_NONE)
		{
			TS->Underline = TRUE;
		}
		else
		{
			TS->Underline = FALSE;
		}

		switch(FontDefn->FontAttr.Placement)
		{
			default:							TS->Script = CDRSCRIPT_NONE;	break;
			case cdrfPLACEMENTTYPE_SUPERSCRIPT:	TS->Script = CDRSCRIPT_SUPER;	break;
			case cdrfPLACEMENTTYPE_SUBSCRIPT:	TS->Script = CDRSCRIPT_SUB;		break;
		}
	}

	// the line spacing is a para wide thingy, so we need to reworkout the linespace value
 	// the line spacing value is a percentage of 120% of the font size
 	TS->LineSpace = (TS->FontSize * (12 * TS->ParaLineSpacePercent)) / 1000;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ApplyTextAttr(Node *ContextNode, const CDRTextStyle *TS, const CDRTextStyle *BasedOn)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		
	Returns:	error flag
	Purpose:	Applies text attributes to a node from a CDRTextStyle. If BasedOn != 0 then
				the attributes will only be applied if they are different from those
				in *BasedOn.
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ApplyTextAttr(Node *ContextNode, const CDRTextStyle *TS, const CDRTextStyle *BasedOn)
{
	BOOL CheckExisting = FALSE;

	// if the node has children, then we want to check existing attributes
	if(ContextNode->FindFirstChild() != 0)
		CheckExisting = TRUE;

	// apply italicness
	if(BasedOn == 0 || TS->Italic != BasedOn->Italic)
	{
		// make a font size attribute
		TxtItalicAttribute Attr(TS->Italic);
	
		// attach
		if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
			return FALSE;
	}

	// apply boldness
	if(BasedOn == 0 || TS->Bold != BasedOn->Bold)
	{
		// make a font size attribute
		TxtBoldAttribute Attr(TS->Bold);
	
		// attach
		if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
			return FALSE;
	}

	// apply underline
	if(BasedOn == 0 || TS->Underline != BasedOn->Underline)
	{
		// make a font size attribute
		TxtUnderlineAttribute Attr(TS->Underline);
	
		// attach
		if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
			return FALSE;
	}

	// apply the scriptness
	if(BasedOn == 0 || TS->Script != BasedOn->Script)
	{
		if(TS->Script != CDRSCRIPT_NONE)
		{
			TxtScriptAttribute Attr((TS->Script == CDRSCRIPT_SUB)?-0.1:0.33, 0.5);

			// attach
			if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
				return FALSE;
		}
	}

	// apply font size
	if(BasedOn == 0 || TS->FontSize != BasedOn->FontSize)
	{
		// make a font size attribute
		TxtFontSizeAttribute Attr(TS->FontSize);
	
		// attach
		if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
			return FALSE;
	}

	// apply font name
//	if(BasedOn == 0 || TS->FontName != BasedOn->FontName)
	if(BasedOn == 0 || TS->FontReference != BasedOn->FontReference)
	{
		// get a font handle
		WORD hTypeface = Fonts.GetHandleForReference(TS->FontReference);
		//WORD hTypeface = FONTMANAGER->GetFontHandle(&String_64(TS->FontName)); 

		// make a typeface attribute
		TxtFontTypefaceAttribute Attr(hTypeface);
	
		// attach
		if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
			return FALSE;
	}

	// apply the justification stuff
	if((BasedOn == 0 || TS->Just != BasedOn->Just) && TS->Just != JLEFT)
	{
		// make a justification attribute
		TxtJustificationAttribute Attr(TS->Just);
	
		// attach
		if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
			return FALSE;
	}

	// slap on a aspect ratio thingy so it gets transformed OK
	TxtAspectRatioAttribute Attr(1);

	if(!ApplyTextAttrDoApply(ContextNode, &Attr, CheckExisting))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ApplyTextAttrDoApply(Node *ContextNode, TxtBaseClassAttribute *Attr, BOOL CheckExising)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		
	Returns:	error flag
	Purpose:	Given a text attribute, apply it to a node. If CheckExising is TRUE, the
				existing attributes will be checked, and this attribute will not be applied
				if one of it's type already exists.
	SeeAlso:	

********************************************************************************************/

BOOL CDRFilter::ApplyTextAttrDoApply(Node *ContextNode, TxtBaseClassAttribute *Attr, BOOL CheckExisting)
{
	// check given attribute
	if(Attr == 0)
		return FALSE;

	// make a node out of it
	NodeAttribute *pNode = Attr->MakeNode();
	if(pNode == 0)
		return FALSE;

	// if necessary check existing attributes
	if(CheckExisting)
	{
		Node *SearchNode = ContextNode->FindFirstChild();

		// run through it's children
		while(SearchNode != 0)
		{
			if(IS_SAME_CLASS(pNode, SearchNode))
			{
				// found an attribute just like the one we're thinking of applying...
				delete pNode;
				return TRUE;		// don't need to do anything more
			}

			SearchNode = SearchNode->FindNext();
		}
	}
	
	// attach the node
	pNode->AttachNode(ContextNode, FIRSTCHILD);
	return TRUE;
}


/********************************************************************************************

>	TCHAR *CDRFilter::GetFontName(WORD FontRef)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		
	Returns:	pointer to font name or 0 for not found
	Purpose:	finds a font name
	SeeAlso:	

********************************************************************************************/

TCHAR *CDRFilter::GetFontName(WORD FontRef)
{
	INT32 Size;
	cdrfFontDefn *FD = (cdrfFontDefn *)Fonts.Find(FontRef, &Size);

	if(Version == CDRVERSION_5)
	{
		if(FD != 0)
			return FD->Name;
	}
	else
	{
		cdrfFontDefnV4 *FD4 = (cdrfFontDefnV4 *)FD;

		if(FD4 != 0)
			return FD4->Name;
	}

	return 0;
}


/********************************************************************************************

>	BOOL CDRFontnameStore::AddChunkToStore(RIFFFile *RIFF, CDRVersion Version)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22 03 95
	Inputs:		A RIFFFile object
	Returns:	error flag
	Purpose:	Stores fontname chunks for the CDRFilter class and caches the name
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFontnameStore::AddChunkToStore(RIFFFile *RIFF, CDRVersion Version)
{
	if(RIFF->GetObjType() != RIFFOBJECTTYPE_CHUNK)
	{
		ERROR2(FALSE, "CDRAttributeStore::AddChunkToStore called without a chunk in the RIFFFile");
	}

	// get a new item obect
	CDRFontnameStoredItem *Item = new CDRFontnameStoredItem;

	if(Item == 0)
		return FALSE;

	// get the data of the RIFF chunk
	if(!Item->Aquire(RIFF))
	{
		delete Item;
		return FALSE;
	}

	Item->Size = RIFF->GetObjSize();

	// and add the new item to the list
	AddTail(Item);

	// cache the font name
	DWORD Reference;
	TCHAR *Name = 0;
	if(Version == CDRVERSION_5)
	{
		cdrfFontDefn *FD = (cdrfFontDefn *)Item->Block;

		if(FD == 0)
			return FALSE;

		Reference = FD->Reference;	
		Name = FD->Name;
	}
	else if(Version == CDRVERSION_4)
	{
		cdrfFontDefnV4 *FD = (cdrfFontDefnV4 *)Item->Block;

		if(FD == 0)
			return FALSE;
	
		Reference = FD->Reference;	
		Name = FD->Name;
	}

//	if(Name != 0)
//		FONTMANAGER->CacheNamedFont(&String_64(Name));

	// get the item ready...
	return Item->GetTypefaceForUse(Reference, Name);
}

/********************************************************************************************

>	BOOL CDRFontnameStore::AddFontToStore(DWORD Reference, TCHAR *Fontname)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03 01 96
	Inputs:		Fontname
	Returns:	error flag
	Purpose:	Adds another fontname to the list
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFontnameStore::AddFontToStore(DWORD Reference, TCHAR *Fontname)
{
TRACEUSER( "Ben", _T("**** attempting to add %s\n"), Fontname);
	// get a new item obect
	CDRFontnameStoredItem *Item = new CDRFontnameStoredItem;

	if(Item == 0)
		return FALSE;

	// and add the new item to the list
	AddTail(Item);

	// get the item ready...
	return Item->GetTypefaceForUse(Reference, Fontname);
}

/********************************************************************************************

>	virtual WORD CDRFontnameStore::GetHandleForReference(DWORD Reference)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03 01 96
	Inputs:		The reference of the interesting font
	Returns:	The handle of the font.
	Purpose:	To get a handle for use in an attribute given a CDR reference
	SeeAlso:	CDRFilter

********************************************************************************************/

WORD CDRFontnameStore::GetHandleForReference(DWORD Reference)
{
	// scan the list comparing references with the stored items.
	CDRFontnameStoredItem *Item;

	if(IsEmpty())
		return 0;		// no items in the list
	
	Item = (CDRFontnameStoredItem *)GetHead();

	// scan though the list looking for the reference
	while(Item != 0)
	{
		if(Item->Reference == Reference)
		{
			return Item->hTypeface;
		}

		Item = (CDRFontnameStoredItem *)GetNext(Item);
	}


	// nothing helpful found - return the default font handle
	return FONTMANAGER->GetFont()->GetFontHandle();
}

/********************************************************************************************

>	class CDRFontnameStoredItemEnumer : public EnumFonts

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03 01 96
	Purpose:	A class for those tricky font enumerations in the CDR filter
	SeeAlso:	CDRFilter

********************************************************************************************/

class CDRFontnameStoredItemEnumer : public EnumAllFonts
{
public:
	CDRFontnameStoredItemEnumer() {TheFontName = 0; Found = FALSE;};
	
	TCHAR *TheFontName;
	FontClass TheFontClass;							 	
	BOOL Found;
	String_64 TheRealFontName;

	BOOL NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf, NEWTEXTMETRIC FAR* lpntm);

};

BOOL CDRFontnameStoredItemEnumer::NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf, NEWTEXTMETRIC FAR* lpntm)
{
	// got it already?
	if(Found)
		return TRUE;
	
	// check we've got a nice font name
	ERROR3IF(TheFontName == 0, "Set up that silly little font name first, there's a good chap");

TRACEUSER( "Ben", _T("against %s\n"), lpelf->elfLogFont.lfFaceName);
	// do a case insenstive compare of the two fontnames
	if(_tcsncicmp(TheFontName, lpelf->elfLogFont.lfFaceName, 64) == 0)
	{
TRACEUSER( "Ben", _T("MATCH\n"));
		// then we've found the thingy we want - let's have it...
		Found = TRUE;
		
		// store the font class
		TheFontClass = Class;

		// copy across the real font name
		TheRealFontName = lpelf->elfLogFont.lfFaceName;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFontnameStoredItem::GetTypefaceForUse(DWORD Reference, TCHAR *FontName)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03 01 96
	Inputs:		CDR file font reference, pointer to font name
	Returns:	error flag
	Purpose:	Sets the Reference and hTypeface variables of the entry.
				It enumerates the font name to get the right class and capitalisation of
				the name, then caches it with the fontmanager. It then gets a typeface
				handle for it, which is stored for future retreval.
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFontnameStoredItem::GetTypefaceForUse(DWORD tReference, TCHAR *FontName)
{
	Reference = tReference;

	// OK, so we need to enumerate all those lovely little font tickles, and
	// would like to know the real font name and the lovelyness known as the
	// font class thingy. Splendid!

	// first of all, catch your font.
	CDRFontnameStoredItemEnumer EnumThing;
	EnumThing.TheFontName = FontName;
	EnumThing.Execute();

	// did we actually get one?
	if(EnumThing.Found == FALSE)
	{
		// make up a default typeface handle...
		hTypeface = FONTMANAGER->GetFont()->GetFontHandle();
		
		return TRUE;
	}

	// righteho, we now have a nice font name... so let's cache the thingy and get
	// ourselves a nice handle.
	hTypeface = FONTMANAGER->CacheNamedFont(&EnumThing.TheRealFontName);
TRACEUSER( "Ben", _T("Reference = %d, Real font name is '%s', handle is %d\n"), tReference, (TCHAR *)EnumThing.TheRealFontName, hTypeface);

	// check to see nothing's gone terriably wrong
	if(!HNDLVALID(hTypeface))
		return FALSE;

	return TRUE;
}

/********************************************************************************************

>	CDRBBoxList::CDRBBoxList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Returns:	-
	Purpose:	constructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRBBoxList::CDRBBoxList()
{
}


/********************************************************************************************

>	CDRBBoxList::~CDRBBoxList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Returns:	-
	Purpose:	destructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRBBoxList::~CDRBBoxList()
{
}


/********************************************************************************************

>	CDRBBoxListItem::CDRBBoxListItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Returns:	-
	Purpose:	constructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRBBoxListItem::CDRBBoxListItem()
{
	pNode = 0;
}


/********************************************************************************************

>	CDRBBoxListItem::~CDRBBoxListItem()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		-
	Returns:	-
	Purpose:	destructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRBBoxListItem::~CDRBBoxListItem()
{
}


/********************************************************************************************

>	BOOL CDRBBoxList::Add(Node *N, DocRect *BBox, Justification Just)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		node the bounding box is associated with, bounding box
	Returns:	error flag
	Purpose:	Adds a bounding box to the list
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRBBoxList::Add(Node *N, DocRect *BBox, Justification Just)
{
	CDRBBoxListItem *Item = new CDRBBoxListItem;

	Item->pNode = N;
	Item->BBox = *BBox;

	// alter the BBox for various justification options
	if(Just == JCENTRE)
	{
		BBox->Translate(0 - (BBox->Width() / 2), 0);
	}
	else if(Just == JRIGHT)
	{
		BBox->Translate(0 - BBox->Width(), 0);
	}

	AddTail(Item);

	return TRUE;
}

/********************************************************************************************

>	BOOL CDRBBoxList::Find(Node *N, DocRect *BBox)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		node to look up, a DocRect for the result
	Returns:	whether it found an entry for this node
	Purpose:	Finds a bounding box from a list of bounding boxes
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRBBoxList::Find(Node *N, DocRect *BBox)
{
	CDRBBoxListItem *Item = (CDRBBoxListItem *)GetHead();

	while(Item != 0)
	{
		if(Item->pNode == N)
		{
			if(BBox != 0)
			{
				*BBox = Item->BBox;
			}
			return TRUE;
		}

		Item = (CDRBBoxListItem *)GetNext(Item);
	}

	return FALSE;
}


/********************************************************************************************

>	DocRect *CDRBBoxList::Find(Node *N)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/95
	Inputs:		node to look up
	Returns:	DocRect or 0, can be modified
	Purpose:	Finds a stored bounding box, returning a pointer to the DocRect	which can be
				modified, for example, as a result of a transformation
	SeeAlso:	CDRFilter

********************************************************************************************/

DocRect *CDRBBoxList::Find(Node *N)
{
	CDRBBoxListItem *Item = (CDRBBoxListItem *)GetHead();

	while(Item != 0)
	{
		if(Item->pNode == N)
		{
			return &Item->BBox;
		}

		Item = (CDRBBoxListItem *)GetNext(Item);
	}

	return 0;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessTextList4()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/95
	Inputs:		-
	Returns:	error flag
	Purpose:	Processes the list of text objects for a version 4 CDR file creating text
				lines and textchars with partially applied attributes.
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::ProcessTextList4()
{
	// What we've got...
	//	btxt				the list we're current on
	//		strl			contains text for a paragraph object
	//			btid		the id of the text
	//			parl		a pargraph
	//				para	paragraph info
	// then any number of...
	//				bnch	n plain characters
	//				bsch	a character with styles applied to it

	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTSTART || RIFF->GetObjChunkType() != cdrT_btxt,
			"ProcessTextList4 called in the wrong context");

	UINT32 EndLevel = RIFF->GetObjLevel();

	do {
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_LISTSTART && RIFF->GetObjChunkType() == cdrT_strl)
		{
			if(!ProcessTextListItem4())
				return FALSE;
		}

		if(!RIFF->NextObject())
			return FALSE;
	
	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > EndLevel);

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::ProcessTextListItem4()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/95
	Inputs:		-
	Returns:	error flag
	Purpose:	Processes a item from the list of text objects for a version 4 CDR file
				creating text lines and textchars with partially applied attributes.
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::ProcessTextListItem4()
{
	// What we've got...
	//	strl			contains text for a paragraph object
	//		btid		the id of the text
	//		parl		a pargraph
	//			para	paragraph info
	// then any number of...
	//			bnch	n plain characters
	//			bsch	a character with styles applied to it

	ERROR3IF(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTSTART || RIFF->GetObjChunkType() != cdrT_strl,
			"ProcessTextListItem4 called in the wrong context");

	UINT32 EndLevel = RIFF->GetObjLevel();

	TextLine *FirstLine = 0;
	TextLine *CurrentLine = 0;
	INT32 MaxFirstLineSpace = 0;
	BOOL IsFirstLine = TRUE;

	WORD ID = 0;

	CDRTextStyle BaseStyle = CDRDefaultTextStyle;

	do {
	
		if(RIFF->GetObjType() == RIFFOBJECTTYPE_CHUNK)
		{
			switch(RIFF->GetObjChunkType())
			{
				case cdrT_btid:
					// id of thingy, store
					{
						if(!RIFF->AquireChunkData())
							return FALSE;

						WORD *Thingy = (WORD *)RIFF->GetAquiredData();

						ERROR2IF(Thingy == 0, FALSE, "Couldn't get aquired data");

						ID = CDRDATA_WORD(*Thingy);
					}
					break;
				
				case cdrT_para:
					{
						// do we need an end of line node for something?
						if(CurrentLine != 0)
						{
							EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);

							if(EOL == 0)
								return FALSE;
						}

						// new paragraph start - create a new line
						if(CurrentLine == 0)
						{
							CurrentLine = new TextLine;
						}
						else
						{
							CurrentLine = new TextLine(CurrentLine, NEXT);
						}

						if(CurrentLine == 0)
							return FALSE;

						if(FirstLine == 0)
							FirstLine = CurrentLine;
						else
							IsFirstLine = FALSE;

						// get the style of this paragraph
						if(!RIFF->AquireChunkData())
							return FALSE;

						cdrfParaDefnV4 *Para = (cdrfParaDefnV4 *)RIFF->GetAquiredData();
						if(Para == 0)
							return FALSE;

						if(!GetTextStyleFromCDRStyle(&BaseStyle, CDRDATA_WORD(Para->Style)))
							return FALSE;

						if(IsFirstLine && BaseStyle.LineSpace > MaxFirstLineSpace)
							MaxFirstLineSpace = BaseStyle.LineSpace;
					}
					break;
				
				case cdrT_bnch:
					// a load of characters
					ERROR2IF(CurrentLine == 0, FALSE, "CurrentLine is zero when characters found");
					
					{
						// get the chunk data
						if(!RIFF->AquireChunkData())
							return FALSE;
					
						// process it...
						cdrfTextListCharV4 *Chars = (cdrfTextListCharV4 *)RIFF->GetAquiredData();
						ERROR2IF(Chars == 0, FALSE, "no data from aquire data");

						INT32 NChars = RIFF->GetObjSize() / sizeof(cdrfTextListCharV4);

						// add all the nice characters we've got
						for(INT32 l = 0; l < NChars; l++)
						{
							if(CDRDATA_WORD(Chars[l].Char) >= ' ')
							{
								TextChar *NewChar = new TextChar(CurrentLine, LASTCHILD, CDRDATA_WORD(Chars[l].Char));

								if(NewChar == 0)
									return FALSE;

								// apply attributes
								if(!ApplyTextAttr(NewChar, &BaseStyle, 0))
									return FALSE;
							}
						}
					}					
					break;

				case cdrT_bsch:
					// a character with styles attached
					ERROR2IF(CurrentLine == 0, FALSE, "CurrentLine is zero when characters found");
					{
						// get the chunk data
						if(!RIFF->AquireChunkData())
							return FALSE;
					
						// process it...
						cdrfTextListCharStyledV4 *Char = (cdrfTextListCharStyledV4 *)RIFF->GetAquiredData();
						ERROR2IF(Char == 0, FALSE, "no data from aquire data");

						if(CDRDATA_WORD(Char->Char) >= ' ')
						{
							TextChar *NewChar = new TextChar(CurrentLine, LASTCHILD, CDRDATA_WORD(Char->Char));

							if(NewChar == 0)
								return FALSE;

							// apply attributes to it
							CDRTextStyle ThisStyle = BaseStyle;

							// font size
							ThisStyle.FontSize = CDRDATA_WORD(Char->FontSize) * CDRCOORDS_TO_MILLIPOINTS;
	
							// font name
/*							TCHAR *FontName = GetFontName(CDRDATA_WORD(Char->FontRef));
							if(FontName != 0)
							{
								ThisStyle.FontName = FontName;
							}
*/
							ThisStyle.FontReference = CDRDATA_WORD(Char->FontRef);

							// bold and italicness
							switch(Char->Weight)
							{
								case cdrfFONTTYPEV4_BOLD:
									ThisStyle.Bold = TRUE;
									ThisStyle.Italic = FALSE;
									break;

								case cdrfFONTTYPEV4_ITALIC:
									ThisStyle.Bold = FALSE;
									ThisStyle.Italic = TRUE;
									break;

								case cdrfFONTTYPEV4_BOLDITALIC:
									ThisStyle.Bold = TRUE;
									ThisStyle.Italic = TRUE;
									break;

								default:
									ThisStyle.Bold = FALSE;
									ThisStyle.Italic = FALSE;
									break;
							}

							// placement
							switch(Char->Placement)
							{
								default:
								case cdrfPLACEMENTV4_NONE:	ThisStyle.Script = CDRSCRIPT_NONE;	break;
								case cdrfPLACEMENTV4_SUPER:	ThisStyle.Script = CDRSCRIPT_SUPER;	break;
								case cdrfPLACEMENTV4_SUB:	ThisStyle.Script = CDRSCRIPT_SUB;	break;
							}

							// apply attributes
							if(!ApplyTextAttr(NewChar, &ThisStyle, 0))
								return FALSE;

							// sort out max first line space
							if(IsFirstLine && ThisStyle.LineSpace > MaxFirstLineSpace)
								MaxFirstLineSpace = ThisStyle.LineSpace;
						}
					}
					break;

				default:
					break;
			}
		}

		UpdateProgress();

		if(!RIFF->NextObject())
			return FALSE;
	
	} while(RIFF->GetObjType() != RIFFOBJECTTYPE_LISTEND || RIFF->GetObjLevel() > EndLevel);

	if(CurrentLine != 0)
	{
		// pop some ending stuff on it
		CaretNode *Caret = new CaretNode(CurrentLine, LASTCHILD);
		EOLNode *EOL = new EOLNode(CurrentLine, LASTCHILD);
		if(Caret == 0 || EOL == 0)
			return FALSE;
	}

	if(FirstLine != 0)
	{
		// got something - add it to the list of text stuff
		CDRVectorStoredItem *Item = new CDRVectorStoredItem;

		if(Item == 0)
			return FALSE;

		// store objects
		Item->Objects = FirstLine;
		Item->Reference = ID;

		// pop max first line in the bbox
		Item->BBox.hi.y = MaxFirstLineSpace;

		TextV4.AddTail(Item);
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::MakeTextOnPathLinkList()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/95
	Inputs:		-
	Returns:	error flag
	Purpose:	creates the list of paths which should be linked to text stories to get
				text on paths.
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::MakeTextOnPathLinkList()
{
	if(LinkTable == 0)
		return TRUE;		// no link table, no list...

	// run through the table creating entries
	cdrfLinkTableHdr *Table = (cdrfLinkTableHdr *)LinkTable;
	INT32 Entries = CDRDATA_WORD(Table->Entries);
	WORD *Offsets = (WORD *)(LinkTable + CDRDATA_WORD(Table->OffsetsOffset));

	if(Version != CDRVERSION_3)
	{
		// make for versions 4 and 5
		for(INT32 l = 0; l < Entries; l++)
		{
			cdrfLinkTableEntryTextOnPath *En = (cdrfLinkTableEntryTextOnPath *)(LinkTable + CDRDATA_WORD(Offsets[l]));

			if(En->Type == cdrfLINKTABLEENTRY_TEXTONPATH)
			{
				// make a new list item for this thingy
				CDRTextOnPathLink *Item = new CDRTextOnPathLink;

				if(Item == 0)
					return FALSE;

				// pop in the serial numbers
				Item->PathSerialNumber = CDRDATA_WORD(En->PathSerialNumber);
				Item->TextSerialNumber = CDRDATA_WORD(En->TextSerialNumber);

				// add the item to the list
				TextOnPathLinks.AddTail(Item);
			}
		}
	}
	else
	{
		// make for version 3
		for(INT32 l = 0; l < Entries; l++)
		{
			cdrfLinkTableEntryTextOnPathV3 *En = (cdrfLinkTableEntryTextOnPathV3 *)(LinkTable + CDRDATA_WORD(Offsets[l]));

			if(En->Type == cdrfLINKTABLEENTRYV3_TEXTONPATH)
			{
				// make a new list item for this thingy
				CDRTextOnPathLink *Item = new CDRTextOnPathLink;

				if(Item == 0)
					return FALSE;

				// pop in the serial numbers
				Item->PathSerialNumber = CDRDATA_WORD(En->PathSerialNumber);
				Item->TextSerialNumber = CDRDATA_WORD(En->TextSerialNumber);

				// add the item to the list
				TextOnPathLinks.AddTail(Item);
			}
		}
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::AttachTextToPaths()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/95
	Inputs:		-
	Returns:	error flag
	Purpose:	attachs paths to text stories from the list of links.
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::AttachTextToPaths()
{
	// set attributes to no colours - we mustn't render these here paths
	if(!SetFillColour(DocColour(COLOUR_TRANS)))
		return FALSE;
	if(!SetLineColour(DocColour(COLOUR_TRANS)))
		return FALSE;
	CurrentAttrs[ATTR_FILLGEOMETRY].Ignore = TRUE;

	// go through the entries in the list...
	CDRTextOnPathLink *Item = (CDRTextOnPathLink *)TextOnPathLinks.GetHead();

	while(Item != 0)
	{
		// if this is complete, process it
		if(Item->pPath != 0 && Item->pTextStory != 0)
		{
			// both things were found, so join them up nicely...

			// make a copy of the path
			NodePath *pPath;
			if(!Item->pPath->NodeCopy((Node **)&pPath))
				return FALSE;

			// delete it's old attributes
			pPath->DeleteChildren(pPath->FindFirstChild());

			// apply some attributes to the path
			pPath->InkPath.IsFilled = FALSE;
			pPath->InkPath.IsStroked = FALSE;
			if(!AttributeManager::ApplyBasedOnDefaults(pPath, CurrentAttrs))
				return FALSE;

			// and attach the nodes together
			Node *pFirstLine = Item->pTextStory->FindFirstChild(CC_RUNTIME_CLASS(TextLine));
			ERROR2IF(pFirstLine == 0, FALSE, "TextStory had no line when attaching path");
			pPath->AttachNode(pFirstLine, PREV);

			// now sort out justification along the path
			if(Item->Justi != JLEFT)
			{
				// knock up an attribute and pop it on
				TxtJustificationAttribute Attr(Item->Justi);
				NodeAttribute *pNode = Attr.MakeNode();
				if(pNode == 0)
					return FALSE;
				pNode->AttachNode(Item->pTextStory, FIRSTCHILD);
			}
			else
			{
				TRACEUSER( "Ben", _T("Text on path not completed: path serial = %d, path ptr = %X\ntext serial = %d, text ptr = %X\n\n"), Item->PathSerialNumber, Item->pPath, Item->TextSerialNumber, Item->pTextStory);
			}
		
			// and the positioning
			Item->pTextStory->SetImportBaseShift((BaseShiftEnum)Item->Position);
		}
		
		// next!
		Item = (CDRTextOnPathLink *)TextOnPathLinks.GetNext(Item);
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CDRFilter::CheckTextForLinks(TextStory *pTextStory, cdrfOffsetHeader *Header)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/95
	Inputs:		-
	Returns:	error flag
	Purpose:	checks to see if this text story is in the link table
	SeeAlso:	CDRFilter

********************************************************************************************/

BOOL CDRFilter::CheckTextForLinks(TextStory *pTextStory, cdrfOffsetHeader *Header)
{
	if(IsTextOnAPath != TRUE)
		return TRUE;		// not on a path, so don't search

	// go through the entries in the list...
	CDRTextOnPathLink *Item = (CDRTextOnPathLink *)TextOnPathLinks.GetHead();

	while(Item != 0)
	{
		// does this link refer to this text story?
		if(SerialNumber == Item->TextSerialNumber)
		{
			// yep - make the entry refer to this text story
			Item->pTextStory = pTextStory;

			// set up some justification and positioning stuff
			cdrfTextOnPathInfo *Info;
			if((Info = (cdrfTextOnPathInfo *)FindDataInObject(Header, cdrfOBJOFFSETTYPE_PATHTEXT1)) != 0)
			{
				switch(CDRDATA_WORD(Info->Alignment))
				{
					case cdrfTEXTONPATHALIGN_RIGHT:
						Item->Justi = JRIGHT;
						break;

					case cdrfTEXTONPATHALIGN_CENTRE:
						Item->Justi = JCENTRE;
						break;

					default:
					case cdrfTEXTONPATHALIGN_LEFT:
						Item->Justi = JLEFT;
						break;
				}

				switch(CDRDATA_WORD(Info->Position))
				{
					case cdrfTEXTONPATHPOS_BOTTOM:
						Item->Position = AlignDescenders;
						break;

					case cdrfTEXTONPATHPOS_TOP:
						Item->Position = AlignAscenders;
						break;

					case cdrfTEXTONPATHPOS_CENTRE:
						Item->Position = AlignCentres;
						break;

					case cdrfTEXTONPATHPOS_BASELINE:
					default:
						Item->Position = AlignBaseline;
						break;

				}
			}

			// return now so we don't get duplicates...
			return TRUE;
		}
		
		// next!
		Item = (CDRTextOnPathLink *)TextOnPathLinks.GetNext(Item);
	}
	
	return TRUE;
}


/********************************************************************************************

>	CDRTextOnPathLink::CDRTextOnPathLink()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/95
	Inputs:		-
	Returns:	-
	Purpose:	constructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRTextOnPathLink::CDRTextOnPathLink()
{
	PathSerialNumber = -1;
	pPath = 0;
	TextSerialNumber = -1;
	pTextStory = 0;
	Justi = JLEFT;
	Position = AlignBaseline;
}


/********************************************************************************************

>	CDRTextOnPathLink::~CDRTextOnPathLink()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/06/95
	Inputs:		-
	Returns:	-
	Purpose:	destructor
	SeeAlso:	CDRFilter

********************************************************************************************/

CDRTextOnPathLink::~CDRTextOnPathLink()
{
}



