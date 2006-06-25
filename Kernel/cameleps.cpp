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

// Camelot Native EPS format filter

/*
*/

#include "camtypes.h"

#include "cameleps.h"

#include "nodepath.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccdc.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bitmpinf.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"
#include "fracfill.h"
//#include "resource.h"
#include "nodeblnd.h"
#include "nodebldr.h"
#include "nodershp.h"
#include "nodemold.h"
#include "ndmldgrp.h"
#include "nodemldr.h"
#include "ndmldpth.h"
#include "nodeshap.h"
//#include "txtattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grndbmp.h"
#include "camview.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opgrad.h"		// for AreLinesPerpendicular()
#include "oilfiles.h"
#include "ndoptmz.h"
#include "ndtxtpth.h"
#include "maskedrr.h"
#include "oilbitmap.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "native.h"			// The new designed native filter, used for v2
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "psdc.h"
#include "osrndrgn.h"
#include "prdlgctl.h"
#include "progress.h"
#include "textfuns.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "will.h"		// for _R(IDE_UNKOWN_EPSOBJECT)
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ben.h"
#include "fontman.h"
#include "guides.h"
#include "snap.h"
//#include "jason.h"
#include "layer.h"
#include "colormgr.h"
#include "attrmap.h"
#include "colplate.h"

//#include "will2.h"

#include "xsepsops.h"		// export eps options
#include "ctrlhelp.h"		// ControlHelper class

#include "unicdman.h"		// For MBCS support

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW


CC_IMPLEMENT_DYNAMIC(CamelotEPSFilter, ArtWorksEPSFilter)



typedef enum
{
	FILLEFFECT_FADE,
	FILLEFFECT_RAINBOW,
	FILLEFFECT_ALTRAINBOW
} TranspFillEffectType;



// This is the array of Camelot EPS command/keyword names.
CommandMap CamelotEPSFilter::CamelotCommands[] =
{
	// Line/Fill colours
	{ EPSC_cx,		_T("cx") },
	{ EPSC_cX,		_T("cX") },
	{ EPSC_ck,		_T("ck") },
	{ EPSC_cK,		_T("cK") },

	// Stroke transparency
	{ EPSC_cst,		_T("cst") },
		  			  
	// Arrow heads
	{ EPSC_csah,	_T("csah") },
	{ EPSC_ceah,	_T("ceah") },

	// Dash Patterns
	{ EPSC_cdp,		_T("cdp") },

	// Bitmap objects/bitmap fills
	{ EPSC_cbm,		_T("cbm") },
	{ EPSC_csbm,	_T("csbm") },
	{ EPSC_cebm,	_T("cebm") },

	// Chromatic fill geometries
	{ EPSC_caz,		_T("caz") },
	{ EPSC_cax,		_T("cax") },
	
	// Transparent fill geometries
	{ EPSC_cxt,		_T("cxt") },

	// Chromatic fill effects
	{ EPSC_cxe,		_T("cxe") },

	// Chromatic fill mappings
	{ EPSC_cxm,		_T("cxm") },

	// Transparent fill mappings
	{ EPSC_cxmt,	_T("cxmt") },

	// Blends
	{ EPSC_csbd,	_T("csbd") },
	{ EPSC_cebd,	_T("cebd") },
	{ EPSC_csbr,	_T("csbr") },
	{ EPSC_cebr,	_T("cebr") },

	// Regular shapes
	{ EPSC_csrs,	_T("csrs") },
	{ EPSC_crsp,	_T("crsp") },
	{ EPSC_crstm,	_T("crstm") },
	{ EPSC_crsp1,	_T("crsp1") },
	{ EPSC_crsp2,	_T("crsp2") },
	{ EPSC_cers,	_T("cers") },

	// Mould commands
	{ EPSC_csev,	_T("csev") },
	{ EPSC_ceev,	_T("ceev") },
	{ EPSC_cspr,	_T("cspr") },
	{ EPSC_cepr,	_T("cepr") },
	{ EPSC_csmp,	_T("csmp") },
	{ EPSC_cemp,	_T("cemp") },
	{ EPSC_csso,	_T("csso") },
	{ EPSC_ceso,	_T("ceso") },
	{ EPSC_csdo,	_T("csdo") },
	{ EPSC_cedo,	_T("cedo") },

	// Text commands
 	{ EPSC_ctf,		_T("ctf") },
 	{ EPSC_ctb,		_T("ctb") },
	{ EPSC_cti,		_T("cti") },
	{ EPSC_cts,		_T("cts") },
	{ EPSC_ctp,		_T("ctp") },
	{ EPSC_ctls,	_T("ctls") },

	{ EPSC_cso,		_T("cso") },
	{ EPSC_ceo,		_T("ceo") },
	{ EPSC_cfft,	_T("cfft") },
	{ EPSC_cftf, 	_T("cftf") },
	{ EPSC_cbot,	_T("cbot") },

	{ EPSC_cpal,	_T("cpal") },

	// Guide layer & guideline
	{ EPSC_glyr,	_T("glyr") },
	{ EPSC_glne,	_T("glne") },

	{ EPSC_cmth,	_T("cmth") },

	{ EPSC_cag,		_T("cag") },

	{ EPSC_cbti,	_T("cbti") },

	// Sentinel
	{ EPSC_Invalid,	_T("Invalid") }
};

/********************************************************************************************

>	CamelotEPSFilter::CamelotEPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an CamelotEPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

CamelotEPSFilter::CamelotEPSFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDT_CAMEPS_FILTERNAME);
	FilterInfoID = _R(IDT_CAMEPS_FILTERINFO);
	ImportMsgID  = _R(IDT_IMPORTMSG_CAMELOT);
	ExportMsgID  = _R(IDT_EXPORTMSG_CAMEPS);

	FilterID = FILTERID_CAMELOT_EPS;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	//WEBSTER-Martin-27/01/97
#ifdef WEBSTER
	Flags.CanExport = FALSE;
#else
	Flags.CanExport = TRUE;
#endif //WEBSTER
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

	// We don't want to re-position Camelot files because they should be ok and
	// Charles gets upset if his drawings move.
	AdjustOrigin = FALSE;

}

/********************************************************************************************

>	BOOL CamelotEPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an CamelotEPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL CamelotEPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new CamelotEPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CamelotEPSFilter::CanIncludePreviewBmp()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE
	Purpose:	Admits that Camelot EPS is able to have a Preview Bitmap attached to the
				document. To actually put a bitmap into the file, call the
				IncludePreviewBmp(TRUE).
	SeeAlso:	Filters::CanIncludePreviewBmp

********************************************************************************************/

BOOL CamelotEPSFilter::CanIncludePreviewBmp()
{
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CamelotEPSFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
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

BOOL CamelotEPSFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// Return false here. All derived classes should return what they need
	return FALSE;
}	


/********************************************************************************************

>	INT32 CamelotEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an Camelot
				generated EPS file, as required.

********************************************************************************************/

