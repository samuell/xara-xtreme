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

// coldrop.h - The Colour Drop-down list support class

#ifndef INC_COLDROP
#define INC_COLDROP

#include "dropdown.h"
#include "gbrush.h"
#include "pump.h"


class IndexedColour;
class DocColour;
class ColourDropMsgHandler;		// Defined below


/********************************************************************************************

>	class ColourDropDown : public DropDown

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	?/10/95
	Purpose:	Colour drop down class - provides a dropdown list of named colours with
				colour patch and name, for use in the colour editor etc.

********************************************************************************************/

class ColourDropDown : public DropDown
{
friend class DialogManager;

CC_DECLARE_DYNCREATE(ColourDropDown)

// --- Kernelly section

public:			// ColourDropDown management
	ColourDropDown(BOOL AutoUpdateOnColourListChange = TRUE);
	virtual ~ColourDropDown();

	virtual BOOL Init(CWindowID Window, CGadgetID Gadget);

	void ClearAllSpecialEntries(void);
				// Ensures that all special entries are wiped

	BOOL AddSpecialEntry(StringBase *Text, DocColour *Colour = NULL);
				// Adds a special entry to the top of the list. Call before FillInColourList

	BOOL UpdateSpecialEntry(StringBase *Text, DocColour *NewColour);
				// Changes the colour splodge shown in a special entry to a different colour

	BOOL FillInColourList(IndexedColour *Selected = NULL, INT32 SelectIndex = -1,
							IndexedColour *NotDescendantOf = NULL);
				// Fills in the combobox list of available colour styles, and selects either
				// the Selected colour or the special entry indicated by SelectIndex

	BOOL SetSelection(IndexedColour *Selected, INT32 SelectIndex = -1);
				// Sets the selected item in the list

	IndexedColour *DecodeSelection(INT32 SelectionIndex);
				// Decodes a selection in the list, returning the IndexedColour it references,
				// or NULL if SelectionIndex is a special entry index.

	static void UpdateForNewColourList(void);
				// Updates the list as appropriate for a new colour list, shading if there
				// is now no Selected Document to get a colour list from.
				// Called at appropriate times by the MessageHandler

	static void RedrawColourDropDowns(void);
				// Redraws all active dropdowns - usually to take into account changes
				// in colour separation/correction settings for the selected DocView.
				// Called from the message handler as appropriate.

protected:
	List SpecialEntries;							// A list of any attached special entries

	BOOL AutoUpdateOnChange;						// TRUE if we should automatically update
													// the list when the current list changes

protected:
	static ColourDropMsgHandler *MessageHandler;	// One msg handler for all coldropdowns
	static INT32 MsgHandlerCount;					// Count of how many people using the MsgHandler	


// --- Winoily section

protected:		// Methods that must be overridden by derived classes
	virtual BOOL HasIcon(void * ItemData);
				// Returns TRUE if the item has an icon. Default is no icon

	virtual BOOL DrawIcon(void * ItemData, wxDC& dc, wxRect& IconRect, BOOL Disabled, INT32 flags);
				// Handles redraw of the icon, if any. Default is nothing gets drawn

	virtual wxString GetText(void * ItemData, INT32 Item);
				// Gets the text

protected:
	GBrush GDrawBrush;								// A GBrush of our very own
};





/********************************************************************************************

>	class ColourDropMsgHandler : public MessageHandler

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Purpose:	Intercepts messages for the Colour Drop Downs to allow them to stay
				up to date when documents change etc.

********************************************************************************************/

class ColourDropMsgHandler : public MessageHandler
{
friend class ColourDropDown;
public:
	CC_DECLARE_DYNAMIC( ColourDropMsgHandler ) 
	ColourDropMsgHandler();					// Constructor

protected:
	MsgResult Message(Msg* Msg);			// Overridden message reciever
};


#endif

