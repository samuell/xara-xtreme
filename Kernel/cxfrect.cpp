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
/********************************************************************************************

>	cxfrect.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Purpose:	Importing and exporting code for rectangles to and from the new
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
#include "cxfrect.h"

/********************************************************************************************

>	BOOL CXaraFileRectangle::WriteRectangle(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimple(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleRounded(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleRoundedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleRoundedStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleSimpleRoundedStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplex(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexRounded(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexRoundedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexRoundedStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
	BOOL CXaraFileRectangle::WriteRectangleComplexRoundedStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pFilter - filter to use
				pShape  - regular shape to save to filter
	Returns:	boolean value indicating success of writing record to filter
	Purpose:	Writes a regular shape record to the filter

********************************************************************************************/

BOOL CXaraFileRectangle::WriteRectangle(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	// we need to determine how is rectangle has been altered, i.e. rotated, rounded, stellated
	// or reformed. using this information we jump to one of the following functions to write
	// the information out for us.

	const INT32 REFORMED	= 0x1;
	const INT32 STELLATED	= 0x2;
	const INT32 ROUNDED	= 0x4;
	const INT32 COMPLEX	= 0x8;

	INT32 RectangleType = 0;

	if (IsReformed(pShape)) RectangleType |= REFORMED;
	if (IsStellated(pShape)) RectangleType |= STELLATED;
	if (IsRounded(pShape)) RectangleType |= ROUNDED;
	if (IsComplex(pShape)) RectangleType |= COMPLEX;

	switch (RectangleType)
	{
		case 0									: ok = WriteRectangleSimple(pFilter, pShape); break;
		case REFORMED							: ok = WriteRectangleSimpleReformed(pFilter, pShape); break;
		case STELLATED							: ok = WriteRectangleSimpleStellated(pFilter, pShape); break;
		case STELLATED|REFORMED					: ok = WriteRectangleSimpleStellatedReformed(pFilter, pShape); break;
		case ROUNDED							: ok = WriteRectangleSimpleRounded(pFilter, pShape); break;
		case ROUNDED|REFORMED					: ok = WriteRectangleSimpleRoundedReformed(pFilter, pShape); break;
		case ROUNDED|STELLATED					: ok = WriteRectangleSimpleRoundedStellated(pFilter, pShape); break;
		case ROUNDED|STELLATED|REFORMED			: ok = WriteRectangleSimpleRoundedStellatedReformed(pFilter, pShape); break;
		case COMPLEX							: ok = WriteRectangleComplex(pFilter, pShape); break;
		case COMPLEX|REFORMED					: ok = WriteRectangleComplexReformed(pFilter, pShape); break;
		case COMPLEX|STELLATED					: ok = WriteRectangleComplexStellated(pFilter, pShape); break;
		case COMPLEX|STELLATED|REFORMED			: ok = WriteRectangleComplexStellatedReformed(pFilter, pShape); break;
		case COMPLEX|ROUNDED					: ok = WriteRectangleComplexRounded(pFilter, pShape); break;
		case COMPLEX|ROUNDED|REFORMED			: ok = WriteRectangleComplexRoundedReformed(pFilter, pShape); break;
		case COMPLEX|ROUNDED|STELLATED			: ok = WriteRectangleComplexRoundedStellated(pFilter, pShape); break;
		case COMPLEX|ROUNDED|STELLATED|REFORMED	: ok = WriteRectangleComplexRoundedStellatedReformed(pFilter, pShape); break;
		default									: ok = WriteShapeInvalid(pFilter, pShape); break;
	}

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimple(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE, TAG_RECTANGLE_SIMPLE_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimpleReformed(BaseCamelotFilter *pFilter, NodeRegularShape *pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_REFORMED, TAG_RECTANGLE_SIMPLE_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WritePath(GetEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}
	
BOOL CXaraFileRectangle::WriteRectangleSimpleStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_STELLATED, TAG_RECTANGLE_SIMPLE_STELLATED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimpleStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED, TAG_RECTANGLE_SIMPLE_STELLATED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = Rec.WritePath(GetPrimaryEdgePath(pShape));
	if (ok) ok = Rec.WritePath(GetSecondaryEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimpleRounded(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_ROUNDED, TAG_RECTANGLE_SIMPLE_ROUNDED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetCurvature(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimpleRoundedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED, TAG_RECTANGLE_SIMPLE_ROUNDED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetCurvature(pShape));
	if (ok) ok = Rec.WritePath(GetEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimpleRoundedStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED, TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetPrimaryCurvature(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetSecondaryCurvature(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleSimpleRoundedStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED, TAG_RECTANGLE_SIMPLE_ROUNDED_STELLATED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteINT32(GetWidth(pShape));
	if (ok) ok = Rec.WriteINT32(GetHeight(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetPrimaryCurvature(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetSecondaryCurvature(pShape));
	if (ok) ok = Rec.WritePath(GetPrimaryEdgePath(pShape));
	if (ok) ok = Rec.WritePath(GetSecondaryEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleComplex(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX, TAG_RECTANGLE_COMPLEX_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleComplexReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_REFORMED, TAG_RECTANGLE_COMPLEX_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoordTrans(GetUTMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetUTMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteMatrix(GetTransformMatrix(pShape));
	if (ok) ok = Rec.WritePath(GetEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleComplexStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_STELLATED, TAG_RECTANGLE_COMPLEX_STELLATED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteDOUBLE(GetStellationRadius(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetStellationOffset(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleComplexStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED, TAG_RECTANGLE_COMPLEX_STELLATED_REFORMED_SIZE);

	ok = Rec.Init();
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

BOOL CXaraFileRectangle::WriteRectangleComplexRounded(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_ROUNDED, TAG_RECTANGLE_COMPLEX_ROUNDED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(GetCentrePoint(pShape));
	if (ok) ok = Rec.WriteCoordTrans(GetMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteDOUBLE(GetCurvature(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleComplexRoundedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED, TAG_RECTANGLE_COMPLEX_ROUNDED_REFORMED_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteCoordTrans(GetUTMajorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteCoordTrans(GetUTMinorAxis(pShape),0,0);
	if (ok) ok = Rec.WriteMatrix(GetTransformMatrix(pShape));
	if (ok) ok = Rec.WriteDOUBLE(GetCurvature(pShape));
	if (ok) ok = Rec.WritePath(GetEdgePath(pShape));
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

BOOL CXaraFileRectangle::WriteRectangleComplexRoundedStellated(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED, TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_SIZE);

	ok = Rec.Init();
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

BOOL CXaraFileRectangle::WriteRectangleComplexRoundedStellatedReformed(BaseCamelotFilter * pFilter, NodeRegularShape * pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter,TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED, TAG_RECTANGLE_COMPLEX_ROUNDED_STELLATED_REFORMED_SIZE);

	ok = Rec.Init();
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

