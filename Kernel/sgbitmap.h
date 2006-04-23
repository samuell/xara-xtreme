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
// SGBitmap.h - the BitmapSGallery (Bitmap SuperGallery) class

#ifndef INC_SGBASE
#define INC_SGBASE

#include "sgallery.h"
#include "sgtree.h"
#include "dragbmp.h"
#include "sgdrag.h"
//#include "bmpsdlg.h"

const INT32 SG_DefaultSmallBmp		= 64000;	
const INT32 SG_DefaultSmallBmpText	= 80000;
const INT32 SG_DefaultLargeBmp		= 96000;
const INT32 SG_DefaultLargeBmpText	= 120000;

class Bitmap;
class KernelBitmap;
class BmpDlgParam;
class SGDisplayKernelBitmap;

PORTNOTE("other", "Disable GalleryBitmapDragInfo")
#ifndef EXCLUDE_FROM_XARALX
/********************************************************************************************

>	class GalleryBitmapDragInfo : public BitmapDragInformation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Purpose:	A bitmap drag, which was started from the bitmap gallery
	SeeAlso:	BitmapDragInformation
				
********************************************************************************************/

class GalleryBitmapDragInfo : public BitmapDragInformation
{ 
	CC_DECLARE_DYNCREATE(GalleryBitmapDragInfo)
 
public:
	GalleryBitmapDragInfo();
 	GalleryBitmapDragInfo(	SGDisplayKernelBitmap *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE);
	~GalleryBitmapDragInfo();

	virtual void OnClick(INT32 Flags, POINT Point);

	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget);

	virtual BOOL CanDropOnPage() { return TRUE; }
	virtual BOOL OnPageDrop(ViewDragTarget*);

	SGDisplayKernelBitmap*	GetDraggedBitmap()	{ return(SourceItem); }

protected:
	SGDisplayKernelBitmap*	SourceItem;
	SGMouseInfo				MouseInfo;
	SGMiscInfo				MiscInfo;

	KernelBitmap*			m_pDragBmp;
};
#endif

/*********************************************************************************************

>	class 	SGBitmapDragTarget : public SGListDragTarget

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/3/95

	Purpose:	An instantiation of this class is created by each entity which wishes to 
				provide a 'destination' to which the mouse can go to complete a drag.

				This particular target is used for handling drags of BitmapSGalllery items
				within their displayed list. It automatically handles drags with draginfo
				derived from BitmapDragInformation.

	Notes:		Drag targets are destructed automatically when a drag finishes by the drag
				manager with which they were registered.

				To remove a drag target at any time, destruct it - it automatically deregisters
				and cleans up.
				
	SeeAlso:	DragManagerOp::StartDrag; DragInformation; DragTarget;
				SGBitmapDragTarget::ProcessEvent

	Documentation:	Docs\Specs\DragMan.doc; Docs\HowToUse\SGallery.doc

*********************************************************************************************/

class SGBitmapDragTarget : public SGListDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGBitmapDragTarget)

public:	// Public interface
	SGBitmapDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);
};

/***********************************************************************************************

>	class SGDisplayKernelBitmap : public SGDisplayItem

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/01/95
	Purpose:	This DisplayTree node is used by the Bitmap SuperGallery
				It is responsible for providing the ability to handle/redraw one KernelBitmap
				displayed in said gallery.

	SeeAlso:	BitmapSGallery; SuperGallery; SGDisplayItem

***********************************************************************************************/

class CCAPI SGDisplayKernelBitmap : public SGDisplayItem
{
	CC_DECLARE_DYNAMIC(SGDisplayKernelBitmap);

public:
	SGDisplayKernelBitmap();
	SGDisplayKernelBitmap(KernelBitmap *KernelBitmapToDisplay);

	virtual	BOOL HandleEvent(SGEventType EventType, void *EventInfo,
							 SGMiscInfo *MiscInfo);

	virtual void DragWasReallyAClick(SGMouseInfo *Mouse, SGMiscInfo *MiscInfo);
		// Special handler called by GalleryColourDragInfo when a drag turns out to be a click

	virtual void GetNameText(String_256 *Result);
	virtual void GetFullInfoText(String_256 *Result);
		// Read the name and full-information text for this item

	virtual void MoveAfter(SGDisplayNode *NodeToMove);
	virtual void MoveBefore(SGDisplayNode *NodeToMove);
		// Move items around in the tree, and also move the original in the bitmap list

	// Status line help for bitmap item
	virtual BOOL GetStatusLineHelp(DocCoord *MousePos, String_256 *Result);

protected:	// Internal handlers
	virtual void CalculateMyRect(SGFormatInfo *FormatInfo, SGMiscInfo *MiscInfo);
			// Determines item size for the current DisplayMode and calculates FormatRect

	virtual void HandleRedraw(SGRedrawInfo *RedrawInfo, SGMiscInfo *MiscInfo);
			// Redraws the item into the current FormatRect

public:		// Specialisations of this class
	inline KernelBitmap *GetDisplayedKernelBitmap(void);


private:		// Special member variables
	KernelBitmap *TheKernelBitmap;
};



