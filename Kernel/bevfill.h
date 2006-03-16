// $Id: bevfill.h 662 2006-03-14 21:31:49Z alex $
//////////////////////////////////////////////
// The bevelling fill profile class
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

#ifndef _BEVFILL_H
#define _BEVFILL_H

/******************************************************************************************

>	class CBevelFill : public SimpleCCObject

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Abstract base class for all fill types

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFill : public CCObject
{
	CC_DECLARE_DYNAMIC(CBevelFill);


public:
	// Pos is between 0 (i.e. on the bevel path) and 1 (i.e. on the inner/outer path)
	virtual COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast) = 0;

	// ensures that the value is in the range 1 to 254 to make
	// sure it can be cast as a BYTE
	void EnsureValueRange(double *value);
} ;

/******************************************************************************************

>	class CBevelFillFlat : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Do a flat fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillFlat : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillFlat);


public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;


/******************************************************************************************

>	class CBevelFillRound : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Do a rounded fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillRound : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillRound);

public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;


/******************************************************************************************

>	class CBevelFillSquare : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Do a rounded fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillSquare : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillSquare);

public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;

/******************************************************************************************

>	class CBevelFillFrame : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Do a rounded fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillFrame : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillFrame);

public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;

/******************************************************************************************

>	class CBevelFillSmooth : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Do a smoothed fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillSmooth : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillSmooth);

public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;

/******************************************************************************************

>	class CBevelFillPoint : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/99

	Purpose:	Do a pointy fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillPoint : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillPoint);

public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;

/******************************************************************************************

>	class CBevelFillRuffle : public CBevelFill

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Do a ruffle fill

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFillRuffle : public CBevelFill
{
	CC_DECLARE_DYNAMIC(CBevelFillRuffle);

public:
	COLORREF CalcPixelValue(BYTE value, double Pos, INT32 Contrast);
} ;

/******************************************************************************************

>	class CBevelFills : public SimpleCCObject

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/10/98

	Purpose:	Encapsulates all the bevel filling stuff

	SeeAlso:	CBeveler

******************************************************************************************/

class CBevelFills : public CCObject
{
	CC_DECLARE_DYNAMIC(CBevelFills);

public:
	CBevelFills();
	~CBevelFills();

	COLORREF CalcPixelValue(BYTE value, INT32 bevType, double Pos, INT32 Contrast);
private:
	CBevelFill **m_pBevFills;

	// set up the above
	BOOL SetUpFills();

	INT32 m_NumFills;
} ;





#endif
