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

// kernel class to encompass bitmaps

#ifndef	INC_BITMAP
#define	INC_BITMAP

#include "listitem.h"
#include "doccolor.h"
#include "gradtype.h"
#include "doccomp.h"
//#include "gifutil.h"
//#include "xmlutils.h"
#include "dibutil.h"

class OILBitmap;
class KernelBitmapRef;
class BitmapInfo;
class RenderRegion;
class Filter;
class FillGeometryAttribute;
class BitmapList;
class Document;
class BaseBitmapFilter;
class BaseCamelotFilter;
class CCLexFile;
class View;
class BitmapSource;
class MaskedFilter;

/*********************************************************************************************

>	struct Pixel32bpp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	This structure defines the layout of a 32bpp RGB pixel value, in a nice kernel
				compatible way. They should be used for kernel access to bitmaps so that
				pixels are treated in a platform independent manner.

				The definitions of these pixels are such that a scanline of pixels is a
				packed array of bytes like this:
					Red0 Green0 Blue0 Alpha0 Red1 Green1 Blue1 Alpha1 Red2 Green2 Blue2 Alpha2...

				When not using the Alpha channel, set its value to 0.

	SeeAlso:	OILBtmap::GetScanline32; OILBtmap::PlotPixel; OILBtmap::ReadPixel

**********************************************************************************************/ 

typedef struct
{
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	BYTE Alpha;
} Pixel32bpp;



/*********************************************************************************************

>	PixelGreyscale

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	This value (a BYTE) represents an 8bpp greyscale pixel value

	SeeAlso:	OILBtmap::PlotPixel; OILBtmap::ReadPixel

**********************************************************************************************/ 

typedef BYTE PixelGreyscale;



/*********************************************************************************************

>	class OriginalBitmapRef : public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Purpose:	A reference to the original bitmap, before it was decompressed.

**********************************************************************************************/ 

class OriginalBitmapRef : public CCObject
{
	CC_DECLARE_DYNAMIC( OriginalBitmapRef )

public:
	OriginalBitmapRef(BitmapSource* pSource = NULL, BaseBitmapFilter* pImportFilter = NULL);
	~OriginalBitmapRef();

	void SetOriginalSource(BitmapSource* pSource = NULL, BaseBitmapFilter* pImportFilter = NULL);
	BOOL GetOriginalSource(BitmapSource** ppSource = NULL, BaseBitmapFilter** ppImportFilter = NULL);

protected:
	BitmapSource*		m_pBitmapSource;
	BaseBitmapFilter*	m_pBitmapFilter;
};


/*********************************************************************************************

>	class OILBitmap : public ListItem

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	To encapsulate the OILy end of bitmaps. Each OIL layer inherits from this
				and puts real functionality into the class. Things defined in this class
				must remain kernel-ly.

**********************************************************************************************/ 

class OILBitmap : public ListItem
{
	CC_DECLARE_DYNAMIC( OILBitmap )

	// These are the only classes allowed to create and delete bitmaps
	friend class KernelBitmap;
	friend class GlobalBitmapList;
//	friend class GreyscaleBitmapList;
	friend class OpDeleteBitmap;
	friend class CachedFractal;
	friend class NodeCacheCompound;
	friend class FeatherAttrValue;
	friend class MaskedFilter;
	friend class FlashRenderRegion;
	friend class CBitmapCache;

protected:
	// these are protected because hardly anyone is allowed to create these directly
	OILBitmap();
	virtual ~OILBitmap();

	// makes a bitmap of the correct type for the platform
	static OILBitmap* Create(UINT32 Width, UINT32 Height, UINT32 Depth, UINT32 dpi);

	// Remove the bitmap from the global list
	BOOL RemoveFromGlobalList();

public:
	// static member variables
	static OILBitmap* Default;
	static INT32 s_lMaxGreyscaleSize;

	// public static fns
	static OILBitmap* Create();
	static OILBitmap* Attach( OILBitmap* = Default ,BOOL Temp = FALSE );
	static BOOL Init();
	static void Deinit();

