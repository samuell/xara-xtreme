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
// CorelDRAW filter

/*
*/


#ifndef INC_CDRFILTER
#define INC_CDRFILTER

#include "filters.h"
#include "colcomp.h"
#include "impcol.h"
#include "rifffile.h"
#include "fixst256.h"
#include "cdrform.h"
#include "bitmap.h"
#include "cdrbitm.h"
#include "txtattr.h"

class TextStory;

typedef enum {
	CDRVERSION_NULL = 0,
	CDRVERSION_3 = 3,
	CDRVERSION_4 = 4,
	CDRVERSION_5 = 5,
	CDRVERSION_6 = 6
} CDRVersion;

/********************************************************************************************

>	class CDRAttributeStore : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	22 03 95	
	Purpose:	A storage object for attributes from a CDR file import

********************************************************************************************/

class CDRAttributeStore : public List
{
	CC_DECLARE_MEMDUMP(CDRAttributeStore);

public:
	virtual BOOL AddChunkToStore(RIFFFile *RIFF);
	
	// find chunks routines, return 0 if they couldn't be found
	ADDR Find(DWORD reference, INT32 *Size);		// find by double word reference
	ADDR Find(WORD reference, INT32 *Size);		// find by single word reference

private:
};


/********************************************************************************************

>	class CDRAttributeStoredItem : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	22 03 95	
	Purpose:	A list item object for attributes from a CDR file import

********************************************************************************************/

class CDRAttributeStoredItem : public ListItem
{
friend class CDRAttributeStore;
friend class CDRArrowheadStore;
friend class CDRArrowheadStoredItem;
friend class CDRFontnameStore;

	CC_DECLARE_MEMDUMP(CDRAttributeStoredItem);

public:
	CDRAttributeStoredItem(void);
	~CDRAttributeStoredItem();

	BOOL Aquire(RIFFFile *RIFF);	

protected:
	INT32 Size;
	ADDR Block;
};


/********************************************************************************************

>	class CDRFontnameStore : public CDRAttributeStore

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	22 03 95	
	Purpose:	A storage object for fontnames from a CDR file import

********************************************************************************************/

class CDRFontnameStore : public CDRAttributeStore
{
	CC_DECLARE_MEMDUMP(CDRFontnameStore);

public:
	virtual BOOL AddChunkToStore(RIFFFile *RIFF, CDRVersion Version);
	virtual BOOL AddFontToStore(DWORD Reference, TCHAR *Fontname);
	virtual WORD GetHandleForReference(DWORD Reference);

private:
};


/********************************************************************************************

>	class CDRFontnameStoredItem : public CDRAttributeStoredItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	22 03 95	
	Purpose:	A list item object for fontnames from a CDR file import

********************************************************************************************/

class CDRFontnameStoredItem : public CDRAttributeStoredItem
{
friend class CDRAttributeStore;
friend class CDRFontnameStore;

	CC_DECLARE_MEMDUMP(CDRFontnameStoredItem);

	BOOL GetTypefaceForUse(DWORD Reference, TCHAR *FontName);

public:
	DWORD	Reference;
	WORD	hTypeface;

private:
};


/********************************************************************************************

>	class CDRArrowheadStore : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	22 03 95	
	Purpose:	A storage object for arrowheads from a CDR file import

********************************************************************************************/

class CDRArrowheadStore : public CDRAttributeStore
{
	CC_DECLARE_MEMDUMP(CDRArrowheadStore);

public:
	BOOL AddChunkToStore(RIFFFile *RIFF);

	NodeRenderableBounded *GetConvertedNode(DWORD Reference, INT32 *Distance, BOOL *NotPresent);	

private:
};


/********************************************************************************************

>	class CDRArrowheadStoredItem : public CDRAttributeStoredItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	22 03 95	
	Purpose:	A list item object for arrowheads from a CDR file import

********************************************************************************************/

class CDRArrowheadStoredItem : public CDRAttributeStoredItem
{
friend class CDRAttributeStore;
friend class CDRArrowheadStore;

	CC_DECLARE_MEMDUMP(CDRArrowheadStoredItem);

public:
	CDRArrowheadStoredItem(void);
	~CDRArrowheadStoredItem();

private:
	NodeRenderableBounded *pNode;	// the cached converted arrowhead
};


