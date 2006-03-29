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
#include "camtypes.h"

#ifdef BUILDSHADOWS

#include "bshadow.h"
#include "gconsts.h"
#include "dibutil.h"
#include "bitmap.h"
#include "fillattr.h"
#include "gdraw.h"

#include "camprofile.h"

#if _DEBUG
	#include "oilbitmap.h"
#endif

//#define MMX
//#define SSE2
#define UNALIGNED


#pragma warning ( disable : 4554 )

/******************************************************************************************************/

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// CC_IMPLEMENT_DYNCREATE(CBitmapShadow, CCObject)
// CC_IMPLEMENT_DYNCREATE(ShadowBitmap, CCObject)

/******************************************************************************************************/

const double Pi = 3.1415926535897932384626433832795 ;

/******************************************************************************************************/

// shrink or extend a bitmap in the y direction only (will have same width)
// uses resampling to produce more accurate shrunken bitmap replications.
// Assumes buffers for source and Dest are already defined and of the given dimensions.
// Merges (resamples) just the alpha channels, colour taken from a spot check

bool CBitmapShadow::ProjectShadow(BYTE* pSBitmap, BYTE* pDBitmap, UINT32 nWidth, UINT32 nSHeight, UINT32 nDHeight)
{
	const UINT32 nWidth0 = DIBUtil::ScanlineSize(nWidth,8) ;
	UINT32 j = 0;
	UINT32 k = 0;
	UINT32 i = 0;
	UINT64 nPos = 0;
	BYTE* pDPtr = NULL;
	BYTE* pSPtr = NULL;
	BYTE* pEPtr = NULL;
	UINT32 t = 0;
	UINT32 nEnd = 0;
	UINT32 nStart = 0;
	UINT32 nLow = 0;
	UINT32 nHgh = 0;

	if ( nSHeight==nDHeight )
		memcpy(pDBitmap,pSBitmap,nWidth0*nSHeight) ;
	else if ( nSHeight>nDHeight )
	{
		const UINT64 nStep = (((UINT64)nSHeight<<32)-1)/((UINT64)nDHeight) ;
		const UINT32 nScale = (nDHeight<<16)/nSHeight ;
		for ( j=0 ; j<nDHeight ; j++ )
		{
			pSPtr = pSBitmap+UINT32(nPos>>32)*nWidth0 ;
			pDPtr  = pDBitmap ;
			nStart = UINT32(nPos>>32) ;
			nLow   = ~UINT32(nPos)>>16 ;
			nPos += nStep ;
			nHgh   =  UINT32(nPos)>>16 ;
			nEnd = UINT32(nPos>>32) ;
			for ( i=0 ; i<nWidth ; i++ )
			{
				t = 0 ;
				pEPtr = pSPtr+nWidth0 ;
				for ( k = nStart+1 ; k<nEnd ; k++ )
				{
					t += *pEPtr ;
					pEPtr += nWidth0 ;
				}
				t = (t<<16)+*pSPtr*nLow+*pEPtr*nHgh ;
				*pDPtr++ = BYTE((UInt32x32To64(t,nScale)+0x80000000)>>32) ;
				pSPtr++ ;
			}
			pDBitmap += nWidth0 ;
		}
	}
	else
	{
		const UINT64 nStep = ((UINT64)(nSHeight-1)<<32)/((UINT64)(nDHeight-1)) ;
		memcpy(pDBitmap,pSBitmap,nWidth0) ;
		pDBitmap += nWidth0 ;
		UINT32 nScale = 0;
		for ( j=2 ; j<nDHeight ; j++ )
		{
			nPos += nStep ;
			pSPtr = pSBitmap+UINT32(nPos>>32)*nWidth0 ;
			pDPtr  = pDBitmap ;
			nScale = UINT32(nPos)>>8 ;
			for ( i=0 ; i<nWidth ; i++ )
			{
				*pDPtr++ = (BYTE)(pSPtr[0]+(nScale*(pSPtr[nWidth0]-pSPtr[0])>>24)) ;
				pSPtr++ ;
			}
			pDBitmap += nWidth0 ;
		}
		memcpy(pDBitmap,pSBitmap+(nSHeight-1)*nWidth0,nWidth0) ;
	}

	return TRUE;
}



CBitmapShadow::CBitmapShadow()
{
	m_bIsWallShadow =0;
	m_dProjection =0.0;
	m_dTiltAngle =0.0;
	m_dBlurriness =0.0;
	m_dDarknessCoeff =0.0;
	m_dPositionX =0.0;
	m_dPositionY =0.0;
	m_OffsetX =0;
	m_OffsetY =0;		
}



BOOL CBitmapShadow::MakeWallShadow (const UINT32* pForegroundBits,
											const CNativeSize ForegroundSize,
											const INT32 nXPosition,
											const INT32 nYPosition,
											const double fBlur,
											INT32 *pOffsetX,
											INT32 *pOffsetY,
											const double dDarknessCoeff/*=1.0*/)
{
	// if the required blur is too small, then just copy the existing bitmap into an 8 bit bitmap.
	if ( fBlur < MIN_BLUR_DIAMETER )
	{
		// set up my bitmap to shadow into
		/*HRESULT Result =*/ CreateFilled( ForegroundSize.x,ForegroundSize.y,0xff,8 ) ;
		UINT32 DestWidth = DIBUtil::ScanlineSize(ForegroundSize.x,8);
		BYTE* pSrc0 = (BYTE *)pForegroundBits + 3;
		BYTE* pDest0 = GetBytes();
		BYTE* pDest0NextLine = NULL;
		INT32 x = 0;

		for (INT32 y = 0; y < ForegroundSize.y; y++)
		{
			pDest0NextLine = pDest0 + DestWidth;
			for (x = 0; x < ForegroundSize.x; x++)
			{
				*pDest0++ = *pSrc0;
				pSrc0 += 4;
			}
			pDest0 = pDest0NextLine;
		}
		return TRUE;
	}

	double fNewBlur = fBlur/2.0 ;
	if ( fNewBlur>MAX_SHADOW_BLUR )
		fNewBlur = MAX_SHADOW_BLUR ;
//	UINT32 nBlur = (UINT32)fNewBlur ;
	UINT32 nBlur = (UINT32)(2*fNewBlur+0.5) - 1;

	// Create the bitmap to pass into gavin's routine
	// must be Width + nNewBlur * 4, Height + nNewBlur * 4 in size and each scanline must be DWORD adjusted
	INT32 nBitmapToShadowWidth = DIBUtil::ScanlineSize(ForegroundSize.x+nBlur*2,8);
	UINT32 nBitmapToShadowSize = nBitmapToShadowWidth*(ForegroundSize.y+nBlur*2) ;
	BYTE* pBitmapToShadow = new BYTE[nBitmapToShadowSize];

	// Now, set up the pointers to do the transfer of bits from one bitmap to the other
	// and move the destination pointer into position
	BYTE* pSrc  = (BYTE *)pForegroundBits+3 ;
	BYTE* pDest = (BYTE *)pBitmapToShadow+(nBitmapToShadowWidth+1)*nBlur;

	memset(pBitmapToShadow, 0xff, nBitmapToShadowSize);

	UINT32 ForegroundScanlineSize = ForegroundSize.x * 4;
	
	// transfer the source bitmap into this larger bitmap to be shadowed
	UINT32 j = 0;
	BYTE* pNextDstLine = NULL;
	BYTE* pNextSrcLine = NULL;

	for ( UINT32 i=0 ; i<(UINT32)ForegroundSize.y ; i++ )
	{
		pNextSrcLine = pSrc +ForegroundScanlineSize;
		pNextDstLine = pDest+nBitmapToShadowWidth;

		for ( j=0 ; j<(UINT32)ForegroundSize.x ; j++ )
		{
			*pDest++ = *pSrc;
			pSrc += 4;
		}
		pSrc  = pNextSrcLine;
		pDest = pNextDstLine;
	}

	// Set up my bitmap to shadow into
	/*HRESULT Result =*/ CreateFilled( ForegroundSize.x+nBlur, ForegroundSize.y+nBlur, 0xff , 8);

	// set up the bitmap info headers
	BITMAPINFOHEADER SourceBI;
	SourceBI.biBitCount = 8;
	SourceBI.biWidth    = ForegroundSize.x+nBlur*2;
	SourceBI.biHeight	= ForegroundSize.y+nBlur*2;
	SourceBI.biPlanes   = 1;

	BITMAPINFOHEADER DestBI;
	DestBI.biBitCount = 8;
	DestBI.biWidth    = ForegroundSize.x+nBlur;
	DestBI.biHeight	  = ForegroundSize.y+nBlur;
	DestBI.biPlanes   = 1;

	// blur the bitmap.
	Blur8BppBitmap(	&SourceBI,	pBitmapToShadow,
					&DestBI,	GetBytes(),
					nBitmapToShadowWidth, fNewBlur );

	// don't forget to discard the source bitmap.
	delete [] pBitmapToShadow;

	*pOffsetX = nXPosition - (nBlur>>1);
	*pOffsetY = nYPosition - (nBlur>>1);
	
	return true;
}



