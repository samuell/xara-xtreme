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
// Implementation of ValueFunction classes (used in stroke providing)

#include "camtypes.h"

#include <math.h>
#include "valfunc.h"

#include "cxftags.h"
//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fixmem.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]


CC_IMPLEMENT_DYNAMIC(ValueFunction,						ListItem)
	CC_IMPLEMENT_DYNAMIC(ValueFunctionConstant,			ValueFunction)
	CC_IMPLEMENT_DYNAMIC(ValueFunctionRandom,			ValueFunction)
	CC_IMPLEMENT_DYNAMIC(ValueFunctionRamp,				ValueFunction)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionRampLinear,	ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionRampS,		ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionRampS2,		ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionRampL,		ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionRampL2,		ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionDoubleRampS,	ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSawTooth,		ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionPropeller,	ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionIntestine,	ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionDecay,		ValueFunctionRamp)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionBevelEnds,	ValueFunctionRamp)
	CC_IMPLEMENT_DYNAMIC(ValueFunctionBlip,				ValueFunction)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionTeardrop,		ValueFunctionBlip)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionTeardropCurvedEnd, ValueFunctionBlip)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionEllipse,		ValueFunctionBlip)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionThumbtack,	ValueFunctionBlip)
	CC_IMPLEMENT_DYNAMIC(ValueFunctionPressure,			ValueFunction)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionPressureS,	ValueFunctionPressure)
	CC_IMPLEMENT_DYNAMIC(ValueFunctionSmoothStroke,		ValueFunction)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Yacht,		ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Iron,		ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Reed,		ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Meteor,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Petal,		ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Comet,		ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Fallout,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Torpedo,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Missile,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Convex,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Concave,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_SlimBlip,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Cigar,		ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Cigar2,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Cigar3,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_OceanLiner,ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Goldfish,	ValueFunctionSmoothStroke)
		CC_IMPLEMENT_DYNAMIC(ValueFunctionSS_Barb,		ValueFunctionSmoothStroke)


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


const double Pi = 3.141592654;

bool ValueFunctionSS_Yacht::bMadeTable		= false;
bool ValueFunctionSS_Iron::bMadeTable		= false;
bool ValueFunctionSS_Reed::bMadeTable		= false;
bool ValueFunctionSS_Meteor::bMadeTable		= false;
bool ValueFunctionSS_Petal::bMadeTable		= false;
bool ValueFunctionSS_Comet::bMadeTable		= false;
bool ValueFunctionSS_Fallout::bMadeTable	= false;
bool ValueFunctionSS_Torpedo::bMadeTable	= false;
bool ValueFunctionSS_Missile::bMadeTable	= false;
bool ValueFunctionSS_Convex::bMadeTable		= false;
bool ValueFunctionSS_Concave::bMadeTable	= false;
bool ValueFunctionSS_SlimBlip::bMadeTable 	= false;
bool ValueFunctionSS_Cigar::bMadeTable 		= false;
bool ValueFunctionSS_Cigar2::bMadeTable 	= false;
bool ValueFunctionSS_Cigar3::bMadeTable 	= false;
bool ValueFunctionSS_OceanLiner::bMadeTable	= false;
bool ValueFunctionSS_Goldfish::bMadeTable	= false;
bool ValueFunctionSS_Barb::bMadeTable		= false;

//Used by the Smooth-Stroke Functions to retain bezier data...
double ValueFunctionSmoothStroke::xvalue[18][256];
double ValueFunctionSmoothStroke::yvalue[18][256];



///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunction base class
///////////////////////////////////////////////////////////////////////////////////////////


List ValueFunction::RegisteredFunctions;


/******************************************************************************************

>	static BOOL ValueFunction::Init(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Returns:	TRUE if it initialises successfully

	Purpose:	Initialises all valueFunction classes defined in valfunc.cpp

				Adds an instance of each ValueFunction to a dynamic list, which is
				used to create new ValueFunctions as they are loaded from files.
				Any dynamically added ValueFunction-derived class should add an
				instance to the RegistreredFunctions list in order to properly
				handle being re-loaded.

******************************************************************************************/

BOOL ValueFunction::Init(void)
{
#define VFINIT(TYPE)								\
	{												\
		TYPE *pFunc = new TYPE;						\
		if (!pFunc)									\
			return(FALSE);							\
		RegisteredFunctions.AddTail(pFunc);			\
	}

	VFINIT(ValueFunctionConstant);
	VFINIT(ValueFunctionRandom);
	VFINIT(ValueFunctionRampLinear);
	VFINIT(ValueFunctionRampS);
	VFINIT(ValueFunctionPressure);
	VFINIT(ValueFunctionPressureS);
	VFINIT(ValueFunctionBlip);
	VFINIT(ValueFunctionTeardrop);
	VFINIT(ValueFunctionEllipse);
	VFINIT(ValueFunctionThumbtack);
	VFINIT(ValueFunctionRampL);
	VFINIT(ValueFunctionRampS2);
	VFINIT(ValueFunctionRampL2);
	VFINIT(ValueFunctionTeardropCurvedEnd);
	VFINIT(ValueFunctionSawTooth);
	VFINIT(ValueFunctionPropeller);
	VFINIT(ValueFunctionDoubleRampS);
	VFINIT(ValueFunctionIntestine);
	VFINIT(ValueFunctionDecay);
	VFINIT(ValueFunctionBevelEnds);
	VFINIT(ValueFunctionSS_Reed);
	VFINIT(ValueFunctionSS_Meteor);
	VFINIT(ValueFunctionSS_Petal);
	VFINIT(ValueFunctionSS_Comet);
	VFINIT(ValueFunctionSS_Barb);
	VFINIT(ValueFunctionSS_Concave);
	VFINIT(ValueFunctionSS_Convex);
	VFINIT(ValueFunctionSS_Iron);
	VFINIT(ValueFunctionSS_Torpedo);
	VFINIT(ValueFunctionSS_Missile);
	VFINIT(ValueFunctionSS_Goldfish);
	VFINIT(ValueFunctionSS_OceanLiner);
	VFINIT(ValueFunctionSS_Yacht);
	VFINIT(ValueFunctionSS_SlimBlip);
	VFINIT(ValueFunctionSS_Cigar);
	VFINIT(ValueFunctionSS_Cigar2);
	VFINIT(ValueFunctionSS_Cigar3);
	VFINIT(ValueFunctionSS_Fallout);

#undef VFINIT

	return(TRUE);
}



/******************************************************************************************

>	static void ValueFunction::DeInit(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Returns:	TRUE if it initialises successfully

	Purpose:	Initialises all valueFunction classes defined in valfunc.cpp

******************************************************************************************/

void ValueFunction::DeInit(void)
{
	RegisteredFunctions.DeleteAll();
}



/******************************************************************************************

>	virtual BOOL ValueFunction::IsDifferent(ValueFunction *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE if the objects are considered different,
				FALSE if they are considered identical

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

	Notes:		The base class simply checks if they are of the same runtime class
				and returns TRUE if they are not. Derived classes which can differ
				in content between instances must override this method.

******************************************************************************************/

BOOL ValueFunction::IsDifferent(ValueFunction *pOther)
{
	ERROR3IF(pOther == NULL, "Illegal NULL param");
	return(GetRuntimeClass() != pOther->GetRuntimeClass());
}



/******************************************************************************************

>	static ValueFunction *ValueFunction::ReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		pInputRecord - The file record to read from

	Returns:	NULL if it fails, else a pointer to a new object initialised from
				the given file record.

	Purpose:	Main entry point for loading ValueFunctions from files. The caller
				record handler passes the record in to this static function, and
				the saved VF is re-constituted from the saved information. On return
				the caller can read any extra values that they saved in the record.

	Notes:		This scans the member RegisteredFunction list to see if it can find
				a ValueFunction of the appropriate type, and asks it to create a new
				ValueFunction to be initialised from the given record (by calling
				the virtual CreateAndReadFileRecord())

******************************************************************************************/

ValueFunction *ValueFunction::ReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL params");

	INT32 ID;
	pInputRecord->ReadINT32(&ID);

	ValueFunction *pFunc = (ValueFunction *) RegisteredFunctions.GetHead();
	while (pFunc != NULL)
	{
		if (pFunc->GetUniqueID() == ID)
			return(pFunc->CreateAndReadFileRecord(pInputRecord));

		pFunc = (ValueFunction *) RegisteredFunctions.GetNext(pFunc);
	}

	return(NULL);
}



