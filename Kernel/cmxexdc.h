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
// CMX export device context

#ifndef __CMXEXDC_H
#define __CMXEXDC_H

#include "kerneldc.h"
#include "list.h"
#include "cmxform.h"
#include "pathtype.h"
#include "docrect.h"
#include "gradtype.h"
#include "attr.h"
#include "arrows.h"
#include "bitmap.h"

class Filter;
class CCDiskFile;
class Rect;
class CMXRenderRegion;
class DocColour;
class FillGeometryAttribute;
class KernelBitmap;
class CMXReferBitmap;
class CMXReferListItem;
class CMXReferPage;
class CMXExportDC;
class Operation;
class Document;
class PathName;

// how many overlapping lenses we allow
#define CMX_MAX_OVERLAP_LENSES		3

// the size of the cache for future values
#define	CMXFUTUREVALUECACHESIZE 	128

// macro for helping to set up future values
#define FUTURE_BACKLOC(obj, mem) (0-(sizeof(*obj) - ( ((INT32)(&((obj)->mem))) - ((INT32)(obj)) )))

// base class for information written, but not know yet
class CMXFutureUpdater : public CCObject
{
	CC_DECLARE_DYNAMIC(CMXFutureUpdater);

public:
	CMXFutureUpdater();
	~CMXFutureUpdater();
	BOOL Init(CMXExportDC *pDC, INT32 RelativePos, UINT32 DataSize);
	virtual BOOL Write(CMXExportDC *pDC) = 0;
protected:
	BOOL WriteValue(CMXExportDC *pDC, INT32 Value);
	virtual void Reset(void) {};
private:
	INT32 AbsPosition;
	UINT32 DataSize;
};

// thing for writing lengths
class CMXFutureLength : public CMXFutureUpdater
{
	CC_DECLARE_DYNAMIC(CMXFutureLength);

public:
	CMXFutureLength();
	BOOL SetLengthStartPos(CMXExportDC *pDC, INT32 RelativePos);
	BOOL Write(CMXExportDC *pDC);
protected:
	void Reset(void);
private:
	INT32 StartPos;
};

// thing for writing numbers
class CMXFutureNumber : public CMXFutureUpdater
{
	CC_DECLARE_DYNAMIC(CMXFutureNumber);

public:
	BOOL Write(CMXExportDC *pDC);

	INT32 operator=(INT32 eq)	{return Number=eq;};
	INT32 operator++()		{return ++Number;};
	INT32 operator++(INT32)	{return Number++;};
	INT32 operator--()		{return --Number;};
	INT32 operator--(INT32)	{return Number--;};
	operator INT32()			{return Number;};
	
private:
	INT32 Number;
};

// thing for writing indexes (offsets to things later on in the file)
class CMXFutureIndex : public CMXFutureUpdater
{
	CC_DECLARE_DYNAMIC(CMXFutureIndex);

public:
	BOOL Write(CMXExportDC *pDC);
};

#define cmxINDEX_NONE			0
#define cmxINDEX_ARROW			1
#define cmxINDEX_BITMAP			2
#define cmxINDEX_EMBEDFILE		3
#define cmxINDEX_LAYER			4
#define cmxINDEX_PAGE			5
#define cmxINDEX_PROCEDURE		6
#define cmxINDEX_MASTERLAYER	7
#define cmxINDEX_MASTERINDEX	8

#define cmxDESC_NONE			0
#define cmxDESC_ARROW			1
#define cmxDESC_ARROWHEADS		2
#define cmxDESC_DOTDASH			3
#define cmxDESC_FONT			4
#define cmxDESC_IMAGE			5
#define cmxDESC_OUTLINE			6
#define cmxDESC_LINESTYLE		7
#define cmxDESC_PEN				8
#define cmxDESC_LENS			9
#define cmxDESC_SCREEN			10
#define cmxDESC_COLOUR			11

/********************************************************************************************

>	class CMXExportDC : public ExportDX

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/6/96
	Purpose:	Export device context for CMX files.
	SeeAlso:	ExportDC

********************************************************************************************/

