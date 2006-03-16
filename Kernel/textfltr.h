// $Id: textfltr.h 662 2006-03-14 21:31:49Z alex $
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


#ifndef INC_TEXTFLTR
#define INC_TEXTFLTR

#include "ccfile.h"
#include "filters.h"
#include "stack.h"


#if BUILD_TEXT_FILTERS	

class Page;
class TextStory;
class VisibleTextNode;
class RTFTextFilter;
class Pathname;


/********************************************************************************************
>	class BaseTextFilter : public TextFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Base class for all text importers/exporters.  Repository for all common code
********************************************************************************************/

class BaseTextFilter : public TextFilter
{
	CC_DECLARE_DYNAMIC(BaseTextFilter);
	
public:
	BaseTextFilter();
	~BaseTextFilter();

	// Operations on story being created
protected:
	TextStory* CreateImportStory(DocCoord Pos);
	VisibleTextNode* AddCharToStory(TextStory* pStory, WCHAR NewChar);
	BOOL AddNewLineToStory(TextStory* pStory);
	BOOL AddTabToStory(TextStory* pStory, MILLIPOINT TabLength = 4000);
};


#if BUILD_OTHER_TEXT_FILTER

/********************************************************************************************
>	class ANSITextFilter : public BaseTextFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Encapsulates a ANSI text filter system.  Also load multi-byte files, but not
				Unicode ones.
********************************************************************************************/

class ANSITextFilter : public BaseTextFilter
{
	CC_DECLARE_DYNAMIC(ANSITextFilter);
	
public:
	ANSITextFilter();
	~ANSITextFilter();
	BOOL Init();	

	// Works out if opening a file of this type requires a default document to be loaded
	// or does the file format supply the document.
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);
	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);

	BOOL DoImport(SelOperation* Op, CCLexFile*, Document* DestDoc,
				  BOOL AutoChosen, ImportPosition* Pos = NULL);
	BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*);

protected:
	BOOL PrepareToImport();
	void CleanUpAfterImport();
	virtual BOOL DoInternalImport(TextStory* pImportStory, CCLexFile* pFile);
	virtual BOOL SkipHeader(CCLexFile* pFile);
	virtual WCHAR GetNextCharacter(CCLexFile* pFile);

	UINT32 BytesReadSoFar;
	BOOL EscapeWasPressed;
};



/********************************************************************************************
>	class UnicodeTextFilter : public ANSITextFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Unicode text filter system.  Derived from ANSI text filer as they have so
				much in common.  Overrides a few virtual functions do to the Unicode specific
				tasks
********************************************************************************************/
class UnicodeTextFilter : public ANSITextFilter
{
	CC_DECLARE_DYNAMIC(UnicodeTextFilter);
	
public:
	UnicodeTextFilter();
	~UnicodeTextFilter();
	BOOL Init();	

	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);

protected:
	virtual BOOL SkipHeader(CCLexFile* pFile);
	virtual WCHAR GetNextCharacter(CCLexFile* pFile);
};



typedef INT32 TWIP;
const INT32 TwipToMillpoint = 1000/20;		// One 'twip' = one twentith of a point




/********************************************************************************************
>	class RTFColourTable : public List

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Stores the colour table from the RTF file.  These are held as DocColors
				(having been read as rgb tuples).
********************************************************************************************/
class RTFColourTable : public List
{
	CC_DECLARE_DYNAMIC(RTFColourTable);

public:
	DocColour GetColourAt(INT32 TableIndex);
};


/********************************************************************************************
>	class RTFColourTableEntry : public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Stores one colour in the colour table from the RTF file.
********************************************************************************************/
class RTFColourTableEntry : public ListItem
{
	CC_DECLARE_DYNAMIC(RTFColourTableEntry);

public:
	DocColour Colour;
};



/********************************************************************************************
>	class RTFFontTable : public List

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Stores the font table from the RTF file. 
********************************************************************************************/
class RTFFontTable : public List
{
	CC_DECLARE_DYNAMIC(RTFFontTable);

public:
	BOOL GetIndexedFont(INT32 TableIndex, String_64* FontName);
};


