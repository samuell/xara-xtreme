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

// The module that controls the OIL side of import/export filters.


#include "camtypes.h"

#include "pathname.h"
#include "document.h"
#include "ensure.h"
#include "errors.h"
//#include "tim.h"
//#include "rik.h"
#include "app.h"
//#include "accures.h"		// General Accusoft DLL strings
//#include "nev.h"			// _R(IDN_USER_CANCELLED), _R(IDN_FILTERNOTPRESENT), _R(IDS_OPMENUIMPORT_ONE), _R(IDS_OPMENUEXPORT_ONE)
#ifndef WEBSTER
//#include "extfilts.h"		// Accusoft filters
#endif //WEBSTER
//#include "giffiltr.h"		// Transparent and interlaced GIF filter - now in filters.cpp
#include "optsmsgs.h"		// options messages IMPORTWITHLAYERS
//#include "mainfrm.h"
#include "camelot.h"
#include "coplfilr.h"		// !!! this MUST occur before any JPEG stuff 'cos of MAX_COMPONENTS define/enum clash
#include "freeeps.h"
//#include "ben.h"
#include "cameleps.h"
//#include "richard.h"		// for sprite ID's
//#include "peter.h"
//#include "sprfilt.h"		// for sprite filter class
//#include "ppmfiltr.h"		// for PPM filter class
#include "textfltr.h"
#include "textfuns.h"
#include "unicdman.h"
#include "fontman.h"
//#include "mario.h"
//#include "filtrres.h"		// for WEB and PNG names
#include "pngfiltr.h"		// for PNG filter class
#include "imjpeg.h"			// for JPEGImportFilter
//#include "exjpeg.h"			// for JPEGExportFilter
#include "filtimag.h"		// Imagemap filter
//#include "resimmap.h"		//Imagemap resources
#include "prvwflt.h"		// for preview filters

#include "oilfltrs.h"
//#include "metafilt.h"
//#include "enhmeta.h"

//#include "filedlgs.h"		// for ImportFileDialog & ExportFileDialog
#include "helpuser.h"

#include "xpfilter.h"
//#include <atlbase.h>

