// $Id: offscrn.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_OFFSCREEN_RR
#define INC_OFFSCREEN_RR

// structs, typedefs, defines
#include "gconsts.h"		// GCONTEXT

// class definitions
//	1 - call classes member functions,
//	2 - use the class type for one of the members (not required if have pointer to class ??)
//	3 - to derive a superclass??

// class predeclarations
class GRenderDIB;
class GRenderRegion;
class ConcurrentRenderer;

// constants for view scaling.
// the pixel values apply for 100% zoom.
#define PIXELS_PER_INCH			96.0
#define MILLIPOINTS_PER_INCH	72000.0
#define INCHES_PER_METRE		(100.0 / 2.54)

//#pragma message( __LOCMSG__ "We're only interested in the consts for now" )
#if 1

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								GRenderRegionWrappers
//
/////////////////////////////////////////////////////////////////////////////////////////////
/*
This class encapsulates the functionality required to temporarily use GDraw to render
into a new bitmap, and then return it to the state that it was in.

It is intended for use when you need to render a bitmap dynamically inside any of the 
following functions:-
1) Node::Render
2) Node::PreRenderChildren
3) Node::GoingOutOfScope
4) Node::ExportRender
5) Node::PreExportRender

It is designed to create bitmaps based on the screen resolution.
*/
class GRenderRegionWrapper : public CCObject // CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(GRenderRegionWrapper)

public:

	GRenderRegionWrapper() {}
	~GRenderRegionWrapper() {}

	// Setup and restore
	static GRenderRegionWrapper* GetAppropriateRenderWrapper (
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			);
	static ConcurrentRenderer* GetConcurrentRenderer (
					GRenderRegion*	pSourceRR,
					double			ResFactor,
					DocRect			AreaOfDocumentToRender,
					UINT32			bpp,
					BOOL			NeedsTransp
			);
	virtual BOOL Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			) = 0;
	virtual BOOL RestorePreviousRendererState() = 0;
	static void NewWrappedRRCreated(GRenderRegion* pGrr);

	// Under the hood access
	virtual GRenderRegion* GetRenderRegion() = 0;
	void GetBitmapPointers(LPBITMAPINFO* bi, LPBYTE *by);

// Karim 06/07/2000
// I'm commenting these pre-processor directives out, to fix a bug with the new shadowing
// code, where shadows at anything other than 100% zoom are incorrectly scaled. This is a
// BODGE, and this code should be revisited at a later date.

//#ifdef _DEBUG
	static void GetBitmapPointers(GRenderRegion* pGrr, LPBITMAPINFO* bi, LPBYTE *by);
//#endif
	void SetBitmapPointers(LPBITMAPINFO bi, LPBYTE by);

	// Extract bmp in a convenient form
	KernelBitmap* GetKernelBitmap();
};

// Borrows GDraw from currently active RR by leveraging GRenderRegions offscreen rendering
// system
// Should be faster than ConcurrentRenderer because it doesn't need to create a new GRenderRegion
// class
class GDrawBorrower: public GRenderRegionWrapper
{
public:

	GDrawBorrower();
	~GDrawBorrower();

	virtual BOOL Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			);
	virtual BOOL RestorePreviousRendererState();
	virtual GRenderRegion* GetRenderRegion()  { return m_pBorrowedRR; }

private:
	GRenderRegion* m_pBorrowedRR;
};

// 'Quick' in that it saves you having to construct a GRenderDIB and manually perform all
// the initialisation steps which are required before you can render using the GRenderDIB
// class
class QuickRenderer : public GRenderRegionWrapper
{
public:

	QuickRenderer();
	~QuickRenderer();

	virtual BOOL Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			);
	virtual BOOL RestorePreviousRendererState();
	virtual GRenderRegion* GetRenderRegion() { return (GRenderRegion*) m_pNewRR; }

	static GRenderDIB* CreateGRenderDIB(
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp,
					Matrix* pMat = NULL);

protected:
	void RestoreSimple();
	GRenderDIB* m_pNewRR;
};

// Saves GDraw context of currently active GRenderRegion
// Creates a new GRenderDIB (using QuickRenderer static fn)
class ConcurrentRenderer : public QuickRenderer
{
public:

	ConcurrentRenderer();
	~ConcurrentRenderer();

	virtual BOOL Initialise(
					GRenderRegion* pActiveRR,
					double ResFactor,
					DocRect AreaOfDocumentToRender,
					UINT32 bpp,
					BOOL NeedsTransp
			);
	virtual BOOL RestorePreviousRendererState();

private:
	GDrawContext* m_pOldGD;
	GDrawContext* m_pNewGD;
	View* m_pView;
};

#endif//0

#endif //INC_OFFSCREEN_RR
