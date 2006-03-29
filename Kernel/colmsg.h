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
 * */

// Definition of Colour messages

#ifndef INC_COLMSG
#define INC_COLMSG

class Document; 
class ColourList;
class IndexedColour;



/*********************************************************************************************

>	class ColourChangingMessage : public Msg

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/94
	Purpose:	This message is sent whenever certain events of importance happen in the
				colour system. Most of these events relate to IndexedColour lists, or
				individual IndexedColours.
				See Develop:Camelot\Docs\howtouse\message.doc for details.

	SeeAlso:	ColourManager

*********************************************************************************************/

class ColourChangingMsg : public Msg
{
	CC_DECLARE_DYNAMIC(ColourChangingMsg)

public:
	// list messages are sent on these events.
	enum ColourState
	{
		LISTPAGED,		  				// Sent when a different list is paged in
		LISTUPDATED,	  				// Sent when the existing list is edited
		COLOURUPDATED,					// Sent when a colour has changed (visibly - see below)
		COLOURUPDATEDINVISIBLE,			// Sent when a colour has changed (invisibly - see below)
		LISTDESELECTED,		   			// Sent when there is now no selected list
		LISTDELETED,					// Sent when a list has been deleted

		NEWLISTBORN,					// Sent when a new list is created, so that
										// others can add sort modes to it.

		SELVIEWCONTEXTCHANGE			// Sent when the colour context attached to the
										// selected view changes (so the col editor, gallery,
										// line can redraw using new colour correction info)
	};
	
	Document *ScopeDoc;					// Points to the document in which the change occurred
	ColourList *NewColourList;  		// NULL, or points to the new/affected ColourList
	IndexedColour *ChangedColour;		// NULL, or pointer to the colour which has changed
	ColourState State;					// The state, as defined above

										// Constructor
	ColourChangingMsg(Document *pScopeDocument, ColourList *pChangedList,
						IndexedColour *pChangedColour, ColourState NewState)
		: ScopeDoc(pScopeDocument), NewColourList(pChangedList), ChangedColour(pChangedColour), State(NewState)
			{ /* empty */ }
};



// **** SPECIAL NOTE ****
// COLOURUPDATED and COLOURUPDATEDINVISIBLE are used in the following fashion:
// If the definition of the colour chnages such that the actual dispalyed colour will change,
// thus necessitating a redraw of the document, a COLOURUPDATED should be sent.
// If part of the definition changes which will not need a redraw of coloured regions (e.g
// the name, type, etc), then a COLOURUPDATEDINVISIBLE is sent. The latter is only used
// by those entities which need to update on all colour changes (they sit on both messages)
// while things like document redraws only occur if the visible colour has chnaged.
// See ColourManager::ColourHasChanged and ColourHasChangedInvisible

#endif
