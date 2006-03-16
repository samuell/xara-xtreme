// $Id: matrix.cpp 662 2006-03-14 21:31:49Z alex $
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


#include "camtypes.h"
// camconfig.h must be included immediately after camtypes.h
//#include "camconfig.h"
#include "errors.h"
#include "ensure.h"
#include "matrix.h"
#include "docrect.h"
//#include "xadrwold.h"
#include "macros.h"
//#include "grndrgn.h"

DECLARE_SOURCE("$Revision: 662 $");

#define	TORADIANS(x)	((x)/180.0*PI)

/********************************************************************************************

>   Matrix::Matrix () 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     None
	Outputs:    None
	Returns:    None.
	Purpose:    Default initialisation - sets up the identity matrix.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

Matrix::Matrix()
{
	a = (FIXED16)1;
	b = (FIXED16)0;
	c = (FIXED16)0;
	d = (FIXED16)1;
	e = 0;
	f = 0;
	
	Type = TRANS_IDENTITY;
	Angle = (ANGLE)0;
}
	
/********************************************************************************************

>   Matrix::Matrix (const FIXED16& xScale, const FIXED16& yScale) 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     Two fixed16s representing the x and y scale factors.
	Outputs:    None
	Returns:    None.
	Purpose:    Default initialisation - sets up the scale matrix.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

Matrix::Matrix(const FIXED16& xScale, const FIXED16& yScale)  
{
	a = xScale;
	b = (FIXED16)0;
	c = (FIXED16)0;
	d = yScale;
	e = 0;
	f = 0;
	
	Type = TRANS_SCALE;
	Angle = (ANGLE)0;
}

/********************************************************************************************

>   Matrix::Matrix (const ANGLE& theta) 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     An angle representing the degree of rotation.
	Outputs:    None
	Returns:    None.
	Purpose:    Default initialisation - sets up the rotation matrix.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

Matrix::Matrix(const ANGLE& theta) 
{
#if 0
	a = b = c = d = theta;
	
	a.Cos();
	b.Sin();
	c = -(c.Sin());
	d.Cos(); 
#else

	double thetaradians = TORADIANS(theta.MakeDouble());
	FIXED16 costheta = cos(thetaradians);
	FIXED16 sintheta = sin(thetaradians);
	
	a = d = costheta;
	b = sintheta;
	c = -sintheta;
#endif

	e = 0;
	f = 0;
	
	Type = TRANS_ROTATION;
	Angle = theta;
}

/********************************************************************************************

>   Matrix::Matrix (const Coord& disp) 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     Displacement of translation.
	Outputs:    None
	Returns:    None.
	Purpose:    Default initialisation - Sets up a translate matrix.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

Matrix::Matrix(const Coord& disp) 
{
	a = (FIXED16)1;
	b = (FIXED16)0;
	c = (FIXED16)0;
	d = (FIXED16)1;
	e = disp.x;
	f = disp.y;
	
	Type = TRANS_TRANSLATION;
	Angle = (ANGLE)0;
}


/********************************************************************************************

>   Matrix::Matrix (const INT32 x, const INT32 y)

	Author:     Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:    30/5/93
	Inputs:     Displacement of translation.
	Outputs:    None
	Returns:    None.
	Purpose:    Default initialisation - Sets up a translate matrix.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

Matrix::Matrix(const INT32 x, const INT32 y)
{
	a = (FIXED16)1;
	b = (FIXED16)0;
	c = (FIXED16)0;
	d = (FIXED16)1;
	e = x;
	f = y;
	
	Type = TRANS_TRANSLATION;
	Angle = (ANGLE)0;
}
	
/********************************************************************************************

>   Matrix::Matrix (const FIXED16& ca,
					const FIXED16& cb,
					const FIXED16& cc,
					const FIXED16& cd,
					const INT32     ce,
					const INT32     cf
					) 

	Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:    21/7/93
	Inputs:     All six parameters which make up a matrix
	Outputs:    None
	Returns:    None.
	Purpose:    Initialise a while matrix.
	Errors:     None.

********************************************************************************************/

Matrix::Matrix( const FIXED16& ca,
				const FIXED16& cb,
				const FIXED16& cc,
				const FIXED16& cd,
				const INT32     ce,
				const INT32     cf
				) 
{
	a = ca;
	b = cb;
	c = cc;
	d = cd;
	e = ce;
	f = cf;
	
	Type = TRANS_COMPLEX;
	Angle = (ANGLE)0;
}
	

/*******************************************************************************************

>	Matrix::Matrix(const DocRect& Source, const DocRect& Destin)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/93
	Inputs:		Source = source rectangle 
				Destin = destination rectangle
	Returns:	a matrix whose domain is the source rectangle and range is the destination
				rectangle.
	Purpose:	Matrix constructor taking two document rectangles.
				Creates a matrix which can be used to transform coordinates within the
				source rectangle into coordinates in the second rectangle

********************************************************************************************/