/********************************************************************************************

>	static LPBITMAPINFO CBitmapShadow::Feather8BppBitmap(	const double fBlur,
															LPBITMAPINFO pSrcInfo,
															LPBYTE pSrcBits8,
															LPBYTE* ppDestBits	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/2000

	Inputs:		fBlur		the blur diameter - may have a fractional part.
				pSrcInfo	bitmap info for the source bitmap.
				pSrcBits8	the source bitmap bits.
				ppDestBits	ptr to receive the address of the destination bitmap bits.

	Returns:	ptr to the destination bitmap's info structure, or NULL if unsuccessful.

	Purpose:	Convolve the given bitmap with a circular patch of the given blur diameter.

	Notes:		Creates a new bitmap to contain the result of the convolution.

********************************************************************************************/
LPBITMAPINFO CBitmapShadow::Feather8BppBitmap(	const double fBlur,
												LPBITMAPINFO pSrcInfo,
												LPBYTE pSrcBits8,
												LPBYTE* ppDestBits)
{
	// shout out loud if the blur size is <= zero,
	// as we needn't do any feathering in this case.
	ERROR3IF(fBlur <= 0,
			"CBitmapShadow::Feather8BppBitmap; Feathering code used for zero blur size!");

	// if the blur size is too small, then just return a copy of the passed in bitmap.
	if ( fBlur < MIN_BLUR_DIAMETER )
	{
// Phil 02/08/2004
// Don't copy vast amounts of memory around, just return the pointers to the input bitmap!
		*ppDestBits = pSrcBits8;
		return pSrcInfo;
//		LPBITMAPINFO pDestInfo = NULL;
//		BOOL bCopied = DIBUtil::CopyBitmap(	pSrcInfo, pSrcBits8,
//											&pDestInfo, ppDestBits );
//		if (bCopied)
//			return pDestInfo;
//		else
//			return NULL;

	}

	// work out the appropriate blur radius, and cap it at our max allowed blur size.
	double fNewBlur = fBlur / 2.0;
	if (fNewBlur > MAX_SHADOW_BLUR)
		fNewBlur = MAX_SHADOW_BLUR;
	const UINT32 nBlur = UINT32(2*fNewBlur+0.5) ;

	// Size of the source bitmap.
	CNativeSize			SrcBmpSize(pSrcInfo->bmiHeader.biWidth, pSrcInfo->bmiHeader.biHeight);

	// size of the destination bitmap.
	CNativeSize			DestBmpSize(SrcBmpSize.x - nBlur + 1, SrcBmpSize.y - nBlur + 1);

	// the source bitmap must be 8 bits-per-pixel.
	ERROR2IF(	pSrcInfo->bmiHeader.biBitCount != 8, NULL,
				"CBitmapShadow::Shadow8BppBitmap; You must pass an 8bpp bitmap!");

	// the destination bitmap must have a positive width and height.
	ERROR2IF(	DestBmpSize.x <= 0 || DestBmpSize.y <= 0, NULL,
				"CBitmapShadow::Feather8BppBitmap; Blur patch bigger than source bitmap!");

	// the source bitmap must also be DWORD aligned.
	// DIBUtil::Alloc actually does give DWORD aligned bmp's,
	// although the bitmap info header does not reflect this.
	UINT32 DWordWidth = DIBUtil::ScanlineSize(SrcBmpSize.x, 8);


	/////////////////////////////////////////////////////////////////////////////////
	// Allocate dib for convolved bmp and set up info to pass to convolution function.
	/////////////////////////////////////////////////////////////////////////////////
	// Source bitmap info-header.
	BITMAPINFOHEADER SourceBI;
	SourceBI.biBitCount = 8;
	SourceBI.biWidth    = SrcBmpSize.x;
	SourceBI.biHeight	= SrcBmpSize.y;
	SourceBI.biPlanes   = 1;

	// Alloc a destination DIB for the convolution.
	LPBITMAPINFO pDestInfo = AllocDIB(DestBmpSize.x, DestBmpSize.y, 8, ppDestBits, NULL, TRUE);
	ERROR2IF(	pDestInfo == NULL, NULL,
				"CBitmapShadow::Shadow8BppBitmap; Failed to allocate space for new bitmap.");

	pDestInfo->bmiHeader.biXPelsPerMeter = pSrcInfo->bmiHeader.biXPelsPerMeter;
	pDestInfo->bmiHeader.biYPelsPerMeter = pSrcInfo->bmiHeader.biYPelsPerMeter;

	Blur8BppBitmap(&SourceBI, pSrcBits8, &pDestInfo->bmiHeader, *ppDestBits, DWordWidth, fNewBlur);

	return pDestInfo;
}