/********************************************************************************************
>	class RTFFontTableEntry : public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Stores the colour table from the RTF file.  These are held as DocColors
				(having been read as rgb tuples).
********************************************************************************************/
class RTFFontTableEntry : public ListItem
{
	CC_DECLARE_DYNAMIC(RTFFontTableEntry);

public:
	enum FontFamilyType {FamNil = 0, FamRoman, FamSwiss, FamModern, FamScript, FamDecor, FamTech, FamBiDi};

	RTFFontTableEntry();
	~RTFFontTableEntry();

	String_64 RealFontName;			// The font name as specified by the RTF file
	String_64 ApplyFontName;		// The font we are actually applying (allows for subsitution)
	FontFamilyType FontFamily;		// The font family type as specified by the RTF file
	INT32 CharacterSet;				// The character set of the font
	INT32 CodePage;	  				// The code page text in this font is in
	INT32 FontIndex;					// The font number (table is not contigious)
};



// When you want to handle a new RTF control word derive it from RTFAttributeAction for
// handling attribute changes, otherwise derive from RTFControlAction.
// Remember to add you new class to be tested in RTFTextFilter::ParseRTFKeyword!!!


/********************************************************************************************
>	class RTFControlAction : public CC_CLASS_MEMDUMP

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Base class of the RTF control word handlers
********************************************************************************************/
class RTFControlAction : public CC_CLASS_MEMDUMP
{
public:
	CC_DECLARE_MEMDUMP(RTFControlAction)
	RTFControlAction(RTFTextFilter* pFilt, INT32 Param);

	virtual BOOL ExecuteControl() = 0;

protected:
	static BOOL ExecuteControlCarefully(RTFControlAction* pAction);

	INT32 Parameter;
	RTFTextFilter* pFilter;
};



/********************************************************************************************
>	class RTFAttributeAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Base class of the RTF attribute control word handlers
********************************************************************************************/
class RTFAttributeAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFAttributeAction)
	RTFAttributeAction(RTFTextFilter* pFilt, INT32 Param);
};



/********************************************************************************************
>	class RTFEnterSkipDest : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	New RTF destination handler
********************************************************************************************/
class RTFEnterSkipDest : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFEnterSkipDest)
	RTFEnterSkipDest(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFColourTableAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF colour table handler
********************************************************************************************/
class RTFColourTableAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFColourTableAction)
	RTFColourTableAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};


/********************************************************************************************
>	class RTFFontTableAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/95
	Purpose:	RTF font table handler
********************************************************************************************/
class RTFFontTableAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFFontTableAction)
	RTFFontTableAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};


/********************************************************************************************
>	class RTFNewParaAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF new paragraph handler
********************************************************************************************/
class RTFNewParaAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFNewParaAction)
	RTFNewParaAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFBoldAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text bold attribute handler
********************************************************************************************/
class RTFBoldAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFBoldAttribute)
	RTFBoldAttribute(RTFTextFilter* pFilt, INT32 Param = 1);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFItalicAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text colour attribute handler
********************************************************************************************/
class RTFItalicAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFItalicAttribute)
	RTFItalicAttribute(RTFTextFilter* pFilt, INT32 Param = 1);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFTextSizeAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text size attribute handler
********************************************************************************************/
class RTFTextSizeAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTextSizeAttribute)
	RTFTextSizeAttribute(RTFTextFilter* pFilt, MILLIPOINT Param);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(TWIP Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFSuperscriptAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text superscript attribute handler
********************************************************************************************/
class RTFSuperscriptAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFSuperscriptAttribute)
	RTFSuperscriptAttribute(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFSubscriptAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text subscript attribute handler
********************************************************************************************/
class RTFSubscriptAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFSubscriptAttribute)
	RTFSubscriptAttribute(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};


/********************************************************************************************
>	class RTFNoscriptAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text superscript/subscript off attribute handler
********************************************************************************************/
class RTFNoscriptAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFNoscriptAttribute)
	RTFNoscriptAttribute(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};


/********************************************************************************************
>	class RTFTextColourAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text colour attribute handler
********************************************************************************************/
class RTFTextColourAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTextColourAttribute)
	RTFTextColourAttribute(RTFTextFilter* pFilt, INT32 ColourNumber = 0);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};


