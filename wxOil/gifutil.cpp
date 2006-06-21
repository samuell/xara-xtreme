// $Header: /Camelot/winoil/gifutil.cpp 19    25/03/99 16:16 Markn $

// Contains useful routines for compressing a bitmap out to a GIF format file and
// routines to load that file back in.

/*
*/

#include "camtypes.h"
#include "ensure.h"
#include "fixmem.h"
#include "errors.h"
#include "progress.h"		// For hourglass stuff
//#include "resource.h"		// IDS_OUTOFMEMORY
//#include "accures.h"		// IDW_CANCELLEDBMPIMPORT
#include "gifutil.h"
//#include "outptgif.h"
//#include "andy.h"
//#include "dibconv.h"
//#include "string.h"			// memcpy
#include "camfiltr.h"		// BaseCamelotFilter

#define	new	CAM_DEBUG_NEW


CC_IMPLEMENT_MEMDUMP(Palette, CC_CLASS_MEMDUMP)
Palette::~Palette()
{
	if (m_pColours)
		delete [] m_pColours;
	m_pColours = NULL;			// ensure state correct
	m_nSize = 0;
}



Palette::Palette(const Palette& otherPalette)
{
	if (&otherPalette == this) return;

	if (otherPalette.m_nSize > m_nSize)
		delete [] m_pColours;
		m_pColours = new RGBQUAD[otherPalette.m_nSize];
		if (m_pColours == NULL)
		{
			m_bInitOK = FALSE;
			return;
		}
	m_nSize = otherPalette.m_nSize;
	memcpy(m_pColours, otherPalette.m_pColours, m_nSize);
	m_bInitOK = TRUE;
}

#define BitSet(byte, bit)      (((byte) & (bit)) == (bit))

// define the statics that we require
BOOL 	GIFUtil::Interlace		= FALSE;	// Use interlace or not
int		GIFUtil::Transparent	= -1;		// colour or -1 = no transparency
UINT32	GIFUtil::m_Delay		= 0;		// Data member, to recieve a bitmaps Animation delay value
BOOL 	GIFUtil::ZeroDataBlock	= FALSE;	// flag to say whether we have a zero length block or not 
int		GIFUtil::m_nCurrentBitmap = 0;
GIFDisposalMethod GIFUtil::m_Restore = GDM_LEAVE;	// Data member, to recieve a bitmaps' Animation  Restore value.

LPRGBQUAD	GIFUtil::lpGlobalPalette = NULL;		// pointer to temporary palette store
int			GIFUtil::GlobalPaletteSize = 0;			// size of the global palette found	 
FilePos		GIFUtil::m_NextImageStartPosition = 0;	// Position in file of next image 
BOOL		GIFUtil::m_bImageRead = FALSE;			// Did we get something?

WORD		GIFUtil::m_GlobalWidth	= 0;			// The overall width from the GIF header
WORD		GIFUtil::m_GlobalHeight	= 0;			// The overall height from the GIF header

/********************************************************************************************

>	GIFUtil::GIFUtil()

	Author:		Neville Humphrys
	Created:	29/6/95
	Purpose:	Default constructor for the class. 
	SeeAlso:	

********************************************************************************************/

//GIFUtil::GIFUtil()
//{
//}

/********************************************************************************************
>	GIFUtil::GIFUtil()

	Author:		Neville Humphrys
	Created:	29/6/95
	Purpose:	Default destructor for the class.
	SeeAlso:	

********************************************************************************************/

//GIFUtil::~GIFUtil()
//{
//}

/********************************************************************************************

>	static BOOL GIFUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
									   int *TransColour, int& nBitmapToRead, 
									   String_64 *ProgressString, BaseCamelotFilter *pFilter = NULL  )

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		File			A opened CCLexFile that can be read from. It should be positioned at the
								start. Caller is responsible for closing it. The file needs to be in
								Binary mode.
				nBitmapToRead	The number of the bitmap in the GIF that you wish to read.
								If 1 then the GIF will be read from the beginning obtaining the
								header information.
								For any number other than 1 it should be noted that bitmaps can only
								be read in an incremental sequence and that the first one must have
								been read prior to any call with a non-1 value.
				ProgressString	allows the user to specify whether they require a progress hourglass or 
								not. If NULL then none is shown, otherwise an progress bar is shown
								using the text supplied. Defaults to NULL i.e. no progress bar.
				pFilter			is an alternative way of handling the progress bar, assume the
								progress bar has been start and just call the IncProgressBarCount in 
								BaseCamelotFilter to do the progress bar update.
								Defaults to NULL i.e. no progress bar.
	Outputs:	Info points to a new LPBITMAPINFO struct and Bits points to the bytes.
				These can be freed up with FreeDIB.
				TransColour is either -1 == none or equal to the transparency colour found
				nBitmapToRead	Returns the number of the next bitmap that can be read from the GIF
								or -1 if no further bitmaps exist.
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Reads a .gif file into memory decompressing it as it goes.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static, Public
	SeeAlso:	DIBUtil::ReadFromFile; AccusoftFilters::ReadFromFile;

********************************************************************************************/
BOOL GIFUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
							int *TransColour, String_64 *ProgressString,
							BaseCamelotFilter *pFilter )
{
	int nBitmap = 1;

	return (ReadFromFile( File, Info, Bits, TransColour, nBitmap, ProgressString, pFilter));
}

