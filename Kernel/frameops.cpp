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

// The Frame Operations source file

/*
*/

#include "camtypes.h"
#include "frameops.h"

//#include "prevwres.h"		// _R(IDS_DISPLAY_FRAME_GALLERY) _R(IDD_ANIMATIONBAR) ...
#include "sgframe.h"		// FrameSGallery
#include "spread.h"			// Spread
//#include "simon.h"			// _R(IDS_NO_DOC)
#include "app.h"			// GetApplication()
#include "fixmem.h"			// CCMalloc
#include "prevwdlg.h"		// OPTOKEN_PREVIEWDIALOG
#include "grndbmp.h"		// GRenderBitmap
#include "filedlgs.h"		// ExportFileDialog
//#include "tim.h"			// _R(IDT_CANT_FIND_FILTER)
#include "oilfltrs.h"
//#include "nev.h"			// _R(IDN_USER_CANCELLED)
#include "layermsg.h"		// layer messages REDRAW_LAYER

//#include "accures.h"		// _R(IDN_FILTEREXT_GIF)

#include "docview.h"

#include "nodebmp.h"		// NodeBitmap
#include "insertnd.h"		// InsertionNode
#include "aprps.h"

// browser preview
#include "fileutil.h"		// FileUtil::GetTemporaryPathName()
#include "cbmpdata.h"		// CBitmapData

#include "palman.h"			// MakePaletteBrowserCompatible
#include "wbitmap.h"		// WinBitmap

#include "xshelpid.h"	//For the help ID

#include "gpalopt.h" // for the palette optimiser
#include "mrhbits.h"
#include "page.h"
#include "quality.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(OpSelectStartFrame,	OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpSelectEndFrame,	OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpSelectPrevFrame,	OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpSelectNextFrame,	OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpCreateNewFrame,	OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpCopyFrame,			OpLayerGalChange);
CC_IMPLEMENT_DYNCREATE(OpDeleteFrame,		OpLayerGalChange);

CC_IMPLEMENT_DYNCREATE(OpFrameProperties, Operation);
CC_IMPLEMENT_DYNCREATE(OpAnimationProperties, Operation);

CC_IMPLEMENT_DYNAMIC(GIFAnimationExportParam,	BitmapExportParam);

CC_IMPLEMENT_DYNCREATE(OpGrabFrame, Operation);
CC_IMPLEMENT_DYNCREATE(OpGrabAllFrames, OpGrabFrame);
CC_IMPLEMENT_DYNAMIC(GrabFrameFilter, MakeBitmapFilter);
CC_IMPLEMENT_DYNCREATE(OpSaveAnimatedGIF, OpGrabAllFrames);
CC_IMPLEMENT_DYNCREATE(OpBrowserPreview, OpSaveAnimatedGIF);

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
// This line mustn't go before any CC_IMPLEMENT_... macros
#define new CAM_DEBUG_NEW

BOOL OpGrabAllFrames::ms_ForceRefreshOfAllFrames = FALSE;

//------------------------------------------------------------------------------------------
// Methods for the OpSelectStartFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpSelectStartFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpSelectStartFrame operation
	Purpose:	For finding the OpSelectStartFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpSelectStartFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	OpState OpSt;  

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		Layer * pPrevLayer = pLayer->FindPrevLayer();
		// The previous layer should really be a frame layer but we cannot guarantee that but
		// it mustn't be a guide, a page background or a background layer.
		while (	pPrevLayer && !pPrevLayer->IsPseudoFrame())
		{
			pPrevLayer = pPrevLayer->FindPrevLayer();
		}

		if (pLayer && pPrevLayer)
		{
			// Yes, so we are allowed to move to the next one
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}

 	return OpSt;   
}

/********************************************************************************************

>	void OpSelectStartFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to select the bottom most animation frame (i.e. layer).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSelectStartFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// yes, so find the first layer and try to move to it
		Layer * pLayer = pSpread->FindFirstFrameLayer();
		if (pLayer)
		{
			OpLayerGalParam Param(FRAME_SELECT, pSpread);
			Param.pLayer = pLayer;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpSelectStartFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpSelectEndFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpSelectEndFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpSelectEndFrame operation
	Purpose:	For finding the OpSelectEndFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpSelectEndFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	OpState OpSt;  

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		// If there is an active layer and one following then we are not greyed
		Layer * pNextLayer = pLayer->FindNextLayer();
		// The next layer should really be a frame layer but we cannot guarantee that but
		// it mustn't be a guide, a page background or a background layer.
		while (	pNextLayer && !pNextLayer->IsPseudoFrame())
		{
			pNextLayer = pNextLayer->FindNextLayer();
		}

		if (pLayer && pNextLayer)
		{
			// Yes, so we are allowed to move to the previous one
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}

 	return OpSt;   
}

/********************************************************************************************

>	void OpSelectEndFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to select the bottom most animation frame (i.e. layer).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSelectEndFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// yes, so find the last layer and try to move to it
		Layer * pLayer = pSpread->FindLastFrameLayer();
		if (pLayer)
		{
			OpLayerGalParam Param(FRAME_SELECT, pSpread);
			Param.pLayer = pLayer;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpSelectEndFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpSelectPrevFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpSelectPrevFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpSelectPrevFrame operation
	Purpose:	For finding the OpSelectPrevFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpSelectPrevFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;  

#ifdef WEBSTER

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		// If there is an active layer and one preceeding then we are not greyed
		Layer * pPrevLayer = pLayer->FindPrevLayer();
		// The previous layer should really be a frame layer but we cannot guarantee that but
		// it mustn't be a guide, a page background or a background layer.
		while (	pPrevLayer && !pPrevLayer->IsPseudoFrame())
		{
			pPrevLayer = pPrevLayer->FindPrevLayer();
		}
	 
		if (pLayer && pPrevLayer)
		{
			// Yes, so we are allowed to move to the previous one
			OpSt.Greyed = FALSE;
	 	}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}

#else
	// Are we in frame mode.
	BOOL FrameMode = IsFrameMode();

	if(FrameMode)
	{
		// Is there a selected spread?
		Spread * pSpread = Document::GetSelectedSpread();
		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();
			// If there is an active layer and one preceeding then we are not greyed
			Layer * pPrevLayer = pLayer->FindPrevLayer();
			// The previous layer should really be a frame layer but we cannot guarantee that but
			// it mustn't be a guide, a page background or a background layer.
			while (	pPrevLayer && !pPrevLayer->IsPseudoFrame())
			{
				pPrevLayer = pPrevLayer->FindPrevLayer();
			}

			if (pLayer && pPrevLayer)
			{
				// Yes, so we are allowed to move to the previous one
				OpSt.Greyed = FALSE;
			}
			else
			{
				// No, so we are greyed
				OpSt.Greyed = TRUE;
			}
		}
		else
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// The document is layer based, so we are greyed.
		OpSt.Greyed = TRUE;
	}
#endif

 	return OpSt;   
}

/********************************************************************************************

>	void OpSelectPrevFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to select the previous animation frame (i.e. layer).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSelectPrevFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// yes, so find the last layer and try to move to it
		Layer * pLayer = pSpread->FindActiveLayer();
		Layer * pPrevLayer = NULL;
		if (pLayer)
			pPrevLayer = pLayer->FindPrevFrameLayer();
		if (pPrevLayer)
		{
			OpLayerGalParam Param(FRAME_SELECT, pSpread);
			Param.pLayer = pPrevLayer;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpSelectPrevFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpSelectNextFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpSelectNextFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpSelectNextFrame operation
	Purpose:	For finding the OpSelectNextFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpSelectNextFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.

	OpState OpSt;  

#ifdef WEBSTER

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		// If there is an active layer and one following then we are not greyed
		Layer * pNextLayer = pLayer->FindNextLayer();
		// The next layer should really be a frame layer but we cannot guarantee that but
		// it mustn't be a guide, a page background or a background layer.
		while (	pNextLayer && !pNextLayer->IsPseudoFrame())
		{
			pNextLayer = pNextLayer->FindNextLayer();
		}
	 
		if (pLayer && pNextLayer)
		{
			// Yes, so we are allowed to move to the previous one
			OpSt.Greyed = FALSE;
	 	}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}

#else
	// Are we in frame mode.
	BOOL FrameMode = IsFrameMode();

	if(FrameMode)
	{
	 	// Is there a selected spread?
		Spread * pSpread = Document::GetSelectedSpread();
		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();
			// If there is an active layer and one following then we are not greyed
			Layer * pNextLayer = pLayer->FindNextLayer();
			// The next layer should really be a frame layer but we cannot guarantee that but
			// it mustn't be a guide, a page background or a background layer.
			while (	pNextLayer && !pNextLayer->IsPseudoFrame())
			{
				pNextLayer = pNextLayer->FindNextLayer();
			}

			if (pLayer && pNextLayer)
			{
				// Yes, so we are allowed to move to the previous one
				OpSt.Greyed = FALSE;
			}
			else
			{
				// No, so we are greyed
				OpSt.Greyed = TRUE;
			}
		}
		else
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
			// The document is layer based, so we are greyed.
			OpSt.Greyed = TRUE;
	}
#endif

 	return OpSt;   
}

/********************************************************************************************

>	void OpSelectNextFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to select the next animation frame (i.e. layer).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpSelectNextFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// yes, so find the next layer and try to move to it
		Layer * pLayer = pSpread->FindActiveLayer();
		Layer * pNextLayer = NULL;
		if (pLayer)
			pNextLayer = pLayer->FindNextFrameLayer();
		if (pNextLayer)
		{
			OpLayerGalParam Param(FRAME_SELECT, pSpread);
			Param.pLayer = pNextLayer;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpSelectNextFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpCreateNewFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpCreateNewFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpCreateNewFrame operation
	Purpose:	For finding the OpCreateNewFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpCreateNewFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;  

#ifdef WEBSTER
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Yes, so we are allowed to move to the previous one
		OpSt.Greyed = FALSE;
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}
#else
	// Are we in frame mode.
	BOOL FrameMode = IsFrameMode();

	if(FrameMode)
	{
		// Is there a selected spread?
		Spread * pSpread = Document::GetSelectedSpread();
		if (pSpread)
		{
			// Yes, so we are allowed to move to the previous one
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// The document is layer based, so we are greyed.
		OpSt.Greyed = TRUE;
	}
#endif

 	return OpSt;   
}

/********************************************************************************************

>	void OpCreateNewFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to create a new animation frame (i.e. layer).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpCreateNewFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// Use the spread as the context node
		OpLayerGalParam Param(FRAME_NEW, pSpread);
		Param.pLayer		= NULL;
		Param.pContextNode  = pSpread;
		Param.AttDir		= LASTCHILD;
		// Set up the name that is going to be applied to the frame
		String_256 NewName = FrameSGallery::CreateUniqueLayerID(pSpread);
		//String_256 NewName(_R(IDS_DEFAULTFRAMENAME));
		Param.Status.StringLayerID = NewName;
		DoWithParam(pOpDesc, (OpParam*)&Param);
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpCreateNewFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpCopyFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpCopyFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpCopyFrame operation
	Purpose:	For finding the OpCopyFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpCopyFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;  

#ifdef WEBSTER
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		// If there is an active layer then we are not greyed
		if (pLayer)
		{
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}
#else
	// Are we in frame mode.
	BOOL FrameMode = IsFrameMode();

	if(FrameMode)
	{
		// Is there a selected spread?
		Spread * pSpread = Document::GetSelectedSpread();
		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();
			// If there is an active layer then we are not greyed
			if (pLayer)
			{
				OpSt.Greyed = FALSE;
			}
			else
			{
				// No, so we are greyed
				OpSt.Greyed = TRUE;
			}
		}
		else
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// The document is layer based, so we are greyed.
		OpSt.Greyed = TRUE;
	}
#endif

 	return OpSt;   
}

/********************************************************************************************

>	void OpCopyFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to create a new frame by copying the present one.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpCopyFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// yes, so check that there is a presently active layer and try to copy it
		// Use the spread as the context node
		Layer * pLayer = pSpread->FindActiveLayer();
		if (pLayer)
		{
			OpLayerGalParam Param(FRAME_COPY, pSpread);
			Param.pLayer = pLayer;
			Param.pContextNode  = pSpread;
			Param.AttDir		= LASTCHILD;
			// Set up the name that is going to be applied to the frame
			String_256 NewName = FrameSGallery::CreateUniqueLayerID(pSpread);
			//String_256 NewName(_R(IDS_DEFAULTFRAMENAME));
			Param.Status.StringLayerID = NewName;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpCopyFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpDeleteFrame
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpDeleteFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpDeleteFrame operation
	Purpose:	For finding the OpDeleteFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDeleteFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;  

#ifdef WEBSTER
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();

		// Get ptr's to the next and previous frame layers.
		Layer* pNextLayer = pLayer->FindNextFrameLayer();
		Layer* pPrevLayer = pLayer->FindPrevFrameLayer();

		// If there is an active layer and a previous/next layer then we are not greyed.
		if ( pLayer && (pPrevLayer || pNextLayer) )
		{
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}
#else
	// Are we in frame mode.
	BOOL FrameMode = IsFrameMode();

	if(FrameMode)
	{
		// Is there a selected spread?
		Spread * pSpread = Document::GetSelectedSpread();
		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();

			// Get ptr's to the next and previous frame layers.
			Layer* pNextLayer = pLayer->FindNextFrameLayer();
			Layer* pPrevLayer = pLayer->FindPrevFrameLayer();

			// If there is an active layer and a previous/next layer then we are not greyed.
			if (pLayer && (pPrevLayer || pNextLayer) )
			{
				OpSt.Greyed = FALSE;
			}
			else
			{
				// No, so we are greyed
				OpSt.Greyed = TRUE;
			}
		}
		else
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// We are in layer mode, so we are greyed.
		OpSt.Greyed = TRUE;
	}
#endif

 	return OpSt;   
}

/********************************************************************************************

>	void OpDeleteFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to create a new frame by copying the present one.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeleteFrame::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		// yes, so check that there is a presently active layer and try to delete it
		Layer * pLayer = pSpread->FindActiveLayer();
		if (pLayer)
		{
			OpLayerGalParam Param(FRAME_DELETE, pSpread);
			Param.pLayer = pLayer;
			DoWithParam(pOpDesc, (OpParam*)&Param);
		}
	}

	// End the operation, but we are calling DoWithParam() which we will assume does it for us.
	//End();

	return;
}

/********************************************************************************************
// End of OpDeleteFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpFrameProperties
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpFrameProperties::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpFrameProperties operation
	Purpose:	For finding the OpFrameProperties's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpFrameProperties::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;  

#ifdef WEBSTER
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		// If there is an active layer and one following then we are not greyed
		if (pLayer)
		{
			// Yes, so we are allowed to move to the previous one
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}
#else
	
	BOOL FrameMode = FALSE;	// Flag to determine the document mode.

	// Is there a selected spread?
	Spread* pSpread = Document::GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			FrameMode = TRUE;
	}
	
	if(FrameMode)
	{
		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();
			// If there is an active layer and one following then we are not greyed
			if (pLayer)
				// Yes, so we are allowed to move to the previous one
				OpSt.Greyed = FALSE;
			else
				// No, so we are greyed
				OpSt.Greyed = TRUE;
		}
		else
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
	}
	else
	{
		// The document is layer based, so we are greyed.
		OpSt.Greyed = TRUE;
	}
#endif
	
	return OpSt;   
}

/********************************************************************************************

>	void OpFrameProperties::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to set some new animation properties.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpFrameProperties::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFANIMPROPERTYTABS);

		if (pOpDesc != NULL)
		{
			// Select the "Frame properties tab".
			GIFAnimationPropertyTabsDlg::SetPageToOpen(GIFAnimationPropertyTabs::FramePropertiesTabNumber);

			String_256 Str;
			OpState State = pOpDesc->GetOpsState(&Str);
			if (!State.Greyed)
				pOpDesc->Invoke();
			else
			{
				GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
				if (pTabHandler)
				{
					DialogTabOp* pDlg =  pTabHandler->GetTabbedDlg();
					if (pDlg != NULL)
						pDlg->Open();
				}
			}
		}
	}

	// End the operation
	End();

	return;
}

/********************************************************************************************
// End of OpFrameProperties
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpAnimationProperties
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	OpState	OpAnimationProperties::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpAnimationProperties operation
	Purpose:	For finding the OpAnimationProperties's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpAnimationProperties::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;  

#ifdef WBSTER
		// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		// If there is an active layer and one following then we are not greyed
		if (pLayer)
		{
			// Yes, so we are allowed to move to the previous one
			OpSt.Greyed = FALSE;
		}
		else
		{
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;
	}	
#else
	
	BOOL FrameMode = FALSE;		// Flag to determine the document mode.

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			FrameMode = TRUE;
	}
	
	if(FrameMode)
	{
		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();

			// If there is an active layer and one following, then we are not greyed.
			if (pLayer)
			{
				// Yes, so we are allowed to move to the previous one
				OpSt.Greyed = FALSE;
			}
			else
			{
				// No, so we are greyed
				OpSt.Greyed = TRUE;
			}
		}
		else
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;
		}	
	}
	else
	{
		// The document is layer based, so we are greyed.
		OpSt.Greyed = TRUE;
		*pUIDescription = String_256 (_R(IDS_NO_FRAMES));
	}
#endif

	return OpSt;   
}

/********************************************************************************************

>	void OpAnimationProperties::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to set some new animation properties.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpAnimationProperties::Do( OpDescriptor * pOpDesc )
{
	// Just find the background layer and then ask our baseclass to delete it for us
	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// Check that all visible layers are actually frame layers.
		FrameSGallery::EnsureFrameLayerIntegrity(pSpread);

		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GIFANIMPROPERTYTABS);

		if (pOpDesc != NULL)
		{
			// Select the "Animation properties tab" on start up.
			GIFAnimationPropertyTabsDlg::SetPageToOpen(GIFAnimationPropertyTabs::AnimationColoursTabNumber);

			String_256 Str;
			OpState State = pOpDesc->GetOpsState(&Str);
			if (!State.Greyed)
				pOpDesc->Invoke();
			else
			{
				GIFAnimationPropertyTabs * pTabHandler = GIFAnimationPropertyTabsDlg::GetGIFAnimationPropertiesTabs();
				if (pTabHandler)
				{
					DialogTabOp* pDlg =  pTabHandler->GetTabbedDlg();
					if (pDlg != NULL)
						pDlg->Open();
				}
			}
		}
	}
	
	// End the operation
	End();

	return;
}

/********************************************************************************************
// End of OpAnimationProperties
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpGrabFrame
//------------------------------------------------------------------------------------------

BitmapPreviewData * OpGrabFrame::m_pBitmapData = NULL;

/********************************************************************************************

>	BOOL OpGrabFrame::Init()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/97
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpGrabFrame initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.

********************************************************************************************/