INT32 CamelotEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// THIS ROUTINE IS NOT UNICODE
	// Check the first line in EPS file
	if (strncmp((char *) pFileHeader, "%!PS-Adobe-2.0 EPSF-1.2", 23) != 0)
	{
		// Incorrect version of EPS header line - we don't want this
		return 0;
	}

	// !PS-Adobe line is ok - check creator line...
	istringstream HeaderFile( (char*)pFileHeader );
	char Buffer[200];

	UINT32 Lines = 0;
	while ((Lines < 20) && !HeaderFile.eof())
	{
		HeaderFile.getline(Buffer, 200);
		Lines++;

		// Return TRUE if this file was created by Camelot
		if (strncmp(Buffer, "%%Creator: Camelot", 18) == 0)
		{
			// Camelot is the creator.
			return 10;
		}
		// (ChrisG 8/11/00) Changed creator type to "Xara X")
		else if (strncmp (Buffer, "%%Creator: Xara X", 17) == 0)
		{
			// New Xara X string was set as the creator.
			return 10;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (strncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Didn't find a suitable Creator line, but we did find an EPS line, so indicate
	// that we're interested, but not sure.
	return 5;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::PrepareToImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/94
	Returns:	TRUE if filter initialised ok;
				FALSE if not.
	Purpose:	See base class.  This just sets up the bitmap fill flag for Camelot EPS.
	Errors:		See base class.
	SeeAlso:	EPSFilter::PrepareToImport;

********************************************************************************************/

BOOL CamelotEPSFilter::PrepareToImport()
{
	if(IS_A(this, CamelotEPSFilter))
	{
		// tell the user that we can't do it
		PathName Path;
		String_256 FileName;
		BOOL HaveName = FALSE;
		if(EPSFile->IsKindOf(CC_RUNTIME_CLASS(CCDiskFile)))
		{
			CCDiskFile *DF = (CCDiskFile *)EPSFile;

			Path = DF->GetPathName();
			FileName = Path.GetFileName();
			HaveName = TRUE;
		}
		String_256 ErrMsg;
		if(HaveName)
		{
			String_256 ErrMsg;
			ErrMsg.MakeMsg(_R(IDE_LOADCAMELOTEPSITHINKNOTNAMED), (TCHAR *)FileName);
			Error::SetError(0, ErrMsg, 0);
		}
		else
		{
			Error::SetError(_R(IDE_LOADCAMELOTEPSITHINKNOT));
		}
		return FALSE;
	}
	else
	{
		// Initialise base class first.
		if (!ArtWorksEPSFilter::PrepareToImport())
			return FALSE;

		// Not expecting a bitmap fill initially
		m_PendingBitmap = PENDING_BITMAP_NONE;
		BitmapTransparencyIndex = -1;
		pBitmap = FALSE;
		pRegularShape = NULL;
	}

	return TRUE;
}

/********************************************************************************************

>	void CamelotEPSFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/07/95
	Inputs:		
	Returns:	-
	Purpose:	stops stuff happening after clearing up for a Camelot EPS import
	Errors:		-

********************************************************************************************/

void CamelotEPSFilter::CleanUpAfterImport(BOOL Successful)
{
	if(IS_A(this, CamelotEPSFilter))
		return;

	ArtWorksEPSFilter::CleanUpAfterImport(Successful);
}

/********************************************************************************************

>	virtual BOOL CamelotEPSFilter::GetExportOptions( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> HUmphrys
	Created:	22/12/95
	Inputs:		-
	Returns:	TRUE if OK, FALSE if user pressed Cancel.
	Purpose:	Allows the user to be prompted to get information for export.
				In this Camelot EPS case, we are affectively exporting a printable EPS
				and so require the user to set a few options.
				Moves the code from OilFilters to a proper function.
	Scope: 		Protected.

********************************************************************************************/

BOOL CamelotEPSFilter::GetExportOptions( )
{
#if !defined(EXCLUDE_FROM_RALPH)
	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(XSEPSExportOptions)); 
	if ( pOpDesc )
		pOpDesc->Invoke();
	
	return !XSEPSExportOptions::Aborted;
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Inputs:		pFile - The file to export to
				pSpread - the spread to export
	Returns:	TRUE if succeeded, FALSE if not (e.g. no memory for EPS stack)
	Purpose:	Prepare to import EPS data using this filter.  This sets up the filter
				to a sensible state for reading.
	Errors:		Out of memory.
	SeeAlso:	EPSFilter::DoImport; EPSFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

BOOL CamelotEPSFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Use base class to do most of it
	if (!EPSFilter::PrepareToExport(pFile, pSpread))
		return FALSE;

	// Set export device to use full Camelot accuracy for user space values.
	ExportDCPtr->SetFullAccuracy(TRUE);

	// Create the region if it is this class
	if (IS_A(this, CamelotEPSFilter))
	{
		// Don't care about clip regions when exporting - create a null region.
		DocRect NullClipRect;
		NullClipRect.MakeEmpty();

		// Don't use rendering matrix when exporting EPS as it uses fractional coordinates.
		Matrix Identity;

		// Don't use view scale; set to 1
		FIXED16 Scale(1);

		ExportRegion = new CamelotEPSRenderRegion(NullClipRect, Identity, Scale);
		if (ExportRegion == NULL)
			return FALSE;

		// Attach to the right device.
		ExportRegion->AttachDevice(DocView::GetSelected(), ExportDCPtr->GetDC(), pSpread);
	}
#endif
	// All ok
	return TRUE;
};

/********************************************************************************************

>	void CamelotEPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if the token is an Camelot EPS token; FALSE if not.
	Purpose:	Compare the current token against the Camelot keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::LookUpToken; EPSFilter::DecodeToken

********************************************************************************************/

void CamelotEPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (CamelotCommands[i].Cmd != EPSC_Invalid)
	{
		if (camStrcmp(TokenBuf, CamelotCommands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = CamelotCommands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	ArtWorksEPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set, or
				which need to be handled differently to the standard Illustrator meanings.
				i.e. this is the function that handles all the Camelot EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	String_64	ColName;
	PColourCMYK	Col;
	TintType	Tint = TINT_NONE;
	FIXEDPOINT	TintVal;
	DocCoord	StartPoint, 
				EndPoint;
	PathFlags 	Flags;
	INT32		ObjectNesting = 0;

	DocCoord 	BmpCoords[4];
	INT32		 	i;

	// Used to keep track of bitmaps loading them in.
	static 		NodeBitmap *pBitmapObject = NULL;

	// Decode the command, and execute it...
	switch (Token)
	{
		case EPSC_cso:
			INT32 ObjectID;
			if (!Stack.Pop(&ObjectID))
				goto EPSError;	   // pop the ID

			switch (ObjectID)
			{
				case EOTAG_FONTFLAGS:
				{
					if (!ProcessFontFlags())
						goto EPSError;
				}
				break;

				case EOTAG_BITMAPFLAGS:
				{
					if (!ProcessBitmapFlags())
						goto EPSError;
				}
				break;

				case EOTAG_TEXTWRAPPED:
				{
					if (!FindEndOfTag())
						goto EPSError;
				}
				break;

				case EOTAG_FONTTYPE:
				{
					if (!ProcessFontType())
						goto EPSError;
				}
				break;

				case EOTAG_GUIDELAYER:
				case EOTAG_GUIDELINE:
					break;

				case EOTAG_MOULDTHRESHOLD:
				{
					if (!ProcessMouldThreshold())
						goto EPSError;
				}
				break;


				default:
				{
					// Report we're skipping an unknown object
					ERROR1RAW(_R(IDE_UNKNOWN_EPSOBJECT));
					ObjectNesting++;
					do
					{
					 	if (!GetToken())
							goto EPSError;

						LexTokenType Type = EPSFile->GetTokenType();
						if ((Type != TOKEN_EOL) && (Type != TOKEN_EOF))
						{
							if (camStrcmp(TokenBuf, _T("cso")) == 0)
							{
								ObjectNesting++;
							}
							else if (camStrcmp(TokenBuf, _T("ceo")) == 0)
							{
								ObjectNesting--;
							}							
						}
					}
					while (ObjectNesting > 0);
				}
				break;
			}

			break;

		case EPSC_ceo:
			break;
//			goto EPSError;

		// Line/Fill colours
		case EPSC_cx:
		case EPSC_cX:
		case EPSC_ck:
		case EPSC_cK:
			break;

		// Linear/radial fills
		case EPSC_cax:
			// Colours are described using a name and tint as well as CMYK.
			Tint = TINT_ILLUSTRATOR;
		case EPSC_caz:
		{
			DocColour StartColour, EndColour;
			DocCoord StartPoint, EndPoint, EndPoint2;
			CamelotEPSFillType FillType;

			// Used for fractal fills
			INT32 Seed;
			double Graininess, Gravity, Squash;
			UINT32 DPI;
			INT32 Tileable;

			// Get fill type
			if (!Stack.Pop((INT32*) &FillType))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing Fill, Type=%d\n"),FillType);

			if (FillType == CAMEPS_FILL_FRACTAL)
			{
				// If it's a fractal fill type, discard the 'sub-type' parameter (should 
				// always be zero currently!)
				INT32 SubType;

				if (!Stack.Pop(&SubType))
					goto EPSError;

				// Default to no tiling
				Tileable = FALSE;

				// Work out the sub-type: either it has no tileable flag, or it does,
				// or it's a type that is not supported.
				if (SubType != 0)
				{
					if (SubType == 1)
					{
						// Get the tileable flag
						if (!Stack.Pop(&Tileable))
							goto EPSError;
					}
					else
					{
						ERROR2RAW("Bad fractal fill sub-type");
						goto EPSError;
					}
				}

				// Now get the fractal parameters:
				if (!Stack.Pop(&DPI) ||
					!Stack.Pop(&Squash) ||
					!Stack.Pop(&Gravity) ||
					!Stack.Pop(&Graininess) ||
					!Stack.Pop(&Seed))
				{
					// Error in fractal parameteres
					goto EPSError;
				}
			}

			// For elliptical & bitmap fills, get the second end-point of the fill
			if ((FillType == CAMEPS_FILL_ELLIPTICAL) ||
				(FillType == CAMEPS_FILL_BITMAP) ||
				(FillType == CAMEPS_FILL_FRACTAL) ||
				(FillType == CAMEPS_FILL_NEWBITMAP) ||
				(FillType == CAMEPS_FILL_NEWLINEAR))
			{
				if (!Stack.PopCoordPair(&EndPoint2))
					goto EPSError;
			}
			
			// Get start and end positions for grad-fills
			if (!Stack.PopCoordPair(&EndPoint) || !Stack.PopCoordPair(&StartPoint))
				goto EPSError;

			// Get start and end colours for grad-fills (but not bitmap fills)
			if (FillType != CAMEPS_FILL_BITMAP)
			{
				if (!Stack.PopColour(&Col, Tint, &TintVal, &ColName))
			    	// Invalid colour operands
					goto EPSError;

				GetEPSColour(&EndColour, &Col, Tint, TintVal, &ColName);

				if (!Stack.PopColour(&Col, Tint, &TintVal, &ColName))
			    	// Invalid colour operands
					goto EPSError;

				GetEPSColour(&StartColour, &Col, Tint, TintVal, &ColName);
			}
				
			switch (FillType)
			{
				// Decode Camelot EPS grad fill codes
				case CAMEPS_FILL_LINEAR:
					if (!SetLinearFill(StartColour, EndColour, StartPoint, EndPoint))
						goto NoMemory;
					break;

				case CAMEPS_FILL_NEWLINEAR:
					if (!SetLinearFill(StartColour, EndColour, StartPoint, EndPoint, &EndPoint2))
						goto NoMemory;
					break;

				case CAMEPS_FILL_ELLIPTICAL:
					if (!SetRadialFill(StartColour, EndColour, StartPoint, EndPoint, EndPoint2))
						goto NoMemory;
					break;

				case CAMEPS_FILL_CIRCULAR:
					if (!SetRadialFill(StartColour, EndColour, StartPoint, EndPoint))
						goto NoMemory;
					break;

				case CAMEPS_FILL_CONICAL:
					if (!SetConicalFill(StartColour, EndColour, StartPoint, EndPoint))
						goto NoMemory;
					break;

				case CAMEPS_FILL_BITMAP:
					
					// Is it really a bitmap fill ?
					// It may be a new type bitmap object
					if (m_PendingBitmap != PENDING_BITMAP_OBJECT_FILL)
						m_PendingBitmap = PENDING_BITMAP_FILL;

					BitmapAttrs.Coords[0] = StartPoint;
					BitmapAttrs.Coords[1] = EndPoint;
					BitmapAttrs.Coords[2] = EndPoint2;

					BitmapAttrs.StartCol 	= COLOUR_NONE;
					BitmapAttrs.EndCol 		= COLOUR_NONE;
					break;

				case CAMEPS_FILL_NEWBITMAP:
					m_PendingBitmap = PENDING_BITMAP_FILL;
					BitmapAttrs.Coords[0] = StartPoint;
					BitmapAttrs.Coords[1] = EndPoint;
					BitmapAttrs.Coords[2] = EndPoint2;

					BitmapAttrs.StartCol 	= StartColour;
					BitmapAttrs.EndCol 		= EndColour;
					break;

				case CAMEPS_FILL_FRACTAL:
					if (!SetFractalFill(StartColour, EndColour, 
										StartPoint, EndPoint, EndPoint2,
										Seed, Graininess, Gravity, Squash, DPI, Tileable))
						goto NoMemory;
					break;

				default:
					ENSURE(FALSE, "Unknown fill type found!");
					break;	// Don't know this fill type
			}

			break;
		}


		case EPSC_cxe:
		{
			INT32 EffectType;

			// Get fill effect type
			if (!Stack.Pop(&EffectType))
				goto EPSError;

			// Call base class to use it
			switch (EffectType)
			{
				case FILLEFFECT_FADE:
					if (!SetFadeFillEffect())
						goto NoMemory;
					break;

				case FILLEFFECT_RAINBOW:
					if (!SetRainbowFillEffect())
						goto NoMemory;
					break;

				case FILLEFFECT_ALTRAINBOW:
					if (!SetAltRainbowFillEffect())
						goto NoMemory;
					break;

				default:
					ERROR3_PF(("Unknown fill effect type (%d) in EPS", EffectType));
			}
			break;
		}

		case EPSC_cxm:
		{
			INT32 MappingType;
			INT32 Repeat;

			// Get fill mapping type (should always be 0)
			if (!Stack.Pop(&MappingType))
				goto EPSError;

			if (MappingType != 0)
			{
				ERROR2RAW("Bad mapping type in EPS");
				goto EPSError;
			}

			// Get proper fill mapping type (should be 1, 2 or 3)
			if (!Stack.Pop(&Repeat))
				goto EPSError;

			if ((Repeat < 1) || (Repeat > 3))
			{
				ERROR2RAW("Bad mapping type in EPS");
				goto EPSError;
			}

			// Call base class to use it
			if (Token == EPSC_cxm)
			{
				if (!SetLinearFillMapping(Repeat))
					goto NoMemory;
			}
			else
			{
				if (!SetLinearTranspFillMapping(Repeat))
					goto NoMemory;
			}

			break;
		}

		case EPSC_csbm:
			// Try to read in a bitmap
			if (!ReadBitmap())
				return FALSE;
			break;

		case EPSC_cebm:
			// Should have just finished reading a bitmap...what should we use for?
			switch (m_PendingBitmap)
			{
				case PENDING_BITMAP_FILL:
					// Use the bitmap to do a bitmap fill

					if (!SetBitmapFill(pBitmap, 
									   BitmapAttrs.Coords[0], 
									   BitmapAttrs.Coords[1], 
									   BitmapAttrs.Coords[2],
									   &BitmapAttrs.StartCol,
									   &BitmapAttrs.EndCol))
						return FALSE;

					// NULL the colours, so we don't get 'Index Colour in Use'
					BitmapAttrs.StartCol 	= COLOUR_NONE;
					BitmapAttrs.EndCol 		= COLOUR_NONE;

					// Get rid of our KernelBitmap object (the bitmap itself is now
					// attached to the current fill geometry attribute.
					pBitmap = NULL;
					break;

				case PENDING_BITMAP_TRANSPFILL:
					// Use the bitmap to do a transparent bitmap fill (texture)

					if (!SetBitmapTranspFill(BitmapAttrs.TranspType, pBitmap, 
										   	 BitmapAttrs.Coords[0], 
											 BitmapAttrs.Coords[1], 
											 BitmapAttrs.Coords[2],
											 BitmapAttrs.Transp,
											 BitmapAttrs.EndTransp))
						return FALSE;

					// Get rid of our KernelBitmap object (the bitmap itself is now
					// attached to the current transparent fill geometry attribute.
					pBitmap = NULL;
					break;

				case PENDING_BITMAP_OBJECT_FILL:

					// This is the new format used to save 'Non-contoned' bitmaps,
					// where it is saved as a bitmap filled path, so the Viewer
					// can render it, and an extra token that tells this code that
					// it's really a bitmap object.

					// Bitmap object - make one ready to put into the tree.
					pBitmapObject = new NodeBitmap;
					if ((pBitmapObject == NULL) || (!pBitmapObject->SetUpPath(12,12)))
						goto EPSError;

					// Now we need to Skip over the path that is output for the
					// Viewer to render ...
					while (!EPSFile->eof())
					{
						GetToken();

						// We'll remember the coords and use them to build the bitmap
						switch (Token)
						{
							case EPSC_Integer:
								Stack.Push(TokenData.Long);
								break;

							case EPSC_Double:
								Stack.Push(TokenData.Double);
								break;

							case EPSC_FixedPoint:
								Stack.Push(TokenData.FixedPoint);
								break;
						}	

						// Look for the 'Filled Path' token...
						if (Token == EPSC_F)
							break;
					}

					Stack.Discard(2);	// Throw away the first coord pair

					// Now read in reverse order because they're stacked
					for (i = 3; i >= 0; i--)
					{
						if (!Stack.PopCoordPair(&BmpCoords[i]))
							goto EPSError;
						pBitmapObject->Parallel[i] = BmpCoords[i];
					}

					// Put them into the path.
					pBitmapObject->InkPath.InsertMoveTo(BmpCoords[0]);

					for (i = 1; i <= 3; i++)
						pBitmapObject->InkPath.InsertLineTo(BmpCoords[i]);

					pBitmapObject->InkPath.InsertLineTo(BmpCoords[0]);
					pBitmapObject->InkPath.CloseSubPath();

					// Make sure the bitmap has no fill colour applied
					SetPathFilled(FALSE);
					
				  	// Now fall though to the Bitmap Object code .....

				case PENDING_BITMAP_OBJECT:
				{
					// Use the bitmap to do a bitmap object

					// Copy the bitmap into this node and delete the one we've just imported.
					if (pBitmapObject == NULL)
						// No bitmap node to load!
						goto EPSError;

					pBitmapObject->GetBitmapRef()->Attach(pBitmap, GetDocument());
					if (pBitmapObject->GetBitmap() != pBitmap)
					{
						// It didn't use the bitmap we gave it, so we can delete it
						delete pBitmap;
					}

					pBitmap = NULL;

					AttributeValue* pOldLineColour = NULL;
					AttributeValue* pOldLineWidth = NULL;

					if (m_PendingBitmap == PENDING_BITMAP_OBJECT_FILL)
					{
						// This must be a non-contone bitmap, so make sure we
						// don't apply any line colour.

						// (The fill colour will already have been turned off
						//  with a 'SetPathFilled(FALSE)' above)
					
						pOldLineColour = CurrentAttrs[ATTR_STROKECOLOUR].pAttr;
						
						// Force Line Colour be NONE.
						StrokeColourAttribute* pLineCol = new StrokeColourAttribute;
						if (pLineCol == NULL)
							goto NoMemory;

						DocColour	colorNone( COLOUR_NONE );
						pLineCol->SetStartColour( &colorNone );
							
						CurrentAttrs[ATTR_STROKECOLOUR].pAttr = pLineCol;
					}

					pOldLineWidth = CurrentAttrs[ATTR_LINEWIDTH].pAttr;
					
					// Force the line width to be zero for Bitmap objects
					LineWidthAttribute* pLineWidth = new LineWidthAttribute;
					if (pLineWidth == NULL)
						goto NoMemory;

					pLineWidth->LineWidth = 0;
						
					CurrentAttrs[ATTR_LINEWIDTH].pAttr = pLineWidth;

					// Add attributes to the path, if they are different from the default...
					BOOL Success = AttributeManager::ApplyBasedOnDefaults(pBitmapObject, 
																		  CurrentAttrs);

					// Did it work?
					if (!Success)
					{
						// No clean up and report error
						delete pBitmapObject;
						pBitmapObject = NULL;
						goto NoMemory;
					}

					// Finally, add it into the tree
					pBitmapObject->InvalidateBoundingRect();
					AddNewNode(pBitmapObject);

					// Switch path filling back on again
					SetPathFilled(TRUE);

					if (pOldLineColour != NULL)
					{
						delete CurrentAttrs[ATTR_STROKECOLOUR].pAttr;

						// Restore the old line colour attribute
						CurrentAttrs[ATTR_STROKECOLOUR].pAttr = pOldLineColour;
					}

					if (pOldLineWidth != NULL)
					{
						delete CurrentAttrs[ATTR_LINEWIDTH].pAttr;

						// Restore the old line width attribute
						CurrentAttrs[ATTR_LINEWIDTH].pAttr = pOldLineWidth;
					}

					// Get rid of our KernelBitmap object (the bitmap itself is now
					// attached to the current fill geometry attribute.
					pBitmap = NULL;
					pBitmapObject = NULL;
					break;
				}

				case PENDING_BITMAP_POOLITEM:
				{
					// Attach the Bitmap to a reference so that it is kept around for things to use
					TRACEUSER( "Rik", _T("What do you know, its a Pool Item\n"));
					BitmapPoolAttach(pBitmap);
					pBitmap = NULL;
					break;
				}

				default:
					ENSURE(FALSE, "Found a bitmap but don't know what to do with it!");
					delete pBitmap;
					pBitmap = NULL;
					goto EPSError;
					break;
			}

			m_PendingBitmap = PENDING_BITMAP_NONE;
			BitmapTransparencyIndex = -1;
			break;

		case EPSC_cbm:
		{
			// Bitmap object - make one ready to put into the tree.
			pBitmapObject = new NodeBitmap;
			if ((pBitmapObject == NULL) || (!pBitmapObject->SetUpPath(12,12)))
				return FALSE;

			// First, try to read in the coordinates of the bitmap object
			DocCoord Coords[4];

			// NB. reverse order because they're stacked, remember!
			INT32 i;
			for ( i = 3; i >= 0; i--)
			{
				if (!Stack.PopCoordPair(&Coords[i]))
					goto EPSError;
				pBitmapObject->Parallel[i] = Coords[i];
			}

			// Put them into the path.
			pBitmapObject->InkPath.InsertMoveTo(Coords[0]);

			for (i = 1; i <= 3; i++)
				pBitmapObject->InkPath.InsertLineTo(Coords[i]);

			pBitmapObject->InkPath.InsertLineTo(Coords[0]);
			pBitmapObject->InkPath.CloseSubPath();

//			pBitmapObject->InkPath.UpdateBoundingRect();

			// Flag to the code above that the next bitmap should be added to this object.
			m_PendingBitmap = PENDING_BITMAP_OBJECT;

			break;
		}

		case EPSC_cbmp:
			// A bitmap palette - should not ever get this in ART files.
			goto EPSError;

		// Blend related procedures
		case EPSC_csbd:
			return ProcessBlend();
			
		case EPSC_cebd:
			goto EPSError;
			
		case EPSC_csbr:
			return ProcessBlender();
			
		case EPSC_cebr:
			goto EPSError;

		// Regular shape related procedures			
		case EPSC_csrs:
			return ProcessRegularShape();

		case EPSC_cers:
			goto EPSError;

		case EPSC_crsp:
			return ProcessRegularShapeParams();

		case EPSC_crstm:
			{
				if (pRegularShape == NULL)
					goto EPSError;

				// Get the shape's matrix from the stack and put it into the shape.
				Matrix TheMatrix;
				if (!Stack.Pop(&TheMatrix, FALSE))
					goto EPSError;

				pRegularShape->SetTransformMatrix(&TheMatrix);
				break;
			}

		case EPSC_crsp1:
			{
				if ((pRegularShape == NULL) || (pInkPath == NULL))
					goto EPSError;

				// Path is held in pInkPath - copy into shape and clear the pInkPath ready for
				// the second edge's path.
				pRegularShape->EdgePath1.ClearPath();
				if (!pRegularShape->EdgePath1.CopyPathDataFrom(pInkPath))
					goto NoMemory;

				pInkPath->ClearPath();
				break;
			}

		case EPSC_crsp2:
			{
				if ((pRegularShape == NULL) || (pInkPath == NULL))
					goto EPSError;

				// Path is held in pInkPath - copy into shape's second edge path and delete the
				// temporary path.
				pRegularShape->EdgePath2.ClearPath();
				if (!pRegularShape->EdgePath2.CopyPathDataFrom(pInkPath))
					goto NoMemory;

				// No more paths to read in.
				delete pInkPath;
				pInkPath = NULL;
				break;
			}

		// Mould commands
		case EPSC_csev:
			return ProcessEnvelope();

		case EPSC_cspr:
			return ProcessPerspective();

		// Guide layers & guidelines
		case EPSC_glyr:
			return ProcessGuideLayer();

		case EPSC_glne:
			return ProcessGuideline();

		case EPSC_cag:
			// A separated grey fill - should not ever get this in ART files.
			goto EPSError;
		
		default:
			// Token not understood - pass on to base class
			return ArtWorksEPSFilter::ProcessToken();
	}


	// No errors encountered while parsing this token and its operands.
	return TRUE;
	
	
	// Error handlers:
	
EPSError:
	HandleEPSError();
	return FALSE;

NoMemory:
	HandleNoMemory();
	return FALSE;
}




/********************************************************************************************

>	TCHAR *CamelotEPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_aoa
	Returns:	Pointer to the string representation of the token, e.g. "aoa"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

TCHAR *CamelotEPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (CamelotCommands[i].Cmd != EPSC_Invalid)
	{
		if (CamelotCommands[i].Cmd == Cmd)
			return CamelotCommands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return ArtWorksEPSFilter::GetEPSCommand(Cmd);
}





/********************************************************************************************

>	BOOL CamelotEPSFilter::ReadBitmap()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>	(Will)
	Created:	25/1/95
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Reads the bitmap info from the eps stack and loads in the bitmap data
				It loads a type 0 bitmap
	SeeAlso:	CamelotNativeEPSFilter::ReadBitmap

********************************************************************************************/

BOOL CamelotEPSFilter::ReadBitmap()
{
	// Find out what kind of bitmap it is - always type 0 at present in CamelotEPS (CamelotNative has a type 1 as well).
	INT32 BitmapType;
	if (!Stack.Pop(&BitmapType) || (BitmapType != 0))
	{
		// Error - not enough operands, or bitmap is wrong type
		ENSURE(BitmapType == 0, "Unknown bitmap type in EPS!");
		HandleEPSError();
		return FALSE;
	}
	
	// Read in info on the bitmap
	// Recommended size is stored as a user space value, i.e. in points, so we
	// use PopCoord() to get the automatic scaling to millipoints.
	BitmapInfo Info;
	String_256 BitmapName;
	if (!Stack.PopCoord(&Info.RecommendedHeight) || !Stack.PopCoord(&Info.RecommendedWidth) ||
		!Stack.Pop(&Info.NumPaletteEntries) 	 || !Stack.Pop(&Info.PixelDepth) 			||
		!Stack.Pop(&Info.PixelHeight) 			 || !Stack.Pop(&Info.PixelWidth)			||
		!Stack.Pop(&BitmapName))
	{
		// Error - not enough operands
		HandleEPSError();
		return FALSE;
	}

	BOOL IsNew;
	BOOL ImportOk = KernelBitmap::ImportBitmap(this, &Info, BitmapType,
					  							&pBitmap, &IsNew);
	if (ImportOk && IsNew)
	{
		// Set the name in it
		pBitmap->ActualBitmap->SetName(BitmapName);

		// Set the TransparencyIndex
		if (BitmapTransparencyIndex >= 0)
		{
			TRACEUSER( "Will", _T("Imported bitmap with Transparent Index %d\n"), BitmapTransparencyIndex);
		}

		pBitmap->SetTransparencyIndex(BitmapTransparencyIndex);
	}

	return ImportOk;
}



/********************************************************************************************

>	virtual void CamelotEPSFilter::BitmapPoolAttach(KernelBitmap* pBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		pBitmap - The bitmap to attach
	Purpose:	Makes a reference to the bitmap so that it can be used throughout the eps
				file with repeating the data. This base class version actually does nothing
				but delete the bitmap. CamelotNativeEPS is where the real processing is done.

********************************************************************************************/

void CamelotEPSFilter::BitmapPoolAttach(KernelBitmap* pBitmap)
{
	// Base class version of the function just deletes the bitmap as it does not really know
	// what it needs to do with it
	if (pBitmap!=NULL)
		delete pBitmap;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ExportBitmap(KernelBitmap& TheBitmap)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/09/94
	Inputs:		TheBitmap - the bitmap to export
	Returns:	TRUE if the bitmap was exported ok (or if it wasn't exported because the
					 file format does not support it;
				FALSE if an error occured.
	Purpose:	Export a bitmap to a Camelot EPS file.  This outputs the standard tokens
				and data required to embed a bitmap in a Camelot EPS file.
	Errors:		Usual disk/file errors.
	SeeAlso:	Filter::ExportBitmap

********************************************************************************************/

BOOL CamelotEPSFilter::ExportBitmap(KernelBitmap& TheBitmap)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BitmapInfo Info;
	String_256 BitmapName;

	// Check to seed if the bitmap has a transparent Index, 
	// and output a special token if so
	if (TheBitmap.ActualBitmap->GetBPP() <= 8)
	{
		INT32 TransparencyIndex;

		if (TheBitmap.ActualBitmap->GetTransparencyIndex(&TransparencyIndex))
		{
			TRACEUSER( "Will", _T("Exporting bitmap with Transparent Index %d\n"), TransparencyIndex);

			ExportDCPtr->OutputValue((INT32) EOTAG_BITMAPFLAGS);
			ExportDCPtr->OutputToken(_T("cso"));	// Start of new object type

			ExportDCPtr->OutputValue((INT32) TransparencyIndex);
			ExportDCPtr->OutputToken(_T("cbti"));	// Index of the colour that is transparent

			ExportDCPtr->OutputToken(_T("ceo"));	// End of new object type
			ExportDCPtr->OutputNewLine();
		}
	}

	// Get details of the bitmap
	TheBitmap.ActualBitmap->GetInfo(&Info);
	BitmapName = TheBitmap.ActualBitmap->GetName();

	// Write them out to the EPS file.
	ExportDCPtr->OutputString((TCHAR*)BitmapName);
	ExportDCPtr->OutputValue((UINT32) Info.PixelWidth);
	ExportDCPtr->OutputValue((UINT32) Info.PixelHeight);
	ExportDCPtr->OutputValue((UINT32) Info.PixelDepth);
	ExportDCPtr->OutputValue((UINT32) Info.NumPaletteEntries);
	ExportDCPtr->OutputUserSpaceValue(Info.RecommendedWidth);
	ExportDCPtr->OutputUserSpaceValue(Info.RecommendedHeight);

	// Bitmap type is 0 as this is the type that exports all the bitmap data
	// Type 1 is used as a reference to a bitmap in the pool
	ExportDCPtr->OutputToken(_T("0"));

	// Write out the bitmap start token
	ExportDCPtr->OutputToken(_T("csbm"));
	ExportDCPtr->OutputNewLine();

	// Write out the bitmap data
	TheBitmap.ActualBitmap->ExportBitmap(ExportRegion);

	// Write out the bitmap end token
	ExportDCPtr->OutputToken(_T("cebm"));
	ExportDCPtr->OutputNewLine();
#endif
	// All ok
	return TRUE;
}



/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessFontFlags()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/95
	Returns:	TRUE if the font flags were processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a font flags structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessFontFlags()
{
	FontFlags.Bold = FALSE;
	FontFlags.Italic = FALSE;
	// Keep processing tokens until we find the end of the font flags tag object
	while (!EPSFile->eof())
	{
		GetToken();

		// Look for the only font tokens we're interrested in
		switch (Token)
		{
			case EPSC_ceo:
				// found the end of the font flags block so return done.
				return TRUE;
				break;

			case EPSC_cfft:
				// This flag indicates that the next bold/italic attributes are to
				// be applied to the current font style rather than as physical attributes
				FontFlags.Bold = TRUE;
				FontFlags.Italic = TRUE;
				break;

			default:
				// try to handle whatever token this is.
				if (!HandleToken())	
					return FALSE;
				break;
		}
	}

	HandleEPSError();
	return FALSE;
}



/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessFontType()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/95
	Returns:	TRUE if the font type was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a font type structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessFontType()
{
	EPSFilter::ClassOfFont = FC_TRUETYPE;
	// Keep processing tokens until we find the end of the font flags tag object
	while (!EPSFile->eof())
	{
		GetToken();

		// Look for the only font tokens we're interrested in
		switch (Token)
		{
			case EPSC_ceo:
				// found the end of the font flags block so return done.
				return TRUE;
				break;

			case EPSC_cftf:
			{
				// This type indicates that the next font name is of a particular font type
				INT32 Type;
				FontClass actclass;

				if (!Stack.Pop(&Type))
				{
					HandleEPSError();
					return FALSE;
				}

				if (FONTMANAGER->LegalFontClass(Type, actclass))
					EPSFilter::ClassOfFont = actclass;
				// if we don't know what the class is assume Truetype.
			}
			break;

			default:
				// try to handle whatever token this is.
				if (!HandleToken())	
					return FALSE;
				break;
		}
	}

	HandleEPSError();
	return FALSE;
}



/********************************************************************************************

>	BOOL CamelotEPSFilter::FindEndOfTag()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/95
	Returns:	TRUE if the end of tag token was found, FALSE if not.
	Purpose:	Continues reading tokens until the ceo token is found
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::FindEndOfTag()
{
	// Keep processing tokens until we find the 'end of tag' token
	while (!EPSFile->eof())
	{
		GetToken();
		switch (Token)
		{
			case EPSC_ceo:
				return TRUE;
				break;

			default:
				if (!HandleToken())
					return FALSE;
				break;
		}
	}
	HandleEPSError();
	return FALSE;
}









/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessBitmapFlags()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/06/95
	Returns:	TRUE if the bitmap flags were processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a bitmap flags structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessBitmapFlags()
{
	// Keep processing tokens until we find the end of the bitmap flags tag object
	while (!EPSFile->eof())
	{
		GetToken();

		// Look for the only bitmap tokens we're interested in
		switch (Token)
		{
			case EPSC_ceo:
				// found the end of the bitmap flags block so return done.
				return TRUE;
				break;

			case EPSC_cbot:
				// This token indicates that the next bitmap fill is really 
				// a bitmap object
				m_PendingBitmap = PENDING_BITMAP_OBJECT_FILL;
				break;

			case EPSC_cbti:
				// The next bitmap must have a transparency Index
				INT32 Index;

				if (!Stack.Pop(&Index))
				{
					HandleEPSError();
					return FALSE;
				}

				BitmapTransparencyIndex = Index;
				break;

			default:
				// try to handle whatever token this is.
				if (!HandleToken())	
					return FALSE;
				break;
		}
	}

	HandleEPSError();
	return FALSE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the blend was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a blend structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessBlend()
{
	if (!StartBlend())
		return FALSE;

	// Keep processing tokens until we find the end of the blend
	do
	{
		GetToken();

		// Look for the end of the blend token...
		if (Token == EPSC_cebd)
		{
			return EndBlend();
		}
	}
	// Otherwise keep going until an error or eof is encountered
	while (HandleToken() && (!EPSFile->eof()));

	if (EPSFile->eof())
	{
		// Didn't find end of blend - syntax error; deal with it
		HandleEPSError();
	}

	// If we're here, something went wrong
	return FALSE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::StartBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the new blend was created ok;
				FALSE if not.
	Purpose:	Used when a blend structure needs to be created - after this is called,
				all new nodes added with AddNewNode() will be added as children of this
				new blend, until EndBlend is called.
	Errors:		Out of memory
	SeeAlso:	CamelotEPSFilter::EndBlend

********************************************************************************************/

BOOL CamelotEPSFilter::StartBlend()
{
	INT32 NumBlendSteps,Version,OneToOne,NotAntialiased,ColBlendType;
	INT32 Reserved3,Reserved2,Reserved1;
	
	// The last param before the token is the version.
	if (!Stack.Pop(&Version))
		return FALSE;

	// Error if the version number is not correct
	ERROR1IF(Version != 1,FALSE,_R(IDT_EPS_BADSYNTAX));

	// Read in the blend's params
	if (!Stack.Pop(&NumBlendSteps) 	||
		!Stack.Pop(&NotAntialiased)	||
		!Stack.Pop(&OneToOne)		||
		!Stack.Pop(&ColBlendType)	||
		!Stack.Pop(&Reserved3)		||
		!Stack.Pop(&Reserved2)		||
		!Stack.Pop(&Reserved1))
	{
		return FALSE;
	}

	// Make a new blend node for this blend
	NodeBlend *pNodeBlend = new NodeBlend;
	ERROR1IF(pNodeBlend == NULL, FALSE, _R(IDT_EPS_NOMEMORY));

	// Add it into the tree
	if (!AddNewNode(pNodeBlend))
		return FALSE;

	// Set the blend's params
	pNodeBlend->SetOneToOne(OneToOne);
	pNodeBlend->SetNotAntialiased(NotAntialiased);
	pNodeBlend->SetNumBlendSteps(NumBlendSteps);
	pNodeBlend->SetColourBlendType((ColourBlendType)ColBlendType);

	// Make sure new objects are added as children of the node
	pNode = pNodeBlend;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::EndBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the blend was ended ok;
				FALSE if not.
	Purpose:	Used when a blend has finished being constructed and we want to return to
				normal node positioning.  The blend is added to the document when this
				function is called (although that depending on the current filter mode, 
				i.e. whether this is a new or existing layer, it may not be added
				directly to the documnent tree straight away - it may be deferred until
				the next layer is found, or the import has ended).
	Errors:		Out of memory
	SeeAlso:	CamelotEPSFilter::StartBlend

********************************************************************************************/

BOOL CamelotEPSFilter::EndBlend()
{
	// Sanity check
	ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBlend)), "No blend in CamelotEPSFilter::EndBlend");

	// Keep the blend ptr and find the first child of the node before we reset pNode.
	Node* pNodeBlend = pNode;
	Node* pChildNode = pNode->FindFirstChild();

	// Get the parent of the blend node, and use that to add new objects to
	pNode = pNodeBlend->FindParent();
	ENSURE(pNode != NULL, "Blend has no parent in ProcessBlend()");

	// Initialise all the child blender nodes
	UINT32 BlenderCount=0;
	while (pChildNode != NULL)
	{
		if (IS_A(pChildNode,NodeBlender))
		{
			NodeBlender* pNodeBlender = (NodeBlender*)pChildNode;

			// Get the imported blender params
			INT32 PathIndexStart = pNodeBlender->GetPathIndexStart();
			INT32 PathIndexEnd   = pNodeBlender->GetPathIndexEnd();
			INT32 ObjIndexStart 	= pNodeBlender->GetObjIndexStart();
			INT32 ObjIndexEnd   	= pNodeBlender->GetObjIndexEnd();

			// Find the nodes this blender is to blend together
			NodeRenderableInk* pNodeStart = pNodeBlender->FindObjIndexedNode(ObjIndexStart);
			NodeRenderableInk* pNodeEnd   = pNodeBlender->FindObjIndexedNode(ObjIndexEnd);

			// They should not be NULL
			if (pNodeStart == NULL || pNodeEnd == NULL)
				return FALSE;

			// Init the blender with the start and end nodes.
			if (!pNodeBlender->Initialise(pNodeStart,pNodeEnd,PathIndexStart,PathIndexEnd,ImportInfo.pOp,NULL,TRUE))
				return FALSE;

			BlenderCount++;
		}
		pChildNode = pChildNode->FindNext();
	}

	if (BlenderCount == 0)
	{
		ERROR3("Imported Blend node doesn't contain any blender nodes");
		if (!ImportInfo.pOp->DoHideNode(pNodeBlend, TRUE))
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessBlender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the blender was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a blender structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessBlender()
{
	if (!StartBlender())
		return FALSE;

	// Keep processing tokens until we find the end of the blender
	do
	{
		GetToken();

		// Look for the end of the blender token...
		if (Token == EPSC_cebr)
		{
			return EndBlender();
		}
	}
	// Otherwise keep going until an error or eof is encountered
	while (HandleToken() && (!EPSFile->eof()));

	if (EPSFile->eof())
	{
		// Didn't find end of blender - syntax error; deal with it
		HandleEPSError();
	}

	// If we're here, something went wrong
	return FALSE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::StartBlender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the new blender was created ok;
				FALSE if not.
	Purpose:	Used when a blender structure needs to be created - after this is called,
				all paths will be discarded, until EndBlender is called.
	Errors:		Out of memory
	SeeAlso:	CamelotEPSFilter::EndBlender

********************************************************************************************/

BOOL CamelotEPSFilter::StartBlender()
{
	INT32 ObjIndexStart,ObjIndexEnd,PathIndexStart,PathIndexEnd;
	INT32 Reserved1,Reserved2,Reserved3,Reserved4;
	
	// Read the blender's params
	if (!Stack.Pop(&PathIndexEnd)	||
		!Stack.Pop(&PathIndexStart)	||
		!Stack.Pop(&ObjIndexEnd)	||
		!Stack.Pop(&ObjIndexStart)	||
		!Stack.Pop(&Reserved4)		||
		!Stack.Pop(&Reserved3)		||
		!Stack.Pop(&Reserved2)		||
		!Stack.Pop(&Reserved1))
	{
		return FALSE;
	}

	// Make a new blender node for this blender
	NodeBlender *pNodeBlender = new NodeBlender;
	ERRORIF(pNodeBlender == NULL, _R(IDT_EPS_NOMEMORY), FALSE);

	// Add it into the tree
	if (!AddNewNode(pNodeBlender))
		return FALSE;

	// Set the blender params
	pNodeBlender->SetObjIndexStart(ObjIndexStart);
	pNodeBlender->SetObjIndexEnd  (ObjIndexEnd);
	pNodeBlender->SetPathIndexStart(PathIndexStart);
	pNodeBlender->SetPathIndexEnd  (PathIndexEnd);
	
	// Discard all subsequent paths - until we get an "end blender" token
	ThePathType = PATH_DISCARD_STICKY;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::EndBlender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the blender was ended ok;
				FALSE if not.
	Purpose:	Used when a blender has finished being constructed.
				Path importing is returned to normal (i.e. they're no longer discarded)
	Errors:		-
	SeeAlso:	CamelotEPSFilter::StartBlender

********************************************************************************************/

BOOL CamelotEPSFilter::EndBlender()
{
	ThePathType = PATH_NORMAL;
	return TRUE;
}



/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessRegularShape()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/94
	Returns:	TRUE if the blend was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a blend structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessRegularShape()
{
	// Make a regular shape node, and use this as a temporary context for storing path
	// data before passing it into the shape object.
	pRegularShape = new NodeRegularShape;
	if (pRegularShape == NULL)
		return FALSE;

	if (!pRegularShape->SetUpShape())
	{
		delete pRegularShape;			 
		pRegularShape = NULL;
		return FALSE;
	}

	// Add it into the tree
	if (!AddNewNode(pRegularShape))
	{
		pRegularShape = NULL;
		return FALSE;
	}
	
	// Get a new ink path to store the shape's edge paths in.
	ERROR3IF(pInkPath != NULL, "Already creating an InkPath object in regular shape");
	pInkPath = new Path;
	if (pInkPath == NULL)
		return FALSE;

	// Set up the shape's path - this is currently 4 elements long - and 
	// move to the start of the path
	if (!pInkPath->Initialise())
	{
		delete pInkPath;
		pInkPath = NULL;
		delete pRegularShape;
		pRegularShape = NULL;
		return FALSE;
	}	

	pInkPath->FindStartOfPath();

	// Keep processing tokens until we find the end of the regular shape
	do
	{
		GetToken();

		// Look for the end of the regular shape token...
		if (Token == EPSC_cers)
		{
			if (pInkPath != NULL)
			{
				// Shape has not been saved correctly if we haven't read in both paths by now.
				HandleEPSError();
				return FALSE;
			}

			// Apply current attributes to the shape
			if (!AddAttributes(pRegularShape, RegularShapeIsStroked, RegularShapeIsFilled))
			{
				HandleNoMemory();
				return FALSE;
			}

			// Force shape's path to be reconstructed
			pRegularShape->InvalidateCache();
			pRegularShape->InvalidateBoundingRect();

			pRegularShape = NULL;
			return TRUE;
		}
	}
	// Otherwise keep going until an error or eof is encountered
	while (HandleToken() && (!EPSFile->eof()));

	if (EPSFile->eof())
	{
		// Didn't find end of regular - syntax error; deal with it
		HandleEPSError();
	}

	// If we're here, something went wrong
	return FALSE;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessRegularShapeParams()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Returns:	TRUE if the regular shape was processed correctly, FALSE if not.
	Purpose:	Reads in all the parameters of a regular shape structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessRegularShapeParams()
{
	// Check we are actually making a regular shape at the moment.
	if (pRegularShape == NULL)
	{
		HandleEPSError();
		return FALSE;
	}

	UINT32 NumSides;
	INT32	Circular;
	INT32	Stellated;
	INT32	PrimaryCurvature;
	INT32	StellationCurvature;
	double	StellRadiusToPrimary;
	double	PrimaryCurveToPrimary;
	double	StellCurveToStell;
	double	StellOffsetRatio;
	DocCoord	UTCentrePoint;
	DocCoord	UTMajorAxes;
	DocCoord	UTMinorAxes;

//Peter was here
	INT32 StrokedFlag;
	INT32 FilledFlag;

	// Try to get all the parameters off the EPS stack.
	if (!Stack.Pop(&StrokedFlag) ||
		!Stack.Pop(&FilledFlag) ||
		!Stack.PopCoordPair(&UTMinorAxes) ||
		!Stack.PopCoordPair(&UTMajorAxes) ||
		!Stack.PopCoordPair(&UTCentrePoint) ||
		!Stack.Pop(&StellOffsetRatio) ||
		!Stack.Pop(&StellCurveToStell) ||
		!Stack.Pop(&PrimaryCurveToPrimary) ||
		!Stack.Pop(&StellRadiusToPrimary) ||
		!Stack.Pop(&StellationCurvature) ||
		!Stack.Pop(&PrimaryCurvature) ||
		!Stack.Pop(&Stellated) ||
		!Stack.Pop(&Circular) ||
		!Stack.Pop(&NumSides))
	{
		HandleEPSError();
		return FALSE;
	}
	else
	{
		RegularShapeIsStroked = StrokedFlag;
		RegularShapeIsFilled = FilledFlag;
	}

	// Now set the flag specifying whether the path is filled
	SetPathFilled(RegularShapeIsFilled);

	// Set up the shape
	pRegularShape->SetNumSides(NumSides);
	pRegularShape->SetCircular(Circular);
	pRegularShape->SetStellated(Stellated);
	pRegularShape->SetPrimaryCurvature(PrimaryCurvature);
	pRegularShape->SetStellationCurvature(StellationCurvature);
	pRegularShape->SetStellRadiusToPrimary(StellRadiusToPrimary);
	pRegularShape->SetPrimaryCurveToPrimary(PrimaryCurveToPrimary);
	pRegularShape->SetStellCurveToStell(StellCurveToStell);
	pRegularShape->SetStellationRatio(StellOffsetRatio);
	pRegularShape->SetCentrePoint(UTCentrePoint);
	pRegularShape->SetMajorAxes(UTMajorAxes);
	pRegularShape->SetMinorAxes(UTMinorAxes);

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessEnvelope()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Returns:	TRUE if the envelope object was processed correctly, 
				FALSE if not.
	Purpose:	Reads in all the elements of an envelope structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessEnvelope()
{
	// NOT IMPLEMENTED
	if (!StartMould(MOULDSPACE_ENVELOPE))
		return FALSE;
	
	if (ProcessMould())
		// If we've completed then all is well
		return EndMould();

	if (EPSFile->eof())
	{
		// Didn't find end of envelope - syntax error; deal with it
		HandleEPSError();
	}

	// if made it here all is not well
	return FALSE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessPerspective()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Returns:	TRUE if the perspective object was processed correctly, 
				FALSE if not.
	Purpose:	Reads in all the elements of an perspective structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory
	SeeAlso:	ProcessEnvelope() for furher details

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessPerspective()
{
	// NOT IMPLEMENTED
	if (!StartMould(MOULDSPACE_PERSPECTIVE))
		return FALSE;

	// If we've completed then all is well
	if (ProcessMould())
		return EndMould();

	if (EPSFile->eof())
	{
		// Didn't find end of perspective - syntax error; deal with it
		HandleEPSError();
	}

	// if made it here all is not well
	return FALSE;
}




/********************************************************************************************

>	BOOL CamelotEPSFilter::StartMould(MouldSpace mSpace)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	14/03/95
	Inputs:		mSpace = Mould geometry to define, either envelope or perspective
	Returns:	TRUE if the new mould object was created ok;
				FALSE if not.
	Purpose:	Used when an mould structure needs to be created - after this is called,
				all new nodes added with AddNewNode() will be added as children of this
				new mould, until EndMould is called.
	Errors:		Out of memory
	SeeAlso:	CamelotEPSFilter::EndMould

********************************************************************************************/

BOOL CamelotEPSFilter::StartMould(MouldSpace mSpace)
{
	// Read in the version number of this camelot mould
	INT32 version;
	if (!Stack.Pop(&version))
		return FALSE;

	// if the version number's not one we recognise then return an error
	if (version!=100)
		return FALSE;	

	// create a mould parent and insert it in the tree
	NodeMould* pMouldParent = new NodeMould;
	ERROR1IF(pMouldParent == NULL, FALSE, _R(IDT_EPS_NOMEMORY));

	// give the parent mould object a shape and mould space to work with and stick it in the tree
	if (!pMouldParent->CreateGeometry(mSpace))
	{
		delete pMouldParent;
		return FALSE;
	}

	if (!AddNewNode(pMouldParent))
	{
		HandleNoMemory();
		return FALSE;
	}

	// Make sure new objects are added as children of the mould
	pNode = pMouldParent;

	// Make sure the default mould threshold is set correctly
	EPSFilter::NewMouldThreshold = MOULD_V1THRESHOLD;

	// All ok
	return TRUE;
}



/********************************************************************************************

>	BOOL CamelotEPSFilter::EndMould()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Returns:	TRUE if the envelope was ended ok;
				FALSE if not.
	Purpose:	Used when a mould has finished being constructed and we want to return to
				normal node positioning.
	SeeAlso:	CamelotEPSFilter::StartMould()

********************************************************************************************/

BOOL CamelotEPSFilter::EndMould()
{
	// Sanity check
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeMould)), "No MouldParent in CamelotEPSFilter::EndMould");

	// Get a pointer to the mould object
	NodeMould* pNodeMould = (NodeMould*)pNode;
	// move pNode up one level
	pNode = pNode->FindParent();
	ERROR3IF(pNode==NULL, "No parent of mould in CamelotEPSFilter::EndMould");

	// for loaded docs we need to set the default threshold to the
	// old 1024 threshold until such time as it changes by way of parsing
	// the threshold token. This avoids blends from remapping!
	pNodeMould->GetGeometry()->SetThreshold(EPSFilter::NewMouldThreshold);

	// We should really have these items, otherwise well, we have nothing whatsoever
	NodeMouldPath* pNodeMouldPath = (NodeMouldPath*)pNodeMould->FindFirstChild(CC_RUNTIME_CLASS(NodeMouldPath));
	NodeMouldGroup* pNodeMouldGroup = (NodeMouldGroup*)pNodeMould->FindFirstChild(CC_RUNTIME_CLASS(NodeMouldGroup));

	if (pNodeMouldPath==NULL || pNodeMouldGroup==NULL)
	{
		// delete the object and all its children
		pNodeMould->CascadeDelete();
	}

	// Finally trash any unwanted attributes which may have been added
	// by SnapShotCurrentAttributes.
	RemoveUnwantedAttributes(pNodeMould);

	// All is well and lovely
	return TRUE;
}


/********************************************************************************************
	
	BOOL CamelotEPSFilter::RemoveUnwantedAttributes(NodeMould* pNodeMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/95
	Inputs:		pNodeMould = a pointer to a node mould object
	Returns:	TRUE if all unwanted attributes were removed ok.
				FALSE if none have been removed
	Purpose:	Removes unwanted attributes added as first children of the node mould.
				It does this by scanning the children of the nodemouldgroup and determining
				what attributes are applied here.

********************************************************************************************/

BOOL CamelotEPSFilter::RemoveUnwantedAttributes(NodeMould* pNodeMould)
{
	/* Ok, at this stage, we have a list of attributes applied to the
	   first level of the node mould. All these atts have been applied to
	   avoid them being applied to the mould source objects which would
	   then be moulded (not what we want). Unfortunately we now need to check
	   all attributes applied to the source objects. This is unfortunately
	   a side effect of the method we are using. If we find that there
	   are attributes applied in the source object tree which have been
	   applied to the root, then we delete those at the root. Groan!
	*/

	// grab the node mould group.
	NodeMouldGroup* pNdMldGrp = (NodeMouldGroup*)pNodeMould->FindFirstChild(CC_RUNTIME_CLASS(NodeMouldGroup));
	if (pNdMldGrp==NULL)
		return FALSE;

	// ok, build a list of attribute pointers.
	CCAttrMap AttrMap(30);
	CCRuntimeClass* pTypeInfo;
	void* pDummy;

	BuildSubtreeAttrMap(&AttrMap, pNdMldGrp);
	
	Node *qNode, *pNode = pNodeMould->FindFirstChild();
	while (pNode)
	{
		qNode = pNode->FindNext();
		if (pNode->IsAnAttribute())
		{
			NodeAttribute* pAttrib = (NodeAttribute*)pNode;
			pTypeInfo = pAttrib->GetAttributeType();
			if (AttrMap.Lookup(pTypeInfo, pDummy))
			{
				pNode->UnlinkNodeFromTree();
				delete pNode;
			}
		}
		pNode = qNode;
	}

	return TRUE;
}



/********************************************************************************************

>	void CamelotEPSFilter::BuildSubtreeAttrMap(CCAttrMap *pAttribMap, Node* pParent)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/95
	Inputs:		pAttribMap  = a pointer to an attribute map
				pParent		= a pointer to the parent node
	Returns:	-
	Purpose:	Adds all attribute pointers to the pAttribMap which it finds as children
				of pParent.
	Notes:		Told off by MarkN for not commenting ... oopps
	
********************************************************************************************/

void CamelotEPSFilter::BuildSubtreeAttrMap(CCAttrMap *pAttribMap, Node* pParent)
{
	CCRuntimeClass* pTypeInfo;
	void* pDummy;

	Node* qNode = pParent->FindFirstChild();
	while (qNode)
	{
		if (qNode->IsAnAttribute())
		{
			NodeAttribute* pAttrib = (NodeAttribute*)qNode;
			pTypeInfo = pAttrib->GetAttributeType();
			if (!pAttribMap->Lookup(pTypeInfo, pDummy))
				pAttribMap->SetAt(pTypeInfo, qNode);
		}

		if (qNode->FindFirstChild())
		{
			BuildSubtreeAttrMap(pAttribMap, qNode);
		}

		qNode = qNode->FindNext();
	}
}




/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessMould()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Returns:	TRUE if the mould was processed ok;
				FALSE if not.
	Purpose:	Processes tokens inside a mould object. Tokens specific to mould objects
				are processed along with some overridding of default tokens such as groups
	SeeAlso:
	
********************************************************************************************/

BOOL CamelotEPSFilter::ProcessMould()
{
	// Try to make a snap shot of the current attribute state
	SnapShotCurrentAttrs();

	// process the mould
	BOOL success=ProcessMouldTokens();

	// Get rid of the snap shot again.
	DeleteSnapShot();

	return success;
}




/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessMouldTokens()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Returns:	TRUE if the mould was processed ok;
				FALSE if not.
	Purpose:	Processes tokens inside a mould object. Tokens specific to mould objects
				are processed along with some overridding of default tokens such as groups
	SeeAlso:
	
********************************************************************************************/

BOOL CamelotEPSFilter::ProcessMouldTokens()
{	
	// Keep processing tokens until we find the end of the envelope
	while (!EPSFile->eof())
	{
		GetToken();

		// Look for the end of the mould token...
		switch (Token)
		{
			case EPSC_ceev:
			case EPSC_cepr:
				// found the end of the mould so return done.
				return TRUE;
				break;

			case EPSC_csmp:
				// if we've found the envelope/perspective end then
				// all is well.
				if (!ProcessMouldShape())
					return FALSE;
				break;

			case EPSC_csso:
			{
				// Remove all current attributes which don't match snap shot
				// saving their values on the stack. (These should not be applied)
				// (See ApplyChangedAttrs call below)
				PushCurrentAttrsBasedOnSnapShot();

				// Process all mould source objects
				BOOL Success = ProcessMouldSourceObjs();
			
				// Put back the current attrs how they were.				
				PopCurrentAttrsBasedOnSnapShot();

				if (!Success)
					return FALSE;
			}
			break;

			case EPSC_csdo:
				// Create next object as a child
				if (!ProcessMouldDestinObjs())
					return FALSE;
				break;

			default:
				// try to handle whatever token this is.
				if (!HandleToken())	
					return FALSE;
				break;
		}
	}

	HandleEPSError();
	return FALSE;
}



/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessMouldThreshold()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/06/95
	Returns:	TRUE if the threshold was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a mould threshold structure in the EPS file.
				The threshold structure is defined as an extension token as it only came into
				existance after version 1.1 documents. This function is called from the
				extension token parser. We read the threshold value and set the EPSFilter
				variable NewMouldThreshold. Having read in the mould, the threshold will be
				read from this variable and overwrite the default constructed threshold.

	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessMouldThreshold()
{
	EPSFilter::NewMouldThreshold = MOULD_V1THRESHOLD;

	while (!EPSFile->eof())
	{
		GetToken();
		switch (Token)
		{
			case EPSC_ceo:
			{
				// found the end of the threshold block so return done.
				return TRUE;
			}
			break;

			case EPSC_cmth:
			{
				INT32 Threshold;
				if (!Stack.Pop(&Threshold))
				{
					HandleEPSError();
					return FALSE;
				}
				EPSFilter::NewMouldThreshold = Threshold;
			}
			break;

			default:
			{
				// try to handle whatever token this is.
				if (!HandleToken())	
					return FALSE;
			}	
			break;
		}
	}

	HandleEPSError();
	return FALSE;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessMouldShape()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Returns:	TRUE if the shape has been read correctly, 
				FALSE if not.
	Purpose:	Reads in all the elements of a mould geometry held in the EPS file.
				The description of the geometry for this particular file type is that of
				a path. 
				Input Stack = coord, coord
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessMouldShape()
{
	// We really should be inside a mould object at this stage
	if (!IS_A(pNode,NodeMould)) return FALSE;
	NodeMould* pNodeMould = (NodeMould*)pNode;

	Path TempPath;
	if (!TempPath.Initialise(24,12))
		return FALSE;

	BOOL done = FALSE;
	// Keep processing tokens until we find the end of the mould path
	while (!done && (!EPSFile->eof()))
	{
		GetToken();

		switch (Token)
		{

			case EPSC_m:
				if (!ReadSimplePath(&TempPath))
					return FALSE;
				break;

			case EPSC_cemp:
				done=TRUE;
				break;

			default:
				// try to handle whatever token this is.
				if (!HandleToken())
					return FALSE;
				break;
		}
	}

	if (EPSFile->eof())
	{
		HandleEPSError();
		return FALSE;
	}

	// We have read the path so now lets try and build the geometry
	// check the shape we've been given is fine and lovely	
	UINT32 errID;
	if (!pNodeMould->GetGeometry()->Validate(&TempPath,errID))
	{
		// ok the path we read is invalid so lets try to build a valid one
		Path* pPath = NULL;
		INT32 Corners[4] = {0, 3, 6, 9}; 
		if (!pNodeMould->GetGeometry()->MakeValidFrom(&pPath, &TempPath, Corners))
			return FALSE;
		if (!TempPath.CloneFrom(*pPath))
		{
			delete pPath;
			return FALSE;
		}
		delete pPath;
	}

	// Build and if necessary fit the geometry
	NodeMouldPath* pNodeMPath = pNodeMould->CreateNewMouldShape(&TempPath,NULL,ImportInfo.pOp);
	if (pNodeMPath==NULL) 
		return FALSE;

	// having created the shape bung it in the tree.
	if (!AddNewNode(pNodeMPath))
	{
		delete pNodeMPath;
		return FALSE;
	}

	/* Ok, what we need to do now is apply the current set of attributes
	   as first children of the pNodeMould. This will apply all none default
	   attributes at this level. We need to do this as we dont want the current
	   attributes as they are now to be applied to objects within the source
	   mould object tree. If they do get applied there, they will be moulded
	   and will look wrong.
	   Background:
	   There is a problem I'm fixing here. Currently you can apply attributes
	   to moulds eg dragging a linear fill over a selected mould. This creates
	   an attribute as a first child of the mould, outside the source object
	   group which gets moulded. So the att is not moulded. Right, when you
	   save, these atts are put into the file before the nodemouldpath.
	   On loading currently all atts are gathered and applied to each object
	   as it is created. This means atts that weren't moulded migrate onto
	   objects which will be moulded and get moulded. yuk. So what I am doing
	   is to save the CurrentAttrs state when we find a start mould token.
	   Then when we find the nodemouldpath, apply all the current atts and
	   reset the current set back to the saved set. That means all the atts
	   that appear before the nodemouldpath dont get applied to the source objects
	   and only those that appear later do.
	*/

	if (!ApplyChangedAttrs(pNodeMould))
		return FALSE;
	
	// All is well.
	return TRUE;
}







/********************************************************************************************

>	BOOL CamelotEPSFilter::ReadSimplePath(Path* TempPath)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Returns:	TRUE if the path shape has been read
				FALSE if not.
	Purpose:	Reads in the defined mould path shape.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ReadSimplePath(Path* TempPath)
{
	DocCoord Ta,Tb,Tc;
	PathFlags flags;
	flags.IsSelected = FALSE;
	BOOL ok, done = FALSE;

	// ok, pop off the move	stacked coordinate
	if (!Stack.PopCoordPair(&Ta))
		return FALSE;

	// Make sure there's absolutely no elements in this path
	TempPath->ClearPath(FALSE);

	// Add this first element as a move to
	if (!TempPath->AddMoveTo(Ta, &flags))
		return FALSE;

	// Keep processing tokens until we find the end of the mould path
	while (!done && (!EPSFile->eof()))
	{
		GetToken();

		switch (Token)
		{
			case EPSC_l:
				// lineto (x,y) found
						ok = Stack.PopCoordPair(&Ta);
				if (ok) ok = TempPath->AddLineTo(Ta, &flags);
				break;
												
			case EPSC_c:
				// curveto (x0,y0,x1,y1,x2,y2) found
						ok = Stack.PopCoordPair(&Tc);
				if (ok) ok = Stack.PopCoordPair(&Tb);
				if (ok)	ok = Stack.PopCoordPair(&Ta);
				if (ok) ok = TempPath->AddCurveTo(Ta,Tb,Tc,&flags);
				break;

			case EPSC_s: case EPSC_f: case EPSC_b: case EPSC_h:	case EPSC_n:
			case EPSC_S: case EPSC_F: case EPSC_B: case EPSC_H:	case EPSC_N:
				// found a stroked close path. That should be it for path elements
				ok = TempPath->CloseSubPath();
				done = TRUE;
				break;

			default:
				// try to handle whatever token this is.
				ok = HandleToken();
				break;
		}

		// if someones failed return.
		if (!ok) return FALSE;
	}

	// All is well
	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessMouldSourceObjs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Returns:	TRUE if the source objects have been read correctly, 
				FALSE if not.
	Purpose:	Reads in all the objects within the mould source grouping.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessMouldSourceObjs()
{
	// Sanity check
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeMould)), "No MouldParent in CamelotEPSFilter::ProcessMouldSourceObjs");
	NodeMould* pNodeMould = (NodeMould*)pNode;

	// Create a mould group object
	NodeMouldGroup*	pMouldGroup = pNodeMould->CreateNewMouldGroup(ImportInfo.pOp);
	if (pMouldGroup==NULL) 
		return FALSE;

	if (!AddNewNode(pMouldGroup))
	{
		delete pMouldGroup;
		return FALSE;
	}

	// save the old insert position	and position to our group
	Node* pOldPos = pNode;
	pNode = pMouldGroup;

	// scan the tokens until we find the one we want
	BOOL done = FALSE;
	while (!done && (!EPSFile->eof()))
	{
		GetToken();
		if (!(done=(Token==EPSC_ceso)))
		{
			if (!HandleToken())
			{	
				pNode=pOldPos;
				return FALSE;
			}
		}
	}

	// reposition our magic insert position	
	pNode = pOldPos;

	if (EPSFile->eof())
	{
		HandleEPSError();
		return FALSE;
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessMouldDestinObjs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Returns:	TRUE if the source objects have been read correctly, 
				FALSE if not.
	Purpose:	Reads in all the objects within the mould destination grouping.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessMouldDestinObjs()
{
	// Sanity check
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeMould)), "No MouldParent in CamelotEPSFilter::ProcessMouldDestinObjs");
	NodeMould* pNodeMould = (NodeMould*)pNode;

	// create a new moulder object
	NodeMoulder* pMoulder = pNodeMould->CreateNewMoulder(ImportInfo.pOp);
	if (!pMoulder)
		return FALSE;

	// Add it into the tree as the last child of the mould
	if (!AddNewNode(pMoulder))
		return FALSE;

	// save the old insert position
	Node* pOldPos=pNode;
	pNode=pMoulder;

	// scan the tokens until we find the one we want
	BOOL done=FALSE;
	while (!done && (!EPSFile->eof()))
	{
		GetToken();
		if (!(done=(Token==EPSC_cedo)))
		{
			if (!HandleToken())
			{	
				pNode=pOldPos;
				return FALSE;
			}
		}
	}

	// reposition our magic insert position	
	pNode=pOldPos;

	if (EPSFile->eof())
	{
		HandleEPSError();
		return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessGuideLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Returns:	TRUE if the guide layer was processed correctly, FALSE if not.
	Purpose:	Reads in a guide layer object
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessGuideLayer()
{
	String_64 GuideColourName;
	PColourCMYK CMYK;
	FIXEDPOINT TintVal;

	String_256 LayerName;
	INT32 Visible;
	INT32 Locked;
	INT32 Foreground;
	INT32 Printable;

	// Read in the guide layer params
	if (!Stack.PopColour(&CMYK, TINT_ILLUSTRATOR, &TintVal, &GuideColourName)	||	
		!Stack.Pop(&Locked)  	|| 
		!Stack.Pop(&Printable)	|| 
		!Stack.Pop(&Visible) 	|| 
		!Stack.Pop(&Foreground)	|| 
		!Stack.Pop(&LayerName))
	{
		return FALSE;
	}

#if defined(EXCLUDE_FROM_RALPH)
	// In ralph we dont want to import guideline layers
#else
	if (Filter::ImportWithLayers)
	{
		// We are importing layers, so put all new nodes on this layer.
		UseLayer(LayerName,TRUE);

		// Try to set the visible/locked flags on this layer, but only if
		// we created a new one (i.e. don't change the flags of existing layers.
		if (EPSFlags.AddToNewLayer)
		{
			// Here we force layers to be printable if they are in the foreground - just in case
			// This will have to change if the UI allows Printable & Foreground to be specified
			// independantly.
			Printable = Foreground;

			// Visible flag
			// Only relevant for non-guide layers. Guide layer visibility is stored in the DocView
			// and is saved in the document comments header

			// Locked flag
			pLayer->SetLocked(!(Locked == 0));

			// Printable flag
			pLayer->SetPrintable(!(Printable == 0));

			// Background flag
			pLayer->SetBackground(Foreground == 0);

			DocColour Col;

			GetEPSColour(&Col,&CMYK,TINT_ILLUSTRATOR,TintVal,&GuideColourName);
			IndexedColour* pIndexedColour = Col.FindParentIndexedColour();
			pLayer->SetGuideColour(pIndexedColour);
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSFilter::ProcessGuideline()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Returns:	TRUE if the guideline was processed correctly, FALSE if not.
	Purpose:	Reads in a guide line object
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL CamelotEPSFilter::ProcessGuideline()
{
	INT32 Type, Ordinate;

	// Read in the guideline's params
	if (!Stack.Pop(&Type) 	||
		!Stack.Pop(&Ordinate))
	{
		return FALSE;
	}

#ifdef WEBSTER
	// Neville 8/8/97
	// If we aren't importing with layers into Webster, which we wont by default,
	// then ignore any guidelines, as these should only be loaded onto guide layers
	if (!Filter::ImportWithLayers)
		return TRUE;
#endif

	NodeGuideline* pGuideline = new NodeGuideline;
	ERROR1IF(pGuideline == NULL, FALSE, _R(IDT_EPS_NOMEMORY));

	// Add it into the tree
	if (!AddNewNode(pGuideline))
		return FALSE;

	pGuideline->SetType(GuidelineType(Type));
	pGuideline->SetOrdinate(Ordinate);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////
// EPSFontCache methods.

/********************************************************************************************

>	EPSFontCache::EPSFontCache()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/30/95
	Purpose:	Set up the EPS font cache to be initially invalid.
	SeeAlso:	CamelotEPSRenderRegion

********************************************************************************************/

EPSFontCache::EPSFontCache()
{
	// Font cache is invalid
	Valid = FALSE;
};

#if !defined(EXCLUDE_FROM_RALPH)

////////////////////////////////////////////////////////////////////////////////////////////
// CamelotEPSRenderRegion methods.

CC_IMPLEMENT_DYNAMIC(CamelotEPSRenderRegion, ArtWorksEPSRenderRegion)

/********************************************************************************************

>	CamelotEPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Purpose:	Initialise a render region for exporting Camelot EPS. Sets up the
				string to put in the %%Creator comment.
	SeeAlso:	EPSRenderRegion::EPSRenderRegion

********************************************************************************************/

CamelotEPSRenderRegion::CamelotEPSRenderRegion(DocRect ClipRect,
												 Matrix ConvertMatrix, 
												 FIXED16 ViewScale) 
	: ArtWorksEPSRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
	CreatorString = _T("Xara X");

	SepTables = NULL;
}



/********************************************************************************************

>	CamelotEPSRenderRegion::~CamelotEPSRenderRegion()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Purpose:	destructor

********************************************************************************************/

CamelotEPSRenderRegion::~CamelotEPSRenderRegion()
{
	if (SepTables != NULL)
	{
		CCFree(SepTables);
		SepTables = NULL;
	}
}



/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::StartRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Prepare the render region for rendering (exporting).
	SeeAlso:	EPSRenderRegion::Initialise; EPSRenderRegion::StopRender

********************************************************************************************/

BOOL CamelotEPSRenderRegion::StartRender()
{
	// (ChrisG 3/1/01) - We no longer write out the restore/save pair, so that multiple objects
	//	(both simple and complex) can be clipped together. This now works the same as the 
	//	printing (see PrintPSRenderRegion::StartRender)

	// This should only be done around areas where there is a possibility of the VM being 
	//	modified without our knowledge, e.g. when including EPS files inside this file, using
	//	third-party functions, etc..., and the save should be done immediately before the 
	//	suspect block, and the restore immediately after.

	// Call base class first
	if (!EPSRenderRegion::StartRender())
		return FALSE;

	InitAttributes ();

	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::StopRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	Deinitialise the render region after rendering (exporting).
	SeeAlso:	EPSRenderRegion::Deinitialise; EPSRenderRegion::StartRender

********************************************************************************************/

BOOL CamelotEPSRenderRegion::StopRender()
{
	// (ChrisG 3/1/01) - We no longer write out the restore/save pair, so that multiple objects
	//	(both simple and complex) can be clipped together. This now works the same as the 
	//	printing (see CamelotEPSRenderRegion::StartRender for more info)

	if (!IS_A(this, NativeRenderRegion))
	{
		// Since we're clearing the the current attributes in the postscript file (using 
		//	restore), we'd better clear them in the exporter, so that they are re-exported
		//	if needed. NOTE: This only seems to affect font info.

		FontInfo.Valid = 0;
	}

	if (SepTables != NULL)
	{
		CCFree(SepTables);
		SepTables = NULL;
	}

	DeInitAttributes ();

	return TRUE;
}


/********************************************************************************************

>	virtual void CamelotEPSRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void CamelotEPSRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	// We can do grad fills, and most kinds of bitmaps.
	pCaps->CanDoNothing();
	pCaps->GradFills = TRUE;
	pCaps->SimpleBitmaps = TRUE;
	pCaps->ArbitraryBitmaps = TRUE;
	pCaps->ClippedSimpleBitmaps = TRUE;
	pCaps->ClippedArbitraryBitmaps = TRUE;
	pCaps->ClippedOutput = TRUE;
}

/********************************************************************************************

>	void CamelotEPSRenderRegion::GetValidPathAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	See EPSRenderRegion::GetValidPathAttributes.
				This version checks and handles grad fill colours before calling the
				base class version.
	SeeAlso:	EPSRenderRegion::GetValidPathAttributes

********************************************************************************************/

void CamelotEPSRenderRegion::GetValidPathAttributes()
{
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

	FillGeometryAttribute *pFillAttr = 
		(FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(GradFillAttribute)) &&
		SetLastOutputAttribute(ATTR_FILLGEOMETRY))
	{
		// If the fill is square
		if (!pFillAttr->IsASquareFill() && !pFillAttr->IsAThreeColFill())
		{
			// Get the correct brush - may be a grad fill.
			if (//pFillAttr->IsKindOf(CC_RUNTIME_CLASS(GradFillAttribute)) &&
				!pFillAttr->IsKindOf(CC_RUNTIME_CLASS(BitmapFillAttribute)) )
			{
				// Output a grad fill command...

				// Get the start colour and end colour in CMYK, and output them.
				GradFillAttribute *pGradFillAttr = (GradFillAttribute *) pFillAttr;

				// Check if the colours are named or not.
				BOOL UnnamedColours = FALSE;
				BOOL GreyFill = FALSE;

				// During a normal CamEPS export we will not be separating, hence
				// pSeparation will always return NULL here. If however we are
				// currently printing we need to perform some giggery pokery with
				// grad fills in order to separate them correctly. 
				
				// We have two fill operators we can use in Postscript, these
				// are cax, a colour fill op and cag a grey fill op. 
				// There are three items of interest to us which we need to consider
				// before choosing to use one of the postscript fill operators, these
				// are
				// (1) the end colours of the fill (the model etc)
				// (2) the fill effect across the grad fill
				// (3) the output plate we're rendering to.
				//
				// We need to work out what happens on both cmyk and spot plates when
				// using one of these operators. For instance when using a grey fill
				// operator, the fill effect will not be used. It may go out in the 
				// output stream, but it will be ignored in Postscript (because you
				// are using a grey fill of course!).
				// You also need to consider that if you use the colour fill operator
				// you are in effect restricting output to the process colour plates.
				// Nothing will appear on spot plates. The colour operator works by
				// performing a separation itself, using the current fill effect and
				// wandering about in rgb space.
				// You also need to know the rules for interaction between mixed colours
				// during a separation, ie if we have a rgb colour to spot colour fill
				// with a rainbow fill effect, question which should immediately leap/
				// to mind are
				// (1) do we use the fill effect
				// (2) what fill is used on the spot plate
				// (3) what fill is used on the process plates
				// (4) how will this all look, is it sensible?

							
				// We should really define a virtual function for this operation and
				// override it in PrintPSRenderRegion.

				ColourContext* pContext;
				ColourPlate* pSeparation;
				GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

				if (pSeparation!=NULL)
				{
					DocColour *Start, *End;
					Start =	&(pGradFillAttr->Colour);
					End   = &(pGradFillAttr->EndColour);
					BOOL StartIsSpot = (Start->GetSpotParent() != NULL);
					BOOL EndIsSpot   = (End->GetSpotParent() != NULL);

					// If we are rendering a spot plate 
					GreyFill = (pSeparation->GetType() == COLOURPLATE_SPOT);
					// or either end of this fill are spot... use the gray fill operator
					// This will mean process colour will be separated and provide a linear
					// fill to white on none spot plates.
					GreyFill = (GreyFill || StartIsSpot || EndIsSpot);

					if (!GreyFill)
					{
						// ok we're on a process plate, and both ends are not spot
						// are both ends CMYK colours with a simple fill effect?
						GreyFill = ( Start->GetColourModel() == COLOURMODEL_CMYK ) && 
								   (   End->GetColourModel() == COLOURMODEL_CMYK );

						if (GreyFill)
						{
							// Is there a crazy fill effect pending?
							UINT32 filleffect;
							BOOL exists = GetCurrFillEffect(filleffect);
							GreyFill = (!exists || filleffect == FILLEFFECT_FADE);
						}
					}
				}

				// if GreyFill is true we output this fill as a single plate gray fill
				// otherwise we splunge it across the process plates.
					
				if (GreyFill)
				{
					UnnamedColours = TRUE;
					// both of these colours are cmyk so we need to handle the separation
					// slightly differently. The user will expect these colours to spread
					// without going through rgb=>devcmyk to do so.
					PColourCMYK sCMYK, eCMYK;

					pGradFillAttr->Colour.GetCMYKValue(pContext, &sCMYK);
					pGradFillAttr->EndColour.GetCMYKValue(pContext, &eCMYK);

					// Sanity check on this plate
					ERROR3IF( (sCMYK.Cyan!=0 || sCMYK.Magenta!=0 || sCMYK.Yellow!=0 ||
							   eCMYK.Cyan!=0 || eCMYK.Magenta!=0 || eCMYK.Yellow!=0) , 
							 "Not a mono plate during export of CMYK gradfills (CAMEPSRndRgn::GetValidPathAttributes())" );

					// output a linear key fill.
					BOOL ok = pDC->OutputColourValue(255 - sCMYK.Key);
					ok = ok && pDC->OutputColourValue(255 - eCMYK.Key);
					// This entire function seems to ignore disc failure which is a bit rampant!
					ERROR3IF(!ok, "Disc buffer failure during gradfill output in GetValidPathAttributes");
				}
				else
				{
					// This is a mixture of colour spaces so simply output the
					// who lot as normal. Postscript will spread the colour
					UnnamedColours = OutputGradFillColours(	&(pGradFillAttr->Colour), &(pGradFillAttr->EndColour) );
				}

				// Output the start and end points of the grad fill
				pDC->OutputCoord(pGradFillAttr->StartPoint);
				pDC->OutputCoord(pGradFillAttr->EndPoint);


				// Output the fill type
				if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(LinearFillAttribute)))
				{
					LinearFillAttribute *pLinearFillAttr = (LinearFillAttribute *) pFillAttr;

					DocCoord* Start = pFillAttr->GetStartPoint();
					DocCoord* End   = pFillAttr->GetEndPoint();
					DocCoord* End2  = pFillAttr->GetEndPoint2();

					if (AreLinesPerpendicular(Start, End, End2))
					{
						TRACEUSER( "Will", _T("Exporting Simple Linear Fill\n"));
						pDC->OutputValue((INT32) CAMEPS_FILL_LINEAR);
					}
					else
					{
						TRACEUSER( "Will", _T("Exporting New Style Linear Fill\n"));
						pDC->OutputCoord(pLinearFillAttr->EndPoint2);
						pDC->OutputValue((INT32) CAMEPS_FILL_NEWLINEAR);
					}
				}
				else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(RadialFillAttribute)))
				{
					// Is it circular or elliptical?
					RadialFillAttribute *pRadialFillAttr = (RadialFillAttribute *) pFillAttr;
					if (pRadialFillAttr->IsElliptical())
					{
						// Elliptical fill - output the second end point.
						pDC->OutputCoord(pRadialFillAttr->EndPoint2);
						pDC->OutputValue((INT32) CAMEPS_FILL_ELLIPTICAL);
					}
					else
					{
						// Circular fill
						pDC->OutputValue((INT32) CAMEPS_FILL_CIRCULAR);
					}
				}
				else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(ConicalFillAttribute)))
				{
					pDC->OutputValue((INT32) CAMEPS_FILL_CONICAL);
				}
				else
				{
					// Bodge: ought to handle this more gracefully soon...
					ENSURE(FALSE, "Unsupported grad fill encountered while exporting");
					EPSRenderRegion::GetValidPathAttributes();
					return;
				}

				// Output the grad fill token
				if (UnnamedColours)
				{
					if (GreyFill)
						pDC->OutputToken(_T("cag"));
					else
						pDC->OutputToken(_T("caz"));
				}
				else
				{
					pDC->OutputToken(_T("cax"));
				}
				pDC->OutputNewLine();
			}
			else if (	// Utterly awful code by WillC
						(pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FractalFillAttribute)) ||
						 pFillAttr->IsKindOf(CC_RUNTIME_CLASS(NoiseFillAttribute))) &&
						 Caps.BitmapFills
					)
			{
				// Fractal fill - output fractal fill command...

				//*************************************************************************
				// this piece of code is copied from the bitmap export but differs
				// only very slightly. Is this difference a bug or intended? We will never
				// know.

				// first output the base class fill stuff...
				BitmapFillAttribute *pFill = (BitmapFillAttribute *) pFillAttr;

				// Get the start colour and end colour in CMYK, and output them.
				// Check if the colours are named or not.
				BOOL UnnamedColours = FALSE;
				ColourContext *cc = NULL;
				
				// (ChrisG 8/12/00) All non-grey grad fill colours are now exported by the 
				// OutputGradFillColours function.
				if ((pFill->Colour.FindParentIndexedColour() == NULL) &&
					(pFill->EndColour.FindParentIndexedColour() == NULL))
				{
					// Neither colour is named...use unnamed colour syntax.
					cc = ColourManager::GetColourContext(COLOURMODEL_CMYK, GetRenderView());
					ERROR3IF(cc == NULL, "No CMYK colour context - somethings' seriously broken");
				}
				else
				{
					cc = NULL;
				}

				// Output the colours (and names)
				UnnamedColours = OutputGradFillColours (&(pFill->Colour), &(pFill->EndColour), cc);

				// Next the coords of the fill
				pDC->OutputCoord(pFill->StartPoint);
				pDC->OutputCoord(pFill->EndPoint);
				pDC->OutputCoord(pFill->EndPoint2);

				//*************************************************************************
				// ok NoiseFillAttributes are saved as FractalFillAttributes. Strange you
				// might say, aha! but NoiseFillAttributes are new objects not supported by
				// camelot 1.1 so what else should we do? Hmm not sure at this point.

				// Save the fractal parameters out.
				pDC->OutputValue(pFill->GetSeed());
				pDC->OutputReal(pFill->GetGraininess().MakeDouble());
				pDC->OutputReal(pFill->GetGravity().MakeDouble());
				pDC->OutputReal(pFill->GetSquash().MakeDouble());

				// And now the DPI of the fractal bitmap
				UINT32 DPI = pFill->GetFractalDPI();
				pDC->OutputValue(DPI);

				// And now the 'tileable' flag
				pDC->OutputValue((INT32) (pFill->GetTileable()));

				// Always Fractal type 1 at present
				pDC->OutputToken(_T("1"));

				// This is a fractal fill
				pDC->OutputValue((INT32) CAMEPS_FILL_FRACTAL);

				if (UnnamedColours)
					pDC->OutputToken(_T("caz"));
				else
					pDC->OutputToken(_T("cax"));
				pDC->OutputNewLine();
			}
			else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(BitmapFillAttribute)) && Caps.BitmapFills)
			{
				// Bitmap fill - output bitmap fill command...
				BitmapFillAttribute *pBitmapFill = (BitmapFillAttribute *) pFillAttr;
				ERROR3IF(pBitmapFill->GetBitmap() == NULL, "Bitmap fill has no bitmap");

				if (pBitmapFill->GetBitmap())
				{
					// Get the start colour and end colour in CMYK, and output them.
					// Check if the colours are named or not.
					BOOL UnnamedColours = FALSE;
					BOOL HasColours = ((pBitmapFill->GetStartColour() != NULL) &&
									   (pBitmapFill->GetEndColour() != NULL));

					if (HasColours)
					{
						// (ChrisG 8/12/00) All non-grey grad fill colours are now exported by the 
						// OutputGradFillColours function.
						ColourContext *cc = NULL;

						if ((pBitmapFill->Colour.FindParentIndexedColour() == NULL) &&
							(pBitmapFill->EndColour.FindParentIndexedColour() == NULL))
						{
							// Neither colour is named...use unnamed colour syntax.
							cc = ColourManager::GetColourContext(COLOURMODEL_CMYK, GetRenderView());
							ERROR3IF(cc == NULL, "No CMYK colour context - somethings' seriously broken");
						}
						else
						{
							// One or two of the colours are named, so use the named colour syntax.
							cc = NULL;
						}

						// Write out the colours themselves.
						UnnamedColours = OutputGradFillColours (&(pBitmapFill->Colour),
																&(pBitmapFill->EndColour),
																cc);
					}

					pDC->OutputCoord(pBitmapFill->StartPoint);
					pDC->OutputCoord(pBitmapFill->EndPoint);
					pDC->OutputCoord(pBitmapFill->EndPoint2);

					if (HasColours)
						pDC->OutputValue((INT32) CAMEPS_FILL_NEWBITMAP);
					else
						pDC->OutputValue((INT32) CAMEPS_FILL_BITMAP);

					if (UnnamedColours)
						pDC->OutputToken(_T("caz"));
					else
						pDC->OutputToken(_T("cax"));

					pDC->OutputNewLine();

					// ...and then the bitmap itself.
					ExportDC *pExportDC = (ExportDC *) pDC;
					pExportDC->GetParentFilter()->ExportBitmap(*pBitmapFill->GetBitmap());
				}
			}
		}
	}

	if (SetLastOutputAttribute(ATTR_FILLEFFECT))
	{
		UINT32 EffectType;
		GetCurrFillEffect(EffectType);

		// Output fill effect
		pDC->OutputValue(EffectType);
	
		// Output the fill effect token
		pDC->OutputToken(_T("cxe"));
		pDC->OutputNewLine();
	}

	if (SetLastOutputAttribute(ATTR_FILLMAPPING))
	{
		// Now do the chromatic fill mapping
		FillMappingAttribute *pFillAttr = 
			(FillMappingAttribute *) CurrentAttrs[ATTR_FILLMAPPING].pAttr;

		// Get the correct mapping
		INT32 MappingType = pFillAttr->Repeat;
//Mark Howitt, 9/10/97. If we are exporting, make sure that repeating grad fill is saved as simple.
#ifdef _DEBUG
		if(MappingType == 4) MappingType = 2;
#endif
		ERROR3IF((MappingType < 1) || (MappingType > 3), "Illegal fill mapping value!");
		
		// Output fill mapping
		pDC->OutputValue((UINT32) MappingType);

		// Allow for future extension of fill mappings.	
		pDC->OutputToken(_T("0"));

		// Output the fill mapping token
		pDC->OutputToken(_T("cxm"));
		pDC->OutputNewLine();
	}

	if (SetLastOutputAttribute(ATTR_WINDINGRULE))
	{
		WindingRuleAttribute *pWindingRuleAttr = 
			(WindingRuleAttribute *) CurrentAttrs[ATTR_WINDINGRULE].pAttr;

		switch (pWindingRuleAttr->WindingRule)
		{
			case NonZeroWinding:
				// Change winding rule to 0, which means non-zero in Camelot EPS.
				pDC->OutputToken(_T("0"));
				pDC->OutputToken(_T("awr"));
				pDC->OutputNewLine();
				break;

			case EvenOddWinding:
				// Change winding rule to 1, which means even-odd in Camelot EPS.
				pDC->OutputToken(_T("1"));
				pDC->OutputToken(_T("awr"));
				pDC->OutputNewLine();
				break;

			case NegativeWinding:
			case PositiveWinding:
				// Not supported in Camelot EPS - ignore.
				break;

			default:
				ERROR3("Unknown winding rule encountered while exporting.");
				break;
				
		}
	}

	// Leave arrows/dash patterns to the base class.

	// Handle usual pens/brushes
	EPSRenderRegion::GetValidPathAttributes();
}



