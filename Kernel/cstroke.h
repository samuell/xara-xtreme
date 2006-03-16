// $Id: cstroke.h 662 2006-03-14 21:31:49Z alex $
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
/************************************************************************/
//
// GStroke.h
//
/************************************************************************/

typedef bool GBOOL ;
typedef	 __int64		DLONG ;
typedef __uint64		UDLONG ;
typedef double			DOUBLE ;

struct FPOINT ;
struct GPOINT ;

#define TypeDef(name)							\
	typedef const name			  c##name ;		\
	typedef		  name*			  p##name ;		\
	typedef const name*			 pc##name ;		\
	typedef		  name* const	 cp##name ;		\
	typedef const name* const	cpc##name ;		\
	typedef		  name**		 pp##name ;		\
	typedef const name**		ppc##name ;

TypeDef( GBOOL	)
TypeDef( INT32	)
TypeDef( UINT32	)
TypeDef( DLONG	)
TypeDef( UDLONG	)

TypeDef( BYTE	)
TypeDef( WORD	)
TypeDef( DWORD	)

TypeDef( FLOAT	)
TypeDef( DOUBLE	)

TypeDef(  POINT	)
TypeDef( FPOINT	)
TypeDef( GPOINT	)

struct FPOINT {
	double X ;
	double Y ;
	FPOINT() {} ;
	FPOINT( cDOUBLE X,cDOUBLE Y ) : X(X),Y(Y) {} ;
	FPOINT( cGPOINT P ) ;
	bool   operator==(cFPOINT& P) const { return X==P.X && Y==P.Y ; }
	bool   operator!=(cFPOINT& P) const { return X!=P.X || Y!=P.Y ; }
	FPOINT operator+ (cFPOINT& P) const { return FPOINT(X+P.X,Y+P.Y) ; }
	FPOINT operator- (cFPOINT& P) const { return FPOINT(X-P.X,Y-P.Y) ; }
	FPOINT operator* (cFPOINT& P) const { return FPOINT(X*P.X,Y*P.Y) ; }
	FPOINT operator* (cDOUBLE& F) const { return FPOINT(X*F  ,Y*F  ) ; }
  // In the event of 32 bit build problems, make the next line
  // conditional upon SIZEOF_LONG != SIZEOF_INT
  	FPOINT operator* (cINT32&   I) const { return FPOINT(X*I  ,Y*I  ) ; }
	FPOINT operator/ (cDOUBLE& F) const { cDOUBLE R=1/F ;
										  return FPOINT(X*R  ,Y*R  ) ; }
	FPOINT operator+ () const { return *this ; }
	FPOINT operator- () const { return FPOINT(-X,-Y) ; }
	DOUBLE Length    () const { return sqrt(X*X+Y*Y) ; }
	FPOINT Normalise () const { cDOUBLE L=Length() ;
								return L>0 ? *this/L : FPOINT(0,0) ; }
} ;
inline FPOINT operator*(cDOUBLE& F,cFPOINT& P) { return P*F ; }
  // In the event of 32 bit build problems, make the next line
  // conditional upon SIZEOF_LONG != SIZEOF_INT
inline FPOINT operator*(cINT32&   I,cFPOINT& P) { return P*I ; }

struct GPOINT {
	INT32 X ;
	INT32 Y ;
	GPOINT() {} ;
	GPOINT( cINT32 X,cINT32 Y ) : X(X),Y(Y) {} ;
	GPOINT( cFPOINT P ) : X((INT32)P.X),Y((INT32)P.Y) {} ;
	bool   operator==(cGPOINT& P) const { return X==P.X && Y==P.Y ; }
	bool   operator!=(cGPOINT& P) const { return X!=P.X || Y!=P.Y ; }
	GPOINT operator+ (cGPOINT& P) const { return GPOINT(X+P.X,Y+P.Y) ; }
	GPOINT operator- (cGPOINT& P) const { return GPOINT(X-P.X,Y-P.Y) ; }
	GPOINT operator* (  cINT32& I) const { return GPOINT(X*I  ,Y*I  ) ; }
	GPOINT operator<<(  cINT32& I) const { return GPOINT(X<<I ,Y<<I ) ; }
	GPOINT operator>>(  cINT32& I) const { return GPOINT(X>>I ,Y>>I ) ; }
	GPOINT operator+ () const { return *this ; }
	GPOINT operator- () const { return GPOINT(-X,-Y) ; }
} ;
inline GPOINT operator*(cINT32& I,cGPOINT& P) { return P*I ; }
inline FPOINT::FPOINT( cGPOINT P ) : X(P.X),Y(P.Y) {}

