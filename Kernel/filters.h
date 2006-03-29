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

#ifndef INC_FILTERS
#define INC_FILTERS

#include "list.h"
#include "pathname.h"
#include "docrect.h"
#include "attr.h"
#include "attrmgr.h"
#include "txtattr.h"
#include "fontbase.h"
#include <limits.h>
#include "oilfltrs.h"

class DocColour;
class CCLexFile;
class CCDiskFile;
class ArrowRec;
class Pathname;


// Every filter should have a unique filter ID which is allocated in this header file. 
#define FILTERID_USERCHOICE			(UINT_MAX)
#define FILTERID_NONE				(0)

#define FILTERID_GENERIC			(1)
#define FILTERID_VECTOR				(2)
#define FILTERID_BITMAP				(3)
#define FILTERID_METAFILE   		(4)
#define FILTERID_AIEPS				(5)
#define FILTERID_AI5EPS				(6)
#define FILTERID_ARTWORKS_EPS		(7)
#define FILTERID_CAMELOT_EPS		(8)
#define FILTERID_COREL3_EPS			(9)
#define FILTERID_COREL4_EPS			(10)
#define FILTERID_ACORN_DRAW			(11)
#define	FILTERID_BMP				(12)
#define	FILTERID_EPS				(13)
#define FILTERID_NATIVE				(14)
#define FILTERID_CORELPALETTEFILE	(15)
#define FILTERID_CDR 				(16)
#define FILTERID_FREEHAND_EPS		(17)
#define FILTERID_ENHMETAFILE		(18)

#define BUILD_NATIVE_FILTERS		FALSE
#define FILTERID_NATIVE_EPS			(19)	// The old version 1.1 native filter
#define FILTERID_WEB				(20)
#define FILTERID_CMX16				(21)
#define FILTERID_CMX32				(22)
#define FILTERID_CMXIMPORT			(23)
#define FILTERID_AI8EPS				(24)

// Flash filter
#define FILTERID_FLASH				(25)	// JCF: added as otherwise returned ID_NONE

// PhotoShop EPS filter
#define FILTERID_PHOTOSHOPEPS		(26)	// (ChrisG 9/11/00) for the PhotoShop EPS Stub filter

// Palette filters
#define FILTERID_PALETTE			(30)
#define FILTERID_MSPALETTE			(31)
#define FILTERID_ADOBECOLOURTABLE	(32)
#define FILTERID_ADOBECOLOURSWATCH	(33)
#define FILTERID_PSPPALETTE			(34)
#define FILTERID_JCWPALETTE			(35)

// Imagemap filter
#define FILTERID_IMAGEMAP			(40)


// Following supplied by Accusoft Image Format Library DLL V5
#define	FILTERID_ACCUSOFT			(100)
#define	FILTERID_ACCUSOFTBMP		(101)
#define	FILTERID_BROOKTROUT			(102)
#define	FILTERID_CALS				(103)
#define	FILTERID_DCX				(104)
#define	FILTERID_GIF				(105)
#define	FILTERID_GX2				(106)
#define	FILTERID_HALOCUT			(107)
#define	FILTERID_IFF				(108)
#define	FILTERID_IMG				(109)
#define	FILTERID_IOCA				(110)
#define	FILTERID_JPEG				(111)
#define	FILTERID_KOFAX				(112)
#define	FILTERID_LASERDATA			(113)
#define	FILTERID_MACPAINT			(114)
#define	FILTERID_MSICONS			(115)
#define	FILTERID_MSPAINT			(116)
#define	FILTERID_PCX				(117)
#define	FILTERID_PHOTOCD			(118)
#define	FILTERID_PHOTOSHOP			(119)
#define	FILTERID_PICT				(120)
#define	FILTERID_SUN				(121)
#define	FILTERID_TARGA				(122)
#define	FILTERID_TIFF				(123)
#define	FILTERID_WPG				(124)
#define	FILTERID_XWINDOWS			(125)
#define FILTERID_PREVIEW			(126)	//WEBSTER-obsolete

#define	FILTERID_TI_GIF				(127)
#define	FILTERID_SPRITE				(128)

#define FILTERID_PPM				(129)
#define FILTERID_PGM				(130)
#define FILTERID_PBM				(131)

