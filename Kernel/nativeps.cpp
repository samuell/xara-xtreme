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
// The Native file format for Camelot first version.

/*
*/

#include "camtypes.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "nativeps.h"
//#include "filtrres.h"
//#include "rik.h"
//#include "tim.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cversion.h"
#include "progress.h"
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "bmpcomp.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opgrad.h"
#include "nodetext.h"
//#include "nev.h"
#include "zstream.h"	// zipping stream class
#include <strstream>
#include "fontman.h"
#include "expbmp.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "colormgr.h"	// for ColourManager (see NativeRenderRegion::OutputGradFillColours)

// An implement to match the Declare in the .h file.
CC_IMPLEMENT_DYNAMIC(CamelotNativeEPSFilter, CamelotEPSFilter);
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
CC_IMPLEMENT_DYNAMIC(NativeRenderRegion, CamelotEPSRenderRegion);
#endif

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

typedef enum
{
	TRANSPFILL_NONE,
	TRANSPFILL_FLAT,
	TRANSPFILL_LINEAR,
	TRANSPFILL_CIRCULAR,
	TRANSPFILL_ELLIPTICAL,
	TRANSPFILL_CONICAL,
	TRANSPFILL_TEXTURE,
	TRANSPFILL_FRACTAL,
	TRANSPFILL_NEWLINEAR,
	TRANSPFILL_NEWTEXTURE,
	TRANSPFILL_NEWFRACTAL
} TranspFillGeometryType;


// This is the array of Camelot EPS command/keyword names.
CommandMap CamelotNativeEPSFilter::NativeCommands[] =
{
	// Bitmap Pool Tokens
	EPSC_cbmp,		"cbmp",

	// Sentinel
	EPSC_Invalid,	"Invalid"
};




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


/********************************************************************************************

	Preference:		CompressNative
	Section:		Filters
	Range:			0 to 1
	Purpose:		Flag for whether we compress the native files saved from Xara Studio.
					True means do compress the files.
	SeeAlso:		-

********************************************************************************************/

BOOL CamelotNativeEPSFilter::CompressNative = TRUE;


/********************************************************************************************

>	CamelotNativeEPSFilter::CamelotNativeEPSFilter()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Purpose:	Constructor for the Native save/load filter.

********************************************************************************************/

CamelotNativeEPSFilter::CamelotNativeEPSFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDS_NATIVE_EPS_FILTERNAME);
	FilterInfoID = _R(IDS_NATIVE_EPS_FILTERINFO);
	ImportMsgID  = _R(IDS_IMPORTMSG_NATIVE_EPS);
	ExportMsgID  = _R(IDS_EXPORTMSG_NATIVE_EPS);

	FilterID = FILTERID_NATIVE_EPS;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	//WEBSTER-Martin-27/01/97
#ifdef WEBSTER
	Flags.CanExport = FALSE;
#else
	Flags.CanExport = FALSE;
#endif //WEBSTER
#else
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#endif

	// We don't want to re-position Camelot files because they should be ok and
	// Charles gets upset if his drawings move.
	AdjustOrigin = FALSE;

	// Prepare the bitmap pool data
	IsSavingBitmapPool = FALSE;
	BitmapCount = 0;
	BitmapPoolRefs = NULL;
	PendingBitmapNum = 0;

	// A flag to store the Value of the ImportWithLayers flag in
	OldImportWithLayers = TRUE;
	OldOpenWithLayers = TRUE;

	// Items to handle the file compression
	CompressionType = 0;	// type of compression in use (0 at present)
	CompressionOn = FALSE;	// on/off flag

	FileVersionNumber = 0.0;
	BuildVersionNumber = 0.0;
}




/********************************************************************************************

>	CamelotNativeEPSFilter::~CamelotNativeEPSFilter()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Purpose:	Tidies up when the filter is destroyed. Mostly takes care of the bitmap pool
				references that may have been built during the import.

********************************************************************************************/

CamelotNativeEPSFilter::~CamelotNativeEPSFilter()
{
	// Get rid of the bitmap references if we have any
	if (BitmapPoolRefs != NULL)
		delete [] BitmapPoolRefs;
}




/********************************************************************************************

>	BOOL CamelotNativeEPSFilter::Init()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Initalises the Filter ready for use. Will fail if it can not get enough
				memory to work with.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new NativeEPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	// Preference to turn native file compression on or off
	if (Camelot.DeclareSection("Filters", 10))
	{
		Camelot.DeclarePref( NULL, "CompressNative", &CompressNative, 0, 1 );
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	static BOOL CamelotNativeEPSFilter::SetNativeCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Inputs:		New state for native file compression, TRUE if want on, FALSE if want off.
	Returns:	The old state of the compression.
	Purpose:	Set the prefernece as to whether we are to compress the native files or not.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::SetNativeCompression(BOOL NewState)
{
	BOOL OldState = CompressNative;
	CompressNative = NewState;
	return CompressNative;
}


/********************************************************************************************

>	static BOOL CamelotNativeEPSFilter::GetNativeCompression(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/95
	Inputs:		New state for native file compression, TRUE if want on, FALSE if want off.
	Returns:	The old state of the compression.
	Purpose:	Set the prefernece as to whether we are to compress the native files or not.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::GetNativeCompression()
{
	return CompressNative;
}	

/********************************************************************************************

>	virtual BOOL CamelotNativeEPSFilter::SetFileCompressionState(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Inputs:		New state for file compression, TRUE if want on, FALSE if want off.
	Returns:	True if succesful or False if not.
	Purpose:	Set a new file compression status into action.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::SetFileCompressionState(BOOL NewState)
{
	TRACEUSER( "Neville", _T("CamelotNativeEPSFilter::SetFileCompressionState new state= %d\n"),NewState);

	CompressionOn = NewState;

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL CamelotNativeEPSFilter::GetFileCompressionState()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/95
	Returns:	The current state of the file compression.
	Purpose:	Get the current file compression state. True if compressing.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::GetFileCompressionState()
{
	return CompressionOn;
}


/********************************************************************************************

>	virtual void CamelotNativeEPSFilter::LookUpToken()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Purpose:	Look up one of the tokens used by the Camelot Native EPS file format.

********************************************************************************************/

void CamelotNativeEPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (NativeCommands[i].Cmd != EPSC_Invalid)
	{
		if (camStrcmp(TokenBuf, NativeCommands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = NativeCommands[i].Cmd;
			return;
		}
		
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	CamelotEPSFilter::LookUpToken();
}


/********************************************************************************************

>	virtual BOOL CamelotNativeEPSFilter::ProcessToken()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Returns:	TRUE if it was able to process the token, FALSE if not.
	Purpose:	Deals with the current token. This function deals with tokens that are specific
				to the Camelot Native EPS format, or different from Camelot EPS.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::ProcessToken()
{
	ArrowRec 	Arrow;
	double 		ArrowWidth;
	double 		ArrowHeight;
	INT32 		ArrowID;

	DashRec 	Dash;
	INT32 		DashID;
	DashPatternAttribute* pDash;

	// Decode the command, and execute it...
	switch (Token)
	{
		// Bitmap Pool Token
		case EPSC_cbmp:
		{
			INT32 PoolType;

			// Find out what type of bitmap pool thing this is (currently 0 is only valid option)
			if ((!Stack.Pop(&PoolType)) || (PoolType!=0))
				goto EPSError;

			// get the bitmap number
			if (!Stack.Pop(&PendingBitmapNum))
				goto EPSError;

			// Next thing will be a pool item
			TRACEUSER( "Rik", _T("Got a Bitmap pool item....\n"));
			PendingBitmap = PENDING_BITMAP_POOLITEM;
			break;
		}

		// Stroke transparency
		case EPSC_cst:
			INT32 TranspType, Transp;

			// Get transparency type...
			if (!Stack.Pop(&TranspType))
				goto EPSError;

			// Get line transparency value
			if (!Stack.Pop(&Transp))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing Line transp, Val=%d, Type=%d\n"),Transp, TranspType);

			if (!SetLineTransp((UINT32)TranspType, (UINT32)Transp))
				goto NoMemory;
			break;
				  			  
		// Start Arrow head
		case EPSC_csah:
			if (!Stack.Pop(&ArrowHeight))
				goto EPSError;

			if (!Stack.Pop(&ArrowWidth))
				goto EPSError;

			if (!Stack.Pop(&ArrowID))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing StartArrow, ID=%d\n"),ArrowID);
			
			if (!Arrow.CreateStockArrow((StockArrow)ArrowID))
				goto NoMemory;

			Arrow.SetArrowSize(FIXED16(ArrowWidth), FIXED16(ArrowHeight));

			if (!SetStartArrow(Arrow))
				goto NoMemory;
			break;

		// End Arrow head
		case EPSC_ceah:
			if (!Stack.Pop(&ArrowHeight))
				goto EPSError;

			if (!Stack.Pop(&ArrowWidth))
				goto EPSError;

			if (!Stack.Pop(&ArrowID))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing EndArrow, ID=%d\n"),ArrowID);
			
			if (!Arrow.CreateStockArrow((StockArrow)ArrowID))
				goto NoMemory;

			Arrow.SetArrowSize(FIXED16(ArrowWidth), FIXED16(ArrowHeight));

			if (!SetEndArrow(Arrow))
				goto NoMemory;
			break;

		// Dash Patterns
		case EPSC_cdp:
			if (!Stack.Pop(&DashID))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing Dash Pattern, ID=%d\n"),DashID);
			
			pDash = new DashPatternAttribute;
			if (pDash == NULL)
				goto NoMemory;

			if (!pDash->SetStockDashPattern((StockDash)DashID))
				goto NoMemory;

			Dash = pDash->DashPattern;
			delete pDash;

			if (!SetDashPattern(Dash))
				goto NoMemory;
			break;

		// Transparency fills.
		case EPSC_cxt:
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			INT32 StartTransp, EndTransp, TranspType;
			INT32 FillType;
			INT32 Tileable;

			// Used for fractal fills
			INT32 Seed;
			double Graininess, Gravity, Squash;
			UINT32 DPI;

			// Get fill type
			if (!Stack.Pop(&FillType))
				goto EPSError;

			TRACEUSER( "Will", _T("Importing Transp Fill, Type=%d\n"),FillType);

			// Read in transparency type and levels...
			if (FillType != TRANSPFILL_NONE)
			{
				// Get transparency type...
				if (!Stack.Pop(&TranspType))
					goto EPSError;

				if ((FillType != TRANSPFILL_TEXTURE) && (FillType != TRANSPFILL_FRACTAL))
				{			
					// Get second transparency level if there is one.
					switch (FillType)
					{
						case TRANSPFILL_LINEAR:
						case TRANSPFILL_ELLIPTICAL:
						case TRANSPFILL_CIRCULAR:
						case TRANSPFILL_CONICAL:
						case TRANSPFILL_NEWLINEAR:
						case TRANSPFILL_NEWTEXTURE:
						case TRANSPFILL_NEWFRACTAL:
							if (!Stack.Pop(&EndTransp))
								goto EPSError;
							break;
					
						default:
							// This is a valid situation - no second transparency level.
							break;
					}

					// Get first transparency level
					if (!Stack.Pop(&StartTransp))
						goto EPSError;
				}
				
			}


			if (FillType == TRANSPFILL_FRACTAL || FillType == TRANSPFILL_NEWFRACTAL)
			{
				// If it's a fractal fill type, discard the 'sub-type' parameter (should 
				// always be zero or one currently!)
				INT32 SubType;

				if (!Stack.Pop(&SubType))
					goto EPSError;

				// Default to no tiling
				Tileable = FALSE;

				// Work out the sub-type, either it has no tileable flag, or it does,
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

			// For elliptical & texture/fractal fills, get the second end-point of the fill
			if ((FillType == TRANSPFILL_ELLIPTICAL) || 
				(FillType == TRANSPFILL_TEXTURE) ||
				(FillType == TRANSPFILL_FRACTAL) ||
				(FillType == TRANSPFILL_NEWTEXTURE) ||
				(FillType == TRANSPFILL_NEWFRACTAL) ||
				(FillType == TRANSPFILL_NEWLINEAR))
			{
				if (!Stack.PopCoordPair(&EndPoint2))
					goto EPSError;
			}
			
			// Get start and end positions for variable transparency fills
			if ((FillType == TRANSPFILL_ELLIPTICAL) || 
				(FillType == TRANSPFILL_CIRCULAR)	||
				(FillType == TRANSPFILL_LINEAR)	||
				(FillType == TRANSPFILL_CONICAL)	||
				(FillType == TRANSPFILL_TEXTURE)	||
				(FillType == TRANSPFILL_FRACTAL)	||
				(FillType == TRANSPFILL_NEWLINEAR)	||
				(FillType == TRANSPFILL_NEWFRACTAL)	||
				(FillType == TRANSPFILL_NEWTEXTURE))
			{
				if (!Stack.PopCoordPair(&EndPoint) || !Stack.PopCoordPair(&StartPoint))
					goto EPSError;
			}

			switch (FillType)
			{
				// Decode Camelot EPS transparency fill codes
				case TRANSPFILL_NONE:
					if (!SetNoTranspFill())
						goto NoMemory;
					break;

				case TRANSPFILL_FLAT:
					if (!SetFlatTranspFill(TranspType, StartTransp))
						goto NoMemory;
					break;

				case TRANSPFILL_LINEAR:
					if (!SetLinearTranspFill(TranspType, StartTransp, EndTransp,
											 StartPoint, EndPoint))
						goto NoMemory;
					break;

				case TRANSPFILL_NEWLINEAR:
					if (!SetLinearTranspFill(TranspType, StartTransp, EndTransp,
											 StartPoint, EndPoint, &EndPoint2))
						goto NoMemory;
					break;

				case TRANSPFILL_ELLIPTICAL:
					if (!SetRadialTranspFill(TranspType, StartTransp, EndTransp,
											 StartPoint, EndPoint, EndPoint2))
						goto NoMemory;
					break;

				case TRANSPFILL_CIRCULAR:
					if (!SetRadialTranspFill(TranspType, StartTransp, EndTransp,
											 StartPoint, EndPoint))
						goto NoMemory;
					break;

				case TRANSPFILL_CONICAL:
					if (!SetConicalTranspFill(TranspType, StartTransp, EndTransp,
											  StartPoint, EndPoint))
						goto NoMemory;
					break;

				case TRANSPFILL_TEXTURE:
					PendingBitmap = PENDING_BITMAP_TRANSPFILL;
					BitmapAttrs.Coords[0] = StartPoint;
					BitmapAttrs.Coords[1] = EndPoint;
					BitmapAttrs.Coords[2] = EndPoint2;
					BitmapAttrs.TranspType = TranspType;
					BitmapAttrs.Transp 		= 0;
					BitmapAttrs.EndTransp 	= 255;
					break;

				case TRANSPFILL_NEWTEXTURE:
					PendingBitmap = PENDING_BITMAP_TRANSPFILL;
					BitmapAttrs.Coords[0] = StartPoint;
					BitmapAttrs.Coords[1] = EndPoint;
					BitmapAttrs.Coords[2] = EndPoint2;
					BitmapAttrs.TranspType = TranspType;
					BitmapAttrs.Transp 		= StartTransp;
					BitmapAttrs.EndTransp 	= EndTransp;
					break;

				case TRANSPFILL_FRACTAL:
					if (!SetFractalTranspFill(TranspType, StartPoint, EndPoint, EndPoint2,
											  Seed, Graininess, Gravity, Squash, DPI, 
											  Tileable))
						goto NoMemory;
					break;

				case TRANSPFILL_NEWFRACTAL:
					if (!SetFractalTranspFill(TranspType, StartPoint, EndPoint, EndPoint2,
											  Seed, Graininess, Gravity, Squash, DPI, 
											  Tileable, StartTransp, EndTransp))
						goto NoMemory;
					break;

				default:
					ENSURE(FALSE, "Unknown fill type found!");
					break;	// Don't know this fill type
			}

			break;
		}

		case EPSC_cxmt:
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

		case EPSC_ctf:
		{
			// <FontName> ctf
			String_64 FName;

			if (!Stack.Pop(&FName))
				goto EPSError;
			FName.SwapChar('-',' ');

			if ((FONTMANAGER->CacheNamedFont(&FName, EPSFilter::ClassOfFont) == ILLEGALFHANDLE))
				goto EPSError;

			if (!SetTextTypeFace(&FName, EPSFilter::ClassOfFont))
				goto EPSError;

			// Reset the font class to true type.
			EPSFilter::ClassOfFont = FC_TRUETYPE;

			break;
		}

 		case EPSC_ctb:
		{
			// <0|1> ctb
			INT32 Bold;
			if (!Stack.Pop(&Bold))
				goto EPSError;

			BOOL Boldon = (Bold==1);
			// Build any style definitions we need
			if (FontFlags.Bold)
			{
				if (!SetTextBoldFont(Boldon))
					goto EPSError;
			}
			else
			{
				if (!SetTextBold(Boldon))
					goto EPSError;
			}
			// Turn it off again
			FontFlags.Bold = FALSE;
			break;
		}			

		case EPSC_cti:
		{
			// <0|1> cti
			INT32 Italic;
			if (!Stack.Pop(&Italic))
				goto EPSError;

			BOOL ItalicOn = (Italic==1);
			// Build any style definitions we need
			if (FontFlags.Italic)
			{
				if (!SetTextItalicFont(ItalicOn))
					goto EPSError;
			}
			else
			{
				if (!SetTextItalic(ItalicOn))
					goto EPSError;
			}
			FontFlags.Italic = FALSE;
			break;
		}			

		case EPSC_cts:
		{
			// <absolute millipoint rise> <absolute millipoint pointsize> cts
			INT32 rise,ptsize;

			if (!Stack.Pop(&ptsize))
				goto EPSError;
				
			if (!Stack.Pop(&rise))
				goto EPSError;

			if (!SetTextScript(rise,ptsize))
				goto EPSError;

			break;
		}


		case EPSC_ctp:
		{
			// <absolute millipoint size> ctp
			INT32 ptsize;

			if (!Stack.Pop(&ptsize))
				goto EPSError;

			if (!SetTextSize(ptsize))
				goto EPSError;

			break;
		}

		case EPSC_ctls:
		{
			INT32 type;
			if (!Stack.Pop(&type))
				goto EPSError;

			switch (type)
			{
				case 0:
				{	
					MILLIPOINT linespace;
					if (!Stack.Pop(&linespace))
						goto EPSError;
					
					if (!SetTextLineSpacing(1,0,linespace,0))
						goto EPSError;
					break;
				}

				case 1:
				{
					double proportional;
					if (!Stack.Pop(&proportional))
						goto EPSError;

					if (!SetTextLineSpacing(2,0,0,proportional))
						goto EPSError;
					break;
				}
			}
			break;
		}

		default:
			// Token not understood - pass on to base class
			return CamelotEPSFilter::ProcessToken();
	}

	// All seemed to work OK
	return TRUE;

// Error handlers
EPSError:
	HandleEPSError();
	return FALSE;

NoMemory:
	HandleNoMemory();
	return FALSE;
}


/********************************************************************************************

>	INT32 CamelotNativeEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Returns:	0 - 10. 0 means not interested in this file, 10 means I know all about this
				file.
	Purpose:	Checks to see if the EPS comment headers specify that this is an Camelot
				Native file, as required.

********************************************************************************************/

INT32 CamelotNativeEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// Check the first line in EPS file
	if (camStrncmp((char *) pFileHeader, "%!PS-Adobe-2.0 EPSF-1.2", 23) != 0)
	{
		// Incorrect version of EPS header line - we don't want this
		return 0;
	}

	// !PS-Adobe line is ok - check creator line...
	istrstream HeaderFile((char *) pFileHeader, HeaderSize);
	char Buffer[200];

	UINT32 Lines = 0;
	while ((Lines < 20) && !HeaderFile.eof())
	{
		HeaderFile.getline(Buffer, 200);
		Lines++;

		// if the file is native camelot, return indicating strong 'interest'!
		if (camStrncmp(Buffer, "%%Creator: Xara Studio (Native)", 31) == 0)
			return 10;

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (camStrncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Didn't find a suitable Creator line. Since this is not really an eps filter it
	// will claim to know nothing about it. Camelot EPS will have a go at this file
	// but we can not really try.
	return 0;
}

/********************************************************************************************

>	BOOL CamelotNativeEPSFilter::PrepareToImport()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/94
	Returns:	TRUE if filter initialised ok;
				FALSE if not.
	Purpose:	See base class.
	Errors:		See base class.
	SeeAlso:	EPSFilter::PrepareToImport;

********************************************************************************************/

BOOL CamelotNativeEPSFilter::PrepareToImport()
{
	// Note the old state so we can put it back at the end
	OldImportWithLayers = Filter::ImportWithLayers;
	OldOpenWithLayers = Filter::OpenWithLayers;

	// Neville 7/8/97 - Layers are bad in Webster as they conflict with frames
#ifndef WEBSTER
	// We always wanted to import with layers in the native filter
	// Nowadays, its a bit more complicated
	// If we are during template loading then always load the layers
	if (TheDocument->IsTemplateLoading())
	{
		ImportWithLayers = TRUE;
		OpenWithLayers = TRUE;
	}
	else
	{
		// Otherwise if we are in:-
		// - a framed document then remove all layers from importing
		// - a layered document then import with layers
		if (TheDocument->IsImporting())
		{
			ERROR2IF(ImportInfo.pSpread == NULL,FALSE,"CamelotNativeEPSFilter::PrepareToImport No spread");
			Layer * pFrame = ImportInfo.pSpread->FindFirstFrameLayer();
			if (pFrame == NULL)
				ImportWithLayers = TRUE;	// Layered = allow layers
			else
				ImportWithLayers = FALSE;	// Framed = disallow layers
		}
		else
			ImportWithLayers = TRUE;
		OpenWithLayers = TRUE;			// Always open with layers
	}
#else
	// If we are during template loading then always load the layers
	if (TheDocument->IsTemplateLoading())
	{
		ImportWithLayers = TRUE;
		OpenWithLayers = TRUE;
	}
	else	// Otherwise remove all layers from importing
	{
		ImportWithLayers = FALSE;
		OpenWithLayers = FALSE;
	}
#endif // WEBSTER */

	// reset version numbers so files without this info have numbers 0.0
	FileVersionNumber  = 0.0;
	BuildVersionNumber = 0.0;

	// Initialise base class first.
	if (!CamelotEPSFilter::PrepareToImport())
		return FALSE;

	// set min line width to zero and store old value
	OldMinLineWidth = MinLineWidth;
	MinLineWidth = 0;
	
	return TRUE;
}



/********************************************************************************************

>	virtual void CamelotNativeEPSFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		Successful - TRUE if the import went OK, FALSE if it failed
	Purpose:	Happens at the end of the import to allow the filters to clean up after
				themselves

********************************************************************************************/

void CamelotNativeEPSFilter::CleanUpAfterImport(BOOL Successful)
{
	// Call the base class
 	CamelotEPSFilter::CleanUpAfterImport(Successful);

	// restore the old min line width
	MinLineWidth = OldMinLineWidth;

	// Get rid of the bitmap references if we have any
	// This will cause all bitmaps that were loaded, but are not used to be thrown away
	if (BitmapPoolRefs != NULL)
		delete [] BitmapPoolRefs;

	// Ready for the next one
	BitmapPoolRefs = NULL;

	// Put the Import with layers flag back as it was
	ImportWithLayers = OldImportWithLayers;
	OpenWithLayers = OldOpenWithLayers;

	// Flag this as an old format document
	if (TheDocument && Successful)
	{
		// But only flag it if we are opening the document rather than importing into an exisiting one
		if (!TheDocument->IsImporting())
			TheDocument->SetLoadedAsVersion1File(TRUE);
	}
}



/********************************************************************************************

>	virtual void CamelotNativeEPSFilter::BitmapPoolAttach(KernelBitmap* pBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		pBitmap - The bitmap to attach
	Purpose:	Makes a reference to the bitmap so that it can be used throughout the eps
				file with repeating the data.

********************************************************************************************/

void CamelotNativeEPSFilter::BitmapPoolAttach(KernelBitmap* pBitmap)
{
	// Make sure we have some refs and its in range
	if ((BitmapPoolRefs!=NULL) && (PendingBitmapNum < BitmapCount))
	{
		// yep, so attach the bitmap to the reference
		BitmapPoolRefs[PendingBitmapNum].Attach(pBitmap, GetDocument());

		if (BitmapPoolRefs[PendingBitmapNum].GetBitmap() != pBitmap)
		{
			// It didn't use the bitmap we gave it, so we can delete it
			delete pBitmap;
		}

	}
}



/********************************************************************************************

>	virtual BOOL CamelotNativeEPSFilter::ReadBitmap()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Returns:	TRUE if it worked, FALSE if it found an error
	Purpose:	Reads in a bitmap. If it is a Type 0 bitmap (ie one with all its data) it
				will ask the base class to import it. If it is a Type 1 bitmap it will look
				up the reference in the bitmap pool and use that.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::ReadBitmap()
{
	TRACEUSER( "Rik", _T("ReadBitmap...\n"));
	// Find out what kind of bitmap it is - always type 0 or 1 at present.
	INT32 BitmapType;
	if (!Stack.Pop(&BitmapType))
	{
		// Error - not enough operands
		HandleEPSError();
		return FALSE;
	}

	// if it is the old type 0 bitmap get the base class to do all the work
	if (BitmapType == 0)
	{
		// Need to push the type back on the stack again ready for the base class to read it
		TRACEUSER( "Rik", _T("Bitmap with data (in the pool I hope)\n"));
		Stack.Push(BitmapType);
		return CamelotEPSFilter::ReadBitmap();
	}

	// OK, see if we have a type we do not know about
	if (BitmapType != 1)
	{
		// Error - Bad Type
		HandleEPSError();
		return FALSE;
	}
	
	// Right, we have a type 1 bitmap....
	INT32 BitmapIndex = 0;
	
	// Read in info on the bitmap (ie, which bitmap from the pool do we want) ...
	if ((!Stack.Pop(&BitmapIndex)) /*|| (BitmapIndex >= BitmapCount)*/)
	{
		// Error - not enough operands
		HandleEPSError();
		return FALSE;
	}

	// ... and make sure that is in the range of bitmaps we have.
	// If the Index is out of range then use the Last one rather than just erroring.
	if (BitmapIndex >= BitmapCount)
		BitmapIndex = (BitmapCount-1);

	TRACEUSER( "Rik", _T("Bitmap reference found - refers to bitmap %d\n"), BitmapIndex);

	// Want to find bitmap 'BitmapIndex' and set pBitmap to point at it
	pBitmap = BitmapPoolRefs[BitmapIndex].GetBitmap();
	return TRUE;
}