/**********************************************************************************************

static	BOOL GIFUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
								    int *TransColour, int& nBitmapToRead, String_64 *ProgressString = NULL,
								    BaseCamelotFilter *pFilter = NULL, UINT32* Delay=NULL,
								    GIFDisposalMethod *Restore=NULL,
								    UINT32 * pLeftOffset = NULL, UINT32 * pTopOffset = NULL,
								    BOOL * pLocalPalette = NULL)
Author:			-
Created:		-
Inputs/Outputs	Refer to the above Function Header for a full explanation. 

*************************************************************************************************/

BOOL GIFUtil::ReadFromFile( CCLexFile *File, LPBITMAPINFO *Info, LPBYTE *Bits,
							int *TransColour, int& nBitmapToRead, String_64 *ProgressString,
							BaseCamelotFilter *pFilter, UINT32* Delay, GIFDisposalMethod *Restore,
							UINT32 * pLeftOffset, UINT32 * pTopOffset,
							BOOL * pLocalPalette)
{
	if ((nBitmapToRead > 1 && nBitmapToRead == m_nCurrentBitmap) || nBitmapToRead < 1)
	{
		ERROR3("GIFUtil::ReadFromFile() - can't read that bitmap");
		return FALSE;
	}

	*Info			= NULL;		// in case of early exit
	*Bits			= NULL;
	Transparent 	= -1;
	*TransColour 	= -1;		// in case of early exit set to none
	int Background 	= 0;		// background colour number !!! not used !!!
	Interlace 		= FALSE;	// set interlace to false by default

	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCLexFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	BOOL OldThrowingState = File->SetThrowExceptions( TRUE );
	BOOL OldReportingState = File->SetReportErrors( FALSE );

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL)
		BeginSlowJob(100, FALSE, ProgressString);

	try
	{
		// If we want the first bitmap we'll assume we're at the start of the file & read the header
		if (nBitmapToRead == 1)
		{
			// place to store the global palette, if present, for later use
			lpGlobalPalette 		= NULL;	// pointer to temporary palette store
			GlobalPaletteSize 			= 0;	// size of the global palette found	 

			GIFINFOHEADER Header;
			// This is really sizeof(GIFINFOHEADER) but this returns 14 instead of 13
			// as it rounds to the nearest word boundary
			const size_t HeaderSize = sizeof(char)* 6 + sizeof(WORD) * 2 + sizeof(BYTE) * 3;

			File->read( &Header, HeaderSize );
			if (File->bad())
				File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

			// Just double check that the signature is correct, if not then fail immediately
			if (
				(strncmp(Header.giName, "GIF89a", 6) != 0) &&
	 			(strncmp(Header.giName, "GIF87a", 6) != 0)
			)
				File->GotError( _R(IDE_BADFORMAT) );

			// Note the overall size of the GIF from the header, may be the animation size
			m_GlobalWidth		= Header.giWidth;
			m_GlobalHeight  	= Header.giHeight;
			// flags word consists of:-
			// - bit 7 colour table flag				= set if global colour table follows
			// - bits 6-4 colour resolution				= bpps - 1
			// - bit 3 sort flag						= set global colour table sorted
			// - bits 3-0 size of global colour table 	= size is 2^(value+1)
			int ColorResolution = (((Header.giFlags >> 4) & 0x07) + 1);
			GlobalPaletteSize	= 2 << (Header.giFlags & 0x07);
			Background			= Header.giBackground;
			int AspectRatio		= Header.giAspect;

TRACEUSER("Neville",_T("Gif Global Width = %d Height = %d\n"), m_GlobalWidth, m_GlobalHeight);
TRACEUSER("Neville",_T("Gif ColorResolution = %d\n"), ColorResolution);

			// Check if we have a global colour map present or not, if so then read it in.
			if (BitSet(Header.giFlags, GLOBALCOLOURMAP))
			{
TRACEUSER("Neville",_T("Gif read global colour table size = %d\n"), GlobalPaletteSize);
			    // Read in the global colour map into a palette structure for possible later use
				const size_t TotalPal = sizeof(RGBQUAD) * GlobalPaletteSize;
				lpGlobalPalette = (LPRGBQUAD)CCMalloc( TotalPal );
				if (lpGlobalPalette==NULL)
					return FALSE;

				ReadColourMap(File, GlobalPaletteSize, lpGlobalPalette);
			}
			m_nCurrentBitmap = 0;
			m_NextImageStartPosition = 0;
		}

		// We now need to go through and process the data in this file which
		// should now consist off GIF blocks until we hit the end of file or
		// we have processed the lot.
		BOOL FileProcessed = FALSE;

		m_bImageRead = FALSE;
		unsigned char c;
		// Set up bad values so that the DIB alloc claim will fail if there
		// has been no GIFIMAGEBLOCK found
//		int	Width 		  	= 0;	// Width of bitmap	
//		int Height 		  	= 0;	// Height of bitmap	
//		int BitsPerPixel	= 0;	//ColorResolution;	// Colour depth required

		if (m_NextImageStartPosition != 0)
		{
			File->seekIn(m_NextImageStartPosition, ios::beg);
			m_NextImageStartPosition = 0;
		}

		while (!File->eof() && !File->bad() && !FileProcessed)
		{
			// Get the next character in the stream and see what it indicates
			// comes next in the file
			// Use Get as this will read the EOF character (255) and exit via the
			// File->eof() test rather than Read which will throw an exception. 
			//File->read( &c, 1 );
			File->get( (char&)c );
			
			switch (c)
			{
				case ';':
				{
					// GIF terminator
					// Cannot assume that the terminator immediately follows the image data
					// Might have some extensions following the image data. 					 
					FileProcessed = TRUE;
					nBitmapToRead = -1;
					break;
				}

				case '!': //EXTENSIONBLOCK:
				{
					// Extension
					ProcessExtension(File);
					break;
				}
				
				case ',':
				{
					// start of image character
					if (m_bImageRead)	// another bitmap - save it for the next call to ReadFromFile()
					{
						if (m_NextImageStartPosition == 0)
						{
							// No transparency info for the next one so start here
							m_NextImageStartPosition = File->tellIn() - 1;
						}
						++nBitmapToRead;
						FileProcessed = TRUE;
						break;
					}

					UINT32 LeftOffset = 0;
					UINT32 TopOffset = 0;
					BOOL LocalPalette = FALSE;

					// Should be followed by a GIFIMAGE BLOCK
					ProcessImageBlock(File, Info, Bits, &LeftOffset, &TopOffset, &LocalPalette);
					++m_nCurrentBitmap;

					if (nBitmapToRead == m_nCurrentBitmap)
					{
						m_bImageRead = TRUE;
					}

					// return the values back to the caller if it desired them
					if (pLeftOffset)
						*pLeftOffset = LeftOffset;
					if (pTopOffset)
						*pTopOffset = TopOffset;
					if (pLocalPalette)
						*pLocalPalette = LocalPalette;
					break;
				}

				default:
					// We have found something other than what we are expecting
					// so fail with an error
					//File->GotError( IDE_BADFORMAT );
					// Cannot do this as some files have random bits
					// e.g. galileo.gif has a random zero before the ;
					//     	colsec.gif has a rampant > at the end instead of ;
					if (m_bImageRead)
					{
						// We've already got something so ignore anything else in case it goes
						// completely wrong.
						FileProcessed = TRUE;
						nBitmapToRead = -1;
					}
					TRACE( _T("Unrecognized Character %x at %d\n"), (int)c, File->tellIn());

					break;
			}
		}

		// If we reach here and the bitmap allocations are still null then no valid image
		// was found and so we should error now.
		// Might have just been a GIF file with extension tags in and no images!
		if (*Info == NULL || *Bits == NULL)
			File->GotError( _R(IDE_BADFORMAT) );

		// We read the desired bitmap but the EOF came along before we could try anything else
		// Signal this is the last bitmap
		if (m_bImageRead && File->eof())
		{
			TRACE( _T("GIF: Premature end of file") );
			nBitmapToRead = -1;
		}

		// Return the transparency/delay/restore found to the caller.
		*TransColour = Transparent;
		if (Delay)
		{
			*Delay = m_Delay;
		}
		if (Restore)
		{
			*Restore = m_Restore;
		}

		// Free up the bit of memory for a palette we grabbed, if present & no more images to read
		if (lpGlobalPalette && nBitmapToRead == -1)
		{
			CCFree(lpGlobalPalette);
			lpGlobalPalette = NULL;
		}

		// If started, then stop then progress bar
		if (ProgressString != NULL)
			EndSlowJob();

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		// er, we seem to have finished OK so say so
		return TRUE;
	}
	catch( CFileException e )
	{
		// catch our form of a file exception
		TRACE( _T("GIFUtil::ReadFromFile CC catch handler\n") );

		FreeDIB( *Info, *Bits );							// free any alloced memory
		*Info = NULL;										// and NULL the pointers
		*Bits = NULL;
		
		// Free up the bit of memory for a palette we grabbed, if present
		if (lpGlobalPalette)
		{
			CCFree(lpGlobalPalette);
			lpGlobalPalette = NULL;
		}

		// If started, then stop then progress bar
		if (ProgressString != NULL)
			EndSlowJob();

		// Must set the exception throwing and reporting flags back to their entry states
		File->SetThrowExceptions( OldThrowingState );
		File->SetReportErrors( OldReportingState );

		return FALSE;
	}

	ERROR2( FALSE, "Escaped exception clause somehow" );
}


