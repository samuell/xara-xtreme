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
//
// Implementation of the print control class

/*
*/

#include "camtypes.h"
#include "app.h"
#include "colcontx.h"
#include "colourix.h"
#include "colormgr.h"
#include "colplate.h"
#include "camelot.h"
#include "convert.h"
#include "document.h"
#include "docview.h"
//#include "justin2.h"
//#include "markn.h"
#include "osrndrgn.h"
#include "prdlgctl.h"
#include "princomp.h"
#include "printctl.h"
#include "prntview.h"
//#include "resource.h"
#include "rndrgn.h"
//#include "simon.h"
#include "spread.h"
//#include "tim.h"
#include "view.h"
#include "prnmks.h"
//#include "isetres.h"

CC_IMPLEMENT_MEMDUMP(PrintControl,	CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(PrintPatchInfo,CC_CLASS_MEMDUMP)
CC_IMPLEMENT_MEMDUMP(TypesetInfo,	CC_CLASS_MEMDUMP)

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

// This is the max DPI at which bitmaps will be printed when using automatic DPI calculations
#define MAXAUTOBITMAPDPI 200

// These two values determine the range of values that the bitmap DPI will lie within
#define MINBITMAPDPI 10
#define MAXBITMAPDPI 600

// Constants used for initialisation
#define A4_WIDTH	(INT32(MM_MP_VAL*210))	// 210 mm - A4 width
#define A4_HEIGHT	(INT32(MM_MP_VAL*297))	// 297 mm - A4 height

//----------------------------------------------------------------

static INT32 MinPrintScale = 5;
static INT32 MaxPrintScale = 999;

PrintMethodType PrintControl::AppPrintMethod = PRINTMETHOD_NORMAL;	// ini file entry declared in princomp.cpp

//	if ( Camelot.DeclareSection(TEXT("Preferences"), 3) )
//	{
		// section declared ok so now define the preference option  
		//	Camelot.DeclarePref(TEXT("Preferences"), TEXT("MinPrintScale"), &MinPrintScale, 1);
		//	Camelot.DeclarePref(TEXT("Preferences"), TEXT("MaxPrintScale"), &MaxPrintScale, 1);

/******************************************************************************************

>	PrintControl::PrintControl()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintControl::PrintControl()
{
	PrintAreaSetUp = FALSE;	// We haven't set the control up with a print area yet
	SetUpFailed    = FALSE; // We haven't called SetUp yet, so we haven't failed yet
	pSpread = NULL;

	TotalPrintArea 	= DocRect(0,0,A4_WIDTH,A4_HEIGHT);
	PrintableArea	= TotalPrintArea;
	PALeftMargin	= 0;
	PATopMargin		= 0;

	Origin 			= DocCoord(0,A4_HEIGHT);

	PageWidth 	= A4_WIDTH;
	PageHeight 	= A4_HEIGHT;
	PaperWidth 	= A4_WIDTH;
	PaperHeight = A4_HEIGHT;

	DPS				= FALSE;
	Bleed			= 0;
	CropArea		= CROPAREA_SIZE+OUTSIDEBLEEDWIDTH;
	CropTopMargin	= 0;
	CropLeftMargin	= 0;

	// Print layout vars
	WholeSpread = FALSE;

	Scale	= FIXED16(100);
	Orient	= PRINTORIENTATION_UPRIGHT;

	FitType	= PRINTFIT_BESTPAPER;

	TopMargin	= 0;
	LeftMargin	= 0;
	ScaledWidth	= A4_WIDTH;
	ScaledHeight= A4_HEIGHT;

	Rows	= 1;
	Columns = 1;
	Gutter	= 0;

	// Print output vars
	Layers					= PRINTLAYERS_VISIBLEFOREGROUND;
	// Job 10463: remove PS Level bits - default to Level 2
	PSLangLevel				= PSLEVEL_2;
//	PrintMethod				= CurrentPrintMethod;
	BitmapResolutionMethod 	= BITMAPRES_AUTO;
	DotsPerInch				= 150;
	FillQuality				= PRINTFILLQUALITY_MEDIUM;
	TextOptions				= PRINTTEXTOPTIONS_NORMAL;

	// General print vars
	Collated		= FALSE;
	NumCopies	 	= 1;
	PrintToFile 	= FALSE;
	ObjPrintRange	= PRINTRANGEOBJ_ALL;
	DPSPrintRange	= PRINTRANGEDPS_BOTH;

	// Misc
	UpdateDependants = TRUE;

	// Print layout var initialisation
	Printing = FALSE;
};

/******************************************************************************************

>	PrintControl::~PrintControl()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default deconstructor
	Errors:		-
	SeeAlso:	PrintControl()::PrintControl()

******************************************************************************************/

PrintControl::~PrintControl()
{
	// Does nowt as yet
}

/******************************************************************************************

>	String_256 PrintControl::BuildPrintInfoStr()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns the print info string 
	Purpose:	This uses the print control's settings to build the string displayed in the print info
				field of a print dlg
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

String_256 PrintControl::BuildPrintInfoStr()
{
	String_256 Str;

	switch (FitType)
	{
		case PRINTFIT_BEST:		Str = String_256(_R(IDS_PRINT_BESTFIT)); 	break;
		case PRINTFIT_CUSTOM:	Str = String_256(_R(IDS_PRINT_CUSTOMFIT)); 	break;
		case PRINTFIT_MULTIPLE:	Str = String_256(_R(IDS_PRINT_MULTIPLEFIT));break;
		case PRINTFIT_BESTPAPER:Str = String_256(_R(IDS_PRINT_BESTFITPAPER)); 	break;
		default: ERROR3_PF(("Unknown fit type (%d)",FitType)); break;
	}

	char* format = "%.2lf%%";
	double fScale = Scale.MakeDouble();
	INT32 n = INT32((fScale*100)+0.5);

	if (n % 100 == 0)
		format = "%.0lf%%";
	else if (n % 10 == 0)
		format = "%.1lf%%";

	char s[100];
	_stprintf(s,format,fScale);
	Str += s;

	switch (Orient)
	{
		case PRINTORIENTATION_UPRIGHT:	Str += String_256(_R(IDS_PRINT_UPRIGHT));	break;
		case PRINTORIENTATION_SIDEWAYS:	Str += String_256(_R(IDS_PRINT_SIDEWAYS));	break;
		default: ERROR3_PF(("Unknown orientation (%d)",Orient)); break;
	}

	// If no print area set up, then assume we have no printer connected
	if (!PrintAreaSetUp)
	{
		Str += String_256(_R(IDS_PRINT_NOPRINTER));
		return Str;
	}

	// Work out how many pages will get completely printed
	UINT32 NumPages = 1;
	UINT32 IDSPlural   = _R(IDS_PRINT_PAGES);
	UINT32 IDSSingular = _R(IDS_PRINT_PAGE);

	switch (FitType)
	{
		case PRINTFIT_BEST:
		case PRINTFIT_BESTPAPER:
			NumPages = 1;
			break;

		case PRINTFIT_MULTIPLE:
			NumPages = 0;
			if (StartPrinting())
			{
				GetNextPaper();

				PrintPatchInfo PatchInfo;
				while (GetNextPatch(&PatchInfo))
				{
					DocRect Rect = PatchInfo.GetClipRect(FALSE,FALSE);
					double ScaleFactor = PatchInfo.Scale.MakeDouble() / 100;

					INT32 w = Rect.Width();
					INT32 h = Rect.Height();

					if (PatchInfo.Rotate)
					{	INT32 t = w;	w = h;	h = t;	}

					Rect.lox = PatchInfo.XTrans;
					Rect.loy = PatchInfo.YTrans;

					Rect.hix = Rect.lox + (INT32((double(w) * ScaleFactor)+0.5));
					Rect.hiy = Rect.loy + (INT32((double(h) * ScaleFactor)+0.5));

					if (PrintableArea.ContainsRect(Rect))
						NumPages++;
				}

				EndPrinting();
			}
			IDSPlural   = _R(IDS_PRINT_TILES);
			IDSSingular = _R(IDS_PRINT_TILE);
			break;

		case PRINTFIT_CUSTOM:
			{
				DocRect Rect(0,PaperHeight-ScaledHeight,ScaledWidth,PaperHeight);
				
				INT32 CropAdj = GetCropAdjust();
				INT32 TransX = LeftMargin + CropAdj;
				INT32 TransY = TopMargin + CropAdj;
				
				Rect.Translate(TransX,-TransY);
				if (PrintableArea.ContainsRect(Rect))
					NumPages = 1;
				else
					NumPages = 0;
			}
			break;
	}

	if (NumPages >= 1)
	{
		wsprintf(s, TEXT("%ld") ,NumPages);
		Str += s;

		if (NumPages == 1)
			Str += String_256(IDSSingular);
		else
			Str += String_256(IDSPlural);
	}
	else
		Str += String_256(_R(IDS_PRINT_NOPAGESFIT));

	return Str;
}

/******************************************************************************************

>	String_256 PrintControl::BuildPaperSizeStr()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns the paper size string 
	Purpose:	This uses the printer settings to build the string displayed in the paper size
				field of a print options tab
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

String_256 PrintControl::BuildPaperSizeStr()
{
	String_256 Str = TEXT("");
	String_256 TempStr = TEXT("");

	// If no print area set up, then assume we have no printer connected
	if (!PrintAreaSetUp)
		return Str;

	DimScale* pDimScale = NULL;
	if (pSpread != NULL)
		pDimScale = DimScale::GetPtrDimScale(pSpread);

	if (pDimScale != NULL)
	{
		BOOL ActiveState = pDimScale->IsActive();
		pDimScale->SetActiveState(FALSE);

		pDimScale->ConvertToUnits(PaperWidth,&TempStr);
		Str += TempStr;

		Str += String_32(_R(IDS_NUM_BY_NUM));

		pDimScale->ConvertToUnits(PaperHeight,&TempStr);
		Str += TempStr;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
		INT32 PrScale;
		if (CCPrintDialog::GetScale(&PrScale))
		{
			if (PrScale != 100)
			{
				Str += String_256(_R(IDS_PRINT_PRINTERSCALING));

				TCHAR s[100];
				String_32 jcf(_R(IDS_PERCENT_FORMAT));
				wsprintf(s, jcf, (INT32) PrScale);
				Str += s;
			}
		}
#endif //webster
		pDimScale->SetActiveState(ActiveState);
	}

	return Str;
}

/******************************************************************************************

>	PrintControl& PrintControl::operator=(PrintControl& other)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Inputs:		the object to copy
	Outputs:	-
	Returns:	ref to copy of other
	Purpose:	The assignment operator for PrintControl
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintControl& PrintControl::operator=(PrintControl& other)
{
	PrintAreaSetUp 	= other.PrintAreaSetUp;
	SetUpFailed  	= other.SetUpFailed;
	pSpread			= other.pSpread;

	TotalPrintArea 	= other.TotalPrintArea;
	PrintableArea	= other.PrintableArea;
	PALeftMargin	= other.PALeftMargin;
	PATopMargin		= other.PATopMargin;

	Origin			= other.Origin;
	PaperWidth 		= other.PaperWidth;
	PaperHeight		= other.PaperHeight;
	PageWidth		= other.PageWidth;
	PageHeight		= other.PageHeight;	
	OrigPageWidth	= other.OrigPageWidth;
	OrigPageHeight	= other.OrigPageHeight;

	// Print layout vars
	WholeSpread		= other.WholeSpread;
	DPS				= other.DPS;
	Bleed			= other.Bleed;

	CropArea		= other.CropArea;
	CropTopMargin	= other.CropTopMargin;
	CropLeftMargin	= other.CropLeftMargin;

	Scale	= other.Scale;
	Orient	= other.Orient;

	FitType	= other.FitType;

	TopMargin	= other.TopMargin;
	LeftMargin	= other.LeftMargin;
	ScaledWidth	= other.ScaledWidth;
	ScaledHeight= other.ScaledHeight;

	Rows	= other.Rows;
	Columns = other.Columns;
	Gutter	= other.Gutter;

	// Print output vars
	Layers					= other.Layers;
	PSLangLevel				= other.PSLangLevel;
//	PrintMethod				= other.PrintMethod;
	BitmapResolutionMethod 	= other.BitmapResolutionMethod;
	DotsPerInch				= other.DotsPerInch;
	FillQuality				= other.FillQuality;
	TextOptions				= other.TextOptions;

	// General print vars
	Collated		= other.Collated;
	NumCopies	 	= other.NumCopies;
	PrintToFile 	= other.PrintToFile;
	ObjPrintRange	= other.ObjPrintRange;
	DPSPrintRange	= other.DPSPrintRange;

	// The TypesetInfo
	Typesetting = other.Typesetting;

	// Finally, make sure we have calculated all of the print settings like scaling properly
	if (pSpread != NULL)
		SetUp(pSpread);

	return *this;
};

//------------------------------------------------

/******************************************************************************************

>	BOOL PrintControl::SetUp(Spread* pThisSpread, BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		pThisSpread = ptr to a spread to print
				RedrawPrintBorders - True if want a force redraw of associated docs (Defaults to TRUE)
	Outputs:	PrintAreaSetUp has same BOOL value as the return result
	Returns:	TRUE if able to set up print area OR already set up
	Purpose:	This function sets up the print control to print the given spread.

				The entire spread area is used as the source area to print.  It can determine from the spread
				whether it is a double page spread (DPS) or not.

				Once it gets the source area for printing from the spread, it calcs the fit for the spread
				using the current fit type.  I.e. it works out the scale factor and scaled page width
				and height for each page printed, given the current settings.

	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL PrintControl::SetUp(Spread* pThisSpread, BOOL RedrawPrintBorders)
{
	ERROR2IF(pThisSpread == NULL,FALSE,"pThisSpread is NULL");

	pSpread = pThisSpread;

	// find out if it's a double page spread
	if (!pSpread->GetPageSize(NULL,NULL,NULL,&Bleed,&DPS,NULL))
		return FALSE;

	// Get a union of all the pages that represents the total printable area of the spread
	pSpread->GetPagesRect(&TotalPrintArea);

	PrintAreaSetUp = (CalcPrintAreaVars(RedrawPrintBorders) && CalcFit());
	SetUpFailed    = !PrintAreaSetUp;

	return PrintAreaSetUp;
}	

/******************************************************************************************

>	BOOL PrintControl::CalcPrintAreaVars(BOOL RedrawPrintBorders = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		RedrawPrintBorders - True if want a force redraw of associated docs (Defaults to TRUE)
	Outputs:	-
	Returns:	TRUE if able to recalc
	Purpose:	This function calcs member vars dependant on the total print area.
				The print area can be set up using SetUp().

				Dependants are:
					PageWidth
					PageHeight
					Origin
					PrintableArea

				It takes into account orientation, and whether its a double page spread (DPS).
				If it's a DPS, it also takes the WholeSpread setting into account

				NOTE: This func also finds out the width and height of the printer's paper using
				CCPrintDialog::GetPaperSize(), and calcs the paper's area too

	Errors:		-
	SeeAlso:	SetUp()

******************************************************************************************/

BOOL PrintControl::CalcPrintAreaVars(BOOL RedrawPrintBorders)
{
	// Calc the print area origin
	Origin.x = TotalPrintArea.lox;
	Origin.y = TotalPrintArea.hiy;

	//-------------------------
	// Find out the dimensions of the printer's paper (in MILLIPOINTS)
	SIZEL PaperSize;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (!CCPrintDialog::GetPaperSize(&PaperSize, RedrawPrintBorders))
		return FALSE;
#endif //webster
	PaperWidth  = PaperSize.cx;
	PaperHeight = PaperSize.cy;

	// ------------------------
	// Get the printable area on the paper (defined relative to a origin at the bottom left of paper)
	// and the top & left margins of the printable area
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (!CCPrintDialog::GetPrintableArea(&PrintableArea))
		return FALSE;
	if (!CCPrintDialog::GetPrintMargins(&PALeftMargin,&PATopMargin))
		return FALSE;
#endif //webster
	//-------------------------
	// Calc the width and height of the print area
	PageWidth  = TotalPrintArea.hix - TotalPrintArea.lox;
	PageHeight = TotalPrintArea.hiy - TotalPrintArea.loy;

	// If double page spread AND printing single pages, half the width for each page printed
	if (DPS && !WholeSpread) 
		(PageWidth) /= 2;

	// Save the orig values in case PageWidth & PageHeight need to be swapped due to sideways orientation
	OrigPageWidth  = PageWidth;
	OrigPageHeight = PageHeight;

	// If printing sideways, the width and height of the printed page should be swapped
	if (Orient == PRINTORIENTATION_SIDEWAYS)
		Swap(PageWidth,PageHeight);

	return TRUE;
}

/******************************************************************************************

>	void PrintControl::StartImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		-
	Purpose:	Tells the object that it's data is being imported.

				In this state, calling the Set functions just sets the aspect of the object,
				rather than recalculation dependant aspects of the object.

					e.g. calling SetScale() will recalc the width & height of the printed page
					unless you call StartImport() first

				You can call this func any time, not just for importing, if you need to alter
				mmembers independently.

	SeeAlso:	EndImport()

******************************************************************************************/

void PrintControl::StartImport()
{
	UpdateDependants = FALSE;
}

/******************************************************************************************

>	void PrintControl::StartImport()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		-
	Purpose:	Tells the object that it's data is no longer being imported.
	SeeAlso:	StartImport()

******************************************************************************************/

void PrintControl::EndImport()
{
	UpdateDependants = TRUE;
}

//-------------------------


/******************************************************************************************

>	INT32 PrintControl::GetNumPrintablePages()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Inputs:		-
	Returns:	Number of printable pages (1 or 2)
	Purpose:	This tells you how many individual pages are printable, given the current settings

				1 is returned:
					If it's not a DPS
					If it's a DPS, but we're printing the whole spread as one page
					If it's a DPS, we're printing individual pages, but the print range is either Left or Right-only

				2 is returned:
					It's a DPS AND individual pages AND printing both pages

	SeeAlso:	-

******************************************************************************************/

INT32 PrintControl::GetNumPrintablePages()
{
	if (DPS && !WholeSpread && (DPSPrintRange == PRINTRANGEDPS_BOTH))
		return 2;
	else
		return 1;
}

//-------------------------

/******************************************************************************************

>	void PrintControl::SetScaleFactor(double ScaleFactor)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		ScaleFactor = the scale factor
	Purpose:	Sets the scale percentage using the given scale factor
				This alters dependants automatically (unless importing)
				Dependants are:
					ScaledWidth
					ScaledHeight
	SeeAlso:	StartImport() & EndImport()
													
******************************************************************************************/

void PrintControl::SetScaleFactor(double ScaleFactor)
{
	SetScale(FIXED16(100*ScaleFactor));
}

/******************************************************************************************

>	void PrintControl::SetScale(FIXED16 NewScale)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewScale = new scale factor
	Purpose:	Sets the scale percentage, altering dependants automatically (unless importing)
				Dependants are:
					ScaledWidth
					ScaledHeight
	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::SetScale(FIXED16 NewScale)
{
	Scale = NewScale;
	FIXED16 Min = FIXED16(MinPrintScale);
	FIXED16 Max = FIXED16(MaxPrintScale);

	if (Scale < Min) Scale = Min;
	if (Scale > Max) Scale = Max;

	if (UpdateDependants)
	{
		double ScaleFactor = (Scale.MakeDouble())/100;

		ScaledWidth  = INT32((double(PageWidth) *ScaleFactor)+0.5);
		ScaledHeight = INT32((double(PageHeight)*ScaleFactor)+0.5);
	}
}

/******************************************************************************************

>	void PrintControl::SetPrintOrient(PrintOrient NewOrient)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewOrient = new orientation
	Purpose:	Sets the print orientatio, altering dependants automatically (unless importing)

				Dependants are:
					ScaledWidth
					ScaledHeight
					PageWidth
					PageHeight
					Scale

				and possibly:
					Rows
					Columns

	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::SetPrintOrient(PrintOrient NewOrient)
{
	if (Orient != NewOrient)
	{
		Orient = NewOrient;

		if (UpdateDependants)
		{
			CalcPrintAreaVars();	// Update the page width and height (these get swapped)
			SetRows(Rows);			// Use SetRows() to recalc max rows and scale factor
			SetColumns(Columns);	// Use SetColumns() to recalc max columns and scale factor

			// This is a bodge fix.  Sometimes the gutter value can become -ve. In this case,
			// reset the multiple fit vars
			if (Gutter < 0)
			{
				SetRows(1);			// Use SetRows() to recalc max rows and scale factor
				SetColumns(1);		// Use SetColumns() to recalc max columns and scale factor
				SetGutter(0);		
			}
		}
	}
}

/******************************************************************************************

>	void PrintControl::SetWidth(INT32 NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewVal = new width for the print area
	Purpose:	Sets the page width, altering dependants automatically (unless importing)
				Dependants are:
					Scale
					ScaledHeight
	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::SetWidth(INT32 NewVal)
{
	if (UpdateDependants)
	{
		double ScaleFactor = double(NewVal)/double(PageWidth);
		SetScaleFactor(ScaleFactor); 		// Use SetScaleFactor() to recalc ScaledWidth and ScaledHeight
	}
	else
		ScaledWidth = NewVal;
}
	

/******************************************************************************************

>	void PrintControl::SetHeight(INT32 NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewVal = new height for the print area
	Purpose:	Sets the page height, altering dependants automatically (unless importing)
				Dependants are:
					Scale
					ScaledWidth
	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::SetHeight(INT32 NewVal)
{
	if (UpdateDependants)
	{
		double ScaleFactor = double(NewVal)/double(PageHeight);
		SetScaleFactor(ScaleFactor); 		// Use SetScaleFactor() to recalc ScaledWidth and ScaledHeight
	}
	else
		ScaledHeight = NewVal;
}

//-------------------------
//-------------------------
//-------------------------

/******************************************************************************************

>	double PrintControl::CalcPatchWidth(INT32 Wd,INT32 NumColumns, INT32 GutterVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		Wd			= Width of paper
				NumColumns 	= number of columns down the paper
				Gutter		= gutter distance between each page
	Returns:	The width of each patch for multiple fit printing
	Purpose:	The patch width = (width of the paper / num columns) - gutter
	SeeAlso:	-

******************************************************************************************/

double PrintControl::CalcPatchWidth(INT32 Wd,INT32 NumColumns,INT32 GutterVal)
{
	return ((double(Wd) / double(NumColumns)) - double(GutterVal));
}

/******************************************************************************************

>	double PrintControl::CalcPatchHeight(INT32 Ht,INT32 NumRows,INT32 GutterVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		Ht			= Height of paper
				NumRows	= number of rows across the paper
				Gutter	= gutter distance between each page
	Returns:	The Height of each patch for multiple fit printing
	Purpose:	The patch Height = (Height of the paper / num rows) - gutter
	SeeAlso:	-

******************************************************************************************/

double PrintControl::CalcPatchHeight(INT32 Ht,INT32 NumRows,INT32 GutterVal)
{
	return ((double(Ht) / double(NumRows)) - double(GutterVal));
}

/******************************************************************************************

>	INT32 PrintControl::CalcMaxRows(INT32 GutterVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		GutterVal = the gutter value to use
	Returns:	Max num rows for the current paper width and gutter setting
	Purpose:	Dynamically calcs the max num rows possible, given the current paper width
				and gutter setting.

				This is based on the min scale factor allowed
	SeeAlso:	-

******************************************************************************************/

INT32 PrintControl::CalcMaxRows(INT32 GutterVal)
{
	INT32 MaxRows = 0;
	double MinScaleFactor = double(MinPrintScale)/100;
	double PatchHeight,ScaleFactor;

	do
	{
		MaxRows += 1;
		PatchHeight = CalcPatchHeight(PaperHeight,MaxRows,GutterVal);
		ScaleFactor = PatchHeight / PageHeight;
	} while (ScaleFactor >= MinScaleFactor);

	if (MaxRows > 1)
		MaxRows -= 1;

	return MaxRows;
}

/******************************************************************************************

>	INT32 PrintControl::CalcMaxColumns(INT32 GutterVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		GutterVal = the gutter value to use
	Returns:	Max num columns for the current paper height and gutter setting
	Purpose:	Dynamically calcs the max num columns possible, given the current paper height 
				and gutter setting.

				This is based on the min scale factor allowed
	SeeAlso:	-

******************************************************************************************/

INT32 PrintControl::CalcMaxColumns(INT32 GutterVal)
{
	INT32 MaxColumns = 0;
	double MinScaleFactor = double(MinPrintScale)/100;
	double PatchWidth,ScaleFactor;

	do
	{
		MaxColumns += 1;
		PatchWidth = CalcPatchWidth(PaperWidth,MaxColumns,GutterVal);
		ScaleFactor = PatchWidth / PageWidth;
	} while (ScaleFactor >= MinScaleFactor);

	if (MaxColumns > 1)
		MaxColumns -= 1;

	return MaxColumns;
}

/******************************************************************************************

>	INT32 PrintControl::CalcMaxGutter(INT32 NumRows,INT32 NumColumns)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		NumRows		= num rows to base calc on
				NumColumns	= num columns to base calc on
	Returns:	Max gutter value
	Purpose:	Dynamically calcs the max gutter value based on the given number of rows and columns, 
				and the current paper size
	SeeAlso:	-

******************************************************************************************/

INT32 PrintControl::CalcMaxGutter(INT32 NumRows,INT32 NumColumns)
{
	double MinScaleFactor = double(MinPrintScale)/100;

	double PatchWidth  = CalcPatchWidth(PaperWidth,NumColumns,0);
	double PatchHeight = CalcPatchHeight(PaperHeight,NumRows,0);

	INT32 MaxGutterWidth  = INT32((PatchWidth  - (double(PageWidth) *MinScaleFactor))+0.5);
	INT32 MaxGutterHeight = INT32((PatchHeight - (double(PageHeight)*MinScaleFactor))+0.5);

	if (MaxGutterWidth < MaxGutterHeight)
		return MaxGutterWidth;
	else
		return MaxGutterHeight;
}

/******************************************************************************************

>	void PrintControl::CalcMultipleFit(INT32 NumRows,INT32 NumColumns,INT32 GutterVal,INT32 Wd,INT32 Ht)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		NumRows		= num rows to fit on paper
				NumColumns	= num columns to fit on paper
				GutterVal	= gutter distance between each page
				Wd			= Width of paper
				Ht			= Height of paper
				Extra		= An adjustment made to the page size (usually 0)
	Returns:	-
	Purpose:	Recalcs the scale factor required for each page to fit the specified number of rows
				and columns, and gutter distance, to the paper

				Dependants are:
					Scale
					ScaledWidth
					ScaledHeight

	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::CalcMultipleFit(INT32 NumRows,INT32 NumColumns,INT32 GutterVal,INT32 Wd,INT32 Ht,INT32 Extra)
{
	// Calc the width and height of each patch on the page, taking into account the given gutter value
	double PatchWidth  = CalcPatchWidth(Wd,NumColumns,GutterVal);
	double PatchHeight = CalcPatchHeight(Ht,NumRows,GutterVal);

	// Work out the scale factor required to get the width of the page into this patch
	double ScaleFactor = PatchWidth / double(PageWidth+Extra);

	// Calc the new width and height of each page by applying the scale factor
	double NewWidth = PageWidth  * ScaleFactor;
	double NewHeight= PageHeight * ScaleFactor;

	// If the scale factor made using the page width and patch width means that the page does NOT entirely
	// fit into a multiple-fit patch, then use the scale factor generated using the patch height and page height
	//
	// (NB: We add 1.0 to these values so that the '>' comparisons don't fail due to insignificant floating point errors).
	//
	if ((NewWidth > (PatchWidth+1.0)) || (NewHeight > (PatchHeight+1.0)))
		ScaleFactor = PatchHeight / double(PageHeight+Extra);
	
	#ifdef _DEBUG
	{
		// In theory, when the scale factor is applied to the page width and height, the page will fit
		// entirely into a patch.  However, we still better check this is true when in debug builds

		NewWidth = PageWidth  * ScaleFactor;
		NewHeight= PageHeight * ScaleFactor;

		if ((NewWidth > (PatchWidth+1.0)) || (NewHeight > (PatchHeight+1.0)))
		{
			ERROR3("Multiple fit scale factor does not fit page into patch");
		}
	}
	#endif
	
	SetScaleFactor(ScaleFactor); 		// Use SetScaleFactor() to recalc ScaledWidth and ScaledHeight
}

/******************************************************************************************

>	BOOL PrintControl::CalcFit()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		-
	Returns:	TRUE if a recalc took place, FALSE if nothing has been recalced
	Purpose:	Recalcs the scale factor needed to fit the current page in the specified
				orientation, using the currently set fit type

				This will only do anything when UpdateDependants is TRUE

	SeeAlso:	CalcMultipleFit()

******************************************************************************************/

BOOL PrintControl::CalcFit()
{
	static BOOL IgnoreCall = FALSE;

	if (!UpdateDependants)
		return FALSE;

	if (IgnoreCall)		// Stop re-entrant calls to this function
		return TRUE;

	IgnoreCall = TRUE;

	// Set these value to absolutely ziltch
	CropTopMargin=0;
	CropLeftMargin=0;

	switch (FitType)
	{
		case PRINTFIT_MULTIPLE:
			{
				INT32 PAWidth  = PaperWidth;
				INT32 PAHeight = PaperHeight;

				INT32 CropAdjust = GetCropAdjust();

				PAWidth -= (CropAdjust<<1);
				PAHeight -= (CropAdjust<<1);

				CalcMultipleFit(Rows,Columns,Gutter,PAWidth,PAHeight,0);
				
				// these settings just show to the user whats happening in the dialogue
				// they do not get used explicitly to position the pages.
				if ((Rows==1) && (Columns==1))
				{
					LeftMargin = (PaperWidth -ScaledWidth) /2;
					TopMargin  = (PaperHeight-ScaledHeight)/2;
				}
			}
			break;

		case PRINTFIT_BEST:
			{
				INT32 PAWidth  = PrintableArea.Width();
				INT32 PAHeight = PrintableArea.Height();

				INT32 CropAdjust = GetCropAdjust();

				PAWidth -= (CropAdjust<<1);
				PAHeight -= (CropAdjust<<1);

				BOOL PAOrient 	= ((double(PAWidth) 		/ double(PAHeight))		  <= 1.0);
				BOOL PageOrient	= ((double(OrigPageWidth)	/ double(OrigPageHeight ))<= 1.0);

				PrintOrient NewOrient;

				if (PAOrient == PageOrient)
					NewOrient = PRINTORIENTATION_UPRIGHT;
				else
					NewOrient = PRINTORIENTATION_SIDEWAYS;

				if (NewOrient != Orient)
					SetPrintOrient(NewOrient);

				CalcMultipleFit(1,1,0,PAWidth,PAHeight,0);

				PAWidth += (CropAdjust<<1);
				PAHeight += (CropAdjust<<1);

				LeftMargin = PALeftMargin+((PAWidth -ScaledWidth) /2);
				TopMargin  = PATopMargin +((PAHeight-ScaledHeight)/2);
			}
			break;

		case PRINTFIT_CUSTOM:
			{
				INT32 CropAdjust = GetCropAdjust();
				CropLeftMargin = CropAdjust;
				CropTopMargin = CropAdjust;
				SetScale(Scale);
			}
			break;

		case PRINTFIT_BESTPAPER:
			// Best fit the document paper size onto the printed page
			// without scaling, distortion or clipping
			{
				INT32 PAWidth  = PaperWidth;
				INT32 PAHeight = PaperHeight;

				INT32 CropAdjust = GetCropAdjust();

				PAWidth -= (CropAdjust<<1);
				PAHeight -= (CropAdjust<<1);

				// Compute aspect of document and aspect of printed page
				// TRUE means rect is portrait orientation, taller than it is wide...
				BOOL PAOrient 	= (double(PAWidth) 			<= double(PAHeight));
				BOOL PageOrient	= (double(OrigPageWidth)	<= double(OrigPageHeight));

				PrintOrient NewOrient;

				if (PAOrient == PageOrient)
					NewOrient = PRINTORIENTATION_UPRIGHT;
				else
					NewOrient = PRINTORIENTATION_SIDEWAYS;

				if (NewOrient != Orient)
					SetPrintOrient(NewOrient);

				CropLeftMargin = CropAdjust;
				CropTopMargin = CropAdjust;
				SetScale(100.0);

//				PAWidth += (CropAdjust<<1);
//				PAHeight += (CropAdjust<<1);

				// Centre document page within printed page
				LeftMargin = (PAWidth - ScaledWidth) / 2;
				TopMargin = (PAHeight - ScaledHeight) / 2;
			}
			break;

		default:
			ERROR3_PF(("Unknown fit type (%d)",FitType));
			IgnoreCall = FALSE;
			return FALSE;
	}

	IgnoreCall = FALSE;

	return TRUE;
}

/******************************************************************************************

>	INT32 PrintControl::GetCropAdjust() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Inputs:		-
	Returns:	An INT32, the width of the crop area
	Purpose:	Return the size of the extra area we need to take into account when fitting
				pages onto the printable paper. Currently the bleed size and crop area size
				is used.
	
******************************************************************************************/

INT32 PrintControl::GetCropAdjust() const
{
	INT32 CropAdjust = 0;
	
	PrintMarksMan *pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan && Typesetting.OutputPrintersMarks())
		CropAdjust = CropArea+Bleed;

	return CropAdjust;
}


