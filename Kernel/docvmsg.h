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

// Definition of DocView messages

/*
 * */

// Definition of Colour messages

#ifndef INC_DOCVMSG
#define INC_DOCVMSG

class DocView;

/*********************************************************************************************

>	class DocViewMsg : public Msg

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/1994
	Purpose:	This message is sent when a DocView is about to be destroyed or has just
				been created/loaded.  It contains a pointer to the appropriate view
				object (pDocView), and a state indicating if the view is being
				destroyed or is being created etc. (State).  Both members are public.
				See the file DOCVIEW.H for more information.

*********************************************************************************************/



class DocViewMsg : public Msg
{
	CC_DECLARE_DYNAMIC(DocViewMsg)
public:

	// Document messages are sent on these events.
	enum DocViewState
	{
		BORN,			// sent when a view is first constructed
		NEWANDSTABLE,	// sent out when a new docview is stable
		SCALECHANGED,	// DocView scale factor changed
		SELCHANGED,		// New style (22/7/94) selection changing message
						// as described in the docstate.doc document
						// Combines the SELECTED and UNSELECTED messages into one
						// message, with pOldDocView being the one that is being unselected
						// and pNewDocView being the one selected.
						// NOTE!!!! pOldDocView and/or pNewDocView can be NULL!!!!!
		KILLED,			// sent when a view is destroyed
		SELABOUTTOCHANGE	// This message is broadcast around just before the selected document
							// is changed (ie, it will be closly followed by a SELCHANGED message.
							// When this message is sent, nothing has actually been changed yet.
	};

	DocView* 		pDocView;			// points to view the message is about

	DocView* 		pOldDocView;
	DocView* 		pNewDocView;

	DocViewState  	State;				// TRUE if message refers to a newly created doc

	DocViewMsg(DocView* pview, DocViewState state)
	  : pDocView(pview), pOldDocView(0), pNewDocView(0), State(state) { /* empty */ };

	DocViewMsg(DocView* pThisOldDocView, DocView* pThisNewDocView, DocViewState state)
	  : pDocView(0), pOldDocView(pThisOldDocView), pNewDocView(pThisNewDocView), State(state)  { /* empty */ }
};

#endif
