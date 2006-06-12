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


// RNDRGN.H
//
// Created: 10/5/93 by Phil, Will, Simon
//

#ifndef INC_RNDRGN
#define INC_RNDRGN

#include "listitem.h"		// We are derived for a ListItem
#include "handles.h"		// Need definition of a MHANDLE
#include "docrect.h"
#include "rndstack.h"
#include "quality.h"
#include "attr.h"
#include "pathtype.h"
#include "gradtype.h"		// For Fill effect types
#include "matrix.h"
#include "txtattr.h"
#include "slowjob.h"

#include "pathshap.h"		// For the PathShape typedef

#include "winrect.h"
#include "bitmapcache.h"	// Included by GAT
#include "capturemanager.h"
#include "genstack.h"

// These constants are WIN32 specific and are used by Windows - only code such as the internet galleries
// They DO NOT NEED to be redefined for other operating systems as the code using them simply won't exist
// Well, actually, they do, as CALCRECT (for instance) is used elsewhere

#define FORMAT_TOP              0x00000000
#define FORMAT_LEFT             0x00000000
#define FORMAT_CENTER           0x00000001
#define FORMAT_RIGHT            0x00000002
#define FORMAT_VCENTER          0x00000004
#define FORMAT_BOTTOM           0x00000008
#define FORMAT_WORDBREAK        0x00000010
#define FORMAT_SINGLELINE       0x00000020
#define FORMAT_EXPANFORMATABS       0x00000040
#define FORMAT_TABSTOP          0x00000080
#define FORMAT_NOCLIP           0x00000100
#define FORMAT_EXTERNALLEADING  0x00000200
#define FORMAT_CALCRECT         0x00000400
#define FORMAT_NOPREFIX         0x00000800
#define FORMAT_INTERNAL         0x00001000
#define DEFAULT_TEXT_FORMATTING		FORMAT_SINGLELINE | FORMAT_NOPREFIX | FORMAT_VCENTER 


class Node;
class Matrix;
class Spread;
class Path;
class AttributeValue;
class AttributeEntry;
class View;
class KernelBitmap;
class RRCaps;
class MaskedRenderRegion;
class NodeBitmap;
class DocColour;
class ColourContext;
class StrokeColourAttribute;
class StrokeTranspAttribute;
class StrokeTypeAttrValue;
class ColourFillAttribute;
class FillMappingAttribute;
class FillEffectAttribute;
class TranspFillAttribute;
class TranspFillMappingAttribute;
class LineWidthAttribute;
class VariableWidthAttrValue;
class StartCapAttribute;
class EndCapAttribute;
class StartArrowAttribute;
class EndArrowAttribute;
class JoinTypeAttribute;
class MitreLimitAttribute;
class DashPatternAttribute;
class WindingRuleAttribute;
class DrawingModeAttribute;
class QualityAttribute;
class BitmapFillAttribute;
class WebAddressAttribute;
class MouldAttribute;
class TxtFontTypefaceAttribute;
class TxtBoldAttribute;
class TxtUnderlineAttribute;
class TxtItalicAttribute;
class TxtAspectRatioAttribute;
class TxtJustificationAttribute;
class TxtTrackingAttribute;
class TxtTrackingAttribute;
class TxtFontSizeAttribute;
class TxtScriptAttribute;
class TxtBaseLineAttribute;
class TxtLineSpaceAttribute;
class OverprintLineAttrValue;
class OverprintFillAttrValue;
class PrintOnAllPlatesAttrValue;
class ProgressDisplay;
class PathProcessor;
class OffscreenAttrValue;
class ClipRegionAttribute;
class CaptureManager;
class RenderCallback;

/********************************************************************************************

< ChangeAttrType

	Comment:	This is used in the function RenderRegion::SetFillAttributes(), to indicate
				that a fill attribute has changed and the render region should do whatever
				is appropriate to handle the change.  This enumerated type is just used
				to indicate which aspect of the fill has changed.
	SeeAlso:	RenderRegion::SetFillAttributes

********************************************************************************************/ 

typedef enum
{
	CHANGEATTR_ALL 		= -1,
	CHANGEATTR_GEOMETRY = 0,
	CHANGEATTR_EFFECT,
	CHANGEATTR_MAPPING,
	CHANGEATTR_TRANSP_GEOMETRY,
	CHANGEATTR_TRANSP_EFFECT,
	CHANGEATTR_TRANSP_MAPPING,
	CHANGEATTR_WINDING_RULE
} ChangeAttrType;

/********************************************************************************************

< ChangeLineAttrType

	Comment:	This is used in the function RenderRegion::SetLineAttributes(), to indicate
				that a line attribute has changed and the render region should do whatever
				is appropriate to handle the change.  This enumerated type is just used
				to indicate which aspect of the fill has changed.
	SeeAlso:	RenderRegion::SetLineAttributes

********************************************************************************************/ 

typedef enum
{
	CHANGELINEATTR_ALL = -1,
	CHANGEATTR_STROKECOL = 0,
	CHANGEATTR_STROKETRANSP,
	CHANGEATTR_LINEWIDTH,
	CHANGEATTR_JOINTYPE,
	CHANGEATTR_DASHPATTERN,
	CHANGEATTR_LINECAP,
	CHANGEATTR_STARTARROW,
	CHANGEATTR_ENDARROW,
	CHANGEATTR_MITRELIMIT
} ChangeLineAttrType;

