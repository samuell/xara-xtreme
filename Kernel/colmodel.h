// $Id: colmodel.h 662 2006-03-14 21:31:49Z alex $
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
// colmodel.h - Camelot colour models

// This defines the available colour models and storage structures for them
// The actual conversions between colours in various colur models are handled
// by colour contexts, which incorporate colour models plus conversion modifiers
// such as gamma correction etc. See colcontx.h


#ifndef INC_COLMODEL
#define INC_COLMODEL


#ifndef INC_FIXED24
#include "fixed24.h"
#endif



/********************************************************************************************

< TransType
< TransparencyType

	Comment:	Camelot supports 4 different models for transparency.
				The TransType enumerated type enumerates the 4 possible values:

				TRANSTYPE_NONE, _REFLECTIVE, _SUBTRACTIVE, _ADDITIVE.

				TRANSTYPE_DEFAULT is set to the default value with which all
				Doc/Indexed Colour objects will be initially created. (Current = 1)

	SeeAlso:	DocColour::SetTransparencyType; IndexedColour::SetTransparencyType
		
********************************************************************************************/

typedef enum
{
	TRANSTYPE_MINIMUM 		= 0,

	TRANSTYPE_NONE			= 0,

	TRANSTYPE_DEFAULT 		= 1,

	TRANSTYPE_REFLECTIVE	= 1,
	TRANSTYPE_SUBTRACTIVE	= 2,
	TRANSTYPE_ADDITIVE		= 3,

	TRANSTYPE_MAXIMUM		= 3
} TransType;



/********************************************************************************************

< ColourModel

  Comment:
	Enumerated type of the available colour models. There can be no more
	than 16 models (as references to colour models are 4-bit fields)

	MonoOn
	typedef enum
	{
		COLOURMODEL_INDEXED		= 0,		//	0 Private to Colour system
		COLOURMODEL_NOTCACHED	= 0,		// 	0 Private to Colour system

		COLOURMODEL_CIET,					//	1 CIE coordinate (X,Y,Z), Trans
		COLOURMODEL_RGBT,					//	2 Red, Green, Blue, Trans
		COLOURMODEL_CMYK,					//	3 Cyan, Magenta, Yellow, Key
		COLOURMODEL_HSVT,					//	4 Hue, Saturation, Value, Trans
		COLOURMODEL_GREYT,					//	5 Greyscale intensity, Trans

		MAX_COLOURMODELS					//  6 Number of available models
	} ColourModel;
	MonoOff

  SeeAlso:
  	Keyword MAX_COLOURMODELS
	
********************************************************************************************/


/********************************************************************************************

< MAX_COLOURMODELS

  Comment:
	This contains the number of colour models available for use in Camelot.
	The main use for this is internal to the colour system, for processing all
	colour models, using a loop	of the form:
		MonoOn
		for (INT32 colourmodel = 0; colourmodel < MAX_COLOURMODELS; colourmodel++)
			... Do something relating to colour model 'colourmodel'...;
		MonoOff
		
  SeeAlso:	Keyword ColourModel
	
********************************************************************************************/

typedef enum
{
	COLOURMODEL_INDEXED		= 0,		//	0 Reference to an indexed colour
										//		(ONLY valid for SourceColourModel
										//		field of immediate colours)
	COLOURMODEL_NOTCACHED	= 0,		// 	0 Indicator that the cache is invalid
										//		(ONLY valid for CacheColourModel
										//		field of immediate colours)

	COLOURMODEL_CIET,					//	1 CIE coordinate (X,Y,Z), Transparent
	COLOURMODEL_RGBT,					//	2 Red, Green, Blue, Transparent
	COLOURMODEL_CMYK,					//	3 Cyan, Magenta, Yellow, Key
	COLOURMODEL_HSVT,					//	4 Hue, Saturation, Value, Transparent
	COLOURMODEL_GREYT,					//	5 Greyscale intensity, Transparent
	COLOURMODEL_WEBRGBT,				//	6 Same as COLOURMODEL_RGBT; but allows us
										//  to (correctly) provide a new ColourContext
										//	that can be selected within the colour editor
										//	for hex input (but still functions internally
										//	as a COLOURMODEL_RGBT

	// Insert new colour models here...

	MAX_COLOURMODELS					// This is automagically set to the
										// number of available models.
										// You MAY:
										//   for (cm=0; cm<MAX_COLOURMODELS; cm++)
										// You may NOT:
										//   for (cm=0; cm<16; cm++)
} ColourModel;



