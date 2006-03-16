// $Id: osrndrgn.h 662 2006-03-14 21:31:49Z alex $
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

// OSRNDRGN.H
//
// Created: 29/6/93 by Will
//

#ifndef INC_OSRNDRGN
#define INC_OSRNDRGN

#include "rndrgn.h"
#include "paths.h"	// We access path objects directly (we are a friend)
#include "wincoord.h"
#include "winrect.h"
#include "rendtype.h"
#include "gbrush.h"
#include "gconsts.h" // Test only
#include "rrcaps.h"

class GradFillAttribute;
class BitmapFillAttribute;
class LinearFillAttribute;
class RadialFillAttribute;
class ConicalFillAttribute;
class SquareFillAttribute;
class ThreeColFillAttribute;
class FourColFillAttribute;
class KernelBitmap;
class DocView;
class CRgn;
class CPalette;

// Used for rendering text via GDI
// (It is a simple cache system).
class OSRRFontInfo
{
public:
	OSRRFontInfo();

public:
	WORD Typeface;
	BOOL Bold;
	BOOL Italic;
	MILLIPOINT Width;
	MILLIPOINT Height;
	ANGLE Rotation;
	wxFont *pRenderFont;
	wxFont *pOldFont;
};


class EORCacheClass
{
public:
	EORCacheClass() : Valid(FALSE) {}

	BOOL Valid;
	DocColour SourceColour;
	COLORREF EORColour;
};


// This is the size of the buffer we use when flattening paths - the maximum sensible
// size is 16384 points, as this is 64k, and obviosuly the longest polyline Win16
// can handle.
#define	SIZEOF_POLYLINE_BUFFER	8192

/********************************************************************************************

>	class OSRenderRegion : public RenderRegion

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Purpose:	Encapsulates rendering using the OS. (GDI, RiscOS, etc).
				This object is created when something needs rendering using the OS calls
				instead of Gavins routines.  This will be ALL THE TIME for the moment.
				See RenderRegion notes for more general details of this class.

********************************************************************************************/
        
class CCAPI OSRenderRegion : public RenderRegion
{
	CC_DECLARE_DYNAMIC(OSRenderRegion)

public:
	// Construction and creation
	// This should always be used to create OSRenderRegions
	static RenderRegion* Create(DocRect ClipRegion, 
								Matrix ConvertMatrix, 
								FIXED16 ViewScale,
								RenderType rType,
								View* pView = NULL,
								BOOL UseOSRendering = FALSE,
								BOOL bForce32BPP = FALSE);

	~OSRenderRegion();
	OSRenderRegion(const OSRenderRegion &other);

	static BOOL Init();
	static void FlushEORCache(void);

	virtual BOOL AttachDevice(View*, wxDC*, Spread* SpreadToAttach = NULL, bool fOwned = false );
	virtual BOOL InitDevice();
	
	// Functions called as rendering starts and stops
	BOOL StartRender();
	BOOL StopRender();

	// Enable/Disable solid-colour (no-dither) plotting
	virtual void SetSolidColours(BOOL SetSolid);

	// Functions that actually draw somthing
	void DrawPathToOutputDevice(Path* PathToDraw, PathShape shapePath=PATHSHAPE_PATH);
	void DrawRect(DocRect* PathToDraw);
	void DrawDragRect(DocRect* PathToDraw);
	virtual void DrawDragBounds(DocRect *RectToRender);
	virtual void DrawDashLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	void DrawPixel(const DocCoord &Point);
	void DrawCross(const DocCoord &Point, const UINT32 Size);
	void DrawBlob(DocCoord p, BlobType type);

	void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap);
	void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL);
	BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape);
	void DrawBitmapBlob(const DocCoord &Point, ResourceID resID );

	static wxSize GetFixedDCPPI (wxDC &dc);

	// Function to draw a bitmap into the render region using a mask, and
	// and equivalent function to do the same, only colour separating it as it goes
	// The Separated version is called by this one if colour seps are enabled
	virtual SlowJobResult DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  		   MaskedRenderRegion* pMask, ProgressDisplay *Progress);
protected:
	virtual SlowJobResult DrawSeparatedMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  		   MaskedRenderRegion* pMask, ProgressDisplay *Progress);