	// public member variables
	virtual INT32 operator==(const OILBitmap&) = 0;
	virtual BOOL GetInfo(BitmapInfo* Info, RenderRegion* pRegion = NULL) = 0;

	virtual UINT32 GetWidth() = 0;
	virtual UINT32 GetHeight() = 0;
	virtual UINT32 GetBPP(RenderRegion* pRegion = NULL) = 0;
	virtual UINT32 GetNumPaletteEntries() = 0;
	virtual ADDR GetBitmapBits() = 0;

	virtual UINT32 GetScanlineSize() = 0;
	UINT32 GetBitmapSize();

	void SetName(String_256& NewName)		{ m_BitmapName = NewName; }
	String_256& GetName()					{ return m_BitmapName; }

	void SetBitmapAnimDelay (UINT32 Delay)	{ m_BitmapAnimDelay = Delay; }
	UINT32 GetBitmapAnimDelay ()				{ return m_BitmapAnimDelay; }

	void SetInterpolation(BOOL Value)		{ m_bInterpolation = Value; }
	BOOL GetInterpolation()					{ return m_bInterpolation ; }

	void SetHidden(BOOL Value)				{ m_bHidden = Value; }
	BOOL IsHidden() const					{ return m_bHidden ; }
	BOOL IsHiddenInGallery();

PORTNOTE("gif","Removed GIF stuff")
#ifndef EXCLUDE_FROM_XARALX
	void SetAnimationRestoreType(GIFDisposalMethod Type)	{ m_AnimationRestoreType = Type; }
	GIFDisposalMethod GetAnimationRestoreType ()			{ return m_AnimationRestoreType; }	
#endif
	void SetLeftOffset(UINT32 LeftOffset)	{ m_LeftOffset = LeftOffset; }
	UINT32 GetLeftOffset()				{ return m_LeftOffset; }
	void SetTopOffset(UINT32 TopOffset)	{ m_TopOffset = TopOffset; }
	UINT32 GetTopOffset()					{ return m_TopOffset; }

	BOOL InvalidateAllReferences(Document*);
	BOOL IsUsedInDocument(Document* pDoc, BOOL bIncludeHidden = FALSE);
	BOOL IsUsedByChildren(Node* pNode, BOOL bIncludeHidden = FALSE);
	BOOL IsUsedByNode(Node* pNode);

	// for loading/saving bitmaps to EPS RenderRegions
	virtual BOOL ExportBitmap(RenderRegion *pRegion) = 0;
	virtual BOOL ExportBitmapPalette(RenderRegion *pRegion) = 0;
	virtual BOOL ExportBitmapData(RenderRegion *pRegion) = 0;

	virtual INT32 GetRecommendedWidth() = 0;
	virtual INT32 GetRecommendedHeight() = 0;
	virtual void SetRecommendedWidth(INT32) = 0;
	virtual void SetRecommendedHeight(INT32) = 0;

	virtual BOOL SetTransparencyIndex(INT32) = 0;
	virtual BOOL GetTransparencyIndex(INT32*) = 0;
	virtual BOOL IsTransparent() = 0;

	// For exporting bitmaps to postscript with colour-separation
	virtual BOOL ExportSeparatedData(RenderRegion *pRegion, BYTE *SepTables) = 0;
	virtual BOOL ExportSeparatedPalette(RenderRegion *pRegion) = 0;

	// Helper function used by ExportSeparatedData - this retrieves one scanline of the
	// bitmap as generic 32bpp RGB data.
	virtual BOOL GetScanline32bpp(UINT32 YPos, BOOL UsePalette, Pixel32bpp *Scanline) = 0;

