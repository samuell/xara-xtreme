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

// Classes to implement the graduation tables required for special path filling attributes

#ifndef INC_GRADTBL
#define INC_GRADTBL

//#include "gconsts.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "gradtype.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "colmodel.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "biasgain.h"			// for CProfileBiasGain (i.e.  profiled fills/transparency!) - in camtypes.h [AUTOMATICALLY REMOVED]

class DocColour;
class ColourContext;
class View;
class ColourRamp;
class TransparencyRamp;

/********************************************************************************************

>	class GradTable: public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Purpose:	A kernel representation of a graduation table.
				(This is a table of 8-bpp dither pattern specifiers for GDraw)

	SeeAlso:	GradTable32

	Notes:		There are now 3 base functions in GradTable and GradTable32 which build
				graduation tables. The tables are actually identical in meaning, but are
				encoded as:
					(a) Gavin 8bpp dither patterns,
					(b) RGBQUAD RGB structures,
					(c) COLORREF RGB values
				this is because the different places that use these tables demand the
				different formats, and converting all the code to use only one table
				format is non-trivial and/or inefficient.

********************************************************************************************/

class GradTable: public CCObject
{
	CC_DECLARE_DYNCREATE(GradTable)

public:
	GradTable(BOOL LargeTable = FALSE);
	~GradTable();

	void SetTableSize(BOOL LargeTable);
			// Set the overall size of the table

	INT32 GetTableLength(void) { return(m_pTable->Length); };

	BOOL BuildTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					BOOL LargeTable = FALSE);
			// Set table size, and build a single-stage gradfill in it

	BOOL BuildTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					CProfileBiasGain& BiasGain, BOOL LargeTable = FALSE);
			// Set table size, and build a single-stage gradfill in it
			// This version takes into account the new profile stuff (CGS - 4/2/2000)

	BOOL BuildTable(DocColour &StartCol, DocColour &EndCol,	ColourRamp *pColourRamp,
					View *ContextView, EFFECTTYPE Effect,
					BOOL LargeTable = FALSE);
			// Set table size, and build a multi-stage gradfill in it

	BOOL BuildHighQualityRepeatTable(DocColour &StartCol, DocColour &EndCol, ColourRamp *pColourRamp,
					View *ContextView, EFFECTTYPE Effect);
			// Set table size, and build a multi-stage gradfill in it
			// This version build a high quality (aliasing free) repeating fill table.
			// This version handles fills with colour ramps, and those without

	BOOL BuildHighQualityRepeatTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					CProfileBiasGain& BiasGain);
			// Set table size, and build a multi-stage gradfill in it
			// This version build a high quality (aliasing free) repeating fill table.
			// This version handles profiled fills

	BOOL BuildTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					DWORD FirstEntry, DWORD LastEntry);
			// Build graduations between StartCol and EndCol, filling the given 
			// portion of the table - used for one stage of a multi-stage fill

	BOOL AddToTable(DocColour &NewCol, View *ContextView, DWORD Index);
			// Adds the given colour as entry 'Index' in the table

	GraduationTable *GetTable(void) { return(m_pTable); };
			// Proper access function for finding our Table data member

	BOOL IsLarge(void) { return(m_bLargeTable); };

protected:
	void FillTable(DWORD StartIndex, DWORD EndIndex,
					PColourRGBT *StartDef, PColourRGBT *EndDef);
			// Internal helper function for linearly interpolating between 2 RGB values

	INT32 GetTableSize(GDrawContext* GD, BOOL LargeTable);
			// Returns the size (bytes) of the structure required

	INT32 GetTableLength(GDrawContext* GD, BOOL LargeTable);
			// Returns the size (entries) of the structure required

private:
	BOOL m_bLargeTable;
	GraduationTable* m_pTable;
};



/********************************************************************************************

>	class TranspGradTable: public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Purpose:	A kernel representation of a transparent graduation table.
	SeeAlso:	-

********************************************************************************************/

class TranspGradTable: public CCObject
{
	CC_DECLARE_DYNCREATE(TranspGradTable)

public:
	TranspGradTable(BOOL LargeTable = FALSE);
	~TranspGradTable();

	
	void SetTableSize(BOOL LargeTable);
			// Set the overall size of the table

	BOOL BuildTable(INT32 StartTransp, INT32 EndTransp);
	BOOL BuildTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain);
	BOOL BuildTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable);
	BOOL BuildHighQualityRepeatTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain);
	BOOL BuildHighQualityRepeatTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable);
	BOOL BuildBitmapTable(INT32 StartTransp, INT32 EndTransp, CProfileBiasGain& BiasGain);
	BOOL BuildBitmapTable(INT32 StartTransp, INT32 EndTransp, TransparencyRamp *pTranspTable, CProfileBiasGain& BiasGain);

	TransparentGradTable* GetTable(void) const { return(m_pTable); };
	BOOL IsLarge(void) const { return(m_bLargeTable); };