class CMXExportDC : public ExportDC
{
public:
	CMXExportDC(Filter *);
	BOOL Init(CCLexFile*, CMXRenderRegion *pReg, BOOL bThirtyTwoBit, BOOL ClipboardExport);

	// data definitions local to this class
	typedef enum {
		CMXSECTION_NONE,		// not proccessing one
		CMXSECTION_HEADER,
		CMXSECTION_PAGE,
		CMXSECTION_REFLIST,
		CMXSECTION_INDEX,
		CMXSECTION_ARROW,
		CMXSECTION_ARROWHEADS,
		CMXSECTION_COLOUR,
		CMXSECTION_DOTDASH,
		CMXSECTION_FONT,
		CMXSECTION_IMAGE,
		CMXSECTION_OUTLINE,
		CMXSECTION_LINESTYLE,
		CMXSECTION_PEN,
		CMXSECTION_LENS,
		CMXSECTION_SCREEN,
		CMXSECTION_TRAILER,
		CMXSECTION_IMAGINFO,
		CMXSECTION_IMAGDATA,
		CMXSECTION_PROCBITFILL,
		CMXSECTION_PREVIEWBITMAP
	} CMXSectionType;

	// structure for caching future numbers
	typedef struct {
		INT32 Location;
		INT32 Data;
		UINT32 DataSize;
	} CMXFutureDataCacheEntry;
	
	// Functions to do the actual output to the CMX file.
	BOOL StartSection(CMXSectionType Type);
	BOOL EndSection(void);
	BOOL StartRIFFList(INT32 Type);
	BOOL EndRIFFList(void);
	BOOL CreateHeader(DocRect *pFileBBox);
	BOOL PolishOffThatFile(void);
	BOOL WriteNumber(INT32 Position, UINT32 DataSize, INT32 Value);
	BOOL WriteBBox(DocRect *BBox, BOOL Transform = TRUE);
	BOOL WriteString(StringBase *Str);
	void CleanUp(void);
	BOOL WriteReferenceList(void);
	BOOL WriteData(void *data, INT32 Size) {ExportFile->write(data, Size); return TRUE;};
	void WriteByte(BYTE Thing) {ExportFile->put(Thing);};	// will exception not return false
	BOOL WriteIndex(void);
	BOOL WriteAnIndex(INT32 Which);
	BOOL WriteADesc(INT32 Which);
	BOOL WriteDescriptionSections(void);
	BOOL WriteMatrix(Matrix *pMatrix = NULL);
	INT32 GetGroupCount(void) {return PageGroupCount;};
	static void CalcCorelBBox(DocCoord *Coords, INT32 NumCoords, DocRect *Result);
	void WriteAngle(double theta);
	BOOL WriteReferSections(void);
	void WriteSizeInFile(WORD Size) {if(ThirtyTwoBit) ExportFile->write(&Size, sizeof(Size));};
	BOOL WriteCamComment(void);
	BOOL WritePreviewBitmap(void);

	// access functions
	void SetTransMatrix(Matrix *pM)
	{
		pMatrix = pM;
		FIXED16 sf;
		pMatrix->Decompose(&sf);
		ScaleFactor = sf.MakeDouble();
	};

	Matrix *GetTransMatrix(void) {return pMatrix;};
	INT32 GetFilePosition();
	BOOL IsThirtyTwoBit() {return ThirtyTwoBit;};
	INT32 GetFirstInstrFilePosition(void) {return FirstInstrFilePosition;};
	INT32 GetCurrentInstrFilePosition(void) {return CurrentInstrFilePosition;};
	INT32 GetRefListOffset(void) {return RefListOffset;};
	INT32 GetInstructionTally(void) {return Tally;};
	WORD GetLayerNumber(void) {return LayerNumber;};
	CMXRenderRegion *GetRenderRegion(void) {return pRenderRegion;};
	double GetScaleFactor(void) {return ScaleFactor;};
	void SetInfo(Operation* op, PathName* ptPath, Document* ptDoc) {pOp = op, pPath = ptPath, TheDocument = ptDoc;};

