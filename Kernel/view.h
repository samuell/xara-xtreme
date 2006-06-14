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

// Base class for kernel View objects.

#ifndef INC_VIEW
#define INC_VIEW

#include "listitem.h"
#include "quality.h"
#include "matrix.h"
#include "docrect.h"
#include "rendtype.h"
#include "oilrect.h"
#include "rndrgn.h"
#include "colcarry.h"
#include "colmodel.h"
//#include "viewbitmapcache.h"

class CCamView;
class ViewState;
class Document;
class wxDC;
class RenderRegion;
class ScanningRenderRegion;
class Spread;
class PaperRenderRegion;
class Node;
class PrintControl;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
class CCPrintInfo;
#endif //webster
class ColourPlate;


/********************************************************************************************

>	class ProgressInfo

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/25/95
	Purpose:	Encapsulates various bits of information required to provide a progress
				display while printing/exporting via View::RenderSimpleView().
	SeeAlso:	View::RenderSimpleView

********************************************************************************************/

class ProgressDisplay
{
public:
	ProgressDisplay();

	// Functions to do the normal progress display (eekk)
	void SetUp(RenderRegion *pRender, ScanningRenderRegion* pScanner);
	BOOL IncProgress(INT32 NumNodes = 1);
	BOOL FirstStageDone();
	BOOL SecondStageDone();
	void AllStagesDone();
	void StartBitmapPhase(INT32 NumBands);
	void StartBitmapPhaseBand(INT32 TotalNumScanlines);
	BOOL BitmapPhaseBandRenderedTo(INT32 ScanlinesRendered);
	BOOL EndBitmapPhaseBand();

	// Functions to deal with the Optimal Render View function
	// this is much simpler, so it gets its own functions.
	void SetUpOptimal(RenderRegion *pRender, ScanningRenderRegion* pScanner);
	BOOL SetNodesRendered(INT32 NumNodes);

protected:
	// General variables
	BOOL DoProgressDisplay;
	BOOL IsPrinting;
	INT32 NumNodesRendered;
	INT32 LastProgressUpdate;

	// 'Normal' node rendering.
	INT32 ProgressInterval;
	INT32 FirstStageCount;
	INT32 SecondStageCount;
	INT32 ThirdStageCount;

	// Banded bitmap rendering
	INT32 BandSize;
	INT32 BandHeight;
	INT32 BandOffset;
	INT32 BandIncrement;

	// Optimal Rendering vars
	INT32 TotalNodes;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	// Used to update print progress (if printing)
	CCPrintInfo *pPrintInfo;
#endif //webster
	// How much we have scaled up the node count by to get a decent progress resolution.
	INT32 ProgressScaleFactor;
};

/********************************************************************************************

< RenderViewResult

	Comment:	This is used to indicate the outcome of calling View::RenderSimpleView.
	SeeAlso:	View::RenderSimpleView

********************************************************************************************/ 

enum RenderViewResult		
{ 
	RENDERVIEW_SUCCESS,
	RENDERVIEW_NOTNEEDED,
	RENDERVIEW_FAILURE, 
	RENDERVIEW_USERABORT 
};



/********************************************************************************************

>	class View : public ListItem

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/07/95
	Purpose:	The basic kernel view item.  This class contains minimal functionality to
				maintain the 'current' view, and the connection from a kernel view
				object to an OIL view object.
				DocView and PrintView are derived from this class.
	SeeAlso:	DocView; PrintView

********************************************************************************************/

class View : public ListItem
{
	CC_DECLARE_DYNAMIC(View);

public:
	View();
	virtual ~View();
	static void Deinit();

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Interface to the OIL layer CCamView and the kernel Document.

protected:
	CCamView*  			pViewWindow;			// Pointer to the associated CCamView
	Document*  			pDoc;					// Pointer to owner document.
	ViewState* 			pVState;				// Device independent state of the view.

public:
	BOOL ConnectToOilView(CCamView*);			// Set up the link to the CCamView object
	
	Document* GetDoc() const;					// Returns document associated with this
	CCamView* GetConnectionToOilView() const;	// Which CCamView are we linked to
	PrintControl *GetPrintControl();			// Get print control for our document.

