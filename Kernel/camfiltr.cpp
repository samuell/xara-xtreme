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
//
// This filter forms the base filter for the v2 Native and Web file filters.

/*
*/

#include "camtypes.h"
#include "app.h"
#include "cversion.h"
#include "product.h"

#include "camfiltr.h"
//#include "oilfltrs.h"	// Web oil filter i.e. Windows UI specific details e.g. extension
//#include "filtrres.h"	// IDs

//#include "tim.h"		// _R(IDT_EXPORT_INTERNAL_ERR), _R(IDT_IMPORT_USERABORT)
//#include "nev.h"		// _R(IDN_USER_CANCELLED)
//#include "simon.h"	// _R(IDS_LAYER_DESCRS)
//#include "will2.h"	// _R(IDS_K_EPSFILTER_IMPORTED)
//#include "richard2.h"	// _R(IDS_SGLAYER_LAYER_NUM)

//#include "ccpanose.h"
#include "fontman.h"
#include "progress.h"
//#include "sglayer.h"
#include "page.h"
#include "spread.h"
#include "sprdmsg.h"
#include "layer.h"
#include "node.h"
#include "nodepath.h"
#include "bars.h"		// UpdateStateOfAllBars
#include "chapter.h"
#include "nodershp.h"
#include "biasgain.h"
//#include "qualops.h"	// for QualitySliderDescriptor::Update();

//#include "bitfilt.h"	// GetSizeOfDrawing()
//#include "bmpsrc.h"		// BitmapSource
#include "colcomp.h"	// colour component, record handling classes for colours
#include "bmpcomp.h"	// bitmap component, record handling classes for bitmaps
#include "unitcomp.h"	// units component, record handling classes for units
#include "infocomp.h"	// doc info component, record handling classes for document information
#include "viewcomp.h"	// view component, record handling classes for view records
//#include "princomp.h"	// print component, record handling classes for font records
#include "fontcomp.h"	// font component, record handling classes for font records

#include "cxfile.h"		// The core v2 format class
#include "cxfrec.h"		// The v2 format record class
#include "cxftags.h"	// The tag definitions
#include "cxfdefs.h"	// The constants
#include "cxflists.h"	// Lists for import & export
#include "taglists.h"	// Lists for holding tag information

#include "rechattr.h"	// Record handling classes for attributes
#include "rechcol.h"	// ColourRecordHandler for importing colours from v2 native/web files
#include "rechbmp.h"	// BitmapRecordHandler for importing bitmaps from v2 native/web files
#include "rechbmpp.h"	// BitmapPropertiesRecordHandler
#include "rechsmth.h"	// BitmapSmoothingRecordHandler
#include "rechcomp.h"	// Record handling classes for compression
#include "rechpath.h"	// Record handling class for paths
#include "rechrshp.h"
#include "rechellp.h"
#include "rechrect.h"
#include "rechpoly.h"
#include "rechtext.h"
#include "rechgrp.h"
#include "rechblnd.h"
#include "rechmld.h"
#include "rechdoc.h"
#include "rechinfo.h"	// DocInfoRecordHandler
#include "rechunit.h"	// UnitsRecordHandler
#include "rechview.h"
//#include "isetattr.h"	// ImagesettingAttrRecordHandler
#include "rechprnt.h"	// PrintingRecordHandler
#include "strkattr.h"	// StrokeAttrRecordHandler
#include "userattr.h"
#include "tmpltatr.h"	// TemplateAttrRecordHandler & WizOpStyleRefRecordHandler
//#include "styles.h"		// WizOpStyleRecordHandler
#include "rechshad.h"	// ShadowRecordHandler
#include "nodebev.h"	// BevelRecordHandler

//#include "xarprefs.h"	// The base Xara file preferences dialogue.
//#include "webprefs.h"	// Web options dialog handler
//#include "webparam.h"	// WebPrefsDlgParam class
//#include "prvwflt.h"	// for PreviewBitmap::PreviewBitmapSize
#include "group.h"

//#include "grptrans.h"	// GroupTransparencyRecordHandler
//#include "ndcchbmp.h"	// CacheBitmapRecordHandler
//#include "nodecach.h"	// CachedNodesGroupRecordHandler

//#include "ralphcri.h"

#include "winrect.h"	//for class WinRect
//#include "osrndrgn.h"	//for BitmapDocRectToWin
//#include "resimmap.h"	//for HTML tag exporting resources 
//#include "clipint.h"	//for putting text on the clipboard

// WEBSTER - markn 29/1/97
#include "becomea.h"
#include "attrmap.h"
//#include "prevwres.h"	// _R(IDS_COMBINING_LAYERS)
#include "exphint.h"
#include "ncntrcnt.h"	// ContourRecordHandler
#include "ngsentry.h"	// NodeSetSentinel

#include "slicehelper.h"// for slice helper static functions - layer merging
//#include "ngcore.h"		// For name gallery code.
//#include "ngitem.h"		// For named item code.
//#include "opbevel.h"
#include "brshattr.h"
#include "ndclpcnt.h"	// ClipViewRecordHandler

#include "fthrattr.h"	// FeatherRecordHandler

#include "nodetext.h"	// For TextStory - BaseCamelotFilter::WriteSelectedNodes

#include "nodeshad.h"	// for NodeShadow export.
#include "nodecont.h"
//#include "objreg.h"

#include "nodeliveeffect.h"
#include "hardwaremanager.h"
using namespace oilHardwareManager;

//----------------------------------------------------------------------------

/********************************************************************************************

>	class GeneralRecordHandler : public CamelotRecordHandler

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Purpose:	This is the general record handler.
				It handles all the general record types, such as the End Of File, file header, etc.

********************************************************************************************/

class GeneralRecordHandler : public CamelotRecordHandler
{
	// Give my name in memory dumps
	CC_DECLARE_DYNAMIC(GeneralRecordHandler);

public:
	virtual UINT32*	GetTagList();
	virtual BOOL HandleRecord(CXaraFileRecord* pCXaraFileRecord);

#if XAR_TREE_DIALOG
	virtual void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);
#endif

private:
	BOOL HandleFileHeader(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleAtomicTagsRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleEssentialTagsRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleTagDescriptionRecord(CXaraFileRecord* pCXaraFileRecord);
	BOOL HandleTagNudeSizeRecord(CXaraFileRecord* pCXaraFileRecord);
};

//----------------------------------------------------------------------------

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(BaseCamelotFilter, VectorFilter);
CC_IMPLEMENT_DYNAMIC(GeneralRecordHandler,CamelotRecordHandler);

CC_IMPLEMENT_MEMDUMP(InsertLevelStack,List);
CC_IMPLEMENT_MEMDUMP(InsertLevelStackItem,ListItem);

CC_IMPLEMENT_MEMDUMP(AtomicTagListItem,ListItem)
CC_IMPLEMENT_MEMDUMP(EssentialTagListItem,ListItem)
CC_IMPLEMENT_MEMDUMP(TagDescriptionListItem,ListItem)
CC_IMPLEMENT_MEMDUMP(AtomicTagList,List)
CC_IMPLEMENT_MEMDUMP(EssentialTagList,List)
CC_IMPLEMENT_MEMDUMP(TagDescriptionList,List)

CC_IMPLEMENT_MEMDUMP(CXaraFilePathRecordRefListItem,ListItem)
CC_IMPLEMENT_MEMDUMP(CXaraFilePathRecordRefList,List)
// WEBSTER - markn 31/1/97
// Replaced with general system
//CC_IMPLEMENT_MEMDUMP(CXaraFileTextStoryGroupRefList,List)
//CC_IMPLEMENT_MEMDUMP(CXaraFileTextStoryGroupRefListItem,ListItem)
CC_IMPLEMENT_MEMDUMP(CXaraFileNodeGroupRefList,List)
CC_IMPLEMENT_MEMDUMP(CXaraFileNodeGroupRefListItem,ListItem)

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
/***********************************************************************************************

>	class NodeToOutlinesBecomeA: public BecomeA

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Purpose:	The class that handles the convertion of a node to outlines.
				It receives all the paths passed back from the node, and appends them 
				to a group object.

				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

***********************************************************************************************/

class NodeToOutlinesBecomeA : public BecomeA
{
CC_DECLARE_MEMDUMP(NodeToOutlinesBecomeA);
public:
	NodeToOutlinesBecomeA(NodeGroup* pThisGroup) :	BecomeA(BECOMEA_PASSBACK,CC_RUNTIME_CLASS(NodePath),NULL), pGroup(pThisGroup) {};
	~NodeToOutlinesBecomeA();

	// This function should be called when Reason == BECOMEA_PASSBACK 
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap);

private:
	NodeGroup* pGroup;
};

CC_IMPLEMENT_MEMDUMP(NodeToOutlinesBecomeA,BecomeA);

//--------------------------------------------------------------------------------------

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// The native file version numbers and some comments about what they can cope with
typedef enum
{
	// First version number
	// Created: 29/5/95
	// Copes with:
	// - File compression
	// Does not cope with:  
	FIRSTVERSION = 100
	
} NativeFileVersion;	

// Define what the current read and write versions are
const NativeFileVersion 	ReadNativeVersion100 		= FIRSTVERSION;
const NativeFileVersion 	WriteNativeVersion100 		= FIRSTVERSION;
const double 	ReadNativeVersion 		= (ReadNativeVersion100/100);
const double 	WriteNativeVersion 		= (WriteNativeVersion100/100);


// Record handler list used by all BaseCamelotFilter derived classes
List BaseCamelotFilter::RecordHandlerList;
BOOL BaseCamelotFilter::RecordHandlersCreated = FALSE;

/********************************************************************************************
//// Start of preference variables
********************************************************************************************/

/********************************************************************************************

	Preference:		DefaultExportPath
	Section:		Filters
	Range:			-
	Purpose:		String to hold the default path for the optional web export path
	SeeAlso:		-

********************************************************************************************/

String_256 BaseCamelotFilter::DefaultExportPath;

/********************************************************************************************

	Preference:		ExportWebFile
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether the user has requested to export a web file when native saving.
	SeeAlso:		0 to 1

********************************************************************************************/
 
BOOL BaseCamelotFilter::ExportWebFile = TRUE;


/********************************************************************************************

	Preference:		CompressNative
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether we compress the native files saved from Xara Studio.
					True means do compress the files.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::CompressNative = TRUE;

/********************************************************************************************

	Preference:		SaveXPEBitmaps
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether we compress the native files saved from Xara Studio.
					True means do save the XPE bitmaps.
					False means don't save them, just save the XPE info and rebuild
					when loading
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::SaveXPEBitmaps = FALSE;

/********************************************************************************************

	Preference:		BitmapCompression
	Section:		Filters
	Range:			0 to 201
	Purpose:		How much compression the user has requested when saving bitmaps.
					0 means maximum compression e.g. use JPEG filter at 0% lossy compression
					99 means minimum compression e.g. use JPEG filter at 99% lossy compression
					101 - 201 means lossless compression e.g. us PNG filter. Range stores the
					old compression setting.
					Could use the BMPZIP filter instead.
					(Very much like the JPEG compression percentage).
	SeeAlso:		-

********************************************************************************************/

#ifdef WEBSTER
// Webster uses web files as its default file format. Therefore, to default to compressing
// the bitmap is bad. This will turn it off but default to 75 if the user turns it on. 
INT32 BaseCamelotFilter::BitmapCompression = 176;	// add 101 to turn compression off
#else
// Camelot needs to default to savin in compressed format, where 75 is a good starting quality.
INT32 BaseCamelotFilter::BitmapCompression = 75;
#endif

/********************************************************************************************

	Preference:		PreviewBitmapExport
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether the user has requested a preview bitmap when
					exporting as a web file.
					True means do export a preview bitmap.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::PreviewBitmapExport = FALSE;

/********************************************************************************************

	Preference:		ConvertTextToOutlines
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether the user has requested a convert text to outlines when
					exporting as a web file.
					True means do convert text to outlines.
	SeeAlso:		-

********************************************************************************************/

// WEBSTER - markn 24/4/97
// Webster can default to FALSE now, because the current compact renderer can now cope with text
BOOL BaseCamelotFilter::ConvertTextToOutlines = FALSE;

/********************************************************************************************

	Preference:		ConvertBlendsToOutlines
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether the user has requested a convert text to outlines when
					exporting as a web file.
					True means do convert text to outlines.

					Added for WEBSTER - markn 28/1/97
	SeeAlso:		-

********************************************************************************************/

// WEBSTER - markn 24/4/97
// Webster can default to FALSE now, because the current compact renderer can now cope with blends
BOOL BaseCamelotFilter::ConvertBlendsToOutlines = FALSE;

/********************************************************************************************

	Preference:		PreviewBitmapFilterType
	Section:		Filters
	Range:			0 to 4
	Purpose:		What type of preview bitmap filter we should be using when
					exporting as a web or native file.
					0 - means BMP.
					1 - means GIF.
					2 - means JPEG.
					3 - means PNG.
					4 - means TIFF.
	SeeAlso:		-

********************************************************************************************/

INT32 BaseCamelotFilter::PreviewBitmapFilterType = 1;


/********************************************************************************************

	Preference:		RemoveInvisibleLayers
	Section:		Filters
	Range:			0 to 1
	Purpose:		Whether invisible layers should be removed or not when
					exporting as a web or native file.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::RemoveInvisibleLayers = 1;

/********************************************************************************************

	Preference:		RemoveUnusedColours
	Section:		Filters
	Range:			0 to 1
	Purpose:		Whether unused colours should be removed or not when
					exporting as a web or native file.
	SeeAlso:		-

********************************************************************************************/

// WEBSTER - markn 11/2/97 - Defaults to FALSE for WEBSTER
#ifndef WEBSTER
BOOL BaseCamelotFilter::RemoveUnusedColours = TRUE;
#else
BOOL BaseCamelotFilter::RemoveUnusedColours = FALSE;
#endif // WEBSTER

/********************************************************************************************

	Preference:		HTMLToClipboard
	Section:		Filters
	Range:			0 to 1
	Purpose:		Whether to export an HTML tag to the clipboard when exporting
					a Web file
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::HTMLToClipboard = FALSE;

/********************************************************************************************

	Preference:		MinimalWebFormat
	Section:		Filters
	Range:			0 to 1
	Purpose:		Whether we should use the minimal web format or not when exporting as a
					web file. The alternative is to be a lot closer to native format.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::MinimalWebFormat = 0;

/********************************************************************************************

	Preference:		WriteRelativePaths
	Section:		Filters
	Range:			0 to 1
	Purpose:		When this is 1, all paths will be written out in a relative format
					0 means use an absolute format.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::WriteRelativePaths = 1;

/********************************************************************************************

	Preference:		DontConvertTheseFontsToOutlines
	Section:		Filters
	Range:			A string of font typefaces
	Purpose:		This string defines the list of font typefaces that won't be converted
					to outlines when exporting to the web format

					The string should consist of a number of typeface names, delimited by ';'
					e.g. "Times New Roman;Arial;CoocheeCoo"
	SeeAlso:		-

********************************************************************************************/

String_256 BaseCamelotFilter::DontConvertTheseFontsToOutlines;

/********************************************************************************************

	Preference:		NativeCheckSimilarPaths
	Section:		Filters
	Range:			0 to 1
	Purpose:		When this is 1, the default, paths will be checked for similarities
					during saving.
					When 0 this check is not made.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::NativeCheckSimilarPaths = 0;

/********************************************************************************************

	Preference:		WebSimilarPathTolerence
	Section:		Filters
	Range:			0 to 500
	Purpose:		This determines the tolerence that is used when checking for similar paths
					in web files. It is measured in millipoints.
	SeeAlso:		-

********************************************************************************************/

INT32 BaseCamelotFilter::WebSimilarPathTolerence = 200;


/********************************************************************************************

	Preference:		ImportNonFramesOntoActivelayer
	Section:		Filters
	Range:			0 to 1
	Purpose:		When this is 1, the default, import of non-frame layers will happen onto
					the active layer.
					When 0, import of non-frame layers will be onto the first non-frame layer
					in the import.
	SeeAlso:		-

********************************************************************************************/

BOOL BaseCamelotFilter::ImportNonFramesOntoActivelayer = TRUE;

/********************************************************************************************
//// End of preference variables
********************************************************************************************/



/********************************************************************************************

>	BaseCamelotFilter::BaseCamelotFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	Constructor for the base Camelot save/load filter off which are derived the
				v2 native file and web filters.
				This filter provides common functionality for these derived filters.

********************************************************************************************/

BaseCamelotFilter::BaseCamelotFilter()
{
	// We are a hidden filter which just contains common functionality
	// This filter should never be set up or seen, the derived classes just use it.

	// Items to handle the file compression
	CompressionType = 0;	// type of compression in use (0 at present)
	CompressionOn	= FALSE;// on/off flag

	FileVersionNumber	= 0.0;
	BuildVersionNumber	= 0.0;

	pColComponent	= NULL;
	pBmpComponent	= NULL;
	pUnitsComponent	= NULL;
	pInfoComponent	= NULL;
	pViewComponent	= NULL;
	pPrintComponent	= NULL;
	pFontComponent	= NULL;
	
	pTheSpread		= NULL;

	pUnitsComponent = NULL;
	pInfoComponent  = NULL;

	pInsertContextNode	= NULL;
	InsertNextAsChild	= FALSE;
	pInsertLevelStack	= NULL;

	pCXaraFile				= NULL;
	pProgress				= NULL;
	ProgressBarCount		= 0;
	TotalProgressBarCount	= 0;

	pPrefs = NULL;

	CoordOrigin = DocCoord(0,0);	// Zero origin

	pLastDocument		= NULL;
	pLastChapter		= NULL;
	pLastSpread			= NULL;
	pLastLayer			= NULL;
	pLastNodePath		= NULL;
	pLastCurrentAttr	= NULL;
	DocumentCount		= 0;
	ChapterCount		= 0;
	SpreadCount			= 0;
	LayerCount			= 0;
	SetSentinelCount	= 0;

	pImportLayer = NULL;

	EscapePressed = FALSE;

  	pAtomicTagList		= NULL;
	pEssentialTagList	= NULL;
	pTagDescriptionList	= NULL;

	pPathRecordRefList	= NULL;
// WEBSTER - markn 31/1/97
// Replaced with general system
//	pTextStoryGroupRefList = NULL;

	// WEBSTER - markn 29/1/97
	// Part of the general form of the system used to convert text to outlines in v1.5
	pNodeGroupRefList = NULL;

  	PreCompFlags = 0;

	// WEBSTER - markn 15/2/97
	// Bug fix for selection type save option
	SelType = DRAWING;

	// reset our import layer variables
	m_pFirstImportedLayer		= NULL;
	m_pActiveLayerBeforeImport	= NULL;

	// Make sure this is safely initialised as NodeSetSentinel hackery relies on it
	// being null except when really in use.
	ImportInfo.pOp = 0;

	// State variables that help to switch functionality of AttachNode from adding nodes
	// to the tree to making current attributes
	// Used by Current Attribute loading mechanism
	m_InsertMode			= INSERTMODE_ATTACHTOTREE;
	m_pCurrentAttrGroup = NULL;
}


/********************************************************************************************

>	BaseCamelotFilter::~BaseCamelotFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Purpose:	Tidies up when the filter is destroyed.

********************************************************************************************/

