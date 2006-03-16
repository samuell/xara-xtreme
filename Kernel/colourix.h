// $Id: colourix.h 662 2006-03-14 21:31:49Z alex $
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
// colourix.h - definitions for the Camelot IndexedColour class (Named/Unnamed colours)


#ifndef INC_COLOURIX
#define INC_COLOURIX


#include "colmodel.h"
#include "list.h"
#include "colcontx.h"



/********************************************************************************************

<	IndexedColourInfo

	Comment:	Each IndexedColour has an information bitfield in its 'Info' data member.
				This provides storage for many bits of useful information. Note that the
				structure has been designed to align the bits nicely into 8/16 bit portions
				to make life easier for compilers when extracting values.

				Information is held on colour model of source and cached colour definitions,
				the cache status and output context handle, inheritance in linked colours,
				colour type (normal/spot/tint(shade)/linked), and whether a colour is 'deleted'
				(hidden in an undo record).

				There is a Reserved field, in which the spare bit(s) are retained.
				There is currently only 1 spare bit.

				A similar field is used in DocColours

	SeeAlso:	IndexedColour; DocColour

********************************************************************************************/

typedef struct
{
	UINT32 OCContextHandle	: 16;	// Handle of context of the cached colour
	UINT32 SourceColourModel	: 4;	// The ColourModel of the source colour data
	UINT32 CacheColourModel	: 4;	// The ColourModel of the cached colour data

	UINT32 InheritComponent1	: 1;	// Inherited components (4 bits)
	UINT32 InheritComponent2	: 1;	// (NOTE: Tints use InheritComponent1 to indicate Shade)
	UINT32 InheritComponent3	: 1;
	UINT32 InheritComponent4	: 1;

	UINT32 Deleted			: 1;	// 'Deleted' flag for undo 'hiding' in ColMgrDlg

	UINT32 ColourType			: 2;	// Colour type (normal/spot/tint/linked)

	UINT32 IsNamed			: 1;	// TRUE if this is a named IndexedColour
} IndexedColourInfo;



/********************************************************************************************

<	IndexedColourType

	Comment:	IndexedColours can be set to one of 4 types (held in their 2-bit
				Info.ColourType	field). The types are enumerated as:
				MonoOn
				COLOURTYPE_NORMAL	A Normal colour
				COLOURTYPE_SPOT		A Spot colour
				COLOURTYPE_TINT		A Tint of another colour
				COLOURTYPE_LINKED	A colour Linked to another colour
				MonoOff

	SeeAlso:	IndexedColour::SetType; IndexedColour::GetType;
				IndexedColour::SetLinkedParent
			
********************************************************************************************/

typedef enum
{
	COLOURTYPE_NORMAL = 0,			// The colour is a perfectly normal colour
	COLOURTYPE_SPOT,				// The colour is a Spot Colour
	COLOURTYPE_TINT,				// The colour is a Tint (or a shade, a special type of tint)
	COLOURTYPE_LINKED				// The colour is a Linked colour
} IndexedColourType;



/********************************************************************************************

>	class IndexedColour

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	Definition of a colour, stored to high (128-bit) precision.
				DocColours in the Document may refer to IndexedColour objects
				from which they are defined.
				
				IndexedColours are used in two forms: Named and Unnamed. The former
				are held in a list for each document, and appear in the colour line/bar.
				The latter do not appear in the user interface (except when editing
				the colour of an object). They are stored in a separate list in the
				ColourList for the document

				NOTE That if an UNNAMED IndexedColour's usage count is decremented to zero,
				it will automatically delete itself. Named colours must be explicitly
				deleted.
	
	SeeAlso:	DocColour; ColourModel; ColourContext; ColourList
	Documentation:	HowToUse\colours.doc
			
********************************************************************************************/

class CCAPI IndexedColour : public ListItem
{
friend class ColourContext;
friend class DocColour;
friend class ColourPicker;

CC_DECLARE_DYNAMIC(IndexedColour)

public:		// Construction/Destruction
	IndexedColour();
	~IndexedColour();

