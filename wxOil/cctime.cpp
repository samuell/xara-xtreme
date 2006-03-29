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
// implementation file for cctime class

/*
*/

#include "camtypes.h"
#include "basestr.h"
#include "convert.h"
#include "cctime.h"
#include <time.h>

CC_IMPLEMENT_DYNCREATE( CCTimeBase, CCObject )
CC_IMPLEMENT_DYNCREATE( CCTime, CCTimeBase )
CC_IMPLEMENT_DYNCREATE( CCGmtTime, CCTimeBase )

#define new CAM_DEBUG_NEW     

/*********************************************************************************************
           
>	CCTime::CCTimeBase()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	CCTimeBase constructor, doesn't do much for the moment
	
**********************************************************************************************/  

CCTimeBase::CCTimeBase()
{
	TheTime.tm_sec	=  0;    // seconds after the minute - [0,59]
	TheTime.tm_min	=  0;    // minutes after the hour - [0,59]
	TheTime.tm_hour	=  0;    // hours since midnight - [0,23]
	TheTime.tm_mday	=  0;    // day of the month - [1,31]	
	TheTime.tm_mon	=  0;    // months since January - [0,11]
	TheTime.tm_year	=  0;    // years since 1900
	TheTime.tm_wday =  0;    // days since Sunday - [0,6]
	TheTime.tm_yday =  0;    // days since January 1 - [0,365] 
	TheTime.tm_isdst = 0;    // daylight savings time flag 
}


/*********************************************************************************************
           
>	CCTimeBase::~CCTimeBase()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Destructor for a CCTimeBase object
	
**********************************************************************************************/  

CCTimeBase::~CCTimeBase()
{
}

/*********************************************************************************************
           
>	cctime* CCTimeBase::GetTimeStruct()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Return a pointer to our internal time structure for derived classes to use.
				They should really use the access functions for all but the initialisation
				of this structure.
	
**********************************************************************************************/  

cctime* CCTimeBase::GetTimeStruct()
{
	return &TheTime;
}



/*********************************************************************************************
           
>	INT32 CCTimeBase::GetDay()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	returns the Day of the month, ranges between 1 and 31
	
**********************************************************************************************/  

INT32 CCTimeBase::GetDay()
{
	return TheTime.tm_mday;
}


/*********************************************************************************************
           
>	INT32 CCTimeBase::GetDayOfWeek()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	return the day of the week, 1=Sunday, 2=Monday etc
	
**********************************************************************************************/  

INT32 CCTimeBase::GetDayOfWeek()
{
	return TheTime.tm_wday;
}


/*********************************************************************************************
           
>	INT32 CCTimeBase::Get24Hour()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	returns the hour, based on local time, in the range 0 through 23
	
**********************************************************************************************/  

INT32 CCTimeBase::GetHour()
{
	return TheTime.tm_hour;
}

INT32 CCTimeBase::Get12Hour()
{
	INT32 t = GetHour();
	if (t>12) t-=12;
	if (t==0) t+=12;	// adjust for midnight 0 time
	return t;
}

INT32 CCTimeBase::Get24Hour()
{
	return GetHour();
}


/*********************************************************************************************
           
>	INT32 CCTimeBase::GetMinute()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Returns the minute based on local time, in the range 0 through 59.
	
**********************************************************************************************/  

INT32 CCTimeBase::GetMinute()
{
	return TheTime.tm_min;
}


/*********************************************************************************************
           
>	INT32 CCTimeBase::GetMonth()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Returns the month based on local time, in the range 1 through 12, (1=January)
	
**********************************************************************************************/  

INT32 CCTimeBase::GetMonth()
{
	return TheTime.tm_mon;
}


/*********************************************************************************************
           
>	INT32 CCTimeBase::GetSecond()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Returns the second based on local time, in the range 0 through 59,
	
**********************************************************************************************/  

INT32 CCTimeBase::GetSecond()
{
	return TheTime.tm_sec;
}


/*********************************************************************************************
           
>	INT32 CCTimeBase::GetYear()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Returns the year based on local time, in the range 1970 through 2038,
	
**********************************************************************************************/  

INT32 CCTimeBase::GetYear()
{
	return TheTime.tm_year;
}


/*********************************************************************************************
           
>	CCTimeBase& CCTimeBase::operator=(const CCTimeBase& timeSrc)

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Assign one CCTimeBase object to another
	
**********************************************************************************************/  

const CCTimeBase& CCTimeBase::operator=(const CCTimeBase& timeSrc)
{
	TheTime = timeSrc.TheTime;
	return (*this);
}