/********************************************************************************************

>	class CDRVectorStoredItem : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	19/04/95	
	Purpose:	A storage item for vectors from a CDR file import

********************************************************************************************/

class CDRVectorStoredItem : public ListItem
{
friend class CDRFilter;
friend class CDRVectorStore;
	CC_DECLARE_MEMDUMP(CDRVectorStoredItem);

public:
	CDRVectorStoredItem();
	~CDRVectorStoredItem();
	KernelBitmap *GetBitmap(Document *pDocument);

private:
	Node *Objects;
	DWORD Reference;
	DocRect BBox;
	KernelBitmap *pBitmap;
};


/********************************************************************************************

>	class CDRVectorStore : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	19/04/95	
	Purpose:	A storage object for vectors from a CDR file import

********************************************************************************************/

class CDRVectorStore : public List
{
friend class CDRFilter;
	CC_DECLARE_MEMDUMP(CDRVectorStore);

public:
	Node *Find(DWORD Reference, DocRect *BBox);
	CDRVectorStoredItem *Find(DWORD Reference);
};


/********************************************************************************************

>	class CDRBitmapStore : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	10/04/95	
	Purpose:	A storage object for bitmaps from a CDR file import

********************************************************************************************/

class CDRBitmapStore : public CDRAttributeStore
{
	CC_DECLARE_MEMDUMP(CDRBitmapStore);

public:
	BOOL AddChunkToStore(RIFFFile *RIFF, BOOL Pattern, CDRFilter *C);

	KernelBitmap *FindBitmap(DWORD Reference);
private:
};


/********************************************************************************************

>	class CDRBitmapStoredItem : public CDRAttributeStoredItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	10/03/95	
	Purpose:	A list item object for bitmaps from a CDR file import

********************************************************************************************/

class CDRBitmapStoredItem : public CDRAttributeStoredItem
{
friend class CDRAttributeStore;
friend class CDRBitmapStore;

	CC_DECLARE_MEMDUMP(CDRBitmapStoredItem);

public:
	CDRBitmapStoredItem();
	~CDRBitmapStoredItem();

private:
	KernelBitmap *pBitmap;
	FOURCC Reference;
	BOOL Used;
};


/********************************************************************************************

>	class CDRLayerList : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A list object to store layers in a page during the conversion process

********************************************************************************************/

class CDRLayerList : public List
{
	CC_DECLARE_MEMDUMP(CDRLayerList);

public:
	CDRLayerList();
	~CDRLayerList();

private:
};


/********************************************************************************************

>	class CDRLayerListItem : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A list object to store layers in a page during the conversion process

********************************************************************************************/

class CDRLayerListItem : public ListItem
{
friend CDRLayerList;
friend class CDRFilter;

	CC_DECLARE_MEMDUMP(CDRLayerListItem);

public:
	CDRLayerListItem();
	~CDRLayerListItem();

private:
	String_256 Name;
	
	Node *Objects;			// node at the top of this layers tree, 0 if there aren't any
							// objects on this layer
	BOOL LayerInDocument;	// used to mark whether a layer has been inserted into the
							// document during the final insertation phase of the conversion
							// process

	BOOL IsVisible;			// layer attribute thingys
	BOOL IsLocked;
};


/********************************************************************************************

>	class CDRPageList : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A list object to store all my pages in during the conversion process

********************************************************************************************/

class CDRPageList : public List
{
	CC_DECLARE_MEMDUMP(CDRPageList);

public:
	CDRPageList();
	~CDRPageList();

private:
};


/********************************************************************************************

>	class CDRPageListItem : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A list object to store all my pages in during the conversion process

********************************************************************************************/

class CDRPageListItem : public ListItem
{
friend CDRPageList;

	CC_DECLARE_MEMDUMP(CDRPageListItem);

public:
	CDRPageListItem();
	~CDRPageListItem();

	CDRLayerList Layers;		// layers in this page, public for ease of access in CDRFilter

private:
};


/********************************************************************************************

>	class CDRActionList : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A list object to store actions which need to be taken after lists in the 
				CDR file have ended

********************************************************************************************/

class CDRActionList : public List
{
	CC_DECLARE_MEMDUMP(CDRActionList);

public:

private:
};