/********************************************************************************************

< ChangeAttrType

	Comment:	This is used in the function RenderRegion::SetFillAttributes(), to indicate
				that a fill attribute has changed and the render region should do whatever
				is appropriate to handle the change.  This enumerated type is just used
				to indicate which aspect of the fill has changed.
	SeeAlso:	RenderRegion::SetFillAttributes

********************************************************************************************/ 

typedef enum
{
	CHANGEFILLATTR_ALL 		= -1,
	CHANGEATTR_FONT = 0,
	CHANGEATTR_BOLD,
	CHANGEATTR_ITALIC,
	CHANGEATTR_UNDERLINE,
	CHANGEATTR_ASPECT,
	CHANGEATTR_TRACKING,
	CHANGEATTR_SIZE,
	CHANGEATTR_JUSTIFY, 
	CHANGEATTR_SCRIPT,
	CHANGEATTR_BASELINE,
	CHANGEATTR_LINESPACE
} ChangeFillAttrType;


/********************************************************************************************

>	class SubRenderContext : public CC_CLASS_MEMDUMP

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Purpose:	Provide a base class for pointing to when storing sub-rendering
				contexts.  These are needed when a node takes a long time to render, such
				as a NodeBlender object which interrupts itself before all the blended
				objects have finished rendering.
				This class is essentially empty - derive your own specific context class
				from it.
	SeeAlso:	RenderRegion; RenderRegion::SetSubRenderContext
	Notes:		RenderTree function uses renderstate member to determine control flow
				when restarting interrupted tree rendering...

********************************************************************************************/

//#pragma message( __LOCMSG__ "SubRenderContext removed" )
class SubRenderContext : public CCObject
{
	CC_DECLARE_DYNAMIC(SubRenderContext);

	SubRenderContext() {renderstate = SUBTREE_ROOTONLY;}

public:
	SubtreeRenderState renderstate;
};


/********************************************************************************************

>	class SubTreeRenderContext : public SubRenderContext

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/2004
	Purpose:	Represents the rendering state where tree rendering has been suspended
				but the current node is the root of a subtree that has ALREADY BEEN RENDERED
	SeeAlso:	RenderRegion; RenderRegion::SetSubRenderContext

********************************************************************************************/

//#pragma message( __LOCMSG__ "SubTreeRenderContext removed" )
class SubTreeRenderContext : public SubRenderContext
{
	CC_DECLARE_DYNAMIC(SubTreeRenderContext);

	SubTreeRenderContext() {renderstate = SUBTREE_NORENDER;}
};


/********************************************************************************************

>	class RenderRegion : public ListItem

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Purpose:	THIS IS AN ABSTRACT CLASS. Don't try to create a RenderRegion itself, but
				rather create one of it's derived classes.
				A RenderRegion object is created when an area of the screen/printer (or any
				destination) has become invalid and needs to be redrawn. It	'lives' for as
				long as that region is still invalid.  It is also used for e.g. exporting
				files or in smart click detection.
				Background rendering and region merging is achieved by using a List of Render
				Region objects.  When a new region becomes invalid, a RenderRegion is created
				and added to that list.  If the rendering is interrupted before it is
				complete, then it will continue again when the OnIdle routine scans through
				the list of RenderRegions.
				RenderRegions can also be created at other times for Immediate rendering.
				To use a RenderRegion, create an OSRenderRegion or a GRenderRegion depending
				on the method of rendering required, then Attach a DC and CWnd to the region
				to specify the Render destination.  Next call InitRender which will return
				the RenderRegions previous render state if it had been interrupted or Null if
				it has just been created.  You can then set attributes, Save/Restore them,
				and Render objects. When rendering is complete, call ExitRender passing the
				current render state.		

	SeeAlso:	class OSRenderRegion; class GRenderRegion
		
********************************************************************************************/

class CCAPI RenderRegion : public ListItem
{

	CC_DECLARE_DYNAMIC( RenderRegion )
    
//#pragma message( __LOCMSG__ "RenderRegion stripped" )
public:
	// Render Region Creation and Destruction
	RenderRegion();
	RenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale);
	RenderRegion(const RenderRegion &other);	// Copy constructor
	virtual ~RenderRegion();

	static BOOL Init();

	// Function to copy all the parameters from another render region to this render region
	virtual BOOL CopyRenderInfo( const RenderRegion &Other);

	virtual BOOL AttachDevice(View *, CNativeDC *, Spread *SpreadToAttach = NULL, bool fOwned = false ); // OVERRIDE THIS
	virtual BOOL AttachDevice(View *, CCDC *, Spread *SpreadToAttach = NULL, bool fOwned = false ); // DO NOT OVERRIDE THIS
	virtual BOOL InitDevice();

	virtual BOOL ImmediateRender();

