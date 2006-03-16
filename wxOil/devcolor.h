// $Id: devcolor.h 662 2006-03-14 21:31:49Z alex $
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
// devcolor.h - Interface between kernel DocColours and ColourContexts and the
// outside world. Currently this is a very thin veneer over functions supplied
// in the RGBT colour context, but this may change in ported versions of Camelot
// and/or as other output devices are supoprted by the rendering subsystem.


#ifndef INC_DEVCOLOR
#define INC_DEVCOLOR


#include "colcontx.h"

/****************************************************************************

>	inline DWORD ConvertColourToScreenWord(ColourContext *cc, DocColour *col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94

	Inputs:		cc  - An *RGBT* colourContext for screen output
				col - The Colour to be converted
	Outputs:	-
	Returns:	An RGB value suitable for passing to the host OS
				- In this case, an rgb DWORD as used by Windows GDI (0x00BBGGRR)

	Purpose:	Converts the given DocColour into the given screen (RGB) output
				context for whatever host OS we are running under. This function
				does NOT include any transparency information in the result.

	Notes:		Note that to save poking about in the internal DocColour
				representations, a function is supplied by the ColourContextRGBT
				class which just happens (!) to suit our needs. However, I've
				wrapped the call up in this WinOil veneer so that on other OS's
				it will not be necessary to touch that kernel code (just write
				equivalent code to go into here)

****************************************************************************/

inline DWORD ConvertColourToScreenWord(ColourContext *cc, DocColour *col)
{
	return( ((ColourContextRGBT *)cc)->ConvertToScreenWord(col) );
}



/****************************************************************************

>	inline DWORD ConvertColourToTransScreenWord(ColourContext *cc, DocColour *col)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94

	Inputs:		cc  - An *RGBT* colourContext for screen output
				col - The Colour to be converted
	Outputs:	-
	Returns:	An RGB value suitable for passing to the host OS
				- In this case, an rgb DWORD as used by GAVIN (0xTTBBGGRR)
				NOTE that this word must NOT be passed to Windows GDI, as GDI
				uses the MS byte in a different manner to Gavin.

	Purpose:	Converts the given DocColour into the given screen (RGB) output
				context for whatever host OS we are running under, including
				the transparent component as well.

	Notes:		Note that to save poking about in the internal DocColour
				representations, a function is supplied by the ColourContextRGBT
				class which just happens (!) to suit our needs. However, I've
				wrapped the call up in this WinOil veneer so that on other OS's
				it will not be necessary to touch that kernel code (just write
				equivalent code to go into here)

****************************************************************************/

inline DWORD ConvertColourToTransScreenWord(ColourContext *cc, DocColour *col)
{
	return( ((ColourContextRGBT *)cc)->ConvertToTransparentScreenWord(col) );
}

#endif

