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
// doccolor.cpp - the DocColour class

/*
*/


#include "camtypes.h"

//#include "colcontx.h"
#include "colormgr.h"
#include "colourix.h"
#include "colplate.h"
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]


CC_IMPLEMENT_MEMDUMP(DocColour, CC_CLASS_MEMDUMP)


#ifdef _DEBUG
// --- Comment out the line below to disable IndexedColour usage tracing
//#define TRACECOLOURIX 1
// ---
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/*	DEFAULTCONTEXT(ColourModel)
 *  Macro to find a pointer to the global default colour context for a given
 *  Colour Model.
 *  	eg:	DEFAULTCONTEXT(ColModel)->PackColour(...);
 */
#define DEFAULTCONTEXT(colmodel) (ColourContext::GetGlobalDefault((ColourModel)colmodel))

/********************************************************************************************

>	void DocColour::InitialiseInfoField(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Private shared code for construction of DocColour objects
				Initialises the 'Info' structure to default values
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void DocColour::InitialiseInfoField(ColourModel ColModel)
{
	ENSURE((INT32) ColModel < MAX_COLOURMODELS,
			"Attempt to use illegal colour model!");

	Info.Reserved			= 0;		// Reserved - set to zero for safety

	Info.OCContextHandle	= 0;
	Info.SourceColourModel	= ColModel;
	Info.CacheColourModel	= COLOURMODEL_NOTCACHED;

	Info.IsSeparable		= TRUE;

	Info.ForceRounding		= FALSE;

	Info.NoColour			= FALSE;
}

/********************************************************************************************

>	DocColour::DocColour()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a DocColour
				Initialises the colour to an RGBT value representing 'no colour'.
				(i.e. The same as DocColour(COLOUR_TRANS))
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocColour::DocColour()
{
	ZapSourceColour();
	InitialiseInfoField(COLOURMODEL_RGBT);

	SourceColour.RGBT.Red			= 0;	// Black & 100% Transparent
	SourceColour.RGBT.Green			= 0;
	SourceColour.RGBT.Blue			= 0;
	SourceColour.RGBT.Transparent	= 255;

	Info.NoColour = TRUE;					// And also 'no colour' transparent
}



/********************************************************************************************

>	DocColour::~DocColour()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor for a DocColour
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocColour::~DocColour()
{
	// If we are a reference to an indexed colour, tell that colour that we are
	// no longer referencing it
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
		SourceColour.Indexed.Colour->DecrementUsage();
}







/********************************************************************************************

>	DocColour::DocColour(StockColour Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/04/94
	Inputs:		Col - Identifies the stock colour to create
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a DocColour
				Initialises it to the given predefined (stock) colours, as defined
				in stockcol.h (cf keyword StockColour)

	Notes:		ALL StockColours are non-separable colours!
				i.e. they should only be applied to user-interface rendering

	Errors:		-
	SeeAlso:	StockColour

********************************************************************************************/

DocColour::DocColour(StockColour Col)
{
	ZapSourceColour();
	PColourValue Red = 0, Green = 0, Blue = 0, Transparent = 0;

	InitialiseInfoField(COLOURMODEL_RGBT);

	switch (Col)
	{
		case COLOUR_TRANS:
			Red = Green = Blue = Transparent = 255;
			Info.NoColour = TRUE;
			break;

		case COLOUR_BLACK:
		case COLOUR_UNSELECTEDBLOB:
		case COLOUR_FILLDEFAULT:
		case COLOUR_LINEDEFAULT:
		case COLOUR_XORDRAG:
			break;

		case COLOUR_DKGREY:
			Red = Green	= Blue =  64;
			break;

		case COLOUR_MIDGREY:
			Red = Green = Blue = 128;
			break;

		case COLOUR_XOREDIT:
			Red = 64;
			Green = 64;
			Blue = 230;
			break;

		case COLOUR_LTGREY:
		case COLOUR_BEZIERLINE:
			Red = Green = Blue = 192;
			break;

		case COLOUR_WHITE:
			Red = Green = Blue = 255;
			break;

		case COLOUR_RED:
		case COLOUR_XORSELECT:
		case COLOUR_TOOLBLOB:
		case COLOUR_SELECTEDBLOB:
		case COLOUR_BEZIERBLOB:
			Red		= 255;
			break;

		case COLOUR_GREEN:
			Green	= 255;
			break;

		case COLOUR_BLUE:
		case COLOUR_XORNEW:
		case COLOUR_GRID:
			Blue	= 255;
			break;

		case COLOUR_CYAN:
			Green  = Blue = 255;
			break;

		case COLOUR_MAGENTA:
			Red = Blue = 255;
			break;

		case COLOUR_YELLOW:
			Red = Green = 255;
			break;
	}


	SourceColour.RGBT.Red			= Red;
	SourceColour.RGBT.Green			= Green;
	SourceColour.RGBT.Blue			= Blue;
	SourceColour.RGBT.Transparent	= Transparent;

	Info.IsSeparable = FALSE;
}




/********************************************************************************************

>	DocColour::DocColour(ColourValue Red, ColourValue Green, ColourValue Blue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		Red; Green; Blue - The RGB definition of the colour
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a DocColour
				Initialises it to the given RGBT value.
				Values are ColourValues (FIXED24s) in the range 0.0 to 1.0

	Notes:		DocColours no longer support transparency/transtype.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocColour::DocColour(ColourValue Red, ColourValue Green, ColourValue Blue)
{
	ZapSourceColour();
	ColourRGBT temp;

	InitialiseInfoField( COLOURMODEL_RGBT );

	temp.Red			= Red;			// Copy data into 128-bit struct, then pack
	temp.Green			= Green;		// it into our 32-bit store.
	temp.Blue			= Blue;
	temp.Transparent	= 0;

	DEFAULTCONTEXT(COLOURMODEL_RGBT)->PackColour( (ColourGeneric *)&temp, &SourceColour );
}



/********************************************************************************************

>	DocColour::DocColour(ColourModel ColModel, ColourGeneric *Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		ColModel - Colour model in which Col is defined
				Col - definition of the colour in model ColModel
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a DocColour
				Initialises it to the given value

	Notes:		Note the existence of the macros:
					DOCCOLOUR_RGBT(Colour_RGBT *col)
					DOCCOLOUR_CMYK(Colour_CMYK *col)
					DOCCOLOUR_HSVT(Colour_HSVT *col)
					DOCCOLOUR_CIET(COLOUR_CIET *col)
				which can be used to create DocColours from ColourRGBT etc structs
				e.g.
					ColourRGBT AnRGBTColourDefn;
					DocColour  MyRGBT = DOCCOLOUR_RGBT(&AnRGBTColourDefn);
				These macros are defined in doccolor.h

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocColour::DocColour(ColourModel ColModel, ColourGeneric *Col)
{
	ZapSourceColour();
	InitialiseInfoField(ColModel);
	DEFAULTCONTEXT(ColModel)->PackColour(Col, &SourceColour);
}



/********************************************************************************************

>	DocColour::DocColour(const DocColour &other);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Other - colour to copy
	Outputs:	-
	Returns:	
	Purpose:	DocColour copy constructor
	Notes:		If copying a reference to an indexed colour, the indexed colours
				usage count will be incremented.
				If this is already a reference to an indexed colour, that indexed
				colours usage count will be decremented.
	Errors:		-

********************************************************************************************/