Matrix::Matrix(const DocRect& Source, const DocRect& Destin)
{
	double Sx0 = Source.lo.x;
    double Sy0 = Source.lo.y;
	double Sx1 = Source.hi.x;
    double Sy1 = Source.hi.y;
	double Dx0 = Destin.lo.x;   
	double Dy0 = Destin.lo.y;
	double Dx1 = Destin.hi.x;   
	double Dy1 = Destin.hi.y;
	double t0,t1;
	
	(Sx1==Sx0) ? t0=1 : t0=(Dx1-Dx0)/(Sx1-Sx0);
	(Sy1==Sy0) ? t1=1 : t1=(Dy1-Dy0)/(Sy1-Sy0);

	a = (FIXED16)t0;
	b = (FIXED16)0;
	c = (FIXED16)0;
	d = (FIXED16)t1;
	e = (INT32)(Dx0-Sx0*t0);
	f = (INT32)(Dy0-Sy0*t1);

	Type = TRANS_COMPLEX;
	Angle = (ANGLE)0;
}





/********************************************************************************************

>   void Matrix::transform(Coord pts[], UINT32 count) 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     result - points to be translated.
				count - number of points.
	Outputs:    None
	Returns:    None.
	Purpose:    Transforms a list of points.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

void Matrix::transform(Coord pts[], UINT32 count ) const
{                    
	if ( Type == TRANS_TRANSLATION )
	{
		for (UINT32 i = 0; i < count; i++)
		{
			pts[i].x += e;
			pts[i].y += f;
		}
	}
	else
	{
#ifdef OLD_MATRIX_TRANSFORMATIONS
		INT32 tx;        // Holds INPUT value of pts[i].x for use in second MatrixCalc

		for (UINT32 i = 0; i < count; i++)
		{
			tx = pts[i].x;
			pts[i].x = MatrixCalc(a, tx, c, pts[i].y) + e;
			pts[i].y = MatrixCalc(b, tx, d, pts[i].y) + f;
		}
#else
	  	// We use GDraw to transform the path
		// Alex asserts it is unimportant to switch contexts here as path transformations are not context
		// dependent; thus we call GDraw directly

		GMATRIX gmat;
		gmat.AX = a.GetShifted16();
		gmat.AY = b.GetShifted16();
		gmat.BX = c.GetShifted16();
		gmat.BY = d.GetShifted16();
		gmat.CX.SetHighLow(e>>16, e<<16);
		gmat.CY.SetHighLow(f>>16, f<<16);
		// Alex promises we don't need a context to do this op
//		XaDrawOld_TransformPath( (LPPOINT)pts, (LPPOINT)pts, count, &gmat );
		GRenderRegion::GetStaticDrawContext()->TransformPath( (LPPOINT)pts, (LPPOINT)pts, count, &gmat );
#endif
	}

}

/********************************************************************************************

>   void Matrix::transform(Coord pts[], const Coord input[], UINT32 count)

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     input - points to be translated.
				count - number of points.
	Outputs:    result points translated.
	Returns:    None.
	Purpose:    Transforms a list of points.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

void Matrix::transform(Coord pts[], const Coord input[], UINT32 count) const
{
	if ( Type == TRANS_TRANSLATION )
	{
		for (UINT32 i = 0; i < count; i++)
		{
			pts[i].x = input[i].x + e;
			pts[i].y = input[i].y + f;
		}
	}
	else
	{
#ifdef OLD_MATRIX_TRANSFORMATIONS
		for (UINT32 i = 0; i < count; i++)
		{
			pts[i].x = MatrixCalc(a, input[i].x, c, input[i].y);  
			pts[i].x += e;
			pts[i].y = MatrixCalc(b, input[i].x, d, input[i].y);
			pts[i].y += f;
		}
#else
	  	// We use GDraw to transform the path
		// Alex asserts it is unimportant to switch contexts here as path transformations are not context
		// dependent; thus we call GDraw directly

		GMATRIX gmat;
		gmat.AX = a.GetShifted16();
		gmat.AY = b.GetShifted16();
		gmat.BX = c.GetShifted16();
		gmat.BY = d.GetShifted16();
		gmat.CX.SetHighLow(e>>16, e<<16);
		gmat.CY.SetHighLow(f>>16, f<<16);
		// Alex promises we don't need a context to do this op
//		XaDrawOld_TransformPath( (LPPOINT)input, (LPPOINT)pts, count, &gmat );
		GRenderRegion::GetStaticDrawContext()->TransformPath( (LPPOINT)input, (LPPOINT)pts, count, &gmat );
#endif
	}
}

/********************************************************************************************

>   void Matrix::transform(Coord* pt) 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    19/5/93
	Inputs:     Coordinate to be transformed.
	Outputs:    Coordinate is overwritten with new values
	Returns:    None.
	Purpose:    Transforms a single point.
	Errors:     None.

********************************************************************************************/
/*
Technical notes:

********************************************************************************************/

void Matrix::transform(Coord* pt) const
{
	INT32 tx;        // Holds INPUT value of pt.x for use in second MatrixCalc!

	if ( Type == TRANS_TRANSLATION )
	{
		pt->x += e;
		pt->y += f;
	}
	else
	{
		tx = pt->x;
		pt->x = MatrixCalc(a, tx, c, pt->y) + e;
		pt->y = MatrixCalc(b, tx, d, pt->y) + f;
	}
}

