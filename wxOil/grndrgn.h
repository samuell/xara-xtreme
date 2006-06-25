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

// GRNDRGN.H
//
// Created: 29/6/93 by Will
//

#ifndef INC_GRNDRGN
#define INC_GRNDRGN

//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rendtype.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "winrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "gdrawcon.h"
//#include "monotime.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "GDrawIntf.h"
//#include "rrcaps.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class GradFillAttribute;
class BitmapFillAttribute;
class BitmapTranspFillAttribute;
class KernelBitmap;
class TranspGradTable;
class OffscreenAttrValue;
class ClipRegionAttribute;
class CaptureManager;
class Capture;

enum BitmapConvertHint;

typedef struct {
	WORD palVersion;
	WORD palNumEntries;
	PALETTEENTRY palPalEntry[256];
} RealLogPalette;

typedef CMapPtrToPtr ClipRegionMap;
typedef CMapPtrToPtr ClipRectMap;

#define USE_wxBITMAP 0

/********************************************************************************************

>	class GRenderRegion : public RenderRegion

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Purpose:	Encapsulates rendering using Gavins routines (ie. into a bitmap).
				This object is created when something needs rendering using Gavins routines
				eg. When anti-aliasing.  
				See RenderRegion notes for more general details of this class. This is an
				abstract class - see GRenderDIB or GRenderDDB for implementations.

********************************************************************************************/

class GRenderRegion : public RenderRegion
{
	CC_DECLARE_DYNAMIC( GRenderRegion )

	friend class GBrush;							// so it can get to our vars etc
	friend class OpGDraw;
//	friend class OpGFPU;
	friend class GRenderRegionWrapper;
	friend class ConcurrentRenderer;
	friend class GDrawBorrower;

public:

	GRenderRegion();								// do NOT use
	GRenderRegion(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale, UINT32 Depth, double dpi);
	virtual ~GRenderRegion();

	static BOOL Init();

	// these are the virtual fns we need to instantiate a class
	virtual BOOL CopyRenderInfo( const RenderRegion &Other);
	virtual BOOL AttachDevice(View*, wxDC *, Spread* = NULL, bool fOwned = false );
	virtual BOOL InitDevice();
		
	BOOL StartRender();
	BOOL StopRender();
	virtual void SetClean(BOOL bResetChangedBounds, BOOL FillWhite);
	virtual BOOL IsClean() {return !HaveRenderedSomething;}

	void DrawPathToOutputDevice(Path* PathToDraw, PathShape shapePath=PATHSHAPE_PATH);
	void DrawRect(DocRect *RectToRender);
	void DrawDragRect(DocRect *RectToRender);
	void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	void DrawBlob(DocCoord p, BlobType type);
	void DrawPixel(const DocCoord &Point);
	void DrawCross(const DocCoord &Point, const UINT32 Size);

