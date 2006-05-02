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

// contains the general initialisation and deinitialisation of the kernel

/*
*/

#include "camtypes.h"

//#include "clipext.h"
#include "clipint.h"
#include "docview.h"
#include "tool.h"
#include "module.h"
#include "bfxbase.h"
#include "printing.h"
#include "csrstack.h"
//#include "ctrlhelp.h"
//#include "fonts.h"
#include "hotkeys.h"
#include "fntcache.h"
#include "sginit.h"
#include "sgliboil.h"
#include "app.h"
//#include "oilmenus.h"
//#include "oilruler.h"
#include "layer.h"
//#include "atminst.h"
#include "progress.h"
//#include "registry.h"
#include "scunit.h"
//#include "prnmks.h"
#include "valfunc.h"
//#include "grndbmp.h"
#include "strkcomp.h"
#include "brshcomp.h"
//#include "dlgcthlp.h"
//#include "xpehost.h"
#include "opfeathr.h"
#include "appprefs.h"

BOOL InitPreTools();
BOOL InitPostTools1();
BOOL InitPostTools2();

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

DECLARE_SOURCE("$Revision$");


/*******************************************************************

>	BOOL InitKernel()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/93
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked OK, FALSE if failed (program will not start)
	Purpose:	Calls all Initxxx functions for everyone else
	Errors:		None

*******************************************************************/

BOOL InitKernel()
{
	if (InitPreTools() &&
	
		Tool::InitTools() &&			   	// Init Modules and tools (keep last) 

		InitPostTools1() &&
		InitPostTools2()

		)					
		return TRUE;
	else
	{
		InformError();
		TRACE( _T("InitKernel failed - will exit very soon\n"));
		return FALSE;
	}
}

/*******************************************************************

>	BOOL InitKernelPostSplash()

	Author: 	Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked OK, FALSE if failed (program will not start)
	Purpose:	Initialises various things that should be done after the 
				splash screen has been shown
	Errors:		None

*******************************************************************/

BOOL InitKernelPostSplash()
{
	if (
#if !defined(PROD_XS) && !defined(EXCLUDE_FROM_XARALX)
		// disabled for XS builds
		PluginOILFilter::AutoRegisterFilters() &&
#endif

		Filter::InitFilters()
		)
		return TRUE;
	else
	{
		InformError();
		TRACE( _T("InitKernelPostSplash failed - will exit very soon\n"));
		return FALSE;
	}
}



/*******************************************************************

>	BOOL DeinitKernel()

	Author: 	Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Calls all Deinitxxx functions for everyone else
	Errors:		None

*******************************************************************/

void DeinitKernel()
{
	// Needs to happen before any major systems disappear (namely the message handler system)
	Layer::Deinit();

// Neville 5/8/97 remove Stroking code
#ifdef VECTOR_STROKING
	// Deinit ValueFunction classes
	ValueFunction::DeInit();
#endif

PORTNOTE("extClipboard", "Do no call ExternalClipboard::Deinit")
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ExternalClipboard::Deinit();					// Destroy clipboard manager
//	InternalClipboard::Deinit();					// Destroy internal clipboard cache
#else
	InternalClipboard::Deinit();					// Destroy internal clipboard cache
#endif

    // Remove all the OpDescriptors from their internal static list
	OpDescriptor::DestroyAll();
	MessageHandler::Destroy(); // Remove MessageHandler class lists

	AttributeManager::Deinit();

#if !defined(EXCLUDE_FROM_RALPH)
	#ifndef STANDALONE
		// Write out the preferences, only if the user has requested this
		if ( AppPrefsDlg::IsSaveOnExitSet() )
			Camelot.WritePreferences();
	#else
		Camelot.WritePreferences();
	#endif
#else
	Camelot.WritePreferences();
#endif

PORTNOTE("other","Removed XPEHost usage")
#ifndef EXCLUDE_FROM_XARALX
	XPEEditOp::DeInit();
	XPEHost::EndXPE3Connection();
#endif

	// Always de-init the tools before the modules!
	Tool::DeinitTools();		// NB. tool function is also called prior to this
	Module::DeinitModules();	// by CMainFrame::OnDestroy() !!

#ifndef EXCLUDE_BFX
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	BitmapEffectBase::DeInit();
#endif
#endif
//WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
#ifndef STANDALONE
	// no printing on Viewer version 
	// defined as a blank nothing function
	OpPrint::Deinitialise();
#endif
#endif	// WEBSTER

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#ifndef STANDALONE
	// Make sure we throw away all marks before the memory manager is deinitialised
	if (Camelot.GetMarksManager())
		Camelot.GetMarksManager()->InvalidateCache();
#endif
#endif

// Neville 5/8/97 remove Stroking code
#ifdef VECTOR_STROKING
	// Make sure any Stroke provider stuff is deinitialised
	StrokeComponentClass::DeInit();
#endif
	// get rid of any stored brushes
	BrushComponentClass::DeInit();

	OpChangeFeatherSize::DeInit();
	
	DeinitMemory(); 					// Must be before deinithandles, as it it uses handles
	DeinitHandles();
//	DeinitHotKeySystem();

	CursorStack::DeInit();

#if !defined(EXCLUDE_FROM_RALPH)
	Progress::Deinit();
	StandardUnit::DeInit();
#endif

#ifndef STANDALONE
	AppPrefsDlg::Deinit();				// Deinit the application preference/options dialog
#endif
	Filter::DeinitFilters();
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ControlHelper::DeInit();
	DialogControlHelper::DeInit();
#endif
PORTNOTE("other","Removed FontFactory usage")
#ifndef EXCLUDE_FROM_XARALX
	FontFactory::Deinit();
#endif
PORTNOTE("other","Removed ATMInstall usage")
#ifndef EXCLUDE_FROM_XARALX
	ATMInstall::AliasList.DeInit();
	DeinitButtonBar();
#endif
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	DeinitMenuSystem();
#endif
	DimScale::Deinit();
	Unit::Deinit();
	DocUnitList::Deinit();
	GlobalBitmapList::DeInit();
	GlobalFractalList::DeInit();
	OILBitmap::Deinit();
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	HotKey::Deinit(); 
#endif
#ifndef DISABLE_TEXT_RENDERING
	FontCache::DeInit();
#endif
#ifndef EXCLUDE_GALS
	SGInit::DeInit();		// De-init the galleries
#endif
	View::Deinit();
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	SGLibOil::TidyUp();
// WEBSTER - markn 15/1/97
// No rulers in Webster
#ifndef WEBSTER
	OILRuler::Deinit();
#endif
//	DeInitRegistry();
#endif

	// Deinit static rendering stuff
PORTNOTE("other","Removed GRenderBitmap usage")
#ifndef EXCLUDE_FROM_XARALX
	GRenderBitmap::Deinit();
#endif

	// Close down the kernel application object (including preferences system)
	Camelot.Deinit();

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// do this last now as we use the key to save out some preference data
	DeInitRegistry();
#endif
}
