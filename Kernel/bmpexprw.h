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

// A dialog box for previwing bitmaps

#ifndef INC_EXPORTPREVIEWDLG
#define INC_EXPORTPREVIEWDLG

// Find out about the DialogOp Class
#include "dialogop.h"
#include "bmpprefs.h"
#include "monotime.h"
#include "bblwnd.h"
#include "grndrgn.h"
#include "cbmpdata.h"

class KernelBitmap;
class RenderRegion;
class BaseBitmapFilter;
class BitmapExportOptions;

// view tool types
typedef enum {PREVIEW_ZOOM_TOOL, PREVIEW_PUSH_TOOL, PREVIEW_OTHER_TOOL, PREVIEW_COLOUR_SELECTOR_TOOL} PreviewTool;


/********************************************************************************************

>	class BitmapExportPreviewDialog : public DialogOp

	Author:		Stefan_Stoykov (Xara Group Ltd) <camelotdev@xara.com> (based on Neville's code)
	Created:	11/4/97
	Purpose:	A dialog box for previewing bitmaps

********************************************************************************************/

#define OPTOKEN_BITMAPPREVIEWDIALOG _T("BitmapExportPreviewDialog")

class BitmapExportPreviewDialog : public DialogOp
{
	// Give my name in memory dumps
	CC_DECLARE_DYNCREATE(BitmapExportPreviewDialog);

public:
	// Construction etc
	BitmapExportPreviewDialog(CWindowID ParentWnd = NULL);
	~BitmapExportPreviewDialog();

	// allows changing one of the preview bitmaps
	BOOL DoWithExportOptions(BitmapExportOptions *pExportOptions, UINT32 Num);

	// Called when the dialog is to be displayed
	void InitPreviewDialog(BmapPrevDlg *pParentDlg, Filter *pFilter);

	// allows obtaining and changing the currently selected bitmap
	UINT32 GetActiveBitmap() { return m_ActiveBitmap;}
	void ChangeActiveBitmap(UINT32 Active);

	const KernelBitmap * GetActiveKernelBmp() { return BitmapData[m_ActiveBitmap].m_pBitmap; }


	// allows the context menu to obtain the current tool
	PreviewTool GetCurrentTool() { return m_CurrentTool;};

	// for browser preview
	BOOL DoBrowserPreview(BrowserPreviewOptions BrowserOptions);

	BOOL OnExport(BitmapExportOptions * pExportOptions);

	// for the message handling
	virtual	MsgResult Message(Msg* Message);

	// handles the context menu commands
	virtual void DoCommand(StringBase *CommandID);

	void InitialiseBitmapPaletteInformation( BitmapExportOptions* pExportOptions, INT32 Num = 0 );

	void SetFilter( Filter* pFilter )	{ m_pBmpFilter = ( BaseBitmapFilter* )pFilter; }

	void SetNeedBitmapCopied( BOOL State ) { m_bNeedBitmapCopied[ m_ActiveBitmap - 1 ] = State; }

	// Static functions
	// Find out about the state of the dlg
	static	OpState GetState(String_256*, OpDescriptor*);
	static	BOOL Init();

	// Static Vars
	static const UINT32 IDD;
	static const CDlgMode Mode;

protected:
	// render control redraw function
	void RenderControl(ReDrawInfoType* pExtraInfo, UINT32 GadgetID);

	// Useful set up functions
	BOOL InitDialog();
	BOOL SetUpCallBack();

	// pushing
	void DoPush(DocCoord *NewPos);

	// zooming
	void CalcViewRect(double NewZoomFactor, DocCoord *Pos);
	void DoZoom(DocCoord *NewPos);
	double FindNearestZoom(BOOL Up);
	void DrawZoomRect();
	void ZoomToRect();
	void DisplayZoomFactor();

	// cursor display
	void SetCurrentCursor(UINT32 id);
	void DeleteCurrentCursor();

	void DisplayBitmapInfo();

	// bubble and status line text
	void ResetInfo(UINT32 id);
	void DoBubbleHelp();

	// handle menu or button selected commands
	void OnZoomTool();
	void OnPushTool();
	void On1to1();
	void On100Percent();
	void OnColourSelectorTool();

	INT32 GetPaletteIndexOfColour( ReDrawInfoType* param, CGadgetID Gadget );

	void InitFileTypeList();
	void HandleFileTypeListChange( INT32 Index );

	// static pointr to this dialog
	static BitmapExportPreviewDialog * m_pBitmapExportPreviewDialog;
	
	// The render region that we are using for redraws
	GRenderRegion * m_pRender;

	// bitmap data for the two preview bitmaps
	BitmapPreviewData BitmapData[2]; // this stores the ptr to BitmapExportOptions

	KernelBitmap* pOriginalBitmap[2];

	BaseBitmapFilter *m_pBmpFilter; // pointer to the bitmap export filter
	BmapPrevDlg *m_pParentDlg;

	// keep the width and height for the first bitmap only
	MILLIPOINT m_Width;
	MILLIPOINT m_Height;
	double m_Scaling; // required to calculate the width and height for the second bitmap
	
	// display data
	DocRect m_BitmapView; // the current view rectangle
	double m_ZoomFactor;  //the current zoom factor

	UINT32 m_ActiveBitmap; // 1 or 2 - indicates that the first or the second bitmap is selected
	DocCoord m_CurrentPos; // the endos of the zoom rect (changes as the mouse pointer is dragged)
	DocCoord m_StartPos; // the start pos of the zoom rectangle (doesn't change during the operation)
	PreviewTool m_CurrentTool; // zoom or push tool
	BOOL m_Dragging; // are we in drag mode?
	BOOL m_ZoomRectRender; // whether we are to render the zoom rectangle in the redraw() function
	INT32 m_CurID; // the id of the last set cursor
	BubbleHelpWnd *m_pBubbleWnd; // the bubble help window
	UINT32 m_LastCursorOverControlID; //the id of the last control, the cursor was over
	MonotonicTime BubbleTimer; // the timer for the bubble help
	BOOL m_BubbleHelpPending; // is it waiting for the timer to signal displaying of bubble help
	BOOL m_StatusTextChanged; // there is a need to update the status line
	BOOL m_LastAdjustState; //1 - Adjust pressed, 0 - Adjust not pressed
	BOOL m_MouseOperationStarted;  //the mouse button was pressed (but not released yet)
	CWindowID m_PreviousWithFocus;
	//ExportFileType m_CurrentFileType[2];	//  The current file types selected
	BOOL m_bWantCursorRedrawn;
	BOOL m_bNeedBitmapCopied[2];
	INT32 m_TopLevelCursor; // The first cursor the dialog puts on the cursor stack
};



#endif  // INC_EXPORTPREVIEWDLG