/********************************************************************************************

>	static BOOL GIFUtil::ProcessExtension(CCLexFile *fd)

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		fd			pointer to a CCLexFile to read the data from
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	
	SeeAlso:	

********************************************************************************************/

BOOL GIFUtil::ProcessExtension(CCLexFile *fd)
{
	static char buf[256];

	// Get the type of this extension and then process it
	unsigned char ExtensionType;
	fd->read( &ExtensionType, 1 );

	switch (ExtensionType)
	{
		case TRANSPARENTBLOCK:
		{
			// The next image in the file might need to start here to get transparency
			// information. So remember the position
			m_NextImageStartPosition = fd->tellIn() - 2;

			// Graphic Control Extension
			// We have read in the identfier and the type of extension block so we
			// really need to read the data from that point onwards
			GIFTRANSBLOCK TransBlock;

			// This is really sizeof(GIFTRANSBLOCK) but this returns 14 instead of 13
			// as it rounds to the nearest word boundary
			const size_t TransBlockSize = sizeof(WORD) * 1 + sizeof(BYTE) * 6;
			fd->read( &(TransBlock.gtbBlockSize), TransBlockSize - 2);
			
			// Contains lots of random rubbish we are not interested in.			
			// Just skip to the crux of the matter and read in the transparency colour,
			// if the transparency flag is set.

			if (!m_bImageRead)
			{
				BOOL fTrans = TransBlock.gtbFlags & 0x1;  
				if (fTrans != 0)
					Transparent = TransBlock.gtbTransparency;
				else
				{
					Transparent = -1; // no transparency	
				}

				TRACEUSER("Neville", _T("ProcessExtension - transparent fTrans = %d Transparent = %d\n"), fTrans, Transparent );

				// Animation Restore\Delay values.
				m_Delay = TransBlock.gtbDelay;
				m_Restore = GIFDisposalMethod((TransBlock.gtbFlags >> 2) & 0x03);
			}

			return TRUE;
		}

		case PLAINTEXTBLOCK:
		{
			// We need to bin any transparency information we have since
			// "The scope of this [Graphic Control (TRANSPARENTBLOCK)] Extension is the 
			// graphic rendering block that follows it"

			if (m_bImageRead)
			{
				// We've read the image but don't need to start with any TRANSPARENT block
				// that might have been read after the image, so make sure we don't
				m_NextImageStartPosition = 0;
			}
			else
			{
				// We haven't read any image yet so just make sure no transparency
				Transparent = -1;
			}
			// ...and drop through
		}
		
		default:
			break;
	}

	// Must set this as otherwise, if this is after we have loaded an image then we have
	// the possibility that the zero block has been encountered and GetDataBlock will
	// always return 0, so force it not to. 
	ZeroDataBlock = FALSE;

	// If not recognised then read the rest of that extension
	while (GetDataBlock(fd, (unsigned char*) buf) != 0)
		;

	return FALSE;
}

