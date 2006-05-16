// $Header: /camelot/winoil/BFXPIXOP.CPP 3     1/05/95 14:25 Alex $
// This file implents the bitmap pixel operations

/*
*/

#include "camtypes.h"
#include "errors.h"
#include "bfxpixop.h"
#include "bfxalu.h"
#include "bitmap.h"
#include "wbitmap.h"

// The asm file defines FASTxxxxxx if there are fast versions of the routeines available
#include "bfxasm.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 3 $");

CC_IMPLEMENT_DYNCREATE(BfxPixelOp, CCObject)
CC_IMPLEMENT_DYNCREATE(BfxPixelOp32, BfxPixelOp)
CC_IMPLEMENT_DYNCREATE(BfxPixelOp24, BfxPixelOp)
CC_IMPLEMENT_DYNCREATE(BfxPixelOpPseudo, BfxPixelOp)
CC_IMPLEMENT_DYNCREATE(BfxThresholdPixelOp, BfxPixelOp32)
CC_IMPLEMENT_DYNCREATE(BfxColourThresholdPixelOp, BfxPixelOp32)
CC_IMPLEMENT_DYNCREATE(BfxPositivePixelOp, BfxPixelOp32)
CC_IMPLEMENT_DYNCREATE(BfxThresholdPixelOpPseudo, BfxPixelOpPseudo)
CC_IMPLEMENT_DYNCREATE(BfxColourThresholdPixelOpPseudo, BfxPixelOpPseudo)
CC_IMPLEMENT_DYNCREATE(BfxPositivePixelOpPseudo, BfxPixelOpPseudo)
CC_IMPLEMENT_DYNCREATE(Octree, CCObject)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BfxPixelOp::BfxPixelOp()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for pixel op
	Errors:		None yet
	SeeAlso:	-

********************************************************************************************/

BfxPixelOp::BfxPixelOp()
{
	Base=NULL;
	Height=0;
	Width=WidthRounded=0;
	Size=0;
	DefaultValue=FALSE;
	Reset();
	pA=NULL;
	pB=NULL;
	pT=NULL;
	Value=0;
	BPP=32;
}

/********************************************************************************************

>	BfxPixelOp::~BfxPixelOp()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default destructor for pixel op
	Errors:		Error3 if DeInit hasn't been called.
	SeeAlso:	-

********************************************************************************************/

BfxPixelOp::~BfxPixelOp()
{
}

/********************************************************************************************

>	BOOL BfxPixelOp::Reset()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Resets all parameters associated with the cache
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Resets all parameters associated with the cache
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOp::Reset()
{
	return TRUE;
}



/********************************************************************************************

>	BOOL BfxPixelOp::SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask,
							  BOOL theDefaultValue);

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		pKB = pointer to the bitmap
				theCacheStateMask = bit mask used to show cache information
				theCacheValueMask = bit mask used to show cache value information
				theDefaultValue = BOOL value associated with pixels outside the bitmap
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Class set up to use bitmap passed in
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOp::SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask,
								  BOOL theDefaultValue)
{
	Base = NULL;
	ERROR2IF( ((!pKB) || (pKB->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(pKB->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF( (BPP && pBMI->biBitCount != BPP), FALSE, "BfxPixelOp called with incorrect BPP");

	Base = (DWORD *)(void *)(((WinBitmap *)(pKB->ActualBitmap))->BMBytes);
	Width = pBMI->biWidth;
	LONG Bits = pBMI->biBitCount;
	switch (Bits)
	{
		case 1 : WidthRounded = (Width+31) &~31; break;
		case 2 : WidthRounded = (Width+15) &~15; break;
		case 4 : WidthRounded = (Width+7) &~7; break;
		case 8 : WidthRounded = (Width+3) &~3; break;
		case 16: WidthRounded = (Width+1) &~1; break;
		case 32:
		default:
				 WidthRounded = Width; break;
	}

 	Height = pBMI->biHeight;
	Size = (pBMI->biSizeImage)>>2;

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxPixelOp::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		Always errors. The base class should never be called
	Scope:		Public
	SeeAlso:	BfxPixelOp::IsInRegion()

Outisiders should used the cached version BfxPixelOp::IsInRegion()

********************************************************************************************/

BOOL BfxPixelOp::IsPixelReallyInRegion(LONG x, LONG y)
{
	ERROR3("How come base class BfxPixelOp::IsPixelReallyInRegion has got called?");
	return DefaultValue;
}

/********************************************************************************************

>	BOOL BfxPixelOp::IsInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not (uses caching)
	Purpose:	To be overriden by derived classes
	Errors:		Always errors. The base class should never be called
	Scope:		Public
	SeeAlso:	BfxPixelOp::IsPixelReallyInRegion()

NB This function is inline

********************************************************************************************/

