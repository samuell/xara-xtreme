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

/*
*/

#include "camtypes.h"
#include "coplfilr.h"

#include "app.h"
#include "ccfile.h"
#include "cdrcol.h"
#include "oilfltrs.h"
#include "optsunit.h"
#include "progress.h"
#include "scunit.h"
#include "sgcolour.h"
//#include "isetres.h"

//#include "ben.h"
//#include "filtrres.h"
//#include "peter.h"
//#include "will2.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(PaletteFilter, Filter)
CC_IMPLEMENT_DYNAMIC(CorelPaletteFilter, PaletteFilter)
CC_IMPLEMENT_DYNAMIC(MSPaletteFilter, PaletteFilter)
CC_IMPLEMENT_DYNAMIC(AdobeColourTableFilter, PaletteFilter)
CC_IMPLEMENT_DYNAMIC(AdobeColourSwatchFilter, PaletteFilter)
CC_IMPLEMENT_DYNAMIC(JCWColourFilter, PaletteFilter)
CC_IMPLEMENT_DYNAMIC(PaintShopProPaletteFilter, PaletteFilter)

#define new CAM_DEBUG_NEW

typedef struct {
	WORD Unknown;
	BYTE Entries[2];
} cpl_header;

#define CPL_MIN_ENTRY_SIZE		14		// minimum size of an entry in a cpl file
#define CPL_MAX_ENTRY_SIZE		(13+32)	// max size
#define CPL_CHECK_TEXT_START 	16		// place to start checking the text
#define CPF_MAX_COLNAME_SIZE 32

#define CPL_FVU		-1		// version unknown
#define CPL_FV4		4		// version 4
#define CPL_FV5		5		// version 5



/********************************************************************************************
>	PaletteFilter::PaletteFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for a base palette filter
	SeeAlso:	Filter
********************************************************************************************/
PaletteFilter::PaletteFilter()
{
	m_pNewColours = NULL;

	ImportIntoGallery = TRUE;
	CurrentGalleryGroup = NULL;
}


/********************************************************************************************
>	PaletteFilter::~PaletteFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for a base palette filter
	SeeAlso:	Filter
********************************************************************************************/
PaletteFilter::~PaletteFilter()
{
	if (m_pNewColours != NULL)
	{
		m_pNewColours->DestroyColours();
		delete m_pNewColours;
		m_pNewColours = NULL;
	}
}


/********************************************************************************************
>	virtual BOOL PaletteFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		pcszPathName - pointer to the name of the file that is about to be imported
	Returns:	TRUE if a document is required to import this file into,
				FALSE if file supplies the document
	Purpose:	For determining wether this filter needs a document to be created to import
				into.
********************************************************************************************/
BOOL PaletteFilter::IsDefaultDocRequired(const TCHAR* pcszPathName)
{
	// Since palette files are unlikely to supply a document we will return TRUE.
	// Derived classes can still virtually override this function and return FALSE if required.

	return TRUE;
}


/********************************************************************************************
>	virtual BOOL PaletteFilter::DoImport(SelOperation* pOp, CCLexFile* pFile, Document* pDoc, BOOL AutoChosen, ImportPosition *Pos,
							 KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		pOp		- The operation this import is running under
				pFile	- The file to import from
				pDoc	- The document to import into
				AutoChosen	- TRUE if this filter was chosen by the system, FALSE if chosen by the user
				ImportPos	- The position to import the file data at
				Pos -
				ppImportedBitmap - this is used mainly in the bitfltr.cpp for the HTML
				import filter. HTMLFilter::DoImport() needs a pointer to a kernel bitmap
				to set the background bitmap up into Camelot.
				pPosTranslate - This is used too by the HTMLFilter in order to do a formatting.
				URL - original URL of the imported file
	Returns:	TRUE for successful import, FALSE if an error occured.
	Purpose:	Base class for handling palette import

	Notes:		**** IMPORTANT **************************************************************
				Palette filters can be called directly from the colour gallery (sgcolour.cpp)
				library groups to import palette files directly into the gallery.
				In this case, pOp and pDoc are NULL, and the member vars CurrentGalleryGroup
				and ImportIntoGallery are set up suitably. It is this important that these
				filters continue to be happy about not importing into a document!
				*****************************************************************************

	SeeAlso:	Filter::DoImport
********************************************************************************************/
BOOL PaletteFilter::DoImport(SelOperation* pOp, CCLexFile* pFile, Document* pDoc, BOOL AutoChosen, ImportPosition *Pos,
							 KernelBitmap** ppImportedBitmap, DocCoord* pPosTranslate, String_256* URL)
{
	BOOL ok = TRUE;

	// Copy parameters into member vars.
	// NOTE that we allow pOp and pDoc to be NULL, when we are importing into a gallery!
	ERROR2IF(pFile==NULL, FALSE, "NULL parameter passed");
	m_pImportOp = pOp;
	m_pImportFile = pFile;
	m_pImportDoc = pDoc;

	// Determine if this is really a huge bodge and we're importing directly into the colour gallery
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	//ColourSGallery *ParentGal = (ColourSGallery *) OpDisplayColourGallery::FindGallery();
	ColourSGallery *ParentGal = ColourSGallery::GetInstance();
	if (ParentGal != NULL)
		CurrentGalleryGroup = ParentGal->GetCurrentImportGroup();
	else
#endif
		CurrentGalleryGroup = NULL;

	ImportIntoGallery = (CurrentGalleryGroup != NULL);
	ERROR2IF(!ImportIntoGallery && (pDoc == NULL || pOp == NULL), FALSE, "NULL doc/op when not importing into gallery");

	// Initialise base class services
	if (ok)
		ok = PrepareForImport();

	// Call derived loading functions																				 	
	if (ok)
		ok = PreImport();
	if (ok)
		ok = ImportPalette();
	if (ok)
		ok = PostImport();

	// Finialise base class importation
	if (ok)
		ok = FinishImport(TRUE);
	else
		FinishImport(FALSE);

	CurrentGalleryGroup = NULL;
	ImportIntoGallery = FALSE;

	return ok;
}


/********************************************************************************************
>	virtual BOOL PaletteFilter::DoExport(Operation* pOp, CCLexFile* pFile,
										 PathName* pPath, Document* pDoc,
										 BOOL ShowOptions)
	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		pOp			- The operation this import is running under
				pFile		- The file to import from
				pPath		- Importing path
				pDoc		- The document to import into
				ShowOptions	- Show the export options dialogue.
	Outputs:	-
	Returns:	TRUE for successful export, FALSE if an error occured.
	Purpose:	Base class for handling palette export
	SeeAlso:	Filter::DoImport
********************************************************************************************/
BOOL PaletteFilter::DoExport(Operation* pOp, CCLexFile* pFile,
							 PathName* pPath, Document* pDoc,
							 BOOL ShowOptions)
{
	// For the moment palettes are not exported.
	return FALSE;
}