	// Initialising/Copy Constructors
	IndexedColour(const IndexedColour& Col);

	IndexedColour(const DocColour& Col);

	IndexedColour(ColourModel ColModel, ColourGeneric *Col);

	IndexedColour(ColourValue Red, ColourValue Green, ColourValue Blue,
					ColourValue Transparent = 0, UINT32 TransType = TRANSTYPE_DEFAULT);

	IndexedColour& operator=(const IndexedColour&);


public:		// Interface
	void SetName(const StringBase &Name, BOOL ForceNamed = TRUE);
												// Forces a colour to be named, and sets the name
	void SetUnnamed(void);						// Forces a colour to be unnamed, clears the name

	String_64 *GetName(BOOL ReturnTrueID = FALSE);
	inline BOOL IsNamed(void);

	void GetSourceColour(ColourGeneric *Result);

	BOOL IsDifferent(const IndexedColour &Other);
	BOOL IsDifferent(const IndexedColour &Other, FIXED24 ErrorLimit);

	ColourModel GetColourModel(void) const;

	inline void IncrementUsage(void);
	inline BOOL DecrementUsage(void);
	inline BOOL IsInUse(BOOL IgnoreColourGallery = FALSE);

	inline BOOL IsDeleted(void);
	inline void SetDeleted(BOOL DeletedFlag);

	inline void InvalidateCache(void);

	IndexedColourType GetType(void) const;

	BOOL IsSpotOrTintOfSpot(void) const;

	IndexedColour *FindLinkedParent(void);
	IndexedColour *FindOldestAncestor(void);
	inline BOOL HasLinkedChildren(void);
	BOOL IsADescendantOf(IndexedColour *Parent);
	inline void LinkedAncestorHasChanged(void);

	void SetLinkedParent(IndexedColour *Parent, IndexedColourType NewType = COLOURTYPE_LINKED);

	inline IndexedColour *FindLastLinkedParent(void);

	BOOL SetInheritsComponent(UINT32 ComponentID, BOOL Inherits);
	BOOL InheritsComponent(UINT32 ComponentID);

	void SetTintValue(FIXED24 NewTintValue);
	FIXED24 GetTintValue(void) const;
	FIXED24 GetAccumulatedTintValue(void);

	void SetShadeValues(FIXED24 NewShadeValueX, FIXED24 NewShadeValueY);
	FIXED24 GetShadeValueX(void) const;
	FIXED24 GetShadeValueY(void) const;

	void SetTintOrShade(BOOL IsATint);		// Set this tint to be a Tint (TRUE) or Shade (FALSE)
	BOOL TintIsShade(void) const;			// Returns TRUE if it's a Shade, FALSE if it's a Tint

	void SwapWith(IndexedColour *Other);

	void GetDebugDetails(StringBase* Str);


public:			// Special global overrides - use with great care
	static BOOL AreSpotsForcedToProcess(void)	{ return(SpotsAreProcess); };
	static void ForceSpotsToBeProcess(BOOL ForceOn);


protected:		// IndexedColour data members
	IndexedColourInfo	Info;				// Flags word
	ColourGeneric		SourceColour;		// Source colour definition
	ColourGeneric		CachedColour;		// Cached output colour

	IndexedColour		*ParentColour;		// NULL, or parent of a based-on colour
	DWORD				ChildUsage;			// Number of ParentColour refs from other colours

	DWORD				UsageCount;			// Number of refs to this colour

	String_64			*Name;				// NULL, or pointer to colours name


protected:		// Internal and friend class access methods
	// Internal initialisation method (shared code)
	void InitialiseInfoFields(ColourModel ColModel);

	// Functions to be used only between IndexedColours
	inline void IncrementChildUsage(void);
	inline BOOL DecrementChildUsage(void);

	BOOL SetType(IndexedColourType NewType);