/********************************************************************************************

>   Matrix& Matrix::operator=(const Matrix& rhs) 

	Author:     Mario_Shamtani (Xara Group Ltd) <camelotdev@xara.com>
	Created:    14/5/93
	Inputs:     rhs - matrix to be assigned.
	Outputs:    None.
	Returns:    None.
	Purpose:    Matrix assignment.
	Errors:     None.

********************************************************************************************/
Matrix& Matrix::operator=(const Matrix& rhs) 
{
	this->a = rhs.a;
	this->b = rhs.b;
	this->c = rhs.c;
	this->d = rhs.d;
	this->e = rhs.e;
	this->f = rhs.f;
	
	this->Type = rhs.Type;
	this->Angle = rhs.Angle;
				 
	return *this;
}                
				 

/********************************************************************************************

>   BOOL Matrix::operator==(const Matrix&) 

	Author:     Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:    26/01/97
	Inputs:     rhs:	The right-hand side of the equality.
	Returns:    TRUE if this Matrix is equal to rhs
	Purpose:    Test for Matrix equality

********************************************************************************************/
BOOL Matrix::operator==(const Matrix& rhs) const
{
	return (a == rhs.a && b == rhs.b && c == rhs.c && d == rhs.d &&
			e == rhs.e && f == rhs.f);
}


/********************************************************************************************
>   Matrix Matrix::operator*(const Matrix& operand1, const Matrix& operand2) 

	Author:     Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/2/96
	Inputs:     Two matrices to be multiplied.
	Returns:    result of multiplication.
	Purpose:    Matrix multiplication.
********************************************************************************************/

Matrix operator*(const Matrix& op1, const Matrix& op2) 
{
	static Matrix t;

	t = op1;
	t *= op2;	// no point in duplicating code, just call *= operator

	return t;
}


/********************************************************************************************
>   Matrix& Matrix::operator*=(const Matrix& op) 

	Author:     Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:    5/2/96
	Inputs:     Matrix to be multiplied by.
	Returns:    result of multiplication.
	Purpose:    optimized *= operator for matrices
	Note:		e,f are not optimised as MatrixCalc() has different round from explicit operations
				however, it does have optimisations for 0 an 1
********************************************************************************************/

Matrix& Matrix::operator*=(const Matrix& op) 
{
	if (op.b==0 && op.c==0)
	{
		// it's just an x or y scaling ...
		if (op.a!=1)
		{
			a *= op.a;
			c *= op.a;
		}
		if (op.d!=1)
		{
			b *= op.d;
			d *= op.d;
		}
	}
	else
	{
		// it's the complex case ...
		FIXED16 t;
		t = a*op.a + b*op.c;
		b = a*op.b + b*op.d;
		a = t;
		t = c*op.a + d*op.c;
		d = c*op.b + d*op.d;
		c = t;
	}

	// either case requires these bits
	INT32 u;
	u = MatrixCalc(op.a, e, op.c, f) + op.e;
	f = MatrixCalc(op.b, e, op.d, f) + op.f;
	e = u;

	Type  = TRANS_COMPLEX;
	Angle = (ANGLE)0;

	return *this;
}




/********************************************************************************************

>   Matrix Matrix::Inverse() const

	Author:     Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:    2/11/93
	Returns:    The Inverse of this matrix
	Purpose:    Inverts the 'this' matrix and returns the result. 'this' matrix is not
				effected

********************************************************************************************/

