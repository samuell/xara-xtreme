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

// Spread class

#ifndef INC_SPREAD
#define	INC_SPREAD

#include "npaper.h"
#include "convert.h"
#include "grid.h"
#include "dibconv.h"
#include "paldefs.h"
#include "animparams.h"


// NEW_PASTEBOARD compile switch
// =============================
// Set this value to 1 to enable Jason's new pasteboard code. This should work fine, except
// that load/save will sometimes get quickshapes or text in the wrong place. D'oh!
// Note that I've set it to fail to compile until you've been made aware of the problem!

#define NEW_PASTEBOARD 0


/***********************************************************************************************

>	class Spread : public NodeRenderablePaper

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/93
	Purpose:	Spreads group together those pages which appear beside each other in the final
				document. 
				
	SeeAlso:	NodeRenderablePaper; Chapter; Page; Layer   

***********************************************************************************************/

class Layer; 
class Page;
class View;

class Spread: public NodeRenderablePaper
{         
	CC_DECLARE_DYNAMIC( Spread )   

public:			// Construction
	Spread();       
		
	Spread(Node* ContextNode,
		   AttachNodeDirection Direction,	
		   const DocRect& PasteRect,
		   MILLIPOINT BleedOffset = 0, 
		   BOOL Locked=FALSE,
		   BOOL Mangled=FALSE,
		   BOOL Marked=FALSE,
		   BOOL Selected=FALSE
		  );


public:			// Spread interface
	virtual DocRect GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	DocRect	GetDrawingSize() const;

	String 	Describe(BOOL Plural, BOOL Verbose);                               

	Spread* FindNextSpread();
	Spread* FindPreviousSpread();

	Page* 	FindFirstPageInSpread() const;
	Page*	FindLastPageInSpread() const;

	Document* FindParentDocument();

	Chapter* FindParentChapter();

	Layer*	FindActiveLayer();
	Layer* 	FindFirstLayer() const; 
	Layer* 	FindLastLayer() const; 
	Layer* 	FindFirstGuideLayer() const;
	Layer*	FindFirstPageBackgroundLayer() const;
	Layer*	FindFirstFrameLayer() const;
	Layer*	FindLastFrameLayer() const;

	DimScale* 	GetPtrDimScale();

	MILLIPOINT GetBleedOffset() const;			// Returns the current bleed offset
	BOOL SetBleedOffset(MILLIPOINT Bleed);		// Sets a new bleed offset 

	BOOL GetShowDropShadow() const;				// Returns the current drop shadow display state
	BOOL SetShowDropShadow(BOOL NewState);		// Sets a new current drop shadow display state

	virtual BOOL IsSpread() const;
	virtual SubtreeRenderState RenderSubtree(RenderRegion* pRender, Node** ppNextNode = NULL, BOOL bClip = TRUE);
	BOOL NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly = FALSE, BOOL CheckSelected = FALSE);

	void Render(RenderRegion* pRender);
      
#ifdef _DEBUG
    void ShowDebugTreeDetails() const;
#endif

    virtual UINT32 GetNodeSize() const;				// Returns size of node in bytes

	virtual void GetDebugDetails(StringBase* Str);	// This is used by the Debug Tree dialog
													// It will probably be deleted when we ship (!!!)

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);

protected:
	// Functions to write the actual data out
	virtual BOOL WriteSpreadInformation(BaseCamelotFilter* pFilter);
	virtual BOOL WriteSpreadScaling(BaseCamelotFilter* pFilter);


