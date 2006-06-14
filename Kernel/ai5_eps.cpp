// $Id$
// Implementation of Adobe Illustrator 5 EPS filter.
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

#include "ai_eps.h"
#include "ai5_eps.h"
#include "ai_epsrr.h"
#include <sstream>
#include <stdio.h>

#include "nodepath.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "nev.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ccdc.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "page.h"
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opbevel.h"
#include "progress.h"        
#include "ai_grad.h"        
#include "ai_bmp.h"
#include "ai_layer.h"  
#include "swffiltr.h"
#include "layer.h"

DECLARE_SOURCE("$Revision");

#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(AI5EPSFilter, AIEPSFilter)

// This is the array of AI EPS command/keyword names.
CommandMap AI5EPSFilter::AI5Commands[] =
{
	// Graduated fills
	{ EPSC_Bd,		_T("Bd")},
	{ EPSC_Bm,		_T("Bm")},
	{ EPSC_Bc,		_T("Bc")},
	{ EPSC__Bs,		_T("%_Bs")},
	{ EPSC__BS,		_T("%_BS")},
	{ EPSC__Br,		_T("%_Br")},
	{ EPSC_BD,		_T("BD")},
	{ EPSC_Bg,		_T("Bg")},
	{ EPSC_Bb,		_T("Bb")},
	{ EPSC_BB,		_T("BB")},
	{ EPSC_Bh,		_T("Bh")},
	{ EPSC_HexStart,	_T("<")},
	{ EPSC_HexEnd,	_T(">")},

	// Layer stuff
	{ EPSC_Lb,		_T("Lb")},
	{ EPSC_LB,		_T("LB")},
	{ EPSC_Ln,		_T("Ln")},

	// Path tweaks
	{ EPSC_Ap,		_T("Ap")},
	{ EPSC_Ar,		_T("Ar")},
	{ EPSC_XR,		_T("XR")},
	
	// bitmaps
	{ EPSC_XI,		_T("XI")},
	{ EPSC_XF,		_T("XF")},
	{ EPSC_XG,		_T("XG")},
	{ EPSC_Xh,		_T("Xh")},
	{ EPSC_XH,		_T("XH")},

	// unknown
	{ EPSC_XP,		_T("XP")},

	// object tags
	{ EPSC_XT,		_T("XT")},

	// spurious
	{ EPSC_Xm,		_T("Xm")},

	// RGB Colours
	{ EPSC_Xa,		_T("Xa")},
	{ EPSC_XA,		_T("XA")},
	{ EPSC_Xx,		_T("Xx")},
	{ EPSC_XX,		_T("XX")},

	// Sentinel
	{ EPSC_Invalid,	_T("Invalid")}
};

/********************************************************************************************

>	AI5EPSFilter::AI5EPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an AI5EPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

AI5EPSFilter::AI5EPSFilter() :
	mpGradientProc(0),
	mpBitmapProc(0),
	mpLayerProc ( 0 ),
	mbReadingGradFill(FALSE),
	mbReadingBitmap(FALSE)
{
	// Set up filter descriptions.
	FilterNameID = _R(IDT_AI5EPS_FILTERNAME);
	FilterInfoID = _R(IDT_AI5EPS_FILTERINFO);
	ImportMsgID  = _R(IDT_IMPORTMSG_AI5);

	FilterID = FILTERID_AI5EPS;

	// Import only for Illustrator 5.
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
}

/********************************************************************************************

>	BOOL AI5EPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an AI5EPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL AI5EPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new AI5EPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(FilterNameID);
	FilterInfo.Load(FilterInfoID);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 AI5EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an AI
				generated EPS file, as required.

********************************************************************************************/