/********************************************************************************************

>	class CDRActionListItem : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A listitem object to store actions which need to be taken after lists in the 
				CDR file have ended

********************************************************************************************/

class CDRActionListItem : public ListItem
{
friend CDRActionList;

	CC_DECLARE_DYNAMIC(CDRActionListItem);

public:
	CDRActionListItem() {Level = 0;};

	UINT32 GetLevel() {return Level;}
	void SetLevel(UINT32 Lev) {Level = Lev;}

	// do the action
	virtual BOOL DoAction(CDRFilter *C) = 0;
	// called if the import fails at any stage - the action should ensure nothing's lost	
	virtual void ImportFailure(CDRFilter *C) = 0;

private:
	UINT32 Level;			// Level at which this action takes place
};


/********************************************************************************************

>	class CDRActionTransform : public CDRActionListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	23 03 95	
	Purpose:	A transform action

********************************************************************************************/

class CDRActionTransform : public CDRActionListItem
{
friend CDRActionList;
friend CDRFilter;

	CC_DECLARE_DYNAMIC(CDRActionTransform);

public:
	CDRActionTransform() {Chunk = 0; LinkTrans = FALSE; IsAGroupTransform = FALSE;};
	~CDRActionTransform();

	BOOL Init(RIFFFile *RIFF, CDRVersion Version);
	
	BOOL DoAction(CDRFilter *C);
	void ImportFailure(CDRFilter *C);

	void SetIsAGroupTransform(BOOL IsOne) {IsAGroupTransform = IsOne;};

public:
	ADDR Chunk;
	INT32 ChunkSize;
	BOOL LinkTrans;
private:
	BOOL IsAGroupTransform;
};


/********************************************************************************************

>	class CDRActionGroupEnd : public CDRActionListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	26 03 95	
	Purpose:	A group end

********************************************************************************************/

class CDRActionGroupEnd : public CDRActionListItem
{
friend CDRActionList;
friend CDRFilter;

	CC_DECLARE_DYNAMIC(CDRActionGroupEnd);

public:
	CDRActionGroupEnd() {pOldLevelNodeList = 0;};

	BOOL DoAction(CDRFilter *C);
	void ImportFailure(CDRFilter *C);

private:
	NodeRenderable *pOldLevelNodeList;
};

/********************************************************************************************

>	class CDRBBoxList : public List

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	01/05/95
	Purpose:	A list of bounding boxes for the CDR filter

********************************************************************************************/

class CDRBBoxList : public List
{
public:
	CDRBBoxList();
	~CDRBBoxList();

	BOOL Add(Node *N, DocRect *BBox, Justification Just);
	BOOL Find(Node *N, DocRect *BBox);
	DocRect *Find(Node *N);
};


/********************************************************************************************

>	class CDRBBoxListItem : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	01/05/95
	Purpose:	A bounding box list item

********************************************************************************************/

class CDRBBoxListItem : public ListItem
{
friend CDRBBoxList;

public:
	CDRBBoxListItem();
	~CDRBBoxListItem();

private:
	Node *pNode;		// the node this bbox is associated with
	DocRect BBox;		// and it's bbox
};


/********************************************************************************************

>	class CDRTextOnPathLink : public ListItem

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	01/05/95
	Purpose:	A bounding box list item

********************************************************************************************/

class CDRTextOnPathLink : public ListItem
{
	CC_DECLARE_MEMDUMP(CDRTextOnPathLink);

public:
	CDRTextOnPathLink();
	~CDRTextOnPathLink();

public:
	INT32 PathSerialNumber;		// -1 for no thingy
	NodePath *pPath;			// pointer to path or zero - the real thing, not a copy
	INT32 TextSerialNumber;		// -1 for no thingy
	TextStory *pTextStory;		// pointer to text story
	Justification Justi;
	/*BaseShiftEnum*/ INT32 Position;
};

/********************************************************************************************

misc structures

********************************************************************************************/

typedef struct {
	INT32 Angle;
	INT32 Pad;
	INT32 HorizontalCentreOffset;
	INT32 VerticalCentreOffset;
} CDRFillInfo;

typedef struct
{
	INT32 SizeX;
	INT32 SizeY;
	INT32 OffsetX;
	INT32 OffsetY;
	DWORD Reference;
} CDRTiledFillInfo;

