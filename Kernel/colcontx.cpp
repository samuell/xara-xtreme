// $Id: colcontx.cpp 662 2006-03-14 21:31:49Z alex $
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
// colcontx.cpp - ColourContext and derived classes

/*
*/


#include "camtypes.h"

#include "app.h"
#include "colcontx.h"
#include "colormgr.h"
#include "colourix.h"
#include "colplate.h"
#include "doccolor.h"
#include "ensure.h"
//#include "jason.h"
#include "view.h"
#include "scunit.h"

#if defined(EXCLUDE_FROM_RALPH) || defined(EXCLUDE_FROM_XARALX)
#undef  NO_XARACMS
#define NO_XARACMS
#endif

#ifndef NO_XARACMS
#include "xaracms.h"
#endif

CC_IMPLEMENT_MEMDUMP(ColourContext,			ListItem)
CC_IMPLEMENT_MEMDUMP(ColourContextRGBT,		ColourContext)
CC_IMPLEMENT_MEMDUMP(ColourContextWebRGBT,  ColourContextRGBT)
CC_IMPLEMENT_MEMDUMP(ColourContextCMYK,		ColourContext)
CC_IMPLEMENT_MEMDUMP(ColourContextHSVT,		ColourContext)
CC_IMPLEMENT_MEMDUMP(ColourContextGreyT,	ColourContext)

#define new CAM_DEBUG_NEW

/*	NOTE WELL ALL YE WHO ENTER HERE
 *
 *	If you are implementing a new context class, have a look at the implementation
 *  of RGBT, which is a simple base model. Note that you may also find it useful
 *	to look at CMYK, as it overrides the ConvertColourInternal to do special stuff.
 */



/*  DefaultColourContexts
 *
 *	This is a 'global' array of default colour contexts for each of the
 *  possible 16 colour models Camelot can support. When defining/converting 
 *  colours, a default colour context is implicitly specified, if no colour
 *  context is given.
 *  Access is via the static function ColourContext::GetDefault()
 */

ColourContextArray ColourContext::GlobalDefaultContext = 
{
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};



/* NextColourContextHandle 
 *
 * This variable keeps track of the current handle - each colour context must have a 
 * unique handle - this is used in colour caches to identify the context in which
 * they are currently cached, so that they know when the cache is invalid.
 */

ColourContextHandle ColourContext::NextColourContextHandle = 1;




/********************************************************************************************

>	void ColourContext::ColourContext(View *Scope)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Scope - The view within which this context is to be used, or NULL
				for a global (non-view-specific) context
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a Colour context. This is shared code used by
				all derived ColourContextXXXX classes. You cannot create
				vanilla ColourContext objects (virtual functions abound)

	Notes:		The usage count for a colour context defaults to 0
				("not in use"). When using colour contexts, you should always
				register them with the global ColContextList object (see
				ColourContextList::AddContext for details)

				When created, relevant conversion parameters (gamma correction
				etc) are passed in to the constructor.The context stores these
				parameters so it can quickly compare itself to another context,
				but it may defer creation of tables and other essential stuff 
				until the Init() function is called (when it is added to the
				list of available colour contexts). Nobody but the context list
				can call the Init() function.

				The passed-in Scope may be NULL, in which case global scope is
				used. However, if you're using this within the scope of any
				view, note that you must set up ScopeView properly or colour
				separated output of some colour models will be incorrect.

	Errors:		-
	SeeAlso:	ColourContextRGBT::ColourContextRGBT;
				ColourContextCMYK::ColourContextCMYK;
				ColourContextHSVT::ColourContextHSVT;
				ColourContextList::AddContext

********************************************************************************************/

ColourContext::ColourContext(View *Scope)
{
	MyHandle = NextColourContextHandle++;

	ScopeView = Scope;

	UsageCount = 0;

	ColPlate = NULL;

PORTNOTE("other","Removed HCMTRANSFORM usage")
#ifndef EXCLUDE_FROM_XARALX
	CMSTransform = NULL;
#endif
	// ColourModel and Model-specific data must be dealt with in the 
	// constructor/Init() for the derived ColourContextXXXX class
}



/********************************************************************************************

>	void ColourContext::~ColourContext(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94

	Purpose:	Colour context destructor

	Notes:		This destroys any attached ColourPlate descriptor

	SeeAlso:	ColourContextList::AddContext

********************************************************************************************/

ColourContext::~ColourContext()
{
	ENSURE(UsageCount == 0, "ColourContext deleted while still in use!");

	if (ColPlate != NULL)
	{
		delete ColPlate;
		ColPlate = NULL;
	}

#ifndef NO_XARACMS
	if (CMSTransform != NULL)
	{
		if (GetApplication()->GetCMSManager() != NULL)
			GetApplication()->GetCMSManager()->DestroyTransform(CMSTransform);
		CMSTransform = NULL;
	}
#endif
}



/********************************************************************************************

>	BOOL ColourContext::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/04/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if it succeeds
				FALSE if it fails (and it'll set an error description)
	Purpose:	Colour context initialiser. This is called by the ColourContextList
				when it wishes to add a context to the list, and makes the context
				ready for use. This allows the list to compare contexts, and only
				initialise a given context when it knows the initialisation is
				absolutely necessary (e.g. allocating a large chunk of memory or 
				calculating a complex table is avoided until we are sure there are
				no equivalent contexts already available)
				This function is overridden by colour contexts which actually need to
				do some initialisation other than just storing a few values away.
	Errors:		-
	SeeAlso:	ColourContextList::AddContext

********************************************************************************************/

BOOL ColourContext::Init(void)
{
	ENSURE(UsageCount == 0, "ColourContext initialised when in use?!?!");

	IncrementUsage();	// We are now in use. UsageCount is now 1
	return(TRUE);		// Initialisation succeeded
}



/********************************************************************************************

>	void ColourContext::DecrementUsage(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the object is still in use
	Purpose:	Decrements usage-count for a colour context. This count allows us to
				determine when we can delete unused contexts.

	Notes:		The usage count for a colour context defaults to 0
				("not in use"). When using colour contexts, you should always
				register them with the global ColContextList object (see
				ColourContextList::AddContext for details)

	Errors:		ENSURE fails if the usage count goes negative, indicating the context
				is being 'released' more times than it is 'claimed'.
	SeeAlso:	ColourContext::IncrementUsage; ColourContextList::AddContext

********************************************************************************************/

BOOL ColourContext::DecrementUsage(void)
{
	ENSURE(UsageCount > 0, "ColourContext::DecrementUsage - Usage count is negative!");
	return((--UsageCount) != 0);
}



/********************************************************************************************

>	virtual void ColourContext::PackColour(ColourGeneric *Source, ColourPacked *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - A ColourGeneric structure defining the colour in the model
				pertaining to this colour context.
				Result - A Packed colour structure in the same model, into which the result
				is copied.

	Outputs:	-
	Returns:	-
	Purpose:	Converts 128-bit colour representation to 32-bit packed form

	Notes:		This function operates on the colour using a generic component pattern
				Colours such as ColourHSVT do not conform to this pattern and thus must
				be unpacked with derived methods (ColourContextHSVT::UnpackColour)
				However, this function will work on any 4-component model which uses 8bits
				and 32-bits respectively for all 4 components, and the same 8.24 fixedpoint
				32-bit representation. 

	Scope:		Private (used internally by friend class DocColour)
	Errors:		-
	SeeAlso:	ColourContext::UnpackColour

********************************************************************************************/

void ColourContext::PackColour(ColourGeneric *Source, ColourPacked *Result)
{
	PColourGeneric *bob = (PColourGeneric *) Result;

	bob->Component1 = PackColour256(Source->Component1);
	bob->Component2 = PackColour256(Source->Component2);
	bob->Component3 = PackColour256(Source->Component3);
	bob->Component4 = PackColour256(Source->Component4);
}




/********************************************************************************************

>	virtual void ColourContext::UnpackColour(ColourPacked *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - A Packed Colour structure defining the colour in the model
				pertaining to this colour context.
				Result - A ColourGeneric structure in the same model, into which the 
				result is copied.
	Outputs:	-
	Returns:	-
	Purpose:	Converts 32-bit packed colour representation to 128-bit form

	Notes:		This function operates on the colour using a generic component pattern
				Colours such as ColourHSVT do not conform to this pattern and thus must
				be unpacked with derived methods (ColourContextHSVT::UnpackColour)
				However, this function will work on any 4-component model which uses 8bits
				and 32-bits respectively for all 4 components, and the same 8.24 fixedpoint
				32-bit representation. 

	Scope:		Private (used internally by friend class DocColour)
	Errors:		-
	SeeAlso:	ColourContext::PackColour

********************************************************************************************/

void ColourContext::UnpackColour(ColourPacked *Source, ColourGeneric *Result)
{
	PColourGeneric *bob = (PColourGeneric *) Source;

	UnpackColour256(bob->Component1, &Result->Component1);
	UnpackColour256(bob->Component2, &Result->Component2);
	UnpackColour256(bob->Component3, &Result->Component3);
	UnpackColour256(bob->Component4, &Result->Component4);
}



/********************************************************************************************

>	virtual BOOL ColourContext::IsDifferent(ColourContext *Other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Other - a colour context to compare ourselves to
	Outputs:	-
	Returns:	TRUE if the other colour context object is different from
				ourself, or FALSE if it is an exactly equivalent context.
	Purpose:	Determine if two colour contexts are not exactly equivalent
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL ColourContext::IsDifferent(ColourContext *Other) const
{
	if (Other == this)
		return(FALSE);						// Different object? (Nope)

	if (Other->ScopeView != ScopeView)
		return(TRUE);						// Different Scope Views?

	return(Other->ColModel != ColModel);	// Different Colour Model?
}




/********************************************************************************************

>	virtual void ColourContext::ConvertColourBase(ColourContext *SourceContext,
											ColourGeneric *Source, ColourGeneric *Result
											IndexedColour *SourceIxCol = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		SourceContext - The context in which the source is defined 

				Source - A ColourGeneric to convert

				Result - A ColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.

				SourceIxCol - NULL, or a pointer to the IndexedColour we are converting.
				This is used only for handling spot colour separations, to determine if
				the colour is a given spot colour or tint thereof.

	Outputs:	Result - will be returned with the converted colour

	Purpose:	Converts a Colour into this colour context, from the default colour context
				implied by the given colour model
				Post-processing (as described by the attached ColourPlate object) will
				be applied to generate colour separations (etc) as required.

				The converted colour will be supplied from a cache where possible

	Notes:		It is up to the caller to update caches etc if necessary
				This is used as a base on which ConvertColourInternal methods are built

				ColourContextCMYK overrides this base class method to provide direct
				passthrough of CMYK colours into a CMYK output context. (passthrough
				otherwise only occurs if the source & destination contexts are the
				same object)

	Scope:		private to colcontx.cpp
	Errors:		-
	SeeAlso:	ColourContext::ConvertColour

********************************************************************************************/

void ColourContext::ConvertColourBase(ColourContext *SourceContext,
										ColourGeneric *Source, ColourGeneric *Result,
										IndexedColour *SourceIxCol)
{
#ifndef NO_XARACMS
	// Last minute bodge for Composite preview of CMYK colours in RGB space
	// This is done here cos it's much faster, and gets around pre/post filter problems
	// with CMYK colours which aren't IndexedColours (like intermediate blend/gradfill DocColours)
	if (ColPlate != NULL && ColPlate->GetType() == COLOURPLATE_COMPOSITE &&
		GetColourModel() == COLOURMODEL_RGBT &&
		SourceContext->GetColourModel() == COLOURMODEL_CMYK)
	{
		// If it's a CMYK colour, then it's already in printer gamut, so we only apply
		// the backward colour correction factor to it.
		// Note that this means we chuck away the normal colour conversion system!
		XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
		if (lpCMSMan != NULL)
		{
			ColourCMYK Def;
			memcpy(&Def, Source, sizeof(ColourGeneric));

			lpCMSMan->ConvertColourForPaperView(&Def, (ColourRGBT *) Result);
			return;
		}
	}
#endif

	// Copy the source colour into a temporary variable
	ColourGeneric FilteredSource;
	memcpy(&FilteredSource, Source, sizeof(ColourGeneric));

	// Call the SourceContext to allow it to pre-filter the colour. This is used to
	// provide colour separations and other doody features when converting CMYK colours
	// - when converting non-CMYK colours, this is usually done in the OutputFilter (below)
	if (ColPlate != NULL && !ColPlate->IsDisabled())
		SourceContext->ApplyInputFilter(ColPlate, this, &FilteredSource, SourceIxCol);

	// -----
	// Call levels expected:
	//	1 inline ConvertColour checks if can satisfy from cache
	//	2 function ConvertColourInternal does any short cuts it can, such
	//	  as CMYK->CMYK passthrough
	//	3 Call ConvertColourBase, which
	//		a) Checks if in same colour model, and implements passthrough, or
	//		b) Converts colour properly via CIET space

	if (SourceContext == this)
	{
		// The SourceContext and Destination context are identical, so we can
		// just copy the definition directly across.
		memcpy(Result, &FilteredSource, sizeof(ColourGeneric));
	}
	else
	{
		DColourCIET IntermediateResult;

		SourceContext->ConvertToCIET(&FilteredSource, &IntermediateResult);
		ConvertFromCIET(&IntermediateResult, Result);
	}


	// Call the DestinationContext (derived class of this) to apply any output filtering -
	// non-CMYK colour conversions will separate the colour (as appropriate) here
	if (ColPlate != NULL && !ColPlate->IsDisabled())
		ApplyOutputFilter(ColPlate, SourceContext, Result, SourceIxCol);
}




/********************************************************************************************

>	void ColourContext::ConvertColourInternal(DocColour *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - A Colour to convert
				Result - A ColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.
	Outputs:	-
	Returns:	-
	Purpose:	Converts a Colour into this colour context.
				The converted colour will be supplied from a cache where possible

	Notes:		This is used internally. It is only called by the inlined conversion
				functions if they fail to return a cached result
	Errors:		-
	SeeAlso:	ColourContext::ConvertColour

********************************************************************************************/