BaseCamelotFilter::~BaseCamelotFilter()
{
	ResetImportExportVars();
//	DestroyRecordHandlers();

	// If we have our preferences class present, then destory it
#ifdef DO_EXPORT
	if (pPrefs)
	{
		PORTNOTETRACE("other","BaseCamelotFilter::~BaseCamelotFilter - removed delete pPrefs");
#ifndef EXCLUDE_FROM_XARALX
		delete pPrefs;
#endif
		pPrefs = NULL;
	}
#endif
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Initalises the Filter ready for use. Will fail if it can not get enough
				memory to work with.
				This is the base class filter and so should be overridden by the derived
				classes.

********************************************************************************************/

BOOL BaseCamelotFilter::Init()
{
	ERROR3("Base Camelot filter class Init called");
	return FALSE;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::Deinit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Purpose:	Deinits the filter when the program shuts down

********************************************************************************************/

void BaseCamelotFilter::Deinit()
{
	DestroyRecordHandlers();
}

/********************************************************************************************
/ Start of Preference setting and getting code
********************************************************************************************/

/********************************************************************************************

>	static BOOL BaseCamelotFilter::SetNativeCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		New state for native file compression, TRUE if want on, FALSE if want off.
	Returns:	The old state of the compression.
	Purpose:	Set the preference as to whether we are to compress the native files or not.

********************************************************************************************/

BOOL BaseCamelotFilter::SetNativeCompression(BOOL NewState)
{
//	BOOL OldState = CompressNative;
	CompressNative = NewState;
	return CompressNative;
}


/********************************************************************************************

>	static BOOL BaseCamelotFilter::GetNativeCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		New state for native file compression, TRUE if want on, FALSE if want off.
	Returns:	The old state of the compression.
	Purpose:	Set the prefernece as to whether we are to compress the native files or not.

********************************************************************************************/

BOOL BaseCamelotFilter::GetNativeCompression()
{
	return CompressNative;
}	


/********************************************************************************************

>	static BOOL BaseCamelotFilter::ShouldSaveXPEBitmaps()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Returns:	The current state of the SaveXPEBitmaps preference
	Purpose:	Find out whether we should save XPE bitmap data or just the information
				needed to rebuild the bitmap

********************************************************************************************/

BOOL BaseCamelotFilter::ShouldSaveXPEBitmaps()
{
	return SaveXPEBitmaps;
}	


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetPreCompression(UINT32 Flags)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		Flags = 32 bit flags word defining the precompression used in the file
	Returns:	True if succesful or False if not.
	Purpose:	Sets the precompression used in this file

********************************************************************************************/

BOOL BaseCamelotFilter::SetPreCompression(UINT32 Flags)
{
	PreCompFlags = Flags;

	if (Flags != 0)
	{
		Error::SetError(_R(IDS_UNKNOWN_COMPRESSION));
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetFileCompressionState(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		New state for file compression, TRUE if want on, FALSE if want off.
	Returns:	True if succesful or False if not.
	Purpose:	Set a new file compression status into action.

********************************************************************************************/

BOOL BaseCamelotFilter::SetFileCompressionState(BOOL NewState)
{
	TRACEUSER( "Neville", _T("BaseCamelotFilter::SetFileCompressionState new state= %d\n"), NewState );

	CompressionOn = NewState;

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetFileCompressionState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Returns:	The current state of the file compression.
	Purpose:	Get the current file compression state. True if compressing.

********************************************************************************************/

BOOL BaseCamelotFilter::GetFileCompressionState()
{
	return CompressionOn;
}

/********************************************************************************************

>	virtual INT32 BaseCamelotFilter::GetBitmapCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		-
	Returns:	The current state of the bitmap compression.
	Purpose:	Public access to the current bitmap compression which the user has requested
				to use when saving bitmaps in the web format.
				0 means maximum compression e.g. use JPEG filter at 0% lossy compression
				100 means minimum compression e.g. use JPEG filter at 100% lossy compression
				101 - 200 means lossless compression e.g. us PNG filter.
				Could use the no bitmap compression at all e.g. use BMP filter. Unused at present.
				(Very much like the JPEG compression percentage).
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

INT32 BaseCamelotFilter::GetBitmapCompression()
{
	return BaseCamelotFilter::BitmapCompression;
}

/********************************************************************************************

>	virtual INT32 BaseCamelotFilter::SetBitmapCompression(INT32 NewBmpComp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		New bitmap compression value
	Returns:	The old current state of the bitmap compression.
	Purpose:	Set up a new state for the the current bitmap compression.
				200 = no compression, 0 = full compression.
				(Very much like the JPEG compression percentage).

********************************************************************************************/

INT32 BaseCamelotFilter::SetBitmapCompression(INT32 NewBmpComp)
{
	INT32 Old = BaseCamelotFilter::BitmapCompression;
	BaseCamelotFilter::BitmapCompression = NewBmpComp;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetPreviewBitmapExport()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		-
	Returns:	The current state of the export preview bitmap.
	Purpose:	Public access to the current export preview bitmap which the user has requested
				to use when saving bitmaps in the web format.
				True means export a preview bitmap, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL BaseCamelotFilter::GetPreviewBitmapExport()
{
#ifdef _BATCHING
	return FALSE;
#else
	return BaseCamelotFilter::PreviewBitmapExport;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetPreviewBitmapExport(BOOL NewExportPreviewBmp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		New export preview bitmap state
	Returns:	The old current state of the export preview bitmap.
	Purpose:	Set up a new state for the the current export preview bitmap.
				True means export a preview bitmap, False means do not.

********************************************************************************************/

BOOL BaseCamelotFilter::SetPreviewBitmapExport(BOOL NewExportPreviewBmp)
{
	BOOL Old = BaseCamelotFilter::PreviewBitmapExport;
	BaseCamelotFilter::PreviewBitmapExport = NewExportPreviewBmp;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetConvertTextToOutlines()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		-
	Returns:	The current state of the convert text to outlines.
	Purpose:	Public access to the current convert text to outlines which the user has requested
				to use when saving in the web format.
				True means convert text to outlines, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL BaseCamelotFilter::GetConvertTextToOutlines()
{
#ifdef _BATCHING
	return FALSE;
#else
	return BaseCamelotFilter::ConvertTextToOutlines;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetConvertTextToOutlines(BOOL NewConvertTextToOutlines)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		New convert text to outlines state
	Returns:	The old current state of the convert text to outlines.
	Purpose:	Set up a new state for the the convert text to outlines.
				True means convert text to outlines, False means do not.

********************************************************************************************/

BOOL BaseCamelotFilter::SetConvertTextToOutlines(BOOL NewConvertTextToOutlines)
{
	BOOL Old = BaseCamelotFilter::ConvertTextToOutlines;
	BaseCamelotFilter::ConvertTextToOutlines = NewConvertTextToOutlines;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetConvertBlendsToOutlines()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/97
	Inputs:		-
	Returns:	The current state of the convert blends to outlines.
	Purpose:	Public access to the current convert text to outlines which the user has requested
				to use when saving in the web format.
				True means convert blends to outlines, False means do not.

				Added for WEBSTER

	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL BaseCamelotFilter::GetConvertBlendsToOutlines()
{
#ifdef _BATCHING
	return FALSE;
#else
	return BaseCamelotFilter::ConvertBlendsToOutlines;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetConvertBlendsToOutlines(BOOL NewConvertBlendsToOutlines)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/97
	Inputs:		New convert blends to outlines state
	Returns:	The old current state of the convert blends to outlines.
	Purpose:	Set up a new state for the the convert blends to outlines.
				True means convert blends to outlines, False means do not.

				Added for WEBSTER

********************************************************************************************/

BOOL BaseCamelotFilter::SetConvertBlendsToOutlines(BOOL NewConvertBlendsToOutlines)
{
	BOOL Old = BaseCamelotFilter::ConvertBlendsToOutlines;
	BaseCamelotFilter::ConvertBlendsToOutlines = NewConvertBlendsToOutlines;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetRemoveInvisibleLayers()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the remove invisible layers.
	Purpose:	Public access to the current remove invisible layers which the user has requested
				to use when saving in the web format.
				True means remove invisible layers, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL BaseCamelotFilter::GetRemoveInvisibleLayers()
{
	return BaseCamelotFilter::RemoveInvisibleLayers;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetRemoveInvisibleLayers(BOOL NewRemoveInvisibleLayers);

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		New remove invisible layers state
	Returns:	The old current state of the remove invisible layers.
	Purpose:	Set up a new state for the the remove invisible layers.
				True means remove invisible layers, False means do not.

********************************************************************************************/

BOOL BaseCamelotFilter::SetRemoveInvisibleLayers(BOOL NewRemoveInvisibleLayers)
{
	BOOL Old = BaseCamelotFilter::RemoveInvisibleLayers;
	BaseCamelotFilter::RemoveInvisibleLayers = NewRemoveInvisibleLayers;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetRemoveUnusedColours()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		-
	Returns:	The current state of the remove unused colours.
	Purpose:	Public access to the current remove unused colours which the user has requested
				to use when saving in the web format.
				True means remove unused colours, False means do not.
	Note:		Virtual so that can be overriden in native filter to save in the proper native
				state

********************************************************************************************/

BOOL BaseCamelotFilter::GetRemoveUnusedColours()
{
#ifdef WEBSTER
	return FALSE;	// WEBSTER - markn 14/2/97 - Don't remove unused colours in Webster
#else
	return BaseCamelotFilter::RemoveUnusedColours;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetHTMLToClipboard(BOOL fNewValue)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/97
	Inputs:		New HTMLToClipboard state
	Returns:	The old HTMLToClipboard state
	Purpose:	Set a new state for HTMLToClipboard

********************************************************************************************/

BOOL BaseCamelotFilter::SetHTMLToClipboard(BOOL fNewValue)
{
	BOOL Old = BaseCamelotFilter::HTMLToClipboard;
	BaseCamelotFilter::HTMLToClipboard = fNewValue;
	return Old;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::ShouldExportHTMLTag()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/97
	Inputs:		-
	Returns:	The current state of the HTMLToClipboard
	Purpose:	Public access to the HTML to clipboard
	
********************************************************************************************/

BOOL BaseCamelotFilter::ShouldExportHTMLTag()
{
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetRemoveUnusedColours(BOOL NewRemoveUnusedColours)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/96
	Inputs:		New remove unused colours state
	Returns:	The old current state of the remove unused colours.
	Purpose:	Set up a new state for the the remove unused colours.
				True means remove unused colours, False means do not.

********************************************************************************************/

BOOL BaseCamelotFilter::SetRemoveUnusedColours(BOOL NewRemoveUnusedColours)
{
	BOOL Old = BaseCamelotFilter::RemoveUnusedColours;
	BaseCamelotFilter::RemoveUnusedColours = NewRemoveUnusedColours;
	return Old;
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetMinimalWebFormat()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/97
	Inputs:		-
	Returns:	The current state of the min web format flag
	Purpose:	Public access function

				WEBSTER - 28/1/97				
	Note:		

********************************************************************************************/

BOOL BaseCamelotFilter::GetMinimalWebFormat()
{
	return BaseCamelotFilter::MinimalWebFormat;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetMinimalWebFormat(BOOL NewMinimalWebFormat)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/1/97
	Inputs:		New min web format state
	Returns:	The old current state.
	Purpose:	Set up a new state for the min web format flag

				WEBSTER - 28/1/97				

********************************************************************************************/

BOOL BaseCamelotFilter::SetMinimalWebFormat(BOOL NewMinimalWebFormat)
{
	BOOL Old = BaseCamelotFilter::MinimalWebFormat;
	BaseCamelotFilter::MinimalWebFormat = NewMinimalWebFormat;
	return Old;
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetSaveXPEBitmaps()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/05
	Inputs:		-
	Returns:	The current state of the save XPE bitmaps flag
	Purpose:	Public access function

********************************************************************************************/

BOOL BaseCamelotFilter::GetSaveXPEBitmaps()
{
	return BaseCamelotFilter::SaveXPEBitmaps;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetSaveXPEBitmaps(BOOL NewSaveXPEBitmaps)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/09/05
	Inputs:		New save XPE bitmaps state
	Returns:	The old current state.
	Purpose:	Set up a new state for the save XPE bitmaps flag

********************************************************************************************/

BOOL BaseCamelotFilter::SetSaveXPEBitmaps(BOOL NewSaveXPEBitmaps)
{
	BOOL Old = BaseCamelotFilter::SaveXPEBitmaps;
	BaseCamelotFilter::SaveXPEBitmaps = NewSaveXPEBitmaps;
	return Old;
}


/********************************************************************************************

>	virtual String_256 BaseCamelotFilter::GetDontConvertTheseFontsToOutlinesString()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		-
	Returns:	The string of fonts that shouldn't be converted to shapes
	Purpose:	Access func to that fint list dude.

********************************************************************************************/

String_256 BaseCamelotFilter::GetDontConvertTheseFontsToOutlinesString()
{
	return DontConvertTheseFontsToOutlines;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetNativeCheckSimilarPaths()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Inputs:		-
	Returns:	The current state of the Native check similar paths flag.
	Purpose:	Public access to the current Native check similar paths flag setting.

********************************************************************************************/

BOOL BaseCamelotFilter::GetNativeCheckSimilarPaths()
{
	return BaseCamelotFilter::NativeCheckSimilarPaths;
}

/********************************************************************************************

>	virtual MILLIPOINT BaseCamelotFilter::GetWebSimilarPathTolerence()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/96
	Inputs:		-
	Returns:	The current state of the Web similar path tolerence.
	Purpose:	Public access to the current Web similar path tolerence setting.
				It is measured in millipoints.

********************************************************************************************/

MILLIPOINT BaseCamelotFilter::GetWebSimilarPathTolerence()
{
	return (MILLIPOINT)BaseCamelotFilter::WebSimilarPathTolerence;
}

/********************************************************************************************
/ End of Preference setting and getting code
********************************************************************************************/

/********************************************************************************************

>	static BOOL BaseCamelotFilter::SkipToPreviewBitmap(	CCLexFile * pFile, 
														BOOL *IsNewNativeFile, 
														UINT32 * pFilterId = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pFile	- file to use to find the preview bitmap in
	Outputs:	IsNewNativeFile - set to TRUE if new native/web format file
								- set to FALSE if not new native/web format file
				pFilterId		- if not NULL and we found a preview set to the corresponding
								  filter id, complementing ExportPreviewBitmap.
	Returns:	True if it is a native/web format file and found the preview bitmap.
				False if any problems are found or the file format is wrong.
	Purpose:	Function used by the gallery thumb nail code to check the file is the native/web
				format and then get to the preview bitmap record and leave the file pointer
				sitting at the bitmap image.
				Assumes file is already opened and ready for reading at the start of the file.
	Notes:		Added pFilterId argument for open dialog Martin-20/01/97
	SeeAlso:	SGThumbs::LoadThumbnailIntoSlot; BaseFileDialog::GetPreviewBitmapFromFile

********************************************************************************************/

BOOL BaseCamelotFilter::SkipToPreviewBitmap(CCLexFile * pFile, BOOL *IsNewNativeFile, UINT32 * pFilterId)
{
#if !defined(EXCLUDE_FROM_RALPH)

	// We will set by default that the file is not a new native/web file, so that this
	// is returned this info to the caller
	if (IsNewNativeFile)
		*IsNewNativeFile = FALSE;

	// If no file pointer then exit with no errors
	if (pFile == NULL)
		return FALSE;

	// To speed up the checks we will just check this number of records before giving up.
	// Assuming we do not find something like a Compression Start or equally compelling reason
	// to give up the search.
	const INT32 MaxRecordNumber = 5;
	INT32 RecordNumber = 0;

	// First check the header on the file to see whether it is our unique one
	BYTE Buffer[8];
	UINT32* pUINT32 = (UINT32*)&Buffer;
	try
	{
		// Read in the first 8 bytes
		pFile->read(pUINT32, 8);
		// Check it is our unique signature
		if (pUINT32[0] != CXF_IDWORD1 || pUINT32[1] != CXF_IDWORD2)
			return FALSE;

		// More than likely the file is a new native/web file, so return this info to the caller
		if (IsNewNativeFile)
			*IsNewNativeFile = TRUE;

		// Now skip records until we either find a preview bitmap record
		// or something like compression start or file end
		// Count the records we have skipped, if 
		UINT32 Tag = 0;
		UINT32 Size = 0;
		while (RecordNumber < MaxRecordNumber)
		{
			// Read in the record header
			pFile->read(pUINT32, 8);
			// Tag is first followed by the size 
			Tag = pUINT32[0];
			Size = pUINT32[1];
			if (Tag == TAG_PREVIEWBITMAP_BMP || Tag == TAG_PREVIEWBITMAP_GIF ||
				Tag == TAG_PREVIEWBITMAP_JPEG || Tag == TAG_PREVIEWBITMAP_PNG ||
				Tag == TAG_PREVIEWBITMAP_TIFFLZW)
			{
				// We have found our preview bitmap header, and the file pointer will
				// be at the correct place, i.e. at the start of the image data  and
				// so we pass back the filter type if needed
				if (pFilterId)
				{
					switch (Tag)
					{
						case TAG_PREVIEWBITMAP_BMP:
							*pFilterId = FILTERID_PREVIEW_BMP;
							break;
						case TAG_PREVIEWBITMAP_GIF:
							*pFilterId = FILTERID_PREVIEW_GIF;
							break;
						case TAG_PREVIEWBITMAP_JPEG:
							*pFilterId = FILTERID_PREVIEW_JPEG;
							break;
						case TAG_PREVIEWBITMAP_PNG:
							*pFilterId = FILTERID_PREVIEW_PNG;
							break;
						case TAG_PREVIEWBITMAP_TIFFLZW:
							*pFilterId = FILTERID_PREVIEW_TIFF;
							break;
						default:
							*pFilterId = FILTERID_NONE;
							break;
					}
				}
				return TRUE;
			}
			else if (Tag == TAG_STARTCOMPRESSION || Tag == TAG_ENDOFFILE ||
					 Tag == TAG_DOWN || Tag == TAG_DOCUMENT)
			{
				// We have reached something that indicates we have gone too far
				// so return False = no preview bitmap found
				return FALSE;
			}
			
			// Tag not recognised
			// Skip to the next record in the file 
			// Move on by the size of the data in this record
			FilePos pos = pFile->tell(); 
			pFile->seekIn(pos + Size);
			RecordNumber ++;
		}
	}		
	catch( CFileException e )
	{
		return FALSE;
	}

	if (RecordNumber > MaxRecordNumber)
		TRACEUSER( "Neville", _T("BaseCamelotFilter::SkipToPreviewBitmap couldn't find preview bitmap in %d records\n"), RecordNumber);

	return FALSE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	static BOOL BaseCamelotFilter::SkipToIndexInformation(CCLexFile * pFile, String_256 *pString, BOOL *IsNewNativeFile)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pFile	- file to use to find the index information in
	Outputs:	pString	- will fill this in with the index information, if found
				IsNewNativeFile - set to TRUE if new native/web format file
								- set to FALSE if not new native/web format file
	Returns:	True if it is a native/web format file and found the index information.
				False if any problems are found or the file format is wrong.
	Purpose:	Function used by the gallery index generation code to check the file is the
				native/web	format and then get to the index information record.
				Assumes file is already opened and ready for reading at the start of the file.
				Could share some common functionality with the above but we need speed so 
				just duplicate the code.
				This function must also be different as we need to use the CXaraFile code as
				we need to read the string from the document comment record. Hence, we can get
				// this to do some of the work such as checking the id is present etc.
	SeeAlso:	SGThumbs::LoadThumbnailIntoSlot;

********************************************************************************************/

BOOL BaseCamelotFilter::SkipToIndexInformation(CCLexFile * pFile, String_256 *pString, BOOL *IsNewNativeFile)
{
#if !defined(EXCLUDE_FROM_RALPH)

	// If no file pointer then exit with no errors
	if (pFile == NULL || pString == NULL)
		return FALSE;

	// To speed up the checks we will just check this number of records before giving up.
	// Assuming we do not find something like a Compression Start or equally compelling reason
	// to give up the search.
	const INT32 MaxRecordNumber = 5;
	INT32 RecordNumber = 0;

	// Create the correct CXaraFile object for the job, return FALSE if this fails
	// Cannot use the OpenToRead function as this is not static
	// All we need to set up is the CCFile pointer inside CXaraFile 
	CXaraFile* pCXaraFile = new CXaraFile;
	if (!pCXaraFile)
		return FALSE;

	// Call the CXaraFile code to open up the pFile. Doesn't really open the file just inits
	// everything required. This also checks the header on the file to see whether it is our unique one
	BOOL ok = pCXaraFile->OpenToRead(pFile);
	if (!ok)
	{
		// More than likely the file is not a new native/web file, so return this info to the caller
		if (IsNewNativeFile)
			*IsNewNativeFile = FALSE;
		// Clean up the CXaraFile and delete it.
		pCXaraFile->Close();
		delete pCXaraFile;
		return FALSE;
	}

	// More than likely the file is a new native/web file, so return this info to the caller
	if (IsNewNativeFile)
		*IsNewNativeFile = TRUE;

	// Assumes the header has been read and checked by OpenToRead
	BYTE Buffer[8];
	UINT32* pUINT32 = (UINT32*)&Buffer;
	try
	{
		// Now skip records until we either find a document comment record
		// or something like compression start or file end
		// This will usually be stored after a document record and a down node and
		// before a chapter or compression start record.
		// Count the records we have skipped, if any
		UINT32 Tag = 0;
		UINT32 Size = 0;
		while (RecordNumber < MaxRecordNumber)
		{
			// Read in the record header
			pFile->read(pUINT32, sizeof(UINT32) * 2);

			// Tag is first followed by the size 
			Tag = pUINT32[0];
			Size = pUINT32[1];
			if (Tag == TAG_DOCUMENTCOMMENT)
			{
				// We have found our index information record header
				// so rip the data from it
				ok = pCXaraFile->ReadUnicode(*pString, pString->MaxLength());
				break;
			}
			else if (Tag == TAG_STARTCOMPRESSION ||
					 Tag == TAG_ENDOFFILE || 
					 Tag == TAG_CHAPTER ||
					 Tag == TAG_SETSENTINEL ||
					 Tag == TAG_SETPROPERTY)
			{
				// We have reached something that indicates we have gone too far
				// so return False = no preview bitmap found
				ok = FALSE;
				break;
			}
			
			// Tag not recognised
			// Skip to the next record in the file 
			// Move on by the size of the data in this record
			FilePos pos = pFile->tell(); 
			pFile->seekIn(pos + Size);
			RecordNumber ++;
		}

		// Clean up the CXaraFile and delete it.
		pCXaraFile->Close();
		delete pCXaraFile;
	}		
	catch( CFileException e )
	{
		// Clean up the CXaraFile and delete it.
		pCXaraFile->Close();
		delete pCXaraFile;
		return FALSE;
	}

	if (!ok && RecordNumber > MaxRecordNumber)
		TRACEUSER("Neville", _T("BaseCamelotFilter::SkipToIndexInformation couldn't find preview bitmap in %d records\n"), RecordNumber);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL Filter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to load the default document before
				importing the bitmap into this file.
				In this baseclass version return FALSE and hence assume that the filters that
				need to will override this function to return TRUE.
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport;

********************************************************************************************/

BOOL BaseCamelotFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// No need to check the pathname, just return FALSE as most filters will not require the
	// default document.
	return FALSE;
}	

/********************************************************************************************

>	virtual INT32 BaseCamelotFilter::HowCompatible( PathName& Filename, ADDR  HeaderStart,
													UINT32 HeaderSize,
													UINT32 FileSize )
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		FileName	The name of the file being imported
				HeaderStart	The address of the start of the data from the file which needs checking
				HeaderSize	The size of this data that we should check
				FileSize	The total size of the file
	Returns:	10 if the file is fully recognised or 0 otherwise
	Purpose:	returns value in range 0 to 10 which indicates how much we like this file:
					0 => "Definitely Not"
					10 => "Yes Sir, that's my file format"

********************************************************************************************/

INT32 BaseCamelotFilter::HowCompatible( PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize,
						   UINT32 FileSize )
{
	// How compatible we think the file is
	INT32 HowCompatible = 0;

	// Check that we've got enough data to do our check
	if (HeaderSize < 8)
	{
		// Not enough data - ignore this file.
		return 0;
	}

	// Check that the first eight bytes are our magic identifier
	UINT32* pUINT32 = (UINT32*)HeaderStart;
	UINT32 p0 = LEtoNative(HeaderSize>=4  ? pUINT32[0] : 0);
	UINT32 p1 = LEtoNative(HeaderSize>=8  ? pUINT32[1] : 0);
	UINT32 p2 = LEtoNative(HeaderSize>=12 ? pUINT32[2] : 0);
	UINT32 p3 = LEtoNative(HeaderSize>=16 ? pUINT32[3] : 0);
	UINT32 p4 = LEtoNative(HeaderSize>=20 ? pUINT32[4] : 0);
//	UINT32 p5 = LEtoNative(HeaderSize>=24 ? pUINT32[5] : 0);

	if (p0 == CXF_IDWORD1 && p1 == CXF_IDWORD2)
	{
		// Should now check that the unique identifier inside the file header record
		// corresponds to this filter.
		
		// The next 8 bytes should be the tag and size of the header record
		// Not too much of a problem if this is not the case as both the native and web filter
		// will return 10 and so the first one in the list or the one with the most appropriate
		// file type will be chosen to load the filter.
		if (HeaderSize > (5 * 4) && p2 == TAG_FILEHEADER && p3 > 3)
		{
/*
			// We have found the file header record
			// The first 3 bytes should be either CXW or CXN
			UINT32 Type = pUINT32[4] & 0x00FFFFFF; // mask off top byte
			// Ask the filter what its unique identifier is and compare against the
			// one we have just got from the file
			char * fred = GetFileType();
			UINT32 CheckType = *(UINT32*)fred;
			if (Type == CheckType)
				HowCompatible = 10;	// we like this file a lot
			else
				HowCompatible = 9;	// we may like this file but give another filter a better chance
*/
		
			UINT32 Type = p4 & 0x00FFFFFF; // mask off top byte
			HowCompatible = HowCompatibleIsFileType((char*)(&Type));		
		}
		else
		{
			// We like this file
			HowCompatible = 10;
		}
	}
				
TRACEUSER( "Neville", _T("BaseCamelotFilter::HowCompatible returning = %d\n"),HowCompatible);
	// Return the found value to the caller.
	return HowCompatible;
}

/********************************************************************************************

>	static BOOL BaseCamelotFilter::CreateRecordHandlers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if not.
	Purpose:	Creates all the record handlers used to import files

********************************************************************************************/

// Helper macro for BaseCamelotFilter::CreateRecordHandlers()
#define NEW_RECORD_HANDLER(HandlerClassName)				\
{															\
	CamelotRecordHandler* pCamelotRecordHandler;			\
	pCamelotRecordHandler = new HandlerClassName;			\
	if (pCamelotRecordHandler != NULL)						\
		RecordHandlerList.AddTail(pCamelotRecordHandler);	\
	else													\
		return FALSE;										\
}															\

BOOL BaseCamelotFilter::CreateRecordHandlers()
{
	if (!RecordHandlersCreated)
	{
		RecordHandlerList.DeleteAll();

		// General handlers
		NEW_RECORD_HANDLER(GeneralRecordHandler);		// The general record handler
		NEW_RECORD_HANDLER(CompressionRecordHandler);	// The compression record handler
		NEW_RECORD_HANDLER(DocumentRecordHandler);		// Document record handler
		NEW_RECORD_HANDLER(ViewRecordHandler);			// View records handler

		// Reuseable data item handlers
		NEW_RECORD_HANDLER(ColourRecordHandler);		// Define colour record handler
		NEW_RECORD_HANDLER(BitmapRecordHandler);		// Define bitmap record handler

		// Document related items
		NEW_RECORD_HANDLER(DocInfoRecordHandler);		// Document info record handler
		NEW_RECORD_HANDLER(UnitsRecordHandler);			// Units record handler

		// Attribute handlers
		NEW_RECORD_HANDLER(LineAttrRecordHandler);		// Line attribute record handler
		NEW_RECORD_HANDLER(FillAttrRecordHandler);		// Fill attribute record handler
		NEW_RECORD_HANDLER(GeneralAttrRecordHandler);	// Misc attribute record handler

		// Object handlers
		NEW_RECORD_HANDLER(PathRecordHandler);			// Path record handler
		NEW_RECORD_HANDLER(PathFlagsRecordHandler);		// Path flags record handler
		NEW_RECORD_HANDLER(GroupRecordHandler);			// Group record handler
		NEW_RECORD_HANDLER(BlendRecordHandler);			// Blend-related record handler
		NEW_RECORD_HANDLER(MouldRecordHandler);			// Mould-related record handler

		NEW_RECORD_HANDLER(RegularShapeRecordHandler);
		NEW_RECORD_HANDLER(EllipseRecordHandler);
		NEW_RECORD_HANDLER(RectangleRecordHandler);
		NEW_RECORD_HANDLER(PolygonRecordHandler);

		NEW_RECORD_HANDLER(TextObjRecordHandler);
		NEW_RECORD_HANDLER(TextAttrRecordHandler);
		NEW_RECORD_HANDLER(FontDefRecordHandler);

PORTNOTE("other","Commented ImagesettingAttrRecordHandler")
#ifndef EXCLUDE_FROM_XARALX
		NEW_RECORD_HANDLER(ImagesettingAttrRecordHandler);
#endif
		NEW_RECORD_HANDLER(PrintingRecordHandler);

// Remove from webster builds at present as half complete Neville 6/8/97
// Taken out by vector stroking code Neville 2/10/97
#ifdef VECTOR_STROKING
		NEW_RECORD_HANDLER(StrokeAttrRecordHandler);
		NEW_RECORD_HANDLER(ShadowRecordHandler);		// Shadow-related record handler
		NEW_RECORD_HANDLER(BevelRecordHandler);
#endif

		NEW_RECORD_HANDLER(ContourRecordHandler);
		NEW_RECORD_HANDLER(TemplateAttrRecordHandler);
PORTNOTE("other","Commented WizOpStyleRecordHandler")
#ifndef EXCLUDE_FROM_XARALX
		NEW_RECORD_HANDLER(WizOpStyleRecordHandler);
#endif
		NEW_RECORD_HANDLER(WizOpStyleRefRecordHandler);
		NEW_RECORD_HANDLER(ExportHintRecordHandler);

		NEW_RECORD_HANDLER(BrushAttrRecordHandler);
		NEW_RECORD_HANDLER(ClipViewRecordHandler);		// ClipView-related record handler

		NEW_RECORD_HANDLER(FeatherRecordHandler);

		// bitmap properties record handler
		NEW_RECORD_HANDLER(BitmapPropertiesRecordHandler);	// Define bmp props record handler
		NEW_RECORD_HANDLER(BitmapSmoothingRecordHandler);	// Bitmap smoothing document flag

		NEW_RECORD_HANDLER(LiveEffectRecordHandler);

		RecordHandlersCreated = TRUE;
	}

	return RecordHandlersCreated;
}

/********************************************************************************************

>	static void BaseCamelotFilter::DestroyRecordHandlers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Destroys all the record handlers associated with the camelot v2 file format filters.

********************************************************************************************/

void BaseCamelotFilter::DestroyRecordHandlers()
{
	RecordHandlerList.DeleteAll();
	RecordHandlersCreated = FALSE;
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::InitRecordHandlers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if not.
	Purpose:	Calls all the record handlers via their BeginImport() function.
				It also registers the list of handlers with the attached CXaraFile object

********************************************************************************************/

BOOL BaseCamelotFilter::InitRecordHandlers()
{
	ERROR2IF(pCXaraFile == NULL, FALSE,"Can't initialise record handlers without a CXaraFile object");

	// Register the list handlers with thw CXaraFile objects
	BOOL ok = pCXaraFile->RegisterRecordHandlers(&RecordHandlerList);

	// Initialise all the handlers
	CamelotRecordHandler* pCamelotRecordHandler = (CamelotRecordHandler*)RecordHandlerList.GetHead();
	while (ok && pCamelotRecordHandler != NULL)
	{
				ok = pCamelotRecordHandler->Init(this);
		if (ok) ok = pCamelotRecordHandler->BeginImport();

		pCamelotRecordHandler = (CamelotRecordHandler*)RecordHandlerList.GetNext(pCamelotRecordHandler);
	}

	if (pCXaraFile != NULL)
		ok = pCXaraFile->SetUpHandlers(this);

	return ok;
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::DeinitRecordHandlers()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if not.
	Purpose:	Calls all the record handlers via their EndImport() function.
				It also registers the list of handlers with the attached CXaraFile object

********************************************************************************************/

BOOL BaseCamelotFilter::DeinitRecordHandlers()
{
	ERROR2IF(pCXaraFile == NULL, FALSE,"Can't initialise record handlers without a CXaraFile object");

	// Register the list handlers with thw CXaraFile objects
	BOOL ok = pCXaraFile->RegisterRecordHandlers(&RecordHandlerList);

	// Initialise all the handlers
	CamelotRecordHandler* pCamelotRecordHandler = (CamelotRecordHandler*)RecordHandlerList.GetHead();
	while (ok && pCamelotRecordHandler != NULL)
	{
		ok = pCamelotRecordHandler->EndImport();

		pCamelotRecordHandler = (CamelotRecordHandler*)RecordHandlerList.GetNext(pCamelotRecordHandler);
	}

	return ok;
}

/********************************************************************************************

>	virtual CXaraFile* BaseCamelotFilter::CreateCXaraFile()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		-
	Returns:	ptr to a CXaraFile object that can be used for import or export
				NULL if there's an error
	Purpose:	This is a central place where the CXaraFile object is created.
				It is a virtual func to allow derived classes to create different flavours
				of CXaraFile

********************************************************************************************/

CXaraFile* BaseCamelotFilter::CreateCXaraFile()
{
	CXaraFile* pCXaraFile = new CXaraFile;

	if (pCXaraFile != NULL)
		pCXaraFile->SetFilter(this);

	return pCXaraFile;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		The new compression state required, True for on, False for off
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression on or off on the underlying CCFile.
	Errors:		-
	SeeAlso:	CXaraFile::SetCompression()

********************************************************************************************/

BOOL BaseCamelotFilter::SetCompression(BOOL NewState)
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"BaseCamelotFilter::SetCompression pCXaraFile is NULL");
	
	return pCXaraFile->SetCompression(NewState);
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::StartCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression on when we are writing to the file. It asks the
				underlying CCFile and CXaraFile to start the compression process up.
	Errors:		-
	SeeAlso:	StopCompression(); CXaraFile::StartCompression();

********************************************************************************************/

BOOL BaseCamelotFilter::StartCompression()
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"BaseCamelotFilter::SetCompression pCXaraFile is NULL");
	
	return pCXaraFile->StartCompression();
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::StopCompression()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		-
	Returns:	TRUE if ok
				FALSE otherwise
	Purpose:	Function to turn Compression off when we are writing to the file. It asks the
				underlying CCFile and CXaraFile to stop the compression process.
	Errors:		-
	SeeAlso:	StartCompression(); CXaraFile::StopCompression()

********************************************************************************************/

BOOL BaseCamelotFilter::StopCompression()
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"BaseCamelotFilter::StopCompression pCXaraFile is NULL");
	
	return pCXaraFile->StopCompression();
}

/********************************************************************************************

>	virtual CCLexFile* BaseCamelotFilter::GetCCFile()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		-
	Returns:	The CCFile that is being used to export the data.
	Purpose:	Function to give public access to the underlying CCFile that is being used
				to save out the data.
				Note: only people who have a genuine reason need to access this - e.g. bitmap savers
	Errors:		-
	SeeAlso:	BitmapListComponent::SaveBitmapDefinition

********************************************************************************************/

CCLexFile* BaseCamelotFilter::GetCCFile()
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"BaseCamelotFilter::GetCCFile pCXaraFile is NULL");
	return pCXaraFile->GetCCFile();
	
}

/********************************************************************************************

>	virtual UINT32 BaseCamelotFilter::GetCurrentRecordSize()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/96
	Inputs:		-
	Returns:	The size of the current record.
	Purpose:	Function to get the current size of the record that has been read in by the 
				CXaraFile and RecordHandlers and are currently processing. This can be used
				by streamed record handlers, such as bitmaps, which need to know the size of
				the recordso that they can update the progress bar size by a proportion of
				this amount on loading. (Not require on saving as the streamed record handlers
				can supply what value they like to the progress system and hence update with
				whatever value they require.

********************************************************************************************/

UINT32 BaseCamelotFilter::GetCurrentRecordSize()
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"BaseCamelotFilter::GetCurrentRecordSize pCXaraFile is NULL");
	return pCXaraFile->GetCurrentRecordSize();
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::StartProgressBar(String_64* pMessage)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		pMessage = ptr to message string (or NULL if you want default message)
	Returns:	TRUE if OK, FALSE if not
	Purpose:	Starts up the progress bar.  If it fails it returns FALSE, but it's no problem
				if you continue - i.e. you shouldn't about import/export just because you can't
				get a progress bar going.

				If a progress bar already exists, it's deleted and a new one created

********************************************************************************************/

BOOL BaseCamelotFilter::StartProgressBar(String_64* pMessage)
{
	TRACE( _T("Starting progress bar\n") );

	// End the current one (if there is one of course).
	EndProgressBar();

	// ptr to the progress bar object
	pProgress = new Progress(pMessage,100);
	if (pProgress != NULL)
	{
		pProgress->Update();
		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::EndProgressBar()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	-
	Purpose:	If a progress bar already exists, it's deleted.

********************************************************************************************/

void BaseCamelotFilter::EndProgressBar()
{
	// Check to see if we need to delete the old one
	if (pProgress != NULL)
	{
		TRACE( _T("Stopping progress bar\n"));
		delete pProgress;
		pProgress = NULL;
	}
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::PrepareImportExportVars()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Inputs:		-
	Returns:	TRUE if OK, FALSE if not
	Purpose:	Prepares all the vars that are common to both import & export.
				This should be the first thing that's called when import/export starts

********************************************************************************************/

BOOL BaseCamelotFilter::PrepareImportExportVars()
{
	// This group of vars are only used on import, but they are reset here just in case some
	// idiot tries to use them on export
	pLastDocument		= NULL;
	pLastChapter		= NULL;
	pLastSpread			= NULL;
	pLastLayer			= NULL;
	pLastNodePath		= NULL;
	pLastCurrentAttr	= NULL;
	DocumentCount		= 0;
	ChapterCount		= 0;
	SpreadCount			= 0;
	LayerCount			= 0;
	SetSentinelCount	= 0;
	pImportLayer		= NULL;

	// Set up the pointers to the document components
	if (!FindDocComponents())
		return FALSE;
	
	ProgressBarCount = 0;			// Current progress bar count

	// Check to see if we need to delete the old one
	if (pCXaraFile != NULL)
	{
		ERROR3("We already have a CXaraFile object");
		delete pCXaraFile;
		pCXaraFile = NULL;
	}

	// Create the correct CXaraFile object for the job
	pCXaraFile = CreateCXaraFile();

	return (pCXaraFile != NULL);
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::ResetImportExportVars()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Inputs:		-
	Returns:	-
	Purpose:	Resets all the vars that are common to both import & export.
				This should be the last thing that's called when import/export has finished

********************************************************************************************/

void BaseCamelotFilter::ResetImportExportVars()
{
	EndProgressBar();

	if (pCXaraFile != NULL)
	{
		// First call close to ensure that the CXaraFile is cleaned up
		pCXaraFile->Close();
		// Now delete it
		delete pCXaraFile;
	}

	pCXaraFile = NULL;

	if (pPathRecordRefList != NULL)
	{
		pPathRecordRefList->DeleteAll();
		delete pPathRecordRefList;
		pPathRecordRefList = NULL;
	}

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
	if (pTextStoryGroupRefList != NULL)
	{
		pTextStoryGroupRefList->DeleteAll();
		delete pTextStoryGroupRefList;
		pTextStoryGroupRefList = NULL;
	}
*/
	// WEBSTER - markn 29/1/97
	// Part of the general form of the system used to convert text to outlines in v1.5
	if (pNodeGroupRefList != NULL)
	{
		pNodeGroupRefList->DeleteAll();
		delete pNodeGroupRefList;
		pNodeGroupRefList = NULL;
	}
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetLastRecordHandler(CXaraFileRecordHandler* pHandler,UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/96
	Inputs:		pHandler = ptr to record handler used to handle the last record
				Tag		 = the tag the handler handled
	Returns:	-
	Purpose:	Sets the handler used to handle the last record that was imported.  It also sets the
				tag value of the last handled record.

				NOTE:  If Tag is either TAG_UP or TAG_DOWN, the call does nothing

********************************************************************************************/

BOOL BaseCamelotFilter::SetLastRecordHandler(CXaraFileRecordHandler* pHandler,UINT32 Tag)
{
	ERROR2IF(pHandler == NULL,FALSE,"NULL handler ptr");
	ERROR2IF(Tag == TAG_UNDEFINED,FALSE,"TAG_UNDEFINED tag value");
	ERROR2IF(pInsertLevelStack == NULL,FALSE,"NULL pInsertLevelStack ptr");

	if (Tag != TAG_UP && Tag != TAG_DOWN)
	{
		// Find the info item containing data on the current level
		InsertLevelStackItem* pItem = pInsertLevelStack->Get();

		// Set the last handler & tag value for this level
		if (pItem != NULL)
			pItem->SetLastHandler(pHandler,Tag);
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::PrepareToImport(CCLexFile* pFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/96
	Inputs:		pFile = ptr to file to read from
	Returns:	TRUE if filter initialised ok;
				FALSE if not.
	Purpose:	To get this filter ready to import a file.

********************************************************************************************/

BOOL BaseCamelotFilter::PrepareToImport(CCLexFile* pFile)
{
	// Set up the CurrentAttribute pointer.
	if ( !SetUpCurrentAttrs () )
		return FALSE;
	
	ProgressBarMessage = GetImportProgressString(pFile, GetImportMsgID());

	// Total progress bar count (only do this on import).
	TotalProgressBarCount = 0;

	// Clear the ignored data warning string
	IgnoredDataString.Empty();
	// Clear the warnings messages strings
	WarningsString.Empty();

	// Clear import file type string
	ImportFileType[0] = 0;

	// Clear general import/export vars
	BOOL ok = PrepareImportExportVars();
	StartProgressBar(&ProgressBarMessage);

	// Reset the first layer to be imported
	m_pFirstImportedLayer		= NULL;
	m_pActiveLayerBeforeImport	= NULL;

	if (TheDocument != NULL)
	{
		Spread* pSpread = TheDocument->GetSelectedSpread();
		if (pSpread != NULL)
		{
			// This must be before PrepareTreeBuilding as if this is importing
			// PrepareTreeBuilding will insert a new layer which may be deleted as
			// part of the import process.
			m_pActiveLayerBeforeImport = pSpread->FindActiveLayer();
		}
	}

	// Prepare the tree building aspects of importing
	if (ok) ok = PrepareTreeBuilding();

	// Reset import vars
	EndOfFileFlag = FALSE;
	EscapePressed = FALSE;
	PreCompFlags  = 0;

	if (ok) ok = StartImportDocComponents();
	if (ok) ok = InitRecordHandlers();
	if (ok)
	{
		// If we fail at this point, the file must be corrupted (i.e. dodgy 8 byte header)
		ok = pCXaraFile->OpenToRead(pFile);
		if (!ok)
		{
			Error::SetError(BadFileMsgID);
			return FALSE;
		}
	}

	if (TheDocument != NULL)
	{
		Spread* pSpread = TheDocument->GetSelectedSpread();
		if (pSpread != NULL)
		{
			DocRect PagesRect;
			pSpread->GetPagesRect(&PagesRect);
			CoordOrigin = PagesRect.lo;
		}
	}

	return ok;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Mark Neves
	Created:	29/5/96
	Inputs:		Successful - TRUE if the import went OK, FALSE if it failed
	Purpose:	Happens at the end of the import to allow the filters to clean up after
				themselves

********************************************************************************************/

void BaseCamelotFilter::CleanUpAfterImport(BOOL Successful)
{
	PORTNOTETRACE("other","BaseCamelotFilter::CleanUpAfterImport - ignoring RalphCriticalSection");
#ifndef EXCLUDE_FROM_XARALX
	RalphCriticalSection rcs;
#endif

	EndImportDocComponents(Successful);

	DeinitRecordHandlers();

	Camelot.UpdateSelection();

	if (Successful && TheDocument != NULL)
	{
		if (pImportLayer != NULL)
		{
			Node* pChild = pImportLayer->FindFirstChild();
			if (pChild == NULL)
			{
				DeleteNode(pImportLayer);
				pImportLayer = NULL;
			}
		}

		Spread* pSpread = TheDocument->GetSelectedSpread();

		ERROR3IF(pSpread == NULL,"We ought to have a selected spread at this point");

		if (pSpread != NULL)
		{
			pSpread->ValidateBoundingRect();
			pSpread->InvalidateBoundingRect(TRUE);

			// Make sure we have an active layer, and let everyone know that layer information has changed
#ifndef WEBSTER
			// In Camelot mode, we must now check to see what document we are importing into
			// If it has frame layers in then we must convert all new layers into frames
			// If it has no frame layers in, then we must convert all frames into layers
			EnsureLayerIntegrity(pSpread);
			//LayerSGallery::EnsureActiveLayerIntegrity(pSpread);
#else
			// In Webster ensure that multiple layers are disallowed unless they are
			// frame layers. At the same time ensure that there is an active layer.
			EnsureFrameLayerIntegrity(pSpread);
#endif

			MergeSameNamedLayers();
			BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::LAYERCHANGES));

			// make sure the spread's pasteboard is extended to include all imported objects
			Node* pNode = pSpread;
			pNode->PostImport();
		}

		if (TheDocument->IsNotAClipboard())
			TheDocument->ResetInsertionPosition();

		// Warn user if font substitution has occurred.
//		TheDocument->CheckFontSubstitution();
		TheDocument->CheckReferences();

#if !defined(EXCLUDE_FROM_RALPH)
		// Make sure all qaulity sliders in the selected doc view get updated
PORTNOTE("other","Removed QualitySliderDescriptor usage")
#ifndef EXCLUDE_FROM_XARALX
		QualitySliderDescriptor::Update();
#endif
#endif
	}
	
	CleanUpTreeBuilding();

	ResetImportExportVars();

	// Any warnings to report?
	// We will do these as two separate warnings as:-
	// - they should both be rare in themselves
	// - they are talking about two completely different items
	// - they are difficult to add together without taking up a very large warning box
	// - they are difficult to add together in a compact and internationalisable way
	// Check the unknown but not essential tags found warning first
	if (Successful && !IgnoredDataString.IsEmpty())
	{
#if !defined(EXCLUDE_FROM_RALPH)
		Error::SetError(0,IgnoredDataString,0);
		InformWarning();		
#else
		// "Non-essential data has been found that is not recognised."
		String_256 Err(_R(IDE_IMPORT_RALPH_NEWTAGS));
		Error::SetError(0,Err,0);
		InformWarning();
#endif
	}
	// Now check the warning about problem data string
	if (Successful && !WarningsString.IsEmpty())
	{
#if !defined(EXCLUDE_FROM_RALPH)
		String_256 Warning = WarningsString;
		// Add the text on the end which mentions about using defaults instead
		String_256 Temp(_R(IDS_NATIVE_REPLACEDEFAULTS));
		Warning += Temp;
		Error::SetError(0,Warning,0);
		InformWarning();		
#else
		// "Problems have been found with some data. The problems have been fixed with default values."
		String_256 Err(_R(IDE_IMPORT_RALPH_WARNING));
		Error::SetError(0,Err,0);
		InformWarning();
#endif
	}

	// Make sure the lists are NULL
	if (pAtomicTagList != NULL)
	{
		pAtomicTagList->DeleteAll();
		delete pAtomicTagList;
		pAtomicTagList = NULL;
	}

	if (pEssentialTagList != NULL)
	{
		pEssentialTagList->DeleteAll();
		delete pEssentialTagList;
		pEssentialTagList = NULL;
	}

	if (pTagDescriptionList != NULL)
	{
		pTagDescriptionList->DeleteAll();
		delete pTagDescriptionList;
		pTagDescriptionList= NULL;
	}

	// Clean up the current attribute pointer and array.
	DeleteCurrentAttrs ();
}


/********************************************************************************************

>	BOOL BaseCamelotFilter::MergeSameNamedLayers()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> JK
	Created:	29/10/99
	Returns:	Successful - TRUE, FALSE implies the spread or op had problems
	Purpose:	We have imported layers of the same name in the document. Merge these layers
				together and if these layers are the special rollover layers make these
				button and bar names unique.
	See Also:	SliceHelper::MeshImportedLayersWithExistingButtonBars

********************************************************************************************/
BOOL BaseCamelotFilter::MergeSameNamedLayers()
{
	// scan for identically named layers
	// background layers are first, foreground layers later
	// find a spread?
	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL || !ImportInfo.pOp)
		return FALSE;

	// scan for the layer to delete
	Layer * pLayer = pSpread->FindFirstLayer();
	Layer * pTestLayer = NULL;
	String_256	LayerName = "";

	// count how many layers are being imported
	INT32 NoOfImportedLayers = 0;
	pTestLayer = m_pFirstImportedLayer;
	while (pTestLayer)
	{
		pTestLayer = pTestLayer->FindNextLayer();
		NoOfImportedLayers++;
	}
	pTestLayer = NULL;

	Node * pFirstNodeOfImportedLayer[5];
	String_256	StateLayerNames[5];

	StateLayerNames[0].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	StateLayerNames[1].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	StateLayerNames[2].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	StateLayerNames[3].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";
	StateLayerNames[4].Load(_R(IDS_BACK_BAR)); // = "BackBar";

	INT32 i;
	for (i = 0; i < 5; i++)
		pFirstNodeOfImportedLayer[i] = NULL;

	// double loop around the layer names looking for duplicate names
	while (pLayer)
	{
		pTestLayer = pLayer->FindNextLayer();
		LayerName = pLayer->GetLayerID();

		while (pTestLayer)
		{
			if (!pTestLayer->IsNodeHidden() && !pLayer->IsNodeHidden() &&
				pTestLayer->GetLayerID().CompareTo(LayerName) == 0)
			{
				// found two identical layer names
				// squeeze the later onto the end of the former
				Node * pNode = pTestLayer->FindFirstChild();
				Node * pTemp = NULL;
				Layer * pNextLayer = pTestLayer->FindNextLayer();
				BOOL IsButtonStateLayer = FALSE;

				// note the layer we have found
				for (i = 0; i < 5; i++)
					if (!pFirstNodeOfImportedLayer[i] && LayerName.CompareTo(StateLayerNames[i]) == 0)
					{
						pFirstNodeOfImportedLayer[i] = pNode;
						IsButtonStateLayer = TRUE;
					}

				if (IsButtonStateLayer || (NoOfImportedLayers < 2 && !pTestLayer->IsBackground()) ||
					(pTestLayer->IsGuide() && pLayer->IsGuide()))
				{ // mesh these two similar layers together
					// always mesh guide layers together but leave other background layers apart
					// loop for all nodes in this layer
					while (pNode)
					{
						pTemp = pNode->FindNext();
						if (!pNode->IsAnAttribute())
							ImportInfo.pOp->DoMoveNode(pNode, pLayer, LASTCHILD);
						pNode = pTemp;
					}

					ImportInfo.pOp->DoHideNode(pTestLayer, FALSE, NULL, FALSE);
				}
				else
				{ // rename this layer to a unique name
					pTestLayer->EnsureUniqueLayerID();
					pTestLayer->SetActive(FALSE);
				}

				// find the next layer now
				pTestLayer = pNextLayer;
			}
			else
				pTestLayer = pTestLayer->FindNextLayer();
		}

		pLayer = pLayer->FindNextLayer();
	}

	// Add the first item on any new layers of the specail bar type as these need to be synced to
	// so take the first imported layer, is this a bar state layer? If so add it to the list
	if (IsImporting())
	{
		pTestLayer = m_pFirstImportedLayer; 
		while (pTestLayer)
		{
			for (i = 0; i < 5; i++)
				if (!pFirstNodeOfImportedLayer[i] && StateLayerNames[i].CompareTo(pTestLayer->GetLayerID()) == 0)
				{
					TRACEUSER( "SimonK", _T("Imported State Layer :%s\n"), (TCHAR *)pTestLayer->GetLayerID() );
					pFirstNodeOfImportedLayer[i] = SliceHelper::FindLayerCalled(StateLayerNames[i]);
					if (pFirstNodeOfImportedLayer[i])
						pFirstNodeOfImportedLayer[i] = pFirstNodeOfImportedLayer[i]->FindFirstChild();
				}

			pTestLayer = pTestLayer->FindNextLayer();
		}
	}

	SliceHelper::MeshImportedLayersWithExistingButtonBars(pFirstNodeOfImportedLayer, ImportInfo.pOp, TRUE);

	// order layers so that the order of the button state layers always occurs in the same order
	pLayer = pSpread->FindFirstLayer();

	Layer * pFoundLayer[5];

	for (i = 0; i < 5; i++) 
		pFoundLayer[i] = NULL;

	while (pLayer)
	{
		LayerName = pLayer->GetLayerID();

		for (i = 0; i < 5; i++)
			if (LayerName.CompareTo(StateLayerNames[i]) == 0)
			{
				pFoundLayer[i] = pLayer;

				// is this layer out of order?

				for (INT32 j = i-1; j >= 0; j--)
				{
					if (pFoundLayer[j])
					{
						// i and j wrong way round
						pFoundLayer[i]->MoveNode(pFoundLayer[j], PREV);
						pLayer = pFoundLayer[j];
						break;
					}
				}

			break;
			}
		
		pLayer = pLayer->FindNextLayer();
	}

	// background layers should be before any normal layers
	// this can only happen through importing them in therefore
	// it should not be a problem moving these new nodes around
	// as undo will just remove them (sjk 2/7/00)
	Layer * pNormalLayer = NULL;

	pLayer = pSpread->FindFirstLayer();
	while (pLayer)
	{
		if (!pLayer->IsBackground())
		{
			if (!pNormalLayer)
				pNormalLayer = pLayer;
		}
		else
		{
			// the normal layer got in first
			// move this background layer to before the first normal layer
			if (pNormalLayer)
			{
				pLayer->MoveNode(pNormalLayer, PREV);
				pLayer = pNormalLayer; // restart iteration from here
			}
		}

		pLayer = pLayer->FindNextLayer();
	}

	return TRUE;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::EnsureLayerIntegrity(Spread* pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/97
	Inputs:		pSpread  - the spread to check the layers on.
	Purpose:	Happens at the end of the import to allow the layers to be cleaned up.
				Mainly used by Camelot so that we can see whether we are using a framed
				document or a layered document. We must check that:-
				- If the document has frame layers in then we must convert all new layers into frames
				- If the document has no frame layers in, then we must convert all frames into layers
				
				Do now as the layers are stable and have all their flags present. We cannot
				decide when we get the layer record as we would need to wait until we have
				the layer details record. Then this would not be the complete information
				until we have seen or not seen as the case may be the frame properties 
				record.

********************************************************************************************/

BOOL BaseCamelotFilter::EnsureLayerIntegrity(Spread* pSpread)
{
	// Search existing layers, if frame layer in there then must convert all new layers into
	// frames. Could use existing code below
	// If frame layer not there then just make sure that all new layers are not frames
	if (IsImporting())
	{
		// Check if we are in a hidden document. This may be the print marks document.
		// In this case we should do nothing.
		// Unfortunately the hidden flag is not set in the print marks case which is the one
		// we are trying to stop. Instead, print marks have been disabled in Webster builds.
		if (TheDocument != NULL && !TheDocument->IsAHiddenDoc())
		{
			// Ensure that all other non-guide and non-background layers are frame layers
			UINT32 OriginalNonFrameLayersFound = 0;
			UINT32 OriginalFrameLayersFound = 0;
			Layer* pCurrentLayer = NULL;
			Layer * pActiveLayer = NULL;
			// Search from the first imported layer, so that we only ever combine newly imported
			// layers together. Only do when importing.
			// Also, find the first active layer in the spread which should be our original active
			// layer.
			if (m_pFirstImportedLayer == NULL && !IsImporting())
				pCurrentLayer = pSpread->FindFirstLayer();
			else
			{
				// Account for minimal web files which do not have a layer and so import onto the
				// layer that is constructed for that purpose i.e. pImportLayer
				Layer * pActualFirstImportedLayer = m_pFirstImportedLayer;
				if (m_pFirstImportedLayer == NULL && pImportLayer != NULL)
					pActualFirstImportedLayer = pImportLayer;
				ERROR3IF(pActualFirstImportedLayer == NULL,"BaseCamelotFilter::EnsureLayerIntegrity No first imported layer!");

				pCurrentLayer = pActualFirstImportedLayer;
				// Check that the other layers in the document before this are all frame layers
				// At present, just mark them as frames as there should always be just one possible
				// non-frame layer present. (From the default doc which is not in the new format)
				Layer* pLayer = pSpread->FindFirstLayer();
				BOOL Finished = FALSE;
				while (pLayer != NULL && !Finished)
				{
					// Only search up to the first imported layer
					if (pLayer == pActualFirstImportedLayer)
					{
						Finished = TRUE;
					}
					else if (pLayer->IsPseudoFrame())
					{
						if (pLayer->IsFrame())
						{
							OriginalFrameLayersFound++;
						}
						else
						{
							OriginalNonFrameLayersFound++;
						}
					}

					// Note the original active layer, if found 
					if (!Finished && pLayer == m_pActiveLayerBeforeImport) // pLayer->IsPseudoFrame() && pLayer->IsActive())
					{
						pActiveLayer = pLayer;
					}

					pLayer = pLayer->FindNextLayer();
				}

				if (OriginalNonFrameLayersFound > 0 || OriginalFrameLayersFound > 0)
				{
					TRACEUSER( "Neville", _T("++++++++++BaseCamelotFilter::EnsureLayerIntegrity %d original frames and %d layers\n"), OriginalFrameLayersFound,OriginalNonFrameLayersFound);
					
				}
			}

			// Search through all the new layers to see potential frame ones are frames and which
			// are layers.
			UINT32 NonFrameLayersFound = 0;
			UINT32 FrameLayersFound = 0;
			Layer* pNonFrameLayer = NULL; 
			const String_32 CustomMarkName(TEXT("_%CUSTOMMA")); // REM 10 long only
			const String_32 PrintMarkName(TEXT("%PRINTMARK"));	// REM 10 long only
			while (pCurrentLayer != NULL)
			{
				// Check if we are one of the uniquely named layers in the print marks document
				String_256 LayerName = pCurrentLayer->GetLayerID();
				String_32 StartOfName;
				LayerName.Left(&StartOfName, 10);  

				// The layer has to pass a lot of tests to be counted!
				if (pCurrentLayer->IsPseudoFrame() &&
					StartOfName != CustomMarkName && StartOfName != PrintMarkName)
				{
					if (pCurrentLayer->IsFrame())
					{
						FrameLayersFound++;
					}
					else
					{
						// Note that we have found a non-frame layer.
						if (pNonFrameLayer == NULL)
							pNonFrameLayer = pCurrentLayer;
						NonFrameLayersFound++;
					}
				}

				pCurrentLayer = pCurrentLayer->FindNextLayer();
			}

			if (NonFrameLayersFound > 0 || FrameLayersFound > 0)
			{
				TRACEUSER("Neville", _T("++++++++++BaseCamelotFilter::EnsureLayerIntegrity %d new frames and %d layers\n"),FrameLayersFound,NonFrameLayersFound);
				
			}

			// Decision time, now that we have decided what layers/frames we had before the import
			// and what layers/frames have been loaded
			if (OriginalFrameLayersFound > 0)
			{
				// We had frames so ensure that we do not have layers in the new document
				// Use the Webster code to strip all extraneous layers into one new frame
				BaseCamelotFilter::EnsureFrameLayerIntegrity(pSpread);

				// This will ensure that there is an active frame layer so we needn't
			}
			else
			{
				// We had layers so ensure that there are no frames in the new document
				Layer* pLayer = pSpread->FindFirstLayer();
				BOOL Finished = FALSE;
				while (pLayer != NULL && !Finished)
				{
					// Ensure ALL layers are not frames
					pLayer->SetFrame(FALSE);
					// Get next one
					pLayer = pLayer->FindNextLayer();
				}				

PORTNOTE("other","Removed LayerSGallery usage")
				// Not importing so just ensure that there is one active layer in the document.
#ifndef EXCLUDE_FROM_XARALX
				LayerSGallery::EnsureActiveLayerIntegrity(pSpread);
#endif
			}
		}		
	}
	else
	{
PORTNOTE("other","Removed LayerSGallery usage")
		// Not importing so just ensure that there is one active layer in the document.
#ifndef EXCLUDE_FROM_XARALX
		LayerSGallery::EnsureActiveLayerIntegrity(pSpread);
#endif
	}

	return TRUE;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::EnsureFrameLayerIntegrity(Spread* pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/97
	Inputs:		pSpread  - the spread to check the layers on.
	Purpose:	Happens at the end of the import to allow the layers to be cleaned up.
				Mainly used by Webster so that we can ensure that frame layers are the order
				of the day.
				Do now as the layers are stable and have all their flags present. We cannot
				decide when we get the layer record as we would need to wait until we have
				the layer details record. Then this would not be the complete information
				until we have seen or not seen as the case may be the frame properties 
				record.

********************************************************************************************/

BOOL BaseCamelotFilter::EnsureFrameLayerIntegrity(Spread* pSpread)
{
	// In Webster, check that we have only frame layers or if not combine all non-frame
	// layers onto one frame layer.
	// In Camelot do not do this
	// In both ensure that we have an active layer
//#ifdef WEBSTER
	// Check if we are in a hidden document. This may be the print marks document.
	// In this case we should do nothing.
	// Unfortunately the hidden flag is not set in the print marks case which is the one
	// we are trying to stop. Instead, print marks have been disabled in Webster builds.
	if (TheDocument != NULL && !TheDocument->IsAHiddenDoc())
	{
		// Ensure that all other non-guide and non-background layers are frame layers
		UINT32 NonFrameLayersFound = 0;
		Layer* pCurrentLayer = NULL;
		Layer * pActiveLayer = NULL;
		// Search from the first imported layer, so that we only ever combine newly imported
		// layers together. Only do when importing.
		// Also, find the first active layer in the spread which should be our original active
		// layer.
		if (m_pFirstImportedLayer == NULL && !IsImporting())
			pCurrentLayer = pSpread->FindFirstLayer();
		else
		{
			// Account for minimal web files which do not have a layer and so import onto the
			// layer that is constructed for that purpose i.e. pImportLayer
			Layer * pActualFirstImportedLayer = m_pFirstImportedLayer;
			if (m_pFirstImportedLayer == NULL && pImportLayer != NULL)
				pActualFirstImportedLayer = pImportLayer;
			ERROR3IF(pActualFirstImportedLayer == NULL,"BaseCamelotFilter::EnsureFrameLayerIntegrity No first imported layer!");

			pCurrentLayer = pActualFirstImportedLayer;
			// Check that the other layers in the document before this are all frame layers
			// At present, just mark them as frames as there should always be just one possible
			// non-frame layer present. (From the default doc which is not in the new format)
			Layer* pLayer = pSpread->FindFirstLayer();
			BOOL Finished = FALSE;
			while (pLayer != NULL && !Finished)
			{
				// Only search up to the first imported layer
				if (pLayer == pActualFirstImportedLayer)
				{
					Finished = TRUE;
				}
				else if (pLayer->IsPseudoFrame() && !pLayer->IsFrame())
				{
					pLayer->SetFrame(TRUE);
					NonFrameLayersFound++;
				}

				// Note the original active layer, if found 
				if (!Finished && pLayer == m_pActiveLayerBeforeImport) // pLayer->IsPseudoFrame() && pLayer->IsActive())
				{
					pActiveLayer = pLayer;
				}

				pLayer = pLayer->FindNextLayer();
			}

			if (NonFrameLayersFound > 0)
			{
				TRACEUSER( "Neville", _T("++++++++++BaseCamelotFilter::EnsureFrameLayerIntegrity fixed %d original non-frame layers\n"),NonFrameLayersFound);
				
			}
		}

		NonFrameLayersFound = 0;
		Layer* pNonFrameLayer = NULL; 
		const String_32 CustomMarkName(TEXT("_%CUSTOMMA")); // REM 10 long only
		const String_32 PrintMarkName(TEXT("%PRINTMARK"));	// REM 10 long only
		while (pCurrentLayer != NULL)
		{
			// Check if we are one of the uniquely named layers in the print marks document
			String_256 LayerName = pCurrentLayer->GetLayerID();
			String_32 StartOfName;
			LayerName.Left(&StartOfName, 10);  

			// The layer has to pass a lot of tests to be counted!
			// Added a visible test so that hidden layers are not included
			// If it was a frame layer then it could be invisible but as we don't allow frame layers
			// then this ok.
			if (pCurrentLayer->IsPseudoFrame() && !pCurrentLayer->IsFrame() && pCurrentLayer->IsVisible() &&
				StartOfName != CustomMarkName && StartOfName != PrintMarkName)
			{
				// Note that we have found a non-frame layer.
				if (pNonFrameLayer == NULL)
					pNonFrameLayer = pCurrentLayer;
				NonFrameLayersFound++;
			}

			pCurrentLayer = pCurrentLayer->FindNextLayer();
		}

		/* if (NonFrameLayersFound == 1 && pNonFrameLayer != NULL)
		{
			TRACEUSER( "Neville", _T("++++++++++BaseCamelotFilter::EnsureFrameLayerIntegrity found %d non-frame layers\n"),NonFrameLayersFound);
			// Just mark the non-frame layer found in the above search as a frame layer.
			// We need do nothing else but fall through and fix up the active layer
			pNonFrameLayer->SetFrame(TRUE);
		}
		else */
		if (NonFrameLayersFound >= 1 && pNonFrameLayer != NULL)
		{
			TRACEUSER("Neville", _T("++++++++++BaseCamelotFilter::EnsureFrameLayerIntegrity found %d non-frame layers\n"),NonFrameLayersFound);
			// Right, we need to move all the nodes from the all non-frame layers and move them
			// to one of the non-frame layers and make this one a frame layer. The user should
			// therefore notice no difference in appearance of the document as they would have
			// been seeing one frame representing all the non-frame layers in the frame gallery.
			
			// If we are importing then use the active layer as the destination for all non-frame layers.
			BOOL UsingExistingLayer = FALSE;
			if (IsImporting() && ImportNonFramesOntoActivelayer)
			{
				// If we found an active layer then use this for importing all the non-frames onto
				// if the user has the preference set
				// if (pActiveLayer) This is not always perfect e.g. first import of a web file
				// so use the version we save in PrepareToImport and have checked is still present above
				// i.e. m_pActiveLayerBeforeImport
				if (pActiveLayer)
				{
					pNonFrameLayer = pActiveLayer;
					UsingExistingLayer = TRUE;
				}
			}

			if (!UsingExistingLayer)
			{
				// First, mark the non-frame layer found in the above search as a frame layer as this
				// is the one that we are going to use as out destination. Then all other non-frame
				// searches will not find it. Also, mark it as edited.
				pNonFrameLayer->SetFrame(TRUE);
				pNonFrameLayer->SetEdited(TRUE);

				// We will also set the new frames to overlay so that the user sees the old plus the new
				pNonFrameLayer->SetOverlay(TRUE);
			}

			// Start a progress bar
			Progress CombiningProgress(_R(IDS_COMBINING_LAYERS));
			CombiningProgress.Update();

			BOOL ok = TRUE;
			BOOL UpdateSelection = FALSE;
			BOOL MoveWithUndo = (IsImporting() && (ImportInfo.pOp != NULL));
			Layer* pLayer = pSpread->FindFirstLayer();
			while (pLayer != NULL)
			{
				if (pLayer->IsPseudoFrame() && !pLayer->IsFrame() && ok)
				{
					Node * pNode = NULL;
					// Move all the top level nodes to the new layer
					// We MUST include hidden nodes as these will be in undo records
					// Added a visible test so that hidden layers are not included but are just deleted
					// If it was a frame layer then it could be invisible but as we don't allow frame layers
					// then this ok.
					if (pLayer->IsVisible())
						pNode = pLayer->FindFirstChild();

					while (pNode && ok)
					{
						// Note the node to move
						Node* pNodeToMove = pNode;
						// and the next node in the tree
						pNode = pNode->FindNext();
						// Note if we have moved at least one selected node
						UpdateSelection |= pNodeToMove->IsSelected();

						// Find the insertion point, which will be after the last child but
						// BEFORE the insertion node, if present.
						Node * pInsertPoint = pNonFrameLayer->FindLastChild(TRUE);
						// If we have an undoable op then move the node with undo
						if (MoveWithUndo)
						{
							// Importing, so use the undoable op to do the work for us
							if (pInsertPoint)
								ok = ok && ImportInfo.pOp->DoMoveNode(pNodeToMove, pInsertPoint, NEXT);
							else
								ok = ok && ImportInfo.pOp->DoMoveNode(pNodeToMove, pNonFrameLayer, LASTCHILD);
						}
						else
						{
							// Loading, so  do it blatently
							if (pInsertPoint)
								pNodeToMove->MoveNode(pInsertPoint, NEXT);
							else
								pNodeToMove->MoveNode(pNonFrameLayer, LASTCHILD);
						}

						// Update the progress display to show we have done something
						CombiningProgress.Update();
					}
					
					// Finally, go and hide that layer
					Layer * pLayerToHide = pLayer;
					// First, invalidate the bounding rect
					pLayerToHide->InvalidateBoundingRect();
					// note the next layer
					pLayer = pLayer->FindNextLayer();
					// and hide the layer itself.
					// This will do it undoably if importing, otherwise blatently
					DeleteNode(pLayerToHide);
					/* // Have to do this ourselves as we are not undoable
					pLayerToHide->CascadeDelete();
					delete pLayerToHide; */
					pLayerToHide = NULL;
				}
				else
					pLayer = pLayer->FindNextLayer();

				// Update the progress display to show we have done something
				CombiningProgress.Update();
			}

			// Make sure that our new frame layer is up to date
			pNonFrameLayer->InvalidateBoundingRect();

			//pSpread->ValidateBoundingRect();
			//pSpread->InvalidateBoundingRect(TRUE);

			// If we have changed a selected node then update the selection
			if (UpdateSelection)
			{
				// Update the selection range 
				Camelot.UpdateSelection();
			}

			// Update the progress display to show we have done something
			CombiningProgress.Update();

			// Finally, select the new frame layer as active and fix the other layers
			// and their visibility accordingly
			// Use an undoable op, if available
//			UndoableOperation * pUndoOp = NULL;
			/* if (IsImporting() && (ImportInfo.pOp != NULL))
				pUndoOp = ImportInfo.pOp; */
PORTNOTE("other","Removed FrameSGallery usage")
#ifndef EXCLUDE_FROM_XARALX
			FrameSGallery::MakeActiveLayer(pNonFrameLayer, pUndoOp);
#endif

			// We do this afterwards in the caller
			/*	// We need to update the display as one or more layer items have changed status
			// So tell ourselves about the change. (We are static and so cannot do it directly!)
			BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::SpreadReason::LAYERCHANGES)); */

			// We don't want to repeat the code below so return now as everything
			// should be set up ok
			return TRUE;
		}
	}
//#endif // WEBSTER

	// In Webster, always pick the top layer to be the active layer.
	// Used to do this in EnsureActiveLayerIntegrity but now that we have frames/layers
	// in Webster we cannot be this blatent. So do what we want here.
	// Only pick the last layer to be active if there is no frame layer in the doc. If there
	// is a frame layer present then use the active layer saved in the document.
	// Use an undoable op, if available
//	UndoableOperation * pUndoOp = NULL;
	/* if (IsImporting() && (ImportInfo.pOp != NULL))
		pUndoOp = ImportInfo.pOp; */
	// Find out if there is a frame layer or not
	Layer * pFrameLayer = pSpread->FindLastFrameLayer();
	if (pFrameLayer != NULL)
	{
		// Frame layer present so just ensure that the active layer is selected
		// and there is only one of them.
		// If we have just imported something then there will be an active layer in this but it
		// will be after the active layer that was present BEFORE the import. Therefore, the original
		// active layer will be retained and so the user wont see anything.
		// Search from the last frame layer backwards until we find an active layer and then choose
		// this as the new active layer
		Layer * pActiveFrameLayer = NULL;
		while (pFrameLayer != NULL && pActiveFrameLayer == NULL)
		{
			if (pFrameLayer->IsActive())
				pActiveFrameLayer = pFrameLayer;
			
			pFrameLayer = pFrameLayer->FindPrevFrameLayer();
		}
		// Try and switch to this new one
PORTNOTE("other","Removed FrameSGallery usage")
#ifndef EXCLUDE_FROM_XARALX
		if (pActiveFrameLayer != NULL)
			FrameSGallery::MakeActiveLayer(pActiveFrameLayer, pUndoOp);
		else
			LayerSGallery::EnsureActiveLayerIntegrity(pSpread);
#endif
	}
	else
	{
		// No frame layers so make the active layer the last layer in the spread
		// This should never be called now as the non-frame layer fixing code should
		// have been invoked, leave it in for safeties sake.
PORTNOTE("other","Removed LayerSGallery usage")
//		LayerSGallery::MakeTopLayerActive(pSpread);
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::StartImportDocComponents()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95
	Inputs:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Tells all the doc components attached to the document that we are about to Import
				to either the web or native file format
	SeeAlso:	PrepareToImport, EndImportDocComponents

********************************************************************************************/

BOOL BaseCamelotFilter::StartImportDocComponents()
{
	ERROR2IF(TheDocument == NULL,FALSE,"NULL document ptr");

	BOOL ok = TRUE;

	// Inform all the document components that we are about to Import
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (ok && pComponent != NULL && !EscapePressed)
	{
		// Inform this document component that we are about to start a Native/Web Import.
		ok = pComponent->StartImport(this);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::EndImportDocComponents(BOOL success)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95
	Inputs:		success = bool denoting import success
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Tells all the doc components attached to the document that we have finished Importing
				the document
	SeeAlso:	PrepareToImport, StartImportDocComponents

********************************************************************************************/

BOOL BaseCamelotFilter::EndImportDocComponents(BOOL success)
{
	ERROR2IF(TheDocument == NULL,FALSE,"NULL document ptr");

	BOOL ok = TRUE;

	// Inform all the document components that we have finished Importing
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	// Cannot use the EscapeFlag in here as we MUST clean up on failure!
	while (ok && pComponent != NULL)
	{
		// Inform this document component that we have finished a Native/Web Import.
		ok = pComponent->EndImport(this,success);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	pColComponent	= NULL;
	pBmpComponent	= NULL;
	pUnitsComponent	= NULL;
	pInfoComponent	= NULL;
	pViewComponent	= NULL;
	pPrintComponent	= NULL;
	pFontComponent	= NULL;

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::ReadFile()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95
	Inputs:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Reads in the file
	SeeAlso:	DoImport

********************************************************************************************/

BOOL BaseCamelotFilter::ReadFile()
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"pCXaraFile is NULL");

	BOOL ok = TRUE;

	while (!EndOfFileFlag && ok && !EscapePressed)
		ok = pCXaraFile->ReadNextRecord();

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	// Karim 09/02/2001
	// Tidy up the tree after shadow import.
	PostImportShadows();

	return ok;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::ReadFileUntil(INT32 tag)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/2004
	Inputs:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Reads in the file until the specified tag is hit
	SeeAlso:	DoImport

********************************************************************************************/

BOOL BaseCamelotFilter::ReadFileUntil(INT32 tag)
{
	ERROR2IF(pCXaraFile == NULL,FALSE,"pCXaraFile is NULL");

	BOOL ok = TRUE;

	while (!EndOfFileFlag && ok && !EscapePressed && !pCXaraFile->GetLastReadTag()==tag)
		ok = pCXaraFile->ReadNextRecord();

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::DoImport(SelOperation* pOp, CCLexFile* pFile, Document* pDestDoc, 
					  BOOL AutoChosen, ImportPosition* pPos,
					  KernelBitmap** ppImportedBitmap,
					  DocCoord* pPosTranslate, String_256* URL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		Op - pointer to the operation that this input process is associated with.
				pFile - The file that we should load the document from
				DestDoc - The Document object which should hold the data read in from
						  the file.
				AutoChosen - 
				pPos - 
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
	Returns:	TRUE if the input operation worked ok, FALSE if not.
	Purpose:	Read the data from the specified file. 
	Errors:		Fails (returns FALSE) if the document structure is incorrect, or if there
				is a problem with the file.
				Assumes the caller will fail and end the operation if False is returned.

********************************************************************************************/

BOOL BaseCamelotFilter::DoImport(SelOperation* pOp, CCLexFile* pFile, Document* pDestDoc, 
								 BOOL AutoChosen, ImportPosition* pPos,
								 KernelBitmap** ppImportedBitmap,
								 DocCoord* pPosTranslate, String_256* URL)
{
	ERROR2IF(FALSE, pOp == NULL,"BaseCamelotFilter::DoImport null operation!");
	ERROR2IF(FALSE, pFile == NULL,"BaseCamelotFilter::DoImport null file!");
	ERROR2IF(FALSE, pDestDoc == NULL,"BaseCamelotFilter::DoImport null document!");

	// Set up document pointer
	TheDocument = pDestDoc;

	// Remember operation and position so we can use them to add nodes in other functions.
	ImportInfo.pOp = pOp;
	ImportInfo.pPos = pPos;

	if (!PrepareToImport(pFile))
	{
		// Didn't work (no memory)
		CleanUpAfterImport(FALSE);
		return FALSE;
	}

	// Get the right spread...
	Spread *pSpread = NULL;

	if (pPos == NULL)
	{
		// Find the spread on the first page of this document...
		pSpread = GetFirstSpread(pDestDoc);
	}
	else
	{
		// Use the spread provided
		pSpread = pPos->pSpread;
	}

	// Set our class variable to point at this spread
	pTheSpread = pSpread;

	if (!pSpread)
	{
		// Didn't work, no spread
		// TODO: Add in Error message!
		Error::SetError(_R(IDT_CANT_FIND_FILTER));
		CleanUpAfterImport(FALSE);
		return FALSE;
	}

	// Remember it
	//ImportInfo.pSpread = pSpread;

	// Get the spread's bounding rectangle and convert it to spread coords.
	DocRect SpreadRect = pSpread->GetPasteboardRect();
	pSpread->DocCoordToSpreadCoord(&SpreadRect);

	if (pPos == NULL)
	{	
		// For now, position objects on 1st page of spread 1
		Page *pPage = pSpread->FindFirstPageInSpread(); 
		ERROR2IF(pPage == NULL,FALSE,"BaseCamelotFilter::DoImport Spread has no pages"); 

		// Use bottom left of page as origin
		DocRect PageRect = pPage->GetPageRect();
		SetCoordOrigin(PageRect.lo);
	}
	else
	{
		// Use position provided
		SetCoordOrigin(pPos->Position);
	}
	
	// Try to open the file
	// The file is already open when it is passed in

	// Find out the size of the file, in bytes.
	size_t filesize = pFile->Size();
	if (filesize <= 0)
	{
		Error::SetError(BadFileMsgID);
		return FALSE;
	}

	// Set the progress indicator, this next bit might take a while.
	// String_64 ImportMessage(ImportMsgID);
	// String_64 ImportMessage = GetImportProgressString(pFile, GetImportMsgID());
	// BeginSlowJob(filesize, TRUE, &ImportMessage);
	
	// ContinueSlowJob();

	// Start up the record parser

	BOOL Ok = TRUE;
	// Must set the exception throwing flag to True and force reporting of errors to False.
	// This means that the caller must report an error if the function returns False.
	// Any calls to CCFile::GotError will now throw a file exception and should fall into
	// the catch handler at the end of the function.
	// Replaces the goto's that handled this before.
	BOOL OldThrowingState = pFile->SetThrowExceptions( TRUE );
	BOOL OldReportingState = pFile->SetReportErrors( FALSE );

	// Record parser starts here

	try
	{
		// Load that file 
		Ok = ReadFile();
	}
	catch( ... )
	{
		Ok = FALSE;
	}
  
	// Must set the exception throwing and reporting flags back to their entry states
	pFile->SetThrowExceptions( OldThrowingState );
	pFile->SetReportErrors( OldReportingState );
  
	// If the load failed for any reason, delete the subtree we have created; otherwise
	// graft it into the tree (currently bodged to be on the first layer of the second page.
	if (!Ok)
	{
		// Tidy up after ourselves.
		CleanUpAfterImport(FALSE);

		// Detach document.
		TheDocument = NULL;

		// If the user has pressed escape then set the proper error
		if (EscapePressed)
			Error::SetError(_R(IDT_IMPORT_USERABORT),0);

		pOp->FailAndExecute();
				
		// All work has been completed.
		//EndSlowJob();
	
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
		// Ensure all the bars are updated to reflect the new document
PORTNOTE("other","Removed DialogBarOp usage")
		DialogBarOp::UpdateStateOfAllBars();
#endif
		
		return FALSE;
	}

	// Try to attach any outstanding nodes
	//BOOL Success = AttachAllNodes();

	// Tidy up after ourselves
	CleanUpAfterImport(TRUE);

	// Flag this as a new format document
	// But only flag it if we are opening the document rather than importing into an exisiting one
	if (TheDocument && !TheDocument->IsImporting())
		TheDocument->SetLoadedAsVersion1File(FALSE);

#if !defined(EXCLUDE_FROM_RALPH)
	BOOL UpdateBars = TheDocument ? !TheDocument->IsAClipboard() : TRUE;
#endif

	// Detach document.
	TheDocument = NULL;

	// End job and inform caller of the success or failure of the venture.
	//EndSlowJob();

#if !defined(EXCLUDE_FROM_RALPH)
	// Ensure all the bars are updated to reflect the new document,
	// but only if it isn't a clipboard document
PORTNOTE("other","Removed DialogBarOp usage")
	if(UpdateBars)
		DialogBarOp::UpdateStateOfAllBars();
#endif

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::IncProgressBarCount(UINT32 n)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Inputs:		n = amount to inc the progress bar count by
	Returns:	TRUE if ok, FALSE if user aborted via the 'escape' key
	Purpose:	Func to incrementing the progress bar count.
				If there is a progress bar available, it is updated by this call.
	Scope: 		Protected.

********************************************************************************************/

BOOL BaseCamelotFilter::IncProgressBarCount(UINT32 n)
{
	ProgressBarCount += n;

	if (pProgress != NULL && TotalProgressBarCount > 0)
	{
		if (ProgressBarCount > TotalProgressBarCount)
			ProgressBarCount = TotalProgressBarCount;

		INT32 Percentage = INT32((ProgressBarCount*100) / TotalProgressBarCount);
#if NEW_NATIVE_FILTER
		// Progress update will return FALSE if the user has pressed escape
		// Note this in our class variable, if the caller does not respond to it
		// then we will need to next time we go around any of our loops
		EscapePressed = !pProgress->Update(Percentage);
		return !EscapePressed;
#else
		return pProgress->Update(Percentage);
#endif 
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::SetTotalProgressBarCount(UINT32 n)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/6/96
	Inputs:		n = amount to set the total progress bar count to
	Returns:	TRUE if ok, FALSE if user aborted via the 'escape' key
	Purpose:	Func to set total the progress bar count.
				If there is a progress bar available, it is updated by this call.
	Scope: 		Protected.

********************************************************************************************/

BOOL BaseCamelotFilter::SetTotalProgressBarCount(UINT32 n)
{
	ERROR3IF(n == 0,"Should only be set to a value greater that 0");

	TotalProgressBarCount = n;
	return IncProgressBarCount(0);
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::GetExportOptions( WebPrefsDlgParam *pPrefs )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pPrefs		- the WebPrefsDlgParam class which holds all of the preferences that
							can be set.
	Outputs:	pPrefs		- new versions
	Returns:	TRUE if OK, FALSE if user pressed Cancel.
	Purpose:	Allows the user to be prompted to get information for export.
				This overriden version returns True so that nothing happens.
	Scope: 		Protected.

********************************************************************************************/

BOOL BaseCamelotFilter::GetExportOptions( WebPrefsDlgParam *pPrefs )
{
// WEBSTER - markn 28/1/97
// Not needed in Webster
#ifdef WEBSTER
	return TRUE;
#else

#ifdef DO_EXPORT
	ERROR2IF(pPrefs == NULL, FALSE, "CamelotWebFilter::GetExportOptions null pPrefs");

	// Use the preference option in the baseclass camelot filter
	pPrefs->SetCompression(GetNativeCompression());

	// Use the preference option in the baseclass camelot filter;
	pPrefs->SetBmpCompression			( GetBitmapCompression () );
	pPrefs->SetExportPreviewBitmap		( GetPreviewBitmapExport () );
	pPrefs->SetConvertTextToOutlines	( GetConvertTextToOutlines () );
	pPrefs->SetConvertBlendsToOutlines	( GetConvertBlendsToOutlines () );
	pPrefs->SetRemoveInvisibleLayers	( GetRemoveInvisibleLayers () );
	pPrefs->SetRemoveUnusedColours		( GetRemoveUnusedColours () );
	pPrefs->SetMinimalWebFormat			( BaseCamelotFilter::MinimalWebFormat );
	pPrefs->SetHTMLToClipboard			( ShouldExportHTMLTag () );
	pPrefs->SetWebOk					( FALSE );

	// Find an associated OpDescriptor.
	OpDescriptor *pOp = GetDialogueOp ();

	// Call the op.
	if ( pOp != NULL )
	{
		pOp->Invoke ( static_cast <OpParam*> ( pPrefs ) );
	}

	// For now, always use the compression option, regardless of ok/cancel used
	// Means you can change the native option by using the web options dialog box
	// without having to save the file

	// Use the preference option in the baseclass camelot filter
	if ( pPrefs->GetWebOk () )
	{
		// Set up some returned variables as the user has oked them
		SetBitmapCompression		( pPrefs->GetBmpCompression () );
		SetPreviewBitmapExport		( pPrefs->GetExportPreviewBitmap () );
		SetConvertTextToOutlines	( pPrefs->GetConvertTextToOutlines () );
		SetRemoveInvisibleLayers	( pPrefs->GetRemoveInvisibleLayers () );
		SetRemoveUnusedColours		( pPrefs->GetRemoveUnusedColours () );
		SetHTMLToClipboard			( pPrefs->GetHTMLToClipboard () );

		BaseCamelotFilter::MinimalWebFormat = pPrefs->GetMinimalWebFormat ();

		return TRUE;
	}

	// It didn't work.
	else
	{
		return FALSE;
	}

#else
	return FALSE;
#endif	// DO_EXPORT
#endif	// WEBSTER
}

/********************************************************************************************

>	virtual OpDescriptor* BaseCamelotFilter::GetDialogueOp ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/10/00
	Inputs:		-
	Returns:	The OpDescriptor pointing to the dialogue used by the export proceedure.
	Purpose:	Obtains a pointer to the OpDescriptor to be used to access the relevant
				export dialogue.

********************************************************************************************/

OpDescriptor* BaseCamelotFilter::GetDialogueOp ( void )
{
	PORTNOTETRACE("other","BaseCamelotFilter::GetDialogueOp - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Just return the found OpDescriptor.
	return OpDescriptor::FindOpDescriptor( CC_RUNTIME_CLASS( NativePrefsDlg ) );
#else
	return NULL;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::PrepareToExport(CCLexFile* pFile)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pFile - the path name
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Gets things ready for the export.
				This opens the CXaraFile object and writes out the file header

********************************************************************************************/

BOOL BaseCamelotFilter::PrepareToExport(CCLexFile* pFile)
{
#ifdef DO_EXPORT
	ProgressBarMessage = GetExportProgressString(pFile, GetExportMsgID());

	// Set up the CurrentAttribute pointer.
	if ( !SetUpCurrentAttrs () )
		return FALSE;

	// Set the coord origin for all objects in the spread to be the origin of the union of
	// all the pages in the spread
	Spread* pSpread = GetSpread();
	if (pSpread != NULL)
	{
		DocRect PagesRect;
		if (pSpread->GetPagesRect(&PagesRect))
			SetCoordOrigin(PagesRect.lo);
	}

	// If we are exporting a preview bitmap then don't start up a progress bar as
	// then things will/ go awry and we will just see one progress bar.
	// We should always want the preview bitmap in native files
	// In Web files we will only want them if the user requests them
//	BOOL WantPreviewBmp = TRUE;
//	if (IsWebFilter() && !PreviewBitmapExport)
//		WantPreviewBmp = FALSE;	

	// Get the variables and classes ready for export
	BOOL ok = PrepareImportExportVars();

	if (ok) ok = pCXaraFile->OpenToWrite(pFile);
	if (ok) ok = WriteHeader();

	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteHeader()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/95
	Inputs:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Writes out the header record to the file.
	SeeAlso:	PrepareToExport, StartExportDocComponents

********************************************************************************************/

BOOL BaseCamelotFilter::WriteHeader()
{
#ifdef DO_EXPORT
	// Get the file pos of the start of the header record
	FileHeaderRecord = GetFilePos();

	CXaraFileRecord Rec(TAG_FILEHEADER,CXF_UNKNOWN_SIZE);

	BOOL ok = Rec.Init();

	if (ok) ok = Rec.WriteBuffer((BYTE*)GetExportFileType(),3);	// File type (ensuring only 3 chars are written)
	if (ok) ok = Rec.WriteUINT32(0);								// File size
	if (ok) ok = Rec.WriteUINT32(0);								// Native/Web link ID
	if (ok) ok = Rec.WriteUINT32(0);								// Precompression flags
	if (ok) ok = Rec.WriteASCII(PRODUCT_NAME);					// Producer
	if (ok) ok = Rec.WriteASCII(PRODUCT_VERSION_NO);			// Producer version
	if (ok) ok = Rec.WriteASCII(CAMELOT_VERSION_STRING);		// Producer build

	if (ok) ok = (Write(&Rec) != 0);

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::FindDocComponents()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Finds ptrs to all the doc components the filter is interested in.
				If it can't find all of them, FALSE will be returned

	Notes:		This is import as well as export

	SeeAlso:	PrepareToExport, EndExportDocComponents

********************************************************************************************/

BOOL BaseCamelotFilter::FindDocComponents()
{
	// Inform all the document components that we are about to export
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		if (pComponent->IS_KIND_OF(ColourListComponent))
			pColComponent = (ColourListComponent*)pComponent;
		else if (pComponent->IS_KIND_OF(BitmapListComponent))
			pBmpComponent = (BitmapListComponent*)pComponent;
		else if (pComponent->IS_KIND_OF(UnitListComponent))
			pUnitsComponent = (UnitListComponent*)pComponent;
		else if (pComponent->IS_KIND_OF(DocInfoComponent))
			pInfoComponent = (DocInfoComponent*)pComponent;
		else if (pComponent->IS_KIND_OF(ViewComponent))
			pViewComponent = (ViewComponent*)pComponent;
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
PORTNOTE("print","Removed PrintComponent usage")
		else if (pComponent->IS_KIND_OF(PrintComponent))
			pPrintComponent = (PrintComponent*)pComponent;
#endif
		else if (pComponent->IS_KIND_OF(FontComponent))
			pFontComponent = (FontComponent*)pComponent;

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	if (	pColComponent	== NULL ||
			pBmpComponent	== NULL ||
			pUnitsComponent == NULL ||
			pInfoComponent	== NULL ||
			pViewComponent	== NULL ||
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
			pPrintComponent	== NULL ||
#endif
			pFontComponent	== NULL)
	{
		ERROR3("Unable to find ptrs to all the objects required for import/export");
		return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::StartExportDocComponents(BOOL BeforeCompression)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/95
	Inputs:		BeforeCompression = TRUE if called before zlib compression has been turned on
									FALSE otherwise
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Tells all the doc components attached to the document that we are about to export
				to either the web or native file format

				if BeforeCompression is TRUE, then the doc components are called via StartExportBeforeComp().
				This gives doc components a chance to export records in a non-compressed form at the beginning
				of the file.

				if BeforeCompression is FALSE, then the doc components are called via StartExport().
				All records written out at this point will be zlib compressed.

	SeeAlso:	PrepareToExport, EndExportDocComponents

********************************************************************************************/

BOOL BaseCamelotFilter::StartExportDocComponents(BOOL BeforeCompression)
{
#ifdef DO_EXPORT
	ERROR2IF(TheDocument == NULL,FALSE,"NULL document ptr");

	BOOL ok = TRUE;

	// Inform all the document components that we are about to export
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (ok && pComponent != NULL && !EscapePressed)
	{
		// Inform this document component that we are about to start a Native/Web export.
		if (BeforeCompression)
			ok = pComponent->StartExportBeforeComp(this);
		else
			ok = pComponent->StartExport(this);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::EndExportDocComponents(BOOL Success)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/95
	Inputs:		Success - True if everything went swimmingly, False if just a clean up is required.
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Tells all the doc components attached to the document that we have finished exporting
				the document
	SeeAlso:	PrepareToExport, StartExportDocComponents

********************************************************************************************/

BOOL BaseCamelotFilter::EndExportDocComponents(BOOL Success)
{
#ifdef DO_EXPORT
	ERROR2IF(TheDocument == NULL,FALSE,"NULL document ptr");

	BOOL ok = TRUE;

	// Inform all the document components that we have finished exporting, so they must
	// save any pending information now.
	DocComponent *pComponent = TheDocument->EnumerateDocComponents(NULL);

	// Cannot use the EscapePressed flag as we MUST call everything on exit to clean up!
	while (ok && pComponent != NULL)
	{
		// Inform this document component that we have finished a Native/Web export.
		ok = pComponent->EndExport(this, Success);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// Now, go through all DocComponents a second time, telling them to clean up
	// their exporting data structures.
	if (!ok)
		Success = FALSE;

	pComponent = TheDocument->EnumerateDocComponents(NULL);

	// Cannot use the EscapePressed flag as we MUST call everything on exit to clean up!
	while (pComponent != NULL)
	{
		// Inform this document component that we have finished a Native/Web export.
		pComponent->CleanUpAfterExport(Success);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}


	// Set the colour and bitmap components back to null
	// This also signals the end of the doc components export
	pColComponent	= NULL;
	pBmpComponent	= NULL;
	pUnitsComponent	= NULL;
	pInfoComponent	= NULL;
	pViewComponent	= NULL;
	pPrintComponent	= NULL;
	pFontComponent	= NULL;

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::ExportPreviewBitmap(CCLexFile* pFile, Operation* pOp,
														PathName* pPath, Document* pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pFile - the file to put the exported data into
				pOp - the operation that started the export off
				pPath - the pathname of the file being exported
				pDoc - the document to export
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Exports a preview bitmap into the file in the correct format.
	SeeAlso:	DoExport;

********************************************************************************************/

BOOL BaseCamelotFilter::ExportPreviewBitmap(CCLexFile* pFile, Operation* pOp,
											PathName* pPath, Document* pDoc)
{
#ifdef DO_EXPORT
	ERROR2IF(pOp == NULL, FALSE,"BaseCamelotFilter::ExportPreviewBitmap no export operation");
	ERROR2IF(pFile == NULL, FALSE,"BaseCamelotFilter::ExportPreviewBitmap no file to export to");
	ERROR2IF(pPath == NULL, FALSE,"BaseCamelotFilter::ExportPreviewBitmap no PathName to export to");
	ERROR2IF(pDoc == NULL, FALSE,"BaseCamelotFilter::ExportPreviewBitmap no document to export");

	// Do we want a Preview Bitmap while we are here?
	// We should always want it in native files
	// In Web files we will only want them if the user requests them
	BOOL WantPreviewBmp = GetPreviewBitmapExport(); //TRUE;
	//if (IsWebFilter() && !PreviewBitmapExport)
	//	WantPreviewBmp = FALSE;	

	if (WantPreviewBmp)
	{
		// Go and find the correct Preview Bitmap Exporter
		UINT32 SearchFilter = FILTERID_PREVIEW_GIF;
		UINT32 Tag = TAG_PREVIEWBITMAP_GIF;

		// Use the preference to work out which filter we require
		switch (PreviewBitmapFilterType)
		{
			case 0:
				SearchFilter = FILTERID_PREVIEW_BMP;
				Tag = TAG_PREVIEWBITMAP_BMP;
				break;
			case 1:
				SearchFilter = FILTERID_PREVIEW_GIF;
				Tag = TAG_PREVIEWBITMAP_GIF;
				break;
			case 2:
				SearchFilter = FILTERID_PREVIEW_JPEG;
				Tag = TAG_PREVIEWBITMAP_JPEG;
				// Force the Accusoft filters to use a 75% compression by default
				//WEBSTER-Martin-02/01/97
				//AccusoftFilters::SetExportCompressionQuality(75);
				break;
			case 3:
				SearchFilter = FILTERID_PREVIEW_PNG;
				Tag = TAG_PREVIEWBITMAP_PNG;
				break;
			case 4:
				SearchFilter = FILTERID_PREVIEW_TIFF;
				Tag = TAG_PREVIEWBITMAP_TIFFLZW;
				break;
		}

		Filter* pFilter = NULL;
		pFilter = Filter::GetFirst();
		while ((pFilter != NULL) && (pFilter->FilterID != SearchFilter))
		{
			// Try the next filter
			pFilter = Filter::GetNext(pFilter);
		}

		// Make sure that we found the preview bitmap filter
		if ((pFilter != NULL) && (pFilter->IS_KIND_OF(BaseBitmapFilter)))
		{
			BitmapFillAttribute::m_doBitmapSmoothing = FALSE;			// we don't want bitmaps to be smoothed for previews
			BitmapTranspFillAttribute::m_doBitmapSmoothing = FALSE;		// we don't want bitmaps to be smoothed for previews

			// Get the system to start us a streamed preview bitmap record ready
			// to take the bitmap data
			INT32 RecordNumber =	StartStreamedRecord(Tag, CXF_UNKNOWN_SIZE);
			// If we had a problem starting the record up then exit now
			if (RecordNumber <= 0)
			{
				BitmapFillAttribute::m_doBitmapSmoothing = TRUE;			// turn bitmap smoothing back on for normal rendering/printing
				BitmapTranspFillAttribute::m_doBitmapSmoothing = TRUE;		// turn bitmap smoothing back on for normal rendering/printing
				return TRUE;
			}

			// As we are the Native (or Web) file Exporter, then we want to force
			// the Preview to a fixed size
			//WEBSTER-Martin-10/01/97
			INT32 OldPreviewSize = PreviewFilter::PreviewBitmapSize;
			// Set the Preview to be just over an inch accross
			PreviewFilter::PreviewBitmapSize = 96000;

			// Get as Bitmap Filter and set it up
			BaseBitmapFilter* pBitmapFilter = (BaseBitmapFilter*) pFilter;
			pBitmapFilter->SetPreviewBitmap(TRUE);

			// Export the Preview to the file
			BOOL ok = pFilter->DoExport(pOp, pFile, pPath, TheDocument);
			// If the Preview bitmap failed to export then we still need to clean up

			// Set it back
			pBitmapFilter->SetPreviewBitmap(FALSE);
			// Set the Preview back to its default setting
			//WEBSTER-Martin-10/01/97
			PreviewFilter::PreviewBitmapSize = OldPreviewSize;

			// Ask for the record to be ended and hence items like the size in the record header
			// to be cleaned up and hence made correct
			if (ok) ok = EndStreamedRecord();

			BitmapFillAttribute::m_doBitmapSmoothing = TRUE;			// turn bitmap smoothing back on for normal rendering/printing
			BitmapTranspFillAttribute::m_doBitmapSmoothing = TRUE;		// turn bitmap smoothing back on for normal rendering/printing
			
			return ok;
		}
		
		BitmapFillAttribute::m_doBitmapSmoothing = TRUE;			// turn bitmap smoothing back on for normal rendering/printing
		BitmapTranspFillAttribute::m_doBitmapSmoothing = TRUE;		// turn bitmap smoothing back on for normal rendering/printing
	}

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::CorrectFileHeader(CCLexFile* pFile)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pFile - the file to put the exported data into
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Corrects the file header so that it contains the correct information now
				that we have correctly reached the end of the file.
	SeeAlso:	DoExport;

********************************************************************************************/

BOOL BaseCamelotFilter::CorrectFileHeader(CCLexFile* pFile)
{
#ifdef DO_EXPORT
	// Seek back to the first thing that needs changing
	// The Start of file header record

	if (FileHeaderRecord == 0)
	{
		ERROR3("The pos of the file header is 0. What went wrong?");
		return TRUE;
	}

	pFile->seek(FileHeaderRecord+8+3);	// seek to the file size field (skip 8 byte header & file type field)

	ERROR3IF(FileSize == 0,"Erm, the file must be more than 0 bytes in size");

	// Write out the uncompressed length of file
	if (pFile->write(&FileSize, 4).fail())
		return FALSE;
TRACEUSER( "Neville", _T("Uncompressed file size is %d\n"), FileSize );

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::CleanUpAfterExport(BOOL Success)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Cleans up the memory allocated by BaseCamelotFilter::PrepareToExport() - used
				when the export process ends, either normally or abnormally.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::CleanUpAfterExport(BOOL Success)
{
#ifdef DO_EXPORT
	// Tell doc components we have finished
	// but only if there is a problem as otherwise this should be being called.
	// DoExport calls EndExportDocComponents before this as it needs to ensure that all
	// document components are exported BEFORE it stops compression so things like unused
	// colours are compressed.
	// As a double check check to see if the BMP and Colour components are null, if not
	// then this indicates the EndExportDocComponents has not been called.
	// CalcExportSize is an example where this happens

	if (!Success ||
		pColComponent	!= NULL ||
		pBmpComponent	!= NULL ||
		pUnitsComponent != NULL ||
		pInfoComponent	!= NULL ||
		pViewComponent	!= NULL ||
		pPrintComponent	!= NULL ||
		pFontComponent	!= NULL)
	{
		EndExportDocComponents(Success);
	}
				
	// Get the exported file size
	FileSize = GetNumBytesWritten();

	// Reset the vars
	ResetImportExportVars();

	// If we have our preferences class allocated then delete it
	if (pPrefs)
	{
		delete pPrefs;
		pPrefs = NULL;
	}

	// WEBSTER - markn 11/2/97
	// The tag description stuff can be used on export now
	if (pTagDescriptionList != NULL)
	{
		pTagDescriptionList->DeleteAll();
		delete pTagDescriptionList;
		pTagDescriptionList= NULL;
	}

	// WEBSTER - markn 15/2/97
	// Bug fix for selection type save option
	// Reset the default view area to DRAWING
	SetSelType(DRAWING);

	// Clean up the current attribute pointer and array.
	DeleteCurrentAttrs ();

#endif
}



/****************************************************************************

>	virtual Node* BaseCamelotFilter::GetExportNode()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/02/2005

	Returns:	NULL in times of grief
	Purpose:	Returns a pointer to the first node to export

****************************************************************************/

Node* BaseCamelotFilter::GetExportNode()
{
	// JCF: changed to begin writing with the first node following the last default attribute,
	// and to simply write out every node at the second level of the tree, not only Chapters,
	// so the traversal will include the NodeSetSentinel and its children.
	Node* pNode = TheDocument->GetFirstNode();
	if (pNode != 0 && pNode->FindNext() != 0)
	{
		pNode = pNode->FindNext()->FindLastChild(CC_RUNTIME_CLASS(NodeAttribute));
		if (pNode != 0) pNode = pNode->FindNext();
	}

	return(pNode);	
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteDocument(Operation * pExportOp)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pExportOp	- the operation that caused this export. This is used for localising
								attributes when exporting selections, so it can be NULL, if
								there is no selection.
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Exports the document pointed to by TheDocument.

				The it skips all the default attributes.  All default attributes are
				assumed in the format so don't have to be saved in the file.

				This is safe because:
				1) Current camelot loading v1 of file format
				The doc it gets loaded into will automatically create the correct default attrs

				2) Current camelot loading future version of file format with new default attr
				It wont understand new attr records, so will ignore them, removing need to create correct default attr

				3) New camelot loading v1 file format
				It will have all the default attrs for the file because this is what the format demands


	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::WriteDocument(Operation * pExportOp)
{
#ifdef DO_EXPORT
	ERROR2IF(TheDocument == NULL,FALSE,"Can't write a doc without a doc ptr");

	Node* pNode = GetExportNode();
	ERROR2IF(pNode == 0, FALSE,
					"BaseCamelotFilter::WriteDocument: can't find starting node");
	BOOL ok = TRUE;

	// Set up the name gallery, using Simon's code, and check that it's valid
	NameGallery	*pNameGallery	= NameGallery::Instance ();
	if ( pNameGallery == NULL )
	{
		// Inform the caller that an error has occurred.
		ok = FALSE;
	}

	// Update the named set information if everythings ok.
	if (ok)
		pNameGallery->FastUpdateNamedSetSizes ();

	// Karim 08/02/2001
	// Set the document up to save out shadows correctly.
	Node* pRoot = pNode;
	if (ok)
		PreExportShadows(pRoot);

	// Write out all the nodes in all the chapters
	if ( (pPrefs == NULL) || (pPrefs->GetExportSel () == DRAWING) )
	{
		while ((ok) && (pNode != NULL) && (!EscapePressed))
		{
			ok = WriteNodes(pNode);
			pNode = pNode->FindNext();
		}
	}
	// Export only the selected nodes.
	else
	{
		SelRange selection = *(GetApplication ()->FindSelection ());
		RangeControl control = selection.GetRangeControlFlags ();
		control.PromoteToParent = TRUE;
		selection.Range::SetRangeControl (control);

		ok = WriteSelectedNodes (pExportOp, pNode, &selection);
	}

	// Karim 09/02/2001
	// Revert the document to the state it was in before PreExportShadow() was called.
	PostExportShadows(pRoot);

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	BOOL BaseCamelotFilter::PreExportShadows(Node* pRoot)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2001
	Inputs:		pRoot	the root of the document to export.
	Outputs:	The document-tree containing pRoot will be modified ready for shadow export.
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Prep shadows for export. We do the following steps:

				1. Disable shadow attr-transp rejection.
				2. For each NodeShadowController & NodeShadow in the document:
					a. Localise attr-transps on the NodeShadowController.
					b. Delete any pre-existing attr-transp directly applied to NodeShadow.
					c. Create a new flat-attr-transp applied directly to NodeShadow,
						reflecting the shadow's darkness.
					d. Factorise attr-transps on the NodeShadowController.

********************************************************************************************/
BOOL BaseCamelotFilter::PreExportShadows(Node* pRoot)
{
	if (pRoot == NULL)
		return FALSE;

	PORTNOTETRACE("other","BaseCamelotFilter::PreExportShadows - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Turn off attr-transp rejection for shadows.
	NodeRenderableInkDetailsItem* pShadowDetails =
		ObjectRegistry::GetAttribDetails(CC_RUNTIME_CLASS(NodeShadow));

	CCRuntimeClass* pTranspClass = CC_RUNTIME_CLASS(AttrTranspFillGeometry);
	BOOL ok = pShadowDetails->AddAttribClass(pTranspClass);

	AttrTypeSet AttrTranspType;
	AttrTranspType.AddToSet(pTranspClass);

	// Iterate over all NodeShadowControllers in the doc.
	NodeShadow*				pShad		= NULL;
	NodeShadowController*	pControl	= NULL;
	NodeAttribute*			pAttrTransp	= NULL;

	for (Node*	pN = pRoot->FindFirstDepthFirst();
				pN != NULL && ok;
				pN = pN->FindNextDepthFirst(pRoot) )
	{
		if (pN->IsAShadow())
		{
			pShad = (NodeShadow*)pN;
			pControl = (NodeShadowController*)pShad->GetParentController();

					ok = (pControl != NULL);
			if (ok) ok = pControl->LocaliseCommonAttributes(FALSE,	// Check for duplicates
															TRUE,	// Global
															&AttrTranspType);
			if (ok)
			{
				// delete any pre-existing transp-fill applied directly to the shadow.
				if (pShad->FindAppliedAttribute(pTranspClass, &pAttrTransp) &&
					pAttrTransp->FindParent() == pShad)
				{
					pAttrTransp->CascadeDelete();
					delete pAttrTransp;
				}

				pAttrTransp = new AttrFlatTranspFill(pShad, FIRSTCHILD);
				if (pAttrTransp == NULL)
					ok = FALSE;
			}
			if (ok)
			{
				UINT32 Transp = pShad->GetTransp();
				((AttrFillGeometry*)pAttrTransp)->SetStartTransp( &Transp );
			}
			
			pControl->FactorOutCommonChildAttributes(	TRUE,	// Global
														&AttrTranspType );
		}
	}

	return ok;
#endif
	return FALSE;
}



/********************************************************************************************

>	BOOL BaseCamelotFilter::PostExportShadows(Node* pRoot)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2001
	Inputs:		pRoot	the root of the document just exported.
	Outputs:	The document-tree containing pRoot will be returned to the state it was in
				prior to a call to PreExportShadows().
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	Repair the document tree after shadow export. We do the following steps:

				1. Enable shadow attr-transp rejection.
				2. For each NodeShadowController & NodeShadow in the document:
					a. If the NodeShadow has an attr-transp, remove & delete it.
					b. Factorise attr-transps on the NodeShadowController.

********************************************************************************************/
BOOL BaseCamelotFilter::PostExportShadows(Node* pRoot)
{
	if (pRoot == NULL)
		return FALSE;

	PORTNOTETRACE("other","BaseCamelotFilter::PostExportShadows - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Turn on attr-transp rejection for shadows.
	NodeRenderableInkDetailsItem* pShadowDetails =
		ObjectRegistry::GetAttribDetails(CC_RUNTIME_CLASS(NodeShadow));

	CCRuntimeClass* pTranspClass = CC_RUNTIME_CLASS(AttrTranspFillGeometry);
	pShadowDetails->RemoveAttribClass(pTranspClass);

	AttrTypeSet AttrTranspType;
	AttrTranspType.AddToSet(pTranspClass);

	// Iterate over all NodeShadowControllers in the doc.
	NodeShadow*				pShad		= NULL;
	NodeShadowController*	pControl	= NULL;
	NodeAttribute*			pAttrTransp	= NULL;

	for (Node*	pN = pRoot->FindFirstDepthFirst();
				pN != NULL;
				pN = pN->FindNextDepthFirst(pRoot) )
	{
		if (pN->IsAShadow())
		{
			pShad = (NodeShadow*)pN;
			pControl = (NodeShadowController*)pShad->GetParentController();

			if (pControl != NULL)
			{
				if (pShad->FindAppliedAttribute(pTranspClass, &pAttrTransp) &&
					pAttrTransp->FindParent() == pShad)
				{
					pAttrTransp->CascadeDelete();
					delete pAttrTransp;
				}
				pControl->FactorOutCommonChildAttributes(TRUE, &AttrTranspType);
			}
		}
	}
#endif
	return TRUE;
}



/********************************************************************************************

>	BOOL BaseCamelotFilter::PostImportShadows()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/02/2001
	Outputs:	The document-tree just imported will have its attr-layout modified.
	Returns:	TRUE if successful, FALSE otherwise.
	Purpose:	We factor out common attributes on all NodeShadowControllers in the document.

********************************************************************************************/
BOOL BaseCamelotFilter::PostImportShadows()
{
	// Get the root-node of the document,
	// which is the first node after the last default attribute.
	Node* pRoot = TheDocument->GetFirstNode();
	if (pRoot != NULL && pRoot->FindNext() != NULL)
	{
		pRoot = pRoot->FindNext()->FindLastChild(CC_RUNTIME_CLASS(NodeAttribute));
		if (pRoot != NULL)
			pRoot = pRoot->FindNext();
	}
	if (pRoot == NULL)
		return FALSE;

	// Iterate over all NodeShadowControllers in the doc.
	NodeShadowController*	pControl	= NULL;
	for (Node*	pN = pRoot->FindFirstDepthFirst();
				pN != NULL;
				pN = pN->FindNextDepthFirst(pRoot) )
	{
		if (pN->IsAShadowController())
		{
			pControl = (NodeShadowController*)pN;
			pControl->FactorOutCommonChildAttributes(TRUE);
		}
	}

	return TRUE;
}



/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WritePreChildren(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pNode = ptr to a node to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Asks the node to write itself out.
				Calls either the Native or Web varient of the function, depending on
				the type of filter this is.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::WritePreChildren(Node* pNode)
{
#ifdef DO_EXPORT
	if (IsWebFilter())
		return pNode->WritePreChildrenWeb(this);
	else
		return pNode->WritePreChildrenNative(this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::CanWriteChildren(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pNode = ptr to a node to write
	Returns:	TRUE if yes, FALSE if no
	Purpose:	Asks the node if it's OK to automatically write out its children.
				Calls either the Native or Web varient of the function, depending on
				the type of filter this is.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::CanWriteChildren(Node* pNode)
{
#ifdef DO_EXPORT
	if (IsWebFilter())
		return pNode->CanWriteChildrenWeb(this);
	else
		return pNode->CanWriteChildrenNative(this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WritePostChildren(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pNode = ptr to a node to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Asks the node to write itself out.
				Calls either the Native or Web varient of the function, depending on
				the type of filter this is.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::WritePostChildren(Node* pNode)
{
#ifdef DO_EXPORT
	if (IsWebFilter())
		return pNode->WritePostChildrenWeb(this);
	else
		return pNode->WritePostChildrenNative(this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::ReadPostChildren(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/96
	Inputs:		pNode = ptr to a node to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Informs a node that all its children have been imported
				Calls either the Native or Web varient of the function, depending on
				the type of filter this is.
	SeeAlso:	BaseCamelotFilter::DecInsertLevel();
	Scope: 		Public

********************************************************************************************/

BOOL BaseCamelotFilter::ReadPostChildren(Node* pNode)
{
	ERROR3IF(pNode == NULL,"pNode is NULL");

	if (pNode != NULL)
	{
		if (IsWebFilter())
			return pNode->ReadPostChildrenWeb(this);
		else
			return pNode->ReadPostChildrenNative(this);
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteBeginChildRecords(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pNode = ptr to a node to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Informs a node that its child records are about to be written
				Calls either the Native or Web varient of the function, depending on
				the type of filter this is.
	SeeAlso:	BaseCamelotFilter::DecInsertLevel();
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::WriteBeginChildRecords(Node* pNode)
{
#ifdef DO_EXPORT
	if (IsWebFilter())
		return pNode->WriteBeginChildRecordsWeb(this);
	else
		return pNode->WriteBeginChildRecordsNative(this);
#else
	return FALSE;
#endif
}
	
/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteEndChildRecords(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pNode = ptr to a node to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Informs a node that its child records have been written
				Calls either the Native or Web varient of the function, depending on
				the type of filter this is.
	SeeAlso:	BaseCamelotFilter::DecInsertLevel();
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::WriteEndChildRecords(Node* pNode)
{
#ifdef DO_EXPORT
	if (IsWebFilter())
		return pNode->WriteEndChildRecordsWeb(this);
	else
		return pNode->WriteEndChildRecordsNative(this);
#else
	return FALSE;
#endif
}
	
/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteZeroSizedRecord(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		Tag = tag of zero-sized record
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Helper function that makes it easy to write out a zero-sized record.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::WriteZeroSizedRecord(UINT32 Tag)
{
#ifdef DO_EXPORT
	CXaraFileRecord Rec(Tag,0);
	return (Write(&Rec) != 0);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteNodes (Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		pNode = ptr to the node to start from.
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This is the main tree traversal routine that writes out the document in the v2 
				file format.

				It recursively writes the document, asking each node to write itself out, in 
				the correct renderable order.

				NOTE: Not only does this function recursively cycle through the nodes beneath
				pNode, It also goes through the ones next to pNode (i.e. those in the same 
				layer) and their children.

	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Public

********************************************************************************************/

BOOL BaseCamelotFilter::WriteNodes (Node* pNode)
{
#ifdef DO_EXPORT

	BOOL ok = TRUE;

	// Cycle through the nodes that we've got writing them out as we go.
	Node * pCurrentNode = pNode;
	while ((ok) && (pCurrentNode != NULL))
	{
		ok = WriteNodeAndSubNodes (pCurrentNode);
		pCurrentNode = pCurrentNode->FindNext ();
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteNodes (Node* pNode)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/2001
	Inputs:		pNode = ptr to head of a sub-tree to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This recursively writes the document, asking each node to write itself out, 
				in the correct renderable order.

				NOTE: This differs from WriteNodes, by only writing out the node and it's
				children, not the next or previous nodes.

	SeeAlso:	BaseCamelotFilter::WriteNodes, BaseCamelotFilter::WriteSelectedNodes
	Scope: 		Public

********************************************************************************************/
BOOL BaseCamelotFilter::WriteNodeAndSubNodes (Node* pNode)
{
#ifdef DO_EXPORT

	// Name Gallery code moved from here to WriteDocument to reduce number of calls to it.
	BOOL ok = TRUE;

	// (ChrisG 26/01/01) All of the code dealing with the selection is now done by 
	//	BaseCamelotFilter::WriteSelectedNodes and BaseCamelotFilter::WriteSelectedLayerAndNodes
	// 
	// This both allows the selection to be used correctly and have additional code, as 
	//	required, and should speed up the export slightly when the exporting the whole 
	//	document, as it no longer has to trudge through any selection code.

	BOOL RecordWritten = WritePreChildren(pNode);
	BOOL WriteChildren = CanWriteChildren(pNode);

	if (WriteChildren)
	{
		if (ok) ok = WriteBeginChildRecords(pNode);

		Node* pChild = pNode->FindFirstChild();
		while (ok && pChild != NULL)
		{
			ok = WriteNodeAndSubNodes (pChild);
			pChild = pChild->FindNext ();
		}
		if (ok) ok = WriteEndChildRecords(pNode);
	}

	if (ok) ok = WritePostChildren(pNode);

	// If the user has pressed escape during export - progress update returns this to us
	if (EscapePressed)
		return FALSE;

	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL BaseCamelotFilter::IsThisASelectedTemplate ( Node *pNode )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/00
	Inputs:		pNode	- A pointer to the node to be tested.
	Returns:	TRUE	- The node should be exported.
				FALSE	- It's not a template node.
	Purpose:	Determines whether or not this node is a selected template node.
	SeeAlso:	BaseCamelotFilter::WriteNodes ()
	Scope: 		Private

********************************************************************************************/

BOOL BaseCamelotFilter::IsThisASelectedTemplate ( Node *pNode )
{
	PORTNOTETRACE("other","BaseCamelotFilter::IsThisASelectedTemplate - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Set up the local variables.
	SGNameItem	*pNameGalleryItem	= NULL;
	BOOL		ExportThisNode		= FALSE;

	// Is this a template attribute?
	if ( IS_A ( pNode, TemplateAttribute ) )
	{
		String_256 Name = static_cast<TemplateAttribute*> ( pNode )->GetParam ();

		// Get the name string used for the button.
		pNameGalleryItem = SliceHelper::LookupNameGalleryItem ( Name );
	}

	// Or is it a set property attribute?
	else if ( IS_A ( pNode, NodeSetProperty ) )
	{
		String_256 Name = static_cast<NodeSetProperty*> ( pNode )->GetName ();

		// Get the name string used for the button.
		pNameGalleryItem = SliceHelper::LookupNameGalleryItem ( Name );
	}

	// Ensure that the pointer to the named item is valid before accessing it.
	if ( pNameGalleryItem != NULL &&
		 pNameGalleryItem->GetSelectedCount () > 0)
	{
		ExportThisNode = TRUE;
	}

	// Return whether or not the node needs to be considered.
	return ExportThisNode; 
#endif
	return false;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::BeginDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called just before document-related records are written out
				to the file.  EndDocumentExport() is called after all the doc records have been saved

				This should be called for each document that is saved out to the file.

	SeeAlso:	BaseCamelotFilter::EndDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::BeginDocumentExport()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::EndDocumentExport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/7/96
	Inputs:		-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	This function is called after document-related records have been are written out
				to the file.  BeginDocumentExport() is called before the doc records are saved

				This should be called for each document that is saved out to the file.

	SeeAlso:	BaseCamelotFilter::BeginDocumentExport()
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::EndDocumentExport()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::DoExport(Operation *pOp, CCLexFile* pFile,
											   PathName * pPath, Document *pDoc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> & Markn
	Created:	29/5/96
	Inputs:		pOp - the operation that started the export off
				pFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pDoc - the document to export
				ShowOptions - Are the export options needed?
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Exports the current document out to file. Uses the virtual functions of this
				class. Do not override unless really necessary.
	SeeAlso:	GetExportOptions; PrepareToExport; CleanUpAfterExport;

********************************************************************************************/

BOOL BaseCamelotFilter::DoExport ( Operation* pOp, CCLexFile* pFile, PathName* pPath,
								   Document* pDoc, BOOL ShowOptions )
{
#ifdef DO_EXPORT
	ERROR2IF(pOp == NULL, FALSE,"BaseCamelotFilter::DoExport no export operation");
	ERROR2IF(pFile == NULL, FALSE,"BaseCamelotFilter::DoExport no file to export to");
	ERROR2IF(pPath == NULL, FALSE,"BaseCamelotFilter::DoExport no export pathname");
	ERROR2IF(pDoc == NULL, FALSE,"BaseCamelotFilter::DoExport no document to export");

	// Set up document pointer
	TheDocument = pDoc;

	// Get pointer to the spread to export.
	pTheSpread = GetFirstSpread(pDoc);
	ERROR2IF(pTheSpread == NULL, FALSE,"BaseCamelotFilter::DoExport no spread to export");

	// We must now check if there is a selection present so that we can set up whether the
	// user gets the choice of exporting the selection, drawing or spread if there is a 
	// selection present OR just a choice between the spread or drawing if no selection is
	// present.
	// If have a caret selected in a text story then the selection will be almost zero so trap
	// this case as well. 

	SelectionType Selection = DRAWING;
	if (ShowOptions)
	{
		Application * pApp = GetApplication();
		ERROR2IF(pApp == NULL, FALSE,"BaseCamelotFilter::DoExport no application!");
		SelRange* pRange = pApp->FindSelection();
		ERROR2IF(pRange == NULL, FALSE,"BaseCamelotFilter::DoExport no selection range!");
		DocRect ClipRect = pRange->GetBoundingRect();
 		if (ClipRect.IsEmpty())
			Selection = DRAWING;		// no selection present, so choose drawing by default
		else
			Selection = SELECTION;		// selection present, so choose this by default

		if (Selection == DRAWING)
		{
			// Work out the size of the rectangle encompassing the drawing (visible layers only)
			ClipRect = BaseBitmapFilter::GetSizeOfDrawing(pTheSpread);

			// Check that that cliprect is ok, if not then set the spread as the export type
 			if (ClipRect.IsEmpty())
				Selection = SPREAD;
		}
	}

	// See if the Camelot filters need some exporting options or not
	// First, create the preferences class we use to get the data to and from the dialog box
	pPrefs = new WebPrefsDlgParam;

	if (pPrefs == NULL)
		return FALSE;

	// Set the viewport selection here.
	if (ShowOptions)
	{
		pPrefs->SetExportSel ( Selection );
		pPrefs->SetViewportSel ( Selection );

		// Invoke the dialogue box.
		if (!GetExportOptions(pPrefs))
		{
			// User has cancelled the operation so set our error which will be suppressed
			// and return FALSE
			Error::SetError(_R(IDN_USER_CANCELLED),0);

			// Clean up the old preferences.
			delete pPrefs;
			pPrefs = NULL;

			return FALSE;
		}
	}
	else
	{
		// Do we want the dialogue box for export options? If not, ensure that the export and
		// viewport selections don't screw things up.
		// Graeme (25/1/00) - Changed SelType to ExportSel and ViewportSel.
		pPrefs->SetExportSel ( DRAWING );
		pPrefs->SetViewportSel ( DRAWING );
	}


	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already. In the PreviewBitmap case the file will already be open.
	if (!pFile->isOpen())
	{
		if (pFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{		
			if (!OpenExportFile((CCDiskFile*) pFile, pPath))
			{
				if (pPrefs)
				{
					delete pPrefs;
					pPrefs = NULL;
				}
				return FALSE;
			}
		}
		else
		{
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in BaseCamelotFilter::DoExport\n") );
			return FALSE;
		}
	}
	
	// Get ready for the binary header by setting all our variables to their initial states
	FileStart = 0;
	FileEnd = 0;
	FileHeaderRecord = 0;
	FileSize = 0;

	BOOL ok = TRUE;
	EscapePressed = FALSE;

//	TRY
	{
		// This needs to be fixed sensibly as it uses the value that is 
		// currently in the TotalProgressBarCount variable
		// UINT32 ExportSize = CalcExportSize(TheDocument);

		// This resets any export vars, opens the file and writes the header out
		if (ok) ok = PrepareToExport(pFile);

		// After we have exported the header, we must export the preview bitmap, if required.
		// Make sure we use the function to get the CCFile pointer or the template version wont
		// get written out properly...
		if (ok) ok = ExportPreviewBitmap(GetCCFile(), pOp, pPath, pDoc);

		// Start up the progress bar - happends after the preview bmp (in case it starts up a progress bar)
		if (ok) StartProgressBar(&ProgressBarMessage);

		// From this point on, everything in the file is related to this document
		if (ok) ok = BeginDocumentExport();

		// WEBSTER - markn 11/2/97
		// Write out a tag description record if necessary
		WriteTagDescriptionRecord();

		// Write out some simple document-scope values
		WriteNudgeSizeRecord();
		WriteBitmapSmoothingRecord();
		WriteDuplicationOffsetRecord();

/*		// Find all the document components that are required
		// Must do this just before we start the doc components export as otherwise things which fail
		// will cause clean up to occur and once FindDocComponents is called, it assumes that start
		// StartExportDocComponents has been called.
		if (ok ok = FindDocComponents(); */

		// Start doc components (before compression is started)
		if (ok) ok = StartExportDocComponents(TRUE);

		// Just before we write the main document, we will start compression up, if required
		if (ok) ok = StartCompression();

		// Start doc components (after compression started)
		if (ok) ok = StartExportDocComponents(FALSE);

		// Write out all remaining automic tags (note brush defintion and stroke definition have already
		// been written within the above call).  I am not going to move these into here ....
		if (ok) ok = WriteRemainingAtomicTagDefinitions ();		// for bevels, contours, shadows, clipviews, etc.

		// Save out the document tree
		if (ok) ok = WriteDocument(pOp);

		// Save out the current attributes...
		if (ok) ok = WriteCurrentAttributes();

		// Tell doc components we have finished
		if (ok) ok = EndExportDocComponents(TRUE);

		// Just before we write the EOF record and close down the file
		// we will stop compression
		if (ok) ok = StopCompression();

		// Everything in the document has been saved
		if (ok) ok = EndDocumentExport();

		if (ok)
		{
			// Write out the EOF record as the last record in the file
			WriteZeroSizedRecord(TAG_ENDOFFILE);

			// All done - deallocate dynamic objects and return success.
			CleanUpAfterExport(TRUE);

			// Detach document
			TheDocument = NULL;

			// Find out where the file ended
			FileEnd = pFile->tell();

			// Correct the binary header, now we know where everything is
			if (!CorrectFileHeader(pFile))
			{
				ERROR1(FALSE, _R(IDT_EXPORT_INTERNAL_ERR));
			}
		}
	}
#if 0
	CATCH(CFileException, e)
	{
		TRACE( _T("BaseCamelotFilter::DoExport An error has occurred during export!\n") );
		// Flag an error has occurred so that things get cleaned up correctly
		ok = FALSE;
	}
	END_CATCH
#endif
	// Clean up if we get an error
	if (!ok)
	{
		CleanUpAfterExport(FALSE);

		// If the user has pressed escape then set the proper error
		if (EscapePressed)
			Error::SetError(_R(IDW_CANCELEXPORT),0);

		return FALSE;
	}

	//Graham 22/5/97: Export an HTML tag to the clipboard if necessary
	if (ShouldExportHTMLTag())
		ExportHTMLTag(pPath);

	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual UINT32 BaseCamelotFilter::CalcExportSize()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/6/96
	Inputs:		-
	Returns:	The estimated number of bytes that will be exported
	Purpose:	Calcs the number of bytes that will be exported to the file.
				It is used to maintain a progress display.
	SeeAlso:	GetExportOptions; PrepareToExport; CleanUpAfterExport;

********************************************************************************************/

UINT32 BaseCamelotFilter::CalcExportSize(Document* pDocument)
{
#ifdef DO_EXPORT
	ERROR2IF(pDocument == NULL,0,"pDocument is NULL");

	UINT32 NumBytes = 0;

	BaseCamelotFilter* pFilter = CreateNULLFilter(pDocument);

	if (pFilter != NULL)
	{
		BOOL ok = TRUE;

		// Prepare to export
		if (ok) ok = pFilter->PrepareToExport(NULL);

		// Start doc components (before & after compression)
		if (ok) ok = pFilter->StartExportDocComponents(TRUE);
		if (ok) ok = pFilter->StartExportDocComponents(FALSE);

		String_64 Str(_R(IDS_PREPARING_TO_SAVE));
		if (ok) StartProgressBar(&Str);				// Update status bar
		// Export document tree
		if (ok) ok = pFilter->WriteDocument(NULL);
		if (ok) EndProgressBar();					// Kill progess bar

		// Tell doc components we have finished
		if (ok) ok = pFilter->EndExportDocComponents(TRUE);

		// All done - deallocate dynamic objects and return success.
		pFilter->CleanUpAfterExport(ok);

		// Set the main filter's progress total to the final progress count of the NULL filter
		SetTotalProgressBarCount(pFilter->GetProgressBarCount());

		delete pFilter;
		pFilter = NULL;
	}

	return NumBytes;
#else
	return 0;
#endif
}


/********************************************************************************************

>	virtual void BaseCamelotFilter::GotError(UINT32 errorID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/96
	Inputs:		errorID = ID of error resource string
	Returns:	-
	Purpose:	Sets an error using the given resource string
				All it does is call the mirror function in the attached CXaraFile.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::GotError(UINT32 errorID)
{
	if (pCXaraFile != NULL)
		pCXaraFile->GotError(errorID);
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::GotError( UINT32 errorID , const TCHAR* errorString)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		errorID = ID of error resource string
				errorString = ptr to the error string.
	Returns:	-
	Purpose:	Sets an error using the given resource string
				All it does is call the mirror function in the attached CXaraFile.
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::GotError(UINT32 errorID, const TCHAR* errorString)
{
	if (pCXaraFile != NULL)
		pCXaraFile->GotError(errorID,errorString);
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::UnrecognisedTag(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Tag = Tag of the record unrecognised by the importer
	Returns:	TRUE if ok to carry on, FALSE otherwise
	Purpose:	Informs the filter that it didn't recognise the given tag

				This function will work out what to do
				If the tag is an essential one, the import will be aborted
				If the tag is atomic, the next subtree will be ignored
				
				If the import is not aborted, the user will be given a suitable warning at the
				end of import, telling him (or her, of course) which tags were ignored.

	SeeAlso:	-
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::UnrecognisedTag(UINT32 Tag)
{
	if (IsTagInEssentialList(Tag))
	{
		// We must understand this tag if we are to render this file correctly
		// However, we don't, so error to the user, and abort the importing of the file
		UnkownEssentialDataError(Tag);
		return FALSE;
	}

	// If it's not essential that we recognise this tag, then let the user know we are
	// ignoring it
	AppendIgnoredDataWarning(Tag);

	// If it's an atomic tag, then we must ignore the record AND IT'S SUB TREE!
	if (IsTagInAtomicList(Tag) && pCXaraFile != NULL)
		pCXaraFile->StripNextSubTree();

	return TRUE;
}

/********************************************************************************************

>	void BaseCamelotFilter::AppendWarning(UINT32 WarningId)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/8/96
	Inputs:		WarningId = id of resource to be added 
	Returns:	-
	Purpose:	Appends a warning saying that it's come across some data it doesn't
				recognise.	This allows the user to be warned about things like:-
					bitmap definitions not being loaded
					colour definitions not being loaded

	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Public

********************************************************************************************/

void BaseCamelotFilter::AppendWarning(UINT32 WarningId)
{
	// We will only add items until we are close to the end of the string's capacity.
	// Otherwise, seeing half entries will look strange.
	if (WarningsString.IsEmpty())
		WarningsString = String_256(_R(IDS_NATIVE_DATAWARNING));

	String_256 Sep(_R(IDS_TAG_ERROR_LIST_SEP)); 

	String_256 Temp(WarningId);
	// Only add it if there is enough room to fit it all in
	if (Sep.Length() + Temp.Length() + WarningsString.Length() < WarningsString.MaxLength())
	{
		WarningsString += Sep;
		WarningsString += Temp;
	}
}

/********************************************************************************************

>	void BaseCamelotFilter::AppendIgnoredDataWarning(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Tag = tag of the record not recognised
	Returns:	-
	Purpose:	Appends a warning saying that it's come across some data it doesn't
				recognise

	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::AppendIgnoredDataWarning(UINT32 Tag)
{
	// We will only add items until we are close to the end of the string's capacity.
	// Otherwise, seeing half entries will look strange.
	if (IgnoredDataString.IsEmpty())
		IgnoredDataString = String_256(_R(IDS_TAG_WARNING_IGNORED_DATA));

	String_256 Sep(_R(IDS_TAG_ERROR_LIST_SEP)); 

	TagDescriptionListItem* pDesc = GetTagDescription(Tag);
	String_256 Temp;
	if (pDesc != NULL && pDesc->GetString() != NULL)
		Temp = *(pDesc->GetString());
	else
		Temp.MakeMsg(_R(IDS_TAG_ERROR_TAG_NUMBER),Tag);
	
	// Only add it if there is enough room to fit it all in
	if (Sep.Length() + Temp.Length() + IgnoredDataString.Length() < IgnoredDataString.MaxLength())
	{
		IgnoredDataString += Sep;
		IgnoredDataString += Temp;
	}
}

/********************************************************************************************

>	void BaseCamelotFilter::UnkownEssentialDataError(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/8/96
	Inputs:		Tag = tag of the record not recognised
	Returns:	-
	Purpose:	Errors that a tag is not recognised, yet it is an essential tag (i.e. one that must
				be recognised in order to load the file correctly).

				It also makes sure that the correct error message is set.

	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::UnkownEssentialDataError(UINT32 Tag)
{
	String_256 Str(_R(IDS_TAG_ERROR_UNKNOWN_ESSENTIAL));

	Str += String_256(_R(IDS_TAG_ERROR_LIST_SEP));

	TagDescriptionListItem* pDesc = GetTagDescription(Tag);
	if (pDesc != NULL && pDesc->GetString() != NULL)
		Str += *(pDesc->GetString());
	else
	{
		String_256 Temp;
		Temp.MakeMsg(_R(IDS_TAG_ERROR_TAG_NUMBER),Tag);
		Str += Temp;
	}

	Error::SetError(0,Str,0);
}

/********************************************************************************************

>	INT32 BaseCamelotFilter::WriteRecord(DocColour* pDocColour)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/5/96
	Inputs:		pDocColour = ptr to a doc colour
	Returns:	The record number of the record that defines the given colour
				Or < 1 if there is an error
	Purpose:	Writes out the given colour to the file, returning the record number
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

INT32 BaseCamelotFilter::WriteRecord(DocColour* pDocColour)
{
#ifdef DO_EXPORT
	ERROR3IF(pDocColour    == NULL,"NULL pDocColour given");
	ERROR3IF(pColComponent == NULL,"NULL pColComponent");

	if (pColComponent != NULL && pDocColour != NULL)
		return pColComponent->GetWriteColourReference(pDocColour,this);

	return 0;
#else
	return -1;
#endif
}

/********************************************************************************************

>	INT32 BaseCamelotFilter::WriteRecord(KernelBitmap* pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pBitmap = ptr to a KernelBitmap
	Returns:	The record number of the record that defines the given bitmap
				Or < 1 if there is an error
	Purpose:	Writes out the given bitmap to the file, returning the record number
	SeeAlso:	BaseCamelotFilter::PrepareToExport; BaseCamelotFilter::DoExport
	Scope: 		Protected

********************************************************************************************/

INT32 BaseCamelotFilter::WriteRecord(KernelBitmap* pBitmap)
{
#ifdef DO_EXPORT
	ERROR3IF(pBitmap    == NULL,"NULL pBitmap given");
	ERROR3IF(pBmpComponent == NULL,"NULL pBmpComponent");

	if (pBmpComponent != NULL && pBitmap != NULL)
		return pBmpComponent->GetWriteBitmapReference(pBitmap, this);

	return 0;
#else
	return -1;
#endif
}

/********************************************************************************************

>	INT32 BaseCamelotFilter::WriteFontDefinition(INT32 FontHandle, BOOL IsBold, BOOL IsItalic)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/08/96
	Inputs:		FontHandle	- Font managers handle
				IsBold		- Bold?
				IsItalic	- Italic?
	Returns:	Record number of font definition record
	Purpose:	Writes a font definition record to the file, and returns with the record
				number of the record.

********************************************************************************************/

INT32 BaseCamelotFilter::WriteFontDefinition(WORD FontHandle, BOOL IsBold, BOOL IsItalic)
{
#ifdef DO_EXPORT
	ERROR2IF(pFontComponent==NULL, NULL, "Member variable pFontComponent==NULL.");
	
	return pFontComponent->WriteFontDefinition(this, FontHandle, IsBold, IsItalic);
#else
	return -1;
#endif
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::SetCoordOrigin(const DocCoord& Origin)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		Origin = the origin of all spread coords that are written out.
	Returns:	-
	Purpose:	Sets the origin for all coords that are written to or read from the file.

				The purpose of this function is to set a constant X & Y translation to all
				coords so that when the coord is imported, it can be retranslated relative to
				a consistant reference point.

				The problem with using the spread origin is that it is not a constant distance from
				the page origin.  This means that if you change the paste board size, an imported object
				may be put in the wrong place on the paper.

				To overcome this, the new format uses the page origin as the origin of all objects, which
				gives us a better chance of importing objects to a sensible place on the page.

	SeeAlso:	Spread::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::SetCoordOrigin(const DocCoord& Origin)
{
	CoordOrigin = Origin;
}

/********************************************************************************************

>	virtual DocCoord BaseCamelotFilter::GetCoordOrigin()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		-
	Returns:	The orgin applied to all spread coords
	Purpose:	The Get function

	SeeAlso:	Spread::WritePreChildrenWeb(BaseCamelotFilter* pFilter), SetCoordOrigin()
	Scope: 		-

********************************************************************************************/

DocCoord BaseCamelotFilter::GetCoordOrigin()
{
	return CoordOrigin;
}

//--------------------------------------------------------------
// The following functions are just mapping functions onto those supplied
// by the attached CXaraFile object.
// They make it easy for objects that receive one of these filters to write
// to the format
//
// For more information on these functions, see the header of the corresponding
// CXaraFile function

INT32 BaseCamelotFilter::StartRecord(UINT32 Tag,INT32 Size)
{
	UINT32 RecordNumber = 0;

	if (pCXaraFile != NULL)
		RecordNumber = pCXaraFile->StartRecord(Tag,Size);

	return RecordNumber;
}

INT32 BaseCamelotFilter::StartStreamedRecord(UINT32 Tag,INT32 Size)
{
	UINT32 RecordNumber = 0;

	if (pCXaraFile != NULL)
		RecordNumber = pCXaraFile->StartStreamedRecord(Tag,Size);

	return RecordNumber;
}

BOOL BaseCamelotFilter::EndRecord()
{
	if (pCXaraFile != NULL)
		return pCXaraFile->EndRecord();

	return FALSE;
}

BOOL BaseCamelotFilter::EndStreamedRecord()
{
	if (pCXaraFile != NULL)
	{
		// Ask the CXaraFile class to end that record for us
		// Get the function to tell us how big it thinks the record is
		// This is the size of the data i.e. does not include the header
		UINT32 RecordSize = 0L;
		BOOL ok = pCXaraFile->EndStreamedRecord(&RecordSize);
		// Update the progress bar count with this amount plus 8 for the header
		//IncProgressBarCount(RecordSize + 8);
//TRACEUSER( "Neville", _T("EndStreamedRecord update progress by %d\n"),RecordSize + 8);
		return ok;
	}

	return FALSE;
}

FilePos BaseCamelotFilter::GetFilePos()
{
	if (pCXaraFile != NULL)
		return pCXaraFile->GetFilePos();

	return 0;
}

UINT32 BaseCamelotFilter::GetNumBytesWritten()
{
	if (pCXaraFile != NULL)
		return pCXaraFile->GetNumBytesWritten();

	return 0;
}

void BaseCamelotFilter::SetTotalNumBytesToRead(UINT32 n)
{
	ERROR3IF(pCXaraFile == NULL,"NULL pCXaraFile");

	FileSize = n;

	if (pCXaraFile != NULL)
		pCXaraFile->SetTotalNumBytesToRead(n);
}

UINT32 BaseCamelotFilter::GetRecordTag()
{
	if (pCXaraFile != NULL)
		return pCXaraFile->GetRecordTag();

	return FALSE;
}

INT32 BaseCamelotFilter::GetRecordNum()
{
	if (pCXaraFile != NULL)
		return pCXaraFile->GetRecordNum();

	return FALSE;
}

BOOL BaseCamelotFilter::Write(BYTE b)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(b);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::Write(UINT32 n)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(n);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::Write(INT32 n)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(n);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::Write(FLOAT f)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(f);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::Write(TCHAR* pStr)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(pStr);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::WriteWCHAR(WCHAR w)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->WriteWCHAR(w);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::WriteASCII(TCHAR* pStr)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->WriteASCII(pStr);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::WriteUnicode(TCHAR* pStr)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->WriteUnicode(pStr);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::Write(BYTE* pBuf,UINT32 BufSize)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(pBuf,BufSize);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::Write(const DocCoord& Coord)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->Write(Coord);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::WriteBitmapSource(const BitmapSource& Source, UINT32 Height)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->WriteBitmapSource(Source, Height, this);
#endif
	return FALSE;
}

BOOL BaseCamelotFilter::WriteCCPanose(const CCPanose & Panose)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
		return pCXaraFile->WriteCCPanose(Panose);
#endif
	return FALSE;
}

UINT32 BaseCamelotFilter::Write(CXaraFileRecord* pRecord)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
	{
		IncProgressBarCount(pRecord->GetSize()+8);
		return pCXaraFile->Write(pRecord);
	}
#endif
	return 0;
}


// Base class version just calls CXaraFile::Write(CXaraFileRecord*)

UINT32 BaseCamelotFilter::WriteDefinitionRecord(CXaraFileRecord* pRecord)
{
#ifdef DO_EXPORT
	if (pCXaraFile != NULL)
	{
		IncProgressBarCount(pRecord->GetSize()+8);
		return pCXaraFile->WriteDefinitionRecord(pRecord);
	}
#endif
	return 0;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::IsImportingAtPosition()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96
	Inputs:		-
	Returns:	TRUE if importing into an existing doc at a specific position.
				FALSE if not
	Purpose:	This will return TRUE only if we are importing into an existing doc
				AND at a specific position.

				This will only return TRUE for drag & drop importing

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::IsImportingAtPosition()
{
	return IsImporting() && (ImportInfo.pPos != NULL);
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::IsImporting()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Returns:	TRUE if importing into an existing doc.
				FALSE if importing into a fresh doc (i.e. openning the file)
	Purpose:	Used to see if the doc is being imported into an existing doc, loaded
				into a fresh doc.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::IsImporting()
{
	ERROR2IF(TheDocument == NULL,FALSE,"No document ptr");

	return TheDocument->IsImporting();
}

/********************************************************************************************

>	virtual Node* BaseCamelotFilter::GetInsertContextNode()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	ptr to the context node
	Purpose:	Returns the node to add the next node to.

				If no context node is found, then a context node is found (and if necessary created)
				by this function

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

Node* BaseCamelotFilter::GetInsertContextNode()
{
	if (pInsertContextNode == NULL)
	{
		ERROR2IF(TheDocument == NULL,FALSE,"TheDocument is NULL!");

		Chapter* pChapter = Node::FindFirstChapter(TheDocument);
		if (pChapter != NULL)
		{
			Spread* pSpread = pChapter->FindFirstSpread();
			if (pSpread != NULL)
			{
				Layer* pLayer = pSpread->FindFirstLayer();
				if (pLayer == NULL)
				{
					pLayer = new Layer;
				    if (pLayer != NULL)
					{
						if (!AttachNode(pLayer,pSpread,LASTCHILD))
							return NULL;

						String_256 Name1(_R(IDS_K_EPSFILTER_IMPORTED));
						String_256 Name2(_R(IDS_LAYER_DESCRS));
						Name1 += Name2;
						pLayer->SetLayerID(Name1);
						pLayer->EnsureUniqueLayerID();
					}
				}

				pInsertContextNode = pLayer;
				InsertNextNodeAsChild();
			}
		}
	}

	return pInsertContextNode;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::SetInsertContextNode(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		pNode = the node to use as a context node
	Returns:	-
	Purpose:	Sets the context node to the node provided
	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::SetInsertContextNode(Node* pNode)
{
	pInsertContextNode = pNode;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::InsertNextNodeAsChild()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Returns:	-
	Purpose:	This call ensures that the next node inserted via InsertNode()
				will be inserted as a child of the context node
	SeeAlso:	BaseCamelotFilter::InsertNode
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::InsertNextNodeAsChild()
{
	InsertNextAsChild = TRUE;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::InsertNextNodeAsSibling()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		-
	Returns:	-
	Purpose:	This call ensures that the next node inserted via InsertNode()
				will be inserted as a sibling of the context node
	SeeAlso:	BaseCamelotFilter::InsertNode
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::InsertNextNodeAsSibling()
{
	InsertNextAsChild = FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::InsertNode(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		pNode = the node to insert
	Returns:	-
	Purpose:	This inserts the given node relative to the current context node.

				If InsertNextAsChild is TRUE,  the node is inserted as the last child of the context node
				If InsertNextAsChild is FALSE, the node is inserted as the next sibling to the context node

				This function sets InsertNextAsChild to FALSE, so that all subsequent calls add siblings.

				InsertNextAsChild is controlled by the IncInsertLevel() and DecInsertLevel() funcs

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::InsertNode(Node* pNode)
{
	ERROR2IF(pNode == NULL,FALSE,"pNode is NULL");
	ERROR2IF(pInsertLevelStack == NULL,FALSE,"pInsertLevelStack == NULL");

	switch (m_InsertMode)
	{
		case INSERTMODE_ATTACHTOTREE:
		{
			BOOL ok = FALSE;
			BOOL Attached = FALSE;

			// Get the context node to which the node will be attached to.
			Node* pContextNode = GetInsertContextNode();
			if (pContextNode == NULL)
				return FALSE;

			if (pNode->IsLayer())
			{
				if (pContextNode->IsLayer() && InsertNextAsChild)
				{
					ok = AttachNode(pNode,pContextNode,NEXT);

					// If the layer is the one which was created for importing purposes only, remove it
					if (ok && pContextNode == pImportLayer)
					{
						ok = DeleteNode(pContextNode); 
						pImportLayer = NULL;
					}

					if (ok) SetInsertContextNode(pNode);
					if (ok) Attached = TRUE;
				}
			}

			if (!Attached && pContextNode != NULL)
			{
				AttachNodeDirection Direction = NEXT;
				if (InsertNextAsChild)
				{
					Direction = LASTCHILD;
					InsertNextNodeAsSibling();
				}

				ok = AttachNode(pNode,pContextNode,Direction);

				if (ok) SetInsertContextNode(pNode);
				if (ok) Attached = TRUE;
			}

			if ( ok )
			{
				InsertLevelStackItem* pItem = pInsertLevelStack->Get();
				ERROR3IF(pItem == NULL,"no tail item");
				if (pItem != NULL)
				{
					// Make this node the last one inserted on the level.
					if (!pItem->SetLastInsertedNode(pNode))
						return FALSE;

					UpdateLastSafeToRenderNode(pNode);
				}
			}

			return ok;
		}
		break;

		case INSERTMODE_SETCURRENTATTRIBUTE:	
		{
			AttributeManager* pAttrManager = &(TheDocument->GetAttributeMgr());

			if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeAttribute)) && m_pCurrentAttrGroup)
			{
				// Forcibly set this attribute as current (by setting bForceNewValue TRUE)
				// because the bounding rect may yet be changed by a SETCURRENTATTRIBUTEBOUNDS
				// record
				NodeAttribute* pCurrentAttr = pAttrManager->UpdateCurrentAttribute(m_pCurrentAttrGroup, (NodeAttribute*)pNode, TRUE, FALSE, FALSE, TRUE);
				SetLastCurrentAttrInserted(pCurrentAttr);
			}

			return TRUE;
		}
		break;

		default:
			ERROR3("Unknown InsertMode\n");
	}

	return FALSE;
}

/********************************************************************************************

>	virtual InsertTreeContext *BaseCamelotFilter::GetInsertContext(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97

	Returns:	A pointer to a record from which the current insertion context can be
				restored at a later date.

	Purpose:	If you wish to use SetInsertcontextNode to alter where the filter
				imports to, then chances are you'll want to save & restore the
				insertion state so that subsequent importing can continue where
				it left off. If this is the case, bracket your subtree import with
				code like this:
					InsertTreeContext *pContext = pFilter->GetInsertContext();
					... import your subtree or whatever ...
					pFilter->RestoreInsertContext(pContext);

	SeeAlso:	BaseCamelotFilter::RestoreInsertContext

********************************************************************************************/

InsertTreeContext *BaseCamelotFilter::GetInsertContext(void)
{
	InsertTreeContext *pContext = new InsertTreeContext;
	if (pContext != NULL)
	{
		pContext->pContextNode  = GetInsertContextNode();
		pContext->InsertAsChild = InsertNextAsChild;
	}

	return(pContext);
}



/********************************************************************************************

>	virtual void BaseCamelotFilter::RestoreInsertContext(InsertTreeContext *pOldState)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/3/97

	Returns:	A pointer to a record from which the current insertion context can be
				restored at a later date.

	Purpose:	If you wish to use SetInsertcontextNode to alter where the filter
				imports to, then chances are you'll want to save & restore the
				insertion state so that subsequent importing can continue where
				it left off. If this is the case, bracket your subtree import with
				code like this:
					InsertTreeContext *pContext = pFilter->GetInsertContext();
					... import your subtree or whatever ...
					pFilter->RestoreInsertContext(pContext);

	SeeAlso:	BaseCamelotFilter::GetInsertContext

********************************************************************************************/

void BaseCamelotFilter::RestoreInsertContext(InsertTreeContext *pOldState)
{
	if (pOldState != NULL)
	{
		SetInsertContextNode(pOldState->pContextNode);
		if (pOldState->InsertAsChild)
			InsertNextNodeAsChild();
		else
			InsertNextNodeAsSibling();
	}
}




void BaseCamelotFilter::UpdateLastSafeToRenderNode(Node* pNode)
{
	if (TheDocument == NULL)
	{
		ERROR3("NULL doc ptr");
		return;
	}

	Node* pSafeNode = NULL;

	if (pLastUnsafeToRenderNode != NULL)
	{
		if (!pLastUnsafeToRenderNode->AreYouSafeToRender())
			return;

		pSafeNode = pLastUnsafeToRenderNode;
		pLastUnsafeToRenderNode = NULL;
	}

	if (pNode != NULL)
	{
		if (pNode->AreYouSafeToRender())
			pSafeNode = pNode;
		else
		{
			pLastUnsafeToRenderNode = pNode;
			//String_256 Details = String_256((TCHAR*)(pNode->GetRuntimeClass()->m_lpszClassName));
			//TRACEUSER( "Markn", _T("Last unsafe node = %s - 0x%x\n"),(TCHAR*)Details,pNode);
		}
	}

	if (pSafeNode != NULL)
	{
		if (pSafeNode->IsAnObject() && (!(IS_A(pSafeNode,NodeGroup))))
		{
			TheDocument->GetSafeRenderPointer().UpdateLastSafeNode(pSafeNode);
		//String_256 Details = String_256((TCHAR*)(pSafeNode->GetRuntimeClass()->m_lpszClassName));
		//TRACEUSER( "Markn", _T("Last safe node = %s - 0x%x\n"),(TCHAR*)Details,pSafeNode);
		}
	}
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::InvalidateRegion(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96
	Inputs:		pNode = ptr to node to invalidate
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Invalidates the area of the document covered by the node, causing it to redraw.

				This will only redraw the node if we are importing AND have an undo op ptr
				and the node is a bounded node

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::InvalidateRegion(Node* pNode)
{
	BOOL IsBounded = pNode->IsAnObject() || pNode->IsPaper();

	if (IsImporting() && IsBounded && (ImportInfo.pOp != NULL))
		return ImportInfo.pOp->DoInvalidateNodeRegion((NodeRenderableBounded*)pNode,TRUE);

	return TRUE;
}

/********************************************************************************************
>	void BaseCamelotFilter::SetAttachMode(AttachMode newAttachMode, CCRuntimeClass* pAttrGroup = NULL)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/03/2004
	Inputs:		AttachMode - New attachMode setting
	Returns:	-
	Purpose:	Set Mode for AttachNode function
	SeeAlso:	-
	Scope: 		Protected
********************************************************************************************/

void BaseCamelotFilter::SetInsertMode(InsertMode newInsertMode, CCRuntimeClass* pAttrGroup)
{
	m_InsertMode = newInsertMode;
	m_pCurrentAttrGroup = pAttrGroup;
}


/********************************************************************************************
>	BOOL BaseCamelotFilter::AttachNode(Node* pNewNode, Node* pContextNode,
									   AttachNodeDirection Direction)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		pNewNode = ptr to node to attach to pContextNode
				pContextNode = the context node
				Direction = how to attach it to pContextNode
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Attachs one node to another. This will attach the node undoably when it
				has to.  At the mo, insertion is only performed undoably when all the
				following conditions are met:-

				1) We are importing, and not opening the file;
				2) pNewNode is a layer, or pContextNode is a NodeSetSentinel and
				   pNewNode is being attached/inserted as a child of it;
				3) we have a ptr to an undoable op;

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected
********************************************************************************************/

BOOL BaseCamelotFilter::AttachNode(Node* pNewNode, Node* pContextNode,
								   AttachNodeDirection Direction)
{
	// JCF: further bodged this already 'orrible bodge to allow undoable attaching of
	// child attributes of NodeSetSentinel.
	if (IsImporting() && ImportInfo.pOp != 0)
	{
		// Insert a layer with undo.
		if (pNewNode->IsLayer())
			return ImportInfo.pOp->DoInsertNewNode((Layer*) pNewNode, pContextNode,
												   Direction, FALSE);

		// If the context is the sentinel as parent then create a hide action.
		else if (IS_A(pContextNode, NodeSetSentinel) &&
					(Direction == FIRSTCHILD || Direction == LASTCHILD))
		{
			// Attach and create an action to hide the new child when we undo.
			HideNodeAction* pHideAct;
			pNewNode->AttachNode(pContextNode, Direction);
			if (AC_FAIL == HideNodeAction::Init(ImportInfo.pOp,
												ImportInfo.pOp->GetUndoActions(),
												pNewNode, TRUE, (Action**) &pHideAct))
			{
				// Tidy up on fail.
				pNewNode->UnlinkNodeFromTree();
				return FALSE;
			}

			return TRUE;
		}		
	}

	pNewNode->AttachNode(pContextNode, Direction);
	return TRUE;
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::DeleteNode(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		pNode = ptr to node to remove from the tree
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	deletes the given node by removing it from the tree

				This will delete the node undoably when it has to.

				At the mo, deletion is only performed undoably when all the following conditions are met:
				1) We are importing, and not openning the file
				2) pNewNode is a layer
				3) we have a ptr to an undoable op

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::DeleteNode(Node* pNode)
{
	BOOL ok = TRUE;

	BOOL DeleteWithUndo = (IsImporting() && pNode->IsLayer() && (ImportInfo.pOp != NULL));

	if (DeleteWithUndo)
		ok = ImportInfo.pOp->DoHideNode(pNode,TRUE);
	else
	{
		pNode->CascadeDelete();
		delete pNode;
	}

	return ok;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::PrepareTreeBuilding()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	This does the setting up of the tree building functions (InsertNode(), etc).

				It must be called before any node is added to the tree.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::PrepareTreeBuilding()
{
	// If there is a level stack delete it (this should never be needed)
	if (pInsertLevelStack != NULL)
	{
		ERROR3("pInsertLevelStack != NULL - has CleanUpTreeBuilding been called?");
		delete pInsertLevelStack;
	}

	// Create a level stack
	pInsertLevelStack = new InsertLevelStack;
	if (pInsertLevelStack == NULL)
		return FALSE;

	// Add a level item to get us going
	InsertLevelStackItem* pItem = new InsertLevelStackItem(this);
	if (pItem == NULL)
		return FALSE;

	pInsertLevelStack->Add(pItem);

	pInsertContextNode = NULL;

	ERROR2IF(TheDocument == NULL,FALSE,"TheDocument is NULL!");

	pLastUnsafeToRenderNode = NULL;
	TheDocument->GetSafeRenderPointer().SetPointerValid();
	TheDocument->GetSafeRenderPointer().UpdateLastSafeNode(NULL);

	Chapter* pChapter = Node::FindFirstChapter(TheDocument);
	if (pChapter != NULL)
	{
		Spread* pSpread = pChapter->FindFirstSpread();
		if (pSpread != NULL)
		{
			Layer* pLayer = pSpread->FindLastLayer();

			if (pLayer != NULL)
				pInsertContextNode = pLayer;
			else
				pInsertContextNode = pSpread;

			InsertNextNodeAsChild();
		}
	}

	ERROR2IF(pInsertContextNode == NULL,FALSE,"Can't find last layer on the spread");

	if (!IsImporting() && pInsertContextNode->IsSpread())
	{
		Layer* pLayer = new Layer;
		if (pLayer == NULL)
			return FALSE;

		if (!AttachNode(pLayer,pInsertContextNode,LASTCHILD))
			return FALSE;

		String_256 LayerName;
#ifdef WEBSTER
		// Make up a unique layer name of the form 'Frame 1'
		String_256 FrameName(_R(IDS_DEFAULTFRAMENAME));
		LayerName.MakeMsg(_R(IDS_FRAMENAMEUNIQUEFORM), (TCHAR *)FrameName, 1);
#else		
		// Make up a unique layer name of the form 'Layer 1'
		LayerName.MakeMsg(_R(IDS_SGLAYER_LAYER_NUM),1);
#endif
		pLayer->SetLayerID(LayerName);
		pLayer->EnsureUniqueLayerID();

		pInsertContextNode = pLayer;
		InsertNextNodeAsChild();
	}

	if (IsImporting() && pInsertContextNode->IsLayer())
	{
		Layer* pLayer = new Layer;
		if (pLayer == NULL)
			return FALSE;

		if (!AttachNode(pLayer,pInsertContextNode,NEXT))
			return FALSE;

		String_256 LayerName(_R(IDS_K_EPSFILTER_IMPORTED));
#ifdef WEBSTER
		String_256 Suffix(_R(IDS_DEFAULTFRAMENAME));	// Frame
#else
		String_256 Suffix(_R(IDS_LAYER_DESCRS));		// layer
#endif
		LayerName += Suffix;
		pLayer->SetLayerID(LayerName);
		pLayer->EnsureUniqueLayerID();

		pImportLayer = pLayer;		// The layer created just for importing purposes

		pInsertContextNode = pLayer;
		InsertNextNodeAsChild();
	}


	return TRUE;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::CleanUpTreeBuilding()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	-
	Purpose:	This deinits the tree building system

				It must be called sometime after the last node is added to the tree.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::CleanUpTreeBuilding()
{
	// Reset the level vars
	if (pInsertLevelStack != NULL)
	{
		delete pInsertLevelStack;
		pInsertLevelStack = NULL;
	}

	if (TheDocument != NULL)
		TheDocument->GetSafeRenderPointer().SetPointerInValid();
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::IncInsertLevel()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Tells the tree building system that the next nodes should be added to the next 
				level of the tree.

				This is called when a TAG_DOWN record is encountered

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::IncInsertLevel()
{
	ERROR2IF(pInsertLevelStack == NULL,FALSE,"pInsertLevelStack == NULL");

	// Find the info item containing data on the current level
	InsertLevelStackItem* pCurrentLevelItem = pInsertLevelStack->Get();
	// Inform the handler that we are about to begin a subtree
	if (pCurrentLevelItem != NULL)
	{
		CXaraFileRecordHandler* pHandler = pCurrentLevelItem->GetLastHandler();
		if (pHandler != NULL)
			pHandler->BeginSubtree(pCurrentLevelItem->GetLastHandledTag());
		
	}
	
	InsertLevelStackItem* pItem = new InsertLevelStackItem(this);
	if (pItem != NULL)
	{
		pInsertLevelStack->Add(pItem);
		InsertNextNodeAsChild();

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::DecInsertLevel()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/96
	Inputs:		-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Tells the tree building system that the next nodes should be added to the previous
				level of the tree.

				This is called when a TAG_UP record is encountered

				This also calls the node last inserted on the new current level, to inform it
				that its sub-tree is complete.  This gives complex nodes (such as moulds) as chance to
				do any initialisation after importing. (See Node::ReadPostChildrenWeb())

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::DecInsertLevel()
{
	ERROR2IF(pInsertLevelStack == NULL,FALSE,"pInsertLevelStack == NULL");

	// Remove the last level item, as we've moved up a level
	InsertLevelStackItem* pItem = pInsertLevelStack->Pop();

	ERROR3IF(pItem == NULL,"no tail item");

	if (pItem != NULL)
	{
		// We've just gone up a level, which inplies that the last inserted node on the last
		// level has a complete child subtree, so inform it of the fact
		if (!pItem->InformLastInsertedNodeChildrenComplete())
			return FALSE;

		UpdateLastSafeToRenderNode(pItem->GetLastInsertedNode());

		// If there were some nodes inserted on the last level (i.e. the insert count > 0), we
		// need to reset the insert context node.
		// In this case the new context node is the parent of the current context node

		if (pItem->GetNumNodesInserted() > 0)
		{
			Node* pContextNode = GetInsertContextNode();
			if (pContextNode == NULL)
				return FALSE;

			SetInsertContextNode(pContextNode->FindParent());
		}

		delete pItem;
	}

	// Insert the next node as a sibling to the current context node
	InsertNextNodeAsSibling();

	BOOL ok = TRUE;

	// Find the info item containing data on the current level
	InsertLevelStackItem* pCurrentLevelItem = pInsertLevelStack->Get();

	if (pCurrentLevelItem != NULL)
	{
		// We've just gone up a level, which inplies that the last inserted node on the new current
		// level has a complete child subtree, so inform it of the fact
		if (!pCurrentLevelItem->InformLastInsertedNodeChildrenComplete())
			return FALSE;

		UpdateLastSafeToRenderNode(pCurrentLevelItem->GetLastInsertedNode());

		// Inform the handler that we have finished a subtree
		CXaraFileRecordHandler* pHandler = pCurrentLevelItem->GetLastHandler();
		if (pHandler != NULL)
			pHandler->EndSubtree(pCurrentLevelItem->GetLastHandledTag());
		
	}

	return ok;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

/********************************************************************************************

>	virtual void BaseCamelotFilter::AddAtomicTag(AtomicTagListItem* pItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pItem = ptr to an atomic list item
	Returns:	-
	Purpose:	Adds the item to the list of atomic tags compiled during import

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::AddAtomicTag(AtomicTagListItem* pItem)
{
	if (pAtomicTagList == NULL)
		pAtomicTagList = new AtomicTagList;

	if (pAtomicTagList != NULL)
		pAtomicTagList->AddTail(pItem);
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::AddEssentialTag(EssentialTagListItem* pItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pItem = ptr to an Essential list item
	Returns:	-
	Purpose:	Adds the item to the list of Essential tags compiled during import

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::AddEssentialTag(EssentialTagListItem* pItem)
{
	if (pEssentialTagList == NULL)
		pEssentialTagList = new EssentialTagList;

	if (pEssentialTagList != NULL)
		pEssentialTagList->AddTail(pItem);
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::AddTagDescription(TagDescriptionListItem* pItem)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pItem = ptr to an Essential list item (if NULL it is just ignored_
	Returns:	TRUE if added, FALSE if not
	Purpose:	Adds the item to the list of tag descriptions compiled during import or export

				WEBSTER - markn 11/2/97
				Doesn't add a description if it's already there, and returns FALSE in this case.
				Also it copes nicely with a null pItem ptr

				Graeme (8-2-00) - Changed Mark's code so that repeated tags don't return
				FALSE, but are simply ignored. This fixes a problem that they're causing with
				some WebStyle templates.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::AddTagDescription(TagDescriptionListItem* pItem)
{
	// WEBSTER - markn 11/2/96
	// Check to see if a description for this tag has already been added
	if ( pItem != NULL )
	{
		if (pTagDescriptionList == NULL)
			pTagDescriptionList = new TagDescriptionList;

		if (pTagDescriptionList != NULL)
		{
			// Graeme (8-2-00) - Moved the GetTagDescription line here, so that items are
			// only added if they're not a repeated tag. This should maintain compatability,
			// and prevent the errors being thrown as before.
			if ( GetTagDescription ( pItem->GetTag () ) == NULL )
			{
				pTagDescriptionList->AddTail(pItem);
			}
			else
			{
				// MRH 10/5/00 - If we`re skipping it then we should be deleting it !!!
				delete pItem;
				pItem = NULL;
			}

			return TRUE;
		}
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL BaseCamelotFilter::SetDocumentNudgeSize(UINT32 newVal)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/200
	Inputs:		newVal - the new value for the nudge size
	Returns:	TRUE
	Purpose:	Sets the nudge size for the current document.

	SeeAlso:	Document::SetDocNudge
	Scope: 		Private

********************************************************************************************/

BOOL BaseCamelotFilter::SetDocumentNudgeSize(UINT32 newVal)
{
	return (TheDocument->SetDocNudge (newVal));
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::AddTagDescription(UINT32 Tag, UINT32 ID)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/97
	Inputs:		Tag = tag of the record you're describing
				ID = string ID of the resourced description text
	Returns:	-
	Purpose:	Adds the description for the given tag export

				WEBSTER - markn 11/2/97

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::AddTagDescription(UINT32 Tag, UINT32 ID)
{
	String_256* pDesc = new String_256(ID);
	if (pDesc != NULL)
	{
		TagDescriptionListItem* pItem = new TagDescriptionListItem(Tag,pDesc);
		if (pItem != NULL)
		{
			if (!AddTagDescription(pItem))
				delete pItem;
		}
	}
}


/********************************************************************************************

>	virtual void BaseCamelotFilter::WriteTagDescriptionRecord()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/97
	Inputs:		-
	Returns:	-
	Purpose:	WEBSTER - markn 11/2/97

				Writes out the tag description record for any descriptions registered during export

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::WriteTagDescriptionRecord()
{
	// Have we got a list
	if (pTagDescriptionList != NULL)
	{
		// Create a description list record
		CXaraFileRecord Rec(TAG_TAGDESCRIPTION,TAG_TAGDESCRIPTION_SIZE);

		BOOL ok = Rec.Init();

		UINT32 Count = pTagDescriptionList->GetCount();

		// First field is the number of tag descriptions in the file
		if (ok) ok = Rec.WriteUINT32(Count);

		// Write all the tag descriptions to the record
		TagDescriptionListItem* pItem = pTagDescriptionList->GetHead();
		while (pItem != NULL && ok && Count > 0)
		{
			String_256* pStr = pItem->GetString();
			if (ok) ok = (pStr != NULL);

			if (ok) ok = Rec.WriteUINT32(pItem->GetTag());
			if (ok) ok = Rec.WriteUnicode(*pStr);

			pItem = pTagDescriptionList->GetNext(pItem);
			Count--;
		}

		if (Count != 0)
		{
			ok = FALSE;
			ERROR3("inconsistancy between the count & the actual number of items in the list");
		}

		if (ok) ok = Write(&Rec);
	}
}



/********************************************************************************************

>	BOOL BaseCamelotFilter::WriteNudgeSizeRecord ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/2000
	Inputs:		-
	Returns:	-
	Purpose:	Writes out the documents nudge size

	SeeAlso:	-
	Scope: 		-

********************************************************************************************/

BOOL BaseCamelotFilter::WriteNudgeSizeRecord ()
{
	// Create a description list record
	CXaraFileRecord Rec(TAG_DOCUMENTNUDGE,TAG_DOCUMENTNUDGESIZE);

	BOOL ok = Rec.Init();

	// Write the date to the file ....
	if (ok) ok = Rec.WriteINT32 ((INT32) TheDocument->GetDocNudge ());

	if (ok) ok = Write(&Rec);

	return (ok);
}


/********************************************************************************************

>	BOOL BaseCamelotFilter::WriteBitmapSmoothingRecord ()

	Author:		Andy_Hills (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/11/2000
	Inputs:		-
	Returns:	-
	Purpose:	Writes out the document's bitmap smoothing setting.
				The first byte is a flags byte.
					LSB			bitmap smoothing flag
					bits 2-8	reserved
				There are 4 reserved bytes for future use (other bitmap settings).
	SeeAlso:	-
	Scope: 		-

********************************************************************************************/

BOOL BaseCamelotFilter::WriteBitmapSmoothingRecord ()
{
	BOOL ok = TRUE;

	// create a new record
	CXaraFileRecord Rec(TAG_DOCUMENTBITMAPSMOOTHING,TAG_DOCUMENTBITMAPSMOOTHINGSIZE);

	// populate the record
	BYTE Flags = 0x00;
	if (TheDocument->GetBitmapSmoothing()) Flags |= 0x01;

	ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(Flags);	// flags
	
	// reserved bytes
	for( INT32 i=0; i<4; i++ )
	{
		if (ok) ok = Rec.WriteBYTE(0);
	}

	// Write the record
	if (ok) ok = Write(&Rec);

	return ok;
}


/********************************************************************************************

>	BOOL BaseCamelotFilter::WriteDuplicationOffsetRecord()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/04/2004
	Inputs:		-
	Returns:	-
	Purpose:	Writes out the documents duplication offset

	SeeAlso:	-
	Scope: 		-

********************************************************************************************/

BOOL BaseCamelotFilter::WriteDuplicationOffsetRecord()
{
	// Create a description list record
	CXaraFileRecord Rec(TAG_DUPLICATIONOFFSET, TAG_DUPLICATIONOFFSET_SIZE);

	BOOL ok = Rec.Init();

	// Write the date to the file ....
	if (ok) ok = Rec.WriteINT32(TheDocument->GetDuplicationOffset().x);
	if (ok) ok = Rec.WriteINT32(TheDocument->GetDuplicationOffset().y);

	if (ok) ok = Write(&Rec);

	return (ok);
}


/********************************************************************************************

>	UINT32 BaseCamelotFilter::WriteXPEBitmapPlaceHolder(String_256& BitmapName)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02-02-2004
	Inputs:		-
	Returns:	Record Number for this record.
	Purpose:	Writes out an XPE placeholder record to notify the loader to
				regenerate the bitmap from the XPE info in the bitmap properties.
	SeeAlso:	-
	Scope: 		-

********************************************************************************************/

UINT32 BaseCamelotFilter::WriteXPEBitmapPlaceHolder(String_256& BitmapName)
{
//	UINT32				rec = UINT32(-1);

	// create a new record
//	CXaraFileRecord Rec(TAG_DEFINEBITMAP_XPE, CXF_UNKNOWN_SIZE);
	CXaraFileRecord Rec(TAG_DEFINEBITMAP_XPE, 0);

	/*BOOL ok =*/ Rec.Init();

	// Write out the bitmap name
//	if (ok) Rec.WriteUnicode(BitmapName);

	// Write the record
	return Write(&Rec);
}


/********************************************************************************************

>	BOOL BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if changed value, FALSE otherwise
	Purpose:	All compound nodes (e.g.  bevels, contours, shadows, clipview) are now defined
				as being atomic.  This is so that they can be backwards compatible with CX2.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
{
	BOOL ok = TRUE;
	
	CXaraFileRecord atomicRec(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec.Init();
	if (ok) ok = atomicRec.WriteUINT32(TAG_BEVEL);						// NodeBevelController
	if (ok)	ok = Write(&atomicRec);										// And write out the record

	CXaraFileRecord atomicRec2(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec2.Init();
	if (ok) ok = atomicRec2.WriteUINT32(TAG_BEVELINK);					// NodeBevel
	if (ok)	ok = Write(&atomicRec2);

	CXaraFileRecord atomicRec3(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec3.Init();
	if (ok) ok = atomicRec3.WriteUINT32(TAG_CONTOURCONTROLLER);			// NodeContourController
	if (ok)	ok = Write(&atomicRec3);

	CXaraFileRecord atomicRec4(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec4.Init();
	if (ok) ok = atomicRec4.WriteUINT32(TAG_CONTOUR);					// NodeContour
	if (ok)	ok = Write(&atomicRec4);

	CXaraFileRecord atomicRec5(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec5.Init();
	if (ok) ok = atomicRec5.WriteUINT32(TAG_SHADOWCONTROLLER);			// NodeShadowController
	if (ok)	ok = Write(&atomicRec5);

	CXaraFileRecord atomicRec6(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec6.Init();
	if (ok) ok = atomicRec6.WriteUINT32(TAG_SHADOW);						// NodeShadow
	if (ok)	ok = Write(&atomicRec6);

	CXaraFileRecord atomicRec7(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec7.Init();
	if (ok) ok = atomicRec7.WriteUINT32(TAG_CLIPVIEWCONTROLLER);			// NodeClipViewController
	if (ok)	ok = Write(&atomicRec7);

	CXaraFileRecord atomicRec8(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec8.Init();
	if (ok) ok = atomicRec8.WriteUINT32(TAG_CLIPVIEW);					// NodeClipView
	if (ok)	ok = Write(&atomicRec8);

	CXaraFileRecord atomicRec9(TAG_ATOMICTAGS, TAG_ATOMICTAGS_SIZE);
	if (ok) ok = atomicRec9.Init();
	if (ok) ok = atomicRec9.WriteUINT32(TAG_CURRENTATTRIBUTES);			// Current Attributes container/component
	if (ok)	ok = Write(&atomicRec9);

	

	return (ok);
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::IsTagInAtomicList(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		Tag = tag value to look for
	Returns:	TRUE if found, FALSE otherwsie
	Purpose:	Searches the atomic tag list to see of the given tag is in the list.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::IsTagInAtomicList(UINT32 Tag)
{
	if (pAtomicTagList != NULL)
	{
		AtomicTagListItem* pItem = pAtomicTagList->GetHead();
		while (pItem != NULL)
		{
			if (pItem->GetTag() == Tag)
				return TRUE;

			pItem = pAtomicTagList->GetNext(pItem);
		}
	}

	return FALSE;
}

/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::IsTagInEssentialList(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		Tag = tag value to look for
	Returns:	TRUE if found, FALSE otherwsie
	Purpose:	Searches the Essential tag list to see of the given tag is in the list.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::IsTagInEssentialList(UINT32 Tag)
{
	if (pEssentialTagList != NULL)
	{
		EssentialTagListItem* pItem = pEssentialTagList->GetHead();
		while (pItem != NULL)
		{
			if (pItem->GetTag() == Tag)
				return TRUE;

			pItem = pEssentialTagList->GetNext(pItem);
		}
	}

	return FALSE;
}

/********************************************************************************************

>	virtual TagDescriptionListItem* BaseCamelotFilter::GetTagDescription(UINT32 Tag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		Tag = tag value to look for
	Returns:	ptr to the desc item, or NULL if not found.
	Purpose:	Searches the tag description list to see of the given tag is in the list.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

TagDescriptionListItem* BaseCamelotFilter::GetTagDescription(UINT32 Tag)
{
	if (pTagDescriptionList != NULL)
	{
		TagDescriptionListItem* pItem = pTagDescriptionList->GetHead();
		while (pItem != NULL)
		{
			if (pItem->GetTag() == Tag)
				return pItem;

			pItem = pTagDescriptionList->GetNext(pItem);
		}
	}

	return NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


/********************************************************************************************

>	void BaseCamelotFilter::AddPathRecordRefToList(NodePath* pNodePath, UINT32 RecordNumber)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		pNodePath	 = ptr to a node path object
				RecordNumber = the record number of the record that holds this path
	Returns:	-
	Purpose:	Adds a path record reference to the list

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::AddPathRecordRefToList(NodePath* pNodePath, UINT32 RecordNumber)
{
	ERROR3IF(pNodePath == NULL,"Null entry param");
	ERROR3IF(RecordNumber == 0,"Zero entry record number");

	if (pPathRecordRefList == NULL)
		pPathRecordRefList = new CXaraFilePathRecordRefList;

	if (pPathRecordRefList != NULL)
	{
		CXaraFilePathRecordRefListItem* pItem = new CXaraFilePathRecordRefListItem(pNodePath,RecordNumber);
		if (pItem != NULL)
			pPathRecordRefList->AddTail(pItem);
	}
}


/********************************************************************************************

>	UINT32 BaseCamelotFilter::FindPathRecordRefRecordNumber(NodePath* pNodePath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		pNodePath	 = ptr to a node path object
	Returns:	RecordNumber = the record number of the record that holds this path
				0 is returned if path could not be found
	Purpose:	Finds the record number for the given path

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

UINT32 BaseCamelotFilter::FindPathRecordRefRecordNumber(NodePath* pNodePath)
{
	ERROR3IF(pNodePath == NULL,"Null entry param");

	if (pNodePath != NULL)
	{
		if (pPathRecordRefList != NULL)
		{
			CXaraFilePathRecordRefListItem* pItem = pPathRecordRefList->GetHead();
			while (pItem != NULL)
			{
				if (pItem->GetNodePath() == pNodePath)
					return pItem->GetRecordNumber();

				pItem = pPathRecordRefList->GetNext(pItem);
			}
		}
	}

	return 0;
}


/********************************************************************************************

>	NodePath* BaseCamelotFilter::FindPathRecordRefPath(UINT32 RecordNumber)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		RecordNumber = the record number of the record that holds this path
	Returns:	pNodePath	 = ptr to a node path object
				NULL is returned if path could not be found
	Purpose:	Finds the path for the given record number

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

NodePath* BaseCamelotFilter::FindPathRecordRefPath(UINT32 RecordNumber)
{
	ERROR3IF(RecordNumber == 0,"Zero entry record number");

	if (RecordNumber != 0)
	{
		if (pPathRecordRefList != NULL)
		{
			CXaraFilePathRecordRefListItem* pItem = pPathRecordRefList->GetHead();
			while (pItem != NULL)
			{
				if (pItem->GetRecordNumber() == RecordNumber)
					return pItem->GetNodePath();

				pItem = pPathRecordRefList->GetNext(pItem);
			}
		}
	}

	return NULL;
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::FindSimilarPath(NodePath* pNodePath,UINT32* pOtherPathRecNum,Matrix* pTransform)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/9/96
	Inputs:		pNodePath		= ptr to path to match
				pOtherPathRecNum= ptr to place the record number of a similar path, if one's found
				pTransform		= ptr to place transformation matrix
	Returns:	TRUE
	Purpose:	Finds a similar path to the one provided that has previously been output to the
				file.

				It scans the list of previously outputted paths

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::FindSimilarPath(NodePath* pNodePath,UINT32* pOtherPathRecNum,Matrix* pTransform)
{
	ERROR2IF(pNodePath == NULL,FALSE,"NULL src path ptr");
	ERROR2IF(pOtherPathRecNum == NULL,FALSE,"NULL other rec num ptr");
	ERROR2IF(pTransform == NULL,FALSE,"NULL matrix ptr");

	BOOL Found = FALSE;
	INT32 PathCount = GetMaxPathLookUp();

	if (pPathRecordRefList != NULL)
	{
		CXaraFilePathRecordRefListItem* pItem = pPathRecordRefList->GetTail();
		while (pItem != NULL && !Found && PathCount != 0)
		{
			PathCount--;

			NodePath* pOtherNodePath = pItem->GetNodePath();

			if (pOtherNodePath != NULL)
			{
				// Is the given path the similar to the other path?
				// First, do a fast check - if the num coords is different, then forget it.
				if (pNodePath->InkPath.GetNumCoords() == pOtherNodePath->InkPath.GetNumCoords())
				{
					// We need the coord origin
					DocCoord Origin = GetCoordOrigin();

					// Translate the two paths by the coord origin.
					// This turns the path coords into the form in which they appear in the file
					//
					// Remember, all absolute coords of objects (such as path coords) get translated before
					// being output to the file, so that the coord's origin is at the page origin, rather
					// than the spread origin.  This allows us to import the same file into a different doc
					// and still position objects that are the same relative distance from the new page origin
					//
					// The following translation turns the path's coord values into the exact values that
					// appear in the file.  This is important if IsIsometric() creates a transform for us
					// to use.  The transform has to be valid for the path that ends up in the file, and not
					// the one in the document, because that is the path that's going to be read during import.
					
					{
						Matrix			TranslateMat(-Origin.x,-Origin.y);
						Trans2DMatrix	Trans(TranslateMat);
						pNodePath->Transform(Trans);
						pOtherNodePath->Transform(Trans);
					}

					// Now, can the path be described using the other path and a transform?
					Found = pOtherNodePath->InkPath.IsIsometric(pNodePath->InkPath,pTransform,GetSimilarPathTolerance(pNodePath));
					if (Found)
					{
						// If so, make a note of the record number which contains the source path data
						*pOtherPathRecNum = pItem->GetRecordNumber();
					}

					// Translate the paths back to the original position
					{
						Matrix			TranslateMat(Origin.x,Origin.y);
						Trans2DMatrix	Trans(TranslateMat);
						pNodePath->Transform(Trans);
						pOtherNodePath->Transform(Trans);
					}
				}
			}

			pItem = pPathRecordRefList->GetPrev(pItem);
		}
	}

	return Found;
}

/********************************************************************************************

>	INT32 BaseCamelotFilter::GetMaxPathLookUp()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/96
	Inputs:		-
	Returns:	-1 always
	Purpose:	The max number pf paths that should be looked at by the function FindSimilarPath().

				-1 means no limit

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

INT32 BaseCamelotFilter::GetMaxPathLookUp()
{
	return -1;
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

/********************************************************************************************

>	void BaseCamelotFilter::SetImportFileType(char* pFileType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/96
	Inputs:		-
	Returns:	-
	Purpose:	Sets the file type of the imported file.

				Only the first 3 chars are read, so don't worry about zero-terminating the string
				before calling this func.

				This function should be called when the file header record is read, because
				that is where the file type is stored.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::SetImportFileType(char* pFileType)
{
	ImportFileType[0] = 0;

	if (pFileType != NULL)
	{
		strncpy(ImportFileType,pFileType,3);
		ImportFileType[3] = 0;
	}
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::IsOpeningMinimalWebFormat()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/9/96
	Inputs:		-
	Returns:	TRUE if we are opening the minimal format, FALSE otherwise
	Purpose:	It will only return TRUE if the file is the minimal web format, and we are OPENING
				the file and NOT importing it into a new document.

				You should only call this after the file header record has been read in, because
				the import file type is only set when this record is read in.

				As the file header is the first record in the file, record handlers should have no problems
				using this function.

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

BOOL BaseCamelotFilter::IsOpeningMinimalWebFormat()
{
	return (!IsImporting() && (strcmp(ImportFileType,EXPORT_FILETYPE_MIN) == 0));
}

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

/********************************************************************************************

>	void BaseCamelotFilter::AddTextStoryGroupRefToList(TextStory* pStory,NodeGroup* pGroup)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		pStory = ptr to a text story
				pGroup = ptr to a group of paths that represent the story, which is not linked into the tree
	Returns:	-
	Purpose:	Adds a text story group reference to the list

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
void BaseCamelotFilter::AddTextStoryGroupRefToList(TextStory* pStory,NodeGroup* pGroup)
{
	ERROR3IF(pStory == NULL,"Null text story entry param");
	ERROR3IF(pGroup == NULL,"Null node group entry param");

	if (pTextStoryGroupRefList == NULL)
		pTextStoryGroupRefList = new CXaraFileTextStoryGroupRefList;

	if (pTextStoryGroupRefList != NULL)
	{
		CXaraFileTextStoryGroupRefListItem* pItem = new CXaraFileTextStoryGroupRefListItem(pStory,pGroup);
		if (pItem != NULL)
			pTextStoryGroupRefList->AddTail(pItem);
	}
}
*/

/********************************************************************************************

>	NodeGroup* BaseCamelotFilter::FindTextStoryGroupRefGroup(TextStory* pStory)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		pStory = ptr to a text story
	Returns:	ptr to a group that's associated with the text story 
				NULL is returned if the group could not be found
	Purpose:	Finds the group associated with the text story

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
NodeGroup* BaseCamelotFilter::FindTextStoryGroupRefGroup(TextStory* pStory)
{
	ERROR3IF(pStory == NULL,"Null pStory entry param");

	if (pStory != NULL)
	{
		if (pTextStoryGroupRefList != NULL)
		{
			CXaraFileTextStoryGroupRefListItem* pItem = pTextStoryGroupRefList->GetHead();
			while (pItem != NULL)
			{
				if (pItem->GetTextStory() == pStory)
					return pItem->GetNodeGroup();

				pItem = pTextStoryGroupRefList->GetNext(pItem);
			}
		}
	}

	return NULL;
}
*/

/********************************************************************************************

>	TextStory* BaseCamelotFilter::FindTextStoryGroupRefTextStory(NodeGroup* pGroup)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/96
	Inputs:		pGroup = ptr to a group that's associated with the text story 
	Returns:	ptr to a text story
				NULL is returned if the text story could not be found
	Purpose:	Finds the text story associated with the group

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

// WEBSTER - markn 31/1/97
// Replaced with general system
/*
TextStory* BaseCamelotFilter::FindTextStoryGroupRefTextStory(NodeGroup* pGroup)
{
	ERROR3("Function not implemented");
	return NULL;
}
*/

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------

/********************************************************************************************

>	void BaseCamelotFilter::AddNodeGroupRefToList(Node* pStory,NodeGroup* pGroup)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		pNode  = ptr to a node in the tree
				pGroup = ptr to a group of paths that represent the node, which is not linked into the tree
	Returns:	-
	Purpose:	Adds a node/group reference to the list

				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

void BaseCamelotFilter::AddNodeGroupRefToList(Node* pNode,NodeGroup* pGroup)
{
	ERROR3IF(pNode  == NULL,"Null node entry param");
	ERROR3IF(pGroup == NULL,"Null node group entry param");

	if (pNodeGroupRefList == NULL)
		pNodeGroupRefList = new CXaraFileNodeGroupRefList;

	if (pNodeGroupRefList != NULL)
	{
		CXaraFileNodeGroupRefListItem* pItem = new CXaraFileNodeGroupRefListItem(pNode,pGroup);
		if (pItem != NULL)
			pNodeGroupRefList->AddTail(pItem);
	}
}


/********************************************************************************************

>	NodeGroup* BaseCamelotFilter::FindGroupForThisNode(Node* pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		pNode = ptr to a node
	Returns:	ptr to a group that's associated with the node
				NULL is returned if the group could not be found
	Purpose:	Finds the group associated with the node

				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

	SeeAlso:	BaseCamelotFilter::PrepareToImport; BaseCamelotFilter::DoImport
	Scope: 		Protected

********************************************************************************************/

NodeGroup* BaseCamelotFilter::FindGroupForThisNode(Node* pNode)
{
	ERROR3IF(pNode == NULL,"Null pNode entry param");

	if (pNode != NULL)
	{
		if (pNodeGroupRefList != NULL)
		{
			CXaraFileNodeGroupRefListItem* pItem = pNodeGroupRefList->GetHead();
			while (pItem != NULL)
			{
				if (pItem->GetNode() == pNode)
					return pItem->GetNodeGroup();

				pItem = pNodeGroupRefList->GetNext(pItem);
			}
		}
	}

	return NULL;
}

/********************************************************************************************

>	virtual void BaseCamelotFilter::ExportHTMLTag(PathName* ppthToUse)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/97
	Inputs:		-
	Returns:	-
	Purpose:	Exports an HTML tag to the clipboard that can be
				used in a page of HTML to embed the file that is
				being exported.
	
********************************************************************************************/

void BaseCamelotFilter::ExportHTMLTag(PathName* ppthToUse)
{
	PORTNOTETRACE("other","BaseCamelotFilter::ExportHTMLTag - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//Check our parameter
	if (ppthToUse==NULL)
	{
		ERROR2RAW("BaseCamelotFilter::ExportHTMLTag - NULL parameter");
		return;
	}

	//First get the name of the file we are exporting to
	String_256 strFileName=ppthToUse->GetFileName(TRUE);

	//Now we need to get the width and height of the document we are exporting,
	//scaled by 96 DPI

	//So first find out what are we are exporting
	SelectionType stExportArea=GetSelType();

	//And get the DocRect enclosing that area, using this useful static function
	DocRect rectExportArea=ImagemapFilterOptions::GetSizeOfExportArea(stExportArea);

	//Scale it to 96 DPI
	WinRect	wrectExportArea = OSRenderRegion::BitmapDocRectToWin(Matrix(), rectExportArea, 96);

	//Now, put all these values into our formatting string
	String_256			strTag;
	strTag.MakeMsg( _R(IDS_WEBFILTER_HTMLTAG), &strFileName, wrectExportArea.GetWidth(), 
		wrectExportArea.GetHeight() );

	//And put that string on the clipboard
	InternalClipboard::CopyText(strTag);
#endif
}

/********************************************************************************************

>	BOOL BaseCamelotFilter::WriteNodeAsOutlines(Node *pNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/1/97
	Inputs:		pNode  = the node to convert to paths
	Returns:	TRUE if one or more records were written, FALSE otherwise
	Purpose:	Converts the node into a group of paths, and then outputs that group
				to the filter

  				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5

	Errors:		Errors via pFilter->GotError() if if fails to create the group of paths properly.

********************************************************************************************/

BOOL BaseCamelotFilter::WriteNodeAsOutlines(Node *pNode)
{
#ifdef DO_EXPORT
	ERROR2IF(pNode==NULL, FALSE, "Parameter pFilter == NULL.");

	BOOL ok = TRUE;

	NodeGroup* pGroup = FindGroupForThisNode(pNode);

	if (pGroup == NULL)
	{
		// Create a group to hold all the outlines
		pGroup = new NodeGroup;
		ok = ok && (pGroup != NULL);

		// Create a BecomeA object that will receive all the outlines and attach them as children
		// of the group
		NodeToOutlinesBecomeA* pBecomeA = new NodeToOutlinesBecomeA(pGroup);
		ok = ok && (pBecomeA != NULL);

		// Collect all the outlines
		if (ok) ok = pNode->DoBecomeA(pBecomeA);

		// This code factors out the unnecessary attributes by placing the group in the tree, normalising
		// the attrs for all the paths in the group, then removing the group from the tree again
		if (ok)
		{
			// Attach it next to the text story
			pGroup->AttachNode(pNode,NEXT);

			// Ask all the child objects in the group to normalise the attributes
			Node* pNode = pGroup->FindFirstChild();
			while (pNode != NULL)
			{
				if (pNode->IsAnObject())
					((NodeRenderableInk*)pNode)->NormaliseAttributes();
				pNode = pNode->FindNext();
			}

			// Unlink the group from the tree
			pGroup->UnlinkNodeFromTree();
		}

		// Add it to the list of Node/groups ref list items
		if (ok) AddNodeGroupRefToList(pNode,pGroup);

		// Tidy up
		if (pBecomeA != NULL)
		{
			delete pBecomeA;
			pBecomeA = NULL;
		}
	}

	// Now write out the group to the file
	if (ok) ok = WriteNodes(pGroup);

	// Report a general error if necessary
	if (!ok)
		GotError(_R(IDS_EXPORT_ERROR_TEXTTOOUTLINE));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteSelectedNodes(Operation * pExportOp, 
														Node *pNode, 
														SelRange * pSelection)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/01
	Inputs:		pExportOp	The operation that caused the export.
				pNode		The node to be written
				pSelection	The selection to be exported
	Returns:	TRUE if the node was exported properly, FALSE otherwise
	Purpose:	Exports the selection and any additional nodes required, starting from
				pNode.

				Notes: Known things this will not cope with (at the moment):

				Individual text characters selected inside a TextStory, without the story
				itself being selected, will cause the whole story to be exported, not just
				the selected characters. This is because of the way that TextChars are
				written. i.e. to avoid unnecessary information, when a text character is
				written out, it also writes out any following characters (upto the first non
				TextChar).

	SeeAlso:	BaseCamelotFilter::WriteNodeAndSubNodes, BaseCamelotFilter::WriteSelectedLayerAndNodes.

********************************************************************************************/
BOOL BaseCamelotFilter::WriteSelectedNodes (Operation * pExportOp, Node * pNode, SelRange * pSelection)
{
	// Start writing out the node passed in.
	BOOL ok	= WritePreChildren(pNode);
	if (ok)
		ok = WriteBeginChildRecords(pNode);

	// Attempt to export all of this node's children
	Node * pChildNode = pNode->FindFirstChild ();
	while ((ok) && (pChildNode != NULL) && (!EscapePressed))
	{
		// Export all layers with selected objects in them, and all non-layers
		if (pChildNode->IsLayer ())
		{
			// This would probably be better done using Node::HasSelectedChildren (in place 
			//	of the while loop and selected flag), as this does EXACTLY what we intend 
			//	the loop to do but very probably does it VASTLY quicker. Still, this loop will
			//	only be done once per layer, so it shouldn't make that much of a difference
			//	to the overall time
			//
			// Export the layer and it's selected children, if it has any (selected children
			//	that is).
			BOOL selected = FALSE;
			Node * pSelNode = pSelection->FindFirst ();
			while ((pSelNode != NULL) &&
					(selected == FALSE ))
			{
				if (pChildNode->IsNodeInSubtree (pSelNode))
					selected = TRUE;

				pSelNode = pSelection->FindNext (pSelNode);
			}

			// This layer has something selected in it, so export it.
			if (selected)
			{
				ok = WriteSelectedLayerAndNodes (pExportOp, pChildNode, pSelection);
			}
		}
		else if (pChildNode->IsNodeHidden () == FALSE)
		{
			ok = WriteSelectedNodes (pExportOp, pChildNode, pSelection);
		}

		// move onto the next child of the node passed in.
		pChildNode = pChildNode->FindNext ();
	}

	// finish of the original node.
	if (ok)
		ok = WriteEndChildRecords(pNode);
	if (ok)
		ok = WritePostChildren(pNode);

	return ok;
}


/********************************************************************************************

>	virtual BOOL BaseCamelotFilter::WriteSelectedLayerAndNodes(Operation * pExportOp, 
																Node *pNode, 
																SelRange * pSelection)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/01
	Inputs:		pExportOp	The operation that caused the export.
				pNode		The node to be written
				pSelection	The selection to be exported
	Returns:	TRUE if the node was exported properly, FALSE otherwise
	Purpose:	Exports a layer node and everything selected below it (i.e. all it's selected 
				children) out through the filter.

				Theoretically pNode doesn't have to be a NodeLayer, but this was designed (and 
				named) to export layers and their selected sub-trees.

	SeeAlso:	BaseCamelotFilter::WriteNodeAndSubNodes, BaseCamelotFilter::WriteSelectedNodes.

********************************************************************************************/
BOOL BaseCamelotFilter::WriteSelectedLayerAndNodes (Operation * pExportOp, Node * pNode, SelRange * pSelection)
{
	// pNode should be a layer for this to work as intended, but it will survive if it isn't
	ERROR3IF ((pNode->IsLayer () != TRUE), "Attempted to export a non-layer node through BaseCamelotFilter::WriteSelectedLayerAndNodes");

	// Is the operation that we've been given one that we can use to manipulate the tree.
	BOOL validOperation;
	if ((pExportOp != NULL) && (pExportOp->IsKindOf (CC_RUNTIME_CLASS (UndoableOperation))))
		validOperation = TRUE;
	else
		validOperation = FALSE;

	ERROR3IF ((validOperation == FALSE), "BaseCamelotFilter::WriteSelectedLayerAndNodes, unusable operation given, continuing without localised attrbutes");

	// Start by writing out the layer beginning
	BOOL ok = WritePreChildren(pNode);
	if (ok)
		ok = WriteBeginChildRecords(pNode);


	// cycle through the selection looking for nodes in this layer.
	Node * pSelectedNode = pSelection->FindFirst ();
	TextStory * pLastStory = NULL;
	while ((ok) && (pSelectedNode != NULL) && (!EscapePressed))
	{
		if (pNode->IsNodeInSubtree (pSelectedNode))
		{
			Node * pExportNode = NULL;

			// If we've got a text character, then export the text story that controls this -
			//	if we haven't already.
			if (pSelectedNode->IsATextChar ())
			{
				TextStory * pParentStory = ((TextChar *) pSelectedNode)->FindParentStory();
				if (pParentStory != pLastStory)
				{
					pExportNode = (Node *) pParentStory;
					pLastStory = pParentStory;					
				}
				else
				{
					pExportNode = NULL;
				}
			}
			// otherwise export as normal
			else
			{
				pExportNode = pSelectedNode;
			}

			if (pExportNode)
			{
				// we've found one, so export it and all it's children.
				Node * pParent = pExportNode->FindParent ();

				if ((pParent->IsLayer () == FALSE) && (validOperation))
					((UndoableOperation *) pExportOp)->DoLocaliseCommonAttributes ((NodeRenderableInk *) pParent, FALSE, TRUE, NULL);
				
				ok = WriteNodeAndSubNodes (pExportNode);

				if ((pParent->IsLayer () == FALSE) && (validOperation))
					((UndoableOperation *) pExportOp)->DoFactorOutCommonChildAttributes ((NodeRenderableInk *) pParent, TRUE, NULL);
			}
		}

		// look at the next node.
		pSelectedNode = pSelection->FindNext (pSelectedNode);
	}

	// finish off writing the children.
	if (ok)
		ok = WriteEndChildRecords(pNode);
	if (ok)
		ok = WritePostChildren(pNode);

	return ok;
}




/********************************************************************************************

>	BOOL BaseCamelotFilter::WriteCurrentAttributes()

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/03/2004
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if worked, FALSE otherwise
	Purpose:	Write out all current attributes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL BaseCamelotFilter::WriteCurrentAttributes()
{
	return TRUE;
}




//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------

/********************************************************************************************

>	NodeToOutlinesBecomeA::~NodeToOutlinesBecomeA()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	The default destructor.

				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeToOutlinesBecomeA::~NodeToOutlinesBecomeA()
{
}

/********************************************************************************************

>	BOOL NodeToOutlinesBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/96
	Inputs:		pNewNode = the node created by the object being made into shapes
				pCreatedByNode = ptr to the node that created the new node
				pAttrMap = ptr to attrs (can be NULL)
	Outputs:	-
	Returns:	TRUE if the new path was used successfully, FALSE if op should be aborted
	Purpose:	This receives the paths and adds them to the group

  				WEBSTER - markn 29/1/97
				Part of the general form of the system used to convert text to outlines in v1.5
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL NodeToOutlinesBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
#ifdef DO_EXPORT
	ERROR2IF(pGroup == NULL,FALSE,"NULL group");
	ERROR2IF(pNewNode == NULL,FALSE,"NULL new path");

	// We only expect to get paths, so do a check
	if (!pNewNode->IsNodePath())
	{
		ERROR3("Received a node that's not a NodePath");
		return TRUE;
	}

	BOOL ok = TRUE;

	// Attach the next path as the last child of the group node
	pNewNode->AttachNode(pGroup,LASTCHILD);

	// Apply the supplied attrs, if there are any
	if (pAttrMap != NULL)
	{
		ok = pNewNode->ApplyAttributes(pAttrMap,TRUE);
		// Delete the attr map, and all it's attributes
		pAttrMap->DeleteAttributes();
		delete pAttrMap;
		pAttrMap = NULL;
	}
	else
	{
		// No supplied attrs, so we need to apply the applied attrs of the node that created the
		// path to the path itself, so that it will have the same appearence

		ok = FALSE;	// Assume failure (as we do when promised a bonus)

		// Create an attr map with the applied attrs of the "created by" node
		pAttrMap = CCAttrMap::MakeAppliedAttrMap(pCreatedByNode);
		if (pAttrMap != NULL)
		{
			ok = pNewNode->ApplyAttributes(pAttrMap,TRUE);		// Apply these attrs to the path
			// Delete the attr map
			delete pAttrMap;
			pAttrMap = NULL;
		}
	}

	return ok;
#else
	return FALSE;
#endif
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------



//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

UINT32* GeneralRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_ENDOFFILE,
								TAG_FILEHEADER,
								TAG_UP,
								TAG_DOWN,
								TAG_ATOMICTAGS,
								TAG_ESSENTIALTAGS,
								TAG_TAGDESCRIPTION,
								TAG_DOCUMENTNUDGE,
								CXFRH_TAG_LIST_END};
	return (UINT32*)&TagList;
}

BOOL GeneralRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_FILEHEADER :
			ok = HandleFileHeader(pCXaraFileRecord);
			break;

		case TAG_ENDOFFILE:
			EndOfFile();
			break;

		case TAG_UP:
			ok = DecInsertLevel();
			break;

		case TAG_DOWN:
			ok = IncInsertLevel();
			break;

		case TAG_ATOMICTAGS:
			ok = HandleAtomicTagsRecord(pCXaraFileRecord);
			break;

		case TAG_ESSENTIALTAGS:
			ok = HandleEssentialTagsRecord(pCXaraFileRecord);
			break;

		case TAG_TAGDESCRIPTION:
			ok = HandleTagDescriptionRecord(pCXaraFileRecord);
			break;

		case TAG_DOCUMENTNUDGE:
			ok = HandleTagNudeSizeRecord(pCXaraFileRecord);
			break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleFileHeader(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the file header record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleFileHeader(CXaraFileRecord* pCXaraFileRecord)
{
	BOOL ok = TRUE;
	BYTE Buffer[100];
	BYTE FileType[4];
	UINT32 n;
	UINT32 PreCompFlags;
	UINT32 FileSize;

	if (ok) ok = pCXaraFileRecord->ReadBuffer(FileType,3);			// File type (ensuring only 3 chars are read)
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&FileSize);			// File size
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&n);					// Native/Web link ID
	if (ok) ok = pCXaraFileRecord->ReadUINT32(&PreCompFlags);		// Precompression flags
	if (ok) ok = pCXaraFileRecord->ReadASCII((TCHAR*)Buffer,100);	// Producer
	if (ok) ok = pCXaraFileRecord->ReadASCII((TCHAR*)Buffer,100);	// Producer version
	if (ok) ok = pCXaraFileRecord->ReadASCII((TCHAR*)Buffer,100);	// Producer build

  	SetTotalProgressBarCount(FileSize);

	SetImportFileType((char*)FileType);

	ok = SetPreCompression(PreCompFlags);

	return ok;
}


/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleAtomicTagsRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the atomic tags record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleAtomicTagsRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_ATOMICTAGS,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32 Size = pCXaraFileRecord->GetSize();
	UINT32 Tag;

	// Create an atomic tag item for each atomic tag in the record

	for (UINT32 i = 0;ok && i < Size; i+=sizeof(UINT32))
	{
		AtomicTagListItem* pItem = NULL;

		ok = pCXaraFileRecord->ReadUINT32(&Tag);
		if (ok) pItem = new AtomicTagListItem(Tag);
		if (ok) ok = (pItem != NULL);
		if (ok) AddAtomicTag(pItem);
	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleEssentialTagsRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the essential tags record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleEssentialTagsRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_ESSENTIALTAGS,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32 Size = pCXaraFileRecord->GetSize();
	UINT32 Tag;

	// Create an essential tag item for each essential tag in the record

	for (UINT32 i = 0;ok && i < Size; i+=sizeof(UINT32))
	{
		EssentialTagListItem* pItem = NULL;

		ok = pCXaraFileRecord->ReadUINT32(&Tag);
		if (ok) pItem = new EssentialTagListItem(Tag);
		if (ok) ok = (pItem != NULL);
		if (ok) AddEssentialTag(pItem);
	}

	return ok;
}

/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleTagDescriptionRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the tag description record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleTagDescriptionRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_TAGDESCRIPTION,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	UINT32 NumTags;
	UINT32 Tag;
	String_256* pDesc = NULL;

	ok = pCXaraFileRecord->ReadUINT32(&NumTags);

	// Create a tag description item for each tag description in the record

	for (UINT32 i = 0;ok && i < NumTags; i++)
	{
		TagDescriptionListItem* pItem = NULL;

		pDesc = new String_256;
		ok = (pDesc != NULL);

		if (ok) ok = pCXaraFileRecord->ReadUINT32(&Tag);
		if (ok) ok = pCXaraFileRecord->ReadUnicode(pDesc);//*pDesc,pDesc->MaxLength());
		if (ok) pItem = new TagDescriptionListItem(Tag,pDesc);
		if (ok) ok = (pItem != NULL);
		if (ok) ok = AddTagDescription(pItem);

		if (!ok && pItem != NULL)
		{
			delete pItem;
			pItem = NULL;
		}
	}

	return ok;
}



/********************************************************************************************

>	BOOL GeneralRecordHandler::HandleTagNudeSizeRecord(CXaraFileRecord* pCXaraFileRecord)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/8/2000
	Inputs:		pCXaraFileRecord = ptr to a record
	Returns:	-
	Purpose:	Handles the nude (!) size description record
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL GeneralRecordHandler::HandleTagNudeSizeRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_DOCUMENTNUDGE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	INT32 val = (INT32) 2835;		//DEFAULT_NUDGE_SIZE;	I'm not moving this just so it will be consistent
	
	ok = pCXaraFileRecord->ReadINT32 (&val);

	ok = SetDocumentNudgeSize ((UINT32) val);

	return ok;
}

/********************************************************************************************

>	virtual void GeneralRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	The general record handler text dumper
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void GeneralRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	switch (pRecord->GetTag())
	{
		case TAG_FILEHEADER :
		{
			TCHAR FileType[4];
			FileType[3] = 0;

			char s[256];
			TCHAR Producer[100];
			TCHAR ProducerVersion[100];
			TCHAR ProducerBuild[100];
			UINT32 NativeWebLink;
			UINT32 PrecompressionFlags;
			UINT32 FileSize;

			pRecord->ReadBuffer((BYTE*)FileType,3);		// File type (ensuring only 3 chars are read)
			pRecord->ReadUINT32(&FileSize);				// File size
			pRecord->ReadUINT32(&NativeWebLink);			// Native/Web link ID
			pRecord->ReadUINT32(&PrecompressionFlags);	// Precompression flags
			pRecord->ReadASCII(Producer,100);			// Producer
			pRecord->ReadASCII(ProducerVersion,100);	// Producer version
			pRecord->ReadASCII(ProducerBuild,100);		// Producer build

			(*pStr) += "File Type\t\t\t: ";
			(*pStr) += FileType;
			(*pStr) += "\r\n";

			(*pStr) += "Uncompressed File Size\t: ";
			_stprintf(s,"%d",FileSize);
			(*pStr) += s;
			(*pStr) += "\r\n";

			(*pStr) += "Native/Web Link ID\t: ";
			_stprintf(s,"%d",NativeWebLink);
			(*pStr) += s;
			(*pStr) += "\r\n";

			(*pStr) += "Precompression Flags\t: ";
			_stprintf(s,"0x%x",PrecompressionFlags);
			(*pStr) += s;
			(*pStr) += "\r\n";

			(*pStr) += "Producer\t\t\t: ";
			(*pStr) += Producer;
			(*pStr) += "\r\n";

			(*pStr) += "Producer Version\t\t: ";
			(*pStr) += ProducerVersion;
			(*pStr) += "\r\n";

			(*pStr) += "Producer Build\t\t: ";
			(*pStr) += ProducerBuild;
			(*pStr) += "\r\n";
		}
		break;

		case TAG_ATOMICTAGS:
		{
			UINT32 Size = pRecord->GetSize();
			UINT32 Tag;
			String_256 TagText;

			(*pStr) += "Atomic tag list:\r\n";

			for (UINT32 i = 0;i < Size; i+=sizeof(UINT32))
			{
				pRecord->ReadUINT32(&Tag);
				GetTagText(Tag,TagText);
				(*pStr) += TagText;
				(*pStr) += "\r\n";
			}
		}
		break;

		case TAG_ESSENTIALTAGS:
		{
			UINT32 Size = pRecord->GetSize();
			UINT32 Tag;
			String_256 TagText;

			(*pStr) += "Essential tag list:\r\n";

			for (UINT32 i = 0;i < Size; i+=sizeof(UINT32))
			{
				pRecord->ReadUINT32(&Tag);
				GetTagText(Tag,TagText);
				(*pStr) += TagText;
				(*pStr) += "\r\n";
			}
		}
		break;

		case TAG_TAGDESCRIPTION:
		{
			UINT32 NumTags;
			UINT32 Tag;
			String_256 Desc,TagText;

			(*pStr) += "Tag description list:\r\n";

			pRecord->ReadUINT32(&NumTags);

			for (UINT32 i = 0;i < NumTags; i++)
			{
				pRecord->ReadUINT32(&Tag);
				pRecord->ReadUnicode(&Desc);//Desc,Desc.MaxLength());
				GetTagText(Tag,TagText);
				(*pStr) += TagText;
				(*pStr) += " (";
				(*pStr) += Desc;
				(*pStr) += ")\r\n";
			}
		}
		break;
	}
}
#endif


