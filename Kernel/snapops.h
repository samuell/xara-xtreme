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
// Ops that are associated with snapping

#ifndef INC_SNAPOPS
#define INC_SNAPOPS

#include "ops.h"

#define	OPTOKEN_SHOWGRID		_T("ShowGrid")
#define	OPTOKEN_SNAPTOGRID		_T("SnapToGrid")

// WEBSTER - markn 15/1/97
// Removed some op that no longer used in Webster
#ifndef WEBSTER
#define	OPTOKEN_SHOWGUIDES		_T("ShowGuides")
#define	OPTOKEN_SNAPTOOBJECTS	_T("SnapToObjects")
#define	OPTOKEN_SNAPTOGUIDES	_T("SnapToGuides")
#endif

class OpDescriptor;

/*******************************************************************************************

>	class OpShowGrid : public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Purpose:	Allows the user to toggle the visible state of all grids.
	Notes:		-

********************************************************************************************/

class OpShowGrid : public Operation
{
	CC_DECLARE_DYNCREATE( OpShowGrid )

public:
	OpShowGrid();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};


/*******************************************************************************************

>	class OpSnapToGrid : public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/2/94
	Purpose:	Allows the user to toggle snap-to-grid on and off.
	Notes:		-

********************************************************************************************/

class OpSnapToGrid : public Operation
{
	CC_DECLARE_DYNCREATE( OpSnapToGrid )

public:
	OpSnapToGrid();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};

// WEBSTER - markn 15/1/97
// Removed some op that no longer used in Webster
#ifndef WEBSTER

/*******************************************************************************************

>	class OpSnapToObjects : public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/94
	Purpose:	Allows the user to toggle snap-to-objects on and off.
	Notes:		-

********************************************************************************************/

class OpSnapToObjects : public Operation
{
	CC_DECLARE_DYNCREATE( OpSnapToObjects )

public:
	OpSnapToObjects();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};

/*******************************************************************************************

>	class OpSnapToGuides : public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Purpose:	Allows the user to toggle snap-to-guides on and off.
	Notes:		-

********************************************************************************************/

class OpSnapToGuides : public Operation
{
	CC_DECLARE_DYNCREATE( OpSnapToGuides )

public:
	OpSnapToGuides();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};

/*******************************************************************************************

>	class OpShowGuides : public Operation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/9/95
	Purpose:	Allows the user to toggle the visible state of all guides.
	Notes:		-

********************************************************************************************/

class OpShowGuides : public Operation
{
	CC_DECLARE_DYNCREATE( OpShowGuides )

public:
	OpShowGuides();

	void Do(OpDescriptor*);

	static BOOL Init();
	static OpState GetState(String_256*, OpDescriptor*);
};

#endif  // WEBSTER
#endif  // INC_SNAPOPS