/********************************************************************************************

>	static BOOL GIFUtil::ReadColourMap(CCLexFile *fd, int number, LPRGBQUAD lpPalette)

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		fd			pointer to a CCLexFile to read the data from
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	
	SeeAlso:	

********************************************************************************************/

BOOL GIFUtil::ReadColourMap(CCLexFile *fd, int number, LPRGBQUAD lpPalette)
{
	GIFRGBTRIPLE rgb;
	for (int i = 0; i < number; ++i)
	{
		fd->read( &rgb, sizeof(GIFRGBTRIPLE) );
		lpPalette->rgbBlue = rgb.grgbtBlue;
		lpPalette->rgbGreen = rgb.grgbtGreen;
		lpPalette->rgbRed = rgb.grgbtRed;
		lpPalette->rgbReserved = 0;
		lpPalette++;
	}
	
	return TRUE;
}



/********************************************************************************************

>	static int GIFUtil::GetDataBlock(CCLexFile *fd, unsigned char *buf)

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		fd			pointer to a CCLexFile to read the data from
	Outputs:	-
	Returns:	Number of bytes read
	Purpose:	To read in a block of data from the specifed file. Reads a 1 byte count
				and then reads that many bytes in, as long as the count is non-zero.
				Any non-image calls must set ZeroDataBlock to False before calling this as
				otherwise it may just return zero. 
	SeeAlso:	

********************************************************************************************/

int GIFUtil::GetDataBlock(CCLexFile *fd, unsigned char *buf)
{
	unsigned char count;

	// Added, as if we encounter the zero block then that is the end of this image
	// and we should never read any more info from that image. (added 14/8/95)
	if (ZeroDataBlock)
		return 0;
	
	fd->read( &count, 1 );

	// If there is a count then read that many bytes in
	// If zero then this is a terminator so return that
	if (count == 0)
	{
		ZeroDataBlock = TRUE;
		// Fall through to return 0
	}
	else
	{
		ZeroDataBlock = FALSE;
		fd->read( buf, count );
		// fall through to return the number of bytes found
	}

	return count;
}

/********************************************************************************************

>	static int GIFUtil::GetCode(CCLexFile *fd, int code_size, int flag)

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		fd			pointer to a CCLexFile to read the data from
				code_size
				flag			
	Outputs:	-
	Returns:	The code found.
	Purpose:	Returns the next code in the stream.
	SeeAlso:	

********************************************************************************************/