DocColour::DocColour(const DocColour &Other)
{
	ENSURE(Other.Info.SourceColourModel < MAX_COLOURMODELS,
			"Attempt to use illegal colour model!");

	Info = Other.Info;
	CachedColour = Other.CachedColour;
	SourceColour = Other.SourceColour;

	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");

		SourceColour.Indexed.Colour->IncrementUsage();
		ENSURE(Info.CacheColourModel == 0, "RefToIndexedColour DocColour has a cache!!");

#ifdef TRACECOLOURIX
TRACE( _T(">> Construct DocColour(DocColour => ColourIx %x) @ %x\n"), (INT32)SourceColour.Indexed.Colour, (INT32)this);
#endif
	}
}



/********************************************************************************************

>	DocColour::DocColour(IndexedColour *Col)
	
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	{Copy} Constructor for a DocColour
	Notes:		THIS FUNCTION DOES NOT EXIST!!
				DocColour::MakeRefToIndexedColour() should be used if you wish
				to "assign" an IndexedColour to a DocColour.
	SeeAlso:	DocColour::MakeRefToIndexedColour

********************************************************************************************/


/********************************************************************************************

>	ColourModel DocColour::GetColourModel(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	A Colour Model identifying the model in which the colour is defined
	Purpose:	To determine the colourmodel in which the given colour was defined
	Notes:		If this is a reference to an IndexedColour, returns the colour of the
				referenced IndexedColour.
				The colour model number returned may be used as an index into the list
				of ColourContexts held by each document, in order to find a relevant 
				context for the colour.
	Errors:		-
	SeeAlso:	DocColour::GetSourceColour

********************************************************************************************/

ColourModel DocColour::GetColourModel(void) const
{
	ENSURE(Info.SourceColourModel < MAX_COLOURMODELS,
			"DocColour based on illegal colour model!");

	// If colour is a reference to an indexed colour, return indexed colour's
	// colour model
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");
		return(SourceColour.Indexed.Colour->GetColourModel());
	}

	return((ColourModel) Info.SourceColourModel);
}




/********************************************************************************************

>	void DocColour::GetSourceColour(ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		Result - pointer to structure to recieve the colour
	Outputs:	-
	Returns:	The native definition of the colour. This is a ColourPacked containing
				the colour as defined in its original Colour Model (cf DocColour::GetColourModel)
	Purpose:	Determine the native definition of the given colour
	Notes:		If this is a reference to an IndexedColour, returns the colour of the
				referenced IndexedColour.
	Errors:		-
	SeeAlso:	DocColour::GetColourModel

********************************************************************************************/

void DocColour::GetSourceColour(ColourGeneric *Result)
{
	ENSURE(Info.SourceColourModel < MAX_COLOURMODELS,
			"DocColour based on illegal colour model!");

	// If colour is reference to indexed colour, ask the indexed colour for the
	// original colour definition, else return our own definition.
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");
		SourceColour.Indexed.Colour->GetSourceColour(Result);
	}
	else
		DEFAULTCONTEXT(Info.SourceColourModel)->
										UnpackColour(&SourceColour, Result);

	if (Info.ForceRounding)
	{
		// Our ForceRounding flag is on, so we must round all components to the closest
		// half percentage value. This is a bodge to correct for representational errors
		// in PANTONE library colours.
		double temp;

		temp = Result->Component1.MakeDouble();
		temp = (floor((temp * 200.0) + 0.5)) / 200.0;	// Round to nearest 200th
		Result->Component1 = temp;

		temp = Result->Component2.MakeDouble();
		temp = (floor((temp * 200.0) + 0.5)) / 200.0;	// Round to nearest 200th
		Result->Component2 = temp;

		temp = Result->Component3.MakeDouble();
		temp = (floor((temp * 200.0) + 0.5)) / 200.0;	// Round to nearest 200th
		Result->Component3 = temp;

		temp = Result->Component4.MakeDouble();
		temp = (floor((temp * 200.0) + 0.5)) / 200.0;	// Round to nearest 200th
		Result->Component4 = temp;
	}
}



/********************************************************************************************

>	void DocColour::SetSeparable(BOOL IsSeparable = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		IsSeparable -	TRUE if this colour will be allowed to be separated and
								colour-corrected during conversions
								FALSE if this colour shouldn't be separated

	Purpose:	To set the separation/correction enable flag for a DocColour.
				Some colours (e.g. EOR blob colours) should not be colour-separated
				on screen. By default, all colours (except StockCols) will separate,
				but this flag can be set on "UI colours" to stop them separating.

	SeeAlso:	DocColour::IsSeparable

********************************************************************************************/

void DocColour::SetSeparable(BOOL IsSeparable)
{
	Info.IsSeparable = IsSeparable;
}


/********************************************************************************************

>	void DocColour::SetReservedFlag(UINT32 Value)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		Value - the value to set the flag to

	Purpose:	To set the Reserved flag of the colour info struct. 

	SeeAlso:	My use of this flag can be seen in LineDefinition::ConvertIndexedColours and
				PathProcessorBrush::RenderAttributes.  If you want to use this flag for your
				own purposes I suggest that you take a look at what I've done to make sure
				there will be no conflict.

********************************************************************************************/

void DocColour::SetReservedFlag(UINT32 Value)
{
	Info.Reserved = Value;
}


/********************************************************************************************

>	UINT32 DocColour::GetReservedFlag()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	the Reserved Flag of the ColourInfo struct
	Purpose:	access fn.

	SeeAlso:	

********************************************************************************************/

UINT32 DocColour::GetReservedFlag()
{
	return Info.Reserved;
}


/********************************************************************************************

>	BOOL DocColour::IsNamed()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/2000
	Inputs:		-
	Returns:	TRUE, if this colour is named, or if it has the flag set that indicates that it
				was created by a named colour
	Purpose:	to identidy if this colour is named, or was created from a named colour.  Note that
				not all colours that were created by Named colours will have the relevant flag set, as
				I have only just started using it (4/2000).  This fn. was really designed to be used
				specifically by the brush attribute replace named colours system.

	SeeAlso:	

********************************************************************************************/

