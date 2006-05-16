// $Header: /camelot/winoil/BFXPIXOP.H 3     1/05/95 14:25 Alex $
// This is the header file for the bitmap effect pixop class

/*
*/

#ifndef INC_BFXPIXOP
#define INC_BFXPIXOP

#include "ccobject.h"

class KernelBitmap;

typedef struct
{
	LONG rsum;
	LONG gsum;
	LONG bsum;
	LONG pixels;
	LONG index;
} QuantisationCell;


/********************************************************************************************

>	class BfxPixelOp : public CCObject

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

The tracing stuff uses this class to determine whether or not a pixel should be traced or
not.

********************************************************************************************/

#define __bfxpixop_xy(x,y) Base[(x)+(y)*Width]

class BfxPixelOp : public CCObject
{
	CC_DECLARE_DYNCREATE(BfxPixelOp);

	public:

	BfxPixelOp();			 /* Constructor */
	virtual ~BfxPixelOp();	     /* Destructor */

	virtual BOOL Reset();

	virtual BOOL SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask, BOOL theDefaultValue);
	
	virtual inline BOOL IsInRegion(LONG x, LONG y) {return DefaultValue;}

	virtual inline BOOL ClearCachedArea() {return TRUE;};
	virtual inline BOOL ClearEntireCache() {return TRUE;};
	virtual BOOL SetAuxilliaryBitmaps(KernelBitmap * pProposed /*A*/, KernelBitmap * pCurrent /*B*/,
							  KernelBitmap * pOriginal /*T*/, LONG Threshold, DWORD theColour);

	virtual inline BOOL CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion)
							{*FoundRegion = FALSE; return TRUE;};

	virtual inline DWORD ReadPixel(void * Image, LONG p) { return 0; }
	virtual inline void TranslateToRGB(DWORD Colour, KernelBitmap * pKB, LONG * R, LONG * G, LONG * B)
		{ *R = (LONG)(Colour >>16) & 0x0FF; *G = (LONG)(Colour>>8) & 0x0FF; *B = (LONG)(Colour/*>>0*/) & 0x0FF;return;};

	inline DWORD ReadOriginalPixel(LONG x, LONG y)
		{ return ReadPixel((void *)pT, x+y*WidthRounded); };

	virtual inline DWORD ReadBPixel(LONG x, LONG y)
		{ return ReadPixel((void *)pB, x+y*WidthRounded); };

	inline DWORD ReadPixel(void * Image, LONG x, LONG y) { return ReadPixel(Image, x+y*WidthRounded); };

	virtual inline void WritePixel(void * Image, LONG p, DWORD Value) { return; }
	virtual inline DWORD TranslateToDWORD(KernelBitmap * pKB, LONG R, LONG G, LONG B)
		{ return ((R & 0xFF)<<16) | ((G & 0xFF)<<8) | (B & 0xFF); };

	inline void WriteOriginalPixel(LONG x, LONG y, DWORD Value)
		{ WritePixel((void *)pT, x+y*WidthRounded, Value); return; };

	inline void WritePixel(void * Image, LONG x, LONG y, DWORD Value) { WritePixel(Image, x+y*WidthRounded, Value); return;};

	protected:

	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);

	DWORD * Base;
	LONG Height;
	LONG Width;
	LONG WidthRounded;
	DWORD Size;
	BOOL DefaultValue;

	LONG BPP;

	DWORD * pA;
	DWORD * pB;
	DWORD * pT;
	DWORD Colour;
	LONG Value;

};

/********************************************************************************************

>	class BfxPixelOp32 : public BfxPixelOp

	Author:		Alex
	Created:	12/10/1994
	Purpose:	32 bit Pixel operator

This class caches the state of a pixel in 2 bits of an otherwised top byte of a 32 bit bitmap.

********************************************************************************************/

#define __bfxpixop_xy(x,y) Base[(x)+(y)*Width]

class BfxPixelOp32 : public BfxPixelOp
{
	CC_DECLARE_DYNCREATE(BfxPixelOp32);

	public:

	BfxPixelOp32();			 /* Constructor */
	virtual ~BfxPixelOp32();	     /* Destructor */

	virtual BOOL Reset();

