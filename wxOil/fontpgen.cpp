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
// FontPGen.cpp - the Font Previewer Thumbnail Generation class

#include "camtypes.h"

#ifndef EXCLUDE_GALS
#include "bfxalu.h"
#include "textfuns.h"
#include "errors.h"
#include "bitmap.h"			  
#include "bitmpinf.h"
#include "ccfile.h"
#include "grndrgn.h"
#include "wbitmap.h"
#include "outptdib.h"
#include "paths.h"
#include "trans2d.h"
#include "fontpgen.h"
#include "progress.h"
#include "sgallery.h"
#include "dlgcol.h"
#include "unicdman.h"

#include "thumb.h" // not normally needed

// Implement the dynamic class bits...
CC_IMPLEMENT_DYNAMIC(StringToBitmap, BitmapEffectBase)

// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

/***********************************************************************************/

// Background font thumbnail colour
const INT32 S2BMP_BACKGROUND_COLOUR = 0x00ffffff; // white
const INT32 S2BMP_FOREGROUND_COLOUR = 0x00000000; // black

// Antialias the previews ?
const BOOL S2BMP_ANTIALIAS = TRUE;

// Mark's magic number for his GetBezierFromChar routine
const INT32 S2BMP_MAGIC = 2000;

/***********************************************************************************************

>	class StringToBitmap

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/95
	Purpose:	Generation of anti-aliased font-string bitmaps for the font installer / de-installer

	SeeAlso:	BfxALU.cpp

***********************************************************************************************/

StringToBitmap::StringToBitmap(void)
{
}

/************************************************************************************/

StringToBitmap::~StringToBitmap(void)
{
}

/************************************************************************************

>	BOOL StringToBitmap::SaveBitmap(KernelBitmap *Bitmap, PathName *Path)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		Bitmap - Pointer to a kernel bitmap
				Path - Pathname to save bitmap under
	Outputs:	Returns TRUE if things went OK.
	Purpose:	Saves the given kernel bitmap to a file...
				This should only be used for saving thumbnails, and has only
				been tested with 32 bpp files so far...

********************************************************************************************/

BOOL StringToBitmap::SaveBitmap(KernelBitmap *Bitmap, PathName *Path)
{
	BOOL ok = TRUE;

//	if(Bitmap->GetBPP() != 32)
//	{
//		TRACEUSER( "Richard", _T("Can only save converted 32bpp thumbnails"));
//		return FALSE;
//	}

	// Get the file ready for outputting
	CCDiskFile OutputFile;
	PathName TmpPath(*Path);
	ok = OutputFile.open(TmpPath, ios::out | ios::binary);
	if(!ok) ERROR3("s2bmp OutputFile.open returned false");
	if(!ok) return FALSE;
	
	
	// Always output at 8bpp or 32bpp
	const UINT32 OutputDepth = Bitmap->GetBPP();

	if(OutputDepth != 8 && OutputDepth != 32)
	{
		ERROR3("Wrong bpp for saving bmp in fontpgen");
		return FALSE;
	}

	// Extract the header info and write it to the file first		
	BitmapInfo Info;
	BitmapInfo *lpInfo = &Info;
	ok = Bitmap->ActualBitmap->GetInfo(lpInfo);
	if(!ok) ERROR3("s2bmp Bitmap->ActualBitmap->GetInfo(lpInfo) returned false");
	if(!ok) return FALSE;

	LPBITMAPINFOHEADER pBMI =&(((WinBitmap *)(Bitmap->ActualBitmap))->BMInfo->bmiHeader); 
	ok = (pBMI!=NULL);
	if(!ok) ERROR3("s2bmp pBMI!=NULL returned false");
	if(!ok) return FALSE;

	UINT32 Ysize = pBMI->biHeight;

	// Sort the palette out
	LPLOGPALETTE lpPalette = NULL;

	DWORD space[258]; // bodge (256 entries + 2 DWORDS)
	if(OutputDepth<=8)
	{
		//DWORD *space = malloc(sizeof(DWORD) * (2<<OutputDepth)+4); // bodge!
		lpPalette = (LPLOGPALETTE)(void *)(space);
		memcpy(lpPalette->palPalEntry /*dest*/, (pBMI+1/*ptr arith*/)/*src*/, (pBMI->biClrUsed)*sizeof(DWORD));
		lpPalette->palVersion = 0x300;
		lpPalette->palNumEntries = (unsigned short)(pBMI->biClrUsed);
	}

	// Set up the compression flag only if the output depth is correct
	UINT32 Compress = BI_RGB;
//	if (OutputDepth==4) Compress = BI_RLE4;
//	if (OutputDepth==8) Compress = BI_RLE8;
				 											 					 		 
	// Write header to output file
	OutputDIB DestDIB;

	BeginSlowJob(100, TRUE, &(String_64)"Creating thumbnail");

	ok = DestDIB.StartFile(	&OutputFile, pBMI, lpPalette,
							OutputDepth,					// actual file depth
							Compress,						// compression (BI_RGB = none)
							(UINT32) Ysize,					// all of it
							100);							// size of progress bar

	if(!ok) ERROR3("DestDIB.StartFile returned false");
	if(!ok)	EndSlowJob();
	if(!ok) return FALSE;

	// write main data to the output file
	ok = DestDIB.WriteBlock(0, Ysize, ((WinBitmap *)(Bitmap->ActualBitmap))->BMBytes, (UINT32) Bitmap->GetBPP());
	if(!ok) ERROR3("DestDIB.WriteBlock returned false");
	if(!ok)	EndSlowJob();
	if(!ok) return FALSE;

	// Tidy up the file before killing the object
	ok = DestDIB.TidyUp();

	EndSlowJob();

	if(!ok) ERROR3("DestDIB.TidyUp returned false");
	if(!ok) return FALSE;

	return TRUE;
}