BOOL DocColour::IsNamed()
{
	BOOL RetVal = FALSE;
	
	// first try the parent colour
	IndexedColour* pParent = FindParentIndexedColour();
	if (pParent != NULL)
	{
		if (pParent->IsNamed())
			RetVal = TRUE;
	}
	
	// now try the flags
	if (Info.Reserved == COL_NAMED)
		RetVal = TRUE;

	return RetVal;
}


/********************************************************************************************

>	void DocColour::HackColReplacerPreDestruct()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/6/2000
	Inputs:		-
	Returns:	-
	Purpose:	As you might expect from the name this is a real bodge. Basically the named
				colour replacer used in the brush needs to store some colours which are 
				converted index colours. 
				To cut a long story short I'm looking at a deadline and this prevents an
				assert upon destruction, theres no reason for anyone else to use it.

	SeeAlso:	

********************************************************************************************/

void DocColour::HackColReplacerPreDestruct()
{
	Info.SourceColourModel = MAX_COLOURMODELS;
}

/********************************************************************************************

>	void DocColour::Mix(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
						ColourContext *BlendContext, BOOL BlendTheLongWay = FALSE,
						ColourContext *OutputContext = NULL)
	
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94 (moved from constructor, 29/5/96)

	Inputs:		BlendStart - The start DocColour

				BlendEnd - The end DocColour

				BlendFraction - The amount by which the two colours should be blended,
				in the range 0.0 to 1.0. Blending is linear in the given colourspace,
				achieved by the equation

				Result = (BlendStart * (1.0-BlendFraction)) + (BlendEnd * BlendFraction)

				BlendContext - the colour context in which the blend will take place
								(may be NULL, but this is NOT recommended)
								(Note that this is ignored if either colour is a Spot)
								(Note that if both end colours are CMYK, and Mix effect
								is used, a CMYK context is used instead, so that CMYK
								grads colour separate properly)
	
				BlendTheLongWay - (ONLY used if the BlendContext points to an HSV context)
				This indicates whether to do 'rainbow' (shortest distance- FALSE) or
				'alt-rainbow' (longest distance- TRUE) HSV colour blending.

				OutputContext - Normally, this should be NULL. However, if you're doing
				a colour separation involving spot colours, you should pass the output
				colour context in here, so that spots can be correctly separated. The
				output for mixing a spot colour will then be a greyscale ink intensity,
				correctly colour separated, and with its IsSeparable flag FALSE so that
				it is not colour separated or corrected a second time when output. It is
				safe to always pass the output context in here, as this function will
				always work out the appropriate behviour.

	Purpose:	"Constructor" for a DocColour - generates a blended DocColour
				The start and end colours are blended to create this new DocColour. The
				amount by which they are mixed is determined by BlendFraction. The blend
				occurs in the colourspace described by BlendContext. Note that BlendContext
				may be NULL, in which case the default context for the BlendStart colour
				will be used. However, this is not recommended, as this will result in
				a blend which may be incorrect for the context(s) in use in a given
				document, which could give seriously wonky colours!

				If blending is requested in HSV space, the extra parameter BlendTheLongWay
				is required, to determine if blending is done in a "Rainbow" or "Alt-Rainbow"
				fashion.

	Notes:		Note that although this may be slow, you must mix colours using this
				method, or colour correction/separation may not occur.

				The blend method specified by BlendContext & BlendTheLongWay is ignored
				if either of the colours is a spot colour - these are always coerced
				into simple "Mix" fills/blends, as otherwise the printed output and
				colour representation on screen will be totally different!

				If both end colours reference the same IndexedColour, and the mixing
				method is Mix (RGB) or Rainbow (HSV, BlendTheLongWay==FALSE), then all
				intermediate colours are identical, and are thus returned referencing the
				original indexed colour.

				If both the end colours are tints of the same spot colour, then two things
				can occur:
				  * If OutputContext != NULL, we presume the mix is temporary for rendering
					to that context, so produce an RGB mix or separated Ink density value
					for the context.
				  * If OutputContext == NULL, we presume you're doing a make shapes on
					a blend or similar, so we create a new local colour for each mix,
					which is a permanent proper tint of the parent spot ink.

********************************************************************************************/

