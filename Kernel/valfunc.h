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
// Definition of ValueFunction classes (used in stroke providing)

#ifndef INC_VALFUNC
#define INC_VALFUNC

//#include "listitem.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class CamelotFileRecord;
class BaseCamelotFilter;
class CXaraFileRecord;


///////////////////////////////////////////////////////////////////////////////////////////
// Unique ID values
// Used when storing ValueFunctions in Native/Web file formats, so they MUST NOT CHANGE
// Hence, add any new ones to the END of the list.
///////////////////////////////////////////////////////////////////////////////////////////

typedef enum
{
	ValueFunctionID_None = 0,
	ValueFunctionID_Constant,
	ValueFunctionID_Random,
	ValueFunctionID_RampLinear,
	ValueFunctionID_RampS,
	ValueFunctionID_Pressure,
	ValueFunctionID_PressureS,
	ValueFunctionID_Teardrop,
	ValueFunctionID_Ellipse,
	ValueFunctionID_Blip,
	ValueFunctionID_Thumbtack,
	ValueFunctionID_RampL,
	ValueFunctionID_RampL2,
	ValueFunctionID_RampS2,
	ValueFunctionID_TeardropCurvedEnd,
	ValueFunctionID_SawTooth,
	ValueFunctionID_Propeller,
	ValueFunctionID_DoubleRampS,
	ValueFunctionID_Intestine,
	ValueFunctionID_Decay,
	ValueFunctionID_BevelEnds,
	//You can gladly tack stuff on the end, but don't screw up the ordering
	//around these 'ere parts!!! Matt
	ValueFunctionID_SS_Reed,
	ValueFunctionID_SS_Meteor,
	ValueFunctionID_SS_Petal,
	ValueFunctionID_SS_Comet,
	ValueFunctionID_SS_Barb,
	ValueFunctionID_SS_Concave,
	ValueFunctionID_SS_Convex,
	ValueFunctionID_SS_Iron,
	ValueFunctionID_SS_Torpedo,
	ValueFunctionID_SS_Missile,
	ValueFunctionID_SS_Goldfish,
	ValueFunctionID_SS_OceanLiner,
	ValueFunctionID_SS_Yacht,
	ValueFunctionID_SS_SlimBlip,
	ValueFunctionID_SS_Cigar,
	ValueFunctionID_SS_Cigar2,
	ValueFunctionID_SS_Cigar3,
	ValueFunctionID_SS_Fallout
} ValueFunctionID;



///////////////////////////////////////////////////////////////////////////////////////////
// Base class
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	class ValueFunction : public CCObject

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Abstract Base class
				Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				The function may be whatever you desire. Likely derived classes
				might return a functional result such as a Sine, or interpolate
				between 2 other values, or possibly look the position up in a
				table.

				This is quite a generic class, but it is mainly used by the stroke
				provider system for handling variable line width functions.

	Notes:		Positions passed into these functions will always be within the
				range 0.0 to 1.0.

				Generally, outputs from these functions will lie between -1.0
				and 1.0, although there is no reason why the derived classes
				cannot be passed ranges (e.g. see the Ramp classes below)

******************************************************************************************/

class ValueFunction : public ListItem
{
	CC_DECLARE_DYNAMIC(ValueFunction);

public:
	static	BOOL Init(void);
	static	void DeInit(void);
			// Initialises/Deinitialises all ValueFunctions currently in valfunc.cpp

public:
	virtual double GetValue(double Position) = 0;
			// Returns the function value for a given position (0.0 - 1.0)

	virtual ValueFunction *Clone(void) = 0;
			// Makes a copy of this ValueFunction object

	virtual BOOL IsDifferent(ValueFunction *pOther);
			// Equality operator


public:		// File format support
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter) = 0;
			// Writes this ValueFunction out to a native/web filter as part of a record with the
			// given tag, and with ExtraBytes reserved in the record for the caller to fill in on return.

	static	ValueFunction *ReadFileRecord(CXaraFileRecord *pInputRecord);
			// Reads a ValueFunction description out of the given file record, and creates a new
			// object of the appropriate type & settings from it.

public:
		virtual ValueFunctionID GetUniqueID(void) = 0;
			// Retrieves a unique ID for this class of ValueFunction.
			// Used as an ID tag in the file format

		virtual INT32 GetMinimumRecursionDepth() { return 0; };
			// Overridable - indicates the minimum number of recursions the path stroker should
			// perform before declaring the function 'flat' enough - ensures that the path stroking
			// can always identify your value function correctly

