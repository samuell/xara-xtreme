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


#include "camtypes.h"

//#include "filters.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "epsfiltr.h"
//#include "coreleps.h"
#include "aw_eps.h"
#include "cameleps.h"
#include "ai_eps.h"
#include "ai5_eps.h"
#include "ai8_eps.h"
//#include "drawfltr.h"
//#include "textfltr.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "errors.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "progress.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "chapter.h"
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpfiltr.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "native.h"			// The new designed native filter, used for v2
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "layer.h"
//#include "nev.h"
//#include "coplfilr.h"
//#include "cdrfiltr.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ben.h"
#include "freeeps.h"
//#include "rik.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fontman.h"
//#include "peter.h"
#include "sgliboil.h"		// FileExists
#include "cmxfiltr.h"
//#include "will2.h"
//#include "filtrres.h"
//#include "cmxifltr.h"
#include "exjpeg.h"			// JPEGExportFilter
#include "filtimag.h"		// Imagemap filter
#include "pngfiltr.h"		// for PNG filter class
#include "imjpeg.h"			// for JPEGImportFilter
#include "swffiltr.h"		// For the SWF export filter.
//#include "extfilts.h"		// For the TIFF filter.
#include "kerneldc.h"
#include "xsepsops.h"

#include "giffiltr.h"		// Transparent and interlaced GIF filter``
#include "htmlfltr.h"		// HTML filter
#include "lineattr.h"

//#include "andy.h"			//For _R(IDM_OVERWRITE)
//#include "resource.h"		//For _R(IDS_CANCEL)
#include "helpuser.h"		//For SetNextMsgHelpContext()
//#include "resimmap.h"		//For _R(IDM_EXPORT_OVERWRITE)
#include "imgmgkft.h"

#include "webfiltr.h"		// The new web filter which is the minimilistic form of the new native filter
#include "ftfilter.h"
#include "hardwaremanager.h"
using namespace oilHardwareManager;

// Ralph's CDR / CMX import state is currently in limbo land... To put
// the filters back in, undefine NOCDRCMX and also add back the relevant
// bits in winoil and kernel.mak
#ifdef RALPH
//#define NOCDRCMX 1
#endif

CC_IMPLEMENT_DYNAMIC(Filter, CCObject)

CC_IMPLEMENT_DYNAMIC(VectorFilter,  Filter)
CC_IMPLEMENT_DYNAMIC(BitmapFilter,  Filter)
CC_IMPLEMENT_DYNAMIC(FilterFamily, Filter)
CC_IMPLEMENT_DYNAMIC(GenericFilter, FilterFamily)
CC_IMPLEMENT_DYNAMIC(VectorFilterFamily, FilterFamily)
CC_IMPLEMENT_DYNAMIC(BitmapFilterFamily, FilterFamily)
CC_IMPLEMENT_DYNAMIC(GenericEPSFilter, FilterFamily)
#ifndef NOCDRCMX
CC_IMPLEMENT_DYNAMIC(PaletteFilterFamily, FilterFamily)
#endif

#if BUILD_TEXT_FILTERS
CC_IMPLEMENT_DYNAMIC(TextFilter,  Filter)
CC_IMPLEMENT_DYNAMIC(TextFilterFamily, FilterFamily)
#endif

