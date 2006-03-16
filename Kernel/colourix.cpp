// $Id: colourix.cpp 662 2006-03-14 21:31:49Z alex $
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
// colourix.cpp - Indexed colour class

/*
*/


#include "camtypes.h"

#include "colcontx.h"
#include "colourix.h"
#include "colormgr.h"
#include "doccolor.h"
#include "ensure.h"
//#include "jason.h"

CC_IMPLEMENT_DYNAMIC(IndexedColour, ListItem)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


#ifndef WEBSTER // there are never spot colours in WEBSTER Martin 15/07/97
BOOL IndexedColour::SpotsAreProcess = FALSE;
#else
BOOL IndexedColour::SpotsAreProcess = TRUE;
#endif

/********************************************************************************************

>	void IndexedColour::InitialiseInfoFields(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Private shared code for construction of IndexedColour objects
				Initialises the 'Info' structure to default values
	Scope:		private
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void IndexedColour::InitialiseInfoFields(ColourModel ColModel)
{
	UsageCount = 0;						// We are not in-use yet
	ChildUsage = 0;

	Name		 = NULL;   				// This colour has no name

	ParentColour = NULL;				// We are not linked to another colour by default

	Info.OCContextHandle	= 0;		// Set colour model, and remember we're not cached
	Info.SourceColourModel	= ColModel;
	Info.CacheColourModel	= COLOURMODEL_NOTCACHED;

	Info.InheritComponent1  =			// By default, override all components in linked colour
		Info.InheritComponent2 =
		Info.InheritComponent3 =
		Info.InheritComponent4 = FALSE;

	Info.ColourType			= COLOURTYPE_NORMAL;	// Is normal (not spot/tint/linked) colour

	Info.Deleted			= FALSE;				// Not a deleted (hidden) colour

	Info.IsNamed = FALSE;
}



/********************************************************************************************

>	IndexedColour::IndexedColour()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an IndexedColour object
				Initialises the colour to the RGBT value for Opaque Black
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

IndexedColour::IndexedColour()
{
	InitialiseInfoFields(COLOURMODEL_RGBT);

	SourceColour.Component1	= 0;	// Black & No-colour (in RGBT)
	SourceColour.Component2	= 0;
	SourceColour.Component3	= 0;
	SourceColour.Component4	= 1.0;	// 100% transparent
}



/********************************************************************************************

>	IndexedColour::~IndexedColour()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor for an IndexedColour object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

IndexedColour::~IndexedColour()
{
	ERROR3IF(UsageCount < 0, "IndexedColour UsageCount is negative");

#ifdef _DEBUG
	if (UsageCount != 0)
	{
		if (IsNamed())
			TRACE( _T("\n>> Named IndexedColour %p ('%s') is still referenced %ld times\n"),
					this, (TCHAR *)(*Name), (INT32)UsageCount);
		else
			TRACE( _T("\n>> Unnamed IndexedColour %p is still referenced %ld times\n"),
					this, (INT32)UsageCount);

		// Now, ask SimpleCCObject to wugg through all current objects to see if we can find something
		// that could be the pointer back at us!
		extern void CheckForUsedPointer(void *ThePointer);

		TRACE( _T("  References to this object were found in:\n"));
		CheckForUsedPointer(this);
		TRACE( _T("\n"));
	}

	ERROR3IF(UsageCount > 0, "IndexedColour deleted while still in use");

	if (ChildUsage != 0)
	{
		if (IsNamed())
		{
			TRACEUSER( "Jason", _T("Jason: Named IndexedColour %p ('%s') is still LINKED %ld times\n"),
					this, (TCHAR *)(*Name), (INT32)ChildUsage);
		}
		else
		{
			TRACEUSER( "Jason", _T("Jason: Unnamed IndexedColour %p is still LINKED %ld times\n"),
					this, (INT32)ChildUsage);
		}
	}
#endif

	// If we are linked to another colour, we need to de-reference it
	if (ParentColour != NULL)
		ParentColour->DecrementChildUsage();

	if (Name != NULL)
		delete Name;
}



/********************************************************************************************

>	IndexedColour::IndexedColour(const IndexedColour& Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/05/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Copy constructor for an IndexedColour object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

IndexedColour::IndexedColour(const IndexedColour& Col)
{
	Info = Col.Info;
	memcpy(&SourceColour, &Col.SourceColour, sizeof(ColourGeneric));
	memcpy(&CachedColour, &Col.CachedColour, sizeof(ColourGeneric));

	// If we are copying a linked/tint colour, we make ourselves linked to their parent
	ParentColour = Col.ParentColour;
	if (ParentColour != NULL)
		ParentColour->IncrementChildUsage();

	UsageCount = 0;
	ChildUsage = 0;
	
	Info.IsNamed = FALSE;
	Name = NULL;
	
	// Nasty casting to get around this messy const problem.
	if (((IndexedColour *)&Col)->IsNamed())
		SetName(*(Col.Name));	// Copy the string, not the string-pointer
}



/********************************************************************************************

>	IndexedColour::IndexedColour(const DocColour& Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/94
	Inputs:		Col - A DocColour from which to take the definition of this new IndexedCol
	Outputs:	-
	Returns:	-
	Purpose:	Copy constructor for an IndexedColour object.
	
	Notes:		If the DocColour is immediate (does not reference an IndexedColour) then
				the indexedcolour uses its colour definition - NOTE that this creates an
				UNNAMED colour by default (SetName it afterwards). 

				If the DocColour references an IndexedColour, the referenced colour is copied
				
				Tints and Linked colours are copied and remain tints/linked (Gosh!)

				Named/Unnamed colours will be copied as Named/Unnamed colours (Gosh!)

				The name is copied verbatim (i.e it is not changed to 'Copy of X')

	Errors:		-
	SeeAlso:	IndexedColour::SetName

********************************************************************************************/

IndexedColour::IndexedColour(const DocColour& Col)
{
	// Find the parent IndexedColour, if any. Nasty cast to remove compiler warning
	IndexedColour *Def = ((DocColour *) &Col)->FindParentIndexedColour();

	if (Def != NULL)
	{
		// The DocColour references an IndexedColour, so copy the parent IndexedColour
		Info = Def->Info;
		memcpy(&SourceColour, &Def->SourceColour, sizeof(ColourGeneric));
		memcpy(&CachedColour, &Def->CachedColour, sizeof(ColourGeneric));

		// If we are copying a linked/tint colour, we need to link ourselves to its parent
		ParentColour = Def->ParentColour;
		if (ParentColour != NULL)
			ParentColour->IncrementChildUsage();

		UsageCount = 0;
		ChildUsage = 0;
	
		Info.IsNamed = FALSE;
		Name = NULL;
		if (Def->IsNamed())
			SetName(*(Def->Name));	// Copy the string, not the string-pointer
	}
	else
	{
		// This is an immediate DocColour, so copy its definition into our SourceColour
		InitialiseInfoFields(Col.GetColourModel());
		((DocColour *) &Col)->GetSourceColour(&SourceColour);
	}
}