/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunction::CreateAndWriteFileRecord(INT32 RecordTag,
														INT32 DerivedBytes, INT32 ExtraBytes,
														BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		RecordTag	- the tag to write this record under

				DerivedBytes- The number of bytes of information the derived VF
							  class wishes to write to save its state. Space
							  for this infomation will be reserved in the new record

				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.

				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

				This base-class function only writes the ValueFunction header for
				the record data. The caller must append its own state information
				to this header - see the record description below.

	Purpose:	Creates an output record containing an embedded ValueFunction description.
	
				The record will be (4 + DerivedBytes + ExtraBytes) bytes in length, and
				on return from this function, the first 4 bytes (only) will have been
				written. The caller(s) must fill in the remainder.

				All ValueFunction record data has 3 sections, whiich are recorded as follows:
				1. ValueFunction header, identifying which type of VF is being saved.
				   This is written by the base class CreateAndWriteFileRecord
						INT32 ValueFunctionUniqueID		(4 bytes)

				2. Derived-class-data (0 or more bytes). This is written by the derived
				   class WriteFileRecord function, into the record returned from here.
				   This data must be DerivedBytes (0 or more) bytes in length.

				3. Caller data. This is written by the caller of the derived function.
				   This data must be ExtraBytes (0 or more) bytes in length.

	SeeAlso:	ValueFunctionConstant::CreateAndReadFileRecord

******************************************************************************************/

CamelotFileRecord *ValueFunction::CreateAndWriteFileRecord(INT32 RecordTag, 
															INT32 DerivedBytes, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::CreateAndWriteFileRecord");

	// Calculate how many bytes of information this VF will write.
	// We write a 4-byte "header", followed by the derived class data and the caller data
	const INT32 RecordSize = 4 + DerivedBytes + ExtraBytes;

	// Create an appropriate record, and write the header info to it
	CamelotFileRecord *pRec = new CamelotFileRecord(pFilter, RecordTag, RecordSize);

	// If that worked, then we now init & add our own data to the record
	if (pRec != NULL)
	{
		BOOL ok	   = pRec->Init();
		if (ok) ok = pRec->WriteUINT32(GetUniqueID());

		if (!ok)		// If we failed, then clean up & return NULL
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}





///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionConstant class
///////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************

>	ValueFunctionConstant::ValueFunctionConstant(double Value)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Value	- The value to be used at all positions

	Purpose:	Constructor
				The Constant ValueFunction always returns the same value!

******************************************************************************************/

ValueFunctionConstant::ValueFunctionConstant(double Value)
{
	Value1 = Value;
}



/******************************************************************************************

>	virtual double ValueFunctionConstant::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A constant value (Value, as given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		Constant is a bit simple - it always returns the same value

******************************************************************************************/

double ValueFunctionConstant::GetValue(double Position)
{
	return(Value1);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionConstant::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionConstant::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionConstant(Value1);
	return(pClone);
}



/******************************************************************************************

>	virtual BOOL ValueFunctionConstant::IsDifferent(ValueFunction *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE if the objects are considered different,
				FALSE if they are considered identical

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

	Notes:		Calls the base class to see if they are different classes,
				and then compares identical classes by checking member vars

******************************************************************************************/

BOOL ValueFunctionConstant::IsDifferent(ValueFunction *pOther)
{
	if (ValueFunction::IsDifferent(pOther))
		return(TRUE);
	
	// Both objects are instances of this class, so compare them more carefully
	return(Value1 != ((ValueFunctionConstant *)pOther)->Value1);
}



/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunctionConstant::WriteFileRecord(INT32 RecordTag,
											INT32 ExtraBytes, BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		RecordTag	- the tag to write this record under
				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.
				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

	Purpose:	Saves a ValueFunction object to a Xara file.
				This function will create a new variable-sized record with the given
				record tag, and will write out whatever data is needed to save this
				ValueFunction's state to the file.
				
				"ExtraBytes" bytes will be added to the record size to reserve space
				at the end of the record for the caller to add their own data. This
				is to allow ValueFunctions to be saved embedded in other object's
				record structures (e.g. inside different types of attributes).

				All ValueFunction record data has 3 sections, whiich are recorded as follows:
				1. ValueFunction header, identifying which type of VF is being saved
						INT32 ValueFunctionUniqueID		(4 bytes)
				2. Derived-class-data (0 or more bytes). This particular class adds:
						float ConstantValue				(4 bytes)
				3. Caller data. This is written by the caller to the returned record
				   object. This data must be ExtraBytes (0 or more) bytes in length.

	SeeAlso:	ValueFunctionConstant::CreateAndReadFileRecord

******************************************************************************************/

CamelotFileRecord *ValueFunctionConstant::WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::WriteFileRecord");

	// Calculate how many bytes of information this VF will write. We do not include
	// the header info written by the base class or the ExtraInfo desired by the caller -
	// the base class adds all that in for us.
	const INT32 MyRecordSize = 4;

	// Create an appropriate record, and write our data to it
	CamelotFileRecord *pRec = CreateAndWriteFileRecord(RecordTag, MyRecordSize, ExtraBytes, pFilter);

	if (pRec != NULL)
	{
		// Write out our ValueFunction's specific data. If it fails, then we'll return NULL
		if (!pRec->WriteFLOAT((float)Value1))
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionConstant::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionConstant::WriteFileRecord;
				ValueFunctionConstant::ReadFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionConstant::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float ConstValue = (float)1.0;
	pInputRecord->ReadFLOAT(&ConstValue);

	return(new ValueFunctionConstant((double) ConstValue));
}









///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionRandom class
///////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************

>	ValueFunctionRandom::ValueFunctionRandom(UINT32 Seed, double Min, double Max)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Seed	- The base random-number seed. This is combined with the
						  position value to generate a "random" number that will
						  not change for any given position/seed combination.

				Min		- The minimum value you want returned
				Max		- The maximum value you want returned

	Purpose:	Constructor
				The Random ValueFunction returns a pseudo-random value between Min & Max

******************************************************************************************/

ValueFunctionRandom::ValueFunctionRandom(UINT32 Seed, double Min, double Max)
{
	ERROR3IF(Min > Max, "ValueFunctionRandom: Min should be smaller than Max");

	MinValue = Min;
	MaxValue = Max;
	SeedValue = Seed;

	// Fill in the array with random values
	srand(SeedValue);
	for (INT32 i = 0; i < NumRandomValues; i++)
		Array[i] = rand() & 0xffff;
}



/******************************************************************************************

>	virtual double ValueFunctionRandom::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A "Random" value (based on the Seed, Min, and Max values given to the
				constructor) representing the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		Random returns a pseudo-random value based on the position and the
				Seed value, and clamped within the range given to the constructor.
				If the Seed/Min/Max values are unchanged, the same position will
				always return the same value.

******************************************************************************************/

double ValueFunctionRandom::GetValue(double Position)
{
	// Determine which array entry to use, and how much to linearly interpolate
	// between it and the next value
	INT32 Index = (INT32) floor(Position * (double)(0x400 * (NumRandomValues - 2)));
	double MixFraction = (double)(Index & 0x3ff) / (double)0x3ff;

	// Scale the Index value down into the proper range now, and make sure it's safe
	Index /= 0x400;
	if (Index < 0)
		Index = 0;
	if (Index > NumRandomValues - 2)
		Index = NumRandomValues - 2;

	// Get a random number between 0.0 and 1.0
	double Value = ((double)Array[Index] * (1.0 - MixFraction)) + ((double)Array[Index+1] * MixFraction);
	Value /= (double) 0xffff;

	// And scale the result into the range specified by the caller
	return( MinValue + (Value * (MaxValue - MinValue)) );
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRandom::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionRandom::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionRandom(SeedValue, MinValue, MaxValue);
	return(pClone);
}



/******************************************************************************************

>	virtual BOOL ValueFunctionRandom::IsDifferent(ValueFunction *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE, always

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

******************************************************************************************/

BOOL ValueFunctionRandom::IsDifferent(ValueFunction *pOther)
{
	return(TRUE);		// Random value functions always consider themselves different
}



/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunctionRandom::WriteFileRecord(INT32 RecordTag,
											INT32 ExtraBytes, BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		RecordTag	- the tag to write this record under
				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.
				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

	Purpose:	Saves a ValueFunction object to a Xara file.
				This function will create a new variable-sized record with the given
				record tag, and will write out whatever data is needed to save this
				ValueFunction's state to the file.
				
				"ExtraBytes" bytes will be added to the record size to reserve space
				at the end of the record for the caller to add their own data. This
				is to allow ValueFunctions to be saved embedded in other object's
				record structures (e.g. inside different types of attributes).

				All ValueFunction record data has 3 sections, whiich are recorded as follows:
				1. ValueFunction header, identifying which type of VF is being saved
						INT32 ValueFunctionUniqueID		(4 bytes)
				2. Derived-class-data (0 or more bytes). This particular class adds:
						UINT32 SeedValue
						float MinValue
						float MaxValue					(12 bytes)
				3. Caller data. This is written by the caller to the returned record
				   object. This data must be ExtraBytes (0 or more) bytes in length.

	SeeAlso:	ValueFunctionRandom::CreateAndReadFileRecord

******************************************************************************************/

CamelotFileRecord *ValueFunctionRandom::WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::WriteFileRecord");

	// Calculate how many bytes of information this VF will write. We do not include
	// the header info written by the base class or the ExtraInfo desired by the caller -
	// the base class adds all that in for us.
	const INT32 MyRecordSize = 12;

	// Create an appropriate record, and write our data to it
	CamelotFileRecord *pRec = CreateAndWriteFileRecord(RecordTag, MyRecordSize, ExtraBytes, pFilter);

	if (pRec != NULL)
	{
		// Write out our ValueFunction's specific data. If it fails, then we'll return NULL
		BOOL ok = TRUE;
		if (ok)		ok = pRec->WriteUINT32((UINT32)SeedValue);
		if (ok)		ok = pRec->WriteFLOAT((float)MinValue);
		if (ok)		ok = pRec->WriteFLOAT((float)MaxValue);

		if (!ok)
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRandom::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRandom::WriteFileRecord;
				ValueFunctionRandom::ReadFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionRandom::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	UINT32 SeedValue = 1;
	float MinValue = (float) 0.0;
	float MaxValue = (float) 0.0;

	pInputRecord->ReadUINT32(&SeedValue);
	pInputRecord->ReadFLOAT(&MinValue);
	pInputRecord->ReadFLOAT(&MaxValue);

	return(new ValueFunctionRandom((UINT32) SeedValue, (double) MinValue, (double) MinValue));
}







///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionRamp classes
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	ValueFunctionRamp::ValueFunctionRamp(double StartValue, double EndValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		StartValue	- The value to be used at the 0.0 position
				EndValue	- The value to be used at the 1.0 position

	Purpose:	Constructor
				Ramp functions interpolate between two values in some fashion
				Derived classes provide such things as linear and sine-wave interpolations

******************************************************************************************/

ValueFunctionRamp::ValueFunctionRamp(double StartValue, double EndValue)
{
	Value1 = StartValue;
	Value2 = EndValue;
}



/******************************************************************************************

>	virtual BOOL ValueFunctionRamp::IsDifferent(ValueFunction *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE if the objects are considered different,
				FALSE if they are considered identical

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

	Notes:		Calls the base class to see if they are different classes,
				and then compares identical classes by checking member vars

******************************************************************************************/

BOOL ValueFunctionRamp::IsDifferent(ValueFunction *pOther)
{
	if (ValueFunction::IsDifferent(pOther))
		return(TRUE);
	
	// Both objects are instances of this class, so compare them more carefully
	return( (Value1 != ((ValueFunctionRamp *)pOther)->Value1) ||
			(Value2 != ((ValueFunctionRamp *)pOther)->Value2) );
}



/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunctionRamp::WriteFileRecord(INT32 RecordTag,
											INT32 ExtraBytes, BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		RecordTag	- the tag to write this record under
				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.
				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

	Purpose:	Saves a ValueFunction object to a Xara file.
				This function will create a new variable-sized record with the given
				record tag, and will write out whatever data is needed to save this
				ValueFunction's state to the file.
				
				"ExtraBytes" bytes will be added to the record size to reserve space
				at the end of the record for the caller to add their own data. This
				is to allow ValueFunctions to be saved embedded in other object's
				record structures (e.g. inside different types of attributes).

				All ValueFunction record data has 3 sections, whiich are recorded as follows:
				1. ValueFunction header, identifying which type of VF is being saved
						INT32 ValueFunctionUniqueID		(4 bytes)
				2. Derived-class-data (0 or more bytes). This particular class adds:
						float Value1
						float Value2					(8 bytes)
				3. Caller data. This is written by the caller to the returned record
				   object. This data must be ExtraBytes (0 or more) bytes in length.

******************************************************************************************/

CamelotFileRecord *ValueFunctionRamp::WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::WriteFileRecord");

	// Calculate how many bytes of information this VF will write. We do not include
	// the header info written by the base class or the ExtraInfo desired by the caller -
	// the base class adds all that in for us.
	const INT32 MyRecordSize = 8;

	// Create an appropriate record, and write our data to it
	CamelotFileRecord *pRec = CreateAndWriteFileRecord(RecordTag, MyRecordSize, ExtraBytes, pFilter);

	if (pRec != NULL)
	{
		BOOL ok = TRUE;
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value1);
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value2);

		if (!ok)			// If we failed, clean up & return NULL
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}






/******************************************************************************************

>	virtual double ValueFunctionRampLinear::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		RampLinear is a linear interpolation between the start & end values

******************************************************************************************/

double ValueFunctionRampLinear::GetValue(double Position)
{
	return(((1.0 - Position) * Value1) + (Position * Value2));
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampLinear::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionRampLinear::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionRampLinear(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampLinear::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionRampLinear::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionRampLinear((double) Value1, (double) Value2));
}





/******************************************************************************************

>	virtual double ValueFunctionRampS::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		RampS is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionRampS::GetValue(double Position)
{
	const double MixValue	= (cos(Position * Pi) + 1.0) / 2.0;

	return((MixValue * Value1) + ((1.0 - MixValue) * Value2));
}


/******************************************************************************************

>	virtual INT32 ValueFunctionRampS::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionRampS::GetMinimumRecursionDepth()
{
	return 2;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampS::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionRampS::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionRampS(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampS::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionRampS::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionRampS((double) Value1, (double) Value2));
}



/******************************************************************************************

>	virtual double ValueFunctionRampS2::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		RampS2 is a half sine-wave "S" shaped function - Value2 set to none-zero

******************************************************************************************/

double ValueFunctionRampS2::GetValue(double Position)
{
	const double MixValue	= (cos(Position * Pi) + 1.0) / 2.0;

	return((MixValue * Value1) + ((1.0 - MixValue) * Value2));
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampS2::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionRampS2::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionRampS2(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampS2::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionRampS2::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.2;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionRampS2((double) Value1, (double) Value2));
}

/******************************************************************************************

>	virtual INT32 ValueFunctionRampS2::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionRampS2::GetMinimumRecursionDepth()
{
	return 2;
}





/******************************************************************************************

>	virtual double ValueFunctionRampL::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		RampL is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionRampL::GetValue(double Position)
{
	const double MixValue	= cos((Pi/2.0) + (Position * Pi/2.0)) + 1.0;

	return((MixValue * Value1) + ((1.0 - MixValue) * Value2));
}


/******************************************************************************************

>	virtual INT32 ValueFunctionRampL::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionRampL::GetMinimumRecursionDepth()
{
	return 2;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampL::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionRampL::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionRampL(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampL::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/2/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionRampL::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionRampL((double) Value1, (double) Value2));
}


/******************************************************************************************

>	virtual double ValueFunctionRampL2::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		RampL is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionRampL2::GetValue(double Position)
{
	const double MixValue	= cos((Pi/2.0) + (Position * Pi/2.0)) + 1.0;

	return((MixValue * Value1) + ((1.0 - MixValue) * Value2));
}


/******************************************************************************************

>	virtual INT32 ValueFunctionRampL2::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionRampL2::GetMinimumRecursionDepth()
{
	return 2;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampL::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionRampL2::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionRampL2(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionRampL::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionRampL2::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.2;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionRampL2((double) Value1, (double) Value2));
}



/******************************************************************************************

>	virtual double ValueFunctionSawTooth::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		SawTooth is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionSawTooth::GetValue(double Position)
{
	if (Position == 1.0)
	{
		return 0.0;
	}
	else
	{
		double Value	= (9.0 * Position) - (floor(9.0 * Position));
		return(1.0 - (Value * Value));
	}
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionSawTooth::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionSawTooth::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionSawTooth(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual INT32 ValueFunctionSawTooth::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionSawTooth::GetMinimumRecursionDepth()
{
	return 4;
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionSawTooth::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionSawTooth::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionSawTooth((double) Value1, (double) Value2));
}



/******************************************************************************************

>	virtual double ValueFunctionCurvedSawTooth::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		CurvedSawTooth is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionPropeller::GetValue(double Position)
{
	double Value;

	if (Position < 0.15)
	{
		//Draw a curved beginning bit...
		Position = 1.0 - Position/0.15;
		if ( Position>1.0 )
			Position = 1.0;
		Value = sqrt(1-Position*Position);
	}
	else
	{
		if (Position > 0.85)
		{
			//Draw a curved end bit...
			Position = (Position - 0.85) / 0.15;
			if ( Position>1.0 )
				Position = 1.0;
			Value = sqrt(1-Position*Position);
		}
		else
		{
			//Scale Position to be a value between 0 and 1 based on how far through between
			//0.15 and 0.85 it is...
			Position = (Position - 0.15) / 0.7;
			Value = (cos(Position * 2.0 * Pi) + 1.5) * 0.4;
		}
	}
	return Value;
}


/******************************************************************************************

>	virtual INT32 ValueFunctionPropeller::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionPropeller::GetMinimumRecursionDepth()
{
	return 4;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionPropeller::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionPropeller::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionPropeller(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionPropeller::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionPropeller::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionPropeller((double) Value1, (double) Value2));
}

/******************************************************************************************

>	virtual double ValueFunctionDoubleRampS::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		DoubleRampS is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionDoubleRampS::GetValue(double Position)
{
	const double MixValue	= (cos(Position * 2.0 * Pi) + 1.5) * 0.4;

	return((MixValue * Value1) + ((1.0 - MixValue) * Value2));
}


/******************************************************************************************

>	virtual INT32 ValueFunctionDoubleRampS::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionDoubleRampS::GetMinimumRecursionDepth()
{
	return 2;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionDoubleRampS::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionDoubleRampS::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionDoubleRampS(Value1, Value2);
	return(pClone);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionDoubleRampS::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionDoubleRampS::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionDoubleRampS((double) Value1, (double) Value2));
}

/******************************************************************************************

>	virtual double ValueFunctionIntestine::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionIntestine::GetValue(double Position)
{
	const double MixValue	= ((cos(Position * 20.0 * Pi) + 3.0) / 4.0);
	return((MixValue * Value1) + ((1.0 - MixValue) * Value2));
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionIntestine::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionIntestine::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionIntestine(Value1, Value2);
	return(pClone);
}


/******************************************************************************************

>	virtual INT32 ValueFunctionIntestine::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionIntestine::GetMinimumRecursionDepth()
{
	return 4;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionIntestine::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionIntestine::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionIntestine((double) Value1, (double) Value2));
}

/******************************************************************************************

>	virtual double ValueFunctionDecay::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionDecay::GetValue(double Position)
{
	const double MixValue	= (1.0 - Position) * ((cos(Position * 20.0 * Pi) + 3.0) / 4.0);
	return MixValue;
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionDecay::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionDecay::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionDecay(Value1, Value2);
	return(pClone);
}

/******************************************************************************************

>	virtual INT32 ValueFunctionDecay::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/


INT32 ValueFunctionDecay::GetMinimumRecursionDepth()
{
	return 4;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionDecay::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionDecay::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionDecay((double) Value1, (double) Value2));
}



/******************************************************************************************

>	virtual double ValueFunctionBevelEnds::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		Blobby is a half sine-wave "S" shaped function

******************************************************************************************/

double ValueFunctionBevelEnds::GetValue(double Position)
{
	double Value;

	if (Position == 1.0)
	{
		Value = 0.0;
	}
	else
	{
		if (Position < 0.15)
		{
			Position = Position / 0.15;
			Value = Position;
		}
		else
		{
			if (Position > 0.85)
			{
				Position = (Position - 0.85) / 0.15;
				Value = 1.0 - Position;
			}
			else
			{
				Value = 1.0;
			}
		}
	}

	return Value;
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionBevelEnds::Clone(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionBevelEnds::Clone(void)
{
	ValueFunction *pClone = new ValueFunctionBevelEnds(Value1, Value2);
	return(pClone);
}


/******************************************************************************************

>	virtual INT32 ValueFunctionBevelEnds::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionBevelEnds::GetMinimumRecursionDepth()
{
	return 4;
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionBevelEnds::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionRamp::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionBevelEnds::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float Value1 = (float) 1.0;
	float Value2 = (float) 0.0;

	pInputRecord->ReadFLOAT(&Value1);
	pInputRecord->ReadFLOAT(&Value2);

	return(new ValueFunctionBevelEnds((double) Value1, (double) Value2));
}





///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionBlip and derived classes
///////////////////////////////////////////////////////////////////////////////////////////

/******************************************************************************************

>	ValueFunctionBlip::ValueFunctionBlip(double Value)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		MaxPosition	- The position at which the function should reach
							  its maximum value. Normally this is at 0.5, but
							  for asymmetrical blips, values like 0.2 are better
	Purpose:	Constructor

******************************************************************************************/

ValueFunctionBlip::ValueFunctionBlip(double MaxPosition)
{
	ERROR3IF(MaxPosition <= 0.0 || MaxPosition >= 1.0, "Silly MaxPosition value");
	MaxPos = MaxPosition;
}



/******************************************************************************************

>	virtual double ValueFunctionBlip::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		This Blip is a 360 degree sine wave from 0.0 to 1.0 and back again.
				However, because the midpoint is adjustable, it is a piecewise function

******************************************************************************************/

double ValueFunctionBlip::GetValue(double Position)
{
	double Value;
	if (Position < MaxPos)
	{
		Position /= MaxPos;
		Value = sin(Position * (Pi / 2.0));
	}
	else
	{
		Position = (Position - MaxPos) / (1.0 - MaxPos);
		Value = cos(Position * (Pi / 2.0));
	}

	return(Value);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionBlip::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionBlip::Clone(void)
{
	ValueFunctionBlip *pClone = CreateInstance();

	if (pClone != NULL)
		pClone->MaxPos = MaxPos;

	return(pClone);
}



/******************************************************************************************

>	virtual BOOL ValueFunctionBlip::IsDifferent(ValueFunction *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE if the objects are considered different,
				FALSE if they are considered identical

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

	Notes:		Calls the base class to see if they are different classes,
				and then compares identical classes by checking member vars

******************************************************************************************/

BOOL ValueFunctionBlip::IsDifferent(ValueFunction *pOther)
{
	if (ValueFunction::IsDifferent(pOther))
		return(TRUE);
	
	// Both objects are instances of this class, so compare them more carefully
	return(MaxPos != ((ValueFunctionBlip *)pOther)->MaxPos);
}



/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunctionBlip::WriteFileRecord(INT32 RecordTag,
											INT32 ExtraBytes, BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		RecordTag	- the tag to write this record under
				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.
				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

	Purpose:	Saves a ValueFunction object to a Xara file.
				This function will create a new variable-sized record with the given
				record tag, and will write out whatever data is needed to save this
				ValueFunction's state to the file.
				
				"ExtraBytes" bytes will be added to the record size to reserve space
				at the end of the record for the caller to add their own data. This
				is to allow ValueFunctions to be saved embedded in other object's
				record structures (e.g. inside different types of attributes).

				All ValueFunction record data has 3 sections, whiich are recorded as follows:
				1. ValueFunction header, identifying which type of VF is being saved
						INT32 ValueFunctionUniqueID		(4 bytes)
				2. Derived-class-data (0 or more bytes). This particular class adds:
						float MaxPosition				(4 bytes)
				3. Caller data. This is written by the caller to the returned record
				   object. This data must be ExtraBytes (0 or more) bytes in length.

	SeeAlso:	ValueFunctionBlip::CreateAndReadFileRecord

******************************************************************************************/

CamelotFileRecord *ValueFunctionBlip::WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::WriteFileRecord");

	// Calculate how many bytes of information this VF will write. We do not include
	// the header info written by the base class or the ExtraInfo desired by the caller -
	// the base class adds all that in for us.
	const INT32 MyRecordSize = 4;

	// Create an appropriate record, and write our data to it
	CamelotFileRecord *pRec = CreateAndWriteFileRecord(RecordTag, MyRecordSize, ExtraBytes, pFilter);

	if (pRec != NULL)
	{
		// Write out our ValueFunction's specific data. If it fails, then we'll return NULL
		if (!pRec->WriteFLOAT((float)MaxPos))
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionBlip::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionBlip::WriteFileRecord;
				ValueFunctionBlip::ReadFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionBlip::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float MaxPosition = (float)0.5;
	pInputRecord->ReadFLOAT(&MaxPosition);

	ValueFunctionBlip *pBlip = CreateInstance();
	if (pBlip != NULL)
		pBlip->MaxPos = MaxPosition;

	return(pBlip);
}



/******************************************************************************************

>	virtual ValueFunctionBlip *ValueFunctionBlip::CreateInstance(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Returns:	NULL if it failed, else a pointer to new ValueFunctionBlip object of
				the same type as "this" one. Used to allow derived Blip classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionBlip & ValueFunctionBlipS.

	Notes:		Called by ValueFunctionBlip::Clone and CreateAndReadFileRecord,
				as this is the only action in those functions that depends on the type
				of the class that is being used

******************************************************************************************/

ValueFunctionBlip *ValueFunctionBlip::CreateInstance(void)
{	
	return(new ValueFunctionBlip);
}








/******************************************************************************************

>	virtual double ValueFunctionTeardrop::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionTeardrop::GetValue(double Position)
{
	double Value;
	if (Position < MaxPos)
	{
		Position = 1.0 - Position/MaxPos;
		if ( Position>1.0 )
			Position = 1.0;
		Value = sqrt(1-Position*Position);
	}
	else
	{
		Position = (Position - MaxPos) / (1.0 - MaxPos);
		Value = (cos( Position * Pi) + 1.0) / 2.0;
	}

	return Value;
}



/******************************************************************************************

>	virtual ValueFunctionBlip *ValueFunctionTeardrop::CreateInstance(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Returns:	NULL if it failed, else a pointer to new ValueFunctionTeardrop object of
				the same type as "this" one. Used to allow derived Teardrop classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionTeardrop & ValueFunctionTeardropS.

	Notes:		Called by ValueFunctionBlip::Clone and CreateAndReadFileRecord,
				as this is the only action in those functions that depends on the type
				of the class that is being used

******************************************************************************************/

ValueFunctionBlip *ValueFunctionTeardrop::CreateInstance(void)
{	
	return(new ValueFunctionTeardrop);
}


/******************************************************************************************

>	virtual double ValueFunctionTeardropCurvedEnd::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionTeardropCurvedEnd::GetValue(double Position)
{
	double Value;
	if (Position < 0.2)
	{
		Position = 1.0 - Position/0.2;
		if ( Position>1.0 )
			Position = 1.0;
		Value = sqrt(1-Position*Position);
	}
	else
	{
		Position = (Position - 0.2) / 0.8;
		Value = sqrt(1.0 - Position);
	}

/*		if (Position > 0.9)
		{
			Position = (Position - 0.9) / 0.1;
			Value = 0.4 * (cos(asin(Position)));
		}
		else
		{
			Position = (Position - 0.2) / 0.7;
			Value = ((1.0 - (Position * Position)) * 0.6) + 0.4;
		}
	}
*/
	return(Value);
}


/******************************************************************************************

>	virtual INT32 ValueFunctionTeardropCurvedEnd::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionTeardropCurvedEnd::GetMinimumRecursionDepth()
{
	return 2;
}


/******************************************************************************************

>	virtual ValueFunctionBlip *ValueFunctionTeardropCurvedEnd::CreateInstance(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Returns:	NULL if it failed, else a pointer to new ValueFunctionTeardrop object of
				the same type as "this" one. Used to allow derived Teardrop classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionTeardrop & ValueFunctionTeardropS.

	Notes:		Called by ValueFunctionBlip::Clone and CreateAndReadFileRecord,
				as this is the only action in those functions that depends on the type
				of the class that is being used

******************************************************************************************/

ValueFunctionBlip *ValueFunctionTeardropCurvedEnd::CreateInstance(void)
{	
	return(new ValueFunctionTeardropCurvedEnd);
}








/******************************************************************************************

>	virtual double ValueFunctionEllipse::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionEllipse::GetValue(double Position)
{
	if (Position < MaxPos)
		Position = 1.0 - Position/MaxPos;
	else
		Position = (Position - MaxPos) / (1.0 - MaxPos);
	if ( Position>1.0 )
		Position = 1.0;
	return sqrt(1-Position*Position);
}



/******************************************************************************************

>	virtual ValueFunctionBlip *ValueFunctionEllipse::CreateInstance(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/97

	Returns:	NULL if it failed, else a pointer to new ValueFunctionEllipse object of
				the same type as "this" one. Used to allow derived Ellipse classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionEllipse & ValueFunctionEllipseS.

	Notes:		Called by ValueFunctionBlip::Clone and CreateAndReadFileRecord,
				as this is the only action in those functions that depends on the type
				of the class that is being used

******************************************************************************************/

ValueFunctionBlip *ValueFunctionEllipse::CreateInstance(void)
{	
	return(new ValueFunctionEllipse);
}









/******************************************************************************************

>	virtual double ValueFunctionThumbtack::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionThumbtack::GetValue(double Position)
{
	if (Position < MaxPos)
		Position = Position / MaxPos;
	else
		Position = 1.0 - ((Position - MaxPos) / (1.0 - MaxPos));

	if ( Position>1.0 )
		Position = 1.0;
	return 1.0 - sqrt(1-Position*Position);
}



/******************************************************************************************

>	virtual ValueFunctionBlip *ValueFunctionThumbtack::CreateInstance(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/97

	Returns:	NULL if it failed, else a pointer to new ValueFunctionThumbtack object of
				the same type as "this" one. Used to allow derived Thumbtack classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionThumbtack & ValueFunctionThumbtackS.

	Notes:		Called by ValueFunctionBlip::Clone and CreateAndReadFileRecord,
				as this is the only action in those functions that depends on the type
				of the class that is being used

******************************************************************************************/

ValueFunctionBlip *ValueFunctionThumbtack::CreateInstance(void)
{	
	return(new ValueFunctionThumbtack);
}






///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionPressure classes
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	ValueFunctionPressure::ValueFunctionPressure()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Purpose:	Constructor
				Pressure functions store a table of recorded pressure values and 
				return curve-interpolated pressure values on demand.

******************************************************************************************/

ValueFunctionPressure::ValueFunctionPressure()
{
	pArray		= NULL;
	ArraySize	= 0;
	NextFree	= 0;
}



/******************************************************************************************

>	ValueFunctionPressure::~ValueFunctionPressure()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Purpose:	Destructor

******************************************************************************************/

ValueFunctionPressure::~ValueFunctionPressure()
{
	if (pArray != NULL)
		CCFree(pArray);
}



/******************************************************************************************

>	virtual double ValueFunctionPressure::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the recorded pressure at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		This determines the 2 closest position values for which pressure
				information was recorded, and interpolates the values by "fitting a
				curve" through the points.

				The "curve" may be improved in future, but is currently a simple
				linear interpolation.

				****!!!!TODO: This should cache the last array index it was asked for,
				and start the search from that index, as we usually ask for values for
				sequential, close, position values, and this will vastly reduce the
				searching overhead in most cases. Note that we may step through
				positions in both positive & negative directions, so it will have
				to handle this carefully.

******************************************************************************************/

double ValueFunctionPressure::GetValue(double Position)
{
	// No entries have been recorded!
	if (NextFree < 1)
		return(0.0);

	// If the position is outside the range in our table, return the closest entry
	if (Position <= pArray[0].Position)
		return(pArray[0].Pressure);

	if (Position >= pArray[NextFree-1].Position)
		return(pArray[NextFree-1].Pressure);

	// Search to find the closest 2 array entries to the position
	// We assume the array fills the 0.0 to 1.0 range, so we search from the closest
	// "end" of the array to halve the average search time.
	INT32 i;
	if (Position >= 0.5)
	{
		i = NextFree;
		while (i > 1 && Position < pArray[i-1].Position)
			i--;
	}
	else
	{
		i = 1;
		while (i < NextFree && Position > pArray[i].Position)
			i++;
	}

	ERROR3IF(i < 1 || i >= NextFree, "I've screwed up the scanning loop code");

	// Otherwise, the position lies between array elements [i-1] and [i], so we
	// interpolate between these positions
	ERROR3IF((pArray[i].Position - pArray[i-1].Position) <= 0.0, "Pressure array is non-ascending!");
	const double MixValue = (Position - pArray[i-1].Position) /
							(pArray[i].Position - pArray[i-1].Position);

	return((MixValue * pArray[i].Pressure) + ((1.0 - MixValue) * pArray[i-1].Pressure));
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionPressure::Clone(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Returns:	NULL if it failed, else an exact copy of this object

	Purpose:	Clone operator. Creates an exact copy of this object

******************************************************************************************/

ValueFunction *ValueFunctionPressure::Clone(void)
{
	// Create another object of this type
	ValueFunctionPressure *pClone = CreateInstance();

	if (pClone != NULL)
	{
		// If that worked, make it allocate an array large enough to hold a copy of
		// our data, and memcpy the array across.
		if (pClone->ExpandArray(NextFree))
		{
			// We only copy the used array entries so the clone will have no wasted space in it
			memcpy(pClone->pArray, pArray, NextFree * sizeof(VFPressureRecord));
			pClone->NextFree  = NextFree;
			pClone->ArraySize = NextFree;
		}
	}

	return(pClone);
}



/******************************************************************************************

>	virtual BOOL ValueFunctionPressure::IsDifferent(ValueFunction *Other)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE if the objects are considered different,
				FALSE if they are considered identical

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

	Notes:		Calls the base class to see if they are different classes,
				and then compares identical classes by checking member vars

******************************************************************************************/

BOOL ValueFunctionPressure::IsDifferent(ValueFunction *pOther)
{
	if (ValueFunction::IsDifferent(pOther))
		return(TRUE);
	
	// Both objects are instances of this class, so compare them more carefully
	ValueFunctionPressure *pPressure = (ValueFunctionPressure *)pOther;
	if (NextFree != pPressure->NextFree)
		return(TRUE);					// Don't have same number of entries

	// Now compare the entire array
	for (INT32 i = 0; i < NextFree; i++)
	{
		if (pArray[i].Position != pPressure->pArray[i].Position ||
			pArray[i].Pressure != pPressure->pArray[i].Pressure)
		{
			return(TRUE);
		}
	}

	// Cor. They must be the same!
	return(FALSE);
}



/******************************************************************************************

>	BOOL ValueFunctionPressure::AddPressure(float Position, BYTE PressureValue)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Inputs:		Position - A number representing distance along the path of this point
				(Normally this would be between 0.0 and 1.0, but you can temporarily
				use positions between 0.0 and any positive value and then call
				NormalisePositions if this is more convenient while building the table)

				NOTE: Each added pressure value must be a positive number, and must
				be greater than the last in order to be recorded. Illegal values
				will be discarded.

				PressureValue - The pressure value to record for this position

	Returns:	FALSE if memory was unavailable
				TRUE if it was happy (even if it discarded the value)

	Purpose:	Adds a pressure record to the ValueFunction table. 

******************************************************************************************/

BOOL ValueFunctionPressure::AddPressure(float Position, float PressureValue)
{
	ERROR3IF(PressureValue < 0.0 || PressureValue > 1.0, "Illegal pressure value");
	ERROR3IF(Position < 0.0, "Illegal (negative) position value");

	// Make sure there's plenty of room for new entries
	if (NextFree >= ArraySize && !ExpandArray(ArraySize + 32))
		return(FALSE);

	// Only record the value if the position has advanced. Error if the position has
	// gone retrograde, but quietly dismiss equal positions.
	ERROR3IF(NextFree > 0 && Position < pArray[NextFree-1].Position,
				"AddPressure - pressure values must be ascending");
	if (NextFree > 0 && Position <= pArray[NextFree-1].Position)
		return(TRUE);

	// And write this entry
	pArray[NextFree].Position = Position;
	pArray[NextFree].Pressure = PressureValue;
	NextFree++;

	return(TRUE);
}



/******************************************************************************************

>	void ValueFunctionPressure::NormalisePositions(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Purpose:	Normalises all positions in the table to lie within a 0.0 to 1.0 range,
				by dividing them by the last element position value. (i.e. it assumes that
				the last element in the array will have the largest Position value)

******************************************************************************************/

void ValueFunctionPressure::NormalisePositions(void)
{
	// If fewer than 1 entries, or no array, then there's nothing to do
	if (pArray == NULL || NextFree < 1)
		return;

	// Find the maximum value, and baulk if it would cause a divide by zero.
	if (pArray[NextFree-1].Position <= 0.0)
	{
		ERROR3("Max position in table is zero or negative - normalisation failed");
		return;
	}

	float RMaxPosition = 1.0/pArray[NextFree-1].Position;
	for (INT32 i = 0; i < NextFree; i++)
		pArray[i].Position *= RMaxPosition;
}



/******************************************************************************************

>	BOOL ValueFunctionPressure::ExpandArray(INT32 NewSize)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Inputs:		NewSize - The number of array entries desired.

	Returns:	FALSE if memory was unavailable

	Purpose:	Ensures that the pressure storage array has no fewer than NewSize entries.

	Notes:		For efficiency, unused array entries are always left uninitialised

******************************************************************************************/

BOOL ValueFunctionPressure::ExpandArray(INT32 NewSize)
{
	if (NewSize <= ArraySize)
		return(TRUE);			// That was easy!

	VFPressureRecord *pNewArray = NULL;

	if (pArray == NULL)
		pNewArray = (VFPressureRecord *) CCMalloc(NewSize * sizeof(VFPressureRecord));
	else
		pNewArray = (VFPressureRecord *) CCRealloc(pArray, NewSize * sizeof(VFPressureRecord));

	if (pNewArray == NULL)
		return(FALSE);

	// We succeeded, so remember the new address & size
	pArray = pNewArray;
	ArraySize = NewSize;

	return(TRUE);
}



/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunctionPressure::WriteFileRecord(INT32 RecordTag,
											INT32 ExtraBytes, BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		RecordTag	- the tag to write this record under
				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.
				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

	Purpose:	Saves a ValueFunction object to a Xara file.
				This function will create a new variable-sized record with the given
				record tag, and will write out whatever data is needed to save this
				ValueFunction's state to the file.
				
				"ExtraBytes" bytes will be added to the record size to reserve space
				at the end of the record for the caller to add their own data. This
				is to allow ValueFunctions to be saved embedded in other object's
				record structures (e.g. inside different types of attributes).

				All ValueFunction record data has 3 sections, whiich are recorded as follows:
				1. ValueFunction header, identifying which type of VF is being saved
						INT32 ValueFunctionUniqueID		(4 bytes)
				2. Derived-class-data (0 or more bytes). This particular class adds:
						UINT32 NumTableEntries
						+ n entries of the form:
							UNIT16 Position
							UINT16 Pressure				(4 + (NumTableEntries*4) bytes)
				3. Caller data. This is written by the caller to the returned record
				   object. This data must be ExtraBytes (0 or more) bytes in length.

				The Position & Pressure values are 16-bit unsigned values between
				0 & 65535. These represent floating point values between 0.0 & 1.0,
				i.e. are simply a (UINT16) (Value * 65535.0). I don't bother avoiding
				rounding errors, as 1/128000th is an insignificant error.

	SeeAlso:	ValueFunctionPressure::CreateAndReadFileRecord;
				ValueFunction::CreateAndWriteFileRecord

******************************************************************************************/

CamelotFileRecord *ValueFunctionPressure::WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::WriteFileRecord");

	// Calculate how many bytes of information this VF will write. We do not include
	// the header info written by the base class or the ExtraInfo desired by the caller -
	// the base class adds all that in for us.
	const INT32 MyRecordSize = 4 + (4 * NextFree);

	// Create an appropriate record, and write our data to it
	CamelotFileRecord *pRec = CreateAndWriteFileRecord(RecordTag, MyRecordSize, ExtraBytes, pFilter);

	if (pRec != NULL)
	{
		BOOL ok = TRUE;
		if (ok)		ok = pRec->WriteUINT32((UINT32) NextFree);

		for (INT32 i = 0; (i < NextFree) && ok; i++)
		{
			if (ok)		ok = pRec->WriteUINT16((UINT16) (pArray[i].Position * 65535.0));
			if (ok)		ok = pRec->WriteUINT16((UINT16) (pArray[i].Pressure * 65535.0));
		}

		if (!ok)			// If we failed, clean up & return NULL
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}



/******************************************************************************************

>	virtual ValueFunction *ValueFunctionPressure::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

				This method creates a new instance of this particular ValueFunction class
				and then loads whatever information is necessary from the file to 
				initialise itself properly. The record read-pointer is left pointing
				at the end of the ValueFunction-saved data so that the caller can continue
				reading their extra bytes of data after loading the ValueFunction.

	SeeAlso:	ValueFunctionPressure::WriteFileRecord

******************************************************************************************/

ValueFunction *ValueFunctionPressure::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	UINT32 NumEntries = 0;
	pInputRecord->ReadUINT32(&NumEntries);

	ValueFunctionPressure *pValFunc = CreateInstance();

	if (pValFunc != NULL && NumEntries > 0)
	{
		// Make the array big enough to hold all the entries, to save reallocing it as we go,
		// and also to save us having to check for failure on each pressure item we add.
		if (!pValFunc->ExpandArray(NumEntries))
		{
			delete pValFunc;
			return(NULL);
		}

		UINT16 Position;
		UINT16 Pressure;

		for (UINT32 i = 0; i < NumEntries; i++)
		{
			pInputRecord->ReadUINT16(&Position);
			pInputRecord->ReadUINT16(&Pressure);
			pValFunc->AddPressure(	((float)Position) / (float) 65535.0,
									((float)Pressure) / (float) 65535.0);
		}
	}

	return(pValFunc);
}



/******************************************************************************************

>	virtual ValueFunctionPressure *ValueFunctionPressure::CreateInstance(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/1/97

	Returns:	NULL if it failed, else a pointer to new ValueFunctionPressure object of
				the same type as "this" one. Used to allow derived Pressure classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionPressure & ValueFunctionPressureS.

	Notes:		Called by ValueFunctionPressure::Clone and CreateAndReadFileRecord,
				as this is the only action in those functions that depends on the type
				of the class that is being used

******************************************************************************************/

ValueFunctionPressure *ValueFunctionPressure::CreateInstance(void)
{	
	return(new ValueFunctionPressure);
}








/******************************************************************************************

>	virtual double ValueFunctionPressureS::GetValue(double Position)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/97

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value representing the recorded pressure at the given Position.

	Purpose:	To read the value of this function at a given position

	Notes:		This determines the 2 closest position values for which pressure
				information was recorded, and interpolates the values by "fitting a
				curve" through the points.

				The "curve" may be improved in future, but is currently a simple
				linear interpolation.

				****!!!!TODO: This could cache the last array index it was asked for,
				and start the search from that index, as we usually ask for values for
				sequential, close, position values, and this will reduce the
				searching overhead in most cases. Note that we may step through
				positions in both positive & negative directions, so it will have
				to handle this carefully. Note that PressureS functions seldom have
				very many samples recorded in their tables, due to my wonderfully
				rampant smoothing, so this may not turn out to be very beneficial.

******************************************************************************************/

double ValueFunctionPressureS::GetValue(double Position)
{
	// No entries have been recorded!
	if (NextFree < 1)
		return(0.0);

	// If the position is outside the range in our table, return the closest entry
	if (Position <= pArray[0].Position)
		return(pArray[0].Pressure);

	if (Position >= pArray[NextFree-1].Position)
		return(pArray[NextFree-1].Pressure);

	// Search to find the closest 2 array entries to the position
	// We assume the array fills the 0.0 to 1.0 range, so we search from the closest
	// "end" of the array to halve the average search time.
	INT32 i;
	if (Position >= 0.5)
	{
		i = NextFree;
		while (i > 1 && Position < pArray[i-1].Position)
			i--;
	}
	else
	{
		i = 1;
		while (i < NextFree && Position > pArray[i].Position)
			i++;
	}

	ERROR3IF(i < 1 || i >= NextFree, "I've screwed up the scanning loop code (1)");
	ERROR3IF(Position < pArray[i-1].Position || Position > pArray[i].Position,
									  "I've screwed up the scanning loop code (2)");

	// Otherwise, the position lies between array elements [i-1] and [i], so we
	// interpolate between these positions
	ERROR3IF((pArray[i].Position - pArray[i-1].Position) <= 0.0, "Pressure array is non-ascending!");
	double MixValue = (Position - pArray[i-1].Position) /
						(pArray[i].Position - pArray[i-1].Position);

	// And convert the linear mix value into a nice S-curve (180 degrees of sine wave)
	// We use a different function on the first and last segments so that you get nicer
	// ends to your strokes. (90 degrees of sine wave)
	if (i < 2 || i >= NextFree - 1)
		MixValue = cos(-MixValue * (Pi / 2.0));
	else
		MixValue = (cos(MixValue * Pi) + 1.0) / 2.0;

	return((MixValue * pArray[i-1].Pressure) + ((1.0 - MixValue) * pArray[i].Pressure));
}



/******************************************************************************************

>	virtual ValueFunctionPressure *ValueFunctionPressureS::CreateInstance(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/97

	Returns:	NULL if it failed, else a pointer to new ValueFunctionPressure object of
				the same type as "this" one. Used to allow derived Pressure classes to
				use all the base class code, and merely override the GetValue method.

	Purpose:	Creates a new blank object of the same type as this one. Internal
				function used in ValueFunctionPressure & ValueFunctionPressureS.

******************************************************************************************/

ValueFunctionPressure *ValueFunctionPressureS::CreateInstance(void)
{	
	return(new ValueFunctionPressureS);
}





///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionSmoothStroke classes
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	ValueFunctionSmoothStroke::ValueFunctionSmoothStroke()

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Purpose:	Constructor

******************************************************************************************/

ValueFunctionSmoothStroke::ValueFunctionSmoothStroke(double newValue1y, double newValue2x, double newValue2y, double newValue3x, double newValue3y, double newValue4y)
{
	Value1y = newValue1y;
	Value2x = newValue2x;
	Value2y = newValue2y;
	Value3x = newValue3x;
	Value3y = newValue3y;
	Value4y = newValue4y;
}

ValueFunctionSmoothStroke::~ValueFunctionSmoothStroke()
{
}

/******************************************************************************************

>	virtual BOOL ValueFunctionSmoothStroke::IsDifferent(ValueFunction *Other)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Inputs:		pOther - Another ValueFunction object to compare this one to

	Returns:	TRUE if the objects are considered different,
				FALSE if they are considered identical

	Purpose:	Comparator.
				Determines if 2 different ValueFunction objects are considered
				different.

	Notes:		Calls the base class to see if they are different classes,
				and then compares identical classes by checking member vars

******************************************************************************************/

BOOL ValueFunctionSmoothStroke::IsDifferent(ValueFunction *pOther)
{
	if (ValueFunction::IsDifferent(pOther))
	{
		return(true);
	}
		// Both objects are instances of this class, so compare them more carefully
		return( (Value1y != ((ValueFunctionSmoothStroke *)pOther)->Value1y) ||
				(Value2x != ((ValueFunctionSmoothStroke *)pOther)->Value2x) ||
				(Value2y != ((ValueFunctionSmoothStroke *)pOther)->Value2y) ||
				(Value3x != ((ValueFunctionSmoothStroke *)pOther)->Value3x) ||
				(Value3y != ((ValueFunctionSmoothStroke *)pOther)->Value3y) ||
				(Value4y != ((ValueFunctionSmoothStroke *)pOther)->Value4y) ) ;
}


/******************************************************************************************

>	virtual CamelotFileRecord *ValueFunctionSmoothStroke::WriteFileRecord(INT32 RecordTag,
											INT32 ExtraBytes, BaseCamelotFilter *pFilter)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Inputs:		RecordTag	- the tag to write this record under
				ExtraBytes	- The number of extra bytes of information the caller
							  will write into the record after caling this function
							  (Space for this many extra bytes will be reserved by
							  this function when it creates the new file record)
							  This may be 0 or more bytes.
				pFilter		- The filter to write to

	Returns:	NULL if it failed, else a pointer to a record which saves the state
				of this valueFunction object. Once the caller has written the record,
				they MUST DELETE it.

	Purpose:	Saves a ValueFunction object to a Xara file.
				This function will create a new variable-sized record with the given
				record tag, and will write out whatever data is needed to save this
				ValueFunction's state to the file.

******************************************************************************************/

CamelotFileRecord *ValueFunctionSmoothStroke::WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes,
															BaseCamelotFilter *pFilter)
{
	ERROR3IF(pFilter == NULL, "Illegal NULL param");
	ERROR3IF(ExtraBytes < 0, "Stupid ExtraBytes request in ValueFunction::WriteFileRecord");

	const INT32 MyRecordSize = 24;

	// Create an appropriate record, and write our data to it
	CamelotFileRecord *pRec = CreateAndWriteFileRecord(RecordTag, MyRecordSize, ExtraBytes, pFilter);

	if (pRec != NULL)
	{
		BOOL ok = TRUE;
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value1y);
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value2x);
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value2y);
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value3x);
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value3y);
		if (ok)		ok = pRec->WriteFLOAT((FLOAT) Value4y);

		if (!ok)			// If we failed, clean up & return NULL
		{
			delete pRec;
			pRec = NULL;
		}
	}

	return(pRec);
}


/******************************************************************************************

>	virtual ValueFunction *ValueFunctionSmoothStroke::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Inputs:		pInputRecord - The file record to read

	Returns:	NULL if it failed, else a pointer to new ValueFunction object representing
				whatever was saved in that record.

	Purpose:	Loads a ValueFunction object from a record which was saved into a file
				using the WriteFileRecord call. This is called by the base class
				loader routine ReadFileRecord, which finds an appropriate instance
				of a derived class to call to load the data in question.

******************************************************************************************/

ValueFunction *ValueFunctionSmoothStroke::CreateAndReadFileRecord(CXaraFileRecord *pInputRecord)
{
	ERROR3IF(pInputRecord == NULL, "Illegal NULL param");

	float newValue1y = 0.0;
	float newValue2x = 0.0;
	float newValue2y = 0.0;
	float newValue3x = 0.0;
	float newValue3y = 0.0;
	float newValue4y = 0.0;

	pInputRecord->ReadFLOAT(&newValue1y);
	pInputRecord->ReadFLOAT(&newValue2x);
	pInputRecord->ReadFLOAT(&newValue2y);
	pInputRecord->ReadFLOAT(&newValue3x);
	pInputRecord->ReadFLOAT(&newValue3y);
	pInputRecord->ReadFLOAT(&newValue4y);


	ValueFunctionSmoothStroke *pSStroke = CreateInstance();
	if (pSStroke != NULL)
	{
		pSStroke->Value1y = newValue1y;
		pSStroke->Value2x = newValue2x;
		pSStroke->Value2y = newValue2y;
		pSStroke->Value3x = newValue3x;
		pSStroke->Value3y = newValue3y;
		pSStroke->Value4y = newValue4y;
	}

	return(pSStroke);
}


/******************************************************************************************

>	virtual ValueFunctionSmoothStroke *ValueFunctionSmoothStroke::CreateInstance(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Returns:	NULL if it failed, else a pointer to new ValueFunctionSmoothStroke object of
				the same type as "this" one.

******************************************************************************************/

ValueFunctionSmoothStroke *ValueFunctionSmoothStroke::CreateInstance(void)
{
	//You shouldn't really be calling this base class CreateInstance() so Fail!
	return NULL;
}


/******************************************************************************************

>	virtual INT32 ValueFunctionSmoothStroke::GetMinimumRecursionDepth(void)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000

	Purpose:	Overriden function to indicate to the path stroker how many recursions to
				perform at minimum before declaring a line segment 'flat' enough

******************************************************************************************/

INT32 ValueFunctionSmoothStroke::GetMinimumRecursionDepth()
{
	return 2;
}



/******************************************************************************************

>	virtual double ValueFunctionSmoothStroke::GetValue(double Position)

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Inputs:		Position - A value between 0.0 and 1.0
	Returns:	A value (between Value1 and Value2 given in the constructor) representing
				the value of the function at the given Position.

	Purpose:	To read the value of this function at a given position

******************************************************************************************/

double ValueFunctionSmoothStroke::GetValue(double Position)
{
	//Which ValueFunction are we looking at?
	INT32 index = (INT32)GetUniqueID();
	index -= (INT32)ValueFunctionID_BevelEnds + 1;


	INT32 counter;

	//Find the first xvalue which is greater than or equal to our position
	for (counter = 0; counter < 256; counter++)
	{
		if (xvalue[index][counter] >= Position)
		{
			//Stop looking through the array when we have found it!
			break;
		}
	}
		
	if (counter > 0)
	{
		counter -= 1;
	}

	//Work out how far through between xvalue[counter] and xvalue[counter+1] we are, then multiply
	//this by the vertical difference of the two coordinates, then add on yvalue[counter] to get the
	//approximate value at this Position...
	double xdifference = xvalue[index][counter+1] - xvalue[index][counter];
	double ydifference = yvalue[index][counter+1] - yvalue[index][counter];

	//Now just check to make sure that we aren't about to divide by zero!
	if (xdifference == 0.0)
	{
		//Set to a very small number...
		xdifference = 0.0000000000000001;
	}

	double value = yvalue[index][counter] + (((Position - xvalue[index][counter]) / xdifference) * ydifference);

	return value;
}




void ValueFunctionSmoothStroke::MakeTable()
{
	//Which ValueFunction are we creating tbe table for?
	INT32 index = (INT32)GetUniqueID();
	index -= (INT32)ValueFunctionID_BevelEnds + 1;


	//Make the four control points of the Bezier...
	DocCoord p1;
	p1.x = 0; p1.y = INT32(Value1y*100000);
	DocCoord p2;
	p2.x = INT32(Value2x*100000); p2.y = INT32(Value2y*100000);
	DocCoord p3;
	p3.x = INT32(Value3x*100000); p3.y = INT32(Value3y*100000);
	DocCoord p4;
	p4.x = 100000; p4.y = INT32(Value4y*100000);

	//Create the Bezier curve path denoted by the four control points
	Path strokePath;
	strokePath.Initialise();
	strokePath.InsertMoveTo(p1, NULL);
	strokePath.InsertCurveTo(p2, p3, p4, NULL);

	//Find out how long the path is...
	double pathlength = strokePath.GetPathLength();
	DocCoord* pPoint = new DocCoord;

	TRACEUSER( "Matt", _T("\nStarting to make Bezier table...\n"));
	for (INT32 counter = 0; counter < 256; counter++)
	{
		//At even intervals along the length of the curve, record the x and y coordinate
		MILLIPOINT Distance = (INT32(pathlength)/256)*counter;
		strokePath.GetPointAtDistance(Distance, pPoint, NULL, NULL);

		xvalue[index][counter] = double(pPoint->x) / 100000.0;
		yvalue[index][counter] = double(pPoint->y) / 100000.0;

//		TRACEUSER( "Matt", _T("xvalue[%d] = %f; yvalue[%d] = %f;\n"),counter,xvalue[counter],counter,yvalue[counter]);
	}
	TRACEUSER( "Matt", _T("Finished table...\n\n"));

	delete pPoint;
}
