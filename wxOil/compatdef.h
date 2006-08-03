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

#ifndef INC_COMPATDEF
#define INC_COMPATDEF

// The following line makes normalize.pl skip this file
/* SKIPFIXTYPES: START */

#define USE_WXAUI

// New string function wrappers allowing portability issues to be resolved
// For now, in wxOil, they are mostly defined as the wxWidgets equivalent

// These should be used for all TCHAR string manipulation that doesn't use a 
// string class

// All ASCII string (plain char) manipulation should be done using the standard 
// strcpy type functions.

#define camIsalnum		wxIsalnum
#define camIsalpha		wxIsalpha
#define camIscntrl		wxIscntrl
#define camIsdigit		wxIsdigit
#define camIsgraph		wxIsgraph
#define camIslower		wxIslower
#define camIsprint		wxIsprint
#define camIspunct		wxIspunct
#define camIsspace		wxIsspace
#define camIsupper		wxIsupper
#define camIsxdigit		wxIsxdigit
#define camTolower		wxTolower
#define camToupper		wxToupper
#define camSetlocale	wxSetlocale
#define camStrcat		wxStrcat
#define camStrchr		wxStrchr
#define camStrcmp		wxStrcmp
#define camStrcoll		wxStrcoll
#define camStrcpy		wxStrcpy
#define camStrcspn		wxStrcspn
#define camStrdup		wxStrdup
#define camStrdupW		wxStrdupW
#define camStrftime		wxStrftime
#define camStricmp		wxStricmp
#define camStrnicmp		wxStrnicmp
#define camStrlen		wxStrlen
#define camStrclen		wxStrlen
#define camStrncat		wxStrncat
#define camStrncmp		wxStrncmp
#define camStrncpy		wxStrncpy
#define camStrpbrk		wxStrpbrk
#define camStrrchr		wxStrrchr
#define camStrspn		wxStrspn
#define camStrstr		wxStrstr
#define camStrtod		wxStrtod
#define camStrtol		wxStrtol
#define camStrtoul		wxStrtoul
#define camStrxfrm		wxStrxfrm
#define camFgetc		wxFgetc
#define camFgetchar		wxFgetchar
#define camFgets		wxFgets
#define camFopen		wxFopen
#define camFputc		wxFputc
#define camFputchar		wxFputchar
#define camFprintf		wxFprintf
#define camFputs		wxFputs
#define camFreopen		wxFreopen
#define camFscanf		wxFscanf
#define camGetc			wxGetc
#define camGetchar		wxGetchar
#define camGets			wxGets
#define camPerror		wxPerror
#define camPrintf		wxPrintf
#define camPutc			wxPutc
#define camPutchar		wxPutchar
#define camPuts			wxPuts
#define camScanf		wxScanf
#define camSprintf		wxSprintf
#define camSscanf		wxSscanf
#define camTmpnam		wxTmpnam
#define camUngetc		wxUngetc
#define camVfprintf		wxVfprintf
#define camVprintf		wxVprintf
#define camVsscanf		wxVsscanf
#define camVsprintf		wxVsprintf
#define camVsnprintf	wxVsnprintf
#define camSnprintf		wxSnprintf
#define camRemove		wxRemove
#define camRename		wxRename
#define camAtoi			wxAtoi
#define camAtol			wxAtol
#define camGetenv		wxGetenv
#define camSystem		wxSystem
#define camAsctime		wxAsctime
#define camCtime		wxCtime
#define camMbstowcs		wxMbstowcs
#define camWcstombs		wxWcstombs

// The following also need to be defined but do not have wx equivalents
// camStrtok
// camStrtof
// camStrinc(v)
// camStrninc(s, n)
// camStrdec(s, v)

// Define types that *nix doesn't have
#if !defined(__WXMSW__)
typedef char			CHAR;
typedef wchar_t			WCHAR;

#if !defined(__64BIT__)
typedef unsigned long		UINT_PTR;
typedef unsigned long		DWORD_PTR;
#else
typedef unsigned long long	UINT_PTR;
typedef unsigned long long	DWORD_PTR;
#endif

#define camStrinc(v)		( (v)+1 )
#define camStrninc(s, n)	( (s)+(n) )
#define camStrdec(s, v)		( (v)-1 )

#if 0 != wxUSE_UNICODE
typedef wchar_t			TCHAR;

#define camStrtok		wcstok
#define camStrtof( str, pszMark )	wcstof( str, pszMark )

// These two have been left in for now as they are currently needed by CCLexFile
#define _strnicmp		strncasecmp
#define _snprintf		snprintf

#else // !wxUSE_UNICODE

typedef char			TCHAR;

#define camStrtok		strtok_r
#define camStrtof( str, pszMark )	strtof( str, pszMark )