Matrix Matrix::Inverse() const
{
	Matrix Temp;

	// Inverting a general matrix is quite an expensive operation, so we will try and
	// avoid having to do all the maths (esp as it can fail - some matrices do not have
	// an inverse! eg Scale by a factor of zero)

	switch ( Type )
	{
		case TRANS_IDENTITY:
			// Inverse is the same as this
			ENSURE(a==FIXED16(1), "Matrix inconsistency!");
			ENSURE(b==FIXED16(0), "Matrix inconsistency!");
			ENSURE(c==FIXED16(0), "Matrix inconsistency!");
			ENSURE(d==FIXED16(1), "Matrix inconsistency!");
			ENSURE(e==0, "Matrix inconsistency!");
			ENSURE(f==0, "Matrix inconsistency!");

			return Temp;
			break;

		case TRANS_TRANSLATION :
			// Translation matrix - The inverse of this requires negating the x and y 
			// components of the translation
			ENSURE(a==FIXED16(1), "Matrix inconsistency!");
			ENSURE(b==FIXED16(0), "Matrix inconsistency!");
			ENSURE(c==FIXED16(0), "Matrix inconsistency!");
			ENSURE(d==FIXED16(1), "Matrix inconsistency!");

			Temp.a     =  this->a;
			Temp.b     =  this->b;
			Temp.c     =  this->c;
			Temp.d     =  this->d;
			Temp.e     = -this->e;
			Temp.f     = -this->f;
			Temp.Type  =  this->Type;
			Temp.Angle =  this->Angle;
			break;

		case TRANS_ROTATION:
			// The inverse of a rotation matrix is the Transpose of this matrix
			// ie components b and c are swapped
			ENSURE(e==0, "Matrix inconsistency!");
			ENSURE(f==0, "Matrix inconsistency!");

			Temp.a     = this->a;
			Temp.b     = this->c;
			Temp.c     = this->b;
			Temp.d     = this->d;
			Temp.e     = this->e;
			Temp.f     = this->f;
			Temp.Type  = this->Type;
			Temp.Angle = -this->Angle;
			break;

		case TRANS_SCALE:
			// This can fail if one of the scale factors is 0. This will cause an ENSURE to fail
			// The inverse of a scale involves finding the inverse of the scale factors
//			ENSURE( this->a != 0, "Matrix Inversion failed - X scale factor was zero!" );
//			ENSURE( this->d != 0, "Matrix Inversion failed - Y scale factor was zero!" );
			if ((this->a==0) || (this->d==0))
			{
				TRACE( _T("Matrix Inversion failed!\n"));
				// There is no inversion of this matrix
				// return the identity matrix
				return Temp;
			}
			ENSURE(e==0, "Matrix inconsistency!");
			ENSURE(f==0, "Matrix inconsistency!");

			Temp.a     = 1/this->a;
			Temp.b     = this->b;
			Temp.c     = this->c;
			Temp.d     = 1/this->d;
			Temp.e     = this->e;
			Temp.f     = this->f;
			Temp.Type  = this->Type;
			Temp.Angle = this->Angle;
			break;

		case TRANS_SHEAR:
		case TRANS_COMPLEX:
		default:
			// This is the general case. It may be possible for this to fail.
			// I got the general idea for this from Graphics Gems page 766.
			// It did take 4 pages of c code in there, but it was a slightly
			// more complex solution.

			// Find the determinent of the Adjoint matrix - Luckily we can calculate
			// this before we calculate the Adjoint matrix as we know that the
			// right hand edge of the matrix is 0 0 1
			double Det = a.MakeDouble()*d.MakeDouble() - b.MakeDouble()*c.MakeDouble();

			if (Det==0.0)
			{
				// There is no inversion of this matrix
				// return the identity matrix
//				ENSURE( FALSE, "Matrix Inversion Failed - Tried to Invert a non-invertable matrix" );
				TRACE( _T("Matrix Inversion failed!\n"));
				return Temp;
			}

			// this section calculates the inverse of the matrix. As it takes into
			// account that our 3x3 matrix always has 0,0,1 down its right hand side
			// it has been greatly simplified. The operations combine the calculation
			// of the adjoint matrix and scaling it by the Determinent with a matrix
			// transpose (the inverse is the transpose of the adjoint scaled by the 
			// determinent)
			Temp.a     = (FIXED16)(d.MakeDouble() / Det);
			Temp.b     = (FIXED16)(-b.MakeDouble() / Det);
			Temp.c     = (FIXED16)(-c.MakeDouble() / Det);
			Temp.d     = (FIXED16)(a.MakeDouble() / Det);
			Temp.e     = (INT32)( ((c.MakeDouble()*f)-(e*d.MakeDouble()))/Det );
			Temp.f     = (INT32)( -(((a.MakeDouble()*f) - (e*b.MakeDouble()))/Det));
			Temp.Type  = TRANS_COMPLEX;
			Temp.Angle = 0;

			break;
	}

	// return the inverted matrix back
	return Temp;
}


/********************************************************************************************

>	void Matrix::GetComponents( FIXED16 *abcd, INT32 *ef ) const

	Author:     Andy_Pennell (Xara Group Ltd) <camelotdev@xara.com>
	Created:    24/5/94
	Inputs:		-
	Outputs:	abcd should point to FIXED16[4] which will get loaded with the a,b,c,d
				values from the matrix. Similarly ef should point to INT32[2] which will get
				loaded with the e and f values. If either pointer is NULL then nothing will
				get written for that type.
	Returns:    -
	Purpose:    Allows code to get to the elements in the matrix without having to be a friend
				of the class. Should be used very sparingly indeed.

********************************************************************************************/

void Matrix::GetComponents( FIXED16 *abcd, INT32 *ef) const
{
	if (abcd)
	{
		abcd[0] = a;
		abcd[1] = b;
		abcd[2] = c;
		abcd[3] = d;
	}

	if (ef)
	{
		ef[0] = e;
		ef[1] = f;
	}
}

#ifdef _DEBUG
void Matrix::Dump() const
{
	// Display a matrix
	/*TRACE( _T("| %+5.3f %+5.3f %+5.3f |\n"), a.MakeDouble(), b.MakeDouble(), 0.0 );
	TRACE( _T("| %+5.3f %+5.3f %+5.3f |\n"), c.MakeDouble(), d.MakeDouble(), 0.0 );
	TRACE( _T("| %+5.3f %+5.3f %+5.3f |\n\n"), (double)e, (double)f, 1.0 );*/
	TRACE( _T("| %f %f %f |\n"), a.MakeDouble(), b.MakeDouble(), 0.0 );
	TRACE( _T("| %f %f %f |\n"), c.MakeDouble(), d.MakeDouble(), 0.0 );
	TRACE( _T("| %f %f %f |\n\n"), (double)e, (double)f, 1.0 );
}
#endif

/********************************************************************************************
>	void Matrix::translate(const INT32 TransX, const INT32 TransY);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/4/95
	Inputs:		TransX, TransY - translations to apply
	Purpose:	QUICKLY add a translation to a matrix!
********************************************************************************************/

void Matrix::translate(const INT32 TransX, const INT32 TransY)
{
	e+=TransX;
	f+=TransY;
}


void Matrix::GetTranslation(DocCoord& coord) const
{
	coord.x = e;
	coord.y = f;	
}

