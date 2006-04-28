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
// Contains general initialisation of the kernel, after InitPostTools1() is called

/*
*/

#include "camtypes.h"
#include "selall.h"
#include "keypress.h"
#include "scunit.h"
#include "finfodlg.h"
#include "aligndlg.h"
#include "bmpprefs.h"
#include "renddlg.h"
#include "tracedlg.h"
#include "appprefs.h"
//#include "coldlog.h"
#include "opnudge.h"
#include "combshps.h"
#include "colcomp.h"
#include "bmpcomp.h"
#include "princomp.h"
#include "nodebmp.h"
//#include "dragcol.h"
//#include "dragbmp.h"
#include "convert.h"
#include "diagnost.h"
#include "rndrgn.h"
#include "osrndrgn.h"
#include "ccobject.h"
//#include "scrcamvw.h"
#include "docview.h"
#include "document.h"
#include "clikmods.h"
#include "transop.h"
#include "attrmgr.h"
//#include "ctrlhelp.h"
//#include "fonts.h"
#include "units.h"
#include "menuops.h"
#include "hotkeys.h"
//#include "oilkeys.h"
//#include "oilbtns.h"
#include "progress.h"
#include "oilmenus.h"
#include "clipint.h"
#include "fillattr.h"
#include "fontman.h"
#include "fntcache.h"
#include "pathops.h"
#include "tunemem.h"
#include "epsfiltr.h"
//#include "prncamvw.h"
//#include "oilruler.h"
//#include "registry.h"
#include "appstate.h"
//#include "basebar.h"
//#include "makebmp.h"
#include "ccpanose.h"
#include "unitcomp.h"	// UnitListComponent
#include "infocomp.h"	// DocInfoComponent
#include "viewcomp.h"
#include "fontcomp.h"
#include "prnmkcom.h"
#include "hlinkdlg.h"
#include "strkcomp.h"
#include "tmpltdlg.h"
#include "cxfrech.h"
#include "styles.h"
#include "brshcomp.h"
//#include "dlgcthlp.h"
//#include "xpehost.h"
#include "editsel.h"

