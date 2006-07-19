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
/*

	helptabs.cpp

	Look-up tables used by the help system.

*/

#include "camtypes.h"

#include "helptabs.h"
//#include "xshelpid.h"
//#include "reshlpid.h"
//#include "bfxrc.h"
//#include "prefsdlg.h"
//#include "printdlg.h"

// These files declare all the relevant OpTokens.
#include "cutop.h"
#include "menuops.h"
#include "printing.h"
#include "aboutdlg.h"
#include "ophist.h"
#include "zoomops.h"
#include "linwthop.h"
#include "zordops.h"
#include "groupops.h"
#include "mkshapes.h"
#include "pathedit.h"
#include "layergal.h"
#include "colgal.h"
#include "snapops.h"
#include "speedtst.h"
#include "grndrgn.h"
#include "exceptio.h"
#include "gbrushop.h"
#include "tooldlg.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "selall.h"
#include "dbugtree.h"
#include "pen.h"
#include "renddlg.h"
#include "blobby.h"
#include "filelist.h"
#include "combshps.h"
#include "tracedlg.h"
#include "bfxdlg.h"
#include "textops.h"
#include "oppull.h"
#include "nativeop.h"
#include "coldlog.h"
#include "optsprin.h"
//#include "lyrprop.h"
#include "guides.h"
#include "optsgrid.h"
#include "prpsgds.h"
//#include "optsres.h"
//#include "bmpres.h"
#include "hlinkdlg.h"
//#include "hotlink.h"
#include "urlimp.h"
#include "frameops.h"
#include "aprps.h"
//#include "webster.h"
//#include "gifdlg.h"

// #include "sgbitmap.h"
// #include "sgfonts.h"
// #include "sgcolour.h"
// #include "sgline.h"
// #include "sglcart.h"
// #include "sglfills.h"
// #include "scaledlg.h"
// #include "unitdlg.h"
//#include "accures.h"
//#include "andy.h"
//#include "alex.h"
//#include "ben.h"
//#include "chris.h"
//#include "galstr.h"
//#include "jason.h"
//#include "jim.h"
//#include "justin.h"
//#include "mario.h"
//#include "markn.h"
//#include "mike.h"
//#include "nev.h"
//#include "peter.h"
//#include "phil.h"
//#include "richard.h"
//#include "rik.h"
//#include "simon.h"
//#include "tim.h"
//#include "will.h"
#include "opimgset.h"
#include "makebmp.h"
//#include "filtrres.h"
//#include "markn.h"
//#include "cmxres.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pathops.h"
#include "opbreak.h"
#include "tipsdlg.h"
//#include "ollie.h"
//#include "resimmap.h"
#include "tmpltdlg.h"
//#include "bfxdlg2.h"	// OPTOKEN_BFXDLG_FLIPROTATE ...
//#include "slice.h"
#include "prnprefs.h"
#include "opclip.h"
#include "opfeathr.h"
#include "opcntr.h"
#include "plugmngr.h"
//#include "sliceres.h"
//#include "brdlgres.h"
#include "brshname.h"

//#include "helppath.h"

#ifdef STANDALONE
#include "speedtst.h"
#endif


// Code version.
DECLARE_SOURCE("$Revision$");


// This table associates all the non-common and non-error/warning dialogues in Camelot with
// the appropriate help topics.
DialogHelpTuple atplDialogTopics[] =
{
	{_T("AboutDlg"),						0,								_R(IDS_HELPPATH_Misc_Xara_Ltd)},

// Neville 14/10/97
#ifndef WEBSTER
	{_T("WebPrefsDlg"),						0,								_R(IDS_HELPPATH_Dialog_Web_Export)},
	{_T("NativePrefsDlg"),					0,								_R(IDS_HELPPATH_Dialog_Xar_Export)},
#else
	{_T("WebPrefsDlg"),						0,								_R(IDS_HELPPATH_Dialog_WEB_File_Options)},
#endif

//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("SepsDlg"),							0,								_R(IDS_HELPPATH_Dialog_Separation_Properties)},
#endif	//webster

	// This is the main application options dialog box and all its tabs
	{_T("AppPrefsDlg"),						0,								_R(IDS_HELPPATH_Command_Options)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_VIEW),				_R(IDS_HELPPATH_Command_Options_View)},
#endif	//webster
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_MISC),				_R(IDS_HELPPATH_Command_Options_General)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_PAGE),				_R(IDS_HELPPATH_Command_Options_Page)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_UNITS),				_R(IDS_HELPPATH_Command_Options_Units)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_TUNE),				_R(IDS_HELPPATH_Command_Options_Tuneups)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_SCALE),				_R(IDS_HELPPATH_Command_Options_Scaling)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_POINTERS),			_R(IDS_HELPPATH_Command_Options_Mouse)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_PRINTPRINT),			_R(IDS_HELPPATH_Command_Options_Separations)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_IMAGESETTING),		_R(IDS_HELPPATH_Command_Options_Imagesetter)},
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_PLUGINS),			_R(IDS_HELPPATH_Command_Options_Plugin)},
#endif//webster
	{_T("AppPrefsDlg"),						_R(IDD_OPTSTAB_GRID),				_R(IDS_HELPPATH_Command_Options_Grid_and_Ruler)},

	{_T("ArrangeAlignment"),					0,								_R(IDS_HELPPATH_Command_Alignment)},
#ifndef WEBSTER
// Old forms
//	{_T("BfxDlg"),							0,								_R(IDS_HELPPATH_Command_Bitmap_Effects)},
//	{_T("BfxDlg"),							_R(IDD_BFXDLG_PG1),					_R(IDS_HELPPATH_Command_Bitmap_Effects_Flip_and_Rotate)},
//	{_T("BfxDlg"),							_R(IDD_BFXDLG_PG2),					_R(IDS_HELPPATH_Command_Bitmap_Effects_Resize)},
//	{_T("BfxDlg"),							_R(IDD_BFXDLG_PG3),					_R(IDS_HELPPATH_Command_Bitmap_Effects_Brightness_and_Contrast)},
//	{_T("BfxDlg"),							_R(IDD_BFXDLG_PG4),					_R(IDS_HELPPATH_Command_Bitmap_Effects_Colour_Depth)},
//	{_T("BfxDlg"),							_R(IDD_BFXDLG_PG5),					_R(IDS_HELPPATH_Command_Bitmap_Effects_Special)},
// New forms
	{_T("BfxFlipAndRotateDlg"),				0,								_R(IDS_HELPPATH_Command_Bitmap_Effects_Flip_and_Rotate)},
	{_T("BfxResizeDlg"),						0,								_R(IDS_HELPPATH_Command_Bitmap_Effects_Resize)},
	{_T("BfxBrightnessContrastDlg"),			0,								_R(IDS_HELPPATH_Command_Bitmap_Effects_Brightness_and_Contrast)},
	{_T("BfxColourDepthDlg"),				0,								_R(IDS_HELPPATH_Command_Bitmap_Effects_Colour_Depth)},
	{_T("BfxSpecialEffectsDlg"),				0,								_R(IDS_HELPPATH_Command_Bitmap_Effects_Special)},
#endif	//webster
	{_T("BmpPrefsDlg"),						0,								_R(IDS_HELPPATH_Dialog_Bitmap_Export)},
#ifndef WEBSTER
	{_T("CCPrintDialog"), 					0,								_R(IDS_HELPPATH_Command_Print)},
	{_T("CCPrintSetupDialog"),				0,								_R(IDS_HELPPATH_Command_Print_Setup)},
#endif	//webster

	{_T("ColourEditDlg"),					0,								_R(IDS_HELPPATH_Misc_Colour_Editor)},
	{_T("ColourNameDlg"),					0,								_R(IDS_HELPPATH_Misc_Colour_Editor)},

	{_T("CustomizeBarDlg"),					0,								_R(IDS_HELPPATH_Command_Control_Bars)},
#ifndef WEBSTER
	{_T("XSEPSExportOptions"),				0,								_R(IDS_HELPPATH_Dialog_CorelXARA_EPS_Export)},
#endif	//webster

#ifndef EXCLUDE_FROM_RALPH
	{_T("FileInfo"),							0,								_R(IDS_HELPPATH_Command_Document_Info)},
#endif
	{_T("GIFAnimationPropertyTabsDlg"),		0,								_R(IDS_HELPPATH_Dialog_Animation_Properties)},
	{_T("GIFAnimationPropertyTabsDlg"),		_R(IDD_TAB_FRAME_PROPERTIES),		_R(IDS_HELPPATH_Dialog_Animation_Properties_Frame_Properties)},
	{_T("GIFAnimationPropertyTabsDlg"),		_R(IDD_TAB_ANIMATION_PROPERTIES),	_R(IDS_HELPPATH_Dialog_Animation_Properties_Animation_Looping_and_Speed)},
	{_T("GIFAnimationPropertyTabsDlg"),		_R(IDD_TAB_ANIMATION_COLOURS),		_R(IDS_HELPPATH_Dialog_Animation_Properties_Animation_Colors)},
	{_T("GIFAnimationPropertyTabsDlg"),		_R(IDD_TAB_PREVIEW_IN_BROWSER),		_R(IDS_HELPPATH_Dialog_Animation_Properties_Preview_in_Browser)},
					
	// This is the op which is defined so that you can go straight to the grid and rulers tab
	{_T("GridTabAppPrefsDlg"),				0,								_R(IDS_HELPPATH_Command_Options)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_VIEW),				_R(IDS_HELPPATH_Command_Options_View)},
