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
// Header for the Text tool implementation

#ifndef INC_TEXTINFO
#define INC_TEXTINFO

//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fontbase.h"
#include "usercord.h"

class TextTool;
class FontDropDown;


enum FontAttribute{JustifyA,BoldA,ItalicA,UnderLineA,AspectRatioA,FontSizeA,FontNameA,BaseLineShiftA,HorizontalKernA,
				   TrackingA,ScriptA,LineSpaceA,LineSpacePercentA,
				   LeftMarginA, RightMarginA, FirstIndentA, RulerA,
				   AutoKernText}; // AutoKernText is not an attribute
enum JustifyMode {JustifyLeft,JustifyRight,JustifyCentre,JustifyFull};
enum ScriptModes {NormalScript,SuperScript,SubScript};
		

/********************************************************************************************

>	class TextInfoBarData :

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/94
	Purpose:	To encapsulate all the data required to interface with the text infobar.

********************************************************************************************/

class TextInfoBarData : public CCObject 
{
	CC_DECLARE_DYNCREATE( TextInfoBarData)


public:
	TextInfoBarData();
	static String_64 	FontName;
	static FontClass 	FontType;
	static JustifyMode 	Justify;
	static BOOL  		Bold;
	static BOOL  		Italic;
	static BOOL  		UnderLine;
	static ScriptModes	CurrentScriptMode;
	static FIXED16 		AspectRatio;
	static MILLIPOINT	FontSize; 
	static MILLIPOINT	BaseLineShift;
	static MILLIPOINT	LineSpace;
	static double		LineSpacePercent;
	static BOOL   		IsLineSpaceAPercent;
	static INT32 		HorizontalKern;
	static INT32 		Tracking;
	static UINT32		PointToLog(double Points){ return (UINT32) (Points*1000);}
 	static double		LogToPoint(UINT32 Log){ return (double)Log/1000;}
	static BOOL			AutoKerning;
};

/********************************************************************************************

>	enum TabStopDragType

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	13/07/06
	Purpose:	Different types of drag operations on the ruler bar

********************************************************************************************/

enum TabStopDragType { DragNew, DragTabStop, DragLeftMargin, DragRightMargin, DragFirstIndent };

/********************************************************************************************

>	class TextRulerBarData:

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	07/07/06
	Purpose:	Keep the current ruler bar display state

********************************************************************************************/

class TextRulerBarData : public CCObject
{
	CC_DECLARE_DYNCREATE(TextRulerBarData)

public:
	TextRulerBarData(): CurrentTabType(LeftTab), IsLeftMarginValid(FALSE), IsFirstIndentValid(FALSE),
        IsRightMarginValid(FALSE), IsRulerValid(FALSE), IsRulerOriginClaimed(FALSE), pNewRuler(NULL),
		TabStopDragRunning(FALSE)
		{ pShownRuler = new TxtRuler; }
	~TextRulerBarData() { if (pShownRuler) delete pShownRuler; }

	TxtTabType CurrentTabType;          // currently chosen tab type (click creates tab of this kind)

	BOOL IsLeftMarginValid:1;
	BOOL IsFirstIndentValid:1;
	BOOL IsRightMarginValid:1;
	BOOL IsRulerValid:1;
	BOOL IsRulerOriginClaimed:1;

	MILLIPOINT LeftMargin;
	MILLIPOINT FirstIndent;
	MILLIPOINT RightMargin;
	TxtRuler* pShownRuler;              // the currently shown ruler - this is a copy that is
										// owned by this object!

	AttrTxtRuler *pNewRuler;            // the new ruler attribute to be applied - this is just
										// transient so need not be destroyed in the destructor

	INT32 CurrentRulerOrigin;           // origin of our ruler section (in user space)
	INT32 CurrentRulerSectionWidth;     // width of our ruler section (-1 for infinite)
	BOOL TabStopDragRunning;
	TabStopDragType CurrentDragType;
};

/********************************************************************************************

>	class FontDataItem : public ListItem

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/94
	Purpose:	A class used for caching Font data.

********************************************************************************************/
/*
class FontDataItem : public ListItem
{
	CC_DECLARE_DYNCREATE( FontDataItem )
friend class TextInfoBarOp;
public:
	FontDataItem();
	FontDataItem(String_64 * Name,UINT32 Size,INT32 Ascent,INT32 Descent);
	
	void SetHandle(WORD Hdle){Handle = Hdle;}
	void SetValid(BOOL State){Valid = State;}
	void SetName(String_64 * Name){FontName = *Name;}
	
	void SetAscent(INT32 A ){Ascent = A;}
 	void SetDescent(INT32 D){Descent = D;}
 	void SetSize(UINT32 S){SizeEM = S;}
 	
 	void SetLogFont(ENUMLOGFONT * pLogFont){ELogFont = *pLogFont;}
	
	ENUMLOGFONT * GetELogFont();

 	String_64 FontName;
	UINT32 SizeEM;
	INT32 Ascent;
	INT32 Descent;
	WORD Handle;
	BOOL Valid;
	ENUMLOGFONT ELogFont;
};
*/

