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

/*
 * */


#ifndef INC_MATRIX
#define INC_MATRIX

#include "ccmaths.h"
#include "coord.h"
#include "ccobject.h"

class DocRect;
class DocCoord;

/********************************************************************************************

<	TransformType; enum

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/93
	Comment:	This enum is used to determine what type of matrix we have. It can be one of
				the following values :- MonoOn
				TRANS_IDENTITY		// An Identity Matrix
				TRANS_TRANSLATION	// A Translation Matrix
				TRANS_ROTATION		// A Rotation Matrix
				TRANS_SCALE			// A Scale Matrix
				TRANS_SHEAR			// A Shear Matrix
				TRANS_COMPLEX		// A Matrix that is a combination of the above
				MonoOff

********************************************************************************************/

typedef enum
{
	TRANS_IDENTITY,
	TRANS_TRANSLATION,
	TRANS_ROTATION,
	TRANS_SCALE,
	TRANS_SHEAR,
	TRANS_COMPLEX
} TransformType;


/***********************************************************************************************

>	class Matrix : public SimpleCCObject

	Author:		Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com> (extended by Rik)
	Created:	13/5/1993 (extended 19/10/93)
	Purpose:	Represents a mathematical transformation matrix. Transformation matrices can 
				transform the coordinate pairs of one coordinate space into another coordinate
				space.
				Matrices are used in Camelot for the rendering process in various ways. These 
				include coordinate conversion, scaling, rotation and translation. 
				
				Depending on the constructor applied it is possible to specify any of the above
				types of matrix. MonoOn
                 eg.	Matrix();              			//creates the identity matrix
                 		Matrix(ANGLE(90));              //creates a rotation matrix by 90 deg
                 		Matrix(ScaleX, ScaleY);         //creates a scaling matrix
                 		Matrix(xOffset, yOffset);       //creates a translation matrix
                MonoOff
                The Matrix also contains a couple of Public vars that may or may not be
                of any use to anyone. These are as follows :- MonoOn
                TransformType	Type;	// The type of transformation this matrix represents
                ANGLE			Angle;	// The angle of rotation for a rotation matrix
                MonoOff
	SeeAlso:	TransformType
                                    
***********************************************************************************************/

class CCAPI Matrix : public SimpleCCObject
{
friend class OSRenderRegion;
friend class XMatrix;

private:
	FIXED16 a;
	FIXED16 b;
	FIXED16 c;
	FIXED16 d;
	INT32 e;
	INT32 f;

public:
	TransformType	Type;	// The type of transform this matrix represents
	ANGLE			Angle;	// If it is a rotation, this holds the angle of rotation

public:
	// Constructors et all
	Matrix();
	Matrix(const FIXED16&, const FIXED16&);
	Matrix(const ANGLE& theta);
	Matrix(const Coord&);
	Matrix(const Coord&, const ANGLE&);
	Matrix(const INT32, const INT32);
	Matrix(const FIXED16&, const FIXED16&, const FIXED16&, const FIXED16&, const INT32, const INT32);
	Matrix(const DocRect&, const DocRect&);
	
	// Transformations using the matrix
	void transform(Coord*) const;
	void transform(Coord[], UINT32 count) const;
	void transform(Coord[], const Coord[], UINT32) const;

	void translate(const INT32 TransX, const INT32 TransY);

	BOOL TransformBounds(DocRect* pRect) const;
	BOOL Decompose(FIXED16* pScale=NULL, FIXED16* pAspect=NULL,
				   ANGLE* pRotation=NULL, ANGLE* pShear=NULL,
				   Coord* pTranslate=NULL, FIXED16* pScaleY=NULL);
	BOOL Compose(FIXED16 Scale=1.0, FIXED16 Aspect=1.0,
				 ANGLE Rotation=0, ANGLE Shear=0, Coord Translation=Coord(0,0));

	BOOL IsRotation(const double epsilon = 0.0) const;
	BOOL IsReflection(const double epsilon = 0.0) const;
	BOOL IsTranslation(const double epsilon = 0.0) const;
	BOOL IsIdentity(/*DOUBLEPARAM epsilon*/) const;

	// Matrix copy and multiplication stuff
	Matrix& operator=(const Matrix&);
	BOOL operator==(const Matrix&) const;
	friend Matrix operator*(const Matrix&, const Matrix&);
	Matrix& operator*=(const Matrix&);

	// Matrix Inversion
	Matrix Inverse() const;

	// obtain matrix compnents
	void GetComponents( FIXED16 *, INT32 * ) const;
	void GetTranslation(DocCoord&) const;
	void GetTranslation(INT32&,INT32&) const;
	void SetTranslation(const DocCoord&);
	void SetTranslation(const INT32&, const INT32&);

	// Karim 23/07/2000
	// ok, here are some factory methods so you don't have to trawl
	// through the codebase every time you want a certain type of matrix.
	static Matrix CreateTransMatrix(const Coord& dcTrans);
	static Matrix CreateScaleMatrix(const double& xScale, const double& yScale);
	static Matrix CreateScaleMatrix(const double& ScaleFactor);
	static Matrix CreateShearMatrix(const double& ShearAngle);
	static Matrix CreateRotateMatrix(const double& RotateAngle);

	// Karim 04/09/2000
	// two methods to obtain the trace and determinant of a matrix.
	double Trace() const;
	double Det() const;

	// Karim 09/2001
	// A set of matrix fns useful for geometric tweening.
	void RatioMatrix (const double ratio);
	Matrix Interpolate(const Matrix& op, const double ratio) const;
	Matrix PostRotate(DocCoord dcCentre, const ANGLE& Angle) const;

	#ifdef _DEBUG
	void Dump() const;
	#endif
};

#endif 
