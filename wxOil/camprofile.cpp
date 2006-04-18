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


#include "camtypes.h"

//#include "camconfig.h"
#include "camprofile.h"

#if WIN32
// include something for GetSytemTimeAsFileTime
#else
#include <sys/time.h>
#include <time.h>
#endif

CC_IMPLEMENT_DYNCREATE(CamProfile, CCObject)
CC_IMPLEMENT_DYNCREATE(CamProfiler, CCObject)

CamProfile* CamProfile::pHead=NULL;
CamProfile* CamProfile::pTail=NULL;
CamProfile* CamProfile::pCurrent=NULL;
CamProfile* CamProfile::OurHead=NULL;
UINT64 CamProfile::LastOilTime=0;
UINT64 CamProfile::OilTimeArray[CAMPROFILE_NONE];
BOOL CamProfile::Running=0;
BOOL CamProfile::Inited=0;

/********************************************************************************************

>	CamProfile::CamProfile(CamProfileMode myMode = CAMPROFILE_NONE )


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamProfile::CamProfile(CamProfileMode myMode)
{
	// Set up in isolation

	Mode = CAMPROFILE_NONE;
	pPrev = NULL;
	pNext = NULL;
	OilTime = 0;
	
	Zombie = FALSE ;

	if (!Running)
	{
		Zombie = TRUE;
		return;
	}

	ERROR3IF(!Inited, "Profiling system not yet initialized");

	// Add us to the end of the list
	if (pTail)
	{
		pTail->pNext = this;
		pPrev = pTail;
	}
	pTail = this;

	if (!pHead)
	{
		pHead = this;
	}	

	// note we didn't alter pCurrent above, so it's still set to the old one
	SetMode(myMode);

}

/********************************************************************************************

>	CamProfile::~CamProfile()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamProfile::~CamProfile()
{
	if (Zombie)
	{
		return;
	}
	
	// If it's not a Zombie, but we are still running, we need to do
	// list maintenance etc.
	
	ERROR3IF(!Inited, "Profiling system not yet initialized");

	if (pPrev)
	{
		pPrev->pNext = pNext;
	}
	else
	{
		// No previous, we must have been the head
		pHead = pNext;
	}

	if (pNext)
	{
		pNext->pPrev = pPrev;
	}
	else
	{
		// We must have been the tail
		pTail = pPrev;
	}

	// Make sure we don't appear to be in the list still
	pNext=NULL;
	pPrev=NULL;

	// Reawake the previous profiler
	if (pTail)
	{
		// this should wipe us off as pCurrent too.
		pTail->SetMode(pTail->Mode);
	}
	else
	{	
		pCurrent = NULL;
	}

	ERROR3IF(pCurrent == this, "Somehow current profiler did not get unset");
}


/********************************************************************************************

>	BOOL CamProfile::Init()

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Starts up the profiling system
	Errors:		Returns FALSE on failure.
	Scope:		Static

********************************************************************************************/


BOOL CamProfile::Init()
{
	if (Inited) return(FALSE);

	for (INT32 i=0; i<CAMPROFILE_NONE; i++)
	{
		OilTimeArray[i]=0;
	}

	pHead = NULL;
	pTail = NULL;
	pCurrent = NULL;
	OurHead = new CamProfile;
	Running = FALSE;

	if (!OurHead) return FALSE;

	Inited = TRUE;
	
	// Give the lists a work through
	ActivateProfiling (TRUE);
	ActivateProfiling (FALSE);

	return TRUE;	

}


/********************************************************************************************

>	void CamProfile::SetMode(CamProfileMode myMode, BOOL Base=FALSE)

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		myMode - the new mode
				Base - TRUE to ditch the existing stack of modes
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Changes the mode of a profiler
	Errors:		Returns FALSE on failure.
	Scope:		

This is the heart of the profiling system. We've just been told our timer has changed
from its current state, to myMode. Therefore, if we were profiling before, we attribute
the time passed since that timer started to the relevant array, and mark the old profiler
as being dormant. We then set our own time, and set our start position.

********************************************************************************************/


