// $Id$
/////////////////////////////////////////////////
// the bitmap beveler (for inner bevels)
// DMc 2/6/98
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
// camconfig.h must be included immediately after camtypes.h
#include "camconfig.h"
//#include "app.h"
//#include "bmanip.h"
//#include "paths.h"
#include "fixmem.h"
//#include "wincoord.h"
//#include "bitmap.h"
//#include "rndrgn.h"
//#include "xadrwold.h"
//#include "dibutil.h"

#include "beveler.h"
//#include "gclips.h"
//#include "bshadow.h"
//#include "biasgain.h"

#include "gdraw2.h"
//#include "errors.h"
//#include "mario.h"

#include "cstroke.h"

//#include "gdrawasm.h"
#include "grndrgn.h"

#include "camprofile.h"

#include <math.h>
#include <stdio.h>

#define new CAM_DEBUG_NEW

//////////////////////////////////////////////////
// These two defines are the max width and height values for the beveling block sizes!
// They are both pixel sizes and DO NOT represent the scaled values!
#define BEVELMAXBITMAPWIDTH		1024
#define BEVELMAXBITMAPHEIGHT	128

// 256 * 16 = 4096 pixels and scaled by 4 = 1024 * 64 = 65536 pixels!!!

CC_IMPLEMENT_DYNAMIC(CBeveler, CCObject)

/********************************************************************************************

>	CBeveler::CBeveler

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/98
	Purpose:	Constructor for the beveling tool
				Sets up the class read to do beveling
	SeeAlso:	-

********************************************************************************************/

CBeveler::CBeveler()
{
	// draw out the path to bevel from the selection
	m_pBevelPath = NULL;
	m_Width  =
	m_Height = 0;
	m_bOuter = FALSE;
	m_JointType = RoundJoin;
	m_pFaceList = NULL;
	m_NumFaces = 0;
	m_pBevelPoints = NULL ;
	m_pBevelTypes = NULL ;
	m_nBevelLength = 0 ;
	m_Tilt = 32.0;
	m_pStrip32 = NULL;
	m_pMaskBitmap = NULL;
	m_pNewMask = NULL;
	m_Indent = 750;
	m_Contrast = 50;
	m_BevelType = 1;
	m_LightAngle = 45;
	m_Transparency = 0;
	m_dBmpToWinX = 300;
	m_dBmpToWinY = 300;
}


CBeveler::CBeveler(Path * pPath)
{
	// draw out the path to bevel from the selection
	m_pBevelPath = pPath;
	m_Width  =
	m_Height = 0;
	m_bOuter = FALSE;
	m_JointType = RoundJoin;
	m_pFaceList = NULL;
	m_NumFaces = 0;
	m_pBevelPoints = NULL ;
	m_pBevelTypes = NULL ;
	m_nBevelLength = 0 ;
	m_Tilt = 45.0;
	m_pStrip32 = NULL;
	m_pMaskBitmap = NULL;
	m_pNewMask = NULL;
	m_Indent = 750;
	m_Contrast = 50;
	m_BevelType = 1;
	m_LightAngle = 45;
	m_Tilt = 45;
	m_Transparency = 0;
	m_dBmpToWinX = 300;
	m_dBmpToWinY = 300;
}

/********************************************************************************************

>	CBeveler::~CBeveler

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/98
	Purpose:	Destructor
	SeeAlso:	-

********************************************************************************************/
CBeveler::~CBeveler()
{
	if (m_pFaceList)
	{
		delete [] m_pFaceList;
		m_pFaceList = NULL;
	}

	if(m_pStrip32)
	{
		delete m_pStrip32;
		m_pStrip32 = NULL;
	}

	if(m_pMaskBitmap)
	{
		delete m_pMaskBitmap;
		m_pMaskBitmap = NULL;
	}

	if(m_pNewMask)
	{
		delete m_pNewMask;
		m_pNewMask = NULL;
	}

	CCFree(m_pBevelPoints);
	CCFree(m_pBevelTypes);
}

/********************************************************************************************

>	BOOL SetUpBeveler(MILLIPOINT Indent, JointType jtype, NormCoord * lightVec, BOOL bOuter,
					  INT32 Contrast, RenderRegion * pRegion, MILLIPOINT Flatness)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/07/00 - Re-write of david code
	Purpose:	Sets up the beveler with the new bevel details
	Returns:	TRUE for success, FALSE if something went wrong!

********************************************************************************************/
BOOL CBeveler::SetUpBeveler(MILLIPOINT Indent, JointType jtype,
							NormCoord * lightVec, BOOL bOuter,
							INT32 Contrast, RenderRegion * pRegion,
							MILLIPOINT Flatness)
{	
	// Check to see if the indent is zero. This is not bad, but just means there`s no bitmap needed!
	if (Indent == 0)
		return TRUE;

	m_Contrast = Contrast;
	m_JointType = jtype;
	m_Indent = Indent;
	m_bOuter = bOuter;

	// draw the bevel
	BEVEL_FACE* pFaces = NULL;
	GenBevelFaces gbf;

	m_pBevelPath->GetTrueBoundingRect(&m_BevelBounds);

	// Calculate the face list
	UINT32 nFaces = gbf.BevelPath(
		(POINT*)m_pBevelPath->GetCoordArray(),
		m_pBevelPath->GetVerbArray(),
		m_pBevelPath->GetNumCoords(),
		&pFaces,
		Indent<<1,
		10<<16,
		Flatness>>3,
		(JoinStyles)jtype,
		bOuter==TRUE
	) ;

	// Check to see if we had an error!
	if ( nFaces<0 || pFaces == NULL)
		return FALSE;

	// Copy the return pointer into the member variable
	m_pFaceList = pFaces;
	m_NumFaces = nFaces;

	// Work out the size of the bevel
	CalcSelectionBounds();

	return TRUE;
}

BOOL CBeveler::SetBevelerSize(INT32 Width, INT32 Height)
{
	m_Width  = Width *BEVELBITMAPSCALE;
	m_Height = Height*BEVELBITMAPSCALE;

	SetUpMapping() ;

	BEVEL_FACE* pFace = m_pFaceList ;
	for ( UINT32 i=0 ; i<m_NumFaces ; i++ )
	{
		UINT32 n = pFace->bTriangle ? 3 : 4 ;
		for ( UINT32 j=0 ; j<n ; j++ )
			ToWinCoord((DocCoord*)&(pFace->aFace[j]),(WinCoord*)&(pFace->aFace[j])) ;
		pFace++ ;
	}

	// Now work out the inner path
	CalcInnerPath() ;

	m_Width  = Width;
	m_Height = Height;

	return TRUE;
}