	// functions to handle structural information
	BOOL StartPage(DocRect *BBox, BOOL IsProcedure = FALSE);
	BOOL EndPage(void);
	BOOL StartLayer(String_256 &Name);
	BOOL EndLayer(void);
	BOOL StartGroup(DocRect *BBox, BOOL TransformBBox = TRUE);
	BOOL EndGroup(void);
	void SetCMXFlag(INT32 flag);

	// functions to handle small structure information
	BOOL WriteCommand(UINT32 Tag, void *Data = 0, UINT32 Size = 0);
	BOOL StartCommand(UINT32 Tag);
	BOOL EndCommand(void);
	BOOL WriteTag(INT32 Tag, void *Data = 0, UINT32 Size = 0)
		{return DoWriteTag(Tag, Data, Size, FALSE);};
	BOOL StartTag(INT32 Tag);
	BOOL EndTag(void);
	BOOL StartNestedTag(INT32 Tag);
	BOOL EndNestedTag(void);
	BOOL WriteNestedTag(INT32 Tag, void *Data = 0, UINT32 Size = 0)
		{return DoWriteTag(Tag, Data, Size, TRUE);};
	// the following fn is necessary as some end tags require the full tag
	// but some don't. 
	BOOL WriteMinEndTag(void) {if(ThirtyTwoBit) WriteByte(cmxTAG_EndTag); return TRUE;};

	// attributes
	BOOL WriteAttributes(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL ForceNoFill = FALSE);
	BOOL WriteBlankAttributes(INT32 Tag);
	BOOL WriteAttrCheckLens(CMXRenderRegion *pReg, INT32 Tag, DocCoord *Coords, INT32 NumCoords, BOOL *WasLens);
	BOOL WriteAttrCheckIgnoreLens(CMXRenderRegion *pReg, DocCoord *Coords, INT32 NumCoords);
	BOOL WriteOutlineSpec(CMXRenderRegion *pReg);
	BOOL WriteFillSpec(CMXRenderRegion *pReg, DocCoord *Coords, INT32 NumCoords, BOOL NoFill);
	BOOL WriteFillType(WORD FillID);
	BOOL WriteFillSpecFlat(CMXRenderRegion *pReg, FillGeometryAttribute *pAttr);
	BOOL WriteFillSpecGrad(CMXRenderRegion *pReg, FillGeometryAttribute *pAttr, DocCoord *Coords, INT32 NumCoords);
	BOOL WriteFillSpecBitmap(CMXRenderRegion *pReg, FillGeometryAttribute *pAttr, DocCoord *Coords, INT32 NumCoords);

	// references
	WORD GetColourReference(DocColour *pTheLovelyColour);
	WORD GetBitmapReference(KernelBitmap *pTheLovelyBitmap, DocColour *pStartCol, DocColour *pEndCol, EFFECTTYPE Effect, CMXReferBitmap **ppRB = NULL);
	WORD GetProcedureReference(CMXReferListItem *pProc);
	WORD GetArrowReference(ArrowRec *pArrow);

	// thingy
	typedef enum {
		CMXBITMAPCOLOURSOURCE_NONE,
		CMXBITMAPCOLOURSOURCE_LINEANDFILL,
		CMXBITMAPCOLOURSOURCE_FILLGEOMETRY
	} CMXBitmapColourSource;

	// objects
	BOOL WritePath ( DocCoord	*Coords,
					 PathVerb	*Verbs,
					 INT32		NumCoords,
					 BOOL		Filled);

	BOOL WriteBitmap ( KernelBitmap				*pBitmap,
					   DocCoord					*pParallelogram,
					   CMXBitmapColourSource	ColSource = CMXBITMAPCOLOURSOURCE_NONE,
					   BOOL						TransformParallelogram = TRUE,
					   WORD						BitmapReference = 0 );

	static double Round ( double ToRound );

protected:
	DoWriteTag(INT32 Tag, void *Data = 0, UINT32 Size = 0, BOOL Nested = FALSE);
	BOOL WriteFutureDataCache();

private:		// all the lovely data we'll need
	CMXRenderRegion *pRenderRegion;			// pointer to the render region -- we need it occasionally

