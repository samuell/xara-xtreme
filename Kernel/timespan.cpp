// $Id: timespan.cpp 662 2006-03-14 21:31:49Z alex $
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
#include "timespan.h"

CC_IMPLEMENT_DYNAMIC(TimeSpan, CCObject)

// We want better memory tracking
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	TimeSpan::TimeSpan()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Purpose:	Constructs an empty TimeSpan object.

********************************************************************************************/

TimeSpan::TimeSpan()
{
	LO=HI=0;
}


/********************************************************************************************

>	TimeSpan::TimeSpan(TimeSlice lower, TimeSlice upper)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		lower = a lower bound time to inherit
				upper = an upper bound time to inherit
	Purpose:	Constructs an bounded TimeSpan object.

********************************************************************************************/

TimeSpan::TimeSpan(TimeSlice lower, TimeSlice upper)
{
	if (!SetBounds(lower,upper))
	{
		ERROR3("TimeSpan() constructor called with invalid bounds")
	}
}


/********************************************************************************************

>	TimeSpan::TimeSpan(const TimeSpan &other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		other = a reference to another time span
	Purpose:	Copy contructor for the TimeSpan object

********************************************************************************************/

TimeSpan::TimeSpan(const TimeSpan &other)
{
	(*this)=other;
}


/********************************************************************************************

>	BOOL TimeSpan::SetBounds(TimeSlice lower, TimeSlice upper)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		lower = a lower bound time to inherit
				upper = an upper bound time to inherit
	Returns:	TRUE if the bounds we valid, FALSE if not.
				(invalid bounds means lower>=upper indicating an empty span)
	Purpose:	Set the bounds of this time span object. The bounds are always set in
				the object even if they are invalid. 

********************************************************************************************/

BOOL TimeSpan::SetBounds(TimeSlice lower, TimeSlice upper)
{
	LO = lower;
	HI = upper;
	return (lower<upper);
}


/********************************************************************************************

>	inline TimeSlice TimeSpan::GetLowerBound() const
>	inline TimeSlice TimeSpan::Lo() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		-
	Returns:	The lower bound of this TimeSpan object
	Purpose:	Dig out the lower bound of this TimeSpan object
	Errors:		Generates an error3 if this object is empty

********************************************************************************************/

inline TimeSlice TimeSpan::GetLowerBound() const
{ 
	ERROR3IF(IsEmpty(), "TimeSpan::GetLowerBound() called on an empty TimeSpan");
	return LO;
}

inline TimeSlice TimeSpan::Lo() const
{ 
	ERROR3IF(IsEmpty(), "TimeSpan::Lo() called on an empty TimeSpan");
	return LO;
}

/********************************************************************************************

>	inline TimeSlice TimeSpan::GetUpperBound() const 
	inline TimeSlice TimeSpan::Hi() const 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		-
	Returns:	The upper bound of this TimeSpan object
	Purpose:	Dig out the upper bound of this TimeSpan object. It should always be
				greater than or equal to the lower bound, otherwise this span is invalid
	Errors:		Generates an error3 if this object is invalid

********************************************************************************************/

inline TimeSlice TimeSpan::GetUpperBound() const 
{
	ERROR3IF(IsEmpty(), "TimeSpan::GetUpperBound() called on an empty TimeSpan");
	return HI; 
}

inline TimeSlice TimeSpan::Hi() const 
{
	ERROR3IF(IsEmpty(), "TimeSpan::Hi() called on an empty TimeSpan");
	return HI; 
}


/********************************************************************************************

>	void TimeSpan::SetCreationTimeFrame(const DocView *pView, const Document *pDoc)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		pView = a view pointer
				pDoc = a document pointer
	Purpose:	Set the lower and upper time span bounds to the view's now and the
				documents max time.

********************************************************************************************/
/*
void TimeSpan::SetCreationTimeFrame(const DocView *pView, const Document *pDoc)
{
	ERROR3IF(pView==NULL,"NULL view pointer passed to SetCreationTimeFrame");
	ERROR3IF(pDoc ==NULL,"NULL doc pointer passed to SetCreationTimeFrame");
	
	LO = pView->GetCurrentTime();
	HI = pDoc->GetUpperTimeBound();
	valid=(LO<=HI);
	
	ERROR3IF(!valid, "TimeSpan::SetCreationTimeFrame() has failed to set a valid time frame!");
}
*/

/********************************************************************************************

>	void TimeSpan::MakeInvalid()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		-
	Purpose:	Make this time span invalid.

********************************************************************************************/