/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::OutputGradFillColours(DocColour* StartCol, 
													   DocColour* EndCol
													   ColourContext pContext = NULL)
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/96
	Inputs:		StartCol = the grad fill start colour
				EndCol	 = the grad fill end colour
				pContext = a pointer to a local colour context (defaults to NULL, in
							which case a global default context will be used instead)
							This context MUST be a CMYK context
	Outputs:	-
	Returns:	TRUE if both colours were unnamed colours;
				FALSE if not
	Purpose:	Output the colour definitions of the start and end colours to be used
				in a graduated fill.

********************************************************************************************/

BOOL CamelotEPSRenderRegion::OutputGradFillColours(DocColour* StartCol, DocColour* EndCol, ColourContext* pContext)
{
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	bool outputNames = FALSE;

	if ((StartCol->FindParentIndexedColour() == NULL) &&
		(EndCol->FindParentIndexedColour() == NULL))
	{
		// Neither colour is named...use unnamed colour syntax.
		outputNames = FALSE;
	}
	else
	{
		// One or two of the colours are named, so use the named colour syntax.
		outputNames = TRUE;
	}

	// Start writing stuff to the EPS file.
	INT32 red = 0;
	INT32 green = 0;
	INT32 blue = 0;

	// Write out the start colour
	StartCol->GetRGBValue (&red, &green, &blue);
	pDC->OutputColourValue (red);
	pDC->OutputColourValue (green);
	pDC->OutputColourValue (blue);

	if (outputNames)
	{
		// Write out the name
		pDC->OutputColourName (StartCol);

		// Write out the tint value. This isn't actually used at the moment, but is left 
		//	for possible future expansion
		pDC->OutputValue ((INT32)0);
	}

	// And the End colour.
	EndCol->GetRGBValue (&red, &green, &blue);
	pDC->OutputColourValue(red);
	pDC->OutputColourValue(green);
	pDC->OutputColourValue(blue);

	if (outputNames)
	{
		// Write out the name
		pDC->OutputColourName (EndCol);

		// Write out the tint value. This isn't actually used at the moment, but is left 
		//	for possible future expansion
		pDC->OutputValue ((INT32)0);
	}

	// Return TRUE for no names used, FALSE for names used.
	return (!outputNames);
}



