// $Header: /Camelot/winoil/gifutil.h 12    20/05/97 16:18 Neville $

// Header for GIFUtil which suprisingly enough contains lots of
// useful routines for:-
//		reading in a GIF file as a bitmap
//

// Code based on the GD image library code which has the following
// copyright notice:-
/* +-------------------------------------------------------------------+ */
/* | Copyright 1990, 1991, 1993, David Koblas.  (koblas@netcom.com)    | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */
// Code in this file at present is mainly from GIDDecod.c 

#ifndef INC_GIFUTIL
#define	INC_GIFUTIL

#include "ccfile.h"						// needs FilePos
#include "dibconv.h"					// needs DIBConvert
#include "dibutil.h"					// needs FNPTR_SCANLINE
//#include "outptdib.h"	

typedef struct tagGIFINFOHEADER
{
        char      	giName[6];		// GIF87a or 9a
        WORD       	giWidth;
        WORD       	giHeight;
        BYTE       	giFlags;		// Colourmap flag, bpp, resolution
        BYTE       	giBackground;	// background colour
        BYTE       	giAspect;		// aspect
} GIFINFOHEADER, FAR *LPGIFINFOHEADER;

typedef struct tagGIFIMAGEBLOCK
{
        WORD       	gibLeft;
        WORD       	gibTop;
        WORD       	gibWidth;
        WORD       	gibDepth;
        BYTE       	gibFlags;		// Colourmap flag, bpp, resolution
} GIFIMAGEBLOCK, FAR *LPGIFIMAGEBLOCK;

// Flags definitions
#define INTERLACE		0x40
#define LOCALCOLOURMAP	0x80
#define GLOBALCOLOURMAP	0x80

// Extension block type definitions
#define EXTENSIONBLOCK 		0x21
#define TRANSPARENTBLOCK	0xf9

typedef struct tagGIFTRANSBLOCK
{
        BYTE       	gtbBlockStart; 	// 0x21
        BYTE      	gtbIdentifier;	// 0xf9
        BYTE       	gtbBlockSize;	// 4 (always)
        BYTE      	gtbFlags;		// 1 (transparent flag 0x01, wait for key 0x02)
        WORD       	gtbDelay;		// 0 (delay to display for if 0x02 set)
        BYTE       	gtbTransparency;// transparency
        BYTE       	gtbTerminator;	// 0
} GIFTRANSBLOCK, FAR *LPGIFTRANSBLOCK;

#define COMMENTBLOCK 0xfe

typedef struct tagGIFCOMMENTBLOCK
{
        BYTE       	gcbBlockStart; 	// 0x21
        BYTE      	gcbIdentifier;	// 0xfe
        BYTE       	gcbTextSize;	//  size of text, followed by text
} GIFCOMMENTBLOCK, FAR *LPGIFCOMMENTBLOCK;

#define APPLICATIONBLOCK 0xff

typedef struct tagGIFAPPLICATIONBLOCK
{
        BYTE       	gabBlockStart; 	// 0x21
        BYTE      	gabIdentifier;	// 0xff
        BYTE       	gabBlockSize;	// size of block
        CHAR       	gabString[8];	// application block
        BYTE       	gabAuthentication;	// check sum
} GIFAPPLICATIONBLOCK, FAR *LPGIFAPPLICATIONBLOCK;

#define PLAINTEXTBLOCK	0x01

typedef struct tagGIFRGBTRIPLE
{
        BYTE    	grgbtRed;
        BYTE    	grgbtGreen;
        BYTE    	grgbtBlue;
} GIFRGBTRIPLE, FAR *LPGIFRGBTRIPLE;


const unsigned long masks[] = {
								 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
		                         0x001F, 0x003F, 0x007F, 0x00FF, 0x01FF,
		                         0x03FF, 0x07FF, 0x0FFF, 0x1FFF, 0x3FFF,
		                         0x7FFF, 0xFFFF
	                          };

//#define MAXCOLORMAPSIZE	256
#define GIFBITS    		12
#define MAX_LWZ_BITS	12
#define HSIZE  			5003            // 80% occupancy

typedef int			code_int;
typedef long int	count_int;

const int maxbits = GIFBITS;            				// user settable max # bits/code
const code_int maxmaxcode = (code_int)1 << GIFBITS; 	// should NEVER generate this code

const code_int hsize = HSIZE;              		// for dynamic table sizing

enum GIFDisposalMethod
{
	GDM_NONE		= 0,	// No disposal specified. The decoder is not required to take any action.
	GDM_LEAVE		= 1,	// Do not dispose. The graphic is to be left in place.
	GDM_BACKTOBACK	= 2,	// Restore to background color. The area used by the graphic must be restored to the background color.
	GDM_PREVIOUS	= 3		// Restore to previous. The decoder is required to restore the area overwritten by the graphic with what was there prior to rendering the graphic.
};


	
class BaseCamelotFilter;

/********************************************************************************************

>	class Palette : public CC_CLASS_MEMDUMP

	Author:		Colin
	Created:	24/4/95
	Purpose:	Something that should have been done long ago...
	SeeAlso:	Everywhere

********************************************************************************************/
class PaletteIterator
{
public:
	PaletteIterator(): m_pPalette(NULL) {}

