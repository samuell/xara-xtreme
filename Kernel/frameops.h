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

// The Frame Operations header file

#ifndef INC_FRAMEOPS
#define INC_FRAMEOPS

//#include "dialogop.h"
#include "undoop.h"
#include "layergal.h"

#include "makebmp.h"	// MakeBitmapFilter
#include "impexpop.h"	// OpMenuExport BitmapExportParam
#include "animparams.h"	// AnimPropertiesParam

//#if _DEBUG // removal of these ops from non-debug builds

class PathName;
class Layer;
class BitmapPreviewData;
class PreviewDialog;

//------------------------------------------------------------------------------------------
// OpLayerGalChange related items

/********************************************************************************************

>	class CCAPI OpSelectStartFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/97
	Purpose:	Performs an undoable change of selecting the bottom most frame layer.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_STARTFRAME	_T("GotoStartFrame")

class CCAPI OpSelectStartFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpSelectStartFrame );

public:
	OpSelectStartFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};  

/********************************************************************************************

>	class CCAPI OpSelectEndFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Performs an undoable change of selecting the top most frame layer.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_ENDFRAME		_T("GotoEndFrame")

class CCAPI OpSelectEndFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpSelectEndFrame );

public:
	OpSelectEndFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

/********************************************************************************************

>	class CCAPI OpSelectPrevFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Performs an undoable change of selecting the previous frame (layer).
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_PREVFRAME		_T("GotoPrevFrame")

class CCAPI OpSelectPrevFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpSelectPrevFrame );

public:
	OpSelectPrevFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

/********************************************************************************************

>	class CCAPI OpSelectNextFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Performs an undoable change of selecting the next frame (layer).
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_NEXTFRAME		_T("GotoNextFrame")

class CCAPI OpSelectNextFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpSelectNextFrame );

public:
	OpSelectNextFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

/********************************************************************************************

>	class CCAPI OpCreateNewFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Performs an undoable change of creating a new frame layer.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_NEWFRAME		_T("NewFrame")

class CCAPI OpCreateNewFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpCreateNewFrame );

public:
	OpCreateNewFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

/********************************************************************************************

>	class CCAPI OpCopyFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Performs an undoable change of copying the selected frame layer.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_COPYFRAME		_T("CopyFrame")

class CCAPI OpCopyFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpCopyFrame );

public:
	OpCopyFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

/********************************************************************************************

>	class CCAPI OpDeleteFrame : public OpLayerGalChange

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Purpose:	Performs an undoable change of deleting the selected frame layer.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_DELETEFRAME		_T("DeleteFrame")

class CCAPI OpDeleteFrame : public OpLayerGalChange
{         
	CC_DECLARE_DYNCREATE( OpDeleteFrame );

public:
	OpDeleteFrame() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

//------------------------------------------------------------------------------------------
// Property related items

/********************************************************************************************

>	class CCAPI OpFrameProperties : public Operation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Purpose:	Performs an operation to pop up the frame properties dialog.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_PROPERTIES	_T("FrameProperties")

class CCAPI OpFrameProperties : public Operation
{         
	CC_DECLARE_DYNCREATE( OpFrameProperties );

public:
	OpFrameProperties() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

/********************************************************************************************

>	class CCAPI OpAnimationProperties : public Operation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Purpose:	Performs an operation to pop up the animation properties dialog.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_ANIPROPERTIES	_T("AnimationProperties")

class CCAPI OpAnimationProperties : public Operation
{         
	CC_DECLARE_DYNCREATE( OpAnimationProperties );

public:
	OpAnimationProperties() { } // nothing yet 
	
	static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );
};

//------------------------------------------------------------------------------------------
// Make bitmap copy related items


//------------------------------------------------------------------------------------------
//#endif // _DEBUG removal of these ops from non-debug builds
//------------------------------------------------------------------------------------------

/********************************************************************************************

  >	class GIFAnimationExportParam : public BitmapExportParam

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/97
	Purpose:	Passes infomation about the GIF animation and its bitmap(s) being exported
	SeeAlso:	OpParam

********************************************************************************************/