CC_IMPLEMENT_MEMDUMP(SnapShotList, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(SnapShotItem, ListItem);
CC_IMPLEMENT_MEMDUMP(AttrRecordList, CC_CLASS_MEMDUMP);
CC_IMPLEMENT_MEMDUMP(AttrRecordItem, ListItem);


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


// Initialised to "no override".
UINT32 Filter::m_nForcedStatusExportMessage = 0;

ID Filter::m_NextFilterID	= FILTERID_ALDUS_END + 1;


/********************************************************************************************

>	BOOL FilterFlags::operator==(const FilterFlags Flags)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/03/94
	Inputs:		Flags - the flags to test against.
	Returns:	TRUE or FALSE!
	Purpose:	Test FilterFlags for equality!

********************************************************************************************/

BOOL FilterFlags::operator==(const FilterFlags Flags)
{
	return (CanImport == Flags.CanImport) && (CanExport == Flags.CanExport) &&
		   (ShowFilter == Flags.ShowFilter) && (CanExportMultipleImages == Flags.CanExportMultipleImages);
}




// The global list of installed filters.
List Filter::FilterList;

// Er..dunno actually
GenericFilter *Filter::pGenericFilter = NULL;

//
// Variables used for progress display update...
//

// The number of nodes saved out so far (also used for accumulating total
// while scanning the tree.
UINT32 Filter::NumNodes = 0;

// Indicates how many 'nodes' should be saved before we update the progress bar.
UINT32 Filter::UpdateEvery = 1;

// The last time we updated the progress bar (in terms of nodes saved out).
UINT32 Filter::LastExportProgressUpdate = 0;

// The maximum limit on the current export sub-operation - i.e. this ensures that
// incorrect exporting code cannot screw up the rest of the progress bar system.
UINT32 Filter::CurrentProgressLimit = 0;

// Allows a ExportRenderer to cope with rendering in strips where mulit-pass rendering
// will be happening. If the progress is started with NumNodes * number of strips we
// we need to have an offset for the start of each strip. This allows this to happen. 
UINT32 Filter::ProgressOffset = 0;

UINT32 Filter::m__StripStart = 0;

// Preference settings

/********************************************************************************************

	Preference:	MinLineWidth
	Section:	Filters
	Range:		Between 0 and 10,000.
	Purpose:	Controls how line widths are handled - if a line width thinner than the
				value of this preference is found, then the line width is forced to be
				this width.  E.g. set it to 500 to make all imported lines be at least
				0.5 points wide.  (It defaults to 0.25pt)
	SeeAlso:	AddUnnamedColours

********************************************************************************************/ 

INT32 Filter::MinLineWidth = 250;

/********************************************************************************************

	Preference:	ImportWithLayers
	Section:	Filters
	Range:		TRUE or FALSE.
	Purpose:	Controls how files are imported:
					TRUE => Layers are imported/created as necessary when importing.
				   FALSE => Layers are ignore when importing, and the whole imported
							file is placed in the document as a group.
	SeeAlso:	Filter

********************************************************************************************/ 
#ifdef WEBSTER
// Layers are bad in Webster as it conflicts with frames
INT32 Filter::ImportWithLayers = 0; // dont import layers
#else
INT32 Filter::ImportWithLayers = 2; // import layers with the names they were given
#endif // WEBSTER

/********************************************************************************************

	Preference:	OpenWithLayers
	Section:	Filters
	Range:		TRUE or FALSE.
	Purpose:	Controls how files are opened:
					TRUE => Layers are imported/created as necessary when opening a file.
				   FALSE => Layers are ignore when importing, and the whole opened
							file is placed in the document as a group.
	Note:		Does not affect v2 Native file formats such as xar and web.

********************************************************************************************/ 
#ifdef WEBSTER
// Layers are bad in Webster as it conflicts with frames
BOOL Filter::OpenWithLayers = FALSE;
#else
BOOL Filter::OpenWithLayers = TRUE;
#endif // WEBSTER

/********************************************************************************************

	Preference:	ImportBitmapsOntoLayers
	Section:	Filters
	Range:		TRUE or FALSE.
	Purpose:	Controls how bitmap files are opened/imported:
					TRUE => each bitmap is placed on a new layer.
				   FALSE => each bitmaps is placed on the current layer.
				Applies to multiple format bitmaps such as animated GIFs

********************************************************************************************/ 

BOOL Filter::ImportBitmapsOntoLayers = TRUE;

/********************************************************************************************

	Preference:	bDontWarnBitmapNonMixTransp
	Section:	Filters
	Range:		TRUE or FALSE.
	Purpose:	Controls how bitmap files are opened/imported:
					TRUE => each bitmap is placed on a new layer.
				   FALSE => each bitmaps is placed on the current layer.
				Applies to multiple format bitmaps such as animated GIFs

********************************************************************************************/ 

BOOL Filter::bDontWarnBitmapNonMixTransp = FALSE;

/********************************************************************************************

	Preference:	AddUnnamedColours
	Section:	Filters
	Range:		TRUE or FALSE
	Purpose:	If it is TRUE then any colours used in the imported file but not explicitly
				named by whatever colour table structure the file uses will be created
				as named colours (usu. of the form Unnamed 1, Unnamed 2 and so on).
				If FALSE, immediate colours will be used, and will not appear on the
				document's colour bar.
				Defaults to FALSE.
	SeeAlso:	MinLineWidth

********************************************************************************************/ 

BOOL Filter::AddUnnamedColours = FALSE;

/********************************************************************************************

>	Filter::Filter ()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Set the Filter object up to be in a clean state.
	SeeAlso:	Filter; Filter::~Filter; OILFilter

********************************************************************************************/

Filter::Filter ()
{
	// Ensure destructor will not corrupt heap
	pOILFilter = NULL;
	CurrentAttrs = NULL;
	FillAttr.pAttr = NULL;

	// Set default of no export message
	ExportMsgID = 0;
	
	// Set default message of none (use default message)
	StopExportMsgID	= 0;

	// Set default of no import message
	ImportMsgID = 0;

	// No document yet...
	TheDocument = NULL;

	// No Preview Bitmap Please
	WantPreviewBmp = FALSE;

	// Ensure these are at known states, set them to do nothing so people have to change them
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
	Flags.CanExportMultipleImages = FALSE;

	// Want to show filters by default, unless the user specificaly requests otherwise
	Flags.ShowFilter = TRUE;

	// set the path filled flag to a sensible state
	PathFilled = TRUE;

	// Don't save attributes as default.
	SaveAttributes = FALSE;

	m_pSoleLayer = NULL;
}

/********************************************************************************************

>	Filter::~Filter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Destroys the OILFilter object and the current attribute array/values
				associated with this Filter.
	SeeAlso:	Filter; OILFilter

********************************************************************************************/

Filter::~Filter()
{
	// Destroy the associated OILFilter object
	delete pOILFilter;

	// Destory any temporary saved fill attribute
	if (FillAttr.Temp)
		delete FillAttr.pAttr;
}


/********************************************************************************************

>	virtual BOOL Filter::DeclareFilterPreferenceSection()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Returns:	TRUE if the preference section was declared correctly
				FALSE otherwise
	Purpose:	Ensures all filter related preferences go to the same section

********************************************************************************************/
BOOL Filter::DeclareFilterPreferenceSection()
{
	return( Camelot.DeclareSection( _T("Filters"), 10 ) );
}


/********************************************************************************************

>	virtual BOOL Filter::CanIncludePreviewBmp()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE if the filter is capable of having a preview bitmap put in it somehow.
				FALSE if it can't. Most filters can not do this.
	Purpose:	Allows you to find out if a particular filter can produce a Preview Bitmap
				in it.

********************************************************************************************/

BOOL Filter::CanIncludePreviewBmp()
{
	// Say no. Filters that can will have to say yes
	return FALSE;
}


/********************************************************************************************

>	void Filter::IncludePreviewBmp(BOOL WantBmp)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		WantBmp - TRUE if you want the filter to Include a Preview Bitmap.
						  FALSE if not.
	Purpose:	Asks the filter to include a Preview bitmap or not. NOTE: This only has
				any effect if the Filter answered TRUE to a call to CanIncludePreviewBmp.
				NOTE2: All the filters hang around even if they are not doing any filtering,
				so if you set this flag one way it will stay like that for all future uses of
				the filter until it is set again. The best method will be to swtich it on if
				required, do the import and then switch it off again immediatly afterwards.
	SeeAlso:	Filter::CanIncludePreviewBmp

********************************************************************************************/

void Filter::IncludePreviewBmp(BOOL WantBmp)
{
	// Set the internal Flag
	WantPreviewBmp = WantBmp;
}



/********************************************************************************************

>	BOOL Filter::SetUpCurrentAttrs()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Returns:	TRUE if attributes set up ok; 
				FALSE otherwise.
	Purpose:	Obtains an attribute array from the attribute manager.  This array is the
				set of default attributes - the filter should use them to maintain its
				set of current attributes while importing.  It then asks the attribute
				manager to add the correct attributes to each node based on this array.
				This should be called when importing, not when intialising, otherwise
				all filters will have this array hanging around all the time, which is bad.
	Errors:		Out of memory.
	SeeAlso:	AttributeManager; AttributeManager::ApplyBasedOnDefaults

********************************************************************************************/

BOOL Filter::SetUpCurrentAttrs()
{
	// Sanity check
	ENSURE(CurrentAttrs == NULL, "Filter::SetUpCurrentAttrs called unnecessarily!");

	// Get the array of default attributes to use as our current attributes.
	CurrentAttrs = AttributeManager::GetDefaultAttributes();
	if (CurrentAttrs == NULL)
		return FALSE;

	// Clear the 'ignore' flag on all attributes.
	INT32 NumAttrs = AttributeManager::GetNumAttributes();
	for (INT32 i = 0; i < NumAttrs; i++)
		CurrentAttrs[i].Ignore = FALSE;

	// All done
	return TRUE;
}

/********************************************************************************************

>	void Filter::DeleteCurrentAttrs()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Purpose:	Deallocates any objects used to hold the current attributes for the filter.
				Should be called after import has finished, so the attribute table isn't 
				hanging around all the time.
	SeeAlso:	Filter::SetUpCurrentAttrs

********************************************************************************************/

void Filter::DeleteCurrentAttrs()
{
	// Delete any attribute values we created
	if (CurrentAttrs != NULL)
	{
		DeleteAttributeList(CurrentAttrs);
		// Lose the attribute value array
		CCFree(CurrentAttrs);
		CurrentAttrs = NULL;
	}
}

/********************************************************************************************

	void Filter::DeleteAttributeList(AttributeEntry* pAttrList)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		pAttrList	= A pointer to the first item in the attribute list
	Returns:	-
	Purpose:	This function removes all temp attributes from the attribute list provided
				All temp attributes will have their list entries updated with NULL pointers.

*********************************************************************************************/

void Filter::DeleteAttributeList(AttributeEntry* pAttrList)
{
	// Delete any attribute values we created
	INT32 NumAttrs = AttributeManager::GetNumAttributes();
	for (INT32 i=0; i<NumAttrs; i++)
	{
		// Delete the object if it is temporary.
		if (pAttrList[i].Temp)
		{
			delete pAttrList[i].pAttr;
			pAttrList[i].pAttr = NULL;
			pAttrList[i].Temp = FALSE;
		}
	}
}



/********************************************************************************************

>	BOOL Filter::SnapShotCurrentAttrs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	TRUE if the currentattrs list has been copied
				FALSE if nothing has been copied
	Purpose:	This function grabs a snap shot of the filter managers current attributes.
				It simply makes a new List identical to that of CurrentAttrs.

*********************************************************************************************/

BOOL Filter::SnapShotCurrentAttrs()
{
	return FilterSnapShot.CreateSnapShot(CurrentAttrs);
}

/********************************************************************************************

>	void Filter::DeleteSnapShot()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	This function removes the snap shot list allocated during SnapShotCurrentAttrs().
				It should be called to match any successfull return of its sister function.
				ie SnapShotCurrentAttrs() and DeleteSnapShot() should be paired.
				Note, the calls are not recursive. ie a
				SnapShot
				SnapShot
				Delete
				Delete
				is not currently supported
	SeeAlso:	SnapShotCurrentAttrs()

*********************************************************************************************/

void Filter::DeleteSnapShot()
{
	FilterSnapShot.DestroySnapShot();
}


/********************************************************************************************

>	BOOL Filter::ApplyChangedAttrs(Node* pNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	TRUE if the changed attributes have been applied to pNode
				FALSE if nothing has been applied
	Purpose:	This function uses the snapshot facility to work out which of the
				current attributes have changed. Obviously it expects a snap shot to have
				been taken some time earlier. Only those attributes that have changes will be
				applied to the node described on entry. Note, any changed attributes which
				now match defaults will not be applied as the function calls
				ApplyBasedOnDefaults() with the changes list.

*********************************************************************************************/

BOOL Filter::ApplyChangedAttrs(Node* pNode)
{
	return FilterSnapShot.ApplyChangedAttrs(pNode,CurrentAttrs);
}



/********************************************************************************************

>	void Filter::PushCurrentAttrsBasedOnSnapShot()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	This function uses the snap shot facility. It checks the snap shot list
				against the current attribute list to find attributes which differ. Those
				that do are obviously new attributes. ie attributes which have been parsed
				by the filter after the snap shot was taken. This function takes the current
				attrubute list and removes all those attributes which have changed in this
				way. It resets them to their default none temp attribute types.

*********************************************************************************************/

void Filter::PushCurrentAttrsBasedOnSnapShot()
{
	FilterSnapShot.PushAttrsBasedOnSnapShot(CurrentAttrs);
}


/********************************************************************************************

>	void Filter::PopCurrentAttrsBasedOnSnapShot()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	This function uses the snap shot facility. It checks the snap shot list
				against the current attribute list to find attributes which differ. Those
				that do are obviously new attributes. ie attributes which have been parsed
				by the filter after the snap shot was taken. This function takes the current
				attrubute list and removes all those attributes which have changed in this
				way. It resets them to their default none temp attribute types.

*********************************************************************************************/

void Filter::PopCurrentAttrsBasedOnSnapShot()
{
	FilterSnapShot.PopAttrsBasedOnSnapShot(CurrentAttrs);
}
	





#define ADD_FILTER(Classname) \
	/* Create, initialise and install the Generic import filter */					\
	pFilter = new Classname;														\
	if (pFilter == NULL) return FALSE;												\
																					\
	if (!pFilter->Init())															\
	{																				\
		/* Error occured - report it and stop trying to initialise filters. */		\
		InformError();																\
		return TRUE;																\
	}																				\
																					\
	FilterList.AddTail(pFilter);

/********************************************************************************************

>	BOOL Filter::InitFilters()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	TRUE if initialised ok, FALSE if not.
	Purpose:	Scans all filters to find out which filters are available for use, and
				instantiates one of each, and holds them in a list.
	Errors:		Out of memory (if a filter fails to initialise for other reasons then an 
				error is reported, but Camelot will still start up ok, i.e. TRUE is 
				returned).
	SeeAlso:	Filter

********************************************************************************************/

BOOL Filter::InitFilters()
{
	// Find the filters - the kernel ones are hard-wired.
	Filter*		pFilter;

	// Create, initialise and install the Generic import filter
	ADD_FILTER(GenericFilter)

	// Keep a pointer to this one, for handling drag'n'drop
	pGenericFilter = (GenericFilter *) pFilter;

	// Create, initialise and install the generic vector and bitmap filters
	ADD_FILTER(VectorFilterFamily)
	ADD_FILTER(BitmapFilterFamily)
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#if BUILD_TEXT_FILTERS
	ADD_FILTER(TextFilterFamily)
#endif
#endif
	// Create, initialise and install the Generic EPS import filter
#ifndef STANDALONE
	ADD_FILTER(GenericEPSFilter)
#endif

#ifndef EXCLUDE_FROM_XARALX
// Remove this next line to add the CDR / CMX filters...
#ifndef NOCDRCMX
	ADD_FILTER(PaletteFilterFamily)
#endif
#endif

	// Create, initialise and install the other supported filters:
	// This must be the first filter that uses .xar or "Save as" wont work
	ADD_FILTER(CamelotNativeFilter)		// The new designed native filter, used for v2
PORTNOTETRACE("filter","Removed CamelotWebFilter usage");
#ifndef EXCLUDE_FROM_XARALX
	ADD_FILTER(CamelotWebFilter)		// The minimalistic form of the v2 native filter
#endif
	ADD_FILTER(CamelotEPSFilter)
	ADD_FILTER(CamelotNativeEPSFilter)	// The old style EPS native filter, used in v1.1
	// Winoil filters are initialised here to prevent the drop-down list from being
	// mis-ordered.

	// Graeme (25-9-00) - Primary filters, in the order that Mark requested.
	ADD_FILTER(JPEGExportFilter)
	ADD_FILTER(JPEGImportFilter)

	ADD_FILTER(PNGFilter)

	if (ImageMagickFilter::CheckPath())
	{
		ADD_FILTER(ImageMagickFilterBMP)
		ADD_FILTER(ImageMagickFilterCUR)
		ADD_FILTER(ImageMagickFilterCUT)
		ADD_FILTER(ImageMagickFilterDCM)
		ADD_FILTER(ImageMagickFilterDCX)
		ADD_FILTER(ImageMagickFilterDIB)
		ADD_FILTER(ImageMagickFilterDNG)
		ADD_FILTER(ImageMagickFilterEPDF)
		ADD_FILTER(ImageMagickFilterEPI)
		ADD_FILTER(ImageMagickFilterEPS)
		ADD_FILTER(ImageMagickFilterEPS2)
		ADD_FILTER(ImageMagickFilterEPS3)
		ADD_FILTER(ImageMagickFilterEPSF)
		ADD_FILTER(ImageMagickFilterEPSI)
		ADD_FILTER(ImageMagickFilterEPT)
		ADD_FILTER(ImageMagickFilterFAX)
		ADD_FILTER(ImageMagickFilterFITS)
		ADD_FILTER(ImageMagickFilterICO)
		ADD_FILTER(ImageMagickFilterJNG)
		ADD_FILTER(ImageMagickFilterMIFF)
		ADD_FILTER(ImageMagickFilterMPC)
		ADD_FILTER(ImageMagickFilterOTB)
		ADD_FILTER(ImageMagickFilterP7)
		ADD_FILTER(ImageMagickFilterPALM)
		ADD_FILTER(ImageMagickFilterPAM)
		ADD_FILTER(ImageMagickFilterPBM)
		ADD_FILTER(ImageMagickFilterPCD)
		ADD_FILTER(ImageMagickFilterPCDS)
		ADD_FILTER(ImageMagickFilterPCL)
		ADD_FILTER(ImageMagickFilterPCX)
		ADD_FILTER(ImageMagickFilterPDB)
		ADD_FILTER(ImageMagickFilterPDF)
		ADD_FILTER(ImageMagickFilterPGM)
		ADD_FILTER(ImageMagickFilterPICT)
		ADD_FILTER(ImageMagickFilterPIX)
		ADD_FILTER(ImageMagickFilterPNM)
		ADD_FILTER(ImageMagickFilterPPM)
		ADD_FILTER(ImageMagickFilterPS)
		ADD_FILTER(ImageMagickFilterPS2)
		ADD_FILTER(ImageMagickFilterPS3)
		ADD_FILTER(ImageMagickFilterPSD)
		ADD_FILTER(ImageMagickFilterPTIF)
		ADD_FILTER(ImageMagickFilterPWP)
		ADD_FILTER(ImageMagickFilterRLA)
		ADD_FILTER(ImageMagickFilterRLE)
		ADD_FILTER(ImageMagickFilterSCT)
		ADD_FILTER(ImageMagickFilterSFW)
		ADD_FILTER(ImageMagickFilterSUN)
		// ADD_FILTER(ImageMagickFilterSVG) - removed per NeilH 19/07/2006 so it doesn't clash with vector SVG import
		ADD_FILTER(ImageMagickFilterTGA)
		ADD_FILTER(ImageMagickFilterTIFF)
		ADD_FILTER(ImageMagickFilterTIM)
		ADD_FILTER(ImageMagickFilterTTF)
		ADD_FILTER(ImageMagickFilterVICAR)
		ADD_FILTER(ImageMagickFilterVIFF)
		ADD_FILTER(ImageMagickFilterWBMP)
		ADD_FILTER(ImageMagickFilterWPG)
		ADD_FILTER(ImageMagickFilterXBM)
		ADD_FILTER(ImageMagickFilterXCF)
		ADD_FILTER(ImageMagickFilterXPM)
		ADD_FILTER(ImageMagickFilterXWD)
	}

	ADD_FILTER(TI_GIFFilter)
PORTNOTETRACE("filter","Removed TIFFFilter");
#ifndef EXCLUDE_FROM_XARALX
	ADD_FILTER(TIFFFilter)					
#endif

#if !defined(EXCLUDE_FROM_XARALX) && !defined(PROD_XS)
	// disabled for XS builds
	OILFilter::InitPluginFilters(FilterList);
#endif

#ifndef STANDALONE

	ADD_FILTER(FlashFilter)

	// Other filters in alphabetical order.
PORTNOTE("filter","Removed filters usage")
#ifndef EXCLUDE_FROM_XARALX
	ADD_FILTER(AcornDrawFilter)
#endif
	// Adobe AI / EPS filters.
	ADD_FILTER(AIEPSFilter)
	ADD_FILTER(AI5EPSFilter)
	ADD_FILTER(AI8EPSFilter)

	ADD_FILTER(PhotoShopEPSFilter)

	ADD_FILTER(ArtWorksEPSFilter)
PORTNOTE("filter","Removed filters usage")
#ifndef EXCLUDE_FROM_XARALX

//	ADD_FILTER(BMPFilter)

	// These filters are not shown in the drop-down menu.
//	ADD_FILTER(AdobeColourSwatchFilter)
//	ADD_FILTER(AdobeColourTableFilter)
//	ADD_FILTER(JCWColourFilter)

// Remove this next line to add the CDR / CMX filters...
#ifndef NOCDRCMX

	// Corel native filters. (CMX and CDR.)
	ADD_FILTER(CDRFilter)
	ADD_FILTER(CMXImportFilter)
PORTNOTE("filter","Removed CMXFilterX usage")
	ADD_FILTER(CMXFilter16)
	ADD_FILTER(CMXFilter32) 

#endif //NOCDRCMX


	// Corel EPS filters.
PORTNOTE("filter","Removed Corel EPS filters usage")
	ADD_FILTER(Corel3EPSFilter)
	ADD_FILTER(Corel4EPSFilter)

	ADD_FILTER(CorelPaletteFilter)
#endif
	ADD_FILTER(FreeHandEPSFilter)

PORTNOTE("filter","Removed filters usage")
#ifndef EXCLUDE_FROM_XARALX
	// HTML based filters.
PORTNOTE("filter","Removed HTMLFilter usage")
	ADD_FILTER(HTMLFilter)	// the HTML filter
	ADD_FILTER(ImagemapFilter)
#endif //EXCLUDE_FROM_XARALX


#if BUILD_OTHER_TEXT_FILTERS

	// The text filters don't appear in the drop-down menu.
	ADD_FILTER(ANSITextFilter)
	ADD_FILTER(UnicodeTextFilter)
	ADD_FILTER(RTFTextFilter)

#endif

#endif

PORTNOTE("filter","Removed FlareTemplateFilter usage")
#ifndef EXCLUDE_FROM_XARALX
	ADD_FILTER(FlareTemplateFilter)		// The text version
#endif

	// Initialise the OIL filters (but they are still objects of class "Filter")
	// This can include filters specific to one platform.
	OILFilter::InitFilters(FilterList);

#if !defined(EXCLUDE_FROM_RALPH)
	// Register our preference for minimum line widths, and whether or not we import
	// unnamed colours into the document colour list.
	Camelot.DeclareSection(_T("Filters"), 10); 
	Camelot.DeclarePref(_T("Filters"), _T("MinLineWidth"), 		&MinLineWidth, 0, 10000); 
	Camelot.DeclarePref(_T("Filters"), _T("ImportWithLayers"), 	&ImportWithLayers); 
	Camelot.DeclarePref(_T("Filters"), _T("OpenWithLayers"), 	&OpenWithLayers); 
	Camelot.DeclarePref(_T("Filters"), _T("ImportBitmapsOntoLayers"), 	&ImportBitmapsOntoLayers); 
	Camelot.DeclarePref(_T("Filters"), _T("DontWarnBitmapNonMixTransp"), 	&bDontWarnBitmapNonMixTransp); 
	Camelot.DeclarePref(_T("Filters"), _T("AddUnnamedColours"), &AddUnnamedColours); 

#endif
	XSEPSExportOptions::Init();
	BaseBitmapFilter::InitBaseClass();
	// All ok
	return TRUE;
}


/********************************************************************************************

>	FilterID FilterManager::GetNewFilterID()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/12/96
	Returns:	A FilterID unique to this FilterManager for each call to the function
	Purpose:	Support function providing an unique ID for each newly registered Filter

********************************************************************************************/
ID Filter::GetNewFilterID()
{
	ERROR3IF(!m_NextFilterID, "Creating duff FilterID");

	return m_NextFilterID++;
}


/********************************************************************************************

>	FilterID FilterManager::RegisterFilter(	const Filter* const pFilter, 
											const DESTRUCTION_METHOD DestructMethod)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		pFilter:	pointer to the filter to register
				DestructMethod :	whether or not the given filter should be destructed when
									the collection of registered filters is destructed.

	Returns:	FilterID that uniquely identifies the given filter within the scope of this
				FilterManager and allows for fast retrieval of the filter.
	Purpose:	Permits the filter to be used by the FilterManager.
				For an import filter to be present on the Import dialog the filter should
				associate itself with the requisite FileFormats via AssociateFilterWithFormat()

********************************************************************************************/
ID Filter::RegisterFilter(	Filter* const pFilter)
{
	ERROR2IF(pFilter == NULL, 0, "NULL Args");

	ID newID = GetNewFilterID();
	GetFilters().AddTail(pFilter);

	return newID;
}



/********************************************************************************************

>	void Filter::DeinitFilters()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Destroy all the Filter objects.
	SeeAlso:	Filter

********************************************************************************************/

BOOL Filter::DeinitFilters()
{
	ListItem* pListItem = FilterList.GetHead();
	while (pListItem != NULL)
	{
		((Filter*)pListItem)->Deinit();

		pListItem = FilterList.GetNext(pListItem);
	}

	// Get rid of our filters
	FilterList.DeleteAll();

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::JoinFamily(CCRuntimeClass *pRuntimeClass)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		pRuntimeClass - the type of filters in this family.
	Returns:	TRUE if the filter can be added to the family;
				FALSE if not.
	Purpose:	Allow a filter to choose whether or not it wants to become part of a
				filter 'family', e.g. a filter family such as "EPS files", or
				"Bitmap files".
				The default implementation checks to see if the filter is of the same
				kind of filter, using IsKindOf(), and if so, it consents to be in the
				filter family.  Filters should only over-ride this if they need
				different behaviour for some bizarre reason.

********************************************************************************************/

BOOL Filter::JoinFamily(CCRuntimeClass *pRuntimeClass)
{
	return IsKindOf(pRuntimeClass);
}

/********************************************************************************************

>	Filter *Filter::GetFirst()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	Pointer to the first filter, or NULL if none.
	Purpose:	Iterating through the list of filters known to Camelot.  This function
				returns the first filter.
	SeeAlso:	Filter::GetNext()
	Scope:		Static

********************************************************************************************/

Filter *Filter::GetFirst()
{
	return (Filter *) FilterList.GetHead();
}

/********************************************************************************************

>	Filter *Filter::GetNext(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Inputs:		pFilter - pointer to the filter to be used to get the following filter.
	Returns:	Pointer to the next filter, or NULL if none.
	Purpose:	Iterating through the list of filters known to Camelot.  This function
				returns the next filter.
	SeeAlso:	Filter::GetFirst

********************************************************************************************/

Filter *Filter::GetNext(Filter *pFilter)
{
	return (Filter *) FilterList.GetNext(pFilter);
}

/********************************************************************************************

>	FilterFlags Filter::GetFlags()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Returns:	The FilterFlags structure
	Purpose:	Get the flags for a specified filter.
	SeeAlso:	FilterFlags

********************************************************************************************/

FilterFlags Filter::GetFlags()
{
	return Flags;
}

/********************************************************************************************

>	virtual BOOL Filter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to laod the default document before
				importing the bitmap into this file.
				In this baseclass version return FALSE and hence assume that the filters that
				need to will override this function to return TRUE.
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport;

********************************************************************************************/

BOOL Filter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// No need to check the pathname, just return FALSE as most filters will not require the
	// default document.
	return FALSE;
}	

/********************************************************************************************

>	BOOL Filter::PreHowCompatible()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94
	Returns:	True if it it worked ok.
	Purpose:	Allows a filter or set of filters to take some action or set up variables
				before being called by HowCompatible. A group of filters which share common
				code may require some variable set up before being asked to check the type.  
				Base class does nothing and so works for all filters which do not require it.
	SeeAlso:	HowCompatible

********************************************************************************************/

BOOL Filter::PreHowCompatible()
{
	return TRUE;
}

/********************************************************************************************

>	INT32 Filter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
							  INT32 FileSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/08/94
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	 0 => Not compatible with this filter - the filter will NOT be asked to
					  load such a file.
				10 => Filter thinks that it is 100% compatible with this file.
				Or any number in between depending on how much the filter likes the file.
	Purpose:	Determine if this filter can load the specified file.
				The filter is provided with the first n bytes of the file, where n is
				HeaderSize. It should look at this data and/or the pathname to determine
				if it can load this file.
				NB. Base class function is pure virtual - override it!

********************************************************************************************/

/********************************************************************************************

>	ADDR Filter::LoadInitialSegment(CCLexFile* pFile, UINT32* pSize, INT32* pFileSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pFile - the file to load from.
				Size - the number of bytes to load from the file.
	Outputs:	Size - the number of bytes actually loaded (not actually implemented yet;
						waiting for the CCFile class to be updated).
	Returns:	Address the the loaded block
	Purpose:	Loads in the first n bytes of a file, in order to investigate whether a
				filter (or number of filters in the case of the FilterFamily class), is
				compatible with the file - i.e. how well this file recognises the file
				header.
				The last byte in the block is set to 0, to prevent ASCII lexers from going
				trundling off the end.
	Errors:		Unable to open file; Out of memory
	SeeAlso:	Filter::HowCompatible; Filter::PreHowCompatible

********************************************************************************************/

ADDR Filter::LoadInitialSegment(CCLexFile* pFile, UINT32* pSize, size_t *pFileSize)
{
	ERROR3IF(pFile == NULL, "Null CCLexFile* in Filter::LoadInitialSegment");

	BOOL ThrowState = pFile->SetThrowExceptions(TRUE);
	BOOL ErrorState = pFile->SetReportErrors(FALSE);
	
	// Tell caller how big the file is in bytes
	*pFileSize = pFile->Size();

	// Now that we know the file size, check to see if it has a chance of being loaded.
	// 4 is a nice small size that would fail the tests below.
	if (*pFileSize < 4)
	{
		// Could set a new special 'I don't think this file has any useful info in'. 
		TRACEUSER( "Neville", _T("LoadInitialSegment(): Very small file found\n"));
		ERROR1(NULL, _R(IDE_FILETOOSMALL));
	}

	// Check if the file size is less than the required initial segement size.
	// If so set the size to be the file size.
	// Casting UINT32 to INT32 should be ok as we know we are between 4 and the specified size.
	if ((UINT32) *pFileSize < *pSize)
		*pSize = (UINT32) *pFileSize;

	ADDR FilterBuf = (ADDR) CCMalloc(*pSize);
	if (FilterBuf == NULL)
		return NULL;

	// Save the current read pointer.
	FilePos nOldPos = pFile->tellIn();

	try
	{
		// A flag see if things are working
		BOOL IsResultOK = TRUE;

		
		// First we will check to see if it starts with the fabby binary EPS marker
		char Buf[4];
		pFile->SetDontFail(TRUE);
		pFile->read(Buf, 4);
		pFile->SetDontFail(FALSE);

		if( Progress::IsRalphAbort() )
			return NULL; 
	
		if ((Buf[0]=='\xC5') && (Buf[1]=='\xD0') && (Buf[2]=='\xD3') && (Buf[3]=='\xC6'))
		{
			// Yes, this is a binary EPS file that has a TIFF attached, so find the EPS
			FilePos StartOfEPS = 0;

			pFile->SetDontFail(TRUE);
			pFile->read(&StartOfEPS, 4);
			StartOfEPS = LEtoNative(StartOfEPS);
			pFile->SetDontFail(FALSE);
			if(Progress::IsRalphAbort())
				return NULL; 

			TRACEUSER( "Rik", _T("EPS Starts at %ld\n"), StartOfEPS);

			// see if the result makes sense
			if (StartOfEPS==0)
			{
				// Must be rubbish
				TRACEUSER( "Rik", _T("This file claims to start at 0 - Its rubbish\n"));
				IsResultOK = FALSE;
			}
		
			// Seek to the start of the EPS ready for the importer to read it.
			pFile->seekIn(StartOfEPS, ios::beg);

			// If the end of the file comes before the 1k we require then we must only read
			// the remaining file size otherwise the read below will error.
			if ((UINT32)(*pFileSize - StartOfEPS) < *pSize)
			{
				*pSize = (UINT32) (*pFileSize - StartOfEPS);
				TRACEUSER( "Neville", _T("Reduced initial segement size to %d\n"), *pSize);
			}
		}
		else
		{
			// This was not a binary EPS file, so go back to the start of the file
			pFile->seekIn(nOldPos, ios::beg);
		}

		// BODGE: Size should be updated to the number of bytes actually read.
		pFile->SetDontFail(TRUE);
		if (IsResultOK) pFile->read(FilterBuf, *pSize);
		pFile->SetDontFail(FALSE);

		if( Progress::IsRalphAbort() )
			return NULL; 

		// Don't need this file anymore - restote its original read pointer.
		pFile->seekIn(nOldPos, ios::beg);

		// Set back the old error informing and throwing states
		pFile->SetThrowExceptions(ThrowState);
		pFile->SetReportErrors(ErrorState);

		// If it worked
		if (IsResultOK)
		{
			// Zero the last byte of the buffer to make it simple for ASCII analysis.
			FilterBuf[(*pSize) - 1] = 0;

			// Return address of file data
			return FilterBuf;
		}

		// If we get here, there was a problem, so return null
		if (FilterBuf != NULL) CCFree(FilterBuf);

		// return
		ERROR1(NULL, _R(IDS_FILECORRUPT));
	}
	catch( CFileException )
	{
		// Restore the original read pointer.
		pFile->seekIn(nOldPos, ios::beg);

		// Set back the old error informing and throwing states
		pFile->SetThrowExceptions(ThrowState);
		pFile->SetReportErrors(ErrorState);
		
		// We had an error so the buffer is no good any more so remove it and
		// return NULL to the caller.
		if (FilterBuf != NULL) CCFree(FilterBuf);
		
		// We had a problem so return null
		return NULL;
	}

	// Should not get here
	return NULL;
}


ADDR Filter::LoadInitialSegment(PathName& Path, UINT32 *Size, size_t *FileSize)
{
	CCDiskFile FilterFile;
	if (!FilterFile.open(Path, ios::in | ios::binary))
	{
		TRACEUSER( "Tim", _T("LoadInitialSegment(): Could not open file\n"));
		ERROR1(NULL, _R(IDT_IMPORT_NOTFOUND));
	}

	return LoadInitialSegment(&FilterFile, Size, FileSize);
}




/********************************************************************************************

>	Layer *Filter::GetActiveLayer(Document *pDoc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pDoc - the document to search.
	Returns:	Pointer to the layer found, or NULL if none,
	Purpose:	Find the active layer of the first spread of the specified document.
	SeeAlso:	Filter::GetFirstSpread

********************************************************************************************/

Layer *Filter::GetActiveLayer(Document *pDoc)
{
PORTNOTE("spread", "Multi-spread warning!")
	Spread *pSpread = GetFirstSpread(pDoc);
	if (pSpread != NULL)
		return pSpread->FindActiveLayer();

	return NULL;
}



/********************************************************************************************

>	BOOL Filter::MakeSureLayerExists(Document* pDoc)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		pDoc - the document that the inserting is happening in
	Returns:	TRUE if there is a layer
	Purpose:	This functions checks that there is a layer to put nodes onto during an
				import, and will create a new layer if there are none. This means that
				filters can call this function just before they try and insert a new node
				into the tree and be sure that there will be a layer for it to go onto.
				The function will return TRUE if there is a layer in the document (even
				if this function had to create the layer). It will return FALSE only if
				there are no layers and it was unable to create any new ones.

********************************************************************************************/

BOOL Filter::MakeSureLayerExists(Document* pDoc)
{
	// Find the spread that all the action is happening on
PORTNOTE("spread", "Multi-spread warning!")
	Spread* pSpread = GetFirstSpread(pDoc);
	if (pSpread==NULL)
		return FALSE;

	// Try and find a layer in this spread
	Layer* pLayer = pSpread->FindFirstLayer();
	
	// If there were no layers, then we should try and make one
	if (pLayer==NULL)
	{
		// Time to build a new layer...
		Layer *pNewLayer = new Layer(pSpread, LASTCHILD, String_256(_R(IDS_K_FILTERS_LAYERNAME)));
		
		// If we failed, return FALSE to the caller
		if (pNewLayer==NULL)
			return FALSE;

		// Make Layer1 (top layer) the active layer
		pNewLayer->SetActive(TRUE);

		// Move the Insertion position onto the new layer
		pDoc->ResetInsertionPosition();
	}

	// all worked
	return TRUE;
}



/********************************************************************************************

>	Spread *Filter::GetFirstSpread(Document *pDoc)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pDoc - the document to search.
	Returns:	Pointer to the spread found, or NULL if no spread was found.
	Purpose:	Find the first spread of the specified document.
	SeeAlso:	Filter::GetActiveLayer

********************************************************************************************/

Spread *Filter::GetFirstSpread(Document *pDoc)
{
	// Search for the first chapter node
	Node *pNode = pDoc->GetFirstNode()->FindNext()->FindFirstChild();
	while ((pNode != NULL) && (!pNode->IsKindOf(CC_RUNTIME_CLASS(Chapter))))
		pNode = pNode->FindNext();
		
	ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(Chapter)), 
		   "Filter::GetFirstSpread(): Could not find Chapter");
	Chapter *pChapter = (Chapter *) pNode;
	
	// pSpread is a child of pChapter
PORTNOTE("spread", "Multi-spread warning!")
	Spread *pSpread = (Spread *) pChapter->FindFirstChild();
	ENSURE(pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)),
		   "Filter::GetFirstSpread(): Could not find Spread");

	if (pSpread->IsKindOf(CC_RUNTIME_CLASS(Spread)))
		return pSpread;
	else
		return NULL;
}


/********************************************************************************************

>	BOOL Filter::ImportBinary(ADDR pData, INT32 Length)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		Length - the number of bytes to read.
	Outputs:	pData - the buffer to place the bytes in.
	Returns:	The number of bytes read, or -1 for an error.
				(NB. Base class version always returns -1)
	Purpose:	Read in binary data from the file and place it in the buffer supplied by
				the caller.  This is over-ridden by filters to do whatever translation is
				necessary, e.g. EPS filters convert ASCII Hexadecimal data to binary.

				NB. This base class version has no functionality - it always returns
				FALSE without setting the error ID and ENSUREs in debug builds.
				Don't use it!

	Errors:		End of file; Disk error

********************************************************************************************/

INT32 Filter::ImportBinary(ADDR pData, INT32 Length)
{
	ENSURE(FALSE, "Base class ImportBinary() function called!");
	return -1;
}


/********************************************************************************************

>	virtual UINT32 Filter::GetImportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/95
	Returns:	The id of the message to put on the progress display whilst importing.
	Purpose:	Used to get the message id to be used during export.
				Virtual, so that multi-stage importers can change the message.
	SeeAlso:	DoImport;

********************************************************************************************/

UINT32 Filter::GetImportMsgID()
{
	return ImportMsgID;
}

/********************************************************************************************

>	virtual UINT32 Filter::GetExportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/95
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Virtual, so that two stage exporters can change the message.
	SeeAlso:	DoExport;

********************************************************************************************/