// Normal exe's and standalones
BOOL InitPostTools2()
{
	return (
			SelectAllOp::Init() &&				// Init the SelectAll op
			SelectNoneOp::Init() &&				// Init the SelectNone op
#ifndef EXCLUDE_FROM_XARALX
PORTNOTE("other","Removed a large amount of initialization")
			XPEEditOp::Init() &&				// Init the XPE Edit operation
			EditSelectionOp::Init() &&			// Init the "Edit Selection" op
			KeyPress::Init() &&					// Init the key press system
			FileInfo::Init() &&					// Init the document info dialogue
#endif
			ArrangeAlignment::Init() &&			// Init the arrange alignment dialogue
			OpAlign::Init() &&					// Init the align op
#ifndef EXCLUDE_FROM_XARALX
			BmpPrefsDlg::Init() &&				// Init the export bitmap options dialog
			TraceDlg::Init() &&					// Init the trace dialog
			WebAddressDlg::Init() &&
			PhotoCDDlg::Init() &&				// Init the import PhotoCD options dialog
#endif
			RenderDemoDlg::Init() &&			// Init the Render Demo Dialog
#ifndef EXCLUDE_FROM_XARALX
			AppPrefsDlg::Init() &&				// Init the application preference/options dialog
			TemplateDialog::Init() &&
			DocPrefsDlg::Init() &&				// Init the document preference/options dialog

			StandardUnit::Init() &&				// create some standard scale units (scunit.h)
			ColourEditDlg::Init() &&			// Init the colour editor dialogue
			OpConvertToBitmap::Init() &&
#endif
			OpNudge::Init() &&					// Init the nudge ops
			OpCombineShapes::Init() &&			// init the shape combining ops

			OpMenuSelectPathPoints::Init() &&
			OpCreateNodeBitmap::Init() &&

			ColourListComponentClass::Init() && // Init & register the colour list component
			BitmapListComponentClass::Init() && // Init & register the bitmap list component
			UnitListComponentClass::Init()	 && // Init & register the units component
			DocInfoComponentClass::Init()	 && // Init & register the document information component
		 	WizOpStyleComponentClass::Init() && // Init & register the styles component

			PrintComponentClass::Init()		 && // Init & register the print component
			PrintMarkComponentClass::Init()	 && // Init & register the print marks component
			ViewComponentClass::Init()		 && // Init & register the view component
			FontComponentClass::Init()		 && // Init & register the font component

// Neville 5/8/97 remove Stroking code
#ifdef VECTOR_STROKING
			StrokeComponentClass::Init()	 && // Init & register the stroke component
#endif // VECTOR_STROKING

			BrushComponentClass::Init()      && // Init & register the brush component
			RenderRegion::InitPrefs() &&		// Get the render region prefs from the .INI
			OSRenderRegion::Init() &&			// init GDI preferences
			SimpleCCObject::Init() &&			// Init the Memory tracking preferences
			ClickModifiers::DeclarePrefs() &&
			DocView::DeclarePreferences() &&	// Deals with DocViews preferences
			Document::ReadPrefs() &&			// read in class Document's preferences
			Convert::Init() &&					// Read number of decimal places, decimal point
												// and thousands seperator characters from
												// operating system .
PORTNOTE("other","Removed more initialization")
#ifndef EXCLUDE_FROM_XARALX
			ColourDragInformation::Init() &&
			BitmapDragInformation::Init() &&
			InitDiagnosticPrefs() &&			// Get the diagnostic preferences
			
			CCamView::ReadViewPrefs() &&
			PrintMonitor::InitPrefs() &&

			BaseBar::DeclarePreferences() &&

			TransOperation::DeclarePrefs() &&
#endif
			AttributeManager::InitDefaults() &&
PORTNOTE("other","Removed yet more initialization")
#ifndef EXCLUDE_FROM_XARALX
			ControlHelper::Init() &&
			DialogControlHelper::Init() &&
			FontFactory::Init() &&

			// The next three inits must be performed in this order
#endif
			Unit::Init() &&						// init the unit class and default units
			DocUnitList::Init() &&				// Init the doc unit list system
			DimScale::Init()  &&					// Init the dimension scaling system
			OpToolSelect::Init() &&				// Register Tool Selection operation


PORTNOTE("other","Removed even more initialization")
#ifndef EXCLUDE_FROM_XARALX
	        // Must always be initialised after all the tools have been
			ToolBarOp::Init() &&				// Make the bar of tool bitmaps
#endif
			HotKey::Init() &&					// Init hotkey system AFTER the tool op descs have been registered
PORTNOTE("other","Removed even more initialization")
#ifndef EXCLUDE_FROM_XARALX
			InitHotKeySystem() &&				// Initialise Hot Key system
			InitButtonBar() &&					// Initialise ButtonBar
#endif
			Progress::Init() &&					// Initialise progress cursor/bar displays

			InternalClipboard::Initialise()	&&	// Create the application's internal clipboard cache
												// (this is a DOCUMENT, so must be left until fairly late)

			InitMenuSystem() &&					// Initialise Menu system (keep very last)
												// (esp. after all Register'ing has been done)


PORTNOTE("other","Removed another lot of initialization")
#ifndef EXCLUDE_FROM_XARALX
// This has been moved to CCamApp::InitCamelotApp so that it happens after MFC's document templates
// are set up.

												// ===================

			AttrFillGeometry::Init() &&			// Initialise the fill editing operations
			EPSFilter::InitPrefs() &&			// Initialise default font mappings and
												// EPS export prefs
#endif
#ifndef DISABLE_TEXT_RENDERING
			FontCache::Init() &&
#endif
#ifndef EXCLUDE_FROM_XARALX
			TunedMemory::Init()	&&				// declare prefs for the memory manager

			OILRuler::Init() &&					// set the ruler widths depending on .ini font settings
			InitRegistry() &&					// register us with the shell etc
			InitAppState()	&&					// get workspace prefs
#endif
			TRUE
			);
}