/********************************************************************************************
>	class RTFTextFontAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF text font attribute handler
********************************************************************************************/
class RTFTextFontAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTextFontAttribute)
	RTFTextFontAttribute(RTFTextFilter* pFilt, INT32 FontNumber = 0);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};


/********************************************************************************************
>	class RTFQPTrackingAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF tracking (defined in 1/4 points) attribute handler
********************************************************************************************/
class RTFQPTrackingAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFQPTrackingAttribute)
	RTFQPTrackingAttribute(RTFTextFilter* pFilt, INT32 Gap = 0);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(INT32 Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFTWIPTrackingAttribute : public RTFAttributeAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	RTF tracking (defined in twips) attribute handler
********************************************************************************************/
class RTFTWIPTrackingAttribute : public RTFAttributeAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTWIPTrackingAttribute)
	RTFTWIPTrackingAttribute(RTFTextFilter* pFilt, INT32 Gap = 0);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(TWIP Param, BOOL UseParam, RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFPaperWidth : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Reads RTF document's paper width
********************************************************************************************/
class RTFPaperWidth : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFPaperWidth)
	RTFPaperWidth(RTFTextFilter* pFilt, MILLIPOINT Width);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter, TWIP Width);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFPaperHeight : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Read RTF document's paper height
********************************************************************************************/
class RTFPaperHeight : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFPaperHeight)
	RTFPaperHeight(RTFTextFilter* pFilt, MILLIPOINT Height);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter, TWIP Height);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFLeftMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the width of the left-hand page margin
********************************************************************************************/
class RTFLeftMargin : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFLeftMargin)
	RTFLeftMargin(RTFTextFilter* pFilt, MILLIPOINT LeftMargin);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter, TWIP LeftMargin);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFRightMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the width of the right-hand page margin
********************************************************************************************/
class RTFRightMargin : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFRightMargin)
	RTFRightMargin(RTFTextFilter* pFilt, MILLIPOINT RightMargin);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter, MILLIPOINT RightMargin);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFTopMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the height of the margin at the top of the page
********************************************************************************************/
class RTFTopMargin : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTopMargin)
	RTFTopMargin(RTFTextFilter* pFilt, MILLIPOINT TopMargin);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter, TWIP TopMargin);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFBottomMargin : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/95
	Purpose:	Sets the height of the margin at the bottom of the page
********************************************************************************************/
class RTFBottomMargin : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFBottomMargin)
	RTFBottomMargin(RTFTextFilter* pFilt, MILLIPOINT BottomMargin);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter, TWIP BottomMargin);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFResetCharAttrs : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Resets all the character attributes back to their default values
********************************************************************************************/
class RTFResetCharAttrs : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFResetCharAttrs)
	RTFResetCharAttrs(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFResetParaAttrs : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Resets all the paragraph attributes back to their default values
********************************************************************************************/
class RTFResetParaAttrs : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFResetParaAttrs)
	RTFResetParaAttrs(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFTableRowAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Resets all the paragraph attributes back to their default values
********************************************************************************************/
class RTFTableRowAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTableRowAction)
	RTFTableRowAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFTableCellAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Resets all the paragraph attributes back to their default values
********************************************************************************************/
class RTFTableCellAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTableCellAction)
	RTFTableCellAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFTabAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a tab
********************************************************************************************/
class RTFTabAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFTabAction)
	RTFTabAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFSingleCharAction : public RTFControlAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/96
	Purpose:	Processes a single 'special' character
********************************************************************************************/
class RTFSingleCharAction : public RTFControlAction
{
public:
	CC_DECLARE_MEMDUMP(RTFSingleCharAction)
protected:
	RTFSingleCharAction(RTFTextFilter* pFilt, WCHAR Character);

public:
	BOOL ExecuteControl();
};



/********************************************************************************************
>	class RTFHexCharAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/1/96
	Purpose:	Processes a character defined by two hex chars
********************************************************************************************/
class RTFHexCharAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFHexCharAction)
	RTFHexCharAction(RTFTextFilter* pFilt, WCHAR Character);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFEmDashAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a em dash
