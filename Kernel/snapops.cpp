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
// Implement the ops that are associated with snapping (e.g. the "snap to grid" menu op)

/*
*/

#include "camtypes.h"
#include "snapops.h"
//#include "markn.h"
#include "docview.h"
#include "tool.h"
#include "grid.h"
#include "bars.h"
//#include "barsdlgs.h"
#include "document.h"
#include "layer.h"
#include "spread.h"
//#include "sglayer.h"
#include "sprdmsg.h"
#include "bubbleid.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(OpShowGrid, 		Operation)
CC_IMPLEMENT_DYNCREATE(OpSnapToGrid, 	Operation)
// WEBSTER - markn 15/1/97
// Removed some op that no longer used in Webster
#ifndef WEBSTER
CC_IMPLEMENT_DYNCREATE(OpShowGuides, 	Operation)
CC_IMPLEMENT_DYNCREATE(OpSnapToObjects, Operation)
CC_IMPLEMENT_DYNCREATE(OpSnapToGuides,  Operation)
#endif

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	static BOOL OpShowGrid::Init()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for show grid toggle op
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpShowGrid::Init()
{
	return RegisterOpDescriptor(	0,							// Tool ID
									_R(IDS_SHOWGRID),				// String resource ID
									CC_RUNTIME_CLASS(OpShowGrid),// Runtime class
									OPTOKEN_SHOWGRID,			// Token string
									GetState,					// GetState function									0,							// help ID
									0,							// help ID
									_R(IDBBL_SHOWGRIDOP),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_BTN_WINDOWGRID),			// control ID
									SYSTEMBAR_WINDOW,			// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
}

/********************************************************************************************

>	OpShowGrid::OpShowGrid()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpShowGrid operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpShowGrid::OpShowGrid()
{
}

/********************************************************************************************

>	void OpShowGrid::Do(OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Forces a redraw of the selected DocView and toggles the NodeGrid::RenderFlag flag
	Errors:		None

********************************************************************************************/

void OpShowGrid::Do(OpDescriptor*)
{
	DocView *pDocView = DocView::GetSelected();

	if (pDocView != NULL)
	{
		if (Tool::GetCurrentID() != TOOLID_GRID)
			NodeGrid::ForceRedrawAllGrids();

		pDocView->SetShowGridState(!pDocView->GetShowGridState());
	}

	End();
}

/********************************************************************************************

>	OpState OpShowGrid::GetState(String_256*, OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpShowGrid::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		OpSt.Ticked = pDocView->GetShowGridState();

	return OpSt;
}


//--------------------------------------------------------------------------------------------


/********************************************************************************************

>	static BOOL OpSnapToGrid::Init()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for snap to grid toggle op
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpSnapToGrid::Init()
{
	return RegisterOpDescriptor(0,							// tool id
								_R(IDS_SNAPTOGRID),				// String resource ID
								CC_RUNTIME_CLASS(OpSnapToGrid),// Runtime class
								OPTOKEN_SNAPTOGRID,			// Token string
								GetState,					// GetState function								0,							// help ID 
								0,								// help ID
								_R(IDBBL_SNAPTOGRIDOP),			// bubble help
								_R(IDD_BARCONTROLSTORE),		// resource ID
								_R(IDC_BTN_WINDOWSNAP),			// control ID
								SYSTEMBAR_WINDOW,			// Bar ID
								TRUE,						// Recieve system messages
								FALSE,						// Smart duplicate operation
								TRUE,						// Clean operation
								0,							// No vertical counterpart
								0,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
							  );
}

/********************************************************************************************

>	OpSnapToGrid::OpSnapToGrid()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpSnapToGrid operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpSnapToGrid::OpSnapToGrid()
{
}

/********************************************************************************************

>	void OpSnapToGrid::Do(OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles the "snap to grid" flag in the selected DocView
	Errors:		None

********************************************************************************************/

void OpSnapToGrid::Do(OpDescriptor*)
{
	DocView *pDocView = DocView::GetSelected();

	if (pDocView != NULL)
		pDocView->SetSnapToGridState(!pDocView->GetSnapToGridState());

	End();
}