int GIFUtil::GetCode(CCLexFile *fd, int code_size, int flag)
{
	static unsigned char	buf[280];
	static int	curbit;
	static int	lastbit;
	static int	done;
	static int	last_byte;
	int	i;
	int	j;
	int	ret;
	unsigned char	count;

	if (flag)
	{
		curbit = 0;
		lastbit = 0;
		done = FALSE;
		return 0;
	}

	if ( (curbit+code_size) >= lastbit)
	{
		if (done)
		{
			if (curbit >= lastbit)
			{
				/* Oh well */
			}                        
			return -1;
		}
		buf[0] = buf[last_byte-2];
		buf[1] = buf[last_byte-1];

		if ((count = GetDataBlock(fd, &buf[2])) == 0)
			done = TRUE;

		last_byte = 2 + count;
		curbit = (curbit - lastbit) + 16;
		lastbit = (2+count)*8 ;
	}

	ret = 0;
	for (i = curbit, j = 0; j < code_size; ++i, ++j)
		ret |= ((buf[ i / 8 ] & (1 << (i % 8))) != 0) << j;

	curbit += code_size;

	return ret;
}


/********************************************************************************************

>	static int GIFUtil::LWZReadByte(CCLexFile *fd, int flag, int input_code_size)

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		fd				pointer to a CCLexFile to read the data from
				flag
				input_code_size	
	Outputs:	-
	Returns:	The next LZW byte in the stream. 
	Purpose:	To read the next LZW byte from the stream.
	SeeAlso:	

********************************************************************************************/

int GIFUtil::LWZReadByte(CCLexFile *fd, int flag, int input_code_size)
{
	static int	fresh = FALSE;
	int			code;
	int			incode;
	static int	code_size;
	static int	set_code_size;
	static int	max_code;
	static int	max_code_size;
	static int	firstcode;
	static int	oldcode;
	static int	clear_code;
	static int	end_code;
	static int	table[2][(1<< MAX_LWZ_BITS)];
	static int	stack[(1<<(MAX_LWZ_BITS))*2];
	static int	*sp;
	int	i;

	if (flag)
	{
		set_code_size = input_code_size;
		code_size = set_code_size+1;
		clear_code = 1 << set_code_size ;
		end_code = clear_code + 1;
		max_code_size = 2*clear_code;
		max_code = clear_code+2;

		GetCode(fd, 0, TRUE);

		fresh = TRUE;

		for (i = 0; i < clear_code; ++i)
		{
			table[0][i] = 0;
			table[1][i] = i;
		}
		for (; i < (1<<MAX_LWZ_BITS); ++i)
			table[0][i] = table[1][0] = 0;

		sp = stack;

		return 0;
	}
	else if (fresh)
	{
		fresh = FALSE;
		do
		{
			firstcode = oldcode = GetCode(fd, code_size, FALSE);
		} while (firstcode == clear_code);

		return firstcode;
	}

	if (sp > stack)
		return *--sp;

	while ((code = GetCode(fd, code_size, FALSE)) >= 0)
	{
		if (code == clear_code)
		{
			for (i = 0; i < clear_code; ++i)
			{
			       table[0][i] = 0;
			       table[1][i] = i;
			}

			for (; i < (1<<MAX_LWZ_BITS); ++i)
			       table[0][i] = table[1][i] = 0;

			code_size = set_code_size+1;
			max_code_size = 2*clear_code;
			max_code = clear_code+2;
			sp = stack;
			firstcode = oldcode = GetCode(fd, code_size, FALSE);

			return firstcode;
		}
		else if (code == end_code)
		{
			int             count;
			unsigned char   buf[260];

			if (ZeroDataBlock)
				return -2;

			while ((count = GetDataBlock(fd, buf)) > 0)
				;

			if (count != 0)
				return -2;
		}

		incode = code;

		if (code >= max_code)
		{
			*sp++ = firstcode;
			code = oldcode;
		}

		while (code >= clear_code)
		{
			*sp++ = table[1][code];
			if (code == table[0][code])
			{
				/* Oh well */
			}
			code = table[0][code];
		}

		*sp++ = firstcode = table[1][code];

		if ((code = max_code) <(1<<MAX_LWZ_BITS))
		{
			table[0][code] = oldcode;
			table[1][code] = firstcode;
			++max_code;
			if ((max_code >= max_code_size) && (max_code_size < (1<<MAX_LWZ_BITS)))
			{
				max_code_size *= 2;
				++code_size;
			}
		}

		oldcode = incode;

		if (sp > stack)
			return *--sp;
	}
	return code;
}

/********************************************************************************************

>	static BOOL GIFUtil::ReadImage(CCLexFile *fd, LPBYTE pBitsData, int width, int height,
								   BOOL interlace, BaseCamelotFilter *pFilter = NULL )

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		fd			pointer to a CCLexFile to read the data from
				pBitsData	pointer to the place to put the bitmap data, should have been
							claimed so that it is the correct size to contain it.
				width		width of the bitmap in pixels
				height		height of the bitmap in pixels
				bpp			colour depth of the bitmap in bits per pixel
				interlace	flag to say if this data is interlaced or not.
				pFilter		is an alternative way of handling the progress bar, assume the
							progress bar has been start and just call the IncProgressBarCount in 
							BaseCamelotFilter to do the progress bar update.
							Defaults to NULL i.e. no progress bar.
	Purpose:	The read the bitmap data into the specified buffer. It decompresses the
				data from the specified file and then puts the pixel data into the buffer.
				Copes with interlaced GIFs.
				Assumes progress hourglass has been started with 100.
	SeeAlso:	GIFUtil::ReadFromFile;

********************************************************************************************/

