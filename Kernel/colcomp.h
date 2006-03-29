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

// The document component for the document's colour list.

#ifndef INC_COLCOMP
#define INC_COLCOMP

#include "doccomp.h"
#include "colmodel.h"
#include "epsstack.h"

class ColourList;
class DocColour;
class ImportedColours;
class EPSExportDC;
class NewColourInfo;
class BaseCamelotFilter;
class ExportedColours;
class CXaraFileRecord;

/********************************************************************************************

>	class ColourListComponentClass : public DocComponentClass

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Purpose:	Provide a class that will add Colour List components to new documents.
	SeeAlso:	DocComponentClass

********************************************************************************************/

class ColourListComponentClass : public DocComponentClass
{
	CC_DECLARE_DYNAMIC(ColourListComponentClass)

public:
	static BOOL Init();

	BOOL AddComponent(BaseDocument *);
};



// CopyColourAcross() returns one of 3 valkues to indicate failure or success.
// Success also indicates if an existing colour was merged, or a completely new
// colour has been generated.
typedef enum
{
	CCCOPY_FAILED = 0,			// FALSE
	CCCOPY_NEWCOLOUR,
	CCCOPY_MERGEDCOLOUR
} ColCompCopyResult;

// Should correspond to the IndexedColourTypes in all but the shade one.
// This is so we can export a shade as a shade rather than a tint

typedef enum
{
	EXPORT_COLOURTYPE_NORMAL = 0,			// The colour is a perfectly normal colour
	EXPORT_COLOURTYPE_SPOT,					// The colour is a Spot Colour
	EXPORT_COLOURTYPE_TINT,					// The colour is a Tint (or a shade, a special type of tint)
	EXPORT_COLOURTYPE_LINKED,				// The colour is a Linked colour
	EXPORT_COLOURTYPE_SHADE					// The colour is a Shade
} ExportColourType;


/********************************************************************************************

>	class ColourListComponent : public DocComponent

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/08/94
	Purpose:	Provide a component that is linked to a ColourList object.  This allows
				us to save out colour lists using the serialisation facilities of the
				DocComponent base class.
	SeeAlso:	DocComponent

********************************************************************************************/

class ColourListComponent : public DocComponent
{
	CC_DECLARE_DYNAMIC(ColourListComponent)

	friend class ImportedColours;

public:
	ColourListComponent();
	ColourListComponent(ColourList *);
	virtual ~ColourListComponent();
	ColourList *GetColourList() { return pIndexedColours; }

public:
	// Usual document component functions...
	// These are the EPS related functions - covers from simple EPS to the old native EPS formats
	virtual BOOL EPSStartImport(EPSFilter *);
	virtual void EPSEndImport(EPSFilter *, BOOL);
	virtual BOOL WriteEPSComments(EPSFilter *);
	virtual ProcessEPSResult ProcessEPSComment(EPSFilter *, PCTSTR );
	virtual void EndOfEPSComment(EPSFilter *);

	// -------------------------------
	
	// These are the Camelot Version 2 Web and Native file related functions
	virtual BOOL StartImport(BaseCamelotFilter *pFilter);
	virtual BOOL EndImport(BaseCamelotFilter *pFilter, BOOL Success);
	virtual BOOL StartExport(BaseCamelotFilter *pFilter);				// Set up export data structures
	virtual BOOL EndExport(BaseCamelotFilter *pFilter, BOOL Success);	// Write out pending data
	virtual void CleanUpAfterExport(BOOL Success);						// Destroy export data structures


public:
	// Functions for use by code that imports objects and needs to reference the colours
	// imported from the file.
	IndexedColour *FindNamedColour(PCTSTR, 
								   DocColour *ColDef = NULL, 
								   UINT32 Tint = 100,
								   BOOL Strict = FALSE);

private:
	ColourList *pIndexedColours;

	ImportedColours *pNewColours;

	// Our list of colours that we have currently exported from this document 
	ExportedColours *pExportedColours;

	void AddNewColour(IndexedColour *pNewCol);

	BOOL FindIdenticalColour(IndexedColour *pColour, IndexedColour **pResult);

	INT32  ExtractString(PCTSTR Comment, INT32 Start, PTSTR String);
	void ReadEPS_RGB(ColourRGBT *pCol, TCHAR *pComment, NewColourInfo *pColourInfo);
	void ReadEPS_CMYK(ColourCMYK *pCol, TCHAR *pComment, NewColourInfo *pColourInfo);
	void ReadEPS_HSV(ColourHSVT *pCol, TCHAR *pComment, NewColourInfo *pColourInfo);
	void ReadEPS_Grey(ColourGreyT *pCol, TCHAR *pComment, NewColourInfo *pColourInfo);
	UINT32 ReadEPS_Tint(TCHAR *pComment);
	void ReadEPS_Shade(TCHAR *pComment, INT32 *ResultX, INT32 *ResultY);

