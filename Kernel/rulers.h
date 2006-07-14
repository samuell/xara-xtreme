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
// Encapsulate the kernel'ly bits of the rulers


#ifndef INC_RULERS
#define INC_RULERS

#include "oilruler.h"
#include "userrect.h"
#include "contmenu.h"

class RulerPair;
class DocView;
class NodeRenderableInk;

// WEBSTER - markn 15/1/97
// No rulers in Webster, same as RALPH
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH)

/********************************************************************************************
>	class RulerBase : public CCObject

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/95
	Purpose:	Common base class for a ruler (X and Y derived)
********************************************************************************************/

class RulerBase : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(RulerBase);

public:
	RulerBase();

	RulerPair* GetpRulerPair() { return pRulerPair; }
	OILRuler*  GetpOILRuler()  { return pOILRuler; }

	virtual MILLIPOINT    GetOrd(const UserCoord& coord) = 0;
	virtual UserCoord  MakeCoord(const MILLIPOINT ord)   = 0;
	virtual BOOL    IsHorizontal()                       = 0;

	// Called by the OIL ruler when the ruler needs to be redrawn.
	// We do all the platform independent computations and call the
	// OIL ruler back to render specific things (e.g., graticules)
	BOOL Redraw(OilRect* pOilRect);

	// For use by tools that wish to render extra things on the ruler
	// (by responding to the RenderRulerBlobs method)
	BOOL HighlightSection(MILLIPOINT ord_lo, MILLIPOINT ord_hi);
	BOOL DrawBitmap(MILLIPOINT ord, ResourceID BitmapID);
	// Allow tools to start a drag after receiving an OnRulerClick call.
	// Drag events will be dispatched via OnRulerClick.
	BOOL StartToolDrag(UINT32 nFlags, UserCoord, String_256* pOpToken, OpParam* pParam);

	virtual BOOL OnRulerClick(UINT32 nFlags, OilCoord, ClickType, ClickModifiers);

protected:
	RulerPair* pRulerPair;
	OILRuler*  pOILRuler;
};


/********************************************************************************************
>	class HorizontalRuler : public RulerBase

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/95
	Purpose:	Encapsulates horizontal ruler
********************************************************************************************/

class HorizontalRuler : public RulerBase
{
	CC_DECLARE_MEMDUMP(HorizontalRuler);

public:
	HorizontalRuler();
	BOOL Init(RulerPair* pRulers, OILHorizontalRuler* pOILHRuler);

	virtual MILLIPOINT    GetOrd(const UserCoord& coord) { return coord.x; }
	virtual UserCoord  MakeCoord(const MILLIPOINT ord)   { return UserCoord(ord,0); }
	virtual BOOL    IsHorizontal() { return TRUE; }
};


/********************************************************************************************
>	class VerticalRuler : public RulerBase

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/95
	Purpose:	Encapsulates vertical ruler
********************************************************************************************/

class VerticalRuler : public RulerBase
{
	CC_DECLARE_MEMDUMP(VerticalRuler);

public:
	VerticalRuler();
	BOOL Init(RulerPair* pRulers, OILVerticalRuler* pOILVRuler);

	virtual MILLIPOINT    GetOrd(const UserCoord& coord) { return coord.y; }
	virtual UserCoord  MakeCoord(const MILLIPOINT ord)   { return UserCoord(0,ord); }
	virtual BOOL    IsHorizontal() { return FALSE; }
};


/********************************************************************************************
>	struct RulerRedrawData

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/9/95
	Purpose:	structure containing semi-permenent redraw data
********************************************************************************************/

struct RulerRedrawData
{
	UserRect   PasteBoardUserRect;
	INT32       GratStep;
	double     GratStepSize;
	INT32       MinorGrats;
	double     MinorGratStepSize;
	double     GratUnitSize;
	double     ScaleFactor;
	String_32  LegendStr;
};


/********************************************************************************************
>	class RulerPair : public MessageHandler

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/95
	Purpose:	Encapsulates a horizontal and vertical ruler pair
********************************************************************************************/

class RulerPair : public MessageHandler
{
	CC_DECLARE_MEMDUMP(RulerPair);

public:
	RulerPair();
	BOOL Init(DocView* pDView, OILHorizontalRuler* pOILHRuler, OILVerticalRuler* pOILVRuler,
			  OriginGadget* pOGadget);

	MsgResult Message(Msg* Msg);
	BOOL      GetStatusLineText(String_256* pText, WinCoord MousePos, CWindowID hWnd);
	BOOL      Update();
	BOOL      UpdateRedrawData();

	BOOL     UpdateMouseFollowers(DocCoord* pDocCoord);
	BOOL     GetMouseFollowerVisibility() { return MouseFollowerVisibility; }
	DocCoord GetMouseFollowerPosition()   { return MouseFollowerPosition; }

	DocView*         GetpDocView()         { return pDocView; }
	Spread*          GetpSpread()          { return pSpread; }
	OriginGadget*    GetpOriginGadget()    { return pOriginGadget; }
	HorizontalRuler* GetpHorizontalRuler() { return &HRuler; }
	VerticalRuler*   GetpVerticalRuler()   { return &VRuler; }
	RulerRedrawData* GetpRedrawData()      { return &RedrawData; }

protected:
	DocView*        pDocView;
	Spread*         pSpread;
	OriginGadget*   pOriginGadget;
	HorizontalRuler HRuler;
	VerticalRuler   VRuler;
	RulerRedrawData RedrawData;

	BOOL     MouseFollowerVisibility;
	DocCoord MouseFollowerPosition;
};


/******************************************************************************
>	class RulerContextMenu : public ContextMenu

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Purpose:	Implements the Ruler context sensitive menu
******************************************************************************/

class RulerContextMenu : public ContextMenu
{
	CC_DECLARE_MEMDUMP(RulerContextMenu);

public:
	virtual BOOL Build();
};


/******************************************************************************
>	class OriginContextMenu : public ContextMenu

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Purpose:	Implements the Ruler context sensitive menu
******************************************************************************/

class OriginContextMenu : public ContextMenu
{
	CC_DECLARE_MEMDUMP(OriginContextMenu);

public:
	virtual BOOL Build();
};
#endif
#endif  // WEBSTER
#endif  // INC_RULERS
