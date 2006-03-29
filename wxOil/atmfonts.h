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
// header file for ATM Font manager. This font manager provides access and
// control of Type1 fonts

/*
*/

#ifndef INC_ATMFONTS
#define INC_ATMFONTS

//#include "atm.h"
#include "fontbase.h"
#include "pathtype.h"

class CharDescription;
class DocCoord;

/////////////////////////////////////////////////////////////////////////////
// Our thunk call mappings
/////////////////////////////////////////////////////////////////////////////

// DLL defined thunk calls
#define XSATM_GetUtilVersion			1
#define XSATM_ATMProperlyLoaded			2
#define XSATM_ATMGetVersion				3
#define XSATM_ATMGetOutline				4
#define XSATM_ATMForceExactWidth		5
#define XSATM_ATMBeginFontChange		6
#define XSATM_ATMEndFontChange			7
#define XSATM_ATMForceFontChange		8
#define XSATM_ATMFontStatus				9
#define XSATM_ATMAddFont				10
#define XSATM_ATMRemoveFont				11

// Minimum version of calls
#define MinVer_GetVersion				0x0100
#define MinVer_ProperlyLoaded			0x0100
#define MinVer_GetOutline				0x0100
#define MinVer_ForceExactWidth  		0x0101
#define MinVer_BeginFontChange			0x0205
#define MinVer_EndFontChange			0x0205
#define MinVer_ForceFontChange			0x0300
#define MinVer_FontStatus				0x0205
#define MinVer_AddFont					0x0205 
#define MinVer_RemoveFont				0x0205

// structures of DLL thunk calls
typedef BOOL (FAR PASCAL *LPFNMYCLOSEPATH) (LPSTR lpData);
typedef BOOL (FAR PASCAL *LPFNMYMOVETO) (LPATMFixedPoint lpFixPnt, LPSTR lpData);
typedef BOOL (FAR PASCAL *LPFNMYLINETO) (LPATMFixedPoint lpFixPnt, LPSTR lpData);
typedef BOOL (FAR PASCAL *LPFNMYCURVETO) (LPATMFixedPoint lpFixPnt0,
										  LPATMFixedPoint lpFixPnt1,
										  LPATMFixedPoint lpFixPnt2,
										  LPSTR lpData);

typedef WORD (FAR *LPFNATMPROPERLYLOADED)(void);

typedef WORD (FAR *LPFNATMGETUTILVERSION)(void);

typedef WORD (FAR *LPFNATMGETVERSION)(void);

typedef INT32  (FAR *LPFNATMGETOUTLINE)(	HDC              hDC,
					 					unsigned char    c,
							 			LPATMFixedMatrix lpMatrix,
							 			LPFNMYMOVETO     lpProcMoveTo,
							 			LPFNMYLINETO     lpProcLineTo,
							 			LPFNMYCURVETO	 lpProcCurveTo,
							 			LPFNMYCLOSEPATH	 lpProcClosePath,
							 			LPSTR            lpData);

typedef BOOL (FAR *LPFNATMFORCEEXACTWIDTH)(void);

typedef INT32 (FAR *LPFNATMFONTSTATUS)(LPSTR lpMenuName,
									WORD  style,
									LPSTR lpMetricsFile,
									LPSTR lpFontFile);

typedef INT32 (FAR *LPFNATMADDFONT)(LPSTR lpMenuName,
								 WORD  style,
								 LPSTR lpMetricsFile,
								 LPSTR lpFontFile);

typedef INT32 (FAR *LPFNATMREMOVEFONT)(LPSTR lpMenuName, WORD style);

typedef INT32 (FAR *LPFNATMBEGINFONTCHANGE)();
typedef INT32 (FAR *LPFNATMENDFONTCHANGE)();

/********************************************************************************************

>	class ATMFont : public FontBase
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	A class description of our ATM fonts. Mainly uses all base class functions
				at the moment, apart from the type

********************************************************************************************/

class ATMFont : public FontBase
{
	CC_DECLARE_DYNCREATE(ATMFont)

public:
	 ATMFont();
	~ATMFont();
	
