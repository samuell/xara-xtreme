// $Id: gclips.h 662 2006-03-14 21:31:49Z alex $
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
// Gavin's path clipping code

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// GClipS.h
//
/////////////////////////////////////////////////////////////////////////////////////////////////

const UINT32 CLIPPING_STYLE		    = 0x0007 ;
const UINT32 CLIPPING_SOURCE_WINDING	= 0x0010 ;
const UINT32 CLIPPING_CLIP_WINDING	= 0x0020 ;
const UINT32 CLIPPING_IS_STROKED		= 0x0040 ;
const UINT32 CLIPPING_IS_CLOSED		= 0x0080 ;

const UINT32 EDGE_CLIP		= 0x0001 ;
const UINT32 EDGE_REVERSED	= 0x0002 ;
const UINT32 EDGE_CURVED		= 0x0004 ;
const UINT32 EDGE_STROKED	= 0x0008 ;

/////////////////////////////////////////////////////////////////////////////////////////////////

struct Curve {
	POINT	P0 ;
	POINT	P1 ;
	POINT	P2 ;
	POINT	P3 ;
} ;

struct Edge {
	Edge	*LK ;			// Link
	Edge	*RLK ;			// Reverse link - only used to initially sort the edges
	double	DX ;			// Slope = (XE-XS)/(YE-YE)
	INT32	XS,YS ;			// Start point
	INT32	XE,YE ;			// End point
	INT32	XI,YI ;			// Intersection point
	UINT32	FF ;			// Flags
	Curve	*ID ;			// Pointer to curve data - used as curve ID.
	UINT32	TS ;			// Start t parameter
	UINT32	TE ;			// End t parameter
} ;

struct LineEdge {
	Edge	*LK ;
	Edge	*RLK ;
	double	DX ;
	INT32	XS,YS ;
	INT32	XE,YE ;
	INT32	XI,YI ;
	UINT32	FF ;
} ;

struct CurveEdge {
	Edge	*LK ;
	Edge	*RLK ;
	double	DX ;
	INT32	XS,YS ;
	INT32	XE,YE ;
	INT32	XI,YI ;
	UINT32	FF ;
	Curve	*ID ;
	UINT32	TS ;
	UINT32	TE ;
} ;

struct Strip {
	Strip	*FSLK ;				// [L]in[K] to next strip, [F]orward sorted by [S]tart value.
	Strip	*RELK ;				// [L]in[K] to next strip, [R]everse sorted by [E]nd value.
	INT32	YS,YE ;
	union	{
		struct {
			INT32	XSL,XEL ;
			union {
				double	DXL ;
				__int64	FGL ;	// -infinity if curve, +infinity if strip is invalid
			} ;
			UINT32	TSL,TEL ;
			INT32	XSR,XER ;
			union {
				double	DXR ;
				__int64	FGR ;	// -infinity if curve
			} ;
			UINT32	TSR,TER ;
			Curve	*IDL,*IDR ;
		} ;
		struct {
			Strip	*FELK ;		// [L]in[K] to next strip, [F]orward sorted by [E]nd value.
			Strip	*RSLK ;		// [L]in[K] to next strip, [R]everse sorted by [S]tart value.
			INT32	XS,XE ;
			union {
				double	DX ;
				__int64	FG ;
			} ;
			UINT32	TS,TE ;
			Curve	*ID ;
		} ;
	} ;
} ;

struct FilledStrip {
	Strip	*FSLK ;				// [L]in[K] to next strip, [F]orward sorted by [S]tart value.
	Strip	*RELK ;				// [L]in[K] to next strip, [R]everse sorted by [E]nd value.
	INT32	YS,YE ;
	INT32	XSL,XEL ;
	union {
		double	DXL ;
		__int64	FGL ;			// -infinity if curve, +infinity if strip is invalid
	} ;
	UINT32	TSL,TEL ;
	INT32	XSR,XER ;
	union {
		double	DXR ;
		__int64	FGR ;			// -infinity if curve
	} ;
	UINT32	TSR,TER ;
	Curve	*IDL,*IDR ;
} ;

struct LineStrip {
	Strip	*FSLK ;				// [L]in[K] to next strip, [F]orward sorted by [S]tart value.
	Strip	*RELK ;				// [L]in[K] to next strip, [R]everse sorted by [E]nd value.
	INT32	YS,YE ;
	Strip	*FELK ;				// [L]in[K] to next strip, [F]orward sorted by [E]nd value.
	Strip	*RSLK ;				// [L]in[K] to next strip, [R]everse sorted by [S]tart value.
	INT32	XS,XE ;
	union {
		double	DX ;
		__int64	FG ;
	} ;
	UINT32	TS,TE ;
	Curve	*ID ;
} ;

/////////////////////////////////////////////////////////////////////////////////////////////////

inline bool IsMove ( CONST BYTE Type ) { return (Type & PT_MOVETO)==PT_MOVETO   ; }
inline bool IsLine ( CONST BYTE Type ) { return (Type & PT_MOVETO)==PT_LINETO   ; }
inline bool IsCurve( CONST BYTE Type ) { return (Type & PT_MOVETO)==PT_BEZIERTO ; }

inline bool IsntMove ( CONST BYTE Type ) { return (Type & PT_MOVETO)!=PT_MOVETO   ; }
inline bool IsntLine ( CONST BYTE Type ) { return (Type & PT_MOVETO)!=PT_LINETO   ; }
inline bool IsntCurve( CONST BYTE Type ) { return (Type & PT_MOVETO)!=PT_BEZIERTO ; }

/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4800 )

inline bool IsSourceWinding( UINT32 Flags ) { return Flags & CLIPPING_SOURCE_WINDING ; }
inline bool IsClipWinding  ( UINT32 Flags ) { return Flags & CLIPPING_CLIP_WINDING   ; }
inline bool IsPathStroked  ( UINT32 Flags ) { return Flags & CLIPPING_IS_STROKED     ; }
inline bool IsPathClosed   ( UINT32 Flags ) { return Flags & CLIPPING_IS_CLOSED      ; }

#pragma warning( default : 4800 )

/////////////////////////////////////////////////////////////////////////////////////////////////

inline UINT32 LO( CONST UINT32 l ) { return l & 0x0000ffff ; }
inline UINT32 HI( CONST UINT32 l ) { return l & 0xffff0000 ; }

/////////////////////////////////////////////////////////////////////////////////////////////////