/********************************************************************************************

>	BOOL BfxPixelOp::SetAuxilliaryBitmaps(KernelBitmap * pProposed, KernelBitmap * pCurrent,
								  KernelBitmap * pOriginal, LONG Threshold, DWORD theColour);
				
	Author:		Alex
	Created:	14/03/95
	Inputs:		pProposed = the proposed bitmap (with new proposed grad fill)
				pCurrent = the current bitmap (as built up)
				pOriginal = the original image (to trace)
				Threshold = new threshold
				theColour = Colour to look for
	Outputs:	None
	Returns:	TRUE on success else FALSE & error set
	Purpose:	Sets up auxilliary bitmaps for derived classes
	Errors:		Several
	Scope:		Public
	SeeAlso:	BfxPixelOp::IsPixelReallyInRegion()

The bitmaps may be NULL if you don't want to use them

********************************************************************************************/

BOOL BfxPixelOp::SetAuxilliaryBitmaps(KernelBitmap * pProposed /*A*/, KernelBitmap * pCurrent /*B*/,
							  KernelBitmap * pOriginal /*T*/, LONG Threshold, DWORD theColour)
{
	if (pProposed)
	{
		ERROR2IF( (pProposed->ActualBitmap==NULL) ,FALSE,"BfxPixelOp can't find OIL bitmap");
		ERROR3IF( (!(pProposed->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxPixelOp Oil layer inconsistency");
		
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pProposed->ActualBitmap))->BMInfo->bmiHeader);
		
		ERROR2IF( ((pBMI->biHeight != Height) || (pBMI->biWidth != Width) || (BPP && pBMI->biBitCount !=BPP)), FALSE,
			  "Incompatible bitmaps for BfxPixelOp::SetAuxilliaryBitmap");
		pA = (DWORD *)(void *)(((WinBitmap *)(pProposed->ActualBitmap))->BMBytes);
	}
	else
	{
		pA = NULL;
	}

	if (pOriginal)
	{
		ERROR2IF( (pOriginal->ActualBitmap==NULL) ,FALSE,"BfxPixelOp can't find OIL bitmap");
		ERROR3IF( (!(pOriginal->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxPixelOp Oil layer inconsistency");
		
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pOriginal->ActualBitmap))->BMInfo->bmiHeader);
		
		ERROR2IF( ((pBMI->biHeight != Height) || (pBMI->biWidth != Width) || (BPP && pBMI->biBitCount !=BPP)), FALSE,
			  "Incompatible bitmaps for BfxPixelOp::SetAuxilliaryBitmap");
		pT = (DWORD *)(void *)(((WinBitmap *)(pOriginal->ActualBitmap))->BMBytes);
	}
	else
	{
		pT = NULL;
	}

	if (pCurrent)
	{
		ERROR2IF( (pCurrent->ActualBitmap==NULL) ,FALSE,"BfxPixelOp can't find OIL bitmap");
		ERROR3IF( (!(pCurrent->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxPixelOp Oil layer inconsistency");
		
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pCurrent->ActualBitmap))->BMInfo->bmiHeader);
		
		ERROR2IF( ((pBMI->biHeight != Height) || (pBMI->biWidth != Width) || (BPP && pBMI->biBitCount !=BPP)), FALSE,
			  "Incompatible bitmaps for BfxPixelOp::SetAuxilliaryBitmap");
		pB = (DWORD *)(void *)(((WinBitmap *)(pCurrent->ActualBitmap))->BMBytes);
	}
	else
	{
		pB = NULL;
	}

	
	Value = Threshold;
	Colour = theColour;
	return TRUE;
}

/********************************************************************************************

>	BfxPixelOp32::BfxPixelOp32()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for pixel op
	Errors:		None yet
	SeeAlso:	-

********************************************************************************************/

BfxPixelOp32::BfxPixelOp32() : BfxPixelOp()
{
	CacheStateMask=0;
	CacheValueMask=0;
	CacheMarks=0;
	BPP=32;
}

/********************************************************************************************

>	BfxPixelOp32::~BfxPixelOp32()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default destructor for pixel op
	Errors:		Error3 if DeInit hasn't been called.
	SeeAlso:	-

********************************************************************************************/

BfxPixelOp32::~BfxPixelOp32()
{
}