	virtual void SetViewState(ViewState*);
	virtual BOOL ViewStateChanged() = 0;		// informs us that ViewState has changed


/////////////////////////////////////////////////////////////////////////////////////////////////
//	"Current" and "Selected" Views

protected:
	static View* Current;						// Global which records Current View

public:
	static View* GetCurrent();					// Returns which View is current
	BOOL SetCurrent();							// Make this View current
	static void	SetNoCurrent();					// Make NO View current

	virtual void SetViewPixelSize() = 0;		// Set pixel size according to this view.

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Rendering

public:
	void OnDraw(wxDC*, OilRect);
	virtual void ContinueRenderView(RenderRegion*, Spread*,
									BOOL fRenderPaper = TRUE,
									BOOL fDeleteRegionAfter = TRUE,
									BOOL bForceImmediate = FALSE) = 0;
	virtual wxDC* GetRenderDC() = 0;
	virtual void DoneWithDC();
	virtual BOOL RenderTreeCallback(Node* pNode, RenderRegion* pRender) {return TRUE;}

	Quality RenderQuality;						// WYSIWYG value (only freinds of Quality can
												// get/set numeric value

	// Functions to render complex shapes into simple render regions
	RenderViewResult RenderOptimalView(RenderRegion*r, Matrix&, Spread*, BOOL);
	SlowJobResult RenderOptimalBitmapPhase(DocRect& ClipRect, Matrix&, Spread*, RenderRegion*, Node*, Node*,
						   	BOOL RenderAllObjects, BOOL bPrintPaper, INT32&, ProgressDisplay* pProgress, INT32 TotalProgress);

	virtual RenderViewResult RenderSimpleView(RenderRegion*, Matrix&, Spread*, BOOL);
	SlowJobResult RenderBitmapPhase(DocRect& ClipRect, Matrix&, Spread*, RenderRegion*, Node*, Node*,
						   			BOOL bRenderAll, BOOL bPrintPaper, ProgressDisplay& Progress);

	static void DeInitOnTopRegion();
	static BOOL IsPrintableNodeSelected(Node *);

	virtual void RenderPaper(Spread*, DocRect, wxDC*, Matrix&);
	virtual BOOL RenderPageMarks(RenderRegion*, Matrix&, DocRect&, Spread*);

protected:
	virtual void RenderPaper(RenderRegion*, Spread*);

	SlowJobResult RenderSimpleNodes(Node *pNode, RenderRegion *pRender,
						   ProgressDisplay& Progress, Node *pLastComplexNode = NULL);
	SlowJobResult RenderSimpleNodesUnclipped(Node *pNode, RenderRegion *pRender,
						   ProgressDisplay& Progress, Node *pLastComplexNode = NULL);

private:
	static PaperRenderRegion* pPaperRegion;		// Region to use when rendering paper only
												// (i.e. only for screen views)

	static PaperRenderRegion* pOnTopRegion;		// Region to use when rendering blobs only
												// (i.e. only for screen views)

//	ViewBitmapCache m_CachedBitmaps;			// Collection of bitmaps for rendering optimisation

public:
	virtual BOOL GetForeBackMode() = 0;
	virtual void SetForeBackMode(BOOL) = 0;

	virtual Matrix ConstructRenderingMatrix(Spread *pSpread);
	// Ilan 06/06/00
	virtual Matrix ConstructScaledRenderingMatrix(Spread *pSpread, double ScaleFactor);

protected:
	RenderRegion *NewRenderRegion(DocRect InvalidRect, Matrix& ViewMatrix,
									wxDC* pDevContext, Spread* pSpread, RenderType rType,
									bool fOwned = false );
	
	// Function to build and control the render regions
	virtual void MakeNewRenderRegion(Spread *, DocRect, wxDC *, RenderType, BOOL PaintPaper = FALSE, Node* pInvalidNode = NULL);

public:
	// Get/Set the size of OIL pixels that this view uses.
	FIXED16 GetPixelWidth();
	FIXED16 GetPixelHeight();
	void GetPixelSize(FIXED16 *PixelWidth, FIXED16 *PixelHeight);
	virtual void SetPixelSize(FIXED16 PixelWidth, FIXED16 PixelHeight);