/********************************************************************************************

>	class TextInfoBarOp : public InformationBarOp

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class TextInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( TextInfoBarOp )
	// friend class FontDataItem;  
public:

	TextInfoBarOp();
	~TextInfoBarOp();

	static BOOL Init();				// Added by Simon 20/09/95. To Init CommonAttribSet
	static void DeInit(); 


	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here

	TextTool* pTextTool;

	void InitControls();
  	static BOOL Update(BOOL DoUpdate = FALSE);
	static BOOL UpdateRulerBar(SelRange* pSelection, BOOL DoUpdate = FALSE);

	//static void GetTMetrics(TEXTMETRIC * Metrics, LOGFONT* pLogFont = NULL);
	static void OnFieldChange(FontAttribute ThisChange);
	static void AddFontToCombo(String_64 * FontName);
	BOOL 	StringToDouble(StringBase* pstrIn, double * pnOut,BOOL * IsMultiple);
	BOOL UpdateFieldsAfterTyping();
	
	// update internal data structure
	static void DoKernBumps(UINT32 ButtonID);
	static void DoTrackingBumps(UINT32 ButtonID);
	static void DoLineSpacingBumps(UINT32 ButtonID);
	
	static void DoFontChange();

	static BOOL SetCurrentPointSize(MILLIPOINT PointSize);
	static BOOL SetCurrentAspectRatio(FIXED16 Ratio);
	static BOOL SetCurrentTracking(INT32 Tracking);
	static BOOL SetCurrentHorizontalKern(INT32 Kern);
	static BOOL SetCurrentBaseLineShift(INT32 Kern);
	static BOOL SetCurrentLineSpace(INT32 LineSpace);
	static BOOL SetCurrentLineSpacePercent(double Percent);
	//static BOOL SetCurrentFontName(String_64 * Name,BOOL Cache);
	static BOOL SetCurrentFontName(String_64 * Name, FontClass Type, BOOL Cache);
	static void SetCurrentFontBold(BOOL IsBold);
	static void SetCurrentFontItalic(BOOL IsItalic);
	static void SetCurrentAutoKerning(BOOL IsAutoKerning);
	static void SetCurrentFontUnderLine(BOOL IsUnderLine);
	static void SetCurrentJustify(UINT32 ButtonID);
	static void SetCurrentScript(ScriptModes Script);
	static void EnableGadgets(BOOL Enable);

	static inline BOOL IsRulerOriginClaimed() { return RulerData.IsRulerOriginClaimed; }
	static inline INT32 GetRulerOrigin() { return RulerData.CurrentRulerOrigin; }
	static void ReleaseRuler();
	static void HighlightRulerSection(RulerBase* pRuler, UserRect& UpdateRect);
	static void RenderRulerBlobs(RulerBase* pRuler, UserRect& UpdateRect);
	static BOOL OnRulerClick(UserCoord PointerPos, ClickType Click, ClickModifiers Mods,
							 Spread* pSpread, RulerBase* pRuler);
	static BOOL GetRulerStatusLineText(String_256* pText, UserCoord PointerPos,
									   Spread* pSpread, RulerBase* pRuler);

	static void ForceRulerRedraw();
	static void TabStopDragStarting(TabStopDragType);
	static void TabStopDragFinished();
	static void DoAddTabStop(MILLIPOINT Position);
	static void DoAddTabStop(TxtTabStop NewTabStop);
	static void DoApplyShownRuler();
	static void DoChangeLeftMargin(MILLIPOINT Ordinate);
	static void DoChangeRightMargin(MILLIPOINT Ordinate);
	static void DoChangeFirstIndent(MILLIPOINT Ordinate);

public:
// the current infobar object - allow static member access
	static InformationBarOp * pTextInfoBar;

	static FontDropDown	*NameDropDown;		// Font name drop-down list support for the font list and
	
	// Font Data Caching ---------------------------------------
	// a list of fontdata items - these may or may not relate to an installed font 
	// the valid flag wil set if there is relevant installed font

	// The Cache
	// static List FontCache;
	// Next Handle to allocate
	// static WORD NextHandle;

	// static FontDataItem DefaultFont;

	// used by call back functions
	// static BOOL	FontIsInstalled;
	static BOOL DisplayFontIsInstalled;
	
	// static BOOL InitFontCache();
	
	// set up a default font - used to substitute for an unavailable cached font  
	// static BOOL SetDefaultFont();
	
	// clean up
	// static BOOL DeleteFontCache();
	
	// cache a font
	// static BOOL CacheFont(String_64 * FontName);

	// try to find a named font
	// static INT32 DoesFontExist(String_64 *FontName, BOOL SearchCache, BOOL SearchInstalled);

	// check to see if a font is installed
	// static BOOL IsFontInstalled(String_64 * FontName);
	
	// check to see if the display font is installed
	static BOOL IsDisplayFontInstalled();
	
	// search for a cached font
	// static WORD GetFontHandle(String_64 * FontName);

	// get a cached logfont structure
	// static ENUMLOGFONT * GetCachedLogFont(WORD Handle); 

	// get cached font data for a given handle - will return 
	// data even if font is unavailable
	// static FontDataItem * GetFontData(WORD Handle);
	
	// get cached font data for a given handle - will return
	// the default font if not available
	// static FontDataItem * GetRenderableFontData( WORD Handle);
	
	static INT32 BuildFontList();	
	// called on WM_FONTCHANGE messages
	//static BOOL ValidateCache();
	// ^^^ this enumerates the fonts - the call back calls :---
	// static BOOL ValidateCacheItem(String_64 * FontName);

	// temporary storage for call back functions
	// static void SetTempLogFont(ENUMLOGFONT  * ThisFont){TempLogFont = *ThisFont;}
	// static void SetFontFound(BOOL State){FontIsInstalled = State;}
	// static void SetDisplayFontFound(BOOL State){DisplayFontIsInstalled = State;}

	//static void EncodeFontName(String_64& FontName, String_64& Encoded, INT32 Styles);
	//static INT32 DecodeFontName(String_64& IStringRef, String_64& OStringRef);


	// Super/SubScript constants
	static double SuperScriptSize;
	static double SuperScriptOffset;
	static double SubScriptSize;
	static double SubScriptOffset;

	static void RegainCaretOnOpEnd();

private:
	static TextInfoBarData InfoData;
	static TextRulerBarData RulerData;

	static BOOL FindBitmapSize(ResourceID ID, UINT32* pWidth, UINT32* pHeight);

	// static ENUMLOGFONT 	TempLogFont;
	static void UpdateButtonStates();
	static void UpdateJustifyButtons(BOOL Clear = FALSE);
	static void UpdateScriptButtons();
	static void UpdateGadgets();
	static void RedrawUnitGadgets();
	static void SetLineSpaceGadget();
	static void DoInputError(UINT32 GadgetID);

	static INT32 GetLogicalStoryWidth(TextStory* pStory);

	// static BOOL AddFontToCache(String_64 * FontName,WORD Handle);
	// static INT32	AddTrueTypeFontsToList();
	// static INT32	AddTypeOneFontsToList();
	
	// static void DumpFontCache();
	static Document * pDoc;
	static UnitType CurrentFontUnits ;
	static BOOL RegainCaretAfterOp;

	// Added by simon. This set contains all attribute types that we need to find common attributes for
	// We only need to build this set once during the lifetime of the app.
	static CommonAttrSet CommonAttrsToFindSet; 

	// cached bitmap sizes
	static UINT32 CurrentTabButtonWidth;
	static UINT32 TabBitmapWidth;
	static UINT32 LeftMarginBitmapWidth;
	static UINT32 LeftMarginBitmapHeight;
	static UINT32 RightMarginBitmapWidth;
};
	


/********************************************************************************************

>	class TextInfoBarOpCreate : public BarCreate

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Class for creating TextInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class TextInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create() { return (new TextInfoBarOp); }
};


/********************************************************************************************

>	class TextInfoBarEnumFont : public OILEnumFonts

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	Class for creating TextInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class TextInfoBarEnumFont : public OILEnumFonts
{
	CC_DECLARE_DYNCREATE( TextInfoBarEnumFont )
	
	public:
		TextInfoBarEnumFont() { count=0; }
		INT32 GetCount() { return count; };
		virtual BOOL NewFont(FontClass Class, ENUMLOGFONT FAR* lpelf);
	
	private:
		INT32 count;

};

/********************************************************************************************

>	class TabStopDragOpParam: public OpParam

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	12/07/06
	Purpose:	Parameters for TabStopDragOp operation class

********************************************************************************************/