void ColourContext::ConvertColourInternal(DocColour *Source, ColourGeneric *Result)
{
	if (Source->Info.SourceColourModel == COLOURMODEL_INDEXED)
	{
		IndexedColour *SrcIndexed = Source->SourceColour.Indexed.Colour;

		// Convert the indexed colour to our context
		ConvertColour(SrcIndexed, Result);

		// If IndexedColour, we do NOT want the colour cached at the DocColour
		// level (else if the IxCol changes, we'd have to search for all related
		// DocCols to fix their caches... erg!)
		return;
	}

	// Unpack the 32-bit colour into a 128-bit structure, and then invoke the
	// shared internal conversion routine.
	ColourGeneric SourceDefn;

	// If this DocColour should not be separated (it's a UI colour), then disable
	// any active colour plate to stop it separating the output - we'll save and restore
	// its previous state around the call to ConvertColourBase
	BOOL WasDisabled = TRUE;
	if (ColPlate != NULL)
	{
		WasDisabled = ColPlate->IsDisabled();
		ColPlate->SetDisabled(!Source->IsSeparable());
	}

		// Check if we're doing a spot colour plate. We either get the colour's source colour,
		// or if it shouldn't appear on the plate, we get White.
		if (ColPlate != NULL && !ColPlate->IsDisabled() && ColPlate->GetType() == COLOURPLATE_SPOT)
		{
			// We're doing a SPOT colour plate, but this colour is a local DocColour, so
			// it cannot possibly be a tint on this plate, so we just return white.
			GetGlobalDefault(Source->GetColourModel())->GetWhite(&SourceDefn);
		}
		else
		{
			// Unpack the colour
			GetGlobalDefault(Source->GetColourModel())->UnpackColour(&Source->SourceColour, &SourceDefn);
		}

		// Assume that the source context is a global default. Generally, this should be true,
		// as all source colours should be defined in terms of logical colour models.
		ConvertColourBase(GetGlobalDefault(Source->GetColourModel()),
					 		&SourceDefn, Result);

	// And restore the previous ColourPlate "disabled" state
	if (ColPlate != NULL)
		ColPlate->SetDisabled(WasDisabled);

	PackColour(Result, &Source->CachedColour);	// And update the cache

	// Update cache flags - I am the context in which the cache is defined
	Source->Info.OCContextHandle  = MyHandle;
	Source->Info.CacheColourModel = ColModel;
}



/********************************************************************************************

>	void ColourContext::ConvertColourInternal(DocColour *Source, ColourPacked *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Source - A Colour to convert
				Result - A PColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.
	Outputs:	-
	Returns:	-
	Purpose:	Converts a Colour into this colour context.
				The converted colour will be supplied from a cache where possible (this check
				is made by the caller function ConvertColour)
	Scope:		PRIVATE - for use only by 'friend' rendering classes:
					OSRenderRegion, GRenderRegion
	Errors:		-
	SeeAlso:	ColourContext::ConvertColour

********************************************************************************************/

void ColourContext::ConvertColourInternal(DocColour *Source, ColourPacked *Result)
{
	ColourGeneric NewResult;
	ConvertColourInternal(Source, &NewResult);	// Convert

	PackColour(&NewResult, Result);				// Pack the colour into Result...

	if (Source->Info.SourceColourModel != COLOURMODEL_INDEXED)
	{
		// And (if not an IndexedColour) update the cache
		Source->Info.OCContextHandle  = MyHandle;
		Source->Info.CacheColourModel = ColModel;
		memcpy(&Source->CachedColour, Result, sizeof(ColourPacked));
	}
}



/********************************************************************************************

>	void ColourContext::ConvertColour(IndexedColour *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - An IndexedColour to convert
				Result - A ColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.
	Outputs:	-
	Returns:	-
	Purpose:	Converts an indexed colour into this colour context.
				The converted colour will be supplied from a cache where possible
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourContext::ConvertColour(IndexedColour *Source, ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	if (Source->Info.OCContextHandle != MyHandle)	// Uncached so convert it into the cache
	{
		// Ask the colour to get its source colour for us. If this is a Linked/Tint colour,
		// this could well involve getting another colour's definition, converting it, and
		// generally giving us an extended tour of stack city.
		ColourGeneric SourceColour;
		Source->GetSourceColour(&SourceColour);

		// Check if we're doing a spot colour plate. We either get the colour's source colour,
		// or if it shouldn't appear on the plate, we get White.
		if (ColPlate != NULL && !ColPlate->IsDisabled())
		{
			switch(ColPlate->GetType())
			{
				case COLOURPLATE_SPOT:
					// If we're doing a spot plate and this isn't a descendant of the spot colour
					// then we must eliminate it from this plate. We must also check that it's a "true"
					// spot/tint, as opposed to a tint which is really a process colour.
					if ((!Source->IsADescendantOf(ColPlate->GetSpotColour())) ||
						(!Source->IsSpotOrTintOfSpot()))
					{
						// Fill it with whatever White is defined as in the source context
						GetGlobalDefault(Source->GetColourModel())->GetWhite(&SourceColour);
					}
					break;

				case COLOURPLATE_CYAN:
				case COLOURPLATE_MAGENTA:
				case COLOURPLATE_YELLOW:
				case COLOURPLATE_KEY:
					// If we're doing a process plate, eliminate any spot colours (or tints) from it
					if (Source->IsSpotOrTintOfSpot())
					{
						// Fill it with whatever White is defined as in the source context
						GetGlobalDefault(Source->GetColourModel())->GetWhite(&SourceColour);
					}
					break;
				default:
					break;
			}
		}

		ConvertColourBase(GetGlobalDefault(Source->GetColourModel()),
						 	&SourceColour, &Source->CachedColour,
							Source);

		Source->Info.OCContextHandle  = MyHandle;
		Source->Info.CacheColourModel = ColModel;
	}

	// Copy the result from our cache
	memcpy(Result, &Source->CachedColour, sizeof(ColourGeneric));
}



/********************************************************************************************

>	void ColourContext::ConvertColour(ColourContext *SourceContext,
									 	ColourGeneric *SourceColour,
										ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		SourceContext - The colour context in which the Source Colour is defined
				Source - A ColourGeneric structure defining a colour to convert, in the
				given SourceContext
				Result - A ColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContext.
	Outputs:	-
	Returns:	-
	Purpose:	Converts a colour in any arbitrary colour context into this colour context.
				The converted colour will be supplied from a cache where possible
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ColourContext::ConvertColour(ColourContext *SourceContext,
									ColourGeneric *SourceColour,
									ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	// This may look like an opportunity for optimisation, but
	// ConvertColourBase is actually an inline function
	ConvertColourBase(SourceContext, SourceColour, Result);
}



/********************************************************************************************

>	virtual void ColourContext::CreateExternalTransform(void)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/96
	Inputs:		-
	Returns:	-
	Purpose:	Create an external transform to be used during colour conversions from
				RGB to the current RCS space. This function uses the Winoil CMS Manager
				to create the transform. Both forward and backward transforms use logical
				RGB descriptions.

********************************************************************************************/

void ColourContext::CreateExternalTransform()
{
	// We do nothing here. If no one creates an external transform, the default
	// internal one will be used. To create an external transform, call the derived class
	// function, not this one as you can see it does nothing.
}


/********************************************************************************************

>	virtual void ColourContext::ApplyInputFilter(ColourPlate *FilterDescription,
													ColourContext *DestContext,
													ColourGeneric *OutputColour,
													IndexedColour *SourceIxCol);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		FilterDescription - A ColourPlate describing how the colour is to be filtered
	
				DestContext - Points to the context to which the OutputColour will be
				converted after we return it.

				OutputColour - The colour to be filtered

				SourceIxCol - NULL, or the original IndexedColour which is being converted.
				This is used for handling spot colour separation correctly.

	Outputs:	OutputColour - Will be modified as appropriate to the current colour
				filtering options (see Purpose)

	Purpose:	All colour conversions call this function for the SOURCE context
				immediately before converting the colour. This gives the input colour
				context the chance to apply a filtering process to the colour.

				This is usually used to provide colour separations. Often, this function
				will do nothing to the output colour - generally it is only used by CMYK
				contexts in order to colour-separate the colour in CMYK space.

	SeeAlso:	ColourContext::ConvertColourBase

********************************************************************************************/

void ColourContext::ApplyInputFilter(ColourPlate *FilterDescription, ColourContext *DestContext,
											ColourGeneric *OutputColour, IndexedColour *SourceIxCol)
{
//	ERROR3IF(FilterDescription == NULL || DestContext == NULL || OutputColour == NULL, "Illegal NULL params");
//
//	if (FilterDescription == NULL)
//		return;
//
}



/********************************************************************************************

>	virtual void ColourContext::ApplyOutputFilter(ColourPlate *FilterDescription,
													ColourContext *SourceContext,
													ColourGeneric *OutputColour,
													IndexedColour *SourceIxCol);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		FilterDescription - A ColourPlate describing how the colour is to be filtered
	
				SourceContext - Points to the context from which the OutputColour has just
				been converted. 

				OutputColour - The colour to be filtered

				SourceIxCol - NULL, or the IndexedColour which was the source of the colour
				being converted. This is only used for information when preparing spot colour
				separations.

	Outputs:	OutputColour - Will be modified as appropriate to the current colour
				filtering options (see Purpose)

	Purpose:	All colour conversions call this function for the DEST context immediately
				prior to returning the converted colour to the caller. This gives the output
				colour context the chance to apply an output filtering process to the output
				colour.

				This is usually used to provide colour separations, and may also be used
				to apply some colour corrections. Often, this function will do nothing
				to the output colour.

	SeeAlso:	ColourContext::ConvertColourBase

********************************************************************************************/

void ColourContext::ApplyOutputFilter(ColourPlate *FilterDescription, ColourContext *SourceContext,
											ColourGeneric *OutputColour, IndexedColour *SourceIxCol)
{
//	ERROR3IF(FilterDescription == NULL || SourceContext == NULL || OutputColour == NULL, "Illegal NULL params");
//
//	if (FilterDescription == NULL)
//		return;
}



/********************************************************************************************

>	void ColourContext::SetColourPlate(ColourPlate *NewColourPlate)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/96
	Inputs:		NULL (to disable colour seps), or points to the new ColourPlate descriptor object
				NOTE that the ColourPlate object now belongs to the ColourContext, and will be
				deleted by the context when it is finished with it.

	Purpose:	Sets a colour separation/plate for this context

	Notes:		The ColourPlate object now belongs to the ColourContext, and will be
				deleted by the context when it is finished with it.

				(To remove the ColourPlate again in a way that does not delete the ColourPlate,
				call ColourContext::DetachColourPlate rather than SetColourPlate(NULL))

				The Context's cache handle will be changed so that all cached colours
				in this output context are effectively 'flushed'.

	SeeAlso:	ColourPlate; ColourContext::ApplyOutputFilter; ColourContext::GetColourPlate;
				ColourContext::DetachColourPlate

********************************************************************************************/

void ColourContext::SetColourPlate(ColourPlate *NewColourPlate)
{
	// Trying to set ColourPlate to the one we're already using - ignore them!
	if (ColPlate == NewColourPlate)
		return;

	// Delete our current ColourPlate
	if (ColPlate != NULL)
	{
		delete ColPlate;
		ColPlate = NULL;
	}

	// Remember our new ColourPlate filter descriptor
	ColPlate = NewColourPlate;

	// Change our context handle so that all cached colours in this context
	// are effectively "flushed"
	ColourPlateHasChanged();
}



/********************************************************************************************

>	ColourPlate *ColourContext::DetachColourPlate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96

	Returns:	The pointer to the previously attached ColourPlate (may be NULL)

	Purpose:	Detaches the ColourPlate (if any) currently attached to this ColourContext.
				The context will no longer use the colour plate descriptor, and will return
				to default colour separation/correction actions.
				
				** The caller is now responsible for deleting the returned object **

	Notes:		The Context's cache handle will be changed so that all cached colours
				in this output context are effectively 'flushed'.

	SeeAlso:	ColourPlate; ColourContext::GetColourPlate; ColourContext::SetColourPlate

********************************************************************************************/

ColourPlate *ColourContext::DetachColourPlate(void)
{
	ColourPlate *OldColourPlate = ColPlate;

	if (ColPlate != NULL)
	{
		ColPlate = NULL;

		// Change our context handle so that all cached colours in this context
		// are effectively "flushed"
		ColourPlateHasChanged();
	}

	return(OldColourPlate);
}



/********************************************************************************************

>	ColourPlate *ColourContext::GetColourPlate(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/01/96

	Returns:	NULL if Colour separation is disabled on this context, else a pointer
				to a ColourPlate descriptor object which describes the current colour
				separation/filtering options.

	Purpose:	Reads the colour separation/plate for this context

	SeeAlso:	ColourPlate; ColourContext::ApplyOutputFilter; ColourContext::SetColourPlate

********************************************************************************************/

ColourPlate *ColourContext::GetColourPlate(void)
{
	return(ColPlate);
}



/********************************************************************************************

>	void ColourContext::ColourPlateHasChanged(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96

	Purpose:	Effectively flushes the caches of all colours currently cached in this
				ColourContext, so that on the next redraw they will re-convert. 

	Notes:		This is achieved by generating a new unique handle for this Context.
				Note that after 65536 handles, we will start re-using handles, and
				things will look bad if we inadvertently use the same handle for 2
				contexts. This is an extremely unlikely occurrence, but please do
				not call this function unless it is absolutely necessary, to minimise
				the number of handle changes that occur.

				This function is automatically called whenever you change the
				ColourPlate attached to this context in any way.

	Scope:		Intended as an internal routine; also called by ColourPlate class
				Should not be called by anyone else

	SeeAlso:	ColourContext::SetColourPlate; IndexedColour; DocColour

********************************************************************************************/

void ColourContext::ColourPlateHasChanged(void)
{
	// We need to change this context's "MyHandle" so that all cached colours for this
	// context are treated as non-cached again, and will be converted correctly for
	// the new output filter.
	MyHandle = NextColourContextHandle++;
}



