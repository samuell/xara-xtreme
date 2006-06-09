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

// Context sensitive menu for SuperGalleries


#ifndef INC_SGMENU
#define INC_SGMENU

#include "contmenu.h"
//#include "ops.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "sgallery.h" - in camtypes.h [AUTOMATICALLY REMOVED]



// --- Gallery menu commands - placed here for all to see, and to reduce dependencies
//   + Useful strings for "standard" commands - these must be handled entirely by the derived class
const String_32 SGCmd_Apply( _T("Apply") );
const String_32 SGCmd_New( _T("New") );
const String_32 SGCmd_Rename( _T("Rename") );
const String_32 SGCmd_Redefine( _T("Redefine") );
const String_32 SGCmd_Edit( _T("Edit") );
const String_32 SGCmd_Copy( _T("Copy") );
const String_32 SGCmd_Delete( _T("Delete") );
const String_32 SGCmd_Create( _T("Create") );

//  + Strings for specific galleries - these are handled only by the galleries that use them
const String_32 SGCmd_Add( _T("Add") );					// Add/Remove libraries (font/fill/clipart)
const String_32 SGCmd_Remove( _T("Remove") );
const String_32 SGCmd_RemoveWebFonts( _T("Get") );

const String_32 SGCmd_XPE_Edit( _T("XPEEdit") );			// Bitmap Trace/Effects/Save/Properties
const String_32 SGCmd_Trace( _T("Trace") );				// Bitmap Trace/Effects/Save/Properties
const String_32 SGCmd_Effects( _T("Effects") );
const String_32 SGCmd_Save( _T("Save") );
//	WEBSTER-ranbirr-22/11/96
const String_32 SGCmd_Props( _T("BmpProperties") );
const String_32 SGCmd_FrameProps( _T("Frame properties") );

const String_32 SGCmd_Install( _T("Install") );			// Font install/deinstall
const String_32 SGCmd_Deinstall( _T("Deinstall") );
const String_32 SGCmd_StopLoading( _T("StopLoading") );
const String_32 SGCmd_EmptyClipartCache( _T("EmptyClpCache") );
const String_32 SGCmd_EmptyFontsCache( _T("EmptyFntCache") );
const String_32 SGCmd_EmptyFillsCache( _T("EmptyFlsCache") );

const String_32 SGCmd_Open( _T("Open") );
const String_32 SGCmd_Import( _T("Import") );
const String_32 SGCmd_Insert( _T("Insert") );
const String_32 SGCmd_Fill( _T("Fill") );
const String_32 SGCmd_Transp( _T("Transp") );
const String_32 SGCmd_Download( _T("Download") );

const String_32 SGCmd_PlugIns( _T("PlugIns") );				// new bitmap plug-ins menu

const String_32 SGCmd_SetBackground( _T("Background") );		// Set background
const String_32 SGCmd_ShowInColourLine( _T("ColourLine") );	// Show group in colour line
const String_32 SGCmd_Preview( _T("Preview") );				// Preview

const String_32 SGCmd_Animation( _T("Animation") );			// Animation properties
const String_32 SGCmd_Browser( _T("Browser") );				// preview in browser

const String_32 SGCmd_Show( _T("Show") );						// show frame in animation

// Name gallery commands.
const String_32 SGCmd_RemoveNames( _T("RemoveNames") );		// remove names from selection
const String_32 SGCmd_Select( _T("Select") );					// select union of attributes
const String_32 SGCmd_Intersect( _T("Intersect") );			// select intersection of attributes
const String_32 SGCmd_Export( _T("Export") );					// export named objects

// Line Gallery commands	
const String_32 SGCmd_ExportBrush( _T("ExportBrush") );        // exports brush to clipboard
const String_32 SGCmd_RenameBrush( _T("RenameBrush") );

//   + Standard commands that are handled by the base class (all you need to do is add a menu
//     command of this name, and the base class will handle it for you)
const String_32 SGCmd_Find( _T("Find") );
const String_32 SGCmd_Sort( _T("Sort") );
const String_32 SGCmd_Properties( _T("Properties") );

const String_32 SGCmd_FoldGroup( _T("FoldGroup") );
const String_32 SGCmd_UnfoldGroup( _T("UnfoldGroup") );
const String_32 SGCmd_NextGroup( _T("NextGroup") );
const String_32 SGCmd_PrevGroup( _T("PrevGroup") );


// With the new GIF Animation frame gallery, this can mean that the OpToken can easily
// overrun its String_32 allocation. Therefore, restrict the name to the first n characters
// defined by the following constant.
const INT32 GalleryBarNameLimitSize = 16;

/********************************************************************************************

>	class GalleryContextMenu : public ContextMenu

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95
	Purpose:	Implements the SuperGallery context sensitive menus

	SeeAlso:	SuperGallery::BuildCommandMenu

********************************************************************************************/

class GalleryContextMenu : public ContextMenu
{
	CC_DECLARE_MEMDUMP(GalleryContextMenu);

public:
	GalleryContextMenu();
			// Default constructor - DO NOT CALL this constructor - use the one below

	GalleryContextMenu(SGMenuID TheMenuType, SuperGallery *ParentGallery);
			// Constructs a menu object, letting it know what kind of menu it should build

	virtual BOOL Build(void);
			// Builds a pop-up menu over the gallery list

protected:
	SuperGallery *ParentGallery;			// The gallery to call to build the menu
	SGMenuID MenuType;						// The type (options or list-popup) of menu
};




/********************************************************************************************

>	class OpGalleryCommand : public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/95

	Purpose:	A polymorphic Op which is used for all generic gallery "commands".

				A gallery command is an OpToken along the lines "Colour gallery:DoSomething"
				where "Colour gallery" is the gallery bar name, and "DoSomething" is a command
				supported by either the "Colour gallery" bar or the SuperGallery base class.

	SeeAlso:	SuperGallery::DoMenuCommand; SuperGallery::GetMenuCommandState;
				SuperGallery::GetMenuCommandName

********************************************************************************************/

class OpGalleryCommand : public Operation
{
CC_DECLARE_DYNCREATE(OpGalleryCommand);

public:
	// There is no Init() method for this base class

	static BOOL InitPolymorphicCommand(StringBase *OpToken, UINT32 MenuTextID);
				// Called by SuperGallery::InitMenuCommand to create another OpDescriptor
				// for this Op.

	static OpState GetCommandState(String_256 *ShadeReason, OpDescriptor *pOpDesc);
				// The GetState method goes by a different name to avoid clashes with
				// derived classes. This one calls the parent gallery to determine the state.
				
	virtual void Do(OpDescriptor* pOpDesc);
				// Calls the parent gallery with a command based on the OpDescriptor OpToken
				// (see GetGalleryAndCommand, below)


protected:		// Derived class interface
	static SuperGallery *GetGalleryAndCommand(OpDescriptor *pOpDesc, String_32 *Command);
				// Determines the gallery and command-string given the OpDescriptor we were invoked with
};


#endif

