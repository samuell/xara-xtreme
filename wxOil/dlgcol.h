// $Id: dlgcol.h 662 2006-03-14 21:31:49Z alex $
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
// Support for kernel use of host Operating System dialogue rendering colours


#ifndef INC_DLGCOL
#define INC_DLGCOL


#include "doccolor.h"


/***********************************************************************************************

>	class DialogColourInfo : public CC_CLASS_MEMDUMP

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/11/94

	Purpose:	Most host operating systems supply applications with a set of colours
				to be used for rendering special parts of dialogues, e.g. window
				background, plinth colours, default text foreground/background colours,
				selection highlight colours, etc.
				
				This class provides a means by which kernel entities can find out the
				required colours for various standard dialogue components.

				The methods provided by this class return a reference to a static DocColour,
				as in:	DocColour &DialogBack(void);
				DO NOT alter the contents of any of these colours!

				Note that if you create a DialogColourInfo on the fly each time you redraw,
				the colours are guaranteed to be kept up to date; but if you keep one around
				permanently, then they may not be updated correctly if the host OS changes
				its colour scheme (As can be done under Windows with the control panel).
				Hence, you should either recreate the object, or call its RecacheColours
				method before each redraw.

				The methods provide the following colours:
				MonoOn
					DialogBack()			// Dialogue window background colour

					TextBack()				// Text background colour
					TextFore()				// Text foreground colour

					DisabledTextFore()		// Text foreground colour (when shaded)

					HighlightedTextBack()	// Text background colour (when selected)
					HighlightedTextFore()	// Text foreground colour (when selected)

					ButtonFace()			// Button face colour
					ButtonHighlight()		// Button/plinth highlight colour
					ButtonShadow()			// Button/plinth shadow colour
				MonoOff
				
				Example:
				MonoOn
					if (pRender != NULL)
					{
						DialogColourInfo RedrawColours;
						pRender->SetLineColour(RedrawColours.DialogBack());
						...
					}
				MonoOff

	
	SeeAlso:	DialogOp::GetHostColours

***********************************************************************************************/


class DialogColourInfo : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(DialogColourInfo)

public:
	// Constructor
	DialogColourInfo();

	
	// Call which recaches all the OS colours. Called on windows WM_SYSCOLORCHANGE message
	static void RecacheColours(void);


	// External methods to retrieve the colours
	inline DocColour &DialogBack(void)			{ return mDialogBack; };

	inline DocColour &TextBack(void)			{ return mTextBack; };
	inline DocColour &TextFore(void)			{ return mTextFore; };

	inline DocColour &DisabledTextFore(void)	{ return mDisabledTextFore; };

	inline DocColour &HighlightedTextBack(void)	{ return mHighlightedTextBack; };
	inline DocColour &HighlightedTextFore(void)	{ return mHighlightedTextFore; };

	inline DocColour &ButtonFace(void)			{ return mButtonFace; };
	inline DocColour &ButtonHighlight(void)		{ return mButtonHighlight; };
	inline DocColour &ButtonShadow(void)		{ return mButtonShadow; };


private:
	static DocColour mDialogBack;				// Dialogue window background colour

	static DocColour mTextBack;					// Text background colour
	static DocColour mTextFore;					// Text foreground colour

	static DocColour mDisabledTextFore;			// Text foreground colour (when shaded)

	static DocColour mHighlightedTextBack;		// Text background colour (when selected)
	static DocColour mHighlightedTextFore;		// Text foreground colour (when selected)

	static DocColour mButtonFace;				// Button face colour
	static DocColour mButtonHighlight;			// Button/plinth highlight colour
	static DocColour mButtonShadow;				// Button/plinth shadow colour
};

#endif

