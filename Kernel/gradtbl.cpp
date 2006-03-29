// $Id: gradtbl.cpp 662 2006-03-14 21:31:49Z alex $
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

// Classes to implement the graduation tables required for special path filling attributes.

/*
*/

#include "camtypes.h"

#include "app.h"
#include "colormgr.h"
#include "colplate.h"
#include "doccolor.h"
#include "devcolor.h"
#include "grndrgn.h"
#include "gradtbl.h"
//#include "xaracms.h"
#include "fixmem.h"
#include "fillramp.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(GradTable, CCObject)
CC_IMPLEMENT_DYNCREATE(TranspGradTable, CCObject)
CC_IMPLEMENT_DYNCREATE(GradTable32, CCObject)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


/********************************************************************************************
 * NOTE																						*
 * There are now 2 functions in this file which build graduation tables. The tables are		*
 * actually identical in meaning, but are encoded as (a) Gavin 8bpp dither patterns, and	*
 * (b) RGBQUAD RGB structures or COLORREF RGB values - this is because the different		*
 * places that use these tables demand the different formats, and converting all the code	*
 * to use only one table format is non-trivial and/or inefficient.							*
 *******************************************************************************************/


// FillStep
// This constant is used when filling grad tables for COLOURPLATE_COMPOSITE modes.
//
// Each ConvertColour is mindbogglingly slow (over 52 multiplies) so to get some
// speed without too much loss of quality, we calculate proper values for every
// 16th entry of the table, and linearly interpolate between them. Note that we can't
// increase the step size beyond about 16, or rainbow fills will start going wrong.
// 
// Decreasing this constant to 1 will slow down Composite preview rendering an awful lot, 
// but may slightly improve display quality.
//
const DWORD FillStep = 16;		// Number of interpolated colours between proper samples



/********************************************************************************************

>	GradTable::GradTable(BOOL LargeTable = FALSE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Purpose:	Default constructor for a GradTable

	Notes:		Sets the default table size to 0x100 entries long

	SeeAlso:	GradTable::SetTableSize

********************************************************************************************/

GradTable::GradTable(BOOL LargeTable)
{
	m_bLargeTable = LargeTable;
	m_pTable = NULL;
}



/********************************************************************************************

>	GradTable::~GradTable()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Purpose:	Destructor for a GradTable

********************************************************************************************/

GradTable::~GradTable()
{
	if (m_pTable != NULL)
	{
		CCFree(m_pTable);
		m_pTable = NULL;
	}
}



/********************************************************************************************

>	INT32 GradTable::GetTableSize(GDrawContext* GD, BOOL LargeTable)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96
	Inputs:		GD - pointer to GDraw context to use
				LargeTable - if true then use a long grad table
	Returns:	The size in bytes of a grad table for the specified context

********************************************************************************************/

INT32 GradTable::GetTableSize(GDrawContext* GD, BOOL LargeTable)
{
	if (LargeTable)
	{
		return(GD->GetLongGraduationTableSize());
	}
	else
	{
		return(GD->GetGraduationTableSize());
	}
}



/********************************************************************************************

>	INT32 GradTable::GetTableLength(GDrawContext* GD, BOOL LargeTable)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96
	Inputs:		GD - pointer to GDraw context to use
				LargeTable - if true then use a long grad table
	Returns:	The number of entries in a grad table for the specified context

********************************************************************************************/

INT32 GradTable::GetTableLength(GDrawContext* GD, BOOL LargeTable)
{
	if (LargeTable)
	{
		return(GD->GetLongGraduationTableLength());
	}
	else
	{
		return(GD->GetGraduationTableLength());
	}
}


/********************************************************************************************

>	void GradTable::SetTableSize(BOOL LargeTable = FALSE)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96

	Inputs:		LargeTable - if true then use a long grad table

	Purpose:	To set the size of this Grad table

	Notes:		The table defaults to small

********************************************************************************************/

void GradTable::SetTableSize(BOOL LargeTable)
{
	// If we don't have the right table size or no table at all
	if ((LargeTable != m_bLargeTable) || (m_pTable == NULL))
	{
		m_bLargeTable = LargeTable;

		if (m_pTable != NULL)
		{
			CCFree(m_pTable);
			m_pTable = NULL;
		}

		GDrawContext *GD = GRenderRegion::GetStaticDrawContext();

		if (GD != NULL)
		{
			INT32 Size = GetTableSize(GD, m_bLargeTable);
			
			m_pTable = (GraduationTable*) CCMalloc(Size);

			if (m_pTable != NULL)
			{
				m_pTable->Length = GetTableLength(GD, LargeTable);
			}
		}
	}
}



/********************************************************************************************

>	BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol,
								View *ContextView, EFFECTTYPE Effect,
								DWORD FirstEntry, DWORD LastEntry)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Will)
	Created:	14/5/96 (14/9/94)

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				TableSize	- The number of entries you wish to make the table (0x100 or
							  0x800)

	Returns:	FALSE if it fails

	Purpose:	Builds a GradTable from StartCol to EndCol, filling in the entire
				table with appropriate GDraw dither patterns.
				
				== NOTE that this version of the call also implicitly sets the table size! == 

	SeeAlso:	GradTable::SetTableSize

********************************************************************************************/
                                                                                 
BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol,
							View *ContextView, EFFECTTYPE Effect,
							BOOL LargeTable)
{
	SetTableSize(LargeTable);

	return(BuildTable(StartCol, EndCol, ContextView, Effect, 0, m_pTable->Length));
}



/********************************************************************************************

>	void GradTable::FillTable(DWORD StartIndex, DWORD EndIndex,
							PColourRGBT *StartDef, PColourRGBT *EndDef)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/96

	Inputs:		StartIndex, EndIndex - Define the indices into the grad fill table
				between which you want to fill in the table. NOTE that Table[StartIndex]
				is filled in, while table[EndIndex] is NOT.
				StartDef, EndDef - Define the end colours of the graduation

	Purpose:	Builds a GradTable from StartIndex to EndIndex by linearly interpolating
				between the StartDef and EndDef RGB colours.

	Notes:		In order to correctly handle Rainbow fills, you must interpolate only small
				sections of the table - it is recommended that you fill no more than 16
				entries of the table at a time with this linear interpolation.

********************************************************************************************/

void GradTable::FillTable(DWORD StartIndex, DWORD EndIndex,
							PColourRGBT *StartDef, PColourRGBT *EndDef)
{
	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();

	// Place the StartDef in as the first entry
	GD->AddToGraduationTable(RGB(StartDef->Red, StartDef->Green, StartDef->Blue),
								FALSE, m_pTable, StartIndex);

	// ...and start interpolating from the second entry
	DWORD MixStep = (1 << 24) / (EndIndex - StartIndex);
	DWORD MixFraction = MixStep;

	DWORD R,G,B;
	for (DWORD Index = StartIndex + 1; Index < EndIndex; Index++)
	{
		// Calculate intermediate RGB values into 8bpp values
		R = (StartDef->Red * (0x01000000 - MixFraction)) + (EndDef->Red * MixFraction);
		R >>= 24;

		G = (StartDef->Green * (0x01000000 - MixFraction)) + (EndDef->Green * MixFraction);
		G >>= 24;

		B = (StartDef->Blue * (0x01000000 - MixFraction)) + (EndDef->Blue * MixFraction);
		B >>= 24;

		// Get Gavin to fill in the dither entry for us (FALSE means it's an RGB Indexour)
		GD->AddToGraduationTable(RGB(R,G,B), FALSE, m_pTable, Index);

		MixFraction += MixStep;
	}
}



/********************************************************************************************

>	BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol,
								View *ContextView, EFFECTTYPE Effect,
								DWORD FirstEntry, DWORD LastEntry)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Will)
	Created:	14/5/96 (14/9/94)

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

	Returns:	FALSE if it fails

	Purpose:	Builds a GradTable from StartCol to EndCol, filling in a portion of the
				table with appropriate GDraw dither patterns

	Notes:		GDraw requires the Start and End colours for the entire table to be
				set separately in the table structure. These will be set if FirstEntry==0
				and/or if LastEntry==TableSize, respectively.

				No longer calls GDraw to build the table. The colours are generated
				by us (mainly so that we can support proper colour correction and
				separation by allowing the colour system to do its stuff)

				This method could be somewhat more optimal than it is, if we start
				poking around more in the internals of the colour system. For the time
				being, I'm leaving it tidy, however.

	Errors:		ERROR2's if it runs out of memory

	SeeAlso:	GradTable::SetTableSize

********************************************************************************************/
                                                                                 
BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol,
							View *ContextView, EFFECTTYPE Effect,
							DWORD FirstEntry, DWORD LastEntry)
{
	// The range should be within the table size. We only ERROR3, though, because
	// the physical table is always large enough for the maximum number of entries.

	ERROR3IF(FirstEntry < 0 || FirstEntry > LastEntry || LastEntry > m_pTable->Length,
				"GradTable::BuildTable expects sensible First/Last Entry values");

	// Find a GDraw context to generate dithertable entries for us
	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();
	ERROR3IF(GD == NULL, "GradTable::BuildTable - No GDraw context?!");

	// Determine if either the start or end colour is a spot colour - if it is, we must
	// coerce the fill into a "mix" (rgb) rather than rainbow fill style
	if (Effect != EFFECT_RGB && (StartCol.GetSpotParent() != NULL || EndCol.GetSpotParent() != NULL))
		Effect = EFFECT_RGB;

	// Get colour contexts: we need 2 - first, an RGB context for the ContextView, which
	// will be used to get colour-corrected/separated/etc output colour in RGB; and second,
	// a generic RGB or HSV context to provide a "mixing colourspace" for the graduations.
	ColourContext *cc = NULL;
	ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);

	if (Effect == EFFECT_RGB)
		cc = ColourManager::GetColourContext(COLOURMODEL_RGBT);		// RGB fade
	else
		cc = ColourManager::GetColourContext(COLOURMODEL_HSVT);		// HSV fade

	// If we can't find RGB/HSV contexts, something is seriously broken
	ERROR3IF(cc == NULL || ccRGB == NULL,
			"GradTable::BuildTable - Can't find my colour context!");

	// Fill in the Start and End colours of the table, if we've hit them
	PColourRGBT Result;

	if (FirstEntry == 0)
	{
		ccRGB->ConvertColour(&StartCol, (ColourPacked *) &Result);
		m_pTable->StartColour = RGB(Result.Red, Result.Green, Result.Blue);
	}

	if (LastEntry >= m_pTable->Length)
	{
		LastEntry = m_pTable->Length;
		ccRGB->ConvertColour(&EndCol, (ColourPacked *) &Result);
		m_pTable->EndColour = RGB(Result.Red, Result.Green, Result.Blue);
	}

	const BOOL ValidColourPlate = (ccRGB->GetColourPlate() != NULL && !ccRGB->GetColourPlate()->IsDisabled());

	if (ValidColourPlate || 
		FirstEntry > 0 || 
		LastEntry < m_pTable->Length)
	{
		if (ValidColourPlate && ccRGB->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE)
		{
			// We're doing a composite print preview.
			PColourRGBT StartDef;
			PColourRGBT EndDef;
			ccRGB->ConvertColour(&StartCol, (ColourPacked *) &EndDef);

			DWORD StartIndex = FirstEntry;
			DWORD EndIndex;

			DocColour TempCol;

			while (StartIndex < LastEntry)
			{
				// Move the last run's end colour into Start
				memcpy(&StartDef, &EndDef, sizeof(PColourRGBT));

				// Calculate the end of this run
				EndIndex = StartIndex + FillStep;
				if (EndIndex > LastEntry)
					EndIndex = LastEntry;
	
				// Calculate the colour at the end of this run, and convert into EndDef
				TempCol.Mix(&StartCol, &EndCol,
							((double)(EndIndex - FirstEntry)) / ((double)(LastEntry - FirstEntry)),
							cc, (Effect == EFFECT_HSV_LONG), ccRGB);

				ccRGB->ConvertColour(&TempCol, (ColourPacked *) &EndDef);

				// Linearly interpolate all colours in the run
				FillTable(StartIndex, EndIndex, &StartDef, &EndDef);

				// And step on to the next run
				StartIndex = EndIndex;
			}
		}
		else
		{
			// We have some type of colour separation on the go - we must convert all intermediate
			// colours through the colour system in order to ensure the results are correct.
			// This is quite slow, but acceptable (ish) when printing.
			double MixFraction	= 0.0;
			// This next line requires the -1 because otherwise it doesn't reach the end colour
			// This is ok if LastEntry = StartEntry + 1 as MixStep will be calculated as zero
			// and the StartCol will be used to fill in the single entry required
			double denumerator = (double) (LastEntry - FirstEntry - 1);
			double MixStep = 0;
			if (denumerator != 0)
			{
				MixStep = 1.0 / denumerator;
			}

			DocColour temp;
			for (DWORD col = FirstEntry; col < LastEntry; col++)
			{
				// Create a new colour which is an appropriate mixture of the start/end colours
				temp.Mix(&StartCol, &EndCol, MixFraction, cc, (Effect == EFFECT_HSV_LONG), ccRGB);

				// Convert back to the raw RGB and place it in the palette.
				// Note that this will colour correct and separate in an appropriate manner.
				// Note also that we're using a special ConvertColour call which returns 8-bit 
				// "packed" component values instead of FIXED24's.
				ccRGB->ConvertColour(&temp, (ColourPacked *) &Result);

				// Get GDraw to fill in the dither entry for us (FALSE means it's an RGB colour)
				GD->AddToGraduationTable(RGB(Result.Red, Result.Green, Result.Blue), FALSE, m_pTable, col);

				// And step to the next mix fraction value
				MixFraction += MixStep;
			}
		}
	}
	else
	{
		// We're doing a completely normal view, and building the entire 256-entry table, so
		// call GDraw to blindly interpolate between the colours, which is much (much) faster.
		GD->BuildGraduationTable(m_pTable->StartColour, m_pTable->EndColour, Effect, m_pTable);
	} 

	return(TRUE);
}



/********************************************************************************************

>	BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol,
							   View *ContextView, EFFECTTYPE Effect,
							   CProfileBiasGain& BiasGain, BOOL LargeTable)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/2000
	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				pColList	- Define the colours and positions of a colour ramp
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				BiasGain	- The baisgain values that are to alter the fill table

				TableSize	- The number of entries you wish to make the table (0x100 or
							  0x800)
	Returns:	TRUE, error thrown otherwise (but only for input variables)
	Purpose:	Generates a graduated fill table that is modified by biasgain values.
	SeeAlso:	-

********************************************************************************************/

BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol,
							View *ContextView, EFFECTTYPE Effect,
							CProfileBiasGain& BiasGain, BOOL LargeTable)
{	
	// Find a GDraw context to generate dithertable entries for us
	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();
	ERROR3IF(GD == NULL, "GradTable::BuildTable - No GDraw context?!");

	// Determine if either the start or end colour is a spot colour - if it is, we must
	// coerce the fill into a "mix" (rgb) rather than rainbow fill style
	if (Effect != EFFECT_RGB && (StartCol.GetSpotParent() != NULL || EndCol.GetSpotParent() != NULL))
		Effect = EFFECT_RGB;

	// Get colour contexts: we need 2 - first, an RGB context for the ContextView, which
	// will be used to get colour-corrected/separated/etc output colour in RGB; and second,
	// a generic RGB or HSV context to provide a "mixing colourspace" for the graduations.
	ColourContext *cc = NULL;
	ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);

	if (Effect == EFFECT_RGB)
		cc = ColourManager::GetColourContext(COLOURMODEL_RGBT);		// RGB fade
	else
		cc = ColourManager::GetColourContext(COLOURMODEL_HSVT);		// HSV fade

	// If we can't find RGB/HSV contexts, something is seriously broken
	ERROR3IF(cc == NULL || ccRGB == NULL,
			"GradTable::BuildTable - Can't find my colour context!");

	// Fill in the Start and End colours of the table, if we've hit them
	PColourRGBT Result;

	// set our table size for this grad
	SetTableSize(LargeTable);

	ccRGB->ConvertColour(&StartCol, (ColourPacked *) &Result);
	m_pTable->StartColour = RGB(Result.Red, Result.Green, Result.Blue);

	ccRGB->ConvertColour(&EndCol, (ColourPacked *) &Result);
	m_pTable->EndColour = RGB(Result.Red, Result.Green, Result.Blue);

	BiasGain.SetIntervals (AFp (0), AFp (m_pTable->Length));

	const INT32 TableLength = m_pTable->Length;
	double MixFraction;
	DocColour temp;
	INT32 r, g, b;
	double res;

	for (INT32 col = 0; col < TableLength; col++)
	{
		res = BiasGain.MapInterval( AFp( col ) );
		
		MixFraction = res / TableLength;
		
		// Create a new colour which is an appropriate mixture of the start/end colours
		temp.Mix(&StartCol, &EndCol, MixFraction, cc, (Effect == EFFECT_HSV_LONG), ccRGB);
		temp.GetRGBValue (&r, &g, &b);

		// Get GDraw to fill in the dither entry for us (FALSE means it's an RGB colour)
		GD->AddToGraduationTable(RGB(r, g, b), FALSE, m_pTable, col);
	}
	
	return (TRUE);
}



/********************************************************************************************

>	BOOL GradTable::BuildHighQualityRepeatTable(DocColour &StartCol, DocColour &EndCol, ColourRamp *pColourRamp,
												View *ContextView, EFFECTTYPE Effect)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/2000
	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				pColourRamp	- Define the colours and positions of a colour ramp
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

	Returns:	TRUE, error thrown otherwise (but only for input variables)
	Purpose:	Generates a high quality repeating graduated fill table.  Handles both
				those with fill ramps and those without.
	SeeAlso:	-

********************************************************************************************/