/********************************************************************************************
>	BOOL PaletteFilter::PrepareForImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Initialises the base class before import
********************************************************************************************/
BOOL PaletteFilter::PrepareForImport()
{
	if (!ImportIntoGallery)
	{
		ERROR2IF(m_pImportDoc==NULL, FALSE, "NULL pointer");

		// Find the colour list document component
		ColourListComponent* pColours = NULL;
		DocComponent *pComponent = m_pImportDoc->EnumerateDocComponents(NULL);
		while (pComponent != NULL)
		{
			// If this is the colour component, remember it
			if (IS_A(pComponent, ColourListComponent))
				pColours = (ColourListComponent*) pComponent;

			// Look for next doc component
			pComponent = m_pImportDoc->EnumerateDocComponents(pComponent);
		}
		ERROR2IF(pColours==NULL, FALSE, "Import document had no colour list");

		// Create a new imported colours manager
		ERROR3IF(m_pNewColours != NULL, "ColourImport manager already present\n");
		if (m_pNewColours != NULL)
		{
			m_pNewColours->DestroyColours();
			delete m_pNewColours;
		}

		m_pNewColours = new ImportedColours(pColours, FALSE);
		if ((m_pNewColours == NULL) || !m_pNewColours->Init())
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL PaletteFilter::FinishImport(BOOL Success)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		Success - TRUE if import has been successful
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Finialises colour import
********************************************************************************************/
BOOL PaletteFilter::FinishImport(BOOL Success)
{
	BOOL ok = TRUE;

	if (!ImportIntoGallery)
	{
		// If we didn't even get to initialise, then return quietly.
		if (m_pNewColours == NULL)
			return FALSE;

		EndSlowJob();
		BeginSlowJob(-1, FALSE);

		// Import is finished, so add any outstanding colours to the document (if the
		// import was successful), and delete the colour table
		if (Success)
			ok = m_pNewColours->AddColoursToDocument();
		else
			m_pNewColours->DestroyColours();

		// Clean up claimed memory
		delete m_pNewColours;
		m_pNewColours = NULL;
	}

	return ok;
}


/********************************************************************************************
>	BOOL PaletteFilter::ProcessRGBColour(double Red, double Green, double Blue, const StringBase* pColName = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		Red - red value 0..1
				Green - green value 0..1
				Blue - blue value 0..1
				pColName - name to give the colour (NULL if no name)
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a Camelot RGB colour and adds it to the imported colours list
********************************************************************************************/
BOOL PaletteFilter::ProcessRGBColour(double Red, double Green, double Blue, const StringBase* pColName)
{
	// Ensure colour components are in range
	ERROR3IF(Red<0, "Red component less than 0");
	ERROR3IF(Red>1, "Red component greater than 1");
	ERROR3IF(Green<0, "Green component less than 0");
	ERROR3IF(Green>1, "Green component greater than 1");
	ERROR3IF(Blue<0, "Blue component less than 0");
	ERROR3IF(Blue>1, "Blue component greater than 1");
	Red = ClampZeroToOne(Red);
	Green = ClampZeroToOne(Green);
	Blue = ClampZeroToOne(Blue);

	// Make a Camelot RGB colour
	ColourRGBT NewColour;
	NewColour.Red =	Red;
	NewColour.Green = Green;
	NewColour.Blue = Blue;
	NewColour.Transparent = 0;

	// Name the colour
	String_64 NewColourName;
	BOOL ok = TRUE;
	if (pColName==NULL || pColName->IsEmpty())
		ok = NameRGBColour(&NewColour, &NewColourName);
	else
		NewColourName = *pColName;		

	if (ok)
	{
		if (ImportIntoGallery)
			ok = AddColourToGallery(PalettePrefix_None, &NewColourName, (ColourGeneric *)&NewColour, COLOURMODEL_RGBT);
		else
			ok = m_pNewColours->AddColour(&NewColourName, &NewColour);
	}

	return ok;
}


/********************************************************************************************
>	BOOL PaletteFilter::ProcessCMYKColour(double Cyan, double Magenta, double Yellow, double Key, const StringBase* pColName = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		Cyan - cyan value 0..1
				Magenta - green value 0..1
				Yellow - blue value 0..1
				Key - black value 0..1
				pColName - name to give the colour (NULL if no name)
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a Camelot CMYK colour and adds it to the imported colours list
********************************************************************************************/
BOOL PaletteFilter::ProcessCMYKColour(double Cyan, double Magenta, double Yellow, double Key, const StringBase* pColName)
{
	// Ensure colour components are in range
	ERROR3IF(Cyan<0, "Cyan component less than 0");
	ERROR3IF(Cyan>1, "Cyan component greater than 1");
	ERROR3IF(Magenta<0, "Magenta component less than 0");
	ERROR3IF(Magenta>1, "Magenta component greater than 1");
	ERROR3IF(Yellow<0, "Yellow component less than 0");
	ERROR3IF(Yellow>1, "Yellow component greater than 1");
	ERROR3IF(Key<0, "Key component less than 0");
	ERROR3IF(Key>1, "Key component greater than 1");
	Cyan = ClampZeroToOne(Cyan);
	Magenta = ClampZeroToOne(Magenta);
	Yellow = ClampZeroToOne(Yellow);
	Key = ClampZeroToOne(Key);

	// Make a Camelot CMYK colour
	ColourCMYK NewColour;
	NewColour.Cyan = Cyan;
	NewColour.Magenta = Magenta;
	NewColour.Yellow = Yellow;
	NewColour.Key = Key;

	// Name the colour
	String_64 NewColourName;
	BOOL ok = TRUE;
	if (pColName==NULL || pColName->IsEmpty())
		ok = NameCMYKColour(&NewColour, &NewColourName);
	else
		NewColourName = *pColName;		

	if (ok)
	{
		if (ImportIntoGallery)
			ok = AddColourToGallery(PalettePrefix_None, &NewColourName, (ColourGeneric *)&NewColour, COLOURMODEL_CMYK);
		else
			ok = m_pNewColours->AddColour(&NewColourName, &NewColour);
	}

	return ok;
}


/********************************************************************************************
>	BOOL PaletteFilter::ProcessHSVColour(double Hue, double Sat, double Value, const StringBase* pColName = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		Hue - Hue value 0..1
				Sat - Saturation value 0..1
				Value - Brightness value 0..1
				pColName - name to give the colour (NULL if no name)
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a Camelot HSV colour and adds it to the imported colours list
********************************************************************************************/
BOOL PaletteFilter::ProcessHSVColour(double Hue, double Sat, double Value, const StringBase* pColName)
{
	// Ensure colour components are in range
	ERROR3IF(Hue<0, "Hue component less than 0");
	ERROR3IF(Hue>1, "Hue component greater than 1");
	ERROR3IF(Sat<0, "Saturation component less than 0");
	ERROR3IF(Sat>1, "Saturation component greater than 1");
	ERROR3IF(Value<0, "Value component less than 0");
	ERROR3IF(Value>1, "Value component greater than 1");
	Hue = ClampZeroToOne(Hue);
	Sat = ClampZeroToOne(Sat);
	Value = ClampZeroToOne(Value);

	// Make a Camelot HSV colour
	ColourHSVT NewColour;
	NewColour.Hue =	Hue;
	NewColour.Saturation = Sat;
	NewColour.Value = Value;
	NewColour.Transparent = 0;

	// Name the colour
	String_64 NewColourName;
	BOOL ok = TRUE;
	if (pColName==NULL || pColName->IsEmpty())
		ok = NameHSVColour(&NewColour, &NewColourName);
	else
		NewColourName = *pColName;		

	if (ok)
	{
		if (ImportIntoGallery)
			ok = AddColourToGallery(PalettePrefix_None, &NewColourName, (ColourGeneric *)&NewColour, COLOURMODEL_HSVT);
		else
			ok = m_pNewColours->AddColour(&NewColourName, &NewColour);
	}

	return ok;
}


/********************************************************************************************
>	BOOL PaletteFilter::ProcessLabColour(double Luminance, double A, double B, const StringBase* pColName = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/96
	Inputs:		Luminance - Luminance value 0..100 (black to white)
				A - a value +127..-128 (green to red)
				B - b value +127..-128 (blue to yellow)
				pColName - name to give the colour (NULL if no name)
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a name for this colour and adds it to the imported colours list
				N.B. Camelot does not support Lab colours so the colour is converted to 
				     RGB or greyscale.
********************************************************************************************/
BOOL PaletteFilter::ProcessLabColour(double Luminance, double A, double B, const StringBase* pColName)
{
	// Ensure colour components are in range
	ERROR3IF(Luminance<0, "Luminance component less than 0");
	ERROR3IF(Luminance>100, "Luminance component greater than 100");
	ERROR3IF(A<-128, "A component less than -128");
	ERROR3IF(A> 127, "A component greater than 127");
	ERROR3IF(B<-128, "B component less than -128");
	ERROR3IF(B> 127, "B component greater than 127");
	Luminance = __min(100, __max(0, Luminance));
	A = __min(127, __max(-128, A));
	B = __min(127, __max(-128, B));

	// Convert to RGB values
	Vector3D Lab(Luminance, A, B);
	Vector3D RGBResult;
	PaletteFilter::ConvertLABToRGB(Lab, &RGBResult);

	return ProcessRGBColour(RGBResult.x, RGBResult.y, RGBResult.z, pColName );
}


/********************************************************************************************
>	BOOL PaletteFilter::ProcessGreyColour(double Intensity, const StringBase* pColName = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/96
	Inputs:		Intensity - 0..1 (black to white)
				pColName - name to give the colour (NULL if no name)
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a Camelot greyscale colour and adds it to the imported colours list
********************************************************************************************/
BOOL PaletteFilter::ProcessGreyColour(double Intensity, const StringBase* pColName)
{
	// Ensure colour components are in range
	ERROR3IF(Intensity<0, "Intensity component less than 0");
	ERROR3IF(Intensity>1, "Intensity component greater than 1");
	Intensity = ClampZeroToOne(Intensity);

	// Make a Camelot greyscale colour
	ColourGreyT NewColour;
	NewColour.Intensity = Intensity;
	NewColour.Reserved1 = 0;
	NewColour.Reserved2 = 0;
	NewColour.Transparent = 0;

	// Name the colour
	String_64 NewColourName;
	BOOL ok = TRUE;
	if (pColName==NULL || pColName->IsEmpty())
		ok = NameGreyColour(&NewColour, &NewColourName);
	else
		NewColourName = *pColName;		

	if (ok)
	{
		if (ImportIntoGallery)
			ok = AddColourToGallery(PalettePrefix_None, &NewColourName, (ColourGeneric *)&NewColour, COLOURMODEL_GREYT);
		else
			ok = m_pNewColours->AddColour(&NewColourName, &NewColour);
	}

	return ok;
}
 

/********************************************************************************************
>	BOOL PaletteFilter::NameRGBColour(ColourRGBT* pColour, StringBase* pName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/96
	Inputs:		pColour - the Camelot colour to name
	Outputs:	pName holds the generated name
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a name for this RGB colour
********************************************************************************************/
BOOL PaletteFilter::NameRGBColour(ColourRGBT* pColour, StringBase* pName)
{
	ERROR2IF(pColour==NULL || pName==NULL, FALSE, "NULL Pointer");

	if (UsePercentageColour())
	{
		pName->MakeMsg(_R(IDS_PALETTEIMPORT_RGBPERCENT),
								(INT32)(pColour->Red.MakeDouble()*100+0.4999),
								(INT32)(pColour->Green.MakeDouble()*100+0.4999),
								(INT32)(pColour->Blue.MakeDouble()*100+0.4999) );
	}
	else
	{
		pName->MakeMsg(_R(IDS_PALETTEIMPORT_RGBVALUE),
								(INT32)(pColour->Red.MakeDouble()*255+0.4999),
								(INT32)(pColour->Green.MakeDouble()*255+0.4999),
								(INT32)(pColour->Blue.MakeDouble()*255+0.4999) );
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL PaletteFilter::NameCMYKColour(ColourCMYK* pColour, StringBase* pName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/96
	Inputs:		pColour - the Camelot colour to name
	Outputs:	pName holds the generated name
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a name for this CMYK colour
********************************************************************************************/
BOOL PaletteFilter::NameCMYKColour(ColourCMYK* pColour, StringBase* pName)
{
	ERROR2IF(pColour==NULL || pName==NULL, FALSE, "NULL Pointer");
	
	if (UsePercentageColour())
	{
		pName->MakeMsg(_R(IDS_PALETTEIMPORT_CMYKPERCENT),
								(INT32)(pColour->Cyan.MakeDouble()*100+0.4999),
								(INT32)(pColour->Magenta.MakeDouble()*100+0.4999),
								(INT32)(pColour->Yellow.MakeDouble()*100+0.4999),
								(INT32)(pColour->Key.MakeDouble()*100+0.4999) );
	}
	else
	{
		pName->MakeMsg(_R(IDS_PALETTEIMPORT_CMYKVALUE),
								(INT32)(pColour->Cyan.MakeDouble()*255+0.4999),
								(INT32)(pColour->Magenta.MakeDouble()*255+0.4999),
								(INT32)(pColour->Yellow.MakeDouble()*255+0.4999),
								(INT32)(pColour->Key.MakeDouble()*255+0.4999) );
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL PaletteFilter::NameHSVColour(ColourHSVT* pColour, StringBase* pName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/96
	Inputs:		pColour - the Camelot colour to name
	Outputs:	pName holds the generated name
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a name for this CMYK colour
********************************************************************************************/
BOOL PaletteFilter::NameHSVColour(ColourHSVT* pColour, StringBase* pName)
{
	ERROR2IF(pColour==NULL || pName==NULL, FALSE, "NULL Pointer");
	
	if (UsePercentageColour())
	{
		pName->MakeMsg(_R(IDS_PALETTEIMPORT_HSVPERCENT),
								(INT32)(pColour->Hue.MakeDouble()*360+0.4999),
								(INT32)(pColour->Saturation.MakeDouble()*100+0.4999),
								(INT32)(pColour->Value.MakeDouble()*100+0.4999) );
	}
	else
	{
		pName->MakeMsg(_R(IDS_PALETTEIMPORT_HSVVALUE),
								(INT32)(pColour->Hue.MakeDouble()*360+0.4999),
								(INT32)(pColour->Saturation.MakeDouble()*255+0.4999),
								(INT32)(pColour->Value.MakeDouble()*255+0.4999) );
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL PaletteFilter::NameGreyColour(ColourGreyT* pColour, StringBase* pName)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/96
	Inputs:		pColour - the Camelot colour to name
	Outputs:	pName holds the generated name
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Generates a name for this greyscale colour
********************************************************************************************/
BOOL PaletteFilter::NameGreyColour(ColourGreyT* pColour, StringBase* pName)
{
	ERROR2IF(pColour==NULL || pName==NULL, FALSE, "NULL Pointer");

	if (pColour->Intensity.MakeDouble() <= 0.0)
		*pName = String_32(_R(IDS_PALETTEIMPORT_GREYBLACK));
	else if (pColour->Intensity.MakeDouble() >= 1.0)
			*pName = String_32(_R(IDS_PALETTEIMPORT_GREYWHITE));
		else
			pName->MakeMsg(_R(IDS_PALETTEIMPORT_GREYPERCENT), (INT32)(pColour->Intensity.MakeDouble()*100+0.4999));

	return TRUE;
}


/********************************************************************************************
>	BOOL PaletteFilter::UsePercentageColour()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the colour editor preference is for percentages, FALSE if not
	Purpose:	To  see wether or not to use the precentage colour naming scheme
********************************************************************************************/
BOOL PaletteFilter::UsePercentageColour()
{
// WEBSTER - markn 24/4/97
#ifdef WEBSTER
	// Always use percentages for colour units in Webster
	return TRUE;
#else
	#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
		// Read what the colour editors preference is
		ScaleUnit* pUnit = UnitsTab::GetColourUnitPreference();

		return (pUnit == &StandardUnit::UnitPercent);
	#else
		return FALSE;
	#endif
#endif // WEBSTER
}


/********************************************************************************************
>	BOOL PaletteFilter::StartPercentage(INT32 UpTo = -1)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		UpTo - the 100% value
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Starts a percentage delay indicator that runs from 0 to 100.  If you don't
				know how many colours you are importing pass -1 and the length of the file
				is used
********************************************************************************************/
BOOL PaletteFilter::StartPercentage(INT32 UpTo)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (UpTo == -1)
	{
		if (m_pImportFile != NULL)
			BeginSlowJob(m_pImportFile->Size());
		else
			BeginSlowJob();
	}
	else
		BeginSlowJob(UpTo);
#endif
	return TRUE;
}


/********************************************************************************************
>	BOOL PaletteFilter::SetPercentage(INT32 GotTo)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		GotTo - the current position
	Outputs:	-
	Returns:	TRUE for success, FALSE if an error occured.
	Purpose:	Continues a percentage delay indicator.  Pass -1 to use the current import
				file position as a progress indicator.
********************************************************************************************/
BOOL PaletteFilter::SetPercentage(INT32 GotTo)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (GotTo == -1)
	{
		if (m_pImportFile != NULL)
			ContinueSlowJob(m_pImportFile->tell());
		else
			ContinueSlowJob();
	}
	else
		ContinueSlowJob(GotTo);
#endif
	return TRUE;
}



/********************************************************************************************
>	void PaletteFilter::ConvertXYZToRGB(const Vector3D& icol, Vector3D& ocol)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/96
	Inputs:		icon = The (Xn, Yn, Zn) coordinates of the white colour in CIE XYZ space.
	Outputs:	ocol  = RGB colour (0..1)
	Returns:	-
	Purpose:	To convert colours from CIE XYZ to RGB
********************************************************************************************/
void PaletteFilter::ConvertXYZToRGB(const Vector3D& icol, Vector3D& ocol)
{
	// Convert from HDTV Rec709 to RGB
	ocol.x = ( 3.240479*icol.x) + (-1.537150*icol.y) + (-0.498535*icol.z);
	ocol.y = (-0.969256*icol.x) + ( 1.875992*icol.y) + ( 0.041556*icol.z);
	ocol.z = ( 0.055648*icol.x) + (-0.204043*icol.y) + ( 1.057311*icol.z);
	
	ClampZeroToOne(ocol.x);
	ClampZeroToOne(ocol.y);
	ClampZeroToOne(ocol.z);
}



/********************************************************************************************
>	void PaletteFilter::ConvertLABtoXYZ(const Vector3D& white, const Vector3D& icol, Vector3D& ocol)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/96
	Inputs:		white = The (Xn, Yn, Zn) coordinates of the white colour in CIE XYZ space.
				icol  = CIE LAB colour
	Outputs:	ocol  = CIE XYZ colour
	Returns:	-
	Purpose:	To convert colours from CIE LUV to CIE XYZ  
********************************************************************************************/
void PaletteFilter::ConvertLABtoXYZ(const Vector3D& white, const Vector3D& icol, Vector3D& ocol)
{
    double d,e;
    static double third = 1.0/3.0;

    if (icol.x < 7.9996248) 
		ocol.y = icol.x * white.y / 903.3;
    else 
        ocol.y = pow((double)((icol.x + 16.0) / 116.0), (double)3.0) * white.y;

    e = pow((double)(ocol.y / white.y), third);
	d =	e + (double)(icol.y / 500);
    
	ocol.x = pow(d, (double)3.0) * white.x;

    d =	e - (double)(icol.z / 200);
    
	ocol.z = pow(d, (double)3.0) * white.z;
}


/********************************************************************************************
>	void PaletteFilter::ConvertCIEtoXYZ(const Vector3D icol, Vector3D& ocol)

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/96
	Inputs:		icol = (x,y) is the chromaticity value
						  YY is the corresponding luminance value
	Outputs:	ocol = (X,Y,Z) coordinates in the CIE XYZ space
						
	Returns:	-
	Purpose:	To convert the given x, y and Y to X, Y and Z in CIE XYZ space
********************************************************************************************/
void PaletteFilter::ConvertCIEtoXYZ(const Vector3D& icol, Vector3D& ocol)
{
    double d = icol.z / icol.y;

    ocol.x = icol.x * d;
    ocol.y = icol.z;
    ocol.z = (1.0 - icol.x - icol.y) * d;
}


/********************************************************************************************
>	void PaletteFilter::ConvertLABToRGB(const Vector3D& icol, Vector3D* ocol)

	Author: 	Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/96
	Inputs:		icol = (x, y, z) - Lab colour (0..100, -128..127, -128..127)
	Outputs:	ocol = (x, y, z) - RGB colour (0..1, 0..1, 0..1)
	Returns:	-
	Purpose:	To convert the given Lab colour to a (near) equivelent RGB colour
				This uses the above conversion functions from Mike.  They are *not* accurate
				but do give a reasonable approximation of the colour.
********************************************************************************************/
void PaletteFilter::ConvertLABToRGB(const Vector3D& icol, Vector3D* ocol)
{
	// First calculate a XYZ white point
	Vector3D CIEWhite = Vector3D(0.3127, 0.3290, 1.0 );
	Vector3D XYZWhite;
	PaletteFilter::ConvertCIEtoXYZ(CIEWhite, XYZWhite);

	Vector3D LabAsXYZ;
	PaletteFilter::ConvertLABtoXYZ(XYZWhite, icol, LabAsXYZ);

	// Finally convert XYZ to RGB
	Vector3D RGB;
	PaletteFilter::ConvertXYZToRGB(LabAsXYZ, RGB);

	ocol->x = ClampZeroToOne( pow(RGB.x, 1.0/2.5) );
	ocol->y = ClampZeroToOne( pow(RGB.y, 1.0/2.5) );
	ocol->z = ClampZeroToOne( pow(RGB.z, 1.0/2.5) );
}



/********************************************************************************************

>	BOOL PaletteFilter::AddColourToGallery(PalettePrefix Prefix, String_64 *Name,
											ColourGeneric *Definition, ColourModel Model,
											BOOL NewlineAfter = FALSE, IsSpotColour = FALSE)
	Author: 	Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		Prefix - Indicates a standard library prefix which should be prepended
							to the name (remembering the prefix ID is much better than
							storing a great long string in every colour)
				Name - the name
				Definition - the colour definition
				Model - The colour model that Definition is given in
				NewlineAfter - TRUE to hint to the colour gallery that it should divide
				the colour items (by starting a new line in the display) in order to
				display them more usefully.

	Returns:	TRUE if it succeeeds

	Purpose:	Sticks the given colour into the colour gallery

********************************************************************************************/

BOOL PaletteFilter::AddColourToGallery(PalettePrefix Prefix, String_64 *Name,
										ColourGeneric *Definition, ColourModel Model,
										BOOL NewlineAfter, BOOL IsSpotColour)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (CurrentGalleryGroup == NULL)
		return(FALSE);

	// Create a new DocColour for this colour
	DocColour NewCol(Model, Definition);
	
	if (Prefix == PalettePrefix_Pantone || Prefix == PalettePrefix_PantoneSpot)
	{
		// Set the "PANTONE bodge" flag which rounds the colour components to the nearest half percent.
		// This gets around errors introduced by storing the 200 possible half-percent values in a 0-255 BYTE
		NewCol.ForceRounding(TRUE);
	}

	SGDisplayLibColour *NewItem = new SGDisplayLibColour(&NewCol, Name, Prefix, NewlineAfter, IsSpotColour);
	if (NewItem == NULL)
		return(FALSE);

	CurrentGalleryGroup->AddItem(NewItem);
#endif
	return(TRUE);
}






/********************************************************************************************
>	CorelPaletteFilter::CorelPaletteFilter()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Constructor
	SeeAlso:	Filter
********************************************************************************************/
CorelPaletteFilter::CorelPaletteFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_CORELPALETTEFILTER_FILTERNAME));
	FilterInfo.Load(_R(IDT_CORELPALETTEFILTER_FILTERINFO));
	FilterID = FILTERID_CORELPALETTEFILE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#endif

	m_FileVersion = CPL_FVU;
}


/********************************************************************************************
>	BOOL CorelPaletteFilter::Init(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Initialisation
	SeeAlso:	Filter
********************************************************************************************/
BOOL CorelPaletteFilter::Init(void)
{
	// Get the OILFilter object
	pOILFilter = new CorelPaletteFileOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	// All ok
	return TRUE;
}


/********************************************************************************************
>	INT32 CorelPaletteFilter::HowCompatible(PathName& Filename, ADDR  HeaderStart,
															UINT32 HeaderSize, UINT32 FileSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter
********************************************************************************************/
INT32 CorelPaletteFilter::HowCompatible(PathName& Filename, ADDR  HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// Check the filetype matches
	if (!pOILFilter->DoesExtensionOfPathNameMatch(&Filename))
		return 0;

	m_FileVersion = CPL_FVU;
	
	// is it a .pal file (textualally based)?
	INT32 result = 0;
	if ((result = HowCompatible4(Filename, HeaderStart, HeaderSize, FileSize)) != 0)
		return result;

	// is it a .cpl file (binary)?
	if ((result = HowCompatible5(Filename, HeaderStart, HeaderSize, FileSize)) != 0)
		return result;

	// it's not a palette file.
	return 0;
}


/********************************************************************************************
>	INT32 CorelPaletteFilter::HowCompatible4(PathName& Filename, ADDR  HeaderStart,
		UINT32 HeaderSize, UINT32 FileSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Examines a file to see how compatable it is with this filter - checks for
				version 3 and 4 .pal files (textual CMYK only)
	SeeAlso:	Filter
********************************************************************************************/
INT32 CorelPaletteFilter::HowCompatible4(PathName& Filename, ADDR  HeaderStart,
		UINT32 HeaderSize, UINT32 FileSize)
{
	char *start = (char *)HeaderStart;
	UINT32 l;

	// is it a .pal file (textualally based?)
	if(start[0] == '"')
	{
		l = 1;
		while(start[l] != '"')
		{
			if(l >= HeaderSize)
			{
				return 0;		// if we over run the header size, it's probably not one of ours...
			}
			if(!_istprint(start[l]))
			{
				return 0;		// a non alphanumerical character
			}

			l++;
		}

		// find the next non-quoted character
		l++;
		while(_istspace(start[l]) && l < HeaderSize)
		  l++;

		// and the next a digit?
		if(isdigit(start[l]))
		{
			m_FileVersion = CPL_FV4;
			return 10;			// well, it's resonably likely to be...
		}
	}

	// it's not a palette file.
	return 0;
}


/********************************************************************************************
>	INT32 CorelPaletteFilter::HowCompatible5(PathName& Filename, ADDR  HeaderStart,
		UINT32 HeaderSize, UINT32 FileSize)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Examines a file to see how compatable it is with this filter - checks for
				version 5 .cpl files (binary)
	SeeAlso:	Filter
********************************************************************************************/
INT32 CorelPaletteFilter::HowCompatible5(PathName& Filename, ADDR  HeaderStart,
		UINT32 HeaderSize, UINT32 FileSize)
{
	// Peter (3/5/96)
	// Ben used to check that the first colour was named (so it failed if first colour didn't have a name!)
	// Changed to check magic word at start of file (extension already checked)

	if (HeaderStart[0]==0xCC && HeaderStart[1]==0xDC)
	{
		m_FileVersion = CPL_FV5;
		return 10;
	}
	else
		return 0;
}


/********************************************************************************************
>	virtual BOOL CorelPaletteFilter::PreImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Ben code
	Created:	7/5/96
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called before import of a Corel palette
********************************************************************************************/
BOOL CorelPaletteFilter::PreImport()
{
	// make sure we know which flavour this palette is
	if (m_FileVersion!=CPL_FV4 && m_FileVersion!=CPL_FV5)
	{
		// call HowCompatable
		BYTE Buffer[1024];

		INT32 FileSize = m_pImportFile->Size();
		INT32 LoadSize = (FileSize<sizeof(Buffer)) ? FileSize : sizeof(Buffer);

		if (m_pImportFile->read(Buffer, LoadSize).bad() || m_pImportFile->seek(0).bad())
			return FALSE;

		PathName WibblePath(_R(IDS_K_COPLFILR_INVALID));
		
		if (HowCompatible(WibblePath, (ADDR)&Buffer, LoadSize, FileSize) != 10)
			ERROR1(FALSE, _R(IDE_CORELPALETTEFILTER_NOTCPLORPAL));
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL CorelPaletteFilter::ImportPalette()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Ben code
	Created:	7/5/96
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called to import a Corel Palette file (.pal or .cpl)
********************************************************************************************/
BOOL CorelPaletteFilter::ImportPalette()
{
	switch(m_FileVersion)
	{
		case CPL_FV4:
			return DoImport4();
			break;

		case CPL_FV5:
			return DoImport5();
			break;

		default:
			ERROR2(FALSE, "Unknown FileVersion - HowCompatible gone horrible wrong or not called?");
			break;
	}
}


/********************************************************************************************
>	BOOL CorelPaletteFilter::DoImport4()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Does the import of a Corel .pal file
********************************************************************************************/
BOOL CorelPaletteFilter::DoImport4()
{
	StartPercentage();

	// init lex handling of the file
	if (!m_pImportFile->InitLexer())
		return FALSE;

	INT32 count = 0;			// count of colours so far
	while (!m_pImportFile->eof())
	{
		// check file is OK
		if (m_pImportFile->bad())
			return FALSE;

		// Read a token
		m_pImportFile->GetLineToken();
		const TCHAR* Line = m_pImportFile->GetTokenBuf();

		// is it a quoted name?
		if (Line[0] != '"')    				
			continue;

		// extract the string name
		String_32 ColName;
		INT32 l = 1;
		while (Line[l] != '"' && l < CPF_MAX_COLNAME_SIZE)
		{
			((TCHAR*)ColName)[l - 1] = Line[l];
			l++;
		}
		((TCHAR*)ColName)[l - 1] = '\0';				// terminate it nicely

		// find the end of the string name
		while(Line[l] != '"' && Line[l] != '\0')
		  l++;

		l++;	// skip "

		// Read the CMYK colour components
		INT32 c, m, y, k;
		if (_stscanf(Line + l, "%d %d %d %d", &c, &m, &y, &k) == 4)
		{
			if (!ProcessCMYKColour(c/100.0, m/100.0, y/100.0, k/100.0, &ColName))
				return FALSE;

			count++;
		}

		SetPercentage();
	}

	if (count == 0)		// no colour imported... oh dear, something was horrible wrong with that file
		ERROR1(FALSE, _R(IDE_CORELPALETTEFILTER_BADPALFILE));

	return TRUE;
}


/********************************************************************************************
>	BOOL CorelPaletteFilter::DoImport5()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15 03 95
	Inputs:		As for the Filter class
	Returns:	As for the Filter class
	Purpose:	Does the import of a Corel .cpl file
********************************************************************************************/

#define MAX_PREFIX_LEN		16
#define PREFIX_PANTONE		"PANTONE "
#define PREFIX_FOCOLTONE	"FOCOLTONE "
#define PREFIX_TRUMATCH		"TRUMATCH "
#define PREFIX_UNIFORM		"RGB "
#define FILENAME_FOCOLTONE	"FOCOLTON.CPL"
#define FILENAME_TRUMATCH	"TRUMATCH.CPL"
#define FILENAME_UNIFORM	"RGBSTD.CPL"
#define ENTRIES_PANTONE		3006
#define ENTRIES_FOCOLTONE	762
#define ENTRIES_TRUMATCH	2104
#define ENTRIES_UNIFORM		480

BOOL CorelPaletteFilter::DoImport5()
{
	char ColName[CPF_MAX_COLNAME_SIZE + MAX_PREFIX_LEN];
	cpl_header hdr;				// file header
	INT32 Len;					// how many of those bytes we're going to use
	INT32 l;
	INT32 ColNameOffset;			// where to start adding the colour name - allows prefixes

	// Set the progress indicator, this next bit might take a while.
	StartPercentage();

	// init lex handling of the file
	if (!m_pImportFile->InitLexer())
		return FALSE;

	INT32 count = 0;
	// get the header of the file
	if (!m_pImportFile->read(&hdr, sizeof(hdr)))
		return FALSE;

	UINT32 Entries = hdr.Entries[0] + (hdr.Entries[1] * 256);

	// depending on the filename, fudge a prefix in somewhere.
	PathName Path;
	String_256 FileName;
	TCHAR *fn;

	if (m_pImportFile->IS_KIND_OF(CCDiskFile))
	{
		Path = ((CCDiskFile*) m_pImportFile)->GetPathName();
	 	FileName = Path.GetFileName();
		fn = FileName;
	}
	else
	{
		FileName = String_256(_R(IDS_K_COPLFILR_UNKNOWN));
		fn = FileName;
	}

	PalettePrefix Prefix = PalettePrefix_None;

	if(Entries == ENTRIES_FOCOLTONE && _tcsncicmp(fn, FILENAME_FOCOLTONE, 16) == 0)
		Prefix = PalettePrefix_Focoltone;
	else if(Entries == ENTRIES_TRUMATCH && _tcsncicmp(fn, FILENAME_TRUMATCH, 16) == 0)
		Prefix = PalettePrefix_Trumatch;
	else if(Entries == ENTRIES_UNIFORM && _tcsncicmp(fn, FILENAME_UNIFORM, 16) == 0)
		Prefix = PalettePrefix_RGB;

	ColNameOffset = 0;
	if (!ImportIntoGallery)
	{
		// When importing into the gallery, we save an awful lot of memory
		// by just recording the prefix type rather than the entire prefix string
		switch (Prefix)
		{
			case PalettePrefix_Pantone:
				_tcscpy(ColName, PREFIX_PANTONE);
				ColNameOffset = sizeof(PREFIX_PANTONE) - 1;
				break;

			case PalettePrefix_Focoltone:
				_tcscpy(ColName, PREFIX_FOCOLTONE);
				ColNameOffset = sizeof(PREFIX_FOCOLTONE) - 1;
				break;

			case PalettePrefix_Trumatch:
				_tcscpy(ColName, PREFIX_TRUMATCH);
				ColNameOffset = sizeof(PREFIX_TRUMATCH) - 1;
				break;

			case PalettePrefix_RGB:
				_tcscpy(ColName, PREFIX_UNIFORM);
				ColNameOffset = sizeof(PREFIX_UNIFORM) - 1;
				break;
		}
	}

	// try and import hdr.Entries colours
	for (UINT32 Entry = 0; Entry < Entries; Entry++)
	{
		// OK, the format of each entry is...
		// a cdrfColour
		// a byte giving the length of the name
		// n bytes of name. No terminator.

		// get the colour definition from the file
		cdrfColour ColIn;
		if (!m_pImportFile->read(&ColIn, sizeof(ColIn)))
			return FALSE;

		// convert it
		ImportedNewColour NewCol;
		CDRColour::Convert5(&ColIn, &NewCol);	// return code can be ignored

		// get the name
		BYTE NameCount;				// count of bytes of name in the file
		if (!m_pImportFile->read(&NameCount, sizeof(NameCount)))
			return FALSE;

		Len = __min(NameCount, CPF_MAX_COLNAME_SIZE - 1);
 		if (!m_pImportFile->read(((char *)&ColName) + ColNameOffset, Len))
			return FALSE;
		ColName[Len + ColNameOffset] = '\0';
		if (Len != NameCount)				// need to skip any unread bytes?
			m_pImportFile->seek(m_pImportFile->tell() + (NameCount - Len));

		// check the name
		for (l = ColNameOffset; l < (Len + ColNameOffset); l++)
		{
			if (!_istprint(ColName[l]))
				ColName[l] = '\0';
		}

		// create the colour
		String_64 ColNameS(ColName);

		if (ImportIntoGallery)
		{
			BOOL NewlineAfterThisColour = FALSE;

			// In the pantone file, if the colour name ends in a "9" character, it's the last colour
			// in a pantone page, so we let it know that it should place a newline after itself.
			if (Prefix == PalettePrefix_Pantone)
			{
				String_8 LastChar;
				ColNameS.Right((StringBase *)&LastChar, 1);
				if (LastChar == String_16(TEXT("9")))
					NewlineAfterThisColour = TRUE;
			}

			AddColourToGallery(Prefix, &ColNameS, &NewCol.Colour, NewCol.Model,
								NewlineAfterThisColour);
		}
		else
		{
			if (!m_pNewColours->AddColour(&ColNameS, &NewCol))
				return FALSE;
		}

		count++;
		SetPercentage();
	}

	if (count == 0)		// no colour imported... oh dear, something was horrible wrong with that file
		ERROR1(FALSE, _R(IDE_CORELPALETTEFILTER_BADCPLFILE));

	return TRUE;
}


/********************************************************************************************
>	BOOL CorelPaletteFilter::PostImport(void)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> - from Ben code
	Created:	7/5/96
	Inputs:		-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Called after import of a Corel palette file
********************************************************************************************/
BOOL CorelPaletteFilter::PostImport(void)
{
	m_FileVersion = CPL_FVU;

	return TRUE;
}





/********************************************************************************************
>	MSPaletteFilter::MSPaletteFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for a Microsoft Windows palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
MSPaletteFilter::MSPaletteFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_FILTERNAME_WINDOWSPALETTE));
	FilterInfo.Load(_R(IDT_FILTERINFO_WINDOWSPALETTE));
	FilterID = FILTERID_MSPALETTE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#endif
}


/********************************************************************************************
>	MSPaletteFilter::~MSPaletteFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for a Microsoft Windows palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
MSPaletteFilter::~MSPaletteFilter()
{
}



/********************************************************************************************
>	virtual BOOL MSPaletteFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		-
	Returns:	TRUE for successful init, FALSE if error occured
	Purpose:	Initialise the Windows palette filter, attaches OIL filter
	SeeAlso:	Filter::Init
********************************************************************************************/
BOOL MSPaletteFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new MSPaletteOILFilter(this);
	
	return (pOILFilter != NULL);
}


/********************************************************************************************
>	INT32 MSPaletteFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/96
	Inputs:		Filename - the file being loads (allows checks on extensions).
				HeaderStart - address of some bytes from the start of the file
				HeaderSize - the number of bytes at HeaderStart
				FileSize - the total length of the file
	Returns:	Between 10 (Mine!) and 0 (Not Mine!) inclusive
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter::HowCompatible
********************************************************************************************/
INT32 MSPaletteFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// MS palette files have a well defined format.  Bytes 0-3 = "RIFF".  Bytes 8-15 = "PAL data"

	// Check we have enough of the file header to look at
	if (HeaderSize < 16)
	{
		TRACE( _T("MSPalette filter needs 16+ bytes in HowCompatible\n"));
		return 0;
	}

	if ( 	HeaderStart[0] == 0x52 &&		// R
			HeaderStart[1] == 0x49 &&		// I
			HeaderStart[2] == 0x46 &&		// F
			HeaderStart[3] == 0x46 &&		// F
			HeaderStart[8] == 0x50 &&		// P
			HeaderStart[9] == 0x41 &&		// A
			HeaderStart[10] == 0x4C &&		// L
			HeaderStart[11] == 0x20 &&		// <Space>
			HeaderStart[12] == 0x64 &&		// d
			HeaderStart[13] == 0x61 &&		// a
			HeaderStart[14] == 0x74 &&		// t
			HeaderStart[15] == 0x61 )		// a
		return 10;
	else
		return 0;
}


/********************************************************************************************
>	BOOL MSPaletteFilter::PreImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	Sets import member vars
	Returns:	TRUE/FALSE for success/error
	Purpose:	Reads the MS palette file header to see how many colours are in the file
********************************************************************************************/
BOOL MSPaletteFilter::PreImport()
{
	ERROR2IF( m_pImportFile==NULL, FALSE, "NULL pointer");

	// Read info out of the header.
	BYTE HeaderBuffer[24];
	m_pImportFile->read(HeaderBuffer, 24);

	if (m_pImportFile->good())
	{
		m_NumToImport = *((WORD*)(HeaderBuffer+22));
		return TRUE;
	}
	else
	{
		m_NumToImport = 0;
		return FALSE;
	}
}


/********************************************************************************************
>	BOOL MSPaletteFilter::ImportPalette()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Imports colours from a MS palette file.
********************************************************************************************/
BOOL MSPaletteFilter::ImportPalette()
{
	UINT32 ColoursToRead = m_NumToImport;
	BOOL ok = StartPercentage(m_NumToImport);

	while ( ok && (ColoursToRead > 0) && m_pImportFile->good() )
	{
		// Read the RGB values for this colour from the file
		BYTE ColourDef[4];
		m_pImportFile->read(ColourDef, 4);

		// Add the colour to the imported colours list
		if (m_pImportFile->good())
			ok = ProcessRGBColour(ColourDef[0]/(double)0xff, ColourDef[1]/(double)0xff, ColourDef[2]/(double)0xff);

		SetPercentage(m_NumToImport-ColoursToRead);
		ColoursToRead--;
	}

	return ok && m_pImportFile->good();
}


/********************************************************************************************
>	BOOL MSPaletteFilter::PostImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Called to allow post import cleanup (if required) for this filter
********************************************************************************************/
BOOL MSPaletteFilter::PostImport()
{
	// Nothing to do, no cleanup required.
	return TRUE;
}







/********************************************************************************************
>	AdobeColourTableFilter::AdobeColourTableFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for a Adobe Colour Table palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
AdobeColourTableFilter::AdobeColourTableFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_FILTERNAME_ADOBECOLOURTABLE));
	FilterInfo.Load(_R(IDT_FILTERINFO_ADOBECOLOURTABLE));
	FilterID = FILTERID_ADOBECOLOURTABLE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#endif
}


/********************************************************************************************
>	AdobeColourTableFilter::~AdobeColourTableFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for a Adobe Colour Table palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
AdobeColourTableFilter::~AdobeColourTableFilter()
{
}



/********************************************************************************************
>	virtual BOOL AdobeColourTableFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Returns:	TRUE for successful init, FALSE if error occured
	Purpose:	Initialise the filter, attaches OIL filter
	SeeAlso:	Filter::Init
********************************************************************************************/
BOOL AdobeColourTableFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new AdobeColourTableOILFilter(this);
	
	return (pOILFilter != NULL);
}


/********************************************************************************************
>	INT32 AdobeColourTableFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		Filename - the file being loads (allows checks on extensions).
				HeaderStart - address of some bytes from the start of the file
				HeaderSize - the number of bytes at HeaderStart
				FileSize - the total length of the file
	Returns:	Between 10 (Mine!) and 0 (Not Mine!) inclusive
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter::HowCompatible
********************************************************************************************/
INT32 AdobeColourTableFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// Adobe colour tables are just raw data.  We have to go on file extension
	ERROR2IF(pOILFilter==NULL, 0, "No oil filter present");
	return ((AdobeColourTableOILFilter*)pOILFilter)->HowCompatible(Filename);
}


/********************************************************************************************
>	BOOL AdobeColourTableFilter::PreImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	Sets import member vars
	Returns:	TRUE/FALSE for success/error
	Purpose:	Reads the MS palette file header to see how many colours are in the file
********************************************************************************************/
BOOL AdobeColourTableFilter::PreImport()
{
	// No setup required
	return TRUE;
}


/********************************************************************************************
>	BOOL AdobeColourTableFilter::ImportPalette()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Imports colours from an Abode Colour Table
********************************************************************************************/
BOOL AdobeColourTableFilter::ImportPalette()
{
	ERROR2IF(m_pImportFile==NULL, FALSE, "NULL pointer");

	BOOL ok = StartPercentage();

	while ( ok && m_pImportFile->good() && (m_pImportFile->tell()<m_pImportFile->Size()) )
	{
		// Read the RGB values for this colour from the file
		BYTE ColourDef[3];
		m_pImportFile->read(ColourDef, 3);

		// Add the colour to the imported colours list
		if (m_pImportFile->good())
			ok = ProcessRGBColour(ColourDef[0]/(double)0xff, ColourDef[1]/(double)0xff, ColourDef[2]/(double)0xff);

		SetPercentage();
	}

	return ok && m_pImportFile->good();
}


/********************************************************************************************
>	BOOL AdobeColourTableFilter::PostImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Called to allow post import cleanup (if required) for this filter
********************************************************************************************/
BOOL AdobeColourTableFilter::PostImport()
{
	// Nothing to do, no cleanup required.
	return TRUE;
}







/********************************************************************************************
>	AdobeColourSwatchFilter::AdobeColourSwatchFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for a Adobe Colour Swatch palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
AdobeColourSwatchFilter::AdobeColourSwatchFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_FILTERNAME_ADOBECOLOURSWATCH));
	FilterInfo.Load(_R(IDT_FILTERINFO_ADOBECOLOURSWATCH));
	FilterID = FILTERID_ADOBECOLOURSWATCH;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#endif
}


/********************************************************************************************
>	AdobeColourSwatchFilter::~AdobeColourSwatchFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for a Adobe Colour Swatch palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
AdobeColourSwatchFilter::~AdobeColourSwatchFilter()
{
}



/********************************************************************************************
>	virtual BOOL AdobeColourSwatchFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		-
	Returns:	TRUE for successful init, FALSE if error occured
	Purpose:	Initialise the filter, attaches OIL filter
	SeeAlso:	Filter::Init
********************************************************************************************/
BOOL AdobeColourSwatchFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new AdobeColourSwatchOILFilter(this);
	
	return (pOILFilter != NULL);
}


/********************************************************************************************
>	INT32 AdobeColourSwatchFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		Filename - the file being loads (allows checks on extensions).
				HeaderStart - address of some bytes from the start of the file
				HeaderSize - the number of bytes at HeaderStart
				FileSize - the total length of the file
	Returns:	Between 10 (Mine!) and 0 (Not Mine!) inclusive
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter::HowCompatible
********************************************************************************************/
INT32 AdobeColourSwatchFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// Adobe colour swatchs appear to be just raw data.  We have to go on file extension
	ERROR2IF(pOILFilter==NULL, 0, "No oil filter present");
	return ((AdobeColourSwatchOILFilter*)pOILFilter)->HowCompatible(Filename);
}


