// $Id: opflip.cpp 662 2006-03-14 21:31:49Z alex $
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
// Flip Operation

/*
*/

#include "camtypes.h"
#include "opflip.h"
//#include "rik.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNCREATE(OpFlipTrans, TransOperation)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW




/********************************************************************************************

>	OpFlipTrans::OpFlipTrans()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/7/93
	Purpose:	Constructor. Does nothing but set a flag to a default value.

********************************************************************************************/

OpFlipTrans::OpFlipTrans(): TransOperation()
{
	// Default to horizontal
	IsHorizontal = TRUE;
}



/********************************************************************************************

>	void OpFlipTrans::SetStartBlob(INT32 StartBlob)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/7/94
	Inputs:		StartBlob  - the Number of the blob that the operation was initiated on,
				if applicabale. They are numbered as follows :-
				MonoOn
				1  2  3
				4     5
				6  7  8
				MonoOff
				All operations that were not started from one of the 8 blobs should use 0
	Purpose:	Allows the operations to know how it was started. This operation is
				interested in the horizontal / vertical nature of the transform

********************************************************************************************/

void OpFlipTrans::SetStartBlob(INT32 StartBlob)
{
	// Only blobs 2, 7, 4 and 5 are relavant. All others are a problem
	ENSURE( (StartBlob==2) || (StartBlob==4) || (StartBlob==5) || (StartBlob==7),
			"Flip started on a diagonal startblob. This should not be.");

	// blobs 2 and 7 are the Middle Top and Middle Bottom blobs
	// and would suggest horizontal shearing
	if ((StartBlob==2) || (StartBlob==7))
		IsHorizontal = TRUE;
	else
		IsHorizontal = FALSE;
}





/********************************************************************************************

>	void OpFlipTrans::InitTransformImmediate(OpParam* pOpParam)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/94
	Inputs:		pOpParam - The parameters that were passed into the operation
	Purpose:	Sets up the transform ready for an immediate translation. This is called from
				DoWithParam()
	SeeAlso:	TransOperation::DoWithParam()

********************************************************************************************/

void OpFlipTrans::InitTransformImmediate(OpParam* pOpParam)
{
	// Does nothing
}




/********************************************************************************************

>	void OpFlipTrans::BuildMatrix()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/94
	Purpose:	Builds the transform matrix required to Flip the selection

********************************************************************************************/

void OpFlipTrans::BuildMatrix()
{
	// Translate to the origin
	Transform = Matrix(-CentreOfTrans.x, -CentreOfTrans.y);

	// Flip the selection according to horizontal / vertical ness
	Matrix FlipIt;
	if (IsHorizontal)
		FlipIt = Matrix(FIXED16(-1), FIXED16(1));
	else
		FlipIt = Matrix(FIXED16(1), FIXED16(-1));

	// translate back again
	Matrix TransFromOrigin(CentreOfTrans.x, CentreOfTrans.y);

	// Combine the 3 of them into a single matrix
	Transform *= FlipIt;
	Transform *= TransFromOrigin;
}




/********************************************************************************************

>	BOOL OpFlipTrans::Declare()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/93
	Returns:	TRUE if all went OK, False otherwise
	Purpose:	Adds the operation to the list of all known operations

********************************************************************************************/

BOOL OpFlipTrans::Declare()
{
	return (RegisterOpDescriptor(0, _R(IDS_FLIPTRANS), CC_RUNTIME_CLASS(OpFlipTrans),
								OPTOKEN_FLIP, TransOperation::GetState)); 
}