/*********************************************************************************************
           
>	void CCTimeBase::ConvertStandardDateAndTime(const String_64 *pFormat, String_256 *pOutput)

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
	Inputs:		pFormat = a pointer to a format string
				pOutput = a pointer to a string to receive the formated date and time
	Outputs		pOutput = contains the formated date and time
    Returns:	-
    Purpose:	Build a date and time string using the format string provided.
				Allowable format fields are
					%yr = year
					%mt = month
					%dy = day
					%hr = hours (24 hour clock)
					%mn = minutes
					%sc = seconds
					%th = hours (12 hour clock)
					%pm = am/pm characters
					%% = %
				These fields should be embedded in the format string along with other
				characters eg
				"%hr/%mn/%sc"				   = 19/22/33
				"%dy/%mt/%yr"				   = 21/12/1996
				"%dy/%mt/%yr - %hr/%mn/%sc"    = 21/12/1996 - 19/22/33"
				"%dy/%mt/%yr - %th.%mn %pm"	   = 21/12/1996 - 6.45 am
	
**********************************************************************************************/  

void CCTimeBase::ConvertStandardDateAndTime(const String_64 *pFormat, String_256 *pOutput)
{
	if (pFormat==NULL || pOutput==NULL)
		return;

	String_64 Format = (*pFormat);

	const TCHAR percent = TEXT('%');
	
	// find the number of characters in this string
	INT32 ilen = Format.Length();
	INT32 ipos = 0;
	String_32 tempstr;
	INT32 num,az;
	TCHAR ch0,ch1,ch2;

	while (ipos<ilen)
	{
		ch0 = Format[ipos];
		if (ch0==percent)
		{
			if ((ipos+2) < ilen)
			{
				ch1 = Format[ipos+1];
				ch2 = Format[ipos+2];

				if (ch1==percent)
				{
					(*pOutput) += ch1;
					ipos+=1;
				}
				else
				{
					if (ch1==TEXT('p') && ch2==TEXT('m'))
					{
						(*pOutput) += ((GetHour()>12) ? TEXT("pm") : TEXT("am"));
						ipos+=2;
					}
					else
					{
						num = -1;
						az = 0;

							 if (ch1==TEXT('y') && ch2==TEXT('r'))	{	num = (INT32)GetYear() % 100;
																		az=1;	}
						else if (ch1==TEXT('m') && ch2==TEXT('t'))	{	num = (INT32)GetMonth();	az=0;	}
						else if (ch1==TEXT('d') && ch2==TEXT('y'))	{	num = (INT32)GetDay();	az=0;	}
						else if (ch1==TEXT('h') && ch2==TEXT('r'))	{	num = (INT32)GetHour();	az=1;	}
						else if (ch1==TEXT('m') && ch2==TEXT('n'))	{	num = (INT32)GetMinute();az=1;	}
						else if (ch1==TEXT('s') && ch2==TEXT('c'))	{	num = (INT32)GetSecond();az=1;	}
						else if (ch1==TEXT('t') && ch2==TEXT('h'))	{	num = (INT32)Get12Hour();az=0;	}

						if (num>-1)
						{
							Convert::LongToString(num, &tempstr);
							if (az==1 && num<10)
								(*pOutput)+=TEXT('0');
							(*pOutput) += tempstr;
							ipos+=2;
						}
					}
				}
			}
			else
				(*pOutput) += ch0;
		}
		else
			(*pOutput) += ch0;

		ipos++;
	}
}












/*********************************************************************************************
           
>	CCTime::CCTime()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Constructor for CCTime object. We get the current local time ready for
				use.

**********************************************************************************************/  

CCTime::CCTime()
{
	// Force the time to be up-to-the-second
	GetCurrentTime();
}

/*********************************************************************************************
           
>	void CCTime::GetCurrentTime()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Updates this CCTime object with the date and time now.

**********************************************************************************************/  

void CCTime::GetCurrentTime()
{
	struct tm *newtime;
	time_t long_time;

    // Set time zone from TZ environment variable. If TZ is not set,
    // operating system default is used, otherwise PST8PDT is used
    // (Pacific standard time, daylight savings).
    _tzset();

	time( &long_time );                // Get time as long integer.
	newtime = localtime( &long_time ); // Convert to local time.

	cctime *p_time = GetTimeStruct();

	p_time->tm_sec	= newtime->tm_sec;
	p_time->tm_min	= newtime->tm_min;
	p_time->tm_hour	= newtime->tm_hour; 
	p_time->tm_mday	= newtime->tm_mday; 
	p_time->tm_mon	= newtime->tm_mon;
	p_time->tm_year	= newtime->tm_year;
	p_time->tm_wday	= newtime->tm_wday; 
	p_time->tm_yday	= newtime->tm_yday; 
	p_time->tm_isdst= newtime->tm_isdst;
}

/*********************************************************************************************
           
>	INT32 CCTime::GetMonth()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Returns the month based on local time, in the range 1 through 12, (1=January)
	
**********************************************************************************************/  

