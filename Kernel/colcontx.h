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
// colcontx.h - Output colour context


#ifndef INC_COLCONTX
#define INC_COLCONTX


//#include "colcarry.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "colmodel.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccolor.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "xaracms.h"


class StringBase;
class UnitGroup;
class View;

// Phil 24/02/2004
// Define this variable if you want to disable the old WEBRGBT colour model and 
// transfer any remaining use of it (for instance in old documents) to the RGBT model
#define DISABLE_WEBRGBT 1


/////////////////////////////////////////////////////////////////////////////////
// ColourContextHandle
// This is a handle which uniquely identifies a given Colour Context
// The value is only 16 bits wide, hence it is safe to use an 'INT32'
// The handle 0 may be used internally to indicate invalid contexts
//
// NOTE: To remove dependencies, this is also defined in doccolor.h, colourix.h

typedef UINT32 ColourContextHandle;


/********************************************************************************************

>	class ColourContext : public ListItem

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:
		This class defines a colour context. This is a description of the context
		in which a colour has been defined, including a colour model and extra
		information on special conversion transformations (e.g. for the RGB colour
		model, an extra conversion parameter is 'Gamma correction').

		To use any Colour, the colour context in which it is defined must be
		available. A set of default colour contexts (one per available colour model)
		is kept globally, within each Camelot document, and for each Camelot View.
		(Actually, the document defaults are a bit defunct now).

		To convert a colour to a different colour model, it is necessary to supply
		the colour context in which the colour is defined, plus a colour context
		describing the destination format (although in most cases the former is
		described implicitly)

		Colour contexts make it possible to have several different instantiations
		of a colour model in use at once (e.g. RGB, RGB with gamma correction, and
		RGB for display of the Cyan plate of a CMYK separation), and are also used
		to allow caching of colours in output colour contexts in order to speed
		up time critical operations such as rendering.

	SeeAlso:	Colour; ColourModel; ColourContextList; ColourPlate
	Documentation:	HowToUse\colours.doc
			
********************************************************************************************/

class CCAPI ColourContextRGBT;
class CCAPI ColourPlate;


class CCAPI ColourContext : public ListItem
{
friend class DocColour;			// Give DocColour access to [Un]PackColour() etc
friend class ColourContextList;	// Give CCList access to Init etc functions

friend class GRenderRegion;
friend class OSRenderRegion;

CC_DECLARE_MEMDUMP(ColourContext)

public:
	// Constructor
	ColourContext(View *Scope);


	// Destructor
	virtual ~ColourContext();


	// Class initialiser/deinitialiser (called from app.cpp on init/deinit)
	static BOOL InitColourContexts(void);
	static void DeinitColourContexts(void);


	// Interface
	virtual BOOL IsDifferent(ColourContext *Other) const;

	inline void ConvertColour(DocColour *Source, ColourGeneric *Result);
	void ConvertColour(IndexedColour *Source, ColourGeneric *Result);
	void ConvertColour(ColourContext *SourceContext, ColourGeneric *SourceColour,
						ColourGeneric *Result);

	virtual void ConvertToCIET(ColourGeneric *Source, DColourCIET *Result) = 0;
	virtual void ConvertFromCIET(DColourCIET *Source,ColourGeneric *Result) = 0;
	virtual void CreateExternalTransform();

	inline ColourModel GetColourModel(void) const;

	// Access to global default colour contexts
	inline static ColourContext *GetGlobalDefault(ColourModel ColModel);
	inline static void GetGlobalDefaults(ColourContextArray *Destination);
	inline static ColourContextRGBT *GetCurrentForScreen(void);

