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
// Header file for example galleries

#ifndef INC_EXAGAL
#define INC_EXAGAL

#if FALSE
#include "gallery.h"


/********************************************************************************************

>	class ExampleGallery : public Gallery

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/1994
	Purpose:	Class for handling an example GalleryBarOp

	Notes:		This provides a very basic example to get you started on galleries.
				However, it is recommended that you use the colour gallery as a more
				advanced example, and if you are basing your code on anything, then
				it is probably easier to rip functionality out of a cloned ColourGallery
				than to add the desired functionality to the ExampleGallery.

********************************************************************************************/

class ExampleGallery : public Gallery
{
	CC_DECLARE_DYNCREATE( ExampleGallery )  

public:
	ExampleGallery();

	MsgResult Message(Msg* Message);


protected:		// Base<->Derived class interface
	BOOL PreCreate(void);
	BOOL PostCreate(void);
		// Initialise display-related stuff before/after the window is created

	BOOL DoRedrawAnItem(ListItem *Item, BOOL Large, UINT32 DisplayMode);
		// Redraws the given item appropriately for large/small mode, and
		// using the given derived class DisplayMode (see DoDisplayMode... below)
		// returns TRUE if it redrew successfully

	BOOL PrepareToDelete(void);
		// returns TRUE if the selection can be deleted, or FALSE if not
		// May alter the selection as appropriate (after getting user confirmation)

	BOOL DoDeleteSelection(void);
		// Force-deletes anything still remaining in the selection
		// returns TRUE if successful

	ListItem *DoCreateNewItem(ListItem *BasedOn);
		// Applies the appropriate action to create a new item, based upon
		// the 'BasedOn' item or the attribs of the document selection in some way
		// Returns NULL, or the new item

	void DoApplyItem(ListItem *ItemToApply);
		// Applies the appropriate action to apply the given item to the document

	BOOL DoRedefineItem(ListItem *ItemToRedefine);
		// Redefines the given item (allow the user to edit it for example)
		// Returns FALSE if the value of the item definitely has not changed
		// Returns TRUE if the item has changed, or if it is unknown whether it was changed

	StringBase *GetItemText(ListItem *TheItem);
		// Returns NULL, or a pointer to the text to use in the listbox.
		// This is an early development aid while we're waiting for the advent
		// of proper user-redraw.

	void SortModeHasBeenChanged(BOOL SetDerivedSortsToo);
		// Called when the display dialogue has set a new sort mode, to make the
		// Gallery (and related entities) redisplay its list in the new order.
		// Note that the base class method for this MUST be calleded in derivations

	BOOL DoSetDisplayModes(GalleryDisplayDlg *TheDialogue);
		// Called to set up the display mode combo box (large icons, full info etc)
		// This will call TheDialogue->SetDisplayModeName repeatedly to set all
		// available modes. Returns FALSE if there are no display modes.

	void DoDisplayModeChange(UINT32 NewDisplayMode);
		// Called when the user has chosen a display mode, with the index of the
		// mode in the given list. You should not need to take action here, as
		// the display mode is passed through to RedrawAnItem, but this is called
		// in case you wish to cache values before redraw...

	void DoShowMenu(void);
		// Pops up the Gallery menu
		// Menu choices will be handled by the derived class through the Ops system
		// or message handlers - the base class knows nothing of this process.

	BOOL SetAlsoButton(String_64 *TextToPutIn);
		// Called to set up the generic Gallery Display dialogue
		// Returns FALSE if no 'also set the sort mode for the xxxx' button is wanted
		// Returns TRUE, and fills in the text with an appropriate string if 
		// a button is wanted. See the ColourGallery for an example.
};
	


/********************************************************************************************

>	class ExampleGalleryBarOpCreate : public BarCreate

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/06/1994
	Purpose:	Class for creating ExampleGalleryBarOps.

********************************************************************************************/

class ExampleGalleryCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new ExampleGallery); }
};

#endif

#endif  // INC_EXAGAL


