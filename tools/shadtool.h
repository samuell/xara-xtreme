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
// Header for the Shadow Tool implementation

#ifndef INC_SOFTSHADOWTOOL
#define INC_SOFTSHADOWTOOL

#include "nodecont.h"

#ifdef BUILDSHADOWS

//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "shadinfo.h"
#include "dragtool.h"
//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opliveeffects.h"

class Cursor;
class Spread;
class SoftShadowInfoBarOp;
class SoftShadowTool;
class PPStackLevel;

#define OPTOKEN_DRAGCREATESHADOW _T("DragCreateShadow")


/********************************************************************************************

>	class SoftShadowTool : public Tool_v1

	Author:		Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/96
	Purpose:	A template tool class

********************************************************************************************/
class CCAPI SoftShadowTool : public DragTool
{
// Give tool name in memory dumps
CC_DECLARE_MEMDUMP(SoftShadowTool);

public:
	SoftShadowTool();
	~SoftShadowTool();

	// Standard calls your tool must provide
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_SOFTSHADOW; };	
	
	// Some very common event handlers
	void SelectChange(BOOL isSelected);

	void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	void OnMouseMove(DocCoord PointerPos,Spread* pSpread, ClickModifiers ClickMods);
	virtual BOOL OnKeyPress(KeyPress* pKey);

	virtual BOOL GetStatusLineText(String_256* ptext, Spread* pSpread, DocCoord DocPos, ClickModifiers ClickMods);
// Don't do anything m_pEditRange copes with changes automatically
//	BOOL SelectionHasChanged() { m_iEditStackPos = STACKPOS_TOP; return TRUE; }
	
	// Some useful static functions
	static SoftShadowInfoBarOp* 	GetSoftShadowInfoBarOp()	{ return pSoftShadowInfoBarOp; }
	static BOOL 					IsCurrentTool()				{ return CurrentTool; }

	void SetBlobRendering(BOOL b) { m_bBlobRendering = b; }

	void SetAmDragging(BOOL bDrag);

	BOOL AreToolBlobsRenderedOnSelection() { return TRUE; }

	Spread * GetSpread() 
	{ 
		if (pLastBlobSpread) 
			return pLastBlobSpread; 
		else
			return Document::GetSelectedSpread();
	}

	ListRange*				GetShadowEditRange(NodeShadowController** ppMasterShadow = NULL);
	INT32						GetShadowEditRangeStackPos() const {return m_iEditStackPos;}
	void					SetShadowEditRange(ListRange* pNewRange, INT32 iNewPos, BOOL bConsistent = FALSE);
	NodeShadowController*	GetShadowEditField(ShadowMaskBits flags, String_256* pStrInfo = NULL);
	NodeShadowController*	GetShadowEditField(ShadowMaskBits flags, BOOL* pbNone, BOOL* pbConsistent, BOOL* pbMany);
	BOOL					SetShadowEditRangeFromSelection(BOOL bBroadcast = FALSE);
	void					SelectShadowRange(ListRange* pNewRange);

protected:
	BOOL FigureUserFeedback(String_256* pText,		Spread* pSpread,
							const DocCoord& dcPos,	ClickModifiers cMods);

	BOOL CreateCursors();						// Create  your tool's cursors in this func
	void DestroyCursors();						// Destroy your tool's cursors in this func

	static BOOL 			CurrentTool;		// Can be useful to keep a "is my tool current?" flag
	static SoftShadowInfoBarOp*	pSoftShadowInfoBarOp;	// Ptr to your tool's infobar

	Cursor*	pcNormalSoftShadowCursor;			// the standard cursor to use when your tool becomes active
	Cursor* pcBlobSoftShadowCursor;				// cursor that will appear when the mouse is over one of the tool blobs
	Cursor*	pcCurrentCursor;					// The cursor your tool is currently displaying
	INT32 CurrentCursorID;						// ID of the current cursor you're displaying

	// Status line text ID
	static UINT32	StatusID;

	// Standard tool static vars
	static 	TCHAR* FamilyName;					// The Tools Family Name
	static 	TCHAR* ToolName;						// The Tool Name
	static 	TCHAR* Purpose;						// What the tool is for
	static 	TCHAR* Author;						// Who wrote it

	BOOL m_bBlobRendering;

	Spread* pLastBlobSpread;

	INT32			m_iEditStackPos;
	ListRange*	m_pEditRange;
};



/********************************************************************************************

>	class DragCreateShadowParam : public OpParam

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/2000
	Purpose:	The parameter for the dragging operation

********************************************************************************************/
class DragCreateShadowParam : public OpParam
{
public:
	DragCreateShadowParam(SoftShadowTool * pTool, SoftShadowInfoBarOp * pInfoBar, DocCoord PointerPos,
		ClickModifiers ClickMods, BOOL bClickedOnBlob = FALSE)
	{
		m_pInfoBar = pInfoBar;
		m_PointerPos = PointerPos;
		m_pTool = pTool;
		m_ClickMods = ClickMods;
		m_bClickedOnBlob = bClickedOnBlob;
	}

