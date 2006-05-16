// $Header: /Camelot/winoil/bfxalu.cpp 4     27/09/00 18:18 Andy $
// This file implents the bitmap effect ALU

/*
*/

#include "camtypes.h"
#include "errors.h"
#include "bfxalu.h"
#include "gdrawasm.h"
#include "gdrawcon.h"
#include "bitmap.h"			  
#include "bitmpinf.h"
#include "wbitmap.h"
#include "ccobject.h"
#include "paths.h"
#include "fixmem.h"
#include "accures.h"
#include "extfilts.h"
#include "resource.h" // for IDS_OUT_OF_MEMORY
#include "tracectl.h"
#include "app.h"
#include "bmplist.h"
#include "bfxpixop.h"
#include "bfxrc.h"
#include "progress.h"
#include "richard2.h"

// The asm file defines FASTxxxxxx if there are fast versions of the routeines available
#include "bfxasm.h"

// Set this whilst GDraw persists in expecting monochrome bitmaps to be screwy
//#define GAVIN_MONOCHROME_BITMAPS_BROKEN

// Set this whilst Accusoft persist in making IMG_promote_8 incorrectly size their 2 colour palette
#define ACCUSOFT_HAS_BROKEN_NON_FULL_PALETTES

//#undef FASTMARKBITBYWORD
//#undef FASTMARKWORDBYBIT
//#undef FASTMARKREGIONS
//#undef FASTMARKPOSITIVE
//#undef FASTMARKCOLOURTHRESHOLD
//#undef FASTCLEARBITBYWORD
//#undef FASTSCANBIT
//#undef FASTSCANTHRESHOLD
//#undef FASTREMOVEDITHER

#ifdef STANDALONE
	#undef FASTLINEARABK
	#undef FASTMARKBITBYWORD
	#undef FASTCLEARBITBYWORD
	#undef FASTMARKWORDBYBIT
	#undef FASTSCANBIT
	#undef FASTSCANTHRESHOLD
	#undef FASTSCANTHRESHOLD8
	#undef FASTMARKREGIONS
	#undef FASTMARKLARGESTAREA
	#undef FASTBUILDERRORREGIONLIST
	#undef FASTMARKTHRESHOLD
	#undef FASTMARKPOSITIVE
	#undef FASTMARKCOLOURTHRESHOLD
	#undef FASTREMOVEDITHER
#endif

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 4 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(BfxALU, CCObject)

#ifndef EXCLUDE_FROM_RALPH
CC_IMPLEMENT_DYNCREATE(BfxALULUT, CCObject)
CC_IMPLEMENT_DYNCREATE(BfxErrorRegionList, CCObject);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

#define ALUC_PART		0
#define	ALUC_PARTMUL 	1
#define ALUC_PARTREVMUL	2
#define ALUC_ADD		6
#define	ALUC_SUB		7

// Here's a macro to fix GDraw monochrome bitmaps
#ifndef GAVIN_MONOCHROME_BITMAPS_BROKEN
#define FixMono(i,s) { /* empty macro*/ }
#else
#define FixMono(i,s) { BYTE __lut[256]; for(ULONG __l=0;__l<256;__l++) __lut[__l]= (BYTE)(\
((__l & 0x80)>>7) + ((__l & 0x40)>>5) + ((__l & 0x20)>>3) + ((__l & 0x10)>>1) + \
((__l & 0x08)<<1) + ((__l & 0x04)<<3) + ((__l & 0x02)<<5) + ((__l & 0x01)<<7) ); \
for (ULONG __b=0; __b<((ULONG)s);__b++) ((BYTE *)(void *)i)[__b]=__lut[((BYTE *)(void *)i)[__b]];}
#endif

// Accusoft have a bug in their bitmap resize with interpolation which puts a grey line
// down the left hand side of the bitmap. This enables a complete bodge to try and fix it
#define BODGE_ACCUSOFT_RESIZE

/********************************************************************************************

>	BOOL BfxALULUT::LinearABK(double PropA, double PropB, double Offset)
					
	Author:		Alex
	Created:	02/11/94
	Inputs:		None
	Outputs:	Initialises LUT
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Initialises LUT as below
	Errors:		None at present
	SeeAlso:	-

This call initialises the LUT so
	out = A*PropA + B*PropB + Offset
Note everything in the above formula works 0..1 whereas the LUT works 0..255

********************************************************************************************/

#define LUTBYTE(x,y) Data[x+(y<<8)]

BOOL BfxALULUT::LinearABK(double PropA, double PropB, double Offset)
{
	LONG pa=(LONG)(PropA*(1<<15));
	LONG pb=(LONG)(PropB*(1<<15));
	LONG po=(LONG)(Offset*((1<<15)*255)+/* for rounding */(1<<14));

	// Please note how we're *NOT* using doubles in the loop as that's
	// sooooo sloooooow on 486SX. We use 17.15 arithmetic throughout

#ifndef FASTLINEARABK
	LONG a;
	LONG b;
	LONG r;
	for (a=0; a<256; a++) for (b=0; b<256; b++)
	{
		r=(a*pa+b*pb+po)>>15; /* we did the rounding in the offset calc */
		LUTBYTE(a,b)=(r<0)?0:((r>255)?255:(BYTE)(LONG)(r));
	}
#else
	FastLinearABK(Data, pa, pb, po, 256);
#if 0
	LONG a;
	LONG b;
	LONG r;
	for (a=0; a<256; a++) for (b=0; b<256; b++)
	{
		r=(a*pa+b*pb+po)>>15; /* we did the rounding in the offset calc */
		if (LUTBYTE(a,b)!=((r<0)?0:((r>255)?255:(BYTE)(LONG)(r))))
		{
			TRACEUSER("Alex","Byte %x,%x was %x should be %x\n",a,b,
				LUTBYTE(a,b),( (r<0)?0:((r>255)?255:(BYTE)(LONG)(r)) ) );
		}
	}
#endif
#endif


	return(TRUE);
}

/********************************************************************************************

>	BYTE BfxALULUT::BYTE GetLUT(unsigned long A, unsigned long B)
					
	Author:		Alex
	Created:	02/11/94
	Inputs:		A & B LUT address
	Outputs:	None
	Returns:	Byte in LUT
	Purpose:	Gets a value out a LUT
	Errors:		None at present
	SeeAlso:	-

The addresses must be 0..255

********************************************************************************************/

BYTE BfxALULUT::GetLUT(unsigned long A, unsigned long B)
{
	return LUTBYTE(A,B);
}

#endif // EXCLUDE_FROM_RALPH

/********************************************************************************************

>	BfxALU::BfxALU()
					
	Author:		Alex
	Created:	12/10/94
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for ALU
	Errors:		None yet
	SeeAlso:	-

This constructs the ALU - most of the work is done in the Init function

********************************************************************************************/

BfxALU::BfxALU()
{
#ifndef EXCLUDE_FROM_RALPH
	GC = NULL;
	ResetAccusoftDibsToRuns = FALSE;
#endif
}

/********************************************************************************************

>	BfxALU::~BfxALU()
					
	Author:		Alex
	Created:	12/10/94
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for ALU
	Errors:		Error3 if DeInit hasn't been called.
	SeeAlso:	-

This destructs the ALU - most of the work should be done in the Init function

********************************************************************************************/

BfxALU::~BfxALU()
{
#ifndef EXCLUDE_FROM_RALPH
	// has to be a level 3 check - we can't return an error code
	ERROR3IF( (GC!=NULL), "Someone hasn't called the BfxALU deinit function!")
#endif
}

#ifndef EXCLUDE_FROM_RALPH

/********************************************************************************************

>	BOOL BfxALU::Init()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	Initialises subsystem
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Initialises the bitmap effect subsystem
	Errors:		Level 2 if out of memory (this isn't a resource problem as we can't start
				camelot if this fails...)
	SeeAlso:	-

This call's primary purposes is to initialise the BfxALU, claiming a GDraw context etc.

********************************************************************************************/

BOOL BfxALU::Init()
{
	GC = new GDrawAsm;	
	ERROR2IF((GC==NULL),FALSE,"Couldn't get a GDrawContext for the BfxALU");
	if (!GC->Init())
	{
		GC=NULL;
		ERROR2(FALSE,"Couldn't initialise GDrawContext for the BfxALU");
	}
	
	identity.AX=identity.BY=1<<(FX+16);identity.CX=identity.CY=identity.AY=identity.BX=0;
	GC->SetupBitmap(256,256,32,NULL,TRUE);
	GC->SetMatrix(&identity);
	GC->SetAntialiasFlag(FALSE);
	GC->SelectPalette(0);
	GC->SetHalftoneOrigin(0,0);

	return TRUE;
}


void BfxALU::TestGD()
{
	identity.AX=identity.BY=1<<(FX+16);identity.CX=identity.CY=identity.AY=identity.BX=0;
	GC->SetupBitmap(256,256,32,NULL,TRUE);
	GC->SetMatrix(&identity);
	GC->SetAntialiasFlag(FALSE); // should cause a context swap or 2
	GC->SelectPalette(0);
	GC->SetHalftoneOrigin(0,0);
}	

/********************************************************************************************

>	BOOL BfxALU::DeInit()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	DeInitialises subsystem
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Initialises the bitmap effect subsystem
	Errors:		None from this one as yet
	SeeAlso:	-

This call's primary purposes is to deinitialise the BfxALU, releasing a GDraw context etc.

********************************************************************************************/

BOOL BfxALU::DeInit()
{
	if (GC)
	{
		delete GC; 
		GC=NULL;
	}
	return TRUE;
}

/********************************************************************************************

>	KernelBitmap * BfxALU::NewBitmap(KernelBitmap *pBitmap,LONG XAdjust=0,LONG YAdjust=0,
									 LONG NewDepth=0, String_256 * pName =NULL, UINT ResID =0)

					
	Author:		Alex
	Created:	27/10/94
	Inputs:		pBitmap = pointer to the bitmap whose size to copy or NULL
				XAdjust = Amount to alter X size by
				YAdjust = Amount to alter Y size by
				Depth   = bpp  of new bitmap or 0 for as pBitmap
	Outputs:	Claims a new bitmap
	Returns:	Pointer to bitmap, or NULL on failure
	Purpose:	General purpose call to claim a new kernel bitmap.
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

Note the new bitmap is uninitialized. I'll say that again. It's uninitialized. IE it contains
potentially random bytes.

Passing a null bitmap pointer is like passing a pointer to a 32bpp 0x0 bitmap, so
kb=NewBitmap(NULL, 128, 128) creates a 128 pixelx128 pixel bitmap.

kb2=NewBitmap(kb1, -1, -1, 8) creates a bitmap kb2 like kb1 but 8bpp and one pixel smaller.
And so on.

If a null pointer is returned, an error is set.

********************************************************************************************/

KernelBitmap * BfxALU::NewBitmap(KernelBitmap *pBitmap,LONG XAdjust,LONG YAdjust,LONG NewDepth,
	String_256 * pName /*=NULL*/, UINT ResID /*=0*/)
{
	LONG Width;
	LONG Height;
	LONG Depth;
	LONG DPI;
	BOOL IsTemp=TRUE;

	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	if (!pBitmap)
	{
		Width=0;
		Height=0;
		Depth=32;
		DPI=96;	
		IsTemp=FALSE;
	}
	else
	{
		ERROR2IF((pBitmap->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(pBitmap->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

		BitmapInfo BMInfo;
		pBitmap->ActualBitmap->GetInfo(&BMInfo);
		Depth=BMInfo.PixelDepth;
		Width=BMInfo.PixelWidth;
		Height=BMInfo.PixelHeight;   		
		DPI=96; /*BMInfo.DPI; not there grrrr */
	}

	Width+=XAdjust;
	Height+=YAdjust;
	if (NewDepth) Depth=NewDepth;

//	Don't do the following as the constructor doesn't error properly
//	Oh dear, we have to do it because OILBitmap::Create is protected. Grrr
	KernelBitmap * pNewBitmap = new KernelBitmap(Width,Height,Depth,DPI,IsTemp); // Note 'new' sets error for this class
	if (!pNewBitmap) return FALSE;
	if (!pNewBitmap->ActualBitmap) // the alloc failed and we must, unfortunately, assume the error is set right
	{
		delete pNewBitmap; // apparently this will work though I don't like it
		return FALSE;
	}

//	OILBitmap * pNewOILBitmap = OILBitmap::Create( Width, Height, Depth, DPI );
//	if (!pNewOILBitmap) return FALSE; // error already set
//  KernelBitmap * pNewBitmap = new KernelBitmap(pNewOILBitmap); // Note 'new' sets error for this class
//	if (!pNewBitmap)
//	{
//		delete pNewOILBitmap;
//		return FALSE;	
//	}

	ERROR2IF((pNewBitmap->ActualBitmap==NULL) || (((WinBitmap *)(pNewBitmap->ActualBitmap))->BMInfo==NULL) || (((WinBitmap *)(pNewBitmap->ActualBitmap))->BMBytes==NULL),
			  FALSE,"BfxALU can't find OIL bitmap of the new bitmap");
	
	if (pBitmap)
	{
		BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader);
		BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(pNewBitmap->ActualBitmap))->BMInfo->bmiHeader);
		// Copy the DPI accross
		pBBMI->biXPelsPerMeter = pABMI->biXPelsPerMeter;
		pBBMI->biYPelsPerMeter = pABMI->biYPelsPerMeter;
		
		if (Depth<=8) // If the bitmaps are the same depth we make a new palette
		{
			if (pBBMI->biClrUsed==pABMI->biClrUsed && pBBMI->biBitCount==pABMI->biBitCount)
			{
				// copy in palette if appropriate
				memcpy(pBBMI+1 /*ptr arith*/, pABMI+1 /*ptr arith*/, pBBMI->biClrUsed*sizeof(DWORD));
			}
		}
	}

	String_256 BitmapName;
	if (pName)
		BitmapName=*pName;
	else
	{
		if (pBitmap)
			BitmapName=pBitmap->ActualBitmap->GetName();
		else
			BitmapName=pNewBitmap->ActualBitmap->GetName();
	}

	if ((ResID) && (BitmapName.Length()<128))
	{
		String_256 Mask(IDS_BFXALU_MASK_START); // " (*"
		String_256 Mask2(IDS_BFXALU_MASK_END); // ")"
		TCHAR Fuzz = '*';
		String_256 Processed(IDS_BFX_PROCESSED);
		String_256 Res(ResID);
		int pos = BitmapName.Sub(Mask, 0, Fuzz);
		if (pos>=0) if (BitmapName.Sub(Mask2, pos, Fuzz)<0) pos=-1;
		if (pos>=0) BitmapName.Left(&BitmapName, pos);
		BitmapName += String_8(IDS_BFXALU_PROCESSED_START);	// " (";
		if (pos>=0) BitmapName+=Processed;
		BitmapName+=Res;
		BitmapName += String_8(IDS_BFXALU_PROCESSED_END); 	// ")";
	}

	GetApplication()->GetGlobalBitmapList()->MakeNameUnique(&BitmapName);

	pNewBitmap->ActualBitmap->SetName(BitmapName);

	return pNewBitmap;

}				


/********************************************************************************************

>	BOOL BfxALU::SetA(KernelBitmap *pBitmap=NULL);
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		pBitmap = pointer to the bitmap to use as an accumulator
	Outputs:	Sets up GDraw to use the kernel bitmap as an accumulator
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	GDraw needs a bitmap to plot to & from, this is the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This call sets up the accumulator register of the ALU to be the kernel bitmap passed in. All
adds, subtracts etc. are done on this register.

pBitmap may be NULL to stop using this bitmap

********************************************************************************************/

BOOL BfxALU::SetA(KernelBitmap * pBitmap)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	if (!pBitmap)
	{
		GC->SetupBitmap(256,256,32,NULL,TRUE);
		GC->SetMatrix(&identity);
		A=NULL;
		return(TRUE);	
	}
	else
	{
		ERROR2IF((pBitmap->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(pBitmap->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
//		ERROR2IF((((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader.biBitCount !=32), FALSE,
//				 "Bad BfxALU accumulator");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader);
		ERROR2IF( (!GC->SetupBitmap(pBMI->biWidth,pBMI->biHeight,pBMI->biBitCount,
									((WinBitmap *)(pBitmap->ActualBitmap))->BMBytes)),
				   FALSE,"GDraw didn't like BfxALU accumulator");
		GC->SetMatrix(&identity);
		A=pBitmap;
		return(TRUE);
	}
}

/********************************************************************************************

>	BOOL BfxALU::SetB(KernelBitmap *pBitmap=NULL,LONG XOffset=0,LONG YOffset=0,
					  const BYTE * pTable=NULL,DWORD Style=0)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		pBitmap = pointer to the bitmap to use as B reg
				X/YOffset = offsets in pixels
				pTable = a lookup table to use on the RGB channels
				Style = Tiling style
	Outputs:	Sets up GDraw to use the kernel bitmap as B reg
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	GDraw needs a bitmap to plot to & from, this is the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This call sets up the B register of the ALU to be the kernel bitmap passed in.

pBitmap may be null to stop using this bitmap

********************************************************************************************/

BOOL BfxALU::SetB(KernelBitmap * pBitmap,LONG XOffset,LONG YOffset,const BYTE * pTable,DWORD Style)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	if (!pBitmap)
	{
		B=NULL;
		BColour=0;
		return(TRUE);	
	}
	else
	{
		ERROR2IF((pBitmap->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(pBitmap->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader);
		//ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
		BPoints[0].x = XOffset; 						BPoints[0].y = YOffset;
		BPoints[1].x = XOffset+(LONG)(pBMI->biWidth);	BPoints[1].y = YOffset;
		BPoints[2].x = XOffset; 						BPoints[2].y = YOffset+(LONG)(pBMI->biHeight);
		B=pBitmap;
		BpTable=pTable;
		BStyle=Style;
		return(TRUE); 
	}
}

/********************************************************************************************

>	BOOL BfxALU::SetB(DWORD Colour)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		Colour is the colour word we're using
	Outputs:	Sets up GDraw to use the kernel bitmap as B reg
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	GDraw needs a bitmap to plot to & from, this is the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This call sets up the B register of the ALU to be the kernel bitmap passed in.

pBitmap may be null to stop using this bitmap

********************************************************************************************/

BOOL BfxALU::SetB(DWORD Colour)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	B=NULL;
	BColour=Colour;
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::SetT(KernelBitmap *pBitmap=NULL,LONG XOffset=0,LONG YOffset=0,
					  const BYTE * pTable=NULL,DWORD Style=0)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		pBitmap = pointer to the bitmap to use as T reg
				X/YOffset = offsets in pixels
				pTable = a lookup table to use on the RGB channels
				Style = Tiling style
	Outputs:	Sets up GDraw to use the kernel bitmap as T reg
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	GDraw needs a bitmap to plot to & from, this is the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This call sets up the T register of the ALU to be the kernel bitmap passed in.

pBitmap may be null to stop using this bitmap

********************************************************************************************/

BOOL BfxALU::SetT(KernelBitmap * pBitmap,LONG XOffset,LONG YOffset,const BYTE * pTable,DWORD Style)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	if (!pBitmap)
	{
		T=NULL;
		TValue=0;
		return(TRUE);	
	}
	else
	{
		ERROR2IF((pBitmap->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(pBitmap->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader);
// Some operations now allow 32 bit 'T' regs though they aren't used for transparency
//		ERROR2IF((pBMI->biBitCount !=8), FALSE,"Bad BfxALU T reg");
		TPoints[0].x = XOffset; 						TPoints[0].y = YOffset;
		TPoints[1].x = XOffset+(LONG)(pBMI->biWidth);	TPoints[1].y = YOffset;
		TPoints[2].x = XOffset; 						TPoints[2].y = YOffset+(LONG)(pBMI->biHeight);
		T=pBitmap;
		TpTable=pTable;
		TStyle=Style;
		return(TRUE);
	}
}

/********************************************************************************************

>	BOOL BfxALU::SetT(DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		Value is the value we're passing in
	Outputs:	Sets up GDraw to use the kernel bitmap as T reg
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	GDraw needs a bitmap to plot to & from, this is the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This call sets up the T register of the ALU to be the kernel bitmap passed in.

pBitmap may be null to stop using this bitmap

********************************************************************************************/

BOOL BfxALU::SetT(DWORD Value)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	T = NULL;
	TValue = Value;
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::DoRect()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None 
	Outputs:	Accumulator altered
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Calls GDraw to perform the desired ALU operation
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::DoRect()
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)),FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	RECT Rect={/*L*/0,/*T*/pBMI->biHeight,/*R*/pBMI->biWidth,/*B*/0};
	ERROR2IF((!GC->FillRectangle(&Rect)),FALSE,"BfxALU GDraw_FillRectangle failed");

#if 0	
GC->SetColour((COLORREF) 0x0000FF00);	
GC->SetTransparency((COLORREF) 0x0000FF00,(enum TransparencyEnum) 0);	
RECT Rect2={/*L*/0+40,/*T*/pBMI->biHeight-40,/*R*/pBMI->biWidth-40,/*B*/0+40};
ERROR2IF((!GC->FillRectangle(&Rect2)),FALSE,"BfxALU GDraw_FillRectangle failed");
#endif	
	
	
	return(TRUE);
}

/********************************************************************************************

>	BOOL BfxALU::ZeroA(DWORD FillValue = 0)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		FillValue - Value to fill with 
	Outputs:	Accumulator altered
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Zeros the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::ZeroA(DWORD FillValue)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	
	memset(((WinBitmap *)(A->ActualBitmap))->BMBytes, FillValue ,pBMI->biSizeImage);
#if 0
	RECT Rect={/*L*/0,/*T*/pBMI->biHeight,/*R*/pBMI->biWidth,/*B*/0};
	GC->SetColour((COLORREF) 0x00000000);	
	GC->SetTransparency((COLORREF) 0x00000000,(enum TransparencyEnum) 0);	
	ERROR2IF((!GC->FillRectangle(&Rect)),FALSE,"BfxALU GDraw_FillRectangle failed");