void CamProfile::SetMode(CamProfileMode myMode, BOOL Base)
{
	if (Zombie || !Running)
	{
		return;
	}

	ERROR3IF(!Inited, "Profiling system not yet initialized");

	UINT64 NewTime;
#if WIN32
	// This windows code completely untested by Alex
	GetSystemTimeAsFileTime ((LPFILETIME)(&NewTime));
#else
	timeval tv;
	gettimeofday(&tv, NULL);
	NewTime = (UINT64)(tv.tv_usec)+ ((UINT64)(tv.tv_sec) * 1000000);
#endif

	// Mainly for debugging
	LastOilTime = NewTime;

	// TRACE(_T("CAMPROFILER OilTime is %lld\n"),NewTime);

	// If there is a current profiler, stop it.
	if (pCurrent && ( pCurrent->Mode != CAMPROFILE_NONE))
	{
		// Stop the current profiler running - note this may be us
		UINT64 Elapsed = NewTime - pCurrent->OilTime;
		OilTimeArray[pCurrent->Mode] += Elapsed;
		
		// TRACE(_T("CAMPROFILER Credit %lld to %d\n"),Elapsed,(INT32)pCurrent->Mode);

		pCurrent->OilTime = 0; // So we can detect screw-ups - we'll set it again when we start it
		pCurrent=NULL;
	}
#if 0
	else
	{
		TRACE(_T("CAMPROFILER AWOOGA1 pCurrent=%llx\n"),pCurrent);
		if (pCurrent)
		{
			TRACE(_T("CAMPROFILER AWOOGA2 Not crediting %llx ticks\n"),NewTime - pCurrent->OilTime);
		}
	}
#endif

	// If the Base flag is set, junk all the others on the list.
	if (Base && ( (pTail != this) || (pHead != this)))	// don't bother if we're the only one on the list
	{
		// We are going to junk all the existing profiles except us. Bwahahaha
		TRACE(_T("CAMPROFILER Clearing up redudant profiling entries\n"));

		// First junk them all
		CamProfile * p = pTail;
		while (p)
		{
			CamProfile* pn = p->pPrev;
			p->Zombie = TRUE;
			p->pPrev = NULL;
			p->pNext = NULL;
			p = pn;
			// sadly they are not ours to delete
		}
		
		// Now unjunk us
		Zombie = FALSE;
		pHead = this;
		pTail = this;
	}	


	// Now start us
	if (myMode != CAMPROFILE_NONE)
	{
		OilTime = NewTime;
		Mode = myMode;
		pCurrent = this;
	}

}

/********************************************************************************************

>	static void CamProfile::UpdateOilTimes();

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Updates the current oil times
	Errors:		Returns FALSE on failure.
	Scope:		static

This writes the OIL time back into the cumulative time. It should be used before
reading the OilTimesArray to ensure the current profiler hasn't got uncredited
time on.

********************************************************************************************/


void CamProfile::UpdateOilTimes()
{
	ERROR3IF(!Inited, "Profiling system not yet initialized");
	if (pCurrent && Running && !pCurrent->Zombie)
	{
		// Resetting the mode is sufficient
		pCurrent->SetMode(pCurrent->Mode);
	}
}

/********************************************************************************************

>	static void CamProfile::GetTimeString(TCHAR * pTime, UINT32 length);

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		length - maximum length of string to be filled in, in CHARACTERS including NULL
	Outputs:	pTime - filled in with string
	Returns:	None
	Purpose:	Returns the current Oil time
	Errors:		Returns FALSE on failure.
	Scope:		static


********************************************************************************************/