BOOL GradTable::BuildHighQualityRepeatTable(DocColour &StartCol, DocColour &EndCol, ColourRamp *pColourRamp,
					View *ContextView, EFFECTTYPE Effect)
{
	if (!pColourRamp)
	{
		// Find a GDraw context to generate dithertable entries for us
		GDrawContext *GD = GRenderRegion::GetStaticDrawContext();
		ERROR3IF(GD == NULL, "GradTable::BuildHighQualityRepeatTable - No GDraw context?!");

		// Determine if either the start or end colour is a spot colour - if it is, we must
		// coerce the fill into a "mix" (rgb) rather than rainbow fill style
		if (Effect != EFFECT_RGB && (StartCol.GetSpotParent() != NULL || EndCol.GetSpotParent() != NULL))
			Effect = EFFECT_RGB;

		// Get colour contexts: we need 2 - first, an RGB context for the ContextView, which
		// will be used to get colour-corrected/separated/etc output colour in RGB; and second,
		// a generic RGB or HSV context to provide a "mixing colourspace" for the graduations.
		ColourContext *cc = NULL;
		ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);

		if (Effect == EFFECT_RGB)
			cc = ColourManager::GetColourContext(COLOURMODEL_RGBT);		// RGB fade
		else
			cc = ColourManager::GetColourContext(COLOURMODEL_HSVT);		// HSV fade

		// If we can't find RGB/HSV contexts, something is seriously broken
		ERROR3IF(cc == NULL || ccRGB == NULL,
				"GradTable::BuildHighQualityRepeatTable - Can't find my colour context!");

		// Fill in the Start and End colours of the table, if we've hit them
		PColourRGBT Result;

		// set our table size for this grad
		SetTableSize(TRUE);

		ccRGB->ConvertColour(&StartCol, (ColourPacked *) &Result);
		ccRGB->ConvertColour(&  EndCol, (ColourPacked *) &Result);
		m_pTable->StartColour = RGB(Result.Red, Result.Green, Result.Blue);
		m_pTable->EndColour   = RGB(Result.Red, Result.Green, Result.Blue);

		const INT32 TableLength = m_pTable->Length;
		double MixFraction = 0.0;
		double MixStep = 0;
				
		if (TableLength != 0)
			MixStep = 2.0 / TableLength;

		DocColour temp;
		INT32 r, g, b;

		// NOTE:  this loop could be optimised to avoid calculating the same colour value twice

		for (INT32 col = 0 ; (col<<1)<TableLength ; col++)
		{	
			// Create a new colour which is an appropriate mixture of the start/end colours
			temp.Mix(&StartCol, &EndCol, MixFraction, cc, (Effect == EFFECT_HSV_LONG), ccRGB);
			temp.GetRGBValue (&r, &g, &b);

			// Get GDraw to fill in the dither entry for us (FALSE means it's an RGB colour)
			GD->AddToGraduationTable(RGB(r, g, b), FALSE, m_pTable, col);
			GD->AddToGraduationTable(RGB(r, g, b), FALSE, m_pTable, TableLength-col-1);

			MixFraction += MixStep;
		}
	}
	else
	{
		// set our table size for this grad
		SetTableSize(TRUE);
		
		ColRampItem *pCTI = pColourRamp->GetFirstCol();
		if (pCTI!=NULL)
		{
			DocColour *pFirstCol=&StartCol, *pSecondCol;
			
			DWORD fi=0 , si=0;
			
			while (pCTI)
			{
				// whip out the colour definition
				pSecondCol = pCTI->GetColourAddr();
				// calculate the index for the next colour
				si = (DWORD)((pCTI->GetPosition() * m_pTable->Length)+0.5);
				// build this section of the table
				if (!BuildTable(*pFirstCol, *pSecondCol, ContextView, Effect, fi, si))
					return FALSE;
				// now skip round to the next				
				pFirstCol = pSecondCol;
				fi = si;
				pCTI = pColourRamp->GetNextCol(pCTI);
			}

			// do the final section
			return BuildTable(*pSecondCol, StartCol, ContextView, Effect, si, m_pTable->Length);
		}				
	}
	
	return (TRUE);
}

/********************************************************************************************

>	BOOL GradTable::BuildHighQualityRepeatTable(DocColour &StartCol, DocColour &EndCol,
												View *ContextView, EFFECTTYPE Effect,
												CProfileBiasGain& BiasGain)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/2000
	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				pColourRamp	- Define the colours and positions of a colour ramp
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				BiasGain	- the profile to build for

	Returns:	TRUE, error thrown otherwise (but only for input variables)
	Purpose:	Generates a high quality repeating graduated fill table for the supplied profile.
	SeeAlso:	-

********************************************************************************************/

BOOL GradTable::BuildHighQualityRepeatTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					CProfileBiasGain& BiasGain)
{
	// Find a GDraw context to generate dithertable entries for us
	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();
	ERROR3IF(GD == NULL, "GradTable::BuildHighQualityRepeatTable - No GDraw context?!");

	// Determine if either the start or end colour is a spot colour - if it is, we must
	// coerce the fill into a "mix" (rgb) rather than rainbow fill style
	if (Effect != EFFECT_RGB && (StartCol.GetSpotParent() != NULL || EndCol.GetSpotParent() != NULL))
		Effect = EFFECT_RGB;

	// Get colour contexts: we need 2 - first, an RGB context for the ContextView, which
	// will be used to get colour-corrected/separated/etc output colour in RGB; and second,
	// a generic RGB or HSV context to provide a "mixing colourspace" for the graduations.
	ColourContext *cc = NULL;
	ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);

	if (Effect == EFFECT_RGB)
		cc = ColourManager::GetColourContext(COLOURMODEL_RGBT);		// RGB fade
	else
		cc = ColourManager::GetColourContext(COLOURMODEL_HSVT);		// HSV fade

	// If we can't find RGB/HSV contexts, something is seriously broken
	ERROR3IF(cc == NULL || ccRGB == NULL,
			"GradTable::BuildHighQualityRepeatTable - Can't find my colour context!");

	// Fill in the Start and End colours of the table, if we've hit them
	PColourRGBT Result;

	// set our table size for this grad
	SetTableSize(TRUE);

	ccRGB->ConvertColour(&StartCol, (ColourPacked *) &Result);
	m_pTable->StartColour = RGB(Result.Red, Result.Green, Result.Blue);

	ccRGB->ConvertColour(&EndCol, (ColourPacked *) &Result);
	m_pTable->EndColour = RGB(Result.Red, Result.Green, Result.Blue);

	const INT32 TableLength = m_pTable->Length;
	INT32 halfTableLength = TableLength/2;
	double MixFraction;

	BiasGain.SetIntervals (AFp (0), AFp (m_pTable->Length/2));
	
	DocColour temp;
	INT32 r, g, b;
	double res;

	BOOL secondHalf = FALSE;
	INT32 secondHalfVal =0;

	// NOTE:  this loop could be optimised to avoid calculating the same colour value twice

	for (INT32 col = 0; col < TableLength; col++)
	{	
		if (!secondHalf)
		{
			res = BiasGain.MapInterval( AFp( col) );
		}
		else
		{
			res = BiasGain.MapInterval( AFp( secondHalfVal) );
		}
		
		MixFraction = res / halfTableLength;
		
		// Create a new colour which is an appropriate mixture of the start/end colours
		temp.Mix(&StartCol, &EndCol, MixFraction, cc, (Effect == EFFECT_HSV_LONG), ccRGB);
		temp.GetRGBValue (&r, &g, &b);

		// Get GDraw to fill in the dither entry for us (FALSE means it's an RGB colour)
		GD->AddToGraduationTable(RGB(r, g, b), FALSE, m_pTable, col);

		if (!(col < halfTableLength))
		{
			secondHalfVal = TableLength - col;
			secondHalf = TRUE;
		}
	}
	
	return (TRUE);
}



/********************************************************************************************

>	BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol, ColourRamp *pColList,
								View *ContextView, EFFECTTYPE Effect,
								BOOL LargeTable)

 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/97
	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				pColList	- Define the colours and positions of a colour ramp
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				TableSize	- The number of entries you wish to make the table (0x100 or
							  0x800)
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes a colour ramp fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL GradTable::BuildTable(DocColour &StartCol, DocColour &EndCol, ColourRamp *pColRamp,
							View *ContextView, EFFECTTYPE Effect,
							BOOL LargeTable)
{
	// set our table size for this grad
	SetTableSize(LargeTable);
	
	// if we've been passed a colour table, lets use it
	if (pColRamp!=NULL)
	{
		ColRampItem *pCTI = pColRamp->GetFirstCol();
		if (pCTI!=NULL)
		{
			DocColour *pFirstCol=&StartCol, *pSecondCol;
			
			DWORD fi=0 , si=0;
			
			while (pCTI)
			{
				// whip out the colour definition
				pSecondCol = pCTI->GetColourAddr();
				// calculate the index for the next colour
				si = (DWORD)((pCTI->GetPosition() * m_pTable->Length)+0.5);
				// build this section of the table
				if (!BuildTable(*pFirstCol, *pSecondCol, ContextView, Effect, fi, si))
					return FALSE;
				// now skip round to the next				
				pFirstCol = pSecondCol;
				fi = si;
				pCTI = pColRamp->GetNextCol(pCTI);
			}

			// do the final section
			return BuildTable(*pSecondCol, EndCol, ContextView, Effect, si, m_pTable->Length);
		}				
	}

	// otherwise do the normal double end linear fill.
	return (BuildTable(StartCol, EndCol, ContextView, Effect, 0, m_pTable->Length));
}


/********************************************************************************************

>	BOOL GradTable::AddToTable(DocColour &Colour, ColourContext* CurrentColContext, 
							BOOL HSVFlag, INT32 Index)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94

	Inputs:		NewCol - The colour to place into the GradTable
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Index - The index of the table entry to overwrite

	Purpose:	Adds a colour to a GradTable, at the specified index
				This is used for multi-stage fills.

	Errors:		-

********************************************************************************************/

BOOL GradTable::AddToTable(DocColour &NewCol, View *ContextView, DWORD Index)
{
	ColourContextRGBT *cc = (ColourContextRGBT *)
							ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);

	ERROR3IF(cc == NULL, "Unable to find an RGB colour context for the view");
	ERROR3IF(Index >= m_pTable->Length, "Illegal table index");

	PColourRGBT Result;
	cc->ConvertColour(&NewCol, (ColourPacked *) &Result);

	COLORREF Col = RGB(Result.Red, Result.Green, Result.Blue);

	// Get GDraw to fill in the table entry for us. FALSE => RGB colour
	return(GRenderRegion::GetStaticDrawContext()->AddToGraduationTable(Col, FALSE, m_pTable, Index));
}





/********************************************************************************************

>	TranspGradTable::TranspGradTable(BOOL LargeTable = FALSE)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96
	Purpose:	Default constructor for a GradTable
	Errors:		-

********************************************************************************************/

TranspGradTable::TranspGradTable(BOOL LargeTable)
{
	m_pTable = NULL;
	m_bLargeTable = LargeTable;
}

/********************************************************************************************

>	TranspGradTable::~TranspGradTable()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Purpose:	Destructor for a TranspGradTable

********************************************************************************************/

TranspGradTable::~TranspGradTable()
{
	if (m_pTable != NULL)
	{
		CCFree(m_pTable);
		m_pTable = NULL;
	}
}


/********************************************************************************************

>	INT32 TranspGradTable::GetTableSize(GDrawContext* GD, BOOL LargeTable)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96
	Inputs:		GD - pointer to GDraw context to use
				LargeTable - if true then use a long grad table
	Returns:	The size in bytes of a grad table for the specified context

********************************************************************************************/

INT32 TranspGradTable::GetTableSize(GDrawContext* GD, BOOL LargeTable)
{
	if (LargeTable)
	{
		return(GD->GetLongTransparentGraduationTableSize());
	}
	else
	{
		return(GD->GetTransparentGraduationTableSize());
	}
}