#endif	
	return(TRUE);
}

/********************************************************************************************

>	BOOL BfxALU::UseBT(DWORD TransparencyStyle=0)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		TransparencyStyle = the transparency method to use
	Outputs:	Performs an ALU operation using B & T with a given transparency type
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	This routine sets up GDraw for A & B, and calls DoRect()
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::UseBT(DWORD TransparencyStyle)
{
	DWORD Style;
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	
	if (T)
		Style = TransparencyStyle+7;
	else
		Style = (TransparencyStyle+4)+(TValue<<8);

	if ((!B) && (!T)) // Flat B, Flat T
	{
		ERROR2IF((!GC->SetTransparency(BColour | TValue,(enum TransparencyEnum) Style)),FALSE,"Bad BfxALU BT reg");
	}
	
	if ((B) && (!T)) // Bitmap B, Flat T
	{
		ERROR2IF((B->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
		//ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
		ERROR2IF( (!GC->SetBitmapFill(pBMI,(((WinBitmap *)(B->ActualBitmap))->BMBytes),
									BStyle | (Style<<8),
									BPoints,
									0,
									((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiColors,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									NULL
									)),
				   FALSE,"GDraw didn't like BfxALU B reg");

	}
	if ((!B) && (T)) // Ftat B, Bitmap T
	{

		ERROR2IF((!GC->SetTransparency(BColour,(enum TransparencyEnum) Style)),FALSE,"Bad BfxALU BT reg");

		ERROR2IF((T->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(T->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(T->ActualBitmap))->BMInfo->bmiHeader);
		ERROR2IF((pBMI->biBitCount !=8), FALSE,"Bad BfxALU T reg");
		ERROR2IF( (!GC->SetTransparentBitmapFill(pBMI,(((WinBitmap *)(T->ActualBitmap))->BMBytes),
									TStyle | (Style<<8),
									TPoints,
									0
									)),
				   FALSE,"GDraw didn't like BfxALU T reg");
	}

	if ((B) && (T)) // Bitmap B, Bitmap T
	{
		ERROR2IF((B->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
		//ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
		ERROR2IF( (!GC->SetBitmapFill(pBMI,(((WinBitmap *)(B->ActualBitmap))->BMBytes),
									BStyle | (Style<<8),
									BPoints,
									0,
									((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiColors,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									NULL
									)),
				   FALSE,"GDraw didn't like BfxALU B reg");

		ERROR2IF((T->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(T->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		pBMI=&(((WinBitmap *)(T->ActualBitmap))->BMInfo->bmiHeader);
		ERROR2IF((pBMI->biBitCount !=8), FALSE,"Bad BfxALU T reg");
		ERROR2IF( (!GC->SetTransparentBitmapFill(pBMI,(((WinBitmap *)(T->ActualBitmap))->BMBytes),
									TStyle | (Style<<8),
									TPoints,
									0
									)),
				   FALSE,"GDraw didn't like BfxALU T reg");
	}


	return DoRect();
}

/********************************************************************************************

>	BOOL BfxALU::UseBK(DWORD TransparencyStyle,DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		TransparencyStyle = the transparency method to use
				Value = Value of T
	Outputs:	Performs an ALU operation using B & Value with a given transparency type
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	This routine sets up GDraw for A & B, and calls DoRect()
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::UseBK(DWORD TransparencyStyle,DWORD Value)
{
	DWORD Style;
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	
	Style = (TransparencyStyle+4)+(Value<<8);

	if (!B)
	{
		ERROR2IF((!GC->SetTransparency(BColour | TValue,(enum TransparencyEnum) Style)),FALSE,"Bad BfxALU BT reg");
	}
	else
	{
		ERROR2IF((B->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
		//ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
		ERROR2IF( (!GC->SetBitmapFill(pBMI,(((WinBitmap *)(B->ActualBitmap))->BMBytes),
									BStyle | (Style<<8),
									BPoints,
									0,
									((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiColors,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									NULL
									)),
				   FALSE,"GDraw didn't like BfxALU B reg");

	}
	
	return DoRect();
}

/********************************************************************************************

>	BOOL BfxALU::PlotB()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	Performs an ALU operation A<-B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	This routine sets up GDraw for A & B, and calls DoRect()
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PlotB()
{
	DWORD Style=0;
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");

	if (!B)
	{
		ERROR2IF((!GC->SetTransparency(BColour,(enum TransparencyEnum) Style)),FALSE,"Bad BfxALU B reg");
	}
	else
	{
		ERROR2IF((B->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
		//ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
		ERROR2IF( (!GC->SetBitmapFill(pBMI,(((WinBitmap *)(B->ActualBitmap))->BMBytes),
									BStyle | (Style<<8),
									BPoints,
									0,
									((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiColors,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									NULL
									)),
				   FALSE,"GDraw didn't like BfxALU B reg");

	}
	
	return DoRect();
}

/********************************************************************************************

>	BOOL BfxALU::PlotBLUT(BfxALULUT * LUT)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	Performs an ALU operation A<-B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	This routine sets up GDraw for A & B, and calls DoRect()
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PlotBLUT(BfxALULUT * LUT)
{
	DWORD Style=12;
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF((GC==NULL),FALSE,"BfxALU::PlotBLUT() where's my LUT then?");

	if (!B)
	{
		ERROR2IF((!GC->SetTransparency(BColour,(enum TransparencyEnum) Style)),FALSE,"Bad BfxALU B reg");
	}
	else
	{
		ERROR2IF((B->ActualBitmap==NULL),FALSE,"BfxALU can't find OIL bitmap");
		ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
		BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
		//ERROR2IF((pBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
		ERROR2IF( (!GC->SetTransparencyLookupTable((CONST BYTE *) (LUT->Data))),FALSE,"Bad LUT");
		ERROR2IF( (!GC->SetBitmapFill(pBMI,(((WinBitmap *)(B->ActualBitmap))->BMBytes),
									BStyle | (Style<<8),
									BPoints,
									0,
									((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiColors,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									(unsigned char *) BpTable,
									NULL
									)),
				   FALSE,"GDraw didn't like BfxALU B reg");

	}
	
	return DoRect();
}

/********************************************************************************************

>	BOOL BfxALU::PartTAB()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- TA + (1-T)B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PartTAB()
{
	return UseBT(ALUC_PART);	
}

/********************************************************************************************

>	BOOL BfxALU::PartMulTAB()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- TAB + (1-T)B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PartMulTAB()
{
	return UseBT(ALUC_PARTMUL);	
}

/********************************************************************************************

>	BOOL BfxALU::PartRevMulTAB()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- T(1-(1-A)(1-B)) + (1-T)B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PartRevMulTAB()
{
	return UseBT(ALUC_PARTREVMUL);	
}

/********************************************************************************************

>	BOOL BfxALU::AddTAB()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- A + TB
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::AddTAB()
{
	return UseBT(ALUC_ADD);	
}

/********************************************************************************************

>	BOOL BfxALU::SubTAB()
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- A - TB
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::SubTAB()
{
	return UseBT(ALUC_SUB);	
}

/********************************************************************************************

>	BOOL BfxALU::PartKAB(DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- TA + (1-Value)B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PartKAB(DWORD Value)
{
	return UseBK(ALUC_PART, Value);	
}

/********************************************************************************************

>	BOOL BfxALU::PartMulKAB(DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- TAB + (1-Value)B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PartMulKAB(DWORD Value)
{
	return UseBK(ALUC_PARTMUL, Value);	
}

/********************************************************************************************

>	BOOL BfxALU::PartRevMulKAB(DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- Value.(1-(1-A)(1-B)) + (1-Value)B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PartRevMulKAB(DWORD Value)
{
	return UseBK(ALUC_PARTREVMUL, Value);	
}

/********************************************************************************************

>	BOOL BfxALU::AddKAB(DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- A + Value.B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::AddKAB(DWORD Value)
{
	return UseBK(ALUC_ADD, Value);	
}

/********************************************************************************************

>	BOOL BfxALU::SubKAB(DWORD Value)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		None
	Outputs:	A <- A - Value.B
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::SubKAB(DWORD Value)
{
	return UseBK(ALUC_SUB, Value);	
}


/********************************************************************************************

>	BOOL BfxALU::MarkThresholdError(LONG Value, DWORD MarkValue, DWORD ClearValue)
					
	Author:		Alex
	Created:	18/1/95
	Inputs:		Value = the threshold
				MarkValue = the value to mark over the threshold pixels with
				ClearValue = the value to mark other pixels with
	Outputs:	A <- ( (B-T)^2 >= Value ) ? MarkValue : ClearValue
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::MarkInitialArea

This is used by the tracer routines to find an initial area to mark.

********************************************************************************************/