INT32 CCTime::GetMonth()
{
	return CCTimeBase::GetMonth() + 1;
}








/*********************************************************************************************
           
>	CCGmtTime::CCGmtTime()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Costructor for CCGmtTime....

**********************************************************************************************/  

CCGmtTime::CCGmtTime()
{
	// Force the time to be up-to-the-second
	GetCurrentTime();
}


/*********************************************************************************************
           
>	void CCGmtTime::GetCurrentTime()

    Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
    Created:	21/08/96
    Purpose:	Updates this CCGMTTime object with the date and time now.

**********************************************************************************************/  

void CCGmtTime::GetCurrentTime()
{
	time_t osBinaryTime;
	time(&osBinaryTime);
	CTime t(osBinaryTime);

	cctime *p_time = GetTimeStruct();
	struct tm *p_tm;
	p_tm = t.GetGmtTm(NULL);

	p_time->tm_sec	= p_tm->tm_sec;
	p_time->tm_min	= p_tm->tm_min;
	p_time->tm_hour	= p_tm->tm_hour; 
	p_time->tm_mday	= p_tm->tm_mday; 
	p_time->tm_mon	= p_tm->tm_mon;
	p_time->tm_year	= p_tm->tm_year;
	p_time->tm_wday	= p_tm->tm_wday; 
	p_time->tm_yday	= p_tm->tm_yday; 
	p_time->tm_isdst= p_tm->tm_isdst;
}


INT32 CCGmtTime::GetMonth()
{
	return CCTimeBase::GetMonth() + 1;
}

INT32 CCGmtTime::GetYear()
{
	return CCTimeBase::GetYear() + 1900;
}








/*
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

void main()
{
    char tmpbuf[128], ampm[] = "AM";
    time_t ltime;
    struct _timeb tstruct;
    struct tm *today, *gmt, xmas = { 0, 0, 12, 25, 11, 93 };

    // Set time zone from TZ environment variable. If TZ is not set,
    // operating system default is used, otherwise PST8PDT is used
    // (Pacific standard time, daylight savings).
    
    _tzset();

    // Display operating system-style date and time.
    _strtime( tmpbuf );
    printf( "OS time:\t\t\t\t%s\n", tmpbuf );
    _strdate( tmpbuf );
    printf( "OS date:\t\t\t\t%s\n", tmpbuf );

    // Get UNIX-style time and display as number and string.
    time( &ltime );
    printf( "Time in seconds since UTC 1/1/70:\t%ld\n", ltime );
    printf( "UNIX time and date:\t\t\t%s", ctime( &ltime ) );

    // Display UTC.
    gmt = gmtime( &ltime );
    printf( "Coordinated universal time:\t\t%s", asctime( gmt ) );

    // Convert to time structure and adjust for PM if necessary.
    today = localtime( &ltime );
    if( today->tm_hour > 12 )
    {
		strcpy( ampm, "PM" );
		today->tm_hour -= 12;
    }
    if( today->tm_hour == 0 )  // Adjust if midnight hour.
   today->tm_hour = 12;

    // Note how pointer addition is used to skip the first 11 
    // characters and printf is used to trim off terminating 
    // characters.

    printf( "12-hour time:\t\t\t\t%.8s %s\n",
       asctime( today ) + 11, ampm );

    // Print additional time information.
    _ftime( &tstruct );
    printf( "Plus milliseconds:\t\t\t%u\n", tstruct.millitm );
    printf( "Zone difference in seconds from UTC:\t%u\n", 
             tstruct.timezone );
    printf( "Time zone name:\t\t\t\t%s\n", _tzname[0] );
    printf( "Daylight savings:\t\t\t%s\n", 
             tstruct.dstflag ? "YES" : "NO" );

    // Make time for noon on Christmas, 1993. 
    if( mktime( &xmas ) != (time_t)-1 )
   printf( "Christmas\t\t\t\t%s\n", asctime( &xmas ) );

    // Use time structure to build a customized time string.
    today = localtime( &ltime );

    // Use strftime to build a customized time string.
    strftime( tmpbuf, 128,
         "Today is %A, day %d of %B in the year %Y.\n", today );
    printf( tmpbuf );
}

  
Output
  
OS time:                                21:51:03
OS date:                                05/03/94
Time in seconds since UTC 1/1/70:       768027063
UNIX time and date:                     Tue May 03 21:51:03 1994
Coordinated universal time:             Wed May 04 04:51:03 1994
12-hour time:                           09:51:03 PM
Plus milliseconds:                      279
Zone difference in seconds from UTC:    480
Time zone name:                         
Daylight savings:                       YES
Christmas                               Sat Dec 25 12:00:00 1993

Today is Tuesday, day 03 of May in the year 1994.
*/

