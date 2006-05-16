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
// This is the header file for the bitmap effect ALU class

/*
*/

#ifndef INC_BFXALU
#define INC_BFXALU

#include "ccobject.h"
class KernelBitmap;
class OILBitmap;

#ifndef EXCLUDE_FROM_RALPH

#include "gconsts.h" // For GMATRIX
#include "tracectl.h" // for enum TraceMethod - annoying...

class GDrawContext;
class BfxALULUT;
class Path;
class String_256;


typedef struct
{
	BYTE	R;
	BYTE	G;
	BYTE	B;
	BYTE	T;
} AluPix32;

enum BfxSpecialEffect { BFX_SFX_USER, BFX_SFX_ISOLATEPOINTS, BFX_SFX_EDGE, BFX_SFX_HEDGE,
						BFX_SFX_VEDGE, BFX_SFX_P45EDGE, BFX_SFX_N45EDGE, BFX_SFX_LAPLACIAN,
						BFX_SFX_DIALATION, BFX_SFX_ROBERTS };

class KernelStatistics /* should be a struct but the C compiler is crap*/
{
	public:

	 INT32  N  ;
	double R  ;
	double R2 ;
	double RX ;
	double RY ;
	double G  ;
	double G2 ;
	double GX ;
	double GY ;
	double B  ;
	double B2 ;
	double BX ;
	double BY ;
	double X  ;
	double Y  ;
	double X2 ;
	double Y2 ;
	double XY ;
	DWORD  Pixel;
	INT32  LowX;
	INT32  LowY;
};
		  
inline INT32 IntegerSquare(INT32 i) { return i*i; }
inline double DoubleSquare(double d) { return d*d; }

/********************************************************************************************

>	class BfxErrorRegionList : public CCObject

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/01/1995
	Purpose:	Stores error region list for tracing

********************************************************************************************/

typedef struct
{
	INT32 X;
	INT32 Y;
	INT32 Area;
} BfxErrorRegionListItem;

class BfxErrorRegionList : public CCObject
{
	CC_DECLARE_DYNCREATE(BfxErrorRegionList);

	public:

	BfxErrorRegionList();			 /* Constructor */
	~BfxErrorRegionList();	     /* Destructor */

	protected:
	
	BfxErrorRegionListItem * pList;
	INT32 ListLength;
	INT32 Used;
	
	INT32 CurrentRead;
	INT32 CurrentWrite;
	BOOL Sorted;

	void Swap( BfxErrorRegionListItem * A, BfxErrorRegionListItem * B) { BfxErrorRegionListItem T;
		T.X=A->X;T.Y=A->Y;T.Area=A->Area;A->X=B->X;A->Y=B->Y;A->Area=B->Area;B->X=T.X;B->Y=T.Y;B->Area=T.Area;};


	public:

	BOOL Next() {if (pList && pList[CurrentRead].Area) CurrentRead++;return TRUE;};
	BOOL Init(UINT32 Length);
	BOOL ResetRead();
	BOOL Empty();
	BOOL Insert(INT32 XCoord, INT32 YCoord, INT32 Area);
	BOOL Sort();
	BOOL IsSpace() { return (CurrentWrite<ListLength); };
	BOOL GetCurrent(INT32 * XCoord, INT32 * YCoord, INT32 * Area)
		{return ((pList) && (*XCoord=pList[CurrentRead].X,*YCoord=pList[CurrentRead].Y,*Area = pList[CurrentRead].Area));};
	BOOL MarkUsed() { return (pList && ((!pList[CurrentRead].Area) || (pList[CurrentRead].Area=/*assign*/1))); };
	INT32 GetSize() { return ListLength; };

};
#endif

/********************************************************************************************

>	class BfxALU : public CCObject

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/1994
	Purpose:	Arithmetic and logic unit for bitmap effects

This class implements the Bitmap Effect ALU (Arithmetic and logic unit). You can think of
this as the Kernel i/f for GDraw when using bitmap effects, but that would be a bit of a
narrow view. The class is there to perform fast arithmetic and logical operations on bitmaps,
and at the moment, we reckon the fastest way to do that is to use GDraw.

The ALU works very like a processor ALU, except each register holds a bitmap (actually a
pointer to a bitmap which can cause some complications), and each operand (like Add) adds
two bitmaps, rather than two integers. In fact, some registers may be set to 'pseudo bitmaps'
like solid colour and linear fills, which are usefully expanded by the ALU at plot time without
occupying memory.

The ALU has the following registers:
	Name			Purpose
	====			=======
	A (Accumulator)	The accumulator is the destination register for all operands, and is the
					source register for many. The accumulator can be 32 bit or 8 bpp grey
					but at the moment only 32 bpp is supported. The T channel of the
					accumulator is always zero (well, that's how the plot operations leave
					it anyway).
	B (Bitmap)		A general purpose bitmap, normally used as a secondary source operand.
	T (Bitmap)		And 8 bit bitmap used for weigthing values on some calls.

Here are some example arithmetic operands:

AddpAqB	A <- pA+qB
AddApB	A <- A+pB
SubApB	A <- A-pB
InvA	A <- 255-A

Here are some example load & store operations

SetA(bitmap)
SetB(bitmap)
SetB(linear fill parameters)

********************************************************************************************/