void Matrix::GetTranslation(INT32& xlate,INT32& ylate) const
{
	xlate=e;
	ylate=f;
}

void Matrix::SetTranslation(const DocCoord& coord)
{
	e = coord.x;
	f = coord.y;

	if (Type!=TRANS_TRANSLATION)
		Type = TRANS_COMPLEX;
}

void Matrix::SetTranslation(const INT32& xlate, const INT32& ylate)
{
	e = xlate;
	f = ylate;

	if (Type!=TRANS_TRANSLATION)
		Type = TRANS_COMPLEX;
}




/********************************************************************************************
>	virtual BOOL Matrix::Decompose(FIXED16* pScale=NULL, FIXED16* pAspect=NULL,
								   ANGLE* pRotation=NULL, ANGLE* pShear=NULL,
								   Coord* pTranslate=NULL, FIXED16* pScaleY=NULL);

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Outputs:	pScale     - if not NULL, outputs scale (-ve indicates mirror Y)
				pAspect    - if not NULL, outputs aspect
				pRotation  - if not NULL, outputs rotation (-PI..PI)
				pShear     - if not NULL, outputs sheer (-PI/2..PI/2)
				pTranslate - if not NULL, outputs translation
	Returns:	FALSE if falis
	Purpose:	Decompose a matrix into its component transforms
********************************************************************************************/

BOOL Matrix::Decompose(FIXED16* pScale, FIXED16* pAspect, ANGLE* pRotation,
					   ANGLE* pShear, Coord* pTranslate, FIXED16* pScaleY)
{
	// unit square decomposition of matrix ...
	// A={a,c}, B={b,d}, Q=angle between vectors (ie PI/2-shear angle)!
	// |A|=sqrt(aa+cc)
	// |B|=sqrt(bb+dd)
	// AxB=|A||B|sinQ=ad-bc=(new area of unit square)
	// A.B=|A||B|cosQ=ab+cd
	// so ...
	// ScaleX   = |A|
	// ScaleY   = |B|
	// Scale    = ScaleY*cos(PI/2-Q) = ScaleY*sinQ = AxB/|A|
	// AbsScale = abs(Scale) // effectively remove any mirror
	// Aspect   = ScaleX/abs(Scale)
	// Shear    = PI/2-Q = PI/2-acos((A.B)/(|A||B|))
	// Rotate   = RotateX = atan2(c,a)

	double a=this->a.MakeDouble();
	double b=this->c.MakeDouble();	// 'cos I think of coords as column vectors not rows!
	double c=this->b.MakeDouble();
	double d=this->d.MakeDouble();

	// get cross product (determinant), modulus (length) of A and scale
	double AxB      = a*d-b*c;
	double ModA     = sqrt(a*a+c*c);
	double ModB		= sqrt(b*b+d*d);
	double Scale    = AxB/ModA;

	// set output values where required
	if (pTranslate) *pTranslate = Coord(this->e,this->f);
	if (pScale)     *pScale     = Scale;
	if (pAspect)
		if (Scale==0)
		    *pAspect    = (FIXED16)1;
		else
		    *pAspect    = ModA/fabs(Scale);
	if (pRotation)  *pRotation  = (ANGLE)atan2(c,a);
	if (pShear)
	{
		double AdotB = a*b+c*d;
		*pShear = (ANGLE)(PI/2-acos(AdotB/(ModB*ModA)));
	}
	if (pScaleY)	*pScaleY = ModB;

//	TRACE( _T("\n"));
//	TRACE( _T("Scale  = %f\n"),Scale);
//	TRACE( _T("Aspect = %f\n"),ModA/fabs(Scale));
//	TRACE( _T("Rotate = %f\n"),atan2(c,a)/PI*180);
//	double AdotB = a*b+c*d;
//	double ModB  = sqrt(b*b+d*d);
//	TRACE( _T("Shear  = %f\n"),(PI/2-acos(AdotB/(ModB*ModA)))/PI*180);

	return TRUE;
}


/********************************************************************************************
>	virtual BOOL Matrix::Compose(FIXED16 Scale=1.0, FIXED16 Aspect=1.0,
							     ANGLE Rotation=0, ANGLE Shear=0,
								 Coord Translation=Coord(0,0));

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/95
	Inputs:		Scale       - 
				Aspect      - 
				Rotation    - 
				Shear       - 
				Translation - 
	Returns:	FALSE if falis
	Purpose:	Compose a matrix from its components
********************************************************************************************/

BOOL Matrix::Compose(FIXED16 Scale, FIXED16 Aspect, ANGLE Rotation, ANGLE Shear, Coord Translation)
{
	// do scale, aspect and shear
	FIXED16 AbsScale = (Scale<0) ? -Scale : Scale;
	*this=Matrix(AbsScale*Aspect,0,Scale*tan(Shear.MakeDouble()),Scale,0,0);

	// do rotate
	*this*=Matrix((Rotation*180)/PI);		// god damned amatuers using degrees!

	// translate
	this->translate(Translation.x,Translation.y);

	return TRUE;
}


/********************************************************************************************
>	BOOL Matrix::TransformBounds(DocRect* pRect) const

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/95
	Inputs:		pRect - rect to transform
	Outputs:	pRect - transformed rect
	Returns:	FALSE if fails
	Purpose:	Given a rect, find its bounds when transformed
********************************************************************************************/