#define FILTERID_DXF				(132)

#define FILTERID_PNG				(133)

#define FILTERID_PREVIEW_BMP		(134)
#define FILTERID_PREVIEW_GIF		(135)
#define FILTERID_PREVIEW_JPEG		(136)
#define FILTERID_PREVIEW_PNG		(137)
#define FILTERID_PREVIEW_TIFF		(138)	//WEBSTER new number, no longer FILTERID_PREVIEW

#define FILTERID_IMPORT_JPEG		(140)
#define FILTERID_EXPORT_JPEG		(141)

//#define BUILD_TEXT_FILTERS FALSE
#define BUILD_TEXT_FILTERS TRUE		// Olivier 26/03/97 HTML filter
#define BUILD_OTHER_TEXT_FILTERS	FALSE
#define	FILTERID_TEXT				(200)
#define FILTERID_TEXT_ASCII			(201)
#define FILTERID_TEXT_UNICODE		(202)
#define FILTERID_TEXT_RTF			(203)

// HTML Filter Id
#define FILTERID_HTML				(204)

// Flare Template ID
#define FILTERID_FLARETEMPLATE		(205)

// external Aldus filters need to have a range of 100 (should be plenty)
#define	FILTERID_ALDUS_START		1000
#define	FILTERID_ALDUS_END			1099

// !!!!!!!!!!!!!!!!! DON'T ADD ANY MORE TO THE ABOVE LIST !!!!!!!!!!!!!!!!


/********************************************************************************************

>	class FilterFlags

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	Indicate what functions a filter is capable of.
	SeeAlso:	Filter

********************************************************************************************/

class FilterFlags
{
public:
	UINT32 CanImport : 1;
	UINT32 CanExport : 1;
	UINT32 CanExportMultipleImages : 1;
	UINT32 ShowFilter : 1;

	BOOL operator==(const FilterFlags Flags);
};


/********************************************************************************************

>	class ImportPosition

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/07/94
	Purpose:	Defines where to position an imported file.
	SeeAlso:	Filter; Filter::DoImport

********************************************************************************************/

class Spread;

class ImportPosition
{
public:
	Spread *pSpread;
	DocCoord Position;
};




// Forward references for the Filter class.
class GenericFilter;
class SelOperation;
class Operation;
class Document;
class GenericFilter;
class Layer;
class Spread;
class ExportDC;
class RenderRegion;
class AttributeEntry;
class KernelBitmap;


// Used to indicate what level of bitmap support a filter has.
enum BitmapFilterSupport 
{
	NoBitmapSupport, 
	SimpleBitmapSupport, 
	IndirectedBitmapSupport
};


/********************************************************************************************

>	class SnapShotItem : public ListItem

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/95
	Purpose:	Defines what is saved during a current attribute snap shot.

********************************************************************************************/

class SnapShotItem : public ListItem
{
	CC_DECLARE_MEMDUMP(SnapShotItem);

	friend class SnapShotList;

public:
	SnapShotItem();
	~SnapShotItem();

	INT32 Initialise( AttributeEntry* CurrAttrs );

private:
	// An attribute mask
	NodeAttribute **SnapShotAttrs;
	AttributeEntry *SnapShotAttrsStack;
};


/********************************************************************************************

>	class SnapShotList : public CC_CLASS_MEMDUMP

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/95
	Purpose:	Defines what is saved during a current attribute snap shot.

********************************************************************************************/

class SnapShotList : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(SnapShotList);

	public:
		 SnapShotList();
		~SnapShotList();

		BOOL 			CreateSnapShot(AttributeEntry* CurrAttrs);
		void 			DestroySnapShot();

		SnapShotItem*	GetCurrentSnapShot();
		BOOL			ApplyChangedAttrs(Node* pNode, AttributeEntry* CurrAttrs);
		void			PushAttrsBasedOnSnapShot(AttributeEntry* CurrAttrs);
		void			PopAttrsBasedOnSnapShot(AttributeEntry* CurrAttrs);
	
	private:
		List			SnapShot;
};