/********************************************************************************************

>	virtual INT32 CamelotNativeEPSFilter::ImportBinary(ADDR pData, INT32 Length)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		Length - the number of bytes to read, in terms of binary data imported, as
						 opposed to how many bytes the file format takes to represent this
						 binary data.
	Outputs:	pData - the buffer to put the data into.
	Returns:	TRUE if it imported the specified number of bytes correctly;
				FALSE if not.
	Purpose:	Reads the binary data out of the file and puts it into the buffer supplied.
				It also makes checks to see if the appropriate chars are found surounding
				the Binary stream.
	Errors:		File input errors; EOF
	SeeAlso:	Filter::ImportBinary

********************************************************************************************/

INT32 CamelotNativeEPSFilter::ImportBinary(ADDR pData, INT32 Length)
{
	// read chars until we get a >
	INT32 CountDown = 50;
	char Ch;
	do {
		// Read a char
		if (EPSFile->read(&Ch, 1).fail())
		{
			HandleEPSError();
			return FALSE;
		}

		// Count it
		CountDown--;
	} while ((CountDown>0) && (Ch!='>'));

	// See if we found it
	if (CountDown==0)
	{
		HandleEPSError();
		return FALSE;
	}
	
	// Read binary section
	if (EPSFile->read(pData, Length).fail())
	{
		HandleEPSError();
		return FALSE;
	}

	// Read char and check that it is a <
	if (EPSFile->read(&Ch, 1).fail())
	{
		HandleEPSError();
		return FALSE;
	}

	if (Ch!='<')
	{
		HandleEPSError();
		return FALSE;		
	}

	// Find out how much of the file we have read
	INT32 CharsRead = EPSFile->GetCharsRead();
	if (CharsRead > (LastProgressUpdate + 2048))
	{
		if (!ContinueSlowJob(CharsRead))
		{
			// Abort operation - make sure nodes are deleted and not added to the tree.
			ERROR(_R(IDT_IMPORT_USERABORT), FALSE);
		}
		else
		{
			LastProgressUpdate = CharsRead;
		}
	}

	// Re-sync the tokeniser
	EPSFile->GetLine();
	EPSFile->GetCh();

	// Must have worked
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL CamelotNativeEPSFilter::ProcessFilterComment()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/12/94
	Returns:	TRUE if it finds something it likes, FALSE if not
	Purpose:	This deals with the page info held in comments in the EPS stream

********************************************************************************************/

BOOL CamelotNativeEPSFilter::ProcessFilterComment()
{
	// read the build version number of the app which saved this file
	if (camStrncmp(TokenBuf, "%%Creator: Xara Studio (Native)", 31) == 0)
	{
		BuildVersionNumber = atof(TokenBuf+31);
		return TRUE;
	}

	// Go and have a look at the token buffer and see if it is our special
	// file version comment. If so, then extract the version number from it. 
	if (camStrncmp(TokenBuf, "%%File version:", 15)==0)
	{
		char* pVersion = ((char*)TokenBuf)+15;
		double Version = atof(pVersion);
		TRACEUSER( "Neville", _T("Native file version = %f\n"), Version);
		FileVersionNumber = Version;
		// Must stop later file versions loading back into this version.
		if (Version > ReadNativeVersion)
		{
			InformWarning(_R(IDW_FILEISWRONGVERSION));
			// flag the error state
			EPSFlags.EPSErrorEncountered = TRUE;
			// set the user has cancelled the error message so that this one is not reported
			Error::SetError(_R(IDN_USER_CANCELLED), 0);

			// We recokonised the token but it is invalid
			return TRUE;
		}
	}
	
	// Go and have a look at the token buffer and see if it is our special
	// compression comment
	if (camStrncmp(TokenBuf, "%%Compression:", 14)==0)
	{
//#ifdef STANDALONE
//		// First release of the file viewer will not have the native file compression
//		// loading code in. Therefore, we must abort if the compression	tokens are
//		// encountered.
//		//if (FileVersionNumber < 0.500)
//		InformWarning(_R(IDW_FILEISCOMPRESSED));
//		// flag the error state
//		EPSFlags.EPSErrorEncountered = TRUE;
//		// set the user has cancelled the error message so that this one is not reported
//		Error::SetError(_R(IDN_USER_CANCELLED), 0);
//
//		// We recokonised the token but it is invalid
//		return TRUE;
//#endif
		char* pType = ((char*)TokenBuf)+14;
		CompressionType = _ttoi(pType);
		TRACEUSER( "Neville", _T("CamelotNativeEPSFilter::ProcessFilterComment compression type %d\n"), CompressionType);

		// We have found our compression token so turn compression on
		BOOL ok = EPSFile->SetCompression(TRUE);
		if (!ok)
		{
			// We had a problem starting up the decompressor
			// flag the error state
			EPSFlags.EPSErrorEncountered = TRUE;
			// set up a useful error message, this one seems to say that we ran out of memory
			// and this is the most likely reason why
			Error::SetError(_R(IDT_EPS_NOMEMORY), 0);
			// We recokonised the token but it is invalid
			return TRUE;
		}

		return TRUE;
	}
	if (camStrncmp(TokenBuf, "%%Compression info:", 19)==0)
	{
		char* pVersion = ((char*)TokenBuf)+19;
		double CompVersion = atof(pVersion);
		TRACEUSER( "Neville", _T("Compression version = %f\n"), CompVersion);
		double StreamVersion = GZipFile::GetStreamVersionNo();
		// If the version stored in the file is later than the one in the stream class
		// then we cannot cope with this data, so error.
		if (CompVersion > StreamVersion)
		{
			InformWarning(_R(IDW_FILEISCOMPRESSED));
			// flag the error state
			EPSFlags.EPSErrorEncountered = TRUE;
			// set the user has cancelled the error message so that this one is not reported
			Error::SetError(_R(IDN_USER_CANCELLED), 0);
			// We recokonised the token but it is invalid
			return TRUE;
		}

		return TRUE;
	}
	if (camStrncmp(TokenBuf, "%%EndCompression:", 17)==0)
	{
		// We have found our compression token so turn compression on
		BOOL ok = EPSFile->SetCompression(FALSE);
		if (!ok)
		{
			// We had a problem stoping up the decompressor
			// flag the error state
			EPSFlags.EPSErrorEncountered = TRUE;
			// We will assume that an error has been set up. At present, the only one
			// most likely is a CRC error.
			// We recokonised the token but it is invalid
			return TRUE;
		}

		return TRUE;
	}
	if (camStrncmp(TokenBuf, "%%EndCompressionInfo:", 21)==0)
	{
		return TRUE;
	}
	  
	// Go and have a look at the token buffer and see if it looks like bitmap count
	if (camStrncmp(TokenBuf, "%%BitmapPoolCount", 17)==0)
	{
		char* pCount = ((char*)TokenBuf)+17;
		BitmapCount = _ttoi(pCount);
		TRACEUSER( "Rik", _T("%d\n"), BitmapCount);

		// There are no bitmaps in this file
		if (BitmapCount==0)
			return TRUE;

		// Make sure that we have not already found one of these
		if (BitmapPoolRefs!=NULL)
			ERROR3("Bitmap Pool already in use!");

		// Allocate a few references to bitmaps
		BitmapPoolRefs = new KernelBitmapRef[BitmapCount];
		return TRUE;
	}

	// Check for any text comments
	if (camStrncmp(TokenBuf, "%%XSScript",	10)==0)
	{
		TextComment[0]=2;
		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************

>	virtual EPSExportDC* CamelotNativeEPSFilter::CreateExportDC()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Returns:	Pointer to a new ExportDC.
	Purpose:	Creates a new export DC (NativeExportDC) ready to export the Native file to.

********************************************************************************************/

EPSExportDC* CamelotNativeEPSFilter::CreateExportDC()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	return new NativeExportDC(this);
#else
	return NULL;
#endif
}


/********************************************************************************************

>	virtual BOOL CamelotNativeEPSFilter::GetExportOptions( )

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> HUmphrys
	Created:	22/12/95
	Inputs:		-
	Returns:	TRUE if OK, FALSE if user pressed Cancel.
	Purpose:	Allows the user to be prompted to get information for export.
				This overriden version returns True so that nothing happens.
	Scope: 		Protected.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::GetExportOptions( )
{
	// Overide the CamelotEPS form so that we do nothing again i.e. just return TRUE
	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotNativeEPSFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		pFile - the path name
				pSpread - The spread to save
	Returns:	TRUE if it worked, FALSE if it failed
	Purpose:	Gets things ready for the export. Creates the render region to do all the
				exporting with.

********************************************************************************************/

BOOL CamelotNativeEPSFilter::PrepareToExport(CCLexFile* pFile, Spread* pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Use base class to do most of it
	if (!CamelotEPSFilter::PrepareToExport(pFile, pSpread)) return FALSE;

	// Try and create a render region if we are really a
	// Native filter and not something derived from it
	if (IS_A(this, CamelotNativeEPSFilter))
	{
		// Don't care about clip regions when exporting - create a null region.
		DocRect NullClipRect;
		NullClipRect.MakeEmpty();

		// Don't use rendering matrix when exporting EPS as it uses fractional coordinates.
		Matrix Identity;

		// Don't use view scale; set to 1
		FIXED16 Scale(1);

		// Create the render region
		ExportRegion = new NativeRenderRegion(NullClipRect, Identity, Scale);
		if (ExportRegion == NULL) return FALSE;

		// use the first docview
		DocView * pDocView = TheDocument->GetFirstDocView();
		// fallback - help no docviews
		if ((!pDocView) || (!pDocView->IsKindOf(CC_RUNTIME_CLASS(DocView))))
			pDocView = DocView::GetSelected(); // help! use selected docview

		// Attach to the right device.
		ExportRegion->AttachDevice(pDocView, ExportDCPtr, pSpread);
	}
#endif
	// Thats it
	return TRUE;
}


/********************************************************************************************

>	BOOL CamelotNativeEPSFilter::ExportBitmap(KernelBitmap& TheBitmap)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		TheBitmap - the bitmap to export
	Returns:	TRUE if the bitmap was exported ok (or if it wasn't exported because the
					 file format does not support it;
				FALSE if an error occured.
	Purpose:	Export a bitmap to a Camelot EPS file.  This does one of 2 things.
				If we are in the middle of exporting the bitmap pool it will save out
				all the bitmap data, just as CamelotEPS does. If we are not in the bitmap
				pool it will simply save out a reference to the bitmap in the pool
	Errors:		Usual disk/file errors.
	SeeAlso:	Filter::ExportBitmap

********************************************************************************************/

BOOL CamelotNativeEPSFilter::ExportBitmap(KernelBitmap& TheBitmap)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// If this is the bitmap pool, then save out some data
	if (IsSavingBitmapPool)
	{
		TRACEUSER( "Rik", _T("In the bitmap pool, so saving out real data\n"));
		return CamelotEPSFilter::ExportBitmap(TheBitmap);
	}

	// If we get here then we must be saving out a reference to a bitmap saved earlier in the pool
	TRACEUSER( "Rik", _T("Trying to save a reference to a bitmap\n"));
	
	// Try and find the bitmap in the bitmap pool
	// Get the document that we are saving
	Document* pDoc = GetDocument();
	if (pDoc==NULL)
		return FALSE;

	// Find the doccomponent with the list of bitmaps in it
	DocComponent* DocBitmapList = pDoc->GetDocComponent(CC_RUNTIME_CLASS(BitmapListComponent));
	if (DocBitmapList==NULL)
		return FALSE;

	ExportedBitmaps* pBitmapExportPool = ((BitmapListComponent*)DocBitmapList)->GetBitmapExportPool();
	ERROR3IF(pBitmapExportPool == NULL, "No BitmapExportPool in CamelotNativeEPSFilter::ExportBitmap");
	if (pBitmapExportPool==NULL)
		return FALSE;

	INT32 BitmapPoolNum = -1;

	// Get ready to look for our bitmap
	class ExportBitmap* pExportBitmap = pBitmapExportPool->GetBitmap(&TheBitmap);
	// The 'class' statement is because this filter has a function called 'ExportBitmap'

	ERROR3IF(pExportBitmap == NULL, "Couldn't find the bitmap in the pool in CamelotNativeEPSFilter::ExportBitmap");

	if (pExportBitmap)
	{
		BitmapPoolNum = pExportBitmap->RecordNumber;
	}
	
	// Did we find it
	if (BitmapPoolNum >= 0)
	{
		// Export the number of the bitmap
		ExportDCPtr->OutputValue((UINT32) BitmapPoolNum);

		// Bitmap type is always 1 to represent a reference (0 is an actual bitmap)
		ExportDCPtr->OutputToken("1");

		// Write out the bitmap start token
		ExportDCPtr->OutputToken("csbm");
		ExportDCPtr->OutputNewLine();

		// Write out the bitmap end token
		ExportDCPtr->OutputToken("cebm");
		ExportDCPtr->OutputNewLine();

		// All ok
		return TRUE;
	}
#endif
	// Did not find the bitmap
	return FALSE;
}


/********************************************************************************************

>	BitmapFilterSupport CamelotNativeEPSFilter::GetBitmapSupportLevel()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/95
	Returns:	IndirectedBitmapSupport - This filter supports bitmap indirection; i.e. a
										  bitmap pool of some type is saved and then this
										  can be referenced in the file.
	Purpose:	Determine how well this filter supports bitmaps when exporting.

********************************************************************************************/

BitmapFilterSupport CamelotNativeEPSFilter::GetBitmapSupportLevel()
{
	return IndirectedBitmapSupport;
}


/********************************************************************************************
>	double CamelotNativeEPSFilter::SmartGetBuildNumber()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/95
	Returns:	Build version number
	Purpose:	Determines a monotomic build version number based on BuildVersionNumber which
				unfortunately jumps to v1.00/a/b/c/d but these are mapped back to real build
				numbers. v1.00/a/b/c/d all have BuildVersionNumbers of 1.00 and are all
				descended from an isolated liniage based on 0.586 so all map to this number
	Note:		Only guaranteed for build versions greater than 0.586!
********************************************************************************************/

double CamelotNativeEPSFilter::SmartGetBuildNumber()
{
	double BuildNumber = GetBuildNumber();
	if (BuildNumber==1.00)
		BuildNumber = 0.586;
	return BuildNumber;
}

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

/////////////////////////////////////////////////////////////////////////////////////////////
// NativeRenderRegion

/********************************************************************************************

>	NativeRenderRegion::NativeRenderRegion(DocRect ClipRect, Matrix ConvertMatrix,
										   FIXED16 ViewScale)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/94
	Inputs:		ClipRect - The rectangle to use as the clip rect for the rendering
				ConvertMatrix - the rendering matrix
				ViewScale - the scale to render at
	Purpose:	Constructor for the Native Save/Load filter render region.

********************************************************************************************/

NativeRenderRegion::NativeRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale) :
					CamelotEPSRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
	CreatorString = "Xara Studio (Native) " CAMELOT_VERSION_STRING;

	// Flag that we have not started up the compressor ok by default.
	CompressionInitedOk = FALSE;
}