/********************************************************************************************

>	BOOL CBeveler::RenderInStripsToBitmap(KernelBitmap * pBitmap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99

	Inputs:		
	Returns:	
				
	Purpose:	Renders the bitmap in strips
	Errors:		FALSE for failure
	SeeAlso:	-

********************************************************************************************/
BOOL CBeveler::RenderInStripsToBitmap(KernelBitmap * pBitmap, DocCoord* pSubPixOffSet)
{
	DocCoord SubPixOffset(0,0);

	if(pSubPixOffSet != NULL)
		SubPixOffset = *pSubPixOffSet;
	else
		ERROR3("Got a NULL DocCoord pointer! Using 0,0 coords!");

	if(m_pStrip32)
	{
		delete m_pStrip32;
		m_pStrip32 = NULL;
	}

	if(m_pMaskBitmap)
	{
		delete m_pMaskBitmap;
		m_pMaskBitmap = NULL;
	}

	if(m_pNewMask)
	{
		delete m_pNewMask;
		m_pNewMask = NULL;
	}

	INT32 width  = pBitmap->GetWidth();
	INT32 height = pBitmap->GetHeight();

	TRACEUSER( "GerryX", _T("RenderInStripsToBitmap: Beveler Width = %d, Height = %d\n"),width * 4,height * 4);

	// Now lets do some optimization!
	KernelBitmap* pRemainderStripBitmap = NULL;
	KernelBitmap* pFullStripBitmap = NULL;
	INT32 LargestBevelWidth = 0;
	INT32 LargestBevelHeight = BEVELMAXBITMAPHEIGHT;
	INT32 LeftOverWidth = width % BEVELMAXBITMAPWIDTH;
	
	// At this point we have a destination bitmap which is larger than the max size bmp for GDraw2!
	// What we need to do is render in strips. For this we require two bitmaps to be setup.
	// Bitmap 1 is of MaxBevelWidthin width and Bitmap2 is the modulus left over in width.
	if(width > BEVELMAXBITMAPWIDTH)
	{
		LargestBevelWidth = BEVELMAXBITMAPWIDTH;
//		INT32 DivideSize = 2;

		while(LeftOverWidth < (0.5 * LargestBevelWidth))
		{
			// Graeme (20/9/00) - Replaced the original calculation with one using integers, so
			// as to avoid the double -> INT32 warning message. Speedwise, there may be a bit of
			// a hit, as this version will take ~30 cycles, but there might be a win due to the
			// loss of an implicit conversion from a floating point number to an integer.
			//
			// The original calculation was LargestBevelWidth * 0.9
			LargestBevelWidth = ( ( LargestBevelWidth * 9 ) + 5 ) / 10;
			LeftOverWidth = width % LargestBevelWidth;
		}

		if(LeftOverWidth > 0)
			pRemainderStripBitmap = new KernelBitmap(LeftOverWidth, LargestBevelHeight, 8, 0, TRUE);
	}
	else
	{
		LeftOverWidth = 0;
		LargestBevelWidth = width;
	}

	pFullStripBitmap = new KernelBitmap(LargestBevelWidth, LargestBevelHeight, 8, 0, TRUE);

	ERROR2IF(pFullStripBitmap == NULL,FALSE,"Failed to create a strip bitmap pointer!");

	// we have to render to areas of the whole bitmap to then merge in
	for ( INT32 y = 0 ; y < height; y += LargestBevelHeight )
	{
		if(m_pStrip32 == NULL)
			m_pStrip32 = new KernelBitmap(LargestBevelWidth * BEVELBITMAPSCALE, LargestBevelHeight * BEVELBITMAPSCALE, 32, 96, TRUE);
		if(m_pMaskBitmap == NULL)
			m_pMaskBitmap = new KernelBitmap(LargestBevelWidth, LargestBevelHeight, 8, 96, TRUE);
		if(m_pNewMask == NULL)
			m_pNewMask = new KernelBitmap(LargestBevelWidth, LargestBevelHeight, 8, 96, TRUE);

		ERROR2IF(m_pStrip32 == NULL,FALSE,"Failed to create a m_pStrip32 pointer!");
		ERROR2IF(m_pMaskBitmap == NULL,FALSE,"Failed to create a m_pMaskBitmap pointer!");
		ERROR2IF(m_pNewMask == NULL,FALSE,"Failed to create a m_pNewMask pointer!");

		INT32 x;
		for( x = 0; x < width; x += LargestBevelWidth )
		{
			if (!RenderWithResampling(pFullStripBitmap, x, y, &SubPixOffset))
			{
				delete pFullStripBitmap;
				delete pRemainderStripBitmap;
				return FALSE;			
			}
			
			// copy the source bitmap into the destination bitmap at the correct place
			CopyBitmapIntoBitmap(pFullStripBitmap, pBitmap, x, y);
		}

		x -= LargestBevelWidth;

		if((width - x) == LeftOverWidth && pRemainderStripBitmap)
		{
			delete m_pStrip32;
			delete m_pMaskBitmap;
			delete m_pNewMask;

			m_pStrip32 = new KernelBitmap(LeftOverWidth * BEVELBITMAPSCALE, LargestBevelHeight * BEVELBITMAPSCALE, 32, 96, TRUE);
			ERROR2IF(m_pStrip32 == NULL,FALSE,"Failed to create a m_pStrip32 pointer!");
			m_pMaskBitmap = new KernelBitmap(LeftOverWidth, LargestBevelHeight, 8, 96, TRUE);
			ERROR2IF(m_pMaskBitmap == NULL,FALSE,"Failed to create a m_pMaskBitmap pointer!");
			m_pNewMask = new KernelBitmap(LeftOverWidth, LargestBevelHeight, 8, 96, TRUE);
			ERROR2IF(m_pNewMask == NULL,FALSE,"Failed to create a m_pNewMask pointer!");

			if (!RenderWithResampling(pRemainderStripBitmap, x, y, &SubPixOffset))
			{
				delete pFullStripBitmap;
				delete pRemainderStripBitmap;
				return FALSE;			
			}
			
			// copy the source bitmap into the destination bitmap at the correct place
			CopyBitmapIntoBitmap(pRemainderStripBitmap, pBitmap, x, y);

			delete m_pStrip32;
			m_pStrip32 = NULL;
			delete m_pMaskBitmap;
			m_pMaskBitmap = NULL;
			delete m_pNewMask;
			m_pNewMask = NULL;
		}
	}

	delete m_pStrip32;
	m_pStrip32 = NULL;
	delete m_pMaskBitmap;
	m_pMaskBitmap = NULL;
	delete m_pNewMask;
	m_pNewMask = NULL;

	// Now delete any memory we used
	delete pFullStripBitmap;

	if(pRemainderStripBitmap)
		delete pRemainderStripBitmap;

	return TRUE;
}

