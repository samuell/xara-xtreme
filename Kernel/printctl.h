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
// Header for the print control object

#ifndef INC_PRINTCTL
#define INC_PRINTCTL

//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "colplate.h"

class Spread;
class DocView;
class PrintPatchInfo;
class RenderRegion;
class PrintView;

//-------------------------------------------------

enum PrintOrient		{ PRINTORIENTATION_UPRIGHT, PRINTORIENTATION_SIDEWAYS };
enum PrintFitType 		{ PRINTFIT_BEST,PRINTFIT_CUSTOM,PRINTFIT_MULTIPLE,PRINTFIT_BESTPAPER };
enum PrintLayers		{ PRINTLAYERS_ALLFOREGROUND, PRINTLAYERS_VISIBLEFOREGROUND };
enum PSLevel			{ PSLEVEL_AUTO, PSLEVEL_1, PSLEVEL_2 };
enum PrintMethodType	{ PRINTMETHOD_NORMAL, PRINTMETHOD_BITMAP, PRINTMETHOD_AABITMAP };
enum BitmapResMethod 	{ BITMAPRES_AUTO, BITMAPRES_MANUAL };
enum PrintRangeObj		{ PRINTRANGEOBJ_ALL, PRINTRANGEOBJ_SELECTED };
enum PrintRangeDPS		{ PRINTRANGEDPS_BOTH, PRINTRANGEDPS_LEFTPAGES, PRINTRANGEDPS_RIGHTPAGES };
enum PrintFillQuality	{ PRINTFILLQUALITY_HIGH, PRINTFILLQUALITY_MEDIUM, PRINTFILLQUALITY_LOW };
enum PrintTextOptions	{ PRINTTEXTOPTIONS_NORMAL, PRINTTEXTOPTIONS_ALLTEXTASSHAPES };


#define DEF_SCREEN_FREQ  60
#define DEF_SCREEN_ANG	 15
#define DEF_SCREEN_FUNC	 SCRTYPE_SPOT1


/********************************************************************************************

>	class TypesetInfo : public CC_CLASS_MEMDUMP
	
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/95
	Purpose:	A class to encapsulate all the bits of typesetting we provide. The
				main entry in the class is a list of colour plate objects. When printing
				separations, the list is used to (a) determin which separations require
				printing and (b) to set each printable colour plate in the active print
				view. The main print loop will use the access functions in this class to
				cycle through each printable plate. This will result in a set of separated
				representations of a page being produced.
				Other information held in this class pertain to Postscript functionality.
				The class contains the prefered screening values for the entire document.

********************************************************************************************/

class TypesetInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(TypesetInfo)

	public:
		TypesetInfo();
		virtual ~TypesetInfo();

		static TypesetInfo *FindTypesetInfoForDoc(Document *pDoc = NULL);

		TypesetInfo& operator=(TypesetInfo& other);

	public:		// Enablement flags
		void			SetSeparations(BOOL sep);		// Enable separations (else composite print)

		BOOL			AreSeparating()		 const { return MakeSeparations; }
		BOOL			AreScreening()		 const { return UseScreening; }

	public:		// Default/global screen settings
		void			SetScreening(BOOL screen, BOOL SetAllPlates = TRUE);
		void			SetPrintResolution(INT32 res, BOOL SetAllPlates = TRUE);
		void			SetDefaultScreenFrequency(double Freq, BOOL SetAllPlates = TRUE);
		void			SetScreenFunction(ScreenType func, BOOL SetAllPlates = TRUE);

		void			ResetAllPlatesToDefaultScreens(void);
		void			EnsureAllPlatesHaveGlobalSettings(void);

		INT32			GetPrintResolution() const				{ return PrintResolution; }
		double			GetDefaultScreenFrequency() const		{ return ScreenFrequency; }
		ScreenType		GetScreenFunction() const				{ return DefaultScreenType; }
		BOOL			GetScreenName(ScreenType type, StringBase *pName);

	public:		// Control of printers marks - see the PrintMarksMan class for more details
		void			SetOutputPrintersMarks(BOOL ShowThem)	{ ShowPrintersMarks = ShowThem; }
		BOOL			OutputPrintersMarks(void) const			{ return(ShowPrintersMarks); }

	public:		// Global imagesetter/separation flags
		// Emulsion down (reflect plates in X axis)
		void			SetEmulsionDown(BOOL On)				{ EmulsionDown = On; }
		BOOL			PrintEmulsionDown() const				{ return EmulsionDown; }

		// Photographic negative
		void			SetPhotoNegative(BOOL On);
		BOOL			PrintPhotoNegative() const				{ return PhotoNegative; }

		// Always overprint "black" (more than 95% Key CMYK colours)
		void			SetOverprintBlack(BOOL Always);
		BOOL			AlwaysOverprintBlack() const			{ return OverprintBlack; }

	public:
		// The plate list creation and access functions. These functions
		// allow the building and maintenance of a list of colour plates.
		// As the list differs from the list of plates to print in that it
		// contains all document plates rather than just that set actively
		// being printed, you should use these functions when concerned with
		// dialogue management.		
		BOOL			CreatePlateList();
		void			UpdatePlateList();
		ColourPlate*	CreateColourPlate();

		void			AddPlate(ColourPlate* pPlate);
		void			DestroyPlateList();

		ColourPlate*	GetCurrentPlate() const;
		void			SetCurrentPlate(ColourPlate* pPlate);
		DWORD			GetNumPlates() const;
		
		inline ColourPlate*	GetFirstPlate() const;
		inline ColourPlate*	GetNextPlate(const ColourPlate* pPlate) const;
		inline ColourPlate* GetPrevPlate(const ColourPlate* pPlate) const;

		// The printing plate access functions. These functions are specifically
		// used to track the plates marked for printing.		
		void			InitPlatesForPrinting();
		ColourPlate*	GetFirstPrintPlate();
		ColourPlate*	GetNextPrintPlate();
		DWORD			GetNumPrintPlates() const;
		ColourPlate*	GetCurrentPrintPlate() const;
		INT32			NumPrintPlatesRemaining() const;
		void			SetNextCompositePlate();


		// All the private class strutures, hands off! And no making them public or else!
	private:
		BOOL			CheckForExistingPlate(ColourPlateType Type, IndexedColour *SpotColour = NULL);
		ColourPlate*	GetPrintPlate(DWORD num, ColourPlate* pPlate);

	private:
		BOOL			MakeSeparations;			// Determins whether separations are printed
		INT32			PrintResolution;			// The resolution of the printing device.
		double			ScreenFrequency;			// The default screen frequency (LPI)
		ScreenType		DefaultScreenType;			// The default screen type

		BOOL			UseScreening;				// Are we outputing screening functions?

		BOOL			ShowPrintersMarks;			// TRUE if printers marks are enabled in this doc

		BOOL			EmulsionDown;				// TRUE if doing EmDown (reflect-in-X) plates
		BOOL			PhotoNegative;				// TRUE if doing photographic negative plates
		BOOL			OverprintBlack;				// TRUE to always overprint CMYK colours with Key >95%
		BOOL			PrintSpotAsProcess;			// TRUE to convert all spot colours to process

	private:
		List			PrintingPlates;				// The list of plates (enabled/disabled)

		DWORD			NumPrintPlates;				// differs from list number as some are disabled
		DWORD			CurrPrintPlateNum;			// the list number of the plate we're printing
		DWORD			PrintPlatesToGo;			// Number of plates left to print
		ColourPlate*	CurrentPlate;				// Use GetCurrent/SetCurrent plate for this
		ColourPlate*	CurrentPrintPlate;			// A multipass print related pointer
};


/********************************************************************************************

>	inline ColourPlate* TypesetInfo::GetFirstPlate() const
	inline ColourPlate* TypesetInfo::GetNextPlate(const ColourPlate* pPlate) const
	inline ColourPlate* TypesetInfo::GetPrevPlate(const ColourPlate* pPlate) const
	
	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/95
	Purpose:	A couple of fast access functions for retrieving plates from the
				documents plate list.

********************************************************************************************/

inline ColourPlate* TypesetInfo::GetFirstPlate() const
{
	return (ColourPlate*)PrintingPlates.GetHead();
}