BOOL GIFUtil::ReadImage(CCLexFile *fd, LPBYTE pBitsData, int width, int height, int bpp,
						BOOL interlace, BaseCamelotFilter *pFilter)
{
	ERROR2IF(pBitsData == NULL, FALSE, "GIFUtil::ReadImage given null pointer to bits data");
	ERROR2IF(bpp != 8 && bpp != 4 && bpp != 1, FALSE, "GIFUtil::ReadImage - bpp invalid");

	unsigned char c;      
	int v = 0;
	int	xpos = 0;
	int	ypos = 0;
	int	pass = 0;
	int yposcount = 0;

	ZeroDataBlock = FALSE;
	
	// Work out how often we need to update the progress bar
	int UpdateEvery = 1;
	INT32 UpdateValue = 1;
	if (pFilter == NULL)
		UpdateEvery = height/100 + 1;
	else
	{
		// Ask the filter what the record size for this bitmap is and hence
		// what the allocation we have for progress bar updates are.
		// We will then have to update our progress bar by
		//	current scanline/total number of scanlines * allocation
		// so that we get update by a proportion of the value.
		// We can assume no interlacing as in native/web files this is turned off.
		INT32 RecordSize = pFilter->GetCurrentRecordSize();
		if (RecordSize == 0)
			RecordSize = 1;
		UpdateValue = RecordSize/height;
		UpdateEvery = 0;	// So it updates every time round the loop
	}
		
	int LastProgressUpdate = 0;
	
	// Work out the word/byte rounded line width rather than the pixel width
	INT32 WidthOfLine = DIBUtil::ScanlineSize( width, bpp );

	// Initialize the Compression routines
	fd->read( &c, 1 );
	ERROR3IF(c > GIFBITS,"Bad code size in GIFUtil ReadImage");
	
	if (LWZReadByte(fd, TRUE, c) < 0)
	{
		return FALSE;
	}

	while ((v = LWZReadByte(fd, FALSE, c)) >= 0 )
	{
		//GIFSetPixel(pBitsData, xpos, ypos, v);
		// Check that the specified pixel is within the bounds specified and that we
		// have a buffer to write to

//		if ( !(((ypos < 0) || (ypos >= height)) || ((xpos < 0) || (xpos >= width))) && pBitsData )
//		{
			// Our DIBs are the wrong way up so we must output the data from the last 
			// line of the image and go up to the start
			// -1 as height = 1 .. Height whereas y goes from 0 .. Height - 1
			// Use the word/byte rounded line width rather than the pixel width
			if (bpp == 8)
			{
				// If 8 bpp then just use the whole byte straight
				LPBYTE pData = pBitsData + xpos + ((height - 1 - ypos)  * WidthOfLine);
				*(pData) = v;
			}
			else if (bpp == 4)
			{
				// 4bpp so we must put the data into either the high or low nibble.
				// This will be dependent on whether we are on an odd or even pixel.
				// So test the LSBit of the curx, if set we will be odd.
				// Only move onto next byte every other pixel hence curx/2.
				LPBYTE pData = pBitsData + xpos/2 + ((height - 1 - ypos)  * WidthOfLine);
				// Get whole present byte 
				if (xpos & 1)
					*(pData) = ((*(pData)) & 0xF0) | (v & 0x0F);	// add into low nibble 
				else
					*(pData) = ((*(pData)) & 0x0F) | ((v << 4) & 0xF0);	// add into top nibble
			}
			else if (bpp == 1)
			{
				// 1bpp so we must put the data into either the high or low nibble.
				// This will be dependent on whether we are on an odd or even pixel.
				// So test the LSBit of the curx, if set we will be odd.
				// Only move onto next byte every other pixel hence curx/2.
				LPBYTE pOutputBitmap = pBitsData + (xpos / 8) + ((height - 1 - ypos)  * WidthOfLine);
				// Get whole present byte
				UINT32 BitPosition = 7 - (xpos % 8);
				BYTE SetBit = v << BitPosition;
				BYTE Mask = ~(1 << BitPosition);

				*pOutputBitmap = ((*pOutputBitmap) & Mask) | SetBit;
			}
			else
			{
				ERROR3IF(TRUE, "GIFUtil::ReadImage() - Invalid bpp");
				// bad bpp but should not get here so do nothing
			}
			
//		}
		
		++xpos;
		
		BOOL JobState = TRUE;
		// if we have reached the end of the current line then work out the next
		// line that we must do, if interlacing is on 
		if (xpos >= width)
		{
			xpos = 0;
			
			// Do different filing methods depending on whether interlaced on non-interlaced.
			if (interlace)
			{
				// We are interlaced so learn not to count properly!
				switch (pass)
				{
					case 0:
					case 1:
						ypos += 8; break;
					case 2:
						ypos += 4; break;
					case 3:
 						ypos += 2; break;
				}

				while (ypos >= height && pass <= 3)
				{
					++pass;
					switch (pass)
					{
						case 1:
							ypos = 4;
							break;
						case 2:
							ypos = 2;
							break;
						case 3:
							ypos = 1;
							break;
						default:
							goto finish;
					}
				}
			}
			else
			{
				// Non-interlaced case
				++ypos;
			}

			yposcount++;
			
			if (yposcount > (LastProgressUpdate + UpdateEvery))
			{
				// Note the update point so that we know the next one  
				LastProgressUpdate = yposcount;

				// Now update the progress display, started with 100
				if (pFilter == NULL)
					JobState = ContinueSlowJob( (long)(100 * yposcount/height) );
				else
					JobState = pFilter->IncProgressBarCount(UpdateValue);

				// If JobState is False then the user has probably pressed escape and we should
				// immediately stop what we are doing. 
				if (!JobState)
				{
					fd->GotError( _R(IDW_CANCELLEDBMPIMPORT) );	// Expects error set on cancel
					return FALSE;
				}
			}
		}

		if (ypos >= height)
			break;
	}

finish:
	if (LWZReadByte(fd, FALSE, c) >= 0)
	{
		/* Ignore extra */
	}

	// Everything seemed to go ok 
	return TRUE;
}