	// functions to access private data members - intended only for use by
	// our friend classes ColourContext and ColourPicker. Functions are used
	// to still provide some measure of data encapsulation.
	// NOTE: Remember when changing the colour definition, you must InvalidateCache
	inline void SetSourceColourModel(ColourModel NewModel);
	inline ColourGeneric *SourceColourPtr(void);

private:
	static BOOL SpotsAreProcess;			// GLOBAL override - all spots are forced "normal"
};


typedef IndexedColour *IndexedColourPtr;



/********************************************************************************************

<	INDEXEDCOLOUR_RGBT(col)
<	INDEXEDCOLOUR_HSVT(col)
<	INDEXEDCOLOUR_CMYK(col)
<	INDEXEDCOLOUR_CIET(col)
<	INDEXEDCOLOUR_GREYT(col)

	Comment:	These macros simplify construction of IndexedColour objects in
				given colour models. Use something like this:

				MonoOn
				{
					ColourRGBT TheDefinition;
					IndexedColour  TheColour = INDEXEDCOLOURRGBT(&TheDefinition);
				}
				MonoOff

	Notes:		Note that these macros explicitly cast 'col' to a
				(ColourGeneric *), so if it is not a pointer to a 128-bit
				colour structure (ColourGeneric, ColourRGBT, ColourCIET etc)
				then nasty things may occur.

	SeeAlso:	IndexedColour::IndexedColour

********************************************************************************************/

#define INDEXEDCOLOUR_RGBT(col)  IndexedColour(COLOURMODEL_RGBT,  (ColourGeneric *)(col))
#define INDEXEDCOLOUR_CMYK(col)  IndexedColour(COLOURMODEL_CMYK,  (ColourGeneric *)(col))
#define INDEXEDCOLOUR_HSVT(col)  IndexedColour(COLOURMODEL_HSVT,  (ColourGeneric *)(col))
#define INDEXEDCOLOUR_CIET(col)  IndexedColour(COLOURMODEL_CIET,  (ColourGeneric *)(col))
#define INDEXEDCOLOUR_GREYT(col) IndexedColour(COLOURMODEL_GREYT, (ColourGeneric *)(col))




/********************************************************************************************

>	inline void IndexedColour::IncrementUsage(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Informs the IndexedColour that yet another thing is referencing it.
				An IndexedColour should not be deleted if its usage count != 0
				A non-zero usage count on exit will generate ENSUREs which usually signal
				memory leaks or people forgetting to link/delink themselves properly.
	Errors:		-
	SeeAlso:	IndexedColour::DecrementUsage; IndexedColour::IsInUse

********************************************************************************************/

void IndexedColour::IncrementUsage(void)
{
	UsageCount++;
}



/********************************************************************************************

>	inline BOOL IndexedColour::DecrementUsage(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if the IndexedColour is not in use (can be safely deleted)
				TRUE if the IndexedColour is in use

	Purpose:	Informs the IndexedColour that a client is no longer referencing it.
				An IndexedColour should not be deleted if its usage count != 0

	Errors:		-
	SeeAlso:	IndexedColour::IncrementUsage; IndexedColour::IsInUse

********************************************************************************************/

BOOL IndexedColour::DecrementUsage(void)
{
	ENSURE(UsageCount > 0, "IndexedColour::DecrementUsage - "
							"UsageCount has gone NEGATIVE!");

	return((--UsageCount) != 0);
}



/********************************************************************************************

>	inline BOOL IndexedColour::IsInUse( [BOOL IgnoreColourGallery = FALSE] )

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94

	Inputs:		[IgnoreColourGallery - INTERNAL paramter for Colour SuperGallery ONLY]
				[LEAVE THIS PARAMETER OUT!]

	Outputs:	-
	Returns:	FALSE if the IndexedColour can safely be deleted
				TRUE if the IndexedColour is in use
	Purpose:	To determine if anything references this IndexedColour
				(An IndexedColour must not be deleted if it is in use)

	Notes:		This is a simple usage count, and does not handle references
				from undo-system hidden nodes etc. This simply records how many
				things hold pointers to this IndexedColour object.
	Errors:		-
	SeeAlso:	IndexedColour::IncrementUsage; IndexedColour::DecrementUsage

********************************************************************************************/

