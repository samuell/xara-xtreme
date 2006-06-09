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
// SGDFonts.h - the FontsSGallery dragging classes

#ifndef INC_SGDFONTS
#define INC_SGDFONTS

#ifndef EXCLUDE_GALS

//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dragbmp.h"
#include "sgdrag.h"

class String_256;
class KernelBitmap;
class SGDisplayGroup;
class SGDisplayPreviewFonts;
class SGLibFontItem;

/********************************************************************************************

>	class GalleryFontsDragInfo : public BitmapDragInformation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95
	Purpose:	A fill drag, which was started from the fill gallery
	SeeAlso:	BitmapDragInformation
				
********************************************************************************************/

class GalleryFontsDragInfo : public BitmapDragInformation
{ 
	CC_DECLARE_DYNCREATE(GalleryFontsDragInfo)
 
public:
	GalleryFontsDragInfo();
	~GalleryFontsDragInfo();
 	GalleryFontsDragInfo(	SGDisplayPreviewFonts *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE, INT32 XSize = 0, INT32 YSize = 0);

	virtual void OnClick(INT32 Flags, POINT Point);

	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget);

	virtual BOOL CanDropOnPage() { return TRUE; }
	virtual BOOL OnPageDrop(ViewDragTarget*);

	SGDisplayPreviewFonts*	GetDraggedFont()		{ return(SourceItem); }

	INT32 GetDragTransparency();

	KernelBitmap* GetSolidDragMask();

protected:
	SGDisplayPreviewFonts*	SourceItem;
	SGMouseInfo				MouseInfo;
	SGMiscInfo				MiscInfo;
};

/********************************************************************************************

>	class GalleryLibFontsDragInfo : public BitmapDragInformation

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95
	Purpose:	A fill drag, which was started from the fill gallery
	SeeAlso:	BitmapDragInformation
				
********************************************************************************************/

class GalleryLibFontsDragInfo : public BitmapDragInformation
{ 
	CC_DECLARE_DYNCREATE(GalleryLibFontsDragInfo)
 
public:
	GalleryLibFontsDragInfo();
	~GalleryLibFontsDragInfo();
 	GalleryLibFontsDragInfo(SGLibFontItem *pSourceItem,
							SGMouseInfo *pMouseInfo, SGMiscInfo *pMiscInfo,
 							BOOL IsAdjust = FALSE, INT32 XSize = 0, INT32 YSize = 0);

	virtual void OnClick(INT32 Flags, POINT Point);

	virtual UINT32 GetCursorID(DragTarget* pDragTarget);
	virtual BOOL GetStatusLineText(String_256 * TheText, DragTarget* pDragTarget);

	virtual BOOL CanDropOnPage() { return TRUE; }
	virtual BOOL OnPageDrop(ViewDragTarget*);

	SGLibFontItem*	GetDraggedFont()		{ return(SourceItem); }

	INT32 GetDragTransparency();

	KernelBitmap* GetSolidDragMask();
	
protected:
	SGLibFontItem*		SourceItem;
	SGMouseInfo				MouseInfo;
	SGMiscInfo				MiscInfo;
};

/*********************************************************************************************

>	class 	SGFontsDragTarget : public SGListDragTarget

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/95

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

class SGFontsDragTarget : public SGListDragTarget
{
friend class DragManagerOp;
	
CC_DECLARE_DYNAMIC(SGFontsDragTarget)

public:	// Public interface
	SGFontsDragTarget(DialogOp *TheDialog, CGadgetID TheGadget = NULL);

protected:
		// Process a drag-related event
	virtual BOOL ProcessEvent(DragEventType Event,
								DragInformation *pDragInfo,
								OilCoord *pMousePos, KeyPress* pKeyPress);
};

#endif
#endif
