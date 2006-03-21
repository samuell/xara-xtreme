// $Id: bmpprefs.h 662 2006-03-14 21:31:49Z alex $
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
// Header file for BmpPrefs.cpp
//
// 

#ifndef INC_BMPPREFS
#define INC_BMPPREFS

//#include "bmpdlg.h"
#include "bitmap.h"
//#include "dialogop.h"
#include "dibconv.h"				// for DITHER
//#include "gpalopt.h"

class BitmapExportDocument;
class DocRect;
class MaskedFilterExportOptions;
class BitmapExportParam;
class BmpDlgParam;
class CXaraFileRecord;


enum SelectionType { SELECTION, DRAWING, SPREAD, ABITMAP, SOMEBITMAPS, PAGE };

enum PALETTE
{
	PAL_STANDARD	= 0,	// just to make sure
	PAL_OPTIMISED,
	PAL_BROWSER,
	PAL_GLOBALOPTIMISED,
	PAL_WEBSNAP				//  For the 'WebSnap optimised' option.
};


//  This enum is used with the antialiasing options on the bitmap tab
//  of the bitmap export options dialog.
enum ANTIALIASING
{
	MAINTAIN_SCREEN_AA	= 0,	//  Just to make sure
	MINIMISE_VISIBLE_AA
};


//  The following 2 structures are for an 'expanded' palette, containing all the information shown 
//  in the palette control of the bitmap export dialog. 
typedef struct
{
	BYTE Red;
	BYTE Green;
	BYTE Blue;
	INT32 Flags;					//  properties that the colour has.	
	BYTE PreEditedRed;
	BYTE PreEditedGreen;
	BYTE PreEditedBlue;
} ExtendedPaletteEntry;


typedef struct
{
	INT32 NumberOfColours;
	ExtendedPaletteEntry Data[256];
} ExtendedPalette;


//  Enum for the different display types in the File type drop list in the bitmap export dialog.
//  The current type is stored in a variable in the bitmap export options.
enum ExportFileType { GIF_TYPE = 0, PNG_TYPE, BMP_TYPE, JPG_TYPE };

typedef double		DPI;
typedef UINT32		BMP_SIZE;	// that's in pixels
typedef UINT32		BMP_DEPTH;	// that's in planes
typedef UINT32		FILTER_ID;

/********************************************************************************************
>	PaletteMarkType
	Author:		Alex_Price (Xara Group Ltd) <camelotdev@xara.com>
	Date:		01/07/99
	Comment:	This is meant for the palette drawing code. It represents
				all the different marks that can be drawn in any one of the palette control's
				colour rectangles.

				...and the flags the palette uses! -- Jonathan
********************************************************************************************/ 
typedef enum
{
	LOCKED_COLOUR			= 1,
	TRANSPARENT_COLOUR		= 2,
	DELETED_COLOUR			= 4,
} PaletteMarkType;


/********************************************************************************************
>	class BitmapExportOptions : public OpParam

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/10/96
	Purpose:   	Base class for storing information about the bitmap we're exporting.
********************************************************************************************/

class BitmapExportOptions : public OpParam
{
	CC_DECLARE_DYNCREATE(BitmapExportOptions)

public:
	BitmapExportOptions();
    BitmapExportOptions(const CDlgResID DialogID, const FILTER_ID FilterID, 
						const StringBase* pFilterName);
	~BitmapExportOptions();
	static BOOL Init();
	static BOOL Declare();

	// Data Integrity Checking
	BOOL		IsValid() const;
	void		MarkInvalid();
	void		MarkValid();

	// Options persistence
	virtual BOOL	RetrieveDefaults();
	virtual BOOL	SetAsDefaults() const;

	// Save and load from named export set properties.
	virtual BOOL Write(CXaraFileRecord* pRec);
	virtual BOOL Read(CXaraFileRecord* pRec);

	// Data member access functions
	DPI				GetDPI() const;
	BOOL			SetDPI(const DPI& Dpi);

	virtual BOOL	GetSupportsImageMap() { return FALSE; }