/********************************************************************************************

>	UINT32 CamelotEPSRenderRegion::GetCurrFillEffect(UINT32& EffectType)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/96
	Inputs:		-
	Outputs:	The current type of fill effect
	Returns:	TRUE if a fill effect exists in the current attrs stack
				FALSE if not
	Purpose:	Return the state of the current fill effect in the EPS attributes stack

********************************************************************************************/

BOOL CamelotEPSRenderRegion::GetCurrFillEffect(UINT32& EffectType)
{
	// set default
	EffectType = FILLEFFECT_FADE;

	FillEffectAttribute *pFillAttr = (FillEffectAttribute *) CurrentAttrs[ATTR_FILLEFFECT].pAttr;
	if (pFillAttr==NULL)
		return FALSE;	

	if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FillEffectFadeAttribute)))
	{
		EffectType = FILLEFFECT_FADE;
	}
	else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FillEffectRainbowAttribute)))
	{
		EffectType = FILLEFFECT_RAINBOW;
	}
	else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FillEffectAltRainbowAttribute)))
	{
		EffectType = FILLEFFECT_ALTRAINBOW;
	}
	else
	{
		ERROR3("Illegal fill effect type!");
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::WriteEPSBoundingBox ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/00
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Calls the base EPS bounding box function.
	SeeAlso:	EPSRenderRegion::WriteEPSBoundingBox

********************************************************************************************/

BOOL CamelotEPSRenderRegion::WriteEPSBoundingBox ( void )
{
	// Call the base class function.
	return EPSRenderRegion::WriteEPSBoundingBox ();
}

/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::WriteProlog(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output any PostScript prolog for this render region.  For EPS and printing,
				this means output of our PostScript rendering procedures; for Native
				files we do nothing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL CamelotEPSRenderRegion::WriteProlog(KernelDC *pDC)
{
	// Get hold of our PostScript prolog resource...
	CCResTextFile PrologFile;

	// Open the file
	if (!PrologFile.open(_R(IDM_PS_PROLOG), _R(IDT_PS_RES)))
	{
		// Failed to open the file...
		ERROR2(FALSE, "Could not get at PostScript resource!");
	} 

	// Put some comments out to mark our procset
	pDC->OutputNewLine();
	pDC->OutputToken(_T("%%BeginResource: procset XaraStudio1Dict"));
	pDC->OutputNewLine();
	pDC->OutputToken(_T("% Copyright (c) 1995,1996 Xara Ltd"));
	pDC->OutputNewLine();

	// Start a new dictionary to avoid clashes...
	pDC->OutputToken(_T("/XaraStudio1Dict 300 dict def XaraStudio1Dict begin"));
	pDC->OutputNewLine();

	// Read each line from the file and output it to the DC.
	String_256 LineBuf;
	TCHAR *pBuf = (TCHAR *) LineBuf;

	while (!PrologFile.read(&LineBuf).eof())
	{
		// Copy this line to output.
		pDC->OutputTCHARAsChar(pBuf, LineBuf.Length());
		pDC->OutputNewLine();
	}

	// All done
	PrologFile.close();

	// Write out any separation functions in this prolog if we
	// need to. Note this is a bool function
	WriteSepFunctions(pDC);

	// Put some comments/code out to mark our procset end
	pDC->OutputToken(_T("end"));
	pDC->OutputNewLine();
	pDC->OutputToken(_T("%%EndResource"));
	pDC->OutputNewLine();

	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::WriteSetup(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output any PostScript setup for this render region.  For EPS and printing,
				this means output of our PostScript code to initialise the context for
				rendering; for Native files we do nothing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL CamelotEPSRenderRegion::WriteSetup(KernelDC *pDC)
{
	// Get hold of our PostScript setup resource...
	CCResTextFile SetupFile;

	// Open the file
	if (!SetupFile.open(_R(IDM_PS_SETUP), _R(IDT_PS_RES)))
	{
		// Failed to open the file...
		ERROR2(FALSE, "Could not get at PostScript resource!");
	} 

	// Output some code so that we use our dictionary
	pDC->OutputToken(_T("save XaraStudio1Dict begin"));
	pDC->OutputNewLine();

	// Read each line from the file and output it to the DC.
	String_256 LineBuf;
	TCHAR *pBuf = (TCHAR *) LineBuf;

	while (!SetupFile.read(&LineBuf).eof())
	{
		// Copy this line to output.
		pDC->OutputTCHARAsChar(pBuf, LineBuf.Length());
		pDC->OutputNewLine();
	}

	// All done
	SetupFile.close();

	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::WriteEPSTrailerComments ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/000
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes out an EPS trailer by calling the base class function.
	SeeAlso:	EPSRenderRegion::CloseDown

********************************************************************************************/

BOOL CamelotEPSRenderRegion::WriteEPSTrailerComments ( void )
{
	// (ChrisG 3/1/01) - Output some code so that we stop using our dictionary, Since
	//	this is no longer controlled by Start- and StopRender, we have restore here. This should 
	//	be done to close the 'save-restore' block started in WriteSetup.
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	pDC->OutputToken(_T("end restore"));
	pDC->OutputNewLine();

	// Call the base class to over-ride the native render region version.
	return EPSRenderRegion::WriteEPSTrailerComments ();
}

/********************************************************************************************

>	void CamelotEPSRenderRegion::ColourSeparateScanline24(BYTE *DestBuffer, BYTE *SrcBuffer,
															INT32 PixelWidth)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96

	Inputs:		DestBuffer	- points to a destination 24bpp RGB scanline buffer
				SrcBuffer	- points to a source 24bpp RGB scanline buffer
				PixelWidth	- The number of pixels to copy from Src to Dest

	Purpose:	Copies the scanline of 24bpp RGB values from SrcBuffer to DestBuffer,
				running the pixels through colour separation tables en route.

				If you're not colour-separating, then you should replace this function
				call with a simple memcpy.

	Notes:		ONLY handles 24bpp RGB scanlines, and expects that SepTables points
				at the separation tables. The pixel bytes must be in the order R,G,B.

				NOTE that it handles data in RGB (postscript) format, rather than the 
				normal Bitmap BGR format!

				All deep bitmap data separates to white on spot plates.

				The CMYK separation plates could have been written generically, but 
				I have separated out the cases in order to squeeze a bit of speed out
				of the inner pixel conversion loop. The conversion algorithm is basically
				like this, only optimised to move as much work out of the loop as
				possible:

				MonoOn
					Cyan	= SepTables + 0		// Find the look up tables
					Magenta	= SepTables + 256
					Yellow	= SepTables + 512
					UCR		= SepTables + 768
					BGT		= SepTables + 1024

					c = 255-r;
					m = 255-g;
					y = 255-b;

					k = min(c, m, y);

					kr = UCR[k];
					ok = BGT[k];

					c -= kr;
					m -= kr;
					y -= kr;

					if (c<0) c=0;
					if (m<0) m=0;
					if (y<0) y=0;

					oc = Cyan[c];
					om = Magenta[m];
					oy = Yellow[y];
			MonoOff

********************************************************************************************/

void CamelotEPSRenderRegion::ColourSeparateScanline24(BYTE *DestBuffer, BYTE *SrcBuffer,
														INT32 PixelWidth)
{
	ERROR3IF(DestBuffer == NULL || SrcBuffer == NULL || PixelWidth < 1, _T("Illegal params"));
	ERROR3IF(SepTables == NULL, _T("No separation tables!?"));
	ERROR3IF(CurrentColContext->GetColourPlate() == NULL,
				_T("The separation ColourPlate has gone missing!"));

	// --- A wee macro to find the Maximum of R,G,B, and place it into a variable called 
	// Temp, which we define right now in function scope so it's always available
	BYTE Temp;
	#define TEMPMAX(R,G,B)					\
	{										\
		Temp = ((R) > (G))  ? (R) : (G);	\
		Temp = (Temp > (B)) ? Temp : (B);	\
	}


	// --- OK, let'ts get busy
	INT32 i;								// Loop i
	BYTE Ink;								// Temporary variable for storing the ink value in

	BYTE *UCR = SepTables + 768 + 255;		// UCR is the 4th table, but this points at the
											// END of the table for optimisation - precalcs (255 - Temp)


PORTNOTE("cms", "Disabled colour separations");
#ifndef EXCLUDE_FROM_XARALX
	BOOL PrintRGBBlackAsKey=XaraCMS::PrintRGBBlackAsKey;
#else
	BOOL PrintRGBBlackAsKey=TRUE;
#endif

	// I've unrolled the shared code for each plate in order to optimise the inner loop
	switch (CurrentColContext->GetColourPlate()->GetType())
	{
		case COLOURPLATE_CYAN:
			{
				BYTE *LUT = SepTables + 0;					// Cyan table is the 1st table

				for (i = 0; i < PixelWidth * 3; i += 3)
				{
					TEMPMAX(SrcBuffer[i], SrcBuffer[i+1], SrcBuffer[i+2]);

					if (!PrintRGBBlackAsKey || Temp > 0)
					{
						Ink = 255 - SrcBuffer[i];			// Cyan ink is (255 - Red)

						// Look up UCR(Key), which is UCR(255 - Temp), but the 255 is precalculated
						// into the base address above, so we i with (-Temp)!
						// Make Ink the ink value for the given plate. Crop at 0
						if ((UINT32)Ink>(UINT32)UCR[-Temp])
							Ink -= UCR[-Temp];
						else
							Ink = 0;

						Ink = LUT[Ink];

						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255 - Ink;
					}
					else
					{
						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255;
					}
				}
			}
			break;


		case COLOURPLATE_MAGENTA:
			{
				BYTE *LUT = SepTables + 256;				// Magenta table is the 2nd table

				for (i = 0; i < PixelWidth * 3; i += 3)
				{
					TEMPMAX(SrcBuffer[i], SrcBuffer[i+1], SrcBuffer[i+2]);

					if (!PrintRGBBlackAsKey || Temp > 0)
					{
						Ink = 255 - SrcBuffer[i+1];			// Magenta ink is (255 - Green)

						// Look up UCR(Key), which is UCR(255 - Temp), but the 255 is precalculated
						// into the base address above, so we i with (-Temp)!
						// Make Ink the ink value for the given plate. Crop at 0
						if ((UINT32)Ink>(UINT32)UCR[-Temp])
							Ink -= UCR[-Temp];
						else
							Ink = 0;

						Ink = LUT[Ink];

						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255 - Ink;
					}
					else
					{
						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255;
					}
				}
			}
			break;


		case COLOURPLATE_YELLOW:
			{
				BYTE *LUT = SepTables + 512;				// Yellow table is the 3rd table

				for (i = 0; i < PixelWidth * 3; i += 3)
				{
					TEMPMAX(SrcBuffer[i], SrcBuffer[i+1], SrcBuffer[i+2]);

					if (!PrintRGBBlackAsKey || Temp > 0)
					{
						Ink = 255 - SrcBuffer[i+2];			// Yellow ink is (255 - Blue)

						// Look up UCR(Key), which is UCR(255 - Temp), but the 255 is precalculated
						// into the base address above, so we i with (-Temp)!
						// Make Ink the ink value for the given plate. Crop at 0
						if ((UINT32)Ink>(UINT32)UCR[-Temp])
							Ink -= UCR[-Temp];
						else
							Ink = 0;

						Ink = LUT[Ink];

						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255 - Ink;
					}
					else
					{
						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255;
					}
				}
			}
			break;


		case COLOURPLATE_KEY:
			{
				BYTE *BGT = SepTables + 1024;			// Black generation is the 5th table

				for (i = 0; i < PixelWidth * 3; i += 3)
				{
					// Get the maximum of R,G,B into Temp
					TEMPMAX(SrcBuffer[i], SrcBuffer[i+1], SrcBuffer[i+2]);

					if (!PrintRGBBlackAsKey || Temp > 0)
					{
						// Key = 255 - Temp. Look up Key in the Black Generation table
						Ink = BGT[255 - Temp];

						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255 - Ink;
					}
					else
					{
						DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 0;
					}
				}
			}
			break;


		case COLOURPLATE_SPOT:
			// Deep colour bitmaps cannot include spot colours, so they always separate out
			// to white...
			for (i = 0; i < PixelWidth * 3; i += 3)
				DestBuffer[i] = DestBuffer[i+1] = DestBuffer[i+2] = 255;
			break;


		default:
			ERROR3(_T("Unsupported separation plate!"));
			break;
	}

	// And finally, vape our helper macro
	#undef TEMPMAX
}



/********************************************************************************************

>	SlowJobResult CamelotEPSRenderRegion::DrawMaskedBitmap(const DocRect &Rect, 
														  KernelBitmap* pBitmap, 
														  MaskedRenderRegion* pMask,
														  ProgressDisplay *Progress)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Inputs:		Point - the position to plot the bitmap
				pBitmap - The bitmap that needs plotting
				pMask - The mask render region to use to indicate which bits of pBitmap
				needs to be plotted

	Purpose:	Plots the bitmap using the mask supplied.

	Notes:		

********************************************************************************************/

SlowJobResult CamelotEPSRenderRegion::DrawMaskedBitmap(const DocRect &Rect, KernelBitmap* pBitmap, 
											  		   MaskedRenderRegion* pMask, 
											  		   ProgressDisplay *Progress)
{
#ifndef STANDALONE

	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

	// Make sure the world is in one piece
	if ((pBitmap==NULL) || (pMask==NULL))
		return SLOWJOB_FAILURE;

	// Are we doing actual EPS? If so, then we need to do some special stuff
	BOOL IsCamelotEPS = IS_A(this, CamelotEPSRenderRegion);

	// If mask coverage is 0% we do not need to do anything. The TRUE param indicates
	// that we don't care about exact coverage, which makes it return as soon as it realises
	// that there are pixel(s) that we'll have to plot
	if (pMask->FindCoverage(TRUE) == 0)
		return SLOWJOB_SUCCESS;

	// Guess we will need to be doing something...
	if (pBitmap->ActualBitmap == NULL)
		return SLOWJOB_FAILURE;

	// Get the 'Actual' windows Bitmap
	CWxBitmap *WinBM = (CWxBitmap*)pBitmap->ActualBitmap;

	// Is it valid ?
	if ((WinBM->BMInfo==NULL) || (WinBM->BMBytes==NULL))
		return SLOWJOB_FAILURE;

	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = WinBM->GetWidth();
	INT32 Height = WinBM->GetHeight();

	if ((Width == 0) || (Height == 0))
		// Error - bitmap has no dimension
		return SLOWJOB_FAILURE;

	// Hideous WINOILY code in the kernel. I get the feeling this was a rush job!
	// Convert the bitmap from a 32bpp bitmap to a 24bpp bitmap
	INT32 BitmapDepth = WinBM->GetBPP();
	if (BitmapDepth == 32)
	{
		// Can't plot 32bpp bitmaps to GDI as 16-bit GDI doesn't understand them,
		// so we convert to 24bpp bitmap in-situ and render that...

		// How many bytes to a source scanline?
		const INT32 ScanlineBytes = WinBM->GetScanlineSize();

		// How many bytes to a destination scanline
		const INT32 DestlineBytes = DIBUtil::ScanlineSize(Width, 24);

		// Now convert the bitmap in-situ
		LPBYTE OriginalBuffer  = WinBM->BMBytes;
		LPBYTE ConvertedBuffer = WinBM->BMBytes;

		INT32 i;
		for (i=0; i<Height; i++)
		{
			DIBUtil::Convert32to24(Width, OriginalBuffer, ConvertedBuffer);
			OriginalBuffer += ScanlineBytes;
			ConvertedBuffer += DestlineBytes;
		}

		// Update bitmap info to show it is now a 24bpp bitmap...
		WinBM->BMInfo->bmiHeader.biSizeImage = DestlineBytes * Height;
		WinBM->BMInfo->bmiHeader.biBitCount  = 24;
		BitmapDepth = 24;

		// Now swap the R and B bytes around as they are in BGR format in DIBS, and
		// we need RGB format for PostScript.
		OriginalBuffer = WinBM->BMBytes;

		for (i=0; i<Height; i++)
		{
			ConvertedBuffer = OriginalBuffer;
			for (INT32 j=0; j<Width; j++)
			{
				// Swap R and B bytes around
				BYTE Temp = ConvertedBuffer[0];
				ConvertedBuffer[0] = ConvertedBuffer[2];
				ConvertedBuffer[2] = Temp;
				ConvertedBuffer += 3;
			}

			OriginalBuffer += DestlineBytes;
		}
	}

	// make sure we have a 24bpp bitmap
	ERROR3IF(BitmapDepth!=24, _T("Non 24bpp bitmap found in DrawMaskedBitmap"));

	// Work out the coords to blit to, taking into acount the differnt dpis of the
	// source and destination bitmaps.
	INT32 SrcDpi = pMask->FindMaskDpi();
	INT32 DestDpi = SrcDpi;
	BOOL UseLevel2 = FALSE;

	if (IsCamelotEPS)
	{
		// EPS is output in points, so it's always 72dpi.
		DestDpi = 72000;

		// Find out if we should do level 2 output.
		if (EPSFilter::XSEPSExportPSType == 2)
			UseLevel2 = TRUE;
	}
	else
	{
		// Find destination device resolution (e.g. PostScript printer)
		if (RenderDC!=NULL)
			DestDpi = RenderDC->GetPPI().GetWidth();
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
		// If printing, then see if we should do level 2.
		if (IsPrinting())
		{
			// Get the print info for this job.
			CCPrintInfo *pInfo = CCPrintInfo::GetCurrent();
			if (pInfo != NULL)
			{
				PrintControl *pPrCtrl = pInfo->GetPrintControl();
				ERROR3IF(pPrCtrl == NULL, _T("Unexpected NULL pointer"));

				if (pPrCtrl->GetPSLevel() == PSLEVEL_2)
					// Printing to a level 2 device - party on!
					UseLevel2 = TRUE;
			}
		}
#endif	//webster
	}

	// Work out the ratio between to two
	double Ratio = DestDpi;
	Ratio = Ratio/SrcDpi;
	TRACEUSER( "Rik", _T("Src = %ld, Dest = %ld, Ratio = %f\n"), SrcDpi, DestDpi, Ratio);

	// Convert the DocCoord into a windows coord
	DocRect ClipRect = pMask->GetClipRect();
	POINT Origin;

	if (IsCamelotEPS)
	{
		// No GDI conversion needed for EPS - we need to convert to points later on.
		Origin.x = ClipRect.lo.x;
		Origin.y = ClipRect.lo.y;
	}
	else
	{
		// Usual GDI conversion (used for PostScript printers)
		WinRect WinClipRect = OSRenderRegion::DocRectToWin(RenderMatrix, ClipRect, DestDpi);
		Origin.x = WinClipRect.GetLeft();
		Origin.y = WinClipRect.GetBottom();
	}

	// Inform progress object how high this band is
	if (Progress!=NULL)
		Progress->StartBitmapPhaseBand(Height);

	// We need to create a tempory bitmap that we use to render each scan line
	LPBITMAPINFO	TempBitmapInfo = NULL;
	LPBYTE			TempBitmapBytes = NULL;

	// Get some memory for a tempory bmp 1 pixel high
	TempBitmapInfo = AllocDIB(Width, 1, BitmapDepth, &TempBitmapBytes);
	if (TempBitmapInfo==NULL)
		return SLOWJOB_FAILURE;

	// Get a buffer for RGB channel separation (for level 2) and colour separation
	BYTE *pRGBBuf = (BYTE *) CCMalloc(Width);
	ERROR2IF(pRGBBuf == NULL, SLOWJOB_FAILURE, _T("No memory for RGB buffer"));

	// How many bytes to a destination scanline
	INT32 ScanLineBytes = DIBUtil::ScanlineSize(Width, BitmapDepth);
	INT32  BytesPerPixel = 3;

	// Now convert the bitmap in-situ
	LPBYTE SrcBuffer  = WinBM->BMBytes;
	LPBYTE DestBuffer = TempBitmapBytes;

	// If we're colour separating, make sure we've got some sep tables to work with.
	// We cache them once we've used them. I would do this sort of set-up in StartRender
	// but of course all the derived classes just override that behaviour and we're stuffed
	if (SepTables == NULL && RenderView->GetColourPlate() != NULL && !RenderView->GetColourPlate()->IsDisabled())
	{
		// We are doing a colour separation - find the sep tables

		ColourContextCMYK *cc = (ColourContextCMYK *)RenderView->GetColourContext(COLOURMODEL_CMYK);
		if (cc != NULL)
		{
			SepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
			if (SepTables != NULL)
			{
				if (!cc->GetProfileTables(SepTables))
				{
					CCFree(SepTables);
					SepTables = NULL;
				}
			}
		}

		ERROR3IF(SepTables == NULL, _T("Can't generate separation tables in CamelotEPSRenderRegion"));
	}

	// Now copy the Bits from our Kernel Bitmap into the Memory DC, scan line at a time
	MaskRegion MaskInfo;
	pMask->GetFirstMaskRegion(&MaskInfo);
	try
	{
		while (MaskInfo.Length!=0)
		{
			// Calculate the source buffer address from the x and y position
			SrcBuffer = WinBM->BMBytes;
			SrcBuffer += ScanLineBytes * MaskInfo.y;
			SrcBuffer += MaskInfo.x*BytesPerPixel;
			INT32 RegionWidth = MaskInfo.Length;

			// Update bitmap info to show the new scan line size
			TempBitmapInfo->bmiHeader.biWidth = RegionWidth;
			TempBitmapInfo->bmiHeader.biSizeImage = (RegionWidth*BytesPerPixel);

			// copy the scan line into the temporary bmp
			if (SepTables == NULL)
			{
				// We are not colour separating, so just do a quick memcpy
				memcpy(DestBuffer, SrcBuffer, RegionWidth*BytesPerPixel);
			}
			else
			{
				// We are doing some sort of colour separation, so copy the data across, colour
				// separating it as we go
				ERROR3IF(BytesPerPixel != 3, _T("non-24bpp scanline unsupported"));
				ColourSeparateScanline24(DestBuffer, SrcBuffer, RegionWidth);
			}

			// Work out the coords of the destination rectangle
			INT32 DestX = Origin.x + INT32(ceil(MaskInfo.x*Ratio));
			INT32 DestY = Origin.y;

			// Y direction is different for our EPS and GDI PostScript, as GDI preserves
			// the Y +ve <=> downwards model of windows.
			if (IsCamelotEPS)
				DestY += INT32(ceil((MaskInfo.y+1)*Ratio));
			else
				DestY -= INT32(ceil((MaskInfo.y+1)*Ratio));

			INT32 DestWidth = INT32(ceil(RegionWidth*Ratio));
			INT32 DestHeight = INT32(ceil(1*Ratio));

			pDC->OutputToken(_T("sv"));
			pDC->OutputValue(RegionWidth);
			pDC->OutputToken(_T("1"));


			if (IsCamelotEPS)
			{
				// Convert to points.
				pDC->OutputUserSpaceValue(DestWidth);
				pDC->OutputUserSpaceValue(DestHeight);

				DocCoord Dest(DestX, DestY);
				pDC->OutputCoord(Dest);
			}
			else
			{
				// Pass straight through
				pDC->OutputValue(DestWidth);
				pDC->OutputValue(DestHeight);
				pDC->OutputValue(DestX);
				pDC->OutputValue(DestY);
			}

			// Tell the proc whether this is L1 or L2 compatible data
			if (UseLevel2)
				pDC->OutputToken(_T("2"));
			else
				pDC->OutputToken(_T("1"));

			// Rendering primitive.
			if (SepTables == NULL)
				pDC->OutputToken(_T("cbsl"));		// 24-bpp colour scanline
			else
				pDC->OutputToken(_T("gbsl"));		// 8-bpp greyscale scanline


			// Output bitmap data - format depends on whether we can do Level 2 or not
			if (UseLevel2)
			{
				// Separate into 3 RGB streams (because it is much more likely to compress well
				// with RLE if we compress each component individually), and send as RLE
				// compressed ASCII85 data.

				// If we're colour separating, then it's greyscale (R=G=B) data, so we just dump
				// only the first channel to the gbsl operator
				const INT32 NumStreams = (SepTables == NULL) ? 3 : 1;

				for (INT32 i = 0; i < NumStreams; i++)
				{
					ERROR3IF(RegionWidth > Width, _T("I didn't allocate enough memory!"));

					// Copy this component into the buffer
					for (INT32 j = 0; j < RegionWidth; j++)
					{
						pRGBBuf[j] = TempBitmapBytes[(j * 3) + i];
					}

					// We are going to do ASCII85, so get ready for it.
					if (!pDC->StartASCII85Output(TRUE))  // TRUE => RLE please
						// Error
						return SLOWJOB_FAILURE;

					// Send to output stream as RLE encoded ASCII85 data.
					/*INT32 nBytes =*/ pDC->OutputASCII85(pRGBBuf, RegionWidth);

					if (!pDC->EndASCII85Output())
						// Error
						return SLOWJOB_FAILURE;
				}
			}
			else
			{
				// Just output as L1 ASCIIHex encoded data
				if (SepTables != NULL)
				{
					// Colour separating, so reduce the 24-bit (R=G=B) greyscale to 8bpp by getting the _T("red") channel
					for (INT32 j = 0; j < RegionWidth; j++)
						pRGBBuf[j] = TempBitmapBytes[j * 3];
					pDC->OutputRawBinary(pRGBBuf, RegionWidth, 1);
				}
				else
				{
					// Normal 24bpp colour output
					pDC->OutputRawBinary(TempBitmapBytes, RegionWidth * 3, 1);
				}
			}

			pDC->OutputToken(_T("rs"));
			pDC->OutputNewLine();

			// Update the progress display if necessary.
			if ((Progress!=NULL) && (!Progress->BitmapPhaseBandRenderedTo(MaskInfo.y)))
			{
				// Free up the tempory DIB I made
				FreeDIB(TempBitmapInfo, TempBitmapBytes);

				return SLOWJOB_USERABORT;
			}

			// Find the next bit of scan line to plot
			pMask->GetNextMaskRegion(&MaskInfo);
		}
	}
	catch(CFileException)
	{
		FreeDIB(TempBitmapInfo, TempBitmapBytes);

		if (UseLevel2)
			CCFree(pRGBBuf);

		return SLOWJOB_FAILURE;
	}


	// Update bitmap info to what it was before we started
	TempBitmapInfo->bmiHeader.biSizeImage = (Width*3);
    TempBitmapInfo->bmiHeader.biWidth = Width;

	// Free up the tempory DIB I made
	FreeDIB(TempBitmapInfo, TempBitmapBytes);

	// Free up the buffer used for separating RGB components
	CCFree(pRGBBuf);
#endif

	// All ok
	return SLOWJOB_SUCCESS;
}



/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/95
	Inputs:		ch      - unicode value of char
				pMatrix - matrix specifying transforms to place char correctly in document
	Returns:	FALSE if fails
	Purpose:	Render a character,
				using the specified transform and current attributes in the render region.

********************************************************************************************/

BOOL CamelotEPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
#ifndef STANDALONE

	// Should we render it as curves? (We do for non-ASCII characters)
	if ((ch > 255) || EPSFilter::XSEPSExportTextAsCurves)
		return RenderRegion::RenderChar(ch, pMatrix);

	// Make sure we have the right attributes for rendering as paths.
	GetValidPathAttributes();

	// get overall matrix - attribute matrix concatenated with given matrix if supplied
	Matrix matrix;
	if (GetCharAttributeMatrix(&matrix)==FALSE)
		return FALSE;
	if (pMatrix)
		matrix*=*pMatrix;

	// Can we do this using a GDI font?
	// We can if the matrix only specifies scaling and translation, with no aspect ratio.
	FIXED16 abcd[4];
	INT32	ef[2];
	matrix.GetComponents(abcd, ef);

	if ((abcd[1] == FIXED16(0)) && (abcd[2] == FIXED16(0)) && (abcd[0] == abcd[3]))
	{
		// Simple transformation - we can do this with an untransformed PostScript font

		// Work out required height of the font
		MILLIPOINT ReferenceSize = TextManager::GetDefaultHeight();
		MILLIPOINT Height = ReferenceSize * abcd[3];

		if (!SelectNewFont(RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC(), Height))
		{
			// Could not select font
			return FALSE;
		}
	}
	else
	{
		// Transformation is complex - we need to use a matrix when selecting the font.
		if (!SelectNewFont(RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC(), abcd))
		{
			// Could not select font
			return FALSE;
		}
	}

	// Render the character in the specified position...
	KernelDC *pKernelDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

	// Output character

	// Graham 5/8/96: Changed this to work with MBCS
	// Set up a string buffer for the output character
	TCHAR cBuffer[3];

	//Convert the character over to a MBCS index number
	UINT32 uiCharNumber = UnicodeManager::UnicodeToMultiByte(ch);

	//DecomposeMultiBytes takes that index number and creates a
	//two-byte character from it. We put that two-byte character
	//in cBuffer.
	UnicodeManager::DecomposeMultiBytes(uiCharNumber, (BYTE*) &cBuffer[0], (BYTE*) &cBuffer[1]);
	
	//And let's null-terminate cBuffer
	cBuffer[2]=0;

	//Now, if the character was a standard one-byte ASCII character, the ASCII
	//value will be in the second byte of cBuffer - cBuffer[1] - and cBuffer[0]
	//will be zero.

	//Note that we have set cBuffer[2] to zero. That means that cBuffer is null
	//terminated whether it contains a one or a two byte character.

	//So let's check if we've got a one-byte or a two-byte character. We do this
	//by saying...is cBuffer[0] zero?

	if (cBuffer[0]==0)
		//It's a standard ASCII character, one byte long.
		//So we only want to pass the second byte of cBuffer - cBuffer[1] -
		//to OutputString
		pKernelDC->OutputString(&cBuffer[1]);
	else
		//The character is two bytes long (that is, it's a foreign character).
		//So we want to pass the whole of cBuffer - starting from cBuffer[0] -
		//to OutputString
		pKernelDC->OutputString(&cBuffer[0]);

	//And that's the end of Graham's MBCS code. Now back to Tim.

	// Output position
	DocCoord DocPos(ef[0], ef[1]);
	pKernelDC->OutputCoord(DocPos);

	// Work out how to render the character..
	BOOL FlatFill = IS_A(CurrentAttrs[ATTR_FILLGEOMETRY].pAttr, FlatFillAttribute);
	BOOL Stroked = !RR_STROKECOLOUR().IsTransparent();

	// The character is filled if the fill is not a simple flat fill, or if it is,
	// but the fill colour is not transparent.
	BOOL Filled = !FlatFill || (!RR_FILLCOLOUR().IsTransparent());

	if (FlatFill && Filled && !Stroked)
	{
		// Simple flat filled text with no outline, so render normally.
		pKernelDC->OutputToken(_T("t"));
	}
	else
	{
		// Need to do something special...
		if (Stroked)
		{
			if (Filled)
				// Stroke and fill it
				pKernelDC->OutputToken(_T("tb"));
			else
				// Just stroke it
				pKernelDC->OutputToken(_T("ts"));
		}
		else
		{
			// Just fill it
			pKernelDC->OutputToken(_T("tf"));
		}
	}

#endif

	return TRUE;
}

/********************************************************************************************

>	virtual void CamelotEPSRenderRegion::ConditionalSuicide ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/00
	Inputs:		-
	Returns:	-
	Purpose:	Doesn't delete the object when invoked. This is to get around using a few
				if IS_A calls elsewhere in Camelot.

********************************************************************************************/

void CamelotEPSRenderRegion::ConditionalSuicide ( void )
{
	// Don't delete this!
}

/********************************************************************************************

>	virtual BOOL CamelotEPSRenderRegion::NeedsPrintComponents ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/00
	Inputs:		-
	Returns:	TRUE - the print components are needed.
	Purpose:	Lets the print component class know that they are needed in this format's
				export.

********************************************************************************************/

BOOL CamelotEPSRenderRegion::NeedsPrintComponents ( void )
{
	// We want print components!
	return TRUE;
}

/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::SelectNewFont ( WORD		Typeface,
												 BOOL		Bold,
												 BOOL		Italic,
												 MILLIPOINT	Size )

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/95
	Inputs:		Typeface	- The font's ID.
				Bold		- Is it bold?
				Italic		- Is it italic?
				Size		- The font size in millipoints.
	Returns:	TRUE		- Success.
				FALSE		- Failure / an error occured.
	Purpose:	Get the font name being used by the Camelot EPS exporter.

********************************************************************************************/

BOOL CamelotEPSRenderRegion::SelectNewFont ( WORD		Typeface,
											 BOOL		Bold,
											 BOOL		Italic,
											 MILLIPOINT	Size )
{
#ifndef STANDALONE
	// BODGE: This is mingled with horrible OIL code that should be moved to the OIL RSN...

	// Check to see if it is cached
	if ((FontInfo.Valid && !FontInfo.Complex) &&
		(FontInfo.Typeface == Typeface) &&
		(FontInfo.Bold == Bold) &&
		(FontInfo.Italic == Italic) &&
		(FontInfo.Size == Size))
	{
		// It is the same as the currently selected/cached font
		TRACEUSER( "Tim", _T("Using the cached font\n"));
		return TRUE;
	}

	// Cache is invalid - map the TrueType name to a PostScript font.

	// Get the typeface name of the font
	String_64 FontName;
	if (!FONTMANAGER->GetFontName(Typeface, FontName))
		return FALSE;

	// Transform to a INI-file friendly format
	String_64 Encoded;
	INT32 Styles = 0;
	if (Bold)
		Styles += 1;
	if (Italic)
		Styles += 2;
	FONTMANAGER->EncodeFontName(FontName, Encoded, Styles);

	// Look up font in INI file.
	String_256 MappedFontName;
	if (!Camelot.GetPrefDirect(_T("EPSFontMapping"), (TCHAR *) Encoded, &MappedFontName))
	{
		// No preference - map to Times-Roman variant
		if (Bold)
		{
			if (Italic)
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAMEBI));
			else
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAMEB));
		}
		else
		{
			if (Italic)
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAMEI));
			else
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAME));
		}			
	}

	// Due to inaccuracies, point sizes like 16pt come out as 15.999pt, so we check
	// for trailing digit errors and round to nearest integer point size if necessary.
	// (NB we have to use another variable for this otherwise the cache values don't match)
	MILLIPOINT CorrectedSize = Size;
	MILLIPOINT Remainder = Size % 1000;
	if ((Remainder <= 2) || (Remainder >= 998))
		// Lose the inaccuracy.
		CorrectedSize = ((Size + 500) / 1000) * 1000;

	// Select this font by prefixing with a forward slash to make it into a name.
	KernelDC *pKernelDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	String_8 Slash(TEXT("/"));
	MappedFontName.Insert(Slash, 0);
	pKernelDC->OutputToken((TCHAR *) MappedFontName);
	pKernelDC->OutputUserSpaceValue(CorrectedSize);
	pKernelDC->OutputToken(_T("sf"));

	// Font cache is now valid
	FontInfo.Valid 	  = TRUE;
	FontInfo.Complex  = FALSE;
	FontInfo.Typeface = Typeface;
	FontInfo.Bold 	  = Bold;
	FontInfo.Italic   = Italic;
	FontInfo.Size 	  = Size;
	