CC_IMPLEMENT_MEMDUMP(OILFilter, CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(OILFilterFamily, OILFilter)
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
CC_IMPLEMENT_MEMDUMP(MetaFileFilter, Filter)
//CC_IMPLEMENT_MEMDUMP(AldusFormatFilter, MetaFileFilter )
#endif

/********************************************************************************************

>	OILFilter::OILFilter(Filter *pFilter)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Inputs:		pFilter - the filter that this OILFilter object belongs to.
	Purpose:	Initialise an OILFilter object, and link it to its parent Filter.
	Errors:		ENSURe failure if pFilter is NULL
	SeeAlso:	-

********************************************************************************************/

OILFilter::OILFilter(Filter *pFilter)
{
	ENSURE(pFilter != NULL, "No parent filter for OILFilter object");

	Position = -1; 
	Parent = pFilter;
}


/********************************************************************************************
// Macro borrowed from Filters.cpp
********************************************************************************************/
#define ADD_FILTER(Classname) \
	/* Create, initialise and install the Generic import filter */					\
	pFilter = new Classname;														\
	/* Error state already set by new */											\
	if (pFilter == NULL) return FALSE;												\
																					\
	if (!pFilter->Init())															\
	{																				\
		/* Error occured - report it and stop trying to initialise filters. */		\
		if (Error::GetErrorNumber() != _R(IDN_FILTERNOTPRESENT))						\
			InformError();															\
		return TRUE;																\
	}																				\
																					\
	FilterList.AddTail(pFilter);

/********************************************************************************************

>	BOOL OILFilter::InitFilters(List& FilterList)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Inputs:		The list of filters to which the OILy filters should be added.
	Outputs:	The updated list of filters.
	Returns:	TRUE if initialised ok, FALSE if not.
	Purpose:	Scans all OILy filters to find out which filters are available for use, and
				instantiates one of each, and holds them in a list.
	Errors:		Out of memory (if a filter fails to initialise for other reasons then an 
				error is reported, but Camelot will still start up ok, i.e. TRUE is 
				returned).
	SeeAlso:	Filter::InitFilters

********************************************************************************************/

BOOL OILFilter::InitFilters(List& FilterList)
{
	// Find the OILy filters
	Filter *pFilter;

#if !defined(EXCLUDE_FROM_RALPH)

	if (!CreatePluginFilters(FilterList))
	{
		// Error has already been set
		return(FALSE);
	}

#if !defined(EXCLUDE_FROM_XARALX)
	// Create, initialise and install the Windows MetaFile import filter
	ADD_FILTER(MetaFileFilter)

	// If the OS can do enhanced metafiles then do them
	if (IsWin32NT() || IsWin32c())
	{
		ADD_FILTER(EnhancedMetaFilter)
	}

	ADD_FILTER(MSPaletteFilter)
	ADD_FILTER(PaintShopProPaletteFilter)

//	AldusFormatFilter::InitAll(FilterList);

	// Call the init function of the main external filters class so that the Accusoft DLL
	// gets loaded. Acts as a dummy filter that can neither import nor export.
	// If it gets the DLL then it initialises each of the filters otherwise it will exit
	// from the function immediately.

	// WEBSTER-Martin-19/12/96 no Accusoft
#ifndef WEBSTER
	ADD_FILTER(AccusoftFilters)

	// Create, initialise and install the external filters supplied by the Accusoft software.
	// These are at present supplied in the Intel and Mac directories as we have Intel versions
	// but no MAC versions.	The Mac versions just do nothing at present except return FALSE.
	// Hence the first filter will fail and we will immediately return TRUE to the caller.
	ADD_FILTER(AccusoftBMPFilter)					// Mac version fails and returns here
	ADD_FILTER(BrookTroutFilter)					
	ADD_FILTER(CALSFilter)
	ADD_FILTER(DCXFilter)
	ADD_FILTER(GX2Filter)
	ADD_FILTER(IFFFilter)
	ADD_FILTER(IMGFilter)
	ADD_FILTER(IOCAFilter)
	ADD_FILTER(HaloCUTFilter)
	ADD_FILTER(KOFAXFilter)
	ADD_FILTER(LaserDataFilter)
	ADD_FILTER(MacPaintFilter)
	ADD_FILTER(MSIconsFilter)				
	ADD_FILTER(MSPaintFilter)
	ADD_FILTER(PCXFilter)
	ADD_FILTER(PICTFilter)
	ADD_FILTER(PhotoCDFilter)
	ADD_FILTER(PhotoShopFilter)
	ADD_FILTER(SUNFilter)
	ADD_FILTER(TARGAFilter)
	ADD_FILTER(WPGFilter)
	ADD_FILTER(XWindowsFilter)

	// Preview filters
	ADD_FILTER(AccPreviewFilter)
	ADD_FILTER(PreviewFilterTIFF)	//this is still an Accusoft filter

#endif //WEBSTER

	ADD_FILTER(PPMFilter)
	ADD_FILTER(PGMFilter)
	ADD_FILTER(PBMFilter)
	ADD_FILTER(SpriteFilter)

//WEBSTER-Martin-19/12/96
// possibly these next ones should now be in Filter::InitFilters as it's native code now

#endif	// EXCLUDE_FROM_XARALX

#if defined(EXCLUDE_FROM_XARALX)
	ADD_FILTER(JPEGImportFilter)
#endif

	// Add the Preview Bitmap Filter
	// Tricky one this. Don't really need this any more (14/3/97)- its only use is the 
	// PreviewFilter::PreviewBitmapSize static that has historically been used all over the place
	ADD_FILTER(PreviewFilter)

	// Add the preview bitmap filters that we can use in the new native/web format
	ADD_FILTER(PreviewFilterBMP)
#if !defined(EXCLUDE_FROM_XARALX)
	ADD_FILTER(PreviewFilterGIF)
#endif
	ADD_FILTER(PreviewFilterJPEG)
	ADD_FILTER(PreviewFilterPNG)
	ADD_FILTER(ThumbnailFilterPNG)

#endif

	// All ok
	return TRUE;
}



/****************************************************************************

>	BOOL OILFilter::CreatePluginFilters(List& FilterList)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/2005

	Inputs:		FilterList	- 
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Creates a filter object for each installed plugin filter

****************************************************************************/

BOOL OILFilter::CreatePluginFilters(List& FilterList)
{
	PORTNOTETRACE("filters","OILFilter::CreatePluginFilters - bodged");

	CLSID Clsid;
	PluginNativeFilter* pFilter = new PluginNativeFilter;
	if (pFilter == NULL)
		return FALSE;

	if (pFilter->Init(Clsid))
		FilterList.AddTail(pFilter);
	else
		delete pFilter;

#ifndef EXCLUDE_FROM_XARALX
	// Iterate through the component category adding each filter to the filter list
	HRESULT hRes = S_OK;

	CComPtr<ICatInformation> pCatInformer;
	hRes = pCatInformer.CoCreateInstance(CLSID_StdComponentCategoriesMgr);
	if (FAILED(hRes))
	{
		// Return an error here
		return false;
	}

	CComPtr<IEnumCLSID> pEnumCLSID;
	CATID Categories[2] = {
		{0x42F818E1, 0x9EF6, 0x4241, {0x90, 0x9B, 0x91, 0xE7, 0x83, 0xB9, 0xB9, 0x35}},
		{0x42F818E1, 0x9EF6, 0x4241, {0x90, 0x9B, 0x91, 0xE7, 0x83, 0xB9, 0xB9, 0x36}}
	};
	hRes = pCatInformer->EnumClassesOfCategories(2, Categories, (UINT32)-1, NULL, &pEnumCLSID);
	if (FAILED(hRes))
	{
		// Return an error here
		return false;
	}

	UINT32 NumRead = 1;
	CLSID Clsid;
	while (NumRead != 0)
	{
		NumRead = 0;
		hRes = pEnumCLSID->Next(1, &Clsid, &NumRead);
		if (FAILED(hRes))
			break;

		if (NumRead > 0)
		{
			PluginNativeFilter* pFilter = new PluginNativeFilter;
			if (pFilter == NULL) return FALSE;

			if (pFilter->Init(Clsid))
				FilterList.AddTail(pFilter);
			else
				delete pFilter;
		}
	}
#endif
	return TRUE;
}



/********************************************************************************************

>	char *OILFilter::ConstructFilterString(UINT32 NumberToExport = 1)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		NumberToExport - The number of files being exported (allows different names)
	Returns:	Pointer to the string for this filter.  This is a pointer to a static
				buffer, so it should be used immediately, and the caller should not
				attempt to deallocate this string.
	Purpose:	Construct a COMMDLG filter definition string to put in the drop-down
				list of filters in the dialog box.
	SeeAlso:	OILFilter; OpMenuImport; OpMenuExport

********************************************************************************************/

TCHAR *OILFilter::ConstructFilterString(UINT32 NumberToExport)
{
	// Load in the filter name and extensions...
	static String_128 FilterStr;
	String_32 Extensions;

	FilterStr = GetFilterName(NumberToExport);

	// Construct the filter string from these two strings:
	FilterStr += _T(" (");

	// Extract each extension and see if we need to add it
	String_32 ExtStr;
	ExtStr = FilterExt;

	TCHAR*	pPointer;
	TCHAR*	pExt = camStrtok( (TCHAR*)ExtStr, _T(","), &pPointer );
	BOOL NoneFound = TRUE;

	while (pExt != NULL)
	{
		if (camStrstr((TCHAR *) Extensions, pExt) == NULL)
		{
			// Not already present - add the string.
			if (!NoneFound)
				// Don't add a semi-colon if this is the first one we find.
				Extensions += _T(";");
			Extensions += _T("*.");
			Extensions += pExt;
			NoneFound = FALSE;
		}

		pExt = camStrtok( NULL, _T(","), &pPointer );
	}

	FilterStr += Extensions;
	FilterStr += _T(") |");
	FilterStr += Extensions;

	// Return a pointer to the string.
	return (TCHAR *) FilterStr;
}



/********************************************************************************************
>	String_64 OILFilter::GetFilterName(UINT32 NumberToExport)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		NumberToExport - the number of files being exported.
	Returns:	The name of the filter
	Purpose:	Allows derived classes to supply differnt filter names
********************************************************************************************/
String_64 OILFilter::GetFilterName(UINT32 NumberToExport)
{
	return FilterName;
}



/********************************************************************************************

>	OILFilterFamily::OILFilterFamily(Filter *pFilter, UINT32 NameID)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/94
	Inputs:		pFilter - the parent filter of this OILFilter
				NameID - the string that describes this filter family.
	Purpose:	Construct the OIL part of a filter family.  This is basically the
				string resource that is the name of the filter, that appears in the
				COMMDLG import dialog box.
	SeeAlso:	OILFilter

********************************************************************************************/

OILFilterFamily::OILFilterFamily(Filter *pFilter, UINT32 NameID) : OILFilter(pFilter)
{
	FilterName.Load( NameID );
}


/********************************************************************************************

>	char *OILFilterFamily::ConstructFilterString(UINT32 NumberToExport = 1)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Returns:	Pointer to the string for this filter.  This is a pointer to a static
				buffer, so it should be used immediately, and the caller should not
				attempt to deallocate this string.
	Purpose:	Construct a COMMDLG filter definition string to put in the drop-down
				list of filters in the dialog box. This version gathers together all the 
				extensions for a filter family.
	SeeAlso:	OILFilter; OpMenuImport; OpMenuExport

********************************************************************************************/

TCHAR *OILFilterFamily::ConstructFilterString(UINT32 NumberToExport)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Load in the filter name and extensions...
	static String_256 FilterStr;
	String_256 Extensions;
	FilterStr = FilterName;

	// Go through the filters and find out which ones to put in this family.
	ENSURE(Parent->IsKindOf(CC_RUNTIME_CLASS(FilterFamily)), "Bad parent filter");

	if (IS_A(Parent, GenericFilter))
	{
		// It's the generic filter - just use "*.*" as the extension mask.
		Extensions = "*.*";
	}
	else
	{
		// Run through all the filters and gather up all their extensions into a list.
		CCRuntimeClass *pFilterType = ((FilterFamily *) Parent)->pFilterType;

		Filter *pFilter = Filter::GetFirst();

		BOOL NoneFound = TRUE;

		while (pFilter != NULL)
		{
			if (pFilter->GetFlags().CanImport && pFilter->JoinFamily(pFilterType) &&
				pFilter->GetFlags().ShowFilter)
			{
				// This is an import filter which belongs to our family - add its
				// extension list (unless it is already in out list).
				String_32 ExtStr;
				ExtStr = pFilter->pOILFilter->FilterExt;

				// Extract each extension and see if we need to add it
				char *pExt = _tcstok((TCHAR *) ExtStr, ",");

				while (pExt != NULL)
				{
					if (camStrstr((TCHAR *) Extensions, pExt) == NULL)
					{
						// Not already present - add the string.
						if (!NoneFound)
							// Don't add a semi-colon if this is the first one we find.
							Extensions += ";";
						Extensions += "*.";
						Extensions += pExt;
						NoneFound = FALSE;
					}

					pExt = _tcstok(NULL, ",");
				}
			}

			// Get the next filter.
			pFilter = Filter::GetNext(pFilter);
		}
	}


	// Construct the filter string from these two strings:
	FilterStr += "|";
	FilterStr += Extensions;

	// Return a pointer to the string.
	return (TCHAR *) FilterStr;
#else
	return NULL;
#endif
}




