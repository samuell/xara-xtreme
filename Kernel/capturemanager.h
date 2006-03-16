// $Id: capturemanager.h 662 2006-03-14 21:31:49Z alex $
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


// CaptureManager.h
//
// Created: 29/06/2004 by Phil
//

#ifndef INC_CaptureManager
#define INC_CaptureManager




// Forward references
class Capture;
class ClipRegionAttribute;


// Typedefs
typedef CMapPtrToPtr ClipRegionMap;



/********************************************************************************************

< CAPTUREINFO

	Comment:	Info structure stored in a CaptureManager instance, describing how it should
				behave in relation to other Captures
	SeeAlso:	RenderRegion::StartCapture

********************************************************************************************/ 

typedef enum
{
	ctDISABLED = 0,			// Don't allow capture to start up
	ctNESTABLE,				// New captures should be nested inside existing ones
	ctREUSE,				// New captures will be ignored if there's one already running
	ctRESTART				// New captures will stop existing captures
} CaptureType;

//  --------------------------------------------------------------------------------------
typedef enum
{
	cfNONE						= 0x00,
	cfMASTER					= 0x01,				// This is the Master Capture
	cfCOLLAPSETOMASTER			= 0x02,				// This Capture can be collapsed on demand
	cfGRABRENDERED				= 0x04,				// This Capture should not have a bitmap of its own
	cfDEBUG						= 0x08,				// This Capture should be debugged
	cfLOCKEDTRANSPARENT			= 0x10,				// This Capture should remain transparent no matter what!
	cfQUALITYNORMAL				= 0x20,				// This Capture will force non-AntiAliased rendering
	cfBITMAPSMOOTHING			= 0x40,				// This Capture will set the BitmapSmoothing flag
	cfTILEFILTERING				= 0x80,				// This Capture will set the TileFiltering flag
	cfALLOWDIRECT				= 0x100,			// This Capture can get a bitmap directly from a supplier node
	cfDIRECT					= 0x200,			// This Capture is "Direct" (gets the bitmap of a child)
	cfFULLCOVERAGE				= 0x400,			// This Capture covers its owner entirely
	cfUNCLIP					= 0x800,			// This Capture removes any clip regions in force when it starts
	cfPIXWIDTHSCALE				= 0x1000			// This Capture allows ScaledPixelWidth to differ from PixelWidth
} CaptureFlagBits;

//  --------------------------------------------------------------------------------------
typedef struct tagCaptureFlags
{
	tagCaptureFlags()
					{
						Master = FALSE;
						CollapseToMaster = FALSE;
						GrabRendered = FALSE;
						Debug = FALSE;
						LockedTransparent = FALSE;
						QualityNormal = FALSE;
						BitmapSmoothing = FALSE;
						TileFiltering = FALSE;
						AllowDirect = FALSE;
						Direct = FALSE;
						FullCoverage = FALSE;
						UnClip = FALSE;
						CalcScaledPixWidth = FALSE;

						HasBeenSuppliedWithBitmap = FALSE;
					}
	tagCaptureFlags(UINT32 flagbits)
					{
						Master = ((flagbits & cfMASTER) == cfMASTER);
						CollapseToMaster = ((flagbits & cfCOLLAPSETOMASTER) == cfCOLLAPSETOMASTER);
						GrabRendered = ((flagbits & cfGRABRENDERED) == cfGRABRENDERED);
						Debug = ((flagbits & cfDEBUG) == cfDEBUG);
						LockedTransparent = ((flagbits & cfLOCKEDTRANSPARENT) == cfLOCKEDTRANSPARENT);
						QualityNormal = ((flagbits & cfQUALITYNORMAL) == cfQUALITYNORMAL);
						BitmapSmoothing = ((flagbits & cfBITMAPSMOOTHING) == cfBITMAPSMOOTHING);
						TileFiltering = ((flagbits & cfTILEFILTERING) == cfTILEFILTERING);
						AllowDirect = ((flagbits & cfALLOWDIRECT) == cfALLOWDIRECT);
						Direct = ((flagbits & cfDIRECT) == cfDIRECT);
						FullCoverage = ((flagbits & cfFULLCOVERAGE) == cfFULLCOVERAGE);
						UnClip = ((flagbits & cfUNCLIP) == cfUNCLIP);
						CalcScaledPixWidth = ((flagbits & cfPIXWIDTHSCALE) == cfPIXWIDTHSCALE);

						HasBeenSuppliedWithBitmap = FALSE;
					}
//	tagCaptureFlags(UINT32 flagbits);

	BOOL Master				: 1;					// This is the Master Capture
	BOOL CollapseToMaster	: 1;					// This Capture can be collapsed on demand
	BOOL GrabRendered		: 1;					// This Capture should not have a bitmap of its own
	BOOL Debug				: 1;					// This Capture should be debugged
	BOOL LockedTransparent  : 1;					// This Capture should remain transparent no matter what!
	BOOL QualityNormal		: 1;					// This Capture will force normal quality rendering
	BOOL BitmapSmoothing	: 1;					// This Capture will set the BitmapSmoothing flag
	BOOL TileFiltering		: 1;					// This Capture will set the TileFiltering flag
	BOOL AllowDirect		: 1;					// This Capture can get its data from a supplier node
	BOOL Direct				: 1;					// This Capture is "Direct" (gets the bitmap of a child)
	BOOL FullCoverage		: 1;					// This Capture covers its owner entirely
	BOOL UnClip				: 1;					// This Capture removes any clip regions in force when it starts
	BOOL CalcScaledPixWidth : 1;					// This Capture allows ScaledPixelWidth to differ from PixelWidth

	BOOL HasBeenSuppliedWithBitmap : 1;				// Supplier has called ConsumeDirectBitmap

} CaptureFlags;

