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
*/


#ifndef INC_NODEREGSHAPE
#define INC_NODEREGSHAPE

//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docrect.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "ink.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "trans2d.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "pathshap.h" //For the PathShape typedef enum - in camtypes.h [AUTOMATICALLY REMOVED]

const UINT32 REGULARSHAPE_MINSIDES = 3;
const UINT32 REGULARSHAPE_MAXSIDES = 99;

const double QUICKSHAPE_RADIUS_TOLERANCE=100.0;
const double QUICKSHAPE_ANGLE_TOLERANCE=0.02;	//Approximately one degree (remember that 2 PI radians = approx 6 radians = 360 degrees)
const INT32 QUICKSHAPE_SCALARPRODUCT_TOLERANCE=1000;	//The square of the radius tolerance

/*********************************************************************************************

< ShapeClickEffect

	Comment:	Used to indicate 'areas of excitment' within a regular shape.  If nothing
				excites the shape then SHAPE_CLICK_NONE is used.

**********************************************************************************************/ 

enum ShapeClickEffect { SHAPECLICK_NONE, SHAPECLICK_EDGE1,  SHAPECLICK_EDGE2, SHAPECLICK_CURVECHORD,
						SHAPECLICK_CENTRE, SHAPECLICK_ELLIPSE, SHAPECLICK_RECTANGLE, SHAPECLICK_PRIMARY,
						SHAPECLICK_STELLATION, SHAPECLICK_PRIMARYCURVE, SHAPECLICK_STELLATIONCURVE };


/***********************************************************************************************

>	class NodeRegularShape : public NodeSimpleShape

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Purpose:	A NodeRegularShape represents an regularly shaped object.  It encompasses
				NodeSimpleShape	(but is not derived from it as it needs to do more things)

***********************************************************************************************/

class NodeRegularShape : public NodeRenderableInk
{
// Give my name out in memory dumps
CC_DECLARE_DYNCREATE(NodeRegularShape);

// friend classes used to import and export regular shapes to and from the new file format.
// friend class CXaraFileRegularShape;
// friend class RegularShapeRecordHandler;
// friend class EllipseRecordHandler;
// friend class RectangleRecordHandler;
// friend class PolygonRecordHandler;

public:
	NodeRegularShape(); 
	~NodeRegularShape(); 
	
	NodeRegularShape(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked = FALSE, 
				BOOL Mangled = FALSE,  
				BOOL Marked = FALSE, 
				BOOL Selected = FALSE    
			  );

	// Initialise the shape
	BOOL SetUpShape();
	virtual Node* SimpleCopy();
	void CopyNodeContents(NodeRegularShape* NodeCopy);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// virtual Functions to deal with bounding boxes
	virtual DocRect	GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE);
	virtual DocRect GetBlobBoundingRect();

	// Rendering Functions
	BOOL BuildShapePath(Path** RenderPath);
	virtual void Render( RenderRegion* pRender );
	virtual void RenderEorDrag( RenderRegion* );
	virtual void PreExportRender( RenderRegion* pRender ); 
	virtual BOOL ExportRender( RenderRegion* pRender ); 

	// Selection functions.
	virtual void RenderObjectBlobs(RenderRegion* pRender);
	virtual void RenderTinyBlobs(RenderRegion* pRender);

	// Functions to transform the shape
	virtual void Transform( TransformBase& );

	// Information functions
	virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
	virtual String Describe(BOOL Plural, BOOL Verbose);
	ShapeClickEffect DetermineClickEffect(DocCoord* PointerPos, Spread* pSpread, INT32* pPointNum, DocCoord* pStart = NULL, DocCoord* pEnd = NULL);
 
	// Make Shapes functions
	virtual BOOL CanBecomeA(BecomeA* pBecomeA);
	virtual BOOL DoBecomeA(BecomeA* pBecomeA);

	// check to see if we have a variable width stroke applied to us, and get the real path
	virtual NodePath* GetVariableWidthStrokePath();
	virtual NodePath* GetSmoothVariableWidthStrokePath();

	// Debugging function
	#ifdef _DEBUG
		void	ShowDebugTreeDetails() const;
	#endif
	void GetDebugDetails( StringBase* Str );

	// Functions to deal with messages
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread*);
	BOOL OnToolClick( DocCoord, ClickType, ClickModifiers, Spread*);
	virtual BOOL OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu);

	// The natty Snap functions
	virtual BOOL Snap(DocCoord* pDocCoord);
	virtual BOOL Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);
	virtual BOOL SnapToCoords(DocCoord* pDocCoord);

	virtual BOOL IsARegularShape() const;
	virtual double GetRotationAngle();

	// Various helper functions
public:
	void PositionPointFromRatio(DocCoord* PosPoint, const DocCoord* EndPoint, const double Ratio);
	BOOL BuildPolygonPoints(DocCoord** pArray, INT32* NumPoints = NULL);

	BOOL MakeRectangle(const INT32 Width, const INT32 Height, const INT32 CurvatureRadius);
	BOOL MakeEllipse(const INT32 Width, const INT32 Height);

	// Curvature building functions
	double GetMajorRadiusSize();