/********************************************************************************************

>	virtual void NativeRenderRegion::ConditionalSuicide ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/00
	Inputs:		-
	Returns:	-
	Purpose:	Causes the object to commit suicide. This is to get around using a few
				if IS_A calls elsewhere in Camelot.

********************************************************************************************/

void NativeRenderRegion::ConditionalSuicide ( void )
{
	// Delete the object.
	delete this;
}

/********************************************************************************************

>	virtual void NativeRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void NativeRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	// We can do everything mate cos we're fabby.
	pCaps->CanDoAll();
}


/********************************************************************************************

>	BOOL NativeRenderRegion::WriteProlog(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Over-ride the routines defined by CamelotEPSRenderRegion - we don't need
				any PostScript prolog in ART files.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL NativeRenderRegion::WriteProlog(KernelDC *pDC)
{
	// No PostScript procedure defns here...
	return TRUE;
}

/********************************************************************************************

>	BOOL NativeRenderRegion::WriteSetup(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Over-ride the routines defined by CamelotEPSRenderRegion - we don't need
				any PostScript setup in ART files.
	SeeAlso:	NativeRenderRegion::WriteSetup

********************************************************************************************/

BOOL NativeRenderRegion::WriteSetup(KernelDC *pDC)
{
	// No PostScript setup code here...
	return TRUE;
}