/********************************************************************************************
>	BOOL AdobeColourSwatchFilter::PreImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		-
	Outputs:	Sets import member vars
	Returns:	TRUE/FALSE for success/error
	Purpose:	Reads the MS palette file header to see how many colours are in the file
********************************************************************************************/
BOOL AdobeColourSwatchFilter::PreImport()
{
	ERROR2IF( m_pImportFile==NULL, FALSE, "NULL pointer");
	ERROR3IF( sizeof(WORD)!=2, "How come a WORD is not two bytes?");

	m_MetUnknownModel = FALSE;

	// Read info out of the header.
	WORD HeaderBuffer[2];
	m_pImportFile->read(HeaderBuffer, 4);

	if (m_pImportFile->good())
	{
		m_NumToImport = ReverseWord(HeaderBuffer[1]);
		return TRUE;
	}
	else
	{
		m_NumToImport = 0;
		return FALSE;
	}
}


/********************************************************************************************
>	BOOL AdobeColourSwatchFilter::ImportPalette()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Imports colours from an Adobe colour swatch
********************************************************************************************/

BOOL AdobeColourSwatchFilter::ImportPalette()
{
	ERROR2IF(m_pImportFile==NULL, FALSE, "NULL pointer");

	BOOL ok = StartPercentage(m_NumToImport);
	UINT32 StillToRead = m_NumToImport;

	while ( ok && m_pImportFile->good() && StillToRead>0 )
	{
		// Read the values for this colour from the file
		WORD ColourDef[5];
		m_pImportFile->read(ColourDef, 10);

		// Add the colour to the imported colours list
		if (m_pImportFile->good())
		{
			switch (ReverseWord(ColourDef[0]))
			{
				case 0 : 	ok = ProcessRGBColour(ReverseWord(ColourDef[1])/(double)0xFFFF,
													ReverseWord(ColourDef[2])/(double)0xFFFF,
													ReverseWord(ColourDef[3])/(double)0xFFFF );
							break;

				case 1 : 	ok = ProcessHSVColour(ReverseWord(ColourDef[1])/(double)0xFFFF,
													ReverseWord(ColourDef[2])/(double)0xFFFF,
													ReverseWord(ColourDef[3])/(double)0xFFFF );
							break;

				case 2 : 	ok = ProcessCMYKColour((0xFFFF-ReverseWord(ColourDef[1]))/(double)0xFFFF,
													(0xFFFF-ReverseWord(ColourDef[2]))/(double)0xFFFF,
				 									(0xFFFF-ReverseWord(ColourDef[3]))/(double)0xFFFF,
				 									(0xFFFF-ReverseWord(ColourDef[4]))/(double)0xFFFF );
							break;

				case 7 : 	ok = ProcessLabColour(ReverseWord(ColourDef[1])/100.0,
													(signed short)(ReverseWord(ColourDef[2]))/100.0,
				 									(signed short)(ReverseWord(ColourDef[3]))/100.0 );
							break;

				case 8 : 	ok = ProcessGreyColour((0x2710-ReverseWord(ColourDef[1]))/10000.0);
							break;

				default :	if (!m_MetUnknownModel)
							{
								InformWarning(_R(IDE_PALETTEIMPORT_ACOUNKNOWN));
								m_MetUnknownModel = TRUE;
							}
							break;
			}
		}

		SetPercentage(m_NumToImport-StillToRead);
		StillToRead--;
	}

	return ok && m_pImportFile->good();
}