/////////////////////////////////////////////////////////////////////////////////
// Colour definition structures
// These encode a colour in one of the available colour models
//
// Two types of colour definition exist:
// PColourABCD:		Packed Colour	- Must fit within  32 bits of memory
//					These are typically built of up to 4 8-bit ColourValues
//
// ColourABCD:		Normal colour	- Must fit within 128 bits of memory
//					These are typically built of up to 4 32-bit PColourValues
//
// NOTE however, that some models (e.g. HSVT) choose to divide the available
// bits up in a different fashion to the typical colour models.
//
// IMPORTANT NOTE!!
//   It is essential that all ColourXXXX structures are exactly 4 ColourValues
//   wide, and that any 'Transparent' component is component 4, so that they
//   are easily interchangable.


typedef UINT32	PColourValue;	// 4 8-bit PColourValues make a PColour
typedef FIXED24 		ColourValue;	// 4 32-bit ColourValues make an Colour



/********************************************************************************************

< ColourRGBT

  Comment:
	Components = Red, Green, Blue, Transparent

	This structure defines a colour in terms of RGBT components.

	Each component is represented as an 8.24 bit fixed point fraction between
	0.0 (0x00000000) and 1.0 (0x01000000).
	

  SeeAlso:	ColourModel; ColourGeneric; ColourCMYK; ColourHSVT;
			ColourGreyT; ColourCIET; IndexedColour
	
********************************************************************************************/

typedef struct							// --- RGBT ---
{
	PColourValue Red			: 8;
	PColourValue Green			: 8;
	PColourValue Blue			: 8;
	PColourValue Transparent	: 8;
} PColourRGBT;

typedef struct
{
	ColourValue Red;
	ColourValue Green;
	ColourValue Blue;
	ColourValue Transparent;
} ColourRGBT;

/********************************************************************************************

< ColourCMYK

  Comment:
	Components = Cyan, Magenta, Yellow, Key

	This structure defines a colour in terms of CMYK components.

	Each component is represented as an 8.24 bit fixed point fraction between
	0.0 (0x00000000) and 1.0 (0x01000000).
	

  SeeAlso:	ColourModel; ColourGeneric; ColourRGBT; ColourHSVT;
			ColourGreyT; ColourCIET; IndexedColour
	
********************************************************************************************/

typedef struct							// --- CMYK ---
{
	PColourValue Cyan			: 8;
	PColourValue Magenta		: 8;
	PColourValue Yellow			: 8;
	PColourValue Key			: 8;
} PColourCMYK;

typedef struct
{
	ColourValue Cyan;
	ColourValue Magenta;
	ColourValue Yellow;
	ColourValue Key;
} ColourCMYK;


/********************************************************************************************

< ColourHSVT

  Comment:
	Components = Hue, Saturation, Value, Transparent

	This structure defines a colour in terms of HSVT components.

	Each component is represented as an 8.24 bit fixed point fraction between
	0.0 (0x00000000) and 1.0 (0x01000000).
	

  SeeAlso:	ColourModel; ColourGeneric; ColourRGBT; ColourCMYK;
			ColourGreyT; ColourCIET; IndexedColour
	
********************************************************************************************/

typedef struct							// --- HSVT ---
{
	PColourValue Saturation		: 8;
	PColourValue Value			: 8;
	PColourValue Hue			: 9;
	PColourValue Transparent	: 7;
} PColourHSVT;

typedef struct
{
	ColourValue Hue;
	ColourValue Saturation;
	ColourValue Value;
	ColourValue Transparent;
} ColourHSVT;


/********************************************************************************************

< ColourGreyT

  Comment:
	Components = Intensity, Transparent	

	This structure defines a colour in terms of IT components.

	Each component is represented as an 8.24 bit fixed point fraction between
	0.0 (0x00000000) and 1.0 (0x01000000).
	

  SeeAlso:	ColourModel; ColourGeneric; ColourRGBT; ColourCMYK; ColourHSVT;
			ColourCIET; IndexedColour
	
********************************************************************************************/

typedef struct							// --- GreyT ---
{
	PColourValue Intensity		: 8;
	PColourValue Reserved		: 16;
	PColourValue Transparent	: 8;
} PColourGreyT;