	// WEBSTER - markn 5/2/97
	virtual INT32 GetTransparencyIndex()	const					{ return m_TransparencyIndex; }
	virtual void SetTransparencyIndex(INT32 TransparencyIndex);

	BMP_DEPTH		GetDepth() const;
	BOOL			SetDepth(const BMP_DEPTH& Depth);

	SelectionType	GetSelectionType() const	{ return m_SelectionType;	}
	void			SetSelectionType(const SelectionType& Sel);

	BOOL			GetSeparateLayerFiles() const				{return m_bSeparateLayerFiles;}
	void			SetSeparateLayerFiles(const BOOL newFlag)	{m_bSeparateLayerFiles = newFlag;}

	BOOL			GetTempFileFlag() const						{return m_bTempFileFlag;}
	void			SetTempFileFlag(const BOOL newFlag)			{m_bTempFileFlag = newFlag;}

	CDlgResID			GetDialogType() const				{ return m_DialogID;	}
	const FILTER_ID		GetFilterType() const				{ return m_FilterID;	}
	const StringBase*	GetFilterName() const;

	// smfix 
	virtual UINT32 GetFilterNameStrID() { return 0;} // this should be overriden by other export option subclasses
	BaseBitmapFilter * FindBitmapFilterForTheseExportOptions(); // uses the about String ID to find a filter of the right name

	Coord			GetOutputSize()					{ return m_OutputSize;	}
	void			SetOutputSize(UINT32 x, UINT32 y)	{ m_OutputSize.x = (INT32) x; m_OutputSize.y = (INT32) y;	}
	Coord			GetPixelOutputSize()					{ return m_PixelOutputSize;	}
	void			SetPixelOutputSize(UINT32 x, UINT32 y)	{ m_PixelOutputSize.x = (INT32) x; m_PixelOutputSize.y = (INT32) y;	}
	BmpDlgParam*	GetBmpDlgParam()				{ return m_pBmpDlgParam; }
	BOOL			SetBitmapExportParam(BitmapExportParam* pParam); 
	virtual BOOL	CanExportSeparateLayers();

// WEBSTER - markn 17/1/97
	virtual void SetNumColsInPalette(UINT32 NumCols)			{ m_NumColsInPalette = NumCols; }
	virtual void SetUseSystemColours(BOOL SystemColours)	{ m_UseSystemColours = SystemColours; }
	virtual UINT32 GetNumColsInPalette()						{ return m_NumColsInPalette; }
	virtual BOOL GetUseSystemColours()						{ return m_UseSystemColours; }
	virtual BOOL UseSpecificNumColsInPalette()				{ return FALSE; }

	//  Access functions for the aa variable.
	ANTIALIASING	GetAntiAliasing()	const				{ return m_Antialiasing; }
	void			SetAntiAliasing( ANTIALIASING aa );

	// Provide a baseclass version which returns something
	virtual	DITHER GetDither() const				{ return XARADITHER_NONE; }
	virtual BOOL SetDither(const DITHER&) { ERROR3("The base class has no concept of dithering"); return FALSE; }

	BOOL ShouldPutHTMLTagOnClipboard()				{ return m_fPutHTMLTagOnClipboard; }
	void SetPutHTMLTagOnClipboard(BOOL fValueToSet)	{ m_fPutHTMLTagOnClipboard = fValueToSet; }

	// for handling the temporary file during export
	void SetPathName(PathName *pTempPath);
	PathName GetPathName()							{ return m_TempPath; };
	BOOL HasTempFile()								{ return m_bTempFileExported; }
	void SetTempFileMatchesExportOptions(BOOL OptionsMatch) {m__TempFileMatchesExportOptions = OptionsMatch; }
	BOOL DoesTempFileMatchExportOptions() { return m__TempFileMatchesExportOptions; }

	// for object copying and comparing
	BitmapExportOptions* MakeCopy();
	virtual BOOL CopyFrom(BitmapExportOptions* pOther);

	// Same as above, except it is used to copy information when changing file types
	virtual BOOL FileTypeChangeCopyFrom(BitmapExportOptions *pOther);
//	virtual BOOL Equal(BitmapExportOptions *pOther);