/********************************************************************************************

>	virtual BOOL ColourContext::SetComponentUnitGroup(UINT32 ComponentID,
														UnitGroup* pComponentUnitGroup)
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Inputs:		ComponentIndex : the number of the component (1..GetComponentCount())
				ComponentUnitGroup : pointer to the required UnitGroup for the component 
	Outputs:	-
	Returns:	TRUE if set OK
				FALSE otherwise
	Purpose:	Allows the UnitGroup of the given component in the ColourModel to be set
	Errors:		ERROR2IF ComponentIndex < 1 or greater than number of components in model

********************************************************************************************/

BOOL ColourContext::SetComponentUnitGroup(UINT32 ComponentID, UnitGroup* pComponentUnitGroup)
{
	ERROR2IF(ComponentID < 1 || ComponentID > GetComponentCount(), FALSE, "Invalid Index");
	ERROR2IF(pComponentUnitGroup == NULL, FALSE, "Invalid UnitGroup");

	ERROR3IF(!pComponentUnitGroup->IS_KIND_OF(UnitGroup), "Invalid UnitGroup");

	UnitGroup** pUnitGroupArray = GetComponentUnitGroups();
	ERROR3IF(!(pUnitGroupArray[ComponentID - 1]->IS_KIND_OF(UnitGroup)), 
				"ColourContext::GetComponentUnitGroup - Not UnitGroup array");

	pUnitGroupArray[ComponentID - 1] = pComponentUnitGroup;
	return TRUE;
}



/********************************************************************************************

>	virtual UnitGroup *ColourContext::GetComponentUnitGroup(UINT32 ComponentID)

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Inputs:		ComponentID - the number of the component (1..GetComponentCount())
	Outputs:	-
	Returns:	A pointer to the desired UnitGroup
	Purpose:	Provides the UnitGroup of the given component in the ColourModel
	Errors:		ERROR2IF ComponentIndex < 1 or greater than number of components in model
	See Also:	ColourContextCMYK::GetComponentUnitGroups()
				ColourContextGreyT::GetComponentUnitGroups()
				ColourContextHSVT::GetComponentUnitGroups()
				ColourContextRGBT::GetComponentUnitGroups()

********************************************************************************************/

UnitGroup *ColourContext::GetComponentUnitGroup(UINT32 ComponentID)
{
	ERROR2IF(ComponentID < 1 || ComponentID > GetComponentCount(), FALSE, "Invalid ID");
	UnitGroup** pUnitGroupArray = GetComponentUnitGroups();
	ERROR3IF(!(pUnitGroupArray[ComponentID - 1]->IS_KIND_OF(UnitGroup)), 
				"ColourContext::GetComponentUnitGroup - Not UnitGroup array");

	return pUnitGroupArray[ComponentID - 1];
}


















/********************************************************************************************

>	void ColourContextRGBT::ColourContextRGBT(View *Scope, double GammaValue = 1.0)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Scope - The view in which this context is to be used, or NULL
				GammaValue - A gamma-correction gamma factor. You know... Gamma.
				Heck, if you don't know what a gamma value is, then don't pass
				anything in, because it has a good default value.
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an RGBT Colour context

	Notes:		Colour Contexts should not be created and used directly. See the
				notes in the SeeAlso's for instructions on proper care and use.

	Errors:		-
	SeeAlso:	ColourContext::ColourContext; ColourContextList::AddContext

********************************************************************************************/

ColourContextRGBT::ColourContextRGBT(View *Scope, double GammaValue)
				  : ColourContext(Scope)
{
	ColModel = COLOURMODEL_RGBT;
	Gamma = GammaValue;

	CreateExternalTransform();
}



/********************************************************************************************

>	ColourContextWebRGBT::ColourContextWebRGBT(View *Scope, double GammaValue)
				  : ColourContextRGBT(Scope, GammaValue)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	??/11/99
	Inputs:		Scope - The view in which this context is to be used, or NULL
				GammaValue - A gamma-correction gamma factor. You know... Gamma.
				Heck, if you don't know what a gamma value is, then don't pass
				anything in, because it has a good default value.
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an RGBT Web Colour context

	Notes:		Colour Contexts should not be created and used directly. See the
				notes in the SeeAlso's for instructions on proper care and use.

	Errors:		-
	SeeAlso:	ColourContext::ColourContext; ColourContextList::AddContext

********************************************************************************************/

ColourContextWebRGBT::ColourContextWebRGBT(View *Scope, double GammaValue)
				  : ColourContextRGBT(Scope, GammaValue)
{
	ColModel = COLOURMODEL_RGBT;
}



/********************************************************************************************

>	void ColourContextRGBT::CreateExternalTransform(void)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/96
	Inputs:		-
	Returns:	-
	Purpose:	Create an external transform to be used during colour conversions from
				RGB to the current RCS space. This function uses the Winoil CMS Manager
				to create the transform. Both forward and backward transforms use logical
				RGB descriptions.

********************************************************************************************/

void ColourContextRGBT::CreateExternalTransform()
{
#ifndef NO_XARACMS
	CMSColourSpace cmsSpace;
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// first read our internal colour calibration space
		lpCMSMan->GetLogicalProfile(&cmsSpace);

		// set this as the active colour profile
		UINT32 err = lpCMSMan->SetProfile(cmsSpace);

		// if we haven't got an error, create that transform
		if (err==0)
			CMSTransform = lpCMSMan->CreateTransform(DISPLAY_RCS);
	}
#endif
}


/********************************************************************************************

>	void ColourContextRGBT::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - A Colour defined in this RGBT context
				Result - pointer to a structure to recieve the resulting colour as
				defined in CIE space
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour from our RGBT colourspace to CIET colourspace
				This conversion will include Gamma correction etc as defined by
				this colour context.
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextRGBT::ConvertFromCIET

********************************************************************************************/

void ColourContextRGBT::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	ColourRGBT *bob = (ColourRGBT *) Source;

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;
		icol.c0 = bob->Red.MakeDouble();
		icol.c1 = bob->Green.MakeDouble();
		icol.c2 = bob->Blue.MakeDouble();

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvRGBtoCIEXYZ(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsForward, icol, ocol);

		Result->X = ocol.c0;
		Result->Y = ocol.c1;
		Result->Z = ocol.c2;
	}
	else
#endif
	{
		// The old rgb method
		Result->X = bob->Red.MakeDouble();
		Result->Y = bob->Green.MakeDouble();
		Result->Z = bob->Blue.MakeDouble();
	}

	Result->Transparent = 0.0;//bob->Transparent;
}




/********************************************************************************************

>	void ColourContextRGBT::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - A Colour defined in the CIET colourspace
				Result - pointer to a structure to recieve the resulting colour as
				defined in the RGBT space described by this colour context.
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour to our RGBT colourspace from CIET colourspace
				This conversion will include Gamma correction etc as defined by
				this colour context.
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextRGBT::ConvertToCIET

********************************************************************************************/

void ColourContextRGBT::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	ColourRGBT *bob = (ColourRGBT *) Result;

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;
		icol.c0	= Source->X;
		icol.c1	= Source->Y;
		icol.c2	= Source->Z;

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvCIEXYZtoRGB(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsReverse, icol, ocol);

		bob->Red	= ocol.c0;
		bob->Green	= ocol.c1;
		bob->Blue	= ocol.c2;
	}
	else
#endif
	{
		// The old rgb method
		bob->Red	= Source->X;
		bob->Green	= Source->Y; 
		bob->Blue	= Source->Z;
	}

	bob->Transparent = 0;//Source->Transparent;
}



/********************************************************************************************

>	virtual BOOL ColourContextRGBT::IsDifferent(ColourContext *Other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/94
	Inputs:		Other - a colour context to comapre ourselves to
	Outputs:	-
	Returns:	TRUE if the other colour context object is different from
				ourself, or TRUE if it is an exactly equivalent context.
	Purpose:	Determine if two colour contexts are not exactly equivalent
	Errors:		-
	SeeAlso:	ColourContext::IsDifferent

********************************************************************************************/


BOOL ColourContextRGBT::IsDifferent(ColourContext *Other) const
{
	if(ColourContext::IsDifferent(Other))	// Different on generic ColourContext basis
		return(TRUE);

	// These must both be RGBT contexts, so check if they are equivalent
	return(Gamma != ((ColourContextRGBT *)Other)->Gamma);
}



/********************************************************************************************

>	void ColourContextRGBT::GetModelName(StringBase *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		-
	Outputs:	Result returns continaing a String of the name of the context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'RGB' 'HSV' 'Greyscale')
				so allow at least 10 chars of space in your displays as well)
	Returns:	-
	Purpose:	Returns the name of this context's colour model

********************************************************************************************/

void ColourContextRGBT::GetModelName(StringBase *Result)
{
	ENSURE(Result != NULL, "ColourContext::GetModelName called with NULL result pointer!");

	*Result = String_32(_R(IDS_COLMODEL_RGBT));
}



/********************************************************************************************

>	void ColourContextWebRGBT::GetModelName(StringBase *Result)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	??/11/99
	Inputs:		-
	Outputs:	Result returns continaing a String of the name of the context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'RGB' 'HSV' 'Greyscale')
				so allow at least 10 chars of space in your displays as well)
	Returns:	-
	Purpose:	Returns the name of this context's colour model

********************************************************************************************/

void ColourContextWebRGBT::GetModelName(StringBase *Result)
{
	ENSURE(Result != NULL, "ColourContext::GetModelName called with NULL result pointer!");

	*Result = String_32(_R(IDS_COLMODEL_WEBRGBT));
}



/********************************************************************************************

>	BOOL ColourContextRGBT::GetComponentName(INT32 ComponentID, StringBase *Result,
											 BOOL LongName = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		ComponentID - Index [1..4] of the component you want the name of
				(Result is an output)
				LongName - TRUE to get the long name, FALSE to get the short name
							(e.g. "Magenta" vs "M", "Saturation" vs "Sat")

	Outputs:	Result returns containing a String of the name of the given component
				of colours defined in this context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'Red' 'Hue' 'Saturation')
				so allow at least 10 chars of space in your displays as well)

				Note that Result can be passed in NULL in order to only determine if the
				context supports a given component.

	Returns:	FALSE if this component is not used by this model.
	Purpose:	Returns the name of one of this context's colour components

	Notes:		The components are given in the same order as they appear in the
				high-precision ColourXYZ structures defined in colmodel.h

********************************************************************************************/

BOOL ColourContextRGBT::GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName)
{
	UINT32 StringID = 0;
	BOOL Used = TRUE;

	switch(ComponentID)
	{
		case 1:
			StringID = (LongName) ? _R(IDS_COLCOMPL_RED) : _R(IDS_COLCOMP_RED);
			break;
			
		case 2:
			StringID = (LongName) ? _R(IDS_COLCOMPL_GREEN) : _R(IDS_COLCOMP_GREEN);
			break;

		case 3:
			StringID = (LongName) ? _R(IDS_COLCOMPL_BLUE) : _R(IDS_COLCOMP_BLUE);
			break;
#ifdef TRANSPARENTCOLOUR
		default:
			StringID = _R(IDS_COLCOMP_TRANS);
			break;
#else
		default:
			Used = FALSE;
			break;
#endif
	}

	if (Result != NULL && StringID != 0)
		*Result = String_32(StringID);

	return(Used);
}



/********************************************************************************************

>	virtual UnitGroup **ColourContextRGBT::GetComponentUnitGroups()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the UnitGroup array
	Purpose:	Provides an array of UnitGroups primarily for use by the base class 
				(ColourContext) members Set/GetComponentUnitGroup().
	Errors:		-
	See Also:	ColourContext::GetComponentUnitGroup()
				ColourContext::SetComponentUnitGroup()

********************************************************************************************/

UnitGroup **ColourContextRGBT::GetComponentUnitGroups()
{
	return m_pUnitGroupArray;
}


UnitGroup *ColourContextRGBT::m_pUnitGroupArray[] =
{
	&(StandardUnit::PercentGroup),
	&(StandardUnit::PercentGroup),
	&(StandardUnit::PercentGroup)
};



/********************************************************************************************

>	UINT32 ColourContextRGBT::GetComponentCount()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Returns:	The number of components
	Purpose:	Provides number of components in the colour context's colour model

********************************************************************************************/

UINT32 ColourContextRGBT::GetComponentCount()
{
	return MAX_COMPONENTS;
}


/********************************************************************************************

>	virtual void ColourContextRGBT::ApplyTint(ColourValue TintFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		TintFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be tinted. This value is expected to lie between 0.0 and 1.0 inclusive

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is tinted by the given TintFraction
				If TintFraction <= 0.0, White is output
				If TintFraction >= 1.0, The source colour is output

	Returns:	-
	Purpose:	Tints a colour (mixes it with white)

	Notes:		If the tinting value is out of range, either source-colour ow white (as
				appropriate) will be output.

********************************************************************************************/

void ColourContextRGBT::ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult)
{
	ColourRGBT *Result = (ColourRGBT *) SourceAndResult;
	
	if (TintFraction <= FIXED24(0.0))		// 0% tint = White
	{
		Result->Red = Result->Green = Result->Blue = FIXED24(1.0);
		return;
	}

	if (TintFraction >= FIXED24(1.0))		// The Result colour is identical to the source
		return;

	// Otherwise, tint the colour...
	Result->Red   = FIXED24(1.0) - (TintFraction - (Result->Red   * TintFraction));
	Result->Green = FIXED24(1.0) - (TintFraction - (Result->Green * TintFraction));
	Result->Blue  = FIXED24(1.0) - (TintFraction - (Result->Blue  * TintFraction));
}


/********************************************************************************************

>	void ColourContextRGBT::ApplyShade(ColourValue XFraction, ColourValue YFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		XFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be saturated. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean tint towards 0, while values above mean tint towards 1.
				0.0 means no tinting

				YFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be shaded. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean shade towards 0, while values above mean shade towards 1.
				0.0 means no shading

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is shaded by the given Fractions
				Any invalid value is clipped to 0.0

	Returns:	-
	Purpose:	Shades a colour (tweaks the saturation and value in a relative way)

	Notes:		Contexts other than HSV currently convert the colour to and from HSV
				form in order to apply the shade. This is a tad inefficient, but a
				quick'n'easy bodge that will get it going nice and quickly.

********************************************************************************************/

