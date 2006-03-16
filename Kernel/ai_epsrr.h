// $Id: ai_epsrr.h 662 2006-03-14 21:31:49Z alex $
// Declaration of Adobe Illustrator EPS render region.
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

#ifndef INC_AI_EPS_RENDER_REGION
#define INC_AI_EPS_RENDER_REGION

#include "saveeps.h"
#include "nodebev.h"
#include "nodeshad.h"


// Constants.
const char	LinearGradient []	= "Linear Gradient";
const char	RadialGradient []	= "Radial Gradient";
const INT32	LinearFill			= 0;
const INT32	RadialFill			= 1;

// number of intermediate steps to use when emulating short or long hsv fills (HSV fills
//	are done by creating an ordinary RGB fill, but with intermediate colour stops to simulate
//	the hue change).
const INT32	NumFillEmulationSteps = 5;

/********************************************************************************************

>	class AIEPSGradientScanRenderRegion : public VectorFileRenderRegion

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/2001
	Purpose:	RenderRegion used to scan through the tree, looking for gradient fills to be
				cached for Illustrator export.
	SeeAlso:	AIEPSRenderRegion, 

********************************************************************************************/
class AIEPSGradientScanRenderRegion : public VectorFileRenderRegion
{
	CC_DECLARE_DYNAMIC (AIEPSGradientScanRenderRegion)

protected:

	RenderRegion * m_pController;

public:

	AIEPSGradientScanRenderRegion (RenderRegion * pControllingRegion);

	RenderRegion * GetControllingRegion () {return m_pController;}
	void SetControllingRegion (RenderRegion * pControllingRegion);

	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH);

	virtual BOOL StopRender() {return FALSE;};
	virtual void DrawRect(DocRect *RectToRender) {};
	virtual void DrawDragRect(DocRect *RectToRender) {};
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint) {};
	virtual void DrawPixel(const DocCoord &Point) {};
	virtual void DrawBlob(DocCoord p, BlobType type) {};
	virtual void DrawCross(const DocCoord &Point, const UINT32 Size) {};

	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap) {};
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL) {};
	virtual void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape) {};
	virtual void DrawBitmapBlob(const DocCoord &Point, ResourceID resID ) {};

	virtual void InitClipping() {};
	virtual void InitAttributes() {};

	virtual void SetOSDrawingMode() {};
	virtual MILLIPOINT CalcPixelWidth() {return MILLIPOINT (1);};
	virtual MILLIPOINT CalcScaledPixelWidth() {return MILLIPOINT (1);};
};



/********************************************************************************************

>	class ListFill : public CCObject

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001
	Purpose:	Relatively generic Gradient Fill holder class, so that gradient fills can 
				be stored in a linked list (mainly used while scanning for grad fills)
	SeeAlso:	AIEPSRenderRegion, AIEPSGradientScanRenderRegion

********************************************************************************************/
class ListFill : public CCObject
{
	CC_DECLARE_DYNAMIC (ListFill);

public:

	ListFill ();
	ListFill (FillGeometryAttribute * pNewFill);
	ListFill (FillGeometryAttribute * pNewFill, EFFECTTYPE eff, ListFill * pNewNext);
	~ListFill ();

	FillGeometryAttribute * pFill;
	EFFECTTYPE effect;
	ListFill * pNext;
	INT32 id;
};

/********************************************************************************************

>	class AIEPSRenderRegion : public EPSRenderRegion

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>, extended by Graeme
	Created:	08/03/94
	Purpose:	A render region for exporting EPS files in Adobe Illustrator 7.0 EPS format.
	SeeAlso:	EPSRenderRegion

********************************************************************************************/
class AIEPSRenderRegion : public EPSRenderRegion
{
friend class AIEPSGradientScanRenderRegion;

	CC_DECLARE_DYNAMIC (AIEPSRenderRegion)
	
public:
	AIEPSRenderRegion (DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);
	~AIEPSRenderRegion ();

	// Export methods for dealing with out of the ordinary classes.
	BOOL ExportBitmap ( NodeBitmap	*pNodeBMP );		// Write a bitmap definition.
	BOOL ExportBevel  ( NodeBevel	*pBevel );			// Write a bevel definition.
	BOOL ExportShadow ( OILBitmap	*pBitmap,			// Write a shadow to the file.
						UINT32		Darkness,
						DocRect		&Bounds );
	BOOL ExportLayer  ( Layer		*pLayer );			// Write a layer to the file.
	BOOL StartLayer   ( Layer		*pLayer );			// Start writing a layer definition.

	virtual BOOL RenderChar	  (WCHAR ch, Matrix* pMatrix);		// Write out a single character.

	BOOL ExportingOnPath ();						// Whether we're exporting text on a path

	virtual void OverflowTextStart ();				// Start a block of overflow text
	virtual void OverflowTextFinish ();				// Finish a block of overflow text

	// Mask control functions.
	BOOL WriteMask ( Path *MaskPath, BOOL OutputGroupToken = FALSE );

	/////////////////////////////////////////////////////////////////////////////////////////
	//	ChrisG 8/11/2000
	//	Implementation of clipping within an EPSRenderRegion.
	virtual void SetClipRegion		(ClipRegionAttribute * pClipAttr, BOOL Temp);
	virtual void RestoreClipRegion	(ClipRegionAttribute * pClipAttr, BOOL Temp);
	/////////////////////////////////////////////////////////////////////////////////////////

	// ChrisG 8/11/00 used make all text printed out as shapes (TRUE), or characters (FALSE)
	virtual void SetTextAsShapes (BOOL shapes);
	virtual BOOL GetTextAsShapes ();