	void SetCanUseDesignNotes(BOOL ok)				{ m_UseDesignNotes = ok; }
	BOOL GetCanUseDesignNotes() const				{ return m_UseDesignNotes; }

//	void SetPalette(ExtendedPalette* pPalette);
	ExtendedPalette* GetPalette();
	//SMFIX

	// Given an index into the logical palette, this function sets its flags so it is unreadable - this is used
	// as a bodge when deleting palette entrys
	void MakePaletteEntryUnreadable(INT32 i) { if (m__pLogicalPalette) m__pLogicalPalette->palPalEntry[i].peFlags = 255; }

	// create palette Optimiser
	// returns TRUE if created
	// returns FALSE if we already have a valid palette optimiser
	BOOL CreatePaletteOptimiser();
	// flush the palette optimiser if the bitmap source pixels have changed
	// since this will affect the stats that were fed into it
	void BitmapSourceHasChanged();
	// feed the optimiser with stats from the image
	void GatherPaletteStats(const RGBQUAD* pBitmap, UINT32 nSize );
	// this sets all the extended palette to fill in the optimised palette colours
	void CreateValidPalette();
	// this likewise sets the extended palette but from a known logical palette
	// which is only used in the creation of a global palette for all animated frames
	void CreateValidPalette(LOGPALETTE * pPal, INT32 TranspIndex = 0);
	// this likewise sets the extended palette to a preset browser palette
	// which is only used in the creation of a global palette for all animated frames
	void CreateValidBrowserPalette(BOOL WantTransp = TRUE);

	// used by the back end to get the logical palette which is managed
	// by this class and based on the extended palette
	LOGPALETTE * BitmapExportOptions::GetLogicalPalette();

	// Get Logical Palette gets uses the pre-edited colours as these are needed for consistant results
	// this function swaps these preedited colours to the postedited colours for any locked colour
	BOOL SwapEditedColoursInLogicalPalette();

	INT32 FindNextLockedColour(INT32 pos); // internal fn

	// the user wants a background transparent colour?
	BOOL IsBackgroundTransparent() { return m__BackgroundIsTransparent; }
	void SetBackgroundTransparency(BOOL t);

	// get the extended palette. Use this to edit what the locked colours are and
	// display the colours used etc.
	ExtendedPalette	* GetExtendedPalette() { return &m_Palette; }

	BOOL IsClipToPage();

	// sjk 20/10/00 access functions to the variables put in to drive the palette optimiser
	// SMFIX
	BOOL IsPaletteValid(); // is the palette in this export object completely valid and perfect?
	void InvalidatePalette();
	INT32	 GetNumberOfUserRequestedColours();
	void SetNumberOfUserRequestedColours(INT32 n);
	BOOL IsUsingBrowserPalette();
	void SetToUseBrowserPalette(BOOL SetIt);
	BOOL IsUsingSystemPalette();
	void SetToUseSystemPalette(BOOL SetIt);
	INT32  GetWebSnapPaletteAmount();
	void SetWebSnapPaletteAmount(INT32 SetIt);
	INT32  GetPrimarySnapPaletteAmount();
	void SetPrimarySnapPaletteAmount(INT32 SetIt);

	// SMFIX!! New Functions to help dialog setup
	DWORD GetSupportedDithers();
	DWORD GetInterlaceType();
	DWORD GetSupportedColourDepths();
	BOOL GetSupportsTransparency();
	BOOL GetSupportsPalette();

	void SetCMYK(BOOL SetIt);
	BOOL IsCMYK();

	// this enum is a list of colour depths we can show.
	// NOTE!!!! The values are bitfiled values so make sure adding a new one
	// is set to 64!!!!!
	enum eCOLOUR_DEPTHS
	{
		COLOUR_2 = 1,
		COLOUR_16 = 2,
		COLOUR_256 = 4,
		COLOUR_16BIT = 8,
		COLOUR_24BIT = 16,
		COLOUR_32BIT = 32
	};

