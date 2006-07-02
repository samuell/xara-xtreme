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

// Camelot Native EPS format filter

#ifndef INC_CAMELEPS
#define INC_CAMELEPS

#include "aw_eps.h"
//#include "epsfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "saveeps.h"
#include "nodemold.h"
//#include "rrcaps.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class KernelBitmap;
class OILBitmap;
class NodeRegularShape;
class CCAttrMap;
class ColourContext;

typedef enum
{
	PENDING_BITMAP_NONE,
	PENDING_BITMAP_FILL,
	PENDING_BITMAP_TRANSPFILL,
	PENDING_BITMAP_OBJECT,
	PENDING_BITMAP_POOLITEM,
	PENDING_BITMAP_OBJECT_FILL
} PendingBitmap;

typedef enum
{
	CAMEPS_FILL_LINEAR = 1,
	CAMEPS_FILL_ELLIPTICAL,
	CAMEPS_FILL_CIRCULAR,
	CAMEPS_FILL_CONICAL,
	CAMEPS_FILL_BITMAP,
	CAMEPS_FILL_FRACTAL,
	CAMEPS_FILL_NEWLINEAR,
	CAMEPS_FILL_NEWBITMAP
} CamelotEPSFillType;


// cso/ceo enumerated tags
enum
{
	EOTAG_FONTFLAGS = 1,
	EOTAG_BITMAPFLAGS,
	EOTAG_TEXTWRAPPED,
	EOTAG_FONTTYPE,
	EOTAG_GUIDELAYER,
	EOTAG_GUIDELINE,
	EOTAG_MOULDTHRESHOLD
};


// All the commands used by Camelot EPS files.
enum
{
	// Chromatic fill geometries
	EPSC_caz = EPSC_aes + 1,
	EPSC_cax,

	// Transparent fill geometries
	EPSC_cxt,

	// Chromatic fill effects
	EPSC_cxe,

	// Chromatic fill mappings
	EPSC_cxm,

	// Transparent fill mappings
	EPSC_cxmt,

	// Bitmap objects/bitmap fills
	EPSC_cbm,
	EPSC_csbm,
	EPSC_cebm,

	// Line/Fill colours
	EPSC_cx,
	EPSC_cX,
	EPSC_ck,
	EPSC_cK,

	// Stroke transparency
	EPSC_cst,

	// Arrow heads
	EPSC_csah,
	EPSC_ceah,

	// Dash Patterns
	EPSC_cdp,

	// Blends
	EPSC_csbd,
	EPSC_cebd,
	EPSC_csbr,
	EPSC_cebr,

	// Regular shapes
	EPSC_csrs,
	EPSC_crsp,
	EPSC_crstm,
	EPSC_crsp1,
	EPSC_crsp2,
	EPSC_cers,

	// Mould commands
	EPSC_csev,
	EPSC_ceev,
	EPSC_cspr,
	EPSC_cepr,
	EPSC_csmp,
	EPSC_cemp,
	EPSC_csso,
	EPSC_ceso,
	EPSC_csdo,
	EPSC_cedo,

 	// Text native commands
 	EPSC_ctf,
 	EPSC_ctb,
	EPSC_cti,
	EPSC_cts,
	EPSC_ctp,
	EPSC_ctls,

	EPSC_cso,
	EPSC_ceo,
	EPSC_cfft,	// font flags token
	EPSC_cftf,	// Type of font
	EPSC_cbot,	// Bitmap object token

	// Renderable bitmap tokens
	EPSC_cpal,	// Bitmap palette token

	// Guide layer & guidelines
	EPSC_glyr,
	EPSC_glne,

	EPSC_cmth,	// mould threshold token

	EPSC_cag,	// new grey level geometric fill operator

	// Another bitmap token
	EPSC_cbti,	// Bitmap transparency index

	// Sentinel - leave this one last
	EPSC_cameps_last
};


/********************************************************************************************

>	class CamelotEPSFilter : public ArtWorksEPSFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	A filter that imports EPS files in Camelot EPS format.
	SeeAlso:	EPSFilter; ArtWorksEPSFilter

********************************************************************************************/

class CamelotEPSFilter : public ArtWorksEPSFilter
{
	CC_DECLARE_DYNAMIC(CamelotEPSFilter);
	
public:
	CamelotEPSFilter();
	BOOL Init();
	BOOL IsDefaultDocRequired(const TCHAR* pcszPathName);
	BOOL PrepareToExport(CCLexFile*, Spread* pSpread);
	TCHAR* GetEPSCommand(EPSCommand Cmd);
	BOOL ExportBitmap(KernelBitmap&);
	