//	virtual BOOL CanBackgroundRender() { return FALSE; }

	virtual Node* GetBackmostChangedNode() { return m_pBackmostChangedNode; }
	virtual void SetBackmostChangedNode(Node* pInvalidNode);
	virtual void ClearBackmostChangedNode(Node* pNode);

	virtual BOOL RenderBits(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocRect rect, BOOL bForceNoTransp = TRUE, Node* pEffectsOwner = NULL);
	virtual BOOL RenderBits(LPBITMAPINFO lpBitmapInfo, LPBYTE lpBits, DocCoord* pCoords, INT32 numcoords, BOOL bForceNoTransp = TRUE, Node* pEffectsOwner = NULL);
	virtual BOOL RenderBits(KernelBitmap* pkBitmap, DocCoord* pCoords, INT32 numcoords, BOOL bForceNoTransp, Node* pEffectsOwner);

	virtual INT64	GetTotalCount() {return m_countTotal;}
	virtual INT64	GetStoredCount() {return m_countStored;}
	virtual void		SetStoredCount(INT64 llcount) {m_countStored = llcount;}

	// Functions to allow the transform matrix to be altered after the rr has been created
	virtual	void	SetMatrix( Matrix& );
			Matrix	GetMatrix() { return RenderMatrix; }
	virtual Matrix	ConcatenateMatrix( Matrix& );

	// static init function to set up the user preferences from the .ini file
	static BOOL InitPrefs();

	virtual BOOL StartRender();
	virtual BOOL StopRender() = 0;
	virtual void SetClean(BOOL bResetChangedBounds, BOOL FillWhite);
	virtual BOOL IsClean() {return FALSE;}

	virtual BOOL CloseDown ();					// Graeme (24-2-00).
	virtual void ConditionalSuicide ( void );	// Graeme (24-2-00).

	Node* 					GetRenderState();
	void					SetRenderState(Node *RenderState) { CurrentRenderState = RenderState; }

	// Set a sub-render context. Note that you must not touch it if it is locked, as
	// that means somebody above you in the tree can store a sub-render context, and
	// hence you must not be interruptible or it'll all go wrong. (A cheap bodge by
	// Jason to allow interruptible stroke rendering without breaking when inside a blend)
	SubRenderContext* 	GetSubRenderState() 
							{ return CurrentSubRenderState; }
	void 	  			SetSubRenderState(SubRenderContext *SubRenderState) 
							{
								ERROR3IF(SubRenderStateLocked && SubRenderState != NULL, "Oi! No!");
								CurrentSubRenderState = SubRenderState;
								if (SubRenderState == NULL)
									SubRenderStateLocked = FALSE;
							}
	BOOL IsSubRenderStateLocked(void) const { return(SubRenderStateLocked); };
	void LockSubRenderState(BOOL Lock) { SubRenderStateLocked = Lock; };


	CNativeDC *GetRenderDC() { return RenderDC; }
	INT32	GetTimeslice() const		{ return Timeslice; }

	void 	DefaultRender(BOOL bForceImmediate = FALSE);	// Called during background rendering
//	wxDC* 	GetDCHandle() 		{ return DCHandle; }
    View* 	GetRenderView() 	{ return RenderView; }
	Spread* GetRenderSpread() 	{ return RenderSpread; }
	BOOL 	IsPrinting() 		{ return RenderFlags.Printing; }
	BOOL 	IsRendering() 		{ return RenderFlags.Rendering; }

	BOOL 	IsVeryMono()		{ return RenderFlags.VeryMono; }
	BOOL 	IsHitDetect() const		{ return RenderFlags.VeryMono || RenderFlags.HitDetect; }

	BOOL	IsUsingSmoothedBitmaps() { return RenderFlags.SmoothedBitmaps; }
	void	SetUsingSmoothedBitmaps(BOOL set) { RenderFlags.SmoothedBitmaps = set; }

	void SaveContext();
	void RestoreContext();

#ifdef _DEBUG
	BOOL m_TraceOutContextLevels;
	INT32 m_CurrentContextLevel;
	virtual void DebugTrace();
