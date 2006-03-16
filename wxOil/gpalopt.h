// $Id: gpalopt.h 662 2006-03-14 21:31:49Z alex $
/////////////////////////////////////////////////////////////////////////////
//
// GPalOpt.h
//
/////////////////////////////////////////////////////////////////////////////
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

#ifndef GAVINS_PALETTE_OPTIMISER
#define GAVINS_PALETTE_OPTIMISER

#define TypeDef2(name1,name2)					\
	typedef const name2			  c##name1 ;	\
	typedef		  name2*		  p##name1 ;	\
	typedef const name2*		 pc##name1 ;	\
	typedef		  name2*  const	 cp##name1 ;	\
	typedef const name2*  const	cpc##name1 ;	\
	typedef		  name2**		 pp##name1 ;	\
	typedef const name2**		ppc##name1 ;	\
	typedef		  name2** const cpp##name1 ;

#define TypeDef(name) TypeDef2(name,name) ;

TypeDef ( BITMAP				)
TypeDef2( BOOL			,bool	)
TypeDef ( BYTE					)
TypeDef2( DOUBLE		,double	)
TypeDef ( DWORD					)
TypeDef ( FILE					)
TypeDef2( INT32			,INT32	)
TypeDef ( LOGPALETTE			)
TypeDef ( INT32					)
TypeDef ( PALETTEENTRY			)
TypeDef ( RGBQUAD				)
TypeDef ( UINT32					)
TypeDef ( UINT32					)
TypeDef2( VOID			,void	)

/////////////////////////////////////////////////////////////////////////////

class PaletteOptimiser {
public :
	//
	// By default, system colours are not included in the palette, the browser
	// palette is not used, snapping is disabled and there are no locked colours.
	//
	PaletteOptimiser() :
		m_bFast					(false),
		m_bMoments				(false),
		m_bAddSystemColours		(false),
		m_bUseBrowserPalette	(false),
		m_bUseBitmapColours		(true),
		m_uSnapToPrimaries		(0),
		m_uSnapToBrowserPalette	(0),
		m_uTotalColours			(0),
		m_uLockedColours		(0),
		m_uAllLockedColours		(0),
		m_uPaletteEntries		(0)
	{} ;

	//
	// Initialise
	// 
	// Call this function prior to calling AddStats the first time.
	// It initialises the internal statistics tables.
	//
	void Initialise() ;
	//
	// AddStats
	// 
	// This function processes the pixels of a bitmap and adds variaus statistics
	// to the internal tables. This function can be called several times, once
	// for each strip of a bitmap. Call Initialise prior to the first call to
	// this function to initialise the statistics tables.
	//
	//		pBitmap - pointer to bitmap
	//		nSize	- size of bitmap
	//
	void AddStats( cpcRGBQUAD pBitmap, cUINT32 uSize ) ;
	//
	// GenPalette
	// 
	// This function generates the colours of an optimised palette. This
	// function is usually called once after calling AddStats. GetPalette can
	// be called several times to then obtain the actual palette of a
	// particular size.
	//
	// It will be necessary to recall GenPalette if any of following properties
	// are altered: AddSystemColours, UseBrowserPalette, SnapToPrimaries, 
	// SnapToBrowserPalette. It will also need to be recalled if the locked
	// colours change. Note it is not necessary to recall it if only the size
	// of the palette is altered.
	//
	// The function takes a single parameter that indicates the maximum size of
	// the palette. It defaults to 256 colours. GetPalette will never return a
	// palette with more than this number of colours. Set this to as low a
	// value as possible as a higher value than necessary will make this
	// function call unnecessarily slow.
	//
	void GenPalette( cUINT32 uMaxColours=0x100 ) ;
	//
	// GetPalette
	// 
	// Call this function to obtain the optimised palette of a particalur size.
	//
	//		pPalette	- points to a logical palette. The palNumEntries entry
	//					  should previously have been initialised.
	//		nMaxColours - required number of optimised colours.
	//
	// Returns true if the palette only includes colours in the original bitmap
	// or snapped versions of such colours. Never returns true if using the
	// browser palette.
	//
	// Note that the palette is sorted in order of most used colours to least
	// used colours. Locked colours and system colours are not guarenteed to be
	// within the palette although they are treated as having a high priority.
	//
	// Sets the palNumEntries entry of the palette to the number of colours
	// generated.
	//
	bool GetPalette( cpLOGPALETTE pPalette, cUINT32 MaxColours ) ;
	//
	// AddSystemColours
	// 
	// Set this if you require the system colours to be included within the
	// optimised palette. Defaults to unset.
	//
	void AddSystemColours( cBOOL bAddSysClrs ) { m_bAddSystemColours = bAddSysClrs ; } ;
	//
	// UseBrowserPalette
	// 
	// Set this if you want all colours except locked colours and system palette
	// colours to come from the browser palette. Defaults to unset.
	//
	void UseBrowserPalette( cBOOL bUseBrwsrPal ) { m_bUseBrowserPalette = bUseBrwsrPal ; } ;
	//
	// UseBitmapColours
	// 
	// Set this if you want the optimiser to attempt to use the original bitmap
	// colours if possible. Defaults to set.
	//
	void UseBitmapColours( cBOOL bUseBtmpClrs ) { m_bUseBitmapColours = bUseBtmpClrs ; } ;
	//
	// SnapToPrimaries
	// 
	// Colours less than or equal to this distance away from the eight primary
	// colours will be snapped to the primary colours. Set to 0 to disable.
	// The usual value for this will probably be 1 or 2. Defaults to 0.
	//
	void SnapToPrimaries( cUINT32 uSnap ) { m_uSnapToPrimaries = uSnap ; } ;
	//
	// SnapToBrowserPalette
	// 
	// Colours less than or equal to this distance away from a browser colour
	// will be snapped to that colours. Set to 0 to disable. The usual value for
	// this will probably be 4 or more. Defaults to 0.
	//
	void SnapToBrowserPalette( cUINT32 uSnap ) { m_uSnapToBrowserPalette = uSnap ; } ;
	//
	// AddLockedColour
	// 
	// Call this function to add a locked colour. Locked colours are always added
	// first to the optimal palette, but the order of the locked colours is not
	// relevant. By default there are no locked colours.
	//
	void AddLockedColour( cBYTE red, cBYTE green, cBYTE blue ) ;
	//
	// RemoveLockedColours
	// 
	// The following function will remove all locked colours.
	//
	void RemoveLockedColours() { m_uLockedColours = 0 ; }
	//
	// SetFast
	// 
	// If set to true, then speeds up GenPalette. Defaults to false. It may be
	// worthwhile setting this if optimising several palettes eg for GIF
	// animations with optimised palette per frame.
	//
	void SetFast( cBOOL bFast ) { m_bFast = bFast ; }