public:			// Conversions between SpreadCoords and User/Page/Document Coords
	DocCoord GetUserOrigin();
	void SetUserOrigin(DocCoord origin) { UserOrigin=origin; }

	void SpreadCoordToDocCoord(DocCoord	*pSpreadCoord) const;
	void SpreadCoordToDocCoord(DocRect	*pSpreadCoord) const;
	void DocCoordToSpreadCoord(DocCoord	*pDocCoord) const;
	void DocCoordToSpreadCoord(DocRect	*pDocCoord) const;
			// Convert between Document and Spread coordinate space

	BOOL SpreadCoordToPagesCoord(DocCoord *pPagesCoord,  DocCoord SpreadCoord);
	BOOL PagesCoordToSpreadCoord(DocCoord *pSpreadCoord, DocCoord PagesCoord);
			// Convert between Page and Spread coordinate space
			// (Page coords are relative to the bottom left corner of the bounding rect of all pages)

	BOOL GetPagesRect(DocRect* pPagesRect);

	BOOL TextToSpreadCoord(DocCoord* pDocCoord, StringBase* pxText, StringBase* pyText);
	BOOL SpreadCoordToText(String_256* pxText, String_256* pyText, DocCoord DocCoord,
							BOOL xUnitSpecifier=TRUE, BOOL yUnitSpecifier=TRUE, INT32 dp=-1);

	BOOL GetDecimalPlacesForPixelResolution(View *pView, INT32* dp);


public:			// Grid related interface
	NodeGrid* FindFirstDefaultGridInSpread();

public:
	BOOL CreateDefaultPageAndGrid(BOOL CreateGrid = TRUE);	// Sets up the spread so that it contains the default page & grid objects

public:			// Page related interface
	DocRect GetPageBounds() const;			// The rectangle enclosing the spread's pages
	DocRect GetPageVisibleBounds() const;	// Same as above, but excludes invisible layers

	BOOL GetPageSize(MILLIPOINT *Width, MILLIPOINT *Height,
					 MILLIPOINT *Margin, MILLIPOINT *Bleed,
					 BOOL *Dps, BOOL *ShowDropShadow);
			// Retrieves the page size of all pages in the spread, as well as other page info

	BOOL SetPageSize(const MILLIPOINT Width, const MILLIPOINT Height,
					 const MILLIPOINT Margin, const MILLIPOINT Bleed,
					 const BOOL Dps, const BOOL ShowDropShadow);
			// Configures the spread's pages - sets their size (all pages must be the same size)
			// a margin of pasteboard area around the outside, single/double page spread, and
			// whether the pages should have a drop shadow.

protected:
#if NEW_PASTEBOARD
	BOOL AdjustAllPages(const MILLIPOINT Width, const MILLIPOINT Height,
						const MILLIPOINT Margin, const BOOL Dps);
			// Ensures all pages are the given size, and butt up against each other properly
			// Also creates/deletes pages as necessary to achieve single/double page spread
			// (Equivalent of SetSizeOfAllPages)
#else
	BOOL SetSizeOfAllPages(const MILLIPOINT Width, const MILLIPOINT Height,
							const MILLIPOINT Margin, const BOOL Dps);
			// Ensures all pages are the given size, and butt up against each other properly
			// Also creates/deletes pages as necessary to achieve single/double page spread
			// (Equivalent of AdjustAllPages)

	BOOL AlignPasteboards(const DocRect NewPasteRect, const MILLIPOINT Margin);
			// Ensures that all pasteboards in the CHAPTER are large enough to encompass
			// all objects on the spreads.
#endif


public:			// Pasteboard related interface (see also the base class)
	DocRect GetWidePasteboard(View *pView);
			// Gets the wide pasteboard, which is: the area visible in the View or the
			// current pasteboard rectangle, whichever is the smaller.

	DocCoord GetSpreadCoordOrigin(BOOL Pixelise = TRUE, View *pView = NULL) const;
			// Retrieves the Document-Coordinate offset where the spread coord origin lies
			// (Add this origin to spread coords to convert to doc coords)

	void SetSpreadCoordOrigin(DocCoord NewOrigin);
			// Sets the spread coord origin. This effectively translates enclosed
			// pages/objects in the opposite direction by the amount the origin has moved.
			// Use with care! (talk to Jason)

	DocCoord GetMaxPasteboardSize(void);
			// Gets the maximum x,y extents to which the pasteboard can expand

	BOOL ExpandPasteboardToInclude(DocRect IncludeRect);
			// Expand (if possible) the pasteboard to include the given rectangle.
			// Returns FALSE if IncludeRect could not be included in the new pasteboard bounds

	void AdjustPasteboards(void);
			// Ensures that all pasteboards from this spread onwards are moved so that
			// there is no overlap, and also ensures that all default grids cover the
			// resulting new pasteboard bounds. Also makes sure that each pasteboard
			// is large enough to encompass all the objects contained in the spread.

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

