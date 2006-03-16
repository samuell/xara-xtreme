// $Id: cctime.h 662 2006-03-14 21:31:49Z alex $
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
// A time related class.

#ifndef INC_CCTIME
#define INC_CCTIME

class String_64;
class String_256;

struct cctime 
{
	INT32 tm_sec;     // seconds after the minute - [0,59]
	INT32 tm_min;     // minutes after the hour - [0,59]
	INT32 tm_hour;    // hours since midnight - [0,23]
	INT32 tm_mday;    // day of the month - [1,31]	
	INT32 tm_mon;     // months since January - [0,11]
	INT32 tm_year;    // years since 1900
	INT32 tm_wday;    // days since Sunday - [0,6]
	INT32 tm_yday;    // days since January 1 - [0,365] 
	INT32 tm_isdst;   // daylight savings time flag 
};


/*********************************************************************************************
           
>	class CCTimeBase : public CCObject

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	The base class local time generator. Normal time generators should be
				derived from this
	
**********************************************************************************************/  

class CCTimeBase : public CCObject
{
	CC_DECLARE_DYNCREATE(CCTimeBase)

	public:
		CCTimeBase();
		virtual ~CCTimeBase();

		virtual INT32 GetDay();
		virtual INT32 GetDayOfWeek();
		virtual INT32 GetMonth();
		virtual INT32 GetYear();
		virtual INT32 GetHour();
		virtual INT32 Get12Hour();
		virtual INT32 Get24Hour();
		virtual INT32 GetMinute();
		virtual INT32 GetSecond();
		
		virtual void GetCurrentTime() {};
		virtual void ConvertStandardDateAndTime(const String_64 *pFormat, String_256 *pOutput);

		const CCTimeBase& operator=(const CCTimeBase& timeSrc);

	protected:
		cctime* GetTimeStruct();

	private:
		cctime TheTime;
};



/*********************************************************************************************
           
>	class CCTime : public CCTimeBase

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Creates a time object which can be interogated about the current time.
				If you create a static version of this object you will most likely need to
				use the GetCurrentTime() function on the object from time to time, otherwise
				it will inevitable become out of date as time and tide waits for no man.
				This is not a class to use to time funtions and such. Its accuracy is to
				the nearest second only.
				This version returns local time.
	SeeAlso:	CCGmtTime
	
**********************************************************************************************/  

class CCTime : public CCTimeBase
{
	CC_DECLARE_DYNCREATE(CCTime)

	public:
		CCTime();

		virtual void GetCurrentTime();
		virtual INT32  GetMonth();
};




/*********************************************************************************************
           
>	class CCGmtTime : public CCTimeBase

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Creates a time object which can be interogated about the current time.
				If you create a static version of this object you will most likely need to
				use the GetCurrentTime() function on the object from time to time, otherwise
				it will inevitable become out of date as time and tide waits for no man.
				This is not a class to use to time funtions and such. Its accuracy is to
				the nearest second only.
				This version returns Greenwich Mean Time rather than Local time
	SeeAlso::	CCTime
	
**********************************************************************************************/  

class CCGmtTime : public CCTimeBase
{
	CC_DECLARE_DYNCREATE(CCGmtTime)

	public:
		CCGmtTime();

		virtual INT32 GetMonth();
		virtual INT32 GetYear();

		virtual void GetCurrentTime();
};



#endif