UINT32 Filter::GetExportMsgID()
{
	return (m_nForcedStatusExportMessage) ? m_nForcedStatusExportMessage : ExportMsgID;
}



/********************************************************************************************
>	static void Filter::ForceStatusExportMessage(UINT32 nMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/96
	Inputs:		nMessage		----	the message to use instead of the usual when
										exporting a file.
	Purpose:	Overrides the status-line message shown when a file is exported.  This is
				used by the clipboard code to change "Saving document..." to
				"Copying to clipboard..."
	SeeAlso:	Filter::UnforceStatusExportMessage; ExternalClipboard::GetDataSource
********************************************************************************************/

void Filter::ForceStatusExportMessage(UINT32 nMessage)
{
	// Use this message please (if derived classes allow it).
	m_nForcedStatusExportMessage = nMessage;
}



/********************************************************************************************
>	static void Filter::UnforceStatusExportMessage()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/9/96
	Purpose:	Resets the message override on export back to "no override".
	SeeAlso:	Filter::ForceStatusExportMessage; ExternalClipboard::GetDataSource
********************************************************************************************/

void Filter::UnforceStatusExportMessage()
{
	// Set this back to "no override".
	m_nForcedStatusExportMessage = 0;
}



/********************************************************************************************

>	BOOL GetProgressString(CCLexFile* pTheFile, String_64 *pMessage, String_64* StrToAddToProgress=NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/95
	Inputs:		pTheFile	file being used for importing
				pMessage	pointer to the string to append extra bits to
				StrToAddToProgress - A string to add to the progress string. This is used mainly by the 
									html import filter when importing bitmap to display "file 1 of n".
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	Works out what progress bar string to use during importing or exporting of
				the file specified.
				This consists of the passed in basic message plus, if it will fit, a truncated
				form, if necessary, of the filename.
				The file must be a diskfile so that it can access a filename, if not then
				nothing will be added. If the pTheFile is null then nothing will be added. 
	SeeAlso:	Filter::GetImportProgressString;  Filter::GetExportProgressString;

********************************************************************************************/

BOOL GetProgressString(CCLexFile* pTheFile, String_64 *pMessage, String_64* StrToAddToProgress=NULL)
{
	// Get a shortened name for the file
	String_256 Filename;
	INT32 MaxSize = pMessage->MaxLength() - pMessage->Length(); 
	// Now take off the size of the " '" and "'" that will be added around it
	MaxSize -= 3;
	
	// CCFile::GetName will return what will fit into the string povided. What we
	// really want is just the filename, only present on disk files.
	if (pTheFile != NULL && pTheFile->IS_KIND_OF(CCDiskFile))
	{
		CCDiskFile *pTheDiskFile = (CCDiskFile *)pTheFile;
		// This will only work on CCDiskFiles
		PathName path = pTheDiskFile->GetPathName();

		// Use just the filename
		Filename = path.GetFileName();

		// If it is still too long, turn it into the form "...ffff.ext", i.e. have an
		// ellipsis followed by as much of the filename as we can fit into the string.
		// (We use the last segment of the filename, i.e. we throw away characters from
		// the beginning).
		INT32 FilenameLen = Filename.Length();
		if (MaxSize < FilenameLen)
		{
			// Shuffle pathname down to fit into buffer
			Filename.Remove(0, FilenameLen - MaxSize);

			// Put the ellipsis at the front
			TCHAR *Buf = (TCHAR *) Filename;
			Buf[0] = '.';
			Buf[1] = '.';
			Buf[2] = '.';
		}

		// Do an extra check to ensure that waht we are about to add in will fit
		if (Filename.Length() <= MaxSize)
		{
			// If we have a valid file name and it will fit into the progress string
			// then tag this onto the end of the progress string with single quotes.

			PCTSTR		res = camStrstr( (const TCHAR *)Filename, _T(".tmp") );
			
			if (res == NULL)
			{
				*pMessage += String_32(_R(IDS_K_FILTERS_FILESTARTQUOTE));
				*pMessage += Filename;	
				*pMessage += String_32(_R(IDS_K_FILTERS_FILEENDQUOTE));
			}

			// add (for instance) "file 1 of 5"....
			if (StrToAddToProgress && !StrToAddToProgress->IsEmpty())
				*pMessage += *StrToAddToProgress;
		}
	}
	
//	if (pExportFile->GetName(&Filename) && (Filename.Length() < MaxSize))
//	{
//		// If we have a valid file name and it will fit into the progress string
//		// then tag this onto the end of the progress string with single quotes.
//		ProgressString += " '";
//		ProgressString += Filename;
//		ProgressString += "'";
//	}
	
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL Filter::AddStringToProgressString(String_64 *Str)

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/04/97
	Inputs:		Str - This is the string to add in the ProgressString
	Outputs:	-
	Returns:	TRUE
	Purpose:	
	SeeAlso:	

********************************************************************************************/

BOOL Filter::AddStringToProgressString(String_64 *Str)
{
	m_StrToAddToProgress = *Str;

	return TRUE;
}


/********************************************************************************************

>	virtual StringBase Filter::GetImportProgressString(CCLexFile* ImportFile, UINT32 ImportingMsgID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/95
	Inputs:		ImportFile		file being used for importing
				ImportingMsgID	id of message to use as basis on progress string
	Outputs:	-
	Returns:	String to use on the progress bar during import.
	Purpose:	Works out what progress bar string to use during importing of the file.
				This consists of a basic message plus, if it will fit, a truncated form of
				the filename being imported.
	SeeAlso:	Filter::GetExportProgressString;

********************************************************************************************/

String_64 Filter::GetImportProgressString(CCLexFile* pImportFile, UINT32 ImportingMsgID)
{
	String_64 ProgressString(ImportingMsgID);

	// Ammend the string with the filename, if we can. 
	GetProgressString(pImportFile, &ProgressString, &m_StrToAddToProgress);

	return ProgressString;
}

/********************************************************************************************

>	virtual StringBase Filter::GetExportProgressString(CCLexFile* ExportFile, UINT32 ExportingMsgID)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/95
	Inputs:		ExportFile		file being used for importing
				ImportingMsgID	id of message to use as basis on progress string
	Outputs:	-
	Returns:	String to use on the progress bar during import.
	Purpose:	Works out what progress bar string to use during exporting of the file.
				This consists of a basic message plus, if it will fit, a truncated form of
				the filename being exported.
	SeeAlso:	Filter::GetImportProgressString;

********************************************************************************************/

String_64 Filter::GetExportProgressString(CCLexFile* pExportFile, UINT32 ExportingMsgID)
{
#ifdef DO_EXPORT
	String_64 ProgressString(ExportingMsgID);

	// Ammend the string with the filename, if we can. 
	GetProgressString(pExportFile, &ProgressString);

	return ProgressString;
#else
	return String_64( _T("") );
#endif
}

/********************************************************************************************

>	virtual BOOL Filter::ExportVisibleLayersOnly()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/95
	Inputs:		-
	Outputs:	-
	Returns:	True if this filter wants to exclude invisible layers and so export only
				visible items Or False if want to export everything.
	Purpose:	Determines if the filter wants to export only visible layers.
				NB. This base class version does the default action of making the filter
				export everything including hidden layers.
	SeeAlso:	Filter::ExportRender;

********************************************************************************************/

BOOL Filter::ExportVisibleLayersOnly()
{
	// base class version includes all layers and so everything will get exported
	return FALSE;
}	

/********************************************************************************************

>	virtual BOOL Filter::ExportSelectionOnly()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/95
	Returns:	True if this filter wants check if an object is selected or not	and so export
				only the selected items Or False if want to export everything.
	Purpose:	Determines if the filter wants to export only selected items.
				In the filters baseclass version the default action will be to
				export all objects by default
				Only used by bitmap export and in particular masked gif rendering at present.
	SeeAlso:	Filter::ExportRender;

********************************************************************************************/

BOOL Filter::ExportSelectionOnly(BOOL MaskedRender)
{
	// base class version says export everything regardless of whether selected or not.
	return FALSE;
}


/********************************************************************************************

>	virtual BOOL Filter::OpenExportFile(CCDiskFile * pFile, PathName *pPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/95
	Inputs:		pDiskFile	The constructed disk file to use
				pPath		The pathname of the file to be opened
	Returns:	True if the file has been opened correctly, False otherwise.
	Purpose:	Opens up the file ready for exporting into. Assumes the file has already
				been constructed with the correct buffers etc..
	SeeAlso:	OpMenuExport::DoWithParam; Filters::DoExport;
	Errors:		The relevent error is set if the file did not open properly

********************************************************************************************/

BOOL Filter::OpenExportFile(CCDiskFile * pDiskFile, PathName *pPath)
{
#ifdef DO_EXPORT
	ERROR2IF(pDiskFile == NULL, FALSE,"Filter::OpenExportFile null file supplied");
	ERROR2IF(pPath == NULL, FALSE,"Filter::OpenExportFile null pathname supplied");

	// Open up the file. This will create a zero length file just to make sure that it
	// is possible, i.e. we have write access. This should have actually already been
	// checked by the file export dialog code and trapped before this.
	// Added 27/9/95 ios::trunc as exporting a bitmap or eps file to the same file twice
	// appended the data to the end of the file on Windows 95.
	BOOL ok = pDiskFile->open(*pPath, ios::in | ios::out | ios::binary | ios::trunc);
	
	if (!ok)
	{
		// Failed to open the file so set the error that we want to report
		Error::SetError(_R(IDT_EXPORT_NOTFOUND), 0);
	}
	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL Filter::DeleteExportFile(CCDiskFile * pFile, PathName *pPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/12/95
	Inputs:		pDiskFile	The constructed disk file to use
	Returns:	True if the file has been closed and deleted correctly, False otherwise.
	Purpose:	Used for when either an export has been aborted by the user half way through
				or when an error has occured. It closes up the export file, if open, and then
				deletes it.
	SeeAlso:	OpMenuExport::DoWithParam; Filters::DoExport;
	SeeAlso:	ClipboardMapping::ExportToTempFile; OpMenuSave::SaveSpecificFile;

********************************************************************************************/

BOOL Filter::DeleteExportFile(CCDiskFile * pDiskFile)
{
#ifdef DO_EXPORT
	ERROR2IF(pDiskFile == NULL, FALSE,"Filter::CloseExportFile null file supplied");

	// Remove the zero length file that we created as part of the process.
	
	// First, ensure file is closed
	if (pDiskFile->isOpen())
		pDiskFile->close();
	
	// Get its pathname	
	PathName Path = pDiskFile->GetPathName();

	// And try and remove it, only if it exists
	if (Path.IsValid())
	{
		BOOL Exists = TRUE;
		BOOL status = TRUE;
		Exists = SGLibOil::FileExists(&Path);
		if (Exists)
			status = SGLibOil::FileDelete(&Path);
		TRACEUSER( "Neville", _T("Filter::DeleteExportFile removed exists status =%d, file status =%d/n"),Exists,status);
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Filter::ExportRender(RenderRegion *pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pRegion - the render region to export to.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Export a file to a given render region.  It is the responsibility of the
				derived filter to create a render region of the correct type, and then pass
				it to this function.
	Errors:		Render region is not connected to a spread.
	SeeAlso:	EPSRenderRegion::ExportRender, FlashRenderRegion::ExportRender

********************************************************************************************/

BOOL Filter::ExportRender ( RenderRegion *pRegion, BOOL MaskedRender)
{
#ifdef DO_EXPORT
	// Disable bitmap caching but remember the old state so we can set it back
	ScopedFlagSetter fsBitmapCacheing(NodeRenderableBounded::bEnableCacheing, FALSE);

	ERROR2IF(pRegion==NULL,FALSE,"Filter::ExportRender null render region supplied");

	// We need special handling for Camelot EPS 
	// (NB. but not native files - do not change this to IS_KIND_OF!)
	BOOL IsCamelotEPS = IS_A(pRegion, CamelotEPSRenderRegion);

	// Get the DC for this export operation
	// This can be NULL, in the case of bitmap export, or a CMetaFileDC in the case
	// of MetaFile export. In the later case we MUST NOT set up the ExportDC pointer
	// pDC otherwise the CATCH handlers will fail as there is no ExportFile. Also the
	// progress bar message will fail.
	// The CDC can be Null if we are talking about bitmap export
	CNativeDC* pCDC = pRegion->GetRenderDC();
	ExportDC* pDC = NULL;
	NumNodes = 0;

	// At present, it appears that only EPS derived filters have a file attached and so
	// are the only ones which use an ExportDC. All bitmap filters use NULL.
	if( pCDC != NULL && this->IS_KIND_OF(EPSFilter))
		pDC = (ExportDC*)CCDC::ConvertFromNativeDC(pCDC);
	
	// Find out how big the document and document components are...
	// (We only do this for EPS files)
	if(
		this->IS_KIND_OF(EPSFilter) &&
		!IsCamelotEPS)
	{
		// First, we ask the document itself
		Document *pDocument = GetDocument();
		NumNodes += pDocument->GetSizeOfExport(this);

		// Next, the document components...
		DocComponent *pComponent = pDocument->EnumerateDocComponents(NULL);

		while (pComponent != NULL)
		{
			// Ask this document component how much data it will export.
			NumNodes += pComponent->GetSizeOfExport(this);

			// Look for next doc component
			pComponent = pDocument->EnumerateDocComponents(pComponent);
		}
	}

	// Remember how big this initial header is, so we don't overflow when writing
	// out and screw up the progress indication (which could happen if, e.g. the
	// bitmap export code was incorrect).
	CurrentProgressLimit = NumNodes;

	// Find the first node that we should export from this spread
	Node *pNode = pRegion->GetRenderSpread();

	ENSURE(pNode != NULL, "Render Region has no spread in Filter::ExportRender!");
	ERRORIF(pNode == NULL, _R(IDT_EXPORT_INTERNAL_ERR), FALSE);

	// Go and find the bounding rect for this spread and set the render regions
	// clipping rect to it
	if (pNode->IsSpread())
	{
		// find out the old clip rect
		DocRect OldClipRect = pRegion->GetClipRect();

		// only do something if it is empty
		if (OldClipRect.IsEmpty())
		{
			// Get a pointer to the spread and find out its bounding rect
			Spread* pSpread = (Spread*) pNode;
			DocRect SpreadRect = pSpread->GetBoundingRect();
			pSpread->DocCoordToSpreadCoord(&SpreadRect);

			// and set the render regions clipping rect to this
			pRegion->SetClipRect(SpreadRect);
		}
	}

	// Work out if this filter wants to include all layers or just the visible ones
	// Default action will be to include all so that say an eps or native filters save
	// everything. 
	BOOL bVisibleLayersOnly = ExportVisibleLayersOnly();
	BOOL bSelectedOnly = ExportSelectionOnly(MaskedRender);

	if (IsCamelotEPS)
	{
		try
		{
			// Special 3-stage rendering needed for Camelot EPS to be renderable.
			View *pView = DocView::GetSelected();
			Matrix RenderMatrix = pRegion->GetMatrix();
			RenderViewResult Result = pView->RenderSimpleView(pRegion, RenderMatrix,
					 										  pRegion->GetRenderSpread(), 
					 										  FALSE);
			if (Result == RENDERVIEW_USERABORT)
				// Export is over - user has aborted it (but this is not an error)
				return TRUE;

			if (Result == RENDERVIEW_FAILURE)
				// Export was not successful
				return FALSE;

			// This should never happen, as Camelot EPS cannot do 'eveything'
			ERROR3IF(Result == RENDERVIEW_NOTNEEDED, 
					 "RenderSimpleView did not render anything into EPS!");

			// Finished render region - close down region (i.e. output trailer).
			((EPSRenderRegion *) pRegion)->CloseDown();
		}

		catch(CFileException)
		{
			// Didn't work - report failure to caller.
			if (pDC)
				pDC->ExportFile->SetThrowExceptions(FALSE);
			return FALSE;
		}
		// All ok
		return TRUE;
	}
	else
	{
		FilterRenderCallback MyCallback(this, TRUE, bVisibleLayersOnly, bSelectedOnly);
		pRegion->RenderTree(pNode, FALSE, FALSE, &MyCallback);
		pRegion->ResetRender();

		// Now actually export the nodes themselves
		// Use a virtual function so that filters can overide this and say render in strips as
		// the bitmap filters are required to do
		ProgressOffset = 0;
		BOOL Result = ExportRenderNodes(pRegion, pDC, bVisibleLayersOnly, bSelectedOnly, FALSE/*TRUE*/);

		// return the result to the caller
		return Result;
	}
#else
	return FALSE;
#endif
}




/********************************************************************************************

>	BOOL FilterRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/2004
	Inputs:		pNode - pointer to node about to be rendered by RenderTree
				pRender - pointer to RenderRegion running RenderTree loop
	Outputs:	-
	Returns:	TRUE if node can be rendered
				FALSE if not
	Purpose:	Handle the callback from RenderTree that has been requested by Filter

********************************************************************************************/

BOOL FilterRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)
{
// Don't need to do this because BeforeSubtree should have doen it for every node
// that gets passed to BeforeNode...
//	if (!pNode->NeedsToExport(pRegion, m_bVisibleLayersOnly, m_bSelectedOnly))
//		return FALSE;

	if (m_bCount)
	{
		if (pNode->IsNodeRenderableClass())
			Filter::NumNodes += ((NodeRenderable*)pNode)->GetSizeOfExport(m_pFilter);

		return FALSE;	// Don't render anything!
	}

	// --------------------------------------------------------------------------
	// If rendering an RGBT bitmap, don't render anything in background layers
	if (pRegion->m_DoCompression)
	{
		Layer* pLayer = (Layer*)pNode->FindParent(CC_RUNTIME_CLASS(Layer));
		if (pLayer && pLayer->IsBackground())
			return FALSE;
	}

	// Update the progress display
	if (pNode->IsNodeRenderableClass())
		m_pFilter->UpdateFilterProgress(((NodeRenderable*)pNode)->GetSizeOfExport(m_pFilter));
	else
		m_pFilter->UpdateFilterProgress(0);				// For back compatibility with previous version of this code

	// Render (export) the node that we have, using special export rendering if it exists.
	BOOL bExportRendered = pNode->ExportRender(pRegion);

	return (!bExportRendered);						// If not exported let RenderTree do it, return TRUE
}


/********************************************************************************************

>	BOOL FilterRenderCallback::BeforeSubtree(RenderRegion* pRegion,
											Node* pNode,
											Node** ppNextNode,
											BOOL bClip,
											SubtreeRenderState* pState)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/2004
	Inputs:		pNode - pointer to node about to be rendered by RenderTree
				pRender - pointer to RenderRegion running RenderTree loop
	Outputs:	pNextNode - pointer to node where navigation should continue
				State - Navigation state for RenderTree state machine
	Returns:	TRUE if we've returned a valid SubRenderState
				FALSE if not
	Purpose:	Handle the callback from RenderTree that has been requested by Filter

********************************************************************************************/

BOOL FilterRenderCallback::BeforeSubtree(RenderRegion* pRegion,
										 Node* pNode,
										 Node** ppNextNode,
										 BOOL bClip,
										 SubtreeRenderState* pState)
{
	if (pNode==NULL)
		return TRUE;

	if (m_bCount)
	{
		// We're only counting so we will control tree navigation to ensure clever
		// nodes don't skip around by themselves
		if (pNode->NeedsToExport(pRegion, m_bVisibleLayersOnly, m_bSelectedOnly))
			*pState = SUBTREE_ROOTANDCHILDREN;
		else
			*pState = SUBTREE_NORENDER;

		return TRUE;
	}

	// --------------------------------------------------------------------------
	// We're not counting so let normal, smart navigation take place
	if (pNode->NeedsToExport(pRegion, m_bVisibleLayersOnly, m_bSelectedOnly))
	{
		// This node does need to be exported
		// Tell the node at the root of the subtree that we're about to export it and it's children
		pNode->PreExportRender(pRegion);
		return FALSE;						// Tell RenderTree we want normal navigation to take place
	}
	else
	{
		*pState = SUBTREE_NORENDER;			// Tell RenderTree we want to forcibly skip this node
		return TRUE;
	}
}




/********************************************************************************************

>	virtual BOOL Filter::ExportRenderNodes ( RenderRegion	*pRegion,
											 ExportDC		*pDC,
								   			 BOOL			VisibleLayersOnly		= FALSE,
								   			 BOOL			CheckSelected			= FALSE,
											 BOOL			ShowProgress			= TRUE )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (from Neville's original code)
	Created:	12/4/00
	Inputs:		pRegion 				- the render region to export to.
				pDc						- device context to use, can be NULL.
				VisibleLayersOnly		- use visible layers or not
				ShowProgress			- TRUE then start up a progress bar or FALSE assume
										  the caller has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Sets up the file to do an export of the nodes in the tree, by initialising
				the file, and any slow-job code. It then calls the WriteNodes method to do
				the actual work, and EndExportRender to complete the task.
	SeeAlso:	Filter::ExportRender, Filter::WriteNodes, Filter::EndExportRender

********************************************************************************************/

BOOL Filter::ExportRenderNodes ( RenderRegion	*pRegion,
								 ExportDC		*pDC,
								 BOOL			VisibleLayersOnly,
								 BOOL			CheckSelected,
								 BOOL			ShowProgress )
{
#ifdef DO_EXPORT
	ERROR2IF ( pRegion == NULL, FALSE, "Filter::ExportRender null render region supplied" );

	// Start a progress update going
	if ( ShowProgress )
	{
		String_64	ExportMessage ( GetExportMsgID () );
		if ( pDC && pDC->ExportFile )
		{
			ExportMessage = GetExportProgressString ( pDC->ExportFile, GetExportMsgID () );
		}
		BeginSlowJob ( NumNodes, FALSE, &ExportMessage );
	}

	// Work out how often we want to update the progress bar.
	UpdateEvery = ( NumNodes / 100 ) + 1;
	
	LastExportProgressUpdate = 0;
	NumNodes = 0;

	// Guard against disk/file errors, and get ready to render.
	//TRY
	{
		// Get the region ready to render.
		if ( !pRegion->StartRender() )
		{
			// An error occured.
			if ( ShowProgress )
				EndSlowJob ();
			return FALSE;
		}
	}
#if 0
	CATCH ( CFileException, e )
	{
		// Didn't work - report failure to caller.
		if ( pDC )
			pDC->ExportFile->SetThrowExceptions ( FALSE );
		if ( ShowProgress )
			EndSlowJob ();
		return FALSE;
	}
	END_CATCH
#endif
	// Write the nodes out to the file, and end the export
	if ( WriteNodes ( pRegion, pDC, VisibleLayersOnly, CheckSelected, ShowProgress ) &&
		 EndExportRender ( pRegion, ShowProgress ) )
	{
		// It worked.
		return TRUE;
	}
	else
	{
		// Houston, we have a problem.
		return FALSE;
	}

#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual BOOL Filter::WriteNodes ( RenderRegion	*pRegion,
									  ExportDC		*pDC,
									  BOOL			VisibleLayersOnly,
									  BOOL			CheckSelected,
									  BOOL			ShowProgress )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (from Neville's original code)
	Created:	12/4/00
	Inputs:		pRegion 				- the render region to export to.
				pDc						- device context to use, can be NULL.
				VisibleLayersOnly		- use visible layers or not
				ShowProgress			- TRUE then start up a progress bar or FALSE assume
										  the caller has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Actually export the nodes to the given render region showing a progress bar
				as we go. Assumes everything has been set up by ExportRender.	
				Assumes that either a progress bar has been started and will be ended by the
				caller or that it should start and end the progress bar itself.
	SeeAlso:	Filter::ExportRender, Filter::ExportRenderNodes

********************************************************************************************/

BOOL Filter::WriteNodes ( RenderRegion	*pRegion,
						  ExportDC		*pDC,
						  BOOL			VisibleLayersOnly,
						  BOOL			SelectedOnly,
						  BOOL			ShowProgress )
{
#ifdef DO_EXPORT

	// Find the first node that we should export from this spread
	Node *pNode = pRegion->GetRenderSpread ();

	// Cycle through all exportable nodes, and render (export) them.
	// Export the file, but catch any file errors.
	//TRY
	{
//		BOOL IsPageBackGroundAnd32BitAlpha = FALSE;
//		BOOL LookForPageBackgrounds = TRUE;

		FilterRenderCallback MyCallback(this, FALSE, VisibleLayersOnly, SelectedOnly);	// Not counting this time!
		pRegion->RenderTree(pNode, FALSE, FALSE, &MyCallback);
		pRegion->ResetRender();
	}
#if 0
	CATCH ( CFileException, e )
	{
		// Didn't work - report failure to caller.
		if ( pDC )
			pDC->ExportFile->SetThrowExceptions ( FALSE );
		pRegion->StopRender ();
		if ( ShowProgress )
			EndSlowJob ();
		return FALSE;
	}
	END_CATCH
#endif
	// All OK
	return TRUE;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual void Filter::UpdateFilterProgress(UINT32 increment)

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (from Neville's original code)
	Created:	12/4/00
	Inputs:		pRegion 				- the render region to export to.
				ShowProgress			- TRUE then start up a progress bar or FALSE assume
										  the caller has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Shuts down the export render process.
	SeeAlso:	Filter::ExportRender, Filter::ExportRenderNodes

********************************************************************************************/

void Filter::UpdateFilterProgress(UINT32 increment)
{
	// Limit the progress display to legal values
	if (increment>0)
		CurrentProgressLimit = NumNodes + increment;

	// Update node count accordingly
	NumNodes = CurrentProgressLimit;

	if (NumNodes > (LastExportProgressUpdate + UpdateEvery))
		LastExportProgressUpdate = NumNodes;
}

/********************************************************************************************

>	virtual BOOL Filter::EndExportRender ( RenderRegion	*pRegion,
										   BOOL			ShowProgress )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com> (from Neville's original code)
	Created:	12/4/00
	Inputs:		pRegion 				- the render region to export to.
				ShowProgress			- TRUE then start up a progress bar or FALSE assume
										  the caller has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	Shuts down the export render process.
	SeeAlso:	Filter::ExportRender, Filter::ExportRenderNodes

********************************************************************************************/

BOOL Filter::EndExportRender ( RenderRegion	*pRegion,
							   BOOL			ShowProgress )
{
#ifdef DO_EXPORT

	//TRY
	{
		// Finished rendering - deinit render region.
		pRegion->StopRender ();

		// Shut down the render region.
		pRegion->CloseDown ();
	}
#if 0
	CATCH ( CFileException, e )
	{
		// Didn't work - report failure to caller.
		if ( ShowProgress )
			EndSlowJob ();
		return FALSE;
	}
	END_CATCH
#endif
	// Close down progress display, if present
	if ( ShowProgress )
		EndSlowJob ();

	// All OK
	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	UINT32 Filter::GetNumNodes()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Inputs:		-
	Outputs:	-
	Returns:	The numbers of nodes which are currently set up to be used for a progress bar.
	Purpose:	Allows access to the stored numbers of nodes which is currently set up to
				be used for a progress bar.
	
********************************************************************************************/

UINT32 Filter::GetNumNodes()
{
	return NumNodes;
}

/********************************************************************************************

>	UINT32 Filter::SetNumNodes(UINT32 NewNumNodes)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/95
	Inputs:		-
	Outputs:	-
	Returns:	The old number of nodes value
	Purpose:	Allows setting of the stored numbers of nodes which is currently set up to
				be used for a progress bar.
	
********************************************************************************************/

UINT32 Filter::SetNumNodes(UINT32 NewNumNodes)
{
	UINT32 OldNumNodes = NumNodes;
	
	NumNodes = NewNumNodes;
	
	return OldNumNodes;
}


/********************************************************************************************

>	BOOL Filter::UpdateExportedNodeCount(UINT32 NumNodesSaved)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		NumNodes - number of nodes just saved.
	Returns:	TRUE if the progress display was updated ok;
				FALSE if not (i.e. the user pressed Esc to cancel the operation).
	Purpose:	Update the progress display according to the number passed in.  The NumNodes
				parameter indicates how many nodes have just been saved.
				Most nodes won't call this when exporting as they are only '1' node.
				Nodes such as blenders or bitmaps may call this to update the progress
				more smoothly.
				E.g. a bitmap might say that it is 20 nodes if it has 20 scanlines.  It 
					could then call this function after each scanline is saved with the
					value 1.  It should not call it with more than the number of nodes it 
					claims to be, e.g. the example bitmap above should only ever call with 
					NumNodes in the range 1 to 20.  And the cumulative total of NumNodesSaved
					passed in by one node should never be more than the number of nodes it
					claims to be either.

	SeeAlso:	Node::GetSizeOfExport

********************************************************************************************/

BOOL Filter::UpdateExportedNodeCount(UINT32 NumNodesSaved)
{
#ifdef DO_EXPORT
	// Work out how many nodes have been saved in total.
	NumNodes += NumNodesSaved;

	// Make sure we remain within the legal range of values
	if (NumNodes > CurrentProgressLimit)
	{
		#if _DEBUG
		// Only report an error once for each sub-section otherwise it's too annoying.
		static UINT32 LastError = 0;
		if (LastError != CurrentProgressLimit)
		{
			if (Error::IsUserName("Tim"))
			{
				ERROR3("Filter::UpdateExportedNodeCount() called with an out of range value!");
			}
			LastError = CurrentProgressLimit;
		}
		#endif

		// Ignore the error in retail builds - it's not serious (but limit the value)
		NumNodes = CurrentProgressLimit;
	}

	// Check to see if it's worth updating yet.
	if (NumNodes > (LastExportProgressUpdate + UpdateEvery))
	{
		LastExportProgressUpdate = NumNodes;

		if (!ContinueSlowJob(NumNodes))
		{
			return FALSE;
		}
	}

#endif
	// Everything is ok if we get here
	return TRUE;
}

/********************************************************************************************

>	BitmapFilterSupport Filter::GetBitmapSupportLevel()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/95
	Returns:			NoBitmapSupport - This filter does not support bitmap images.
					SimpleBitmapSupport - This filter has simple support for bitmap images;
										  they must be saved into the file whenever
										  they are used.
				IndirectedBitmapSupport - This filter supports bitmap indirection; i.e. a
										  bitmap pool of some type is saved and then this
										  can be referenced in the file.
	Purpose:	Determine how well this filter supports bitmaps when exporting.

********************************************************************************************/

BitmapFilterSupport Filter::GetBitmapSupportLevel()
{
	// Defaults to no bitmap support
	return NoBitmapSupport;
}

/********************************************************************************************

>	BOOL Filter::ImportBitmap(CCLexFile*, KernelBitmap**)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/94
	Inputs:		-
	Returns:	TRUE if the bitmap was imported ok (or if it wasn't imported because the
					 file format does not support it;
				FALSE if an error occured.
	Purpose:	-
	Errors:		Usual disk/file errors.
	SeeAlso:	CamelotEPSFilter::ExportBitmap

********************************************************************************************/

BOOL Filter::ImportBitmap(CCLexFile*, KernelBitmap**)
{
	// Base class - ignore bitmaps
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::ExportBitmap(KernelBitmap& TheBitmap)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/09/94
	Inputs:		TheBitmap - the bitmap to export
	Returns:	TRUE if the bitmap was exported ok (or if it wasn't exported because the
					 file format does not support it;
				FALSE if an error occured.
	Purpose:	Virtual function to export a bitmap to a file.  This base class version
				always does nothing and returns TRUE, so formats which do not support
				bitmaps need do nothing.  Formats that support bitmaps should over-ride
				this function to do the necessary work required to embed the bitmap into
				the file.
	Errors:		Usual disk/file errors.
	SeeAlso:	CamelotEPSFilter::ExportBitmap

********************************************************************************************/

BOOL Filter::ExportBitmap(KernelBitmap&)
{
	// Base class - ignore bitmaps
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetLineColour(DocColour& Col)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Col - the new line colour to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for line colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetLineColour(DocColour& Col)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_STROKECOLOUR].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_STROKECOLOUR].pAttr = new StrokeColourAttribute(Col);
		if (CurrentAttrs[ATTR_STROKECOLOUR].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_STROKECOLOUR].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		StrokeColourAttribute *pAttr = 
			(StrokeColourAttribute *) CurrentAttrs[ATTR_STROKECOLOUR].pAttr;
		pAttr->Colour = Col;
	}

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetLineWidth(MILLIPOINT Width)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Width - the new line width to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for line width to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetLineWidth(MILLIPOINT Width)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Avoid zero width lines - change them to the value specified in the user preference.
	if (Width == 0)
		Width = MinLineWidth;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_LINEWIDTH].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_LINEWIDTH].pAttr = new LineWidthAttribute(Width);
		if (CurrentAttrs[ATTR_LINEWIDTH].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_LINEWIDTH].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		LineWidthAttribute *pAttr = 
			(LineWidthAttribute *) CurrentAttrs[ATTR_LINEWIDTH].pAttr;
		pAttr->LineWidth = Width;
	}

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetLineCap(LineCapType LineCap)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		LineCap - the new line cap style to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for line cap style to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetLineCap(LineCapType LineCap)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_STARTCAP].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_STARTCAP].pAttr = new StartCapAttribute;
		if (CurrentAttrs[ATTR_STARTCAP].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_STARTCAP].Temp = TRUE;
	}

	// We have an attribute - just change it.
	StartCapAttribute *pAttr = 
		(StartCapAttribute *) CurrentAttrs[ATTR_STARTCAP].pAttr;
	pAttr->StartCap = LineCap;

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetJoinType(JointType JoinType)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		JoinType - the new line join style to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for line join style to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetJoinType(JointType JoinType)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_JOINTYPE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_JOINTYPE].pAttr = new JoinTypeAttribute;
		if (CurrentAttrs[ATTR_JOINTYPE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_JOINTYPE].Temp = TRUE;
	}

	// We have an attribute - just change it.
	JoinTypeAttribute *pAttr = 
		(JoinTypeAttribute *) CurrentAttrs[ATTR_JOINTYPE].pAttr;
	pAttr->JoinType = JoinType;

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetLineTransp(UINT32 TranspType, UINT32 Transp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/95
	Inputs:		TranspType - the transparency type to use.
				Transp - the transparency (0 to 255) of the line.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for line transparency to reflect the value 
				passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetLineTransp(UINT32 TranspType, UINT32 Transp)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_STROKETRANSP].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_STROKETRANSP].pAttr = new StrokeTranspAttribute;
		if (CurrentAttrs[ATTR_STROKETRANSP].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_STROKETRANSP].Temp = TRUE;
	}

	// We have an attribute - just change it.
	StrokeTranspAttribute *pAttr = 
		(StrokeTranspAttribute *) CurrentAttrs[ATTR_STROKETRANSP].pAttr;

	pAttr->TranspType = TranspType;
	pAttr->Transp = Transp;

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetDashPattern(DashRec& Dash)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/95
	Inputs:		Dash - the Dash Pattern to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for dash pattern to reflect the value 
				passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetDashPattern(DashRec& Dash)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_DASHPATTERN].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_DASHPATTERN].pAttr = new DashPatternAttribute;
		if (CurrentAttrs[ATTR_DASHPATTERN].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_DASHPATTERN].Temp = TRUE;
	}

	// We have an attribute - just change it.
	DashPatternAttribute *pAttr = 
		(DashPatternAttribute *) CurrentAttrs[ATTR_DASHPATTERN].pAttr;

	// Bodge 
	// When we save Native files we at present save out a dash pattern ID and then
	// save out the full dash pattern definition! This is ok apart from wasting space.
	// The problem happens when we load the information back in, we load the id and apply
	// the relevent dash pattern and then load the definition and apply it, zapping the
	// id.
	// If we are a native filter then assume that the id has already been set up
	// Now done in CheckIfDefaultPattern below.
	// But have to disable both checks as if we set the id then we would have an id on 
	// a scaled version of the pattern. This would also only fix files which have been
	// save once, as saving again will have completely lost all trace of the id! 
	/* if (this->IS_KIND_OF(CamelotNativeEPSFilter))
	{
		// Note the current id already put there by the native filter
		INT32 DashId = pAttr->DashPattern.GetDashID();
		// Copy the new dash pattern over
		pAttr->DashPattern = Dash;
		// Copy the original ID across but only if it has been set
		// As it will default to zero then we copy it across if we have changed it!
		if (DashId != 0)
			pAttr->DashPattern.DashID = DashId;	
	}
	else */

	pAttr->DashPattern = Dash;
	
	// make sure the dash pattern is OK
	pAttr->DashPattern.CheckAndFix();

	// Reset the ID if it is one of our default patterns
	// Cannot do this check as when a line is first saved the definition is scaled
	// for some strange and bizarre reason, possible to do with the fact that a general
	// EPS saving routine is being used.
	//pAttr->DashPattern.CheckIfDefaultPattern();

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetStartArrow(ArrowRec& Arrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/95
	Inputs:		Arrow - the Arrow to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for Start Arrow to reflect the value 
				passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetStartArrow(ArrowRec& Arrow)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_STARTARROW].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_STARTARROW].pAttr = new StartArrowAttribute;
		if (CurrentAttrs[ATTR_STARTARROW].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_STARTARROW].Temp = TRUE;
	}

	// We have an attribute - just change it.
	StartArrowAttribute *pAttr = 
		(StartArrowAttribute *) CurrentAttrs[ATTR_STARTARROW].pAttr;

	pAttr->StartArrow = Arrow;

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetEndArrow(ArrowRec& Arrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/95
	Inputs:		Arrow - the Arrow to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for End Arrow to reflect the value 
				passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetEndArrow(ArrowRec& Arrow)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_ENDARROW].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_ENDARROW].pAttr = new EndArrowAttribute;
		if (CurrentAttrs[ATTR_ENDARROW].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_ENDARROW].Temp = TRUE;
	}

	// We have an attribute - just change it.
	EndArrowAttribute *pAttr = 
		(EndArrowAttribute *) CurrentAttrs[ATTR_ENDARROW].pAttr;

	pAttr->EndArrow = Arrow;

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetMitreLimit(MILLIPOINT MitreLimit)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/95
	Inputs:		Arrow - the Arrow to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for the Mitre Limit to reflect the value 
				passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetMitreLimit(MILLIPOINT MitreLimit)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_MITRELIMIT].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_MITRELIMIT].pAttr = new MitreLimitAttribute;
		if (CurrentAttrs[ATTR_MITRELIMIT].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_MITRELIMIT].Temp = TRUE;
	}

	// We have an attribute - just change it.
	MitreLimitAttribute *pAttr = 
		(MitreLimitAttribute *) CurrentAttrs[ATTR_MITRELIMIT].pAttr;

	pAttr->MitreLimit = MitreLimit;

	// All worked ok.
	return TRUE;
}



