// $Id: attr.h 662 2006-03-14 21:31:49Z alex $
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

// Attribute type definitions

#ifndef INC_ATTR
#define INC_ATTR

#include "doccoord.h"

class Path;

/********************************************************************************************
	These are the Attribute type definitions
********************************************************************************************/

/********************************************************************************************

< LineCapType

Comment:
	enum LineCapType
	{
		LineCapButt,
		LineCapRound,
		LineCapSquare
	};
	
********************************************************************************************/

enum LineCapType
{
	LineCapButt,
	LineCapRound,
	LineCapSquare
};

/********************************************************************************************

< JointType

Comment:
	enum JointType
	{
		MitreJoin,
		RoundJoin,
		BevelledJoin	
	};
	
********************************************************************************************/

enum JointType
{
	MitreJoin,
	RoundJoin,
	BevelledJoin	
};

/********************************************************************************************

< WindingType

Comment:
	enum WindingType
	{
		NonZeroWinding,
		NegativeWinding,
		EvenOddWinding,
		PositiveWinding
	};
	
********************************************************************************************/

enum WindingType
{
	NonZeroWinding,
	NegativeWinding,
	EvenOddWinding,
	PositiveWinding
};
   
/********************************************************************************************

< DrawModeType

Comment:
	enum DrawModeType
	{
		DM_COPYPEN = 13,	// These are set to the same numbers as Windows uses, so they can
		DM_EORPEN = 7       // be passed straight to the CDC::SetROP2 function.
	};
	
********************************************************************************************/

enum DrawModeType
{
	DM_COPYPEN = 13,	// These are set to the same numbers as Windows uses, so they can
	DM_EORPEN = 7,      // be passed straight to the CDC::SetROP2 function.

	DM_EORDITHER = 1	// Used for drawing grad fills with EOR - we just want the brush
						// to be set as if we were using DM_COPYPEN, i.e. dithered and
						// not messed about with like it is when we do DM_EORPEN.
};

/********************************************************************************************

< BlobType

Comment:
	MonoOn
	enum BlobType
	{
		BT_UNSELECTED,
		BT_SELECTED,
		BT_SELECTEDLARGEST,							the larger of the two above
		BT_CLICKME,									invisible blob used for click detection
		BT_MSTAGEFILLUNSELECTED,
		BT_MSTAGEFILLSELECTED,
		BT_MSTAGESELECTEDLARGEST,
		BT_CLIPVIEW									refers to ClipView's ToolObject blobs
	};
	MonoOff
	
********************************************************************************************/

enum BlobType
{
	BT_UNSELECTED,
	BT_SELECTED,
	BT_SELECTEDLARGEST,
	BT_CLICKME,
	BT_MSTAGEFILLUNSELECTED,
	BT_MSTAGEFILLSELECTED,
	BT_MSTAGESELECTEDLARGEST,
	BT_CLIPVIEW
};

/********************************************************************************************
	Dash Records.  These define Dash patterns for dotted lines.
********************************************************************************************/

/********************************************************************************************

>	class DashRec : public CCObject

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/10/94
	Purpose:	A class defining a Dash pattern.
	SeeAlso:	-

********************************************************************************************/

enum StockDash
{
// These are old dashes,
// don't use these any more
	SD_DASH = 1,
	SD_DOT,
	SD_DASHDOT,
	SD_DASHDOTDOT,
	SD_FILLMESH,

// New Dashes
	SD_SOLID = 0,
	SD_DASH1,
	SD_DASH2,
	SD_DASH3,
	SD_DASH4,
	SD_DASH5,
	SD_DASH6,
	SD_DASH7,
	SD_DASH8,
	SD_DASH9,
	SD_DASH10,
	SD_DASH11,
	SD_DASH12,
	SD_DASH13,
	SD_DASH14,
	SD_DASH15,
	SD_DASH16,
	SD_DASH17,
	SD_DASH18,
	SD_DASH19,
	SD_DASH20,
	SD_DASH_GUIDELAYER,

	NUM_STOCK_DASHES
};

typedef INT32 DashElement;

class DashRec : public CCObject
{
	CC_DECLARE_DYNCREATE(DashRec)
public:
	DashRec();
	
	INT32 operator==(DashRec&);
	INT32 operator==(StockDash);
	DashRec& operator=(DashRec&);
	DashRec& operator=(StockDash);

// Number of Stock Dashes available
	static INT32 GetNumStockDashes() { return NUM_STOCK_DASHES; }
	static String_256 GetStockDashName(StockDash);

	INT32 GetDashID() { return DashID; }
	BOOL ShouldScaleWithLineWidth() { return ScaleWithLineWidth; }

	void CheckAndFix();
	void CheckIfDefaultPattern();

public:
	INT32 Elements;
	INT32 DashStart;
	DashElement* ElementData;

	INT32 LineWidth;
	BOOL ScaleWithLineWidth;
	INT32 DashID;
};

#endif // INC_ATTR
