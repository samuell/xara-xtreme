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

// A GIF import/export filter 


#include "camtypes.h"
#include "giffiltr.h"
//#include "andy.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndbmp.h"
#include "nodebmp.h"
#include "bmpfiltr.h"
#include "progress.h"
#include "oilbitmap.h"	// Windows specific bitmap information
//#include "docview.h"	// DocView - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "richard2.h"	// resource strings _R(IDS_GIFFILTR_FAILED_MASK)
//#include "accures.h"	// Accusoft filter ids and messages 
//#include "bmpres.h"		// general bitmap filter based resources (_R(IDE_TIGIFFILTER_MASKFAILED)/MASK)
//#include "will3.h"		// for _R(IDS_GENOPTPALMSGID)

#include "maskfilt.h"	// MaskedFilter class
#include "bmpsdlg.h"
#include "frameops.h"	// GIFAnimationExportParam
//#include "spread.h"		// Pasteboard rect - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmapprev.h"	// tab preview dialog
#include "palman.h"		// MakePaletteBrowserCompatible
#include "sprdmsg.h"	// SpreadMsg::ANIMATIONPROPERTIESCHANGED
#include "impexpop.h"

#include "exphint.h"

#include "camdoc.h"
#include "menuops.h"

//#include "outptdib.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrec.h"		// for CXaraFileRecord - in camtypes.h [AUTOMATICALLY REMOVED]

#include "mrhbits.h"	//  For CBMPBits::RenderSelectionToBMP
#include "selall.h"		//  For OPTOKEN_EDITSELECTALL
#include "bmpprefs.h"
//#include "animparams.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "tmplmngr.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(TI_GIFFilter, MaskedFilter)
CC_IMPLEMENT_MEMDUMP(GIFImportOptions, BitmapImportOptions)
CC_IMPLEMENT_DYNCREATE(GIFExportOptions, MaskedFilterExportOptions)

CC_IMPLEMENT_DYNCREATE(AnimatedGIFImage, ListItem)

#define	new	CAM_DEBUG_NEW

static UINT32 g_DefaultGIFType = 0;

OutputGIF 	TI_GIFFilter::DestGIF;
FilterType	TI_GIFFilter::s_FilterType = GIF;	// Type of filter in use (TI_GIF .. TI_GIF_TRANSINTER)

CENTISECONDS		GIFExportOptions::g_AnimationDelay 			= 10;			// Delay in CentiSeconds
GIFDisposalMethod	GIFExportOptions::g_AnimationRestoreType 	= GDM_LEAVE;	// How to restore after each frame
UINT32				GIFExportOptions::g_AnimationLoopCount 		= 1;			// Number of loops
KernelBitmap**		GIFExportOptions::m_pBitmapList				= NULL;
UINT32				GIFExportOptions::m_ListSize				= 0;

/********************************************************************************************

>	static BOOL GIFExportOptions::Declare()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com> (from Peter)
	Created:	29/10/96
	Returns:	TRUE if successfully declared preferences
				FALSE otherwise
	Purpose:	To declare preferences associated with these export options

********************************************************************************************/
BOOL GIFExportOptions::Declare()
{
	if (Camelot.DeclareSection(_T("Filters"), 10))
	{
		Camelot.DeclarePref( NULL, _T("ExportGIFtype"), &g_DefaultGIFType, 0, 3 );
		Camelot.DeclarePref( NULL, _T("AnimationDelay"), &g_AnimationDelay, 0, 65535 );
		Camelot.DeclarePref( NULL, _T("AnimationRestoreType"), (INT32*)&g_AnimationRestoreType, 0, 3 );
		Camelot.DeclarePref( NULL, _T("AnimationLoopCount"), &g_AnimationLoopCount, 0, 65535 );
	}

	// All ok
	return TRUE;
}



/********************************************************************************************
>	GIFExportOptions::GIFExportOptions()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Default constructor for a GIFExportOptions object to provide GIF export
				options
********************************************************************************************/

GIFExportOptions::GIFExportOptions() :
	DelayIndex(0),
	RestoreIndex(0),
	m_GlobalPalette(TRUE),
	m_LeftOffset(0),
	m_TopOffset(0),
	m_AnimDelay(g_AnimationDelay),
	m_AnimDispose((GIFDisposalMethod) g_AnimationRestoreType),
	m_AnimLoop(g_AnimationLoopCount),
	m_AnimSize(0, 0),
	DelayValueIsDefault(FALSE)
{
	g_AnimationLoopCount = 1;
}



/********************************************************************************************
>	GIFExportOptions::GIFExportOptions(const FilterType FilterID, const StringBase* pFilterName)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Constructor for a GIFExportOptions object to provide GIF export
				options
********************************************************************************************/

GIFExportOptions::GIFExportOptions(const FilterType FilterID, const StringBase* pFilterName)
  : MaskedFilterExportOptions(_R(IDD_EXPORTBMPOPTS), FilterID, pFilterName),
	DelayIndex(0),
	RestoreIndex(0),
	m_GlobalPalette(TRUE),
	m_LeftOffset(0),
	m_TopOffset(0),
	m_AnimDelay(g_AnimationDelay),
	m_AnimDispose((GIFDisposalMethod) g_AnimationRestoreType),
	m_AnimLoop(g_AnimationLoopCount),
	m_AnimSize(0, 0),
	DelayValueIsDefault(FALSE)
{
	g_AnimationLoopCount = 1;
}



/********************************************************************************************

>	virtual BOOL GIFExportOptions::CopyFrom(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Sets the contents of this object from the passed object
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
BOOL GIFExportOptions::CopyFrom(BitmapExportOptions *pSource) 
{
	// copy the base class first
	if (!MaskedFilterExportOptions::CopyFrom(pSource))
		return FALSE;

	// must be the same class, otherwise the base class  function above returns FALSE
	GIFExportOptions *pOther = (GIFExportOptions *)pSource;

	m_AnimDelay			= pOther->m_AnimDelay;
	m_AnimDispose		= pOther->m_AnimDispose;
	m_AnimLoop			= pOther->m_AnimLoop;
	DelayValueIsDefault	= pOther->DelayValueIsDefault;
	DelayIndex			= pOther->DelayIndex;
	RestoreIndex		= pOther->RestoreIndex;
	m_GlobalPalette		= pOther->m_GlobalPalette;
	m_LeftOffset		= pOther->m_LeftOffset;
	m_TopOffset			= pOther->m_TopOffset;
	m_AnimSize			= pOther->m_AnimSize;

	return TRUE;
};


BOOL GIFExportOptions::FileTypeChangeCopyFrom(BitmapExportOptions *pOther)
{
	if( !MaskedFilterExportOptions::FileTypeChangeCopyFrom(pOther) )
		return FALSE;

	//  Since we are not copying from another GIFOptions-based object, then the variables
	//  in this object must be assigned some default values.
	m_AnimDelay				= g_AnimationDelay;
	m_AnimDispose			= (GIFDisposalMethod)g_AnimationRestoreType;
	m_AnimLoop				= g_AnimationLoopCount;

	g_AnimationLoopCount	= 1;
	DelayValueIsDefault		= FALSE;
	DelayIndex				= 0;
	RestoreIndex			= 0;

	m_GlobalPalette			= TRUE;
	m_LeftOffset			= 0;
	m_TopOffset				= 0;

	// Graeme (14-9-00) - Limit the colour depth to be no more than 8bpp. Otherwise we get
	// problems with changing file types in the export dialogue.
	if ( GetDepth () > 8 )
	{
		SetDepth ( 8 );
	}

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL GIFExportOptions::Equal(BitmapExportOptions *pSource) 

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/97
	Inputs:		pSource - the other BitmapExportOptions object to copy the data from
	Purpose:	Compares the contents of this and pOther objects
	Returns:	TRUE, if objects are equal, FALSE otherwise
	See Also:	BitmapExportOptions::MakeCopy()

********************************************************************************************/
/*
BOOL GIFExportOptions::Equal(BitmapExportOptions *pSource) 
{
	BOOL ok = TRUE;

	// compare the base classes first
	ok = MaskedFilterExportOptions::Equal(pSource);

	// must be the same class, otherwise the base class  function above returns FALSE
	GIFExportOptions *pOther = (GIFExportOptions *)pSource;

	if (ok) ok = (m_AnimDelay	== pOther->m_AnimDelay);
	if (ok) ok = (m_AnimDispose	== pOther->m_AnimDispose);
	if (ok) ok = (m_AnimLoop== pOther->m_AnimLoop);
	if (ok) ok = (g_AnimationLoopCount	== pOther->g_AnimationLoopCount);
	if (ok) ok = (DelayValueIsDefault	== pOther->DelayValueIsDefault);
	if (ok) ok = (DelayIndex== pOther->DelayIndex);
	if (ok) ok = (RestoreIndex	== pOther->RestoreIndex);
	return ok;
};
*/