/********************************************************************************************

>	INT32 TranspGradTable::GetTableLength(GDrawContext* GD, BOOL LargeTable)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96
	Inputs:		GD - pointer to GDraw context to use
				LargeTable - if true then use a long grad table
	Returns:	The number of entries in a grad table for the specified context

********************************************************************************************/

INT32 TranspGradTable::GetTableLength(GDrawContext* GD, BOOL LargeTable)
{
	if (LargeTable)
	{
		return(GD->GetLongTransparentGraduationTableLength());
	}
	else
	{
		return(GD->GetTransparentGraduationTableLength());
	}
}


/********************************************************************************************

>	void TranspGradTable::SetTableSize(BOOL LargeTable = FALSE)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/96

	Inputs:		LargeTable - if true then use a long grad table

	Purpose:	To set the size of this Grad table

	Notes:		The table defaults to small

********************************************************************************************/

void TranspGradTable::SetTableSize(BOOL LargeTable)
{
	// If we don't have the right table size or no table at all
	if ((LargeTable != m_bLargeTable) || (m_pTable == NULL))
	{
		m_bLargeTable = LargeTable;

		if (m_pTable != NULL)
		{
			CCFree(m_pTable);
			m_pTable = NULL;
		}

		GDrawContext *GD = GRenderRegion::GetStaticDrawContext();

		if (GD != NULL)
		{
			INT32 Size = GetTableSize(GD, m_bLargeTable);
			
			m_pTable = (TransparentGradTable*) CCMalloc(Size);
		}
	}
}



/********************************************************************************************

>	BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Inputs:		-
	Purpose:	Builds a TransparentGradTable from StartTransp to EndTransp.
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp)
{
	SetTableSize(m_bLargeTable);

	if (m_pTable == NULL)
		return(FALSE);

	GDrawContext *pContext = GRenderRegion::GetStaticDrawContext();

	// If we are a large table then build it ourself
	if (IsLarge())
	{
		const INT32 Length = GetTableLength(pContext, IsLarge());

		const INT32 TranspIncrement = ((EndTransp - StartTransp) << 24) / Length;

		INT32 Transp = StartTransp;
		INT32 Index;

		for (Index = 0; Index < Length; Index++)
		{
			m_pTable->Table[Index] = (Transp >> 24) & 0xFF;
			Transp += TranspIncrement;
		}

		return(TRUE);
	}
	else
	{
		// Otherwise get a GDrawContext to build it
		return(pContext->BuildTransparencyTable(StartTransp, EndTransp, m_pTable));
	}
}



/********************************************************************************************

>	BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/2000
	Inputs:		-
	Purpose:	Builds a TransparentGradTable from StartTransp to EndTransp that reflects
				the supplied BiasGain values.
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain)
{
	SetTableSize(FALSE);

	if (m_pTable == NULL)
		return(FALSE);

	GDrawContext *pContext = GRenderRegion::GetStaticDrawContext();

	const INT32 Length = GetTableLength(pContext, FALSE);
	const double RLength = 1.0/Length;
	BiasGain.SetIntervals (AFp (0), AFp (Length));

	double MixFraction;
	double temp;
	INT32 Index;

	for (Index = 0; Index < Length; Index++)
	{
		MixFraction = BiasGain.MapInterval( AFp( Index ) );
		MixFraction *= RLength;
		temp = StartTransp * (1-MixFraction);
		temp += (EndTransp * MixFraction);
		m_pTable->Table[Index] = static_cast<BYTE> ( temp );
	}

	return(TRUE);
}


/********************************************************************************************

>	BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/97
	Inputs:		StartTransp	 = the current start transparency level
				EndTransp	 = the current end transparency level
				pTranspTable = all the inbetween transparency table levels.
	Purpose:	Builds a TransparentGradTable from StartTransp to EndTransp with inbetween
				transparency levels held in pTranspTable
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable)
{
	TranspRampItem *pTTI=NULL;
	if (pTranspTable!=NULL)	
		pTTI=pTranspTable->GetFirstTransp();

	if (pTTI==NULL)
		return BuildTable(StartTransp,EndTransp);

	SetTableSize(m_bLargeTable);

	if (m_pTable == NULL)
		return FALSE;

	INT32 ei, si=0;
	INT32 end, start=StartTransp;
	
	GDrawContext *pContext = GRenderRegion::GetStaticDrawContext();
	const INT32 Length = GetTableLength(pContext, IsLarge());

	while (pTTI)
	{
		ei = (INT32)((pTTI->GetPosition() * (Length-1)) + 0.5);
		end = pTTI->GetTransparency();
		FillSection(si,ei,start,end);
		si = ei;
		start = end;
		pTTI = pTranspTable->GetNextTransp(pTTI);
	}
	ei  = Length-1;
	end = EndTransp;
	FillSection(si,ei,start,end);

	return TRUE;
}



/********************************************************************************************

>	BOOL TranspGradTable::BuildHighQualityRepeatTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/2000
	Inputs:		StartTransp	 = the current start transparency level
				EndTransp	 = the current end transparency level
				BiasGain	 = the profile to build for
	Purpose:	Builds a high quality repeating TransparentGradTable from StartTransp to
				EndTransp that reflects the supplied BiasGain values.
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildHighQualityRepeatTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain)
{
	SetTableSize(TRUE);

	if (m_pTable == NULL)
	{
		return FALSE;
	}

	GDrawContext *pContext = GRenderRegion::GetStaticDrawContext();

	INT32 TableLength = GetTableLength (pContext, IsLarge ());
	INT32 halfTableLength = TableLength/2;
		
	double MixFraction = 0.0;
	double temp;
	double res;
	INT32 Index;

	BOOL secondHalf = FALSE;
	INT32 secondHalfVal =0;

	BiasGain.SetIntervals (AFp (0), AFp (halfTableLength));

	for (Index = 0; Index < TableLength; Index++)
	{
		if (!secondHalf)
		{
			res = BiasGain.MapInterval( AFp( Index) );
		}
		else
		{
			res = BiasGain.MapInterval( AFp( secondHalfVal) );
		}

		MixFraction = res / (halfTableLength);
		temp = StartTransp * (1-MixFraction);
		temp += (EndTransp * MixFraction);
		m_pTable->Table[Index] = static_cast<BYTE> ( temp );
			
		if (!(Index < halfTableLength))
		{
			secondHalfVal = TableLength - Index;
			secondHalf = TRUE;
		}
	}

	return (TRUE);
}


/********************************************************************************************

>	BOOL TranspGradTable::BuildHighQualityRepeatTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/2000
	Inputs:		StartTransp	 = the current start transparency level
				EndTransp	 = the current end transparency level
				pTranspTable = the ramp to build for
	Purpose:	Builds a high quality repeating TransparentGradTable from StartTransp to
				EndTransp.
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildHighQualityRepeatTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable)
{
	if (!pTranspTable)
	{
		SetTableSize(TRUE);

		if (m_pTable == NULL)
			return FALSE;

		GDrawContext *pContext = GRenderRegion::GetStaticDrawContext();

		INT32 TableLength = GetTableSize (pContext, IsLarge ());
		
		double MixFraction = 0.0;
		double MixStep = 2.0/TableLength;
		double temp;

		for (INT32 Index = 0; (Index<<1) < TableLength; Index++)
		{
			//MixFraction = BiasGain.MapInterval( AFp( Index ) );
			//MixFraction /= Length;
			temp = StartTransp * (1-MixFraction);
			temp += (EndTransp * MixFraction);
			m_pTable->Table[Index] =
			m_pTable->Table[TableLength-Index-1] = static_cast<BYTE> ( temp );
			
			MixFraction += MixStep;
		}
	}
	else
	{
		ERROR3 ("Attempt to build a multi-stage transparent fill - NOT fully implemented yet!");
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL TranspGradTable::BuildTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com> (modified by CGS)
	Created:	24/10/96 (15/2/2000)
	Inputs:		-
	Purpose:	Builds a TransparentGradTable from StartTransp to EndTransp
				This function forces the table to be 256 entries (for bitmap transparency)
				The function now (also) takes into account the applied transparency profile.
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildBitmapTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain)
{
	if (m_pTable != NULL)
	{
		CCFree(m_pTable);
		m_pTable = NULL;
	}

	m_pTable = (TransparentGradTable*) CCMalloc(256);

	if (m_pTable == NULL)
		return(FALSE);

	const INT32 Length = 0x100;
	DWORD Index;

	CProfileBiasGain DefaultBiasGain;

	// make some optimisation decisions ....

	if (!(BiasGain == DefaultBiasGain))
	{
		double MixFraction;
		double temp;
		double RLength = 1.0/Length;
		
		BiasGain.SetIntervals (AFp (0), AFp (0x100));
		
		for (Index= 0; Index < 0x100; Index++)
		{
			MixFraction = BiasGain.MapInterval( AFp( Index ) );
			MixFraction *= RLength;
			
			temp = StartTransp * (1-MixFraction);
			temp += (EndTransp * MixFraction);

			m_pTable->Table[Index] = static_cast<BYTE> ( temp );
		}
		
		if (BiasGain.GetIsAFeatherProfile ())
		{
			// were a feather profile - we need to do a bit of extra work to ensure that ilans
			// feather renders correctly.  We MUST ensure that ilans white part of the bitmap
			// is rendered as 100% transparent.  The following lines of code do this ....

			// TODO: (Gavin) Why on earth is this setting Index-2 and Index-1 to 255
			// when the code below overwrites Index-2 anyway?
			m_pTable->Table[Index-2] = static_cast<BYTE> ( 255 );
			m_pTable->Table[Index-1] = static_cast<BYTE> ( 255 );
		}
	}
	else
	{
		INT32 Transp = (StartTransp << 24) + 0x00800000;
		INT32 TranspStep = (EndTransp - StartTransp) * 0x010101;
		BYTE *pTransp = &(m_pTable->Table[0]);
		
		for (Index= 0; Index < 0x100; Index++)
		{
			pTransp[Index] = (BYTE)((Transp >> 24) & 0xFF);
			Transp += TranspStep;
		}
	}

	// TODO: (Gavin) Why on earth is this setting 0xFE and not 0xFF?
	m_pTable->Table[0xFE] = static_cast <BYTE> ( EndTransp );
	
	return(TRUE);
}



/********************************************************************************************

>	BOOL TranspGradTable::BuildBitmapTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable, CProfileBiasGain& BiasGain)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com> (modified by CGS)
	Created:	12/3/97 (15/2/2000)
	Inputs:		-
	Purpose:	Builds a TransparentGradTable from StartTransp to EndTransp
				This function forces the table to be 256 entries (for bitmap transparency)
				The function now (also) takes into account the applied transparency profile.
	Errors:		-

********************************************************************************************/