/********************************************************************************************
>	BOOL AdobeColourSwatchFilter::PostImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Called to allow post import cleanup (if required) for this filter
********************************************************************************************/
BOOL AdobeColourSwatchFilter::PostImport()
{
	// Nothing to do, no cleanup required.
	return TRUE;
}


/********************************************************************************************
>	WORD AdobeColourSwatchFilter::ReverseWord(WORD Orig)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/96
	Inputs:		Orig - a word with the 'wrong' byte order
	Outputs:	-
	Returns:	A word with the two bytes switched
	Purpose:	WORDs in swatch files have the wrong byte order.  This function swaps the bytes.
********************************************************************************************/
WORD AdobeColourSwatchFilter::ReverseWord(WORD Orig)
{
	BYTE NewLow = (Orig & 0xFF00) >> 8;
	Orig = Orig << 8;
	
	return Orig | NewLow;
}





/********************************************************************************************
>	PaintShopProPaletteFilter::PaintShopProPaletteFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor for a PaintShopPro palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
PaintShopProPaletteFilter::PaintShopProPaletteFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_FILTERNAME_PSPPALETTE));
	FilterInfo.Load(_R(IDT_FILTERINFO_PSPPALETTE));
	FilterID = FILTERID_PSPPALETTE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
	Flags.CanExport = FALSE;
#else
	Flags.CanImport = FALSE;
	Flags.CanExport = FALSE;
#endif

#ifndef DO_EXPORT
	Flags.CanExport = FALSE;
#endif
}


/********************************************************************************************
>	PaintShopProPaletteFilter::~PaintShopProPaletteFilter()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor for a PaintShopPro palette filter
	SeeAlso:	PaletteFilter
********************************************************************************************/
PaintShopProPaletteFilter::~PaintShopProPaletteFilter()
{
}



