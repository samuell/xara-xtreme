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
// ******** Aldus FreeHand EPS filter

/*
*/

#include "camtypes.h"
#include "freeeps.h"
#include "oilfltrs.h"
#include "filters.h"
//#include "ben.h"
#include "fixstr64.h"
#include "colcomp.h"
#include "colourix.h"
#include "collist.h"
#include "fixmem.h"
#include "nodepath.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(FreeHandEPSFilter, EPSFilter)

#define new CAM_DEBUG_NEW

// All the commands used by AI EPS files.
enum
{
	// colours
	EPSC_Xa = EPSC_EOF + 1,
	EPSC_xa,
	EPSC_Ka,
	EPSC_ka,

	// fills
	EPSC_radfill,
	EPSC_recfill,
	EPSC_load,

	// colour list stuff
	EPSC_BeginSetup,
	EPSC_def,
	EPSC_newcmykcustomcolor,
	
	// text type stuff
	EPSC_makesetfont,
	EPSC_ts,
	EPSC_sts,

	// complex path stuff
	EPSC_eomode,
	EPSC_true,
	EPSC_false,

	// misc stuff which is ignored
	EPSC_concat,
	EPSC_vms,
	EPSC_vmr,
	EPSC_vmrs,
	EPSC_stob,
	EPSC_fhsetspreadallow,
	EPSC_FREEHAND_IGNOREDTOKEN		// a token used to ingore other tokens
};

// This is the array of AI EPS command/keyword names.
CommandMap FreeHandEPSFilter::FHCommands[] =
{
	// colours
	EPSC_Xa,		"Xa",
	EPSC_xa,		"xa",
	EPSC_Ka,		"Ka",
	EPSC_ka,		"ka",

	// fills
	EPSC_radfill,	"radfill",
	EPSC_recfill,	"recfill",
	EPSC_load,		"load",

	// colour list stuff
	EPSC_BeginSetup,"%%BeginSetup",
	EPSC_def,		"def",
	EPSC_newcmykcustomcolor, "newcmykcustomcolor",

	// text stuff
	EPSC_makesetfont, "makesetfont",
	EPSC_ts,		"ts",
	EPSC_sts,		"sts",

	// complex path stuff
	EPSC_eomode,	"eomode",
	EPSC_true,		"true",
	EPSC_false,		"false",

	// misc stuff
	EPSC_concat,	"concat",
	EPSC_vms,		"vms",
	EPSC_vmr,		"vmr",
	EPSC_vmrs,		"vmrs",
	EPSC_stob,		"stob",
	EPSC_fhsetspreadallow, "fhsetspreadallow",

	// Sentinel
	EPSC_Invalid,	"Invalid"
};

/********************************************************************************************

>	FreeHandEPSFilter::FreeHandEPSFilter()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Purpose:	Constructor for an FreeHandEPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

FreeHandEPSFilter::FreeHandEPSFilter()
{
	// Set up filter descriptions.
	FilterNameID = _R(IDT_FREEHANDEPS_FILTERNAME);
	FilterInfoID = _R(IDT_FREEHANDEPS_FILTERINFO);
	ImportMsgID  = _R(IDT_IMPORTMSG_FREEHANDEPS);

	FilterID = FILTERID_FREEHAND_EPS;

	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;

	AdjustOrigin = FALSE;
}

/********************************************************************************************

>	BOOL FreeHandEPSFilter::Init()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/05/95
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an FreeHandEPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL FreeHandEPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new FreeHandEPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL FreeHandEPSFilter::PrepareToImport()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	error flag
	Purpose:	prepares to import a freehand EPS file
	SeeAlso:	EPSFilter

********************************************************************************************/

BOOL FreeHandEPSFilter::PrepareToImport()
{
	if(!EPSFilter::PrepareToImport())
		return FALSE;

	// set some nice variables
	HadhToken = FALSE;
	DoingGradFill = FALSE;
	OldFill = 0;
	InColours = FALSE;
	InText = FALSE;
	ComplexPathMode = FALSE;
	pLastPathSeen = 0;
	
	// set the clipping flags
	ClipRegion.SetClippingFlags(2);

	// get an importedcolours object
	pNewColours = new ImportedColours(pColours, FALSE);
	if(pNewColours == 0 || !pNewColours->Init())
		return FALSE;

	// get a colour array
	ColourArray = (IndexedColour **)CCMalloc(FHEF_COLOURARRAY_INITIALSIZE * sizeof(IndexedColour *));
	if(ColourArray == 0)
	{
		delete pNewColours;
		pNewColours = 0;
		return FALSE;
	}
	ColourArrayEntries = 0;
	ColourArraySize = FHEF_COLOURARRAY_INITIALSIZE;

	return TRUE;
}