class GIFAnimationExportParam : public BitmapExportParam
{
CC_DECLARE_DYNAMIC(GIFAnimationExportParam);

public:
	GIFAnimationExportParam();
	GIFAnimationExportParam(UINT32 Count, KernelBitmap** pBitmap);

	~GIFAnimationExportParam();

	// Access functions with helpful checking
	virtual UINT32 GetBitmapCount();
	virtual KernelBitmap* GetBitmap(UINT32 Index);
	virtual BOOL AddBitmap(KernelBitmap * pBitmap);

	// Access functions.
	UINT32	GetRegeneratedBitmapPosition()const				{ return m_RegeneratedBitmapPosition; }
	void	SetRegeneratedBitmapPosition(const UINT32 Pos)	{  m_RegeneratedBitmapPosition = Pos; }

	virtual UINT32 GetLayerCount();
	virtual Layer * GetLayer(UINT32 Index);
	virtual BOOL AddLayer(Layer * pLayer);

	virtual BOOL AddLayerAndBitmap(Layer * pLayer, KernelBitmap * pBitmap);

	AnimPropertiesParam& GetSpreadAnimPropertiesParam();
	void SetSpreadAnimPropertiesParam(const AnimPropertiesParam& Param);

	DWORD			GetAnimLoop() const			{ return m_AnimPropertiesParam.GetAnimLoop();			}
	DWORD			GetGlobalAnimDelay() const	{ return m_AnimPropertiesParam.GetGlobalAnimDelay();	}
	WEB_PALETTE		GetPalette() const			{ return m_AnimPropertiesParam.GetPalette();			}
	PALETTE_COLOURS GetPaletteCols() const		{ return m_AnimPropertiesParam.GetPaletteCols();		}
	DWORD			GetNumColsInPalette() const	{ return m_AnimPropertiesParam.GetNumColsInPalette();	}
	DITHER			GetDither()const			{ return m_AnimPropertiesParam.GetDither();				}
	BOOL 			GetUseSystemCols() const	{ return m_AnimPropertiesParam.GetUseSystemCols();		}
	BOOL 			GetIsBackGroundTransp() const	{ return m_AnimPropertiesParam.GetIsBackGroundTransp();	}

	virtual DocRect GetBoundingRect() const;
	
	DocRect GetAnimationBoundingRect() const { return m_AnimPropertiesParam.GetBoundingRect(); }
	void SetAnimationBoundingRect(const DocRect& BoundingRect) { m_AnimPropertiesParam.SetBoundingRect(BoundingRect); }

protected:
	// This is a copy of the version stored in the spread
	AnimPropertiesParam		m_AnimPropertiesParam; 
	// The following values for a GIF animation will be stored:-
								// Animation loop	
								// Global Delay
								// Dither
								// Palette	
								// Number of Cols In Palette
								// Use System Colours flag

	KernelBitmap** m_pBitmaps;			// The array of bitmaps 
	UINT32 m_BitmapCount;					// The number of bitmaps in the above array
	Layer** m_pLayers;					// The array of layers
	UINT32 m_LayerCount;					// The number of frame layers in the above array
	UINT32 m_RegeneratedBitmapPosition;	// The position of the regenerated bitmap in the array. 	
};

//------------------------------------------------------------------------------------------
//#if _DEBUG // removal of these ops from non-debug builds
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	class GrabFrameFilter : public MakeBitmapFilter

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/97
	Purpose:	Special filter used to convert the selected frame into a bitmap

********************************************************************************************/

class GrabFrameFilter : public MakeBitmapFilter
{
	CC_DECLARE_DYNAMIC(GrabFrameFilter);
	
public:
	GrabFrameFilter();
	~GrabFrameFilter();

	// Function which determines if a filter should export invisible layers or not
	virtual BOOL ExportVisibleLayersOnly(); 

	// Function which determines if a filter should export the selected items or not
	virtual BOOL ExportSelectionOnly(BOOL MaskedRender = FALSE); 

	// Function which returns whether a second rendering pass is required or not
	virtual BOOL IsSecondPassRequired();

	virtual BOOL DoExportBitmaps(Operation* pOp, CCLexFile* pFile, PathName* pPath,
								 BitmapExportParam* pParam, BOOL DontShowFileName = FALSE);

