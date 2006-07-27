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
//

#ifndef INC_BITFILT
#define INC_BITFILT

//#include "fixst256.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpprefs.h"  // SelectionType
//#include "dibconv.h"   // for XARADITHER enum - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "quality.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpsrc.h"		// for OFFSET
//#include "pathname.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "filtimop.h"	//For ImagemapFilterOptions

class GRenderRegion;
class GRenderBitmap;
class KernelBitmap;
class NodeBitmap;
class OILBitmap;
class BitmapExportParam;
class BitmapExportOptions;
class BaseCamelotFilter;
class Progress;
//class CCFile;
//class CCDiskFile;
class CCLexFile;
class ImagemapFilter;

/********************************************************************************************

  >	class BitmapImportOptions : public CC_CLASS_MEMDUMP

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Base class for storing import options
				It's not actually used much though...

********************************************************************************************/
class BitmapImportOptions : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(BitmapImportOptions);
public:
	BitmapImportOptions() : m_bValid(FALSE), m_bCancelled(TRUE),
							m_bProvideProgress(FALSE) {};

	BOOL	m_bValid;
	BOOL	m_bCancelled;
	BOOL	m_bProvideProgress;
};



/********************************************************************************************

>	class BaseBitmapFilter : public BitmapFilter

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:   	Base class for bitmap filters. (Currently export only). Inherited classes
				tend to be in the OIL layer. The base class fails on both imports & exports,
				so you only need to override the half that you implement.

********************************************************************************************/

class BaseBitmapFilter : public BitmapFilter
{
	CC_DECLARE_DYNAMIC(BaseBitmapFilter);
	
public:
	BaseBitmapFilter();
	~BaseBitmapFilter();

	// inherited classes MUST override this one:
	virtual BOOL Init() { return TRUE; }

	static BOOL InitBaseClass();

