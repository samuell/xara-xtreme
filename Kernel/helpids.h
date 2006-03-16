// $Id: helpids.h 662 2006-03-14 21:31:49Z alex $
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
 */ 

#ifndef INC_HELPIDS
#define INC_HELPIDS

//---------------------------------------------------------------------
// Old Format IDs - DON'T USE THESE, SEE BELOW...
//
// File commands
#define HID_FILE_NEW                     0x0100
#define HID_FILE_OPEN                    0x0101
#define HID_FILE_CLOSE                   0x0102
#define HID_FILE_SAVE                    0x0103
#define HID_FILE_SAVE_AS                 0x0104
#define HID_FILE_SAVEAS					0x0104		// Alias to HID_FILE_SAVE_AS
#define HID_FILE_PAGE_SETUP              0x0105
#define HID_FILE_PRINT_SETUP             0x0106
#define HID_FILE_PRINT                   0x0107
#define HID_FILE_PRINT_PREVIEW           0x0108

#define HID_FILE_MRU_FILE1               0x0110
#define HID_FILE_MRU_FILE2               0x0111
#define HID_FILE_MRU_FILE3               0x0112
#define HID_FILE_MRU_FILE4               0x0113

// Edit commands
#define HID_EDIT_CLEAR                   0x0120
#define HID_EDIT_CLEAR_ALL               0x0121
#define HID_EDIT_COPY                    0x0122
#define HID_EDIT_CUT                     0x0123
#define HID_EDIT_FIND                    0x0124
#define HID_EDIT_PASTE                   0x0125
#define HID_EDIT_PASTE_LINK              0x0126
#define HID_EDIT_PASTE_SPECIAL           0x0127
#define HID_EDIT_REPEAT                  0x0128
#define HID_EDIT_REPLACE                 0x0129
#define HID_EDIT_SELECT_ALL              0x0130
#define HID_EDIT_UNDO                    0x0131
#define HID_EDIT_REDO                    0x0132

// Window commands
#define HID_WINDOW_NEW					0x0133
#define HID_WINDOW_NEWVIEW				0x0133		// Alias
#define HID_WINDOW_ARRANGE				0x0134
#define HID_WINDOW_CASCADE				0x0135
#define HID_WINDOW_TILE					0x0136
//#define HID_WINDOW_TILE_HORZ             0x0136	// No longer used
//#define HID_WINDOW_TILE_VERT             0x0137	// No longer used
#define HID_WINDOW_SPLIT                 0x0138

// View Commands
#define HID_VIEW_TOOLBAR                 0x0800
#define HID_VIEW_STATUS_BAR              0x0801
#define HID_VIEW_STATUSBAR				 0x0801		// Alias
#define HID_VIEW_FULLSCREEN              0x0802     

// Help and App commands
#define HID_APP_ABOUT                    0x0140		 // No longer used
#define HID_APP_EXIT                     0x0141
#define HID_HELP_INDEX                   0x0142
#define HID_HELP_USING                   0x0143
#define HID_CONTEXT_HELP                 0x0144      // shift-F1
#define HID_HELP_SPEC	                 0x0145      // Help on Xara Spec
// End of old format IDs.

//---------------------------------------------------------------------
// New Format IDs
//
// File commands
#define HID_FILENEW						0x0100
#define HID_FILEOPEN                    0x0101
#define HID_FILECLOSE                   0x0102
#define HID_FILESAVE                    0x0103
#define HID_FILESAVEAS					0x0104
#define HID_FILEPAGESETUP              	0x0105
#define HID_FILEPRINTSETUP             	0x0106
#define HID_FILEPRINT                   0x0107
#define HID_FILEPRINTPREVIEW           	0x0108

#define HID_FILEMRUFILE1               	0x0110
#define HID_FILEMRUFILE2               	0x0111
#define HID_FILEMRUFILE3               	0x0112
#define HID_FILEMRUFILE4               	0x0113

// Edit commands
#define HID_EDITCLEAR                   0x0120
#define HID_EDITCLEARALL               	0x0121
#define HID_EDITCOPY                    0x0122
#define HID_EDITCUT                     0x0123
#define HID_EDITFIND                    0x0124
#define HID_EDITPASTE                   0x0125
#define HID_EDITPASTELINK              	0x0126
#define HID_EDITPASTESPECIAL           	0x0127
#define HID_EDITREPEAT                  0x0128
#define HID_EDITREPLACE                 0x0129
#define HID_EDITSELECTALL              	0x0130
#define HID_EDITUNDO                    0x0131
#define HID_EDITREDO                    0x0132

// Window commands
#define HID_WINDOWNEW					0x0133
#define HID_WINDOWNEWVIEW				0x0133		// Alias
#define HID_WINDOWARRANGE				0x0134
#define HID_WINDOWCASCADE				0x0135
#define HID_WINDOWTILE					0x0136
#define HID_WINDOWSPLIT                 0x0138

// View Commands
#define HID_VIEWTOOLBAR                 0x0800
#define HID_VIEWSTATUSBAR				0x0801

// Help and App commands
#define HID_HELPINDEX                   0x0142
#define HID_HELPUSING                   0x0143

#define HID_EDITSELECTNONE				0x0144

#define HID_JOINSHAPEOP					0x0145
#define HID_BREAKSHAPEOP				0x0146
#define HID_DELETEPOINTSOP				0x0147
#define HID_XPE_EDIT					0x0148
#define HID_EDIT_EDITSELECTION			0x0149

// End of New format IDs.
//---------------------------------------------------------------------

// Misc
#define HID_NEXT_PANE                    0x0150
#define HID_PREV_PANE                    0x0151

// OLE commands
#define HID_OLE_INSERT_NEW               0x0200
#define HID_OLE_EDIT_LINKS               0x0201
#define HID_OLE_VERB_FIRST               0x0210     // range - 16 max

#endif