	// this enum is a list of Interlacing and progresion we can show.
	// NOTE!!!! The values are bitfiled values so make sure adding a new one
	// is set to 8!!!!!
	enum eINTERLACE_PROGRESSIVE
	{
		GIF_INTERLACE = 1,
		PNG_INTERLACE = 2,
		JPG_PROGRESSIVE = 4
	};

	// this enum is a list of Dithering we can show.
	// NOTE!!!! The values are bitfiled values so make sure adding a new one
	// is set to 4!!!!!
	enum eDITHERING
	{
		ORDERED_DITHER = 1,
		ERROR_DITHER = 2
	};

protected:
	BOOL ArePalettesEqual(BitmapExportOptions* pOther);
	void CopyPaletteInformationFrom(BitmapExportOptions* pOther);

protected:
	SelectionType	m_SelectionType;			// ...remove this
	Coord			m_OutputSize;				// ...and this
	Coord			m_PixelOutputSize;			// Added by Graham 1/7/97
	FILTER_ID		m_FilterID;					// keep this rubbish for now
	BmpDlgParam*	m_pBmpDlgParam;
	BOOL			m_UseDesignNotes;			// sjk - use compat. files with Dream Weaver?
	BOOL			m_bSeparateLayerFiles;			// True if should export layers as separate files
	BOOL			m_bTempFileFlag;

// WEBSTER - markn 17/1/97
	UINT32			m_NumColsInPalette;
	BOOL			m_UseSystemColours;
	static UINT32		g_NumColsInPalette;			// These persistant vars maintained in MaskedFilterExportOptions
	static BOOL		g_UseSystemColours;		

public:
	BOOL m_fPutHTMLTagOnClipboard;				// Whether an HTML tag should be put on the clipboard
	static BOOL ms_fPutHTMLTagOnClipboard;		// The default value of whether an HTML tag should be put on the clipboard
	static TCHAR ms_strPutHTMLTagOnClipboard[];	// The string identifying our HTML tag preference
	static BOOL ms_IsBackGroundTransparent;
	static ANTIALIASING	ms_Antialiasing;
	static BOOL ms_bSeparateLayerFiles;


private:
	// Parameters that the dialog can set
	BOOL				m_bValid;				// Ok or cancel selected 
	DPI					m_DPI;
	BMP_DEPTH			m_Depth;

	// WEBSTER - markn 5/2/97
	INT32				m_TransparencyIndex;
	CDlgResID			m_DialogID;
	const StringBase*	m_pFilterName;			// pointer to filter name

	// for the export preview dialog
	PathName			m_TempPath;				// the path to the temp file
	BOOL				m_bTempFileExported;	// true, if a temp file was exported

	//  Need the following to hold all the palette information for the bitmap
	ExtendedPalette		m_Palette;
	
	//  For use with the aa options in the bitmap size tab of the bitmap export dialog
	ANTIALIASING		m_Antialiasing;

//	// Persistent buff
//	static String_256 g_DefaultPath;


	// new variables added 20/10/00 by sjk in the recoding of the bitmap export dlg
	// the theory is that these export options can be used to regenerate the bitmap correctly
	// even if some change has occured to it
	// SMFIX

	BOOL m__PaletteIsValid; // this is the major flag, TRUE means that the palette held
							// by the export object (m_Palette) is THE palette to use - no arguements.
							// FALSE means that something has changed and the palette may no longer
							// be exactly the best palette to use. But do not disregard it, it still holds the
							// locked colours and the users edited colours. These can be passed to the palette
							// optimiser to generate an optimal palette containing these colours that meets
							// the spec of the export options.

	LOGPALETTE * m__pLogicalPalette; // A logical palette that is managed by this class. It can be accessed by GetLogicalPalette()
									 // but should never be deleted by any other class.
									 // This is invalidated when the palette is validated and is created on demand from the
									 // extended palette.

	INT32 m__NumberOfColoursUserRequested; // This is the figure that the user has typed in the no of colours field.
										 // this is a top range of the number of colours that we will give in the palette.
										 // the palette may contain fewer than this.
										 // If the colour depth is changed this value should be changed to match the largest
										 // value available at this colour depth.