/******************************************************************************************

>	void PrintControl::AdjustForTypesetting()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/96
	Inputs:		-
	Returns:	-
	Purpose:	Adjust the printing variables to allow for typesetting marks to be positioned
				What happens here is that we adjust the X,Y translations so that we ensure
				a patch is moved into a position where we can get the crop marks between the
				patch and the papers edges.

******************************************************************************************/

/*
void PrintControl::AdjustForTypesetting()
{
	// Used to do it like this but don't anymore.

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
#ifndef STANDALONE
	PrintMarksMan *pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan && GetTypesetInfo()->OutputPrintersMarks())
	{
		switch (FitType)
		{
			case PRINTFIT_MULTIPLE:
			{
				CropTopMargin = CropArea;
				CropLeftMargin = CropArea;
				// if the bleed is more than 1/2 of the gutter, then the
				// bleed will overflow the edge of the page by the difference!
				double sectw = CalcPatchWidth( PaperWidth, Columns,0);
				double secth = CalcPatchHeight(PaperHeight,Rows   ,0);
				// Translate to the bottom left of the current patch
				double ovflx = Bleed - ((sectw - (double)ScaledWidth)/2);
				double ovfly = Bleed - ((secth - (double)ScaledHeight)/2);

				CropLeftMargin += (INT32)(ovflx+0.5);
				CropTopMargin  += (INT32)(ovfly+0.5);
			}
			break;

			case PRINTFIT_BEST:
			{
				// does nothing at the moment.
			}
			break;

			case PRINTFIT_CUSTOM:
			{
				CropLeftMargin = CropArea+Bleed;
				 CropTopMargin = CropArea+Bleed;
			}
			break;

			default:
				ERROR3_PF(("Unknown fit type (%d)",FitType));
				break;
		}
	}
#endif
#endif
}
*/