/************************************************************************************

>	BOOL StringToBitmap::BlankBitmap(UINT32 Xsize, UINT32 Ysize, UINT32 BPP, UINT32 BackCol,
								 KernelBitmap **BM)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95

	Inputs:		Xsize - Width (in pixels) of thumbnail to generate
				Ysize - Height (in pixels) of thumbnail to generate
				BPP - Bits Per Pixel for output bitmap... 32 or 8 are sensible
				BackCol - Colour for bitmap background
	Returns:	BM - returns a pointer to a bitmap which we allocate here
	Outputs:	Returns TRUE if things went OK.

	Purpose:	Creates a bitmap of the specified size, and blanks it with the given
				colours... Even gives it a greyscale palette if <= 8bpp.
				
********************************************************************************************/

BOOL StringToBitmap::BlankBitmap(UINT32 Xsize, UINT32 Ysize, UINT32 BPP, UINT32 BackCol,
								 KernelBitmap **BM)
{
	// Nowt wrong yet
	BOOL ok = TRUE;	  	

	// Create a new (temp) bitmap of desired size
	INT32 Width = Xsize;
	INT32 Height = Ysize;
	INT32 Depth = BPP;
	INT32 DPI = 96;
	KernelBitmap * Bitmap = new KernelBitmap(Width,Height,Depth,DPI, TRUE);
	Error::ClearError();

	// Check it 'worked'
	ok = ( (Bitmap != NULL) && (Bitmap->ActualBitmap != NULL)
			&& (((WinBitmap *)(Bitmap->ActualBitmap))->BMInfo != NULL)
			&& (((WinBitmap *)(Bitmap->ActualBitmap))->BMBytes != NULL) );

	ERROR3IF(!ok, "BlankBitmap can't allocate space for bitmap");
	if(ok)
	{		
		// If the bitmap is capable of having a palette, give it a nice even greyscale one.
		if(BPP <= 8)
		{
			// How many entries in the palette ?
			DWORD Entries = ((WinBitmap*)Bitmap->ActualBitmap)->BMInfo->bmiHeader.biClrUsed;
			if (Entries==0) Entries = 1<<BPP;
			RGBQUAD Quad;
			INT32 i;
			BYTE Value;

			// Factor by which to multiply the counter to obtain an even spread
			INT32 Mult = 256 / Entries;

			((WinBitmap*)Bitmap->ActualBitmap)->BMInfo->bmiHeader.biClrImportant = Entries;

			for(i=0; i < (INT32)Entries; i++)
			{
				Value = i * Mult;

				if(Value > 255) Value = 255;

				// Fill up a quad with a grey-scale entry
				Quad.rgbBlue = Value;
        		Quad.rgbGreen = Value;
        		Quad.rgbRed = Value;
				Quad.rgbReserved = 0;

				// Poke the quad into the wbitmap
				((WinBitmap*)Bitmap->ActualBitmap)->BMInfo->bmiColors[i] = Quad;
								 
			}
		}

		// Set the bfx accumulator to our new bitmap
		ok = ALU->SetA(Bitmap);
		ERROR3IF(!ok, "BlankBitmap ALU->SetA returned false");
		if(ok)
		{
	 		// Fill the bitmap with the specified fill colour
			ok = ALU->ZeroA(BackCol);
			ERROR3IF(!ok, "BlankBitmap ALU->ZeroA returned false");
		}
	}

	if(!ok && Bitmap != NULL)
	{
		delete Bitmap;
		Bitmap = NULL;
	}

	*BM = Bitmap;
	return ok;
}