	// Flags of user preference that the palette optimiser needs to build a good optimal palette
	BOOL m__UseBrowserPalette;
	BOOL m__UseSystemColoursInPalette;
		// use like a bool, is an INT32 for future proofing. The value if the distance of the snap.
	INT32 m__UseWebSnapPalette;		// usually 12 if on
	INT32 m__UsePrimarySnapPalette;	// usually 1 if on

	BOOL m__BackgroundIsTransparent; // Does the user want the background to be visible or not
									 // if this is off this doesn't mean that there can be no transparency as the
									 // user could have marked an arby colour in the palette as transparent
PORTNOTE("other","Need PaletteOptimiser when implemented")
#ifndef EXCLUDE_FROM_XARALX // NB Nested
	PaletteOptimiser   *m__pPaletteOptimiser; // ptr to the current palette optimiser
											 // this can be null if there isn't one or we dont need a palette
#endif // NB Nested
	BOOL m__HavePrimedOptimier;	// m__pPaletteOptimiser may be not null but has it been fed with stats and these stats been processed?

	BOOL m__TempFileMatchesExportOptions;	// This is TRUE when the options haven't been changed in a way
											// that would affect the appearance of the temp file
	BOOL m_bCMYK;
};


/********************************************************************************************
>	class BmpPrefsDlg : public DialogOp

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	23/08/94
	Purpose:	The export bitmaps preference dialog box
	
				Allows the user to set options like:-
				Save selection, drawing or spread
				Bitmap resolution
				Colour depth
				Dithering type
				TIFF/JPEG/BMP  	
********************************************************************************************/

#define OPTOKEN_BMPPREFSDLG _T("BmpPrefsDlg")

PORTNOTE("other","Removed BmpPrefsDlg - derived from DialogOp")
#ifndef EXCLUDE_FROM_XARALX
class BmpPrefsDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(BmpPrefsDlg)

public:
	BmpPrefsDlg();
	BOOL Create();							// Create this dialog

	static BOOL Init();										// Setup function
	static OpState GetState(String_256*, OpDescriptor*);	// Return the state of this operation
	virtual void DoWithParam(OpDescriptor*, OpParam* Param);
	virtual void Do(OpDescriptor*);
	
	// Set up explicit details about this dialog box such as the id and type
	static CDlgResID IDD;					// Dialog box id
	static const CDlgMode Mode;				// Dialog box mode of operation (mode or modeless)

	static BOOL InvokeDialog(BitmapExportOptions* pOptions);

protected:
	// Overridables
	virtual BOOL		InitDialog();
	virtual MsgResult	Message(Msg* Message);		// Message handler
	virtual BOOL		CommitDialogValues();

	// Support Functions
	BitmapExportOptions*	GetOptions() const;

	typedef UINT32	TIFF_COMPRESSION;
	BOOL SetTIFFCompressionSelection( TIFF_COMPRESSION Compression, INT32 nBPP );
	TIFF_COMPRESSION GetTIFFCompressionSelection( INT32 nBPP );

	void SetBPPButtons(CGadgetID ButtonClicked,BOOL CheckNumColValueRange);	// handles things reliant on bpp
	void SetPaletteButtons(CGadgetID ButtonClicked);
	void SetDitherButtons(CGadgetID ButtonClicked);
	void RecalculateSize();							// Works out the selection size
	void RecalculatePixels(); 						// Works out the size in pixels
	void RecalculateDpiFromX();						// Works out the dpi from x pixels
	void RecalculateDpiFromY();						// Works out the dpi from y pixels

	SelectionType GetSelection();					// returns spread/drawing/selection
	BOOL SetUpResolutionList();						// Put values into resolution como box

	void InitDepthRadioGroup();		// GetDepthGadget()->Init()???
	void InitPaletteRadioGroup(const PALETTE& Palette);
	void InitDitherRadioGroup(const DITHER& DitherType);
	void InitSelectionRadioGroup();

	UINT32 GetFilterHelpID(UINT32 FilterType);