void CamProfile::GetTimeString(TCHAR * pTime, UINT32 length)
{
	ERROR3IF(!Inited, "Profiling system not yet initialized");
	UpdateOilTimes();
	if ((length < 26+6) || pCurrent && Running && !pCurrent->Zombie)
	{
		// Read from LastOilTime
		UINT64 Time=LastOilTime;
#if WIN32
		// Convert LastOil time to secs since 1 Jan 1970. We don't care about the date,
		// we only care about the time, so we pick a roughly correct number of DAYS
		// to add.
		// That's 369 years, which would have 92 leap years, except for the fact that
		// 1700, 1800, 1900 were not leap years. So that's 369 years of 365 days plus 89
		// leap year days, or 134,774 days, or 11,644,473,600 seconds
		// For the purist, I suppose we might be a few leap seconds out. Yawn.
		// Another test (thta's all one line):
		// 11perl -e '{use DateTime;$o=DateTime->new(year=>1601,month=>1,day=>1); 
		//            $n=DateTime->new(year=>1970,month=>1,day=>1), 
		//            printf "%d\n", $n->subtract_datetime_absolute($o)->in_units(seconds)}'
		//
		// 11644473600
		//
		Time = Time-11644473600LL*10000000LL;
		Time = (Time+5)/10;
#endif
		// Time is now in microseconds since 1 Jan 1970
		UINT64 uSecs = Time % 1000000;
		UINT64 Secs = Time / 1000000;
		time_t t = Secs;

		// Don't use ctime_r as it isn't present on Windows

		//Ask wx for a string with the time
		wxDateTime TheTime(t);
		wxString sTime(TheTime.FormatISOTime());

		// Now camSnprintf this into the string. This turns it into UNICODE if appropriate
		camSnprintf (pTime, length, _T("%s.%06d"), sTime.c_str(), uSecs);
	}
	else camStrncpy(pTime, _T("[UNAVAILABLE]"), length);
	pTime[length-1]=0; // ensure string terminated - string copy primatives do funny thing
}


/********************************************************************************************

>	static void CamProfile::AtBase(CamProfileMode myMode=CAMPROFILE_NONE)

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		None
	Outputs:	None
	Returns:	None
	Purpose:	Indicates to the profiling system we are at the base of the system
	Errors:		Returns FALSE on failure.
	Scope:		Static

This is the heart of the profiling system. We've just been told our timer has changed
from its current state, to myMode. Therefore, if we were profiling before, we attribute
the time passed since that timer started to the relevant array, and mark the old profiler
as being dormant. We then set our own time, and set our start position.

********************************************************************************************/


void CamProfile::AtBase(CamProfileMode myMode)
{
	ERROR3IF(!Inited, "Profiling system not yet initialized");

	if (!Running)
	{
		return;
	}

	if (!pTail)
	{
		// Note we allocate this, and discard the pointer. That's because it has already
		// added itself to the linked list
		if (OurHead) delete(OurHead);
		OurHead = new CamProfile;	
		if (!OurHead) return;
	}

	ERROR3IF(!pTail,"Didn't get a tail pointer");
	if (pTail)
	{
		pTail->SetMode(myMode, TRUE);
	}
}

/********************************************************************************************

>	static BOOL CamProfile::ResetCounters()

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Resets the profiling counters
	Errors:		Returns FALSE on failure.
	Scope:		Static

Reset the counters. DO NOT CALL THIS unless you know what you are doing as it will
mean that anyone with stored counter values is going to get a nasty shock.

********************************************************************************************/