/********************************************************************************************

>	BOOL OILFilter::DoesExtensionOfFileMatch(CCLexFile *File)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/95
	Purpose:	Sees if the extension of the given file matches one of the extensions
				given in the filter extension string. Used for ensuring a good match in
				filters.cpp

********************************************************************************************/

BOOL OILFilter::DoesExtensionOfFileMatch(CCLexFile *File)
{
	ERROR2IF(File == NULL,FALSE,"OILFilter::DoesExtensionOfFileMatch File supplied is null");

TRACEUSER( "Ben", _T("Checking extension for filter %s\n"), (TCHAR *)FilterName);
	if(File->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
	{
		// check the filename
		CCDiskFile *pDiskFile = (CCDiskFile *)File;
		PathName Path;

		Path = pDiskFile->GetPathName();

		return DoesExtensionOfPathNameMatch(&Path);
	}

	return FALSE;
}


/********************************************************************************************

>	BOOL OILFilter::DoesExtensionOfPathNameMatch(PathName *Path)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/05/95
	Purpose:	Sees if the extension of the given pathname matches one of the extensions
				given in the filter extension string. Used for ensuring a good match in
				filters.cpp and stuff.

********************************************************************************************/

BOOL OILFilter::DoesExtensionOfPathNameMatch(PathName *Path)
{
	ERROR2IF(Path == NULL,FALSE,"OILFilter::DoesExtensionOfPathNameMatch Path supplied is null");

	String_256 Ext;
	TCHAR *fe;
	TCHAR *pex;
	TCHAR ex[256];

	Ext = Path->GetType();

	// Retrieve a copy of the filter extension(s) defined to this filter 
	fe = FilterExt;
	pex = Ext;

	INT32 l = 0;
	while(pex[l] != '\0')
	{
		ex[l] = camTolower(pex[l]);
		l++;
	}
	ex[l] = '\0';

TRACEUSER( "Ben", _T("Checking extension matches, of file = '%s', supported = '%s'\nLard\n"), (TCHAR *)ex, fe);
	if(ex[0] == '\0')
	{
		// it's a blank extension. It can't match then, can it?
TRACEUSER( "Ben", _T("File didn't have an extension\n"));
		return FALSE;
	}

	// see if the extension is in the list of supported extensions in the filter
	if(camStrstr(fe, (ex[0] == '.')?(ex + 1):ex) != NULL)
		return TRUE;

	return FALSE;
}



/********************************************************************************************

>	BOOL OILFilter::FixExtensionOfPathName(PathName *Path)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/01/96
	Outputs:	Path	the pathname of the file to be fixed
	Returns:	True if replace the extension ok, False otherwise.
	Purpose:	Replace the extension in the pathname supplied with one for this filter.
				If more than one extension exists then replace with the first available one.

********************************************************************************************/
BOOL OILFilter::FixExtensionOfPathName(PathName *Path)
{
	ERROR2IF(Path == NULL,FALSE,"OILFilter::FixExtensionOfPathName Path supplied is null");

	// Retrieve a copy of the filter extension(s) defined to this filter 
	String_256 ExtStr = FilterExt;
		
	// If there is no filter extension string then all we can do is return False
	if (ExtStr.IsEmpty())
		return FALSE;

	// Filters might have multiple extensions listed separated by commas
	// so use first e.g. AI eps filter has ai and eps.
	// Work out the position of the first separator character, - 1 if none
	INT32	Position;
	String_8 SepMark = _T(",");
	Position = ExtStr.Sub(SepMark);
	// If comma separator was found use up to the separator 
	if (Position > 0)
		ExtStr.Left(&ExtStr, Position);

	// Put the found extension string in the path we were told to fix
	Path->SetType(ExtStr);
	
	return FALSE;
}


#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/********************************************************************************************
***** Definition of the OILy parts for individual filters								*****
********************************************************************************************/



/********************************************************************************************

>	AIEPSOILFilter::AIEPSOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

AIEPSOILFilter::AIEPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_AIEPS));
	FilterExt.Load(_R(IDT_FILTEREXT_AIEPS));
}

/********************************************************************************************

>	AI5EPSOILFilter::AI5EPSOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

AI5EPSOILFilter::AI5EPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_AI5EPS));
	FilterExt.Load(_R(IDT_FILTEREXT_AIEPS));
}

/********************************************************************************************

>	AI8EPSOILFilter::AI8EPSOILFilter()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/00
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/
AI8EPSOILFilter::AI8EPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_AI8EPS));
	FilterExt.Load(_R(IDT_FILTEREXT_AIEPS));
}

#endif

PORTNOTE("EpsFilter", "Removed use of EPS filters")
#if !defined(EXCLUDE_FROM_XARALX)
/********************************************************************************************

>	ArtWorksEPSOILFilter::EPSOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

ArtWorksEPSOILFilter::ArtWorksEPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_AWEPS));
	FilterExt.Load(_R(IDT_FILTEREXT_EPS));
}

/********************************************************************************************

>	CamelotEPSOILFilter::EPSOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CamelotEPSOILFilter::CamelotEPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_CAMEPS));
	FilterExt.Load(_R(IDT_FILTEREXT_EPS));
}



/********************************************************************************************

>	NativeEPSOILFilter::NativeEPSOILFilter(Filter* pFilter)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		pFilter - The Filter
	Purpose:	Constructs to oily parts of the Native File Format Filter (ie the list of
				File Extensions that this filter understands)

********************************************************************************************/

NativeEPSOILFilter::NativeEPSOILFilter(Filter* pFilter) : CamelotEPSOILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_NATIVE_EPS));
	FilterExt.Load(_R(IDS_FILTEREXT_NATIVE));
}

