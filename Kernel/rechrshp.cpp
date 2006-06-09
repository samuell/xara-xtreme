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

>	rechrshp.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Purpose:	Implementation of an base class record handler for regular shapes.

********************************************************************************************/

#include "camtypes.h"

#include "nodershp.h"

//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "rechrshp.h"

// to match the declare in the header file
CC_IMPLEMENT_DYNAMIC(RegularShapeRecordHandler, CamelotRecordHandler);

/********************************************************************************************

>	UINT32 *RegularShapeRecordHandler::GetTagList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/96
	Inputs:		-
	Returns:	UINT32* to an array of records handled by this class.
	Purpose:	Returns an array of records handled by this class.
	See Also:	<???>

********************************************************************************************/

UINT32 *RegularShapeRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_REGULAR_SHAPE_PHASE_1,
							  TAG_REGULAR_SHAPE_PHASE_2,
							  CXFRH_TAG_LIST_END};
	return &TagList[0];
}

/********************************************************************************************

>	BOOL RegularShapeRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/96
	Inputs:		pCXaraFileRecord - pointer to a CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Reads a regular shape record from a CXaraFile.

********************************************************************************************/

BOOL RegularShapeRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok;

	// see kernel/cxftags.h for a description of the differences between these two tags - ach
	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_REGULAR_SHAPE_PHASE_1	: ok = ReadShapeGeneralPhase1(pCXaraFileRecord);
												  break;
		case TAG_REGULAR_SHAPE_PHASE_2	: ok = ReadShapeGeneralPhase2(pCXaraFileRecord);
												  break;
		default									: ok = ReadShapeInvalid(pCXaraFileRecord);
												  break;
	}

	return ok;
}

/********************************************************************************************

>	BOOL RegularShapeRecordHandler::ReadShapeGeneralPhase1(pCXaraFileRecord)
	BOOL RegularShapeRecordHandler::ReadShapeGeneralPhase2(pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/96
	Inputs:		pCXaraFileRecord - pointer to a CXaraFileRecord to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	<???>

********************************************************************************************/

BOOL RegularShapeRecordHandler::ReadShapeGeneralPhase1(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pShape;
	BYTE Flags;
	UINT16 NumberOfSides;
	DocCoord UTCentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double StellationRadius;
	double StellationOffset;
	double PrimaryCurvature;
	double SecondaryCurvature;
	
	pShape = new NodeRegularShape;

	if (ok) ok = SetupShape(pShape);

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);
	if (ok) ok = SetFlags(pShape, Flags);
	
	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pShape, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&UTCentrePoint,0,0);
	if (ok) ok = SetUTCentrePoint(pShape, UTCentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pShape, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pShape, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = SetTransformMatrix(pShape, TransformMatrix);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = SetStellationRadius(pShape, StellationRadius);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = SetStellationOffset(pShape, StellationOffset);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&PrimaryCurvature);
	if (ok) ok = SetPrimaryCurvature(pShape, PrimaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&SecondaryCurvature);
	if (ok) ok = SetSecondaryCurvature(pShape, SecondaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadPath(&(pShape->EdgePath1));
	if (ok) ok = pCXaraFileRecord->ReadPath(&(pShape->EdgePath2));
	
	if (ok) ok = InvalidateCache(pShape);
	if (ok) ok = InsertNode(pShape);

	return ok;
}

BOOL RegularShapeRecordHandler::ReadShapeGeneralPhase2(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pShape;
	BYTE Flags;
	UINT16 NumberOfSides;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double StellationRadius;
	double StellationOffset;
	double PrimaryCurvature;
	double SecondaryCurvature;
	
	pShape = new NodeRegularShape;

	if (ok) ok = SetupShape(pShape);

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Flags);
	if (ok) ok = SetFlags(pShape, Flags);
	
	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pShape, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pShape, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pShape, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = SetTransformMatrix(pShape, TransformMatrix);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = SetStellationRadius(pShape, StellationRadius);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = SetStellationOffset(pShape, StellationOffset);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&PrimaryCurvature);
	if (ok) ok = SetPrimaryCurvature(pShape, PrimaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&SecondaryCurvature);
	if (ok) ok = SetSecondaryCurvature(pShape, SecondaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadPath(&(pShape->EdgePath1));
	if (ok) ok = pCXaraFileRecord->ReadPath(&(pShape->EdgePath2));
	
	if (ok) ok = InvalidateCache(pShape);
	if (ok) ok = InsertNode(pShape);

	return ok;
}