BOOL OpGrabFrame::Init()
{
	BOOL ok = FALSE;
	 
	// start of non-undoable operations
	ok = RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_FRAMEPROPERTIES),			// String resource ID
								CC_RUNTIME_CLASS(OpFrameProperties),	// Runtime class
	 							OPTOKEN_FRAME_PROPERTIES,		// Token string
	 							OpFrameProperties::GetState,			// GetState function
	 							0,								// help ID
								_R(IDBBL_FRAMEPROPERTIES),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_FRAMEPROPERTIES),		// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
	if(ok)
		ok = RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_ANIMATIONPROPERTIES),		// String resource ID
								CC_RUNTIME_CLASS(OpAnimationProperties),	// Runtime class
	 							OPTOKEN_FRAME_ANIPROPERTIES,	// Token string
	 							OpAnimationProperties::GetState,			// GetState function
	 							0,								// help ID
								_R(IDBBL_ANIMATIONPROPERTIES),		// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_ANIMATIONPROPERTIES),	// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
	 if(ok)
	 	ok = RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_GRABFRAME),					// String resource ID
								CC_RUNTIME_CLASS(OpGrabFrame),	// Runtime class
	 							OPTOKEN_FRAME_GRABFRAME,		// Token string
	 							OpGrabFrame::GetState,			// GetState function
	 							_R(IDH_Command_Preview_All_Frames),	// help ID
								_R(IDBBL_GRABFRAME),				// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_GRABFRAME),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
	 if(ok)
		ok = RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_GRABALLFRAMES),			// String resource ID
								CC_RUNTIME_CLASS(OpGrabAllFrames),	// Runtime class
	 							OPTOKEN_FRAME_GRABALLFRAMES,	// Token string
	 							OpGrabAllFrames::GetState,	// GetState function
	 							_R(IDH_Command_Preview_All_Frames),	// help ID
								_R(IDBBL_GRABALLFRAMES),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_GRABALLFRAMES),		// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
	if(ok)
		ok = RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_FRAME_SAVEANIMATEDGIF),		// String resource ID
								CC_RUNTIME_CLASS(OpSaveAnimatedGIF),	// Runtime class
	 							OPTOKEN_SAVEANIMATEDGIF,		// Token string
	 							OpSaveAnimatedGIF::GetState,			// GetState function
	 							0,								// help ID
								_R(IDBBL_SAVEANIMATEDGIF),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_SAVEANIMATEDGIF),		// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
	if(ok)
		ok = RegisterOpDescriptor(
								0,								// Tool ID 
	 							_R(IDS_BROWSERPREVIEW),				// String resource ID
								CC_RUNTIME_CLASS(OpBrowserPreview),	// Runtime class
	 							OPTOKEN_FRAME_BROWSERPREVIEW,	// Token string
	 							OpBrowserPreview::GetState,			// GetState function
	 							_R(IDH_Command_Preview_Animation_in_Browser), // help ID
								_R(IDBBL_BROWSERPREVIEW),			// bubble help
								_R(IDD_ANIMATIONBAR),				// resource ID
								_R(IDC_FRAME_GOTOBROWSER),			// control ID
								SYSTEMBAR_ANIMATION,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								NULL,							// No vertical counterpart
								NULL,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
			// end of non-undoable operations

			if(ok)
				ok = PreviewInBrowserTab::Declare();
			
			return ok;
}               

/********************************************************************************************

>	static void OpGrabFrame::DeInit(void)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/97
	Purpose:	De-Initialises OpGrabFrame

********************************************************************************************/

void OpGrabFrame::DeInit(void)
{
	// Deinit our static preview bitmap data pointer if allocated
	// This should clean up any temp files that we have used
	if (m_pBitmapData != NULL)
	{
		delete m_pBitmapData;
		m_pBitmapData = NULL;
	}

	// Give the browser preview chance to clean up its temp file useage
	OpBrowserPreview::RemoveTempFile();
}

/********************************************************************************************

>	OpState	OpGrabFrame::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpGrabFrame operation
	Purpose:	For finding the OpGrabFrame's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpGrabFrame::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{

	//The frame movement controls on the GIF animation bar now work on frames only.
	OpState OpSt;

#ifdef WEBSTER
	String_256 DisableReason;

	// Is there a selected spread?
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread)
	{
		// yes, so check if there is an active layer
		Layer * pLayer = pSpread->FindActiveLayer();
		if (pLayer)
		{
			// Ensure that we have some visible objects on the specified layer to render
			// Could use a range but easier just to check if there are visible objects on the page
			DocRect VisibleRect = pSpread->GetPageVisibleBounds();
			BOOL Empty = VisibleRect.IsEmpty();
			// If this frame has nothing visible on it then check if there are renderable objects
			// on any other frames in the animation.
			if (Empty)
			{
				// Cannot assume that all the frame layers are actually frame layers so
				// go through all those that might be frame layers.
				Layer * pFrame = pSpread->FindFirstLayer();
				while (pFrame && Empty)
				{
					if (pFrame->IsPseudoFrame() && pFrame->HasLayerGotRenderableChildren())
						Empty = FALSE;
						pFrame = pFrame->FindNextLayer();
				}
			}
			if (pOpDesc && pOpDesc->Token != String(OPTOKEN_FRAME_GRABFRAME) && !Empty)
			{
				// Cannot assume that all the frame layers are actually frame layers so
				// go through all those that might be frame layers.
				Layer * pFrame = pSpread->FindFirstLayer();
				BOOL AllHidden = TRUE;
				while (pFrame && AllHidden)
				{
					if (pFrame->IsPseudoFrame() && !pFrame->IsHiddenFrame())
						AllHidden = FALSE;
						pFrame = pFrame->FindNextLayer();
				}
				if (AllHidden)
				{
					OpSt.Greyed = TRUE; 
					// Load reason why operation is disabled
					DisableReason = String_256(_R(IDS_ALLFRAMESAREHIDDEN));
					if (pUIDescription)
	   					*pUIDescription = DisableReason;
				}
				else
				{
					// Yes, so we are allowed to grab the frame or all the frames
					OpSt.Greyed = FALSE;
				}
			}
			else if (pOpDesc && pOpDesc->Token == String(OPTOKEN_FRAME_GRABFRAME) && pLayer->IsHiddenFrame())
			{
				// If we are the grab frame op then check that the current layer is not
				// hidden. If so then disallow
				OpSt.Greyed = TRUE; 
				// Load reason why operation is disabled
				DisableReason = String_256(_R(IDS_FRAMEISHIDDEN));
				if (pUIDescription)
	   				*pUIDescription = DisableReason;    
			}
			else if (!Empty)
			{
				// Yes, so we are allowed to grab the frame or all the frames
				OpSt.Greyed = FALSE;
			}
			else
			{
				OpSt.Greyed = TRUE; 
				// Load reason why operation is disabled
				DisableReason = String_256(_R(IDS_NO_VISIBLE_OBJECTS));
				if (pUIDescription)
	   				*pUIDescription = DisableReason;	        
			}			
		}
		else
		{ 
			// No, so we are greyed
			OpSt.Greyed = TRUE;
		}
	}
	else 
	{
		// No selected spread, so we are greyed
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		DisableReason = String_256(_R(IDS_NO_DOC));
		if (pUIDescription)
		   	*pUIDescription = DisableReason;	     
	}

#else
	// Flag to determine the document mode.
	BOOL FrameMode = FALSE;

	// Get a ptr to the selected spread
	Spread* pSpread = Document::GetSelectedSpread();

	// Ensure a valid spread ptr.
	if(pSpread)
	{
		// Are there any frame layers?
		Layer* pFrameLayer = pSpread->FindFirstFrameLayer();	

		//If a frame layer exists, then this is an animation doc.
		if (pFrameLayer)
			FrameMode = TRUE;
	}

	if(FrameMode)
	{
		String_256 DisableReason;

		if (pSpread)
		{
			// yes, so check if there is an active layer
			Layer * pLayer = pSpread->FindActiveLayer();

			if (pLayer)
			{
				// Ensure that we have some visible objects on the specified layer to render
				// Could use a range but easier just to check if there are visible objects on the page
				DocRect VisibleRect = pSpread->GetPageVisibleBounds();
				BOOL Empty = VisibleRect.IsEmpty();
				// If this frame has nothing visible on it then check if there are renderable objects
				// on any other frames in the animation.
				if (Empty)
				{
					// Cannot assume that all the frame layers are actually frame layers so
					// go through all those that might be frame layers.
					Layer * pFrame = pSpread->FindFirstLayer();
					while (pFrame && Empty)
					{
						if (pFrame->IsPseudoFrame() && pFrame->HasLayerGotRenderableChildren())
							Empty = FALSE;
						
						pFrame = pFrame->FindNextLayer();
					}
				}

				if (pOpDesc && pOpDesc->Token != String(OPTOKEN_FRAME_GRABFRAME) && !Empty)
				{
					// Cannot assume that all the frame layers are actually frame layers so
					// go through all those that might be frame layers.
					Layer * pFrame = pSpread->FindFirstLayer();
					BOOL AllHidden = TRUE;
					while (pFrame && AllHidden)
					{
						if (pFrame->IsPseudoFrame() && !pFrame->IsHiddenFrame())
							AllHidden = FALSE;
						
						pFrame = pFrame->FindNextLayer();
					}
					
					if (AllHidden)
					{
						OpSt.Greyed = TRUE; 
						// Load reason why operation is disabled
						DisableReason = String_256(_R(IDS_ALLFRAMESAREHIDDEN));
						if (pUIDescription)
	   						*pUIDescription = DisableReason;
					}
					else
					{
						// Yes, so we are allowed to grab the frame or all the frames
						OpSt.Greyed = FALSE;
					}
				}
				else if (pOpDesc && pOpDesc->Token == String(OPTOKEN_FRAME_GRABFRAME) && pLayer->IsHiddenFrame())
				{
					// If we are the grab frame op then check that the current layer is not
					// hidden. If so then disallow
					OpSt.Greyed = TRUE; 
					// Load reason why operation is disabled
					DisableReason = String_256(_R(IDS_FRAMEISHIDDEN));
					if (pUIDescription)
	   					*pUIDescription = DisableReason;    
				}
				else if (!Empty)
				{
					// Yes, so we are allowed to grab the frame or all the frames
					OpSt.Greyed = FALSE;
				}
				else
				{
					OpSt.Greyed = TRUE; 
					// Load reason why operation is disabled
					DisableReason = String_256(_R(IDS_NO_VISIBLE_OBJECTS));
					if (pUIDescription)
	   					*pUIDescription = DisableReason;	        
				}			
			}
			else
			{ 
				// No, so we are greyed
				OpSt.Greyed = TRUE;
			}
		}
		else 
		{
			// No selected spread, so we are greyed
			OpSt.Greyed = TRUE;

			// Load reason why operation is disabled
			DisableReason = String_256(_R(IDS_NO_DOC));
			if (pUIDescription)
		   		*pUIDescription = DisableReason;	     
		}
	}
	else
	{
		// The document is layer based, so we are greyed.
		OpSt.Greyed = TRUE;
		*pUIDescription = String_256 (_R(IDS_NO_FRAMES));
	}
#endif

 	return OpSt;   
}

/********************************************************************************************

>	OpGrabFrame::OpGrabFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Purpose:	Constructor

********************************************************************************************/

OpGrabFrame::OpGrabFrame()
{
	m_pDocument		= NULL;
	m_pSpread		= NULL;
	m_pLayer		= NULL;
	m_pView			= NULL;

	m_pBitmapFilter = NULL;
	m_pSelState		= NULL;

	//m_pBitmapData	= NULL;

	m_pBrowserPalette	= NULL;
}

/********************************************************************************************

>	OpGrabFrame::~OpGrabFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Purpose:	Destructor

********************************************************************************************/

OpGrabFrame::~OpGrabFrame()
{
	if (m_pBitmapFilter != NULL)
	{
		delete m_pBitmapFilter;
		m_pBitmapFilter = NULL;
	}

	if (m_pSelState != NULL)
	{
		delete m_pSelState;
		m_pSelState = NULL;
	}

	/* if (m_pBitmapData != NULL)
	{
		delete m_pBitmapData;
		m_pBitmapData = NULL;
	} */

	if (m_pBrowserPalette != NULL)
	{
		CCFree(m_pBrowserPalette);
		m_pBrowserPalette = NULL;
	}
}

/************************************************************************************************************

>	void OpGrabFrame::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to grab the selected animation frame (i.e. layer).

	This Op can be invoked under a number of circumstances:-
	 1. When the preview dialog is open. 
		  Dissplay the bitmap corresponding to the selected frame layer.
		  If the selected frame layer is not in the Preview list (i.e has not been generated)	
		  then generate the bitmap and add it to the list of bitmaps which be will included in the Preview.
	 2. When the preview dialog is closed.
		  In this case we need to generate the bitmap corresponding to the selected Frame layer.
	
****************************************************************************************************************/

void OpGrabFrame::Do( OpDescriptor * pOpDesc )
{
	DoWithParam(pOpDesc, NULL);
	return;
}

/********************************************************************************************

>	void OpGrabFrame::DoWithParam(OpDescriptor * pOpDesc, Param* pParam)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com> (From Neville code.)
	Created:	21/05/97
	Inputs:		OpDescriptor (unused)
				pParam	- NULL		- means grab the active frame
						- Param1 = pLayer	- means grab the specified layer
	Outputs:	-
	Returns:	-
	Purpose:	Regenerate the bitmap associated with the active frame layer.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
void OpGrabFrame::DoWithParam(OpDescriptor * pOpDesc, OpParam* pParam)
{
//	ERROR3IF(pParam == NULL, "OpGrabFrame::DoWithParam - NULL Param passed in");
	
	// Get useful data and set up items before starting the grab process
	if (!StartGrab())
	{
		// We failed to grab the frame
		FailAndExecute();
		End();
		return; 
	} 

	// If a parameter has been supplied then this should be a layer pointer, which we
	// should then use to grab the frame
	// Otherwise, we assume we must grab the active frame (StartGrab sets m_pLayer to the active layer)
	if (pParam != NULL)
	{
		// recover the layer pointer that has been specified
		Layer* pLayer = (Layer *) pParam->Param1;
		if (pLayer)
		{
			ERROR3IF(!pLayer->IS_KIND_OF(Layer), "OpParam passed is not a Layer");
			
			// Note the layer we are expected to grab in our class variable
			m_pLayer = pLayer;

			// Set up the visibility of the other layers using this layer
			// as the new temporary and acting active layer
			FrameSGallery::FixOtherLayersFromActive(m_pLayer);
		}
	}

	// Are we trying to grab a hidden frame?
	if (m_pLayer->IsHiddenFrame())
	{
		// yes, tell the user
		InformError(_R(IDS_FRAMEISHIDDEN));
		//ERROR3IF(m_pLayer->IsHiddenFrame(),"OpGrabFrame::DoWithParam Frame is hidden");
		// and then fail
		FailAndExecute();
		End();
		return; 
	}

	// If required, allow the op to get a filename from the user.
	// This also allows them access to the options via the button on the save dialog box.
	// This is why we must do this first rather than after we have generated the frames.
	if (!GetFileNameFromUser())
	{
		// We failed to grab the frame
		FailAndExecute();
		End();
		return; 
	} 

	// Get our static pointer to the Preview Dialog.
	PreviewDialog* pPreviewDlg = PreviewDialog::GetPreviewDialog();
	// If the preview dialog is already open, find the selected frame layer.
	if (pPreviewDlg)
		pPreviewDlg->StopPreviewing();
	
	// Go and do the work to grab the frame
	KernelBitmap* pBitmap = GrabFrame();
	if (pBitmap == NULL)
	{
		// We failed to grab the frame.
		// Try and put back the selection that we recorded at the start
		PutBackTheSelection();
		FailAndExecute();
		End();
		return; 
	} 

	// If we have grabbed the non-active layer then make sure the active layer
	// is set up as current
	Layer *pActiveLayer = m_pSpread->FindActiveLayer();
	if (pActiveLayer && m_pLayer != pActiveLayer)
		FrameSGallery::FixOtherLayersFromActive(pActiveLayer);

	// Clean up after the grabbing process.
	if (!EndGrab())
	{
		// We failed to clean up after grabbing the frame.
		FailAndExecute();
		End();
		return; 
	} 

	// Add this bitmap and layer pairing to our export list
	if (!m_ExportParams.AddLayerAndBitmap(m_pLayer, pBitmap))
	{
		FailAndExecute();
		End();
		return; 
	}

	// Open the preview dialog, and display the selected frame.
	if(!PreviewAnimation())
	{
		// We failed to preview the bitmap
		FailAndExecute();
		End();
		return; 
	}

 	// Finish the operation
	End(); 

	return;
}

/********************************************************************************************

>	BOOL OpGrabFrame::StartGrab()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to set up useful items prior to doing the grab.

********************************************************************************************/