/********************************************************************************************

>	IndexedColour::IndexedColour(ColourValue Red, ColourValue Green, ColourValue Blue,
									ColourValue Transparent = 0,
									UINT32 TransType = TRANSTYPE_DEFAULT)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		Red; Green; Blue - The RGB definition of the colour

				Transparent - The transparency of the colour - IGNORED!
				TransType - The transparency type of the colour - IGNORED!

	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an IndexedColour object
				Initialises the colour to the given Extended RGBT value

	Notes:		Colours no longer support transparency. The transparency fields are
				ignored.

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

IndexedColour::IndexedColour(ColourValue Red, ColourValue Green, ColourValue Blue,
								ColourValue Transparent, UINT32 TransType)
{
	InitialiseInfoFields(COLOURMODEL_RGBT);
	SourceColour.Component1	= Red;
	SourceColour.Component2	= Green;
	SourceColour.Component3	= Blue;
	SourceColour.Component4	= Transparent;
}



/********************************************************************************************

>	IndexedColour::IndexedColour(ColourModel ColModel, ColourGeneric *Col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		ColModel - The colour model in which Col is defined
				Col - The definition, in the given colour model, of the colour
	Outputs:	-
	Returns:	-
	Purpose:	Constructor for an IndexedColour object
				Initialises the colour to the given value in the given colour model
				NOTE that if you have a ColourABCD structure, you can pass this in
				as a single parameter, and inline functions will convert the call
				into a call to this function on your behalf.
				i.e. you can use IndexedColour((ColourRGBT *)Bob);	-- See colour.h
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

IndexedColour::IndexedColour(ColourModel ColModel, ColourGeneric *Col)
{
	InitialiseInfoFields(ColModel);
	memcpy(&SourceColour, Col, sizeof(ColourGeneric));
}



/********************************************************************************************

>	IndexedColour& IndexedColour::operator=(const IndexedColour& Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Other - colour to copy
	Outputs:	-
	Returns:	
	Purpose:	IndexedColour assignment operator
				Copies the definition of one IndexedColour into another
	Notes:		The name field is copied verbatim - if you want it to say 'Copy of ...'
				then you'll have to prepend this text yourself.
	Errors:		ERROR2 if you try to copy a colour over itself

********************************************************************************************/

IndexedColour& IndexedColour::operator=(const IndexedColour& Other)
{
	ERROR2IF(&Other == this, *this, "Attempt to copy an IndexedColour over itself!");

	Info = Other.Info;
	memcpy(&SourceColour, &Other.SourceColour, sizeof(ColourGeneric));
	memcpy(&CachedColour, &Other.CachedColour, sizeof(ColourGeneric));

	if (ParentColour != NULL)					// Delink from old parent (if any)
		ParentColour->DecrementChildUsage();

	ParentColour = Other.ParentColour;
	if (ParentColour != NULL)					// Link to new parent (if any)
		ParentColour->IncrementChildUsage();

//	UsageCount = 0;		// These DO NOT CHANGE! We still have people referencing us
//	ChildUsage = 0;		// and we must not forget about them!

	Info.IsNamed = FALSE;
	if (((IndexedColour *)&Other)->IsNamed())
		SetName(*(Other.Name));		// Copy the string, not the string-pointer

	return(*this);
}



/********************************************************************************************

>	IndexedColourType IndexedColour::GetType(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/94
	Returns:	The type of this colour:
				MonoOn
				COLOURTYPE_NORMAL, COLOURTYPE_SPOT, COLOURTYPE_TINT, COLOURTYPE_LINKED
				MonoOff
	
	Purpose:	Determines the type of this colour

	Notes:		If this colour is linked to a deleted colour, then for sneaky undo reasons
				it (internally) remains linked. However, it pretends to the outside world
				that it is in fact a normal colour.

	SeeAlso:	IndexedColour::SetType; IndexedColour::FindLinkedParent;
				IndexedColour::SetLinkedParent

********************************************************************************************/

IndexedColourType IndexedColour::GetType(void) const
{
	if ((Info.ColourType == (UINT32) COLOURTYPE_LINKED ||
		 Info.ColourType == (UINT32) COLOURTYPE_TINT) &&
		 ParentColour != NULL && ParentColour->IsDeleted())
	{
		return(COLOURTYPE_NORMAL);
	}

	// If the global "SpotsareProcess" flag is set, then spots pretend to be normal
	// Unnamed/Local colours also cannot be spots, so in case we accidentally get into the
	// wrong state, we make sure never to return a silly result.
	if (Info.ColourType == COLOURTYPE_SPOT && (SpotsAreProcess || !Info.IsNamed))
		return(COLOURTYPE_NORMAL);

	return((IndexedColourType) Info.ColourType);
}



/********************************************************************************************

>	BOOL IndexedColour::IsSpotOrTintOfSpot(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96

	Returns:	TRUE if this colour is either a spot colour or a tint of a spot colour

	Purpose:	Determines if this colour is either a spot colour or a true tint of a
				spot colour. (Tints can unfortunately be made from non-spot colour
				parents, so you need to call this function to determine if a tint
				really is cleanly derived from a spot colour, or if it must be treated
				as a simple process colour)

				Thus, this returns TRUE if the colour itself is a Spot, or
				if it is a Tint and its parent returns TRUE from IsSpotOrTintOfSpot().

	Notes:		If a colour in the tint chain is deleted, then all children	must
				become process (non true-tint) colours, so this will return FALSE

********************************************************************************************/

BOOL IndexedColour::IsSpotOrTintOfSpot(void) const
{
	if (Info.Deleted)
		return(FALSE);

	IndexedColourType Type = GetType();
	if (Type == COLOURTYPE_SPOT)
		return(TRUE);								// Is it a true spot colour?

	if (Type != COLOURTYPE_TINT || TintIsShade())
		return(FALSE);								// It's not even a tint

	if (ParentColour == NULL)
		return(FALSE);								// We have no parent colour

	// OK, we're a tint of something. Recurse to find out if that something
	// is a true tint or a spot colour.
	return(ParentColour->IsSpotOrTintOfSpot());
}



/********************************************************************************************

>	ColourModel IndexedColour::GetColourModel(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	The colour model in which the IndexedColour was defined
	
	Purpose:	To determine the colour model in which an IndexedColour is defined

	Notes:		If this colour is a TINT, the parent colour's colour model is returned,
				as tints cannot be defined in a different model to their parents

	Errors:		-
	SeeAlso:	IndexedColour::GetSourceColour

********************************************************************************************/

ColourModel IndexedColour::GetColourModel(void) const
{
	// If we're a tint, return our parent's colour. NOTE that we do not call GetType()
	// as we want to ask our parent even if it is 'deleted' for UNDO reasons.
	if (Info.ColourType == COLOURTYPE_TINT && ParentColour != NULL)
		return(ParentColour->GetColourModel());

	return((ColourModel) Info.SourceColourModel);
}