// These two have been left in for now as they are currently needed by CCLexFile
#define _strnicmp		strncasecmp
#define _snprintf		snprintf

#endif

static void CharUpper( TCHAR *psz )
{
	for( ; *psz != '\0'; ++psz )
		*psz = camToupper( *psz );
}
static void CharLower( TCHAR *psz )
{
	for( ; *psz != '\0'; ++psz )
		*psz = camTolower( *psz );
}

#if SIZEOF_CHAR == 1
typedef unsigned char		BYTE;
typedef signed char			INT8;
typedef unsigned char		UINT8;
#else
#error "Can't define BYTE, no built-in type with 1 byte length"
#endif

#if SIZEOF_SHORT == 2
typedef unsigned short		WORD;
typedef signed short		INT16;
typedef unsigned short		UINT16;
#else
#error "Can't define WORD, no built-in type with 2 byte length"
#endif

#if SIZEOF_INT == 4
typedef unsigned int		DWORD;
typedef signed int			INT32;
typedef unsigned int		UINT32;
#define INT32_MAX INT_MAX
#define INT32_MIN INT_MIN
#define UINT32_MAX UINT_MAX
#define UINT32_MIN UINT_MIN
#elif SIZEOF_LONG == 4
typedef unsigned long		DWORD;
typedef signed long			INT32;
typedef unsigned long		UINT32;
#define INT32_MAX LONG_MAX
#define INT32_MIN LONG_MIN
#define UINT32_MAX ULONG_MAX
#define UINT32_MIN ULONG_MIN
#else
#error "Can't define DWORD, no built-in type with 4 byte length"
#endif

#if SIZEOF_LONG_LONG == 8
#if SIZEOF___INT64 == 0
typedef signed long long	__int64;
#endif
typedef unsigned long long	__uint64;
typedef unsigned long long	QWORD;
typedef signed long long	INT64, 	LONGLONG;
typedef unsigned long long	UINT64,	ULONGLONG;
#else
#error "Can't define QWORD, no built-in type with 8 byte length"
#endif

// These types are deprecated
//typedef UINT32		ULONG;
//typedef INT32		LONG;
//
// So we will define them as something nasty
wxDEPRECATED(typedef UINT32		ULONG);
wxDEPRECATED(typedef INT32		LONG);


typedef INT8	*PINT8;
typedef INT16	*PINT16;
typedef INT32	*PINT32;
typedef INT64	*PINT64;
typedef UINT8	*PUINT8;
typedef UINT16	*PUINT16;
typedef UINT32	*PUINT32;
typedef UINT64	*PUINT64;

typedef INT32	BOOL;
typedef INT32   *INT_PTR;


typedef BYTE			   *PBYTE, *LPBYTE;
typedef WORD			   *PWORD, *LPWORD;
typedef DWORD			   *PDWORD, *LPDWORD;

typedef float				FLOAT;

typedef DWORD				HRESULT;
const HRESULT				S_OK = 0;
const HRESULT				E_FAIL = 0x80004005;

typedef char			   *LPSTR;
typedef const char		   *LPCSTR;

typedef char			   *PSTR;
typedef const char		   *PCSTR;

typedef wchar_t			   *LPWSTR;
typedef const wchar_t	   *LPCWSTR;

typedef TCHAR			   *LPTSTR;
typedef const TCHAR		   *LPCTSTR;

typedef TCHAR			   *PTSTR;
typedef const TCHAR		   *PCTSTR;

typedef void			   *PVOID, *LPVOID;
typedef const void		   *PCVOID;

typedef INT32				WPARAM, LPARAM, LRESULT;

#define Int32x32To64( a, b )	(INT64)( (INT64)((INT32)(a)) * (INT32)(b) )
#define UInt32x32To64( a, b )	(UINT64)( (UINT64)((INT32)(a)) * (INT32)(b) )


inline UINT32 _lrotr( UINT32 lVal, UINT32 nShift )
{
	return ( lVal >> nShift ) | ( lVal << ( 32 - nShift ) );
}

inline UINT32 _lrotl( UINT32 lVal, UINT32 nShift )
{
	return ( lVal << nShift ) | ( lVal >> ( 32 - nShift ) );
}

inline INT32 MulDiv( INT32 X, INT32 N, INT32 D )
{
//	return X*N/D ;
#if defined(__WXMSW__)
	UINT32 temp ;
	__asm {
		mov		eax,X
		imul	N
		idiv	D
		mov		temp,eax
	}
	return temp ;
#else
	return INT32( __int64(X) * N / D );
#endif
}

inline WORD HIWORD( DWORD dw )
{
	return WORD( dw >> 16 );
}

