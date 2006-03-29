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

>	cxfrgshp.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Purpose:	Base class holding code for exporting regular shapes to the new file format
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
#include "cxfellp.h"
#include "cxfrect.h"
#include "cxfpoly.h"

/********************************************************************************************

>	BOOL CXaraFileRegularShape::WritePreChildrenNative(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::WritePreChildrenWeb(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pFilter - filter to save this shape to
				pShape  - regular shape to save
	Returns:	boolean value indicating success of writing record
	Purpose:	Writes a regular shape to the filter in native or web format

********************************************************************************************/

BOOL CXaraFileRegularShape::WritePreChildrenNative(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
{
	ERROR2IF(pFilter == NULL, FALSE, "Argument pFilter == NULL.");
	ERROR2IF(pShape == NULL, FALSE, "Argument pShape == NULL.");

	// Due to last minute changes, of course after the spec was approved, we now use this for native
	// and the pants web files. There is a new flag to say if we are doing a compact native AKA the web file 
	// half way between the true web file and the full native file.
	if (pFilter->IsCompactNativeFilter() || pFilter->WriteSpecificRegularShapes())
		return WriteShapeWeb(pFilter, pShape);

	return WriteShapeNative(pFilter, pShape);
}

BOOL CXaraFileRegularShape::WritePreChildrenWeb(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
{
	ERROR2IF(pFilter == NULL, FALSE, "Argument pFilter == NULL.");
	ERROR2IF(pShape == NULL, FALSE, "Argument pShape == NULL.");

	return WriteShapeWeb(pFilter, pShape);
}

/********************************************************************************************

>	BOOL CXaraFileRegularShape::WriteShapeNative(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::WriteShapeWeb(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pFilter - filter to use
				pShape  - regular shape to save out
	Returns:	boolean value indicating success of writing record
	Purpose:	Writes a regular shape record to the filter

********************************************************************************************/

BOOL CXaraFileRegularShape::WriteShapeNative(BaseCamelotFilter *pFilter, NodeRegularShape *pShape)
{
	ERROR2IF(pFilter == NULL, FALSE, "Argument pFilter == NULL.");
	ERROR2IF(pShape == NULL, FALSE, "Argument pShape == NULL.");

	NodeRegularShape MyRegularShape;
	BOOL ok = TRUE;

	if (GetUTCentrePoint(pShape) != DocCoord(0,0))
	{
		// sigh, we've got a very old regular shape, where the UT Centre Point isn't the
		// origin (it is for all v1.0 and later versions of Camelot). Instead we make a copy
		// of the shape, fix the coordinates and then save it.
		pShape->CopyNodeContents(&MyRegularShape);
		if (ok) ok = FixOldShape(&MyRegularShape);
		pShape = &MyRegularShape;
	}

	if (ok) ok = CXaraFileRegularShape::WriteShapeGeneral(pFilter, pShape);

	return ok;
}

BOOL CXaraFileRegularShape::WriteShapeWeb(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
{
	ERROR2IF(pFilter == NULL, FALSE, "Argument pFilter == NULL.");
	ERROR2IF(pShape == NULL, FALSE, "Argument pShape == NULL.");

	NodeRegularShape MyRegularShape;
	BOOL ok = TRUE;

	if (GetUTCentrePoint(pShape) != DocCoord(0,0))
	{
		// sigh, we've got a very old regular shape, where the UT Centre Point isn't the
		// origin (it is for all v1.0 and later versions of Camelot). Instead we make a copy
		// of the shape, fix the coordinates and then save it.
		pShape->CopyNodeContents(&MyRegularShape);
		if (ok) ok = FixOldShape(&MyRegularShape);
		pShape = &MyRegularShape;
	}
	
	if (IsEllipse(pShape))
	{
		ok = CXaraFileEllipse::WriteEllipse(pFilter, pShape);
	}
	else if (IsRectangle(pShape))
	{
		ok = CXaraFileRectangle::WriteRectangle(pFilter, pShape);
	}
	else // it's a polygon
	{
		ok = CXaraFilePolygon::WritePolygon(pFilter, pShape);
	}
	return ok;
}

/********************************************************************************************

>	BOOL CXaraFileRegularShape::FixOldShape(NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/07/96
	Inputs:		pShape - NodeRegularShape to fix
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Fixes old regular shapes, i.e. those which have UTCentrePoint != DocCoord(0,0)
				by adjusting the UTCentrePoint, UTMajorAxis, UTMinorAxis, TransformMatrix.

********************************************************************************************/

BOOL CXaraFileRegularShape::FixOldShape(NodeRegularShape *pShape)
{
	// since the UTCentrePoint is != DocCoord(0,0), we need to fix the major axis, minor
	// axis and the transform before we can save them. The idea is to get the
	// UTCentrePoint = DocCoord(0,0), and alter the other variables so that the transformed
	// remains the same.

	DocCoord MyUTCentrePointOld;
	DocCoord MyUTCentrePointNew;

	Matrix MyTransformOld;
	Matrix MyTransformNew;
	Matrix MyTransformNewInverse;

	DocCoord MyUTMajorAxisOld;
	DocCoord MyUTMajorAxisNew;
	DocCoord MyUTMinorAxisOld;
	DocCoord MyUTMinorAxisNew;

	{
		// retrieve the all values
		MyUTCentrePointOld = pShape->GetUTCentrePoint();
		pShape->GetTransformMatrix(&MyTransformOld);
		MyUTMajorAxisOld = pShape->GetUTMajorAxes();
		MyUTMinorAxisOld = pShape->GetUTMinorAxes();
	}

	{
		// set the new UTCentrePoint. Simple really, and the compiler should constant-fold
		// this variable away.
		MyUTCentrePointNew = DocCoord(0,0);
	}

	{
		// calculate the new transform matrix. We know that
		//
		// UTCentrePointOld * TransformOld = UTCentrePointNew * TransformNew
		//
		// and that UTCentrePointNew = DocCoord(0,0). Hence (you can probably guess what I
		// mean by this) TransformNew.matrix = TransformOld.matrix and
		// TransformNew.translation = TransformedCentrePoint.

		DocCoord MyCentrePoint;

		MyCentrePoint = pShape->GetUTCentrePoint();
		MyTransformOld.transform(&MyCentrePoint);
		MyTransformNew = MyTransformOld;
		MyTransformNew.SetTranslation(MyCentrePoint);

		MyTransformNewInverse = MyTransformNew.Inverse();
	}

	{
		// calculate the new major and minor axes. Since:
		//
		// UTMajorAxisOld * TransformOld = UTMajorAxisNew * TransformNew
		//
		// then
		//
		// UTMajorAxisNew = (UTMajorAxisOld * TransformOld) * TransformNew'

		MyUTMajorAxisNew = MyUTMajorAxisOld;
		MyTransformOld.transform(&MyUTMajorAxisNew);
		MyTransformNewInverse.transform(&MyUTMajorAxisNew);

		MyUTMinorAxisNew = MyUTMinorAxisOld;
		MyTransformOld.transform(&MyUTMinorAxisNew);
		MyTransformNewInverse.transform(&MyUTMinorAxisNew);
	}

	{
		// now set the variables to their new values.

		pShape->SetCentrePoint(MyUTCentrePointNew);
		pShape->SetTransformMatrix(&MyTransformNew);
		pShape->SetMajorAxes(MyUTMajorAxisNew);
		pShape->SetMinorAxes(MyUTMinorAxisNew);
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL CXaraFileRegularShape::WriteShapeGeneral(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pFilter - filter to use
				pShape  - regular shape to save out
	Returns:	boolean value indicating sucess of writing record to filter
	Purpose:	Writes a generic regular shape record to the filter

********************************************************************************************/

BOOL CXaraFileRegularShape::WriteShapeGeneral(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
{
	BOOL ok;

	CamelotFileRecord Rec(pFilter, TAG_REGULAR_SHAPE_PHASE_2, TAG_REGULAR_SHAPE_PHASE_2_SIZE);

	ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(GetFlags(pShape));
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

/********************************************************************************************

>	BOOL CXaraFileRegularShape::WriteShapeInvalid(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pFilter - filter to use
				pShape  - regular shape to save out
	Returns:	FALSE
	Purpose:	Called when earlier functions are asked to write an invalid shape to the file.
				We raise an ERROR3, then return FALSE.

********************************************************************************************/

BOOL CXaraFileRegularShape::WriteShapeInvalid(BaseCamelotFilter * pFilter, NodeRegularShape *pShape)
{
	ERROR3("Invalid shape being written to file.");

	return FALSE;
}

/********************************************************************************************

>	BOOL CXaraFileRegularShape::IsEllipse(NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::IsRectangle(NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::IsSimple(NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::IsComplex(NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::IsRounded(NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::IsStellated(NodeRegularShape *pShape)
	BOOL CXaraFileRegularShape::IsReformed(NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pShape - regular shape to test for a particular attribute
	Returns:	boolean value, true if the regular shape has the particular attribute, false otherwise
	Purpose:	Tests the regular shape for particular attributes.
	See Also:	<???>

********************************************************************************************/

BOOL CXaraFileRegularShape::IsEllipse(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	return pShape->IsCircular();
}

BOOL CXaraFileRegularShape::IsRectangle(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	// by rectangular, we mean it's not circular, and has four sides. A 'four-sided
	// polygon' would be a better name, but we stick with rectangle for legacy reasons.
	// i.e. I can't be bothered to change everything now.

	return (!IsEllipse(pShape)) && (GetNumberOfSides(pShape) == 4);
}

BOOL CXaraFileRegularShape::IsSimple(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	// check to see if the transformation is just a translation, and that the major and
	// minor axes are perpendicular and parallel to the y and x axes respectively
	Matrix MyMatrix;
	BOOL FlagIsTranslation;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	
	pShape->GetTransformMatrix(&MyMatrix);
	FlagIsTranslation = MyMatrix.IsTranslation(mEpsilon);	
	MajorAxis = pShape->GetUTMajorAxes();
	MinorAxis = pShape->GetUTMinorAxes();
	
	return ((FlagIsTranslation && MajorAxis.x == 0 && MinorAxis.y == 0) ||
			(FlagIsTranslation && MajorAxis.y == 0 && MinorAxis.x == 0));

//	return MyMatrix.IsTranslation(mEpsilon) && MajorAxis.x == 0 && MinorAxis.y == 0;
}

BOOL CXaraFileRegularShape::IsComplex(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	return !(IsSimple(pShape));
}

BOOL CXaraFileRegularShape::IsRounded(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	return pShape->IsPrimaryCurvature();
}

BOOL CXaraFileRegularShape::IsStellated(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	return pShape->IsStellated();
}

BOOL CXaraFileRegularShape::IsReformed(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	return pShape->IsReformed();
}

/********************************************************************************************

>	BYTE CXaraFileRegularShape::GetFlags(NodeRegularShape *pShape);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96
	Inputs:		pShape - Node regular shape to retrieve flag information from
	Returns:	BYTE of information
	Purpose:	Retrieves flag information from node regular shapes

********************************************************************************************/

BYTE CXaraFileRegularShape::GetFlags(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, 0, "parameter pShape == NULL");

	BYTE result = 0;

	// !!HACK!! constants in code.
	// !!HACK!! see RegularShapeRecordHandler::SetFlags for the other bit of code which
	// !!HACK!! needs these constants.
	if (pShape->IsCircular()) result |= 0x1;
	if (pShape->IsStellated()) result |= 0x2;
	if (pShape->IsPrimaryCurvature()) result |= 0x4;
	if (pShape->IsStellationCurvature()) result |= 0x8;

	return result;
}

/********************************************************************************************

>	DocCoord CXaraFileRegularShape::GetCentrePoint(NodeRegularShape *pShape)
	DocCoord CXaraFileRegularShape::GetMajorAxis(NodeRegularShape *pShape)
	DocCoord CXaraFileRegularShape::GetMinorAxis(NodeRegularShape *pShape)
	DocCoord CXaraFileRegularShape::GetUTCentrePoint(NodeRegularShape *pShape)
	DocCoord CXaraFileRegularShape::GetUTMajorAxis(NodeRegularShape *pShape)
	DocCoord CXaraFileRegularShape::GetUTMinorAxis(NodeRegularShape *pShape)
	INT32 CXaraFileRegularShape::GetHeight(NodeRegularShape *pShape)
	INT32 CXaraFileRegularShape::GetWidth(NodeRegularShape *pShape)
	UINT16 CXaraFileRegularShape::GetNumberOfSides(NodeRegularShape *pShape)
	double CXaraFileRegularShape::GetCurvature(NodeRegularShape *pShape)
	double CXaraFileRegularShape::GetPrimaryCurvature(NodeRegularShape *pShape)
	double CXaraFileRegularShape::GetSecondaryCurvature(NodeRegularShape *pShape)
	double CXaraFileRegularShape::GetStellationRadius(NodeRegularShape *pShape)
	double CXaraFileRegularShape::GetStellationOffset(NodeRegularShape *pShape)
	Matrix * CXaraFileRegularShape::GetTransformMatrix(NodeRegularShape *pShape);
	PATH * CXaraFileRegularShape::GetEdgePath(NodeRegularShape *pShape)
	PATH * CXaraFileRegularShape::GetPrimaryEdgePath(NodeRegularShape *pShape)
	PATH * CXaraFileRegularShape::GetSecondaryEdgePath(NodeRegularShape *pShape)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/06/96
	Inputs:		pShape - regular shape to retrive information from
	Returns:	information from the regular shape
	Purpose:	Functions to retrieve information from regular shapes

********************************************************************************************/

DocCoord CXaraFileRegularShape::GetCentrePoint(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, DocCoord(0,0), "parameter pShape == NULL");

	return (pShape->GetCentrePoint());
}

// returns the transformed major axis relative to the origin
DocCoord CXaraFileRegularShape::GetMajorAxis(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, DocCoord(0,0), "parameter pShape == NULL");

	return (pShape->GetMajorAxes() - pShape->GetCentrePoint());
}

// returns the transformed minor axis relative to the origin
DocCoord CXaraFileRegularShape::GetMinorAxis(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, DocCoord(0,0), "parameter pShape == NULL");

	return (pShape->GetMinorAxes() - pShape->GetCentrePoint());
}

DocCoord CXaraFileRegularShape::GetUTCentrePoint(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, DocCoord(0,0), "parameter pShape == NULL");

	return (pShape->GetUTCentrePoint());
}

// returns the untransformed major axis
DocCoord CXaraFileRegularShape::GetUTMajorAxis(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, DocCoord(0,0), "parameter pShape == NULL");

	return pShape->GetUTMajorAxes();
}

// returns the untransformed minor axis
DocCoord CXaraFileRegularShape::GetUTMinorAxis(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, DocCoord(0,0), "parameter pShape == NULL");

	return pShape->GetUTMinorAxes();
}

// uses the GetMajorAxis and GetMinorAxis functions in this class to get the
// transformed major and minor axes for this regular shape. calculating the width
// and height of the shape is then trivial.
INT32 CXaraFileRegularShape::GetHeight(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, 0, "parameter pShape == NULL");

	return 2*abs(GetMajorAxis(pShape).y + GetMinorAxis(pShape).y);
}