	void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap);
	void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL);
	void DrawBitmap(const DocRect& rect, KernelBitmap* pBitmap);
	
	void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape) {}
	void DrawBitmapBlob(const DocCoord &Point, ResourceID resID ) {}

	void DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat = DEFAULT_TEXT_FORMATTING);
	void SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background);
	void GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi = NULL);

	void InitClipping();
	void InitAttributes();
	void SetLineAttributes();
	void SetFillAttributes();
	void SetOSDrawingMode();
	void SetQualityLevel();

	void Restore(RenderRegion*);

	BOOL RenderGradFillPath(Path *, GradFillAttribute* );

	BOOL RenderBitmapFill  (Path *, BitmapFillAttribute* );

	virtual void SetSolidColours(BOOL SetSolid);

	//static public functions
	static RenderRegion* Create(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale,
								RenderType, View* pView = NULL, BOOL bForce32BPP = FALSE);
	static BOOL Init(BOOL);
	static void DeInit();
	static BOOL CalcBlitMode( INT32 Wanted );

	GDrawContext *GetDrawContext() const
	{
		return GD;
	};

	static GDrawContext* SetTempDrawContext(GDrawContext* pContext)
	{
		// Bad things will happen if we set this to NULL
		if (pContext == NULL)
		{
			TRACEUSER( "Gerry", _T("******** Trying to set a NULL GDrawContext\n") );
			pContext = pRealGD;
		}
		
		GDrawContext* pOldCon = GD;
		GD = pContext;
//		TRACEUSER( "Gerry", _T("GDraw context set to 0x%08x (prev = 0x%08x)\n"), GD, pOldCon);
		return(pOldCon);
	};

	static GDrawContext *GetStaticDrawContext()
	{
#ifdef RALPH
		if (GDrawAsm::GetContextForCurrentThread() != NULL)
			return(GDrawAsm::GetContextForCurrentThread());
#endif

		return GD;
	};

	static GDrawContext *GetStaticDrawContext(INT32 DitherType)
	{
#ifdef RALPH
		if (GDrawAsm::GetContextForCurrentThread() != NULL)
			return(GDrawAsm::GetContextForCurrentThread());
#endif

		if ((DitherType == 0) || (DitherType == 3))
			return(ErrorDiffContext);
		else
			return(GD);
	}

	static INT32 GetDefaultTransparency() { return WhichTransparency; }

	static UINT32 SetPaletteEntries( LPBITMAPINFO lpBmi, wxDC* pOutputDC = NULL );

	// Used to calculate the size of the render region required
	virtual WinRect CalculateWinRect( Matrix& RenderMatrix, const DocRect& docrect,
									  const double dpi);
	// let's non-renderregion code determine pixel dimensions of bounding boxes
	virtual WinRect CalculateWinRect( const DocRect& docrect);

	void SetDeepDIB(BOOL Value);

	// SMFIX
	// call to get the 32bit bitmap just to read it with no conversion going on
	// used when gathering stats for the palette
	const RGBQUAD* Get32BitRGBQuadData(){ return (RGBQUAD*)pBits;}
	UINT32 GetSizeOfRGBQuadData() { return pBitmapInfo ? pBitmapInfo->bmiHeader.biWidth * pBitmapInfo->bmiHeader.biHeight : 0; }

	void SetDoCompression(BOOL bDoCompression) { m_DoCompression = bDoCompression; }

	void SetDoBitmapConversion(BOOL bEnable) { m_bEnableConversion = bEnable; }

	DocRect GetChangedRect();

	void SetBitmapPointers(LPBITMAPINFO bi, LPBYTE by);
	BOOL GetBitmapPointers(LPBITMAPINFO* ppInfo, LPBYTE* ppBits, BOOL bCorrectTransparency = TRUE);

protected:

	static GDrawContext *pRealGD;						// ptr to GDraw we want to use
	static GDrawContext *GD;							// ptr to GDraw we want to use
	static GDrawContext *ErrorDiffContext;				// ptr to special context for error diffusion

	GMATRIX CurrentGMatrix;

	GMATRIX SetGavinMatrix(GMATRIX* GMatrix);
	virtual GMATRIX MakeGavinMatrix(Matrix NewRenderMatrix, DocRect ClipRect, double PixelsPerInch, BOOL bMasterCapture);
	virtual double GetPixelsPerInch() {return PixelsPerInch;}

	MILLIPOINT CalcPixelWidth();
	double CalcPixelWidthDouble();
	MILLIPOINT CalcScaledPixelWidth();
	double CalcScaledPixelWidthDouble();

	RRCaps Caps;

public:
	double GetScaledPixelWidthDouble();
	void SetSimulatePrinting(UINT32 PrintType);
	virtual void GetRenderRegionCaps(RRCaps* pCaps);

	// Function to draw a bitmap into the render region using a mask, and
	// and equivalent function to do the same, only colour separating it as it goes
	// The Separated version is called by this one if colour seps are enabled
	virtual SlowJobResult DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  		   MaskedRenderRegion* pMask, ProgressDisplay *Progress);
protected:
	BOOL GetTransparencyFill(TranspGradTable*, DWORD*);

	virtual BOOL DrawArrowHead(ArrowRec &ArrowToDraw, DocCoord &Centre, DocCoord &Direction);

	BOOL SetBitmapTransparencyFill(TranspFillAttribute*, DWORD*, TranspGradTable*);

//	LPBYTE m_pTempTransparencyBMPBits;	// member variable that may get set up in SetBitmapTransparencyFill and needs freeing later
										// GAT: Never used.

	BOOL SetFractalFill(ColourFillAttribute*, DWORD*);
	BOOL SetFractalTransparencyFill(TranspFillAttribute*, DWORD*);

	// these are pure virtual fns that must be instantiated in all derived classes,
	// together with the multiple paramter constructor and destructor.
	virtual LPBITMAPINFO GetLPBits( INT32 Width, INT32 Height, INT32 Depth, LPBYTE*) = 0;
	virtual void FreeLPBits( LPBITMAPINFO, LPBYTE ) = 0;
	virtual BOOL DisplayBits(LPBITMAPINFO lpDisplayBitmapInfo = NULL, LPBYTE lpDisplayBits = NULL) = 0;
	
	// this isn't pure, but is needed by some inherited classes
	virtual void InitBmpBits();					// added by Ilan.
	virtual BOOL StartRenderAfter(GMATRIX*);
	virtual BOOL StartRenderMiddle();
	virtual void DisplayCurrentState();

	// these are the member variables
	LPBITMAPINFO pBitmapInfo;							// ptr to BITMAPINFO
	LPBYTE pBits;										// ptr to actual bytes (or NULL)