	void SetRed(const UINT32 nRed)
	{	m_pPalette->rgbRed = nRed;	}

	void SetGreen(const UINT32 nGreen)
	{	m_pPalette->rgbGreen = nGreen;	}

	void SetBlue(const UINT32 nBlue)
	{	m_pPalette->rgbBlue = nBlue;	}

	void SetTransparent(const UINT32 nTransparent)
	{	m_pPalette->rgbReserved = nTransparent;	}

	BOOL operator< (const PaletteIterator& rhsIterator)
	{	return m_pPalette < rhsIterator.m_pPalette;	}

	PaletteIterator& operator++()
	{
		++m_pPalette;
		return *this;
	}

protected:
	LPRGBQUAD	m_pPalette;
};


/********************************************************************************************

>	class Palette : public CC_CLASS_MEMDUMP

	Author:		Colin
	Created:	24/4/95
	Purpose:	Something that should have been done long ago...
	SeeAlso:	Everywhere

********************************************************************************************/
class Palette : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(Palette);
public:
	Palette() : m_pColours(NULL), m_nSize(0) {}
	~Palette();

//	Palette& operator= (const Palette& otherPalette) {return Palette(otherPalette);}

	BOOL	SetSize(const UINT32 nSize);

	
	BOOL	IsConstructedOK() const
	{	return m_bInitOK;	}


	const PaletteIterator&	Start() const
	{	return m_StartIterator;	}

	const PaletteIterator&	End() const
	{	return m_EndIterator;	}

protected:
	Palette(const Palette& otherPalette);

protected:
	LPRGBQUAD	m_pColours;
	UINT32		m_nSize;

	PaletteIterator		m_StartIterator;
	PaletteIterator		m_EndIterator;

private:
	BOOL		m_bInitOK;
};


/********************************************************************************************

>	class OutputGIF : public OutputDIB

	Author:		Neville Humphrys
	Created:	24/4/95
	Purpose:	Contains functions to read in a GIF file as a DIB.
	SeeAlso:	OutputGIF;

********************************************************************************************/

class GIFUtil //: public CC_CLASS_MEMDUMP
{
public:

	//GIFUtil();
	//~GIFUtil();

	static BOOL ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
							int *TransColour, String_64 *ProgressString = NULL,
							BaseCamelotFilter *pFilter = NULL );
	static BOOL ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
							  int *TransColour, int& nBitmapToRead, String_64 *ProgressString = NULL,
							  BaseCamelotFilter *pFilter = NULL, UINT32* Delay=NULL,
							  GIFDisposalMethod *Restore=NULL,
							  UINT32 * pLeftOffset = NULL, UINT32* pTopOffset = NULL,
							  BOOL * pLocalPalette = NULL);
//	static BOOL WriteToFile ( CCLexFile *, LPBITMAPINFO Info, LPBYTE Bits,
//							  String_64 *ProgressString = NULL);

	// Access to the values read in as part of the GIF header
	static UINT32 GetGlobalWidth() { return m_GlobalWidth; }
	static UINT32 GetGlobalHeight() { return m_GlobalHeight; }

protected:
	BOOL ProcessHeader();
	static BOOL ProcessExtension(CCLexFile *fd);
	static BOOL ProcessImageBlock(CCLexFile* File, LPBITMAPINFO *Info, LPBYTE *Bits,
								  UINT32 * pLeftOffset = NULL, UINT32 * pTopOffset = NULL,
								  BOOL * pLocalPalette = NULL);

	static BOOL ReadColourMap(CCLexFile *fd, int number, LPRGBQUAD lpPalette);
	static int GetDataBlock( CCLexFile *fd, unsigned char  *buf );
	static int GetCode( CCLexFile *fd, int code_size, int flag );
	static int LWZReadByte( CCLexFile *fd, int flag, int input_code_size );
	static BOOL ReadImage( CCLexFile *fd, LPBYTE pBitsData, int width, int height, int bpp,
						   BOOL interlace, BaseCamelotFilter *pFilter = NULL );

private:
	// Some useful variables
	//int Width;							// Width of the image
	//int Height;							// Height of the image
	static WORD m_GlobalWidth;				// The overall width from the GIF header
	static WORD m_GlobalHeight;				// The overall height from the GIF header

	static BOOL Interlace;					// Use interlace or not
	static int Transparent;					// colour or -1 = no transparency
	static UINT32 m_Delay;					// The Animation delay value for each bitmap.
	static GIFDisposalMethod m_Restore;		// The Animation Restore value for exch bitmap.		

	static int	m_nCurrentBitmap;			// the number of the bitmap currently (or was just) in the buffer 

	//WORD BitsPerPixel;				// Colour depth required
	
	//UINT32 WidthOfLine;				// word/byte rounded line width rather than the pixel width

	// Importint specific variables
	static BOOL ZeroDataBlock;		// flag to say whether we have a zero length block or not 

	Palette	m_GlobalPalette;	// the global palette of the GIF

	// replace this with the above...eventually
	static LPRGBQUAD lpGlobalPalette;		// pointer to temporary palette store
	static int GlobalPaletteSize;			// size of the global palette found	 

	static BOOL			m_bImageRead;	// Has an image been found this pass?
	// the start position of the next image in the file
	static FilePos		m_NextImageStartPosition;	
};

#endif // INC_GIFUTIL