/********************************************************************************************
>	virtual BOOL PaintShopProPaletteFilter::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		-
	Returns:	TRUE for successful init, FALSE if error occured
	Purpose:	Initialise the filter, attaches OIL filter
	SeeAlso:	Filter::Init
********************************************************************************************/
BOOL PaintShopProPaletteFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new PSPPaletteOILFilter(this);
	
	return (pOILFilter != NULL);
}


/********************************************************************************************
>	INT32 PaintShopProPaletteFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		Filename - the file being loads (allows checks on extensions).
				HeaderStart - address of some bytes from the start of the file
				HeaderSize - the number of bytes at HeaderStart
				FileSize - the total length of the file
	Returns:	Between 10 (Mine!) and 0 (Not Mine!) inclusive
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter::HowCompatible
********************************************************************************************/
INT32 PaintShopProPaletteFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// Check we have enough of the file header to look at
	if (HeaderSize < 8)
	{
		TRACE( _T("PaintShopProPaletteFilter filter needs 8+ bytes in HowCompatible\n"));
		return 0;
	}

	if ( 	HeaderStart[0] == 0x4A &&		// J
			HeaderStart[1] == 0x41 &&		// A
			HeaderStart[2] == 0x53 &&		// S
			HeaderStart[3] == 0x43 &&		// C
			HeaderStart[4] == 0x2D &&		// -
			HeaderStart[5] == 0x50 &&		// P
			HeaderStart[6] == 0x41 &&		// A
			HeaderStart[7] == 0x4C )		// L
		return 10;
	else
		return 0;
}