/********************************************************************************************

>	BOOL Filter::SetPathFilled(BOOL Filled)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/95
	Inputs:		Filled - whether or not paths should be filled.
	Returns:	TRUE if ok;
				FALSE otherwise (e.g. out of memory)
	Purpose:	Indicate whether paths should be filled or not.  This is independent of the
				current fill attribute, so, for example, if this function is called to
				set the paths to be unfilled, and then called again to set them to be
				filled, then afterwards all paths filled will be filled with whatever
				fill attribute was last set.
	Errors:		Out of memory => ERROR1
	SeeAlso:	Filter::SetFillColour

********************************************************************************************/

BOOL Filter::SetPathFilled(BOOL Filled)
{
	if(Filled == PathFilled)
		return TRUE;
	
	if (Filled)
	{
		// See if we need to reinstate the fill attribute
		if (FillAttr.pAttr != NULL)
		{
			// Yes we do...
			if (CurrentAttrs[ATTR_FILLGEOMETRY].Temp)
			{
				// Get rid of the current one (if it is a temporary attribute)
				delete CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
			}

			// Install the new one, and vape the pointer so we don't do this again.
			CurrentAttrs[ATTR_FILLGEOMETRY] = FillAttr;
			FillAttr.pAttr = NULL;
		}
	}
	else
	{
		// Lose the current saved FillAttr if we need to.
		if ((FillAttr.pAttr != NULL) && (FillAttr.Temp))
		{
			delete FillAttr.pAttr;
		}

		// Disabling the fill attribute - remember the current one.
		FillAttr = CurrentAttrs[ATTR_FILLGEOMETRY];

		// Install a new flat colour fill...
		CurrentAttrs[ATTR_FILLGEOMETRY].pAttr = new FlatFillAttribute;
		if (CurrentAttrs[ATTR_FILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_FILLGEOMETRY].Temp = TRUE;

		// .. and make it a transparent fill
		DocColour TransCol(COLOUR_TRANS);
		FillGeometryAttribute *pAttr = (FillGeometryAttribute *) 
													CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
		pAttr->SetStartColour(&TransCol);
	}

	PathFilled = Filled;

	// Must be ok if we got here
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetFillColour(DocColour& Col)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Col - the new fill colour to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for fill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetFillColour(DocColour& Col)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new FlatFillAttribute(Col);
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, FlatFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new FlatFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	FlatFillAttribute *pAttr = (FlatFillAttribute *) pEntry->pAttr;

	pAttr->SetStartColour(&Col);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetLinearFill(DocColour StartColour, DocColour EndColour,
					  		   DocCoord StartPoint, DocCoord EndPoint,
					  		   DocCoord *EndPoint2 = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		StartColour, EndColour, StartPoint, EndPoint - see GradFillAttribute.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for gradfill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetLinearFill(DocColour &StartColour, DocColour &EndColour,
					  	   DocCoord StartPoint, DocCoord EndPoint,
					  	   DocCoord *EndPoint2)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new LinearFillAttribute;
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, LinearFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new LinearFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	LinearFillAttribute *pAttr = (LinearFillAttribute *) pEntry->pAttr;

	// Copy field values.
	pAttr->SetStartColour(&StartColour);
	pAttr->SetEndColour(&EndColour);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint);
	pAttr->SetEndPoint2(EndPoint2);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetRadialFill(DocColour StartColour, DocColour EndColour,
					  		   DocCoord StartPoint, DocCoord EndPoint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		StartColour, EndColour - colours to fade between.
				StartPoint, EndPoint1 - the points defining the radial fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for gradfill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetRadialFill(DocColour &StartColour, DocColour &EndColour,
				  		   DocCoord StartPoint, DocCoord EndPoint)
{
	// Call the more general function...
	BOOL Success = SetRadialFill(StartColour, EndColour, StartPoint, EndPoint, EndPoint);

	// Set the new attribute to be a circular one, if it worked
	if (Success)
	{
		RadialFillAttribute *pAttr = 
			(RadialFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
		pAttr->MakeCircular();
	}

	return Success;
}

/********************************************************************************************

>	BOOL Filter::SetRadialFill(DocColour StartColour, DocColour EndColour,
					  		   DocCoord StartPoint, 
					  		   DocCoord EndPoint1, DocCoord EndPoint2)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		StartColour, EndColour - colours to fade between.
				StartPoint, EndPoint1, EndPoint2 - the points defining the elliptical
												   radial fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for gradfill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetRadialFill(DocColour &StartColour, DocColour &EndColour,
				  		   DocCoord StartPoint, 
				  		   DocCoord EndPoint1, DocCoord EndPoint2)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new RadialFillAttribute;
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, RadialFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new RadialFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	RadialFillAttribute *pAttr = (RadialFillAttribute *) pEntry->pAttr;

	// Copy field values.
	pAttr->SetStartColour(&StartColour);
	pAttr->SetEndColour(&EndColour);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint1);
	pAttr->SetEndPoint2(&EndPoint2);

	// Make sure it's elliptical
	pAttr->MakeElliptical();

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetSquareFill(DocColour StartColour, DocColour EndColour,
					  		   DocCoord StartPoint, 
					  		   DocCoord EndPoint1, DocCoord EndPoint2)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/08/94
	Inputs:		StartColour, EndColour - colours to fade between.
				StartPoint, EndPoint1, EndPoint2 - the points defining the elliptical
												   radial fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for gradfill colour to reflect the value passed 
				in.

				FN copied and modified by Ben.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetSquareFill(DocColour &StartColour, DocColour &EndColour,
				  		   DocCoord StartPoint, 
				  		   DocCoord EndPoint1, DocCoord EndPoint2)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new SquareFillAttribute;
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, SquareFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new SquareFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	SquareFillAttribute *pAttr = (SquareFillAttribute *) pEntry->pAttr;

	// Copy field values.
	pAttr->SetStartColour(&StartColour);
	pAttr->SetEndColour(&EndColour);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint1);
	pAttr->SetEndPoint2(&EndPoint2);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetConicalFill(DocColour StartColour, DocColour EndColour,
					  		   DocCoord StartPoint, DocCoord EndPoint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		StartColour, EndColour, StartPoint, EndPoint - see GradFillAttribute.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for gradfill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetConicalFill(DocColour &StartColour, DocColour &EndColour,
					  	   DocCoord StartPoint, DocCoord EndPoint)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new ConicalFillAttribute;
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, ConicalFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new ConicalFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	ConicalFillAttribute *pAttr = (ConicalFillAttribute *) pEntry->pAttr;

	// Copy field values.
	pAttr->SetStartColour(&StartColour);
	pAttr->SetEndColour(&EndColour);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetBitmapFill(KernelBitmap *pBitmap,
				       	   DocCoord StartPoint, DocCoord EndPoint, DocCoord EndPoint2,
				       	   DocColour *StartColour = 0, DocColour *EndColour = 0)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		pBitmap - pointer to the bitmap object to fill with.
				StartPoint, EndPoint, EndPoint2 - see BitmapFillAttribute.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for fill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; BitmapFillAttribute

********************************************************************************************/

BOOL Filter::SetBitmapFill(KernelBitmap *pBitmap,
				       	   DocCoord StartPoint, DocCoord EndPoint, DocCoord EndPoint2,
				       	   DocColour *StartColour, DocColour *EndColour)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");
	ERROR3IF((StartColour != 0 && EndColour == 0) || (StartColour == 0 && EndColour != 0),
			"SetBitmapFill, StartColour and EndColour have a different status");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new BitmapFillAttribute;
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, BitmapFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new BitmapFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	BitmapFillAttribute *pAttr = (BitmapFillAttribute *) pEntry->pAttr;

	// Copy field values.
	pAttr->SetStartColour(StartColour);
	pAttr->SetEndColour(EndColour);

	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint);
	pAttr->SetEndPoint2(&EndPoint2);

	GetDocument()->SetCurrent();
//	pAttr->AttachBitmap(pBitmap);
	pAttr->GetBitmapRef()->Attach(pBitmap, GetDocument());

	if (pAttr->GetBitmap() != pBitmap)
	{
		// It didn't use the bitmap we gave it, so we can delete it
		delete pBitmap;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetFractalFill(DocColour &StartColour, DocColour &EndColour,
				        	DocCoord StartPoint, DocCoord EndPoint1, DocCoord EndPoint2,
							INT32 Seed, double Graininess, double Gravity, double Squash, 
							UINT32 DPI, BOOL Tileable)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/11/94
	Inputs:		StartColour, EndColour - the colours of the fill
				StartPoint, EndPoint1, EndPoint2 - co-ordinates of the fill mesh
				Seed, Graininess, Gravity, Squash - strange fractal parameters
				DPI - the resolution of the fractal bitmap (dots per inch).
				Tileable - whether the fractal bitmap is tileable or not (ask Alex).
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise => ERROR1
	Purpose:	Updates the current attribute for colour fill geometry to be a fractal fill
				using the values passed in.
	Errors:		Out of memory => ERROR1
	SeeAlso:	Filter; Filter::Init; FractalFillAttribute

********************************************************************************************/

BOOL Filter::SetFractalFill(DocColour &StartColour, DocColour &EndColour,
				        	DocCoord StartPoint, DocCoord EndPoint1, DocCoord EndPoint2,
							INT32 Seed, double Graininess, double Gravity, double Squash, 
							UINT32 DPI, BOOL Tileable)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// Find out which fill attribute to change
	AttributeEntry *pEntry;
	if (FillAttr.pAttr == NULL)
		// No 'saved' fill attribute, so just update the normal one.
		pEntry = &CurrentAttrs[ATTR_FILLGEOMETRY];
	else
		// Update the saved fill attribute.
		pEntry = &FillAttr;

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!pEntry->Temp)
	{
		// Make the correct attribute value and install it as the current one.
		pEntry->pAttr = new FractalFillAttribute;
		if (pEntry->pAttr == NULL)
			return FALSE;
		pEntry->Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(pEntry->pAttr, FractalFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete pEntry->pAttr;
			pEntry->pAttr = new FractalFillAttribute;
			if (pEntry->pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	FractalFillAttribute *pAttr = (FractalFillAttribute *) pEntry->pAttr;

	// Copy field values.
	pAttr->SetStartColour(&StartColour);
	pAttr->SetEndColour(&EndColour);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint1);
	pAttr->SetEndPoint2(&EndPoint2);
	pAttr->SetSeed(Seed);
	pAttr->SetGraininess(Graininess);
	pAttr->SetGravity(Gravity);
	pAttr->SetSquash(Squash);
	pAttr->SetTileable(Tileable);

	// Actually create the fractal bitmap
	pAttr->SetFractalDPI(DPI);
	pAttr->RecalcFractal();

	// All worked ok.
	return TRUE;
}



///////////////////////
// TRANSPARENT FILLS //
///////////////////////

/********************************************************************************************

>	BOOL Filter::SetNoTranspFill()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Returns:	TRUE if the fill attribute is set ok;
				FALSE if not (e.g. out of memory)
	Purpose:	Set the current transparency fill for this import operation to be none,
				i.e. there is no transparency.
	Errors:		Out of memory
	SeeAlso:	Filter; Filter::Init; Filter::SetFlatTranspFill; TranspFillAttribute
	Note:		This also (via SetFlatTranspFill) set the new painting mode attribute as well

********************************************************************************************/

BOOL Filter::SetNoTranspFill()
{
	// At present, we use a flat transparent fill with 0% transparency and T type 1 for
	// 'no transparency'.
	return SetFlatTranspFill(1, 0);
}

/********************************************************************************************

>	BOOL Filter::SetFlatTranspFill(UINT32 TranspType, UINT32 Transp)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/94
	Inputs:		TranspType - the type of transparency (1-3)
				Transp - the 8 bit transparency value (0-255)
	Returns:	TRUE if the fill attribute is set ok;
				FALSE if not (e.g. out of memory)
	Purpose:	Set the current transparency fill for this import operation to be a flat
				transparency value.
	Errors:		Out of memory
	SeeAlso:	Filter; Filter::Init; TranspFillAttribute

********************************************************************************************/

BOOL Filter::SetFlatTranspFill(UINT32 TranspType, UINT32 Transp)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new FlatTranspFillAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr->GetRuntimeClass() != 
			CC_RUNTIME_CLASS(FlatTranspFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new FlatTranspFillAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	FlatTranspFillAttribute *pAttr = 
		(FlatTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	// Copy field values.
	pAttr->SetTranspType(TranspType);
	pAttr->SetStartTransp(&Transp);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetLinearTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
						  		     DocCoord StartPoint, DocCoord EndPoint,
						  		     DocCoord *EndPoint2 = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		TranspType - the type of transparency (1-3)
				StartTransp - the 8 bit start transparency value (0-255)
				EndTransp - the 8 bit end transparency value (0-255)
				StartPoint, EndPoint - the start and end-points of the linear fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for transparency fill to be a linear
				transparent fill as specified by the values passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; TranspFillAttribute

********************************************************************************************/

BOOL Filter::SetLinearTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
							  	 DocCoord StartPoint, DocCoord EndPoint,
							  	 DocCoord *EndPoint2)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new LinearTranspFillAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr->GetRuntimeClass() != 
			CC_RUNTIME_CLASS(LinearTranspFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new LinearTranspFillAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	LinearTranspFillAttribute *pAttr = 
		(LinearTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	// Copy field values.
	pAttr->SetTranspType(TranspType);			// waz 3362
	pAttr->SetStartTransp(&StartTransp);
	pAttr->SetEndTransp(&EndTransp);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint);
	pAttr->SetEndPoint2(EndPoint2);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetRadialTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
					  		   		 DocCoord StartPoint, DocCoord EndPoint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		TranspType - the type of transparency (1-3)
				StartTransp - the 8 bit start transparency value (0-255)
				EndTransp - the 8 bit end transparency value (0-255)
				StartPoint - the centre of the radial fill.
				EndPoint - a point on the edge of the circle defining the radial fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for transparency fill to be a circular
				transparent fill as specified by the values passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetRadialTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
				  		   DocCoord StartPoint, DocCoord EndPoint)
{
	// Call the more general function...
	BOOL Success = SetRadialTranspFill(TranspType, StartTransp, EndTransp, 
									   StartPoint, EndPoint, EndPoint);

	// Set the new attribute to be a circular one, if it worked
	if (Success)
	{
		RadialTranspFillAttribute *pAttr = 
			(RadialTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
		pAttr->MakeCircular();
	}

	return Success;
}

/********************************************************************************************

>	BOOL Filter::SetRadialTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
					  		   		 DocCoord StartPoint, 
					  		   		 DocCoord EndPoint1, DocCoord EndPoint2)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		TranspType - the type of transparency (1-3)
				StartTransp - the 8 bit start transparency value (0-255)
				EndTransp - the 8 bit end transparency value (0-255)
				StartPoint, EndPoint, EndPoint2 - the points defining the elliptical fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for transparency fill to be an elliptical
				transparent fill as specified by the values passed in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetRadialTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
				  		   		 DocCoord StartPoint, 
				  		   		 DocCoord EndPoint1, DocCoord EndPoint2)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new RadialTranspFillAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr->GetRuntimeClass() != 
			CC_RUNTIME_CLASS(RadialTranspFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new RadialTranspFillAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	RadialTranspFillAttribute *pAttr = 
		(RadialTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	// Copy field values.
	pAttr->SetTranspType(TranspType);			// waz 3469
	pAttr->SetStartTransp(&StartTransp);
	pAttr->SetEndTransp(&EndTransp);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint1);
	pAttr->SetEndPoint2(&EndPoint2);

	// Make sure it's elliptical
	pAttr->MakeElliptical();

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetConicalTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
					  		   		  DocCoord StartPoint, DocCoord EndPoint)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		TranspType - the type of transparency (1-3)
				StartTransp - the 8 bit start transparency value (0-255)
				EndTransp - the 8 bit end transparency value (0-255)
				StartPoint, EndPoint - the points defining the conical fill.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for gradfill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; GradFillAttribute

********************************************************************************************/

BOOL Filter::SetConicalTranspFill(UINT32 TranspType, UINT32 StartTransp, UINT32 EndTransp,
					  	   		  DocCoord StartPoint, DocCoord EndPoint)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new ConicalTranspFillAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr->GetRuntimeClass() != 
			CC_RUNTIME_CLASS(ConicalTranspFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new ConicalTranspFillAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	ConicalTranspFillAttribute *pAttr = 
		(ConicalTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	// Copy field values.
	pAttr->SetTranspType(TranspType);			// waz 3538
	pAttr->SetStartTransp(&StartTransp);
	pAttr->SetEndTransp(&EndTransp);
	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint);

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetBitmapTranspFill(UINT32 TranspType, KernelBitmap *pBitmap,
				       	   			 DocCoord StartPoint, 
				       	   			 DocCoord EndPoint, DocCoord EndPoint2,
	 				        	  	 UINT32 StartTransp = 0, UINT32 EndTransp = 255)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Inputs:		TranspType - the type of transparency (1-3)
				pBitmap - pointer to the bitmap object to fill with.
				StartPoint, EndPoint, EndPoint2 - see BitmapTranspFillAttribute.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for fill colour to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; BitmapFillAttribute

********************************************************************************************/

BOOL Filter::SetBitmapTranspFill(UINT32 TranspType, KernelBitmap *pBitmap,
				       	   		 DocCoord StartPoint, DocCoord EndPoint, DocCoord EndPoint2,
 				        	  	 UINT32 StartTransp, UINT32 EndTransp)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new BitmapTranspFillAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr->GetRuntimeClass() != 
			CC_RUNTIME_CLASS(BitmapTranspFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new BitmapTranspFillAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	BitmapTranspFillAttribute *pAttr = 
		(BitmapTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	// Copy field values.
	pAttr->SetTranspType(TranspType);			// waz 3606
	pAttr->SetStartTransp(&StartTransp);
	pAttr->SetEndTransp(&EndTransp);

	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint);
	pAttr->SetEndPoint2(&EndPoint2);

	GetDocument()->SetCurrent();
//	pAttr->AttachBitmap(pBitmap);
	pAttr->GetBitmapRef()->Attach(pBitmap, GetDocument());

	if (pAttr->GetBitmap() != pBitmap)
	{
		// It didn't use the bitmap we gave it, so we can delete it
		delete pBitmap;
	}

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	BOOL Filter::SetFractalTranspFill(UINT32 TranspType,
							DocCoord StartPoint, DocCoord EndPoint1, DocCoord EndPoint2,
							INT32 Seed, double Graininess, double Gravity, double Squash, 
							UINT32 DPI, BOOL Tileable,
  			        	  	UINT32 StartTransp = 0, UINT32 EndTransp = 255)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/11/94
	Inputs:		TranspType - the type of transparency (1-3)
				StartPoint, EndPoint1, EndPoint2 - co-ordinates of the fill mesh
				Seed, Graininess, Gravity, Squash - strange fractal parameters
				DPI - the resolution of the fractal bitmap (dots per inch).
				Tileable - whether the fractal bitmap is tileable or not (ask Alex).
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise => ERROR1
	Purpose:	Updates the current attribute for colour fill geometry to be a fractal fill
				using the values passed in.
	Errors:		Out of memory => ERROR1
	SeeAlso:	Filter; Filter::Init; FractalFillAttribute

********************************************************************************************/

BOOL Filter::SetFractalTranspFill(UINT32 TranspType,
								  DocCoord StartPoint, DocCoord EndPoint1, DocCoord EndPoint2,
								  INT32 Seed, double Graininess, double Gravity, double Squash, 
								  UINT32 DPI, BOOL Tileable,
		  			        	  UINT32 StartTransp, UINT32 EndTransp)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new FractalTranspFillAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr->GetRuntimeClass() != 
			CC_RUNTIME_CLASS(FractalTranspFillAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr = new FractalTranspFillAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr == NULL)
				return FALSE;
		}
	}

	// We've got an attribute - change it.
	FractalTranspFillAttribute *pAttr = 
		(FractalTranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	// Copy field values.
	pAttr->SetTranspType(TranspType);			// waz 3690
	pAttr->SetStartTransp(&StartTransp);
	pAttr->SetEndTransp(&EndTransp);

	pAttr->SetStartPoint(&StartPoint);
	pAttr->SetEndPoint(&EndPoint1);
	pAttr->SetEndPoint2(&EndPoint2);

	pAttr->SetSeed(Seed);
	pAttr->SetGraininess(Graininess);
	pAttr->SetGravity(Gravity);
	pAttr->SetSquash(Squash);
	pAttr->SetTileable(Tileable);

	// Actually create the fractal bitmap
	pAttr->SetFractalDPI(DPI);
	pAttr->RecalcFractal();

	// All worked ok.
	return TRUE;
}



BOOL Filter::SetFadeFillEffect()
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_FILLEFFECT].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_FILLEFFECT].pAttr = new FillEffectFadeAttribute;
		if (CurrentAttrs[ATTR_FILLEFFECT].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_FILLEFFECT].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(CurrentAttrs[ATTR_FILLEFFECT].pAttr, FillEffectFadeAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_FILLEFFECT].pAttr;
			CurrentAttrs[ATTR_FILLEFFECT].pAttr = new FillEffectFadeAttribute;
			if (CurrentAttrs[ATTR_FILLEFFECT].pAttr == NULL)
				return FALSE;
		}
	}

	// All worked ok.
	return TRUE;
}

BOOL Filter::SetRainbowFillEffect()
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_FILLEFFECT].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_FILLEFFECT].pAttr = new FillEffectRainbowAttribute;
		if (CurrentAttrs[ATTR_FILLEFFECT].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_FILLEFFECT].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(CurrentAttrs[ATTR_FILLEFFECT].pAttr, FillEffectRainbowAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_FILLEFFECT].pAttr;
			CurrentAttrs[ATTR_FILLEFFECT].pAttr = new FillEffectRainbowAttribute;
			if (CurrentAttrs[ATTR_FILLEFFECT].pAttr == NULL)
				return FALSE;
		}
	}

	// All worked ok.
	return TRUE;
}