public:
	void DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat = DEFAULT_TEXT_FORMATTING);
	void SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background);
	void GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi = NULL);

	static void GetBlobRect( FIXED16, const DocCoord& BlobPoint, BlobType bType, DocRect *pResult );

	BOOL RenderGradFillPath(Path *, GradFillAttribute* );
	BOOL RenderBitmapFill  (Path *, BitmapFillAttribute* );

	virtual BOOL RenderChar(WCHAR ch, Matrix* pMatrix);

	// Conversion functions
	static WinRect DocRectToWin( View *pView, const Matrix& RenderMatrix, 
								 const DocRect& docrect,
				  				 INT32 leftshift, INT32 topshift, INT32 rightshift, INT32 bottomshift,
				  				 BOOL MightClip = FALSE);
	//static WinRect DocRectToWin( const Matrix& RenderMatrix, const DocRect& docrect,
	//							 const INT32 dpi);

	static WinRect DocRectToWin( const Matrix& RenderMatrix, const DocRect& docrect,
								 const double dpi);
	// Same as above but used for exporting bitmaps to try and fix pixel problems
	static WinRect BitmapDocRectToWin( Matrix& RenderMatrix, const DocRect& docrect,
									   const double dpi);
	static BOOL CalculateGavinOffsetsWinRect( const Matrix& RenderMatrix,
											  const DocRect& DRect,
											  const double dpi,
											  GMATRIX* GMat,
											  BOOL bCentralise,
											  double* pdXCentralAdjust,
											  double* pdYCentralAdjust);

	static DocRect WinRectToDoc( const Matrix &RenderMatrix,
								 const WinRect& WRect,
							  	 const double dpi);

	static BOOL DoBetterLines;								// TRUE for GDraw lnies, FALSE for GDI

	static MILLIPOINT GetHitTestRadius(DocView *pDocView);

	BOOL StrokeProperly( Path *const );

	// Function to allow render regions to say what they can not render
	virtual void GetRenderRegionCaps(RRCaps* pCaps);

protected:
	
	static INT32 HitTestRadius;				// how close a click must be to hit-detect

	// Constructor (protected as you should use Create() )
	OSRenderRegion(DocRect ClipRegion, Matrix ConvertMatrix, FIXED16 ViewScale = 1);

	// Various startup and shutdown functions
	
	void InitClipping();
	void DeInitClipping();

	void InitAttributes();
	void DeInitAttributes();

	// These functions are called from the RenderRegion base class when attributes are changed.
	void SetLineAttributes(ChangeLineAttrType Type = CHANGELINEATTR_ALL);
	void SetFillAttributes(ChangeAttrType Type = CHANGEATTR_ALL);
	void SetOSDrawingMode();
	void SetQualityLevel();

	// Conversion functions
	Coord DocCoordToOS256(const DocCoord& DocPoint);
	WinCoord DocCoordToWin(const DocCoord& DocPoint);
	WinRect DocRectToWin( const DocRect& docrect, INT32 leftshift, INT32 topshift,
						  INT32 rightshift, INT32 bottomshift, BOOL MightClip = FALSE)
						{ return DocRectToWin(RenderView, RenderMatrix, docrect, 
											  leftshift, topshift,
											  rightshift, bottomshift, MightClip ); }

	// Render fills.
	BOOL RenderRadialFill	( Path *PathToDraw, RadialFillAttribute		*Fill );
	BOOL RenderLinearFill	( Path *PathToDraw, LinearFillAttribute		*Fill );
	BOOL RenderConicalFill	( Path *PathToDraw, ConicalFillAttribute	*Fill );
	BOOL RenderSquareFill	( Path *PathToDraw, SquareFillAttribute		*Fill );
	BOOL RenderThreeColFill	( Path *PathToDraw, ThreeColFillAttribute	*Fill );
	BOOL RenderFourColFill	( Path *PathToDraw, FourColFillAttribute	*Fill );

	static MILLIPOINT CalcDistance(DocCoord a, DocCoord b);
	static void MakeEllipticalPath(Path *pPath, DocCoord Parallel[4]);

#if 0
	// Functions to help with line arrows
	void DrawLineArrow(ArrowRec &ArrowToDraw, const Coord &Centre, const Coord &Direction);