#endif

	// For use by AttributeValue objects...
	virtual BOOL SaveAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp);
	void RestoreAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp);

	// Virtual Functions to help with rendering.
	virtual BOOL WantsGrids();

	// Set the background colour. This function is used for dealing with shadows.
	void SetBackgroundColour ( DocColour &Colour );

	// Functions that actually draw things		
	virtual void DrawPath(Path *PathToRender, PathProcessor *pCaller = NULL, PathShape psThis=PATHSHAPE_PATH);
					// Renders a path (possibly via PathProcessors)

	PathProcessor *GetFirstPathProcessor(void) { return(PathProcessorStack); };
	void PushPathProcessor(PathProcessor *pProcessor);
	void PopPathProcessor(void);
					// Path Processor stack management functions (called by attributes)
	
	void EnableAllBrushPathProcessors(BOOL Enable);

	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH) = 0;
					// Internal call used by DrawPath to actually render a path
					// after it has been processed by the pathProcessor system

	virtual void DrawRect(DocRect *RectToRender) = 0;
	virtual void DrawDragRect(DocRect *RectToRender) = 0;
	virtual void DrawDragBounds(DocRect *RectToRender);
	virtual void DrawDashLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint) = 0;
	virtual void DrawPixel(const DocCoord &Point) = 0;
	virtual void DrawBlob(DocCoord p, BlobType type) = 0;
	virtual void DrawCross(const DocCoord &Point, const UINT32 Size) = 0;

	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap) = 0;
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = 0) = 0;
	virtual BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	virtual void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape) = 0;
	virtual void DrawBitmapBlob(const DocCoord &Point, ResourceID resID ) = 0;

	virtual SlowJobResult DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
								  		   MaskedRenderRegion* pMask, ProgressDisplay *Progress);

	virtual void DrawFixedSystemText(StringBase *TheText, DocRect &BoundsRect, UINT32 uFormat = DEFAULT_TEXT_FORMATTING);
	virtual void SetFixedSystemTextColours(DocColour *TextCol, DocColour *Background);
	virtual void GetFixedSystemTextSize(StringBase *TheText, DocRect *BoundsRect, double* atDpi = NULL);

	DrawModeType DrawingMode;

	// Arrow head rendering
	virtual BOOL DrawPathArrowHeads(DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords);
	virtual BOOL DrawArrowHead(ArrowRec &ArrowToDraw, DocCoord &Centre, DocCoord &Direction,
		DocCoord * RetnCentre = NULL);

	// Functions for trying to draw reliable pixel lines and rectangles.
	// They actually draw using filled one pixel wide rectangles rather
	// than drawing single pixel lines
	virtual void DrawPixelRect(DocRect *RectToRender);
	virtual void DrawPixelLine(const DocCoord &StartPoint, const DocCoord &EndPoint);

	// Functions to Set the Attributes
	virtual void SetLineColour  (StrokeColourAttribute 	*, BOOL Temp);
	virtual void SetLineOverprint(OverprintLineAttrValue *, BOOL Temp);
	virtual void SetLineTransp  (StrokeTranspAttribute 	*, BOOL Temp);

	virtual void SetStrokeType   (StrokeTypeAttrValue *,	BOOL Temp);
	virtual void SetVariableWidth(VariableWidthAttrValue *,	BOOL Temp);

	virtual void SetFillGeometry(ColourFillAttribute	*, BOOL Temp);
	virtual void SetFillMapping (FillMappingAttribute	*, BOOL Temp);
	virtual void SetFillEffect  (FillEffectAttribute   	*, BOOL Temp);
	virtual void SetFillOverprint(OverprintFillAttrValue *, BOOL Temp);

	virtual void SetTranspFillGeometry(TranspFillAttribute *, BOOL Temp);
	virtual void SetTranspFillMapping (TranspFillMappingAttribute  *, BOOL Temp);

	virtual void SetPrintOnAllPlates(PrintOnAllPlatesAttrValue *, BOOL Temp);

	virtual void SetLineWidth   (LineWidthAttribute    	*, BOOL Temp);

	virtual void SetStartCap    (StartCapAttribute     	*, BOOL Temp);
	virtual void SetStartArrow  (StartArrowAttribute   	*, BOOL Temp);
	virtual void SetEndArrow    (EndArrowAttribute     	*, BOOL Temp);
	virtual void SetJoinType    (JoinTypeAttribute     	*, BOOL Temp);
	virtual void SetMitreLimit  (MitreLimitAttribute   	*, BOOL Temp);
	virtual void SetDashPattern (DashPatternAttribute  	*, BOOL Temp);
	virtual void SetWindingRule (WindingRuleAttribute  	*, BOOL Temp);
	virtual void SetDrawingMode (DrawingModeAttribute  	*, BOOL Temp);
	virtual void SetQuality	    (QualityAttribute 	  	*, BOOL Temp);

	virtual void SetWebAddress	    (WebAddressAttribute 	  	*, BOOL Temp);

	virtual void SetTxtFontTypeface		(TxtFontTypefaceAttribute	*, BOOL Temp);
	virtual void SetTxtBold				(TxtBoldAttribute			*, BOOL Temp);
	virtual void SetTxtUnderline		(TxtUnderlineAttribute		*, BOOL Temp);
	virtual void SetTxtItalic			(TxtItalicAttribute			*, BOOL Temp);
	virtual void SetTxtAspectRatio		(TxtAspectRatioAttribute	*, BOOL Temp);
	virtual void SetTxtJustification	(TxtJustificationAttribute	*, BOOL Temp);
	virtual void SetTxtTracking			(TxtTrackingAttribute		*, BOOL Temp);
	virtual void SetTxtFontSize			(TxtFontSizeAttribute		*, BOOL Temp);
	virtual void SetTxtScript			(TxtScriptAttribute			*, BOOL Temp);
	virtual void SetTxtBaseLine			(TxtBaseLineAttribute		*, BOOL Temp);
	virtual void SetTxtLineSpace		(TxtLineSpaceAttribute		*, BOOL Temp);

	virtual void SetSolidColours(BOOL SetSolid) {}

	void SetLineColour(DocColour &NewLineColour);
	void SetLineColour(StockColour NewLineColour);
	void SetFillColour(DocColour &NewFillColour);
	void SetFillColour(StockColour NewFillColour);
	void SetLineWidth(MILLIPOINT NewLineWidth);
	void SetStartCap(LineCapType NewStartCap);
	void SetStartArrow(StockArrow NewStartArrow);
	void SetEndArrow(StockArrow NewEndArrow);
	void SetStartArrow(ArrowRec &NewStartArrow);
	void SetEndArrow(ArrowRec &NewEndArrow);
	void SetJoinType(JointType NewJoinType);
	void SetMitreLimit(MILLIPOINT NewMitreLimit);
	void SetDashPattern(DashRec &NewDashPattern);
	void SetDashPattern(StockDash NewDashPattern);
	void SetWindingRule(WindingType NewWindingRule);
	void SetDrawingMode(DrawModeType NewDrawingType);
	void SetDefaultQuality();

	void SetDeviceDashPattern(DashRec &NewDashPattern);
	void SetDeviceDashPattern(StockDash NewDashPattern);

	virtual void SetOffscreen(OffscreenAttrValue* pAttr);
	virtual void SetClipRegion(ClipRegionAttribute	*, BOOL Temp);
	virtual CCachedBitmap GrabBitmap(DocRect rectGrab) {return CCachedBitmap();}

	// Functions to restore the attributes
	virtual void RestoreLineColour  (StrokeColourAttribute *, BOOL);
	virtual void RestoreLineOverprint(OverprintLineAttrValue *, BOOL);
	virtual void RestoreLineTransp  (StrokeTranspAttribute *, BOOL);

	virtual void RestoreStrokeType   (StrokeTypeAttrValue *,	BOOL);
	virtual void RestoreVariableWidth(VariableWidthAttrValue *,	BOOL);

	virtual void RestoreFillGeometry(ColourFillAttribute *, BOOL);
	virtual void RestoreFillMapping (FillMappingAttribute  *, BOOL);
	virtual void RestoreFillOverprint(OverprintFillAttrValue *, BOOL);

	virtual void RestoreTranspFillGeometry(TranspFillAttribute *, BOOL);
	virtual void RestoreTranspFillMapping (TranspFillMappingAttribute  *, BOOL);

	virtual void RestoreFillEffect  (FillEffectAttribute   *, BOOL);
	virtual void RestoreLineWidth   (LineWidthAttribute    *, BOOL);
	virtual void RestoreDrawingMode (DrawingModeAttribute  *, BOOL);
	virtual void RestoreWindingRule (WindingRuleAttribute  *, BOOL);
	virtual void RestoreStartCap    (StartCapAttribute 	   *, BOOL);
	virtual void RestoreJoinType    (JoinTypeAttribute 	   *, BOOL);
	virtual void RestoreQuality     (QualityAttribute 	   *, BOOL);
	virtual void RestoreDashPattern (DashPatternAttribute  *, BOOL);
	virtual void RestoreStartArrow  (StartArrowAttribute   *, BOOL);
	virtual void RestoreEndArrow    (EndArrowAttribute     *, BOOL);
	virtual void RestoreMitreLimit  (MitreLimitAttribute   *, BOOL);

	virtual void RestoreWebAddress  (WebAddressAttribute   *, BOOL);

	virtual void RestorePrintOnAllPlates(PrintOnAllPlatesAttrValue *, BOOL);

	void RestoreTxtFontTypeface		(TxtFontTypefaceAttribute	*, BOOL);
	void RestoreTxtBold				(TxtBoldAttribute			*, BOOL);
	void RestoreTxtUnderline		(TxtUnderlineAttribute		*, BOOL);
	void RestoreTxtItalic			(TxtItalicAttribute			*, BOOL);
	void RestoreTxtAspectRatio		(TxtAspectRatioAttribute	*, BOOL);
	void RestoreTxtJustification	(TxtJustificationAttribute	*, BOOL);
	void RestoreTxtTracking			(TxtTrackingAttribute		*, BOOL);
	void RestoreTxtFontSize			(TxtFontSizeAttribute		*, BOOL);
	void RestoreTxtScript			(TxtScriptAttribute			*, BOOL);
	void RestoreTxtBaseLine			(TxtBaseLineAttribute		*, BOOL);
	void RestoreTxtLineSpace		(TxtLineSpaceAttribute		*, BOOL);

	virtual void RestoreOffscreen(OffscreenAttrValue*);
	virtual void RestoreClipRegion	(ClipRegionAttribute	*, BOOL);

	AttributeValue *GetCurrentAttribute(UINT32 Index);
	OffscreenAttrValue* GetCurrentOffscreenAttr() { return(CurrentOffscreenAttr); }

	virtual BOOL CheckOffscreenBounds() { return(TRUE); }

	// RenderRegion clipping
	void 	SetClipRect(DocRect &NewClipRect, BOOL ReCalcContextNode = FALSE);
	DocRect GetClipRect();

	// Return the size of a Pixel in MILLIPOINTS
	MILLIPOINT GetPixelWidth() { return PixelWidth; }
	MILLIPOINT GetScaledPixelWidth() { return ScaledPixelWidth; }
	virtual double GetPixelsPerInch() { return 72000.0/ScaledPixelWidth; }

	// Function to allow render regions to say what they can not render
	virtual void GetRenderRegionCaps(RRCaps* pCaps);
	BOOL SetRenderComplexShapes(BOOL NewState);

	// Function to work out if we are trying to render a complex shape
	BOOL TestForComplexShape(RRCaps* pCaps);

	// Used to render graduated fills.
	EFFECTTYPE GetFillEffect();

	// Record connection to a filter using this render region
	void SetFilter(Filter* pFilter) {m_pFilter = pFilter;}
	Filter* GetFilter() {return m_pFilter;}

	// Called when a region changes in size
	virtual void ResizeRegion(DocRect &NewClipRect);

	// Functions to help with the Banded rendering
	virtual void ResetRegion(DocRect &NewClipRect);
	virtual BOOL SetFirstBand();
	virtual BOOL GetNextBand();
	DocRect GetRegionRect();
	BOOL IsBanded();
	BOOL CanBeMerged();
	BOOL ResetColourContext();
	ColourContext* GetColourContext();

	virtual void ResetRender();

	virtual BOOL RenderChar(WCHAR ch, Matrix* pMatrix);
	virtual BOOL WriteNewLine ( void );

	Path* CreateCharPath(WCHAR ch, Matrix* pMatrix=NULL);
	BOOL  GetAttrdCharBounds(DocRect* pRect, WCHAR ch);
	BOOL  GetCharAttributeMatrix(Matrix* pAttrMatrix);

	void ColourCorrectBitmap(BitmapFillAttribute* Fill, BITMAPINFO *Info, RGBQUAD **Palette);

	// Core tree rendering function now in RenderRegion!
	BOOL RenderTree(Node* pRoot,
					BOOL bTimeSlice = TRUE,
					BOOL bClip = TRUE,
					RenderCallback* pCallback = NULL);
	BOOL RenderTreeCanContinue();
	BOOL RenderTreeNoCache(Node* pRoot);

	virtual BOOL IsWrappedRender() const {return FALSE;}
	virtual BOOL TranspTypeIsRGBTCompatible(UINT32 ttype) const {return TRUE;}	// Should be TranspType ttype
	virtual BOOL SetSmoothingFlags(FillGeometryAttribute *Fill) {return TRUE;}

	// If this returns TRUE then paper nodes will be rendered via RenderTree
	virtual BOOL RenderPaperAsInk() { return(FALSE); }

