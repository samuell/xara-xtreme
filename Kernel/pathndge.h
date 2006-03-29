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
// Definition of the path nudge classes

#ifndef INC_PATHNDGE
#define INC_PATHNDGE

#include "tranlate.h"

#define OPTOKEN_PATHNUDGE 		_T("PathNudge")

/********************************************************************************************

>	class OpPathNudge : public TransOperation

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	The underlying nudge op that does a nudge on the selected points in a path.
	SeeAlso:	class OpNudge

********************************************************************************************/

class OpPathNudge : public TransOperation
{
CC_DECLARE_DYNCREATE(OpPathNudge);

public:
	static BOOL    Init();
	static OpState GetState(String_256*, OpDescriptor*);

	// The actual nudge displacement is the one that takes into account the nudge factor, which
	// provides the magnitude and direction.
	// Use these functions to get the X and Y nudge displacements to apply
	// chnaged from MIILIPOINT to double to get 0.2MM nudging
	static double	GetXNudgeDisplacement()			{ return NudgeStep * X_NudgeFactor; }
	static double	GetYNudgeDisplacement()			{ return NudgeStep * Y_NudgeFactor; }
	double			GetXPixelDisplacement()			{ return PixelNudge * X_NudgeFactor;}
	double			GetYPixelDisplacement()			{ return PixelNudge * Y_NudgeFactor;}
	
	// Do() will nudge each selected object by GetXNudgeDisplacement() in the x direction
	// and GetYNudgeDisplacement in the y direction
	virtual void Do(OpDescriptor* pOpDesc);

	// Overide GetOpName() so the correct string appears in the undo/redo menu item strings
	void GetOpName(String_256* OpName);

	virtual void PerformMergeProcessing();

protected:
	// The X and Y nudge factors are set up by a derived class' Do() function
	// E.g. OpNudgeUp1::Do() would set X_NudgeFactor=0, and Y_NudgeFactor = 1
	static double			X_NudgeFactor;	// displacement nudged in x dir = NudgeStep * X_NudgeFactor;
	static double			Y_NudgeFactor;	// displacement nudged in y dir = NudgeStep * Y_NudgeFactor;

	UINT32 NudgeUndoIDS;

private:
	static	MILLIPOINT 	NudgeStep;		// The size of a single nudge (must be a +ve value)
	BOOL	PixelNudgeClass ;			// Flag to test whether to nudge in MM or PIXELS
	double	PixelNudge;					// The size of a single nudge 

};

#define OPTOKEN_PATHNUDGEUP1		_T("PathNudgeUp1")
#define OPTOKEN_PATHNUDGEUP5		_T("PathNudgeUp5")
#define OPTOKEN_PATHNUDGEUP10		_T("PathNudgeUp10")
#define OPTOKEN_PATHNUDGEUPFIFTH	_T("PathNudgeUpFifth")
#define OPTOKEN_PATHNUDGEUPPIXEL1	_T("PathNudgeUpPixel1")
#define OPTOKEN_PATHNUDGEUPPIEL10	_T("PathNudgeUpPixel10")

#define OPTOKEN_PATHNUDGEDOWN1		 _T("PathNudgeDown1")
#define OPTOKEN_PATHNUDGEDOWN5		 _T("PathNudgeDown5")
#define OPTOKEN_PATHNUDGEDOWN10		 _T("PathNudgeDown10")
#define OPTOKEN_PATHNUDGEDOWNFIFTH	 _T("PathNudgeDownFifth")
#define OPTOKEN_PATHNUDGEDOWNPIXEL1	 _T("PathNudgeDownPixel1")
#define OPTOKEN_PATHNUDGEDOWNPIXEL10 _T("PathNudgeDownPixel10")


#define OPTOKEN_PATHNUDGELEFT1		 _T("PathNudgeLeft1")
#define OPTOKEN_PATHNUDGELEFT5		 _T("PathNudgeLeft5")
#define OPTOKEN_PATHNUDGELEFT10		 _T("PathNudgeLeft10")
#define OPTOKEN_PATHNUDGELEFTFIFTH	 _T("PathNudgeLeftFifth")
#define OPTOKEN_PATHNUDGELEFTPIXEL1	 _T("PathNudgeLeftPixel1")
#define OPTOKEN_PATHNUDGELEFTPIXEL10 _T("PathNudgeLeftPixel10")


#define OPTOKEN_PATHNUDGERIGHT1		  _T("PathNudgeRight1")
#define OPTOKEN_PATHNUDGERIGHT5		  _T("PathNudgeRight5")
#define OPTOKEN_PATHNUDGERIGHT10	  _T("PathNudgeRight10")
#define OPTOKEN_PATHNUDGERIGHTFIFTH	  _T("PathNudgeRightFifth")
#define OPTOKEN_PATHNUDGERIGHTPIXEL1  _T("PathNudgeRightPixel1")
#define OPTOKEN_PATHNUDGERIGHTPIXEL10 _T("PathNudgeRightPixel10")



#define DECLARE_OPPATHNUDGE_CLASS(DIR1) \
	class OpPathNudge ## DIR1 : public OpPathNudge \
	{ \
	CC_DECLARE_DYNCREATE(OpPathNudge ## DIR1) \
	public: \
		virtual void Do(OpDescriptor* pOpDesc); \
	}; \

DECLARE_OPPATHNUDGE_CLASS	(Up1)
DECLARE_OPPATHNUDGE_CLASS	(Up5)
DECLARE_OPPATHNUDGE_CLASS	(Up10)
DECLARE_OPPATHNUDGE_CLASS	(UpFifth)
DECLARE_OPPATHNUDGE_CLASS	(UpPixel1)
DECLARE_OPPATHNUDGE_CLASS	(UpPixel10)

DECLARE_OPPATHNUDGE_CLASS	(Down1)
DECLARE_OPPATHNUDGE_CLASS	(Down5)
DECLARE_OPPATHNUDGE_CLASS	(Down10)
DECLARE_OPPATHNUDGE_CLASS	(DownFifth)
DECLARE_OPPATHNUDGE_CLASS	(DownPixel1)
DECLARE_OPPATHNUDGE_CLASS	(DownPixel10)

DECLARE_OPPATHNUDGE_CLASS	(Left1)
DECLARE_OPPATHNUDGE_CLASS	(Left5)
DECLARE_OPPATHNUDGE_CLASS	(Left10)
DECLARE_OPPATHNUDGE_CLASS	(LeftFifth)
DECLARE_OPPATHNUDGE_CLASS	(LeftPixel1)
DECLARE_OPPATHNUDGE_CLASS	(LeftPixel10)

DECLARE_OPPATHNUDGE_CLASS	(Right1)
DECLARE_OPPATHNUDGE_CLASS	(Right5)
DECLARE_OPPATHNUDGE_CLASS	(Right10)
DECLARE_OPPATHNUDGE_CLASS	(RightFifth)
DECLARE_OPPATHNUDGE_CLASS	(RightPixel1)
DECLARE_OPPATHNUDGE_CLASS	(RightPixel10)


#endif  
