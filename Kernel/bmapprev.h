// $Id: bmapprev.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_BMAPPREV
#define INC_BMAPPREV

//#include "bmpprefs.h"	// ExportFileType
//#include "bmpalctrl.h"	// Required as BmapPrevDlg HasA BitmapExportPaletteControl

class BitmapExportPreviewDialog;
class BrowserPreviewOptions;
class ImagemapFilterOptions;
class BubbleHelpWnd;

#define OPTOKEN_GIFTABDLG _T("BmapPrevDlg")

PORTNOTE("dialog","Removed BmapPrevDlg - derived from DialogTabOp")
#ifndef EXCLUDE_FROM_XARALX
class BmapPrevDlg : public DialogTabOp
{         
	CC_DECLARE_DYNCREATE( BmapPrevDlg )  
	
public:		// Contructors
	BmapPrevDlg(); 
	~BmapPrevDlg();

public:		// Interface (the good bit)
	static BOOL Init();
	MsgResult Message(Msg* Message);
	void SetDontWantMouseMessage( BOOL  ) { ASSERT(FALSE); }
	void OnHelpButtonClicked();
	void UpdateCurrentTab();
	void SetPaletteSelection(INT32 index);
		// Set the selected colour in the palette control
	void SetPaletteHighlight(INT32 index);
		// Set the highlighted colour in the palette control
	BOOL DoPreview();

public:		// Interface
	// PLEASE NOTE: The following two functions have no implementation
	// and are very broken - any code that uses them should be changed
	static INT32 GetNumberOfTransparentColours();
	static INT32 GetTransparentColour( INT32 Index );

public:		// Public data (bad design)
	static BitmapExportOptions *m_pExportOptions;
	static BOOL m_bUseExistingPalette;
	static PathName m_pthExport;			// The path we are exporting the bitmap to
	static BOOL m_bSlicingImage;			// Are we doing image slicing or not?
	static BOOL m_bIsCreateBitmap;			// Are we using the Create Bitmap dialog?
	static INT32 m_NumOfColoursUserAskedFor;	// this is the value that the user has typed into no of clours field
											// or if they didn't type it is the top value defined by the max possible at this colour depth
//	static BOOL m_bNeedPaletteUpdated;
	static BOOL m_bIsOrderedDither;
	//  Did the user click on Export or not? This variable is used when the file type
	//  has changed, and so have the options.
	static BOOL m_bClickedOnExport;
	//  This is the original file type for use with the above variable
	static ExportFileType m_OriginalFileType;
	UINT32 m_FilterType;

private:	// Functions
	virtual void DoWithParam(OpDescriptor*, OpParam* pParam);
	virtual BOOL RegisterYourPagesInOrderPlease(); 

	BOOL OnCreate();
	void SetButtonsText();
	void SetPreviewButtonState(bool enabled);

	void DoBubbleHelp();
	void PrepareBubbleHelp();
	void UpdateStatusBar(UINT32 id);

	void SetPreEditedPaletteColour(INT32 r = -1, INT32 g = -1, INT32 b = -1);

	// Functions I have not deleted yet
	INT32 GetPositionOfItemInDropList( INT32 ListID, INT32 ItemID );
	BOOL SetSelectedDropListItem( INT32 ListID, INT32 ItemID );
	void RenderControl( ReDrawInfoType* pExtraInfo );

	void BrowserPreviewGetOptions(BrowserPreviewOptions *pBrowserOptions);

	void ImageMapGetOptions(ImagemapFilterOptions* pOptions);
	void ImageMapOnFileNameChanged();
	void ImageMapOnBrowseClicked();
	void ImageMapEnableExportToFileOptions(BOOL fValue);
	void ImageMapEnableOptions(BOOL fValue);
	void ImageMapOnCreate();
	void ImageMapOnCommit();
	WinRect GetExportSize(double dpi);

private:	// Data
	BitmapExportPreviewDialog *m_pPreviewDlg;			// pointer to the preview dialog
	CWindowID m_DialogWnd;								// pointer to the preview dialog window
	BaseBitmapFilter *m_pBmpFilter;
	BOOL m_bDpiSupported;								// Does the (should be) given filter support DPI?
	BOOL m_CurrentPreview;
	BOOL m_bPopularityValuesWanted;
PORTNOTE("other","Removed m_PaletteControl")
#ifndef EXCLUDE_FROM_XARALX // note nested!
	BitmapExportPaletteControl m_PaletteControl;
#endif //EXCLUDE_FROM_XARALX - note nested!
	bool m_bImageMapTabCreated;
	bool m_bBrowserPreviewTabCreated;
	enum { CLIPBOARD_NONE, CLIPBOARD_IMAGEMAP, CLIPBOARD_HTML } m_ClipBoardUsage;
	typedef enum { MODEL_INVALID, MODEL_RGBHEX, MODEL_RGBPERCENT, MODEL_RGBNUMBER, MODEL_HSV } ColourTypes;
	ColourTypes m_ColorModelType;
	bool m_MouseInsidePaletteControl;					// variable used to find out when the mouse leaves
														// the palette control
	BOOL m_LockSizeUpdates;								// when we update on sizes

