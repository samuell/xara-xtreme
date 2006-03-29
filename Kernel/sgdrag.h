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
// sgdrag.h - Drag Manager classes (information, targets) for supergallery scollbars
//			  and generic supergallery item dragging (for rearrangement only)

#ifndef INC_SGDRAG
#define INC_SGDRAG


#include "draginfo.h"
#include "dragtrgt.h"
#include "monotime.h"
#include "sgallery.h"


class SGDisplayRootScroll;
class SGDisplayItem;
class DocCoord;


/*********************************************************************************************

>	class 	SGScrollDragTarget : public KernelDragTarget

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.

				This particular target is used for handling drags of supergallery scroll
				bars, and for providing auto-repeat on scroll buttons.

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget

	Documentation:	Docs\Specs\DragMan.doc

*********************************************************************************************/

class SGScrollDragTarget : public KernelDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGScrollDragTarget)


public:	// Public interface
	SGScrollDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);
	
	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);

protected:
	MonotonicTime Timer;
};



typedef enum
{
	SGDRAG_NONE = 0,				// Used to notate when no drag is active/valid
	SGDRAG_SCROLLUP,				// Up	} - click scroll arrow
	SGDRAG_SCROLLDOWN,				// Down	}
	SGDRAG_PAGEUP,					// Up	} - click page-scroll areas
	SGDRAG_PAGEDOWN,				// Down	}
	SGDRAG_SAUSAGE					// Drag of scroll sausage
} SGDragType;



/********************************************************************************************

>	class SGScrollDragInfo : public DragInformation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/95
	Purpose:	Describes a supergallery scroll drag
				
********************************************************************************************/

class SGScrollDragInfo : public DragInformation
{ 
CC_DECLARE_DYNCREATE(SGScrollDragInfo)
 
public:
	SGScrollDragInfo();
 	SGScrollDragInfo(SGDisplayRootScroll *ParentRootNode, SGDragType TheDragType,
						SGMiscInfo *MiscInfo, INT32 DragAnchorOffset,
						BOOL IsAdjust = FALSE);
	
	void OnClick(INT32 Flags, POINT Point);
	
	virtual UINT32 GetCursorID(void);
 	virtual BOOL GetStatusLineText(String_256 * TheText);

	SGDisplayRootScroll *GetDragRootNode(void)	{ return(DragRootNode); }
	SGDragType GetDragType(void)				{ return(DragType); }
	SGMiscInfo *GetDragMiscInfo(void)			{ return(&DragMiscInfo); }
	INT32 GetDragAnchorOffset(void)				{ return(DragAnchor); }
	BOOL IsAdjustDrag(void)						{ return(DragAdjust); }

private:
	SGDisplayRootScroll *DragRootNode;		// The node which started the drag
	SGDragType	DragType;					// The type of drag (see SGDragType, above)
	SGMiscInfo	DragMiscInfo;				// Copy of MiscInfo for the event causing the drag
	INT32		DragAnchor;					// Offset from top of sausage rect to the point
											//   which was dragged.
	BOOL		DragAdjust;					// TRUE if this is an 'adjust' drag
};







typedef enum								// Where an item can be inserted...
{
	SGDRAGINSERT_NONE,						// No valid target
	SGDRAGINSERT_ADD,						// Add an item into a group
	SGDRAGINSERT_BEFORE,					// Insert before target
	SGDRAGINSERT_AFTER						// Insert after target
} SGDragInsertType;

/*********************************************************************************************

>	class 	SGListDragTarget : public KernelDragTarget

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.

				This particular target is used for handling drags of supergallery items
				within their displayed list. It automatically handles drags with draginfo
				derived from SGListDragInfo. If you wish to handle drags of other
				more specialised types, then you should derive a class from this one, and
				simply override the ProcessEvent method in order to correctly divine
				information such as parent gallery and dragged gallery item from the
				current DragInfo object. (c.f. sgcolour.cpp -the SGColourDragTarget)

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget;
				SGColourDragTarget::ProcessEvent

	Documentation:	Docs\Specs\DragMan.doc; Docs\HowToUse\SGallery.doc

*********************************************************************************************/

class SGListDragInfo;

class SGListDragTarget : public KernelDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGListDragTarget)


public:	// Public interface
	SGListDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);
	
	virtual UINT32 GetCursorID();
	virtual BOOL GetStatusLineText(String_256 * TheText);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);

		// Subroutine to determine whether we will try to insert above or below an item
	SGDragInsertType GetDragInsertType(SuperGallery *ParentGallery,
								SGDisplayNode *DraggedNode,
								OilCoord *pMousePos,
								SGDisplayNode *DestNode);

		// Subroutine for handling a successful drag completion
	void HandleDragCompleted(SuperGallery *ParentGallery,
								SGDisplayNode *DraggedNode,
								OilCoord *pMousePos,
								BOOL DragThisItemOnly = FALSE);

		// Determines a cursor shape appropriate to the current mouse position
	virtual BOOL DetermineCursorShape(SuperGallery *ParentGallery,
								SGDisplayNode *DraggedNode,
								OilCoord *pMousePos);

		// Determines (by comparing format rects) if the given node takes up an
		// entire 'line' of the dislay list.
	BOOL OwnsEntireLine(SGDisplayNode *TheNode);

protected:
	UINT32 CurrentCursorID;
};






/********************************************************************************************

>	class SGListDragInfo : public DragInformation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Purpose:	Describes a generic supergallery item drag

	Documentation:	Docs\Specs\DragMan.doc; Docs\HowToUse\SGallery.doc

	SeeAlso:	SGListDragTarget; SuperGallery; SGDisplayNode::HandleEvent
				
********************************************************************************************/

class SGListDragInfo : public DragInformation
{ 
CC_DECLARE_DYNCREATE(SGListDragInfo)
 
public:
	SGListDragInfo();
 	SGListDragInfo(SuperGallery *ParentGal, SGDisplayNode *DraggedNode,
					SGMouseInfo *TheMouseInfo, BOOL IsAdjust = FALSE);
	
	void OnClick(INT32 Flags, POINT Point);
	
	virtual UINT32 GetCursorID(void);
 	virtual BOOL GetStatusLineText(String_256 * TheText);

	SuperGallery  *GetParentGallery(void)	{ return(ParentGallery); }
	SGDisplayNode *GetDraggedNode(void)		{ return(DragNode); }
	SGMouseInfo	  *GetMouseInfo(void)		{ return(&MouseInfo); }

private:
	SuperGallery  *ParentGallery;			// The gallery from which this drag originated
	SGDisplayNode *DragNode;				// The node which is being dragged											//   which was dragged.
	SGMouseInfo	  MouseInfo;				// A COPY of the original MouseInfo for the click
};


#endif




