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

#ifndef INC_OILFLTRS
#define INC_OILFLTRS

#include "filters.h"
#include "selop.h"
//#include "cmxres.h"

class CCLexFile;
class RTFTextFilter;

/********************************************************************************************

>	class OILFilter : public CC_CLASS_MEMDUMP

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/03/94
	Purpose:	This class is used to encapsulate the platform specific features of a filter.
				Every Filter object has a pointer to an OILFilter object.  For example, 
				under Windows, it holds the file extensions that this file understands.
	SeeAlso:	Filter

********************************************************************************************/

class OILFilter : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(OILFilter);

public:
	OILFilter() {}
	virtual ~OILFilter() { }
	OILFilter(Filter *pFilter);

	// The COMMDLG filter string resource IDs to be used for this filter:
	String_64 FilterName;  // The filter name, e.g. "Corel 4.0 EPS"
	String_32 FilterExt;	// The extensions to use, e.g. "*.eps"

	// When a COMMDLG is open, indicates the position in the filter list of this particular
	// filter.
	INT32 Position;

	Filter *Parent;

	static BOOL InitFilters(List&);

	static BOOL CreatePluginFilters(List&);

	virtual char *ConstructFilterString(UINT32 NumberToExport = 1);
	virtual String_64 GetFilterName(UINT32 NumberToExport);

	// Some useful helper functions for checking that filename extensions match the
	// ones that this filter supports. Also for fixing ones that don't match.
	BOOL DoesExtensionOfFileMatch(CCLexFile* File);
	BOOL DoesExtensionOfPathNameMatch(PathName *Path);
	BOOL FixExtensionOfPathName(PathName *Path);
};


/********************************************************************************************

>	class OILFilterFamily : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Purpose:	Automate the production of the OIL parts of a filter family object.
	SeeAlso:	FilterFamily

********************************************************************************************/

class OILFilterFamily : public OILFilter
{
	CC_DECLARE_MEMDUMP(OILFilterFamily);

public:
	OILFilterFamily(Filter *pFilter);
	OILFilterFamily(Filter *pFilter, UINT32 FilterNameID);

	virtual char *ConstructFilterString(UINT32 NumberToExport = 1);
};



//
// These are the OILy parts of the built-in filters.
//


/********************************************************************************************

>	class AIEPSOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the Illustrator EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class AIEPSOILFilter : public OILFilter
{
public:
	AIEPSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class AI5EPSOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the Illustrator 5.0 EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class AI5EPSOILFilter : public OILFilter
{
public:
	AI5EPSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class AI8EPSOILFilter : public OILFilter

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/00
	Purpose:	Provide the OIL parts of the Illustrator 8.0 EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class AI8EPSOILFilter : public OILFilter
{
public:
	AI8EPSOILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class ArtWorksEPSOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the ArtWorks EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class ArtWorksEPSOILFilter : public OILFilter
{
public:
	ArtWorksEPSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class CamelotEPSOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the ArtWorks EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class CamelotEPSOILFilter : public OILFilter
{
public:
	CamelotEPSOILFilter(Filter *pFilter);
};



/********************************************************************************************

>	class NativeEPSOILFilter : public CamelotEPSOILFilter

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Purpose:	The Native File format Oil Filter (Extension etc)

********************************************************************************************/

class NativeEPSOILFilter : public CamelotEPSOILFilter
{
public:
	NativeEPSOILFilter (Filter* pFilter);
};


/********************************************************************************************

>	class Corel3EPSOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the Corel 3.x EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class Corel3EPSOILFilter : public OILFilter
{
public:
	Corel3EPSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class Corel4EPSOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the Corel 4.x EPS filter.
	SeeAlso:	EPSFilter

********************************************************************************************/