void DocColour::Mix(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext, BOOL BlendTheLongWay,
							ColourContext *OutputContext)
{
//	if (BlendFraction >= 1.0)
//		BlendFraction = 0.999999999;
	
//	ERROR3IF(BlendFraction < 0.0 || BlendFraction > 1.0,
//				"DocColour::DocColour Blend: Illegal Blend fraction!");

	ERROR3IF(BlendStart == NULL || BlendEnd == NULL,
				"DocColour::DocColour Blend: NULL input parameters!");

	// Make sure that this colour is not a reference to an IndexedColour, otherwise
	// if/when we InitialiseInfoField() below, we'll forget the reference, and trigger
	// lots of bogus "IndexedColour deleted while in use" warnings
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		SourceColour.Indexed.Colour->DecrementUsage();
		Info.SourceColourModel	= COLOURMODEL_RGBT;
	}

	// If either the start of end colour is a spot/tint colour, then we will coerce
	// this mix into a simple RGB mix, not a Hue mix, by grabbing the default RGB context
	// We also special-case a Mix/Rainbow from an IndexedColour to itself, by simply making 
	// the mixed step another reference to that colour (but not for alt-rainbow!)
	BOOL IncludesSpots = FALSE;			// Remember if we've got to worry about spots

	IndexedColour *StartParent	= BlendStart->FindParentIndexedColour();
	IndexedColour *EndParent	= BlendEnd->FindParentIndexedColour();


	// --- Are blend start/end colours identical? - we might optimise them out if they are
	if (StartParent != NULL && StartParent == EndParent)
	{
		// If it's an RGB Mix or an HSV Rainbow (short way) then all mixes are identical to
		// the original colour, so just return that instead.
		if ( BlendContext != NULL &&
			 ( BlendContext->GetColourModel () == COLOURMODEL_RGBT || 
			 ( BlendContext->GetColourModel () == COLOURMODEL_HSVT && !BlendTheLongWay ) ) )
		{
			// Make ourselves a reference to the parent colour
			MakeRefToIndexedColour ( StartParent );
			return;
		}
	}


	// --- Determine if one or both of the colours are spot/tint colours
	// (When separating, we must treat blends including spot colours very specially)
	if (StartParent != NULL && StartParent->IsSpotOrTintOfSpot())
		IncludesSpots = TRUE;
	else if (EndParent != NULL && EndParent->IsSpotOrTintOfSpot())
		IncludesSpots = TRUE;

	if (IncludesSpots)
	{
		// The blend includes spot colours, so must be coerced into an RGB "Mix"
		BlendContext = ColourManager::GetColourContext(COLOURMODEL_RGBT);

		IndexedColour *StartSpot = BlendStart->GetSpotParent();
		IndexedColour *EndSpot	 = BlendEnd->GetSpotParent();

		if (OutputContext == NULL)
		{
			// Doing Make Shapes, so should make permanent IndexedColour tints, if
			// both the end colours are tints of the same spot colour
			if (StartSpot != NULL && StartSpot == EndSpot)
			{
				MixTint(StartSpot, StartParent, EndParent, BlendFraction);
				return;
			}
			// else drop through to normal RGB mixing code (which means the colours
			// become process colours. Damn! -- but there's nothing we can do about it)
		}
		else
		{
			ColourPlate *ColPlate = OutputContext->GetColourPlate();

			if (ColPlate != NULL && !ColPlate->IsDisabled())
			{
				// Make us into a CMYK with the ink value in our Key component
				// The Key is calculated below (in the if statement) but we'll do the common stuff here
				InitialiseInfoField(COLOURMODEL_CMYK);
				SourceColour.CMYK.Cyan = SourceColour.CMYK.Magenta = 
											SourceColour.CMYK.Yellow = 0;

				if (ColPlate->GetType() == COLOURPLATE_SPOT)
				{
					// We're rendering to a spot plate, so we want to output an ink density value.
					// Thus, we create a CMYK value, and place the ink density in the Key component.
					// We will then set it non-separable so that it is not separated a second time
					// (see after this if statement)
					// (NOTE: IF both ends are tints of this spot plate, we'll interpolate the tint. If
					// only one end is a spot colour, then the other end will become a 0% tint, i.e. white)

					// Duh!  We need to check which spot plate we are doing or spot colours end up
					// being printed on all spot plates!

					IndexedColour* pPlateSpot = ColPlate->GetSpotColour();

					// Work out the start and end tint values for use with the common ancestor (if any).
					double StartValue = 0.0;
					if (StartSpot != NULL && StartSpot == pPlateSpot)
						StartValue = StartParent->GetAccumulatedTintValue().MakeDouble();

					double EndValue	 = 0.0;
					if (EndSpot != NULL && EndSpot == pPlateSpot)
						EndValue = EndParent->GetAccumulatedTintValue().MakeDouble();

					// Linearly interpolate the tint values
					double NewTint = (StartValue * (1.0 - BlendFraction)) + (EndValue * (BlendFraction));

					SourceColour.CMYK.Key = (PColourValue) (NewTint * 255.0);
				}
				else
				{
					// Generate separated RGB versions of the end colours
					ColourRGBT SeparatedStart;
					if (BlendStart->IsTransparent())
						OutputContext->GetWhite((ColourGeneric *) &SeparatedStart);
					else
						OutputContext->ConvertColour(BlendStart, (ColourGeneric *) &SeparatedStart);

					ColourRGBT SeparatedEnd;
					if (BlendEnd->IsTransparent())
						OutputContext->GetWhite((ColourGeneric *) &SeparatedEnd);
					else
						OutputContext->ConvertColour(BlendEnd, (ColourGeneric *) &SeparatedEnd);


					if (ColPlate->IsMonochrome())
					{
						// We're doing a monochrome sep
						// The separated colour should be a greyscale, but let's just check
						ERROR3IF(SeparatedStart.Red != SeparatedStart.Green ||
								 SeparatedStart.Red != SeparatedStart.Blue,
								 "Separated colour is not a greyscale! Doh!");

						ERROR3IF(SeparatedEnd.Red != SeparatedEnd.Green ||
								 SeparatedEnd.Red != SeparatedEnd.Blue,
								 "Separated colour is not a greyscale! Doh!");

						// Assuming that R==G==B, use the R levels as ink intensities, and generate
						// this into the Key component
						double temp;
						BlendFraction *= 255.0;
						temp = SeparatedStart.Red.MakeDouble() * (255.0 - BlendFraction);
						temp += SeparatedEnd.Red.MakeDouble() * BlendFraction;
						SourceColour.CMYK.Key = 255 - ((PColourValue) (temp + 0.5));
					}
					else
					{
						// We're doing a colour (e.g. screen preview in shades of Cyan or whatever)
						// separation, so just mix the colour in RGB for previewing)

						// Override the CMYK setting we just did above!
						InitialiseInfoField(COLOURMODEL_RGBT);

						// Now, generate an RGB mixed value
						double temp;
						BlendFraction *= 255.0;
						temp = SeparatedStart.Red.MakeDouble() * (255.0 - BlendFraction);
						temp += SeparatedEnd.Red.MakeDouble() * BlendFraction;
						SourceColour.RGBT.Red = (PColourValue) (temp + 0.5);

						temp = SeparatedStart.Green.MakeDouble() * (255.0 - BlendFraction);
						temp += SeparatedEnd.Green.MakeDouble() * BlendFraction;
						SourceColour.RGBT.Green = (PColourValue) (temp + 0.5);

						temp = SeparatedStart.Blue.MakeDouble() * (255.0 - BlendFraction);
						temp += SeparatedEnd.Blue.MakeDouble() * BlendFraction;
						SourceColour.RGBT.Blue = (PColourValue) (temp + 0.5);
					}
				}

				// Finally, set ourselves non-separable so that we are not colour corrected
				// or separated a second time during output!
				SetSeparable(FALSE);

				// Let's get out of here before we're spotted! (sorry, last bad joke, I promise)
				return;
			}
			// else drop through to do a normal RGB mix
		}
		// else drop through to do a normal RGB mix
	}

	// --- Check for CMYK -> CMYK fill
	// If we're doing an RGB mix and both colours are CMYK, then we will use CMYK mixing
	// instead of RGB, so that CMYK grads separate properly
	if (BlendContext != NULL && BlendContext->GetColourModel() == COLOURMODEL_RGBT &&
			BlendStart->GetColourModel() == COLOURMODEL_CMYK &&
			BlendEnd->GetColourModel() == COLOURMODEL_CMYK)
	{
		BlendContext = ColourManager::GetColourContext(COLOURMODEL_CMYK);
	}


	// OK, so we're not picking at our spots (I lied about that being the last bad joke).
	// In that case, do a normal mix
	if (BlendContext == NULL)
	{
		BlendContext = ColourManager::GetColourContext(BlendStart->GetColourModel());
		ERROR3IF(BlendContext == NULL, "Can't find a default ColourContext for blend colour space");
	}

	switch(BlendContext->GetColourModel())
	{
		case COLOURMODEL_RGBT:
			MixRGB(BlendStart, BlendEnd, BlendFraction, BlendContext);
			break;

		case COLOURMODEL_CMYK:
			MixCMYK(BlendStart, BlendEnd, BlendFraction, BlendContext);
			break;

		case COLOURMODEL_HSVT:
			MixHSV(BlendStart, BlendEnd, BlendFraction, BlendContext, BlendTheLongWay);
			break;

		default:
			ERROR3("DocColour::Mix unimplemented for BlendContext other then RGB or HSV");
			break;
	}

	// Finally, if both the start and end colour were non-separable, all intermediate
	// blended colours will also come out non-separable. This only usually has an effect with
	// the "print on all plates" attribute, which overrides the rendering colour with
	// special pre-separated greyscales
	if (!BlendStart->IsSeparable() && !BlendEnd->IsSeparable())
		SetSeparable(FALSE);
}



