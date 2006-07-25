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
// The TextLine object

#ifndef INC_NODETXTL
#define INC_NODETXTL

//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodetxts.h"
//#include "attrmgr.h"	// for FormatRegion::GetTracking() - in camtypes.h [AUTOMATICALLY REMOVED]

class EOLNode;
class CharMetrics;
class NodeGroup;
class BecomeA;

/********************************************************************************************
>	class FormatRegion:	public RenderRegion

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Purpose:	A FormatRegion is a render region that is used during the formatting of
				characters.  No actual rendering is done; the FormatRegion provides a simple
				way of managing the attribute stack.

				Call the init function before using the FormatRegion.  This is passed the 
				root node of the subtree to format.  The Init function renders all the attributes
				applied to this node into the FormatRegion.  You can then use the object
				as an attribute stack.  You will want to call each nodes Format function.
				NOTE: you will also want to render all attributes you come across into the
				FormatRegion
********************************************************************************************/

class FormatRegion : public RenderRegion
{
	CC_DECLARE_DYNAMIC(FormatRegion)

	FormatRegion();
	~FormatRegion();										   

	BOOL Init(NodeRenderableInk* pFirstNode);

	// To be safe we override all drawing functions to to prevent drawing
	// Some are pure and have to be overridden
	// Pure drawing functions
	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawRect(DocRect *RectToRender) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawDragRect(DocRect *RectToRender) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawPixel(const DocCoord &Point) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawBlob(DocCoord p, BlobType type) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawCross(const DocCoord &Point, const UINT32 Size) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawBitmapBlob(const DocCoord &Point, ResourceID resID ) {ERROR3("Rendering into a FormatRegion");};

	// Non pure ones
	virtual void DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat = DEFAULT_TEXT_FORMATTING) {ERROR3("Rendering into a FormatRegion");};
	virtual void SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background) {ERROR3("Rendering into a FormatRegion");};
	virtual void GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi = NULL) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawPixelRect(DocRect *RectToRender) {ERROR3("Rendering into a FormatRegion");};
	virtual void DrawPixelLine(const DocCoord &StartPoint, const DocCoord &EndPoint) {ERROR3("Rendering into a FormatRegion");};

	// More pure functions
	virtual BOOL StopRender(void) {return TRUE;};
	virtual void InitClipping(void) {};
	virtual void InitAttributes(void) {};
	virtual void SetLineAttributes(void) {};
	virtual void SetOSDrawingMode(void) {};
	virtual void SetQualityLevel(void) {};
	virtual INT32 CalcPixelWidth(void) {return 1;};
	virtual INT32 CalcScaledPixelWidth(void) {return 1;};

public:
	BOOL		GetCharMetrics(CharMetrics* pCharMetrics, WCHAR ch);
	MILLIPOINT	GetCharsKerning(WCHAR chLeft, WCHAR chRight);		// jwp 15/10/2000

	MILLIPOINT    GetTracking()      { return RR_TXTTRACKING(); }
	Justification GetJustification() { return RR_TXTJUSTIFICATION(); }
	MILLIPOINT    GetLineSpacing()   { return RR_TXTLINESPACE(); }
	FIXED16       GetLineSpaceRatio(){ return ((TxtLineSpaceAttribute*)CurrentAttrs[ATTR_TXTLINESPACE].pAttr)->Ratio; }
	MILLIPOINT    GetFontSize()      { return RR_TXTFONTSIZE(); }
	MILLIPOINT    GetBaseLineShift() { return RR_TXTBASELINE(); }
	MILLIPOINT    GetLeftMargin()    { return RR_TXTLEFTMARGIN(); }
	MILLIPOINT    GetRightMargin()   { return RR_TXTRIGHTMARGIN(); }
	MILLIPOINT    GetFirstIndent()   { return RR_TXTFIRSTINDENT(); }
	TxtRuler*     GetRuler()         { return RR_TXTRULER(); }
private:
	std::auto_ptr<wxDC>	m_pFormatDC;
};


/********************************************************************************************
>	class FormatState: public CC_CLASS_MEMDUMP

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	21/06/06
	Purpose:	encapsulates state when wrapping/formatting a text line
********************************************************************************************/

class FormatState: public CC_CLASS_MEMDUMP
{
 public:
	const BOOL       SetCharPositions;   // flag indicating whether formatting should set char positions
	const MILLIPOINT FitWidth;           // overall width available
	VisibleTextNode* const pFirstVTN;    // the first VTN in the line
	const MILLIPOINT CharPosOffset;      // used for formatting only (i.e., if SetCharPositions = TRUE)
	const MILLIPOINT ExtraOnChars;       // used for formatting only (i.e., if SetCharPositions = TRUE)
	const MILLIPOINT ExtraOnSpaces;      // used for formatting only (i.e., if SetCharPositions = TRUE)
	MILLIPOINT       Width;              // overall width used so far

	// All formatted text is anchored to a tab stop. There is a notional left tab at position 0
	// that anchors the start of the text. ActiveTabType/ActiveTabPos always contain information
	// about the currently active tab stop.
	TxtTabType       ActiveTabType;      // the type of the currently active tab stop
	MILLIPOINT       ActiveTabPos;       // its position
	VisibleTextNode* pLastTabVTN;        // the most recently fitted tab character object (maybe NULL)

