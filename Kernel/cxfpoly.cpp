// $Id: cxfpoly.cpp 662 2006-03-14 21:31:49Z alex $
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
/********************************************************************************************

>	cxfpoly.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Purpose:	Importing and exporting code for polygons to and from the new
				file format.
	SeeAlso:	Class NodeRegularShape

********************************************************************************************/

#include "camtypes.h"

#include "doccoord.h"
#include "paths.h"
#include "nodershp.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "camfiltr.h"
#include "cxfile.h"
#include "cxfrec.h"
#include "cxfrgshp.h"
#include "cxfpoly.h"

/********************************************************************************************

>	BOOL CXaraFilePolygon::WritePolygon(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplex(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexRounded(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexRoundedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexRoundedStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFilePolygon::WritePolygonComplexRoundedStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Inputs:		pFilter - filter to use
				pShape  - regular shape to save to filter
	Returns:	boolean value indicating success of writing record to filter
	Purpose:	Writes a polygon record to the filter

********************************************************************************************/

BOOL CXaraFilePolygon::WritePolygon(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	// we need to determine how is polygon has been altered, i.e. rotated, rounded, stellated
	// or reformed. using this information we jump to one of the following functions to write
	// the information out for us.

	const INT32 REFORMED	= 0x1;
	const INT32 STELLATED	= 0x2;
	const INT32 ROUNDED	= 0x4;

	INT32 PolygonType = 0;

	if (IsReformed(pShape)) PolygonType |= REFORMED;
	if (IsStellated(pShape)) PolygonType |= STELLATED;
	if (IsRounded(pShape)) PolygonType |= ROUNDED;

	switch (PolygonType)
	{
		case 0							: ok = WritePolygonComplex(pFilter, pShape); break;
		case REFORMED					: ok = WritePolygonComplexReformed(pFilter, pShape); break;
		case STELLATED					: ok = WritePolygonComplexStellated(pFilter, pShape); break;
		case STELLATED|REFORMED			: ok = WritePolygonComplexStellatedReformed(pFilter, pShape); break;
		case ROUNDED					: ok = WritePolygonComplexRounded(pFilter, pShape); break;
		case ROUNDED|REFORMED			: ok = WritePolygonComplexRoundedReformed(pFilter, pShape); break;
		case ROUNDED|STELLATED			: ok = WritePolygonComplexRoundedStellated(pFilter, pShape); break;
		case ROUNDED|STELLATED|REFORMED	: ok = WritePolygonComplexRoundedStellatedReformed(pFilter, pShape); break;
		default							: ok = WriteShapeInvalid(pFilter, pShape); break;
	}

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplex(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX, TAG_POLYGON_COMPLEX_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_REFORMED, TAG_POLYGON_COMPLEX_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetUTMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetUTMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteMatrix(GetTransformMatrix(pShape));
	if (ok) ok = Rec.WritePath(GetEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_STELLATED, TAG_POLYGON_COMPLEX_STELLATED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_STELLATED_REFORMED, TAG_POLYGON_COMPLEX_STELLATED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetUTMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetUTMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteMatrix(GetTransformMatrix(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = Rec.WritePath(GetPrimaryEdgePath(pShape));
	if (ok) ok = Rec.WritePath(GetSecondaryEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexRounded(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_ROUNDED, TAG_POLYGON_COMPLEX_ROUNDED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteDOUBLE(GetCurvature(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexRoundedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_ROUNDED_REFORMED, TAG_POLYGON_COMPLEX_ROUNDED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetUTMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetUTMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteMatrix(GetTransformMatrix(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetCurvature(pShape));
	if (ok) ok = Rec.WritePath(GetEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexRoundedStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_ROUNDED_STELLATED, TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetPrimaryCurvature(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetSecondaryCurvature(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFilePolygon::WritePolygonComplexRoundedStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED, TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteUINT16(GetNumberOfSides(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetUTMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetUTMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteMatrix(GetTransformMatrix(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetPrimaryCurvature(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetSecondaryCurvature(pShape));
	if (ok) ok = Rec.WritePath(GetPrimaryEdgePath(pShape));
	if (ok) ok = Rec.WritePath(GetSecondaryEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