/********************************************************************************************

>	BOOL StringToBitmap::TTFAddString(String_256 *text, UINT32 Xsize, UINT32 Ysize, UINT32 DPI,
			PLOGFONT pLogFont, INT32 IntLeading, KernelBitmap **BM, UINT32 ForeColour)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		text - pointer to a string of thumbnail text
				Xsize - Width (in pixels) of thumbnail to generate
				Ysize - Height (in pixels) of thumbnail to generate
				DPI - DPI of bitmap to add string to
				pLogFont - Pointer to a log font structure of the font to use
				IntLeading - Internal leading valoue of font, as given in the NewTextMetrics
				BM - Pointer to a pointer to a kernel bitmap to return the result in
				ForeColour - Foreground colour of text

	Outputs:	Returns TRUE if things went OK.

	Purpose:	This is the main routine for sticking strings in bitmaps...
				Using the supplied info we generate a path, and plot this into the
				bitmap, which has already been set up to the required size...
				
********************************************************************************************/

BOOL StringToBitmap::TTFAddString(String_256 *text, UINT32 Xsize, UINT32 Ysize, UINT32 DPI, PLOGFONT pLogFont,
								 INT32 IntLeading, KernelBitmap **BM, UINT32 ForeColour)
{
	KernelBitmap *Bitmap = *BM;

/*	HDC ScreenDC = CreateCompatibleDC(NULL);
	if (ScreenDC == NULL)
	{
		ERROR3("StringToBitmap::AddString: Unable to create screen DC");
		return FALSE;
	}*/

   	CDC SysDisplay;
	BOOL ok=SysDisplay.CreateCompatibleDC(NULL);
	if(!ok)
	{
		//DeleteDC(ScreenDC);
		ERROR3("StringToBitmap::TTF AddString: Unable to create CDC");
		return FALSE;
	}

	HDC ScreenDC = SysDisplay.m_hDC;
	
	// bodge to get things working with GetBezierFromChar
	INT32 OldlfHeight = pLogFont->lfHeight;
	pLogFont->lfHeight = -(pLogFont->lfHeight - IntLeading);
    
	UINT32 CurrentPathSizeAlloc = 0;
	Trans2DMatrix *pTransform = NULL; 
	DocCoord *pPathCoords = NULL;

	Path *pPath = NULL;
	//pPath = new Path(); 

	DocCoord *pPolyCordBuffer = NULL;
	PathVerb *pPolyVerbBuffer = NULL;
	UINT32 TextLength = (UINT32)text->Length();
	SIZE StringSize={0,0};
			
	// Get handle of font
	
//	HFONT hNewFont = CreateFontIndirect(pLogFont);
//	HGDIOBJ hOldFont = SelectObject(ScreenDC, hNewFont);

	CFont UnHintedCFont;
	if(!UnHintedCFont.CreateFontIndirect(pLogFont))
	{
		SysDisplay.DeleteDC();
		pLogFont->lfHeight = OldlfHeight;
		return FALSE;
	}
	
	CFont* pOldCFont=SysDisplay.SelectObject(&UnHintedCFont);

	// Get the default character to use if a charater is not present in the font.
	WCHAR FontDefaultCharacter = (unsigned char)'?';
	TEXTMETRIC FontTextData;
#ifdef _UNCCODE
	if (SysDisplay.GetTextMetrics(&FontTextData))
		FontDefaultCharacter = FontTextData.tmDefaultChar;
#else
	if (SysDisplay.GetTextMetrics(&FontTextData))
		FontDefaultCharacter = (unsigned char)FontTextData.tmDefaultChar;
#endif
	
	// Work out a nice scaling factor so the font fits in the bitmap ok...
	
	// Not 32 ?
	GetTextExtentPoint(ScreenDC, *text, TextLength, &StringSize);

	if(StringSize.cy == 0)
	{
		SysDisplay.SelectObject(pOldCFont);
		SysDisplay.DeleteDC();
		pLogFont->lfHeight = OldlfHeight;
		return FALSE;
	}

	//ERROR3IF(!ok, "Initial GetTextExtentPoint32() failed");
	double YScale = ((double)Ysize / (double)StringSize.cy) / (double)2;
	double XScale = YScale;

	// Shift thumbnail upwards, and scale down a bit - to get the g's looking right
	// One or two fonts require this reducing (their tops are clipped), 72000/100 is
	// about right for most of them though...
	// Note the external previews were done with 72000/220 for Matrix and 72000/140 for
	// the capital only fonts.
	double YShift = 72000/100;//72000/80;

	YScale = (YScale * 78) / 100;
	XScale = (XScale * 78) / 100;

	if(!text->IsEmpty())
	{
		const TCHAR* pCurrentChar = (const TCHAR*)(*text);

		while (ok && *pCurrentChar!=0)
		{
			// Get the current character as Unicode.
#ifdef _UNICODE
			WCHAR wchr = *pCurrentChar;		// pCurrentChar is a pointer to WCHAR in _UNICODE builds
#else
			UINT32 CharToConvert = 0;
			if (UnicodeManager::IsDBCSLeadByte(*pCurrentChar))
				CharToConvert = UnicodeManager::ComposeMultiBytes(*pCurrentChar, *(pCurrentChar+1));
			else
				CharToConvert = (unsigned char)(*pCurrentChar);
			WCHAR wchr = UnicodeManager::MultiByteToUnicode(CharToConvert);
#endif

			// Get positioning information for this character
			ok = GetTextExtentPoint(ScreenDC, *text, (pCurrentChar-(TCHAR*)(*text)), &StringSize);
			ERROR3IF(!ok, "GetTextExtentPoint32() failed");
			if (!ok) break;
						 
			// Get the characters path
			DWORD PathSize = 0;							
			ok = TextManager::GetBezierFromChar(&SysDisplay, wchr, pLogFont, &PathSize, (POINT *)NULL, (BYTE *)NULL);
			if (!ok)
			{
				wchr = FontDefaultCharacter;
				ok = TextManager::GetBezierFromChar(&SysDisplay, wchr, pLogFont, &PathSize, (POINT *)NULL, (BYTE *)NULL);
			}
			ERROR3IF(!ok, "GetBezierFromChar returned false");
			if (!ok) break;

 			// Pointer to an array of path coordinates
			if(pPolyCordBuffer == NULL)
			{
				TRY
				{
					pPolyCordBuffer = new DocCoord[PathSize];
				}
				CATCH (CMemoryException, e)
				{
					pPolyCordBuffer = NULL;
					/*ERROR(_R(IDS_OUT_OF_MEMORY), FALSE);*/
				}
				END_CATCH
			}

		 	// Pointer to an array of path verbs
			if(pPolyVerbBuffer == NULL)
			{
				TRY
				{
					pPolyVerbBuffer = new PathVerb[PathSize];
				}
				CATCH (CMemoryException, e)
				{
					pPolyVerbBuffer = NULL;
					/*ERROR(_R(IDS_OUT_OF_MEMORY), FALSE);*/
				}
				END_CATCH
			}
											
			if (pPolyCordBuffer == NULL || pPolyVerbBuffer == NULL)
			{
				ok = FALSE;
				break;
			}

			CurrentPathSizeAlloc = PathSize;

			// Fill up the buffers until they're bursting with fontyness
			ok = TextManager::GetBezierFromChar(&SysDisplay, wchr, pLogFont, &PathSize, (POINT *)pPolyCordBuffer,
				 															(BYTE *)pPolyVerbBuffer);
			if(!ok) TRACEUSER( "Richard", _T("GetBezierFromChar returned false in second phase...\n"));
			if(!ok)	break;

			// Spaces set PathSize to zero
			if((PathSize > 0)/* && (pPath != NULL)*/)
			{				
				pPath = new Path(); 
				pPath->Initialise(PathSize, 12);
				pPath->CopyPathDataFrom(pPolyCordBuffer, pPolyVerbBuffer, PathSize, TRUE);

				// Major bodge at present with the x spacing...
				Matrix scale(XScale, 0, 0, YScale, (INT32)((XScale*StringSize.cx*72000)/(double)DPI), (INT32)YShift);
				
				pTransform = new Trans2DMatrix(scale);		
				pPathCoords = pPath->GetCoordArray();							     
				pTransform->Transform( pPathCoords, pPath->GetNumCoords() );
				delete pTransform;

				pPath->InitialiseFlags();
	
				ok = ALU->GradFillPath(pPath, ForeColour, ForeColour, 0, 0, 0,/*Xsize/2,*/ Ysize, S2BMP_ANTIALIAS);
				ERROR3IF(!ok, "Gradfillpath returned false");
				if(!ok)	break;

				delete pPath;
			}

			// S2BMP_MAGIC is the worderfully fabby constant that mark's getbezierfromchar returns
			// Theory goes that he's going to sort this out sometime...
			if(CurrentPathSizeAlloc != S2BMP_MAGIC)
			{
				delete []pPolyCordBuffer;
				delete []pPolyVerbBuffer;

				pPolyCordBuffer = NULL;
				pPolyVerbBuffer = NULL;
				CurrentPathSizeAlloc = 0;
			}

			pPath = NULL;
			pTransform = NULL;

			pCurrentChar = camStrinc(pCurrentChar);
		}
	}

	// We don't want any memory left dangling about...
	if(pPolyCordBuffer != NULL) delete []pPolyCordBuffer;
	if(pPolyVerbBuffer != NULL)	delete []pPolyVerbBuffer;
	if(pPath != NULL) delete pPath;
	if(pTransform != NULL) delete pTransform;						   

	// Revert back to old font
//	SelectObject(ScreenDC, hOldFont);
//	DeleteObject(hNewFont);

	//Lets restore the DC font status
	SysDisplay.SelectObject(pOldCFont);
//	DeleteObject(UnHintedCFont);
							  
//	DeleteDC(ScreenDC);
	SysDisplay.DeleteDC();

	pLogFont->lfHeight = OldlfHeight;

	return ok;
}