BOOL BfxALU::MarkThresholdError(LONG Value, DWORD MarkValue, DWORD ClearValue)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!T) || (T->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(T->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pTBMI=&(((WinBitmap *)(T->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	ERROR2IF((pBBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
	ERROR2IF((pTBMI->biBitCount !=32), FALSE,"Bad BfxALU T reg");

	ERROR2IF( ((pABMI->biSizeImage != pBBMI->biSizeImage) || (pABMI->biSizeImage != pTBMI->biSizeImage)), FALSE,
			  "Incompatible bitmaps for MarkThresholdError");
	
	AluPix32 * pA = (AluPix32 *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	AluPix32 * pB = (AluPix32 *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
	AluPix32 * pT = (AluPix32 *)(void *)(((WinBitmap *)(T->ActualBitmap))->BMBytes);
	DWORD * pO = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	
	ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

	LONG Size = (pABMI->biSizeImage)>>2;
	
#ifdef FASTMARKTHRESHOLD
	XLONG BError = 0;
	LONG MarkedPixels = 0;

	FastMarkThreshold((DWORD *)(void *)pA, (DWORD *)(void *)pB, (DWORD *)(void *)pT, Size, Value, MarkValue, ClearValue,
					 &BError, &MarkedPixels);

	//if (pBError) *pBError = BError.MakeDouble();
	//if (pMarkedPixels) *pMarkedPixels = MarkedPixels;
#else
	for (long Pixel = 0; Pixel < Size; Pixel ++)
	 	pO[Pixel] =  (( IntegerSquare( ((LONG)(pB[Pixel].R)) - ((LONG)(pT[Pixel].R)))
	 		 		  + IntegerSquare( ((LONG)(pB[Pixel].G)) - ((LONG)(pT[Pixel].G)))
	 		 		  + IntegerSquare( ((LONG)(pB[Pixel].B)) - ((LONG)(pT[Pixel].B)))) >= Value ) ? MarkValue : ClearValue;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::MarkColourThresholdError(LONG Value, DWORD Colour, DWORD MarkValue, DWORD ClearValue,
							  double * pAError = NULL, double * pBError = NULL, LONG * pTotalPixels = NULL)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		Value = the threshould
				MarkValue = the value to mark positively affected pixels with
				ClearValue = the value to mark other pixels with
				Colour = the intended colour to use
	Outputs:	A <- ( ((B-T)^2 >= Value ) && ((Colour-T)^2 < Value)) ? MarkValue : ClearValue
				*pAError = the total error (Colour-T) (or NULL not to fill in)
				*pBError = the total error in B (or NULL not to fill in)
				*TotalPixels = the total number of pixels (or NULL not to fill in)
				*pMarkedPixels = the number of pixels marked (or NULL not to fill in)
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::MarkPositiveArea

This is used by the tracer routines to find positively affected areas to mark.

********************************************************************************************/

BOOL BfxALU::MarkColourThresholdError(LONG Value, DWORD Colour, DWORD MarkValue, DWORD ClearValue,
							  double * pAError, double * pBError, LONG * pTotalPixels, LONG * pMarkedPixels)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!T) || (T->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(T->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pTBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	ERROR2IF((pBBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
	ERROR2IF((pTBMI->biBitCount !=32), FALSE,"Bad BfxALU T reg");

	ERROR2IF( ((pABMI->biSizeImage != pBBMI->biSizeImage) || (pABMI->biSizeImage != pTBMI->biSizeImage)), FALSE,
			  "Incompatible bitmaps for MarkThresholdError");
	
	AluPix32 * pA = (AluPix32 *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	AluPix32 * pB = (AluPix32 *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
	AluPix32 * pT = (AluPix32 *)(void *)(((WinBitmap *)(T->ActualBitmap))->BMBytes);
	DWORD * pO = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);

	ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

	AluPix32 TheColour;
	*(DWORD *)(void *)(&TheColour)=Colour; // Yuck

	LONG Size = (pABMI->biSizeImage)>>2;
	if (pTotalPixels) *pTotalPixels = Size;

#ifdef FASTMARKCOLOURTHRESHOLD
	XLONG AError = 0;
	XLONG BError = 0;
	LONG MarkedPixels = 0;

	FastMarkColourThreshold((DWORD *)(void *)pA, (DWORD *)(void *)pB, (DWORD *)(void *)pT, Size, Value, Colour,
					 MarkValue, ClearValue, &AError, &BError, &MarkedPixels);

	if (pAError) *pAError = AError.MakeDouble();
	if (pBError) *pBError = BError.MakeDouble();
	if (pMarkedPixels) *pMarkedPixels = MarkedPixels;

#else
	double AError = 0.0;
	double BError = 0.0;
	LONG BE;
	LONG AE;
	LONG MarkedPixels=0; 
	
	for (long Pixel = 0; Pixel < Size; Pixel ++)
	{	AE = ( IntegerSquare( ((LONG)(TheColour.R)) - ((LONG)(pT[Pixel].R)))
	 		 + IntegerSquare( ((LONG)(TheColour.G)) - ((LONG)(pT[Pixel].G)))
	 		 + IntegerSquare( ((LONG)(TheColour.B)) - ((LONG)(pT[Pixel].B))) );

		BE = ( IntegerSquare( ((LONG)(pB[Pixel].R)) - ((LONG)(pT[Pixel].R)))
	 		 + IntegerSquare( ((LONG)(pB[Pixel].G)) - ((LONG)(pT[Pixel].G)))
	 		 + IntegerSquare( ((LONG)(pB[Pixel].B)) - ((LONG)(pT[Pixel].B))) );

		AError += AE;
		BError += BE;
	 	
	 	pO[Pixel] = ((AE<Value) && (BE>=Value)) ? (MarkedPixels++,MarkValue) : ClearValue;
	}

	if (pAError) *pAError = AError;
	if (pBError) *pBError = BError;
	if (pMarkedPixels) *pMarkedPixels = MarkedPixels;
#endif

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::MarkPositive(LONG Value, DWORD MarkValue, DWORD ClearValue,
							  double * pAError = NULL, double * pBError = NULL, LONG * pTotalPixels = NULL)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		Value = the threshould
				MarkValue = the value to mark positively affected pixels with
				ClearValue = the value to mark other pixels with
	Outputs:	A <- ( ((B-T)^2 >= Value ) && ((A-T)^2 < Value)) ? MarkValue : ClearValue
				*pAError = the total error in A (or NULL not to fill in)
				*pBError = the total error in B (or NULL not to fill in)
				*TotalPixels = the total number of pixels (or NULL not to fill in)
				*pMarkedPixels = the number of pixels marked (or NULL not to fill in)
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::MarkPositiveArea

This is used by the tracer routines to find positively affected areas to mark.

********************************************************************************************/

BOOL BfxALU::MarkPositive(LONG Value, DWORD MarkValue, DWORD ClearValue,
							  double * pAError, double * pBError, LONG * pTotalPixels, LONG * pMarkedPixels)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!T) || (T->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(T->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pTBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	ERROR2IF((pBBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");
	ERROR2IF((pTBMI->biBitCount !=32), FALSE,"Bad BfxALU T reg");

	ERROR2IF( ((pABMI->biSizeImage != pBBMI->biSizeImage) || (pABMI->biSizeImage != pTBMI->biSizeImage)), FALSE,
			  "Incompatible bitmaps for MarkThresholdError");
	
	AluPix32 * pA = (AluPix32 *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	AluPix32 * pB = (AluPix32 *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
	AluPix32 * pT = (AluPix32 *)(void *)(((WinBitmap *)(T->ActualBitmap))->BMBytes);
	DWORD * pO = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);

	ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

	LONG Size = (pABMI->biSizeImage)>>2;
	if (pTotalPixels) *pTotalPixels = Size;

#ifdef FASTMARKPOSITIVE
	XLONG AError = 0;
	XLONG BError = 0;
	LONG MarkedPixels = 0;

	FastMarkPositive((DWORD *)(void *)pA, (DWORD *)(void *)pB, (DWORD *)(void *)pT, Size, Value, MarkValue, ClearValue,
					 &AError, &BError, &MarkedPixels);

	if (pAError) *pAError = AError.MakeDouble();
	if (pBError) *pBError = BError.MakeDouble();
	if (pMarkedPixels) *pMarkedPixels = MarkedPixels;

#else
	double AError = 0.0;
	double BError = 0.0;
	LONG BE;
	LONG AE;
	LONG MarkedPixels=0; 
	
	for (long Pixel = 0; Pixel < Size; Pixel ++)
	{	AE = ( IntegerSquare( ((LONG)(pA[Pixel].R)) - ((LONG)(pT[Pixel].R)))
	 		 + IntegerSquare( ((LONG)(pA[Pixel].G)) - ((LONG)(pT[Pixel].G)))
	 		 + IntegerSquare( ((LONG)(pA[Pixel].B)) - ((LONG)(pT[Pixel].B))) );

		BE = ( IntegerSquare( ((LONG)(pB[Pixel].R)) - ((LONG)(pT[Pixel].R)))
	 		 + IntegerSquare( ((LONG)(pB[Pixel].G)) - ((LONG)(pT[Pixel].G)))
	 		 + IntegerSquare( ((LONG)(pB[Pixel].B)) - ((LONG)(pT[Pixel].B))) );

		AError += AE;
		BError += BE;
	 	
	 	pO[Pixel] = ((AE<Value) && (BE>=Value)) ? (MarkedPixels++,MarkValue) : ClearValue;
	}

	if (pAError) *pAError = AError;
	if (pBError) *pBError = BError;
	if (pMarkedPixels) *pMarkedPixels = MarkedPixels;
#endif

	return TRUE;
}


/********************************************************************************************

>	BOOL BfxALU::MarkBitByWord(DWORD BitMask, DWORD TheWord)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		TheWord = value to look for to mark
	Outputs:	A |= BitMask if B=TheWord
				  &= ~BitMask otherwise
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	BfxALU::MarkWordByBit, TraceControl::FindInitialArea

********************************************************************************************/

BOOL BfxALU::MarkBitByWord(DWORD BitMask, DWORD TheWord)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	ERROR2IF((pBBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");

	ERROR2IF( (pABMI->biSizeImage != pBBMI->biSizeImage), FALSE, "Incompatible bitmaps for MarkBitByWord");
	
	DWORD * pA = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	DWORD * pB = (DWORD *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);

	ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

	LONG Size = (pABMI->biSizeImage)>>2;

#ifdef FASTMARKBITBYWORD
	FastMarkBitByWord(pB, pA, Size, BitMask, TheWord);
#else
	for (long Pixel = 0; Pixel < Size; Pixel ++) if (pB[Pixel]==TheWord) pA[Pixel]|=BitMask; else pA[Pixel]&=~BitMask;
#endif

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::ClearBitByWord(DWORD BitMask, DWORD TheWord)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		TheWord = value to look for to mark
	Outputs:	A &= ~BitMask if B=TheWord else intact
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	BfxALU::MarkWordByBit, TraceControl::FindInitialArea

********************************************************************************************/

BOOL BfxALU::ClearBitByWord(DWORD BitMask, DWORD TheWord)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	ERROR2IF((pBBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");

	ERROR2IF( (pABMI->biSizeImage != pBBMI->biSizeImage), FALSE, "Incompatible bitmaps for MarkBitByWord");
	
	DWORD * pA = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	DWORD * pB = (DWORD *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);

	ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

	LONG Size = (pABMI->biSizeImage)>>2;

#ifdef FASTCLEARBITBYWORD
	FastClearBitByWord(pB, pA, Size, BitMask, TheWord);
#else
	for (long Pixel = 0; Pixel < Size; Pixel ++) if (pB[Pixel]==TheWord) pA[Pixel]&=~BitMask;
#endif

	return TRUE;
}
		
/********************************************************************************************

>	BOOL BfxALU::MarkWordByBit(DWORD BitMask, DWORD MarkValue, DWORD ClearValue)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		MarkValue & ClearValue = the two workds with which to mark
	Outputs:	A = ((B & BitMask) ? MarkValue:ClearValue);
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	BfxALU::MarkWordByBit, TraceControl::FindInitialArea

********************************************************************************************/

BOOL BfxALU::MarkWordByBit(DWORD BitMask, DWORD MarkValue, DWORD ClearValue)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	ERROR2IF((pBBMI->biBitCount !=32), FALSE,"Bad BfxALU B reg");

	ERROR2IF( (pABMI->biSizeImage != pBBMI->biSizeImage), FALSE, "Incompatible bitmaps for MarkWordByBit");
	
	DWORD * pB = (DWORD *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
	DWORD * pA = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);

	ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

	LONG Size = (pABMI->biSizeImage)>>2;

#ifdef FASTMARKWORDBYBIT
	FastMarkWordByBit(pB, pA, Size, BitMask, MarkValue, ClearValue);
#else	
	for (long Pixel = 0; Pixel < Size; Pixel ++) pA[Pixel] = (pB[Pixel] & BitMask) ? MarkValue : ClearValue;
#endif

	return TRUE;
}

		
/********************************************************************************************

>	BOOL BfxALU::MarkRegions(DWORD MarkValue, DWORD ClearValue,
								 BOOL * FoundRegion, LONG * InitialX, LONG * InitialY, LONG * Area,
								 LONG * pSize, LONG * pHighestChain, LONG * pWidth, DWORD * * ppA)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		MarkValue = the value marked pixel have
				ClearValue = the value cleared pixels have
	Outputs:	A = RegionValue if pixel is in largest region, otherwise a different number
					ClearValue if pixel is not marked
					otherwise a different value from the above two.
				*pAError = the total error in A (or NULL not to fill in)
				*pBError = the total error in B (or NULL not to fill in)
				*TotalPixels = the total number of pixels (or NULL not to fill in)
				*pSize = Size of image if not NULL
				*pHighestChain = chain ID of highest chain if not NULL
				*pWidth=Width if not NULL
				*ppA=pointer to bitmap if not null
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::FindInitialPoint, BfxALU::MarkLargestArea, BfxALU::BuildErrorRegionList

The areas are chain coded.

********************************************************************************************/

#define MLATestPixel(q) \
{ \
	OtherPixel = q; \
	while (pA[OtherPixel] < 0x80000000) OtherPixel = pA[OtherPixel]; /* get top of pixel chain */ \
	/* only count marked pixels (we can do this by eliminating clear pixels as we've processed all the pixels */ \
	/*  this routine considers). Don't count pixels which are attached to the same chain as us already */ \
	if ((pA[OtherPixel] != ClearValue) && (OtherPixel !=TopChain)) \
	{ \
		/* OtherPixel points to the top of the other pixel's pixel chain */ \
		/* Always use lowest numbered chain */ \
		if (TopChain<OtherPixel) {SwapTemp=TopChain;TopChain=OtherPixel;OtherPixel=SwapTemp;} \
		pA[OtherPixel]+=pA[TopChain]-0x80000000; /* Add chain values together */ \
		pA[TopChain]= OtherPixel; /* attach top of current chain to pixel */ \
		TopChain=OtherPixel; /* Move the current chain top. */ \
	} \
}

BOOL BfxALU::MarkRegions(DWORD MarkValue, DWORD ClearValue,
						     BOOL * FoundRegion, LONG * InitialX, LONG * InitialY, LONG * Area,
							 LONG * pSize, LONG * pHighestChain, LONG * pWidth, DWORD * * ppA)

{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR2IF(!FoundRegion,FALSE,"Found Region flag is compulsorary");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");
	
	DWORD * pA = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);

	LONG Size = (pABMI->biSizeImage)>>2;
	LONG Width = pABMI->biWidth;
	LONG x=0;
	LONG y=0;
	LONG HighestChainVal = 0;
	LONG HighestChain = -1;

#ifdef FASTMARKREGIONS
	FastMarkRegions(pA, Size, Width, MarkValue, ClearValue, &HighestChain, &HighestChainVal);
#else
	LONG Pixel;
	LONG TopChain;
	LONG OtherPixel;
	LONG SwapTemp;
			
	for (Pixel = 0; Pixel < Size; Pixel ++)
	{
		
		if (pA[Pixel] != ClearValue)
		{
			pA[Pixel] = 0x80000001; // Mark as a single pixel chain
			TopChain = Pixel; // and this is it's top
			// OK, we've found a new pixel. First work out if it is connected to any other previously
			if (x!=0) MLATestPixel(Pixel-1);
			if (y!=0)
			{
				if (x!=Width-1) MLATestPixel(Pixel-Width+1);
				MLATestPixel(Pixel-Width);
				if (x!=0) MLATestPixel(Pixel-Width-1);
			}
			if (((LONG)(pA[TopChain]-0x80000000))>HighestChainVal)
			{
				HighestChainVal = (LONG)(pA[TopChain]-0x80000000);
				HighestChain = TopChain;
			}
		}
		if ((++x)==Width)
		{
 			x=0;
			y++;
		}
	}
#endif

	if (InitialX) *InitialX = 0;
	if (InitialY) *InitialY = 0;
	if (Area) *Area = HighestChainVal;
	if ( (*FoundRegion =/*assign*/ (HighestChain != -1) ) != FALSE )
	{
		if (InitialX) *InitialX = HighestChain % Width;
		if (InitialY) *InitialY = HighestChain / Width;
	}
	if (pSize) *pSize = Size;
	if (pHighestChain) *pHighestChain = HighestChain;
	if (ppA) *ppA=pA;
	if (pWidth) *pWidth=Width;
	return TRUE;
}


/********************************************************************************************

>	BOOL BfxALU::MarkLargestArea(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue
								 BOOL * FoundRegion, LONG * InitialX, LONG * InitialY, LONG * Area)

					
	Author:		Alex
	Created:	27/10/94
	Inputs:		MarkValue = the value marked pixel have
				ClearValue = the value cleared pixels have
				RegionValue = the value to mark pixels in the largest region witgh
	Outputs:	A = RegionValue if pixel is in largest region, otherwise a different number
					ClearValue if pixel is not marked
					otherwise a different value from the above two.
				*pAError = the total error in A (or NULL not to fill in)
				*pBError = the total error in B (or NULL not to fill in)
				*TotalPixels = the total number of pixels (or NULL not to fill in)
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::FindInitialPoint, BfxALU::MarkRegions, BfxALU::BuildErrorRegionList

This is used by the tracer routines work out the area to mark.

********************************************************************************************/

BOOL BfxALU::MarkLargestArea(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
						     BOOL * FoundRegion, LONG * InitialX, LONG * InitialY, LONG * Area)
{
	LONG Size;
	LONG HighestChain;
	DWORD * pA;
	// Our error checking done in MarkRegions
	if (!MarkRegions(MarkValue, ClearValue, FoundRegion, InitialX, InitialY, Area,
					 &Size, &HighestChain, NULL, &pA)) return FALSE;

	if (!*FoundRegion) return TRUE; //No need for this poxy coding then

#ifdef FASTMARKLARGESTAREA
	FastMarkLargestArea(pA, Size, MarkValue, ClearValue, RegionValue, HighestChain);
#else
	LONG OtherPixel;
	LONG Pixel;
	for (Pixel = (Size-1); Pixel >= 0; Pixel--)
	{
		OtherPixel = Pixel;
		while (pA[OtherPixel] < 0x80000000) OtherPixel = pA[OtherPixel]; // get top of pixel chain
		if (OtherPixel==HighestChain)
		{
			pA[Pixel] = RegionValue;
		}
		else
		{
			if (pA[Pixel]!=ClearValue)
#if 1
				pA[Pixel] =	MarkValue;
#else			
				pA[Pixel] = 0x7F020202 | (_lrotl((OtherPixel ^ 0xabcdef13),(OtherPixel % 31)) % 0xFFFFFF);
#endif
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::MarkGivenArea(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue
								 BOOL * FoundRegion, LONG * InitialX, LONG * InitialY, LONG * Area)

					
	Author:		Alex
	Created:	27/10/94
	Inputs:		MarkValue = the value marked pixel have
				ClearValue = the value cleared pixels have
				RegionValue = the value to mark pixels in the largest region witgh
				*InitialX,Y = A coordinate in the region to be marked
	Outputs:	A = RegionValue if pixel is in largest region, otherwise a different number
					ClearValue if pixel is not marked
					otherwise a different value from the above two.
				*pAError = the total error in A (or NULL not to fill in)
				*pBError = the total error in B (or NULL not to fill in)
				*TotalPixels = the total number of pixels (or NULL not to fill in)
				*InitialX,Y = Initial point in region to be marked
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::FindInitialPoint, BfxALU::MarkRegions, BfxALU::BuildErrorRegionList

This is used by the tracer routines work out the area to mark.

********************************************************************************************/

BOOL BfxALU::MarkGivenArea(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
						     BOOL * FoundRegion, LONG * InitialX, LONG * InitialY, LONG * Area)
{
	LONG Size;
	LONG HighestChain;
	DWORD * pA;
	ERROR2IF((!InitialX)||(!InitialY),FALSE, "Why not pass in an InitialX & InitialY");
	LONG ix = *InitialX;
	LONG iy = *InitialY;
	LONG Width;
	// Our error checking done in MarkRegions
	if (!MarkRegions(MarkValue, ClearValue, FoundRegion, InitialX, InitialY, Area,
					 &Size, &HighestChain, &Width, &pA)) return FALSE;

	HighestChain=ix+Width*iy;
	while (pA[HighestChain] < 0x80000000) HighestChain = pA[HighestChain]; // get top of pixel chain
	if (pA[HighestChain] == ClearValue) HighestChain=-1;

	if (InitialX) *InitialX = 0;
	if (InitialY) *InitialY = 0;
	if (Area) *Area = 0;
	if ( (*FoundRegion =/*assign*/ (HighestChain != -1) ) != FALSE )
	{
		if (InitialX) *InitialX = HighestChain % Width;
		if (InitialY) *InitialY = HighestChain / Width;
		if (Area) *Area = pA[HighestChain] - 0x80000000;
	}
	
	if (!*FoundRegion) return TRUE; //No need for this poxy coding then

#ifdef FASTMARKLARGESTAREA
	FastMarkLargestArea(pA, Size, MarkValue, ClearValue, RegionValue, HighestChain);
#else
	LONG OtherPixel;
	LONG Pixel;
	for (Pixel = (Size-1); Pixel >= 0; Pixel--)
	{
		OtherPixel = Pixel;
		while (pA[OtherPixel] < 0x80000000) OtherPixel = pA[OtherPixel]; // get top of pixel chain
		if (OtherPixel==HighestChain)
		{
			pA[Pixel] = RegionValue;
		}
		else
		{
			if (pA[Pixel]!=ClearValue)
#if 1
				pA[Pixel] =	MarkValue;
#else			
				pA[Pixel] = 0x7F020202 | (_lrotl((OtherPixel ^ 0xabcdef13),(OtherPixel % 31)) % 0xFFFFFF);
#endif
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::BuildErrorRegionList(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
							  BfxErrorRegionList * pERL, LONG MinimumArea)

					
	Author:		Alex
	Created:	27/10/94
	Inputs:		MarkValue = the value marked pixel have
				ClearValue = the value cleared pixels have
				*pERL = error region list to alter
				MinimumArea = min area to consider
	Outputs:	A = RegionValue if pixel is in region, otherwise a different number
					ClearValue if pixel is not marked
				*pERL altered
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to do the above
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::FindInitialPoint, BfxALU::MarkRegions, BfxALU::BuildErrorRegionList

This is used by the tracer routines work out the area to mark.

********************************************************************************************/

BOOL BfxALU::BuildErrorRegionList(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
							  BfxErrorRegionList * pERL, LONG MinimumArea)
{
	LONG Pixel;
	LONG Size;
	LONG Width;
	DWORD * pA;
	BOOL FoundRegion=FALSE;
	
	ERROR2IF(!pERL, FALSE, "OK, Where's the error region list then?");
	
	// Empty the region list
	if (!pERL->Empty()) return FALSE;

	LONG ListSize=pERL->GetSize();

	// Most of our error checking done in MarkRegions
	if (!MarkRegions(MarkValue, ClearValue, &FoundRegion, NULL, NULL, NULL,
					 &Size, NULL, &Width, &pA)) return FALSE;

	LONG Area;
	
	if (FoundRegion)
	{
		Pixel = 0;
		do
		{

#ifdef FASTBUILDERRORREGIONLIST
			FastBuildErrorRegionList(pA, Size, RegionValue, ClearValue, MinimumArea, &Pixel, &Area);
#else
			do
			{
				if ((DWORD) (Area=(LONG)(pA[Pixel])) != ClearValue) pA[Pixel]=RegionValue;
				// We evilly use bit 30 to check for clearvalue
			} while ( (((DWORD)Area & 0xC0000000)!=0x80000000) && (Pixel++<Size));
#endif
	
			if (Pixel<Size)
			{
				if ((DWORD)(Area-0x80000000) >= (DWORD)MinimumArea)
				{
//					TRACEUSER("Alex","Found region at %d,%d Area %d\n",Pixel % Width, Pixel / Width, Area-0x80000000);
					if (pERL->IsSpace()) pERL->Insert(Pixel % Width, Pixel / Width, Area-0x80000000);
				}

			}
		} while (Pixel<Size);

	}
	if (!pERL->IsSpace()) TRACEUSER("Alex", "[[BuildErrorRegionList ran out of space]]\n");

	return (pERL->Sort() && pERL->ResetRead());
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Here's the old routine
#if 0
	if (FoundRegion)
	{
		for (Pixel = (Size-1); Pixel >= 0; Pixel--)
		{
			if (((Area/*assign*/=pA[Pixel])<0xFF000000) && (Area>=0x80000000))
			{
				if ((LONG)(Area-0x80000000) > MinimumArea)
				{
//					TRACEUSER("Alex","Found region at %d,%d Area %d\n",Pixel % Width, Pixel / Width, Area-0x80000000);
					if (pERL->IsSpace()) pERL->Insert(Pixel % Width, Pixel / Width, Area-0x80000000);
				}
			}
			if (((DWORD)Area) != ClearValue) pA[Pixel]=RegionValue;
		}
	}
#endif

/********************************************************************************************

>	BOOL BfxALU::ScanBit(DWORD BitMask, LONG * X, LONG * Y, LONG * pPixel, BOOL * Found, BOOL CheckConnected=FALSE)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		BitMask = the bit mask to search for
				*X = the start X position or NULL
				*Y = the start Y position or NULL
				*pPixel = the start pixel position (takes priority over X & Y) or NULL
				CheckConnected = TRUE to check at least one connected bit is also set
	Outputs:	*X = the end X position or 0 if none found
				*Y = the end Y position or 0 if none found
				*pPixel = the end pixel position or 0 if none found
				*Found = TRUE if found a pixel such that (A & BitMask) is non zero
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to search A for the next pixel s.t. (A & BitMask) !=0
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::FindInitialArea

********************************************************************************************/

BOOL BfxALU::ScanBit(DWORD BitMask, LONG * X, LONG * Y, LONG *pPixel, BOOL * Found, BOOL CheckConnected)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount !=32), FALSE,"Bad BfxALU A reg");

	DWORD * pA = (DWORD *)(void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);

	LONG Width = pABMI->biWidth;
	LONG Size = (pABMI->biSizeImage)>>2;

	LONG Pixel = 0;
	BOOL Connected = TRUE;
	if (pPixel) Pixel=*pPixel; else if (X && Y) Pixel=(*X)+(*Y)*Width;
	LONG tx;
	LONG ty;

	do
	{
#ifdef FASTSCANBIT
		if (Pixel<Size) FastScanBit(pA, Size, BitMask, &Pixel);
#else	
		while (Pixel<Size && !(pA[Pixel] & BitMask)) Pixel++;
#endif
		if (Pixel>=Size)
		{
			if (X) *X=0;
			if (Y) *Y=0;
			if (pPixel) *pPixel=0;
			if (Found) *Found=FALSE;
			return TRUE;
		}
		tx=Pixel % Width;
		ty=Pixel / Width;
		if (pPixel) *pPixel=Pixel;
		if (X) *X=tx;
		if (Y) *Y=ty;
		if (Found) *Found=TRUE;

		Connected = !CheckConnected;
		if ((!Connected) && (tx!=Width-1)) Connected = ((pA[Pixel+1] & BitMask) !=0);
		if ((!Connected) && (Pixel+Width<Size)) Connected = ((pA[Pixel+Width] & BitMask) !=0);
		if ((!Connected) && (tx!=Width-1) && (Pixel+Width+1<Size)) Connected = ((pA[Pixel+Width+1] & BitMask) !=0);
		if ((!Connected) && (tx!=0) && (Pixel+Width-1<Size)) Connected = ((pA[Pixel+Width-1] & BitMask) !=0);
		if (!Connected) Pixel++;

	} while (!Connected);


	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::ScanThreshold(LONG Value, LONG * X, LONG * Y, LONG * pPixel, BOOL * Found, BOOL CheckConnected=FALSE)
					
	Author:		Alex
	Created:	27/10/94
	Inputs:		Value = the threshold to look for
				*X = the start X position or NULL
				*Y = the start Y position or NULL
				*pPixel = the start pixel position (takes priority over X & Y) or NULL
				CheckConnected = TRUE to check at least one connected bit is also set
	Outputs:	*X = the end X position or 0 if none found
				*Y = the end Y position or 0 if none found
				*pPixel = the end pixel position or 0 if none found
				*Found = TRUE if found a pixel such that (A & BitMask) is non zero
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Arithemtic operation to search A for the next pixel s.t. (A & BitMask) !=0
	Errors:		Error 2 & 3 from called routines
	Scope:		Public
	SeeAlso:	TraceControl::FindInitialArea

********************************************************************************************/

BOOL BfxALU::ScanThreshold(LONG Value, LONG * X, LONG * Y, LONG *pPixel, BOOL * Found, BOOL CheckConnected)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!T) || (T->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(T->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pTBMI=&(((WinBitmap *)(T->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pBBMI->biBitCount != pTBMI->biBitCount), FALSE,"Bad BfxALU B reg");
	ERROR2IF( (pTBMI->biSizeImage != pBBMI->biSizeImage), FALSE,
			  "Incompatible bitmaps for ScanThreshold");

	switch (pTBMI->biBitCount)
	{
		case 32:
		{

			AluPix32 * pB = (AluPix32 *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
			AluPix32 * pT = (AluPix32 *)(void *)(((WinBitmap *)(T->ActualBitmap))->BMBytes);
	
			ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

			LONG Width = pBBMI->biWidth;
			LONG Size = (pBBMI->biSizeImage)>>2;
	
			LONG Pixel = 0;
			BOOL Connected = TRUE;
			if (pPixel) Pixel=*pPixel; else if (X && Y) Pixel=(*X)+(*Y)*Width;
			LONG tx;
			LONG ty;
			LONG tp;

#define __scanthresh(p) (( IntegerSquare( ((LONG)(pB[p].R)) - ((LONG)(pT[p].R))) \
		 		 		  + IntegerSquare( ((LONG)(pB[p].G)) - ((LONG)(pT[p].G))) \
		 		 		  + IntegerSquare( ((LONG)(pB[p].B)) - ((LONG)(pT[p].B)))) >= Value )

			do
			{

#ifdef FASTSCANTHRESHOLD
				if (Pixel<Size) FastScanThreshold((DWORD *)(void *)pB, (DWORD *)(void *)pT, Size, Value, &Pixel);
#else	
				while (Pixel<Size && !__scanthresh(Pixel)) Pixel++;
#endif

				if (Pixel>=Size)
				{
					if (X) *X=0;
					if (Y) *Y=0;
					if (pPixel) *pPixel=0;
					if (Found) *Found=FALSE;
					return TRUE;
				}
				tx=Pixel % Width;
				ty=Pixel / Width;
				if (pPixel) *pPixel=Pixel;
				if (X) *X=tx;
				if (Y) *Y=ty;
				if (Found) *Found=TRUE;

				if ((Connected = /*assign*/ !CheckConnected) != FALSE) return TRUE;
				if ((!Connected) && (tx!=Width-1)) {tp=Pixel+1;Connected = __scanthresh(tp);}
				if ((!Connected) && (Pixel+Width<Size)) {tp=Pixel+Width;Connected = __scanthresh(tp);}
				if ((!Connected) && (tx!=Width-1) && (Pixel+Width+1<Size)) {tp=Pixel+Width+1;Connected = __scanthresh(tp);}
				if ((!Connected) && (tx!=0) && (Pixel+Width-1<Size)) {tp=Pixel+Width-1;Connected = __scanthresh(tp);}
				if (!Connected) Pixel++;

			} while (!Connected);
			return TRUE;
		}

		case 8:
		{

			BYTE * pB = (BYTE *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
			BYTE * pT = (BYTE *)(void *)(((WinBitmap *)(T->ActualBitmap))->BMBytes);
	
			ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

			LONG Width = pBBMI->biWidth;
			LONG Size = pBBMI->biSizeImage;
	
			LONG Pixel = 0;
			BOOL Connected = TRUE;
			LONG WidthR = (Width+3) & ~3;
			if (pPixel) Pixel=*pPixel; else if (X && Y) Pixel=(*X)+(*Y)*WidthR;
			LONG tx = Pixel % WidthR;
			LONG ty = Pixel / WidthR;
			LONG tp;

#define __scanthresh8(p) (pB[p]!=pT[p])

			do
			{

#ifdef FASTSCANTHRESHOLD8
				if (Pixel<Size) FastScanThreshold8((DWORD *)(void *)pB, (DWORD *)(void *)pT, Size, Value, &Pixel,
												   Width, WidthR, &tx, &ty);
#else	
				while (Pixel<Size && (tx>=Width || !__scanthresh8(Pixel)))
				{
					Pixel++;
					if ((++tx)>=WidthR)
					{
						ty++;
						tx=0;
					}
				}
#endif

				if (Pixel>=Size)
				{
					if (X) *X=0;
					if (Y) *Y=0;
					if (pPixel) *pPixel=0;
					if (Found) *Found=FALSE;
					return TRUE;
				}
				if (pPixel) *pPixel=Pixel;
				if (X) *X=tx;
				if (Y) *Y=ty;
				if (Found) *Found=TRUE;

				if ((Connected = /*assign*/ !CheckConnected) != FALSE) return TRUE;
				if ((!Connected) && (tx!=Width-1)) {tp=Pixel+1;Connected = __scanthresh8(tp);}
				if ((!Connected) && (Pixel+WidthR<Size)) {tp=Pixel+WidthR;Connected = __scanthresh8(tp);}
				if ((!Connected) && (tx!=Width-1) && (Pixel+WidthR+1<Size)) {tp=Pixel+WidthR+1;Connected = __scanthresh8(tp);}
				if ((!Connected) && (tx!=0) && (Pixel+WidthR-1<Size)) {tp=Pixel+WidthR-1;Connected = __scanthresh8(tp);}
				if (!Connected)
				{
					Pixel++;
					if ((++tx)>=WidthR)
					{
						ty++;
						tx=0;
					}
				}
			} while (!Connected);
			return TRUE;
		}

		case 1:
		{

			BYTE * pB = (BYTE *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
			BYTE * pT = (BYTE *)(void *)(((WinBitmap *)(T->ActualBitmap))->BMBytes);
	
			ERROR3IF((sizeof(DWORD) != sizeof(AluPix32)), "OK who's broken AluPix32 not to be 32 bits");

			LONG Width = pBBMI->biWidth;
			LONG Size = pBBMI->biSizeImage<<3;
	
			LONG Pixel = 0;
			BOOL Connected = TRUE;
			LONG WidthR = (Width+31) & ~31;
			if (pPixel) Pixel=*pPixel; else if (X && Y) Pixel=(*X)+(*Y)*WidthR;
			LONG tx = Pixel % WidthR;
			LONG ty = Pixel / WidthR;
			LONG tp;

#define __scanthresh1(p) (((pB[p>>3]^pT[p>>3]) & (0x80>>(p&7))) !=0)

			do
			{

#ifdef FASTSCANTHRESHOLD1
				if (Pixel<Size) FastScanThreshold1((DWORD *)(void *)pB, (DWORD *)(void *)pT, Size, Value, &Pixel);
#else	
				while (Pixel<Size && (tx>=Width || !__scanthresh1(Pixel)))
				{
					Pixel++;
					if ((++tx)>=WidthR)
					{
						ty++;
						tx=0;
					}
				}
#endif

				if (Pixel>=Size)
				{
					if (X) *X=0;
					if (Y) *Y=0;
					if (pPixel) *pPixel=0;
					if (Found) *Found=FALSE;
					return TRUE;
				}
				if (pPixel) *pPixel=Pixel;
				if (X) *X=tx;
				if (Y) *Y=ty;
				if (Found) *Found=TRUE;

				if ((Connected = /*assign*/ !CheckConnected) != FALSE) return TRUE;
				if ((!Connected) && (tx!=Width-1)) {tp=Pixel+1;Connected = __scanthresh1(tp);}
				if ((!Connected) && (Pixel+WidthR<Size)) {tp=Pixel+WidthR;Connected = __scanthresh1(tp);}
				if ((!Connected) && (tx!=Width-1) && (Pixel+WidthR+1<Size)) {tp=Pixel+WidthR+1;Connected = __scanthresh1(tp);}
				if ((!Connected) && (tx!=0) && (Pixel+WidthR-1<Size)) {tp=Pixel+WidthR-1;Connected = __scanthresh1(tp);}
				if (!Connected)
				{
					Pixel++;
					if ((++tx)>=WidthR)
					{
						ty++;
						tx=0;
					}
				}
			} while (!Connected);
			return TRUE;
		}

		default: break;
	}
	ERROR2(FALSE, "BfxALU::ScanThreshold doesn't understand that BPP");
	return TRUE; // shouldn't get here
}


/********************************************************************************************

>	BOOL BfxALU::GradFillPath(Path * ThePath, DWORD StartColour, DWORD EndColour,
							  LONG StartX, LONG StartY, LONG EndX, LONG EndY, BOOL Antialias=FALSE,
							  BOOL ColoursAreDWORDs = FALSE)
					
	Author:		Alex
	Created:	26/01/95
	Inputs:		ThePath = pointer to path to plot or NULL for whole thing
				StartColour = COLORREF of start col
				EndColour = COLORREF of end colour
				{Start,End}{x,y} = coords of grad fill
				AntiAlias = TRUE for antialiasing
				ColoursAreDWORDS = TRUE to treat the colours as memory values to be replicated
								   rather than disguised COLORREFs
	Outputs:	Accumulator altered
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Calls GDraw to fill a path
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

The path is rendered grad filled into the accumulator

NOTE! The coordinates passed to this routine, both in the parh and as the grad fill end points,
are all 24.8 values (secretly), i.e. StartX=256 plots from pixel 1.

********************************************************************************************/

BOOL BfxALU::GradFillPath(Path * ThePath, DWORD StartColour, DWORD EndColour,
							  LONG StartX, LONG StartY, LONG EndX, LONG EndY, BOOL AntiAlias, BOOL ColoursAreDWORDs)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)),FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	DWORD Style=0;
	GraduationTable Table;
	Table.Length = 0x100;									// set up small one

	if (StartColour != EndColour)
	{
		ERROR2IF(ColoursAreDWORDs, FALSE, "Grad fill DWORD colours not yet supported");

		ERROR2IF((!GC->BuildGraduationTable( (COLORREF) StartColour, (COLORREF) EndColour, FALSE, &Table)), FALSE,
					"GDraw couldn't build grad fill table for BfxALU");

		// convert ArtWorks style attribute into Gavin-style
		POINT A,B,C;

		A.x = StartX;	A.y = StartY;
		C.x = EndX;		C.y = EndY;
		B.x = A.x + (C.y-A.y);							// make 3 points into a rotated square
		B.y = A.y - (C.x-A.x);

		ERROR2IF((!GC->SetGraduation( Style, &Table, &A, &B, &C )), FALSE, "GDraw couldn't set BfxALU graduation");
	}
	else
	{
		if (ColoursAreDWORDs)
		{
			DWORD DupColour=StartColour;
			switch(pBMI->biBitCount)
			{
				case 32: DupColour=StartColour; break;
				case 16: DupColour=(StartColour & 0xFFFF); DupColour|=DupColour<<16; break;
				case 8:  DupColour=(StartColour & 0xFF); DupColour|=DupColour<<8; DupColour|=DupColour<<16; break;
				case 4:  DupColour=(StartColour & 0xF); DupColour|=DupColour<<4; DupColour|=DupColour<<8;
														DupColour|=DupColour<<16; break;
				case 2:  DupColour=(StartColour & 0x3); DupColour|=DupColour<<2; DupColour|=DupColour<<4;
														DupColour|=DupColour<<8; DupColour|=DupColour<<16; break;
				case 1:  DupColour=(StartColour & 0x1); DupColour|=DupColour<<1; DupColour|=DupColour<<2;
														DupColour|=DupColour<<4; DupColour|=DupColour<<8;
														DupColour|=DupColour<<16; break;
				default:
				{
					ERROR2(FALSE, "Corrupt biBitCount in BfxALU::GradFillPath");
					break;
				}
			}

			ERROR2IF((!GC->SetWordColour(DupColour )), FALSE, "GDraw couldn't set BfxALU word colour");
		}
		else
		{
			ERROR2IF((!GC->SetColour( (COLORREF) StartColour )), FALSE, "GDraw couldn't set BfxALU colour");
		}
	}

	GMATRIX pathmatrix;
	pathmatrix.AX=pathmatrix.BY=1<<(FX+8);pathmatrix.CX=pathmatrix.CY=pathmatrix.AY=pathmatrix.BX=0;
	ERROR2IF((!GC->SetMatrix(&pathmatrix)), FALSE, "GDraw couldn't set path matrix");
	GC->SetAntialiasFlag(AntiAlias); // Grrr void
	GC->SetFlatness(AntiAlias?32:128); // 128 is half a pixel. This is a void fn;  grrr...

	BOOL Success;
	if (ThePath)
	{
		Success = GC->FillPath( (POINT*)ThePath->GetCoordArray(), ThePath->GetVerbArray(), ThePath->GetNumCoords(), 0);
	}
	else
	{
		RECT Rect={/*L*/0,/*T*/(pBMI->biHeight)<<8,/*R*/(pBMI->biWidth)<<8,/*B*/0};
		Success = GC->FillRectangle(&Rect);
	}

	GC->SetAntialiasFlag(FALSE); // Grrr void
	ERROR2IF(!(GC->SetMatrix(&identity)), FALSE, "GDraw couldn't restore BfxALU Matrix");
	ERROR2IF((!Success), FALSE, "GDraw couldn't fill BfxALU path");
	
	return(TRUE);
}

/********************************************************************************************

>	BOOL BfxALU::GetStatistics(Path * ThePath, KernelStatistics * pStats)
					
	Author:		Alex
	Created:	26/01/95
	Inputs:		ThePath = the path to do
	Outputs:	pStats filled in
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Calls GDraw to statistically analyse the contents of a path
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

NOTE! The coordinates passed to this routine, both in the parh and as the grad fill end points,
are all 24.8 values (secretly), i.e. StartX=256 plots from pixel 1.

********************************************************************************************/

BOOL BfxALU::GetStatistics(Path * ThePath, KernelStatistics * pStats)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)),FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	GMATRIX pathmatrix;
	pathmatrix.AX=pathmatrix.BY=1<<(FX+8);pathmatrix.CX=pathmatrix.CY=pathmatrix.AY=pathmatrix.BX=0;
	ERROR2IF((!GC->SetMatrix(&pathmatrix)), FALSE, "GDraw couldn't set path matrix");
	GC->SetFlatness(128); // 128 is half a pixel. This is a void fn;  grrr...
	STATISTICS Stats;
	BOOL Success = GC->GetStatistics( (POINT*)ThePath->GetCoordArray(), ThePath->GetVerbArray(), ThePath->GetNumCoords(),
									  0, &Stats);
	ERROR2IF((!GC->SetMatrix(&identity)), FALSE, "GDraw couldn't restore BfxALU Matrix");
	ERROR2IF((!Success), FALSE, "GDraw couldn't fill BfxALU path");

	pStats->N = Stats.N;
	double N = (double) Stats.N;
	if (N==0) N=1;
	pStats->R = ((Stats.R ).MakeDouble())/(N*255.0);
	pStats->R2= ((Stats.R2).MakeDouble())/(N*255.0*255.0);
	pStats->RX= ((Stats.RX).MakeDouble())/(N*255.0);
	pStats->RY= ((Stats.RY).MakeDouble())/(N*255.0);
	pStats->G = ((Stats.G ).MakeDouble())/(N*255.0);
	pStats->G2= ((Stats.G2).MakeDouble())/(N*255.0*255.0);
	pStats->GX= ((Stats.GX).MakeDouble())/(N*255.0);
	pStats->GY= ((Stats.GY).MakeDouble())/(N*255.0);
	pStats->B = ((Stats.B ).MakeDouble())/(N*255.0);
	pStats->B2= ((Stats.B2).MakeDouble())/(N*255.0*255.0);
	pStats->BX= ((Stats.BX).MakeDouble())/(N*255.0);
	pStats->BY= ((Stats.BY).MakeDouble())/(N*255.0);
	pStats->X = ((Stats.X ).MakeDouble())/(N*255.0);
	pStats->Y = ((Stats.Y ).MakeDouble())/(N*255.0);
	pStats->X2= ((Stats.X2).MakeDouble())/(N*255.0);
	pStats->Y2= ((Stats.Y2).MakeDouble())/(N*255.0);
	pStats->XY= ((Stats.XY).MakeDouble())/(N*255.0);
//	pStats->Pixel = (DWORD)(Stats.C);

	LONG Width = pBMI->biWidth;
		
	pStats->LowX = -1;
	pStats->LowY = -1;
	pStats->Pixel = (DWORD)(-1);

	if (Stats.C>0)
	{
		ULONG Offset = ((ULONG)(Stats.C))-((ULONG)((((WinBitmap *)(A->ActualBitmap))->BMBytes)));
		if (Offset<(ULONG)pBMI->biSizeImage)
		{
			pStats->LowX = (Offset>>2) % Width;
			pStats->LowY = (Offset>>2) / Width;
			DWORD pval = *((DWORD *)(Stats.C));
			pStats->Pixel = (pval & 0x00FF00) | ((pval >> 16) & 0xFF) | ((pval << 16) & 0xFF0000);
		}
	}

	return(TRUE);
}


/********************************************************************************************

>	BOOL BfxALU::GetSize(KernelBitmap * pBitmap, LONG * pXSize, LONG * pYSize, LONG * pDepth)

					
	Author:		Alex
	Created:	26/01/95
	Inputs:		pBitmap
	Outputs:	pXSize (if non NULL) contains X size,
				pYSize (if non NULL) contains Y size,
				pDepth (if non NULL) contains Depth ,
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Convenient place to overcome the inadequacies of the KernelBitmapClass
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

The non NULL pointers are filled in with the appropriate items

********************************************************************************************/

BOOL BfxALU::GetSize(KernelBitmap * pBitmap, LONG * pXSize, LONG * pYSize, LONG * pDepth)
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!pBitmap) || (pBitmap->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(pBitmap->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader);

	if (pXSize) *pXSize=(LONG)(pABMI->biWidth);
	if (pYSize) *pYSize=(LONG)(pABMI->biHeight);
	if (pDepth) *pDepth=(LONG)(pABMI->biBitCount);

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::ByteCopyBA()
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		None
	Outputs:	Accumulator altered
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Copies B into the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Protected
	SeeAlso:	-

The bitmaps must be identical in dimension and depth - this is a bytewise copy.

********************************************************************************************/

BOOL BfxALU::ByteCopyBA()
{
	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!A) || (A->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(A->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(A->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF((pABMI->biBitCount != pBBMI->biBitCount), FALSE,"Bad BfxALU A/B reg");
	ERROR2IF( (pABMI->biSizeImage != pBBMI->biSizeImage), FALSE,
			  "Incompatible bitmaps for ByteCopyBA()");
	
	void * pA = (void *)(((WinBitmap *)(A->ActualBitmap))->BMBytes);
	void * pB = (void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);

	memcpy(pA, pB, pABMI->biSizeImage);

	return TRUE;
}

/********************************************************************************************

>	BfxErrorRegionList::BfxErrorRegionList()
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for ALU
	Errors:		None yet
	SeeAlso:	-

This constructs the error region list - most of the work is done in the Init function

********************************************************************************************/

BfxErrorRegionList::BfxErrorRegionList()
{
	pList = NULL;
	ListLength = 0;
	Used = 0;
	CurrentRead = 0;
	CurrentWrite = 0;
	Sorted = FALSE;
}

/********************************************************************************************

>	BfxErrorRegionList::~BfxErrorRegionList()
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		None
	Outputs:	Constructs object
	Returns:	Nothing
	Purpose:	Default constructor for ALU
	Errors:		Error3 if DeInit hasn't been called.
	SeeAlso:	-

This destructs the region list freeing memory if appropriate

********************************************************************************************/

BfxErrorRegionList::~BfxErrorRegionList()
{
	if (pList) CCFree(pList);
}

/********************************************************************************************

>	BOOL BfxErrorRegionList::Init(ULONG Length)
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		Length = no. of items that should go into list
	Outputs:	None
	Returns:	TRUE if succeeds else FALSE & error set
	Purpose:	Default constructor for ALU
	Errors:		Memory
	SeeAlso:	-

This claims the memory for the region list. You cna init it to 0 to remove the memory.

********************************************************************************************/

BOOL BfxErrorRegionList::Init(ULONG Length)
{
	if (pList) CCFree(pList);
	pList = NULL;
	ListLength = 0;
	Used = 0;
	CurrentRead = 0;
	CurrentWrite = 0;
	Sorted = FALSE;

	if (!Length) return TRUE;

	if ((pList = /*assign*/ (BfxErrorRegionListItem *)CCMalloc(sizeof(BfxErrorRegionListItem) * (Length+1))) == NULL)
		return FALSE;

	ListLength = Length;
	
	return (Empty() && ResetRead());
}

/********************************************************************************************

>	BOOL BfxErrorRegionList::Empty()
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeds else FALSE & error set
	Purpose:	Empties the list
	Errors:		Memory
	SeeAlso:	-

This empties the list

********************************************************************************************/

BOOL BfxErrorRegionList::Empty()
{
	ERROR2IF (!pList, FALSE, "No BfxErrorRegionList");
	CurrentRead = 0;
	CurrentWrite = 0;
	Sorted = FALSE;

	pList[CurrentWrite].X=pList[CurrentWrite].Y=pList[CurrentWrite].Area = 0;

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxErrorRegionList::ResetRead()
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeds else FALSE & error set
	Purpose:	Resets the read pointer
	Errors:		Memory
	SeeAlso:	-

********************************************************************************************/

BOOL BfxErrorRegionList::ResetRead()
{
	CurrentRead = 0;
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxErrorRegionList::Insert(LONG XCoord, LONG YCoord, LONG Area)
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		XCoord, YCoord & Area = the number of the 
	Outputs:	None
	Returns:	TRUE if succeeds else FALSE & error set
	Purpose:	Inserts an item into the list
	Errors:		Memory
	SeeAlso:	-

This empties the list

********************************************************************************************/

BOOL BfxErrorRegionList::Insert(LONG XCoord, LONG YCoord, LONG Area)
{
	ERROR2IF (!pList, FALSE, "No BfxErrorRegionList");
	ERROR2IF (!IsSpace(), FALSE, "It's full! Why not use IsSpace() first like a good programmer?");
	ERROR2IF (!Area, FALSE, "BfxErrorRegionLists cannot hold zero area regions coz they aren't really regions, are they?");

	pList[CurrentWrite].X=XCoord;
	pList[CurrentWrite].Y=YCoord;
	pList[CurrentWrite++].Area=Area;
	pList[CurrentWrite].Area=0;
	pList[CurrentWrite].X=0;
	pList[CurrentWrite].Y=0;

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxErrorRegionList::Sort()
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeds else FALSE & error set
	Purpose:	Sorts the list
	Errors:		Memory
	SeeAlso:	-

SSSSLLLLOOOOOWWWW sort. But never mind.

********************************************************************************************/


BOOL BfxErrorRegionList::Sort()
{
	ERROR2IF (!pList, FALSE, "No BfxErrorRegionList");
	Sorted = FALSE;

	// Handle special cases
	if ((pList[0].Area == 0) || (pList[1].Area ==0))
	{
		Sorted = TRUE;
		return TRUE;
	}
	
	while (!Sorted)
	{
		LONG Current = 0;
		Sorted = TRUE;
		while (pList[++Current].Area !=0) if (pList[Current].Area > pList[Current-1].Area)
		{	
			Sorted=FALSE;
			Swap(&pList[Current], &pList[Current-1]);
		}
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxErrorRegionList::GetCurrent(LONG * XCoord, LONG * YCoord, LONG * Area)
					
	Author:		Alex
	Created:	01/02/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if is an item, else FALSE
	Purpose:	gets items from list
	Errors:		None
	SeeAlso:	-

This function returns FALSE at the end of a list (NOT to indicate an error condition)

********************************************************************************************/

/* INLINE */

#ifndef WEBSTER

/********************************************************************************************

>	BOOL BfxALU::MakeAccusoftHandle(int * pHandle, BOOL DoBodge = FALSE)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		DoBodge - tries to get round a bug in the Accusoft linear interpolation resize code
	Outputs:	*pHandle = Accusoft handle
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new accusoft bitmap handle is created for the B reg
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This actually copies the bitmap. Note we'll have to fudge 32bit bmps somehow

********************************************************************************************/

BOOL BfxALU::MakeAccusoftHandle(int * pHandle, BOOL DoBodge)
{
	ERROR1IF((!(AccusoftFilters::pfnIMG_create_handle_keep)), FALSE, IDN_FILTERNOTPRESENT);
	ERROR2IF((!pHandle ),FALSE, "BfxALU handle parameter must be non-null");

	ERROR2IF((GC==NULL),FALSE,"BfxALU::Init not called / failed");
	ERROR2IF( ((!B) || (B->ActualBitmap==NULL)) ,FALSE,"BfxALU can't find OIL bitmap");
	ERROR3IF( (!(B->ActualBitmap->IsKindOf(CC_RUNTIME_CLASS(WinBitmap)) )),"BfxALU Oil layer inconsistency");

	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);

	DWORD PaletteUsed = pBBMI->biClrUsed;
	
	ERROR2IF ((pBBMI->biSizeImage == 0) || (pBBMI->biCompression != BI_RGB) || (pBBMI->biHeight<0), FALSE, 
				"A naughty DIB format has been used internally");
	
	// Handle wierd palette stuff
	if (pBBMI->biBitCount > 8)
	{
		PaletteUsed = 0;
	}
	else
	{
		// 0 is MS for maximum
//Mark Howitt, 31/10/97. Set the PaletteUsed to the Bpp to ensure Accusoft filters don`t crash when using less
//						 than the maximum number of colours for any given Bpp.
		ERROR3IF(pBBMI->biBitCount==0,"There`s no depth to this picture! Cannot set Palette Used!");
		switch(pBBMI->biBitCount)
		{
			case 1:
				PaletteUsed = 2;	//Why bother? Just in case!
				break;
			case 2:
				PaletteUsed = 4;
				break;
			case 4:
				PaletteUsed = 16;
				break;
			case 8:
				PaletteUsed = 256;
				break;
			default:
				break;
		}
	}

	LONG SizeImage = pBBMI->biSizeImage;
	int Depth = 0;
	if (pBBMI->biBitCount == 32)
		Depth = 24;
	else
		Depth = pBBMI->biBitCount;
#ifdef BODGE_ACCUSOFT_RESIZE
	if (DoBodge)
	{
		// Add some extra bodge pixels on the left hand side
		if (pBBMI->biBitCount == 32)
			SizeImage = DIBUtil::ScanlineSize(3 * pBBMI->biWidth, 24) * pBBMI->biHeight;
		else			
			SizeImage = DIBUtil::ScanlineSize(3 * pBBMI->biWidth, Depth) * pBBMI->biHeight;
	}
	else if (pBBMI->biBitCount == 32)
	{
		ULONG ScanLineSize = DIBUtil::ScanlineSize(pBBMI->biWidth, 24);
		SizeImage = ScanLineSize * pBBMI->biHeight;	
	}
#else
	if (pBBMI->biBitCount == 32)
	{
		ULONG ScanLineSize = DIBUtil::ScanlineSize(pBBMI->biWidth, 24);
		SizeImage = ScanLineSize * pBBMI->biHeight;	
	}
#endif

	HGLOBAL HMem = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + (sizeof(DWORD)*PaletteUsed) + SizeImage);
	ERROR1IF(!HMem, FALSE, IDS_OUT_OF_MEMORY);
	LPBYTE Mem = (LPBYTE) (void *) GlobalLock(HMem);
	ERROR1IF(!Mem, FALSE, IDS_OUT_OF_MEMORY); // wierd - we've only just got it!

	BITMAPINFOHEADER * pCBMI = (BITMAPINFOHEADER *) Mem;
	LPBYTE pPal = Mem + sizeof(BITMAPINFOHEADER);
	LPBYTE pBits = pPal + (sizeof(DWORD)*PaletteUsed);

	pCBMI->biSize = pBBMI->biSize;
	if (DoBodge)
		pCBMI->biWidth = pBBMI->biWidth * 3;
	else
		pCBMI->biWidth = pBBMI->biWidth;
	pCBMI->biHeight = pBBMI->biHeight;
	pCBMI->biPlanes = 1;
	pCBMI->biBitCount = (pBBMI->biBitCount != 32) ? (pBBMI->biBitCount) : 24;
	pCBMI->biCompression = BI_RGB;
	pCBMI->biSizeImage = SizeImage;
	pCBMI->biXPelsPerMeter = pBBMI->biXPelsPerMeter;
	pCBMI->biYPelsPerMeter = pBBMI->biYPelsPerMeter;
	if (PaletteUsed)
	{
		pCBMI->biClrUsed = PaletteUsed;
		pCBMI->biClrImportant = /*pBBMI->biClrImportant;*/PaletteUsed;
	}
	else
	{
		pCBMI->biClrUsed = 0;
		pCBMI->biClrImportant = 0;
	}
	
	// Copy the palette across
	if (PaletteUsed) memcpy(pPal /*dest*/, ((LPBYTE)(pBBMI)) + sizeof(BITMAPINFOHEADER), sizeof(DWORD) * PaletteUsed); 

#ifdef BODGE_ACCUSOFT_RESIZE
	// Copy the main bitmap data across
	if (DoBodge)
	{
		LPBYTE pSourceBits = ((WinBitmap *)(B->ActualBitmap))->BMBytes;
		if (!MAH_BodgeCopyBitmap(pBBMI->biWidth, pBBMI->biHeight, pBBMI->biBitCount, pSourceBits, pBits))
		{
			GlobalUnlock(HMem);
			GlobalFree(HMem);
			return FALSE;
		}
	}
	else
#endif
	{
		if (pBBMI->biBitCount != 32)
		{
			memcpy(pBits /*dest*/, ((WinBitmap *)(B->ActualBitmap))->BMBytes, pBBMI->biSizeImage);
		}
		else
		{
			GC->ConvertBitmap(pBBMI, ((WinBitmap *)(B->ActualBitmap))->BMBytes, pCBMI, pBits, 8);
		}
	}

	if (pBBMI->biBitCount==1) FixMono(pBits, pBBMI->biSizeImage);

	int result = AccusoftFilters::pfnIMG_create_handle_keep(pCBMI);
	if (result < 0)
	{
		GlobalUnlock(HMem);
		GlobalFree(HMem);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	*pHandle = result;

	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::MAH_BodgeCopyBitmap(int Width, int Height, int Depth, LPBYTE pSourceBits, LPBYTE pDestBits)
					
	Author:		Neville Humphrys
	Created:	20/09/96
	Inputs:		Width		- Width of bitmap
				Height		- Height of bitmap
				Depth		- ColourDepth of bitmap
				pSourceBits - source data to use
				pDestBits	- destination for the data
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	To copy the bitmap across to the destination but instead of doing a straight
				copy we copy it to a bitmap which is three times the width and to the middle
				section of this. We then pad the left hand side with the first left pixel.
				We then pad the right hand side with the right hand pixel. 
	Assumes:	Destination bitmap is three times the width of the source but is the same
				in all other aspects.
	SeeAlso:	MakeAccusoftHandle;

********************************************************************************************/

BOOL BfxALU::MAH_BodgeCopyBitmap(int Width, int Height, int Depth, LPBYTE pSourceBits, LPBYTE pDestBits)
{
#ifdef BODGE_ACCUSOFT_RESIZE
	UINT SourceScanline = DIBUtil::ScanlineSize(Width, Depth);
	UINT DestScanline = 1;
	if (Depth == 32)
		DestScanline = DIBUtil::ScanlineSize(Width * 3, 24);
	else
		DestScanline = DIBUtil::ScanlineSize(Width * 3, Depth);

	// We need to do it the hard way and copy it by hand
	// Repeat the first pixel in the extra bodged pixels on the left hand side
	// Copy the source bitmap to the centre section
	// Repeat the last pixel in the extra bodged pixels on the right hand side
	int Bytes = 1;
	switch (Depth)
	{
		case 4:
		{
			// 2 pixels per byte
			// 4bpp so we must put the data into either the high or low nibble.
			// This will be dependent on whether we are on an odd or even pixel.
			// So test the LSBit of the current x, if set we will be odd.
			// Only move onto next byte every other pixel hence current x/2.
			Bytes = Width;
			for (int i = 0; i < Height; i++ )
			{
				// Probably not the most efficient way to copy the data but it is PROVEN!
				BYTE * pDBits = pDestBits;
				// Duplicate the left hand pixel into the first third
				BYTE LeftHandByte = ((*pSourceBits) & 0xF0) >> 4;
				for (int j = 0; j < Width; j++ )
				{
					LPBYTE pData = pDBits + j/2;
					if (j & 1)
						*(pData) = ((*(pData)) & 0xF0) | (LeftHandByte & 0x0F);	// add into low nibble 
					else
						*(pData) = ((*(pData)) & 0x0F) | ((LeftHandByte << 4) & 0xF0);	// add into top nibble
				}				

				BYTE Byte = 0;
				int k = 0;
				for (; j < (2 * Width); j++ )
				{
					// Get the source pixel
					LPBYTE pSourceData = pSourceBits + k/2;
					if (k & 1)
						Byte = ((*(pSourceData)) & 0x0F);	// get low nibble 
					else
						Byte = ((*(pSourceData)) & 0xF0) >> 4;	// get top nibble
					
					// Put it in the correct destination pixel
					LPBYTE pData = pDBits + j/2;
					if (j & 1)
						*(pData) = ((*(pData)) & 0xF0) | (Byte & 0x0F);	// add into low nibble 
					else
						*(pData) = ((*(pData)) & 0x0F) | ((Byte << 4) & 0xF0);	// add into top nibble
					
					k ++;
				}
				
				// Duplicate the right hand pixel for the last third
				LPBYTE pData = pDBits + Width/2;
				BYTE RightHandByte = 0;
				if (Width & 1)
					RightHandByte = ((*(pData)) & 0x0F);	// get low nibble 
				else
					RightHandByte = ((*(pData)) & 0xF0) >> 4;	// get top nibble

				for (; j < (3 * Width); j++ )
				{
					// Probably not the most efficient way but it is PROVEN!
					LPBYTE pData = pDBits + j/2;
					if (j & 1)
					{
						*(pData) = ((*(pData)) & 0xF0) | (RightHandByte & 0x0F);	// add into low nibble 
					}
					else
						*(pData) = ((*(pData)) & 0x0F) | ((RightHandByte << 4) & 0xF0);	// add into top nibble
				}				

				pDestBits += DestScanline;
				pSourceBits += SourceScanline;
			}

			break;
		}
		case 8:
		{
			// 1 byte per pixel
			Bytes = Width;
			for (int i = 0; i < Height; i++ )
			{
				BYTE * pDBits = pDestBits;
				// Duplicate the left hand pixel into the first third
				BYTE LeftHandByte = *pSourceBits;
				memset(pDBits, LeftHandByte, Bytes );
				pDBits += Bytes;
				// Copy the main centre section
				memcpy(pDBits /*dest*/, pSourceBits, Bytes);
				pDBits += Bytes;
				// Duplicate the right hand pixel for the last third
				BYTE RightHandByte = *(pSourceBits + Bytes - 1);
				memset(pDBits, RightHandByte, Bytes );

				pDestBits += DestScanline;
				pSourceBits += SourceScanline;
			}

			break;
		}
		case 32:
		case 24:
		{
			// 3 bytes per pixel
			Bytes = 3 * Width;
			BYTE LeftHandByte_R		= 0;
			BYTE RightHandByte_R	= 0;
			BYTE LeftHandByte_G		= 0;
			BYTE RightHandByte_G	= 0;
			BYTE LeftHandByte_B		= 0;
			BYTE RightHandByte_B	= 0;
			for (int i = 0; i < Height; i++ )
			{
				BYTE * pDBits = pDestBits;
				// Duplicate the left hand pixel into the first third
				LeftHandByte_R = *pSourceBits;
				LeftHandByte_G = *(pSourceBits + 1);
				LeftHandByte_B = *(pSourceBits + 2);
				for (int j = 0; j < Width; j++ )
				{
					*pDBits = LeftHandByte_R;
					pDBits ++;
					*pDBits = LeftHandByte_G;
					pDBits ++;
					*pDBits = LeftHandByte_B;
					pDBits ++;
				}
				
				// Copy the main centre section
				if (Depth != 32)
				{
					memcpy(pDBits /*dest*/, pSourceBits, Bytes);
					pDBits += Bytes;
				}
				else
				{
					LPBYTE pSBits = pSourceBits;
					for (int j = 0; j < Width; j++ )
					{
						*(pDBits) = *(pSBits);
						*(pDBits + 1) = *(pSBits + 1);
						*(pDBits + 2) = *(pSBits + 2);
						pDBits += 3;
						pSBits += 4;
					}
				}
				
				// Duplicate the right hand pixel for the last third
				if (Depth != 32)
				{
					RightHandByte_R = *(pSourceBits + Bytes - 3);
					RightHandByte_G = *(pSourceBits + Bytes - 2);
					RightHandByte_B = *(pSourceBits + Bytes - 1);
				}
				else
				{
					RightHandByte_R = *(pSourceBits + Bytes - 4);
					RightHandByte_G = *(pSourceBits + Bytes - 3);
					RightHandByte_B = *(pSourceBits + Bytes - 2);
				}

				for (j = 0; j < Width; j++ )
				{
					*pDBits = RightHandByte_R;
					pDBits ++;
					*pDBits = RightHandByte_G;
					pDBits ++;
					*pDBits = RightHandByte_B;
					pDBits ++;
				}

				pDestBits += DestScanline;
				pSourceBits += SourceScanline;
			}

			break;
		}
		default:
			ERROR2(FALSE,"Bad colour depth when bodge copying");
	}

#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::MKB_BodgeCopyBitmap(int Width, int Height, int Depth, LPBYTE pSourceBits, LPBYTE pDestBits)
					
	Author:		Neville Humphrys
	Created:	20/09/96
	Inputs:		Width		- Width of bitmap
				Height		- Height of bitmap
				Depth		- ColourDepth of bitmap
				pSourceBits - source data to use
				pDestBits	- destination for the data
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	The reverse of the MAH_BodgeCopyBitmap version. This copies the bitmap across
				to the destination but instead of doing a straight copy we copy the middle
				section from the source bitmap which should be three times the width we want
				to the destination bitmap which should be a third of the width we want.
	Assumes:	Source bitmap is three times the width of the destination but is the same
				in all other aspects.
	SeeAlso:	MakeAccusoftHandle; MAH_BodgeCopyBitmap;

********************************************************************************************/

BOOL BfxALU::MKB_BodgeCopyBitmap(int Width, int Height, int Depth, LPBYTE pSourceBits, LPBYTE pDestBits)
{
#ifdef BODGE_ACCUSOFT_RESIZE

	// Copy back stripping the first bodge pixels from the start of each line
	UINT SourceScanline = DIBUtil::ScanlineSize(Width * 3, Depth);
	UINT DestScanline = DIBUtil::ScanlineSize(Width, Depth);
	// We need to do it the hard way and copy it by hand
	// Remove the repeated extra bodged pixels
	int Bytes = 1;
	switch (Depth)
	{
		/* case 8:
		{
			// 1 pixel per byte
			Bytes = Width;
			// For each scanline copy the middle third, which is the original bitmap
			// back across to the destination
			for (int i = 0; i < Height; i++ )
			{ */
				//memcpy(pDestBits /*dest*/, pSourceBits + Bytes, DestScanline);
			/*	pDestBits += DestScanline;
				pSourceBits += SourceScanline;
			}
			break;
		} */
		case 24:
		{
			// Copy 8 * 3 pixels 1/3 of a pixel at a time
			Bytes = 3 * Width;
			// For each scanline copy the middle third, which is the original bitmap
			// back across to the destination
			for (int i = 0; i < Height; i++ )
			{
				memcpy(pDestBits /*dest*/, pSourceBits + Bytes, DestScanline);
				pDestBits += DestScanline;
				pSourceBits += SourceScanline;
			}
			break;
		}
		default:
			ERROR2(FALSE,"Bad colour depth when bodge copying");
	}

#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::MakeKernelBitmap(int Handle, KernelBitmap * * ppOutput, BOOL Make32Bits=FALSE,
								  String_256 * pName =NULL, UINT ResID =0, BOOL DoBodge = FALSE)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		DoBodge - tries to get round a bug in the Accusoft linear interpolation resize code
	Outputs:	*pHandle = Accusoft handle
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new accusoft bitmap handle is created for the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

This actually copies the bitmap. Note we'll have to fudge 32bit bmps somehow.

*** NOTE: The BfxALU A register is corrupted IFF Make32Bits is set ***

********************************************************************************************/

BOOL BfxALU::MakeKernelBitmap(int Handle, KernelBitmap * * ppOutput, BOOL Make32Bits,
							  String_256 * pName, UINT ResID, BOOL DoBodge)
{
	ERROR3IF((!ppOutput ), "BfxALU output parameter must be non-null");
	int width = 0;
	int	height = 0;
	int	bpp = 0;
	
	HANDLE MHandle = AccusoftFilters::pfnIMG_bitmap_info(Handle, &width, &height, &bpp);
	if (Make32Bits && bpp != 24)
	{
		int result = AccusoftFilters::pfnIMG_delete_bitmap(Handle);
		ERROR2(FALSE, "Can't make a non-24 bit image into a 32 bit one guv");
	}
	
	ERROR2IF(width ==0 || height == 0 || (bpp != 1 && bpp != 4 && bpp != 8 && bpp !=24 && bpp != 32),
			 FALSE,"Bad bitmap height, width or colour depth in MakeKernelBitmap");

#ifdef BODGE_ACCUSOFT_RESIZE
	// if DoBodge set then we must strip our extra pixels from the left hand side and the right hand side
	// of the artificially wider bitmap. In fact three times wider than it should be.
	if (DoBodge)
	{
		width /= 3;
	}
#endif
	
	BITMAPINFOHEADER * pBBMI = (LPBITMAPINFOHEADER) GlobalLock(MHandle);
	KernelBitmap * pKB = NewBitmap(NULL, width, height, Make32Bits ? 32 : bpp, pName, ResID);
	if (!pKB)
	{
		GlobalUnlock(MHandle);
		int result = AccusoftFilters::pfnIMG_delete_bitmap(Handle);
		return FALSE; // Error already set
	}

	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiHeader); // Dest

	if (
		// if our created bitmap  is not equal to the size of the accusoft one ....
		((Make32Bits ? 24 : pABMI->biBitCount) != pBBMI->biBitCount) ||
		((!Make32Bits && !DoBodge) && pABMI->biSizeImage != pBBMI->biSizeImage)
	   )
	{
		GlobalUnlock(MHandle);
		delete pKB;
		int result = AccusoftFilters::pfnIMG_delete_bitmap(Handle);
		ERROR2(FALSE,"Accusoft DLL lied about bpp / size");
	}

	// Fix dodgy Accusoft Palette returns. See how they know what they are doing. I think not.
	if (bpp > 8)
	{
		pBBMI->biClrUsed = 0;
		pBBMI->biClrImportant = 0;
	}
	else
	{
		ULONG UsedColours = 1 << bpp;
#ifdef ACCUSOFT_HAS_BROKEN_NON_FULL_PALETTES
// Accusoft Promote_8 from monochrome returns ClrUsed=2 but has a 256 entry colour table. Arrrggghhh!
pBBMI->biClrUsed=pBBMI->biClrImportant=UsedColours;
#endif
		if (pBBMI->biClrUsed == 0) pBBMI->biClrUsed = UsedColours; // 0 is MS speak for maximum
		if (pBBMI->biClrImportant == 0) pBBMI->biClrImportant = UsedColours; // 0 is MS speak for maximum
		if (UsedColours < pBBMI->biClrUsed)
		{
			TRACEUSER("Alex", "These Accusoft boys really know what they're doing with Palettes - NOT\n");
			pBBMI->biClrUsed = UsedColours;
			pBBMI->biClrImportant = UsedColours;
		}
	}

	// This is fabby Alex code. Just check out those void * rather than LPBYTE and DWORD instead of RGBQUAD
	void * pA = (void *)(((WinBitmap *)(pKB->ActualBitmap))->BMBytes); //Dest
	void * pB = (void *)(((char *)(void *)(pBBMI)) + sizeof (BITMAPINFOHEADER) + (pBBMI->biClrUsed * sizeof (DWORD)) ); //Source

	// Now fix problems with our DIB format palettes (like we don't support optimised palettes for 24 bit stuff)
	if (pBBMI->biClrUsed > pABMI->biClrUsed)
	{
		pBBMI->biClrUsed = pABMI->biClrUsed;
		TRACEUSER("Alex", "We've lost some palette information\n");
	}
	if (pBBMI->biClrUsed < pBBMI->biClrImportant) pBBMI->biClrImportant = pBBMI->biClrUsed;			

	if (!Make32Bits)
	{
#ifdef BODGE_ACCUSOFT_RESIZE
		if (DoBodge)
		{
			// Copy the palette across
			if (pBBMI->biClrUsed > 0)
			{
				LPRGBQUAD pAPAL = &(((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiColors[0]); // Dest
				LPRGBQUAD pBPal = (LPRGBQUAD)(pBBMI + sizeof(BITMAPINFOHEADER)); // Source
				memcpy(pAPAL /*dest*/, pBPal,  (pBBMI->biClrUsed * sizeof (RGBQUAD)) );
			}

			// Make sure we copy any relevent information across to the destination bitmaps header.
			//pABMI->biWidth = width;
			pABMI->biClrUsed = pBBMI->biClrUsed;
			pABMI->biClrImportant = pBBMI->biClrImportant;

			if (!MKB_BodgeCopyBitmap(width, height, bpp,(LPBYTE)pB, (LPBYTE)pA))
			{
				GlobalUnlock(MHandle);
				int result = AccusoftFilters::pfnIMG_delete_bitmap(Handle);
				delete pKB;// We can't delete it - what can we do?
				*ppOutput = NULL;
				return FALSE;
			}
		}
		else
#endif
		{
			// Copy the header and palette across
			memcpy(pABMI /*dest*/, pBBMI, sizeof(BITMAPINFOHEADER) + (pBBMI->biClrUsed * sizeof (DWORD)) );

			// Copy the main bitmap data across
			memcpy(pA /*dest*/, pB, pABMI->biSizeImage);
		}
	}
	else
	{
		POINT tempPoints[3];
		tempPoints[0].x = 0; 						tempPoints[0].y = 0;
		tempPoints[1].x = (LONG)(pBBMI->biWidth);	tempPoints[1].y = 0;
		tempPoints[2].x = 0; 						tempPoints[2].y = (LONG)(pBBMI->biHeight);
		SetA(pKB);
		ZeroA();
		GC->SetBitmapFill(pBBMI,(LPBYTE)pB,
									0,
									tempPoints,
									0,
									NULL, // palette
									NULL,
									NULL,
									NULL,
									NULL
									);
		DoRect();
	}

	pABMI->biXPelsPerMeter = pBBMI->biXPelsPerMeter;
	pABMI->biYPelsPerMeter = pBBMI->biYPelsPerMeter;

	//if (pBBMI->biBitCount==1) FixMono(pA, pBBMI->biSizeImage);

	GlobalUnlock(MHandle);

	if (ResetAccusoftDibsToRuns)
	{
		//AccusoftFilters::pfnIMG_dib_to_runs (Handle);
		ResetAccusoftDibsToRuns = FALSE;
	}

	int result = AccusoftFilters::pfnIMG_delete_bitmap(Handle);
	if (result < 0)
	{
		delete pKB;// We can't delete it - what can we do?
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}

	*ppOutput = pKB;


#ifdef _DEBUG
// Check the memory is *really* gone and Accusoft isn't just mucking us around
/*pBBMI = (LPBITMAPINFOHEADER) GlobalLock(MHandle);
if (pBBMI)
{
	TRACEUSER("Alex","Accusoft DLL failed to release memory\n");
	GlobalUnlock(MHandle);
	GlobalFree(MHandle);
}*/
#endif

	return TRUE;
}


/********************************************************************************************

>	BOOL BfxALU::FlipX(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created which is the flipped version of the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::FlipX(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	int result = AccusoftFilters::pfnIMG_flip_bitmapx(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_FLIPX);
}

/********************************************************************************************

>	BOOL BfxALU::FlipY(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created which is the flipped version of the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/


BOOL BfxALU::FlipY(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	int result = AccusoftFilters::pfnIMG_flip_bitmapy(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_FLIPY);
}


/********************************************************************************************

>	BOOL BfxALU::Rotate90(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created which is the flipped version of the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::Rotate90(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	int result = AccusoftFilters::pfnIMG_rotate_bitmap(AccusoftHandle, 9000);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_ROTATE90);
}


/********************************************************************************************

>	BOOL BfxALU::Rotate180(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created which is the flipped version of the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::Rotate180(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	int result = AccusoftFilters::pfnIMG_rotate_bitmap(AccusoftHandle, 18000);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_ROTATE180);
}


/********************************************************************************************

>	BOOL BfxALU::Rotate270(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	27/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created which is the flipped version of the accumulator
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::Rotate270(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	int result = AccusoftFilters::pfnIMG_rotate_bitmap(AccusoftHandle, 27000);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	
	return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_ROTATE270);
}

/********************************************************************************************

>	BOOL BfxALU::BrightnessContrast(KernelBitmap * * ppOutput, LONG Brightness, LONG Contrast, LONG Colour)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		Brightness & Contast (-127 to +127)
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::BrightnessContrast(KernelBitmap * * ppOutput, LONG Brightness, LONG Contrast, LONG Colour)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
#if 0
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	int result = AccusoftFilters::pfnIMG_set_lut(AccusoftHandle, (int) Brightness, (int) Contrast);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_BRIGHTCONT);
#endif
	
	*ppOutput = NewBitmap(B,0,0,0,NULL, IDS_BFX_BRIGHTCONT);
	if (!ppOutput) return FALSE;
	
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)((*ppOutput)->ActualBitmap))->BMInfo->bmiHeader);

	DWORD PaletteUsed = pBBMI->biClrUsed;
	
	ERROR2IF ((pBBMI->biSizeImage == 0) || (pBBMI->biCompression != BI_RGB) || (pBBMI->biHeight<0), FALSE, 
				"A naughty DIB format has been used internally");
	pABMI->biXPelsPerMeter = pBBMI->biXPelsPerMeter;
	pABMI->biYPelsPerMeter = pBBMI->biYPelsPerMeter;
	ERROR2IF ((pBBMI->biSizeImage != pABMI->biSizeImage), FALSE, "BfxALU brightness & constrast size upset");
	
	DWORD Width;
	DWORD Height;
	DWORD Size;
	LPBYTE Source;
	LPBYTE Dest;
	LPBYTE pSBits = (((WinBitmap *)(B->ActualBitmap))->BMBytes);
	LPBYTE pDBits = (((WinBitmap *)((*ppOutput)->ActualBitmap))->BMBytes);
	
	// We modify the image itself in the case of greyscale images
	BOOL SingleChannel=IsGreyscaleBitmap(B);

	// Handle wierd palette stuff
	if ((SingleChannel) || pBBMI->biBitCount>8)
	{
		if (SingleChannel && PaletteUsed)
		{
			// Copy the palette
			memcpy(((LPBYTE)(pABMI + 1/*ptr arith*/)) /*dest*/, ((LPBYTE)(pBBMI + 1/*ptr arith*/)) /*source*/,
					PaletteUsed * sizeof(DWORD));
		}
		else
		{
			PaletteUsed = 0;
		}
		Width = pBBMI->biWidth;
		Height = pBBMI->biHeight;
		Size = (pBBMI->biBitCount / 8);
		Source = pSBits;
		Dest = pDBits;
	}
	else
	{
		// 0 is MS for maximum
		if (PaletteUsed == 0) PaletteUsed = 1<<pBBMI->biBitCount;
		Width = PaletteUsed;
		Height = 1;
		Size = 4;
		// This points to palette
		Source = ((LPBYTE)(pBBMI + 1/*ptr arith*/));
		Dest = ((LPBYTE)(pABMI + 1/*ptr arith*/));
		// Copy the image
		memcpy(pDBits /*dest*/, pSBits /*source*/, pBBMI->biSizeImage);
	} 

	if (!AdjustBrightnessContrastColour(Source, Dest, Size, Width, Height, Brightness, Contrast, Colour, SingleChannel))
	{
		delete *ppOutput;
		*ppOutput=NULL;
		return FALSE;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL BfxALU::BayerMono(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::BayerMono(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_bayer_mono(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	
	//AccusoftFilters::pfnIMG_runs_to_dib (AccusoftHandle);
	ResetAccusoftDibsToRuns = TRUE;
	
	return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_BAYERMONO);
}

/********************************************************************************************

>	BOOL BfxALU::HalftoneMono(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::HalftoneMono(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_halftone_mono(AccusoftHandle);
	
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	
	// we MUST now call this function BEFORE we create a new bitmap from the data!
	// see the extfilts.h file for why this is necessary!
	//AccusoftFilters::pfnIMG_runs_to_dib (AccusoftHandle);
	ResetAccusoftDibsToRuns = TRUE;

	// DEBUG CODE - PUT THIS LINE BACK IN IF YOU ARE WONDERING WHAT THE HEEL IS GOING ON!	

	//AccusoftFilters::pfnIMG_display_bitmap (AccusoftHandle, GetDC (GetActiveWindow ()), 100, 0,
	//										100,300);

	return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_HALFTONEMONO);
}

/********************************************************************************************

>	BOOL BfxALU::DiffusionMono(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::DiffusionMono(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_diffusion_mono(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	
	//AccusoftFilters::pfnIMG_runs_to_dib (AccusoftHandle);
	ResetAccusoftDibsToRuns = TRUE;
	
	return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_DIFFUSIONMONO);
}



/********************************************************************************************

>	BOOL BfxALU::BayerColour(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::BayerColour(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_bayer_color(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_BAYERCOLOUR);
}

/********************************************************************************************

>	BOOL BfxALU::PopularityColour(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::PopularityColour(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_popularity_color(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_POPULARITYCOLOUR);
}

/********************************************************************************************

>	BOOL BfxALU::DiffusionColour(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::DiffusionColour(KernelBitmap * * ppOutput)
{

//ERROR2(FALSE, "The Accusoft boys have chosen to make this function rampantly crash");

	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_diffusion_color(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_DIFFUSIONCOLOUR);
}

/********************************************************************************************

>	BOOL BfxALU::MakeGreyscale(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	BfxALU::MakeGreyscale32to8, BFXALU::IsGreyscaleBitmap

********************************************************************************************/

BOOL BfxALU::MakeGreyscale(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	int result = AccusoftFilters::pfnIMG_color_gray(AccusoftHandle);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_GREYSCALE);
}
#endif //WEBSTER

/********************************************************************************************
							  	
>	BOOL BfxALU::MakeGreyscale32to8(KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	BfxALU::MakeGreyscale, BFXALU::IsGreyscaleBitmap

This is an optimised routine for converting 32 bit greyscale rendered bitmaps to 8 bpp

********************************************************************************************/

BOOL BfxALU::MakeGreyscale32to8(KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");

	*ppOutput = NewBitmap(B,0,0,8, NULL, IDS_BFX_GREYSCALE);
	if (!ppOutput) return FALSE;
	
	BITMAPINFOHEADER * pBBMI=&(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader);
	BITMAPINFOHEADER * pABMI=&(((WinBitmap *)((*ppOutput)->ActualBitmap))->BMInfo->bmiHeader);

	ERROR2IF ((pBBMI->biSizeImage == 0) || (pBBMI->biCompression != BI_RGB) || (pBBMI->biHeight<0)
			   || (pBBMI->biBitCount !=32) , FALSE,
				"Can't convert this DIB (may be it's not 32 bits)");
	pABMI->biXPelsPerMeter = pBBMI->biXPelsPerMeter;
	pABMI->biYPelsPerMeter = pBBMI->biYPelsPerMeter;
	ERROR2IF ((pBBMI->biWidth != pABMI->biWidth)||(pBBMI->biHeight != pABMI->biHeight)||(pABMI->biClrUsed!=0x100),
		FALSE, "BfxALU MakeGreyscale32to8 size upset");
	
	RGBQUAD * pSBits = (RGBQUAD *)(void *)(((WinBitmap *)(B->ActualBitmap))->BMBytes);
	LPBYTE pDBits = (((WinBitmap *)((*ppOutput)->ActualBitmap))->BMBytes);

	RGBQUAD * Pal = ((RGBQUAD *)(void *)(pABMI + 1/*ptr arith*/));
	for (long x=0;x<0x100;x++)
	{	
		Pal[x].rgbRed=Pal[x].rgbGreen=Pal[x].rgbBlue=(BYTE)x;
		Pal[x].rgbReserved=0;
	}

	DWORD w=0;
	DWORD v=0;
	for (LONG y=0;y<pBBMI->biHeight;y++)
	{
		for (LONG x=0; x<pBBMI->biWidth; x++)
		{
			pDBits[v]=(2+pSBits[w].rgbRed+pSBits[w].rgbGreen+pSBits[w].rgbBlue)/3;
			w++;
			v++;
		}
		for (;v&3;v++) pDBits[v]=0;
	}
	ERROR3IF((w!=(pBBMI->biSizeImage>>2) || v!=pABMI->biSizeImage), "Alex messed up MakeGreyscale32to8 bitmap sizes")
	return TRUE;
}

#endif //EXCLUDE_FROM_RALPH

/********************************************************************************************

>	static BOOL BfxALU::IsGreyscaleBitmap(KernelBitmap * pKB);
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		pKB the bitmap to operate on
	Outputs:	None
	Returns:	TRUE if a 8bit GS bitmap else FALSE
	Purpose:	Determines whether or not a bitmap is 32bpp
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxALU::MakeGreyscale, BFXALU::MakeGreyscale32to8

This routine returns false if the bitmap is not a correctly formed 8 bpp bitmap. It does
not return or set errors

********************************************************************************************/

BOOL BfxALU::IsGreyscaleBitmap(KernelBitmap * pKB)
{
	if ( (!pKB) || (!pKB->ActualBitmap) )
		return FALSE;

	return IsGreyscaleBitmap(pKB->ActualBitmap);
}

/********************************************************************************************

>	static BOOL BfxALU::IsGreyscaleBitmap(OILBitmap * pKB);
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		pKB the bitmap to operate on
	Outputs:	None
	Returns:	TRUE if a 8bit GS bitmap else FALSE
	Purpose:	Determines whether or not a bitmap is 32bpp
	Errors:		None
	Scope:		Public
	SeeAlso:	BfxALU::MakeGreyscale, BFXALU::MakeGreyscale32to8

This routine returns false if the bitmap is not a correctly formed 8 bpp bitmap. It does
not return or set errors

********************************************************************************************/

BOOL BfxALU::IsGreyscaleBitmap(OILBitmap * pOilBmp)
{
	if ( (!pOilBmp) || (!pOilBmp->IsKindOf(CC_RUNTIME_CLASS(WinBitmap))) )
		return FALSE;

	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)pOilBmp)->BMInfo->bmiHeader);
	if ((pBMI->biBitCount!=8) || (pBMI->biClrUsed!=0x100)) return FALSE;
	DWORD * pPal = (DWORD *)(void *)(pBMI +1 /*ptr arith*/);
	for (DWORD x=0; x<0x100; x++) if ((pPal[x]&0x00ffffff) != (x|(x<<8)|(x<<16))) return FALSE;
	return TRUE;
}

#ifndef WEBSTER
#ifndef EXCLUDE_FROM_RALPH

/********************************************************************************************

>	BOOL BfxALU::Octree (KernelBitmap * * ppOutput)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		None
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::Octree (KernelBitmap * * ppOutput)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_8(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	
	int result = AccusoftFilters::pfnIMG_octree_color (AccusoftHandle, 8, NULL, 256);

	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_OCTREECOLOUR);
}


/********************************************************************************************

>	BOOL BfxALU::SharpenBlur(KernelBitmap * * ppOutput, LONG Degree, LONG Times)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		Degree = degree of sharpening (+ve) or blurring (-ve)
				Times = number of times to apply
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::SharpenBlur(KernelBitmap * * ppOutput, LONG Degree, LONG Times)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;
	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <=8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_24(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
	if (Times>20) Times=20;
	for (int i=1; i<=Times; i++)
	{
		int result = AccusoftFilters::pfnIMG_sharpen_bitmap(AccusoftHandle, (int) Degree);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}
		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), (Degree>0)?IDS_BFX_SHARPEN:IDS_BFX_BLUR);
}

/********************************************************************************************

>	BOOL BfxALU::RemoveDither(KernelBitmap * * ppOutput, LONG Thresh, LONG QuantCols, TraceMethod Method)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		Thresh = Threshold for a sharp edge (0-255). No dither removal at 0, highest at 255
				QuantCols = number of quantisation colours to use (17-255 colour, 2-255 GS)
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

The bitmap becomes 32 bits so use this for tracing only!

********************************************************************************************/

int BfxALUQuantisationRoutine( const void *arg1, const void *arg2 )
{
	QuantisationCell * qc1 = *((QuantisationCell * *)(arg1));
	QuantisationCell * qc2 = *((QuantisationCell * *)(arg2));
	return ( ((qc1->pixels)>(qc2->pixels))?-1:( ((qc1->pixels)<(qc2->pixels))?1:0 ) );
}

// QUANT_SHIFT is Log2 width of the colour quantisation cube, i.e. 4 means the cubes are 16x16x16 colour values.
// In this case there would be 17x17x17 of them (as 0 & 255 are on centre values).
#define QUANT_SHIFT 4
#define QUANT_SIZE ((1<<(8-QUANT_SHIFT))+1)
#define QUANT_ROUND (1<<(QUANT_SHIFT-1))

// here's what we want to do
//#define QMEMBER(x,y,z) [((x)+QUANT_SIZE*((y)+QUANT_SIZE*(z)))]
// and here's how to do it faster
#define QMULT(w) (((w)<<(8-QUANT_SHIFT))+(w))
#define QMEMBER(x,y,z) [(__mtemp=y+QMULT(z),x+QMULT(__mtemp))]

#define QUANT_NUMBER (QUANT_SIZE*QUANT_SIZE*QUANT_SIZE)

BOOL BfxALU::RemoveDither(KernelBitmap * * ppOutput, LONG Thresh, LONG QuantCols, TraceMethod Method)
{
	TRACEUSER("Alex","RemoveDither called Method %d, Degree %d\n");
	
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");

	LONG OutputBPP=1;
	LONG NumPasses=1;
	LONG Width=0;
	LONG Depth=0;
	LONG Height=0;
	LONG l;
	LONG __mtemp;
	if (!GetSize(B, &Width, &Height, &Depth)) return FALSE;

	DWORD * InputLine[5];
	BYTE * ByteLine[5];
	BYTE * OutputLine = NULL;
	for (l=0; l<5; l++) InputLine[l]=NULL;

	Thresh=(Thresh*Thresh*3); // must be +ve
	if (Thresh>255*255*3) Thresh=255*255*3;

	if (QuantCols<2) QuantCols=2;
	if ((Method==TRACEMETHOD_256COL) && (QuantCols<17)) QuantCols=17;
	if (QuantCols>255) QuantCols=255;

	LONG TotalCols=QuantCols;

	*ppOutput=NULL;

	// The array of quantisation cells
	//QuantisationCell QuantMap[QUANT_SIZE][QUANT_SIZE][QUANT_SIZE];
	
	QuantisationCell * pQuantMap=NULL;
	if ((pQuantMap=(QuantisationCell *)CCMalloc(sizeof(QuantisationCell)*QUANT_NUMBER))==NULL)
	{
		return FALSE;
	}

	// An array of pointers to cells which get sorted soon
	//QuantisationCell * QuantIndex[QUANT_SIZE][QUANT_SIZE][QUANT_SIZE];
	//QuantisationCell * * QuantSort = &(QuantIndex[0][0][0]);
	
	QuantisationCell * * pQuantIndex = NULL;
	if ((pQuantIndex=(QuantisationCell * *)CCMalloc(sizeof(QuantisationCell *)*QUANT_NUMBER))==NULL)
	{
		CCFree(pQuantMap);
		return FALSE;
	}

	// Make this point to the 1st pointer so we can sort it.
	QuantisationCell * * QuantSort = pQuantIndex;


	ERROR3IF(QUANT_SIZE*QUANT_SIZE*QUANT_SIZE<256, "Too few quantisation steps");

	switch(Method)
	{
		case TRACEMETHOD_MONO:
			OutputBPP=1;
			NumPasses=2;
			break;
		case TRACEMETHOD_GREYSCALE:
			OutputBPP=8;
			NumPasses=1;
			break;
		case TRACEMETHOD_256COL:
			{
				OutputBPP=8;
				NumPasses=2;
				for (LONG i=0; i<QUANT_SIZE; i++) for (LONG j=0; j<QUANT_SIZE; j++) for (LONG k=0; k<QUANT_SIZE; k++)
				{
					(pQuantMap)QMEMBER(i,j,k).rsum=(pQuantMap)QMEMBER(i,j,k).gsum=(pQuantMap)QMEMBER(i,j,k).bsum=(pQuantMap)QMEMBER(i,j,k).pixels=0;
					(pQuantMap)QMEMBER(i,j,k).index=-1;
					(pQuantIndex)QMEMBER(i,j,k)=&(pQuantMap)QMEMBER(i,j,k);
				}
			}
			break;
		case TRACEMETHOD_TRUECOL:
			OutputBPP=32;
			NumPasses=1;
			break;
		default:
			ERROR2(FALSE,"BfxALU::RemoveDither doesn't understand trace method");
			break;
	}

	KernelBitmap * pKB = NewBitmap(B, 0, 0, OutputBPP, NULL, IDS_BFX_REMOVEDITHER);

	if (!pKB)
	{
		CCFree(pQuantMap);
		CCFree(pQuantIndex);
		return FALSE; // error already set
	}

	if (OutputBPP==1 && Depth==1)
	{
		DWORD * SPalette= (DWORD*)(void*)(((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiColors);
		if ((SPalette[0] & 0x00ffffff)==0 && (SPalette[1] & 0x00ffffff)==0x00ffffff)
		{
			// Forsooth! It is a true mono bitmap.
		
			// We know a song about that:
			// 		memcpy memcpy, its so much quicker
			//		mono bitmaps handled so much slicker
			//		don't bother trying to remove their noise
			//		as they don't have any (not by our definition anywoise)
			// (OK, enough of that).

			memcpy(/*dest*/(((WinBitmap *)(pKB->ActualBitmap))->BMBytes), /*src*/(((WinBitmap *)(B->ActualBitmap))->BMBytes),
				   (((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiHeader.biSizeImage));
			RGBQUAD * Palette= (((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiColors);
			Palette[0].rgbRed=Palette[0].rgbBlue=Palette[0].rgbGreen=0;
			Palette[1].rgbRed=Palette[1].rgbBlue=Palette[1].rgbGreen=255;
			*ppOutput=pKB;
			CCFree(pQuantMap);
			CCFree(pQuantIndex);
			return TRUE;
		}
	}

	LONG x;
	LONG y;

/*
	LONG NoiseMatrix[5][5]={
		{  32, 128, 192, 128,  32 },
		{ 128, 256, 256, 256, 128 },
		{ 192, 256, 256, 256, 192 },
		{ 128, 256, 256, 256, 128 },
		{  32, 128, 192, 128,  32 }};

	LONG NoiseMatrix[5][5]={
		{ 1, 2, 3, 2, 1 },
		{ 2, 4, 4, 4, 2 },
		{ 3, 4, 4, 4, 3 },
		{ 2, 4, 4, 4, 2 },
		{ 1, 2, 3, 2, 1 }};
// Sum=68
*/
	LONG NoiseMatrix[5][5]={
		{ 1, 4, 6, 4, 1 },
		{ 4, 8, 8, 8, 4 },
		{ 6, 8, 8, 8, 6 },
		{ 4, 8, 8, 8, 4 },
		{ 1, 4, 6, 4, 1 }};

	LONG * NoiseMatrixP[5];

	LONG Distrib[256];
	LONG Points=0;
	for (LONG n=0; n<256; n++) Distrib[n]=0;

	if (Method==TRACEMETHOD_GREYSCALE)
	{
		double QuantPoints[255];
		double QuantDist=256.0/(QuantCols-1);
		double q=-0.5*QuantDist;
		for (n=0; n<QuantCols; n++)
		{
			QuantPoints[n]=q;
			q+=QuantDist;
		}
		LONG cp=0;
		for (n=0; n<256; n++)
		{
			while ((cp<QuantCols) && (n>QuantPoints[cp])) cp++;
			Distrib[n]=(LONG)(QuantDist*(cp-1));
			if (Distrib[n]<0) Distrib[n]=0;
			if (Distrib[n]>255) Distrib[n]=255;
		}
	}

	OutputLine=(BYTE *) CCMalloc(Width<<2);
	if (!OutputLine)
	{
			CCFree(pQuantMap);
			CCFree(pQuantIndex);
			delete pKB;
			return FALSE;
	}
	for (x=0; x<(Width<<2); x++) OutputLine[x]=0;

	for (l=0; l<5; l++)
	{
		if ((InputLine[l]=(DWORD *)/*assign*/CCMalloc((Width+4)<<2))==NULL)
		{
			for (l=0; l<5; l++) if (InputLine[l]) CCFree(InputLine[l]);
			CCFree(OutputLine);
			delete pKB;
			CCFree(pQuantMap);
			CCFree(pQuantIndex);
			return FALSE;
		};
		ByteLine[l]=(BYTE *)InputLine[l];
		NoiseMatrixP[l]=&(NoiseMatrix[l][0]);
	}
	

	for (LONG Pass=0; Pass<NumPasses; Pass++)
	{

		// First of all we must convert a new block to 32 bpp

		for (y=0; y<Height; y++)
		{	
			// Shuffle the existing lines down
			// L4 shifts into L3
			// ..
			// L1 shifts into L0
			//
			// L0 is forgotten and its memory space is used by the new L4
			
			DWORD * L0=InputLine[0];
			for (l=0; l<5; l++)
			{
				InputLine[l]=(l==4)?L0:InputLine[l+1];
			}

			// at y=0 we must fill in all lines, otherwise just the new line (line 4)
			for (l=(y==0)?0:4; l<5; l++)
			{
				LONG sy=y+l-2;
				if (sy<0) sy=0;
				if (sy>=Height) sy=Height-1;
				ConvertScanLineToDWORD(B,sy,&(InputLine[l][2]));
				InputLine[l][0]=InputLine[l][1]=InputLine[l][2]; // duplicate 1st pixel
				InputLine[l][Width+3]=InputLine[l][Width+2]=InputLine[l][Width+1]; // duplicate last pixel
			}

			for (l=0; l<5; l++) ByteLine[l]=(BYTE *)InputLine[l];

			LONG r;
			LONG g;
			LONG b;

			// First create the output scanline
			BYTE * pOutput=OutputLine;
			if (Depth==1)
			{
				// This is a great optimisation for mono source where the pixels will always be completely different
				// (and thus be unaffected as they'll be over the threshold) or identical (and won't affect things
				// as they are identical). Thus we might as well just memcpy the line
				memcpy(OutputLine, &(ByteLine[2][2<<2]), Width<<2);
			}
			else
			{
#ifdef FASTREMOVEDITHER

				FastRemoveDither(ByteLine, NoiseMatrixP, (DWORD *) OutputLine, Width, Thresh,
					(Method==TRACEMETHOD_GREYSCALE || Method==TRACEMETHOD_MONO)?1:0);

#else
				SlowRemoveDither(ByteLine, NoiseMatrix, OutputLine, Width, Thresh,
					(Method==TRACEMETHOD_GREYSCALE || Method==TRACEMETHOD_MONO));
#endif
			}
			// Now write it back

			void * VImage = (((WinBitmap *)(pKB->ActualBitmap))->BMBytes);
			switch (Method)
			{
				case TRACEMETHOD_GREYSCALE:
				{
					BYTE * Dest = &(((BYTE *) VImage)[((Width+3)&~3)*y]);
					for (x=0; x<Width; x++) 
					{
						*(Dest++)=(BYTE)(DWORD)Distrib[*pOutput];
						pOutput+=4;
					}
				}
				break;
				case TRACEMETHOD_MONO:
				{
					if (!Pass) // Mono - 1st pass modify distribution curve
					{
						for (x=0; x<Width; x++) 
						{
							r=(LONG)(DWORD)(*pOutput);
							pOutput+=4;
							Distrib[r]++;
							Points++;
						}
					}
					else
					{
						BYTE * Dest = (&(((BYTE *) VImage)[(((Width+31)&~31)>>3)*y]))-1/*as we preinc*/;
						for (x=0; x<Width; x++) 
						{
							if (!(x&7)) *(++Dest)=0;
							*Dest |= Distrib[(LONG)(DWORD)(*pOutput)] >> (x&7);
							pOutput+=4;
						}
					}
				}
				break;
				case TRACEMETHOD_TRUECOL:
				{
					memcpy(&(((DWORD *) VImage)[Width*y]), pOutput, Width<<2);
				}
				break;
				case TRACEMETHOD_256COL:
				{
					if (!Pass)
					{
						for (x=0; x<Width; x++) 
						{
							b=(LONG)(DWORD)*(pOutput++);
							g=(LONG)(DWORD)*(pOutput++);
							r=(LONG)(DWORD)*(pOutput++);
							pOutput++;
							LONG qr=(r+QUANT_ROUND)>>QUANT_SHIFT;
							LONG qg=(g+QUANT_ROUND)>>QUANT_SHIFT;
							LONG qb=(b+QUANT_ROUND)>>QUANT_SHIFT;
							QuantisationCell * pQuantCell=&(pQuantMap)QMEMBER(qr,qg,qb);
							pQuantCell->pixels++;
							pQuantCell->rsum+=r;
							pQuantCell->gsum+=g;
							pQuantCell->bsum+=b;
						}
					}
					else
					{
						BYTE * Dest = &(((BYTE *) VImage)[((Width+3)&~3)*y]);
						for (x=0; x<Width; x++) 
						{
							b=(LONG)(DWORD)*(pOutput++);
							g=(LONG)(DWORD)*(pOutput++);
							r=(LONG)(DWORD)*(pOutput++);
							pOutput++;
							LONG qr=(r+QUANT_ROUND)>>QUANT_SHIFT;
							LONG qg=(g+QUANT_ROUND)>>QUANT_SHIFT;
							LONG qb=(b+QUANT_ROUND)>>QUANT_SHIFT;
							QuantisationCell * pQuantCell=&(pQuantMap)QMEMBER(qr,qg,qb);
							LONG Index = pQuantCell->index;
							if (Index<0)
							{
								// better colourtrans it then
								LONG rsum = pQuantCell->rsum;
								LONG gsum = pQuantCell->gsum;
								LONG bsum = pQuantCell->bsum;

								LONG MinD=0x7fffffff;
								Index=0;
								for (LONG m=0; m<TotalCols; m++)
								{
									// Find the colour difference
									LONG d=IntegerSquare(QuantSort[m]->rsum - rsum)
											+ IntegerSquare(QuantSort[m]->gsum - gsum)
											+ IntegerSquare(QuantSort[m]->bsum - bsum);
									if (d<MinD)
									{
										// Note the cube with the smallest difference
										MinD=d;
										Index=m;
									}
								}
								// write it back in so we dont have to do it again
								pQuantCell->index=Index;
							}
							ERROR3IF(((DWORD)Index)>255, "Wierd index value");
							*Dest++=(BYTE)(DWORD)Index;
						}
					}
				}
				break;
				default:
				{
					ERROR3("We shouldn't get here...");
				}
				break;
			} // End of case


			BOOL Continue=TRUE;
			if ((y & 15)==0) Continue=ContinueSlowJob((100*y/Height+100*Pass)/NumPasses);
			if (!Continue)
			{
				ERROR1RAW(IDE_BFX_ABORT);
				for (l=0; l<5; l++) if (InputLine[l]) CCFree(InputLine[l]);
				CCFree(OutputLine);
				delete pKB;
				CCFree(pQuantMap);
				CCFree(pQuantIndex);
				return FALSE;
			}
		} // End of y loop
		
		switch (Method)
		{
			case TRACEMETHOD_MONO:
				if (!Pass) {
					RGBQUAD * Palette= (((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiColors);
					LONG Sum=0;
					for (LONG n=0; n<256; n++)
					{
						BOOL Thresh=(Sum>Points/2)|(n==255);
						Sum+=Distrib[n];
						Distrib[n]=Thresh?0x80:0;
					}
					Palette[0].rgbRed=Palette[0].rgbBlue=Palette[0].rgbGreen=0;
					Palette[1].rgbRed=Palette[1].rgbBlue=Palette[1].rgbGreen=255;
				}
				break;
			case TRACEMETHOD_256COL:
				if (!Pass) {
					// First bodge the issue so the gun maxima values always appear, which ensures
					// we can represent a full contrast range. This probably isn't necessary as we're not
					// dithering it but heh, we might as well keep white as white etc.

					RGBQUAD * Palette= (((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiColors);
					LONG totalpixels=Width * Height;
					LONG pixmax=totalpixels * LONG(0xff);
					for (LONG r=0; r<QUANT_SIZE; r+=QUANT_SIZE-1)
						for (LONG g=0; g<QUANT_SIZE; g+=QUANT_SIZE-1) 
							for (LONG b=0; b<QUANT_SIZE; b+=QUANT_SIZE-1) 					
							{
								QuantisationCell * pQuantCell=&(pQuantMap)QMEMBER(r,g,b);
								pQuantCell->rsum=r?pixmax:0;
								pQuantCell->gsum=g?pixmax:0;
								pQuantCell->bsum=b?pixmax:0;
								pQuantCell->pixels=totalpixels;
							}

					// And now for some greyscales
					// This avoids inverse-Acorn colourtrans problems
					for (r=1; r<QUANT_SIZE-1/*even*/; r+=2)
					{
						QuantisationCell * pQuantCell=&(pQuantMap)QMEMBER(r,r,r);
						pQuantCell->rsum=pQuantCell->gsum=pQuantCell->bsum=
							(r<<QUANT_SHIFT) * (pQuantCell->pixels=/*assign*/totalpixels);
					}

					// Now sort the index
					qsort((void *)(QuantSort),QUANT_SIZE*QUANT_SIZE*QUANT_SIZE,sizeof(QuantisationCell *),
						&BfxALUQuantisationRoutine);
					TotalCols=0;
					LONG n;

					// blank the palette
					for (n=0; n<256; n++) Palette[n].rgbRed=Palette[n].rgbBlue=Palette[n].rgbGreen=Palette[n].rgbReserved=0;
					
					// Now calculate the real av RGB values for each subcube
					for (n=0; (n<QUANT_SIZE*QUANT_SIZE*QUANT_SIZE) ; n++ ) if (QuantSort[n]->pixels>0) 
					{
						LONG r=(QuantSort[n]->rsum+(LONG)(QuantSort[n]->pixels>>1))/QuantSort[n]->pixels;
						LONG g=(QuantSort[n]->gsum+(LONG)(QuantSort[n]->pixels>>1))/QuantSort[n]->pixels;
						LONG b=(QuantSort[n]->bsum+(LONG)(QuantSort[n]->pixels>>1))/QuantSort[n]->pixels;
						if (r<0) r=0; 							// Clip
						if (r>255) r=255;
						if (g<0) g=0; 							// Clip
						if (g>255) g=255;
						if (b<0) b=0; 							// Clip
						if (b>255) b=255;
						QuantSort[n]->rsum = r;
						QuantSort[n]->gsum = g;
						QuantSort[n]->bsum = b;
						ERROR3IF((n>0) && (QuantSort[n-1]->pixels<QuantSort[n]->pixels), "urm, they aren't in order...");
					}
					
					// Label the most important subcubes and fill in the palette
					for (n=0; (n<QuantCols)  && (QuantSort[n]->pixels>0); n++)
					{
						QuantSort[n]->index=n;
						TotalCols=n+1;
						Palette[n].rgbRed=(BYTE)(DWORD)(QuantSort[n]->rsum);
						Palette[n].rgbBlue=(BYTE)(DWORD)(QuantSort[n]->bsum);
						Palette[n].rgbGreen=(BYTE)(DWORD)(QuantSort[n]->gsum);
					}

					// Now find the nearest matches for the other colours in the colour cube
					ERROR3IF(!TotalCols, "OK, how come this image had no colours in it *at all*");
				}				
				break;
			case TRACEMETHOD_GREYSCALE:
				{
					RGBQUAD * Palette= (((WinBitmap *)(pKB->ActualBitmap))->BMInfo->bmiColors);
					for (LONG c=0; c<256; c++)
						Palette[c].rgbRed=Palette[c].rgbBlue=Palette[c].rgbGreen=(BYTE)(DWORD)c;
				}
				break;
			default:
				break;
		}
	}

	for (l=0; l<5; l++) if (InputLine[l]) CCFree(InputLine[l]);
	CCFree(OutputLine);
	*ppOutput=pKB;
	CCFree(pQuantMap);
	CCFree(pQuantIndex);
	return TRUE;
}

/********************************************************************************************

>	void BfxALU::SlowRemoveDither(BYTE * ByteLine[5], LONG NoiseMatrix[5][5],
				 				  BYTE * OutputLine, LONG Width, LONG Thresh, BOOL Mono)
					
	Author:		Alex
	Created:	28/04/95
	Inputs:		ByteLine - a ptr to 5 ptrs to extended scanlines of 32 bit source data
				NoiseMatrix - the variable convolution matrix
				Width = the (unextended) scanline width. Th input scanline must be extended by 2 pixels each side
				Thresh = the noise cutoff threshold
				Mono = true to make the output single channel
	Outputs:	BYTE * OutputLine - the 32 bit scanline to be filled in
	Returns:	None
	Purpose:	Removes dither (slowly)
	Errors:		Error 3 for algorithmic failures
	Scope:		Protected
	SeeAlso:	BfxALU::RemoveDither, FastRemoveDither

********************************************************************************************/

void BfxALU::SlowRemoveDither(BYTE * ByteLine[5], LONG NoiseMatrix[5][5], BYTE * OutputLine, LONG Width, LONG Thresh, BOOL Mono)
{
	for (LONG x=0; x<Width; x++) 
	{
		LONG r=0;
		LONG g=0;
		LONG b=0;

		LONG s=0;

		LONG CB=(LONG)(ByteLine[2][2*4]);
		LONG CG=(LONG)(ByteLine[2][2*4+1]);
		LONG CR=(LONG)(ByteLine[2][2*4+2]);

		for (LONG dy=0; dy<=4; dy++)
		{	
			for (LONG dx=0; dx<=4; dx++)
			{
				LONG v=NoiseMatrix[dx][dy];

				LONG LB=(LONG)(DWORD)(*(ByteLine[dy]++));
				LONG LG=(LONG)(DWORD)(*(ByteLine[dy]++));
				LONG LR=(LONG)(DWORD)(*(ByteLine[dy]++));

				LONG dr=LR-CR;
				LONG dg=LG-CG;
				LONG db=LB-CB;
				if (((dr*dr)+(dg*dg)+(db*db))<=Thresh)
				{
					s+=v;
					r+=v*LR;
					g+=v*LG;
					b+=v*LB;
				}
				ByteLine[dy]++; // past reserved byte
			}
			ByteLine[dy]-=16; // we've advanced it 5 and we only wanted to advance it 1.
		}

		if (s) // should always be true as centre always has value
		{
			if (Mono)
			{
				r=r+g+b; 								// Sum RGB
				s*=3; 									// Divide by 3x more
				r+=s>>1;								// Add rounding factor
			 	r=r/s;									// Find result
				if (r<0) r=0; 							// Clip
				if (r>255) r=255;
			}
			else
			{
				r+=s>>1;
				g+=s>>1;
				b+=s>>1;
				r=r/s;
				g=g/s;
				b=b/s;
				if (r<0) r=0; 							// Clip
				if (r>255) r=255;
				if (g<0) g=0; 							// Clip
				if (g>255) g=255;
				if (b<0) b=0; 							// Clip
				if (b>255) b=255;
			}	
		}
		else
		{
			ERROR3("BfxALU::SlowRemoveDither should never get here!");
		}

		*(OutputLine++)=(BYTE)(DWORD)b;
		*(OutputLine++)=(BYTE)(DWORD)g;
		*(OutputLine++)=(BYTE)(DWORD)r;
		OutputLine++; // skip reserved byte
	}
	return;
}

/********************************************************************************************

>	BOOL BfxALU::SpecialEffect(KernelBitmap * * ppOutput, double * Matrix, BfxSpecialEffect Type)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		Brightness & Contast (-127 to +127)
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::SpecialEffect(KernelBitmap * * ppOutput, double * Matrix, BfxSpecialEffect Type)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
	ERROR2IF((!Matrix) && (Type == BFX_SFX_USER), FALSE, "Must pass a matrix for BFX_SFX_USER");
	char matrix[9];
	if (Type==BFX_SFX_USER) for (int i=0; i<=8; i++) matrix[i]=(char)(LONG)Matrix[i];
	
	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle)) return FALSE;

	if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <=8 )
	{
		int result = AccusoftFilters::pfnIMG_promote_24(AccusoftHandle);
		if (result < 0)
		{
			AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
			ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
		}
	}

	int result = AccusoftFilters::pfnIMG_process_bitmap(AccusoftHandle, (int) Type, matrix);
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	UINT res = IDS_BFX_EFFECTNAME0;
	switch (Type)
	{
		case BFX_SFX_USER:
			res=IDS_BFX_EFFECTNAME0;
			break;
		case BFX_SFX_ISOLATEPOINTS:
			res=IDS_BFX_EFFECTNAME1;
			break;
		case BFX_SFX_EDGE:
			res=IDS_BFX_EFFECTNAME2;
			break;
		case BFX_SFX_HEDGE:
			res=IDS_BFX_EFFECTNAME3;
			break;
		case BFX_SFX_VEDGE:
			res=IDS_BFX_EFFECTNAME4;
			break;
		case BFX_SFX_P45EDGE:
			res=IDS_BFX_EFFECTNAME5;
			break;
		case BFX_SFX_N45EDGE:
			res=IDS_BFX_EFFECTNAME6;
			break;
		case BFX_SFX_LAPLACIAN:
			res=IDS_BFX_EFFECTNAME7;
			break;
		case BFX_SFX_DIALATION:
			res=IDS_BFX_EFFECTNAME8;
			break;
		case BFX_SFX_ROBERTS:
			res=IDS_BFX_EFFECTNAME9;
			break;
		default:
			res=IDS_BFX_EFFECTNAME0;
			break;
	}

		return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), res);
}

/********************************************************************************************

>	BOOL BfxALU::Resize(KernelBitmap * * ppOutput, LONG Width, LONG Height, BOOL LinearInterpolation)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		Brightness & Contast (-127 to +127)
	Outputs:	*ppOutput = a pointer to the output bitmap
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::Resize(KernelBitmap * * ppOutput, LONG Width, LONG Height, BOOL LinearInterpolation)
{
	ERROR2IF((!ppOutput),FALSE, "BfxALU output parameter must be non-null");
		
	// Accusoft have a bug on their linear interpolation code which puts a grey line down
	// the left hand side of the image, so we must bodge it!!!!!
	BOOL DoBodge = FALSE;
	int bpp = B->GetBPP();
#ifdef BODGE_ACCUSOFT_RESIZE
	if (LinearInterpolation && (bpp == 4 || bpp == 8 || bpp == 24 || bpp == 32))
		DoBodge = TRUE;
#endif

	int AccusoftHandle = -1;
	if (!MakeAccusoftHandle(&AccusoftHandle, DoBodge))
		return FALSE;

	int result = 0;
	if (LinearInterpolation)
	{
		// Accusoft require a 24bpp bitmap for linear interpolation
		// If less than or equal 8bpp 
//		if ( (((WinBitmap *)(B->ActualBitmap))->BMInfo->bmiHeader).biBitCount <=8 ) SCARY!!!!!
		LONG NewWidth = Width;
#ifdef BODGE_ACCUSOFT_RESIZE
		// If doing the bodge to get around the Accusoft 24bpp linear interpolation problem, then
		// we make the bitmap 3 times as wide as it should be to hide the grey pixels on the left
		// hand side when we remove this extra space after the inerpolation has happened.
		// We therefore need to make the resize 3 times the width it should be.
		if (DoBodge)
			NewWidth *= 3;
#endif
		if (bpp <= 8)
		{
			result = AccusoftFilters::pfnIMG_promote_24(AccusoftHandle);
			if (result < 0)
			{
				AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
				ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
			}
		}
		result = AccusoftFilters::pfnIMG_resize_bitmap_interp(AccusoftHandle, (int) NewWidth, (int) Height);
	}
	else
	{
		// Do a straight resize
		result = AccusoftFilters::pfnIMG_resize_bitmap(AccusoftHandle, (int) Width, (int) Height);
	}
	
	if (result < 0)
	{
		AccusoftFilters::pfnIMG_delete_bitmap(AccusoftHandle);
		ERROR1(FALSE, IDE_ACCUSOFT_ERROR - result);
	}
	
	return MakeKernelBitmap(AccusoftHandle, ppOutput, FALSE, &(B->ActualBitmap->GetName()), IDS_BFX_RESIZE, DoBodge);
}

/********************************************************************************************

>	BOOL BfxALU::AdjustBrightnessContrastColour(LPBYTE Source, LPBYTE Dest, LONG Size, LONG Width, LONG Height,
							LONG Brightness, LONG Contrast, LONG Colour, BOOL SingleChannel)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		Source = pointer to source bytes
				Size = size of entries
				Brightness, Contast, Colour (-127 to +127)
				SingleChannel set if just to do a .. urm ... single channel.
	Outputs:	Dest bytes filled in
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	A new bitmap is created as per function name
	Errors:		Error 2 if init hasn't been called or GDraw fails
				Error 3 if windows and some other Oil layer are stangely mixed...
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

BOOL BfxALU::AdjustBrightnessContrastColour(LPBYTE Source, LPBYTE Dest, LONG Size, LONG Width, LONG Height,
							LONG Brightness, LONG Contrast, LONG Colour, BOOL SingleChannel)
{
 	// We make a LUT which is Output = LUT[gun, sumofguns]

	double dBrightness = (Brightness*1.0)/127.0   *0.75; // *0.75 is the 'Charles' factor.
	double dContrast = pow(2.0,3.0*(Contrast*1.0)/127.0);
	double dColour = pow(2.0,3.0*(Colour*1.0)/127.0);

	// GunOut = dBrightness + dContrast * ((GunSum/3) + dColour * (GunIn- GunSum/3)) + (1-dContrast)*0.5;
	//        = (dBrightness + ((1-dContrast)*0.5)) + GunSum * (dContrast/3.0 * (1-dColour)) + GunIn * (dContrast*dColour)

	double PropA = dContrast * dColour;
	double PropB = dContrast / 3.0 * (1.0-dColour);
	double Offset = dBrightness + (1.0-dContrast)*0.5;

	LONG pa=(LONG)(PropA*(1<<15));
	LONG pb=(LONG)(PropB*(1<<15));
	LONG po=(LONG)(Offset*((1<<15)*255)+/* for rounding */(1<<14));

 	BYTE * Data = (BYTE *) CCMalloc(3*256*256);
	if (!Data) return FALSE; // error already set

	// Please note how we're *NOT* using doubles in the loop as that's
	// sooooo sloooooow on 486SX. We use 17.15 arithmetic throughout

#ifndef FASTLINEARABK
	LONG a;
	LONG b;
	LONG r;
	for (a=0; a<256; a++) for (b=0; b<256*3; b++)
	{
		r=(a*pa+b*pb+po)>>15; /* we did the rounding in the offset calc */
		LUTBYTE(a,b)=(r<0)?0:((r>255)?255:(BYTE)(LONG)(r));
	}
#else
	FastLinearABK(Data, pa, pb, po, 256*3);
#endif
	
	DWORD Sum;

	if (!SingleChannel)
	{
		for (long y = 0 ;y<Height; y++)
		{
			for (long x = 0; x<Width; x++)
			{
				Sum=((DWORD)Source[0])+((DWORD)Source[1])+((DWORD)Source[2]);
				Dest[0]=LUTBYTE(Source[0], Sum);
				Dest[1]=LUTBYTE(Source[1], Sum);
				Dest[2]=LUTBYTE(Source[2], Sum);
				Source+=Size;
				Dest+=Size;
			}
			Source = (LPBYTE)(void *)((((DWORD)(void *)Source) +3)&(~3));
			Dest = (LPBYTE)(void *)((((DWORD)(void *)Dest) +3)&(~3));
		}
	}
	else
	{
		for (long y = 0 ;y<Height; y++)
		{
			for (long x = 0; x<Width; x++)
			{
				*Dest=LUTBYTE((*Source), (*Source));
				Source+=Size;
				Dest+=Size;
			}
			Source = (LPBYTE)(void *)((((DWORD)(void *)Source) +3)&(~3));
			Dest = (LPBYTE)(void *)((((DWORD)(void *)Dest) +3)&(~3));
		}
	}

	CCFree(Data);	
	return TRUE;
}

/********************************************************************************************

>	void BfxALU::ConvertScanLineToDWORD(KernelBitmap * pBitmap, LONG Line, DWORD * pBuffer)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		pBitmap pointer to bitmap
				Line = line number
				pBuffer = buffer
	Outputs:	None
	Returns:	None
	Purpose:	Converts a scanline in the bitmap to a 32 bit scanline
	Errors:		None
	Scope:		Protected
	SeeAlso:	-

THIS FUNCTION HAS NO ERROR CHECKING. How does it smell? TERRIBLE

(i.e. you are expected to do your own.). This is because it must be **FAST**

********************************************************************************************/

void BfxALU::ConvertScanLineToDWORD(KernelBitmap * pBitmap, LONG Line, DWORD * pBuffer)
{
	BITMAPINFOHEADER * pBMI=&(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiHeader);
	RGBQUAD * pPal=(((WinBitmap *)(pBitmap->ActualBitmap))->BMInfo->bmiColors);
	void * VImage = (((WinBitmap *)(pBitmap->ActualBitmap))->BMBytes);
	LONG Width=pBMI->biWidth;
	switch (pBMI->biBitCount)
	{
		case 32:
		{
			DWORD * Source = &(((DWORD *) VImage)[Width*Line]);
			for (LONG w=0; w<Width; w++) pBuffer[w]=Source[w] & 0x00ffffff;
			return;
		}
		break; // not really needed
		case 24:
		{
			BYTE * Source = &(((BYTE *) VImage)[(((Width*3)+3)&~3)*Line]);
			LONG b=0;
			for (LONG w=0; w<Width; w++)
			{	
				pBuffer[w]=(((LONG)(Source[b])))|(((LONG)(Source[b+1]))<<8)|(((LONG)(Source[b+2]))<<16);
				b+=3; /// Grrr MS bug prevents us from doing this efficiently
			}
			return;
		}
		break; // not really needed
		case 8:
		{
			BYTE * Source = &(((BYTE *) VImage)[((Width+3)&~3)*Line]);
			for (LONG w=0; w<Width; w++)
			{
				pBuffer[w]=0x00ffffff & *((DWORD *)(&pPal[Source[w]]));
			}
			return;
		}
		break; // not really needed
		case 4:
		{
			BYTE * Source = &(((BYTE *) VImage)[(((Width+7)&~7)>>1)*Line]);
			for (LONG w=0; w<Width; w++)
			{
				pBuffer[w]=0x00ffffff & *((DWORD *)(&pPal[ (Source[w>>1]>>( (1-(w&1)) <<2) )&0xf]));
			}
			return;
		}
		break; // not really needed
		case 2:
		{
			BYTE * Source = &(((BYTE *) VImage)[(((Width+15)&~15)>>2)*Line]);
			for (LONG w=0; w<Width; w++)
			{
				pBuffer[w]=0x00ffffff & *((DWORD *)(&pPal[ (Source[w>>2]>>( (3-(w&3)) <<1) )&0x3]));
			}
			return;
		}
		break; // not really needed
		case 1:
		{
			BYTE * Source = &(((BYTE *) VImage)[(((Width+31)&~31)>>3)*Line]);
			for (LONG w=0; w<Width; w++)
			{
				pBuffer[w]=0x00ffffff & *((DWORD *)(&pPal[ (Source[w>>3]>>( (7-(w&7)) /*<<0*/) )&0x1]));
			}
			return;
		}
		break; // not really needed
		default:
		{
			ERROR3("BPP not supported");
		}
		break; // not really needed
	}
	return; // not really needed
}

/********************************************************************************************

>	void BfxALU::ConvertScanLineToDWORD(KernelBitmap * pBitmap, LONG Line, DWORD * pBuffer)
					
	Author:		Alex
	Created:	28/01/95
	Inputs:		pBitmap pointer to bitmap
				Line = line number
				pBuffer = buffer
	Outputs:	None
	Returns:	None
	Purpose:	Converts 32 bit scanline to a scanline in the bitmap
	Errors:		None
	Scope:		Protected
	SeeAlso:	-

THIS FUNCTION HAS NO ERROR CHECKING. How does it smell? TERRIBLE

(i.e. you are expected to do your own.). This is because it must be **FAST**

********************************************************************************************/

void BfxALU::ConvertScanLineFromDWORD(KernelBitmap * pBitmap, LONG Line, DWORD * pBuffer)
{
	return;
}

#endif // EXCLUDE_FROM_RALPH
#endif //WEBSTER