/********************************************************************************************

>	BOOL BfxPixelOp32::Reset()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Resets all parameters associated with the cache
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Resets all parameters associated with the cache
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOp32::Reset()
{
	MinX=(1<<30);
	MinY=(1<<30);
	MaxX=0;
	MaxY=0;
	CacheMarks = 0;
	return BfxPixelOp::Reset();
}



/********************************************************************************************

>	BOOL BfxPixelOp32::SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask,
							  BOOL theDefaultValue);

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		pKB = pointer to the bitmap
				theCacheStateMask = bit mask used to show cache information
				theCacheValueMask = bit mask used to show cache value information
				theDefaultValue = BOOL value associated with pixels outside the bitmap
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Class set up to use bitmap passed in
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOp32::SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask,
								  BOOL theDefaultValue)
{
	Base = NULL;
	ERROR2IF( ((!pKB) || (pKB->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(pKB->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");

	if (!BfxPixelOp::SetBitmap(pKB, theCacheStateMask,theCacheValueMask, theDefaultValue)) return FALSE;

	CacheStateMask = theCacheStateMask;
	CacheValueMask = theCacheValueMask;
	DefaultValue = theDefaultValue;

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxPixelOp32::ClearEntireCache()

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Clears the entire bitmap of caching information
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOp32::ClearEntireCache()
{
	MinX=0;
	MinY=0;
	MaxX=Width-1;
	MaxY=Height-1;
	return ClearCachedArea();
}

/********************************************************************************************

>	BOOL BfxPixelOp32::ClearCachedArea()

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Clears the caching information from the area in which it's been stored
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOp32::ClearCachedArea()
{
	ERROR2IF(!Base,FALSE,"BfxPixelOp32::SetBitmap not called");
	if ((MaxX<MinX) || (MaxY<MinY)) return TRUE;
	LONG x;
	LONG y;
	DWORD v=~(CacheValueMask | CacheStateMask);
	for (y=MinY; y<=MaxY; y++) for (x=MinX; x<=MaxX; x++) __bfxpixop_xy(x,y)&=v;
	Reset();
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxPixelOp32::IsInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not (uses caching)
	Purpose:	To be overriden by derived classes
	Errors:		Always errors. The base class should never be called
	Scope:		Public
	SeeAlso:	BfxPixelOp32::IsPixelReallyInRegion()

NB This function is inline

********************************************************************************************/

/********************************************************************************************

>	BOOL BfxPixelOp32::GetValueAdjustCache(LONG x, LONG y)
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not (and alters cache parameters)
	Purpose:	To be overriden by derived classes
	Errors:		Always errors. The base class should never be called
	Scope:		Public
	SeeAlso:	BfxPixelOp32::IsPixelReallyInRegion()

********************************************************************************************/

BOOL BfxPixelOp32::GetValueAdjustCache(LONG x, LONG y)
{
	if (x<MinX) MinX=x;
	if (x>MaxX) MaxX=x;
	if (y<MinY) MinY=y;
	if (y>MaxY) MaxY=y;
	BOOL state;
	// The following is disgusting but is time critical. Increment CacheMarks where we find
	// a previously undiscovered newly marked pixel
	CacheMarks+= (state = /*assign*/ IsPixelReallyInRegion(x,y));
	return state;
}

/********************************************************************************************

>	BOOL BfxPixelOp32::CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion)
					
	Author:		Alex
	Created:	17/1/95
	Inputs:		MinimumArea = minimum area 
	Outputs:	FoundRegion set as per whether it's found a region
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Determines whether atleast (MinimumArea) pixels are within the region
	Errors:		None yet
	Scope:		Public
	SeeAlso:	TraceControl::FloodSearch

FoundRegion is set according the the result. The search starts at InitialX/InitialY

This call *must* be done immediately (yes, immediately) after the cache is cleared. If not
you lose all your caching. Har har.

********************************************************************************************/