/********************************************************************************************

>	void IndexedColour::GetSourceColour(ColourGeneric *Result)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	The colour definition of this IndexedColour, in its native colour model
	Purpose:	Gets the definition of the IndexedColour.

	Notes:		If the colour is Tint/Linked, this may involve a recursive set of calls which
				will convert and override each parent colour up the chain until the
				resulting colour definition for this colour is eventually produced at
				the end.

	Errors:		-
	SeeAlso:	IndexedColour::GetColourModel

********************************************************************************************/

void IndexedColour::GetSourceColour(ColourGeneric *Result)
{
	ERROR3IF(Result == &SourceColour,
			 "Illegal attempt to GetSourceColour into a colour's own SourceColour field!");

	if (ParentColour == NULL || ParentColour == this ||
		(Info.ColourType != COLOURTYPE_LINKED && Info.ColourType != COLOURTYPE_TINT))
	{
		// We are not linked to a parent colour	(or the link is scarily illegal to ourself!)
		// (ParentColour == NULL OR we are not tint/linked)
		// - simply copy our own colour definition, checking that this is not an internal
		// call to get a source colour definition into our own source colour!
		if (&SourceColour != Result)
			memcpy(Result, &SourceColour, sizeof(ColourGeneric));
	}
	else
	{
		ERROR3IF(ParentColour == NULL || ParentColour == this ||
				 ParentColour->IsADescendantOf(this),
				 "Consistency failure in IndexedColour parent linkage");

		// Ask our parent for its colour definition - recurse!
		ParentColour->GetSourceColour(Result);

		// Get our 'parent' colour context
		ColourContext *cc = ColourContext::GetGlobalDefault(GetColourModel());
		ERROR3IF(cc == NULL, "IndexedColour::GetSourceColour - illegal colour context in use?!");

		// Now, override the colour as necessary
		// NOTE that we do NOT call GetType(), as this might return COLOURTYPE_NORMAL for tints/linked
		// colours which have 'deleted' parents. This is due to nasty UNDO considerations - we retain
		// our linked parent internally when it is 'deleted'.
		if (Info.ColourType == COLOURTYPE_LINKED)
		{
			// If necessary, convert the Result colour into our own colour model
			if (ParentColour->GetColourModel() != GetColourModel())
			{
				ColourContext *ccSource = ColourContext::GetGlobalDefault(ParentColour->GetColourModel());

				if (ccSource != NULL)	// Shouldn't happen, but let's be safe
				{
					ColourGeneric Source;
					memcpy(&Source, Result, sizeof(ColourGeneric));

					cc->ConvertColour(ccSource, &Source, Result);
				}
			}

// **** ToDo !!!! Override by copying OR Inherit by scaling !!!!

			// This is a linked colour. Override the given components with our own values
			if (!Info.InheritComponent1)
				Result->Component1 = SourceColour.Component1;
			if (!Info.InheritComponent2)
				Result->Component2 = SourceColour.Component2;
			if (!Info.InheritComponent3)
				Result->Component3 = SourceColour.Component3;
			if (!Info.InheritComponent4)
				Result->Component4 = SourceColour.Component4;
		}
		else if (Info.ColourType == COLOURTYPE_TINT)
		{
			// This is a tint. Component 1 of our 'SourceColour' is the tinting value
			// We get our 'parent' colour context to work out how to apply the tint/shade
			if (TintIsShade())
				cc->ApplyShade(SourceColour.Component1, SourceColour.Component2, Result);	// Shade it
			else
				cc->ApplyTint(SourceColour.Component1, Result);								// Tint it
		}
		else
		{
			ERROR3("Unknown/Illegal colour type in IndexedColour::GetSourceColour");
		}
	}
}



/********************************************************************************************

>	BOOL IndexedColour::IsDifferent(const IndexedColour &Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Other - the other indexed colour to which 'this' will be compared
	Outputs:	-
	Returns:	TRUE if the two colours are considered different
				FALSE if they are considered the same
	Purpose:	To determine if two indexed colours share a common definition.

				This is defined as: Same Colour model, and source colour definitions
				and both are linked to/tint of the same colour (or none), and the 'no colour'
				(and if linked: inheritance bits; if tint: tinting values) and colour type
				match exactly.

				NOTE especially that this does NOT compare the names of the colours

********************************************************************************************/

BOOL IndexedColour::IsDifferent(const IndexedColour &Other)
{
	if (GetType() != Other.GetType())
		return(TRUE);

	if (GetColourModel() != Other.GetColourModel())
		return(TRUE);

	// If this is a linked colour, check if their inheritance flags match
	if (GetType() == COLOURTYPE_LINKED)
	{
		if (ParentColour != Other.ParentColour)
			return(TRUE);

		if (Info.InheritComponent1 != Other.Info.InheritComponent1 ||
			Info.InheritComponent2 != Other.Info.InheritComponent2 ||
			Info.InheritComponent3 != Other.Info.InheritComponent3 ||
			Info.InheritComponent4 != Other.Info.InheritComponent4)
			return(TRUE);
	}

	// If this is a tint, it has no components in the regular sense.
	// Return immediately with the result of comparing the tint values
	if (GetType() == COLOURTYPE_TINT)
	{
		if (ParentColour != Other.ParentColour)
			return(TRUE);

		if (TintIsShade() != Other.TintIsShade())
			return(TRUE);

		if (TintIsShade())
		{
			return(GetShadeValueX() != Other.GetShadeValueX() ||
					GetShadeValueY() != Other.GetShadeValueY());
		}

		return(GetTintValue() != Other.GetTintValue());
	}

	if (GetColourModel() == COLOURMODEL_GREYT)
	{
		// We ignore the 2nd and 3rd components as they are not used.
		// (Should not be necessary, as these components should be zeroed anyway)
		if (SourceColour.Component1 != Other.SourceColour.Component1 ||
			SourceColour.Component4 != Other.SourceColour.Component4)
			return(TRUE);
	}
	else
	{	
		if (SourceColour.Component1 != Other.SourceColour.Component1 ||
			SourceColour.Component2 != Other.SourceColour.Component2 ||
			SourceColour.Component3 != Other.SourceColour.Component3 ||
			SourceColour.Component4 != Other.SourceColour.Component4)
			return(TRUE);
	}

	return(FALSE);
}



/********************************************************************************************

>	BOOL IndexedColour::IsDifferent(const IndexedColour &Other, FIXED24 ErrorLimit)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/94
	Inputs:		Other - the other indexed colour to which 'this' will be compared
	Outputs:	-
	Returns:	TRUE if the two colours are considered different
				FALSE if they are considered the same
	Purpose:	To determine if two indexed colours share a common definition.

				This is defined as: Same Colour model, and source colour definitions
				and both are linked to/tint of the same colour (or none), and the 'no colour'
				(and if linked: inheritance bits; if tint: tinting values) and colour type
				match within the limits specified by ErrorLimit.

				This version of the function takes an accuracy parameter, to allow
				'rough' comparisons to be made.  This is used when importing colours so 
				that we don't have to store 32bit colour values in EPS files and so on
				in order for them to be recognised as an existing colour when re-importing.

				NOTE especially that this does NOT compare the names of the colours

********************************************************************************************/