/********************************************************************************************

>	inline KernelBitmap *SGDisplayKernelBitmap::GetDisplayedKernelBitmap(void)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95

	Returns:	A pointer to the KernelBitmap which this Display Item is used to display.
			
	Purpose:	To find out the KernelBitmap this object is responsible for displaying

********************************************************************************************/

KernelBitmap *SGDisplayKernelBitmap::GetDisplayedKernelBitmap(void)
{
	return(TheKernelBitmap);
}






/***********************************************************************************************

>	class BitmapSGallery : public SuperGallery

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (Based on template code by Jason)
	Created:	27/1/95 (Based on the Colour SGallery code)
	Purpose:	The Bitmap SuperGallery class

	Notes:		SuperGalleries are specced in a number of docs including
					specs\sgallery.doc ("super" gallery extensions)
					specs\propui.doc  (gallery basic ui)
				There is also howtouse\sgallery.doc which describes creating a supergallery

	SeeAlso:	SuperGallery; SGDisplayBitmap

***********************************************************************************************/

class BitmapSGallery : public SuperGallery
{
	CC_DECLARE_DYNCREATE(BitmapSGallery)

public:
	BitmapSGallery();
	~BitmapSGallery();

	virtual MsgResult Message(Msg* Message);
	virtual void SelectionHasChanged(void);

	virtual SGDisplayItem *CopyDisplayItem(SGDisplayItem *SourceItem, 
								SGDisplayGroup *DestGroup,
								SGDisplayItem *TargetPosition = NULL);
		// Asks the gallery to copy (or if appropriate, move) the given item to
		// the given group. This hould be done by copying the given item, and
		// then calling InsertCopiedItem() to insert it appropriately (see above)

protected:				// Overridden upcall methods
	virtual BOOL PreCreate(void);

	virtual BOOL ApplyAction(SGActionType Action);

	virtual void HandleDragStart(DragMessage *DragMsg);

	virtual RenderRegion *CreateRenderRegion(DocRect *VirtualSize,
											ReDrawInfoType *DlgRedrawInfo);

	virtual void DestroyRenderRegion(RenderRegion *pRender);

	virtual void DoShadeGallery(BOOL ShadeIt);
		// Called by SuperGallery::ShadeGallery to allow the derived gallery to add code
		// to shade/unshade any extra controls it may have in the window

public:					// Overridden Command interface (for Ops and menu support)
	virtual BOOL PreContextMenu(void);
		// Give the gallery a chance to do something before popping up the context menu
		// At present, used by the bitmap gallery to ensure that the plug-ins have all
		// been parsed.

	virtual BOOL InitMenuCommands(void);
		// Called on startup to initialise any desired menu commands. Will make repeated calls
		// to InitMenuCommand(), below.

	virtual BOOL BuildCommandMenu(GalleryContextMenu *TheMenu, SGMenuID MenuID);
		// Builds the command menu for the given menu pop-up (over an item, or options button)

	virtual OpState GetCommandState(StringBase *CommandID, String_256 *ShadeReason);
		// Returns an OpState indicating the state of this command, much like an Op GetState

	virtual void DoCommand(StringBase *CommandID);
		// Attempts to "do" the given command. Should call down to the base class to handle
		// any commands it can't deal with.


protected:		   		// Personal/Internal methods
	void CreateNewSubtree(Document *ParentDoc, SGDisplayGroup *ExistingGroup = NULL);
			// Creates a new subtree group to display the Bitmaps for the given document
			// If ExistingGroup != NULL, that group will be re-cached for the given doc

	KernelBitmap* GetSelectedBitmap();
	BOOL GetSelectedBitmaps(KernelBitmap*** pOutputArray, UINT32* pOutputCount);
public:
	BOOL HasDelayValueChanged ();
	//BOOL AreDelayValuesSame();
	//UINT32 GetBitmapDelay();
	BOOL IsJPEG(KernelBitmap** pList,UINT32 ListSize, BmpDlgParam* Param, INT32* pBytes);
	
protected:
	BOOL DeleteSelection();

	KernelBitmap* CheckTextureBitmap(KernelBitmap* pBitmap);

	BOOL FindCentreInsertionPosition(Spread** Spread, DocCoord* Position);

	void SetSelectionFromDocument(BOOL AlwaysScroll = FALSE);

	BOOL GetBmpInfo(BmpDlgParam* Param);
	BOOL SetBmpInfo(BmpDlgParam* Param);

public:
	static BOOL IgnoreBmpListChange;
};

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

// Optoken for the display bitmap gallery operation
#define OPTOKEN_DISPLAYBITMAPGALLERY _T("DisplayBitmapGallery")	

/********************************************************************************************

>	class OpDisplayBitmapGallery: public Operation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Purpose:	Displays the bitmap gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayBitmapGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayBitmapGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	void 			Do(OpDescriptor*);	
};  

#endif