/********************************************************************************************

>	BOOL RegularShapeRecordHandler::ReadShapeInvalid(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/06/96
	Inputs:		pCXaraFileRecord - points to a CXaraFileRecord to read
	Returns:	FALSE
	Purpose:	Called when this class is asked to read a record it doesn't know about. We
				raise an ERROR3 and return FALSE.
				
********************************************************************************************/

BOOL RegularShapeRecordHandler::ReadShapeInvalid(CXaraFileRecord *pCXaraFileRecord)
{
	ERROR3("Attempt to read an invalid shape.");
	return FALSE;
}

/********************************************************************************************

>	BOOL RegularShapeRecordHandler::SetupShape(NodeRegularShape *pShape);
	BOOL RegularShapeRecordhandler::InvalidateCache(NodeRegularShape *pShape);
	
	BOOL RegularShapeRecordHandler::SetCentrePoint(NodeRegularShape *pShape, const DocCoord & CentrePoint);
	BOOL RegularShapeRecordHandler::SetMajorAxis(NodeRegularShape *pShape, const DocCoord & MajorAxis);
	BOOL RegularShapeRecordHandler::SetMinorAxis(NodeRegularShape *pShape, const DocCoord & MinorAxis);
	BOOL RegularShapeRecordHandler::SetHeightAndWidth(NodeRegularShape *pShape, const INT32 & Height, const INT32 & Width);
	BOOL RegularShapeRecordHandler::SetNumberOfSides(NodeRegularShape *pShape, const INT32 & NumSides);
	BOOL RegularShapeRecordHandler::SetCurvature(NodeRegularShape *pShape, const double & Curvature);
	BOOL RegularShapeRecordHandler::SetPrimaryCurvature(NodeRegularShape *pShape, const double & PrimaryCurvature);
	BOOL RegularShapeRecordHandler::SetSecondaryCurvature(NodeRegularShape *pShape, const double & SecondaryCurvature);
	BOOL RegularShapeRecordHandler::SetStellationRadius(NodeRegularShape *pShape, const double & StellationRadius);
	BOOL RegularShapeRecordHandler::SetStellationOffset(NodeRegularShape *pShape, const double & StellationOffset);
	BOOL RegularShapeRecordHandler::SetEdgePath(NodeRegularShape *pShape, const Path & NewPath);
	BOOL RegularShapeRecordHandler::SetPrimaryEdgePath(NodeRegularShape *pShape, const Path & NewPrimaryPath);
	BOOL RegularShapeRecordHandler::SetSecondaryEdgePath(NodeRegularShape *pShape, const Path & NewSecondaryPath);
	BOOL RegularShapeRecordHandler::SetTransformMatrix(NodeRegularShape *pShape, const Matrix & NewMatrix);
	BOOL RegularShapeRecordHandler::SetCircular(NodeRegularShape *pShape, const BOOL & IsCircular);
	BOOL RegularShapeRecordHandler::SetStellated(NodeRegularShape *pShape, const BOOL & IsStellated);
	BOOL RegularShapeRecordHandler::SetPrimaryCurvature(NodeRegularShape *pShape, const BOOL & Value); 
	BOOL RegularShapeRecordHandler::SetStellationCurvature(NodeRegularShape *pShape, const BOOL & Value);
	
	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		xxx		- information to set
				pShape	- regular shape to alter
	Returns:	-
	Purpose:	Functions to set various pieces of information in a regular shape.

********************************************************************************************/

BOOL RegularShapeRecordHandler::SetupShape(NodeRegularShape *pShape)
{
	DocCoord Origin = DocCoord(0,0);

	pShape->SetCentrePoint(Origin);
	pShape->SetMinorAxes(Origin);
	pShape->SetMajorAxes(Origin);

	Matrix m = Matrix();	// Identity transform
	pShape->SetTransformMatrix(&m);

	return pShape->SetUpShape();
}

BOOL RegularShapeRecordHandler::InvalidateCache(NodeRegularShape *pShape)
{
	pShape->InvalidateCache();
	return TRUE;
}

// sets the centre point of the shape (after transformation, rather than the UT centre
// point, which should always be DocCoord(0,0). since this should be set before any
// of the following functions get called, we just set the transform to a translation to
// this point
BOOL RegularShapeRecordHandler::SetCentrePoint(NodeRegularShape *pShape, const DocCoord & CentrePoint)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	Matrix m = Matrix(CentrePoint);
	pShape->SetTransformMatrix(&m);

	return TRUE;
}