#define OUTSIDE_LIMIT(a,b) ((ABS((a) - (b))) > (ErrorLimit))

BOOL IndexedColour::IsDifferent(const IndexedColour &Other, FIXED24 ErrorLimit)
{
	if (GetType() != Other.GetType())
		return(TRUE);

	if (GetColourModel() != Other.GetColourModel())
		return(TRUE);

	// If this is a linked colour, check if their inheritance flags match
	if (GetType() == COLOURTYPE_LINKED)
	{
		if (ParentColour != Other.ParentColour)
			return(TRUE);

		if (Info.InheritComponent1 != Other.Info.InheritComponent1 ||
			Info.InheritComponent2 != Other.Info.InheritComponent2 ||
			Info.InheritComponent3 != Other.Info.InheritComponent3 ||
			Info.InheritComponent4 != Other.Info.InheritComponent4)
			return(TRUE);
	}

	// If this is a tint, it has no components in the regular sense.
	// Return immediately with the result of comparing the tint values
	if (GetType() == COLOURTYPE_TINT)
	{
		if (ParentColour != Other.ParentColour)
			return(TRUE);

		if (TintIsShade() != Other.TintIsShade())
			return(TRUE);

		if (TintIsShade())
		{
			return( OUTSIDE_LIMIT(GetShadeValueX(), Other.GetShadeValueX()) ||
					OUTSIDE_LIMIT(GetShadeValueY(), Other.GetShadeValueY()) );
		}

		return( OUTSIDE_LIMIT(GetTintValue(), Other.GetTintValue()) );
	}

	// Special case for greyscales...
	if (GetColourModel() == COLOURMODEL_GREYT)
	{
		// We ignore the 2nd and 3rd components as they are not used.
		if (OUTSIDE_LIMIT(SourceColour.Component1, Other.SourceColour.Component1) ||
			OUTSIDE_LIMIT(SourceColour.Component4, Other.SourceColour.Component4))
			return(TRUE);
	}
	else
	{
		if (OUTSIDE_LIMIT(SourceColour.Component1, Other.SourceColour.Component1) ||
			OUTSIDE_LIMIT(SourceColour.Component2, Other.SourceColour.Component2) ||
			OUTSIDE_LIMIT(SourceColour.Component3, Other.SourceColour.Component3) ||
			OUTSIDE_LIMIT(SourceColour.Component4, Other.SourceColour.Component4))
			return(TRUE);
	}
	
	return(FALSE);
}

// Ensure we don't use this macro out of its intended scope - it uses 'ErrorLimit'!
#undef OUTSIDE_LIMIT



/********************************************************************************************

>	String_64 *IndexedColour::GetName(BOOL ReturnTrueID = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/04/94

	Inputs:		ReturnTrueID - FALSE (the default) to retrieve the 'normal' name. If this
				colour is unnamed, the string "Unnamed" will be returned.
				TRUE (*use with care*) to return a unique id for this colour. If unnamed,
				this will return a unique name string like "_362180". This is only intended
				for use by the import/export system.

	Outputs:	-
	Returns:	A pointer to a String_64 containing the name of this indexed colour
				If it is named, the return value is always the name
				If it is unnamed, then either "Unnamed" or a unique id "_1234567" will be
				returned.

	Purpose:	To find the name of an IndexedColour (or the ID of an unnamed indexed colour)
	Errors:		-

********************************************************************************************/

String_64 *IndexedColour::GetName(BOOL ReturnTrueID)
{
	static String_64 Default(_R(IDS_LOCALCOLOUR));

	if (IsNamed())
	{
		if (Name != NULL)
			return(Name);
	}
	else
	{
		if (ReturnTrueID)
		{
			// If we're an unnamed colour, then we generate an "ID" (not a name, honest, guv!)
			// which is unique. This is used for export/import to recognise unnamed colours

			if (Name == NULL)
			{
				Name = new String_64;
				if (Name != NULL)
					Name->_MakeMsg( TEXT("_#1%p"), this );
			}

			if (Name != NULL)
				return(Name);
		}
	}

	return(&Default);
}



/********************************************************************************************

>	void IndexedColour::SetName(const StringBase &NewName, [BOOL ForceNamed = TRUE])

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94
	Inputs:		NewName - A StringBase which will be *copied* as this colours new name.
				NOTE that this name will be truncated to a maximum length of 63 characters,
				and any underscore characters within it will be converted to spaces

				ForceNamed - if TRUE (the default), this forces the colour to become 'named'
				and will strip out underline characters from it.
				If FALSE (*use with care*) this will leave the colour in its current state,
				(i.e. won't make an unnamed colour named) and will retain underlines. This is
				only intended for use by importers for handling export/import of unnamed
				colours.

	Purpose:	To set the name of an IndexedColour.
				The name will be truncated to a maximum length of 63 characters.

	Notes:		Any underscore characters in the name will be converted to spaces, on the
				grounds that the export system uses underscores as a special name identifier
				to recognise unnamed colours. Suffice to say we just don't allow underscores

				If NewName is NULL, or if we fail to allocate space for the name to
				be stored, the colour will revert to having no name.

				Setting the name of an IndexedColour makes it a Named IndexedColour
				(Trust me, that is less silly than it sounds! ;-)
	
	SeeAlso:	IndexedColour::SetUnnamed; IndexedColour::IsNamed

********************************************************************************************/

void IndexedColour::SetName(const StringBase &NewName, BOOL ForceNamed)
{
	if (ForceNamed)
		Info.IsNamed = TRUE;

	if (Name == NULL)
		Name = new String_64;		// If this fails, Name will just be NULL, which is safe

	if (Name != NULL)
	{
		if (ForceNamed)
		{
			const TCHAR *Src  = (const TCHAR *)NewName;
			TCHAR *Dest = (TCHAR *)(*Name);

			// Copy up to 63 chars across
			cc_lstrcpyn(Dest, Src, 63);
			
			// convert underscores into spaces
			Name->SwapChar(TCHAR('_'), TCHAR(' '));
		}
		else
		{
			NewName.Left(Name, 63);			// Truncate to 63 chars max, and copy
		}
	}
}



/********************************************************************************************

>	void IndexedColour::SetUnnamed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/04/94

	Purpose:	To UNset the name of an IndexedColour, forcing it back to being Unnamed

********************************************************************************************/

void IndexedColour::SetUnnamed(void)
{
	Info.IsNamed = FALSE;

	if (Name != NULL)
		delete Name;

	Name = NULL;
}



/********************************************************************************************

>	BOOL IndexedColour::IsADescendantOf(IndexedColour *TestParent)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		TestParent - the parent/ancestor colour we wish to test for
	Returns:	TRUE if this is indeed a descendant of the given colour
	
	Purpose:	Recursively backtracks through the chain of LinkedParent links to determine
				if this colour is in any way (directly or indirectly) linked to the given
				Parent colour, and might therefore be affected by any change to said colour.

	Notes:		To determine if a colour is the *direct* parent of another, use
					if (TestParent == ThisColour->FindLinkedParent())

				If the TestParent is THIS colour, the return value is TRUE (i.e Yes, a change
				to the 'Parent' colour will affect 'this' colour!). {This is also an end-of-
				recursion clause}

				This method only calculates descendant information for LINKED/TINT colours.
				Non-linked/tint colours will always spell the end of the search.

				Is quite happy if TestParent is NULL - will always return FALSE in this case

********************************************************************************************/

