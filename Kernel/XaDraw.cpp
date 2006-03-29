// $Id: XaDraw.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "camtypes.h"
// camconfig.h must be included immediately after camtypes.h
#include "camconfig.h"
#include "xadraw.h"

// If CONFIG_DEBUG_XADRAW is defined then include the implementation of debug versions
// of all the GDraw functions. The debug versions wrap calls to the actual GDraw
// functions and allow the context to be inspected before and after the call to
// GDraw.

#ifdef CONFIG_DEBUG_XADRAW

#include "gdrawdbg.h"

#if FALSE

#define XA_DRAW_ENTER XaDrawEnter

/***********************************************************************************************

>	static VOID XaDrawEnter(const TCHAR* pszFuncName, pcGCONTEXT pContext)

	Author:		Derek_Cox (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/2001
	Inputs:		pszFuncName - name of the GDraw function being called
				pContext - pointer to the context passed to the GDraw function
	Returns:	-
	Purpose:	Called before the actual GDraw function is called when CONFIG_DEBUG_XADRAW
				is defined

***********************************************************************************************/

static VOID XaDrawEnter(const TCHAR* pszFuncName, pcGCONTEXT pContext)
{
	// Output the function name and the pointer to the context
	TRACE(TEXT("%s - CTX: %p\n"), pszFuncName, pContext);
}

#define XA_DRAW_LEAVE XaDrawLeave

/***********************************************************************************************

>	static VOID XaDrawLeave(pcGCONTEXT pContext)

	Author:		Derek_Cox (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/2001
	Inputs:		pContext - pointer to the context passed to the GDraw function
	Returns:	-
	Purpose:	Called after the actual GDraw function is called when CONFIG_DEBUG_XADRAW
				is defined

***********************************************************************************************/

static VOID XaDrawLeave(pcGCONTEXT pContext)
{
	// If a context is provided then see if an error was returned (error is stored
	// in the context)
	if (NULL != pContext)
	{
		// Get the error return value
		eError Error = g_XaFnsOriginal.pfnXaDraw_GetLastError(pContext);
		if (GERROR_NO_ERROR != Error)
		{
			// If an error ocurred then output the error as a TRACE message.
			// NOTE: Don't use TRACE as this only produces output in DEBUG
			// builds.
			TCHAR szBuffer[512];
			_sntprintf(	szBuffer,
						sizeof(szBuffer),
						TEXT("XaDraw Error: (%i) %s\n"),
						(INT32)Error,
						g_XaFnsOriginal.pfnXaDraw_GetLastErrorText(pContext));
			OutputDebugString(szBuffer);
		}
	}
}

#endif	// FALSE

/***********************************************************************************************

>	static VOID XaDrawEnter(const TCHAR* pszFuncName, pcGCONTEXT pContext)

	Author:		Derek_Cox (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/2001
	Inputs:		pszFuncName - name of the GDraw function being called
				pContext - pointer to the context passed to the GDraw function
	Returns:	-
	Purpose:	Called before the actual GDraw function is called when CONFIG_DEBUG_XADRAW
				is defined

***********************************************************************************************/

#define XA_DRAW_ENTER(name,ctx)												\
	LARGE_INTEGER StartTime;												\
	XaDrawEnter(name, ctx);													\
	if (g_bAddXaTimes)														\
		QueryPerformanceCounter(&StartTime);


static VOID XaDrawEnter(const TCHAR* pszFuncName, pcGCONTEXT pContext)
{
//	TCHAR buf[512];
//	_sntprintf(buf, 512, TEXT("%s - CTX: %p\n"), pszFuncName, pContext);
//	OutputDebugString(buf);
}

/***********************************************************************************************

>	static VOID XaDrawLeave(pcGCONTEXT pContext)

	Author:		Derek_Cox (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/2001
	Inputs:		pContext - pointer to the context passed to the GDraw function
	Returns:	-
	Purpose:	Called after the actual GDraw function is called when CONFIG_DEBUG_XADRAW
				is defined

***********************************************************************************************/

#define XA_DRAW_LEAVE(ctx,ind)												\
	LARGE_INTEGER EndTime;													\
	if (g_bAddXaTimes)														\
	{																		\
		QueryPerformanceCounter(&EndTime);									\
		g_XaTimes[ind].QuadPart += EndTime.QuadPart - StartTime.QuadPart;	\
		g_XaCalls[ind] += 1;												\
	}																		\
	XaDrawLeave(ctx);

static VOID XaDrawLeave(pcGCONTEXT pContext)
{
//	if (NULL != pContext)
//	{
//		eError Error = g_XaFnsOriginal.pfnXaDraw_GetLastError(pContext);
//		if (GERROR_NO_ERROR != Error)
//		{
//			TCHAR buf[512];
//			_sntprintf(buf, 512, TEXT("XaDraw Error: (%i) %s\n"),
//					(INT32)Error,
//					g_XaFnsOriginal.pfnXaDraw_GetLastErrorText(pContext));
//			OutputDebugString(buf);
//			g_XaFnsOriginal.pfnXaDraw_ClearLastError((GCONTEXT*)pContext);
//		}
//	}
}


/***********************************************************************************************

	Debug wrapper functions for GDraw.
	See GDraw V3 for details about each function.

	Each wrapper:
		1) calls XA_DRAW_ENTER
		2) calls the original GDraw function,
		3) calls XA_DRAW_LEAVE

    For GDraw V3 most functions take a context as the first parameter. If the function
	does not require a context NULL can be passed to XA_DRAW_ENTER and XA_DRAW_LEAVE.

	Template:
	
	INT32 Xa<catgegory>Dbg_<fn_name>(GCONTEXT* pContext, INT32 nParam1)
	{
		XA_DRAW_ENTER(TEXT("<fn_name>"), pContext);
		INT32 nRetVal = g_XaFnsOriginal.pfnXa<category>_<fn_name>(pContext, nParam1);
		XA_DRAW_LEAVE(pContext);
		return nRetVal;
	}

***********************************************************************************************/

DWORD XaDrawDbg_GetVersion()
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetVersion"), NULL);
	DWORD nRetVal = g_XaFnsOriginal.pfnXaDraw_GetVersion();
	XA_DRAW_LEAVE(NULL, 0);
	return nRetVal;
}

INT32 XaDrawDbg_ClearLastError(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_ClearLastError"), pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaDraw_ClearLastError(pContext);
	XA_DRAW_LEAVE(pContext, 1);
	return nRetVal;
}

eError XaDrawDbg_GetLastError(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetLastError"), pContext);
	eError nRetVal = g_XaFnsOriginal.pfnXaDraw_GetLastError(pContext);
	XA_DRAW_LEAVE(pContext, 2);
	return nRetVal;
}

pcCHAR XaDrawDbg_GetLastErrorText(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetLastErrorText"), pContext);
	pcCHAR pRetVal = g_XaFnsOriginal.pfnXaDraw_GetLastErrorText(pContext);
	XA_DRAW_LEAVE(pContext, 3);
	return pRetVal;
}

INT32 XaDrawDbg_Initialise(pGCONTEXT pContext,pcGCONTEXT pOldContext /* =NULL */)
{
	XA_DRAW_ENTER(TEXT("XaDraw_Initialise"), pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaDraw_Initialise(pContext, pOldContext);
	XA_DRAW_LEAVE(pContext, 4);
	return nRetVal;
}

INT32 XaDrawDbg_CopyContext(pcGCONTEXT pContext,pGCONTEXT pNewContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_CopyContext"), pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaDraw_CopyContext(pContext, pNewContext);
	XA_DRAW_LEAVE(pContext, 5);
	return nRetVal;
}

INT32 XaColourDbg_GetGraduationTableSize(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaColour_GetGraduationTableSize"), pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetGraduationTableSize(pContext);
	XA_DRAW_LEAVE(pContext, 6);
	return nRetVal;
}

INT32 XaColourDbg_GetGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColour_GetGraduationTableLength"), NULL);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetGraduationTableLength();
	XA_DRAW_LEAVE(NULL, 7);
	return nRetVal;
}