#define CDRSCRIPT_NONE	0
#define CDRSCRIPT_SUB	1
#define CDRSCRIPT_SUPER	2

typedef struct
{
//	TCHAR *FontName;
	DWORD FontReference;
	MILLIPOINT FontSize;
	BOOL Bold;
	BOOL Italic;
	BOOL Underline;
	UINT32 Script;
	MILLIPOINT LineSpace;
	INT32 ParaLineSpacePercent;		// line spacing percentage set in text style block
	Justification Just;
} CDRTextStyle;

typedef struct {
	DocRect OriginalBBox;
	DocCoord NewCorners[4];
} cdrPerspective;

typedef struct {
	FLOAT8 a, b, c, d, e, f;
} cdrMatrix;
// note that this is not the same element order as Camelot uses... the calculation is
//    x, = ax + by + c
//	  y, = dx + ey + f

typedef struct {
	FLOAT8	cf11;
	FLOAT8	cf21;
	FLOAT8	cf31;
	FLOAT8	cf41;
	FLOAT8	cf12;
	FLOAT8	cf22;
	FLOAT8	cf32;
	FLOAT8	cf42;
	FLOAT8	cf13;
	FLOAT8	cf23;
	FLOAT8	cf33;
	FLOAT8	cf43;
} cdr3DMatrix;

typedef struct {
	FLOAT8	x,y,z;
} cdr3DCoord;

typedef struct {
	cdr3DMatrix	Trans;
	cdr3DCoord	Camera;
} cdrExtrudeInfo;

/********************************************************************************************

>	class CDRFilter : public VectorFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	21 03 95	
	Purpose:	Corel Draw file importing class	

********************************************************************************************/

class CDRFilter : public VectorFilter
{
friend CDRActionTransform;
friend CDRActionGroupEnd;
friend CDRBitmapStore;
friend CDRBitmap;

	CC_DECLARE_DYNAMIC(CDRFilter);

public:
	CDRFilter();
	BOOL Init();
	
	INT32 HowCompatible(PathName& Filename, 
					ADDR  HeaderStart, 
					UINT32 HeaderSize,
					UINT32 FileSize);

	BOOL DoImport(SelOperation*, CCLexFile*, Document*, 
				BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL,
				KernelBitmap** ppImportedBitmap = NULL,
				DocCoord* pPosTranslate = NULL, String_256* = NULL);

	BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

public:
	static INT32 BitmapNumber;		// the number of the next bitmap to import (used for names)
	static INT32 PatternNumber;		// number of next pattern
	static void DeleteNodeList(Node *);
	static BOOL GetCorelBBox(NodeRenderableBounded *pNode, DocRect *BBox);
	static ADDR FindDataInObject(cdrfOffsetHeader *Header, WORD Type);
	static BOOL AddElementsToPath(Path *pPath, INT32 NCoords, cdrfCoord *Coords, BYTE *Types, BOOL *IsClosed);

private:
	class Document *pDocument;		// document
	class Spread *pSpread;			// spread to import to
	class Page *pPage;				// page to import to

	CDRVersion Version;				// version of the file we're importing
	BOOL Success;					// whether the import suceeded

	RIFFFile *RIFF;					// pointer to RIFF file object
	UndoableOperation *ThisOp;		// the operation we're doing

	BOOL SlowJobStarted;			// have started the indication thing
	UINT32 ProgressCount;				// count of number of times ProgressUpdate called

	BOOL InMasterPage;
	BOOL DoneMasterPage;			// master page handling things

	BOOL DoneDoc;					// done doc list?

	BOOL UseLayers;					// importing on layers?

	CDRAttributeStore Fills;		// store for fills
	CDRAttributeStore Outlines;		// outlines
	CDRAttributeStore Styles;		// styles
	CDRFontnameStore Fonts;			// fonts
	CDRArrowheadStore Arrowheads;	// arrowheads (has a special derviced store which
									// converts arrowheads into nodes)
	CDRBitmapStore Patterns;		// patterns (1bpp things with colours applied to them)
	CDRBitmapStore Bitmaps;			// bitmaps for the file
	CDRVectorStore Vectors;			// vectors for the file (things like full col pattern/texture fills)

	CDRVectorStore TextV4;			// text objects from version 4 files

