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
// sgname.h - the NameGallery classes.

/*
	$Log: /Camelot/kernel/sgname.h $
 * 
 * 2     16/07/99 18:06 Justinf
 * Now shows little circular blobs for adding sets to / subtracting sets
 * from the selection.
 * 
 * 1     24/06/99 16:23 Justinf
 * Name Gallery
*/

#ifndef SGNAME_H
#define SGNAME_H

//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "sgtree.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgdrag.h"
#include "dragbmp.h"
//#include "dragtrgt.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "attrsets.h"


/***********************************************************************************************
>	class CCAPI NameGallery : public SuperGallery

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95 (Based on the Colour SGallery code)
	Purpose:	The Name SuperGallery class
	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery
	SeeAlso:	SuperGallery; SGDisplayName
***********************************************************************************************/

class SGNameGroup;

class CCAPI NameGallery : public SuperGallery
{
public:
	NameGallery();
	virtual ~NameGallery();
	virtual MsgResult Message(Msg* pMessage);

protected:
	virtual BOOL PreCreate();

	virtual BOOL InitMenuCommands();
	virtual BOOL BuildCommandMenu(GalleryContextMenu* pMenu, SGMenuID id);
	virtual OpState GetCommandState(StringBase* pstrCommandID, String_256* pstrShadeReason);
	virtual void DoCommand(StringBase* pstrCommandID);

	virtual void SelectionHasChanged();
	virtual BOOL ApplyAction(SGActionType Action);
	
	virtual BOOL OnApply(SGNameGroup* pGroup, BOOL fIsAdjust);
	virtual BOOL OnCreate(SGNameGroup* pGroup);
	virtual BOOL OnRename(SGNameGroup* pGroup);
	virtual BOOL OnDelete(SGNameGroup* pGroup);
	virtual BOOL OnRedefine(SGNameGroup* pGroup);
	virtual BOOL OnUnname(SGNameGroup* pGroup);

	virtual SGNameGroup* CreateItems(Document* pParentDoc, SGNameGroup* pGroup);
	virtual BOOL GetNewName(SGNameGroup* pGroup, UINT32 nDialogTemplateID, String_256* pstrName);
	virtual void RefreshDocument(Document* pDoc);
	virtual BOOL OnIdleEvent();

private:
	// Extension to the action codes already defined by the SuperGallery class.
	enum { SGACTION_UNNAME = SGACTION_UNDO + 1 };

	BOOL m_fMenusCreated;
	BOOL m_fIdleProc;

	CC_DECLARE_DYNCREATE(NameGallery)
};



/***********************************************************************************************
>	class CCAPI SGNameGroup : public SGDisplayGroup

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/01/95
	Purpose:	A SuperGallery group of SGNameItems.  Records whether this group's display
				needs to be updated on the next idle event because of a change to its
				Document's named objects.
	SeeAlso:	NameGallery; SuperGallery; SGDisplayItem
***********************************************************************************************/

class CCAPI SGNameGroup : public SGDisplayGroup
{
public:
	SGNameGroup(NameGallery* pParentGal, Document* pParentDoc = 0)
	  :	SGDisplayGroup(pParentGal, pParentDoc),
		m_fRefresh(FALSE)
			{ Flags.CanSelect = TRUE; }

	// Call this to mark (or unmark) this group for refresh on the next idle event.
	void SetRefresh(BOOL fR = TRUE)
			{ m_fRefresh = fR; }
	
	// Query whether this group needs to be refreshed and also reset its status.
	BOOL NeedsRefresh()
			{ BOOL fR = m_fRefresh; m_fRefresh = FALSE; return fR; }

private:
	BOOL m_fRefresh;
};



/***********************************************************************************************
>	class CCAPI SGNameItem : public SGDisplayItem

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/01/95
	Purpose:	This DisplayTree node is used by the Name SuperGallery
				It is responsible for providing the ability to handle/redraw one NameItem
				displayed in said gallery.
	SeeAlso:	NameGallery; SuperGallery; SGDisplayItem
***********************************************************************************************/

class CCAPI SGNameItem : public SGDisplayItem
{
public:
	SGNameItem(const String_256& strName);
	
	virtual void GetNameText(String_256* pResult);
	virtual BOOL GetBubbleHelp(DocCoord* pMousePos, String_256* pResult);
	virtual BOOL GetStatusLineHelp(DocCoord* pMousePos, String_256* pResult);
	virtual	BOOL HandleEvent(SGEventType EventType, void* pEventInfo,
							 SGMiscInfo* pMiscInfo);
	Document* GetDocument()
		{ return ((SGDisplayGroup*) GetParent())->GetParentDocument(); }

protected:
	virtual void CalculateMyRect(SGFormatInfo* pFormatInfo, SGMiscInfo* pMiscInfo);
	virtual void HandleRedraw(SGRedrawInfo* pRedrawInfo, SGMiscInfo* pMiscInfo);
	
private:
	String_256 m_strSetName;
	AttributeSets::Intersect m_eIntersect;
	