void TimeSpan::MakeInvalid()
{
	LO=HI;
}


/********************************************************************************************

>	BOOL TimeSpan::IsEmpty() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		-
	Returns:	TRUE if this time span object is empty, i.e. it contains a lower and
				upper time such that lower>=upper.
				FALSE if these conditions are not met.
	Purpose:	Check whether this time span object contains a non empty time frame. A time
				span becomes empty as a result of trying to perform boolean operations
				on time spans resulting in empty spans.

********************************************************************************************/

inline BOOL TimeSpan::IsEmpty() const
{
	return (LO>=HI);
}



/********************************************************************************************

>	BOOL TimeSpan::IsSameAs(const TimeSpan &other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		other = a reference to another TimeSpan object
	Returns:	TRUE if other contains the same time span as this and they are both none empty
				TRUE if other and this object are both empty
				FALSE if neither of these conditions hold
	Purpose:	Check whether two TimeSpan objects are the same.

********************************************************************************************/

BOOL TimeSpan::IsSameAs(const TimeSpan &other) const
{
	BOOL same = (IsEmpty() == other.IsEmpty());
	if (!IsEmpty() && same)
	{	
		same = same && (LO == other.GetLowerBound());
		same = same && (HI == other.GetUpperBound());
	}
	return same;
}


/********************************************************************************************

>	TimeSpan TimeSpan::Union(const TimeSpan &other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		other = a reference to another TimeSpan object
	Returns:	the union of the two time spans
	Purpose:	Find the union of this span with the other span. This union will always
				be valid if either or both spans are non empty

********************************************************************************************/

TimeSpan TimeSpan::Union(const TimeSpan &other) const
{
	return (*this)+other;
}

/********************************************************************************************

>	TimeSpan TimeSpan::Intersection(const TimeSpan &other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		other = a reference to another TimeSpan object
	Returns:	the intersection of the two time spans
	Purpose:	Find the intersection of this span with 'other' span. If the intersection
				does not exist, either because one of the spans is empty or both spans
				are valid but they don't intersect then an empty timespan object is
				returned. Otherwise the intersection is returned

********************************************************************************************/

TimeSpan TimeSpan::Intersection(const TimeSpan &other) const
{
	return (*this)-other;
}

/********************************************************************************************

>	BOOL TimeSpan::IsIntersectedWith(const TimeSpan &other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		other = a reference to another TimeSpan
	Returns:	TRUE if this time span does intersect with the 'other' time span
	Purpose:	Determine if this time span is intersected with the time span supplied
	SeeAlso:	TimeSpan::Intersection()

********************************************************************************************/

BOOL TimeSpan::IsIntersectedWith(const TimeSpan &other) const
{
	// both bounds must be valid for an intersection
	if (IsEmpty() || other.IsEmpty())
		return FALSE;

	// check for none intersection case
	TimeSlice lower = other.GetLowerBound();
	TimeSlice upper = other.GetUpperBound();
	return !(upper<LO || lower>HI);
}


/********************************************************************************************

>	BOOL TimeSpan::InBounds(TimeSlice current) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A time slice
	Returns:	TRUE then current is within the bounds of this time span
	Purpose:	Checks whether the given time slice is within the bounds of this span
				
********************************************************************************************/

BOOL TimeSpan::InBounds(TimeSlice current) const
{
	// Although Lo>Hi will return false always, lo==hi==current will return true
	// on an empty time span so check this.
	if (IsEmpty())
		return FALSE;

	return (LO<=current && current<=HI);
}

/********************************************************************************************

>	BOOL TimeSpan::Contains(const TimeSpan& other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to another timespan
	Returns:	TRUE then other is contained entirely within this time span
				FALSE if not. Other may still intersect this time span.
	Purpose:	Check wether the time span expressed by 'other' is contained entirely within
				this time span
				
********************************************************************************************/

BOOL TimeSpan::Contains(const TimeSpan& other) const
{
	// A.Contains(Empty(B))?  Empty(A).Contains(B)?
	if (IsEmpty() || other.IsEmpty())
		return FALSE;

	return (LO<=other.GetLowerBound() && HI>=other.GetUpperBound());
}


/********************************************************************************************

>	void TimeSpan::Inflate(TimeSlice inflate)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to the inflation value
	Returns:	-
	Purpose:	Inflates the bounds of this object by 'inflate'
				
********************************************************************************************/