	// Function used when exporting an 8bpp JPEG into the web file
	virtual BOOL WritePalette(BaseCamelotFilter *pFilter) = 0;
	// Function used when importing that 24bpp JPEG back in and wanting it in 8bpp format
	virtual BOOL Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries) = 0;

	// Function used by the native/web filters to pull a bitmap from inside a bitmap
	// definition record
	virtual BOOL ImportBitmap(CCLexFile* pFile, BaseBitmapFilter* pBitmapFilter,
							  BaseCamelotFilter* pFilter, BOOL IsCompressed) = 0;

	// Function used by the EPS filters to import bitmaps and by the bitmap filter
	// to import a bitmap directly from a bitmap file
	virtual BOOL ImportBitmap(Filter*, const BitmapInfo*, INT32) = 0;

 	virtual BOOL LoadBitmap(UINT32 BitmapResourceID) = 0;

	virtual BOOL GetPaletteUsage(INT32 *) = 0;
	virtual BOOL PrepareMask(INT32 PaletteEntry) = 0;
	virtual BOOL GetScanlineMaskBits(INT32 ScanlineY, LPBYTE ScanlineBuf) = 0;
	virtual BOOL DestroyMask() = 0;

	virtual BOOL BuildContonePalette(DocColour &Start, DocColour &End, EFFECTTYPE Effect, View *ScopeView);
	virtual BOOL ExportContonePalette(RenderRegion *) = 0;
	virtual DocColour GetContonePaletteEntry(UINT32 PaletteIndex);
	virtual BOOL DestroyContonePalette();

	virtual void PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue) = 0;
	virtual void PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue) = 0;
	virtual Pixel32bpp ReadPixel32bpp (INT32 x, INT32 y, BOOL UsePalette = TRUE) = 0;
	virtual INT32 ReturnPaletteIndexUsed( INT32 x, INT32 y ) = 0;
	virtual PixelGreyscale ReadPixelGreyscale (INT32 x, INT32 y) = 0;

	// New function to free the actual bitmap memory
	virtual void DeleteData(BOOL bResetToDefault = TRUE) = 0;
	virtual BOOL HasBeenDeleted() = 0;

	// Functions to access the orginal bitmap file
	virtual void SetOriginalSource(BitmapSource* pSource = NULL, BaseBitmapFilter* pImportFilter = NULL);
	virtual BOOL GetOriginalSource(BitmapSource** ppSource = NULL, BaseBitmapFilter** ppImportFilter = NULL);

	// Functions for contoning
	virtual BOOL SetGreyscaleVersion(OILBitmap*);
	virtual BOOL DestroyGreyscaleVersion();
	virtual OILBitmap* GetGreyscaleVersion() { return m_pGreyscaleVersion; }

	virtual void GenerateGreyscaleTable() = 0;
	BYTE* GetGreyscaleTable() { return m_pGreyscaleTable; }

	BOOL IsGreyscale() { return m_bIsGreyscale; }
	BOOL IsDefaultBitmap() { return this == OILBitmap::Default || HasBeenDeleted(); }
	BOOL IsTemp() { return m_bTemp; }		
	BOOL IsInGlobalList() { return !m_bTemp; }

	void SetAsFractal() { m_bIsAFractal = TRUE;
						  m_bIsGreyscale = TRUE; }

	void SetAsGreyscale() { m_bIsGreyscale = TRUE; };
		// Informs this object that the bitmap it contains *is* a greyscale 8bpp bitmap, and that
		// it therefore doesn't need to make a copy in GetGreyscaleVersion()

	BOOL IsAFractal() { return m_bIsAFractal; }

	virtual BOOL IsBrowserPalette() = 0;
	virtual BOOL ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour = -1) = 0;

	virtual UINT32 GetHorizontalDPI() = 0;
	virtual UINT32 GetVerticalDPI() = 0;

	virtual LPBITMAPINFO		GetBitmapInfo()			= 0;
	virtual LPRGBQUAD			GetPaletteForBitmap()	= 0;
	virtual LPBITMAPINFOHEADER	GetBitmapInfoHeader()	= 0;
	virtual void				ReplaceBitmap(LPBYTE pBytes, LPBITMAPINFO pInfo) = 0;

	virtual OILBitmap* MakeCopy() = 0;

	void SetUsedByBrush(BOOL Used) { m_bUsedByBrush = Used;}
	BOOL IsUsedByBrush() { return m_bUsedByBrush; }