********************************************************************************************/
class RTFEmDashAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFEmDashAction)
	RTFEmDashAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFEnDashAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a en dash
********************************************************************************************/
class RTFEnDashAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFEnDashAction)
	RTFEnDashAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFEmSpaceAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a em space
********************************************************************************************/
class RTFEmSpaceAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFEmSpaceAction)
	RTFEmSpaceAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFEnSpaceAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a en dash
********************************************************************************************/
class RTFEnSpaceAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFEnSpaceAction)
	RTFEnSpaceAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFBulletAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a bullet symbol
********************************************************************************************/
class RTFBulletAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFBulletAction)
	RTFBulletAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFSingleLeftQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a single left hand quote
********************************************************************************************/
class RTFSingleLeftQuoteAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFSingleLeftQuoteAction)
	RTFSingleLeftQuoteAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFSingleRightQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a single right hand quote
********************************************************************************************/
class RTFSingleRightQuoteAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFSingleRightQuoteAction)
	RTFSingleRightQuoteAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFDoubleLeftQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a double left hand quote
********************************************************************************************/
class RTFDoubleLeftQuoteAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFDoubleLeftQuoteAction)
	RTFDoubleLeftQuoteAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFDoubleRightQuoteAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a single right hand quote
********************************************************************************************/
class RTFDoubleRightQuoteAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFDoubleRightQuoteAction)
	RTFDoubleRightQuoteAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class RTFNonBreakSpaceAction : public RTFSingleCharAction

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/96
	Purpose:	Processes a non-breaking space character
********************************************************************************************/
class RTFNonBreakSpaceAction : public RTFSingleCharAction
{
public:
	CC_DECLARE_MEMDUMP(RTFNonBreakSpaceAction)
	RTFNonBreakSpaceAction(RTFTextFilter* pFilt);

	static BOOL IsThisAction(StringBase* pControl);
	static BOOL CreateAndExecute(RTFTextFilter* pFilter);
};



/********************************************************************************************
>	class StateStore : public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/95
	Purpose:	Holds the current non-arribute state of the importer.  Can be poped onto the
				stack.
********************************************************************************************/
class StateStore : public ListItem
{
	CC_DECLARE_DYNAMIC(StateStore)

public:
	StateStore();
	~StateStore();
	
	StateStore& operator=(const StateStore&);

	enum DestinationStateTypes { rdsNorm, rdsSkip };
	enum InternalStateTypes { risNorm, risBin };

	DestinationStateTypes	DestinationState;		// Reading or skipping text
	InternalStateTypes		InternalState;			// Normal, binary or hex
	MILLIPOINT InterCharGap;
	// If you add member variables initialise in constructor and add to operator= too!
};



/********************************************************************************************
>	class RTFDefaultAttributes : public CC_CLASS_MEMDUMP

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/95
	Purpose:	Keeps a copy of the attributes that the current attributes are set back to
				when a 'plain' or 'pard' command is encountered
********************************************************************************************/
class RTFDefaultAttributes : public CC_CLASS_MEMDUMP
{
public:
	String_64 FontName;
};



/********************************************************************************************
>	class RTFTextFilter : public BaseTextFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Encapsulates a Rich Text Format filter system.  RTF loading code based on
				'rtfreadr' from MSDN
********************************************************************************************/
class RTFTextFilter : public BaseTextFilter
{
	CC_DECLARE_DYNAMIC(RTFTextFilter);
	
public:
	friend class RTFTextOILFilter;

	RTFTextFilter();
	~RTFTextFilter();