/******************************************************************************************

>	BOOL PrintControl::SetRows(INT32 NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewVal = new number of coloumns of printed pages
	Returns:	TRUE if new row num == NewVal, FALSE if it's had to be altered because NewVal is out of range
	Purpose:	Sets the number of pages in each printed column, altering dependants automatically (unless importing)
				Dependants are:
					Scale
					ScaledWidth
					ScaledHeight
					Columns

				If not updating dependants, this will always return TRUE

	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

BOOL PrintControl::SetRows(INT32 NewVal)
{
	Rows = NewVal;

	if (UpdateDependants)
	{
		INT32 Max = CalcMaxRows(0);

		if (Rows < 1)   Rows = 1;
		if (Rows > Max) Rows = Max;

		INT32 MaxGutter = CalcMaxGutter(Rows,Columns);
		if (Gutter > MaxGutter)
			Gutter = MaxGutter;

		CalcFit();
	}

	return (Rows == NewVal);
}

/******************************************************************************************

>	BOOL PrintControl::SetColumns(INT32 NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewVal = new number of coloumns of printed pages
	Returns:	TRUE if new row num == NewVal, FALSE if it's had to be altered because NewVal is out of range
	Purpose:	Sets the number of pages in each printed column, altering dependants automatically (unless importing)
				Dependants are:
					Scale
					ScaledWidth
					ScaledHeight
					Rows

				If not updating dependants, this will always return TRUE

	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

BOOL PrintControl::SetColumns(INT32 NewVal)
{
	Columns = NewVal;

	if (UpdateDependants)
	{
		INT32 Max = CalcMaxColumns(0);

		if (Columns < 1)   Columns = 1;
		if (Columns > Max) Columns = Max;

		INT32 MaxGutter = CalcMaxGutter(Rows,Columns);
		if (Gutter > MaxGutter)
			Gutter = MaxGutter;

		CalcFit();
	}

	return (Columns == NewVal);
}

/******************************************************************************************

>	BOOL PrintControl::SetGutter(INT32 NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Inputs:		NewVal = new gutter measurement between each page
	Returns:	TRUE if new row num == NewVal, FALSE if it's had to be altered because NewVal is out of range
	Purpose:	Sets the gutter measurement, altering dependants automatically (unless importing)

				The gutter is the distance between each page when doing multiple fit printing.
				Half the gutter width is used as a margin for each page next to the edge of the paper

				Dependants are:
					Scale
					ScaledWidth
					ScaledHeight
					Rows
					Colunmns

				If not updating dependants, this will always return TRUE

	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

BOOL PrintControl::SetGutter(INT32 NewVal)
{
	Gutter = NewVal;

	if (UpdateDependants)
	{
		INT32 Max = CalcMaxGutter(1,1);

		if (Gutter < 0)		Gutter = 0;
		if (Gutter > Max)	Gutter = Max;

		INT32 MaxRows = CalcMaxRows(Gutter);
		if (Rows > MaxRows)
			Rows = MaxRows;

		INT32 MaxColumns = CalcMaxColumns(Gutter);
		if (Columns > MaxColumns)
			Columns = MaxColumns;

		CalcFit();
	}

	return (Gutter == NewVal);
}

/******************************************************************************************

>	void PrintControl::SetWholeSpread(BOOL State)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		State = new WholeSpread flag state
	Purpose:	Sets the WholeSpread state, altering dependants automatically (unless importing)

				Dependants are:
					Scale
					ScaledWidth
					ScaledHeight
					PageWidth
					PageHeight
					Rows
					Columns
	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::SetWholeSpread(BOOL State)
{
	if (WholeSpread != State)
	{
		WholeSpread = State;

		if (UpdateDependants)
		{
			CalcPrintAreaVars();	// Update the page width & height (the page width either doubles or halves)
			SetRows(Rows);			// Use SetRows() to recalc max rows and scale factor
			SetColumns(Columns);	// Use SetColumns() to recalc max columns and scale factor
		}
	}
}

/******************************************************************************************

>	void PrintControl::SetFitType(PrintFitType NewFitType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/95
	Inputs:		NewFitType = page fit type
	Purpose:	Sets the fit type, altering dependants automatically (unless importing)

				Dependants are:
					ScaledWidth
					ScaledHeight
					Scale
	SeeAlso:	StartImport() & EndImport()

******************************************************************************************/

void PrintControl::SetFitType(PrintFitType NewFitType)
{
	FitType = NewFitType;

	if (UpdateDependants)
		CalcFit();
}

/******************************************************************************************

>	INT32 PrintControl::GetDotsPerInch()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		-
	Returns:	DPI value used to render bitmaps
	Purpose:	If the bitmap res setting is manual then the value the user entered is returned.
				If the bitmap res setting is automatic, then base the DPI on the printer's DPI

					Bitmap res = Half printer res

					if (Bitmap res > MAXAUTOBITMAPDPI)
						Bitmap res = MAXAUTOBITMAPDPI;

					if (Bitmap res < MINBITMAPDPI)
						Bitmap res = MINBITMAPDPI;

	SeeAlso:	SetDotsPerInch();

******************************************************************************************/

