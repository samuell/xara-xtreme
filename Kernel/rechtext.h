// $Id: rechtext.h 662 2006-03-14 21:31:49Z alex $
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

>	rechtext.h

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Purpose:	Text record handler for the new file format

********************************************************************************************/

#ifndef INC_RECH_TEXT
#define INC_RECH_TEXT

class TextStory;

/********************************************************************************************

>	class TextObjRecordHandler : public CamelotRecordHandler

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Base Class:	CamelotRecordHandler
	Purpose:	Record handler for text objects
	See also:	class TextAttrRecordHandler

********************************************************************************************/

class TextObjRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(TextObjRecordHandler);
	
public:
	TextObjRecordHandler() { pLastInsertedTextStory = NULL; };
	~TextObjRecordHandler() {};
	
// functions that must be implemented
public:
	UINT32* GetTagList();
	BOOL HandleRecord(CXaraFileRecord* pCXaraFileRecord);

	virtual BOOL BeginImport();

private:
	BOOL ReadTextStorySimple(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryComplex(CXaraFileRecord *pCXaraFileRecord);
	
	BOOL ReadTextStorySimpleStartLeft(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStorySimpleStartRight(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStorySimpleEndLeft(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStorySimpleEndRight(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryComplexStartLeft(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryComplexStartRight(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryComplexEndLeft(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryComplexEndRight(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryWordWrapInfo(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextStoryIndentInfo(CXaraFileRecord *pCXaraFileRecord);

	BOOL ReadTextLine(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextString(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextChar(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextEOL(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextKernCode(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextCaret(CXaraFileRecord *PCXaraFileRecord);
	BOOL ReadTextLineInfo(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextInvalid(CXaraFileRecord *pCXaraFileRecord);

	BOOL ReadAutoKern ( CXaraFileRecord	*pCXaraFileRecord,
						TextStory		*pStory );

	// Code to keep track of the last text story inserted into the tree
	BOOL InsertTextStoryNode(TextStory* pTextStory);
	TextStory* pLastInsertedTextStory;

#if XAR_TREE_DIALOG
public:
	void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr);
	void DescribeStorySimple(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryComplex(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStorySimpleStartLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStorySimpleStartRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStorySimpleEndLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStorySimpleEndRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryComplexStartLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryComplexStartRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryComplexEndLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryComplexEndRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryWordWrapInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeStoryIndentInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeLine(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeString(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeChar(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeEOL(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeKern(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeCaret(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeLineInfo(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
#endif
};

/********************************************************************************************

>	class TextAttrRecordHandler : public CamelotRecordHandler

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/96
	Base Class:	CamelotRecordHandler
	Purpose:	Record handler for text attribute records

********************************************************************************************/

class TextAttrRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(TextAttrRecordHandler);
	
public:
	TextAttrRecordHandler() {};
	~TextAttrRecordHandler() {};
	
// functions that must be implemented
public:
	UINT32* GetTagList();
	BOOL HandleRecord(CXaraFileRecord* pCXaraFileRecord);

private:
	BOOL ReadTextStoryVanilla(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextString(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextKern(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextCaret(CXaraFileRecord *PCXaraFileRecord);
	BOOL ReadTextLineSpaceRatio(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextLineSpaceAbsolute(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextJustificationLeft(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextJustificationCentre(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextJustificationRight(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextJustificationFull(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextFontSize(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextFontTypeface(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextBoldOn(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextBoldOff(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextItalicOn(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextItalicOff(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextUnderlineOn(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextUnderlineOff(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextScriptOn(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextScriptOff(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextSuperscriptOn(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextSubscriptOn(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextTracking(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextAspectRatio(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextBaseline(CXaraFileRecord *pCXaraFileRecord);
	BOOL ReadTextInvalid(CXaraFileRecord *pCXaraFileRecord);

#if XAR_TREE_DIALOG
public:
	void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr);
	void DescribeLineSpaceRatio(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeLineSpaceAbsolute(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeJustificationLeft(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeJustificationCentre(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeJustificationRight(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeJustificationFull(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeFontSize(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeFontTypeface(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeBoldOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeBoldOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeItalicOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeItalicOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeUnderlineOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeUnderlineOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeScriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeScriptOff(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeSuperscriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeSubscriptOn(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeTracking(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeAspectRatio(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeBaseline(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
#endif

};

/********************************************************************************************

>	class FontDefRecordHandler : public CamelotRecordHandler

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/96
	Base Class:	CamelotRecordHandler
	Purpose:	Record Handler for font definition records

********************************************************************************************/

class FontDefRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(FontDefRecordHandler);
	
public:
	FontDefRecordHandler() {};
	~FontDefRecordHandler() {};
	
// functions that must be implemented
public:
	UINT32* GetTagList();
	BOOL HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#if XAR_TREE_DIALOG
public:
	void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord, StringBase *pStr);
	void DescribeFontDefTrueType(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeFontDefATM(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
	void DescribeFontDefInvalid(CXaraFileRecord *pCXaraFileRecord, StringBase *pStr);
#endif
};

#endif
