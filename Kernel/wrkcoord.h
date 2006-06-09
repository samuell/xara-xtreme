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
 *  */

/*********************************************************************************************

>	class WorkCoord

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/1993          
	Purpose:	Structure holding a 64-bit cartesian coordinate.
				Its main use is to hold the coordinates of scrolled objects in CCamView.
	Errors:		None.



*********************************************************************************************/

#ifndef INC_WRKCOORD
#define INC_WRKCOORD

#include "ccmaths.h"						// for XLONG
#include "coord.h"							// for Coord

class OSCoord;
class OilCoord;

class CCAPI WorkCoord
{
public:
	XLONG x;
	XLONG y;

	WorkCoord() { };
	WorkCoord( INT32 x, INT32 y);
	WorkCoord( XLONG x, XLONG y);

	OilCoord ToOil(const WorkCoord& scrollOffset);
	void ToOil(const WorkCoord& scrollOffset, OilCoord *result);

	WorkCoord& operator=(const Coord&);
	friend WorkCoord operator-(const WorkCoord&);

	WorkCoord Pixelise(double pixwidthx, double pixwidthy) {return WorkCoord(
		(INT64)((floor(0.5 + (double)x / pixwidthx)) * pixwidthx),
		(INT64)((floor(0.5 + (double)x / pixwidthy)) * pixwidthy)
		);}
};

/********************************************************************************************

>	WorkCoord::WorkCoord( XLONG x, XLONG y )

>	WorkCoord::WorkCoord( INT32 x, INT32 y )

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	Inputs:		x and y positions, as ints or XLONGs
	Outputs:	None
	Purpose:	Constructor that can initialise elements
	Errors:		None

********************************************************************************************/

inline WorkCoord::WorkCoord( XLONG initialX, XLONG initialY )
{
	x = initialX;
	y = initialY;
}

inline WorkCoord::WorkCoord( INT32 initialX, INT32 initialY )
{
	x = (XLONG) initialX;
	y = (XLONG) initialY;
}


/********************************************************************************************

>	WorkCoord& WorkCoord::operator=(const Coord& input)

	Author:		Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/93
	Inputs:		A Coord
	Outputs:	A WorkCoord
	Purpose:	Safe casting between types using operator overloading.
	Errors:		None

********************************************************************************************/


inline WorkCoord& WorkCoord::operator=(const Coord& input)
{
	this->x = input.x;
	this->y = input.y;

	return *this;
}

/********************************************************************************************

>	WorkCoord& operator-(const WorkCoord& input)

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/93
	Inputs:		A WprkCoord
	Outputs:	A WorkCoord
	Purpose:	Negates a WorkCoord.
	Errors:		None

********************************************************************************************/

inline WorkCoord operator-(const WorkCoord& input)
{
	WorkCoord result;

	result.x = -input.x;
	result.y = -input.y;
	return result;
}


#endif