	// Determining the text names of this context's colour model and colour components
	virtual void GetModelName(StringBase *Result) = 0;
	virtual BOOL GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName = FALSE) = 0;
	virtual UINT32 GetComponentCount() = 0;
	virtual BOOL SetComponentUnitGroup(UINT32 ComponentID, UnitGroup *pComponentUnitGroup);
	virtual UnitGroup *GetComponentUnitGroup(UINT32 ComponentID);

	// Applying Tinting and Shading to a source colour
	virtual void ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult) = 0;
	virtual void ApplyShade(ColourValue XFraction, ColourValue YFraction, ColourGeneric *SourceAndResult) = 0;

	// Pre/Post-processing output colours for providing Colour Separations etc
	virtual void ApplyInputFilter(ColourPlate *FilterDescription, ColourContext *DestContext,
									ColourGeneric *OutputColour, IndexedColour *SourceIxCol);
	virtual void ApplyOutputFilter(ColourPlate *FilterDescription, ColourContext *SourceContext,
									ColourGeneric *OutputColour, IndexedColour *SourceIxCol);

	// Setting/Reading the ColourPlate descriptor for this context's output filter
	void SetColourPlate(ColourPlate *NewColourPlate);
	ColourPlate *GetColourPlate(void);
	ColourPlate *DetachColourPlate(void);

	// Internal function for ColourPlates - this "flushes" all cached colours in this context
	// by changing our ColourContextHandle. Should be used as infrequently as possible!
	void ColourPlateHasChanged(void);

	// Getting white as it is expressed in this colour model
	virtual void GetWhite(ColourGeneric *Result) = 0;
	virtual BOOL GetProfileTables(BYTE* Tables) { return FALSE; }

	// Determine whether this colour context is logical or physical (device)
PORTNOTE("other","Removed HCMTRANSFORM usage")
#ifndef EXCLUDE_FROM_XARALX
	virtual BOOL IsDeviceContext() const { return CMSTransform != NULL; }
#endif

protected:
	ColourContextHandle	MyHandle;		// Our own OutputColourContext handle
	INT32					UsageCount;		// Count of references to this object

	ColourModel			ColModel;		// The contexts colour model

	ColourPlate			*ColPlate;		// Describes colour plate/separation

PORTNOTE("other","Removed HCMTRANSFORM usage")
#ifndef EXCLUDE_FROM_XARALX
	HCMTRANSFORM		CMSTransform;	// A handle to the calibration manager transform, or NULL
#endif

	View				*ScopeView;		// The view within which this context lives. Used to
										// find "sibling" colour contexts when colour separating

protected:
	static ColourContextHandle NextColourContextHandle;
										// Used to generate unique Context handles

protected:
	// Functions used by our Friend class, ColourContextList
	// Colour contexts *cannot* be used wihtout first being added to the
	// global ColourContext list. IncrementUsage and DecrementUsage therefore
	// should only be called by the list when another instance of the thing
	// is added/removed.
	// SetDefault is used by ColourContextList::InitCC's to set our static data
	virtual BOOL Init(void);
	inline static void SetGlobalDefault(ColourModel ColModel, ColourContext *Default);

	BOOL DecrementUsage(void);
	inline void IncrementUsage(void);


	// Functions used by our Friend class, DocColour.
	// {Un}PackColour are used on generic colour structs. Derived classes will
	// override this function if necessary to change its default behaviour
	virtual void PackColour(ColourGeneric *Source, ColourPacked *Result);
	virtual void UnpackColour(ColourPacked *Source, ColourGeneric *Result);

	void ConvertColourInternal(DocColour *Source, ColourGeneric *Result);
	void ConvertColourInternal(DocColour *Source, ColourPacked *Result);

	// Functions for use only in colcontx.cpp
	inline PColourValue PackColour128(ColourValue Source);
	inline void UnpackColour128(PColourValue Source, FIXED24 *Result);

	inline PColourValue PackColour256(ColourValue Source);
	inline void UnpackColour256(PColourValue Source, FIXED24 *Result);

	inline PColourValue PackColour360(ColourValue Source);
	inline void UnpackColour360(PColourValue Source, FIXED24 *Result);

	virtual UnitGroup **GetComponentUnitGroups() = 0;

public:
	// Functions for whipping out a colour in a packed format. These are not intended
	// for general use (mainly for getting 24bit RGB easily for rendering).
	inline void ConvertColour(DocColour *Source, ColourPacked *Result);

private:
	static ColourContextArray GlobalDefaultContext;

protected:
	virtual void ConvertColourBase(ColourContext *SourceContext,
								ColourGeneric *Source, ColourGeneric *Result,
								IndexedColour *SourceIxCol = NULL);
};