PORTNOTE("other","Removed XML stuff")
#ifndef EXCLUDE_FROM_XARALX
	virtual void		SetXPEInfo(OILBitmap* pMasterBitmap, IXMLDOMDocumentPtr pNewEditList);
	virtual void		GetXPEInfo(OILBitmap*& refMasterBitmap,
								   IXMLDOMDocumentPtr& refEditList,
								   BOOL bEnsureList = FALSE);
#endif
	virtual BOOL		IsXPEMaster();
	virtual BOOL		DestroyXPEInfo();
	virtual BOOL		RemoveXPEBitmap();
	virtual	BOOL		RebuildXPEBitmap();
	virtual BOOL		NeedsXPERebuild();

protected:
	String_256 m_BitmapName;
	UINT32 m_BitmapAnimDelay;
	BOOL m_bInterpolation;			// 'smooth when scaled up' property
PORTNOTE("gif","Removed GIF stuff")
#ifndef EXCLUDE_FROM_XARALX
	GIFDisposalMethod	m_AnimationRestoreType;
#endif
	UINT32 m_LeftOffset;
	UINT32 m_TopOffset;

	// Link to the original version of the bitmap
	OriginalBitmapRef m_OriginalBitmap;

	// Contoning stuff
	OILBitmap*  m_pColourVersion;
	OILBitmap*  m_pGreyscaleVersion;
	RGBQUAD*	m_pContonePalette;
	BYTE*		m_pGreyscaleTable;

	// XPE stuff
	OILBitmap* m_pMasterBitmap;
PORTNOTE("other","Removed XML stuff")
#ifndef EXCLUDE_FROM_XARALX
	IXMLDOMDocumentPtr m_pEditList;
#endif
	// Flags
	BOOL m_bTemp : 1;				// This is a temp bitmap (ie. It's not in the global bitmap list)
	BOOL m_bIsAFractal : 1;
	BOOL m_bIsGreyscale : 1;
	BOOL m_bUsedByBrush : 1;
	BOOL m_bNeedsXPERebuild : 1;	// Bitmap data has been removed from memory, can be rebuilt from m_pMasterBitmap and m_pEditList
	BOOL m_bHidden : 1;				// Should not show up in bitmap gallery

#ifdef _DEBUG
	// Debug flag, so we can detect multiple references to temp bitmaps
	BOOL m_bIsAttached : 1;
#endif
};


/*********************************************************************************************

>	class KernelBmpListItem : public ListItem

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/09/05
	Purpose:	Holds a pointer to a KernelBitmap for tracking the creation of
				bitmaps for cleanup purposes.  This class should be eliminated 
				by modifying the bitmap classes to correctly reference count and 
				handle the concept of temporary bitmaps.

**********************************************************************************************/ 

class KernelBmpListItem : public ListItem
{
public:
	CC_DECLARE_DYNAMIC( KernelBmpListItem )
	KernelBmpListItem(KernelBitmap* pBmp = NULL)
	{
		m_pBmp = pBmp;
	}
	
	KernelBitmap* m_pBmp;			// pointer to bitmap
};

/*********************************************************************************************

>	class KernelBitmap : public ListItem

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	To encapsulate bitmaps, which are implemented in the OIL layer. Kernel code
				is only allowed to use this interface. OIL code is allowed to use the
				specific OILy functions that can be done via the OILBitmap pointer.

**********************************************************************************************/ 


class KernelBitmap : public ListItem
{
	friend class BitmapList;

	CC_DECLARE_DYNAMIC( KernelBitmap )

public:
	KernelBitmap( OILBitmap *pBitmap = NULL, BOOL Temp = FALSE );
	KernelBitmap( const KernelBitmap& );
	KernelBitmap( UINT32, UINT32, UINT32, UINT32, BOOL Temp = FALSE );
	~KernelBitmap();

	static KernelBitmap* MakeKernelBitmap(OILBitmap *pBitmap = NULL,
										  Document *pDoc = NULL,
										  BOOL bAlwaysNew = FALSE);