	// AnchorPos contains the width of the text formatted up to the most recently encountered tab
	// character, in other words, the starting position of the field into which the section of text
	// of text anchored to the currently active tab is to be formatted.
	MILLIPOINT       AnchorPos;

	// When a non-left tab is encountered, RemainingSpace is set to the remaining space between the
	// tab character position (AnchorPos) and the position of the tab stop to which it advances
	// (ActiveTabPos). It is updated as text is formatted to the left of the tab stop.
	MILLIPOINT       RemainingSpace;
	// if the most recently encountered tab was a Decimal tab, keep track of what its decimal point
	// character is and whether we have met the decimal point already
	WCHAR            DecimalPointChar;
	BOOL             DecimalPointFound;

	// default constructor (no need to initialise RemainingSpace and DecimalPointFound)
	FormatState(MILLIPOINT AvailableWidth, BOOL SetPositions, VisibleTextNode* pFirstNode,
				MILLIPOINT Indent, MILLIPOINT _CharPosOffset,
				MILLIPOINT _ExtraOnChars, MILLIPOINT _ExtraOnSpaces):
		SetCharPositions(SetPositions),
		FitWidth(AvailableWidth), pFirstVTN(pFirstNode),
		CharPosOffset(_CharPosOffset),
		ExtraOnChars(_ExtraOnChars), ExtraOnSpaces(_ExtraOnSpaces),
		Width(Indent), ActiveTabType(LeftTab), ActiveTabPos(0), 
		pLastTabVTN(NULL), AnchorPos(Indent) {};
	void AdvanceBy(MILLIPOINT Advance, BOOL IsADecimalPoint);
	BOOL FinishTabSection(VisibleTextNode* pLastFormattedNode, BOOL IsLastTabSection);
	BOOL IsAvailable(MILLIPOINT CharWidth, BOOL IsATab, BOOL IsADecimalPoint);
};

/********************************************************************************************
>	class TextLineInfo: public CC_CLASS_MEMDUMP

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/96
	Purpose:	passing line level info to/from functions
********************************************************************************************/

class TextLineInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(TextLineInfo);

public:
	TextLineInfo();

	MILLIPOINT    SumCharAdvances;	// NB does not include last char tracking
	Justification justification;
	MILLIPOINT    LeftMargin;		// relative to start of line
	MILLIPOINT    RightMargin;		// relative to start of line

	MILLIPOINT    ParaLeftMargin;   // left margin from line level attribute (as opposed to object property for text on path)
	MILLIPOINT    ParaRightMargin;  // (NB. - the ParaLeftMargin value comes from either the LeftMargin or the FirstLine attribute)
	const TxtRuler* Ruler;

	BOOL          WordWrapping;
	INT32         NumChars;
	INT32         NumSpaces;
};


/********************************************************************************************
>	class TextLine:	public BaseTextClass

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	A TextLine gathers together all characters on a single line in a text story.
				The TextLine object is responsible for rendering all its character node
				children. It needs to do this because TextChars have no associated
				bounds and so they can only render themselves relative to other chars.
	SeeAlso:	TextStory
********************************************************************************************/

class TextLine:	public BaseTextClass
{
	CC_DECLARE_DYNAMIC(TextLine)

public:
	TextLine();
	~TextLine();
	TextLine(Node* ContextNode, AttachNodeDirection Direction);
	void Init();
	