/********************************************************************************************
>	class AttrRecordItem : public ListItem

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Used to record the state of an AttributeEntry array.
	SeeAlso:	AttrRecordList
********************************************************************************************/
class AttrRecordItem : public ListItem
{
	CC_DECLARE_MEMDUMP(AttrRecordItem);
	friend class AttrRecordList;

public:
	 AttrRecordItem();
	~AttrRecordItem();

	BOOL Initialise(AttributeEntry* CurrAttrs);
	BOOL Restore(AttributeEntry* CurrAttrs);

protected:
	AttributeEntry *RecordedAttrs;
};


/********************************************************************************************
>	class AttrRecordList : public CC_CLASS_MEMDUMP

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/95
	Purpose:	Used to store a stack of AttributeEntry states. Used by filters that need to
				keep an attribute state stack (eg the RTF filter)
********************************************************************************************/
class AttrRecordList : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(AttrRecordList);

public:
	 AttrRecordList();
	~AttrRecordList();

	BOOL SaveContext(AttributeEntry* CurrAttrs);
	BOOL RestoreContext(AttributeEntry* CurrAttrs);
	List* GetList()			{return &AttrRecord;}

protected:
	List AttrRecord;
};


enum DESTRUCTION_METHOD
{
	DM_INVALID			= 0,	// don't use this, it's for catching duff initialization
	DM_DELETE_WHEN_DONE	= 1,
	DM_KEEP_WHEN_DONE
};

typedef UINT32 ID;
/********************************************************************************************

>	class Filter : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	Pure base class for import and export filters.
				Provides static functions to initialise and deinitialise all the
				available filters.
	SeeAlso:	OILFilter

********************************************************************************************/

class Filter : public ListItem
{
	friend class FilterRenderCallback;

	CC_DECLARE_DYNAMIC(Filter)

public:
	Filter();
	virtual ~Filter();

	virtual BOOL Init() = 0;
	virtual void Deinit()
	{
		if (pOILFilter)
		{
			delete pOILFilter;
			pOILFilter = NULL;
		}
	};

	FilterFlags GetFlags();

	// Use these to override the status-bar messages for typical file exports.  We use it
	// to change the "Saving document..." message "Copying to clipboard..." when we
	// export native data to the clipboard.
	static void ForceStatusExportMessage(UINT32 nMessageID);
	static void UnforceStatusExportMessage();

	// Function to load in the first part of a file so it can be validated before
	// importing it.
	ADDR LoadInitialSegment(PathName& Path, UINT32 *Size, size_t *FileSize);
	
	// Overloaded version of the above that doesn't open or close the file passed to it.
	ADDR LoadInitialSegment(CCLexFile* pFile, UINT32* pSize, size_t *pFileSize);

	// Import filters override these functions:

	// Works out if opening a file of this type requires a default document to be loaded
	// or does the file format supply the document.
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	// Function called before HowCompatible so that filters have a chance to do
	// something, if required. Baseclass version is present but does nothing.
	// Therefore, only needs to be overridden by a filter or set of filters which requires it.
	virtual BOOL PreHowCompatible();

	// Graham 11/4/97: The next three functions used to be pure virtual. However,
	// this meant that export filters derived from Filter had to override functions
	// like DoImport and HowCompatible, which was rather odd. So now these
	// base class functions all return NULL values.
	
	// HowCompatible() returns value in range 0 to 10:
	//   0 => "Definitely Not"
	//  10 => "Yes Sir, that's my file format"
	
	virtual INT32 HowCompatible(	PathName& Filename, 
								ADDR  HeaderStart, 
								UINT32 HeaderSize,
								UINT32 FileSize )
	{
		return 0;
	}

	virtual BOOL DoImport(SelOperation*, CCLexFile*, Document*, 
						  BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL,
						  KernelBitmap** ppImportedBitmap = NULL,
						  DocCoord* pPosTranslate = NULL, String_256* URL = NULL
						  )
	{
		return FALSE;
	}