/********************************************************************************************

>	void FreeHandEPSFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	error flag
	Purpose:	clears up after importing a FreeHand 3.0 EPS file
	SeeAlso:	EPSFilter

********************************************************************************************/

void FreeHandEPSFilter::CleanUpAfterImport(BOOL Successful)
{
	// ensure that there's no old fill hanging around
	if(OldFill != 0)
		delete OldFill;

	OldFill = 0;

	// add new colours if successful, destroy them if not
	if(Successful)
	{
		pNewColours->AddColoursToDocument();
	}
	else
	{
		pNewColours->DestroyColours();
	}

	// delete the colour reference array
	if(ColourArray != 0)
	{
		CCFree(ColourArray);
		ColourArray = 0;
	}

	// get rid of any stuff in the stack
	Stack.DeleteAll ();

	EPSFilter::CleanUpAfterImport(Successful);
}


/********************************************************************************************

>	INT32 FreeHandEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/95
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an FreeHand 3.0
				generated EPS file, as required.

********************************************************************************************/

INT32 FreeHandEPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// Check the first line in EPS file
	if (_tcsncmp((char *) pFileHeader, "%!PS-Adobe", 10) != 0)
	{
		// Incorrect version of EPS header line - we don't want this
		return 0;
	}

	// !PS-Adobe line is ok - check creator line...
	char *Buffer;
	CCMemTextFile HeaderFile((char *)pFileHeader, HeaderSize);
	if(HeaderFile.IsMemFileInited() == FALSE || HeaderFile.InitLexer() == FALSE)
	{
		HeaderFile.close();
		return 0;
	}


	BOOL HaveCreatorString = FALSE;
	
	UINT32 Lines = 0;
	while ((Lines < 20) && !HeaderFile.eof())
	{
		HeaderFile.GetLineToken();
		Buffer = (char *)HeaderFile.GetTokenBuf();
		ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");
		Lines++;

		// Return TRUE if this file was created by Illustrator, or has been exported in 
		// Illustrator format.
		if (_tcsncmp(Buffer, "%%Creator: ", 11) == 0 && strstr(Buffer, "FreeHand") != 0)
		{
			// found a plausible creator string - but it could be any version
			// (3.0 for the Mac gives it's version number here, but the PC one doesn't)
			HaveCreatorString = TRUE;
		}

		if (_tcsncmp(Buffer, "%%DocumentProcSets: FreeHand_header 3 ", 38) == 0 && HaveCreatorString)
		{
			// I'll have that then.
			HeaderFile.close();
			return 10;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (_tcsncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	HeaderFile.close();
	
	return 0;
}


/********************************************************************************************

>	void FreeHandEPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if the token is an FreeHand EPS token; FALSE if not.
	Purpose:	Compare the current token against the AI keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::LookUpToken; EPSFilter::DecodeToken

********************************************************************************************/

void FreeHandEPSFilter::LookUpToken()
{
	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (FHCommands[i].Cmd != EPSC_Invalid)
	{
		if (_tcscmp(TokenBuf, FHCommands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = FHCommands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	EPSFilter::LookUpToken();
}


/********************************************************************************************

>	BOOL AIEPSFilter::ProcessToken()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/95
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set, or
				which need to be handled differently to the standard Illustrator meanings.
				i.e. this is the function that handles all the FreeHand EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

#define FHEF_SENDh { if(pPath != 0) {				\
		EPSCommand ThisToken = Token;				\
		Token = ComplexPathMode?EPSC_s:EPSC_h;		\
		if(!EPSFilter::ProcessToken())				\
			return FALSE;							\
		Token = ThisToken; 							\
		HadhToken = FALSE; } }


BOOL FreeHandEPSFilter::ProcessToken()
{
	// Decode the command, and execute it...
	switch (Token)
	{
		// state saving
		case EPSC_vms:
			if(!Import_gsave())
				return FALSE;
			break;

		case EPSC_vmr:
			if(!Import_grestore())
				return FALSE;
			break;

		case EPSC_vmrs:
			if(!Import_grestore())
				return FALSE;
			if(!Import_gsave())
				return FALSE;
			break;

		// tokens to ignore
		case EPSC_FREEHAND_IGNOREDTOKEN:
			break;

		// tokens which should be ignored and one entry discarded
		case EPSC_load:			// the load part of a fill - discard the /clipper before it
		case EPSC_fhsetspreadallow:
			if(!Stack.Discard(1))
				goto EPSError;
			break;

		case EPSC_concat:
			if(!Stack.DiscardArray())
				goto EPSError;
			break;
		
		// complex paths...
		case EPSC_eomode:
			{
				INT32 ComplexStart;

				if(!Stack.Pop(&ComplexStart))
					goto EPSError;

				// is this a start of a complex path?
				if(ComplexStart != TRUE)
				{
					ComplexPathMode = FALSE;
				}
				else
				{
					HadFirstOfComplexPath = FALSE;
					ComplexPathMode = TRUE;
				}
			}
			break;

		case EPSC_true:
			Stack.Push((INT32)TRUE);
			break;

		case EPSC_false:
			Stack.Push((INT32)FALSE);
			break;

		case EPSC_u:
			HadFirstOfComplexPath = FALSE;
			return EPSFilter::ProcessToken();
			break;

		// colours
		case EPSC_Ka:
		case EPSC_ka:
			{
				DocColour Colour;

				if(PopColour(&Colour))
				{
					// Remember this colour for future objects
					if (Token == EPSC_ka)
					{
				    	if (!SetFillColour(Colour))
							goto NoMemory;
					}
					else
					{
						if (!SetLineColour(Colour))
							goto NoMemory;
					}
				}
			    else	
			    	// Invalid colour operands
					goto EPSError;
			}
			break;

		case EPSC_Xa:
		case EPSC_xa:
			{
				DocColour Colour;

				if(PopNamedColour(&Colour))
				{
					// Remember this colour for future objects
					if (Token == EPSC_xa)
					{
				    	if (!SetFillColour(Colour))
							goto NoMemory;
					}
					else
					{
						if (!SetLineColour(Colour))
							goto NoMemory;
					}
				}
			    else	
			    	// Invalid colour operands
					goto EPSError;
			}
			break;

		case EPSC_H:
			if(ComplexPathMode)
			{
				// in complex path mode - make this a filled one, not a discarded one
				Token = EPSC_S; 
			}
			return EPSFilter::ProcessToken();
			break;
		
		case EPSC_h:
			if(ComplexPathMode)
			{
				// in complex path mode - modify and process
				Token = EPSC_s;
				return EPSFilter::ProcessToken();
				break;
			}
			// the hidden path closing operator - a grad fill thingy will follow shortly maybe...
			// this will prevent it being processed now, although it may get processed later on.
			HadhToken = TRUE;
			break;

		// for clipping masks, do some funky stuff
		case EPSC_q:
			// if there's a pending grad fill...
			if(DoingGradFill)
			{
				if(pPath != 0)
				{
					// right then, make a copy of the path...
					NodePath *pPathClone;

					if(!pPath->NodeCopy((Node **)&pPathClone))
					{
						goto NoMemory;
					}

					// copy the flags
					EPSFlagsDefn EPSFlagsClone = EPSFlags;
				
					// send a token to finish and fill the path...
					Token = (pInkPath->IsFilled)?EPSC_f:EPSC_s;
					if(!EPSFilter::ProcessToken())
						return FALSE;

					// restore the old fill
					if(!RestoreCurrentFill())
						goto NoMemory;

					// restore the copy of the path
					pPath = pPathClone;
					pInkPath = &pPath->InkPath;

					// restore the flags
					EPSFlags = EPSFlagsClone;

					// definately want to send an h
					HadhToken = TRUE;
				}

				// done the grad fill
				DoingGradFill = FALSE;

				// restore the old token
				Token = EPSC_q;
			}

			// clipping started - have we got an h token to send?
			if(HadhToken)
				FHEF_SENDh

			// process this
			return EPSFilter::ProcessToken();
			break;

		// for now, if there's no path, don't return a W
		case EPSC_W:
			if(pPath == 0)
			{
				// OK, now we want to get the last path we created, make a copy of it and then install it as the current one
				if(pLastPathSeen == 0)
					goto EPSError;

				// make a copy of it
				NodePath *pClone;
				
				if(!pLastPathSeen->NodeCopy((Node **)&pClone))
					goto NoMemory;

				// delete it's attributes
				pClone->DeleteChildren(pClone->FindFirstChild());

				// make it the current path
				pPath = pClone;
				pInkPath = &pPath->InkPath;
				ThePathType = PATH_NORMAL;
				EPSFlags.NoAttributes = TRUE;
			}
			if(pPath != 0)
				return EPSFilter::ProcessToken();
			break;

		// we may need to modify path closing things if we're doing a grad fill
		case EPSC_s:
		case EPSC_S:
			if(Token == EPSC_S)
			{
				// if we've had an h token but no grad fill, send the h now
				if(HadhToken)
					FHEF_SENDh

				// if we've got a grad fill, modify the token we got
				if(DoingGradFill)
					Token = EPSC_b;
			}
			
			// process the possily modified token normally
			HadhToken = FALSE;
			return EPSFilter::ProcessToken();
			break;

		// modify path closing for grad fills.
		case EPSC_n:
			if(DoingGradFill)
			{
				Token = EPSC_f;			// we want to fill the thing
				HadhToken = FALSE;
				return EPSFilter::ProcessToken();
				break;
			}
   			HadhToken = FALSE;			// ignore h's as this is another end path thingy...

			if(pPath != 0)
				return EPSFilter::ProcessToken();
			break;

		// unset the had h token for other path closing things
		case EPSC_N:
		case EPSC_F:
		case EPSC_f:
		case EPSC_B:
		case EPSC_b:
			HadhToken = FALSE;
			return EPSFilter::ProcessToken();
			break;
		
		// interested in path element things to switch off grad fills
		case EPSC_m:
			if(InText)
			{
				// if we're doing some text, discard the moveto command
				if(!Stack.Discard(2))
					goto EPSError;

				break;
			}
		case EPSC_l:
		case EPSC_L:
		case EPSC_c:
		case EPSC_C:
		case EPSC_v:
		case EPSC_V:
		case EPSC_y:
		case EPSC_Y:
			// maybe we need an h token to be sent
			if(HadhToken)
				FHEF_SENDh
			
			// stop grad fill
			if(DoingGradFill)
			{
				// turn the grad fill state off
				DoingGradFill = FALSE;

				// restore the old fill type
				RestoreCurrentFill();
			}
			return EPSFilter::ProcessToken();
			break;

		
		case EPSC_recfill:
			{
				// get the colours
				DocColour StartColour, EndColour;

				if(!PopColour(&EndColour) ||
						!PopColour(&StartColour))
					goto EPSError;

				// discard the fill type thingy - we can only do colours
				if(!DiscardFillSubType())
					goto EPSError;

				// OK, now a few coords
				DocCoord Centre;
				double Angle;
				DocRect BBox;
				if(!Stack.PopCoordPair(&BBox.hi) ||
						!Stack.PopCoordPair(&BBox.lo) ||
						!Stack.Pop(&Angle) ||
						!Stack.PopCoordPair(&Centre))
					goto EPSError;

				// munge the angle a little and get it into radians
				Angle += 225;
				Angle = (Angle * (2 * PI)) / 360;

				// see if we can get a more accurate BBox
				if(pPath != 0)
				{
					BBox = pPath->GetBoundingRect();
					Centre.x = BBox.lo.x + (BBox.Width() / 2);
					Centre.y = BBox.lo.y + (BBox.Height() / 2);
				}

				// OK, we've got all the stuff we need to do some niceness on it
				BBox.Translate(0 - Centre.x, 0 - Centre.y);
				DocCoord StartPoint, EndPoint;

				StartPoint.x = Centre.x + (INT32)(((double)BBox.lo.x * cos(Angle)) - ((double)BBox.lo.y * sin(Angle)));
				StartPoint.y = Centre.y + (INT32)(((double)BBox.lo.x * sin(Angle)) + ((double)BBox.lo.y * cos(Angle)));
				EndPoint.x = Centre.x + (INT32)(((double)BBox.hi.x * cos(Angle)) - ((double)BBox.hi.y * sin(Angle)));
				EndPoint.y = Centre.y + (INT32)(((double)BBox.hi.x * sin(Angle)) + ((double)BBox.hi.y * cos(Angle)));
		
				// store current fill attribute
				SaveCurrentFill();

				// set the fill
				if(!SetLinearFill(StartColour, EndColour, StartPoint, EndPoint))
					goto NoMemory;

				// say we're doing a grad fill
				DoingGradFill = TRUE;
				HadhToken = FALSE;			// absorb this
			}
			break;

		case EPSC_radfill:
			{
				// get the colours
				DocColour StartColour, EndColour;

				if(!PopColour(&StartColour) ||
						!PopColour(&EndColour))
					goto EPSError;

				// get the radius and centre coordinate
				DocCoord Centre;
				INT32 Radius;
				if(!Stack.PopCoord(&Radius) ||
						!Stack.PopCoordPair(&Centre))
					goto EPSError;

				// store current fill attribute
				SaveCurrentFill();

				// set the fill
				DocCoord EndPoint(Centre.x + Radius, Centre.y);
				if(!SetRadialFill(StartColour, EndColour, Centre, EndPoint))
					goto NoMemory;

				// say we're doing a grad fill
				DoingGradFill = TRUE;
				HadhToken = FALSE;
			}
			break;

		case EPSC_BeginSetup:
			// there's probably a colour list or something in that there setup thingy - search for the spots token
			{
				BOOL Found = FALSE;
				
				while(Found == FALSE)
				{
					if(!EPSFile->GetToken())
						return FALSE;

			 		if(EPSFile->GetTokenType() == TOKEN_NORMAL)
			 		{
			 			if(_tcscmp(TokenBuf, "spots") == 0)
						{
							// check to see if the array is about to start
							if(!EPSFile->GetToken())
								return FALSE;

							if(TokenBuf[0] == '[')
							{
								TRACEUSER( "Ben", _T("Found spots\n"));
								Found = TRUE;
							}
						}
					}
				
					if(_tcsncmp(TokenBuf, "%%EndSetup", 10) == 0)
					{
						TRACEUSER( "Ben", _T("Met end of setup without finding spots\n"));
						break;
					}

					if(EPSFile->eof())
						goto EPSError;
			 	}

				if(Found == TRUE)
				{
					InColours = TRUE;
				}
			}
			break;

		case EPSC_def:
			if(InColours)
			{
				// finished the colours...
				TRACEUSER( "Ben", _T("Finished spot colours\n"));			
				// scan for the end of the setup section
				BOOL Found = FALSE;
				
				while(Found == FALSE)
				{
					if(!EPSFile->GetToken())
						return FALSE;

			 		if(EPSFile->GetTokenType() == TOKEN_COMMENT)
			 		{
						if(_tcsncmp(TokenBuf, "%%EndSetup", 10) == 0)
						{
							TRACEUSER( "Ben", _T("Found end of setup\n"));
							Found = TRUE;
						}
					}

					if(EPSFile->eof())
						goto EPSError;
			 	}
				
				// get the ] off the stack
				EPSCommand Ignored;
				Stack.PopCmd(&Ignored);

				// empty it...
				Stack.DeleteAll ();
				InColours = FALSE;
			}
			else
			{
				// probably a font type thingy - empty the stack including commands
				Stack.DeleteAll ();
			}
			break;

		case EPSC_newcmykcustomcolor:
			// OK, here's a named colour... add it to those known
			{
				// discard some random thingy
				if(!Stack.Discard())
					goto EPSError;
			
				// get the name
				String_64 ColourName;
				if(!Stack.Pop(&ColourName))
					goto EPSError;

				// get the components
				double C, M, Y, K;
				if(!Stack.Pop(&K) ||
						!Stack.Pop(&Y) ||
						!Stack.Pop(&M) ||
						!Stack.Pop(&C))
					goto EPSError;

				// make the new colour
				ColourCMYK Colour;
				Colour.Cyan = C;
				Colour.Magenta = M;
				Colour.Yellow = Y;
				Colour.Key = K;

				// add it
				if(!pNewColours->AddColour(&ColourName, &Colour))
					goto NoMemory;

				// add it to the list of colours
				// this is a bit of a bodge, but never mind. Shouldn't be that bad.
				IndexedColour *TheNewColour = pNewColours->GetColour(ColourName);

				if(TheNewColour == 0)
					goto NoMemory;

				// add it to the list of colours
				// enough space?
				if((ColourArrayEntries + 1) >= ColourArraySize)
				{
					TRACEUSER( "Ben", _T("Extening colour array\n"));
					IndexedColour **NewPtr = (IndexedColour **)CCRealloc(ColourArray, (ColourArraySize + FHEF_COLOURARRAY_CHUNK) * sizeof(IndexedColour *));

					if(NewPtr == 0)
						goto NoMemory;

					ColourArray = NewPtr;
					ColourArraySize += FHEF_COLOURARRAY_CHUNK;
				}

				// add
				ColourArray[ColourArrayEntries] = TheNewColour;
				ColourArrayEntries++;
			}
			break;

		// ignore text stuff
		case EPSC_makesetfont:
			if(!Stack.DiscardArray())
				goto EPSError;
			if(!Stack.Discard(1))
				goto EPSError;
			InText = TRUE;
			break;

		case EPSC_ts:
			if(!Stack.Discard(6))
				goto EPSError;
			break;

		case EPSC_stob:
		case EPSC_sts:
			Stack.DeleteAll ();
			InText = FALSE;
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

>	BOOL FreeHandEPSFilter::DiscardFillSubType()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	EPS error
	Purpose:	discards a fill sub type from the stack ('{}' or '{logtaper}' or something)

********************************************************************************************/

BOOL FreeHandEPSFilter::DiscardFillSubType()
{
	BOOL Done = FALSE;
	EPSType Type;
	while(Done == FALSE)
	{
		Type = Stack.GetType();
		if(Type == EPSTYPE_NONE)
		{
			// not enough stuff on the stack
			return FALSE;;
		}
		else if(Type == EPSTYPE_STRING)
		{
			// string - is this the ending?
			String_64 Str;
			if(!Stack.Pop(&Str))
				return FALSE;
			if(_tcscmp(Str, "{") == 0)
				Done = TRUE;		// found end of this bit
		}
		else
		{
			// just something unimportant - discard it
			if(!Stack.Discard())
				return FALSE;
		}
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandEPSFilter::PopColour(DocColour *Col)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/95
	Returns:	TRUE if colour understood and processed ok, FALSE if not.
	Purpose:	Pops a FreeHand colour off the stack.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL FreeHandEPSFilter::PopColour(DocColour *Col)
{
	// OK, we expect an array of four values in the order (of popping) K, Y, M, C

	// check that we've got a array end
	EPSCommand Cmd;
	if(!Stack.PopCmd(&Cmd))
		return FALSE;
	if(Cmd != EPSC_ArrayEnd)
		return FALSE;

	// pop the colours
	double C, M, Y, K;
	if(!Stack.Pop(&K) ||
			!Stack.Pop(&Y) ||
			!Stack.Pop(&M) ||
			!Stack.Pop(&C))
		return FALSE;

	// check that we've got a array start
	if(!Stack.PopCmd(&Cmd))
		return FALSE;
	if(Cmd != EPSC_ArrayStart)
		return FALSE;

	// knock up the colour...
	ColourCMYK Colour;
	
	Colour.Cyan = C;
	Colour.Magenta = M;
	Colour.Yellow = Y;
	Colour.Key = K;
	
	IndexedColour *NewCol = new IndexedColour(COLOURMODEL_CMYK, (ColourGeneric *)&Colour);

	if(NewCol == 0)
		return FALSE;

	pColours->GetColourList()->GetUnnamedColours()->AddTail(NewCol);

	Col->MakeRefToIndexedColour(NewCol);

	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandEPSFilter::PopNamedColour(DocColour *Col)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/06/95
	Returns:	TRUE if colour understood and processed ok, FALSE if not.
	Purpose:	Pops a FreeHand named colour off the stack.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL FreeHandEPSFilter::PopNamedColour(DocColour *Col)
{
	// OK, we expect an array of two values in the order (of popping) random, index
	// and we can use the index to look up the indexed colour we should use in our
	// nice list in ColourArray.
	// 'random'? Maybe it's a tint or something? Which doesn't seem to be used.

	// check that we've got a array end
	EPSCommand Cmd;
	if(!Stack.PopCmd(&Cmd))
		return FALSE;
	if(Cmd != EPSC_ArrayEnd)
		return FALSE;

	// pop the index
	double Random;
	INT32 Index;
	if(!Stack.Pop(&Index) ||
			!Stack.Pop(&Random))
		return FALSE;

	// check that we've got a array start
	if(!Stack.PopCmd(&Cmd))
		return FALSE;
	if(Cmd != EPSC_ArrayStart)
		return FALSE;

	// check that the index is valid
	if(Index < 0 || Index >= ColourArrayEntries)
		return FALSE;

	// knock up the colour...
	Col->MakeRefToIndexedColour(ColourArray[Index]);

	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandEPSFilter::SaveCurrentFill()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	error flag
	Purpose:	stores the current fill attribute
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL FreeHandEPSFilter::SaveCurrentFill()
{
	// get rid of any stored fill
	if(OldFill != 0)
		delete OldFill;

	// create a copy of the current fill attribute
	CCRuntimeClass* ObjectType = CurrentAttrs[ATTR_FILLGEOMETRY].pAttr->GetRuntimeClass();
	AttributeValue* AttrClone = (AttributeValue*)ObjectType->CreateObject();

	if(AttrClone == 0)
		return FALSE;
	
	AttrClone->SimpleCopy(CurrentAttrs[ATTR_FILLGEOMETRY].pAttr);

	// set the old fill pointer to the nice clone we've made
	OldFill = AttrClone;

	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandEPSFilter::RestoreCurrentFill()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	error flag
	Purpose:	restores the current fill attribute from the stored one
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL FreeHandEPSFilter::RestoreCurrentFill()
{
	if(OldFill == 0)
		return TRUE;			// no saved state

	// delete the old attribute
	if(CurrentAttrs[ATTR_FILLGEOMETRY].pAttr != 0)
		delete CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;

	// make the current one the old one
	CurrentAttrs[ATTR_FILLGEOMETRY].pAttr = OldFill;

	// mark this as used so it doesn't get deleted
	OldFill = 0;
	
	return TRUE;
}

/********************************************************************************************

>	BOOL FreeHandEPSFilter::ValidateGroup(Node *pGroup);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	error flag
	Purpose:	validates a group created by the filter. As all objects have a group around
				then, this removes groups which only have one entry.

********************************************************************************************/

BOOL FreeHandEPSFilter::ValidateGroup(Node *pGroup)
{
	ERROR2IF(pGroup == 0, FALSE, "Group passed to validateGroup is null");

	Node *pFirstChild = pGroup->FindFirstChild();

	ERROR2IF(pFirstChild == 0, FALSE, "Group without children passed to ValidateGroup - should have been deleted by EndGroup"); 

	if(pFirstChild->FindNext() == 0)
	{
		// there was only one object - delete this group.

		// if pFirstNodeInRange is this node, set it to the child
		if(pFirstNodeInRange == pGroup)
			pFirstNodeInRange = pFirstChild;
		
		// move the child to the parent of the group
		if (!ImportInfo.pOp->DoMoveNode(pFirstChild, pNode, LASTCHILD))
			return FALSE;

		// see EPSFilter::EndGroup for the reason why it's hidden rather than deleted
		if (!ImportInfo.pOp->DoHideNode(pGroup, TRUE))
			return FALSE;

	}

	return TRUE;
}

/********************************************************************************************

>	BOOL FreeHandEPSFilter::MaskedGroupEnding()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/95
	Returns:	EPS error flag
	Purpose:	Restores the graphic state after a masked group ends.

********************************************************************************************/

BOOL FreeHandEPSFilter::MaskedGroupEnding()
{
	// make the path type normal
	if(ThePathType == PATH_DISCARD || ThePathType == PATH_DISCARD_STICKY)
	{
		ThePathType = PATH_NORMAL;
	}
	
	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandEPSFilter::RemoveLastSubPathIfNotUnique(Path *pIPath)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/06/95
	Returns:	error flag
	Purpose:	Removes the last subpath from a path if an sub path with it's exact coords
				already exists.

********************************************************************************************/

BOOL FreeHandEPSFilter::RemoveLastSubPathIfNotUnique(Path *pIPath)
{
	// get coord arrays
	INT32 NCoords = pIPath->GetNumCoords();
	DocCoord *Coords = pIPath->GetCoordArray();
	PathVerb *Verbs = pIPath->GetVerbArray();

	// find the last moveto
	INT32 LastMoveTo = -1;
	INT32 c;

	for(c = 0; c < NCoords; c++)
	{
		if((Verbs[c] & ~PT_CLOSEFIGURE) == PT_MOVETO)
		{
			LastMoveTo = c;
		}
	}

	// check that there is a last moveto and it's not the first coord...
	if(LastMoveTo == -1 || LastMoveTo == 0)
		return TRUE;			// nothing more to do
	
	// go through the coords matching subpaths...
	for(c = 0; c < LastMoveTo; c++)
	{
		if((Verbs[c] & ~PT_CLOSEFIGURE) == PT_MOVETO)
		{
			// found a moveto... compare the subpaths
			INT32 n = 0;

			while(((Verbs[c+n] & ~PT_CLOSEFIGURE) == (Verbs[LastMoveTo+n] & ~PT_CLOSEFIGURE)) && (Coords[c+n] == Coords[LastMoveTo+n]))
			{
				// this was ok... is the next a moveto and the subpath the right length?

				if((LastMoveTo+n+1) == NCoords)
				{
					// yes, delete and return
					return pIPath->DeleteFromElement(LastMoveTo);
				}
				
				n++;
			}
		}
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL FreeHandEPSFilter::AddNewNode(Node *pNewNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/95
	Returns:	error flag
	Purpose:	checks to see if we're in complex path mode before passing on the node to
				the base class. If we are, it joins it to the last one and fiddles about
				with the attributes.

********************************************************************************************/

BOOL FreeHandEPSFilter::AddNewNode(Node *pNewNode)
{
	if(IS_A(pNewNode, NodePath))
		pLastPathSeen = (NodePath *)pNewNode;
	
	// check to see if we want to handle this
	if((ComplexPathMode == FALSE) || (pNewNode == 0) || (pNode == 0) || (!IS_A(pNewNode, NodePath)))
		return EPSFilter::AddNewNode(pNewNode);

	// check to see if this is the first...
	if(HadFirstOfComplexPath == FALSE)
	{
		HadFirstOfComplexPath = TRUE;
		return EPSFilter::AddNewNode(pNewNode);
	}

	// find the last child of the node
	Node *pLastChild = pNode->FindLastChild();
	if(pLastChild == 0 || !IS_A(pLastChild, NodePath))
		return EPSFilter::AddNewNode(pNewNode);

	// we know that both of these things are NodePaths.
	Path *pTarget = &((NodePath *)pLastChild)->InkPath;
	Path *pAddition = &((NodePath *)pNewNode)->InkPath;

	// work out the new flags for the target
	BOOL TargetFilled = pTarget->IsFilled;
	BOOL TargetStroked = pTarget->IsStroked;
	if(pAddition->IsFilled)		TargetFilled = TRUE;
	if(pAddition->IsStroked)	TargetStroked = TRUE;

	// add this new path to the old one...
	if(!pTarget->MergeTwoPaths(*pAddition))
		return FALSE;

	// check that the thing we just added isn't already there
	if(!RemoveLastSubPathIfNotUnique(pTarget))
		return FALSE;

	// set it's flags
	pTarget->IsFilled = TargetFilled;
	pTarget->IsStroked = TargetStroked;

	// vape it's attributes
	pLastChild->DeleteChildren(pLastChild->FindFirstChild());

	// apply some new ones
	SetPathFilled(TargetFilled);
	if(!AddAttributes((NodePath *)pLastChild, TargetStroked, TargetFilled))
		return FALSE;

	// hide the nice additional path
	//if(!ImportInfo.pOp->DoHideNode(pNewNode, TRUE))
	//	return FALSE;
	pNewNode->CascadeDelete();
	delete pNewNode;

	// set the last seen path bollox
	pLastPathSeen = (NodePath *)pLastChild;

	// done!
	return TRUE;
}