protected:
	CamelotFileRecord *CreateAndWriteFileRecord(INT32 RecordTag, INT32 DerivedBytes, INT32 ExtraBytes,
												BaseCamelotFilter *pFilter);
			// Writes out header information for a standard ValueFunction record structure


	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord) = 0;
			// Reads the given record and creates a ValueFunciton of an appropriate type from it

protected:
	static	List RegisteredFunctions;
};



///////////////////////////////////////////////////////////////////////////////////////////
// Derived classes
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	class ValueFunctionConstant : public ValueFunction

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class simply returns a constant value

******************************************************************************************/

class ValueFunctionConstant : public ValueFunction
{
	CC_DECLARE_DYNAMIC(ValueFunctionConstant);

public:
	ValueFunctionConstant(double Value = 1.0);
	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual BOOL IsDifferent(ValueFunction *pOther);

public:		// File format support
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Constant); };

protected:
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);


protected:
	double Value1;
};




/******************************************************************************************

>	class ValueFunctionRandom : public ValueFunction

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class simply returns a "Random" value. Actually, the
				random value is seeded from the Position plus the seed passed to the
				constructor, so the same position will always give the same value.

	Notes:		This class actually generates 512 random numbers, and returns samples
				from that array. This is so that when used for path stroking, it is
				possible to limit the flattening of the generated curve - with truly
				random results, you can never know when you have flattened enough!

******************************************************************************************/

const INT32 NumRandomValues = 512;

class ValueFunctionRandom : public ValueFunction
{
	CC_DECLARE_DYNAMIC(ValueFunctionRandom);

public:
	ValueFunctionRandom(UINT32 Seed = 0, double Min = 0.8, double Max = 1.0);
	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual BOOL IsDifferent(ValueFunction *pOther);

public:		// File format support
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Random); };

protected:
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	

protected:
	short  Array[NumRandomValues];
	UINT32   SeedValue;
	double MinValue;
	double MaxValue;
};




/******************************************************************************************

>	class ValueFunctionRamp			: public ValueFunction		(abstract base class)
>	class ValueFunctionRampLinear	: public ValueFunctionRamp
>	class ValueFunctionRampS		: public ValueFunctionRamp

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				ValueFunctionRampLinear
						provides a linear ramp between 2 values

				ValueFunctionRampS
						provides a half-sine-wave (S) ramp between 2 values

******************************************************************************************/

class ValueFunctionRamp : public ValueFunction
{
	CC_DECLARE_DYNAMIC(ValueFunctionRamp);

public:
	ValueFunctionRamp(double StartValue = 1.0, double EndValue = 0.0);
	virtual BOOL IsDifferent(ValueFunction *pOther);

public:		// File format support
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter);

protected:
	double Value1;
	double Value2;
};

// -------------------------------------------------------------------------------------

class ValueFunctionRampLinear : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionRampLinear);

public:
	ValueFunctionRampLinear(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_RampLinear); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionRampS : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionRampS);

public:
	ValueFunctionRampS(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_RampS); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionRampS2 : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionRampS2);

public:
	ValueFunctionRampS2(double StartValue = 1.0, double EndValue = 0.2)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_RampS2); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionRampL : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionRampL);

public:
	ValueFunctionRampL(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_RampL); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionRampL2 : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionRampL2);

public:
	ValueFunctionRampL2(double StartValue = 1.0, double EndValue = 0.2)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_RampL2); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};



// -------------------------------------------------------------------------------------

class ValueFunctionSawTooth : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionSawTooth);

public:
	ValueFunctionSawTooth(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SawTooth); };

	virtual INT32 GetMinimumRecursionDepth();

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionPropeller : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionPropeller);

public:
	ValueFunctionPropeller(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Propeller); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionDoubleRampS : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionDoubleRampS);

public:
	ValueFunctionDoubleRampS(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_DoubleRampS); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionBevelEnds : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionBevelEnds);

public:
	ValueFunctionBevelEnds(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_BevelEnds); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionIntestine : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionIntestine);

public:
	ValueFunctionIntestine(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
					{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Intestine); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};

// -------------------------------------------------------------------------------------

class ValueFunctionDecay : public ValueFunctionRamp
{
	CC_DECLARE_DYNAMIC(ValueFunctionDecay);

public:
	ValueFunctionDecay(double StartValue = 1.0, double EndValue = 0.0)
					: 	ValueFunctionRamp(StartValue, EndValue)
						{};

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);

	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Decay); };

protected:		// File format support
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	
};



