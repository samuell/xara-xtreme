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

// Implementation of the ArtWorks EPS filter.

#include "camtypes.h"
#include "aw_eps.h"

#include <strstrea.h>
#include <math.h>

#include "nodepath.h"
#include "paths.h"
//#include "tim.h"
#include "oilfltrs.h"
#include "kerneldc.h"
#include "spread.h"
#include "page.h"
#include "fillattr.h"
#include "attrmgr.h"
#include "nodeblnd.h"
#include "nodebldr.h"
#include "layer.h"
#include "docview.h"
#include "nodemold.h"
#include "ndmldgrp.h"
#include "nodemldr.h"
#include "ndmldpth.h"
#include "nodeshap.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(ArtWorksEPSFilter, EPSFilter)


// This is the array of ArtWorks EPS command/keyword names.
CommandMap ArtWorksEPSFilter::ArtWorksCommands[] =
{
	EPSC_aoa,		"aoa",
	EPSC_aafs,		"aafs",
	
	// Path related procedures
	EPSC_ar,		"ar",
	EPSC_arr,		"arr",
	EPSC_ae,		"ae",
	EPSC_apl,		"apl",
	EPSC_apc,		"apc",
	EPSC_aof,		"aof",
	
	// Text related procedures
	EPSC_asto,		"asto",
	EPSC_aeto,		"aeto",
	EPSC_aco,		"aco",
	EPSC_atc,		"atc",
	EPSC_atph,		"atph",
	EPSC_atof,		"atof",
	
	// Blend related procedures
	EPSC_asbd,		"asbd",
	EPSC_aebd,		"aebd",
	EPSC_asbr,		"asbr",
	EPSC_aebr,		"aebr",
	
	// Mould related procedures
	EPSC_asev,		"asev",
	EPSC_aeev,		"aeev",
	EPSC_aspr,		"aspr",
	EPSC_aepr,		"aepr",
	EPSC_amm,		"amm",
	EPSC_aml,		"aml",
	EPSC_amc,		"amc",
	EPSC_amcp,		"amcp",
	EPSC_amep,		"amep",
	
	// Group related procedures
	EPSC_anu,		"anu",
	
	// Linear/radial fills
	EPSC_az,		"az",
	EPSC_ax,		"ax",
	EPSC_axm,		"axm",
	
	// Overprint related procedures
	EPSC_axop,		"axop",
	
	// Others(!)
	EPSC_awr,		"awr",
	EPSC_asc,		"asc",
	EPSC_aec,		"aec",
	EPSC_aca,		"aca",
	EPSC_asah,		"asah",
	EPSC_aeah,		"aeah",
	EPSC_asat,		"asat",
	EPSC_aeat,		"aeat",
	
	// Procedures that define a text object
	EPSC_atp,		"atp",
	EPSC_atf,		"atf",
	EPSC_atxy,		"atxy",
	EPSC_atrk,		"atrk",
	EPSC_akrn,		"akrn",
	
	// Layer procedure
	EPSC_alyr,		"alyr",
	
	// Sprite procedure
	EPSC_ass,		"ass",
	EPSC_aes,		"aes",

	// Sentinel
	EPSC_Invalid,	"Invalid"
};

/********************************************************************************************

>	ArtWorksEPSFilter::ArtWorksEPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an ArtWorksEPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

ArtWorksEPSFilter::ArtWorksEPSFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDT_AWEPS_FILTERNAME);
	FilterInfoID = _R(IDT_AWEPS_FILTERINFO);
	ImportMsgID  = _R(IDT_IMPORTMSG_ARTWORKS);

	FilterID = FILTERID_ARTWORKS_EPS;

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

	LastFillType = 0;
}


/********************************************************************************************

>	BOOL ArtWorksEPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an ArtWorksEPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL ArtWorksEPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new ArtWorksEPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 ArtWorksEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an ArtWorks
				generated EPS file, as required.

********************************************************************************************/

