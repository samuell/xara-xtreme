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

	helpuser.h

	Routines for running the user help system.
*/

#ifndef HELPUSER_H
#define HELPUSER_H

// Forward references.
class DialogOp;
class CDialog;
class OpDescriptor;

// These overloaded functions are passed various Camelot objects to provide user help on.
// Each version knows how to associate a help topic index with the given object.
BOOL HelpUser(const DialogOp& dlgop);
BOOL HelpUser(const wxDialog& dlg);
BOOL HelpUser(const OpDescriptor& opdesc);
BOOL HelpUser(UINT32 nMessageID);
BOOL HelpUserPropertyPage(UINT32 PageID);
BOOL HelpUserTopic(DWORD dwHelpIndex);

// Unfortunately, there are a few cases where it isn't possible to associate an object type
// with a help topic, such as the Print Setup dialog.  We use this bodgy "direct-access"
// function for those tricky cases.
BOOL _HelpUser(LPCTSTR lpcszClassName, UINT32 nSubTopic = 0);

// These functions run the help system and display the appropriate topic.
BOOL HelpContents();
BOOL HelpUsingHelp();
BOOL HelpUsingTools();
BOOL HelpUsingGalleries();
BOOL HelpOnlineDemos();
BOOL HelpTechnicalSupport();

// This displays the special Xara Studio info helpfile in the viewer build.
#ifdef STANDALONE
BOOL ShowHelpSpec();
#endif

// This function determines if help is available to the user for the given message.
BOOL CanHelpUser(UINT32 nMessageID);

// This is a "bodgy" function called by the String class MakeMsg function.  Many error
// and warning boxes require parameter substitution in their message strings.  This
// function records the help context of the last string resource passed to MakeMsg,
// usually this will be the key for context-sensitive help.
void SetNextMsgHelpContext(UINT32 nMessageID);
// Andy Hills, 22-11-00
// We also need a 'get' function for a 'bodge-on-top-of-a-bodge'
UINT32 GetNextMsgHelpContext();

// These are called on program startup and shutdown.  You can leave 'em alone, otherwise.
BOOL InitUserHelp();
BOOL DeInitUserHelp();

#endif	// !HELPUSER_H