BOOL TranspGradTable::BuildBitmapTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable, CProfileBiasGain& BiasGain)
{
	TranspRampItem *pTTI=NULL;
	if (pTranspTable!=NULL)	
		pTTI=pTranspTable->GetFirstTransp();

	if (pTTI==NULL)
	{
		return BuildBitmapTable(StartTransp, EndTransp, BiasGain);
	}

	if (m_pTable != NULL)
	{
		CCFree(m_pTable);
		m_pTable = NULL;
	}

	m_pTable = (TransparentGradTable*) CCMalloc(256);
	if (m_pTable == NULL)
		return(FALSE);

	INT32 ei, si=0;
	INT32 end, start = StartTransp;
	while (pTTI)
	{
		ei = (INT32)(pTTI->GetPosition()*255.0 + 0.5);
		end = pTTI->GetTransparency();
		FillSection(si,ei,start,end);
		si = ei;
		start = end;
		pTTI = pTranspTable->GetNextTransp(pTTI);
	}
	end = EndTransp;
	FillSection(si,255,start,end);
	
	return TRUE;
}



/********************************************************************************************

>	void TranspGradTable::FillSection(INT32 startindex, INT32 endindex, INT32 starttransp, INT32 endtransp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Inputs:		StartIndex,	EndIndex = range of indexes to fill
				starttransp
				endtransp
	Purpose:	Builds a TransparentGradTable from StartTransp to EndTransp with inbetween
				transparency levels held in pTranspTable
	Errors:		-

********************************************************************************************/

void TranspGradTable::FillSection(INT32 startindex, INT32 endindex, INT32 starttransp, INT32 endtransp)
{
	INT32 den = endindex-startindex;
	if (den<0)
		return;

	if (den==0)
	{
		m_pTable->Table[endindex] = (UINT32)endtransp;
	}
	else
	{
		INT32 t = (starttransp<<22) + 0x00200000;
		INT32 inc = ((endtransp-starttransp)<<22) / den;
		for (INT32 i=startindex; i<=endindex; i++)
		{
			m_pTable->Table[i] = (t>>22) & 0xFF;
			t+=inc;
		}
	}
}



/********************************************************************************************

>	GradTable32::GradTable32()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Purpose:	Default constructor for a GradTable32
	Errors:		-

********************************************************************************************/

GradTable32::GradTable32()
{
}


/********************************************************************************************

>	GradTable32::~GradTable32()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/96
	Purpose:	Destructor for a GradTable32

********************************************************************************************/

GradTable32::~GradTable32()
{
}

/********************************************************************************************

>	void GradTable32::SetTableSize(BOOL LargeTable = FALSE)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		NewSize - The new size (number of entries) in this table.
				May only be 0x100 or 0x800

	Purpose:	To set the size of this Grad table

	Notes:		The default table size is 0x100 entries

********************************************************************************************/

void GradTable32::SetTableSize(BOOL LargeTable)
{
}



/********************************************************************************************

>	BOOL GradTable32::BuildTable(DocColour &StartCol, DocColour &EndCol,
								View *ContextView, EFFECTTYPE Effect,
								DWORD TableSize = 0x100)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Will)
	Created:	14/5/96 (14/9/94)

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				TableSize	- The number of entries you wish to make the table (0x100 or
							  0x800)

	Returns:	FALSE if it fails

	Purpose:	Builds a GradTable from StartCol to EndCol, filling in the entire
				table with appropriate GDraw dither patterns.
				
				== NOTE that this version of the call also implicitly sets the table size! == 

	SeeAlso:	GradTable::SetTableSize

********************************************************************************************/
                                                                                 
BOOL GradTable32::BuildTable(DocColour &StartCol, DocColour &EndCol,
							View *ContextView, EFFECTTYPE Effect,
							DWORD TableSize)
{
	ERROR3("GradTable32::BuildTable() is rampant so don't use it\n");

	return(FALSE);	
/*
	SetTableSize(TableSize);

	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect,
								0, Table.Length,
								(void *)Table.Table, TRUE));
*/
}



/********************************************************************************************

>	BOOL GradTable32::BuildTable(DocColour &StartCol, DocColour &EndCol,
								View *ContextView, EFFECTTYPE Effect,
								DWORD FirstEntry, DWORD LastEntry)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Will)
	Created:	14/5/96 (14/9/94)

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

	Returns:	FALSE if it fails

	Purpose:	Builds a GradTable from StartCol to EndCol, filling in a portion of the
				table with appropriate GDraw dither patterns

	Notes:		GDraw requires the Start and End colours for the entire table to be
				set separately in the table structure. These will be set if FirstEntry==0
				and/or if LastEntry==TableSize, respectively.

	SeeAlso:	GradTable::SetTableSize

********************************************************************************************/
                                                                                 
BOOL GradTable32::BuildTable(DocColour &StartCol, DocColour &EndCol,
							View *ContextView, EFFECTTYPE Effect,
							DWORD FirstEntry, DWORD LastEntry)
{
	ERROR3("GradTable32::BuildTable() is rampant so don't use it\n");

	return(FALSE);	
/*	
	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect,
								FirstEntry, LastEntry,
								(void *)Table.Table, TRUE));
*/
}



/********************************************************************************************

>	static BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, COLORREF *pTable)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				TableSize	- Number of entries in the table

	Outputs:	pTable		- On return, will be filled in with appropriate colours

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated Table, filling in the given table with COLORREF
				entries. The entries will be colour corrected and/or separated as
				appropriate to the context view.

	Errors:		-

********************************************************************************************/
                                                                                 
BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, COLORREF *pTable)
{
	CProfileBiasGain DefaultBiasGain;			// we need a 'default' profile so we call
												// standardise the following call ....
	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect, 0, TableSize,
							(void *) pTable, TRUE, DefaultBiasGain));
}



/********************************************************************************************

>	static BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										COLORREF *pTable)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

	Outputs:	pTable		- On return, will be filled in with appropriate colours

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated Table from StartCol to EndCol, filling in the given
				table with COLORREF entries. The entries will be colour corrected
				and/or separated as appropriate to the context view.

	Errors:		Out of memory

********************************************************************************************/

BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										COLORREF *pTable)
{
	CProfileBiasGain DefaultBiasGain;			// we need a 'default' profile so we call
												// standardise the following call ....
	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect, FirstEntry, LastEntry,
							(void *) pTable, TRUE, DefaultBiasGain));
}



/********************************************************************************************

>	static BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, RGBQUAD *pTable)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				TableSize	- Number of entries in the table

	Outputs:	pTable		- On return, will be filled in with appropriate colours

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated Table, filling in the given table with RGBQUAD
				entries. The entries will be colour corrected and/or separated as
				appropriate to the context view.

	Errors:		-

********************************************************************************************/
                                                                                 
BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, RGBQUAD *pTable)
{
	CProfileBiasGain DefaultBiasGain;			// we need a 'default' profile so we call
												// standardise the following call ....
	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect, 0, TableSize,
							(void *) pTable, FALSE, DefaultBiasGain));
}



/********************************************************************************************

>	static BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, RGBQUAD *pTable,
										CProfileBiasGain& BiasGain)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/2000

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				TableSize	- Number of entries in the table

				BiasGain	- The biasgain profile that is to alter the table

	Outputs:	pTable		- On return, will be filled in with appropriate colours

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated Table, filling in the given table with RGBQUAD
				entries (taking into account the biasgain). The entries will be colour
				corrected and/or separated as appropriate to the context view.

	Errors:		-

********************************************************************************************/

BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, RGBQUAD *pTable,
										CProfileBiasGain& BiasGain)
{
	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect, 0, TableSize,
							(void *) pTable, FALSE, BiasGain));
}



/********************************************************************************************

>	static BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										RGBQUAD *pTable)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

	Outputs:	pTable		- On return, will be filled in with appropriate colours

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated Table from StartCol to EndCol, filling in the given
				table with RGBQUAD entries. The entries will be colour corrected
				and/or separated as appropriate to the context view.

	Errors:		Out of memory

********************************************************************************************/

BOOL GradTable32::BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										RGBQUAD *pTable)
{
	CProfileBiasGain DefaultBiasGain;			// we need a 'default' profile so we call
												// standardise the following call ....
	return(BuildPaletteInternal(StartCol, EndCol, ContextView, Effect, FirstEntry, LastEntry,
							(void *) pTable, FALSE, DefaultBiasGain));
}



/********************************************************************************************

>	static BOOL GradTable32::BuildPaletteInternal(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_FADE, EFFECT_HSV_SHORT, or EFFECT_HSV_LONG

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

				IsColorRef	-	TRUE if the pTable pointer is a (COLORREF *)
								FALSE if the pTable pointer is an (RGBQUAD *)

	Outputs:	pTable		- On return, will be filled in with appropriate colours, in the format
								indicated by the IsColorRef flag.

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated Table from StartCol to EndCol, filling in the given
				table with either COLORREF or RGBQUAD entries.
				The entries will be colour corrected and/or separated as appropriate to
				the context view.

				This is an internal shared function used by all the other GradTable32 
				grad palette building methods

	Notes:		This routine supplies a simple, generic mixing loop. However, for normal
				(no colour plate) rendering, this function will call the other internal
				methods which are optimised for specific cases - this gives about a 3x
				performance enhancement for normal rendering.

	Errors:		Out of memory

********************************************************************************************/