BOOL BfxPixelOp32::CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion)
{
	if (!ClearCachedArea()) return FALSE;	
	if (!FloodSearch(InitialX, InitialY, MinimumArea)) return FALSE;
	*FoundRegion = (GetCacheMarks() >= MinimumArea);
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxPixelOp32::FloodSearch(LONG x, LONG y, LONG MinimumArea)
					
	Author:		Alex
	Created:	17/1/95
	Inputs:		x = start x
				y = start y
				MinimumArea = minimum area
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Recursively flood fills an area until >=MinimumArea pixels are found
	Errors:		None yet
	Scope:		Public
	SeeAlso:	BfxPixelOp32::CheckMinimumArea, TraceControl::CheckMinimumArea

***ONLY TO BE USED FROM CheckMinimumArea ***

********************************************************************************************/

BOOL BfxPixelOp32::FloodSearch(LONG x, LONG y, LONG MinimumArea)
{
	if (IsCached(x,y)  // We've been here before
		|| GetCacheMarks()>=MinimumArea) // We've got enough
		return TRUE;
	if (IsInRegion(x,y))
	{
		if (!(	FloodSearch(x-1, y-1, MinimumArea) &&
				FloodSearch(x  , y-1, MinimumArea) &&
				FloodSearch(x+1, y-1, MinimumArea) &&
				FloodSearch(x-1, y  , MinimumArea) &&
				FloodSearch(x+1, y  , MinimumArea) &&
				FloodSearch(x-1, y+1, MinimumArea) &&
				FloodSearch(x  , y+1, MinimumArea) &&
				FloodSearch(x+1, y+1, MinimumArea) &&
				TRUE)) return TRUE;
	}
	return TRUE;
}


/********************************************************************************************

>	BfxPixelOpPseudo::BfxPixelOpPseudo()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for pixel op
	Errors:		None yet
	SeeAlso:	-

********************************************************************************************/

BfxPixelOpPseudo::BfxPixelOpPseudo() : BfxPixelOp()
{
	BPP=8;
}

/********************************************************************************************

>	BfxPixelOpPseudo::~BfxPixelOpPseudo()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default destructor for pixel op
	Errors:		Error3 if DeInit hasn't been called.
	SeeAlso:	-

********************************************************************************************/

BfxPixelOpPseudo::~BfxPixelOpPseudo()
{
}

/********************************************************************************************

>	BOOL BfxPixelOpPseudo::Reset()
					
	Author:		Alex
	Created:	14/03/95
	Inputs:		None
	Outputs:	Resets all parameters associated with the cache
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Resets all parameters associated with the cache
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOpPseudo::Reset()
{
	return BfxPixelOp::Reset();
}



/********************************************************************************************

>	BOOL BfxPixelOpPseudo::SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask,
							  BOOL theDefaultValue);

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		pKB = pointer to the bitmap
				theCacheStateMask = bit mask used to show cache information
				theCacheValueMask = bit mask used to show cache value information
				theDefaultValue = BOOL value associated with pixels outside the bitmap
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Class set up to use bitmap passed in
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxPixelOpPseudo::SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask,
								  BOOL theDefaultValue)
{
	Base = NULL;
	ERROR2IF( ((!pKB) || (pKB->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(pKB->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiHeader);

	BPP=pBMI->biBitCount;
	ERROR2IF(BPP>8, FALSE,"Pseudo colour BMP not pseudo colour");
	switch (BPP)
	{
		case 1 : Log2BPP=0; break;
		case 2 : Log2BPP=1; break;
		case 4 : Log2BPP=2; break;
		case 8 : Log2BPP=3; break;
		default:
		ERROR2(FALSE, "Invalid BPP field");
	}

	if (!BfxPixelOp::SetBitmap(pKB, theCacheStateMask,theCacheValueMask, theDefaultValue)) return FALSE;

	BPPMask = (BPP==32)?0xFFFFFFFF:(1<<BPP)-1;
	XShift = 3-Log2BPP;
	XMask = (1<<XShift)-1;
	Size = (pBMI->biSizeImage<<3)>>Log2BPP;

	return TRUE;
}




/********************************************************************************************

>	BOOL BfxPositivePixelOp::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOp::IsInRegion()

The region calculation is done as ( ((B-T)^2 >= Value ) && ((A-T)^2 < Value))
where	A = pProposed - the proposed image (i.e. new thing with grad fill on)
		B = pCurrent - the current built up image
		T = pOriginal - the image to be traced

********************************************************************************************/

BOOL BfxPositivePixelOp::IsPixelReallyInRegion(LONG x, LONG y)
{
	LONG BE;
	LONG AE;
	DWORD Pixel = y*Width+x;

	AE = ( IntegerSquare( ((LONG)(((AluPix32 *) pA)[Pixel].R)) - ((LONG)(((AluPix32 *) pT)[Pixel].R)))
 	 + IntegerSquare( ((LONG)(((AluPix32 *) pA)[Pixel].G)) - ((LONG)(((AluPix32 *) pT)[Pixel].G)))
	 + IntegerSquare( ((LONG)(((AluPix32 *) pA)[Pixel].B)) - ((LONG)(((AluPix32 *) pT)[Pixel].B))) );

	BE = (IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].R)) - ((LONG)(((AluPix32 *) pT)[Pixel].R)))
 	 + IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].G)) - ((LONG)(((AluPix32 *) pT)[Pixel].G)))
	 + IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].B)) - ((LONG)(((AluPix32 *) pT)[Pixel].B))) );

 	return ((AE<Value) && (BE>=Value));
}

/********************************************************************************************

>	BOOL BfxColourThresholdPixelOp::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOp::IsInRegion()

The region calculation is done as ( ((B-T)^2 >= Value ) && ((Colour-T)^2 < Value))
where	Colour = the colour to measure against
		B = pCurrent - the current built up image
		T = pOriginal - the image to be traced

********************************************************************************************/

BOOL BfxColourThresholdPixelOp::IsPixelReallyInRegion(LONG x, LONG y)
{
	LONG BE;
	LONG AE;
	DWORD Pixel = y*Width+x;

	AluPix32 TheColour;
	*(DWORD *)(void *)(&TheColour)=Colour; // Yuck

	AE = ( IntegerSquare( ((LONG)(TheColour.R)) - ((LONG)(((AluPix32 *) pT)[Pixel].R))) 
 		 + IntegerSquare( ((LONG)(TheColour.G)) - ((LONG)(((AluPix32 *) pT)[Pixel].G)))    
 		 + IntegerSquare( ((LONG)(TheColour.B)) - ((LONG)(((AluPix32 *) pT)[Pixel].B))) ); 

	BE = (IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].R)) - ((LONG)(((AluPix32 *) pT)[Pixel].R)))
 	 + IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].G)) - ((LONG)(((AluPix32 *) pT)[Pixel].G)))
	 + IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].B)) - ((LONG)(((AluPix32 *) pT)[Pixel].B))) );

 	return ((AE<Value) && (BE>=Value));
}

/********************************************************************************************

>	BOOL BfxThresholdPixelOp::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOp::IsInRegion()

The region calculation is done as ( (B-T)^2 >= Value )
where	Colour = the colour to measure against
		B = pCurrent - the current built up image
		T = pOriginal - the image to be traced

********************************************************************************************/