	virtual BOOL IsGradientFillValidForExport (FillGeometryAttribute * pFill);
	virtual void IncludeGradientFill (FillGeometryAttribute * pFill, EFFECTTYPE effect);

protected:
	// Class-specific initialisation code.
	virtual void Initialise ();
	
	// Virtual functions to export document information for AI files.
	virtual BOOL WriteEPSVersion ();
	virtual BOOL WriteEPSProcessColours ();
	virtual BOOL WriteEPSResources ( EPSFilter	*pFilter,
									 Document	*pDocument );
	virtual BOOL WriteEPSProlog ( EPSFilter	*pFilter,
								  Document	*pDocument );

	virtual void OutputFillColour ( void );
									  
	// Extract the gradient fills from the tree, and save them to file.
	virtual BOOL WriteGradientFills ( Document *pDocument );

	// Determine whether or not to write the font definitions to file.
	virtual BOOL WriteDocumentSetup ( Document	*pDocument,
									  EPSFilter	*pFilter );

	virtual BOOL EndLayer ( void );

	// These Output functions output any additional information that the 
	//	Illustrator EPS export needs before calling the base EPS output.
	virtual void OutputWindingRule ();

	virtual void OutputStrokeColour ();
	virtual void OutputLineWidth ();
	virtual void OutputJoinType ();
	virtual void OutputDashPattern ();
	virtual void OutputStartCap ();
	virtual void OutputMitreLimit ();
	
	virtual void OutputFontName ();

	virtual void OutputTextRenderMode ();
	virtual void OutputTextAspectRatio ();
	virtual void OutputTextTracking ();
	virtual void OutputTextJustification ();
	virtual void OutputTextLineSpacing ();
	virtual void OutputTextBaselineShift ();
	virtual void OutputTextSubSuperScript ();

	virtual void OutputFillRGBColour ();
	virtual void OutputFillCMYKColour ();
	virtual void OutputStrokeRGBColour ();
	virtual void OutputStrokeCMYKColour ();

	// These OverflowText functions are used to control the output of overflow text 
	//	(TX) to	the EPS file.

	virtual void OverflowTextStartGap ();
	virtual void OverflowTextFinishGap ();
	virtual void OverflowTextWrite (TCHAR * text);
	virtual void OverflowTextWriteSingleColour (UINT32 n);
	virtual void OverflowTextWriteColour (PColourCMYK*);
	virtual void OverflowTextWriteNamedColour (DocColour*, ColourContext* pContext=NULL);
	virtual void OverflowTextWriteColourName (DocColour* pCol);

	// (Chris G 8/11/00) - added so that gradient-filled text can be exported as shapes.
	virtual BOOL RenderCharAsShape (WCHAR ch, Matrix* pMatrix);

	// (Chris G 9/11/00) - added to stop new line characters from being exported when text
	//	is exported as shapes.
	virtual BOOL WriteNewLine ( void );

private:
	// Functions for writing gradients out.
	virtual void BuildGradientCache (Node * pStartNode);
	virtual void BuildGradientCacheUsingScanner (Node * pNode, AIEPSGradientScanRenderRegion * pScan);
	virtual void WriteGradientCount ();
	virtual void WriteGradientDefinitions ();
	virtual void ClearGradientCache ();
	virtual INT32 FindGradientInCache (FillGeometryAttribute * pFill, EFFECTTYPE effect);

	BOOL WriteLinearFill ( FillGeometryAttribute * pFill, EFFECTTYPE effect, INT32 id);
	BOOL WriteRadialFill ( FillGeometryAttribute * pFill, EFFECTTYPE effect, INT32 id);
	virtual void WriteGradientFillInstance ();
	BOOL WriteGradientEntry ( DocColour	*pColour,
							  INT32		Position,
							  INT32		Midpoint );
	BOOL WriteFillEffectSteps (	DocColour *pColour1,
								INT32 pos1, 
								DocColour *pColour2, 
								INT32 pos2, 
								INT32 bias, 
								EFFECTTYPE effect);
	
	// Matrix write and store functions.
	BOOL LoadBitmapMatrix ( NodeBitmap	*pNodeBMP );
	BOOL LoadTranslationMatrix ( DocCoord &Translation );
	BOOL WriteLoadedMatrix ( void );

	BOOL WriteMaskTags ( void );

	// Bitmap handling functions.
	BOOL WriteBitmapRecord	( OILBitmap	*pBitmap );
	BOOL WriteBitmapHeader	( INT32 Width,
							  INT32 Height );
	BOOL WriteBitmapBody	( OILBitmap	*pBitmap,
							  INT32		Width,
							  INT32		Height );
	BOOL WriteContoneBody	( OILBitmap	*pBitmap );
	BOOL WriteBitmapTail	( void );

	// Member variables.
	ListFill *	m_pLinearGradList;	// The linear gradient fill cache
	ListFill *	m_pRadialGradList;	// The radial gradient fill cache

	double		m_a;				// Transformation matrix component.
	double		m_b;				// Transformation matrix component.
	double		m_c;				// Transformation matrix component.
	double		m_d;				// Transformation matrix component.
	DocCoord	m_T;				// Transformation matrix component.

	BOOL		m_ActiveLayer;		// Is there a layer being exported right now?
	UINT32		m_LayerColour;		// Colour code used to ID a layer.

	FILE *		m_fpOverflowText;	// File to write the extra info to	( for the overflow (TX)	)
	BOOL		m_bInTextGap;		// If we're setting attributes		( part of a text block	)
	BOOL		m_bTextAsShapes;	// we're going to draw text as shapes (i.e. it's gradient filled.

};

#endif
