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

#if 0 != wxUSE_UNICODE
typedef wchar_t			TCHAR;
#define _istalpha		iswalpha
#define IsCharAlpha		iswalpha
#define IsCharAlphaNumeric	iswalnum
#define _istlower		iswlower
#define _istupper		iswupper
#define _istspace		iswspace
#define _istxdigit		iswxdigit
#define _istcntrl		iswcntrl
#define _istprint		iswprint
#define _totlower		towlower
#define _totupper		towupper

#define lstrlen			wcslen
#define lstrcpy			wcscpy
#define lstrcpyn		wcsncpy
#define lstrcat			wcscat
#define lstrcmp			wcscmp
#if defined(__WXMAC__)
//PORTNOTE("MacPort", "Horrendous bodge - can't find case insensitive string compare function")
// NOTE! wcscasecmp not available in wchar.h on Mac OS X (???) Bodged to be case senstive temporarilly...
#define lstrcmpi		wcscmp
#else
#define lstrcmpi		wcscasecmp
#endif

#define _strnicmp		strncasecmp
#define _snprintf		snprintf
#define _stscanf		swscanf

#define _tcslen			wcslen
#define _tcscpy			wcscpy
#define _tcsncpy		wcsncpy
#define _tcscat			wcscat
#define _tcschr			wcschr
#if defined(__WXMAC__)
#define _tcsdup			wxStrdup
#else
#define _tcsdup			wcsdup
#endif
#define _tcsrchr		wcsrchr
#define tcstok			wcstok
#define _tcscmp			wcscmp
#define _tcsncmp		wcsncmp
#if defined(__WXMAC__)
//PORTNOTE("MacPort", "Horrendous bodge - can't find case insensitive string compare function")
// NOTE! wcsncasecmp not available in wchar.h on Mac OS X (???) Bodged to be case senstive temporarilly...
#define _tcsnicmp		wcsncmp
#else
#define _tcsnicmp		wcsncasecmp
#endif
#define _tcsclen		wcslen
#define _tcsstr			wcsstr
#define _tcsinc(v)		( (v)+1 )
#define _tcsninc(s, n)	( (s)+(n) )
#define _tcsdec(s, v)	( (v)-1 )

#define tcstol( str, pszMark, radix )	wcstol( str, pszMark, radix )
#define tcstof							wcstof
#define tcstoul							wcstoul

#define tsprintf		swprintf
#define tvsprintf( dst, cch, tmt, param )	vswprintf( dst, cch, tmt, param )
#define tprintf			wprintf

#else // !wxUSE_UNICODE

typedef char			TCHAR;
#define _istalpha		isalpha
#define IsCharAlpha		isalpha
#define IsCharAlphaNumeric	isalnum
#define _istlower		islower
#define _istupper		isupper
#define _istspace		isspace
#define _istxdigit		isxdigit
#define _istcntrl		iscntrl
#define _istprint		isprint
#define _totlower		tolower
#define _totupper		toupper

#define lstrlen			strlen
#define lstrcpy			strcpy
#define lstrcpyn		strncpy
#define lstrcat			strcat
#define lstrcmp			strcmp
#define lstrcmpi		strcasecmp

#define _strnicmp		strncasecmp
#define _snprintf		snprintf

#define _tcslen			strlen
#define _tcscpy			strcpy
#define _tcsncpy		strncpy
#define _tcscat			strcat
#define _tcschr			strchr
#define _tcsdup			wcsdup
#define _tcsrchr		strrchr
#define _tcstok			strtok
#define _tcscmp			strcmp
#define _tcsncmp		strncmp
#define _tcsnicmp		strncasecmp
#define _tcsclen		strlen
#define _tcsstr			strstr
#define _tcsinc(v)		( (v)+1 )
#define _tcsninc(s, n)	( (s) + (n) )
#define _tcsdec(s, v)	( (v)-1 )

#define _ttol			atol
#define _ttoi			atoi
#define _ttof			atof
#define _tcstoul		atoul

#define _stscanf		sscanf
#define tsprintf		snprintf
#define tvsprintf( dst, cch, tmt, param )	vsprintf( dst, cch, tmt, param )
#define tprintf			printf

#endif

static void CharUpper( TCHAR *psz )
{
	for( ; *psz != '\0'; ++psz )
		*psz = _totupper( *psz );
}
static void CharLower( TCHAR *psz )
{
	for( ; *psz != '\0'; ++psz )
		*psz = _totlower( *psz );
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

typedef unsigned long long	__uint64;

#define INT32_MAX INT_MAX
#define INT32_MIN INT_MIN
#define UINT32_MAX UINT_MAX
#define UINT32_MIN UINT_MIN

#define SIZEOF_VOIDP	sizeof(void *)

#define tprintf			_tprintf

#if 0 != wxUSE_UNICODE
#define tsprintf		_stprintf
#define tvsprintf( dst, cch, tmt, param )	_vstprintf( dst, cch, tmt, param )

#define tcstok( str, sep, pszMark )			wcstok( str, sep )

#define tcstol( str, pszMark, radix )		_wtol( str, radix )
#define tcstof( str, pszMark )				wcstof( str )
#else
inline int tsprintf( TCHAR *pszDst, size_t, TCHAR *pszFormat, ... )
{
	va_list				marker;
	va_start( marker, pszFormat );
	return _vstprintf( pszDst, pszFormat, marker );
}
#define tvsprintf( dst, cch, tmt, param )	_vstprintf( dst, tmt, param )

#define tcstok( str, sep, pszMark )			strtok( str, sep )

#define tcstol( str, pszMark, radix )		atol( str )
#define tcstof( str, pszMark )				atof( str )
#endif

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

// for sprintf etc.
#if 0 != wxUSE_UNICODE
#define PERCENT_S L"%ls"
#define PERCENT_C L"%lc"
#else
#define PERCENT_S "%s"
#define PERCENT_C "%lc"
#endif

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

// The following line makes normalize.pl stop skipping this file
// Leave it at the end
/* SKIPFIXTYPES: END */

#endif // INC_COMPATDEF