/********************************************************************************************

>	void NativeRenderRegion::GetValidPathAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/5/95
	Purpose:	See CamelotEPSRenderRegion::GetValidPathAttributes.
				This version checks and handles special native features.
	SeeAlso:	EPSRenderRegion::GetValidPathAttributes

********************************************************************************************/

void NativeRenderRegion::GetValidPathAttributes()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Find out what this render region can do.
	RRCaps Caps;
	GetRenderRegionCaps(&Caps);

	if (SetLastOutputAttribute(ATTR_STARTARROW))
	{
		// Start arrow head
		StartArrowAttribute *pArrow = 			
			(StartArrowAttribute *) CurrentAttrs[ATTR_STARTARROW].pAttr;

		TRACEUSER( "Will", _T("Outputing Start Arrow, ID=%d\n"),pArrow->StartArrow.GetArrowID());

		if (pArrow->StartArrow.GetArrowID() >= 0)
		{
			pDC->OutputValue((INT32)pArrow->StartArrow.GetArrowID());
			pDC->OutputReal((pArrow->StartArrow.GetArrowWidth()).MakeDouble());
			pDC->OutputReal((pArrow->StartArrow.GetArrowHeight()).MakeDouble());

			pDC->OutputToken("csah");
			pDC->OutputNewLine();
		}
	}

	if (SetLastOutputAttribute(ATTR_ENDARROW))
	{
		// End arrow head
		EndArrowAttribute *pArrow = 			
			(EndArrowAttribute *) CurrentAttrs[ATTR_ENDARROW].pAttr;

		TRACEUSER( "Will", _T("Outputing End Arrow, ID=%d\n"),pArrow->EndArrow.GetArrowID());

		if (pArrow->EndArrow.GetArrowID() >= 0)
		{
			pDC->OutputValue((INT32)pArrow->EndArrow.GetArrowID());
			pDC->OutputReal((pArrow->EndArrow.GetArrowWidth()).MakeDouble());
			pDC->OutputReal((pArrow->EndArrow.GetArrowHeight()).MakeDouble());

			pDC->OutputToken("ceah");
			pDC->OutputNewLine();
		}
	}

	if (SetLastOutputAttribute(ATTR_DASHPATTERN))
	{
		// Get dash pattern
		DashPatternAttribute *pDash = 			
			(DashPatternAttribute *) CurrentAttrs[ATTR_DASHPATTERN].pAttr;

		TRACEUSER( "Will", _T("Outputing Dash Pattern, ID=%d\n"),pDash->DashPattern.GetDashID());

		pDC->OutputValue((INT32)pDash->DashPattern.GetDashID());
		pDC->OutputToken("cdp");
		pDC->OutputNewLine();
	}

	// Now do transparency fills
	GetValidTransparencyAttributes();

	// Handle usual pens/brushes
	CamelotEPSRenderRegion::GetValidPathAttributes();
}