	CMXSectionType CurrentSection;			// the type of the section we're currently thinking about
	CMXFutureLength SectionLengthFuture;	// the length of the section

	BOOL ThirtyTwoBit;						// the bitness of the file we're writing

	CMXFutureLength FileLengthFuture;		// length of file
	CMXFutureNumber Tally;					// tally of commands in the file
	CMXFutureIndex PageEndIndexFuture;		// for index of end page command
	CMXFutureNumber PageInstructionCount;	// for instructions in page
	CMXFutureNumber PageGroupCount;			// for the number of groups in the page
	CMXFutureNumber PageFlags;				// for the page flags
	CMXFutureNumber LayerInstructionCount;	// for layer...
	CMXFutureNumber LayerFlags;				// for layer flags
	CMXFutureLength CommandLengthFuture;	// for the length of the current command
	CMXFutureIndex IndexSectionFuture;		// for the offset to the index section
	CMXFutureIndex TrailerIndexFuture;		// location of the index section
	CMXFutureIndex ThumbnailIndexFuture;	// location of the index section
	CMXFutureLength TagLengthFuture;		// for the size of each tag
	CMXFutureLength RIFFListLengthFuture;	// for size of riff lists

	BOOL DoingCommand;						// whether we're currently doing a command or not
	BOOL DoingPage;							// whether we're in a page or not
	BOOL DoingLayer;						// whether we're in a layer or not
	BOOL DoingTag;							// whether we're doing a tag
	WORD LayerNumber;						// number of layer we're doing
	BOOL DoingRIFFList;						// doing a RIFF list
	BOOL PageIsProc;						// page is really a procedure

	INT32 NestedTagLevel;						// level of nested tags
	List NestedTagList;
	List ReferList;							// List of things to refer to
	CMXReferPage *PageRefer;				// address of the current page reference
	INT32 RefListOffset;						// offset of reflist chunk in file
	List GroupList;							// list of groups -- stack type thing
	Matrix *pMatrix;						// matrix to transform all exported things by
	INT32 FirstInstrFilePosition;			// position of the first instruction in the file
	INT32 CurrentInstrFilePosition;			// the position of the current instruction in the file

	INT32 NumberOfEntriesUsedInCache;			// number of entries used in the cache
	CMXFutureDataCacheEntry FutureDataCache[CMXFUTUREVALUECACHESIZE];
											// the cache for writing number
	double ScaleFactor;						// scale factor between cam and CMX coords

	// stuff to handle not putting out too many overlapped lens objects
	List LensBBoxes;						// a list of lens bounding boxes to avoid overlapping them too much
	BOOL OverlappedLensesHaveBeenIgnored;	// whether we ignored something
	BOOL AreIgnoreingOverlappedLenses;		// a flag to tell whether the user said we could ignore them
	BOOL HaveAskedUserAboutLenses;			// whether we've asked the user about stuff

	Document *TheDocument;
	Operation *pOp;
	PathName *pPath;
};


// class for putting things into reference lists and other stuff
class CMXReferListItem : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXReferListItem);

public:
	CMXReferListItem(CMXExportDC *pDC) {};

	virtual BOOL IsInReferenceList(void) {return FALSE;};
	virtual BOOL WriteInReferenceList(CMXExportDC *pDC) {return TRUE;};

	virtual INT32 IsInWhichIndex(void) {return cmxINDEX_NONE;};
	virtual BOOL WriteInIndex(CMXExportDC *pDC) {return TRUE;};
	virtual INT32 IndexEntrySize(CMXExportDC *pDC) {return 0;};		// size of entry which goes in index

	virtual INT32 IsInWhichDesc(void) {return cmxDESC_NONE;};
	virtual BOOL WriteInDesc(CMXExportDC *pDC) {return TRUE;};

	virtual BOOL IsASection(void) {return FALSE;};
	virtual BOOL WriteSection(CMXExportDC *pDC) {return TRUE;};

	virtual BOOL IsAProcedure(void) {return FALSE;};
};

// a version of the refer list item which gets an offset of where it is in the exported file
class CMXReferListItemOffset : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferListItemOffset);

public:
	CMXReferListItemOffset(CMXExportDC *pDC);