inline ColourPlate* TypesetInfo::GetNextPlate(const ColourPlate* pPlate) const
{
	return (ColourPlate*)PrintingPlates.GetNext((ListItem*)pPlate);
}

inline ColourPlate* TypesetInfo::GetPrevPlate(const ColourPlate* pPlate) const
{
	return (ColourPlate*)PrintingPlates.GetPrev((ListItem*)pPlate);
}





/********************************************************************************************

>	class PrintControl : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/95
	Purpose:	This class encapsulates all the print settings for a document, and controls
				how the document pages are printed on the paper

	SeeAlso:	OptionsTabs; AppPrefsDlg; DocPrefsDlg;

********************************************************************************************/

class PrintControl : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PrintControl)

public:
	PrintControl();
	~PrintControl();

	// Set up functions
	BOOL SetUp(Spread* pSpread, BOOL RedrawPrintBorders = TRUE);

	// Info functions
	String_256 BuildPrintInfoStr();
	String_256 BuildPaperSizeStr();

	// Print layout vars
	BOOL 			IsWholeSpread()							{ return WholeSpread; }
	void 			SetWholeSpread(BOOL State);

	BOOL			IsDoublePageSpread()					{ return DPS; }
	void			SetDoublePageSpread(BOOL State)			{ DPS = State; }

	FIXED16 		GetScale()								{ return Scale; }
	void			SetScale(FIXED16 NewScale);

	PrintOrient		GetPrintOrient()						{ return Orient; }
	void 			SetPrintOrient(PrintOrient NewOrient);

	PrintFitType	GetFitType()							{ return FitType; }
	void			SetFitType(PrintFitType NewFitType);

	TypesetInfo*	GetTypesetInfo()						{ return &Typesetting; }

	
	INT32 GetTopMargin()				{ return TopMargin; }
	void SetTopMargin(INT32 NewVal)	{ TopMargin = NewVal; }
	INT32 GetLeftMargin()			{ return LeftMargin; }
	void SetLeftMargin(INT32 NewVal)	{ LeftMargin = NewVal; }
	INT32 GetWidth()					{ return ScaledWidth; }
	void SetWidth(INT32 NewVal);
	INT32 GetHeight()				{ return ScaledHeight;}
	void SetHeight(INT32 NewVal);

	INT32 GetRows()					{ return Rows; }
	BOOL SetRows(INT32 NewVal);
	INT32 GetColumns()				{ return Columns; }
	BOOL SetColumns(INT32 NewVal);
	INT32 GetGutter()				{ return Gutter; }
	BOOL SetGutter(INT32 NewVal);

	// Print output vars
	PrintLayers			GetPrintLayers()						{ return Layers; }
	void 				SetPrintLayers(PrintLayers NewVal)		{ Layers = NewVal; }
	PSLevel				GetPSLevel()							{ return PSLangLevel; }
	void 				SetPSLevel(PSLevel NewVal)				{ PSLangLevel = NewVal; }
	PrintMethodType		GetPrintMethod();						//{ return PrintMethod; }
	void 				SetPrintMethod(PrintMethodType NewVal);	//{ PrintMethod = NewVal; }
	BitmapResMethod 	GetBitmapResMethod()					{ return BitmapResolutionMethod; }
	void				SetBitmapResMethod(BitmapResMethod BRM) { BitmapResolutionMethod = BRM; }
	INT32				GetDotsPerInch();
	void 				SetDotsPerInch(INT32 NewVal);
	PrintFillQuality	GetFillQuality()						{ return FillQuality; }
	void 				SetFillQuality(PrintFillQuality Qual)	{ FillQuality = Qual; }
	PrintTextOptions	GetTextOptions()						{ return TextOptions; }
	void				SetTextOptions(PrintTextOptions pto)	{ TextOptions = pto; }

	// static print output vars that are not saved with the document.  Instead, these vars
	// are global app settings that have related ini file settings
	static PrintMethodType AppPrintMethod;	

	// General print vars
	BOOL			IsCollated()							{ return Collated; }
	void 			SetCollated(BOOL NewVal)				{ Collated = NewVal; }
	INT32			GetNumCopies()							{ return NumCopies; }
	void 			SetNumCopies(INT32 NewVal)				{ NumCopies = NewVal; }
	BOOL			GetPrintToFile()						{ return PrintToFile; }
	void 			SetPrintToFile(BOOL NewVal)				{ PrintToFile = NewVal; }
	PrintRangeObj	GetObjPrintRange();
	void 			SetObjPrintRange(PrintRangeObj NewVal)	{ ObjPrintRange = NewVal; }
	PrintRangeDPS	GetDPSPrintRange()						{ return DPSPrintRange; }
	void 			SetDPSPrintRange(PrintRangeDPS NewVal)	{ DPSPrintRange = NewVal; }

	// This will return either 1 or 2 depending on DPS, WholeSpread and DPSPrintRange
	INT32 GetNumPrintablePages();

	// Operators
	PrintControl& operator=(PrintControl& other);

	// Functions that deal with the generation of pages and patches used in the main print loop
	// We now have plate related functions too!
	BOOL StartPrinting();
	BOOL MorePaper();
	BOOL GetNextPaper();
	BOOL ReprintPaper();
	BOOL GetNextPatch(PrintPatchInfo* pPatchInfo);
	BOOL EndPrinting();

	BOOL StartPlatePrinting(PrintView *pPrintView, UINT32* const errID);
	BOOL MorePlates();
	BOOL SetNextPlate(PrintView *pPrintView);
	void EndPlatePrinting(PrintView *pPrintView);

	// Misc
	void StartImport();
	void EndImport();
	void RenderPrintBorder(RenderRegion* pRRegion);

	static BOOL RedrawPrintableArea(DocView* pDocView);

