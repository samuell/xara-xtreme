// $Id: binds.h 662 2006-03-14 21:31:49Z alex $
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
	This file binds various platform-indy identifiers to run-time constant numbers.
	It includes things like the identifiers describing different types of click, different
	types of modifiers, etc...

	You should only add things to this file if you know they are not going to change
	frequently. What I mean is don't put anything in here which is allocated on request from
	thrid-parties - things like Tool IDs should NOT go in this file.

*/

/*
 * */

#ifndef INC_BINDS
#define INC_BINDS

/*******************************************************************************************

< ClickType

	Comment:	The kernel only understands pointing-devices with ONE button! DON'T PANIC!
				If a pointing- device has more than one button then the functions of
				the other buttons are encoded as "modifiers" of the simple click state (See
				Modifiers below). The reason for doing things this way is not to be ready for
				the Mac but simply because functions which want to treat all clicks in the
				same way don't have to do any special tests.

				The model of clicking that the kernel expects is that it always receives a
				single-click whether or not further testing decides that a double-click or a
				drag follows. The kernel expects that it will always have received a
				single-click when dealing with a double-click or a drag. It is up to the OIL
				layer to guarantee this!

MonoOn
				enum ClickType { CLICKTYPE_NONE,
								 CLICKTYPE_SINGLE,
								 CLICKTYPE_DOUBLE,
								 CLICKTYPE_DRAG
							    };
MonoOff

	SeeAlso:	ClickModifiers; DocView::OnClick; Tool_v1::OnClick

********************************************************************************************/ 

enum ClickType
{
	CLICKTYPE_NONE, 			// what's this then?
	CLICKTYPE_SINGLE, 			// single click, any button
	CLICKTYPE_DOUBLE, 			// double click, any button
	CLICKTYPE_DRAG,				// mouse move
	CLICKTYPE_UP				// button up, any button
};




/*******************************************************************************************

< ButtonFunction

	Comment:	This enum lists the possible functions that can be assigned to each of the
				mouse buttons via the preferences system.
MonoOn
enum ButtonFunction {
						BUTTFUNC_NORMAL,
						BUTTFUNC_SHIFT,
						BUTTFUNC_CONSTRAIN,
						BUTTFUNC_ALTERNATIVE,
						BUTTFUNC_MENU,
						BUTTFUNC_EDITOBJECT,
						BUTTFUNC_FULLSCREEN,
						BUTTFUNC_ZOOMIN
					};
MonoOff

********************************************************************************************/ 

enum ButtonFunction {
						BUTTFUNC_NORMAL,
						BUTTFUNC_SHIFT,
						BUTTFUNC_CONSTRAIN,
						BUTTFUNC_ALTERNATIVE,
						BUTTFUNC_MENU,
//						BUTTFUNC_EDITOBJECT,
						BUTTFUNC_FULLSCREEN,
						BUTTFUNC_ZOOMIN,
						BUTTFUNC_PUSHCENTRE,

							BUTTFUNC_LAST
					};




/*******************************************************************************************

< DragType

	Comment:	When a drag is started there are various options which control the way
				scrolling occurs when the pointer moves outside the client area.

MonoOn
enum DragType { DRAGTYPE_NOSCROLL,
				DRAGTYPE_AUTOSCROLL,
				DRAGTYPE_DEFERSCROLL,
				DRAGTYPE_OLESCROLL
				};
MonoOff

	SeeAlso:	CCamView::StartDrag

********************************************************************************************/ 

enum DragType { DRAGTYPE_NOSCROLL,
				DRAGTYPE_AUTOSCROLL,
				DRAGTYPE_DEFERSCROLL,
				DRAGTYPE_OLESCROLL
				};




/*******************************************************************************************

< ToolKeyClaimDetails

	Comment:	When a tool claims a modifier key combination for temporary tool selection 
				(e.g. ALT for the selector tool), it uses this structure to indicate the 
				modifier key combination it wishes to claim.  For example, it can claim the 
				"right shift and right control key" combination, or the "left ALT key" 
				combination.
				Note that while control and shift are fairly constant, the "Alternative"
				fields may not always map onto ALT keys, e.g on the Mac, Alternative2
				might be the 'splat' key etc.

MonoOn
union ToolKeyClaimDetails
{
	struct
	{
		BOOL Alternative1 : 1;
		BOOL Alternative2 : 1;
		BOOL LeftShift	  : 1;
		BOOL RightShift   : 1;
		BOOL LeftControl  : 1;
		BOOL RightControl : 1;
	} Keys;

	// Used for direct comparisons
	UINT32 Value;
};
MonoOff

	SeeAlso:	Tool::ClaimToolKey

********************************************************************************************/
/*
union ToolKeyClaimDetails
{
	struct
	{
		BOOL LeftShift	  : 1;
		BOOL RightShift   : 1;
		BOOL LeftControl  : 1;		//	>	how you distinguish between
		BOOL RightControl : 1;		//  >	these is anyone's guess
		BOOL Alternative1 : 1;
		BOOL Alternative2 : 1;
	} Keys;

	// Used for direct comparisons
	UINT32 Value;
};
*/
#endif