BOOL RegularShapeRecordHandler::SetUTCentrePoint(NodeRegularShape *pShape, const DocCoord & CentrePoint)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");
	
	pShape->SetCentrePoint(CentrePoint);
	return TRUE;
}

// sets the untransformed major axis of the shape.
BOOL RegularShapeRecordHandler::SetMajorAxis(NodeRegularShape *pShape, const DocCoord & MajorAxis)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetMajorAxes(MajorAxis);
	return TRUE;
}

// sets the untransformed minor axis of the shape.
BOOL RegularShapeRecordHandler::SetMinorAxis(NodeRegularShape *pShape, const DocCoord & MinorAxis)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetMinorAxes(MinorAxis);
	return TRUE;
}

// sets the height and width of the shape. if this function gets called, we assume that the shape has
// no rotation or skew, so the MajorAxis = (0, Height), MinorAxis = (Width, 0)
BOOL RegularShapeRecordHandler::SetHeightAndWidth(NodeRegularShape *pShape, const INT32 & Height, const INT32 & Width)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	SetMajorAxis(pShape, DocCoord(0, Height/2));
	SetMinorAxis(pShape, DocCoord(Width/2, 0));
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetNumberOfSides(NodeRegularShape *pShape, const INT32 & NumberOfSides)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetNumSides(NumberOfSides);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetCurvature(NodeRegularShape *pShape, const double & Curvature)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetPrimaryCurveToPrimary(Curvature);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetPrimaryCurvature(NodeRegularShape *pShape, const double & PrimaryCurvature)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetPrimaryCurveToPrimary(PrimaryCurvature);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetSecondaryCurvature(NodeRegularShape *pShape, const double & SecondaryCurvature)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetStellCurveToStell(SecondaryCurvature);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetStellationRadius(NodeRegularShape *pShape, const double & StellationRadius)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetStellRadiusToPrimary(StellationRadius);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetStellationOffset(NodeRegularShape *pShape, const double & StellationOffset)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetStellationRatio(StellationOffset);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetTransformMatrix(NodeRegularShape *pShape, const Matrix & NewMatrix)
{
	pShape->SetTransformMatrix(&NewMatrix);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetIsCircular(NodeRegularShape *pShape, const BOOL & IsCircular)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetCircular(IsCircular);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetIsStellated(NodeRegularShape *pShape, const BOOL & IsStellated)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetStellated(IsStellated);
	return TRUE;
}

BOOL RegularShapeRecordHandler::SetIsRounded(NodeRegularShape * pShape, const BOOL & IsRounded)
{
	ERROR2IF(pShape==NULL, FALSE, "parameter pShape == NULL");

	pShape->SetPrimaryCurvature(IsRounded);
	pShape->SetStellationCurvature(IsRounded);
	return TRUE;
}

/********************************************************************************************

>	BOOL RegularShapeRecordHandler:SetFlags(NodeRegularShape *pShape, const BYTE & Value);

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/06/96
	Inputs:		pShape	- node regular shape to write to
				BYTE x	- flags information to write
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Sets various flags in node regular shapes.

********************************************************************************************/

BOOL RegularShapeRecordHandler::SetFlags(NodeRegularShape *pShape, const BYTE & Value)
{
	// !!HACK!! constants embedded in code
	// !!HACK!! see CXaraFileRegularShape::GetFlags for the other code which needs to
	// !!HACK!! understand how the flags are packed into the byte.
	if ((Value & 0x1) == 0x1) pShape->SetCircular(TRUE);
	if ((Value & 0x2) == 0x2) pShape->SetStellated(TRUE);
	if ((Value & 0x4) == 0x4) pShape->SetPrimaryCurvature(TRUE);
	if ((Value & 0x8) == 0x8) pShape->SetStellationCurvature(TRUE);

	return TRUE;
}

/********************************************************************************************

>	void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);


	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/07/96
	Inputs:		pCXaraFileRecord - CXaraFileRecord holding the record in question
				Str - StringBase to add our information to
	Returns:	-
	Purpose:	Returns textual information on this record

********************************************************************************************/