void ColourContextRGBT::ApplyShade(ColourValue XFraction, ColourValue YFraction,
									ColourGeneric *SourceAndResult)
{
	ColourHSVT HSVDef;
	ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);
	ERROR3IF(cc == NULL, "No default HSV colour context?!");

	cc->ConvertColour(this, SourceAndResult, (ColourGeneric *) &HSVDef);
	cc->ApplyShade(XFraction, YFraction,  (ColourGeneric *) &HSVDef);
	ConvertColour(cc, (ColourGeneric *) &HSVDef, SourceAndResult);
}



/********************************************************************************************

>	virtual void ColourContextRGBT::ApplyOutputFilter(ColourPlate *FilterDescription,
													ColourContext *SourceContext,
													ColourGeneric *OutputColour,
													IndexedColour *SourceIxCol)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		FilterDescription - A ColourPlate describing how the colour is to be filtered
	
				SourceContext - Points to the context from which the OutputColour has just
				been converted. 

				OutputColour - The colour to be filtered

				SourceIxCol - NULL, or the IndexedColour which was the source of the colour
				being converted. This is only used for information when preparing spot colour
				separations.

	Outputs:	OutputColour - Will be modified as appropriate to the current colour
				filtering options (see Purpose)

	Purpose:	All colour conversions call this function immediately prior to returning
				the converted colour to the caller. This gives the output colour context
				the chance to apply an output filtering process to the output colour.

				This is usually used to provide colour separations, and may also be used
				to apply some colour corrections. Often, this function will do nothing
				to the output colour.

	SeeAlso:	ColourContext::ConvertColourBase

********************************************************************************************/

void ColourContextRGBT::ApplyOutputFilter(ColourPlate *FilterDescription, ColourContext *SourceContext,
											ColourGeneric *OutputColour, IndexedColour *SourceIxCol)
{
	ERROR3IF(FilterDescription == NULL || SourceContext == NULL || OutputColour == NULL, "Illegal NULL params");

	// No FilterDescription means no filtering, so we return immeidately
	// If the source colour was CMYK, we mustn't filter it, except when doing a composite preview, as
	// the filtering (separation) of CMYK all happens in ApplyInputFilter
	if (FilterDescription == NULL ||
		(SourceContext->GetColourModel() == COLOURMODEL_CMYK &&
		 FilterDescription->GetType() != COLOURPLATE_COMPOSITE))
	{
		return;
	}

	ColourRGBT *OutputRGB = (ColourRGBT *)OutputColour;
	FIXED24 temp;


/*-----------------------------------------------------------------------------------------------
	Local Macro GETCOLOURINCMYK(ColourCMYK RESULT)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Date:		3/7/96
	Inputs:		RESULT - will be filled in with a CMYK definition for OutputRGB
	Purpose:	Converts our OutputRGB variable into a CMYK form.

	Notes:		This only works in the scope in which it is used below. It is merely to save
				writing the code out longhand, and to make the repeated section of the code below
				more readable.

				Special care is taken with the conversion - We try to find a CMYK context from
				our ScopeView, so that we use the correct colour-separation profile for the current
				output device. We also do the conversion manually (to & from CIE space) rather
				than just calling ConvertColour because (a) it's more efficient, and (b) we don't
				want the CMYK context to do anything like colour separation on the colour!

				This macro ius undefined at the end of the switch statement which uses it

  -----------------------------------------------------------------------------------------------
*/
#define GETCOLOURINCMYK(RESULT)																	\
	{																							\
		ColourContextCMYK *ccCMYK;																\
		if (ScopeView != NULL)																	\
			ccCMYK = (ColourContextCMYK *) ScopeView->GetColourContext(COLOURMODEL_CMYK);		\
		else																					\
			ccCMYK = (ColourContextCMYK *) ColourManager::GetColourContext(COLOURMODEL_CMYK);	\
																								\
		DColourCIET IntermediateResult;															\
		ConvertToCIET(OutputColour, &IntermediateResult);										\
		ccCMYK->ConvertFromCIET(&IntermediateResult, (ColourGeneric *)&(RESULT));				\
	}
//-----------------------------------------------------------------------------------------------


	// Separate the colour as appropriate
	switch(FilterDescription->GetType())
	{
		case COLOURPLATE_CYAN:
			{
				ColourCMYK cmyk;								// Get OutputRGB as defined in cmyk
				GETCOLOURINCMYK(cmyk);

				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputRGB->Red = cmyk.Cyan;
				else
					OutputRGB->Red = 1.0 - cmyk.Cyan;

				if (FilterDescription->IsMonochrome())			// Make the plate a greyscale if necessary
					OutputRGB->Green = OutputRGB->Blue = OutputRGB->Red;
				else
					OutputRGB->Green = OutputRGB->Blue = 1.0;
			}
			break;

		case COLOURPLATE_MAGENTA:
			{
				ColourCMYK cmyk;								// Get OutputRGB as defined in cmyk
				GETCOLOURINCMYK(cmyk);

				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputRGB->Green = cmyk.Magenta;
				else
					OutputRGB->Green = 1.0 - cmyk.Magenta;

				if (FilterDescription->IsMonochrome())			// Make the plate a greyscale if necessary
					OutputRGB->Red = OutputRGB->Blue = OutputRGB->Green;
				else
					OutputRGB->Red = OutputRGB->Blue = 1.0;
			}
			break;

		case COLOURPLATE_YELLOW:
			{
				ColourCMYK cmyk;								// Get OutputRGB as defined in cmyk
				GETCOLOURINCMYK(cmyk);

				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputRGB->Blue = cmyk.Yellow;
				else
					OutputRGB->Blue = 1.0 - cmyk.Yellow;

				if (FilterDescription->IsMonochrome())			// Make the plate a greyscale if necessary
					OutputRGB->Green = OutputRGB->Red = OutputRGB->Blue;
				else
					OutputRGB->Green = OutputRGB->Red = 1.0;
			}
			break;

		case COLOURPLATE_KEY:
			{
				ColourCMYK cmyk;								// Get OutputRGB as defined in cmyk
				GETCOLOURINCMYK(cmyk);

				// The FilterDescription->IsMonochrome() flag has no effect on this plate!

				if (FilterDescription->IsNegative())
					OutputRGB->Red = OutputRGB->Green = OutputRGB->Blue = cmyk.Key;
				else
					OutputRGB->Red = OutputRGB->Green = OutputRGB->Blue = 1.0 - cmyk.Key;
			}
			break;

		case COLOURPLATE_SPOT:
			// NOTE:
			// Spot colours are partially handled by the IndexedColour variants of
			// ConvertColour, so will be separated appropriately by the time they
			// reach us here. However, non-IndexedColours will not be, so we must
			// sit on them to make them white, so that DocColour and GenericColour
			// conversions are eliminated properly from spot plates.
			// However, if a spot colour gets this far, we need to generate an ink
			// density value for output, as well as applying the negate flag.
			//
			// Actually, I have decreed that all spot plates will always be monochrome,
			// because that makes coding DocColour::Mix much easier, and anyway it makes
			// a load more sense for the very good reason that if you don't believe me
			// I'll get my Dad to come around and beat up your dad.

			if (SourceIxCol == NULL)
			{
				// It's not an IndexedColour, so can't appear on this plate - make it white
				OutputRGB->Red = OutputRGB->Green = OutputRGB->Blue = 1.0;
			}
			else
			{
				// We have a source IndexedColour, so we know it's been separated.
				// We just have to get a decent ink density value out of the IxCol
				if (SourceIxCol == FilterDescription->GetSpotColour())
				{
					// The spot colour itself is always considered a 100% tint (100% ink)
					OutputRGB->Red = OutputRGB->Green = OutputRGB->Blue = 0.0;
				}
				else if (SourceIxCol->IsADescendantOf(FilterDescription->GetSpotColour()))
				{
					// It's not the spot colour, but it is a child - check that it's a tint
					ERROR3IF(SourceIxCol->GetType() != COLOURTYPE_TINT ||
							 SourceIxCol->TintIsShade(),
							 "Spot colour children must all be tints!");

					// Tint gives the amount of ink - we have to inverse this in RGB
					// to get the appropriate grey level. Note that we get the accumulated
					// tint value rather than the simple tint, which is the true output ink
					// density.
					OutputRGB->Red = OutputRGB->Green = OutputRGB->Blue =
											1.0 - SourceIxCol->GetAccumulatedTintValue();
				}
			}

			// If negative, then photographically invert the colour
			if (FilterDescription->IsNegative())
			{
				OutputRGB->Red   = 1.0 - OutputRGB->Red;
				OutputRGB->Green = 1.0 - OutputRGB->Green;
				OutputRGB->Blue  = 1.0 - OutputRGB->Blue;
			}
			break;

		case COLOURPLATE_COMPOSITE:
#ifndef NO_XARACMS
			// Go to and from (printer) cmyk just for a laugh.
			// (NOTE that ConvertColourBase won't actually call us in this case any more)
			if (SourceIxCol != NULL && SourceIxCol->GetColourModel() == COLOURMODEL_CMYK)
			{
				// If it's a CMYK colour, then it's already in printer gamut, so we only apply
				// the backward colour correction factor to it.
				// Note that this means we chuck away the entire colour conversion result!
				XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
				if (lpCMSMan != NULL)
				{
					ColourCMYK Def;
					SourceIxCol->GetSourceColour((ColourGeneric *) &Def);

					lpCMSMan->ConvertColourForPaperView(&Def, OutputRGB);
				}
			}
			else
			{
				// If it's a non-CMYK colour, then we go to and from printer gamut just for a laugh
				// Find the CMS manager, and get it to do all the work
				XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
				if (lpCMSMan != NULL)
					lpCMSMan->ConvertColourForPaperView(OutputRGB);
			}
#endif
			// ... drop through to the default case to apply monochrome/negate

		default:
			// If monochrome, convert the image to a greyscale
			if (FilterDescription->IsMonochrome())
			{
				FIXED24 temp = (OutputRGB->Red * 0.305) + (OutputRGB->Green * 0.586) + (OutputRGB->Blue * 0.109);
				OutputRGB->Red = OutputRGB->Green = OutputRGB->Blue = temp;
			}

			// If negative, then photographically invert the colour
			if (FilterDescription->IsNegative())
			{
				OutputRGB->Red   = 1.0 - OutputRGB->Red;
				OutputRGB->Green = 1.0 - OutputRGB->Green;
				OutputRGB->Blue  = 1.0 - OutputRGB->Blue;
			}
			break;
	}

// And un-define our macro again to make sure it's not used out of its scope
#undef GETCOLOURINCMYK
}



/********************************************************************************************

>	virtual void ColourContextRGBT::GetWhite(ColourGeneric *Result);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/96

	Outputs:	Result - Will be filled in with White, however that is defined in
				this context's colour model. (i.e. RGB(1,1,1) or CMYK(0,0,0,0), etc)

	Purpose:	An easy way to get a colour definition in this colour model which
				is white (paper colour).

********************************************************************************************/

void ColourContextRGBT::GetWhite(ColourGeneric *Result)
{
	ColourRGBT *rgb = (ColourRGBT *) Result;
	rgb->Red = rgb->Green = rgb->Blue = 1.0;
	rgb->Transparent = 0;
}










/********************************************************************************************

>	void ColourContextCMYK::ColourContextCMYK(View *Scope)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Scope - The view in which this context is to be used, or NULL
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a "logical" CMYK Colour context

				A logical context represents an "ideal" CMYK printer. This is used
				internally when converting colours between logical colour models,
				and only used externally when no printer device profile is available.

	Notes:		Colour Contexts should not be created and used directly. See the
				notes in the SeeAlso's for instructions on proper care and use.

	Errors:		-
	SeeAlso:	ColourContext::ColourContext; ColourContextList::AddContext

********************************************************************************************/

ColourContextCMYK::ColourContextCMYK(View *Scope)
				  : ColourContext(Scope)
{
	ColModel = COLOURMODEL_CMYK;
	CreateExternalTransform();
}



/********************************************************************************************

>	void ColourContextCMYK::ColourContextCMYK(View *Scope, StringBase *ProfileName)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		Scope - The view in which this context is to be used, or NULL
				ProfileName - A string indicating the XaraCMS device profile which
				describes the printer device this context represents
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for a CMYK Colour context

	Notes:		Colour Contexts should not be created and used directly. See the
				notes in the SeeAlso's for instructions on proper care and use.

				XaraCMS::GetPrinterProfile() can be used to find the default printer
				profile name.

	Errors:		-
	SeeAlso:	ColourContext::ColourContext; ColourContextList::AddContext

********************************************************************************************/

ColourContextCMYK::ColourContextCMYK(View *Scope, StringBase *NewProfileName)
				  : ColourContext(Scope)
{
	ColModel = COLOURMODEL_CMYK;

	if (NewProfileName == NULL)
	{
		ERROR3("Illegal NULL param");
		ProfileName.Empty();
	}
	else
		ProfileName = *NewProfileName;

	CreateExternalTransform();
}



/********************************************************************************************

>	void ColourContextCMYK::CreateExternalTransform(void)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/96
	Inputs:		-
	Returns:	-
	Purpose:	Create an external transform to be used during colour conversions from
				CMYK to the current RCS space. This function uses the Winoil CMS Manager
				to create the transform. Both forward and backward transforms use a physical
				printer profile description.

********************************************************************************************/

void ColourContextCMYK::CreateExternalTransform()
{
#ifndef NO_XARACMS
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	// If we have a CMS and we are not a logical context, set up a transform
	if (lpCMSMan != NULL && !ProfileName.IsEmpty())
	{
		TCHAR* pProfile = (TCHAR*)ProfileName;

		// set this as the active colour profile
		UINT32 err = lpCMSMan->SetProfile(ptPrinter, pProfile);

		// if we haven't got an error, create that transform
		if (err==0)
			CMSTransform = lpCMSMan->CreateTransform(PRINT_RCS);
	}
#endif
}