INT32 AI5EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	UINT32	Lines	= 0;
	TCHAR	*Buffer	= NULL;

	// !PS-Adobe line is ok - check creator line...
	CCMemTextFile HeaderFile ( reinterpret_cast<char *> ( pFileHeader ), HeaderSize );

	if( HeaderFile.IsMemFileInited () == FALSE || HeaderFile.InitLexer () == FALSE )
	{
		HeaderFile.close();
		return 0;
	}

	// Graeme (28/6/00) - Adobe have changed their file format, and so the first line can
	// now be a %PDF directive. Therefore look for this directive in the first twenty
	// lines.
	while ( ( Lines < 100 ) && !HeaderFile.eof () )
	{
		// Get the current line from the file.
		HeaderFile.GetLineToken();
		Buffer = const_cast<TCHAR *> ( HeaderFile.GetTokenBuf () );

		// Ensure that it's OK.
		ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");

		// Increment the line counter.
		Lines++;

		if (camStrncmp(Buffer, _T("%!PS-Adobe"), 10) == 0)
		{
			// Now find the %%Creator string.
			while ((Lines < 100) && !HeaderFile.eof())
			{
				HeaderFile.GetLineToken();
				Buffer = const_cast<TCHAR *> ( HeaderFile.GetTokenBuf() );
				ERROR2IF(Buffer == 0, 0, "Returned buffer from lex file == 0");
				Lines++;

				// Return TRUE if this file was created by Illustrator, or has been exported
				// in Illustrator format.
				if (camStrncmp(Buffer, _T("%%Creator: Adobe Illustrator(TM) 5"), 34) == 0)
				{
					// We definitely want this.
					HeaderFile.close();
					return 10;
				}

				if (camStrncmp(Buffer, _T("%%Creator:"), 10) == 0)
				{
					// Found the creator line - does it contain the word Illustrator?
					if (camStrstr( (const TCHAR*)Buffer, _T("Illustrator(TM) 5")) != NULL)
					{
						HeaderFile.close();
						return 10;
					}
					// we'll accept version 7.0 as well
					else if ((camStrstr( (const TCHAR*)Buffer, _T("Illustrator(TM) 7")) != NULL) ||
							 (camStrstr( (const TCHAR*)Buffer, _T("Illustrator(R) 7")) != NULL))
					{
						HeaderFile.close();
						return 10;
					}
					// Catch FreeHand generated EPS files.
					else if (camStrstr( (const TCHAR*)Buffer, _T("FreeHand")) != NULL)
					{
						HeaderFile.close();
						return 8;
					}
					else
						break;
				}

				// If we find the compression token then stop the search as we don't want to
				// start looking in the compressed data!
				if (camStrncmp(Buffer, _T("%%Compression:"), 14)==0)
					break;
			}

			// Remember to close the file before returning.
			HeaderFile.close();

			// Didn't find a suitable Creator line, but the EPS line was ok, so return
			// that we're interested, but not sure.
			return 5;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (camStrncmp(Buffer, _T("%%Compression:"), 14)==0)
			break;
	}

	// Remember to close the file before returning.
	HeaderFile.close();
	
	// This file type isn't suitable.
	return 0;
}

/********************************************************************************************

>	BOOL AI5EPSFilter::PrepareToImport()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/94
	Returns:	TRUE if successfully prepared to import;
				FALSE if not.
	Purpose:	Set up the stuff ready for adding gradient fills from AI 5 files.
	SeeAlso:	EPSFilter::PrepareToImport

********************************************************************************************/

BOOL AI5EPSFilter::PrepareToImport()
{
	mbReadingGradFill	= FALSE;
	mbReadingBitmap		= FALSE;

	return AIEPSFilter::PrepareToImport();
}


/********************************************************************************************

>	void AI5EPSFilter::CleanUpAfterImport(BOOL Successful)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/94
	Inputs:		Successful - TRUE if the import was successful.
	Purpose:	Cleans up our list of AI5 gradient fill definitions, and then calls the base
				class to perform usual clean up.
	SeeAlso:	EPSFilter::CleanUpAfterImport

********************************************************************************************/