#endif



#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/********************************************************************************************

>	Corel3EPSOILFilter::EPSOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

Corel3EPSOILFilter::Corel3EPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_COREL3EPS));
	FilterExt.Load(_R(IDT_FILTEREXT_EPS));
}

/********************************************************************************************

>	Corel4EPSOILFilter::EPSOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

Corel4EPSOILFilter::Corel4EPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_COREL4EPS));
	FilterExt.Load(_R(IDT_FILTEREXT_EPS));
}

/********************************************************************************************

>	DrawOILFilter::DrawOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

DrawOILFilter::DrawOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_ACORNDRAW));
	FilterExt.Load(_R(IDT_FILTEREXT_ACORNDRAW));
}

/********************************************************************************************

>	FlashOILFilter::FlashOILFilter()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/99
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

FlashOILFilter::FlashOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_FLASH));
	FilterExt.Load(_R(IDT_FILTEREXT_FLASH));
}

/********************************************************************************************

>	PhotoShopEPSOILFilter::PhotoShopEPSOILFilter()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PhotoShopEPSOILFilter::PhotoShopEPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_PHOTOSHOPEPS_FILTERNAME));
	FilterExt.Load(_R(IDT_FILTEREXT_PHOTOSHOPEPS));
}


#endif

/********************************************************************************************

>	CorelPaletteFileOILFilter::CorelPaletteFileOILFilter(Filter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CorelPaletteFileOILFilter::CorelPaletteFileOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_CORELPALETTEFILE));
	FilterExt.Load(_R(IDT_FILTEREXT_CORELPALETTEFILE));
}

/********************************************************************************************
>	MSPaletteOILFilter::MSPaletteOILFilter(Filter *pFilter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
MSPaletteOILFilter::MSPaletteOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_WINDOWSPALETTE));
	FilterExt.Load(_R(IDT_FILTEREXT_WINDOWSPALETTE));
}



/********************************************************************************************
>	AdobeColourTableOILFilter::AdobeColourTableOILFilter(Filter *pFilter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
AdobeColourTableOILFilter::AdobeColourTableOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_ADOBECOLOURTABLE));
	FilterExt.Load(_R(IDT_FILTEREXT_ADOBECOLOURTABLE));
}

/********************************************************************************************
>	INT32 AdobeColourTableOILFilter::HowCompatible(PathName& Filename)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Purpose:	Retuns a compatibility number based on the file extension
********************************************************************************************/
INT32 AdobeColourTableOILFilter::HowCompatible(PathName& Filename)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (DoesExtensionOfPathNameMatch(&Filename))
		return 9;
	else
		return 0;
}