// WEBSTER - markn 17/1/97
	void InitNumColoursGroup(BitmapExportOptions* pOptions);
	void HideNumColoursGroup();
	void UpdateNumColoursGroup(BOOL CheckNumColValueRange);	// New 'CheckNumColValueRange' flag - WEBSTER - markn 1/2/97
	BOOL SetNumColoursInPalette(BitmapExportOptions* pOptions);

	inline	void SetLoopCheckBox(BOOL Value)	{ LoopCheckBoxEnabled = Value;	}	// Selector function
	inline	BOOL GetLoopCheckBox()				{ return LoopCheckBoxEnabled;	}	// Selector function
	BOOL	SetBmpPrefsDlgState();													 	
	
	// Remember the last bpp setting
	CGadgetID LastBppSelected;

	// Remember the selection/drawing/spreads height and width in Millipoints
	// and the DocRect that was used to calculate this.
	MILLIPOINT Width;	
	MILLIPOINT Height;	
	INT32 PixelWidth;	
	INT32 PixelHeight;
	double Dpi;	
	DocRect	ClipRect;

	BOOL m_bDpiSupported;			// Does the (should be) given filter support DPI?
	BOOL m_bLockAspect;				// Should we lock the aspect ratio ?
	BOOL IsDelayMany;				// Is the delay value set to MANY ?	
	BOOL IsDelayDefault;			// Is the delay value set to the default of 10cs ?
	BOOL HasDelayChanged;			// Has user changed the Delay ed field?
	BOOL HasRestoreChanged;			// Has the user changed the Restore ed field?
	BOOL IsManyDisplayed;			// Is MANY displayed in the Restore ListBox ?
	BOOL DisplayMany;				// Should we display MANY ? 
	BOOL LoopCheckBoxEnabled;		// Is the Loop Continously check box selected.

private:
	BitmapExportOptions*	m_pOptions;

	// Variables used to lock out other fields being updated just by putting a
	// new number into them and hence getting circular updates.
	BOOL RecalculatingPixels;
	BOOL RecalculatingXDpi;
	BOOL RecalculatingYDpi;
};
#endif



/********************************************************************************************
>	class ExportSelection : public CC_CLASS_MEMDUMP
	class RandomSelection : public ExportSelection
	class SingleBitmapSelection : public ExportSelection
	class MultipleBitmapSelection : public ExportSelection
	class SpreadSelection : public ExportSelection
	class DrawingSelection : public ExportSelection

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/06/96
	Purpose:   	Abstract classes representing selections of objects
				They're not actually used much though ...
********************************************************************************************/

class ExportSelection : public CC_CLASS_MEMDUMP
{
public:
	virtual ~ExportSelection() { }
	// Don't use this: for backwards compatability only
	virtual SelectionType GetSelectionType() = 0;
	virtual DocRect GetSize() = 0;
};


class RandomSelection : public ExportSelection
{
public:
	RandomSelection(SelRange* pRange) : m_pSelectionRange(pRange)	{}

	virtual DocRect GetSize();
	virtual SelectionType GetSelectionType()	{	return SELECTION;	}

protected:
	SelRange*	m_pSelectionRange;
};


class SingleBitmapSelection : public ExportSelection
{
public:
	SingleBitmapSelection(KernelBitmap* pBitmap) : m_pBitmap(pBitmap) {}
	virtual DocRect GetSize();
	virtual SelectionType GetSelectionType()	{	return ABITMAP;	}

protected:
	KernelBitmap*	m_pBitmap;
};


class MultipleBitmapSelection : public ExportSelection
{
public:
	MultipleBitmapSelection(BitmapExportDocument* pBitmaps) : m_pBitmaps(pBitmaps)	{}
	virtual DocRect GetSize();
	virtual SelectionType GetSelectionType()	{	return SOMEBITMAPS;	}

protected:
	BitmapExportDocument*	m_pBitmaps;
};