/********************************************************************************************

>	void DocColour::MixRGB(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext, ColourContext *OutputContext)
	
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96

	Inputs:		BlendStart - The start DocColour
				BlendEnd - The end DocColour
				BlendFraction - Blending amount between 0.0 and 1.0
				BlendContext - the RGB colour context in which the blend will take place

	Purpose:	Internal helper function for DocColour::Mix.
				This is a reasonably optimised routine for mixing RGB colours.

	SeeAlso:	DocColour::Mix

********************************************************************************************/

void DocColour::MixRGB(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext)
{
	ERROR3IF(BlendStart == NULL || BlendEnd == NULL || BlendContext == NULL, "Illegal NULL params");
	ERROR3IF(BlendContext->GetColourModel() != COLOURMODEL_RGBT,
			 "DocColour::MixRGB is optimised for RGB colours only!");

	InitialiseInfoField(COLOURMODEL_RGBT);

	ColourGeneric StartDef;
	if (BlendStart->IsTransparent())					// If no-colour, then...
	{
		BlendContext->GetWhite(&StartDef);				// ... use white

		if (BlendEnd->IsTransparent())					// If both ends no-colour, return no-colour
			Info.NoColour = TRUE;
	}
	else
		BlendContext->ConvertColour(BlendStart, &StartDef);		// else use start colour

	// If 0.0 then StartDef now has the colour to use (no blending necessary)
	// If BOTH colours are transparent, we will be NoColour, so no need to blend
	if (BlendFraction > 0.0 && !Info.NoColour)
	{
		ColourGeneric EndDef;
		if (BlendEnd->IsTransparent())							// If no-colour then use white
			BlendContext->GetWhite(&EndDef);
		else
			BlendContext->ConvertColour(BlendEnd, &EndDef);

		// Precalculate the blending fractions. Optimisation - Multiply by 255 here to save
		// doing it for each component when we convert them into bytes below.
		BlendFraction *= 255.0;
		const double InverseFraction = (255.0 - BlendFraction);

		double temp;

		// Mix the RGB components into a 0..255 byte, and store into our packed colour definition
		temp = StartDef.Component1.MakeDouble() * InverseFraction;
		temp += EndDef.Component1.MakeDouble() * BlendFraction;
		SourceColour.RGBT.Red = (PColourValue) (temp + 0.5);

		temp = StartDef.Component2.MakeDouble() * InverseFraction;
		temp += EndDef.Component2.MakeDouble() * BlendFraction;
		SourceColour.RGBT.Green = (PColourValue) (temp + 0.5);

		temp = StartDef.Component3.MakeDouble() * InverseFraction;
		temp += EndDef.Component3.MakeDouble() * BlendFraction;
		SourceColour.RGBT.Blue = (PColourValue) (temp + 0.5);

		// We don't bother with component 4, as RGB doesn't use it
	}
	else
	{
		// Pack the start colour straight into our SourceColour
		BlendContext->PackColour(&StartDef, &SourceColour);
	}
}



/********************************************************************************************

>	void DocColour::MixCMYK(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext, ColourContext *OutputContext)
	
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96

	Inputs:		BlendStart - The start DocColour
				BlendEnd - The end DocColour
				BlendFraction - Blending amount between 0.0 and 1.0
				BlendContext - the RGB colour context in which the blend will take place

	Purpose:	Internal helper function for DocColour::Mix.
				This is a reasonably optimised routine for mixing CMYK colours.

	SeeAlso:	DocColour::Mix

********************************************************************************************/

void DocColour::MixCMYK(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext)
{
	ERROR3IF(BlendStart == NULL || BlendEnd == NULL || BlendContext == NULL, "Illegal NULL params");
	ERROR3IF(BlendContext->GetColourModel() != COLOURMODEL_CMYK,
			 "DocColour::MixRGB is optimised for CMYK colours only!");

	InitialiseInfoField(COLOURMODEL_CMYK);

	ColourGeneric StartDef;
	if (BlendStart->IsTransparent())					// If no-colour, then...
	{
		BlendContext->GetWhite(&StartDef);				// ... use white

		if (BlendEnd->IsTransparent())					// If both ends no-colour, return no-colour
			Info.NoColour = TRUE;
	}
	else
		BlendContext->ConvertColour(BlendStart, &StartDef);		// else use start colour

	// If 0.0 then StartDef now has the colour to use (no blending necessary)
	// If BOTH colours are transparent, we will be NoColour, so no need to blend
	if (BlendFraction > 0.0 && !Info.NoColour)
	{
		ColourGeneric EndDef;
		if (BlendEnd->IsTransparent())							// If no-colour then use white
			BlendContext->GetWhite(&EndDef);
		else
			BlendContext->ConvertColour(BlendEnd, &EndDef);

		// Precalculate the blending fractions. Optimisation - Multiply by 255 here to save
		// doing it for each component when we convert them into bytes below.
		BlendFraction *= 255.0;
		const double InverseFraction = (255.0 - BlendFraction);

		double temp;

		// Mix the CMYK components into a 0..255 byte, and store into our packed colour definition
		temp = StartDef.Component1.MakeDouble() * InverseFraction;
		temp += EndDef.Component1.MakeDouble() * BlendFraction;
		SourceColour.CMYK.Cyan = (PColourValue) (temp + 0.5);

		temp = StartDef.Component2.MakeDouble() * InverseFraction;
		temp += EndDef.Component2.MakeDouble() * BlendFraction;
		SourceColour.CMYK.Magenta = (PColourValue) (temp + 0.5);

		temp = StartDef.Component3.MakeDouble() * InverseFraction;
		temp += EndDef.Component3.MakeDouble() * BlendFraction;
		SourceColour.CMYK.Yellow = (PColourValue) (temp + 0.5);

		temp = StartDef.Component4.MakeDouble() * InverseFraction;
		temp += EndDef.Component4.MakeDouble() * BlendFraction;
		SourceColour.CMYK.Key = (PColourValue) (temp + 0.5);
	}
	else
	{
		// Pack the start colour straight into our SourceColour
		BlendContext->PackColour(&StartDef, &SourceColour);
	}
}