	// Works out if opening a file of this type requires a default document to be loaded
	// or does the file format supply the document.
	virtual BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);

	virtual FilterType GetFilterType() const { return INVALID; }

	BOOL DoImport(SelOperation *Op, CCLexFile*, Document *DestDoc, 
				  BOOL AutoChosen = FALSE, ImportPosition *Pos = NULL,
				  KernelBitmap** ppImportedBitmap = NULL,
				  DocCoord* pPosTranslate = NULL, String_256* = NULL);

	virtual BOOL DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL);
	virtual BOOL DoExportBitmap(Operation*, CCLexFile*, PathName*, KernelBitmap*);
	virtual BOOL DoExportBitmaps(Operation*, CCLexFile*, PathName*, BitmapExportParam*);
	virtual BOOL DoExportWithOptions(Operation*, CCLexFile*, PathName*, Document*, BitmapExportOptions*, DocRect* = NULL);
	virtual BOOL DoExportBitmapWithOptions(Operation*, CCLexFile*, PathName*, KernelBitmap*, BitmapExportOptions *);
	virtual BOOL DoExportHelper(Operation* pOp,
							    CCLexFile* pFile,
							    PathName* pPath,
							    Document* pDoc);

	CCLexFile* GetImportFile() { return InputFile; }

	virtual BOOL ImportBitmap(CCLexFile*, KernelBitmap**);
	BOOL ImportBitmap(CCLexFile*, KernelBitmap**, const UINT32 nBitmapToRead);

	// Import inherited classes MUST override these ones
    virtual BOOL ReadFromFile(OILBitmap* pOilBitmap);

	// This is the function for native/web files to use
    virtual BOOL ReadFromFile(OILBitmap* pOilBitmap, BaseCamelotFilter *pFilter,
							  CCLexFile * pFile, BOOL IsCompressed);

	// function to set the IsPreviewBitmap flag.
	void SetPreviewBitmap(BOOL);

	// Function which determines if a filter should export invisible layers or not
	virtual BOOL ExportVisibleLayersOnly(); 

	// Allow somebody to read the ID
	UINT32 GetImportMsgID() { return ImportMsgID; }

	// Allow somebody to read the export ID
	virtual UINT32 GetExportMsgID();
	virtual BOOL ShouldReuseExistingBitmaps() { return TRUE; }

	// derived classes should NEVER use this : shouldn't be here really...
	BOOL SetExportOptions ( BitmapExportOptions* pOptions );

	// retrieves export options by using the options dialog
	BOOL SetUpExportOptions(BitmapExportOptions **ppExportOptions, BOOL OnlyDefaults = FALSE);
	BOOL SetUpExportBitmapOptions(BitmapExportOptions **ppExportOptions, KernelBitmap *pBitmap, 
																		BOOL OnlyDefaults = FALSE);

	// Function which returns whether a second rendering pass is required or not
	virtual BOOL IsSecondPassRequired();

	// Function to generate an optimised palette
	virtual BOOL GenerateOptimisedPalette(Spread *pSpread, BitmapExportOptions *pOptions, BOOL SnapToBrowserPalette);
	virtual BOOL GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI, BOOL SnapToBrowserPalette, UINT32 NumColsInPalette, BOOL UsePrimaryCols);

	static BOOL GeneratingOptimisedPalette();

	// These shouldn't be public...(the second definitely)
	virtual BOOL			IsFormatLossy() const;
	virtual BitmapSource*	CreateBitmapSource(OFFSET Size) const;


	//Graham's HTML tag to clipboard code. GTODO: Take out
	virtual void ExportHTMLTag(PathName* ppthToUse);

	// DMC moved because we need to get at the dpi outside of the filter
	// Used to be protected
	virtual BitmapExportOptions* CreateExportOptions() const;
	BitmapExportOptions* GetBitmapExportOptions() const;
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);
	// SMFIX there used to be some junk in the call to GetExportOptions that assumed the
	// filter type being used which could be changed by the GetExportOptions call itself
	// therefore all this sort of stuff should be called on the correct known filter using this
	// call afterwards
	virtual void PostGetExportOptions(BitmapExportOptions* pOptions);

	virtual BOOL SetUpClippingRectangleForExport(Spread *pSpread, SelectionType Selection);

	// Function for web/native filters to use to write out the bitmap data to file
	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter* pFilter,
								   CCLexFile* pFile, INT32 Compression);

	// Function to write out a list of bitmaps to file
	virtual BOOL SaveExportBitmapsToFile(CCLexFile* pFile, PathName* pPath, BitmapExportParam* pParam,
										 BOOL DontShowFileName = FALSE);
	
	// Check if this Bitmap filter can cope with saving at this Bpp/Colour depth
	virtual BOOL IsThisBppOk(UINT32 Bpp);

	// Export a spread to an export DC. Now virtual and broken into two parts.
	// We overide the actual export of the nodes so that we can render in strips.
	virtual	BOOL ExportRenderNodes(RenderRegion *pRegion, ExportDC *pDC,
								   BOOL VisibleLayersOnly = FALSE,
								   BOOL CheckSelected = FALSE, BOOL ShowProgress = TRUE);

	// Find out which way we need to render
	virtual BOOL GetRenderBottomToTop();

	// public access to the cliprect currently in use
	DocRect GetClipRect() const { return ClipRect; }

	// public access to the currently exported bitmap
	static KernelBitmap *GetExportBitmap() {return pExportBitmap; };

	// Functions to work out the size of the export
	static	DocRect	GetSizeOfDrawing(Spread *pSpread=NULL);
	static	DocRect	GetSizeOfSpread(Spread *pSpread); 
	static	BOOL	GetSizeOfBitmap(INT32 *PixelWidth, INT32 *PixelHeight); 
	static  void    CallPixelAlignedFiddle( DocRect * r );

	// Value which we allow PelsPerMeter to be minimum  
	static const INT32 MinPelsPerMeter;
	// Minimum selection width/height allowed
	static const INT32 MinExportSize;

	// Added by Craig Hamilton 29/8/00.
	// This variable will hold the number of palette entries generated in DoExportDoc.
	static INT32 m_NumberOfPaletteEntries;
	// End added.

	//Functions and variables to allow imagemap exporting
	//This function does the export
	BOOL ExportImagemap(Operation*, PathName*, Document*);

	// Base do-nothing DPI functions
	virtual BOOL CanDoImportDPI()				{ return FALSE; }
	virtual void SetImportDPI(DPI ImportDPI)	{}