//  --------------------------------------------------------------------------------------
typedef struct tagCaptureInfo
{
	tagCaptureInfo() {caType = ctDISABLED, caFlags = CaptureFlags(0);}
	tagCaptureInfo(CaptureType cctype, CaptureFlags ccflags) {caType=cctype; caFlags=ccflags;}
	tagCaptureInfo(CaptureType cctype, UINT32 ccflags) {caType=cctype; caFlags=CaptureFlags(ccflags);}

	CaptureType			caType;
	CaptureFlags		caFlags;
} CAPTUREINFO;




/********************************************************************************************

>	class Capture : public CCObject

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/2004
	Purpose:	Hold current bitmap cacheing state during rendering and functionality
				to store bitmaps in cache, detect limits of cached spans, etc.

				This class is instantiated while a chunk of the tree is being rendered
				with the intention of capturing it in the BitmapCache.
				When the chunk ends, the CaptureManager decides whether it can
				store the bitmap or not and then dies.
				The "chunk" of tree may be a single node, a subtree or an arbitrary span
				of nodes from point A to point B

				A Capture may own a bitmap but doesn't always do so.

				Note! This class replaces OffscreenState and CaptureManager.
				This is how it should have been implemented in the first place...!				

	SeeAlso:	RenderRegion; RenderRegion::RenderTree; Node::RenderSubtree

********************************************************************************************/

class Capture : public CCObject
{
public:
	CC_DECLARE_DYNAMIC(Capture);

	Capture();												// Default constructor
	Capture(CCObject* pOwner,								// New Capture constructor
			CAPTUREINFO cinfo,
			Matrix cMatrix,
			DocRect cRect,
			double PixelsPerInch,
			ColourContext* pcColourContext
			);
	Capture(Capture* pSourceCapture,						// Copy constructor
			BOOL bCopyBitmap = FALSE);
	~Capture();

// Operating functions
public:
	BOOL		IsTransparent() const	{return (lpBitmapInfo && lpBitmapInfo->bmiHeader.biCompression==0x80000001);}
	BOOL		IsMaster() const		{return info.caFlags.Master; }
	BOOL		IsDirect() const		{return info.caFlags.Direct; }
	BOOL		HasFullCoverage() const	{return info.caFlags.FullCoverage; }
	BOOL		IsUnclipped() const		{return info.caFlags.UnClip; }
	CCObject*	GetOwner() const		{return m_pOwner;}
	BOOL		OwnsBitmap() const		{return lpBitmapInfo!=NULL && lpBits!=NULL;}
//	BOOL		AllocateDIB(BOOL bTransparent = FALSE);
	BOOL		SetDIB(LPBITMAPINFO lpnewInfo, LPBYTE lpnewBits) {lpBitmapInfo = lpnewInfo; lpBits = lpnewBits; return TRUE;}
	BOOL		SetClipRect(DocRect NewClipRect) {CaptureRect = NewClipRect; return TRUE;}
	void		SetTransparent(BOOL bNewState);
	BOOL		FreeDIB();
	void		SetDirectSupplier(NodeRenderableInk* pSupplier) {m_pDirectBitmapSupplier = pSupplier;}
	NodeRenderableInk* GetDirectSupplier() const {return m_pDirectBitmapSupplier;}
	BOOL		ConsumeDirectBitmap(NodeRenderableInk* pSupplier);
	void		SetClipRegionAttrValue(ClipRegionAttribute* pClipRegion) {m_pClipRegionAttrValue = pClipRegion;}

	const REGION*	GetCachedClipRegion(ClipRegionAttribute* pClipAttr);
	void			SetCachedClipRegion(ClipRegionAttribute* pClipAttr, const REGION* pClipRegion);
	void			RemoveCachedClipRegion(ClipRegionAttribute* pClipAttr);
	void			RemoveAllCachedClipRegions();
	
// Public Properties
public:
	// These values record the state of THIS capture! NOT the values to restore
	// when this capture ends!
	DocRect					CaptureRect;
	LPBITMAPINFO			lpBitmapInfo;
	LPBYTE					lpBits;
	Matrix					CaptureMatrix;
	ColourContext*			pColContext;
	CAPTUREINFO				info;
	double					dPixelsPerInch;

// Private members
private:
	CCObject*				m_pOwner;
	NodeRenderableInk*		m_pDirectBitmapSupplier;
	ClipRegionMap			m_ClipRegionMap;
	ClipRegionAttribute*	m_pClipRegionAttrValue;

};

#endif	// INC_CaptureManager
