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
/********************************************************************************************

>	cxftrans.cpp

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Purpose:	Code for importing and exporting transformation attributes to and
			from the new native file format.

********************************************************************************************/

#include "camtypes.h"
/*
#include "matrix.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"
#include "camfiltr.h"
#include "cxftrans.h"
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::Write(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - Filter to save the record to
				pMatrix	- Matrix to save
	Returns:	BOOL indicating success or not.
	Purpose:	Writes a transformation attribute to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::Write(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	// check the arguments
	ERROR2IF(pFilter == NULL, FALSE, "NULL filter pointer passed as argument.");
	ERROR2IF(pMatrix == NULL, FALSE, "NULL matrix pointer passed as argument.");
	
	BOOL Result;

	// !!ACH!!
	// here we just switch on the transform type of the matrix, and assume it gets set
	// properly by whoever created it. What we ought to do is decompose the matrix and
	// decide whcih of the many transform attribute records we ought to output.
	// This works for the moment, but needs changing.
	
	switch (pMatrix->Type)
	{
		case TRANS_IDENTITY   	:	Result = WriteIdentity(pFilter, pMatrix);
									break;
		case TRANS_TRANSLATION	:	Result = WriteTranslation(pFilter, pMatrix);
									break;
		case TRANS_ROTATION		:	Result = WriteRotation(pFilter, pMatrix);
									break;
		case TRANS_SCALE		:	Result = WriteScale(pFilter, pMatrix);
									break;
		case TRANS_SHEAR		:	Result = WriteShearX(pFilter, pMatrix);
									break;
		case TRANS_COMPLEX		:	Result = WriteGeneral(pFilter, pMatrix);
									break;
		default					:	Result = WriteGeneral(pFilter, pMatrix);
									break;
	}

	return Result;
}
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::WriteIdentity(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
				pMatrix - matrix to save
	Returns:	Bool indicating success
	Purpose:	Writes an identity transform attribute record to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::WriteIdentity(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	BOOL ok;

	CXaraFileRecord Rec(TAG_TRANSFORM_IDENTITY, TAG_TRANSFORM_IDENTITY_SIZE);

	ok = Rec.Init();
	
	if (ok) ok = pFilter->Write(&Rec);
	
	return ok;
}
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::WriteTranslation(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
				pMatrix - matrix to save
	Returns:	boolean value indicating success
	Purpose:	Writes a translation transform attribute to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::WriteTranslation(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	BOOL ok;
	INT32 XTrans, YTrans;

	CXaraFileRecord Rec(TAG_TRANSFORM_TRANSLATION, TAG_TRANSFORM_TRANSLATION_SIZE);

	ok = Rec.Init();

	pMatrix->GetTranslation(XTrans, YTrans);

	if (ok) ok = Rec.Write(XTrans);
	if (ok) ok = Rec.Write(YTrans);
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::WriteRotation(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
				pMatrix - matrix to save
	Returns:	boolean value indicating success
	Purpose:	Writes a rotation transform attribute to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::WriteRotation(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	BOOL ok;

	ANGLE Rotation;

	CXaraFileRecord Rec(TAG_TRANSFORM_ROTATION, TAG_TRANSFORM_ROTATION_SIZE);

	ok = Rec.Init();

	if (ok) ok = pMatrix->Decompose(NULL, NULL, &Rotation, NULL, NULL);
	if (ok) ok = Rec.Write( (FLOAT) Rotation.MakeDouble() );
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::WriteScale(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
				pMatrix - matrix to save
	Returns:	boolean value indicating success
	Purpose:	Writes a scaling transform attribute to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::WriteScale(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	BOOL ok;

	FIXED16 Scale;

	CXaraFileRecord Rec(TAG_TRANSFORM_SCALE, TAG_TRANSFORM_SCALE_SIZE);

	ok = Rec.Init();

	if (ok) ok = pMatrix->Decompose(&Scale, NULL, NULL, NULL, NULL);
	if (ok) ok = Rec.Write( (FLOAT) Scale.MakeDouble() );
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::WriteShearX(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
				pMatrix - matrix to save
	Returns:	boolean indicating success
	Purpose:	Writes a shear parallel to the X axis to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::WriteShearX(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	BOOL ok;
	
	ANGLE Shear;

	CXaraFileRecord Rec(TAG_TRANSFORM_SHEAR_X, TAG_TRANSFORM_SHEAR_X_SIZE);

	ok = Rec.Init();

	if (ok) ok = pMatrix->Decompose(NULL, NULL, NULL, &Shear, NULL);
	if (ok) ok = Rec.Write( (FLOAT) Shear.MakeDouble() );
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}
*/
/********************************************************************************************

>	BOOL CXaraFileTransforms::WriteGeneral(BaseCamelotFilter * pFilter, Matrix * pMatrix)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/06/96
	Inputs:		pFilter - filter to use
				pMatrix - matrix to save out
	Returns:	boolean value indicating success
	Purpose:	writes a general transformation matrix to the filter

********************************************************************************************/
/*

BOOL CXaraFileTransforms::WriteGeneral(BaseCamelotFilter * pFilter, Matrix * pMatrix)
{
	BOOL ok;

	FIXED16 abcd[4];
	INT32 ef[2];

	pMatrix->GetComponents(&abcd[0], &ef[0]);

	CXaraFileRecord Rec(TAG_TRANSFORM_GENERAL, TAG_TRANSFORM_GENERAL_SIZE);

	ok = Rec.Init();

	if (ok) ok = Rec.Write( (FLOAT) abcd[0].MakeDouble() );
	if (ok) ok = Rec.Write( (FLOAT) abcd[1].MakeDouble() );
	if (ok) ok = Rec.Write( (FLOAT) abcd[2].MakeDouble() );
	if (ok) ok = Rec.Write( (FLOAT) abcd[3].MakeDouble() );
	if (ok) ok = Rec.Write( ef[0] );
	if (ok) ok = Rec.Write( ef[1] );
	if (ok) ok = pFilter->Write(&Rec);

	return ok;
}

*/