class TabStopDragOpParam: public OpParam
{
public:
	TabStopDragOpParam(TabStopDragType Type, TxtTabStop DraggedTabStop, UserCoord Pos):
		DragType(Type), DraggedTabStop(DraggedTabStop), StartPos(Pos) {}
	TabStopDragType DragType;
	TxtTabStop      DraggedTabStop;   // only for Type == DragNew or DragTabStop
	UserCoord       StartPos;
};

#define OPTOKEN_TABSTOPDRAG _T("TabStopDrag")

/********************************************************************************************

>	class TabStopDragOp: public Operation

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	12/07/06
	Purpose:	Operation to handle tab stop dragging

********************************************************************************************/

class TabStopDragOp: public Operation
{
	CC_DECLARE_DYNCREATE(TabStopDragOp)
public:
	TabStopDragOp();
	~TabStopDragOp();

	static BOOL Init();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The main entry point
	void DoWithParam(OpDescriptor *pOpDesc, OpParam* pParam);
    void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	ResourceID GetStatusLineID();
	void UpdateStatusLineAndPos(UserCoord* ToolPos, Spread* pSpread);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, Spread*, 
								BOOL Success, BOOL bSolidDrag);

private:
	void DoDrag(Spread* pThisSpread);
	static BOOL IsMouseOverRuler();

	Spread*    pSpread;
	MILLIPOINT Ordinate;
	INT32	   CursorStackID;
	Cursor*	   m_pTabCursor;
	Cursor*    m_pDelCursor;
	BOOL       m_TabCursorShown;
	ResourceID m_CurrentStatusTextID;
	TabStopDragOpParam* m_pParam;
};

#endif 		// INC_TEXTINFO