BOOL BfxThresholdPixelOp::IsPixelReallyInRegion(LONG x, LONG y)
{
	DWORD Pixel = y*Width+x;

	return ( (IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].R)) - ((LONG)(((AluPix32 *) pT)[Pixel].R)))
 	 + IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].G)) - ((LONG)(((AluPix32 *) pT)[Pixel].G)))
	 + IntegerSquare( ((LONG)(((AluPix32 *) pB)[Pixel].B)) - ((LONG)(((AluPix32 *) pT)[Pixel].B))) ) >= Value );
}


/********************************************************************************************

>	BOOL BfxPositivePixelOpPseudo::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOpPseudo::IsInRegion()

The region calculation is done as ( ((B-T)^2 >= Value ) && ((A-T)^2 < Value))
where	A = pProposed - the proposed image (i.e. new thing with grad fill on)
		B = pCurrent - the current built up image
		T = pOriginal - the image to be traced

********************************************************************************************/

#define __bfx_pseudopixel(i,x,y) BfxPixelOp::ReadPixel((void *)i,x,y)
//((((DWORD *)i)[Pixel]>>shift)&BPPMask)


BOOL BfxPositivePixelOpPseudo::IsPixelReallyInRegion(LONG x, LONG y)
{
//	DWORD Pixel = (x+y*WidthRounded)>>XShift;
//	DWORD shift = x & XMask;
	DWORD tval=__bfx_pseudopixel(pT,x,y);
	return (__bfx_pseudopixel(pB,x,y) != tval) && (__bfx_pseudopixel(pA,x,y) == tval);
}

/********************************************************************************************

>	BOOL BfxColourThresholdPixelOpPseudo::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOpPseudo::IsInRegion()

The region calculation is done as ( ((B-T)^2 >= Value ) && ((Colour-T)^2 < Value))
where	Colour = the colour to measure against
		B = pCurrent - the current built up image
		T = pOriginal - the image to be traced

********************************************************************************************/

BOOL BfxColourThresholdPixelOpPseudo::IsPixelReallyInRegion(LONG x, LONG y)
{
//	DWORD Pixel = (x+y*WidthRounded)>>XShift;
//	DWORD shift = x & XMask;
	DWORD tval=__bfx_pseudopixel(pT,x,y);
	return (__bfx_pseudopixel(pB,x,y) != tval) && (Colour == tval);
}

/********************************************************************************************

>	BOOL BfxThresholdPixelOpPseudo::IsPixelReallyInRegion(LONG x, LONG y)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		x,y = coordinates of pixel
	Outputs:	None
	Returns:	Whether pixel is in the region or not.
	Purpose:	To be overriden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOpPseudo::IsInRegion()

The region calculation is done as ( (B-T)^2 >= Value )
where	Colour = the colour to measure against
		B = pCurrent - the current built up image
		T = pOriginal - the image to be traced

********************************************************************************************/

BOOL BfxThresholdPixelOpPseudo::IsPixelReallyInRegion(LONG x, LONG y)
{
//	DWORD Pixel = (x+y*WidthRounded)>>XShift;
//	DWORD shift = x & XMask;
	return __bfx_pseudopixel(pB,x,y) != __bfx_pseudopixel(pT,x,y);
}

/********************************************************************************************

>	void BfxPixelOpPseudo::TranslateToRGB(DWORD Colour, KernelBitmap * pKB,
												   LONG * R, LONG * G, LONG * B)

					
	Author:		Alex
	Created:	14/03/95
	Inputs:		Colour = the colour
	Outputs:	R, G, B = rd, green, blue components
	Returns:	None
	Purpose:	To get the R G B values corresponding to a memory entry
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxPixelOpPseudo::IsInRegion()

********************************************************************************************/