void NativeRenderRegion::GetValidTransparencyAttributes()
{
	// Find out what this render region can do.
	RRCaps Caps;
	GetRenderRegionCaps(&Caps);

	// Can we do transparency?
	if (!Caps.Transparency)
		// No - so don't do anything
		return;

	KernelDC *pDC = (KernelDC *) RenderDC;

	if (SetLastOutputAttribute(ATTR_STROKETRANSP))
	{
		// Now do the line transparency
		StrokeTranspAttribute *pAttr = 
			(StrokeTranspAttribute *) CurrentAttrs[ATTR_STROKETRANSP].pAttr;

		UINT32 Transp = *pAttr->GetStartTransp();
		UINT32 TranspType = pAttr->GetTranspType();
		
		TRACEUSER( "Will", _T("Outputing Line transp, Val=%d, Type=%d\n"),Transp, TranspType);

		// Output transparency level
		pDC->OutputValue((UINT32) Transp);
		// Output transparency type...
		pDC->OutputValue((UINT32) TranspType);

		pDC->OutputToken("cst");
		pDC->OutputNewLine();
	}

	if (SetLastOutputAttribute(ATTR_TRANSPFILLGEOMETRY))
	{
		// Now do the transparent fill geometry
		TranspFillAttribute *pFillAttr = 
			(TranspFillAttribute *) CurrentAttrs[ATTR_TRANSPFILLGEOMETRY].pAttr;

		// Get the transparency information
		UINT32 *StartTransp,
			 *EndTransp,
			 TranspType,
			 FillType = TRANSPFILL_NONE;

		TranspType = pFillAttr->GetTranspType();		//pPaintMode->m_PaintingMode;
		ENSURE((TranspType >= 1) && (TranspType <= 3), "Bad transparency type!");		// Deliberately restrictive?

		StartTransp = pFillAttr->GetStartTransp();
		ENSURE((StartTransp == NULL) || ((*StartTransp >= 0) && (*StartTransp <= 255)), 
			   "Bad start transparency level!");

		// Get the correct brush - may be a grad fill.
		if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FlatTranspFillAttribute)))
		{
			// Check special case for 'no transparency' (0% transparent and type 1)
			if (((*StartTransp) != 0) || (TranspType != TT_Mix))
				FillType = TRANSPFILL_FLAT;
		}
		else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(GradTranspFillAttribute)) && 
				 !pFillAttr->IsKindOf(CC_RUNTIME_CLASS(BitmapTranspFillAttribute)) )
		{
			// Output the start and end points of the grad fill
			DocCoord *Point;
			Point = pFillAttr->GetStartPoint();
			pDC->OutputCoord(*Point);
			Point = pFillAttr->GetEndPoint();
			pDC->OutputCoord(*Point);

			// Output the fill type
			if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(LinearTranspFillAttribute)))
			{
				DocCoord* Start = pFillAttr->GetStartPoint();
				DocCoord* End   = pFillAttr->GetEndPoint();
				DocCoord* End2  = pFillAttr->GetEndPoint2();

				if (AreLinesPerpendicular(Start, End, End2))
				{
					TRACEUSER( "Will", _T("Exporting Simple Transp Linear Fill\n"));
					FillType = TRANSPFILL_LINEAR;
				}
				else
				{
					TRACEUSER( "Will", _T("Exporting New Style Transp Linear Fill\n"));
					pDC->OutputCoord(*pFillAttr->GetEndPoint2());
					FillType = TRANSPFILL_NEWLINEAR;
				}
			}
			else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(RadialTranspFillAttribute)))
			{
				// Is it circular or elliptical?
				RadialTranspFillAttribute *pRadialFillAttr = 
					(RadialTranspFillAttribute *) pFillAttr;

				if (pRadialFillAttr->IsElliptical())
				{
					// Elliptical fill - output the second end point.
					Point = pRadialFillAttr->GetEndPoint2();
					pDC->OutputCoord(*Point);
					FillType = TRANSPFILL_ELLIPTICAL;
				}
				else
				{
					// Circular fill
					FillType = TRANSPFILL_CIRCULAR;
				}
			}
			else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(ConicalTranspFillAttribute)))
			{
				FillType = TRANSPFILL_CONICAL;
			}
			else
			{
				// Unknown fill type - fall back to basic attributes.
				ERROR3("Unsupported grad fill encountered while exporting");
				EPSRenderRegion::GetValidPathAttributes();
				return;
			}

		}
		else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FractalTranspFillAttribute)) &&
				 Caps.BitmapFills)
		{
			// Fractal fill - output fractal fill command...
			FractalTranspFillAttribute *pFractalFill = (FractalTranspFillAttribute *) pFillAttr;

			// Save the coords of the fill mesh
			pDC->OutputCoord(pFractalFill->StartPoint);
			pDC->OutputCoord(pFractalFill->EndPoint);
			pDC->OutputCoord(pFractalFill->EndPoint2);

			// Save the fractal parameters out.
			pDC->OutputValue(pFractalFill->Seed);
			pDC->OutputReal(pFractalFill->Graininess.MakeDouble());
			pDC->OutputReal(pFractalFill->Gravity.MakeDouble());
			pDC->OutputReal(pFractalFill->Squash.MakeDouble());

			// And now the DPI of the fractal bitmap
			UINT32 DPI = pFractalFill->GetFractalDPI();
			pDC->OutputValue(DPI);

			// And now the 'tileable' flag
			pDC->OutputValue((INT32) (pFractalFill->GetTileable()));

			// Always Fractal type 1 at present
			pDC->OutputToken("1");

			TRACEUSER( "Will", _T("Exporting new fractal transp\n"));
			
			// This is a fractal fill
			FillType = TRANSPFILL_NEWFRACTAL;
		}
		else if	(pFillAttr->IsKindOf(CC_RUNTIME_CLASS(NoiseTranspFillAttribute)))
		{
			FillType = TRANSPFILL_NONE;
		}
		else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(BitmapTranspFillAttribute)) &&
				 Caps.BitmapFills)
		{
			// Texture fill - output texture fill command:
			BitmapFillAttribute *pBitmapFill = (BitmapFillAttribute *) pFillAttr;

			// Output the 3 fill points...
			DocCoord *Point;
			Point = pFillAttr->GetStartPoint();
			pDC->OutputCoord(*Point);
			Point = pFillAttr->GetEndPoint();
			pDC->OutputCoord(*Point);
			Point = pFillAttr->GetEndPoint2();
			pDC->OutputCoord(*Point);

			TRACEUSER( "Will", _T("Exporting new bitmap transp\n"));

			// This is a texture fill
			FillType = TRANSPFILL_NEWTEXTURE;
		}
		else
		{
			ERROR3("Illegal transparency fill type!");
		}

		// Output transparency levels...
		if (FillType != TRANSPFILL_NONE)
		{
			if ((FillType != TRANSPFILL_TEXTURE) && (FillType != TRANSPFILL_FRACTAL))
			{
				// Output first transparency level
				pDC->OutputValue((UINT32) (*StartTransp));

				// Output second transparency level if there is one.
				switch (FillType)
				{
					case TRANSPFILL_LINEAR:
					case TRANSPFILL_ELLIPTICAL:
					case TRANSPFILL_CIRCULAR:
					case TRANSPFILL_CONICAL:
					case TRANSPFILL_NEWLINEAR:
					case TRANSPFILL_NEWTEXTURE:
					case TRANSPFILL_NEWFRACTAL:
						EndTransp = pFillAttr->GetEndTransp();
						ENSURE((*EndTransp >= 0) && (*EndTransp <= 255), 
							   "Bad end transparency level!");
						pDC->OutputValue((UINT32) (*EndTransp));
						break;

					default:
						// This is a valid situation - no second transparency level.
						break;
				}
			}

			// Output transparency type...
			pDC->OutputValue((UINT32) TranspType);
		
		}
		
		// Output transparency fill type
		pDC->OutputValue((UINT32) FillType);
	
		// Output the transparent fill token
		pDC->OutputToken("cxt");
		pDC->OutputNewLine();

		// If this is a bitmap-based transprency fill, output the bitmap
		if ((FillType == TRANSPFILL_TEXTURE || FillType == TRANSPFILL_NEWTEXTURE) &&
			 Caps.BitmapFills)
		{
			BitmapTranspFillAttribute *pBitmapFill = (BitmapTranspFillAttribute *) pFillAttr;
			ExportDC *pExportDC = (ExportDC *) pDC;

			if (pBitmapFill->GetBitmap())
				pExportDC->GetParentFilter()->ExportBitmap(*pBitmapFill->GetBitmap());
		}
	}

	if (SetLastOutputAttribute(ATTR_TRANSPFILLMAPPING))
	{
		// Now do the chromatic fill mapping
		TranspFillMappingAttribute *pFillAttr = 
			(TranspFillMappingAttribute *) CurrentAttrs[ATTR_TRANSPFILLMAPPING].pAttr;

		// Get the correct mapping
		INT32 MappingType = pFillAttr->Repeat;
		ERROR3IF((MappingType < 1) || (MappingType > 3), "Illegal fill mapping value!");
		
		// Output fill mapping
		pDC->OutputValue((UINT32) MappingType);

		// Allow for future extension of fill mappings.	
		pDC->OutputToken("0");

		// Output the fill mapping token
		pDC->OutputToken("cxmt");
		pDC->OutputNewLine();
	}
}