/********************************************************************************************

>	void ColourContextCMYK::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		Source - A Colour defined in this CMYK context
				Result - pointer to a structure to recieve the resulting colour as
				defined in CIE space
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour from our CMYK colourspace to CIET colourspace

	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextCMYK::ConvertFromCIET

********************************************************************************************/

void ColourContextCMYK::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	ColourCMYK *bob = (ColourCMYK *) Source;

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;
		icol.c0 = bob->Cyan.MakeDouble();
		icol.c1 = bob->Magenta.MakeDouble();
		icol.c2 = bob->Yellow.MakeDouble();
		icol.c3 = bob->Key.MakeDouble();

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvCMYKtoCIEXYZ(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsForward, icol, ocol);

		Result->X = ocol.c0;
		Result->Y = ocol.c1;
		Result->Z = ocol.c2;
	}
	else
#endif
	{
		// The old rgb method
		Result->X = ( 1.0 - ( min( 1.0, ( bob->Cyan    + bob->Key ).MakeDouble() ) ) );	// R
		Result->Y = ( 1.0 - ( min( 1.0, ( bob->Magenta + bob->Key ).MakeDouble() ) ) );	// G
		Result->Z = ( 1.0 - ( min( 1.0, ( bob->Yellow  + bob->Key ).MakeDouble() ) ) );	// B
	}

	Result->Transparent = 0.0;				// CMYK values are not transparent
}




/********************************************************************************************

>	void ColourContextCMYK::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Source - A Colour defined in the CIET colourspace
				Result - pointer to a structure to recieve the resulting colour as
				defined in the CMYK space described by this colour context.
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour to our CMYK colourspace from CIET colourspace
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextCMYK::ConvertToCIET

********************************************************************************************/

void ColourContextCMYK::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	ColourCMYK *cmyk = (ColourCMYK *) Result;

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;
		icol.c0	= Source->X;
		icol.c1	= Source->Y;
		icol.c2	= Source->Z;

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvCIEXYZtoCMYK(icol, ocol);
		else			
			lpCMSMan->GTransform(CMSTransform, cmsReverse, icol, ocol);
		
		cmyk->Cyan	 = FIXED24(ocol.c0);
		cmyk->Magenta= FIXED24(ocol.c1);
		cmyk->Yellow = FIXED24(ocol.c2);
		cmyk->Key	 = FIXED24(ocol.c3);

/*
#ifdef _DEBUG
		{
		// Check that the colour will round-trip from CIE to CMYK and back to CIE correctly
			DColourCIET temp;
			ConvertToCIET(Result, &temp);
			INT32 t1 = (INT32)(Source->X*1000.0);
			INT32 t2 = (INT32)(Source->Y*1000.0);
			INT32 t3 = (INT32)(Source->Z*1000.0);
			INT32 t4 = (INT32)INT32(temp.X*1000.0);
			INT32 t5 = (INT32)INT32(temp.Y*1000.0);
			INT32 t6 = (INT32)INT32(temp.Z*1000.0);
			CMSColour orgb, orgb2;
			lpCMSMan->ConvCIEXYZtoRGB(icol, orgb);
			if (t1==t4 && t2==t5 && t3==t6)
			{
				TRACE( _T("CIE-CMYK-CIE roundtrip OK   : RGB(%.3f,%.3f,%.3f) (%.3f,%.3f,%.3f,%.3f)\n"), orgb.c0, orgb.c1, orgb.c2, ocol.c0, ocol.c1, ocol.c2, ocol.c3);
			}
			else
			{
				TRACE( _T("CIE-CMYK-CIE roundtrip error: RGB(%.3f,%.3f,%.3f) (%.3f,%.3f,%.3f,%.3f)\n"), orgb.c0, orgb.c1, orgb.c2, ocol.c0, ocol.c1, ocol.c2, ocol.c3);
				TRACE( _T("                              CIEIn[%d,%d,%d] CIEOut[%d,%d,%d]\n"), t1, t2, t3, t4, t5, t6);
			}

//			ERROR3IF((INT32)(temp.X*1000.0)!=(INT32)(Source->X*1000.0) ||
//					(INT32)(temp.Y*1000.0)!=(INT32)(Source->Y*1000.0) ||
//					(INT32)(temp.Z*1000.0)!=(INT32)(Source->Z*1000.0),
//					"CMYK does not round-trip!");
		}
#endif
*/

	}
	else
#endif
	{
		// The old rgb method
		ColourRGBT rgb;
		rgb.Red   = FIXED24(Source->X);
		rgb.Green = FIXED24(Source->Y);
		rgb.Blue  = FIXED24(Source->Z);

		// If all components are 0 then we know it's BLACK.
		if (rgb.Red <= 0 && rgb.Green <= 0 && rgb.Blue <= 0)
		{
			cmyk->Cyan = cmyk->Magenta = cmyk->Yellow = 0;
			cmyk->Key  = 1.0;
		}
		else
		{
			// Invert the Colour Vector
			cmyk->Cyan    = 1.0 - rgb.Red;
			cmyk->Magenta = 1.0 - rgb.Green;
			cmyk->Yellow  = 1.0 - rgb.Blue;
			
			double smallest;
			
			smallest = min(cmyk->Cyan.MakeDouble(), cmyk->Magenta.MakeDouble());	
			smallest = min(smallest, cmyk->Yellow.MakeDouble());
			smallest = max(smallest-0.5, 0.0);

			// Key starts being generated beyond threshold
			cmyk->Key = smallest;

			// Subtract KEY component.
			cmyk->Cyan    -= cmyk->Key;
			cmyk->Magenta -= cmyk->Key;
			cmyk->Yellow  -= cmyk->Key;
		}
	}
}



/********************************************************************************************

>	BOOL ColourContextCMYK::GetProfileTables(BYTE* Tables)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		Tables = a byte pointer to a 5*256 byte buffer
	Outputs:	TRUE if the Tables buffer has been filled
				FALSE otherwise.
	Purpose:	Uses the CMSManager to extract the profile data from our current CMSTransform

********************************************************************************************/

BOOL ColourContextCMYK::GetProfileTables(BYTE* Tables)
{
PORTNOTE("other","Removed HCMTRANSFORM usage")
#ifndef EXCLUDE_FROM_XARALX
	if (CMSTransform==NULL)
		return FALSE;
#endif

#ifdef NO_XARACMS
	return FALSE;
#else
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
	if (lpCMSMan==NULL)
		return FALSE;

	return lpCMSMan->GetProfileTables(CMSTransform, Tables);
#endif
}



/********************************************************************************************

>	void ColourContextCMYK::GetModelName(StringBase *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		-
	Outputs:	Result returns continaing a String of the name of the context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'RGB' 'HSV' 'Greyscale')
				so allow at least 10 chars of space in your displays as well)
	Returns:	-
	Purpose:	Returns the name of this context's colour model
	Notes:		

********************************************************************************************/

void ColourContextCMYK::GetModelName(StringBase *Result)
{
	ENSURE(Result != NULL, "ColourContext::GetModelName called with NULL result pointer!");

	*Result = String_32(_R(IDS_COLMODEL_CMYK));
}



/********************************************************************************************

>	BOOL ColourContextCMYK::GetComponentName(INT32 ComponentID, StringBase *Result,
											 BOOL LongName = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		ComponentID - Index [1..4] of the component you want the name of
				(Result is an output)
				LongName - TRUE to get the long name, FALSE to get the short name
							(e.g. "Magenta" vs "M", "Saturation" vs "Sat")

	Outputs:	Result returns containing a String of the name of the given component
				of colours defined in this context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'Red' 'Hue' 'Saturation')
				so allow at least 10 chars of space in your displays as well)

				Note that Result can be passed in NULL in order to only determine if the
				context supports a given component.

	Returns:	FALSE if this component is not used by this model.
	Purpose:	Returns the name of one of this context's colour components

	Notes:		The components are given in the same order as they appear in the
				high-precision ColourXYZ structures defined in colmodel.h

********************************************************************************************/

BOOL ColourContextCMYK::GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName)
{
	UINT32 StringID = 0;
	BOOL Used = TRUE;

	switch(ComponentID)
	{
		case 1:
			StringID = (LongName) ? _R(IDS_COLCOMPL_CYAN) : _R(IDS_COLCOMP_CYAN);
			break;
			
		case 2:
			StringID = (LongName) ? _R(IDS_COLCOMPL_MAGENTA) : _R(IDS_COLCOMP_MAGENTA);
			break;

		case 3:
			StringID = (LongName) ? _R(IDS_COLCOMPL_YELLOW) : _R(IDS_COLCOMP_YELLOW);
			break;

		default:
			StringID = (LongName) ? _R(IDS_COLCOMPL_KEY) : _R(IDS_COLCOMP_KEY);
			break;
	}

	if (Result != NULL && StringID != 0)
		*Result = String_32(StringID);

	return(Used);
}


/********************************************************************************************

>	virtual UnitGroup **ColourContextCMYK::GetComponentUnitGroups()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the UnitGroup array
	Purpose:	Provides an array of UnitGroups primarily for use by the base class 
				(ColourContext) members Set/GetComponentUnitGroup().
	Errors:		-
	See Also:	ColourContext::GetComponentUnitGroup()
				ColourContext::SetComponentUnitGroup()

********************************************************************************************/

UnitGroup **ColourContextCMYK::GetComponentUnitGroups()
{
	return m_pUnitGroupArray;
}


UnitGroup* ColourContextCMYK::m_pUnitGroupArray[] =
{
	&(StandardUnit::PercentGroup),
	&(StandardUnit::PercentGroup),
	&(StandardUnit::PercentGroup),
	&(StandardUnit::PercentGroup)
};



/********************************************************************************************

>	UINT32 ColourContextCMYK::GetComponentCount()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Returns:	The number of components
	Purpose:	Provides number of components in the colour context's colour model

********************************************************************************************/

UINT32 ColourContextCMYK::GetComponentCount()
{
	return MAX_COMPONENTS;
}



/********************************************************************************************

>	virtual void ColourContextCMYK::ApplyTint(ColourValue TintFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		TintFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be tinted. This value is expected to lie between 0.0 and 1.0 inclusive

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is tinted by the given TintFraction
				If TintFraction <= 0.0, White is output
				If TintFraction >= 1.0, The source colour is output

	Returns:	-
	Purpose:	Tints a colour (mixes it with white)

	Notes:		If the tinting value is out of range, either source-colour ow white (as
				appropriate) will be output.

********************************************************************************************/

void ColourContextCMYK::ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult)
{
	ColourCMYK *Result = (ColourCMYK *) SourceAndResult;
	
	if (TintFraction <= FIXED24(0.0))		// 0% tint = White
	{
		Result->Cyan = Result->Magenta = Result->Yellow = Result->Key = 0;
		return;
	}

	if (TintFraction >= FIXED24(1.0))		// The Result colour is identical to the source
		return;

	// Otherwise, tint the colour...
	Result->Cyan    = (Result->Cyan    * TintFraction);
	Result->Magenta = (Result->Magenta * TintFraction);
	Result->Yellow  = (Result->Yellow  * TintFraction);
	Result->Key     = (Result->Key     * TintFraction);
}



/********************************************************************************************

>	void ColourContextCMYK::ApplyShade(ColourValue XFraction, ColourValue YFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		XFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be saturated. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean tint towards 0, while values above mean tint towards 1.
				0.0 means no tinting

				YFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be shaded. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean shade towards 0, while values above mean shade towards 1.
				0.0 means no shading

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is shaded by the given Fractions
				Any invalid value is clipped to 0.0

	Returns:	-
	Purpose:	Shades a colour (tweaks the saturation and value in a relative way)

	Notes:		Contexts other than HSV currently convert the colour to and from HSV
				form in order to apply the shade. This is a tad inefficient, but a
				quick'n'easy bodge that will get it going nice and quickly.

********************************************************************************************/

void ColourContextCMYK::ApplyShade(ColourValue XFraction, ColourValue YFraction,
									ColourGeneric *SourceAndResult)
{
	ColourHSVT HSVDef;
	ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);
	ERROR3IF(cc == NULL, "No default HSV colour context?!");

	cc->ConvertColour(this, SourceAndResult, (ColourGeneric *) &HSVDef);
	cc->ApplyShade(XFraction, YFraction,  (ColourGeneric *) &HSVDef);
	ConvertColour(cc, (ColourGeneric *) &HSVDef, SourceAndResult);
}



/********************************************************************************************

>	virtual void ColourContextCMYK::ApplyInputFilter(ColourPlate *FilterDescription,
													ColourContext *DestContext,
													ColourGeneric *OutputColour,
													IndexedColour *SourceIxCol);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		FilterDescription - A ColourPlate describing how the colour is to be filtered
	
				DestContext - Points to the context to which the OutputColour will be
				converted after we return it (in case this information is useful)

				OutputColour - The colour to be filtered

				SourceIxCol - NULL, or the original IndexedColour which is being converted.
				This is used for handling spot colour separation correctly.

	Outputs:	OutputColour - Will be modified as appropriate to the current colour
				filtering options (see Purpose)

	Purpose:	All colour conversions call this function for the SOURCE context
				immediately before converting the colour. This gives the input colour
				context the chance to apply a filtering process to the colour.

				This is usually used to provide colour separations. Often, this function
				will do nothing to the output colour - generally it is only used by CMYK
				contexts in order to colour-separate the colour in CMYK space.

	SeeAlso:	ColourContext::ConvertColourBase

********************************************************************************************/