/********************************************************************************************

>	static void CBitmapShadow::Blur8BppBitmap(	LPBITMAPINFOHEADER pSrcBMIHeader, LPBYTE pSrcBits,
												LPBITMAPINFOHEADER pDestBMIHeader, LPBYTE pDestBits,
												const UINT32 SWordBitmapWidth, const double fBlur )
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/2000
	Inputs:		pSrcBMIHeader		ptr to a bmiHeader for the source bitmap.
				pSrcBits			ptr to the source bitmap bits.
				pDestBMIHeader		ptr to a bmiHeader for the destination bitmap.
				pDestBits			ptr to the destination bitmap bits.
				SWordBitmapWidth	the width of the bitmap, which must be DWord-aligned.
				fBlur				the blur radius - can be non-integer.

	Outputs:	pDestBits gets the result of blurring the bitmap stored in pSrcBits.

	Purpose:	Blur the given source bitmap and, putting the result into the destination
				bitmap, by convolving the source with a circular patch of the given blur
				radius.
	Errors:		
	See also:	

********************************************************************************************/
void CBitmapShadow::Blur8BppBitmap(	LPBITMAPINFOHEADER  pSrcBMIHeader, LPBYTE  pSrcBits,
									LPBITMAPINFOHEADER pDestBMIHeader, LPBYTE pDestBits,
									const UINT32 SWordBitmapWidth, const double fBlur )
{
	////////////////////////////////////////////////////////////////
	// set up the row & columns for the circular convolution mask //
	////////////////////////////////////////////////////////////////
	DWORD aLeft [MAX_ROW_OFFSETS] ;
	DWORD aRight[MAX_ROW_OFFSETS] ;
	DWORD aLow  [MAX_ROW_OFFSETS] ;
	DWORD aHigh [MAX_ROW_OFFSETS] ;

//	UINT32 nBlur = (UINT32)fBlur;
//	UINT32 nSize = 1 + 2*nBlur;
//	double S  = nBlur ;
//	UINT32 nLine = 0 ;
	const UINT32 nSize = (UINT32)(2*fBlur+0.5) ;
	const double R = (nSize+1)*0.5 ;
		  double S = (nSize-1)*0.5 ;
	const double R2 = fBlur*fBlur;
	UINT32 nLine = 0 ;
	UINT32 nArea = 0 ;
	UINT32 nRows = 0 ;
	
	for ( UINT32 r=0 ; r<nSize ; ++r )
	{
		double fRadius = R2-S*S ;
		if ( fRadius>=0 )
		{
			fRadius = sqrt(fRadius) ;
			const UINT32  uLeft = UINT32(R-fRadius) ;
//			const UINT32 uRight = UINT32(R+fRadius) ;
			const UINT32 uRight = nSize-uLeft ;
			if ( uLeft<uRight )
			{
				 aLeft[nRows] =  uLeft+nLine ;
				aRight[nRows] = uRight+nLine ;
				  aLow[nRows] =  uLeft*SWordBitmapWidth+r ;
				 aHigh[nRows] = uRight*SWordBitmapWidth+r ;
				nArea += uRight-uLeft ;
				nRows++ ;
			}
		}
		S-- ;
		nLine += SWordBitmapWidth ;
	}

	UINT32 nTableSize = nArea*255 ;
	UINT32 nInc = 0xffffffffu/nTableSize ;
	UINT32 nShift = 0 ;
	while ( nTableSize>=TABLE_SIZE )
	{
		nTableSize >>= 1 ;
		nShift++ ;
	}
	nInc <<= nShift ;
	BYTE* pTranslationTable = new BYTE[nTableSize+1] ;
	UINT32 nCount = 0 ;
	for ( UINT32 i=0 ; i<=nTableSize ; ++i )
	{
		pTranslationTable[i] = nCount>>24 ;
		nCount += nInc ;
	}


	/////////////////////////////////////////////////////////////////////////////////
	// call gavin's routine to do the shadowing.
	/////////////////////////////////////////////////////////////////////////////////
	CamProfile cp(CAMPROFILE_SHADOW);
	::GenerateWallShadow(
		 pSrcBMIHeader,  pSrcBits,
		pDestBMIHeader, pDestBits,
		nRows, aLeft,aRight,
		nRows, aLow ,aHigh ,
		nShift,pTranslationTable
	) ;	

	/////////////////////////////////////////////////////////////////////////////////
	// clean up the translation table.
	/////////////////////////////////////////////////////////////////////////////////
	delete [] pTranslationTable;
}



// Same as the MakeShadow function above
// only this produces a cast floor shadow
// angle is in degrees - leans left + leans right
// DestHeight lenghtens or shortens the shadow
// make DestHeight = SourceHeight for 100% projection