	virtual FontClass GetFontClass() { return FC_ATM; };
	virtual void Dump();
};

/********************************************************************************************

>	class ATMFontMan : public CCObject
	   		
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	A static class to allow access to Type 1 fonts. There are few functions
				in this class which should be called directly. The OILFontMan provides the
				generic interface of font calls within this application.

********************************************************************************************/

class ATMFontMan : public CCObject
{
	CC_DECLARE_DYNCREATE(ATMFontMan)

	friend class OILFontMan;

// constructor
public:
	ATMFontMan();

// Function not supported by the generic OILFontMan, so you need to call them directly (yuk!)
public:
	static BOOL 	InitDLL();	// Older thunking dll 
	static BOOL 	InitLib();	// New non-thinking technology
	static void 	EnumAllFonts(OILEnumFonts* pClass);

	static INT32 	FontStatus(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile);
	static INT32 	InstallFont(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile);
	static INT32 	DeInstallFont(TCHAR *pMenuName, WORD Style);
	static BOOL 	ForceExactWidth();

	static OUTLINETEXTMETRIC *GetOutlineTextMetric(LOGFONT *pLogFont) { return NULL; }
// The support interface (functions OILFontMan will use)
private:
	static BOOL 	CacheNamedFont(String_64* pFontName);
	static BOOL 	CacheCompatibleFont(String_64* pFontName);
	static void 	ValidateCache();
	static void		FindClosestFont();
	static ATMFont* CreateNewFont(String_64* pFontName);
	static INT32 	GetUtilVersion() { return ATMUtilVersion; }
	static INT32 	GetVersion() { return ATMVersion; }
	static BOOL 	GetCharOutline( CharDescription& ChDesc, 
										DocCoord** ppCoords,
										PathVerb** ppVerbs,
										UINT32* pNumCoords,
										CDC* pDC);

// Utterly private - not even OILFontMan should use these!
private:
	static HINSTANCE GetATMDLL();
	static BOOL IsOkToCall();
	static BOOL CorrectVersion(INT32 version);

	// Older thunking dll
	static INT32 ThunkProperlyLoaded();
	static INT32	ThunkGetVersion();
	static INT32 ThunkGetUtilVersion();

private:
	// New non-thinking technology
	static INT32 LibProperlyLoaded();
	static INT32	LibGetVersion();
//	static INT32 LibGetUtilVersion();

	// this is NULL if the DLL isn't found on startup
	static HINSTANCE DLLHandle;

	// and here are types of ptrs to functions
	typedef WORD (FAR PASCAL * PFN_ATMProperlyLoaded)(void);
	typedef WORD (FAR PASCAL * PFN_ATMGetVersion)(void);

	typedef INT32  (FAR PASCAL * PFN_ATMGetOutline)(	HDC              hDC,
					 								unsigned char    c,
							 						LPATMFixedMatrix lpMatrix,
							 						LPFNMYMOVETO     lpProcMoveTo,
							 						LPFNMYLINETO     lpProcLineTo,
							 						LPFNMYCURVETO	 lpProcCurveTo,
							 						LPFNMYCLOSEPATH	 lpProcClosePath,
							 						LPSTR            lpData);

	typedef BOOL (FAR PASCAL * PFN_ATMForceExactWidth)(void);
	typedef WORD (FAR PASCAL * PFN_ATMBeginFontChange)(void);
	typedef WORD (FAR PASCAL * PFN_ATMEndFontChange)(void);
	typedef WORD (FAR PASCAL * PFN_ATMForceFontChange)(void);

	typedef INT32 (FAR PASCAL * PFN_ATMFontStatus)(	LPSTR lpMenuName,
													WORD  style,
													LPSTR lpMetricsFile,
													LPSTR lpFontFile);
	typedef INT32 (FAR PASCAL * PFN_ATMAddFont)(		LPSTR lpMenuName,
													WORD  style,
													LPSTR lpMetricsFile,
													LPSTR lpFontFile);
	typedef INT32 (FAR PASCAL * PFN_ATMRemoveFont)(	LPSTR lpMenuName, WORD style);


