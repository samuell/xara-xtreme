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
// The Transformation Base Classes

/*
*/


#ifndef TRANFORM_INC
#define TRANFORM_INC

#include "ccobject.h"


class DocCoord;

/********************************************************************************************

>	class TransformBase : public CCObject

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/02/94
	Purpose:	The Transformation Base Class. This is an abstract class - you can only create
				instances of its derived classes. The Transformation classes are used to
				give a transparent interface to arbitary transformations on objects. Nodes
				do not need to be aware of what type of transformation is being applied or
				how is should be undone.
				This class contains two virtual function :-
				MonoOn virtual void Transform(DocCoord* Coords, INT32 NumCoords) MonoOff
				MonoOn virtual BOOL IsInvertable() MonoOff
				It also contains the following public member vars
				MonoOn BOOL TransLines;		// TRUE if Line Widths are to be transformed MonoOff
				MonoOn BOOL TransFills;		// TRUE if Fills are to be transformed MonoOff
	SeeAlso:	TransInvertable; TransNonInvertable

********************************************************************************************/

class TransformBase : public CCObject
{
CC_DECLARE_DYNAMIC(TransformBase);

public:
	TransformBase()
	{
		TransFills					= TRUE;
		TransLines					= TRUE;
		bSolidDrag					= FALSE;
		bTransformYourChildren		= TRUE;
		bHaveTransformedChildren	= TRUE;
		bHaveTransformedCached		= TRUE;
		bHaveTransformedAllChildren	= FALSE;
		bHaveTransformedAllCached	= FALSE;
	};
	virtual void Transform( DocCoord* Coords, INT32 NumCoords) = 0;
	virtual BOOL IsInvertable() = 0;
	virtual BOOL IsTranslation() {return FALSE;}

	// Other functions
	virtual FIXED16 GetScalar();

// vars
public:
	BOOL TransFills : 1;					// Should the Fills be transformed
	BOOL TransLines : 1;					// Should the line widths be transformed
	BOOL bSolidDrag : 1;					// Should the object update itself fully so it can be rendered?

	BOOL bTransformYourChildren : 1;		// Should the object transform all its children (it might not need to if it can transform its cached data)
	BOOL bHaveTransformedChildren : 1;		// Feedback from Node: Node is fully transformed, including all children
	BOOL bHaveTransformedCached : 1;		// Feedback from Node: Node has transformed its cached data

	BOOL bHaveTransformedAllChildren : 1;	// Feedback from Iterator: All iterated nodes returned bHaveTransformedChildren;
	BOOL bHaveTransformedAllCached : 1;		// Feedback from Iterator: All iterated nodes returned bHaveTransformedCached;
};




/********************************************************************************************

>	class TransInvertable : public TransformBase

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/02/94
	Purpose:	One of the transformation base classes. All Transformations derived from this
				class can be inverted directly. ie Transforms that make use of a matrix can
				be inverted by inverting the matrix.
				This class adds the following virtual functions :-
				MonoOn virtual void Invert() MonoOff
	SeeAlso:	Trans2DMatrix

********************************************************************************************/

class TransInvertable : public TransformBase
{
CC_DECLARE_DYNAMIC(TransInvertable);

public:
	TransInvertable(){};
	virtual BOOL IsInvertable() { return TRUE; }
	virtual void Invert() = 0;
};




/********************************************************************************************

>	class TransNonInvertable : public TransformBase

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/02/94
	Purpose:	One of the Transformation base classes. All the transformations derived from
				this class can not be inverted. Transforms of this kind are considered to be
				one way mappings from the original coords. There is no way of getting the
				original coords back after they have been transformed by a class derived from
				this one. Examples include the Envelope transformation

********************************************************************************************/

class TransNonInvertable : public TransformBase
{
CC_DECLARE_DYNAMIC(TransNonInvertable);

public:
	TransNonInvertable(){};
	virtual BOOL IsInvertable() { return FALSE; }
};



#endif	// TRANFORM_INC