#endif	//webster
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_MISC),				_R(IDS_HELPPATH_Command_Options_General)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PAGE),				_R(IDS_HELPPATH_Command_Options_Page)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_UNITS),				_R(IDS_HELPPATH_Command_Options_Units)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_TUNE),				_R(IDS_HELPPATH_Command_Options_Tuneups)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_SCALE),				_R(IDS_HELPPATH_Command_Options_Scaling)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_POINTERS),			_R(IDS_HELPPATH_Command_Options_Mouse)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PLUGINS),			_R(IDS_HELPPATH_Command_Options_Plugin)},
#endif	//webster
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_GRID),				_R(IDS_HELPPATH_Command_Options_Grid_and_Ruler)},
	{_T("GridTabAppPrefsDlg"),				_R(IDD_OPTSTAB_INTERNET),			_R(IDS_HELPPATH_Command_Options_Internet)},
		
#ifndef WEBSTER
	{_T("GuidePropertiesDlg"),				0,								_R(IDS_HELPPATH_Dialog_Layer_Properties)},
	{_T("GuidePropertiesDlg"),				_R(IDD_TAB_GUIDELINE_PROPERTIES),	_R(IDS_HELPPATH_Dialog_Layer_Properties)},
	{_T("GuidePropertiesDlg"),				_R(IDD_TAB_LAYER_PROPERTIES),		_R(IDS_HELPPATH_Dialog_Layer_Properties)},

	{_T("GuidesPropertiesTab"),				0,								_R(IDS_HELPPATH_Dialog_Guideline_Properties)},

	{_T("GuidelinePropDlg"),					0,								_R(IDS_HELPPATH_Dialog_Guideline_Properties)},
			
	{_T("LayerNameDlg"),						0,								_R(IDS_HELPPATH_Dialog_Name_Layer)},

	{_T("LayerPropertyTabsDlg"),				0,								_R(IDS_HELPPATH_Dialog_Layer_Properties)},
	{_T("LayerPropertyTabsDlg"),				_R(IDD_TAB_GUIDELINE_PROPERTIES),	_R(IDS_HELPPATH_Dialog_Layer_Properties)},
	{_T("LayerPropertyTabsDlg"),				_R(IDD_TAB_LAYER_PROPERTIES),		_R(IDS_HELPPATH_Dialog_Layer_Properties)},
#endif	//webster

	//"NameDialog",						0,								_R(IDS_HELPPATH_Command_Wizard_Properties),

	{_T("ImagemapDlg"),						0,								_R(IDS_HELPPATH_Dialog_Image_Map_Export_Options)},
	{_T("OpenFileDialog"),					0,								_R(IDS_HELPPATH_Command_Open)},
	{_T("SaveFileDialog"),					0,								_R(IDS_HELPPATH_Command_Save_As)},
	{_T("SaveTemplateDialog"),				0,								_R(IDS_HELPPATH_Save_Template_Dialog)},
	{_T("ImportFileDialog"),					0,								_R(IDS_HELPPATH_Command_Import)},
	{_T("ExportFileDialog"),					0,								_R(IDS_HELPPATH_Command_Export)},

	// This is the op which is defined so that you can go straight to the page tab
	{_T("PageTabAppPrefsDlg"),				0,								_R(IDS_HELPPATH_Command_Options)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_VIEW),				_R(IDS_HELPPATH_Command_Options_View)},
#endif	//webster
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_MISC),				_R(IDS_HELPPATH_Command_Options_General)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PAGE),				_R(IDS_HELPPATH_Command_Options_Page)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_UNITS),				_R(IDS_HELPPATH_Command_Options_Units)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_TUNE),				_R(IDS_HELPPATH_Command_Options_Tuneups)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_SCALE),				_R(IDS_HELPPATH_Command_Options_Scaling)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_POINTERS),			_R(IDS_HELPPATH_Command_Options_Mouse)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PLUGINS),			_R(IDS_HELPPATH_Command_Options_Plugin)},
#endif	//webster
	{_T("PageTabAppPrefsDlg"),				_R(IDD_OPTSTAB_GRID),				_R(IDS_HELPPATH_Command_Options_Grid_and_Ruler)},

	{_T("PasteSpecialDlg"),					0,								_R(IDS_HELPPATH_Dialog_Paste_Special)},

	{_T("PreviewDialog"),					0,								_R(IDS_HELPPATH_Command_Preview_All_Frames)},
#ifndef WEBSTER
	{_T("PhotoCDDlg"),						0,								_R(IDS_HELPPATH_Dialog_PhotoCD_Import)},
	
	{_T("PrintPrefsDlg"),					0,								_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("PrintPrefsDlg"),					_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("PrintPrefsDlg"),					_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("PrintPrefsDlg"),					_R(IDD_OPTSTAB_PRINTPRINT),			_R(IDS_HELPPATH_Command_Options_Separations)},
	{_T("PrintPrefsDlg"),					_R(IDD_OPTSTAB_IMAGESETTING),		_R(IDS_HELPPATH_Command_Options_Imagesetter)},

	// This is the op which is defined so that you can go straight to the print tab
	{_T("PrintTabPrintPrefsDlg"),			0,								_R(IDS_HELPPATH_Command_Options_Print_Layout)}, //?????
	{_T("PrintTabPrintPrefsDlg"),			_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("PrintTabPrintPrefsDlg"),			_R(IDD_OPTSTAB_PRINTPRINT),			_R(IDS_HELPPATH_Command_Options_Separations)},
	{_T("PrintTabPrintPrefsDlg"),			_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("PrintTabPrintPrefsDlg"),			_R(IDD_OPTSTAB_IMAGESETTING),		_R(IDS_HELPPATH_Command_Options_Imagesetter)},
	{_T("PrintProblems"),					0,								_R(IDS_HELPPATH_Trouble_Print)},
#endif	//webster

	// This is the op which is defined so that you can go straight to the scale tab
	{_T("ScaleTabAppPrefsDlg"),				0,								_R(IDS_HELPPATH_Command_Options)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_VIEW),				_R(IDS_HELPPATH_Command_Options_View)},
#endif	//webster
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_MISC),				_R(IDS_HELPPATH_Command_Options_General)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PAGE),				_R(IDS_HELPPATH_Command_Options_Page)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_UNITS),				_R(IDS_HELPPATH_Command_Options_Units)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_TUNE),				_R(IDS_HELPPATH_Command_Options_Tuneups)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_SCALE),				_R(IDS_HELPPATH_Command_Options_Scaling)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_POINTERS),			_R(IDS_HELPPATH_Command_Options_Mouse)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PLUGINS),			_R(IDS_HELPPATH_Command_Options_Plugin)},
#endif	//webster
	{_T("ScaleTabAppPrefsDlg"),				_R(IDD_OPTSTAB_GRID),				_R(IDS_HELPPATH_Command_Options_Grid_and_Ruler)},
	
	{_T("SGalleryOptionsDlg"),				0,								_R(IDS_HELPPATH_Dialog_Gallery_Properties)},
	{_T("SGallerySearchDlg"),				0,								_R(IDS_HELPPATH_Dialog_Gallery_Find_Dialog)},
	{_T("SGallerySortDlg"),					0,								_R(IDS_HELPPATH_Dialog_Gallery_Sort_Dialog)},
	{_T("SGalleryAddFolderDlg"),				0,								_R(IDS_HELPPATH_Dialog_Gallery_Add_Folder)},

	//"TemplateDialog",					0,								_R(IDS_HELPPATH_Command_Wizard_Properties),
	{_T("ToolbarDlg"),						0,								_R(IDS_HELPPATH_Command_Control_Bars)},
	{_T("ToolnameDlg"),						0,								_R(IDS_HELPPATH_Command_Control_Bars)},
#ifndef WEBSTER
	{_T("TraceDlg"),							0,								_R(IDS_HELPPATH_Command_Bitmap_Tracer)},
#endif //Webster

	// This is the op which is defined so that you can go straight to the units tab
	{_T("UnitsTabAppPrefsDlg"),				0,								_R(IDS_HELPPATH_Command_Options)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_VIEW),				_R(IDS_HELPPATH_Command_Options_View)},