/************************************************************************/

class CBezier
{
public :
	CBezier( cpcFPOINT C ) ;
	double  Length( cDOUBLE T ) ;
private :
	double BLength( cDOUBLE T ) ;
	double P0,P1,P2,P3,P4 ;
} ;

/************************************************************************/

struct BEVEL_FACE {
	GPOINT aFace[4] ;
	GBOOL  bTriangle ;
	FPOINT Normal ;
} ;

struct DASH {
	DWORD	Length ;
	DWORD	Offset ;
	DWORD	Array[8] ;
} ;

TypeDef( BEVEL_FACE	)
TypeDef( DASH )

/************************************************************************/

class GenStroke
{

public :
	virtual ~GenStroke() { }

	enum {
		MAX_DASH_LENGTH = 8
	} ;

	//
	// Returns path length (>=0) or error (<0).
	//
	// Errors are
	//		-1 CalcBBox failed.
	//		-2 Bad source path
	//		-3 Out of path space
	//		-4 Out of memory
	//
	INT32 StrokePathToPath(
				cpcPOINT	pIPoints,
				cpcBYTE		pITypes,
				UINT32		nILength,
				ppPOINT		ppOPoints,
				ppBYTE		ppOTypes,
				BOOL		bClose,
				UINT32		nLineWidth,
				UINT32		nMitreLimit,
//				UINT32		nFlatness,
				DOUBLE		nFlatness,
				const  CapStyles LineCaps,
				const JoinStyles LineJoin,
				cpcDASH		pDash
			) ;

protected :
	
	enum eEndStyle {			// End of line styles
		END_UNDEFINED,
		END_JOIN,
		END_SMOOTH,
		END_START_CAP,
		END_END_CAP
	} ;

	 CapStyles	 CapStyle ;
	JoinStyles	JoinStyle ;
	DOUBLE		FFlatness ;			// Flatness
	DOUBLE		FFlatness8 ;		// Flatness*8
	DOUBLE		FLineWidth ;		// LineWidth/2
	DOUBLE		FSqrLineWidth ;		// LineWidth/4
	DOUBLE		FDivLineWidth ;		// 2/LineWidth
	DOUBLE		FDivSqrLineWidth ;	// (1<<24)/(LineWidth/4)
	DOUBLE		FMitreLimit ;		// LineWidth/(2*MitreLimit)<<32
	DOUBLE		FSmoothLimit ;
	DOUBLE		FStrokeLimit ;		// LineWidth*3/4*0.001
				
	pGPOINT		pOutputPoints ;		// -> Output path points
	pBYTE		pOutputTypes ;		// -> Output path types
	UINT32		nOutputLength ;		// Length of output arrays
	UINT32		nMaxOutputLength ;	// Maximum allowed length of output arrays 
				