	// Export filters override these functions:
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL = TRUE)
	{
		return FALSE;
	}

	virtual BOOL JoinFamily(CCRuntimeClass*);

	virtual BOOL ImportBitmap(CCLexFile*, KernelBitmap**);
	virtual BOOL ExportBitmap(KernelBitmap&);

	static GenericFilter* GetGenericFilter() { return pGenericFilter; }

	Document* GetDocument() { return TheDocument; }
	AttributeEntry* GetCurrentAttrs() { return CurrentAttrs; }

	virtual INT32 ImportBinary(ADDR pData, INT32 Length);

	virtual BitmapFilterSupport GetBitmapSupportLevel();

	// Functions to help with the inclusion of Preview Bitmaps
	virtual BOOL CanIncludePreviewBmp();
	void IncludePreviewBmp(BOOL WantBmp);

	// Function which determines if a filter should export invisible layers or not
	virtual BOOL ExportVisibleLayersOnly(); 
			   
	// Function which determines if a filter should export the selected items or not
	virtual BOOL ExportSelectionOnly(BOOL MaskedRender = FALSE); 

	String_256  FilterName;		// That the user sees to choose from.
	String_256  FilterInfo;		// e.g. author, type, version.
	UINT32		FilterID;		// Unique ID for this filter (see above).
	OILFilter  *pOILFilter;

	// This function produces the "Do you want to replace file?" message
	virtual BOOL WillAcceptExistingFile(PathName pthToReplace);

	// Tells Camelot whether or not to include attributes within the saved file. (Only
	// used by the native filters.)
	virtual void SetSaveAttributes (BOOL DoSave);

	void SetSoleLayer(Layer* pLayer) {m_pSoleLayer = pLayer;}
	Layer* GetSoleLayer() {return m_pSoleLayer;}

protected:
	// Support for Filter Preferences
	static BOOL DeclareFilterPreferenceSection();
	//virtual BOOL DeclareFilterPref(const Preference& preference);

	// This (tries to) override the status-line message displayed when a file exports.
	static UINT32 m_nForcedStatusExportMessage;

	// Flag to indicate if we want to attach a Preview bitmap at the end of the export
	BOOL WantPreviewBmp;

	// Flags for the filter
	FilterFlags Flags;

	// The document associated with this filter
	Document* TheDocument;

	// Simple bodge until we position imported files properly.
	Layer *GetActiveLayer(Document*);
	BOOL MakeSureLayerExists(Document* pDoc);

	// Simple bodge until we allow the user to choose which spread to export.
	static Spread* GetFirstSpread(Document*);

	// The string to display when exporting, or 0 if we're not an export filter.
	UINT32 ExportMsgID;

	// A message to be displayed when the export has been stopped, such as pressing
	// escape when using the progress bar/hourglass.
	UINT32 StopExportMsgID;

	// The string to display when importing or 0 if we're not an import filter
	UINT32 ImportMsgID;

	// Should attributes be saved?
	BOOL SaveAttributes;

	Layer* m_pSoleLayer;

public:
	virtual BOOL OpenExportFile(CCDiskFile* pDiskFile, PathName* pPath);
	virtual BOOL DeleteExportFile(CCDiskFile* pDiskFile);

	// Export a spread to an export DC. Now virtual and broken into two parts so that the
	// bitmap export can render in strips.
	virtual	BOOL ExportRender( RenderRegion *pRegion, BOOL MaskedRender = FALSE);
	virtual	BOOL ExportRenderNodes ( RenderRegion*	pRegion,
									 ExportDC*		pDC,
									 BOOL			VisibleLayersOnly		= FALSE,
									 BOOL			CheckSelected			= FALSE,
									 BOOL			ShowProgress			= TRUE );

	// Functions to put correct information on progress bar during import/export 
	virtual String_64 GetImportProgressString(CCLexFile* File, UINT32 ImportingMsgID);
	virtual String_64 GetExportProgressString(CCLexFile* File, UINT32 ExportingMsgID);

	// Baseclass functions for returning the import and export message ids.
	// Baseclass versions just return the ExportMsgID and ImportMsgID
	// Allow somebody to read the export ID and to override it for say multistage exports
	virtual UINT32 GetExportMsgID();
	virtual UINT32 GetImportMsgID();


	// this function is used to add (for instance) 'file 1 of 5.' 
	virtual BOOL AddStringToProgressString(String_64* Str);

	//Graham 22/5/97
	//Determines whether we want to export an HTML tag to the clipboard
	virtual BOOL ShouldExportHTMLTag()
	{
		return FALSE;
	}

	//Actually exports the HTML tag to the clipboard
	virtual void ExportHTMLTag()
	{
	}

	//Function to find a filter given a Filter ID
	static Filter* FindFilterFromID(const UINT32 FilterID);