INT32 CXaraFileRegularShape::GetWidth(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, 0, "parameter pShape == NULL");

 	return 2*abs(GetMajorAxis(pShape).x + GetMinorAxis(pShape).x);
}

UINT16 CXaraFileRegularShape::GetNumberOfSides(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, 0, "parameter pShape == NULL");

	return pShape->GetNumSides();
}

double CXaraFileRegularShape::GetCurvature(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, .0, "parameter pShape == NULL");

	return pShape->GetPrimaryCurveToPrimary();
}

double CXaraFileRegularShape::GetPrimaryCurvature(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, .0, "parameter pShape == NULL");

	return pShape->GetPrimaryCurveToPrimary();
}

double CXaraFileRegularShape::GetSecondaryCurvature(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, .0, "parameter pShape == NULL");

	return pShape->GetStellCurveToStell();
}

double CXaraFileRegularShape::GetStellationRadius(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, .0, "parameter pShape == NULL");

	return pShape->GetStellRadiusToPrimary();
}

double CXaraFileRegularShape::GetStellationOffset(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, .0, "parameter pShape == NULL");

	return pShape->GetStellationRatio();
}

Matrix CXaraFileRegularShape::GetTransformMatrix(NodeRegularShape *pShape)
{
//	ERROR2IF(pShape==NULL, Matrix(), "parameter pShape == NULL");
	Matrix m;

	pShape->GetTransformMatrix(&m);

	return m;
}

Path * CXaraFileRegularShape::GetEdgePath(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, NULL, "parameter pShape == NULL");

	return &(pShape->EdgePath1);
}

Path * CXaraFileRegularShape::GetPrimaryEdgePath(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, NULL, "parameter pShape == NULL");

	return &(pShape->EdgePath1);
}

Path * CXaraFileRegularShape::GetSecondaryEdgePath(NodeRegularShape *pShape)
{
	ERROR2IF(pShape==NULL, NULL, "parameter pShape == NULL");

	return &(pShape->EdgePath2);
}

/********************************************************************************************

>	const double CXaraFileRegularShape::mEpsilon

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>	
	Created:	04/06/96
	Purpose:	Epsilon value used for checking matrix type
	See Also:	CXaraFileRegularShape::IsIdentity

********************************************************************************************/

const double CXaraFileRegularShape::mEpsilon = 0.000016;