BOOL CBitmapShadow::MakeCastShadow (UINT32* pForegroundBits,
											const CNativeSize ForegroundSize,
											const double fBlur,									 
											const double dTiltAngle, 
											const double dProjection,
											const double dDarknessCoeff/*=1.0*/ )
{
	m_OffsetX = 0;  
	m_OffsetY = 0;  
	
	// Convert the bitmap down from 32bpp to 8bpp.

	UINT32 nForegroundWidth = DIBUtil::ScanlineSize(ForegroundSize.x,8) ;
	BYTE* p8BitBitmap = new BYTE[nForegroundWidth*ForegroundSize.y] ;
	CONST BYTE* pSrc = (BYTE*)pForegroundBits+3 ;
	BYTE* pDst = p8BitBitmap ;
	BYTE* pDPtr = NULL;
	UINT32 i=0;
	UINT32 j;
	
	for( j = 0 ; j<(UINT32)ForegroundSize.y ; j++ )
	{
		pDPtr = pDst ;
		for ( i=0 ; i<(UINT32)ForegroundSize.x ; i++ )
		{
			*pDPtr++ = *pSrc ;
			pSrc += 4 ;
		}
		pDst += nForegroundWidth ;
	}

	double fNewBlur = fBlur/2.0 ;
	if ( fNewBlur>MAX_SHADOW_BLUR )
		fNewBlur = MAX_SHADOW_BLUR ;
	UINT32 nBlur = (UINT32)fNewBlur ;
	
	CNativeSize ProjectedSize;
	ProjectedSize.x = ForegroundSize.x ;
	ProjectedSize.y = (INT32)( ForegroundSize.y*dProjection+0.5 );
	if ( ProjectedSize.y==0 )
		ProjectedSize.y = 1 ;
	if ( ProjectedSize.y+nBlur<2 )
		ProjectedSize.y = 2-nBlur ;

	// Create a stretched bitmap, for the projection by
	// shrinking/stretching the shadow to the correct projection height.
	const INT32 nProjectedSize = nForegroundWidth*ProjectedSize.y;
	BYTE* pProjected = new BYTE[nProjectedSize];
	ProjectShadow( p8BitBitmap, pProjected, ForegroundSize.x,ForegroundSize.y, ProjectedSize.y) ;

	delete [] p8BitBitmap;

#if 0
	KernelBitmap *pBitmap0 = new KernelBitmap(ProjectedSize.x,ProjectedSize.y,8,0,TRUE) ;
	memcpy( pBitmap0->GetBitmapBits(),pProjected,nProjectedSize ) ;
	LPRGBQUAD pPalette0 = pBitmap0->GetPaletteForBitmap();
	for ( i=0 ; i<256 ; i++ )
	{
		pPalette0[i].rgbRed   =
		pPalette0[i].rgbGreen =
		pPalette0[i].rgbBlue  = i;
	}
	BitmapTranspFillAttribute* pTranspBitmapFill0 = new BitmapTranspFillAttribute;
	pTranspBitmapFill0->AttachBitmap(pBitmap0);
#endif

	UINT32 nBitmapToShadowWidth = DIBUtil::ScanlineSize(ProjectedSize.x+nBlur*4,8) ;
	UINT32 nBitmapToShadowSize = nBitmapToShadowWidth*(ProjectedSize.y+nBlur*2) ;
	BYTE* pBitmapToShadow = new BYTE[nBitmapToShadowSize] ;
	memset( pBitmapToShadow, 0xff, nBitmapToShadowSize ) ;
	pSrc = pProjected;
	pDst = pBitmapToShadow+nBlur*2;
	for ( j=0 ; j<(UINT32)ProjectedSize.y ; j++ )
	{
		memcpy(pDst,pSrc,ProjectedSize.x) ;
		pSrc += nForegroundWidth;
		pDst += nBitmapToShadowWidth;
	}

#if 0
	KernelBitmap *pBitmap1 = new KernelBitmap(ProjectedSize.x+nBlur*4,ProjectedSize.y+nBlur*2,8,0,TRUE) ;
	memcpy( pBitmap1->GetBitmapBits(),pBitmapToShadow,nBitmapToShadowSize ) ;
	LPRGBQUAD pPalette1 = pBitmap1->GetPaletteForBitmap();
	for ( i=0 ; i<256 ; i++ )
	{
		pPalette1[i].rgbRed   =
		pPalette1[i].rgbGreen =
		pPalette1[i].rgbBlue  = i;
	}
	BitmapTranspFillAttribute* pTranspBitmapFill1 = new BitmapTranspFillAttribute;
	pTranspBitmapFill1->AttachBitmap(pBitmap1);
#endif

	delete [] pProjected;

	UINT32 nShadowWidth = DIBUtil::ScanlineSize(ProjectedSize.x+nBlur*2,8) ;
	const INT32 nShadowSize = nShadowWidth*(ProjectedSize.y+nBlur);
	BYTE* pShadowBitmap = new BYTE[nShadowSize];
	memset(pShadowBitmap,0xff,nShadowSize);

	// Set up the bitmap info headers
	BITMAPINFOHEADER SourceBI;
	SourceBI.biBitCount = 8;
	SourceBI.biWidth    = ProjectedSize.x+nBlur*4;
	SourceBI.biPlanes   = 1;
	BITMAPINFOHEADER DestBI;
	DestBI.biBitCount = 8;
	DestBI.biWidth    = ProjectedSize.x+nBlur*2;
	DestBI.biHeight	  = 1;
	DestBI.biPlanes   = 1;

#if defined(MMX) || defined(SSE2)
	BYTE aCode[40*MAX_ROW_OFFSETS] ;
#else
	BYTE aCode[20*MAX_ROW_OFFSETS] ;
#endif
	BYTE aTranslationTable[TABLE_SIZE+1] ;
	UINT32 nLastArea = 0 ;
	UINT32  nLastRows = 0 ;
	UINT32  nLastBlur = 0 ;
	UINT32  nShift = 0 ;

	const BYTE* pSLine = pBitmapToShadow ;
		  BYTE* pDLine = pShadowBitmap ;
	UINT32 nThisBlur = 0;
	UINT32 nSize = 0;
	double S = 0.0;
	double R2 = 0.0;
	UINT32 nLine = 0;
	UINT32 nArea = 0 ;
	UINT32 nRows = 0 ;
	UINT32 r=0 ;
	double Radius2 = 0.0;
	UINT32 nRadius = 0;
	UINT32 nTableSize = 0;
	UINT32 nInc = 0;
	UINT32 nCount = 0 ;
	double fBlur2 = 0.0;
	double fScale = 0.0;

	for ( j=0 ; j<ProjectedSize.y+nBlur ; j++ )
	{
		// Set up the row & columns - a round patch

		DWORD aLeft [MAX_ROW_OFFSETS] ;
		DWORD aRight[MAX_ROW_OFFSETS] ;

		fScale = (double)j/(ProjectedSize.y+nBlur-1) ;
		fBlur2 = fScale * fNewBlur;
		nThisBlur = (UINT32)fBlur2 ;
		if ( nThisBlur>j )
			nThisBlur = j ;
		if ( nThisBlur>nBlur )
			nThisBlur = nBlur ;
		nSize = nThisBlur*2+1 ;
		S = nThisBlur ;
		R2 = fBlur2*fBlur2 ;
		nLine = nThisBlur-(INT32)(nThisBlur*nBitmapToShadowWidth) ;
		nArea = 0 ;
		nRows = 0 ;

		for ( r=0 ; r<nSize ; ++r )
		{
			Radius2 = R2-S*S ;
			if ( Radius2>=0 )
			{
				nRadius = (UINT32)sqrt(Radius2) ;
				 aLeft[nRows] = nLine-nRadius ;
				aRight[nRows] = nLine+nRadius+1 ;
				if ( aLeft[nRows]<aRight[nRows] )
				{
					nArea += aRight[nRows]-aLeft[nRows] ;
					nRows++ ;
				}
			}
			S-- ;
			nLine += nBitmapToShadowWidth ;
		}
	
		if ( nRows==0 || nArea==1 )
			memcpy( pDLine+nBlur,pSLine+nBlur*2,ProjectedSize.x ) ;
		else
		{
			if ( nLastArea!=nArea || nLastRows!=nRows )
			{
				nTableSize = nArea*255 ;
				nInc = 0xffffffffu/nTableSize ;
				nShift = 0 ;
				
				while ( nTableSize>=TABLE_SIZE )
				{
					nTableSize >>= 1 ;
					nShift++ ;
				}
				nInc <<= nShift ;
				nCount = 0 ;
				
				for ( i=0 ; i<=nTableSize ; ++i )
				{
					aTranslationTable[i] = nCount>>24 ;
					nCount += nInc ;
				}
			}
			if ( nLastArea!=nArea || nLastRows!=nRows || nLastBlur!=nThisBlur )
			{
				nLastArea = nArea ;
				nLastRows = nRows ;
				nLastBlur = nThisBlur ;
#if 0 && defined(__WXMSW__)
				::CompileShadowCode(
					aCode,
					ProjectedSize.x+nThisBlur*2,
					nRows,aLeft,aRight,
					nShift,aTranslationTable
				) ;
#endif
			}
			// Call Gavin's routine to do the shadowing for a single scanline
			CamProfile cp(CAMPROFILE_SHADOW);
			::GenerateFloorShadow(
				aCode,
				&SourceBI,pSLine+(nBlur-nThisBlur)*2,
				  &DestBI,pDLine+nBlur-nThisBlur,
				nRows, aLeft,aRight,
				nShift, aTranslationTable
			) ;
		}
		pSLine += nBitmapToShadowWidth ;
		pDLine += nShadowWidth ;
	}	

	delete [] pBitmapToShadow;

#if 0
	KernelBitmap *pBitmap2 = new KernelBitmap(ProjectedSize.x+nBlur*2,ProjectedSize.y+nBlur,8,0,TRUE) ;
	memcpy( pBitmap2->GetBitmapBits(),pShadowBitmap,nShadowSize ) ;
	LPRGBQUAD pPalette2 = pBitmap2->GetPaletteForBitmap();
	for ( INT32 i=0 ; i<256 ; i++ )
	{
		pPalette2[i].rgbRed   =
		pPalette2[i].rgbGreen =
		pPalette2[i].rgbBlue  = i;
	}
	BitmapTranspFillAttribute* pTranspBitmapFill2 = new BitmapTranspFillAttribute;
	pTranspBitmapFill2->AttachBitmap(pBitmap2);
#endif

	Tilt(pShadowBitmap, ProjectedSize.x+nBlur*2,ProjectedSize.y+nBlur, dTiltAngle);
	m_OffsetX -= nBlur;
	
	delete [] pShadowBitmap;

	return true;	
}



BOOL CBitmapShadow::CreateShadow(const BOOL bIsWallShadow,
								 const double dProjection,
								 const double dTiltAngle,
								 const double dBlurriness,
								 const double dDarknessCoeff,
								 const double dPositionX,
								 const double dPositionY,
								 UINT32* pForegroundBits,
								 const CNativeSize ForegroundSize
								 )
{										
	RECT ShadowRect;
	ShadowRect.left =0;
	ShadowRect.right =0;
	ShadowRect.top =0;
	ShadowRect.bottom =0;

	if(bIsWallShadow)
		MakeWallShadow (pForegroundBits,
						ForegroundSize,
						(INT32)(dPositionX + 0.5),
						(INT32)(dPositionY + 0.5),
						dBlurriness,
						&m_OffsetX,
						&m_OffsetY,
						dDarknessCoeff);		
	else
		MakeCastShadow (pForegroundBits,
						ForegroundSize,
						dBlurriness,
						dTiltAngle,
						dProjection,
						dDarknessCoeff);
	
	m_bIsWallShadow  = bIsWallShadow;
	m_dBlurriness    = dBlurriness;
	m_dDarknessCoeff = dDarknessCoeff;

	if(bIsWallShadow)
	{
		m_dPositionX = dPositionX;
		m_dPositionY = dPositionY;
	}
	else
	{
		m_dPositionX = 0;
		m_dPositionY = 0;
	}
	
	return true;
}