/********************************************************************************************

>	virtual BOOL GIFExportOptions::RetrieveDefaults()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BitmapExportOptions for interface details
	Notes:		Gets GIF specific preferences

********************************************************************************************/
BOOL GIFExportOptions::RetrieveDefaults()
{
	if (!MaskedFilterExportOptions::RetrieveDefaults())
	{
		return FALSE;
	}

	ERROR2IF(g_AnimationRestoreType != GDM_NONE && g_AnimationRestoreType != GDM_LEAVE &&
			g_AnimationRestoreType != GDM_BACKTOBACK && g_AnimationRestoreType != GDM_PREVIOUS,
			FALSE, "Invalid g_AnimationRestoreType");

	// Ensure that the depth is no more than 8 bits.
	if ( GetDepth () > 8 )
	{
		this->SetDepth ( 8 );
	}

	m_AnimDelay		= g_AnimationDelay;
	m_AnimDispose	= g_AnimationRestoreType;
	m_AnimLoop		= g_AnimationLoopCount;

//	SetMakeTransparent((g_DefaultGIFType >> 1) & 1);
	SetMakeInterlaced(g_DefaultGIFType & 1);

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL GIFExportOptions::SetAsDefaults() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Provides additional implementation to set GIF specific info as defaults
	See Also:	BitmapExportOptions::SetAsDefaults()

********************************************************************************************/
BOOL GIFExportOptions::SetAsDefaults() const
{
	if (!MaskedFilterExportOptions::SetAsDefaults())
	{
		return FALSE;
	}

	g_AnimationDelay		= m_AnimDelay;
	
	g_AnimationRestoreType	= m_AnimDispose;
	g_AnimationLoopCount	= m_AnimLoop;

	g_DefaultGIFType = WantTransparent() ? 2 : 0;
	g_DefaultGIFType |= WantInterlaced() ? 1 : 0;

	return TRUE;
}



/********************************************************************************************
>	virtual BOOL GIFExportOptions::Write(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to write
	Returns:	TRUE if successful.
	Purpose:	Writes this object to the given record.
	SeeAlso:	GIFExportOptions::Read
********************************************************************************************/

BOOL GIFExportOptions::Write(CXaraFileRecord* pRec)
{
	return MaskedFilterExportOptions::Write(pRec) &&
		   pRec->WriteUINT32(DelayIndex) &&
		   pRec->WriteUINT32(RestoreIndex) &&
		   pRec->WriteBYTE((BYTE) !!m_GlobalPalette) &&
		   pRec->WriteUINT32(m_LeftOffset) &&
		   pRec->WriteUINT32(m_TopOffset) &&
		   pRec->WriteUINT32((UINT32) m_AnimDelay) &&
		   pRec->WriteINT16((INT16) m_AnimDispose) &&
		   pRec->WriteUINT32(m_AnimLoop) &&
		   pRec->WriteCoord((const DocCoord&) m_AnimSize) &&
		   pRec->WriteBYTE((BYTE) !!DelayValueIsDefault);
}



/********************************************************************************************
>	virtual BOOL GIFExportOptions::Read(CXaraFileRecord* pRec)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Inputs:		pRec	---		record to read
	Returns:	TRUE if successful.
	Purpose:	Reads this object from the given record.
	SeeAlso:	GIFExportOptions::Write
********************************************************************************************/

BOOL GIFExportOptions::Read(CXaraFileRecord* pRec)
{
	m_GlobalPalette = DelayValueIsDefault = FALSE;
	return MaskedFilterExportOptions::Read(pRec) &&
		   pRec->ReadUINT32((UINT32*) &DelayIndex) &&
		   pRec->ReadUINT32((UINT32*) &RestoreIndex) &&
		   pRec->ReadBYTE((BYTE*) &m_GlobalPalette) &&
		   pRec->ReadUINT32((UINT32*) &m_LeftOffset) &&
		   pRec->ReadUINT32((UINT32*) &m_TopOffset) &&
		   pRec->ReadUINT32((UINT32*) &m_AnimDelay) &&
		   pRec->ReadINT16((INT16*) &m_AnimDispose) &&
		   pRec->ReadUINT32((UINT32*) &m_AnimLoop) &&
		   pRec->ReadCoord((DocCoord*) &m_AnimSize) &&
		   pRec->ReadBYTE((BYTE*) &DelayValueIsDefault);
}



/********************************************************************************************

>	CENTISECONDS GIFExportOptions::GetAnimationDelay() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The delay desired for animated GIFs
	Purpose:	Support function to obtain export options

********************************************************************************************/
CENTISECONDS GIFExportOptions::GetAnimationDelay() 
{
	//	return m_AnimDelay;
	UINT32 Delay = m_pBitmapList[DelayIndex]->GetDelay();
	DelayIndex++;
	return Delay;
}


/********************************************************************************************

>	BOOL GIFExportOptions::SetAnimationDelay(CENTISECONDS Delay)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		Delay : The desired delay to be set in the animated GIF
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetAnimationDelay()

********************************************************************************************/
BOOL GIFExportOptions::SetAnimationDelay(CENTISECONDS Delay)
{
	m_AnimDelay = Delay;
	return TRUE;
}


/********************************************************************************************

>	GIFDisposalMethod GIFExportOptions::GetAnimationRestore() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	The disposal method to be used in animated GIFs
	Purpose:	Support function to obtain export options

********************************************************************************************/
GIFDisposalMethod GIFExportOptions::GetAnimationRestore()
{
	// return m_AnimDispose;
	GIFDisposalMethod Restore = m_pBitmapList[RestoreIndex]->GetActualBitmap()->GetAnimationRestoreType();
	RestoreIndex++;
	return Restore;
}


/********************************************************************************************

>	BOOL GIFExportOptions::SetAnimationRestore(const GIFDisposalMethod& DisposalMethod)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		DisposalMethod : The desired DisposalMethod to be set in the animated GIF
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetAnimationRestore(), enum GIFDisposalMethod

********************************************************************************************/
BOOL GIFExportOptions::SetAnimationRestore(const GIFDisposalMethod& DisposalMethod)
{
	m_AnimDispose = DisposalMethod;
	return TRUE;
}


/********************************************************************************************

>	UINT32 GIFExportOptions::GetAnimationLoopCount() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	How loopy our animated GIF will be
	Purpose:	Support function to obtain export options
	See Also:	SetAnimationLoopCount()

********************************************************************************************/
UINT32 GIFExportOptions::GetAnimationLoopCount() const
{
	return m_AnimLoop;
}


/********************************************************************************************

>	BOOL GIFExportOptions::SetAnimationLoopCount(UINT32 LoopCount)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Inputs:		LoopCount : For Animated GIFs: 0 = Repeat forever, 1 = One shot animation.
				n = Plays animation n times.
	Returns:	TRUE if set successfully
				FALSE otherwise
	Purpose:	Support function to set up information for the ensuing export
	See Also:	GetAnimationLoopCount()

********************************************************************************************/
BOOL GIFExportOptions::SetAnimationLoopCount(UINT32 LoopCount, BOOL Value)
{
	if(Value)
	{
		m_AnimLoop = ZERO;
		return TRUE;
	}
	else
	{
		m_AnimLoop = LoopCount;
		return TRUE;
	}
}





/********************************************************************************************

	Preference:		ExportGIFtype
	Section:		Filters
	Range:			0 to 3
	Purpose:		Allows a default GIF type to be remembered for the next export GIF
					operation.
	SeeAlso:		-

********************************************************************************************/


/********************************************************************************************

>	TI_GIFFilter::TI_GIFFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	Constructor for an TI_GIFFilter object.  The object should be initialised
				before use.
	SeeAlso:	TI_GIFFilter::Init

********************************************************************************************/

TI_GIFFilter::TI_GIFFilter() : MaskedFilter()
{
	ImportMsgID = _R(IDN_IMPORTMSG_GIF);
	Flags.CanImport = TRUE;
#ifndef STANDALONE
	Flags.CanExport = TRUE;
	Flags.CanExportMultipleImages = TRUE;
#else
	Flags.CanExport = FALSE;
	Flags.CanExportMultipleImages = FALSE;
#endif
	FilterID = FILTERID_TI_GIF;

	ExportMsgID = _R(IDN_EXPORTMSG_GIF);			// "Preparing GIF file..."

	ExportingMsgID = _R(IDN_EXPORTINGMSG_GIF);		// "Exporting GIF file..."

	// Special Mask prepartion stage ID
	Export2ndStageMsgID = _R(IDN_MASKINGMSG_GIF);	// "Preparing mask for GIF file..."

	ExportRegion = NULL;

	// Initalise flags
	m_DoingAnimation = FALSE;
	m_AnimationFrame = 0;
	m_LeftOffset = 0;
	m_TopOffset = 0;

	m_PreviousMethod = GDM_BACKTOBACK;
}

/********************************************************************************************

>	BOOL TI_GIFFilter::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an TI_GIFFilter object.
	Errors:		Will fail if not enough memory to initialise.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL TI_GIFFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new GIFOILFilter(this);
	if (pOILFilter==NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDN_FILTERNAME_GIF));
	FilterInfo.Load(_R(IDN_GIF_FILTERINFO));

	if (!GIFExportOptions::Declare())
	{
		return FALSE;
	}

	// All ok
	return TRUE;
}


/********************************************************************************************

>	virtual BitmapImportOptions* TI_GIFFilter::CreateImportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	Overrides base class to provide GIF specific options

********************************************************************************************/
BitmapImportOptions* TI_GIFFilter::CreateImportOptions() const
{
	GIFImportOptions* pOptions = new GIFImportOptions;
	return (BitmapImportOptions*)pOptions;
}



/********************************************************************************************

>	virtual BOOL TI_GIFFilter::BOOL ReadFromFile(OILBitmap* pOilBitmap,
												 BaseCamelotFilter* pFilter,
												 CCLexFile* pFile, BOOL IsCompressed);


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
				pFilter			- the BaseCamelotFilter which provides functions like progress update
								  (not used at the moment 20/01/97)
				pFile			- the CCFile class to use to read the data from
				IsCompressed	- Flag to say the bitmap is compressed or not.  
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				It is used by the web/native filters to pull out a bitmap definition from
				inside a bitmap definition record.
				It is also used for extracting previews.
				IsCompressed is only used for BMP/BMPZIP type bitmaps at present.
				Assumes:
					pFile has already been opened up for reading
					pFilter has been set up for reading the data e.g. progress bar 
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL TI_GIFFilter::ReadFromFile( OILBitmap* pOilBitmap, BaseCamelotFilter* pFilter,
								 CCLexFile* pFile, BOOL IsCompressed)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"TI_GIFFilter::ReadFromFile null OilBitmap pointer");
	// no pFilter means we don't get a progress WEBSTER-Martin-20/01/97
	// as we are calling this from BaseFileDialog::GetPreviewBitmapFromFile we don't have
	// a BaseCamelotFilter. However GIFUtil::ReadImage expects a 100% progress bar.
	// ERROR2IF(pFilter == NULL,FALSE,"TI_GIFFilter::ReadFromFile null pFilter pointer");
	ERROR2IF(pFile == NULL,FALSE,"TI_GIFFilter::ReadFromFile null pFile pointer");

	if (!pFilter)
		BeginSlowJob(-1);	// GIFUtil::ReadImage expects a progress bar.

	// Try to import bitmap as usual binary BMP file.
	CWxBitmap* pWBitmap = (CWxBitmap*) pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);

	// Set a default transparent colour
	INT32 TransColour = -1;
	SetTransColour(TransColour);

	// Blank the offsets just in case
	m_LeftOffset = 0;
	m_TopOffset = 0;

	// Read from file, no header and using pFilter for progress bar updates
	// 20/01/97 In fact pFilter is not used so NULL ptr ok
	if (!GIFUtil::ReadFromFile(pFile, pInfo, pBytes, &TransColour, NULL, pFilter))
	{
		if (!pFilter)
			EndSlowJob();
		return FALSE;
	}

	SetTransColour(TransColour);
	UINT32 Bpp = pWBitmap->GetBPP();
	if (TransColour != -1 && Bpp <= 8)
		pOilBitmap->SetTransparencyIndex(TransColour);

	SetLastBitmap();		// can only import one bitmap at the moment

	if (!pFilter)
		EndSlowJob();

	// Everything went ok and we imported the bitmap ok
	return TRUE;
}

/********************************************************************************************

>	BOOL TI_GIFFilter::ReadFromFile(OILBitmap* pOilBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/95
	Inputs:		pOilBitmap	pointer to the oil bitmap data to be filled in
	Outputs:	Will have filled in BMInfo	pointer to the bitmap header to fill in
									BMBytes	pointer to the bitmap data to fill in
	Purpose:	Actually does the process of reading a bitmap from a file.
				Inherited classes override this to read in different file formats.
				
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL TI_GIFFilter::ReadFromFile(OILBitmap* pOilBitmap)
{
	ERROR2IF(pOilBitmap == NULL,FALSE,"TI_GIFFilter::ReadFromFile null OilBitmap pointer");
	
	// Try to import the bitmap as a GIF file.
		
	CCLexFile* pImportFile = GetImportFile();
	ERROR2IF(pImportFile==NULL,FALSE,"TI_GIFFilter::ReadFromFile - No import file");

	UINT32 ImportMsgId = GetImportMsgID();		
	String_64 ProgressString(ImportMsgId);
	ProgressString = GetImportProgressString(pImportFile, ImportMsgId);

	// Set a default transparent colour
	INT32 TransColour = -1;
	SetTransColour(TransColour);
	INT32 nBitmapToRead = GetBitmapNumber();

	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	
	LPBITMAPINFO *pInfo = &(pWBitmap->BMInfo);
	LPBYTE *pBytes = &(pWBitmap->BMBytes);
	// Receives the Bitmap delay/Restore value from the Import.
	UINT32 Delay=0;
	GIFDisposalMethod Restore = GDM_LEAVE;

	// Blank the offsets just in case
	m_LeftOffset = 0;
	m_TopOffset = 0;

	// The GIF filter liked it very much and so use it, showing progress bar
	if (GIFUtil::ReadFromFile(pImportFile, pInfo, pBytes, &TransColour,nBitmapToRead, &ProgressString, NULL,
							  &Delay, &Restore, &m_LeftOffset, &m_TopOffset))
	{
		// Set the Delay\Restore value of the OILBitmap.
		pOilBitmap->SetBitmapAnimDelay(Delay);
		pOilBitmap->SetAnimationRestoreType(Restore);

		// Set the offset positions
		pOilBitmap->SetLeftOffset(m_LeftOffset);
		pOilBitmap->SetTopOffset(m_TopOffset);

		// Pass on the transparent colour that we found in the loading process
		// -1 if none found.  
		SetTransColour(TransColour);
		pOilBitmap->SetTransparencyIndex(TransColour);
		if (nBitmapToRead == -1)
			SetLastBitmap();
		else
			SetBitmapNumber(nBitmapToRead);
		return TRUE;
	}
	
	return FALSE;
}

/********************************************************************************************

>	BOOL TI_GIFFilter::GetDragAndDropTranslation(ImportPosition *pPos, DocRect BoundsRect, 
												 Coord* Offset)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> based on Tim function
	Created:	15/5/97
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
	SeeAlso:	Filter::DoImport

********************************************************************************************/

BOOL TI_GIFFilter::GetDragAndDropTranslation(ImportPosition *pPos, DocRect BoundsRect, 
											 Coord* Offset)
{
	// First check to se if we actually have a drag and drop point.
	if (pPos == NULL || pPos->pSpread == NULL)
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
	// This is the default way of doing it. We actually want the bounding box of the animation
	// centered on the drop point with the inidividual frames positioned inside this.
	// The x and y offsets are handled latter so we must just position the rectangle to the left
	// of the main animations' bounding box
	UINT32 GIFWidth = GIFUtil::GetGlobalWidth();
	UINT32 GIFHeight = GIFUtil::GetGlobalHeight();
	if (GIFWidth > 0 && GIFHeight > 0)
	{
		// GIFs have no concept of DPI so assume 96
		const double dpi = 96.0;
		const INT32 PixelSize = (INT32) ((72000.0 / dpi) + 0.5);	// Size of output pixel in millipoints

		// We have been told how many pixels the animation is in size so we should just
		// multiply by 72000 millipoints per inch and then divide by the pixels per inch or dpi.
		MILLIPOINT AnimationWidth = (INT32) (((GIFWidth * 72000.0) / dpi) + 0.5);
		MILLIPOINT AnimationHeight = (INT32) (((GIFHeight * 72000.0) / dpi) + 0.5);
		// Add these offsets onto the one that is passed in
		DocCoord Centre;
		Centre.x = (AnimationWidth) / 2;
		Centre.y = (AnimationHeight) / 2;

		// If the height or width is an odd number then we may be positioning on half pixel
		// boundaries. So snap to the nearest whole pixel boundary.
		Centre.x -= Centre.x % PixelSize;
		Centre.y -= Centre.y % PixelSize;

		Offset->x = -Centre.x;
		Offset->y = -Centre.y;

		// The bitmaps will now be so that their bottom left hand corner is at the corner
		// of the animation. So we need to move it up so that its top left hand corner is
		// at the top left of the animation.
		Offset->y += AnimationHeight - (BoundsRect.hi.y - BoundsRect.lo.y);
	}
	else
	{
		// No - set offset to 0 and return FALSE to indicate we can't cope with this.
		Offset->x = 0;
		Offset->y = 0;
	}

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
}

/********************************************************************************************

>	virtual BOOL TI_GIFFilter::SetFlagsFromBitmap(Layer * pLayer, KernelBitmap * pBitmap,
												  UINT32 nBitmapToRead)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/5/97
	Inputs:		pLayer			the layer that the bitmap is being put on
				pBitmap			the bitmap being placed on the layer
				nBitmapToRead	the number of the bitmap in the sequence
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	When loading in multiple bitmaps and placing them on new layers, we should
				give the filter an opportunity to set the layer flags from the bitmap. In the
				case of a GIF filter this would allow it to set the solid and overlay flags.

********************************************************************************************/

BOOL TI_GIFFilter::SetFlagsFromBitmap(Layer * pLayer, KernelBitmap * pBitmap,
									  UINT32 nBitmapToRead)
{
	if (pBitmap && pLayer)
	{
		// transfer the disposal method from the bitmap to the layer
		GIFDisposalMethod method = pBitmap->GetAnimationRestoreType();
		// First frame is always restore background
		if (nBitmapToRead == 1)
			m_PreviousMethod = GDM_BACKTOBACK;
		
		// REM use the previous frame's restore method, not this one!
		switch (m_PreviousMethod)
		{
			case GDM_NONE:
			case GDM_LEAVE:
				// Always set frame overlay
				pLayer->SetOverlay(TRUE);
				// As we are setting overlay, solid is not required
				pLayer->SetSolid(FALSE);
				// This seems to work but may need some extra clauses....
				//if (nBitmapToRead == 1)
				//	pLayer->SetSolid(TRUE);
				//else
				//	pLayer->SetSolid(FALSE);
				break;
			case GDM_PREVIOUS:
			case GDM_BACKTOBACK:
				// Always set frame overlay false and solid false
				pLayer->SetOverlay(FALSE);
				pLayer->SetSolid(FALSE);
				break;
		}

		// Note the new restore method for the next frame
		m_PreviousMethod = method;

		// transfer the delay from the bitmap to the layer
		UINT32 delay = pBitmap->GetDelay();
		pLayer->SetFrameDelay(delay);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL TI_GIFFilter::AddOffsetFromBitmap(DocCoord * pOffset)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/97
	Inputs:		pOffset			the present offset in the bitmap
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	When loading in a bitmap, the bitmap definition may conatin an offset. We need
				to add this to the offset that we are given so that the bitmap is loaded at
				the correct position.
				This uses the class variables that were set up during the GIF loading process
				to add the offset onto the import position.

********************************************************************************************/

BOOL TI_GIFFilter::AddOffsetFromBitmap(DocCoord * pOffset)
{
	if (pOffset && m_LeftOffset >= 0 && m_TopOffset >= 0)
	{
		// GIFs have no concept of DPI so assume 96
		const UINT32 dpi = 96;
		// We have been told how many pixels to offset by so we should just
		// multiply by 72000 millipoints per inch and then divide by the pixels per inch or dpi.
		MILLIPOINT ExtraOffsetX = (m_LeftOffset * 72000) / dpi;
		MILLIPOINT ExtraOffsetY = (m_TopOffset * 72000) / dpi;
		// Add these offsets onto the one that is passed in
		pOffset->x += ExtraOffsetX;
		// Remember, we have been told the offset from the top, so we need to move downwards
		pOffset->y -= ExtraOffsetY;
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL TI_GIFFilter::SetAnimationPropertiesFromLoaded(Spread * pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/97
	Inputs:		pSpread		the spread that the bitmaps have been put on
	Returns:	TRUE if was ok, FALSE otherwise.
	Purpose:	When loading in multiple bitmaps and placing them on new layers, we should
				give the filter an opportunity after all the bitmaps have been loaded, to go
				and look at the bitmaps and set the animation properties accordingly.
				In the case of a GIF filter this would allow it to set the dithering to none
				and the palette to something closer to what has been loaded.
				Assumes that each bitmap has been added to a separate layer and that the
				referenced bitmap in the layer is set to this loaded bitmap.

********************************************************************************************/

BOOL TI_GIFFilter::SetAnimationPropertiesFromLoaded(Spread * pSpread)
{
	ERROR2IF(pSpread == NULL,FALSE,"SetAnimationPropertiesFromLoaded pSpread = NULL");
	
	// Find out about the palettes that the bitmaps have
	BOOL SamePalettes		= FALSE;
	BOOL OurBrowserPalette	= FALSE;
	BOOL AllHaveBitmaps		= FALSE;
	BOOL AllWithinCount		= FALSE;
	UINT32 PaletteEntries		= 0;
	CheckSingleBitmapsOnSpread(pSpread, &SamePalettes, &OurBrowserPalette, &AllHaveBitmaps,
							   &AllWithinCount, &PaletteEntries);

	// Grab the present animation properties from the spread
	AnimPropertiesParam *pProperties = &pSpread->GetSpreadAnimPropertiesParam();
	if (pProperties != NULL)
	{
		// Make any changes that we require
		pProperties->SetDither(XARADITHER_NONE);
		
		if (SamePalettes && OurBrowserPalette)
		{
			// Set ourselves a global browser palette
			pProperties->SetPaletteCols(PALCOL_BROWSER);
			pProperties->SetPalette(PAL_GLOBAL);
		}
		else if (SamePalettes)
		{
			// Set ourselves a global optimised palette
			pProperties->SetPaletteCols(PALCOL_OPTIMIZED);
			pProperties->SetPalette(PAL_GLOBAL);
			if (PaletteEntries > 0)
				pProperties->SetNumColsInPalette(PaletteEntries);
		}
		else
		{
			// Set ourselves a local optimised palette
			pProperties->SetPaletteCols(PALCOL_OPTIMIZED);
			pProperties->SetPalette(PAL_LOCAL);
			// Ensure that the optimised palette contains
			if (PaletteEntries > 0)
				pProperties->SetNumColsInPalette(PaletteEntries);
		}

		// Put back the new animation properties
		pSpread->SetSpreadAnimPropertiesParam(*pProperties);

		// Tell anybody that is interested about the changes	
		BROADCAST_TO_ALL(SpreadMsg(pSpread, SpreadMsg::ANIMATIONPROPERTIESCHANGED));
	}

	return TRUE;
}

/********************************************************************************************

>	virtual void TI_GIFFilter::CleanUpAfterExport()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/9/95
	Purpose:	Cleans up the memory allocated at the end of Exporting or when exporting has
				been aborted for some reason. Does its cleaning up and then calls the
				baseclass version to do its stuff,  - used
				when the import process ends, either normally or abnormally. Override if
				extra things are required.
	SeeAlso:	BaseBitmapFilter::PrepareToExport(); BaseBitmapFilter::CleanUpAfterExport();
	Scope: 		Protected

********************************************************************************************/

void TI_GIFFilter::CleanUpAfterExport()
{
	// Called right at the end of the export process or when the epxort has been aborted
	// Clean up any objects unique to this class.
	// Free up any DIBs that we might have left lying around on the export
	if (pDestBMInfo && pDestBMBytes)
	{
		FreeDIB( pDestBMInfo, pDestBMBytes );
		pDestBMInfo = NULL;
		pDestBMBytes = NULL;	
	}

	// reset the filter settings, as the preview export might have messed with them

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Now call the baseclass version to do its stuff
	BaseBitmapFilter::CleanUpAfterExport();
}

/********************************************************************************************

>	BOOL TI_GIFFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	See BaseBitmapClass for interface details
	Notes:		Initializes GIF specific members
	Scope: 		Protected

********************************************************************************************/
BOOL TI_GIFFilter::GetExportOptions(BitmapExportOptions* pOptions)
{

//[MD]

#ifndef STANDALONE
	ERROR2IF(pOptions == NULL, FALSE, "NULL Args");

	GIFExportOptions* pGIFOptions = (GIFExportOptions*)pOptions;
	ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");

	// the depth we ask GDraw to render is always 32-bit, so we can get transparency
	// we have to convert for other formats	when writing the actual bytes to the file
	SetDepthToRender(32);

	// We haven't written the header yet
	WrittenHeader = FALSE;

	// We are a first pass render and not doing the mask, by default
	SecondPass = FALSE;
	DoingMask = FALSE;

	// Default will range from 0 to 4 types which correspond to the combinations of
	// on/off interlaced and on/off transparent
	
	// Determine the filter type currently in use in Accusoft format
	s_FilterType = (pGIFOptions->GetSelectionType() == SOMEBITMAPS) ? TI_GIF_ANIM : TI_GIF;
	pGIFOptions->SetFilterType(s_FilterType);
	m_DoingAnimation = (pGIFOptions->GetSelectionType() == SOMEBITMAPS);
	
	// This is ok as we are using a modal dialog box	
	BOOL Ok = FALSE;
	
	if (s_FilterType == TI_GIF)
	{
		OpDescriptor* pOpDes = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFTABDLG);
		if (pOpDes != NULL)
		{
			// set up the data for the export options dialog
			OpParam Param((void *)pOptions, (void *)this);

			// invoke the dialog
			pOpDes->Invoke(&Param);

			// SMFIX
			// we have brought the dlg up so get the options from the dlg as the graphic type may have changed
			pOptions = BmapPrevDlg::m_pExportOptions;

			// check for valid options
			//  This may get messed up, so have to use the second line below.
			Ok = BmapPrevDlg::m_bClickedOnExport;
		}
		else
		{	
			ERROR3("Unable to find OPTOKEN_BMAPPREVDLG");
		} 
	}
	else
	{
#ifndef EXCLUDE_FROM_XARALX
		// invoke the dialog
		Ok = BmpPrefsDlg::InvokeDialog( pOptions);
#endif
	}

	// Return with the ok/cancel state used on the dialog box
	return Ok;

#else
	return FALSE;
#endif
}


// SMFIX sjk 5/12/00 there used to be some junk in the call to GetExportOptions that assumed the
// filter type being used which could be changed by the GetExportOptions call itself
// therefore all this sort of stuff should be called on the correct known filter using this
// call afterwards
void TI_GIFFilter::PostGetExportOptions(BitmapExportOptions* pOptions)
{
	// should be of this type
	GIFExportOptions* pGIFOptions = (GIFExportOptions*)pOptions;
	ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");

	// do the baseclass options
	MaskedFilter::PostGetExportOptions(pOptions);

	// do the specific to this class options
	SetDepthToRender(pGIFOptions->GetDepth());

	m_DoingAnimation = (pGIFOptions->GetSelectionType() == SOMEBITMAPS);

	// Filter type can be changed by the export options dialog box from say 
	// TI_GIF to TI_GIF_INTERLACED
	UINT32 Silliness = pGIFOptions->WantTransparent() ? 2 : 0;
	Silliness |= pGIFOptions->WantInterlaced() ? 1 : 0;
	if (Silliness >= 0 && Silliness < 4)
	{
		Compression = Silliness;
		// Compression ranges from 0 .. 3 so map this onto our filter types
//		FilterID = Silliness + TI_GIF;
		switch (Silliness)
		{
		case 0:	s_FilterType = TI_GIF; break;
		case 1:	s_FilterType = TI_GIF_INTERLACED; break;
		case 2:	s_FilterType = TI_GIF_TRANSPARENT; break;
		case 3:	s_FilterType = TI_GIF_TRANSINTER; break;
		case 4:	s_FilterType = TI_GIF_ANIM; break;
		}

		if (pGIFOptions->WantTransparent() && pGIFOptions->GetSelectionType() == SELECTION)
			DoingMask = TRUE;
	}
}


/********************************************************************************************

>	virtual UINT32 TI_GIFFilter::GetExportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/09/95
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Overides the baseclass form of the function so that during the two stage
				export process it can change the message.
	SeeAlso:	DoExport;

********************************************************************************************/

UINT32 TI_GIFFilter::GetExportMsgID()
{
	if (GeneratingOptimisedPalette())
		return _R(IDS_GENOPTPALMSGID);				// "Generating optimised palette..."

	GIFExportOptions* pGIFOptions = (GIFExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pGIFOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");
	
	// If we are exporting with transparency on and on first pass use the masking message
	// otherwise use the exporting message.
	if (pGIFOptions->GetSelectionType() == SELECTION && pGIFOptions->WantTransparent())
	{
		// Special 4 stage rendering operation
		// - Render selected objects to white background
		// - Render mask 1bpp
		// - Render all objects
		// - Save data out to disk
		if (!SecondPass)
			return Export2ndStageMsgID;			// "Preparing mask for GIF file..."
		else
			return Filter::GetExportMsgID();	// "Preparing GIF file..."
	}
	else
	{
		// Special 3 stage rendering operation
		// - Render objects to white background
		// - Render mask 1bpp
		// - Save data out to disk
		if (DoingMask)
			return Export2ndStageMsgID;			// "Preparing mask for GIF file..."
		else
			return Filter::GetExportMsgID();	// "Preparing GIF file..."
	}
}


/********************************************************************************************

>	BOOL TI_GIFFilter::EndWriteToFile( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		-
	Purpose:	Cleans up after writing the bitmap data out to a file. Inherited classes
				override this to write in different file formats.
				This is slightly different to most other bitmap filters in that it is here
				that the data actually gets written out to file, after doing the transparency
				translation, if required.
	Returns:	TRUE if worked, FALSE if failed.

********************************************************************************************/

BOOL TI_GIFFilter::EndWriteToFile( )
{
#ifndef STANDALONE

	if (GeneratingOptimisedPalette())
		return TRUE;		// No need to output anything

		//  Can reset the band number now.
	m_BandNumber = 0;

	GIFExportOptions* pGIFOptions = (GIFExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pGIFOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pGIFOptions->IS_KIND_OF(GIFExportOptions), "pGIFOptions isn't");

TRACEUSER( "Neville", _T("TI_GIFFilter::EndWriteToFile\n"));
	// Do the transparency translation just before we write out the data as a GIF.
	// This involves doing a 1 bpp export of the same area and using this to work
	// out which areas are transparent or not.
	// Only do this if the user has requested transparency and we outputting at 8bpp

	if (BadExportRender)
	{
		// Delete our whitearea bitmap
		if (pTempBitmapMask != NULL)
			CCFree(pTempBitmapMask);

		pTempBitmapMask = NULL;
	}

	BOOL ok = TRUE;
	BOOL SaveDataOut = TRUE;
	BOOL WantTransparent = pGIFOptions->WantTransparent();
	BOOL WantInterlaced = pGIFOptions->WantInterlaced();

	// Save the data out if required. Only if we exported ok.
	if (SaveDataOut && !BadExportRender)
	{
		// Now that we know the transparent index we can output the GIF header
		if (ok)
		{
			// Work out whether we should save as GIF87 or GIF89
			// If we are using animation or transparency then save out as GIF89
			BOOL Enhanced = m_DoingAnimation || WantTransparent;

			if (m_DoingAnimation)
			{
				if (m_AnimationFrame == 0)
				{
					// We force a transparent colour of -1 so the transparent index colour is not changed to white in the global colour table.
					ok = DestGIF.OutputGifFileHeader(OutputFile, Enhanced, -1);
					if (ok)
					{
						// Shall we output a Loop block ?

						// We only put a loop block in if the loop count is 0, or > 1
						// and we subtract 1 from values other than 0, so that the
						// loop count represent the number of times the animation
						// is 'played' rather than 'repeated' ....
						// 
						// ie.  0, loop block with value 0.  	Repeat forever.
						//		1, no loop block.				One shot animation.
						//		2, loop block with value 1.		Plays animation 2 times.
						//		3, loop block with value 2.		Plays animation 3 times.
						//		4, loop block with value 3.		Plays animation 4 times.
						//      etc ...
						UINT32 LoopCount = pGIFOptions->GetAnimationLoopCount();
						if (LoopCount != 1)
						{
							if (LoopCount > 1) LoopCount -= 1;

							// Output a loop block then ...
							ok = DestGIF.OutputAnimationControl(OutputFile, LoopCount);
						}
					}
				}
			}
			else
			{
				ok = DestGIF.OutputGifFileHeader(OutputFile, Enhanced, pGIFOptions->GetTransparencyIndex());
			}
		}

		// Actually write the destination bitmap out to the file showing an hourglass
		// and/or progress bar as we go. Always show the Exporting message.
		// Need to do in one go due to interlacing

		// Andy, 13-12-00: removed progress bar.
		// It goes by very quickly compared with the main rendering jobbie.
		// Also removed corresponding ContinueSlowJob from OutputGIF::BumpPixel
		if (ok)
		{
			String_64 ProgressString(ExportingMsgID);
			ProgressString = GetExportProgressString(OutputFile, ExportingMsgID);
			//BeginSlowJob(100, FALSE, &ProgressString);
		
			if (ok && (WantTransparent || m_DoingAnimation || WantInterlaced))
			{
				ok = DestGIF.OutputGifImageExtensionHeader(OutputFile, WantInterlaced,
						pGIFOptions->GetTransparencyIndex(),
						m_DoingAnimation ? pGIFOptions->GetAnimationDelay() : 0,
						m_DoingAnimation ? pGIFOptions->GetAnimationRestore() : 0);
			}
			// if we are doing an animation with an optimised palette then we will need
			// save out this palette as a local colour table (the first frame uses the global table)
			BOOL OutputLocalColourTable = (m_DoingAnimation && pOptimisedPalette != NULL && m_AnimationFrame != 0);
			if (ok)
				ok = DestGIF.OutputGifImageBits(OutputFile, DestGIF.GetDestBitmapBits(), WantInterlaced, OutputLocalColourTable);

			//EndSlowJob();
		}
	}

	return ok;

#else	

	return TRUE;

#endif

}


/********************************************************************************************

>	virtual BOOL TI_GIFFilter::SaveExportBitmapsToFile(CCLexFile* pFile, PathName* pPath, BitmapExportParam* pParam,
													   BOOL DontShowFileName = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/97
	Inputs:		pDiskFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pParam - the data to use when exporting the bitmaps
				DontShowFileName - if set to true then don't show the filename in the progress string
	Purpose:	Exports the bitmaps specified by the BitmapExportParam object to a multi-
				image capable filter. Each of the bitmaps will be saved to file using the
				settings specified.
	Returns:	TRUE if worked, FALSE if failed.
	SeeAlso:	GrabFrame Filter::DoExportBitmaps;

********************************************************************************************/

BOOL TI_GIFFilter::SaveExportBitmapsToFile(CCLexFile* pFile, PathName* pPath, BitmapExportParam* pParam,
										   BOOL DontShowFileName)
{
#if defined(DO_EXPORT) && !defined(EXCLUDE_FROM_XARALX)
	ERROR2IF(pFile == NULL || pPath == NULL, FALSE,"NULL Parameters");
	ERROR2IF(pParam == NULL,FALSE,"TI_GIFFilter::DoExportBitmaps null BitmapExportParam specified");

	BOOL ok = TRUE;

	// Make sure the Region pointer is NULL for the time being
	ExportRegion = NULL;

	ERROR2IF(!pParam->IS_KIND_OF(GIFAnimationExportParam), FALSE, "pParam isn't GIFAnimationExportParam");
	GIFAnimationExportParam* pGIFOptions = (GIFAnimationExportParam*)pParam;
	UINT32 BitmapCount					= pGIFOptions->GetBitmapCount();		// Number of bitmaps in animation
	KernelBitmap * pBitmap				= NULL;
	WEB_PALETTE	GlobalLocalPal			= pGIFOptions->GetPalette();			// Global or local palette
	PALETTE_COLOURS	PaletteColours		= pGIFOptions->GetPaletteCols();		// Browser or Optimised palette
	BOOL SaveLocalPalette				= FALSE;								// Default to global palette
	if (GlobalLocalPal == PAL_LOCAL) // && PaletteColours == PAL_OPTIMIZED)
		SaveLocalPalette = TRUE;

	Spread * pSpread = Document::GetSelectedSpread();

	LPBITMAPINFO pOldInfo				= NULL;
	LPBYTE pOldBytes					= NULL;
	LPBITMAPINFO pDiffInfo				= NULL;
	LPBYTE pDiffBytes					= NULL;
	LPBITMAPINFOHEADER pTrueInfoHeader	= NULL;

	// The list of bitmap frames that we need to output as part of the animatation
	List BitmapFrameList;
	AnimatedGIFImage * pPrevGIFImage = NULL;
	for (UINT32 Index = 0; ok && (Index < BitmapCount); Index++)
	{
		// Get the bitmap pointer
		pBitmap = pGIFOptions->GetBitmap(Index);

		LPBITMAPINFO	pInfo	= NULL;
		LPBYTE			pBytes	= NULL;
		if (pBitmap)
		{
			pInfo	= pBitmap->GetBitmapInfo();
			pBytes	= pBitmap->GetBitmapBits();
		}	
		if (pBitmap && pInfo && pBytes)
		{
			// Get the useful information on the bitmap
			UINT32			Bpp		= pBitmap->GetBPP();
			INT32			TransColour = -1;
			if (Bpp <= 8)
			{
				 pBitmap->GetTransparencyIndex(&TransColour);
				 if (TransColour == -1 && pInfo->bmiColors[0].rgbRed == 255 && pInfo->bmiColors[0].rgbGreen == 255 && pInfo->bmiColors[0].rgbBlue == 255)
				 {
					 TRACEUSER( "SimonK", _T("taking 1st palette entry as transp since it is white\n"));
					 TransColour = 0;
				 }
			}

			// Recover the restore type assigned to the bitmap which we will then use
			// unless we override it further down
			GIFDisposalMethod FrameRestore = pBitmap->GetAnimationRestoreType();

			// Recover the delay from the bitmap itself
			CENTISECONDS FrameDelay = pBitmap->GetDelay();

			// If we are on the first bitmap then note the first bitmap's size as the
			// overall size of the animation
			if (ok && Index == 0)
			{
				// BITMAPINFO  consists of:-
				//    	BITMAPINFOHEADER    bmiHeader;
				//		RGBQUAD             bmiColors[1];
				// This is the real bitmap info header giving the overall size of the export
				pTrueInfoHeader = pBitmap->GetBitmapInfoHeader();
			}

			// If we have a previous frame then minimise the amount of bitmap which we will output
			// by working out the difference between the two frames and making everything else the
			// transparent colour.
			LPBITMAPINFO pOutputInfo	= pInfo;
			LPBYTE pOutputBytes			= pBytes;
			UINT32 LeftOffset				= 0;
			UINT32 TopOffset				= 0;
			
			// Recover the offsets from the bitmap itself. Only really useful if we are using
			// pre-loaded bitmaps as single items on the layer.
			LeftOffset					= pBitmap->GetLeftOffset();
			TopOffset					= pBitmap->GetTopOffset();

			// If we are in local optimised mode then we cannot diff the bitmaps as we are overlaying
			// and so a change in palette could be very bad indeed!
			BOOL DiffFailed = FALSE;
			if (pOldInfo && pOldBytes && !SaveLocalPalette)
			{
				// Pass in the present bitmap and get a new one back
				BOOL FoundBadOverlay = FALSE;

				if (pSpread->GetSpreadAnimPropertiesParam().GetIsBackGroundTransp() /*pGIFOptions->GetIsBackGroundTransp()*/)
				{
					// with a transparent background every overlay is a bad overlay
					FoundBadOverlay = TRUE;
				}
				else
				{
					ok = DIBUtil::GenerateDifferenceBitmap(pOldInfo, pOldBytes, pInfo, pBytes,
													   &pDiffInfo, &pDiffBytes,
													   TransColour, &FoundBadOverlay);
				}
				// If we have found that we cannot overlay the previous frame then
				// set the restore type to put the background back and forget about
				// the diff bitmap.
				// We must change the previous bitmap's restore type so access the previous
				// item in the output list
				if (FoundBadOverlay)
				{
					if (pPrevGIFImage)
						pPrevGIFImage->SetFrameRestore(GDM_BACKTOBACK);
					FrameRestore = GDM_BACKTOBACK;
				}
				else if (ok && pDiffInfo && pDiffBytes)
				{
					// If everything went ok then output the difference bitmap
					pOutputInfo = pDiffInfo;
					pOutputBytes = pDiffBytes;
					
					// Ensure that we have an overlay restore type
					FrameRestore = GDM_LEAVE;
					if (pPrevGIFImage)
						pPrevGIFImage->SetFrameRestore(GDM_LEAVE);
				}
				else
				{
					// Possible loaded an animation and now wanting to output it again
					// Use the left and top offsets in the bitmap
					// These should have been set up in CheckIfSingleBitmap();
					TRACEUSER( "Neville", _T("Something went wrong, outputting whole bitmap\n"));
					DiffFailed = TRUE;
					//LeftOffset = pBitmap->GetLeftOffset();
					//TopOffset = pBitmap->GetTopOffset();
					//FrameRestore = pBitmap->GetAnimationRestoreType();
				}
			}
			else if (pOldInfo && pOldBytes && pSpread->GetSpreadAnimPropertiesParam().GetIsBackGroundTransp())
			{
				// dont overlay transp images have them back to back
				if (pPrevGIFImage)
					pPrevGIFImage->SetFrameRestore(GDM_BACKTOBACK);
				FrameRestore = GDM_BACKTOBACK;
			}


			// Work out if we can shrink the bitmap down in size
			// Only do this if we are not on the first bitmap. Internet Explorer screws up
			// as it seems to assume that the size of the first frame is the size of the animation
			// rather than taking the true animation size.
			// This may re-allocate the difference bitmap for us to a new smaller size
			// In this case it will return a left and top offset for us to use.
			if (ok && pOutputInfo && pOutputBytes && Index != 0 &&
				LeftOffset == 0 && TopOffset == 0 && !DiffFailed)
			{
				ok = DIBUtil::GenerateSubRegionBitmap(pOutputInfo, pOutputBytes,
													  &pDiffInfo, &pDiffBytes, TransColour,
													  &LeftOffset, &TopOffset);
				if (ok && pDiffInfo && pDiffBytes)
				{
					// If everything went ok then output the sub-region bitmap
					pOutputInfo = pDiffInfo;
					pOutputBytes = pDiffBytes;
				}
				// Otherwise just output the whole bitmap
			}

			// Add an item to the list of frames to output
			AnimatedGIFImage * pGIFImage = new AnimatedGIFImage(TransColour, FrameRestore, FrameDelay,
																LeftOffset, TopOffset,
																pOutputInfo, pOutputBytes,
																pDiffInfo, pDiffBytes);
			if (pGIFImage == NULL)
			{
				ok = FALSE;
				break;
			}

			BitmapFrameList.AddTail(pGIFImage);
			
			// Remember this bitmap for later use
			pOldInfo = pInfo;
			pOldBytes = pBytes;

			pPrevGIFImage = pGIFImage;

			// blank these so as 
			pDiffInfo	= NULL;
			pDiffBytes	= NULL;
		}
	}

	if (!ok)
	{
		ERROR3("Failed to make the output list of GIF images");
		// Clean out the list of GIF images that we made ready for exporting
		BitmapFrameList.DeleteAll();
		return FALSE;
	}

	// **** Actually output the data to file
	
	// Used to open the file up before starting DoExport. But this meant a cancel on the export
	// options dialog had filled the file, if it was already present. So now up up here if
	// not open already. In the PreviewBitmap case the file will already be open.
	if (!pFile->isOpen())
	{
		if (pFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{
			ok = OpenExportFile((CCDiskFile*) pFile, pPath);
		}
		else
		{
			TRACEUSER( "JustinF", _T("Tried to open non-CCDiskFile in BaseBitmapFilter::DoExportBitmaps\n"));
			ok = FALSE;
		}
	}

	if (!ok)
	{
		ERROR3("Failed to open the export file for GIF images");
		// Clean out the list of GIF images that we made ready for exporting
		BitmapFrameList.DeleteAll();
		return FALSE;
	}

	// Make a note of the Disk file we are to use
	OutputFile = pFile;

	// Recover information on the animation 
	const BOOL WantInterlaced				= FALSE;		// We never want interlacing
	UINT32 LoopCount							= pGIFOptions->GetAnimLoop();			// Looping for the animation
	const CENTISECONDS AnimationDelay		= pGIFOptions->GetGlobalAnimDelay();	// Delay in CentiSeconds

	// Check that we have an animation size information block
	ERROR2IF(pTrueInfoHeader==NULL,FALSE,"TI_GIFFilter::WriteToFile BitmapInfoHeader pointer is null");

	UINT32	Index = 0;
	AnimatedGIFImage * pCurrentGIFImage = (AnimatedGIFImage *)BitmapFrameList.GetHead();
	while ( ok && pCurrentGIFImage)
	{
		// Tecover the info from the GIF image item
		// This is the real bitmap info header giving the overall size of the export
		LPBITMAPINFO pOutputInfo		= pCurrentGIFImage->GetOutputInfo();
		LPBYTE pOutputBytes				= pCurrentGIFImage->GetOutputBits();
		INT32 TransColour				= pCurrentGIFImage->GetTransColour();
		GIFDisposalMethod FrameRestore	= pCurrentGIFImage->GetFrameRestore();
		CENTISECONDS FrameDelay			= pCurrentGIFImage->GetFrameDelay();
		UINT32 LeftOffset					= pCurrentGIFImage->GetLeftOffset();
		UINT32 TopOffset					= pCurrentGIFImage->GetTopOffset();

		// BITMAPINFO  consists of:-
		//    	BITMAPINFOHEADER    bmiHeader;
		//		RGBQUAD             bmiColors[1];

		// This is the header for the bitmap that we are actually outputting
		LPBITMAPINFOHEADER pInfoHeader = &pOutputInfo->bmiHeader;
		ERROR2IF(pInfoHeader==NULL,FALSE,"TI_GIFFilter::WriteToFile BitmapInfoHeader pointer is null");
			
		LPRGBQUAD pPalette = &(pOutputInfo->bmiColors[0]);
		ERROR2IF(pPalette==NULL,FALSE,"TI_GIFFilter::WriteToFile palette pointer is null");

		// If we are on the first bitmap then out the file start information
		if (ok && Index == 0)
		{
			// We force a transparent colour of -1 so the transparent index colour is not changed to white in the global colour table.
			ok = DestGIF.OutputGifFileHeader(pFile, pTrueInfoHeader, TRUE, TransColour, NULL, pPalette);
			if (!ok)
				break;

			// Shall we output a Loop block ?

			// We only put a loop block in if the loop count is 0, or > 1
			// and we subtract 1 from values other than 0, so that the
			// loop count represent the number of times the animation
			// is 'played' rather than 'repeated' ....
			// 
			// ie.  0, loop block with value 0.  	Repeat forever.
			//		1, no loop block.				One shot animation.
			//		2, loop block with value 1.		Plays animation 2 times.
			//		3, loop block with value 2.		Plays animation 3 times.
			//		4, loop block with value 3.		Plays animation 4 times.
			//      etc ...
			if (LoopCount != 1)
			{
				if (LoopCount > 1) LoopCount -= 1;

				// Output a loop block then ...
				ok = DestGIF.OutputAnimationControl(pFile, LoopCount);
				if (!ok)
					break;
			}

			// For now force a restore type of restore background on the first frame 
			//AnimationRestore = GDM_BACKTOBACK;
		}

		// Actually write the destination bitmap out to the file showing an hourglass
		// and/or progress bar as we go. Always show the Exporting message.
		// Need to do in one go due to interlacing
		if (ok)
		{
			String_64 ProgressString(ExportingMsgID);
			// If the caller has requested it, then supress the filename
			// Useful if we are doing Browser preview so that the temp file does not
			// get shown.
			if (DontShowFileName)
				ProgressString = GetExportProgressString(NULL, ExportingMsgID);
			else
				ProgressString = GetExportProgressString(pFile, ExportingMsgID);
			BeginSlowJob(100, FALSE, &ProgressString);
		
			ok = DestGIF.OutputGifImageExtensionHeader( pFile, WantInterlaced,
														TransColour,
														FrameDelay,
														FrameRestore);

			// If we are doing an animation with an optimised palette then we will need to
			// save out this palette as a local colour table (the first frame uses the global table)
			BOOL OutputLocalColourTable = (SaveLocalPalette && Index != 0);
			if (ok)
				ok = DestGIF.OutputGifImageBits(pFile, pOutputBytes,
												WantInterlaced, OutputLocalColourTable,
												NULL,
												pInfoHeader->biWidth, pInfoHeader->biHeight, 
												LeftOffset, TopOffset,
												pPalette, pInfoHeader->biClrUsed, pInfoHeader->biBitCount);
		}

		Index ++;

		// Get the next item in the list
		pCurrentGIFImage = (AnimatedGIFImage *)BitmapFrameList.GetNext(pCurrentGIFImage);
	}

	// Finish the file off
	if (ok)
		ok = DestGIF.OutputGifTerminator(pFile);

	// Clean out the list of GIF images that we made ready for exporting
	BitmapFrameList.DeleteAll();

	// All done - deallocate dynamic objects, stop the progress display/hourglass
	// and return success. (Also closes file. Eh! Does it?).
	CleanUpAfterExport();

	return ok;
#endif
	return FALSE;
}

/********************************************************************************************

>	static BOOL TI_GIFFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		End - TRUE if this is the last block of the file.
				Bpp - output depth in terms of bits per pixel
				Compression - usually True if compression required, False otherwise
							  In the GIF case this is used to pass in the transparency and
							  interlace state.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.
				NOTE - ONLY COPES WITH End=TRUE currently
				AtEnd is ignored now and should always be set to TRUE.
				Unused at present due to static problems when cretaing the 1bpp bitmap. 
	SeeAlso:	WriteToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/

BOOL TI_GIFFilter::WriteDataToFile( BOOL End, UINT32 Bpp, UINT32 Compression)
{
	ERROR2(FALSE,"TI_GIFFilter::WriteDataToFile called when not implemented");
	return FALSE;
}


/********************************************************************************************

>	INT32 TI_GIFFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, 
								 UINT32 FileSize)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		Filename - name of the file.
				HeaderStart - Address of the first few bytes of the file.
				HeaderSize - the number of bytes in the header pointed to by FileStart.
				FileSize - the size of the whole file, in bytes.
	Returns:	0 => Not a GIF file.
				10 => It is a GIF file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/

INT32 TI_GIFFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
TRACEUSER( "Neville", _T("TI_GIFFilter::HowCompatible"));	
	// We need to remember what we thought of this file in our class variable.
	// So, set it to a nice default value at the start.
	GIFHowCompatible = 0;

	// Check that we've got enough data to do our check
	// This is really sizeof(GIFINFOHEADER) but this returns 14 instead of 13
	// as it rounds to the nearest word boundary
	const size_t GIFHeaderSize = sizeof(char)* 6 + sizeof(WORD) * 2 + sizeof(BYTE) * 3;
	if (HeaderSize < GIFHeaderSize)
	{
		// Not enough data - ignore this file.
		return 0;
	}

	// Check the header for the "GIF" signature.
	LPGIFINFOHEADER pHeader = (LPGIFINFOHEADER) HeaderStart;

	if (
		( strncmp( pHeader->giName, "GIF89a", 6 ) == 0 ) ||
	 	( strncmp( pHeader->giName, "GIF87a", 6 ) == 0 )
	   )
	{
		// the other fields in the GIFINFOHEADER don't really hold any useful information
		// and really finding GIF87a/89a should be good enough to determine that there is
		// a high chance that this is a GIF file.

		// Remember what we thought in our class variable.
		GIFHowCompatible = 10;
	}
	else
	{
		// No GIF signature - we don't want this file.
		GIFHowCompatible = 0;
	}
				
TRACEUSER( "Neville", _T("TI_GIFFilter::HowCompatible returning = %d\n"),GIFHowCompatible);
	// Return the found value to the caller.
	return GIFHowCompatible;
}

/********************************************************************************************

>	INT32 TI_GIFFilter::GetGifCompatibility()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Inputs:		-
	Returns:	The value we found in the HowCompatible call.
				0 => Not a GIF file.
				10 => It is a GIF file.
	Purpose:	Determine if this filter can load the specified file.

********************************************************************************************/

INT32 TI_GIFFilter::GetGifCompatibility()
{
	return GIFHowCompatible;
}

/********************************************************************************************

>	virtual BOOL TI_GIFFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		Pointer to the bitmap to save.
				Dpi of the bitmap to be saved
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.  Inherited classes override this to write
				in different file formats.
	SeeAlso:	WriteDataToFile(); AccusoftFilters::WriteToFile; AccusoftFilters::WriteDataToFile;

********************************************************************************************/

BOOL TI_GIFFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, double Dpi)
{
	ERROR2IF(pKernelBitmap == NULL,FALSE,"TI_GIFFilter::WriteBitmapToFile null bitmap pointer specified");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"TI_GIFFilter::WriteBitmapToFile null oil bitmap pointer");

	// Now get the pointer to the info header and actual bits data.
	// Need to use the actual bitmap pointer
	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	LPBITMAPINFO pInfo = pWBitmap->BMInfo;
	LPBYTE pBytes = pWBitmap->BMBytes;
//	UINT32 Bpp = pWBitmap->GetBPP();

	// Now, save the data out showing the correct progress string
	String_64 ProgressString(ExportingMsgID);
	BOOL ok = FALSE;

	ok = WriteToFile(OutputFile, pInfo, pBytes, &ProgressString);
	
	return ok;
}

/********************************************************************************************

>	virtual BOOL TI_GIFFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap,
												 BaseCamelotFilter* pFilter,
												 CCLexFile* pFile, INT32 Compression);


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/96
	Inputs:		pKernelBitmap	- Pointer to the bitmap to be exported.
				pFilter			- Pointer to the BaseCamelot filter which provides progress functions
				pFile			- Pointer to the CCFile class to use for export
				Compression		- used to flag how much compression of the data is required.
	Returns:	TRUE if worked, FALSE if errored.
	Purpose:	Physically put the bitmap into the disk.  Inherited classes override this to write
				in different file formats.
				This is used by the native/web format to output the actual bitmap data content
				of a bitmap definition record. The function can assume that the CCFile is open
				and ready for writing and must use the functions provided by pFilter to update
				the progress system.
	SeeAlso:	BitmapListComponent::SaveBitmapDefinition;

********************************************************************************************/

BOOL TI_GIFFilter::WriteBitmapToFile(KernelBitmap* pKernelBitmap, BaseCamelotFilter *pFilter,
										 CCLexFile* pFile, INT32 Compression)
{
	ERROR2IF(pKernelBitmap == NULL,FALSE, "TI_GIFFilter::WriteBitmapToFile null pKernelBitmap");
	ERROR2IF(pFilter == NULL,FALSE, "TI_GIFFilter::WriteBitmapToFile null pFilter");
	ERROR2IF(pFile == NULL,FALSE, "TI_GIFFilter::WriteBitmapToFile null pFile");

	// Get a pointer to the actual bitmap so that we can get some details from it.
	OILBitmap *pOilBitmap = pKernelBitmap->ActualBitmap;
	ERROR2IF(pOilBitmap == NULL,FALSE,"TI_GIFFilter::WriteBitmapToFile null oil bitmap pointer");

	// Now get the pointer to the info header and actual bits data.
	// Need to use the actual bitmap pointer
	CWxBitmap* pWBitmap = (CWxBitmap*)pOilBitmap;
	LPBITMAPINFO Info = pWBitmap->BMInfo;
	LPBYTE Bytes = pWBitmap->BMBytes;
	UINT32 Bpp = pWBitmap->GetBPP();

	// Now, save the data out showing the correct progress string
	BOOL ok = FALSE;
	// We must output with no interlacing and no transparency as otherwise things like
	// the progress bar update will be screwed, so force the filter type to be simple.
	s_FilterType = TI_GIF;

	BOOL Interlace = FALSE;	// Don't use interlacing by default
	INT32 Transparent = -1;	// colour or -1 = no transparency
	if (Bpp <= 8)
		pOilBitmap->GetTransparencyIndex(&Transparent);

	// Write to file, using pFilter for progress bar updates
	ok = WriteToFile(pFile, Info, Bytes, Interlace, Transparent, pFilter);
	
	return ok;
}


/********************************************************************************************

>	virtual BOOL TI_GIFFilter::IsThisBppOk(UINT32 Bpp)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/95
	Inputs:		Bpp or Colour depth.
	Returns:	TRUE if this filter can cope with this colour depth, FALSE otherwise.
	Purpose:	Check if this Bitmap filter can cope with saving at this Bpp/Colour depth.
	SeeAlso:	OpMenuExport::DoWithParam;

********************************************************************************************/

BOOL TI_GIFFilter::IsThisBppOk(UINT32 Bpp)
{
	// Webster - RanbirR
	// Animated GIF's do not support 24 colour bitmaps.
	// However, allow this format here, since we later convert to 8 Bpp.
			
	return (Bpp == 24 || Bpp == 8 || Bpp == 4 || Bpp == 1);
}

/********************************************************************************************

>	BOOL TI_GIFFilter::WriteToFile ( CCLexFile *File, LPBITMAPINFO Info, LPBYTE Bits,
									 BOOL Interlace, INT32 Transparent,
									 BaseCamelotFilter *pFilter = NULL )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		File		An opened CCFile that can be written to. It should be positioned at the
							start. Caller is responsible for closing it. The file needs to be in
							Binary mode.
				Info		BITMAPINFO structure for the dib.
				Bits		The bitmap data itself
				Interlace	True if want interlacing, false otherwise
				Transparent	-1 if no transparent colour required, 0 to maximum palette entry
							to specify the transparent palette entry
				pFilter		is an alternative way of handling the progress bar, assume the
							progress bar has been start and just call the IncProgressBarCount in 
							BaseCamelotFilter to do the progress bar update.
							Defaults to NULL i.e. no progress bar.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Write a bitmap in memory straight out as a GIF to file with no rendering or
				conversion between different colour depths (apart from 32 to 24) or resolution.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
				This function is used by the native/web file format to save a bitmap as a GIF
				directly into the file. Hence why it needs to be completely different to the 
				version below which is used for extras like saving animated gifs.
				(caller should close file)
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static
	SeeAlso:	AccusoftFilters::WriteToFile; DIBUtil::WriteToFile;

********************************************************************************************/

BOOL TI_GIFFilter::WriteToFile( CCLexFile *File, LPBITMAPINFO Info, LPBYTE Bits,
								BOOL Interlace, INT32 Transparent,
								BaseCamelotFilter *pFilter )
{
	ERROR2IF(File==NULL,FALSE,"TI_GIFFilter::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"TI_GIFFilter::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"TI_GIFFilter::WriteToFile Bits pointer is null");

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
	ERROR2IF(pInfoHeader==NULL,FALSE,"TI_GIFFilter::WriteToFile BitmapInfoHeader pointer is null");
		
	LPRGBQUAD pPalette = &(Info->bmiColors[0]);
	ERROR2IF(pPalette==NULL,FALSE,"TI_GIFFilter::WriteToFile palette pointer is null");

	// Output the GIF data
	BOOL ok = TRUE;
	if (Transparent == -1)
		ok = DestGIF.OutputGifFileHeader(File, pInfoHeader, FALSE, -1, NULL, pPalette);
	else
		ok = DestGIF.OutputGifFileHeader(File, pInfoHeader, TRUE, Transparent, NULL, pPalette);
	if (ok && Transparent != -1)
		ok = DestGIF.OutputGifImageExtensionHeader(File, Interlace, Transparent, 0, 0);
	if (ok)
		ok = DestGIF.OutputGifImageBits(File, Bits, Interlace, FALSE, pFilter);
	
	if (ok)
		ok = DestGIF.OutputGifTerminator(File);

	DestGIF.TidyUp();
	
	return ok;
}

/********************************************************************************************

>	BOOL TI_GIFFilter::WriteToFile ( CCLexFile *File, LPBITMAPINFO Info, LPBYTE Bits,
									 String_64 *ProgressString = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		File	An opened CCFile that can be written to. It should be positioned at the
						start. Caller is responsible for closing it. The file needs to be in
						Binary mode.
				Info	BITMAPINFO structure for the dib.
				Bits	The bitmap data itself
				ProgressString allows the user to specify whether they require a progress
						hourglass or not. If NULL then none is shown, otherwise an progress	bar 
						is shown using the text supplied. Defaults to NULL i.e. no progress bar.
	Outputs:	-
	Returns:	TRUE if worked, FALSE if failed (error will be set accordingly but not reported)
	Purpose:	Write a bitmap in memory straight out as a GIF to file with no rendering or
				conversion between different colour depths (apart from 32 to 24) or resolution.
				***Errors on 16-bit builds***
				A progress hourglass can be shown if required.
				This function is used by the save bitmap button on the bitmap gallery. All
				other bitmap export uses the OutputDIB class instead as this copes with using
				a render region and converting from 32 to the destination format.
				(caller should close file)
	Errors:		Calls SetError on FALSE returns.
	Scope:		Static
	SeeAlso:	AccusoftFilters::WriteToFile; DIBUtil::WriteToFile;

********************************************************************************************/

BOOL TI_GIFFilter::WriteToFile( CCLexFile *File, LPBITMAPINFO Info, LPBYTE Bits,
							    String_64 *ProgressString)
{
	ERROR2IF(File==NULL,FALSE,"TI_GIFFilter::WriteToFile File pointer is null");
	ERROR2IF(Info==NULL,FALSE,"TI_GIFFilter::WriteToFile BitmapInfo pointer is null");
	ERROR2IF(Bits==NULL,FALSE,"TI_GIFFilter::WriteToFile Bits pointer is null");

	// If the caller has specified a string then assume they require a progress bar
	// Start it up.
	if (ProgressString != NULL)
		BeginSlowJob(100, FALSE, ProgressString);

	// BITMAPINFO  consists of:-
	//    	BITMAPINFOHEADER    bmiHeader;
	//		RGBQUAD             bmiColors[1];
	LPBITMAPINFOHEADER pInfoHeader = &Info->bmiHeader;
	ERROR2IF(pInfoHeader==NULL,FALSE,"TI_GIFFilter::WriteToFile BitmapInfoHeader pointer is null");
		
	LPRGBQUAD pPalette = &(Info->bmiColors[0]);
	ERROR2IF(pPalette==NULL,FALSE,"TI_GIFFilter::WriteToFile palette pointer is null");

	// Set up our format type flags.
	BOOL Interlace = TRUE;	// Use interlace or not
	INT32 Transparent = -1;	// colour or -1 = no transparency
	BOOL MakeTransparent = FALSE;

	switch (s_FilterType)
	{
		default:
		case TI_GIF:
			Interlace 		= FALSE;
			MakeTransparent = FALSE;
			break;
		case TI_GIF_INTERLACED:
			Interlace 		= TRUE;
			MakeTransparent = FALSE;
			break;
		case TI_GIF_TRANSPARENT:
			Interlace 		= FALSE;
			MakeTransparent = TRUE;
			break;
		case TI_GIF_TRANSINTER:
			Interlace 		= TRUE;
			MakeTransparent = TRUE;
			break;
	}
TRACEUSER( "Neville", _T("TI_GIFFilter::WriteToFile FilterType = %d\n"),s_FilterType);
TRACEUSER( "Neville", _T("TI_GIFFilter::WriteToFile Interlace = %d\n"),Interlace);
TRACEUSER( "Neville", _T("TI_GIFFilter::WriteToFile Transparent = %d\n"),Transparent);

	if (MakeTransparent)
	{
		// We want to try and output the transparency if possible ...

		// Scan through the palette, and try and find an index with
		// the transparency flag set

		Transparent = -1;	// -1 = no transparency
		INT32 cols = Info->bmiHeader.biClrUsed;
		// If we have zero colours on a bitmap which is 8bpp or less then this is bad.
		// This should be translated as the maximum number of colours allowed
		if (Info->bmiHeader.biBitCount <= 8 && cols == 0)
			cols = 1 << Info->bmiHeader.biBitCount;

		for (INT32 i = 0; i < cols; i++)
		{
			if (Info->bmiColors[i].rgbReserved == 0xFF)
			{
			 	Transparent = i;
				TRACEUSER( "Neville", _T("GIF output with transp index of %d\n"),Transparent);
				break;
			}		
		}	

		// WEBSTER - markn 5/2/97
		// If we were unable to find the transparent colour, don't save out a transparent image
		if (Transparent == -1)
			MakeTransparent = FALSE;
	}

TRACEUSER( "Neville", _T("TI_GIFFilter::WriteToFile FilterType = %d\n"),s_FilterType);
TRACEUSER( "Neville", _T("TI_GIFFilter::WriteToFile Interlace = %d\n"),Interlace);
TRACEUSER( "Neville", _T("TI_GIFFilter::WriteToFile Transparent = %d\n"),Transparent);

	// Output the GIF data
	BOOL ok = TRUE;
	// WEBSTER - markn 5/2/97
	// Bug fix - passes the transparent colour index to OutputGifImageExtensionHeader() instead of TRUE
	// and also the 'if' statements use the 'MakeTransparent' flag
	if (!MakeTransparent)
		ok = DestGIF.OutputGifFileHeader(File, pInfoHeader, FALSE, -1, NULL, pPalette);
	else
	{
				ok = DestGIF.OutputGifFileHeader(File, pInfoHeader, TRUE, Transparent, NULL, pPalette);
		if (ok) ok = DestGIF.OutputGifImageExtensionHeader(File, Interlace, Transparent, 0, 0);
	}

//	if (Transparent == -1)
//		ok = DestGIF.OutputGifFileHeader(File, pInfoHeader, FALSE, -1, NULL, pPalette);
//	else
//		ok = DestGIF.OutputGifFileHeader(File, pInfoHeader, TRUE, Transparent, NULL, pPalette);
//	if (ok && Transparent)
//		ok = DestGIF.OutputGifImageExtensionHeader(File, Interlace, TRUE, 0, 0);

	if (ok)
		ok = DestGIF.OutputGifImageBits(File, Bits, Interlace, FALSE);
	
	DestGIF.TidyUp();
	
	// If started, then stop then progress bar
	if (ProgressString != NULL)
		EndSlowJob();

	return ok;
}


void TI_GIFFilter::AlterPaletteContents( LPLOGPALETTE pPalette )
{
	DestGIF.AlterExportPalette( pPalette );
}

/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WriteFileHeader(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific header data
********************************************************************************************/
BOOL TI_GIFFilter::WriteFileHeader(void)
{
	m_AnimationFrame = 0;
	return(TRUE);   
}


/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WritePreFrame(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info before the image
********************************************************************************************/
BOOL TI_GIFFilter::WritePreFrame(void)
{
	return DestGIF.ReStartFile(NULL);
}


/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WriteFrame(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the image itself
				This base class version actually calls the WriteToFile() function so that
				derived classes do not have to implement any of the multi-image stuff
********************************************************************************************/
BOOL TI_GIFFilter::WriteFrame(void)
{
	return MaskedFilter::WriteToFile ( TRUE );
}


/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WritePostFrame(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out any frame specific info after the image
********************************************************************************************/
BOOL TI_GIFFilter::WritePostFrame(void)
{
	BOOL ok = EndWriteToFile();

	if (m_DoingAnimation)
		m_AnimationFrame++;

	return ok;
}


/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WriteFileEnd(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	To write out the file specific data at the end of the file
				This base class version calls EndWriteToFile() so that derived classes
				do not have to implement the multi-image stuff
********************************************************************************************/
BOOL TI_GIFFilter::WriteFileEnd(void)
{
	/*BOOL ok =*/ DestGIF.OutputGifTerminator(OutputFile);
	return DestGIF.TidyUp();
}


/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WritePreSecondPass(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	Called to do any processing required after the first and before the second
				pass of a two pass export
********************************************************************************************/
BOOL TI_GIFFilter::WritePreSecondPass(void)
{
	return EndWriteToFile();
}



/********************************************************************************************
>	virtual BOOL TI_GIFFilter::WritePostOptimisedPalette(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		-
	Returns:	FALSE if failed else TRUE
	Purpose:	Called after an optimised palette has been generated
********************************************************************************************/
BOOL TI_GIFFilter::WritePostOptimisedPalette(void)
{
	if (m_AnimationFrame > 0)
		return DestGIF.ReStartFile(pOptimisedPalette);
	else
		return TRUE;
}



/********************************************************************************************
>	BOOL TI_GIFFilter::ShouldReuseExistingBitmaps()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Returns:	True if this filter should try and reuse existing bitmaps during import
	Purpose:	Determines whether or not to re-use existing bitmap.
				Animation Import will say no, so that frames that are same still appear
				more than once.
********************************************************************************************/
BOOL TI_GIFFilter::ShouldReuseExistingBitmaps()
{
	return (GetBitmapNumber() <= 1);
}

/********************************************************************************************

>	virtual OutputDIB* TI_GIFFilter::GetOutputDIB ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/00
	Returns		OutputDIB* - A pointer to DestGIF.
	Purpose:	Casts the current output DIB to be a generic OutputDIB class. This allows the
				same code to be re-used in the base class.

********************************************************************************************/

OutputDIB* TI_GIFFilter::GetOutputDIB ( void )
{
	// Perform an upcast to allow the pointer to be used in a generic manner.
	return static_cast<OutputDIB*> ( &DestGIF );
}

/********************************************************************************************

>	virtual BitmapExportOptions* TI_GIFFilter::CreateExportOptions() const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Returns:	A pointer to a new BitmapExportInfo class
	Purpose:	Allows derived classes to override this function to provide their own class
				derived from BitmapExportInfo containing filter specific information.

********************************************************************************************/
BitmapExportOptions* TI_GIFFilter::CreateExportOptions() const
{
	GIFExportOptions* pGIFOptions = new GIFExportOptions(TI_GIF, &FilterName);

	return (BitmapExportOptions*)pGIFOptions;
}


/********************************************************************************************

>	virtual UINT32 TI_GIFFilter::GetHintType(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/07/97
	Purpose:	Base class sets bad so no hint is set

********************************************************************************************/

UINT32 TI_GIFFilter::GetHintType(void)
{
	return(HINTTYPE_GIF);
}

/********************************************************************************************

  >	virtual BOOL TI_GIFFilter::IsDefaultDocRequired(const TCHAR* pcszPathName);

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/11/97
	Purpose:	Is called when a TIFF or GIF is opened, to let us say whether we
							need a default document.

							We need to open GIFs into the Animated template (in case they're
							animated). We do this as follows: we tell the Document system
							that the next template to use is the animation template. Then 
							we return TRUE so that this template is loaded.

							Not very elegant, but it works.
********************************************************************************************/

BOOL TI_GIFFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	//Tell the system to use the default animation template
	CCamDoc::SetNextTemplateToUse( CTemplateManager::GetDefaultAnimationTemplate() );

	return TRUE;
}




/*******************************************************************************************
/// AnimatedGIFImage class
********************************************************************************************/



/********************************************************************************************

>	AnimatedGIFImage::AnimatedGIFImage(const INT32 TransColour, const GIFDisposalMethod FrameRestore,
									   const CENTISECONDS FrameDelay,
									   const UINT32 LeftOffset, const UINT32 TopOffset,
									   const LPBITMAPINFO pInfo, const LPBYTE pBytes,
									   const LPBITMAPINFO pDiffInfo = NULL, const LPBYTE pDiffBytes = NULL)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/97
	Inputs:		TransColour		- the transparent colour for this frame
				FrameRestore	- The restore type for this frame
				FrameDelay		- the delay to apply to this frame
				LeftOffset, TopOffset	- offset for the bitmap
				pInfo, pBytes	- The bitmap to output for this frame
				pDiffInfo, pDiffBytes	- The difference bitmap for this frame
	Purpose:	To add a bitmap to the list of frames in the animation sequence.

********************************************************************************************/

AnimatedGIFImage::AnimatedGIFImage(const INT32 TransColour, const GIFDisposalMethod FrameRestore,
								   const CENTISECONDS FrameDelay,
								   const UINT32 LeftOffset, const UINT32 TopOffset,
								   const LPBITMAPINFO pInfo, const LPBYTE pBytes,
								   const LPBITMAPINFO pDiffInfo, const LPBYTE pDiffBytes)
{
	m_TransColour	= TransColour;
	m_FrameRestore	= FrameRestore;
	m_FrameDelay	= FrameDelay;

	m_LeftOffset	= LeftOffset;
	m_TopOffset		= TopOffset;

	m_pInfo			= pInfo;
	m_pBytes		= pBytes;
	m_pDiffInfo		= pDiffInfo;
	m_pDiffBytes	= pDiffBytes;
}

/********************************************************************************************

>	AnimatedGIFImage::AnimatedGIFImage()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/97
	Purpose:	Default contructor.

********************************************************************************************/

AnimatedGIFImage::AnimatedGIFImage()
{
	ERROR3("AnimatedGIFImage::AnimatedGIFImage - call the other constructor");
	m_TransColour	= -1;
	m_FrameRestore	= GDM_LEAVE;
	m_FrameDelay	= 0;

	m_LeftOffset	= 0;
	m_TopOffset		= 0;

	m_pInfo			= NULL;
	m_pBytes		= NULL;
	m_pDiffInfo		= NULL;
	m_pDiffBytes	= NULL;
}

/********************************************************************************************

>	AnimatedGIFImage::~AnimatedGIFImage()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/97
	Purpose:	Default destructor

********************************************************************************************/

AnimatedGIFImage::~AnimatedGIFImage()
{
	// Free up the difference bitmap that may have allocated
	if (m_pDiffInfo && m_pDiffBytes)
	{
		FreeDIB(m_pDiffInfo, m_pDiffBytes);
		m_pDiffInfo = NULL;
		m_pDiffBytes = NULL;
	}
}

/*******************************************************************************************
/// end of AnimatedGIFImage class
********************************************************************************************/
