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

// Brush point info classes header
// This file contains the declarations of various small classes that are used to store data 
// relevant to the brush attribute

#ifndef INC_BRPRESS
#define INC_BRPRESS

class CCObject;
class CXaraFileRecord;

class BrushPointInfo;
typedef std::map< MILLIPOINT, BrushPointInfo > PointsMap;

class CPathPointInfo
{
public:
	CPathPointInfo() { m_Point.x = m_Point.y = 0; m_Tangent = 0; }
	
	CPathPointInfo &operator=(CPathPointInfo CP)
	{
		m_Point.x = CP.m_Point.x; 
		m_Point.y = CP.m_Point.y;							
		m_Tangent = CP.m_Tangent;

		return *this;
	}

public: //members				  
	DocCoord	m_Point;
	double		m_Tangent;
};

/********************************************************************************************

>	class PressureItem : public CCObject

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	to store information about brush objects, including pressure
				
	Notes:		
********************************************************************************************/

class PressureItem : public CCObject
{
CC_DECLARE_MEMDUMP(PressureItem)

public:
	PressureItem() {m_Pressure = 0; m_Proportion = 0; m_Distance = 0;}
	PressureItem(const PressureItem&);
	PressureItem(UINT32 Pressure, double Proportion, MILLIPOINT Distance);

	PressureItem operator=(const PressureItem& Other);

	void WorkOutProportion(MILLIPOINT PathLength);
	BOOL WriteNative(CXaraFileRecord* pRecord);

public: //data
	UINT32 m_Pressure;
	double m_Proportion;
	MILLIPOINT m_Distance;
};


/********************************************************************************************

>	class BrushPointInfo : public CPathPointInfo

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	to store information about brush objects, including pressure
				
	Notes:		This class inherits coordinate and tangent data from CPathPointInfo
				This is a pretty fast and loose implementation but so it the parent and i
				didn't write that.

********************************************************************************************/

class BrushPointInfo : public CPathPointInfo
{
public:
	BrushPointInfo();
	BrushPointInfo(DocCoord Point, double Tangent, UINT32 Pressure);

	BrushPointInfo& operator=(BrushPointInfo &Other);

public:
	// data
	UINT32 m_Pressure;
};

/********************************************************************************************

>	class TimeStampBrushPoint : public BrushPointInfo

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/1/97

	Purpose:	to store information about brush objects generated by the timestamping method
				
	Notes:		This class inherits coordinate and tangent data from CPathPointInfo
				This is a pretty fast and loose implementation but so it the parent and i
				didn't write that.

********************************************************************************************/


class TimeStampBrushPoint : public BrushPointInfo
{
public:
	TimeStampBrushPoint();
	TimeStampBrushPoint(CPathPointInfo PointInfo);
	TimeStampBrushPoint(DocCoord Point, double Tangent, MILLIPOINT Distance); 
	TimeStampBrushPoint(CPathPointInfo PointInfo, MILLIPOINT Distance);
	void WorkOutProportion(MILLIPOINT PathLength);

	static INT32  ComparePointInfo(const void* Point1, const void* Point2);

	BOOL WriteNative(CXaraFileRecord* pRecord);

public: //members
	TimeStampBrushPoint operator=(TimeStampBrushPoint OtherPoint);
	MILLIPOINT m_Distance; // the distance along the path
	double     m_Proportion; // the proportional distance along the path
};


typedef std::list< TimeStampBrushPoint >	TimeStampList;
typedef std::vector< PressureItem >			PressureArray;
typedef std::list< PressureItem >			PressureList;

#endif