/********************************************************************************************

>	OpState OpSnapToGrid::GetState(String_256*, OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpSnapToGrid::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		OpSt.Ticked = pDocView->GetSnapToGridState();
	
	return OpSt;
}

//--------------------------------------------------------------------------------------------

// WEBSTER - markn 15/1/97
// Removed some op that no longer used in Webster
#ifndef WEBSTER


/********************************************************************************************

>	static BOOL OpSnapToObjects::Init()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for snap to objects toggle op
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpSnapToObjects::Init()
{
	return RegisterOpDescriptor(0,							// Tool ID
								_R(IDS_SNAPTOOBJECTS),			// String resource ID
								CC_RUNTIME_CLASS(OpSnapToObjects),// Runtime class
								OPTOKEN_SNAPTOOBJECTS,		// Token string
								GetState,					// GetState function
								0,							// help ID 
								_R(IDBBL_SNAPTOOBJECTS),		// Bubble ID 
								_R(IDD_BARCONTROLSTORE),		// resource ID
								0,							// control ID
								SYSTEMBAR_WINDOW,			// Bar ID
								TRUE,						// Recieve system messages
								FALSE,						// Smart duplicate operation
								TRUE,						// Clean operation
								0,							// No vertical counterpart
								0,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
							  );
}

/********************************************************************************************

>	OpSnapToObjects::OpSnapToObjects()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpSnapToObjects operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpSnapToObjects::OpSnapToObjects()
{
}

/********************************************************************************************

>	void OpSnapToObjects::Do(OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles the "snap to objects" flag in the selected DocView
	Errors:		None

********************************************************************************************/

void OpSnapToObjects::Do(OpDescriptor*)
{
	DocView *pDocView = DocView::GetSelected();

	if (pDocView != NULL)
	{
		// Find out the new value for the magnetic snapping
		BOOL NewState = !pDocView->GetSnapToObjectsState();
		pDocView->SetSnapToObjectsState(NewState);

		// update all slider controls that show the view quality
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_SNAPTOOBJECTS);
		if (pOpDesc!=NULL)
		{
			// Found the opdescriptor. Now find all the gadgets associated with it
			List Gadgets;
			if (pOpDesc->BuildGadgetList(&Gadgets))
			{
				// Found some. Set the controls position according to the quality
				GadgetListItem* pGadgetItem = (GadgetListItem*) Gadgets.GetHead();

				while (pGadgetItem!=NULL)
				{
					// Set the sliders position
					pGadgetItem->pDialogOp->SetLongGadgetValue(pGadgetItem->gidGadgetID, NewState, FALSE);

					// go find the next gadget
					pGadgetItem = (GadgetListItem*) Gadgets.GetNext(pGadgetItem);
				}
		
				// Clean out the list
				Gadgets.DeleteAll();
			}
		}
	}

	End();
}

/********************************************************************************************

>	OpState OpSnapToObjects::GetState(String_256*, OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpSnapToObjects::GetState(String_256*, OpDescriptor*)
{
	OpState OpSt;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		OpSt.Ticked = pDocView->GetSnapToObjectsState();
	
	return OpSt;
}

//--------------------------------------------------------------------------------------------

/********************************************************************************************

>	static BOOL OpSnapToGuides::Init()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for snap to guides toggle op
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpSnapToGuides::Init()
{
	return RegisterOpDescriptor(0,							// Tool ID
								_R(IDS_SNAPTOGUIDES),			// String resource ID
								CC_RUNTIME_CLASS(OpSnapToGuides),// Runtime class
								OPTOKEN_SNAPTOGUIDES,		// Token string
								GetState,					// GetState function
								0,							// help ID 
								_R(IDBBL_SNAPTOGUIDES),			// Bubble ID 
								_R(IDD_BARCONTROLSTORE),		// resource ID
								0,							// control ID
								SYSTEMBAR_WINDOW,			// Bar ID
								TRUE,						// Recieve system messages
								FALSE,						// Smart duplicate operation
								TRUE,						// Clean operation
								0,							// No vertical counterpart
								0,							// String for one copy only error
								(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
							  );
}

/********************************************************************************************

>	OpSnapToGuides::OpSnapToGuides()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpSnapToGuides operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpSnapToGuides::OpSnapToGuides()
{
}

/********************************************************************************************

>	void OpSnapToGuides::Do(OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Toggles the "snap to guides" flag in the selected DocView
	Errors:		None

********************************************************************************************/