BOOL IndexedColour::IsADescendantOf(IndexedColour *TestParent)
{
	if (TestParent == NULL)			// Silly test! Of course it's not!
		return(FALSE);

	if (this == TestParent)			// We are the colour being tested - we are related
		return(TRUE);
									// We cannot be descended, as we're a normal colour
	if (GetType() == COLOURTYPE_NORMAL || GetType() == COLOURTYPE_SPOT)
		return(FALSE);

	if (ParentColour == NULL)		// We have no parent, so cannot be descended from anything
		return(FALSE);
									// Ask our parent to check back in the family tree
	return(ParentColour->IsADescendantOf(TestParent));
}



/********************************************************************************************

>	IndexedColour *IndexedColour::FindLinkedParent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Returns:	The colour upon which this Tint or Linked colour is based.
				(If this colour is not a Tint or Linked colour, or if it is a
				deleted (hidden for undo) colour, the return value is NULL)
				
	Notes:		If you need to know what colour this colour *was* linked to, as the
				colour editor does when the user is randomly plonking about with the
				colour type, then call FindLastLinkedParent instead.

	Purpose:	Finds the colour upon which a tint/based-on colour is based, if any.
				Can also be used to determine if a colour is a tint/basedon colour.

	SeeAlso:	IndexedColour::FindLastLinkedParent

********************************************************************************************/

IndexedColour *IndexedColour::FindLinkedParent(void)
{
	if (IsDeleted())
		return(NULL);

	if (GetType() != COLOURTYPE_TINT && GetType() != COLOURTYPE_LINKED)
		return(NULL);

	return(FindLastLinkedParent());
}



/********************************************************************************************

>	IndexedColour *IndexedColour::FindOldestAncestor(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96
	Returns:	The utimate colour upon which this Tint or Linked colour is based.
				Note that if this colour has no parent, it will return itself!

	Purpose:	Finds the colour upon which a tint/based-on colour is based, if any.
				Can also be used to determine if a colour is a tint/basedon colour.

				This is similar to FindLinkedParent, except it traverses all parent links
				until it finds the ultimate parent of the entire linked-colour tree
				in which this colour resides.

	SeeAlso:	IndexedColour::FindLinkedParent

********************************************************************************************/

IndexedColour *IndexedColour::FindOldestAncestor(void)
{
	IndexedColour *Ptr	= FindLinkedParent();
	IndexedColour *Last = this;

	while (Ptr != NULL)
	{
		Last = Ptr;
		Ptr	= Ptr->FindLinkedParent();
	}

	return(Last);
}



/********************************************************************************************

>	void IndexedColour::SetLinkedParent(IndexedColour *Parent,
										IndexedColourType NewType = COLOURTYPE_LINKED)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		Parent - the new Linked/Tint parent IndexedColour, or
						 NULL to remove any existing link

				NewType - The ColourType to set this to. Note that for NORMAL/SPOT, the
				parent field must be NULL; For LINKED/TINT, it must point at a valid
				Named IndexedColour. If this is not the case, an ERROR3 will occur.

	Purpose:	To make a colour linked to or a tint of another colour.
				This links the colour to the parent.
				
	Notes:		Generally, the colour's colour model and definition will be unaffected.
				However, if the colour was previously linked/tint, and it is being made
				into a standalone colour, the existing linked colour definition will be
				read, and then placed into SourceColour as a standalone version of the
				previous linked definition. (i.e. the colour's appearance will not change
				but the link will be broken)
				
				If the colour was not already a COLOURTYPE_LINKED when it is being made
				LINKED, all components will be set to override the parent.
				
				If the colour was not already a COLOURTYPE_TINT when it is being made
				a TINT, it will be set to be a 100% tint (no tinting at all)

				When making a COLOURTYPE_TINT colour, it will be a TINT, not a SHADE.
				To make a shade, make it COLOURTYPE_TINT then call SetTintOrShade(TRUE);
				If it was already a COLOURTYPE_TINT, the tint/shade state is unaffected.

				The colour's output cache is flushed by this call

				It is your responsibility to confirm overwriting the colour. It is also
				your responsibility to inform the system that this colour has changed, by
				calling ColourManager::ColourHasChanged

				IMPORTANT NOTE: This method allows you to do things like making a colour
				'normal' (supposedly not linked), but still set a parent colour. This is
				a potentially dangerous practice so be very careful. (It is allowed so that
				the colour editor can cunningly remember what a colour was last linked
				to, in order to be far more helpful to the user).
				
	Scope:		private? Generally speaking, you should not be calling this method, unless
				you are the colour editor, or a load/import filter.

	SeeAlso:	IndexedColour::GetLinkedParent; IndexedColour::GetLastLinkedParent;
				IndexedColour::SetInheritsComponent;
				IndexedColour::SetType; IndexedColourType;
				ColourManager::ColourHasChanged

	Errors:		ERROR3 if you try to do anythign totally stupid (circular linking,
				linking to deleted colours, linking to unnamed colours, etc)

********************************************************************************************/

void IndexedColour::SetLinkedParent(IndexedColour *Parent, IndexedColourType NewType)
{
	if (this == Parent)
	{
		ERROR3("Illegal attempt to make an IndexedColour Linked to itself");
		return;
	}

	if (Parent != NULL && Parent->IsADescendantOf(this))
	{
		// Our new parent-to-be is already a linked child of us!
		ERROR3("Illegal attempt to generate circular reference in Colour linking"); 
		return;
	}

	if (Parent != NULL && !Parent->IsNamed())
	{
		ERROR3("Illegal attempt to use UNNAMED IndexedColour as a linked parent");
		return;
	}

	if (Parent!= NULL && Parent->IsDeleted())
	{
		ERROR3("Illegal attempt to use 'DELETED' IndexedColour as a linked parent");
		return;
	}


	switch(NewType)
	{
		case COLOURTYPE_LINKED:
			if (GetType() != COLOURTYPE_LINKED)
			{
				Info.InheritComponent1  =		// Override all components for now
					Info.InheritComponent2 =
					Info.InheritComponent3 =
					Info.InheritComponent4 = FALSE;
			}
			break;


		case COLOURTYPE_TINT:
			if (GetType() != COLOURTYPE_TINT)
			{
				SetType(NewType);				// Set type now to stop the next call ENSURE'ing
				SetTintOrShade(FALSE);			// Make it a real tint (not a shade "tint")
 				SetTintValue(FIXED24(1.0));		// 100% tint (the colour) for now
			}

			// Swapping from Tint to Shade (which are both COLOURTYPE_TINT will need to do this,
			// so we do it even if it is supposely chnaging to the same "type"
			if (Parent != NULL)
				Info.SourceColourModel = (UINT32) Parent->GetColourModel();
			break;


		default:		// COLOURTYPE_NORMAL, COLOURTYPE_SPOT
			if (FindLastLinkedParent() != NULL)
			{
				// If this was previously a tint/linked colour, then copy the resulting
				// colour definition into this colour to make it standalone again.

				ColourGeneric NewSourceColour;
				GetSourceColour(&NewSourceColour);
				memcpy(&SourceColour, &NewSourceColour, sizeof(ColourGeneric));
			}
			break;
	}

	if (ParentColour != NULL)
		ParentColour->DecrementChildUsage();

	ParentColour = Parent;			// Set the new linked/tint parent colour

	if (ParentColour != NULL)
		ParentColour->IncrementChildUsage();

	InvalidateCache();				// Invalidate the output colour cache
	SetType(NewType);				// And set the new type of the colour
}