//Allocates a bitmap, and returns a pointer to it.
//the returned bitmap is black with transparancy values from the specified source sheared by the 
//specified angle
//Modifies the XOffset members to maintain the position of the larger returned bitmap with respect to the source
ShadowBitmap* CBitmapShadow::Tilt(BYTE* pSourceBits, const INT32 nSourceWidth, const INT32 nSourceHeight, const double dTiltDegrees)
{
	//Convert the given tilt angle to radians, then get it's tangent
	const double dTanTiltAngle = tan(dTiltDegrees*2*Pi/360) ;

	//Find the X offset at the top of the bitmap, this gives the width increase of the
	//destination bitmap over the source
	const INT32 nMaxOffset = (INT32)ceil((nSourceHeight-1)*fabs(dTanTiltAngle)) ;

	//Create the destination bitmap
//	ShadowBitmap* pDestBitmap = NULL ;
	CreateFilled(nSourceWidth+nMaxOffset,nSourceHeight,0xff,8);

	const UINT32 nSWidth = DIBUtil::ScanlineSize(nSourceWidth			,8) ;
	const UINT32 nDWidth = DIBUtil::ScanlineSize(nSourceWidth+nMaxOffset,8) ;
	BYTE* pSLine = pSourceBits ;
	BYTE* pDLine = GetBytes() ;

	double fXOffset = dTanTiltAngle>0 ? 0 : nMaxOffset ;
	INT32 nXOffset = 0;
	UINT32 nXShift = 0;
	BYTE* pSPtr = NULL;
	BYTE* pDPtr = NULL;
	INT32 X=1 ;

	for ( INT32 Y=0 ; Y<nSourceHeight ; Y++ )
	{
		nXOffset = (INT32)floor(fXOffset) ;
		nXShift = (UINT32)((fXOffset-nXOffset)*0xffffff) ;

		if ( nXShift )
		{
			pSPtr = pSLine ;
			pDPtr = pDLine+nXOffset ;
			*pDPtr++ = pSPtr[0]+((0xff-pSPtr[0])*nXShift >> 24) ;

			for( X=1 ; X<nSourceWidth ; X++ )
			{
				*pDPtr++ = pSPtr[1]+((pSPtr[0]-pSPtr[1])*nXShift >> 24) ;
				pSPtr++ ;
			}
			*pDPtr++ = 0xff+((pSPtr[0]-0xff)*nXShift >> 24) ;
		}
		else
			memcpy(pDLine+nXOffset,pSLine,nSWidth) ;
		pSLine += nSWidth ;
		pDLine += nDWidth ;
		fXOffset += dTanTiltAngle ;
	}		
	if ( dTanTiltAngle<0 )
		m_OffsetX -= nMaxOffset;

	return this ;
}

/***************************************************************************************************************************

>	static bool CBitmapShadow::ContourBitmap(
						const LPBITMAPINFO	pSrcInfo32,
						const LPBYTE		pSrcBits32,
						const double		fContour,
						const BYTE			uThreshold,
						LPBITMAPINFO*		ppDestInfo32,
						LPBYTE*				ppDestBits32,
						INT32*				pnOffsetX,
						INT32*				pnOffsetY
					)
	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/2004
	Inputs:		pSrcInfo32			Ptr to 32bpp source bitmap info.
				pSrcBits32			Ptr to 32bpp source bitmap bits.
				fContour			Contour size, in pixels.
				uThreshold			Grey levels greater than or equal to this are considered transparent.
				ppDestInfo8			Ptr to ptr to 32bpp destination bitmap info.
				ppDestBits8			Ptr to ptr to 32bpp destination bitmap bits.
				pnOffsetX,pnOffsetY	On return, set to amount by which the destination bitmap must be
									offset in comparison to the source bitmap.

	Outputs:	true if succesful.

	Purpose:	Contour the given source bitmap and, putting the result into the destination
				bitmap. Transparency is removed from the source bitmap, although antialiasing is
				retained.
	Errors:		
	See also:	

**************************************************************************************************************************/

//#define DISPLAY_CONTOUR
#define ALLOW_HALF_OFFSET