BOOL Filter::SetAltRainbowFillEffect()
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_FILLEFFECT].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_FILLEFFECT].pAttr = new FillEffectAltRainbowAttribute;
		if (CurrentAttrs[ATTR_FILLEFFECT].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_FILLEFFECT].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(CurrentAttrs[ATTR_FILLEFFECT].pAttr, FillEffectAltRainbowAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_FILLEFFECT].pAttr;
			CurrentAttrs[ATTR_FILLEFFECT].pAttr = new FillEffectAltRainbowAttribute;
			if (CurrentAttrs[ATTR_FILLEFFECT].pAttr == NULL)
				return FALSE;
		}
	}

	// All worked ok.
	return TRUE;
}


BOOL Filter::SetLinearFillMapping(INT32 Repeat)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");
	ERROR3IF((Repeat < 0) || (Repeat > 3),"Bad mapping repeat in SetLinearFillMapping()");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_FILLMAPPING].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_FILLMAPPING].pAttr = new FillMappingLinearAttribute;
		if (CurrentAttrs[ATTR_FILLMAPPING].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_FILLMAPPING].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(CurrentAttrs[ATTR_FILLMAPPING].pAttr, FillMappingLinearAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_FILLMAPPING].pAttr;
			CurrentAttrs[ATTR_FILLMAPPING].pAttr = new FillMappingLinearAttribute;
			if (CurrentAttrs[ATTR_FILLMAPPING].pAttr == NULL)
				return FALSE;
		}
	}

	// Set the repeat state
	FillMappingLinearAttribute *pAttr = 
			(FillMappingLinearAttribute *) CurrentAttrs[ATTR_FILLMAPPING].pAttr;
	pAttr->Repeat = Repeat;

// *******************************************************************
// Added by Will 12/2/95
// Bitmap and Fractal tesselation does not appear to be being saved.
// This code ensures that the repeat and tesselation values matach up.
	
	FillGeometryAttribute* pCurrentFill =
			(FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if (pCurrentFill != NULL)
		pCurrentFill->SetTesselation(Repeat);

// *******************************************************************

	// All worked ok.
	return TRUE;
}


BOOL Filter::SetLinearTranspFillMapping(INT32 Repeat)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");
	ERROR3IF((Repeat < 0) || (Repeat > 3),"Bad mapping repeat in SetLinearTranspFillMapping()");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TRANSPFILLMAPPING].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr = new TranspFillMappingLinearAttribute;
		if (CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TRANSPFILLMAPPING].Temp = TRUE;
	}
	else
	{
		// Ensure we have the correct attribute
		if (!IS_A(CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr, TranspFillMappingLinearAttribute))
		{
			// Wrong type - delete it and get the right type of attribute value.
			delete CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr;
			CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr = new TranspFillMappingLinearAttribute;
			if (CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr == NULL)
				return FALSE;
		}
	}

	// Set the repeat state
	TranspFillMappingLinearAttribute *pAttr = 
			(TranspFillMappingLinearAttribute *) CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr;
	pAttr->Repeat = Repeat;