/********************************************************************************************
>	AdobeColourSwatchOILFilter::AdobeColourSwatchOILFilter(Filter *pFilter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
AdobeColourSwatchOILFilter::AdobeColourSwatchOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_ADOBECOLOURSWATCH));
	FilterExt.Load(_R(IDT_FILTEREXT_ADOBECOLOURSWATCH));
}

/********************************************************************************************
>	INT32 AdobeColourSwatchOILFilter::HowCompatible(PathName& Filename)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Purpose:	Retuns a compatibility number based on the file extension
********************************************************************************************/
INT32 AdobeColourSwatchOILFilter::HowCompatible(PathName& Filename)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (DoesExtensionOfPathNameMatch(&Filename))
		return 9;
	else
		return 0;
}





/********************************************************************************************
>	PSPPaletteOILFilter::PSPPaletteOILFilter(Filter *pFilter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
PSPPaletteOILFilter::PSPPaletteOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_PSPPALETTE));
	FilterExt.Load(_R(IDT_FILTEREXT_PSPPALETTE));
}

/********************************************************************************************
>	JCWColourOILFilter::JCWColourOILFilter(Filter *pFilter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
JCWColourOILFilter::JCWColourOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_JCWPALETTE));
	FilterExt.Load(_R(IDT_FILTEREXT_JCWPALETTE));
}

/********************************************************************************************
>	INT32 JCWColourOILFilter::HowCompatible(PathName& Filename)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Purpose:	Retuns a compatibility number based on the file extension
********************************************************************************************/
INT32 JCWColourOILFilter::HowCompatible(PathName& Filename)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	if (DoesExtensionOfPathNameMatch(&Filename))
		return 9;
	else
		return 0;
}