BOOL Matrix::TransformBounds(DocRect* pRect) const
{
	ERROR2IF(pRect==NULL,FALSE,"Matrix::Transform() - pRect==NULL");

	static DocCoord coords[4];
	coords[0] = pRect->lo;
	coords[1] = DocCoord(pRect->hi.x,pRect->lo.y);
	coords[2] = pRect->hi;
	coords[3] = DocCoord(pRect->lo.x,pRect->hi.y);

	this->transform((Coord*)&coords,4);

	*pRect = DocRect(coords[0],coords[0]);
	pRect->IncludePoint(coords[1]);
	pRect->IncludePoint(coords[2]);
	pRect->IncludePoint(coords[3]);

	return TRUE;
}
	

/********************************************************************************************

>	Matrix::Matrix(const Coord& CentreOfRotation, const ANGLE& Rotation)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/94
	Inputs:		CentreOfRot - The centre of rotation
				RotateBy    - The angle of the rotation that this transform object will perform
	Purpose:	Creates a Matrix transform object that will perform a rotation about the
				given point by the given number of degrees.

********************************************************************************************/

Matrix::Matrix(const Coord& CentreOfRotation, const ANGLE& Rotation)
{
	// need to translate the centre of rotation to the origin
	*this = Matrix(-CentreOfRotation.x, -CentreOfRotation.y);

	// rotate about the origin
	*this *= Matrix(Rotation);

	// translate back to the centre of rotation
	this->translate(CentreOfRotation.x, CentreOfRotation.y);
	
	Type  = TRANS_COMPLEX;
	Angle = (ANGLE)0;
}



/********************************************************************************************

>	BOOL Matrix::IsTranslation(double epsilon=0.0) const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/01/96
	Inputs:		epsilon - an error threshold to use against the scaling components
	Returns:	TRUE if this matrix is a translation matrix
				FALSE if not
	Purpose:	Examine this matrix and determin whether it is a translation matrix or not.
				This function is needed as within the code there are areas where complex
				matricees are built which are really translation matricees. The function
				will return immediately with TRUE if the matrix type is Translation otherwise
				the function will actually check the matrix values for translation type
	See also:	IsRotation(), IsReflection().

********************************************************************************************/
BOOL Matrix::IsTranslation(const double epsilon) const
{
	if (Type==TRANS_TRANSLATION)
		return TRUE;

	if (epsilon==0.0)
	{
		FIXED16 c0,c1,c2,c3;
		c0 = (FIXED16)1;
		c1 = (FIXED16)0;
		c2 = (FIXED16)0;
		c3 = (FIXED16)1;
		return  ((a==c0) && (b==c1) && (c==c2) && (d==c3));
	}

	double c0,c1,c2,c3;
	
	c0=a.MakeDouble();
	c1=b.MakeDouble();
	c2=c.MakeDouble();
	c3=d.MakeDouble();

	return ( (fabs(c0-1.0)<epsilon) && (fabs(c1-0.0)<epsilon) && (fabs(c2-0.0)<epsilon) && (fabs(c3-1.0)<epsilon) );
}