protected:
	// The body of ExportRenderNodes:
	virtual	BOOL WriteNodes ( RenderRegion*	pRegion,
							  ExportDC*		pDC,
							  BOOL			VisibleLayersOnly,
							  BOOL			CheckSelected,
							  BOOL			ShowProgress);
	virtual BOOL EndExportRender ( RenderRegion	*pRegion,
								   BOOL			ShowProgress );
	
	// see just a line above
	// see also GetProgressString() for the modifications
	String_64 m_StrToAddToProgress;


	// Functions to make it simple to change the current attributes.
	BOOL SetUpCurrentAttrs();
	void DeleteCurrentAttrs();
	BOOL SetLineColour(DocColour&);
	BOOL SetLineWidth(MILLIPOINT);
	BOOL SetLineCap(LineCapType);
	BOOL SetJoinType(JointType);
	BOOL SetLineTransp(UINT32 TranspType, UINT32 Transp);
	BOOL SetDashPattern(DashRec&);
	BOOL SetStartArrow(ArrowRec&);
	BOOL SetEndArrow(ArrowRec&);
	BOOL SetMitreLimit(MILLIPOINT);

	BOOL SetTextFont(String_64 *EncodedName, INT32 Millipoint);
	BOOL SetTextFontStyle(String_64* FName, INT32 MillFSize, BOOL Bold, BOOL Italic);
	BOOL SetTextSize(INT32 Size);
	BOOL SetTextTypeFace(String_64* Name, FontClass Class = FC_UNDEFINED);
	BOOL SetTextBold(BOOL Bold);
	BOOL SetTextItalic(BOOL Italic);
	BOOL SetTextUnderline(BOOL Underline);
	BOOL SetTextAspectRatio(FIXED16 AspectR);
	BOOL SetTextTracking(INT32 Tracking);
	BOOL SetTextJustification(Justification Justify);
	virtual BOOL SetTextLineSpacing(INT32 Type, INT32 EMLSpace, MILLIPOINT MLSpace, double DLSpace);
	BOOL SetTextBaseLine(MILLIPOINT BaseLine);
	BOOL SetTextScript(INT32 rise, INT32 pointsize);
	BOOL SetTextBoldFont(BOOL Bold);
	BOOL SetTextItalicFont(BOOL Italic);
	BOOL RemoveTextScript();

	// Whether or not the path/shape/etc should be filled
	BOOL SetPathFilled(BOOL Filled);

	// Chromatic fill geometries
	BOOL SetFillColour(DocColour&);

	BOOL SetLinearFill(DocColour &StartColour, DocColour &EndColour,
 				       DocCoord StartPoint, DocCoord EndPoint,
   				       DocCoord *EndPoint3 = NULL);

	BOOL SetRadialFill(DocColour &StartColour, DocColour &EndColour,
				       DocCoord StartPoint, DocCoord EndPoint);

	BOOL SetRadialFill(DocColour &StartColour, DocColour &EndColour,
				       DocCoord StartPoint, 
				       DocCoord EndPoint1, DocCoord EndPoint2);

	BOOL SetSquareFill(DocColour &StartColour, DocColour &EndColour,
				       DocCoord StartPoint, 
				       DocCoord EndPoint1, DocCoord EndPoint2);

	BOOL SetConicalFill(DocColour &StartColour, DocColour &EndColour,
				        DocCoord StartPoint, DocCoord EndPoint);

	BOOL SetBitmapFill(KernelBitmap *pBitmap,
					   DocCoord StartPoint, DocCoord EndPoint, DocCoord EndPoint2,
				       DocColour *StartColour = 0, DocColour *EndColour = 0);

	BOOL SetFractalFill(DocColour &StartColour, DocColour &EndColour,
				        DocCoord StartPoint, DocCoord EndPoint1, DocCoord EndPoint2,
						INT32 Seed, double Graininess, double Gravity, double Squash, 
						UINT32 DPI, BOOL Tileable);

	// Transparent fill geometries
	BOOL SetNoTranspFill();

	BOOL SetFlatTranspFill(UINT32 TranspType, UINT32 Transp);

	BOOL SetLinearTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
					         DocCoord StartPoint, DocCoord EndPoint, 
					         DocCoord *EndPoint3 = NULL);

	BOOL SetRadialTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
					         DocCoord StartPoint, DocCoord EndPoint);

	BOOL SetRadialTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
						     DocCoord StartPoint, 
						     DocCoord EndPoint1, DocCoord EndPoint2);

	BOOL SetConicalTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
				        	  DocCoord StartPoint, DocCoord EndPoint);

	BOOL SetBitmapTranspFill(UINT32 TranspType, KernelBitmap *pBitmap,
				        	 DocCoord StartPoint, DocCoord EndPoint, DocCoord EndPoint2,
				        	 UINT32 StartTransp = 0, UINT32 EndTransp = 255);

	BOOL SetFractalTranspFill(UINT32 TranspType,
							  DocCoord StartPoint, DocCoord EndPoint1, DocCoord EndPoint2,
							  INT32 Seed, double Graininess, double Gravity, double Squash, 
							  UINT32 DPI, BOOL Tileable,
				        	  UINT32 StartTransp = 0, UINT32 EndTransp = 255);

	// Chromatic fill effects
	BOOL SetFadeFillEffect();
	BOOL SetRainbowFillEffect();
	BOOL SetAltRainbowFillEffect();

	// Fill mappings
	BOOL SetLinearFillMapping(INT32 Repeat);
	BOOL SetLinearTranspFillMapping(INT32 Repeat);

	// Winding rule
	BOOL SetWindingRule(WindingType);

	virtual BOOL GetDragAndDropTranslation(ImportPosition* pPos, DocRect BoundsRect, Coord* Offset);

	// Save and restore the current attrs list
	BOOL SnapShotCurrentAttrs();
	void PushCurrentAttrsBasedOnSnapShot();
	void PopCurrentAttrsBasedOnSnapShot();
	void DeleteSnapShot();
	void DeleteAttributeList(AttributeEntry* pAttrList);
	BOOL ApplyChangedAttrs(Node* pNode);

	// Render callback progress helper
	virtual void UpdateFilterProgress(UINT32 increment);

	// Pointer to the current attributes.
	AttributeEntry *CurrentAttrs;

	// An attribute mask
	SnapShotList FilterSnapShot;

	// Used to remember the fill colour when the fill flag is set
	AttributeEntry FillAttr;
	BOOL PathFilled; 