class BfxALU : public CCObject
{
	CC_DECLARE_DYNCREATE(BfxALU);

	public:

	BfxALU();			 /* Constructor */
	virtual ~BfxALU();	     /* Destructor */

#ifndef EXCLUDE_FROM_RALPH
	virtual BOOL Init();
	virtual BOOL DeInit();

	virtual KernelBitmap * NewBitmap(KernelBitmap *pBitmap,INT32 XAdjust=0,INT32 YAdjust=0,
									 INT32 NewDepth=0, String_256 * pName =NULL, UINT32 ResID =0);

	virtual	BOOL SetA(KernelBitmap * pBitmap=NULL);
	virtual BOOL SetB(KernelBitmap *pBitmap=NULL,INT32 XOffset=0,INT32 YOffset=0,
					  const BYTE * pTable=NULL,DWORD Style=0);
	virtual BOOL SetB(DWORD Colour);
	virtual BOOL SetT(KernelBitmap *pBitmap=NULL,INT32 XOffset=0,INT32 YOffset=0,
					  const BYTE * pTable=NULL,DWORD Style=0);
	virtual BOOL SetT(DWORD Value);

	virtual BOOL PlotB();
	virtual BOOL PlotBLUT(BfxALULUT * LUT);
	
	virtual	BOOL PartTAB();
	virtual	BOOL PartMulTAB();
	virtual	BOOL PartRevMulTAB();
	virtual BOOL AddTAB();
	virtual BOOL SubTAB();

	virtual	BOOL PartKAB(DWORD Value);
	virtual	BOOL PartMulKAB(DWORD Value);
	virtual	BOOL PartRevMulKAB(DWORD Value);
	virtual BOOL AddKAB(DWORD Value);
	virtual BOOL SubKAB(DWORD Value);

	virtual BOOL MarkThresholdError(INT32 Value, DWORD MarkValue, DWORD ClearValue);
	virtual BOOL MarkColourThresholdError(INT32 Value, DWORD Colour, DWORD MarkValue, DWORD ClearValue,
							  double * pAError, double * pBError, INT32 * pTotalPixels, INT32 * pMarkedPixels);

	virtual BOOL MarkPositive(INT32 Value, DWORD MarkValue, DWORD ClearValue,
							  double * pAError = NULL, double * pBError = NULL,
							  INT32 * pTotalPixels = NULL, INT32 * pMarkedPixels = NULL);
	virtual BOOL MarkLargestArea(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
								 BOOL * FoundRegion, INT32 * InitialX, INT32 * InitialY, INT32 * Area);
	virtual	BOOL MarkGivenArea(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
						     BOOL * FoundRegion, INT32 * InitialX, INT32 * InitialY, INT32 * Area);
	virtual BOOL GradFillPath(Path * ThePath, DWORD StartColour, DWORD EndColour,
							  INT32 StartX, INT32 StartY, INT32 EndX, INT32 EndY,
							  BOOL AntiAlias = FALSE, BOOL ColoursAreDWORDs = FALSE);
	virtual BOOL GetStatistics(Path * ThePath, KernelStatistics * pStats);
	virtual BOOL MarkBitByWord(DWORD BitMask, DWORD TheWord);
	virtual BOOL ClearBitByWord(DWORD BitMask, DWORD TheWord);
	virtual BOOL MarkWordByBit(DWORD BitMask, DWORD MarkValue, DWORD ClearValue);
	virtual BOOL BuildErrorRegionList(DWORD MarkValue, DWORD ClearValue, DWORD RegionValue,
									  BfxErrorRegionList * pERL, INT32 MinimumArea);
	virtual BOOL ScanBit(DWORD BitMask, INT32 * X, INT32 * Y, INT32 *pPixel, BOOL * Found, BOOL CheckConnected=FALSE);
	virtual BOOL ScanThreshold(INT32 Value, INT32 * X, INT32 * Y, INT32 *pPixel, BOOL * Found, BOOL CheckConnected=FALSE);
			
	virtual BOOL GetSize(KernelBitmap * pBitmap, INT32 * pXSize=NULL, INT32 * pYSize=NULL, INT32 * pDepth=NULL);

	virtual BOOL ZeroA(DWORD FillValue = 0);
	virtual BOOL ByteCopyBA();

	virtual void TestGD();

#ifndef WEBSTER
PORTNOTE("other", "Remove accusoft effects")
#ifndef EXCLUDE_FROM_XARALX
	// Accusoft effects
	virtual BOOL FlipX(KernelBitmap * * ppOutput);
	virtual BOOL FlipY(KernelBitmap * * ppOutput);
	virtual BOOL Rotate90(KernelBitmap * * ppOutput);
	virtual BOOL Rotate180(KernelBitmap * * ppOutput);
	virtual BOOL Rotate270(KernelBitmap * * ppOutput);

	virtual BOOL Resize(KernelBitmap * * ppOutput, INT32 Width, INT32 Height, BOOL LinearInterpolation);