/********************************************************************************************

>	BOOL StringToBitmap::AddString(String_256 *text, UINT32 Xsize, UINT32 Ysize, UINT32 DPI,
			PLOGFONT pLogFont, INT32 IntLeading, KernelBitmap **BM, UINT32 ForeColour, FontClass Type)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		text 		- pointer to a string of thumbnail text
				Xsize 		- Width (in pixels) of thumbnail to generate
				Ysize 		- Height (in pixels) of thumbnail to generate
				DPI 		- DPI of bitmap to add string to
				pLogFont 	- Pointer to a log font structure of the font to use
				IntLeading 	- Internal leading valoue of font, as given in the NewTextMetrics
				BM 			- Pointer to a pointer to a kernel bitmap to return the result in
				ForeColour 	- Foreground colour of text
				Type		- FontClass of Font -> FC_TRUETYPE, or FC_ATM

	Outputs:	Returns TRUE if things went OK.

	Purpose:	This is the main routine for sticking strings in bitmaps...
				Using the supplied info we generate a path, and plot this into the
				bitmap, which has already been set up to the required size...
				
********************************************************************************************/

BOOL StringToBitmap::AddString(String_256 *text, UINT32 Xsize, UINT32 Ysize, UINT32 DPI, PLOGFONT pLogFont,
								 INT32 IntLeading, KernelBitmap **BM, UINT32 ForeColour, FontClass Type)
{
	ERROR3IF(text == NULL, "StringToBitmap::AddString given null text pointer");
	ERROR3IF(pLogFont == NULL, "StringToBitmap::AddString given null logfont pointer");
	ERROR3IF(BM == NULL, "StringToBitmap::AddString given null bmp pointer");
	if(text == NULL || pLogFont == NULL || BM == NULL)
		return FALSE;

	KernelBitmap *Bitmap = *BM;

	FontInfo info;
	BOOL ok=TextManager::GetInfoFromLogFont(&info, pLogFont, Type);
	if (!ok)
	{
		ERROR3("StringToBitmap::ATMAddString problems getting font handle");		
		return FALSE;
	}

	BOOL Bold = info.Bold;
	BOOL Italic = info.Italic;
	WORD TypeFaceHandle = info.Handle;

   	CDC SysDisplay;
	ok=SysDisplay.CreateCompatibleDC(NULL);
	if(!ok)
	{
		ERROR3("StringToBitmap::AddString: Unable to create CDC");
		return FALSE;
	}

	HDC ScreenDC = SysDisplay.m_hDC;

	Trans2DMatrix *pTransform = NULL; 
	UINT32 ChNum = 0;
	DocCoord *pPathCoords = NULL;

	Path *pPath = NULL;

	DocCoord *pPolyCordBuffer = NULL;
	PathVerb *pPolyVerbBuffer = NULL;
	UINT32 TextLength = (UINT32)text->Length();
	SIZE StringSize={0,0};
			
	// Get handle of font
	LOGFONT CharLogFont;
	CharDescription DummyChar(TEXT('A'), TypeFaceHandle, Bold, Italic);
	ok=TextManager::GetLogFontFromCharDescriptor(&SysDisplay, DummyChar, &CharLogFont);

	CFont UnHintedCFont;
	if(!UnHintedCFont.CreateFontIndirect(&CharLogFont))
	{
		SysDisplay.DeleteDC();
		return FALSE;
	}

	CFont* pOldCFont=SysDisplay.SelectObject(&UnHintedCFont);
	
	// Work out a nice scaling factor so the font fits in the bitmap ok...
	// Not 32 ?
	if(!GetTextExtentPoint(ScreenDC, *text, TextLength, &StringSize))
	{
		SysDisplay.SelectObject(pOldCFont);
		SysDisplay.DeleteDC();
		return FALSE;
	}

	if(StringSize.cy == 0)
	{
		SysDisplay.SelectObject(pOldCFont);
		SysDisplay.DeleteDC();
		return FALSE;
	}

	double YScale = ((double)Ysize / (double)StringSize.cy) / (double)2;
	YScale = (YScale * 78) / 100;
	double XScale = YScale;
	double XSpacing = (XScale * 72000) / (double)DPI;

	// Shift thumbnail upwards, and scale down a bit - to get the g's looking right
	// One or two fonts require this reducing (their tops are clipped), 72000/100 is
	// about right for most of them though...
	// Note the external previews were done with 72000/220 for Matrix and 72000/140 for
	// the capital only fonts.
	double YShift = 72000/100;//72000/80;

	UINT32 NumCoords = 0;

	if(!text->IsEmpty())
	{
		const TCHAR* pCurrentChar = (const TCHAR*)(*text);

		while (ok && *pCurrentChar!=0)
		{
			// Get the current character as Unicode.
#ifdef _UNICODE
			WCHAR wchr = *pCurrentChar;		// pCurrentChar is a pointer to WCHAR in _UNICODE builds
#else
			UINT32 CharToConvert = 0;
			if (UnicodeManager::IsDBCSLeadByte(*pCurrentChar))
				CharToConvert = UnicodeManager::ComposeMultiBytes(*pCurrentChar, *(pCurrentChar+1));
			else
				CharToConvert = (unsigned char)(*pCurrentChar);
			WCHAR wchr = UnicodeManager::MultiByteToUnicode(CharToConvert);
#endif

			// Get positioning information for this character
			ok = GetTextExtentPoint(ScreenDC, *text, (pCurrentChar-(TCHAR*)(*text)), &StringSize);
			ERROR3IF(!ok, "GetTextExtentPoint32() failed");
			if (!ok) break;

			CharDescription CharDesc(wchr, TypeFaceHandle, Bold, Italic);
			NumCoords = 0;

			ok = OILFontMan::GetCharPath(Type, CharDesc, &pPolyCordBuffer, &pPolyVerbBuffer, &NumCoords, NULL);//&SysDisplay);
			if(!ok) break;

			// Spaces set PathSize to zero
			if((NumCoords > 0)/* && (pPath != NULL)*/)
			{				
				pPath = new Path(); 
				pPath->Initialise(NumCoords, 12);
				pPath->CopyPathDataFrom(pPolyCordBuffer, pPolyVerbBuffer, NumCoords, TRUE);

				// Major bodge at present with the x spacing...
				Matrix scale(XScale, 0, 0, YScale, (INT32)(XSpacing * (double)StringSize.cx), (INT32)YShift);
				
				pTransform = new Trans2DMatrix(scale);		
				pPathCoords = pPath->GetCoordArray();							     
				pTransform->Transform( pPathCoords, pPath->GetNumCoords() );
				delete pTransform;

				pPath->InitialiseFlags();

				ok = ALU->GradFillPath(pPath, ForeColour, ForeColour, 0, 0, 0,/*Xsize/2,*/ Ysize, S2BMP_ANTIALIAS);
				ERROR3IF(!ok, "Gradfillpath returned false");
				if(!ok)	break;

				delete pPath;
			}

			pPath = NULL;
			pTransform = NULL;
  
			pCurrentChar = camStrinc(pCurrentChar);
		}
	}

	// We don't want any memory left dangling about...
	if(pPath != NULL) delete pPath;
	if(pTransform != NULL) delete pTransform;						   

	//Lets restore the DC font status
	SysDisplay.SelectObject(pOldCFont);
	SysDisplay.DeleteDC();

	return ok;
}