//	INT32 uBitmapSize;									// size of above
	UINT32 uBitmapDepth;									// in bits/pixel
	UINT32 uLineSize;

#if USE_wxBITMAP
	wxBitmap* pBitmap;
	wxAlphaPixelData* pBitmapData;						// Used to obtain and maintain lpBits.

	LPBITMAPINFO AllocWxBitmap( UINT32 Width, UINT32 Height, UINT32 Depth );
	void FreeWxBitmap();
#endif

	wxRect WRect;										// its position in the owner window
	wxRect ScreenRect;									// its position in the whole screen bitmap

	double PixelsPerInch;								// resolution of bitmap
	double dScaledPixelWidth;							// accurate size of pixel with view zoom factored in
 
	wxPalette* hPalette;								// palette used to render (or NULL)
	wxPalette* hPrevPalette;							// original (previous) palette

	BOOL LocalBitmap;									// TRUE if this bitmap is a little one,
														// FALSE if its a chunk of ScreenBitmap
	BOOL UseSolidColours;

	BOOL m_bForceBitmapSmoothing;						// TRUE to enforce smoothing of all rendered bitmaps
														// ; req'd by shadowing framework.
	BOOL m_bEnableConversion;
	BOOL ForceInitBmpBits;								// Set when in-place conversion of RGBA bitmap invalidates it

	INT32 DitherStyle8Bit;								// DitherStyle for 8 bit rendering

	BOOL HaveRenderedSomething;							// TRUE if drawing has occurred

	UINT32 m_ForcePrintingCaps;

	void Display1to1Bitmap();
	void DisplayLtoHBitmap();
	void DisplayHto8Bitmap();

	void PlotBitmap( UINT32 ColourFlag );
	void PlotBitmap( wxDC *pDC, UINT32 ColourFlag, INT32 Left, INT32 Top, UINT32 Width, UINT32 Height, wxPalette* hPal, INT32, INT32 );
public:
	static LPBYTE StaticPlotBitmap( wxDC *pDC, UINT32 ColourFlag, LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, INT32 Left, INT32 Top, UINT32 Width, UINT32 Height, wxPalette* hPal, INT32, INT32 );
protected:
//	KernelBitmap* FractalBitmap;		// GAT: Never used.

	// Limit of transparency in hit-detect pixels before pixel will be ignored
	static UINT32 ClickTranspLimit;

	// Whether to use high-quality anti-aliasing or not
	static BOOL HighQualityAA;

	// Bitmap conversion algorithm for show printer colours
	static INT32 BitmapConversion;

public:
	// Allows you to override the normal checks which are made when deciding
	// whether to smooth a bitmap or not; if we're rendering at full quality
	// and the bitmap will let us, then we _will_ render it with smoothing.
	void SetForceBitmapSmoothing(BOOL ForceSmoothing) { m_bForceBitmapSmoothing = ForceSmoothing; }
	BOOL IsForcingBitmapSmoothing() { return m_bForceBitmapSmoothing; }

private:
	BOOL SetGCol( DocColour& ) const;
	BOOL SetFillGCol( DocColour& ) const;

	DocCoord DocCoordToG(const DocCoord& DocPoint);
	void DocCoordToG( DocCoord *DocPoint );
	BOOL StrokePath( const DocCoord *, const PathVerb*, UINT32,
					 const DocCoord *, const PathVerb*, UINT32, BOOL DrawArrows = FALSE);


protected:
	// don't make these into bitfields, as they get setup by the preferences code which
	// won't be able to cope
	static BOOL WantDoGDraw;										// TRUE if user wants it
	static UINT32 WantGDrawDepth;										// desired depth of off-screen bitmap
//	static BOOL WantNoPalette;										// TRUE if palette switching not wanted
//	static UINT32 WantBlitMode;										// 0=auto, 1=Stretch, 2=SetDIB, 3=BitBlt
	static BOOL WantHighColourDirect;								// TRUE if deep sprite plots directly
	static BOOL WantDeepGDraw; 										// TRUE for 32-bit bitmaps