/********************************************************************************************

>	CDROILFilter::CDROILFilter(Filter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21 03 95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CDROILFilter::CDROILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_CDR));
	FilterExt.Load(_R(IDT_FILTEREXT_CDR));
}

/********************************************************************************************

>	CMX32OILFilter::CMXOILFilter(Filter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/96
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CMXImportOILFilter::CMXImportOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	PORTNOTETRACE("filters","CMXImportOILFilter::CMXImportOILFilter - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	FilterName.Load(_R(IDT_FILTERNAME_CMXIMPORT));
	FilterExt.Load(_R(IDT_FILTEREXT_CMX));
#endif
}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/********************************************************************************************

>	CMX16OILFilter::CMXOILFilter(Filter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/96
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CMX16OILFilter::CMX16OILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_CMX16));
	FilterExt.Load(_R(IDT_FILTEREXT_CMX));
}

/********************************************************************************************

>	CMX32OILFilter::CMXOILFilter(Filter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/96
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CMX32OILFilter::CMX32OILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_CMX32));
	FilterExt.Load(_R(IDT_FILTEREXT_CMX));
}

/********************************************************************************************

>	FreeHandEPSOILFilter::FreeHandEPSOILFilter(Filter *pFilter)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

FreeHandEPSOILFilter::FreeHandEPSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FREEHANDEPS_FILTERNAME));
	FilterExt.Load(_R(IDT_FILTEREXT_FREEHANDEPS));
}

/********************************************************************************************

>	MetaFileOILFilter::MetaOILFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

MetaFileOILFilter::MetaFileOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_METAFILE));
	FilterExt.Load(_R(IDT_FILTEREXT_METAFILE));
}

/********************************************************************************************

>	EnhancedMetaFileOILFilter::EnhancedMetaFileOILFilter()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

EnhancedMetaFileOILFilter::EnhancedMetaFileOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_ENHMETA_FILTERNAME));
	FilterExt.Load(_R(IDS_FILTEREXT_ENHMETA));
}

/********************************************************************************************

>	AldusFormatOILFilter::AldusFormatOILFilter()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

AldusFormatOILFilter::AldusFormatOILFilter(Filter *pFilter, LPTCHAR UserName, LPTCHAR Extension) : OILFilter(pFilter)
{
	FilterName = UserName;
	FilterExt = Extension;
}

#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX

/********************************************************************************************

>	BMPOILFilter::BMPOILFilter()

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

BMPOILFilter::BMPOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_BMP));
	FilterExt.Load(_R(IDT_FILTEREXT_BMP));
}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/********************************************************************************************

>	AccusoftOILFilter::AccusoftOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

AccusoftOILFilter::AccusoftOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_ACCUSOFT));
	FilterExt.Load(_R(IDN_FILTEREXT_ACCUSOFT));
}

/********************************************************************************************

>	AccusoftBMPOILFilter::AccusoftBMPOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

AccusoftBMPOILFilter::AccusoftBMPOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_ACCUSOFTBMP));
	FilterExt.Load(_R(IDN_FILTEREXT_ACCUSOFTBMP));
}

/********************************************************************************************

>	BrookTroutOILFilter::BrookTroutOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

BrookTroutOILFilter::BrookTroutOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_BROOKTROUT));
	FilterExt.Load(_R(IDN_FILTEREXT_BROOKTROUT));
}

/********************************************************************************************

>	CALSOILFilter::CALSOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

CALSOILFilter::CALSOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_CALS));
	FilterExt.Load(_R(IDN_FILTEREXT_CALS));
}

/********************************************************************************************

>	DCXOILFilter::DCXOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

DCXOILFilter::DCXOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_DCX));
	FilterExt.Load(_R(IDN_FILTEREXT_DCX));
}
#endif

/********************************************************************************************

>	GIFOILFilter::GIFOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

GIFOILFilter::GIFOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_GIF));
	FilterExt.Load(_R(IDN_FILTEREXT_GIF));
}

String_64 GIFOILFilter::GetFilterName(UINT32 NumberToExport)
{
	if (NumberToExport > 1)
		return String_64(_R(IDS_FILTERNAME_ANIMGIF));
	else
		return FilterName;
}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/********************************************************************************************

>	GX2OILFilter::GX2OILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

GX2OILFilter::GX2OILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_GX2));
	FilterExt.Load(_R(IDN_FILTEREXT_GX2));
}

/********************************************************************************************

>	HaloCUTOILFilter::HaloCUTOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

HaloCUTOILFilter::HaloCUTOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_HALOCUT));
	FilterExt.Load(_R(IDN_FILTEREXT_HALOCUT));
}

/********************************************************************************************

>	IFFOILFilter::IFFOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

IFFOILFilter::IFFOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_IFF));
	FilterExt.Load(_R(IDN_FILTEREXT_IFF));
}

/********************************************************************************************

>	IMGOILFilter::IMGOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

IMGOILFilter::IMGOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_IMG));
	FilterExt.Load(_R(IDN_FILTEREXT_IMG));
}

/********************************************************************************************

>	IOCAOILFilter::IOCAOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

IOCAOILFilter::IOCAOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_IOCA));
	FilterExt.Load(_R(IDN_FILTEREXT_IOCA));
}



#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX


/********************************************************************************************

>	JPEGImportOILFilter::JPEGImportOILFilter(Filter *pFilter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Purpose:	Sole constructor for JPEG Import Filter (non-Accusoft version).
				Sets up the list of filename extensions that this filter understands.

********************************************************************************************/
JPEGImportOILFilter::JPEGImportOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_JPG_IMP_FILTERNAME));
	FilterExt.Load(_R(IDN_FILTEREXT_JPEG));
}



/********************************************************************************************

>	JPEGExportOILFilter::JPEGExportOILFilter(Filter *pFilter)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Sole constructor for JPEG Export Filter (non-Accusoft version).
				Sets up the list of filename extensions that this filter understands.

********************************************************************************************/
JPEGExportOILFilter::JPEGExportOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_JPG_EXP_FILTERNAME));
	FilterExt.Load(_R(IDN_FILTEREXT_JPEG));
}



/********************************************************************************************

>	JPEGOILFilter::JPEGOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

JPEGOILFilter::JPEGOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_JPEG));
	FilterExt.Load(_R(IDN_FILTEREXT_JPEG));
}

/********************************************************************************************

>	PNGOILFilter::PNGOILFilter(Filter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/96
	Inputs:		pFilter - The Filter
	Purpose:	Constructs the oily parts of the PNG File Format Filter (ie the list of
				File Extensions that this filter understands)

********************************************************************************************/

PNGOILFilter::PNGOILFilter(Filter* pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_PNG));
	FilterExt.Load(_R(IDS_FILTEREXT_PNG));
} 

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)