#endif

	// All ok
	return TRUE;	
}



BOOL CamelotEPSRenderRegion::SelectNewFont(WORD Typeface, BOOL Bold, BOOL Italic, FIXED16 *abcd)
{
#ifndef STANDALONE

	// BODGE: This is mingled with horrible OIL code that should be moved to the OIL RSN...

	// Check to see if it is cached
	if ((FontInfo.Valid && FontInfo.Complex) &&
		(FontInfo.Typeface == Typeface) &&
		(FontInfo.Bold == Bold) &&
		(FontInfo.Italic == Italic) &&
		(FontInfo.a == abcd[0]) &&
		(FontInfo.b == abcd[1]) &&
		(FontInfo.c == abcd[2]) &&
		(FontInfo.d == abcd[3]))
	{
		// It is the same as the currently selected/cached font
		TRACEUSER( "Tim", _T("Using the cached font\n"));
		return TRUE;
	}

	// Cache is invalid - map the TrueType name to a PostScript font.

	// Get the typeface name of the font
	//	ENUMLOGFONT *pEnumLogFont = TextInfoBarOp::GetCachedLogFont(Typeface);
	//	String_64 FontName(pEnumLogFont->elfLogFont.lfFaceName);
	// Changed by Mike (2/8/95)
	String_64 FontName;
	if (!FONTMANAGER->GetFontName(Typeface, FontName))
		return FALSE;

	// Transform to a INI-file friendly format
	String_64 Encoded;
	INT32 Styles = 0;
	if (Bold)
		Styles += 1;
	if (Italic)
		Styles += 2;
	FONTMANAGER->EncodeFontName(FontName, Encoded, Styles);

	// Look up font in INI file.
	String_256 MappedFontName;
	if (!Camelot.GetPrefDirect(_T("EPSFontMapping"), (TCHAR *) Encoded, &MappedFontName))
	{
		// No preference - map to Times-Roman variant
		if (Bold)
		{
			if (Italic)
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAMEBI2));
			else
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAMEB2));
		}
		else
		{
			if (Italic)
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAMEI2));
			else
				MappedFontName.Load(_R(IDS_K_CAMEPS_MAPFNAME2));
		}			
	}

	// Select this font by prefixing with a forward slash to make it into a name.
	KernelDC *pKernelDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	String_8 Slash(TEXT("/"));
	MappedFontName.Insert(Slash, 0);
	pKernelDC->OutputToken((TCHAR *) MappedFontName);

	// Get the default text height, and work out how to scale to PostScript's default of
	// 1 user space unit (i.e. 1 pt)
	double ScaleFactor = TextManager::GetDefaultHeight() / 1000.0;
	TCHAR MatrixBuf[100];
	camSprintf(MatrixBuf, _T("[%.3f %.3f %.3f %.3f 0 0]"), 
		    (double) (abcd[0].MakeDouble() * ScaleFactor),
		    (double) (abcd[1].MakeDouble() * ScaleFactor),
		    (double) (abcd[2].MakeDouble() * ScaleFactor),
		    (double) (abcd[3].MakeDouble() * ScaleFactor));

	pKernelDC->OutputToken(MatrixBuf);
	
	// Select the complex font
	pKernelDC->OutputToken(_T("sf"));

	// Font cache is now valid
	FontInfo.Valid 	  = TRUE;
	FontInfo.Complex  = TRUE;
	FontInfo.Typeface = Typeface;
	FontInfo.Bold 	  = Bold;
	FontInfo.Italic   = Italic;
	FontInfo.a = abcd[0];
	FontInfo.b = abcd[1];
	FontInfo.c = abcd[2];
	FontInfo.d = abcd[3];

