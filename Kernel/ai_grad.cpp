// $Id: ai_grad.cpp 662 2006-03-14 21:31:49Z alex $
// ai_grad.cpp: implementation of the AIGradientProcessor class.
//
//////////////////////////////////////////////////////////////////////
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
#include "ai_grad.h"

#include "ai5_eps.h"
#include "fillramp.h"

DECLARE_SOURCE("$Revision");

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	AI5Gradient::AI5Gradient()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/08/94
	Purpose:	Set up a gradient record to sensible values (linear fill black to black).
	SeeAlso:	AI5Gradient; AI5EPSFilter

********************************************************************************************/
AI5Gradient::AI5Gradient() :
	IsRadial( FALSE ),
	StartColour( BLACK ),
	EndColour( BLACK ),
	mpCurrentRamp( 0 )
{
}


AI5Gradient::~AI5Gradient()
{
	delete mpCurrentRamp;
	mpCurrentRamp = 0;
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AIGradientProcessor::AIGradientProcessor() :
	mpCurrentFill(0),
	mbGradFillTooComplex(FALSE)
{
	// The last ramp point is set to 101, since this is effectively a
	// counter from 100 down to 0.
	mMidPoint		= 0;
	mLastRampPoint	= 101;
}

AIGradientProcessor::~AIGradientProcessor()
{
	mGradFills.DeleteAll();

	delete mpCurrentFill;
	mpCurrentFill = 0;
}


/********************************************************************************************

>	BOOL AIGradientProcessor::BeginGradient()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Allows the gradient processor to intialize itself for a new gradient
				signalled by the %BeginGradient comment.

	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..

********************************************************************************************/
BOOL AIGradientProcessor::BeginGradient()
{
	ENSURE( mpCurrentFill == NULL, "mpCurrentFill is not NULL");

	// Found the start of a gradient definition - prepare to read in the
	// definition...
	mpCurrentFill = new AI5Gradient;

	if ( !mpCurrentFill )
		return FALSE;

	return TRUE;
}



/********************************************************************************************

>	BOOL AIGradientProcessor::DecodeBd( AI5EPSFilter& filter )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Processes the gradient definition start operator which tells us how many
				colours are in it and what its name is.

	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..

********************************************************************************************/
BOOL AIGradientProcessor::DecodeBd( AI5EPSFilter& filter )
{
	if ( !mpCurrentFill )
		return FALSE;

	///////////////
	// Name, type, and number of steps in gradient fill
	///////////////
	INT32 colours = 0;
	INT32 type = 0;

	if (!filter.GetStack().Pop(&colours) || 
		!filter.GetStack().Pop(&type) || 
		!filter.GetStack().Pop(&mpCurrentFill->Name))
		// Error in syntax
		return FALSE;

	// Translate fill type
	switch (type)
	{
		case 0:		mpCurrentFill->IsRadial = FALSE;	break;
		case 1:		mpCurrentFill->IsRadial = TRUE;		break;
		default:	return FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL AIGradientProcessor::DecodeBS( AI5EPSFilter& filter )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Processes the gradient stop operator (indicating a step in the ramp)

	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..

********************************************************************************************/
BOOL AIGradientProcessor::DecodeBS( AI5EPSFilter& filter )
{
	// Decode colour/step definition
	INT32 rampPoint = 0, midPoint = 0;

	if ( !filter.GetStack().Pop(&rampPoint) || !filter.GetStack().Pop(&midPoint) )
		return FALSE;

	// Check if these are values we can handle
	// (We still import an approximation of the fill even if not)
	if ( midPoint > 100 || midPoint < 0 || rampPoint < 0 || rampPoint > 100 )
	{
		return FALSE;
	}

	// Work out what sort of colour this is - CMYK or B&W tint
	INT32 colorStyle;
	if ( !filter.GetStack().Pop(&colorStyle) )
		return FALSE;

	DocColour newColour;

	switch ( colorStyle )
	{
		case 0:		// grey
		{
			double dTint( 0.0 );
			if ( !filter.GetStack().Pop(&dTint) )
				return FALSE;

			// Make a colour from this
			INT32 intensity = (INT32) (dTint * 255.0);
			newColour.SetRGBValue( intensity, intensity, intensity );
			break;
		}

		case 1:		// CMYK
		{
			PColourCMYK	col;
			if (!filter.GetStack().PopColour(&col, TINT_NONE))
				return FALSE;

			// Set up the colour with what we have.
			newColour.SetCMYKValue( &col );

			break;
		}

		case 2:		// RGB
		{
			INT32 red, green, blue;
			if (!filter.GetStack().PopColourRGB (&red, &green, &blue, TINT_NONE))
				return FALSE;

				// remove the CMYK colour info.
			(filter.GetStack()).Discard (4);
			newColour.SetRGBValue (red, green, blue);
			break;
		}

		case 3:		// CMYK custom
		{
			PColourCMYK col;
			FIXEDPOINT tint;
			String_64 name;
			if ( !filter.GetStack().PopColour( &col, TINT_ILLUSTRATOR, &tint, &name ) )
				return FALSE;

			newColour.SetCMYKValue( &col );

			break;
		}

		case 4:		// RGB custom
		{
			INT32 red, green, blue;
			FIXEDPOINT tint;
			String_64 name;

			// remove the 'type' flag (is always 1 for RGB)
			filter.GetStack().Discard (1);

			if (!filter.GetStack().PopColourRGB (&red, &green, &blue, 
												TINT_ILLUSTRATOR, &tint, &name))
				return FALSE;

			(filter.GetStack()).Discard (4);
			newColour.SetRGBValue (red, green, blue);

			break;
		}
		default:
			break;
	}


	////////////////
	//	add the colour to the ramp
	////////////////

	double dPos = double(rampPoint) / double(100.0);

	if ( mpCurrentFill )
	{
		if ( !mpCurrentFill->mpCurrentRamp )
		{
			mpCurrentFill->mpCurrentRamp = new ColourRamp;
			if ( ! mpCurrentFill->mpCurrentRamp )
			{
				return FALSE;
			}
		}

		mpCurrentFill->mpCurrentRamp->AddEntry( static_cast<float> ( dPos ), &newColour );
	}

	// Set the midpoint if this point is nearer to the start than the stored mid-point. 
	//	The midpoint nearest to the 0% ramp point is used, as there should only be one 
	//	midpoint if Xara X's profiling is going to cope with it, and the last midpoint in 
	//	an Illustrator gradient should always be ignored.
	if (rampPoint < mLastRampPoint)
	{
		mMidPoint = midPoint;
		mLastRampPoint = rampPoint;
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL AIGradientProcessor::DecodeBh( AI5EPSFilter& filter )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	The Bh operator adds a highlight to a fill. In Camelot we interpret this
				as a conical fill.

	Returns:	TRUE if the definition was processed happily
				FALSE if not


	The format of the operator is:
		xHilight yHilight angle length Bh

	Arguments to the Bh operator are as follows:
		xHilight yHilight	These arguments specify the hilight placement, in x and y offsets 
							from the gradient vector origin.
		angle				This argument is the angle to the hilight point, measured 
							counterclockwise from the x axis.
		length				This argument is the distance of the hilight from the origin, 
							expressed as a fraction of the radius a value between 0 and 1.


********************************************************************************************/
BOOL AIGradientProcessor::DecodeBh( AI5EPSFilter& filter )
{
	//////////////////
	// read the arguments from the stack
	//////////////////

	INT32 nAngle		= 0;
	double dLength	= 0.0;
	DocCoord hilight;

	if ( !filter.GetStack().Pop(&dLength) || !filter.GetStack().Pop(&nAngle) ||
		!filter.GetStack().PopCoordPair(&hilight) )
		return FALSE;

	////////////////
	// Check if these are values we can handle
	////////////////

	////////////////
	// Attach the highlight details so we can work out what to do later
	// but if there's already a highlight ignore this one
	////////////////
/*

  Won't do this for now


	mpCurrentHighlight = new AI5Gradient::Highlight( nAngle, dLength, hilight );
*/
	return TRUE;
}

/********************************************************************************************

>	BOOL AIGradientProcessor::Approx (const double & d1, const double & d2)

	Author:		?
	Created:	presumably March 2000
	Inputs:		d1, d2 - two doubles that are to be compared
	Purpose:	This is used to determine whether the position of one gradient stop is 
				equivalent to another during import, mainly for working out which stops
				are at the start and the end.

				Currently the tolerance for this is 2% either way, as this seems to give
				good results.

	Returns:	TRUE if the d1 is approximately equal to d2
				FALSE if not

********************************************************************************************/
BOOL AIGradientProcessor::Approx (const double & d1, const double & d2)
{
	const double dTolerance = 2;
	return ( fabs( d2 - d1 ) < dTolerance );
}


/********************************************************************************************

>	BOOL AIGradientProcessor::EndGradient()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Allows the gradient processor to complete the gradient
				signalled by the %EndGradient comment.

	Returns:	TRUE if the definition was processed, 
				FALSE if not used by this filter..

********************************************************************************************/
BOOL AIGradientProcessor::EndGradient( AI5EPSFilter& filter )
{
	ENSURE( mpCurrentFill != NULL, "mpGradFill is NULL");

	// The end of the definition - lose the rampant '[' from the input stream.
	EPSCommand Cmd;
	if (!filter.GetStack().PopCmd(&Cmd) || (Cmd != EPSC_ArrayStart))
	{
		// if it isn't what we're hoping for, ignore it
		filter.GetStack().Push( Cmd );
	}

	////////////////////
	// an n-stage multi-stage fill in camelot is divided into n - 2 ramp items
	// and the start and end colours of the fill.
	// When creating the fill we have just added ramp items, so we need to pick 
	// off the first and last for the start and end colours.
	////////////////////

	if ( mpCurrentFill->mpCurrentRamp )
	{	
		// (ChrisG 3/4/2001) Gradient stops are now always removed from the list if they are
		//	used as a start or end colour. There was a check to make sure they were near the
		//	end, which often failed, leading to additional stops at the ends.

		// first the start colour
		ColRampItem* pFirstItem = reinterpret_cast<ColRampItem*>( mpCurrentFill->mpCurrentRamp->GetHead() );
		if ( pFirstItem )
		{
			// Take the first colour out of the ramp, as this is represented by the start colour
			mpCurrentFill->StartColour = pFirstItem->GetColour();

			// if the first ramp item isn't at the start of the fill, leave it as
			// part of the ramp and make the start the same colour
			if ( Approx( pFirstItem->GetPosition(), 0.0) ||
				 Approx( pFirstItem->GetPosition(), 100.0))
			{
				mpCurrentFill->mpCurrentRamp->RemoveHead();
			}
		}

		// then the end colour
		ColRampItem* pLastItem = reinterpret_cast<ColRampItem*>( mpCurrentFill->mpCurrentRamp->GetTail() );
		if ( pLastItem )
		{
			// Take the last colour out of the ramp, as this is represented by the end colour
			mpCurrentFill->EndColour = pLastItem->GetColour();

			// if the first ramp item isn't at the start of the fill, leave it as
			// part of the ramp and make the start the same colour
			if ( Approx( pLastItem->GetPosition(), 0.0) ||
				 Approx( pLastItem->GetPosition(), 100.0))
			{
				mpCurrentFill->mpCurrentRamp->RemoveTail();
			}
		}
	}

	// set the mid-point, for the profiling.
	mpCurrentFill->midPoint = mMidPoint;

	// add the current fill to our list.
	mGradFills.AddTail( mpCurrentFill );

	// We're done with this fill.
	mpCurrentFill = NULL;

	// reset the profiling variables.
	mMidPoint		= 0;
	mLastRampPoint	= 101;

	return TRUE;
}


/********************************************************************************************

>	AI5Gradient* AIGradientProcessor::FindGradient( const StringBase& name )

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/00

	Purpose:	Once the gradient definitions have been read, gradient instances can be
				created using the name to identify the gradient "style".

	Returns:	NULL if no gradient exists with the given name
				Otherwise the one that does.

********************************************************************************************/
AI5Gradient* AIGradientProcessor::FindGradient( const StringBase& name )
{
	// First find this named gradient fill
	AI5Gradient* pGradient = (AI5Gradient *) mGradFills.GetHead();

	while ( pGradient )
	{
		if ( pGradient->Name == name )
			// This is the one we want!
			break;

		// Try the next one
		pGradient = (AI5Gradient *) mGradFills.GetNext(pGradient);
	}

	return pGradient;
}
