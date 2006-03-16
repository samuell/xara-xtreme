// $Id: prnmks.h 662 2006-03-14 21:31:49Z alex $
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
// Header file for Camelots Print Marks Manager.

/*
*/

#ifndef INC_PRINTMARKS
#define INC_PRINTMARKS

#include "list.h"
#include "listitem.h"
#include "lddirect.h"
#include "doccoord.h"
#include "docrect.h"

class String_256;
class NodeGroup;
class PrintMarksMan;
class PrintMarksComponent;
class TextStory;

// Representative of a pseduo mark

#define MAXPAGEREGIONS		16
#define MAXREGIONMARKS		8
#define INCH				72000
#define CROPMARK_THICKNESS	425			// (0.15mm*72000/25.4)
#define CROPMARK_LENGTH		27360		// (38*72000/100) 0.38 inch in length
#define CROPAREA_SIZE		27360		// size reserved for crop marks
#define OUTSIDEBLEEDWIDTH	0			// space to position marks beyond the bleed

/*****************************************************************************************

>	class LoadPrintMarks : public LoadDirect

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/08/96
	Purpose:	A derived class to load the print marks. Uses the winoil class LoadDirect

*****************************************************************************************/

class LoadPrintMarks : public LoadDirect
{
	CC_DECLARE_MEMDUMP(LoadPrintMarks);

	protected:
		virtual BOOL OnLoadDocument(Document* pKernelDoc);
};


/*****************************************************************************************
> enum MarkType - specify all the types of print mark we know about
*****************************************************************************************/

enum MarkType					// These values are used in the file format - do not change
{
	MarkType_Unknown = 0,				// A user mark with an unknown ID, (ie no fabby icon)
	MarkType_Star,
	MarkType_Registration,
	MarkType_ColourBar,
	MarkType_GreyBar,
	MarkType_Information,
	MarkType_Crop
};


/*****************************************************************************************
>	enum MarkRegion, the 20 named regions around the page
*****************************************************************************************/

enum MarkRegion
{
	MarkRegion_TopLeft = 0,		// These values are used in the file format - do not change
	MarkRegion_Top1,
	MarkRegion_Top2,       
	MarkRegion_Top3,       
	MarkRegion_TopRight,
	MarkRegion_Right1,     
	MarkRegion_Right2,     
	MarkRegion_Right3,     
	MarkRegion_BottomRight,
	MarkRegion_Bottom3,    
	MarkRegion_Bottom2,    
	MarkRegion_Bottom1,    
	MarkRegion_BottomLeft,
	MarkRegion_Left1,      
	MarkRegion_Left2,      
	MarkRegion_Left3,

	MarkRegion_FileFormatTerminator = 0xff
};


/*****************************************************************************************
> Class MarkFormat - specify all the formatting types we can cope with
*****************************************************************************************/

class MarkFormat
{
	public:
		MarkFormat();
		
		BOOL IsValid() const;
		void MakeValid();

		friend INT32 operator==(const MarkFormat& x, const MarkFormat& y);

	public:		// Support functions for load/save in native files
		BYTE GetAsFlagByte(void);
		void SetFromFlagByte(BYTE Flags);

	public:
		BYTE	Left	: 1;
		BYTE	Centre	: 1;
		BYTE	Right	: 1;
		BYTE	Top		: 1;
		BYTE	Middle	: 1;
		BYTE	Bottom	: 1;
		BYTE	unused  : 1;
		BYTE	unused1 : 1;
};

/*****************************************************************************************
>	enum MarkOrient, describes the orientation of this mark
*****************************************************************************************/

enum MarkOrient
{
	MO_None = 0,			// These values are used in the file format - do not change
	MO_Vertical = 1,
	MO_Horizontal = 2
};




/*****************************************************************************************

>	class MarkPosition : public ListItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	An instance of a mark lives in a particular region and not only this, 
				contains a format and a transform

*****************************************************************************************/

class MarkPosition : public ListItem
{
	CC_DECLARE_DYNCREATE(MarkPosition);

	public:
		MarkPosition();

		void Initialise();

		void SetRegion(MarkRegion region) { Region = region; }
		void SetFormat(MarkFormat format) { Format = format; }

		inline MarkFormat		GetFormat() const { return Format; }
		inline MarkRegion		GetRegion() const { return Region; }

		BOOL		IsEqualTo(const MarkPosition* pOther) const;

	private:
		MarkRegion	Region;
		MarkFormat	Format;
};


// Default Print Mark ID's - These must NOT be changed, as they are used as unique IDs
// in the native file format. The marks loaded from the mark template file use unique
// IDs from 2 to 7 (the last time I looked).
const BYTE PRINTMARK_CROPMARKID = 0;
const BYTE PRINTMARK_INFOMARKID = 1;


