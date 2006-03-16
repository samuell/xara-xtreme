// $Id: doccolor.h 662 2006-03-14 21:31:49Z alex $
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
// doccolor.h - definitions for Camelot DocColour class


#ifndef INC_DOCCOLOR
#define INC_DOCCOLOR


#include "colmodel.h"
#include "stockcol.h"



// This is defined in colcontx.h, but we define it here to eliminate dependencies
#ifndef INC_COLCONTX
class ColourContext;
typedef UINT32 ColourContextHandle;
#endif


// these constants are used to set the Reserved flag in the Info struct when
// colours are converted for use in Brush Attributes. - DY 12/4/2000
const UINT32 COL_NAMED = 1;
const UINT32 COL_BLACK = 2;
const UINT32 COL_WHITE = 3;
const UINT32 COL_OVERRIDABLE = 4;

/////////////////////////////////////////////////////////////////////////////////
// DocColour Information struct
// Each DocColour contains this information on its contents
// Note:     The order of fields was chosen to encourage byte/word16 alignment
// See Also: IndexedColourInfo (in colourix.h)

typedef struct
{
	UINT32 OCContextHandle	: 16;	// Handle of context of the cached colour
	UINT32 SourceColourModel	: 4;	// The ColourModel of the source colour data
	UINT32 CacheColourModel	: 4;	// The ColourModel of the cached colour data

	UINT32 IsSeparable		: 1;	// TRUE if this colour should be separated

	UINT32 ForceRounding		: 1;	// TRUE if this colour should always be shown rounded to half-percents
											// (A bodge for PANTONE colours!)
	UINT32 Reserved			: 5;

	UINT32 NoColour			: 1;	// 'None' Flag: Colour is 100% transparent
} ColourInfo;



/********************************************************************************************

>	class DocColour : public CC_CLASS_MEMDUMP

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	This class defines a colour.
				A DocColour stores an original definition of a colour, a cache
				containing the definition of the colour in the last colour
				context in which it was used, and sundry information. DocColours
				have been designed to be small things to put in attributes.

				DocColours cannot be used on their own, however. They have meaning
				only within the scope of a given colour context.

				DocColours can be defined in any of the available colour models.

				DocColours are stored to "low" (32-bit) precision, except when they
				are references to IndexedColours (which are 128-bit precision).
				However, colour calculations, and access to values stored in
				colours are generally only available through the 128bit interface.

 	Notes:		NOTE that any use of a DocColour may result in updating its cache,
				so you must NOT treat DocColours as const anywhere. Also, work on
				original DocColours rather than copies if you want the cache to
				be kept fresh on the original.

	SeeAlso:	IndexedColour; ColourModel; ColourContext
	Documentation:	HowToUse\colours.doc
			
********************************************************************************************/

class CCAPI DocColour : public CC_CLASS_MEMDUMP
{
friend class ColourContext;

CC_DECLARE_MEMDUMP(DocColour)

public:		// Construction/Destruction
	DocColour();
	~DocColour();

	DocColour(StockColour Col);
			// Initialise with a non-separating stock colour. See kernel\stockcol.h

	DocColour(ColourModel ColModel, ColourGeneric *Col);
			// Initialise with a given colour definition

	DocColour(ColourValue Red, ColourValue Green, ColourValue Blue);
			// Initialise with RGB values (in the range 0.0-1.0)

	DocColour(const DocColour &other);
	DocColour& operator=(const DocColour&);
			// Copy constructor, assignment operator

public:		// Linking to IndexedColours
	void MakeRefToIndexedColour(IndexedColour *Other);
			// Link this DocColour to a given IndexedColour parent

	inline IndexedColour *FindParentIndexedColour(void);
			// Get a pointer (or NULL if none) to this colour's IndexedColour parent

#ifdef _DEBUG
			// Special function to provide extra tracing for Jason to detect where
			// "IndexedColour in use" problems originate. Only used in debug builds
#define MakeRefToIndexedColour(Bob) MakeRefToIndexedColourDBG(Bob, THIS_FILE, __LINE__)
#endif

	void MakeRefToIndexedColourDBG(IndexedColour *Other, LPCSTR TheFile = NULL, INT32 TheLine = 0);
			// Special debug version of MakeRefToIndexedColour

	IndexedColour *GetSpotParent(void);
			// Get the ultimate spot colour parent of this DocColour, if any


public:		// Comparison operators
	BOOL operator==(const DocColour) const;
	BOOL operator!=(const DocColour) const;


public:		// Interface
	ColourModel GetColourModel(void) const;
			// Get the colour model in which this colour is defined

	void GetSourceColour(ColourGeneric *Result);
			// Read the original colour definition in the GetColourModel() colour model

	inline BOOL IsTransparent(void) const;
			// Returns TRUE if this is "no colour"

	inline BOOL IsSeparable(void) const;
	void SetSeparable(BOOL IsSeparable = TRUE);
			// Determine/set flag to enable/disable separation and colour correction

	void Mix(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
				ColourContext *BlendContext, BOOL BlendTheLongWay = FALSE,
				ColourContext *OutputContext = NULL);
			// Redefine this colour as a blend/mix of two other colours

