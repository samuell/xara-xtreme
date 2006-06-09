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

// brush pressure classes implementation

#include "camtypes.h"
#include "brpress.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "pen.h"
#include "ndbldpth.h"

CC_IMPLEMENT_MEMDUMP(PressureItem, CCObject);

/********************************************************************************************

>	PressureItem::PressureItem(UINT32 Pressure, double Proportion, MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97
	Inputs:		Pressure - the pressure value
				Proportion - the proportional distance along the path
				Distance - the absolute distance along the path
	Purpose:	constructor with initialisation
				
	Notes:		
********************************************************************************************/

PressureItem::PressureItem(UINT32 Pressure, double Proportion, MILLIPOINT Distance)
{
	m_Pressure = Pressure;
	m_Proportion = Proportion;
	m_Distance = Distance;
}

/********************************************************************************************

>	PressureItem::PressureItem(PressureItem& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97
	Inputs:		Other
	Purpose:	copy constructor
				
	Notes:		
********************************************************************************************/

PressureItem::PressureItem(const PressureItem& Other)
{
	m_Pressure = Other.m_Pressure;
	m_Proportion = Other.m_Proportion;
	m_Distance = Other.m_Distance;
}


/********************************************************************************************

>	PressureItem::PressureItem(PressureItem& Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97
	Inputs:		Other
	Purpose:	copy constructor
				
	Notes:		
********************************************************************************************/

PressureItem PressureItem::operator =(const PressureItem& Other)
{
	m_Pressure = Other.m_Pressure;
	m_Proportion = Other.m_Proportion;
	m_Distance = Other.m_Distance;

	return *this;
}


/********************************************************************************************

>	PressureItem::WorkOutProportion(MILLIPOINT PathLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		length of the path that this object is on
	Purpose:	calculates how far along the path we are and sets the member variable

********************************************************************************************/

void PressureItem::WorkOutProportion(MILLIPOINT PathLength)
{
	if (m_Distance < 0 || m_Distance > PathLength)
	{
		ERROR3("Attempting to calculate proportional distance with invalid data");
		TRACEUSER( "Diccon", _T("Invalid distance: %d, Path Length: %d\n"), m_Distance, PathLength );
		m_Proportion = 1.0;
		return;
	}
	m_Proportion = (double)((double)m_Distance / (double)PathLength);
	//if (m_Proportion < 0)
//	TRACEUSER( "Diccon", _T("Proportion: %f\n"), m_Proportion);

}

		
/********************************************************************************************


>	BOOL PressureItem::WriteNative(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pFilter - filter to write to
	Returns:	TRUE if the Node has written out a record to the filter
	Purpose:	Writes out a record of this PressureItem to the record supplied.  Note that because
				PressureItems are generally part of a larger object they do not have their own tag etc.
				This function is merely a convenience function.

	SeeAlso:	AttrBrushType::WritePreChildrenNative; BrushAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL PressureItem::WriteNative(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL in PressureItem::WriteNative");
	
	BOOL ok = pRecord->WriteINT32((INT32)m_Pressure);
	if (ok) ok = pRecord->WriteDOUBLE(m_Proportion);
	if (ok) ok = pRecord->WriteINT32(m_Distance);

	return ok;
}





/********************************************************************************************

>	BrushPointInfo::BrushPointInfo()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	Default Constuctor for BrushPointInfo

********************************************************************************************/

BrushPointInfo::BrushPointInfo()
{
	m_Pressure = MAXPRESSURE / 2;
	m_Tangent = 0.0;
	m_Point = DocCoord(0, 0);
}


/********************************************************************************************

>	BrushPointInfo::BrushPointInfo(DocCoord Point, double Tangent, UINT32 Pressure)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	initialising Constuctor for BrushPointInfo

********************************************************************************************/

BrushPointInfo::BrushPointInfo(DocCoord Point, double Tangent, UINT32 Pressure)
{
	m_Point = Point;
	m_Tangent = Tangent;
	m_Pressure = Pressure;
}


/********************************************************************************************

>	BrushPointInfo BrushPointInfo::operator=(BrushPointInfo &Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	initialising Constuctor for BrushPointInfo

********************************************************************************************/

BrushPointInfo& BrushPointInfo::operator =(BrushPointInfo &Other)
{
	m_Point = Other.m_Point;
	m_Tangent = Other.m_Tangent;
	m_Pressure = Other.m_Pressure;

	return *(this);
}


/********************************************************************************************

>	TimeStampBrushPoint::TimeStampBrushPoint()

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	Default Constuctor for TimeStampBrushPoint

********************************************************************************************/

TimeStampBrushPoint::TimeStampBrushPoint() : BrushPointInfo()
{
	m_Distance = -1;
	m_Proportion = -1;
}


/********************************************************************************************

>	TimeStampBrushPoint::TimeStampBrushPoint(CPathPointInfo)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	Constuctor for TimeStampBrushPoint

********************************************************************************************/