	// *******************
	// and actual pointers to functions in the DLL
	static PFN_ATMProperlyLoaded	pfnATMProperlyLoaded;
	static PFN_ATMGetVersion		pfnATMGetVersion;
	static PFN_ATMGetOutline		pfnATMGetOutline;
	static PFN_ATMForceExactWidth	pfnATMForceExactWidth;
	static PFN_ATMBeginFontChange	pfnATMBeginFontChange;
	static PFN_ATMEndFontChange		pfnATMEndFontChange;
	static PFN_ATMForceFontChange	pfnATMForceFontChange;
	static PFN_ATMFontStatus		pfnATMFontStatus;
	static PFN_ATMAddFont			pfnATMAddFont;
	static PFN_ATMRemoveFont		pfnATMRemoveFont;

	// *******************
	// Actual pointers to functions in the thunking DLL
	static LPFNATMPROPERLYLOADED	pfnXSATMProperlyLoaded;
//	static LPFNATMGETVERSION		pfnXSATMGetVersion;
	static LPFNATMGETOUTLINE		pfnXSATMGetOutline;
	static LPFNATMFORCEEXACTWIDTH	pfnXSATMForceExactWidth;
	static LPFNATMBEGINFONTCHANGE	pfnXSATMBeginFontChange;
	static LPFNATMENDFONTCHANGE		pfnXSATMEndFontChange;
//	static PFN_ATMForceFontChange	pfnXSATMForceFontChange;
	static LPFNATMFONTSTATUS		pfnXSATMFontStatus;
	static LPFNATMADDFONT			pfnXSATMAddFont;
	static LPFNATMREMOVEFONT		pfnXSATMRemoveFont;

private:
	static INT32 BeginFontChange();
	static INT32 AddFont(TCHAR *pMenuName, WORD Style, TCHAR *lpMetricsFile, TCHAR *lpFontFile);
	static INT32 RemoveFont(TCHAR *pMenuName, WORD Style);
	static INT32 EndFontChange();

private:
	static BOOL FAR PASCAL EXPORT MyClosePath(LPSTR lpData);
	static BOOL FAR PASCAL EXPORT MyMoveTo(LPATMFixedPoint lpFixPnt, LPSTR lpData);
	static BOOL FAR PASCAL EXPORT MyLineTo(LPATMFixedPoint lpFixPnt, LPSTR lpData);
	static BOOL FAR PASCAL EXPORT MyCurveTo(LPATMFixedPoint lpFixPnt1,
											LPATMFixedPoint lpFixPnt2,
											LPATMFixedPoint lpFixPnt3,
											LPSTR lpData);

// finally the class variables.
private:
	static BOOL ATMRunning;
	static INT32 ATMVersion;
	static INT32 ATMUtilVersion;

	static BOOL UseThunking;
};

// functions called as callbacks from some of the previous functions. The names break one
// of the style rules, but this is to re-enforce the fact that while these functions are
// global they should be treated as members of the ATMFontMan class.
// They can't be member functions, since they are used as callback functions.

INT32 APIENTRY	ATMFontMan_CallBackCacheNamedFont(ENUMLOGFONT FAR* lpelf,
												  NEWTEXTMETRIC FAR* lpntm,
												  INT32 FontType,
												  LPARAM lParam);
INT32 APIENTRY	ATMFontMan_CallBackValidateFont(ENUMLOGFONT FAR* lpelf,
												NEWTEXTMETRIC FAR* lpntm,
												INT32 FontType,
												LPARAM lParam);
INT32 APIENTRY	ATMFontMan_CallBackDispatchFont(ENUMLOGFONT FAR* lpelf,
												NEWTEXTMETRIC FAR* lpntm,
												INT32 FontType,
												LPARAM lParam);
INT32 APIENTRY	ATMFontMan_CallBackFindClosestFont(ENUMLOGFONT FAR* lpelf,
												   NEWTEXTMETRIC FAR* lpntm,
												   INT32 FontType,
												   LPARAM lParam);
#endif