	enum {
		AXIS		= 0x21,
		AXIS2		= AXIS*AXIS,
		AXIS3		= AXIS*AXIS*AXIS,

		MAXCOLOURS	= 0x100,

		RED			= 0,
		GREEN		= 1,
		BLUE		= 2
	} ;

protected :
	struct Stats {
		double W ;
		double R ;
		double G ;
		double B ;
		double S ;
	} ;
	struct Clr {
		UINT32 r ;
		UINT32 g ;
		UINT32 b ;
	} ;
	struct Box {
		UINT32 r0,r1 ;	// min value, exclusive
		UINT32 g0,g1 ;	// max value, inclusive
		UINT32 b0,b1 ;
		UINT32 vol ;
		double var ;
		Box* pPair ;
		Clr clr ;
	} ;
	struct PalEntry {
		PALETTEENTRY C ;
		double W ;
	} ;
	TypeDef( Stats		)
	TypeDef( Clr		)
	TypeDef( Box		)
	TypeDef( PalEntry	)

	bool m_bFast ;
	bool m_bMoments ;
	bool m_bAddSystemColours ;
	bool m_bUseBrowserPalette ;
	bool m_bUseBitmapColours ;
	UINT32 m_uSnapToPrimaries ;
	UINT32 m_uSnapToPrimaries2 ;			// Square of nSnapToPrimaries.
	UINT32 m_uSnapToBrowserPalette ;
	UINT32 m_uSnapToBrowserPalette2 ;		// Square of nSnapToBrowserPalette.

	UINT32 m_uTotalColours ;				// Count of colours found in image, upto 0x101.
	RGBQUAD m_aColours[MAXCOLOURS+1] ;	// Array of colours found in image.
	UINT32	m_aCount  [MAXCOLOURS+1] ;	// Count of colours found in image.

	UINT32    m_uLockedColours ;			// Number of locked colours.
	UINT32 m_uAllLockedColours ;			// Number of locked colours and system colours.
	PALETTEENTRY m_aLockedColours[MAXCOLOURS] ;

	UINT32 m_uPaletteEntries ;
	PalEntry m_aPalette[3*MAXCOLOURS] ;
	static INT32 __cdecl SortFn( pcVOID elem1, pcVOID elem2 ) ;

	double Snap( cpBox pCube, cDOUBLE halfW,cDOUBLE halfR,cDOUBLE halfG,cDOUBLE halfB ) ;

	double LCube( double Stats::* p, cUINT32 r,cUINT32 g,cUINT32 b ) const { return m_aStats[(r*AXIS+g)*AXIS+b].*p ; }

	double    Vol( cpcBox pCube,					  double Stats::* p ) const ;
	double Bottom( cpcBox pCube, cUINT32 dir,			  double Stats::* p ) const ;
	double    Top( cpcBox pCube, cUINT32 dir,cUINT32 pos, double Stats::* p ) const ;
	double    Var( cpcBox pCube ) const ;

	void Store( cUINT32 i, cpBox pCube ) ;

	void Moments3D() ;

	double Maximize(
			cpBox pCube, 
			cUINT32 dir, 
			cUINT32 first,
			cUINT32 last,
			cpBox pCube1,
			cpBox pCube2,
			cDOUBLE wholeW,
			cDOUBLE wholeR,
			cDOUBLE wholeG,
			cDOUBLE wholeB
		) ;
	double Maximize(
			cpBox pCube,
			cUINT32 dir, 
			cUINT32 first,
			cUINT32 last,
			cpBox pCube1,
			cpBox pCube2,
			cpBox pCube3,
			cDOUBLE wholeW,
			cDOUBLE wholeR,
			cDOUBLE wholeG,
			cDOUBLE wholeB
		) ;
	bool Cut( cpBox pSet1,cpBox pSet2, cpDOUBLE pMax ) ;
	bool Cut( cpBox pSet1,cpBox pSet2,cpBox pSet3 ) ;

	Stats m_aStats[AXIS3] ;

	static const BYTE  aSnap[0x132] ;
	static const UINT32 aBDist[0x100] ;
	static const UINT32 aPDist[0x100] ;
	static const UINT32  aSqrs[0x100] ;
} ;


#endif

/////////////////////////////////////////////////////////////////////////////