inline WORD LOWORD( DWORD dw )
{
	return WORD(dw);
}

struct RECT
{
    INT32				left;
    INT32				top;
    INT32				right;
    INT32				bottom;
};
typedef RECT		   *PRECT, *LPRECT;

struct POINT
{
	INT32					x;
	INT32					y;
};
typedef POINT		   *PPOINT, *LPPOINT;

typedef DWORD			COLORREF;
inline COLORREF RGB( BYTE r, BYTE g, BYTE b )
{
	return COLORREF(r) | ( WORD(g) << 8 ) | ( DWORD(b) << 16 );
}

inline BYTE GetBValue( COLORREF cr )
{
	return( BYTE(cr >> 16) );
}

inline BYTE GetGValue( COLORREF cr )
{
	return( BYTE(cr >> 8) );
}

inline BYTE GetRValue( COLORREF cr )
{
	return( BYTE(cr) );
}

struct RGBQUAD
{
	BYTE				rgbBlue;
	BYTE				rgbGreen;
	BYTE				rgbRed;
	BYTE				rgbReserved;
};
typedef RGBQUAD *PRGBQUAD, *LPRGBQUAD;

struct RGBTRIPLE
{
	BYTE				rgbtBlue;
	BYTE				rgbtGreen;
	BYTE				rgbtRed;
};
typedef RGBTRIPLE *PRGBTRIPLE, *LPRGBTRIPLE;

struct PALETTEENTRY
{
	BYTE				peRed;
	BYTE				peGreen;
	BYTE				peBlue;
	BYTE				peFlags;
};
typedef PALETTEENTRY *PPALETTEENTRY, *LPPALETTEENTRY;

struct LOGPALETTE
{
	WORD				palVersion;
	WORD				palNumEntries;
	PALETTEENTRY		palPalEntry[1];
};
typedef LOGPALETTE *PLOGPALETTE, *LPLOGPALETTE;

struct BITMAPINFOHEADER
{
	WORD				biSize;
	INT32				biWidth;
	INT32				biHeight;
	WORD				biPlanes;
	WORD				biBitCount;
	DWORD				biCompression;
	DWORD				biSizeImage;
	INT32				biXPelsPerMeter;
	INT32				biYPelsPerMeter;
	DWORD				biClrUsed;
	DWORD				biClrImportant;
};
typedef BITMAPINFOHEADER *PBITMAPINFOHEADER, *LPBITMAPINFOHEADER;

const DWORD				BI_RGB			= 0;
const DWORD				BI_RLE8			= 1;
const DWORD				BI_RLE4			= 2;
const DWORD				BI_BITFIELDS	= 3;

// DIB color table identifiers
const DWORD				DIB_RGB_COLORS	= 0;
const DWORD				DIB_PAL_COLORS	= 1;

struct BITMAPCOREHEADER
{
	DWORD				bcSize;
	WORD				bcWidth;
	WORD				bcHeight;
	WORD				bcPlanes;
	WORD				bcBitCount;
};
typedef BITMAPCOREHEADER *PBITMAPCOREHEADER, *LPBITMAPCOREHEADER;

struct BITMAPINFO
{
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[1];
};
typedef BITMAPINFO *PBITMAPINFO, *LPBITMAPINFO;

#pragma pack(push, 2)
struct BITMAPFILEHEADER
{
	WORD				bfType;
	DWORD				bfSize;
	WORD				bfReserved1;
	WORD				bfReserved2;
	DWORD				bfOffBits;
};
#pragma pack(pop)
typedef BITMAPFILEHEADER *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

#define MAKEFOURCC( ch0, ch1, ch2, ch3 )							\
	( DWORD(BYTE(ch0))			 | ( DWORD(BYTE(ch1)) << 8 ) |		\
	  ( DWORD(BYTE(ch2)) << 16 ) | ( DWORD(BYTE(ch1)) << 24 ) )

#define mmioFOURCC( ch0, ch1, ch2, ch3 )	MAKEFOURCC( ch0, ch1, ch2, ch3 )

struct LOGBRUSH
{
	UINT32				lbStyle;
	COLORREF			lbColor;
	PUINT32				lbHatch;
};
typedef LOGBRUSH *PLOGBRUSH, *LPLOGBRUSH;

struct BITMAP
{
};

struct DLGTEMPLATE
{
};

struct DLGITEMTEMPLATE
{
};

struct CDC
{
};

struct ABC
{
};

struct RGNDATA
{
};
typedef RGNDATA		   *LPRGNDATA;

struct NEWTEXTMETRIC
{
};

struct PAINTSTRUCT
{
};

#define TEXT				wxT

#define FAR
#define CDECL
#define PASCAL
#define __cdecl

typedef INT32 (*FARPROC)();