void ColourContextCMYK::ApplyInputFilter(ColourPlate *FilterDescription, ColourContext *DestContext,
											ColourGeneric *OutputColour, IndexedColour *SourceIxCol)
{
	ERROR3IF(FilterDescription == NULL || DestContext == NULL || OutputColour == NULL, "Illegal NULL params");

	if (FilterDescription == NULL)
		return;

	ColourCMYK *OutputCMYK = (ColourCMYK *)OutputColour;
	FIXED24 temp;

	// Separate the colour as appropriate
	switch(FilterDescription->GetType())
	{
		case COLOURPLATE_CYAN:
			{
				if (FilterDescription->IsNegative())		// Negate the plate if necessary
					OutputCMYK->Cyan = 1.0 - OutputCMYK->Cyan;

				if (FilterDescription->IsMonochrome())		// Make the plate a greyscale if necessary
				{
					OutputCMYK->Key = OutputCMYK->Cyan;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else
					OutputCMYK->Key = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
			}
			break;

		case COLOURPLATE_MAGENTA:
			{
				if (FilterDescription->IsNegative())		// Negate the plate if necessary
					OutputCMYK->Magenta = 1.0 - OutputCMYK->Magenta;

				if (FilterDescription->IsMonochrome())		// Make the plate a greyscale if necessary
				{
					OutputCMYK->Key = OutputCMYK->Magenta;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else
					OutputCMYK->Key = OutputCMYK->Cyan = OutputCMYK->Yellow = 0.0;
			}
			break;

		case COLOURPLATE_YELLOW:
			{
				if (FilterDescription->IsNegative())		// Negate the plate if necessary
					OutputCMYK->Yellow = 1.0 - OutputCMYK->Yellow;

				if (FilterDescription->IsMonochrome())		// Make the plate a greyscale if necessary
				{
					OutputCMYK->Key = OutputCMYK->Yellow;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else
					OutputCMYK->Key = OutputCMYK->Magenta = OutputCMYK->Cyan = 0.0;
			}
			break;

		case COLOURPLATE_KEY:
			{
				OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;

				// The FilterDescription->IsMonochrome() flag has no effect on this plate!

				if (FilterDescription->IsNegative())
					OutputCMYK->Key = 1.0 - OutputCMYK->Key;
			}
			break;

			case COLOURPLATE_SPOT:
				// NOTE:
				// Real spot colours are generally sorted out by the higher level ConvertColour
				// calls. However, 2 cases may seep through to this level:
				// 1)	Colours which are not indexed colours (which can't be spots, so are always
				//		just converted to white)
				// 2)	Spot colours which *do* appear on this plate must be made into monochrome
				//		ink density values (which we put into the Key component).

				// In all cases, we're going monochrome in the Key only, so clear C/M/Y now.
				OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0;
				if (SourceIxCol == NULL)
				{
					// It's not an IndexedColour, so can't appear on this plate - make it white
					OutputCMYK->Key = 0;
				}
				else
				{
					// We have a source IndexedColour, so we know it's been separated.
					// We just have to get a decent ink density value out of the IxCol
					if (SourceIxCol == FilterDescription->GetSpotColour())
					{
						// The spot colour itself is always considered a 100% tint (100% ink)
						OutputCMYK->Key = 1.0;
					}
					else if (SourceIxCol->IsADescendantOf(FilterDescription->GetSpotColour()))
					{
						// It's not the spot colour, but it is a child - check that it's a tint
						ERROR3IF(SourceIxCol->GetType() != COLOURTYPE_TINT ||
								 SourceIxCol->TintIsShade(),
								 "Spot colour children must all be tints!");

						// Tint gives the amount of ink, which we stick into the Key plate.
						// Note that we get the accumulated tint value rather than the simple
						// tint, which is the true output ink density.
						OutputCMYK->Key = SourceIxCol->GetAccumulatedTintValue();
					}
				}

				// If negative, then photographically invert the colour
				if (FilterDescription->IsNegative())
				{
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow	= 1.0;
					OutputCMYK->Key  = 1.0 - OutputCMYK->Key;
				}
				break;

//			case COLOURPLATE_COMPOSITE:
//				-- We don't need to do anything here
//				break;

			default:
				// If monochrome, convert the image to a greyscale
				if (FilterDescription->IsMonochrome())
				{
					// It is extremely unlikely that anyone will ever need to do this.
					// (If you want a monochrome preview, you should really use a greyscale ColourContext)
					ERROR3("Unimplemented: Filtering of composite CMYK colour to monochrome!");
				}

				// If negative, then photographically invert the colour
				if (FilterDescription->IsNegative())
				{
					OutputCMYK->Cyan	= 1.0 - OutputCMYK->Cyan;
					OutputCMYK->Magenta	= 1.0 - OutputCMYK->Magenta;
					OutputCMYK->Yellow	= 1.0 - OutputCMYK->Yellow;
					OutputCMYK->Key		= 1.0 - OutputCMYK->Key;
				}
				break;
	}	
}



/********************************************************************************************

>	virtual void ColourContextCMYK::ApplyOutputFilter(ColourPlate *FilterDescription,
													ColourContext *SourceContext,
													ColourGeneric *OutputColour,
													IndexedColour *SourceIxCol)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		FilterDescription - A ColourPlate describing how the colour is to be filtered
	
				SourceContext - Points to the context from which the OutputColour has just
				been converted. 

				OutputColour - The colour to be filtered

				SourceIxCol - NULL, or the IndexedColour which was the source of the colour
				being converted. This is only used for information when preparing spot colour
				separations.

	Outputs:	OutputColour - Will be modified as appropriate to the current colour
				filtering options (see Purpose)

	Purpose:	All colour conversions call this function immediately prior to returning
				the converted colour to the caller. This gives the output colour context
				the chance to apply an output filtering process to the output colour.

				This is usually used to provide colour separations, and may also be used
				to apply some colour corrections. Often, this function will do nothing
				to the output colour.

	SeeAlso:	ColourContext::ConvertColourBase

********************************************************************************************/

void ColourContextCMYK::ApplyOutputFilter(ColourPlate *FilterDescription, ColourContext *SourceContext,
											ColourGeneric *OutputColour, IndexedColour *SourceIxCol)
{
	ERROR3IF(FilterDescription == NULL || SourceContext == NULL || OutputColour == NULL, "Illegal NULL params");

	// No FilterDescription means no filtering; If the source colour was CMYK, we msutn't filter it
	// as the ApplyInputFilter will have dealt with it
	if (FilterDescription == NULL || SourceContext->GetColourModel() == COLOURMODEL_CMYK)
		return;

	ColourCMYK *OutputCMYK = (ColourCMYK *)OutputColour;
	FIXED24 temp;

	// Separate the colour as appropriate
	switch(FilterDescription->GetType())
	{
		case COLOURPLATE_CYAN:
			{
				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputCMYK->Cyan = 1.0 - OutputCMYK->Cyan;

				if (FilterDescription->IsMonochrome())			// Make the plate a greyscale if necessary
				{
					// Move the Cyan value into the Key component
					OutputCMYK->Key = OutputCMYK->Cyan;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else
					OutputCMYK->Key = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
			}
			break;

		case COLOURPLATE_MAGENTA:
			{
				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputCMYK->Magenta = 1.0 - OutputCMYK->Magenta;

				if (FilterDescription->IsMonochrome())			// Make the plate a greyscale if necessary
				{
					// Move the Cyan value into the Key component
					OutputCMYK->Key = OutputCMYK->Magenta;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else
					OutputCMYK->Key = OutputCMYK->Cyan = OutputCMYK->Yellow = 0.0;
			}
			break;

		case COLOURPLATE_YELLOW:
			{
				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputCMYK->Yellow = 1.0 - OutputCMYK->Yellow;

				if (FilterDescription->IsMonochrome())			// Make the plate a greyscale if necessary
				{
					// Move the Cyan value into the Key component
					OutputCMYK->Key = OutputCMYK->Yellow;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else
					OutputCMYK->Key = OutputCMYK->Cyan = OutputCMYK->Magenta = 0.0;
			}
			break;

		case COLOURPLATE_KEY:
			{
				if (FilterDescription->IsNegative())			// Negate the plate if necessary
					OutputCMYK->Key = 1.0 - OutputCMYK->Key;

				// The monochrome flag has no effect on a key plate!
				OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
			}
			break;

		case COLOURPLATE_SPOT:
			// NOTE:
			// Spot colours are partially handled by the IndexedColour variants of
			// ConvertColour, so will never reach us here if they have been
			// eliminated from the plate.
			// However, if a spot colour gets this far, we need to generate an ink
			// density value for output, as well as applying the negate flag.
			//
			// Actually, I have decreed that all spot plates will always be monochrome,
			// because that makes coding DocColour::Mix much easier, and anyway it makes
			// a load more sense for the very good reason that if you don't believe me
			// I'll get my Dad to come around and beat up your dad.

			if (SourceIxCol != NULL) // && FilterDescription->IsMonochrome())
			{
				if (SourceIxCol == FilterDescription->GetSpotColour())
				{
					// The spot colour itself is always considered a 100% tint (100% ink)
					OutputCMYK->Key = 1.0;
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
				else if (SourceIxCol->IsADescendantOf(FilterDescription->GetSpotColour()))
				{
					// It's not the spot colour, but it is a child - check that it's a tint
					ERROR3IF(SourceIxCol->GetType() != COLOURTYPE_TINT ||
							 SourceIxCol->TintIsShade(),
							 "Spot colour children must all be tints!");

					// Tint gives the amount of ink. Note that we get the accumlated
					// tint value (true ink density) rather than the simple tint.
					OutputCMYK->Key = SourceIxCol->GetAccumulatedTintValue();
					OutputCMYK->Cyan = OutputCMYK->Magenta = OutputCMYK->Yellow = 0.0;
				}
			}

			// If negative, then photographically invert the colour
			if (FilterDescription->IsNegative())
			{
				OutputCMYK->Cyan	= 1.0 - OutputCMYK->Cyan;
				OutputCMYK->Magenta	= 1.0 - OutputCMYK->Magenta;
				OutputCMYK->Yellow	= 1.0 - OutputCMYK->Yellow;
				OutputCMYK->Key		= 1.0 - OutputCMYK->Key;
			}
			break;

		case COLOURPLATE_COMPOSITE:
#ifndef NO_XARACMS
			{
/*
				// Go to and from (printer) cmyk just for a laugh
				// Find the CMS manager, and get it to do all the work
				XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
				if (lpCMSMan != NULL)
					lpCMSMan->ConvertColourForPaperView(OutputRGB);
*/
				ERROR3("Composite preview to a CMYK colour context is barmy!");
			}
#endif
			// ... drop through to the default case to apply monochrome/negate

		default:
			// If monochrome, convert the image to a greyscale
			if (FilterDescription->IsMonochrome())
			{
				// Will anyone ever need this? It seems unlikely - but we may well get Key coming
				// through here, which is entirely sensible, so we only error if there is colour info.
				ERROR3IF(OutputCMYK->Cyan != 0 || OutputCMYK->Magenta != 0 || OutputCMYK->Yellow != 0,
						"Monochrome output of colour CMYK to CMYK colour context is unimplemented");
			}

			// If negative, then photographically invert the colour
			if (FilterDescription->IsNegative())
			{
				OutputCMYK->Cyan	= 1.0 - OutputCMYK->Cyan;
				OutputCMYK->Magenta	= 1.0 - OutputCMYK->Magenta;
				OutputCMYK->Yellow	= 1.0 - OutputCMYK->Yellow;
				OutputCMYK->Key		= 1.0 - OutputCMYK->Key;
			}
			break;
	}
}



/********************************************************************************************

>	virtual BOOL ColourContextCMYK::IsDifferent(ColourContext *Other) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96
	Inputs:		Other - a colour context to comapre ourselves to
	Outputs:	-
	Returns:	TRUE if the other colour context object is different from
				ourself, or TRUE if it is an exactly equivalent context.
	Purpose:	Determine if two colour contexts are not exactly equivalent
	Errors:		-
	SeeAlso:	ColourContext::IsDifferent

********************************************************************************************/

BOOL ColourContextCMYK::IsDifferent(ColourContext *Other) const
{
	if (ColourContext::IsDifferent(Other))	// Different on generic ColourContext basis
		return(TRUE);

	// These must both be CMYK contexts, so check if they are equivalent
	return(ProfileName.IsIdentical(((ColourContextCMYK *)Other)->ProfileName));
}



/********************************************************************************************

>	virtual void ColourContextCMYK::GetWhite(ColourGeneric *Result);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/96

	Outputs:	Result - Will be filled in with White, however that is defined in
				this context's colour model. (i.e. RGB(1,1,1) or CMYK(0,0,0,0), etc)

	Purpose:	An easy way to get a colour definition in this colour model which
				is white (paper colour).

********************************************************************************************/

void ColourContextCMYK::GetWhite(ColourGeneric *Result)
{
	ColourCMYK *cmyk = (ColourCMYK *) Result;
	cmyk->Cyan = cmyk->Magenta = cmyk->Yellow = cmyk->Key = 0;
}



/********************************************************************************************

>	virtual void ColourContextCMYK::ConvertColourBase(ColourContextCMYK *SourceContext,
										ColourGeneric *Source, ColourGeneric *Result
										IndexedColour *SourceIxCol = NULL)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/03/94
	Inputs:		SourceContext - The context in which the source is defined 

				Source - A ColourGeneric to convert

				Result - A ColourGeneric structure to recieve the resulting converted colour,
				as defined in this ColourContextCMYK.

				SourceIxCol - NULL, or a pointer to the IndexedColour we are converting.
				This is used only for handling spot colour separations, to determine if
				the colour is a given spot colour or tint thereof.

	Outputs:	Result - will be returned with the converted colour

	Purpose:	SEE ColourContext::ConvertColourbase
				This method is identical to that function, except that it also passes the
				colour directly through (rather than converting to/from CIET) if the source
				and destination contexts are both CMYK (even if they are different contexts,
				CMYK colours are always passed through without colour correction)

	Scope:		private to colcontx.cpp
	Errors:		-
	SeeAlso:	ColourContextCMYK::ConvertColour

********************************************************************************************/

void ColourContextCMYK::ConvertColourBase(ColourContext *SourceContext,
										ColourGeneric *Source, ColourGeneric *Result,
										IndexedColour *SourceIxCol)
{
	// Copy the source colour into a temporary variable
	ColourGeneric FilteredSource;
	memcpy(&FilteredSource, Source, sizeof(ColourGeneric));

	// Call the SourceContext to allow it to pre-filter the colour. This is used to
	// provide colour separations and other doody features when converting CMYK colours
	// - when converting non-CMYK colours, this is usually done in the OutputFilter (below)
	if (ColPlate != NULL && !ColPlate->IsDisabled())
		SourceContext->ApplyInputFilter(ColPlate, this, &FilteredSource, SourceIxCol);

	// -----
	// Call levels expected:
	//	1 inline ConvertColour checks if can satisfy from cache
	//	2 function ConvertColourInternal does any short cuts it can, such
	//	  as CMYK->CMYK passthrough
	//	3 Call ConvertColourBase, which
	//		a) Checks if in same colour model, and implements passthrough, or
	//		b) Converts colour properly via CIET space

	if (SourceContext == this || SourceContext->GetColourModel() == COLOURMODEL_CMYK)
	{
		// The SourceContext and Destination context are identical, so we can
		// just copy the definition directly across.
		memcpy(Result, &FilteredSource, sizeof(ColourGeneric));
	}
	else
	{
		DColourCIET IntermediateResult;

		SourceContext->ConvertToCIET(&FilteredSource, &IntermediateResult);
		ConvertFromCIET(&IntermediateResult, Result);
	}


	// Call the DestinationContext (derived class of this) to apply any output filtering -
	// non-CMYK colour conversions will separate the colour (as appropriate) here
	if (ColPlate != NULL && !ColPlate->IsDisabled())
		ApplyOutputFilter(ColPlate, SourceContext, Result, SourceIxCol);
}


















/********************************************************************************************

>	void ColourContextHSVT::ColourContextHSVT(View *Scope)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/94
	Inputs:		Scope - The view in which this context is to be used, or NULL
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an HSVT Colour context

	Notes:		Colour Contexts should not be created and used directly. See the
				notes in the SeeAlso's for instructions on proper care and use.

	Errors:		-
	SeeAlso:	ColourContext::ColourContext; ColourContextList::AddContext

********************************************************************************************/

ColourContextHSVT::ColourContextHSVT(View *Scope)
				  : ColourContext(Scope)
{
	ColModel = COLOURMODEL_HSVT;

	CreateExternalTransform();
}

/********************************************************************************************

>	void ColourContextHSVT::CreateExternalTransform(void)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/96
	Inputs:		-
	Returns:	-
	Purpose:	Create an external transform to be used during colour conversions from
				HSV to the current RCS space. This function uses the Winoil CMS Manager
				to create the transform. Both forward and backward transforms use a physical
				printer profile description.

********************************************************************************************/

void ColourContextHSVT::CreateExternalTransform()
{
#ifndef NO_XARACMS
	CMSColourSpace cmsSpace;
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
	
	if (lpCMSMan != NULL)
	{
		// first read our internal colour calibration space
		lpCMSMan->GetLogicalProfile(&cmsSpace);
		// set this as the active colour profile
		UINT32 err = lpCMSMan->SetProfile(cmsSpace);
		// if we haven't got an error, create that transform
		if (err==0)
			CMSTransform = lpCMSMan->CreateTransform(DISPLAY_RCS);
	}
#endif
}



/********************************************************************************************

>	void ColourContextHSVT::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/94
	Inputs:		Source - A Colour defined in this HSVT context
				Result - pointer to a structure to recieve the resulting colour as
				defined in CIE space
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour from our HSVT colourspace to CIET colourspace
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextHSVT::ConvertFromCIET

********************************************************************************************/

void ColourContextHSVT::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	// First, let's convert the HSV into RGB
	FIXED24 H,S,V, R,G,B, F, P,Q,T;			// Sorry, I just have to override the style guide here!

	H = ((ColourHSVT *)Source)->Hue;
	S = ((ColourHSVT *)Source)->Saturation;
	V = ((ColourHSVT *)Source)->Value;

	if (S == FIXED24(0))
	{
		R = V;
		G = V;
		B = V;
	}
	else
	{
		H *= 6;
		INT32 I = H.MakeInt();
		F = H - I;
		P = V * (1.0 - S);
		Q = V * (1.0 - (S * F));
		T = V * (1.0 - (S * (1.0 - F)));

		switch (I)
		{
			case 1:  R = Q; G = V; B = P; break;
			case 2:  R = P; G = V; B = T; break;
			case 3:  R = P; G = Q; B = V; break;
			case 4:  R = T; G = P; B = V; break;
			case 5:  R = V; G = P; B = Q; break;
			default: R = V; G = T; B = P; break;
		}
	}

#ifndef NO_XARACMS
	// Now, convert the RGB to CIE
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;
		icol.c0 = R.MakeDouble();
		icol.c1 = G.MakeDouble();
		icol.c2 = B.MakeDouble();
		
		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvRGBtoCIEXYZ(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsForward, icol, ocol);
		
		Result->X = ocol.c0;
		Result->Y = ocol.c1;
		Result->Z = ocol.c2;
		Result->Transparent = 0.0;//bob->Transparent;
	}
	else
#endif
	{
		// The old rgb system
		Result->X = R.MakeDouble();
		Result->Y = G.MakeDouble();
		Result->Z = B.MakeDouble();
		Result->Transparent = 0;//((ColourHSVT *)Source)->Transparent;
	}
}




/********************************************************************************************

>	void ColourContextHSVT::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/94
	Inputs:		Source - A Colour defined in the CIET colourspace
				Result - pointer to a structure to recieve the resulting colour as
				defined in the HSVT space described by this colour context.
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour to our HSVT colourspace from CIET colourspace
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextHSVT::ConvertToCIET

********************************************************************************************/

void ColourContextHSVT::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

	// First, we'll convert from CIE to RGB
	double R, G, B;

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;
		icol.c0	= Source->X;
		icol.c1	= Source->Y;
		icol.c2	= Source->Z;

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvCIEXYZtoRGB(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsReverse, icol, ocol);

		R = ocol.c0;
		G = ocol.c1;
		B = ocol.c2;
	}
	else
#endif
	{
		// The old rgb system
		R = Source->X;
		G = Source->Y;
		B = Source->Z;
	}


	// Now, convert the RGB value into HSV
	double Min, Max, Delta, H, S, V;
	Min = min(R, G);
	Min = min(Min, B);
	Max = max(R, G);
	Max = max(Max, B);

	V = Max;
	Delta = Max - Min;

	if (fabs(Delta) > 0.000001)
	{
		if (Max > 0.0)
			S = Delta / Max;
		else
			S = 0.0;

		if (R == Max)
			H = (G - B) / Delta;
		else if (G == Max)
			H = 2.0 + (B - R) / Delta;
		else
			H = 4 + (R - G) / Delta;


		H = H / 6.0;
		if (H < 0.0) H += 1.0;
	}
	else
	{
		H = S = 0.0;
	}

	((ColourHSVT *)Result)->Hue			= H;
	((ColourHSVT *)Result)->Saturation	= S;
	((ColourHSVT *)Result)->Value		= V;
	((ColourHSVT *)Result)->Transparent	= 0; //Source->Transparent;
}

	

/********************************************************************************************

>	virtual void ColourContextHSVT::PackColour(ColourGeneric *Source, ColourPacked *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/94
	Inputs:		Source - A ColourHSVT structure defining the colour in the model
				pertaining to this colour context (HSVT)
				Result - A Packed colour structure in the same model, into which the result
				is copied.

	Outputs:	-
	Returns:	-
	Purpose:	Converts 128-bit colour representation to 32-bit packed form

				Overrides the base class to provide special packing of HSV colours,
				which use a 9-bit component for packed Hue, and a different component
				ordering within the PColourHSVT structure.

	Scope:		Private (used internally by friend class DocColour)
	Errors:		-
	SeeAlso:	ColourContextHSVT::UnpackColour

********************************************************************************************/

void ColourContextHSVT::PackColour(ColourGeneric *Source, ColourPacked *Result)
{
	PColourHSVT *bob = (PColourHSVT *) Result;

	bob->Hue			= PackColour360(Source->Component1);
	bob->Saturation		= PackColour256(Source->Component2);
	bob->Value			= PackColour256(Source->Component3);
	bob->Transparent	= 0; //PackColour128(Source->Transparent);
}



/********************************************************************************************

>	virtual void ColourContextHSVT::UnpackColour(ColourPacked *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/94
	Inputs:		Source - A Packed Colour structure defining the colour in the model
				pertaining to this colour context (HSVT)
				Result - A ColourGeneric structure in the same model, into which the 
				result is copied.
	Outputs:	-
	Returns:	-
	Purpose:	Converts 32-bit packed colour representation to 128-bit form

				Overrides the base class to provide special packing of HSV colours,
				which use a 9-bit component for packed Hue, and a different component
				ordering within the PColourHSVT structure.

	Scope:		Private (used internally by friend class DocColour)
	Errors:		-
	SeeAlso:	ColourContextHSVT::PackColour

********************************************************************************************/

void ColourContextHSVT::UnpackColour(ColourPacked *Source, ColourGeneric *Result)
{
	PColourHSVT *bob = (PColourHSVT *) Source;

	UnpackColour360(bob->Hue,		 &Result->Component1);
	UnpackColour256(bob->Saturation, &Result->Component2);
	UnpackColour256(bob->Value,		 &Result->Component3);
	Result->Component4 = 0;//UnpackColour128(bob->Transparent, &Result->Transparent);
}



/********************************************************************************************

>	void ColourContextHSVT::GetModelName(StringBase *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		-
	Outputs:	Result returns continaing a String of the name of the context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'RGB' 'HSV' 'Greyscale')
				so allow at least 10 chars of space in your displays as well)
	Returns:	-
	Purpose:	Returns the name of this context's colour model
	Notes:		

********************************************************************************************/

void ColourContextHSVT::GetModelName(StringBase *Result)
{
	ENSURE(Result != NULL, "ColourContext::GetModelName called with NULL result pointer!");

	*Result = String_32(_R(IDS_COLMODEL_HSVT));
}



/********************************************************************************************

>	BOOL ColourContextHSVT::GetComponentName(INT32 ComponentID, StringBase *Result,
											 BOOL LongName = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		ComponentID - Index [1..4] of the component you want the name of
				(Result is an output)
				LongName - TRUE to get the long name, FALSE to get the short name
							(e.g. "Magenta" vs "M", "Saturation" vs "Sat")

	Outputs:	Result returns containing a String of the name of the given component
				of colours defined in this context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'Red' 'Hue' 'Saturation')
				so allow at least 10 chars of space in your displays as well)

				Note that Result can be passed in NULL in order to only determine if the
				context supports a given component.

	Returns:	FALSE if this component is not used by this model.
	Purpose:	Returns the name of one of this context's colour components

	Notes:		The components are given in the same order as they appear in the
				high-precision ColourXYZ structures defined in colmodel.h

********************************************************************************************/

BOOL ColourContextHSVT::GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName)
{
	UINT32 StringID = 0;
	BOOL Used = TRUE;

	switch(ComponentID)
	{
		case 1:
			StringID = (LongName) ? _R(IDS_COLCOMPL_HUE) : _R(IDS_COLCOMP_HUE);
			break;
			
		case 2:
			StringID = (LongName) ? _R(IDS_COLCOMPL_SATURATION) : _R(IDS_COLCOMP_SATURATION);
			break;

		case 3:
			StringID = (LongName) ? _R(IDS_COLCOMPL_VALUE) : _R(IDS_COLCOMP_VALUE);
			break;

#ifdef TRANSPARENTCOLOUR
		default:
			StringID = _R(IDS_COLCOMP_TRANS);
			break;
#else
		default:
			Used = FALSE;
			break;
#endif
	}

	if (Result != NULL && StringID != 0)
		*Result = String_32(StringID);

	return(Used);
}



/********************************************************************************************

>	virtual UnitGroup **ColourContextHSVT::GetComponentUnitGroups()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the UnitGroup array
	Purpose:	Provides an array of UnitGroups primarily for use by the base class 
				(ColourContext) members Set/GetComponentUnitGroup().
	Errors:		-
	See Also:	ColourContext::GetComponentUnitGroup()
				ColourContext::SetComponentUnitGroup()

********************************************************************************************/

UnitGroup **ColourContextHSVT::GetComponentUnitGroups()
{
	return m_pUnitGroupArray;
}


UnitGroup* ColourContextHSVT::m_pUnitGroupArray[] = 
{
	&(StandardUnit::AngleGroup),
	&(StandardUnit::PercentGroup),
	&(StandardUnit::PercentGroup)
};



/********************************************************************************************

>	virtual UINT32 ColourContextHSVT::GetComponentCount()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Returns:	The number of components
	Purpose:	Provides number of components in the colour context's colour model

********************************************************************************************/

UINT32 ColourContextHSVT::GetComponentCount()
{
	return MAX_COMPONENTS;
}



/********************************************************************************************

>	virtual void ColourContextHSVT::ApplyTint(ColourValue TintFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		TintFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be tinted. This value is expected to lie between 0.0 and 1.0 inclusive

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is tinted by the given TintFraction
				If TintFraction <= 0.0, White is output
				If TintFraction >= 1.0, The source colour is output

	Returns:	-
	Purpose:	Tints a colour (mixes it with white)

	Notes:		If the tinting value is out of range, either source-colour ow white (as
				appropriate) will be output.

********************************************************************************************/

void ColourContextHSVT::ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult)
{
	ColourHSVT *Result = (ColourHSVT *) SourceAndResult;
	
	if (TintFraction <= FIXED24(0.0))		// 0% tint = White
	{
		Result->Saturation = 0;
		Result->Value = FIXED24(1.0);
		return;
	}

	if (TintFraction >= FIXED24(1.0))		// The Result colour is identical to the source
		return;

	// Otherwise, tint the colour...
	// Hue remains constant
	Result->Saturation = Result->Saturation * TintFraction;
	Result->Value = FIXED24(1.0) - (TintFraction - (Result->Value * TintFraction));
}



/********************************************************************************************

>	void ColourContextHSVT::ApplyShade(ColourValue XFraction, ColourValue YFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		XFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be saturated. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean tint towards 0, while values above mean tint towards 1.
				0.0 means no tinting

				YFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be shaded. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean shade towards 0, while values above mean shade towards 1.
				0.0 means no shading

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is shaded by the given Fractions
				Any invalid value is clipped to 0.0

	Returns:	-
	Purpose:	Shades a colour (tweaks the saturation and value in a relative way)

********************************************************************************************/

void ColourContextHSVT::ApplyShade(ColourValue XFraction, ColourValue YFraction,
									ColourGeneric *SourceAndResult)
{
	ColourHSVT *Result = (ColourHSVT *) SourceAndResult;

	ERROR3IF(XFraction < FIXED24(-1.0) || XFraction > FIXED24(1.0), "Illegal X Shading value");
	ERROR3IF(YFraction < FIXED24(-1.0) || YFraction > FIXED24(1.0), "Illegal Y Shading value");

	// Adjust the Saturation according to XFraction
	if (XFraction != FIXED24(0.0))
	{
		if (XFraction < FIXED24(0.0))
		{
			XFraction = -XFraction;
			Result->Saturation = Result->Saturation * (FIXED24(1.0) - XFraction);
		}
		else
			Result->Saturation = XFraction + Result->Saturation * (FIXED24(1.0) - XFraction);
	}


	// Adjust the Value according to YFraction
	if (YFraction != FIXED24(0.0))
	{
		if (YFraction < FIXED24(0.0))
		{
			YFraction = -YFraction;
			Result->Value = Result->Value * (FIXED24(1.0) - YFraction);
		}
		else
			Result->Value = YFraction + Result->Value * (FIXED24(1.0) - YFraction);
	}
}



/********************************************************************************************

>	virtual void ColourContextHSVT::GetWhite(ColourGeneric *Result);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/96

	Outputs:	Result - Will be filled in with White, however that is defined in
				this context's colour model. (i.e. RGB(1,1,1) or CMYK(0,0,0,0), etc)

	Purpose:	An easy way to get a colour definition in this colour model which
				is white (paper colour).

********************************************************************************************/

void ColourContextHSVT::GetWhite(ColourGeneric *Result)
{
	ColourHSVT *hsvt = (ColourHSVT *) Result;
	hsvt->Hue = hsvt->Saturation = 0;
	hsvt->Value = 1.0;
	hsvt->Transparent = 0;
}
















/********************************************************************************************

>	void ColourContextGreyT::ColourContextGreyT(View *Scope)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		Scope - The view in which this context is to be used, or NULL
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an GreyT Colour context

	Notes:		Colour Contexts should not be created and used directly. See the
				notes in the SeeAlso's for instructions on proper care and use.

	Errors:		-
	SeeAlso:	ColourContext::ColourContext; ColourContextList::AddContext

********************************************************************************************/

ColourContextGreyT::ColourContextGreyT(View *Scope)
				  : ColourContext(Scope)
{
	ColModel = COLOURMODEL_GREYT;

	CreateExternalTransform();
}



/********************************************************************************************

>	void ColourContextGreyT::CreateExternalTransform(void)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/05/96
	Inputs:		-
	Returns:	-
	Purpose:	Create an external transform to be used during colour conversions from
				Grey to the current RCS space. This function uses the Winoil CMS Manager
				to create the transform. Both forward and backward transforms use logical
				RGB descriptions.

********************************************************************************************/

void ColourContextGreyT::CreateExternalTransform()
{
#ifndef NO_XARACMS
	CMSColourSpace cmsSpace;
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();
	
	if (lpCMSMan != NULL)
	{
		// first read our internal colour calibration space
		lpCMSMan->GetLogicalProfile(&cmsSpace);

		// set this as the active colour profile
		UINT32 err = lpCMSMan->SetProfile(cmsSpace);

		// if we haven't got an error, create that transform
		if (err==0)
			CMSTransform = lpCMSMan->CreateTransform(DISPLAY_RCS);
	}
#endif
}


/********************************************************************************************

>	void ColourContextGreyT::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/6/94
	Inputs:		Source - A Colour defined in this GreyT context
				Result - pointer to a structure to recieve the resulting colour as
				defined in CIE space
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour from our GreyT colourspace to CIET colourspace
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextGreyT::ConvertFromCIET

********************************************************************************************/

void ColourContextGreyT::ConvertToCIET(ColourGeneric *Source, DColourCIET *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		// The new colour calibration method
		CMSColour icol, ocol;

		icol.c2 = ((ColourGreyT *)Source)->Intensity.MakeDouble();
		icol.c0 = icol.c1 = icol.c2;

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvRGBtoCIEXYZ(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsForward, icol, ocol);

		Result->X = ocol.c0;
		Result->Y = ocol.c1;
		Result->Z = ocol.c2;
	}
	else
#endif
	{
		// The old rgb system
		Result->X = Result->Y = Result->Z = ((ColourGreyT *)Source)->Intensity.MakeDouble();
	}

	Result->Transparent = 0.0;//((ColourGreyT *)Source)->Transparent;
}