protected:		// Overridden base class interfaces
	virtual void SetInitialPasteboardRect(const DocRect& PasteRect);
			// When setting the initial rect, also now sets the Spread Coord Origin

	virtual BOOL PostImport();
			// Called after import of this document. The spread resizes its pasteboard automatically


protected:		// Internal routines
	void RenderPasteboard(RenderRegion* pRender);
	void RenderBleedArea(RenderRegion* pRender);
   	virtual Node* SimpleCopy();
	void CopyNodeContents(Spread* NodeCopy);
   	void RenderDropShadows(RenderRegion* pRender);


protected:		// Internal variables
   	MILLIPOINT BleedOffset;
	BOOL ShowDropShadow;
	BOOL RalphDontShowPaper;
	DocCoord SpreadOrigin;		// The origin of the spread coordinate space (in document coords)
	DocCoord UserOrigin;		// The origin of user (displayed on screen) coordinates (ruler origin)
	AnimPropertiesParam		m_AnimPropertiesParam;
								// The following values for a GIF animation will be stored in a spread.
								// Animation loop	
								// Global Delay
								// Dither
								// Palette	
								// Number of Cols In Palette
								// Use System Colours flag
	
public:
	// Selector functions.
	
	BOOL SetSpreadAnimPropertiesParam(const DWORD &Loop, const DWORD &GlobalDelay, const DITHER &Dither, 
										const WEB_PALETTE &WebPalette, const PALETTE_COLOURS &ColoursPalette,
										const DWORD &NumColsInPalette, const BOOL& UseSystemColours,
										const BOOL& IsBackgroundTransparent);



	BOOL GetSpreadAnimPropertiesParam(DWORD *Loop, DWORD *GlobalDelay, DITHER *Dither, 
										WEB_PALETTE *WebPalette, PALETTE_COLOURS* ColoursPalette,
										DWORD* NumColsInPalette, BOOL* UseSystemColours,
										BOOL* pIsBgTransparent);

	AnimPropertiesParam& GetSpreadAnimPropertiesParam();
	void SetSpreadAnimPropertiesParam(const AnimPropertiesParam& Param);
	void SetAnimationDelay(const DWORD GlobalDelay);
	void SetAnimationLoop(const DWORD &Loop);

	void SetAnimationColours(const DITHER& Dither, const WEB_PALETTE& WebPalette,
							 const PALETTE_COLOURS& ColoursPalette, const DWORD& NumColsInPalette,
							 const BOOL& IsBackgroundTransparent);
	void GetAnimationColours(DITHER * pDither, WEB_PALETTE * pWebPalette, 
							 PALETTE_COLOURS * pColoursPalette, DWORD * pNumColsInPalette,
							 BOOL * pIsBackgroundTransparent);
		
	virtual BOOL WriteSpreadAnimProperties(BaseCamelotFilter* pFilter);

	// The bounding rectangle for the animation
	DocRect GetAnimationBoundingRect();
	void SetAnimationBoundingRect(const DocRect& BoundingRect);

	// The quality level that the animation was captured at
	Quality GetAnimationQuality();
	void SetAnimationQuality(const Quality& NewQuality);

//#endif //debug

private:
	DimScale SpreadDimScale;
};   

#endif