class SpreadSelection : public ExportSelection
{
public:
	SpreadSelection(Spread* pSpread) : m_pSpread(pSpread) {}
	virtual DocRect GetSize();
	virtual SelectionType GetSelectionType()	{	return SPREAD;	}

protected:
	Spread*		m_pSpread;
};


class DrawingSelection : public ExportSelection
{
public:
	DrawingSelection(Spread* pSpread) : m_pSpread(pSpread) {}
	virtual DocRect GetSize();
	virtual SelectionType GetSelectionType()	{	return DRAWING;	}

protected:
	Spread*		m_pSpread;
};



/********************************************************************************************
>	class JPEGExportPrefsDialog : public BmpPrefsDlg

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/11/96
	Purpose:	Provides a dialog asking for user options in the JPEG export
********************************************************************************************/

PORTNOTE("other","Removed JPEGExportPrefsDialog - derived from BmpPrefsDlg")
#ifndef EXCLUDE_FROM_XARALX
class JPEGExportPrefsDialog : public BmpPrefsDlg
{
	CC_DECLARE_DYNCREATE( JPEGExportPrefsDialog )

public:
	JPEGExportPrefsDialog();
	void	SetJPEGDlgState ();
	inline BOOL GetOriginalSourcePresent() const		{ return m_OriginalSourcePresent;  } 
	inline void SetOriginalSourcePresent(BOOL Value)	{ m_OriginalSourcePresent = Value; }

protected:
	// Overridden functions
	virtual BOOL		InitDialog();
	virtual MsgResult	Message(Msg* Message);
	virtual BOOL		CommitDialogValues();
	
	BOOL m_OriginalSourcePresent;
};
#endif


/********************************************************************************************
>	class PhotoCDDlgParam : public OpParam

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	23/11/94
	Purpose:	Allows data to be passed into the photocd options dialog box and 
				then returned to the caller.
********************************************************************************************/

class PhotoCDDlgParam : public OpParam
{
	CC_DECLARE_MEMDUMP(PhotoCDDlgParam)

public:
    PhotoCDDlgParam(UINT32 Page)
	  : NumberOfPages(Page) { /* empty */ }

	// Parameters that the dialog can set
	UINT32 ImageNumber;					// number of the image chosen by the user
	UINT32 NumberOfPages;					// number of pages available
	BOOL PhotoCDOk;						// Ok or cancel selected 
};



/********************************************************************************************
>	class ImpPhotoCDDlg : public DialogOp

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	23/11/94
	Purpose:	The import photocd dialog box.
				Allows the user to select which of the photocd images are imported.
********************************************************************************************/

#define OPTOKEN_PHOTOCDDLG _T("PhotoCDDlg")

PORTNOTE("other","Removed PhotoCDDlg - derived from DialogOp")
#ifndef EXCLUDE_FROM_XARALX
class PhotoCDDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE( PhotoCDDlg )

public:
	PhotoCDDlg();									// constructor

	void DoWithParam(OpDescriptor*, OpParam* Param); // "DoWithParam" function
	void Do(OpDescriptor*); 						// "Do" function
	static BOOL Init();								// Setup function
	BOOL Create();									// Create this dialog
	static OpState GetState(String_256*, OpDescriptor*);
													// Return the state of this operation
	virtual MsgResult Message(Msg* Message);
													// Message handler
	
	// Set up explicit details about this dialog box such as the id and type
PORTNOTE("other","Need dialog resources PhotoCDDlg")
#ifndef EXCLUDE_FROM_XARALX
	enum { IDD = _R(IDD_PHOTOCDOPTS) };					// id
#endif
	static const CDlgMode Mode;						// mode or modeless

public:
	static BOOL InvokeDialog(UINT32 *ImageNumber);

private:
	BOOL InitDialog(PhotoCDDlg* pPhotoCD);	// Standard set up function
	
protected:
	// Useful dialog processing functions
	BOOL CommitDialogValues(PhotoCDDlg* pPhotoCD);

	// Parameter passing class
	static PhotoCDDlgParam *pParams;
}; 
#endif

#endif  // !INC_BMPPREFS