/********************************************************************************************

>	BOOL IndexedColour::SetType(IndexedColourType NewType)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Returns:	TRUE if this caused the type of this colour to be changed
				FALSE if the new type is the same as the old type
	
	Purpose:	Sets the type of this colour:
				MonoOn
				COLOURTYPE_NORMAL, COLOURTYPE_SPOT, COLOURTYPE_TINT, COLOURTYPE_LINKED
				MonoOff

	Notes:		If you wish to make a colour Linked or a Tint, then you should set its type
				at the same time as its parent-link in SetLinkedParent(). If you wish to
				make a linked/tint colour normal again, then you should also use
				SetLinkedParent.

				On second thoughts, you shouldn't be calling this method!

	Scope:		private

	SeeAlso:	IndexedColour::SetType; IndexedColour::FindLinkedParent;
				IndexedColour::SetLinkedParent

********************************************************************************************/

BOOL IndexedColour::SetType(IndexedColourType NewType)
{
	BOOL Result = (NewType != (IndexedColourType) Info.ColourType);

	if ((INT32)NewType < 0 || (INT32)NewType > 3)
	{
		ERROR3("Attempt to set invalid colour type");
		return(FALSE);
	}

	Info.ColourType = (UINT32) NewType;

	if (Result)					// If the colour has changed, invalidate the output cache
		InvalidateCache();

	return(Result);
}



/********************************************************************************************

>	BOOL IndexedColour::InheritsComponent(UINT32 ComponentID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		ComponentID: a number [1..4] indicating which component of the colour
				you wish to find out about.

	Returns:	TRUE if the component is inherited from the parent
				FALSE if the component overrides the parent, or if the colour isn't linked

	Purpose:	To determine if a given component of a colour is inherited from its linked
				parent colour. Always returns FALSE if a colour is not linked.

	Notes:		Should be used in conjunction with the appropriate ColourContext for this
				colour's colour model to determine if *valid* components of a colour are
				inherited from their parent colour. If asked about an invalid component,
				the result will be indeterminate (generally FALSE, but somebody may have
				set it to TRUE without checking if it is a valid component)

	SeeAlso:	IndexedColour::GetType(); ColourContext::GetComponentName;
				IndexedColour::SetInheritsComponent

********************************************************************************************/

BOOL IndexedColour::InheritsComponent(UINT32 ComponentID)
{
	ERROR3IF(ComponentID < 1 || ComponentID > 4,
				"Bad colour component index passed to IndexedColour::InheritsComponent");

	if (GetType() == COLOURTYPE_LINKED)
	{
		switch(ComponentID)
		{
			case 1:
				return(Info.InheritComponent1);

			case 2:
				return(Info.InheritComponent2);

			case 3:
				return(Info.InheritComponent3);

			case 4:
				return(Info.InheritComponent4);
		}
	}

	return(FALSE);
}



/********************************************************************************************

>	BOOL IndexedColour::SetInheritsComponent(UINT32 ComponentID, BOOL Inherits)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/94
	Inputs:		ComponentID: a number [1..4] indicating which component of the colour
				you wish to set inheritance information for.
				Inherits -	TRUE if the component is to be inherited from the parent
							FALSE if the component is to override the parent

	Returns:	TRUE if this results in the stored value actually changing

	Purpose:	To set if a given component of a colour is inherited from its linked
				parent colour.

	Errors:		In a debug build, will give an ERROR3 if the colour is not linked
				or if the index given is out of range.
				In all builds, incorrect parameters will be ignored.

	Notes:		Should be used in conjunction with the appropriate ColourContext for this
				colour's colour model to determine if a component is *valid* before you
				try to set it.

	SeeAlso:	IndexedColour::GetType(); ColourContext::GetComponentName;
				IndexedColour::InheritsComponent

********************************************************************************************/

BOOL IndexedColour::SetInheritsComponent(UINT32 ComponentID, BOOL Inherits)
{
	BOOL Result = FALSE;
	
	ERROR3IF(ComponentID < 1 || ComponentID > 4,
				"Bad colour component index passed to IndexedColour::InheritsComponent");

	ERROR3IF(GetType() != COLOURTYPE_LINKED,
				"Attempt to set Component Inheritance for non-Linked colour");

	UINT32 ItInherits = (Inherits) ? 1 : 0;

	if (GetType() == COLOURTYPE_LINKED)
	{
		switch(ComponentID)
		{
			case 1:
				Result = (Info.InheritComponent1 != ItInherits);
				Info.InheritComponent1 = ItInherits;
				break;

			case 2:
				Result = (Info.InheritComponent2 != ItInherits);
				Info.InheritComponent2 = ItInherits;
				break;

			case 3:
				Result = (Info.InheritComponent3 != ItInherits);
				Info.InheritComponent3 = ItInherits;
				break;

			case 4:
				Result = (Info.InheritComponent4 != ItInherits);
				Info.InheritComponent4 = ItInherits;
				break;
		}
	}

	if (Result)					// If the colour has changed, invalidate the output cache
		InvalidateCache();

	return(Result);
}



/********************************************************************************************

>	void IndexedColour::SetTintValue(FIXED24 NewTintValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		NewTintValue - the new tint value, in the range 0.0 to 1.0
				Any value outside this range will be clipped to 0.0 or 1.0
	Outputs:	-
	Returns:	-
	Purpose:	Sets a new tint value for a Tint colour. Will generate ENSURE failures in
				the debug build if the colour is not a tint

				This also forces a Tint to be a true tint (not a shade)

	Notes:		Internally, tint is currently stored in the SourceColour.Component1 field.
				This should not be relied upon externally.

	SeeAlso:	IndexedColour::SetShadeValues

********************************************************************************************/

void IndexedColour::SetTintValue(FIXED24 NewTintValue)
{
	if (GetType() != COLOURTYPE_TINT)
	{
		ERROR3("Your puny attempt to set the tint value for a non-tint colour has been ignored");
		return;
	}

	Info.InheritComponent1 = FALSE;		// Force it to be a tint (not a shade)

	if (NewTintValue <= FIXED24(0.0))
		NewTintValue = 0;

	if (NewTintValue >= FIXED24(1.0))
		NewTintValue = FIXED24(1.0);

	SourceColour.Component1 = NewTintValue;

	InvalidateCache();		// The colour has changed, so invalidate the output cache
}