class Corel4EPSOILFilter : public OILFilter
{
public:
	Corel4EPSOILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class DrawOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the Draw filter.
	SeeAlso:	DrawFilter

********************************************************************************************/

class DrawOILFilter : public OILFilter
{
public:
	DrawOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class FlashOILFilter : public OILFilter

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/99
	Purpose:	Provide the OIL parts of the Flash 4 filter.
	SeeAlso:	FlashFilter

********************************************************************************************/

class FlashOILFilter : public OILFilter
{
public:
	FlashOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PhotoShopEPSOILFilter : public OILFilter

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Purpose:	Provide the OIL parts of the PhotoShop EPS filter.
	SeeAlso:	PhotoShopEPSFilter

********************************************************************************************/

class PhotoShopEPSOILFilter : public OILFilter
{
public:
	PhotoShopEPSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class CorelPaletteFileOILFilter : public OILFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Purpose:	Provide the OIL parts of the Corel Palette file filter.
	SeeAlso:	CorelPaletteFileFilter

********************************************************************************************/

class CorelPaletteFileOILFilter : public OILFilter
{
public:
	CorelPaletteFileOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class CDROILFilter : public OILFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/95
	Purpose:	Provide the OIL parts of the CDR filter.
	SeeAlso:	CDRFilter

********************************************************************************************/

class CDROILFilter : public OILFilter
{
public:
	CDROILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class CMXOILFilter : public OILFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/96
	Purpose:	Provide the OIL parts of the CMX filter.
	SeeAlso:	CMXFilter

********************************************************************************************/

class CMX16OILFilter : public OILFilter
{
public:
	CMX16OILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class CMXOILFilter : public OILFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/96
	Purpose:	Provide the OIL parts of the CMX filter.
	SeeAlso:	CMXFilter

********************************************************************************************/

class CMX32OILFilter : public OILFilter
{
public:
	CMX32OILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class CMXImportOILFilter : public OILFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/08/96
	Purpose:	Provide the OIL parts of the CMX import filter.
	SeeAlso:	CMXImportFilter

********************************************************************************************/

class CMXImportOILFilter : public OILFilter
{
public:
	CMXImportOILFilter(Filter *pFilter);
};



/********************************************************************************************
>	class MSPaletteOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Purpose:	Provide the OIL parts of the Windows Palette file filter.
	SeeAlso:	MSPaletteFilter
********************************************************************************************/
class MSPaletteOILFilter : public OILFilter
{
public:
	MSPaletteOILFilter(Filter *pFilter);
};


/********************************************************************************************
>	class AdobeColourTableOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Purpose:	Provide the OIL parts of the Adobe Colour Table file filter.
	SeeAlso:	AdobeColourTableFilter
********************************************************************************************/
class AdobeColourTableOILFilter : public OILFilter
{
public:
	AdobeColourTableOILFilter(Filter *pFilter);

	INT32 HowCompatible(PathName& Filename);
};


/********************************************************************************************
>	class AdobeColourSwatchOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Purpose:	Provide the OIL parts of the Adobe Colour Swatch file filter.
	SeeAlso:	AdobeColourSwatchFilter
********************************************************************************************/
class AdobeColourSwatchOILFilter : public OILFilter
{
public:
	AdobeColourSwatchOILFilter(Filter *pFilter);

