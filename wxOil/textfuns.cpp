// $Id: textfuns.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the TEXT tool

/*
*/

#include "camtypes.h"
#include "textfuns.h"

#include "app.h"
#include "camelot.h"
#include "fontman.h"
#include "atmfonts.h"
#include "unicdman.h"

DECLARE_SOURCE( "$Revision: 662 $" );

CC_IMPLEMENT_MEMDUMP( CharMetrics, CC_CLASS_MEMDUMP );

#define new CAM_DEBUG_NEW     


/////////////////////////////////////////////////////////////////////////////////////////////
// CharMetrics

/***************************************************************************************
>	CharMetrics::CharMetrics()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/1/96
	Purpose:	default constructorGet the path associated with a given char
********************************************************************************************/

CharMetrics::CharMetrics()
{
	CharWidth   = 0;
	FontAscent  = 0;
	FontDescent = 0;
	FontEmWidth = 0;
}


/***************************************************************************************
>	BOOL CharMetrics::Scale(double ScaleX, double ScaleY)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/96
	Inputs:		ScaleX - 
				ScaleY - 
	Purpose:	Scale the metrics by the x/y scale factors
********************************************************************************************/

void CharMetrics::Scale(double ScaleX, double ScaleY)
{
	CharWidth   = (MILLIPOINT)(CharWidth   * ScaleX + 0.5);
	FontEmWidth = (MILLIPOINT)(FontEmWidth * ScaleX + 0.5);
	FontAscent  = (MILLIPOINT)(FontAscent  * ScaleY + 0.5);
	FontDescent = (MILLIPOINT)(FontDescent * ScaleY + 0.5);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// TextManager

/***************************************************************************************
>	static BOOL TextManager::GetTTCharPath(CharDescription& ChDesc, DocCoord** ppCoords,
										 PathVerb** ppVerbs, UINT32* pNumCoords, CDC* pDC=NULL)

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		ChDesc= description of char
				pDC= (possible) pointer to a DC (for optimisation) defaults to NULL
	Outputs:	ppCoords=>pointer to path coords buffer
				ppVerbs=>pointer to path verbs buffer
				pNumCoords=number of elements in path
	Returns:	FALSE if fails
	Purpose:	Get the path associated with a given char
	Global:		Assumes MM_TextMode	
********************************************************************************************/

BOOL TextManager::GetTTCharPath(CharDescription& ChDesc, DocCoord** ppCoords,
							  PathVerb** ppVerbs, UINT32* pNumCoords, CDC* pDC)
{
	ERROR2IF(ppCoords==NULL,FALSE,"TextManager::GetTTCharPath pCoords==NULL");
	ERROR2IF(ppVerbs==NULL,FALSE,"TextManager::GetTTCharPath ppVerbs==NULL");
	ERROR2IF(pNumCoords==NULL,FALSE,"TextManager::GetTTCharPath pNumCoords==NULL");

	const  CharPathBufferSize=2048;
	static DocCoord CharPathCoordArray[CharPathBufferSize];
	static PathVerb CharPathVerbArray[CharPathBufferSize];

	// ensure we have a DC
	BOOL LocalDC=(pDC==NULL);
	if (LocalDC)
	{
		pDC=new CDC;
		if (pDC==NULL)
		{
			ERROR3("TextManager::GetCharPath() - failed to create DC");
			return FALSE;
		}
		if (pDC->CreateCompatibleDC(NULL)==FALSE)
		{
			delete pDC;
			ERROR3("TextManager::GetCharPath() - CreateCompatibleDC() failed");
			return FALSE;
		}
	}
	
	// get a log font from the char description, then the path data from the logfont
	LOGFONT	CharLogFont;
	BOOL ok=GetLogFontFromCharDescriptor(pDC, ChDesc, &CharLogFont);
	UINT32 Coords=CharPathBufferSize;
	if (ok)
	{
		ok=GetBezierFromChar(pDC, ChDesc.GetCharCode(), &CharLogFont, &Coords, (POINT*)CharPathCoordArray, CharPathVerbArray);
		
		// If that failed try again with the default character
		if (!ok)
		{
			// Get the default character to use if a charater is not present in the font.
			WCHAR DefChar = (unsigned char)'?';
			TEXTMETRIC FontTextData;
#ifdef _UNCCODE
			if (pDC->GetTextMetrics(&FontTextData))
				DefChar = FontTextData.tmDefaultChar;
#else
			if (pDC->GetTextMetrics(&FontTextData))
				DefChar = (unsigned char)FontTextData.tmDefaultChar;
#endif

			ok = GetBezierFromChar(pDC, DefChar, &CharLogFont, &Coords, (POINT*)CharPathCoordArray, CharPathVerbArray);
		}

		ERROR3IF(!ok, "TextManager::GetCharPath error from GetBezierFromChar");
	}

	// if ok, set outputs, tidy up and return
	if (ok)
	{
		*ppCoords   = CharPathCoordArray;
		*ppVerbs    = CharPathVerbArray;
		*pNumCoords = Coords;
	}
	if (LocalDC) delete pDC;
	return ok;
}


/***************************************************************************************
>	static BOOL TextManager::GetBezierFromChar(CDC* pDC,const WCHAR CharNumber,
											   LPLOGFONT pLogFont, DWORD* NoPolyElements,
											   POINT* pPolyCordBuffer, BYTE* pPolyVerbBuffer)

	Author:		Mark_Goodall (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Purpose:	To generate the bezier paths which represent a character from a TrueType and, eventually, an ATM, font character
	Inputs:		pDC=>device context
				CharNumber=character number of char	this is a wide Unicode value
				pLogFontStruct=>LOGFONT struct defining char, the size field is in logcords (=device cords in MM_TextMode) 
				NoPolyElements=Number of elements in PolyDraw arrays, 
					If NoPolyElements=0	then NoPolyElements is set to no. of elements required
					If NoPolyElements<>0 then generate paths in PolyDraw arrays
				pPolyCordBuffer=>Cord array in PolyDraw system
				pPolyVerbBuffer=>Verb array in PolyDraw system
	Global:		Assumes MM_TextMode	
	Outputs:	NoPolyElements=No. required if entry value=0
				PolyDrawArrays (Verb,Cord) filled if NoPolyElements<>0, CordArrays hold cords in 1/72000's
	Returns:	If FALSE returned then error has occured
********************************************************************************************/
		
#define FIXEDTOINT32(FIXED) (*((INT32 *)&FIXED))
#define SCALEX(jobby) (MulDiv(MulDiv(jobby,72000,DivConstX),pLogFont->lfHeight,-4096))
#define SCALEY(jobby) (MulDiv(MulDiv(jobby,72000,DivConstY),pLogFont->lfHeight,-4096))  
   
BOOL TextManager::GetBezierFromChar(CDC* pDC,WCHAR CharNumber,
 LPLOGFONT pLogFont, DWORD* NoPolyElements, POINT* pPolyCordBuffer, BYTE* pPolyVerbBuffer)
{
	if (*NoPolyElements==0)
	{ 
		*NoPolyElements=2000; // Assume that 500 element array is big enough, although we still check on this later!!
		return TRUE;
	}		 

	INT32 DivConstX=pDC->GetDeviceCaps(LOGPIXELSX)<<16;
	INT32 DivConstY=pDC->GetDeviceCaps(LOGPIXELSY)<<16;
	
	LOGFONT TempLogFont=*pLogFont;
	TempLogFont.lfHeight=-4096;//This ought to be -2048 but this doesn't return correct outlines on Times Italic on o's etc
	TempLogFont.lfWidth=0;
	CFont UnHintedCFont;
	UnHintedCFont.CreateFontIndirect(&TempLogFont);
	CFont* pOldCFont=pDC->SelectObject(&UnHintedCFont);
	
	MAT2 DefaultMatrix={ {0,1} ,{0,0}, {0,0}, {0,1} };
	TEXTMETRIC ATextMetricStruct;
	pDC->GetTextMetrics(&ATextMetricStruct);
	GLYPHMETRICS GlyphMetricsBuffer;
		
	// Get the size of the glyph data
	DWORD SizeOfGlyphData = (DWORD)-1;
	UINT32 MBChar = 0;
	if (UnicodeManager::IsUnicodeCompleteOS())
		SizeOfGlyphData = GetGlyphOutlineW(pDC->GetSafeHdc(), CharNumber, GGO_NATIVE, &GlyphMetricsBuffer, 0, NULL, &DefaultMatrix);
 	else
	{
		MBChar = UnicodeManager::UnicodeToMultiByte(CharNumber);
		SizeOfGlyphData = GetGlyphOutlineA(pDC->GetSafeHdc(), MBChar, GGO_NATIVE, &GlyphMetricsBuffer, 0, NULL, &DefaultMatrix);
	}
	if (SizeOfGlyphData==-1)
		return FALSE;

 	// claim memory for glyph data
	BYTE* pGlyphBuffer= new BYTE[SizeOfGlyphData];
	if (pGlyphBuffer == NULL)
		return FALSE;

	// Get the glyph outline data
	DWORD Error = (DWORD)-1;
	if (UnicodeManager::IsUnicodeCompleteOS())
		Error = GetGlyphOutlineW(pDC->GetSafeHdc(), CharNumber, GGO_NATIVE, &GlyphMetricsBuffer, SizeOfGlyphData, pGlyphBuffer, &DefaultMatrix);
	else
		Error = GetGlyphOutlineA(pDC->GetSafeHdc(), MBChar, GGO_NATIVE, &GlyphMetricsBuffer, SizeOfGlyphData, pGlyphBuffer, &DefaultMatrix);

	if (Error == -1)
 	{
		delete[] pGlyphBuffer;
		ERROR2(FALSE, "GetGlyphOutline failed");
	}

	//Lets restore the DC font status
	pDC->SelectObject(pOldCFont);
	
	// We now have the GlyphOutline structure in the GlyphBuffer, this consists of TTPolygon etc headers 
	// and records
	
	BYTE* pPosInGlyphBuffer=(BYTE*)pGlyphBuffer;// pointer to our current position in GlyphBuffer
	DWORD ContourSize;		   					// size of data controlled by a PolyHeader structure
	TTPOLYGONHEADER* pPolyHeader;				// needed to allow access to structure data
	TTPOLYCURVE* pPolyCurve;					// ditto
	DWORD	CurrentPolyIndex=0;					// Current index in the output PolyBuffer holding beziers etc
	WORD 	NoOfArrayElements;					// No of elements in a Qspline or prim_line record
	INT32	CurrentX;							// Last point of current path (line or quad b-spline) or previous one
	INT32	CurrentY;
	DWORD	FirstContourIndex;					// The index in PolyCordBuffer of the first member of the current contour

	while (pPosInGlyphBuffer<(BYTE*)pGlyphBuffer+SizeOfGlyphData) 
	{
		// Right lets process a TTPOLYGONHEADER and its associated spline and line records	
		pPolyHeader=(TTPOLYGONHEADER*)pPosInGlyphBuffer;
		ContourSize=pPolyHeader->cb;
		CurrentX=FIXEDTOINT32(pPolyHeader->pfxStart.x);
		CurrentY=FIXEDTOINT32(pPolyHeader->pfxStart.y);
		pPolyCordBuffer[CurrentPolyIndex].x=SCALEX(CurrentX); // First entry is always MoveTo to set 1st point of bezier etc
		pPolyCordBuffer[CurrentPolyIndex].y=SCALEY(CurrentY);
		pPolyVerbBuffer[CurrentPolyIndex]  =PT_MOVETO;
		FirstContourIndex=CurrentPolyIndex;
		CurrentPolyIndex++;							 
						   		
		pPosInGlyphBuffer=pPosInGlyphBuffer+sizeof(TTPOLYGONHEADER);  // pPosInGlyphBuffer=>1st TTPOLYCURVE
		
		// Lets process all the TTPOLYGONCURVE structs that exist within the current TTPOLYGONHEADER domain (contour)
		do
		{
			if (CurrentPolyIndex+4>=*NoPolyElements)				  // are we overflowing poly draw buffer
			{
				delete[] pGlyphBuffer;
				TRACE( _T("overflowing polydraw buffer"));
				return FALSE;
			}

			pPolyCurve=(TTPOLYCURVE*)pPosInGlyphBuffer;
			NoOfArrayElements=pPolyCurve->cpfx;
			
			DWORD i;
			switch (pPolyCurve->wType)
			{	
				case TT_PRIM_LINE:
				for	(i=0;i<NoOfArrayElements;i++)
				{
					CurrentX=FIXEDTOINT32(pPolyCurve->apfx[i].x);
					pPolyCordBuffer[CurrentPolyIndex].x=SCALEX(CurrentX); // First entry is always MoveTo to set 1st point of bezier etc
					CurrentY=FIXEDTOINT32(pPolyCurve->apfx[i].y);
					pPolyCordBuffer[CurrentPolyIndex].y=SCALEY(CurrentY);
					pPolyVerbBuffer[CurrentPolyIndex]=PT_LINETO;
					CurrentPolyIndex++;
				}
				break;

				
				case TT_PRIM_QSPLINE:
				
				for ( i=0;i+1<NoOfArrayElements;i++)
				{
					pPolyVerbBuffer[CurrentPolyIndex]=PT_BEZIERTO;
					pPolyVerbBuffer[CurrentPolyIndex+1]=PT_BEZIERTO;
					pPolyVerbBuffer[CurrentPolyIndex+2]=PT_BEZIERTO;

					//TRACE( _T("i=%d\n"),i);
					// Lets set CurrentPolyIndex [+0] & [+1]
					pPolyCordBuffer[CurrentPolyIndex].x=
// This can overflow - best to scale the components separately
//					SCALEX((CurrentX+((FIXEDTOINT32(pPolyCurve->apfx[i].x))<<1))/3);
					SCALEX(CurrentX/3) + SCALEX(((FIXEDTOINT32(pPolyCurve->apfx[i].x))<<1)/3);
					
					pPolyCordBuffer[CurrentPolyIndex].y=
// This can overflow - best to scale the components separately
//					SCALEY((CurrentY+((FIXEDTOINT32(pPolyCurve->apfx[i].y))<<1))/3);
					SCALEY(CurrentY/3) + SCALEY(((FIXEDTOINT32(pPolyCurve->apfx[i].y))<<1)/3);
					
					if (i+2==NoOfArrayElements)	 // are we on last processable point in record?
					{ // yes we are, so just get last point
						CurrentX=FIXEDTOINT32(pPolyCurve->apfx[i+1].x);
						pPolyCordBuffer[CurrentPolyIndex+2].x=SCALEX(CurrentX);
						CurrentY=FIXEDTOINT32(pPolyCurve->apfx[i+1].y);	
						pPolyCordBuffer[CurrentPolyIndex+2].y=SCALEY(CurrentY);	
					}
					else 
					{ // no we're not, so we have to calculate midpoint between the two following points
						CurrentX=(FIXEDTOINT32(pPolyCurve->apfx[i].x)+FIXEDTOINT32(pPolyCurve->apfx[i+1].x))>>1;
						pPolyCordBuffer[CurrentPolyIndex+2].x=SCALEX(CurrentX);
						CurrentY=(FIXEDTOINT32(pPolyCurve->apfx[i].y)+FIXEDTOINT32(pPolyCurve->apfx[i+1].y))>>1;
						pPolyCordBuffer[CurrentPolyIndex+2].y=SCALEY(CurrentY);
					}

					pPolyCordBuffer[CurrentPolyIndex+1].x=
// This can overflow - best to scale the components separately
//					SCALEX((CurrentX+((FIXEDTOINT32(pPolyCurve->apfx[i].x))<<1))/3);
					SCALEX(CurrentX/3) + SCALEX(((FIXEDTOINT32(pPolyCurve->apfx[i].x))<<1)/3);
					
					pPolyCordBuffer[CurrentPolyIndex+1].y=
// This can overflow - best to scale the components separately
//					SCALEY((CurrentY+((FIXEDTOINT32(pPolyCurve->apfx[i].y))<<1))/3);
					SCALEY(CurrentY/3) + SCALEY(((FIXEDTOINT32(pPolyCurve->apfx[i].y))<<1)/3);
					
					CurrentPolyIndex=CurrentPolyIndex+3;

				}
				break;
				
				default:
				delete[] pGlyphBuffer;
				TRACE( _T("oh my god\n"));
				AfxThrowMemoryException();
			}

			pPosInGlyphBuffer=pPosInGlyphBuffer+sizeof(TTPOLYCURVE)+(NoOfArrayElements-1)*sizeof(POINTFX);

		} while (pPosInGlyphBuffer<(BYTE*)pPolyHeader+ContourSize);
		
		if ( (pPolyCordBuffer[FirstContourIndex].x != pPolyCordBuffer[CurrentPolyIndex-1].x) ||
				 (pPolyCordBuffer[FirstContourIndex].y != pPolyCordBuffer[CurrentPolyIndex-1].y))
		{
			pPolyVerbBuffer[CurrentPolyIndex]=PT_LINETO | PT_CLOSEFIGURE;
			pPolyCordBuffer[CurrentPolyIndex]=pPolyCordBuffer[FirstContourIndex];
			CurrentPolyIndex++;
		}
		else 
		{
			pPolyVerbBuffer[CurrentPolyIndex-1]=pPolyVerbBuffer[CurrentPolyIndex-1] | PT_CLOSEFIGURE;
 		}

	}		
	delete[] pGlyphBuffer;

	*NoPolyElements=CurrentPolyIndex;
	return TRUE; 
}


/*********************************************************************************************
>   static INT32 TextManager::GetDesignSize(CDC* pDC)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		pDC - ptr to DC with font selected, for which design size required
	Returns:	design size of font in logical units (or -1 if fails)
	Note:		>>>> just returns a constant for the moment, but gives reasonable accuracy <<<<
********************************************************************************************/

INT32 TextManager::GetDesignSize(CDC* pDC)
{
//	ERROR2IF(pDC==NULL,FALSE,"TextManager::GetDesignSize() - pDC==NULL");

	return 2048;
}


/**********************************************************************************************
>	static BOOL TextManager::GetLogFontFromCharDescriptor(CDC* pDC, CharDescription& ChDesc,
						LPLOGFONT pLogFont, INT32 LogicalHeight=-1)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		pDC           - ptr to DC
				ChDesc        - CharDescription containing font handle, bold, italic
				LogicalHeight - desired height in DC pixels (or -1 for dpi - ie 1" high)
	Outputs:	pLogFont      - Logfont corresponding to given ChDesc at specified height
	Returns:	FALSE if fails
	Purpose:	Given a char descriptor, get its corresponding Logfont, and Font metrics if required
	Note:		Assumes MM_TextMode	
*********************************************************************************************/

BOOL TextManager::GetLogFontFromCharDescriptor(CDC* pDC, CharDescription& ChDesc,
											LOGFONT* pLogFont, INT32 LogicalHeight)
{
	ERROR2IF(pLogFont==NULL,FALSE,"TextManager::GetLogFontFromCharDescriptor() pLogFont==NULL");
	ERROR2IF(     pDC==NULL,FALSE,"TextManager::GetLogFontFromCharDescriptor() pDC==NULL");

	// get LogFont from face handle in char descriptor
	WORD            FaceHandle    = ChDesc.GetTypefaceHandle();
	CachedFontItem* pFontDataItem = FONTMANAGER->GetFont(FaceHandle);
  	ENUMLOGFONT*    pEnumLogFont     = pFontDataItem->GetEnumLogFont();
  	*pLogFont = pEnumLogFont->elfLogFont;

	// if not specified, set LogicalHeight to dpi (ie 1")
	if (LogicalHeight==-1)
		LogicalHeight = pDC->GetDeviceCaps(LOGPIXELSY);

	// set font height, aspect (100%), bold, italic
 	pLogFont->lfHeight = -LogicalHeight;
 	pLogFont->lfWidth  = 0;
	pLogFont->lfWeight = ChDesc.GetBold() ? 700 : 0; 
	pLogFont->lfItalic = ChDesc.GetItalic();

	return TRUE;
}


/********************************************************************************************
>	static BOOL TextManager::GetInfoFromLogFont(FontInfo* pFontInfo, LOGFONT* pLogFont,
																 FontClass Class = FC_UNDEFINED);
	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95
	Inputs:		pLogFont  - Log font to obtain info from
				Class	  - describes the type of class (see typedef for details)
							FC_UNDEFINED then ignore the class match and match the first
							item with the same name whatever font class it is
	Outputs:	pFontInfo - Fontinfo structure built from given log font
	Returns:	FALSE if fails
	Purpose:	extract as many text attributes as possible from a LOGFONT structure
	Note:		assumes the font is cached
********************************************************************************************/

BOOL TextManager::GetInfoFromLogFont(FontInfo* pFontInfo, LOGFONT* pLogFont, FontClass Class)
{
// BODGE TEXT - no error trapping of creating new MFC objects - TRY/THROW required

	// ensure the font is cached
	String_64 Desc(pLogFont->lfFaceName);
	if (FONTMANAGER->CacheNamedFont(&Desc, Class)==ILLEGALFHANDLE)
		return FALSE;

	// create DC and get it's pixels per inch
	CDC ADC;
	if (ADC.CreateCompatibleDC(NULL)==0)
		ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - CreateCompatibleDC() failed");
	INT32 PixelsPerInch=ADC.GetDeviceCaps(LOGPIXELSX);
	if (PixelsPerInch==0)
		ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - GetDeviceCaps() failed");

	// create a font from the LOGFONT but of default width
	INT32 DefaultAveCharWidth=0;
	if (pLogFont->lfWidth!=0)
	{
		LOGFONT DefaultWidthLogFont=*pLogFont;
		DefaultWidthLogFont.lfWidth=0;
		CFont DefaultWidthFont;
		if (DefaultWidthFont.CreateFontIndirect(&DefaultWidthLogFont)==0)
			ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - CreateFontIndirect() failed");
		CFont* pOldFont=ADC.SelectObject(&DefaultWidthFont);
		ERROR2IF(pOldFont==NULL,FALSE,"TextManager::GetInfoFromLogFont() - SelectObject() failed");
		TEXTMETRIC TM;
		if (ADC.GetTextMetrics(&TM)==0)
			ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - GetTextMetrics() failed");
		DefaultAveCharWidth=TM.tmAveCharWidth;
		if (ADC.SelectObject(pOldFont)==NULL)
			ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - SelectObject() failed");
	}

	// create font from LOGFONT, select it into DC, and get metrics
	CFont font;
	if (font.CreateFontIndirect(pLogFont)==0)
		ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - CreateFontIndirect() failed");
	CFont* pOldFont=ADC.SelectObject(&font);
	ERROR2IF(pOldFont==NULL,FALSE,"TextManager::GetInfoFromLogFont() - SelectObject() failed");
	TEXTMETRIC TM;
	if (ADC.GetTextMetrics(&TM)==0)
		ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - GetTextMetrics() failed");
	if (ADC.SelectObject(pOldFont)==NULL)
		ERROR2(FALSE,"TextManager::GetInfoFromLogFont() - SelectObject() failed");

	MILLIPOINT FontSize = MulDiv(TM.tmHeight-TM.tmInternalLeading,72000,PixelsPerInch);
	ERROR2IF(FontSize==-1,FALSE,"TextManager::GetInfoFromLogFont() - MulDiv() failed");
	FIXED16 FontAspect = DefaultAveCharWidth!=0 ? Div32By32(TM.tmAveCharWidth,DefaultAveCharWidth) : 1;
	ERROR2IF(FontAspect==-1,FALSE,"TextManager::GetInfoFromLogFont() - MulDiv() failed");

	pFontInfo->Handle   = FONTMANAGER->GetFontHandle(&Desc, Class);
	pFontInfo->Size     = FontSize;
	pFontInfo->Aspect   = FontAspect;
	pFontInfo->Bold     = pLogFont->lfWeight >  FW_MEDIUM;
	pFontInfo->Italic   = pLogFont->lfItalic != FALSE;
	pFontInfo->Rotation = pLogFont->lfEscapement / 10.0;

	return TRUE;
}


/********************************************************************************************
>	static CharCase TextManager::ProcessCharCase(WCHAR* pChar, CharCase NewState)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Inputs:		pChar - pointer to char to process
				NewState - Upper/Lower/Swap/Read
	Ouptuts:	pChar - modified char (if required)
	Returns:	previous state of char (Upper/Lower/Unknown) or Failed
	Purpose:	process the case of a char (accounting for Unicode and non-unicode builds!)
********************************************************************************************/

CharCase TextManager::ProcessCharCase(WCHAR* pChar, CharCase NewState)
{
	ERROR2IF(pChar==NULL,Failed,"TextManager::ProcessCharCase() - pChar==NULL");
	ERROR2IF(NewState==Failed || NewState==Unknown,Failed,"TextManager::ProcessCharCase() - invalid NewState");

	WCHAR OldCharW = *pChar;
	CHAR OldCharA = UnicodeManager::UnicodeToMultiByte(OldCharW);
	CharCase OldCase=Unknown;

	// get a lower case version of the char (if it changes it must have been upper)
	WCHAR LowerCharW;
	if (UnicodeManager::IsUnicodeCompleteOS())
	{
		WCHAR pLowerChar[2]={OldCharW,0};
		::CharLowerW(pLowerChar);								
		if (*pLowerChar != OldCharW)
			OldCase = Upper;
		LowerCharW = *pLowerChar;
	}
	else
	{
		CHAR pLowerChar[2]={OldCharA,0};
	  	::CharLowerA(pLowerChar);								
		if (*pLowerChar != OldCharA)
			OldCase = Upper;
		LowerCharW = UnicodeManager::MultiByteToUnicode(*pLowerChar);
	}

	// get an upper case version of the char (if it changes it must have been lower)
	WCHAR UpperCharW;
	if (UnicodeManager::IsUnicodeCompleteOS())
	{
		WCHAR pUpperChar[2]={OldCharW,0};
		::CharUpperW(pUpperChar);								
		if (*pUpperChar != OldCharW)
			OldCase = Lower;
		UpperCharW = *pUpperChar;
	}
	else
	{
		CHAR pUpperChar[2]={OldCharA,0};
	  	::CharUpperA(pUpperChar);								
		if (*pUpperChar != OldCharA)
			OldCase = Lower;
		UpperCharW = UnicodeManager::MultiByteToUnicode(*pUpperChar);
	}

	// if its case can be changed and we want to change it do it!
	if (OldCase!=Unknown && NewState!=Read)
	{
		if (NewState==Swap)
			NewState = OldCase==Lower ? Upper : Lower;
		WCHAR NewChar = NewState==Lower ? LowerCharW : UpperCharW;
		*pChar = NewChar;
	}

	return OldCase;
}



/********************************************************************************************
>	BOOL TextManager::GetCharWidth(CDC* pDC, WCHAR FirstChar, WCHAR, LastChar, INT32* pCharWidthsBuf)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		pDC                 - pointer to DC with correct font selected
				FirstChar, LastChar - range of chars for which char widths required
	Outputs:	pCharWidthsBuf      - buffer filled with char widths
	Returns:	FALSE if fails
	Purpose:	Interface to API call, dynamically accounting for Unicode ability of OS
	Note:		pCharWidthsBuf size must be LastChar-FirstChar+1
********************************************************************************************/

BOOL TextManager::GetCharWidth(CDC* pDC, WCHAR FirstChar, WCHAR LastChar, INT32* pCharWidthsBuf)
{
	ERROR2IF(           pDC==NULL,FALSE,"TextManager::GetCharWidth() - pDC==NULL");
	ERROR2IF(pCharWidthsBuf==NULL,FALSE,"TextManager::GetCharWidth() - pCharWidthsBuf==NULL");

	BOOL ok=TRUE;
	if (UnicodeManager::IsUnicodeCompleteOS())
		ok = ::GetCharWidthW(pDC->GetSafeHdc(), FirstChar, LastChar, pCharWidthsBuf);
	else
	{
		UINT32 FirstMBChar = UnicodeManager::UnicodeToMultiByte(FirstChar);
		UINT32 LastMBChar  = UnicodeManager::UnicodeToMultiByte(LastChar);
		ok = ::GetCharWidthA(pDC->GetSafeHdc(), FirstMBChar, LastMBChar, pCharWidthsBuf);
	}

	ERROR2IF(!ok,FALSE,"TextManager::GetCharWidth() - ::GetCharWidth() failed");

	return TRUE;
}


/********************************************************************************************
>	BOOL TextManager::GetCharABCWidths(CDC* pDC, WCHAR FirstChar, WCHAR, LastChar, INT32* pCharABCsBuf)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/96
	Inputs:		pDC                 - pointer to DC with correct font selected
				FirstChar, LastChar - range of chars for which char ABC widths required
	Outputs:	pCharABCsBuf   - buffer filled with char ABC widths
	Returns:	FALSE if fails
	Purpose:	Interface to API call, dynamically accounting for Unicode ability of OS
	Note:		pABCWidthsBuf size must be LastChar-FirstChar+1
********************************************************************************************/

BOOL TextManager::GetCharABCWidths(CDC* pDC, WCHAR FirstChar, WCHAR LastChar, ABC* pCharABCsBuf)
{
	ERROR2IF(              pDC==NULL,FALSE,"TextManager::GetABCWidths() - pDC==NULL");
	ERROR2IF(pCharABCsBuf==NULL,FALSE,"TextManager::GetABCWidths() - pCharABCsBuf==NULL");

	BOOL ok=TRUE;
	if (UnicodeManager::IsUnicodeCompleteOS())
		ok = ::GetCharABCWidthsW(pDC->GetSafeHdc(), FirstChar, LastChar, pCharABCsBuf);
	else
	{
		UINT32 FirstMBChar = UnicodeManager::UnicodeToMultiByte(FirstChar);
		UINT32 LastMBChar  = UnicodeManager::UnicodeToMultiByte(LastChar);
		ok = ::GetCharABCWidthsA(pDC->GetSafeHdc(), FirstMBChar, LastMBChar, pCharABCsBuf);
	}

	ERROR2IF(!ok,FALSE,"TextManager::GetABCWidths() - ::GetCharABCWidths() failed");

	return TRUE;
}


/********************************************************************************************

>	INT32	TextManager::GetKernCount(CDC* pDC)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		pDC				- DC with currently selected font
	Returns:	Number of kerns (0 if there are no kerns or there is an error)
	Purpose:	Find the number of kerns for the currently selected font
	See also:	TextManager::FillKernArray()

********************************************************************************************/
INT32 TextManager::GetKernCount(CDC* pDC)
{
	ERROR2IF(pDC==0,0,"TextManager::GetKernCount() passed null DC");

	return pDC->GetKerningPairs(0, 0);
}



/********************************************************************************************

>	bool TextManager::FillKernArray(CDC* pDC, KERNINGPAIR *pKerningPairs, INT32 count)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		pDC				- DC with currently selected font
				count			- Size of the array (must be >= GetKernCount() )
	Outputs		pKerningPairs	- Pointer to an array to fill with the kerning pairs
	Returns:	true if all is OK or false if things go real bad
	Purpose:	Fills an array with kerning data for the currently selected font	
	See also:	TextManager::GetKernCount()

********************************************************************************************/
bool TextManager::FillKernArray(CDC* pDC, MillipointKerningPair* pKerningPairs, INT32 count)
{
	ERROR2IF(pDC==0, 0,					"TextManager::FillKernArray() passed null DC");
	ERROR3IF(count<GetKernCount(pDC),	"TextManager::FillKernArray() not passed enough memory - "
										"kern table will be incomplete");

	// Allocate some tmp memory to store windows kerning data
	KERNINGPAIR* pKPtmp = new KERNINGPAIR[count];
	if (!pKPtmp) return false;

	// Get windows kerning data
	INT32 number = pDC->GetKerningPairs(count, pKPtmp);

	// Convert windows data into our own format
	for (INT32 c = 0; c < number; ++c)
	{
		pKerningPairs[c].wFirst			= pKPtmp[c].wFirst;
		pKerningPairs[c].wSecond		= pKPtmp[c].wSecond;
		pKerningPairs[c].iKernAmount	= pKPtmp[c].iKernAmount;
	}
	
	// Delete tmp memory
	delete pKPtmp;

	// Check for error condition
	if (number == 0 && GetKernCount(pDC) != 0)
	{
		ERROR3("Error reading kerning data");
		return false;
	}
	else
	{
		return true;
	}
}