/********************************************************************************************

>	void NativeRenderRegion::GetValidTextAttributes()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/05/95
	Purpose:	Output Native file compatible text attribute tokens.

********************************************************************************************/

void NativeRenderRegion::GetValidTextAttributes()
{
   	KernelDC *pDC = (KernelDC *) RenderDC;

	/* In native format documents we output 
		fontname			= <fontname> ctf
		fontsize			= <mp size> ctp
		bold				= <0|1> ctb
		italic				= <0|1> cti
		sub/superscript		= <abs mp rise> <abs mp pointsize> cts
		linespacing			= <absolute linespace> <0> ctls
							  <proportional linespace> <1> ctls
		
		all other styles are output by the baseclass
	*/

   	if (SetLastOutputAttribute(ATTR_TXTFONTTYPEFACE))
   	{
		TxtFontTypefaceAttribute* pFontAttr = (TxtFontTypefaceAttribute*)CurrentAttrs[ATTR_TXTFONTTYPEFACE].pAttr;
		if (pFontAttr)
		{
			String_64 OutputFont;
			FontClass Class;

			CachedFontItem* pItem = FONTMANAGER->GetCachedFont(pFontAttr->HTypeface);
			ERROR3IF(pItem==NULL,"Cannot find the cached font in GetValidTextAttributes");
			OutputFont = *(pItem->GetFontName());
			Class =	pItem->GetFontClass();

			// If we are not outputting true type fonts we need to output an
			// extension token to say, the next ctf on the input stream should be
			// treated as a FontClass type of font. This allows us to load back
			// ATM fonts which have the same name as true type fonts.
			if (Class!=FC_TRUETYPE)
			{
				pDC->OutputValue((INT32)TAG_FONTTYPE);
				pDC->OutputToken("cso");
				pDC->OutputValue((INT32)Class);
				pDC->OutputToken("cftf");
				pDC->OutputToken("ceo");
				pDC->OutputNewLine();
			}

			BOOL ok = FONTMANAGER->GetFontName(pFontAttr->HTypeface, OutputFont);
			OutputFont.SwapChar(' ','-');

			pDC->OutputToken((TCHAR *)OutputFont);
			pDC->OutputToken("ctf");
			pDC->OutputNewLine();

			if ((pFontAttr->IsBold) || (pFontAttr->IsItalic))
			{
				// font flags a bold or italic substyle.

				pDC->OutputValue((INT32)TAG_FONTFLAGS);
				pDC->OutputToken("cso");
				pDC->OutputNewLine();
				pDC->OutputToken("cfft");
				pDC->OutputNewLine();
				pDC->OutputToken("ceo");
				pDC->OutputNewLine();

				pDC->OutputValue((INT32) pFontAttr->IsBold);
				pDC->OutputToken("ctb");
 				pDC->OutputNewLine();

				pDC->OutputValue((INT32) pFontAttr->IsItalic);
				pDC->OutputToken("cti");
 				pDC->OutputNewLine();
			}
		}
   	}

	if (SetLastOutputAttribute(ATTR_TXTFONTSIZE))
   	{
		// Output the fontsize next
		pDC->OutputValue(RR_TXTFONTSIZE());
		pDC->OutputToken("ctp");
		pDC->OutputNewLine();
   	}		 

	if (SetLastOutputAttribute(ATTR_TXTBOLD))
   	{
		// read the bold attribute value explicitly	(dont use RR_TXTBOLD)
		BOOL Bold = ( (TxtBoldAttribute*)(CurrentAttrs[ATTR_TXTBOLD].pAttr) )->BoldOn; 
		pDC->OutputValue((INT32)Bold);
		pDC->OutputToken("ctb");
 		pDC->OutputNewLine();
   	}		 

	if (SetLastOutputAttribute(ATTR_TXTITALIC))
   	{
		// read the italic attribute value explicitly (dont use RR_TXTITALIC)
		BOOL Italic =  ( (TxtItalicAttribute*)(CurrentAttrs[ATTR_TXTITALIC].pAttr) )->ItalicOn;
		pDC->OutputValue((INT32)Italic);
		pDC->OutputToken("cti");
 		pDC->OutputNewLine();
   	}		

	if (SetLastOutputAttribute(ATTR_TXTSCRIPT))
	{
		TxtScriptAttribute* pScript = RR_TXTSCRIPT();

		double FontSize = (double)RR_TXTFONTSIZE();			// in millipoints
		double offset = (pScript->Offset).MakeDouble();
		double size = (pScript->Size).MakeDouble();

		MILLIPOINT rise = (MILLIPOINT)(FontSize*offset);	// rise in millipoints
		MILLIPOINT ptsize = (MILLIPOINT)(FontSize*size);	// pointsize in millipoints
		pDC->OutputValue(rise);
		pDC->OutputValue(ptsize);
		pDC->OutputToken("cts");
		pDC->OutputNewLine();
	}

	// output proportional or none proportional line spacing
   	if (SetLastOutputAttribute(ATTR_TXTLINESPACE))
   	{
		TxtLineSpaceAttribute* pLineSpace = (TxtLineSpaceAttribute*)(CurrentAttrs[ATTR_TXTLINESPACE].pAttr);

		// There are some rules for reading the linespacing value which I shall divulge
		// If IsARatio is true then use the proportinal linespacing value.
		// else use the absolute linespacing
		// However if the absolute linespacing is zero, then we MUST use the proportional
		// linespacing. Eeek!
		// ie it is an error if (absolute==0 && !IsARatio()) which we will check for here

		if (!pLineSpace->IsARatio())
		{
			INT32 LineSpace = pLineSpace->Value;
			ERROR3IF(LineSpace==0, "Absolute line spacing is zero yet IsARatio() is FALSE, in GetValidTextAttributes()");
			pDC->OutputValue(LineSpace);
			pDC->OutputValue((INT32)0);
		}
		else
		{
			double Ratio = (pLineSpace->Ratio).MakeDouble();
			pDC->OutputFloat(Ratio,4);
			pDC->OutputValue((INT32)1);		 
		}

		pDC->OutputToken("ctls");
		pDC->OutputNewLine();
	}


	// Handle all other types
	EPSRenderRegion::GetValidTextAttributes();
}


/********************************************************************************************

>	BOOL NativeRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/05/95
	Inputs:		ch      - unicode value of char
				pMatrix - matrix specifying transforms to place char correctly in document
	Returns:	FALSE if fails
	Purpose:	

********************************************************************************************/

BOOL NativeRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return TRUE;

	ERROR2IF(   this==NULL,FALSE,"RenderRegion::RenderChar() - this==NULL");
	ERROR2IF(pMatrix==NULL,FALSE,"RenderRegion::RenderChar() - pMatrix==NULL");

#if EXPORT_TEXT
	// Check for changed attributes
	GetValidPathAttributes();
	GetValidTextAttributes();

	KernelDC *pDC = (KernelDC *) RenderDC;

	INT32 CharOut = (INT32)ch;
	INT32 NumCodes = 1;
	pDC->OutputValue(CharOut);
	pDC->OutputValue(NumCodes);
	pDC->OutputToken("ctx");
	pDC->OutputNewLine();

#else
	// just do what RenderRegion::RenderChar() would do!

	// create the char's path
	Path* pCharPath=CreateCharPath(ch,pMatrix);
	if (pCharPath==NULL)
		return FALSE;

	// draw path using current attibutes in render region
 	if (pCharPath->GetNumCoords()!=0)
		DrawPath(pCharPath);

	// clean up
	delete pCharPath;
#endif

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NativeRenderRegion::WriteFileVersion(KernelDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error)
	Purpose:	This allows the filter to save out a comment line giving file version
				information. In this Native format version we actually write out the current
				file version information.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/