void BfxPixelOpPseudo::TranslateToRGB(DWORD Colour, KernelBitmap * pKB,
											   LONG * R, LONG * G, LONG * B)
{
	RGBQUAD Col;

	if ((!pKB) || (pKB->ActualBitmap==NULL))
	{
		ERROR3( "BfxALU can't find OIL bitmap");
		return;
	}
	ERROR3IF( (!(pKB->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiHeader);

	if ( Colour >= pBMI->biClrUsed )
	{
		ERROR3("Too large palette entry");
		*R=*G=*B=0;
		return;
	}

	Col = ((RGBQUAD *)(pBMI+1/*pointer arith*/))[Colour];	
	
	*R = Col.rgbRed;
	*G = Col.rgbGreen;
	*B = Col.rgbBlue;
	return;
};

/********************************************************************************************

>	BOOL BfxPixelOpPseudo::CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion)
					
	Author:		Alex
	Created:	17/1/95
	Inputs:		MinimumArea = minimum area 
	Outputs:	FoundRegion set as per whether it's found a region
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Determines whether atleast (MinimumArea) pixels are within the region
	Errors:		None yet
	Scope:		Public
	SeeAlso:	BfxPixelOpPseudo::CheckMinimumArea, TraceControl::FloodSearch

FoundRegion is set according the the result. The search starts at InitialX/InitialY

This call *must* be done immediately (yes, immediately) after the cache is cleared. If not
you lose all your caching. Har har.

********************************************************************************************/

BOOL BfxPixelOpPseudo::CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion)
{
	// This is a bodge currently
	*FoundRegion = FALSE;
	if (IsInRegion(InitialX, InitialY))
	{
		if (MinimumArea>BFX_PSEUDOFLOODSIZE-1) MinimumArea=BFX_PSEUDOFLOODSIZE-1;
		LONG FloodArray[BFX_PSEUDOFLOODSIZE][2];
		LONG FloodTop=0;

		if (!FloodSearch(InitialX, InitialY, MinimumArea, FloodArray, &FloodTop)) return FALSE;
		*FoundRegion = (FloodTop >= MinimumArea);

		for (LONG n=0; n<FloodTop; n++) WriteOriginalPixel(FloodArray[n][0], FloodArray[n][1], Colour);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxPixelOpPseudo::FloodSearch(LONG x, LONG y, LONG MinimumArea,
									  LONG FloodArray[BFX_PSEUDOFLOODSIZE][2], LONG * FloodTop)
					
	Author:		Alex
	Created:	17/1/95
	Inputs:		x = start x
				y = start y
				MinimumArea = minimum area
	Outputs:	None
	Returns:	TRUE on success, FALSE (& error set) on failure
	Purpose:	Recursively flood fills an area until >=MinimumArea pixels are found
	Errors:		None yet
	Scope:		Public
	SeeAlso:	BfxPixelOpPseudo::CheckMinimumArea, TraceControl::CheckMinimumArea

***ONLY TO BE USED FROM CheckMinimumArea ***

********************************************************************************************/

BOOL BfxPixelOpPseudo::FloodSearch(LONG x, LONG y, LONG MinimumArea,
				  LONG FloodArray[BFX_PSEUDOFLOODSIZE][2], LONG * FloodTop)
{
	if (IsInRegion(x,y))
	{
		WriteOriginalPixel(x, y, Colour^1); // make it out of the region
		FloodArray[*FloodTop][0]=x;
		FloodArray[(*FloodTop)++][1]=y; // save coordinate so we can restore value later
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x-1, y-1, MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x  , y-1, MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x+1, y-1, MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x-1, y  , MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x+1, y  , MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x-1, y+1, MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x  , y+1, MinimumArea, FloodArray, FloodTop)) return FALSE;
		if (*FloodTop>=MinimumArea) return TRUE;
		if (!FloodSearch(x+1, y+1, MinimumArea, FloodArray, FloodTop)) return FALSE;
	}
	return TRUE;
}


/********************************************************************************************

>	Octree::Octree()
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor
	Errors:		None yet
	SeeAlso:	-

********************************************************************************************/

Octree::Octree()
{
	MaxLeaves=255;
	CurrentLeaves=0;
	CurrentIndex=0;
	pTopElement=NULL;
	for (LONG d=0; d<=9; d++) for (LONG c=0; c<=8; c++) ListHead[d][c]=NULL;
}

/********************************************************************************************

>	Octree::~Octree()
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default destructor for pixel op
	Errors:		Error3 if DeInit hasn't been called.
	SeeAlso:	-

********************************************************************************************/

Octree::~Octree()
{
	if (pTopElement)
	{
		delete pTopElement; // which deletes its children.
		pTopElement=NULL;
	}
}

/********************************************************************************************

>	BOOL Octree::Init(LONG NumColours)
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		None
	Outputs:	Resets all parameters associated with the cache
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Resets all parameters associated with the cache
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL Octree::Init(LONG NumColours)
{
	if (pTopElement)
	{
		delete pTopElement; // which deletes its children.
		pTopElement=NULL;
	}
	MaxLeaves=NumColours;
	CurrentLeaves=0;
	CurrentIndex=0;

	for (LONG d=0; d<=9; d++) for (LONG c=0; c<=8; c++) ListHead[d][c]=NULL;

	if ((pTopElement=new OctreeElement) == NULL) return FALSE; // error already set
	// and it's already in a fairly well set up state

	ListHead[0][0]=pTopElement;

	return TRUE;
}


/********************************************************************************************

>	OctreeElement * Octree::GetElement(LONG r, LONG g, LONG b)
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		r, g, b values to octree search for
	Outputs:	None
	Returns:	pointer to relevant octree element
	Purpose:	Resets all parameters associated with the cache
	Errors:		None yet
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

OctreeElement * Octree::GetElement(LONG r, LONG g, LONG b)
{
	ERROR2IF(!pTopElement, NULL, "Octree not initialised");
	OctreeElement * pEl = pTopElement;
	
	// descend the tree till we reach the minimal cube containing the colour
	while(TRUE)
	{
		LONG halfwidth=(1<<(8 - pEl->Depth))>>1;
		LONG R=pEl->R+halfwidth;
		LONG G=pEl->G+halfwidth;
		LONG B=pEl->B+halfwidth;
		LONG child=((r>=R)?1:0)+((g>=G)?2:0)+((b>=B)?4:0);
		if (!pEl->pChildren[child]) return pEl;
		pEl=pEl->pChildren[child];
	}
	
	ERROR3("How did it get here?");
	return NULL;
}

/********************************************************************************************

>	BOOL Octree::Insert(LONG r, LONG g, LONG b)
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		r, g, b values to insert
	Outputs:	None
	Returns:	TRUE on success, FALSE on error
	Purpose:	Resets all parameters associated with the cache
	Errors:		2 & 3
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL Octree::Insert(LONG r, LONG g, LONG b)
{
	//CheckIntegrity();
	
	OctreeElement * pEl = GetElement(r, g, b);
	if (!pEl) return FALSE;

	// Insert any necessary intermediates
	while (pEl->Depth !=8)
	{
		OctreeElement * pNewEl = new OctreeElement;
		if (!pNewEl) return FALSE;
		pNewEl->Depth=pEl->Depth+1;
		LONG halfwidth=(1<<(8 - pEl->Depth))>>1;
		LONG R=pEl->R+halfwidth;
		LONG G=pEl->G+halfwidth;
		LONG B=pEl->B+halfwidth;

		pNewEl->R=(r>=R)?R:(pEl->R);
		pNewEl->G=(g>=G)?G:(pEl->G);
		pNewEl->B=(b>=B)?B:(pEl->B);

		LONG child=((r>=R)?1:0)+((g>=G)?2:0)+((b>=B)?4:0);
		ERROR2IF(pEl->pChildren[child], FALSE, "Blurk! Octree element already has a child");
		pEl->pChildren[child]=pNewEl;
		pNewEl->pParent=pEl;

		Unlink(pEl);
		pEl->NumChildren++;
		Link(pEl);

		pEl=pNewEl;
		Link(pEl);
	
		//CheckIntegrity();
	}

	if (!pEl->Pixels) CurrentLeaves++;
	pEl->Pixels++;

	while (CurrentLeaves>MaxLeaves)
	{
		pEl=NULL;

		// Seatch deepest in the tree first ...
		for (LONG d=8; (d>=0) && !pEl; d--)
		{
			// ... for an element with no children ...
			OctreeElement * plEl=ListHead[d][0];
			while (plEl && !pEl)
			{
				// ... which has a parent ...
				pEl=plEl->pParent;
				// ... whose children have no children themselves
				for (LONG c=0; (c<8) && pEl; c++) if (pEl->pChildren[c] && pEl->pChildren[c]->NumChildren) pEl=NULL;
				plEl=plEl->pListNext;
			}
		}

		if (pEl)
		{
			Unlink(pEl);
			if (!pEl->Pixels) CurrentLeaves++;
			for (LONG c=0; c<8; c++) if (pEl->pChildren[c])
			{
				pEl->Pixels+=pEl->pChildren[c]->Pixels;
				
				Unlink (pEl->pChildren[c]);
				pEl->NumChildren--;
				ERROR3IF(pEl->pChildren[c]->NumChildren, "OK, why have I been given an octree element with children?");
				delete pEl->pChildren[c];
				pEl->pChildren[c]=NULL;
				CurrentLeaves--;
			}
			Link(pEl);
			//CheckIntegrity();
		} else
		{
			ERROR3("Can't find a candidate for combining octree");
			break;
		}
	}
	
	return TRUE;
}

/********************************************************************************************

>	void Octree::CheckIntegrity()

					
	Author:		Alex
	Created:	19/04/95
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Unlinks elt from lists
	Errors:		2 & 3
	Scope:		
	SeeAlso:	-

Checks the octrees integrity

********************************************************************************************/

void Octree::CheckIntegrity()
{
	for (LONG d=0; d<=8; d++) for (LONG c=0; c<=8; c++)
	{
		OctreeElement *pCheck = ListHead[d][c];
		OctreeElement *pChild = NULL;
		while (pCheck)
		{
			if (pCheck->pListPrev) ERROR3IF(pCheck->pListPrev->pListNext!=pCheck, "Bad prev ptr");
			if (pCheck->pListNext) ERROR3IF(pCheck->pListNext->pListPrev!=pCheck, "Bad next ptr");
			ERROR3IF((d!=0) && !pCheck->pParent, "Has no parent on non zero depth");
			ERROR3IF((d==0) && (pCheck!=pTopElement), "Bad top element");
			ERROR3IF(pCheck->Depth!=d,"Bad depth");
			ERROR3IF(pCheck->NumChildren!=c, "Bad Num Children");
			LONG count=0;
			LONG halfwidth=(1<<(8 - pCheck->Depth))>>1;
			for (LONG cc=0; cc<8; cc++) if ((pChild=pCheck->pChildren[cc]/*assign*/)!=NULL)
			{
			 	count++;
				ERROR3IF(pChild->pParent != pCheck, "Bad parent link");
				ERROR3IF(pChild->Depth != d+1, "Bad child depth");
				LONG R=pCheck->R + ((cc&1)?halfwidth:0);
				LONG G=pCheck->G + ((cc&2)?halfwidth:0);				
				LONG B=pCheck->B + ((cc&4)?halfwidth:0);
				ERROR3IF(pChild->R != R, "Bad child R");
				ERROR3IF(pChild->G != G, "Bad child G");
				ERROR3IF(pChild->B != B, "Bad child B");
				OctreeElement *pLChild = pChild;
				while (pLChild->pListPrev) pLChild=pLChild->pListPrev;
				ERROR3IF(pLChild!=ListHead[d+1][pLChild->NumChildren],"Child not in a list");
			}
			ERROR3IF(c!=count, "Bad child count");
			pCheck=pCheck->pListNext;
		}
	}
	return;
}	

/********************************************************************************************

>	void Octree::Unlink(OctreeElement * pEl)

					
	Author:		Alex
	Created:	19/04/95
	Inputs:		pEl = pointer to element to unlink
	Outputs:	None
	Returns:	None
	Purpose:	Unlinks elt from lists
	Errors:		2 & 3
	Scope:		
	SeeAlso:	-

********************************************************************************************/

void Octree::Unlink(OctreeElement * pEl)
{
	// Now unlink pEl from its previous chain firt by sorting out NextPtr of Prev elt
	if (pEl->pListPrev)
	{
		pEl->pListPrev->pListNext=pEl->pListNext; // possibly NULL
	}
	else // it was the head previously
	{
		ListHead[pEl->Depth][pEl->NumChildren]=pEl->pListNext; // possibly NULL
	}
	// then by sorting out PrevPtr of next elt
	if (pEl->pListNext) // if there is a next ptr
	{
	 	pEl->pListNext->pListPrev=pEl->pListPrev; // possibly NULL
	}
	pEl->pListNext=pEl->pListPrev=NULL;
	return;
}

/********************************************************************************************

>	void Octree::Link(OctreeElement * pEl)

					
	Author:		Alex
	Created:	19/04/95
	Inputs:		pEl = pointer to element to unlink
	Outputs:	None
	Returns:	None
	Purpose:	Links elt into lists
	Errors:		2 & 3
	Scope:		
	SeeAlso:	-

********************************************************************************************/

void Octree::Link(OctreeElement * pEl)
{
	// Now insert pEl on the correct child list
	if ((pEl->pListNext=/*assign*/ListHead[pEl->Depth][pEl->NumChildren])!=NULL) // if the list is non empty
	{
		ListHead[pEl->Depth][pEl->NumChildren]->pListPrev=pEl; // adjust the prev pointer
	}
	pEl->pListPrev=NULL; // our prev ptr set to NULL
	ListHead[pEl->Depth][pEl->NumChildren]=pEl; // put us at the front of the list
}



/********************************************************************************************

>	BOOL Octree::Index(RGBQUAD * Palette, OctreeElement * pEl=NULL)
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		*pEl = element to index (and its children) or NULL for top element
	Outputs:	*Palette = palette filled in as per octree
	Returns:	TRUE on success, FALSE on error
	Purpose:	Resets all parameters associated with the cache
	Errors:		2 & 3
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL Octree::Index(RGBQUAD * Palette, OctreeElement * pEl)
{
	CheckIntegrity();

	if (!pEl)
	{
		pEl=pTopElement;
		CurrentIndex=0;
		if (!pEl) return TRUE; // for we have done
	}

	if (pEl->Pixels)
	{
		if (CurrentIndex>=MaxLeaves)
		{
			ERROR3("CurrentIndex>=MaxLeaves in Octree::Index");
			CurrentIndex=0;
		}
		pEl->Index=CurrentIndex;
		Palette[CurrentIndex].rgbRed=(BYTE)(pEl->R);
		Palette[CurrentIndex].rgbGreen=(BYTE)(pEl->G);
		Palette[CurrentIndex].rgbBlue=(BYTE)(pEl->B);
		CurrentIndex++;
	}	
	else
	{
		pEl->Index=-1;
	}
	for (LONG c=0; c<8; c++) if (pEl->pChildren[c]) if (!Index(Palette, pEl->pChildren[c])) return FALSE;
	return TRUE;
}

/********************************************************************************************

>	DWORD Octree::GetIndex(LONG r, LONG g, LONG b)
					
	Author:		Alex
	Created:	19/04/95
	Inputs:		r, g, b to get
	Outputs:	None
	Returns:	Index for this element
	Purpose:	Resets all parameters associated with the cache
	Errors:		2 & 3
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

LONG Octree::GetIndex(LONG r, LONG g, LONG b)
{
	OctreeElement * pEl=GetElement(r, g, b);
	if (!pEl)
	{
		ERROR3("Help! Can't find an enclosing octree element");
		return 0;
	}
	if (pEl->Index<0)
	{
		ERROR3("You've given us an RGB value you didn't give us on the first pass, quoth the octree");
		return 0;
	}
	return pEl->Index;
}