private:
	//----------------------------------------
	// Private members that are used by the print layout functions

	BOOL CalcNumPaper();	// Stuffs the num paper prints we will do into NumPaper
	
	BOOL	Printing;
	BOOL	SelectionPresent;
	DocRect	PatchClipRect;

	INT32 	NumPrintablePages;

	INT32	NumPaper;
	INT32	PaperNumber;
	INT32	MaxPaperNumber;
	BOOL 	Reprint;

	INT32	NumPatches;
	INT32	PatchNumber;
	INT32	MaxPatchNumber;

	// Multiple fit vars
	INT32		PatchRow;
	INT32		PatchColumn;
	MILLIPOINT	SectionWidth;
	MILLIPOINT	SectionHeight;

private:
	//----------------------------------------
	// Private member vars

	BOOL 	PrintAreaSetUp;	// Can only be TRUE if SetUp() called successfully
	BOOL	SetUpFailed;	// True if the last call to SetUp() failed
	Spread*	pSpread;		// ptr to the spread that contains objects that we will print

	// The total printable area.  This is the area which we will try and print.  
	// Only objects that lie within this area will be printed.  It is possible for the actual print
	// area for a particular piece of paper to be smaller than the total print area.
	//
	// Currently, TotalPrintArea represents the entire spread bounds (the union of all pages on the spread)
	// On double page spreads, we could be printing left and/or right pages.

	DocRect TotalPrintArea;

	// The printable area on the paper
	DocRect PrintableArea;
	INT32	PALeftMargin;
	INT32	PATopMargin;

	DocCoord Origin;

	INT32	PageWidth;		// Width  of each printed page of the document (before scaling)
	INT32	PageHeight;		// Height of each printed page of the document (before scaling)
	INT32	PaperWidth;		// Width  of paper we are printing to
	INT32	PaperHeight;	// Height of paper we are printing to

	INT32	OrigPageWidth;	// Width  of each printed page ignoring print orientation
	INT32	OrigPageHeight;	// Height of each printed page ignoring print orientation

	BOOL		DPS;			// TRUE if the document page is a double page spread
	MILLIPOINT	Bleed;			// The bleed margin for the spread
	MILLIPOINT	CropArea;		// The size of the crop area

	// Print layout vars
	BOOL			WholeSpread;

	FIXED16			Scale;
	PrintOrient		Orient;

	PrintFitType	FitType;

	INT32			TopMargin;		// Top  margin used in Custom fit
	INT32			LeftMargin;		// Left margin used in Custom fit
	INT32			ScaledWidth;	// Width  of each printed page (after scaling)
	INT32			ScaledHeight;	// Height of each printed page (after scaling)
	INT32			CropTopMargin;	// Crop top margin size used in Custom and Multi fit
	INT32			CropLeftMargin;	// Crop left margin

	INT32			Rows;
	INT32			Columns;
	INT32			Gutter;

	// Print typeset vars
	TypesetInfo		Typesetting;	// Our typeset object.
	
	// Print output vars
	PrintLayers			Layers;
	PSLevel				PSLangLevel;