void AI5EPSFilter::CleanUpAfterImport(BOOL Successful)
{
	///////////////
	//	free the space occupied by the processors
	///////////////
	delete mpGradientProc;
	mpGradientProc = NULL;

	delete mpBitmapProc;
	mpBitmapProc = NULL;

	// Call base class to tidy up
	AIEPSFilter::CleanUpAfterImport(Successful);
}

/********************************************************************************************

>	void AI5EPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if the token is an AI EPS token; FALSE if not.
	Purpose:	Compare the current token against the AI keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::LookUpToken; EPSFilter::DecodeToken

********************************************************************************************/

void AI5EPSFilter::LookUpToken()
{
	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (AI5Commands[i].Cmd != EPSC_Invalid)
	{
		if (camStrcmp(TokenBuf, AI5Commands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = AI5Commands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	AIEPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL AI5EPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set, or
				which need to be handled differently to the standard Illustrator meanings.
				i.e. this is the function that handles all the AI EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL AI5EPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	// (ChrisG 12/12/00) these variables are used when processing the RGB colour commands (Xa, 
	//	XA, Xx and XX)
	INT32		red = 0;
	INT32		green = 0;
	INT32		blue = 0;
	DocColour	DocCol;
	TintType	Tint = TINT_NONE;
	FIXEDPOINT	TintVal;
	String_64	ColName;

	// Decode the command, and execute it...
	switch (Token)
	{
		case EPSC_Bd:
		{
			if ( !mpGradientProc || !mpGradientProc->DecodeBd( *this ) )
			{
				goto EPSError;
			}
			break;
		}

		case EPSC_Bm:
		case EPSC_Bc:
			// "The two imaging operators specific to gradients are Bm (gradient matrix) and
			// Bc (gradient cap). Although required for imaging, these operators are not
			// essential to Adobe Illustrator file formats."
			if (!Stack.Discard(6))
				goto EPSError;
			break;

		case EPSC__Bs:
		case EPSC__BS:
		{
			if ( !mpGradientProc || !mpGradientProc->DecodeBS( *this ) )
			{
				goto EPSError;
			}
			break;
		}

		case EPSC_BD:
		{
			// The end of the definition - lose the rampant '[' from the input stream.
			EPSCommand Cmd;
			if (!Stack.PopCmd(&Cmd) || (Cmd != EPSC_ArrayStart))
				goto EPSError;
			break;
		}

		case EPSC_Bg:
		{
			// A grad fill attribute is being used...

			// First get rid of the strange "1 0 0 1 0 0" bit wot I don't understand yet.
			if (!Stack.Discard(6))
				goto EPSError;

			// Get the Start point, angle, and length of the gradient fill line.
			DocCoord StartPoint;
			double Angle, Length;
			if (!Stack.Pop(&Length) || 
				!Stack.Pop(&Angle)  ||
				!Stack.PopCoordPair(&StartPoint))
				goto EPSError;

			// Get the fillname - we need this to get the rest of the fill details.
			// (Also lose the random number before it that I don't understand yet)
			String_64 FillName;
			if (!Stack.Pop(&FillName) || !Stack.Discard())
				goto EPSError;

			// Work out what all this means!
			DecodeAI5GradFill(StartPoint, Angle, Length, FillName);

			break;
		}

		case EPSC_Bb:
			// begin gradient instance
			TRACE(_T("Ignoring token Bb\n"));
			break;

		case EPSC__Br:
		{
			// gradient ramp
			TRACE(_T("Ignoring token %%_Br\n"));
			//	determine the number of tokens we can discard
			INT32 rampType = 0;
			if (!Stack.Pop(&rampType))
				goto EPSError;

			static const UINT32 sNumToDiscard[] =
			{
				1, 4, 5, 6, 7, 8, 9
			};

			if ( rampType < 0 || rampType > 6 )
				goto EPSError;

			if (!Stack.Discard(sNumToDiscard[rampType]))
				goto EPSError;

			break;
		}

		case EPSC_Lb:
			// Process the layer.
			if ( !DecodeLayer () )
			{
				TRACE( _T("There's been a problem with a layer!") );
			}

			break;

		case EPSC_Ln:
			// Decode the layer's name.
			if ( mpLayerProc == NULL || !mpLayerProc->DecodeLn ( *this ) )
			{
				goto EPSError;
			}

			break;

		case EPSC_LB:
			// End layer
			if ( mpLayerProc == NULL || !mpLayerProc->DecodeLB ( *this ) )
			{
				goto EPSError;
			}
			else
			{
				delete mpLayerProc;
				mpLayerProc = NULL;
			}
			break;

		case EPSC_Bh:
			// gradient highlights
			if ( !mpGradientProc || !mpGradientProc->DecodeBh( *this ) )
			{
				goto EPSError;
			}
			break;

		case EPSC_Ap:
			// path centre point
			TRACE(_T("Ignoring token Ap\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_Ar:
			// path resolution
			TRACE(_T("Ignoring token Ar\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_XR:
			//	fill rule
			//	0 = use non-zero winding number fill rule
			//	1 = use even-odd fill rule
			TRACE(_T("Ignoring token XR\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;


		case EPSC_BB:
			// end gradient instance
			TRACE(_T("Ignoring token BB\n"));
			if (!Stack.Discard())
				goto EPSError;
			break;

		case EPSC_Xm:
			// a b c d x y Xm 
			//	The Xm operator and its parameters are additional information written out as
			// part of a linear gradient definition. 
			TRACE(_T("Ignoring token Xm\n"));
			if (!Stack.Discard(6))
				goto EPSError;
			break;

		case EPSC_XT:
			// object tag
			// potential tags to process include /AdobeURL
			TRACE(_T("Ignoring token XT\n"));
			if (!Stack.Discard(2))
				goto EPSError;
			break;

		case EPSC_XI:
			// image definition
			if ( !mpBitmapProc || !mpBitmapProc->DecodeXI( *this ) )
				goto EPSError;
			break;

		case EPSC_XG:
			// image (via path) operator
			// we can ignore this
			if (!Stack.Discard(2))
				goto EPSError;
			break;

		case EPSC_HexStart:
			//	the lexical analyzer of lexfile doesn't process this so we have to
			//	it's the start of a hex string. 
			//	if we're reading a %_Br token we can ignore it
			if ( mbReadingGradFill )
				//	push a bogus hex value onto the stack so that %_Br can read it
				if ( !Stack.Push( INT32(0xffffffff) ) )
					goto EPSError;
				IgnoreHexData();
			break;

		case EPSC_HexEnd:
			//	the lexical analyzer of lexfile doesn't process this so we have to
			if ( mbReadingGradFill )
				break;

		case EPSC_Xh:
			// The Xh token re-iterates information from the XI image definition.
			// As a result, we can ignore it.
			TRACE(_T("Ignoring token Xh\n"));
			if (!Stack.Discard(2))
			{
				goto EPSError;
			}
			else
			{
				// Graeme (13/6/00) - Reduced the strength of the error checking. It
				// shouldn't really matter for this operator since it's ignored.
				Stack.DiscardArray();
			}
		break;

		case EPSC_XH:
			// End of bitmap declaration, but this token is unused by Camelot.
			TRACE(_T("Ignoring token XH\n"));
			break;

		case EPSC_XP:
			// don't know what this is but we can ignore it
			TRACE(_T("Ignoring unknown token XP\n"));
			if (!Stack.Discard(4))
				goto EPSError;
			break;


			// RGB Custom colours (fill, stroke)
		case EPSC_Xx:
		case EPSC_XX:
			Tint = TINT_ILLUSTRATOR;

			// remove 'type' info - we only support RGB versions.
			if (!Stack.Discard (1))
				goto EPSError;

			// RGB standard colours (fill, stroke)
		case EPSC_Xa:
		case EPSC_XA:

			// Set current line/fill colour (CMYK)
			if (Stack.PopColourRGB(&red, &green, &blue, Tint, &TintVal, &ColName))
			{
				GetEPSColourRGB(&DocCol, red, green, blue, Tint, TintVal, &ColName);

				// Remember this colour for future objects
				if ((Token == EPSC_Xa) || (Token == EPSC_Xx))
				{
			    	if (!SetFillColour(DocCol))
						goto EPSError;
				}
				else
				{
					if (!SetLineColour(DocCol))
						goto EPSError;
				}
			}
		    else	
		    	// Invalid colour operands
				goto EPSError;

			break;

		default:
			// Token not understood - pass on to base class
			return AIEPSFilter::ProcessToken();
	}


	// No errors encountered while parsing this token and its operands.
	return TRUE;
	
	
	// Error handlers:
EPSError:
	HandleEPSError();
	return FALSE;
}


/********************************************************************************************

>	void AI5EPSFilter::IgnoreHexData()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/03/00
	Purpose:	Skips hex data in the import stream (used for %_Br)

********************************************************************************************/

BOOL AI5EPSFilter::IgnoreHexData()
{
	while (!EPSFile->eof())
	{
		// Get the next token from the file
		if (!EPSFile->GetHexToken())
			return TRUE;

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
	}

	return FALSE;
}


/********************************************************************************************

>	void AI5EPSFilter::DecodeAI5GradFill(const DocCoord& StartPoint, 
						   			 double Angle, 
						   			 double Length, 
						   			 const String_64& FillName)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/94
	Inputs:		StartPoint - the start of the grad fill arrow.
				Angle 	   - the angle of the grad fill arrow.
				Length	   - the length of the grad fill arrow.
				FillName   - the gradient fill to use (i.e. what colours and type)
	Purpose:	Given the AI5 format infor on a grad fill, convert it into a Camelot
				grad fill, and set it as the current attribute.
	SeeAlso:	AI5EPSFilter::ProcessFilterComment

********************************************************************************************/

void AI5EPSFilter::DecodeAI5GradFill(const DocCoord& StartPoint, 
						   			 double Angle, 
						   			 double Length, 
						   			 const String_64& FillName)
{
	if ( !mpGradientProc ) 
		return;
	// First find this named gradient fill
	AI5Gradient* pGradient = mpGradientProc->FindGradient( FillName );

	// Did we find it?
	ENSURE(pGradient != NULL, "Could not find named grad fill in AI5 file!");
	if (pGradient == NULL)
		// No - oh dear just pretend we didn't notice and use current fill instead
		return;

	// Convert the length into millipoints.
	Length *= 1000;

	// Ok - got the named fill; first convert the angle+length into an end-point.
	DocCoord EndPoint;
	double Radians = (((double) Angle) / 180.0) * PI;
	
	EndPoint.x = StartPoint.x + (MILLIPOINT) (Length * cos(Radians));
	EndPoint.y = StartPoint.y + (MILLIPOINT) (Length * sin(Radians));

	// We appear to have all the info we need - set the gradient fill.
	if (pGradient->IsRadial)
	{
		if ( pGradient->mpHighlight )	// we have a highlight so make it a conical fill
		{
			SetConicalFill( pGradient->StartColour, pGradient->EndColour,
				        StartPoint, EndPoint);
		}
		SetRadialFill(pGradient->StartColour, pGradient->EndColour,
					  StartPoint, EndPoint);
	}
	else
	{
		SetLinearFill(pGradient->StartColour, pGradient->EndColour,
					  StartPoint, EndPoint);
	}

	////////////////////
	// if we have only a gradient fill use that instead
	//	first get the attribute we just set...
	////////////////////

	// Find out which fill attribute to change
	AttributeEntry* const pEntry = FillAttr.pAttr ? &FillAttr : &CurrentAttrs[ATTR_FILLGEOMETRY];

	// We've got an attribute - change it.
	GradFillAttribute* pAttr = static_cast<GradFillAttribute*>( pEntry->pAttr );

	// (ChrisG 3/4/2001) Added support for importing profiles. 
	//
	//	If this has a colour ramp, then add it in. If it doesn't, but it has a profile, then
	//	set that up instead.
	if ( pGradient->mpCurrentRamp &&
		 pGradient->mpCurrentRamp->GetCount ())
	{
		pAttr->SetColourRamp( pGradient->mpCurrentRamp );

		// Clean up any old profiling information, so that we make sure that this fill isn't 
		//	profiled, as profiling doesn't work with multi-stage fills.
		CProfileBiasGain profile;
		pAttr->SetProfile (profile);
	}
	else if (pGradient->midPoint != 0)
	{
		// Convert profile from	the 0 to 100 range into the -1 to +1 range, then set it.
		CProfileBiasGain profile = pAttr->GetProfile ();
		profile.SetBias (1 - ((double) pGradient->midPoint/50));
		pAttr->SetProfile (profile);
	}
}

/********************************************************************************************

>	BOOL AI5EPSFilter::DecodeLayer ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/00
	Inputs:		-  
	Returns:	TRUE	- The command was processed.
				FALSE	- It wasn't used by this filter.
	Purpose:	Inserts a layer into the tree.

********************************************************************************************/

BOOL AI5EPSFilter::DecodeLayer ( void )
{
	// If there's still an active layer, the LB token has been missed out, so throw an EPS
	// error message.
	if ( mpLayerProc != NULL )
	{
		return FALSE;
	}

	// Create the new layer processor.
	mpLayerProc = new AILayerProcessor;

	ERROR2IF( mpLayerProc == NULL, FALSE, "Insufficient memory to create layer processor.");

	// Insert the layer into the tree.
	return mpLayerProc->DecodeAI5Lb ( *this );
}

/********************************************************************************************

>	BOOL AI5EPSFilter::ProcessFilterComment()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/04/94
	Returns:	TRUE if the comment was processed, 
				FALSE if not used by this filter..
	Purpose:	Decodes the EPS comments that specify Illustrator 5 grad fills.

********************************************************************************************/

BOOL AI5EPSFilter::ProcessFilterComment()
{
	BOOL ok = TRUE;

	if (camStrncmp(TokenBuf, _T("%AI5_BeginGradient"), 18) == 0)
	{
		ENSURE(!mbReadingGradFill, "Already reading a grad fill!");

		if ( !mpGradientProc )
		{
			mpGradientProc = new AIGradientProcessor;
			if ( !mpGradientProc )
				ok = FALSE;
		}

		if (ok)
			ok = mpGradientProc->BeginGradient();

		if (ok)
			mbReadingGradFill = TRUE;

		// (this is a (close) copy of the loop in the main importing section).
		//	we need to do this because the definition is in the setup header
		//  which doesn't pass tokens back
		do
		{
			GetToken();
		}
		while (mbReadingGradFill && HandleToken() && (!EPSFile->eof()));
	
		// Put the token back onto the input stream to allow the caller to use it.
		EPSFile->UngetToken();
	}
	else if (camStrncmp(TokenBuf, _T("%AI5_EndGradient"), 16) == 0)
	{
		ENSURE(mbReadingGradFill, "Not reading a grad fill!");

		if ( mpGradientProc )
			ok = mpGradientProc->EndGradient( *this );

		if (ok)
			mbReadingGradFill = FALSE;

		return ok;
	}
	else if (camStrncmp(TokenBuf, _T("%AI5_BeginRaster"), 16) == 0)
	{
		ENSURE(!mbReadingBitmap, "Already reading a bitmap!");

		if ( !mpBitmapProc )
		{
			mpBitmapProc = new AIBitmapProcessor;
			if ( !mpBitmapProc )
				ok = FALSE;
		}

		if ( mpBitmapProc )
		{
			ok = mpBitmapProc->BeginRaster();
		}

		if (ok)
		{
			// Found the start of a bitmap definition
			mbReadingBitmap	= TRUE;
		}

		// All done - don't pass this on to document components.
		return ok;
	}
	else if (camStrncmp(TokenBuf, _T("%AI5_EndRaster"), 14) == 0)
	{
		ENSURE(mbReadingBitmap, "Not reading a bitmap!");

		/////////////////
		// add bitmap to tentative bitmap list, giving an index number
		/////////////////

		if ( mpBitmapProc )
		{
			ok = mpBitmapProc->EndRaster();
		}

		mbReadingBitmap	= FALSE;

		return ok;
	}
	else if (camStrncmp(TokenBuf, _T("%AI8_BeginMesh"), 14) == 0)
	{
		/////////////////
		// completely ignore the mesh definition 'cos it isn't in the current documentation
		/////////////////

		do
		{
			GetToken();
		}
		while ((camStrncmp(TokenBuf, _T("%AI8_EndMesh"), 12) != 0) && (!EPSFile->eof()));

		return TRUE;
	}
	else if ( camStrncmp ( TokenBuf, _T ( "%AI3_BeginPattern" ), 17 ) == 0 )
	{
/*		// Get the fillname - we need this to get the rest of the fill details.
		String_64 PatternName;

		GetLineToken ();

		Stack.Discard ( 4 );

		if ( !Stack.Pop ( &PatternName ) || !Stack.Discard () )
			return FALSE;
*/
		return TRUE;
	}
	else if ( camStrncmp ( TokenBuf, _T( "%AI3_EndPattern" ), 15 ) == 0 )
	{
		// NOP for now.
//		INT32 NOP = 0;
		return TRUE;
	}
	else if ( camStrncmp ( TokenBuf, _T ( "%AI6_BeginPatternLayer" ), 22 ) == 0 )
	{
		// NOP for now.
//		INT32 NOP = 0;
		return TRUE;
	}
	else if (camStrncmp(TokenBuf, _T("%AI6_EndPatternLayer"), 20) == 0)
	{
		// NOP for now.
//		INT32 NOP = 0;
		return TRUE;
	}

	// Otherwise, we don't want this comment
	return FALSE;
}

/********************************************************************************************

>	TCHAR *AI5EPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_aoa
	Returns:	Pointer to the string representation of the token, e.g. "aoa"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

TCHAR *AI5EPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (AI5Commands[i].Cmd != EPSC_Invalid)
	{
		if (AI5Commands[i].Cmd == Cmd)
			return AI5Commands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return AIEPSFilter::GetEPSCommand(Cmd);
}

/********************************************************************************************

>	BOOL AI5EPSFilter::CreateLayer ( String_256	&LayerName,
									 BOOL		IsLocked,
									 BOOL		IsPrintable,
									 BOOL		IsVisible )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/00
	Inputs:		LayerName	- The name of the new layer.
				IsLocked	- Is the layer locked?
				IsPrintable	- Is the layer printable?
				IsVisible	- Is the layer visible?
	Returns:	TRUE		- Success.
				FALSE		- Failure - the layer wasn't created.
	Purpose:	Creates a new layer using the AddLayer () method in the EPSFilter class.
				The returned pointer is then used so that I can set the layer property flags
				individually.
	SeeAlso:	EPSFilter::AddLayer ()

********************************************************************************************/

BOOL AI5EPSFilter::CreateLayer ( String_256	&LayerName,
								 BOOL		IsLocked,
								 BOOL		IsPrintable,
								 BOOL		IsVisible )
{
	// Create the new layer.
	Layer	*pLayer	= AddLayer ( LayerName, FALSE );

	// Check that things worked.
	if ( pLayer != NULL )
	{
		// It went OK - set the flags.
		pLayer->SetLocked		( IsLocked );
		pLayer->SetPrintable	( IsPrintable );
		pLayer->SetVisible		( IsVisible );

		// Success!
		return TRUE;
	}
	else
	{
		// A problem occurred.
		return FALSE;
	}
}