/********************************************************************************************

>	FIXED24 IndexedColour::GetTintValue(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	A FIXED24 value between 0.0 and 1.0 representing the Tint for this colour

				Note that a spot colour returns 1.0 (a 100% tint - 100% ink density), and
				any process (non-spot/tint) colour returns 0.0 (a 0% tint, i.e. no ink).

	Purpose:	Gets the current Tint value for the colour.

				NOTE that the returned tint value simply indicates how much this colour
				tints its parent colour (if at all). If you want to know how much ink will
				actually be produced for this colour, then you want to use
				GetAccumulatedTintValue() instead.

	Notes:		Internally, tint is currently stored in the SourceColour.Component1 field
				This should not be relied upon externally.

	SeeAlso:	IndexedColour::GetAccumulatedTintValue

********************************************************************************************/

FIXED24 IndexedColour::GetTintValue(void) const
{
	if (GetType() == COLOURTYPE_SPOT)
		return(FIXED24(1.0));					// Spot colour is a 100% tint

	if (GetType() != COLOURTYPE_TINT || TintIsShade())
		return(FIXED24(0.0));					// Any process colour is a 0% tint

	return(SourceColour.Component1);			// Any tint is a (Tint)% tint
}



/********************************************************************************************

>	FIXED24 IndexedColour::GetAccumulatedTintValue(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/96

	Returns:	A FIXED24 value between 0.0 and 1.0 representing the accumulated Tint for
				this colour. See Purpose for details.

	Purpose:	To determine the accumulated tint value of any colour.

				If this is not a true tint colour, returns 0.0 (a 0% tint, i.e. white)

				If this is a spot colour, returns 1.0 (i.e. 100% ink)

				If this is a true tint of a spot ink, it returns the overall ink density
				that should be used. With a simple tint-of-spot tint, this is the normal
				tint value, but if there are a chain of tints, the tint values are
				cumulative, i.e. a 50% tint of a 50% tint of Red gives a _25%_ ink density.

				Thus, if you apply the GetAccumulatedTintVlaue() to the GetOldestAncestor()
				colour, you will arrive at the right output colour. (This isn't actually
				how screen values for tints are produced, but the cumulative tint is used
				when mixing tints in blends, and most importantly when outputting separated
				Spot ink plates!

	Notes:		The cumulative value is simply the tint values of each colour in turn in the
				parent-colour chain multiplied together (where process colours are treated
				as a tint value of 0.0 and the ultimate spot colour is 1.0)

********************************************************************************************/

FIXED24 IndexedColour::GetAccumulatedTintValue(void)
{
	double Tint = 1.0;
	IndexedColour *Ptr = this;

	while (Ptr != NULL && Tint > 0.0)
	{
		Tint *= Ptr->GetTintValue().MakeDouble();

		Ptr	= Ptr->FindLinkedParent();
	}

	return(FIXED24(Tint));
}



/********************************************************************************************

>	void IndexedColour::SetShadeValues(FIXED24 NewShadeValueX, FIXED24 NewShadeValueY)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		NewShadeValueX - the new Shade Saturation value, in the range -1.0 to 1.0
				NewShadeValueY - the new Shade Brightness value, in the range -1.0 to 1.0
	Outputs:	-
	Returns:	-
	Purpose:	Sets new shade values for a Shade colour. Will generate ENSURE failures in
				the debug build if the colour is not a Tint/Shade

				This also forces a Tint to be a shade (not a true tint)

	Notes:		Internally, tint/shade is currently stored in the SourceColour.Component1 field.
				And now the Component2 field as well. Bodgy city dude.
				This should not be relied upon externally.

	SeeAlso:	IndexedColour::SetTintValue

********************************************************************************************/

void IndexedColour::SetShadeValues(FIXED24 NewShadeValueX, FIXED24 NewShadeValueY)
{
	if (GetType() != COLOURTYPE_TINT)
	{
		ERROR3("Your puny attempt to set the shade values for a non-shade colour has been ignored");
		return;
	}

	Info.InheritComponent1 = TRUE;		// Force it to be a shade


	if (NewShadeValueX <= FIXED24(-1.0))
		NewShadeValueX = FIXED24(-1.0);

	if (NewShadeValueX >= FIXED24(1.0))
		NewShadeValueX = FIXED24(1.0);

	SourceColour.Component1 = NewShadeValueX;


	if (NewShadeValueY <= FIXED24(-1.0))
		NewShadeValueY = FIXED24(-1.0);

	if (NewShadeValueY >= FIXED24(1.0))
		NewShadeValueY = FIXED24(1.0);

	SourceColour.Component2 = NewShadeValueY;


	InvalidateCache();		// The colour has changed, so invalidate the output cache
}



/********************************************************************************************

>	FIXED24 IndexedColour::GetShadeValueX(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		-
	Outputs:	-
	Returns:	A FIXED24 value between -1.0 and 1.0 representing the Shade for this colour
				This value is the half of the shading settings for affecting Saturation
				(If the colour is not a Tint/Shade, debug builds will ENSURE, and all builds will
				return 1.0 (a 100% shade))

	Purpose:	Gets the current Shade Saturation value for the colour

	Notes:		Internally, tint/shade is currently stored in the SourceColour.Component1 field
				And now the Component2 field as well. Bodgy city dude.
				This should not be relied upon externally.

********************************************************************************************/

FIXED24 IndexedColour::GetShadeValueX(void) const
{
	if (GetType() != COLOURTYPE_TINT || !Info.InheritComponent1)
	{
		ERROR3("Attempt to read tint value for a non-shade colour");
		return(FIXED24(1.0));
	}

	return(SourceColour.Component1);
}



/********************************************************************************************

>	FIXED24 IndexedColour::GetShadeValueY(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95
	Inputs:		-
	Outputs:	-
	Returns:	A FIXED24 value between -1.0 and 1.0 representing the Shade for this colour
				This value is the half of the shading settings for affecting Brightness
				(If the colour is not a Tint/Shade, debug builds will ENSURE, and all builds will
				return 1.0 (a 100% shade))

	Purpose:	Gets the current Shade Brightness value for the colour

	Notes:		Internally, tint/shade is currently stored in the SourceColour.Component1 field
				And now the Component2 field as well. Bodgy city dude.
				This should not be relied upon externally.

********************************************************************************************/

FIXED24 IndexedColour::GetShadeValueY(void) const
{
	if (GetType() != COLOURTYPE_TINT || !Info.InheritComponent1)
	{
		ERROR3("Attempt to read tint value for a non-shade colour");
		return(FIXED24(1.0));
	}

	return(SourceColour.Component2);
}



