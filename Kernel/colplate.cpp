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
// colplate.cpp - Describe colour separation plates for ColourContexts


#include "camtypes.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colcontx.h"
#include "colourix.h"
#include "colplate.h"
//#include "isetres.h"
#include "printctl.h"


CC_IMPLEMENT_DYNCREATE(ColourPlate, ListItem);

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	void ColourPlate::InitObject(ColourPlateType NewType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		25/6/96

	Inputs:		TheType	- Describes what kind of colour plate you want

	Purpose:	Sets this colour plate to the given type, and then initialises ALL
				member variables to suitable defautls for that type.

				Used by all the constructors to share code for initialisation.

	Notes:		Does not check the validity of the arguments (i.e. if you ask for
				COLOURPLATE_SPOT, does not check that the SpotColour is set up)

********************************************************************************************/

void ColourPlate::InitObject(ColourPlateType NewType)
{
	Disabled		= FALSE;

	Type			= NewType;

	Overprint		= FALSE;
	Monochrome		= TRUE;
	Negate			= FALSE;
	ActiveScreens	= FALSE;

	ScreenFunction	= SCRTYPE_NONE;		// represents no setscreen command output

	// And set up a suitable default for screen angle/frequency
	// Use the selected document's printer resolution and LPI defaults
	INT32 DPI	= 1200;		// Defaults in case of catastrophy
	INT32 LPI	= 60;
	
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	TypesetInfo *TPInfo = TypesetInfo::FindTypesetInfoForDoc();
	if (TPInfo != NULL)
	{
		DPI = TPInfo->GetPrintResolution();
		LPI = (INT32) floor(TPInfo->GetDefaultScreenFrequency());
	}
#endif

	ResetScreenToDefaults(DPI, LPI);
}



/********************************************************************************************

>	ColourPlate::ColourPlate()
>	ColourPlate::ColourPlate(ColourPlateType TheType,
								BOOL MonochromePlate = FALSE, BOOL NegatePlate = FALSE)
>	ColourPlate::ColourPlate(IndexedColour *SpotColour,
								BOOL MonochromePlate = FALSE, BOOL NegatePlate = FALSE)
>	ColourPlate::ColourPlate(const ColourPlate &Other);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/12/95

	Inputs:		TheType	- Describes what kind of colour plate you want

				SpotColour - Indicates that this is a spot plate, and which colour is to be
				treated as the spot colour for output. Only this colour and its descendants
				will be output on the plate.

				Monochrome - if TRUE, all output on the plate will be monochromatic (greyscale).
				If FALSE then the plate may contain colour (i.e. a Cyan plate can be output in
				greyscales (e.g. for printer output) or in shades of cyan (e.g. for screen))

				Negate - if TRUE, all output will be negated to give a photographic negative
				of the plate. if FALSE, the output will be left alone.

				Other - Another ColourPlate you want to copy

	Purpose:	Construct ColourPlate descriptions

	Notes:		The default constructor makes a simple COLOURPLATE_NONE plate

	Errors:		ERROR3 reports will occur if you do something stupid

	SeeAlso:	ColourContext

********************************************************************************************/

ColourPlate::ColourPlate()
{
	InitObject(COLOURPLATE_NONE);
}



ColourPlate::ColourPlate(ColourPlateType TheType, BOOL MonochromePlate, BOOL NegatePlate)
{
	ERROR3IF(TheType == COLOURPLATE_SPOT, "Spot plates must be constructed with the Spot Colour constructor");

	InitObject(TheType);

	// And override the defaults with the supplied settings
	Monochrome		= MonochromePlate;
	Negate			= NegatePlate;
}



ColourPlate::ColourPlate(IndexedColour *SpotColour, BOOL MonochromePlate, BOOL NegatePlate)
{
	ERROR3IF(SpotColour == NULL, "Spot plates must include a valid Spot Colour!");

	InitObject(COLOURPLATE_SPOT);

	// And override the defaults with the supplied settings
	Monochrome		= MonochromePlate;
	Negate			= NegatePlate;

	Spot.MakeRefToIndexedColour(SpotColour);
}



ColourPlate::ColourPlate(const ColourPlate &Other)
{
	Disabled		= Other.Disabled;
	Type			= Other.Type;
	Overprint		= Other.Overprint;
	Monochrome		= Other.Monochrome;
	Negate			= Other.Negate;
	ActiveScreens	= Other.ActiveScreens;
	Spot			= Other.Spot;

	ScreenAngle		= Other.ScreenAngle;
	ScreenFrequency	= Other.ScreenFrequency;
	ScreenFunction	= Other.ScreenFunction;
}



/********************************************************************************************

>	ColourPlate &operator=(const ColourPlate &Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/6//96

	Purpose:	ColourPlate assignment operator

********************************************************************************************/

ColourPlate &ColourPlate::operator=(const ColourPlate &Other)
{
	Disabled		= Other.Disabled;
	Type			= Other.Type;
	Overprint		= Other.Overprint;
	Monochrome		= Other.Monochrome;
	Negate			= Other.Negate;
	ActiveScreens	= Other.ActiveScreens;
	Spot			= Other.Spot;

	ScreenAngle		= Other.ScreenAngle;
	ScreenFrequency	= Other.ScreenFrequency;
	ScreenFunction	= Other.ScreenFunction;

	return(*this);
}



/********************************************************************************************

>	ColourPlate::~ColourPlate()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/12/95

	Purpose:	Er... damn. It was on the tip of my tongue...

 ********************************************************************************************/

ColourPlate::~ColourPlate()
{
}



/********************************************************************************************

>	void ColourPlate::GetDescription(StringBase *Description)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		13/6/96

	Outputs:	On return, the Description string will contain a description of this
				colour plate. The string should be more than 64 characters in length,
				or the result could be truncated.

	Purpose:	To retrieve a textual description of this colour plate
				(e.g. "Cyan" "Gold (Spot)")

********************************************************************************************/

void ColourPlate::GetDescription(StringBase *Description)
{
	ERROR3IF(Description == NULL, "Illegal NULL param");
	*Description = TEXT("");

	switch(GetType())
	{
		case COLOURPLATE_CYAN:
			Description->MakeMsg(_R(IDS_COLOURPLATE_CYAN));
			break;

		case COLOURPLATE_MAGENTA:
			Description->MakeMsg(_R(IDS_COLOURPLATE_MAGENTA));
			break;

		case COLOURPLATE_YELLOW:
			Description->MakeMsg(_R(IDS_COLOURPLATE_YELLOW));
			break;

		case COLOURPLATE_KEY:
			Description->MakeMsg(_R(IDS_COLOURPLATE_BLACK));
			break;

		case COLOURPLATE_SPOT:
			{
				IndexedColour *Col = GetSpotColour();
				ERROR3IF(Col == NULL, "NULL Spot colour in spot plate");

				Description->MakeMsg(_R(IDS_COLOURPLATE_SPOT), (TCHAR *) *(Col->GetName()) );
			}
			break;
		default:
			break;
	}
}



/********************************************************************************************

>	void ColourPlate::GetDisplayColour(DocColour *Colour)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		24/6/96

	Outputs:	On return, the Colour parameter will be filled in with an appropriate colour

	Purpose:	To retrieve a suitable user-interface display colour for this plate.
				For a spot plate, this is simply the spot colour itself.
				For the process plates, a suitable colour will be returned.

********************************************************************************************/

void ColourPlate::GetDisplayColour(DocColour *Colour)
{
	ERROR3IF(Colour == NULL, "Illegal NULL param");

	PColourCMYK CMYKDef;
	CMYKDef.Cyan = CMYKDef.Magenta = CMYKDef.Yellow = CMYKDef.Key = 0;

	switch(GetType())
	{
		case COLOURPLATE_CYAN:		CMYKDef.Cyan = 255;		break;
		case COLOURPLATE_MAGENTA:	CMYKDef.Magenta = 255;	break;
		case COLOURPLATE_YELLOW:	CMYKDef.Yellow = 255;	break;
		case COLOURPLATE_KEY:		CMYKDef.Key = 255;		break;

		case COLOURPLATE_SPOT:
			{
				IndexedColour *Col = GetSpotColour();
				ERROR3IF(Col == NULL, "NULL Spot colour in spot plate");

				Colour->MakeRefToIndexedColour(Col);
				return;		// Return immediately
			}
			break;

		default:
			// Just leave it alone, which will return the default value - white
			break;
	}

	// If we've dropped through this far, then set the CMYK value we've
	// filled in, and return
	Colour->SetCMYKValue(&CMYKDef);
}



/********************************************************************************************

>	void ColourPlate::SetDisabled(BOOL IsDisabled = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/5/96

	Inputs:		IsDisabled - TRUE to disable this ColourPlate, FALSE to enable it

	Purpose:	Internally, some colour conversions need to turn of colour separations.
				(e.g. user-interface colours such as dialogue greys and EOR colours).

				These disable separation by setting this disabling flag - this should
				only be used in a VERY temporary sense (i.e. around individual calls
				to ConvertColour).

	Notes:		ColourPlates always default to being Enabled.

	SeeAlso:	ColourContext::ConvertColour; ColourContext::ConvertColourBase

********************************************************************************************/

void ColourPlate::SetDisabled(BOOL IsDisabled)
{
	Disabled = IsDisabled;
}



/********************************************************************************************

>	void ColourPlate::SetPlateInfo(ColourContext *Parent, ColourPlateType TheType,
									BOOL MonochromePlate = FALSE, BOOL NegatePlate = FALSE)
>	void ColourPlate::SetPlateInfo(ColourContext *Parent, IndexedColour *SpotColour,
									BOOL MonochromePlate = FALSE, BOOL NegatePlate = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/12/95

	Inputs:		Parent - the ColourContext this plate is attached to, or NULL if it is currently
				unattached. NOTE that it is vital that you pass the parent context in if it is
				attached, or colours in that context will remain incorrectly cached indefinitely!

				TheType	- Describes what kind of colour plate you want

				SpotColour - Indicates that this is a spot plate, and which colour is to be
				treated as the spot colour for output. Only this colour and its descendants
				will be output on the plate.

				Monochrome - if TRUE, all output on the plate will be monochromatic (greyscale).
				If FALSE then the plate may contain colour (i.e. a Cyan plate can be output in
				greyscales (e.g. for printer output) or in shades of cyan (e.g. for screen))

				Negate - if TRUE, all output will be negated to give a photographic negative
				of the plate. if FALSE, the output will be left alone.

	Purpose:	Change this ColourPlate description

	Notes:		Every time the plate changes, the attached ColourContext must chnage its handle
				in order to "flush" all cached colours. After 65536 such flushes, it is possible
				for contexts to start using duplicate handles, so it is preferable that changes
				are made to plates as infrequently as possible.

				** ALL ColourPlate settings will be reset when you call this function, i.e.
				disabled plates will be re-enabled, new default screen angle/frequency values
				filled in, overprint disabled, etc.

	Errors:		ERROR3 reports will occur if you do something stupid

	SeeAlso:	ColourContext

********************************************************************************************/

void ColourPlate::SetPlateInfo(ColourContext *Parent, ColourPlateType TheType,
								BOOL MonochromePlate, BOOL NegatePlate)
{
	ERROR3IF(TheType == COLOURPLATE_SPOT, "Spot plates must be constructed with the Spot Colour constructor");

	InitObject(TheType);

	Monochrome	= MonochromePlate;
	Negate		= NegatePlate;

	Spot		= DocColour(COLOUR_BLACK);

	if (Parent != NULL)
		Parent->ColourPlateHasChanged();
}


void ColourPlate::SetPlateInfo(ColourContext *Parent, IndexedColour *SpotColour,
								BOOL MonochromePlate, BOOL NegatePlate)
{
	ERROR3IF(SpotColour == NULL, "Spot plates must include a valid Spot Colour!");

	InitObject(COLOURPLATE_SPOT);

	Monochrome	= MonochromePlate;
	Negate		= NegatePlate;

	Spot.MakeRefToIndexedColour(SpotColour);

	if (Parent != NULL)
		Parent->ColourPlateHasChanged();
}



/********************************************************************************************

>	void ColourPlate::SetType(ColourContext *Parent, ColourPlateType TheType,
								IndexedColour *SpotColour = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/12/95

	Inputs:		Parent - the ColourContext this plate is attached to, or NULL if it is currently
				unattached. NOTE that it is vital that you pass the parent context in if it is
				attached, or colours in that context will remain incorrectly cached indefinitely!

				TheType	- Describes what kind of colour plate you want

				SpotColour - NULL, or points to the colour which is to be treated as the spot
				colour for output. Only this colour and its descendants will be output on the
				plate. TheType must be COLOURPLATE_SPOT if this is non-NULL.

	Purpose:	Change this ColourPlate description

	Notes:		Every time the plate changes, the attached ColourContext must chnage its handle
				in order to "flush" all cached colours. After 65536 such flushes, it is possible
				for contexts to start using duplicate handles, so it is preferable that changes
				are made to plates as infrequently as possible.

	Errors:		ERROR3 reports will occur if you do something stupid

	SeeAlso:	ColourContext

********************************************************************************************/

void ColourPlate::SetType(ColourContext *Parent, ColourPlateType TheType, IndexedColour *SpotColour)
{
	ERROR3IF(TheType == COLOURPLATE_SPOT && SpotColour == NULL,
				"Spot plates must include a valid Spot Colour!");

	if (Type != COLOURPLATE_SPOT || Spot.FindParentIndexedColour() != SpotColour)
	{
		InitObject(TheType);

		if (SpotColour != NULL)
			Spot.MakeRefToIndexedColour(SpotColour);

		if (Parent != NULL)
			Parent->ColourPlateHasChanged();
	}
}



/********************************************************************************************

>	void ColourPlate::SetScreenInfo(double Angle, double Frequency)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		24/6/96

	Inputs:		Angle		- The screen angle to use for screening of this plate
								in the range 0..360 degrees
				Frequency	- The screen frequency (lpi) to use for screening of this plate

	Purpose:	Set new screen angle/frequency settings

	SeeAlso:	ColourPlate::ResetScreenToDefaults

********************************************************************************************/

void ColourPlate::SetScreenInfo(double Angle, double Frequency)
{
	ERROR3IF(Angle < 0.0 || Angle >= 360.0, "Illegal screen angle");
	ScreenAngle = Angle;

	ERROR3IF(Frequency < 1.0 || Frequency > 1000.0, "Silly screen frequency");
	ScreenFrequency = Frequency;
}



/********************************************************************************************

>	void ColourPlate::SetOverprint(BOOL On)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		24/6/96

	Inputs:		On - TRUE to enable or FALSE to disable overprinting of this plate

	Purpose:	Sets this plate to overprint or knock-out

********************************************************************************************/

void ColourPlate::SetOverprint(BOOL On)
{
	Overprint = On;
}



/********************************************************************************************

>	void ColourPlate::SetMonochrome(ColourContext *Parent, BOOL On = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/12/95

	Inputs:		Parent - the ColourContext this plate is attached to, or NULL if it is currently
				unattached. NOTE that it is vital that you pass the parent context in if it is
				attached, or colours in that context will remain incorrectly cached indefinitely!

				On - if TRUE, all output on the plate will be monochromatic (greyscale).
				If FALSE then the plate may contain colour (i.e. a Cyan plate can be output in
				greyscales (e.g. for printer output) or in shades of cyan (e.g. for screen))

	Purpose:	Change this ColourPlate description

	Notes:		Every time the plate changes, the attached ColourContext must chnage its handle
				in order to "flush" all cached colours. After 65536 such flushes, it is possible
				for contexts to start using duplicate handles, so it is preferable that changes
				are made to plates as infrequently as possible.

	SeeAlso:	ColourContext

********************************************************************************************/

void ColourPlate::SetMonochrome(ColourContext *Parent, BOOL On)
{
	if (Monochrome != On)
	{
		Monochrome = On;
		if (Parent != NULL)
			Parent->ColourPlateHasChanged();
	}
}



/********************************************************************************************

>	void ColourPlate::SetNegative(ColourContext *Parent, BOOL On = TRUE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		20/12/95

	Inputs:		Parent - the ColourContext this plate is attached to, or NULL if it is currently
				unattached. NOTE that it is vital that you pass the parent context in if it is
				attached, or colours in that context will remain incorrectly cached indefinitely!

				On - if TRUE, all output will be negated to give a photographic negative
				of the plate. if FALSE, the output will be left alone.

	Purpose:	Change this ColourPlate description

	Notes:		Every time the plate changes, the attached ColourContext must chnage its handle
				in order to "flush" all cached colours. After 65536 such flushes, it is possible
				for contexts to start using duplicate handles, so it is preferable that changes
				are made to plates as infrequently as possible.

	SeeAlso:	ColourContext

********************************************************************************************/

void ColourPlate::SetNegative(ColourContext *Parent, BOOL On)
{
	if (Negate != On)
	{
		Negate = On;
		if (Parent != NULL)
			Parent->ColourPlateHasChanged();
	}
}



/********************************************************************************************

>	void ColourPlate::SetActiveScreening(BOOL On = TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Date:		30/8/96

	Inputs:		On - if TRUE, this indicates to internal EPS renderers they need to render using
							  the specified screening functions in this plate
				   - if FALSE, the output device will be unaffected.
				
	Purpose:	This function indicates whether the screen function, angle and frequency described
				in this plate should be used when rendering to EPS devices.

********************************************************************************************/

void ColourPlate::SetActiveScreening(BOOL On)
{
	ActiveScreens = On;
}


/********************************************************************************************

>	void ColourPlate::SetScreenFunction(ScreenType func)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Date:		26/6/96
	Inputs:		func = the screen function to use
	Purpose:	Set a new screen function in this plate

********************************************************************************************/

void ColourPlate::SetScreenFunction(ScreenType func)
{
	ScreenFunction = func;
}



/********************************************************************************************

>	void ColourPlate::ResetScreenToDefaults(INT32 DeviceDPI = 1200, INT32 DefaultFrequency = 60)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		14/8/96

	Inputs:		DeviceDPI -			The anticipated resolution of the output (imagesetting) device
				DefaultFrequency -	The basic screen frequency (LPI) from which to derive the
									recommended defaults

	Purpose:	Resets this ColourPlate's screen angle to the recommended angle for
				its current Screen frequency (lpi) and the current output device
				resolution.

	SeeAlso:	ColourPlate::SetScreenInfo

********************************************************************************************/

void ColourPlate::ResetScreenToDefaults(INT32 DeviceDPI, INT32 DefaultFrequency)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Ask the XaraCMS for recommended angles for the current typesetter settings
	CMSScreenAngles RecAngles;
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	// Set suitable defaults in case of a problem
	RecAngles.cyanfreq = RecAngles.magentafreq =
			RecAngles.yellowfreq = RecAngles.keyfreq = DefaultFrequency;

	RecAngles.cyanangle		= 105.0;
	RecAngles.magentaangle	= 75.0;
	RecAngles.yellowangle	= 90.0;
	RecAngles.keyangle		= 45.0;

	if (lpCMSMan != NULL)
		lpCMSMan->GetRecommendedAngles(DeviceDPI, DefaultFrequency, &RecAngles);

	// Now set a suitable default screen angle for the type of plate
	switch(Type)
	{
		case COLOURPLATE_CYAN:
			ScreenAngle		= RecAngles.cyanangle;
			ScreenFrequency	= RecAngles.cyanfreq;
			break;

		case COLOURPLATE_MAGENTA:
			ScreenAngle		= RecAngles.magentaangle;
			ScreenFrequency	= RecAngles.magentafreq;
 			break;
		
		case COLOURPLATE_YELLOW:
			ScreenAngle		= RecAngles.yellowangle;
			ScreenFrequency	= RecAngles.yellowfreq;
			break;
		
		case COLOURPLATE_KEY:
			ScreenAngle		= RecAngles.keyangle;
			ScreenFrequency	= RecAngles.keyfreq;
			break;

		default:
			ScreenAngle = 45.0;
			ScreenFrequency	= (double) DefaultFrequency;
			break;
	}
#endif
}