///////////////////////////////////////////////////////////////////////////////////////////
// Blips (functions which go from 0.0 to 1.0 and back again)
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	class ValueFunctionBlip : public ValueFunction

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class returns a basic Blip shape:
								 ______
						   __----	   ----__
					____---				     ---____

				Classes derived from this override the "blip" with different shapes

				In all cases, the constructor takes a "maxposition" value which
				indicates at which position the maximum function value occurs,
				as some blips are not symmetrical.

******************************************************************************************/

class ValueFunctionBlip : public ValueFunction
{
	CC_DECLARE_DYNAMIC(ValueFunctionBlip);

public:
	ValueFunctionBlip(double MaxPosition = 0.50);
	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual BOOL IsDifferent(ValueFunction *pOther);

public:		// File format support
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Blip); };

protected:
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	

protected:	// Derived class overrides
	virtual ValueFunctionBlip *CreateInstance(void);

protected:
	double MaxPos;
};



/******************************************************************************************

>	class ValueFunctionTeardrop : public ValueFunctionBlip

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class returns a Teardrop shape:
					  ___
					 /	  ---___
					|			---___

******************************************************************************************/

class ValueFunctionTeardrop : public ValueFunctionBlip
{
	CC_DECLARE_DYNAMIC(ValueFunctionTeardrop);

public:
	ValueFunctionTeardrop(double MaxPosition = 0.20) : ValueFunctionBlip(MaxPosition) {};
	virtual double GetValue(double Position);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Teardrop); };

protected:
	
	virtual ValueFunctionBlip *CreateInstance(void);
};


/******************************************************************************************

>	class ValueFunctionTeardropCurvedEnd : public ValueFunctionBlip

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com> from Jason
	Created:	9/10/2000

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class returns a Teardrop shape with a curved trailing edge

******************************************************************************************/

class ValueFunctionTeardropCurvedEnd : public ValueFunctionBlip
{
	CC_DECLARE_DYNAMIC(ValueFunctionTeardropCurvedEnd);

public:
	ValueFunctionTeardropCurvedEnd(double MaxPosition = 0.20) : ValueFunctionBlip(MaxPosition) {};
	virtual double GetValue(double Position);
	virtual INT32 GetMinimumRecursionDepth();

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_TeardropCurvedEnd); };

protected:
	
	virtual ValueFunctionBlip *CreateInstance(void);
};


/******************************************************************************************

>	class ValueFunctionEllipse : public ValueFunction

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/12/96

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class returns an elliptical arc:
					  ___
					 /	 \
					|	  |	

******************************************************************************************/

class ValueFunctionEllipse : public ValueFunctionBlip
{
	CC_DECLARE_DYNAMIC(ValueFunctionEllipse);

public:
	virtual double GetValue(double Position);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Ellipse); };

protected:
	virtual ValueFunctionBlip *CreateInstance(void);
};



/******************************************************************************************

>	class ValueFunctionThumbtack : public ValueFunction

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/97

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

				This derived class returns a Thumbtack shape built of 2 quarter circles
				back to back:
					  
					   /\
					__-	 -__

******************************************************************************************/

class ValueFunctionThumbtack : public ValueFunctionBlip
{
	CC_DECLARE_DYNAMIC(ValueFunctionThumbtack);

public:
	virtual double GetValue(double Position);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Thumbtack); };

protected:
	virtual ValueFunctionBlip *CreateInstance(void);
};





///////////////////////////////////////////////////////////////////////////////////////////
// ValueFunctionPressure
///////////////////////////////////////////////////////////////////////////////////////////


/******************************************************************************************

>	class ValueFunctionPressure  : public ValueFunction
>	class ValueFunctionPressureS : public ValueFunction

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/97

	Purpose:	Stores Pressure-sensitive pen values (as used for variable-width strokes)
				and allows the caller to retrieve values from such recorded information.

				ValueFunctionPressure linearly interpolates all pressure samples, while
				ValueFunctionPressureS interpolates with an S-shaped curve. (This latter
				one is used for smoothed pressure information, and have a vastly reduced
				storage footprint. See Also kernel\pressure.cpp for the smoothing code)

	SeeAlso:	tools\opfree.cpp (freehand tool pressure recording)

******************************************************************************************/

typedef struct
{
	float Position;			// Pos must be stored as a float - less than 32bits is not enough accuracy
	float Pressure;			// Pressure is stored as a float, as there is a very high probability that
							// the compiler will pad the struct out to 8 bytes anyway, and this reduces
							// the on-the-fly calculation required marginally.
} VFPressureRecord;