protected:
	INT32 Offset;
};

// list item for layers
class CMXReferLayer : public CMXReferListItemOffset
{
	CC_DECLARE_DYNAMIC(CMXReferLayer);

public:
	CMXReferLayer(CMXExportDC *pDC) : CMXReferListItemOffset(pDC) {};

	void SetLayerDetails(WORD Number, String_256 *tName);

	BOOL IsInReferenceList(void) {return TRUE;};
	BOOL WriteInReferenceList(CMXExportDC *pDC);

	BOOL IsInWhichIndex(void) {return cmxINDEX_LAYER;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC);

protected:
	WORD LayerNumber;
	String_256 Name;
};

// list item for the default screen attributes
class CMXReferDefaultScreen : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferDefaultScreen);

public:
	CMXReferDefaultScreen(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	INT32 IsInWhichDesc(void) {return cmxDESC_SCREEN;};
	BOOL WriteInDesc(CMXExportDC *pDC);
};

// list item for dot-dash patterns
class CMXReferDotDash : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferDotDash);

public:
	CMXReferDotDash(CMXExportDC *pDC) : CMXReferListItem(pDC) {pDash = 0;};

	void Set(DashRec *ptDash) {pDash = ptDash;};

	INT32 IsInWhichDesc(void) {return cmxDESC_DOTDASH;};
	BOOL WriteInDesc(CMXExportDC *pDC);

	virtual BOOL AreYouThisDotDash(DashRec *ptDash) {return ((*pDash) == (*ptDash))?TRUE:FALSE;};

protected:
	DashRec *pDash;
};

// list item for default dot-dash attr
class CMXReferDefaultDotDash : public CMXReferDotDash
{
	CC_DECLARE_DYNAMIC(CMXReferDefaultDotDash);

public:
	CMXReferDefaultDotDash(CMXExportDC *pDC) : CMXReferDotDash(pDC) {};

	INT32 IsInWhichDesc(void) {return cmxDESC_DOTDASH;};
	BOOL WriteInDesc(CMXExportDC *pDC);

	BOOL AreYouThisDotDash(DashRec *ptDash) {return (ptDash->Elements == 0)?TRUE:FALSE;};
};

#define CMXDEFAULTDOTDASHREF	1

// list item for colours
class CMXReferColour : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferColour);

public:
	CMXReferColour(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	BOOL SetColour(DocColour *pCol) {Colour = *pCol; return TRUE;};
	BOOL AreYouThisColour(DocColour *pCompareColour)
		{return ((*pCompareColour) == (Colour))?TRUE:FALSE;};

	INT32 IsInWhichDesc(void) {return cmxDESC_COLOUR;};
	BOOL WriteInDesc(CMXExportDC *pDC);

protected:
	DocColour Colour;
};

// list item for pens
class CMXReferPen : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferPen);

public:
	CMXReferPen(CMXExportDC *pDC) : CMXReferListItem(pDC) {Width = -1;};

	BOOL SetWidth(MILLIPOINT TWidth) {ERROR2IF(Width != -1, FALSE, "width already set");
										Width = TWidth; return TRUE;};
	BOOL AreYouThisWidth(MILLIPOINT CompareWidth) {return (CompareWidth == Width)?TRUE:FALSE;};

	INT32 IsInWhichDesc(void) {return cmxDESC_PEN;};
	BOOL WriteInDesc(CMXExportDC *pDC);

protected:
	MILLIPOINT Width;
};

// list item for line styles
class CMXReferLineStyle : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferLineStyle);

public:
	CMXReferLineStyle(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	void SetLineStyle(cmxLineStyle *pLineStyle) {LineStyle = *pLineStyle;};
	BOOL AreYouThisStyle(cmxLineStyle *pLineStyle);

	INT32 IsInWhichDesc(void) {return cmxDESC_LINESTYLE;};
	BOOL WriteInDesc(CMXExportDC *pDC);

protected:
	cmxLineStyle LineStyle;
};

// list item for outlines
class CMXReferOutline : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferOutline);