/********************************************************************************************

>	BOOL StringToBitmap::MakeBitmap(String_256 *text, UINT32 Xsize, UINT32 Ysize, UINT32 BPP, UINT32 DPI,
			PLOGFONT pLogFont, INT32 IntLeading, KernelBitmap **BM, FontClass Type, PathName *Filename)
	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/2/95
	Inputs:		text - pointer to a string of thumbnail text
				Xsize - Width (in pixels) of thumbnail to generate
				Ysize - Height (in pixels) of thumbnail to generate
				BPP - Bits per pixel of image to create
				DPI - DPI of bitmap
				pLogFont - Pointer to a log font structure of the font to use
				IntLeading - Internal leading value for the font. Given by the font's NewTextMetrics
				BM - Pointer to a pointer to a kernel bitmap to return the result in
				Type - Type of font - Truetype or ATM
				Filename - Pointer to a filename for the thumbnail to be saved with (optional)
	Returns:	BM - Pointer to a pointer to a kernel bitmap to return the result in
	Outputs:	Returns TRUE if things went OK.

	Purpose:	Give me a string of text, some bitmap dimensions and a couple of font
				specifications, and I'll go away and generate a thumbnail using the 
				text and font specified.

				I can also save bitmaps out, that's where the Filename comes in. This
				can be left out, or passed as NULL if you don't want this...
				
********************************************************************************************/
					