/********************************************************************************************

>	inline void ColourContext::IncrementUsage(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Increments usage-count for a colour context. This count allows us to
				determine when we can delete unused contexts.

	Notes:		The usage count for a colour context defaults to 0
				("not in use"). When using colour contexts, you should always
				register them with the global ColContextList object (see
				ColourContextList::AddContext for details)

	Errors:		-
	SeeAlso:	ColourContext::DecrementUsage; ColourContextList::AddContext

********************************************************************************************/

inline void ColourContext::IncrementUsage(void)
{
	UsageCount++;
}



/********************************************************************************************

>	inline ColourModel ColourContext::GetColourModel(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	The colour model on which this colour context is based
	Purpose:	Determines the colour model on which this colour context is based
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline ColourModel ColourContext::GetColourModel(void) const
{
	return(ColModel);
}



/********************************************************************************************

>	inline static ColourContext *ColourContext::GetCurrentForScreen(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the current screen output colour context for the given colour
				model. (This will either be the Selected Document's RGBT output context, or
				the global default output context, if Selected Document returns NULL)

	Purpose:	Code used by all renderers to find the current screen output context

	Notes:		If you're colour separating, then you should use the context for your
				current render View - this function just gives a logical global default.

********************************************************************************************/

inline ColourContextRGBT *ColourContext::GetCurrentForScreen(void)
{
	Document *Scope = Document::GetSelected();

	if (Scope == NULL)
		return((ColourContextRGBT *) GlobalDefaultContext.Context[COLOURMODEL_RGBT]);
	else
		return((ColourContextRGBT *)
					Scope->GetDefaultColourContexts()->Context[COLOURMODEL_RGBT]);
}



/********************************************************************************************

>	inline static ColourContext *ColourContext::GetGlobalDefault(ColourModel ColModel)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the default colour context for the given colour model,
				within the GLOBAL scope.
	Purpose:	Find a default colour context for a given colour model number within
				GLOBAL scope. This is a static function, so may be called without
				having to create an actual ColourContext instance.
	Errors:		-
	SeeAlso:	ColourContext::GetDefault

********************************************************************************************/

inline ColourContext *ColourContext::GetGlobalDefault(ColourModel ColModel)
{
#ifdef DISABLE_WEBRGBT
	if (ColModel==COLOURMODEL_WEBRGBT)
		ColModel = COLOURMODEL_RGBT;
#endif
	ENSURE(ColModel >= 0 && ColModel < MAX_COLOURMODELS,
			"ColourContext::GetGlobalDefault - Illegal colour model!");

	ENSURE(GlobalDefaultContext.Context[ColModel] != NULL,
			"Requested global default Colour Context is NULL!");

	return(GlobalDefaultContext.Context[ColModel]);
}



/********************************************************************************************

>	inline static void GetGlobalDefaults(ColourContextArray *Destination)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		-
	Outputs:	The supplied destination ColourContextArray will be filled with
				a copy of the global array of default colour context pointers
	Returns:	-
	Purpose:	Find the default contexts used by this 
	Scope:		Pretty private. Should only need to be used by document.cpp
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline void ColourContext::GetGlobalDefaults(ColourContextArray *Destination)
{
	memcpy(Destination, &GlobalDefaultContext, sizeof(ColourContextArray));
}



/********************************************************************************************

>	inline static void ColourContext::SetGlobalDefault(ColourModel ColModel, ColourContext *Default)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Set the default colour context for a given colour model number
	Scope:		private - used internally by ColourContextList::InitColourContexts
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline void ColourContext::SetGlobalDefault(ColourModel ColModel, ColourContext *Default)
{
#ifdef DISABLE_WEBRGBT
	if (ColModel==COLOURMODEL_WEBRGBT)
		ColModel = COLOURMODEL_RGBT;
#endif
	GlobalDefaultContext.Context[ColModel] = Default;
}



/********************************************************************************************

>	inline PColourValue ColourContext::PackColour128(ColourValue Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Source - The source FIXED24 value to be packed into an 8-bit value
	Returns:	A packed representation of the input value

	Purpose:	Given a 32-bit (FIXED24) colour definition value, packs it into a
				7-bit (unsigned byte) colour definition value. The value is rounded
				and clipped to lie within gamut (0.0 <= g <= 1.0).

	Scope:		private - used internally by ColourContext code
	SeeAlso:	ColourContext::UnpackColour128

********************************************************************************************/