	void ForceRounding(BOOL RoundingOn) { Info.ForceRounding = RoundingOn; };
	BOOL IsForcedToRound(void) { return(Info.ForceRounding); };
			// Bodge for PANTONE colours - When Rounding is in effect, GetSourceColour will round all
			// returned components to the nearest half percent, which corrects the errors introduced
			// by using 0..255 values to represent the 200 possible half-percent values PANTONE use.
			// DO NOT USE THESE FUNCTIONS unless you know what you're doing


	// new functions to access the reserved flag
	void SetReservedFlag(UINT32 Value);
	UINT32 GetReservedFlag();

	BOOL IsNamed();
	void HackColReplacerPreDestruct();

protected:		// Internal helper functions for RGB and HSV mixing
	void MixRGB(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
				ColourContext *BlendContext);
			// Redefine this colour as a blend/mix of two other colours

	void MixCMYK(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
				ColourContext *BlendContext);
			// Redefine this colour as a blend/mix of two other colours

	void MixHSV(DocColour *BlendStart, DocColour *BlendEnd, double BlendFraction,
				ColourContext *BlendContext, BOOL BlendTheLongWay = FALSE);
			// Redefine this colour as a blend/mix of two other colours

	void MixTint(IndexedColour *SpotParent, IndexedColour *StartTint,
					IndexedColour *EndTint, double BlendFraction);
			// Does a linear interpolation for tints, generating new local IndexedColours
			// to represent the resulting tint/shade values. Must only be used sparingly,
			// to avoid making millions of new local colours


public:		// Debug helpers
	void GetDebugDetails(StringBase* Str);	// Appends details of self to Str
											// e.g. Str = "GradFill"
											// GetDebugDetails(&Str);
											// gives: "GradFill colour = (...)"


public:		// Old interface - do not use these calls any more!
	void SetRGBValue(INT32 Red, INT32 Green, INT32 Blue);
	void GetRGBValue(INT32* Red, INT32* Green, INT32* Blue);

	void SetCMYKValue(PColourCMYK *New); 
	void GetCMYKValue(PColourCMYK *Result);
	void GetCMYKValue(ColourContext* pContext, PColourCMYK *Result);

	void SetHSVValue(INT32 h, INT32 s, INT32 v);
	void GetHSVValue(INT32* h, INT32* s, INT32* v);

	DocColour(INT32 Red, INT32 Green, INT32 Blue);


protected:
	ColourInfo		Info;
	ColourPacked	SourceColour;
	ColourPacked	CachedColour;


protected:
	void InitialiseInfoField(ColourModel ColModel);
};




/********************************************************************************************

<	DOCCOLOUR_RGBT(col)
<	DOCCOLOUR_HSVT(col)
<	DOCCOLOUR_CMYK(col)
<	DOCCOLOUR_CIET(col)

	Comment:	These macros simplify construction of DocColour objects in
				given colour models. Use something like this:

				{

					ColourRGBT TheDefinition;

					DocColour  TheColour = DOCCOLOURRGBT(&TheDefinition);

				}

	Notes:		Note that these macros explicitly cast 'col' to a
				(ColourGeneric *), so if it is not a pointer to a 128-bit
				colour structure (ColourGeneric, ColourRGBT, ColourCIET etc)
				then nasty things may occur.

	SeeAlso:	DocColour::DocColour

********************************************************************************************/

#define DOCCOLOUR_RGBT(col) DocColour(COLOURMODEL_RGBT, (ColourGeneric *) col)
#define DOCCOLOUR_HSVT(col) DocColour(COLOURMODEL_HSVT, (ColourGeneric *) col)
#define DOCCOLOUR_CMYK(col) DocColour(COLOURMODEL_CMYK, (ColourGeneric *) col)
#define DOCCOLOUR_CIET(col) DocColour(COLOURMODEL_CIET, (ColourGeneric *) col)





/********************************************************************************************

>	inline BOOL DocColour::IsTransparent() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/04/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the colour is "no colour" ("transparent").
	Purpose:	Determines if a colour's "no colour" flag is set
	SeeAlso:	-

********************************************************************************************/

inline BOOL DocColour::IsTransparent(void) const
{
	return(Info.NoColour);
}

/********************************************************************************************

>	inline BOOL DocColour::IsSeparable() const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/5/96

	Returns:	TRUE if the colour is allowed to be colour separated and/or corrected
				during colour conversions.

	Purpose:	Determines if a colour's "IsSeparable" flag is set
	SeeAlso:	DocColour::SetSeparable

********************************************************************************************/

inline BOOL DocColour::IsSeparable(void) const
{
	return(Info.IsSeparable);
}



/********************************************************************************************

>	inline IndexedColour *DocColour::FindParentIndexedColour(void);

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/94
	Inputs:		-
	Outputs:	-
	Returns:	NULL if this is an immediate colour
				else a pointer to the IndexedColour which this DocColour references
	Purpose:	Determines if a colour references an IndexedColour, and if so, gives
				access to the parent colour.
	SeeAlso:	DocColour::MakeRefToIndexedColour

********************************************************************************************/

inline IndexedColour *DocColour::FindParentIndexedColour(void)
{
	if (Info.SourceColourModel != COLOURMODEL_INDEXED)
		return(NULL);

	return(SourceColour.Indexed.Colour);
}

#endif		// INC_DOCCOLOR