bool CBitmapShadow::ContourBitmap(
						const LPBITMAPINFO	pSrcInfo32,
						const LPBYTE		pSrcBits32,
#if _DEBUG
							  double		fContour,
#else
						const double		fContour,
#endif
						const BYTE			uThreshold,
						LPBITMAPINFO*		ppDestInfo32,
						LPBYTE*				ppDestBits32,
						double*				pfOffsetX,
						double*				pfOffsetY
					)
{
#if _DEBUG
	if ( fContour>MAX_CONTOUR )
		fContour = MAX_CONTOUR ;
	else if ( fContour<0.1 )
		fContour = 0.1 ;
#else
	if ( fContour>MAX_CONTOUR )
		return false ;
#endif

#ifdef ALLOW_HALF_OFFSET
	const UINT32 uContour = UINT32(ceil(2*fContour)) ;
#else
	const UINT32 uContour = UINT32(ceil(fContour)*2) ;
#endif

	UINT32 uSrcWidth32  = pSrcInfo32->bmiHeader.biWidth ;
	UINT32 uSrcHeight32 = pSrcInfo32->bmiHeader.biHeight ;
	/*UINT32 uSrcByteWidth32 =*/ DIBUtil::ScanlineSize(uSrcWidth32,32) ;
	//
	// Create bitmap that is copy of the source T channel but with
	// a 1 pixel transparent surround.
	//
	// All pixels greater than uThreshold are made fully transparent.
	//
	LPBYTE pSBits8 ;
	LPBITMAPINFO pSInfo8 = ::AllocDIB(uSrcWidth32+2,uSrcHeight32+2,8,&pSBits8) ;
	if ( !pSInfo8 )
		return false ;
	UINT32 uSWidth8  = pSInfo8->bmiHeader.biWidth ;
	UINT32 uSHeight8 = pSInfo8->bmiHeader.biHeight ;
	UINT32 uSByteWidth8 = DIBUtil::ScanlineSize(uSWidth8,8) ;
	memset(pSBits8,0xff,uSByteWidth8*uSHeight8) ;

	LPBYTE pSrc = pSrcBits32+3 ;
	LPBYTE pDst = pSBits8+uSByteWidth8+1 ;
	UINT32 i,j ;
	for ( j=0 ; j<uSrcHeight32 ; j++ )
	{
		for ( i=0 ; i<uSrcWidth32 ; i++ )
		{
			if ( *pSrc<uThreshold )
				pDst[i] = *pSrc ;
			pSrc += 4 ;
		}
		pDst += uSByteWidth8 ;
	}

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pSInfo8,pSBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 1") ;
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif
	//
	// Create bitmap that is the size of the source but with
	// a 1 pixel surround. Pixels are set if they have four non-transparent
	// neighbours.
	//
	LPBYTE pNBits8 ;
	LPBITMAPINFO pNInfo8 = ::AllocDIB(uSWidth8,uSHeight8,8,&pNBits8) ;
	if ( !pNInfo8 )
	{
		::FreeDIB(pSInfo8,pSBits8) ;
		return false ;
	}
	memset(pNBits8,0x00,uSByteWidth8*uSHeight8) ;

	pSrc = pSBits8+uSByteWidth8+1 ;
	pDst = pNBits8+uSByteWidth8+1 ;
	for ( j=0 ; j<uSrcHeight32 ; j++ )
	{
		LPBYTE p = pSrc ;
		for ( i=0 ; i<uSrcWidth32 ; i++ )
		{
			bool c0 = p[-1]				    !=0xff ;
			bool c1 = p[+1]				    !=0xff ;
			bool c2 = p[-(INT32)uSByteWidth8]!=0xff ;
			bool c3 = p[+(INT32)uSByteWidth8]!=0xff ;
			p++ ;
			pDst[i] = -INT32(c0+c1+c2+c3==4) ;
		}
		pSrc += uSByteWidth8 ;
		pDst += uSByteWidth8 ;
	}

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pNInfo8,pNBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		(UINT32&)pPalette[0x00] = 0x00000000 ;
		(UINT32&)pPalette[0xff] = 0x00ffffff ;
		for ( i=0x01 ; i<0xff ; i++ )
			(UINT32&)pPalette[i] = 0x000000ff ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 2");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif
	//
	// Now create bitmap that is the size of the source bitmap but with a border of
	// uContour-1 transparent pixels around it. Insert the last bitmap but make
	// semi-transparent areas opaque but preserve antialiasing. We recognise semi-transparent
	// areas as having 4 non-transparent neighbours and antialiasing as having from 1 to 3
	// non-transparent neighbours.
	//
	LPBYTE pBits8 ;
	LPBITMAPINFO pInfo8 = ::AllocDIB(uSrcWidth32+uContour*2,uSrcHeight32+uContour*2,8,&pBits8) ;
	if ( !pInfo8 )
	{
		::FreeDIB(pSInfo8,pSBits8) ;
		::FreeDIB(pNInfo8,pNBits8) ;
		return false ;
	}
	UINT32 uWidth8  = pInfo8->bmiHeader.biWidth ;
	UINT32 uHeight8 = pInfo8->bmiHeader.biHeight ;
	UINT32 uByteWidth8 = DIBUtil::ScanlineSize(uWidth8,8) ;

	memset(pBits8,0xff,uByteWidth8*uHeight8) ;

	LPBYTE pSSrc = pSBits8+uSByteWidth8+1 ;
	LPBYTE pNSrc = pNBits8+uSByteWidth8+1 ;
		   pDst  =  pBits8+(uByteWidth8+1)*uContour ;
	for ( j=0 ; j<uSrcHeight32 ; j++ )
	{
		BYTE* pSBits = pSSrc ;
		BYTE* pNBits = pNSrc ;
		for ( i=0 ; i<uSrcWidth32 ; i++ )
		{
			BYTE b = *pSBits ;
			if ( b && b!=0xff )
			{
				BYTE c0 = pNBits[-1] ;
				BYTE c1 = pNBits[+1] ;
				BYTE c2 = pNBits[-(INT32)uSByteWidth8] ;
				BYTE c3 = pNBits[+(INT32)uSByteWidth8] ;
				UINT32 c = -(c0+c1+c2+c3) & 0xff ;
				if ( c==4 )
					b = 0x00 ;
				else if ( c )
				{
					UINT32 t = (c0 & pSBits[-1				])+
							 (c1 & pSBits[+1				])+
							 (c2 & pSBits[-(INT32)uSByteWidth8])+
							 (c3 & pSBits[+(INT32)uSByteWidth8]) ;
					if ( t )
					{
						static const UINT32 aMul[5] = {0*0xff,1*0xff,2*0xff,3*0xff,4*0xff} ;
						t = aMul[c]-t ;
						if ( t )
						{
							c = (b^0xff)*aMul[c]/t ;		// Could use a float table to replace divide by multiply.
							b = ~c ;
							if ( c>0xff )
								b = 0x00 ;
						}
					}
				}
			}
			pSBits++ ;
			pNBits++ ;
			pDst[i] = b ;
		}
		pSSrc += uSByteWidth8 ;
		pNSrc += uSByteWidth8 ;
		pDst  +=  uByteWidth8 ;
	}
	::FreeDIB(pSInfo8,pSBits8) ;
	::FreeDIB(pNInfo8,pNBits8) ;

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pInfo8,pBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 3");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif

#if 0	
	*ppDestInfo8 = pInfo8 ;
	*ppDestBits8 = pBits8 ;
	*pfOffsetX = -uContour ;
	*pfOffsetY = -uContour ;
#else
	//
	// Now create bitmap that is the size of the source but with a border of half the
	// contour size. This is the bitmap in which we will create the contoured image.
	//
	LPBYTE pDBits8 ;
	LPBITMAPINFO pDInfo8 = ::AllocDIB(uSrcWidth32+uContour,uSrcHeight32+uContour,8,&pDBits8) ;
	if ( !pDInfo8 )
	{
		::FreeDIB(pInfo8,pBits8) ;
		return false ;
	}
	{
		CamProfile cp(CAMPROFILE_SHADOW);
		::ContourBitmap(pBits8,uByteWidth8, pDBits8,pDInfo8, fContour,uContour);
	}
	::FreeDIB(pInfo8,pBits8) ;

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pDInfo8,pDBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 4");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif
	//
	// Now create final bitmap. This is simply a 32bpp copy of the last bitmap.
	//
	UINT32 uDWidth8  = pDInfo8->bmiHeader.biWidth ;
	UINT32 uDHeight8 = pDInfo8->bmiHeader.biHeight ;
	UINT32 uDByteWidth8 = DIBUtil::ScanlineSize(uDWidth8,8) ;

	LPBYTE pDBits32 ;
	LPBITMAPINFO pDInfo32 = ::AllocDIB(uDWidth8,uDHeight8,32,&pDBits32) ;
	if ( !pDInfo32 )
	{
		::FreeDIB(pDInfo8,pDBits8) ;
		return false ;
	}
	memset(pDBits32,0x00,uDWidth8*uDHeight8<<2) ;

	pSrc = pDBits8 ;
	pDst = pDBits32+3 ;
	for ( j=0 ; j<uDHeight8 ; j++ )
	{
		for ( i=0 ; i<uDWidth8 ; i++ )
		{
			*pDst = pSrc[i] ;
			pDst += 4 ;
		}
		pSrc += uDByteWidth8 ;
	}

	::FreeDIB(pDInfo8,pDBits8) ;

	*ppDestInfo32 = pDInfo32 ;
	*ppDestBits32 = pDBits32 ;
	*pfOffsetX = -0.5*uContour ;
	*pfOffsetY = -0.5*uContour ;
#endif
	return true ;
}

/***************************************************************************************************************************

>	static bool CBitmapShadow::ContourBitmap(
						const LPBITMAPINFO	pSrcInfo8,
						const LPBYTE		pSrcBits8,
						double				fContour,
						const bool			bInside,
						const BYTE			uThreshold,
						LPBITMAPINFO*		ppDestInfo8,
						LPBYTE*				ppDestBits8,
						INT32*				pnOffsetX,
						INT32*				pnOffsetY
					)
	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/2005
	Inputs:		pSrcInfo8			Ptr to 8bpp source bitmap info.
				pSrcBits8			Ptr to 8bpp source bitmap bits.
				fContour			Contour size, in pixels.
				bInside				Set to contour inside rather than outside the image
				uThreshold			Grey levels greater than or equal to this are considered transparent.
				ppDestInfo8			Ptr to ptr to 8bpp destination bitmap info.
				ppDestBits8			Ptr to ptr to 8bpp destination bitmap bits.
				pnOffsetX,pnOffsetY	On return, set to amount by which the destination bitmap must be
									offset in comparison to the source bitmap.

	Outputs:	true if succesful.

	Purpose:	Contour the given source bitmap and, putting the result into the destination
				bitmap. Transparency is removed from the source bitmap, although antialiasing is
				retained.

				Even if successful the destination info and bitmap may be returned as NULL if the
				result is empty. This is only possible if contouring inside.

	Errors:		
	See also:	

**************************************************************************************************************************/

