// $Id: bitmapfx.cpp 662 2006-03-14 21:31:49Z alex $
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
// This file implents the BitmapEffect class

/*
*/

#include "camtypes.h"
#include "errors.h"
#include "bitmapfx.h"
#include "bfxalu.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(BitmapEffect, BitmapEffectBase)
CC_IMPLEMENT_DYNCREATE(AccusoftBitmapEffect, BitmapEffect)

CC_IMPLEMENT_DYNCREATE(ABFXFlipX, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXFlipY, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXRotate90, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXRotate180, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXRotate270, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXBayerMono, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXHalftoneMono, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXDiffusionMono, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXBayerColour, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXDiffusionColour, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXPopularityColour, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXMakeGreyscale, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXOctree, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXResize, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXBrightnessContrast, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXSharpenBlur, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXSpecialEffect, AccusoftBitmapEffect)
CC_IMPLEMENT_DYNCREATE(ABFXRemoveDither, AccusoftBitmapEffect)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	KernelBitmap * AccusoftBitmapEffect::DoALUWork()
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Overridden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

KernelBitmap *  AccusoftBitmapEffect::DoALUWork()
{
	ERROR2(FALSE, "Base class AccusoftBitmapEffect::DoALUWork() called");
}

/********************************************************************************************

>	BOOL AccusoftBitmapEffect::SetParameters(INT32 p1=0, INT32 p2=0, INT32 p3=0, double * pD=NULL)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		The generic parameters to set
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Overridden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

/* inline */

/********************************************************************************************

>	KernelBitmap * AccusoftBitmapEffect::GetProcessedBitmap(KernelBitmap * pOriginal)
					
	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/01/95
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Overridden by derived classes
	Errors:		None
	Scope:		Public
	SeeAlso:	-

********************************************************************************************/

KernelBitmap * AccusoftBitmapEffect::GetProcessedBitmap(KernelBitmap * pOriginal)
{
	KernelBitmap * pDest;
	if (!ALU->SetB(pOriginal)) return FALSE;
	pDest=DoALUWork();
	if (!pDest) return FALSE;
	return pDest;
}

// Now the DoALUWork functions. These are so obvious and repetitive I'm not going to doc them all

#define MakeDoALUWork(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result)) return NULL; return Result; }

#define MakeDoALUWorkL2(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result, Param1, Param2)) return NULL; return Result; }

#define MakeDoALUWorkL3(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result, Param1, Param2, Param3)) return NULL; return Result; }

#define MakeDoALUWorkL1(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result, Param1)) return NULL; return Result; }

#define MakeDoALUWorkLP(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result, pDouble, (enum BfxSpecialEffect) (INT32) Param1)) return NULL; return Result; }

#define MakeDoALUWorkL2B(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result, Param1, Param2, (BOOL)Param3)) return NULL; return Result; }

#define MakeDoALUWorkL2T(ALUFunction) \
KernelBitmap * ABFX##ALUFunction::DoALUWork() { KernelBitmap * Result = NULL; \
if (!ALU->ALUFunction(&Result, Param1, Param2, (enum TraceMethod)(INT32)Param3)) return NULL; return Result; }

MakeDoALUWork(FlipX)
MakeDoALUWork(FlipY)
MakeDoALUWork(Rotate90)
MakeDoALUWork(Rotate180)
MakeDoALUWork(Rotate270)
MakeDoALUWork(BayerMono);
MakeDoALUWork(HalftoneMono);
MakeDoALUWork(DiffusionMono);
MakeDoALUWork(BayerColour);
MakeDoALUWork(DiffusionColour);
MakeDoALUWork(PopularityColour);
MakeDoALUWork(MakeGreyscale);
MakeDoALUWork(Octree);
MakeDoALUWorkL2B(Resize);
MakeDoALUWorkL3(BrightnessContrast);
MakeDoALUWorkL2(SharpenBlur);
MakeDoALUWorkLP(SpecialEffect);
MakeDoALUWorkL2T(RemoveDither);