/********************************************************************************************

>	void IndexedColour::SetTintOrShade(BOOL IsATint)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95

	Inputs:		IsATint - TRUE to make this Tint a true Tint, FALSE to make it a Shade

	Purpose:	Sets a COLOURTYPE_TINT colour to be either a Tint (fade-to-white) or
				a shade (fade-to-black) colour. The tint/shade fraction is unchnaged (the
				colour will just toggle between being darker and lighter!)

	Notes:		Internally, tint/shade is currently stored in the SourceColour.Component1 field
				This should not be relied upon externally.

********************************************************************************************/

void IndexedColour::SetTintOrShade(BOOL IsATint)
{
	if (GetType() != COLOURTYPE_TINT)
	{
		ERROR3("Attempt to set tint/shade value for a non-tint colour");
		return;
	}

	Info.InheritComponent1 = IsATint ? 1 : 0;
}



/********************************************************************************************

>	BOOL IndexedColour::TintIsShade(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/95

	Returns:	TRUE if this COLOURTYPE_TINT colour is a true Tint, FALSE if it's a Shade

	Purpose:	Determines if a COLOURTYPE_TINT colour is really a tint or a shade

	Notes:		COLOURTYPE_TINT has two "subtypes" (tint-to white (tint) and tint-to-black
				(shade)). These are differentiated by a single flag, and are otherwise
				implemented identically. Thus, if it's type is "Tint" then it could be a
				shade! (Nasty, but gets around a shortage of flag bits, and also allows
				us to continue using screeds of old code which doesn't know the difference
				between tints and shades and doesn't need to know)

				Internally, tint/shade is currently stored in the SourceColour.Component1 field
				This should not be relied upon externally.

********************************************************************************************/

BOOL IndexedColour::TintIsShade(void) const
{
	if (Info.ColourType != COLOURTYPE_TINT)
	{
		ERROR3("TintIsShade() called for non-tint/shade colour");
		return(FALSE);
	}

	return(Info.InheritComponent1);
}



/********************************************************************************************

>	void IndexedColour::SwapWith(IndexedColour *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		Other - The colour to swap with
	Outputs:	-
	Returns:	-
	Purpose:	This swaps the definitions of the two IndexedColours.

	Notes:		This is used to provide undo/redo via the OpColourChange defined
				in colormgr.cpp/.h, as when we undo a change to an IndexedColour,
				the definition must remain at the same memory address so that all
				references to it in the document do not need to be fixed.
				
				The implementation swaps the Info, SourceColour, & Name fields
				(i.e. the colour definition) ONLY. That is, the usage count for
				each object and the caches are not swapped. The caches for both
				colours are in fact flushed, to guarantee their integrity.

********************************************************************************************/

#define JASONSWAP(X,Y,VARTYPE)					\
	{											\
		VARTYPE T;								\
		memcpy(&(T), &(X), sizeof(VARTYPE));	\
		memcpy(&(X), &(Y), sizeof(VARTYPE));	\
		memcpy(&(Y), &(T), sizeof(VARTYPE));	\
	}

void IndexedColour::SwapWith(IndexedColour *Other)
{
	// Components to copy are:
	// 		Info
	//		SourceColour
	//		ParentColour pointer
	//		Name POINTER
	//			CachedColour	-- We are discarding the cache anyway so ignore
	//			UsageCount		-- We wish to keep the usage counts intact, as it is
	//			ChildUsage			the number of refs to THIS object.
	// Thus, only the first 4 in this list are copied.

	JASONSWAP(Info, Other->Info, IndexedColourInfo);
	JASONSWAP(SourceColour, Other->SourceColour, ColourGeneric);

	// Swap the Name POINTER
	String_64 *NameTemp = Name;
	Name = Other->Name;
	Other->Name = NameTemp;

	// Swap the linked/tint Parent Colour pointer
	IndexedColour *ParentTemp = ParentColour;
	ParentColour = Other->ParentColour;
	Other->ParentColour = ParentTemp;

	// And invalidate the caches just to be on the safe side
	Info.CacheColourModel = Other->Info.CacheColourModel = COLOURMODEL_NOTCACHED;
	Info.OCContextHandle  = Other->Info.OCContextHandle  = 0;
}

#undef JASONSWAP



/********************************************************************************************

>	void IndexedColour::GetDebugDetails(StringBase* Str)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/94
	Inputs:		Str - string to receive the debug details
	Outputs:	-
	Returns:	-
	Purpose:	To get information on this colour for display in the Debug Tree
	Errors:		-

********************************************************************************************/

void IndexedColour::GetDebugDetails(StringBase* Str)
{
	String_256 TempStr;
	ColourContext *cc = ColourContext::GetGlobalDefault(GetColourModel());
	ColourGeneric col;

	String_8 UnnamedString(TEXT("N"));
	if (!IsNamed())
		UnnamedString = TEXT("U");

	GetSourceColour(&col);
	
	String_32 ModelName;
	cc->GetModelName(&ModelName);

	String_32 Type(TEXT(""));
	switch(GetType())
	{
		case COLOURTYPE_SPOT:
			Type._MakeMsg(TEXT(" Spot"));
			break;

		case COLOURTYPE_TINT:
			if (TintIsShade())
				TempStr._MakeMsg( TEXT(" Shade, resulting in"));
			else
				TempStr._MakeMsg( TEXT(" #1%ld% Tint, resulting in"),
									(INT32) (GetTintValue().MakeDouble()*100));
			break;

		case COLOURTYPE_LINKED:
			Type._MakeMsg(TEXT(" Linked, resulting in"));
			break;

		default:
			break;
	}


	TempStr._MakeMsg( TEXT(" #1%sIxCol (#2%s) =#3%s #4%s(#5%ld, #6%ld, #7%ld, #8%ld)\r\n"),
						(TCHAR *) UnnamedString,
						(TCHAR *) *(GetName(TRUE)),
						(TCHAR *) Type,
						(TCHAR *) ModelName,
						(INT32) (col.Component1.MakeDouble()*100),
						(INT32) (col.Component2.MakeDouble()*100),
						(INT32) (col.Component3.MakeDouble()*100),
						(INT32) (col.Component4.MakeDouble()*100));

	(*Str) += TempStr;
}



/********************************************************************************************

>	static void IndexedColour::ForceSpotsToBeProcess(BOOL ForceOn)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/96

	Inputs:		ForceOn - TRUE to force spots to process, FALSE to let them be spots

	Purpose:	To set the global colour flag. When the flag is TRUE, all spot colours
				in the entire program are forced to act as "normal" colours; when set
				FALSE, all spot colours behave normally.

	Notes:		In many ways this would be better as a localised option, at least
				limited to the selected document, but this is much more difficult to
				achieve, and anyway, it's better that spot colours are effectively
				disabled by this flag, so that it becomes obvious that spot colours
				are no longer special.

				This calls ColourManager::SelViewContextHasChanged() to make all
				the appropriate colour interfaces update themselves correctly.
				(Yeah, it's not quite the right name for what the message does,
				but this is just as fundamental a change as context changing)

********************************************************************************************/

void IndexedColour::ForceSpotsToBeProcess(BOOL ForceOn)
{
	SpotsAreProcess = ForceOn;
	ColourManager::SelViewContextHasChanged();
}