	FPOINT		FirstPoint1 ;		// First point not equal to StartPoint
	FPOINT		FirstPoint2 ;		// First point not equal to StartPoint
	FPOINT		FirstPoint ;		// First point not equal to StartPoint
	FPOINT		SecondPoint1 ;		// Next point not equal to FirstPoint
	FPOINT		SecondPoint2 ;		// Next point not equal to FirstPoint
	FPOINT		SecondPoint ;		// Next point not equal to FirstPoint
	FPOINT		ThisPoint1 ;		// Current point of path
	FPOINT		ThisPoint2 ;		// Current point of path
	FPOINT		ThisPoint ;			// Current point of path
	FPOINT		PrevPoint1 ;		// Previous point to ThisPoint, but not same as ThisPoint
	FPOINT		PrevPoint2 ;		// Previous point to ThisPoint, but not same as ThisPoint
	FPOINT		PrevPoint ;			// Previous point to ThisPoint, but not same as ThisPoint
	FPOINT		LastPoint1 ;		// Previous point to PrevPoint, but not same as PrevPoint
	FPOINT		LastPoint2 ;		// Previous point to PrevPoint, but not same as PrevPoint
	FPOINT		LastPoint ;			// Previous point to PrevPoint, but not same as PrevPoint
	FPOINT		FirstOffsetS ;		// Offset for line from StartPoint to FirstPoint
	FPOINT		FirstOffsetE ;		// Offset for line from StartPoint to FirstPoint
	FPOINT		SecondOffsetS ;		// Offset for line from FirstPoint to SecondPoint
	FPOINT		SecondOffsetE ;		// Offset for line from FirstPoint to SecondPoint
	FPOINT		ThisOffsetS ;		// Offset for line from PrevPoint to ThisPoint
	FPOINT		ThisOffsetE ;		// Offset for line from PrevPoint to ThisPoint
	FPOINT		PrevOffsetS ;		// Offset for line from LastPoint to PrevPoint
	FPOINT		PrevOffsetE ;		// Offset for line from LastPoint to PrevPoint
	FPOINT		ThisMitre ;			// Offset from PrevPoint to mitre
	FPOINT		PrevMitre ;			// Offset from LastPoint to mitre
	DOUBLE		ThisSin ;			// Positive if bend at PrevPoint to left else negative
	DOUBLE		PrevSin ;			// Positive if bend at LastPoint to left else negative
	DOUBLE		ThisCos ;			// COS of angle at PrevPoint
	DOUBLE		FirstLengthS ;		// Length of line from StartPoint to FirstPoint
	DOUBLE		FirstLengthE ;		// Length of line from StartPoint to FirstPoint
	DOUBLE		SecondLengthS ;		// Length of line from FirstPoint to SecondPoint
	DOUBLE		SecondLengthE ;		// Length of line from FirstPoint to SecondPoint
	DOUBLE		ThisLengthS ;		// Length of line from PrevPoint to ThisPoint
	DOUBLE		ThisLengthE ;		// Length of line from PrevPoint to ThisPoint
	DOUBLE		PrevLengthE ;		// Length of line from LastPoint to PrevPoint
	GBOOL		ThisMitreFlag ;		// Set if mitre length at PrevPoint less than MitreLimit
	GBOOL		PrevMitreFlag ;		// Set if mitre length at LastPoint less than MitreLimit
	GBOOL		ThisLengthFlag ;	// Set if dashed or lines shorter than mitre at PrevPoint
	GBOOL		PrevLengthFlag ;	// Set if dashed or lines shorter than mitre at LastPoint
	eEndStyle	FirstLineEnd ;		// At FirstPoint : Line end type
	eEndStyle	ThisLineEnd ;		// At  ThisPoint : Line end type
	eEndStyle	PrevLineEnd ;		// At  PrevPoint : Line end type
	eEndStyle	LastLineEnd ;		// At  LastPoint : Line end type
	GBOOL		FirstIsCurve ;
	GBOOL		SecondIsCurve ;
	GBOOL		ThisIsCurve ;
	GBOOL		PrevIsCurve ;
	GBOOL		LastIsCurve ;
	GBOOL		FirstOSign ;
	GBOOL		FirstISign ;
	GBOOL		SecondOSign ;
	GBOOL		SecondISign ;
	GBOOL		ThisOSign ;
	GBOOL		ThisISign ;
	GBOOL		PrevOSign ;
	GBOOL		PrevISign ;
	GBOOL		LastOSign ;
	GBOOL		LastISign ;