private:
	// Pointer to the generic filter used for file drag'n'drop etc.
	static GenericFilter* pGenericFilter;

	// The list of filters that Camelot understands.
	static List FilterList;

	// Going soon...
	static ID		m_NextFilterID;

protected:
	// Minimum line width preference
	static INT32 MinLineWidth;

public:
	static BOOL InitFilters();
	static BOOL DeinitFilters();
	static Filter* GetFirst();
	static Filter* GetNext(Filter*);

	// Say goodbye to this...
	static List& GetFilters()			{	return FilterList;	}
	static ID	GetNewFilterID();
	static ID	RegisterFilter(	Filter* const pFilter);




	// Colour list preference
	static BOOL AddUnnamedColours;

	// Use/don't use layers import preference
	static INT32 ImportWithLayers;

	// Use/don't use layers open preference
	static BOOL OpenWithLayers;

	// Use/don't use layers when opening/importing bitmaps
	// Applies to multiple format bitmaps such as animated GIFs
	static BOOL ImportBitmapsOntoLayers;

	static BOOL bDontWarnBitmapNonMixTransp;

public:
	virtual BOOL IncProgressBarCount(UINT32 n)	{ return TRUE; }	// do nothing
	
	// To provide a smooth progress display for large objects
	BOOL UpdateExportedNodeCount(UINT32 NumNodes = 1);
	
	// We need to access the private variables from basebitmap filter
	UINT32 GetNumNodes();
	UINT32 SetNumNodes(UINT32 NewNumNodes);

// Graeme 22/9/99
// Changed access permission from private to protected.
protected:
	// Used for progress display/update.
	static UINT32 NumNodes;
	static UINT32 UpdateEvery;
	static UINT32 LastExportProgressUpdate;
	static UINT32 CurrentProgressLimit;