/********************************************************************************************

>	KOFAXOILFilter::KOFAXOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

KOFAXOILFilter::KOFAXOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_KOFAX));
	FilterExt.Load(_R(IDN_FILTEREXT_KOFAX));
}

/********************************************************************************************

>	LaserDataOILFilter::LaserDataOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

LaserDataOILFilter::LaserDataOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_LASERDATA));
	FilterExt.Load(_R(IDN_FILTEREXT_LASERDATA));
}

/********************************************************************************************

>	MacPaintOILFilter::MacPaintOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

MacPaintOILFilter::MacPaintOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_MACPAINT));
	FilterExt.Load(_R(IDN_FILTEREXT_MACPAINT));
}

/********************************************************************************************

>	MSIconsOILFilter::MSIconsOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

MSIconsOILFilter::MSIconsOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_MSICONS));
	FilterExt.Load(_R(IDN_FILTEREXT_MSICONS));
}

/********************************************************************************************

>	MSPaintOILFilter::MSPaintOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

MSPaintOILFilter::MSPaintOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_MSPAINT));
	FilterExt.Load(_R(IDN_FILTEREXT_MSPAINT));
}

/********************************************************************************************

>	PCXOILFilter::PCXOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PCXOILFilter::PCXOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PCX));
	FilterExt.Load(_R(IDN_FILTEREXT_PCX));
}

/********************************************************************************************

>	PHOTOCDOILFilter::PHOTOCDOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PhotoCDOILFilter::PhotoCDOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PHOTOCD));
	FilterExt.Load(_R(IDN_FILTEREXT_PHOTOCD));
}

/********************************************************************************************

>	PhotoShopOILFilter::PhotoShopOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PhotoShopOILFilter::PhotoShopOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PHOTOSHOP));
	FilterExt.Load(_R(IDN_FILTEREXT_PHOTOSHOP));
}

/********************************************************************************************

>	PICTOILFilter::PICTOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

PICTOILFilter::PICTOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_PICT));
	FilterExt.Load(_R(IDN_FILTEREXT_PICT));
}

/********************************************************************************************

>	SUNOILFilter::SUNOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

SUNOILFilter::SUNOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_SUN));
	FilterExt.Load(_R(IDN_FILTEREXT_SUN));
}

/********************************************************************************************

>	TARGAOILFilter::TARGAOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

TARGAOILFilter::TARGAOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_TARGA));
	FilterExt.Load(_R(IDN_FILTEREXT_TARGA));
}

/********************************************************************************************

>	TIFFOILFilter::TIFFOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

TIFFOILFilter::TIFFOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_TIFF));
	FilterExt.Load(_R(IDN_FILTEREXT_TIFF));
}

/********************************************************************************************

>	WPGOILFilter::WPGOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

WPGOILFilter::WPGOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_WPG));
	FilterExt.Load(_R(IDN_FILTEREXT_WPG));
}

/********************************************************************************************

>	XWindowsOILFilter::XWindowsOILFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

XWindowsOILFilter::XWindowsOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_XWINDOWS));
	FilterExt.Load(_R(IDN_FILTEREXT_XWINDOWS));
}

/********************************************************************************************

>	SpriteOILFilter::SpriteOILFilter()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Purpose:	Sets up the list of filename extensions that this filter understands.

********************************************************************************************/

SpriteOILFilter::SpriteOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDN_FILTERNAME_SPRITE));
	FilterExt.Load(_R(IDN_FILTEREXT_SPRITE));
}

#endif

/********************************************************************************************

>	NativeOILFilter::NativeOILFilter(Filter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/96
	Inputs:		pFilter - The Filter
	Purpose:	Constructs the oily parts of the Native File Format Filter (ie the list of
				File Extensions that this filter understands)

********************************************************************************************/

NativeOILFilter::NativeOILFilter(Filter* pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_NATIVE));
	FilterExt.Load(_R(IDS_FILTEREXT_NATIVE_NEW));
} 

/********************************************************************************************

>	WebOILFilter::WebOILFilter(Filter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/96
	Inputs:		pFilter - The Filter
	Purpose:	Constructs the oily parts of the Web File Format Filter (ie the list of
				File Extensions that this filter understands)

********************************************************************************************/

WebOILFilter::WebOILFilter(Filter* pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_WEB));
	FilterExt.Load(_R(IDS_FILTEREXT_WEB));
} 

/********************************************************************************************

>	FlareTemplateOILFilter::FlareTemplateOILFilter(Filter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/97
	Inputs:		pFilter - The Filter
	Purpose:	Constructs the oily parts of the Flare Template File Filter (ie the list of
				File Extensions that this filter understands)

********************************************************************************************/

FlareTemplateOILFilter::FlareTemplateOILFilter(Filter* pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_FLARETEMPLATE));
	FilterExt.Load(_R(IDS_FILTEREXT_FLARETEMPLATE));
} 


/********************************************************************************************

>	ImagemapOILFilter::ImagemapOILFilter(Filter* pFilter)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Inputs:		pFilter - The Filter
	Purpose:	Constructs the oily parts of the Imagemap Filter (ie the list of
				File Extensions that this filter understands)

********************************************************************************************/

ImagemapOILFilter::ImagemapOILFilter(Filter* pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_IMAGEMAP));
	FilterExt.Load(_R(IDS_FILTEREXT_IMAGEMAP));
} 


#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

// No text filters as the wonderful new system really encourages innovation!
#if BUILD_OTHER_TEXT_FILTERS
/********************************************************************************************
>	ANSITextOILFilter::ANSITextOILFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
ANSITextOILFilter::ANSITextOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_ANSITEXT));
	FilterExt.Load(_R(IDT_FILTEREXT_ANSITEXT));
}


/********************************************************************************************
>	WCHAR ANSITextOILFilter::ReadMultiByteChar(CCLexFile* pFile, UINT32* pBytesRead)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pFile - pointer to file to read from
	Outputs:	pBytesRead = Number of bytes read from file
	Returns:	Unicode value of character read from file.  0 returned on error.
	Purpose:	Reads an 8 or 16 bit character from the current file.  Converts it to Unicode
				using the current code-page.
********************************************************************************************/
WCHAR ANSITextOILFilter::ReadMultiByteChar(CCLexFile* pFile, UINT32* pBytesRead)
{
	CHAR First;
	pFile->read(&First);
	if (pBytesRead != NULL)
		*pBytesRead = 1;

	if (IsDBCSLeadByte(First))
	{
		UINT32 Both = First << 8;
		pFile->read(&First);
		if (pBytesRead != NULL)
			*pBytesRead = 2;
		Both = Both | First;
		return UnicodeManager::MultiByteToUnicode(Both);
	}
	else
		return First;
}



/********************************************************************************************
>	UnicodeTextOILFilter::UnicodeTextOILFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
UnicodeTextOILFilter::UnicodeTextOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_UNICODETEXT));
	FilterExt.Load(_R(IDT_FILTEREXT_UNICODETEXT));
}


/********************************************************************************************
>	WCHAR UnicodeTextOILFilter::ReadUnicodeChar(CCLexFile* pFile)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Inputs:		pFile - pointer to file to read from
	Outputs:	-
	Returns:	Unicode value of character read from file.  0 returned on error.
	Purpose:	Reads an Unicode character value from the current file.
********************************************************************************************/
WCHAR UnicodeTextOILFilter::ReadUnicodeChar(CCLexFile* pFile)
{
	BYTE Header[2];
	pFile->read(Header, 2);

	return (WCHAR)((Header[1]<<8) | Header[0]);
}