	// Function to generate an optimised palette
	// Overide the baseclass to do GLOBAL OPTIMISED palette
	virtual BOOL GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI, BOOL SnapToBrowserPalette, UINT32 NumColsInPalette, BOOL UsePrimaryCols);

public:
	// Special access functions for this filter
	Layer * GetLayerBeingCaptured() { return m_pLayer; }
	BOOL SetLayerBeingCaptured(Layer * pNewLayer);

	virtual BOOL SetUpClippingRectangleForExport(Spread *pSpread, SelectionType Selection);

	// This is just a public interface on the normal filter's GetExportOptions which is protected
	virtual BOOL SetupExportOptions(BitmapExportOptions* pOptions, Spread * pCurrentSpread);
	// This is just a public interface on the normal filter's CreateExportOptions which is protected
	virtual MakeBitmapExportOptions * MakeExportOptions() { return (MakeBitmapExportOptions*)CreateExportOptions(); }

	virtual MakeBitmapExportOptions * GetCurrentExportOptions() { return m_pMkBmpOptions; }
	virtual MakeBitmapExportOptions * GetCurrentExportOptionsCopy() { return m_pCopyMkBmpOptions; }

	LPLOGPALETTE GetGlobalOptimisedPalette() { return m_pOptimisedPalette; }
	void SetGlobalOptimisedPalette(LPLOGPALETTE pOptimisedPalette);

	// Functions for returning the import and export message ids.
	// Baseclass versions just return the ExportMsgID and ImportMsgID
	// Allow somebody to read the export ID and to override it for say multistage exports
	virtual UINT32 GetExportMsgID();

	// Almost-a-bodge addition by Jonathan.  We need to use the GrabFrameFilter in a
	// different way when we are generating bitmaps to pass to the palette optimiser
	// from when we are actually generating the bitmaps that are used in the animation.
	// (For palette optimisation, we need 32 bit bitmaps and for normal use we need
	// 8 bit bitmaps).  For this system to work, before using the filter to generate
	// a bitmap, call SetGrabMode to set the filter to the way you want it to work.
	enum GRABMODE { GRABMODE_PREVIEW, GRABMODE_FOROUTPUT };
	void SetGrabMode(GRABMODE newmode) { m_GrabMode = newmode; }

protected:

	GRABMODE m_GrabMode;

	
	//
	// Export related functions
	//
	// Just override the ones that are necessary
	//virtual BitmapExportOptions*	CreateExportOptions() const;
	virtual BOOL GetExportOptions(BitmapExportOptions* pOptions);

	virtual KernelBitmap* GetTheBitmap();

private:
	Layer * m_pLayer;

	// This is a bit OILy, so will need to be moved some time
	// As Bitfilt.h has the same problem we will worry about it when it does...
	LPLOGPALETTE m_pOptimisedPalette;
	
	// Take a copy of the export parameters
	MakeBitmapExportOptions* m_pMkBmpOptions;

	MakeBitmapExportOptions* m_pCopyMkBmpOptions;
};

