// $Id: epsstack.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_EPSSTACK_H
#define INC_EPSSTACK_H

#include "fixst256.h"
#include "list.h"
#include "doccoord.h"
#include "doccolor.h"

class StringBase;

// Special class for floating point values held as fixed point (would be a typedef except
// we then can't overload INT32s and FIXEDPOINTs).
class FIXEDPOINT
{
public:
	INT32 Long;
	void FromAscii(const char *FltPtString);
	operator INT32() const { return (INT32) Long; }
	FIXEDPOINT& operator= (INT32 Val) { this->Long = Val; return *this; }
};

// The scaling factor used to store fixed point values on the EPSStack.
const INT32 FixedPointScale = 1000;

// EPSType enumerated type - all the kinds of objects that can be pushed onto the stack
typedef enum
{
	EPSTYPE_EMPTY,
	EPSTYPE_NONE,
	EPSTYPE_INT32,
	EPSTYPE_FIXEDPOINT,
	EPSTYPE_DOUBLE,
	EPSTYPE_STRING,
	EPSTYPE_NAME,
	EPSTYPE_COMMAND
} EPSType;

typedef enum
{
	TINT_NONE,
	TINT_ILLUSTRATOR,
	TINT_COREL
} TintType;

// Forward reference for the EPSStack class
class EPSStackItem;
class EPSFilter;

typedef UINT32 EPSCommand;

/********************************************************************************************

>	class EPSStack : public List

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/93
	Purpose:	Provides a heterogeneous stack for use when reading PostScript (EPS) files.
				Graeme (30/5/00) - I've replaced the array implementation that Tim originally
				used with a linked list. The array was originally there as a performance
				tweak, but increased CPU power has made this a less perceiveable gain. By
				using a linked list, I've made the implementation a little more error
				tolerant, which means that files containing stranglely placed bitmaps are
				now loaded.
	SeeAlso:	EPSInputFilter; EPSStackItem

********************************************************************************************/

class EPSStack : public List
{
	CC_DECLARE_MEMDUMP(EPSStack)
	
public:
	EPSStack();
	~EPSStack();
	
	BOOL Push(const INT32);
	BOOL Push(const FIXEDPOINT);
	BOOL Push(const double);
	BOOL Push(const char*, BOOL IsName = FALSE);
	BOOL Push(const EPSCommand Cmd);
	
	BOOL Pop(INT32 *);
	BOOL Pop(UINT32 *);
	BOOL Pop(FIXEDPOINT *);
	BOOL Pop(double *);
	BOOL Pop(char*);
	BOOL Pop(StringBase*);

	BOOL PopCmd(EPSCommand*);
	BOOL PopCoord(INT32*);
	BOOL PopCoordPair(DocCoord*);
	BOOL PopColour(PColourCMYK *, 
				   TintType Tint = TINT_NONE, 
				   FIXEDPOINT *TintVal = NULL,
				   String_64 *pName = NULL);
		// (ChrisG 12/12/00) - Added for EPS support of RGB colours
	BOOL PopColourRGB (INT32 * red, INT32 * green, INT32 * blue,
					   TintType Tint = TINT_NONE, 
					   FIXEDPOINT *TintVal = NULL,
					   String_64 *pName = NULL);
	BOOL PopGrayScale(PColourCMYK *);

	BOOL Pop(Matrix *, BOOL);

	BOOL PopArray(INT32*, INT32*);
	
	BOOL Discard(UINT32 NumToDiscard = 1, BOOL EvenCommands = FALSE);
	BOOL DiscardArray();

	void SetCoordScaleFactor(INT32 NewScaleFactor);
	void SetCoordOrigin(DocCoord NewOrigin);
	void TranslateCoordOrigin(MILLIPOINT dx, MILLIPOINT dy);

	void SetXformMatrix(Matrix&);
	void SetNoXformMatrix();
	
	EPSType GetType();
	EPSCommand ReadCmd();
	
	void Dump(EPSFilter *);

	BOOL IsEmpty();
	
private:
	// Member variables.
	INT32 ScaleFactor;
	DocCoord Origin;

	Matrix XformMatrix;
	BOOL UseXformMatrix;
};

#endif