	CC_DECLARE_DYNAMIC(SGNameItem);
};



/********************************************************************************************
>	class CCAPI GalleryNameDragInfo : public BitmapDragInformation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	A Wix ObjectName attribute drag, which was started from the Name gallery
	SeeAlso:	BitmapDragInformation			
********************************************************************************************/

class TemplateAttribute;
class DragTarget;
class ViewDragTarget;

class CCAPI GalleryNameDragInfo : public BitmapDragInformation
{ 
public:
	// Creation & destruction.
	GalleryNameDragInfo();
 	GalleryNameDragInfo(SGNameItem* pSourceItem, SGMouseInfo* pMouseInfo,
						SGMiscInfo* pMiscInfo, TemplateAttribute* pAttrib,
						BOOL fIsAdjust = FALSE);
	virtual ~GalleryNameDragInfo();

	// Click handling and user feedback.
	virtual void OnClick(INT32 nFlags, POINT ptClick);
	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256* pText, DragTarget* pDragTarget);

	// Called when the drag ends over a view onto a document.
	virtual BOOL CanDropOnPage();
	virtual BOOL OnPageDrop(ViewDragTarget*);

	// Handlers for bitmap rendering during a drag.
	virtual INT32 GetDragTransparency();
	virtual KernelBitmap* GetSolidDragMask();
	
	SGNameItem* GetDraggedNameAttr()	{ return m_pSourceItem; }

private:
	SGNameItem*	   m_pSourceItem;
	SGMouseInfo	   m_MouseInfo;
	SGMiscInfo	   m_MiscInfo;
	NodeAttribute* m_pAttrib;

	CC_DECLARE_DYNCREATE(GalleryNameDragInfo)
};



/*********************************************************************************************
>	class CCAPI SGNameDragTarget : public SGListDragTarget

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.
				This particular target is used for handling drags of NameGalllery items
				within their displayed list. It automatically handles drags with draginfo
				derived from BitmapDragInformation.
	Notes:		Drag targets are destroyed automatically when a drag finishes by the drag
				manager with which they were registered.
				To remove a drag target at any time, destroy it - it automatically deregisters
				and cleans up.			
	SeeAlso:	SGListDragTarget; DragManagerOp::StartDrag; DragInformation; DragTarget;
				SGBitmapDragTarget::ProcessEvent
*********************************************************************************************/

class CCAPI SGNameDragTarget : public SGListDragTarget
{
public:
	SGNameDragTarget();
	SGNameDragTarget(NameGallery* pGallery, CGadgetID idGadget = 0);

protected:
	virtual BOOL ProcessEvent(DragEventType nEvent, DragInformation* pDragInfo,
							  OilCoord* pMousePos, KeyPress* pKeyPress);
private:
	CC_DECLARE_DYNAMIC(SGNameDragTarget)
};



/********************************************************************************************
>	class CCAPI OpDisplayNameGallery: public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/2/95 (Based on the Colour SGallery code)
	Purpose:	Displays the Name gallery.  This op can be attached to a menu, button
				or keypress.
	SeeAlso:	NameGallery
********************************************************************************************/

#define OPTOKEN_DISPLAY_NAME_GALLERY    TEXT("DisplayNameGallery")

class CCAPI OpDisplayNameGallery: public Operation
{         
public:
	static BOOL	Init();				
	static OpState GetState(String_256*, OpDescriptor*);		
    void Do(OpDescriptor*);

private:
	static DialogBarOp* FindGallery();

private:
	CC_DECLARE_DYNCREATE(OpDisplayNameGallery);
};  



/********************************************************************************************
>	class CCAPI NameObjectsDlg: public DialogOp

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/6/94
	Purpose:	Runs a dialog which asks the user to enter a name for the selected objects.
********************************************************************************************/

#define OPTOKEN_NAME_OBJECTS_DLG	TEXT("NameObjectsDlg")

class CCAPI NameObjectsDlg : public DialogOp
{         
public:
	NameObjectsDlg();
	static BOOL Init();
	
	static OpState GetState(String_256*, OpDescriptor*);
	void DoWithParam(OpDescriptor*, OpParam*);
	MsgResult Message(Msg* Message);

private:
	// Where to output the results of the dialog.
	String_256* m_pstrOutputName;
	BOOL* m_pfOkCancel;	

	CC_DECLARE_DYNCREATE(NameObjectsDlg)  
}; 

#endif	/* SGNAME_H */