	virtual BOOL BrightnessContrast(KernelBitmap * * ppOutput, INT32 Brightness=0, INT32 Contrast=0, INT32 Colour=0);

	virtual BOOL BayerMono(KernelBitmap * * ppOutput);
	virtual BOOL HalftoneMono(KernelBitmap * * ppOutput);
	virtual BOOL DiffusionMono(KernelBitmap * * ppOutput);
	virtual BOOL BayerColour(KernelBitmap * * ppOutput);
	virtual BOOL DiffusionColour(KernelBitmap * * ppOutput);
	virtual BOOL PopularityColour(KernelBitmap * * ppOutput);
	virtual BOOL MakeGreyscale(KernelBitmap * * ppOutput);
#endif //XARALX
#endif //WEBSTER
	virtual BOOL MakeGreyscale32to8(KernelBitmap * * ppOutput);

#endif	
	static  BOOL IsGreyscaleBitmap(KernelBitmap * pKB);
	static  BOOL IsGreyscaleBitmap(OILBitmap * pOilBmp);
#ifndef EXCLUDE_FROM_RALPH
#ifndef WEBSTER
PORTNOTE("other", "Remove accusoft effects")
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL Octree (KernelBitmap * * ppOutput);

	virtual BOOL SharpenBlur(KernelBitmap * * ppOutput, INT32 Degree, INT32 Times);
#endif
	virtual BOOL RemoveDither(KernelBitmap * * ppOutput, INT32 Thresh, INT32 QuantCols, TraceMethod Method);
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL SpecialEffect(KernelBitmap * * ppOutput, double * Matrix, BfxSpecialEffect Type);
#endif
#endif //WEBSTER

	protected:

	virtual	BOOL DoRect();
	virtual BOOL UseBT(DWORD TransparencyStyle=0);
	virtual BOOL UseBK(DWORD TransparencyStyle,DWORD Value);

	virtual BOOL MarkRegions(DWORD MarkValue, DWORD ClearValue,
						     BOOL * FoundRegion, INT32 * InitialX, INT32 * InitialY, INT32 * Area,
							 INT32 * pSize, INT32 * pHighestChain, INT32 * pWidth, DWORD * * ppA);	

#ifndef WEBSTER
PORTNOTE("other", "Remove accusoft effects")
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL MakeAccusoftHandle(INT32 * pHandle, BOOL DoBodge = FALSE);
	virtual BOOL MakeKernelBitmap(INT32 Handle, KernelBitmap * * ppOutput, BOOL Make32Bits = FALSE,
								  String_256 * pName = NULL, UINT32 ResID = 0, BOOL DoBodge = FALSE);
#endif
	virtual BOOL AdjustBrightnessContrastColour(LPBYTE Source, LPBYTE Dest, INT32 Size, INT32 Width, INT32 Height,
							INT32 Brightness, INT32 Contrast, INT32 Colour, BOOL SingleChannel);
#ifndef EXCLUDE_FROM_XARALX
	BOOL MAH_BodgeCopyBitmap(INT32 Width, INT32 Height, INT32 Depth, LPBYTE pSourceBits, LPBYTE pDestBits);
	BOOL MKB_BodgeCopyBitmap(INT32 Width, INT32 Height, INT32 Depth, LPBYTE pSourceBits, LPBYTE pDestBits);
#endif
#endif //WEBSTER

	void ConvertScanLineToDWORD(KernelBitmap * pBitmap, INT32 Line, DWORD * pBuffer);
	void ConvertScanLineFromDWORD(KernelBitmap * pBitmap, INT32 Line, DWORD * pBuffer);

	void SlowRemoveDither(BYTE * ByteLine[5], INT32 NoiseMatrix[5][5],
						  BYTE * OutputLine, INT32 Width, INT32 Thresh, BOOL Mono);

	KernelBitmap * A;
	KernelBitmap * B;
	KernelBitmap * T;
	
	POINT BPoints[3];
	POINT TPoints[3];
	
	COLORREF BColour;
	DWORD TValue;

	const BYTE * BpTable;
	const BYTE * TpTable;

	DWORD BStyle;
	DWORD TStyle;

	GDrawContext * GC;	
	
	GMATRIX identity;

	BOOL ResetAccusoftDibsToRuns;
#endif
};

#ifndef EXCLUDE_FROM_RALPH
/********************************************************************************************

>	class BfxALULUT : public CCObject

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/1994
	Purpose:	Lookup table

This class implements the Bitmap Effect ALU (Arithmetic and logic unit) LUT (look up table).
This isolates the kernel classes from th storage format of the LUT

********************************************************************************************/

class BfxALU;

class BfxALULUT : public CCObject
{
	CC_DECLARE_DYNCREATE(BfxALULUT);

	friend class BfxALU;

	//BfxALULUT(); constructors and destructors are simple
	//~BFXALULUT();

	public:
	
	BOOL LinearABK(double PropA, double PropB, double Offset);
	BYTE GetLUT(UINT32 A, UINT32 B);

	protected:

	BYTE Data[256*256];
};

#endif	// EXCLUDE_FROM_RALPH
#endif  // INC_BFXALU

