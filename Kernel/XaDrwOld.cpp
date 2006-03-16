// $Id: XaDrwOld.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "xadrwold.h"
#include "gdrawdbg.h"

/***********************************************************************************************

  s_pContext is the global context used to simulate the global context of GDraw V2

***********************************************************************************************/

static GCONTEXT* s_pContext = NULL;

/***********************************************************************************************

  If CONFIG_DEBUG_XADRAW is defined XaDrawEnter and XaDrawLeave is called before and after each
  call to the real GDraw respectively.

***********************************************************************************************/

#ifdef CONFIG_DEBUG_XADRAW

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

#define XA_DRAW_ENTER XaDrawEnter

static VOID XaDrawEnter(const TCHAR* pszFuncName, pcGCONTEXT pContext)
{
	TRACE(TEXT("%s - CTX: %p\n"), pszFuncName, pContext);
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

#define XA_DRAW_LEAVE XaDrawLeave

static VOID XaDrawLeave(pcGCONTEXT pContext)
{
	if (NULL != pContext)
	{
		eError Error = g_XaFnsOriginal.pfnXaDraw_GetLastError(pContext);
		if (GERROR_NO_ERROR != Error)
		{
			TRACE(	TEXT("XaDraw Error: (%i) %s\n"),
					(INT32)Error,
					g_XaFnsOriginal.pfnXaDraw_GetLastErrorText(pContext));
		}
	}
}

#else

/***********************************************************************************************

	If CONFIG_DEBUG_XADRAW is not defined XA_DRAW_ENTER and XA_DRAW_LEAVE expand to nothing.

***********************************************************************************************/

#define XA_DRAW_ENTER(name,ctx)
#define XA_DRAW_LEAVE(ctx)

#endif

/***********************************************************************************************

>	const GCONTEXT* XaDrawOld_GetContext()

	Author:		Derek_Cox (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/2001
	Inputs:		-
	Returns:	const pointer to the global context
	Purpose:	Returns a pointer to the global context. (Useful for making clones of the
				global context).

***********************************************************************************************/

const GCONTEXT* XaDrawOld_GetContext()
{
	return s_pContext;
}

/***********************************************************************************************

	Translations functions to provide the GDraw V2 interface to GDraw V3.
	See GDraw V2 for details about each function.

	Each wrapper:
		1) calls XA_DRAW_ENTER
		2) calls the original GDraw function,
		3) calls XA_DRAW_LEAVE

	Template:
	
	INT32 Xa<catgegory>Old_<fn_name>(INT32 nParam1)
	{
		XA_DRAW_ENTER(TEXT("<fn_name>"), s_pContext);
		INT32 nRetVal = Xa<category>_<fn_name>(s_pContext, nParam1);
		XA_DRAW_LEAVE(s_pContext);
		return nRetVal;
	}

***********************************************************************************************/

DWORD XaDrawOld_GetVersion()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetVersion"), NULL);
	DWORD nRetVal = XaDraw_GetVersion();
	XA_DRAW_LEAVE(NULL);

	if (NULL == s_pContext)
	{
		s_pContext = static_cast<GCONTEXT*>(malloc(XaDraw_ContextLength()));
		memset(s_pContext, 0, XaDraw_ContextLength());
		INT32 nError = XaDraw_Initialise(s_pContext, NULL);
		if (GERROR_NO_ERROR != nError)
			return 0;
	}

	return nRetVal;
}