	CDRPageList Pages;				// list to store all my nice pages in during conversion
	CDRPageListItem *ThisPage;		// pointer to the list entry we're working on
	CDRLayerListItem *ThisLayer;	// pointer to the layer we're working on

	CDRActionList Actions;			// list of actions to perform when list ends are detected

	CDRBBoxList TextBBoxes;			// list of bounding boxes of text nodes

	INT32 PageX;						// page size (same for all pages in file)
	INT32 PageY;
	BOOL Landscape;					// whether the page is landscape or not

	DocCoord Origin;				// the origin of the page

	INT32 TranslateByX;				// distance to translate the coords by in the x direction
	INT32 TranslateByY;				// y

	INT32 ObjectsConverted;			// number of objects converted OK
	INT32 ObjectsFormatWrong;		// number of objects found with a wrong format
	INT32 ObjectsUnknown;			// number of unknown objects
	INT32 EnvelopesApproximated;		// number of envelopes which have been approximated

	BOOL FormatError;				// whether the current object had a format error

	NodeRenderable *pLevelNodeList;	// a linked list of nodes which have been created on this level
	NodeRenderable *pMadeNode;		// the node which has just been made but not linked in to the tree

	INT32 SerialNumber;				// serial number of object currently being converted

	BOOL ObjFilled;					// last object converted should be filled
	BOOL ObjStroked;				// last object should be stroked

	BOOL IsText;					// current object is a spot of text
	BOOL IsTextStory;				// the current object is a text story - apply bodges and remove
									// functionality
	BOOL AttrsAlreadyApplied;		// used when arrowheads are applied to an object

	ADDR ObjDataBlock;				// block to put the object data in
	INT32 ObjDataBlockSize;			// size of the block

	ColourList *ColList;			// documents colour list for adding local colours

	ADDR TransformChunk;			// a pointer to the transform chunk
	INT32 TransformChunkSize;		// and the size of it

	ADDR FontTableV3;				// pointer to font table for version 3 files
	INT32 FontTableEntriesV3;			// number of entries
	ADDR FontStylesV3;				// pointer to font styles block for version 3 files

	ADDR LinkTable;					// pointer to the link table

	BOOL LinkTransformsExist;		// whether there are some link transforms in the action list (not necessarily correct but updated in ProcessObject)

	List TextOnPathLinks;			// list of links from text to paths
	BOOL IsTextOnAPath;				// flag for whether to search the link table for this one...

private:
	BOOL ClearUp(void);
	BOOL UpdateProgress(BOOL Now = FALSE);

	// process large elements from CDR files
	BOOL ProcessDoc(void);
	BOOL ProcessPage(void);
	BOOL ProcessLayer(void);
	BOOL ProcessObjectTree(UINT32 Level);
	BOOL ProcessObject(void);
	BOOL ProcessGroup(UINT32 Level, BOOL FromLink = FALSE);
	BOOL ProcessLink(void);
	BOOL ProcessVectorList(ADDR Block, INT32 Size);
	BOOL ProcessVectorListEntry();
	BOOL ProcessTextList4();
	BOOL ProcessTextListItem4();

	// convert objects
	BOOL ConvertRectangle(cdrfOffsetHeader *);
	BOOL ConvertEllipse(cdrfOffsetHeader *);
	BOOL ConvertPath(cdrfOffsetHeader *);
	BOOL ConvertText(cdrfOffsetHeader *);
	BOOL ConvertText4Art(cdrfOffsetHeader *);
	BOOL ConvertText4Para(cdrfOffsetHeader *);
	BOOL ConvertText3(cdrfOffsetHeader *);
	BOOL ConvertBitmap(cdrfOffsetHeader *);

