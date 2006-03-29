// $Id$
// $Header: /Camelot/kernel/opnudge.h 2     28/07/00 11:51 Simonk $Descriptor
// Definition of the op nudge classes
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

#ifndef INC_OPNUDGE
#define INC_OPNUDGE
#include "tranlate.h"

#define OPTOKEN_NUDGE 		_T("Nudge")

/********************************************************************************************

>	class OpNudge : public CCObject

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/94
	Purpose:	The underlying nudge op that does a nudge on the selection.	
				Note: Mark initially implemented the twelve different types of nudge as 
					  twelve seperate ops. Simon changed this (07/11/94) so that it now
					  uses only one operation, but has 12 OpDescriptors all of which map
					  to the same operation. We determine the type of nudge we are after
					  from the runtime class of the OpDescriptor in the Do function. 

********************************************************************************************/

class OpNudge : public OpTranslateTrans
{
CC_DECLARE_DYNCREATE(OpNudge);

public:
	static BOOL    Init();
	static OpState GetState(String_256*, OpDescriptor*);

	// Nudge step is the setting people will see in the set up dialogue, etc
	// i.e. it is the underlying nudge step distance
	static void 		SetNudgeStep(MILLIPOINT Step) 	{ NudgeStep = Step; }
	static MILLIPOINT 	GetNudgeStep()					{ return NudgeStep; }
	
	// The actual nudge displacement is the one that takes into account the nudge factor, which
	// provides the magnitude and direction.
	// Use these functions to get the X and Y nudge displacements to apply
	
	//changed from MILLIPOINT to double, to allow 1/5 nudge option  
	double	GetXNudgeDisplacement()			{ return NudgeStep  * X_NudgeFactor; }
	double	GetYNudgeDisplacement()			{ return NudgeStep  * Y_NudgeFactor; }
	double  GetXPixelDisplacement()			{ return PixelNudge * X_NudgeFactor;}
	double  GetYPixelDisplacement()			{ return PixelNudge * Y_NudgeFactor;}

	// Do() will nudge each selected object by GetXNudgeDisplacement() in the x direction
	// and GetYNudgeDisplacement in the y direction
	virtual void Do(OpDescriptor* pOpDesc);

	// Overide GetOpName() so the correct string appears in the undo/redo menu item strings
	void GetOpName(String_256* OpName);

	// Used to merge sequences of nudge operations
	virtual void PerformMergeProcessing() ; 

	// Will the nudge send the selection out of bounds
	BOOL IsNudgeOK(BOOL dx,BOOL dy);
		
	void ScrollToShow(DocCoord Coord);

	BOOL DoesActionListHideNodes(Operation * pOp);

protected:
	double			X_NudgeFactor;	// displacement nudged in x dir = NudgeStep * X_NudgeFactor;
	double			Y_NudgeFactor;	// displacement nudged in y dir = NudgeStep * Y_NudgeFactor;
	
private:
	static MILLIPOINT 	NudgeStep;	// The size of a single nudge (must be a +ve value)
	double PixelNudge;				// The size of a single nudge. 
	BOOL flag;						//Flag to determine whether to nudge in MM or pixels.
};

// Define all the op tokens

#define		OPTOKEN_NUDGEUP1				_T("NudgeUp1")
#define		OPTOKEN_NUDGEUP5				_T("NudgeUp5")
#define		OPTOKEN_NUDGEUP10				_T("NudgeUp10")
#define		OPTOKEN_NUDGEUPFIFTH			_T("NudgeUpFifth") 
#define		OPTOKEN_NUDGEUPPIXEL1			_T("NudgeUpPixel1")
#define		OPTOKEN_NUDGEUPPIXEL10			_T("NudgeUpPixel10")

#define		OPTOKEN_NUDGEDOWN1				_T("NudgeDown1")
#define		OPTOKEN_NUDGEDOWN5				_T("NudgeDown5")
#define		OPTOKEN_NUDGEDOWN10				_T("NudgeDown10")
#define		OPTOKEN_NUDGEDOWNFIFTH			_T("NudgeDownFifth")
#define		OPTOKEN_NUDGEDOWNPIXEL1			_T("NudgeDownPixel1")
#define		OPTOKEN_NUDGEDOWNPIXEL10		_T("NudgeDownPixel10")

#define		OPTOKEN_NUDGELEFT1				_T("NudgeLeft1")
#define		OPTOKEN_NUDGELEFT5				_T("NudgeLeft5")
#define		OPTOKEN_NUDGELEFT10				_T("NudgeLeft10")
#define		OPTOKEN_NUDGELEFTFIFTH			_T("NudgeLeftFifth")
#define		OPTOKEN_NUDGELEFTPIXEL1			_T("NudgeLeftPixel1")
#define		OPTOKEN_NUDGELEFTPIXEL10		_T("NudgeLeftPixel10")

#define		OPTOKEN_NUDGERIGHT1				_T("NudgeRight1")
#define		OPTOKEN_NUDGERIGHT5				_T("NudgeRight5")
#define		OPTOKEN_NUDGERIGHT10			_T("NudgeRight10")
#define		OPTOKEN_NUDGERIGHTFIFTH			_T("NudgeRightFifth")
#define		OPTOKEN_NUDGERIGHTPIXEL1		_T("NudgeRightPixel1")
#define		OPTOKEN_NUDGERIGHTPIXEL10		_T("NudgeRightPixel10")

#endif  // INC_OPNUDGE