BOOL IndexedColour::IsInUse(BOOL IgnoreColourGallery)
{
	// If this item is referenced in the colour gallery, the gallery needs to know if
	// nobody *else* is using the colour (i.e. if it has more than 1 user)
	if (IgnoreColourGallery)
		return(UsageCount > 1);

	// Under normal circumstances, the colour is in use if anybody is using it
	return(UsageCount != 0);
}



/********************************************************************************************

>	inline void IndexedColour::IncrementChildUsage(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Informs the IndexedColour that yet another thing is referencing it.
				An IndexedColour must not be deleted if its usage count != 0
	Scope:		private
	Errors:		-
	SeeAlso:	IndexedColour::DecrementChildUsage; IndexedColour::HasLinkedChildren

********************************************************************************************/

void IndexedColour::IncrementChildUsage(void)
{
	ENSURE(IsNamed(), "Attempt to increment child usage of an UNNAMED colour!");
	ChildUsage++;
}



/********************************************************************************************

>	inline BOOL IndexedColour::DecrementChildUsage(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if the IndexedColour is not in use (can be safely deleted)
				TRUE if the IndexedColour is in use

	Purpose:	Informs the IndexedColour that a client is no longer referencing it.
				An IndexedColour should not be deleted if its usage count != 0
	Scope:		private

	Errors:		-
	SeeAlso:	IndexedColour::IncrementChildUsage; IndexedColour::HasLinkedChildren

********************************************************************************************/

BOOL IndexedColour::DecrementChildUsage(void)
{
	ENSURE(IsNamed(), "Attempt to decrement child usage of an UNNAMED colour!");

	ENSURE(ChildUsage > 0, "IndexedColour::DecrementChildUsage - "
							"UsageCount has gone NEGATIVE!");

	return((--ChildUsage) != 0);
}



/********************************************************************************************

>	inline BOOL IndexedColour::HasLinkedChildren(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the IndexedColour is in use (as a ParentColour) by other IndexedCols
	Purpose:	To determine if any other IndexedColour references this IndexedColour
				(An IndexedColour must not be deleted if it is in use)
	
	Notes:		This is a simple usage count, and does not handle references
				from undo-system hidden nodes etc. This simply records how many
				other IndexedColours hold ParentColour pointers to this IndexedColour.
	Errors:		-
	SeeAlso:	IndexedColour::IncrementChildUsage; IndexedColour::DecrementChildUsage

********************************************************************************************/

BOOL IndexedColour::HasLinkedChildren(void)
{
	return(ChildUsage != 0);
}



/********************************************************************************************

>	inline BOOL IndexedColour::IsDeleted(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this colour is marked 'deleted'
				FALSE if it is not
	Purpose:	To determine if this colour has been marked 'deleted'. This is used
				for undo, to hide colours which have been 'deleted'.
	Errors:		-
	SeeAlso:	IndexedColour::SetDeleted

********************************************************************************************/

BOOL IndexedColour::IsDeleted(void)
{
	return(Info.Deleted);
}



/********************************************************************************************

>	inline void IndexedColour::SetDeleted(BOOL DeletedFlag)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/6/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This marks/unmarks a colour as 'deleted'. This flag is used
				for undo, to hide colours which have been 'deleted'. Remember
				that this constitutes a change to the colour, so you should call
				ColourManager::ColourListHasChanged, to ensure that the Colour
				dialogues update all displayed lists to show/not-show this colour.
	Scope:		Only intended for internal ColourManager use
	Notes:		Used by the undoable ColourManager OpColourChange. You should use
				colour manager calls to delete colours rather than touching them
				directly. Note that you can 'delete'/'undelete' colours even when
				they are deemed 'In Use'.
	Errors:		-
	SeeAlso:	IndexedColour::IsInUse; IndexedColour::IsDeleted;
				ColourManager::ColourHasChanged

********************************************************************************************/