class ValueFunctionPressure : public ValueFunction
{
	CC_DECLARE_DYNAMIC(ValueFunctionPressure);

public:		// Overridden virtuals
	ValueFunctionPressure();
	~ValueFunctionPressure();

	virtual double GetValue(double Position);
	virtual ValueFunction *Clone(void);
	virtual BOOL IsDifferent(ValueFunction *pOther);


public:		// Specialised interface
	BOOL AddPressure(float Position, float PressureValue);
			// Appends the given pressure information to the end of the pressure table

	void NormalisePositions(void);
			// Post-processing pass. Normalises all position values on the assumption that
			// the last position entry is the largest position in the array.

public:		// File format support
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter);


	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_Pressure); };

protected:
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	


protected:
	BOOL ExpandArray(INT32 NewSize);
			// Expands the array to contain no fewer than NewSize entries. Returns FALSE if it fails

protected:
	virtual ValueFunctionPressure *CreateInstance(void);
			// Creates a new instance of a Pressure or derived class

protected:
	VFPressureRecord *pArray;		// A dynamic array of pressure information records
	INT32 ArraySize;					// The current (maximum) size of the array
	INT32 NextFree;					// The number of entries used at present (indexes next free space)
};

// -------------------------------------------------------------------------------------

class ValueFunctionPressureS : public ValueFunctionPressure
{
	CC_DECLARE_DYNAMIC(ValueFunctionPressureS);

public:
	virtual double GetValue(double Position);

	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_PressureS); };

protected:
	
	virtual ValueFunctionPressure *CreateInstance(void);
};

/******************************************************************************************

	Author:		Priestley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/2000

	Purpose:	Provides an interface by which the caller can obtain the value of a
				given function at a given "position".

******************************************************************************************/

class ValueFunctionSmoothStroke : public ValueFunction
{
	CC_DECLARE_DYNAMIC(ValueFunctionSmoothStroke);

public:
	ValueFunctionSmoothStroke(double newValue1y, double newValue2x, double newValue2y, double newValue3x, double newValue3y, double newValue4y);
	virtual ~ValueFunctionSmoothStroke();
	virtual BOOL IsDifferent(ValueFunction *pOther);
	virtual INT32 GetMinimumRecursionDepth();
	virtual CamelotFileRecord *WriteFileRecord(INT32 RecordTag, INT32 ExtraBytes, BaseCamelotFilter *pFilter);
	virtual ValueFunction *CreateAndReadFileRecord(CXaraFileRecord *pInputRecord);
	virtual ValueFunctionSmoothStroke *CreateInstance(void);
	virtual double GetValue(double Position);

	void MakeTable();

protected:
	double Value1y;
	double Value2x;
	double Value2y;
	double Value3x;
	double Value3y;
	double Value4y;

	static double xvalue[18][256];
	static double yvalue[18][256];
};



class ValueFunctionSS_Yacht : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Yacht);

public:
	ValueFunctionSS_Yacht(double newValue1y = 0.5, double newValue2x = 0.333, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Yacht(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Yacht); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Yacht); };
	static bool bMadeTable;
};


class ValueFunctionSS_Iron : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Iron);

public:
	ValueFunctionSS_Iron(double newValue1y = 1.0, double newValue2x = 0.333, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Iron(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Iron); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Iron); };
	static bool bMadeTable;
};

class ValueFunctionSS_Reed : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Reed);

public:
	ValueFunctionSS_Reed(double newValue1y = 0.0, double newValue2x = 0.333, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 0.5, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Reed(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Reed); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Reed); };
	static bool bMadeTable;
};

class ValueFunctionSS_Meteor : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Meteor);

public:
	ValueFunctionSS_Meteor(double newValue1y = 0.0, double newValue2x = 0.15, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 0.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Meteor(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Meteor); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Meteor); };
	static bool bMadeTable;
};

class ValueFunctionSS_Petal : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Petal);

public:
	ValueFunctionSS_Petal(double newValue1y = 0.0, double newValue2x = 0.15, double newValue2y = 1.0, double newValue3x = 0.3, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Petal(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Petal); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Petal); };
	static bool bMadeTable;
};


class ValueFunctionSS_Comet : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Comet);

public:
	ValueFunctionSS_Comet(double newValue1y = 0.1, double newValue2x = 0.05, double newValue2y = 1.6, double newValue3x = 0.15, double newValue3y = 0.2, double newValue4y = 0.05)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Comet(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Comet); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Comet); };
	static bool bMadeTable;
};


class ValueFunctionSS_Fallout : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Fallout);

public:
	ValueFunctionSS_Fallout(double newValue1y = 1.0, double newValue2x = 0.333, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.5)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Fallout(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Fallout); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Fallout); };
	static bool bMadeTable;
};

