// $Id: optsmsgs.h 662 2006-03-14 21:31:49Z alex $
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

// docmsgs.h - The DocChangingMsg class
// See document.cpp for what little implementation exists (CC_IMPLEMENT_DYNAMIC)

/*
$Log
*/

// Definition of Options changing messages

#ifndef INC_OPTSMSGS
#define INC_OPTSMSGS

//#include "msg.h"


/*********************************************************************************************
>	class OptionsChangingMessage : public Msg

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/1/1995
	Purpose:	This message is sent when a user has changed the options on a document or
				a global application based option. At present the messages include:-

				NEWUNITS messages are broadcast when the user has specified new display, page
				or font units are to be used.

				An OptionsChangingMsg contains a pointer to the appropriate Document
				object (pScopeDoc), and a state indicating the option that is being changed.
				Both members are public.
	
	SeeAlso:	OptionsTabs; UnitsTab; AppPrefsDlg; DocPrefsDlg;

*********************************************************************************************/

	

class OptionsChangingMsg : public Msg
{
	CC_DECLARE_DYNAMIC(OptionsChangingMsg)
public:

	// Options changing messages are sent on these events.
	enum OptionsState
	{
		NEWUNITS,					// Sent when a new set of default units has been specified
									// This will include page, display and font default units.
		NEWCOLOURBARMODE,			// Sent when a new colour bar display mode has been set

		ASKBEFORESETTINGATTR,		// Sent when the ask before setting attribute mode has been
									// changed.

		IMPORTWITHLAYERS,			// Sent when the import with layers mode has been changed
		
		AUTOMATICTRANSPARENCY,		// Sent when the automatic transparency mode has been changed

		NEWDEFAULTGRID,				// Sent when there has been a change in the default grid

		NEWPAGESIZE,				// Sent when there has been a change in the page size (undo/redo)
									// This is sent by the redo/undo actions and forces an update of
									// the page tab.
		PAGESIZEHASCHANGED,			// Sent when there has been a change in the page size (do)
									// This is sent by the page tab itself when the do happens and
									// does not force an update of the page tab.

		SEPARATIONSENABLED			// Sent when colour seps are enabled/disabled, so that the
									// related imagesetting tabs can update themselves
	};

	Document *pScopeDoc;			// Points to the document in which the change occurred
	OptionsState  State;			// The message specifics, as enumerated above

	OptionsChangingMsg(Document* pScopeDocument, OptionsState NewState)
	  : pScopeDoc(pScopeDocument), State(NewState)
	  	{ /* empty */ }

	OptionsChangingMsg(OptionsState NewState)
	  : pScopeDoc(NULL), State(NewState)
	  	{ /* empty */ }
};

#endif // INC_OPTSMSGS

