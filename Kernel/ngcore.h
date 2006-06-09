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
	$Header: /wxCamelot/Kernel/ngcore.h 18    17/11/05 15:31 Luke $
	Attribute gallery core object
*/

#ifndef NGCORE_H
#define NGCORE_H

//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "ktimer.h"

#include <list>

class SGNameGroup;
class SGNameItem;
class SGUsedNames;
class Node;
class NamedStretchProp;
class NodeBarProperty;

// no more than 25 objects can change size through a stretch at one time
#define MAX_SIM_EXTENDS		35

// there can be no more than 20 bars in a single document
#define MAX_BARS			100


/***********************************************************************************************
>	class CCAPI NameGallery : public SuperGallery

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/5/99
	Purpose:	Core object of the Camelot naming system.
	SeeAlso:	SuperGallery
***********************************************************************************************/

class CCAPI NameGallery : public SuperGallery
{
public:
	// Creation & destruction.
	NameGallery();
	virtual ~NameGallery();
	static NameGallery* Instance();

	// Display groups and items.
	SGNameGroup* GetFirstGroup();
	SGUsedNames* GetUsedNames();

	// Item properties.
	INT32 GetPropertyIndex() const;
	INT32 SetPropertyIndex(INT32 nNewIndex);

	// UI state helper.  Returns how many items of each type are highlighted by the user.
	void GetHighlightCount(INT32* pnTotalItems, INT32* pnJustNames);

	// Call this to force an immediate update of the gallery's display, and thereby the
	// status of the document's sets.
	void ForceUpdate();
	BOOL FastUpdateNamedSetSizes(BOOL PropagateChanges = TRUE);
	BOOL FastApplyStretchScan(ObjChangeParam & ObjChange);

	String_256	m_BarToIgnoreTargetsOf;
	String_256	TheIgnoreBar() { return m_BarToIgnoreTargetsOf; }
	INT32 m_TouchedBar; // every item in this bar MAY has moved so sort it out


	// Called when the document has been changed.  Updates the targets of stretches whose
	// sources have changed bounds etc.
	BOOL PreTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam, Range* pRange);
	BOOL PreTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam, Node* pNode);
	BOOL PostTriggerEdit(UndoableOperation* pOp, ObjChangeParam* pParam);

	BOOL OnTimerEvent();

protected:
	// Event & redraw handlers.
	virtual BOOL PreCreate();
	virtual MsgResult Message(Msg* pMessage);
	virtual void DisplayDirty();
	virtual BOOL OnIdleEvent();
	
	// SuperGallery UI handlers.
	virtual void SetVisibility(BOOL fOpen);
	virtual BOOL InitMenuCommands();
	virtual BOOL BuildCommandMenu(GalleryContextMenu* pMenu, SGMenuID id);
	virtual void DoCommand(StringBase* pstrCommandID);
	virtual OpState GetCommandState(StringBase* pstrCommandID, String_256* pstrShadeReason);
	virtual BOOL ApplyAction(SGActionType nAction);
	virtual void SelectionHasChanged();

	BOOL CreateDisplayTree(); // initialises the display tree

	// bar handling functions
	void ResetRelationshipRects(SGNameItem* pTarget, NamedStretchProp* pProp,
								DocRect* pCleanTargetRect = 0, DocRect* pKnownTriggerRect = 0);
	void ShuffleBar(INT32 BarNumber, INT32 Spacing, INT32 BarDirection,
					SGUsedNames* pNames, Spread * pSpread, ObjChangeParam & ObjChange);

	BOOL ExpandVirtualTriggers(INT32 ExpandType, INT32 BarNo, DocRect &r1);

private:
	// Data.
	SGUsedNames*		m_pUsedNames;			// cached ptr to 'Used Names' display group
	INT32					m_nPropertyIndex;		// type of item property being displayed
	UINT32				m_nRefresh;				// non-zero if gallery display needs redrawing
	UINT32				m_nHiddenUpdates;		// non-zero if doc changed while gallery hidden
	BOOL				m_fMenusCreated;		// TRUE if menus have been created
	BOOL				m_fChildChanges;		// TRUE if must send child-change messages
	UndoableOperation*	m_LastOpUsed;			// the ptr to the op given in PreTrigger and reused in PostTrigger

	static NameGallery* m_pInstance;			// the global instance of the gallery

	BOOL				m_bResetARelationship;  // gets set when calling ResetRelationships

	// bar size data is required to make the buttons the same size
	// since they are required to be the size of the largest, we need to know the largest
	// element of every bar.
	struct BarSizeType
	{
		DWORD MaxWidth;
		DWORD MaxHeight;
		BYTE  TriggeredBar;
		BYTE  HasABackBar;
	} m_BarSize[MAX_BARS];

	// Matt - 06/02/2001
	// Accessor and Mutator Functions for m_BarSize[] BarSizeType...
	BOOL  SetBSTData(INT32 Index, DWORD MaxWidth, DWORD MaxHeight, BYTE TriggeredBar, BYTE HasABackBar);
	DWORD GetBSTMaxWidth(INT32 Index)		{ if (Index < MAX_BARS) { return m_BarSize[Index].MaxWidth;     } else { ERROR3("Out of range m_BarSize[] index!"); return 0; }};
	DWORD GetBSTMaxHeight(INT32 Index)	{ if (Index < MAX_BARS) { return m_BarSize[Index].MaxHeight;    } else { ERROR3("Out of range m_BarSize[] index!"); return 0; }};
	BYTE  GetBSTTriggeredBar(INT32 Index) { if (Index < MAX_BARS) { return m_BarSize[Index].TriggeredBar; } else { ERROR3("Out of range m_BarSize[] index!"); return 0; }};
	BYTE  GetBSTHasABackBar(INT32 Index)  { if (Index < MAX_BARS) { return m_BarSize[Index].HasABackBar;  } else { ERROR3("Out of range m_BarSize[] index!"); return 0; }};

private:
	class NameGalleryTimer : public KernelTimer
	{
	public:
		NameGalleryTimer(NameGallery * pOwner) : m_pOwner(pOwner) {}
		virtual void Notify() {m_pOwner->OnTimerEvent();}
		NameGallery * m_pOwner;
	};
	NameGalleryTimer m_Timer;

	CC_DECLARE_DYNCREATE(NameGallery)
};

#endif	/* !NGCORE_H */
