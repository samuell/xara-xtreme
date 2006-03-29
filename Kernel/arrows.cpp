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
// Defines ArrowRec class (a Line ArrowHead Record)

/*
*/

#include "camtypes.h"
#include "arrows.h"
#include "paths.h"
#include "doccoord.h"
#include "fixed16.h"
#include "trans2d.h"
#include "gdraw.h"
#include "grndrgn.h"			// for REAL_GDRAW

//#include "will2.h"

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNCREATE(ArrowRec, CCObject)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

const INT32 ZEROWIDTH_ARROW_LINEWIDTH = (72000/64);

/********************************************************************************************

>	ArrowRec::ArrowRec()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Purpose:	Default constructor for Arrow Record class.
				Create a NULL Arrow Record.
	SeeAlso:	-

********************************************************************************************/

ArrowRec::ArrowRec()
{
	ArrowShape 	= NULL;
	Centre 		= DocCoord(0,0);

	LineWidth	= DEFAULT_ARROW_LINEWIDTH;
	ArrowWidth	= 3;
	ArrowHeight	= 3;

	StartArrow = FALSE;
	ScaleWithLineWidth = TRUE;
	ArrowID = 0;

	m_bExtendPath = TRUE;

	IsNull = TRUE;
}

/********************************************************************************************

>	ArrowRec::ArrowRec(Path* pArrowPath, const DocCoord& ArrowCentre, 
					   INT32 LineWidth = DEFAULT_ARROW_LINEWIDTH, 
				 	   FIXED16 WidthScale = 1, FIXED16 HeightScale = 1)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95

	Inputs:		pArrowPath 	- The Path describing the shape of the ArrowHead.
				Centre 		- The arrowhead centre (relative to the path coords).
				LineWidth 	- The line width that the Arrow head was designed for.
				WidthScale 	- The amount to scale the Arrow Width by (ie. Aspect Ratio)
				HeightScale - The amount to scale the Arrow Height by (ie. Aspect Ratio)

	Purpose:	Constructor for Arrow Record class.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ArrowRec::ArrowRec(Path* pArrowPath, const DocCoord& ArrowCentre, 
				   INT32 ArrowLineWidth, 
			 	   FIXED16 ArrowWidthScale, FIXED16 ArrowHeightScale,
			 	   BOOL Start)
{
	ArrowShape 	= pArrowPath;
	Centre 		= ArrowCentre;

	LineWidth	= ArrowLineWidth;
	ArrowWidth	= ArrowWidthScale;
	ArrowHeight	= ArrowHeightScale;

	StartArrow = Start;

	ScaleWithLineWidth = TRUE;
	ArrowID = -1;

	m_bExtendPath = TRUE;

	IsNull = FALSE;
}

/********************************************************************************************

>	ArrowRec::ArrowRec(ArrowRec& OtherArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Purpose:	Copy constructor for Arrows.
				Makes an ArrowHead identical to the specified one.
	SeeAlso:	-

********************************************************************************************/

ArrowRec::ArrowRec(const ArrowRec& OtherArrow)
{
	*this = OtherArrow;
}

/********************************************************************************************

>	ArrowRec::~ArrowRec()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Purpose:	Destructor for Arrow Record class.
				Deletes the arrow path.
	SeeAlso:	-

********************************************************************************************/

ArrowRec::~ArrowRec()
{
	if (ArrowShape != NULL)
		delete ArrowShape;
}

/********************************************************************************************

>	BOOL ArrowRec::CreateStockArrow(StockArrow ArrowType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		ArrowType - The StockArrow number of the ArrowHead to create.
	Returns:	FALSE, if failed to create the arrow path.
	Purpose:	Constructs a Stock ArrowHead.
	Errors:		Out of Memory.
	SeeAlso:	StockArrow

********************************************************************************************/