#endif
	
	// All ok
	return TRUE;	
}


/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap))

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/6/95
	Inputs:		pNodeBitmap - the bitmap node to render!
	Returns:	TRUE if the bitmap could be rendered as specified;
				FALSE if it could not be (and hence wasn't).
	Purpose:	Plot an arbitrarily transformed bitmap using a matrix and an offset.
				The bitmap is assumed to occupy a rectangle at the origin, which has pixels 1 
				point across, and the matrix supplied transforms this area to the correct 
				place.  The Point parameter supplies an optional offset which can be applied 
				after the transform.
				If the offset is 0,0, this means there is no offset (surprise, surprise!)
	SeeAlso:	NodeBitmap::Render

********************************************************************************************/

BOOL CamelotEPSRenderRegion::DrawTransformedBitmap(NodeBitmap *pNodeBitmap)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return TRUE;

	if ((!RenderComplexShapes) && (pNodeBitmap->NeedsTransparency()))
		return TRUE;

	DocCoord *Coords = pNodeBitmap->InkPath.GetCoordArray();

	// Get the 'actual' OIL Bitmap
	OILBitmap *TheBitmap = pNodeBitmap->GetBitmap()->ActualBitmap;

	// Is it contoned?
	DocColour *StartCol = NULL, 
			  *EndCol =   NULL;

	if (!RR_STROKECOLOUR().IsTransparent())
	{
		// It has a line colour, so it is contoned!
		StartCol = &(RR_STROKECOLOUR());
		EndCol   = &(RR_FILLCOLOUR());
	}

	// Render it
	return DrawParallelogramBitmap(Coords, TheBitmap, GetFillEffect(), StartCol, EndCol);
}