/********************************************************************************************

>	void DocColour::MixHSV(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext, BOOL BlendTheLongWay)
	
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96

	Inputs:		BlendStart - The start DocColour
				BlendEnd - The end DocColour
				BlendFraction - Blending amount between 0.0 and 1.0
				BlendContext - the HSV colour context in which the blend will take place
				BlendTheLongWay - FALSE for rainbow, TRUE for Alt-Rainbow mixing

	Purpose:	Internal helper function for DocColour::Mix.
				This is a reasonably optimised routine for mixing HSV colours.

	SeeAlso:	DocColour::Mix

********************************************************************************************/

void DocColour::MixHSV(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
							ColourContext *BlendContext, BOOL BlendTheLongWay)
{
	ERROR3IF(BlendStart == NULL || BlendEnd == NULL || BlendContext == NULL, "Illegal NULL params");
	ERROR3IF(BlendContext->GetColourModel() != COLOURMODEL_HSVT,
				 "DocColour::MixHSV is optimised for HSV colours only!");

	InitialiseInfoField(COLOURMODEL_HSVT);

	ColourHSVT StartDef;
	if (BlendStart->IsTransparent())							// If no-colour then use white
	{
		BlendContext->GetWhite((ColourGeneric *) &StartDef);

		if (BlendEnd->IsTransparent())							// If both ends no-colour, return no-colour
			Info.NoColour = TRUE;
	}
	else
		BlendContext->ConvertColour(BlendStart, (ColourGeneric *) &StartDef);

	// If 0.0 then StartDef now has the colour to use (no blending necessary)
	// If BOTH colours are transparent, we will be NoColour, so no need to blend
	if (BlendFraction > 0.0 && !Info.NoColour)
	{
		ColourHSVT EndDef;
		if (BlendEnd->IsTransparent())							// If no-colour then use white
			BlendContext->GetWhite((ColourGeneric *) &EndDef);
		else
			BlendContext->ConvertColour(BlendEnd, (ColourGeneric *) &EndDef);

		if (BlendTheLongWay)
		{
			// In Alt-rainbow blend, if exaclty one of the colours has undefined hue (it has 0 saturation
			// i.e. is black./grey/white), then Gavin decides to go to the Hue of the other colour
			// (i.e. do a full rainbow) rather than going to hue 0.
			if (StartDef.Saturation.MakeDouble() < 0.01 && EndDef.Saturation.MakeDouble() >= 0.01)
			{
				// Start has no hue, but End does, so copy Hue from End
				StartDef.Hue = EndDef.Hue;
			}
			else if (StartDef.Saturation.MakeDouble() >= 0.01 && EndDef.Saturation.MakeDouble() < 0.01)
			{
				// End has no hue, but Start does, so copy Hue from Start
				EndDef.Hue = StartDef.Hue;
			}
		}

		const double InverseFraction = 1.0 - BlendFraction;

		// HSV blend! We can go 2 ways, as HSV can 'wrap' from 1.0 back to 0.0
		BOOL BlendNormally = TRUE;

		// Calc. the "simple" (non-wrapping) distance between the hues
		double StartHue		= StartDef.Hue.MakeDouble();
		double EndHue		= EndDef.Hue.MakeDouble();

		const double SimpleDist	= fabs(StartHue - EndHue);

		// Determine whether we do a simple blend, or we have to "wrap"
		if (SimpleDist <= 0.5)
			BlendNormally = !(BlendTheLongWay);
		else
			BlendNormally = BlendTheLongWay;

		// If we have to go the long way, then move the smaller of the two hue
		// values up by 360 degrees (1.0) - after blending we'll 'mod' the result
		// back down into gamut.
		if (!BlendNormally)
		{
			if (StartHue > EndHue)
				EndHue += 1.0;
			else
				StartHue += 1.0;
		}

		// Do the blend
		double temp;
		temp = StartHue * InverseFraction;
		temp +=  EndHue * BlendFraction;

		// And if we had to 'wrap', we must 'mod' the value back down into gamut
		if (temp > 1.0)
			temp -= 1.0;

		StartDef.Hue = temp;

		temp = StartDef.Saturation.MakeDouble() * InverseFraction;
		temp +=  EndDef.Saturation.MakeDouble() * BlendFraction;
		StartDef.Saturation = temp;

		temp = StartDef.Value.MakeDouble() * InverseFraction;
		temp +=  EndDef.Value.MakeDouble() * BlendFraction;
		StartDef.Value = temp;
	}

	// Pack the colour straight into our SourceColour
	BlendContext->PackColour((ColourGeneric *) &StartDef, &SourceColour);
}



/********************************************************************************************

>	void DocColour::MixTint(IndexedColour *SpotParent, IndexedColour *StartTint,
							IndexedColour *EndTint, double BlendFraction)
	
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96

	Inputs:		SpotParent	- The IndexedColour which both tints share in common. This
								MUST be the ultimate spot colour, not a tint half way down
								the linked chain.

				StartTint	- The tint colour for the start of the blend. Must be a true
								tint of SpotParent
				EndTint		- The tint colour for the end of the blend. Must be a true
								tint of SpotParent

				BlendFraction - Blending amount between 0.0 and 1.0

	Purpose:	Internal helper function for DocColour::Mix.
				This method is used to mix the DocColours to an IndexedColour result when
				one or both of the end colours are tints. This simply linearly
				interpolates the tint values.

	SeeAlso:	DocColour::Mix

********************************************************************************************/

void DocColour::MixTint(IndexedColour *SpotParent, IndexedColour *StartTint,
						IndexedColour *EndTint, double BlendFraction)
{
	ERROR3IF(SpotParent == NULL || StartTint == NULL || EndTint == NULL,
				"Illegal NULL params");

	ERROR3IF(SpotParent->GetType() != COLOURTYPE_SPOT, "SpotParent must be a spot colour!");

	ERROR3IF(!StartTint->IsSpotOrTintOfSpot(), "StartTint must be a true tint!");
	ERROR3IF(!EndTint->IsSpotOrTintOfSpot(), "EndTint must be a true tint!");

	ColourList *ColList = ColourManager::GetCurrentColourList();
	if (ColList == NULL)
	{
		ERROR3("No current colour list?!");
		return;
	}

	IndexedColour *MixedLocal = new IndexedColour;
	if (MixedLocal == NULL)
	{
		ERROR3("Couldn't create IndexedColour local mix");
		return;
	}

	// Work out the start and end tint values _for use with the common ancestor_.
	double StartValue = StartTint->GetAccumulatedTintValue().MakeDouble();
	double EndValue	 = EndTint->GetAccumulatedTintValue().MakeDouble();

	// Linearly interpolate the tint values
	double NewTint = (StartValue * (1.0 - BlendFraction)) + (EndValue * (BlendFraction));

	// Fill in the new local colour appropriately
	MixedLocal->SetLinkedParent(SpotParent, COLOURTYPE_TINT);
	MixedLocal->SetTintValue(NewTint);
	MixedLocal->SetUnnamed();		// Ensure it's unnamed (local)

	// Add the local colour to the unnamed colour list
	ColList->AddItem(MixedLocal);

	// And make this DocColour reference the new local
	MakeRefToIndexedColour(MixedLocal);
}