	// Various virtual functions for filtery things
	BOOL Init();	
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);
	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);
	BOOL DoImport(SelOperation* Op, CCLexFile*, Document* DestDoc,
				  BOOL AutoChosen, ImportPosition* Pos = NULL);
	BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*);

	// Access functions for the control handlers to set attribute states
	BOOL SetBold(BOOL On)					{return SetTextBold(On);}
	BOOL SetItalic(BOOL On)					{return SetTextItalic(On);}
	BOOL SetSize(MILLIPOINT NewSize)		{return SetTextSize(NewSize);}
	MILLIPOINT GetTextSize()
		{return ((TxtFontSizeAttribute*) CurrentAttrs[ATTR_TXTFONTSIZE].pAttr)->FontSize;}
	BOOL SetSuperscript();
	BOOL SetSubscript();
	BOOL SetNoscript()		 				{return RemoveTextScript();}
	BOOL SetTextColour(DocColour& NewCol)	{return SetFillColour(NewCol);}
	BOOL SetFont(String_64* pName)			{return SetTextTypeFace(pName);}

	BOOL ResetCharacterAttributes();
	BOOL ResetParagraphAttributes();

	// More functions for control handlers
	void SetSkipping()	{CurrentState.DestinationState = StateStore::rdsSkip;}
	BOOL StartNewLine() {return AddNewLineToStory(pImportStory);}
	BOOL InsertTab()	{return AddTabToStory(pImportStory);}
	BOOL InsertChar(WCHAR Char)	{return ParseChar(Char);}

	// Information Table handlers
	BOOL ReadColourTable();
	RTFColourTable* GetColourTable()		{return &ColourTable;}
	BOOL ReadFontTable();
	RTFFontTable* GetFontTable()			{return &FontTable;}

	BOOL ReadCharFromFile(TCHAR* ch, BOOL SkipCR = TRUE);

protected:
	// Import helpers
	BOOL PrepareToImport();					// Does initialisations
	void CleanUpAfterImport();				// Frees memory (empties tables etc..)
	BOOL DoInternalImport();				// The actual import loop

	BOOL InitialiseInfo();					// Inits RTF document info held in filter

	// From rtfreadr - function headers
	BOOL PushRTFState();
	BOOL PopRTFState();
	BOOL ParseRTFKeyword();
	BOOL ParseChar(WCHAR Char);
	BOOL TranslateKeyword(StringBase* pKeyword, INT32 param, BOOL fParam);

	BOOL EndGroupAction(StateStore::DestinationStateTypes rds);

	// File processors
	BOOL ReadKeyword(StringBase* pKeyword, INT32* pParam, BOOL* pParamUsed);
  	BOOL SkipBackChar(INT32 Count = 1);
	BOOL ReadOneFontFromTable();

	// RTF member variable declarations
	RTFDefaultAttributes DefaultAttrs;
	StateStore CurrentState;
	INT32 cbBin;
	BOOL fSkipDestIfUnk;

	UINT32 BytesReadSoFar;					// Count of the bytes read so far
	TextStory* pImportStory;				// Pointer to the story we are importing into
	CCLexFile* pImportFile;					// Pointer to file we are importing from
	BOOL EscapeWasPressed;					// TRUE if the use pressed ESCAPE to abort the import
	BOOL ImportHasFinished;

	// Various stacks and tables
	MarkedStack StateStack;					// RTF state stack
	AttrRecordList PreviousAttributes;		// Attribute stack
	RTFColourTable ColourTable;				// Text colour lookup table
	RTFFontTable FontTable;					// Text font lookup table

	// Document information
	MILLIPOINT	PaperWidth;
	MILLIPOINT	PaperHeight;
	MILLIPOINT	LeftMargin;
	MILLIPOINT	RightMargin;
	MILLIPOINT	TopMargin;
	MILLIPOINT	BottomMargin;

public:
	// Access functions for internal members
	void SetPaperWidth(MILLIPOINT NewWidth)		{PaperWidth = NewWidth;}
	void SetPaperHeight(MILLIPOINT NewHeight)	{PaperHeight = NewHeight;}
	void SetLeftMargin(MILLIPOINT NewMargin)	{LeftMargin = NewMargin;}
	void SetRightMargin(MILLIPOINT NewMargin)	{RightMargin = NewMargin;}
	void SetTopMargin(MILLIPOINT NewMargin)		{TopMargin = NewMargin;}
	void SetBottomMargin(MILLIPOINT NewMargin)	{BottomMargin = NewMargin;}

	void SetInterCharGap(MILLIPOINT NewGap)		{CurrentState.InterCharGap = NewGap;}
};

#endif	//#if BUILD_OTHER_TEXT_FILTER

#endif	//include
#endif	//#if BUILD_TEXT_FILTERS