INT32 PrintControl::GetDotsPerInch()
{
	// MRH 11/9/00 - Tidied up code to make it more readable
	// Basically the code checks to see if the auto flag is set.
	// If NOT set then the DotsPerInch variable is left with the value in the manual edit box.
	// If it is set then it gets the current printer resolution and divides it by 2!
	if (BitmapResolutionMethod == BITMAPRES_AUTO)
	{
		// Choose the DPI automatically
		// This is based on the printer resolution
		// Set up the local variable to the max auto dpi!
		INT32 PrinterDPI = MAXAUTOBITMAPDPI;

		// Get the printer resolution (if this fails, choose the maximum setting)
		if (!CCPrintDialog::GetResolution(&PrinterDPI))
			PrinterDPI = MAXAUTOBITMAPDPI; // Failing could change the variable so reset it!

		// Make the DPI half the printer res
		DotsPerInch = PrinterDPI >> 1;

		// If this DPI value > MAXAUTOBITMAPDPI, then probably printing to a typesetter, in which case cap it as 
		// there's no point in printing at any higher DPI
		if (DotsPerInch > MAXAUTOBITMAPDPI)
			DotsPerInch = MAXAUTOBITMAPDPI;

		// Make sure that it doesn't dip below the min.
		// If it does, choose the printer DPI, so that we ensure good quality for low DPI settings
		if (DotsPerInch < MINBITMAPDPI)
			DotsPerInch = PrinterDPI;
	}

	// return the new value!
	return DotsPerInch;
}


/******************************************************************************************

>	void PrintControl::SetDotsPerInch(INT32 NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		NewVal = DPI value
	Returns:	-
	Purpose:	Ensures the NewVal is not < MINBITMAPDPI

				If NewVal < MINBITMAP, the DPI is set to the printer's DPI

	SeeAlso:	GetDotsPerInch()

******************************************************************************************/

void PrintControl::SetDotsPerInch(INT32 NewVal)
{ 
	DotsPerInch = NewVal;

	if (DotsPerInch > MAXBITMAPDPI)
		DotsPerInch = MAXBITMAPDPI;

	if (DotsPerInch < MINBITMAPDPI)
		DotsPerInch = MINBITMAPDPI;

/*
	if (DotsPerInch < MINBITMAPDPI)
	{
		INT32 PrinterDPI;

		// Get the printer resolution (if this fails, choose the minimum setting)
		if (!CCPrintDialog::GetResolution(&PrinterDPI))
			PrinterDPI = MINBITMAPDPI;

		DotsPerInch = PrinterDPI;
	}
*/
}

/******************************************************************************************

>	PrintMethodType PrintControl::GetPrintMethod(PrintMethodType NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/95
	Inputs:		-
	Returns:	The app's current print method
	Purpose:	Returns the app's print method.
				This is stored in the global var AppPrintMethod

	SeeAlso:	SetPrintMethod()

******************************************************************************************/

PrintMethodType PrintControl::GetPrintMethod()
{
	return AppPrintMethod;
}

/******************************************************************************************

>	void PrintControl::SetPrintMethod(PrintMethodType NewVal)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/95
	Inputs:		NewVal = new print method
	Returns:	-
	Purpose:	Sets the app's print method to be NewVal
				This is stored in the global var AppPrintMethod

	SeeAlso:	GetPrintMethod()

******************************************************************************************/

void PrintControl::SetPrintMethod(PrintMethodType NewVal)
{
	AppPrintMethod = NewVal;
}

/******************************************************************************************

>	void PrintControl::GetObjPrintRange()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/96
	Inputs:		-
	Returns:	PRINTRANGEOBJ_ALL or PRINTRANGEOBJ_SELECTED
	Purpose:	Find out the object range the user wants to print to.

				This function has been modified (today 22/7/96) so that it reacts to the current selection.
				This is to fix bug 4857, which meant if you selected "print selected objects" then
				deselected all the objects, it would still print the selected objects, resulting in a 
				blank page.

				What's worse, the UI in the print dlg would grey out, so you couldn't switch it off.

				This routine will mean that "print all" is on if the internal var is PRINTRANGEOBJ_ALL
				OR there are no selected objects.

	SeeAlso:	GetPrintMethod()

******************************************************************************************/

PrintRangeObj PrintControl::GetObjPrintRange()
{
	// Find the selection
	SelRange* pSelRange = GetApplication()->FindSelection();

	if (pSelRange->FindFirst() != NULL)
		return ObjPrintRange;		// There are selected objects, so return the setting the user has made.
	else
		return PRINTRANGEOBJ_ALL;	// There's no selected objects, so always print all objects.
}

//--------------------------------------------------------------------------
// Print layout funcs

/******************************************************************************************

>	BOOL PrintControl::CalcNumPaper()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		-
	Outputs:	NumPaper = The number of pieces of paper we will print to
	Returns:	TRUE if it was able to cal num paper to print to, FALSE otherwise

	Purpose:	This works out how many pieces of paper we will print to.
				NB: This can be different to the number of pieces of paper that comes out of the printer
				when the printer can do multiple copies and we are in a position to exploit this

				It takes into account DPS, whole spread & left or right-only settings.
				It also takes into account the collated flag & the ability of the printer
				to do multiple copies.

				The number of pieces of paper that the printer will produce at the end of the print job
					= number of printable pages * num copies

				If the output is colla

	SeeAlso:	GetDotsPerInch()

******************************************************************************************/

BOOL PrintControl::CalcNumPaper()
{
	BOOL CanMultiCopy;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER	
	// Can the printer do multiple copies?
	if (!CCPrintDialog::CanMultiCopy(&CanMultiCopy))
		return FALSE;
#endif //webster
	INT32 NumActualPiecesOfPaper = NumPrintablePages * NumCopies;	// Num pieces of paper that will come out of printer

	// NumPaper will be the number of pieces of paper we will print to
	// Default to the number printable of pages
	NumPaper = NumPrintablePages;

	if (NumPrintablePages == 1)
	{
		// If the printer can't cope with multiple copies, we'll have to do them all
		if (!CanMultiCopy)
			NumPaper = NumActualPiecesOfPaper;
	}
	else
	{
		// If the printer can't cope with multiple copies OR we need to collate the output
		// we'll have to do them all
		if (Collated || !CanMultiCopy)
			NumPaper = NumActualPiecesOfPaper;
	}	

	return TRUE;
}

/********************************************************************************************

>	BOOL PrintControl::StartPrinting()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/4/95
	Inputs:		-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This informs the print info object that you are about to do some printing.

				This is purely to set up the print layout mechanism.  It will work out how many 
				pieces of paper will be printed to, and how many times the page will be printed
				on each piece of of paper (for multiple fit printing).

				The mechanism works like this:

				if (StartPrinting())
				{
					while (GetNextPaper())
					{
						while (GetNextPatch())
						{
							Use access funcs to get hold of data set up by GetNextPath(), e.g. scale, translation, etc
						
							if (SelectedOnly)
								print all selected objects that lie within the ClipRect area in the spread
							else
								print all objects that lie within the ClipRect area in the spread
						}
					}

					EndPrinting()
				}

	SeeAlso:	GetNextPaper(), GetNextPath(), EndPrinting()

********************************************************************************************/

BOOL PrintControl::StartPrinting()
{
	ERROR2IF(Printing,FALSE,"Already printing - have you called EndPrinting()?");
	ERROR2IF(!PrintAreaSetUp,FALSE,"The print control hasn't been set up via SetUp()");
	ERROR2IF(pSpread == NULL,FALSE,"The print control pSpread is NULL");

	// Find out the number of printable pages
	NumPrintablePages = GetNumPrintablePages();				// This will be either 1 or 2

	// Init NumPaper member var with the number of pieces of paper we will print to.
	if (!CalcNumPaper())
		return FALSE;

	// PaperNumber is the number of the current piece of paper being printed to.
	// MaxPaperNumber is the total number of pieces of paper we will print to.
	PaperNumber 	= 0;		// incremented at the start of GetNextPaper()
	MaxPaperNumber 	= NumPaper;
	Reprint			= FALSE;	// Set to TRUE by ReprintPaper() to make GetNextPaper() to return 
								// the same paper again

	NumPatches = 0;
	Printing = TRUE;

	SelRange* pSelRange = GetApplication()->FindSelection();
	SelectionPresent	= (pSelRange->FindFirst() != NULL);

	// Now make sure that the list of printing plates is updated in case any spot colours
	// have been created or deleted recently...
	TypesetInfo* tpInfo = GetTypesetInfo();
	if (tpInfo != NULL && tpInfo->AreSeparating())
	{
		// Call CreatePlateList to ensure that the list of printing plates is set up
		// This will just update the list (if necessary) if it has been previously created,
		// to make sure that screening options and present plates are all hunky-dory.
		tpInfo->CreatePlateList();
	}

	return TRUE;
}




/********************************************************************************************

>	BOOL PrintControl::EndPrinting()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This informs the print info object that you have finished printing.
	SeeAlso:	StartPrinting();

********************************************************************************************/

BOOL PrintControl::EndPrinting()
{
	ERROR2IF(!Printing,FALSE,"Not printing - have you called StartPrinting()?");
	Printing = FALSE;
	return TRUE;
}





/********************************************************************************************

>	BOOL PrintControl::GetNextPaper()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	TRUE if you should print on another piece of paper
				FALSE if the print job should end
	Purpose:	Call this for each piece of paper to be printed to.
				NB: You must call StartPrinting() before calling this func as part of your print loop

				If ReprintPaper() is called after GetNextPaper(), then the next time you call
				GetNextPaper() you will actually be printing the previous piece of paper
				e.g
					while (GetNextPaper())		// Infinite loop
						ReprintPaper();


	SeeAlso:	StartPrinting(),EndPrinting(), ReprintPaper()

********************************************************************************************/