#endif	//webster
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_MISC),				_R(IDS_HELPPATH_Command_Options_General)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PAGE),				_R(IDS_HELPPATH_Command_Options_Page)},
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_UNITS),				_R(IDS_HELPPATH_Command_Options_Units)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_TUNE),				_R(IDS_HELPPATH_Command_Options_Tuneups)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_SCALE),				_R(IDS_HELPPATH_Command_Options_Scaling)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_POINTERS),			_R(IDS_HELPPATH_Command_Options_Mouse)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTLAYOUT),		_R(IDS_HELPPATH_Command_Options_Print_Layout)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PRINTGENERAL),		_R(IDS_HELPPATH_Command_Options_Output)},
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_PLUGINS),			_R(IDS_HELPPATH_Command_Options_Plugin)},
#endif	//webster
	{_T("UnitsTabAppPrefsDlg"),				_R(IDD_OPTSTAB_GRID),				_R(IDS_HELPPATH_Command_Options_Grid_and_Ruler)},
#ifndef WEBSTER
	{_T("UnitPropertiesDlg"),				0,								_R(IDS_HELPPATH_Dialog_Unit_Properties)},
#endif	//webster

	{_T("URLImportDlg"),						0,								_R(IDS_HELPPATH_Command_Import_from_Web)},
	{_T("WebAddressDlg"),					0,								_R(IDS_HELPPATH_Command_Web_Address)},
	{_T("BmpDlg"),							0,								_R(IDS_HELPPATH_Dialog_Bitmap_Properties)},
	{_T("GIFExportFileDialog"),				0,								_R(IDS_HELPPATH_Command_Export_Animated_GIF)},
	{_T("NameObjectsDlg"),					0,								_R(IDS_HELPPATH_Dialog_Name_Objects)},
	{_T("RenameObjectsDlg"),					0,								_R(IDS_HELPPATH_Dialog_Rename_Objects)},
	{_T("ExtendSetsDlg"),					0,								_R(IDS_HELPPATH_Name_Gallery_Extend_Dialog_Box)},
	{_T("BarCreationDlg"),					0,								_R(IDS_HELPPATH_NavBar_Create_Dialog_Box)},
	{_T("BarStatesDlg"),						0,								_R(IDS_HELPPATH_NavBar_Edit_States_Dialog_Box)},
	{_T("BarRedefineStateDlg"),				0,								_R(IDS_HELPPATH_NavBar_Edit_Design_Dialog_Box)},
	{_T("BrushEditDialog"),					_R(IDD_BRUSHEDITSPACING),			_R(IDS_HELPPATH_Dialog_Edit_Brush_Spacing)},
	{_T("BrushEditDialog"),					_R(IDD_BRUSHEDITOFFSET),			_R(IDS_HELPPATH_Dialog_Edit_Brush_Offset)},
	{_T("BrushEditDialog"),					_R(IDD_BRUSHEDITSCALING),			_R(IDS_HELPPATH_Dialog_Edit_Brush_Scaling)},
	{_T("BrushEditDialog"),					_R(IDD_BRUSHEDITEFFECTS),			_R(IDS_HELPPATH_Dialog_Edit_Brush_Rotation)},
	{_T("BrushEditDialog"),					_R(IDD_BRUSHEDITSEQUENCE),			_R(IDS_HELPPATH_Dialog_Edit_Brush_Sequence)},
	{_T("BrushEditDialog"),					_R(IDD_BRUSHEDITFILL),				_R(IDS_HELPPATH_Dialog_Edit_Brush_Properties)},
	{_T("CInitBrushNameDlg"),				0,								_R(IDS_HELPPATH_How_Adding_Custom_Brush_Strokes)},
	{NULL, 0, 0}
};