#if XAR_TREE_DIALOG
void RegularShapeRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase *pStr)
{
	if (pRecord != NULL && pStr != NULL)
	{
		// Call base class first
		// This outputs the tag and size
		CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

		switch (pRecord->GetTag())
		{
			case TAG_REGULAR_SHAPE_PHASE_1 : DescribeShapeGeneralPhase1(pRecord, pStr); break;
			case TAG_REGULAR_SHAPE_PHASE_2 : DescribeShapeGeneralPhase2(pRecord, pStr); break;
			default : DescribeInvalid(pRecord, pStr); break;
		}
	}
}

void RegularShapeRecordHandler::DescribeShapeGeneralPhase1(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	char s[512];

	BOOL ok = TRUE;

	BYTE Flags;
	UINT16 NumberOfSides;
	DocCoord UTCentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double StellationRadius;
	double StellationOffset;
	double PrimaryCurvature;
	double SecondaryCurvature;
	
	if (ok) ok = pRecord->ReadBYTE(&Flags);
	if (ok) ok = pRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = pRecord->ReadCoordTrans(&UTCentrePoint,0,0);
	if (ok) ok = pRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = pRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = pRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = pRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = pRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = pRecord->ReadDOUBLE(&PrimaryCurvature);
	if (ok) ok = pRecord->ReadDOUBLE(&SecondaryCurvature);
	//if (ok) ok = pRecord->ReadPath(&(pShape->EdgePath1));
	//if (ok) ok = pRecord->ReadPath(&(pShape->EdgePath2));

	(*pStr) += "General regular shape (Phase 1)\r\n\r\n";
	_stprintf(s,"Flags\t\t= %x\r\n",Flags);
	(*pStr) += s;
	_stprintf(s,"Number of sides\t= %d\r\n",NumberOfSides);
	(*pStr) += s;
	_stprintf(s,"Centre point\t= %d, %d\r\n",UTCentrePoint.x,UTCentrePoint.y);
	(*pStr) += s;
	_stprintf(s,"Major axis\t\t= %d, %d\r\n",MajorAxis.x,MajorAxis.y);
	(*pStr) += s;
	_stprintf(s,"Minor axis\t\t= %d, %d\r\n",MinorAxis.x,MinorAxis.y);
	(*pStr) += s;
	_stprintf(s,"Stellation radius\t= %f\r\n",StellationRadius);
	(*pStr) += s;
	_stprintf(s,"Primary curvature\t= %f\r\n",PrimaryCurvature);
	(*pStr) += s;
	_stprintf(s,"Secondary curvature\t= %f\r\n",SecondaryCurvature);
	(*pStr) += s;
}

void RegularShapeRecordHandler::DescribeShapeGeneralPhase2(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	char s[512];

	BOOL ok = TRUE;

	BYTE Flags;
	UINT16 NumberOfSides;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double StellationRadius;
	double StellationOffset;
	double PrimaryCurvature;
	double SecondaryCurvature;
	
	if (ok) ok = pRecord->ReadBYTE(&Flags);
	if (ok) ok = pRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = pRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = pRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = pRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = pRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = pRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = pRecord->ReadDOUBLE(&PrimaryCurvature);
	if (ok) ok = pRecord->ReadDOUBLE(&SecondaryCurvature);
	//if (ok) ok = pRecord->ReadPath(&(pShape->EdgePath1));
	//if (ok) ok = pRecord->ReadPath(&(pShape->EdgePath2));
	
	(*pStr) += "General regular shape (Phase 2)\r\n\r\n";
	_stprintf(s,"Flags\t\t= %x\r\n",Flags);
	(*pStr) += s;
	_stprintf(s,"Number of sides\t= %d\r\n",NumberOfSides);
	(*pStr) += s;
	_stprintf(s,"Major axis\t\t= %d, %d\r\n",MajorAxis.x,MajorAxis.y);
	(*pStr) += s;
	_stprintf(s,"Minor axis\t\t= %d, %d\r\n",MinorAxis.x,MinorAxis.y);
	(*pStr) += s;
	_stprintf(s,"Stellation radius\t= %f\r\n",StellationRadius);
	(*pStr) += s;
	_stprintf(s,"Stellation offset\t= %f\r\n",StellationOffset);
	(*pStr) += s;
	_stprintf(s,"Primary curvature\t= %f\r\n",PrimaryCurvature);
	(*pStr) += s;
	_stprintf(s,"Secondary curvature\t= %f\r\n",SecondaryCurvature);
	(*pStr) += s;
}

void RegularShapeRecordHandler::DescribeInvalid(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	(*pStr) += "Invalid regular shape\r\n";
}
#endif