void OpSnapToGuides::Do(OpDescriptor*)
{
	DocView *pDocView = DocView::GetSelected();

	if (pDocView != NULL)
		pDocView->SetSnapToGuidesState(!pDocView->GetSnapToGuidesState());

	End();
}

/********************************************************************************************

>	OpState OpSnapToGuides::GetState(String_256*, OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpSnapToGuides::GetState(String_256* pUIDescription, OpDescriptor*)
{
	OpState OpSt;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		OpSt.Ticked = pDocView->GetSnapToGuidesState();

	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
	{
		// if we have a guide layer, then make the menu item available, if not, grey it 
		//	and give a reason.
		if (pSpread->FindFirstGuideLayer() != NULL)
		{
			OpSt.Greyed = FALSE;
		}
		else
		{
			OpSt.Greyed = TRUE;
			*pUIDescription = String_256 (_R(IDS_NO_GUIDES));
		}
	}
	
	return OpSt;
}

//--------------------------------------------------------------------------------------------

/********************************************************************************************

>	static BOOL OpShowGuides::Init()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Declares op descriptors for show grid toggle op
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL OpShowGuides::Init()
{
	return RegisterOpDescriptor(	0,							// Tool ID
									_R(IDS_SHOWGUIDES),				// String resource ID
									CC_RUNTIME_CLASS(OpShowGuides),// Runtime class
									OPTOKEN_SHOWGUIDES,			// Token string
									GetState,					// GetState function									0,							// help ID
									0,							// help ID
									_R(IDBBL_SHOWGUIDES),			// bubble help
									_R(IDD_BARCONTROLSTORE),		// resource ID
									_R(IDC_BTN_WINDOWGRID),			// control ID
									SYSTEMBAR_WINDOW,			// Bar ID
									TRUE,						// Recieve system messages
									FALSE,						// Smart duplicate operation
									TRUE,						// Clean operation
									0,							// No vertical counterpart
									0,							// String for one copy only error
									(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC) // Auto state flags
								);
}

/********************************************************************************************

>	OpShowGuides::OpShowGuides()

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	Constructor for OpShowGuides operation. It is not undoable.
	Errors:		None

********************************************************************************************/

OpShowGuides::OpShowGuides()
{
}

/********************************************************************************************

>	void OpShowGuides::Do(OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Forces a redraw of the selected DocView and toggles the view's ShowGuides flag
	Errors:		None

********************************************************************************************/

void OpShowGuides::Do(OpDescriptor*)
{
	DocView *pDocView = DocView::GetSelected();

	if (pDocView != NULL)
	{
		pDocView->SetShowGuidesState(!pDocView->GetShowGuidesState());

		Spread* pSpread = Document::GetSelectedSpread();
		Document* pDoc  = Document::GetSelected();
		if (pSpread != NULL && pDoc != NULL)
		{
PORTNOTE("gallery", "Removed use of Gallery")
#if !defined(EXCLUDE_FROM_XARALX)
			Layer* pLayer = pSpread->FindFirstGuideLayer();
			if (pLayer != NULL)
				LayerSGallery::ForceRedrawLayer(pDoc,pLayer);
#endif
		}

		BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
	}

	End();
}

/********************************************************************************************

>	OpState OpShowGuides::GetState(String_256*, OpDescriptor*)

	Author: 	Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Inputs:
	Outputs:
	Returns:
	Purpose:	This item is always available, so long as a document is visible.
	Errors:		None

********************************************************************************************/

OpState OpShowGuides::GetState(String_256* pUIDescription, OpDescriptor*)
{
	OpState OpSt;

	DocView *pDocView = DocView::GetSelected();
	if (pDocView != NULL)
		OpSt.Ticked = pDocView->GetShowGuidesState();

	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
	{
		// if we have a guide layer, then make the menu item available, if not, grey it 
		//	and give a reason.
		if (pSpread->FindFirstGuideLayer() != NULL)
		{
			OpSt.Greyed = FALSE;
		}
		else
		{
			OpSt.Greyed = TRUE;
			*pUIDescription = String_256 (_R(IDS_NO_GUIDES));
		}

	}	// if (pSpread != NULL)

	return OpSt;
}

#endif // WEBSTER