	SoftShadowInfoBarOp * m_pInfoBar;
	SoftShadowTool * m_pTool;
	DocCoord m_PointerPos;
	ClickModifiers m_ClickMods;
	BOOL m_bClickedOnBlob;
} ;



/********************************************************************************************

>	class OpDragCreateShadow2 : public SelOperation

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/06/2000
	Purpose:	The Op which deals with drag-creation of shadows.

********************************************************************************************/
class OpDragCreateShadow2 : public SelOperation
{
CC_DECLARE_DYNCREATE(OpDragCreateShadow2)

friend class SoftShadowTool;

public:
	// Construction/Destruction
	OpDragCreateShadow2();
	~OpDragCreateShadow2();

	// start the drag - must have a DragCreateShadowParam passed in.
	virtual void DoWithParam(OpDescriptor* pOpDesc, OpParam* pOpParam);
	virtual void Do(OpDescriptor *pOpDesc) {};

	// should probably be implemented, but I'll leave this for the time being.
	virtual BOOL SnappingDrag() { return FALSE; }

	// virtual dragging functions.
	virtual void DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSolidDrag);
	virtual void DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread* pSpread, BOOL bSuccess, BOOL bSolidDrag);

	// the declaration function, which tells Camelot that we exist.
	static BOOL Declare();

	// the state function - allows us to enable or disable ourself.
	static OpState GetState(String_256* Description, OpDescriptor*);

	// helper methods for ourself.
private:
	static NodeRenderableInk*	GetSelectedShadowFromNode(NodeRenderableInk* pClickedNode);

	void ChangeAllShadows(	DocCoord* pdcWallOffset, DocCoord* pdcFloorVector,
							MILLIPOINT GlowWidth, ShadowType ChangeType	);
	BOOL SaveSettings(NodeShadowController* pShadow);
	BOOL SaveSettings(Range* pShadowRange);
	void RestoreSavedSettings();

	// ptrs to our info-bar and tool.
	SoftShadowInfoBarOp	*	m_pInfoBar;
	SoftShadowTool		*	m_pTool;

	// records of past mouse positions.
	DocCoord m_InitialPointerPos;

	// the node on which the drag originated.
	NodeRenderableInk * m_pClickNode;

	// pointer to a 'master shadow', which defines behaviour
	// for all other selected shadows of the same type.
	NodeShadowController* m_pMasterShadow;

	// information on initial shadow settings before the drag.
	MILLIPOINT	m_InitialGlowWidth;
	DocCoord	m_dcInitialWallOffset;
	DocCoord	m_dcInitialFloorShadowVector;
	MILLIPOINT	m_InitialFeatherWidth;

	// the region of screen currently covered by the dragged shadows.
	DocRect		m_drCurrentShadowBounds;

	// the region of screen initially covered by the dragged shadows.
	DocRect		m_drInitialShadowBounds;

	// a list to store shadow settings before the drag.
	List*		m_plInitialShadowSettings;

	// A range we do own and must delete when we're finished
	ListRange*	m_pOwnedRange;

};



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	class SaveShadowSettingsAction : public Action

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19 June	2000
	Purpose:	Save/restore the shadow settings for a given list of shadows.
				Used to undo/redo the results of OpDragCreateShadow2.

	Notes:		(usage notes here)

********************************************************************************************/
class SaveShadowSettingsAction : public Action
{
// dynamic class creation.
CC_DECLARE_DYNCREATE(SaveShadowSettingsAction)

public:
	// required Action methods.
	SaveShadowSettingsAction();
	~SaveShadowSettingsAction();
	static ActionCode Init(	UndoableOperation *	pUndoOp,
							ActionList *		pActionList,
							List *				plShadowSettings );
	virtual ActionCode Execute();

private:
	// initialise our own information.
	void SetShadowList(List* plShadowSettings) { m_plShadowSettings = plShadowSettings; }

	// member variables.
	List*	m_plShadowSettings;
};



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************

>	class ShadowSettingsItem : public ListItem

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/2000
	Purpose:	A container class for objects to go in our shadow settings list.

	SeeAlso:	OpDragCreateShadow2, SaveShadowSettingsAction.

********************************************************************************************/
class ShadowSettingsItem : public ListItem
{
public:
	ShadowSettingsItem(NodeShadowController* pShadow) { this->pShadow = pShadow; }
	ShadowSettingsItem() { ERROR3("Do not use this constructor!"); }
	~ShadowSettingsItem() { /* empty. */ };

	// the shadow which these settings apply to.
	NodeShadowController* pShadow;

	// the old settings for the shadow.
	DocCoord	dcWallOffset;
	DocCoord	dcFloorVector;
	MILLIPOINT	GlowWidth;
	MILLIPOINT	FeatherWidth;
};



#endif		// BUILDSHADOWS
#endif 		// INC_SOFTSHADOWTOOL
