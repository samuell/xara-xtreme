// $Id$
// vector3d.h
//
// Author  : Mike
// Purpose : Definition file for vector3d class
// Version : 1.0
// Started : 29/11/95
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

#ifndef INC_VEC3D
#define INC_VEC3D

#include <math.h>

/********************************************************************************************

>	class Vector3D 

	Author: 	Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/08/94
	Purpose:	Define all 3 vector operations we need.

********************************************************************************************/

class Vector3D 
{
	public:
		Vector3D()			{ x=y=z=0; }
		Vector3D(double p)	{ x=y=z=p; }
		Vector3D(double px, double py, double pz) { x=px; y=py; z=pz; }
		Vector3D(const Vector3D& v) { x=v.x; y=v.y; z=v.z; }

		inline Vector3D operator+(const Vector3D& V) const { return Vector3D(x+V.x, y+V.y, z+V.z); }
		inline Vector3D operator-(const Vector3D& V) const { return Vector3D(x-V.x, y-V.y, z-V.z); }
		inline Vector3D operator*(const Vector3D& V) const { return Vector3D(x*V.x, y*V.y, z*V.z); }
		inline Vector3D operator/(const Vector3D& V) const { return Vector3D(x/V.x, y/V.y, z/V.z); }
	
		inline Vector3D operator+(const double& V) const { return Vector3D(x+V, y+V, z+V); }
		inline Vector3D operator-(const double& V) const { return Vector3D(x-V, y-V, z-V); }
		inline Vector3D operator*(const double& V) const { return Vector3D(x*V, y*V, z*V); }
		inline Vector3D operator/(const double& V) const { return Vector3D(x/V, y/V, z/V); }

		inline Vector3D operator+() const { return Vector3D(+x, +y, +z); }
		inline Vector3D operator-() const { return Vector3D(-x, -y, -z); }

		inline BOOL operator==(const Vector3D& V) { return x==V.x && y==V.y && z==V.z; }
		inline BOOL	operator!=(const Vector3D& V) { return x!=V.x || y!=V.y || z!=V.z; }
		
		inline Vector3D& operator+=(const Vector3D& V);
		inline Vector3D& operator-=(const Vector3D& V);
		inline Vector3D& operator*=(const Vector3D& V);

		inline double xcomp() const { return x; }
		inline double ycomp() const { return y; }
		inline double zcomp() const { return z; }

		inline double Length()						const { return ( sqrt(x*x + y*y + z*z) ); }
		inline double SumSquares()					const { return ( x*x + y*y + z*z ); }
		inline double Dot(const Vector3D& p)		const { return ( x*p.x + y*p.y + z*p.z ); }
		inline double SumMult(const Vector3D& p)	const { return ( x*p.x + y*p.y + z*p.z); }
		inline double SqrDist(const Vector3D& p)	const { return (p.x-x)*(p.x-x) + (p.y-y)*(p.y-y) + (p.z-z)*(p.z-z); }
		inline double DistanceTo(const Vector3D& p) const { return sqrt(SqrDist(p)); }

		inline Vector3D Mix(const Vector3D &mix, double t) const;

		void		FaceForward(const Vector3D& p);
		double		Normalise();
		Vector3D	NormaliseI();
		Vector3D	Cross(const Vector3D& q) const;

		//Vector3D	TransformP(const Matrix3D& mat) const;
		//Vector3D	TransformN(const Matrix3D& mat) const;
		//Vector3D	TransformD(const Matrix3D& mat) const;

		static Vector3D Half(const Vector3D&, const Vector3D&);
		static Vector3D TorusUV(double srad, double trad, double u, double v);

	public:
		double x; 
		double y; 
		double z; 
};




/********************************************************************************************

********************************************************************************************/

inline Vector3D& Vector3D::operator+=(const Vector3D& p)
{
	x += p.x;
	y += p.y;
	z += p.z;
	return *this;
}

inline Vector3D& Vector3D::operator-=(const Vector3D& p)
{
	x -= p.x;
	y -= p.y;
	z -= p.z;
	return *this;
}

inline Vector3D& Vector3D::operator*=(const Vector3D& p)
{
	x *= p.x;
	y *= p.y;
	z *= p.z;
	return *this;
}

inline Vector3D Vector3D::Mix(const Vector3D &mix, double t) const
{
	Vector3D v;
	v.x = (1.0-t)*x + t*mix.x;
	v.y = (1.0-t)*y + t*mix.y;
	v.z = (1.0-t)*z + t*mix.z;
	return v;
}

#endif