	// This filter can include a Preview Bmp
	virtual BOOL CanIncludePreviewBmp();

protected:
	virtual void LookUpToken();
	virtual BOOL ProcessToken();
	INT32 EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize);
	BOOL PrepareToImport();
	void CleanUpAfterImport(BOOL Successful);

	virtual BOOL GetExportOptions( );

	PendingBitmap		m_PendingBitmap;
	INT32				BitmapTransparencyIndex;
	virtual BOOL ReadBitmap();
	virtual void BitmapPoolAttach(KernelBitmap* pBitmap);
	KernelBitmap *pBitmap;

	NodeRegularShape *pRegularShape;
	BOOL RegularShapeIsFilled;
	BOOL RegularShapeIsStroked;

	struct _BitmapAttrs
	{
		DocCoord Coords[3];

		DocColour StartCol;
		DocColour EndCol;

		UINT32	 TranspType; 	// Only needed for transparent bitmap fills
		UINT32	 Transp; 		// Only needed for transparent bitmap fills
		UINT32	 EndTransp; 	// Only needed for transparent bitmap fills

	} BitmapAttrs;

	// Blend and blender processing functions
	BOOL ProcessBlend();
	BOOL ProcessBlender();
	BOOL StartBlend();
	BOOL StartBlender();
	BOOL EndBlend();
	BOOL EndBlender();

	// Regular shape processing functions
	BOOL ProcessRegularShape();
	BOOL ProcessRegularShapeParams();

	// Mould processing functions
	BOOL ProcessEnvelope();
	BOOL ProcessPerspective();
	BOOL StartMould(MouldSpace mSpace);
	BOOL EndMould();
	BOOL RemoveUnwantedAttributes(NodeMould* pNodeMould);
	void BuildSubtreeAttrMap(CCAttrMap *pAttribMap, Node* pNode);
	BOOL ProcessMould();
	BOOL ProcessMouldTokens();
	BOOL ProcessMouldShape();
	BOOL ProcessMouldSourceObjs();
	BOOL ProcessMouldDestinObjs();
	BOOL ProcessMouldThreshold();
	BOOL ReadSimplePath(Path* TempPath);

	BOOL ProcessFontFlags();
	BOOL ProcessBitmapFlags();
	BOOL ProcessFontType();
	BOOL FindEndOfTag();

	// Guide layer && guideline processing
	BOOL ProcessGuideLayer();
	BOOL ProcessGuideline();

	// The array of keyword names
	static CommandMap CamelotCommands[];
};

class EPSFontCache
{
public:
	EPSFontCache();

public:
	BOOL Valid;
	BOOL Complex;
	WORD Typeface;
	BOOL Bold;
	BOOL Italic;
	MILLIPOINT Size;
	FIXED16 a, b, c, d;
};

/********************************************************************************************

>	class CamelotEPSRenderRegion : public ArtWorksEPSRenderRegion

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/03/94
	Purpose:	A render region for exporting files in the native EPS format.
	SeeAlso:	EPSRenderRegion; ArtWorksEPSRenderRegion

********************************************************************************************/

class CamelotEPSRenderRegion : public ArtWorksEPSRenderRegion
{
	CC_DECLARE_DYNAMIC(CamelotEPSRenderRegion)
	
public:
	CamelotEPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);

	virtual ~CamelotEPSRenderRegion();

	// This version handles grad fills.
	void GetValidPathAttributes();
	void GetRenderRegionCaps(RRCaps* pCaps);

	BOOL StartRender();
	BOOL StopRender();

	void DrawPathToOutputDevice(Path *DrawPath, PathShape shapePath=PATHSHAPE_PATH);
	BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	// Function to draw a bitmap into the render region using a mask
	virtual SlowJobResult DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  		   MaskedRenderRegion* pMask, ProgressDisplay *Progress);

	virtual BOOL RenderChar(WCHAR ch, Matrix* pMatrix);

	// Must it export the print componets?
	virtual BOOL NeedsPrintComponents ( void );

	// Commit suicide - used instead of if ( IS_A (...) )
	virtual void ConditionalSuicide ( void );

	// write out a return character (\r). Used in Wrap-around text, etc...
	virtual BOOL WriteNewLine ( void );

	// these two functions are used to call the base EPS Output commands.
	virtual void OutputStrokeColour ();
	virtual void OutputFillColour ();

	virtual BOOL DrawParallelogramBitmap(DocCoord *Coords, OILBitmap *pBitmap, EFFECTTYPE Effect = EFFECT_RGB,
				 DocColour *StartCol = NULL, DocColour *EndCol = NULL);

protected:
	virtual BOOL WriteEPSBoundingBox ( void );
	virtual BOOL WriteProlog(KernelDC*);
	virtual BOOL WriteSetup(KernelDC*);
	virtual BOOL WriteEPSTrailerComments ( void );

	BOOL DrawClippedBitmap(Path *DrawPath);

	BOOL SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, MILLIPOINT Size);
	BOOL SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, FIXED16 *abcd);

	// (ChrisG - 30/01/01) Changed to virtual, so that the pre-1.5 export can use CMYK colours, instead
	//	of RGB ones (as used by the Xara X EPS export) .
	virtual BOOL OutputGradFillColours(DocColour* StartCol, DocColour* EndCol, ColourContext* pContext=NULL);
	BOOL GetCurrFillEffect(UINT32& EffectType);

	// Used for rendering text via PostScript fonts
	EPSFontCache FontInfo;

	// Colour separation - bitmap support stuff
	void ColourSeparateScanline24(BYTE *DestBuffer, BYTE *SrcBuffer, INT32 PixelWidth);

	// Separation look-up tables for bitmap output (or NULL if not separating) -  This references
	// a set of 5 256-byte lookup tables (C, M, Y, UCR, BlackGen), or is NULL if not separating
	BYTE *SepTables;
};



#endif  // INC_CAMELEPS


