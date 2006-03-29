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
//

#ifndef  Include_CShadowerFlat
#define  Include_CShadowerFlat

struct IComShadowerFlat;   // defined in the .cpp file

typedef	float			AFloatPt01;
typedef	INT32			APixels;
typedef	INT32*			ACoordPixels;    // array of 2 elements please
typedef	INT32			ADegrees;
typedef	UINT32*	AAlphaChannel;   // AlphaRGB or AlphaBGR array (alpha in the high byte)

/*******************************************************************************

>	class CShadowerFlat

	Updated by: Mark Howitt 3/8/98

	Author:		Harrison_Ainsworth (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/97
	Purpose:	'Wrapper' for the COM interface CShadowerFlat, which is itself
	            a 'Facade' for the flat shadow generation system.
	            Simply delegates to its owned COM component.
	See also:	CShadowerFlat ( in CShadowerFlat.cpp )
	Errors:		(constructor throws if ::CoCreateInstance() fails) not currently.

	Instructions:	Construction:
						Construct, then call _init_ (if possible, rewrite this wrapper
						class, moving the _Init_() into the costructor).
					Setting/getting choices:
						Pass null pointers for the parameters you don't want to change/read.
						The baseline value means:
							position of baseline / height of image
						where:
							position of baseline  =  no. of pixels from top of image
					Switches:
						You might want to change the default init settings in _Init_(),
						The internal defaults are FALSE, FALSE - alpha channel, right way up
						(top left pixel is lowest address)
					Make shadow functions:
						Positions have their origin at the top left of the image.
						Call CalcShadowBound() to get the size and pos of the shadow,
						then allocate your shadow pixmap, then call DrawShadow() to draw the
						shadow.
						The shadow has zeros in the color channels.
					InverseShadowBoundCalc()
						Purpose:	To calculate/estimate the bound of an unshadowed pixmap,
									given the bound of a (hypothetical) shadowed pixmap,
									- the inverse of CalcShadowBound().
						Inputs:		The width, height of a hypothetical pixmap with shadow
									already applied.
						Outputs:	The width, height and position of the unshadowed pixmap,
									that would, after having it's shadow added, take up the
									above input bound.
					CalcShadowBound()
						Purpose:	Calculate the rectangular bound of the shadow that
									would be produced by DrawShadow() given a particular
									transparency/alpha map.
									Call this, use the results to allocate a shadow map,
									then call DrawShadow().
						Inputs:		A transparency/alpha map, it's width, height and position
						Outputs:	The width, height and position of the shadow
					DrawShadow()
						Purpose:	Draw the shadow cast by a particular transparency/alpha map
						Inputs:		A shadow map, it's width, height and position,
									A transparency/alpha map, it's width, height and position
						Outputs:	The shadow drawn onto the shadow map

*******************************************************************************/

class CShadowerFlat
{
public:
	// Standard object management
	CShadowerFlat();
	virtual	~CShadowerFlat();

	// (Should be in constructor)
	virtual	HRESULT _Init_();

	// Set choices
	virtual	VOID SetChoicesToDefault();
	virtual	VOID SetChoices( const BOOL* MakeWallShadow, const AFloatPt01* Darkness01,
							 const APixels* Blurriness, const ACoordPixels  XYPosition,
							 const ADegrees* Tilt, const AFloatPt01* YScale01 );

	virtual	VOID SetBaseline( const AFloatPt01* Baseline );

	// Get choices
	virtual	VOID GetChoices( BOOL* MakeWallShadow, AFloatPt01* Darkness01, APixels* Blurriness,
							 ACoordPixels  XYPosition, ADegrees* Tilt, AFloatPt01* YScale01 ) const;

	virtual	VOID GetBaseline( AFloatPt01* Baseline ) const;

	// Choices range queries
	virtual	VOID GetDarknessRange( AFloatPt01& Min, AFloatPt01& Max ) const;
	virtual	VOID GetBlurrinessRange( APixels& Min, APixels& Max ) const;
	virtual	VOID GetPositionRange( ACoordPixels Min, ACoordPixels Max ) const;

	virtual	VOID GetProjectionRange( ADegrees& MinTilt, ADegrees& MaxTilt,
									 AFloatPt01& MinYScale, AFloatPt01& MaxYScale ) const;
	// Alpha channel type switches
	virtual	HRESULT	SwitchToTransparencyInsteadOfAlpha( BOOL YesOrNo );
	virtual	HRESULT	SwitchToUpsideDownPixelAddressing( BOOL YesOrNo );

	// Make shadow
	virtual	HRESULT	InverseShadowBoundCalc( ACoordPixels UnshadowedWidHei, ACoordPixels UnshadowedPos,
											const ACoordPixels& ShadowedWidHei, const AFloatPt01* Baseline ) const;
	
	virtual	HRESULT	CalcShadowBound( ACoordPixels ShadowWidHei, ACoordPixels ShadowPos, const AAlphaChannel& TransparencyMap,
									 const ACoordPixels& tWidHei, const ACoordPixels& tPos ) const;
	
	virtual	HRESULT	DrawShadow( const AAlphaChannel& ShadowMapOut, const ACoordPixels& sWidHei,
								const ACoordPixels& sPos, const AAlphaChannel& TransparencyMapIn,
								const ACoordPixels& tWidHei, const ACoordPixels& tPos ) const;

protected:

private:
	// ptr to the actual COM component object
	IComShadowerFlat* pShadower_m;

	// COM componenent identifiers
	static const GUID IID_IComShadowerFlat;
	static const GUID CLSID_CComShadowerFlat;

	// prohibit copying
	CShadowerFlat( const CShadowerFlat& );
	CShadowerFlat& operator= ( const CShadowerFlat& );
};

/**************************************************************************************

>	class CCWobJob : public CC_CLASS_MEMDUMP

	Author:
	Created:
	Purpose:

**************************************************************************************/

class CCWobJob : public CC_CLASS_MEMDUMP
{
	// Declare the class for memory tracking
	CC_DECLARE_MEMDUMP(CCWobJob);

};

#endif	//Include_CShadowerFlat