/********************************************************************************************

>	void CBeveler::ToWinCoord(const DocCoord * dc, WinCoord * wc)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/98
	Purpose:	Converts a win coord to a doc coord depending on current spread and view
	SeeAlso:	-

********************************************************************************************/
void CBeveler::ToWinCoord(const DocCoord * dc, WinCoord * wc)
{
	if(m_bOuter)
	{
		wc->x = (INT32)(m_dBmpToWinX*(dc->x-m_SelectionBounds.lo.x)) ;
		wc->y = (INT32)(m_dBmpToWinY*(dc->y-m_SelectionBounds.lo.y)) ;
	}
	else
	{
		wc->x = (INT32)(m_dBmpToWinX*(dc->x-m_BevelBounds.lo.x)) ;
		wc->y = (INT32)(m_dBmpToWinY*(dc->y-m_BevelBounds.lo.y)) ;
	}
	
	return ;
}

/********************************************************************************************

>	void CBeveler::SetUpMapping()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/9/98
	Purpose:	Sets up the scale factors used to change doc coordinates into bitmap coordinates
				and vice-versa
	SeeAlso:	-

********************************************************************************************/
void CBeveler::SetUpMapping()
{
	if(m_bOuter)
	{
		if(m_SelectionBounds.IsEmpty())
		{
			m_dBmpToWinX = 1.0;
			m_dBmpToWinY = 1.0;
		}
		else
		{
			m_dBmpToWinX = (double)(m_Width <<16)/m_SelectionBounds.Width () ;
			m_dBmpToWinY = (double)(m_Height<<16)/m_SelectionBounds.Height() ;
		}
	}
	else
	{
		if(m_BevelBounds.IsEmpty())
		{
			m_dBmpToWinX = 1.0;
			m_dBmpToWinY = 1.0;
		}
		else
		{
			m_dBmpToWinX = (double)(m_Width <<16)/m_BevelBounds.Width () ;
			m_dBmpToWinY = (double)(m_Height<<16)/m_BevelBounds.Height() ;
		}
	}
}

void CBeveler::CalcSelectionBounds()
{
	m_SelectionBounds.lo.x =
	m_SelectionBounds.lo.y = INT_MAX ;
	m_SelectionBounds.hi.x =
	m_SelectionBounds.hi.y = INT_MIN ;

	UINT32 i ;
	BEVEL_FACE* pFace = (BEVEL_FACE*)m_pFaceList ;
	if ( m_bOuter )
		for ( i=0 ; i<m_NumFaces ; i++ )
		{
			m_SelectionBounds.lo.x = min(m_SelectionBounds.lo.x,pFace->aFace[2].X) ;
			m_SelectionBounds.lo.y = min(m_SelectionBounds.lo.y,pFace->aFace[2].Y) ;
			m_SelectionBounds.hi.x = max(m_SelectionBounds.hi.x,pFace->aFace[2].X) ;
			m_SelectionBounds.hi.y = max(m_SelectionBounds.hi.y,pFace->aFace[2].Y) ;
			if ( !pFace->bTriangle )
			{
				m_SelectionBounds.lo.x = min(m_SelectionBounds.lo.x,pFace->aFace[3].X) ;
				m_SelectionBounds.lo.y = min(m_SelectionBounds.lo.y,pFace->aFace[3].Y) ;
				m_SelectionBounds.hi.x = max(m_SelectionBounds.hi.x,pFace->aFace[3].X) ;
				m_SelectionBounds.hi.y = max(m_SelectionBounds.hi.y,pFace->aFace[3].Y) ;
			}
			pFace++ ;
		}
	else
		for ( i=0 ; i<m_NumFaces ; i++ )
		{
			m_SelectionBounds.lo.x = min(m_SelectionBounds.lo.x,pFace->aFace[0].X) ;
			m_SelectionBounds.lo.y = min(m_SelectionBounds.lo.y,pFace->aFace[0].Y) ;
			m_SelectionBounds.hi.x = max(m_SelectionBounds.hi.x,pFace->aFace[0].X) ;
			m_SelectionBounds.hi.y = max(m_SelectionBounds.hi.y,pFace->aFace[0].Y) ;
			m_SelectionBounds.lo.x = min(m_SelectionBounds.lo.x,pFace->aFace[1].X) ;
			m_SelectionBounds.lo.y = min(m_SelectionBounds.lo.y,pFace->aFace[1].Y) ;
			m_SelectionBounds.hi.x = max(m_SelectionBounds.hi.x,pFace->aFace[1].X) ;
			m_SelectionBounds.hi.y = max(m_SelectionBounds.hi.y,pFace->aFace[1].Y) ;
			pFace++ ;
		}
}

void CBeveler::CalcInnerPath()
{
	CCFree(m_pBevelPoints);
	m_pBevelPoints = NULL;
	CCFree(m_pBevelTypes);
	m_pBevelTypes = NULL;

	if (m_NumFaces == 0)
	{
		ERROR3("CBeveler::CalcInnerPath has empty face list");
		return;
	}
	
	m_pBevelPoints = (POINT*)CCMalloc(sizeof(POINT) * m_NumFaces);
	m_pBevelTypes  = (BYTE*)CCMalloc(sizeof(BYTE) * m_NumFaces);

	GPOINT* pPoints = (GPOINT*)m_pBevelPoints ;
	BYTE*   pTypes  =		   m_pBevelTypes  ;

	GPOINT LastPoint(INT_MAX,INT_MAX) ;
	BEVEL_FACE* pFace = (BEVEL_FACE*)m_pFaceList ;
	for ( UINT32 i=0 ; i<m_NumFaces ; i++ )
	{
		if ( !pFace->bTriangle )
			if ( LastPoint!=pFace->aFace[0] )
			{
				*pTypes++ = PT_MOVETO ;
				*pPoints++ = LastPoint = pFace->aFace[1] ;
			}
			else if ( LastPoint!=pFace->aFace[1] )
			{
				*pTypes++ = PT_LINETO ;
				*pPoints++ = LastPoint = pFace->aFace[1] ;
			}
		pFace++ ;
	}
	m_nBevelLength = pTypes-m_pBevelTypes ;
}