//#define DISPLAY_CONTOUR
#define ALLOW_HALF_OFFSET

bool CBitmapShadow::ContourBitmap(
						const LPBITMAPINFO	pSrcInfo8,
						const LPBYTE		pSrcBits8,
#if _DEBUG
							  double		fContour,
#else
						const double		fContour,
#endif
						const bool			bInside,
						const BYTE			uThreshold,
						LPBITMAPINFO*		ppDestInfo8,
						LPBYTE*				ppDestBits8,
						double*				pfOffsetX,
						double*				pfOffsetY
					)
{
#if _DEBUG
	if ( fContour>MAX_CONTOUR )
		fContour = MAX_CONTOUR ;
	else if ( fContour<0.1 )
		fContour = 0.1 ;
#else
	if ( fContour>MAX_CONTOUR )
		return false ;
#endif

#ifdef ALLOW_HALF_OFFSET
	const UINT32 uContour = UINT32(ceil(2*fContour)) ;
#else
	const UINT32 uContour = UINT32(ceil(fContour)*2) ;
#endif

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR) && 0
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pSrcInfo8,pSrcBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( UINT32 i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap source");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif

	UINT32 uSrcWidth8  = pSrcInfo8->bmiHeader.biWidth ;
	UINT32 uSrcHeight8 = pSrcInfo8->bmiHeader.biHeight ;
	UINT32 uSrcByteWidth8 = DIBUtil::ScanlineSize(uSrcWidth8,8) ;

	if ( bInside && (uSrcWidth8<=uContour || uSrcHeight8<=uContour) )
	{
		*ppDestInfo8 = NULL ;
		*ppDestBits8 = NULL ;
		*pfOffsetX = 0 ;
		*pfOffsetY = 0 ;
		return true ;
	}
	//
	// Create bitmap that is copy of the source but with
	// a 1 pixel transparent surround.
	//
	// All pixels greater than uThreshold are made fully transparent.
	//
	LPBYTE pSBits8 ;
	LPBITMAPINFO pSInfo8 = ::AllocDIB(uSrcWidth8+2,uSrcHeight8+2,8,&pSBits8) ;
	if ( !pSInfo8 )
		return false ;
	UINT32 uSWidth8  = pSInfo8->bmiHeader.biWidth ;
	UINT32 uSHeight8 = pSInfo8->bmiHeader.biHeight ;
	UINT32 uSByteWidth8 = DIBUtil::ScanlineSize(uSWidth8,8) ;

	LPBYTE pSrc = pSrcBits8 ;
	LPBYTE pDst =   pSBits8+uSByteWidth8+1 ;
	UINT32 i,j ;
	memset(pSBits8,0xff,uSByteWidth8*uSHeight8) ;
	for ( j=0 ; j<uSrcHeight8 ; j++ )
	{
		for ( i=0 ; i<uSrcWidth8 ; i++ )
			if ( pSrc[i]<uThreshold )
				pDst[i] = pSrc[i] ;
		pSrc += uSrcByteWidth8 ;
		pDst +=   uSByteWidth8 ;
	}

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pSInfo8,pSBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 1") ;
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif
	//
	// Create bitmap that is the size of the source but with
	// a 1 pixel surround. Pixels are set if they have four non-transparent
	// neighbours.
	//
	LPBYTE pNBits8 ;
	LPBITMAPINFO pNInfo8 = ::AllocDIB(uSWidth8,uSHeight8,8,&pNBits8) ;
	if ( !pNInfo8 )
	{
		::FreeDIB(pSInfo8,pSBits8) ;
		return false ;
	}
	memset(pNBits8,0x00,uSByteWidth8*uSHeight8) ;
	pSrc = pSBits8+uSByteWidth8+1 ;
	pDst = pNBits8+uSByteWidth8+1 ;
	for ( j=0 ; j<uSrcHeight8 ; j++ )
	{
		LPBYTE p = pSrc ;
		for ( i=0 ; i<uSrcWidth8 ; i++ )
		{
			INT32 c0 = p[-1]				  !=0xff ;
			INT32 c1 = p[+1]				  !=0xff ;
			INT32 c2 = p[-(INT32)uSByteWidth8]!=0xff ;
			INT32 c3 = p[+(INT32)uSByteWidth8]!=0xff ;
			p++ ;
			pDst[i] = -INT32(c0+c1+c2+c3==4) ;
		}
		pSrc += uSByteWidth8 ;
		pDst += uSByteWidth8 ;
	}

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pNInfo8,pNBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		(UINT32&)pPalette[0x00] = 0x00000000 ;
		(UINT32&)pPalette[0xff] = 0x00ffffff ;
		for ( i=0x01 ; i<0xff ; i++ )
			(UINT32&)pPalette[i] = 0x000000ff ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 2");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif
	//
	// Now create bitmap that is the size of the source bitmap but with a border of
	// uContour-1 transparent pixels around it. Insert the last bitmap but make
	// semi-transparent areas opaque but preserve antialiasing. We recognise semi-transparent
	// areas as having 4 non-transparent neighbours and antialiasing as having from 1 to 3
	// non-transparent neighbours.
	//
	LPBYTE pBits8 ;
	LPBITMAPINFO pInfo8 = bInside ?
		::AllocDIB(uSrcWidth8			,uSrcHeight8		   ,8,&pBits8) :
		::AllocDIB(uSrcWidth8+uContour*2,uSrcHeight8+uContour*2,8,&pBits8) ;
	if ( !pInfo8 )
	{
		::FreeDIB(pSInfo8,pSBits8) ;
		::FreeDIB(pNInfo8,pNBits8) ;
		return false ;
	}
	UINT32 uWidth8  = pInfo8->bmiHeader.biWidth ;
	UINT32 uHeight8 = pInfo8->bmiHeader.biHeight ;
	UINT32 uByteWidth8 = DIBUtil::ScanlineSize(uWidth8,8) ;

	if ( !bInside )
		memset(pBits8,0xff,uByteWidth8*uHeight8) ;

	LPBYTE pSSrc = pSBits8+uSByteWidth8+1 ;
	LPBYTE pNSrc = pNBits8+uSByteWidth8+1 ;
		   pDst  = bInside ?
					pBits8 :
					pBits8+(uByteWidth8+1)*uContour ;
	for ( j=0 ; j<uSrcHeight8 ; j++ )
	{
		BYTE* pSBits = pSSrc ;
		BYTE* pNBits = pNSrc ;
		for ( i=0 ; i<uSrcWidth8 ; i++ )
		{
			BYTE b = *pSBits ;
			if ( b && b!=0xff )
			{
				BYTE c0 = pNBits[-1] ;
				BYTE c1 = pNBits[+1] ;
				BYTE c2 = pNBits[-(INT32)uSByteWidth8] ;
				BYTE c3 = pNBits[+(INT32)uSByteWidth8] ;
				UINT32 c = -(c0+c1+c2+c3) & 0xff ;
				if ( c==4 )
					b = 0x00 ;
				else if ( c )
				{
					UINT32 t = (c0 & pSBits[-1					])+
							   (c1 & pSBits[+1					])+
							   (c2 & pSBits[-(INT32)uSByteWidth8])+
							   (c3 & pSBits[+(INT32)uSByteWidth8]) ;
					if ( t )
					{
						static const UINT32 aMul[5] = {0*0xff,1*0xff,2*0xff,3*0xff,4*0xff} ;
						t = aMul[c]-t ;
						if ( t )
						{
							c = (b^0xff)*aMul[c]/t ;		// Could use a float table to replace divide by multiply.
							b = ~c ;
							if ( c>0xff )
								b = 0x00 ;
						}
					}
				}
			}
			pSBits++ ;
			pNBits++ ;
			pDst[i] = b ;
		}
		pSSrc += uSByteWidth8 ;
		pNSrc += uSByteWidth8 ;
		pDst  +=  uByteWidth8 ;
	}
	::FreeDIB(pSInfo8,pSBits8) ;
	::FreeDIB(pNInfo8,pNBits8) ;

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pInfo8,pBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 3");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif

	if ( bInside )
	{
		UINT32 uSize = uByteWidth8*uHeight8>>2 ;
		DWORD* p = (DWORD*)pBits8 ;
		for ( i=0 ; i<uSize ; i++ )
			p[i] = ~p[i] ;
	#if defined(_DEBUG) && defined(DISPLAY_CONTOUR) && 0
		{
			WinBitmap* pwbmpSBitmap = new WinBitmap(pInfo8,pBits8) ;
			KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
			LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
			for ( i=0 ; i<0x100 ; i++ )
				(UINT32&)pPalette[i] = i*0x010101 ;
			pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 3I");
			pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
			delete pkbmpSBitmap ;
		}
	#endif
	}

	//
	// Now create bitmap that is the size of the source but with a border of half the
	// contour size. This is the bitmap in which we will create the contoured image.
	//
	LPBYTE pDBits8 ;
	LPBITMAPINFO pDInfo8 = ::AllocDIB(uWidth8-uContour,uHeight8-uContour,8,&pDBits8) ;
	if ( !pDInfo8 )
	{
		::FreeDIB(pInfo8,pBits8) ;
		return false ;
	}
	{
		CamProfile cp(CAMPROFILE_SHADOW);
		::ContourBitmap(pBits8,uByteWidth8, pDBits8,pDInfo8, fContour,uContour);
	}
	::FreeDIB(pInfo8,pBits8) ;

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR)
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pDInfo8,pDBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 4");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif

	UINT32 uDWidth8  = pDInfo8->bmiHeader.biWidth ;
	UINT32 uDHeight8 = pDInfo8->bmiHeader.biHeight ;
	UINT32 uDByteWidth8 = DIBUtil::ScanlineSize(uDWidth8,8) ;

	if ( bInside )
	{
		UINT32 uSize = uDByteWidth8*uDHeight8>>2 ;
		DWORD* p = (DWORD*)pDBits8 ;
		for ( i=0 ; i<uSize ; i++ )
			p[i] = ~p[i] ;
	}