//	virtual double GetTranspBoostM() {return m_dboostm;}
//	virtual double GetTranspBoostC() {return m_dboostc;}
//	virtual void SetTranspBoost(double dc = 0.0, double dm = 1.0) {m_dboostc = dc; m_dboostm = dm;}

	void SetImmediateRender(BOOL SetVal)	{RenderFlags.bImmediateRender = SetVal;}
	BOOL GetImmediateRender() const			{return RenderFlags.bImmediateRender;}

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
	virtual INT32 GetCaptureDepth()				{return m_CaptureStack.Size();}
	virtual BOOL MasterCaptureIsCurrent() const	{return (!m_CaptureStack.Empty() && GetTopCapture() && GetTopCapture()->IsMaster());}
	virtual Capture* GetTopCapture() const		{return m_CaptureStack.Empty() ? NULL : m_CaptureStack.Top();}
	virtual Capture* GetMasterCapture()	const	{return m_CaptureStack.Empty() ? NULL : m_CaptureStack.Bottom();}
	virtual BOOL LockedTransparentCaptures() const;

protected:
	virtual BOOL SetRenderToCapture(Capture* pCapture, BOOL ApplyClipRegion, BOOL bSetBitmap) {return TRUE;}

// State variables
protected:
	StackT<Capture*> m_CaptureStack;


	/////////////////////////////////////////////////////////////////////
	//