/********************************************************************************************

>	class CCAPI OpGrabFrame : public Operation

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Purpose:	Performs an operation to grab the selected frame layer and preview it.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_GRABFRAME		_T("GrabFrame")

class CCAPI OpGrabFrame : public Operation
{         
	CC_DECLARE_DYNCREATE( OpGrabFrame );

public:
	OpGrabFrame(); 
	~OpGrabFrame(); 
	
	static BOOL		Init();				
	static void		DeInit();
	static OpState	GetState(String_256*, OpDescriptor*);		

	virtual void Do( OpDescriptor * pOpDesc );
	virtual void DoWithParam(OpDescriptor* pOpDesc, OpParam* Param);

protected:
	// Frame grabbing related functions
	virtual BOOL StartGrab();
	virtual KernelBitmap * GrabFrame();
	virtual KernelBitmap * GrabTheFrame();
	virtual BOOL EndGrab();

	// Actions associated with frame grabbing
	virtual BOOL GetFileNameFromUser();
	virtual BOOL PreviewAnimation();

	// This does the actually job in hand, e.g. preview, save, preview in browser
	virtual BOOL SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam);

	// Some useful helper functions
			BOOL SetFullBitmapList(BOOL SetFlagState = FALSE);
			BOOL SetSingleBitmapInList();
			BOOL SetBitmapToListTail();

			BOOL CheckIfSingleBitmap(Layer * pLayer, KernelBitmap ** ppBitmap);
			BOOL CheckAllForSingleBitmaps(Spread * pSpread, GrabFrameFilter * pBitmapFilter);

	virtual BitmapPreviewData * GetBitmapPreviewData() { return m_pBitmapData; }

protected:
	// we must remember the selection and then try and put it back at the end of the op
	virtual BOOL RememberTheSelection();
	virtual BOOL PutBackTheSelection();
	
	// The remembered selection state
	SelectionState *		m_pSelState;

protected:
	// This encapsulates the list of bitmaps and associated layers/frames
	// plus the other relevant GIF animation export details
	GIFAnimationExportParam m_ExportParams;

	GrabFrameFilter *		m_pBitmapFilter;

	Document *				m_pDocument;
	Spread *				m_pSpread;
	Layer *					m_pLayer;
	DocView *				m_pView;

	static BitmapPreviewData *	m_pBitmapData;

	LPLOGPALETTE			m_pBrowserPalette;
};

/********************************************************************************************

>	class CCAPI OpGrabAllFrames : public OpGrabFrame

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Purpose:	Performs an operation to grab all of the frame layers and preview them.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_GRABALLFRAMES	_T("GrabAllFrames")

class CCAPI OpGrabAllFrames : public OpGrabFrame
{         
	CC_DECLARE_DYNCREATE( OpGrabAllFrames );

public:
	OpGrabAllFrames() { } // nothing yet 
	
	//static OpState	GetState(String_256*, OpDescriptor*);		

	void Do( OpDescriptor * pOpDesc );

	static BOOL ms_ForceRefreshOfAllFrames;

// Helper functions
protected:
	void CreateGlobalPalette(DWORD NumColsInPalette, BOOL RegenerateAllFrames);
};

//------------------------------------------------------------------------------------------
// Save related items

/********************************************************************************************

>	class OpSaveAnimatedGIF : public OpGrabAllFrames

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/97
	Purpose:	Pops up a common dialog to allow the user to export an animated GIF from Camelot.
				This is for the special frame related export so we have all the options set up.
	SeeAlso:	OpMenuExport;

********************************************************************************************/

// Operation token for the OpMenuExport class
#define	OPTOKEN_SAVEANIMATEDGIF	_T("SaveAnimatedGIF")

class OpSaveAnimatedGIF : public OpGrabAllFrames
{
	CC_DECLARE_DYNCREATE(OpSaveAnimatedGIF)

public:
	static OpState	GetState(String_256*, OpDescriptor*);		

protected:
	// Actions associated with frame grabbing
	virtual BOOL GetFileNameFromUser();

	// This does the actually job in hand, e.g. preview, save, preview in browser
	virtual BOOL SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam);

	// This checks to see that the file type/extension is ok 
	BOOL EnsureFileType(PathName * pPath);

private:
	PathName m_SavePath;
};

//------------------------------------------------------------------------------------------
// Browser Preview related items

/********************************************************************************************

>	class CCAPI OpBrowserPreview : public OpSaveAnimatedGIF

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Purpose:	Performs an operation to preview the animation in a browser.
	SeeAlso:	-

********************************************************************************************/
#define OPTOKEN_FRAME_BROWSERPREVIEW _T("BrowserPreview")

class CCAPI OpBrowserPreview : public OpSaveAnimatedGIF
{         
	CC_DECLARE_DYNCREATE( OpBrowserPreview );

public:
	static BOOL RemoveTempFile();

protected:
	// Actions associated with frame grabbing
	virtual BOOL GetFileNameFromUser();

	// This does the actually job in hand, e.g. preview, save, preview in browser
	virtual BOOL SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam);

private:
	static PathName m_TempPath;
};

//------------------------------------------------------------------------------------------
//#endif // _DEBUG removal of these ops from non-debug builds

#endif	// INC_FRAMEOPS