INT32 XaColourDbg_GetLongGraduationTableSize(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaColour_GetLongGraduationTableSize"), pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetLongGraduationTableSize(pContext);
	XA_DRAW_LEAVE(pContext, 8);
	return nRetVal;
}

INT32 XaColourDbg_GetLongGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColour_GetLongGraduationTableLength"), NULL);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetLongGraduationTableLength();
	XA_DRAW_LEAVE(NULL, 9);
	return nRetVal;
}

INT32 XaColourDbg_GetTransparentGraduationTableSize()
{
	XA_DRAW_ENTER(TEXT("XaColour_GetTransparentGraduationTableSize"), NULL);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetTransparentGraduationTableSize();
	XA_DRAW_LEAVE(NULL, 10);
	return nRetVal;
}

INT32 XaColourDbg_GetTransparentGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColour_GetTransparentGraduationTableLength"), NULL);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetTransparentGraduationTableLength();
	XA_DRAW_LEAVE(NULL, 11);
	return nRetVal;
}

INT32 XaColourDbg_GetLongTransparentGraduationTableSize()
{
	XA_DRAW_ENTER(TEXT("XaColour_GetLongTransparentGraduationTableSize"), NULL);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetLongTransparentGraduationTableSize();
	XA_DRAW_LEAVE(NULL, 12);
	return nRetVal;
}

INT32 XaColourDbg_GetLongTransparentGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColour_GetLongTransparentGraduationTableLength"), NULL);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaColour_GetLongTransparentGraduationTableLength();
	XA_DRAW_LEAVE(NULL, 13);
	return nRetVal;
}

INT32 XaDrawDbg_SetMemoryHandlers(
	pGCONTEXT pContext,
	pBYTE (*MemoryAlloc)( UINT32 Size ),
	void  (*MemoryFree) ( pBYTE Address )
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetMemoryHandlers"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetMemoryHandlers(pContext, MemoryAlloc, MemoryFree);
	XA_DRAW_LEAVE(pContext, 14);
	return nRetVal;
}

INT32 XaDrawDbg_SetStackLimit( pGCONTEXT pContext,pcVOID StackLimit )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetStackLimit"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetStackLimit(pContext, StackLimit);
	XA_DRAW_LEAVE(pContext, 15);
	return nRetVal;
}

INT32 XaDrawDbg_SetStackSize( pGCONTEXT pContext,UINT32 StackSize )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetStackSize"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetStackSize(pContext, StackSize);
	XA_DRAW_LEAVE(pContext, 16);
	return nRetVal;
}

INT32 XaDrawDbg_Terminate(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_Terminate"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_Terminate(pContext);
	XA_DRAW_LEAVE(pContext, 17);
	return nRetVal;
}

INT32 XaDrawDbg_Clean(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_Clean"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_Clean(pContext);
	XA_DRAW_LEAVE(pContext, 18);
	return nRetVal;
}

INT32 XaDrawDbg_ContextLength()
{
	XA_DRAW_ENTER(TEXT("XaDraw_ContextLength"), NULL);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_ContextLength();
	XA_DRAW_LEAVE(NULL, 19);
	return nRetVal;
}

INT32 XaDrawDbg_ComparePalettes( pcGCONTEXT pContext,pcLOGPALETTE Palette,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDraw_ComparePalettes"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_ComparePalettes(pContext, Palette, Flag);
	XA_DRAW_LEAVE(pContext, 20);
	return nRetVal;
}

pcLOGPALETTE XaColourDbg_SelectPalette( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaColour_SelectPalette"), pContext);
	pcLOGPALETTE pRetVal =
		g_XaFnsOriginal.pfnXaColour_SelectPalette(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 21);
	return pRetVal;
}

INT32 XaColourDbg_InitialiseWithPalette( pGCONTEXT pContext,pcLOGPALETTE Palette )
{
	XA_DRAW_ENTER(TEXT("XaColour_InitialiseWithPalette"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_InitialiseWithPalette(pContext, Palette);
	XA_DRAW_LEAVE(pContext, 22);
	return nRetVal;
}

INT32 XaColourDbg_SetDitherStyle( pGCONTEXT pContext,DitherStyle Style /* =DITHER_GREY_ORDERED */)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetDitherStyle"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetDitherStyle(pContext, Style);
	XA_DRAW_LEAVE(pContext, 23);
	return nRetVal;
}

INT32 XaColourDbg_SetTileSmoothingFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTileSmoothingFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTileSmoothingFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 24);
	return nRetVal;
}

INT32 XaColourDbg_SetTileFilteringFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTileFilteringFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTileFilteringFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 25);
	return nRetVal;
}

INT32 XaColourDbg_Sharpen( pGCONTEXT pContext,INT32 Sharpen )
{
	XA_DRAW_ENTER(TEXT("XaColour_Sharpen"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_Sharpen(pContext, Sharpen);
	XA_DRAW_LEAVE(pContext, 26);
	return nRetVal;
}

INT32 XaColourDbg_Blur   ( pGCONTEXT pContext,INT32 Blur    )
{
	XA_DRAW_ENTER(TEXT("XaColour_Blur"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_Blur(pContext, Blur);
	XA_DRAW_LEAVE(pContext, 27);
	return nRetVal;
}

INT32 XaColourDbg_SetHalftoneOrigin( pGCONTEXT pContext, INT32 x,INT32 y )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetHalftoneOrigin"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetHalftoneOrigin(pContext, x, y);
	XA_DRAW_LEAVE(pContext, 28);
	return nRetVal;
}

INT32 XaColourDbg_SetColour( pGCONTEXT pContext,COLORREF Colour )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetColour"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetColour(pContext, Colour);
	XA_DRAW_LEAVE(pContext, 29);
	return nRetVal;
}

INT32 XaColourDbg_SetSolidColour( pGCONTEXT pContext,COLORREF Colour,
									UINT32 BPP /* =0 */,UINT32 Format16BPP /* =FORMAT16BPP_555 */)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetSolidColour"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetSolidColour(pContext, Colour, BPP, Format16BPP);
	XA_DRAW_LEAVE(pContext, 30);
	return nRetVal;
}

INT32 XaColourDbg_SetWordColour( pGCONTEXT pContext,DWORD Colour )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetWordColour"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetWordColour(pContext, Colour);
	XA_DRAW_LEAVE(pContext, 31);
	return nRetVal;
}

INT32 XaColourDbg_SetInvert( pGCONTEXT pContext,DWORD Colour )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetInvert"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetInvert(pContext, Colour);
	XA_DRAW_LEAVE(pContext, 32);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparency( pGCONTEXT pContext,COLORREF Colour,DWORD Style )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparency"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparency(pContext, Colour, Style);
	XA_DRAW_LEAVE(pContext, 33);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparencyLookupTable( pGCONTEXT pContext,pcBYTE Table )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparencyLookupTable"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparencyLookupTable(pContext, Table);
	XA_DRAW_LEAVE(pContext, 34);
	return nRetVal;
}

pcBYTE XaColourDbg_ReturnBrush(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaColour_ReturnBrush"), pContext);
	pcBYTE pRetVal =
		g_XaFnsOriginal.pfnXaColour_ReturnBrush(pContext);
	XA_DRAW_LEAVE(pContext, 35);
	return pRetVal;
}

pcBYTE XaColourDbg_ReturnBrushRGB(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaColour_ReturnBrushRGB"), pContext);
	pcBYTE pRetVal =
		g_XaFnsOriginal.pfnXaColour_ReturnBrushRGB(pContext);
	XA_DRAW_LEAVE(pContext, 35);
	return pRetVal;
}