	FIXED16 GetScaledPixelWidth();
	FIXED16 GetScaledPixelHeight();
	void GetScaledPixelSize(FIXED16 *PixelWidth, FIXED16 *PixelHeight);
	virtual void SetScaledPixelSize(FIXED16 PixelWidth, FIXED16 PixelHeight);

	virtual double GetConvertToEditableShapesDPI();

protected:
	// Actual size of OIL pixels that this view uses.
	FIXED16 PixelWidth,
			PixelHeight;

	// Scaled size of OIL pixels that this view uses.
	FIXED16 ScaledPixelWidth,
			ScaledPixelHeight;


public:		// Colour contexts
	ColourContext *GetColourContext(ColourModel Model, BOOL ReturnNULLIfNone = FALSE);
			// Find the view's context for the given colour model

	ColourPlate *GetColourPlate(void);
			// Find out what ColourPlate separation (if any) this view is using

	void SetColourPlate(ColourPlate *NewPlate, BOOL bSendContextChanged = TRUE);
			// Set the colour separation mode that will be used in all future output
			// from this view

	void SetColourContext(ColourModel Model, ColourContext *NewContext = NULL);
			// Set or clear the special colour context for a given colour model.
			// The View will delete the context when it is finished with it, or when
			// you call this function again with another context or a NULL pointer.

	BOOL GetForceDefaultColourContexts(void) { return(ForceDefaultColourContexts); }
	
	BOOL SetForceDefaultColourContexts(BOOL bForceDefault = TRUE)
	{
		BOOL bOldForce = ForceDefaultColourContexts;
		ForceDefaultColourContexts = bForceDefault;
		return(bOldForce);
	}

protected:
	ColourPlate *ColPlate;					// NULL, or Current colour separation options
	ColourContextArray ColourContexts;		// Cached colour contexts using the ColourPlate
	BOOL ShouldDeleteContext[16];			// Flags for each cached context

	BOOL ForceDefaultColourContexts;		// Internal flag
											// if TRUE, forces GetColourContext to return global
											// default colour contexts rather than special ones.
											// Used by RenderOptimalBitmapPhase to disable colour
											// separation/correction in masked bitmap rendering.


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Scrolling, scaling and extents

protected:
	FIXED16 	Scale;								// User viewing/printing scale factor.
	DocRect		PhysExtent;							// Size of a document (bang!)

public:
	WorkCoord GetScrollOffsets() const;
	virtual BOOL SetScrollOffsets(WorkCoord, BOOL RedrawNeeded = TRUE);

	FIXED16 GetViewScale() const;					// ? the user scale in this view
	virtual BOOL SetViewScale(FIXED16);				// Set the user scale factor

	virtual DocRect GetDocViewRect(Spread*) = 0;
	virtual void SetExtent(DocCoord, DocCoord) = 0;	// Reset the extent of the document
	virtual WorkRect GetViewRect() = 0;				// The physical rect of the view


/////////////////////////////////////////////////////////////////////////////////////////////////
//	Dragging

protected:
	BOOL		m_bSolidDrag;						// Flag indicating that current drag is solid

};




/********************************************************************************************

>	class CountView : Public View

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/06/2004
	Purpose:	For counting "rendered" objects without actually rendering them
	SeeAlso:	View

********************************************************************************************/

/*class CountView : Public View
{
	virtual BOOL RenderSubtreeCallback(Node* pNode, RenderRegion* pRender);
};
*/


class ScopedFlagSetter
{
public:
	ScopedFlagSetter(BOOL& bTarget, BOOL bNewVal)
	{
		// Remeber a pointer to the target and the original value
		m_pbTarget = &bTarget;
		m_bOldVal = bTarget;
		// And set the target to the new value
		bTarget = bNewVal;
	}

	~ScopedFlagSetter()
	{
		// Reset the target to the original value
		*m_pbTarget = m_bOldVal;
	}
	BOOL m_bOldVal;
	BOOL* m_pbTarget;
};



#endif  // INC_VIEW