/*****************************************************************************************

>	class PrintMark : public CC_CLASS_MEMDUMP

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	This class contains all the information pertaining to a mark. ie the
				type of mark it is, its menu text, icon id and a list of all the regions
				and formatting rules it uses.

*****************************************************************************************/

class PrintMark : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PrintMark);

	public:
		 PrintMark();
		~PrintMark();

		void		SetCustomOrDefault(BOOL IsCustom)	{ MarkIsCustom = IsCustom; }
		void		SetIDByte(BYTE NewID)				{ IDByte = NewID; }
		void		SetType(MarkType type)				{ Type = type; }
		void		SetMarkMenuText(const String_256* pMenuText);
		void		SetOrientation(MarkOrient orient)	{ Orientation = orient; }
		void		SetDefaultState(BOOL enabled)		{ OnAsDefault = enabled; }

		void		AddNewPosition(MarkPosition* pMarkPos);
		BOOL		DefaultsToOn() const				{ return OnAsDefault; }
		
		BOOL		IsCustom(void) const				{ return(MarkIsCustom); }
		BYTE		GetIDByte(void) const				{ return(IDByte); }
		MarkType	GetType() const						{ return Type; }
		String_256	GetMarkMenuText() const;
		BOOL		FindMarkPosition(const MarkPosition* pSearchMark, MarkPosition** pFoundMark=NULL) const;
		BOOL		IsSimilarTo(const PrintMark* pOther) const;
		
		inline MarkOrient GetOrientation() const		{ return Orientation; }

		inline MarkPosition* GetFirstPosition() const	{ return (MarkPosition*)MarkPositions.GetHead(); }
		
		inline MarkPosition* GetNextPosition(MarkPosition* pMP) const
														{ return (MarkPosition*)MarkPositions.GetNext(pMP); }

	private:
		BOOL			MarkIsCustom;
		BYTE			IDByte;
		MarkType		Type;
		MarkOrient		Orientation;
		BOOL			OnAsDefault;
		String_256		MenuText;
		List			MarkPositions;
};


/*****************************************************************************************

>	class PrintMarkItem : public ListItem
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	The PrintMarkItem contains details about this print mark. It holds the
				mark type, formatting information and a pointer to a subtree of objects
				which represent the mark. It also contains various bits of information
				used by Camelots UI to present the mark to the user.

*****************************************************************************************/

class PrintMarkItem : public ListItem
{
	CC_DECLARE_DYNCREATE(PrintMarkItem);

	friend class PrintMarksCache;

	public:
		 PrintMarkItem();
		~PrintMarkItem();

		inline NodeGroup*  GetMarkGlyph() const	{ return pMarkGlyph; }
		inline UINT32	   GetHandle() const	{ return Handle; }
		inline PrintMark*  GetPrintMark()		{ return &Mark; }
		
		INT32		GetWidth() const;
		INT32		GetHeight() const;
		DocCoord	GetOrigin() const;
		MarkOrient	GetOrient() const;
		BOOL		DefaultsToOn() const;
		BOOL		NeedsSaving() const;
		BOOL		NeedsToRender() const;

	private:
		void		RemoveMark();
		void		SetType(MarkType type)	{ Mark.SetType(type); }
		void		SetHandle(UINT32 hndle)	{ Handle=hndle; }
		BOOL		SetMarkGlyph(NodeGroup* pMark, BOOL MakeCopy=FALSE);
		void		SetSaveable(BOOL state);
		void		SetRenderable(BOOL state);

	private:
		UINT32		Handle;
		PrintMark	Mark;
		NodeGroup*	pMarkGlyph;
		BOOL		Saveable;
		BOOL		Renderable;
};







/*****************************************************************************************

>	class PageMarkRegion
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	Holds the position and orientation of a page mark region. ie the region
				a page mark will be formatted in. There are current 16 regions around
				a typical page each having a vertical or horizontal orientation.
	SeeAlso:

*****************************************************************************************/

class PageMarkRegion
{
	public:
		PageMarkRegion();
	
	public:
		BOOL		OrientX;
		BOOL		Valid;
		DocRect		Pos;
};


/*****************************************************************************************

>	class PageMarkRegions
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	A simple structure holding the positions of all 16 page mark regions
				around a typical page. The constructor of this class creates the regions
				given the bounding rect of a page.
	SeeAlso:	PageMarkRegion

*****************************************************************************************/

class PageMarkRegions
{
	public:
		PageMarkRegions();
		
		void Initialise(const DocRect& ParentPage, MILLIPOINT Bleed);
		
		void HideLeft();
		void HideRight();
		void HideTop();
		void HideBottom();