// Define some unwanted types
typedef INT32 			CRITICAL_SECTION;
typedef void 		   *HANDLE;
typedef void 		   *HGLOBAL;
typedef void 		   *HKEY;
typedef void 		   *HPALETTE;
typedef INT32			CLSID;
typedef void		   *HINSTANCE;

const HINSTANCE			HINSTANCE_ERROR = HINSTANCE(-1);

class IUnknown
{
public:
	virtual UINT32 AddRef()	= 0;
	virtual UINT32 Release() = 0;
	virtual ~IUnknown() { }; // avoid gcc warning
};

#define STDMETHOD(fn)		virtual HRESULT fn

const size_t			MAX_PATH = 256;

#if !defined(__WXMAC__)
inline size_t _msize( PVOID ptr )
{
	return malloc_usable_size( ptr );
}
#endif

#elif defined(__WXMSW__) /////////// MS WINDOWS SPECIFIC //////////////////////

#include <tchar.h>

typedef unsigned long long	__uint64;

#define INT32_MAX INT_MAX
#define INT32_MIN INT_MIN
#define UINT32_MAX UINT_MAX
#define UINT32_MIN UINT_MIN

#define SIZEOF_VOIDP	sizeof(void *)

#define camStrinc				_tcsinc
#define camStrninc				_tcsninc
#define camStrdec				_tcsdec
#define camStrtok( t, d, x )	_tcstok( (t), (d) )
#define camStrtof				_tcstod

#endif

// Replacements for some MFC classes
typedef TCHAR				TBYTE;
typedef TCHAR			   *PTBYTE;
typedef TCHAR			   *LPTBYTE;
typedef TCHAR			   *PTCHAR;
typedef TCHAR			   *LPTCHAR;
typedef std::map<void *, WORD>		CMapPtrToWord;
typedef std::map<void *, void *>	CMapPtrToPtr;
typedef std::map<INT32, void *>		CMapLongToPtr;
typedef std::map<void *, INT32>		CMapPtrToLong;

typedef wxPoint				CNativePoint;
typedef wxSize				CNativeSize;
typedef wxRect				CNativeRect;
typedef wxDC				CNativeDC;
typedef wxWindow			CNativeWnd;
typedef wxMenu				CNativeMenu;

#if defined(CALLBACK)
#undef CALLBACK
#endif
#if defined(APIENTRY)
#undef APIENTRY
#endif

#define CALLBACK
#define EXPORT
#define AFXAPI
#define APIENTRY
#define CONST				const

#define	AfxGetApp			::wxGetApp

#define AfxAbort			abort

// From WinUser.h - juat to allow helpuser to compile...
/*
 * Commands to pass to WinHelp()
 */
#define HELP_CONTEXT      0x0001L  /* Display topic in ulTopic */
#define HELP_QUIT         0x0002L  /* Terminate help */
#define HELP_INDEX        0x0003L  /* Display index */
#define HELP_CONTENTS     0x0003L
#define HELP_HELPONHELP   0x0004L  /* Display help on using help */
#define HELP_SETINDEX     0x0005L  /* Set current Index for multi index help */
#define HELP_SETCONTENTS  0x0005L
#define HELP_CONTEXTPOPUP 0x0008L
#define HELP_FORCEFILE    0x0009L
#define HELP_KEY          0x0101L  /* Display topic for keyword in offabData */
#define HELP_COMMAND      0x0102L
#define HELP_PARTIALKEY   0x0105L
#define HELP_MULTIKEY     0x0201L
#define HELP_SETWINPOS    0x0203L
#define HELP_CONTEXTMENU  0x000a
#define HELP_FINDER       0x000b
#define HELP_WM_HELP      0x000c
#define HELP_SETPOPUP_POS 0x000d

#define HELP_TCARD              0x8000
#define HELP_TCARD_DATA         0x0010
#define HELP_TCARD_OTHER_CALLER 0x0011


#define CAMUNUSED(x) WXUNUSED(x)

// Key bindings
#define CAMKEY(x) WXK_ ## x

inline void Beep() {wxBell();}

inline double camAtof(TCHAR * s) { double r=0.0; camSscanf(s, _T("%f"), &r); return r;}
inline INT32 camAtol(TCHAR * s) { INT32 r=0; camSscanf(s, _T("%d"), &r); return r;}

// Macro to get rid of unused variable warnings
inline void CamUse(void * p) {}
#define CAM_USE(x) do {CamUse( & x );} while(0)

#ifndef _MAX_PATH
#define _MAX_PATH _POSIX_PATH_MAX
#endif

// The following line makes normalize.pl stop skipping this file
// Leave it at the end
/* SKIPFIXTYPES: END */

#endif // INC_COMPATDEF
