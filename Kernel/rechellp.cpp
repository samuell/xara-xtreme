// $Id: rechellp.cpp 662 2006-03-14 21:31:49Z alex $
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

>	rechellp.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Purpose:	Functions for the class to handle importing ellipse records from
			the new file format

********************************************************************************************/

#include "camtypes.h"

#include "nodershp.h"

#include "cxfdefs.h"
#include "cxftags.h"
#include "cxfrec.h"
#include "cxfrech.h"

#include "rechrshp.h"
#include "rechellp.h"

// to match the declare in the header file
CC_IMPLEMENT_DYNAMIC(EllipseRecordHandler, RegularShapeRecordHandler);

/********************************************************************************************

>	UINT32* EllipseRecordHandler::GetTagList()

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		-
	Returns:	UINT32* to an array of records handled by this class. The last entry in the
				array is CXFRH_TAG_LIST_END
	Purpose:	Returns an array of records handled by this class.
	See Also:	EllispeRecordHandler::HandleRecord

********************************************************************************************/

UINT32* EllipseRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {TAG_ELLIPSE_SIMPLE,
								TAG_ELLIPSE_COMPLEX,
								CXFRH_TAG_LIST_END};
	return &TagList[0];
}

/********************************************************************************************

>	BOOL EllispeRecordHandler::HandleRecord(CXaraFileRecord * pCXaraFileRecord)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		pCXaraFileRecord - CXaraFileRecord object to read
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Handles ellipse type records for the new file format

********************************************************************************************/

BOOL EllipseRecordHandler::HandleRecord(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_ELLIPSE_SIMPLE		: ok = ReadEllipseSimple(pCXaraFileRecord); break;
		case TAG_ELLIPSE_COMPLEX	: ok = ReadEllipseComplex(pCXaraFileRecord); break;
		default						: ok = ReadShapeInvalid(pCXaraFileRecord); break;
	}
	
	return ok;
}

/********************************************************************************************

>	BOOL EllipseRecordHandler::ReadEllipseSimple(pCXaraFileRecord); break;
	BOOL EllipseRecordHandler::ReadEllipseComplex(pCXaraFileRecord); break;

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/06/96
	Inputs:		pCXaraFileRecord - pointer to the record to read
	Returns:	TRUE if successfull, false otherwise
	Purpose:	Reads in an ellise record

********************************************************************************************/

BOOL EllipseRecordHandler::ReadEllipseSimple(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape * pEllipse;
	DocCoord CentrePoint;
	INT32 Height;
	INT32 Width;

	pEllipse = new NodeRegularShape;

	if (ok) ok = SetupShape(pEllipse);
	if (ok) ok = SetIsCircular(pEllipse, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadCoord(&CentrePoint);
	if (ok) ok = SetCentrePoint(pEllipse, CentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&Width);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Height);
	if (ok) ok = SetHeightAndWidth(pEllipse, Height, Width);

	if (ok) ok = InvalidateCache(pEllipse);
	if (ok) ok = InsertNode(pEllipse);

	return ok;
}

BOOL EllipseRecordHandler::ReadEllipseComplex(CXaraFileRecord *pCXaraFileRecord)
{
	BOOL ok = TRUE;

	NodeRegularShape *pEllipse;
	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;

	pEllipse = new NodeRegularShape;

	if (ok) ok = SetupShape(pEllipse);
	if (ok) ok = SetIsCircular(pEllipse, TRUE);

	if (ok) ok = pCXaraFileRecord->ReadCoord(&CentrePoint);
	if (ok) ok = SetCentrePoint(pEllipse, CentrePoint);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = SetMajorAxis(pEllipse, MajorAxis);

	if (ok) ok = pCXaraFileRecord->ReadCoordTrans(&MinorAxis,0,0);
	if (ok) ok = SetMinorAxis(pEllipse, MinorAxis);

	if (ok) ok = InvalidateCache(pEllipse);
	if (ok) ok = InsertNode(pEllipse);

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

#if XAR_TREE_DIALOG
void EllipseRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord, StringBase *pStr)
{
	if (pRecord != NULL && pStr != NULL)
	{
		// Call base class first
		// This outputs the tag and size
		CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

		switch (pRecord->GetTag())
		{
			case TAG_ELLIPSE_SIMPLE : DescribeEllipseSimple(pRecord, pStr); break;
			case TAG_ELLIPSE_COMPLEX : DescribeEllipseComplex(pRecord, pStr); break;
			default : DescribeInvalid(pRecord, pStr); break;
		}
	}
}

void EllipseRecordHandler::DescribeEllipseSimple(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	DocCoord CentrePoint;
	INT32 Height;
	INT32 Width;
	char s[256];

	BOOL ok = TRUE;
	if (ok) ok = pRecord->ReadCoord(&CentrePoint);
	if (ok) ok = pRecord->ReadINT32(&Width);
	if (ok) ok = pRecord->ReadINT32(&Height);

	(*pStr) += "Simple ellipse\r\n\r\n";

	_stprintf(s,"Centre point\t= %d, %d\r\n",CentrePoint.x,CentrePoint.y);
	(*pStr) += s;
	_stprintf(s,"Width\t\t= %d\r\n",Width);
	(*pStr) += s;
	_stprintf(s,"Height\t\t= %d\r\n",Height);
	(*pStr) += s;
}

void EllipseRecordHandler::DescribeEllipseComplex(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	DocCoord CentrePoint;
	DocCoord MajorAxis;
	DocCoord MinorAxis;
	char s[256];

	BOOL ok = TRUE;

	if (ok) ok = pRecord->ReadCoord(&CentrePoint);
	if (ok) ok = pRecord->ReadCoordTrans(&MajorAxis,0,0);
	if (ok) ok = pRecord->ReadCoordTrans(&MinorAxis,0,0);

	(*pStr) += "Complex ellipse\r\n\r\n";
	_stprintf(s,"Centre point\t= %d, %d\r\n",CentrePoint.x,CentrePoint.y);
	(*pStr) += s;
	_stprintf(s,"Major axis\t\t= %d, %d\r\n",MajorAxis.x,MajorAxis.y);
	(*pStr) += s;
	_stprintf(s,"Minor axis\t\t= %d, %d\r\n",MinorAxis.x,MinorAxis.y);
	(*pStr) += s;
}

void EllipseRecordHandler::DescribeInvalid(CXaraFileRecord *pRecord, StringBase *pStr)
{
	if (pRecord == NULL || pStr == NULL)
		return;

	(*pStr) += "Invalid ellipse\r\n";
}
#endif