/********************************************************************************************

>	BOOL Matrix::IsRotation(const double epsilon = 0.0) const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2000
	Inputs:		epsilon		optional error threshold to use in our calculations.

	Returns:	TRUE if we're a rotation matrix,
				FALSE otherwise.

	Purpose:	Determine whether this matrix is a rotation matrix or not.

	See also:	IsTranslation(), IsReflection().

********************************************************************************************/
BOOL Matrix::IsRotation(const double epsilon) const
{
	if (Type == TRANS_ROTATION)
		return TRUE;

	// Note: Camelot matrices swap the b & c components, so the form is:
	//
	//	(a c) , instead of the more familiar (a b) .
	//	(b d)								 (c d)
	//
	// So tests for rotation are:
	//	1. a = d
	//	2. b = -c
	//	3. |(a b)| = |(c d)| = 1.

	if (epsilon == 0.0)
	{
		if (a == d && b == -c)
		{
			FIXED16 aa = (a * a);
			FIXED16 bb = (b * b);
			FIXED16 ab = aa + bb;
			if (ab == FIXED16(1))
				return TRUE;
		}
	}

	else
	{
		double c0,c1,c2,c3;

		c0 = a.MakeDouble();
		c1 = b.MakeDouble();
		c2 = c.MakeDouble();
		c3 = d.MakeDouble();

		if (fabs(c0 - c3) < epsilon && fabs(c1 + c2) < epsilon)
		{
			double ab = (c0 * c0) + (c1 * c1);
			if (fabs(ab - 1.0) < epsilon)
				return TRUE;
		}
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL Matrix::IsReflection(const double epsilon = 0.0) const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/2000
	Inputs:		epsilon		optional error threshold to use in our calculations.

	Returns:	TRUE if we're a reflection matrix,
				FALSE otherwise.

	Purpose:	Determine whether this matrix is a reflection matrix or not.

	Notes:		In the interests of speed, this test is INCOMPLETE - we're *only*
				testing for x- or y- axis reflections (no, not both together either).

				Feel free to modify this method to test for all reflections, but first make
				sure you won't break any code which uses it (at time of writing, you're ok).

	See also:	IsTranslation(), IsRotation().

********************************************************************************************/
BOOL Matrix::IsReflection(const double epsilon) const
{
	// A Camelot matrix of the form: (a c) , is an x- or y- reflection if:
	//								 (b d)
	//	1. b = c = 0.
	//	2. a = -d.
	//	3. |a| = 1.

	if (epsilon == 0.0)
	{
		if (b == c && c == FIXED16(0) &&
			a == -d &&
			(a == FIXED16(1) || a == FIXED16(-1)) )
			return TRUE;
	}

	else
	{
		double c0,c1,c2,c3;

		c0 = a.MakeDouble();
		c1 = b.MakeDouble();
		c2 = c.MakeDouble();
		c3 = d.MakeDouble();

		if (fabs(c1) < epsilon && fabs(c2) < epsilon &&
			fabs(c0 + c3) < epsilon &&
			(fabs(c0 - 1.0) < epsilon || fabs(c0 + 1.0) < epsilon) )
			return TRUE;
	}

	return FALSE;
}



/********************************************************************************************

>	BOOL IsNear(const T& x, const T& value, const T& Tolerance)
						
	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/08/96
	Inputs:		x1:			The first of the two values to compare
				x2:			The second of the two values to compare
				Tolerance:	The value within which x1 is considered to be near to x2
	Returns:	TRUE if the two values have an absolute difference greater than or equal to
				the given Tolerance
				FALSE otherwise
	Purpose:	Support function for IsIsometric
				Determines whether two values are near enough to each other to be considered
				equal.
	Notes:		This is a templated function with <class T>

********************************************************************************************/
/*
template <class T>
inline BOOL IsNear(const T& x1, const T& x2, const T& Tolerance)
{
	return (ABS(x1 - x2) <= Tolerance) ? TRUE : FALSE;
}
*/

/********************************************************************************************

>	BOOL Matrix::IsIdentity(DOUBLEPARAM epsilon) const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/01/97
	Returns:	TRUE if this matrix is an identity matrix
				FALSE if not
	Purpose:	Determines whether or not this matrix is an identity matrix.
				Used by the MOA interface, XMOAFhMtx::FHIMtxIsUnity.

********************************************************************************************/
BOOL Matrix::IsIdentity(/*DOUBLEPARAM epsilon*/) const
{
	BOOL bIsIdentity = FALSE;

	if (Type == TRANS_IDENTITY)
	{
		bIsIdentity = TRUE;
	}

	if (Type != TRANS_IDENTITY)	// && epsilon == 0.0)
	{
		const FIXED16 c0 = FIXED16(0);
		const FIXED16 c1 = FIXED16(1);

		bIsIdentity = (	(a == c1) && (b == c0) && (c == c0) && (d == c1) &&
						(e == 0) && (f == 0));
	}
/*
	if (Type != TRANS_IDENTITY && epsilon != 0.0)
	{
		const double ca = a.MakeDouble();
		const double cb = b.MakeDouble();
		const double cc = c.MakeDouble();
		const double cd = d.MakeDouble();
		const double ce = e.MakeDouble();
		const double cf = f.MakeDouble();

		bIsIdentity = (	IsNear(ca, 1.0, epsilon) && IsNear(cb, 1.0, epsilon) && 
						IsNear(cc, 1.0, epsilon) && IsNear(cd, 1.0, epsilon) && 
						IsNear(ce, 1.0, epsilon) && IsNear(cf, 1.0, epsilon))
	}*/
	return bIsIdentity;
}



/********************************************************************************************

>	static Matrix Matrix::CreateTransMatrix(const Coord& dcTrans)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23-07-2000
	Inputs:		dcTrans		the translation to perform.
	Returns:	A translation matrix.
	Purpose:	Factory method - maps directly onto the translation Matrix constructor.

********************************************************************************************/
Matrix Matrix::CreateTransMatrix(const Coord& dcTrans)
{
	return Matrix(dcTrans);
}



/********************************************************************************************

>	static Matrix Matrix::CreateScaleMatrix(const double& xScale, const double& yScale)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23-07-2000
	Inputs:		xScale		the x-scaling to perform.
				yScale		the y-scaling to perform.
	Returns:	A scaling matrix.
	Purpose:	Factory method - maps directly onto the scaling Matrix constructor.

********************************************************************************************/
Matrix Matrix::CreateScaleMatrix(const double& xScale, const double& yScale)
{
	return Matrix(FIXED16(xScale), FIXED16(yScale));
}



/********************************************************************************************

>	static Matrix Matrix::CreateScaleMatrix(const double& ScaleFactor)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23-07-2000
	Inputs:		ScaleFactor		the uniform scaling to perform.
	Returns:	A scaling matrix.
	Purpose:	Factory method - maps directly onto the scaling Matrix constructor.

********************************************************************************************/
Matrix Matrix::CreateScaleMatrix(const double& ScaleFactor)
{
	return Matrix(FIXED16(ScaleFactor), FIXED16(ScaleFactor));
}



/********************************************************************************************

>	static Matrix Matrix::CreateShearMatrix(const double& ShearAngle)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23-07-2000
	Inputs:		ShearAngle		the angle clockwise from vertical to shear by, in radians.
	Returns:	A shearing matrix.
	Purpose:	Factory method - maps directly onto the 'complex' Matrix constructor.

********************************************************************************************/
Matrix Matrix::CreateShearMatrix(const double& ShearAngle)
{
	FIXED16 one  = FIXED16(1);
	FIXED16 zero = FIXED16(0);
	FIXED16 shear = FIXED16(tan(ShearAngle));
	return Matrix(	one,	zero,
					shear,	one,
					0,		0	);
}



/********************************************************************************************

>	static Matrix Matrix::CreateRotateMatrix(const double& RotateAngle)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23-07-2000
	Inputs:		RotateAngle		the angle to rotate anti-clockwise by, in radians.
	Returns:	A rotation matrix.
	Purpose:	Factory method - maps directly onto the rotation Matrix constructor.

	Notes:		Karim 06/09/2001
				Method now in use & fixed bug so input is processed as radians, not degrees.

********************************************************************************************/
Matrix Matrix::CreateRotateMatrix(const double& RotateAngle)
{
	return Matrix(FIXED16(DDegrees(RotateAngle)));
}



/********************************************************************************************

>	double Matrix::Trace() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04 September 2000
	Returns:	The trace of this matrix (the sum of the elements on its leading diagonal).

	Notes:		Trace(Rotation matrix) = 2cos(theta), where theta is the angle of rotation.
				(Usually 1+2cos(theta), but these are 3x2 matrices).

********************************************************************************************/
double Matrix::Trace() const
{
	return (a.MakeDouble() + d.MakeDouble());
}



/********************************************************************************************

>	double Matrix::Det() const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04 September 2000
	Returns:	The determinant of this matrix: (ad - bc) if matrix is ((a b),(c d)).

	Notes:		Det(Matrix) = area scale factor on application of the matrix.

********************************************************************************************/
double Matrix::Det() const
{
	return ( (a.MakeDouble() * d.MakeDouble()) - (b.MakeDouble() * c.MakeDouble()) );
}

/********************************************************************************************

>	void Matrix::RatioMatrix (const double ratio)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	????
	Returns:	This routine ratios *this* matrix.  For example, if we have a scaling matrix
				and ratio is 0.5, then we get a scaling matrix that has half the effect of
				the original matrix.

				Do NOT go changing this routine !!!!!!  It is fundamental to the entire
				animation system - and an error will break everything !!!!!!!

				You should get CGS to change this routine if needs be ....

				This routine cannot be used to ratio a rotation matrix. This is because the
				elements in a rotation matrix are cos & sin of the quantity which must be
				interpolated (the angle), and these are non-linear functions.

********************************************************************************************/
void Matrix::RatioMatrix (const double ratio)
{
	static Matrix Identity;
	*this = Identity.Interpolate(*this, ratio);
}



/********************************************************************************************

>	Matrix Matrix::Interpolate(const Matrix& op, const double ratio) const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/09/2001

	Inputs:		op		other matrix to interpolate to.
				ratio	interpolation ratio.

	Returns:	Interpolated matrix.

	Purpose:	Interpolate between this and another matrix.

********************************************************************************************/
Matrix Matrix::Interpolate(const Matrix& op, const double ratio) const
{
	// Essential: 0 <= ratio <= 1.

	if (ratio < 0 || ratio > 1 || Type == TRANS_ROTATION || op.Type == TRANS_ROTATION)
	{
		ERROR3("Interpolate() requires 0 <= ratio <= 1 and no rotation matrices.");
		return Matrix();
	}

	else if (ratio == 0)
		return *this;

	else if (ratio == 1)
		return op;

	else
	{
		const FIXED16 f16Ratio (ratio);
		const FIXED16 f16InvRatio (1 - ratio);

		Matrix temp;

		temp.a = a * f16InvRatio + op.a * f16Ratio;
		temp.b = b * f16InvRatio + op.b * f16Ratio;
		temp.c = c * f16InvRatio + op.c * f16Ratio;
		temp.d = d * f16InvRatio + op.d * f16Ratio;

		temp.e = e * (INT32)((1 - ratio) + op.e * ratio);
		temp.f = f * (INT32)((1 - ratio) + op.f * ratio);

		temp.Type	= TRANS_COMPLEX;
		temp.Angle	= 0;

		return temp;
	}
}



/********************************************************************************************

>	Matrix Matrix::PostRotate(DocCoord dcCentre, const ANGLE& Angle) const

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/10/2001

	Inputs:		dcCentre	centre of rotation *before* it is transformed by this matrix.
				Angle		angle of rotation, anti-clockwise in degrees.

	Returns:	A matrix which will apply this transformation followed by a rotation of Angle
				around the transformed centre.

	Purpose:	Allows you to transform an object, then rotate it about some point relative
				to its original self, which has now been transformed.

				eg:	squash an object, then rotate it around its centre. You can't
					rotate-then-squash, as you will lose the right-angles. Nor should you
					rotate about its new centre, as this may be different to the transformed
					version of its old centre.

********************************************************************************************/
Matrix Matrix::PostRotate(DocCoord dcCentre, const ANGLE& Angle) const
{
	transform(&dcCentre);
	return (*this) * Matrix(dcCentre, Angle);
}
