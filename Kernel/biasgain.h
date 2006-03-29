// $Id$
//------------------------------------------------
//  CProfileBiasGain.h
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




#ifndef  Include_CProfileBiasGain
#define  Include_CProfileBiasGain




#include "iprofile.h"












///-------------------------------------------------------------------------------------------------
///
///	class CProfileBiasGain
///
///	Author:     Harrison Ainsworth
///	Date:		05/99
///	Purpose:	Implementation of the IProfile interface as Perlin Bias and Gain: Bias resembling a 
///				gamma function, Gain resembling a sigma function, each being positive or negative. 
///				Together they provide a variety of simple and useful shapes.
///				references:
///				 Hypertexture - Perlin, Hoffert - Siggraph conference proceedings 1989
///				 Fast Alternatives to Perlin's Bias and Gain Functions - Schlick - Graphics Gems IV (AP 94)
///	Implemen-	There are two sub-interfaces: construction/setting and IProfile/mapping:
///	tation:			'Setting' comprises the standard object services, and the choice of the Bias and 
///				Gain parameters.
///					'Mapping' comprises the methods set out in IProfile: for using the object to map 
///				values.
///	Invariants:	0 < Bias_m < 1,                     0 < Gain_m < 1                     (0,1)
///				-1 <= BiasMinus1ToPlus1_m <= +1,    -1 <= GainMinus1ToPlus1_m <= +1    [-1,+1]
///				DomainLength_m != 0,  RangeLength_m != 0
///
///-------------------------------------------------------------------------------------------------

class CProfileBiasGain  :  public IProfile
{

public: ///-----------------------------------------------------------------------------------------

	/// standard object services -------------------------------

								CProfileBiasGain ();
								CProfileBiasGain ( AFp BiasMinus1ToPlus1,  AFp GainMinus1ToPlus1 );

								~CProfileBiasGain ();
								CProfileBiasGain ( const CProfileBiasGain& );
			CProfileBiasGain&	operator= ( const CProfileBiasGain& );

	/// --------------------------------------------------------



	/// set/get mapping
	///		Each parameter must be in the interval -1 to +1  [-1,+1]
	///		Bias:  -1 'thickest',  0 'flat',  +1 'thinnest'
	///		Gain:  -1 'sharpest',  0 'flat',  +1 'softest'
	virtual	void		SetBiasGain ( AFp BiasMinus1ToPlus1,  AFp GainMinus1ToPlus1 );
	virtual	void		SetBias ( AFp BiasMinus1ToPlus1 );
	virtual	void		SetGain ( AFp GainMinus1ToPlus1 );

	virtual	AFp			GetBias ()															const;
	virtual	AFp			GetGain ()															const;


	/// IProfile -----------------------------------------------
	/// map zero-one interval
	virtual	AFp			MapZeroToOne ( AFp ZeroToOne )										const;

	/// map general intervals
	virtual	void		SetIntervals ( AFp Low,  AFp High );
	virtual	void		SetIntervals ( AFp DomainLow,  AFp DomainHigh,  AFp RangeLow,  AFp RangeHigh );

	virtual	AFp			MapInterval ( AFp )													const;
	virtual	void		MapInterval ( AFp Table[],  INT32 length )							const;

	/// compare
	virtual	bool		operator== ( const CProfileBiasGain& )								const;

	void SetGeneratesInfiniteUndo (BOOL newVal) { generatesInfiniteUndo = newVal; }
	BOOL GetGeneratesInfiniteUndo () const { return (generatesInfiniteUndo); }

	void SetIsAFeatherProfile (BOOL newVal) { isAFeatherProfile = newVal; }
	BOOL GetIsAFeatherProfile () { return (isAFeatherProfile); }


protected: ///--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--

	/// conversion between intervals
	static	AFp			ConvertIntervalMinus1Plus1ToZeroOne ( AFp );
	        AFp			ScaleDomainToZeroOne ( AFp )										const;
	        AFp			ScaleZeroOneToRange  ( AFp )										const;

	/// the actual function-shape procedural definitions
	static	AFp			BiasGain ( AFp BiasAmount,  AFp GainAmount,  AFp Input );
	inline static	AFp			Bias ( AFp BiasAmount,  AFp Input );
	inline static	AFp			Gain ( AFp GainAmount,  AFp Input );	


private: ///////////////////////////////////////////////////////////////////////////////////////////

	/// functions ----------------------------------------------
	/// construction assistance
			void		SetDefaultIntervals ();


	/// members ------------------------------------------------
	/// parameters of the mapping
			AFp			BiasMinus1ToPlus1_m;
			AFp			GainMinus1ToPlus1_m;
			AFp			Bias_m;
			AFp			Gain_m;

	/// bounds of the mapping
			AFp			DomainLow_m;
			AFp			DomainLength_m;
			AFp			RangeLow_m;
			AFp			RangeLength_m;

	/// constants
	static const AFp	kSmallAmount_s;

	/// CGS:  when we compound a blend and change the the blends profile, we need to know whether the blend
	/// allows its parent compound to regenerate.  This is controlled by the following variable, which is
	/// guaranteed to be set CBiasGainGadget::Message () returns (for dropdown) and HandleInput () is called
	/// (for sliders).  This is the only sensible place to store the data - since it needs to be accessible
	/// (potentially) within all tool infobars
	
	BOOL generatesInfiniteUndo;			// DO NOT SET THIS VALUE YOURSELF !!!!

	BOOL isAFeatherProfile;				// CGS:  we need to do some special processing for profiles that are
										// used with feathering.
};








#endif//Include_CProfileBiasGain