/********************************************************************************************

>	void ColourContextGreyT::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/04/94
	Inputs:		Source - A Colour defined in the CIET colourspace
				Result - pointer to a structure to recieve the resulting colour as
				defined in the GreyT space described by this colour context.
	Outputs:	-
	Returns:	-
	Purpose:	Converts the given colour to our GreyT colourspace from CIET colourspace
	Notes:		-
	Errors:		-
	SeeAlso:	ColourContextGreyT::ConvertToCIET

********************************************************************************************/

void ColourContextGreyT::ConvertFromCIET(DColourCIET *Source, ColourGeneric *Result)
{
	ENSURE(UsageCount > 0, "Colour context being used when not initialised!");

#ifndef NO_XARACMS
	// If we've got a CMS manager, use it, else use the version 1.1 bodge
	XaraCMS* lpCMSMan = GetApplication()->GetCMSManager();

	if (lpCMSMan != NULL)
	{
		CMSColour icol, ocol;

		icol.c0	= Source->X;
		icol.c1	= Source->Y;
		icol.c2	= Source->Z;

		// If we have no colour transform, or if we don't want the colour corrected/separated,
		// then we will just do a "logical" conversion.
		if (CMSTransform == NULL || (ColPlate != NULL && ColPlate->IsDisabled()))
			lpCMSMan->ConvCIEXYZtoRGB(icol, ocol);
		else
			lpCMSMan->GTransform(CMSTransform, cmsReverse, icol, ocol);

		// The loadings below are taken from Gerry, the expert on colour loadings
		// in this particular office. No expense was spared to research these
		// values properly... ;-)
		double inten;
		inten = (ocol.c0 * 0.305) + (ocol.c1 * 0.586) + (ocol.c2 * 0.109);
		((ColourGreyT *)Result)->Intensity = inten;
	}
	else
#endif
	{
		double R, G, B;
		R = Source->X;
		G = Source->Y;
		B = Source->Z;

		// The loadings below are taken from Gerry, the expert on colour loadings
		// in this particular office. No expense was spared to research these
		// values properly... ;-)
		((ColourGreyT *)Result)->Intensity = (R * 0.305) + (G * 0.586) + (B * 0.109);
	}

	// Set the transparency field
	((ColourGreyT *)Result)->Transparent = 0;//Source->Transparent;

	// And ensure that the reserved fields are cleared
	((ColourGreyT *)Result)->Reserved1 = ((ColourGreyT *)Result)->Reserved2 = 0;
}