void IndexedColour::SetDeleted(BOOL DeletedFlag)
{
	Info.Deleted = DeletedFlag;
}



/********************************************************************************************

>	inline void IndexedColour::InvalidateCache(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Invalidates the IndexedColours output colour cache. This ensures that
				the colour is re-converted and cached the next time it is used. 
	Scope:		Generally private; All public access functions invalidate the cache
				automatically if it is necessary (if the colour changes)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void IndexedColour::InvalidateCache(void)
{
	Info.CacheColourModel	= COLOURMODEL_NOTCACHED;
	Info.OCContextHandle	= 0;
}



/********************************************************************************************

>	inline void IndexedColour::LinkedAncestorHasChanged(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Informs an IndexedColour that an ancestor in the linking chain has changed,
				and that therefore, it may also have changed.

				The ColourManager automatically calls this function for all affected
				colours whenever it recieves a ColourChangingMsg COLOURUPDATED, so it should
				never be necessary for anyone other than ColourManager to call this function.
				
	Notes:		Currently this simply invalidates the cache to make the next access update
				the colour from its	Parent(s). A separate function has been used just in case
				we change our minds about how this should be done in the future.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void IndexedColour::LinkedAncestorHasChanged(void)
{
	InvalidateCache();
}



/********************************************************************************************

>	void IndexedColour::SetSourceColourModel(ColourModel NewModel)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sets our colour model number, and invalidates the colour cache
	Scope:		Private, for use by friend classes only
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void IndexedColour::SetSourceColourModel(ColourModel NewModel)
{
	// Set the new colour model
	Info.SourceColourModel	= NewModel;

	// And invalidate the cache...
	Info.CacheColourModel	= COLOURMODEL_NOTCACHED;
	Info.OCContextHandle	= 0;
}



/********************************************************************************************

>	inline ColourDefn *IndexedColour::SourceColourPtr(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/04/94
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to our source colour. Note that this is our source colour
				so it would be appreciated if you'd not trample all over it with
				hob nailed boots.
	Purpose:	Returns  a pointer to our source colour.
				Used to encapsulate our data for external friend users, rather
				than them poking directly at our data structures. Inlined, so
				just as efficient anyway.
	Scope:		Private, for use by friend class ColourContext
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ColourGeneric *IndexedColour::SourceColourPtr(void)
{
	return(&SourceColour);
}



/********************************************************************************************

>	inline BOOL IndexedColour::IsNamed(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Returns:	TRUE if this colour is named (GetName will return this name)
				FALSE if this colour is not named (GetName will return a string like
				"Unnamed colour" if this is the case)

	Purpose:	Determines if this colour has been given a name. This cannot be determined
				using the GetName function, which returns a default name if the colour is
				unnamed.

	SeeAlso:	IndexedColour::SetName; IndexedColour::GetName

********************************************************************************************/

BOOL IndexedColour::IsNamed(void)
{
	return((Info.IsNamed) ? TRUE : FALSE);		// **** !!!! TEMPORARY BODGE!
	//return(Name != NULL);
}



/********************************************************************************************

>	inline IndexedColour *IndexedColour::FindLastLinkedParent(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/94
	Returns:	The parent 'linked' colour of this colour. This will be NULL if the
				colour is not linked to another.
				
	Purpose:	Finds the last parent colour of this colour. Note that after changing a
				linked/tint colour back into a normal/spot colour, the parent is
				remembered. This call thus returns the parent even when the colour
				is no longer a linked/tint colour. Intended only for use by the colour
				editor to allow temporary type chnages to not lose the parent link info.
				The colour editor probably clears the parent colour on exit.

				YOU SHOULD NOT BE USING THIS FUNCTION unless absolutely necessary. Use
				FindLinkedParent instead.

	SeeAlso:	IndexedColour::FindLinkedParent

********************************************************************************************/

IndexedColour *IndexedColour::FindLastLinkedParent(void)
{
	return(ParentColour);
}


#endif		// INC_COLOURIX