protected:
	void DeleteCachedPath();
	void InitialiseMemberVars();

private:
	BOOL MakeRegular(DocCoord RegularPoint);
	BOOL MakeStraight(BOOL StraightenOne);
	BOOL ToggleCurvature();
	BOOL InsertShapeEdge(Path* RenderPath, Path* Edge, DocCoord NewEnd);
	BOOL BuildEllipsePath(Path* RenderPath);
	BOOL BuildPolygonPath(Path* RenderPath, DocCoord* PointsArray);	   
	BOOL SmoothCurvatureJoin(Path* RenderPath, DocCoord* Coords, INT32 Index, BOOL MoveLeft); 
	void EmergencyFixShape();

	BOOL ClickEllipse(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, BOOL DragCentre); 
	BOOL ClickCentre(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread); 
	BOOL ClickPrimary(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, BOOL DragCentre); 
	BOOL ClickStellation(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread); 
	BOOL ClickPCurve(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, DocCoord Start, DocCoord End); 
	BOOL ClickSCurve(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, DocCoord Start, DocCoord End); 
	BOOL ClickEdge(DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread, BOOL EdgeOne); 

public:
	// Access functions
	UINT32	GetNumSides() const;
	void	SetNumSides(UINT32 NewNumSides);
	BOOL	IsCircular() const;
	void	SetCircular(BOOL Value);
	BOOL	IsStellated() const;
	void	SetStellated(BOOL Value);
	BOOL	IsPrimaryCurvature() const;
	void	SetPrimaryCurvature(BOOL Value); 
	BOOL	IsStellationCurvature() const;
	void	SetStellationCurvature(BOOL Value);
	double	GetStellRadiusToPrimary() const;
	void	SetStellRadiusToPrimary(double Value);
	double	GetPrimaryCurveToPrimary() const;
	void	SetPrimaryCurveToPrimary(double Value);
	double	GetStellCurveToStell() const;
	void	SetStellCurveToStell(double Value);
	double	GetStellationRatio() const;
	void	SetStellationRatio(double Value);
	void	SetCentrePoint(DocCoord Value);
	void	SetMinorAxes(DocCoord Value);
	void	SetMajorAxes(DocCoord Value);
	DocCoord GetCentrePoint();
	DocCoord GetMajorAxes();
	DocCoord GetMinorAxes();
	DocCoord GetUTCentrePoint() const	{return UTCentrePoint;};
	DocCoord GetUTMajorAxes() const		{return UTMajorAxes;};
	DocCoord GetUTMinorAxes() const		{return UTMinorAxes;};
	void	GetTransformMatrix(Matrix* store) const;
	void	SetTransformMatrix(const Matrix* newmatrix);

	BOOL IsARectangle() const;
	
	INT32 GetRectWidth();
	INT32 GetRectHeight();

	inline void InvalidateCache()					{PathCacheInvalid = TRUE;};
	BOOL IsReformed();

	// Data vars
public:
	// The paths are public to allow people to party all over them easily.
	Path	EdgePath1;			// First edge path from primary point to stellation point
	Path	EdgePath2;			// Second edge path from stellation point to primary point (not uses if not stellated)

protected:
	UINT32	NumSides;			// Number of sides the shape has (gives primary angle) (unused if IsCircular)
	BOOL	Circular : 1;		// TRUE if the shape is based on a circle
	BOOL	Stellated : 1;		// TRUE if the shape is stellated.
	BOOL	PrimaryCurvature : 1;		// TRUE if there is primary curvature
	BOOL	StellationCurvature : 1;	// TRUE if there is stellation curvature
	double	StellRadiusToPrimary;	// Ratio of stellation radius to primary radius	(unused if not stellated)
	double	PrimaryCurveToPrimary;	// Ratio of primary curvature radius to primary radius (unused if none)
	double	StellCurveToStell;		// Ratio of stellation curvature radius to stellation radius (unused if none)
	double	StellOffsetRatio;		// The ratio of the stellation offset (+-0.5 = 360/NumSides)

	// These values could be calculated on the fly, but are cached for speed
	DocCoord	UTCentrePoint;		// Centre point of the shape
	DocCoord	UTMajorAxes;		// Major axes from centre to the bounding ellipse
	DocCoord	UTMinorAxes;		// Minor axes from centre to the bounding ellipse
	Path*	CachedRenderPath;
	BOOL	PathCacheInvalid : 1;
	Matrix	TransformMatrix;

// Version 2 file format functions
public:
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	//PathShape functions (Graham 29/4/97)
public:
	PathShape GetPathShape();

	BOOL IsRotated();
	BOOL AxesAreEqual();
	BOOL AxesArePerpendicular();

	virtual INT32 EstimateNodeComplexity (OpParam* details);

	// Karim MacDonald 25/11/1999
	// Extend functions.
	// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
	// also provide complete implementations of ValidateExtend and Extend,
	// which should usually check/extend its children.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);
};


#endif  // NODEREGSHAPE
