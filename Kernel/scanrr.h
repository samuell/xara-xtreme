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
// The Scanning Render region class

#ifndef INC_SCANRR
#define INC_SCANRR

#include "rndrgn.h"
#include "rrcaps.h"

/********************************************************************************************

>	class NodeListItemWithDocPtr : public NodeListItem

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/7/2000
	Purpose:	This is used to ensure that blends print correctly.  If a particular
				NodeBlender was classified as simple, and is later used with a blend
				that is complex, then complex blend steps will not render.  This class
				allows us to reclassify a simple node as a complex one.
	SeeAlso:	-

********************************************************************************************/

class NodeListItemWithComplexity : public NodeListItem
{
public:
	CC_DECLARE_DYNAMIC( NodeListItemWithComplexity )
	NodeListItemWithComplexity();							// will contain a NULL pointer
	NodeListItemWithComplexity(Node* WhichNode, BOOL val);	// initialise pNode to be WhichNode
	~NodeListItemWithComplexity() {}						// Nothing to destruct
	
	BOOL wasClassifiedSimple;						// was the node classified as simple?
	INT32 RunLength;
	DocRect BoundsRect;
};

/********************************************************************************************

>	class ScanningRenderRegion : public RenderRegion

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Purpose:	The Scanning Render Region is a special sort of render region that simply
				scans the document and works out where abouts there are complex shapes that
				can not be rendered by a normal OSRenderRegion. Complex shapes are shapes
				that are transparant or any other attribute that we are unable to render
				to normally. The Scanning Render Region is used to help with things like
				printing, Renderable EPS export etc where transparancy is not available.

********************************************************************************************/

class ScanningRenderRegion : public RenderRegion
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(ScanningRenderRegion);

	// Construction and destruction
	ScanningRenderRegion(BOOL Printing);
	~ScanningRenderRegion();

	// Starting and stopping the rendering process
	virtual BOOL StartRender();
	virtual BOOL StopRender();

	// Functions that are meant to do the drawing
	virtual void DrawPathToOutputDevice(Path *PathToRender, PathShape shapePath=PATHSHAPE_PATH);
	virtual void DrawRect(DocRect *RectToRender);
	virtual void DrawLine(const DocCoord &StartPoint, const DocCoord &EndPoint);
	virtual void DrawPixel(const DocCoord &Point);

	// Functions that are supposed to draw bitmaps
	virtual void DrawBitmap(const DocCoord &Point, KernelBitmap* pBitmap);
	virtual void DrawBitmap(const DocCoord &Point, UINT32 BitmapID, UINT32 ToolID = NULL);
	virtual BOOL DrawTransformedBitmap(NodeBitmap *pNodeBitmap);

	// Other Pure virtual drawing functions that have to be overidden
	virtual void DrawDragRect(DocRect *RectToRender);
	virtual void DrawBlob(DocCoord p, BlobType type);
	virtual void DrawCross(const DocCoord &Point, const UINT32 Size);
	virtual void DrawBitmapBlob(const DocCoord &Point, KernelBitmap* BlobShape);
	virtual void DrawBitmapBlob(const DocCoord &Point, ResourceID resID );

	// Pure Virtual functions that have to be overidden to use this class.
	// They all do nothing in this class
	virtual void InitClipping();
	virtual void InitAttributes();
	virtual void SetLineAttributes();
	virtual void SetOSDrawingMode();
	virtual void SetQualityLevel();
	virtual MILLIPOINT CalcPixelWidth();
	virtual MILLIPOINT CalcScaledPixelWidth();

	// Force this to return a sensible value so that the capture system
	// doesn't get horribly upset
	virtual double GetPixelsPerInch() { return 96.0; }

	// Make sure that offscreen attrs are rendered when out of bounds
	virtual BOOL CheckOffscreenBounds() { return(FALSE); }

	// Function to work out what things this type of render region is capable of rendering
	virtual void GetRenderRegionCaps(RRCaps* pCaps);

	// Function to tell the scanning render region what the region it is scanning
	// for can render.
	void SetHostRRCaps(const RRCaps& pCaps);

	// Functions to get at the info held by the scanning render regions
	Node* GetFirstComplexObject();
	Node* GetLastComplexObject();

	// Find the number of nodes in various sections of the tree
	INT32 GetNumComplex();

	// Function to find out the size of the bounding rect that covers all complex shapes
	DocRect GetComplexBounds();

	// Function to find out if we are done yet
	BOOL IsFinished();

	// Functions to get at the list of runs etc
	BOOL IsFirstNodeComplex();
	Node* FindFirstFromList();
	Node* FindNextFromList();
	BOOL IsThisNodeComplex();
	DocRect GetSpanBounds();

	void SetRenderState(Node *RenderState);

protected:
	// Function that is called when a complex shape has been found
	void ComplexShapeFound() { bIsComplex = TRUE; };
	void ComplexShapeFoundHelper();		// CGS (12/10/2000):  MUST take NodeBlenders of compound nodes into account!
	void SimpleShapeFound() { bIsSimple = TRUE; };
	void SimpleShapeFoundHelper();		// CGS (12/10/2000):  MUST take NodeBlenders of compound nodes into account!

public:
	void ComplexShapeFoundWrapper();	// CGS (20/10/2000):  allows us to access ComplexShapeFound directly.
										// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
										// YOU SHOULD ONLY CALL THIS FUNCTION IF YOU KNOW WHAT YOU ARE DOING !!!!!!!
										// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#ifdef _DEBUG
	void DumpNodeRuns();
#endif


protected:
	// Vars that the Scanning render region uses
	// The first and last complex objects
	Node* pFirstComplex;
	Node* pLastComplex;

	// The number of complex shapes in the region
	INT32 NumComplexShapes;

	// a rectangle that holds the bounds of all the complex shapes in the render region
	DocRect ComplexBounds;
	DocRect SpanBounds;

	// List of runs of complex and non-complex nodes
	List NodeRuns;
	NodeListItemWithComplexity* pCurrentListItem;
	BOOL IsFirstComplex;
	BOOL IsThisTheFirstNode;
	BOOL InComplexRun;
	INT32 RunLength;

	BOOL bIsComplex;
	BOOL bIsSimple;
	ClipRegionAttribute* pLastClipAttr;

private:
	// A flag to say if we have finished rendering or not
	BOOL IsFinishedRendering;

	// The Caps for the region we are scanning on behlaf of
	RRCaps HostCaps;
};



#endif  // INC_SCANRR