public:
	// Variables.
	BOOL IsWaitingForRAM;
	BOOL IsLastBand;

	// Flag to indicate whether this render region needs the PaperRenderRegion to
	// update the paper on screen.
	BOOL NeedsOSPaper;

	// Flag to indicate that the paper should be rendered on the next 'StartRender' call
	BOOL RenderOSPaper;

    // Flag used to tell if the paper or any of the ink has been rendered yet
	BOOL IsPaperRendered;
	BOOL IsInkRenderStarted;
	Quality RRQuality;							// used to control level of rendering

	// New Flag to say wether or not GDraw uses the biCompression field to do transparencies!
	BOOL m_DoCompression;

protected:
	DocColour				mBackgroundColour;
	StrokeColourAttribute	*PrepareLineColour(StrokeColourAttribute *SourceAttr = NULL);
	ColourFillAttribute		*PrepareFillGeometry(ColourFillAttribute *SourceAttr = NULL);
		// Internal helper functions for handling effects of overprint and print-on-all-plates on colours

	// Protected functions
	virtual void InitClipping() = 0;		// Initialises the Clipping rectangle
	virtual void InitAttributes() = 0;		// Initialises all Attributes
	virtual void SetLineAttributes(ChangeLineAttrType = CHANGELINEATTR_ALL);
	virtual void SetFillAttributes(ChangeAttrType Type = CHANGEATTR_ALL);
	virtual void SetTextAttributes(ChangeFillAttrType Type = CHANGEFILLATTR_ALL);
	virtual BOOL RenderNode(Node* pNode, Node* pRunToNode, RenderCallback* pCallback);

	virtual void SetOSDrawingMode() = 0;

	virtual void SetQualityLevel();	 // Made non-pure by Markn 6/8/97, because base class can now get called

	virtual MILLIPOINT CalcPixelWidth() = 0;
	virtual MILLIPOINT CalcScaledPixelWidth() = 0;

	// Functions to deal with the Fuzzy Clipping we perform at High Zoom factors
	BOOL TryToFuzzyClip(Path* pPath, DocCoord** pNewCoords, PathVerb** pNewVerbs, UINT32 *NewNumCoords);
	void SetupFuzzyClipRects();

	// The Fuzzy Clipping Rects used in Fuzzy Clipping.
	DocRect InnerRect;
	DocRect OuterRect;

	// BOOL to help things descide if they need to render complex shapes or not
	BOOL RenderComplexShapes;

	// Timing values to help optimise cacheing
	INT64 m_countTotal;
	INT64 m_countStored;

	struct CCAPI FlagBlock
	{
		BOOL Printing 			: 1;
		BOOL Rendering 			: 1;
		BOOL StackClaimed 		: 1;
		BOOL ValidDevice		: 1;
		BOOL bImmediateRender 	: 1;
		BOOL VeryMono   		: 1;		// TRUE if very black&white (i.e. no dithering)
		BOOL SmoothedBitmaps	: 1;
		BOOL HitDetect			: 1;
    }	RenderFlags;

	struct _StrokeFlags
	{
		BOOL ValidStrokeColour	: 1;
		BOOL ValidStrokeTransp	: 1;
		BOOL ValidLineWidth		: 1;
		BOOL ValidJoinType		: 1;
		BOOL ValidDashPattern	: 1;
		BOOL ValidLineCap		: 1;
		BOOL ValidStartArrow	: 1;
		BOOL ValidEndArrow		: 1;
		BOOL ValidMitreLimit	: 1;
	} StrokeFlags;

	struct _FillFlags
	{
		BOOL ValidGeometry		: 1;
		BOOL ValidEffect		: 1;
		BOOL ValidMapping		: 1;
		BOOL ValidTransGeometry	: 1;
		BOOL ValidTransEffect	: 1;
		BOOL ValidTransMapping	: 1;
		BOOL ValidWindingRule	: 1;
	} FillFlags;
	
	struct _TextFlags
	{
		BOOL ValidFont			: 1;
		BOOL ValidBold			: 1;
		BOOL ValidItalic		: 1;
		BOOL ValidUnderline		: 1;
		BOOL ValidAspect		: 1;
		BOOL ValidTracking		: 1;
		BOOL ValidFontSize		: 1;
		BOOL ValidJustify		: 1;
		BOOL ValidScript		: 1;
		BOOL ValidBaseLine		: 1;
		BOOL ValidLineSpace		: 1;

	} TextFlags;
	
