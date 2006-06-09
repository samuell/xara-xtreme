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

/*
	monotime.cpp
	Created by Phil: 8th, July 1993

	Implements the MonotonicTime class
	This class stores samples of the ever-increasing system ticker (the "monotonic clock" in
	Archy speak) and stores them. The samples can then be manipulated by member functions.
	Where times are passed in and out of these functions they are alwasy given in
	milliseconds.

*/

/*
//*/

#include "camtypes.h"
//#include "monotime.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");

wxStopWatch		MonotonicTime::s_stopwatch;

/********************************************************************************************

>	static void MonotonicTime::Init() 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/02/2006
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	MonotonicTime initialiser
	Errors:		None

********************************************************************************************/

void MonotonicTime::Init()
{
	s_stopwatch.Start();
}




/********************************************************************************************

>	MonotonicTime::MonotonicTime() 

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	MonotonicTime class constructor
				Create an instance of the time class by taking a sample of MonotonicTime.
	Errors:		None

	
********************************************************************************************/
/*
	Technical notes:
	Calls Sample which in turn calls Windows 3.1 GetTickCount.

********************************************************************************************/

MonotonicTime::MonotonicTime()
{
	Sample();
}




/********************************************************************************************

>	MonotonicTime::MonotonicTime(MonotonicTime initialiser)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	MonotonicTime class constructor
				Create an instance of the time class by setting it to the specified time
	Errors:		None

	
********************************************************************************************/
/*
	Technical notes:

********************************************************************************************/

MonotonicTime::MonotonicTime(UINT32 initialiser)
{
	SampleTime = initialiser;
}




/********************************************************************************************

>	UINT32 MonotonicTime::Sample()

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		None
	Outputs:	None
	Returns:	The Sampled time
	Purpose:	Take a sample of "now" in monotonic time
	Errors:		None

	
********************************************************************************************/
/*
	Technical notes:
	Calls Windows 3.1 GetTickCount function.

********************************************************************************************/

UINT32 MonotonicTime::Sample()
{
#if defined(__WXMSW__)
	SampleTime = GetTickCount();
#elif defined(__WXGTK__) || defined(__WXMAC__)
//	SampleTime = count();
//	SampleTime = GetPosixTickCount();
	SampleTime = s_stopwatch.Time();
#else
	#pragma error( "Not impl'ed from this architechure" );
#endif

	return SampleTime;
}




/********************************************************************************************

>	BOOL MonotonicTime::Elapsed(UINT32 interval)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		None
	Outputs:	None
	Returns:	Flag saying whether interval has elapsed or not
	Purpose:	Test whether the given time interval has elapsed since the monotonic time was
				last sampled.
	Errors:		None

	
********************************************************************************************/
/*
	Technical notes:

********************************************************************************************/

BOOL MonotonicTime::Elapsed(UINT32 interval)
{
#if defined(__WXMSW__)
	return ( ( SampleTime + interval ) < GetTickCount() );
#elif defined(__WXGTK__) || defined(__WXMAC__)
//	return( ( SampleTime + ( interval * CLOCKS_PER_SEC / 1000 ) ) < (UINT32)clock() );
//	return ( ( SampleTime + interval ) < GetPosixTickCount() );
	return ((SampleTime +interval) < (UINT32)s_stopwatch.Time());
#else
	#pragma error( "Not impl'ed from this architechure" );
#endif
}




/********************************************************************************************

>	BOOL MonotonicTime::Elapsed(UINT32 interval, BOOL update)

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/93
	Inputs:		None
	Outputs:	None
	Returns:	Flag saying whether interval has elapsed or not
	Purpose:	Test whether the given time interval has elapsed since the monotonic time was
				last sampled. If the update flag is set then the sample time is reset to be
				the EXACT time when the interval was exceeded. This allows the interval to be
				regular and accurate in the long-term even though individual intervals will
				be subject to timing and sampling innaccuracies
	Errors:		None

	
********************************************************************************************/
/*
	Technical notes:

********************************************************************************************/

BOOL MonotonicTime::Elapsed(UINT32 interval, BOOL update)
{
	if ( Elapsed( interval ) )
	{
//#if defined(__WXGTK__)
//		interval = interval * CLOCKS_PER_SEC / 1000;
//#endif		

		if ( update )
			SampleTime += interval;

		return TRUE;
	}
	return FALSE;
}




/********************************************************************************************
>	UINT32 MonotonicTime::operator UINT32()

	Author: 	Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/95
	Inputs:		None
	Outputs:	None
	Returns:	The monotonic time expressed as a UINT32
	Purpose:	Cast operator that allows comparisons of MonotonicTime objects to be made
	Errors:		None
********************************************************************************************/
MonotonicTime::operator UINT32()
{
	return SampleTime;
}




/********************************************************************************************
>	UINT32 MonotonicTime::GetPosixTickCount() const

	Author: 	Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/02/2006
	Inputs:		None
	Outputs:	None
	Returns:	The monotonic time extracted from the Posix library expressed as a UINT32
				in milliseconds
	Purpose:	Read high-quality monotonic timer value
	Errors:		None
********************************************************************************************/

//#if defined(__WXGTK__) || defined(__WXMAC__)
//// Should test that _POSIX_TIMERS > 0 && _POSIX_MONOTONIC_CLOCK > 0
//UINT32 MonotonicTime::GetPosixTickCount() const
//{
//	timespec ts;
//	clock_gettime(CLOCK_MONOTONIC, &ts);
//	return ts.tv_sec*1000+ts.tv_nsec/1000000;
//}
//#endif