/********************************************************************************************
>	RTFTextOILFilter::RTFTextOILFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
RTFTextOILFilter::RTFTextOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDT_FILTERNAME_RTF));
	FilterExt.Load(_R(IDT_FILTEREXT_RTF));
}


/********************************************************************************************
>	BOOL RTFTextOILFilter::RegisterWithClipboard(RTFTextFilter* pFilter)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/95
	Inputs:		pFilter - pointer to the kernel RTF filter
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Registers with the external clipboard system so we can import RTF from the
				clipboard
********************************************************************************************/
BOOL RTFTextOILFilter::RegisterWithClipboard(RTFTextFilter* pFilter)
{
	UINT32 WindowsFormatID = ::RegisterClipboardFormat("Rich Text Format");

	ERROR2IF(WindowsFormatID==0, FALSE, "Windows denies all knowledge of RTF!")
	
	InternalClipboardFormat Format(CLIPTYPE_TEXT);
	RTFClipMap::CreateAndRegister(CLIPMAP_IMPORTONLY, (Filter*)pFilter, Format, WindowsFormatID, 60);

	return TRUE;
}



/********************************************************************************************
>	BOOL RTFTextOILFilter::GetCurrentEmSize(RTFTextFilter* pFilterMILLIPOINT* pResult)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/95
	Inputs:		pFilter - pointer to the RTF filter in action
				pResult - return pointer for the current width of an Em
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Gets the size of an Em given the current font, size, etc, (from the current
				attribute array in the filter)
********************************************************************************************/
BOOL RTFTextOILFilter::GetCurrentEmSize(RTFTextFilter* pFilter, MILLIPOINT* pResult)
{
	ERROR2IF((pFilter == NULL) || (pResult == NULL), FALSE, "NULL parameter");

	// Setup a device context and character description for the font manager to use
	CDC DeviceContext;
	BOOL Result = DeviceContext.CreateCompatibleDC(NULL);
	ERROR1IF(!Result, FALSE, _R(IDE_NOMORE_MEMORY));
	CharDescription FontDesc(FONTEMCHAR,
							((TxtFontTypefaceAttribute *) pFilter->CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr)->HTypeface,
							((TxtBoldAttribute *) pFilter->CurrentAttrs[ATTR_TXTBOLD ].pAttr)->BoldOn ||
								((TxtFontTypefaceAttribute *) pFilter->CurrentAttrs[ATTR_TXTFONTTYPEFACE ].pAttr)->IsBold,
							((TxtItalicAttribute *) pFilter->CurrentAttrs[ATTR_TXTITALIC ].pAttr)->ItalicOn ||
								((TxtFontTypefaceAttribute *) pFilter->CurrentAttrs[ATTR_TXTFONTTYPEFACE ].pAttr)->IsItalic);

	// Get info on an M
	CharMetrics chMetrics; 
	if (!FONTMANAGER->GetCharMetrics(&DeviceContext, FONTEMCHAR, FontDesc, &chMetrics))
		return FALSE;
				
	// transform the default values by the attribute matrix
//	MILLIPOINT FontSize = RR_TXTFONTSIZE();
//	MILLIPOINT OffsetY  = FontSize * pScriptAttr->Offset;
//	FIXED16    fScaleY  = pScriptAttr->Size * Div32By32(FontSize, TextManager::GetDefaultHeight());
//	FIXED16    fScaleX  = fScaleY * RR_TXTASPECTRATIO();
//	*pAttrMatrix = Matrix(fScaleX,0,0,fScaleY,0,OffsetY);

	
	
	TxtScriptAttribute* pScriptAttr=(TxtScriptAttribute*) pFilter->CurrentAttrs[ATTR_TXTSCRIPT ].pAttr;
	ERROR2IF(pScriptAttr==NULL, FALSE, "pScriptAttr==NULL");
	MILLIPOINT FontSize = ((TxtFontSizeAttribute*) pFilter->CurrentAttrs[ATTR_TXTFONTSIZE ].pAttr)->FontSize;
	MILLIPOINT OffsetY  = FontSize * pScriptAttr->Offset;
	FIXED16    fScaleY  = pScriptAttr->Size * Div32By32(FontSize, TextManager::GetDefaultHeight());
	FIXED16    fScaleX  = fScaleY * ((TxtAspectRatioAttribute *) pFilter->CurrentAttrs[ATTR_TXTASPECTRATIO ].pAttr)->AspectRatio;
	Matrix AttrMatrix(fScaleX,0,0,fScaleY,0,OffsetY);

	// Get the current size of an Em 
	DocCoord pXYValue(chMetrics.FontEmWidth, 0);
	AttrMatrix.transform(&pXYValue);
	*pResult = pXYValue.x;

	return TRUE;
}
#endif //#if BUILD_OTHER_TEXT_FILTERS




/********************************************************************************************
>	HTMLOILFilter::HTMLOILFilter(Filter *pFilter) 

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/03/97
	Purpose:	Sets up the list of filename extensions that this filter understands.
********************************************************************************************/
HTMLOILFilter::HTMLOILFilter(Filter *pFilter) : OILFilter(pFilter)
{
	FilterName.Load(_R(IDS_FILTERNAME_HTML));
	FilterExt.Load(_R(IDS_FILTEREXT_HTML));
}



#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX
