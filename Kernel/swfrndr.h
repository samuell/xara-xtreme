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
#ifndef INC_FLASH_RENDER_H
#define INC_FLASH_RENDER_H

#include "fillramp.h"
#include "rndrgn.h"
#include "vectrndr.h"
#include "nodetext.h"
#include "nodebev.h"
#include "nodeshad.h"
#include "swfbitmp.h"
#include "swftext.h"
#include "swffont.h"
#include "swfshape.h"
#include "swfbuttn.h"
#include "swfplace.h"
#include "swfsprit.h"

// The following functions perform the same role as many of the macros defined in the
// older Camelot code, but since they're based around templates, incorporate type
// checking, and are thus safer to use.

/********************************************************************************************

>	template < class T > T Max ( T InputA, T InputB )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Replacement for the Max macro. It's type safe, and can operate on any C++
				data type.
	Returns:	The maximum of either InputA or InputB.
	SeeAlso:	-

********************************************************************************************/

template < class T > T Max ( T InputA, T InputB )
{
	if ( InputA > InputB )
		return InputA;
	else
		return InputB;
}

/********************************************************************************************

>	template < class T > T Max ( T InputA, T InputB, T InputC, T InputD )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Wrapper around the Max function to return the maximum value from four
				values. (Used for finding the maximum value for the extends of a bounding
				rectangle.
	Returns:	The maximum value of the four inputs.
	SeeAlso:	-

********************************************************************************************/

template < class T > T Max ( T InputA, T InputB, T InputC, T InputD )
{
	return Max ( Max ( InputA, InputB ), Max ( InputC, InputD ) );
}

/********************************************************************************************

>	template < class T > T Min ( T InputA, T InputB )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Purpose:	Replacement for the Min macro. It's type safe, and can operate on any C++
				data type.
	Returns:	The minimum of either InputA or InputB.
	SeeAlso:	-

********************************************************************************************/

template < class T > T Min ( T InputA, T InputB )
{
	if (  ( InputA ) <  ( InputB ) )
		return InputA;
	else
		return InputB;
}

/********************************************************************************************

>	template < class T > T Min ( T InputA, T InputB, T InputC, T InputD )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/11/99
	Purpose:	Wrapper around the Min function to return the minimum value from four
				values. (Used for finding the minimum value for the extends of a bounding
				rectangle.
	Returns:	The minimum value of the four inputs.
	SeeAlso:	-

********************************************************************************************/

template < class T > T Min ( T InputA, T InputB, T InputC, T InputD )
{
	return Min ( Min ( InputA, InputB ), Min ( InputC, InputD ) );
}

/********************************************************************************************

>	template < class T > T Absol ( T a )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Replacement for the Abs macro. It's got the advantage of being easier
				to debug, and fully type safe.
	Returns:	The maximum of either a or b.
	SeeAlso:	-

********************************************************************************************/

template < class T > T Absol ( T a )
{
	if ( a < 0 )
		return -a;
	else
		return a;
}

/********************************************************************************************

>	class FlashRenderRegion : public VectorFileRenderRegion

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/8/99
	Purpose:	Provides the necessary functions to convert image data stored in the
				Camelot tree into the correct values for exporting to the Flash 4 format,
				and output these to the relevant device context.
	
********************************************************************************************/

class FlashRenderRegion : public VectorFileRenderRegion
{
public:
	CC_DECLARE_DYNAMIC( FlashRenderRegion )
 
	// Constructors.
	FlashRenderRegion ( void );
	FlashRenderRegion ( DocRect ClipRect,
						Matrix ConvertMatrix,
						FIXED16 ViewScale );

	// Destructor.
	~FlashRenderRegion( void );

	// Normal functions.
	BOOL ExportCharacter ( TextChar *pTheLetter );
	BOOL ExportBevel ( NodeBevel *pBevel );
	BOOL ExportBevelBegin ( NodeBevelBegin *pBevel );
	BOOL ExportShadow ( Path *pShadowPath,
						OILBitmap *pBitmap,
					    UINT32 Darkness );
	BOOL ExportRenderableNode ( NodeRenderableInk *pInk );

	BOOL ExportAll ( void );

	// These set layer and button IDs used to export roll-over states.
	void SetButtonName ( TCHAR *pButtonName );
	void SetLayerState ( LayerState State );

protected:

	void DrawPathToOutputDevice ( Path *PathToRender,
								  PathShape shapePath = PATHSHAPE_PATH );

	// Extra functionality.
	void InitPointers ( void );

	BOOL ProcessPath ( Path *pPath );
	BOOL ProcessFill ( FillGeometryAttribute *pFill,
					   TranspFillAttribute *pTransparency );
	BOOL ProcessColour ( FillGeometryAttribute *pFill );
	BOOL ProcessTransparency ( TranspFillAttribute *pTransparency,
							   FillGeometryAttribute *pFill );
	BOOL CreateTextRecord ( TextChar *pTheLetter );
	BOOL ProcessFlatFill ( DocColour *pColour );
	BOOL ProcessGradientFill ( DocColour *pStartColour,
							   DocColour *pMidColour,
							   DocColour *pEndColour,
							   ColourRamp *pRamp,
							   DocCoord *pStartPoint,
							   DocCoord *pEndPoint,
							   DocCoord *pEndPoint2,
							   BYTE FillType );
	BOOL ProcessBitmapFill ( FillGeometryAttribute *pFill );
	BOOL RecordBitmapFill ( BitmapFillAttribute *pFill,
							WORD BitmapID );
	WORD GetBitmapID ( OILBitmap *pBitmap,
					   DocColour *pStartColour,
					   DocColour *pEndColour,
					   UINT32		 Transparency );
	BOOL ProcessFlatTransp ( UINT32 Transparency );
	BOOL ProcessGradientTransp ( UINT32 *pStartTransparency,
								 UINT32 *pMidTransparency,
								 UINT32 *pEndTransparency,
								 DocCoord *pStartPoint,
								 DocCoord *pEndPoint,
								 DocCoord *pEndPoint2,
								 BYTE FillType );
	BOOL ProcessBitmapTransp ( TranspFillAttribute *pTransparency,
							   FillGeometryAttribute *pFill );

	UINT32 BlendTransparencies ( UINT32 *pStart,
							   UINT32 *pEnd1,
							   UINT32 *pEnd2,
							   UINT32 *pEnd3 );

	DocColour BlendColours ( DocColour *pStart,
							 DocColour *pEnd1,
							 DocColour *pEnd2,
							 DocColour *pEnd3,
							 ColourRamp *pRamp );

	FlashButtonRecord* ProcessURL ( WebAddressAttribute *pWebAddress );

	BOOL ExportButtonState ( FlashButtonRecord *pButton );

	FlashButtonRecord* MatchButton ( void );

	FlashSprite* MatchSprite ( FlashButtonRecord *pButton );

	BOOL PlaceObject ( WORD ID,
					   FlashType ToRender,
					   DocCoord *pPosition = NULL,
					   BOOL DoTransform = FALSE );

	// Member variables.
	FlashBitmapRecord	*mpBitmap;
	FlashBitmapRecord	*mpBitmapTail;
	FlashTextRecord		*mpText;
	FlashTextRecord		*mpTextTail;
	FlashFontRecord		*mpFont;
	FlashFontRecord		*mpFontTail;
	FlashShapeRecord	*mpShape;
	FlashShapeRecord	*mpShapeTail;
	FlashShapeRecord	*mpShapeLast;
	FlashButtonRecord	*mpButton;
	FlashButtonRecord	*mpButtonTail;
	FlashSprite			*mpSprite;
	FlashSprite			*mpSpriteTail;
	FlashPlaceObject	*mpPlace;
	FlashPlaceObject	*mpPlaceTail;
	String_256			mButtonName;
	LayerState			mButtonState;
	BOOL				mSupressLines;
	BOOL				mExportStroke;
};

#endif	// #ifndef