inline PColourValue ColourContext::PackColour128(ColourValue Source)
{
	// Get the value as a fixed24 INT32, and round it up by half a 0..127 value
	INT32 result = Source.GetAsFixed24() + 0x00010000;

	// Clip the value into the FIXED24 range 0.0 - 1.0
	if (result > 0x01000000)
		result = 0x01000000;

	if (result < 0)
		result = 0;

	// Multiply by 127, and shift the resulting value down to get a 7-bit value
	result = (result * 127) >> 24;

	return((PColourValue) result);
}



/********************************************************************************************

>	inline void ColourContext::UnpackColour128(PColourValue Source, FIXED24 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Source - The packed input value, as a 7-bit (0..127) value.
	Outputs:	Result - is filled in on exit with an unpacked (FIXED24) version of the
				source 7-bit value

	Purpose:	Unpacks the packed source colour value into a FIXED24 unpacked format

	Scope:		private - used internally by ColourContext code
	SeeAlso:	ColourContext::PackColour128

********************************************************************************************/

inline void ColourContext::UnpackColour128(PColourValue Source, FIXED24 *Result)
{
	INT32 temp = (INT32) Source;

	if (temp >= 127)		// If hit upper limit of gamut, return 1.0
		*Result = 1.0;
	else
	{
		if (temp <= 0)		// if hit lower limit of gamut, return 0.0
			*Result = 0;
		else
		{
			// Repeat the 7 bit value 4 times in the bottom 24 bits of the word
			// (losing the LS 4 bits of the LS copy off the end, hence ">>4")
			Result->SetAsFixed24((temp >> 4) | (temp << 3) | (temp << 10) | (temp << 17));
		}
	}
}



/********************************************************************************************

>	inline PColourValue ColourContext::PackColour256(ColourValue Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Source - The source FIXED24 value to be packed into an 8-bit value
	Returns:	A packed representation of the input value

	Purpose:	Given a 32-bit (FIXED24) colour definition value, packs it into an
				8-bit (unsigned byte) colour definition value. The value is rounded
				and clipped to lie within gamut (0.0 <= g <= 1.0).

	Scope:		private - used internally by ColourContext code
	SeeAlso:	ColourContext::UnpackColour256

********************************************************************************************/

inline PColourValue ColourContext::PackColour256(ColourValue Source)
{
	// Get the value as a fixed24 INT32, and round it up by half a 0..255 value
	INT32 result = Source.GetAsFixed24() + 0x00008000;

	// Clip the value into the FIXED24 range 0.0 - 1.0
	if (result > 0x01000000)
		result = 0x01000000;

	if (result < 0)
		result = 0;

	// Multiply by 255, and shift the resulting value down to get a byte value
	result = (result * 255) >> 24;

	return((PColourValue) result);
}



/********************************************************************************************

>	inline void ColourContext::UnpackColour256(PColourValue Source, FIXED24 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Source - The packed input value, as a 8-bit (0..255) value.
	Outputs:	Result - is filled in on exit with an unpacked (FIXED24) version of the
				source 8-bit value

	Purpose:	Unpacks the packed source colour value into a FIXED24 unpacked format

	Scope:		private - used internally by ColourContext code
	SeeAlso:	ColourContext::PackColour256

********************************************************************************************/

inline void ColourContext::UnpackColour256(PColourValue Source, FIXED24 *Result)
{
	INT32 temp = (INT32) Source;

	if (temp >= 255)		// If hit upper limit of gamut, return 1.0
		*Result = 1.0;
	else
	{
		if (temp <= 0)		// If hit lower limit of gamut, return 0.0
			*Result = 0;
		else
			Result->SetAsFixed24(temp | (temp << 8) | (temp << 16));
	}
}


	
/********************************************************************************************

>	inline PColourValue ColourContext::PackColour360(ColourValue Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Source - The source FIXED24 value to be packed into a 9-bit value
	Returns:	A packed representation of the input value

	Purpose:	Given a 32-bit (FIXED24) colour definition value, packs it into a
				9-bit (0..359) colour definition value. The value is rounded
				and clipped to lie within gamut (0.0 <= g <= 1.0).

	Scope:		private - used internally by ColourContext code
	SeeAlso:	ColourContext::UnpackColour360

********************************************************************************************/