/********************************************************************************************
>	BOOL PaintShopProPaletteFilter::PreImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		-
	Outputs:	Sets import member vars
	Returns:	TRUE/FALSE for success/error
	Purpose:	Reads the PSP palette file header to see how many colours are in the file
********************************************************************************************/
BOOL PaintShopProPaletteFilter::PreImport()
{
	ERROR2IF( m_pImportFile==NULL, FALSE, "NULL pointer");

	if (!m_pImportFile->InitLexer())
		return FALSE;

	// Read the two header lines
	INT32 ReadLines = 0;
	while (ReadLines<2 && m_pImportFile->good())
	{
		m_pImportFile->GetLineToken();
		LexTokenType TokType = m_pImportFile->GetTokenType();
		if (TokType == TOKEN_LINE)
			ReadLines++;
	}

	// Read the number of colours line
	if (m_pImportFile->good())
		m_pImportFile->GetLineToken();
	if (m_pImportFile->good())
	{
		const TCHAR* Line = m_pImportFile->GetTokenBuf();
		// BODGE - does this work in UNICODE builds?
		_stscanf(Line, "%d", &m_NumToImport);
	}

	return m_pImportFile->good();
}


/********************************************************************************************
>	BOOL PaintShopProPaletteFilter::ImportPalette()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Imports colours from an Adobe colour swatch
********************************************************************************************/
BOOL PaintShopProPaletteFilter::ImportPalette()
{
	ERROR2IF(m_pImportFile==NULL, FALSE, "NULL pointer");

	BOOL ok = StartPercentage(m_NumToImport);
	UINT32 StillToRead = m_NumToImport;

	while ( ok && StillToRead>0 )
	{
		// Read the colour definition line
		ok = m_pImportFile->GetLineToken();

		// Add the colour to the imported colours list
		const TCHAR* Line = m_pImportFile->GetTokenBuf();
		UINT32 Red = 0;
		UINT32 Green = 0;
		UINT32 Blue = 0;
		INT32 result = _stscanf(Line, "%d %d %d", &Red, &Green, &Blue);

		if (result != 0)
			ok = ProcessRGBColour(Red/(double)0xFF, Green/(double)0xFF, Blue/(double)0xFF);

		SetPercentage(m_NumToImport-StillToRead);
		StillToRead--;
	}

	return ok;
}