/********************************************************************************************

>	static void GIFUtil::GIFSetPixel(LPBYTE pBitsData, int x, int y, int colour)

	Author:		Neville Humphrys
	Created:	29/6/95
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	SeeAlso:	

********************************************************************************************/

//void GIFUtil::GIFSetPixel(LPBYTE pBitsData, int x, int y, int colour)
//{
//	// Check that the specified pixel is within the bounds specified and that we
//	// have a buffer to write to
//	if ( !(((y < 0) || (y >= Height)) || ((x < 0) || (x >= Width))) && pBitsData )
//	{
//		// Our DIBs are the wrong way up so we must output the data from the last 
//		// line of the image and go up to the start
//		// -1 as height = 1 .. Height whereas y goes from 0 .. Height - 1
//		// Use the word/byte rounded line width rather than the pixel width
//		LPBYTE pData = pBitsData + x + ((Height - 1 - y)  * WidthOfLine);
//		*(pData) = colour;
//	}
//}

/********************************************************************************************

>	static BOOL ReadColourMap(CCLexFile *fd, Palette& NewPalette)

	Author:		Colin
	Created:	29/6/95
	Inputs:		fd			pointer to a CCLexFile to read the data from
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	
	SeeAlso:	

********************************************************************************************/
BOOL Palette::SetSize(const UINT32 nNewSize)
{
	if (nNewSize == m_nSize) return TRUE;
	if (m_pColours != NULL)
	{
		delete [] m_pColours;
	}
	if (nNewSize == 0)
	{
		m_pColours = NULL;
		return TRUE;
	}
	m_pColours = new RGBQUAD[nNewSize];
//	m_StartIterator;
//	m_EndIterator;
	return (m_pColours == NULL) ? FALSE : TRUE;
}