// *******************************************************************
// Added by Will 12/2/95
// Bitmap and Fractal tesselation does not appear to be being saved.
// This code ensures that the repeat and tesselation values matach up.
	
	TranspFillAttribute* pCurrentFill =
			(TranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

	if (pCurrentFill != NULL)
		pCurrentFill->SetTesselation(Repeat);

// *******************************************************************

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetWindingRule(WindingType WindingRule)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		WindingRule - the new winding rule to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for winding rule to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init; WindingType

********************************************************************************************/

BOOL Filter::SetWindingRule(WindingType WindingRule)
{
	// Sanity check
	ENSURE(CurrentAttrs != NULL, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_WINDINGRULE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_WINDINGRULE].pAttr = new WindingRuleAttribute(WindingRule);
		if (CurrentAttrs[ATTR_WINDINGRULE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_WINDINGRULE].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		WindingRuleAttribute *pAttr = 
			(WindingRuleAttribute *) CurrentAttrs[ATTR_WINDINGRULE].pAttr;
		pAttr->WindingRule = WindingRule;
	}

	// All worked ok.
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL Filter::GetDragAndDropTranslation(ImportPosition *pPos, DocRect BoundsRect, 
												   Coord* Offset)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/10/94
	Inputs:		pSpread - the spread to check against.
				BoundsRect - the bounding rectangle to check (in spread coordinates).
	Outputs:	Offset -
	Returns:	TRUE if the bounding rectangle is smaller than the spread => Offset now
					 contains the translation necessary to position the objects correctly.
				FALSE if bounding rectangle is bigger than the spread, or pPos was NULL.
					  In either case, a FALSE return results in Offset containing (0,0).
	Purpose:	Given an drag'n'drop point (in pPos), calculate the translation necessary
				to position the imported objects at this position on the spread.  It ensures
				that bounding rectangles smaller than the spread are wholly contained within
				the spread.  i.e. if you drag a file right to the edge of a spread, the 
				translation will move it closer to the centre of the spread so that it is
				wholly contained within the spread.

				Used by the import filters for drag'n'drop to ensure that after the
				imported objects are translated to the drop point, they are all still
				on the spread.
				Made virtual 20/5/97.
	SeeAlso:	Filter::DoImport

********************************************************************************************/

BOOL Filter::GetDragAndDropTranslation(ImportPosition *pPos, DocRect BoundsRect, 
									   Coord* Offset)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// First check to se if we actually have a drag and drop point.
	if (pPos == NULL)
	{
		// No - set offset to 0 and return FALSE to indicate we can't cope with this.
		Offset->x = 0;
		Offset->y = 0;
		return FALSE;
	}

	// Get the spread's bounding rectangle and convert it to spread coords.
	DocRect SpreadRect = pPos->pSpread->GetPasteboardRect();
	pPos->pSpread->DocCoordToSpreadCoord(&SpreadRect);

	// Now check that the bounding rectangle is small enough to fit on the spread...
	if ((BoundsRect.Width() > SpreadRect.Width()) ||
		(BoundsRect.Height() > SpreadRect.Height()))
	{
		// No - set offset to 0 and return FALSE to indicate we can't cope with this.
		Offset->x = 0;
		Offset->y = 0;
		return FALSE;
	}

	// Bounding box should be centred on drop point
	DocCoord Centre;
	Centre.x = (BoundsRect.lo.x + BoundsRect.hi.x) / 2;
	Centre.y = (BoundsRect.lo.y + BoundsRect.hi.y) / 2;

	Offset->x = pPos->Position.x - Centre.x;
	Offset->y = pPos->Position.y - Centre.y;

	// Transform the bounding rect to see if it's off the spread.
	BoundsRect.Translate(Offset->x, Offset->y);

	// If it's off the spread - limit it to the edge of the spread:

	// (a) Horizontal adjustment
	if (BoundsRect.lo.x < SpreadRect.lo.x)
		Offset->x += (SpreadRect.lo.x - BoundsRect.lo.x);
	else if (BoundsRect.hi.x > SpreadRect.hi.x)
		Offset->x -= (BoundsRect.hi.x - SpreadRect.hi.x);

	// (b) Vertical adjustment
	if (BoundsRect.lo.y < SpreadRect.lo.y)
		Offset->y += (SpreadRect.lo.y - BoundsRect.lo.y);
	else if (BoundsRect.hi.y > SpreadRect.hi.y)
		Offset->y -= (BoundsRect.hi.y - SpreadRect.hi.y);

	// Whatever happened, we can fit it on the spread.
	return TRUE;
#else
	Offset->x = 0;
	Offset->y = 0;
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL Filter::WillAcceptExistingFile(PathName pthToReplace)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/97
	Returns:	TRUE if this filter is willing to accept this existing file
				FALSE otherwise
	Purpose:	Checks to see if this filter will accept an existing file to export
				to.

				This base version will simply ask the user if she wants to overwrite
				the file.

				But ImagemapFilter::WillAcceptExistingFile will override this
				so the user can insert an imagemap into an existing HTML file.

********************************************************************************************/

BOOL Filter::WillAcceptExistingFile(PathName pthToReplace)
{
PORTNOTETRACE("filter","Filter::WillAcceptExistingFile - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	//We're going to put up a message box to ask the user if she wants to
	//replace the existing file.

	//So first get the truncated path name
	String_256 strToReplace =  pthToReplace.GetTruncatedPath(50);

	//Now create a string to put our error message in
	String_256 strError;

	//And make the error message up using the file name
	strError.MakeMsg(_R(IDM_EXPORT_OVERWRITE), &strToReplace);
												
	//Now, set that error message as the next one to display
	Error::SetError( 0, strError, 0 );

	//And set up the message help context
	SetNextMsgHelpContext(_R(IDM_EXPORT_OVERWRITE));

	//Now set up the buttons
	ErrorInfo Info;
	Info.Button[0] = _R(IDB_OVERWRITE);
	Info.Button[1] = _R(IDS_CANCEL);
	
	//Make "cancel" the default
	Info.OK = Info.Cancel = 2;

	//And put the error box up
	switch (AskQuestion( &Info ))
	{
		case _R(IDS_CANCEL):
			return FALSE;

		case _R(IDB_OVERWRITE):
		default:
			return TRUE;
	}
#endif
	return false;
}

/********************************************************************************************

>	virtual void Filter::SetSaveAttributes ( BOOL DoSave )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/00
	Inputs:		DoSave	- Should the attributes be saved along with the file data?
	Returns:	-
	Purpose:	Informs the filter whether or not it should record the attribute settings
				along with the file data. This allows templates to have default colours other
				than the standard Camelot values, whilst not encumbering save files with this
				extra information.
********************************************************************************************/

void Filter::SetSaveAttributes ( BOOL DoSave )
{
	SaveAttributes = DoSave;
}

/********************************************************************************************

>	CompatibleFilter::CompatibleFilter(Filter *NewFilter, INT32 NewCompatibility)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		NewFilter - the filter to add to the list.
				NewCompatibility - the compatibility rating for this filter.
	Purpose:	Initialises a list item for a CompatibleFilterList.
	SeeAlso:	CompatibleFilter

********************************************************************************************/

CompatibleFilter::CompatibleFilter(Filter *NewFilter, INT32 NewCompatibility)
{
	pFilter = NewFilter; 
	Compatibility = NewCompatibility;
}



CC_IMPLEMENT_MEMDUMP(CompatibleFilter, ListItem)
CC_IMPLEMENT_MEMDUMP(CompatibleFilterList, List)



/********************************************************************************************

>	BOOL CompatibleFilterList::AddFilter(Filter *pFilter, INT32 Compatibility)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		pFilter - the filter to add to the list.
				Compatibility - the compatibility rating of the filter.
	Returns:	TRUE if the filter was added to the list ok; FALSE if not.
	Purpose:	Add a new item to the compatible filter list, which points to the given
				filter, and stores the given compatibility rating in the list entry.
				The list is kept in sorted order, i.e. the list item is added to the list
				according to its compatibility rating; the higher the rating, the closer
				it is to the head of the list.
	Errors:		Out of memory.
	SeeAlso:	ComaptibleFilterList; CompatibleFilter

********************************************************************************************/

BOOL CompatibleFilterList::AddFilter(Filter *pFilter, INT32 Compatibility)
{
	// Make a new list item for this filter.
	CompatibleFilter *pNewItem = new CompatibleFilter(pFilter, Compatibility);
	if (pNewItem == NULL)
		return FALSE;

	// Search for the correct place in the list
	CompatibleFilter *pItem = (CompatibleFilter *) GetHead();

	while ((pItem != NULL) && (pItem->Compatibility > Compatibility))
		pItem = (CompatibleFilter *) GetNext(pItem);

	// If come to the end of the list, add it to the end, otherwise add before the
	// current item
	if (pItem == NULL)
		AddTail(pNewItem);
	else
		InsertBefore(pItem, pNewItem);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	FilterFamily::FilterFamily()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Initialise the FilterFamily object.  This can import but not export.
	SeeAlso:	FilterFamily; Filter

********************************************************************************************/

FilterFamily::FilterFamily()
{
	// We can import, but not export.
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
	Flags.CanExportMultipleImages = FALSE;
	
	pCachedBestFilter = NULL;
}

/********************************************************************************************

>	FilterFamily::~FilterFamily()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Purpose:	Destructor for FilterFamily object.  Does nothing at present.
	SeeAlso:	FilterFamily

********************************************************************************************/

FilterFamily::~FilterFamily()
{
}



/********************************************************************************************

>	INT32 FilterFamily::HowCompatible(PathName& Filename, 
							   		ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize);

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		As base class version.
	Returns:	Always returns 0 => don't understand file.
	Purpose:	Returns 0, to indicate that this filter does not recognise the file.
				This is because this is not a 'proper' import filter - it's just pretending
				to be a number of the other filters joined into one.
	SeeAlso:	FilterFamily; Filter::HowCompatible

********************************************************************************************/

INT32 FilterFamily::HowCompatible(PathName& Filename, 
							   	ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

{
	return 0;
}


/********************************************************************************************

>	Filter* FilterFamily::GetBestFilter(CCLexFile* pFile, BOOL ShowWarnings = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pFile			the file to be checked
				ShowWarnings	TRUE shows a message box if the best filter is found is
								not perfect for this file. (added 1/2/01 ChrisG)
	Returns:	pointer to the best filter found or NULL if none found.
	Purpose:	Works out the best filter to use for the specified file by asking all the
				filters in turn whether it recognises the file. 
	Errors:		Out of memory, File not recognised.
	SeeAlso:	FilterFamily::DoImport;	FilterFamily::IsDefaultDocRequired;
	SeeAlso:	Filter::IsDefaultDocRequired;
		
********************************************************************************************/

// Graeme (28/6/00) - Because AI9 files have a load of junk at their start, the original
// 1kb file header just isn't big enough. I've extended it to 8kb (which should fit nicely
// into the cache), to cover for this.
#define FILTER_HDRSIZE (8192)

Filter* FilterFamily::GetBestFilter(CCLexFile* pFile, BOOL ShowWarnings /*= FALSE*/)
{
	ERROR2IF(pFile == NULL, NULL, "FilterFamily::GetBestFilter NULL CCFile* supplied");
	
	// Load the first 1k of the file.
	UINT32 SegmentSize = FILTER_HDRSIZE;
	size_t FileSize;
	ADDR FilterBuf = LoadInitialSegment(pFile, &SegmentSize, &FileSize);

	// Error occured when accessing file
	if (FilterBuf == NULL)
		return NULL;

#if FALSE
	{
		DWORD* pDWord = (DWORD*) FilterBuf;
		BYTE* pByte = (BYTE*) FilterBuf;

		TRACE( _T("Initial segment\n"));

		for (UINT32 i = 0; i < 16; i++)
		{
			char c0 = isprint(pByte[0])?pByte[0]:'.';
			char c1 = isprint(pByte[1])?pByte[1]:'.';
			char c2 = isprint(pByte[2])?pByte[2]:'.';
			char c3 = isprint(pByte[3])?pByte[3]:'.';
			
			TRACE( _T("%04x : %08x  "), (i*4), pDWord[0]);
			TRACE( _T("%02x %02x %02x %02x  "), pByte[0], pByte[1], pByte[2], pByte[3]);
			TRACE( _T("%c%c%c%c\n"), c0, c1, c2, c3);
			pDWord += 1;
			pByte += 4;
		}
	}
#endif

	// Try to create a file-path from the file.  Some files don't have a notion of a
	// path, for these ones we will use a blank string.
	PathName Path = pFile->GetPathName();

	// Create a list of all the filters that understand this file to one degree or
	// another, sorted on how much they like the file.
	CompatibleFilterList Filters;

	Filter* pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		// Inform any filters that we are about to do a HowCompatible call.
		// This would allow a set of filters which have common functionality hidden in a
		// filter that cannot import and cannot export handle this call and hence set
		// itself up. This would allow it to maybe cache a result which should only be
		// checked by the first filter in the group. 
		if (pFilter->JoinFamily(pFilterType) && (!pFilter->GetFlags().CanImport))
			/*BOOL ok =*/ pFilter->PreHowCompatible();

		if (pFilter->GetFlags().CanImport && pFilter->JoinFamily(pFilterType))
		{
			// This is an import filter which belongs to our family - ask it if it 
			// understands this file.
			INT32 Compatibility = pFilter->HowCompatible(Path, FilterBuf, SegmentSize, 
													   (UINT32) FileSize);

//			RELTRACE( _T("Filter %s - Compatibility %d\n"), (LPCTSTR)pFilter->FilterName, (INT32)Compatibility);
			if (Compatibility > 0)
			{
				// Add this filter to the list
				if (Filters.AddFilter(pFilter, Compatibility) == FALSE)
				{
					// Error has occured - clean up our objects and return error
					CCFree(FilterBuf);
					Filters.DeleteAll();
					return NULL;
				}
			}
		}

		// Get the next filter.
		pFilter = Filter::GetNext(pFilter);
	}

	
	// added by Ben to avoid the MacPaint filter grabbing everything
	CompatibleFilter* pCFilter = (CompatibleFilter *)Filters.GetHead();
	
	// run through all the filters to see if their extensions match
	CompatibleFilter* pBestFilter = NULL;
	CompatibleFilter* pSecondBestFilter = NULL;

/*	BOOL fPathValid = Path.IsValid();
	TRACEUSER( "JustinF", _T("File path is %s\n"),
		(LPCTSTR) (fPathValid ? Path.GetPath() : "INVALID"));
*/

	INT32 bestCompatibility = 0;
	while(pCFilter != NULL)
	{
PORTNOTE("filter","Removed OILFilter usage")
#ifndef EXCLUDE_FROM_XARALX
		// (ChrisG 22/01/01) File extension matching is now done for all of the filters,
		//	regardless of their current comaptibility score. If one is found that doesn't
		//	have the correct extension, then we subtract one from that filter's score.
		//
		// This allows dodgy EPS files to have a better chance of getting the right 
		//	filter without having to give any indication of what format they're in other than
		//	the file extension.
		//
		//	NOTE: This check is only done if the path is valid.
		if (fPathValid && !pCFilter->pFilter->pOILFilter->DoesExtensionOfFileMatch(pFile))
		{
			pCFilter->Compatibility --;
		}
#endif
		// If this filter has a higher compatibility than the current best, make this the new
		// "best filter" and relegate the other to second best.
		if (pCFilter->Compatibility >= bestCompatibility)
		{
			pSecondBestFilter = pBestFilter;
			pBestFilter = pCFilter;
			bestCompatibility = pCFilter->Compatibility;
		}
		// if this isn't the best, but IS better than the previous best, then update SecondBest.
		else if (pSecondBestFilter != NULL)
		{
			if (pCFilter->Compatibility >= pSecondBestFilter->Compatibility)
			{
				pSecondBestFilter = pCFilter;
			}
		}

		pCFilter = (CompatibleFilter*) Filters.GetNext(pCFilter);
	}

	// BODGE:
	// For now, just choose the filter at the head of the list.
	// We should check for the compatibility being less than 10 and provide a scrolly list 
	// for the user to choose from if more than one filter is interested.
	//
	// Improved 16/10/2000 by ChrisG - added a scan for the best possible compatibility, so 
	// that the filter's position in the list is not quite as important as it was. Although a 
	// dialog with a list box filled with the results and compatibilities would be better.
	//
	// Note: SecondBestFilter is a bit of an irrelevance now, but may be useful for debugging 
	// purposes

	if (pBestFilter == NULL)
	{
		// If we have a second best filter then choose it
		if (pSecondBestFilter != NULL)
			pBestFilter = pSecondBestFilter;
		else
		{
			pBestFilter = (CompatibleFilter*) Filters.GetHead();
			TRACE( _T("Couldn't find best filter in FilterFamily::GetBestFilter\n"));
		}
	}

	if (pBestFilter == NULL)
	{
#ifdef STANDALONE
		// if this is the standalone version, see if it's an EPS file we just tried
		if(camStrncmp((char*) FilterBuf, "%!PS-Adobe", 10) == 0)
		{
			Error::SetError(_R(IDT_NOTTHISEPS));
			CCFree(FilterBuf);
			Filters.DeleteAll();
			return NULL;
		}
#endif
		// None of the filters liked this file - tell the user
		CCFree(FilterBuf);
		Filters.DeleteAll();
		Error::SetError(_R(IDT_FILE_NOTRECOGNISED));
		return NULL;
	}

	// (ChrisG 1/2/01) if this filter is not perfectly compatible, tell the user, 
	//	if we're showing warnings
	if (pBestFilter->Compatibility < 9)
	{
		if (ShowWarnings)
			InformWarning (_R(IDT_IMPORTFILTER_NOTPERFECT));
	}

	// Free up the objects that we used
	Filter* pBest = pBestFilter->pFilter;
	CCFree(FilterBuf);
	Filters.DeleteAll();
	return pBest;	
}	



/********************************************************************************************

>	BOOL FilterFamily::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pcszPathName	pointer to the pathname to check
	Returns:	TRUE if the filter requires a default document, FALSE if not.
	Purpose:	Works out if opening a file of this type requires a default document to be
				loaded. If the file format supplies the document then return FALSE otherwise
				return TRUE. An example would be opening a bitmap file. This has no document
				defined in the file format and so we need to laod the default document before
				importing the bitmap into this file.
	SeeAlso:	Filter; Filter::IsDefaultDocRequired; CCamDoc::OnOpenDocument;
	SeeAlso:	FilterFamily::DoImport; Filter::DoImport; FilterFamily::GetBestFilter

********************************************************************************************/

BOOL FilterFamily::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// If the pathname passed in is NULL then there is nothing we can do so say no
	// default document is required.
	if (pcszPathName == NULL) return FALSE;

	// Ensure the cached best filter is set to null
	pCachedBestFilter = NULL;

	// Work out the best filter to use on this file.
	Filter* pBestFilter = NULL;

	// First off, we have to try and open the file
	CCDiskFile file(1024, FALSE, TRUE);

	// Build a string out of the path name
	String_256 FileName(pcszPathName);

	// get it into the proper PathName class
	PathName Path(FileName);

	// If we cannot open the file then return false
	if (!file.open(Path, ios::in | ios::binary)) return FALSE;		

	// Find the best file for this to use
	pBestFilter = GetBestFilter(&file);
	
	// On this pass, no errors are allowed, at present.
	Error::ClearError();

	// Close the file again
	if (file.isOpen()) file.close();
	
	// If the filter is NULL then not found, must have set an error. 
	if (pBestFilter == NULL) return FALSE;

	// Result: do we require the default document?
	BOOL Result;

	// Ask the best filter to see if we need to load a default document for it to use.
	// At this point the pathname is irrelevent.
	Result = pBestFilter->IsDefaultDocRequired(pcszPathName);

	// Make a note of the filter that we decided on as the best filter.
	pCachedBestFilter = pBestFilter;

	return Result;
}	


/********************************************************************************************

>	BOOL FilterFamily::DoImport(SelOperation *pOp, CCLexFile* pFile, 
								Document *pDestDoc, BOOL AutoChosen, ImportPosition *Pos,
								KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate,
								String_56& URL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/03/94
	Inputs:		pOp - the live operation associated with this file import process.
				pFile - The file to import from
				pDestDoc - the document to import the file into.
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
				URL - the URL of the original imported file
	Returns:	TRUE if the document was loaded ok, FALSE if not.
	Purpose:	Loads in the initial 1k of the file, and passes this around all the filters
				to see which ones understand it.  It then chooses the filter that claims
				the highest 'compatibility', and calls its DoImport() function to load it.
	Errors:		Out of memory, File not recognised.
	SeeAlso:	Filter; Filter::DoImport; FilterFamily::GetBestFilter;
	SeeAlso:	FilterFamily::IsDefaultDocRequired; Filter::IsDefaultDocRequired;

********************************************************************************************/

BOOL FilterFamily::DoImport(SelOperation *pOp, CCLexFile* pFile, 
							Document *pDestDoc, BOOL AutoChosen, ImportPosition *Pos,
							KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	// Work out the best filter to use on this file.
	// (ChrisG 1/2/01) We do want to be warned if this filter isn't perfect
	Filter *pBestFilter = GetBestFilter(pFile, TRUE);

	// If the filter is NULL then not found, must have set an error. 
	if (pBestFilter == NULL)
	{
		// Ensure the cached best filter is set to null
		pCachedBestFilter = NULL;
		return FALSE;
	}
	
	// Result: did we import the file ok?
	BOOL Result = FALSE;

	// Use this filter to import the file, and tell the filter that it was chosen
	// automatically.
	Result = pBestFilter->DoImport(pOp, pFile, pDestDoc, TRUE, Pos, NULL, NULL, URL);

	// Make sure the art file error is more specific
	if (!Result && pBestFilter->FilterID == FILTERID_NATIVE_EPS)
	{
		// Check that it is not our special do nothing message
		// or that the error has been reported before and cleared 
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED) &&
			Error::GetErrorNumber() != _R(IDT_IMPORT_USERABORT))
		{
			Error::ClearError();
			Error::SetError(_R(IDS_ERRORINARTFILE));
		}
	}

	// Ensure the cached best filter is set back to null again
	pCachedBestFilter = NULL;

	// Return the result of importing the file.
	return Result;
}



/********************************************************************************************

>	BOOL FilterFamily::ImportBitmap(CCLexFile* pFile, KernelBitmap** ppBitmap)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/04/94
	Inputs:		pFile - The file to import from
	Outputs:	*ppBitmap is updated to point at the imported bitmap.
	Returns:	TRUE if the bitmap was loaded ok, FALSE if not.
	Purpose:	Loads in the initial 1k of the file, and passes this around all the filters
				to see which ones understand it.  It then chooses the filter that claims
				the highest 'compatibility', and calls its ImportBitmap() function to load it.
	Errors:		Out of memory, File not recognised.
	SeeAlso:	Filter; Filter::ImportBitmap

********************************************************************************************/

BOOL FilterFamily::ImportBitmap(CCLexFile* pFile, KernelBitmap** ppBitmap)
{
	// If it is not a disk file, then bail right out now.
//	if (!pFile->IS_KIND_OF(CCDiskFile)) return FALSE;

	// Find out the pathname of the file in question
	PathName Path = pFile->GetPathName();

	// Load the first 1k of the file.
	UINT32 SegmentSize = FILTER_HDRSIZE;
	size_t FileSize;
	ADDR FilterBuf = LoadInitialSegment(pFile, &SegmentSize, &FileSize);
	if (FilterBuf == NULL)
		// Error occured when accessing file
		return FALSE;

	// Create a list of all the filters that understand this file to one degree or
	// another, sorted on how much they like the file.
	CompatibleFilterList Filters;

	Filter* pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		// Inform any filters that we are about to do a HowCompatible call.
		// This would allow a set of filters which have common functionality hidden in a
		// filter that cannot import and cannot export handle this call and hence set
		// itself up. This would allow it to maybe cache a result which should only be
		// checked by the first filter in the group. 
		if (pFilter->JoinFamily(pFilterType) && (!pFilter->GetFlags().CanImport))
			/*BOOL ok =*/ pFilter->PreHowCompatible();

		if (pFilter->GetFlags().CanImport && pFilter->JoinFamily(pFilterType))
		{
			// This is an import filter which belongs to our family - ask it if it 
			// understands this file.
			INT32 Compatibility = pFilter->HowCompatible(Path, FilterBuf, SegmentSize, 
													   (UINT32) FileSize);

			if (Compatibility > 0)
			{
				// Add this filter to the list
				if (Filters.AddFilter(pFilter, Compatibility) == FALSE)
				{
					// Error has occured - clean up our objects and return error
					CCFree(FilterBuf);
					Filters.DeleteAll();
					return FALSE;
				}
			}
		}

		// Get the next filter.
		pFilter = Filter::GetNext(pFilter);
	}

	// BODGE:
	// For now, just choose the filter at the head of the list.
	// We should check for the compatibility being less than 10 and provide a scrolly list 
	// for the user to choose from if more than one filter is interested.
	CompatibleFilter* pBestFilter = (CompatibleFilter*) Filters.GetHead();

	// Result: did we import the file ok?
	BOOL Result;

	if (pBestFilter == NULL)
	{
		// None of the filters liked this file - tell the user
		Result = FALSE;
		Error::SetError(_R(IDT_FILE_NOTRECOGNISED));
	}
	else
	{
		// Use this filter to import the file, and tell the filter that it was chosen
		// automatically.
		Result = pBestFilter->pFilter->ImportBitmap(pFile, ppBitmap);
	}

	// Clean up
	CCFree(FilterBuf);
	Filters.DeleteAll();

	// Return the result of importing the file.
	return Result;
}