BOOL ArrowRec::CreateStockArrow(StockArrow ArrowType)
{
	if (ArrowShape != NULL)
	{
		delete ArrowShape;
		ArrowShape = NULL;
	}

	BOOL ok;
	Path* pPath;

	const INT32 Size  = 3;
	const INT32 Width = (72000/2)*3;

	if (ArrowType >= NUM_STOCK_ARROWS)
		ArrowType = SA_STRAIGHTARROW;

	// Set the ArrowID
	ArrowID = ArrowType;

	switch (ArrowType)
	{
		case SA_STRAIGHTARROW:

			/*
			STRAIGHT
			-9 54 m
			-9 0 l
			-9 -54 l
			117 0 l
			-9 54 l
			-9 54 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(4);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord( -9000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( -9000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(117000,      0));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = FALSE;

			break;

		case SA_ANGLEDARROW:

			/*
			ANGLED
			-26.999 53.999 m
			-9 0 l
			-26.999 -54.001 l
			135 0 l
			-26.999 53.999 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(5);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord(-27000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( -9000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(-27000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(135000,      0));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = FALSE;

			break;


		case SA_ROUNDEDARROW:

			/*
			ROUND
			-9 0 m
			-9 -45 l
			-9 -51.708 2.808 -56.580 9 -54 c
			117 -9 l
			120.916 -7.369 126 -4.242 126 0 c
			126 4.242 120.916 7.369 117 9 c
			9 54 l
			2.808 56.580 -9 51.708 -9 45 c
			-9 0 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(17);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo( DocCoord(  -9000,      0));
			if (ok) ok = pPath->InsertLineTo( DocCoord(  -9000, -45000));
			if (ok) ok = pPath->InsertCurveTo(DocCoord(  -9000, -51708),
											  DocCoord(   2808, -56580),
											  DocCoord(   9000, -54000));
			if (ok) ok = pPath->InsertLineTo( DocCoord( 117000,  -9000));
			if (ok) ok = pPath->InsertCurveTo(DocCoord( 120916,  -7369),
											  DocCoord( 126000,  -4242),
											  DocCoord( 126000,      0));
			if (ok) ok = pPath->InsertCurveTo(DocCoord( 126000,   4242),
											  DocCoord( 120916,   7369),
											  DocCoord( 117000,   9000));
			if (ok) ok = pPath->InsertLineTo( DocCoord(   9000,  54000));
			if (ok) ok = pPath->InsertCurveTo(DocCoord(   2808,  56580),
											  DocCoord(  -9000,  51708),
											  DocCoord(  -9000,  45000));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = FALSE;

			break;
	

		case SA_SPOT:

			/*
			BLOB
			-54 0 m
			-54 29.807 -29.807 54 0 54 c
			29.807 54 54 29.807 54 0 c
			54 -29.807 29.807 -54 0 -54 c
			-29.807 -54 -54 -29.807 -54 0 c
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(14);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo( DocCoord( -54000,      0));
			if (ok) ok = pPath->InsertCurveTo(DocCoord( -54000,  29807),
											  DocCoord( -29807,  54000),
											  DocCoord(      0,  54000));
			if (ok) ok = pPath->InsertCurveTo(DocCoord(  29807,  54000),
											  DocCoord(  54000,  29807),
											  DocCoord(  54000,      0));
			if (ok) ok = pPath->InsertCurveTo(DocCoord(  54000, -29807),
											  DocCoord(  29807, -54000),
											  DocCoord(      0, -54000));
			if (ok) ok = pPath->InsertCurveTo(DocCoord( -29807, -54000),
											  DocCoord( -54000, -29807),
											  DocCoord( -54000,      0));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = TRUE;

			break;
	

		case SA_DIAMOND:

			/*
			DIAMOND
			-63 0 m
			0 63 l
			63 0 l
			0 -63 l
			-63 0 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(5);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord(-63000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(     0,  63000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 63000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(     0, -63000));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = TRUE;

			break;
	
		case SA_ARROWFEATHER:

			/*
			FEATHER
			18 -54 m
			108 -54 l
			63 0 l
			108 54 l
			18 54 l
			-36 0 l
			18 -54 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(7);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord( 18000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(108000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 63000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(108000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 18000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(-36000,      0));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = TRUE;

			break;
	
		case SA_ARROWFEATHER2:

			/*
			FEATHER3
			-35.998 0 m
			18 -54 l
			54 -54 l
			18 -18 l
			27 -18 l
			63 -54 l
			99 -54 l
			63 -18 l
			72 -18 l
			108 -54 l
			144 -54 l
			90 0 l
			144 54 l
			108 54 l
			72 18 l
			63 18 l
			99 54 l
			63 54 l
			27 18 l
			18 18 l
			54 54 l
			18 54 l
			-35.998 0 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(23);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord( -36000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  18000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  54000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  18000, -18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  27000, -18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  63000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  99000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  63000, -18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  72000, -18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 108000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 144000, -54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  90000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 144000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 108000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  72000,  18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  63000,  18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  99000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  63000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  27000,  18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  18000,  18000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  54000,  54000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(  18000,  54000));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(0,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = TRUE;

			break;
	
		case SA_HOLLOWDIAMOND:

			/*
			HOLLOW
			0 45 m
			-45 0 l
			0 -45 l
			45 0 l
			0 45 l
			0 63 m
			-63 0 l
			0 -63 l
			63 0 l
			0 63 l
			*/

			pPath = new Path();
			if (pPath == NULL)
				return FALSE;

			ok = pPath->Initialise(10);
			if (ok) 	 pPath->FindStartOfPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord(     0,  45000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(-45000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(     0, -45000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 45000,      0));
			if (ok) ok = pPath->CloseSubPath();
			if (ok) ok = pPath->InsertMoveTo(DocCoord(     0,  63000));
			if (ok) ok = pPath->InsertLineTo(DocCoord(-63000,      0));
			if (ok) ok = pPath->InsertLineTo(DocCoord(     0, -63000));
			if (ok) ok = pPath->InsertLineTo(DocCoord( 63000,      0));
			if (ok) ok = pPath->CloseSubPath();

			if (!ok)
			{
				delete pPath;
				return FALSE;
			}

			pPath->IsFilled  = TRUE;
			pPath->IsStroked = FALSE;

			ArrowShape 	= pPath;
			Centre 		= DocCoord(-45000,0);
			LineWidth 	= Width;
			ArrowWidth 	= INT32(Size);
			ArrowHeight = INT32(Size);
 			IsNull 		= FALSE;

			StartArrow = TRUE;

			break;

		default:
			ArrowShape 	= NULL;
			Centre 		= DocCoord(0,0);

			LineWidth	= DEFAULT_ARROW_LINEWIDTH;
			ArrowWidth	= 3;
			ArrowHeight	= 3;

			StartArrow 			= FALSE;
			ScaleWithLineWidth 	= TRUE;
			ArrowID 			= SA_NULLARROW;
 			IsNull 				= TRUE;
			break;
	}

	return TRUE;
}

/********************************************************************************************

>	static String_256 ArrowRec::GetStockArrowName(StockArrow ArrowType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Get a description of a Stock ArrowHead.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256 ArrowRec::GetStockArrowName(StockArrow ArrowType)
{
	String_256 ArrowName = String_256(_R(IDS_K_ARROWS_DEFAULTARROW));

	switch (ArrowType)
	{
		case SA_STRAIGHTARROW:
			ArrowName.Load(_R(IDS_K_ARROWS_STRAIGHTARROW));
			break;

		case SA_ANGLEDARROW:
			ArrowName.Load(_R(IDS_K_ARROWS_ANGLEDARROW));
			break;

		case SA_ROUNDEDARROW:
			ArrowName.Load(_R(IDS_K_ARROWS_ROUNDEDARROW));
			break;

		case SA_SPOT:
			ArrowName.Load(_R(IDS_K_ARROWS_SPOT));
			break;

		case SA_DIAMOND:
			ArrowName.Load(_R(IDS_K_ARROWS_DIAMOND));
			break;

		case SA_ARROWFEATHER:
			ArrowName.Load(_R(IDS_K_ARROWS_ARROWFEATHER));
			break;

		case SA_ARROWFEATHER2:
			ArrowName.Load(_R(IDS_K_ARROWS_ARROWFEATHER2));
			break;

		case SA_HOLLOWDIAMOND:
			ArrowName.Load(_R(IDS_K_ARROWS_HOLLOWDIAMOND));
			break;

		default:
			ArrowName = "";
			break;
	}

	return ArrowName;
}

/********************************************************************************************

>	void ArrowRec::SetArrowSize(FIXED16 Width, FIXED16 Height = 0)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Set the size of the ArrowHead
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ArrowRec::SetArrowSize(FIXED16 Width, FIXED16 Height)
{
	ArrowWidth = Width;
	ArrowHeight = (Height > 0) ? Height : Width;
}

/********************************************************************************************

>	ArrowRec& ArrowRec::operator=(const ArrowRec& OtherArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Make one Arrow the same as another.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ArrowRec& ArrowRec::operator=(const ArrowRec& OtherArrow)
{
	if (ArrowShape != NULL)
	{
		delete ArrowShape;		// Get rid of existing arrow path
		ArrowShape = NULL;
	}

	if (OtherArrow.ArrowShape != NULL)
	{
		// Now try and create a new one
		ArrowShape = new Path();
		if (ArrowShape == NULL)
			return (*this);

		// Copy the path data from the Other Arrow into our new path.
		BOOL 	ok = ArrowShape->Initialise(OtherArrow.ArrowShape->GetNumCoords());
		if (ok) ok = ArrowShape->CopyPathDataFrom(OtherArrow.ArrowShape);

		if (!ok)
		{
			// Tidy up if we failed
			delete ArrowShape;
			ArrowShape = NULL;
		}
	}

	Centre 		= OtherArrow.Centre;

	LineWidth	= OtherArrow.LineWidth;
	ArrowWidth	= OtherArrow.ArrowWidth;
	ArrowHeight	= OtherArrow.ArrowHeight;

	StartArrow 	= OtherArrow.StartArrow;
	ArrowID 	= OtherArrow.ArrowID;

	ScaleWithLineWidth = OtherArrow.ScaleWithLineWidth;

	m_bExtendPath = OtherArrow.m_bExtendPath;

	IsNull 		= OtherArrow.IsNull;

	return (*this);
}

/********************************************************************************************

>	ArrowRec& ArrowRec::operator=(StockArrow ArrowType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Make an arrow into a stock arrow.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ArrowRec& ArrowRec::operator=(StockArrow ArrowType)
{
	// Now try and create a new one
	CreateStockArrow(ArrowType);

	return (*this);
}

/********************************************************************************************

>	INT32 ArrowRec::operator==(const ArrowRec& OtherArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Returns:	-
	Purpose:	Test to see if this ArrowHead is the same as another.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 ArrowRec::operator==(const ArrowRec& OtherArrow)
{
	if (ArrowShape == NULL && OtherArrow.ArrowShape == NULL)
		return TRUE;

	if (ArrowShape == NULL || OtherArrow.ArrowShape == NULL)
		return FALSE;

	INT32 NumCoords 		= ArrowShape->GetNumCoords();
	INT32 OtherNumCoords = OtherArrow.ArrowShape->GetNumCoords();

	if (NumCoords != OtherNumCoords)
		return FALSE;

	DocCoord* Coords 	= ArrowShape->GetCoordArray(); 
	PathVerb* Verbs 	= ArrowShape->GetVerbArray(); 

	DocCoord* OtherCoords 	= OtherArrow.ArrowShape->GetCoordArray(); 
	PathVerb* OtherVerbs 	= OtherArrow.ArrowShape->GetVerbArray(); 

	for (INT32 i=0; i<NumCoords; i++)
	{
		if (Coords[i] != OtherCoords[i] || Verbs[i] != OtherVerbs[i])
			return FALSE;
	}

	TRACEUSER( "DavidM", wxT("ArrowRec::operator == %d %d\n"),
		m_bExtendPath, OtherArrow.m_bExtendPath);

	return (Centre 		== OtherArrow.Centre 		&&
			LineWidth	== OtherArrow.LineWidth		&&
			ArrowWidth	== OtherArrow.ArrowWidth	&&
			ArrowHeight	== OtherArrow.ArrowHeight   &&
			StartArrow  == OtherArrow.StartArrow	&&
			m_bExtendPath == OtherArrow.m_bExtendPath);
}

/********************************************************************************************

>	INT32 ArrowRec::operator==(StockArrow ArrowType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Returns:	-
	Purpose:	Test to see if this ArrowHead is the same as another.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

INT32 ArrowRec::operator==(StockArrow ArrowType)
{
	ArrowRec TempArrow;
	TempArrow.CreateStockArrow(ArrowType);
	TempArrow.m_bExtendPath = m_bExtendPath;

	return (*this == TempArrow);
}

/********************************************************************************************

>	Path* ArrowRec::GetArrowPath()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Get the path describing the shape of this ArrowHead.
	SeeAlso:	-

********************************************************************************************/

Path* ArrowRec::GetArrowPath()
{
	return ArrowShape;
}

/********************************************************************************************

>	DocCoord ArrowRec::GetArrowCentre()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Get the centre of the Arrow (relative to the Arrow Path coordinates).
	SeeAlso:	-

********************************************************************************************/

DocCoord ArrowRec::GetArrowCentre()
{
	return Centre;
}

/********************************************************************************************

>	INT32 ArrowRec::GetArrowLineWidth()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Get the line width that this arrow was defined for.
	SeeAlso:	-

********************************************************************************************/

INT32 ArrowRec::GetArrowLineWidth()
{
	return LineWidth;
}

/********************************************************************************************

>	FIXED16 ArrowRec::GetArrowWidth()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Get the width scaling of this arrow head.
				(How much it is stretched horizontally).
	SeeAlso:	-

********************************************************************************************/

FIXED16 ArrowRec::GetArrowWidth()
{
	return ArrowWidth;
}

/********************************************************************************************

>	FIXED16 ArrowRec::GetArrowHeight()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Returns:	-
	Purpose:	Get the Height scaling of this arrow head.
				(How much it is stretched Vertically).
	SeeAlso:	-

********************************************************************************************/

FIXED16 ArrowRec::GetArrowHeight()
{
	return ArrowHeight;
}

/********************************************************************************************

>	BOOL ArrowRec::IsStartArrow()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/4/95
	Returns:	-
	Purpose:	Is this arrow designed to be a Start or End arrow.
	SeeAlso:	-

********************************************************************************************/

BOOL ArrowRec::IsStartArrow()
{
	return StartArrow;
}

BOOL ArrowRec::ShouldScaleWithLineWidth()
{
	return ScaleWithLineWidth;
}

INT32 ArrowRec::GetArrowID()
{
	return ArrowID;
}

/********************************************************************************************

>	DocRect ArrowRec::GetArrowBoundingRect(Path* pParent, INT32 ParentLineWidth, BOOL StartArrow)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		pParent, The Path to get the Arrow Bounds of.
				ParentLineWidth, The current applied LineWidth of the Path.
				StartArrow, TRUE = Calc StartArrow Bounds, FALSE = Calc EndArrow Bounds.

	Returns:	The Union of all the bounds of ArrowHeads on this Path.
	Purpose:	Calculates the Union of all the bounds of ArrowHeads on a Path.
				The path may have many subpaths, so it will union the Bounds of the Arrows
				on the Start or End of each subpath.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocRect ArrowRec::GetArrowBoundingRect(Path* pParent, INT32 ParentLineWidth, BOOL StartArrow)
{
	if (IsNullArrow())
		return DocRect(0,0,0,0);	// Hopefully we won't be asked for our bounds
									// if we are NULL.  But just in case ...

	// We will now try and find the total bounds of the ArrowHeads,
	// on this path.  The path may be made up of many subpaths, so
	// we'll need to union the bounds of each ArrowHead on the end
	// of each subpath.

	DocRect Bounds(0,0,0,0);

	INT32 PathIndex = 0;
	DocCoord ArrowCentre;
	DocCoord ArrowDirection;

	DocCoord* Coords = pParent->GetCoordArray(); 
	PathVerb* Verbs = pParent->GetVerbArray(); 
	INT32 NumCoords = pParent->GetNumCoords();


	// Find the first position of an ArrowHead
	BOOL GotPos = GetFirstArrowPos(StartArrow,
								   Coords, Verbs, NumCoords, 
								   &PathIndex, &ArrowCentre, &ArrowDirection);
	while (GotPos)
	{
		// Union all the Bounds of the Arrows on each subpath
		Bounds = Bounds.Union(FindBoundsAt(ArrowCentre, ArrowDirection, ParentLineWidth));

		// Find the next Arrow position (if there are any more subpaths)
		GotPos = GetNextArrowPos(StartArrow,
								 Coords, Verbs, NumCoords, 
							     &PathIndex, &ArrowCentre, &ArrowDirection);
	}
	
	// Return the Union of all the arrow bounds
	return Bounds;
}

/********************************************************************************************

>	DocRect ArrowRec::FindBoundsAt(const DocCoord& Centre, const DocCoord& Direction,
								   INT32 ParentLineWidth)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		Centre, The Centre point (in Spread Coords) of the Arrow.
				Direction, A Spread coord specifying the arrow direction (Relative to Centre).
				ParentLineWidth, The LineWidth of the Path.

	Returns:	The Bounds of the arrow at this position.
	Purpose:	Finds the Bounds of this ArrowHead at the Specified position.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocRect ArrowRec::FindBoundsAt(const DocCoord& ArrowCentre, const DocCoord& Direction, 
							   INT32 ParentLineWidth)
{
	DocRect Bounds(0,0,0,0);

	// Find a matrix to transform the ArrowHead to this Position.
	Trans2DMatrix Trans;
	GetArrowMatrix(ArrowCentre, Direction, ParentLineWidth, &Trans);

	// Note:
	// We should really be able to ask Gavin to Calculate the Bounds,
	// and pass him this Transform Matrix, but he can't do this at the
	// moment, so we'll have to actually transform the path into
	// a tempory path, and then ask him to calc the bounds of that.

	// Make a tempory path to transform
	Path* TransPath = new Path();
	if (TransPath == NULL)
		return Bounds;

	// Copy the path data from the ArrorHead into our tempory path.
	BOOL 	ok = TransPath->Initialise(ArrowShape->GetNumCoords());
	if (ok) ok = TransPath->CopyPathDataFrom(ArrowShape);

	if (!ok)
	{
		// Tidy up if we failed
		delete TransPath;
		return Bounds;
	}

	// Go transform the Tempory path
	Trans.Transform(TransPath->GetCoordArray(), 
					TransPath->GetNumCoords() );

	BOOL GDrawResult = FALSE;
	
	// Find out what the paths bounding rect is, taking into account
	// any bezier curve thingies. 

	GDrawContext *GD = GRenderRegion::GetStaticDrawContext();
	
	if (GD != NULL)
		GDrawResult = GD->CalcStrokeBBox((POINT*)TransPath->GetCoordArray(),
											TransPath->GetVerbArray(), TransPath->GetNumCoords(),
											(RECT *)(&Bounds),
											TRUE, 0, CAPS_ROUND, JOIN_ROUND, NULL) == 0;

	// If Gavin failed, then use backup technique of getting coord array bounds
	if (!GDrawResult)
		Bounds = TransPath->GetBoundingRect();

	// Delete the temporary transformed path
	delete TransPath;

	return Bounds;
}

/********************************************************************************************

>	BOOL ArrowRec::GetArrowMatrix(const DocCoord& Centre, const DocCoord& Direction, 
			 					  INT32 ParentLineWidth, Trans2DMatrix* pMatrix)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Gets a Transform Matrix for this ArrowHead, given the specified position.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ArrowRec::GetArrowMatrix(const DocCoord& ArrowCentre, const DocCoord& Direction, 
		 				  	  INT32 ParentLineWidth, Trans2DMatrix* pMatrix)
{
	if (IsNullArrow())
		return FALSE;

	TRACEUSER( "DavidM", wxT("ArrowRec::GetArrowMatrix %d\n"), m_bExtendPath );

	ANGLE RotateAngle = CalcAngle(ArrowCentre, Direction);

	// Now we work how much we need to scale the path
	DocRect ArrowBounds = ArrowShape->GetBoundingRect();

	double TempLineWidth = ParentLineWidth;

	if (TempLineWidth == 0)
		TempLineWidth = ZEROWIDTH_ARROW_LINEWIDTH;

	BOOL DoScale = ScaleWithLineWidth;

	FIXED16 xscale = DoScale ? ((ArrowWidth.MakeDouble() * TempLineWidth) / LineWidth)
							 :  (ArrowWidth);
	FIXED16 yscale = DoScale ? ((ArrowHeight.MakeDouble() * TempLineWidth) / LineWidth)
							 :  (ArrowWidth);

	// move the centre of the arrow
	DocRect ArrowRect = ArrowShape->GetBoundingRect();
	
	DocCoord centre;
		
	if (!m_bExtendPath)
	{
		centre.x = ArrowRect.hi.x;
		centre.y = (ArrowRect.lo.y + ArrowRect.hi.y) / 2;
	}
	else
	{
		centre = Centre;
	}
	
	Matrix Mat = Matrix(-centre.x,
    		 			-centre.y);					// Move shape to origin
	Mat *= Matrix(xscale, yscale);   				// Scale it
	Mat *= Matrix(RotateAngle);						// Rotate it

  	Mat *= Matrix(ArrowCentre);						// Move it to the line point

	Trans2DMatrix Trans(Mat);

	*pMatrix = Trans;

	return TRUE;
}

/********************************************************************************************

>	static BOOL ArrowRec::DoesPathNeedArrowHeads(Path* pPath)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Determines whether a path has any open subpaths which will need arrows
				drawn onto.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ArrowRec::DoesPathNeedArrowHeads(DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords)
{
	// Scan for subpaths, and check to see if they're closed or not.
	BOOL FoundCloseFigure = TRUE;

	for (INT32 coord=0; coord<NumCoords; coord++)
	{
		if (Verbs[coord] == PT_MOVETO)
		{
			if (!FoundCloseFigure)
				return TRUE;	// Got a MOVETO before a CLOSEFIGURE, 
								// so must be at least one open subpath

			FoundCloseFigure = FALSE;
		}

		if (Verbs[coord] & PT_CLOSEFIGURE)
		{
			FoundCloseFigure = TRUE;
		}
	}

	if (!FoundCloseFigure)
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	static BOOL ArrowRec::GetFirstArrowPos(Path* pPath, BOOL StartArrow, 
										   INT32* Index, DocCoord* Centre, DocCoord* Direction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Scans a path for the first position on a ArrowHead.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ArrowRec::GetFirstArrowPos(BOOL StartArrow,
								DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords,
								INT32* Index, DocCoord* Centre, DocCoord* Direction)
{
	*Index = 0;  	// Start at the beginning of the path

	// And scan for open subpaths
	return GetNextArrowPos(StartArrow, Coords, Verbs, NumCoords, Index, Centre, Direction);
}

/********************************************************************************************

>	static BOOL ArrowRec::GetNextArrowPos(Path* pPath, BOOL StartArrow, 
									 	  INT32* Index, DocCoord* Centre, DocCoord* Direction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Scans a path for the next position on a ArrowHead.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL ArrowRec::GetNextArrowPos(BOOL StartArrow, 
							   DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords,
						 	   INT32* Index, DocCoord* Centre, DocCoord* Direction)
{
	INT32 coord = *Index;
	INT32 StartPos = -1;		// Look for next subpath start

	while (coord<NumCoords)
	{
		if (Verbs[coord] == PT_MOVETO)
		{
			if (StartPos != -1)
				break;			// Break out, We've found an open subpath !!

			StartPos = coord;	// Remember the Subpath start pos
		}

		if (Verbs[coord] & PT_CLOSEFIGURE)
		{
			StartPos = -1;		// Look for next subpath
		}
	
		coord++;
	}

	if (StartPos != -1)
	{
		// We've found an open subpath !!
		if (StartArrow)
		{
			*Centre 	= Coords[StartPos];
			*Direction 	= Coords[StartPos+1];

			// move the centre back by the direction & the size of the arrow
			
		}
		else
		{
			*Centre 	= Coords[coord-1];
			*Direction 	= Coords[coord-2];
		}

		*Index = coord;
		
		return TRUE;
	}

	// There are no more open subpaths
	return FALSE;
}

/********************************************************************************************

>	static ANGLE ArrowRec::CalcAngle(const DocCoord& Centre, const DocCoord& Direction)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Calculates the angle of a Line, given the Centre point,
				and a point specifying the direction. 0 is due East.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

ANGLE ArrowRec::CalcAngle(const DocCoord& ArrowCentre, const DocCoord& Direction)
{
	ANGLE Angle;

	// Calculate the angle of the arrow from the line angle
	const INT32 dx = Direction.x - ArrowCentre.x;
	const INT32 dy = Direction.y - ArrowCentre.y;

	if ( (dy==0) && (dx==0) )
	{
		return FALSE;			// Too small
	}

	double LineAngle;

	if ( (dx>=0) && (dy>=0) )
		LineAngle = -PI + atan2( (double)dy, (double)dx );
	else if ( (dx<0) && (dy>=0) )
		LineAngle = -atan2( (double)dy, (double)-dx );
	else if ( (dx<0) && (dy<0) )
		LineAngle = atan2( (double)-dy, (double)-dx );
	else if ( (dx>=0) && (dy<0) )
		LineAngle = PI - atan2( (double)-dy, (double)dx );

	else
	{
		TRACE( wxT("invalid angle in CalcAngle\n") );		// this should be impossible
		return FALSE;
	}

	Angle = 360 * (LineAngle / (2*PI));	// Convert from radians to degrees

	return Angle;
}

/********************************************************************************************

>	void ArrowRec::GetBasePoint(DocCoord * pPoint)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/2/99
	Inputs:		The doccoord to put the point into
	Outputs:	-
	Returns:	-
	Purpose:	Gives the base point for this particular arrow (stock arrow)
				Note : HEAVILY dependent on the stock arrow paths
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void ArrowRec::GetArrowBase(DocCoord * pPoint)
{
	if (!pPoint)
		return;

	DocRect ArrowRect = ArrowShape->GetBoundingRect();

	switch (ArrowID)
	{
	case SA_STRAIGHTARROW:
		pPoint->x = -9000;
		pPoint->y = 0;
		break;
	case SA_ANGLEDARROW:
		pPoint->x = -9000;
		pPoint->y = 0;
		break;
	case SA_ROUNDEDARROW:
		pPoint->x = -9000;
		pPoint->y = 0;
		break;
	case SA_SPOT:
		pPoint->x = -30000;
		pPoint->y = (ArrowRect.hi.y + ArrowRect.lo.y) / 2;
		break;
	case SA_DIAMOND:
		pPoint->x = -54000;
		pPoint->y = (ArrowRect.hi.y + ArrowRect.lo.y) / 2;
		break;
	case SA_ARROWFEATHER:
		pPoint->x = -30000;
		pPoint->y = (ArrowRect.hi.y + ArrowRect.lo.y) / 2;
		break;
	case SA_ARROWFEATHER2:
		pPoint->x = -30000;
		pPoint->y = (ArrowRect.hi.y + ArrowRect.lo.y) / 2;
		break;
	case SA_HOLLOWDIAMOND:
		pPoint->x = -54000;
		pPoint->y = (ArrowRect.hi.y + ArrowRect.lo.y) / 2;
		break;
	default:
		pPoint->x = ArrowRect.lo.x;
		pPoint->y = (ArrowRect.hi.y + ArrowRect.lo.y) / 2;
		break;
	}
}