/********************************************************************************************
>	BOOL PaintShopProPaletteFilter::PostImport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Called to allow post import cleanup (if required) for this filter
********************************************************************************************/
BOOL PaintShopProPaletteFilter::PostImport()
{
	ERROR2IF( m_pImportFile==NULL, FALSE, "NULL pointer");

	m_pImportFile->DeinitLexer();

	return TRUE;
}




#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)



/********************************************************************************************
>	JCWColourFilter::JCWColourFilter()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Constructor

				The JCWPalette filter loads my own personal file format, which I invented
				to store PANTONE libraries in a reasonably sensible form.

	SeeAlso:	PaletteFilter
********************************************************************************************/
JCWColourFilter::JCWColourFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_FILTERNAME_JCWPALETTE));
	FilterInfo.Load(_R(IDT_FILTERINFO_JCWPALETTE));
	FilterID = FILTERID_JCWPALETTE;

#ifndef STANDALONE
	Flags.CanImport = TRUE;
#else
	Flags.CanImport = FALSE;
#endif

	// Never export, 'cos JCW is the only dude who knows the format! ;-)
	Flags.CanExport = FALSE;

	// And never show this filter in the UI
	Flags.ShowFilter = FALSE;

	m_ColoursAreSpots = FALSE;
	m_NumToImport = 0;
}