BOOL CamProfile::ResetCounters()
{
	if (!Running)
	{
		return TRUE;
	}

	ERROR3IF(!Inited, "Profiling system not yet initialized");

	CamProfileMode OldMode=CAMPROFILE_NONE;

	// Save the old mode of the current operator so we do not credit
	// already elapsed time

	CamProfile * pActive = pCurrent;

	if (pActive)
	{
		OldMode = pActive->Mode;
		pActive->SetMode(CAMPROFILE_NONE);
	}	

	for (INT32 i=0; i<CAMPROFILE_NONE; i++)
	{
		OilTimeArray[i]=0;
	}

	if (pActive)
	{
		pActive->SetMode(OldMode);
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL CamProfile::ActivateProfiling(BOOL Run = TRUE)

	Author: 	Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if worked, FALSE if failed (out of memory)
	Purpose:	Starts up the profiling system
	Errors:		Returns FALSE on failure.
	Scope:		Static

This is the heart of the profiling system. We've just been told our timer has changed
from its current state, to myMode. Therefore, if we were profiling before, we attribute
the time passed since that timer started to the relevant array, and mark the old profiler
as being dormant. We then set our own time, and set our start position.

********************************************************************************************/


BOOL CamProfile::ActivateProfiling(BOOL Run)
{
	// Do nothing if we are already in that state
	if (Running == Run)
	{
		return TRUE;
	}

	ERROR3IF(!Inited, "Profiling system not yet initialized");

	if (Run)
	{
		// Ignore old linked list
		pHead = NULL;
		pTail = NULL;

		Running=TRUE;
		if (!pHead)
		{
			// Note we allocate this, and discard the pointer. That's because it has already
			// added itself to the linked list
			if (OurHead) delete(OurHead);
			OurHead = new CamProfile;	
			if (!OurHead) return FALSE;			
		}
		// We can't wake the zombies up as we don't have a list of them
		ResetCounters();
		return TRUE;
	}
	else
	{
		if (pCurrent)
		{
			pCurrent->SetMode(CAMPROFILE_NONE);
		}
		ResetCounters();

		// Now go through the list removing them all
		CamProfile * p = pTail;
		while (p)
		{
			CamProfile* pn = p->pPrev;
			p->Zombie = TRUE;
			p->pPrev = NULL;
			p->pNext = NULL;
			p = pn;
			// sadly they are not ours to delete
		}
		pHead = NULL;
		pTail = NULL;

		if (OurHead)
		{
			delete (OurHead);
			OurHead = NULL;
		}
		Running = FALSE;

	}
	return TRUE;
}

/********************************************************************************************

>	CamProfiler::CamProfiler()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CamProfiler::CamProfiler()
{
	Reset();
}

/********************************************************************************************

>	void CamProfiler::Reset()


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Resets counters to current
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void CamProfiler::Reset()
{
	CamProfile::UpdateOilTimes();
	memcpy(OilTimeArray, CamProfile::OilTimeArray, sizeof(OilTimeArray));
	FirstOilTime = CamProfile::LastOilTime;
}

/********************************************************************************************

>	double CamProfiler::Read(double Results[CAMPROFILE_NONE])


	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Inputs:		-
	Outputs:	Results[CAMPROFILE_NONE] - array of doubles filled with number of seconds elapsed
	Returns:	total elapsed time
	Purpose:	Reads counters
	Errors:		-
	SeeAlso:	-

Results may be NULL, in which case only the total elapsed time will be returned.

The results in the array should add up to the total returned, but may not if profiling
has been turned off in the mean time.

********************************************************************************************/

double CamProfiler::Read(double Results[CAMPROFILE_NONE])
{
	CamProfileMode Mode;
	CamProfile::UpdateOilTimes();

#if WIN32
	// This windows code completely untested by Alex
	const double factor = 1.0/10000000.0; // Windows uses 100 nanosecond timeslices apparently
#else
	const double factor = 1.0/1000000.0; // UNIX counts in microseconds
#endif

	INT64 check = 0;

	for (Mode = (CamProfileMode)0 ; Mode < CAMPROFILE_NONE ; Mode=(CamProfileMode)((INT32)Mode+1))
	{
		// Cast via INT64 to cope with wrap event (ha ha ha).
		if (Results)
			Results[Mode] = factor * (double)((INT64)(CamProfile::OilTimeArray[Mode] - OilTimeArray[Mode]));
		TRACE(_T("CAMPROFILER type %d=%lld\n"),(INT32)Mode,CamProfile::OilTimeArray[Mode] - OilTimeArray[Mode]);
		check+=CamProfile::OilTimeArray[Mode] - OilTimeArray[Mode];
	}

	INT64 total = (INT64)(CamProfile::LastOilTime - FirstOilTime);
	TRACE(_T("CAMPROFILER check=%lld, total=%lld\n"),check,total);

	return factor * (double)(total);
}