/********************************************************************************************

>	BOOL FilterFamily::DoExport(Operation*, CCLexFile*, PathName *, Document*, BOOL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		As base class version.
	Returns:	Always returns FALSE.
	Purpose:	Dummy function - this filter does not allow export.
	Errors:		ENSURE failure when called.

********************************************************************************************/

BOOL FilterFamily::DoExport(Operation*, CCLexFile*, PathName*, Document*, BOOL)
{
	// This filter doesn't export.
	ENSURE(FALSE, "FilterFamily::DoExport() called - this should not happen!");
	return FALSE;
}


/********************************************************************************************

>	BOOL FilterFamily::JoinFamily(CCRuntimeClass *)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Purpose:	Over-rides this function, because we don't ever want a filter family to
				join another filter family, as this would be nonsensical.
	SeeAlso:	FilterFamily

********************************************************************************************/

BOOL FilterFamily::JoinFamily(CCRuntimeClass *)
{
	return FALSE;
}


/********************************************************************************************

>	GenericFilter::GenericFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	Sets up the generic filter object to interface to all the installed
				filters.

********************************************************************************************/

GenericFilter::GenericFilter()
{
	// This is the generic filter, so we want *all* filters in it.
	pFilterType = CC_RUNTIME_CLASS(Filter);
}

/********************************************************************************************

>	BOOL GenericFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	TRUE if initalised ok; FALSE if error occurred.
	Purpose:	Set up a generic filter object.  This creates the associated OILFilter
				object, and sets up the filter ID number and strings.
	Errors:		Out of memory.
	SeeAlso:	FilterFamily; OILFilter

********************************************************************************************/

BOOL GenericFilter::Init()
{
PORTNOTE("filter", "Removed OILFilter usage")
#ifndef EXCLUDE_FROM_XARALX	
	// Get the OILFilter object
	pOILFilter = new OILFilterFamily(this, _R(IDT_FILTERNAME_GENERIC));
	if (pOILFilter == NULL)
		return FALSE;
#endif

	// Load the description strings
	FilterName.Load(_R(IDT_GENERIC_FILTERNAME));
	FilterInfo.Load(_R(IDT_GENERIC_FILTERINFO));
	FilterID = FILTERID_GENERIC;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	VectorFilterFamily::VectorFilterFamily()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	Sets up the vector filter object to interface to all the installed
				filters that decode vector file formats.

********************************************************************************************/

VectorFilterFamily::VectorFilterFamily()
{
	// This is the vector files filter, so we want *all* vector filters in it.
	pFilterType = CC_RUNTIME_CLASS(VectorFilter);
}

/********************************************************************************************

>	BOOL VectorFilterFamily::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	TRUE if initalised ok; FALSE if error occurred.
	Purpose:	Set up a vector file filter object.  This creates the associated OILFilter
				object, and sets up the filter ID number and strings.
	Errors:		Out of memory.
	SeeAlso:	FilterFamily; OILFilter

********************************************************************************************/

BOOL VectorFilterFamily::Init()
{
PORTNOTETRACE("filter","Removed OILFilter usage");
#ifndef EXCLUDE_FROM_XARALX
	// Get the OILFilter object
	pOILFilter = new OILFilterFamily(this, _R(IDT_FILTERNAME_VECTOR));
	if (pOILFilter == NULL)
		return FALSE;
#endif

	// Load the description strings
	FilterName.Load(_R(IDT_VECTOR_FILTERNAME));
	FilterInfo.Load(_R(IDT_VECTOR_FILTERINFO));
	FilterID = FILTERID_VECTOR;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BitmapFilterFamily::BitmapFilterFamily()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	Sets up the bitmap filter object to interface to all the installed
				filters that decode bitmap file formats.

********************************************************************************************/

BitmapFilterFamily::BitmapFilterFamily()
{
	// This is the bitmap files filter, so we want *all* bitmap filters in it.
	pFilterType = CC_RUNTIME_CLASS(BitmapFilter);
}

/********************************************************************************************

>	BOOL BitmapFilterFamily::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	TRUE if initalised ok; FALSE if error occurred.
	Purpose:	Set up a bitmap filter object.  This creates the associated OILFilter
				object, and sets up the filter ID number and strings.
	Errors:		Out of memory.
	SeeAlso:	FilterFamily; OILFilter

********************************************************************************************/

BOOL BitmapFilterFamily::Init()
{
PORTNOTETRACE("filter","Removed OILFilter usage");
#ifndef EXCLUDE_FROM_XARALX
	// Get the OILFilter object
	pOILFilter = new OILFilterFamily(this, _R(IDT_FILTERNAME_BITMAP));
	if (pOILFilter == NULL)
		return FALSE;
#endif

	// Load the description strings
	FilterName.Load(_R(IDT_BITMAP_FILTERNAME));
	FilterInfo.Load(_R(IDT_BITMAP_FILTERINFO));
	FilterID = FILTERID_BITMAP;

	// All ok
	return TRUE;
}



#if BUILD_TEXT_FILTERS
/********************************************************************************************
>	TextFilterFamily::TextFilterFamily()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Sets up the text filter object to interface to all the installed
				filters that decode text file formats.
********************************************************************************************/
TextFilterFamily::TextFilterFamily()
{
	pFilterType = CC_RUNTIME_CLASS(TextFilter);
}


/********************************************************************************************
>	BOOL TextFilterFamily::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Returns:	TRUE if initalised ok; FALSE if error occurred.
	Purpose:	Set up a text filter object.  This creates the associated OILFilter
				object, and sets up the filter ID number and strings.
	Errors:		Out of memory.
	SeeAlso:	FilterFamily; OILFilter
********************************************************************************************/
BOOL TextFilterFamily::Init()
{
PORTNOTETRACE("filter","Removed OILFilter usage");
#ifndef EXCLUDE_FROM_XARALX
	// Get the OILFilter object
	pOILFilter = new OILFilterFamily(this, _R(IDT_FILTERNAME_TEXT));
	if (pOILFilter == NULL)
		return FALSE;
#endif

	// Load the description strings
	FilterName.Load(_R(IDT_TEXT_FILTERNAME));
	FilterInfo.Load(_R(IDT_TEXT_FILTERINFO));
	FilterID = FILTERID_TEXT;

	// All ok
	return TRUE;
}
#endif	// #if BUILD_TEXT_FILTERS


/********************************************************************************************

>	GenericEPSFilter::GenericFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Purpose:	Sets up the generic EPS filter object to interface to all the installed
				EPS filters.

********************************************************************************************/

GenericEPSFilter::GenericEPSFilter()
{
	// This is the generic EPS filter, so we want all EPS filters in it.
	pFilterType = CC_RUNTIME_CLASS(EPSFilter);
}

/********************************************************************************************

>	BOOL GenericEPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	TRUE if initalised ok; FALSE if error occurred.
	Purpose:	Set up a generic EPS filter object.  This creates the associated OILFilter
				object, and sets up the filter ID number and strings.
	Errors:		Out of memory.
	SeeAlso:	FilterFamily; OILFilter; EPSFilter

********************************************************************************************/

BOOL GenericEPSFilter::Init()
{
PORTNOTE("filter","Removed OILFilter usage")
#ifndef EXCLUDE_FROM_XARALX
	// Get the OILFilter object
	pOILFilter = new OILFilterFamily(this, _R(IDT_FILTERNAME_EPS));
	if (pOILFilter == NULL)
		return FALSE;
#endif

	// Load the description strings
	FilterName.Load(_R(IDT_EPS_FILTERNAME));
	FilterInfo.Load(_R(IDT_EPS_FILTERINFO));
	FilterID = FILTERID_EPS;

	// All ok
	return TRUE;
}


#ifndef NOCDRCMX

/********************************************************************************************
>	PaletteFilterFamily::PaletteFilterFamily()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Purpose:	Sets up the filter object to interface to all the installed filters that
				decode palette file formats.
********************************************************************************************/
PaletteFilterFamily::PaletteFilterFamily()
{
	PORTNOTETRACE("filter","PaletteFilterFamily::PaletteFilterFamily - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	pFilterType = CC_RUNTIME_CLASS(PaletteFilter);
#endif

}


/********************************************************************************************
>	BOOL PaletteFilterFamily::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Returns:	TRUE if initalised ok; FALSE if error occurred.
	Purpose:	Set up a filter object.  This creates the associated OILFilter object, and
				sets up the filter ID number and strings.
	Errors:		Out of memory.
	SeeAlso:	FilterFamily; OILFilter
********************************************************************************************/
BOOL PaletteFilterFamily::Init()
{
PORTNOTE("filter","Removed OILFilter usage")
#ifndef EXCLUDE_FROM_XARALX
	// Get the OILFilter object
	pOILFilter = new OILFilterFamily(this, _R(IDT_FILTERNAME_PALETTE));
	if (pOILFilter == NULL)
		return FALSE;
#endif
	// Load the description strings
	FilterName.Load(_R(IDT_FILTERNAME_PALETTE));
	FilterInfo.Load(_R(IDT_FILTERINFO_PALETTE));
	FilterID = FILTERID_PALETTE;

	// All ok
	return TRUE;
}

#endif

/********************************************************************************************

>	SnapShotItem::SnapShotItem()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	Construct a SnapShotItem to record a list of current attributes

*********************************************************************************************/

SnapShotItem::SnapShotItem()
{
	SnapShotAttrs = NULL;
	SnapShotAttrsStack = NULL;
}


/********************************************************************************************

>	SnapShotItem::~SnapShotItem()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	Destroy a SnapShotItem - removes all allocated arrays

*********************************************************************************************/

SnapShotItem::~SnapShotItem()
{
	if (SnapShotAttrs!=NULL)
	{
		INT32 i, NumAttrs = AttributeManager::GetNumAttributes();
		for (i=0; i<NumAttrs; i++)
		{
			if (SnapShotAttrs[i] != NULL)
			{
				delete SnapShotAttrs[i];
				SnapShotAttrs[i]=NULL;
			}
		}
		CCFree(SnapShotAttrs);
		SnapShotAttrs=NULL;
	}

	if (SnapShotAttrsStack!=NULL)
	{
		CCFree(SnapShotAttrsStack);
		SnapShotAttrsStack=NULL;
	}
}


/********************************************************************************************

>	BOOL SnapShotItem::Initialise(AttributeEntry* CurrAttrs)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		CurrAttrs = a pointer to a list of attributeentry items. There
							as usual should be AttributeManager::NumAttributes() of them
	Returns:	TRUE if able to initialise this snapshot item
				FALSE if not
	Purpose:	This function grabs a snap shot of the described attributes list

*********************************************************************************************/

BOOL SnapShotItem::Initialise(AttributeEntry* CurrAttrs)
{
	INT32 i, NumAttrs = AttributeManager::GetNumAttributes();

	SnapShotAttrs = (NodeAttribute**)CCMalloc(NumAttrs*sizeof(NodeAttribute*));
	if (SnapShotAttrs==NULL)
		return FALSE;

	SnapShotAttrsStack = (AttributeEntry*)CCMalloc(NumAttrs*sizeof(AttributeEntry));
	if (SnapShotAttrsStack==NULL)
	{
		CCFree(SnapShotAttrs);
		SnapShotAttrs=NULL;
		return FALSE;
	}

	// store the temp values
	for (i=0; i<NumAttrs; i++)
	{
		// of course the following MakeNode may fail, but it just sets
		// the SnapShotAttrs ptr to NULL which we MUST check (and do) later on
		SnapShotAttrs[i] = (CurrAttrs[i].pAttr)->MakeNode();
	
		SnapShotAttrsStack[i].pAttr  = NULL;
		SnapShotAttrsStack[i].Temp   = FALSE;
		SnapShotAttrsStack[i].Ignore = TRUE;
	}

	return TRUE;
}


//********************************************************************************************
//********************************************************************************************



/********************************************************************************************

>	SnapShotList::SnapShotList()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Purpose:	Constructs a snap shot list

*********************************************************************************************/

SnapShotList::SnapShotList()
{
}


/********************************************************************************************

>	SnapShotList::~SnapShotList()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	Destroys a snapshot list. At this stage the snap shot list should be
				empty (if Create and Destroy have been matched correctly). 

*********************************************************************************************/

SnapShotList::~SnapShotList()
{
	BOOL notempty=FALSE;
	SnapShotItem* pItem=NULL;
	while ( (pItem=(SnapShotItem*)SnapShot.RemoveTail()) !=NULL )
	{
		notempty=TRUE;
		delete pItem;
	}
	if (notempty)
	{
		ERROR3("Destroyed a none empty snapshot list!\n");
	}
}


/********************************************************************************************

>	SnapShotItem* SnapShotList::GetCurrentSnapShot()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	A pointer to the current snap shot.
	Purpose:	This function is an abstracted way of finding the current snap shot
				created by the last CreateSnapShot() to be called (as snap shots can
				be stacked)

*********************************************************************************************/

SnapShotItem* SnapShotList::GetCurrentSnapShot()
{
	return (SnapShotItem*)SnapShot.GetTail();
}


/********************************************************************************************

>	BOOL SnapShotList::CreateSnapShot(AttributeEntry* CurrAttrs)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		CurrAttrs = a pointer to a list of attributeentry items. There as usual 
							should be AttributeManager::NumAttributes() of them
	Returns:	TRUE if a snap shot has been created
				FALSE if not
	Purpose:	Take a snap shot of the attribute list described by CurrAttrs

*********************************************************************************************/

BOOL SnapShotList::CreateSnapShot(AttributeEntry* CurrAttrs)
{
	SnapShotItem* pItem = new SnapShotItem;
	if (pItem==NULL)
		return FALSE;

	if (!pItem->Initialise(CurrAttrs))
	{
		delete pItem;
		return FALSE;
	}

	SnapShot.AddTail(pItem);
	return TRUE;
}


/********************************************************************************************

>	void SnapShotList::DestroySnapShot()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	Destroy the last snap shot taken by CreateSnapShot()

*********************************************************************************************/

void SnapShotList::DestroySnapShot()
{
	SnapShotItem* pItem = (SnapShotItem*)SnapShot.RemoveTail();
	if (pItem==NULL)
	{
		ERROR3("underflow in SnapShotList::DestroySnapShot()");
		return;
	}		 
		
	delete pItem;
}


/********************************************************************************************

>	BOOL SnapShotList::ApplyChangedAttrs(Node* pNode, AttributeEntry* CurrAttrs)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	TRUE if the changed attributes have been applied to pNode
				FALSE if nothing has been applied
	Purpose:	This function uses the snapshot facility to work out which of the
				current attributes have changed. Obviously it expects a snap shot to have
				been taken some time earlier. Only those attributes that have changes will be
				applied to the node described on entry. Note, any changed attributes which
				now match defaults will not be applied as the function calls
				ApplyBasedOnDefaults() with the changes list.

*********************************************************************************************/

BOOL SnapShotList::ApplyChangedAttrs(Node* pNode, AttributeEntry* CurrAttrs)
{
	// check that we have a snap shot attr mask
	SnapShotItem* pCurrItem = GetCurrentSnapShot();
	if (pCurrItem==NULL)
	{
		ERROR3("No snap shot taken of the current attributes in SnapShotList::ApplyChangedAttrs()");
		return FALSE;
	}

	// Now get a set of default attributes so we can use them to reset the
	// current values
	INT32 i, NumAttrs = AttributeManager::GetNumAttributes();
	AttributeEntry *pDefAtts = AttributeManager::GetDefaultAttributes();
	if (pDefAtts==NULL)
		return FALSE;
	
	// Alter the list of default attrs we've just created to include the differences
	// between our snap shot set and the current attributes described.
	for (i=0; i<NumAttrs; i++)
	{
		pDefAtts[i].Ignore = TRUE;
		NodeAttribute* pNodeAtt = pCurrItem->SnapShotAttrs[i];
		if (pNodeAtt)
		{
			AttributeValue* pSavedAttr = pNodeAtt->GetAttributeValue();
		
			if ((pSavedAttr!=NULL) && (CurrAttrs[i].pAttr->IsDifferent(pSavedAttr)))
			{
				pDefAtts[i].pAttr  = CurrAttrs[i].pAttr;
				pDefAtts[i].Temp   = FALSE;
				pDefAtts[i].Ignore = FALSE;
			}
		}
	}

	// Now apply the attribute list we have built
	BOOL Success = AttributeManager::ApplyBasedOnDefaults(pNode, pDefAtts);
	
	// finally free up the array we have created and return
	CCFree(pDefAtts);
	return Success;
}




/********************************************************************************************

>	void SnapShotList::PushAttrsBasedOnSnapShot(AttributeEntry* CurrAttrs)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	This function uses the snap shot facility. It checks the snap shot list
				against the current attribute list to find attributes which differ. Those
				that do are obviously new attributes. ie attributes which have been parsed
				by the filter after the snap shot was taken. This function takes the current
				attrubute list and removes all those attributes which have changed in this
				way. It resets them to their default none temp attribute types.

*********************************************************************************************/

void SnapShotList::PushAttrsBasedOnSnapShot(AttributeEntry* CurrAttrs)
{
	// check that we have a snap shot attr mask
	SnapShotItem* pCurrItem = GetCurrentSnapShot();
	if (pCurrItem==NULL)
	{
		ERROR3("No snap shot taken of the current attributes in SnapShotList::PushAttrsBasedOnSnapShot()");
		return;
	}

	// Now get a set of default attributes so we can use them to reset the
	// current values
	INT32 i, NumAttrs = AttributeManager::GetNumAttributes();
	AttributeEntry *pDefAtts = AttributeManager::GetDefaultAttributes();
	if (pDefAtts==NULL)
		return;
	
	for (i=0; i<NumAttrs; i++)
	{
		pCurrItem->SnapShotAttrsStack[i].pAttr  = NULL;
		pCurrItem->SnapShotAttrsStack[i].Temp   = FALSE;
		pCurrItem->SnapShotAttrsStack[i].Ignore = TRUE;
		
		NodeAttribute* pNodeAtt = pCurrItem->SnapShotAttrs[i];
		
		if (pNodeAtt)
		{
			AttributeValue* pSavedAttr = pNodeAtt->GetAttributeValue();
		
			if ((pSavedAttr!=NULL) && (CurrAttrs[i].pAttr->IsDifferent(pSavedAttr)))
			{
				// we don't want to apply this attr!
				pCurrItem->SnapShotAttrsStack[i].pAttr  = CurrAttrs[i].pAttr;
				pCurrItem->SnapShotAttrsStack[i].Temp	= CurrAttrs[i].Temp;
				pCurrItem->SnapShotAttrsStack[i].Ignore = CurrAttrs[i].Ignore;	
				
				CurrAttrs[i].pAttr  = pDefAtts[i].pAttr;
				CurrAttrs[i].Temp   = pDefAtts[i].Temp;
				CurrAttrs[i].Ignore = pDefAtts[i].Ignore;
			}
		}
	}

	CCFree(pDefAtts);
}



/********************************************************************************************

>	void SnapShotList::PopAttrsBasedOnSnapShot(AttributeEntry* CurrAttrs)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		-
	Returns:	-
	Purpose:	This function uses the snap shot facility. It checks the snap shot list
				against the current attribute list to find attributes which differ. Those
				that do are obviously new attributes. ie attributes which have been parsed
				by the filter after the snap shot was taken. This function takes the current
				attrubute list and removes all those attributes which have changed in this
				way. It resets them to their default none temp attribute types.

*********************************************************************************************/

void SnapShotList::PopAttrsBasedOnSnapShot(AttributeEntry* CurrAttrs)
{
	// check that we have a snap shot attr mask
	SnapShotItem* pCurrItem = GetCurrentSnapShot();
	if (pCurrItem==NULL)
	{
		ERROR3("No snap shot taken of the current attributes in SnapShotList::PopAttrsBasedOnSnapShot()");	
		return;
	}

	INT32 i, NumAttrs = AttributeManager::GetNumAttributes();
	AttributeEntry *pDefAtts = AttributeManager::GetDefaultAttributes();
	if (pDefAtts==NULL)
		return;
	
	for (i=0; i<NumAttrs; i++)
	{
		if (pCurrItem->SnapShotAttrsStack[i].pAttr != NULL)
		{
			if ( !(CurrAttrs[i].pAttr->IsDifferent(pDefAtts[i].pAttr)) )
			{			
				CurrAttrs[i].pAttr  = pCurrItem->SnapShotAttrsStack[i].pAttr;
				CurrAttrs[i].Temp   = pCurrItem->SnapShotAttrsStack[i].Temp;
				CurrAttrs[i].Ignore = pCurrItem->SnapShotAttrsStack[i].Ignore;
			}
		}
	}

	CCFree(pDefAtts);
}



/********************************************************************************************

>	BOOL Filter::SetTextFont(String_64 *EncodedFont, INT32 Millipoint)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/95
	Returns:	TRUE if the font has been set correctly
				FALSE if an error occured specifying the font
	Purpose:	Set the font specified in the encoded font name. The font which actually
				gets set in the attribute stack is one found by GetCompatibleFont(), a super
				new fontmanager call.

********************************************************************************************/

BOOL Filter::SetTextFont(String_64 *EncodedName, INT32 MillFSize)
{
	String_64 CompFont;
	INT32 Style;
	BOOL Bold,Italic;

	// Find a nice compatible font we can use
	FONTMANAGER->GetCompatibleFont(*EncodedName, CompFont, Style);
	// get the style bits
	Bold=((Style & 1) != 0);
	Italic=((Style & 2) != 0);

	return SetTextFontStyle(&CompFont, MillFSize, Bold, Italic);
}
	
/*	
// This has all moved into the fontmanager, tada!

	if (EncodedName->Sub(String_32("_"),0,0) == 0)
		EncodedName->Remove(0,1);

	String_64 TryFont((*EncodedName));
	TryFont.SwapChar('-',' ');

	// Try to find the specified font name
	INT32 found = FONTMANAGER->DoesFontExist(&TryFont,TRUE,TRUE);
	if (found>0)
		return SetTextFontStyle(&TryFont, MillFSize, FALSE, FALSE);

	// Ok, if we can't find the whole thing, try decoding
	String_64 DecodedName;
	INT32 Style = FONTMANAGER->DecodeFontName(*EncodedName, DecodedName);

	// find out what styles this font has
	BOOL Bold,Italic;
	Bold=((Style & 1) != 0);
	Italic=((Style & 2) != 0);

	// if both bold and italic found
	if (Bold && Italic)
	{
		String_64 HoldName(TryFont);
		// try to find bold first
		INT32 pos = HoldName.Sub(String_32("Italic"),0,0);
		HoldName.Remove(pos,6);
		found =  FONTMANAGER->DoesFontExist(&HoldName,TRUE,TRUE);
		if (found>0)
			return SetTextFontStyle(&HoldName, MillFSize, FALSE, TRUE);

		// Then try Italic
		HoldName=TryFont;
		pos = HoldName.Sub(String_32("Bold"),0,0);
		HoldName.Remove(pos,4);
		found =  FONTMANAGER->DoesFontExist(&HoldName,TRUE,TRUE);
		if (found>0)
			return SetTextFontStyle(&HoldName, MillFSize, TRUE, FALSE);
	}

	// Does the decoded name exist?, so we can make the rest up in styles?
	found =  FONTMANAGER->DoesFontExist(&DecodedName,TRUE,TRUE);
	if (found>0)
		return SetTextFontStyle(&DecodedName, MillFSize, Bold, Italic);

	// otherwise, we have a font with an extension, eg Trinity.Bold
	// (1) Trinity.Bold   - does not exist
	// (2) Trinity        - does not exist
	// now we set the font to (1), which will be replaced by Times-Roman.
	// Now we should also set the font styles too to give us TimesRoman bold
	return SetTextFontStyle(&TryFont, MillFSize, Bold, Italic);
}
*/

/********************************************************************************************

>	BOOL Filter::SetTextFontStyle(String_64 *FName, INT32 MillFSize, BOOL Bold, BOOL Italic)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/05/95
	Returns:	TRUE if the font and styles have been set
				FALSE if an error occured (out of memory)
	Purpose:	This function creates attributes for the typeface, the pointsize, bold and
				italic all at once.

********************************************************************************************/

BOOL Filter::SetTextFontStyle(String_64 *FName, INT32 MillFSize, BOOL Bold, BOOL Italic)
{
	if ((FONTMANAGER->CacheNamedFont(FName) == ILLEGALFHANDLE))
		return FALSE;

	if (!SetTextTypeFace(FName))
		return FALSE;

	if (!SetTextSize(MillFSize))
		return FALSE;

	// Build any style definitions we need
	if (!SetTextBold(Bold))
		return FALSE;

	if (!SetTextItalic(Italic))
		return FALSE;

	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextSize(INT32 Size)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Size - the new FontSize to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for TextSize to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextSize(INT32 Size)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTFONTSIZE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTFONTSIZE].pAttr = new TxtFontSizeAttribute(Size);
		if (CurrentAttrs[ATTR_TXTFONTSIZE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTFONTSIZE].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtFontSizeAttribute *pAttr = 
		(TxtFontSizeAttribute *) CurrentAttrs[ATTR_TXTFONTSIZE].pAttr;
		pAttr->FontSize = Size;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextTypeFace(String_64 * Name, FontClass Class=FC_UNDEFINED)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Name	- the font name to use when creating a new typeface attribute
				Class	- the class of font this name is supposed to be eg FC_TRUETYPE.
						  the class defaults to FC_UDEFINED which means the first font
						  cached which matches the font name will be used whatever
						  type of font it is.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for TextSize to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextTypeFace(String_64 * Name, FontClass Class)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	WORD hTypeface = FONTMANAGER->GetFontHandle(Name, Class);

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTFONTTYPEFACE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr = new TxtFontTypefaceAttribute(hTypeface);
		if (CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTFONTTYPEFACE].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtFontTypefaceAttribute *pAttr = (TxtFontTypefaceAttribute*) CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr;
		pAttr->HTypeface = hTypeface;
		pAttr->IsBold = FALSE;
		pAttr->IsItalic = FALSE;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextBold(BOOL Bold)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Bold - the new Bold state to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for Bold to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextBold(BOOL Bold)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTBOLD].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTBOLD].pAttr = new TxtBoldAttribute(Bold);
		if (CurrentAttrs[ATTR_TXTBOLD].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTBOLD].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtBoldAttribute *pAttr = (TxtBoldAttribute*) CurrentAttrs[ATTR_TXTBOLD].pAttr;
		pAttr->BoldOn = Bold;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextItalic(BOOL Italic)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Italic- the new Italic state to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for Italic to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextItalic(BOOL Italic)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTITALIC].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTITALIC].pAttr = new TxtItalicAttribute(Italic);
		if (CurrentAttrs[ATTR_TXTITALIC].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTITALIC].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtItalicAttribute *pAttr = (TxtItalicAttribute*) CurrentAttrs[ATTR_TXTITALIC].pAttr;
		pAttr->ItalicOn = Italic;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextUnderline(BOOL Underline)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Underline - the new Underline state to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for Underline to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextUnderline(BOOL Underline)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTUNDERLINE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTUNDERLINE].pAttr = new TxtUnderlineAttribute(Underline);
		if (CurrentAttrs[ATTR_TXTUNDERLINE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTUNDERLINE].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtUnderlineAttribute *pAttr = (TxtUnderlineAttribute*) CurrentAttrs[ATTR_TXTUNDERLINE].pAttr;
		pAttr->Underlined = Underline;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextAspectRatio(FIXED16 AspectRatio)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		AspectRatio - the new Aspect Ratio to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for AspectRatio to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextAspectRatio(FIXED16 AspectRatio)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTASPECTRATIO].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTASPECTRATIO].pAttr = new TxtAspectRatioAttribute(AspectRatio);
		if (CurrentAttrs[ATTR_TXTASPECTRATIO].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTASPECTRATIO].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtAspectRatioAttribute *pAttr = (TxtAspectRatioAttribute*) CurrentAttrs[ATTR_TXTASPECTRATIO].pAttr;
		pAttr->AspectRatio = AspectRatio;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextTracking(INT32 Tracking)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Size - the new FontSize to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for TextSize to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextTracking(INT32 Tracking)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTTRACKING].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTTRACKING].pAttr = new TxtTrackingAttribute(Tracking);
		if (CurrentAttrs[ATTR_TXTTRACKING].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTTRACKING].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtTrackingAttribute *pAttr = (TxtTrackingAttribute*) CurrentAttrs[ATTR_TXTTRACKING].pAttr;
		pAttr->Tracking = Tracking;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextJustification(Justification Justify)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Size - the new FontSize to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for TextSize to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextJustification(Justification Justify)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTJUSTIFICATION].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTJUSTIFICATION].pAttr = new TxtJustificationAttribute(Justify);
		if (CurrentAttrs[ATTR_TXTJUSTIFICATION].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTJUSTIFICATION].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtJustificationAttribute *pAttr = (TxtJustificationAttribute*) CurrentAttrs[ATTR_TXTJUSTIFICATION].pAttr;
		pAttr->justification = Justify;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextLineSpacing(	INT32 Type, 
								  		INT32 EMLSpace,
								  		MILLIPOINT MLSpace,
										double DLSpace)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		Type 		= the type of linespacing to create (0,1 or 2)
				0 EMLSpace	= linespacing in em's (units related to the current pointsize)
				1 MLSpace	= absolute millipoint line spacing
				2 DLSpace	= propoptional linespacing
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for line spacing to reflect the value passed 
				in. One of three values will be used depending on the type (as above)
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextLineSpacing(INT32 Type, 
								   INT32 EMLSpace,
								   MILLIPOINT MLSpace,
								   double DLSpace)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	INT32 LLSpace=0;
	double RLSpace=0;

	switch (Type)
	{
		case 0:
		{
			// Use the default size.
			INT32 FSize = 19200;
			// Need to convert the linespacing to a sensible millipoint value
			if (CurrentAttrs[ATTR_TXTFONTSIZE].Temp)
			{
				// We already have an attribute - just change it.
				TxtFontSizeAttribute *pAttr = (TxtFontSizeAttribute*) CurrentAttrs[ATTR_TXTFONTSIZE].pAttr;
				FSize = pAttr->FontSize;
			}
			// Convert line space to millipoints.
			double LS = (double)EMLSpace;
			LLSpace = (INT32)((LS*FSize/1000)+0.5);
			break;
		 }

		case 1:
			LLSpace = MLSpace;
			break;
		
		case 2:
			RLSpace = DLSpace;
			break;
	}

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTLINESPACE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTLINESPACE].pAttr = new TxtLineSpaceAttribute();
		if (CurrentAttrs[ATTR_TXTLINESPACE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTLINESPACE].Temp = TRUE;
	}

	// Get a pointer to the attribute and set its new values
	TxtLineSpaceAttribute *pAttr = (TxtLineSpaceAttribute*) CurrentAttrs[ATTR_TXTLINESPACE].pAttr;

	switch (Type)
	{
		case 0:
		case 1:
			pAttr->IsRatio = FALSE;
			pAttr->Value = LLSpace;
			pAttr->Ratio = FIXED16(0);
			break;
		
		case 2:
			pAttr->IsRatio = TRUE;
			pAttr->Value = 0;
			pAttr->Ratio = RLSpace;
			break;
	}	

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextBaseLine(MILLIPOINT BaseShift)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		BaseShift - the new baseline shift to use.
	Returns:	TRUE if the attribute was changed ok; 
				FALSE otherwise.
	Purpose:	Updates the current attribute for BaseLine shift to reflect the value passed 
				in.
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextBaseLine(MILLIPOINT BaseShift)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTBASELINE].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTBASELINE].pAttr = new TxtBaseLineAttribute(BaseShift);
		if (CurrentAttrs[ATTR_TXTBASELINE].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTBASELINE].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		TxtBaseLineAttribute *pAttr = (TxtBaseLineAttribute*) CurrentAttrs[ATTR_TXTBASELINE].pAttr;
		pAttr->Value = BaseShift;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextScript(INT32 rise, INT32 pointsize)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/94
	Inputs:		rise = signed rise in millipoints away from the baseline
	        	pointsize = size of font to use in millipoints 
	Returns		TRUE if the attribute was changed ok
				FALSE otherwise.
	Purpose:	Updates the current attribute for subscript or superscript
	Errors:		Out of memory.
	SeeAlso:	Filter; Filter::Init