class ValueFunctionSS_Torpedo : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Torpedo);

public:
	ValueFunctionSS_Torpedo(double newValue1y = 1.0, double newValue2x = 0.333, double newValue2y = 0.5, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Torpedo(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Torpedo); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Torpedo); };
	static bool bMadeTable;
};

class ValueFunctionSS_Missile : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Missile);

public:
	ValueFunctionSS_Missile(double newValue1y = 1.0, double newValue2x = 0.333, double newValue2y = 0.0, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Missile(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Missile); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Missile); };
	static bool bMadeTable;
};

class ValueFunctionSS_Convex : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Convex);

public:
	ValueFunctionSS_Convex(double newValue1y = 1.0, double newValue2x = 0.55, double newValue2y = 1.3, double newValue3x = 0.667, double newValue3y = 0.4, double newValue4y = 0.33)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Convex(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Convex); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Convex); };
	static bool bMadeTable;
};

class ValueFunctionSS_Concave : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Concave);

public:
	ValueFunctionSS_Concave(double newValue1y = 1.0, double newValue2x = 0.3, double newValue2y = 0.2, double newValue3x = 0.667, double newValue3y = 0.15, double newValue4y = 0.2)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Concave(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Concave); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Concave); };
	static bool bMadeTable;
};




class ValueFunctionSS_Cigar : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Cigar);

public:
	ValueFunctionSS_Cigar(double newValue1y = 0.4, double newValue2x = 0.333, double newValue2y = 1.3, double newValue3x = 0.667, double newValue3y = 1.3, double newValue4y = 0.4)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Cigar(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Cigar); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Cigar); };
	static bool bMadeTable;
};

class ValueFunctionSS_Cigar2 : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Cigar2);

public:
	ValueFunctionSS_Cigar2(double newValue1y = 0.4, double newValue2x = 0.333, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.4)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Cigar2(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Cigar2); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Cigar2); };
	static bool bMadeTable;
};

class ValueFunctionSS_Cigar3 : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Cigar3);

public:
	ValueFunctionSS_Cigar3(double newValue1y = 0.4, double newValue2x = 0.333, double newValue2y = 0.7, double newValue3x = 0.667, double newValue3y = 0.7, double newValue4y = 0.4)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Cigar3(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Cigar3); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Cigar3); };
	static bool bMadeTable;
};

class ValueFunctionSS_SlimBlip : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_SlimBlip);

public:
	ValueFunctionSS_SlimBlip(double newValue1y = 0.0, double newValue2x = 0.15, double newValue2y = 1.2, double newValue3x = 0.85, double newValue3y = 1.2, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_SlimBlip(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_SlimBlip); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_SlimBlip); };
	static bool bMadeTable;
};

class ValueFunctionSS_OceanLiner : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_OceanLiner);

public:
	ValueFunctionSS_OceanLiner(double newValue1y = 0.5, double newValue2x = 0.333, double newValue2y = 0.5, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_OceanLiner(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_OceanLiner); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_OceanLiner); };
	static bool bMadeTable;
};

class ValueFunctionSS_Goldfish : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Goldfish);

public:
	ValueFunctionSS_Goldfish(double newValue1y = 0.5, double newValue2x = 0.333, double newValue2y = 0.0, double newValue3x = 0.667, double newValue3y = 1.0, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Goldfish(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Goldfish); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Goldfish); };
	static bool bMadeTable;
};

class ValueFunctionSS_Barb : public ValueFunctionSmoothStroke
{
	CC_DECLARE_DYNAMIC(ValueFunctionSS_Barb);

public:
	ValueFunctionSS_Barb(double newValue1y = 0.5, double newValue2x = 0.333, double newValue2y = 1.0, double newValue3x = 0.667, double newValue3y = 0.5, double newValue4y = 0.0)
		: 	ValueFunctionSmoothStroke(newValue1y, newValue2x, newValue2y, newValue3x, newValue3y, newValue4y) { if (!bMadeTable) { MakeTable(); bMadeTable = true; } };

	virtual ValueFunction *Clone(void) { ValueFunction *pClone = new ValueFunctionSS_Barb(Value1y, Value2x, Value2y, Value3x, Value3y, Value4y); return(pClone); };
	virtual ValueFunctionSmoothStroke *CreateInstance(void) { return(new ValueFunctionSS_Barb); };
	virtual ValueFunctionID GetUniqueID(void)	{ return(ValueFunctionID_SS_Barb); };
	static bool bMadeTable;
};


#endif