public:
	CMXReferOutline(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	void SetOutline(cmxOutline *pOutline) {Outline = *pOutline;};
	BOOL AreYouThisOutline(cmxOutline *pOutline);

	INT32 IsInWhichDesc(void) {return cmxDESC_OUTLINE;};
	BOOL WriteInDesc(CMXExportDC *pDC);

protected:
	cmxOutline Outline;
};

// list item for arrowheads
class CMXReferArrowheads : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferArrowheads);

public:
	CMXReferArrowheads(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	void SetArrowheads(WORD StartArrow, WORD EndArrow) {Start = StartArrow; End = EndArrow;};
	BOOL AreYouThisArrowheads(WORD StartArrow, WORD EndArrow) {return (Start == StartArrow) && (End == EndArrow);};

	INT32 IsInWhichDesc(void) {return cmxDESC_ARROWHEADS;};
	BOOL WriteInDesc(CMXExportDC *pDC);

protected:
	WORD Start, End;
};

// list item for the actual arrow description
class CMXReferArrow : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferArrow);

public:
	CMXReferArrow(CMXExportDC *pDC) : CMXReferListItem(pDC) {pArrow = 0;};
	
	void Set(ArrowRec *ptArrow) {pArrow = ptArrow;};

	INT32 IsInWhichDesc(void) {return cmxDESC_ARROW;};
	BOOL WriteInDesc(CMXExportDC *pDC);

	BOOL AreYouThisArrow(ArrowRec *ptArrow)
		{return ((ptArrow->ArrowID == pArrow->ArrowID) || (ptArrow == pArrow))?TRUE:FALSE;};

	INT32 IsInWhichIndex(void) {return cmxINDEX_ARROW;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return sizeof(cmxArrowIndexEntry);};

protected:
	INT32 ArrowFilePosition;
	ArrowRec *pArrow;
};

// list item for lenses
class CMXReferLens : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferLens);

public:
	CMXReferLens(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	BOOL Set(CMXExportDC *pDC, DocRect *tBBox);

	BOOL IsInReferenceList(void) {return TRUE;};
	BOOL WriteInReferenceList(CMXExportDC *pDC);

	BOOL IsASection(void) {return TRUE;};
	BOOL WriteSection(CMXExportDC *pDC);

	BOOL IsAProcedure(void) {return TRUE;};

	INT32 IsInWhichIndex(void) {return cmxINDEX_PROCEDURE;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return sizeof(cmxProcIndexEntry)+(pDC->IsThirtyTwoBit()?sizeof(WORD):0);};		// size of entry which goes in index

protected:
	INT32 CommandFilePosition;	// position of the layer instruction
	DocRect BBox;
	INT32 LayerNumber;
	INT32 LensDescFilePosition;
};

// list item for pages
class CMXReferPage : public CMXReferListItemOffset
{
	CC_DECLARE_DYNAMIC(CMXReferPage);

public:
	CMXReferPage(CMXExportDC *pDC) : CMXReferListItemOffset(pDC) {};

	void SetLayerTableOffset(INT32 off) {LayerTableOffset = off;};
	void SetRefListOffset(INT32 off) {RefListOffset = off;};

	BOOL IsInWhichIndex(void) {return cmxINDEX_PAGE;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return sizeof(cmxPageIndexEntry)+(pDC->IsThirtyTwoBit()?sizeof(WORD):0);};

protected:
	INT32 LayerTableOffset;
	INT32 RefListOffset;
};

// list item for indexes
class CMXReferMasterIndex : public CMXReferListItemOffset
{
	CC_DECLARE_DYNAMIC(CMXReferMasterIndex);

public:
	CMXReferMasterIndex(CMXExportDC *pDC) : CMXReferListItemOffset(pDC) {};

	void SetType(WORD tType) {Type = tType;};

	BOOL IsInWhichIndex(void) {return cmxINDEX_MASTERINDEX;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return sizeof(cmxMasterIndexEntry);};

protected:
	WORD Type;
};


// list item for master layer index
class CMXReferMasterLayer : public CMXReferListItemOffset
{
	CC_DECLARE_DYNAMIC(CMXReferMasterLayer);

public:
	CMXReferMasterLayer(CMXExportDC *pDC) : CMXReferListItemOffset(pDC) {};