inline PColourValue ColourContext::PackColour360(ColourValue Source)	// 8-bit (Most values)
{
/*
	// Get the value as a fixed24 INT32, and round it up by half a 0..360 value
	INT32 result = Source.GetAsFixed24() + 0x00005606;

	// Clip the value into the FIXED24 range 0.0 - 1.0
	if (result > 0x01000000)
		result = 0x01000000;

	if (result < 0)
		result = 0;

	// Convert to 9.23 bit fixed point, to avoid overflow in the multiply below
	result >>= 1;

	// Multiply by 255, and shift the resulting value down to get a byte value
	result = (result * 359) >> 23;
*/

	// Doubles are more accurate, simpler, and quite possibly faster than the code above
	double result = (Source.MakeDouble() * 359.0) + 0.5;

	if (result > 359.0)
		result = 359.0;

	if (result < 0.0)
		result = 0.0;

	return((PColourValue) result);
}



/********************************************************************************************

>	inline void ColourContext::UnpackColour360(PColourValue Source, FIXED24 *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Source - The packed input value, as a 9-bit (0..359) value.
	Outputs:	Result - is filled in on exit with an unpacked (FIXED24) version of the
				source 9-bit value

	Purpose:	Unpacks the packed source colour value into a FIXED24 unpacked format

	Scope:		private - used internally by ColourContext code
	SeeAlso:	ColourContext::PackColour360

********************************************************************************************/

inline void ColourContext::UnpackColour360(PColourValue Source, FIXED24 *Result)
{
/*
	INT32 temp = (INT32) Source;

	if (temp > 359)	temp = 359;
	if (temp < 0)  	temp = 0;

	// The number fits into 9 bits, so shift it up to get 9.23 bit fixed point
	// then divide by 359 to scale it, then  shift it up by one more bit
	// to convert into 8.24 fixed point (thus we lose the 24th bit of accuracy)

	temp = (temp << 23) / 359;			// Get scaled value as 9.23 bit fp

	Result->SetAsFixed24(temp << 1);	// Convert to 8.24 fp and then to FIXED24
*/

	// Doubles are more accurate, simpler, and quite possibly faster than the code above
	double temp = Source;

	temp /= 359.0;

	if (temp < 0.0)
		temp = 0.0;

	if (temp > 1.0)
		temp = 1.0;

	*Result = temp;
}



/********************************************************************************************

>	inline void ColourContext::ConvertColour(DocColour *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/94
	Inputs:		Source - A Colour to convert
				Result - A ColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.
	Outputs:	-
	Returns:	-
	Purpose:	Converts a Colour into this colour context.
				The converted colour will be supplied from a cache where possible
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline void ColourContext::ConvertColour(DocColour *Source, ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");
	ConvertColourInternal(Source, Result);
}



/********************************************************************************************

>	inline void ColourContext::ConvertColour(DocColour *Source, ColourPacked *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Source - A Colour to convert
				Result - A PColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.
	Outputs:	-
	Returns:	-
	Purpose:	Converts a Colour into this colour context.
				The converted colour will be supplied from a cache where possible
	Scope:		PRIVATE - for use only by 'friend' rendering classes:
					OSRenderRegion, GRenderRegion
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline void ColourContext::ConvertColour(DocColour *Source, ColourPacked *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");
	ConvertColourInternal(Source, Result);
}



















/********************************************************************************************

>	class ColourContextList : public List

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/04/94
	Purpose:	Defines a list of colour contexts, as must be kept on a global basis.
				This is a perfectly normal list, except it has a special function to
				add a ColourContext item to the list, which ensures that no duplicate
				colour contexts are ever used, and that the usage count for any shared
				context is properly updated.

	SeeAlso:	ColourContextList::AddContext; ColourContextList::RemoveContext;
				List; Colour; ColourModel; ColourContext
	Documentation:	HowToUse\colours.doc
		
********************************************************************************************/


class CCAPI ColourContextList : public List
{

CC_DECLARE_MEMDUMP(ColourContextList)

public:
	// Specialised methods to add/remove contexts to/from the list
	void AddContext(ColourContext **NewContext);
	void RemoveContext(ColourContext **OldContext);