protected:
	// Something to mark if this is a meant as a Preview Bitmap or not
	BOOL IsPreviewBitmap;

	// Export the bitmap. This function is called from DoExport ().
	BOOL ExportBitmap ( Operation	*pOp,
						CCLexFile	*pFile,
						PathName	*pPath,
						Document	*pDoc );


	BitmapImportOptions*	m_pImportOptions;
	//
	// Import related functions
	//
	virtual BOOL PrepareToImport();
	virtual void CleanUpAfterImport();

	// To do with bitmap source preservation
	BOOL					PrepareBitmapSource();
	// Overridable...
	virtual OFFSET			GetDataStartOffset() const;

	// Optional user choices e.g. PhotoCD choose image 
	virtual BitmapImportOptions*	GetImportOptions();
	virtual BOOL ProcessImportOptions();

	virtual BOOL CreateBitmapName(const CCLexFile* pDiskFile, const KernelBitmap* pBitmap,
										 const UINT32 nBitmapNumber);

	// Multiple Bitmap Functions
	UINT32	GetBitmapNumber() const;
	BOOL	SetLastBitmap();
	BOOL	SetBitmapNumber(const UINT32 nBitmapToRead);
	BOOL	AreBitmapsToRead() const;

	// Progress Bar Functions
	virtual BOOL IncProgressBarCount(UINT32 n);
	void	SetTotalProgressBarCount(UINT32 n);

	// Functions to setup the View ready for exporting, and to restore it's state afterwards
	void    ForceBackgroundRedrawOff(DocView*);
	void    RestoreBackgroundRedrawState(DocView*);

	void    ForceAntialiasingFull(DocView*);
	void    ForceAntialiasingOff(DocView*);
	void    RestoreViewQuality(DocView*);

	//
	// Export related functions
	//
	virtual BOOL PrepareToExport(Spread *pSpread, UINT32 Depth, double DPI, SelectionType, 
										UINT32 Dither = XARADITHER_ORDERED_GREY);
	virtual void CleanUpAfterExport();

	virtual BOOL DoExportDoc(Operation*, CCLexFile*, PathName*, Document*,
							 UINT32, double, const SelectionType&, BOOL UseExistingPalette = FALSE);

	virtual UINT32 GetNumReservedColours();

	// inherited classes MUST override these ones
	virtual BOOL WriteToFile( BOOL End );
	virtual BOOL EndWriteToFile( );

	virtual BOOL WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi);
	
	// virtual functions for export of multi-image files
	// If a derived filter handles multi-image exports then ALL FIVE of the
	// following functions MUST be overridden
	virtual BOOL WriteFileHeader(void);
	virtual BOOL WritePreFrame(void);
	virtual BOOL WritePostOptimisedPalette(void);
	virtual BOOL WritePreSecondPass(void);
	virtual BOOL WriteFrame(void);
	virtual BOOL WritePostFrame(void);
	virtual BOOL WriteFileEnd(void);

	virtual void AlterPaletteContents( LPLOGPALETTE pPalette );
	
	// Support functions giving the export requirements
	LPBITMAPINFO GetBitmapInfo() const;
	BOOL GetCurrentStripInfo(	ADDR* ppBits, 
								BMP_SIZE* pWidth, BMP_SIZE* pHeight,
								BMP_DEPTH* pDepth) const;

	CCLexFile*	GetExportFile() const;

	// WEBSTER - Gerry 25/07/97
	// Functions for handling of bitmap export hint
	// Virtual function to set the document's export hint
	// Override it if youf filter wants to actually do anything
	virtual BOOL SetExportHint(Document* pDoc) { return(TRUE); }
	
	// WEBSTER - markn 8/2/97
	// Centralised function for creating an 8bpp palette
	static LPLOGPALETTE Create8bppPalette();

	BOOL InsertBitmapOnNewLayer(SelOperation *pOp, Spread * pSpread, NodeBitmap *pNodeBitmap,
								Layer ** ppNewLayer);

	// Give the filter a chance to add in an offset that was contained in the bitmap definition
	virtual BOOL AddOffsetFromBitmap(DocCoord * pOffset);
	// Give the filter a chance to change the layer's flags given the newly loaded bitmap
	// Only applies if loading multiple bitmaps onto new layers.
	virtual BOOL SetFlagsFromBitmap(Layer * pLayer, KernelBitmap * pBitmap, UINT32 nBitmapToRead);
	// Give the filter a chance to change the animation properties given the newly loaded bitmaps
	// Only applies if loading multiple bitmaps onto new layers.
	virtual BOOL SetAnimationPropertiesFromLoaded(Spread * pSpread);

	// this is used for the actual writing of the file
	static BOOL WrittenHeader;		// have we written the file header out or this first strip
	BOOL RenderInStrips;		// control over rendering in strips
	BOOL RenderBottomToTop;	// control over which way we render
	BOOL BadExportRender;	// whether the export has gone ok or not
	static UINT32 OurNumNodes;		// our copy of the number of nodes being exported

	// This is used so that we know what to do when updating the progress bar.
	static UINT32 SizeOfExport;
	UINT32 GetSizeOfExport() { return SizeOfExport; }
	void SetSizeOfExport(UINT32 NewVal) { SizeOfExport = NewVal; }

	BMP_SIZE	GetPixelWidth() const;
	BOOL		SetPixelWidth(const BMP_SIZE& Width);
	BMP_SIZE	GetPixelHeight() const;
	BOOL		SetPixelHeight(const BMP_SIZE& Height);

	// Resource IDs of the strings that describe this filter.
	UINT32 FilterNameID;
	UINT32 FilterInfoID;

	// The string to display when actually exporting rather than rendering.
	// 0 if we're not an export filter.
	// ExportMsgID is used in ExportRender when we are rendering the bitmap
	UINT32 ExportingMsgID;

	// pointer to input file
	CCLexFile* InputFile;

	// pointer to output file
	static CCLexFile* OutputFile;

	// pointer to bitmap being imported
	KernelBitmap *pNewBitmap;

	// pointer to RenderRegion
	static GRenderBitmap *ExportRegion;

	// the views current background redraw state
	BOOL BackgroundRedrawState;
	BOOL BackgroundRedrawStateSet;

	// the views current quality setting
	Quality ViewQuality;
	BOOL ViewQualitySet;

	// Store for the bitmap pointr in DoExportBitmap
	static KernelBitmap *pExportBitmap;
	
	// Store for the document rectangle that we are exporting
	DocRect ClipRect;

	// Store for the spread that we are exporting 
	Spread *pSpread;

	UINT32 PaletteType;

	UINT32 PreviewDither;
	UINT32 PreviewPalette;

	// This is a bit OILy, so will need to be moved some time
	static LPLOGPALETTE pOptimisedPalette;
	static BOOL WeAreGeneratingOptPalette;

	LPLOGPALETTE m_pOptimisedPalette2;
	static LPLOGPALETTE m_pBrowserPalette;

	void		SetDepthToRender(const BMP_DEPTH& Depth)	{	m_RenderDepth = Depth;	}
	BMP_DEPTH	GetRenderDepth() const						{	return m_RenderDepth;	}
	BOOL		WarnIfIncompatible();
	
private:
	// Progress bar variables...should be in Filter class really
	UINT32		m_ProgressBarCount;			// Current progress bar count
	UINT32		m_TotalProgressBarCount;		// Total progress bar count
	Progress*	m_pProgress;					// ptr to the progress bar object
	String_64	m_ProgressBarMessage;			// Progress bar message
	INT32			m_nBitmapToRead;

	BitmapSource*	m_pBitmapSource;		// The bitmap source file
	ImagemapFilter* GetImagemapFilter();

	// 8 out of 10 users who expressed a preference said their cats prefer these...
	static DPI		m_DefaultExportDPI;
	BitmapExportOptions*	m_pExportOptions;

	BMP_SIZE	m_PixelWidth;
	BMP_SIZE	m_PixelHeight;
	BMP_DEPTH	m_RenderDepth;
};

#endif