protected:
	static UINT32 ProgressOffset;
	static UINT32 m__StripStart;
};



/********************************************************************************************

>	class FilterRenderCallback : public RenderCallback

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/2004
	Purpose:	Handling callbacks from RenderTree
	Notes:		RenderTree function calls the main function in this class when it's about
				to render any Node.

********************************************************************************************/

PORTNOTE("filters","Removed FilterRenderCallback - derived from RenderCallback")
#ifndef EXCLUDE_FROM_XARALX
class FilterRenderCallback : public RenderCallback
{
public:
	FilterRenderCallback(Filter* pFilter, BOOL bCount, BOOL bVisibleLayersOnly, BOOL bSelectedOnly)
	{
		m_pFilter = pFilter;
		m_bCount = bCount;
		m_bVisibleLayersOnly = bVisibleLayersOnly;
		m_bSelectedOnly = bSelectedOnly;
	}

	virtual BOOL BeforeNode(RenderRegion* pRegion, Node* pNode);
	virtual BOOL BeforeSubtree(RenderRegion* pRegion,
								Node* pNode,
								Node** ppNextNode,
								BOOL bClip,
								SubtreeRenderState* pState);

private:
	Filter* m_pFilter;
	BOOL m_bCount;
	BOOL m_bVisibleLayersOnly;
	BOOL m_bSelectedOnly;
};
#endif

/********************************************************************************************

>	class VectorFilter : public Filter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Purpose:	Allows filter families to contain all vector formats.  All filters that
				load vector-based files (EPS, metafiles, and so on), should be derived from
				this class.
				Vector files often contain bitmaps - if in doubt, derive from the
				VectorFilter.
	SeeAlso:	BitmapFilter

********************************************************************************************/

class VectorFilter : public Filter
{
	CC_DECLARE_DYNAMIC(VectorFilter)
};

/********************************************************************************************

>	class BitmapFilter : public Filter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Purpose:	Allows filter families to contain all bitmap formats. All filters that
				load pure bitmap files (TIFF, JPEG, BMP etc) should be derived from this
				class.
				Some file formats contain mixes of vector and bitmap graphics (e.g. EPS) - in
				this case, always derive from the VectorFilter.
	SeeAlso:	VectorFilter

********************************************************************************************/

class BitmapFilter : public Filter
{
	CC_DECLARE_DYNAMIC(BitmapFilter)
};

/********************************************************************************************

>	class CompatibleFilter : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	A simple ListItem derived class to allow a list of filters to be
				maintained - the list holds all the filters that claim to understand a
				particular file (see FilterFamily).
				It is not derived from the Filter class itself because although the Filter 
				class is derived from ListItem too, we want to store a list of pointers to 
				existing filters, rather than actual instances of the Filter class.
				For each entry, a 'compatibility rating' is held, to allow the 
				FilterFamily class (the client of this class) to choose the most
				compatible filter from the list.
	SeeAlso:	CompatibleFilterList; Filter; ListItem

********************************************************************************************/

class CompatibleFilter : public ListItem
{
	CC_DECLARE_MEMDUMP(CompatibleFilter)

public:
	CompatibleFilter(Filter *NewFilter, INT32 NewCompatibility);

	// Pointer to the filter in question.
	Filter *pFilter;

	// Compatibility rating for this filter.
	INT32 Compatibility;
};

/********************************************************************************************

>	class CompatibleFilterList : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Simple list class to allow a list of compatible filters to be maintained.
	SeeAlso:	CompatibleFilter; Filter; FilterFamily; CompatibleFilterList::AddFilter

********************************************************************************************/

class CompatibleFilterList : public List
{
	CC_DECLARE_MEMDUMP(CompatibleFilterList)
public:
	BOOL AddFilter(Filter*, INT32 Compatibility);
};