	BOOL ColourOrChildIsShade(IndexedColour *pCol);
	BOOL SaveColourAndChildren(IndexedColour *pCol, EPSExportDC *pDC, 
							   BOOL CamelotEPS, INT32 NestingLevel = 0,
							   BOOL SaveShades = FALSE);

	void AddComponentValue( PTSTR ColDef, PCTSTR Format, 
						   double ComponentValue, BOOL Inherited);

	// Web and Native functions	
	INT32 SaveColourAndParents(IndexedColour *pCol, BaseCamelotFilter *pFilter, INT32 NestingLevel = 0);
	INT32 SaveComplexColour(IndexedColour *pCol, BaseCamelotFilter *pFilter,	BOOL AddColourToList = TRUE);
	INT32 SaveSimpleColour(IndexedColour *pCol, BaseCamelotFilter *pFilter);
	INT32 SaveSimpleColour(DocColour *pDocCol, BaseCamelotFilter *pFilter, BOOL CheckSimpleColourList = FALSE);
	UINT32 AddComponentValue(FIXED24 ComponentValue, BOOL Inherited);

	INT32 GetIndexColourNumber(IndexedColour *pCol);

	BOOL ReadRGB(ColourRGBT *pCol, NewColourInfo *pColourInfo,
				 UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4);
	BOOL ReadCMYK(ColourCMYK *pCol, NewColourInfo *pColourInfo,
				 UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4);
	BOOL ReadHSV(ColourHSVT *pCol, NewColourInfo *pColourInfo,
				 UINT32 Comp1, UINT32 Comp2, UINT32 Comp3, UINT32 Comp4);
	BOOL ReadGrey(ColourGreyT *pCol, NewColourInfo *pColourInfo, UINT32 Comp1);
	BOOL ReadTint(NewColourInfo *pColourInfo, UINT32 Comp1);
	BOOL ReadShade(NewColourInfo *pColourInfo, UINT32 Comp1, UINT32 Comp2);

public:
	// The public way of importing and exporting colours for the new native/web filters
	BOOL GetDocColour(INT32 ColourRef, DocColour* pDocColour);
	BOOL ImportSimpleColour(CXaraFileRecord* pCXFileRec);
	BOOL ImportColourDefinition(CXaraFileRecord* pCXFileRec);
	INT32 GetWriteColourReference(DocColour* pDocColour, BaseCamelotFilter *pFilter);

public:
	// Functions to fix DocColours (that reference IndexedColours) as we copy them between
	// documents. They copy the IndexedColours as necessary to the destination document,
	// and fix the copied DocColours to reference the version of the IndexedColour for the
	// destination document rather than the source document.
	virtual BOOL StartComponentCopy();
	virtual BOOL EndComponentCopy();
	virtual BOOL EndComponentCopy(IndexedColour *InsertBefore, BOOL Report);
	virtual void AbortComponentCopy();
	ColCompCopyResult CopyColourAcross(DocColour* pDocColour, BOOL SourceIsUnique = FALSE);
	IndexedColour *ExactMatchExists(IndexedColour *SourceColour);


private:	// Private ComponentCopy methods/data
	// Methods exclusively for CopyColourAcross to use.
	// Should NOT be used by any other caller.
	INT32 ExtendTables(void);
	IndexedColour *MapColour(IndexedColour *SourceColour, BOOL *AlreadyThere = NULL,
							 BOOL SourceIsUnique = FALSE);

	// Array pointers - used when copying colour componenents in order to remember
	// colour mappings between the source and destination documents.
	// To be used only by [Start|End|Abort]ComponentCopy and CopyColourAcross methods
	IndexedColour **SourceColours;		// Colour pointers as they appeared in the source doc
	IndexedColour **DestColours;		// Col pointers as they now appear in destination doc
	BOOL		   *MergeColours;		// TRUE if the given colour is to be merged into the dest
	INT32 ColourTableSize;				// Size of the dynamically allocated arrays

	IndexedColour *GetIndexedParentOfColour(DocColour Colour);
private:	// Private Importing data
	// Used when importing linked/tinted colours
	INT32 LinkNesting;

private:	// Private link used during Importing to provide warnings to the user
	BaseCamelotFilter *pCamFilter;
	BOOL WarnedBefore;
};

#endif  // INC_COLCOMP


