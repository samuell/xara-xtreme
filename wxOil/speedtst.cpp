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

// operation(s) to test Camelot speed

/*
*/

#include "camtypes.h"
//#include "camconfig.h"
#include "speedtst.h"
#include "docview.h"
#include "convert.h"

//#include "andy.h"
//#include "will.h"
//#include "richard2.h"
//#include "phil.h"
//#include "resource.h"
#include "camprofile.h"

/*
#include "errors.h"
#include <time.h>
#include "mainfrm.h"
#include "app.h"
#include "rgnlist.h"

#include "xadraw.h"
#include "zoomops.h"
#include "wrkrect.h"
#include "bitmapcache.h"
*/

CC_IMPLEMENT_DYNCREATE(OpTimeDraw, Operation)
CC_IMPLEMENT_DYNCREATE(OpClearCache, Operation)
CC_IMPLEMENT_DYNCREATE(OpThrottleCache, Operation)

#define new CAM_DEBUG_NEW


// This global var is incremented every time the speed test op is executed.
// It was put in so I could test the speed increase of the path point caching inside 
// the NodeBlendPath object. When SpeedTest_Count was even I checked the cache, 
// when odd I ignored it
// I've left it in as I'm sure others will find it useful
// Markn - 14-5-99

//INT32 SpeedTest_Count = 0;

/********************************************************************************************

>	BOOL OpTimeDraw::Init()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for time testing
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpTimeDraw::Init()
{
	BOOL bOK = TRUE;
	if (bOK) bOK = RegisterOpDescriptor(
										0,
										_R(IDS_TIMEDRAW),
										CC_RUNTIME_CLASS(OpTimeDraw),
										OPTOKEN_TIMEDRAW,
										GetState,
										_R(IDS_TIMEDRAW),
										_R(IDBBL_TIMEDRAW)
									  );

	if (bOK) bOK = RegisterOpDescriptor(
										0,
										_R(IDS_CLEARCACHE),
										CC_RUNTIME_CLASS(OpClearCache),
										OPTOKEN_CLEARCACHE,
										GetState,
										_R(IDS_CLEARCACHE),
										_R(IDBBL_CLEARCACHE)
									  );

	if (bOK) bOK = RegisterOpDescriptor(
										0,
										_R(IDS_THROTTLECACHE),
										CC_RUNTIME_CLASS(OpThrottleCache),
										OPTOKEN_THROTTLECACHE,
										GetState,
										_R(IDS_THROTTLECACHE),
										_R(IDBBL_THROTTLECACHE)
									  );

	return bOK;
}

/********************************************************************************************

>	void OpTimeDraw::Do(OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Forces a redraw of the current document and displays the time taken to do
	Errors:		None

********************************************************************************************/

#ifdef CONFIG_DEBUG_XADRAW
INT32 _cdecl XaTimingSortFunc(const void *arg1, const void *arg2)
{
	DWORD *p1 = (DWORD*)arg1;
	DWORD *p2 = (DWORD*)arg2;
	if (g_XaTimes[*p2].QuadPart - g_XaTimes[*p1].QuadPart)
		return((INT32)(g_XaTimes[*p2].QuadPart - g_XaTimes[*p1].QuadPart));
	else
		return((INT32)g_XaCalls[*p2] - (INT32)g_XaCalls[*p1]);
}
#endif

void OpTimeDraw::Do(OpDescriptor*)
{
//	SpeedTest_Count++;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView == NULL)
		return;

	wxYield(); // let through some wxPaint events

	CamProfiler Profiler;
	// clock() measures system time so excludes (e.g.) X-Windows blit time
	// so this is rather a peculiar test
	clock_t Timer = clock();											// start clock

	pDocView->ForceRedraw();											// posts the paint message

	wxWindow* pWnd = DocView::GetCurrentRenderWindow();
	if (pWnd)
		pWnd->Update();											// do the paint

	// now we wait until all rendering has finished
	while ( Camelot.ServiceRendering() )
	{
	}

	Timer = clock() - Timer;											// stop clock

	double Results[CAMPROFILE_NONE];
	double total = Profiler.Read(Results);

	// as this is for our use only, it is permitted to hard-code English strings

	// this lovely line translates the clock_t value into a useful value without using
	// floats
	String_256 Buf; // "Redraw took %d.%03d secs"

	Buf.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK), (INT32)(Timer / CLOCKS_PER_SEC),
								(INT32)((Timer % CLOCKS_PER_SEC) * 1000 / CLOCKS_PER_SEC) );
#if 1

	// All this because MakeMsg doesn't do floating point
