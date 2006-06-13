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

// Implemenation of Corel EPS filters.

/*
*/

#include "camtypes.h"

#include "coreleps.h"

#include <sstream>

#include "nodepath.h"
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "tim.h"
//#include "oilfltrs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cdrfiltr.h"


DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW

CC_IMPLEMENT_DYNAMIC(CorelEPSFilter, EPSFilter)
CC_IMPLEMENT_DYNAMIC(Corel3EPSFilter, CorelEPSFilter)
CC_IMPLEMENT_DYNAMIC(Corel4EPSFilter, CorelEPSFilter)


// All the commands used by Corel X.x EPS files.
enum
{
	EPSC_cd_0sv = EPSC_EOF + 1,
	EPSC_cd_0sm,
	EPSC_cd_0rs,
	EPSC_cd_0rax,
	EPSC_cd_0B,
	EPSC_cd_0E,
	EPSC_cd_0G,
	EPSC_cd_0g,
	EPSC_cd_0j,
	EPSC_cd_0J,
	EPSC_cd_0w,
	EPSC_cd_0c,
	EPSC_cd_0t,
	EPSC_cd_0tm,
	EPSC_cd_a,
	EPSC_cd_r,
	EPSC_cd_x,
	EPSC_cd_X,
	EPSC_cd_e,
	EPSC_cd_z,
	EPSC_cd_Z,
	EPSC_cd_T,
	EPSC_cd_m,
	EPSC_matrix,
	EPSC_currentmatrix,
	EPSC_def,
	EPSC_begin,
	EPSC_cd_Sentinel
};

// This is the array of Corel EPS command/keyword names.
CommandMap CorelEPSFilter::CorelCommands[] =
{
	EPSC_cd_0sv,		"@sv",
	EPSC_cd_0sm,		"@sm",
	EPSC_cd_0rs,		"@rs",
	EPSC_cd_0rax,		"@rax",
	EPSC_cd_0B, 		"@B",
	EPSC_cd_0E, 		"@E",
	EPSC_cd_0G, 		"@G",
	EPSC_cd_0g, 		"@g",
	EPSC_cd_0j, 		"@j",
	EPSC_cd_0J, 		"@J",
	EPSC_cd_0w, 		"@w",
	EPSC_cd_0c, 		"@c",
	EPSC_cd_0t,			"@t",
	EPSC_cd_0tm,		"@tm",
	EPSC_cd_a,			"a",
	EPSC_cd_r,			"r",
	EPSC_cd_x,			"x",
	EPSC_cd_X,			"X",
	EPSC_cd_e,			"e",
	EPSC_cd_z,			"z",
	EPSC_cd_Z,			"Z",
	EPSC_cd_T,			"T",
	EPSC_cd_m, 			"m",

	EPSC_matrix,		"matrix",
	EPSC_currentmatrix,	"currentmatrix",
	EPSC_def,			"def",
	EPSC_begin,			"begin",

	// Sentinel
	EPSC_Invalid,		"Invalid"
};

/********************************************************************************************

>	CorelEPSFilter::CorelEPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an CorelEPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

CorelEPSFilter::CorelEPSFilter()
{
	// Set up filter description.
	FilterID = FILTERID_NONE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

	// Corel EPS does not store layer information
	SupportsLayers = FALSE;
}

/********************************************************************************************

>	BOOL CorelEPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an CorelEPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL CorelEPSFilter::Init()
{
	// All ok
	return TRUE;
}

/********************************************************************************************

>	void CorelEPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Purpose:	Compare the current token against the Corel keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::LookUpToken

********************************************************************************************/

void CorelEPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (CorelCommands[i].Cmd != EPSC_Invalid)
	{
		if (camStrcmp(TokenBuf, CorelCommands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = CorelCommands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - pass on to base class.
	EPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL CorelEPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set.
				i.e. this is the function that handles all the Corel EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL CorelEPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	DocCoord 	Coords[3];
	INT32		Long;
	FIXEDPOINT	FixedPoint;
	PColourCMYK	Col;
	DocColour	DocCol;
	String_64	ColName;
	FIXEDPOINT	TintVal;

	// Decode the command, and execute it...
	switch (Token)
	{
		case EPSC_S:
		case EPSC_b:
		case EPSC_B:
		case EPSC_f:
		case EPSC_F:
		case EPSC_s:
			if (ThePathType == PATH_DISCARD)
			{
				// Discard this path, but try to use the next one.
				ThePathType = PATH_NORMAL;
				break;
			}
			
			if (ThePathType == PATH_DISCARD_STICKY)
				// Keep discarding paths until explicitly told not to.
				break;

			ENSURE((ThePathType == PATH_NORMAL) || 
				   (ThePathType == PATH_RECT) || 
				   (ThePathType == PATH_ELLIPSE), "No path in stroke!");
						
			// If no path being constructed, probably just some strange masking going on
			// that we can't cope with yet, so ignore it.
			if (pPath == NULL)
				break;

			// Terminate path - always have to do this with Corel EPS, as it doesn't
			// do it itself.
			if ((Token == EPSC_s) || (Token == EPSC_b) || (Token == EPSC_f))
			{
				if (!pPath->InkPath.CloseSubPath())
					// Not enough dynamic heap to insert the final element of the path
					goto NoMemory;
			}

			// Make the path filled or not, depending on the EPS token.
			// NB. The IsFilled flag is set here too - this is an area in which
			//	   Corel EPS is different to AI/AW/Camelot EPS.
			if ((Token == EPSC_b) || 
				(Token == EPSC_B) || 
				(Token == EPSC_F) || 
				(Token == EPSC_f))
			{
				pPath->InkPath.IsFilled = TRUE;
				SetPathFilled(TRUE);
			}
			else
			{
				pPath->InkPath.IsFilled = FALSE;
				SetPathFilled(FALSE);
			}

			if (!EPSFlags.ComplexPath)
			{
				// Not a complex path, so we terminate properly.
				pPath->InvalidateBoundingRect();
							
				// Add the attributes, if we haven't done so for this path
				if (EPSFlags.NoAttributes)
				{
					if (!AddAttributes(pPath, (Token != EPSC_f) && (Token != EPSC_F), pPath->InkPath.IsFilled))
						goto NoMemory;
					EPSFlags.NoAttributes = FALSE;
				}
				
				// Add this path into the tree
				if (!AddNewNode(pPath))
					goto NoMemory;

				// We've finished building this path - set to NULL so the other routines
				// know that we're not building a path any more.
				pPath = NULL;
				pInkPath = NULL;
			}
			else
				EPSFlags.StrokeComplexPath = (Token != EPSC_f) && (Token != EPSC_F);

			// Handle the next path normally
			ThePathType = PATH_NORMAL;
			break;

		case EPSC_cd_0sv:
		case EPSC_cd_0sm:
		case EPSC_cd_0rs:
		case EPSC_cd_0rax:
			// Ignore these commands - not needed
			break;

		case EPSC_cd_0E:
			// Bounding box of object - ignore it; we do it ourselves
			if (!Stack.Discard(4))
				goto EPSError;
			break;

		case EPSC_cd_0B:
		case EPSC_cd_a:
		case EPSC_cd_r:
			// We don't need these commands - it seems safe to ignore them.
			// I think they're something to do with text handling, so when we do text
			// imoport, we'll need to process them properly.
			break;

		case EPSC_cd_0G:
		case EPSC_cd_0g:
			// Check the flag on the stack
			if (!Stack.Pop(&Long))
				goto EPSError;
			if (Long == 1)
			{
				// Discard the 3 top values from the stack
				if (!Stack.Discard(3))
					goto EPSError;
			}
			break;

		case EPSC_cd_0J:
		case EPSC_cd_0j:
			// Save/restore not needed - ignore
			break;

		case EPSC_cd_0w:
			// NOT IMPLEMENTED IN FULL
			// Set current line width to 1 (defined in points)
			if (!Stack.Discard(2))
				goto EPSError;
				
			if (Stack.Pop(&FixedPoint))
			{
				// Scale line width from points to millipoints, and remember for future
				// objects.
		    	if (!SetLineWidth((MILLIPOINT) (FixedPoint * EPSScaleFactor) / FixedPointScale))
					goto NoMemory;

				// Check the flag
				if (Stack.Pop(&Long))
				{
					if (Long == 1)
					{
						if (!Stack.DiscardArray())
							goto EPSError;
					}
				}
				else
					goto EPSError;
			}
		    else	
				goto EPSError;
			break;

		case EPSC_cd_0c:
			// Close the current path  
			if (pPath == NULL)
				// No path to close!
				goto EPSError;

			if (!pPath->InkPath.CloseSubPath())
				// Not enough dynamic heap to insert the final element of the path
				goto NoMemory;
			break;

		case EPSC_cd_0t:
			// Display text - ignore for now; discard (x, y, string)
			if (!Stack.Discard(3))
				goto EPSError;
			break;
				
		case EPSC_cd_0tm:
			{
				// Set text matrix - use this to update the stack's transform matrix, as
				// otherwise pathified text will not be imported correctly.
				EPSCommand Cmd;
				Stack.PopCmd(&Cmd);
				if (Cmd != EPSC_ArrayEnd)
				{
					if (IsUserName("Tim"))
						TRACE( _T("@tm: Expected ArrayEnd, found '%s'\n"), GetEPSCommand(Cmd));
					goto EPSError;
				}

				// Extract the six array values from the stack.
				double M[6];
				INT32 i = 5;
				for (i = 5; i >= 0; i--)
				{
					if (!Stack.Pop(&M[i]))
						goto EPSError;
				}

				Stack.PopCmd(&Cmd);
				if (Cmd != EPSC_ArrayStart)
				{
					if (IsUserName("Tim"))
						TRACE( _T("@tm: Expected ArrayStart, found '%s'\n"), GetEPSCommand(Cmd));
					goto EPSError;
				}

				// Convert the abcd values into FIXED16s, cos that's what we use.
				FIXED16 F16[4];
				for (i = 0; i < 4; i++)
				{
					F16[i] = FIXED16(M[i]);
				}

				// Convert the Tx and Ty to MILLIPOINTS, cos that's what we use.
				INT32 L1 = (INT32) (M[4] * 1000.0);
				INT32 L2 = (INT32) (M[5] * 1000.0);

				// Construct the matrix and tell the EPS stack to use it for future 
				// coordinates.
				Matrix TextMatrix(F16[0], F16[1], F16[2], F16[3], L1, L2);
				Stack.SetXformMatrix(TextMatrix);

				break;
			}
				
		case EPSC_cd_x:
			// Set current fill colour (CMYK)
			GradFill = FALSE;
			if (Stack.PopColour(&Col, TINT_COREL, &TintVal, &ColName))
			{
				GetEPSColour(&DocCol, &Col, TINT_COREL, TintVal, &ColName);

				// Remember this fill colour for future objects
		    	if (!SetFillColour(DocCol))
					goto NoMemory;
			}
		    else
		    	// Invalid colour operands
				goto EPSError;
			break;
			
		case EPSC_cd_X:
			// Set current line colour (CMYK)
			if (Stack.PopColour(&Col, TINT_COREL, &TintVal, &ColName))
			{
				GetEPSColour(&DocCol, &Col, TINT_COREL, TintVal, &ColName);

				// Remember this line colour for future objects
				if (!SetLineColour(DocCol))
					goto NoMemory;
			}
		    else	
		    	// Invalid colour operands
				goto EPSError;
			break;

		case EPSC_cd_e:
			// Set text fill mode - ignore
			break;

		case EPSC_cd_z:
			// Get and scale font - ignore; discard font name and scale
			if (!Stack.Discard(2))
				goto EPSError;
			break;
				
		case EPSC_cd_Z:
			// Change font encosing vector - ignore; discard font names and encoding vector.
			if (!Stack.Discard(3) || !Stack.DiscardArray())
				goto EPSError;
			break;
				
		case EPSC_cd_T:
			// Stop using text matrix for transformations.
			Stack.SetNoXformMatrix();
			break;

		// MoveTo
		case EPSC_cd_m:
			// Get the co-ordinate from the stack
			if (Stack.PopCoordPair(&Coords[0]))
			{
				if (pPath == NULL)
				{
					ERROR3IF(pInkPath != NULL,"Already got a path");
					if (pInkPath != NULL)
						goto EPSError;

					// Create a new NodePath object
					pPath = new NodePath;
					
					// No attributes on the path yet
					EPSFlags.NoAttributes = TRUE;
					
					if ((pPath == NULL) || (!pPath->SetUpPath()))
						// Not enough memory to initialise path
						goto NoMemory;
					
					// Position tag at start of path.
					pPath->InkPath.FindStartOfPath();
					// Point inkpath pointer at the nodepath's path object.
					pInkPath = &pPath->InkPath;
				}
				
				// Insert a moveto into the path
				if (!pPath->InkPath.InsertMoveTo(Coords[0]))
					// Not enough dynamic heap to insert the moveto command
					goto NoMemory;
			}
			else
				// Invalid number/type of coordinate operands
				goto EPSError;
			break;

		case EPSC_matrix:
			// NOT IMPLEMENTED IN FULL
			// Push dummy argument on
			Stack.Push((INT32) 0);
			break;

		case EPSC_currentmatrix:
			// NOT IMPLEMENTED IN FULL
			// Ignore this command
			break;

		case EPSC_def:
			// Pretend to do a 'def' - discard name and defn from stack
			if (!Stack.Discard(2))
				goto EPSError;
			break;

		case EPSC_begin:
			// Pretend to do a 'begin' - discard dictionary
			if (!Stack.Discard())
				goto EPSError;
			break;

		default:
			// Token not understood - pass onto base class.
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

>	char *CorelEPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_cd_c
	Returns:	Pointer to the string representation of the token, e.g. "@c"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

char *CorelEPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (CorelCommands[i].Cmd != EPSC_Invalid)
	{
		if (CorelCommands[i].Cmd == Cmd)
			return CorelCommands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return EPSFilter::GetEPSCommand(Cmd);
}

/********************************************************************************************

>	BOOL CorelEPSFilter::DecodeCorelGradFill()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/04/94
	Returns:	TRUE if grad fill converted without problems;
				FALSE if not.
	Purpose:	Convert the preposterous Corel format of graduated fill information into
				something that resembles sanity, and install it as the current fill.
				The graduated fill information is assumed to be in the
				CorelEPSFilter::GradFillInfo structure.

********************************************************************************************/

BOOL CorelEPSFilter::DecodeCorelGradFill()
{
	switch (GradFillInfo.FillType)
	{
		case 0:
			return DecodeLinearGradFill();

		case 1:
		case 3:		// square - radial is closest approximation
			return DecodeRadialGradFill();

		case 2:
			return DecodeConicalGradFill();
	}

	// Can't handle any other kinds of fills - ignore them.
	return TRUE;
}

/********************************************************************************************

>	BOOL CorelEPSFilter::DecodeLinearGradFill()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/94
	Returns:	TRUE if grad fill converted without problems;
				FALSE if not.
	Purpose:	Convert the Corel grad values (padding, angle of fill etc) into a Camelot
				style grad fill - i.e. start and end points, like Camelot.
				This function copes with all styles of Corel linear graduated fills.
	Errors:		Out of memory.
	SeeAlso:	CorelEPSFilter::DecodeCorelGradFill; CorelEPSFilter::DecodeRadialGradFill

********************************************************************************************/

BOOL CorelEPSFilter::DecodeLinearGradFill()
{
	// NB. this function is (hopefully!) over-complex and will be simplified.
	// However, it's like this atm so I can get my head around the weird maths 

	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// First, get the bounding box.
	DocRect BBox; 
	CDRFilter::GetCorelBBox(pPath, &BBox);

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// Find centre of box
	DocCoord Centre;
	Centre.x = BBox.lo.x + (Width / 2);
	Centre.y = BBox.lo.y + (Height / 2);

	// Find total area of BBox
	double TotalArea = (double) Width * (double) Height;

	// Cope with angles > 180
	BOOL Mirror = FALSE;
	INT32 Angle = GradFillInfo.Angle;
	
	if (Angle >= 180)
	{
		Angle -= 180;
		Mirror = TRUE;
	}
	else if (Angle < 0)
	{
		Angle += 180;
		Mirror = TRUE;
	}

	Angle += 90;

	if (Angle >= 180)
	{
		Angle -= 180;
	}

	// Calculate tan of the angle - convert angle to radians first.
	double Radians = (((double) Angle) / 180.0) * PI;
	double TanTheta;
	if (Angle == 90)
	{
		// Special case for horizontal grad fill arrow.

		// Make 0% padding first
		Start.x = BBox.lo.x;
		Start.y = Centre.y;
		End.x = BBox.hi.x;
		End.y = Centre.y;

		// Find out width of padding
		INT32 Padding = (Width * GradFillInfo.EdgePad) / 100;
		Start.x += Padding;
		End.x -= Padding;
	}
	else if (Angle == 0)
	{
		// Special case for vertical grad fill arrow.

		// Make 0% padding first
		Start.x = Centre.x;
		Start.y = BBox.lo.y;
		End.x = Centre.x;
		End.y = BBox.hi.y;

		// Find out width of padding
		INT32 Padding = (Height * GradFillInfo.EdgePad) / 100;
		Start.y += Padding;
		End.y -= Padding;
	}
	else
	{
		TanTheta = tan(Radians);

		// Find out what the maximum padding is that we can achieve using just triangles:

		// Find the maximum triangle width
		MILLIPOINT TriWidth = (MILLIPOINT) ((double) Height / TanTheta);

		// Limit it to sensible value
		if (TriWidth < 0)
			TriWidth = -TriWidth;
		if (TriWidth > Width)
			TriWidth = Width;

		// Find the maximum triangle width
		MILLIPOINT TriHeight = (MILLIPOINT) ((double) Width * TanTheta);

		// Limit it to sensible value
		if (TriHeight < 0)
			TriHeight = -TriHeight;
		if (TriHeight > Height)
			TriHeight = Height;

		// The 'c' values of the y = mx+c equation.
		MILLIPOINT StartC, EndC;

		// Work out the maximum percentage/edge padding this gives us
		// (50 because it's 100 / 2 because we want area of triangle, not rectangle).
		double Percentage = (50.0 * (double) TriWidth * (double) TriHeight) / TotalArea;

		INT32 Diff = 0;

		// Is this enough?
		if (((INT32) Percentage) >= GradFillInfo.EdgePad)
		{
			// Yes - calculate exactly how big the triangle needs to be.
			TriHeight = (MILLIPOINT) sqrt(ABS(((double) GradFillInfo.EdgePad * TotalArea * TanTheta) / 100.0));

			TriWidth = (MILLIPOINT) ((double) TriHeight / TanTheta);
			if (TriWidth < 0)
				TriWidth = -TriWidth;

			ENSURE(TriWidth < Width, "Error in Corel Grad fill decoding logic");
		}
		else
		{
			// How much percentage do we need to add with each rectangle?
			Percentage = (GradFillInfo.EdgePad - Percentage) / 2;

			// Handle the rectangle stuff.
			if (TriWidth == Width)
			{
				// Need to add rectangles to the top and bottom.
				Diff = (MILLIPOINT) ((Percentage * Height) / 100.0);
			}
			else
			{
				// Need to add rectangles to left and right
				Diff = (MILLIPOINT) ((Percentage * Width) / 100.0);
				Diff = (MILLIPOINT) (Diff / tan(PI - Radians));
				Diff = ABS(Diff);
			}
		}

		// Work out the C value for the start line (c = y - mx)
		// (m = tan(angle) )
		if (Angle == 90)
		{
			//ENSURE(FALSE, "90 degree angle found!");
		}
		else if (Angle < 90)
		{
			StartC = (MILLIPOINT) (BBox.lo.y - ((BBox.hi.x - TriWidth) * TanTheta));
			EndC = (MILLIPOINT) (BBox.hi.y - ((BBox.lo.x + TriWidth) * TanTheta));
		}
		else
		{
			StartC = (MILLIPOINT) (BBox.lo.y - ((BBox.lo.x + TriWidth) * TanTheta));
			EndC = (MILLIPOINT) (BBox.hi.y - ((BBox.hi.x - TriWidth) * TanTheta));
		}

		// Add on difference for rectangles, if any.
		StartC += Diff;
		EndC -= Diff;


		// Work out m and c for the grad fill line.
		// We know m is -1/m of the triangle's hypotenuse.
		// c = roy - (rox/m)
		double FillM = -1.00 / TanTheta;
		MILLIPOINT FillC = (MILLIPOINT) (Centre.y - (Centre.x * FillM));

		// Work out intersections:  x = (c2 - c1) / (2m)

		Start.x = (MILLIPOINT) ( (FillC - StartC) / (TanTheta + (1.00 / TanTheta)) );
		Start.y = (MILLIPOINT) ((FillM * Start.x) + FillC);

		End.x = (MILLIPOINT) ( (FillC - EndC) / (TanTheta + (1.00 / TanTheta)) );
		End.y = (MILLIPOINT) ((FillM * End.x) + FillC);
	}

	if (Mirror)
	{
		// Swap the grid fill end-points over.
		DocCoord Tmp = Start;
		Start = End;
		End = Tmp;
	}

	// Set the fill type according to these calculations.
	return SetLinearFill(StartColour, EndColour, Start, End);	
}

/********************************************************************************************

>	BOOL CorelEPSFilter::DecodeRadialGradFill()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Returns:	TRUE if grad fill converted without problems;
				FALSE if not.
	Purpose:	Convert the Corel grad values (padding, centre of fill etc) into a Camelot
				style radial fill - i.e. start and end points, like ArtWorks.
				The calculations may seem strange. They are - but that's how Corel define
				their grad fills.
	Errors:		Out of memory.
	SeeAlso:	CorelEPSFilter::DecodeCorelGradFill; CorelEPSFilter::DecodeLinearGradFill

********************************************************************************************/

BOOL CorelEPSFilter::DecodeRadialGradFill()
{
	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// First, get the bounding box.
	DocRect BBox; 
	CDRFilter::GetCorelBBox(pPath, &BBox);

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// caluculate the source area
	// first, what's the diagonal length
	double dWidth = Width;
	double dHeight = Height;
	INT32 Diagonal = (INT32)sqrt(dWidth*dWidth + dHeight*dHeight);

	// and from that calculate area of the box containing the bit of the
	// bit of the circle in the bbox
	INT32 Edge = (Diagonal * (100 - (GradFillInfo.EdgePad * 2))) / 100;

	// Start point is the centre given by Corel.
	// This centre is percentage offsets from the centre of the object, i.e. (0,0) is
	// the centre of the bounding box.
	DocCoord Centre = DocCoord(BBox.lo.x + (Width / 2), BBox.lo.y + (Height / 2));
	INT32 OffX = (GradFillInfo.RotateX * Width) / 100;
	INT32 OffY = (GradFillInfo.RotateY * Height) / 100;
	Start.x = Centre.x + OffX;
	Start.y = Centre.y + OffY;

	// Find required radius of circle.
	double Radius = Edge / 2;

	// how far away is the centre of the fill from the centre of the bbox?
	double dOffX = OffX;
	double dOffY = OffY;
	double Dist = (INT32)sqrt(dOffX*dOffX + dOffY*dOffY);

	// and increase the radius by a bodge factor
	double BodgeFactor = 1 + (Dist / (double)(Diagonal / 2));
	Radius *= BodgeFactor;
	
	// End point is start point + radius
	End.x = Start.x + ((MILLIPOINT) Radius);
	End.y = Start.y;

	// Seems that we need to swap start and end colours...

	// Set the fill type according to these calculations.
	return SetRadialFill(EndColour, StartColour, Start, End);	
}

/********************************************************************************************

>	BOOL CorelEPSFilter::DecodeConicalGradFill()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/04/94
	Returns:	TRUE if grad fill converted without problems;
				FALSE if not.
	Purpose:	Convert the Corel grad values (padding, centre of fill etc) into a Camelot
				style radial fill - i.e. start and end points, like ArtWorks.
	Errors:		Out of memory.
	SeeAlso:	CorelEPSFilter::DecodeCorelGradFill; CorelEPSFilter::DecodeLinearGradFill

********************************************************************************************/

BOOL CorelEPSFilter::DecodeConicalGradFill()
{
	// modified by Ben to get the start and end points right

	// The desired start and end points of the grad fill 'arrow'.
	DocCoord Start, End;

	// First, get the bounding box.
	DocRect BBox; 
	CDRFilter::GetCorelBBox(pPath, &BBox);

	// Calculate width and height
	MILLIPOINT Width = BBox.Width();
	MILLIPOINT Height = BBox.Height();

	// Start point is the centre given by Corel.
	// This centre is percentage offsets from the centre of the object, i.e. (0,0) is
	// the centre of the bounding box.
	Start.x = BBox.lo.x + (Width  / 2);
	Start.y = BBox.lo.y + (Height / 2);
 	Start.x += ((GradFillInfo.RotateX * Width) / 100);
	Start.y += ((GradFillInfo.RotateY * Height) / 100);

	// End point is start point + radius but takes into account the angle
	double Radius = Width / 2;
	// angle is * 10, and needs to be in radians
	double Theta = (((double)(GradFillInfo.Angle)) / 360.0) * (2 * PI);

	// make the angle go anti-clockwise
	Theta = 0 - Theta;

	// rotate by PI / 2
	Theta -= PI / 2;
	
	// angle can be negative, ensure it's positive
	while(Theta < 0)
		Theta += (2 * PI);

	// calculate the triangle
	double dx, dy;

	dx = Radius * sin(Theta);
	dy = Radius * cos(Theta);

	End.x = Start.x + (INT32)dx;
	End.y = Start.y + (INT32)dy;

	// Seems that we need to swap start and end colours...

	// Set the fill type according to these calculations.
	return SetConicalFill(EndColour, StartColour, Start, End);	
}


/********************************************************************************************

>	BOOL CorelEPSFilter::AddAttributes(NodeRenderableBounded *pNode, BOOL Stroked, BOOL Filled)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/04/94
	Inputs:		pNode - the bounded renderable node to add atributes to.
				Stroked - TRUE if the object to add attributes to should be stroked, 
						 FALSE if not.
				Stroked - TRUE if the object to add attributes to should be filled, 
						 FALSE if not.
	Returns:	TRUE if the attributes were added ok;
				FALSE if not.
	Purpose:	Used as a veneer to EPSFilter::AddAttributes.  It checks to see if the
				current fill style is a graduated fill - if it is, then it calls
				CorelEPSFilter::DecodeCorelGradFill, which converts the Corel form of the
				grad fill to the Camelot form.  This must be done for each object, as the
				grad fill is stored relative to the object's bounding box in Corel EPS files.
	Errors:		Out of memory.
	SeeAlso:	EPSFilter::AddAttributes; CorelEPSFilter::DeocdeCorelGradFill

********************************************************************************************/

BOOL CorelEPSFilter::AddAttributes(NodeRenderableBounded *pNode, BOOL Stroked, BOOL Filled)
{
	// Check to see if we need to set up a Corel graduated fill attribute
	if (GradFill)
	{
		GradFill = FALSE;
		// Try to set up the grad fill for this object.
		if (!DecodeCorelGradFill())									   
			return FALSE;
	}

	// Call the base class version
	return EPSFilter::AddAttributes(pNode, Stroked, Filled);
}


// All the commands used by Corel 3.x EPS files.
enum
{
	EPSC_cd3_0k = EPSC_cd_Sentinel + 1,
	EPSC_cd3_0x,
};

// This is the array of Corel EPS command/keyword names.
CommandMap Corel3EPSFilter::Corel3Commands[] =
{
	EPSC_cd3_0k, 		"@k",
	EPSC_cd3_0x, 		"@x",

	// Sentinel
	EPSC_Invalid,		"Invalid"
};

/********************************************************************************************

>	Corel3EPSFilter::Corel3EPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for a Corel3EPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

Corel3EPSFilter::Corel3EPSFilter()
{
	// Set up filter description.
	FilterID = FILTERID_COREL3_EPS;
	ImportMsgID = _R(IDT_IMPORTMSG_COREL3);

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif
}

/********************************************************************************************

>	BOOL Corel3EPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an Corel3EPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL Corel3EPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new Corel3EPSOILFilter(this);
	if (pOILFilter == NULL)
		return  FALSE;

	// Load the description strings
	FilterName.Load(_R(IDT_CORELEPS_FILTERNAME));
	FilterInfo.Load(_R(IDT_CORELEPS_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 Corel3EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an Corel
				generated EPS file, as required.

********************************************************************************************/

INT32 Corel3EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// Check the first line in EPS file
	if (camStrncmp((char *) pFileHeader, "%!PS-Adobe-2.0 EPSF", 19) != 0)
	{
		// Incorrect version of EPS header line - we don't want this
		return 0;
	}

	// !PS-Adobe line is ok - check creator line...
	istrstream HeaderFile((char *) pFileHeader, HeaderSize);
	char Buffer[200];
	BOOL HaveCreatorString = FALSE;

	UINT32 Lines = 0;
	while ((Lines < 20) && !HeaderFile.eof())
	{
		HeaderFile.getline(Buffer, 200);
		Lines++;

		if (camStrncmp(Buffer, "%%Creator: CorelDRAW!", 21) == 0)
		{
			HaveCreatorString = TRUE;
		}

		// Check for CorelDRAW exported by other packages
		if (camStrstr("Corel", Buffer) != NULL)
			// Found Corel in the creator line - good chance it's Corel EPS.
			return 8;

		if(camStrncmp(Buffer, "/wCorelDict", 11) == 0 && HaveCreatorString == TRUE)
		{
			// OK, found it
			return 10;
		}

		// If we find the compression token then stop the search as we don't want to start
		// looking in the compressed data!
		if (camStrncmp(Buffer, "%%Compression:", 14)==0)
			break;
	}

	// Didn't find a suitable Creator line, but we found an EPS header line.
	return 5;

	// May want to look for this string one day.
	#if 0
	if (camStrncmp(Buf, "% -------------- POSTSCRIPT PROLOG FOR CORELDRAW 3.X", 52) == 0)
		FoundProlog = TRUE;
	#endif
}

/********************************************************************************************

>	void Corel3EPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Purpose:	Compare the current token against the Corel 3 specific keywords to see if it 
				is one of them.
	SeeAlso:	EPSFilter::DecodeToken

********************************************************************************************/

void Corel3EPSFilter::LookUpToken()
{
	// Not interested in comments
	if (Token == EPSC_Comment)
		return;

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (Corel3Commands[i].Cmd != EPSC_Invalid)
	{
		if (camStrcmp(TokenBuf, Corel3Commands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = Corel3Commands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - try base class
	CorelEPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL Corel3EPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set.
				i.e. this is the function that handles all the Corel EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL Corel3EPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	PColourCMYK	Col;
	DocCoord 	Coords[3];
	TintType	Tint = TINT_NONE;
	FIXEDPOINT	TintVal;
	String_64	ColName;
	double		Padding, RotateX, RotateY;

	// Decode the command, and execute it...
	switch (Token)
	{
		case EPSC_cd3_0x:
			Tint = TINT_COREL;
		case EPSC_cd3_0k:
			// Get the grad fill parameters.
			if (!Stack.Pop(&RotateY) || !Stack.Pop(&RotateX) ||
				!Stack.Pop(&Padding) || !Stack.Pop(&GradFillInfo.FillType) ||
				!Stack.Pop(&GradFillInfo.Angle))
				goto EPSError;
			
			// Convert values from floating point to integer
			GradFillInfo.EdgePad = (INT32) (Padding * 100.0);
			GradFillInfo.RotateX = (INT32) (RotateX * 100.0);
			GradFillInfo.RotateY = (INT32) (RotateY * 100.0);

			// Get the end colour.
			if (!Stack.PopColour(&Col, Tint, &TintVal, &ColName))
		    	// Invalid colour operands
				goto EPSError;

			GetEPSColour(&EndColour, &Col, Tint, TintVal, &ColName);
//			EndColour.SetCMYKValue(&Col);

			// Get the start colour.
			if (!Stack.PopColour(&Col, Tint, &TintVal, &ColName))
		    	// Invalid colour operands
				goto EPSError;

			GetEPSColour(&StartColour, &Col, Tint, TintVal, &ColName);
//			StartColour.SetCMYKValue(&Col);

			GradFill = TRUE;
			break;
			
		default:
			return CorelEPSFilter::ProcessToken();
	}


	// No errors encountered while parsing this token and its operands.
	return TRUE;
	
	
	// Error handlers:
	
EPSError:
	HandleEPSError();
	return FALSE;
#if 0
NoMemory:
	HandleNoMemory();
	return FALSE;
#endif
}


/********************************************************************************************

>	char *Corel3EPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_cd_c
	Returns:	Pointer to the string representation of the token, e.g. "@c"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

char *Corel3EPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (Corel3Commands[i].Cmd != EPSC_Invalid)
	{
		if (Corel3Commands[i].Cmd == Cmd)
			return Corel3Commands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return CorelEPSFilter::GetEPSCommand(Cmd);
}





// All the commands used by Corel 4.x EPS files.
enum
{
	EPSC_cd4_0k = EPSC_cd_Sentinel + 1,
	EPSC_cd4_0x,
	EPSC_eoclip,
	EPSC_cd5_0gs,
	EPSC_cd5_0gr,
	EPSC_cd5_0np
};

// This is the array of Corel EPS command/keyword names.
CommandMap Corel4EPSFilter::Corel4Commands[] =
{
	EPSC_cd4_0k, 		"@k",
	EPSC_cd4_0x, 		"@x",
	EPSC_eoclip,		"eoclip",
	EPSC_cd5_0gs,		"@gs",
	EPSC_cd5_0gr,		"@gr",
	EPSC_cd5_0np,		"@np",

	// Sentinel
	EPSC_Invalid,		"Invalid"
};

/********************************************************************************************

>	Corel4EPSFilter::Corel4EPSFilter()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for a Corel4EPSFilter object.  The object should be 
				initialised	before use.
	SeeAlso:	EPSFilter::Init

********************************************************************************************/

Corel4EPSFilter::Corel4EPSFilter()
{
	// Set up filter description.
	FilterID = FILTERID_COREL4_EPS;
	ImportMsgID = _R(IDT_IMPORTMSG_COREL4);

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif
}

/********************************************************************************************

>	BOOL Corel4EPSFilter::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an Corel4EPSFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL Corel4EPSFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new Corel4EPSOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// Load the description strings
	FilterName.Load(_R(IDT_CORELEPS_FILTERNAME));
	FilterInfo.Load(_R(IDT_CORELEPS_FILTERINFO));

	// All ok
	return TRUE;
}

/********************************************************************************************

>	INT32 Corel4EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Returns:	TRUE if the header is ok and import should proceed, FALSE if not.
	Purpose:	Checks to see if the EPS comment headers specify that this is an Corel
				generated EPS file, as required.

********************************************************************************************/

INT32 Corel4EPSFilter::EPSHeaderIsOk(ADDR pFileHeader, UINT32 HeaderSize)
{
	// Check the first line in EPS file
	if (camStrncmp((char *) pFileHeader, "%!PS-Adobe-2.0 EPSF-2.0", 23) != 0)
	{
		// Incorrect version of EPS header line - we don't want this
		return 0;
	}

	// !PS-Adobe line is ok...

	// Scan first 20 lines for Corel 4.x's unique(ish) line
	// (If you have no prolog, then it won't be there)
	istrstream HeaderFile((char *) pFileHeader, HeaderSize);
	char Buffer[200];
	BOOL HaveCreatorString = FALSE;

	UINT32 Lines = 0;
	while ((Lines < 20) && !HeaderFile.eof())
	{
		HeaderFile.getline(Buffer, 200);
		Lines++;

		if (camStrncmp(Buffer, "%%BeginResource: procset wCorel4Dict", 36) == 0)
			// This must be Corel 4.0 EPS
			return 10;

		if ((camStrncmp(Buffer, "%%Creator:", 10) == 0) && (camStrclen(Buffer) <= 12))
		{
			// Blank Creator comment - this could be Corel 4.0 EPS
			if (IsUserName("Tim"))
				TRACE( _T("Found blank creator field\n"));

			// Return 6 rather than 5 so we're just above the other EPS filters in terms
			// of compatibility, because a blank Creator comment is a reasonably good
			// sign thatthis file came from Corel 4.
			return 6;
		}
		
		if ((camStrncmp(Buffer, "%%Creator: CorelDRAW!", 21) == 0) && (camStrclen(Buffer) <= 23))
		{
			// yep, that's the one
			HaveCreatorString = TRUE;
		}

		if(camStrncmp(Buffer, "/wCorel5Dict", 12) == 0 && HaveCreatorString)
		{
			// it's a Corel5 file
			return 10;
		}

	}

	// Corel 4.0 doesn't put anything in the Creator comment field - return
	// 5 to indicate we're interested, but not sure.
	return 5;

}

/********************************************************************************************

>	void Corel4EPSFilter::LookUpToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Purpose:	Compare the current token against the Corel keywords to see if it is
				one of them.
	SeeAlso:	EPSFilter::DecodeToken

********************************************************************************************/

void Corel4EPSFilter::LookUpToken()
{
//	// Not interested in comments
//	if (Token == EPSC_Comment)
//		return;

	if(Token == EPSC_Comment)
	{
		// is it one of the clipping things?
		if(camStrncmp(TokenBuf, "%SetClippingRegion", 17) == 0)
			Token = EPSC_q;
		else if(camStrncmp(TokenBuf, "%ClearClipping", 13) == 0)
			Token = EPSC_Q; 

		return;
	}

	// Check to see if it is a keyword - cycle through the array of keyword names and
	// compare against our token (could use a hash table?)
	INT32 i = 0;
	while (Corel4Commands[i].Cmd != EPSC_Invalid)
	{
		if (camStrcmp(TokenBuf, Corel4Commands[i].CmdStr) == 0)
		{
			// Found the token - set the token variable and return success
			Token = Corel4Commands[i].Cmd;
			return;
		}
		// Try next command
		i++;
	}

	// Did not find this token - try base class
	CorelEPSFilter::LookUpToken();
}

/********************************************************************************************

>	BOOL Corel4EPSFilter::ProcessToken()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/02/94
	Returns:	TRUE if token understood and processed ok, FALSE if not.
	Purpose:	Processes EPS tokens that are not part of the standard Illustrator set.
				i.e. this is the function that handles all the Corel EPS operators.
	Errors:		Syntax error in EPS, Out of memory.
	SeeAlso:	EPSFilter::ProcessToken

********************************************************************************************/

BOOL Corel4EPSFilter::ProcessToken()
{
	// Variables used to extract operands from the stack
	PColourCMYK	Col;
	DocCoord 	Coords[3];
	TintType	Tint = TINT_NONE;
	double		Padding, RotateX, RotateY;

	// Decode the command, and execute it...
	switch (Token)
	{
		case EPSC_cd4_0x:
			Tint = TINT_COREL;
		case EPSC_cd4_0k:
			// Get the grad fill parameters
			if (!Stack.Pop(&RotateY) || !Stack.Pop(&RotateX) ||
				!Stack.Pop(&Padding) || !Stack.Pop(&GradFillInfo.FillType) ||
				!Stack.Pop(&GradFillInfo.Angle) || !Stack.Discard(3))
				goto EPSError;

			// Convert angle to sane value
//			GradFillInfo.Angle = GradFillInfo.Angle;

			// Convert values from floating point to integer
			GradFillInfo.EdgePad = (INT32) (Padding * 100.0);
			GradFillInfo.RotateX = (INT32) (RotateX * 100.0);
			GradFillInfo.RotateY = (INT32) (RotateY * 100.0);

			if (Stack.GetType() != EPSTYPE_COMMAND)
			{
				if (IsUserName("Tim"))
					TRACE( _T("@x/@k: Expected ArrayEnd, found '%s'\n"), TokenBuf);
				goto EPSError;
			}
			
			EPSCommand Cmd;
			Stack.PopCmd(&Cmd);
			if (Cmd != EPSC_ArrayEnd)
			{
				if (IsUserName("Tim"))
					TRACE( _T("@x/@k: Expected ArrayEnd, found '%s'\n"), GetEPSCommand(Cmd));
				goto EPSError;
			}

			// Discard the steps parameter 
			if (!Stack.Discard())
				goto EPSError;

			// Get the end colour.
			if (!Stack.PopColour(&Col, Tint))
		    	// Invalid colour operands
				goto EPSError;
			EndColour.SetCMYKValue(&Col);

			// Discard some strange parameter I don't yet understand
			if (!Stack.Discard())
				goto EPSError;

			// Get the start colour.
			if (!Stack.PopColour(&Col, Tint))
		    	// Invalid colour operands
				goto EPSError;
			StartColour.SetCMYKValue(&Col);

			// Discard the array end symbol
			if (Stack.IsEmpty())
			{
				// Run out of operands!
				if (IsUserName("Tim"))
					TRACE( _T("@x/@k: Run out of operands\n"));
				goto EPSError;
			}
				
			if (Stack.GetType() != EPSTYPE_COMMAND)
			{
				if (IsUserName("Tim"))
					TRACE( _T("@x/@k: Expected ArrayStart but found '%s'\n"), TokenBuf);
				goto EPSError;
			}
			else
			{
				EPSCommand Cmd = Stack.ReadCmd();
				if (!Stack.PopCmd(&Cmd) || Cmd != EPSC_ArrayStart)
				{
					if (IsUserName("Tim"))
						TRACE( _T("@x/@k: Expected ArrayStart, found '%s'\n"), GetEPSCommand(Cmd));
					goto EPSError;
				}
			}

			GradFill = TRUE;

			break;
			
		case EPSC_eoclip:
			// add clip thingy operator...
			EPSFlags.PathIsHidden = TRUE;
			Token = EPSC_W;
			return EPSFilter::ProcessToken();
			break;

		case EPSC_cd5_0gs:
			return Import_gsave();
			break;

		case EPSC_cd5_0gr:
			return Import_grestore();
			break;

		case EPSC_cd5_0np:
			// newpath... blank current if necessary
			if(pInkPath !=0)
			{
				if(!pInkPath->ClearPath())
					return FALSE;
			}
			break;

		default:
			return CorelEPSFilter::ProcessToken();
			break;
	}


	// No errors encountered while parsing this token and its operands.
	return TRUE;
	
	
	// Error handlers:
	
EPSError:
	HandleEPSError();
	return FALSE;
#if 0
NoMemory:
	HandleNoMemory();
	return FALSE;
#endif
}


/********************************************************************************************

>	char *Corel4EPSFilter::GetEPSCommand(EPSCommand Cmd)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/02/94
	Inputs:		Cmd - the EPS token, e.g. EPSC_cd_c
	Returns:	Pointer to the string representation of the token, e.g. "@c"
	Purpose:	Given an EPS token, return the string representation of it; mainly for
				debugging purposes.

********************************************************************************************/

char *Corel4EPSFilter::GetEPSCommand(EPSCommand Cmd)
{
	INT32 i = 0;
	while (Corel4Commands[i].Cmd != EPSC_Invalid)
	{
		if (Corel4Commands[i].Cmd == Cmd)
			return Corel4Commands[i].CmdStr;

		// Try next command
		i++;
	}

	// Couldn't find it - default to base class method
	return CorelEPSFilter::GetEPSCommand(Cmd);
}


