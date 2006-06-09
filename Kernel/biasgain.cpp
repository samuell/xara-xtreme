// $Id$
//------------------------------------------------
//  CProfileBiasGain.cpp
//------------------------------------------------
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

//#include "biasgain.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "clamp.h"








/// statics ///

const AFp  CProfileBiasGain::kSmallAmount_s( 0.00001 );








/// standard object services ///////////////////////////////////////////////////////////////////////

///-------------------------------------------------------------------------------------------------
///	Author:		Harrison Ainsworth
///	Date:		05/99
///	Purpose:	Set each member to a default value.
///-------------------------------------------------------------------------------------------------

CProfileBiasGain::CProfileBiasGain ()
	: IProfile()/*,
	  BiasMinus1ToPlus1_m( AFp(0.0) ),
	  GainMinus1ToPlus1_m( AFp(0.0) ),
	  Bias_m( AFp(0.0) ),
	  Gain_m( AFp(0.0) ),
	  DomainLow_m   ( AFp(0.0) ),
	  DomainLength_m( AFp(1.0) ),
	  RangeLow_m    ( AFp(0.0) ),
	  RangeLength_m ( AFp(1.0) )*/
{

	CProfileBiasGain::SetBiasGain( AFp(0.0),  AFp(0.0) );
	CProfileBiasGain::SetDefaultIntervals();
	generatesInfiniteUndo = FALSE;
}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Set the parameters that control the BiasGain mapping.
///	Preconditions:	Both bias and gain inputs are in the interval -1 to +1  [-1,+1]
///-------------------------------------------------------------------------------------------------

CProfileBiasGain::CProfileBiasGain ( AFp BiasMinus1ToPlus1,  AFp GainMinus1ToPlus1 )
	: IProfile()/*,
	  BiasMinus1ToPlus1_m( AFp(0.0) ),
	  GainMinus1ToPlus1_m( AFp(0.0) ),
	  Bias_m( AFp(0.0) ),
	  Gain_m( AFp(0.0) ),
	  DomainLow_m   ( AFp(0.0) ),
	  DomainLength_m( AFp(1.0) ),
	  RangeLow_m    ( AFp(0.0) ),
	  RangeLength_m ( AFp(1.0) )*/
{

	CProfileBiasGain::SetBiasGain( BiasMinus1ToPlus1,  GainMinus1ToPlus1 );
	CProfileBiasGain::SetDefaultIntervals();
	generatesInfiniteUndo = FALSE;
}




CProfileBiasGain::~CProfileBiasGain ()
{
}




CProfileBiasGain::CProfileBiasGain ( const CProfileBiasGain& Other )
	/*: IProfile( Other ),
	  BiasMinus1ToPlus1_m( Other.BiasMinus1ToPlus1_m ),
	  GainMinus1ToPlus1_m( Other.GainMinus1ToPlus1_m ),
	  Bias_m( Other.Bias_m ),
	  Gain_m( Other.Gain_m ),
	  DomainLow_m   ( Other.DomainLow_m ),
	  DomainLength_m( Other.DomainLength_m ),
	  RangeLow_m    ( Other.RangeLow_m ),
	  RangeLength_m ( Other.RangeLength_m )*/
{

	CProfileBiasGain::operator=( Other );

}




CProfileBiasGain&  CProfileBiasGain::operator= ( const CProfileBiasGain& Other )
{

	IProfile::operator=( Other );

	BiasMinus1ToPlus1_m  =  Other.BiasMinus1ToPlus1_m;
	GainMinus1ToPlus1_m  =  Other.GainMinus1ToPlus1_m;
	Bias_m               =  Other.Bias_m;
	Gain_m               =  Other.Gain_m;

	DomainLow_m     =  Other.DomainLow_m;
	DomainLength_m  =  Other.DomainLength_m;
	RangeLow_m      =  Other.RangeLow_m;
	RangeLength_m   =  Other.RangeLength_m;

	return  *this;

}








/// main interface /////////////////////////////////////////////////////////////////////////////////