/********************************************************************************************

>	class FilterFamily : public Filter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Special version of an import filter.
				This filter is actually a subset of the installed filters, to allow the user
				to easily specify a broad category of file rather than the exact file
				format and application version that they want to import (because they might
				not actually know!).
				It understands all the files that all the family of filters
				understand, by the simple expedient of being a generic interface to all
				of them.
				i.e. Given any file, this filter asks all the others how compatible they
					 are with the file.  It then chooses the filter that has the highest
					 compatibility rating, and gets it to import the file.

				It is also used to implement a 'generic' filter, which includes *all*
				filters, so that the user can just click on a file and not have to specify
				the actual filter to use unless they really want to.  We will also use
				this generic filter to handle files that are dropped on Camelot's window.

	SeeAlso:	Filter; Filter::HowCompatible

********************************************************************************************/

class FilterFamily : public Filter
{
	CC_DECLARE_DYNAMIC(FilterFamily)

public:
	FilterFamily();
	~FilterFamily();

	// Works out if opening a file of this type requires a default document to be loaded
	// or does the file format supply the document.
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	Filter* GetBestFilter(CCLexFile* pFile, BOOL ShowWarnings = FALSE);

	INT32 HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);
	virtual BOOL DoImport(SelOperation*, CCLexFile*, Document*, 
			      		  BOOL Auto = FALSE, ImportPosition* Pos = NULL,
						  KernelBitmap** ppImportedBitmap = NULL,
						  DocCoord* pPosTranslate = NULL, String_256* = NULL);
	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);

	virtual BOOL ImportBitmap(CCLexFile*, KernelBitmap**);
	virtual BOOL JoinFamily(CCRuntimeClass*);

	Filter* pCachedBestFilter;
	CCRuntimeClass* pFilterType;
};


/********************************************************************************************

>	class GenericFilter : public FilterFamily

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	A filter for all file types.   This is the default filter that the user
				can import with.
	SeeAlso:	FilterFamily

********************************************************************************************/

class GenericFilter : public FilterFamily
{
	CC_DECLARE_DYNAMIC(GenericFilter);

public:
	GenericFilter();
	BOOL Init();
};

/********************************************************************************************

>	class VectorFilterFamily : public FilterFamily

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	A filter for all vector file types. e.g. EPS, WMF, Draw, etc.
	SeeAlso:	FilterFamily

********************************************************************************************/

class VectorFilterFamily : public FilterFamily
{
	CC_DECLARE_DYNAMIC(VectorFilterFamily);

public:
	VectorFilterFamily();
	BOOL Init();
};

#if BUILD_TEXT_FILTERS
/********************************************************************************************
>	class TextFilter : public Filter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Allows filter families to contain all text formats. All filters that
				load textual files should be derived from this class.
	SeeAlso:	Filter
********************************************************************************************/
class TextFilter : public Filter
{
	CC_DECLARE_DYNAMIC(TextFilter)
};
#endif


/********************************************************************************************

>	class BitmapFilterFamily : public FilterFamily

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	A filter for all bitmap file types. e.g. BMP, JPEG, PCX etc
	SeeAlso:	FilterFamily

********************************************************************************************/

class BitmapFilterFamily : public FilterFamily
{
	CC_DECLARE_DYNAMIC(BitmapFilterFamily);

public:
	BitmapFilterFamily();
	BOOL Init();
};


#if BUILD_TEXT_FILTERS
/********************************************************************************************
>	class TextFilterFamily : public FilterFamily

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	A filter for all text file types. e.g. TXT, RTF
	SeeAlso:	FilterFamily
********************************************************************************************/
class TextFilterFamily : public FilterFamily
{
	CC_DECLARE_DYNAMIC(TextFilterFamily);

public:
	TextFilterFamily();
	BOOL Init();
};
#endif


/********************************************************************************************

>	class GenericEPSFilter : public FilterFamily

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	A filter for all EPS files, regardless of the flavour.
	SeeAlso:	FilterFamily; EPSFilter

********************************************************************************************/

class GenericEPSFilter : public FilterFamily
{
	CC_DECLARE_DYNAMIC(GenericEPSFilter);

public:
	GenericEPSFilter();
	BOOL Init();
};



/********************************************************************************************
>	class PaletteFilterFamily : public FilterFamily

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Purpose:	A filter for all palette files
	SeeAlso:	FilterFamily
********************************************************************************************/

class PaletteFilterFamily : public FilterFamily
{
	CC_DECLARE_DYNAMIC(PaletteFilterFamily);

public:
	PaletteFilterFamily();
	BOOL Init();
};

#endif // INC_FILTERS