BOOL GradTable32::BuildPaletteInternal(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef,
										CProfileBiasGain& BiasGain)
{
	ERROR3IF(pTable == NULL, "Illegal NULL param");

	// The range should be withion the table size. We only ERROR3, though, because
	// the physical table is always large enough for the maximum number of entries.
	ERROR3IF(FirstEntry < 0 || FirstEntry > LastEntry,
				"GradTable32::BuildPaletteInternal expects sensible First/Last Entry values");

	// Determine if either the start or end colour is a spot colour - if it is, we must
	// coerce the fill into a "mix" (rgb) rather than rainbow fill style
	if (Effect != EFFECT_RGB && (StartCol.GetSpotParent() != NULL || EndCol.GetSpotParent() != NULL))
		Effect = EFFECT_RGB;


	// --- Optimisation - are we trying to build a simple grad table under normal rendering
	// circumstances? If so, special case this to make it as fast as possible. This more than
	// doubles the speed of table generation.
	ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);
	ERROR3IF(ccRGB == NULL, "GradTable32::BuildPaletteInternal - Can't find my colour context!");

	if (ccRGB->GetColourPlate() == NULL || ccRGB->GetColourPlate()->IsDisabled())
	{
		if (Effect == EFFECT_RGB)	// RGB mix?
		{
			return(BuildPaletteInternalRGB(StartCol, EndCol, ContextView,
										FirstEntry, LastEntry, pTable, IsColorRef, BiasGain));
		}

		// HSV (rainbow) mix
		return(BuildPaletteInternalHSV(StartCol, EndCol, ContextView, Effect,
									FirstEntry, LastEntry, pTable, IsColorRef, BiasGain));
	}

	if (Effect == EFFECT_RGB && ccRGB->GetColourPlate()->GetType() == COLOURPLATE_COMPOSITE &&
		(StartCol.GetColourModel() != COLOURMODEL_CMYK || EndCol.GetColourModel() != COLOURMODEL_CMYK))
	{
		return(BuildPaletteInternalRGBComposite(StartCol, EndCol, ContextView,
												FirstEntry, LastEntry, pTable, IsColorRef));
	}


	// --- OK, we're trying to do something special. Drop back to the generic (slow) code
	RGBQUAD  *pRGBTable	= (RGBQUAD *)  pTable;		// Cast the table pointer to the two
	COLORREF *pRefTable	= (COLORREF *) pTable;		// possible types

	// Get colour contexts: we need 2 - first, an RGB context (got above) for the ContextView, which
	// will be used to get colour-corrected/separated/etc output colour in RGB; and second,
 	// a generic RGB or HSV context to provide a "mixing colourspace" for the graduations.
	ColourContext *cc = NULL;

	if (Effect == EFFECT_RGB)
		cc = ColourManager::GetColourContext(COLOURMODEL_RGBT);		// RGB fade
	else
		cc = ColourManager::GetColourContext(COLOURMODEL_HSVT);		// HSV fade

	// If we can't find RGB/HSV contexts, something is seriously broken
	ERROR3IF(cc == NULL, "GradTable32::BuildPaletteInternal - Can't find my colour context!");

	// Fill in the Start and End colours of the table, if we've hit them
	PColourRGBT Result;

	// And build our graduated palette
	double MixFraction	= 0.0;
	double MixStep		= 1.0 / (double) (LastEntry - FirstEntry - 1);

	DocColour temp;

	for (DWORD col = FirstEntry; col < LastEntry; col++)
	{
		// Create a new colour which is an appropriate mixture of the start/end colours
		temp.Mix(&StartCol, &EndCol, MixFraction, cc, (Effect == EFFECT_HSV_LONG), ccRGB);

		// Convert back to the raw RGB and place it in the palette.
		// Note that this will colour correct and separate in an appropriate manner.
		// Note also that we're using a special ConvertColour call which returns 8-bit 
		// "packed" component values instead of FIXED24's.
		ccRGB->ConvertColour(&temp, (ColourPacked *) &Result);

		// And add to the table
		if (IsColorRef)
		{
			pRefTable[col] = RGB(Result.Red, Result.Green, Result.Blue);
		}
		else
		{
			pRGBTable[col].rgbRed		= Result.Red;
			pRGBTable[col].rgbGreen		= Result.Green;
			pRGBTable[col].rgbBlue		= Result.Blue;
			pRGBTable[col].rgbReserved	= 0;
		}

		// And step to the next mix fraction value
		MixFraction += MixStep;
	}

	return(TRUE);
}



/********************************************************************************************

>	static BOOL GradTable32::BuildPaletteInternalRGB(DocColour &StartCol, DocColour &EndCol,
													View *ContextView,
													DWORD FirstEntry, DWORD LastEntry,
													void *pTable, BOOL IsColorRef,
													CProfileBiasGain& BiasGain)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

				IsColorRef	-	TRUE if the pTable pointer is a (COLORREF *)
								FALSE if the pTable pointer is an (RGBQUAD *)

				BiasGain	- The profile that controls how the table is built

	Outputs:	pTable		- On return, will be filled in with appropriate colours, in the format
								indicated by the IsColorRef flag.

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated (RGB Mix) Table from StartCol to EndCol, filling in the given
				table with either COLORREF or RGBQUAD entries.

				NOTE that the generated entries will NOT be colour corrected or separated in
				any way. You should not use this code when generating colour plates.

********************************************************************************************/

BOOL GradTable32::BuildPaletteInternalRGB(DocColour &StartCol, DocColour &EndCol,
										View *ContextView,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef, CProfileBiasGain& BiasGain)
{
	ERROR3IF(ContextView == NULL || pTable == NULL, "Illegal NULL params");

	ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);
	ERROR3IF(ccRGB == NULL, "No colour context");

	PColourRGBT StartDef;
	PColourRGBT EndDef;

	if (StartCol.IsTransparent())
		StartDef.Red = StartDef.Green = StartDef.Blue = 255L;	// If transparent, use white
	else
		ccRGB->ConvertColour(&StartCol, (ColourPacked *) &StartDef);

	if (EndCol.IsTransparent())
		EndDef.Red = EndDef.Green = EndDef.Blue = 255L;			// If transparent, use white
	else
		ccRGB->ConvertColour(&EndCol, (ColourPacked *) &EndDef);

	// Precalculate mixing fraction and step to minimise mul/div in the inner loop
	// We use an 8.24 bit fixed point notation (homebrew FIXED24s)
	DWORD MixStep = (1 << 24) / (LastEntry - FirstEntry);
	DWORD MixFraction = 0;

	if (IsColorRef)
	{
		COLORREF *pRefTable	= (COLORREF *) pTable;

		DWORD R,G,B;
		for (DWORD col = FirstEntry; col < LastEntry; col++)
		{
			const DWORD InvFraction = 0x01000000 - MixFraction;

			// Calculate intermediate RGB values into 8bpp values
			R = ((StartDef.Red	 * InvFraction) + (EndDef.Red	* MixFraction)) >> 24;
			G = ((StartDef.Green * InvFraction) + (EndDef.Green	* MixFraction)) >> 24;
			B = ((StartDef.Blue	 * InvFraction) + (EndDef.Blue	* MixFraction)) >> 24;

			// Fill in this table entry
			pRefTable[col] = RGB(R, G, B);

			MixFraction += MixStep;
		}
	}
	else
	{
		CProfileBiasGain DefaultBiasGain;			// default
		if (BiasGain == DefaultBiasGain)
		{
			RGBQUAD  *pRGBTable	= (RGBQUAD *)  pTable;

			DWORD InvFraction;

			for (DWORD col = FirstEntry; col < LastEntry; col++)
			{
				InvFraction = 0x01000000 - MixFraction;

				// Calculate intermediate RGB values into the 8bpp table values
#if defined(__WXMSW__)
				pRGBTable[col].rgbRed	= static_cast<BYTE> (((StartDef.Red		* InvFraction) + (EndDef.Red	* MixFraction)) >> 24);
				pRGBTable[col].rgbGreen	= static_cast<BYTE> (((StartDef.Green	* InvFraction) + (EndDef.Green	* MixFraction)) >> 24);
				pRGBTable[col].rgbBlue	= static_cast<BYTE> (((StartDef.Blue	* InvFraction) + (EndDef.Blue	* MixFraction)) >> 24);
#else
				pRGBTable[col].rgbBlue	= static_cast<BYTE> (((StartDef.Red		* InvFraction) + (EndDef.Red	* MixFraction)) >> 24);
				pRGBTable[col].rgbGreen	= static_cast<BYTE> (((StartDef.Green	* InvFraction) + (EndDef.Green	* MixFraction)) >> 24);
				pRGBTable[col].rgbRed	= static_cast<BYTE> (((StartDef.Blue	* InvFraction) + (EndDef.Blue	* MixFraction)) >> 24);
#endif
				pRGBTable[col].rgbReserved	= 0;

				MixFraction += MixStep;
			}
		}
		else
		{
			BiasGain.SetIntervals (AFp (0), AFp (LastEntry));

			RGBQUAD  *pRGBTable	= (RGBQUAD *)  pTable;

			double MixFraction;
			double InvFraction;
			double RLastEntry = 1.0/LastEntry;

			for (DWORD col = FirstEntry; col < LastEntry; col++)
			{
				MixFraction = (INT32) BiasGain.MapInterval( AFp( col ) );
				MixFraction *= RLastEntry;
				InvFraction = 1 - MixFraction;

				// Calculate intermediate RGB values into the 8bpp table values
#if defined(__WXMSW__)
				pRGBTable[col].rgbRed	= static_cast<BYTE> (((StartDef.Red		* InvFraction) + (EndDef.Red	* MixFraction)));
				pRGBTable[col].rgbGreen	= static_cast<BYTE> (((StartDef.Green	* InvFraction) + (EndDef.Green	* MixFraction)));
				pRGBTable[col].rgbBlue	= static_cast<BYTE> (((StartDef.Blue	* InvFraction) + (EndDef.Blue	* MixFraction)));
#else
				pRGBTable[col].rgbBlue	= static_cast<BYTE> (((StartDef.Red		* InvFraction) + (EndDef.Red	* MixFraction)));
				pRGBTable[col].rgbGreen	= static_cast<BYTE> (((StartDef.Green	* InvFraction) + (EndDef.Green	* MixFraction)));
				pRGBTable[col].rgbRed	= static_cast<BYTE> (((StartDef.Blue	* InvFraction) + (EndDef.Blue	* MixFraction)));
#endif
				pRGBTable[col].rgbReserved	= 0;
			}
		}
	}

	return(TRUE);
}