// This table holds the help topic associations for all OpDescriptor based actions, such
// as menu items.
OpHelpTuple atplOpTopics[] =
{
	{OPTOKEN_FILE_MENU,						_R(IDS_HELPPATH_Menu_File)},
		{OPTOKEN_FILENEW,					_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_DRAWING,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_ANIMATION,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE1,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE2,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE3,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE4,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE5,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE6,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE7,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE8,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE9,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILENEW_TEMPLATE10,			_R(IDS_HELPPATH_Command_New)},
		{OPTOKEN_FILEOPEN,					_R(IDS_HELPPATH_Command_Open)},
		{OPTOKEN_FILECLOSE, 					_R(IDS_HELPPATH_Command_Close)},
		{OPTOKEN_FILESAVE,					_R(IDS_HELPPATH_Command_Save)},
		{OPTOKEN_FILESAVEAS,					_R(IDS_HELPPATH_Command_Save_As)},
#ifndef WEBSTER
		{OPTOKEN_FILESAVEALL,				_R(IDS_HELPPATH_Command_Save_All)},
		{OPTOKEN_SAVEASDEFAULT,				_R(IDS_HELPPATH_Command_Save_As_Template)},
#endif	//webster
		{_T("Import"),							_R(IDS_HELPPATH_Command_Import)},
		{OPTOKEN_URLIMPORT,					_R(IDS_HELPPATH_Command_Import_from_Web)},
		{_T("Export"),							_R(IDS_HELPPATH_Command_Export)},
		{OPTOKEN_SAVEANIMATEDGIF,			_R(IDS_HELPPATH_Command_Export_Animated_GIF)},
#if !defined(EXCLUDE_FROM_XARALX)
		{OPTOKEN_IMAGESLICE,					_R(IDS_HELPPATH_Command_Slice_Image)},
#endif
#ifndef EXCLUDE_FROM_RALPH
		{OPTOKEN_FILEINFO,  					_R(IDS_HELPPATH_Command_Document_Info)},
#endif
//WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
		{OPTOKEN_PRINT_SETUP,				_R(IDS_HELPPATH_Command_Print_Setup)},
		{OPTOKEN_PRINT,         				_R(IDS_HELPPATH_Command_Print)},
		{OPTOKEN_PRINTOPTIONSDLG,			_R(IDS_HELPPATH_Command_Print_Options)},
#endif	//webster
		{OPTOKEN_MRU_FILE1,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE2,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE3,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE4,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE5,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE6,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE7,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE8,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_MRU_FILE9,					_R(IDS_HELPPATH_Command_1234)},
		{OPTOKEN_EXIT,						_R(IDS_HELPPATH_Command_Exit)},
	
	{OPTOKEN_EDIT_MENU,						_R(IDS_HELPPATH_Menu_Edit)},
		{OPTOKEN_UNDO,						_R(IDS_HELPPATH_Command_Undo)},
		{OPTOKEN_REDO, 						_R(IDS_HELPPATH_Command_Undo)},
		{OPTOKEN_CUT,						_R(IDS_HELPPATH_Command_Cut)},
		{OPTOKEN_COPY,						_R(IDS_HELPPATH_Command_Copy)},
		{OPTOKEN_PASTE,						_R(IDS_HELPPATH_Command_Paste)},
		{OPTOKEN_PASTEATTRIBUTES,			_R(IDS_HELPPATH_Command_Paste_Attributes)},
		{OPTOKEN_DELETE,						_R(IDS_HELPPATH_Command_Delete)},
		{OPTOKEN_EDITSELECTALL,				_R(IDS_HELPPATH_Command_Select_All)},
		{OPTOKEN_EDITSELECTNONE,				_R(IDS_HELPPATH_Command_Clear_Selection)},
		{OPTOKEN_DUPLICATE,					_R(IDS_HELPPATH_Command_Duplicate)},
		{OPTOKEN_EDITCLONE,					_R(IDS_HELPPATH_Command_Clone)},

	{OPTOKEN_ARRANGE_MENU,					_R(IDS_HELPPATH_Menu_Arrange)},
#ifdef WEBSTER
		{OPTOKEN_MOVELAYERINFRONT,			_R(IDS_HELPPATH_Command_Move_to_Next_Frame)},
#else
		{OPTOKEN_MOVELAYERINFRONT,			_R(IDS_HELPPATH_Command_Move_to_Layer_in_Front)},		
#endif
		{OPTOKEN_BRINGTOFRONT,				_R(IDS_HELPPATH_Command_Bring_to_Front)},
		{OPTOKEN_MOVEFORWARDS,				_R(IDS_HELPPATH_Command_Move_Forwards)},
		{OPTOKEN_MOVEBACKWARDS,				_R(IDS_HELPPATH_Command_Move_Backwards)},
		{OPTOKEN_PUTTOBACK,					_R(IDS_HELPPATH_Command_Put_to_Back)},
#ifdef WEBSTER
		{OPTOKEN_MOVELAYERBEHIND,			_R(IDS_HELPPATH_Command_Move_to_Previous_Frame)},
#else
		{OPTOKEN_MOVELAYERBEHIND,			_R(IDS_HELPPATH_Command_Move_to_Layer_Behind)},		
#endif
		{OPTOKEN_GROUP,						_R(IDS_HELPPATH_Command_Group)},
		{OPTOKEN_UNGROUP,					_R(IDS_HELPPATH_Command_Ungroup)},
		{OPTOKEN_GROUPTRANSP,				_R(IDS_HELPPATH_Command_Group_Transparency)},
		{OPTOKEN_UNGROUPTRANSP,				_R(IDS_HELPPATH_Command_Ungroup_Transparency)},
		{OPTOKEN_APPLY_CLIPVIEW,				_R(IDS_HELPPATH_Command_Apply_ClipView)},
		{OPTOKEN_REMOVE_CLIPVIEW,			_R(IDS_HELPPATH_Command_Apply_ClipView)},				// NB.
		{OPTOKEN_ARRANGEALIGNMENT,			_R(IDS_HELPPATH_Command_Alignment)},
#ifndef WEBSTER
		{OPTOKEN_JOINSHAPEOP,				_R(IDS_HELPPATH_Command_Join_Shapes)},
		{OPTOKEN_BREAKSHAPEOP,				_R(IDS_HELPPATH_Command_Break_Shapes)},
#endif	//webster
		{OPTOKEN_COMBINESHAPES,				_R(IDS_HELPPATH_Command_Combine_Shapes)},
			{OPTOKEN_ADDSHAPES,				_R(IDS_HELPPATH_Command_Add_Shapes)},
			{OPTOKEN_SUBTRACTSHAPES,			_R(IDS_HELPPATH_Command_Subtract_Shapes)},
			{OPTOKEN_INTERSECTSHAPES,		_R(IDS_HELPPATH_Command_Intersect_Shapes)},
			{OPTOKEN_SLICESHAPES,			_R(IDS_HELPPATH_Command_Slice_Shapes)},
		{OPTOKEN_MAKE_SHAPES,				_R(IDS_HELPPATH_Command_Convert_to_Shapes)},
		{OPTOKEN_CONVERTPATHTOSHAPES,		_R(IDS_HELPPATH_Command_Convert_Line_to_Shape)},
		{OPTOKEN_CONVERTTOBITMAP,			_R(IDS_HELPPATH_Command_Create_Bitmap_Copy)},
		{OPTOKEN_FITTEXTTOPATH,				_R(IDS_HELPPATH_Command_Fit_Text_To_Curve)},

#ifdef PHOTOSHOPPLUGINS
		{OPTOKEN_PLUGINS_UNDO_MENU,			_R(IDS_HELPPATH_Menu_Plugin)},
		{OPTOKEN_PLUGINS_PARSING,			_R(IDS_HELPPATH_Menu_Plugin)},
		{OPTOKEN_PLUGSDLG,					_R(IDS_HELPPATH_Command_Options_Plugin)},
#endif

	{OPTOKEN_UTILS_MENU,						_R(IDS_HELPPATH_Menu_Utilities)},
		{_T("DisplayColourGallery"),				_R(IDS_HELPPATH_Command_Colour_Gallery)},
#ifndef WEBSTER
		{_T("DisplayLayerGallery"),				_R(IDS_HELPPATH_Command_Layer_Gallery)},
#endif	//Webster
		{_T("DisplayBitmapGallery"),				_R(IDS_HELPPATH_Command_Bitmap_Gallery)},
#ifndef WEBSTER
		{_T("DisplayLineGallery"),				_R(IDS_HELPPATH_Command_Line_Attribute_Gallery)},
#endif	//webster
		{_T("DisplayFontsGallery"),				_R(IDS_HELPPATH_Command_Font_Gallery)},
		{_T("DisplayFrameGallery"),				_R(IDS_HELPPATH_Command_Frame_Gallery)},
		{_T("DisplayClipartGallery"),			_R(IDS_HELPPATH_Command_Clipart_Gallery)},
		{_T("DisplayFillsGallery"),				_R(IDS_HELPPATH_Command_Fill_Gallery)},
		{_T("DisplayNameGallery"),				_R(IDS_HELPPATH_Command_Name_Gallery)},
		{OPTOKEN_COLOUREDITDLG,				_R(IDS_HELPPATH_Misc_Colour_Editor)},					// ????????
#ifndef WEBSTER
#ifdef BUILD_SEPARATIONS
		{OPTOKEN_IMAGESETTING,				_R(IDS_HELPPATH_Command_Imagesetting)},
		{OPTOKEN_OVERPRINTFILL,				_R(IDS_HELPPATH_Command_Overprint_Fill)},
		{OPTOKEN_OVERPRINTLINE,				_R(IDS_HELPPATH_Command_Overprint_Line)},
		{OPTOKEN_PRINTONALLPLATES,			_R(IDS_HELPPATH_Command_Print_On_All_Plates)},
#endif	// BUILD_SEPARATIONS
//		OPTOKEN_BFXDLG,						_R(IDS_HELPPATH_Command_Bitmap_Effects),
#if !defined(EXCLUDE_FROM_XARALX)
		{OPTOKEN_BFXDLG_FLIPROTATE,			_R(IDS_HELPPATH_Command_Bitmap_Effects_Flip_and_Rotate)},
		{OPTOKEN_BFXDLG_RESIZE,				_R(IDS_HELPPATH_Command_Bitmap_Effects_Resize)},
		{OPTOKEN_BFXDLG_BRIGHTNESSCONTRAST,	_R(IDS_HELPPATH_Command_Bitmap_Effects_Brightness_and_Contrast)},
		{OPTOKEN_BFXDLG_COLOURDEPTH,			_R(IDS_HELPPATH_Command_Bitmap_Effects_Colour_Depth)},
		{OPTOKEN_BFXDLG_SPECIALEFFECTS,		_R(IDS_HELPPATH_Command_Bitmap_Effects_Special)},
		{OPTOKEN_TRACEDLG,					_R(IDS_HELPPATH_Command_Bitmap_Tracer)},
#endif
#endif	//webster
		{OPTOKEN_WEBADDRESSDLG,				_R(IDS_HELPPATH_Command_Web_Address)},
		{OPTOKEN_UTILOPTIONS,				_R(IDS_HELPPATH_Command_Options)},
		{OPTOKEN_FRAME_ANIPROPERTIES,		_R(IDS_HELPPATH_Dialog_Animation_Properties)},
		{OPTOKEN_FRAME_GRABALLFRAMES,		_R(IDS_HELPPATH_Command_Preview_All_Frames)},
		{OPTOKEN_FRAME_BROWSERPREVIEW,		_R(IDS_HELPPATH_Command_Preview_Animation_in_Browser)},
//		OPTOKEN_TEMPLATEDLG,				_R(IDS_HELPPATH_Command_Wizard_Properties),

#ifdef STANDALONE
		{OPTOKEN_TIMEDRAW,					_R(IDS_HELPPATH_Command_Time_Redraw)},				// only in viewer
#endif 

	{OPTOKEN_WINDOW_MENU,					_R(IDS_HELPPATH_Menu_Window)},
		{OPTOKEN_WINDOWNEWVIEW,				_R(IDS_HELPPATH_Command_New_View)},
		{OPTOKEN_WINDOWTILE,					_R(IDS_HELPPATH_Command_Arrange_Views)},
		{OPTOKEN_WINDOWCASCADE,				_R(IDS_HELPPATH_Command_Cascade_Views)},
		{OPTOKEN_WINDOWARRANGE,				_R(IDS_HELPPATH_Command_Arrange_Icons)},
		{OPTOKEN_TOOLBARDLG,					_R(IDS_HELPPATH_Command_Control_Bars)},
#ifndef WEBSTER
		{OPTOKEN_BARS_MENU,					_R(IDS_HELPPATH_Command_Bars)},
			{OPTOKEN_VIEWCOLOURBAR,			_R(IDS_HELPPATH_Command_Colour_Line)},
			{OPTOKEN_VIEWSTATUSBAR,			_R(IDS_HELPPATH_Command_Status_Line)},
			{OPTOKEN_VIEWSCROLLBARS,			_R(IDS_HELPPATH_Command_Scrollbars)},
			{OPTOKEN_VIEWRULERS,				_R(IDS_HELPPATH_Command_Rulers)},
		{OPTOKEN_VIEWFULLSCREEN,				_R(IDS_HELPPATH_Command_Fullscreen)},
#endif	//Webster
		{OPTOKEN_SHOWGRID,					_R(IDS_HELPPATH_Command_Show_Grid)},
		{OPTOKEN_SNAPTOGRID,					_R(IDS_HELPPATH_Command_Snap_To_Grid)},
// WEBSTER - markn 15/1/97
// Removed these ops as they are not used
#ifndef WEBSTER
		{OPTOKEN_SHOWGUIDES,					_R(IDS_HELPPATH_Command_Show_Guides)},
		{OPTOKEN_SNAPTOGUIDES,				_R(IDS_HELPPATH_Command_Snap_to_Guides)},
		{OPTOKEN_SNAPTOOBJECTS,				_R(IDS_HELPPATH_Command_Snap_To_Objects)},
		{OPTOKEN_SHOWPRINTBORDERS,			_R(IDS_HELPPATH_Command_Print_Borders)},
#endif // WEBSTER
		{OPTOKEN_WINDOWQUALITY,				_R(IDS_HELPPATH_Command_Quality)},                                 
			{OPTOKEN_QUALITYANTIALIASED,		_R(IDS_HELPPATH_Command_Quality)},
			{OPTOKEN_QUALITYNORMAL,			_R(IDS_HELPPATH_Command_Quality)},
			{OPTOKEN_QUALITYSIMPLE,			_R(IDS_HELPPATH_Command_Quality)},
			{OPTOKEN_QUALITYOUTLINE,			_R(IDS_HELPPATH_Command_Quality)},
#ifndef WEBSTER
		{OPTOKEN_COMPOSITEPREVIEW,			_R(IDS_HELPPATH_Command_Composite_Print_Preview)},
#endif // WEBSTER

	{OPTOKEN_HELP_MENU,						_R(IDS_HELPPATH_Menu_Help)},
		{OPTOKEN_HELPINDEX,					_R(IDS_HELPPATH_Command_Contents)},
		{OPTOKEN_HELPTOOLS,					_R(IDS_HELPPATH_Misc_Tools)},
#ifndef WEBSTER
		{OPTOKEN_HELPGALLERIES,				_R(IDS_HELPPATH_Overview_Galleries)},
#endif // WEBSTER
		{OPTOKEN_HELPDEMOS,					_R(IDS_HELPPATH_Misc_Demos)},
#ifndef WEBSTER
		{OPTOKEN_HELPTECHSUPPORT,			_R(IDS_HELPPATH_Misc_Tech_Support)},
#endif // WEBSTER
		{OPTOKEN_TIPSDLG,					_R(IDS_HELPPATH_Command_Tip_of_the_Day)},
		{OPTOKEN_WEBLINK,					_R(IDS_HELPPATH_Command_Use_Help_From_CD)},
		{OPTOKEN_XARAX_FORUM,				_R(IDS_HELPPATH_Command_Xara_On_The_Web)},
		{OPTOKEN_WEBSTER_HOME_PAGE,			_R(IDS_HELPPATH_Command_Xara_On_The_Web)},
		{OPTOKEN_XARA_HOME_PAGE,				_R(IDS_HELPPATH_Command_Xara_On_The_Web)},
		{OPTOKEN_XARA_PURCHASE_PAGE,			_R(IDS_HELPPATH_Command_Xara_On_The_Web)},
		{OPTOKEN_XARA_WEB_MENU,				_R(IDS_HELPPATH_Command_Xara_On_The_Web)},
		{OPTOKEN_HINTS_AND_TIPS_PAGES,		_R(IDS_HELPPATH_Command_Xara_On_The_Web)},

#ifndef EXCLUDE_FROM_RALPH
		{OPTOKEN_ABOUTDLG,					_R(IDS_HELPPATH_Command_About_Studio)},
#endif

	// Other Operations (not on menus).
	{OPTOKEN_PASTEATSAMEPOS,					_R(IDS_HELPPATH_Command_Paste_at_Same_Position)},
	{OPTOKEN_PAGESIZEDLG,					_R(IDS_HELPPATH_Command_Options_Page)},

#ifndef WEBSTER
	{TOOL_OPTOKEN_BEZTOOL,					_R(IDS_HELPPATH_Tool_Shape)},
	{TOOL_OPTOKEN_REGSHAPE,					_R(IDS_HELPPATH_Tool_QuickShape)},
	{TOOL_OPTOKEN_BLEND,						_R(IDS_HELPPATH_Tool_Blend)},
	{TOOL_OPTOKEN_MOULD,						_R(IDS_HELPPATH_Tool_Mould)},
	{TOOL_OPTOKEN_SELECTOR,					_R(IDS_HELPPATH_Tool_Selector)},
	{TOOL_OPTOKEN_FREEHAND,					_R(IDS_HELPPATH_Tool_Freehand)	},
	{TOOL_OPTOKEN_PEN,						_R(IDS_HELPPATH_Tool_Pen)	},	
	{TOOL_OPTOKEN_RECTANGLE,					_R(IDS_HELPPATH_Tool_Rectangle)	},
	{TOOL_OPTOKEN_ELLIPSE,					_R(IDS_HELPPATH_Tool_Ellipse)	},
	{TOOL_OPTOKEN_GRADFILL,					_R(IDS_HELPPATH_Tool_Fill)		},
	{TOOL_OPTOKEN_TRANSP,					_R(IDS_HELPPATH_Tool_Transparency)	},
	{TOOL_OPTOKEN_SOFTSHADOW,				_R(IDS_HELPPATH_Tool_Shadow)},		
	{TOOL_OPTOKEN_BEVEL,						_R(IDS_HELPPATH_Tool_Bevel)		},	
	{TOOL_OPTOKEN_CONTOUR,					_R(IDS_HELPPATH_Tool_Contour)},		
	{TOOL_OPTOKEN_ZOOM,						_R(IDS_HELPPATH_Tool_Zoom)	},		


	{OPTOKEN_REVERSESTORYPATH,				_R(IDS_HELPPATH_Command_Reverse_Text_on_Curve)},
#endif // WEBSTER

	{TOOL_OPTOKEN_TEXT,						_R(IDS_HELPPATH_Tool_Text)},

#ifndef WEBSTER
	{OPTOKEN_TOGGLEPRINTASSHAPES,			_R(IDS_HELPPATH_Command_Print_Text_as_Shapes)},
	{OPTOKEN_MAKELINESOP,					_R(IDS_HELPPATH_Tool_Shape_Straight)},
	{OPTOKEN_MAKECURVESOP,					_R(IDS_HELPPATH_Tool_Shape_Curved)},
	{OPTOKEN_DELETEPOINTSOP,					_R(IDS_HELPPATH_Tool_Shape_Delete)},
	{OPTOKEN_BREAKATPOINTS,					_R(IDS_HELPPATH_Tool_Shape_Break)},
	{OPTOKEN_SELECTALLPATHPOINTS,			_R(IDS_HELPPATH_Popup_Point_Handle_Select_All_Points)},
	{OPTOKEN_DESELECTALLPATHPOINTS,			_R(IDS_HELPPATH_Popup_Point_Handle_Deselect_All_Handles)},
	{OPTOKEN_FRAME_GRABFRAME,				_R(IDS_HELPPATH_Command_Preview_All_Frames)},
#endif // WEBSTER
	
	{NULL, 0}
};