#endif

	// Helper functions
	void CreateNewPen();
	void SelectNewPen();
	void CreateNewBrush();
	void SelectNewBrush();

	BOOL SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, 
					   MILLIPOINT Width, MILLIPOINT Height, ANGLE Rotation);

	void MakeDashType(DashRec&, DashType*);

	// Functions to render a Path
	void RenderPath32( Path *DrawPath);
	void RenderPath( Path *DrawPath );

	// low-level internal bezier primitives
	BOOL RawRenderPath32( Path *const );
	BOOL RawRenderPath( DocCoord *const, PathVerb *const, INT32 NumCoords, INT32 *PolyCount, INT32 *Count,
						INT32 Flatness = 0, INT32 *pActualFlatness = NULL );
	BOOL SetClipToPathTemporary( Path *const);

	// this macro for use in RawRenderPath calls
	#define	NORMALPATH(pp)	pp->GetCoordArray(), pp->GetVerbArray(), pp->GetNumCoords()

	// Bezier flattening functions
	BOOL Bezier(INT32 Px0,INT32 Py0, INT32 Px1,INT32 Py1, INT32 Px2,INT32 Py2, INT32 Px3,INT32 Py3, INT32 Flatness);
	BOOL Split(INT32 Px0,INT32 Py0, INT32 Px1,INT32 Py1, INT32 Px2,INT32 Py2, INT32 Px3,INT32 Py3, INT32 Flatness);
	INT32 CalcPathFlattening();

	// misc
	INT32 	 MPtoLP(MILLIPOINT MPtoConvert);
	wxColour CalcEORColour( DocColour &Wanted, COLORREF = RGB(255,255,255) );
	void CalcLogBrush(wxBrush*, DocColour &);
//	BOOL NewPolyDraw( const POINT*, const BYTE*, INT32);

	wxPoint FindBitmapOrigin(DocCoord Centre, INT32 Width, INT32 Height);

// Variables
protected:

	MILLIPOINT CalcScaledPixelWidth();
	MILLIPOINT CalcPixelWidth();

	// The rendering flags
	struct
	{
		BOOL Metafile   : 1;		// TRUE if metafile
//		BOOL GDI32      : 1;		// TRUE if can use GDI32 calls
		BOOL ValidPen   : 1;		// TRUE if RenderPen is correct
		BOOL ValidBrush : 1;		// TRUE if RenderBrush is correct
		BOOL UsePalette : 1;		// TRUE if using special palette
	} RFlags;

	// The clip region
	wxRegion*  OSClipRegion;
	wxPalette* OldPalette;			// NULL if no old palette

	GBrush GDrawBrush;				// special Gavin-compatible brush

protected:
	void GetValidPen();
	void GetValidBrush();

	// The position in the polygon array to insert points at and the array
	static wxPoint PointArray[SIZEOF_POLYLINE_BUFFER];

	// cache for EOR colour
	static EORCacheClass EORCache;
	
	INT32 InsertPos;

	// The size of a pixel
	FIXED16 PixelScale;

	INT32	nFillStyle ;

	// A selection of pens and brushes for drawing with
	INT32 	CurrentPen;
	INT32 	CurrentBrush;
	wxPen 	RenderPen[2];
	wxBrush RenderBrush[2];
//	wxPen 	*OldPen;
//	wxBrush *OldBrush;
	wxFont	*OldFont;				// Used for supporting "FixedSystem" font rendering

	OSRRFontInfo FontInfo;

	// The brush origin
	wxPoint NewBrushOrg;

	// This render regions caps. We keep a cached version here as it can be needed in rendering
	RRCaps Caps;

protected:
	BYTE *SepTables;				// NULL, or a pointer to colour separation tables
};

/********************************************************************************************

>	class PaperRenderRegion : public OSRenderRegion

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/94
	Purpose:	Provide a fast way of rendering paper.  One of these regions is used to
				render paper on screen, to avoid any lag we might get from using a bitmap
				type render region (e.g. GRenderRegion).
	SeeAlso:	OSRenderRegion; GRenderRegion; DocView

********************************************************************************************/

class PaperRenderRegion : public OSRenderRegion
{
	CC_DECLARE_DYNAMIC(PaperRenderRegion)

public:
	// Construction and creation
	PaperRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);
	~PaperRenderRegion();

	// Changing the render destination.
	BOOL AttachDevice(View *pView, Spread *pSpread, wxDC *pDC, Matrix& ViewMatrix,
					  FIXED16 ViewScale, DocRect& ClipRect, bool fOwned = false );
	void DetachDevice();

	virtual BOOL InitDevice();
};

#endif