/********************************************************************************************

>	void ColourContextGreyT::GetModelName(StringBase *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/94
	Inputs:		-
	Outputs:	Result returns continaing a String of the name of the context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'RGB' 'HSV' 'Greyscale')
				so allow at least 10 chars of space in your displays as well)
	Returns:	-
	Purpose:	Returns the name of this context's colour model
	Notes:		

********************************************************************************************/

void ColourContextGreyT::GetModelName(StringBase *Result)
{
	ENSURE(Result != NULL, "ColourContext::GetModelName called with NULL result pointer!");

	*Result = String_32(_R(IDS_COLMODEL_GREY));
}



/********************************************************************************************

>	BOOL ColourContextGreyT::GetComponentName(INT32 ComponentID, StringBase *Result,
											 BOOL LongName = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		ComponentID - Index [1..4] of the component you want the name of
				(Result is an output)
				LongName - TRUE to get the long name, FALSE to get the short name
							(e.g. "Magenta" vs "M", "Saturation" vs "Sat")

	Outputs:	Result returns containing a String of the name of the given component
				of colours defined in this context.
				This string is guaranteed to fit within a String_32 - this is the
				minimum size you should allocate. (Most names, in English at least,
				are shorter than this (10 chars max, e.g. 'Red' 'Hue' 'Saturation')
				so allow at least 10 chars of space in your displays as well)

				Note that Result can be passed in NULL in order to only determine if the
				context supports a given component.

	Returns:	FALSE if this component is not used by this model.
	Purpose:	Returns the name of one of this context's colour components

	Notes:		The components are given in the same order as they appear in the
				high-precision ColourXYZ structures defined in colmodel.h

********************************************************************************************/

BOOL ColourContextGreyT::GetComponentName(INT32 ComponentID, StringBase *Result, BOOL LongName)
{
	UINT32 StringID = 0;
	BOOL Used = TRUE;

	switch(ComponentID)
	{
		case 1:
			StringID = (LongName) ? _R(IDS_COLCOMPL_INTENSITY) : _R(IDS_COLCOMP_INTENSITY);
			break;
			
#ifdef TRANSPARENTCOLOUR
		case 2:
		case 3:
			Used = FALSE;		// These components are not used
			break;

		default:
			StringID = _R(IDS_COLCOMP_TRANS);
			break;
#else
		default:
			Used = FALSE;
			break;
#endif
	}

	if (Result != NULL && StringID != 0)
		*Result = String_32(StringID);

	return(Used);
}



/********************************************************************************************

>	virtual UnitGroup **ColourContextGreyT::GetComponentUnitGroups()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the UnitGroup array
	Purpose:	Provides an array of UnitGroups primarily for use by the base class 
				(ColourContext) members Set/GetComponentUnitGroup().
	Errors:		-
	See Also:	ColourContext::GetComponentUnitGroup()
				ColourContext::SetComponentUnitGroup()

********************************************************************************************/

UnitGroup **ColourContextGreyT::GetComponentUnitGroups()
{
	return m_pUnitGroupArray;
}


UnitGroup* ColourContextGreyT::m_pUnitGroupArray[] =
{
	&(StandardUnit::PercentGroup)
};



/********************************************************************************************

>	virtual UINT32 ColourContextGreyT::GetComponentCount()

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/05/96		  
	Returns:	The number of components
	Purpose:	Provides number of components in the colour context's colour model

********************************************************************************************/

UINT32 ColourContextGreyT::GetComponentCount()
{
	return MAX_COMPONENTS;
}



/********************************************************************************************

>	virtual void ColourContextGreyT::ApplyTint(ColourValue TintFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		TintFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be tinted. This value is expected to lie between 0.0 and 1.0 inclusive

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is tinted by the given TintFraction
				If TintFraction <= 0.0, White is output
				If TintFraction >= 1.0, The source colour is output

	Returns:	-
	Purpose:	Tints a colour (mixes it with white)

	Notes:		If the tinting value is out of range, either source-colour ow white (as
				appropriate) will be output.

********************************************************************************************/

void ColourContextGreyT::ApplyTint(ColourValue TintFraction, ColourGeneric *SourceAndResult)
{
	ColourGreyT *Result = (ColourGreyT *) SourceAndResult;
	
	if (TintFraction <= FIXED24(0.0))		// 0% tint = White
	{
		Result->Intensity = FIXED24(1.0);
		return;
	}

	if (TintFraction >= FIXED24(1.0))		// The Result colour is identical to the source
		return;

	// Otherwise, tint the colour...
	Result->Intensity = FIXED24(1.0) - (TintFraction - (Result->Intensity * TintFraction));
}



/********************************************************************************************

>	void ColourContextGreyT::ApplyShade(ColourValue XFraction, ColourValue YFraction,
												ColourGeneric *SourceAndResult)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		XFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be saturated. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean tint towards 0, while values above mean tint towards 1.
				0.0 means no tinting

				YFraction - The fraction (as a FIXED24 ColourValue) by which the colour
				should be shaded. This value is expected to lie between -1.0 and 1.0 inclusive
				Values below 0.0 mean shade towards 0, while values above mean shade towards 1.
				0.0 means no shading

				SourceAndResult - The source colour to be tinted, as defined in this
				colour model

	Outputs:	SourceAndResult is shaded by the given Fractions
				Any invalid value is clipped to 0.0

	Returns:	-
	Purpose:	Shades a colour (tweaks the saturation and value in a relative way)

	Notes:		Contexts other than HSV currently convert the colour to and from HSV
				form in order to apply the shade. This is a tad inefficient, but a
				quick'n'easy bodge that will get it going nice and quickly.

********************************************************************************************/

void ColourContextGreyT::ApplyShade(ColourValue XFraction, ColourValue YFraction,
										ColourGeneric *SourceAndResult)
{
	ColourHSVT HSVDef;
	ColourContext *cc = ColourContext::GetGlobalDefault(COLOURMODEL_HSVT);
	ERROR3IF(cc == NULL, "No default HSV colour context?!");

	cc->ConvertColour(this, SourceAndResult, (ColourGeneric *) &HSVDef);
	cc->ApplyShade(XFraction, YFraction,  (ColourGeneric *) &HSVDef);
	ConvertColour(cc, (ColourGeneric *) &HSVDef, SourceAndResult);
}



/********************************************************************************************

>	virtual void ColourContextGreyT::GetWhite(ColourGeneric *Result);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/6/96

	Outputs:	Result - Will be filled in with White, however that is defined in
				this context's colour model. (i.e. RGB(1,1,1) or CMYK(0,0,0,0), etc)

	Purpose:	An easy way to get a colour definition in this colour model which
				is white (paper colour).

********************************************************************************************/

void ColourContextGreyT::GetWhite(ColourGeneric *Result)
{
	ColourGreyT *grey = (ColourGreyT *) Result;
	grey->Intensity = 1.0;

	grey->Reserved1 = grey->Reserved2 = grey->Transparent = 0;
}