	size_t		DashArrayLength ;
	INT32		DashStartRemain ;
	DOUBLE		DashRemain ;
	UINT32		DashStartCount ;
	UINT32		DashCount ;
	GBOOL		DashStartState ;
	GBOOL		DashState ;
	GBOOL		DashFlag ;
	UINT32		DashTotalLength ;
	UINT32		aDashArray[MAX_DASH_LENGTH] ;

	GBOOL		bInner ;
	GBOOL		bOuter ;

	FPOINT		PrevCOffset ;
	FPOINT		ThisCOffset ;

	static cFLOAT aArcTable[0x201] ;
	static cpcFLOAT pArcTable ;

	void StrokePath(
				cpcGPOINT pInputPoints,
				cpcBYTE   pInputTypes,
				cUINT32	  InputLength,
				cGBOOL	  bClose,
				cUINT32	  LineWidth,
				cUINT32	  MitreLimit,
//				cUINT32    nFlatness,
				cDOUBLE    nFlatness,
				const  CapStyles LineCaps,
				const JoinStyles LineJoin,
				cpcDASH	  pDash
			) ;

	void StrokeLine ( cFPOINT EndPoint, const eEndStyle End = END_JOIN ) ;
	void StrokeCurve( FPOINT C[4] ) ;
	void StrokeCurve( cpcFPOINT FPoint, cGBOOL bOSign,cGBOOL bISign ) ;
	void StrokeCurve( cpcFPOINT FPoint, const eEndStyle End, cGBOOL bOSign,cGBOOL bISign ) ;
	void GenSection(
				cFPOINT Point1,cFPOINT Point2,cFPOINT Point,
				cFPOINT OffsetS,cDOUBLE LengthS,
				cFPOINT OffsetE,cDOUBLE LengthE,
				const eEndStyle LineEnd,
				cGBOOL IsCurve,
				cGBOOL bOSign,cGBOOL bISign
			) ;
	virtual void GenLineEdge() ;

	void GenStrokeCurves(
				cFPOINT P0,cFPOINT P1,cFPOINT P2,cFPOINT P3,
				cFPOINT N00,cFPOINT N33
			) ;
	void GenStrokeInnerCurves(
				cFPOINT P0,cFPOINT P1,cFPOINT P2,cFPOINT P3,
				cFPOINT N00,cFPOINT N33
			) ;
	void DoStrokeCurves(
				cINT32 Depth,
				cFPOINT P06,cFPOINT P26,cFPOINT P46,cFPOINT P66, cFPOINT PC1,cFPOINT PC2,
				cFPOINT T06,cFPOINT T26,cFPOINT T46,cFPOINT T66,
				cFPOINT N06,cFPOINT N26,cFPOINT N46,cFPOINT N66
			) ;

	void SplitCurve( cpFPOINT C0, cpFPOINT C1, cDOUBLE T ) ;

	void GenRoundCap( cFPOINT C,cFPOINT O ) ;
	void CalcCurveOffset() ;
	void AddPoint( UINT32 n ) ;
	virtual void MoveTo_Start( cDOUBLE X,cDOUBLE Y ) ;
	virtual void LineTo_Start( cDOUBLE X,cDOUBLE Y ) ;
	virtual void LineTo_End  ( cDOUBLE X,cDOUBLE Y ) ;
	virtual void Close() ;
	virtual void LineTo(
				cFPOINT C0,cFPOINT C1,
				cFPOINT P0,cFPOINT P1
			) ;
	virtual void CurveTo(
				cFPOINT C0,cFPOINT C1,cFPOINT C2,cFPOINT C3,
				cFPOINT P0,cFPOINT P1,cFPOINT P2,cFPOINT P3
			) ;
	void Set_Prev( cFPOINT P ) ;
	virtual void PathOpen () {} ;
	virtual void PathClose() {} ;

	enum {
		PT_PATHELEMENT = 6		// Path element mask
	} ;