	// attribute setting functions
	BOOL SetLineAttr(cdrfOffsetHeader *Object);
	BOOL SetFillAttr(cdrfOffsetHeader *Object);
	BOOL SetLineAttr3(cdrfOffsetHeader *Object);
	BOOL SetFillAttr3(cdrfOffsetHeader *Object);
	BOOL SetFillAttrFlat(cdrfFillHeader *Fill, INT32 Size);
	BOOL SetFillAttrGrad(cdrfFillHeader *Fill, INT32 Size);
	BOOL SetUKFill(void);			// sets up the fill defn I use when I don't know about a fill type
	BOOL SetLinearGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour);
	BOOL SetRadialGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour);
	BOOL SetConicalGrad(CDRFillInfo *GFill, DocRect &BBox, DocColour *StartColour, DocColour *EndColour);
	BOOL SetFillAttrPattern(CDRTiledFillInfo *Info, DocColour *Colour1, DocColour *Colour2);
	BOOL SetFillAttrTexture(cdrfFillHeader *Fill, INT32 Size);
	BOOL SetFillAttrVector(CDRTiledFillInfo *Info);
 	BOOL NoFill(void);

	// arrowhead functions
	BOOL AddAttributesToArrowheadNode(NodeRenderableBounded *N, DocColour *Col, INT32 LineWidth,
			LineCapType Cap, JointType Join);
	BOOL AddAttributesToArrowheadPath(NodePath *P, DocColour *Col, INT32 LineWidth,
			LineCapType Cap, JointType Join);
	BOOL DoesPathNeedArrowheads(NodePath *P);
	BOOL TransformArrowhead(NodeRenderableBounded *N, DocCoord *Point, DocCoord *Other, BOOL Start,
			INT32 LineWidth, INT32 Distance);
	BOOL AddArrowheadsToPath(DWORD StartArrow, DWORD EndArrow, DocColour *Col, INT32 LineWidth,
			LineCapType Cap, JointType Join);
		
	// misc functions
	BOOL ConvertColour(cdrfColour *Col, DocColour *Out);
	BOOL IsTransformJustMatrices(ADDR Trans, INT32 Size);
	BOOL TransformConvertedObject(NodeRenderable **N, ADDR Trans, INT32 Size, BOOL TranslateForPage = TRUE, BOOL IsAGroupTransform = FALSE, BOOL WorkAtOrigin = FALSE);
	BOOL TransformConvertedObjectDoMatrix(NodeRenderable **N, cdrMatrix *Ma, BOOL TranslateForPage);
	BOOL TransformConvertedObjectDoPerspective(NodeRenderable **N, cdrPerspective *P, BOOL TranslateShapeForPage = FALSE);
	BOOL TransformConvertedObjectDoEnvelope(NodeRenderable **N, Path *P, DocRect *OriginalBBox, INT32 *Corners, BOOL TranslateShapeForPage = FALSE);
	BOOL TransformConvertedObjectDoExtrude(NodeRenderable **N, cdrExtrudeInfo *Info, BOOL TranslateShapeForPage = FALSE);
	BOOL TransformConvertedObjectDoExtrudeTransPath(NodePath *pPath, cdrExtrudeInfo *Info);
	BOOL GetMatrixFromTransform(Matrix *M, ADDR Trans, INT32 Size, BOOL TranslateForPage = TRUE);
	BOOL GetObjCorelBBox(NodeRenderableBounded *pNode, DocRect *BBox);
	KernelBitmap *BitmapFromNodeList(Node *List);
	BOOL MakeTextOnPathLinkList();
	BOOL AttachTextToPaths();
	BOOL CheckTextForLinks(TextStory *pTextStory, cdrfOffsetHeader *Header);

	// text style functions
	BOOL GetTextStyleFromCDRStyle(CDRTextStyle *TS, WORD StyleRef, BOOL SearchForParent = TRUE);
	BOOL GetTextStyleFromDefn(CDRTextStyle *TS, cdrfTextInfoFontDefn *FontDefn, const CDRTextStyle *BasedOn);
	BOOL ApplyTextAttr(Node *ContextNode, const CDRTextStyle *TS, const CDRTextStyle *BasedOn);
	BOOL ApplyTextAttrDoApply(Node *ContextNode, TxtBaseClassAttribute *Attr, BOOL CheckExisting);
	TCHAR *GetFontName(WORD FontRef);
	BOOL GetTextStyle3(CDRTextStyle *TS, WORD Style, CDRTextStyle *BasedOn = 0);
	BOOL GetTextStyleFromChar4(CDRTextStyle *TS, cdrfTextCharStyledV4 *Char, CDRTextStyle *BasedOn);

public:
	// this function is used by the CMX filter to add the same standard set of colours to CMX documents
	static BOOL AddStandardColourSet(Document *pDocument);

	// and this function is used to see if a file has a CMX file inside it
	static BOOL HasCDRFileGotCMXFile(PathName *FileName, CCLexFile *pFile = NULL);
};
                