	BOOL IsInWhichIndex(void) {return cmxINDEX_MASTERLAYER;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return (pDC->IsThirtyTwoBit())?sizeof(cmxMasterLayerEntry32):sizeof(cmxMasterLayerEntry16);};
};


// list item for bitmaps
class CMXReferBitmap : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferBitmap);

public:
	CMXReferBitmap(CMXExportDC *pDC) : CMXReferListItem(pDC)
		{Effect = EFFECT_RGB; Contone = FALSE;};

	void Set(KernelBitmap *pBitmap, DocColour *pStartCol, DocColour *pEndCol, EFFECTTYPE tEffect)
		{if(pStartCol != 0 && pEndCol != 0)
			{StartColour = *pStartCol; EndColour = *pEndCol; Contone = TRUE;}
			Effect = tEffect; Ref.Attach(pBitmap, NULL);};

	BOOL IsInWhichIndex(void) {return cmxINDEX_EMBEDFILE;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return sizeof(cmxEmbedFileIndexEntry)+(pDC->IsThirtyTwoBit()?sizeof(WORD):0);};

	BOOL IsASection(void) {return TRUE;};
	BOOL WriteSection(CMXExportDC *pDC);

	BOOL AreYouThisBitmap(KernelBitmap *pBitmap, DocColour *pStartCol, DocColour *pEndCol, EFFECTTYPE tEffect)
		{return ((pBitmap == Ref.GetBitmap())
			&& ( (!Contone && (pStartCol == NULL)) || ((pStartCol != NULL) && ((*pStartCol) == StartColour)))
			&& ( (!Contone && (pEndCol == NULL  )) || ((pEndCol != NULL  ) && ((*pEndCol)   == EndColour  )))
			&& (tEffect == Effect))
			?TRUE:FALSE;};

	KernelBitmap *GetBitmap(void) {return Ref.GetBitmap();};

protected:
	INT32 BitmapFileOffset;
	KernelBitmapRef Ref;
	DocColour StartColour;
	DocColour EndColour;
	EFFECTTYPE Effect;
	BOOL Contone;
};

#define cmxBITMAPFILLTILESIZE_X		(1024*24)
#define cmxBITMAPFILLTILESIZE_Y		(1024*24)

class CMXReferBitmapFill : public CMXReferListItem
{
	CC_DECLARE_DYNAMIC(CMXReferBitmapFill);

public:
	CMXReferBitmapFill(CMXExportDC *pDC) : CMXReferListItem(pDC) {};

	void Set(WORD Ref) {BitmapReference = Ref;};

	BOOL IsASection(void) {return TRUE;};
	BOOL WriteSection(CMXExportDC *pDC);

	BOOL IsAProcedure(void) {return TRUE;};

	INT32 IsInWhichIndex(void) {return cmxINDEX_PROCEDURE;};
	BOOL WriteInIndex(CMXExportDC *pDC);
	INT32 IndexEntrySize(CMXExportDC *pDC) {return sizeof(cmxProcIndexEntry)+(pDC->IsThirtyTwoBit()?sizeof(WORD):0);};		// size of entry which goes in index

protected:
	WORD BitmapReference;
	INT32 ProcedureFilePosition;
};

// group record class
class CMXGroupRecord : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXGroupRecord);

public:
	CMXGroupRecord(CMXExportDC *pDC);
	BOOL Write(CMXExportDC *pDC);

protected:
	INT32 Offset;
	INT32 TallyAtStart;
	INT32 GroupCountAtStart;
};


// nested tag class
class CMXNestedTagRecord : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXNestedTagRecord);

public:
	CMXFutureLength LengthFuture;
};

class CMXLensBBox : public ListItem
{
	CC_DECLARE_DYNAMIC(CMXLensBBox);

public:
	CMXLensBBox(DocRect *bbox) {BBox = *bbox;};

	BOOL DoesIntersect(DocRect *pBBox) {return BBox.IsIntersectedWith(*pBBox);};

private:
	DocRect BBox;
};

#endif // __ CMXEXDC_H