protected:
	INT32 GetTableSize(GDrawContext* GD, BOOL LargeTable);
			// Returns the size (bytes) of the structure required

	INT32 GetTableLength(GDrawContext* GD, BOOL LargeTable);
			// Returns the size (entries) of the structure required

private:
	void FillSection(INT32 startindex, INT32 endindex, INT32 starttransp, INT32 endtransp);

private:	
	BOOL m_bLargeTable;
	TransparentGradTable* m_pTable;
};



/********************************************************************************************

>	class GradTable32: public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/94
	Purpose:	A kernel representation of a 32 bpp graduation table.
				(This is a table of COLORREF RGB values for GDraw)

	SeeAlso:	GradTable

	Notes:		There are now 3 base functions in GradTable and GradTable32 which build
				graduation tables. The tables are actually identical in meaning, but are
				encoded as:
					(a) Gavin 8bpp dither patterns,
					(b) RGBQUAD RGB structures,
					(c) COLORREF RGB values
				this is because the different places that use these tables demand the
				different formats, and converting all the code to use only one table
				format is non-trivial and/or inefficient.

********************************************************************************************/

class GradTable32: public CCObject
{
	CC_DECLARE_DYNCREATE(GradTable32)

public:
	GradTable32();
	~GradTable32();


public:		// GradTable32 sepcific methods
	void SetTableSize(BOOL LargeTable);
			// Set the overall size of the table

	BOOL BuildTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					DWORD TableSize = 0x100);
			// Set table size, and build a single-stage gradfill in it

	BOOL BuildTable(DocColour &StartCol, DocColour &EndCol,
					View *ContextView, EFFECTTYPE Effect,
					DWORD FirstEntry, DWORD LastEntry);
			// Build graduations between StartCol and EndCol, filling the given 
			// portion of the table - used for one stage of a multi-stage fill

public:		// Generic graduation-building methods - produce tables of RGB values
			// These are used internally to build the GradTable32, but are also exported
			// for GRenderRegion, OSRenderRegion, OILBitmap, etc for generating
			// fill/contone tables with.
	static BOOL BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, COLORREF *pTable);
			// Builds a table of graduated RGBQUADs between StartCol and EndCol

	static BOOL BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										COLORREF *pTable);
			// Builds a portion of a table of graduated RGBQUADs between StartCol and EndCol

	static BOOL BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, RGBQUAD *pTable);
			// Builds a table of graduated RGBQUADs between StartCol and EndCol

	static BOOL BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,		// CGS
										View *ContextView, EFFECTTYPE Effect,
										DWORD TableSize, RGBQUAD *pTable,
										CProfileBiasGain& BiasGain);
			// Builds a portion of a table of graduated RGBQUADs between StartCol and EndCol

	static BOOL BuildGraduatedPalette(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										RGBQUAD *pTable);
			// Builds a portion of a table of graduated RGBQUADs between StartCol and EndCol

protected:
	static BOOL BuildPaletteInternal(DocColour &StartCol, DocColour &EndCol,
										View *ContextView, EFFECTTYPE Effect,
										DWORD FirstEntry, DWORD LastEntry,
										void *pTable, BOOL IsColorRef, CProfileBiasGain& BiasGain);		// CGS
			// Internal function called by all of the above variants

	// ---
	static BOOL BuildPaletteInternalRGB(DocColour &StartCol, DocColour &EndCol, View *ContextView,
											DWORD FirstEntry, DWORD LastEntry,
											void *pTable, BOOL IsColorRef, CProfileBiasGain& BiasGain);	// CGS
			// Internal helper function for optimised RGB Mix GradTable32 generation

	static BOOL BuildPaletteInternalHSV(DocColour &StartCol, DocColour &EndCol,
											View *ContextView, EFFECTTYPE Effect,
											DWORD FirstEntry, DWORD LastEntry,
											void *pTable, BOOL IsColorRef,CProfileBiasGain& BiasGain);	// CGS
			// Internal helper function for optimised HSV Mix GradTable32 generation

	// ---
	static BOOL BuildPaletteInternalRGBComposite(DocColour &StartCol, DocColour &EndCol, View *ContextView,
											DWORD FirstEntry, DWORD LastEntry,
											void *pTable, BOOL IsColorRef);
			// Internal helper function for optimised RGB Mix GradTable32 generation

};

#endif  // INC_GRADTBL