//	static BOOL WantWinG;											// TRUE to use WinG if poss
	static INT32  WhichPalette;
	static INT32  WhichDither;
	static INT32  WhichTransparency;
	static INT32  ViewDither;											// GDraw dither style used for Views

	static BOOL CanDoGDraw;											// TRUE if it is physically permitted
																	// GAT: Set if GDraw initialises OK.
	static BOOL CanDoPalIndices;									// TRUE if GDI does DIB_PAL_INDICES
	static BOOL CanSetPalette;										// TRUE if screen has palette support
//	static BOOL CanDoDeepDIBs;

	static INT32 ScreenDepth;											// in bits/pixel
	static INT32 ScreenWidth;											// in pixels
	static INT32 ScreenHeight;										// in pixels
	static INT32 LogPixelsX;
	static INT32 LogPixelsY;

//GAT - Not used
//	static LPBITMAPINFO ScreenBitmap;								// NULL if none
//	static LPBYTE ScreenBits;										// bitmap memory for above

	static LPRGBQUAD Fixed256Palette;								// NULL if not fixed palette (256 cols only)
//	static BOOL RealFPU;
	static BitmapConvertHint ScreenHinting;							// used for hinting bitmap conversion

	static INT32 LastPaletteFlag;									// last value passed to GDraw_SelectPalette

	static RealLogPalette ErrorDiffPalette;							// static LOGPALETTE structure for error diffusion

protected:
	static void UpdateErrorDiffPalette(void);

public:
//	static INT32 MaxGMemory;											// largest allocation allowed (0=no limit)
//	static UINT32 MaxGMemoryInK;										// the above in Kbytes
//	static INT32 FreeGMemory;										// how much left to allocate


public:
	static const LOGPALETTE* GetRecommendedPalette();
//	static void PlotBitmap( wxDC *pDC, UINT32 ColourFlag, LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, INT32 Left, INT32 Top, INT32 Width, INT32 Height, wxPalette* hPal, INT32, INT32 );
	static BOOL StrokePathAvailable();
	static INT32 StrokePathToPath(
	 	CONST DocCoord *IPoints,
		CONST BYTE FAR *ITypes,
		size_t ILength,
		DocCoord *OPoints,
		BYTE  *OTypes,
		DWORD OLength,
		BOOL Close,
		DWORD LineWidth,
		DWORD Flattening,
		LineCapType LineCaps,
		JointType LineJoin,
		CONST DashType *Dash);
	static void GColInit( wxDC *, BOOL = FALSE );
	static void PaletteHasChanged();
	static void EnsurePalette(INT32 PaletteFlag);
	static LPLOGPALETTE GetErrorDiffPalette(void)
	{
		return((LPLOGPALETTE)&ErrorDiffPalette);
	};
	
	static void SetViewDither(INT32 dither){ViewDither = dither;};

	static DWORD GetMaxBitmapWidth(void);
	static DWORD GetMaxBitmapDepth(void);
	static INT32 GetDefaultDPI()
	{
		return LogPixelsX;
	};

public:
	void SetDitherStyle8Bit(INT32 DitherStyle)
	{
		DitherStyle8Bit = DitherStyle;
	};

	// banding and mergeing with offscreen rendering system
	virtual void ResetRegion(DocRect &NewClipRect);

public:
	void FreeOffscreenState();

public:
	virtual void SetOffscreen(OffscreenAttrValue*);
	virtual void RestoreOffscreen(OffscreenAttrValue*);
	BOOL RenderBitmapWithTransparency(
					Path*,
					BitmapFillAttribute*,
					BitmapTranspFillAttribute*
				);
	virtual BOOL IsWrappedRender() const {return IsWrapped;}
	virtual BOOL TranspTypeIsRGBTCompatible(UINT32 ttype) const;	// Should be TranspType ttype
	TransparencyEnum MapTranspTypeToGDraw(UINT32 ttype, BOOL bGraduated) const;


	/////////////////////////////////////////////////////////////////////
	//
	// New Capture system
	//