	inline GBOOL   IsClosed  ( cBYTE Type ) { return  (Type & PT_CLOSEFIGURE) ; }
	inline GBOOL IsntClosed  ( cBYTE Type ) { return !(Type & PT_CLOSEFIGURE) ; }
	inline GBOOL   IsMoveTo  ( cBYTE Type ) { return  (Type & PT_PATHELEMENT)==PT_MOVETO   ; } ;
	inline GBOOL IsntMoveTo  ( cBYTE Type ) { return  (Type & PT_PATHELEMENT)!=PT_MOVETO   ; } ;
	inline GBOOL   IsLineTo  ( cBYTE Type ) { return  (Type & PT_PATHELEMENT)==PT_LINETO   ; } ;
	inline GBOOL IsntLineTo  ( cBYTE Type ) { return  (Type & PT_PATHELEMENT)!=PT_LINETO   ; } ;
	inline GBOOL   IsBezierTo( cBYTE Type ) { return  (Type & PT_PATHELEMENT)==PT_BEZIERTO ; } ;
	inline GBOOL IsntBezierTo( cBYTE Type ) { return  (Type & PT_PATHELEMENT)!=PT_BEZIERTO ; } ;
} ;

/************************************************************************/

class GenPathContour : public GenStroke
{
public :
	//
	// Set bOuter for outer contour.
	//
	// Returns path length (>=0) or error (<0).
	//
	// Errors are
	//		-1 CalcBBox failed.
	//		-2 Bad source path
	//		-3 Out of path space
	//		-4 Out of memory
	//
	INT32 ContourPath(
				cpcPOINT	pIPoints,
				cpcBYTE		pITypes,
				UINT32		nILength,
				ppPOINT		ppOPoints,
				ppBYTE		ppOTypes,
				UINT32		LineWidth,
				UINT32		MitreLimit,
//				UINT32		nFlatness,
				DOUBLE		nFlatness,
				const JoinStyles LineJoin,
				bool		bOuter
			) ;

protected :
	bool bStart ;

	void GenLineEdge() ;
	void GenOuterEdge() ;
	void GenInnerEdge() ;

	void LineTo_Start( cDOUBLE X,cDOUBLE Y ) ;
	void PathOpen () { bStart = true ; }
	void PathClose() { if (!bStart) Close() ; }
} ;

/************************************************************************/

class GenBevelFaces : public GenStroke
{
public :
	//
	// Set bOuter for outer contour.
	//
	// Returns number of bevel faces (>=0) or error (<0).
	//
	// Errors are
	//		-1 CalcBBox failed.
	//		-2 Bad source path
	//		-3 Out of path space
	//		-4 Out of memory
	//
	INT32 BevelPath(
				cpcPOINT	pIPoints,
				cpcBYTE		pITypes,
				UINT32		ILength,
				ppBEVEL_FACE ppOutputFaces,
				UINT32		LineWidth,
				UINT32		MitreLimit,
//				UINT32		nFlatness,
				DOUBLE		nFlatness,
				const JoinStyles LineJoin,
				bool		bOuter
			) ;

protected :
	pBEVEL_FACE	pBFaces ;		// -> Bevel faces
	UINT32		nMaxBFaces ;	// Maximum number of bevel faces.
	UINT32		nBFaces ;		// Current number of bevel faces.

	pBEVEL_FACE NewBFace() ;

	void GenLineEdge() ;
	void GenOuterEdge() ;
	void GenInnerEdge() ;

	void LineTo(
				cFPOINT C0,cFPOINT C1,
				cFPOINT P0,cFPOINT P1
			) ;
	void CurveTo(
				cFPOINT C0,cFPOINT C1,cFPOINT C2,cFPOINT C3,
				cFPOINT P0,cFPOINT P1,cFPOINT P2,cFPOINT P3
			) ;
	void CurveTo( cFPOINT C, cFPOINT P0,cFPOINT P1,cFPOINT P2,cFPOINT P3 ) ;
} ;

/************************************************************************/