//	wxDC* DCHandle;				// This is used to recreate a CDC during BackGrnd redraw
	Matrix RenderMatrix;		// The current Transform Matrix	
	DocRect CurrentClipRect;	// The current Clipping rectangle (in DocCoords)
	ColourContext *CurrentColContext;	// The current ouptut colour context

	View			   *RenderView;		// The kernel view that this RenderRegion is rendering to
	CNativeDC		   *RenderDC;		// The DC that we are rendering into
	Spread			   *RenderSpread;	// The Spread that we are rendering into
	bool				m_fOwned;

	Node* 				CurrentRenderState;
	SubRenderContext*	CurrentSubRenderState;
	BOOL				SubRenderStateLocked;

	FIXED16 ScaleFactor;	// The scale factor of the view (1 is 100%)

	// Pointer to our current attributes
	AttributeEntry *CurrentAttrs;
	INT32 NumCurrentAttrs;

	// Ilan
	// Space for current offscreen attribute
	// Allows offscreen attribute derived classes to modify CurrentAttrs table
	// as well as render offscreen
	OffscreenAttrValue* CurrentOffscreenAttr;

	BOOL CopyCurrentAttributes(const RenderRegion &);

	// timeslice in milliseconds to be used in BG rendering and controlling vars
	INT32			Timeslice;
	BOOL			m_bRenderTreeTimeSlice;
	MonotonicTime	m_timeRenderTreeStartTime;

	// Static vars
	// The sizes of the Selection blobs (these are saved out as preferences)
	static UINT32 SelectedBlobSize;
	static UINT32 UnSelectedBlobSize;
	static UINT32 MultiStageSelectedBlobSize;
	static UINT32 MultiStageUnSelectedBlobSize;

	// The ClipView blob size is not saved out as a preference, as it refers to a
	// (fixed-width) bitmap blob.
	static UINT32 ClipViewBlobSize;

	// Size of pixels
	MILLIPOINT PixelWidth;
	MILLIPOINT ScaledPixelWidth;

	RenderStack TheStack;

	// Some vars used by the banded rendering stuff
	DocRect RegionRect;
	BOOL 	IsRegionBanded;
	BOOL	CanRegionBeMerged;

	// This pointer tells code using this region what the backmost changed object was
	// that caused this Region to be generated (NULL if nothing changed)
	// Dumb routines can test for non-NULL to throw away cached info
	// Smart routines can test whether they are related to this object in various
	// ways before deciding to bin cached data (e.g. IsUnder)
	Node*	m_pBackmostChangedNode;

	// Ilan
	// set to TRUE to indicate to AttrValues, during the unwinding of the attribute context
	// stack, that the RR is being ResetRegion()d
	// set to FALSE otherwise (ie in the normal process of traversing the tree during rendering)
	BOOL	IsBeingReset;

	// Used to record a connection to a filter
	Filter* m_pFilter;

//	double m_dboostm;
//	double m_dboostc;

private:
	PathProcessor *PathProcessorStack;		// A stack of currently active path processors 
};


// Macros to get at the basic attributes quickly...

