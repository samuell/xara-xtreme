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

>	rechpoly.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/06/96
	Purpose:	Implementation of class to handle loading of polygon records from the new
				file format.

********************************************************************************************/

#include "camtypes.h"

#include "nodershp.h"

//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cxfrech.h" - in camtypes.h [AUTOMATICALLY REMOVED]

#include "rechrshp.h"
#include "rechpoly.h"

// to match the declare in the header file
CC_IMPLEMENT_DYNAMIC(PolygonRecordHandler, RegularShapeRecordHandler);

/********************************************************************************************

>	UINT32* PolygonRecordHandler::GetTagList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		-
	Returns:	UINT32* to an array of records handled by this class. The last entry in the
				array is CXFRH_TAG_LIST_END
	Purpose:	Returns an array of records handled by this class.
	See Also:	EllispeRecordHandler::HandleRecord

********************************************************************************************/

UINT32* PolygonRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_POLYGON_COMPLEX,
								TAG_POLYGON_COMPLEX_REFORMED,
								TAG_POLYGON_COMPLEX_STELLATED,
								TAG_POLYGON_COMPLEX_STELLATED_REFORMED,
								TAG_POLYGON_COMPLEX_ROUNDED,
								TAG_POLYGON_COMPLEX_ROUNDED_REFORMED,
								TAG_POLYGON_COMPLEX_ROUNDED_STELLATED,
								TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED,
								CXFRH_TAG_LIST_END};
	return &TagList[0];
}

/********************************************************************************************

>	BOOL PolygonRecordHandler::HandleRecord(CXaraFileRecord * pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		pCXaraFileRecord - CXaraFileRecord object to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Handles ellipse type records for the new file format

********************************************************************************************/

