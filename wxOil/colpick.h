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

// colpick.h - The Colour Picker dialogue

#ifndef INC_COLPICK
#define INC_COLPICK

//#include "colmodel.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class ColourList;
class ColourContext;
class IndexedColour;
class String_8;
class NewColourDlg;
class ColourEditDlg;

/********************************************************************************************

>	class ColourPicker

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/5/95
	Purpose:	Encapsulates OS-specific colour picker dialogue system, giving a
				kernel interface for using whatever colour picker(s) are available
				
				Given a (named or unnamed) IndexedColour, a Colour Picker somehow
				provides a user interface to change it, and returns the colour,
				cleaner and brighter than you ever thought possible, and for only
				about the same price as you'd expect to pay for an inferior colour
				picker. 1 out of 10 home economists prefer our colour picker to
				all others.

				This class also provides secrel-squirrel interfaces for doing
				rather horrendous direct-pokey Coloue Editor things to IndexedColours.
				These are only available to our friend editor dialogue classes.

	Friends:	ColourEditDlg; ColourLinkDlg
	SeeAlso:	IndexedColour; ColourEditDlg; ColourLinkDlg
			
********************************************************************************************/

class CCAPI ColourPicker : public CCObject
{
CC_DECLARE_MEMDUMP(ColourPicker)

friend class ColourEditDlg;
friend class ColourLinkDlg;
friend class NewColourDlg;

public:
	ColourPicker();
	~ColourPicker();
	

public:			// Entire enduser interface
	void EditColour(ColourList *ParentList, IndexedColour *SourceAndResult,
						BOOL PreferLineColour = FALSE);
				// Edit a specific IndexedColour

public:			// Status line/Bubble help interface
				// Get status line help text for the current pointer position
	static BOOL GetStatusLineText(String_256 *Result);

				// Update bubble help and mouse pointer shape (call on MouseMoves)
	static void UpdateBubbleHelpAndPointer(void);

	static void SetBubbleHelp(CGadgetID * GadgetList);

protected:
				// Internal variant of the above
	static BOOL GetStatusLineText(ColourEditDlg *Editor, UINT32 GadgetID, String_256 *Result);

	static TCHAR *HelpCallbackHandler(CWindowID Window, UINT32 Item, void *UserData);
	static BOOL		GetComponentHelp(ColourContext* const pSourceContext, 
									const UINT32 ComponentIndex, StringBase& HelpString);


protected:		// Interfaces strictly for use by the kernel ColourEditDlg/ColourLinkDlg classes
	
				// Read a component from a colour, and return a decimal string
				// representation, in the given output colour model
	static BOOL GetComponentAsString(IndexedColour *Source,
								 		ColourContext *DestContext, INT32 ComponentIndex,
								 		String_8 *Result);

	static BOOL GetComponentsAsHexString(IndexedColour *Source,
								 		 ColourContext *DestContext, INT32 ComponentIndex,
								 		 String_16 *Result);
				
				// Set a component value from a given decimal string value
				// Forces the colour into the given colour model
	static BOOL SetComponentFromString(IndexedColour *Dest,
										ColourContext *SourceContext, INT32 ComponentIndex,
										String_8 *NewValue);

				// Set a component value from a given hexadecimal string value
				// Forces the colour into the given colour model
	static BOOL SetComponentsFromHexString(IndexedColour *Dest,
										   ColourContext *SourceContext,
										   String_16 *NewValue);

	static BOOL IsValidHexString (String_16 Hex);
				// private helper function for ColourPicker::SetComponentsFromHexString
				// DON'T CALL THIS AS IT ONLY EXISTS TO HELP THE ABOVE FUNCTION - AND I
				// CANNOT SEE WHY YOUR CALLING THIS FROM ELSEWHERE ANYWAY ....

				// Set a component value from a given double value
				// Forces the colour into the given colour model
	static BOOL SetComponentFromDouble(IndexedColour *Dest,
										ColourContext *SourceContext, INT32 ComponentIndex,
										double NewValue);

				// Force a colour to be in the given colour model
	static void ForceColourModel(IndexedColour *Dest, ColourContext *SourceContext);

				// Read the tint from a colour, and return a decimal string
				// representation, in the given output colour model
				// Ensures and returns 0.0 iff the colour is not a tint.
	static BOOL GetTintAsString(IndexedColour *Source, StringBase *Result);
				
				// Set a tint value from a given decimal string value
				// Ensures and does nothing further if the colour is not a tint
	static BOOL SetTintFromString(IndexedColour *Dest, StringBase *NewValue);

				// Read the shade value (1=x or 2=y) from a colour, and return a decimal string
				// representation, as -100..+100
				// Ensures and returns 0.0 iff the colour is not a shade.
	static BOOL GetShadeValueAsString(IndexedColour *Source, INT32 ValueIndex, StringBase *Result);

				// Set a shade from a given pair of decimal string values (-100..+100)
				// Ensures and does nothing further if the colour is not a shade
	static BOOL SetShadeFromStrings(IndexedColour *Dest, StringBase *NewValueX, StringBase *NewValueY);

				// Set the extent of a window to barely include the given gadgets
				// (in the X and Y axes respectively)
				// (Used for folding/unfolding the colour editor dialogue)
	static void SetWindowExtent(CWindowID WindowID, CGadgetID XGadgetID, CGadgetID YGadgetID);

				// Moves all gadgets in NULL-terminated Gadgets list to lie under the
				// given gadget (does not affect X position) (The offset to the first
				// gadget in the list is used to calculate the amount by which to move all)
				// If MoveUnder == NULL, then all gadgets are moved downwards enough to
				// ensure they are not visible in the window.
	static void SetGadgetPositions(CWindowID WindowID,
									CGadgetID *Gadgets, CGadgetID MoveUnder);

				// Highly nasty. Spaces out the 4 component gadgets in an appropriate fashion
	static void SetComponentGadgets(CWindowID WindowID, CGadgetID *Gadgets,
													CGadgetID PickerGadget,
													ColourModel ModelToDisplay);
	static void SetFixedComponentGadgets(CWindowID WindowID, CGadgetID *Gadgets,
													CGadgetID PickerGadget,
													ColourModel ModelToDisplay,
													INT32 TextWidth[],
													INT32 EditWidth[]);

	static void RelayoutDialog(CWindowID WindowID);
	static void OnSize(CWindowID WindowID);
	static void RecursiveBestSize(wxWindow * pwxWindow);
	static BOOL OnIdleEvent(CWindowID WindowID);
	static void OnCreate(CWindowID WindowID);
	static void ArtificialSizeEvents(CWindowID WindowID);

private:
	static wxSize s_LastSize;
	static wxSize s_UserSize;
	static wxSize s_MinSize;
	static BOOL s_InColourDialogLayout;
	static BOOL s_JustCreated;
	static INT32 s_IdleCounter;

};


#endif