/********************************************************************************************

>	static BOOL GradTable32::BuildPaletteInternalHSV(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef, CProfileBiasGain& BiasGain)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/9/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				Effect		- EFFECT_HSV_SHORT or EFFECT_HSV_LONG

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

				IsColorRef	-	TRUE if the pTable pointer is a (COLORREF *)
								FALSE if the pTable pointer is an (RGBQUAD *)

				BiasGain	- The profile that controls how the table is built

	Outputs:	pTable		- On return, will be filled in with appropriate colours, in the format
								indicated by the IsColorRef flag.

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated (HSV rainbow/alt-rainbow) Table from StartCol to EndCol, filling in
				the given table with either COLORREF or RGBQUAD entries.

				NOTE that the generated entries will NOT be colour corrected or separated in
				any way. You should not use this code when generating colour plates.

********************************************************************************************/

BOOL GradTable32::BuildPaletteInternalHSV(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef, CProfileBiasGain& BiasGain)
{
	ERROR3IF(ContextView == NULL || pTable == NULL, "Illegal NULL params");
	ERROR3IF(Effect != EFFECT_HSV_SHORT && Effect != EFFECT_HSV_LONG, "Unsupported HSV effect");

	ColourContext *ccHSV = ColourManager::GetColourContext(COLOURMODEL_HSVT, ContextView);
	ERROR3IF(ccHSV == NULL, "No colour context");

	PColourHSVT StartDef;
	PColourHSVT EndDef;

	if (StartCol.IsTransparent())
	{
		StartDef.Hue = 0;		// If transparent, use white
		StartDef.Value = 255;
		StartDef.Saturation = 0;
	}
	else
		ccHSV->ConvertColour(&StartCol, (ColourPacked *) &StartDef);

	if (EndCol.IsTransparent())
	{
		EndDef.Hue = 0;			// If transparent, use white
		EndDef.Value = 255;
		EndDef.Saturation = 0;
	}
	else
		ccHSV->ConvertColour(&EndCol, (ColourPacked *) &EndDef);

	// When doing alt-rainbow, if one end colour has no specific hue (saturation near 0)
	// then use the other colour's hue (if any)
	//
	// TODO: WRONG!!! This should always be doing these two tests, not just when
	// the Effect equals EFFECT_HSV_LONG. Note also that the long HSV colour
	// generation is incorrect, although I'm not too sure why. (Gavin).
	//
	if (Effect == EFFECT_HSV_LONG)
	{
		if (StartDef.Saturation < 2 && EndDef.Saturation >= 2)
			StartDef.Hue = EndDef.Hue;
		else if (StartDef.Saturation >= 2 && EndDef.Saturation < 2)
			EndDef.Hue = StartDef.Hue;
	}

	// Now work out which way we have to do the mix (rainbow or alt-rainbow)
	BOOL BlendNormally = TRUE;

	// Calc. the "simple" (non-wrapping) distance between the hues
	double StartHue	= StartDef.Hue / 360.0;
	double EndHue	= EndDef.Hue / 360.0;

	const double SimpleDist	= fabs(StartHue - EndHue);

	// Determine whether we do a simple blend, or we have to "wrap"
	if (SimpleDist <= 0.5)
		BlendNormally = (Effect != EFFECT_HSV_LONG);
	else
		BlendNormally = (Effect == EFFECT_HSV_LONG);

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

	// Precalculate mixing fraction and step to minimise mul/div in the inner loop
	// We use an 8.24 bit fixed point notation (homebrew FIXED24s)
	double MixStep = 1.0 / (double)(LastEntry - FirstEntry);
	double MixFraction = 0.0;

	if (IsColorRef)
	{
		COLORREF *pRefTable	= (COLORREF *) pTable;

		double H, S, V;
		for (DWORD col = FirstEntry; col < LastEntry; col++)
		{
			const double InvFraction = 1.0 - MixFraction;

			// Calculate intermediate RGB values into 8bpp values
			H =  (StartHue			  * InvFraction) + (EndHue			  * MixFraction);			// 0.0 - 1.0
			if (H > 1.0)
				H -= 1.0;		// If necessary, "mod" the value back down into the 0..1 range

			S = ((StartDef.Saturation * InvFraction) + (EndDef.Saturation * MixFraction)) / 255.0;	// 0.0 - 1.0
			V =  (StartDef.Value	  * InvFraction) + (EndDef.Value	  * MixFraction);			// 0.0 - 255.0

			// Convert the HSV value to an RGB table entry
			if (S == 0)
			{
				BYTE Val = (BYTE) V;
				pRefTable[col] = RGB(Val, Val, Val);
			}
			else
			{
				H *= 6.0;
				const INT32 I = (INT32) floor(H);
				const double F = H - I;
				const double P = V * (1.0 - S);
				const double Q = V * (1.0 - (S * F));
				const double T = V * (1.0 - (S * (1.0 - F)));

				switch (I)
				{
					case 1:  pRefTable[col] = RGB(BYTE(Q), BYTE(V), BYTE(P)); break;
					case 2:  pRefTable[col] = RGB(BYTE(P), BYTE(V), BYTE(T)); break;
					case 3:  pRefTable[col] = RGB(BYTE(P), BYTE(Q), BYTE(V)); break;
					case 4:  pRefTable[col] = RGB(BYTE(T), BYTE(P), BYTE(V)); break;
					case 5:  pRefTable[col] = RGB(BYTE(V), BYTE(P), BYTE(Q)); break;
					default: pRefTable[col] = RGB(BYTE(V), BYTE(T), BYTE(P)); break;
				}
			}

			MixFraction += MixStep;
		}
	}
	else
	{
		CProfileBiasGain DefaultBiasGain;			// default
		if (BiasGain == DefaultBiasGain)
		{
			RGBQUAD  *pRGBTable	= (RGBQUAD *)  pTable;

			double H, S, V;
			for (DWORD col = FirstEntry; col < LastEntry; col++)
			{
				const double InvFraction = 1.0 - MixFraction;

				// Calculate intermediate RGB values into 8bpp values
				H =  (StartHue			  * InvFraction) + (EndHue			  * MixFraction);			// 0.0 - 1.0
				if (H > 1.0)
					H -= 1.0;		// If necessary, "mod" the value back down into the 0..1 range

				S = ((StartDef.Saturation * InvFraction) + (EndDef.Saturation * MixFraction)) / 255.0;	// 0.0 - 1.0
				V =  (StartDef.Value	  * InvFraction) + (EndDef.Value	  * MixFraction);			// 0.0 - 255.0

				// Convert the HSV value to an RGB table entry
				if (S == 0)
				{
					pRGBTable[col].rgbRed = pRGBTable[col].rgbGreen = pRGBTable[col].rgbBlue = (BYTE) V;
				}
				else
				{
					H *= 6.0;
					const INT32 I = (INT32) floor(H);
					const double F = H - I;
					const double P = V * (1.0 - S);
					const double Q = V * (1.0 - (S * F));
					const double T = V * (1.0 - (S * (1.0 - F)));

// Define a wee macro to make this all a lot more readable
#if defined(__WXGTK__)
#define SETRGBENTRY(P1,P2,P3) pRGBTable[col].rgbRed = (BYTE)P3; pRGBTable[col].rgbGreen = (BYTE)P2; pRGBTable[col].rgbBlue = (BYTE)P1;
#else
#define SETRGBENTRY(P1,P2,P3) pRGBTable[col].rgbRed = (BYTE)P1; pRGBTable[col].rgbGreen = (BYTE)P2; pRGBTable[col].rgbBlue = (BYTE)P3;
#endif
					switch (I)
					{
						case 1:  SETRGBENTRY(Q, V, P); break;
						case 2:  SETRGBENTRY(P, V, T); break;
						case 3:  SETRGBENTRY(P, Q, V); break;
						case 4:  SETRGBENTRY(T, P, V); break;
						case 5:  SETRGBENTRY(V, P, Q); break;
						default: SETRGBENTRY(V, T, P); break;
					}
#undef SETRGBENTRY
				}

				pRGBTable[col].rgbReserved = 0;

				MixFraction += MixStep;
			}
		}
		else
		{
			BiasGain.SetIntervals (AFp (0), AFp (LastEntry));

			RGBQUAD  *pRGBTable	= (RGBQUAD *)  pTable;

			double MixFraction;
			double InvFraction;
			double RLastEntry = 1.0/LastEntry;

			double H, S, V;

			for (DWORD col = FirstEntry; col < LastEntry; col++)
			{
				//const double InvFraction = 1.0 - MixFraction;

				MixFraction = (INT32) BiasGain.MapInterval( AFp( col ) );
				MixFraction *= RLastEntry;
				InvFraction = 1 - MixFraction;

				// Calculate intermediate RGB values into 8bpp values
				H =  (StartHue			  * InvFraction) + (EndHue			  * MixFraction);			// 0.0 - 1.0
				if (H > 1.0)
					H -= 1.0;		// If necessary, "mod" the value back down into the 0..1 range

				S = ((StartDef.Saturation * InvFraction) + (EndDef.Saturation * MixFraction)) / 255.0;	// 0.0 - 1.0
				V =  (StartDef.Value	  * InvFraction) + (EndDef.Value	  * MixFraction);			// 0.0 - 255.0

				// Convert the HSV value to an RGB table entry
				if (S == 0)
				{
					pRGBTable[col].rgbRed = pRGBTable[col].rgbGreen = pRGBTable[col].rgbBlue = (BYTE) V;
				}
				else
				{
					H *= 6.0;
					const INT32 I = (INT32) floor(H);
					const double F = H - I;
					const double P = V * (1.0 - S);
					const double Q = V * (1.0 - (S * F));
					const double T = V * (1.0 - (S * (1.0 - F)));

// Define a wee macro to make this all a lot more readable
#define SETRGBENTRY(P1,P2,P3) pRGBTable[col].rgbRed = (BYTE)P1; pRGBTable[col].rgbGreen = (BYTE)P2; pRGBTable[col].rgbBlue = (BYTE)P3;
					switch (I)
					{
						case 1:  SETRGBENTRY(Q, V, P); break;
						case 2:  SETRGBENTRY(P, V, T); break;
						case 3:  SETRGBENTRY(P, Q, V); break;
						case 4:  SETRGBENTRY(T, P, V); break;
						case 5:  SETRGBENTRY(V, P, Q); break;
						default: SETRGBENTRY(V, T, P); break;
					}
#undef SETRGBENTRY
				}

				pRGBTable[col].rgbReserved = 0;

				MixFraction += MixStep;
			}
		}
	}

	return(TRUE);
}