	virtual BOOL SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask, BOOL theDefaultValue);
	
	inline BOOL IsCached(LONG x, LONG y) { return (( (DWORD)x>=(DWORD)Width ) || ( (DWORD)y>=(DWORD)Height ) || (!Base))?
												DefaultValue:((__bfxpixop_xy(x,y) & CacheStateMask)!=0); };
	virtual inline BOOL IsInRegion(LONG x, LONG y) {
		if (( (DWORD)x>=(DWORD)Width ) || ( (DWORD)y>=(DWORD)Height ) || (!Base)) return DefaultValue;
		DWORD *pix = &__bfxpixop_xy(x,y);
		BOOL val;
		return (((*pix) & CacheStateMask)!=0)?(((*pix) & CacheValueMask)!=0):
		(val=GetValueAdjustCache(x,y),(*pix)=((*pix)&~CacheValueMask) | CacheStateMask | (val?CacheValueMask:0),val);
		};
	virtual BOOL ClearCachedArea();
	virtual BOOL ClearEntireCache();

	virtual inline DWORD ReadPixel(void * Image, LONG p)
	{
		if ((((DWORD)p)>=Size)||!Image)
		{
			ERROR3("Out of range BfxPixOp::ReadPixel()");
			return 0;
		}
		return ((DWORD *) Image)[p];
	};

	virtual inline void WritePixel(void * Image, LONG p, DWORD Value)
	{
		if ((((DWORD)p)>=Size)||!Image)
		{
			ERROR3("Out of range BfxPixOp::WritePixel()");
			return;
		}
		((DWORD *) Image)[p]=Value;
		return;
	};

	inline LONG GetCacheMarks() { return CacheMarks; };

	virtual BOOL CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion);

	protected:

	virtual BOOL GetValueAdjustCache(LONG x, LONG y);
	virtual BOOL FloodSearch(LONG x, LONG y, LONG MinimumArea);

	DWORD CacheStateMask;
	DWORD CacheValueMask;
	LONG CacheMarks;

	LONG MinX;
	LONG MinY;
	LONG MaxX;
	LONG MaxY;
};

/********************************************************************************************

>	class BfxPixelOpPseudo : public BfxPixelOp

	Author:		Alex
	Created:	12/10/1994
	Purpose:	32 bit Pixel operator

This pixcel op handles pseudocolour bitmaps on an equality basis

********************************************************************************************/

#define __bfxpixopl2_xy(x,y) (((Base[(x>>Log2BPP)+(y)*WidthRounded]) >> (x & XMask)) & BPPMask)

// this is the maximum minimum area that can be used
#define BFX_PSEUDOFLOODSIZE 256

class BfxPixelOpPseudo : public BfxPixelOp
{
	CC_DECLARE_DYNCREATE(BfxPixelOpPseudo);

	public:

	BfxPixelOpPseudo();			 /* Constructor */
	virtual ~BfxPixelOpPseudo();	     /* Destructor */

	virtual BOOL Reset();

	virtual BOOL SetBitmap(KernelBitmap * pKB, DWORD theCacheStateMask, DWORD theCacheValueMask, BOOL theDefaultValue);
	
	virtual inline BOOL IsInRegion(LONG x, LONG y) {
		if (( (DWORD)x>=(DWORD)Width ) || ( (DWORD)y>=(DWORD)Height ) ) return DefaultValue;
		else return IsPixelReallyInRegion(x, y);};

	virtual void TranslateToRGB(DWORD Colour, KernelBitmap * pKB, LONG * R, LONG * G, LONG * B);

	virtual inline DWORD ReadPixel(void * Image, LONG p)
	{
		if ( (((DWORD)p)>=Size)||!Image)
		{
			ERROR3("Out of range BfxPixOp::ReadPixel()");
			return 0;
		}
		return (DWORD)(((((BYTE *) Image)[p>>XShift])>>((XMask-(p & XMask))<<Log2BPP))&BPPMask);
	};

	virtual inline void WritePixel(void * Image, LONG p, DWORD Value)
	{
		if ( (((DWORD)p)>=Size)||!Image)
		{
			ERROR3("Out of range BfxPixOp::ReadPixel()");
			return;
		}
		BYTE * thebyte = &(((BYTE *) Image)[p>>XShift]);
		LONG shift = ((XMask-(p & XMask))<<Log2BPP);
		*thebyte = (BYTE)(((*thebyte) & ~(BPPMask<<shift)) | ((Value & BPPMask)<<shift));
		return;
	};

	virtual BOOL CheckMinimumArea(LONG MinimumArea, LONG InitialX, LONG InitialY, BOOL * FoundRegion);

	protected:

	BOOL FloodSearch(LONG x, LONG y, LONG MinimumArea,
				  LONG FloodArray[BFX_PSEUDOFLOODSIZE][2], LONG * FloodTop);


	LONG Log2BPP;
	LONG XShift;
	LONG XMask;
	LONG BPPMask;
};

/********************************************************************************************

>	class BfxPixelOp24 : public BfxPixelOp

	Author:		Alex
	Created:	12/10/1994
	Purpose:	32 bit Pixel operator

This pixcel op handles pseudocolour bitmaps on an equality basis

THIS PIXEL OP IS ONLY VERY PARTIALLY IMPLEMENTED - IT ONLY READS B PIXELS

********************************************************************************************/

#define __bfxpixopl2_xy(x,y) (((Base[(x>>Log2BPP)+(y)*WidthRounded]) >> (x & XMask)) & BPPMask)

class BfxPixelOp24 : public BfxPixelOp
{
	CC_DECLARE_DYNCREATE(BfxPixelOp24);