BOOL StringToBitmap::MakeBitmap(String_256 *text, UINT32 Xsize, UINT32 Ysize, UINT32 BPP, UINT32 DPI,
			PLOGFONT pLogFont, INT32 IntLeading, KernelBitmap **BM, FontClass Type, PathName *Filename)
{
	ERROR3IF(text == NULL, "StringToBitmap::MakeBitmap given null text pointer");
	ERROR3IF(pLogFont == NULL, "StringToBitmap::MakeBitmap given null logfont pointer");
	ERROR3IF(BM == NULL, "StringToBitmap::MakeBitmap given null bmp pointer");
	if(text == NULL || pLogFont == NULL || BM == NULL)
		return FALSE;

	// Nowt wrong yet
	BOOL ok = TRUE;	  	

	// Our temporary bitmap pointer		
	KernelBitmap *Bitmap = NULL;

#if 0
	// work out the current window background colour so we don't get white rectangles round our items
	DialogColourInfo DCol;
	DocColour DialogBack = DCol.TextBack(); //DialogBack();
	INT32 Red, Green, Blue;	
	DialogBack.GetRGBValue(&Red, &Green, &Blue);	
	DWORD BackColRef = (DWORD) Red;
	BackColRef |= ((DWORD)Green) << 8;
	BackColRef |= ((DWORD)Blue) << 16;
#endif

	// Allocate memory and blank it
	// We always create a 32bpp bmp now, and convert it later...
	if(!BlankBitmap(Xsize, Ysize, 32, /*BackColRef*/S2BMP_BACKGROUND_COLOUR, &Bitmap))
	{
		ERROR3("BlankBitmap returned false");
		return FALSE;
	}

	// Actually store the bitmap pointer in the class
	*BM = Bitmap;

	// Now lets put the string in the bitmap (so help me god)
	if(!AddString(text, Xsize, Ysize, DPI, pLogFont, IntLeading, &Bitmap, S2BMP_FOREGROUND_COLOUR, Type))
	{
		Error::ClearError();
		ERROR3("Addstring returned false");
		return FALSE;
	}
	Error::ClearError();

	// Convert the 32bpp thumbnail into an 8bpp one	
	if(BPP == 8)
	{
		ALU->SetB(Bitmap);
		KernelBitmap *pNewBitmap = NULL;
		//if(ALU->MakeGreyscale(&pNewBitmap))
		if(ALU->MakeGreyscale32to8(&pNewBitmap))
		{
			if(pNewBitmap != NULL)
			{
				delete Bitmap;
				ContoneFontBitmap(pNewBitmap);
				Bitmap = pNewBitmap;
				*BM = Bitmap;
			}
		}
	}

	// Do we want to save the thumbnail to a file ?
	if(Filename != NULL)
	{
		// Save the bitmap
		ok = StringToBitmap::SaveBitmap(Bitmap, Filename);
		if(!ok) TRACEUSER( "Richard", _T("Failed to save thumbnail file"));
	}				

	return ok;
}