TimeStampBrushPoint::TimeStampBrushPoint(CPathPointInfo PathInfo)
{
	m_Point = PathInfo.m_Point;
	m_Tangent = PathInfo.m_Tangent;
	m_Distance = -1;
	m_Proportion = -1;
}

/********************************************************************************************

>	TimeStampBrushPoint::TimeStampBrushPoint(CPathPointInfo)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	Constuctor for TimeStampBrushPoint

********************************************************************************************/

TimeStampBrushPoint::TimeStampBrushPoint(CPathPointInfo PathInfo, MILLIPOINT Distance)
{
	m_Point = PathInfo.m_Point;
	m_Tangent = PathInfo.m_Tangent;
	m_Distance = Distance;
	m_Proportion = -1;
}


/********************************************************************************************

>	TimeStampBrushPoint::TimeStampBrushPoint(DocCoord Point, double Tangent, MILLIPOINT Distance)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		-
	Purpose:	Constuctor for TimeStampBrushPoint

********************************************************************************************/

TimeStampBrushPoint::TimeStampBrushPoint(DocCoord Point, double Tangent, MILLIPOINT Distance)
{
	m_Point = Point;
	m_Tangent = Tangent;
	m_Distance = Distance;
}

/********************************************************************************************

>	TimeStampBrushPoint TimeStampBrushPoint::operator=(TimeStampBrushPoint Other)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		Other - the TSBP to assign
	Purpose:	assignment operator
********************************************************************************************/

TimeStampBrushPoint TimeStampBrushPoint::operator=(TimeStampBrushPoint Other)
{
	m_Point = Other.m_Point;
	m_Tangent = Other.m_Tangent;
	m_Distance = Other.m_Distance;
	m_Proportion = Other.m_Proportion;

	return *this;
}

/********************************************************************************************

>	TimeStampBrushPoint::WorkOutProportion(MILLIPOINT PathLength)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Inputs:		length of the path that this object is on
	Purpose:	calculates how far along the path we are and sets the member variable

********************************************************************************************/

void TimeStampBrushPoint::WorkOutProportion(MILLIPOINT PathLength)
{
	if (m_Distance < 0 || m_Distance > PathLength)
	{
	//	ERROR3("Attempting to calculate proportional distance with invalid data");
		TRACEUSER( "Diccon", _T("Invalid distance: %d, Path Length: %d\n"), m_Distance, PathLength );
		m_Proportion = 1.0;
		return;
	}
	m_Proportion = (double)((double)m_Distance / (double)PathLength);
	//if (m_Proportion < 0)
//	TRACEUSER( "Diccon", _T("Proportion: %f\n"), m_Proportion);

}


/********************************************************************************************

>	static INT32 TimeStampBrushPoint::ComparePointInfo(void* Point1, void* Point2)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/99
	Inputs:     Point1, Point2- the distances to be compared
	Returns:	-1, if Dist1 < Dist2
				0 , if Dist1 == Dist2
				1, if Dist1 > Dist2
	Purpose:	Compares the two points by their distances, note that they are specified as void*
				so that I can supply this fn.to qsort
	SeeAlso:	-

********************************************************************************************/

INT32 TimeStampBrushPoint::ComparePointInfo(const void* Point1, const void* Point2)
{
	TimeStampBrushPoint* pPoint1 = (TimeStampBrushPoint*)Point1;
	TimeStampBrushPoint* pPoint2 = (TimeStampBrushPoint*)Point2;
	
	if (pPoint1->m_Distance < pPoint2->m_Distance)
		return -1;
	if (pPoint1->m_Distance > pPoint2->m_Distance)
		return 1;

	return 0;
}

		
/********************************************************************************************


>	BOOL TimeStampBrushPoint::WriteNative(BaseCamelotFilter *pFilter)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99
	Inputs:		pFilter - filter to write to
	Returns:	TRUE if the Node has written out a record to the filter
	Purpose:	Writes out a record of this TSBP to the record supplied.  Note that because
				TSBPs are generally part of a larger object they do not have their own tag etc.
				This function is merely a convenience function.

	SeeAlso:	AttrBrushType::WritePreChildrenNative; BrushAttrRecordHandler::HandleRecord

********************************************************************************************/

BOOL TimeStampBrushPoint::WriteNative(CXaraFileRecord* pRecord)
{
	ERROR2IF(pRecord == NULL, FALSE, "Record is NULL in TimeStampBrushPoint::WriteNative");
	
	BOOL ok    = pRecord->WriteCoord(m_Point);
	TRACEUSER( "Diccon", _T("Wrote Point %d, %d\n"), m_Point.x, m_Point.y );
	if (ok) ok = pRecord->WriteDOUBLE(m_Tangent);
	if (ok) ok = pRecord->WriteDOUBLE(m_Proportion);
	if (ok) ok = pRecord->WriteINT32(m_Distance);

	return ok;
}