/********************************************************************************************

>	class CDRTransformConvertor : public CCObject

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>		
	Created:	26 03 95	
	Purpose:	Corel trfd transform block convertor - hides differences in format between
				versions.

********************************************************************************************/

typedef enum {
	CDRTRANSFORMTYPE_UNKNOWN,
	CDRTRANSFORMTYPE_MATRIX,
	CDRTRANSFORMTYPE_PERSPECTIVE,
	CDRTRANSFORMTYPE_ENVELOPE,
	CDRTRANSFORMTYPE_EXTRUDE
} CDRTransformType;

class CDRTransformConvertor : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(CDRTransformConvertor);

public:
	virtual BOOL Init(ADDR Data, INT32 Size) = 0;
	virtual UINT32 GetNTransforms() = 0;						// returns the number of transforms
	virtual CDRTransformType GetTransformType(UINT32 n) = 0;	// gets the type of transform
	virtual BOOL GetMatrix(cdrMatrix *M, UINT32 n) = 0;		// gets a matrix
	virtual BOOL GetPerspective(cdrPerspective *P, UINT32 n) = 0;		// gets a perspective
	virtual BOOL GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n) = 0;
	virtual BOOL GetExtrude(cdrExtrudeInfo *Info, UINT32 n) = 0;
protected:
	BOOL ConvertPerspective(cdrPerspective *P, cdrfPerspectiveTransform *Trans);
	BOOL ConvertEnvelopeBBox(ADDR Entry, DocRect *BBox, INT32 *Corners);
	BOOL ConvertEnvelopePath(ADDR List, INT32 ListSize, INT32 Entry, Path **ppPath);
	BOOL ConvertExtrudeInfo(cdrfTransformExtrude *pTransform, cdrExtrudeInfo *pInfo);
};


class CDRTransformConvertor5 : public CDRTransformConvertor
{
	CC_DECLARE_MEMDUMP(CDRTransformConvertor5);

public:
	BOOL Init(ADDR Data, INT32 Size);
	UINT32 GetNTransforms() {return CDRDATA_WORD(Trans->NTransforms);};
	CDRTransformType GetTransformType(UINT32 n);
	BOOL GetMatrix(cdrMatrix *M, UINT32 n);
	BOOL GetPerspective(cdrPerspective *P, UINT32 n);
	BOOL GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n);
	BOOL GetExtrude(cdrExtrudeInfo *Info, UINT32 n);
private:
	cdrfTransform *Trans;
	ADDR List;
	INT32 ListSize;
};

class CDRTransformConvertor4 : public CDRTransformConvertor
{
	CC_DECLARE_MEMDUMP(CDRTransformConvertor4);

public:
	BOOL Init(ADDR Data, INT32 Size);
	UINT32 GetNTransforms() {return CDRDATA_WORD(Trans->NTransforms);};
	CDRTransformType GetTransformType(UINT32 n);
	BOOL GetMatrix(cdrMatrix *M, UINT32 n);
	BOOL GetPerspective(cdrPerspective *P, UINT32 n);
	BOOL GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n);
	BOOL GetExtrude(cdrExtrudeInfo *Info, UINT32 n);
private:
	cdrfTransformV4 *Trans;
	ADDR List;
	INT32 ListSize;
};

class CDRTransformConvertor3 : public CDRTransformConvertor
{
	CC_DECLARE_MEMDUMP(CDRTransformConvertor3);

public:
	BOOL Init(ADDR Data, INT32 Size) {Trans = (cdrfTransformV3 *)Data; return TRUE;};
	UINT32 GetNTransforms() {return CDRDATA_WORD(Trans->NTransforms);};
	CDRTransformType GetTransformType(UINT32 n);
	BOOL GetMatrix(cdrMatrix *M, UINT32 n);
	BOOL GetPerspective(cdrPerspective *P, UINT32 n);
	BOOL GetEnvelope(Path **P, DocRect *OriginalBBox, INT32 *Corners, UINT32 n);
	BOOL GetExtrude(cdrExtrudeInfo *Info, UINT32 n);
private:
	cdrfTransformV3 *Trans;
};

#endif  // INC_CDRFILTR