// This table holds the help topic associations for all messages/warnings/errors.  The format
// of this table is:  <help topic ID> followed by one or more <message resource ID> followed
// by a zero.  The table is terminated by a help topic ID that is zero.
MsgHelpTuple atplMsgTopics[] =
{
	// The .INI file is out-of-date.
	_R(IDS_HELPPATH_Alert_INI_File_Out_of_Date),
			_R(IDT_INIFILE_OUTOFDATE),
			_R(IDT_INIFILE_NEWEREXISTS),
			0,

	// Problem reading/writing .INI file - is corrupt?
	_R(IDS_HELPPATH_Alert_Problem_With_INI_File),
			_R(IDT_INIFILE_CANTREMOVE),
			_R(IDT_INIFILE_CANTCREATE),
			_R(IDE_BAD_INI_FILE),
			_R(IDN_BAD_DECLARE_SECTION),
			_R(IDN_BAD_CURRENT_SECTION),
			_R(IDE_OPTS_SETPREF_VIEW),
			_R(IDE_OPTS_SETPREF_EDIT),
			_R(IDE_OPTS_SETPREF_MISC),
			_R(IDE_OPTS_SETPREF_UNDO),
			_R(IDE_OPTS_SETPREF_SCALE),
			_R(IDE_OPTS_SETPREF_TUNE),
			_R(IDE_OPTS_READPREF_VIEW),
			_R(IDE_OPTS_READPREF_EDIT),
			_R(IDE_OPTS_READPREF_MISC),
			_R(IDE_OPTS_READPREF_UNDO),
			_R(IDE_OPTS_READPREF_SCALE),
			_R(IDE_OPTS_READPREF_TUNE),
			0,

	// Bad bars file - is corrupt?
	_R(IDS_HELPPATH_Alert_Bars_File_Problem),
			_R(IDS_INVALID_BARINFO),
			_R(IDE_CANT_SAVE_BARS),
//			_R(IDE_BADBARSFILE),		// removed Neville 5/8/97 as no longer required
			0,

	// File not found
	_R(IDS_HELPPATH_Alert_File_Not_Found),
			_R(IDE_ACCUSOFT_ERROR2),
			_R(IDS_LIBRARY_INVALID_PATH),
			_R(IDE_OPEN_ERROR),
			_R(IDE_ZOPEN_ERROR),
			0,

	// Can't create file - disk full?
	_R(IDS_HELPPATH_Alert_Unable_to_Create_File),
			_R(IDE_MF_NOCREATE),
			_R(IDE_ACCUSOFT_ERROR6),
			_R(IDE_ACCUSOFT_ERROR9),
			0,

	// File-share violation on write
	_R(IDS_HELPPATH_Alert_Share_Violation),
			_R(IDE_MF_NOMEMLOCK),
			0,

	// General fail on read/write
	_R(IDS_HELPPATH_Alert_Read_Write_Failure),
			_R(IDE_FILE_READ_ERROR),
			_R(IDE_FILE_WRITE_ERROR),
			_R(IDE_CORELPALETTEFILTER_FILEERR),
			_R(IDE_IO_ERROR),
			_R(IDS_LIBRARY_INDEX_FILE_ERROR),
			_R(IDS_LIBRARY_PROBLEMS_CREATING_INDEX),
			_R(IDE_ZFILE_READ_ERROR),
			_R(IDE_ZFILE_WRITE_ERROR),
			0,

	// File is read-only
	_R(IDS_HELPPATH_Alert_File_is_Read_Only),
			_R(IDS_READONLY_MEDIA),
			_R(IDE_FILEREADERROR),
			_R(IDS_READONLY_INDEX_EXISTS),
			0,

	// Overrun/truncated file
	_R(IDS_HELPPATH_Alert_Overrun_or_Truncated_File),
			_R(IDE_EOF_ERROR),
			_R(IDE_ACCUSOFT_ERROR12),
			0,

	// Can't close file (floppy disk changed?)
	_R(IDS_HELPPATH_Alert_Cannot_Close_File),
			_R(IDE_MF_NOCLOSE),
			0,

	// Bad filename
	_R(IDS_HELPPATH_Alert_Bad_Filename),
			_R(IDT_FILENAME_TOO_LONG),
			_R(IDE_ACCUSOFT_ERROR4),
			_R(IDE_PATH_ERROR),
			0,

	// Out of memory!  Remove this as there won't be enough memory to show it ;-)
/*	_R(IDS_HELPPATH_Alert_Out_of_Memory),
			_R(IDE_FRACTMEM),
			_R(IDE_NOMORE_MEMORY),
			_R(IDE_DIBALLOCFAILED),
			_R(IDE_ACCUSOFT_ERROR1),
			_R(IDE_BAD_GROWHEAP),
			_R(IDE_MEM_BLOCK_FAILURE),
			_R(IDS_RETROOUTOFMEM),
			_R(IDS_PATH_MEM_ERROR),
			_R(IDT_INFOBAR_NOMEM),
			_R(IDE_REALLOC_ERROR),
			_R(IDE_FHAND_BADCURSORS),
			0,
*/
#ifndef WEBSTER
	// Fractal too small? - unexpected error creating it
	_R(IDS_HELPPATH_Alert_Fractal_Too_Small),
			_R(IDE_FRACTFAILED),
			0,

	// Wrong version of Win32s - reinstall
	//_R(IDS_HELPPATH_Alert_Incorrect_Win32s_Version),
	//		_R(IDW_OLDFILE),
	//		0,
#endif // WEBSTER

	// Reinstall the program - you've trashed it somehow
	_R(IDS_HELPPATH_Alert_Reinstall_the_Program),
			_R(IDW_NOTOOLS),
			_R(IDW_BADGDRAW),
			_R(IDW_NO3D),
			_R(IDE_PROBLEMDLL),
			_R(IDW_NOACCUSOFTDLL),
			_R(IDW_IFFYACCUDLL),
			_R(IDE_ACCUSOFT_ERROR15),
			_R(IDE_ACCUSOFT_ERROR16),
			_R(IDE_ACCUSOFT_ERROR17),
			_R(IDE_FILTERDLL_BADLOAD),
			_R(IDE_FILTERDLL_NOTGRAPHICS),
			_R(IDW_IFFYFILE),
			_R(IDN_FILTERNOTPRESENT),
			_R(IDT_CANT_FIND_FILTER),
			0,
	
#ifndef WEBSTER
	// User cancelled something
	_R(IDS_HELPPATH_Alert_User_Cancelled_Something),
			_R(IDW_CANCELLEDBMPIMPORT),
			_R(IDW_CANCELLEDBMPEXPORT),
			_R(IDW_CANCELEXPORT),
			_R(IDT_IMP_USERABORT),
			0,

	// Duff processor.  Where's the one about having a 386?
	_R(IDS_HELPPATH_Alert_Bad_Pentium),
			_R(IDE_BAD_PENTIUM),
			0,
#endif // WEBSTER

	// File already exists - overwrite?
	_R(IDS_HELPPATH_Alert_File_Exists),
			_R(IDM_OVERWRITE),
			_R(IDM_EXPORT_OVERWRITE),
			_R(IDS_LIBRARY_OVERWRITE_INDEX_FILE),
			0,

	// Doc is unsaved - save it now?
	_R(IDS_HELPPATH_Alert_Document_Unsaved),
			_R(IDM_SAVEPROMPT),
			0,

	// Exception handling.  Alex says these shouldn't have help buttons.
/*	_R(IDS_HELPPATH_Alert_Serious_Problem),
			_R(IDE_EX_ACCESS_VIOLATION),
			_R(IDE_EX_BADMEMORY),
			_R(IDE_EX_BADFLOAT),
			_R(IDE_EX_FLOATZERO),
			_R(IDE_EX_INTZERO),
			_R(IDE_EX_BADOP),
			_R(IDE_EX_THREAD),
			0,
*/
	// Internal error - a disaster!
	_R(IDS_HELPPATH_Alert_Serious_Problem),
			_R(IDS_ERRORBOX_ENSURE),
			_R(IDS_ERRORBOX_SPURIOUS),
			_R(IDS_ERRORBOX_SEVERE),
			_R(IDT_INTERNAL_ERROR),
			_R(IDE_INTERNAL_ERROR),
			_R(IDE_BUFFER_ERROR),
			_R(IDE_CLOSE_ERROR),
			_R(IDE_FILEPOS_ERROR),
			_R(IDE_MEM_CLOSE_ERROR),
			_R(IDE_NOTOPEN_ERROR),
			_R(IDE_STRING_SIZE_ERROR),
			_R(IDE_NULL_FILE),
			_R(IDE_MEM_OPEN_ERROR),
			_R(IDE_LOAD_RES_ERROR),
			_R(IDE_READ_ONLY),
			_R(IDE_WRITE_ONLY),
			_R(IDE_LOAD_CLIPBOARD),
			_R(IDE_EMPTY_CLIPBOARD),
			_R(IDE_FILE_SIZE_ZERO),
			_R(IDE_RES_SIZE_ERROR),
			_R(IDE_NO_DOC_VIEW),
			_R(IDE_INVALID_VIRTKEY),
			_R(IDE_INVALID_CHARKEY),
			_R(IDS_PATH_DELETE_ERROR),
			_R(IDS_NO_DOC),
			_R(IDT_EXPORT_INTERNAL_ERR),
			_R(IDT_DIALOG_CREATEFAIL),
			_R(IDE_DV_NOTINCHAPTER),
			_R(IDE_ACCUSOFT_ERROR),
			_R(IDE_ACCUSOFT_ERROR5),
			_R(IDE_ACCUSOFT_ERROR10),
			_R(IDE_ACCUSOFT_ERROR11),
			_R(IDE_ACCUSOFT_ERROR13),
			_R(IDE_ACCUSOFT_ERROR19),
			_R(IDE_ACCUSOFT_ERROR20),
			_R(IDW_NOUNDORESIZING),
			_R(IDE_OPEN_WRITE),

			_R(IDS_BARSINFO_ONE),					// status-line?
			_R(IDS_APPPREFSDLG_ONE),				// status-line?
			_R(IDS_PRINTPREFSDLG_ONE),				// status-line?
			_R(IDS_SCALE_DLG_OPEN),					// status-line?
			_R(IDS_UNIT_DLG_OPEN),					// status-line?
			0,

	// Transparency mode switching
/*	_R(IDS_HELPPATH_Alert_Transparency_Mode_Switching),					OBSOLETE
			_R(IDS_NEED_TRANSPARENCY_TXT),
			_R(IDS_WARN_DOC_DEEP),
			0,
*/
#ifndef WEBSTER
	// Numeric input is invalid
	_R(IDS_HELPPATH_Alert_Invalid_Input_Value),
			_R(IDE_INVALIDZOOMFACTOR),
			_R(IDS_SEL_ERROR_XY),
			_R(IDS_SEL_ERROR_WH),
			_R(IDS_SEL_ERROR_XYSCALE),
			_R(IDS_SEL_ERROR_ANGLE),
			_R(IDS_SEL_ERROR_SHEAR),
			_R(IDS_BLENDSTEPS_INVALID),
			_R(IDS_BMPPREFS_INVALIDDPI),
			_R(IDE_OPTS_INVALIDANGLE),
			_R(IDE_OPTS_INVALIDDISTANCE),
			_R(IDE_OPTS_INVALIDNUDGE),
			_R(IDE_OPTS_INVALIDLINERAD),
			_R(IDE_OPTS_INVALIDPOINTRAD),
			_R(IDE_OPTS_INVALIDBLEED),
			_R(IDE_OPTS_INVALIDFILELIST),
			_R(IDE_OPTS_INVALIDGRIDSPACE),
			_R(IDE_OPTS_INVALIDGRIDDIV),
			_R(IDE_OPTS_INVALIDUNDOSIZE),
			_R(IDE_OPTS_INVALIDDRAWSCALE),
			_R(IDE_OPTS_INVALIDREALSCALE),
			_R(IDE_OPTS_INVALIDSCALING),
			_R(IDE_OPTS_BADTEMPWORKMEM),
			_R(IDE_OPTS_INVALIDCACHESIZE),
			_R(IDE_OPTS_BADUNITDENOMINATOR),
			_R(IDE_OPTS_BADUNITNUMERATOR),
			_R(IDE_OPTS_BADSPECSTRING),
			_R(IDE_OPTS_BADTOKENSTRING),
			_R(IDE_PATHMOVE_DUFFFIELD),
			_R(IDE_INVALID_CENTRE),
			_R(IDE_INVALID_RADIUS),
			_R(IDE_INVALID_ANGLE),
			_R(IDE_INVALID_STELLATIONOFFSET),
			_R(IDE_INVALID_CURVATURE),
//			_R(IDE_INVALID_CURVATURESIZE),
			_R(IDS_REGSHAPETOOL_TOOSMALL),
			_R(IDS_NOT_VALID_LINE_WIDTH),
			0,
#endif // WEBSTER

	// Bad doc file format
	_R(IDS_HELPPATH_Alert_Bad_Document_File_Format),
			_R(IDE_BADBITMAPINFO),
			_R(IDE_ACCUSOFT_ERROR3),
			_R(IDE_ACCUSOFT_ERROR7),
			_R(IDE_ACCUSOFT_ERROR8),
			_R(IDE_ACCUSOFT_ERROR18),
			_R(IDE_ACCUSOFT_ERROR21),
			_R(IDE_ACCUSOFT_ERROR22),
			_R(IDE_ACCUSOFT_ERROR23),
			_R(IDE_BADFORMAT),
			_R(IDE_FORMATNOTSUPPORTED),
			_R(IDE_CORELPALETTEFILTER_BADPALFILE),
			_R(IDE_CORELPALETTEFILTER_BADCPLFILE),
			_R(IDE_CORELPALETTEFILTER_NOTCPLORPAL),
			_R(IDE_CDRFILTER_FORMATERROR),
			_R(IDE_CDRFILTER_NOTCDR),
			_R(IDT_EPS_STRINGTOOLONG),
			_R(IDT_EPS_IS_EMPTY),
			_R(IDT_IMPORT_BAD_PATH_FOUND),
			_R(IDT_FILE_NOTRECOGNISED),
			_R(IDT_BAD_METAFILE),
			_R(IDT_LEX_STRINGTOOLONG),
			_R(IDE_FILETOOSMALL),
			_R(IDE_FILTERDLL_NOIMPORT),
			_R(IDT_EPS_STACKLIMIT),
			_R(IDT_EPS_BADSYNTAX),
			0,

#ifndef WEBSTER
	// Doc template
	_R(IDS_HELPPATH_Alert_Document_Template),
			_R(IDT_SAVEASDEFAULTQUEST),
			0,

	// Abort/continue printing?
	_R(IDS_HELPPATH_Alert_Abort_or_Continue_Printing),
			_R(IDS_PRINT_SUSPEND),
			0,

	// How to use layers. These all point to the layer gallery. This is a horrible cop-out. =8-(
	_R(IDS_HELPPATH_Gallery_Layer),
			_R(IDS_ALREADY_ON_BOTTOM_LYRS),
			_R(IDS_ALREADY_ON_BOTTOM_LYRP),
			_R(IDS_ALREADY_ON_TOP_LYRS),
			_R(IDS_ALREADY_ON_TOP_LYRP),
			_R(IDS_ALREADY_AT_FRONTS),
			_R(IDS_ALREADY_AT_FRONTP),
			_R(IDS_ALREADY_AT_BACKS),
			_R(IDS_ALREADY_AT_BACKP),
			_R(IDS_LAYERIDS_NOT_UNIQUE),
			_R(IDS_LAYER_NOCHANGE_VISIBLE),
			_R(IDS_LAYER_NOCHANGE_LOCKED),
			_R(IDS_LAYER_NOCHANGE_VISIBLE_ACTIVE),
			_R(IDS_LAYER_NOCHANGE_LOCKED_ACTIVE),
			_R(IDS_WARNING_INSERTLAYER),
			0,

	// Layer contains objects, do you want to delete it?
	_R(IDS_HELPPATH_Alert_Layer_Delete_Query),
			_R(IDS_LAYER_CONSTAINSOBJECTS),
			0,
#endif // WEBSTER

	// Can't do that because nothing is selected!
	_R(IDS_HELPPATH_Alert_Nothing_Selected),
			_R(IDS_NO_OBJECTS_SELECTED),
			_R(IDS_NO_GROUPS_SEL),
			_R(IDS_FONTS_CANT_FIND_SELECTED_FONT),
			_R(IDE_COLGAL_NOSOURCE),
			_R(IDS_NOTPATHANDTEXTSEL),
			0,
	
	// Export entire spread/page ? No known problems here.
	_R(IDS_HELPPATH_Alert_Export_Entire_Spread),
			_R(IDT_BMPEXP_NOSELECTION),
			0,

#ifndef WEBSTER
	// Trying to set "current" (default) attribute(s)?
	_R(IDS_HELPPATH_Alert_Set_Current_Attribute),
			_R(IDS_MAKE_COL_CURRENT_ATTRIB),
			_R(IDS_ATTRIB_NOT_REQUIRED),
			_R(IDS_ATTRIB_NOT_REQUIRED_BY_OBJECT),
			_R(IDS_DROPPED_ONTO_PAGE),
			0,
#endif // WEBSTER

	// Undo warnings - you're going to lose it
	_R(IDS_HELPPATH_Alert_Undo_Warnings),
			_R(IDT_OPTS_LOSSOFUNDO),
			_R(IDE_OPTS_BADUNDORESIZE),
			0,

	// Bits that can't quite be imported.
	_R(IDS_HELPPATH_Alert_File_Cannot_All_be_Imported),
			_R(IDE_INVALID_PATH),
			_R(IDT_IMPQUERY_NOTSURE),
			_R(IDW_OPENQUERY_NOTSURE),
			_R(IDT_AIEPS_COMPLEXGRADFILL),
			_R(IDT_CDRFILTER_OBJWARNING),
			_R(IDT_CDRFILTER_OBJWARNING1),
			_R(IDS_MEATFILE_WARN),
			0,

#ifndef WEBSTER
	// Bits that can't quite be exported.
	_R(IDS_HELPPATH_Alert_File_Cannot_All_be_Exported),
			_R(IDT_EXPQUERY_NOTSURE),
			_R(IDT_EXPORT_FRACTALWARNING),
			0,
#endif // WEBSTER

	// Clipboard problems . . .
	_R(IDS_HELPPATH_Alert_Clipboard_Problem),
			_R(IDE_ACCUSOFT_ERROR14),
			_R(IDS_CLIPBOARD_KEEPONQUIT),
			_R(IDS_NO_GRID_CLIPBOARD_OPS),
			_R(IDS_CLIPBOARD_EMPTY),
			0,

	// Colour problems . . .
	_R(IDS_HELPPATH_Gallery_Colour),
			_R(IDS_COLGAL_QREDEFINE),
			_R(IDE_COLGAL_SAMEITEM),
			_R(IDS_EDIT_NOPARENT),
			_R(IDS_COLOURSMERGED),
			0,

#ifndef WEBSTER
	// Trying to delete a colour that is in use.
	_R(IDS_HELPPATH_Alert_Delete_Used_Colour),
			_R(IDS_COLGAL_COLINUSE),
			0,
#endif // WEBSTER

	// How to use groups . . .
	_R(IDS_HELPPATH_Command_Group),
			_R(IDS_ILLEGAL_TO_GROUP_SINGLE_GROUP),
			0,

	// Bad search text.
	_R(IDS_HELPPATH_Alert_Bad_Text_Search),
			_R(IDE_SGSEARCH_BADTEXT),
			0,

#ifndef WEBSTER
	// How to use units . . .
	_R(IDS_HELPPATH_Command_Options_Units),
			_R(IDS_UNITERROR_HASDESCENDENTS),
			_R(IDS_UNITERROR_BEINGUSED),
			_R(IDS_UNITERROR_BASEISCURRENT),
			_R(IDS_UNITERROR_BASEISDESCENDENT),
			0,

	// No result when combining shapes
	_R(IDS_HELPPATH_Alert_No_Result_When_Combining_Shapes),
			_R(IDS_COMBINE_NORESULT),
			0,

	// This op will remove perspective
	_R(IDS_HELPPATH_Alert_This_Op_Will_Remove_Perspective),
			_R(IDS_OKTOMAKESHAPE),
			0,
#endif // WEBSTER

	// Multiple references to a bitmap about to be deleted
	_R(IDS_HELPPATH_Alert_Multiple_References_to_a_Bitmap_About_to_be_Deleted),
			_R(IDS_BMPGAL_BMPINUSE),
			0,
/*
	// No textures loaded							OBSOLETE
	_R(IDS_HELPPATH_Alert_No_Texture_Loaded),
			_R(IDE_BADBITMAP),
			0,
*/
	// No library index file available.
	_R(IDS_HELPPATH_Alert_Library_Cannot_Find_Index),
			_R(IDS_LIBRARY_NO_INDEX_FILE),
			0,

	// No library index file (possibly generate one?)
	_R(IDS_HELPPATH_Alert_Library_No_Index_File_Gen),
			_R(IDS_LIBRARY_NO_INDEX_FILE_GEN),
			0,

	// Browse or scan for an index file?
	_R(IDS_HELPPATH_Alert_Library_Browse_Or_Scan),
			_R(IDS_BROWSE_OR_SCAN),
			0,

	// The library index is read-only.
	_R(IDS_HELPPATH_Alert_Library_Cannot_Update_Index),
			_R(IDS_LIBRARY_UPDATE_RO_WITH_INDEX),
			_R(IDS_READONLY_INDEX_EXISTS),
			0,

	// The library CD has been removed.
	_R(IDS_HELPPATH_Alert_Library_Check_Remove),
			_R(IDS_LIBRARY_CD_REMOVED),
			_R(IDS_LIBRARY_CANT_FIND_INDEX),
			0,

	// Can't find library index (provides a Skip button)
	_R(IDS_HELPPATH_Alert_Library_No_Index_Skip),
			_R(IDS_LIBRARY_CANT_FIND_INDEX_SKIP),
			0,

	// Create remote indices?
	_R(IDS_HELPPATH_Alert_Library_Create_Remote),
			_R(IDS_LIBRARY_CREATE_REMOTE),
			0,

	// Failed to find the library CD.
	_R(IDS_HELPPATH_Alert_Browsing_Cannot_Find_Library),
			_R(IDS_LIBRARY_BROWSING_CANT_FIND_LIBRARY),
			0,

	// Can't find any files to add to the index.
	_R(IDS_HELPPATH_Alert_Library_No_Files),
			_R(IDS_LIBRARY_NO_FILES),
			0,

	// Problems creating a thumbnail.
	_R(IDS_HELPPATH_Alert_Library_Thumbnail_Problem),
			_R(IDS_LIBRARY_THUMBNAIL_PROBLEMS),
			0,

	// Removing selected items.
	_R(IDS_HELPPATH_Alert_Library_Check_Remove),
			_R(IDS_LIBRARY_CHECK_REMOVE),
			0,

	// Check if user really wants to create a new index.
	_R(IDS_HELPPATH_Alert_Library_Check_Index_Create),
			_R(IDS_LIBRARY_CHECK_INDEX_CREATE),
			0,

	// Problems creating an index.
	_R(IDS_HELPPATH_Alert_Library_Problems_Creating_Index),
			_R(IDS_LIBRARY_PROBLEMS_CREATING_INDEX),
			0,
/*
	// Remove old library groups?						OBSOLETE (?)
	_R(IDS_HELPPATH_Alert_Remove_Old_Library_Groups),
			_R(IDS_REMOVE_OLD_GOUPS_FONTS),
			_R(IDS_REMOVE_OLD_GOUPS_CLIPART),
			_R(IDS_REMOVE_OLD_GOUPS_FILLS),
			0,
*/

#ifndef WEBSTER
	// Sure you want to install/desintall font?
	_R(IDS_HELPPATH_Alert_Fonts_Gallery_Sure_InstallDeinstall),
			_R(IDS_FONTS_GALLERY_SURE_INSTALL),
			_R(IDS_FONTS_GALLERY_SURE_DEINSTALL),
			0,

	// Problem installing font
	_R(IDS_HELPPATH_Alert_Problem_Installing_Font),
			_R(IDS_DEINSTALL_FONT_IN_USE),
			_R(IDS_PROBLEMS_INSTALLING),
			_R(IDS_PROBLEMS_DEINSTALLING),
			_R(IDS_INSTALL_FONT_ALREADY_THERE),
			0,
#endif // WEBSTER

/*
	// Trying to open a document that is already open.			OBSOLETE
	_R(IDS_HELPPATH_Alert_document_already_open),
			_R(IDE_DOC_ALREADY_OPEN),
			0,
*/
#ifndef WEBSTER
	// Do you want to close other docs when opening a new one?
	_R(IDS_HELPPATH_Alert_Close_open_documents),
			_R(IDS_CLOSEOTHERDOCS),
			0,
#endif // WEBSTER

	// The 'Xara Studio' font for dialogs etc is not installed/enabled.
	_R(IDS_HELPPATH_Alert_XaraFont_Not_Found),
			_R(IDS_NODISPLAYFONTS),
			0,

	// You don't have a font used by a doc, so we've substituted one.
	_R(IDS_HELPPATH_Alert_Fonts_Substituted),
			_R(IDS_FONTSSUBSTITUTED),
			0,

	// User entered a spread size that is too big.
	_R(IDS_HELPPATH_Alert_Spread_Too_Large),
			_R(IDE_OPTS_OVERALLSPREADSIZE),
			_R(IDE_OPTS_INVALIDHEIGHT),
			_R(IDE_OPTS_INVALIDWIDTH),
			_R(IDE_OPTS_INVALIDMARGIN),
			0,

	// Failed to create a transparent GIF mask.
	_R(IDS_HELPPATH_Alert_GIF_Transparency_Mask_Creation_Failed),
			_R(IDE_TIGIFFILTER_MASKFAILED),
			0,

#ifndef WEBSTER
	// The shape on the clipboard is not suitable for use as a mould.
	_R(IDS_HELPPATH_Alert_Bad_Mould_Shape),
			_R(IDE_UNSUITABLEMOULDSHAPE),
			0,

	// Tried to load an EPS file
	_R(IDS_HELPPATH_Alert_Loading_EPS),
			_R(IDE_LOADCAMELOTEPSITHINKNOT),
			_R(IDE_LOADCAMELOTEPSITHINKNOTNAMED),
			0,

	_R(IDS_HELPPATH_Alert_Overwrite_V1_file_with_a_V1_5),
			_R(IDS_OVERWRITEOLDFILE),
			0,

	_R(IDS_HELPPATH_Alert_Unable_to_find_printer_profile),
			_R(IDT_OPTS_LOGICALCONTEXT),
			0,

	_R(IDS_HELPPATH_Alert_Drawing_too_large_for_CMX_export),
			_R(IDT_CMXFILTER_SQUISHWARNING),
			0,

	_R(IDS_HELPPATH_Alert_This_document_contains_overlapping_transparent_objects),
			_R(IDW_CMXOVERLAPPEDLENS),
			0,
#endif // WEBSTER

	_R(IDS_HELPPATH_Alert_CD6_files_not_supported),
			_R(IDE_CDRFILTER_CDR6),
			0,

#ifndef WEBSTER
	_R(IDS_HELPPATH_Alert_ATM_Not_Running),
			_R(IDS_ATM_NOT_RUNNING),
			0,
	
	_R(IDS_HELPPATH_Alert__No_plates_have_been_set_to_print),
			_R(IDE_NOPLATESTOPRINT),
			0,

	_R(IDS_HELPPATH_Alert_Do_you_want_to_print_all_these_plates),
			_R(IDE_MANYPLATESTOPRINT),
			0,
#endif // WEBSTER
	
	_R(IDS_HELPPATH_Alert_Web_Address_Too_Long),
			_R(IDE_HOTLINKDLG_STRINGTOOLONG),
			0,
	
#ifndef WEBSTER
	_R(IDS_HELPPATH_Alert_You_cannot_create_spot_colors),
			_R(IDE_SPOTSAREPROCESS),
			0,
#endif // WEBSTER

	_R(IDS_HELPPATH_Alert_You_cannot_edit_library_colors),
			_R(IDE_CANTEDITLIBCOLOUR),
			0,
	
#ifndef WEBSTER
	_R(IDS_HELPPATH_Alert_Only_named_colors_can_be_spot_colors),
			_R(IDE_LOCALCANTBESPOT),
			0,

	_R(IDS_HELPPATH_Alert_Print_as_bitmap),
			_R(IDE_PRINTSEPSASBMP),
			0,
#endif // WEBSTER

	_R(IDS_HELPPATH_Alert_Bitmap_Too_Large),
			_R(IDE_BMP_BADPIXELWIDTH),
			0,

	_R(IDS_HELPPATH_Alert_Cannot_Open_Web_Page),
			_R(IDS_NO_WEB_UTILITY),
			0,

	_R(IDS_HELPPATH_Alert_Cannot_Open_Web_Page),
			_R(IDS_NO_WEB_PAGE),
			0,

	_R(IDS_HELPPATH_Alert_Export_to_Existing_File),
			_R(IDM_IMAGEMAP_INSERT),
			0,

	_R(IDS_HELPPATH_Alert_NavBar_File_Exists),
			_R(IDM_OVERWRITE_INJECT),
			0,

	_R(IDS_HELPPATH_Plugin_Filters),
			_R(IDS_XPF_MIXEDTRANSLAYER),
			0,

	// End of table.
	0
};