	static void SetCreateTracker(List* pList)
	{
		s_pTrackerList = pList;
	}
	static List* s_pTrackerList;

	INT32 operator==(const KernelBitmap&);
	KernelBitmap& operator=(const KernelBitmap&);

	inline OILBitmap* GetActualBitmap() const { return ActualBitmap; }

	BOOL IsOK() const;

	// Function used when exporting an 8bpp JPEG into the web file
	virtual BOOL WritePalette(BaseCamelotFilter *pFilter);
	// Function used when importing that 24bpp JPEG back in and wanting it in 8bpp format
	virtual BOOL Convert24To8(RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries);

	// Function used by the native/web filters to pull a bitmap from inside a bitmap
	// definition record
	static BOOL ImportBitmap(CCLexFile* pFile, BaseBitmapFilter *pBitmapFilter, BaseCamelotFilter *pFilter,
							 BOOL IsCompressed,
							 RGBTRIPLE *pPalette, UINT32 NumberOfPaletteEntries,
							 KernelBitmap** ppImportedBitmap, BOOL* IsNew);
	// Function used by the EPS filters to import bitmaps and by the bitmap filter
	// to import a bitmap directly from a bitmap file
	static BOOL ImportBitmap(Filter *pFilter, const BitmapInfo *pInfo, INT32 BitmapType,
							 KernelBitmap**, BOOL*);

	static KernelBitmap* TryAndUseExistingBitmap(OILBitmap* pOILBitmap, Document* pDoc, 
							 					 BOOL* IsNew);

	inline void PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue);
	inline void PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue);
	inline Pixel32bpp ReadPixel32bpp(INT32 x, INT32 y, BOOL UsePalette = TRUE) const;
	inline INT32 ReturnPaletteIndexUsed( INT32 x, INT32 y ) const;
	inline PixelGreyscale ReadPixelGreyscale(INT32 x, INT32 y) const;
	ADDR GetBitmapBits() const;

	BitmapList* GetParentBitmapList() { return m_pParentList; }
	BOOL IsUsedInDocument(Document* pDoc, BOOL bIncludeHidden = FALSE);

	void Attach(BitmapList* = NULL);
	void Detach();

	UINT32 GetWidth() const;
	UINT32 GetHeight() const;
	UINT32 GetBPP();
	UINT32 GetNumPaletteEntries() const;

	INT32 GetRecommendedWidth();
	INT32 GetRecommendedHeight();
	void SetRecommendedWidth(INT32);
	void SetRecommendedHeight(INT32);

	BOOL SetTransparencyIndex(INT32);
	BOOL GetTransparencyIndex(INT32* pIndex);
	BOOL IsTransparent();

	BOOL IsDefaultBitmap();
	BOOL HasBeenDeleted();
	BOOL IsGreyscale();

	// Functions to access the orginal bitmap file
	void SetOriginalSource(BitmapSource* pFileBuffer = NULL, BaseBitmapFilter* pImportFilter = NULL);
	BOOL GetOriginalSource(BitmapSource** ppFileBuffer = NULL, BaseBitmapFilter** ppImportFilter = NULL);

	// Functions for contoning
	BOOL CreateGreyscaleVersion(BOOL ForceGreyBmp = FALSE);
	BOOL DestroyGreyscaleVersion();
	OILBitmap* GetGreyscaleVersion(BOOL ForceGreyBmp = FALSE);

	OILBitmap* ExtractOILBitmap();
	void GenerateGreyscaleTable();

	BOOL SetAsFractal();
	BOOL SetAsGreyscale();

	UINT32 GetDelay();
	void SetDelay(UINT32 Delay);

	BOOL GetInterpolation();
	void SetInterpolation(BOOL bInterpolation);

PORTNOTE("other","Removed GIF stuff")
#ifndef EXCLUDE_FROM_XARALX
	void SetAnimationRestoreType(GIFDisposalMethod Type);
	GIFDisposalMethod GetAnimationRestoreType() const;