typedef struct
{
	ColourValue Intensity;
	ColourValue Reserved1;
	ColourValue Reserved2;
	ColourValue Transparent;
} ColourGreyT;



/********************************************************************************************

< ColourCIET

  Comment:
	Components = X, Y, Z, Transparent

	This structure defines a colour in terms of XYZT components.
	The X,Y,Z define a point in the CIE colourspace.

	Each component is represented as an 8.24 bit fixed point fraction between
	0.0 (0x00000000) and 1.0 (0x01000000).
	

  SeeAlso:	ColourModel; ColourGeneric; ColourRGBT; ColourCMYK; ColourHSVT;
			ColourGreyT; ColourCIET; IndexedColour
	
********************************************************************************************/

typedef struct							// --- CIET ---
{
	PColourValue X				: 8;
	PColourValue Y				: 8;
	PColourValue Z				: 8;
	PColourValue Transparent	: 8;
} PColourCIET;

typedef struct
{
	ColourValue X;
	ColourValue Y;
	ColourValue Z;
	ColourValue Transparent;
} ColourCIET;

typedef struct
{
	double X;
	double Y;
	double Z;
	double Transparent;
} DColourCIET;

class IndexedColour;
typedef struct							// --- Indexed ---
{
	IndexedColour *Colour;
} PColourIndexed;





/********************************************************************************************

< ColourGeneric

  Comment:
	Components = Component1, Component2, Component3, Component4

	This structure defines a colour in terms of 4 generic components.

	It is only to be used internally to the colour system to provide a generic
	means of conversion between 8-bit and 32-bit component representations.
	(Note that this means colours such as HSVT cannot be converted via the
	generic system, as they don't use generic 8-bit fields)
	

  SeeAlso:	ColourModel; ColourGeneric; ColourRGBT; ColourCMYK; ColourHSVT;
			ColourGreyT; ColourCIET; IndexedColour
	
********************************************************************************************/

typedef struct							// --- CIET ---
{
	PColourValue Component1		: 8;
	PColourValue Component2		: 8;
	PColourValue Component3		: 8;
	PColourValue Component4		: 8;
} PColourGeneric;

typedef struct
{
	ColourValue Component1;
	ColourValue Component2;
	ColourValue Component3;
	ColourValue Component4;
} ColourGeneric;




/////////////////////////////////////////////////////////////////////////////////
// Packed Colour Block
// This is a union of all Normal (32-bit) colour definition structures

typedef union
{
//	PColourGeneric	Generic;	-- This is an explicit cast, only for internal use
	PColourRGBT		RGBT;
	PColourCMYK		CMYK;
	PColourHSVT		HSVT;
	PColourGreyT	GreyT;
	PColourCIET		CIET;
	PColourIndexed	Indexed;
} ColourPacked;




#if FALSE
	// The following can't be used (as we are using FIXED24, a class, the
	// union gets upset about it having default constructors and stuff, making
	// it difficult/impossible to use. Seeing no particular need for a ColourGeneric
	// other than making the colour-class internals neater, I've decided to
	// discontinue it - if it becomes absolutely necessary in the future, a
	// ColourGeneric class with special access methods can be made.

	// The internal structure used to store the info is now a ColourGeneric.


/********************************************************************************************

 ColourDefn

Comment :
	This union contains the definition of a single colour in one of the available
	colour models. The data is stored in 4 model-specific components, each of which
	is stored as an 8.24-bit fixed-point fractional value, i.e.
		0.0 is represented as	0x00000000
		0.5 					0x007fffff
		1.0 					0x01000000
	Colours are described and processed in this format wherever possible (although a
	low-precision packed form is available for rendering etc - see colmodel.h - it is
	desirable that we use proper precision in colour calculations)
	

See Also:	ColourModel; ColourRGBT; ColourCMYK; ColourHSVT;
			ColourGreyT; ColourCIET; IndexedColour
	
********************************************************************************************/

typedef union
{
//	ColourGeneric	Generic;	-- This is an explicit cast, only for internal use
	ColourRGBT		RGBT;
	ColourCMYK		CMYK;
	ColourHSVT		HSVT;
	ColourGreyT		GreyT;
	ColourCIET		CIET;
		//			Indexed		-- Extended colours have no 'Indexed' form
} ColourDefn;
#endif		// if FALSE



#endif		// INC_COLMODEL