BOOL OpGrabFrame::StartGrab()
{
	// Is there a selected Document?
	m_pDocument = Document::GetSelected();
	if (m_pDocument == NULL)
	{
		ERROR3("Called OpGrabFrame with no selected document!");
		return FALSE; 
	}

	// Is there a selected spread?
	m_pSpread = Document::GetSelectedSpread();
	if (m_pSpread == NULL)
	{
		ERROR3("Called OpGrabFrame with no selected spread!");
		return FALSE; 
	}

	// Is there a selected view?
	m_pView = DocView::GetSelected();
	if (m_pView == NULL)
	{
		ERROR3("Called OpGrabFrame with no selected view!");
		return FALSE; 
	}

	// Check that all visible layers are actually frame layers
	FrameSGallery::EnsureFrameLayerIntegrity(m_pSpread);

	// yes, so check if there is an active layer and try to grab it
	m_pLayer = m_pSpread->FindActiveLayer();
	if (m_pLayer == NULL)
	{
		ERROR3("Called OpGrabFrame with no active layer!");
		return FALSE; 
	}

	// Find all objects on this spread from the specified layer,
	// selected or unselected, and get the bounding them.
	// Range Control parameters are:-
	// Include selected nodes - we want TRUE.
	// Include Unselected nodes - we want TRUE.
	// Cross layers - we want TRUE
	// Ignore locked layers - we want FALSE so that we include locked layers.
	// IsRenderable if set then calls NeedsToRender - we want TRUE
	// Ignore invisible layers - we want FALSE so that we don't include invisible layers.
	//Range SearchRange(m_pLayer, NULL, RangeControl(TRUE, TRUE, TRUE, FALSE, TRUE, FALSE)); 

	// We need to invalidate the region
	/* if (!DoInvalidateNodesRegions(SearchRange, TRUE))
	{
		return FALSE; 
	} */

	/* if (!DoStartSelOp(FALSE,FALSE))  // Try to record the selection state , don't render the blobs though 
	{
		return FALSE; 
	}*/

	// Remember the selection so that we can put it back at the end of the operation
	RememberTheSelection();

	// Copy all the current animation parameters from the spread into our export
	// params.
	m_ExportParams.SetSpreadAnimPropertiesParam(m_pSpread->GetSpreadAnimPropertiesParam());

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL OpGrabFrame::RememberTheSelection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to remember the current selection state so that we can put it back
				at the end of the operation.
				Don't error if it fails as the loss of the selection is no great hardship
				when memory is tight!

********************************************************************************************/

BOOL OpGrabFrame::RememberTheSelection()
{
	// Create a SelectionState object
	m_pSelState = new SelectionState(); 
	if (m_pSelState == NULL)
	{
		return FALSE; // Failure 
	}

	// We have managed to create a SelectionState instance, now lets try and 
	// record the current selections 
	if (!m_pSelState->Record())  // We failed to allocate enough memory to store the selection state
	{
		// There was insufficient memory to record the selections 
		delete m_pSelState;		// Delete the selection state 
		m_pSelState = NULL; 
		return FALSE; 
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL OpGrabFrame::PutBackTheSelection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/6/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to put back the remembered selection state at the start of the operation.

********************************************************************************************/

BOOL OpGrabFrame::PutBackTheSelection()
{
	if (m_pSelState == NULL)
		return FALSE;
	
	BOOL RestoreSelBlobs = TRUE; //FALSE;
	BOOL RemoveBlobs = FALSE; //TRUE;
	// Restores the recorded selected state   
	m_pSelState->Restore(RestoreSelBlobs, RemoveBlobs);

	delete m_pSelState;
	m_pSelState = NULL;

	return TRUE;
}

/********************************************************************************************

>	KernelBitmap* OpGrabFrame::GrabFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		
	Returns:	KernelBitmap if worked ok, NULL otherwise
	Purpose:	Tries to grab the currently selected frame/layer into a bitmap.

********************************************************************************************/

KernelBitmap* OpGrabFrame::GrabFrame()
{
	ERROR2IF(m_pDocument == NULL,FALSE,"GrabFrame bad document pointer");
	ERROR2IF(m_pSpread == NULL,FALSE,"GrabFrame bad spread pointer");
	ERROR2IF(m_pLayer == NULL,FALSE,"GrabFrame bad layer pointer");
	ERROR2IF(m_pView == NULL,FALSE,"GrabFrame bad view pointer");

	// Create a special Bitmap filter, that creates a bitmap
    // and puts it in the tree   
	ERROR3IF(m_pBitmapFilter != NULL,"Already have a pBitmapFilter!");
	m_pBitmapFilter = new GrabFrameFilter();
	if (m_pBitmapFilter == NULL)
	{
		return NULL; 
	}

	// Get the filter to work out the new rectangle 
	m_pBitmapFilter->SetUpClippingRectangleForExport(m_pSpread, DRAWING);
	DocRect NewBoundingRect = m_pBitmapFilter->GetClipRect();
	// Save away the new bounding rectangle
	m_pSpread->SetAnimationBoundingRect(NewBoundingRect);
	m_ExportParams.SetAnimationBoundingRect(NewBoundingRect);

	// Save away the quality that this was grabbed at
	Quality NewQuality = m_pView->RenderQuality;
	m_pSpread->SetAnimationQuality(NewQuality);

	// Check all the layers to see if they contain a single bitmap which we can use
	// instead of generating one.
	// This will mark all valid single bitmaps using the reference in the layer
	if (!CheckAllForSingleBitmaps(m_pSpread, m_pBitmapFilter))
	{
		delete m_pBitmapFilter;
		m_pBitmapFilter = NULL;
		return NULL; 
	} 

	// Check if we are grabbing a changed background layer frame, if it is then
	// we will then clear the edited bit on this. Once this happens then if the
	// user grabs all frames then the dependent frames wont be updated.
	if (m_pLayer->IsEdited() && m_pLayer->IsSolid())
	{
		// Mark all frames above this as edited until we reach the last frame
		// OR we hit a solid/background layer.
		Layer * pFrame = m_pLayer->FindNextFrameLayer();
		while (pFrame != NULL && !pFrame->IsSolid())
		{
			// Mark that frame as edited
			pFrame->SetEdited(TRUE);
			pFrame = pFrame->FindNextFrameLayer();
		}
	}

	// Check to see if we have a single bitmap on the layer which is stored
	// in a NodeBitmap at a 1:1 scale and is 8bpp or less and the palette matches
	// the present options.
	// If it passes all these tests, then set pBitmapToCreate to be that bitmap.
	// Otherwise, we will need to go and convert the current frame into a bitmap
	KernelBitmap* pBitmapToCreate = NULL;
	//if (!CheckIfSingleBitmap(m_pLayer, &pBitmapToCreate))
	// If there is a referenced bitmap then we can use this instead of generating one
	pBitmapToCreate = m_pLayer->GetReferencedBitmap();
	if (pBitmapToCreate == NULL)
	{
		// Grab the frame


		// As this will affect us during grabbing, we should force it into the known
		// non-interferring state
		BOOL CurrentVisEdState = Layer::ActiveLayerVisibleAndEditable;
		Layer::ActiveLayerVisibleAndEditable = FALSE;
		// The all visible flag will also do this for us. 
		Document* pDoc = m_pSpread->FindParentDocument();
		BOOL CurrentAllVisibleState = FALSE;
		if (pDoc != NULL)
		{
			CurrentAllVisibleState = pDoc->IsAllVisible();
			pDoc->SetAllVisible(FALSE);
		}
		
		// Go and convert the frame into its bitmap form
		pBitmapToCreate = GrabTheFrame();

		// Put back the active layer's visibilty and selection
		Layer::ActiveLayerVisibleAndEditable = CurrentVisEdState;
		// Put back the all visible state
		if (pDoc != NULL)
			pDoc->SetAllVisible(CurrentAllVisibleState);
	}

	// Note the capture quality that the frame was captured at but only if we worked correctly
	if (pBitmapToCreate)
		m_pLayer->SetCaptureQuality(NewQuality);

	// remove the filter as we have no more use for it
	delete m_pBitmapFilter;
	m_pBitmapFilter = NULL;

	// Clear the edited flag on the layer
	m_pLayer->SetEdited(FALSE);
#ifdef _DEBUG
	// Tell the frame gallery to update its display of the frame
	BROADCAST_TO_ALL(LayerMsg(m_pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
#endif

	// return the bitmap to the caller
	return pBitmapToCreate;
}

/********************************************************************************************

>	KernelBitmap* OpGrabFrame::GrabTheFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/97
	Inputs:		
	Returns:	KernelBitmap if worked ok, NULL otherwise
	Purpose:	Tries to grab the currently selected frame/layer into a bitmap.
				Same as GrabFrame but assumes that the filter to do the grab, m_pBitmapFilter,
				is already present.

********************************************************************************************/

KernelBitmap* OpGrabFrame::GrabTheFrame()
{
	ERROR2IF(m_pDocument == NULL,FALSE,"GrabFrame bad document pointer");
	ERROR2IF(m_pLayer == NULL,FALSE,"GrabFrame bad layer pointer");
	ERROR2IF(m_pBitmapFilter == NULL,FALSE,"GrabFrame m_pBitmapFilter pointer");
    
	// Tell it about the layer that is being captured
	// This supplies the name of the bitmap and the export parameters
	m_pBitmapFilter->SetLayerBeingCaptured(m_pLayer);

	KernelBitmap* pBitmapToCreate = NULL;
	if (!m_pBitmapFilter->DoCreateBitmap(this, m_pDocument, &pBitmapToCreate) || pBitmapToCreate == NULL)
	{
		return NULL; 
	}

	// Copy across the delay from the layer (Andy 5/1/01)
	DWORD delay = m_pLayer->GetFrameDelay();
	pBitmapToCreate->SetDelay(delay);

	// Make the layer remember this bitmap as the one that was generated
	if (!m_pLayer->SetGeneratedBitmap(pBitmapToCreate))
	{
		ERROR3("Couldn't add bitmap to layer!");
		if (pBitmapToCreate)
			delete pBitmapToCreate;
		return NULL; 
	}

	// DO NOT unset the edited flag. This version is used in GrabAllFrames and we need the
	// edited flags present until we have finished grabbing so that we can work out dependent
	// layers.

	// return the bitmap to the caller
	return pBitmapToCreate;
}

/********************************************************************************************

>	BOOL OpGrabFrame::EndGrab()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to clean up after doing the grab.

********************************************************************************************/

BOOL OpGrabFrame::EndGrab()
{
	// Try and put back the selection that we recorded at the start
	PutBackTheSelection();

	// Make sure that the new selection is up to date
	//Range * pSelRange = GetApplication()->FindSelection();
/*	if (pSelRange == NULL || !DoInvalidateNodesRegions(*pSelRange, TRUE))
	{
		// We failed to invalidate the region
		return FALSE; 
	} */

/*	// Recover the active layer again and make sure that it is being shown correctly.
	ERROR2IF(m_pSpread == NULL,FALSE,"bad spread in EndGrab()");
	Layer *pLayer = m_pSpread->FindActiveLayer();
	ERROR2IF(pLayer == NULL,"No active layer in EndGrab()")
	if (pLayer)
	{
		FrameSGallery::FixOtherLayersFromActive(pLayer);
	} */

	return TRUE;
}

/********************************************************************************************

>	BOOL OpGrabFrame::GetFileNameFromUser()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Gives the operation chance to get the filename from the user.
				In this baseclass version we do nothing as we preview a single frame and
				so have no need of a filename.

********************************************************************************************/

BOOL OpGrabFrame::GetFileNameFromUser()
{
	// Baseclass version, do nothing
	return TRUE;
}

/********************************************************************************************

>	BOOL OpGrabFrame::PreviewAnimation()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to preview the frames that have been grabbed.
				Uses the stored animation details in m_ExportParams

********************************************************************************************/

BOOL OpGrabFrame::PreviewAnimation()
{
	// Copy all the current animation parameters from the spread into our export
	// params.
	m_ExportParams.SetSpreadAnimPropertiesParam(m_pSpread->GetSpreadAnimPropertiesParam());

	// See if there is a currently open preview dialog box
	PreviewDialog * pPreviewDialog = PreviewDialog::GetPreviewDialog();

	// Check that we have a list of bitmaps to preview. If not then complain bitterly.
	// Hidden frames allow us to get this far.
	if (m_ExportParams.GetBitmapCount() == 0)
	{
		// If the preview dialog is open then force it into an idle state
		if (pPreviewDialog != NULL)
			pPreviewDialog->SetDialogToIdleState();
		
		// tell the user about the problems
		InformError(_R(IDS_NOFRAMESTOPREVIEW));
		return FALSE;
	}

	// If the preview dialog is currently open, pass it the list of bitmaps.
	// We must do this on all types of preview e.g. save, as we will have stopped
	// the animation before previewing and so we need to ensure that the preview
	// dialog box is restored to a known state.
	if (pPreviewDialog != NULL)
	{
		// Pass the preview dialog box the new list of bitmaps
		pPreviewDialog->SetBitmapList(&m_ExportParams);

		PreviewDialog::ToggleViaBitmapGallery (FALSE);
		//pPreviewDialog->ToggleViaBitmapGallery (FALSE);
		pPreviewDialog->SetPlayAnimation(TRUE);

		// Call the dialog box so that it sets up the necessary states
		pPreviewDialog->ReInitDialog();
	}

	// This does the actually job in hand, e.g. preview, save, preview in browser
	BOOL ok = SaveOrShowTheAnimation(pPreviewDialog, &m_ExportParams);

	return ok;
}

/********************************************************************************************

>	BOOL OpGrabFrame::SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/97
	Inputs:		pPreviewDialog	the preview dialog box, if open
				pExportParam	the export param which we will use
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to actually do the preview operation required.
				In this case we just start up the preview dialog box, if not already open,
				to show the frames that have been grabbed.
				Uses the stored animation details in m_ExportParams

********************************************************************************************/

BOOL OpGrabFrame::SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam)
{
	// Only boot up the preview dialog box if it is not already on display
	// If it is already on display then we will have already dealt with it
	if (pPreviewDialog == NULL)
	{
		// Invoke the preview operation
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_PREVIEWDIALOG);
		if (pOpDesc)
		{
			PreviewDialog::ToggleViaBitmapGallery (FALSE);
			pOpDesc->Invoke(&m_ExportParams);
			pPreviewDialog = PreviewDialog::GetPreviewDialog();
			//pPreviewDialog->ToggleViaBitmapGallery (FALSE);
		}
	}

	return TRUE;
}

/****************************************************************************************************

>	BOOL OpGrabFrame::SetSingleBitmapInList()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise.
	Purpose:	If the Preview Dialog is open and we regenerate a single frame,
				this function allows us to update our Bitmap list in the Preview Dialog. 

******************************************************************************************************/

/* BOOL OpGrabFrame::SetSingleBitmapInList()
{
	// Copy all the current animation parameters from the spread into our export params.
	m_ExportParams.SetSpreadAnimPropertiesParam(m_pSpread->GetSpreadAnimPropertiesParam());

	// Get our static pointer to the Preview Dialog.
	PreviewDialog* pPreviewDialog = PreviewDialog::GetPreviewDialog();

	if (!pPreviewDialog)
		return FALSE;

	pPreviewDialog->SetRegeneratedBitmapInList(&m_ExportParams);

	return TRUE;
} */

/******************************************************************************************************

>	BOOL OpGrabFrame::SetBitmapToListTail()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		-
	Returns:	True if worked ok, False otherwise.
	Purpose:	If the Preview Dialog is open and we regenerate a single frame, this function
				allows us to add that Bitmap to the head of the preview dialog bitmap list. 

********************************************************************************************************/

/* BOOL OpGrabFrame::SetBitmapToListTail()
{
	// Copy all the current animation parameters from the spread into our export params.
	m_ExportParams.SetSpreadAnimPropertiesParam(m_pSpread->GetSpreadAnimPropertiesParam());

	// Get our static pointer to the Preview Dialog.
	PreviewDialog* pPreviewDialog = PreviewDialog::GetPreviewDialog();

	if (!pPreviewDialog)
		return FALSE;

	// Add this bitmap to the list tail.
	pPreviewDialog->SetBitmapToListTail(&m_ExportParams);

	return TRUE;
} */

/****************************************************************************************************

>	BOOL OpGrabFrame::SetFullBitmapList()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/05/97
	Inputs:		SetFlagState: If this is specified, it allows us to set the state of certain flags 
				in the Preview dialog.
	Returns:	True if worked ok, False otherwise.
	Purpose:	If the Preview Dialog is open and we regenerate the frames in the animation,
				this function allows us to update our Bitmap list in the Preview Dialog. 

******************************************************************************************************/

/* BOOL OpGrabFrame::SetFullBitmapList(BOOL SetFlagState)
{
	// Copy all the current animation parameters from the spread into our export params.
	m_ExportParams.SetSpreadAnimPropertiesParam(m_pSpread->GetSpreadAnimPropertiesParam());

	// Get our static pointer to the Preview Dialog.
	PreviewDialog* pPreviewDialog = PreviewDialog::GetPreviewDialog();

	if (!pPreviewDialog)
		return FALSE;

	// If the animation is playing, stop the animation before we pass it our new list of bitmaps.
	if(pPreviewDialog->GetPlayAnimation())
		pPreviewDialog->SetPlayAnimation(FALSE);

	pPreviewDialog->SetBitmapList(&m_ExportParams);

	// Set the state of the preview dlg flags.
	if(SetFlagState)
	{
		// Call the dialog box so that it sets up the necessary states
		pPreviewDialog->ReInitDialog();
	}

	// Play the animation.
	pPreviewDialog->SetPlayAnimation(TRUE);

	return TRUE;
} */


/********************************************************************************************

>	BOOL OpGrabFrame::CheckIfSingleBitmap(Layer * pLayer, KernelBitmap ** ppBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/6/97
	Inputs:		pLayer	- the layer we need to check
	Outputs:	The bitmap to use for this frame
	Returns:	True if found a single bitmap on the frame/layer. False otherwise.
	Purpose:	For finding out if there is a single bitmap on the layer which is stored
				in a NodeBitmap at a 1:1 scale and is 8bpp or less and the palette matches
				the present options. This palette testing is now done after this
				If it passes all these tests, then set pBitmap to be that bitmap and return
				True. Also note in the layer that this bitmap can possibly be used to specify
				this layer. We can only say for sure when we have checked that its palette
				matches the current animation options.
	SeeAlso:	CheckAllForSingleBitmaps;

********************************************************************************************/

BOOL OpGrabFrame::CheckIfSingleBitmap(Layer * pLayer, KernelBitmap ** ppBitmap)
{
	ERROR2IF(pLayer == NULL, FALSE,"CheckIfSingleBitmap bad layer pointer");
	ERROR2IF(ppBitmap == NULL, FALSE,"CheckIfSingleBitmap bad bitmap pointer");

	// Get some of the current export options
	DocRect	BoundingRect	= m_ExportParams.GetAnimationBoundingRect();
	ERROR3IF(BoundingRect.IsEmpty(),"Bounding rect not set up")
//	PALETTE_COLOURS	Palette = m_ExportParams.GetPaletteCols();
//	WEB_PALETTE WebPalette	= m_ExportParams.GetPalette();

	KernelBitmap * pBitmap	= NULL;
	BOOL Scaled1to1			= FALSE;
	BOOL PaletteOk			= FALSE;
	BOOL InsideBoundingRect = TRUE;
	BOOL FullFirstFrame		= TRUE;
	UINT32 Bpp				= 0;
	INT32 NodeCount			= 0;
	NodeBitmap *pNodeBmp	= NULL;

	// check if we are the first animation frame, if so then we the bitmap must be the same
	// size as the animation bounding rect. Otherwise, the first frame will not be the size
	// of the animation which highlights a bug in MS IE 3.0
	Layer * pPreviousFrame = pLayer->FindPrevFrameLayer();

	Node * pNode = pLayer->FindFirstChild();
	while (pNode != NULL)
	{
		// If the node renders something on the screen then we count it as this will mean that
		// the single bitmap does not fully define the layer.
		// It ignores attributes, hidden nodes, etc - nodes that don't render anything themselves.
		if (pNode->IsBounded())
			NodeCount++;

		// We need to note any NodeBitmap nodes 
		if (pNode->IS_KIND_OF(NodeBitmap) && NodeCount == 1)
		{
			// Convert the node to a nodebitmap so we can get at the useful NodeBitmap
			// related data
			pNodeBmp = (NodeBitmap *)pNode;
			
			// Get the size and position of the node bitmap
			DocRect rect = pNodeBmp->GetBoundingRect(TRUE, FALSE);

			// If we have no previous frame then check that the first frame is the
			// size of the animation
			if (pPreviousFrame == NULL)
			{
				DocRect AniBoundingRect = m_ExportParams.GetAnimationBoundingRect();
				ERROR3IF(AniBoundingRect.IsEmpty(),"CheckIfSingleBitmap Animation bounding rect not set up!");

				// REMEMBER account for rounding errors
				const INT32 delta = 50;
				if (
					(AniBoundingRect.lo.x >= (rect.lo.x - delta)) && (AniBoundingRect.lo.x <= (rect.lo.x + delta)) &&
					(AniBoundingRect.hi.x >= (rect.hi.x - delta)) && (AniBoundingRect.hi.x <= (rect.hi.x + delta)) &&
					(AniBoundingRect.lo.y >= (rect.lo.y - delta)) && (AniBoundingRect.lo.y <= (rect.lo.y + delta)) &&
					(AniBoundingRect.hi.y >= (rect.hi.y - delta)) && (AniBoundingRect.hi.y <= (rect.hi.y + delta))
					)
				{
					FullFirstFrame = TRUE;
				}
				else
				{
					FullFirstFrame = FALSE;
				}
			}
			
			// Get the bitmap stored inside the node, if any
			pBitmap = pNodeBmp->GetBitmap();
			if (pBitmap)
			{
				// Get the width/height of the bitmap in millipoints
				INT32 BmpWidth = pBitmap->GetRecommendedWidth();
				INT32 BmpHeight = pBitmap->GetRecommendedHeight();

				// Get the Width of the Bitmap in Millipoints
				INT32 Width  = INT32(pNodeBmp->Parallel[0].Distance(pNodeBmp->Parallel[1]));
				INT32 Height = INT32(pNodeBmp->Parallel[1].Distance(pNodeBmp->Parallel[2]));

				// REMEMBER account for rounding errors and the orientation being the other
				// way round
				const INT32 delta = 2;
				if ((Width >= BmpWidth - delta && Width <= BmpWidth + delta &&
					 Height >= BmpHeight - delta && Height <= BmpHeight + delta) ||
					(Height >= BmpWidth - delta && Height <= BmpWidth + delta &&
					 Width >= BmpHeight - delta && Width <= BmpHeight + delta) )
					Scaled1to1 = TRUE;
				
				Bpp = pBitmap->GetBPP();

				// If the bitmap is 8bpp of less then assume this is ok for now
				// We will check the palette in a second pass
				if (Bpp <= 8)
				{
					PaletteOk = TRUE;
				}

				/*	// We cannot cope with GLOBAL OPTIMISED palettes at present as we
					// would need to wait until the palette was generated before we could
					// check.
					if (Palette == PALCOL_OPTIMIZED && WebPalette == PAL_LOCAL)
					{
						// Saving local palette so any palette ok
						PaletteOk = TRUE;
					}
					else if (Palette == PALCOL_BROWSER && WebPalette == PAL_GLOBAL)
					{
						// Check if we have a browser palette on this bitmap
						//PaletteOk = pBitmap->IsBrowserPalette();
						// Old way of testing. Too lenient as all it does it check that the palette
						// has the correct colours in it, not that the palette will be the same one
						// that we are going to output as the browser palette
						
						// Obviously, browser palettes can only happen on 8bpp bitmaps
						if (Bpp == 8)
						{
							// First of all, if we haven't already got our palette that we check against
							// then construct it.
							if (m_pBrowserPalette == NULL)
							{
								UINT32 PaletteSize = UINT32(pow(2,Bpp));
								m_pBrowserPalette = DIBUtil::AllocateLogPalette(PaletteSize);
								if (m_pBrowserPalette != NULL)
								{
									// Now force it into a browser compatible state
									BOOL AvoidSystemColours = TRUE;
									PaletteManager::MakePaletteBrowserCompatible(m_pBrowserPalette, AvoidSystemColours);
								}
							}
							
							INT32 TransColour = -1;
							pBitmap->GetTransparencyIndex(&TransColour);

							if (m_pBrowserPalette != NULL)
							{
								PaletteOk = pBitmap->ArePalettesTheSame(m_pBrowserPalette, TransColour);
							}
							else
							{
								ERROR3("Couldn't allocate browser palette");
								PaletteOk = FALSE;
							}
						}
						else
							PaletteOk = FALSE;
					}
				} */

				if (Scaled1to1 && PaletteOk && !rect.IsEmpty() && !BoundingRect.IsEmpty())
				{
					// Work out the position of the bitmap relative to the top left hand
					// corner of the bounding rectangle in terms of pixels and put this
					// in the bitmap 
					MILLIPOINT LeftOffsetMP = rect.lox - BoundingRect.lox;
					MILLIPOINT TopOffsetMP = BoundingRect.hiy - rect.hiy;
					if (LeftOffsetMP >= 0 && TopOffsetMP >= 0)
					{
						const UINT32 dpi = 96;
						UINT32 LeftOffset = LeftOffsetMP * dpi/72000;
						UINT32 TopOffset = TopOffsetMP * dpi/72000;

						pBitmap->SetLeftOffset(LeftOffset);
						pBitmap->SetTopOffset(TopOffset);
						InsideBoundingRect = TRUE;
					}
				}
			}
		}

		pNode = pNode->FindNext();
	}

	// If all these checks are TRUE then return the bitmap as the source of the frame
	if (pBitmap != NULL && Scaled1to1 && Bpp <= 8 && NodeCount == 1 &&
		PaletteOk && InsideBoundingRect && FullFirstFrame)
	{
		if (ppBitmap)
			*ppBitmap = pBitmap;

		// Make the layer remember this bitmap
		// Remember this bitmap as the reference one
		pLayer->SetReferencedBitmap(pBitmap);

		//TRACEUSER( "Neville", _T("Using single bitmap\n"));

		return TRUE;
	}

	// Ensure that we have no reference saved
	pLayer->SetReferencedBitmap(NULL);
			
	return FALSE;
}


/********************************************************************************************

>	BOOL OpGrabFrame::CheckAllForSingleBitmaps(Spread * pSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/97
	Inputs:		pLayer			- the layer we need to check
				pBitmapFilter	- a grab frame filter we can use 
	Returns:	True if worked correctly, False otherwise.
	Purpose:	For going through all the frames and seeing if there is a single bitmap
				on the frame/layer which is stored in a NodeBitmap at a 1:1 scale and is 8bpp
				or less and the palette matches	the present options.
				We use the grab frame filter as this is derived off GIF filter and this must
				already cope with this when loading animations so that it can set the animation
				options accordingly.

********************************************************************************************/

BOOL OpGrabFrame::CheckAllForSingleBitmaps(Spread * pSpread, GrabFrameFilter * pBitmapFilter)
{
	ERROR2IF(pSpread == NULL, FALSE,"CheckIfSingleBitmap bad Spread pointer");
	ERROR2IF(pBitmapFilter == NULL, FALSE,"CheckIfSingleBitmap bad GrabFrameFilter pointer");

	// So we can do the optimised palette case, we must go though all the
	// layers/frames and see if there is a single bitmap present. We don't worry
	// about the palette on this first pass. We just set up the referenced bitmap
	// to be the single bitmap if it passes all the tests.
	KernelBitmap *	pBitmap = NULL;
	BOOL Found				= FALSE;
	BOOL SolidSet			= FALSE;
	BOOL OverlaySet			= FALSE;
	BOOL OverlayedBitmapSet = FALSE;
	Layer* pCurrentLayer	= pSpread->FindFirstFrameLayer();
	Layer* pNextLayer		= NULL;
	Layer* pPrevFrameLayer	= NULL;
	while (pCurrentLayer != NULL)
	{
		// Move to the next bitmap in the animation
		pNextLayer = pCurrentLayer->FindNextFrameLayer();

		// This is just as a double check
		if (pCurrentLayer->IsPseudoFrame())
		{
			Found = CheckIfSingleBitmap(pCurrentLayer, &pBitmap);
			// If the solid flag is set on anything but the last layer then note this
			// as dangerous
			if (pCurrentLayer->IsSolid() && pNextLayer != NULL)
				SolidSet = TRUE;
			if (pCurrentLayer->IsOverlay())
			{
				// If overlay is set then we can allow it if the previous bitmap has a restore
				// type of overlay as this may be an animation that we have saved out
				// and then loaded in. If a diff has happened then overlay will be set.
				// We can only ever allow this if the layers consist of all bitmaps as
				// otherwise the layer may be overlaying a vector layer and hence be wrong.
				KernelBitmap * pPrevBitmap = NULL;
				if (pPrevFrameLayer)
					pPrevBitmap = pPrevFrameLayer->GetReferencedBitmap();
				// Check that we have a single reuseable bitmap on this layer plus one of the previous
				if (pPrevBitmap != NULL && pBitmap != NULL)
				{
					GIFDisposalMethod restore = pPrevBitmap->GetAnimationRestoreType();
					// Do we include GDM_NONE? Not for now.
					if (restore != GDM_LEAVE)
					{
						// We cannot use the bitmap so set the reference to NULL
						pCurrentLayer->SetReferencedBitmap(NULL);
						OverlaySet = TRUE;
					}
					else
						OverlayedBitmapSet = TRUE;
				}
				else if (pPrevFrameLayer != NULL)
				{
					// If overlay set on the first frame then ignore
					// We cannot use the bitmap so set the reference to NULL
					pCurrentLayer->SetReferencedBitmap(NULL);
					OverlaySet = TRUE;
				}
			}
		}

		// Move to the next bitmap in the animation
		pPrevFrameLayer = pCurrentLayer;
		pCurrentLayer = pNextLayer;
	}

	// Find out about the palettes that the bitmaps have
	// Our filter has a useful function for this
	UINT32 ReqNumberOfColours	= m_ExportParams.GetNumColsInPalette();
	BOOL SamePalettes		= FALSE;
	BOOL OurBrowserPalette	= FALSE;
	BOOL AllHaveBitmaps		= FALSE;
	BOOL AllWithinCount		= FALSE; 
	UINT32 NumberOfColours	= ReqNumberOfColours;
	pBitmapFilter->CheckSingleBitmapsOnSpread(pSpread, &SamePalettes, &OurBrowserPalette, &AllHaveBitmaps,
											  &AllWithinCount, &NumberOfColours);

	PALETTE_COLOURS	Palette = m_ExportParams.GetPaletteCols();
	WEB_PALETTE WebPalette	= m_ExportParams.GetPalette();
	
	BOOL OptionsOk = FALSE;
	// If OverlaySet is true then we have found an overlay situation which we cannot cope with and
	// so will have set the referenced bitmap to null.
	if (OverlaySet && AllHaveBitmaps)
		AllHaveBitmaps = FALSE;

	// If solid is set on any of the frames then it is too dangerous to reuse any single bitmaps
	// We can only allow overlay, if all layers are single bitmaps and the bitmap concerned is
	// of type GDM_LEAVE. Otherwise, it is too dangerous.
	BOOL BitmapOverlayOk = TRUE;
	if (OverlayedBitmapSet && !AllHaveBitmaps)
		BitmapOverlayOk = FALSE;

	if (!SolidSet && BitmapOverlayOk)
	{
		// Check from the most fussy option to the least fussy option
		if (Palette == PALCOL_BROWSER && WebPalette == PAL_GLOBAL && OurBrowserPalette)
		{
			OptionsOk = TRUE;
			//TRACEUSER( "Neville", _T("Global browser, palettes ok\n"));
		}
		else if (Palette == PALCOL_OPTIMIZED && WebPalette == PAL_GLOBAL && SamePalettes && AllHaveBitmaps &&
				 NumberOfColours <= ReqNumberOfColours)
		{
			OptionsOk = TRUE;
			//TRACEUSER( "Neville", _T("Global opt, palettes ok\n"));
			// Force the global optimised palette to be null for now
			pBitmapFilter->SetGlobalOptimisedPalette(NULL);
		}
		else if (Palette == PALCOL_OPTIMIZED && WebPalette == PAL_LOCAL && AllWithinCount)
		{
			// Saving local palette so any palette ok but must be within the count
			OptionsOk = TRUE;
			//TRACEUSER( "Neville", _T("Local Opt, palettes ok\n"));
		}
	}
	
	// Now we need to go through the layers again and ratify our check
	pCurrentLayer = pSpread->FindFirstFrameLayer();
	pBitmap = NULL; 
	while (pCurrentLayer != NULL)
	{
		// This is just as a double check
		if (pCurrentLayer->IsPseudoFrame())
		{
			pBitmap = pCurrentLayer->GetReferencedBitmap();
			if (OptionsOk && pBitmap)
			{
				// We can use the bitmap instead of generating one
				// So make sure that the generated bitmap is clean out
				pCurrentLayer->SetGeneratedBitmap(NULL);
				// And ensure that the bitmap has the layer's delay
				pBitmap->SetDelay(pCurrentLayer->GetFrameDelay());

				// If we are in 
				if (Palette == PALCOL_OPTIMIZED && WebPalette == PAL_GLOBAL)
				{
					// If we haven't set up the global optimised palette then do so now
					// Pull it from the first bitmap that we find
					LPLOGPALETTE pOptPal = pBitmapFilter->GetGlobalOptimisedPalette();
					if (pOptPal == NULL)
					{
						pOptPal = DIBUtil::CopyBitmapPaletteIntoLogPalette(pBitmap);
						// Something went a bit wrong
						if (pOptPal == NULL)
							return FALSE;
						pBitmapFilter->SetGlobalOptimisedPalette(pOptPal);
					}
				}
			}
			else
			{
				// We cannot use the bitmap so set the reference to NULL
				pCurrentLayer->SetReferencedBitmap(NULL);
			}
		}

		// Move to the next bitmap in the animation
		pCurrentLayer = pCurrentLayer->FindNextFrameLayer();
	}

	return TRUE;
}

/********************************************************************************************
// End of OpGrabFrame
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpGrabAllFrames
//------------------------------------------------------------------------------------------

// Macro to determine the bytes in a DWORD aligned DIB scanline
#define BYTESPERLINE(Width, BPP) ((WORD)((((DWORD)(Width) * (DWORD)(BPP) + 31) >> 5)) << 2)

void OpGrabAllFrames::CreateGlobalPalette(DWORD NumColsInPalette, BOOL RegenerateAllFrames)
{
	ERROR3IF(!m_pBitmapFilter, "No bitmap filter to use when generating global palette");
	if (!m_pBitmapFilter)
		return;

	PaletteOptimiser* pPalOpt = new PaletteOptimiser();
	if (!pPalOpt)
		return;
	pPalOpt->Initialise();

	// Go through all the frame layers in the spread and export them
	KernelBitmap * pBitmap = NULL;
	Layer* pCurrentLayer = NULL;

	// Set the filter to generate preview bitmaps
	m_pBitmapFilter->SetGrabMode(GrabFrameFilter::GRABMODE_PREVIEW);

	pBitmap = NULL;
	pCurrentLayer = m_pSpread->FindFirstFrameLayer();

	while (pCurrentLayer != NULL)
	{
		// see if the user has hidden the frame
		if (!pCurrentLayer->IsPseudoFrame() || pCurrentLayer->IsHiddenFrame())
		{
			// It is either an invalid frame or it is hidden. In both cases but especially
			// the second we should ensure that if there is a generated bitmap then we
			// vape it as it is now out of date.
			pCurrentLayer->SetGeneratedBitmap(NULL);

			// Move to the next bitmap in the animation
			pCurrentLayer = pCurrentLayer->FindNextFrameLayer();

			// Go to the next frame
			continue;
		}

		// Set up the visibility of the other layers using this layer
		// as the new temporary and acting active layer
		FrameSGallery::FixOtherLayersFromActive(pCurrentLayer);
			
		// This is the layer that we are adding to the global palette
		m_pLayer = pCurrentLayer;

		// Render a bitmap of the entire image being exported
		KernelBitmap* pTestBitmap;
		m_pBitmapFilter->DoCreateBitmap(this, m_pDocument, &pTestBitmap);

		// not enough memory? or no content worth talking about then give up!
		if (!pTestBitmap || !pTestBitmap->GetActualBitmap())
		{
			FailAndExecute();
			End();
			return;
		}

		// We need to pass Gavin a 32bbp bitmap with an empty channel (rather than an
		// alpha channel).  Lets quickly create one (I am sure this is not the best way
		// of doing this but it will do for now).
		UINT32 nPixels = pTestBitmap->GetActualBitmap()->GetBitmapSize() / 3;
		RGBQUAD* pNewBitmap = new RGBQUAD[nPixels];
		UINT32 width	= pTestBitmap->GetActualBitmap()->GetBitmapInfoHeader()->biWidth;
		UINT32 height	= pTestBitmap->GetActualBitmap()->GetBitmapInfoHeader()->biHeight;
		BYTE* pOldBitmapBytes = (BYTE*)pTestBitmap->GetActualBitmap()->GetBitmapBits();
		UINT32 bytesPerLine = BYTESPERLINE(width, 24);
		for (UINT32 y = 0; y < height; ++y)
			for (UINT32 x = 0; x < width; x++)
			{
				pNewBitmap[y * width + x].rgbBlue		= pOldBitmapBytes[bytesPerLine * y + x * 3 + 0];
				pNewBitmap[y * width + x].rgbGreen		= pOldBitmapBytes[bytesPerLine * y + x * 3 + 1];
				pNewBitmap[y * width + x].rgbRed		= pOldBitmapBytes[bytesPerLine * y + x * 3 + 2];
				pNewBitmap[y * width + x].rgbReserved	= 0;
			}

		// add the stats to the palette optimiser
		pPalOpt->AddStats(pNewBitmap, nPixels);

		delete pNewBitmap;
		delete pTestBitmap;

		// Move to the next bitmap in the animation
		pCurrentLayer = pCurrentLayer->FindNextFrameLayer();
	}

	// set the global palette
	pPalOpt->SnapToBrowserPalette(0);
	pPalOpt->SnapToPrimaries(1);
	pPalOpt->AddSystemColours((m_pSpread->GetSpreadAnimPropertiesParam().GetUseSystemCols() == TRUE));
	pPalOpt->UseBrowserPalette(0);
	pPalOpt->SetFast(TRUE);

	INT32 ColoursToOptimise = max(2, NumColsInPalette-1);

	// tell the optimiser that we have finished gathering the stats
	pPalOpt->GenPalette(ColoursToOptimise);

	LOGPALETTE * pGlobalPal = DIBUtil::AllocateLogPalette(ColoursToOptimise + 1);

	pGlobalPal->palNumEntries = ColoursToOptimise;

	// get the optimised palette from the optimser
	pPalOpt->GetPalette( pGlobalPal, ColoursToOptimise);

	 // we want to add a transparent colour to this palette
	pGlobalPal->palNumEntries = min(255,pGlobalPal->palNumEntries+1);

	// the first entry in the palette is special it should be the background colour as it will
	// be what is visible if the background is not transparent so swap the first entry into
	// the last and set the last
	pGlobalPal->palPalEntry[ pGlobalPal->palNumEntries-1 ].peRed	= pGlobalPal->palPalEntry[ 0 ].peRed;
	pGlobalPal->palPalEntry[ pGlobalPal->palNumEntries-1 ].peGreen	= pGlobalPal->palPalEntry[ 0 ].peGreen;
	pGlobalPal->palPalEntry[ pGlobalPal->palNumEntries-1 ].peBlue	= pGlobalPal->palPalEntry[ 0 ].peBlue;
	pGlobalPal->palPalEntry[ pGlobalPal->palNumEntries-1 ].peFlags	= 0;

	DocColour dcol = Page::GetPageColour();
	INT32 r = 255, g = 255, b = 255;
	dcol.GetRGBValue(&r,&g,&b);

	pGlobalPal->palPalEntry[ 0 ].peRed		= (BYTE)r;
	pGlobalPal->palPalEntry[ 0 ].peGreen	= (BYTE)g;
	pGlobalPal->palPalEntry[ 0 ].peBlue		= (BYTE)b;
	pGlobalPal->palPalEntry[ 0 ].peFlags	= 255; // the transp entry - not renderable

	// remove the optimiser
	delete pPalOpt;

	// save this global palette for later
	m_pBitmapFilter->SetGlobalOptimisedPalette(pGlobalPal);
	m_pBitmapFilter->AlterPaletteContents(pGlobalPal);

	// Undo our change to the filter
	m_pBitmapFilter->SetGrabMode(GrabFrameFilter::GRABMODE_FOROUTPUT);
}

/********************************************************************************************

>	void OpGrabAllFrames::Do(OpDescriptor * pOpDesc)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/4/97
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Tries to grab the selected animation frame (i.e. layer).
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpGrabAllFrames::Do( OpDescriptor * pOpDesc )
{
	// Get useful data and set up items before starting the grab process
	if (!StartGrab())
	{
		// We failed to grab the frame
		FailAndExecute();
		End();
		return; 
	} 

	// If required, allow the op to get a filename from the user.
	// This also allows them access to the options via the button on the save dialog box.
	// This is why we must do this first rather than after we have generated the frames.
	if (!GetFileNameFromUser())
	{
		// We failed to grab the frame
		FailAndExecute();
		End();
		return; 
	} 

	// Get our static pointer to the Preview Dialog.
	PreviewDialog* pPreviewDlg = PreviewDialog::GetPreviewDialog();
	// If the preview dialog is already open, find the selected frame layer.
	BOOL PlayAnimation = TRUE;
	if (pPreviewDlg)
	{
		// Note the current state of the playing
		PlayAnimation = pPreviewDlg->GetPlayAnimation();
		// Force the dialog into stop mode
		pPreviewDlg->StopPreviewing();
	}

	// Create a special Bitmap filter, that creates a bitmap
    // and puts it in the tree   
	ERROR3IF(m_pBitmapFilter != NULL,"Already have a pBitmapFilter!");
	m_pBitmapFilter = new GrabFrameFilter();
	if (m_pBitmapFilter == NULL)
	{
		// We failed to grab the frame
		// Try and put back the selection that we recorded at the start
		PutBackTheSelection();
		FailAndExecute();
		End();
		return; 
	}

	// Get the stored cliprect from the spread.
	// This may be blank if the frames have not been generated before.
	DocRect OldBoundingRect = m_pSpread->GetAnimationBoundingRect();
	// Get the filter to work out the new rectangle 
	m_pBitmapFilter->SetUpClippingRectangleForExport(m_pSpread, DRAWING);
	DocRect NewBoundingRect = m_pBitmapFilter->GetClipRect();
	// Save away the new bounding rectangle
	m_pSpread->SetAnimationBoundingRect(NewBoundingRect);
	m_ExportParams.SetAnimationBoundingRect(NewBoundingRect);
	// If they are different then we must regenerate all frames
	BOOL RegenerateAllFrames = ms_ForceRefreshOfAllFrames; // used to be = FALSE (sjk)
	ms_ForceRefreshOfAllFrames = FALSE; // have used the force refresh

	if (NewBoundingRect != OldBoundingRect)
		RegenerateAllFrames = TRUE;

	Quality OldQuality = m_pSpread->GetAnimationQuality();
	Quality NewQuality = m_pView->RenderQuality;
	NewQuality.Antialias = Quality::FullAntialias;
	NewQuality.Transparency = Quality::FullTransparency;
	// Save away the new quality
	m_pSpread->SetAnimationQuality(NewQuality);
	if (NewQuality != OldQuality)
		RegenerateAllFrames = TRUE;

	// Check if somebody has edited the page background layer. If they have then
	// regenerate all frames
	Layer * pPageBackground = m_pSpread->FindFirstPageBackgroundLayer();
	if (pPageBackground)
	{
		if (pPageBackground->IsEdited())
			RegenerateAllFrames = TRUE;
	}

	// If the user has a global optimised palette and they have edited one layer
	// then we have really no choice but to always regenerate all frames
	// as the user may have changed something that will affect the colours in the palette.
	//DITHER			Dither;
	WEB_PALETTE		WebPalette			= PAL_GLOBAL;
	PALETTE_COLOURS ColoursPalette		= PALCOL_BROWSER;
	DWORD			NumColsInPalette;
	m_pSpread->GetAnimationColours(NULL, &WebPalette, &ColoursPalette, &NumColsInPalette, NULL);
	if (WebPalette == PAL_GLOBAL && ColoursPalette == PALCOL_OPTIMIZED)
	{
		// check to see if one of the frame layers has been edited
		Layer * pTestLayer = m_pSpread->FindFirstFrameLayer();
		while (pTestLayer)
		{
			if (pTestLayer->IsEdited() && pTestLayer->IsPseudoFrame())
			{
				// Somebody has edited one of the frames so we must regenerate all
				RegenerateAllFrames = TRUE;
				break;
			}
			pTestLayer = pTestLayer->FindNextFrameLayer();
		}
	}

	// Check all the layers to see if they contain a single bitmap which we can use
	// instead of generating one.
	// This will mark all valid single bitmaps using the reference in the layer
	if (!CheckAllForSingleBitmaps(m_pSpread, m_pBitmapFilter))
	{
		// Try and put back the selection that we recorded at the start
		PutBackTheSelection();
		delete m_pBitmapFilter;
		m_pBitmapFilter = NULL;
		// We failed to clean up after grabbing the frame
		FailAndExecute();
		End();
		return; 
	} 
	
	// As this will affect us during grabbing, we should force it into the known
	// non-interferring state
	BOOL CurrentVisEdState = Layer::ActiveLayerVisibleAndEditable;
	Layer::ActiveLayerVisibleAndEditable = FALSE;
	// The all visible flag will also do this for us. 
	Document* pDoc = m_pSpread->FindParentDocument();
	BOOL CurrentAllVisibleState = FALSE;
	if (pDoc != NULL)
	{
		CurrentAllVisibleState = pDoc->IsAllVisible();
		pDoc->SetAllVisible(FALSE);
	}

	// SMFIX Do we need a global palette for all the frames?
	BOOL CreatingGlobalPalette = (WebPalette == PAL_GLOBAL && ColoursPalette == PALCOL_OPTIMIZED);


	// if we are having a global palette then get an optimiser to create one from the bitmaps
	if (CreatingGlobalPalette)
		CreateGlobalPalette(NumColsInPalette, RegenerateAllFrames);

	// Go through all the frame layers in the spread and export them
	KernelBitmap * pBitmap = NULL;
	Layer* pCurrentLayer = NULL;

	// Set the filter to generate output bitmaps
	m_pBitmapFilter->SetGrabMode(GrabFrameFilter::GRABMODE_FOROUTPUT);

	// go round once unless we are creating a global palette then go round twice
	pBitmap = NULL;
	pCurrentLayer = m_pSpread->FindFirstFrameLayer();

	while (pCurrentLayer != NULL)
	{
		// see if the user has hidden the frame
		if (!pCurrentLayer->IsPseudoFrame() || pCurrentLayer->IsHiddenFrame())
		{
			// It is either an invalid frame or it is hidden. In both cases but especially
			// the second we should ensure that if there is a generated bitmap then we
			// vape it as it is now out of date.
			pCurrentLayer->SetGeneratedBitmap(NULL);

			// Move to the next bitmap in the animation
			pCurrentLayer = pCurrentLayer->FindNextFrameLayer();

			// Go to the next frame
			continue;
		}

		// Set up the visibility of the other layers using this layer
		// as the new temporary and acting active layer
		FrameSGallery::FixOtherLayersFromActive(pCurrentLayer);
			
		// This is where the real work starts!	
		// This is the layer that we want grabbing
		m_pLayer = pCurrentLayer;

		// Check to see if we have a single bitmap on the layer which is stored
		// in a NodeBitmap at a 1:1 scale and is 8bpp or less and the palette matches
		// the present options.
		// If it passes all these tests, then set pBitmap to be that bitmap.
		//if (!CheckIfSingleBitmap(pCurrentLayer, &pBitmap))
		// If there is a referenced bitmap then we can use this instead of generating one
		pBitmap = m_pLayer->GetReferencedBitmap();
		if (pBitmap == NULL)
		{
			// Look to see if we have a bitmap for this layer
			KernelBitmap * pOldBitmap = pCurrentLayer->GetGeneratedBitmap(); 
			
			// Look to see if any visible foreground layers below this one have
			// been edited
			BOOL LayersBelowEdited = FALSE;
			// Only check if we are not already forced to regnerate all frames
			if (!RegenerateAllFrames)
			{
				Layer * pTestLayer = pCurrentLayer;
				pTestLayer = pTestLayer->FindPrevFrameLayer();
				while (pTestLayer)
				{
					// Dont disclude hidden frames as they can affect the make up of the current frame
					if (pTestLayer->IsEdited() && pTestLayer->IsPseudoFrame() &&
						pTestLayer->IsVisible())
					{
						LayersBelowEdited = TRUE;
						break;
					}
					pTestLayer = pTestLayer->FindPrevFrameLayer();
				}
			}

			// we are getting the bitmaps themselves
			// check that the quality it was last captured at was the same as the current quality
			Quality CurrentFrameQuality = pCurrentLayer->GetCaptureQuality();

			// For a number of reasons we may need to regenerate the bitmap for this frame
			// If nay are true then regenerate, otherwise reuse the old one
			if (pOldBitmap == NULL || LayersBelowEdited || RegenerateAllFrames ||
				pCurrentLayer->IsEdited() || CurrentFrameQuality != NewQuality)
			{
				// Turn this frame into a bitmap
				pBitmap = GrabTheFrame();
			}
			else
			{
				// Reuse the old bitmap that was generated
				pBitmap = pOldBitmap;

				// test to see that this bitmap uses the correct palette if required
				// otherwise we will have to grab it anyway
				if (CreatingGlobalPalette)
				{
					// compare palettes
					LOGPALETTE * pGlobalPalette = m_pBitmapFilter->GetGlobalOptimisedPalette();
					if (!pBitmap->GetActualBitmap()->ArePalettesTheSame(pGlobalPalette, 0))
					{
						// NOTE:  This happens when the preview window has just gone and generated a bitmap
						// using a locally optimal palette rather than our globally optimal palette and in
						// the process it has replaced our cache version with its dodgy bitmap.
						TRACEALL( _T("Bum bitmap passed to us with wrong palette, grab a fresh one"));
						pBitmap = GrabTheFrame();
					}
				}
			}
		}

		BOOL bAddLayer = pBitmap != 0;
		if (bAddLayer)
		{
			// We have a bitmap so add this layer pairing to our export list
			bAddLayer = m_ExportParams.AddLayerAndBitmap(pCurrentLayer, pBitmap);
		}

		if (!bAddLayer)
		{
			// We failed to grab the frame or failed to add it to our export list

			// Put back the active layer's visibilty and selection
			Layer::ActiveLayerVisibleAndEditable = CurrentVisEdState;
			FrameSGallery::FixOtherLayersFromActive(m_pLayer);
			// Try and put back the selection that we recorded at the start
			PutBackTheSelection();
			FailAndExecute();
			End();
			return; 
		} 

		//TRACEUSER( "SimonK", _T("colours in frame %d\n"), pBitmap->GetNumPaletteEntries());

		// Note the capture quality that the frame was captured at but only if we worked correctly
		pCurrentLayer->SetCaptureQuality(NewQuality);

		// Move to the next bitmap in the animation
		pCurrentLayer = pCurrentLayer->FindNextFrameLayer();
	}

	// Put back the active layer's visibilty and selection
	Layer::ActiveLayerVisibleAndEditable = CurrentVisEdState;

	// Put back the all visible state
	if (pDoc != NULL)
		pDoc->SetAllVisible(CurrentAllVisibleState);

	Layer *pActiveLayer = m_pSpread->FindActiveLayer();
	if (pActiveLayer)
		FrameSGallery::FixOtherLayersFromActive(pActiveLayer);

	// Go and mark all layers as not edited now that we have finished the grabbing
	// process
	Layer* pLayer = m_pSpread->FindFirstFrameLayer();
	while (pLayer != NULL)
	{
		// This is just as a double check
		if (pLayer->IsPseudoFrame())
		{
			// Clear the edited flag on the layer
			pLayer->SetEdited(FALSE);
#ifdef _DEBUG
			// Tell the frame gallery to update its display of the frame
			BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
#endif
		}

		// Move to the next frame layer in the animation
		pLayer = pLayer->FindNextFrameLayer();
	}

	// If we found a page background layer above, now mark it as not edited
	if (pPageBackground)
		pPageBackground->SetEdited(FALSE);

	// Clean up after the grabbing process
	if (!EndGrab())
	{
		// We failed to clean up after grabbing the frame
		delete m_pBitmapFilter;
		m_pBitmapFilter = NULL;
		FailAndExecute();
		End();
		return; 
	} 


	// Grab all frames should usually play the animation.
	// If the preview dialog is open and in stop mode then it should continue in stop mode.
	// In this case we should have noted this before calling stop at the start of this function
	// So up the play state we noted earlier before we preview
	PreviewDialog * pPreviewDialog = PreviewDialog::GetPreviewDialog();
	INT32 CurrentFrame = 0;
	if (pPreviewDialog != NULL)
	{
		pPreviewDialog->SetPlayAnimation(PlayAnimation);
		CurrentFrame = pPreviewDialog->GetCurrentItem();
	}

	// Fire up the preview dialog box with the bitmaps in our list
	if (!PreviewAnimation())
	{
		// We failed to preview the bitmap
		delete m_pBitmapFilter;
		m_pBitmapFilter = NULL;
		FailAndExecute();
		End();
		return; 
	} 

	// If the dialog is still there then select the new frame to be displayed
	// Default is frame 0 so only do it if the requested item is different to this
	pPreviewDialog = PreviewDialog::GetPreviewDialog();
	if (pPreviewDialog && CurrentFrame != 0)
		pPreviewDialog->SelectCurrentFrame(CurrentFrame);

	// remove the filter as we have no more use for it
	delete m_pBitmapFilter;
	m_pBitmapFilter = NULL;
	
 	End(); 
	return;
}

/********************************************************************************************
// End of OpGrabAllFrames
********************************************************************************************/

//-----------------------------------------------------------------------------------------------
// GrabFrameFilter - Dummy filter, used to render the selection into a bitmap
//-----------------------------------------------------------------------------------------------

/********************************************************************************************

>	GrabFrameFilter::GrabFrameFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/97
	Purpose:	Constructor for an GrabFrameFilter object.  The object should be initialised
				before use.

********************************************************************************************/

GrabFrameFilter::GrabFrameFilter()
{
	pTheBitmap			= NULL;
	m_pLayer			= NULL;
	m_pOptimisedPalette = NULL;
	m_pMkBmpOptions		= NULL;
	m_pCopyMkBmpOptions = NULL;
	m_GrabMode			= GRABMODE_FOROUTPUT;

	// Override this to say something more useful
	ExportingMsgID = _R(IDS_EXPORTINGANIMATEDGIF);	// 	"Exporting animated GIF file..."
}

/********************************************************************************************

>	GrabFrameFilter::GrabFrameFilter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/6/97
	Purpose:	Destructor for an GrabFrameFilter object.
********************************************************************************************/

GrabFrameFilter::~GrabFrameFilter()
{
	if (m_pOptimisedPalette != NULL)
	{
		CCFree(m_pOptimisedPalette);
		m_pOptimisedPalette = NULL;
	}
	if (m_pCopyMkBmpOptions != NULL)
	{
		delete m_pCopyMkBmpOptions;
		m_pCopyMkBmpOptions = NULL;
	}
}

/********************************************************************************************

>	virtual BOOL GrabFrameFilter::GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI,
														   BOOL SnapToBrowserPalette, UINT32 NumColsInPalette,
														   BOOL UsePrimaryCols)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> based on Will code
	Created:	9/6/97
	Inputs:		pSpread				= ptr to spread
				Depth				= The BPP of the bitmap
				DPI					= the dpi of the bitmap
				NumColsInPalette	= prefered number of cols in palette (0 = select maximum allowed)
									  (only applies when Depth == 8)
				UsePrimaryCols		= Put primary colours in palette
									  (only applies when Depth == 8)
				SnapToBrowserPalette= TRUE if the palette should be snapped to the browser palette after generation

	Returns:	TRUE if Palette generated ok
	Purpose:	Generates an optimised palette to use during export
				We override the baseclass form so that we generate one optimised palette for
				all frames if we are in global optimised mode.
				Otherwise, we can just call the base class version.
	SeeAlso:	-

********************************************************************************************/

BOOL GrabFrameFilter::GenerateOptimisedPalette(Spread *pSpread, UINT32 Depth, double DPI,
											   BOOL SnapToBrowserPalette, UINT32 NumColsInPalette, BOOL UsePrimaryCols)

{

	// SMFIX it will pick a valid palette as it goes along
	// optimal palettes are no longer created in this manner
	// sjk

	return TRUE;
}

/********************************************************************************************

>	BOOL GrabFrameFilter::SetLayerBeingCaptured(Layer * pNewLayer)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/97
	Inputs:		pNewLayer the layer which is being captured.
	Purpose:	This remembers the layer that is being captured. It sets the member variable
				up to the specified layer. 

********************************************************************************************/

BOOL GrabFrameFilter::SetLayerBeingCaptured(Layer * pNewLayer)
{
	m_pLayer = pNewLayer;
	return TRUE;
}

/********************************************************************************************

 >	virtual BOOL GrabFrameFilter::SetUpClippingRectangleForExport(Spread *pSpread, SelectionType Selection)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/4/97
	Inputs:		pSpread		the spread to export
				Selection	the selection type to use during export
	Returns:	TRUE if the operation was successful
				FALSE otherwise
	Purpose:	Sets up the class variable ClipRect to be the clipping rectangle to be used
				during export.
	See Also:	PrepareToExport()

********************************************************************************************/

BOOL GrabFrameFilter::SetUpClippingRectangleForExport(Spread *pSpread, SelectionType Selection)
{
	ERROR2IF(pSpread == NULL,FALSE,"SetUpClippingRectangleForExport Bad spread");
	
	// Make an empty rect to start with in the class variable
	ClipRect.MakeEmpty();
	
	// Walk through all the frame layers and work out the bounding boxes of each
	Layer* pCurrentLayer = pSpread->FindFirstFrameLayer();
	Layer* pNextLayer = NULL;
	while (pCurrentLayer != NULL)
	{
		pNextLayer = pCurrentLayer->FindNextFrameLayer();
		// Double check that it is a suitable frame layer and check it is not hidden
		if (pCurrentLayer->IsPseudoFrame())
		{
			// Cannot include a simple hidden check as the purpose of hidden frames is to
			// act as backgrounds for the animation. Could check for hidden and solid but
			// would also need to check for an overlay onto the hidden frame
			if (
				(!pCurrentLayer->IsHiddenFrame()) ||
				(pCurrentLayer->IsHiddenFrame() && pCurrentLayer->IsSolid()) ||
				(pCurrentLayer->IsHiddenFrame() && pNextLayer && pNextLayer->IsOverlay())
				)
			{
				// Get the bounding rect for this layer
				DocRect LayerRect = pCurrentLayer->GetBoundingRect();
				// Add this to our clipping rectangle
				ClipRect = ClipRect.Union(LayerRect);
			}
		}
		
		// Move to the next bitmap in the animation
		pCurrentLayer = pNextLayer;
	}

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL GrabFrameFilter::GetExportOptions(BitmapExportOptions* pOptions)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/97
	Purpose:	See BaseBitmapFilter for interface details

********************************************************************************************/

BOOL GrabFrameFilter::GetExportOptions(BitmapExportOptions* pOptions)
{
	ERROR2IF(pOptions == NULL, FALSE, "pOptions NULL");
	ERROR3IF(!pOptions->IS_KIND_OF(MakeBitmapExportOptions), "pOptions isn't of type MakeBitmapExportOptions");

	m_pMkBmpOptions = (MakeBitmapExportOptions*) pOptions;

	// Determine the filter type currently in use in Accusoft format
	s_FilterType = MAKE_BITMAP_FILTER;

	// Force the selection type to be DRAWING as this should render all visible objects
	m_pMkBmpOptions->SetSelectionType(DRAWING);

	m_pMkBmpOptions->SetDPI(96.0);

	if (m_GrabMode == GRABMODE_PREVIEW)
	{
		// We want a 24bbp bitmap to pass to Gavin's code but using 32bbp to
		// store each pixel (ie the 4 channel of each pixel is waste).  This
		// should do the trick.
		m_pMkBmpOptions->SetDepth(24);
	}
	else
	{
		// Force the depth to be 8bpp
		m_pMkBmpOptions->SetDepth(8);
	}

	// Force the interlacing off
	m_pMkBmpOptions->SetMakeInterlaced(FALSE);

	// Force the transparency to on.
	//m_pMkBmpOptions->SetMakeTransparent(TRUE);

	// recover the relevant animation options from the spread
	if (pSpread)
	{
		DWORD			Loop				= 0;
		DWORD			GlobalDelay			= 10;
		DITHER			Dither				= XARADITHER_ORDERED_GREY;
		WEB_PALETTE		WebPalette			= PAL_GLOBAL;
		PALETTE_COLOURS ColoursPalette		= PALCOL_BROWSER;
		DWORD			NumColsInPalette	= 255;
		BOOL			UseSystemColours	= FALSE;
		// Ask the spread for its version of these values
		pSpread->GetSpreadAnimPropertiesParam(&Loop, &GlobalDelay, &Dither, &WebPalette,
											  &ColoursPalette, &NumColsInPalette, &UseSystemColours, NULL); 

		// set to use background transp if it is set in the animation properties dlg
		m_pMkBmpOptions->SetBackgroundTransparency(pSpread->GetSpreadAnimPropertiesParam().GetIsBackGroundTransp());

		// Ensure the dependent options are set correctly
		// There are 20 colours in the system palette
		switch (ColoursPalette)
		{
			case PALCOL_STANDARD:
			case PALCOL_BROWSER:
				// Fixed browser palette
				// SMFIX we want a browser palette
				m_pMkBmpOptions->SetToUseBrowserPalette(TRUE);
				m_pMkBmpOptions->CreateValidBrowserPalette(TRUE);
				// set this as the palette being used in output dib
				AlterPaletteContents(m_pMkBmpOptions->GetLogicalPalette());
				break;

			case PALCOL_OPTIMIZED:
				// Optimised, so take the user's chosen number of required colours
				// Check if local or globally optimised
/*				if (WebPalette == PAL_GLOBAL)
					TRACEUSER( "SimonK", _T("PAL_GLOBALOPTIMISED\n"));
				else
					TRACEUSER( "SimonK", _T("PAL_OPTIMISED\n"));
*/				//m_pMkBmpOptions->SetNumColsInPalette(NumColsInPalette);
				// store the number of user defined colours in the options
				m_pMkBmpOptions->SetNumberOfUserRequestedColours(NumColsInPalette);
				// In optimised mode we cannot have ordered dithering as it
				// must have a known palette. So switch the dithering to error
				// diffusion.
				if (Dither == XARADITHER_ORDERED_GREY)
					Dither = XARADITHER_ERROR_DIFFUSION;

				// Set up whether we save system colours or not
				//m_pMkBmpOptions->SetUseSystemColours(UseSystemColours);
				//m_pMkBmpOptions->SetToUseSystemPalette(UseSystemColours);
				break;
			default:
				ERROR3("Bad ColoursPalette option");
				break;
		}

		switch (WebPalette)
		{
			case PAL_GLOBAL:
				// If global set then must be browser palette selected
				//TRACEUSER( "SimonK", _T("PAL_GLOBAL\n"));
//				ColoursPalette = PALCOL_BROWSER;
				// use the global palette here
				if (GetGlobalOptimisedPalette())
				{
					m_pMkBmpOptions->CreateValidPalette(GetGlobalOptimisedPalette(),
						m_pMkBmpOptions->IsBackgroundTransparent() ? 0 : -1);
					// set this as the palette being used in output dib
					AlterPaletteContents(m_pMkBmpOptions->GetLogicalPalette());
				}
				break;
			case PAL_LOCAL:
				// If local palette selected then must be optimised as
				// no point saving out a global palette
				//TRACEUSER( "SimonK", _T("PAL_LOCAL\n"));
//				ColoursPalette = PALCOL_OPTIMIZED;
				// SMFIX
				// if we are the first frame generate a new valid palette
				// we could use that as a first guess at a global palette
				// but we do know that if we do not have a global palette we
				// should be getting a different palette for each frame
				m_pMkBmpOptions->InvalidatePalette();
				break;
			default:
				ERROR3("Bad WebPalette option");
				break;
		}

		// Set up the dithering that we are going to use
		m_pMkBmpOptions->SetDither(Dither);
	}


	// We have all the options that we require so don't ask the user for anything

	// Take a copy of these options for latter use (Browser Preview)
	if (m_pCopyMkBmpOptions == NULL)
	{
		m_pCopyMkBmpOptions = (MakeBitmapExportOptions *)m_pMkBmpOptions->MakeCopy();
		// don't complain about null as only required by browser preview at present
	}

	// All ok
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL GrabFrameFilter::SetupExportOptions(BitmapExportOptions* pOptions, Spread * pCurrentSpread)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/97
	Inputs:		pCurrentSpread	The spread we are currently working with
	Purpose:	This is just a public interface on the normal filter's GetExportOptions which is protected.
				It also ensures that the pSpread is set up.

********************************************************************************************/

BOOL GrabFrameFilter::SetupExportOptions(BitmapExportOptions* pOptions, Spread * pCurrentSpread)
{
	// Ensure that the spread pointer is set up
	// If we haven't actually grabbed any frames then it will be NULL
	pSpread = pCurrentSpread;
	// Set up those export options
	return GetExportOptions(pOptions);
}


/********************************************************************************************

>	virtual KernelBitmap* GrabFrameFilter::GetTheBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/97
	Purpose:	Creates a kernel bitmap from the Export RenderRegion
				Overrides the baseclass version as that uses MakeKernelBitmap which does
				check to see if the OIL bitmap pointers are the same but does not check that
				the contents are the same. This means we end up with duplicated versions.
				This is bad. We use TryAndUseExistingBitmap which does this extra check.
				This is ok in the Creat bitmap copy case as this is the only way to generate
				duplicate frames.
	Returns:	Ptr to the bitmap, or NULL if error

Note:
We cannot even do it this way as
a)	the code which checks for duplicates in the global bitmap list complains as the bitmap
	is already attached to the list
b)	even after this, using the TryAndUseExistingBitmap does not reuse the existing bitmap
So try plan c. Add the extra code to ExtractBitmapCopy so when it tries to attach the oilbitmap
it sees if it is reusing an existing bitmap. If so then it will just return the existing bitmap
rather than adding a new duplicate to the system.
********************************************************************************************/

KernelBitmap* GrabFrameFilter::GetTheBitmap()
{
	// Extract a copy of the bitmap from the RenderRegion
	// We need to generate new bitmaps as we delete them and also name them ourselves.
	BOOL LookForDuplicates = FALSE;
	OILBitmap* pOilBitmap = ExportRegion->ExtractBitmapCopy(pOptimisedPalette, pDestBMInfo, pDestBMBytes,
															TransColour, LookForDuplicates);

	if (pOilBitmap == NULL)
	{
		CleanUpAfterExport();
		return NULL;
	}

	if (m_pLayer)
	{
		// Make the bitmap name the layer name
		String_256 Name = m_pLayer->GetLayerID();
		pOilBitmap->SetName(Name);

		// Copy across the delay from the layer
		DWORD delay = m_pLayer->GetFrameDelay();
		pOilBitmap->SetBitmapAnimDelay(delay);

		// As all these frames are complete, we output them using
		// a replace background restore type.
		// It is only later that we may do some trickery on them so that
		// we can overlay them.
		pOilBitmap->SetAnimationRestoreType(GDM_BACKTOBACK);

		// The exception is if the frame is solid in which case we invariably
		// want that one to overlay
		//if (m_pLayer->IsSolid())
		//	pOilBitmap->SetAnimationRestoreType(GDM_LEAVE);
		//else
	}
		
	// Make a kernel bitmap from the oily one
	// The class variable pDocument should be holding the document required
	// Must check it as TryAndUseExistingBitmap does not!
//	ERROR2IF(TheDocument == NULL,NULL,"GrabFrameFilter::GetTheBitmap null document")
//	BOOL IsNew = TRUE;
//	return KernelBitmap::TryAndUseExistingBitmap(pOilBitmap, TheDocument, &IsNew);
	return KernelBitmap::MakeKernelBitmap(pOilBitmap, TheDocument);
}

/********************************************************************************************

>	virtual BOOL GrabFrameFilter::ExportVisibleLayersOnly()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/97
	Inputs:		-
	Outputs:	-
	Returns:	True if this filter wants to exclude invisible layers and so export only
				visible items Or False if want to export everything.
	Purpose:	Determines if the filter wants to export only visible layers.
				In the base bitmap filters class version the default action will be to
				export only those layers which are visible.
	SeeAlso:	Filter::ExportRender;

********************************************************************************************/

BOOL GrabFrameFilter::ExportVisibleLayersOnly()
{
#ifdef DO_EXPORT
	MaskedFilterExportOptions* pMaskedOptions = (MaskedFilterExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pMaskedOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pMaskedOptions->IS_KIND_OF(MaskedFilterExportOptions), "pMaskedOptions isn't");

	ERROR2IF(pSpread == NULL, FALSE,"GrabFrameFilter::VisibleLayersOnly no spread to export");

	// We always want to export oONLY the visible layers.
	// When we are exporting in transparent mode and making up the mask, we must go and set the
	// page background layer to be invisible
	// We are just taking the oppurtunity that this call gives us to fix up the page background
	// layer to be invisible
	if (pMaskedOptions->WantTransparent() && !SecondPass && !GeneratingOptimisedPalette())
	{
		// Masking operation
		// If we find a page background layer then make sure it is invisible
		Layer * pLayer = pSpread->FindFirstPageBackgroundLayer();
		if (pLayer)
			pLayer->SetVisible(FALSE);
	}
	else
	{
		// Normal export operation
		// If we find a page background layer then make sure it is visible
		Layer * pLayer = pSpread->FindFirstPageBackgroundLayer();
		if (pLayer)
			pLayer->SetVisible(TRUE);
	}

	// only include all layer that are visible in this bitmap export
	return TRUE;
#else
	// only include all layer that are visible in this bitmap export
	return TRUE;
#endif
}	


/********************************************************************************************

>	virtual BOOL GrabFrameFilter::ExportSelectionOnly()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/97
	Returns:	True if this filter wants check if an object is selected or not	and so export
				only the selected items Or False if want to export everything.
	Purpose:	Determines if the filter wants to export only selected items.
				In the base bitmap filters class version the default action will be to
				export all objects by default
	SeeAlso:	Filter::ExportRender;

********************************************************************************************/

BOOL GrabFrameFilter::ExportSelectionOnly(BOOL MaskedRender)
{
	// We always export all the objects
	return FALSE;
}

/********************************************************************************************

>	BOOL GrabFrameFilter::IsSecondPassRequired()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/04/97
	Returns:	True if this filter wants a second export rendering pass and so have a chance
				to setup semething like a mask.
	Purpose:	Determines if the a second rendering pass is required or not.

********************************************************************************************/

BOOL GrabFrameFilter::IsSecondPassRequired()
{
#ifdef DO_EXPORT
	MaskedFilterExportOptions* pMaskedOptions = (MaskedFilterExportOptions*)GetBitmapExportOptions();
	ERROR2IF(pMaskedOptions == NULL, FALSE, "NULL Args");
	ERROR3IF(!pMaskedOptions->IS_KIND_OF(MaskedFilterExportOptions), "pMaskedOptions isn't");

	// Only require a second pass if wanting transparency
	if (pMaskedOptions->WantTransparent())
		return TRUE;
	else
		return FALSE;	
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual BOOL GrabFrameFilter::DoExportBitmaps(Operation* pOp, CCLexFile* pFile, PathName* pPath,
												  BitmapExportParam* pParam, BOOL DontShowFileName = FALSE)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/04/97
	Inputs:		pOp - the operation that started the export off
				pDiskFile - the file to put the exported data into
				pPath - the pathname of the file to be exported to
				pParam - the data to use when exporting the bitmaps
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Exports the bitmaps specified by the BitmapExportParam object to a multi-
				image capable filter.

********************************************************************************************/

BOOL GrabFrameFilter::DoExportBitmaps(Operation *pOp, CCLexFile* pFile, PathName* pPath,
									  BitmapExportParam* pParam, BOOL DontShowFileName)
{
#ifdef DO_EXPORT
	ERROR2IF(pOp == NULL || pFile == NULL || pPath == NULL, FALSE,"NULL Parameters");
	ERROR2IF(pParam == NULL,FALSE,"GrabFrameFilter::DoExportBitmaps null BitmapExportParam specified");

	// We could just call the function direct but this may give us some more functionality
	// in the long term
	return SaveExportBitmapsToFile(pFile, pPath, pParam, DontShowFileName);
#endif
	return FALSE;
}


/********************************************************************************************

>	void GrabFrameFilter::SetGlobalOptimisedPalette(LPLOGPALETTE pOptimisedPalette)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/07/97
	Inputs:		pOptimisedPalette - the new global optimised palette
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Sets up a new optimised palette ready to be applied to all frames in the
				animation. Should be used with care.
				At present, only used if we discover that all frames contain compatible
				bitmaps that have the same palette on them.

********************************************************************************************/

void GrabFrameFilter::SetGlobalOptimisedPalette(LPLOGPALETTE pOptimisedPalette)
{
	if (m_pOptimisedPalette != NULL)
	{
		CCFree(m_pOptimisedPalette);
		m_pOptimisedPalette = NULL;
	}

	m_pOptimisedPalette = pOptimisedPalette;
}

/********************************************************************************************

>	virtual UINT32 GrabFrameFilter::GetExportMsgID()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/07/97
	Returns:	The id of the message to put on the progress display whilst exporting.
	Purpose:	Used to get the message id to be used during export.
				Virtual, so that two stage exporters can change the message.
	SeeAlso:	DoExport;

********************************************************************************************/

UINT32 GrabFrameFilter::GetExportMsgID()
{
	// Always return this message regardless of the stage that we are on
	return _R(IDS_GENERATINGFRAMES);		// "Generating animation frames..."
}

/********************************************************************************************
// End of GrabFrameFilter
********************************************************************************************/

//------------------------------------------------------------------------------------------
// GIFAnimationExportParam
//------------------------------------------------------------------------------------------

/********************************************************************************************

>	GIFAnimationExportParam::GIFAnimationExportParam()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Purpose:	Default constructor

********************************************************************************************/

GIFAnimationExportParam::GIFAnimationExportParam()
{
	m_BitmapCount = 0;
	m_pBitmaps = NULL;

	m_LayerCount = 0;
	m_pLayers = NULL;

	m_RegeneratedBitmapPosition = 0;
}

/********************************************************************************************

>	GIFAnimationExportParam::~GIFAnimationExportParam()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Purpose:	Default destructor

********************************************************************************************/

GIFAnimationExportParam::~GIFAnimationExportParam()
{
	// free up the arrays that we grabbed
	if (m_pBitmaps != NULL)
	{
		CCFree(m_pBitmaps);
		m_pBitmaps = NULL;
		m_BitmapCount = 0;
	}
	if (m_pLayers != NULL)
	{
		CCFree(m_pLayers);
		m_pLayers = NULL;
		m_LayerCount = 0;
	}
}

/********************************************************************************************

>	GIFAnimationExportParam::GIFAnimationExportParam(UINT32 Count, KernelBitmap** pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		Count - The number of indexes in the array
				pBitmap - pointer to an array of pointers to kernel bitmaps
	Outputs:	-
	Returns:	-
	Purpose:	Used to pass around lists of bitmaps.  Note that deleting the bitmap array
				is the responsibility of the caller.
********************************************************************************************/

GIFAnimationExportParam::GIFAnimationExportParam(UINT32 Count, KernelBitmap** pBitmap)
{
	ERROR3IF(Count == 0, "Zero bitmap count, that's not right");
	ERROR3IF(pBitmap == NULL, "NULL bitmap array");
	ERROR3IF(!(*pBitmap)->IS_KIND_OF(KernelBitmap), "This dosen't seem to be an array of kernel bitmaps");

	m_BitmapCount = Count;
	m_pBitmaps = pBitmap;

	// set up everything else to defaults
	m_RegeneratedBitmapPosition = 0;
	m_LayerCount = 0;
	m_pLayers = NULL;
}

/********************************************************************************************

>	virtual UINT32 GIFAnimationExportParam::GetBitmapCount()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Returns:	The number of bitmaps in the bitmap array.  (The array runs 0..Count-1)
	Purpose:	For getting the number of bitmaps in the array!

********************************************************************************************/

UINT32 GIFAnimationExportParam::GetBitmapCount()
{
	return m_BitmapCount;
}

/********************************************************************************************

>	virtual KernelBitmap* GIFAnimationExportParam::GetBitmap(UINT32 Index)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		Index - the index of the required bitmap
	Outputs:	-
	Returns:	Pointer to the nth bitmap, NULL if error
	Purpose:	For safely getting a pointer to a bitmap in the bitmap array

********************************************************************************************/

KernelBitmap* GIFAnimationExportParam::GetBitmap(UINT32 Index)
{
	ERROR2IF(Index >= m_BitmapCount, NULL, "Index out of bounds");
	ERROR2IF(m_pBitmaps == NULL, NULL, "NULL array");

	ERROR3IF(!(m_pBitmaps[Index])->IS_KIND_OF(KernelBitmap), "This dosen't seem to be a pointer to a kernel bitmap");
	return m_pBitmaps[Index];
}


/********************************************************************************************

>	virtual UINT32 GIFAnimationExportParam::GetLayerCount()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Returns:	The number of bitmaps/layers in the layers/bitmap array.  (The array runs 0..Count-1)
	Purpose:	For getting the number of layers/bitmaps in the array!

********************************************************************************************/

UINT32 GIFAnimationExportParam::GetLayerCount()
{
	return m_LayerCount;
}

/********************************************************************************************

>	virtual Layer * GIFAnimationExportParam::GetLayer(UINT32 Index)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		Index - the index of the required bitmap
	Outputs:	-
	Returns:	Pointer to the nth bitmap, NULL if error
	Purpose:	For safely getting a pointer to a bitmap in the bitmap array

********************************************************************************************/

Layer* GIFAnimationExportParam::GetLayer(UINT32 Index)
{
	ERROR2IF(Index >= m_LayerCount, NULL, "Index out of bounds");
	ERROR2IF(m_pLayers == NULL, NULL, "NULL array");

	ERROR3IF(!(m_pLayers[Index])->IS_KIND_OF(Layer), "This dosen't seem to be a pointer to a layer");
	return m_pLayers[Index];
}

/********************************************************************************************

>	virtual BOOL GIFAnimationExportParam::AddLayerAndBitmap(Layer * pLayer, KernelBitmap * pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		pLayer	the layer to be added to the list/array
				pBitmap	the bitmap associated with this layer to be added to the list/array
	Returns:	True if worked ok, False otherwise.
	Purpose:	For adding a layer to list ready for exporting the GIF animation.
				Uses the class variables m_pLayers and m_LayerCount.

********************************************************************************************/

BOOL GIFAnimationExportParam::AddLayerAndBitmap(Layer * pLayer, KernelBitmap * pBitmap)
{
	// Let the called functions do the parameter checking!
	return AddLayer(pLayer) && AddBitmap(pBitmap);
}

/********************************************************************************************

>	virtual BOOL GIFAnimationExportParam::AddLayer(Layer * pLayer)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		pLayer	the layer to be added to the list/array
	Outputs:	-
	Returns:	True if worked ok, False otherwise.
	Purpose:	For adding a layer to list ready for exporting the GIF animation.
				Uses the class variables m_pLayers and m_LayerCount.

********************************************************************************************/

BOOL GIFAnimationExportParam::AddLayer(Layer * pLayer)
{
	ERROR2IF(pLayer == NULL,FALSE,"AddLayer given null layer")
	BOOL ok = TRUE;

	// If we have a null list then just allocate the first item
	if (m_pLayers == NULL)
	{
		// Claim selected space
		m_pLayers = (Layer**) CCMalloc(sizeof(Layer*));
		if (m_pLayers == NULL)
			ok = FALSE;
		else
			m_pLayers[0] = pLayer;
		m_LayerCount = 1;
	}
	else
	{
		// Add to end of selection
		Layer** pTemp = (Layer**) CCRealloc(m_pLayers, (m_LayerCount + 1) * sizeof(Layer*));
		if (pTemp == NULL)
			ok = FALSE;
		else
		{
			m_pLayers = pTemp;
			m_pLayers[m_LayerCount] = pLayer;
		}
		m_LayerCount++;
	}

	return ok;
}

/********************************************************************************************

>	BOOL GIFAnimationExportParam::AddBitmap(KernelBitmap * pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		pBitmap	the bitmap to add to our list in the animation
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to add the bitmap to the list.
				Uses the class variables m_pBitmaps and m_BitmapCount.
				Copied from BitmapSGallery and BitmapExportParam way of doing things so that
				we are compatible. Not the best way of doing it!
	SeeAlso:	BitmapSGallery::GetSelectedBitmaps

********************************************************************************************/

BOOL GIFAnimationExportParam::AddBitmap(KernelBitmap * pBitmap)
{
	ERROR2IF(pBitmap == NULL,FALSE,"Addbitmap given null bitmap")
	BOOL ok = TRUE;

	// If we have a null list then just allocate the first item
	if (m_pBitmaps == NULL)
	{
		// Claim selected space
		m_pBitmaps = (KernelBitmap**) CCMalloc(sizeof(KernelBitmap*));
		if (m_pBitmaps == NULL)
			ok = FALSE;
		else
			m_pBitmaps[0] = pBitmap;
		m_BitmapCount = 1;
	}
	else
	{
		// Add to end of selection
		KernelBitmap** pTemp = (KernelBitmap**) CCRealloc(m_pBitmaps, (m_BitmapCount + 1) * sizeof(KernelBitmap*));
		if (pTemp == NULL)
			ok = FALSE;
		else
		{
			m_pBitmaps = pTemp;
			m_pBitmaps[m_BitmapCount] = pBitmap;
		}
		m_BitmapCount++;
	}

	return ok;
}

/********************************************************************************************

>	BOOL GIFAnimationExportParam::SetAnimPropertiesParam(AnimPropertiesParam* pParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Inputs:		pParam - Sets the Animation Properties details for this GIFAnimationExportParam.
	Returns:	TRUE if ok, FALSE otherwise.
	
********************************************************************************************/

void GIFAnimationExportParam::SetSpreadAnimPropertiesParam(const AnimPropertiesParam& Param)
{
	m_AnimPropertiesParam = Param;
}

/********************************************************************************************

>	void GIFAnimationExportParam::GetAnimPropertiesParam()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/05/97
	Purpose:	Returns the Animation Properties details for this GIFAnimationExportParam.
		
********************************************************************************************/

AnimPropertiesParam& GIFAnimationExportParam::GetSpreadAnimPropertiesParam()
{
	return m_AnimPropertiesParam;
}

/********************************************************************************************

>	DocRect GIFAnimationExportParam::GetBoundingRect() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/97
	Purpose:	Returns the bounding rectangle of the animation.
		
********************************************************************************************/

DocRect GIFAnimationExportParam::GetBoundingRect() const
{
	// Recover the bounding rect of the animation from our copy of the spread's
	// animation properties.
	DocRect BoundingRect = m_AnimPropertiesParam.GetBoundingRect();
	// We are only interested in the size and so translate it to zero
	BoundingRect.Translate(-BoundingRect.lo.x, -BoundingRect.lo.y);
	// return it to the caller
	return BoundingRect;
}


//------------------------------------------------------------------------------------------
// end of GIFAnimationExportParam
//------------------------------------------------------------------------------------------

/********************************************************************************************
// OpSaveAnimatedGIF
********************************************************************************************/

/********************************************************************************************

>	OpState	OpSaveAnimatedGIF::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/9/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpSaveAnimatedGIF operation
	Purpose:	For finding the OpSaveAnimatedGIF's state. 

********************************************************************************************/

OpState	OpSaveAnimatedGIF::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
 	// else just use the base class version
	return OpGrabAllFrames::GetState(pUIDescription, pOpDesc);
}

/********************************************************************************************

>	BOOL OpSaveAnimatedGIF::GetFileNameFromUser()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Gives the operation chance to get the filename from the user.
				In this version we need to do this before grabbing the frames so that the
				user is given the oppurtunity to change the options using the button on the
				file save dialog box.

********************************************************************************************/

BOOL OpSaveAnimatedGIF::GetFileNameFromUser()
{
	// This is used to export an animated GIF. So we need only show the normal
	// GIF filter. This is despite the fact that this is not going to be the filter
	// that we use for the job.
	
	// We are just going to show the animated GIF filter and all files
	String_256 FilterString(_R(IDS_SAVEGIFANIFILTERS));

	// Ensure that all the filters have a -1 in their position string so any matching
	// of selected filter will fail. Make only the GIF filter have 1 in there.
	Filter* pFilter = Filter::GetFirst();
	while (pFilter != NULL)
	{
		if (IS_A(pFilter, TI_GIFFilter))
		{
			// Its the GIF filter we want, mark it as the first position
			pFilter->pOILFilter->Position = 1;
		}
		else
		{
			// Make sure we don't try this one later
			pFilter->pOILFilter->Position = -1;
		}

		// Try the next filter
		pFilter = Filter::GetNext(pFilter);
	}

	// Create the dialog box
	GIFExportFileDialog FDialog(FilterString);
	
	FDialog.SetTitle(_R(IDS_SAVEGIFANI_TITLE));

	// Use the document name as the default name of animation
	String_256 DefaultName(_R(IDS_BUTTBAR_ANIMATION));	// GIF Animation
	if (m_pDocument)
		DefaultName = m_pDocument->GetDocName(FALSE);
	// This is the pathname that we are going to save to
	m_SavePath = DefaultName;
	// Ensure that we have the correct extension on the filename we have allocated
	EnsureFileType(&m_SavePath);

	// Pass this name onto the dialog box
	DefaultName = m_SavePath.GetFileName();
	FDialog.SetDefaultFileName(DefaultName);
	
	//FDialog.SetSelectedFilterIndex(1);

	// Select the desired path
	String_256 ExportFilterPath	= OpMenuExport::DefaultExportFilterPath;
	//Camelot.GetPrefDirect("Filters", "DefaultExportFilterPath",&ExportFilterPath);
	if (ExportFilterPath.Length() > 0)
		FDialog.SetInitialDirectory(ExportFilterPath);

	// 'Do' the dialog and get that filename that we require
	BOOL DlgResult = FDialog.OpenAndGetFileName();
	if (!DlgResult)
	{
		return FALSE;
	}

	// Get the filename.
	FDialog.GetChosenFileName(&m_SavePath);

	// This is now taken care of in FileDlgs.cpp on the ok as it then checks if the file
	// exists or not. If we do it here then this does not happen.
	// Unfortunately, changes made to the string in the dialog do not filter through
	// and so we ust fix it up here as well.
	// Always make sure that the filter's default extension is on if the user has not
	// specified an extension.
	EnsureFileType(&m_SavePath);

	// Ensure that the path is valid
	if (!m_SavePath.IsValid())
	{
		InformError();
		//delete m_pBitmapFilter;
		//m_pBitmapFilter = NULL;
		return FALSE;
	}

	// Extract directory name (minus terminating backslash) and remember for next time.
	ExportFilterPath = m_SavePath.GetLocation(FALSE);
	if (ExportFilterPath.Length() > 0)
		OpMenuExport::DefaultExportFilterPath = ExportFilterPath;
		//Camelot.SetPrefDirect("Filters", "DefaultExportFilterPath",DefaultExportFilterPath, TRUE);
			
	//UINT32 TheSelectedFilterID = m_pBitmapFilter->FilterID;

	// Everything got so return to the grabbing process
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL OpSaveAnimatedGIF::SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/97
	Inputs:		pPreviewDialog	the preview dialog box, if open
				pExportParam	the export param which we will use
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to actually do the preview operation required.
				In this case we just save the animation out to the file that the user specified.

********************************************************************************************/

BOOL OpSaveAnimatedGIF::SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam)
{
	// A bit of checking on parameters
	if (pExportParam != NULL)
	{
		ERROR3IF(!pExportParam->IS_KIND_OF(BitmapExportParam), "OpParam passed is not a BitmapExportParam");

		if (pExportParam->GetBitmapCount() == 0)
		{
			ERROR2RAW("OpSaveAnimatedGIF::DoWithParam no bitmaps to export");
			return FALSE;
		}				
	}
	else
	{
		ERROR2RAW("OpSaveAnimatedGIF::DoWithParam bad params!");
		return FALSE;
	}

	// Recover the bitmap pointer from the parameter passed in
	// This is to ensure that we have at least one bitmap to save
	KernelBitmap* pTheBitmap = pExportParam->GetBitmap(0);
	if (pTheBitmap == NULL)
	{
		ERROR2RAW("OpSaveAnimatedGIF::DoWithParam bitmap pointer is null");
		return FALSE;
	}

	// Ensure that we have a special grab frame filter available
	if (m_pBitmapFilter == NULL)
		m_pBitmapFilter = new GrabFrameFilter();
	if (m_pBitmapFilter == NULL)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		return FALSE;
	}

	// Should really check that the pathname is not null
	String_256 Filename;
	Filename = m_SavePath.GetPath();
	ERROR2IF(Filename.IsEmpty(),FALSE,"OpSaveAnimatedGIF::SaveAnimation Save path not set up!");

	// First off, we have to try and open the file
	CCDiskFile DiskFile(1024, FALSE, TRUE);

	BOOL ExportedOk = TRUE;
	TRY
	{
		// Export bitmaps
		ExportedOk = m_pBitmapFilter->DoExportBitmaps(this, &DiskFile, &m_SavePath, pExportParam);

		if (!ExportedOk)
		{
			// Something went a bit wrong - tell the user what it was.
			// Supress the error if it was the 'user has cancelled one'
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			{
				InformError();
				m_pBitmapFilter->DeleteExportFile(&DiskFile);
			}
			else
			{
		 		// otherwise remove the error so it won't get reported
			 	Error::ClearError();
			}
		}

		// close the file
		if (DiskFile.isOpen())
			DiskFile.close();
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
		{
			InformError();
		}
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported

		// Make sure that the file is closed and deleted
		TRY
		{
			// First try and delete it (tries to close it first)
			if (m_pBitmapFilter)
				m_pBitmapFilter->DeleteExportFile(&DiskFile);

			// Double check to make sure it is closed.
			if (DiskFile.isOpen())
				DiskFile.close();
		}
		CATCH(CFileException, e)
		{
			// Failed to close the file - not much we can do about it really
		}
		END_CATCH

		// Fail
		ExportedOk = FALSE;
	}
	END_CATCH

	// Clean out the filter we created
	delete m_pBitmapFilter;
	m_pBitmapFilter = NULL;

	// Blank the path
	//m_SavePath.SetPathName(TEXT(""));
	// This will give us a nice pathname is invalid error! Don't you just love the pathname class!

	return ExportedOk;
}

/********************************************************************************************

>	BOOL OpSaveAnimatedGIF::EnsureFileType(PathName * pPath)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/97
	Inputs:		pPath	the pathname in use
	Purpose:	Ensures that the pathname has the file extension/type that we require.

********************************************************************************************/

BOOL OpSaveAnimatedGIF::EnsureFileType(PathName * pPath)
{
	ERROR2IF(pPath == NULL,FALSE,"EnsureFileExtension bad path specified")

	// we are essentially a GIF filter so we need that extension
	String_32 ExtStr(_R(IDN_FILTEREXT_GIF));
	ExtStr.toLower();
	String_256 Extension = pPath->GetType();
	Extension.toLower();
	if (Extension.IsEmpty() || Extension != ExtStr)
	{
		// There is no extension set for this file, so give it one
		pPath->SetType(ExtStr);
	}

	return TRUE;
} 

/********************************************************************************************
// end of OpSaveAnimatedGIF
********************************************************************************************/

//------------------------------------------------------------------------------------------
// Methods for the OpBrowserPreview
//------------------------------------------------------------------------------------------

// Init the static that we have
PathName	OpBrowserPreview::m_TempPath;

/********************************************************************************************

>	BOOL OpBrowserPreview::GetFileNameFromUser()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Returns:	True if worked ok, False otherwise
	Purpose:	Gives the operation chance to get the filename from the user.
				In this version we need to override the OpSaveAnimation's version so that
				we do nothing.

********************************************************************************************/

BOOL OpBrowserPreview::GetFileNameFromUser()
{
	// This version, do nothing
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL OpBrowserPreview::SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/8/97
	Inputs:		pPreviewDialog	the preview dialog box, if open
				pExportParam	the export param which we will use
	Returns:	True if worked ok, False otherwise
	Purpose:	Tries to actually do the preview operation required.
				In this case we just save the animation out to a temporary file and then fire
				up the browser with an html wrapper which references that file.

********************************************************************************************/

BOOL OpBrowserPreview::SaveOrShowTheAnimation(PreviewDialog * pPreviewDialog, BitmapExportParam* pExportParam)
{
	// A bit of checking on parameters
	if (pExportParam != NULL)
	{
		ERROR3IF(!pExportParam->IS_KIND_OF(BitmapExportParam), "OpParam passed is not a BitmapExportParam");

		if (pExportParam->GetBitmapCount() == 0)
		{
			ERROR2RAW("OpSaveAnimatedGIF::DoWithParam no bitmaps to export");
			return FALSE;
		}				
	}
	else
	{
		ERROR2RAW("OpSaveAnimatedGIF::DoWithParam bad params!");
		return FALSE;
	}

	// Go and preview it in a browser.
	// Get our saving filter
	if (m_pBitmapFilter == NULL)
		m_pBitmapFilter = new GrabFrameFilter();
	if (m_pBitmapFilter == NULL)
	{
		InformError(_R(IDT_CANT_FIND_FILTER));
		return FALSE;
	}
	
	// create a disk file
	CCDiskFile TempDiskFile(1024, FALSE, TRUE);

	// Ensure that if we have been using a previous temp file then
	// it is cleaned up
	RemoveTempFile();

	// Create the required unique temporary of the correct filetype
	String_32 ExtStr(_R(IDN_FILTEREXT_GIF));
	ExtStr.toLower();
	if (!FileUtil::GetTemporaryPathName(ExtStr, &m_TempPath))
		return FALSE;

	// Ensure that we have the correct extension on the filename we have allocated
	EnsureFileType(&m_TempPath);

	// First off, we have to try and open the file
	CCDiskFile DiskFile(1024, FALSE, TRUE);

	BOOL ExportedOk = TRUE;
	TRY
	{
		// Export bitmaps supressing the filename so that the user does not see the temp filename
		ExportedOk = m_pBitmapFilter->DoExportBitmaps(this, &TempDiskFile, &m_TempPath, &m_ExportParams, TRUE);

		if (!ExportedOk)
		{
			// Something went a bit wrong - tell the user what it was.
			// Supress the error if it was the 'user has cancelled one'
			if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
			{
				InformError();
				m_pBitmapFilter->DeleteExportFile(&DiskFile);
			}
			else
			{
		 		// otherwise remove the error so it won't get reported
			 	Error::ClearError();
			}
		}

		// close the file
		if (DiskFile.isOpen())
			DiskFile.close();
	}

	// See if there was a file io error
	CATCH(CFileException, e)
	{
		// Report the error if no one else did
		if (Error::GetErrorNumber() != _R(IDN_USER_CANCELLED))
		{
			InformError();
		}
		else
			Error::ClearError();	// otherwise remove the error so it won't get reported

		// Make sure that the file is closed and deleted
		TRY
		{
			// First try and delete it (tries to close it first)
			if (m_pBitmapFilter)
				m_pBitmapFilter->DeleteExportFile(&DiskFile);

			// Double check to make sure it is closed.
			if (DiskFile.isOpen())
				DiskFile.close();
		}
		CATCH(CFileException, e)
		{
			// Failed to close the file - not much we can do about it really
		}
		END_CATCH

		// Fail
		ExportedOk = FALSE;
	}
	END_CATCH

	///------------------------------------
	// Now we have the file, we can go and preview it in the browser
	
	// Allocate our preview class, must keep it around as destruction causes
	// it to clean up the temp files, which is bad if this is before it has
	// had a chance to fire up the browser!
	if (m_pBitmapData == NULL)
	{
		m_pBitmapData = new BitmapPreviewData;
		if (m_pBitmapData == NULL)
			return FALSE;
	} 
	// Now a static pointer as otherwise the temp file is deleted too quickly
	
	MakeBitmapExportOptions * pBmpOptions = NULL;
	// Recover the options from the filter that it saved during the preparation of the bitmaps
	// They are helpfully vaped by now! So, we took a copy during GetExportOptions
	// But if all the frame are already captured then we will not have any options
	pBmpOptions = m_pBitmapFilter->GetCurrentExportOptionsCopy();
	BOOL MadeBitmapOptions = FALSE;
	if (pBmpOptions == NULL)
	{
		// Get the filter to set up the bitmap options for us (Use out public version of CreateExportOptions)
		pBmpOptions = m_pBitmapFilter->MakeExportOptions();
		// Use our public interface onto the GetExportOptions protected function
		if (pBmpOptions != NULL)
		{
			MadeBitmapOptions = TRUE;
			m_pBitmapFilter->SetupExportOptions(pBmpOptions, m_pSpread);
		}
	}
	ERROR2IF(pBmpOptions == NULL,FALSE,"OpBrowserPreview::PreviewAnimation No current bitmap options");

	// Get the first bitmap in the list, this will give us the size of the animation
	KernelBitmap* pTheBitmap = pExportParam->GetBitmap(0);
	// Get the filesize that we have exported
	UINT32 FileSize = TempDiskFile.Size();

	// tell the options about the temp file that we are using
	pBmpOptions->SetPathName(&m_TempPath);

	// This is our copy of the BitmapExportParam
	m_pBitmapData->SetNewBitmap(pTheBitmap, FileSize, pBmpOptions);

	// Set up the options that we require
/*	BrowserBackground Bgr	= BROWSER_BGR_DOC; //BROWSER_BGR_CHECKER;
	BOOL ShowInfo			= TRUE;
	BOOL GenerateImagemap	= FALSE;
*/
	// Get the values from our global variables.
	BrowserBackground Bgr	= PreviewInBrowserTab::g_Background;
	BOOL ShowInfo			= PreviewInBrowserTab::g_InfoInHtmlStub;
	BOOL GenerateImagemap	= FALSE;	

	BrowserPreviewOptions PreviewOptions(Bgr, ShowInfo, GenerateImagemap);

	//BrowserPreviewOptions PreviewOptions(PreviewInBrowserTab::GetBrowserOptions());

	// tell it what spread to use when getting the document background
	PreviewOptions.SetSpread(m_pSpread);
	
	// This should actually preview it in the browser
	m_pBitmapData->GenerateHTMLStub(PreviewOptions);
	

	// Clean out the filter we created
	delete m_pBitmapFilter;
	m_pBitmapFilter = NULL;
	
	if (MadeBitmapOptions && pBmpOptions != NULL)
		delete pBmpOptions;

	return ExportedOk;
}

/********************************************************************************************

>	OpState	OpBrowserPreview::GetState(String_256*, OpDescriptor*)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/97
	Inputs:		Name of the OpDescriptor being queried
	Outputs:	The string to show to the user
	Returns:	The state of the OpBrowserPreview operation
	Purpose:	For finding the OpBrowserPreview's state. 

********************************************************************************************/

/* OpState	OpBrowserPreview::GetState(String_256 * pUIDescription, OpDescriptor * pOpDesc)
{
	// This is always unavailable for the present moment
	OpState OpSt;
	OpSt.Greyed = TRUE; 
 	return OpSt;   
} */

/********************************************************************************************

>	static BOOL OpBrowserPreview::RemoveTempFile()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/7/97
	Returns:	True if worked correctly, False otherwise.
	Purpose:	For cleaning up any temp file useage that we may have made.
				This should be called in the DeInit during program shut down.

********************************************************************************************/

BOOL OpBrowserPreview::RemoveTempFile()
{
	// If we have used the m_TempPath during this session then ensure that we have cleaned up
	// any temp files
	String_256 Filename;
	Filename = m_TempPath.GetPath();
	if (!Filename.IsEmpty() && m_TempPath.IsValid())
	{	
		FileUtil::DeleteFile(&m_TempPath);
	}

	return TRUE;
}

/********************************************************************************************
// End of OpBrowserPreview
********************************************************************************************/