/********************************************************************************************

>	BOOL CBeveler::RenderToBitmap(KernelBitmap * pBitmap, BOOL bResample)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/98
	Purpose:	Renders the bevel bitmap (after the initial render by CreateBevelBitmap)
				to a given bitmap (thus changes in light angle, type etc don't cause the
				beveler to regenerate the normal list etc). 
	Inputs:		The bitmap to render to - MUST be of the same size as m_Width and m_Height and
				8-bit
				Also Whether to resample the bitmap
	Returns:	TRUE for success
	SeeAlso:	-

********************************************************************************************/
BOOL CBeveler::RenderToBitmap(KernelBitmap * pRetnBitmap, BOOL bResample, DocCoord* pSubPixOffSet)
{
	TRACEUSER( "GerryX", _T("(Beveller) Entering Render To Bitmap ------- \n"));
	DocCoord SubPixOffset(0,0);

	if(m_Width == 0 || m_Height == 0)
		return FALSE;

#ifdef DEBUG
	DWORD Time = GetTickCount();
#endif
	if (m_NumFaces == 0)
	{
		ERROR3("Face list is empty");
		return FALSE;
	}

	if (pRetnBitmap->GetBPP() != 8)
	{
		ERROR3("Bitmap isn't 8 bit !");
		return FALSE;
	}

	if ( pRetnBitmap->GetWidth ()!=(UINT32)m_Width ||
		 pRetnBitmap->GetHeight()!=(UINT32)m_Height )
	{
		// we need to change the render list
		double XScale = (double)pRetnBitmap->GetWidth ()/m_Width  ;
		double YScale = (double)pRetnBitmap->GetHeight()/m_Height ;
		m_Width  = pRetnBitmap->GetWidth ();
		m_Height = pRetnBitmap->GetHeight();

		BEVEL_FACE* pFace = (BEVEL_FACE*)m_pFaceList ;
		for ( UINT32 i=0 ; i<m_NumFaces ; i++ )
		{
			UINT32 n = pFace->bTriangle ? 3 : 4 ;
			for ( UINT32 j=0 ; j<n ; j++ )
			{
				pFace->aFace[j].X = (INT32)(pFace->aFace[j].X*XScale);
				pFace->aFace[j].Y = (INT32)(pFace->aFace[j].Y*YScale);
			}
			pFace++ ;
		}
		CalcInnerPath() ;
	}

	// set up GDraw2
	// if we're not resampling, then deal with the 8 bit DIB directly
	if ( !bResample )
	{
		KernelBitmap* pBitmap = new KernelBitmap( m_Width, m_Height, 32, 96, TRUE );
		
		ERRORIF(pBitmap == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
		
		UINT32 nLength = m_Width*m_Height;
		DWORD* pBits = (DWORD*)pBitmap->GetBitmapBits() ;
		UINT32 i ;
		for ( i=0 ; i<nLength ; i++ )
#if defined(WORDS_BIGENDIAN)
			pBits[i] = 0xffff007f ;
#else
			pBits[i] = 0x7f00ffff ;
#endif	
	
		BITMAPINFOHEADER Header;
		Header.biWidth		 = m_Width ;
		Header.biHeight		 = m_Height ;
		Header.biPlanes		 = 1 ;
		Header.biBitCount	 = 32 ;
		Header.biCompression = BI_RGB ;
		
		INT32 Error = SetDIBitmap( &Header, (BYTE*)pBits ) ;
		if (Error != GERROR_NO_ERROR)
		{
			delete pBitmap;
			ERROR(_R(IDE_NOMORE_MEMORY), FALSE);
		}
		
		BEVEL_FACE* pFace = (BEVEL_FACE*)m_pFaceList ;
		for ( i=0 ; i<m_NumFaces ; i++ )
		{
			POINT Points[4] ;
			Points[0].x = pFace->aFace[0].X>>BEVELBITMAPSHIFT ;
			Points[0].y = pFace->aFace[0].Y>>BEVELBITMAPSHIFT ;
			Points[1].x = pFace->aFace[1].X>>BEVELBITMAPSHIFT ;
			Points[1].y = pFace->aFace[1].Y>>BEVELBITMAPSHIFT ;
			Points[2].x = pFace->aFace[2].X>>BEVELBITMAPSHIFT ;
			Points[2].y = pFace->aFace[2].Y>>BEVELBITMAPSHIFT ;
			if (pFace->bTriangle)
			{
				if ( Points[1].x!=Points[2].x ||
					 Points[1].y!=Points[2].y )
				{
					CamProfile cp(CAMPROFILE_GDRAW);
					GDraw2_FillTriangle(Points,pFace->Normal.X,pFace->Normal.Y);
				}
			}
			else
			{
				Points[3].x = pFace->aFace[3].X>>BEVELBITMAPSHIFT ;
				Points[3].y = pFace->aFace[3].Y>>BEVELBITMAPSHIFT ;
				CamProfile cp(CAMPROFILE_GDRAW);
				GDraw2_FillTrapezium(Points,pFace->Normal.X,pFace->Normal.Y);
			}
			pFace++ ;
		}

		// Create a new GDraw context to use as this is much quicker than
		// saving and restoring the static context
		GDrawAsm* pGD = new GDrawAsm;
		if (pGD)
		{
			if (pGD->Init() && pGD->SetupBitmap(Header.biWidth, Header.biHeight, 32, pBitmap->GetBitmapBits(), 0))
			{
				GMATRIX GMatrix ;
				GMatrix.AX = 1<<(FX-BEVELBITMAPSHIFT) ;
				GMatrix.AY = 0 ;
				GMatrix.BX = 0 ;
				GMatrix.BY = 1<<(FX-BEVELBITMAPSHIFT) ;
				GMatrix.CX = SubPixOffset.x ;	// Surely this is wrong!
				GMatrix.CY = SubPixOffset.y ;	// Convert to WinCoord and <<FX-BEVELBITMAPSHIFT.
				pGD->SetMatrix(&GMatrix) ;
				pGD->SetAntialiasFlag(FALSE) ;
#if defined(WORDS_BIGENDIAN)
				pGD->SetWordColour(0xffff007f) ;
#else
				pGD->SetWordColour(0x7f00ffff) ;
#endif
//				pGD->FillPath(m_pBevelPoints,m_pBevelTypes,m_nBevelLength,ALTERNATE|!m_bOuter<<1);
				pGD->FillPath(m_pBevelPoints,m_pBevelTypes,m_nBevelLength,!m_bOuter<<1);
			}
			delete pGD;
		}

		BYTE* pSrcPtr  = pBitmap->GetBitmapBits()+3 ;
		BYTE* pDestPtr = pRetnBitmap->GetBitmapBits() ;
		UINT32 nSize = DIBUtil::ScanlineSize(m_Width,8)-m_Width ;
		for ( UINT32 j=0 ; j<(UINT32)m_Height ; j++ )
		{
			for ( i=0 ; i<(UINT32)m_Width ; i++ )
			{		
				*pDestPtr++ = *pSrcPtr;
				pSrcPtr += 4;
			}
			pDestPtr += nSize ;
		}
		delete pBitmap;
	}
	else
	{
		// render with sampling
		if ( !RenderInStripsToBitmap(pRetnBitmap, &SubPixOffset) )
			return FALSE;		
	}

#ifdef DEBUG
	TRACEUSER( "GerryX", _T("Last Bevel Bitmap render = %dms\n"),GetTickCount() - Time);
#endif

	return TRUE;
}


/********************************************************************************************

>	INT32 CBeveler::SetDIBitmap( BITMAPINFOHEADER* pHeader, BYTE* pBits )

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/00
	Purpose:	Passes parameters to GDraw2_SetDIBitmap adjusted for inner bevelling.
	Inputs:		pHeader	- pointer to bitmap info header
				pBits	- pointer to bitmap bytes
	Returns:	
	SeeAlso:	-

********************************************************************************************/

INT32 CBeveler::SetDIBitmap( BITMAPINFOHEADER* pHeader, BYTE* pBits )
{
	if ( m_bOuter )
	{
		CamProfile cp(CAMPROFILE_GDRAW);
		return GDraw2_SetDIBitmap(
			pHeader,
			pBits,
			(eBevelStyle)m_BevelType,
			(float)m_LightAngle,
			(float)(90-m_Tilt)
		) ;
	}
	else
	{
		static const bool aFlipLightAngle[] = {
			false,	// BEVEL_FLAT
			true,	// BEVEL_ROUND
			true,	// BEVEL_HALFROUND
			true,	// BEVEL_FRAME
			false,	// BEVEL_MESA_1
			false,	// BEVEL_MESA_2
			false,	// BEVEL_SMOOTH_1
			false,	// BEVEL_SMOOTH_2
			true,	// BEVEL_POINT_1
			true,	// BEVEL_POINT_2a
			true,	// BEVEL_POINT_2b
			true,	// BEVEL_RUFFLE_1
			true	// BEVEL_RUFFLE_2
		} ;
		static const eBevelStyle aFlipBevels[] = {
			BEVEL_FLAT,		// BEVEL_FLAT
			BEVEL_ROUND,	// BEVEL_ROUND
			BEVEL_HALFROUND,// BEVEL_HALFROUND
			BEVEL_FRAME,	// BEVEL_FRAME
			BEVEL_SMOOTH_1,	// BEVEL_MESA_1
			BEVEL_SMOOTH_2,	// BEVEL_MESA_2
			BEVEL_MESA_1,	// BEVEL_SMOOTH_1
			BEVEL_MESA_2,	// BEVEL_SMOOTH_2
			BEVEL_POINT_1,	// BEVEL_POINT_1
			BEVEL_POINT_2b,	// BEVEL_POINT_2a
			BEVEL_POINT_2a,	// BEVEL_POINT_2b
			BEVEL_RUFFLE_2a,// BEVEL_RUFFLE_2a
			BEVEL_RUFFLE_2b,// BEVEL_RUFFLE_2b
			BEVEL_RUFFLE_3a,// BEVEL_RUFFLE_3a
			BEVEL_RUFFLE_3b	// BEVEL_RUFFLE_3b
		} ;
		CamProfile cp(CAMPROFILE_GDRAW);
		return GDraw2_SetDIBitmap(
			pHeader,
			pBits,
			aFlipBevels[m_BevelType],
			(float)(aFlipLightAngle[m_BevelType] ? 180+m_LightAngle : m_LightAngle),
			(float)(90-m_Tilt)
		) ;
	}
	return 0;
}

/********************************************************************************************

>	void CBeveler::ResampleBitmap(BYTE * p32BitDIB, UINT32 Width, UINT32 Height, BYTE * pDestBits,
								BYTE * pMaskBitmap)

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Purpose:	Resamples the 32 bit DIB given into the bits specified by pKB
				and pMaskBitmap
	Inputs:		p32BitDIB		- pointer to the DIB bits to resample
				Width, Height	- The width & height of the 32 bit DIB 
				pDestBits		- The bitmap to sample into
				pMaskBitmap		- Optional mask bitmap pointer (can be NULL)

				NB The mask bitmap returns with 0x00 indicating no pixel written to,
				and 0xff indicating pixel written to. This is to speed up the emboldening
				process

				Also, the memory allocation of pDestBits and pMaskBits must be
				Width * Height / (BEVELBITMAPSCALE * BEVELBITMAPSCALE)
				i.e. the width & height of the destination bitmaps are Width/BEVELBITMAPSCALE,
				Height/BEVELBITMAPSCALE
	Returns:	
	SeeAlso:	-

********************************************************************************************/
void CBeveler::ResampleBitmap(BYTE * p32BitDIB, UINT32 SrcWidth, UINT32 SrcHeight, BYTE *pDestBits,
							  BYTE * pMaskBits)
{
	CamProfile cp(CAMPROFILE_BEVEL);

	UINT32 Width  = SrcWidth /BEVELBITMAPSCALE;
	UINT32 Height = SrcHeight/BEVELBITMAPSCALE;
	
	UINT32 Scanline8  = DIBUtil::ScanlineSize(Width,8);
	UINT32 Scanline32 = Width*4 * BEVELBITMAPSCALE;

	static CONST UINT32 ScaleTable[17] = {
		0,
		0x101010/1,
		0x101010/2,
		0x101010/3,
		0x101010/4,
		0x101010/5,
		0x101010/6,
		0x101010/7,
		0x101010/8,
		0x101010/9,
		0x101010/10,
		0x101010/11,
		0x101010/12,
		0x101010/13,
		0x101010/14,
		0x101010/15,
		0x101010/16
	};

	BYTE*  pSrcPtr = p32BitDIB;
	BYTE* pDestPtr = pDestBits;
	BYTE* pMaskPtr = pMaskBits;

	for ( UINT32 j=0 ; j<Height ; j++ )
	{
		for ( UINT32 i=0 ; i<Width ; i++ )
		{
			UINT32 uCount = 0;
			UINT32 uTotal = 0;
			BYTE* pSrc = pSrcPtr;
			for ( UINT32 k=0 ; k<4 ; k++ )
			{
#if defined(WORDS_BIGENDIAN)
				if ( pSrc[0*4]!=0xff ) { uCount++; uTotal += pSrc[0*4+3]; }
				if ( pSrc[1*4]!=0xff ) { uCount++; uTotal += pSrc[1*4+3]; }
				if ( pSrc[2*4]!=0xff ) { uCount++; uTotal += pSrc[2*4+3]; }
				if ( pSrc[3*4]!=0xff ) { uCount++; uTotal += pSrc[3*4+3]; }
#else
				if ( pSrc[0*4+1]!=0xff ) { uCount++; uTotal += pSrc[0*4+3]; }
				if ( pSrc[1*4+1]!=0xff ) { uCount++; uTotal += pSrc[1*4+3]; }
				if ( pSrc[2*4+1]!=0xff ) { uCount++; uTotal += pSrc[2*4+3]; }
				if ( pSrc[3*4+1]!=0xff ) { uCount++; uTotal += pSrc[3*4+3]; }
#endif
				pSrc += Scanline32 ;
			}
			if ( uCount==0 )
			{
				pDestPtr[i] = 0x7f;
				pMaskPtr[i] = 0x00;
			}
			else
			{
				pDestPtr[i] = uTotal*ScaleTable[uCount]>>20;
				pMaskPtr[i] = 0xff;
			}
			pSrcPtr += 16;
		}
		// move down by 1 scanline
		pSrcPtr  += Scanline32*(BEVELBITMAPSCALE-1);
		pDestPtr += Scanline8;
		pMaskPtr += Scanline8;
	}
}		

/********************************************************************************************

>	BOOL CBeveler::EmboldenBitmap(KernelBitmap * pImgBitmap, KernelBitmap * pMaskBitmap)

	Author:		Gavin_Theobald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Purpose:	'Bleeds' the bitmap into the mask to cover up any inconsistancies
				between the screen paths and the bitmap
	Inputs:		The image bitmap to embolden, and the mask bitmap to use.
	Outputs:	New mask bitmap.
	Returns:	
	SeeAlso:	-

********************************************************************************************/

BOOL CBeveler::EmboldenBitmap(KernelBitmap * pImgBitmap, KernelBitmap * pMaskBitmap, KernelBitmap * pNewMaskBitmap)
{
	CamProfile cp(CAMPROFILE_BEVEL);

	static CONST UINT32 ScaleTable[8] = {
		0x101010/8,		// uCount==FF*8
		0x101010/7,		// uCount==FF*7
		0x101010/6,		// uCount==FF*6
		0x101010/5,		// uCount==FF*5
		0x101010/4,		// uCount==FF*4
		0x101010/3,		// uCount==FF*3
		0x101010/2,		// uCount==FF*2
		0x101010/1		// uCount==FF*1
	} ;

	UINT32 Width = pMaskBitmap->GetWidth ();
	UINT32 Depth = pMaskBitmap->GetHeight();
	// Best check that we`ve got valid widths and heights!
	if ( Width < 1 || Depth <= 2 )
	{
		ERROR3("Got a width or height of 2 or smaller in beveler EmboldenBitmap!");
		return TRUE;
	}
	if ( Width!=pNewMaskBitmap->GetWidth() || Depth!=pNewMaskBitmap->GetHeight() )
	{
		ERROR3("pMaskBitmap and pNewMask have different sizes in beveler EmboldenBitmap!");
		return FALSE;
	}

	UINT32 W = DIBUtil::ScanlineSize(Width,8);
	BYTE* pILine =	   pImgBitmap->GetBitmapBits();
	BYTE* pMLine =	  pMaskBitmap->GetBitmapBits();
	BYTE* pNLine = pNewMaskBitmap->GetBitmapBits();
	UINT32 uCount;
	UINT32 i;
	//
	// First scanline
	//
	BYTE* pIPtr = pILine;
	BYTE* pMPtr = pMLine;
	BYTE* pNPtr = pNLine;
	*pNPtr = *pMPtr;
	if ( *pMPtr==0x00 )
	{
		uCount = pMPtr[ +1]+
 				 pMPtr[W  ]+
				 pMPtr[W+1];
		if ( uCount )
		{
			*pIPtr = ((pMPtr[ +1] & pIPtr[ +1])+
					  (pMPtr[W  ] & pIPtr[W  ])+
					  (pMPtr[W+1] & pIPtr[W+1]))*ScaleTable[uCount & 7]>>20;
			*pNPtr = 0xff;
		}
	}
	pIPtr++; pMPtr++; pNPtr++;
	for ( i=2 ; i<Width ; i++ )
	{
		*pNPtr = *pMPtr;
		if ( *pMPtr==0x00 )
		{
			uCount = pMPtr[ -1]+
					 pMPtr[ +1]+
					 pMPtr[W-1]+
					 pMPtr[W  ]+
					 pMPtr[W+1];
			if ( uCount )
			{
				*pIPtr = ((pMPtr[ -1] & pIPtr[ -1])+
						  (pMPtr[ +1] & pIPtr[ +1])+
						  (pMPtr[W-1] & pIPtr[W-1])+
						  (pMPtr[W  ] & pIPtr[W  ])+
						  (pMPtr[W+1] & pIPtr[W+1]))*ScaleTable[uCount & 7]>>20;
				*pNPtr = 0xff;
			}
		}
		pIPtr++; pMPtr++; pNPtr++;
	}
	*pNPtr = *pMPtr;
	if ( *pMPtr==0x00 )
	{
		uCount = pMPtr[ -1]+
 				 pMPtr[W  ]+
				 pMPtr[W-1];
		if ( uCount )
		{
			*pIPtr = ((pMPtr[ -1] & pIPtr[ -1])+
					  (pMPtr[W  ] & pIPtr[W  ])+
					  (pMPtr[W-1] & pIPtr[W-1]))*ScaleTable[uCount & 7]>>20;
			*pNPtr = 0xff;
		}
	}
	//
	// Second to penultimate scanline
	//
	for ( UINT32 j=2 ; j<Depth ; j++ )
	{
		pIPtr = pILine;
		pMPtr = pMLine;
		pNPtr = pNLine;
		pNPtr[W] = pMPtr[W];
		if ( pMPtr[W]==0x00 )
		{
			uCount = pMPtr[    0]+
 					 pMPtr[   +1]+
					 pMPtr[W  +1]+
					 pMPtr[W*2  ]+
					 pMPtr[W*2+1];
			if ( uCount )
			{
				pIPtr[W] = ((pMPtr[    0] & pIPtr[    0])+
							(pMPtr[   +1] & pIPtr[   +1])+
							(pMPtr[W  +1] & pIPtr[W  +1])+
							(pMPtr[W*2  ] & pIPtr[W*2  ])+
							(pMPtr[W*2+1] & pIPtr[W*2+1]))*ScaleTable[uCount & 7]>>20;
				pNPtr[W] = 0xff;
			}
		}
		pIPtr++; pMPtr++; pNPtr++;
		for ( i=2 ; i<Width ; i++ )
		{
			pNPtr[W] = pMPtr[W];
			if ( pMPtr[W]==0x00 )
			{
				uCount = pMPtr[   -1]+
						 pMPtr[    0]+
						 pMPtr[   +1]+
						 pMPtr[W  -1]+
						 pMPtr[W  +1]+
						 pMPtr[W*2-1]+
						 pMPtr[W*2  ]+
						 pMPtr[W*2+1];
				if ( uCount )
				{
					pIPtr[W] = ((pMPtr[   -1] & pIPtr[   -1])+
								(pMPtr[    0] & pIPtr[    0])+
								(pMPtr[   +1] & pIPtr[   +1])+
								(pMPtr[W  -1] & pIPtr[W  -1])+
								(pMPtr[W  +1] & pIPtr[W  +1])+
								(pMPtr[W*2-1] & pIPtr[W*2-1])+
								(pMPtr[W*2  ] & pIPtr[W*2  ])+
								(pMPtr[W*2+1] & pIPtr[W*2+1]))*ScaleTable[uCount & 7]>>20;
					pNPtr[W] = 0xff;
				}
			}
			pIPtr++; pMPtr++; pNPtr++;
		}
		pNPtr[W] = pMPtr[W];
		if ( pMPtr[W]==0x00 )
		{
			uCount = pMPtr[   -1]+
					 pMPtr[    0]+
 					 pMPtr[W  -1]+
					 pMPtr[W*2-1]+
					 pMPtr[W*2  ];
			if ( uCount )
			{
				pIPtr[W] = ((pMPtr[   -1] & pIPtr[   -1])+
							(pMPtr[    0] & pIPtr[    0])+
							(pMPtr[W  -1] & pIPtr[W  -1])+
							(pMPtr[W*2-1] & pIPtr[W*2-1])+
							(pMPtr[W*2  ] & pIPtr[W*2  ]))*ScaleTable[uCount & 7]>>20;
				pNPtr[W] = 0xff;
			}
		}
		pILine += W;
		pMLine += W;
		pNLine += W;
	}
	//
	// Last scanline
	//
	pIPtr = pILine;
	pMPtr = pMLine;
	pNPtr = pNLine;
	pNPtr[W] = pMPtr[W];
	if ( pMPtr[W]==0x00 )
	{
		uCount = pMPtr[  0]+
				 pMPtr[ +1]+
 				 pMPtr[W+1];
		if ( uCount )
		{
			pIPtr[W] = ((pMPtr[  0] & pIPtr[  0])+
						(pMPtr[ +1] & pIPtr[ +1])+
						(pMPtr[W+1] & pIPtr[W+1]))*ScaleTable[uCount & 7]>>20;
			pNPtr[W] = 0xff;
		}
	}
	pIPtr++; pMPtr++; pNPtr++;
	for ( i=2 ; i<Width ; i++ )
	{
		pNPtr[W] = pMPtr[W];
		if ( pMPtr[W]==0x00 )
		{
			uCount = pMPtr[ -1]+
					 pMPtr[  0]+
					 pMPtr[ +1]+
					 pMPtr[W-1]+
					 pMPtr[W+1];
			if ( uCount )
			{
				pIPtr[W] = ((pMPtr[ -1] & pIPtr[ -1])+
							(pMPtr[  0] & pIPtr[  0])+
							(pMPtr[ +1] & pIPtr[ +1])+
							(pMPtr[W-1] & pIPtr[W-1])+
							(pMPtr[W+1] & pIPtr[W+1]))*ScaleTable[uCount & 7]>>20;
				pNPtr[W] = 0xff;
			}
		}
		pIPtr++; pMPtr++; pNPtr++;
	}
	pNPtr[W] = pMPtr[W];
	if ( pMPtr[W]==0x00 )
	{
		uCount = pMPtr[ -1]+
 				 pMPtr[  0]+
				 pMPtr[W-1];
		if ( uCount )
		{
			pIPtr[W] = ((pMPtr[ -1] & pIPtr[ -1])+
					  	(pMPtr[  0] & pIPtr[  0])+
					  	(pMPtr[W-1] & pIPtr[W-1]))*ScaleTable[uCount & 7]>>20;
			pNPtr[W] = 0xff;
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CBeveler::RenderWithResampling(KernelBitmap * pBitmap, INT32 OffsetX, INT32 OffsetY)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Purpose:	Renders the points list in strips to the given 8 bit DIB
	Inputs:		pBitmap - 8 bit bitmap to render to
				OffsetX	- the x offset to add to the x coords of the areas to be rendered
							(in pixels)
				OffsetY - the y offset to add to the y coords of the areas to be rendered
							(in pixels)
				
	Returns:	TRUE for success, FALSE for failure
	Notes:		OffsetX & OffsetY denote the origin of the super-bitmap where the bitmap 
				which this fn creates is to be placed
				This is so that this function can create an area in a larger bitmap to
				avoid the maximum width & height of bitmap that GDraw2 can cope with
	SeeAlso:	-

********************************************************************************************/

void memset32(DWORD* pMem, DWORD Val, DWORD Len)
{
	for (DWORD i=0; i < Len; i++)
		*pMem++ = Val;
}


BOOL CBeveler::RenderWithResampling(KernelBitmap * pBitmap, INT32 OrigOffsetX, INT32 OrigOffsetY, DocCoord* pSubPixOffSet)
{
	TRACEUSER( "GerryX", _T("(Beveller) Entering Render With Resampling ------- \n"));

#ifdef DEBUG
	DWORD Time = GetTickCount();
#endif

	DocCoord SubPixOffset(0,0);

	if(pSubPixOffSet != NULL)
		SubPixOffset = *pSubPixOffSet;
	else
		ERROR3("Got a NULL DocCoord pointer! Using 0,0 coords!");

	// alter the offsets into coordinates that gdraw2 recognises
	OrigOffsetX *= BEVELBITMAPSCALE<<16;
	OrigOffsetY *= BEVELBITMAPSCALE<<16;

	// NEW BIT! This now adds the subpixel offsets of the coordinates
	FIXED16 XOff = SubPixOffset.x * BEVELBITMAPSCALE * 0.001;
	FIXED16 YOff = SubPixOffset.y * BEVELBITMAPSCALE * 0.001;
	OrigOffsetX -= XOff.GetShifted16();
	OrigOffsetY -= YOff.GetShifted16();

	// set up the bitmap in GDraw and GDraw2
	BITMAPINFOHEADER Header;
	Header.biWidth		 = m_pStrip32->GetWidth ();
	Header.biHeight		 = m_pStrip32->GetHeight();
	Header.biPlanes		 = 1;
	Header.biBitCount	 = 32;
	Header.biCompression = BI_RGB ;

	INT32 Error = SetDIBitmap( &Header,(BYTE*)m_pStrip32->GetBitmapBits() ) ;
	if ( Error!=GERROR_NO_ERROR )
		ERROR2(FALSE, "GDraw2 can't initialise");

	// set up the strip first
	UINT32 i;
	DWORD* pBits = (DWORD*)m_pStrip32->GetBitmapBits() ;
	UINT32 nLength = m_pStrip32->GetWidth()*m_pStrip32->GetHeight() ;
#if defined(WORDS_BIGENDIAN)
	memset32(pBits, 0xffff007f, nLength);
#else
	memset32(pBits, 0x7f00ffff, nLength);
#endif

	// draw the points list at this scanline position
	// pixel size in point units of the bitmap strip
	BEVEL_FACE* pFace = (BEVEL_FACE*)m_pFaceList ;
	for ( i=0 ; i<m_NumFaces ; i++ )
	{
		POINT Points[4];
		Points[0].x = pFace->aFace[0].X-OrigOffsetX ;
		Points[0].y = pFace->aFace[0].Y-OrigOffsetY ;
		Points[1].x = pFace->aFace[1].X-OrigOffsetX ;
		Points[1].y = pFace->aFace[1].Y-OrigOffsetY ;
		Points[2].x = pFace->aFace[2].X-OrigOffsetX ;
		Points[2].y = pFace->aFace[2].Y-OrigOffsetY ;
		if ( pFace->bTriangle )
		{
			if ( Points[1].x!=Points[2].x ||
				 Points[1].y!=Points[2].y )
			{
				CamProfile cp(CAMPROFILE_GDRAW);
				GDraw2_FillTriangle(Points,pFace->Normal.X,pFace->Normal.Y);
			}
		}
		else
		{
			Points[3].x = pFace->aFace[3].X-OrigOffsetX ;
			Points[3].y = pFace->aFace[3].Y-OrigOffsetY ;
			CamProfile cp(CAMPROFILE_GDRAW);
			GDraw2_FillTrapezium(Points,pFace->Normal.X,pFace->Normal.Y);
		}
		pFace++ ;
	}
	//
	// Blank out invalid parts of bitmap. For inner contours we blank out
	// outside of the path and for outer contours we blank out inside of
	// the path.
	//
	// We create our own GDrawAsm context to do this as it is much quicker than
	// saving and restoring the static one
	GDrawAsm* pGD = new GDrawAsm;
	if (pGD)
	{
		if (pGD->Init() && pGD->SetupBitmap(Header.biWidth, Header.biHeight, 32, m_pStrip32->GetBitmapBits(), 0))
		{
			GMATRIX GMatrix ;
			GMatrix.AX = 1<<FX ;
			GMatrix.AY = 0 ;
			GMatrix.BX = 0 ;
			GMatrix.BY = 1<<FX ;
			GMatrix.CX = -(__int64)OrigOffsetX<<FX ;
			GMatrix.CY = -(__int64)OrigOffsetY<<FX ;
			pGD->SetMatrix(&GMatrix) ;
			pGD->SetAntialiasFlag(FALSE) ;
#if defined(WORDS_BIGENDIAN)
			pGD->SetWordColour(0xffff007f) ;
#else
			pGD->SetWordColour(0x7f00ffff) ;
#endif
//			pGD->FillPath(m_pBevelPoints,m_pBevelTypes,m_nBevelLength,ALTERNATE|!m_bOuter<<1);
			pGD->FillPath(m_pBevelPoints,m_pBevelTypes,m_nBevelLength,!m_bOuter<<1);
		}
		delete pGD;
	}

	// now we've rendered to the scanline, resample into the correct place in the
	// bitmap
	//
	// create the mask bitmap
	BYTE* pMaskBits = m_pMaskBitmap->GetBitmapBits() ;
	BYTE* pDestBits = pBitmap->GetBitmapBits() ;
	ERROR2IF(!pMaskBits,FALSE,"NULL MaskBits Pointer in Beveler!");
	ERROR2IF(!pDestBits,FALSE,"NULL DestBits Pointer in Beveler!");
	ResampleBitmap(m_pStrip32->GetBitmapBits(), m_pStrip32->GetWidth (),
				   m_pStrip32->GetHeight(), pDestBits, pMaskBits);

	if(pBitmap->GetWidth() > 4 && pBitmap->GetHeight() > 4)
	{
		// embolden the bitmap
		if ( !EmboldenBitmap(pBitmap,m_pMaskBitmap,m_pNewMask) ||
			 !EmboldenBitmap(pBitmap,m_pNewMask,m_pMaskBitmap) )
		{
			return FALSE ;
		}
	}

#ifdef DEBUG
	TRACEUSER( "GerryX", _T("Last Bevel Bitmap render resampling = %dms\n"),GetTickCount() - Time);
#endif

	return TRUE;
}

/********************************************************************************************

>	void CBeveler::CopyBitmapIntoBitmap(const KernelBitmap * pSrcBitmap, KernelBitmap * pDestBitmap,
						INT32 DestX, INT32 DestY)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/99
	Purpose:	Copies the original bitmap into the destination bitmap given the origin to
				start the copy in the destination
	Inputs:		pSrcBitmap  - the bitmap to copy
				pDestBitmap - the destination bitmap to copy into
				DestX		- the pixel X position to start the copy
				DestY		- the pixel Y position to start the copy

	Returns:	
	Notes:		Accounts for overshoots i.e. if
					pSrcBitmap->GetWidth () > pDestBitmap->GetWidth ()+DestX or
					pSrcBitmap->GetHeight() > pDestBitmap->GetHeight()+DestY

				then this is ok.

				Both bitmaps MUST be 8-bit
	SeeAlso:	-

********************************************************************************************/
void CBeveler::CopyBitmapIntoBitmap(const KernelBitmap * pSrcBitmap, KernelBitmap * pDestBitmap,
						INT32 DestX, INT32 DestY)
{
	// start out by calculating how many pixels across & down we are to copy (takes
	// into account overshoots of the source on the destination)
	INT32 SrcWidth = (INT32)pSrcBitmap->GetWidth ();
	INT32 SrcDepth = (INT32)pSrcBitmap->GetHeight();
//	INT32 DstWidth = SrcWidth;
//	INT32 DstDepth = SrcDepth;
	INT32 DstWidth = (INT32)pDestBitmap->GetWidth ();
	INT32 DstDepth = (INT32)pDestBitmap->GetHeight();
	if ( DestX+SrcWidth>DstWidth ) SrcWidth = DstWidth-DestX;
	if ( DestY+SrcDepth>DstDepth ) SrcDepth = DstDepth-DestY;
	if ( SrcWidth>0 && SrcDepth>0 )
	{
		// the increase for the scanline in the source bitmap
		INT32  SrcScanlineInc = DIBUtil::ScanlineSize(SrcWidth,8);
		INT32 DestScanlineInc = DIBUtil::ScanlineSize(DstWidth,8);

		// ok, all variables set up so lets do the transfer !
		BYTE*  pSrcPtr =  pSrcBitmap->GetBitmapBits() ;
		BYTE* pDestPtr = pDestBitmap->GetBitmapBits()+DestX+DestY*DestScanlineInc ;
		while ( SrcDepth>0 )
		{
			memcpy( pDestPtr,pSrcPtr,SrcWidth ) ;
			 pSrcPtr +=  SrcScanlineInc ;
			pDestPtr += DestScanlineInc ;
			SrcDepth-- ;
		}
	}
}