#endif

	void SetLeftOffset(UINT32 LeftOffset);
	UINT32 GetLeftOffset() const;
	void SetTopOffset(UINT32 TopOffset);
	UINT32 GetTopOffset() const;

	BOOL IsBrowserPalette() const;
	BOOL ArePalettesTheSame(const LPLOGPALETTE pLogPalette, const INT32 TransColour = -1);

	UINT32 GetHorizontalDPI() const;
	UINT32 GetVerticalDPI() const;

	void SetName(String_256& NewName);
	String_256& GetName();

	// These should only really be used by Winoil code
	LPBITMAPINFO		GetBitmapInfo();
	LPRGBQUAD			GetPaletteForBitmap();
	LPBITMAPINFOHEADER	GetBitmapInfoHeader();

	void SetUsedByBrush(BOOL Used) { m_bUsedByBrush = Used;}
	BOOL IsUsedByBrush() { return m_bUsedByBrush;}

PORTNOTE("other","Removed XML stuff")
#ifndef EXCLUDE_FROM_XARALX
	virtual void		SetXPEInfo(KernelBitmap* pMasterBitmap, IXMLDOMDocumentPtr pNewEditList);
	virtual void		GetXPEInfo(KernelBitmap*& refMasterBitmap,
								   IXMLDOMDocumentPtr& refEditList,
								   BOOL bEnsureList = FALSE);
#endif
	virtual BOOL		IsXPEMaster();
	virtual BOOL		DestroyXPEInfo();
	virtual	void		ReplaceOILBitmap(LPBYTE pBits, LPBITMAPINFO pInfo);

	// attach a copy of this bitmap to the current doc's bitmap gallery.
	// see method definition for more info.
#ifdef _DEBUG
public:
	BOOL AttachDebugCopyToCurrentDocument(String_256 strName);
#endif

public:
	OILBitmap *ActualBitmap;

protected:
	BitmapList*	m_pParentList;

	BOOL m_bDontDeleteActualBitmap : 1;
	BOOL m_bFractalAttached : 1;
	BOOL m_bUsedByBrush : 1;

};

/*********************************************************************************************

>	class KernelBitmapRef : public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	A reference to a KernelBitmap object.

**********************************************************************************************/ 

class KernelBitmapRef : public CCObject
{
	CC_DECLARE_DYNAMIC( KernelBitmapRef )

public:
	KernelBitmapRef();
	~KernelBitmapRef();

	INT32 operator==(const KernelBitmapRef&);
	KernelBitmapRef& operator=(KernelBitmapRef&);

	void Attach(KernelBitmap*, Document* Doc = NULL);
	void Detach(BOOL bTryRemoveFromDoc = FALSE);
	void DeleteBmp();

	void RemoveFromTree();
	void AddtoTree();
	BOOL IsHidden() { return m_bHidden; }

	KernelBitmap* GetBitmap() { return m_pTheBitmap; }
	void SetBitmap(KernelBitmap*);

protected:
	KernelBitmap* m_pTheBitmap;

	BOOL m_bHidden;		// Is this ref in a hidden sub-tree
};


/*********************************************************************************************

>	class BitmapPtr : public ListItem

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/94
	Purpose:	A simple class containing a pointer to a kernel bitmap, so we can build
				lists with them.

**********************************************************************************************/ 

class BitmapPtr : public ListItem
{
	CC_DECLARE_DYNAMIC( BitmapPtr )

public:
	KernelBitmap* m_TheBitmap;
};

/********************************************************************************************

>	inline void KernelBitmap::PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	13/9/94 (3/7/96)
	Inputs:		x, y - coordinates within the bitmap of the pixel to plot
				NewValue - a 32bpp pixel value.

	Outputs:	(changes attached oil bitmap)
	Returns:	Nothing
	Purpose:	Plots a 32bpp pixel to a deep bitmap (16, 24, or 32bpp)

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.
		
	Errors:		ERROR3 if the bitmap is not 16, 24, or 32bpp

	SeeAlso:	OILBitmap::PlotPixel

********************************************************************************************/

