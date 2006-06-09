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
// The ColourGallery class

#ifndef INC_COLGAL
#define INC_COLGAL


#if FALSE
// The old colour gallery no longer exists. THIS FILE IS DEFUNCT


#include "gallery.h"

//#include "sgallery.h"	// To replace the colour gallery with a supergallery - in camtypes.h [AUTOMATICALLY REMOVED]
#include "sgcolour.h"


class Document;
class ColGalDisplayList;

/********************************************************************************************

>	class ColourGallery : public Gallery

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	This class provides the Colour Gallery functionality - a gallery
				for applying and editing a documents IndexedColours.

********************************************************************************************/

class ColourGallery : public Gallery
{
	CC_DECLARE_DYNCREATE(ColourGallery)

public:
	ColourGallery();
	~ColourGallery();

	MsgResult Message(Msg* Message);


public:			// External interface (for the colour manager/colour bar)
//	static ColGalDisplayList *FindColourDisplayList(void);
		// Returns the current ColourGallery DisplayList, if any

	static ColourGallery *FindTheGallery(void);
		// Returns the current Colour gallery, if any

	static void ColourListHasChanged(ColourList *TheList);
		// Called in place of the ColourChangingMsg LISTUPDATED by the colour manager
		// to ensure that ColourGallery gets to update before other message processors
		// such as the colour bar, which expect ColGal to be up to date.


private:		// Private static data
	static ColourGallery *TheColourGallery;
		// Points to our single ColourGallery instance


protected:		// Base->Derived class interface
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
		// Applies the appropriate action to create a new item, possibly based upon
		// the selected item in some way. Returns the newly created real item

	void DoApplyItem(ListItem *ItemToApply);
		// Applies the appropriate action to apply the given item to the document

	BOOL DoRedefineItem(ListItem *ItemToRedefine);
		// Redefines the given item (allow the user to edit it for example)
		// Returns FALSE if the value of the item definitely has not changed
		// Returns TRUE if the item has changed, or if it is unknown whether it was changed

	void DoSpecialClick(ListItem *ClickedItem);
		// Handles a special (alt-)click on a list item. This is generally used as a short-
		// cut to bring up an editor on the clicked item, but the exact interpretation is
		// left to the derived gallery to decide.

	StringBase *GetItemText(ListItem *TheItem);
		// Returns NULL, or a pointer to the text to use in the listbox.
		// This is an early development aid while we're waiting for the advent
		// of proper user-redraw.

	void SortModeHasBeenChanged(BOOL SetDerivedSortsToo);
		// Called when the display dialogue has set a new sort mode, to make the
		// Gallery (and related entities) redisplay its list in the new order.
		// Note that the base class method for this MUST be calleded in derivations

	virtual BOOL DoSetDisplayModes(GalleryDisplayDlg *TheDialogue);
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


protected:		// ColourGallery specific methods
	BOOL SwitchTheDisplayList(Document *ScopeDoc = NULL);
	void CreateCachedDisplayList(Document *ScopeDoc);
	void ReadNewColourList(void);
	void SetSelectionFromDocument(void);


//	static INT32 ColourBarSortFunction(OrderedList *Parent,
//										ListItem *Item1, ListItem *Item2);
		// Sort function which works in an identical manner to that of the
		// Gallery baseclass, except it takes its SortMode info from the
		// ColourGallery::ColourBarSortMode member variable. This is used by
		// the colour bar as the sort function for its display.
};
	



/********************************************************************************************

>	class ColourGalleryCreate : public BarCreate

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Purpose:	Class for creating instantiations of the Colour SuperGallery

********************************************************************************************/

class ColourGalleryCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new ColourSGallery); }
};




//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------

#define OPTOKEN_DISPLAYCOLOURGALLERY _T("DisplayColourGallery")	// Optoken for the display colour gallery operation

/********************************************************************************************

>	class OpDisplayColourGallery: public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Displays the colours gallery. This op can be attached to a menu, button or keypress
	SeeAlso:	-

********************************************************************************************/

class CCAPI OpDisplayColourGallery: public Operation
{         
	CC_DECLARE_DYNCREATE( OpDisplayColourGallery );

public:
	static BOOL		Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
    void 			Do(OpDescriptor*);
};  

#endif

#endif