	BfxPixelOp24() : BfxPixelOp() { BPP=24; };

	public:

	virtual inline DWORD ReadBPixel(LONG x, LONG y)
		{ return *((DWORD *)&( ((BYTE *)(pB))[y*(((Width*3)+3)&~3)+(x*3)] ) ) &0x00FFFFFF; };
	
};

/********************************************************************************************

>	class BfxPositivePixelOp : public BfxPixelOp32

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

PixelOp that determines whether a pixel is better by more than a given amount in one bitmap
than in another

********************************************************************************************/

class BfxPositivePixelOp : public BfxPixelOp32
{
	CC_DECLARE_DYNCREATE(BfxPositivePixelOp);

	protected:
	
	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);
	
};

/********************************************************************************************

>	class BfxThresholdPixelOp : public BfxPixelOp32

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

PixelOp that determines whether a pixel is better by more than a given amount in one bitmap
than in another

********************************************************************************************/

class BfxThresholdPixelOp : public BfxPixelOp32
{
	CC_DECLARE_DYNCREATE(BfxThresholdPixelOp);

	protected:

	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);

};

/********************************************************************************************

>	class BfxColourThresholdPixelOp : public BfxPixelOp32

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

PixelOp that determines whether a pixel is better by more than a given amount in one bitmap
than in another

********************************************************************************************/

class BfxColourThresholdPixelOp : public BfxPixelOp32
{
	CC_DECLARE_DYNCREATE(BfxColourThresholdPixelOp);

	protected:

	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);

};

/********************************************************************************************

>	class BfxPositivePixelOpPseudo : public BfxPixelOpPseudo

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

PixelOp that determines whether a pixel is better by more than a given amount in one bitmap
than in another

********************************************************************************************/

class BfxPositivePixelOpPseudo : public BfxPixelOpPseudo
{
	CC_DECLARE_DYNCREATE(BfxPositivePixelOpPseudo);

	protected:
	
	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);
	
};

/********************************************************************************************

>	class BfxThresholdPixelOpPseudo : public BfxPixelOpPseudo

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

PixelOp that determines whether a pixel is better by more than a given amount in one bitmap
than in another

********************************************************************************************/

class BfxThresholdPixelOpPseudo : public BfxPixelOpPseudo
{
	CC_DECLARE_DYNCREATE(BfxThresholdPixelOpPseudo);

	protected:

	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);

};

/********************************************************************************************

>	class BfxColourThresholdPixelOpPseudo : public BfxPixelOpPseudo

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Pixel operator

PixelOp that determines whether a pixel is better by more than a given amount in one bitmap
than in another

********************************************************************************************/

class BfxColourThresholdPixelOpPseudo : public BfxPixelOpPseudo
{
	CC_DECLARE_DYNCREATE(BfxColourThresholdPixelOpPseudo);

	protected:

	virtual BOOL IsPixelReallyInRegion(LONG x, LONG y);

};

/********************************************************************************************

>	class Octree : public CCObject

	Author:		Alex
	Created:	12/10/1994
	Purpose:	Colour octree

********************************************************************************************/

class OctreeElement : public SimpleCCObject
{
	public:

	OctreeElement() { pParent=pListNext=pListPrev=NULL; for (LONG c=0; c<8; c++) pChildren[c]=NULL;
					  Pixels=Depth=R=G=B=NumChildren=0;Index=-1;};
	~OctreeElement() {
			for (LONG c=0; c<8; c++) if (pChildren[c])
			{
				delete (pChildren[c]);
				pChildren[c]=NULL;
			}
			pParent=pListNext=pListPrev=NULL;Pixels=Depth=R=G=B=NumChildren=0;Index=-1;
		};

	LONG Depth;
	LONG R;
	LONG G;
	LONG B;
	LONG Index;
	LONG Pixels;
	LONG NumChildren;
	OctreeElement * pChildren[8];
	OctreeElement * pParent;
	OctreeElement * pListNext;
	OctreeElement * pListPrev;
};

class Octree : public CCObject
{
	CC_DECLARE_DYNCREATE(Octree);

	public:

	Octree();			 /* Constructor */
	~Octree();	     /* Destructor */

	BOOL Init(LONG NumColours);
	BOOL Insert(LONG r, LONG g, LONG b);
	BOOL Index(RGBQUAD * Palette, OctreeElement * pEl=NULL);
	LONG GetIndex(LONG r, LONG g, LONG b);

	private:

	OctreeElement * GetElement(LONG r, LONG g, LONG b);
	void CheckIntegrity();
	void Unlink(OctreeElement * pEl);
	void Link(OctreeElement * pEl);

	LONG MaxLeaves;
	LONG CurrentLeaves;
	OctreeElement * pTopElement;
	LONG CurrentIndex;

	OctreeElement * ListHead[9 /*depth 0-8*/ ][9 /*children 0-8*/ ];

};




#endif

