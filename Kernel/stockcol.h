// $Id: stockcol.h 662 2006-03-14 21:31:49Z alex $
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

// StockCol.h - definition of the available stock colours
//
// Created: 21/04/94 by Jason
//

#ifndef INC_STOCKCOL
#define INC_STOCKCOL


/********************************************************************************************

< StockColour

	Comment:	Stock Colours are currently being rebuilt
				See the top of kernel\colour.h for details on the new 
				stock colours.

				Anyway, the old ones are TRANS, BLACK, GREY, etc etc
				and you must not use them any more. In fact, they'll 
				generate ENSURE failures in the near future.

				The new ones are:

					COLOUR_NONE		(100% transparent, i.e. 'no colour')
					COLOUR_TRANS

					COLOUR_BLACK

					COLOUR_DKGREY
					COLOUR_MIDGREY
					COLOUR_LTGREY
					COLOUR_GREY		(Same as LTGREY)

					COLOUR_WHITE

					COLOUR_RED
					COLOUR_GREEN
					COLOUR_BLUE	

					COLOUR_CYAN
					COLOUR_MAGENTA
					COLOUR_YELLOW

				However, you shouldn't need the above ones, as you should
				generally be using one of the following, which will probably
				become user preferences one day.

					COLOUR_UNSELECTEDBLOB 	// Drag blob (handle) things
					COLOUR_SELECTEDBLOB	  	// (when selected & unselected)
	
					COLOUR_BEZIERBLOB		// Bezier control blob
					COLOUR_BEZIERLINE	  	// Bezier tangent line

					COLOUR_XORNEW		  	// XOR outline colours (when...
					COLOUR_XOREDIT		  	// creating new object, editing
					COLOUR_XORDRAG		  	// existing object, moving/rotating,
					COLOUR_XORSELECT	  	// dragging a selection rectangle)

					COLOUR_GRIDMAJOR	  	// Grid crosses & points
					COLOUR_GRIDMINOR  

					COLOUR_TOOLBLOB	  		// Tool blobs (center of rotation etc)

					COLOUR_LINEDEFAULT	  	// Document default line colour
					COLOUR_FILLDEFAULT	  	// Document default fill colour

				If you are the slightest bit unsure which one to use, ask
				Jason immediately. Not that he'll have a clue either, mind...

********************************************************************************************/


enum StockColour
{
	// **** Deprecated colours - DO NOT USE any from this first block
	TRANS,
	BLACK,
	DKGREY,
	MIDGREY,
	LTGREY,
	GREY = LTGREY,
	WHITE,
	RED,
	GREEN,
	BLUE,
	CYAN,
	MAGENTA,
	YELLOW,




	// Predefined SCREEN colours that may one day become user preferences
	COLOUR_UNSELECTEDBLOB,			// Drag blob (handle) things
	COLOUR_SELECTEDBLOB,			// (when selected & unselected)
	
	COLOUR_BEZIERBLOB,				// Bezier control blob
	COLOUR_BEZIERLINE,				// Bezier tangent line

	COLOUR_XORNEW,					// XOR outline colours (when...
	COLOUR_XOREDIT,					// creating new object, editing
	COLOUR_XORDRAG,					// existing object, moving/rotating,
	COLOUR_XORSELECT,				// dragging a selection rectangle)

	COLOUR_GRID,					// Grid crosses & points

	COLOUR_TOOLBLOB,				// Tool blobs (center of rotation etc)

	// Predefined DOCUMENT colours
	COLOUR_LINEDEFAULT,			  	// Document default line colour
	COLOUR_FILLDEFAULT,			  	// Document default fill colour



	// Predefined stock colours
	COLOUR_NONE		= TRANS,					// } No colour (fully transparent)
	COLOUR_TRANS	= TRANS,					// }
	COLOUR_BLACK	= BLACK,
	COLOUR_DKGREY	= DKGREY,
	COLOUR_MIDGREY	= MIDGREY,
	COLOUR_LTGREY	= LTGREY,
	COLOUR_GREY		= LTGREY,
	COLOUR_WHITE	= WHITE,
	COLOUR_RED		= RED,
	COLOUR_GREEN	= GREEN,
	COLOUR_BLUE		= BLUE,
	COLOUR_CYAN		= CYAN,
	COLOUR_MAGENTA	= MAGENTA,
	COLOUR_YELLOW	= YELLOW
};


#endif