INT32 XaColourDbg_AddToGraduationTable(
	pGCONTEXT pContext,
	COLORREF Colour,
	BOOL HSVFlag,
	pGraduationTable Table,
	UINT32 Index
)
{
	XA_DRAW_ENTER(TEXT("XaColour_AddToGraduationTable"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_AddToGraduationTable(pContext, Colour, HSVFlag,
															Table, Index);
	XA_DRAW_LEAVE(pContext, 36);
	return nRetVal;
}

INT32 XaColourDbg_BuildGraduationTable(
	pGCONTEXT pContext,
	COLORREF StartColour,
	COLORREF EndColour,
	BOOL HSVFlag,
	pGraduationTable Table
)
{
	XA_DRAW_ENTER(TEXT("XaColour_BuildGraduationTable"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_BuildGraduationTable(pContext, StartColour, EndColour,
															HSVFlag, Table);
	XA_DRAW_LEAVE(pContext, 37);
	return nRetVal;
}

INT32 XaColourDbg_BuildGraduationTable32(
	pGCONTEXT pContext,
	COLORREF StartColour,
	COLORREF EndColour,
	BOOL HSVFlag,
	pGraduationTable32 Table
)
{
	XA_DRAW_ENTER(TEXT("XaColour_BuildGraduationTable32"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_BuildGraduationTable32(pContext, StartColour, EndColour,
															HSVFlag, Table);
	XA_DRAW_LEAVE(pContext, 38);
	return nRetVal;
}

INT32 XaColourDbg_BuildTransparencyTable(
	pGCONTEXT pContext,
	BYTE StartColour,
	BYTE EndColour,
	pTransparentGradTable Table
)
{
	XA_DRAW_ENTER(TEXT("XaColour_BuildTransparencyTable"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_BuildTransparencyTable(pContext, StartColour, EndColour,
															Table);
	XA_DRAW_LEAVE(pContext, 39);
	return nRetVal;
}

INT32 XaColourDbg_SetGraduation(
	pGCONTEXT pContext,
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetGraduation"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetGraduation(pContext, Style, Table,
													PointA, PointB, PointC);
	XA_DRAW_LEAVE(pContext, 40);
	return nRetVal;
}

INT32 XaColourDbg_SetGraduation4(
	pGCONTEXT pContext,
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetGraduation4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetGraduation4(pContext, Style, Table,
													PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(pContext, 41);
	return nRetVal;
}

INT32 XaColourDbg_Set3WayGraduation(
	pGCONTEXT pContext,
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_Set3WayGraduation"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_Set3WayGraduation(	pContext, Style,
														ColourA, ColourB, ColourD,
														PointA, PointB, PointD);
	XA_DRAW_LEAVE(pContext, 42);
	return nRetVal;
}

INT32 XaColourDbg_Set3WayGraduation4(
	pGCONTEXT pContext,
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB,					  COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_Set3WayGraduation4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_Set3WayGraduation4(pContext, Style,
														ColourA, ColourB, ColourD,
														PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(pContext, 43);
	return nRetVal;
}

INT32 XaColourDbg_Set4WayGraduation(
	pGCONTEXT pContext,
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB,					  pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_Set4WayGraduation"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_Set4WayGraduation(	pContext, Style,
														ColourA, ColourB, ColourC, ColourD,
														PointA, PointB, PointD);
	XA_DRAW_LEAVE(pContext, 44);
	return nRetVal;
}

INT32 XaColourDbg_Set4WayGraduation4(
	pGCONTEXT pContext,
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_Set4WayGraduation4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_Set4WayGraduation4(	pContext, Style,
														ColourA, ColourB, ColourC, ColourD,
														PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(pContext, 45);
	return nRetVal;
}

INT32 XaColourDbg_SetGourand(
	pGCONTEXT pContext,
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetGourand"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetGourand(pContext, Style,
												ColourA, ColourB, ColourD,
												PointA, PointB, PointD);
	XA_DRAW_LEAVE(pContext, 46);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparentGraduation(
	pGCONTEXT pContext,
	DWORD Style,
	pcTransparentGradTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparentGraduation"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparentGraduation(	pContext, Style, Table,
																PointA, PointB, PointC);
	XA_DRAW_LEAVE(pContext, 47);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparentGraduation4(
	pGCONTEXT pContext,
	DWORD Style,
	pcTransparentGradTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparentGraduation4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparentGraduation4(	pContext, Style, Table,
																PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(pContext, 48);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparent3WayGraduation(
	pGCONTEXT pContext,
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparent3WayGraduation"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparent3WayGraduation(	pContext, Style,
																	ValueA, ValueB, ValueD,
																	PointA, PointB, PointD);
	XA_DRAW_LEAVE(pContext, 49);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparent3WayGraduation4(
	pGCONTEXT pContext,
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB,				    BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparent3WayGraduation4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparent3WayGraduation4(	pContext, Style,
																	ValueA, ValueB, ValueD,
																	PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(pContext, 50);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparent4WayGraduation(
	pGCONTEXT pContext,
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB,					pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparent4WayGraduation"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparent4WayGraduation(	pContext, Style,
																	ValueA, ValueB, ValueC, ValueD,
																	PointA, PointB,         PointD);
	XA_DRAW_LEAVE(pContext, 51);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparent4WayGraduation4(
	pGCONTEXT pContext,
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparent4WayGraduation4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparent4WayGraduation4(	pContext, Style,
																	ValueA, ValueB, ValueC, ValueD,
																	PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(pContext, 52);
	return nRetVal;
}

INT32 XaBitmap_SetBias( double fBias )
{
	XA_DRAW_ENTER(TEXT("XaBitmap_SetBias"),pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaBitmap_SetBias(pContext,fBias);
	XA_DRAW_LEAVE(pConvert,?);
	return nRetVal;
}

INT32 XaBitmap_SetGain( double fGain )
{
	XA_DRAW_ENTER(TEXT("XaBitmap_SetGain"),pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaBitmap_SetGain(pContext,fGain);
	XA_DRAW_LEAVE(pConvert,?);
	return nRetVal;
}

INT32 XaBitmap_SetContone( pGCONTEXT pContext, UINT32 uContoneStyle, COLORREF rgbStart,COLORREF rgbEnd )
{
	XA_DRAW_ENTER(TEXT("XaBitmap_SetContone"),pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaBitmap_SetContone(pContext,uContoneStyle,rgbStart,rgbEnd);
	XA_DRAW_LEAVE(pConvert,?);
	return nRetVal;
}

INT32 XaBitmap_SetTransparencyRamp( pGCONTEXT pContext, BYTE uStart, BYTE uEnd )
{
	XA_DRAW_ENTER(TEXT("XaBitmap_SetTransparencyRamp"),pContext);
	INT32 nRetVal = g_XaFnsOriginal.pfnXaBitmap_SetTransparencyRamp(pContext,uStart,uEnd);
	XA_DRAW_LEAVE(pConvert,?);
	return nRetVal;
}

INT32 XaColourDbg_SetTilePattern(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	COLORREF DefaultColour /* =0 */,
	pcBGRT TranslationTable /* =0 */,
	pcBYTE   RedTranslationTable /* =0 */,
	pcBYTE GreenTranslationTable /* =0 */,
	pcBYTE  BlueTranslationTable /* =0 */,
	pcBYTE TransparencyTranslationTable /* =0 */,
	INT32 TileOffset /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTilePattern"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTilePattern(pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC,
													DefaultColour,
													TranslationTable,
													RedTranslationTable,
													GreenTranslationTable,
													BlueTranslationTable,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(pContext, 53);
	return nRetVal;
}

INT32 XaColourDbg_SetTilePattern4(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	COLORREF DefaultColour /* =0 */,
	pcBGRT TranslationTable /* =0 */,
	pcBYTE   RedTranslationTable /* =0 */,
	pcBYTE GreenTranslationTable /* =0 */,
	pcBYTE  BlueTranslationTable /* =0 */,
	pcBYTE TransparencyTranslationTable /* =0 */,
	INT32 TileOffset /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTilePattern4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTilePattern4(pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC, PointD,
													DefaultColour,
													TranslationTable,
													RedTranslationTable,
													GreenTranslationTable,
													BlueTranslationTable,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(pContext, 54);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparentTilePattern(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	BYTE DefaultColour /* =0 */,
	pcBYTE TransparencyTranslationTable /* =0 */,
	INT32 TileOffset /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparentTilePattern"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparentTilePattern(pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC,
													DefaultColour,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(pContext, 55);
	return nRetVal;
}

INT32 XaColourDbg_SetTransparentTilePattern4(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	BYTE DefaultColour /* =0 */,
	pcBYTE TransparencyTranslationTable /* =0 */,
	INT32 TileOffset /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetTransparentTilePattern4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetTransparentTilePattern4(pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC, PointD,
													DefaultColour,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(pContext, 56);
	return nRetVal;
}

INT32 XaColourDbg_ConvertBitmap(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
	pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
	DWORD Dither
)
{
	XA_DRAW_ENTER(TEXT("XaColour_ConvertBitmap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_ConvertBitmap(	pContext,
													SBitmapInfo, SBitmap,
													DBitmapInfo, DBitmap, Dither);
	XA_DRAW_LEAVE(pContext, 57);
	return nRetVal;
}

INT32 XaColourDbg_SetConversionPalette( pGCONTEXT pContext,pcLOGPALETTE pPalette )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetConversionPalette"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetConversionPalette(pContext, pPalette);
	XA_DRAW_LEAVE(pContext, 58);
	return nRetVal;
}

INT32 XaColourDbg_SetMaxDiffusionError( pGCONTEXT pContext,UINT32 MaxError )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetMaxDiffusionError"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetMaxDiffusionError(pContext, MaxError);
	XA_DRAW_LEAVE(pContext, 59);
	return nRetVal;
}

INT32 XaColourDbg_ScaleBitmap(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
	pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
	UINT32 Channels
)
{
	XA_DRAW_ENTER(TEXT("XaColour_ScaleBitmap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_ScaleBitmap(	pContext,
													SBitmapInfo, SBitmap,
													DBitmapInfo, DBitmap,
													Channels);
	XA_DRAW_LEAVE(pContext, 60);
	return nRetVal;
}


INT32 XaColourDbg_SetGreyConversionValues( pGCONTEXT pContext,UINT32 Red,UINT32 Green,UINT32 Blue )
{
	XA_DRAW_ENTER(TEXT("XaColour_SetGreyConversionValues"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetGreyConversionValues(	pContext,
																Red, Green, Blue);
	XA_DRAW_LEAVE(pContext, 61);
	return nRetVal;
}

COLORREF XaColourDbg_ConvertHSVtoRGB( COLORREF hsv )
{
	XA_DRAW_ENTER(TEXT("XaColour_ConvertHSVtoRGB"), NULL);
	COLORREF rgb =
		g_XaFnsOriginal.pfnXaColour_ConvertHSVtoRGB(hsv);
	XA_DRAW_LEAVE(NULL, 62);
	return rgb;
}

COLORREF XaColourDbg_ConvertRGBtoHSV( COLORREF rgb )
{
	XA_DRAW_ENTER(TEXT("XaColour_ConvertRGBtoHSV"), NULL);
	COLORREF hsv =
		g_XaFnsOriginal.pfnXaColour_ConvertRGBtoHSV(rgb);
	XA_DRAW_LEAVE(NULL, 63);
	return hsv;
}

INT32 XaColourDbg_SetSeparationTables(
	pGCONTEXT pContext,
	pcBGR    CyanSeparationTable,
	pcBGR MagentaSeparationTable,
	pcBGR  YellowSeparationTable,
	pcBGR   BlackSeparationTable,
	pcBYTE UnderColourRemovalTable,
	pcBYTE BlackGenerationTable
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetSeparationTables"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetSeparationTables(	pContext,
															CyanSeparationTable,
															MagentaSeparationTable,
															YellowSeparationTable,
															BlackSeparationTable,
															UnderColourRemovalTable,
															BlackGenerationTable);
	XA_DRAW_LEAVE(pContext, 64);
	return nRetVal;
}

INT32 XaColourDbg_SetBitmapConversionTable(
	pGCONTEXT pContext,
	pcBGR BitmapConversionTable
)
{
	XA_DRAW_ENTER(TEXT("XaColour_SetBitmapConversionTable"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaColour_SetBitmapConversionTable(	pContext,
																BitmapConversionTable);
	XA_DRAW_LEAVE(pContext, 65);
	return nRetVal;
}

INT32 XaDrawDbg_SetDIBitmap(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pBYTE Bitmap,
	UINT32 Format16BPP /* =FORMAT16BPP_555 */
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetDIBitmap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetDIBitmap(	pContext,
												BitmapInfo, Bitmap, Format16BPP);
	XA_DRAW_LEAVE(pContext, 66);
	return nRetVal;
}

INT32 XaDrawDbg_SetInvertedDIBitmap(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pBYTE Bitmap,
	UINT32 Format16BPP /* =FORMAT16BPP_555 */
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetInvertedDIBitmap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetInvertedDIBitmap(	pContext,
														BitmapInfo, Bitmap, Format16BPP);
	XA_DRAW_LEAVE(pContext, 67);
	return nRetVal;
}

INT32 XaDrawDbg_SetMatrix( pGCONTEXT pContext,pcGMATRIX Matrix )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetMatrix"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetMatrix(pContext, Matrix);
	XA_DRAW_LEAVE(pContext, 68);
	return nRetVal;
}

INT32 XaDrawDbg_MaxScale ( pGCONTEXT pContext,pcGMATRIX Matrix,pcRECT BBox )
{
	XA_DRAW_ENTER(TEXT("XaDraw_MaxScale"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_MaxScale(pContext, Matrix, BBox);
	XA_DRAW_LEAVE(pContext, 69);
	return nRetVal;
}

INT32 XaDrawDbg_GetMaxBitmapWidth()
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetMaxBitmapWidth"), NULL);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_GetMaxBitmapWidth();
	XA_DRAW_LEAVE(NULL, 70);
	return nRetVal;
}

INT32 XaDrawDbg_GetMaxBitmapDepth()
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetMaxBitmapDepth"), NULL);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_GetMaxBitmapDepth();
	XA_DRAW_LEAVE(NULL, 71);
	return nRetVal;
}

INT32 XaDrawDbg_SetAntialiasFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetAntialiasFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetAntialiasFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 72);
	return nRetVal;
}

INT32 XaDrawDbg_SetDashAdjustmentFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetDashAdjustmentFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetDashAdjustmentFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 73);
	return nRetVal;
}

INT32 XaDrawDbg_SetHintingFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetHintingFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetHintingFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 74);
	return nRetVal;
}

INT32 XaDrawDbg_SetFlatness( pGCONTEXT pContext,UINT32 Flatness )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetFlatness"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetFlatness(pContext, Flatness);
	XA_DRAW_LEAVE(pContext, 75);
	return nRetVal;
}

INT32 XaDrawDbg_SetMiterLimit( pGCONTEXT pContext,UINT32 MiterLimit )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetMiterLimit"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetMiterLimit(pContext, MiterLimit);
	XA_DRAW_LEAVE(pContext, 76);
	return nRetVal;
}

INT32 XaDrawDbg_FillRectangle( pGCONTEXT pContext,pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDraw_FillRectangle"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_FillRectangle(pContext, Rectangle);
	XA_DRAW_LEAVE(pContext, 77);
	return nRetVal;
}

INT32 XaDrawDbg_FillPoint( pGCONTEXT pContext,pcPOINT Point )
{
	XA_DRAW_ENTER(TEXT("XaDraw_FillPoint"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_FillPoint(pContext, Point);
	XA_DRAW_LEAVE(pContext, 78);
	return nRetVal;
}

INT32 XaDrawDbg_FillPath(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_FillPath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_FillPath(pContext, Points, Types, Length, Winding);
	XA_DRAW_LEAVE(pContext, 79);
	return nRetVal;
}

INT32 XaDrawDbg_InitialiseFillPath( pGCONTEXT pContext,UINT32 Winding,BOOL DontPlot )
{
	XA_DRAW_ENTER(TEXT("XaDraw_InitialiseFillPath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_InitialiseFillPath(pContext, Winding, DontPlot);
	XA_DRAW_LEAVE(pContext, 80);
	return nRetVal;
}

INT32 XaDrawDbg_FillPathLine( pGCONTEXT pContext,pcFVector2D Points, BOOL Reverse )
{
	XA_DRAW_ENTER(TEXT("XaDraw_FillPathLine"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_FillPathLine(pContext, Points, Reverse);
	XA_DRAW_LEAVE(pContext, 81);
	return nRetVal;
}

INT32 XaDrawDbg_DoFillPath( pGCONTEXT pContext )
{
	XA_DRAW_ENTER(TEXT("XaDraw_DoFillPath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_DoFillPath(pContext);
	XA_DRAW_LEAVE(pContext, 82);
	return nRetVal;
}

INT32 XaDrawDbg_FillTriangle(
	pGCONTEXT pContext,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	DWORD Flags
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_FillTriangle"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_FillTriangle(pContext, PointA, PointB, PointC, Flags);
	XA_DRAW_LEAVE(pContext, 83);
	return nRetVal;
}

INT32 XaDrawDbg_HintPath(
	pGCONTEXT pContext,
	pPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_HintPath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_HintPath(pContext, Points, Types, Length, Close, LineWidth);
	XA_DRAW_LEAVE(pContext, 84);
	return nRetVal;
}

INT32 XaDrawDbg_StrokePath(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_StrokePath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_StrokePath(pContext, Points, Types, Length, Close,
												LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(pContext, 85);
	return nRetVal;
}

INT32 XaDrawDbg_StrokePathToPath(
	pGCONTEXT pContext,
	pcPOINT IPoints,
	pcBYTE  ITypes,
	UINT32 ILength,
	pPOINT OPoints,
	pBYTE  OTypes,
	UINT32 OLength,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_StrokePathToPath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_StrokePathToPath(pContext,
													IPoints, ITypes, ILength,
													OPoints, OTypes, OLength, Close,
													LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(pContext, 86);
	return nRetVal;
}

INT32 XaDrawDbg_CalcStrokeBBox(
	pGCONTEXT pContext,
	pcPOINT IPoints,
	pcBYTE  ITypes,
	UINT32 ILength,
	pRECT Rect,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_CalcStrokeBBox"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_CalcStrokeBBox(	pContext,
													IPoints, ITypes, ILength,
													Rect, Close,
													LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(pContext, 87);
	return nRetVal;
}

INT32 XaDrawDbg_IsOverlap(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_IsOverlap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_IsOverlap(	pContext,
												Points, Types, Length, Winding);
	XA_DRAW_LEAVE(pContext, 88);
	return nRetVal;
}

INT32 XaDrawDbg_IsStrokeOverlap(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth,
	DWORD LineCaps,
	DWORD LineJoin,
	pcDashType Dash
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_IsStrokeOverlap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_IsStrokeOverlap(	pContext,
													Points, Types, Length, Close,
													LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(pContext, 89);
	return nRetVal;
}

INT32 XaDrawDbg_GetStatistics(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pSTATISTICS Stats
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetStatistics"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_GetStatistics(	pContext,
													Points, Types, Length,
													Winding, Stats);
	XA_DRAW_LEAVE(pContext, 90);
	return nRetVal;
}

INT32 XaDrawDbg_MakeRegion(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pREGION ORegion,
	UINT32 OLength
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_MakeRegion"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_MakeRegion(	pContext,
												Points, Types, Length,
												Winding,
												ORegion, OLength);
	XA_DRAW_LEAVE(pContext, 91);
	return nRetVal;
}

INT32 XaDrawDbg_MakeUnclippedRegion(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pREGION ORegion,
	UINT32 OLength
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_MakeUnclippedRegion"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_MakeUnclippedRegion(	pContext,
														Points, Types, Length,
														Winding,
														ORegion, OLength);
	XA_DRAW_LEAVE(pContext, 92);
	return nRetVal;
}

INT32 XaDrawDbg_FillRegion(
	pGCONTEXT pContext,
	pcREGION Region,
	pcPOINT Offset
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_FillRegion"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_FillRegion(pContext, Region, Offset);
	XA_DRAW_LEAVE(pContext, 93);
	return nRetVal;
}

INT32 XaDrawDbg_ClipRectangle( pGCONTEXT pContext,pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDraw_ClipRectangle"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_ClipRectangle(pContext, Rectangle);
	XA_DRAW_LEAVE(pContext, 94);
	return nRetVal;
}

INT32 XaDrawDbg_DeviceClipRectangle( pGCONTEXT pContext,pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDraw_DeviceClipRectangle"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_DeviceClipRectangle(pContext, Rectangle);
	XA_DRAW_LEAVE(pContext, 95);
	return nRetVal;
}

INT32 XaDrawDbg_GetDeviceClipRectangle( pcGCONTEXT pContext,pRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetDeviceClipRectangle"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_GetDeviceClipRectangle(pContext, Rectangle);
	XA_DRAW_LEAVE(pContext, 96);
	return nRetVal;
}

INT32 XaDrawDbg_ClipRegion( pGCONTEXT pContext,pcREGION Region )
{
	XA_DRAW_ENTER(TEXT("XaDraw_ClipRegion"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_ClipRegion(pContext, Region);
	XA_DRAW_LEAVE(pContext, 97);
	return nRetVal;
}

pcREGION XaDrawDbg_GetClipRegion(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetClipRegion"), pContext);
	pcREGION pRetVal =
		g_XaFnsOriginal.pfnXaDraw_GetClipRegion(pContext);
	XA_DRAW_LEAVE(pContext, 98);
	return pRetVal;
}

INT32 XaDrawDbg_CalcSimpleBBox(
	pcPOINT Points,
	UINT32 Length,
	pRECT Rect
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_CalcSimpleBBox"), NULL);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_CalcSimpleBBox(Points, Length, Rect);
	XA_DRAW_LEAVE(NULL, 99);
	return nRetVal;
}

INT32 XaDrawDbg_CalcBBox(
	pGCONTEXT pContext,
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	pRECT Rect,
	BOOL Flatten
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_CalcBBox"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_CalcBBox(pContext, Points, Types, Length, Rect, Flatten);
	XA_DRAW_LEAVE(pContext, 100);
	return nRetVal;
}

INT32 XaDrawDbg_TransformPath(
	pGCONTEXT pContext,
	pcPOINT IPoints,
	 pPOINT OPoints,
	UINT32 Length,
	pcGMATRIX Matrix
)
{
	XA_DRAW_ENTER(TEXT("XaDraw_TransformPath"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_TransformPath(	pContext,
													IPoints, OPoints,
													Length, Matrix);
	XA_DRAW_LEAVE(pContext, 101);
	return nRetVal;
}

INT32 XaDrawDbg_ScrollBitmap( pGCONTEXT pContext, INT32 x,INT32 y )
{
	XA_DRAW_ENTER(TEXT("XaDraw_ScrollBitmap"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_ScrollBitmap(pContext, x, y);
	XA_DRAW_LEAVE(pContext, 102);
	return nRetVal;
}

INT32 XaDrawDbg_ClearChangedBBox(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDraw_ClearChangedBBox"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_ClearChangedBBox(pContext);
	XA_DRAW_LEAVE(pContext, 103);
	return nRetVal;
}

INT32 XaDrawDbg_GetChangedBBox( pcGCONTEXT pContext, pRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDraw_GetChangedBBox"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_GetChangedBBox(pContext, Rectangle);
	XA_DRAW_LEAVE(pContext, 104);
	return nRetVal;
}

INT32 XaDrawDbg_SetChangedBBox(  pGCONTEXT pContext,pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetChangedBBox"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetChangedBBox(pContext, Rectangle);
	XA_DRAW_LEAVE(pContext, 105);
	return nRetVal;
}

INT32 XaDrawDbg_SetBevelContrast ( pGCONTEXT pContext,UINT32 Contrast )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetBevelContrast"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetBevelContrast(pContext, Contrast);
	XA_DRAW_LEAVE(pContext, 106);
	return nRetVal;
}

INT32 XaDrawDbg_SetBevelLightness( pGCONTEXT pContext,UINT32 Contrast )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetBevelLightness"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetBevelLightness(pContext, Contrast);
	XA_DRAW_LEAVE(pContext, 107);
	return nRetVal;
}

INT32 XaDrawDbg_SetBevelDarkness ( pGCONTEXT pContext,UINT32 Contrast )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetBevelDarkness"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetBevelDarkness(pContext, Contrast);
	XA_DRAW_LEAVE(pContext, 108);
	return nRetVal;
}

INT32 XaDrawDbg_TranslateBevelValue( pcGCONTEXT pContext, BYTE Index, BYTE Colour )
{
	XA_DRAW_ENTER(TEXT("XaDraw_TranslateBevelValue"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_TranslateBevelValue(pContext, Index, Colour);
	XA_DRAW_LEAVE(pContext, 109);
	return nRetVal;
}

INT32 XaSpriteDbg_PlotTile(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcBGRT TranslationTable /* =0 */,
	pcBYTE   RedTranslationTable /* =0 */,
	pcBYTE GreenTranslationTable /* =0 */,
	pcBYTE  BlueTranslationTable /* =0 */,
	pcBYTE TransparencyTranslationTable /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("XaSprite_PlotTile"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaSprite_PlotTile(	pContext,
												BitmapInfo, Bitmap, Style,
												PointA, PointB, PointC,
												TranslationTable,
												RedTranslationTable,
												GreenTranslationTable,
												BlueTranslationTable,
												TransparencyTranslationTable);
	XA_DRAW_LEAVE(pContext, 110);
	return nRetVal;
}

INT32 XaSpriteDbg_PlotTile4(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	pcBGRT TranslationTable = 0,
	pcBYTE   RedTranslationTable = 0,
	pcBYTE GreenTranslationTable = 0,
	pcBYTE  BlueTranslationTable = 0,
	pcBYTE TransparencyTranslationTable = 0
)
{
	XA_DRAW_ENTER(TEXT("XaSprite_PlotTile4"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaSprite_PlotTile4(	pContext,
												BitmapInfo, Bitmap, Style,
												PointA, PointB, PointC, PointD,
												TranslationTable,
												RedTranslationTable,
												GreenTranslationTable,
												BlueTranslationTable,
												TransparencyTranslationTable);
	XA_DRAW_LEAVE(pContext, 111);
	return nRetVal;
}

INT32 Xa3DDbg_SetTruePerspectiveFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("Xa3D_SetTruePerspectiveFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_SetTruePerspectiveFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 112);
	return nRetVal;
}

INT32 Xa3DDbg_DefineView(
	pGCONTEXT pContext,
	INT32 nObserverDistance,
	COLORREF Background /* =0 */,
	pcBITMAPINFOHEADER pTextureInfo /* =NULL */,
	pcBYTE pTextureBitmap /* =NULL */,
	pcBGRT pTextureColours /* =NULL */,
	UINT32 uTextureXOffset /* =0 */,
	UINT32 uTextureYOffset /* =0 */,
	pcBYTE pOverlayBitmap /* =NULL */,
	pcBITMAPINFOHEADER pShadowInfo /* =NULL */,
	pcBYTE pShadowBitmap /* =NULL */,
	COLORREF ShadowColour /* =0 */,
	BOOL bTransparent /* =NULL */,
	BOOL bAlpha /* =NULL */,
	BOOL bTransparentTexture /* =NULL */,
	UINT32 uFade /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_DefineView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_DefineView(pContext,
											nObserverDistance, Background,
											pTextureInfo, pTextureBitmap, pTextureColours,
											uTextureXOffset, uTextureYOffset,
											pOverlayBitmap,
											pShadowInfo, pShadowBitmap, ShadowColour,
											bTransparent, bAlpha, bTransparentTexture,
											uFade);
	XA_DRAW_LEAVE(pContext, 113);
	return nRetVal;
}

INT32 Xa3DDbg_DefineTexture(
	pGCONTEXT pContext,
	pcBITMAP pBitmap,
	BOOL bScaleDown,
	BOOL bRepeating
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_DefineTexture"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_DefineTexture(pContext,
												pBitmap, bScaleDown, bRepeating);
	XA_DRAW_LEAVE(pContext, 114);
	return nRetVal;
}

INT32 Xa3DDbg_Define2ndTexture(
	pGCONTEXT pContext,
	pcBITMAPINFOHEADER pBitmapInfo,
	pcBYTE pBitmap,
	pcBGRT pRGBIndex,
	BOOL bTint,
	COLORREF TintColour
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_Define2ndTexture"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_Define2ndTexture(	pContext,
													pBitmapInfo, pBitmap, pRGBIndex,
													bTint, TintColour);
	XA_DRAW_LEAVE(pContext, 115);
	return nRetVal;
}

INT32 Xa3DDbg_ConvertColourFormat( pcColour pSColour, pColour pDColour )
{
	XA_DRAW_ENTER(TEXT("Xa3D_ConvertColourFormat"), NULL);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_ConvertColourFormat(pSColour, pDColour);
	XA_DRAW_LEAVE(NULL, 116);
	return nRetVal;
}

INT32 Xa3DDbg_AddTriangleToView(
	pGCONTEXT pContext,
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_AddTriangleToView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_AddTriangleToView(	pContext,
													pPointA, pPointB, pPointC,
													pColourA, pColourB, pColourC,
													bForward);
	XA_DRAW_LEAVE(pContext, 117);
	return nRetVal;
}

INT32 Xa3DDbg_AddFlatTriangleToView(
	pGCONTEXT pContext,
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour pFlatColour,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_AddFlatTriangleToView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_AddFlatTriangleToView(	pContext,
														pPointA, pPointB, pPointC,
														pFlatColour, bForward);
	XA_DRAW_LEAVE(pContext, 118);
	return nRetVal;
}

INT32 Xa3DDbg_AddTextureToView(
	pGCONTEXT pContext,
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
	pcColour   pColourA2,pcColour   pColourB2,pcColour   pColourC2,
	pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_AddTextureToView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_AddTextureToView(	pContext,
													pPointA, pPointB, pPointC,
													pColourA, pColourB, pColourC,
													pColourA2, pColourB2, pColourC2,
													pPointA2, pPointB2, pPointC2,
													bForward);
	XA_DRAW_LEAVE(pContext, 119);
	return nRetVal;
}

INT32 Xa3DDbg_AddFlatTextureToView(
	pGCONTEXT pContext,
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour pFlatColour,
	pcColour pFlatColour2,
	pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_AddFlatTextureToView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_AddFlatTextureToView(	pContext,
														pPointA, pPointB, pPointC,
														pFlatColour, pFlatColour2,
														pPointA2, pPointB2, pPointC2,
														bForward);
	XA_DRAW_LEAVE(pContext, 120);
	return nRetVal;
}

INT32 Xa3DDbg_AddTexturesToView(
	pGCONTEXT pContext,
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
	pcColour   pColourA2,pcColour   pColourB2,pcColour   pColourC2,
	pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
	pcFVector2D pPointA3,pcFVector2D pPointB3,pcFVector2D pPointC3,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_AddTexturesToView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_AddTexturesToView(	pContext,
													pPointA, pPointB, pPointC,
													pColourA, pColourB, pColourC,
													pColourA2, pColourB2, pColourC2,
													pPointA2, pPointB2, pPointC2,
													pPointA3, pPointB3, pPointC3,
													bForward);
	XA_DRAW_LEAVE(pContext, 121);
	return nRetVal;
}

INT32 Xa3DDbg_AddFlatTexturesToView(
	pGCONTEXT pContext,
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour pFlatColour,
	pcColour pFlatColour2,
	pcFVector2D pPointA2,pcFVector2D pPointB2,pcFVector2D pPointC2,
	pcFVector2D pPointA3,pcFVector2D pPointB3,pcFVector2D pPointC3,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3D_AddFlatTexturesToView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_AddFlatTexturesToView(	pContext,
														pPointA, pPointB, pPointC,
														pFlatColour, pFlatColour2,
														pPointA2, pPointB2, pPointC2,
														pPointA3, pPointB3, pPointC3,
														bForward);
	XA_DRAW_LEAVE(pContext, 122);
	return nRetVal;
}

INT32 Xa3DDbg_SetSideFaceFlag( pGCONTEXT pContext,BOOL bFlag )
{
	XA_DRAW_ENTER(TEXT("Xa3D_SetSideFaceFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_SetSideFaceFlag(pContext, bFlag);
	XA_DRAW_LEAVE(pContext, 123);
	return nRetVal;
}

INT32 Xa3DDbg_GetViewBBox( pcGCONTEXT pContext,pRECT pBBox )
{
	XA_DRAW_ENTER(TEXT("Xa3D_GetViewBBox"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_GetViewBBox(pContext, pBBox);
	XA_DRAW_LEAVE(pContext, 124);
	return nRetVal;
}

INT32 Xa3DDbg_PlotView(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("Xa3D_PlotView"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXa3D_PlotView(pContext);
	XA_DRAW_LEAVE(pContext, 125);
	return nRetVal;
}

INT32 XaDrawDbg_SetAntialiasQualityFlag( pGCONTEXT pContext,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDraw_SetAntialiasQualityFlag"), pContext);
	INT32 nRetVal =
		g_XaFnsOriginal.pfnXaDraw_SetAntialiasQualityFlag(pContext, Flag);
	XA_DRAW_LEAVE(pContext, 126);
	return nRetVal;
}

/***********************************************************************************************

  g_XaFns is a table of pointers to the debug wrapper functions implemented above. This
  structure is used by the Xa*_ macros to call the wrapper functions.

***********************************************************************************************/

BOOL g_bAddXaTimes = FALSE;

LARGE_INTEGER g_Frequency = {0};

LARGE_INTEGER g_XaTimes[127] = {{0}};

DWORD g_XaCalls[127] = {0};

TCHAR* g_XaNames[127] = {
	"GDraw_GetVersion",
	"GDraw_ClearLastError",
	"GDraw_GetLastError",
	"GDraw_GetLastErrorText",
	"GDraw_Initialise",
	"GDraw_CopyContext",
	"GColour_GetGraduationTableSize",
	"GColour_GetGraduationTableLength",
	"GColour_GetLongGraduationTableSize",
	"GColour_GetLongGraduationTableLength",
	"GColour_GetTransparentGraduationTableSize",
	"GColour_GetTransparentGraduationTableLength",
	"GColour_GetLongTransparentGraduationTableSize",
	"GColour_GetLongTransparentGraduationTableLength",
	"GDraw_SetMemoryHandlers",
	"GDraw_SetStackLimit",
	"GDraw_SetStackSize",
	"GDraw_Terminate",
	"GDraw_Clean",
	"GDraw_ContextLength",
	"GDraw_ComparePalettes",
	"GColour_SelectPalette",
	"GColour_InitialiseWithPalette",
	"GColour_SetDitherStyle",
	"GColour_SetTileSmoothingFlag",
	"GColour_SetTileFilteringFlag",
	"GColour_Sharpen",
	"GColour_Blur   ",
	"GColour_SetHalftoneOrigin",
	"GColour_SetColour",
	"GColour_SetSolidColour",
	"GColour_SetWordColour",
	"GColour_SetInvert",
	"GColour_SetTransparency",
	"GColour_SetTransparencyLookupTable",
	"GColour_ReturnBrush",
	"GColour_ReturnBrushRGB",
	"GColour_AddToGraduationTable",
	"GColour_BuildGraduationTable",
	"GColour_BuildGraduationTable32",
	"GColour_BuildTransparencyTable",
	"GColour_SetGraduation",
	"GColour_SetGraduation4",
	"GColour_Set3WayGraduation",
	"GColour_Set3WayGraduation4",
	"GColour_Set4WayGraduation",
	"GColour_Set4WayGraduation4",
	"GColour_SetGourand",
	"GColour_SetTransparentGraduation",
	"GColour_SetTransparentGraduation4",
	"GColour_SetTransparent3WayGraduation",
	"GColour_SetTransparent3WayGraduation4",
	"GColour_SetTransparent4WayGraduation",
	"GColour_SetTransparent4WayGraduation4",
	"GBitmap_SetBias",
	"GBitmap_SetGain",
	"GBitmap_SetContone",
	"GBitmap_SetTransparencyRamp",
	"GColour_SetTilePattern",
	"GColour_SetTilePattern4",
	"GColour_SetTransparentTilePattern",
	"GColour_SetTransparentTilePattern4",
	"GColour_ConvertBitmap",
	"GColour_SetConversionPalette",
	"GColour_SetMaxDiffusionError",
	"GColour_ScaleBitmap",
	"GColour_SetGreyConversionValues",
	"GColour_ConvertHSVtoRGB",
	"GColour_ConvertRGBtoHSV",
	"GColour_SetSeparationTables",
	"GColour_SetBitmapConversionTable",
	"GDraw_SetDIBitmap",
	"GDraw_SetInvertedDIBitmap",
	"GDraw_SetMatrix",
	"GDraw_MaxScale ",
	"GDraw_GetMaxBitmapWidth",
	"GDraw_GetMaxBitmapDepth",
	"GDraw_SetAntialiasFlag",
	"GDraw_SetDashAdjustmentFlag",
	"GDraw_SetHintingFlag",
	"GDraw_SetFlatness",
	"GDraw_SetMiterLimit",
	"GDraw_FillRectangle",
	"GDraw_FillPoint",
	"GDraw_FillPath",
	"GDraw_InitialiseFillPath",
	"GDraw_FillPathLine",
	"GDraw_DoFillPath",
	"GDraw_FillTriangle",
	"GDraw_HintPath",
	"GDraw_StrokePath",
	"GDraw_StrokePathToPath",
	"GDraw_CalcStrokeBBox",
	"GDraw_IsOverlap",
	"GDraw_IsStrokeOverlap",
	"GDraw_GetStatistics",
	"GDraw_MakeRegion",
	"GDraw_MakeUnclippedRegion",
	"GDraw_FillRegion",
	"GDraw_ClipRectangle",
	"GDraw_DeviceClipRectangle",
	"GDraw_GetDeviceClipRectangle",
	"GDraw_ClipRegion",
	"GDraw_GetClipRegion",
	"GDraw_CalcSimpleBBox",
	"GDraw_CalcBBox",
	"GDraw_TransformPath",
	"GDraw_ScrollBitmap",
	"GDraw_ClearChangedBBox",
	"GDraw_GetChangedBBox",
	"GDraw_SetChangedBBox",
	"GDraw_SetBevelContrast ",
	"GDraw_SetBevelLightness",
	"GDraw_SetBevelDarkness ",
	"GDraw_TranslateBevelValue",
	"GSprite_PlotTile",
	"GSprite_PlotTile4",
	"G3D_SetTruePerspectiveFlag",
	"G3D_DefineView",
	"G3D_DefineTexture",
	"G3D_Define2ndTexture",
	"G3D_ConvertColourFormat",
	"G3D_AddTriangleToView",
	"G3D_AddFlatTriangleToView",
	"G3D_AddTextureToView",
	"G3D_AddFlatTextureToView",
	"G3D_AddTexturesToView",
	"G3D_AddFlatTexturesToView",
	"G3D_SetSideFaceFlag",
	"G3D_GetViewBBox",
	"G3D_PlotView",
	"GDraw_SetAntialiasQualityFlag"
};

XA_DRAW_FNS g_XaFns =
{
	XaDrawDbg_GetVersion,
	XaDrawDbg_ClearLastError,
	XaDrawDbg_GetLastError,
	XaDrawDbg_GetLastErrorText,
	XaDrawDbg_Initialise,
	XaDrawDbg_CopyContext,
	XaColourDbg_GetGraduationTableSize,
	XaColourDbg_GetGraduationTableLength,
	XaColourDbg_GetLongGraduationTableSize,
	XaColourDbg_GetLongGraduationTableLength,
	XaColourDbg_GetTransparentGraduationTableSize,
	XaColourDbg_GetTransparentGraduationTableLength,
	XaColourDbg_GetLongTransparentGraduationTableSize,
	XaColourDbg_GetLongTransparentGraduationTableLength,
	XaDrawDbg_SetMemoryHandlers,
	XaDrawDbg_SetStackLimit,
	XaDrawDbg_SetStackSize,
	XaDrawDbg_Terminate,
	XaDrawDbg_Clean,
	XaDrawDbg_ContextLength,
	XaDrawDbg_ComparePalettes,
	XaColourDbg_SelectPalette,
	XaColourDbg_InitialiseWithPalette,
	XaColourDbg_SetDitherStyle,
	XaColourDbg_SetTileSmoothingFlag,
	XaColourDbg_SetTileFilteringFlag,
	XaColourDbg_Sharpen,
	XaColourDbg_Blur   ,
	XaColourDbg_SetHalftoneOrigin,
	XaColourDbg_SetColour,
	XaColourDbg_SetSolidColour,
	XaColourDbg_SetWordColour,
	XaColourDbg_SetInvert,
	XaColourDbg_SetTransparency,
	XaColourDbg_SetTransparencyLookupTable,
	XaColourDbg_ReturnBrush,
	XaColourDbg_ReturnBrushRGB,
	XaColourDbg_AddToGraduationTable,
	XaColourDbg_BuildGraduationTable,
	XaColourDbg_BuildGraduationTable32,
	XaColourDbg_BuildTransparencyTable,
	XaColourDbg_SetGraduation,
	XaColourDbg_SetGraduation4,
	XaColourDbg_Set3WayGraduation,
	XaColourDbg_Set3WayGraduation4,
	XaColourDbg_Set4WayGraduation,
	XaColourDbg_Set4WayGraduation4,
	XaColourDbg_SetGourand,
	XaColourDbg_SetTransparentGraduation,
	XaColourDbg_SetTransparentGraduation4,
	XaColourDbg_SetTransparent3WayGraduation,
	XaColourDbg_SetTransparent3WayGraduation4,
	XaColourDbg_SetTransparent4WayGraduation,
	XaColourDbg_SetTransparent4WayGraduation4,
	XaBitmapDbg_SetBias,
	XaBitmapDbg_SetGain,
	XaBitmapDbg_SetContone,
	XaBitmapDbg_SetTransparencyRamp,
	XaColourDbg_SetTilePattern,
	XaColourDbg_SetTilePattern4,
	XaColourDbg_SetTransparentTilePattern,
	XaColourDbg_SetTransparentTilePattern4,
	XaColourDbg_ConvertBitmap,
	XaColourDbg_SetConversionPalette,
	XaColourDbg_SetMaxDiffusionError,
	XaColourDbg_ScaleBitmap,
	XaColourDbg_SetGreyConversionValues,
	XaColourDbg_ConvertHSVtoRGB,
	XaColourDbg_ConvertRGBtoHSV,
	XaColourDbg_SetSeparationTables,
	XaColourDbg_SetBitmapConversionTable,
	XaDrawDbg_SetDIBitmap,
	XaDrawDbg_SetInvertedDIBitmap,
	XaDrawDbg_SetMatrix,
	XaDrawDbg_MaxScale ,
	XaDrawDbg_GetMaxBitmapWidth,
	XaDrawDbg_GetMaxBitmapDepth,
	XaDrawDbg_SetAntialiasFlag,
	XaDrawDbg_SetDashAdjustmentFlag,
	XaDrawDbg_SetHintingFlag,
	XaDrawDbg_SetFlatness,
	XaDrawDbg_SetMiterLimit,
	XaDrawDbg_FillRectangle,
	XaDrawDbg_FillPoint,
	XaDrawDbg_FillPath,
	XaDrawDbg_InitialiseFillPath,
	XaDrawDbg_FillPathLine,
	XaDrawDbg_DoFillPath,
	XaDrawDbg_FillTriangle,
	XaDrawDbg_HintPath,
	XaDrawDbg_StrokePath,
	XaDrawDbg_StrokePathToPath,
	XaDrawDbg_CalcStrokeBBox,
	XaDrawDbg_IsOverlap,
	XaDrawDbg_IsStrokeOverlap,
	XaDrawDbg_GetStatistics,
	XaDrawDbg_MakeRegion,
	XaDrawDbg_MakeUnclippedRegion,
	XaDrawDbg_FillRegion,
	XaDrawDbg_ClipRectangle,
	XaDrawDbg_DeviceClipRectangle,
	XaDrawDbg_GetDeviceClipRectangle,
	XaDrawDbg_ClipRegion,
	XaDrawDbg_GetClipRegion,
	XaDrawDbg_CalcSimpleBBox,
	XaDrawDbg_CalcBBox,
	XaDrawDbg_TransformPath,
	XaDrawDbg_ScrollBitmap,
	XaDrawDbg_ClearChangedBBox,
	XaDrawDbg_GetChangedBBox,
	XaDrawDbg_SetChangedBBox,
	XaDrawDbg_SetBevelContrast ,
	XaDrawDbg_SetBevelLightness,
	XaDrawDbg_SetBevelDarkness ,
	XaDrawDbg_TranslateBevelValue,
	XaSpriteDbg_PlotTile,
	XaSpriteDbg_PlotTile4,
	Xa3DDbg_SetTruePerspectiveFlag,
	Xa3DDbg_DefineView,
	Xa3DDbg_DefineTexture,
	Xa3DDbg_Define2ndTexture,
	Xa3DDbg_ConvertColourFormat,
	Xa3DDbg_AddTriangleToView,
	Xa3DDbg_AddFlatTriangleToView,
	Xa3DDbg_AddTextureToView,
	Xa3DDbg_AddFlatTextureToView,
	Xa3DDbg_AddTexturesToView,
	Xa3DDbg_AddFlatTexturesToView,
	Xa3DDbg_SetSideFaceFlag,
	Xa3DDbg_GetViewBBox,
	Xa3DDbg_PlotView,
	XaDrawDbg_SetAntialiasQualityFlag
};

#endif // CONFIG_DEBUG_XADRAW