	// Class initialiser/deinitialiser (called from app.cpp on init/deinit)
	static BOOL InitColourContexts(void);
	static void DeinitColourContexts(void);

	// Access to 'global' data structure 'ColContextList'
	inline static ColourContextList *GetList(void);


private:
	static ColourContextList *ColContextList;
};



/********************************************************************************************

>	inline static ColourContextList *ColourContextList::GetList(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the list of colour contexts
	Purpose:	Find the 'global' list of current colour contexts
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline ColourContextList *ColourContextList::GetList(void)
{
	return(ColContextList);
}










/********************************************************************************************

>	class ColourContextRGBT : public ColourContext

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/04/94
	Purpose:	Object class representing colour contexts based upon the RGBT
				colour model (Red, Green, Blue, Transparent)
	SeeAlso:	Colour; ColourModel; ColourContext; 
				ColourContextRGBT::ColourContextRGBT
	Documentation:	HowToUse\colours.doc
		
********************************************************************************************/

class CCAPI ColourContextRGBT : public ColourContext
{
friend class DocColour;			// Give DocColour access to [Un]PackColour() etc
friend class ColourContextList;	// Give CCList access to Init etc functions

friend class Colour;			// Colour needs access to [Un]PackColour() but
								// ONLY in an RGBT output context

friend class GRenderRegion;
friend class OSRenderRegion;

CC_DECLARE_MEMDUMP(ColourContextRGBT)

public:		// Overridden Constructor
 	ColourContextRGBT(View *Scope, double GammaValue = 1.0);


public:		// Overridden Interface
	void ConvertToCIET(ColourGeneric *Source, DColourCIET *Result);
	void ConvertFromCIET(DColourCIET *Source,ColourGeneric *Result);
	void CreateExternalTransform();
	virtual void GetModelName(StringBase *Result);
	virtual BOOL GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName = FALSE);
	virtual UINT32 GetComponentCount();

	virtual BOOL IsDifferent(ColourContext *Other) const;

	
	virtual void ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult);
	virtual void ApplyShade(ColourValue XFraction, ColourValue YFraction, ColourGeneric *SourceAndResult);

	virtual void ApplyOutputFilter(ColourPlate *FilterDescription, ColourContext *SourceContext,
									ColourGeneric *OutputColour, IndexedColour *SourceIxCol);

	virtual void GetWhite(ColourGeneric *Result);

public:		// Derived-Class Interface
	double GetGamma(void) const;

	inline DWORD ConvertToScreenWord(DocColour *Source);	// For Renderers only
	inline DWORD ConvertToTransparentScreenWord(DocColour *Source);


protected:
	double Gamma;		// Example wossname data for an individual context

	virtual UnitGroup **GetComponentUnitGroups();

private:
	enum
	{
		MAX_COMPONENTS = 3									// Number of components in colour context
	};
	static UnitGroup *m_pUnitGroupArray[MAX_COMPONENTS];	// unit groups used by RGB components
};



/********************************************************************************************

>	class ColourContextWebRGBT : public ColourContextRGBT

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/11/99
	Purpose:	This class inherits from class ColourContextRGBT, and although (at the
				moment) does not add any new functionality - provides us with an appropriate
				way of allowing us to edit RGBT in hexadecimal (i.e.  rrggbb or 0xrrggbb web
				format).  Reasons for this are:  1)  so that I did NOT have to keep copying
				and pasting code for all the places that a colour model combobox appears.
				2)  allow us to integrate correctly with the edit dialog (thereby not doing
				2). 3) make the addition of subsequent ColourContexts easier (since you
				won't have to worry about this class at all); and 4) cause I felft like it.
	SeeAlso:	Colour; ColourModel; ColourContext; 
				ColourContextRGBT::ColourContextRGBT
	Documentation:	HowToUse\colours.doc
		
********************************************************************************************/

class CCAPI ColourContextWebRGBT : public ColourContextRGBT
{
	CC_DECLARE_MEMDUMP(ColourContextWebRGBT)

public:		// Overridden Constructor
 	ColourContextWebRGBT(View *Scope, double GammaValue = 1.0);

	virtual void GetModelName(StringBase *Result);
};