INT32 ArtWorksEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// Check the first line in EPS file
	if (_tcsncmp((char *) pFileHeader, "%!PS-Adobe-2.0 EPSF-1.2", 23) != 0)
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

		// Return TRUE if this file was created by ArtWorks
		if (_tcsncmp(Buffer, "%%Creator: ArtWorks", 19) == 0)
		{
			// ArtWorks is the creator - but has it exported it in an alien format?
			// Return 10 if it hasn't, 1 if it has.
			if (_tcsstr(Buffer, "exported") == NULL)
				return 10;
			else
				// 5 because it *might* be "ArtWorks (exported by Mr. Blobby)", and
				// we don't want to let the user not load this at all...
				return 5;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Didn't find a suitable Creator line, but we did find an EPS line, so indicate
	// that we're interested, but not sure.
	return 5;
}

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::PrepareToExport(CCLexFile* pFile, Spread *pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Inputs:		pFile - The File to export to
				pSpread - The spread to export
	Returns:	TRUE if succeeded, FALSE if not (e.g. no memory for EPS stack)
	Purpose:	Prepare to import EPS data using this filter.  This sets up the filter
				to a sensible state for reading.
	Errors:		Out of memory.
	SeeAlso:	EPSFilter::DoImport; EPSFilter::CleanUpAfterImport
	Scope: 		Private

********************************************************************************************/

BOOL ArtWorksEPSFilter::PrepareToExport(CCLexFile* pFile, Spread *pSpread)
{
#ifdef DO_EXPORT
	// Created the 'file' DC for rendering and try to open the specified file.
	ExportDCPtr = new EPSExportDC(this);
	if (ExportDCPtr == NULL) return FALSE;
	
	// Tell it about the file
	if (!ExportDCPtr->Init(pFile)) return FALSE;

	// Get the position of the first page, and use this to set the origin.
	Page *pPage = pSpread->FindFirstPageInSpread(); 
	ENSURE(pPage != NULL, "Spread has no pages");
	ERRORIF(pPage == NULL, _R(IDT_DOC_BADSTRUCTURE), FALSE);

	// Use bottom left of page as origin
	DocRect PageRect = pPage->GetPageRect();
	ExportDCPtr->SetOrigin(PageRect.lo);

	// Create a new render region to export to:

	// Don't care about clip regions when exporting - create a null region.
	DocRect NullClipRect;
	NullClipRect.MakeEmpty();

	if (IS_A(this, ArtWorksEPSFilter))
	{
		// Don't use rendering matrix when exporting EPS as it uses fractional coordinates.
		Matrix Identity;

		// Don't use view scale; set to 1
		FIXED16 Scale(1);

		// Create the region
		ExportRegion = new ArtWorksEPSRenderRegion(NullClipRect, Identity, Scale);
		if (ExportRegion == NULL)
			return FALSE;

		// Attach to the right device.
		ExportRegion->AttachDevice(DocView::GetSelected(), ExportDCPtr, pSpread);
	}

	// All ok
	return TRUE;
#else
	return FALSE;
#endif
};

/********************************************************************************************

>	void ArtWorksEPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if the token is an ArtWorks EPS token; FALSE if not.
	Purpose:	Compare the current token against the ArtWorks keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::LookUpToken; EPSFilter::DecodeToken

********************************************************************************************/

void ArtWorksEPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (ArtWorksCommands[i].Cmd != EPSC_Invalid)
	{
		if (_tcscmp(TokenBuf, ArtWorksCommands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = ArtWorksCommands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	EPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set, or
				which need to be handled differently to the standard Illustrator meanings.
				i.e. this is the function that handles all the ArtWorks EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	DocCoord 	Coords[3];
	String_64	ColName;
	PColourCMYK	Col;
	TintType	Tint = TINT_NONE;
	FIXEDPOINT	TintVal;
	INT32		Long;
	DocCoord	StartPoint, 
				EndPoint;
	PathFlags 	Flags;

	// Decode the command, and execute it...
	switch (Token)
	{
		// LineTo - overridden to overcome ArtWorks EPS incompatibility with AI EPS.
		case EPSC_l:
			if (ThePathType == PATH_DISCARD || ThePathType == PATH_DISCARD_STICKY)
			{
				if (!Stack.PopCoordPair(&Coords[0]))
					goto EPSError;
				break;
			}

			if (pInkPath == NULL)
				// Paths must start with a moveto
				goto EPSError;
			
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[0]))
			{
				// ArtWorks doesn't save smoothing bit of the point, so default to
				// no smoothing (it's the logical default).
				Flags.IsSmooth = FALSE;

				if (!pInkPath->InsertLineTo(Coords[0], &Flags))
					// Not enough dynamic heap to insert the lineto command
					goto NoMemory;
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;

		// Curveto - overridden to overcome ArtWorks EPS incompatibility with AI EPS.
		case EPSC_c:
			if (ThePathType == PATH_DISCARD || ThePathType == PATH_DISCARD_STICKY)
			{
				if (!Stack.PopCoordPair(&Coords[2]) ||
				    !Stack.PopCoordPair(&Coords[1]) ||
				    !Stack.PopCoordPair(&Coords[0]))
					goto EPSError;

				break;
			}

			if (pInkPath == NULL)
				// Paths must start with a moveto
				goto EPSError;
			
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[2]) &&
			    Stack.PopCoordPair(&Coords[1]) &&
			    Stack.PopCoordPair(&Coords[0]))
			{
				// ArtWorks doesn't save smoothing bit of the point, so default to
				// no smoothing (it's the logical default).
				Flags.IsSmooth = FALSE;

				if (!pInkPath->InsertCurveTo(Coords[0], Coords[1], Coords[2], &Flags))
					// Not enough dynamic heap to insert the curveto command
					goto NoMemory;
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;
			
		case EPSC_aoa:
			// NOT IMPLEMENTED
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_aafs:
			// NOT IMPLEMENTED
			if (!Stack.Discard(3))
				goto EPSError;
			break;
			
		
		// Path related procedures
		case EPSC_ar:
			// ArtWorks rectangle
			ThePathType = PATH_RECT;
			break;
		
		case EPSC_arr:
			// NOT IMPLEMENTED
			if (!Stack.Discard(7))
				goto EPSError;
			break;
			
		case EPSC_ae:
		{
			// ArtWorks ellipse
			ThePathType = PATH_ELLIPSE;

			// Read in parallelogram bounding box.
			if (!Stack.PopCoordPair(&ShapeBBox[0]) ||
			    !Stack.PopCoordPair(&ShapeBBox[1]) ||
			    !Stack.PopCoordPair(&ShapeBBox[2]))
				goto EPSError;

			// Fill in the 4th co-ordinate
			ShapeBBox[3].x = ShapeBBox[0].x + (ShapeBBox[2].x - ShapeBBox[1].x);
			ShapeBBox[3].y = ShapeBBox[0].y + (ShapeBBox[2].y - ShapeBBox[1].y);

			break;
		}
			
		case EPSC_apl:
			// NOT IMPLEMENTED
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_apc:
			// NOT IMPLEMENTED
			break;
			
		case EPSC_aof:
			// NOT IMPLEMENTED
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		
		// Text related procedures
		case EPSC_asto:
			// NOT IMPLEMENTED
			break;
			
		case EPSC_aeto:
			// NOT IMPLEMENTED
			break;
			
		case EPSC_aco:
			// NOT IMPLEMENTED
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		case EPSC_atc:
			// NOT IMPLEMENTED
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_atph:
			// NOT IMPLEMENTED
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_atof:
			// NOT IMPLEMENTED
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		
		// Blend related procedures
		case EPSC_asbd:
			return ProcessBlend();
			
		case EPSC_aebd:
			goto EPSError;
			
		case EPSC_asbr:
			return ProcessBlender();
			
		case EPSC_aebr:
			goto EPSError;
			
		
		// Mould related procedures
		case EPSC_asev:
			return ProcessEnvelope();
			
		case EPSC_aeev:
			goto EPSError;
			
		case EPSC_aspr:
			return ProcessPerspective();
			
		case EPSC_aepr:
			goto EPSError;
			
		case EPSC_amm:
			return ProcessMouldShape();

		case EPSC_aml:
		case EPSC_amc:
		case EPSC_amcp:
		case EPSC_amep:
			// all these tokens are mould path related and are
			// delt with inside ProcessMouldShape(). If they are found
			// on their own elsewhere then we obviously have an error
			goto EPSError;
						
		
		// Group related procedures
		
		case EPSC_anu:
			// Discard the name - we don't do named groups.
			if (!Stack.Discard())
				goto EPSError;

			// Process as a normal EPS group.
			GroupNesting++;
			return ProcessGroup();
			
		// Linear/radial fills
		case EPSC_ax:
			// Colours are described using a name and tint as well as CMYK.
			Tint = TINT_ILLUSTRATOR;
		case EPSC_az:
		{
			DocColour StartColour, EndColour;
			DocCoord StartPoint, EndPoint;

			// Get start and end positions for grad-fills
			if (!Stack.PopCoordPair(&EndPoint) || !Stack.PopCoordPair(&StartPoint))
				goto EPSError;
			
			// Get start and end colours for grad-fills
			if (!Stack.PopColour(&Col, Tint, &TintVal, &ColName))
		    	// Invalid colour operands
				goto EPSError;

			// Keep hold of this colour definition 
			LastEndColour.Col = Col;
			LastEndColour.Tint = Tint;
			LastEndColour.TintVal = TintVal;
			LastEndColour.ColName = ColName;

			GetEPSColour(&EndColour, &Col, Tint, TintVal, &ColName);

			if (!Stack.PopColour(&Col, Tint, &TintVal, &ColName))
		    	// Invalid colour operands
				goto EPSError;

			// Keep hold of this colour definition 
			LastStartColour.Col = Col;
			LastStartColour.Tint = Tint;
			LastStartColour.TintVal = TintVal;
			LastStartColour.ColName = ColName;

			GetEPSColour(&StartColour, &Col, Tint, TintVal, &ColName);
				
			// Get fill type
			if (!Stack.Pop(&Long))
				goto EPSError;

			// Also save the last fill style
			LastFillType = Long;

			switch (Long)
			{
				// Decode ArtWorks EPS grad fill codes
				case 1:
					// Check for silly grad fills first...
					if (StartPoint == EndPoint)
					{
						// Zero length grad fill line!
						// Just set a flat fill  using the first colour, cos that is
						// how Arc GDraw renders such fills.
						if (!SetFillColour(StartColour))
							goto NoMemory;
					}
					else
					{
						if (!SetLinearFill(StartColour, EndColour, StartPoint, EndPoint))
							goto NoMemory;
					}
					break;

				case 2:
					// Check for silly grad fills first...
					if (StartPoint == EndPoint)
					{
						// Zero length grad fill line!
						// Just set a flat fill  using the second colour, cos that is
						// how Arc GDraw renders such fills.
						//
						// NB. This is different to linear fills above, which use the first 
						// colour - that is how the GDraw cookie crumbles, cos that's
						// the kind of guys we are! :-)
						if (!SetFillColour(EndColour))
							goto NoMemory;
					}
					else
					{
						if (!SetRadialFill(StartColour, EndColour, StartPoint, EndPoint))
							goto NoMemory;
					}
					break;

				default:
					ENSURE(FALSE, "Unknown fill type found!");
					break;	// Don't know this fill type
			}
			break;
		}
			
		case EPSC_axm:
		{
			// ignore axm in this context
			if (!HandleMouldedFill())
				goto EPSError;
			break;
		}

		case EPSC_axop:
			// NOT IMPLEMENTED
			if (!Stack.Discard(4))
				goto EPSError;
			break;
			
		
		// Others(!)
		case EPSC_awr:
			if (!Stack.Pop(&Long))
				goto EPSError;
			// Decode winding rule
			ENSURE((Long == 0) || (Long == 1), "Bad winding rule found in Artworks EPS");
			if (Long == 0)
			{
				if (!SetWindingRule(NonZeroWinding))
					goto NoMemory;
			}
			else if (Long == 1)
			{
				if (!SetWindingRule(EvenOddWinding))
					goto NoMemory;
			}
			break;
			
		case EPSC_asc:
			// NOT IMPLEMENTED
			if (!Stack.Discard(3))
				goto EPSError;
			break;
			
		case EPSC_aec:
			// NOT IMPLEMENTED
			if (!Stack.Discard(3))
				goto EPSError;
			break;
			
		case EPSC_aca:
			// NOT IMPLEMENTED
			break;
			
		case EPSC_asah:
			// NOT IMPLEMENTED
			if (!Stack.Discard(3))
				goto EPSError;
			break;
			
		case EPSC_aeah:
			// NOT IMPLEMENTED
			if (!Stack.Discard(3))
				goto EPSError;
			break;
			
		case EPSC_asat:
			// NOT IMPLEMENTED
			if (!Stack.Discard(6))
				goto EPSError;
			break;
			
		case EPSC_aeat:
			// NOT IMPLEMENTED
			if (!Stack.Discard(6))
				goto EPSError;
			break;
			
		// Procedures that define a text object
		case EPSC_atp:
			// NOT IMPLEMENTED
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		case EPSC_atf:
			// NOT IMPLEMENTED
			if (!Stack.Discard())
				goto EPSError;
			break;
			
		case EPSC_atxy:
			// NOT IMPLEMENTED
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		case EPSC_atrk:
			// NOT IMPLEMENTED
			if (!Stack.Discard(4))
				goto EPSError;
			break;
			
		case EPSC_akrn:
			// NOT IMPLEMENTED
			if (!Stack.Discard(2))
				goto EPSError;
			break;
			
		
		// Layer procedure
		case EPSC_alyr:
		{
			String_256 LayerName;
			INT32 Visible;
			INT32 Locked;
			INT32 Foreground;
			INT32 Printable;

			// We ignore the layer type and print over-ride flag (for now)
			if (!Stack.Pop(&Locked)  	|| 
				!Stack.Pop(&Printable)	|| 
				!Stack.Pop(&Visible) 	|| 
				!Stack.Pop(&Foreground)	|| 
				!Stack.Pop(&LayerName))
				goto EPSError;

			if (Filter::ImportWithLayers)
			{
				// We are importing layers, so put all new nodes on this layer.
				UseLayer(LayerName);

				// Try to set the visible/locked flags on this layer, but only if
				// we created a new one (i.e. don't change the flags of existing layers.
				if (EPSFlags.AddToNewLayer)
				{
					// Here we force layers to be printable if they are in the foreground - just in case
					// This will have to change if the UI allows Printable & Foreground to be specified
					// independantly.
					Printable = Foreground;

					// Visible flag
					pLayer->SetVisible(!(Visible == 0));

					// Locked flag
					pLayer->SetLocked(!(Locked == 0));

					// Printable flag
					pLayer->SetPrintable(!(Printable == 0));

					// Background flag
					pLayer->SetBackground(Foreground == 0);
				}
			}
			break;
		}
			
		
		// Sprite procedure
		case EPSC_ass:
			// NOT IMPLEMENTED
			ERROR2RAW("EPS reader cannot handle Acorn sprites - Dream On!");
			goto EPSError;
			break;
			
		case EPSC_aes:
			// NOT IMPLEMENTED
			ERROR2RAW("EPS reader cannot handle Acorn sprites! - Dream On!");
			goto EPSError;
			break;
			
		default:
			// Token not understood - pass on to base class
			return EPSFilter::ProcessToken();
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

>	char *ArtWorksEPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_aoa
	Returns:	Pointer to the string representation of the token, e.g. "aoa"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

char *ArtWorksEPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (ArtWorksCommands[i].Cmd != EPSC_Invalid)
	{
		if (ArtWorksCommands[i].Cmd == Cmd)
			return ArtWorksCommands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return EPSFilter::GetEPSCommand(Cmd);
}

/********************************************************************************************

>	virtual BOOL ArtWorksEPSFilter::NeedsPrintComponents ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/5/00
	Returns:	FALSE - ArtWorks files don't need print components.
	Purpose:	Informs the print components code that no data should be written.

********************************************************************************************/

BOOL ArtWorksEPSFilter::NeedsPrintComponents ()
{
	// We don't want print components!
	return FALSE;
}

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ProcessBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the blend was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a blend structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessBlend()
{
	if (!StartBlend())
		return FALSE;

	// Keep processing tokens until we find the end of the blend
	do
	{
		GetToken();

		// Look for the end of the blend token...
		if (Token == EPSC_aebd)
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

>	BOOL ArtWorksEPSFilter::StartBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the new blend was created ok;
				FALSE if not.
	Purpose:	Used when a blend structure needs to be created - after this is called,
				all new nodes added with AddNewNode() will be added as children of this
				new blend, until EndBlend is called.
	Errors:		Out of memory
	SeeAlso:	ArtWorksEPSFilter::EndBlend

********************************************************************************************/

BOOL ArtWorksEPSFilter::StartBlend()
{
	INT32 NumBlendShapes,Version,Expanded;
	
	// Read in the params
	BOOL 	ok = Stack.Pop(&NumBlendShapes);
	if (ok) ok = Stack.Pop(&Expanded);
	if (ok) ok = Stack.Pop(&Version);
	if (!ok) 
	{
		HandleEPSError();
		return FALSE;
	}

	// If the version isn;t one we know, go b-b-b-bang!
	ERROR1IF(Version != 1,FALSE,_R(IDT_EPS_BADSYNTAX));

	// Make a new blend node for this blend
	NodeBlend *pBlend = new NodeBlend;
	if (pBlend==NULL)
	{
		HandleNoMemory();
		return FALSE;
	}

	// Add it into the tree
	if (!AddNewNode(pBlend))
	{
		HandleNoMemory();
		return FALSE;
	}

	// Make sure new objects are added as children of the node
	pNode = pBlend;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::EndBlend()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the blend was ended ok;
				FALSE if not.
	Purpose:	Used when a blend has finished being constructed and we want to return to
				normal node positioning.  The blend is added to the document when this
				function is called (although that depending on the current filter mode, 
				i.e. whether this is a new or existing layer, it may not be added
				directly to the documnent tree straight away - it may be deferred until
				the next layer is found, or the import has ended).
	Errors:		Out of memory
	SeeAlso:	ArtWorksEPSFilter::StartBlend

********************************************************************************************/

BOOL ArtWorksEPSFilter::EndBlend()
{
	// Sanity check
	ENSURE(pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBlend)), "No blend in ArtWorksEPSFilter::EndBlend");

	// Keep the blend ptr and find the first child of the node before we reset pNode.
	Node* pBlend     = pNode;
	Node* pChildNode = pNode->FindFirstChild();

	// Get the parent of the blend node, and use that to add new objects to
	pNode = pBlend->FindParent();
	ERROR2IF(pNode==NULL, FALSE, "Blend has no parent in EndBlend()");

	// Initialise all the child blender nodes
	UINT32 BlenderCount=0;
	while (pChildNode != NULL)
	{
		if (IS_A(pChildNode,NodeBlender))
		{
			NodeBlender* pNodeBlender = (NodeBlender*)pChildNode;
			INT32 PathIndexStart = pNodeBlender->GetPathIndexStart();
			INT32 PathIndexEnd   = pNodeBlender->GetPathIndexEnd();

			// Convert the AW path indexes into Camelot path indexes
			if (!pNodeBlender->ConvertAWPathIndexesToCamelot(&PathIndexStart,&PathIndexEnd))
			{
				ERROR2(FALSE,"Unable to convert artworks path indexes in EndBlend()");
			}

			// Init the blender to blend the prev and next ink nodes together
			if (!pNodeBlender->Initialise(NULL,NULL,PathIndexStart,PathIndexEnd,ImportInfo.pOp,NULL,TRUE))
			{
				ERROR2(FALSE,"Unable to initialise a node blender in EndBlend()");
			}

			BlenderCount++;
		}
		pChildNode = pChildNode->FindNext();
	}

	if (BlenderCount == 0)
	{
		ERROR3("Imported Blend node doesn't contain any blender nodes");
		if (!ImportInfo.pOp->DoHideNode(pBlend, TRUE))
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ProcessBlender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the blender was processed correctly, FALSE if not.
	Purpose:	Reads in all the elements of a blender structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessBlender()
{
	if (!StartBlender())
		return FALSE;

	// Keep processing tokens until we find the end of the blender
	do
	{
		GetToken();

		// Look for the end of the blender token...
		if (Token == EPSC_aebr)
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

>	BOOL ArtWorksEPSFilter::StartBlender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the new blender was created ok;
				FALSE if not.
	Purpose:	Used when a blender structure needs to be created - after this is called,
				all new paths will be discarded, until EndBlender is called.
	Errors:		Out of memory
	SeeAlso:	ArtWorksEPSFilter::EndBlender

********************************************************************************************/

BOOL ArtWorksEPSFilter::StartBlender()
{
	UINT32 NumBlendSteps;
	INT32 Expanded,OneToOne,Complex;
	INT32 PathIndexStart,PathIndexEnd;
	
	// Get the blender params
	BOOL 	ok = Stack.Pop(&PathIndexEnd);
	if (ok) ok = Stack.Pop(&PathIndexStart);
	if (ok) ok = Stack.Pop(&NumBlendSteps);
	if (ok) ok = Stack.Pop(&OneToOne);
	if (ok) ok = Stack.Pop(&Complex);
	if (ok) ok = Stack.Pop(&Expanded);
	if (!ok)
	{
		HandleEPSError();
		return FALSE;
	}

	// Make a new blender node for this blender
	NodeBlender *pNodeBlender = new NodeBlender;
	if (pNodeBlender == NULL)
	{
		HandleNoMemory();
		return FALSE;
	}

	// Add it into the tree
	if (!AddNewNode(pNodeBlender))
	{
		delete pNodeBlender;
		HandleNoMemory();
		return FALSE;
	}

	// If both indexes are -ve, don't change the start points of the paths when blending
	if (PathIndexEnd < 0 && PathIndexEnd < 0)
		PathIndexStart = PathIndexEnd = 0;

	// Set the AW path indexes to be used when we get an "aebd" (end blend) token
	pNodeBlender->SetPathIndexStart(PathIndexStart);
	pNodeBlender->SetPathIndexEnd  (PathIndexEnd);

	// Some of these flags are stored in the parent blend node in Camelot - so set them!
	if (!IS_A(pNode,NodeBlend))
	{
		ERROR2(FALSE,"Found a blender token without finding a blend");
	}

	NodeBlend* pNodeBlend = (NodeBlend*)pNode;
	pNodeBlend->SetNumBlendSteps(NumBlendSteps-1);
	pNodeBlend->SetOneToOne(OneToOne);

	// Discard all paths between this and the aebr tokens
	ThePathType = PATH_DISCARD_STICKY;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::EndBlender()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/94
	Returns:	TRUE if the blender was ended ok;
				FALSE if not.
	Purpose:	Used when a blender has finished being constructed.
				Paths are not longer discarded
	Errors:		-
	SeeAlso:	ArtWorksEPSFilter::StartBlender

********************************************************************************************/

BOOL ArtWorksEPSFilter::EndBlender()
{
	ThePathType = PATH_NORMAL;
	return TRUE;
}



/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ProcessEnvelope()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Returns:	TRUE if the envelope object was processed correctly, 
				FALSE if not.
	Purpose:	Reads in all the elements of an envelope structure in the ArtWorksEPS file 
				and builds the camelot envelope tree
	Errors:		Syntax error in EPS file, Out of memory

	Notes:		This reader is reasonably complex. It is made so soley by the problem of
				being unable to calculate an inverse transform of an envelope. The crux of
				the matter is this. If you apply attributes to an already enveloped set of
				objects (lets call them the Dset), then in some way the attributes need to
				be applied to the source objects (the Sset). Now when you do this in ArtWorks
				eg, apply a linear fill to the Dset, the same fill is exported as applied to
				the Sset. Import the file back in and ArtWorks rebuilds both Sset and Dset
				from the file applying all necessary attributes there in. This works. But
				now we have a Dset attribute applied to an Sset object. As soon as you
				regenerate the Dset from the Sset (by tweeking the envelope shape) the new
				attributes created are wrong, as it transforms already transformed 
				attributes.
				Ok, this problem prevents me from simply loading all Sset members from the
				file and recreating the Dset automatically. The loaded image does not look
				correct when grad fills are applied. (ie more often than not).
				Complexity level 1.
				To combat this problem, I'll do as ArtWorks did and load both Sset & Dset
				members. However, another problem arrises here....
				ArtWorks envelope structure	is as follows

				  line col
				  fill col
				  start env
				     envelope shape
					 fill col
					 invisible path	  (Sset)
						mangled path  (Dset)
					 fill col
					 invisible path	  (Sset)
					 	mangled path  (Dset)
				  end env

				and this is saved as such in its EPS structure. Now Camelots structure is
				slightly different. It creates a source tree for Sset and a destination tree
				for Dset. Hence we need to disassemble the AW structure into two trees with
				the correct attributes in each.
				How do we do this?
				Complexity level 2.
				We could do this while importing, keeping two trees on the go, importing the
				correct bits into the correct tree. However this has many problems associated
				with it. Globally applied atts will need to be created in both trees. Groups too.
				We would have to intercept far more than the envelope tokens during import to
				do it. - no chance.
				So the only way to do it is to import the structure as is, creating an Artworks
				envelope inside Camelot. This will be structured as follows

				  Env
				   |
				   > EnvShape => MouldGroup => Moulder
									 |
									 > { Sset + Dset objects }


				Having done this, we will convert this into

				  Env
				   |
				   > EnvShape => MouldGroup => Moulder
									 |			 |
									 > Sset      > Dset

				So long as we can tell the difference between Sset and Dset objects, which
				we can (all Dset objs are primary children of an Sset obj and Dset objects
				never contain Sset objs as children) we can use object move technology to
				build the correct moulder. Attributes will move (via inheritance) correctly 
				with objects.

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessEnvelope()
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

>	BOOL ArtWorksEPSFilter::ProcessPerspective()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Returns:	TRUE if the perspective object was processed correctly, 
				FALSE if not.
	Purpose:	Reads in all the elements of an perspective structure in the EPS file.
	Errors:		Syntax error in EPS file, Out of memory
	SeeAlso:	ProcessEnvelope() for furher details

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessPerspective()
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

>	BOOL ArtWorksEPSFilter::StartMould(MouldSpace mSpace)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> (from MarkN's)
	Created:	06/03/95
	Inputs:		mSpace = Mould geometry to define, either envelope or perspective
	Returns:	TRUE if the new mould object was created ok;
				FALSE if not.
	Purpose:	Used when an mould structure needs to be created - after this is called,
				all new nodes added with AddNewNode() will be added as children of this
				new mould, until EndMould is called.
	Errors:		Out of memory
	SeeAlso:	ArtWorksEPSFilter::EndMould

********************************************************************************************/

BOOL ArtWorksEPSFilter::StartMould(MouldSpace mSpace)
{
	// Read in the bounding box of the original objects
	if (!Stack.PopCoordPair(&MouldRect.hi) || 
		!Stack.PopCoordPair(&MouldRect.lo))
	{
		HandleEPSError();
		return FALSE;
	}

	// create a mould parent and insert it in the tree
	NodeMould* pMouldParent = new NodeMould;
	if (pMouldParent==NULL)
	{
		HandleNoMemory();
		return FALSE;
	}

	// give the parent mould object a shape and mould space to work with and stick it
	// in the tree
	if (!pMouldParent->CreateGeometry(mSpace))
	{
		delete pMouldParent;
		HandleNoMemory();
		return FALSE;
	}

	if (!AddNewNode(pMouldParent))
	{
		HandleNoMemory();
		return FALSE;
	}
			
	// Make sure new objects are added as children of the mould
	pNode = pMouldParent;

	// All ok
	return TRUE;
}



/********************************************************************************************

>	BOOL ArtWorksEPSFilter::EndEnvelope()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Returns:	TRUE if the envelope was ended ok;
				FALSE if not.
	Purpose:	Used when a envelope has finished being constructed and we want to return to
				normal node positioning.
	SeeAlso:	ArtWorksEPSFilter::StartEnvelope()

********************************************************************************************/

BOOL ArtWorksEPSFilter::EndMould()
{
	// Sanity check
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeMouldGroup)), "No mouldgroup in ArtWorksEPSFilter::EndMould");
	NodeMouldGroup* pNodeMGroup = (NodeMouldGroup*)pNode;
	pNode = pNode->FindParent();
	ERROR3IF(!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeMould)), "No MouldParent in ArtWorksEPSFilter::EndMould");
	NodeMould* pNodeMould = (NodeMould*)pNode;

	NodeMoulder* pMoulder = pNodeMould->CreateNewMoulder(NULL);
	if (!pMoulder)
	{
		HandleNoMemory();
		return FALSE;
	}

	// Add it into the tree as the last child of the mould
	if (!AddNewNode(pMoulder))
	{
		HandleNoMemory();
		return FALSE;
	}

	// Right, now we need to scan through all moulded objects and copy their fill and
	// stroke styles to the originals. We need to do this because the original objects
	// within the file are terminated by h and H functions. These describe closed and open
	// (none filled,none stroked) paths ie invisible paths. Hence to create the correct
	// original styles we need to take the definitions from the destination objects
	// instead. This means when we edit the imported mould the drawing doesn't suddenly
	// take on random fill and stroke styles.

	// Right here comes the difficult bit. What we have now is a really interresting
	// structure. We should have a complete tree beneath the mould group node. This tree
	// contains invisible source objects and visible moulded shapes. So if we we're to
	// render this tree, it would look sensible. Now we need to disassemble this tree
	// by placing all the moulded objects and their attributes inside our moulder object.
	// This we will do by copying the entire tree and deleting the right bits. (Isn't it
	// yuk but there you go, its the only sensible and simple way of doing it, without
	// massively duplicating bits of code.

	// go and convert the artworks tree
	if (!ConvertArtMould(pNodeMould))
	{
		HandleNoMemory();
		return FALSE;
	}

	// and finally move pNode up one level
	pNode = pNode->FindParent();
	ERROR3IF(pNode==NULL, "No parent of envelope in ArtWorksEPSFilter::EndMould");

	return TRUE;
}


/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ProcessMould()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/95
	Returns:	TRUE if the mould was processed ok;
				FALSE if not.
	Purpose:	Processes tokens inside a mould object. Tokens specific to mould objects
				are processed along with some overridding of default tokens such as groups
	SeeAlso:
	
********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessMould()
{
	// Keep processing tokens until we find the end of the envelope
	while (!EPSFile->eof())
	{
		GetToken();

		// Look for the end of the envelope token...
		switch (Token)
		{
			case EPSC_aeev:
			case EPSC_aepr:
				// if we've found the envelope/perspective end then
				// all is well.
				return TRUE;
				break;

			case EPSC_apc:
				// Create next object as a child
				if (!ProcessMangledObjs())
					return FALSE;
				break;

			case EPSC_u:
				// NOT IMPLEMENTED INSIDE MOULDS
				break;

			case EPSC_U:
				// NOT IMPLEMENTED INSIDE MOULDS
				break;

			case EPSC_aof:
				// don't need this token
				if (!Stack.Discard(2))
				{
					HandleEPSError();
					return FALSE;
				}
				break;

			case EPSC_h:
				Token = EPSC_b;
				if (!HandleToken())
					return FALSE;
				break;

			case EPSC_H:
				Token = EPSC_B;
				if (!HandleToken())
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

>	BOOL ArtWorksEPSFilter::ProcessMouldShape()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Returns:	TRUE if the shape has been read correctly, 
				FALSE if not.
	Purpose:	Reads in all the elements of a mould geometry held in the EPS file.
				The description of the geometry for this particular file type is that of
				a path. Depending on the geometry type (perspective or envelope) we will
				obviously expect a slightly different path. The rules are, that currently
				perspective geometry is described as {m,l,l,l,l} and envelope geometry
				is {m,c,c,c,c}. Internally we are not restricted to these but ArtworksEPS
				certainly is.
				Input Stack = coord, coord
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessMouldShape()
{
	// We really should be inside a mould object at this stage
	if (!IS_A(pNode,NodeMould)) 
	{
		ERROR3("Mould shape found outside a mould object");
		return FALSE;
	}

	NodeMould* pNodeMould = (NodeMould*)pNode;

	// Create a path object to read the mould geometry into
	Path TempPath;
	if (!TempPath.Initialise(24,12))
	{
		HandleNoMemory();
		return FALSE;
	}

	// Set the current used slots to none
	INT32 numt = 0;
	DocCoord Ta,Tb,Tc;
	PathFlags flags;
	flags.IsSelected = FALSE;

	// ok, pop off the move	stacked coordinate
	BOOL ok = Stack.PopCoordPair(&Ta);
	if (!ok)
	{
		HandleEPSError();
		return FALSE;
	}

	// Add this first element as a move to
	if (!TempPath.AddMoveTo(Ta, &flags))
	{
		HandleNoMemory();
		return FALSE;
	}

	// increase the number of tokens read
	numt++;
	BOOL done = FALSE;

	// Keep processing tokens until we find the end of the mould path
	while (!done && (!EPSFile->eof()))
	{
		GetToken();

		// {m,c,c,c,c,c,c,c,c,e} is current maximum
		// if more complex, then illegal
		if ((numt++)>60)
		{
			HandleEPSError();
			return FALSE;
		}

		switch (Token)
		{
			case EPSC_aml:
				// lineto (x,y) found
						ok = Stack.PopCoordPair(&Ta);
				if (ok) ok = TempPath.AddLineTo(Ta, &flags);
				break;
												
			case EPSC_amc:
				// curveto (x0,y0,x1,y1,x2,y2) found
						ok = Stack.PopCoordPair(&Tc);
				if (ok) ok = Stack.PopCoordPair(&Tb);
				if (ok)	ok = Stack.PopCoordPair(&Ta);
				if (ok) ok = TempPath.AddCurveTo(Ta,Tb,Tc,&flags);
				break;

			case EPSC_amcp:
				// found a close path. That should be it for path elements
				ok = TempPath.CloseSubPath();
				break;

			case EPSC_amep:
				// end path drawfile parameter completes the path
				// it contains a single 'length' parameter which seems
				// to be zero always
				ok = Stack.Discard();
				done = TRUE;
				break;

			default:
				// try to handle whatever token this is.
				ok = HandleToken();
				break;
		}

		if (!ok)
		{
			HandleNoMemory();
			return FALSE;
		}
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
		{
			ERROR2(FALSE,"Unable to rectify invalid mould path in ProcessMouldShape()");
		}
 
		if (!TempPath.CloneFrom(*pPath))
		{
			delete pPath;
			HandleNoMemory();
			return FALSE;
		}

		delete pPath;
	}

	// Build and if necessary fit the geometry
	NodeMouldPath* pNodeMPath = pNodeMould->CreateNewMouldShape(&TempPath,NULL,NULL);
	if (pNodeMPath==NULL) 
	{
		HandleNoMemory();	
		return FALSE;
	}

	// set the geometry using this new mould shape
	if (!pNodeMould->GetGeometry()->Define(&(pNodeMPath->InkPath), &MouldRect))
	{
		HandleNoMemory();
		return FALSE;
	}

	// having created the shape bung it in the tree.
	if (!AddNewNode(pNodeMPath))
	{
		delete pNodeMPath;
		HandleNoMemory();
		return FALSE;
	}

	// Create a mould group object
	NodeMouldGroup*	pMouldGroup = pNodeMould->CreateNewMouldGroup(NULL);
	if (pMouldGroup==NULL)
	{
		HandleNoMemory();
		return FALSE;
	}

	if (!AddNewNode(pMouldGroup))
	{
		delete pMouldGroup;
		HandleNoMemory();
		return FALSE;
	}

	// finally leave the insert pointer on the MouldGroup object
	pNode = pMouldGroup;

	return TRUE;

}



/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ProcessMangledObjs()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Returns:	TRUE if the mangled object has been dealt with correctly
				FALSE if not.
	Purpose:	On finding a 'apc' token in the input stream, this function is executed.
				It will process all further tokens in the stream until a path termination
				has been found. A path terminator is considered any of { s,S,b,B,f,F,h,H }
				Its purpose is strictly defined. It must place the next set of attributes
				and single path as children of the last object imported rather than the 
				default action which would be to stick them in as siblings of the object.

	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL ArtWorksEPSFilter::ProcessMangledObjs()
{
	// Set the new position of pNode, our insert indicator
	Node *pOldPos = pNode;
	pNode=pNode->FindLastChild();
	if (pNode==NULL)
	{
		pNode=pOldPos;
		return FALSE;
	}

	BOOL ok = TRUE, end = FALSE;

	// The next path we create should be mangled
	ThePathType = PATH_MANGLED;

	while ((!end) && (!EPSFile->eof()) && (ok))
	{
		GetToken();

		switch (Token)
		{
			case EPSC_axm:
				// ignore axm in this context
				// See notes on func HandleMouldedFill below
				ok = Stack.Discard(4);
				break;
				
			case EPSC_s: case EPSC_f: case EPSC_b: case EPSC_h:
			case EPSC_S: case EPSC_F: case EPSC_B: case EPSC_H:
				// We've finished so set term flag
				end = TRUE;
				ok = HandleToken();
				break;

			default:
				// try to handle whatever token it was.
				ok = HandleToken();
				break;
		}
	}

	// Set the path back to something sensible
	ThePathType=PATH_NORMAL;
	pNode=pOldPos;

 	if (ok && !(EPSFile->eof())) 
		return TRUE;

	// Arrgh didn't find an end-of-path token - syntax error; deal with it
	HandleEPSError();
	return FALSE;
}



/********************************************************************************************

>	BOOL ArtWorksEPSFilter::HandleMouldedFill()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Inputs:		-
	Returns:	TRUE if the mangled fill has been dealt with correctly
				FALSE if not.
	Purpose:	On finding a 'axm' token in the input stream, this function is executed.
				It will build a graduated fill structure and set it as the current fill
				style to be applied to future objects. The function has been split off from
				the main structure for one very important reason. Fills applied to moulds
				work in a random way. Imagine the senario, you have a rectangle, and you
				envelope it. You then apply a linear grad fill to it. If this gets exported
				as AWEPS what you get are two objects, one moulded and one rectangle. Now
				you also get a moulded and none moulded fill style represented by ax and
				axm. Unfortunately although ax stands for 'standard fill style' and axm is
				used as an override describing the coordinates of a moulded fill (hence the
				m) the coordinates of both fills are reversed. DONT ASK ME WHY, ASK THE
				AUTHOR OF AWEPS! So we have to perform some brain dead processing here to
				get the right attributes applied to the right objects. 				

	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL ArtWorksEPSFilter::HandleMouldedFill()
{

	BOOL ok;

	DocCoord StartPoint, EndPoint;
	DocColour StartColour, EndColour;

	// Get start and end positions for grad-fills
	if (!Stack.PopCoordPair(&EndPoint) || !Stack.PopCoordPair(&StartPoint))
		return FALSE;

	// read the last definitions of set colours
	if (LastFillType>0)
	{
		GetEPSColour(&StartColour,
					 &LastStartColour.Col, 
					 LastStartColour.Tint,
					 LastStartColour.TintVal,
					 &LastStartColour.ColName);

		GetEPSColour(&EndColour,
					 &LastEndColour.Col, 
					 LastEndColour.Tint,
					 LastEndColour.TintVal,
					 &LastEndColour.ColName);
	}

	switch (LastFillType)
	{
		// Decode ArtWorks EPS grad fill codes
		case 1:
			// Check for silly grad fills first...
			if (StartPoint==EndPoint)
				ok = SetFillColour(StartColour);
			else
				ok = SetLinearFill(StartColour, EndColour, StartPoint, EndPoint);
			break;

		case 2:
			// Check for silly grad fills first...
			if (StartPoint==EndPoint)
				ok=SetFillColour(EndColour);
			else
				ok=SetRadialFill(StartColour, EndColour, StartPoint, EndPoint);
			break;

		default:
			// if there's no fill applied previously then this is rampant so ignore it.
			ok=Stack.Discard(4);
	}
	
	return ok;
}

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ConvertMouldStyles(Node* pNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Inputs:		pNode - a pointer to an object to start scanning from
	Returns:	TRUE if the mould styles have been converted correctly
				FALSE if not.
	Purpose:	Scan through all moulded objects, copying their fill and stroke styles
				to the original objects (which are currently their parents)

********************************************************************************************/

BOOL ArtWorksEPSFilter::ConvertMouldStyles(Node* pNode)
{
	Node *qNode, *rNode;
	BOOL IsFilled = FALSE, IsStroked = FALSE;

	while (pNode)
	{
		qNode=pNode;
		pNode=pNode->FindNext();

		// if its mangled delete it otherwise scan its kids
		if (qNode->IsMangled())
		{
			rNode = qNode->FindParent();
			if (rNode)
			{
				// this should be polymorphic but there's no virtual func to
				// do it so I'll have to be explicit.
				if (IS_A(qNode, NodePath))
				{
					IsFilled  = ((NodePath*)qNode)->InkPath.IsFilled;
					IsStroked =	((NodePath*)qNode)->InkPath.IsStroked;

					if (rNode->IsKindOf(CC_RUNTIME_CLASS(NodePath)))
					{
						((NodePath*)rNode)->InkPath.IsFilled  = IsFilled;
						((NodePath*)rNode)->InkPath.IsStroked = IsStroked;
					}
					else if (rNode->IsKindOf(CC_RUNTIME_CLASS(NodeSimpleShape)))
					{
						((NodePath*)rNode)->InkPath.IsFilled  = IsFilled;
						((NodePath*)rNode)->InkPath.IsStroked = IsStroked;
					}
				}
			}
		}
		else
			ConvertMouldStyles(qNode->FindFirstChild());
	}
	return TRUE;
}


/********************************************************************************************

>	BOOL ArtWorksEPSFilter::ConvertArtMould(NodeMould* pNodeMould)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/03/94
	Inputs:		pNodeMould - a pointer to the mould object
	Returns:	TRUE if the artworks mould has been converted correctly
				FALSE if not.
	Purpose:	Right here comes the difficult bit. What we have now is a really interresting
				structure. We should have a complete tree beneath the mould group node 
				containing invisible source objects and visible moulded shapes. So if we were to
				render this tree, it would look sensible. Now we need to disassemble this tree
				by placing all the moulded objects and their attributes inside our moulder object.
				This we will do by copying the entire tree and deleting the right bits. (Isn't it
				yuk but there you go, its the only sensible and simple way of doing it, without
				massively duplicating bits of code.
	Errors:		Syntax error in EPS file, Out of memory

********************************************************************************************/

BOOL ArtWorksEPSFilter::ConvertArtMould(NodeMould* pNodeMould)

{
	ERROR2IF(pNodeMould==NULL, FALSE, "NULL mould pointer passed to ConvertArtMould()");

	NodeMouldGroup* pNodeMGroup = pNodeMould->FindMouldGroup();
	if (pNodeMGroup==NULL)
		return FALSE;

	NodeMoulder* pMoulder = pNodeMould->FindFirstMoulder();
	if (pMoulder==NULL)
		return FALSE;

	Node *pNode;
	// Stage 1
	// Copy all objects from the mould group to this new moulder object

	pNode = pNodeMGroup->FindFirstChild();
	while (pNode)
	{
		if (!pNode->CopyNode(pMoulder,LASTCHILD))
		{
			pMoulder->CascadeDelete();
			return FALSE;
		}
		pNode=pNode->FindNext();
	}

	// Stage2
	// Scan through deleting all mangled objects in the source tree
	pNode=pNodeMGroup->FindFirstChild();
	DeleteAllMangled(pNode);

	//Stage 3
	//Scan through deleting all none mangled objects in the destination tree
	//but leaving the mangled versions!
	pNode=pMoulder->FindFirstChild();
	DeleteAllNoneMangled(pNode);

	return TRUE;
}



/********************************************************************************************

>	void ArtWorksEPSFilter::DeleteAllMangled(Node* pNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Inputs:		pNode = a pointer to a node to start deleting from
	Returns:	-
	Purpose:	Delete all nodes with their mangled bits set at and below pNode
	SeeAlso:

********************************************************************************************/

void ArtWorksEPSFilter::DeleteAllMangled(Node* pNode)
{
	Node *qNode;

	while (pNode)
	{
		qNode=pNode;
		pNode=pNode->FindNext();

		// if its mangled delete it otherwise scan its kids
		if (qNode->IsMangled())
		{
			qNode->CascadeDelete();
			delete qNode;
		}
		else
			DeleteAllMangled(qNode->FindFirstChild());
	}
}
		
/********************************************************************************************

>	void ArtWorksEPSFilter::DeleteAllNoneMangled(Node* pNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Inputs:		pNode = a pointer to a node to start deleting from
	Returns:	-
	Purpose:	Delete all renderable parents of mangled objects from pNode onwards.
				This really is a specialised function and should be used with care. It
				scans through the tree, hunting for mangled objects. When one is found
				it will attempt to promote the node to become a sibling of its parent and
				delete the parent. It will only do so if the parent is renderable.
				We know or are assuming this is a confined and sensible action. It is only
				so having loaded an ArtWorksEPS envelope or perspective object.
	SeeAlso:

********************************************************************************************/

BOOL ArtWorksEPSFilter::DeleteAllNoneMangled(Node* pNode)
{
	Node* qNode;
	while (pNode)
	{
		qNode=pNode;
		pNode=pNode->FindNext();
		if (!PromoteMangled(qNode))
			DeleteAllNoneMangled(qNode->FindFirstChild());
	}
	return TRUE;
}	

/********************************************************************************************

>	BOOL ArtWorksEPSFilter::PromoteMangled(Node* rNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/03/95
	Inputs:		rNode = a pointer to a mangled node to promote
	Returns:	-
	Purpose:	Promote all mangled child nodes to siblings of their parents.

********************************************************************************************/

BOOL ArtWorksEPSFilter::PromoteMangled(Node* rNode)
{
	Node* sNode;
	sNode=rNode->FindFirstChild();
	while (sNode)
	{
		if (sNode->IsMangled())
		{
			sNode->SetMangled(FALSE);
			sNode->MoveNode(rNode,NEXT);
			rNode->CascadeDelete();
			delete rNode;
			return TRUE;
		}
		sNode=sNode->FindNext();
	}
	return FALSE;
}

/********************************************************************************************

>	BitmapFilterSupport ArtWorksEPSFilter::GetBitmapSupportLevel()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/01/95
	Returns:	SimpleBitmapSupport - This filter has simple support for bitmap images;
									  they must be saved into the file whenever
									  they are used.
	Purpose:	Determine how well this filter supports bitmaps when exporting.

********************************************************************************************/

BitmapFilterSupport ArtWorksEPSFilter::GetBitmapSupportLevel()
{
	// ArtWorks EPS has simple bitmap support
	return SimpleBitmapSupport;
}

////////////////////////////////////////////////////////////////////////////////////////////
// ArtWorksEPSRenderRegion methods.

CC_IMPLEMENT_DYNAMIC(ArtWorksEPSRenderRegion, EPSRenderRegion)

/********************************************************************************************

>	ArtWorksEPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Purpose:	Initialise a render region for exporting ArtWorks EPS. Sets up the
				string to put in the %%Creator comment.
	SeeAlso:	EPSRenderRegion::EPSRenderRegion

********************************************************************************************/

ArtWorksEPSRenderRegion::ArtWorksEPSRenderRegion(DocRect ClipRect,
												 Matrix ConvertMatrix, 
												 FIXED16 ViewScale) 
	: EPSRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
	CreatorString = "ArtWorks";
}


/********************************************************************************************

>	BOOL ArtWorksEPSRenderRegion::StartRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/94
	Returns:	TRUE if render region started rendering mechanisms ok.
	Purpose:	Over-rides normal EPS StartRender() function - this is because we want to
				have a different default winding rule when exporting as ArtWorks EPS.

********************************************************************************************/

BOOL ArtWorksEPSRenderRegion::StartRender()
{
#ifdef DO_EXPORT
	// Check the base class is working ok.
	if (!EPSRenderRegion::StartRender())
		return FALSE;

	// Set up normal ArtWorks default rendering - basic difference is that ArtWorks
	// uses even-odd winding rule.
	// NB. it only does it for 'proper' ArtWorks render regions, not any derived
	//	   from this class.
	if (IS_A(this, ArtWorksEPSRenderRegion))
		SetWindingRule(EvenOddWinding);

	// All ok
	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	void ArtWorksEPSRenderRegion::GetValidPathAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	See EPSRenderRegion::GetValidPathAttributes.
				This version checks and handles grad fill colours before calling the
				base class version.
	SeeAlso:	EPSRenderRegion::GetValidPathAttributes

********************************************************************************************/

void ArtWorksEPSRenderRegion::GetValidPathAttributes()
{
#ifdef DO_EXPORT
	KernelDC *pDC = (KernelDC *) RenderDC;

	FillGeometryAttribute *pFillAttr = (FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	if ((pFillAttr->IsKindOf(CC_RUNTIME_CLASS(GradFillAttribute))) &&
		SetLastOutputAttribute(ATTR_FILLGEOMETRY))
	{
		// Output a grad fill command...

		// Output the fill type
		if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(LinearFillAttribute)))
		{
			pDC->OutputToken("1");
		}
		else if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(RadialFillAttribute)))
		{
			pDC->OutputToken("2");
		}
		else
		{
			// Bodge: ought to handle this more gracefully soon...i.e. get the
			// fill provider to render the fill using normal path rendering code.
			ENSURE(FALSE, "Unsupported grad fill encountered while exporting");
			EPSRenderRegion::GetValidPathAttributes();
			return;
		}

		// Get the start colour and end colour in CMYK, and output them.
		GradFillAttribute *pGradFillAttr = (GradFillAttribute *) pFillAttr;
		pDC->OutputNamedColour(&pGradFillAttr->Colour);
		pDC->OutputNamedColour(&pGradFillAttr->EndColour);
	
		// Output the start and end points of the grad fill
		pDC->OutputCoord(pGradFillAttr->StartPoint);
		pDC->OutputCoord(pGradFillAttr->EndPoint);

		// Output the grad fill token
		pDC->OutputToken("ax");
		pDC->OutputNewLine();
	}

	if (SetLastOutputAttribute(ATTR_WINDINGRULE))
	{
		WindingRuleAttribute *pWindingRuleAttr = 
			(WindingRuleAttribute *) CurrentAttrs[ATTR_WINDINGRULE].pAttr;

		switch (pWindingRuleAttr->WindingRule)
		{
			case NonZeroWinding:
				// Change winding rule to 0, which means non-zero in ArtWorks EPS.
				pDC->OutputToken("0");
				pDC->OutputToken("awr");
				pDC->OutputNewLine();
				break;

			case EvenOddWinding:
				// Change winding rule to 1, which means even-odd in ArtWorks EPS.
				pDC->OutputToken("1");
				pDC->OutputToken("awr");
				pDC->OutputNewLine();
				break;

			case NegativeWinding:
			case PositiveWinding:
				// Not supported in ArtWorks EPS - ignore.
				break;

			default:
				ERROR3("Unknown winding rule encountered while exporting.");
				break;
				
		}
	}

	// Handle usual pens/brushes
	EPSRenderRegion::GetValidPathAttributes();
#endif
}

/********************************************************************************************

>	BOOL ArtWorksEPSRenderRegion::WriteEPSBoundingBox ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/000
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes an EPS bounding box.
	SeeAlso:	EPSRenderRegion::InitDevice

********************************************************************************************/

BOOL ArtWorksEPSRenderRegion::WriteEPSBoundingBox ( void )
{
	// Scale conversion between millipoints and millimetres.
	const double	Scale			= 25.4 / 72000.0;

	// Cast a pointer to the appropriate DC.
	KernelDC		*pDC			= static_cast<KernelDC*> ( RenderDC );
	DocRect			PageBounds		= RenderSpread->GetPageBounds ();
	DocRect			DrawingBounds	= RenderSpread->GetDrawingSize ();
	TCHAR			Buffer [256];

	// Set up the output buffer.
	_stprintf( Buffer, "%gmm %gmm",
			   static_cast<double> ( PageBounds.Width () )  * Scale,
			   static_cast<double> ( PageBounds.Height () ) * Scale );

	// Output the page size data.
	pDC->OutputToken	("%%DocumentPageSize:");
	pDC->OutputToken	( Buffer );
	pDC->OutputNewLine	();

	// Write the AW bounding box out. I'm doing this manually because ArtWorks runs at a
	// higher resolution than standard EPS files. This way I get an extra two decimal
	// places of accuracy.
	pDC->OutputToken	( "%%BoundingBox:" );
	pDC->OutputCoord	( DrawingBounds.lo, ACCURACY_NORMAL );
	pDC->OutputCoord	( DrawingBounds.hi, ACCURACY_NORMAL );
	pDC->OutputNewLine	();

	// Success.
	return TRUE;
}

/********************************************************************************************

>	BOOL ArtWorksEPSRenderRegion::WriteEPSTrailerComments ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/000
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes out an EPS trailer.
	SeeAlso:	EPSRenderRegion::CloseDown

********************************************************************************************/

BOOL ArtWorksEPSRenderRegion::WriteEPSTrailerComments ( void )
{
	// Get a pointer to the kernel DC.
	KernelDC	*pDC	= static_cast<KernelDC*> ( RenderDC );

	// Write out the trailer comments.
	pDC->OutputToken	( "%%Trailer" );
	pDC->OutputNewLine	();
	pDC->OutputToken	( "showpage" );
	pDC->OutputNewLine	();

	// Success.
	return TRUE;
}


/********************************************************************************************

>	virtual void ArtWorksEPSRenderRegion::OutputStrokeColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/00
	Inputs:		-
	Returns:	-
	Purpose:	Writes out the stroke colour. Since AW can't cope with our new Xa/XA and 
				Xx/XX.RGB colour tokens, this function is overridden to use only the old 
				CMYK tokens (k/K and x/X)
	SeeAlso:	EPSRenderRegion::OutputStrokeColour

********************************************************************************************/

void ArtWorksEPSRenderRegion::OutputStrokeColour ()
{
	EPSRenderRegion::OutputStrokeCMYKColour ();
}


/********************************************************************************************

>	virtual void ArtWorksEPSRenderRegion::OutputFillColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/00
	Inputs:		-
	Returns:	-
	Purpose:	Writes out the fill colour. Since AW can't cope with our new Xa/XA and 
				Xx/XX.RGB colour tokens, this function is overridden to use only the old 
				CMYK tokens (k/K and x/X)
	SeeAlso:	EPSRenderRegion::OutputStrokeColour

********************************************************************************************/

void ArtWorksEPSRenderRegion::OutputFillColour ()
{
	EPSRenderRegion::OutputFillCMYKColour ();
}