		void ShowLeft();
		void ShowRight();
		void ShowTop();
		void ShowBottom();

	public:	
		PageMarkRegion	Region[MAXPAGEREGIONS];
};






/*****************************************************************************************

>	class MarkPosType
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:
	SeeAlso:

*****************************************************************************************/

class MarkPosType
{
	public:
		MarkPosType();

		PrintMarkItem *pMarkItem;
		DocCoord pos;
		DocCoord dim;
		BOOL rotate;
};


/*****************************************************************************************

>	class MarkList
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	Holds an array of up to 8 marks formatted into a particular position eg
				middle centre in a particular region (of which there can be 16 around a
				page).
	SeeAlso:

*****************************************************************************************/

class MarkList
{
	public:
		MarkList();

	public:	
		INT32		count;
		DocCoord	curpos;
		DocCoord	size;
		MarkPosType MarkPos[MAXREGIONMARKS];
};

/*****************************************************************************************

>	class MarkFormatRegion
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/96
	Purpose:	The mark formatting region. Given a doc rect ie the position of the
				region around the page, this class can be used to work out the position
				of the marks which need to be formatted in this region ie those marks
				which have been specified by the user to fall into this position. As
				one adds marks, the formatting region updates itself and keeps track
				of all mark positions. You can then call the formatting regions render
				function which will use the positions so generated to render each mark
				within it. On rendering, each mark will be asked to render itself at
				a given position.
	SeeAlso:

*****************************************************************************************/

class MarkFormatRegion : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(MarkFormatRegion);

	public:
		 MarkFormatRegion();
		 MarkFormatRegion(const DocRect &Position);
		~MarkFormatRegion();

		 void Empty();
		 void SetPosition(const DocRect& Position);
		 void SetOrientation(BOOL orientX=TRUE);
		 void SetFlipRender(BOOL flip);
		 
		 virtual BOOL AddMark(PrintMarkItem* pMark, MarkFormat *pForm);
		 virtual void Render(RenderRegion *pRRegion, const Matrix& Transform);
		 
	protected:
		 virtual BOOL MakeMark(PrintMarkItem *pMark, 
							   DocCoord Anchor,
							   BOOL cx,
							   BOOL cy,
							   BOOL posx,
							   BOOL posy,
							   INT32 shiftx,
							   INT32 shifty,
							   BOOL rotate,
							   MarkList **pMarkList);

	private:
		void  Render(RenderRegion *pRRegion, MarkList *pItem, const Matrix& Transform);
		void  RenderMark(RenderRegion* pRRegion, PrintMarkItem *pMark);
		Node* FindMarkChildToRender(Node* pParent, RenderRegion *const pRender);
		Node* FindMarkNextForClippedInkRender(Node* pRoot, Node* pNode, RenderRegion *pRender);

	private:
		 void Init();

	private:						
		DocRect	  TheBounds;		// The position of this region
		INT32	  MkCx;				// the centre of this region
		INT32	  MkCy;
		BOOL	  OrientX;			// Orientation of this mark region
		BOOL	  OnRenderFlipX;	// Flip all marks in this region about X before rendering

		MarkList* BLeft;
		MarkList* BCentre;
		MarkList* BRight;
		MarkList* MLeft;
		MarkList* MCentre;
		MarkList* MRight;
		MarkList* TLeft;
		MarkList* TCentre;
		MarkList* TRight;
};



/*****************************************************************************************

>	class PageRect : public ListItem
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	Stores a rectangle and thats it. When printing, the print manager adds
				rectangles to this list
	SeeAlso:	

*****************************************************************************************/

class PageRect : public ListItem
{
	CC_DECLARE_DYNCREATE(PageRect);

	public:
		PageRect() {};
		PageRect(const DocRect &rect);

	public:
		DocRect	Rect;
};
	


/********************************************************************************************

>	class PageRectList : public List

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/07/96
	Purpose:	Keeps a list of page rectanges in order to service the crop mark renderer

********************************************************************************************/

class PageRectList : public List
{
	CC_DECLARE_DYNAMIC(PageRectList)
	
	public:
		PageRectList() {};
		virtual ~PageRectList();

		BOOL AddPageRect(const DocRect &rect);
		void DeleteAll();

		inline PageRect* GetFirstPageRect() const;
		inline PageRect* GetNextPageRect(PageRect* pItem) const;

		PageRect* FindPageRect(const DocRect &rect) const;
};



/*****************************************************************************************

>	class PrintMarksCache : public CC_CLASS_MEMDUMP
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	The print marks cache. This class contains a list of cached printmarks
				kept by the print marks manager. It is split into its own class here to
				avoid complications will the mark managers handling of cache marks and
				document component marks.
	SeeAlso:	

*****************************************************************************************/

