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

#ifndef INC_CAMPROFILE
#define	INC_CAMPROFILE

//#include "ccobject.h" - in camtypes.h [AUTOMATICALLY REMOVED]

typedef enum
{
	CAMPROFILE_OTHER = 0,
	CAMPROFILE_GDRAW,
	CAMPROFILE_SHADOW,
	CAMPROFILE_BEVEL,
	CAMPROFILE_BLIT,

	CAMPROFILE_NONE				// Must be the last in the list
} CamProfileMode;

class CamProfiler;

/*******************************************************************************************

>	class CamProfile : public CCObject

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Purpose:	To time various bits of the Camelot subsytem
	Notes:		In the OIL
	See Also:	CamProfiler

This class allows a limited degree of built in profiling of Camelot operations.

Around something you want to time, construct a new CamProfile object

So for instance:

	void myslowfunc()
	{
	... do something ...
	CamProfile myCamProfile(CAMPROFILE_MYSLOWFUNC)l

	... do the thing you want timed ...

	// Object destroyed as it goes out of scope.
	}

Note that you do not need a specific destructor. We cope with nested profiles in the
obvious manner. When an exception is thrown, Camelot at various points cleans off the
list of nested profiles. This may make them inaccurate.

********************************************************************************************/


class CamProfile : public CCObject
{
	CC_DECLARE_DYNCREATE( CamProfile )

	friend class CamProfiler;

public:
	CamProfile(CamProfileMode myMode = CAMPROFILE_NONE );
	virtual ~CamProfile();

	void SetMode(CamProfileMode myMode, BOOL Base = FALSE);

	static BOOL Init();
	static BOOL ClearList();
	static BOOL ResetCounters();
	static BOOL ActivateProfiling(BOOL Run = TRUE);
	static void AtBase(CamProfileMode myMode=CAMPROFILE_NONE);
	static void UpdateOilTimes();
	static void GetTimeString(TCHAR * pTime, UINT32 length);

private:
	// Prev and next pointers - don't use list class for speed and as lists are one obvious
	// thing we might wish to profile
	CamProfile* pPrev; // The next one on the list
	CamProfile* pNext; // The previous one the list

	CamProfileMode Mode;

	static CamProfile* pHead; // The head of the list
	static CamProfile* pTail; // The tail of the list
	static CamProfile* pCurrent; // The current profiler - almost inevitably the tail unless it's not running

	static BOOL Running;	
	static BOOL Inited;

	static CamProfile * OurHead; // Base member we allocate and thus can free

	UINT64 OilTime;
	static UINT64 LastOilTime;

	BOOL Zombie;	// Set on objects created whilst profiling isn't running

	static UINT64 OilTimeArray[CAMPROFILE_NONE];

};

/*******************************************************************************************

>	class CamProfiler : public CCObject

	Author:		Alex_Bligh <alex@alex.org.uk>
	Created:	24/11/2005
	Purpose:	To report on time taken in various CamProfile sections
	Notes:		In the OIL
	See Also:	CamProfile

This class allows the user to maintain a set of timers that will be updated depending
on the time spent in Camelot in various CamProfile sections (see header for CamProfile).

To use:
	CamProfiler myProfile;

	... do some stuff ...

	double results[CAMPROFILE_NONE]
	myProfile->Read(results); // Read results in microseconds

All this class actually does is store the values of CamProfile::OilTimeArray array
when it's created, Read() then performs a subtraction to give the elapsed time
in each element.

********************************************************************************************/


class CamProfiler : public CCObject
{
	CC_DECLARE_DYNCREATE( CamProfiler )

public:
	CamProfiler();

	void Reset();
	double Read(double Results[CAMPROFILE_NONE]);

private:
	UINT64 OilTimeArray[CAMPROFILE_NONE];
	UINT64 FirstOilTime;
};


#endif