/********************************************************************************************

>	static BOOL GradTable32::BuildPaletteInternalRGBComposite(DocColour &StartCol, DocColour &EndCol,
													View *ContextView,
													DWORD FirstEntry, DWORD LastEntry,
													void *pTable, BOOL IsColorRef)
	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/96

	Inputs:		StartCol, EndCol - Define the end colours of the graduation
				ContextView	- Points to the view in which the gradation is to be displayed.
							  (This is necessary for correction and separation etc. If proper
							  colour handling is not needed, then you may pass NULL, but
							  generally, this parameter should be sensible)

				FirstEntry	- The index of the first entry (inclusive) to fill in (0 is the first)
				LastEntry	- The index of the last entry (exclusive) to fill in (TableSize is the last)
								(i.e. to fill entire table use (0,TableSize) which will fill in entries
									0..Tablesize-1)

				IsColorRef	-	TRUE if the pTable pointer is a (COLORREF *)
								FALSE if the pTable pointer is an (RGBQUAD *)

	Outputs:	pTable		- On return, will be filled in with appropriate colours, in the format
								indicated by the IsColorRef flag.

	Returns:	FALSE if it fails

	Purpose:	Builds a Graduated (RGB Mix) Table from StartCol to EndCol, filling in the given
				table with either COLORREF or RGBQUAD entries.
				
				** The table is constructed taking into account "Composite Print Preview" colour
				** correction. You must only call this when generating a COLOURPLATE_COPMPOSITE
				** colour plate, when doing RGB mixes of non-CMYK colours!

	Notes:		(Analogue of GradTable code in BuildTable() and Filltable())
				For composite preview, I've only bothered to optimise the RGB mix case, as this
				is by far and away the most common one.

	SeeAlso:	GradTable32::BuildPaletteInternal

********************************************************************************************/

BOOL GradTable32::BuildPaletteInternalRGBComposite(DocColour &StartCol, DocColour &EndCol,
										View *ContextView,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef)
{
	ERROR3IF(ContextView == NULL || pTable == NULL, "Illegal NULL params");

	ColourContext *ccRGB = ColourManager::GetColourContext(COLOURMODEL_RGBT, ContextView);
	ColourContext *cc = ColourManager::GetColourContext(COLOURMODEL_RGBT);		// RGB fade
	ERROR3IF(cc == NULL || ccRGB == NULL, "No colour context");

	ERROR3IF(ccRGB->GetColourPlate() == NULL || ccRGB->GetColourPlate()->IsDisabled() ||
				ccRGB->GetColourPlate()->GetType() != COLOURPLATE_COMPOSITE,
					"BuildPaletteInternalRGBComposite called under incorrect conditions");

	ERROR3IF(StartCol.GetColourModel() == COLOURMODEL_CMYK && EndCol.GetColourModel() == COLOURMODEL_CMYK,
				"BuildPaletteInternalRGBComposite called for CMYK -> CMYK fill!");

	COLORREF *pRefTable	= (COLORREF *) pTable;
	RGBQUAD  *pRGBTable	= (RGBQUAD *)  pTable;

	PColourRGBT StartDef;
	PColourRGBT EndDef;
	ccRGB->ConvertColour(&StartCol, (ColourPacked *) &EndDef);

	DWORD StartIndex = FirstEntry;
	DWORD EndIndex;

	DocColour TempCol;

	while (StartIndex < LastEntry)
	{
		// Move the last run's end colour into Start
		memcpy(&StartDef, &EndDef, sizeof(PColourRGBT));

		// Calculate the end of this run
		EndIndex = StartIndex + FillStep;
		if (EndIndex > LastEntry)
			EndIndex = LastEntry;

		// Calculate the colour at the end of this run, and convert into EndDef
		TempCol.Mix(&StartCol, &EndCol,
					((double)(EndIndex - FirstEntry)) / ((double)(LastEntry - FirstEntry)),
					cc, FALSE, ccRGB);

		ccRGB->ConvertColour(&TempCol, (ColourPacked *) &EndDef);

		// Linearly interpolate all colours in the run (cf GradTable::FillTable code)
		// BLOCK
		{
			#define MFX 24

			// optimised code by Mike

			INT32 MixFracR = ((INT32)StartDef.Red)  <<MFX;
			INT32 MixFracG = ((INT32)StartDef.Green)<<MFX;
			INT32 MixFracB = ((INT32)StartDef.Blue) <<MFX;
			
			INT32 MixStepR = ((INT32)(EndDef.Red   - StartDef.Red  ))<<MFX;
			INT32 MixStepG = ((INT32)(EndDef.Green - StartDef.Green))<<MFX;
			INT32 MixStepB = ((INT32)(EndDef.Blue  - StartDef.Blue ))<<MFX;
			
			MixStepR = MixStepR / ((INT32)(EndIndex-StartIndex));
			MixStepG = MixStepG / ((INT32)(EndIndex-StartIndex));
			MixStepB = MixStepB / ((INT32)(EndIndex-StartIndex));

			if (IsColorRef)
			{
				for (DWORD Index=StartIndex; Index<EndIndex; Index++)
				{
					// Calculate intermediate RGB values into 8bpp values
					pRefTable[Index] = RGB(MixFracR>>MFX, MixFracG>>MFX, MixFracB>>MFX);
					MixFracR += MixStepR;
					MixFracG += MixStepG;
					MixFracB += MixStepB;
				}
			}
			else
			{
				for (DWORD Index=StartIndex; Index<EndIndex; Index++)
				{
					// Calculate intermediate RGB values into 8bpp values
					pRGBTable[Index].rgbRed		= (BYTE)(MixFracR>>MFX);
					pRGBTable[Index].rgbGreen	= (BYTE)(MixFracG>>MFX);
					pRGBTable[Index].rgbBlue	= (BYTE)(MixFracB>>MFX);
					pRGBTable[Index].rgbReserved = 0;
					MixFracR += MixStepR;
					MixFracG += MixStepG;
					MixFracB += MixStepB;
				}
			}

			#undef MFX
/*
			// Place the StartDef in as the first entry

			if (IsColorRef)
				pRefTable[StartIndex] = RGB(StartDef.Red, StartDef.Green, StartDef.Blue);
			else
			{
				pRGBTable[StartIndex].rgbRed	= StartDef.Red;
				pRGBTable[StartIndex].rgbGreen	= StartDef.Green;
				pRGBTable[StartIndex].rgbBlue	= StartDef.Blue;
				pRGBTable[StartIndex].rgbReserved = 0;
			}

			// ...and start interpolating from the second entry
			DWORD MixStep = (1 << 24) / (EndIndex - StartIndex);
			DWORD MixFraction = MixStep;

			DWORD R,G,B;

			if (IsColorRef)
			{
				for (DWORD Index = StartIndex + 1; Index < EndIndex; Index++)
				{
					// Calculate intermediate RGB values into 8bpp values
					const DWORD InvFraction = (0x01000000 - MixFraction);
					R = ((StartDef.Red	 * InvFraction) + (EndDef.Red	* MixFraction)) >> 24;
					G = ((StartDef.Green * InvFraction) + (EndDef.Green	* MixFraction)) >> 24;
					B = ((StartDef.Blue	 * InvFraction) + (EndDef.Blue	* MixFraction)) >> 24;

					pRefTable[Index] = RGB(R, G, B);
					MixFraction += MixStep;
				}
			}
			else
			{
				for (DWORD Index = StartIndex + 1; Index < EndIndex; Index++)
				{
					// Calculate intermediate RGB values into 8bpp values
					const DWORD InvFraction = (0x01000000 - MixFraction);
					R = ((StartDef.Red	 * InvFraction) + (EndDef.Red	* MixFraction)) >> 24;
					G = ((StartDef.Green * InvFraction) + (EndDef.Green	* MixFraction)) >> 24;
					B = ((StartDef.Blue	 * InvFraction) + (EndDef.Blue	* MixFraction)) >> 24;

					// Fill in this table entry
					pRGBTable[Index].rgbRed		= (BYTE)R;
					pRGBTable[Index].rgbGreen	= (BYTE)G;
					pRGBTable[Index].rgbBlue	= (BYTE)B;
					pRGBTable[Index].rgbReserved = 0;

					MixFraction += MixStep;
				}
			}
*/
		}

		// And step on to the next run
		StartIndex = EndIndex;
	}

	return(TRUE);
}
