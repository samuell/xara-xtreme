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
// OpImgSet - Imagesetting related Ops

#ifndef INC_OPIMGSET
#define INC_OPIMGSET

#include "colplate.h"
#include "selop.h"

// The following Ops are defined in this file:
#define OPTOKEN_COMPOSITEPREVIEW	_T("CompositePreview")
#define OPTOKEN_IMAGESETTING		_T("ImagesettingSubmenu")
#define OPTOKEN_OVERPRINTFILL		_T("OverprintFill")
#define OPTOKEN_OVERPRINTLINE		_T("OverprintLine")
#define OPTOKEN_PRINTONALLPLATES	_T("PrintOnAllPlates")

#define OPTOKEN_CYANPREVIEW			_T("CyanPreview")
#define OPTOKEN_MAGENTAPREVIEW		_T("MagentaPreview")
#define OPTOKEN_YELLOWPREVIEW		_T("YellowPreview")
#define OPTOKEN_KEYPREVIEW			_T("KeyPreview")
#define OPTOKEN_MONOPLATEPREVIEW	_T("MonoPlatePreview")
#define OPTOKEN_SHOWPRINTERCOLOURS	_T("ShowPrinterColours")
#define OPTOKEN_CANCELPREVIEW		_T("CancelPreview")

#define OPTOKEN_SPOTPREVIEWBASE		_T("SpotPreview")
#define OPTOKEN_SPOTPREVIEW1		_T("SpotPreview1")
#define OPTOKEN_SPOTPREVIEW2		_T("SpotPreview2")
#define OPTOKEN_SPOTPREVIEW3		_T("SpotPreview3")
#define OPTOKEN_SPOTPREVIEW4		_T("SpotPreview4")
#define OPTOKEN_SPOTPREVIEW5		_T("SpotPreview5")
#define OPTOKEN_SPOTPREVIEW6		_T("SpotPreview6")
#define OPTOKEN_SPOTPREVIEW7		_T("SpotPreview7")
#define OPTOKEN_SPOTPREVIEW8		_T("SpotPreview8")
#define OPTOKEN_SPOTPREVIEWMORE		_T("SpotPreviewMore")

// Centralised function which inits all of the other Ops in this file
extern BOOL InitImagesettingOps(void);
void SetNewColourPlate(ColourPlateType NewType, INT32 WhichToggle, INT32 WhichSpot);
IndexedColour* FindWhichSpot(INT32 WhichSpot);


/**************************************************************************************

>	class OpToggleCompositePreview : public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96

	Purpose:	An Op for toggling composite print preview on and off

	Notes:		This Op is available from the main "Window" menu

**************************************************************************************/

class OpShowPrinterColours : public Operation
{
	CC_DECLARE_DYNCREATE(OpShowPrinterColours)

public:
	OpShowPrinterColours() : Operation() {};

	void Do(OpDescriptor* OpDesc);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);
};


/**************************************************************************************

>	class OpToggleCompositePreview : public Operation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/6/96

	Purpose:	An Op for toggling composite print preview on and off

	Notes:		This Op is available from the main "Window" menu

**************************************************************************************/

class OpToggleCompositePreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleCompositePreview)

public:
	OpToggleCompositePreview();

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInCompositeMode(void);
};


/**************************************************************************************

>	class OpToggleCyanPreview : public Operation

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	An Op for toggling cyan plate print preview on and off

	Notes:		This Op is available from the "Window/Show Printer Colours" submenu

**************************************************************************************/

class OpToggleCyanPreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleCyanPreview)

public:
	OpToggleCyanPreview() {};

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInCyanMode(void);
};


/**************************************************************************************

>	class OpToggleMagentaPreview : public Operation

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	An Op for toggling Magenta plate print preview on and off

	Notes:		This Op is available from the "Window/Show Printer Colours" submenu

**************************************************************************************/

class OpToggleMagentaPreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleMagentaPreview)

public:
	OpToggleMagentaPreview() {};

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInMagentaMode(void);
};


/**************************************************************************************

>	class OpToggleYellowPreview : public Operation

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	An Op for toggling Yellow plate print preview on and off

	Notes:		This Op is available from the "Window/Show Printer Colours" submenu

**************************************************************************************/

class OpToggleYellowPreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleYellowPreview)

public:
	OpToggleYellowPreview() {};

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInYellowMode(void);
};


/**************************************************************************************

>	class OpToggleKeyPreview : public Operation

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	An Op for toggling Key plate print preview on and off

	Notes:		This Op is available from the "Window/Show Printer Colours" submenu

**************************************************************************************/

class OpToggleKeyPreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleKeyPreview)

public:
	OpToggleKeyPreview() {};

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInKeyMode(void);
};


/**************************************************************************************

>	class OpToggleSpotPreview : public Operation

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	An Op for toggling Spot plate print preview on and off

	Notes:		This Op is available from the "Window/Show Printer Colours" submenu

**************************************************************************************/

class OpToggleSpotPreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleSpotPreview)

public:
	OpToggleSpotPreview() {};

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInSpotMode(void);
};


/**************************************************************************************

>	class OpToggleMonoPlatePreview : public Operation

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/2003

	Purpose:	An Op for toggling mono plate print preview on and off

	Notes:		This Op is available from the "Window/Show Printer Colours" submenu

**************************************************************************************/

class OpToggleMonoPlatePreview : public Operation
{
	CC_DECLARE_DYNCREATE(OpToggleMonoPlatePreview)

public:
	OpToggleMonoPlatePreview() {};

	void Do(OpDescriptor*);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

protected:
	static BOOL CanToggleState(void);
	static BOOL IsInMonoMode(void);
};



/**************************************************************************************

>	class OpOverprintLine : public SelOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	An Op for toggling the AttrOverprintLine attribute

	Notes:		This Op is available from the context menu, when popped up
				over a selection. It resides in the "Imagesetting" submenu.
				See viewmenu.cpp for details.

**************************************************************************************/

class OpOverprintLine : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpOverprintLine)

public:
	OpOverprintLine();

	void Do(OpDescriptor *OpDesc);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};



/**************************************************************************************

>	class OpOverprintFill : public SelOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	An Op for toggling the AttrOverprintFill attribute

	Notes:		This Op is available from the context menu, when popped up
				over a selection. It resides in the "Imagesetting" submenu.
				See viewmenu.cpp for details.

**************************************************************************************/

class OpOverprintFill : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpOverprintFill)

public:
	OpOverprintFill();

	void Do(OpDescriptor *OpDesc);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};



/**************************************************************************************

>	class OpPrintOnAllPlates : public SelOperation

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96

	Purpose:	An Op for toggling the AttrPrintOnAllPlates attribute

	Notes:		This Op is available from the context menu, when popped up
				over a selection. It resides in the "Imagesetting" submenu.
				See viewmenu.cpp for details.

**************************************************************************************/

class OpPrintOnAllPlates : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpPrintOnAllPlates)

public:
	OpPrintOnAllPlates();

	void Do(OpDescriptor *OpDesc);

	static BOOL Init(void);
	static OpState GetState(String_256 *ShadeReason, OpDescriptor *OpDesc);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }
};

#endif