BOOL PrintControl::GetNextPaper()
{
	ERROR2IF(!Printing,FALSE,"Not printing - have you called StartPrinting()?");

	// Do we need to reprint the same piece of paper?
	if (Reprint)
	{
		// Only re-jig the counters if we have called GetNextPaper() at least once
		if (NumPaper < MaxPaperNumber)
		{
			NumPaper++;
			PaperNumber--;
		}
		// Reprinting, so set flag to FALSE
		Reprint = FALSE;
	}

	if (NumPaper > 0)
	{
		NumPaper--;
		PaperNumber++;

		// Calc the number of patches we'll have to print on this next piece of paper
		if (FitType == PRINTFIT_MULTIPLE)
		{
			NumPatches 	= Rows*Columns;

			// PatchRow & PatchColumn form a coordinate specifying which patch to print next
			PatchRow 	= 0;
			PatchColumn = 0;

			// Section width and height define the size of the areas of the page patches will fit into.
			// Effectively these are the patch sizes ignoring the gutter value.
			// Each patch is centred within a section of the page.
			INT32 CropAdjust = GetCropAdjust();
			INT32 PAWidth    = PaperWidth - (CropAdjust<<1);
			INT32 PAHeight   = PaperHeight - (CropAdjust<<1);
			SectionWidth	= INT32(CalcPatchWidth(PAWidth, Columns,0));
			SectionHeight	= INT32(CalcPatchHeight(PAHeight,Rows,   0));
		}
		else
			NumPatches = 1;

		// PatchNumber is the number of the current patch being printed to.
		// MaxPatchNumber is the total number of patches printed on this piece of paper
		PatchNumber    = 0;			// incremented at the start of GetNextPatch()
		MaxPatchNumber = NumPatches;

		// Calc the object clip rect used to define which objects within the total printable area should
		// be printed on this piece of paper
		//
		// Default to the total printable area
		PatchClipRect = TotalPrintArea;

		if (NumPrintablePages > 1)
		{
			// The number of printable pages > 1 (i.e. we have a double page spread, we are printing
			// both left & right pages, and printing individual pages)
			
			INT32 m;
			
			if (Collated)
			{
				// If collated output, then oscillate the clip rect
				// between the left and right page, controlled by the current paper number
				m = (PaperNumber-1) % NumPrintablePages;
			}
			else
			{
				// Otherwise, print 'n' left pages followed by 'n' right pages
				m = (PaperNumber-1) / (MaxPaperNumber / NumPrintablePages);
			}

			PatchClipRect.lox += (OrigPageWidth*m);
			PatchClipRect.hix  = PatchClipRect.lox + OrigPageWidth;
		}
		else
		{
			if (DPS && !WholeSpread && (DPSPrintRange != PRINTRANGEDPS_BOTH))
			{
				// Here we have a DPS, we are printing individual pages, but we are either
				// printing left-only or right-only pages
				//
				// So if right only, add the page width to the lox coord to move the area to the right hand side.
				// In either case, the patch clip rect hix will be lox+OrigPageWidth.

				if (DPSPrintRange == PRINTRANGEDPS_RIGHTPAGES)
					PatchClipRect.lox += OrigPageWidth;

				PatchClipRect.hix = PatchClipRect.lox + OrigPageWidth;
			}
		}					

		// take into account the bleed margin
/*
		PatchClipRect.Inflate(Bleed);
		if (Orient == PRINTORIENTATION_SIDEWAYS)
			PatchClipRect.Translate(-Bleed,Bleed);
		else
			PatchClipRect.Translate(Bleed,Bleed);
*/
		return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL PrintControl::MorePaper()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Returns:	TRUE if there is more paper to come
				FALSE if there's no more paper. 
	Purpose:	Call this for each piece of paper to be printed to.

********************************************************************************************/

BOOL PrintControl::MorePaper()
{
	if (Reprint && NumPaper < MaxPaperNumber)
		return TRUE;

	return (NumPaper > 0);
}


/********************************************************************************************

>	BOOL PrintControl::ReprintPaper()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Returns:	TRUE if all OK, FALSE otherwise
	Purpose:	This allows you to print on the same piece of paper again.

				If you call this func after a call to GetNextPaper(), then the next time you
				call GetNextPaper() you will actually be starting the print on the same piece of
				paper.

	SeeAlso:	GetNextPaper()

********************************************************************************************/

BOOL PrintControl::ReprintPaper()
{
	Reprint = TRUE;
	return TRUE;
}

/********************************************************************************************

>	BOOL PrintControl::GetNextPatch(PrintPatchInfo* pPatchInfo)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		pPatchInfo = ptr to object to store patch info in
	Outputs:	-
	Returns:	TRUE if you should print another page on the paper
				FALSE if the piece of paper printed to is finished.
	Purpose:	Each time this func returns TRUE, print the page using the data set up by this call.

				If this call returns TRUE, the the given PrintPatchInfo has been set up with
				all the data needed to print the given patch.

				NB: You must call GetNextPaper() before calling this func as part of your print loop

	SeeAlso:	StartPrinting(), GetNextPaper(), EndPrinting()

********************************************************************************************/

BOOL PrintControl::GetNextPatch(PrintPatchInfo* pPatchInfo)
{
	ERROR2IF(!Printing,FALSE,"Not printing - have you called StartPrinting()?");
	ERROR2IF(pPatchInfo == NULL,FALSE,"Given NULL pPatchInfo param");

	if (NumPatches > 0)
	{
		NumPatches--;
		PatchNumber++;

		pPatchInfo->pSpread			= pSpread;
		pPatchInfo->Scale 			= Scale;
		pPatchInfo->ClipRect		= PatchClipRect;
		pPatchInfo->Rotate			= (Orient == PRINTORIENTATION_SIDEWAYS);
		pPatchInfo->SelectedOnly	= ((ObjPrintRange == PRINTRANGEOBJ_SELECTED) && SelectionPresent);
		pPatchInfo->VisibleLayers	= (Layers == PRINTLAYERS_VISIBLEFOREGROUND);
		pPatchInfo->XTrans 			= 0;
		pPatchInfo->YTrans 			= 0;
		pPatchInfo->PaperWidth		= PaperWidth;
		pPatchInfo->PaperHeight		= PaperHeight;
		pPatchInfo->ScaledWidth		= ScaledWidth;
		pPatchInfo->ScaledHeight	= ScaledHeight;
		pPatchInfo->Bleed			= Bleed;
		pPatchInfo->CropArea		= CropArea;
		pPatchInfo->PaperNumber		= PaperNumber;
		pPatchInfo->MaxPaperNumber	= MaxPaperNumber;
		pPatchInfo->PatchNumber		= PatchNumber;
		pPatchInfo->MaxPatchNumber	= MaxPatchNumber;
		pPatchInfo->PALeftMargin	= PALeftMargin;
		pPatchInfo->PATopMargin		= PATopMargin;

		// set emulsion up/down for print rendering
		TypesetInfo* tpInfo = GetTypesetInfo();
		pPatchInfo->EmulsionDown = ( (tpInfo != NULL) && tpInfo->PrintEmulsionDown() );

		switch (FitType)
		{
			case PRINTFIT_BEST:
				// Move it to the right by LeftMargin, and down by TopMargin
				pPatchInfo->XTrans = LeftMargin;
				pPatchInfo->YTrans = (PaperHeight-ScaledHeight)-TopMargin;
				break;

			case PRINTFIT_BESTPAPER:
				// Move it to the right by LeftMargin, and down by TopMargin
				pPatchInfo->XTrans = LeftMargin;
				pPatchInfo->YTrans = (PaperHeight-ScaledHeight)-TopMargin;
				pPatchInfo->XTrans += CropLeftMargin;
				pPatchInfo->YTrans -= CropTopMargin;
				break;

			case PRINTFIT_CUSTOM:
			{
				// Move it to the right by LeftMargin, and down by TopMargin
				pPatchInfo->XTrans = LeftMargin;
				pPatchInfo->YTrans = (PaperHeight-ScaledHeight)-TopMargin;
				pPatchInfo->XTrans += CropLeftMargin;
				pPatchInfo->YTrans -= CropTopMargin;
			}
			break;

			case PRINTFIT_MULTIPLE:
			{
				// Translate to the bottom left of the current patch
				INT32 CropAdjust = GetCropAdjust();
				
				pPatchInfo->XTrans = CropAdjust + (SectionWidth  * PatchColumn) + ((SectionWidth  - ScaledWidth)/2);
				pPatchInfo->YTrans = CropAdjust + (SectionHeight * PatchRow)    + ((SectionHeight - ScaledHeight)/2);

				PatchColumn += 1;
				if (PatchColumn >= Columns)
				{
					PatchColumn = 0;
					PatchRow += 1;
				}
			}
			break;

			default:
				ERROR3_PF(("Unknown fit type : %d",FitType));
				break;
		}

		return TRUE;
	}

	return FALSE;
}




/********************************************************************************************

>	BOOL PrintControl::StartPlatePrinting(PrintView *pPrintView, UINT32 *const pErrNo)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pPrintView - The View to which we are rendering
	Outputs:	errno - 
	Returns:	-
	Purpose:	This function makes sure the plate printing loop is initialised for the
				next set of pages on a single sheet of paper to pass through the driver.
				It sets up an appropriate ColourPlate and ColourCOntextCMYK for the
				given PrintView.

********************************************************************************************/

BOOL PrintControl::StartPlatePrinting(PrintView *pPrintView, UINT32 *const pErrNo)
{
	// Clear the ouput error first
	(*pErrNo)=0;

	// If there's no view, drawing will be rather tricky
	if (pPrintView == NULL)
	{
		ERROR3("Sanity check, The view is NULL during StartPlatePrinting!")
		return FALSE;
	}

	// Get hold of the resident colour management system
	ColourContext *pContext;
	XaraCMS* ptheCMS=GetApplication()->GetCMSManager();

	if (ptheCMS!=NULL)
	{
		// First try to set the printer context specified in the ini file read
		// when we started up
		String_256 PrintProfile;
		ptheCMS->GetPrinterProfile(&PrintProfile);
		pContext = new ColourContextCMYK(pPrintView, &PrintProfile);
	
		if (pContext==NULL)
		{
			// We need to error here and inform the user we
			// are out of memory
			ERROR3("Failed to create a CMYK colour context during print initialisation in SetNextPlate()");
			(*pErrNo) = _R(IDS_OUT_OF_MEMORY);
			return FALSE;
		}
		
		if (!(pContext->IsDeviceContext()))
		{
			// Eeek, we failed to build the physical device context
			// which means something rather nasty went on. We must now
			// warn the user. They can either continue with a logical
			// context or abort the proceedings.

			// TO DO: Make this a nice resourced string!
			// Hang about, we should attempt to use a printer.xcms down
			// the root directory of Camelot, lets see if we can find that

			delete pContext;
			pContext=NULL;

			ptheCMS->GetDefaultPrinterProfile(&PrintProfile);
			pContext = new ColourContextCMYK(pPrintView, &PrintProfile);

			// if we've failed again
			if (pContext==NULL)
			{
				ERROR3("Failed to create a CMYK colour context during print initialisation in SetNextPlate()");
				(*pErrNo) = _R(IDS_OUT_OF_MEMORY);
				return FALSE;
			}

			// ok if we've still failed to locate even this we really are stuck and will have to use the context
			// in logical mode, but lets ask the user about this first.
			if (!(pContext->IsDeviceContext()))
			{
				ErrorInfo Info;
				Info.ErrorMsg = _R(IDT_OPTS_LOGICALCONTEXT);
				Info.Button[0] = _R(IDS_CONTINUE);
				Info.Button[1] = _R(IDS_CANCEL);
				if (AskQuestion(&Info) == _R(IDS_CANCEL))
				{
					(*pErrNo) = _R(IDS_PRINT_USERTERM);
					return FALSE;
				}
			}
		}
	}
	else
	{
		// Create a logical colour context. This isn't strictly necessary, as the view will default to
		// using the global default (logical) context anyway, but we might as well be safe
		pContext = new ColourContextCMYK(pPrintView);
		if (pContext==NULL)
		{
			// we've failed to create a new colour context for this document
			// We must abort the printing loop
			ERROR3("Failed to create a CMYK colour context during print initialisation in SetNextPlate()");
			(*pErrNo) = _R(IDS_OUT_OF_MEMORY);
			return FALSE;
		}
	}

	// Set the new context in the view. The view will delete it when it's done with it
	// NOTE that the view will attach its own ColourPlate to this context, so there is no need
	// (indeed, no point whatsoever) for us to try to set that up for it.
	pPrintView->SetColourContext(COLOURMODEL_CMYK, pContext);

	// Set up the plate printing system, we do this for composite and separated output
	Typesetting.InitPlatesForPrinting();

	return TRUE;
}



/********************************************************************************************

>	void PrintControl::EndPlatePrinting(PrintView *pPrintView)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Returns:	-
	Purpose:	This function makes sure the plate printing loop is shut down correctly.
				We need to make sure that the last plate is detached from the colour context
				inside the print view, otherwise when the print view is destroyed the plate
				in the colour context in the print view will be destroyed. As our printer
				dialogue keeps this in its list, we need to make sure it only gets deleted
				once during that lists destruction.
	
********************************************************************************************/

void PrintControl::EndPlatePrinting(PrintView *pPrintView)
{
	// If there's no view, drawing will be rather tricky
	if (pPrintView == NULL)
	{
		ERROR3("Sanity check, The view is NULL during StartPlatePrinting!")
		return;
	}
	
	pPrintView->SetColourContext(COLOURMODEL_CMYK, NULL);
}



/********************************************************************************************

>	BOOL PrintControl::MorePlates()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		-
	Returns:	TRUE if there is more plates to come
				FALSE if there's no more plates. 
	Purpose:	Call this to determin whether there are more plates to print during this
				separation.

********************************************************************************************/

BOOL PrintControl::MorePlates()
{
	return (Typesetting.NumPrintPlatesRemaining() > 0);
}


/********************************************************************************************

>	void PrintControl::SetNextPlate(PrintView *pPrintView)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pPrintView = a pointer to the print view class.
	Returns:	-
	Purpose:	Call this to set up the view related data ready for the next plate rendering
	
********************************************************************************************/

BOOL PrintControl::SetNextPlate(PrintView *pPrintView)
{
	// If there's no view, drawing will be rather tricky
	if (pPrintView == NULL)
	{
		ERROR3("Sanity check, The view is NULL during SetNextPlate!");
		return FALSE;
	}

	// If we are not separating then do nothing but make sure
	// we force MorePlates() to terminate properly the next time
	// around.
	if (!Typesetting.AreSeparating())
	{
		Typesetting.SetNextCompositePlate();
		return TRUE;
	}

	// Find the next printing plate. We need to call this to decrement
	// the plate printing count and other such unsightlyness so that MorePlates
	// executes correctly. 
	ColourPlate *pPrintPlate = Typesetting.GetNextPrintPlate();
	if (pPrintPlate==NULL)
	{
		// There are no more plates left to print. MorePlates() has 
		// failed in its duty and should be shot.
		return FALSE;
	}

	// We now need to set this plate in the print view in order
	// to describe the separation correctly, this should be pretty easy
	// if we can get a hold of the print view of course.
	pPrintView->SetColourPlate(pPrintPlate);

	return TRUE;
}




/********************************************************************************************

>	void PrintControl::RenderRect(RenderRegion* pRRegion,DocRect Rect,BOOL Fill)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/5/95
	Inputs:		pRRegion 	= ptr to a render region
				Rect	 	= rect to redraw
				Fill	 	= TRUE if filled, FALSE if empty
	Outputs:	-
	Returns:	-
	Purpose:	Renders the rect given
	SeeAlso:	RenderPrintBorders()

********************************************************************************************/

void PrintControl::RenderRect(RenderRegion* pRRegion,DocRect Rect,BOOL Fill)
{
	if (Rect.IsValid() && !Rect.IsEmpty())
	{
		if (Fill)
			pRRegion->DrawRect(&Rect);
		else
			pRRegion->DrawPixelRect(&Rect);
	}
}

/********************************************************************************************

>	void PrintControl::RenderPrintBorder(RenderRegion* pRRegion)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		pRRegion = ptr to a render region
	Outputs:	-
	Returns:	-
	Purpose:	Renders the printable area on the spread according to the current print layout method.
	SeeAlso:	StartPrinting(), GetNextPaper(), EndPrinting()
	Note:		As we are being given a render region that may be in the background redraw list,
				forcing a redraw of documents is very bad as this **MAY** delete the render region
				we hae been given. This used to happen if setup hadn't been called before when
				routine was entered for the first time.

********************************************************************************************/

void PrintControl::RenderPrintBorder(RenderRegion* pRRegion)
{
	if (SetUpFailed)
		return;

	BOOL DrawBorders = FALSE;

	switch (FitType)
	{
		// Always try and show print borders for these fit types
		case PRINTFIT_BEST:
		case PRINTFIT_BESTPAPER:
		case PRINTFIT_CUSTOM:
			DrawBorders = TRUE;
			break;

		// Only try and show print borders for multiple fit when doing 1 row by 1 column
		case PRINTFIT_MULTIPLE:
			DrawBorders = ((Rows == 1) && (Columns == 1));
			break;

		default: 
			ERROR3_PF(("Unknown fit type (%d)",FitType)); 
			break;
	}

	// If we can draw borders for this fit type, ensure that the relevent info is available to do so
	if (DrawBorders)
	{
		// Neville 29/8/97
		// As we are being passed a render region and so might be in the background redraw
		// loop, we must not force a redraw of the doc if we haven't called set up before.
		// If we do then this render region may be deleted and we wont get told about it.
		// This is bad! e.g. Load document with print borders set and have a font replacement
		// message go off.
		if (!PrintAreaSetUp)
			DrawBorders = SetUp(Document::GetSelectedSpread(), FALSE);
		else
			DrawBorders = SetUp(pSpread, FALSE);
	}

	// If the fit type is right, and we have all the info, draw the borders
	if (DrawBorders)
	{
		double ScaleFactor 	= 100.0 / Scale.MakeDouble();

		INT32 ScaledPaperWidth	= INT32(double(PaperWidth) *ScaleFactor);
		INT32 ScaledPaperHeight	= INT32(double(PaperHeight)*ScaleFactor);
		INT32 ScaledPAWidth  	= INT32(double(PrintableArea.Width()) *ScaleFactor);
		INT32 ScaledPAHeight 	= INT32(double(PrintableArea.Height())*ScaleFactor);
		INT32 ScaledPALeftMargin = INT32(double(PALeftMargin)*ScaleFactor);
		INT32 ScaledPATopMargin 	= INT32(double(PATopMargin) *ScaleFactor);
		INT32 ScaledLeftMargin 	= INT32(double(LeftMargin)  *ScaleFactor)+CropLeftMargin;
		INT32 ScaledTopMargin 	= INT32(double(TopMargin)   *ScaleFactor)+CropTopMargin;

		INT32 StartPageNum = 0; 					// We're starting from page 0 (the left page) by default
		INT32 NumPages = GetNumPrintablePages();	// Find out how many we need to print

		// If we're only printing the right page, then start from page 1 (the right page)
		// and increment the number of pages by 1 (i.e. the loop terminating value)
		// This will make sure that the print borders get drawn over the right page
		if ((NumPages == 1) && (DPSPrintRange == PRINTRANGEDPS_RIGHTPAGES))
		{
			StartPageNum = 1;
			NumPages++;
		}

		for (INT32 PageNum = StartPageNum;PageNum < NumPages;PageNum++)
		{
			StockColour PaperCol  = COLOUR_YELLOW;
			StockColour PrAreaCol = COLOUR_RED;

			if (PageNum > 0)
			{
				PaperCol  = COLOUR_GREEN;
				PrAreaCol = COLOUR_BLUE;
			}

			// PagePrintArea defines the portion of the spread that is printable (this
			// is either the whole spread, or a single page when printing DPS & individual pages)
			DocRect PagePrintArea = TotalPrintArea;
			PagePrintArea.lox += OrigPageWidth*PageNum;
			PagePrintArea.hix = PagePrintArea.lox + OrigPageWidth;
			PagePrintArea.hiy = PagePrintArea.loy + OrigPageHeight;

			// PrbleArea will define the printable area in relation to the current spread
			DocRect PrbleArea = PagePrintArea;

			// PaperArea represents the total area of the paper in the printer
			DocRect PaperArea;

			// If printing sideways, the width and height of the printable area should be swapped
			if (Orient == PRINTORIENTATION_SIDEWAYS)
			{
				PrbleArea.hix = PrbleArea.lox+ScaledPAHeight;
				PrbleArea.hiy = PrbleArea.loy+ScaledPAWidth;
				PrbleArea.Translate(ScaledPATopMargin,ScaledPALeftMargin);
				PrbleArea.Translate(-ScaledTopMargin,-ScaledLeftMargin);

				PaperArea = PrbleArea;
				PaperArea.Translate(-ScaledPATopMargin,-ScaledPALeftMargin);
				PaperArea.hix = PaperArea.lox + ScaledPaperHeight;
				PaperArea.hiy = PaperArea.loy + ScaledPaperWidth;
			}
			else
			{
				PrbleArea.hix = PrbleArea.lox+ScaledPAWidth;
				PrbleArea.loy = PrbleArea.hiy-ScaledPAHeight;
				PrbleArea.Translate(ScaledPALeftMargin,-ScaledPATopMargin);
				PrbleArea.Translate(-ScaledLeftMargin,ScaledTopMargin);

				PaperArea = PrbleArea;
				PaperArea.Translate(-ScaledPALeftMargin,ScaledPATopMargin);
				PaperArea.hix = PaperArea.lox + ScaledPaperWidth;
				PaperArea.loy = PaperArea.hiy - ScaledPaperHeight;
			}

			// We need to draw four grey slabs that show the areas on the page that will
			// not be printed

			pRRegion->SetLineColour(COLOUR_NONE);
			pRRegion->SetFillColour(COLOUR_MIDGREY);

			DocRect Slab = PagePrintArea;
			Slab.hix = PrbleArea.lox;
			RenderRect(pRRegion,Slab,TRUE);

			Slab = PagePrintArea;
			Slab.lox = PrbleArea.hix;
			RenderRect(pRRegion,Slab,TRUE);

			Slab = PagePrintArea;
			Slab.hiy = PrbleArea.loy;
			RenderRect(pRRegion,Slab,TRUE);

			Slab = PagePrintArea;
			Slab.loy = PrbleArea.hiy;
			RenderRect(pRRegion,Slab,TRUE);

			pRRegion->SetFillColour(COLOUR_BLACK);
			RenderRect(pRRegion,PagePrintArea,FALSE);

			pRRegion->SetFillColour(PrAreaCol);
			RenderRect(pRRegion,PrbleArea,FALSE);

			pRRegion->SetFillColour(PaperCol);
			RenderRect(pRRegion,PaperArea,FALSE);
		}
	}
}

/********************************************************************************************

>	BOOL PrintControl::RedrawPrintableArea(DocView* pDocView)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		pDocView - ptr to DocView to redraw
	Outputs:	-
	Returns:	-
	Purpose:	This func can be passed as a param to Document::ForceRedraw() and hence can control
				which views get redraw.
				This func ensures that only views that are showing print borders get redrawn.
	SeeAlso:	StartPrinting(), GetNextPaper(), EndPrinting()

********************************************************************************************/

BOOL PrintControl::RedrawPrintableArea(DocView* pDocView)
{
	if (pDocView->GetShowPrintBorders())
		return(TRUE);
	
	ColourPlateType PlateType = COLOURPLATE_NONE;
	if (pDocView->GetColourPlate() && !pDocView->GetColourPlate()->IsDisabled())
		PlateType = pDocView->GetColourPlate()->GetType();
		
	return(PlateType != COLOURPLATE_NONE && PlateType != COLOURPLATE_COMPOSITE);
}


//------------------------------------------------------------
//------------------------------------------------------------
//------------------------------------------------------------

/******************************************************************************************

>	PrintPatchInfo::PrintPatchInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

PrintPatchInfo::PrintPatchInfo()
{
	Scale 			= FIXED16(100);
	XTrans 			= 0;
	YTrans 			= 0;
	ClipRect 		= DocRect(0,0,0,0);
	Rotate 			= FALSE;
	SelectedOnly	= FALSE;
	EmulsionDown	= FALSE;
	VisibleLayers	= TRUE;
	pSpread			= NULL;
	PaperWidth 		= 0;
	PaperHeight		= 0;


	PaperNumber		= 0;
	MaxPaperNumber	= 0;
	PatchNumber		= 0;
	MaxPatchNumber	= 0;
};

/******************************************************************************************

>	PrintPatchInfo::~PrintPatchInfo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default deconstructor
	Errors:		-
	SeeAlso:	PrintPatchInfo()::PrintPatchInfo()

******************************************************************************************/

PrintPatchInfo::~PrintPatchInfo()
{
	// Does nowt as yet
}


/******************************************************************************************

>	DocRect PrintPatchInfo::GetClipRect(BOOL AddBleed, BOOL AddCropArea)

	Author	:	Mike
	Created	:	11/6/95 
	Inputs	:	AddBleed	= Inflate the output clip rectangle by the bleed value
				AddCropArea	= Inflate the output clip rectangle by the crop area
	Returns	:	A clip rectangle.
	Purpose	:	Return the value of the clip rectangle held in this structure. The clip
				rectangle is defined to fit snuggly around a page. No other inflations
				should or do occur around this clip rectangle. However the printing mechanism
				needs to know about bleed sizes and cropmark areas in order to render
				and clip objects correctly so it uses inflated rectangles on occasions.
				It should not and cannot inflate rectangles itself however because this
				would assume knowledge of patch positioning which only the printcontrol
				class knows anything about.

******************************************************************************************/

DocRect PrintPatchInfo::GetClipRect(BOOL AddBleed, BOOL AddCropArea) const
{
	DocRect Temp = ClipRect;
	InflateRectBy(&Temp, AddBleed, AddCropArea);
	return Temp;
}

void PrintPatchInfo::InflateRectBy(DocRect *pRect, BOOL AddBleed, BOOL AddCropArea) const
{
	if (AddBleed)
		pRect->Inflate(Bleed);
	if (AddCropArea)
		pRect->Inflate(CropArea);
}

void PrintPatchInfo::InflateRectBy(OilRect *pRect, BOOL AddBleed, BOOL AddCropArea) const
{
	if (AddBleed)
		pRect->Inflate(Bleed);
	if (AddCropArea)
		pRect->Inflate(CropArea);
}

void PrintPatchInfo::DeflateRectBy(DocRect *pRect, BOOL AddBleed, BOOL AddCropArea) const
{
	if (AddBleed)
		pRect->Inflate(-Bleed);
	if (AddCropArea)
		pRect->Inflate(-CropArea);
}

MILLIPOINT PrintPatchInfo::GetBleed() const
{
	return Bleed;
}

MILLIPOINT PrintPatchInfo::GetCropArea() const
{
	return CropArea;
}



/******************************************************************************************

>	TypesetInfo::TypesetInfo()

	Author	: Mike
	Created	: 11/6/95 
	Purpose	: The default constructor for the typesetting class. We use this class to
			  define the typesetting requirements of the next print run.

******************************************************************************************/

TypesetInfo::TypesetInfo()
{
	MakeSeparations		= FALSE;

	PrintResolution		= 300;

	ScreenFrequency		= 60.0;
	DefaultScreenType	= SCRTYPE_SPOT1;

	UseScreening		= FALSE;
	ShowPrintersMarks	= FALSE;

	EmulsionDown		= FALSE;
	PhotoNegative		= FALSE;
	OverprintBlack		= FALSE;

	NumPrintPlates		= 0;
	CurrPrintPlateNum	= 0;
	CurrentPlate		= NULL;
	CurrentPrintPlate	= NULL;
}



/******************************************************************************************

>	TypesetInfo::~TypesetInfo()

	Author	: Mike
	Created	: 11/6/95 
	Purpose	: The destructor. We must remove all items from our PrintingPlates list
			  and delete them, otherwise the base list class will complain profusely.

******************************************************************************************/

TypesetInfo::~TypesetInfo()
{
	DestroyPlateList();
}


/******************************************************************************************

>	static TypesetInfo *TypesetInfo::FindTypesetInfoForDoc(Document *pDoc = NULL);

	Author	: Jason
	Created	: 15/8/96
	Inputs  : pDoc = NULL (to look in selected doc) or the document to find the info for
	Returns : NULL, or the selected doc's TypesetInfo
	Purpose	: Conveniently find the typeset info for the given document

******************************************************************************************/

TypesetInfo *TypesetInfo::FindTypesetInfoForDoc(Document *pDoc)
{
	if (pDoc == NULL)
		pDoc = Document::GetSelected();

	ERROR2IF(pDoc == NULL, NULL, "No document to find typeset info for!");

	// Find the doc's print component
	PrintComponent *pPrComp = (PrintComponent*)pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
	ERROR2IF(pPrComp == NULL, NULL, "Can't find a document's print component");

	// Find a ptr to the print component's print control object
	PrintControl* pPrCtrl = pPrComp->GetPrintControl();
	ERROR2IF(pPrCtrl == NULL, NULL, "The doc's print component gave me a NULL print control object");

	TypesetInfo *pTInfo = pPrCtrl->GetTypesetInfo();
	ERROR2IF(pTInfo == NULL, NULL, "The doc's print control object gave me a NULL TypesetInfo object");

	return(pTInfo);
}



/******************************************************************************************

>	TypesetInfo& TypesetInfo::operator=(TypesetInfo& other)

	Author	: Jason
	Created	: 17/9/96

	Inputs  : other - the TypesetInfo object to copy
	Returns : the copy (this)

	Purpose	: TypesetInfo assignment operator

******************************************************************************************/

TypesetInfo& TypesetInfo::operator=(TypesetInfo& other)
{
	MakeSeparations		= other.MakeSeparations;
	PrintResolution		= other.PrintResolution;
	ScreenFrequency		= other.ScreenFrequency;
	DefaultScreenType	= other.DefaultScreenType;

	UseScreening		= other.UseScreening;

	ShowPrintersMarks	= other.ShowPrintersMarks;

	EmulsionDown		= other.EmulsionDown;
	PhotoNegative		= other.PhotoNegative;
	OverprintBlack		= other.OverprintBlack;
	PrintSpotAsProcess	= other.PrintSpotAsProcess;

	// Now copy the plate list
	// First, vape all pointers to default values
	CurrPrintPlateNum	= 0;
	CurrentPlate		= NULL;
	CurrentPrintPlate	= NULL;

	// Now destroy any plates we may have already
	DestroyPlateList();

	ColourPlate *pPlate = other.GetFirstPlate();
	while (pPlate != NULL)
	{
		ColourPlate *pNewPlate = new ColourPlate(*pPlate);

		if (pNewPlate == NULL)
			break;

		AddPlate(pNewPlate);
		pPlate = other.GetNextPlate(pPlate);
	}

	return(*this);
}



/******************************************************************************************

>	BOOL TypesetInfo::GetScreenName(ScreenType type, StringBase *pString)

	Author	: Mike
	Created	: 31/08/96
	Inputs  : type = the type of screen to find the name for
	Returns : TRUE if the type is known
			  FALSE if the type is unknown - pString will contain the default SPOT name
	Purpose	: Conveniently convert from a screen type to a textual name

******************************************************************************************/

BOOL TypesetInfo::GetScreenName(ScreenType type, StringBase *pString)
{
	ERROR2IF(pString==NULL, FALSE, "NULL output string pointer passed to GetScreenName");

	String_256 Str;
	BOOL known=TRUE;	

	switch (type)
	{
		case SCRTYPE_SPOT2:
			Str.Load(_R(IDS_SCRTYPE_SPOT2));
			break;
		case SCRTYPE_TRIPLESPOT1:
			Str.Load(_R(IDS_SCRTYPE_TRIPLESPOT1));
			break;
		case SCRTYPE_TRIPLESPOT2:
			Str.Load(_R(IDS_SCRTYPE_TRIPLESPOT2));
			break;
		case SCRTYPE_ELLIPTICAL:
			Str.Load(_R(IDS_SCRTYPE_ELLIPTICAL));
			break;
		case SCRTYPE_LINE:
			Str.Load(_R(IDS_SCRTYPE_LINE));
			break;
		case SCRTYPE_CROSSHATCH:
			Str.Load(_R(IDS_SCRTYPE_CROSSHATCH));
			break;
		case SCRTYPE_MEZZOTINT:
			Str.Load(_R(IDS_SCRTYPE_MEZZOTINT));
			break;
		case SCRTYPE_SQUARE:
			Str.Load(_R(IDS_SCRTYPE_SQUARE));
			break;
		case SCRTYPE_DITHER:
			Str.Load(_R(IDS_SCRTYPE_DITHER));
			break;
		case SCRTYPE_SPOT1:
			Str.Load(_R(IDS_SCRTYPE_SPOT1));
			break;
		default:
			known=FALSE;
			Str.Load(_R(IDS_SCRTYPE_SPOT1));
			break;
	}	

	(*pString) = Str;

	return known;
}


/******************************************************************************************

	void TypesetInfo::SetSeparations(BOOL sep)

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: sep = a boolean which enables or disables separation output
	Purpose	: To turn off and on printing of separated output.

				if enabled, we colour-separate all output

				if disabled, we print normal composite output

******************************************************************************************/

void TypesetInfo::SetSeparations(BOOL sep)
{
	MakeSeparations = sep;
}



/******************************************************************************************

	void TypesetInfo::SetScreening(BOOL state, BOOL SetAllPlates = TRUE)

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: sep = a boolean which enables or disables the output of setscreen commands
			  SetAllPlates = FALSE to simply record the new state in this info class (not
							 recommended)
							 TRUE to set the state in all the plates held in this class

	Purpose	: To turn off and on printing of screened output.

			  If screening is enabled, the colour plate screen ang, freq, function
			  will be output to the printer

			  If disabled, no screening info willl be sent, and thus we use the
			  printer default settings.

******************************************************************************************/

void TypesetInfo::SetScreening(BOOL state, BOOL SetAllPlates)
{
	UseScreening = state;
	if (SetAllPlates)
	{
		ColourPlate *pPlate = GetFirstPlate();
		while (pPlate != NULL)
		{
			pPlate->SetActiveScreening(state);	
			pPlate = GetNextPlate(pPlate);
		}
	}
}



/******************************************************************************************

	void TypesetInfo::SetPrintResolution(INT32 res, BOOL SetAllPlates = TRUE);

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: res = a dpi resolution
			  SetAllPlates = FALSE to simply record the new printer resolution,
							 TRUE to reset all contained colour plates to recommended
							 default values for the new resolution. This will set 
							 angle, freq, and function for all plates.

	Purpose	: Use this function to set the prefered postscript device resolution. The
			  resolution determines the set of preferred screen frequencies.

******************************************************************************************/

void TypesetInfo::SetPrintResolution(INT32 res, BOOL SetAllPlates)
{
	PrintResolution = res;

	if (SetAllPlates)
		ResetAllPlatesToDefaultScreens();
}



/******************************************************************************************

	void TypesetInfo::SetDefaultScreenFrequency(double freq, BOOL SetAllPlates = TRUE)

	Author	:	Mike
	Created	:	11/6/95 
	Inputs	:	freq = a double representing the screen frequency in lpi (possibly fractional)
				SetAllPlates = FALSE to simply record the new printer resolution,
							 TRUE to reset all contained colour plates to this new
							 screening frequency.

	Purpose	:	Use this function to set the preferred screening frequency used for plates

******************************************************************************************/

void TypesetInfo::SetDefaultScreenFrequency(double freq, BOOL SetAllPlates)
{
	if (freq < 2.0 || freq > 10000.0)
	{
		ERROR3("Silly screen frequency");
		return;
	}

	ScreenFrequency = freq;

	if (SetAllPlates)
		ResetAllPlatesToDefaultScreens();
}



/******************************************************************************************

>	void TypesetInfo::SetScreenFunction(ScreenType func, BOOL SetAllPlates = TRUE);

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: func   = the screening function to set
			  setall = TRUE, then set this screen type in all the plates in the plate list
					   FALSE - only remember this as the default (not recommended!)

	Purpose	: Use this function to set the preferred screening function used for plates
	
******************************************************************************************/

void TypesetInfo::SetScreenFunction(ScreenType func, BOOL SetAllPlates)
{
	DefaultScreenType = func;

	if (SetAllPlates)
	{
		ColourPlate *pPlate = GetFirstPlate();
		while (pPlate != NULL)
		{
			pPlate->SetScreenFunction(func);	
			pPlate = GetNextPlate(pPlate);
		}
	}
}



/******************************************************************************************

>	void TypesetInfo::ResetAllPlatesToDefaultScreens(void)

	Author	: Jason
	Created	: 19/8/96

	Purpose	: Resets all attached ColourPlates to the recommended defaults for:
				Screen Angle
				Screen Frequency
				Screen Function (dot shape)

	SeeAlso:	SetPrintResolution; SetDefaultScreenFrequency; SetScreenFunction

******************************************************************************************/

void TypesetInfo::ResetAllPlatesToDefaultScreens(void)
{
	INT32 Res  = GetPrintResolution();
	INT32 Freq = (INT32) floor(GetDefaultScreenFrequency());

	ColourPlate *pPlate = GetFirstPlate();
	while (pPlate != NULL)
	{
		pPlate->ResetScreenToDefaults(Res, Freq);
		pPlate->SetScreenFunction(GetScreenFunction());

		pPlate = GetNextPlate(pPlate);
	}
}



/******************************************************************************************

>	void TypesetInfo::EnsureAllPlatesHaveGlobalSettigns(void)

	Author	: Jason
	Created	: 18/9/96

	Purpose	:	For convenience and efficiency while rendering print jobs, ColourPlates
				have their own copies of certain "global" flags (such as whether
				screening is enabled).

				As a consequence of this slightly untidy situation, it is possible for
				the colour plates to get out of step with the overall option stored
				in the TypesetInfo. To get around this, this function simply copies the
				overall option from the TypesetInfo into all its colour plates.

				It should be called prior to printing etc to ensure that these 'global'
				settings are consistent across all plates and the parent TypesettingInfo

	Notes:		Currently copies the following states:
					* Screen function
					* Screening enabled flag

******************************************************************************************/

void TypesetInfo::EnsureAllPlatesHaveGlobalSettings(void)
{
	ColourPlate *pPlate = GetFirstPlate();
	while (pPlate != NULL)
	{
		pPlate->SetScreenFunction(GetScreenFunction());
		pPlate->SetActiveScreening(AreScreening());

		pPlate = GetNextPlate(pPlate);
	}	
}



/******************************************************************************************

>	void TypesetInfo::SetPhotoNegative(BOOL state)

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: state  = the state of the photonegative flag
	Purpose	: Use this function to set the photonegative local flag.
	
******************************************************************************************/

void TypesetInfo::SetPhotoNegative(BOOL state)
{
	PhotoNegative = state;
}


/******************************************************************************************

>	void TypesetInfo::SetOverprintBlack(BOOL Always)

	Author	:	Jason
	Created	:	7/8/96

	Inputs	:	Always - FALSE to disbale, or TRUE to enable "Always overprint black"

	Purpose	:	Sets whether "black" will be always be overprinted automatically.

				This works as follows:
					Any colour which is defined as CMYK and which has more than 95% Key
					component is considered "black". When AlwaysOverprintBlack is enabled,
					such "black" colours will be overprinted automatically.

					This option does not affect non-CMYK colours

	Notes:		The TypesetInfo constructor defaults to the FALSE (disabled) state

	SeeAlso:	RenderRegion::PrepareStrokeColour; RenderRegion::PrepareFillGeometry

******************************************************************************************/

void TypesetInfo::SetOverprintBlack(BOOL Always)
{
	OverprintBlack = Always;
}



/******************************************************************************************

>	BOOL TypesetInfo::CheckForExistingPlate(ColourPlateType Type,
											IndexedColour *SpotColour = NULL)

	Author	: Jason
	Created	: 25/6/96
	Inputs	: Type - The type of plate you are checking for
			  SpotColour - If Type is COLOURPLATE_SPOT, then SpotColour should point
			  to the IndexedColour defining the spot plate. It is ignored for all other
			  types.

	Returns : TRUE if a matching plate is already in the list
			  FALSE if you'll need to create and add a new plate of that type

	Purpose	: Helper function for CreatePlateList. Determines if a plate of the given
			  type is already available in the ColourPlate list. If Type is
			  COLOURTYPE_SPOT, then SpotColour must also be specified.

	Notes:	  This doesn't find an exact match for the ColourPlate, but simply another
			  plate of the same type (e.g. Another CYAN plate, or another SPOT plate
			  which is based on the same spot colour) - all other options (monochrome,
			  screen settings etc) are ignored in the comparison.

******************************************************************************************/

BOOL TypesetInfo::CheckForExistingPlate(ColourPlateType Type, IndexedColour *SpotColour)
{
	ERROR3IF(Type == COLOURPLATE_SPOT && SpotColour == NULL, "Illegal NULL param");

	ColourPlate *Ptr = GetFirstPlate();

	while (Ptr != NULL)
	{
		if (Ptr->GetType() == Type)
		{
			// Just for safety we will make sure that the plate uses the same setting
			// for screen function as we do. (The plate holds a copy of this info for
			// convenience, and we don't really want plates using different screens)
			Ptr->SetScreenFunction(GetScreenFunction());

			// The basic types match, but if it's a spot plate, we have to check if
			// it is the same actual spot plate before we have an exact match
			if (Type == COLOURPLATE_SPOT)
			{
				if (Ptr->GetSpotColour() == SpotColour)
					return(TRUE);
				//else 
				//	Not an exact match, so drop through to continue searching
			}
			else
				return(TRUE);				// The types match, so return TRUE
		}

		Ptr = GetNextPlate(Ptr);
	}

	// We didn't find it, so return FALSE
	return(FALSE);
}



/******************************************************************************************

>	BOOL TypesetInfo::CreatePlateList()

	Author	: Jason (shell function by Mike)
	Created	: 25/6/96 (11/6/95 - 95? Are you sure, Mike? ;-)
	Inputs	: -
	Returns	: TRUE if the plate list has been constructed correctly
			  FALSE if there's nothing in the plate list.

	Purpose	: This function initialises or re-initialises the document plate list.
			  This list contains a set of ColourPlates which will be used to control
			  the printing of the document. Each plate will produce a distinct pass
			  in the print loop.

			  If the ColourPlate list has already been created, this function will
			  update it by the following means:
			  * Ensure that the process (C,M,Y,K) plates are in the list
			  * Remove any Spot colours from the list which have since been deleted or
			    edited to no longer be spot colours
			  * Add any new spot colours to the plate list

			  Any existing plate settings for plates that are still valid will thus
			  be retained.

******************************************************************************************/

BOOL TypesetInfo::CreatePlateList()
{
	ColourPlate *pPlate;

	INT32 Res  = GetPrintResolution();
	INT32 Freq = (INT32) floor(GetDefaultScreenFrequency());

	// --- First, make sure we've got a set of process (CMYK) plates...
	// Create a cyan plate to print.
	if (!CheckForExistingPlate(COLOURPLATE_CYAN))
	{
		pPlate = new ColourPlate(COLOURPLATE_CYAN, TRUE, FALSE);
		if (pPlate)
		{
			pPlate->ResetScreenToDefaults(Res, Freq);
			pPlate->SetScreenFunction(GetScreenFunction());

			AddPlate(pPlate);
		}
	}

	// Create a magenta plate to print.
	if (!CheckForExistingPlate(COLOURPLATE_MAGENTA))
	{
		pPlate = new ColourPlate(COLOURPLATE_MAGENTA, TRUE, FALSE);
		if (pPlate)
		{
			pPlate->ResetScreenToDefaults(Res, Freq);
			pPlate->SetScreenFunction(GetScreenFunction());

			AddPlate(pPlate);
		}
	}

	// Create a yellow plate to print.
	if (!CheckForExistingPlate(COLOURPLATE_YELLOW))
	{
		pPlate = new ColourPlate(COLOURPLATE_YELLOW, TRUE, FALSE);
		if (pPlate)
		{
			pPlate->ResetScreenToDefaults(Res, Freq);
			pPlate->SetScreenFunction(GetScreenFunction());

			AddPlate(pPlate);
		}
	}

	// Create a key plate to print.
	if (!CheckForExistingPlate(COLOURPLATE_KEY))
	{
		pPlate = new ColourPlate(COLOURPLATE_KEY, TRUE, FALSE);
		if (pPlate)
		{
			pPlate->ResetScreenToDefaults(Res, Freq);
			pPlate->SetScreenFunction(GetScreenFunction());

			AddPlate(pPlate);
		}
	}


	// --- Second, remove any "dead" plates (spot colours which are now deleted or
	//     have been edited so they are no longer spot colours)
	pPlate = (ColourPlate *) PrintingPlates.GetHead();
	ColourPlate *pNext;
	while (pPlate != NULL)
	{
		pNext = (ColourPlate *) PrintingPlates.GetNext(pPlate);

		if (pPlate->GetType() == COLOURPLATE_SPOT)
		{
			IndexedColour *pSpot = pPlate->GetSpotColour();
			if (pSpot != NULL)
			{
				// If it's not a spot colour, or is deleted, then it no longer generates a plate
				if (pSpot->GetType() != COLOURTYPE_SPOT || pSpot->IsDeleted())
				{
					PrintingPlates.RemoveItem(pPlate);
					delete pPlate;
				}
			}
		}
		pPlate = pNext;
	}


	// --- Finally, add plates for any spot colour which is not already accounted for
	//     in the existing plate list. We use the colours from the Selected document
	ColourList *ColList = ColourManager::GetColourList();
	ERROR3IF(ColList == NULL, "Can't find selected ColourList");

	if (ColList != NULL)
	{
		IndexedColour *IxCol = 	(IndexedColour *) ColList->GetUndeletedHead();
		while (IxCol != NULL)
		{
			if (IxCol->GetType() == COLOURTYPE_SPOT)
			{
				// We've found a spot colour. Is it already represented by a ColourPlate?
				if (!CheckForExistingPlate(COLOURPLATE_SPOT, IxCol))
				{
					// No, it's not, so we'd better create a new plate for it
					pPlate = new ColourPlate(IxCol, TRUE, FALSE);
					if (pPlate)
					{
						pPlate->ResetScreenToDefaults(Res, Freq);
						pPlate->SetScreenFunction(GetScreenFunction());

						AddPlate(pPlate);

						// If this colour is not used in the document, turn off printing of the plate
						// by default. (This isn't brilliant - it'll leave it on if the colour is used
						// in the UNDO, but it's better than leaving it on all the time)
						// NOTE: We pass in TRUE so that it will ignore one use of the colour (the
						// reference from the ColourPlate itself!)
						if (!IxCol->IsInUse(TRUE))
							pPlate->SetDisabled(TRUE);
					}
				}
			}

			IxCol = (IndexedColour *) ColList->GetUndeletedNext(IxCol);
		}
	}

	// And now, just make sure everything is tidy
	EnsureAllPlatesHaveGlobalSettings();

	return TRUE;
}



/******************************************************************************************

>	void TypesetInfo::UpdatePlateList()

	Author	: Mike
	Created	: 31/08/96
	Inputs	: -
	Returns	: -
	Purpose	: Makes sure all the plates in the plate list have all values up-to-date.
			  This is called on StartPrinting(). The reason it is here is that on
			  start up, the previous settings will be loaded in and set in TypsetInfo.
			  However, the user could print immediately without bringing up any extra
			  print option tabs. The list of plates is created needs to be updated with
			  the loaded settings.

******************************************************************************************/

void TypesetInfo::UpdatePlateList()
{
	// Make sure all plate screening values are up-to-date
	SetScreening(UseScreening,TRUE);
	// Make sure screen plate / lpi / freq / angles are all correct
	ResetAllPlatesToDefaultScreens();
}


/******************************************************************************************

>	ColourPlate *TypesetInfo::CreateColourPlate()

	Author	: Jason
	Created	: 16/8/96
	Inputs	: -
	Returns	: NULL (failed) or a new colour plate

	Purpose	: Creates a new colour plate
				The plate defaults to the same as for the ColourPlate() default constructor
				(i.e. COLOURPLATE_NONE), but things like the screen function will be set
				up by copying them from this TypesetInfo object.

				This function is intended only for use by the native file import code,
				which creates colour plates if any were saved in the file. However, each
				plate takes many settings from its parent TypesetInfo (they're only stored
				in the plate for easy access/efficiency when rendering)

	Notes:	See Also CheckForExistingPlate() which also sets the plate screen functions to
			make sure they tow the party line...

******************************************************************************************/

ColourPlate *TypesetInfo::CreateColourPlate()
{
	ColourPlate *pNewPlate = new ColourPlate(COLOURPLATE_NONE, TRUE, FALSE);

	// The only special info we currently need to set is the screen function...
	if (pNewPlate != NULL)
		pNewPlate->SetScreenFunction(GetScreenFunction());

	return(pNewPlate);
}



/******************************************************************************************

>	void TypesetInfo::DestroyPlateList()

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns	: -
	Purpose	: This function simply empties the entire plate list of its contents, deleting
			  each record as it goes. It is a seriously non-constant function.

******************************************************************************************/

void TypesetInfo::DestroyPlateList()
{
	ColourPlate *pPlate;
	while (pPlate=(ColourPlate*)PrintingPlates.RemoveTail())
		delete pPlate;

	// We have no plates, so it's best to vape our current pointers
	CurrentPlate		= NULL;
	CurrentPrintPlate	= NULL;
}

/******************************************************************************************

>	void TypesetInfo::AddPlate(ColourPlate* pPlate)

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: pPlate = a pointer to a colour plate
	Returns	: -
	Purpose	: Use this function to add a colour plate to the colour plate list.
			  Note, the calling function no longer has ownership of this object. The object
			  will be deleted automatically from the list whenever the plate list class is
			  destroyed.
			  
******************************************************************************************/

void TypesetInfo::AddPlate(ColourPlate* pPlate)
{
	ERROR3IF(pPlate==NULL, "AddPlate given a NULL plate pointer!");
	PrintingPlates.AddTail(pPlate);
}

/******************************************************************************************

>	void TypesetInfo::SetCurrentPlate(ColourPlate* pPlate)

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: pPlate = a pointer to a colour plate
	Returns : -
	Purpose	: Use this function to set the current colour plate. This current plate
			  object is usually shown as the highlighted plate in the printing dialogue
			  plate display list.

******************************************************************************************/

void TypesetInfo::SetCurrentPlate(ColourPlate* pPlate)
{
	ERROR3IF(pPlate==NULL, "SetCurrentPlate given a NULL plate pointer!");
	CurrentPlate = pPlate;
}

/******************************************************************************************

>	ColourPlate* TypesetInfo::GetCurrentPlate() const

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : a pointer to the current colour plate
	Purpose	: Use this function to retrieve a pointer to the current plate.
			  The current plate variable simply keeps track of the selected plate in the
			  printing UI. This plate is the one which will receive all edits.
			  
******************************************************************************************/

ColourPlate* TypesetInfo::GetCurrentPlate() const
{
	return CurrentPlate;
}


/******************************************************************************************

>	DWORD TypesetInfo::GetNumPlates() const

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : a dword, the number of plates in the list
	Purpose	: Returns the number of plates in the print class plate list
			  
******************************************************************************************/

DWORD TypesetInfo::GetNumPlates() const
{
	return PrintingPlates.GetCount();
}




/******************************************************************************************

>	void TypesetInfo::InitPlatesForPrinting()

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : -
	Purpose	: Set up the plate printing service. This should be called before any attempt
			  to access the functions GetNextPrintPlate(), NumPrintPlatesRemaining etc
			  
******************************************************************************************/

void TypesetInfo::InitPlatesForPrinting()
{
	// We don't need to do anything if we're not separating, but as we
	// do nothing but set a plate, we don't need extra check code here
	// Set our internal current plate
	CurrPrintPlateNum=1;
	NumPrintPlates=1;
	CurrentPrintPlate=NULL;
	PrintPlatesToGo=1;

	if (AreSeparating())
	{
		NumPrintPlates = GetNumPrintPlates();
		PrintPlatesToGo = NumPrintPlates;
	}
}



/******************************************************************************************

>	ColourPlate* TypesetInfo::GetFirstPrintPlate()

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : A pointer to the first print plate (NULL if no more)
	Purpose	: Return the first plate through which the document will be printed.
			  The class variables CurrentPrintPlate and CurrPrintPlateNum will be
			  set.
			  
******************************************************************************************/

ColourPlate* TypesetInfo::GetFirstPrintPlate()
{
	// Retrieve the first plate from the list
	ColourPlate *pPlate = GetFirstPlate();
	return GetPrintPlate(0,pPlate);
}

/******************************************************************************************

>	ColourPlate* TypesetInfo::GetNextPrintPlate()

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : A pointer to the next print plate (NULL if no more)
	Purpose	: Return the next plate through which the document will be printed.
			  The class variables CurrentPrintPlate and CurrPrintPlateNum will be
			  used to determin the next plate
			  
******************************************************************************************/

ColourPlate* TypesetInfo::GetNextPrintPlate()
{
	ColourPlate *pPlate;
	if (CurrentPrintPlate==NULL)
	{
		// Retrieve the first plate from the list
		pPlate = GetFirstPlate();
		return GetPrintPlate(0,pPlate);
	}
	// otherwise retrieve the next plate
	pPlate = GetNextPlate(CurrentPrintPlate);
	return GetPrintPlate(CurrPrintPlateNum,pPlate);
}


/******************************************************************************************

	ColourPlate* TypesetInfo::GetPrintPlate(DWORD num, ColourPlate* pPlate)

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: num = the number of the plate
			  pPlate = a pointer to the plate
	Returns : A pointer to a print plate (NULL if none)
	Purpose	: This function is an internal helper function to GetFirstPrintPlate() and
			  GetNextPrintPlate()
			  
******************************************************************************************/

ColourPlate* TypesetInfo::GetPrintPlate(DWORD pnum, ColourPlate* pPlate)
{
	while (pPlate!=NULL)
	{
		pnum++;
		// if this plate is not disabled then return it
		if (!pPlate->IsDisabled())
		{
			CurrPrintPlateNum=pnum;
			PrintPlatesToGo--;
			return (CurrentPrintPlate=pPlate);
		}
		pPlate=GetNextPlate(pPlate);
	}
	return NULL;
}


/******************************************************************************************

>	DWORD TypesetInfo::GetNumPrintPlates() const

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : A DWORD, the number of plates this document can theoretically be separated
			  to. This will usually be 4 (C,M,Y and K) plus any spot colour plates
	Purpose	: Find out how many print plates are stored in the plate list.
			  
******************************************************************************************/

DWORD TypesetInfo::GetNumPrintPlates() const
{
	ColourPlate* pPlate;
	DWORD        nPlates=0;

	pPlate = GetFirstPlate();
	while (pPlate!=NULL)
	{
		if (!pPlate->IsDisabled())
			nPlates++;
		pPlate=GetNextPlate(pPlate);
	}		
	return nPlates;
}


/******************************************************************************************

>	ColourPlate* TypesetInfo::GetCurrentPlate() const

	Author	: Jason
	Created	: 5/8/96
	Inputs	: -
	Returns : a pointer to the currently PRINTING colour plate
	Purpose	: Use this function to retrieve a pointer to the currently printing plate.
			  
******************************************************************************************/

ColourPlate* TypesetInfo::GetCurrentPrintPlate() const
{
	return CurrentPrintPlate;
}


/******************************************************************************************

>	void TypesetInfo::NumPrintPlatesRemaining()

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : The number of plate which remain to be printed
	Purpose	: Calculate and return the number of plates remaining for this piece of paper
			  
******************************************************************************************/

INT32 TypesetInfo::NumPrintPlatesRemaining() const
{
	// Calculate the number of plates remaining print
	return ( PrintPlatesToGo );
}


/******************************************************************************************

>	void TypesetInfo::SetNextCompositePlate()

	Author	: Mike
	Created	: 11/6/95 
	Inputs	: -
	Returns : -
	Purpose	: Called from PrintControl::SetNextPlate(). This function simply allows
			  composite printing to pass around the plate loop once without actually
			  setting any plates. This function will set the internal typeset plate
			  variables so that MorePlates() will return FALSE the next time it is
			  called.
			  
******************************************************************************************/

void TypesetInfo::SetNextCompositePlate()
{
	// Do absolutely nothing if we are in separation mode
	if (AreSeparating())
		return;

	// Set out internal variables so a call to MorePlates()
	// will terminate the print loop
	CurrentPrintPlate=NULL;
	PrintPlatesToGo=0;
}