/********************************************************************************************

>	DocColour::DocColour& operator=(const DocColour& Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Other - colour to copy
	Outputs:	-
	Returns:	
	Purpose:	DocColour assignment operator
	Notes:		If copying a reference to an indexed colour, the indexed colours
				usage count will be incremented.
				If this is already a reference to an indexed colour, that indexed
				colours usage count will be decremented.

				NOTE - SUPER IMPORTANT!
				This requires that the DocColour has been initialised by its 
				constructor! You cannot thus treat an area of uninitialised memory
				as a DocColour and then try to initialise it using the copy constructor,
				as this could cause a fatal attempt to dereference a pointer.
				(This will hopefully be trapped by ENSURES)

				If you need to initialise such a chunk of memory as a DocColour,
				then you'll need to call the Constructor in a special way. Ask Tim
				for details of how to do this.
	Errors:		-

********************************************************************************************/

DocColour& DocColour::operator=(const DocColour& Other)
{
	ENSURE(Info.SourceColourModel < MAX_COLOURMODELS,
			"Attempt to use illegal colour model!");

	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");

		CC_ASSERT_VALID(SourceColour.Indexed.Colour);

		SourceColour.Indexed.Colour->DecrementUsage();
	}

	Info = Other.Info;
	CachedColour = Other.CachedColour;
	SourceColour = Other.SourceColour;

	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");

		SourceColour.Indexed.Colour->IncrementUsage();
		ENSURE(Info.CacheColourModel == 0, "RefToIndexedColour DocColour has a cache!!");

#ifdef TRACECOLOURIX
TRACE( _T(">> DocColour @ %x = (DocColour => ColourIx %x)\n"), (INT32)this, (INT32)SourceColour.Indexed.Colour);
#endif
	}

	return(*this);
}

/********************************************************************************************

>	void DocColour::MakeRefToIndexedColour(IndexedColour *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/05/94
	Inputs:		Other - Indexed colour to assign
	Outputs:	-
	Returns:	
	Purpose:	DocColour assignment - Makes this DocColour a reference to the
				given Indexed Colour. The Indexed Colour cannot be deleted until
				this DocColour releases this claim upon it (which occurs when it is
				deleted or re-assigned)

	Notes:		This is not done as an "operator=" because it does NOT make an
				independent copy of the other colour, merely a reference to it;
				I force you to use this method in order to remind you of this.
   				This makes the given DocColour a reference to an Indexed Colour,
				incrementing the usage count of that indexed colour.
				If this is already a reference to an indexed colour, that indexed
				colours usage count will be decremented.

	Errors:		-
	SeeAlso:	DocColour:FindParentIndexedColour

********************************************************************************************/

// ---------
// Special debugging code to help track down IndexedColour usage 'leaks'
// Set 'TRACECOLOURIX' at the top of this file if you want tracking information
// spewing out into your trace stream.
void DocColour::MakeRefToIndexedColourDBG(IndexedColour *Other, LPCSTR TheFile, INT32 TheLine)
{
#ifdef TRACECOLOURIX
	TRACE( _T(">> DocColour @ %x ref ColourIx: %x @ %s line %ld\n"), (INT32)this, (INT32)Other, TheFile, TheLine);
#endif

// Disable the debugging macro so that the function code will compile
#undef MakeRefToIndexedColour
// ---------

	MakeRefToIndexedColour(Other);
}



void DocColour::MakeRefToIndexedColour(IndexedColour *Other)
{
	ENSURE(Other != NULL,
		"NULL IndexedColour pointer passed to DocColour::MakeRefToIndexedColour!");

	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");
		SourceColour.Indexed.Colour->DecrementUsage();
	}

	Info.NoColour			= FALSE;
	Info.SourceColourModel	= COLOURMODEL_INDEXED;
	Info.CacheColourModel	= COLOURMODEL_NOTCACHED;	// NEVER cache indexed colours
	Info.OCContextHandle	= 0;
	Info.IsSeparable		= TRUE;						// IxCols are always separable

	SourceColour.Indexed.Colour = Other;
	Other->IncrementUsage();
}



/********************************************************************************************

>	IndexedColour *DocColour::GetSpotParent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96

	Returns:	NULL if this DocColour does not reference an indexed colour, or if the
				referenced colour is not a spot colour, else
				A pointer to the ultimate parent spot colour of this colour.

	Purpose:	To determine which spot colour (if any) is the ultimate spot colour
				parent of this DocColour. (i.e. to see which spot plate a DocColour
				will appear on).

	Notes:		This only returns a non-NULL result if this colour is a TRUE tint of
				a spot colour (as opposed to a tint of a process colour).

********************************************************************************************/

IndexedColour *DocColour::GetSpotParent(void)
{
	IndexedColour *Parent = FindParentIndexedColour();
	if (Parent != NULL)
	{
		if (!Parent->IsSpotOrTintOfSpot())			// If we aren't a TRUE tint, return NULL
			return(NULL);

		Parent = Parent->FindOldestAncestor();		// ...else find our spot parent
		if (Parent->IsSpotOrTintOfSpot())			// (and check just to be really safe)
			return(Parent);
	}

	// no parent, or it's not a spot colour
	return(NULL);
}

/********************************************************************************************

>	BOOL DocColour::operator==(const DocColour Other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Other - colour to compare ourselves against
	Outputs:	-
	Returns:	TRUE if we are 'identical' to the other colour.

	Purpose:	DocColour comparison operator

	Notes:		When comparing 2 colours, they will be considered equal if:
				* They are both 'no colour' (fully transparent), or
				* They are *exactly* equal, i.e. the same colour defined in the same
				  colour model (Pure RGB red is NOT equal to pure HSV red)

				Note that for this comparison, references to indexedcolours
				are compared as references, i.e. it does NOT compare the definitions
				of IndexedColours, only the pointer to the IndexedColours (so 
				IndexedColour references are essentially treated as another colour model)

	Errors:		-
	SeeAlso:	DocColour::GetColourModel

********************************************************************************************/

BOOL DocColour::operator==(const DocColour Other) const
{
	if (Info.NoColour && Other.Info.NoColour)
		return(TRUE);

	return (Info.NoColour == Other.Info.NoColour &&
			Info.SourceColourModel == Other.Info.SourceColourModel &&
			memcmp(&SourceColour, &Other.SourceColour, sizeof(ColourPacked)) == 0 );
}