/********************************************************************************************

>	static BOOL StringToBitmap::ContoneFontBitmap(KernelBitmap *pBitmap)

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95

	Inputs:		pBitmap - Pointer to the KernelBitmap for the preview (must be 8bpp, etc)
	Returns:	TRUE if things went ok...

	Purpose:	Given a Greyscale font preview, contone it to blend in with the background
				of the gallery...
				
********************************************************************************************/

BOOL StringToBitmap::ContoneFontBitmap(KernelBitmap *pBitmap)
{
	ERROR3IF(pBitmap == NULL, "StringToBitmap::ContoneFontBitmap given a NULL param");

	if(pBitmap->GetBPP() != 8)
		return FALSE;

	if(!SuperGallery::UseFixedColourSet)
	{
		RGBQUAD Pal;
		RGBQUAD Start;
		RGBQUAD End;
		INT32 Red, Green, Blue;	
		DialogColourInfo RedrawColours;

		DocColour TmpCol = RedrawColours.TextFore();
		TmpCol.GetRGBValue(&Red, &Green, &Blue);	

		Start.rgbRed = (BYTE)Red;
		Start.rgbGreen = (BYTE)Green;
		Start.rgbBlue = (BYTE)Blue;
		Start.rgbReserved = 0;

		TmpCol = RedrawColours.TextBack();
		TmpCol.GetRGBValue(&Red, &Green, &Blue);	

		End.rgbRed = (BYTE)Red;
		End.rgbGreen = (BYTE)Green;
		End.rgbBlue = (BYTE)Blue;
		End.rgbReserved = 0;

		RGBQUAD *DestPal = ((WinBitmap *)pBitmap->ActualBitmap)->BMInfo->bmiColors;

		for(INT32 x=0; x<=255; x++)
		{	
			Pal.rgbRed = (BYTE)(Start.rgbRed + (INT32)(((float)(End.rgbRed - Start.rgbRed) / 256.0) * x) & 255);
			Pal.rgbGreen = (BYTE)(Start.rgbGreen + (INT32)(((float)(End.rgbGreen - Start.rgbGreen) / 256.0) * x) & 255);
			Pal.rgbBlue = (BYTE)(Start.rgbBlue + (INT32)(((float)(End.rgbBlue - Start.rgbBlue) / 256.0) * x) & 255);
			Pal.rgbReserved = 0;

			DestPal[x] = Pal;
		}

		// Bit of a bodge here to get the background and foreground absolutely correct (with no roundings, etc).
		// The background of the library fonts is sometimes 253, 254 or 255, hence the three lines below.
		DestPal[0] = Start;
		DestPal[253] = End;
		DestPal[254] = End;
		DestPal[255] = End;
	}

	return TRUE;
}
#endif