BOOL PolygonRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_POLYGON_COMPLEX
				: ok = ReadPolygonComplex(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_REFORMED
				: ok = ReadPolygonComplexReformed(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_STELLATED
				: ok = ReadPolygonComplexStellated(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_STELLATED_REFORMED
				: ok = ReadPolygonComplexStellatedReformed(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_ROUNDED
				: ok = ReadPolygonComplexRounded(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_ROUNDED_REFORMED
				: ok = ReadPolygonComplexRoundedReformed(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED
				: ok = ReadPolygonComplexRoundedStellated(pCXaraFileRecord); break;
		case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED
				: ok = ReadPolygonComplexRoundedStellatedReformed(pCXaraFileRecord); break;
		default
				: ok = ReadShapeInvalid(pCXaraFileRecord); break;
	}
	
	return ok;
}

/********************************************************************************************

	BOOL PolygonRecordHandler::ReadPolygonComplex(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexReformed(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexStellated(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexStellatedReformed(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexRounded(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexRoundedReformed(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexRoundedStellated(CXaraFileRecord *pCXaraFileRecord)
	BOOL PolygonRecordHandler::ReadPolygonComplexRoundedStellatedReformed(CXaraFileRecord *pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		pCXaraFileRecord - pointer to the record to read
	Returns:	TRUE if successfull, false otherwise
	Purpose:	Reads in an polygon record

********************************************************************************************/

BOOL PolygonRecordHandler::ReadPolygonComplex(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	
	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoord(&CentrePoint);
	if (ok) ok = SetCentrePoint(pPolygon, CentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexReformed(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	
	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = SetTransformMatrix(pPolygon, TransformMatrix);

	if (ok) ok = pCXaraFileRecord->ReadPath(&(pPolygon->EdgePath1));

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexStellated(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	double StellationRadius;
	double StellationOffset;

	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);
	if (ok) ok = SetIsStellated(pPolygon, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoord(&CentrePoint);
	if (ok) ok = SetCentrePoint(pPolygon, CentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = SetStellationRadius(pPolygon, StellationRadius);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = SetStellationOffset(pPolygon, StellationOffset);

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexStellatedReformed(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double StellationRadius;
	double StellationOffset;

	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);
	if (ok) ok = SetIsStellated(pPolygon, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = SetTransformMatrix(pPolygon, TransformMatrix);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = SetStellationRadius(pPolygon, StellationRadius);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = SetStellationOffset(pPolygon, StellationOffset);

	if (ok) ok = pCXaraFileRecord->ReadPath(&(pPolygon->EdgePath1));
	if (ok) ok = pCXaraFileRecord->ReadPath(&(pPolygon->EdgePath2));

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexRounded(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	double Curvature;
	
	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);
	if (ok) ok = SetIsRounded(pPolygon, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoord(&CentrePoint);
	if (ok) ok = SetCentrePoint(pPolygon, CentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&Curvature);
	if (ok) ok = SetCurvature(pPolygon, Curvature);

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexRoundedReformed(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double Curvature;
	
	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);
	if (ok) ok = SetIsRounded(pPolygon, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = SetTransformMatrix(pPolygon, TransformMatrix);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&Curvature);
	if (ok) ok = SetCurvature(pPolygon, Curvature);

	if (ok) ok = pCXaraFileRecord->ReadPath(&(pPolygon->EdgePath1));

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexRoundedStellated(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	double StellationRadius;
	double StellationOffset;
	double PrimaryCurvature;
	double SecondaryCurvature;
	
	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);
	if (ok) ok = SetIsRounded(pPolygon, TRUE);
	if (ok) ok = SetIsStellated(pPolygon, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoord(&CentrePoint);
	if (ok) ok = SetCentrePoint(pPolygon, CentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = SetStellationRadius(pPolygon, StellationRadius);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = SetStellationOffset(pPolygon, StellationOffset);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&PrimaryCurvature);
	if (ok) ok = SetPrimaryCurvature(pPolygon, PrimaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&SecondaryCurvature);
	if (ok) ok = SetSecondaryCurvature(pPolygon, SecondaryCurvature);

	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}

BOOL PolygonRecordHandler::ReadPolygonComplexRoundedStellatedReformed(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pPolygon;
	UINT16 NumberOfSides;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	Matrix TransformMatrix;
	double StellationRadius;
	double StellationOffset;
	double PrimaryCurvature;
	double SecondaryCurvature;
	
	pPolygon = new NodeRegularShape;

	if (ok) ok = SetupShape(pPolygon);
	if (ok) ok = SetIsRounded(pPolygon, TRUE);
	if (ok) ok = SetIsStellated(pPolygon, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = SetNumberOfSides(pPolygon, NumberOfSides);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pPolygon, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pPolygon, MinorAxis);

	if (ok) ok = pCXaraFileRecord->ReadMatrix(&TransformMatrix);
	if (ok) ok = SetTransformMatrix(pPolygon, TransformMatrix);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationRadius);
	if (ok) ok = SetStellationRadius(pPolygon, StellationRadius);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&StellationOffset);
	if (ok) ok = SetStellationOffset(pPolygon, StellationOffset);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&PrimaryCurvature);
	if (ok) ok = SetPrimaryCurvature(pPolygon, PrimaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&SecondaryCurvature);
	if (ok) ok = SetSecondaryCurvature(pPolygon, SecondaryCurvature);

	if (ok) ok = pCXaraFileRecord->ReadPath(&(pPolygon->EdgePath1));
	if (ok) ok = pCXaraFileRecord->ReadPath(&(pPolygon->EdgePath2));
	
	if (ok) ok = InvalidateCache(pPolygon);
	if (ok) ok = InsertNode(pPolygon);

	return ok;
}


/********************************************************************************************

>	void GetRecordDescriptionText(CXaraFileRecord* pCXaraFileRecord,StringBase* Str);


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/08/96
	Inputs:		pCXaraFileRecord - CXaraFileRecord holding the record in question
				Str - StringBase to add our information to
	Returns:	-
	Purpose:	Returns textual information on this record

********************************************************************************************/

#ifdef XAR_TREE_DIALOG
void PolygonRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase *pStr)
{
	if (pRecord != NULL && pStr != NULL)
	{
		// Call base class first
		// This outputs the tag and size
		CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

		switch (pRecord->GetTag())
		{
			case TAG_POLYGON_COMPLEX :
			case TAG_POLYGON_COMPLEX_STELLATED :
			case TAG_POLYGON_COMPLEX_ROUNDED :
			case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED :
				(*pStr) += _T("Polygon complex\r\n\r\n");
				DescribePolygonComplex(pRecord, pStr);
				break;

			case TAG_POLYGON_COMPLEX_REFORMED :
			case TAG_POLYGON_COMPLEX_STELLATED_REFORMED :
			case TAG_POLYGON_COMPLEX_ROUNDED_REFORMED :
			case TAG_POLYGON_COMPLEX_ROUNDED_STELLATED_REFORMED :
				(*pStr) += _T("Polygon complex reformed\r\n\r\n");
				DescribePolygonComplexReformed(pRecord, pStr);
				break;

			default : DescribeInvalid(pRecord, pStr); break;
		}
	}
}

void PolygonRecordHandler::DescribePolygonComplex(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	UINT16 NumberOfSides;
	TCHAR s[256];

	BOOL ok = TRUE;

	if (ok) ok = pRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = pRecord->ReadCoord(&CentrePoint);
	if (ok) ok = pRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = pRecord->ReadCoordTrans(&MinorAxis,0,0);

	camSprintf(s,_T("Number of sides\t= %d\r\n"),NumberOfSides);
	(*pStr) += s;
	camSprintf(s,_T("Centre point\t= %d, %d\r\n"),CentrePoint.x,CentrePoint.y);
	(*pStr) += s;
	camSprintf(s,_T("Major axis\t\t= %d, %d\r\n"),MajorAxis.x,MajorAxis.y);
	(*pStr) += s;
	camSprintf(s,_T("Minor axis\t\t= %d, %d\r\n"),MinorAxis.x,MinorAxis.y);
	(*pStr) += s;
}

void PolygonRecordHandler::DescribePolygonComplexReformed(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	DocCoord MajorAxis;
	DocCoord MinorAxis;
	UINT16 NumberOfSides;
	TCHAR s[256];

	BOOL ok = TRUE;

	if (ok) ok = pRecord->ReadUINT16(&NumberOfSides);
	if (ok) ok = pRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = pRecord->ReadCoordTrans(&MinorAxis,0,0);

	camSprintf(s,_T("Number of sides\t= %d\r\n"),NumberOfSides);
	(*pStr) += s;
	camSprintf(s,_T("Major axis\t\t= %d, %d\r\n"),MajorAxis.x,MajorAxis.y);
	(*pStr) += s;
	camSprintf(s,_T("Minor axis\t\t= %d, %d\r\n"),MinorAxis.x,MinorAxis.y);
	(*pStr) += s;
}

void PolygonRecordHandler::DescribeInvalid(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	(*pStr) += _T("Invalid rectangle\r\n");
}
#endif