void TimeSpan::Inflate(TimeSlice inflate)
{
	ERROR3IF(IsEmpty(),"TimeSpan::Inflate() called on an empty span");
	LO-=inflate;
	HI+=inflate;
}

/********************************************************************************************

>	double TimeSpan::GetRelativeTime(TimeSlice current) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A current time to make relative
	Returns:	A relative time
	Purpose:	Convert the absolute time 'current' into a relative value. If the current
				value is within this time span then the double result will lie
				between 0 and 1, otherwise it will lie somewhere on the real line.
				
********************************************************************************************/

inline double TimeSpan::GetRelativeTime(TimeSlice current) const
{
	ERROR3IF(IsEmpty(),"TimeSpan::GetRelativeTime() called on an empty span");
	return ((double)(LO-current)) / ((double)(HI-LO));
}

/********************************************************************************************

>	inline TimeSlice TimeSpan::GetAbsoluteTime(double current) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A relative time to make absolute
	Returns:	An absolute time
	Purpose:	Convert the relative time 'current' into an absolute value. Performs
				a linear interpolation on this timespans start and end times. Hence if
				0<=current<=1 then the returned time will be somewhere within this time
				span.
				
********************************************************************************************/

inline TimeSlice TimeSpan::GetAbsoluteTime(double current) const
{
	ERROR3IF(IsEmpty(),"TimeSpan::GetAbsoluteTime() called on an empty span");
	return (LO + (TimeSlice)(0.5+current*((double)(HI-LO))));
}



/********************************************************************************************

>	TimeSpan TimeSpan::operator+(TimeSlice value) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to the TimeSlice value to add to both lower and upper bounds
	Returns:	The new shifted time span
	Purpose:	Returns TimeSpanA = TimeSpanB + TimeSlice
				
********************************************************************************************/

TimeSpan TimeSpan::operator+(TimeSlice value) const
{
	return TimeSpan(LO+value, HI+value);
}


/********************************************************************************************

>	TimeSpan TimeSpan::operator+(const TimeSpan& other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to a rhs TimeSpan value to add to this time span
	Returns:	A new time span holding the result of the addition ( a union )
	Purpose:	Overload operator+ to perform a union opertion.
				
********************************************************************************************/

TimeSpan TimeSpan::operator+(const TimeSpan& other) const
{
	// is this empty?
	if (IsEmpty())
		return other;

	if (other.IsEmpty())
		return (*this);

	// make a union out of the bounds
	TimeSlice lower = other.GetLowerBound();
	TimeSlice upper = other.GetUpperBound();
	if (LO<lower) lower=LO;
	if (HI>upper) upper=HI;
	
	return TimeSpan(lower,upper);
}





/********************************************************************************************

>	TimeSpan TimeSpan::operator-(TimeSlice value) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to the TimeSlice value to add to both lower and upper bounds
	Returns:	A resulting time span
	Purpose:	Subtracts 'value' from the bounds of this timespan and returns the result
				as a new time span.
				
********************************************************************************************/

TimeSpan TimeSpan::operator-(TimeSlice value) const
{
	return TimeSpan(LO-value, HI-value);
}


/********************************************************************************************

>	TimeSpan TimeSpan::operator-(const TimeSpan &other) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to a rhs TimeSpan value to subtract from this time span
	Returns:	The result of subtracting other from this
	Purpose:	Overload the operator- function to perform an intersection of two time spans
				returning the result in a new timespan.
				
********************************************************************************************/

TimeSpan TimeSpan::operator-(const TimeSpan &other) const
{
	// both bounds must be valid for an intersection
	if (IsEmpty() || other.IsEmpty())
		return (*this);

	// check for none intersection case
	TimeSlice lower = other.GetLowerBound();
	TimeSlice upper = other.GetUpperBound();
	if (upper<LO || lower>HI)
		return (*this);

	// make an intersection now we know the two actually do intersect
	if (HI<upper) upper=HI;
	if (LO>lower) lower=LO;
	
	return TimeSpan(lower,upper);
}



/********************************************************************************************

>	TimeSpan& TimeSpan::operator=(const TimeSpan &other)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/97
	Inputs:		A reference to another time span
	Returns:	A reference to this time span
	Purpose:	Assignment operator overload, assigns this time span with that of
				'other' and returns a reference for further assignment
				
********************************************************************************************/

TimeSpan& TimeSpan::operator=(const TimeSpan &other)
{
	LO = other.GetLowerBound();
	HI = other.GetUpperBound();
	return (*this);
}