/********************************************************************************************

>	BOOL DocColour::operator!=(const DocColour Other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Other - colour to compare ourselves against
	Outputs:	-
	Returns:	FALSE if we are identical to the other colour. NOTE that this
				will only be the case if we are both defined in the same colour
				model (i.e. pure red defined in HSVT is NOT equal to pure red in RGBT)
	Purpose:	DocColour comparison operator
	Notes:		If comparing two references to indexed colours, they will only
				be considered equal if they reference the SAME indexed colour
				(i.e. we do not compare the indexed colours)
	Errors:		-
	SeeAlso:	DocColour::GetColourModel

********************************************************************************************/

BOOL DocColour::operator!=(const DocColour Other) const
{
	return (Info.NoColour != Other.Info.NoColour ||
			Info.SourceColourModel != Other.Info.SourceColourModel ||
			memcmp(&SourceColour, &Other.SourceColour, sizeof(ColourPacked)) != 0 );
}

// ---------------------------------------------------------------------------------------
// Functions below this point have been left in purely for backward compatability with the
// old colour system. They may be removed in the future if they are found to be unused.

DocColour::DocColour(INT32 Red, INT32 Green, INT32 Blue)
{
	ZapSourceColour();
	InitialiseInfoField(COLOURMODEL_RGBT);

	SourceColour.RGBT.Red	= (PColourValue) Red;
	SourceColour.RGBT.Green	= (PColourValue) Green;
	SourceColour.RGBT.Blue	= (PColourValue) Blue;
	SourceColour.RGBT.Transparent = 0;
}



void DocColour::SetRGBValue(INT32 Red, INT32 Green, INT32 Blue)
// NOTE that this forces our colourmodel to RGBT.
{
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");
		SourceColour.Indexed.Colour->DecrementUsage();
	}

	InitialiseInfoField(COLOURMODEL_RGBT);

	SourceColour.RGBT.Red	= (PColourValue) Red;
	SourceColour.RGBT.Green	= (PColourValue) Green;
	SourceColour.RGBT.Blue	= (PColourValue) Blue;
	SourceColour.RGBT.Transparent = 0;	
}


void DocColour::GetRGBValue(INT32* Red, INT32* Green, INT32* Blue)
{
	ColourContextRGBT *CCrgbt = (ColourContextRGBT *)ColourContext::GetGlobalDefault(COLOURMODEL_RGBT);
	ColourRGBT result;
	ColourPacked bob;

	CCrgbt->ConvertColour((DocColour *)this, (ColourGeneric *)&result);
	CCrgbt->PackColour((ColourGeneric *)&result, (ColourPacked *)&bob);

	*Red 	= (INT32) bob.RGBT.Red;
	*Green 	= (INT32) bob.RGBT.Green;
	*Blue 	= (INT32) bob.RGBT.Blue;
}


void DocColour::SetHSVValue(INT32 h, INT32 s, INT32 v)
// NOTE that this forces our colourmodel to HSVT.
{
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");
		SourceColour.Indexed.Colour->DecrementUsage();
	}

	InitialiseInfoField(COLOURMODEL_HSVT);

	SourceColour.HSVT.Hue			= (PColourValue) h;
	SourceColour.HSVT.Saturation	= (PColourValue) s;
	SourceColour.HSVT.Value			= (PColourValue) v;
	SourceColour.HSVT.Transparent	= 0;	
}


void DocColour::GetHSVValue(INT32* h, INT32* s, INT32* v)
{
	ColourContextHSVT *CChsvt = (ColourContextHSVT *)ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);
	ColourHSVT result;
	ColourPacked bob;

	CChsvt->ConvertColour((DocColour *)this, (ColourGeneric *)&result);
	CChsvt->PackColour((ColourGeneric *)&result, (ColourPacked *)&bob);

	*h 	= (INT32) bob.HSVT.Hue;
	*s 	= (INT32) bob.HSVT.Saturation;
	*v 	= (INT32) bob.HSVT.Value;
}


void DocColour::SetCMYKValue(PColourCMYK *New)
{
	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");
		SourceColour.Indexed.Colour->DecrementUsage();
	}

	InitialiseInfoField(COLOURMODEL_CMYK);

	memcpy(&SourceColour, New, sizeof(PColourCMYK));
}


void DocColour::GetCMYKValue(PColourCMYK *Result)
{
	ColourContext *CCcmyk = ColourContext::GetGlobalDefault(COLOURMODEL_CMYK);
	CCcmyk->ConvertColour(this, (ColourPacked *) Result);
}


void DocColour::GetCMYKValue(ColourContext* pContext, PColourCMYK *Result)
{
	if (pContext!=NULL)
	{
		ERROR3IF(pContext->GetColourModel() != COLOURMODEL_CMYK, "Colour context is not CMYK!");
		pContext->ConvertColour(this, (ColourPacked *) Result);
		return;
	}

	ColourContext *CCcmyk = ColourContext::GetGlobalDefault(COLOURMODEL_CMYK);
	ERROR3IF(CCcmyk == NULL, "No default CMYK colour context?!");

	CCcmyk->ConvertColour(this, (ColourPacked *) Result);
}



void DocColour::GetDebugDetails(StringBase* Str)
{
	String_256 TempStr;

	if (Info.IsSeparable)
		*Str += TEXT(" Sep ");
	else
		*Str += TEXT(" NotSep ");

	if (Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		ENSURE(SourceColour.Indexed.Colour != NULL,
				"Corrupted DocColour (NULL IndexedColour ref) detected");

		String_64 *pIndexedName = SourceColour.Indexed.Colour->GetName();
//		TempStr._MakeMsg(TEXT(" (#1%s) ref->"), 
//						 pIndexedName);	// This is a reference to an indexed col
//		(*Str) += TempStr;
		(*Str) += TEXT(" (");
		(*Str) += (*pIndexedName);
		(*Str) += TEXT(") ref ->");
		SourceColour.Indexed.Colour->GetDebugDetails(Str);
		return;
	}
		

	if (Info.NoColour)
		TempStr._MakeMsg( TEXT(" colour=transparent\r\n"));
	else
	{		
		ColourContext *cc = ColourContext::GetGlobalDefault((ColourModel) Info.SourceColourModel);
		ColourGeneric col;

		String_32 ModelName;
		cc->GetModelName(&ModelName);
		cc->UnpackColour(&SourceColour, &col);

		TempStr._MakeMsg( TEXT(" colour=#1%s(#2%ld, #3%ld, #4%ld, #5%ld)\r\n"), (TCHAR *) ModelName,
							(INT32) (col.Component1.MakeDouble()*100), (INT32) (col.Component2.MakeDouble()*100),
							(INT32) (col.Component3.MakeDouble()*100), (INT32) (col.Component4.MakeDouble()*100));
	}

	(*Str) += TempStr;
}