	// Bubble help stuff
	BubbleHelpWnd *m_pBubbleWnd;						// pointer the the bubble help window
	UINT32 m_LastCursorOverControlID;						// the id of the last control the cursor was over
	MonotonicTime *m_pBubbleTimer;						// The timer for the bubble help

private:	// Statics
	static const UINT32 IDD;
	static const CDlgMode Mode; 
	static OpState GetState(String_256*, OpDescriptor*);	

	static bool m_MovedWindow;
		// Have we moved the main window to the correct palce?  (This needs to be static so
		// it can be used each time the dialog is created).

private:	// Init methods for each tab/control
//	void InitTabControls();

	void InitPaletteTab();
		void InitPaletteDitheringList();
		void InitPalettePaletteList();
		void InitPaletteColourDepthList();
		void InitPaletteColourModelList();
		void InitPaletteButtonStrip();
		void InitPaletteSortList();

	void InitBitmapSizeTab();
		void InitBitmapSizeDPIList();
		void InitBitmapSizeSelectionRadioGroup();
		void InitBitmapSizeAntiAlisingRadioGroup();
		void InitBitmapSizePutHTMLTick();

	void InitBitmapOptionsTab();

private:	// Message handlers for each tab/control

	// The Palette Tab
	void HandlePaletteTabMsg(DialogMsg* Msg);
		// Combo boxes & edit controls
		void HandlePaletteDitheringListChange();
		void HandlePalettePaletteListChange();
		void HandlePaletteColourDepthListChange();
		void HandlePaletteColoursUsedChange();
		void HandlePaletteColourModelListChange();
		void HandlePaletteSortListChange();
		void HandlePaletteColourEditChange(CGadgetID id);

		// Button messages
		void HandlePaletteLockedColourButtonChange();
		void HandlePaletteWebSafeColourButtonChange();
		void HandlePaletteTransparentBackgroundButtonChange();
		void HandlePaletteTransparentColourButtonChange();
		void HandlePaletteDeleteColourButtonChnage();
		void HandlePaletteRestoreColourButtonChange();
		void HandlePaletteSystemColoursButtonChnage();

	// The Bitmap Size Tab
	void HandleBitmapSizeTabMsg(DialogMsg* Msg);
		void HandleBitmapSizeDPIChange();
		void HandleBitmapSizeWidthChange();
		void HandleBitmapSizeHeightChange();
		void HandleBitmapSizeAreaToSaveChange(SelectionType type);
		void HandleBitmapSizeAntiAliasingChange(BOOL DoMinimise);
		void HandleBitmapSizePutHTMLChange();
		void HandleBitmapSizeScaleFactorChange();

	void HandleBitmapOptionsTicks(UINT32 tick);

	void HandleBrowserPreviewTabMsg(DialogMsg* Msg);

	void HandleBitmapOptionsTabMsg(DialogMsg* Msg);

	void HandleImageMapTabMsg(DialogMsg* Msg); 

//	void HandleJPEGTabMsg(DialogMsg* Msg);

//	void HandlePNGTabMsg(DialogMsg* Msg);

private:	// Refresh functions to sync the controls with the bitmap export options
	// The Palette Tab
	void RefreshPaletteOptionsTab();				// This function calls all the functions below it
		void RefreshPaletteDitheringList();
		void RefreshPalettePaletteList();
		void RefreshPaletteColourDepthList();
		void RefreshPaletteSortList();
		void RefreshPaletteLinkedControls();		// This function calls all the functions below it
			void RefreshPaletteColourModelList();
			void RefreshPaletteButtonStrip();
			void RefreshPaletteColoursUsedEdit();
			void RefreshPaletteColoursEdit();

	// The Bitmap Size Tab
	void RefreshBitmapSizeTab();
		void RefreshBitmapSizeScaleFactor();

	void RefreshBitmapOptionsTab();

	void RefreshImageMapTab();

//	void RefreshJPEGTab();

//	void RefreshPNGTab();
}; */
#endif // EXCLUDE_FROM_XARALX
#endif