BOOL NativeRenderRegion::WriteFileVersion(KernelDC *pDC)
{
	// Buffer used to build up the file version comment.
	char buf[50];

	// Output the current file version in the format 1.00 for NativeFileVersion 100
	_stprintf(buf, "%%%%File version: %.2f", WriteNativeVersion);
	pDC->OutputToken(buf);
	pDC->OutputNewLine();

	return TRUE;
}
	
/********************************************************************************************

>	BOOL NativeRenderRegion::WriteCompressionState(KernelDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	In this native file saving class version we save out the compression status
				of this file with other useful compression type information. We also start
				compressing the data from this point onwards.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL NativeRenderRegion::WriteCompressionState(KernelDC *pDC)
{
TRACEUSER( "Neville", _T("WriteCompressionState\n"));

	// If the preference is set then compress the file, otherwise do nothing
	if (CamelotNativeEPSFilter::GetNativeCompression())
	{
		// Get a pointer to the CCDiskFile object
		EPSExportDC *pEPSDC = (EPSExportDC *) pDC;
		if (pEPSDC == NULL || ExportFile == NULL)
			return FALSE;

		// First, see if we have enough memory to start up the compression system
		BOOL Ok = pEPSDC->ExportFile->InitCompression();
		// If this fails then don't try anything
		if (!Ok)
		{
			// Flag that we have not started up the compressor ok.
			// Do not try and save out the end compression tokens.
			CompressionInitedOk = FALSE;

			return FALSE;
		}

		// Flag that we have started up the compressor ok.
		// We can write out the end compression bits
		CompressionInitedOk = TRUE;

		// Could require some compression here...
		pDC->OutputToken("%%Compression: ");
		// The compression type that we are using.
		pDC->OutputToken("0 ");
		pDC->OutputNewLine();
		// Output an extra line to sync to 
		pDC->OutputToken("%%Compression info:");

        // Write a very simple .gz header:
		// Just output the two magic numbers plus a status word plus a flags word
		// We must output text rather than numbers and must never output LF or CR in the
		// middle as this will screw the lexer
		double StreamVersion = GZipFile::GetStreamVersionNo();
		char buf[300];
		_stprintf(buf, " %.2fD", StreamVersion);	// version in form 0.92 followed by D for deflated
		pDC->OutputToken(buf);
TRACEUSER( "Neville", _T("WriteCompressionState wrote version %.2f\n"),StreamVersion); 
		pDC->OutputNewLine();

		// Now that we have written out compression token to say that this is a compressed file,
		// start up the file compression.
		// This will write out some useful data such as compressed file version
		pEPSDC->ExportFile->SetCompression(TRUE);
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NativeRenderRegion::WriteEndCompressionState(KernelDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/05/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	This is called once everything has been output and before we fix the EPS
				header.
				In this baseclass version we do nothing as compression would be a very bad
				thing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL NativeRenderRegion::WriteEndCompressionState(KernelDC* pDC)
{
TRACEUSER( "Neville", _T("WriteEndCompressionState\n"));

	// If the preference is set then compress the file, otherwise do nothing
	// Check though that the compressor started up ok.
	if (CamelotNativeEPSFilter::GetNativeCompression() && CompressionInitedOk)
	{
		// Could require some compression ending here...
		pDC->OutputToken("%%EndCompression: ");
		pDC->OutputNewLine();
		// Output an extra line to sync to 
		pDC->OutputToken("%%EndCompressionInfo: ");
		pDC->OutputNewLine();

		// Now that we have written out compression token to say that this is a compressed file,
		// start up the file compression.
		// This will output some useful data such as a crc check and amoutn written
		// Get a pointer to the CCDiskFile object
		EPSExportDC *pEPSDC = (EPSExportDC *) pDC;
		if (pEPSDC && ExportFile)
			pEPSDC->ExportFile->SetCompression(FALSE);
	}
	
	return TRUE;
}





/********************************************************************************************

>	virtual BOOL NativeRenderRegion::WantsGrids()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Returns: 	TRUE
	Purpose:	This function is designed to help with the rendering of grids, as they are
				not always wanted (eg xara eps). If your class of render region does not
				want grids to appear, then overide this function and get it to return FALSE.
				The default behaviour is to return TRUE, which will allow grids to render
				if they want to.

********************************************************************************************/

BOOL NativeRenderRegion::WantsGrids()
{
	return TRUE;
}


/********************************************************************************************

>	virtual void NativeRenderRegion::OutputFillColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/01
	Purpose:	Used to output fill colours (in any colour model). Since the CorelXara 
				pre-1.5 format can only cope with CMYK colours (not RGB ones), we export 
				the colour as CMYK, regardless of the colour model used.

********************************************************************************************/

void NativeRenderRegion::OutputFillColour ()
{
	OutputFillCMYKColour ();
}


/********************************************************************************************

>	virtual void NativeRenderRegion::OutputStrokeColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/01
	Purpose:	Used to output stroke colours (in any colour model). Since the CorelXara 
				pre-1.5 format can only cope with CMYK colours (not RGB ones), we export 
				the colour in CMYK format, regardless of the colour model used..

********************************************************************************************/

void NativeRenderRegion::OutputStrokeColour ()
{
	OutputStrokeCMYKColour ();
}


/********************************************************************************************

>	virtual BOOL NativeRenderRegion::OutputGradFillColours (DocColour* StartCol, DocColour* EndCol, ColourContext* pContext)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/01
	Inputs:		See CamelotEPSRenderRegion (they're identical)
	Purpose:	Used to output gradient fill colours, using the CMYK clour model. This is
				necessary, as the standard EPS export uses RGB colours for gradient fills,
				which pre-1.5 CorelXara doesn't understand.

********************************************************************************************/

BOOL NativeRenderRegion::OutputGradFillColours(DocColour* StartCol, DocColour* EndCol, ColourContext* pContext)
{
	KernelDC *pDC = (KernelDC *) RenderDC;
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
	PColourCMYK CMYK;

	// Write out the start colour
	StartCol->GetCMYKValue (pContext, &CMYK);
	pDC->OutputColour (&CMYK);

	if (outputNames)
	{
		// Write out the name
		pDC->OutputColourName (StartCol);

		// Write out the tint value. This isn't actually used at the moment, but is left 
		//	for possible future expansion
		pDC->OutputValue (0l);
	}

	// And the End colour.
	EndCol->GetCMYKValue (pContext, &CMYK);
	pDC->OutputColour (&CMYK);

	if (outputNames)
	{
		// Write out the name
		pDC->OutputColourName (EndCol);

		// Write out the tint value. This isn't actually used at the moment, but is left 
		//	for possible future expansion
		pDC->OutputValue (0l);
	}

	// Return TRUE for no names used, FALSE for names used.
	return (!outputNames);
}




/********************************************************************************************

>	NativeExportDC::NativeExportDC(Filter* pFilter)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		pFilter
	Purpose:	Constructor for the NativeExportDC. This does nothing, except call the base
				class constructor.

********************************************************************************************/

NativeExportDC::NativeExportDC(Filter* pFilter) : EPSExportDC(pFilter)
{
}



/********************************************************************************************

>	virtual INT32 NativeExportDC::OutputRawBinary(BYTE* Data, UINT32 Length, UINT32 Alignment=1)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/94
	Inputs:		Data - pointer to the data to output.
				Length - the number of bytes to output.
				Alignment - the byte alignment to use - it will pad the end of
							the data with 0 bytes if Length is not divisible by
							this parameter.  Hence use 1 for no padding.
	Returns:	The number of bytes output to the file, in terms of the source data, not
				in terms of the number of ASCII characters used to represent them.
				-1 => error occured while writing => ERROR1
	Purpose:	Outputs a sequence of bytes as raw data
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

INT32 NativeExportDC::OutputRawBinary(BYTE* Data, UINT32 Length, UINT32 Alignment)
{
	// Param to note how many bytes we are writing out to the file
	INT32 nBytes = 0;

	// Start off with a blank line
	if (!OutputNewLine())
		// Error encountered
		return -1;

	// Set the mode to binary
	//ExportFile.setMode(filebuf::binary);

	// Output a character to mark the begining of the binary section
	static char Begin = '>';
	if (ExportFile->write(&Begin, 1).fail())
		// Error
		return -1;

	// Output the line of binary data
	if (ExportFile->write(Data, Length).fail())
	{
		// Error
		return -1;
	}
	
	// We saved out 'Length' bytes there
	nBytes += Length;

	// Work out the padding needed
	ENSURE(Alignment != 0, "Bad alignment in OutputRawBinary!");
	UINT32 Padding = Alignment - (Length % Alignment);
	if (Padding == Alignment)
		Padding = 0;

	if (Padding > 0)
	{
		// Put the string "00" into a Buffer
		char Buffer[2];
		Buffer[0] = 0;
		Buffer[1] = 0;

		// Output it however many times we need to
		while (Padding > 0)
		{
			if (ExportFile->write(Buffer, 1).fail())
				// Error
				return -1;

			nBytes++;
			Padding--;
		}
	}

	//output an end marker
	static char End = '<';
	if (ExportFile->write(&End, 1).fail())
		// Error
		return -1;
		
	// Set the mode to binary
	//ExportFile.setMode(filebuf::text);

	// All done
	return nBytes;
}

#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX
