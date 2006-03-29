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

#ifndef INC_PATHTYPE
#define INC_PATHTYPE


// This file describes the PathFlags and PathVerb types used by the path system
// This enables me to reduce the number of dependencies we have.

typedef BYTE PathVerb;

/********************************************************************************************

<	struct
<	PathFlags

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/94
	Comment:	The various flags associated with each control point on the path. They
				are as follows :-
				IsSelected - TRUE if the control point is selected
				IsSmooth - TRUE if the control point should be smoothed when the path is
				edited
				IsRotate - TRUE if this bezier control point should be kept colinear with
				its partner
				NeedToRender - TRUE if we need to render this control point during an edit
				IsEndPoint - TRUE if this control point is an EndPoint
				Spare1-3 - No specific use as yet but can be used for any localised task
				you perform on a path

********************************************************************************************/

struct PathFlags
{
	PathFlags();

	// Is this element of the path selected
	BYTE IsSelected : 1;

	// Flags to aid with the smoothing of curves
	BYTE IsSmooth : 1;
	BYTE IsRotate : 1;

	// Flags that help with the EORed rendering as the path is edited
	BYTE NeedToRender : 1;

	// All Endpoints in the path have this flag set
	BYTE IsEndPoint : 1;

	// May as well use up the spare bits in this byte for the future
	BYTE Spare1 : 1;
	BYTE Spare2 : 1;
	BYTE Spare3 : 1;
	inline INT32 operator ==(const PathFlags&);
	inline INT32 operator !=(const PathFlags&);
};

INT32 PathFlags::operator==(const PathFlags& rhs)
{
	if ((IsSelected == rhs.IsSelected) && 
		(IsSmooth == rhs.IsSmooth) &&
		(IsRotate == rhs.IsRotate) &&
		(NeedToRender == rhs.NeedToRender) &&
		(IsEndPoint == rhs.IsEndPoint) &&
		(Spare1 == rhs.Spare1) &&
		(Spare2 == rhs.Spare2) &&
		(Spare3 == rhs.Spare2)
	   )
	   		return 1;
	else
			return 0;

}

INT32 PathFlags::operator!=(const PathFlags& rhs)
{
	if ((IsSelected != rhs.IsSelected) || 
		(IsSmooth != rhs.IsSmooth) ||
		(IsRotate != rhs.IsRotate) ||
		(NeedToRender != rhs.NeedToRender) ||
		(IsEndPoint != rhs.IsEndPoint) ||
		(Spare1 != rhs.Spare1) ||
		(Spare2 != rhs.Spare2) ||
		(Spare3 != rhs.Spare2)
	   )
	   		return 1;
	else
			return 0;

}

#endif		// INC_PATHTYPE
