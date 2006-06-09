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
// Short Description of what this file is for

#ifndef INC_FRACFILL
#define INC_FRACFILL

//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class KernelBitmap;

/********************************************************************************************

>	class PlasmaFractalFill: public CCObject;

	Author:			   Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:		   12/09/1994
	Purpose:		   A fractal fill class. Static fns only.

********************************************************************************************/


class PlasmaFractalFill: public	CCObject
{
	CC_DECLARE_DYNCREATE(PlasmaFractalFill);

	public:

	PlasmaFractalFill(
		INT32 tSeed = 0,
		BOOL tTileable = TRUE,
		fixed16 tSquash = 0,
		fixed16 tGraininess = 1,
		fixed16 tGravity = 0
		);

	static INT32 GetDimension(INT32 x, INT32 y);

	BOOL DoFill(KernelBitmap * pBitmap, INT32 Dimension=0, INT32 tOx=0, INT32 tOy=0);

	static void Test(KernelBitmap * pB,
		INT32 tSeed = 0,
		BOOL tTileable = TRUE,
		fixed16 tSquash = 0,
		fixed16 tGraininess = 1,
		fixed16 tGravity = 0
		);
	
	private:

	BOOL SubDivide(INT32 x1, INT32 y1, INT32 x2, INT32 y2,
		   						 	  INT32 p11, INT32 p12, INT32 p21, INT32 p22, BOOL PlotNow);

	inline INT32 Adjust(INT32 pa,INT32 pb,INT32 x, INT32 y,
					   INT32 aGraininess, INT32 aGravity);

	void SetSeed(INT32 NewSeed);
	inline INT32 GetRandom();


	KernelBitmap * pBitmap;

	INT32 Seed;
	UINT32 CurrentSeed;
	UINT32 CurrentSeed2;

	INT32 EigenValue;
	
	UINT32 xGraininess;
	UINT32 yGraininess;
	UINT32 xGravity;
	UINT32 yGravity;
	UINT32 CoordinateMask;
	
	INT32 MaxPotential;
	INT32 RecursionLevel;

	INT32 Width;
	INT32 Height;
	INT32 Ox;
	INT32 Oy;

	fixed16 Squash;

	BOOL ForceCornersToZero;
	BOOL Tileable;
 };



#endif  // INC_FRACFILL