/********************************************************************************************

>	inline DWORD ColourContextRGBT::ConvertToScreenWord(DocColour *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		Source - the DocColour to convert
	Outputs:	-
	Returns:	The colour, converted into the current screen RGBT context, in the
				form of an RGB WORD (in the same format as a Microsoft/Gavin RGB word)
					i.e. 0x00BBGGRR
	Purpose:	Conversion of colours for screen output, NOT including a transparency
				value (i.e. in a Windows GDI compatible format)
	Scope:		Pretty private. Should only need to be used by renderers
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline DWORD ColourContextRGBT::ConvertToScreenWord(DocColour *Source)
{
	ColourPacked Result;

	ConvertColour(Source, &Result);

	DWORD tmp = (DWORD) Result.RGBT.Red;		// (Compiler bug?) forced me
	tmp |= ((DWORD)Result.RGBT.Green) << 8;		// to do this as 3 stmts instead
	tmp |= ((DWORD)Result.RGBT.Blue) << 16;		// of only one.

	return(tmp);
}



/********************************************************************************************

>	inline DWORD ColourContextRGBT::ConvertToTransparentScreenWord(DocColour *Source)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/94
	Inputs:		Source - the DocColour to convert
	Outputs:	-
	Returns:	The colour, converted into the current screen RGBT context, in the
				form of an RGB WORD (in the same format as a Gavin RGBT word)
					i.e. 0xTTBBGGRR
	Purpose:	Conversion of colours for screen output, including a transparency
				value. (i.e in a Gavin compatible format, but NOT Windows GDI compatible)
	Scope:		Pretty private. Should only need to be used by renderers
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

inline DWORD ColourContextRGBT::ConvertToTransparentScreenWord(DocColour *Source)
{
	ColourPacked Result;

	ConvertColour(Source, &Result);

	DWORD tmp = (DWORD) Result.RGBT.Red;
	tmp |= ((DWORD)Result.RGBT.Green) << 8;
	tmp |= ((DWORD)Result.RGBT.Blue) << 16;
	tmp |= ((DWORD)Result.RGBT.Transparent) << 24;

	return(tmp);
}









/********************************************************************************************

>	class ColourContextCMYK : public ColourContext

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Purpose:	Object class representing colour contexts based upon the CMYK
				colour model (Cyan, Magenta, Yellow, Key)
	SeeAlso:	Colour; ColourModel; ColourContext; 
				ColourContextCMYK::ColourContextCMYK
	Documentation:	HowToUse\colours.doc
		
********************************************************************************************/

class CCAPI ColourContextCMYK : public ColourContext
{
friend class DocColour;			// Give DocColour access to [Un]PackColour() etc
friend class ColourContextList;	// Give CCList access to Init etc functions

friend class GRenderRegion;
friend class OSRenderRegion;

CC_DECLARE_MEMDUMP(ColourContextCMYK)

public:		// Overridden Constructor
 	ColourContextCMYK(View *Scope);
			// Construct a "logical" CMYK context

	ColourContextCMYK(View *Scope, StringBase *NewProfileName);
			// Construct a CMYK context for a specific device profile


public:		// Overridden Interface
	void ConvertToCIET(ColourGeneric *Source, DColourCIET *Result);
	void ConvertFromCIET(DColourCIET *Source,ColourGeneric *Result);
	void CreateExternalTransform();
	virtual void GetModelName(StringBase *Result);
	virtual BOOL GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName = FALSE);
	virtual UINT32 GetComponentCount();

	virtual void ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult);
	virtual void ApplyShade(ColourValue XFraction, ColourValue YFraction, ColourGeneric *SourceAndResult);

	virtual void ApplyInputFilter(ColourPlate *FilterDescription, ColourContext *DestContext,
									ColourGeneric *OutputColour, IndexedColour *SourceIxCol);
	virtual void ApplyOutputFilter(ColourPlate *FilterDescription, ColourContext *SourceContext,
									ColourGeneric *OutputColour, IndexedColour *SourceIxCol);

	virtual BOOL IsDifferent(ColourContext *Other) const;

	virtual void GetWhite(ColourGeneric *Result);

	virtual BOOL GetProfileTables(BYTE* Tables);