inline void KernelBitmap::PlotPixel(INT32 x, INT32 y, Pixel32bpp NewValue)
{
	ERROR3IF(ActualBitmap == NULL, "PlotPixel with no bitmap!");
	ActualBitmap->PlotPixel(x, y, NewValue);
}



/********************************************************************************************

>	inline void KernelBitmap::PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue)
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Inputs:		x, y - coordinates within the bitmap of the pixel to plot
				NewValue - an 8bpp pixel value, presumed to be a greyscale.

	Outputs:	(changes attached oil bitmap)
	Returns:	Nothing

	Purpose:	Plots an 8bpp (0..255) pixel to any type of bitmap
				The pixel is taken to be a greyscale, and is truncated or expanded
				as appropriate to fit the greyscale value into the destination bitmap
				pixel format.

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

	SeeAlso:	OILBitmap::PlotPixel

********************************************************************************************/

inline void KernelBitmap::PlotPixel(INT32 x, INT32 y, PixelGreyscale NewValue)
{
	ERROR3IF(ActualBitmap == NULL, "PlotPixel with no bitmap!");
	ActualBitmap->PlotPixel(x, y, NewValue);
}



/********************************************************************************************

>	inline Pixel32bpp KernelBitmap::ReadPixel32bpp(INT32 x, INT32 y, BOOL UsePalette = TRUE) const
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com> (Jason)
	Created:	13/9/94 (3/7/96)
	Inputs:		x, y - coordinates of the pixel to read
				UsePalette -Ignored for deep bitmaps
							For <=8bpp bitmaps, if this value is:
								FALSE, the bitmap's palette will be ignored, and the actual pixel
								(palette index) value will be returned as a greyscale RGB value
								TRUE, the pixel index value will be used to retrieve the 24bpp
								palette entry for the pixel, if a palette is available.

	Outputs:	(changes attached oil bitmap)
	Returns:	The pixel value of the given pixel

	Purpose:	reads a pixel

	Notes:		In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

	Errors:		-
	SeeAlso:	OILBitmap::ReadPixel32bpp

********************************************************************************************/

inline Pixel32bpp KernelBitmap::ReadPixel32bpp(INT32 x, INT32 y, BOOL UsePalette) const
{
	ERROR3IF(ActualBitmap == NULL, "PlotPixel with no bitmap!");
	return(ActualBitmap->ReadPixel32bpp(x, y));
}


// ap 29/9/99
inline INT32 KernelBitmap::ReturnPaletteIndexUsed( INT32 x, INT32 y ) const
{
	ERROR3IF(ActualBitmap == NULL, "Can't do anything without a bitmap");
	return(ActualBitmap->ReturnPaletteIndexUsed(x, y));
}



/********************************************************************************************

>	inline PixelGreyscale KernelBitmap::ReadPixelGreyscale(INT32 x, INT32 y) const
					
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Inputs:		x, y - coordinates of the pixel to read
	Outputs:	(changes attached oil bitmap)
	Returns:	The pixel value of the given pixel

	Purpose:	Reads a pixel as a greyscale intensity value (0..255)

	Notes:		When reading a pixel from <8bpp bitmaps, the pixel value will be assumed
				to be a greyscale, and will be expanded up to 8bpp.
				When reading a pixel from a deep bitmap, the RGB value will be converted
				into a greyscale intensity value.

				In debug builds, will check all kinds of stuff, but in release
				builds, it assumes that you are providing legal offsets in the bitmap,
				and are only using this call where it will work - this is to reduce
				retail overhead on each pixel operation.

	Errors:		-
	SeeAlso:	OILBitmap::ReadPixelGreyscale

********************************************************************************************/

inline PixelGreyscale KernelBitmap::ReadPixelGreyscale(INT32 x, INT32 y) const
{
	ERROR3IF(ActualBitmap == NULL, "PlotPixel with no bitmap!");
	return(ActualBitmap->ReadPixelGreyscale(x, y));
}


#endif