class PrintMarksCache : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PrintMarksCache);

	friend class PrintMarksMan;
	friend class LoadPrintMarks;
	friend class PrintMarksComponent;

	public:
		 PrintMarksCache();
		~PrintMarksCache();

		PrintMarkItem* GetFirstMark() const;
		PrintMarkItem* GetNextMark(PrintMarkItem* pCurrItem) const;
		PrintMarkItem* FindMark(UINT32 SearchHandle);

	private:
		BOOL	DecodeCachedLayers(Document* pDocument);
		BOOL	DecodeCachedLayer(Layer* pLayer);
		BOOL	DecodeMarkFormat(const String_256* pFormatString, PrintMark* pMark);
		UINT32	DecodeToMarkType(const String_256* Name) const;
		BOOL	DoesMarkExist(PrintMark* pCheckMark) const;
		UINT32	AddNewMark(PrintMarkItem* pCheckMark, NodeGroup* pGroup);
		void	UpdateMarkGlyph(PrintMarkItem *pMarkItem, NodeGroup *pGroup);
		BOOL	CreateTextInfoMark();
		BOOL	CreateCropMark();
		BOOL	UpdateTextInfoMark(Document* pDocument, RenderRegion* pRRegion);
		void	DestroyTextInfoMark();
		void	BuildTextInfoString(Document *pDocument, RenderRegion *pRegion, String_256 *pString);
		BOOL	SetInfoMarkAttributes(TextStory *pStory, String_64 *pFontName, MILLIPOINT FontSize);
		BOOL	DeleteChildAttribute(NodeRenderableInk* pParent, CCRuntimeClass* pReqdAttrib);

		UINT32	GetUniqueHandle();
		void	Invalidate();

	private:
		UINT32	NextHandle;
		UINT32	TextInfoHandle;
		UINT32	CropMarkHandle;
		List	PrintMarkCache;
		
		String_256	CachedTime;
};


/*****************************************************************************************

>	class PrintMarksMan : public CC_CLASS_MEMDUMP
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/07/96
	Purpose:	The 'Print Marks Manager'. This manager handles the installation, formatting
				and rendering of printer marks. It liaises with the print box to provide
				a UI to enable and disable print marks.
	SeeAlso:	Specification of print marks inside 'ImageSetting Phase 3' documentation

*****************************************************************************************/

class PrintMarksMan : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PrintMarksMan);
	
	public:
		PrintMarksCache PMMCache;

		 PrintMarksMan();
		~PrintMarksMan();

		BOOL IsTemplateCached() const;
		BOOL ConstructCache();
		void InvalidateCache();

		BOOL AddMarkToDoc(UINT32 MarkHandle, Document* pDocument);
		BOOL RemoveMarkFromDoc(UINT32 MarkHandle, Document* pDocument);
		void ConvertAllDocColours(Document* pDoc);
		BOOL AddDefaultMarksToDoc(Document* pDocument);

		BOOL ShowPrintMarks(Document *ScopeDoc) const;
		void RenderPrintMarks(Document *ScopeDoc, BOOL DisplayEm=TRUE, BOOL LoadTemplateMarks=TRUE);
		void RenderPrintMarks(PrintMarksComponent *pComp, 
							  RenderRegion *pRRegion,
							  const Matrix &Transform,
							  const DocRect &ClipRect,
							  Spread *pSpread);

		void StartPrinting();
		BOOL AddPageRect(const DocRect &Rect);
		void ResetPageRects();
		void SetBleed(MILLIPOINT bleed);
		void SetEmulsionDown(BOOL state);
		void SetImagesettingRect();
		void EndPrinting();
		
		MILLIPOINT GetCropMarkWidth(Document *ScopeDoc) const;
		void ClipToBleed(RenderRegion *pRRegion, Spread *pSpread);

	private:
		void CompileMarkRegion(PrintMarksComponent *pComp, MarkRegion CurrRegion);
		void RenderPrintMarksAroundRect(PrintMarksComponent *pComp, 
										RenderRegion *pRRegion,
										const Matrix *pTrans,
										const DocRect &Rect,
										MILLIPOINT bleed);
		void RenderCropMarksAroundRect(	RenderRegion *pRRegion,
										const Matrix *pTrans,
										const DocRect &Rect,
										MILLIPOINT bleed);
		MILLIPOINT AdjustedBleed() const;

	private:
		BOOL TemplateCached;

	private:
		MarkFormatRegion MFRegion;
		DocRect			 ImagesettingRect;
		PageRectList	 PageRects;
		MILLIPOINT		 Bleed;
		BOOL			 EmulsionDown;
};


#endif