	INT32 HowCompatible(PathName& Filename);
};


/********************************************************************************************
>	class PSPPaletteOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Purpose:	Provide the OIL parts of the paint shop pro palette file filter.
	SeeAlso:	PaintShopProPaletteFilter
********************************************************************************************/
class PSPPaletteOILFilter : public OILFilter
{
public:
	PSPPaletteOILFilter(Filter *pFilter);
};


/********************************************************************************************
>	class JCWColourOILFilter : public OILFilter

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Purpose:	Provide the OIL parts of the Jason palette filter
	SeeAlso:	JCWColourFilter
********************************************************************************************/
class JCWColourOILFilter : public OILFilter
{
public:
	JCWColourOILFilter(Filter *pFilter);
	INT32 HowCompatible(PathName& Filename);
};

/********************************************************************************************

>	class FreeHandEPSOILFilter : public OILFilter

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Purpose:	Provide the OIL parts of the FreeHand EPS filter.
	SeeAlso:	FreeHandEPSFilter

********************************************************************************************/

class FreeHandEPSOILFilter : public OILFilter
{
public:
	FreeHandEPSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class MetaFileOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the MetaFile filter.
	SeeAlso:	MetaFileFilter

********************************************************************************************/

class MetaFileOILFilter : public OILFilter
{
public:
	MetaFileOILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class EnhancedMetaFileOILFilter : public OILFilter

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/5/95
	Purpose:	Provide the OIL parts of the Enhanced MetaFile filter.
	SeeAlso:	EnhancedMetaFilter

********************************************************************************************/

class EnhancedMetaFileOILFilter : public OILFilter
{
public:
	EnhancedMetaFileOILFilter(Filter *pFilter);
};



/********************************************************************************************

>	class AldusFormatOILFilter : public OILFilter

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/95
	Purpose:	Provide the OIL parts of the Aldus-format filter
	SeeAlso:	MetaFileFilter

********************************************************************************************/

class AldusFormatOILFilter : public OILFilter
{
public:
	AldusFormatOILFilter(Filter *pFilter,LPTCHAR,LPTCHAR);
};


/********************************************************************************************

>	class BMPOILFilter : public OILFilter

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/02/94
	Purpose:	Provide the OIL parts of the BMP filter.
	SeeAlso:	BMPFilter

********************************************************************************************/

class BMPOILFilter : public OILFilter
{
public:
	BMPOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class AccusoftOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the Accusoft filters.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class AccusoftOILFilter : public OILFilter
{
public:
	AccusoftOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class AccusoftBMPOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/95
	Purpose:	Provide the OIL parts of the Accusoft BMP filter.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class AccusoftBMPOILFilter : public OILFilter
{
public:
	AccusoftBMPOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class BrookTroutOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the DBrookTroutCX filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class BrookTroutOILFilter : public OILFilter
{
public:
	BrookTroutOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class CALSOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the CALS filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class CALSOILFilter : public OILFilter
{
public:
	CALSOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class DCXOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the DCX filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class DCXOILFilter : public OILFilter
{
public:
	DCXOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class GIFOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the GIF filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class GIFOILFilter : public OILFilter
{
public:
	GIFOILFilter(Filter *pFilter);
	String_64 GetFilterName(UINT32 NumberToExport);
};

/********************************************************************************************

>	class GX2OILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the GX2 filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class GX2OILFilter : public OILFilter
{
public:
	GX2OILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class HaloCUTOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the HaloCUT filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class HaloCUTOILFilter : public OILFilter
{
public:
	HaloCUTOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class IFFOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the IFF filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class IFFOILFilter : public OILFilter
{
public:
	IFFOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class IMGOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the IMG filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class IMGOILFilter : public OILFilter
{
public:
	IMGOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class IOCAOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the IOCA filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class IOCAOILFilter : public OILFilter
{
public:
	IOCAOILFilter(Filter *pFilter);
};





/********************************************************************************************

>	class JPEGImportOILFilter : public OILFilter

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Purpose:	OILFilter for non-Accusost version of JPEG import filter.

********************************************************************************************/
class JPEGImportOILFilter : public OILFilter
{
public:
	JPEGImportOILFilter(Filter *pFilter);
};



/********************************************************************************************

>	class JPEGExportOILFilter : public OILFilter

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	OILFilter for non-Accusost version of JPEG export filter.

********************************************************************************************/
class JPEGExportOILFilter : public OILFilter
{
public:
	JPEGExportOILFilter(Filter *pFilter);
};




/********************************************************************************************

>	class JPEGOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the JPEG filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class JPEGOILFilter : public OILFilter
{
public:
	JPEGOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class KOFAXOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the KOFAX filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class KOFAXOILFilter : public OILFilter
{
public:
	KOFAXOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class LaserDataOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the LaserData filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class LaserDataOILFilter : public OILFilter
{
public:
	LaserDataOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class MacPaintOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the MacPaint filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class MacPaintOILFilter : public OILFilter
{
public:
	MacPaintOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class MSIconsOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the MSIcons filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class MSIconsOILFilter : public OILFilter
{
public:
	MSIconsOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class MSPaintOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the MSPaint filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class MSPaintOILFilter : public OILFilter
{
public:
	MSPaintOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PCXOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the PCX filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class PCXOILFilter : public OILFilter
{
public:
	PCXOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PhotoCDOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the PHOTOCD filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class PhotoCDOILFilter : public OILFilter
{
public:
	PhotoCDOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PhotoShopOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the PhotoShop filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class PhotoShopOILFilter : public OILFilter
{
public:
	PhotoShopOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class PICTOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the PICT filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class PICTOILFilter : public OILFilter
{
public:
	PICTOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class SUNOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the SUN filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class SUNOILFilter : public OILFilter
{
public:
	SUNOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class TARGAOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the TARGA filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class TARGAOILFilter : public OILFilter
{
public:
	TARGAOILFilter(Filter *pFilter);
};

/********************************************************************************************
>	class TIFFOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the TIFF filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class TIFFOILFilter : public OILFilter
{
public:
	TIFFOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class WPGOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the WPG filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class WPGOILFilter : public OILFilter
{
public:
	WPGOILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class XWindowsOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Purpose:	Provide the OIL parts of the XWindows filter which loads in the file as a BMP.
	SeeAlso:	AccusoftFilter

********************************************************************************************/

class XWindowsOILFilter : public OILFilter
{
public:
	XWindowsOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class SpriteOILFilter : public OILFilter

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Purpose:	Provide the OIL parts of the Acorn Sprite filter which loads in the file as a BMP.
	SeeAlso:	SpriteFilter

********************************************************************************************/

class SpriteOILFilter : public OILFilter
{
public:
	SpriteOILFilter(Filter *pFilter);
};


/********************************************************************************************

>	class DXFOILFilter : public DXFFilter

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Purpose:	Provide the OIL parts of the DXF filter
	SeeAlso:	DXFFilter

********************************************************************************************/

class DXFOILFilter : public OILFilter
{
public:
	DXFOILFilter(Filter *pFilter);
};

/********************************************************************************************

>	class NativeOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/96
	Purpose:	The Native File format Oil Filter (Extension etc)

********************************************************************************************/

class NativeOILFilter : public OILFilter
{
public:
	NativeOILFilter (Filter* pFilter);
};

/********************************************************************************************

>	class WebOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/96
	Purpose:	The Web File format Oil Filter (Extension etc)

********************************************************************************************/

class WebOILFilter : public OILFilter
{
public:
	WebOILFilter (Filter* pFilter);
};


/********************************************************************************************

>	class FlareTemplateOILFilter : public OILFilter

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/97
	Purpose:	The Flare template format Oil Filter (Extension etc)

********************************************************************************************/

class FlareTemplateOILFilter : public OILFilter
{
public:
 	FlareTemplateOILFilter(Filter* pFilter);
};


/********************************************************************************************

>	class PNGOILFilter : public OILFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/96
	Purpose:	The PNG File format Oil Filter (Extension etc)

********************************************************************************************/

class PNGOILFilter : public OILFilter
{
public:
	PNGOILFilter (Filter* pFilter);
};

/********************************************************************************************

>	class ImagemapOILFilter : public OILFilter

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/4/97
	Purpose:	The Imagemap Oil Filter

********************************************************************************************/

class ImagemapOILFilter : public OILFilter
{
public:
	ImagemapOILFilter (Filter* pFilter);
};




#if BUILD_TEXT_FILTERS
/********************************************************************************************
>	class ANSITextOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/95
	Purpose:	Provide the OIL parts of the ANSI text filter.
	SeeAlso:	ANSITextFilter
********************************************************************************************/
class ANSITextOILFilter : public OILFilter
{
public:
	ANSITextOILFilter(Filter *pFilter);

	WCHAR ReadMultiByteChar(CCLexFile* pFile, UINT32* pBytesRead);
};


/********************************************************************************************
>	class UnicodeTextOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Provide the OIL parts of the Unicode text filter.
	SeeAlso:	UnicodeTextFilter
********************************************************************************************/
class UnicodeTextOILFilter : public OILFilter
{
public:
	UnicodeTextOILFilter(Filter *pFilter);

	WCHAR ReadUnicodeChar(CCLexFile* pFile);
};


/********************************************************************************************
>	class RTFTextOILFilter : public OILFilter

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/95
	Purpose:	Provide the OIL parts of the RTF filter.
	SeeAlso:	RTFTextFilter
********************************************************************************************/
class RTFTextOILFilter : public OILFilter
{
public:
	RTFTextOILFilter(Filter *pFilter);

	BOOL RegisterWithClipboard(RTFTextFilter* pFilter);
	BOOL GetCurrentEmSize(RTFTextFilter* pFilter, MILLIPOINT* pResult);
};
#endif	// #if BUILD_TEXT_FILTERS


/********************************************************************************************
>	class HTMLOILFilter : public OILFilter

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/03/97
	Purpose:	Provide the OIL parts of the HTML filter.
	SeeAlso:	HTMLFilter
********************************************************************************************/

class HTMLOILFilter : public OILFilter
{
public:
	HTMLOILFilter(Filter* pFilter);

};






#endif // INC_OILFLTRS