#if defined(_DEBUG) && defined(DISPLAY_CONTOUR) && 0
	{
		WinBitmap* pwbmpSBitmap = new WinBitmap(pDInfo8,pDBits8) ;
		KernelBitmap* pkbmpSBitmap = new KernelBitmap(pwbmpSBitmap,TRUE) ;
		LPRGBQUAD pPalette = pkbmpSBitmap->GetPaletteForBitmap();
		for ( i=0 ; i<0x100 ; i++ )
			(UINT32&)pPalette[i] = i*0x010101 ;
		pkbmpSBitmap->AttachDebugCopyToCurrentDocument("Contour bitmap 5");
		pwbmpSBitmap->BMBytes = ((WinBitmap*)OILBitmap::Default)->BMBytes ;
		delete pkbmpSBitmap ;
	}
#endif

	*ppDestInfo8 = pDInfo8 ;
	*ppDestBits8 = pDBits8 ;
	*pfOffsetX = (bInside?+0.5:-0.5)*uContour ;
	*pfOffsetY = (bInside?+0.5:-0.5)*uContour ;

	return true ;
}

/******************************************************************************************************/
/******************************************************************************************************/
/******************************************************************************************************/
//////////////////////////////////////////////////////////////////////
// Dummy class to maintain consistency between WebStyle and Camelot
// DMc 9/6/99

/********************************************************************************************
>	ShadowBitmap::ShadowBitmap()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/99
	Inputs:		-
	Returns:	
	Purpose:	Constructor
********************************************************************************************/

ShadowBitmap::ShadowBitmap()
{
	m_Bytes = NULL;
	m_Width = 0;
	m_Height = 0;
	m_Depth = 0;
}

/********************************************************************************************
>	ShadowBitmap::~ShadowBitmap()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/99
	Inputs:		-
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
ShadowBitmap::~ShadowBitmap()
{
	if (m_Bytes)
	{
		delete [] m_Bytes;
		m_Bytes = NULL;
	}
}

/********************************************************************************************
>	BOOL ShadowBitmap::CreateFilled(INT32 Width, INT32 Height, UINT32 value);

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/99
	Inputs:		-
	Returns:	
	Purpose:	Creates a filled bitmap of the given value
********************************************************************************************/
BOOL ShadowBitmap::CreateFilled(INT32 Width, INT32 Height, BYTE value, UINT32 Depth)
{
	m_Depth = Depth;
	
	if (m_Bytes)
	{
		delete m_Bytes;
		m_Bytes = NULL;
	}

	INT32 ScanlineWidth = DIBUtil::ScanlineSize(Width, Depth);
	INT32 Size		  = ScanlineWidth * Height;

	m_Bytes = new BYTE[Size];

	if (!m_Bytes)
		return FALSE;

	m_Width = Width;
	m_Height = Height;

	memset(m_Bytes,value,Size) ;

	return TRUE;
}

/********************************************************************************************
>	BOOL ShadowBitmap::Create(INT32 Width, INT32 Height);

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/99
	Inputs:		-
	Returns:	
	Purpose:	Creates a filled bitmap of the given value
********************************************************************************************/
HRESULT ShadowBitmap::Create(INT32 Width, INT32 Height, UINT32 Depth)
{
	m_Depth = Depth;
	
	if (m_Bytes)
	{
		delete m_Bytes;
		m_Bytes = NULL;
	}

	INT32 ScanlineWidth = DIBUtil::ScanlineSize(Width, Depth);
	INT32 Size		  = ScanlineWidth * Height;

	m_Bytes = new BYTE[Size];

	if (!m_Bytes)
		return FALSE;

	m_Width = Width;
	m_Height = Height;

	return S_OK;
}

/********************************************************************************************
>	BOOL ShadowBitmap::Create8BitBitmap(UINT32 * pBits, UINT32 Width, UINT32 Height)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/99
	Inputs:		-
	Returns:	
	Purpose:	Creates an 8 bit bitmap from a 32 bit bitmap (out of the alpha channel)
********************************************************************************************/

BOOL ShadowBitmap::Create8BitBitmap(UINT32 * pBits, UINT32 Width, UINT32 Height)
{
	CreateFilled(Width, Height, 0xff, 8);

	INT32 Width8 = DIBUtil::ScanlineSize(Width, 8);

	BYTE* pSrc = (BYTE *)pBits + 3;
	BYTE* pDest = GetBytes();
	BYTE* pSrcNextLine  = NULL;
	BYTE* pDestNextLine = NULL;
	UINT32 j = 0;

	for ( UINT32 i = 0; i < Height; i++)
	{
		pSrcNextLine  = pSrc + Width*4 ;
		pDestNextLine = pDest+ Width8 ;

		for ( j=0 ; j<Width; j++ )
		{
			*pDest = *pSrc;
			pDest ++;
			pSrc += 4;
		}
		pSrc = pSrcNextLine;
		pDest = pDestNextLine;
	}

	return TRUE;
}

/******************************************************************************************************/

#endif