INT32 XaDrawOld_ClearLastError()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ClearLastError"), s_pContext);
	INT32 nRetVal = XaDraw_ClearLastError(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

eError XaDrawOld_GetLastError()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetLastError"), s_pContext);
	eError nRetVal = XaDraw_GetLastError(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

pcCHAR XaDrawOld_GetLastErrorText()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetLastErrorText"), s_pContext);
	pcCHAR pRetVal = XaDraw_GetLastErrorText(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return pRetVal;
}

INT32 XaColourOld_GetGraduationTableSize()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetGraduationTableSize"), s_pContext);
	INT32 nRetVal = XaColour_GetGraduationTableSize(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_GetGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetGraduationTableLength"), NULL);
	INT32 nRetVal = XaColour_GetGraduationTableLength();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaColourOld_GetLongGraduationTableSize()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetLongGraduationTableSize"), s_pContext);
	INT32 nRetVal = XaColour_GetLongGraduationTableSize(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_GetLongGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetLongGraduationTableLength"), NULL);
	INT32 nRetVal = XaColour_GetLongGraduationTableLength();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaColourOld_GetTransparentGraduationTableSize()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetTransparentGraduationTableSize"), NULL);
	INT32 nRetVal = XaColour_GetTransparentGraduationTableSize();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaColourOld_GetTransparentGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetTransparentGraduationTableLength"), NULL);
	INT32 nRetVal = XaColour_GetTransparentGraduationTableLength();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaColourOld_GetLongTransparentGraduationTableSize()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetLongTransparentGraduationTableSize"), NULL);
	INT32 nRetVal = XaColour_GetLongTransparentGraduationTableSize();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaColourOld_GetLongTransparentGraduationTableLength()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_GetLongTransparentGraduationTableLength"), NULL);
	INT32 nRetVal = XaColour_GetLongTransparentGraduationTableLength();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaDrawOld_IsFPU(BOOL Flag)
{
	Flag = Flag;
	// No longer required
	return GERROR_NO_ERROR;
}

INT32 XaDrawOld_SetMemoryHandlers(
	pBYTE (*MemoryAlloc)( UINT32 Size ),
	void  (*MemoryFree) ( pBYTE Address )
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetMemoryHandlers"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetMemoryHandlers(s_pContext, MemoryAlloc, MemoryFree);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetStackLimit( pcVOID StackLimit )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetStackLimit"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetStackLimit(s_pContext, StackLimit);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetStackSize( UINT32 StackSize )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetStackSize"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetStackSize(s_pContext, StackSize);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_Terminate()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_Terminate"), s_pContext);
	INT32 nRetVal =
		XaDraw_Terminate(s_pContext);
	XA_DRAW_LEAVE(s_pContext);

	if (NULL != s_pContext)
	{
		free(s_pContext);
		s_pContext = NULL;
	}

	return nRetVal;
}

INT32 XaDrawOld_ContextLength()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ContextLength"), NULL);
	INT32 nRetVal =
		XaDraw_ContextLength();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaDrawOld_ComparePalettes( pcGCONTEXT pContext, pcLOGPALETTE Palette,BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ComparePalettes"), s_pContext);
	INT32 nRetVal =
		XaDraw_ComparePalettes(pContext, Palette, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SaveContext(pGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SaveContext"), s_pContext);
	INT32 nRetVal = XaDraw_CopyContext(s_pContext, pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_RestoreContext(pcGCONTEXT pContext)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_RestoreContext"), s_pContext);
	INT32 nRetVal = XaDraw_CopyContext(pContext, s_pContext);
	DWORD* pStyle = (DWORD*)((BYTE*)s_pContext + 0x37C);
	*pStyle = 0;	// STYLE_INVALID
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

pcLOGPALETTE XaColourOld_SelectPalette( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SelectPalette"), s_pContext);
	pcLOGPALETTE pRetVal =
		XaColour_SelectPalette(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return pRetVal;
}

INT32 XaColourOld_InitialiseWithPalette( pcLOGPALETTE Palette )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_InitialiseWithPalette"), s_pContext);
	INT32 nRetVal =
		XaColour_InitialiseWithPalette(s_pContext, Palette);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetDitherStyle( DitherStyle Style /* =DITHER_GREY_ORDERED */)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetDitherStyle"), s_pContext);
	INT32 nRetVal =
		XaColour_SetDitherStyle(s_pContext, Style);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTileSmoothingFlag( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTileSmoothingFlag"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTileSmoothingFlag(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTileFilteringFlag( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTileFilteringFlag"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTileFilteringFlag(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetHalftoneOrigin( INT32 x,INT32 y )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetHalftoneOrigin"), s_pContext);
	INT32 nRetVal =
		XaColour_SetHalftoneOrigin(s_pContext, x, y);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetColour( COLORREF Colour )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetColour"), s_pContext);
	INT32 nRetVal =
		XaColour_SetColour(s_pContext, Colour);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetSolidColour(	COLORREF Colour,
									UINT32 BPP,UINT32 Format16BPP)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetSolidColour"), s_pContext);
	INT32 nRetVal =
		XaColour_SetSolidColour(s_pContext, Colour, BPP, Format16BPP);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetWordColour( DWORD Colour )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetWordColour"), s_pContext);
	INT32 nRetVal =
		XaColour_SetWordColour(s_pContext, Colour);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetInvert( DWORD Colour )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetInvert"), s_pContext);
	INT32 nRetVal =
		XaColour_SetInvert(s_pContext, Colour);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparency( COLORREF Colour,DWORD Style )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparency"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparency(s_pContext, Colour, Style);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparencyLookupTable( pcBYTE Table )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparencyLookupTable"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparencyLookupTable(s_pContext, Table);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

pcBYTE XaColourOld_ReturnBrush()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_ReturnBrush"), s_pContext);
	pcBYTE pRetVal =
		XaColour_ReturnBrush(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return pRetVal;
}

pcBYTE XaColourOld_ReturnBrushRGB()
{
	XA_DRAW_ENTER(TEXT("XaColourOld_ReturnBrushRGB"), s_pContext);
	pcBYTE pRetVal =
		XaColour_ReturnBrushRGB(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return pRetVal;
}

INT32 XaColourOld_AddToGraduationTable(
	COLORREF Colour,
	BOOL HSVFlag,
	pGraduationTable Table,
	UINT32 Index
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_AddToGraduationTable"), s_pContext);
	INT32 nRetVal =
		XaColour_AddToGraduationTable(s_pContext, Colour, HSVFlag,
															Table, Index);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_BuildGraduationTable(
	COLORREF StartColour,
	COLORREF EndColour,
	BOOL HSVFlag,
	pGraduationTable Table
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_BuildGraduationTable"), s_pContext);
	INT32 nRetVal =
		XaColour_BuildGraduationTable(s_pContext, StartColour, EndColour,
															HSVFlag, Table);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_BuildGraduationTable32(
	COLORREF StartColour,
	COLORREF EndColour,
	BOOL HSVFlag,
	pGraduationTable32 Table
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_BuildGraduationTable32"), s_pContext);
	INT32 nRetVal =
		XaColour_BuildGraduationTable32(s_pContext, StartColour, EndColour,
															HSVFlag, Table);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_BuildTransparencyTable(
	BYTE StartColour,
	BYTE EndColour,
	pTransparentGradTable Table
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_BuildTransparencyTable"), s_pContext);
	INT32 nRetVal =
		XaColour_BuildTransparencyTable(s_pContext, StartColour, EndColour,
															Table);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetGraduation(
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetGraduation"), s_pContext);
	INT32 nRetVal =
		XaColour_SetGraduation(s_pContext, Style, Table,
													PointA, PointB, PointC);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetGraduation4(
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetGraduation4"), s_pContext);
	INT32 nRetVal =
		XaColour_SetGraduation4(s_pContext, Style, Table,
													PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetMultiRadial(
	DWORD Style,
	pcGraduationTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
)
{
	return GERROR_UNIMPLEMENTED;
}

INT32 XaColourOld_Set3WayGraduation(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_Set3WayGraduation"), s_pContext);
	INT32 nRetVal =
		XaColour_Set3WayGraduation(	s_pContext, Style,
														ColourA, ColourB, ColourD,
														PointA, PointB, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_Set3WayGraduation4(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB,					  COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_Set3WayGraduation4"), s_pContext);
	INT32 nRetVal =
		XaColour_Set3WayGraduation4(s_pContext, Style,
														ColourA, ColourB, ColourD,
														PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_Set4WayGraduation(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB,					  pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_Set4WayGraduation"), s_pContext);
	INT32 nRetVal =
		XaColour_Set4WayGraduation(	s_pContext, Style,
														ColourA, ColourB, ColourC, ColourD,
														PointA, PointB, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_Set4WayGraduation4(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourC, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointC, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_Set4WayGraduation4"), s_pContext);
	INT32 nRetVal =
		XaColour_Set4WayGraduation4(	s_pContext, Style,
														ColourA, ColourB, ColourC, ColourD,
														PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetGourand(
	DWORD Style,
	COLORREF ColourA, COLORREF ColourB, COLORREF ColourD,
	pcPOINT   PointA, pcPOINT   PointB, pcPOINT   PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetGourand"), s_pContext);
	INT32 nRetVal =
		XaColour_SetGourand(s_pContext, Style,
												ColourA, ColourB, ColourD,
												PointA, PointB, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparentGraduation(
	DWORD Style,
	pcTransparentGradTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparentGraduation"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparentGraduation(	s_pContext, Style, Table,
																PointA, PointB, PointC);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparentGraduation4(
	DWORD Style,
	pcTransparentGradTable Table,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparentGraduation4"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparentGraduation4(	s_pContext, Style, Table,
																PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparent3WayGraduation(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparent3WayGraduation"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparent3WayGraduation(	s_pContext, Style,
																	ValueA, ValueB, ValueD,
																	PointA, PointB, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparent3WayGraduation4(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB,				    BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparent3WayGraduation4"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparent3WayGraduation4(	s_pContext, Style,
																	ValueA, ValueB, ValueD,
																	PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparent4WayGraduation(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB,					pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparent4WayGraduation"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparent4WayGraduation(	s_pContext, Style,
																	ValueA, ValueB, ValueC, ValueD,
																	PointA, PointB,         PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparent4WayGraduation4(
	DWORD Style,
	BYTE	ValueA, BYTE	ValueB, BYTE	ValueC, BYTE	ValueD,
	pcPOINT PointA, pcPOINT PointB, pcPOINT PointC, pcPOINT PointD
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparent4WayGraduation4"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparent4WayGraduation4(	s_pContext, Style,
																	ValueA, ValueB, ValueC, ValueD,
																	PointA, PointB, PointC, PointD);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTilePattern(
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
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTilePattern"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTilePattern(s_pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC,
													DefaultColour,
													TranslationTable,
													RedTranslationTable,
													GreenTranslationTable,
													BlueTranslationTable,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTilePattern4(
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
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTilePattern4"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTilePattern4(s_pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC, PointD,
													DefaultColour,
													TranslationTable,
													RedTranslationTable,
													GreenTranslationTable,
													BlueTranslationTable,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparentTilePattern(
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
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparentTilePattern"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparentTilePattern(s_pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC,
													DefaultColour,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetTransparentTilePattern4(
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
	XA_DRAW_ENTER(TEXT("XaColourOld_SetTransparentTilePattern4"), s_pContext);
	INT32 nRetVal =
		XaColour_SetTransparentTilePattern4(s_pContext,
													BitmapInfo, Bitmap, Style,
													PointA, PointB, PointC, PointD,
													DefaultColour,
													TransparencyTranslationTable,
													TileOffset);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_ConvertBitmap(
	pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
	pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
	DWORD Dither
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_ConvertBitmap"), s_pContext);
	INT32 nRetVal =
		XaColour_ConvertBitmap(	s_pContext,
													SBitmapInfo, SBitmap,
													DBitmapInfo, DBitmap, Dither);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetConversionPalette( pcLOGPALETTE pPalette )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetConversionPalette"), s_pContext);
	INT32 nRetVal =
		XaColour_SetConversionPalette(s_pContext, pPalette);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetMaxDiffusionError( UINT32 MaxError )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetMaxDiffusionError"), s_pContext);
	INT32 nRetVal =
		XaColour_SetMaxDiffusionError(s_pContext, MaxError);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_ScaleBitmap(
	pcBITMAPINFOHEADER SBitmapInfo, pcBYTE SBitmap,
	pcBITMAPINFOHEADER DBitmapInfo,  pBYTE DBitmap,
	UINT32 Channels
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_ScaleBitmap"), s_pContext);
	INT32 nRetVal =
		XaColour_ScaleBitmap(	s_pContext,
													SBitmapInfo, SBitmap,
													DBitmapInfo, DBitmap,
													Channels);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}


INT32 XaColourOld_SetGreyConversionValues( UINT32 Red,UINT32 Green,UINT32 Blue )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetGreyConversionValues"), s_pContext);
	INT32 nRetVal =
		XaColour_SetGreyConversionValues(	s_pContext,
																Red, Green, Blue);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

COLORREF XaColourOld_ConvertHSVtoRGB( COLORREF hsv )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_ConvertHSVtoRGB"), NULL);
	COLORREF rgb =
		XaColour_ConvertHSVtoRGB(hsv);
	XA_DRAW_LEAVE(NULL);
	return rgb;
}

COLORREF XaColourOld_ConvertRGBtoHSV( COLORREF rgb )
{
	XA_DRAW_ENTER(TEXT("XaColourOld_ConvertRGBtoHSV"), NULL);
	COLORREF hsv =
		XaColour_ConvertRGBtoHSV(rgb);
	XA_DRAW_LEAVE(NULL);
	return hsv;
}

INT32 XaColourOld_SetSeparationTables(
	pcBGR    CyanSeparationTable,
	pcBGR MagentaSeparationTable,
	pcBGR  YellowSeparationTable,
	pcBGR   BlackSeparationTable,
	pcBYTE UnderColourRemovalTable,
	pcBYTE BlackGenerationTable
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetSeparationTables"), s_pContext);
	INT32 nRetVal =
		XaColour_SetSeparationTables(	s_pContext,
															CyanSeparationTable,
															MagentaSeparationTable,
															YellowSeparationTable,
															BlackSeparationTable,
															UnderColourRemovalTable,
															BlackGenerationTable);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaColourOld_SetBitmapConversionTable(
	pcBGR BitmapConversionTable
)
{
	XA_DRAW_ENTER(TEXT("XaColourOld_SetBitmapConversionTable"), s_pContext);
	INT32 nRetVal =
		XaColour_SetBitmapConversionTable(	s_pContext,
																BitmapConversionTable);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetDIBitmap(
	pcBITMAPINFOHEADER BitmapInfo,
	pBYTE Bitmap,
	UINT32 Format16BPP /* =FORMAT16BPP_555 */
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetDIBitmap"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetDIBitmap(	s_pContext,
												BitmapInfo, Bitmap, Format16BPP);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetInvertedDIBitmap(
	pcBITMAPINFOHEADER BitmapInfo,
	pBYTE Bitmap,
	UINT32 Format16BPP /* =FORMAT16BPP_555 */
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetInvertedDIBitmap"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetInvertedDIBitmap(	s_pContext,
														BitmapInfo, Bitmap, Format16BPP);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetMatrix( pcGMATRIX Matrix )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetMatrix"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetMatrix(s_pContext, Matrix);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_MaxScale ( pcGMATRIX Matrix,pcRECT BBox )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_MaxScale"), s_pContext);
	INT32 nRetVal =
		XaDraw_MaxScale(s_pContext, Matrix, BBox);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_GetMaxBitmapWidth()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetMaxBitmapWidth"), NULL);
	INT32 nRetVal =
		XaDraw_GetMaxBitmapWidth();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaDrawOld_GetMaxBitmapDepth()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetMaxBitmapDepth"), NULL);
	INT32 nRetVal =
		XaDraw_GetMaxBitmapDepth();
	XA_DRAW_LEAVE(NULL);
	return nRetVal;
}

INT32 XaDrawOld_SetAntialiasFlag( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetAntialiasFlag"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetAntialiasFlag(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetDashAdjustmentFlag( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetDashAdjustmentFlag"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetDashAdjustmentFlag(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetHintingFlag( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetHintingFlag"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetHintingFlag(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetFlatness( UINT32 Flatness )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetFlatness"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetFlatness(s_pContext, Flatness);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetMiterLimit( UINT32 MiterLimit )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetMiterLimit"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetMiterLimit(s_pContext, MiterLimit);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_FillRectangle( pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_FillRectangle"), s_pContext);
	INT32 nRetVal =
		XaDraw_FillRectangle(s_pContext, Rectangle);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_FillPoint( pcPOINT Point )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_FillPoint"), s_pContext);
	INT32 nRetVal =
		XaDraw_FillPoint(s_pContext, Point);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_FillPath(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_FillPath"), s_pContext);
	INT32 nRetVal =
		XaDraw_FillPath(s_pContext, Points, Types, Length, Winding);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_FillTriangle(
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	DWORD Flags
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_FillTriangle"), s_pContext);
	INT32 nRetVal =
		XaDraw_FillTriangle(s_pContext, PointA, PointB, PointC, Flags);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_HintPath(
	pPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	BOOL Close,
	UINT32 LineWidth
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_HintPath"), s_pContext);
	INT32 nRetVal =
		XaDraw_HintPath(s_pContext, Points, Types, Length, Close, LineWidth);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_StrokePath(
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
	XA_DRAW_ENTER(TEXT("XaDrawOld_StrokePath"), s_pContext);
	INT32 nRetVal =
		XaDraw_StrokePath(s_pContext, Points, Types, Length, Close,
												LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_StrokePathToPath(
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
	XA_DRAW_ENTER(TEXT("XaDrawOld_StrokePathToPath"), s_pContext);
	INT32 nRetVal =
		XaDraw_StrokePathToPath(s_pContext,
													IPoints, ITypes, ILength,
													OPoints, OTypes, OLength, Close,
													LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_CalcStrokeBBox(
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
	XA_DRAW_ENTER(TEXT("XaDrawOld_CalcStrokeBBox"), s_pContext);
	INT32 nRetVal =
		XaDraw_CalcStrokeBBox(	s_pContext,
													IPoints, ITypes, ILength,
													Rect, Close,
													LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_IsOverlap(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_IsOverlap"), s_pContext);
	INT32 nRetVal =
		XaDraw_IsOverlap(	s_pContext,
												Points, Types, Length, Winding);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_IsStrokeOverlap(
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
	XA_DRAW_ENTER(TEXT("XaDrawOld_IsStrokeOverlap"), s_pContext);
	INT32 nRetVal =
		XaDraw_IsStrokeOverlap(	s_pContext,
													Points, Types, Length, Close,
													LineWidth, LineCaps, LineJoin, Dash);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_GetStatistics(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pSTATISTICS Stats
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetStatistics"), s_pContext);
	INT32 nRetVal =
		XaDraw_GetStatistics(	s_pContext,
													Points, Types, Length,
													Winding, Stats);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_MakeRegion(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pREGION ORegion,
	UINT32 OLength
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_MakeRegion"), s_pContext);
	INT32 nRetVal =
		XaDraw_MakeRegion(	s_pContext,
												Points, Types, Length,
												Winding,
												ORegion, OLength);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_MakeUnclippedRegion(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	UINT32 Winding,
	pREGION ORegion,
	UINT32 OLength
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_MakeUnclippedRegion"), s_pContext);
	INT32 nRetVal =
		XaDraw_MakeUnclippedRegion(	s_pContext,
														Points, Types, Length,
														Winding,
														ORegion, OLength);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_FillRegion(
	pcREGION Region,
	pcPOINT Offset
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_FillRegion"), s_pContext);
	INT32 nRetVal =
		XaDraw_FillRegion(s_pContext, Region, Offset);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_ClipRectangle( pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ClipRectangle"), s_pContext);
	INT32 nRetVal =
		XaDraw_ClipRectangle(s_pContext, Rectangle);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_DeviceClipRectangle( pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_DeviceClipRectangle"), s_pContext);
	INT32 nRetVal =
		XaDraw_DeviceClipRectangle(s_pContext, Rectangle);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_GetDeviceClipRectangle( pRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetDeviceClipRectangle"), s_pContext);
	INT32 nRetVal =
		XaDraw_GetDeviceClipRectangle(s_pContext, Rectangle);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_ClipRegion( pcREGION Region )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ClipRegion"), s_pContext);
	INT32 nRetVal =
		XaDraw_ClipRegion(s_pContext, Region);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

pcREGION XaDrawOld_GetClipRegion()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetClipRegion"), s_pContext);
	pcREGION pRetVal =
		XaDraw_GetClipRegion(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return pRetVal;
}

INT32 XaDrawOld_CalcBBox(
	pcPOINT Points,
	pcBYTE  Types,
	UINT32 Length,
	pRECT Rect,
	BOOL Flatten
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_CalcBBox"), s_pContext);
	INT32 nRetVal =
		XaDraw_CalcBBox(s_pContext, Points, Types, Length, Rect, Flatten);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_TransformPath(
	pcPOINT IPoints,
	 pPOINT OPoints,
	UINT32 Length,
	pcGMATRIX Matrix
)
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_TransformPath"), s_pContext);
	INT32 nRetVal =
		XaDraw_TransformPath(	s_pContext,
													IPoints, OPoints,
													Length, Matrix);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_ScrollBitmap( INT32 x,INT32 y )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ScrollBitmap"), s_pContext);
	INT32 nRetVal =
		XaDraw_ScrollBitmap(s_pContext, x, y);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_ClearChangedBBox()
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_ClearChangedBBox"), s_pContext);
	INT32 nRetVal =
		XaDraw_ClearChangedBBox(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_GetChangedBBox( pRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_GetChangedBBox"), s_pContext);
	INT32 nRetVal =
		XaDraw_GetChangedBBox(s_pContext, Rectangle);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetChangedBBox( pcRECT Rectangle )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetChangedBBox"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetChangedBBox(s_pContext, Rectangle);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetBevelContrast ( UINT32 Contrast )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetBevelContrast"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetBevelContrast(s_pContext, Contrast);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetBevelLightness( UINT32 Contrast )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetBevelLightness"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetBevelLightness(s_pContext, Contrast);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_SetBevelDarkness ( UINT32 Contrast )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_SetBevelDarkness"), s_pContext);
	INT32 nRetVal =
		XaDraw_SetBevelDarkness(s_pContext, Contrast);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaDrawOld_TranslateBevelValue( BYTE Index, BYTE Colour )
{
	XA_DRAW_ENTER(TEXT("XaDrawOld_TranslateBevelValue"), s_pContext);
	INT32 nRetVal =
		XaDraw_TranslateBevelValue(s_pContext, Index, Colour);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaSpriteOld_PlotTile(
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
	XA_DRAW_ENTER(TEXT("XaSpriteOld_PlotTile"), s_pContext);
	INT32 nRetVal =
		XaSprite_PlotTile(	s_pContext,
												BitmapInfo, Bitmap, Style,
												PointA, PointB, PointC,
												TranslationTable,
												RedTranslationTable,
												GreenTranslationTable,
												BlueTranslationTable,
												TransparencyTranslationTable);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 XaSpriteOld_PlotTile4(
	pcBITMAPINFOHEADER BitmapInfo,
	pcBYTE Bitmap,
	DWORD Style,
	pcPOINT PointA,
	pcPOINT PointB,
	pcPOINT PointC,
	pcPOINT PointD,
	pcBGRT TranslationTable /* =0 */,
	pcBYTE   RedTranslationTable /* =0 */,
	pcBYTE GreenTranslationTable /* =0 */,
	pcBYTE  BlueTranslationTable /* =0 */,
	pcBYTE TransparencyTranslationTable /* =0 */
)
{
	XA_DRAW_ENTER(TEXT("XaSpriteOld_PlotTile4"), s_pContext);
	INT32 nRetVal =
		XaSprite_PlotTile4(	s_pContext,
												BitmapInfo, Bitmap, Style,
												PointA, PointB, PointC, PointD,
												TranslationTable,
												RedTranslationTable,
												GreenTranslationTable,
												BlueTranslationTable,
												TransparencyTranslationTable);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 Xa3DOld_SetTruePerspectiveFlag( BOOL Flag )
{
	XA_DRAW_ENTER(TEXT("Xa3DOld_SetTruePerspectiveFlag"), s_pContext);
	INT32 nRetVal =
		Xa3D_SetTruePerspectiveFlag(s_pContext, Flag);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 Xa3DOld_DefineView(
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
	XA_DRAW_ENTER(TEXT("Xa3DOld_DefineView"), s_pContext);
	INT32 nRetVal =
		Xa3D_DefineView(s_pContext,
											nObserverDistance, Background,
											pTextureInfo, pTextureBitmap, pTextureColours,
											uTextureXOffset, uTextureYOffset,
											pOverlayBitmap,
											pShadowInfo, pShadowBitmap, ShadowColour,
											bTransparent, bAlpha, bTransparentTexture,
											uFade);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 Xa3DOld_AddTriangleToView(
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour   pColourA, pcColour   pColourB, pcColour   pColourC,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3DOld_AddTriangleToView"), s_pContext);
	INT32 nRetVal =
		Xa3D_AddTriangleToView(	s_pContext,
													pPointA, pPointB, pPointC,
													pColourA, pColourB, pColourC,
													bForward);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 Xa3DOld_AddFlatTriangleToView(
	pcFVector3D pPointA, pcFVector3D pPointB, pcFVector3D pPointC,
	pcColour pFlatColour,
	BOOL bForward
)
{
	XA_DRAW_ENTER(TEXT("Xa3DOld_AddFlatTriangleToView"), s_pContext);
	INT32 nRetVal =
		Xa3D_AddFlatTriangleToView(	s_pContext,
														pPointA, pPointB, pPointC,
														pFlatColour, bForward);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 Xa3DOld_GetViewBBox( pRECT pBBox )
{
	XA_DRAW_ENTER(TEXT("Xa3DOld_GetViewBBox"), s_pContext);
	INT32 nRetVal =
		Xa3D_GetViewBBox(s_pContext, pBBox);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}

INT32 Xa3DOld_PlotView()
{
	XA_DRAW_ENTER(TEXT("Xa3DOld_PlotView"), s_pContext);
	INT32 nRetVal =
		Xa3D_PlotView(s_pContext);
	XA_DRAW_LEAVE(s_pContext);
	return nRetVal;
}