	virtual Node* SimpleCopy();
	void CopyNodeContents(TextLine* NodeCopy);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	virtual CopyType GetCopyType();
	virtual INT32 ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput);
	virtual INT32 ComplexHide(UndoableOperation* pOp, Node* pNextNode);

	virtual BOOL IsSetCandidate() const		{ return TRUE; }
	virtual BOOL IsCompound() const			{ return TRUE; }		// See also NodeCompound

	virtual UINT32 GetNodeSize() const; 	
	virtual void GetDebugDetails(StringBase* Str); // Can be deleted before we ship
	virtual String Describe(BOOL Plural, BOOL Verbose);  		   

	DocRect GetBlobBoundingRect();
	void RenderObjectBlobs(RenderRegion* pRRegion);
	void RenderTinyBlobs(RenderRegion* pRRegion);
	DocCoord GetTinyBlobPos();

	BOOL CreateNodeGroup(NodeGroup** ppNodeGroup, FormatRegion* pFormatRegion, BecomeA* pBecomeA);
	static TextLine* CreateEmptyTextLine(Node* pContextNode=NULL, AttachNodeDirection Direction=FIRSTCHILD);

	virtual BOOL ReCacheMetrics(FormatRegion* pFormmatRegion);
	BOOL Format(TextStoryInfo* pStoryInfo);
	BOOL EnsureNextLineOfParagraphHasSameLineLevelAttrs(UndoableOperation* pUndoOp);
	BOOL ReCalcLineInfo(TextLineInfo* pLineInfo);
	BOOL PositionCharsInLine(TextLineInfo* pLineInfo);
	BOOL CalcBaseAndDescentLine(MILLIPOINT* pBaseLine, MILLIPOINT* pDescentLine,
								MILLIPOINT LastDescentLine, BOOL FirstLine);
	BOOL SetCharMatrices(MILLIPOINT LinePos);
	BOOL FitTextToPath(TextStoryInfo* pPathInfo, MILLIPOINT LinePos);
	BOOL Wrap(UndoableOperation* pUndoOp, MILLIPOINT WrapWidth, MILLIPOINT Indent);
	VisibleTextNode* FindBreakChar(MILLIPOINT FitWidth, BOOL SetCharPositions,
								   MILLIPOINT Indent, MILLIPOINT CharPosOffset = 0,
								   MILLIPOINT ExtraOnChars = 0, MILLIPOINT ExtraOnSpaces = 0);

	TextLine* FindFirstLineOfParagraph();
	static BOOL IsAttrTypeLineLevel(CCRuntimeClass* pAttrType);
	BOOL AddChildLineLevelAttrsToSet(AttrTypeSet* pAttrSet);

	VisibleTextNode* FindCharAtDistAlongLine(MILLIPOINT Distance, BOOL* LeftHandSide);
	VisibleTextNode* FindFirstVTN() const;
	VisibleTextNode* FindLastVTN() const;
	EOLNode*         FindEOLNode() const;
	CaretNode*       FindCaret() const;
	TextLine*        FindNextLine() const;
	TextLine*        FindPrevLine() const;
	MILLIPOINT GetLastCharTracking();

	BOOL WholeLineSelected();

	BOOL WillLineWrapOnPath(FIXED16 xscale, MILLIPOINT PLength);

	// v2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter *pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter *pFilter);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);

	// private member access functions
	MILLIPOINT GetLineDescent() { return mLineDescent; }
	MILLIPOINT GetLineAscent()  { return mLineAscent; }
	MILLIPOINT GetLineSize()    { return mLineSize; }
	void SetLineDescent(MILLIPOINT Descent) { mLineDescent = Descent; }
	void SetLineAscent( MILLIPOINT Ascent)  { mLineAscent  = Ascent; }
	void SetLineSize(   MILLIPOINT Size)    { mLineSize    = Size; }
	void UpdateLineDescent(MILLIPOINT Descent) { if (Descent<mLineDescent) mLineDescent = Descent; }
	void UpdateLineAscent( MILLIPOINT Ascent)  { if ( Ascent>mLineAscent ) mLineAscent  = Ascent; }
	void UpdateLineSize(   MILLIPOINT Size)    { if (   Size>mLineSize   ) mLineSize    = Size; }

	Justification GetJustification()   { return mJustification; }
	MILLIPOINT    GetLineSpacing()     { return mLineSpacing; }
	FIXED16       GetLineSpaceRatio()  { return mLineSpaceRatio; }
	MILLIPOINT    GetParaLeftMargin()  { return mLeftMargin; }
	MILLIPOINT    GetParaFirstIndent() { return mFirstIndent; }
	MILLIPOINT    GetParaRightMargin() { return mRightMargin; }
	const TxtRuler* GetRuler()         { return mpRuler; }
	void SetJustification(  Justification justification) { mJustification  = justification; }
	void SetLineSpacing(    MILLIPOINT    Spacing)       { mLineSpacing    = Spacing; }
	void SetLineSpaceRatio( FIXED16       SpaceRatio)    { mLineSpaceRatio = SpaceRatio; }
	void SetParaLeftMargin( MILLIPOINT    Margin)        { mLeftMargin = Margin; }
	void SetParaFirstIndent(MILLIPOINT    Indent)        { mFirstIndent = Indent; }
	void SetParaRightMargin(MILLIPOINT    Margin)        { mRightMargin = Margin; }
	void SetRuler(          const TxtRuler* pRuler)      { if (*mpRuler != *pRuler) *mpRuler = *pRuler; }

	MILLIPOINT GetPosInStory() { return mPosInStory; }
	void SetPosInStory(MILLIPOINT pos) { mPosInStory=pos; }

protected:
    MILLIPOINT GetEffectiveLeftMargin();
private:
	MILLIPOINT mLineDescent;	// largest descent of any char in all fonts on the line
	MILLIPOINT mLineAscent;		// largest  ascent of any char in all fonts on the line
	MILLIPOINT mLineSize;		// largest   size  of any char in all fonts on the line

	Justification mJustification;	// cache for value read from attr stack
	MILLIPOINT    mLineSpacing;		// cache for value read from attr stack
	FIXED16       mLineSpaceRatio;	// cache for value read from attr stack
	MILLIPOINT mLeftMargin;	        // cache for value read from attr stack
	MILLIPOINT mFirstIndent;        // cache for value read from attr stack
	MILLIPOINT mRightMargin;        // cache for value read from attr stack
	TxtRuler* mpRuler;              // cache for value read from attr stack
									// NB - this is a list object owned by this object
	MILLIPOINT mPosInStory;		// y position of base of line relative to story
};

#endif