public:
	virtual Capture* StartCapture(CCObject* pOwner,
							  DocRect CaptureRect,
							  CAPTUREINFO cinfo,
							  BOOL bTransparent = TRUE,
							  BOOL bCaptureBackground = FALSE,
							  double mpPixelWidth = 0,
							  NodeRenderableInk* pDirectSupplier = NULL
							  );
	virtual BOOL StopCapture(CCObject* pOwner,
							 BOOL bRender = TRUE,
							 BOOL bReleaseBitmap = FALSE,
							 LPBITMAPINFO* plpBitmapInfo = NULL,
							 LPBYTE* plpBits = NULL,
							 DocRect* pCaptureRect = NULL,
							 Matrix* pmatTransform = NULL,
							 double* pdResolution = NULL
							 );
	virtual BOOL ChangeCapture(CAPTUREINFO cinfo,
							  BOOL bTransparent = TRUE,
							  BOOL bCaptureBackground = FALSE
							 );
//	virtual INT32 GetCaptureDepth()				{return m_CaptureStack.Size();}
	virtual BOOL MasterCaptureIsCurrent() const	{return (!m_CaptureStack.Empty() && GetTopCaptureBitmap() && GetTopCaptureBitmap()->IsMaster());}
//	virtual Capture* GetTopCapture() const		{return m_CaptureStack.Empty() ? NULL : m_CaptureStack.Top();}
//	virtual Capture* GetMasterCapture()	const	{return m_CaptureStack.Empty() ? NULL : m_CaptureStack.Bottom();}
	virtual BOOL RenderCurrentCaptureState(BOOL bStartFromMaster = FALSE);
	virtual Capture* GetTopCaptureBitmap(Capture* pFromCapture = NULL) const;
	virtual BOOL TopCaptureIsDirect()			{return (GetTopCapture() && GetTopCapture()->IsDirect());}

// Internal worker functions
protected:
	virtual BOOL SetRenderToCapture(Capture* pCapture, BOOL bApplyClipRegion = FALSE, BOOL bSetBitmap = TRUE);
	virtual BOOL SetupDirectCapture(Capture* pNewCapture, NodeRenderableInk* pSupplier);

// State variables
//private:
//	StackT<Capture*> m_CaptureStack;


	/////////////////////////////////////////////////////////////////////
	// Additional stuff for GRenderRegionWrapper (affects how bmp mem is allocated)
protected:
	BOOL IsWrapped;
	void SetIsWrapped(BOOL flag) { IsWrapped = flag; }

	/////////////////////////////////////////////////////////////////////////////////////////
	//	Karim 10/04/2000
	//	Implementation of clipping within a GRenderRegion.
public:
	virtual void SetClipRegion		(ClipRegionAttribute * pClipAttr, BOOL Temp);
	virtual void RestoreClipRegion	(ClipRegionAttribute * pClipAttr, BOOL Temp);

private:
//	ClipRegionMap	m_ClipRegionMap;

protected:
	const REGION* MakeClipRegionFromClipAttr(ClipRegionAttribute* pClipAttr);
	BOOL ApplyCurrentClipRegion();
	void FreeStoredClipRegions();
	/////////////////////////////////////////////////////////////////////////////////////////

public:
	void OffsetByHalfPixel();
	virtual BOOL GrabBitmap(DocRect* rectGrab, LPBITMAPINFO* plpBitmapInfo, LPBYTE* plpBits);

#ifdef _DEBUG
virtual void DebugTrace();
	BOOL DumpCaptureStack();
#endif

protected:
	// Andy Hills, 23-10-00:
	// Helper functions for RenderBitmapFill and SetBitmapTransparencyFill
	BOOL IsScaledUp(FillGeometryAttribute *Fill);
	BOOL IsAt100Percent(FillGeometryAttribute *Fill);
	BOOL IsDistorted(FillGeometryAttribute *Fill);
	BOOL NeedToSmooth(FillGeometryAttribute *Fill,BOOL bAlreadyScaled);
	virtual BOOL SetSmoothingFlags(FillGeometryAttribute *Fill);
};


class OpDescriptor;


/******************************************************************************************

>	class OpGDraw : public Operation

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/94
	Purpose:	Operation to toggle gdraw setting
	Notes:		Will be on a dialog somewhere eventually

********************************************************************************************/

class OpGDraw : public Operation
{
	CC_DECLARE_DYNCREATE( OpGDraw )

public:
	OpGDraw();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);	
};

#define	OPTOKEN_GDRAW		_T("GDrawToggle")

// We now always have CDraw (which pretends to be GDraw)
//#define REAL_GDRAW 1

// the possibility of nasty DDBs is only available on Intel platforms. It is permitted on
// Intel 16-bit for test purposes
//#ifdef _M_IX86
//#define	REAL_DDB	0
//#else
//#define	REAL_DDB	0
//#endif

#endif