/// set mapping ------------------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Set the parameters that control the BiasGain mapping.
///	Preconditions:	Both bias and gain inputs are in the interval -1 to +1  [-1,+1]
///-------------------------------------------------------------------------------------------------

void  CProfileBiasGain::SetBiasGain ( AFp BiasMinus1ToPlus1,  AFp GainMinus1ToPlus1 )
{

	SetBias( BiasMinus1ToPlus1 );
	SetGain( GainMinus1ToPlus1 );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Set the parameters that control the BiasGain mapping.
///	Preconditions:	Input is in the interval -1 to +1  [-1,+1]
///-------------------------------------------------------------------------------------------------

void  CProfileBiasGain::SetBias ( AFp BiasMinus1ToPlus1 )
{

	/// enforce precondition
	BiasMinus1ToPlus1  =  ClampBetween( BiasMinus1ToPlus1,  AFp(-1.0),  AFp(+1.0) );


	/// change interval from [-1,+1] to (0,1) in order to satisfy the invariants for Bias_m and Gain_m
	const AFp  BiasZeroToOne( ConvertIntervalMinus1Plus1ToZeroOne( BiasMinus1ToPlus1 ) );


	/// set members
	BiasMinus1ToPlus1_m  =  BiasMinus1ToPlus1;
	Bias_m               =  BiasZeroToOne;

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Set the parameters that control the BiasGain mapping.
///	Preconditions:	Input is in the interval -1 to +1  [-1,+1]
///-------------------------------------------------------------------------------------------------

void  CProfileBiasGain::SetGain ( AFp GainMinus1ToPlus1 )
{

	/// enforce precondition
	GainMinus1ToPlus1  =  ClampBetween( GainMinus1ToPlus1,  AFp(-1.0),  AFp(+1.0) );


	/// change interval from [-1,+1] to (0,1) in order to satisfy the invariants for Bias_m and Gain_m
	const AFp  GainZeroToOne( ConvertIntervalMinus1Plus1ToZeroOne( GainMinus1ToPlus1 ) );


	/// set members
	GainMinus1ToPlus1_m  =  GainMinus1ToPlus1;
	Gain_m               =  GainZeroToOne;

}


AFp  CProfileBiasGain::GetBias ()   const
{

	return  BiasMinus1ToPlus1_m;

}


AFp  CProfileBiasGain::GetGain ()   const
{

	return  GainMinus1ToPlus1_m;

}




/// map zero-one interval --------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Map an input value to an output value, both in the interval 0 to 1.
///	Preconditions:	The input is in the interval 0 to 1  [0,1]
///	Postconditions:	The output is in the interval 0 to 1  [0,1]
///-------------------------------------------------------------------------------------------------

AFp  CProfileBiasGain::MapZeroToOne ( AFp InputZeroToOne )   const
{

	/// enforce preconditions
	InputZeroToOne  =  ClampBetween( InputZeroToOne,  AFp(0.0),  AFp(1.0) );


	/// do the mapping
	AFp  BiasedGained( 0.0 );
	{
		/// special case check - for zero value biasgain, output == input, so avoid calculation
		/// - mainly for accuracy
		if(     ( BiasMinus1ToPlus1_m  !=  AFp(0.0) )
			||  ( GainMinus1ToPlus1_m  !=  AFp(0.0) ) )
		{
			/// really do the mapping
			BiasedGained  =  BiasGain( Bias_m,  Gain_m,  InputZeroToOne );
		}
		else
		{
			BiasedGained  =  InputZeroToOne; 
		}
	}


	return  BiasedGained;

}




/// map general intervals --------------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Set the interval over which the mapping will work.
///	Preconditions:	The interval is not of zero magnitude: High - Low != 0
///-------------------------------------------------------------------------------------------------

void  CProfileBiasGain::SetIntervals ( AFp Low,   AFp High )
{

	SetIntervals( Low, High,  Low, High );

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Set the intervals over which the mapping will work.
///	Preconditions:	Neither interval is of zero magnitude. ___High - ___Low != 0
///-------------------------------------------------------------------------------------------------

void  CProfileBiasGain::SetIntervals ( AFp DomainLow,  AFp DomainHigh,  AFp RangeLow,  AFp RangeHigh )
{

	AFp  DomainLength( DomainHigh - DomainLow );
	AFp  RangeLength(  RangeHigh  - RangeLow  );


	/// enforce preconditions
	if( DomainLength == AFp(0.0) )
	{
		DomainLength  =  kSmallAmount_s;
	}
	if( RangeLength  == AFp(0.0) )
	{
		RangeLength   =  kSmallAmount_s;
	}


	/// set members
	DomainLow_m     =  DomainLow;
	DomainLength_m  =  DomainLength;

	RangeLow_m      =  RangeLow;
	RangeLength_m   =  RangeLength;

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Map an input value to an output value, each in the intervals set with SetIntervals()
///	Preconditions:	The input is in the interval DomainLow to DomainHigh  [DomainLow,DomainHigh]
///	Postconditions:	The output is in the interval RangeLow to RangeHigh  [RangeLow,RangeHigh]
///-------------------------------------------------------------------------------------------------

AFp  CProfileBiasGain::MapInterval ( AFp InputDomain )   const
{

	/// enforce preconditions
	InputDomain  =  ClampBetween( InputDomain,  DomainLow_m,  DomainLow_m + DomainLength_m );


	/// scale the input domain into 0 to 1 interval  [0,1]
	const AFp  InputZeroToOne( ScaleDomainToZeroOne( InputDomain ) );

	/// calculate the 0 to 1 mapping
	const AFp  BiasedGained( MapZeroToOne( InputZeroToOne ) );

	/// scale the output from 0 to 1 interval back to the user-set range
	const AFp  OutputRange( ScaleZeroOneToRange( BiasedGained ) );


	return  OutputRange;

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Fill a table with values mapped from the Domain, all in the intervals set with SetIntervals()
///	Preconditions:	Table points to an array of TableLength length
///	Postconditions:	The output values in Table are in the interval RangeLow to RangeHigh  [RangeLow,RangeHigh]
///-------------------------------------------------------------------------------------------------

void  CProfileBiasGain::MapInterval ( AFp Table[],  INT32 TableLength )   const
{

	/// check precondition
	if( Table != 0 )
	{

		/// set the start-point and increment for the domain
		const AFp  Increment( AFp(1.0) / AFp(TableLength) );
			  AFp  InputZeroOne( 0.0 );


		/// step through domain, calculating a range value to put in each table entry
		for( INT32 Index = 0  ;  Index < TableLength  ;  ++Index  )
		{
			const AFp  BiasedGained( MapZeroToOne( InputZeroOne ) );
			const AFp  OutputRange( ScaleZeroOneToRange( BiasedGained ) );

			Table[ Index ]  =  OutputRange;
			
			InputZeroOne   +=  Increment;
		}

	}

}




/// compare ----------------------------------------------------------------------------------------

bool  CProfileBiasGain::operator== ( const CProfileBiasGain& Other )   const
{

	/// the essence of the class is it's bias and gain parameters, so only compare those
	const bool  IsSame  =      ( Bias_m  ==  Other.Bias_m )
	                       &&  ( Gain_m  ==  Other.Gain_m );

	return  IsSame;

}








/// protected //////////////////////////////////////////////////////////////////////////////////////

/// conversion between intervals -------------------------------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Convert one fp interval into another, in fact [-1,+1] into (0,1)
///					(this function happens to be where the Bias_m and Gain_m invariants are 'defined')
///	Preconditions:	Input value is in the interval [-1,+1]
///	Postconditions:	Output value is in the interval (0,1)
///-------------------------------------------------------------------------------------------------

AFp  CProfileBiasGain::ConvertIntervalMinus1Plus1ToZeroOne ( AFp Minus1ToPlus1 )   /// static
{

	//static const AFp  kShrink( ( AFp(1.0) - kSmallAmount_s ) / AFp(2.0) );   /// (0,1]
	static const AFp  kShrink( ( AFp(1.0) / AFp(2.0) ) - kSmallAmount_s );     /// (0,1)

	const AFp  ZeroToOne( ( Minus1ToPlus1 + AFp(1.0) ) * kShrink + kSmallAmount_s );


	return  ZeroToOne;

}


AFp  CProfileBiasGain::ScaleDomainToZeroOne ( AFp InDomain )   const
{

	/// the invariant demands DomainLength_m != 0, but in case someone has been remiss...
	if( DomainLength_m  !=  AFp(0.0) )
	{
		return  ( InDomain - DomainLow_m ) / DomainLength_m;
	}
	else
	{
		return  AFp( 0.0 );
	}

}

	
AFp  CProfileBiasGain::ScaleZeroOneToRange ( AFp InZeroOne )   const
{

	return   ( InZeroOne * RangeLength_m ) + RangeLow_m;

}

	
	
	




/// the actual function-shape procedural definitions -----------------------------------------------

///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Perform the core BiasGain calculation.
///	Preconditions:	Bias is in the interval (0,1), that is to say: 0 < Bias < 1
///					Gain is in the interval (0,1), that is to say: 0 < Gain < 1
///					Input is in the interval [0,1], that is to say: 0 <= Input <= 1
///	Postconditions:	Return value is in the interval [0,1], that is to say: 0 <= Value <= 1
///-------------------------------------------------------------------------------------------------

AFp  CProfileBiasGain::BiasGain ( AFp bias,  AFp gain,  AFp Input )   /// static
{

	const AFp  Biased( Bias( bias,  Input  ) );
	const AFp  Output( Gain( gain,  Biased ) );

	return  Output;

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Perform the core Bias calculation.
///	Preconditions:	Bias is in the interval (0,1), that is to say: 0 < Bias < 1
///					Input is in the interval [0,1], that is to say: 0 <= Input <= 1
///	Postconditions:	Return value is in the interval [0,1], that is to say: 0 <= Value <= 1
///-------------------------------------------------------------------------------------------------

AFp  CProfileBiasGain::Bias ( AFp Bias,  AFp Input )   /// static
{

	static const AFp  kOne( 1.0 );
	static const AFp  kTwo( 2.0 );

//	const AFp  Output( Input  /  ( ( kOne / Bias - kTwo ) * ( kOne - Input ) + kOne ) );
	const AFp  Output( Input*Bias  /  ( ( kOne - kTwo*Bias ) * ( kOne - Input ) + Bias ) );


	return  Output;

}


///-------------------------------------------------------------------------------------------------
///	Author:			Harrison Ainsworth
///	Date:			05/99
///	Purpose:		Perform the core Gain calculation.
///	Preconditions:	Gain is in the interval (0,1), that is to say: 0 < Gain < 1
///					Input is in the interval [0,1], that is to say: 0 <= Input <= 1
///	Postconditions:	Return value is in the interval [0,1], that is to say: 0 <= Value <= 1
///-------------------------------------------------------------------------------------------------

AFp  CProfileBiasGain::Gain ( AFp Gain,   AFp Input )   /// static
{

	static const AFp  kOne( 1.0 );
	static const AFp  kTwo( 2.0 );
	static const AFp  kHalf( 0.5 );

//	const AFp  CommonPart( ( kOne / Gain - kTwo ) * ( kOne - kTwo * Input ) );
	const AFp  CommonPart( ( kOne - kTwo*Gain ) * ( kOne - kTwo * Input ) );

	if( Input  <  kHalf )
	{
//		return  Input / ( CommonPart + kOne );
		return  Input*Gain / ( CommonPart + Gain );
	}
	else
	{
//		return  ( CommonPart - Input ) / ( CommonPart - kOne );
		return  ( CommonPart - Input*Gain ) / ( CommonPart - Gain );
	}

}




/// private ////////////////////////////////////////////////////////////////////////////////////////

void  CProfileBiasGain::SetDefaultIntervals ()
{

	DomainLow_m     =  AFp( 0.0 );
	DomainLength_m  =  AFp( 1.0 );
	RangeLow_m      =  AFp( 0.0 );
	RangeLength_m   =  AFp( 1.0 );

}