/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::DrawParallelogramBitmap(DocCoord *Coords, 
														 OILBitmap *pBitmap,
														 EFFECTTYPE Effect = EFFECT_RGB,
													 	 DocColour *StartCol = NULL, 
													 	 DocColour *EndCol = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/95
	Inputs:		Coords - the parallelogram to plot the bitmap into (should be an array of
						 4 co-ordinates)
				pBitmap - the bitmap to render!
				Effect - if contone colours are supplied, this is how the colours should fade.
				StartCol, EndCol - optional colours, if non-NULL, this indicates that the
								   bitmap is a contoned bitmap and should be rendered
								   using these colours.
	Returns:	TRUE if the bitmap could be rendered as specified;
				FALSE if it could not be (and hence wasn't).
	Purpose:	Plot an arbitrarily transformed bitmap in the position specified.

				For a normal untransformed rectangle, the Coords array should start specify
				the parallelogram as follows:

					Coords[0]		Coords[1]
					   +---------------+
					   |               |
					   |               |
					   |               |
					   |               |
					   +---------------+
					Coords[3]		Coords[2]

	SeeAlso:	NodeBitmap::Render; CamelotEPSRenderRegion::DrawTransformedBitmap

********************************************************************************************/

BOOL CamelotEPSRenderRegion::DrawParallelogramBitmap(DocCoord *Coords, OILBitmap *pBitmap,
													 EFFECTTYPE Effect,
													 DocColour *StartCol, DocColour *EndCol)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return TRUE;

	// If it's contoned between two no-colours, it is invisible (on this plate) so don't render anything
	if (StartCol != NULL && StartCol->IsTransparent() && EndCol != NULL && EndCol->IsTransparent())
		return(TRUE);

	// Get information about the bitmap to be rendered.
	BitmapInfo BMInfo;
	if (!pBitmap->GetInfo(&BMInfo))
	{
		ERROR3("Bitmap GetInfo failed");
		// Error getting bitmap info
		return FALSE;
	}

	// Remember the Size of the Bitmap (in pixels)
	INT32 Width  = BMInfo.PixelWidth;
	INT32 Height = BMInfo.PixelHeight;

	if ((Width == 0) || (Height == 0))
	{
		// Error - bitmap has no dimension
		ERROR3("Bitmap width/height is 0");
		return FALSE;
	}

	// Find out if this is real renderable EPS.
	BOOL IsCamelotEPS = IS_A(this, CamelotEPSRenderRegion);

	// Is it contoned?
	BOOL IsContoned = (StartCol != NULL) && (EndCol != NULL);
	if (IsContoned)
	{
		if (pBitmap->GetGreyscaleVersion() != NULL)
		{
			pBitmap = pBitmap->GetGreyscaleVersion();
			if (!pBitmap->GetInfo(&BMInfo))
			{
				ERROR3("Bitmap GetInfo failed");
				// Error getting bitmap info
				return FALSE;
			}
		}
	}

	// Find out if we can use Level 2 features.
	BOOL UseLevel2 = FALSE;

	if (IsCamelotEPS)
	{
		// Use the EPS preference.
		if (EPSFilter::XSEPSExportPSType == 2)
			UseLevel2 = TRUE;
	}
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
	else if (IsPrinting())
	{
#ifndef STANDALONE
		// Get the print info for this job.
		CCPrintInfo *pInfo = CCPrintInfo::GetCurrent();
		if (pInfo != NULL)
		{
			PrintControl *pPrCtrl = pInfo->GetPrintControl();
			if (pPrCtrl->GetPSLevel() == PSLEVEL_2)
				// Printing to a level 2 device - party on!
				UseLevel2 = TRUE;
		}
#else
	ERROR2(FALSE,"CamelotEPSRenderRegion::DrawParallelogramBitmap trying to print in Viewer version!")
#endif
	}

#endif //webster

	// Work out the coords of the destination rectangle
//	INT32 DestX = 0;
//	INT32 DestY = 0;

	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

	pDC->OutputToken(_T("sv"));

	// Dimensions of bitmap
	pDC->OutputValue(Width);
	pDC->OutputValue(Height);

	// Number of bits per component sample
	INT32 BytesPerScanline;

	// Are we colour separating? Get SepTables if we need them
	// NOTE that we get them even if we have a <= 8bpp bitmap, as the pointer is also used 
	// as a "we are separating" flag. Note also that we don't try to separate contoned bitmaps
	// via the normal system- we just let the old contone bitmap code output it, and separate the palette
	if (!IsContoned && SepTables == NULL && RenderView->GetColourPlate() != NULL)
	{
		ColourContextCMYK *cc = (ColourContextCMYK *)RenderView->GetColourContext(COLOURMODEL_CMYK);
		if (cc != NULL)
		{
			SepTables = (BYTE *) CCMalloc(5 * 256 * sizeof(BYTE));
			if (SepTables != NULL)
			{
				if (!cc->GetProfileTables(SepTables))
				{
					CCFree(SepTables);
					SepTables = NULL;
				}
			}
		}

		ERROR3IF(SepTables == NULL, "Can't generate separation tables in CamelotEPSRenderRegion");
	}

	if (SepTables != NULL && !IsContoned)
	{
		// ALL colour separated bitmaps go out as simple 8bpp greyscales (yay! no special cases!)
		pDC->OutputValue((INT32)8);
		BytesPerScanline = Width;
	}
	else
	{
		// It's not colour separated, so output the right values for the bitmap's BPP.
		switch (BMInfo.PixelDepth)
		{
			case 1:
			case 8:
				pDC->OutputValue((INT32) BMInfo.PixelDepth);
				BytesPerScanline = ((BMInfo.PixelDepth * Width) + 7) / 8;	// Round 1bpp up to multiple of bytes
				break;

			case 4:
				// 4bpp images get converted to 8bpp because Adobe SDK image library can't
				// do 4bpp on some PS printers (e.g. Canon LBP4-PS).
				pDC->OutputValue((INT32) 8);
				BytesPerScanline = Width;
				break;

			case 24:
			case 32:			// 32bpp images are output as 24bpp to postscript
				// Output will be standard 24bpp
				pDC->OutputValue((INT32) 24);
				BytesPerScanline = 3 * Width;
				break;

			default:
				// Unknown bitmap depth
				ERROR3("Unsupported bitmap depth in CamelotEPSRenderRegion::DrawParallelogramBitmap");
				return FALSE;
		}
	}

	// Now, output how many bytes per scanline will be used.
	pDC->OutputValue(BytesPerScanline);

	// Now output the matrix - colorimage expects the inverse of this matrix, i.e.
	// it needs the matrix which maps the destination area to the 1-1 rectangle at
	// the origin (cos PostScript is weird like that).
	pDC->OutputToken(_T("["));

	// NB. We have to do some 'bespoke' matrix maths here, because we need to work
	// 	   in points, and our Matrix class is not accurate enough (notably the e and 
	//	   f components are integer only, and we need fractional points).
	double a[2],b[2],c[2],d[2], e[2], f[2];

	// Find out the size of one unit, in MILLIPOINTS.
	double UnitSize;
	if (IsCamelotEPS)
	{
		// Unit is 1000 millipoints, as we always use 1 point as unit in user space
		// in Camelot EPS.
		UnitSize = 1000;
	}
	else
	{
		// Find the size of a pixel, because Windows maps a unit to a pixel.
		FIXED16 fxPixelSize;
		RenderView->GetPixelSize(&fxPixelSize, &fxPixelSize);
		UnitSize = fxPixelSize.MakeDouble();
	}

	// Work out the width and height of the bitmap, assuming each pixel corresponds to
	// one unit.
	double dWidth = (double) Width;
	double dHeight = (double) Height;

	DocCoord DCOrigin = pDC->GetOrigin();

	if (IsCamelotEPS)
	{
		dWidth *= UnitSize;
		dHeight *= UnitSize;
		a[0] = ((double) (Coords[2].x - Coords[3].x)) / dWidth;
		b[0] = ((double) (Coords[2].y - Coords[3].y)) / dWidth;
		c[0] = ((double) (Coords[0].x - Coords[3].x)) / dHeight;
		d[0] = ((double) (Coords[0].y - Coords[3].y)) / dHeight;
		e[0] = ((double) (Coords[3].x - DCOrigin.x)) / UnitSize;
		f[0] = ((double) (Coords[3].y - DCOrigin.y)) / UnitSize;
	}
	else
	{
		ERROR3IF(!pDC->IsKindOf(CC_RUNTIME_CLASS(PSPrintDC)), "KernelDC is not a PSPrintDC - how did that happen?");
		// Printing to PostScript - convert to Window co-ordinates, using the DC.
		PSPrintDC *pPSDC = (PSPrintDC *) pDC;

		WinCoord PSCoords[3];
		for (INT32 i = 0; i <= 3; i++)
			PSCoords[i] = pPSDC->TransformCoord(Coords[i]);

		a[0] = ((double) (PSCoords[2].x - PSCoords[3].x)) / dWidth;
		b[0] = ((double) (PSCoords[2].y - PSCoords[3].y)) / dWidth;
		c[0] = ((double) (PSCoords[0].x - PSCoords[3].x)) / dHeight;
		d[0] = ((double) (PSCoords[0].y - PSCoords[3].y)) / dHeight;
		e[0] = (double) PSCoords[3].x;
		f[0] = (double) PSCoords[3].y;
	}

	// Calculate the inverse of the above.
	double Det = a[0] * d[0] - b[0] * c[0];

	if (Det==0.0)
	{
		// There is no inversion of this matrix
		// return the identity matrix
		ERROR3("Matrix Inversion Failed - Tried to Invert a non-invertable matrix" );
		return FALSE;
	}

	// this section calculates the inverse of the matrix. As it takes into
	// account that our 3x3 matrix always has 0,0,1 down its right hand side
	// it has been greatly simplified. The operations combine the calculation
	// of the adjoint matrix and scaling it by the Determinent with a matrix
	// transpose (the inverse is the transpose of the adjoint scaled by the 
	// determinent)
	a[1] =  d[0] / Det;
	b[1] = -b[0] / Det;
	c[1] = -c[0] / Det;
	d[1] =  a[0] / Det;
	e[1] =   ((c[0] * f[0]) - (e[0] * d[0])) / Det ;
	f[1] = -(((a[0] * f[0]) - (e[0] * b[0])) / Det);

	pDC->OutputReal(a[1]);
	pDC->OutputReal(b[1]);
	pDC->OutputReal(c[1]);
	pDC->OutputReal(d[1]);

	if (IsCamelotEPS)
	{
		// Convert to points.
		pDC->OutputUserSpaceValue((MILLIPOINT) (e[1] * 1000.0));
		pDC->OutputUserSpaceValue((MILLIPOINT) (f[1] * 1000.0));
	}
	else
	{
		// Pass straight through
		pDC->OutputReal(e[1]);
		pDC->OutputReal(f[1]);
	}

	pDC->OutputToken(_T("]"));

	// Default values for smoothing and polarity
	pDC->OutputToken(_T("false"));
	pDC->OutputToken(_T("false"));

	// Normally we use ASCII Hex encoding, but for Level 2 specific output, we use ASCII85.
	if (UseLevel2)
		// Run length encoded and ASCII85 encoded
		pDC->OutputToken(_T("3"));
	else
		// Simple level 1 compatible ASCII Hex encoding.
		pDC->OutputToken(_T("1"));

	// Set up the image procs
	pDC->OutputToken(_T("beginimage"));

	if (SepTables != NULL && !IsContoned)
	{
		// If we are colour-separating, then output always goes out as an 8bpp greyscale image
		// This may seem like a gross thing to do (especially for 1/4bpp), but as 4bpp is always
		// expanded to 8bpp anyway, it is much better to treat all images in a nice generic manner
		// and save ourselves lots of work, lots of confusion, and the potential for lots of
		// special cases which don't work (for examples of code like this, see the old composite
		// printing code below).
		if (!pBitmap->ExportSeparatedPalette(this))
		{
			ERROR3("ExportSeparatedPalette failed");
			return FALSE;
		}

		pDC->OutputToken(_T("doclutimage"));		// Colour separation to 8bpp greyscale image

		pDC->OutputNewLine();

		// Output bitmap (scanline) data as an 8bpp greyscale
		if (!pBitmap->ExportSeparatedData(this, SepTables))
		{
			ERROR3("ExportSeparatedData failed");
			return FALSE;
		}
	}
	else
	{
		// We've specified the size, depth and position of the bitmap, so now we work 
		// out which PostScript procedure we should render the image with.
		switch (BMInfo.PixelDepth)
		{
			case 1:
				// It's a 1bpp image - should we output a palette with it?
				if (IsContoned)
				{
					// (Jason here: I believe the postscript routine 1bitcopyimage doesn't work.
					// This is probably OK, as I think all contoned bitmaps in camelot are made 8bpp.
					// I've left this code in place, however, just in case it would break something)

					// Convert colours to RGB and output for use with the special
					// 1bpp contone operator.
					Document *pDoc = RenderView->GetDoc();
					if (pDoc == NULL)
					{
						ERROR3("No document when rendering");
						// Panic!
						return FALSE;
					}

					// Use the output context to convert the colours, and then output them...
					ColourContext *pContext = CurrentColContext;
					if (pContext == NULL)
					{
						ERROR3("No Colour context when rendering");
						return(FALSE);		// Panic!
					}

					ERROR3IF(pContext->GetColourModel() != COLOURMODEL_RGBT,
								"RGB output context expected in EPS rendering");
					ColourRGBT Result;

					if (IsContoned)
						pContext->ConvertColour((DocColour *) StartCol, (ColourGeneric *)&Result);
					else
					{
						DocColour Black(0L, 0L, 0L);
						pContext->ConvertColour(&Black, (ColourGeneric *)&Result);
					}

					pDC->OutputReal(Result.Red.MakeDouble());
					pDC->OutputReal(Result.Green.MakeDouble());
					pDC->OutputReal(Result.Blue.MakeDouble());

					if (IsContoned)
						pContext->ConvertColour((DocColour *) EndCol, (ColourGeneric *)&Result);
					else
					{
						DocColour White(255L, 255L, 255L);
						pContext->ConvertColour(&White, (ColourGeneric *)&Result);
					}

					pDC->OutputReal(Result.Red.MakeDouble());
					pDC->OutputReal(Result.Green.MakeDouble());
					pDC->OutputReal(Result.Blue.MakeDouble());

					// Use special colour 1bpp operator
					pDC->OutputToken(_T("1bitcopyimage"));
				}
				else
				{
					// Normal black and white image
					pDC->OutputToken(_T("1bitbwcopyimage"));
				}
				break;

			case 4:
			case 8:
				if (IsContoned)
				{
					// Contoned bitmap - generate and output the contone palette.
					if (!pBitmap->BuildContonePalette(*StartCol, *EndCol, Effect, RenderView))
					{
						ERROR3("Contoned palette failed");
						// Error while building palette (out of memory?)
						return FALSE;
					}

					BOOL Success = pBitmap->ExportContonePalette(this);

					// Clean up contone palette
					pBitmap->DestroyContonePalette();
				
					// Did it work?
					if (!Success)
					{
						ERROR3("Contoned palette export failed");
						return FALSE;
					}

					// Use the procedure for rendering paletted images.
					pDC->OutputToken(_T("doclutimage"));
				}
				else if (BMInfo.NumPaletteEntries > 0)
				{
					// It's a paletted image - export the palette
					if (!pBitmap->ExportBitmapPalette(this))
					{
						ERROR3("ExportBitmapPalette failed");
						return FALSE;
					}

					// Use the procedure for rendering paletted images.
					pDC->OutputToken(_T("doclutimage"));
				}
				else
				{
					// It's a greyscale image - use the image operator.
					// (Jason here: I reckon the postscript for doimage doesn't work. This is
					// probably OK, because camelot can't theoretically have a non-paletted image?)
					pDC->OutputToken(_T("doimage"));
				}
				break;

			case 24:
			case 32:									// 32bpp is output as 24bpp data
				pDC->OutputToken(_T("do24image"));			// Normal composite colour 24bpp image
				break;

			default:
				// Unknown bitmap depth
				ERROR3("Unsupported bitmap depth");
				return FALSE;
		}

		pDC->OutputNewLine();

		// Output bitmap (scanline) data
		if (!pBitmap->ExportBitmapData(this))
		{
			ERROR3("ExportBitmapData failed");
			return FALSE;
		}
	}

	// Clean up image stuff
	pDC->OutputNewLine();
	pDC->OutputToken(_T("endimage"));

	pDC->OutputToken(_T("rs"));
	pDC->OutputNewLine();

	return TRUE;
}


/********************************************************************************************

>	void CamelotEPSRenderRegion::DrawPathToOutputDevice(Path *DrawPath)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		DrawPath - the path to render.
	Purpose:	Output all the commands required to render this path to the EPS file.
	SeeAlso:	RenderRegion::DrawPath; EPSRenderRegion::GetValidPathAttributes
				EPSRenderRegion::ExportPath

********************************************************************************************/

void CamelotEPSRenderRegion::DrawPathToOutputDevice(Path *DrawPath, PathShape)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return;

	// Work out if we need to be renderable.
	BOOL Renderable = (IS_A(this, CamelotEPSRenderRegion) || IsPrinting());

	if (Renderable)
	{
		// Check for clipped (non-repeating) bitmaps...

		// Get the current fill attr
		ColourFillAttribute* pFillAttr = (ColourFillAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

		// See if it is a simple non-repeating bitmap fill...
		if (pFillAttr->IS_KIND_OF(BitmapFillAttribute))
		{
			FillMappingAttribute* pMapAttr = (FillMappingAttribute*) CurrentAttrs[ATTR_FILLMAPPING].pAttr;
			if (pMapAttr->Repeat == 1)
			{
				// Simple non-repeating clipped bitmap - we can do this!
				DrawClippedBitmap(DrawPath);
				return;
			}
		}
	}

	ExportPath(DrawPath, FALSE);

	// Now do the arrow heads if the render region can't do them directly
	if (!Caps.ArrowHeads)
	{
		// Doesn't support arrow heads directly so we render them as paths.
		DrawPathArrowHeads(DrawPath->GetCoordArray(),
						   DrawPath->GetVerbArray(),
						   DrawPath->GetNumCoords());
	}
}


/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::DrawClippedBitmap(Path *DrawPath)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/95
	Inputs:		DrawPath - the path to use to clip the bitmap fill.
	Returns:	TRUE if rendered ok;	
				FALSE if not.
	Purpose:	Render a clipped bitmap using the given path.  This is because we can do
				clipped bitmap rendering in PostScript, and it is much cheaper to do it
				via PostScript instead of via the 'transparency' mask (via GDraw).

********************************************************************************************/

BOOL CamelotEPSRenderRegion::DrawClippedBitmap(Path *DrawPath)
{
	// Get a device context
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

	// What we do is: export the path, save the graphics context, use the path as a clipping 
	// region, render the bitmap, restore the graphics context, and optionally stroke the
	// path, if necessary.

	// make sure the eps-rr is set up with the correct path attributes,
	// as ExportPath() will only be rendering pure path information.
	GetValidPathAttributes();

	// export our path.
	ExportPath(DrawPath, TRUE, TRUE);

	// Save the context
	pDC->OutputToken(_T("gs"));

	// export a 'clip' command. note that if our current winding rule is for even-odd
	// winding, we must export an 'eoclip' command instead.
	if (RR_WINDINGRULE() == EvenOddWinding)
		pDC->OutputToken(TEXT("eoclip"));
	else
		pDC->OutputToken(TEXT("clip"));

	// Render the bitmap - first, set up the parallelogram to render into.
	BitmapFillAttribute* pFillAttr = (BitmapFillAttribute*) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	DocCoord Coords[4];
	Coords[3] = *(pFillAttr->GetStartPoint());
	Coords[2] = *(pFillAttr->GetEndPoint());
	Coords[0] = *(pFillAttr->GetEndPoint2());

	// Deduce the other co-ordinate (not used by DrawParalleogramBitmap(), but calculate
	// it just in case...)
	Coords[1].x = Coords[0].x + (Coords[2].x - Coords[3].x);
	Coords[1].y = Coords[0].y + (Coords[2].y - Coords[3].y);

	// Get the bitmap
	KernelBitmap *pKernelBitmap = pFillAttr->GetBitmap();
	if (pKernelBitmap == NULL)
		return FALSE;

	// Render it.
	if (!DrawParallelogramBitmap(Coords, pKernelBitmap->ActualBitmap, GetFillEffect(),
								 pFillAttr->GetStartColour(), pFillAttr->GetEndColour()))
		return FALSE;

	// Restore the context
	pDC->OutputToken(_T("gr"));

	// Work out if we should stroke the path
	TCHAR PathType[2] = _T("N");

	if (!RR_STROKECOLOUR().IsTransparent())
	{
		// Stroke path, leaving it open
		PathType[0] = 'S';
	}

	PathVerb* Verbs = DrawPath->GetVerbArray();
	INT32 NumCoords = DrawPath->GetNumCoords();

	if (Verbs[NumCoords - 1] & PT_CLOSEFIGURE)
		// Path should be closed
		PathType[0] = tolower(PathType[0]);

	// Do the stroke command
	pDC->OutputToken(PathType);
	pDC->OutputNewLine();

	// Ok if we got to here
	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotEPSRenderRegion::WriteNewLine ( void )

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/11/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- Failure.
	Purpose:	Outputs a new line tag to the file.

********************************************************************************************/

BOOL CamelotEPSRenderRegion::WriteNewLine ( void )
{
	// Since the positions of the characters are explicitly given in the EPS file, newline
	//	(/r) characters are not required. And they 
	return TRUE;
}


/********************************************************************************************

>	virtual void CamelotEPSRenderRegion::OutputStrokeColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/00
	Inputs:		-
	Returns:	-
	Purpose:	This simply calls EPSRenderRegion::OutputStrokeColour, as otherwise the 
				ArtWorks version is used (which can't cope with the RGB colour tokens)
	SeeAlso:	EPSRenderRegion::OutputStrokeColour

********************************************************************************************/

void CamelotEPSRenderRegion::OutputStrokeColour ()
{
	EPSRenderRegion::OutputStrokeColour ();
}


/********************************************************************************************

>	virtual void CamelotEPSRenderRegion::OutputFillColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/00
	Inputs:		-
	Returns:	-
	Purpose:	This simply calls EPSRenderRegion::OutputFillColour, otherwise this render
				region uses the overridden version found in ArtWorksEPSRenderRegion.
	SeeAlso:	EPSRenderRegion::OutputStrokeColour

********************************************************************************************/

void CamelotEPSRenderRegion::OutputFillColour ()
{
	EPSRenderRegion::OutputFillColour ();
}

#endif