#define RR_STROKECOLOUR() (((StrokeColourAttribute *) CurrentAttrs[ATTR_STROKECOLOUR].pAttr)->Colour)
#define RR_FILLCOLOUR()   (((ColourFillAttribute   *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr)->Colour)
#define RR_LINEWIDTH()    (((LineWidthAttribute    *) CurrentAttrs[ATTR_LINEWIDTH   ].pAttr)->LineWidth)
#define RR_WINDINGRULE()  (((WindingRuleAttribute  *) CurrentAttrs[ATTR_WINDINGRULE ].pAttr)->WindingRule)
#define RR_STARTCAP()     (((StartCapAttribute     *) CurrentAttrs[ATTR_STARTCAP    ].pAttr)->StartCap)
#define RR_JOINTYPE()     (((JoinTypeAttribute     *) CurrentAttrs[ATTR_JOINTYPE    ].pAttr)->JoinType)
#define RR_QUALITY()      (((QualityAttribute      *) CurrentAttrs[ATTR_QUALITY     ].pAttr)->Quality)
#define RR_STARTARROW()   (((StartArrowAttribute   *) CurrentAttrs[ATTR_STARTARROW  ].pAttr)->StartArrow)
#define RR_ENDARROW()     (((EndArrowAttribute     *) CurrentAttrs[ATTR_ENDARROW    ].pAttr)->EndArrow)
#define RR_MITRELIMIT()   (((MitreLimitAttribute   *) CurrentAttrs[ATTR_MITRELIMIT  ].pAttr)->MitreLimit)
#define RR_DASHPATTERN()  (((DashPatternAttribute  *) CurrentAttrs[ATTR_DASHPATTERN ].pAttr)->DashPattern)

#define RR_FILLTRANSP()   		((TranspFillAttribute*)CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr)
#define RR_FILLEFFECT()   		((FillEffectAttribute*)CurrentAttrs[ATTR_FILLEFFECT].pAttr)
#define RR_FILLMAPPING()  		((FillMappingAttribute*)CurrentAttrs[ATTR_FILLMAPPING].pAttr)
#define RR_TRANSPFILLMAPPING()  ((FillTranspMappingAttribute*)CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr)
#define RR_STROKETRANSP() 		((StrokeTranspAttribute*)CurrentAttrs[ATTR_STROKETRANSP].pAttr)

#define RR_TXTFONTTYPEFACE()  	(((TxtFontTypefaceAttribute  *) CurrentAttrs[ATTR_TXTFONTTYPEFACE ].pAttr)->HTypeface)

#define RR_TXTBOLD()  			(((TxtBoldAttribute  		*) CurrentAttrs[ATTR_TXTBOLD ].pAttr)->BoldOn \
								||((TxtFontTypefaceAttribute *) CurrentAttrs[ATTR_TXTFONTTYPEFACE ].pAttr)->IsBold)

#define RR_TXTITALIC()  		(((TxtItalicAttribute  		*) CurrentAttrs[ATTR_TXTITALIC ].pAttr)->ItalicOn \
								||((TxtFontTypefaceAttribute *) CurrentAttrs[ATTR_TXTFONTTYPEFACE ].pAttr)->IsItalic)

#define RR_TXTASPECTRATIO()  	(((TxtAspectRatioAttribute  	*) CurrentAttrs[ATTR_TXTASPECTRATIO ].pAttr)->AspectRatio)
#define RR_TXTJUSTIFICATION()  	(((TxtJustificationAttribute *) CurrentAttrs[ATTR_TXTJUSTIFICATION ].pAttr)->justification)
#define RR_TXTTRACKING()  		(((TxtTrackingAttribute  		*) CurrentAttrs[ATTR_TXTTRACKING ].pAttr)->Tracking)
#define RR_TXTUNDERLINE()  		(((TxtUnderlineAttribute  		*) CurrentAttrs[ATTR_TXTUNDERLINE ].pAttr)->Underlined)
#define RR_TXTFONTSIZE()  		(((TxtFontSizeAttribute 		*) CurrentAttrs[ATTR_TXTFONTSIZE ].pAttr)->FontSize)
#define RR_TXTSCRIPT()  		((TxtScriptAttribute 		*) CurrentAttrs[ATTR_TXTSCRIPT ].pAttr)
#define RR_TXTBASELINE()  		(((TxtBaseLineAttribute 		*) CurrentAttrs[ATTR_TXTBASELINE ].pAttr)->Value)
#define RR_TXTLINESPACE()  		(((TxtLineSpaceAttribute 		*) CurrentAttrs[ATTR_TXTLINESPACE ].pAttr)->Value)

#define RR_CLIPREGION()			((ClipRegionAttribute*)CurrentAttrs[ATTR_CLIPREGION].pAttr)




/********************************************************************************************

>	class RenderCallback : public CCObject

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/2004
	Purpose:	Provide a base class for handling callbacks from RenderTree
	Notes:		RenderTree function calls the BeforeNode function in this class when it's about
				to render any Node.
				Derive from this class and implement Callback to provide specialised
				processing of nodes during rendering
				Derived versions of Callback can:
					* Prevent the node being rendered
					* Set attributes
					* Set subrender contexts
					* Accumulate their own info about the render sequence

				BeforeSubtree can be used to control navigation around the tree
				AfterSubtree is there for completeness

********************************************************************************************/

class RenderCallback : public CCObject
{
public:
	virtual BOOL BeforeNode(	RenderRegion* pRegion,
								Node* pNode)					{return TRUE;}

	virtual BOOL BeforeSubtree(	RenderRegion* pRegion,
								Node* pNode,
								Node** ppNextNode,
								BOOL bClip,
								SubtreeRenderState* pState)		{return FALSE;}

	virtual BOOL AfterSubtree(	RenderRegion* pRegion,
								Node* pNode)					{return FALSE;}
};


#endif