public:		// Derived-Class Interface
	String_256 *GetProfileName(void) { return(&ProfileName); };


protected:
	virtual UnitGroup **GetComponentUnitGroups();

private:
	enum
	{
		MAX_COMPONENTS = 4									// Number of components in colour context
	};
	static UnitGroup *m_pUnitGroupArray[MAX_COMPONENTS];	// unit groups used by RGB components

protected:	// Special base-class override
	virtual void ConvertColourBase(ColourContext *SourceContext,
								ColourGeneric *Source, ColourGeneric *Result,
								IndexedColour *SourceIxCol = NULL);

protected:
	String_256 ProfileName;
};





/********************************************************************************************

>	class ColourContextHSVT : public ColourContext

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/04/94
	Purpose:	Object class representing colour contexts based upon the HSVT
				colour model (Red, Green, Blue, Transparent)
	SeeAlso:	Colour; ColourModel; ColourContext; 
				ColourContextHSVT::ColourContextHSVT
	Documentation:	HowToUse\colours.doc
		
********************************************************************************************/

class CCAPI ColourContextHSVT : public ColourContext
{
friend class DocColour;			// Give DocColour access to [Un]PackColour() etc
friend class ColourContextList;	// Give CCList access to Init etc functions

friend class GRenderRegion;
friend class OSRenderRegion;

CC_DECLARE_MEMDUMP(ColourContextHSVT)

public:		// Overridden Constructor
 	ColourContextHSVT(View *Scope);


public:		// Overridden Interface
	void ConvertToCIET(ColourGeneric *Source, DColourCIET *Result);
	void ConvertFromCIET(DColourCIET *Source,ColourGeneric *Result);
	void CreateExternalTransform();
	virtual void GetModelName(StringBase *Result);
	virtual BOOL GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName = FALSE);
	virtual UINT32 GetComponentCount();

	virtual void ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult);
	virtual void ApplyShade(ColourValue XFraction, ColourValue YFraction, ColourGeneric *SourceAndResult);

	virtual void GetWhite(ColourGeneric *Result);

protected:
	virtual void PackColour(ColourGeneric *Source, ColourPacked *Result);
	virtual void UnpackColour(ColourPacked *Source, ColourGeneric *Result);

	virtual UnitGroup** GetComponentUnitGroups();

private:
	enum
	{
		MAX_COMPONENTS = 3									// Number of components in colour context
	};
	static UnitGroup *m_pUnitGroupArray[MAX_COMPONENTS];	// unit groups used by RGB components
};




/********************************************************************************************

>	class ColourContextGreyT : public ColourContext

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/06/94
	Purpose:	Object class representing colour contexts based upon the GreyT
				colour model (Greyscale intensity, Transparent)
	SeeAlso:	Colour; ColourModel; ColourContext; 
				ColourContextGreyT::ColourContextGreyT
	Documentation:	HowToUse\colours.doc
		
********************************************************************************************/

class CCAPI ColourContextGreyT : public ColourContext
{
friend class DocColour;			// Give DocColour access to [Un]PackColour() etc
friend class ColourContextList;	// Give CCList access to Init etc functions

friend class GRenderRegion;
friend class OSRenderRegion;

CC_DECLARE_MEMDUMP(ColourContextGreyT)

public:		// Overridden Constructor
 	ColourContextGreyT(View *Scope);


public:		// Overridden Interface
	void ConvertToCIET(ColourGeneric *Source, DColourCIET *Result);
	void ConvertFromCIET(DColourCIET *Source,ColourGeneric *Result);
	void CreateExternalTransform();
	virtual void GetModelName(StringBase *Result);
	virtual BOOL GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName = FALSE);
	virtual UINT32 GetComponentCount();

	virtual void ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult);
	virtual void ApplyShade(ColourValue XFraction, ColourValue YFraction, ColourGeneric *SourceAndResult);

	virtual void GetWhite(ColourGeneric *Result);

protected:
	virtual UnitGroup **GetComponentUnitGroups();

private:
	enum
	{
		MAX_COMPONENTS = 3									// Number of components in colour context
	};
	static UnitGroup *m_pUnitGroupArray[MAX_COMPONENTS];	// unit groups used by RGB components
};


#endif			// INC_COLCONTX