#define PFORM(x) ((INT32)((x<0)?ceil(x):floor(x))) , ((INT32)(floor(0.5+( ( (x<0)?(ceil(x)-x):(x - floor(x)) ) * 1000 ))))

	double error=Results[CAMPROFILE_OTHER ]+
				 Results[CAMPROFILE_GDRAW ]+
				 Results[CAMPROFILE_SHADOW]+
				 Results[CAMPROFILE_BEVEL ]+
				 Results[CAMPROFILE_BLIT  ]-total;

	String_256 Buf2;
	
	Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "TOTAL", PFORM(total));
	Buf += _T(", ");
	Buf += Buf2;
	Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "GDRAW", PFORM(Results[CAMPROFILE_GDRAW]));
	Buf += _T(", ");
	Buf += Buf2;
	Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "SHADOW", PFORM(Results[CAMPROFILE_SHADOW]));
	Buf += _T(", ");
	Buf += Buf2;
	Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "BEVEL", PFORM(Results[CAMPROFILE_BEVEL]));
	Buf += _T(", ");
	Buf += Buf2;
	Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "BLIT", PFORM(Results[CAMPROFILE_BLIT]));
	Buf += _T(", ");
	Buf += Buf2;
	Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "OTHER", PFORM(Results[CAMPROFILE_OTHER]));
	Buf += _T(", ");
	Buf += Buf2;
	
	// Only report error if it is significant. With current code, it shouldn't actually exist at all bar
	// floating point to INT32 rounding error.
	if (fabs(error) > 0.5/1000.0)
	{
		Buf2.MakeMsg(_R(IDS_SPEEDTST_REDRAW_TOOK2), "ERROR", PFORM(error));
		Buf += _T(", ");
		Buf += Buf2;
	}

#endif
	Convert::ReplaceDecimalPoint(&Buf);									// Just to keep Neville happy... :-


	// URGH! This should use the proper Camelot dialog box system but we don't have it available right now
	::wxMessageBox( (TCHAR *)Buf );

	End();																// cleanup memory	
}
/********************************************************************************************

>	OpTimeDraw::OpTimeDraw()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpTimeDraw operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpTimeDraw::OpTimeDraw()
{
}


/********************************************************************************************

>	OpState OpTimeDraw::GetState(String_256*, OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpTimeDraw::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	return OpSt;
}

/********************************************************************************************

>	void OpClearCache::Do(OpDescriptor*)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/2004
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles bitmap cache enabled pref also Clears the bitmap cache for performance testing
	Errors:		None

********************************************************************************************/

void OpClearCache::Do(OpDescriptor*)
{
	if (NodeRenderableBounded::bEnableCacheing)
	{
		InformWarning(_R(IDS_WARN_DISABLECACHEING), _R(IDS_OK));
	}

	NodeRenderableBounded::bEnableCacheing = ! NodeRenderableBounded::bEnableCacheing;

	CBitmapCache* pBitmapCache = Camelot.GetBitmapCache();
	if (pBitmapCache)
	{
		pBitmapCache->DeInitialise();
	}

	End();																// cleanup memory	
}

/********************************************************************************************

>	OpClearCache::OpClearCache()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpTimeDraw operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpClearCache::OpClearCache()
{
}


/********************************************************************************************

>	OpState OpClearCache::GetState(String_256*, OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpClearCache::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	OpSt.Ticked = NodeRenderableBounded::bEnableCacheing;

	return OpSt;
}




/********************************************************************************************

>	void OpThrottleCache::Do(OpDescriptor*)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/2004
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles bitmap cache enabled pref also Clears the bitmap cache for performance testing
	Errors:		None

********************************************************************************************/

void OpThrottleCache::Do(OpDescriptor*)
{
	if (NodeRenderableBounded::bThrottleCacheing)
	{
		InformWarning(_R(IDS_WARN_DISABLE_THROTTLING), _R(IDS_OK));
	}
	else
	{
		InformWarning(_R(IDS_WARN_ENABLE_THROTTLING), _R(IDS_OK));
	}

	NodeRenderableBounded::bThrottleCacheing = ! NodeRenderableBounded::bThrottleCacheing;

	End();																// cleanup memory	
}

/********************************************************************************************

>	OpThrottleCache::OpThrottleCache()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpTimeDraw operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpThrottleCache::OpThrottleCache()
{
}


/********************************************************************************************

>	OpState OpThrottleCache::GetState(String_256*, OpDescriptor*)

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpThrottleCache::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	OpSt.Ticked = NodeRenderableBounded::bThrottleCacheing;

	return OpSt;
}