********************************************************************************************/

BOOL Filter::SetTextScript(INT32 rise, INT32 pointsize)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// ignore any crazy values
	if (pointsize<=0)
		return TRUE;

	// Use the default size.
	INT32 fsize = 19200;
	if (CurrentAttrs[ATTR_TXTFONTSIZE].Temp)
	{
		TxtFontSizeAttribute *pAttr = (TxtFontSizeAttribute*) CurrentAttrs[ATTR_TXTFONTSIZE].pAttr;
		fsize = pAttr->FontSize;
	}

	double tr = (double)rise/(double)fsize;
	double tp = (double)pointsize/(double)fsize;
	FIXED16 offset(tr);
	FIXED16 size(tp);

	// If we haven't changed this attribute yet, then make a new attribute object for
	// our own personal use...
	if (!CurrentAttrs[ATTR_TXTSCRIPT].Temp)
	{
		// Make the correct attribute value and install it as the current one.
		CurrentAttrs[ATTR_TXTSCRIPT].pAttr = new TxtScriptAttribute(offset,size);
		if (CurrentAttrs[ATTR_TXTSCRIPT].pAttr == NULL)
			return FALSE;
		CurrentAttrs[ATTR_TXTSCRIPT].Temp = TRUE;
	}
	else
	{
		// We already have an attribute - just change it.
		CurrentAttrs[ATTR_TXTSCRIPT].Ignore = FALSE;
		TxtScriptAttribute *pAttr = (TxtScriptAttribute*) CurrentAttrs[ATTR_TXTSCRIPT].pAttr;
		pAttr->Offset = offset;
		pAttr->Size = size;
	}

	// All worked ok.
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::RemoveTextScript()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/04/94
	Inputs:		-
	Returns		TRUE if the attribute was removed
				FALSE otherwise.
	Purpose:	Removes any script attribute which is currently active
	Errors:		Out of memory.
	SeeAlso:	

********************************************************************************************/

BOOL Filter::RemoveTextScript()
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// if a script attribute exists, delete it.
	if (CurrentAttrs[ATTR_TXTSCRIPT].Temp)
		CurrentAttrs[ATTR_TXTSCRIPT].Ignore = TRUE;
	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextBoldFont(BOOL Bold)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/95
	Returns:	TRUE if the font and style has been set
				FALSE if an error occured (out current typeface)
	Purpose:	This function simply sets the bold style flag in the current
				font typeface attribute (to be applied to the next created character).

********************************************************************************************/

BOOL Filter::SetTextBoldFont(BOOL Bold)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If there is no current typeface then we cannot set its style
	if (!CurrentAttrs[ATTR_TXTFONTTYPEFACE].Temp)
		return FALSE;

	// There is an attribute - just change it.
	TxtFontTypefaceAttribute *pAttr = (TxtFontTypefaceAttribute*) CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr;
	pAttr->IsBold = Bold;

	return TRUE;
}


/********************************************************************************************

>	BOOL Filter::SetTextItalicFont(BOOL Italic)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/95
	Returns:	TRUE if the font and style has been set
				FALSE if an error occured (no current typeface)
	Purpose:	This function simply sets the italic style flag in the current
				font typeface attribute (to be applied to the next created character).

********************************************************************************************/

BOOL Filter::SetTextItalicFont(BOOL Italic)
{
	// Sanity check
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No current attributes in filter!");

	// If there is no current typeface then we cannot set its style
	if (!CurrentAttrs[ATTR_TXTFONTTYPEFACE].Temp)
		return FALSE;

	// There is an attribute - just change it.
	TxtFontTypefaceAttribute *pAttr = (TxtFontTypefaceAttribute*) CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr;
	pAttr->IsItalic = Italic;

	return TRUE;
}



/********************************************************************************************
>	Filter* FilterManager::FindFilterFromID(const UINT32 FilterID) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (moved into this class by Graham 7/7/97)
	Created:	10/12/96
	Purpose:	Provides the Filter given its ID

				(This function is also in Filter Manager. Unfortunately, having it there
				means that every time you use it you have to get a pointer to the
				application, then use that to get a pointer to the filter manager, then
				use that pointer to call this function. So I've copied it here for
				convenience)
********************************************************************************************/

Filter* Filter::FindFilterFromID(const UINT32 FilterID)
{
	Filter			   *pFilter;
	for( pFilter = Filter::GetFirst();
		 pFilter != 0;
		 pFilter = Filter::GetNext(pFilter))
			if (pFilter->FilterID == FilterID)
			{
				ERROR3IF(FilterID == FILTERID_NONE,
							"Filter::FindFilterFromID: filter has _R(ID_NONE)?");
				break;
			}

	return pFilter;
}



/*********************************************************************************************
>	AttrRecordItem::AttrRecordItem()

	Author: 	Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/95
	Purpose:	Constructor - initalises member variables
**********************************************************************************************/  
AttrRecordItem::AttrRecordItem()
{
	RecordedAttrs = NULL;
}


/*********************************************************************************************
>    AttrRecordItem::~AttrRecordItem()

     Author: 	Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
     Created:   1/12/95
     Purpose:	Destructor - deletes any claimed memory
**********************************************************************************************/  
AttrRecordItem::~AttrRecordItem()
{
	// Free the AttributeEntry array
	if (RecordedAttrs != NULL)
		CCFree(RecordedAttrs);
}


/*********************************************************************************************
>	BOOL AttrRecordItem::Initialise(AttributeEntry* pCurrAttrs)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/95
	Inputs:		pCurrAttrs - an AttributeEntry array
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Copies the current AttributeValues in the AttributeEntry array so the current
				state can be restored at a later date
**********************************************************************************************/  
BOOL AttrRecordItem::Initialise(AttributeEntry* pCurrAttrs)
{
	const INT32 NumAttrs = AttributeManager::GetNumAttributes();

	// Claim memory for copy of AttributeEntry array
	RecordedAttrs = (AttributeEntry*)CCMalloc(NumAttrs*sizeof(AttributeEntry));
	if (RecordedAttrs==NULL)
		return FALSE;

	// store the temp values
	for (INT32 i=0; i<NumAttrs; i++)
	{
		// Get the runtime class info on this object and create another object of that type
		CCRuntimeClass *pCCRuntimeClass = pCurrAttrs[i].pAttr->GetRuntimeClass();
		AttributeValue *pNewAttr = (AttributeValue *) pCCRuntimeClass->CreateObject();
		if (pNewAttr == NULL)
			return FALSE;

		// Object created ok - get the object to copy its contents across.
		pNewAttr->SimpleCopy(pCurrAttrs[i].pAttr);
		RecordedAttrs[i].pAttr = pNewAttr;
	}

	return TRUE;
}


/*********************************************************************************************
>	BOOL AttrRecordItem::Restore(AttributeEntry* pCurrAttrs)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/95
	Inputs:		pCurrAttrs - an AttributeEntry array
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Copies the stored attribute state back into the current attribute array
**********************************************************************************************/  
BOOL AttrRecordItem::Restore(AttributeEntry* pCurrAttrs)
{
	const INT32 NumAttrs = AttributeManager::GetNumAttributes();

	for (INT32 i=0; i<NumAttrs; i++)
	{
		if (RecordedAttrs[i].pAttr != NULL)
		{
			pCurrAttrs[i].pAttr->SimpleCopy(RecordedAttrs[i].pAttr);
			delete RecordedAttrs[i].pAttr;
			RecordedAttrs[i].pAttr = NULL;
		}
	}

	return TRUE;
}


/*********************************************************************************************
>	AttrRecordList::AttrRecordList()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/95
	Purpose:	Constructor - initalises member variables
**********************************************************************************************/  
AttrRecordList::AttrRecordList()
{
}


/*********************************************************************************************
>    AttrRecordItem::~AttrRecordItem()

     Author: 	Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
     Created:   1/12/95
     Purpose:	Destructor - deletes any claimed memory
**********************************************************************************************/  
AttrRecordList::~AttrRecordList()
{
}


/*********************************************************************************************
>	BOOL AttrRecordList::SaveContext(AttributeEntry* pCurrAttrs)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/95
	Inputs:		pCurrAttrs - an AttributeEntry array
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Stores the current state of the attribute entry array
**********************************************************************************************/  
BOOL AttrRecordList::SaveContext(AttributeEntry* pCurrAttrs)
{
	// Create a new record
	AttrRecordItem* pItem = new AttrRecordItem;
	if (pItem==NULL)
		return FALSE;

	// Copy the attributes into it
	if (!pItem->Initialise(pCurrAttrs))
	{
		delete pItem;
		return FALSE;
	}
	else
	{
		AttrRecord.AddTail(pItem);
	 	return TRUE;
	}
}


/*********************************************************************************************
>	BOOL AttrRecordList::RestoreContext(AttributeEntry* pCurrAttrs)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/12/95
	Inputs:		pCurrAttrs - an AttributeEntry array
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Returns the current attribute array back to the stored state
**********************************************************************************************/  
BOOL AttrRecordList::RestoreContext(AttributeEntry* pCurrAttrs)
{
	AttrRecordItem* pItem = (AttrRecordItem*)AttrRecord.RemoveTail();
	ERROR2IF(pItem == NULL, FALSE, "Record list empty");
	pItem->Restore(pCurrAttrs);
	delete pItem;

	return TRUE;
}