/********************************************************************************************
>	JCWColourFilter::~JCWColourFilter()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Returns:	-
	Purpose:	Destructor
	SeeAlso:	PaletteFilter
********************************************************************************************/
JCWColourFilter::~JCWColourFilter()
{
}



/********************************************************************************************
>	virtual BOOL JCWColourFilter::Init()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Returns:	TRUE for successful init, FALSE if error occured
	Purpose:	Initialise the filter, attaches OIL filter
	SeeAlso:	Filter::Init
********************************************************************************************/
BOOL JCWColourFilter::Init()
{
	// Get the OILFilter object
	pOILFilter = new JCWColourOILFilter(this);
	
	return (pOILFilter != NULL);
}


/********************************************************************************************
>	INT32 JCWColourFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		Filename - the file being loads (allows checks on extensions).
				HeaderStart - address of some bytes from the start of the file
				HeaderSize - the number of bytes at HeaderStart
				FileSize - the total length of the file
	Returns:	Between 10 (Mine!) and 0 (Not Mine!) inclusive
	Purpose:	Examines a file to see how compatable it is with this filter.
	SeeAlso:	Filter::HowCompatible
********************************************************************************************/
INT32 JCWColourFilter::HowCompatible(PathName& Filename, ADDR HeaderStart, UINT32 HeaderSize, UINT32 FileSize)
{
PORTNOTE("byteorder", "TODO: Check byte ordering")
	// Adobe colour swatchs appear to be just raw data.  We have to go on file extension
	ERROR2IF(pOILFilter==NULL, 0, "No oil filter present");

	if (HeaderSize < 4)
	{
		TRACE( _T("JCWColourFilter needs 4 or more bytes in HowCompatible\n"));
		return 0;
	}

	if (((JCWColourOILFilter*)pOILFilter)->HowCompatible(Filename))
	{
		BYTE *Hdr = (BYTE *)HeaderStart;
		if (Hdr[0] == 'J' && Hdr[1] == 'C' && Hdr[2] == 'W' && Hdr[3] > 0)
			return(9);
	}

	return(0);
}


/********************************************************************************************
>	BOOL JCWColourFilter::PreImport()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Outputs:	Sets import member vars
	Returns:	TRUE/FALSE for success/error
	Purpose:	Reads the MS palette file header to see how many colours are in the file
********************************************************************************************/
BOOL JCWColourFilter::PreImport()
{
	ERROR2IF( m_pImportFile==NULL, FALSE, "NULL pointer");

	// Read info out of the header.
	BYTE HeaderBuffer[8];
	m_pImportFile->read(HeaderBuffer, 8);

	if (m_pImportFile->good() && HeaderBuffer[0] == 'J' && HeaderBuffer[1] == 'C' && HeaderBuffer[2] == 'W' && HeaderBuffer[3] == 1)
	{
		m_NumToImport = HeaderBuffer[4] + (HeaderBuffer[5] << 8);
		m_ColoursAreSpots = (HeaderBuffer[6] & 0x01) ? TRUE : FALSE;
		// HeaderBuffer[7] is currently reserved, and set to zero
		return TRUE;
	}

	m_NumToImport = 0;
	return FALSE;
}


/********************************************************************************************
>	BOOL JCWColourFilter::ImportPalette()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Imports colours from an Adobe colour swatch
********************************************************************************************/

BOOL JCWColourFilter::ImportPalette()
{
	BOOL ok = StartPercentage(m_NumToImport);
	UINT32 Index = 0;

	BYTE ColourDef[8];
	String_64 Name;

	while (ok && m_pImportFile->good() && Index < m_NumToImport)
	{
		// Read the values for this colour from the file
		m_pImportFile->read(ColourDef, 8);
		m_pImportFile->read((TCHAR *)Name, 16);

		// Add the colour to the imported colours list
		if (m_pImportFile->good())
		{
			ColourCMYK NewColour;
			NewColour.Cyan		= (double) ((ColourDef[0] + (ColourDef[1] << 8))) / 10000.0;
			NewColour.Magenta	= (double) ((ColourDef[2] + (ColourDef[3] << 8))) / 10000.0;
			NewColour.Yellow	= (double) ((ColourDef[4] + (ColourDef[5] << 8))) / 10000.0;
			NewColour.Key		= (double) ((ColourDef[6] + (ColourDef[7] << 8))) / 10000.0;

			if (ImportIntoGallery)
			{
				// Add the colour to the gallery, as either a process or spot colour (as appropriate)
				// Every 7th colour has a newline after it
				ok = AddColourToGallery(((m_ColoursAreSpots) ? PalettePrefix_PantoneSpot : PalettePrefix_Pantone),
											&Name, (ColourGeneric *)&NewColour, COLOURMODEL_CMYK, (Index % 7 == 6),
											m_ColoursAreSpots);
			}
			else
				ok = m_pNewColours->AddColour(&Name, &NewColour);
		}

		SetPercentage(Index++);
	}

	return ok && m_pImportFile->good();
}


/********************************************************************************************
>	BOOL JCWColourFilter::PostImport()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/error
	Purpose:	Called to allow post import cleanup (if required) for this filter
********************************************************************************************/
BOOL JCWColourFilter::PostImport()
{
	// Nothing to do, no cleanup required.
	return TRUE;
}
#endif