static BOOL ReadColourMap(CCLexFile *fd, Palette& NewPalette)
{
	GIFRGBTRIPLE rgb;
	PaletteIterator PalColour;
	for (PalColour = NewPalette.Start(); PalColour < NewPalette.End(); ++PalColour)
	{
		fd->read(&rgb, sizeof(GIFRGBTRIPLE));

		PalColour.SetBlue(rgb.grgbtBlue);
		PalColour.SetGreen(rgb.grgbtGreen);
		PalColour.SetRed(rgb.grgbtRed);
		PalColour.SetTransparent(0);
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL GIFUtil::ProcessHeader()

	Author:		Colin
	Created:	11/06/96
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	SeeAlso:	

********************************************************************************************/
BOOL GIFUtil::ProcessHeader()
{
	ERROR3("GIFUtil::ProcessHeader : not implemented");
	if (TRUE) return FALSE;

	CCLexFile* File;
	GIFINFOHEADER Header;
	// This is really sizeof(GIFINFOHEADER) but this returns 14 instead of 13
	// as it rounds to the nearest word boundary
	const size_t HeaderSize = sizeof(char)* 6 + sizeof(WORD) * 2 + sizeof(BYTE) * 3;
//	const size_t HeaderSize = (sizeof(GIFINFOHEADER)* sizeof(WORD)) / sizeof(WORD);

	File->read( &Header, HeaderSize );
	if (File->bad())
		File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

	// Just double check that the signature is correct, if not then fail immediately
	if (
		( strncmp( Header.giName, "GIF89a", 6 ) != 0 ) &&
		( strncmp( Header.giName, "GIF87a", 6 ) != 0 )
	   	)
		File->GotError( _R(IDE_BADFORMAT) );

	// Note the overall size of the GIF from the header, may be the animation size
	m_GlobalWidth		= Header.giWidth;
	m_GlobalHeight  	= Header.giHeight;
//	int ColorResolution = (((Header.giFlags >> 4) & 0x07) + 1);
//	int AspectRatio		= Header.giAspect;

	UINT32 nGlobalPaletteSize	= 2 << (Header.giFlags & 0x07);
//	m_nBackground = Header.giBackground;

	// Check if we have a global colour map present or not, if so then read it in.
	if (BitSet(Header.giFlags, GLOBALCOLOURMAP))
	{
	    // Read in the global colour map into a palette structure for possible later use
		if (m_GlobalPalette.SetSize(nGlobalPaletteSize) == FALSE)
		{
			ERROR3("GIFUtil::ProcessHeader() - can't renew palette");
			return FALSE;
		}
		::ReadColourMap(File, m_GlobalPalette);
	}
	m_nCurrentBitmap = 0;
}


/********************************************************************************************

>	BOOL GIFUtil::ProcessImageBlock(CCLexFile* File, LPBITMAPINFO *Info, LPBYTE *Bits,
									UINT32 * pLeftOffset = NULL, UINT32 * pTopOffset = NULL,
									BOOL * pLocalPalette = NULL)

	Author:		Colin
	Created:	11/06/96
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	
	SeeAlso:	

********************************************************************************************/
BOOL GIFUtil::ProcessImageBlock(CCLexFile* File, LPBITMAPINFO *Info, LPBYTE *Bits,
								UINT32 * pLeftOffset, UINT32 * pTopOffset, BOOL * pLocalPalette)
{
	GIFIMAGEBLOCK ImageBlock;
	// This is really sizeof(GIFIMAGEBLOCK) but this returns 10 instead of 9 as it rounds to the nearest word boundary
	const size_t ImageHeaderSize = sizeof(WORD) * 4 + sizeof(BYTE) * 1;
	File->read( &ImageBlock, ImageHeaderSize );

	// Check whether we there is a local colour map or not
	// If there is use that otherwise use the global one read in earlier 		
	BOOL UseLocalColourMap = BitSet(ImageBlock.gibFlags, LOCALCOLOURMAP);
	int LocalColourMapSize = 1 << ((ImageBlock.gibFlags & 0x07) + 1);
					
	int Width 	= ImageBlock.gibWidth; 	//LM_to_uint(buf[4],buf[5]);
	int Height 	= ImageBlock.gibDepth;	//LM_to_uint(buf[6],buf[7]);
	
	// If the user asked for them and they are within the animation's boundaries
	// Then return the offsets to the user
	if (pLeftOffset && ImageBlock.gibLeft < m_GlobalWidth)
		*pLeftOffset = ImageBlock.gibLeft;
	if (pTopOffset && ImageBlock.gibTop < m_GlobalHeight)
		*pTopOffset = ImageBlock.gibTop;
	if (pLocalPalette)
		*pLocalPalette = UseLocalColourMap;
	
	Interlace 		= BitSet(ImageBlock.gibFlags, INTERLACE);

	int ColoursInPalette = UseLocalColourMap ? LocalColourMapSize : GlobalPaletteSize;
	int BitsPerPixel;
	if (ColoursInPalette > 16)
		BitsPerPixel = 8;
	else if (ColoursInPalette > 2)
		BitsPerPixel = 4;
	else
		BitsPerPixel = 1;

TRACEUSER("Neville", _T("Colours in palette = %d bpp = %d\n"), ColoursInPalette, BitsPerPixel );

	// Allocate the space that we require for this bitmap
	// Sanity checks on the file that we have been asked to load.
	if ((BitsPerPixel != 8) && (BitsPerPixel != 4) && (BitsPerPixel != 1) && (Width != 0) && (Height != 0))
		File->GotError( _R(IDE_FORMATNOTSUPPORTED) );

	// we know what sort of bitmap we are - lets allocate a new LPBITMAPINFO and some bytes
	*Info = AllocDIB( Width, Height, BitsPerPixel, Bits, NULL );

	if (*Info == NULL || *Bits == NULL)
		File->GotError( _R(IDS_OUT_OF_MEMORY) );

	// if the clrUsed field is zero, put a sensible value in it

	BOOL ok = FALSE;
	if (UseLocalColourMap)
	{
		// There is a local colour table specified so read this directly into
		// the palette of the DIB
		if (LocalColourMapSize > (1 << BitsPerPixel))
				File->GotError( _R(IDE_BADFORMAT) );

		ReadColourMap(File, LocalColourMapSize, (*Info)->bmiColors);
	}
	else
	{
		// No local palette so copy the global palette into the palette of the DIB, if 
		if (lpGlobalPalette && ((*Info)->bmiColors))
		{
			const size_t TotalPal = sizeof(RGBQUAD) * GlobalPaletteSize;
			memcpy((*Info)->bmiColors, lpGlobalPalette, TotalPal);
		}
	}

	// Note how many colours are in the palette
	(*Info)->bmiHeader.biClrUsed = ColoursInPalette;

	ok = ReadImage(File, *Bits, Width, Height, BitsPerPixel, Interlace);
	// Now read all that lovely data in
	if (!ok)
		File->GotError( _R(IDE_BADFORMAT) );

	return TRUE;
}