//	PrintMethodType		PrintMethod;
	BitmapResMethod 	BitmapResolutionMethod;
	INT32				DotsPerInch;
	PrintFillQuality	FillQuality;
	PrintTextOptions	TextOptions;

	// General print vars
	BOOL			Collated;
	INT32			NumCopies;
	BOOL			PrintToFile;
	PrintRangeObj	ObjPrintRange;
	PrintRangeDPS 	DPSPrintRange;

	// Misc
	BOOL UpdateDependants;	// If TRUE, then setting certain members changes others, e.g. Setting Scale changes width, etc

	//-------------------------------
	// Private functions

	void SetScaleFactor(double ScaleFactor);		// Set scale percentage using a scale factor
	BOOL CalcPrintAreaVars(BOOL RedrawPrintBorders = TRUE);

	// Recalcs the scale factor and scaled width & height according to the current fit type
	// plus any other relevent settings, such as orientation, etc
	BOOL CalcFit();

	void CalcMultipleFit(INT32 NumRows,INT32 NumColumns,INT32 GutterVal,INT32 Wd,INT32 Ht,INT32 Extra);
	INT32 GetCropAdjust() const;

	double 	CalcPatchWidth(INT32 PaperWidth,INT32 NumColumns,INT32 GutterVal);
	double 	CalcPatchHeight(INT32 PaperHeight,INT32 NumRows,INT32 GutterVal);
	INT32 	CalcMaxRows(INT32 GutterVal);
	INT32 	CalcMaxColumns(INT32 GutterVal);
	INT32 	CalcMaxGutter(INT32 NumRows,INT32 NumColumns);

	void	RenderRect(RenderRegion* pRRegion,DocRect Rect,BOOL Fill);
};


/********************************************************************************************

>	class PrintPatchInfo: public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Purpose:	This class encapsulates all the info required to print the next patch on the
				current piece of paper

	SeeAlso:	PrintControl::GetNextPatch()

********************************************************************************************/

class PrintPatchInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(PrintPatchInfo)

	friend class PrintControl;

public:
	PrintPatchInfo();
	~PrintPatchInfo();

	// Note there are NO set functions. This is on purpose.
	DocRect		GetClipRect(BOOL AddBleed=FALSE, BOOL AddCropArea=FALSE) const;
	void   		InflateRectBy(DocRect *pRect, BOOL AddBleed=FALSE, BOOL AddCropArea=FALSE) const;
	void   		InflateRectBy(OilRect *pRect, BOOL AddBleed=FALSE, BOOL AddCropArea=FALSE) const;
	void   		DeflateRectBy(DocRect *pRect, BOOL AddBleed=FALSE, BOOL AddCropArea=FALSE) const;
	MILLIPOINT	GetBleed() const;
	MILLIPOINT	GetCropArea() const;

public:
	FIXED16		Scale;
	MILLIPOINT	XTrans;
	MILLIPOINT	YTrans;
	BOOL		Rotate;
	BOOL		SelectedOnly;
	BOOL		VisibleLayers;
	BOOL		EmulsionDown;
	Spread*		pSpread;
	MILLIPOINT	PaperWidth;
	MILLIPOINT	PaperHeight;
	INT32		ScaledWidth;
	INT32		ScaledHeight;
	INT32		PaperNumber;
	INT32		MaxPaperNumber;
	INT32		PatchNumber;
	INT32		MaxPatchNumber;
	INT32		PALeftMargin;
	INT32		PATopMargin;

private:
	DocRect		ClipRect;
	MILLIPOINT	Bleed;
	MILLIPOINT	CropArea;

};



#endif // INC_PRINTCTL
