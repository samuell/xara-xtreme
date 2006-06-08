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

// This file holds all the AttributeValue and NodeAttribute classes to do with path
// filling attributes.

#ifndef INC_FILLATTR
#define INC_FILLATTR

#include "attr.h"
#include "nodeattr.h"
#include "msg.h"
#include "bitmap.h"
#include "fillval.h"
#include "list.h"
#include "binds.h"
#include "clikmods.h"

class AttrValueChange;
class AttrColourChange;
class AttrTranspChange;
class AttrColourDrop;
class FillRamp;
class AttrColFillRampChange;
class AttrTranspFillRampChange;
class UndoableOperation;
class SelectedAttrs;
 
/********************************************************************************************

< FillControl

	Comment:	An Enum, specifying a particular Fill Control Point.

				MonoOn
				FILLCONTROL_STARTPOINT,
				FILLCONTROL_ENDPOINT,
				FILLCONTROL_SECONDARYPOINT,
				FILLCONTROL_ENDPOINT2,
				FILLCONTROL_ENDPOINT3,
				FILLCONTROL_DRAGPOINT,
				FILLCONTROL_DRAGPOINT2,
				FILLCONTROL_DRAGPOINT3,
				FILLCONTROL_MANY,
				FILLCONTROL_NULL
				MonoOff
			
				FILLCONTROL_NULL is used in Hit Testing to indicate no control was hit.

********************************************************************************************/

typedef INT32 FillControl;

enum
{
	FILLCONTROL_STARTPOINT,
	FILLCONTROL_ENDPOINT,
	FILLCONTROL_ENDPOINT2,
	FILLCONTROL_ENDPOINT3,
	FILLCONTROL_SECONDARYPOINT,
	FILLCONTROL_DRAGPOINT,
	FILLCONTROL_DRAGPOINT2,
	FILLCONTROL_DRAGPOINT3,
	FILLCONTROL_MANY,
	FILLCONTROL_RAMPPOINT,
	FILLCONTROL_NULL
};

enum
{
	BLOBSTATE_OFF,
	BLOBSTATE_ON,
	BLOBSTATE_TOGGLE
};

#define NUMCONTROLPOINTS FILLCONTROL_NULL

#define DEFAULT_FILLWIDTH 72000;
#define DEFAULT_FILLHEIGHT 72000;

#define ATTRVALUE() ((FillGeometryAttribute*)GetAttributeValue())

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								NodeAttribute classes
//
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	class AttrFillGeometry : public NodeAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Fill Geometry Attribute class.
	SeeAlso:	FillGeometryAttribute

***********************************************************************************************/

class AttrFillGeometry : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrFillGeometry)
	
public:
	AttrFillGeometry();
	~AttrFillGeometry();
	AttrFillGeometry(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		NodeAttribute(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	static BOOL Init();

	virtual INT32 operator==(const NodeAttribute& NodeAttrib);
	virtual INT32 operator=(AttrFillGeometry& FillAttrib);

	// Funcions for rendering the Fill Mesh
	virtual void RenderFillBlobs(RenderRegion* pRender) {}
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0) {}
	virtual void RenderFillBlobs();
	
	// The following allow us to redraw multi-stage fill with no flicker ....
	BOOL AllowRampRedraw;
	void DisableRampRedraw () { AllowRampRedraw = FALSE; }
	void EnableRampRedraw () { AllowRampRedraw = TRUE; }

	// The following allow us to redraw only particular parts of a fills mesh when
	// dragging a multi-stage fill blob ....
	BOOL AllowBoundsRedraw;
	void DisableBoundsRedraw () { AllowBoundsRedraw = FALSE; }
	void EnableBoundsRedraw () { AllowBoundsRedraw = TRUE; }

	virtual void DrawEndBlobs();

	virtual DocColour* GetStartColour() { return ATTRVALUE()->GetStartColour(); }
	virtual DocColour* GetEndColour()   { return ATTRVALUE()->GetEndColour();  }
	virtual DocColour* GetEndColour2()  { return ATTRVALUE()->GetEndColour2(); }
	virtual DocColour* GetEndColour3()  { return ATTRVALUE()->GetEndColour3(); }
	virtual UINT32*	   GetStartTransp() { return ATTRVALUE()->GetStartTransp(); }
	virtual UINT32*	   GetEndTransp()	{ return ATTRVALUE()->GetEndTransp();  }
	virtual UINT32*	   GetEndTransp2()	{ return ATTRVALUE()->GetEndTransp2(); }
	virtual UINT32*	   GetEndTransp3()	{ return ATTRVALUE()->GetEndTransp3(); }
	virtual DocCoord*  GetStartPoint()	{ return ATTRVALUE()->GetStartPoint(); }
	virtual DocCoord*  GetEndPoint()	{ return ATTRVALUE()->GetEndPoint();  }
	virtual DocCoord*  GetEndPoint2()	{ return ATTRVALUE()->GetEndPoint2(); }
	virtual DocCoord*  GetEndPoint3()	{ return ATTRVALUE()->GetEndPoint3(); }
	virtual CProfileBiasGain& GetProfile () { return ATTRVALUE()->GetProfile (); }

	virtual void SetStartColour(DocColour* NewCol) { ATTRVALUE()->SetStartColour(NewCol); }
	virtual void SetEndColour(DocColour* NewCol)   { ATTRVALUE()->SetEndColour(NewCol); }
	virtual void SetEndColour2(DocColour* NewCol)  { ATTRVALUE()->SetEndColour2(NewCol); }
	virtual void SetEndColour3(DocColour* NewCol)  { ATTRVALUE()->SetEndColour3(NewCol); }
	virtual void SetStartTransp(UINT32* NewTransp)   { ATTRVALUE()->SetStartTransp(NewTransp); }
	virtual void SetEndTransp(UINT32* NewTransp)	   { ATTRVALUE()->SetEndTransp(NewTransp); }
	virtual void SetEndTransp2(UINT32* NewTransp)	   { ATTRVALUE()->SetEndTransp2(NewTransp); }
	virtual void SetEndTransp3(UINT32* NewTransp)    { ATTRVALUE()->SetEndTransp3(NewTransp); }
	virtual void SetStartPoint(DocCoord* Pos)	   { ATTRVALUE()->SetStartPoint(Pos); }
	virtual void SetEndPoint(DocCoord* Pos)		   { ATTRVALUE()->SetEndPoint(Pos); }
	virtual void SetEndPoint2(DocCoord* Pos)	   { ATTRVALUE()->SetEndPoint2(Pos); }
	virtual void SetEndPoint3(DocCoord* Pos)	   { ATTRVALUE()->SetEndPoint3(Pos); }
	virtual void SetProfile (CProfileBiasGain& SetWith) { ATTRVALUE()->SetProfile(SetWith); }

	virtual UINT32	GetTranspType() { return ATTRVALUE()->GetTranspType(); }
	virtual UINT32	GetDPI()		{ return ATTRVALUE()->GetDPI(); }
	virtual UINT32	GetFractalDPI() { return ATTRVALUE()->GetFractalDPI(); }
	virtual UINT32	GetFractalDim() { return ATTRVALUE()->GetFractalDim(); }
	virtual INT32    GetSeed()		{ return ATTRVALUE()->GetSeed(); }
	virtual FIXED16 GetGraininess() { return ATTRVALUE()->GetGraininess(); }
	virtual FIXED16 GetGravity()	{ return ATTRVALUE()->GetGravity(); }
	virtual FIXED16 GetSquash()		{ return ATTRVALUE()->GetSquash(); }
	virtual BOOL    GetTileable()	{ return ATTRVALUE()->GetTileable(); }
	virtual double	GetNoiseScale() { return ATTRVALUE()->GetNoiseScale(); }

	virtual void	SetTranspType(UINT32 NewType)		{		 ATTRVALUE()->SetTranspType(NewType); }
	virtual BOOL	SetDPI(UINT32 NewDpi)				{ return ATTRVALUE()->SetDPI(NewDpi); }
	virtual BOOL	SetFractalDPI(UINT32 NewDpi)		{ return ATTRVALUE()->SetFractalDPI(NewDpi); }
	virtual void	SetFractalDim(UINT32 NewDim)		{		 ATTRVALUE()->SetFractalDim(NewDim); }
	virtual BOOL    SetSeed(INT32 NewSeed)			{ return ATTRVALUE()->SetSeed(NewSeed); }
	virtual BOOL    SetGraininess(FIXED16 NewGrain) { return ATTRVALUE()->SetGraininess(NewGrain); }
	virtual BOOL    SetGravity(FIXED16 NewGrav)		{ return ATTRVALUE()->SetGravity(NewGrav); }
	virtual BOOL    SetSquash(FIXED16 NewSquash)	{ return ATTRVALUE()->SetSquash(NewSquash); }
	virtual BOOL    SetTileable(BOOL NewTile)		{ return ATTRVALUE()->SetTileable(NewTile); }
	virtual BOOL	SetNoiseScale(double ns)		{ return ATTRVALUE()->SetNoiseScale(ns); }

	virtual BOOL RecalcFractal() { return ATTRVALUE()->RecalcFractal(); }
	virtual BOOL Randomise()	 { return ATTRVALUE()->Randomise(); }

	virtual BOOL AttachBitmap(KernelBitmap* NewBitmap) { return ATTRVALUE()->AttachBitmap(NewBitmap); }
	virtual BOOL DetachBitmap() { return ATTRVALUE()->DetachBitmap(); }
	virtual KernelBitmap* GetBitmap() { return ATTRVALUE()->GetBitmap(); }
	virtual BOOL CopyBitmap(KernelBitmap* CopyBitmap) { return ATTRVALUE()->CopyBitmap(CopyBitmap); }

	virtual KernelBitmapRef* GetBitmapRef() { return ATTRVALUE()->GetBitmapRef(); }

	virtual INT32 GetTesselation() { return ATTRVALUE()->GetTesselation(); }
	virtual void SetTesselation(INT32 NewTess) { ATTRVALUE()->SetTesselation(NewTess); }

	virtual BOOL IsPerspective() { return ATTRVALUE()->IsPerspective(); }
	virtual void MakePerspective(UndoableOperation* pUndoOp = NULL);
	virtual void RemovePerspective(UndoableOperation* pUndoOp = NULL);

	virtual DocColour *EnumerateColourFields(UINT32 Context);

	virtual AttributeValue* GetAttributeValue() = 0;
	virtual DocRect GetBlobBoundingRect();
	virtual ANGLE GetSecondaryAngle() { return 0; }

	virtual ColourRamp*		  GetColourRamp() { return ATTRVALUE()->GetColourRamp(); }
	virtual TransparencyRamp* GetTranspRamp() { return ATTRVALUE()->GetTranspRamp(); }
	virtual void		  SetColourRamp(ColourRamp* New) { ATTRVALUE()->SetColourRamp (New); }
	virtual void		  SetTranspRamp(TransparencyRamp* New) { ATTRVALUE()->SetTranspRamp(New); }
			FillRamp*		  GetFillRamp();

	// get some data from this fill.
	DocColour*	GetFirstSelectedColour();
	UINT32*		GetFirstSelectedTransp();
	INT32		GetFirstSelectedIndex();

	// Bodge Functions for Current Attribute Transformation
	DocRect GetBoundingRect() { return AttrBounds; }
	void SetBoundingRect(DocRect &NewBounds);
	BOOL WriteBoundingRect(BaseCamelotFilter* pFilter);

	// Functions to cope with the selections of Control Points
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread * );

	virtual void GetControlPoints(DocCoord*, INT32);	
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );
	virtual void SetAspectRatio(double Ratio);

	UINT32 GetSelectionState(BOOL**);
	BOOL IsSelected(UINT32 Control);

	UINT32 GetSelectionCount();
	UINT32 GetBlobCount(BOOL IncludeRamps=TRUE);

	virtual void SetBlobState(FillControl, BOOL);
	virtual void ToggleSelection(FillControl);	
	virtual void SelectBlob(FillControl);	
	virtual void DeselectBlob(FillControl);
	virtual void DeselectAll();
	virtual void DeselectAllBut(FillControl);	
	virtual void DeselectAllNoRedraw();

	virtual void CycleSelection(BOOL Reverse);

	static AttrFillGeometry* FindFirstSelectedAttr(CCRuntimeClass* AttrType = (CCRuntimeClass*)CC_RUNTIME_CLASS(AttrFillGeometry));
	static AttrFillGeometry* FindNextSelectedAttr(CCRuntimeClass* AttrType = (CCRuntimeClass*)CC_RUNTIME_CLASS(AttrFillGeometry));

	static UINT32 CountSelectionControlPoints();
	static BOOL CheckAttrClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread* pSpread);
	static BOOL CheckForFillControlHit(DocCoord Pos, UINT32* Status);

	static FillControl DoColourDropTest(AttrColourDrop*, NodeRenderableInk**, AttrFillGeometry** FillAttr = NULL);

	static BOOL IsTranspMeshVisible();
	static BOOL IsColourMeshVisible();

	virtual BOOL IsVisible() { return IsColourMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual FillControl CheckForControlHit(DocCoord &);	

	virtual AttrFillGeometry* Mutate(AttrFillGeometry*, BOOL bOptimise = TRUE);
	virtual AttrFillGeometry* ChangeAttrValue(AttrValueChange*) { return NULL; }

	virtual void AttributeChanged();

	static UINT32 FillSelectionCount();
	static void SetSelectionCount(UINT32);
	static void SetTranspMeshesVisible(BOOL);

	virtual BOOL Blend(BlendAttrParam* pBlendParam);

	virtual AttrFillGeometry* DoColourDrop(AttrColourDrop*);
	virtual FillControl TestColourDrop(AttrColourDrop*);

	virtual BOOL ChangeControlColour(AttrColourChange *);
	virtual BOOL ChangeControlTransp(AttrTranspChange *);
	virtual BOOL ChangeControlColour(DocColour&, FillControl, AttrColourDrop * pColDrop = NULL);
	virtual BOOL EditColourFillRamp(AttrColFillRampChange *);
	virtual BOOL EditTranspFillRamp(AttrTranspFillRampChange *);
	
	virtual BOOL IsAFillAttr() const { return TRUE; }

	virtual void TransformSelectedControlPoints( TransformBase&, BOOL* isARampBlob = NULL );
	virtual void TransformSelectedFillRampControlPoints (TransformBase&, ClickModifiers ClickMods, BOOL* isARampBlob);

	// Transforms the attributes bounds to new bounds. Used when applying stored attribute 
	// to an object. 
	virtual void TransformToNewBounds(DocRect& NewBounds); 

	virtual BOOL NeedsForceToSimpleMapping() { return(TRUE); }
	
	virtual BOOL IsARadialColourFill() const { return FALSE;}

	virtual BOOL CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc);

	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter) { return (TRUE); }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

protected:
	virtual void ValidateAttributeValue() { }
	virtual BOOL CopyNodeContents(AttrFillGeometry* NodeCopy);
	virtual void RenderControl(FillControl, BOOL);	

	void RenderFillControl(BOOL RenderOn, 
						   DocRect* ControlRect, 
						   Spread* pSpread, 
					   	   NodeRenderable* pParent);

	void RenderControlIntoPendingRegions(DocRect* ControlRect, 
										 Spread* pSpread, 
										 NodeRenderable* pParent);

	AttrFillGeometry* ChangeColour(AttrValueChange*);
	AttrFillGeometry* ChangeTransp(AttrValueChange*);

	BOOL CheckPreviousFillMesh();
	BOOL IsFillBeingEdited();
	static BOOL IsMeshSame(FillGeometryAttribute* , FillGeometryAttribute*);

	void SetLastRenderedBlob(FillControl);
	BOOL IsBlobSame(FillControl);
	void ChangeBlobState(FillControl, INT32);

// Karim MacDonald 03/12/1999
// Extend functions.
// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
// also provide complete implementations of ValidateExtend and Extend,
// which should usually check/extend its children.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// Find the extend reference-centre of this Node, for use with Extension.
	virtual DocCoord FindExtendCentre() { return *GetStartPoint(); }

	// virtual override of NodeRenderable::TransformTranslateObject.
	virtual void TransformTranslateObject(const ExtendParams& ExtParams);

	// returns the number of control points required to describe this fill,
	// for instance flat fill - 0, linear fill - 2, four-colour fill - 4.
	// this value is 0 by default for fills without control points.
	virtual INT32 GetNumberOfControlPoints() { return 0; }

public:
	static AttrFillGeometry* EditedFill;
	static AttrFillGeometry* DraggedFill;
	static List HitList;

	static BOOL DoFillMeshCheck;
	static FillGeometryAttribute* LastRenderedMesh;

	static DocCoord LastRenderedStartBlob;
	static DocCoord LastRenderedEndBlob;
	static DocCoord LastRenderedEnd2Blob;
	static DocCoord LastRenderedEnd3Blob;

	static UINT32 SelectionCount;

	static INT32 FractalSeed;
	static double FractalGraininess;
	static double FractalGravity;

	static INT32 FractalDPI;
	static INT32 MaxFractalSize;

	static BOOL TranspMeshesVisible;

	static BOOL DoCheckOnFillRampMesh;			// this could be changed a variable passed in with
												// CheckForControlHit.  This variable exists so that
												// we avoid this check when we are changing the cursor
												// (since the filltool gets this incorrect - and says that
												// we can move the fillramp)

	static BOOL s_bGroupTransparency;

protected:
	BOOL SelectionState[5];

	static FillControl ControlHit;
	static AttrFillGeometry* FillHit;

	static SelectedAttrs s_SelAttrs;
	static CCAttrMap AttribMap;

	// Bodge variable for Current Attribute Transformation
	DocRect AttrBounds;

	// DMc 22-10-99
	// Find the ramp fill point (0 - 1.0) given certain coordinates
	virtual double FindRampPoint(DocCoord &dc, DocCoord &StartPoint, DocCoord &EndPoint);
 };


/***********************************************************************************************

>	class SelectedAttrs : public CCObject

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/2005
	Purpose:	-
	SeeAlso:	-

***********************************************************************************************/

class SelectedAttrs : public CCObject
{
public:
	SelectedAttrs()
	{
		m_pAttrSelectionLevel = NULL;
		m_pAttrSelection = NULL;
		m_pSelNode = NULL;
		m_pLastSelectedAttr = NULL;
	}

	~SelectedAttrs()
	{
		if (m_pAttrSelectionLevel)
		{
			delete m_pAttrSelectionLevel;
			m_pAttrSelectionLevel = NULL;
		}
	}

	NodeAttribute*	FindFirst(CCRuntimeClass* AttrType = (CCRuntimeClass*)CC_RUNTIME_CLASS(AttrFillGeometry));
	NodeAttribute*	FindNext(CCRuntimeClass* AttrType = (CCRuntimeClass*)CC_RUNTIME_CLASS(AttrFillGeometry));

private:
	Range*			m_pAttrSelectionLevel;
	SelRange*		m_pAttrSelection;
	Node*			m_pSelNode;
	NodeAttribute*	m_pLastSelectedAttr;
};


/***********************************************************************************************

>	class AttrTranspFillGeometry : public AttrFillGeometry

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	-
	SeeAlso:	-

***********************************************************************************************/

class AttrTranspFillGeometry : public virtual AttrFillGeometry
{
	CC_DECLARE_DYNAMIC(AttrTranspFillGeometry)
	
public:
	AttrTranspFillGeometry() : AttrFillGeometry() {}
};

/***********************************************************************************************

>	class AttrValueChange : public AttrFillGeometry

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill colour Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the colours
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrValueChange : public AttrFillGeometry
{
	CC_DECLARE_DYNAMIC(AttrValueChange)
	
public:
	AttrValueChange();

	virtual UINT32 GetAttrNameID(void);  

	virtual AttributeValue* GetAttributeValue() = 0;
	virtual CCRuntimeClass* GetAttributeType();

	virtual void MutateColourFills(BOOL Change);
	virtual void MutateTranspFills(BOOL Change);

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate) { return NULL; }

	virtual BOOL IsAValueChange() const { return TRUE; }

protected:
	BOOL	Colour;
	BOOL	Transp;
};

/***********************************************************************************************

>	class AttrColourChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill colour Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the colours
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrColourChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrColourChange)
	
public:
	AttrColourChange() : AttrValueChange() {}

	virtual UINT32 GetAttrNameID(void);  

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

protected:
	FlatFillAttribute Value;
};

/***********************************************************************************************

>	class AttrColourDrop : public AttrColourChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	'Drop a colour on an object' Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the colours
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrColourDrop : public AttrColourChange
{
	CC_DECLARE_DYNCREATE(AttrColourDrop)
	
public:
	AttrColourDrop() : AttrColourChange() {}
	AttrColourDrop(DocCoord&, DocRect&, DocColour);

	virtual UINT32 GetAttrNameID(void);  
	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);
	virtual BOOL EffectsParentBounds ();		// need to do this now, because otherwise
												// when adding a multi-stage fill blob that
												// is outside the object, then we encounter
												// redraw problems

	void SetDropPoint(DocCoord&);
	void SetObjectBounds(DocRect&);

	void SetObjectDroppedOn(NodeRenderableInk*);
	NodeRenderableInk* GetObjectDroppedOn();

	DocCoord& GetDropPoint() { return DropPos; }
	DocRect& GetObjectBounds() { return Bounds; }

	BOOL IsObjectSelected();
	
protected:
	DocCoord 	DropPos;
	DocRect  	Bounds;

	NodeRenderableInk* 	ObjectHit;
	AttrFillGeometry*  	FillHit;
	FillControl		  	ControlHit;

	BOOL			 	ObjectIsSelected;
};

/***********************************************************************************************

>	class AttrBitmapChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Bitmap Attribute class.
				This Attribute never exists in the Tree.  It is used for changing a
				Bitmap Fill.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrBitmapChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrBitmapChange)
	
public:
	AttrBitmapChange() : AttrValueChange() {}

	virtual UINT32 GetAttrNameID(void);  

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

protected:
	BitmapFillAttribute Value;
};

/***********************************************************************************************

>	class AttrBitmapTessChange : public AttrBitmapChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill Transp Type Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the Transp
                type of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrBitmapTessChange : public AttrBitmapChange
{
	CC_DECLARE_DYNCREATE(AttrBitmapTessChange)
	
public:
	AttrBitmapTessChange() : AttrBitmapChange() { }

	virtual UINT32 GetAttrNameID(void);  

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);
};

/***********************************************************************************************

>	class AttrBitmapDpiChange : public AttrBitmapChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill Transp Type Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the Transp
                type of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrBitmapDpiChange : public AttrBitmapChange
{
	CC_DECLARE_DYNCREATE(AttrBitmapDpiChange)
	
public:
	AttrBitmapDpiChange() : AttrBitmapChange() { }
	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	virtual UINT32 GetAttrNameID(void);  

	void SetDPI(INT32 NewDpi) { Dpi = NewDpi; } 
	UINT32 GetDPI() { return Dpi; } 

protected:
	INT32 Dpi;
};

/***********************************************************************************************

>	class AttrFractalChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fractal Attribute class.
				This Attribute never exists in the Tree.  It is used for changing a
				Fractal Fill.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrFractalChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrFractalChange)
	
public:
	AttrFractalChange() : AttrValueChange() {}

	virtual UINT32 GetAttrNameID(void);  

	virtual AttributeValue* GetAttributeValue() { return &Value; }

protected:
	FractalFillAttribute Value;
};

/***********************************************************************************************

>	class AttrFractalGrainChange : public AttrFractalChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill Transp Type Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the Transp
                type of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrFractalGrainChange : public AttrFractalChange
{
	CC_DECLARE_DYNCREATE(AttrFractalGrainChange)
	
public:
	AttrFractalGrainChange() : AttrFractalChange() { }

	virtual UINT32 GetAttrNameID(void);  

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);
};

/***********************************************************************************************

>	class AttrFractalTileableChange : public AttrFractalChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fractal Tileable Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the Transp
                type of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrFractalTileableChange : public AttrFractalChange
{
	CC_DECLARE_DYNCREATE(AttrFractalTileableChange)
	
public:
	AttrFractalTileableChange() : AttrFractalChange() { }

	virtual UINT32 GetAttrNameID(void);  

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);
};

/***********************************************************************************************

>	class AttrTranspChange : public AttrValueChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill colour Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the Transp
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrTranspChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrTranspChange)
	
public:
	AttrTranspChange();

	virtual UINT32 GetAttrNameID(void);  

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);

protected:
	FlatTranspFillAttribute Value;
};

/***********************************************************************************************

>	class AttrTranspTypeChange : public AttrTranspChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Change Fill Transp Type Attribute class.
				This Attribute never exists in the Tree.  It is used for changing the Transp
                type of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrTranspTypeChange : public AttrTranspChange
{
	CC_DECLARE_DYNCREATE(AttrTranspTypeChange)
	
public:
	AttrTranspTypeChange() : AttrTranspChange() { }

	virtual UINT32 GetAttrNameID(void);  

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);
};



/***********************************************************************************************

>	class AttrNoiseChange : public AttrValueChange

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	Change Noise fill class
				This Attribute never exists in the Tree.  It is used for changing a
				Noise fill.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrNoiseScaleChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrNoiseScaleChange)
	
	public:
		virtual UINT32 GetAttrNameID(void);  
		virtual AttributeValue* GetAttributeValue() { return &Value; }
		virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	protected:
		NoiseFillAttribute Value;
};




/********************************************************************************************

>	RampEdit 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	A typedef to describe the actions one can perform with a AttrFillRampChange

********************************************************************************************/

typedef enum
{
	ACT_DONOTHING,

	ACT_CREATENEWCOL,
	ACT_EDITCOLOFITEM,
	ACT_EDITCOLOFSELECTED,

	ACT_CREATENEWTRANSP,
	ACT_EDITTRANSPOFITEM,
	ACT_EDITTRANSPOFSELECTED
} RampEdit;



/********************************************************************************************

>	class AttrColFillRampChange : public AttrValueChange

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	An attribute to control the changes of colour on a fill ramp.

********************************************************************************************/

class AttrColFillRampChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrColFillRampChange)
	
public:
	AttrColFillRampChange();

	virtual UINT32 GetAttrNameID(void);  
	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }

	// init functions to turn this ramp change attribute into some
	// specific action.
	void InitForColourAdd(DocColour* pColour, float pos);
	void InitForColourEdit(UINT32 i, DocColour* pColour);
	void InitForSelEdit(DocColour* pColour);

	// the methods
	RampEdit GetAction() const 	 { return action; }
	UINT32	 GetIndex() const	 { return index;  }
	float	 GetPosition() const { return position; }

protected:
	RampEdit	action;
	UINT32	  	index;
	float	  	position;

	FlatFillAttribute Value;
			// holds a colour value, this is ridiculous but helps with providing
			// the necessary API to derive from AttrValueChange!
};


/********************************************************************************************

>	class AttrTranspFillRampChange : public AttrValueChange

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	An attribute to control the changes of transparency on a fill ramp.

********************************************************************************************/

class AttrTranspFillRampChange : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrTranspFillRampChange)
	
public:
	AttrTranspFillRampChange();

	virtual UINT32 GetAttrNameID(void);  
	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }

	// init functions to turn this ramp change attribute into some
	// specific action.
	void InitForTranspAdd(UINT32 t, float pos);
	void InitForTranspEdit(UINT32 i, UINT32 t);
	void InitForSelEdit(UINT32 transparency);

	// the methods
	RampEdit GetAction() const 	 { return action; }
	UINT32	 GetIndex() const	 { return index;  }
	float	 GetPosition() const { return position; }

protected:
	RampEdit	action;
	UINT32	  	index;
	float	  	position;

	FlatTranspFillAttribute Value;
			// holds a colour value, this is ridiculous but helps with providing
			// the necessary API to derive from AttrValueChange!
};






/***********************************************************************************************

>	class AttrFlatFill : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Flat Fill colour Attribute class.
	SeeAlso:	FlatFillAttribute

***********************************************************************************************/

class AttrFlatFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrFlatFill)
	
public:
	AttrFlatFill();
	AttrFlatFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	// Selection Functions
	virtual BOOL OnClick( DocCoord, ClickType, ClickModifiers, Spread * ) { return FALSE; }

	virtual DocRect GetBlobBoundingRect();

	virtual BOOL ChangeControlColour(AttrColourChange *) { return FALSE; }
	virtual BOOL ChangeControlTransp(AttrTranspChange *) { return FALSE; }

	virtual void ToggleSelection(FillControl Blob) { DeselectBlob(Blob); }	
	virtual void SelectBlob(FillControl Blob) { DeselectBlob(Blob); }	

	virtual FillControl TestColourDrop(AttrColourDrop*);

	virtual BOOL IsAFlatFill() const { return TRUE; } 

protected:
	virtual void ValidateAttributeValue();
};

/***********************************************************************************************

>	class AttrFlatColourFill : public AttrFlatFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Flat Colour Fill Attribute class.
	SeeAlso:	FlatFillAttribute

***********************************************************************************************/

class AttrFlatColourFill : public AttrFlatFill
{
	CC_DECLARE_DYNCREATE(AttrFlatColourFill)
	
public:
	AttrFlatColourFill() : AttrFlatFill() {}
	AttrFlatColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFlatFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL && GetStartColour()->IsTransparent());
	}

protected:
	FlatFillAttribute Value;
};

/***********************************************************************************************

>	class AttrFlatTranspFill : public AttrFlatFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Flat Transparency Fill Attribute class.
	SeeAlso:	FlatFillAttribute

***********************************************************************************************/

class AttrFlatTranspFill : public AttrFlatFill
{
	CC_DECLARE_DYNCREATE(AttrFlatTranspFill)
	
public:
	AttrFlatTranspFill() : AttrFlatFill() {}
	AttrFlatTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFlatFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );

	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}

protected:
	FlatTranspFillAttribute Value;
};

/***********************************************************************************************

>	class AttrLinearFill : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Linear Graduated Fill Attribute class.
	SeeAlso:	LinearFillAttribute

***********************************************************************************************/

class AttrLinearFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrLinearFill)
	
public:
	AttrLinearFill() : AttrFillGeometry() {}
	AttrLinearFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform( TransformBase& );
	virtual BOOL CanTransform();

	// Functions for rendering Fill Mesh 
	virtual void RenderFillBlobs(RenderRegion* pRender) {}
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);

	virtual AttributeValue* GetAttributeValue() = 0;

	virtual FillControl TestColourDrop(AttrColourDrop*);
	virtual AttrFillGeometry* DoColourDrop(AttrColourDrop* ColDrop) ;

	virtual BOOL IsAGradFill() const { return TRUE; } 

	virtual FillControl CheckForControlHit(DocCoord &ClickPos);
	virtual BOOL ChangeControlColour(DocColour& Col, 
	FillControl Cntrl, AttrColourDrop * pColDrop = NULL);

	virtual BOOL NeedsToRenderAtEachBrushStroke() const { return TRUE;}

	// returns the number of control points required to describe this fill,
	// ie 2 in the case of a linear fill.
	virtual INT32 GetNumberOfControlPoints() { return 2; }

	// virtual override of AttrFillGeometry::TransformTranslateObject.
	virtual void TransformTranslateObject(const ExtendParams& ExtParams);

protected:
	virtual void ValidateAttributeValue();
};

/***********************************************************************************************

>	class AttrLinearColourFill : public AttrLinearFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Linear Graduated Colour Fill Attribute class.
	SeeAlso:	LinearFillAttribute

***********************************************************************************************/

class AttrLinearColourFill : public AttrLinearFill
{
	CC_DECLARE_DYNCREATE(AttrLinearColourFill)
	
public:
	AttrLinearColourFill() : AttrLinearFill() {}
	AttrLinearColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrLinearFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent() &&
				GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());
	}

protected:
	LinearFillAttribute Value;
};


/***********************************************************************************************

>	class AttrLinearTranspFill : public AttrLinearFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Linear Graduated Transparency Fill Attribute class.
	SeeAlso:	LinearFillAttribute

***********************************************************************************************/

class AttrLinearTranspFill : public AttrLinearFill
{
	CC_DECLARE_DYNCREATE(AttrLinearTranspFill)
	
public:
	AttrLinearTranspFill() : AttrLinearFill() {}
	AttrLinearTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrLinearFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}

protected:
	LinearTranspFillAttribute Value;
};

/***********************************************************************************************

>	class AttrRadialFill : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Radial Graduated Fill Attribute class.
	SeeAlso:	RadialFillAttribute

***********************************************************************************************/

class AttrRadialFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrRadialFill)
	
public:
	AttrRadialFill() : AttrFillGeometry() {}
	AttrRadialFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform( TransformBase& );
	virtual BOOL CanTransform();

	// Functions for rendering Fill Mesh 
	virtual void RenderFillBlobs(RenderRegion* pRender) {}
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );

	virtual AttributeValue* GetAttributeValue() = 0;

	virtual DocRect GetBlobBoundingRect();

	virtual ANGLE GetSecondaryAngle() { return 90; }

	virtual FillControl CheckForControlHit(DocCoord &);	

	void MakeCircular();
	void MakeElliptical();

	BOOL IsCircular() { return ATTRVALUE()->IsAspectLocked(); }
	BOOL IsElliptical() { return !IsCircular(); }

	virtual FillControl TestColourDrop(AttrColourDrop*);

	virtual BOOL IsAGradFill() const { return TRUE; } 
	virtual BOOL NeedsToRenderAtEachBrushStroke() const; 

	// returns the number of control points required to describe this fill,
	// ie 2 for circular, 3 for elliptical fills.
	virtual INT32 GetNumberOfControlPoints() { return IsCircular() ? 2 : 3; }

	// virtual override of AttrFillGeometry::TransformTranslateObject.
	virtual void TransformTranslateObject(const ExtendParams& ExtParams);

protected:
	virtual void ValidateAttributeValue();
};

/***********************************************************************************************

>	class AttrRadialColourFill : public AttrRadialFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Radial Graduated Colour Fill Attribute class.
	SeeAlso:	RadialFillAttribute

***********************************************************************************************/

class AttrRadialColourFill : public AttrRadialFill
{
	CC_DECLARE_DYNCREATE(AttrRadialColourFill)
	
public:
	AttrRadialColourFill() : AttrRadialFill() {}
	AttrRadialColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrRadialFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	virtual Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);
	virtual BOOL IsARadialColourFill() const { return TRUE;}

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent() &&
				GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());
	}

protected:
	RadialFillAttribute Value;
};

/***********************************************************************************************

>	class AttrRadialTranspFill : public AttrRadialFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Radial Graduated Transp Fill Attribute class.
	SeeAlso:	RadialFillAttribute

***********************************************************************************************/

class AttrRadialTranspFill : public AttrRadialFill
{
	CC_DECLARE_DYNCREATE(AttrRadialTranspFill)
	
public:
	AttrRadialTranspFill() : AttrRadialFill() {}
	AttrRadialTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrRadialFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	virtual Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL NeedsToRenderAtEachBrushStroke() const; 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}

protected:
	RadialTranspFillAttribute Value;
};

/***********************************************************************************************

>	class AttrCircularColourFill : public AttrRadialColourFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Radial Graduated Fill Attribute class.
	SeeAlso:	RadialFillAttribute

***********************************************************************************************/

class AttrCircularColourFill : public AttrRadialColourFill
{
	CC_DECLARE_DYNCREATE(AttrCircularColourFill)
	
public:
	AttrCircularColourFill() : AttrRadialColourFill() {}
};

/***********************************************************************************************

>	class AttrCircularTranspFill : public AttrRadialTranspFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Radial Graduated Fill Attribute class.
	SeeAlso:	RadialFillAttribute

***********************************************************************************************/

class AttrCircularTranspFill : public AttrRadialTranspFill
{
	CC_DECLARE_DYNCREATE(AttrCircularTranspFill)
	
public:
	AttrCircularTranspFill() : AttrRadialTranspFill() {}
};

/***********************************************************************************************

>	class AttrConicalFill : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Conical Fill Attribute class.
	SeeAlso:	ConicalFillAttribute

***********************************************************************************************/

class AttrConicalFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrConicalFill)
	
public:
	AttrConicalFill() : AttrFillGeometry() {}
	AttrConicalFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform( TransformBase& );
	virtual BOOL CanTransform();

	virtual void TransformSelectedControlPoints( TransformBase&, BOOL* isARampBlob = NULL );

	// Selection Functions
	virtual void RenderFillBlobs(RenderRegion* pRender) {}
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );
	
	virtual AttributeValue* GetAttributeValue() = 0;

	virtual DocRect GetBlobBoundingRect();
	virtual FillControl CheckForControlHit(DocCoord &);	

	virtual FillControl TestColourDrop(AttrColourDrop*);

	virtual BOOL IsAGradFill() const { return TRUE; } 

	virtual AttrFillGeometry* DoColourDrop(AttrColourDrop*);

	virtual BOOL NeedsToRenderAtEachBrushStroke() const { return TRUE;}

	// returns the number of control points required to describe this fill,
	// ie 2 in the case of a conical fill.
	virtual INT32 GetNumberOfControlPoints() { return 2; }

protected:
	virtual void ValidateAttributeValue();
	virtual void RenderControl(FillControl, BOOL);	

	// DMc 22-10-99
	// Find the ramp fill point (0 - 1.0) given certain coordinates
	virtual double FindRampPoint(DocCoord &dc, DocCoord &StartPoint, DocCoord &EndPoint);

};

/***********************************************************************************************

>	class AttrConicalColourFill : public AttrConicalFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Conical Graduated Colour Fill Attribute class.
	SeeAlso:	ConicalFillAttribute

***********************************************************************************************/

class AttrConicalColourFill : public AttrConicalFill
{
	CC_DECLARE_DYNCREATE(AttrConicalColourFill)
	
public:
	AttrConicalColourFill() : AttrConicalFill() {}
	AttrConicalColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrConicalFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent() &&
				GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());
	}

protected:
	ConicalFillAttribute Value;
};

/***********************************************************************************************

>	class AttrConicalTranspFill : public AttrConicalFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Conical Graduated Transparency Fill Attribute class.
	SeeAlso:	ConicalFillAttribute

***********************************************************************************************/

class AttrConicalTranspFill : public AttrConicalFill
{
	CC_DECLARE_DYNCREATE(AttrConicalTranspFill)
	
public:
	AttrConicalTranspFill() : AttrConicalFill() {}
	AttrConicalTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrConicalFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}

protected:
	ConicalTranspFillAttribute Value;
};


/***********************************************************************************************

>	class AttrSquareFill : public AttrFillGeometry

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Square Graduated Fill Attribute class.
	SeeAlso:	SquareFillAttribute

***********************************************************************************************/

class AttrSquareFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrSquareFill)
	
public:
	AttrSquareFill() : AttrFillGeometry() {}
	AttrSquareFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform(TransformBase& );
	virtual BOOL CanTransform();

	// Functions for rendering Fill Mesh 
	virtual void RenderFillBlobs(RenderRegion* pRender) {};
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );

	virtual AttributeValue* GetAttributeValue() = 0;

	virtual FillControl TestColourDrop(AttrColourDrop *ColDrop);

	virtual BOOL IsAGradFill() const { return TRUE; } 

	virtual DocRect GetBlobBoundingRect();
	virtual BOOL NeedsToRenderAtEachBrushStroke() const { return TRUE;}

	// returns the number of control points required to describe this fill,
	// ie 3 in the case of a square fill.
	virtual INT32 GetNumberOfControlPoints() { return 3; }

protected:
	virtual void ValidateAttributeValue();
};

/***********************************************************************************************

>	class AttrSquareColourFill : public AttrSquareFill

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Square Graduated Colour Fill Attribute class.
	SeeAlso:	SquareFillAttribute

***********************************************************************************************/

class AttrSquareColourFill : public AttrSquareFill
{
	CC_DECLARE_DYNCREATE(AttrSquareColourFill)
	
public:
	AttrSquareColourFill() : AttrSquareFill() {}
	AttrSquareColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrSquareFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent() &&
				GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());
	}

protected:
	SquareFillAttribute Value;
};

/***********************************************************************************************

>	class AttrSquareTranspFill : public AttrSquareFill

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Square Graduated Transparency Fill Attribute class.
	SeeAlso:	SquareFillAttribute

***********************************************************************************************/

class AttrSquareTranspFill : public AttrSquareFill
{
	CC_DECLARE_DYNCREATE(AttrSquareTranspFill)
	
public:
	AttrSquareTranspFill() : AttrSquareFill() {}
	AttrSquareTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrSquareFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}

protected:
	SquareTranspFillAttribute Value;
};



/***********************************************************************************************

>	class AttrThreeColFill : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	ThreeCol Graduated Fill Attribute class.
	SeeAlso:	ThreeColFillAttribute

***********************************************************************************************/

class AttrThreeColFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrThreeColFill)
	
public:
	AttrThreeColFill() : AttrFillGeometry() {}
	AttrThreeColFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform(TransformBase& );
	virtual BOOL CanTransform();

	// Functions for rendering Fill Mesh 
	virtual void RenderFillBlobs(RenderRegion* pRender) {};
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );

	virtual void RenderControl(FillControl Control, BOOL RenderOn);

	virtual void CycleSelection(BOOL Reverse);

	virtual FillControl CheckForControlHit(DocCoord &ClickPos);	

	virtual AttributeValue* GetAttributeValue() = 0;

	virtual FillControl TestColourDrop(AttrColourDrop *ColDrop);

	virtual void TransformSelectedControlPoints( TransformBase&, BOOL* isARampBlob = NULL );

	virtual BOOL IsAGradFill() const { return TRUE; } 

	virtual DocRect GetBlobBoundingRect();

	virtual BOOL NeedsForceToSimpleMapping() { return (FALSE); }

	// returns the number of control points required to describe this fill,
	// ie 3 in the case of a three-colour fill.
	virtual INT32 GetNumberOfControlPoints() { return 3; }

protected:
	virtual void ValidateAttributeValue();
};

/***********************************************************************************************

>	class AttrThreeColColourFill : public AttrThreeColFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	ThreeCol Graduated Colour Fill Attribute class.
	SeeAlso:	ThreeColFillAttribute

***********************************************************************************************/

class AttrThreeColColourFill : public AttrThreeColFill
{
	CC_DECLARE_DYNCREATE(AttrThreeColColourFill)
	
public:
	AttrThreeColColourFill() : AttrThreeColFill() {}
	AttrThreeColColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrThreeColFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent()	&&
				GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());
	}

protected:
	ThreeColFillAttribute Value;
};


/***********************************************************************************************

>	class AttrThreeColTranspFill : public AttrThreeColFill

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	ThreeCol Graduated Transparency Fill Attribute class.
	SeeAlso:	ThreeColFillAttribute

***********************************************************************************************/

class AttrThreeColTranspFill : public AttrThreeColFill
{
	CC_DECLARE_DYNCREATE(AttrThreeColTranspFill)
	
public:
	AttrThreeColTranspFill() : AttrThreeColFill() {}
	AttrThreeColTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrThreeColFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0)	||
				(GetEndTransp2() != NULL	&&	*GetEndTransp2() != 0));
	}

protected:
	ThreeColTranspFillAttribute Value;
};






/***********************************************************************************************

>	class AttrFourColFill : public AttrFillGeometry

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	FourCol Graduated Fill Attribute class.
	SeeAlso:	FourColFillAttribute

***********************************************************************************************/

class AttrFourColFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrFourColFill)
	
public:
	AttrFourColFill() : AttrFillGeometry() {}
	AttrFourColFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform(TransformBase& );
	virtual BOOL CanTransform();

	// Functions for rendering Fill Mesh 
	virtual void RenderFillBlobs(RenderRegion* pRender) {};
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );

	virtual void RenderControl(FillControl Control, BOOL RenderOn);

	virtual void CycleSelection(BOOL Reverse);

	virtual FillControl CheckForControlHit(DocCoord &ClickPos);	

	virtual AttributeValue* GetAttributeValue() = 0;

	virtual FillControl TestColourDrop(AttrColourDrop *ColDrop);

	virtual void TransformSelectedControlPoints( TransformBase&, BOOL* isARampBlob = NULL );

	virtual BOOL IsAGradFill() const { return TRUE; } 

	virtual DocRect GetBlobBoundingRect();

	virtual BOOL NeedsForceToSimpleMapping() { return (FALSE); }

	// returns the number of control points required to describe this fill,
	// ie 3 in the case of a four-colour fill.
	virtual INT32 GetNumberOfControlPoints() { return 3; }

protected:
	virtual void ValidateAttributeValue();
};

/***********************************************************************************************

>	class AttrFourColColourFill : public AttrFourColFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	FourCol Graduated Colour Fill Attribute class.
	SeeAlso:	FourColFillAttribute

***********************************************************************************************/

class AttrFourColColourFill : public AttrFourColFill
{
	CC_DECLARE_DYNCREATE(AttrFourColColourFill)
	
public:
	AttrFourColColourFill() : AttrFourColFill() {}
	AttrFourColColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFourColFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
	{
		return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent()	&&
				GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());
	}

protected:
	FourColFillAttribute Value;
};


/***********************************************************************************************

>	class AttrFourColTranspFill : public AttrFourColFill

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Four point graduated transparency Fill Attribute class.
	SeeAlso:	FourColFillAttribute

***********************************************************************************************/

class AttrFourColTranspFill : public AttrFourColFill
{
	CC_DECLARE_DYNCREATE(AttrFourColTranspFill)
	
public:
	AttrFourColTranspFill() : AttrFourColFill() {}
	AttrFourColTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFourColFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillBlobs(RenderRegion* pRender);

	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0)	||
				(GetEndTransp2() != NULL	&&	*GetEndTransp2() != 0)	||
				(GetEndTransp3() != NULL	&&	*GetEndTransp3() != 0));
	}

protected:
	FourColTranspFillAttribute Value;
};




/***********************************************************************************************

>	class AttrBitmapFill : public AttrFillGeometry

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Bitmap Fill Attribute class.
	SeeAlso:	BitmapFillAttribute

***********************************************************************************************/

class AttrBitmapFill : public AttrFillGeometry
{
	CC_DECLARE_DYNCREATE(AttrBitmapFill)
	
public:
	AttrBitmapFill() : AttrFillGeometry() {}
	AttrBitmapFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillGeometry(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual void Transform( TransformBase& );
	virtual BOOL CanTransform();

	virtual void TransformSelectedControlPoints( TransformBase&, BOOL* isARampBlob = NULL );

	virtual AttributeValue* GetAttributeValue() = 0;

	virtual void RenderFillBlobs(RenderRegion* pRender) {}
	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );
	virtual void SetAspectRatio(double Ratio);

	virtual void DrawEndBlobs();

	virtual FillControl CheckForControlHit(DocCoord &);	

	virtual BOOL ChangeControlTransp(AttrTranspChange *) { return FALSE; }
	virtual BOOL ChangeControlColour(AttrColourChange *) { return FALSE; }

	virtual DocRect GetBlobBoundingRect();

	virtual BOOL HidingNode();
	virtual BOOL ShowingNode();

	virtual FillControl TestColourDrop(AttrColourDrop*);

	virtual BOOL IsABitmapFill() const { return TRUE; }
	virtual BOOL IsAGradFill() const { return TRUE; } 

	virtual KernelBitmap *EnumerateBitmaps(UINT32 Count);

	virtual BOOL NeedsForceToSimpleMapping() { return(FALSE); }
	
	virtual BOOL NeedsToRenderAtEachBrushStroke() const { return FALSE;}
// Karim MacDonald 08/12/1999
// Extend functions.
// Note that if a sub-class returns TRUE from IsTypeExtendible, it must
// also provide complete implementations of ValidateExtend and Extend,
// which should usually check/extend its children.
public:
	virtual BOOL IsTypeExtendible() const { return TRUE; }
	virtual DocRect ValidateExtend(const ExtendParams& ExtParams);
	virtual void Extend(const ExtendParams& ExtParams);

	// virtual override of NodeRenderable::TransformTranslateObject.
	virtual void TransformTranslateObject(const ExtendParams& ExtParams);

	// returns the number of control points required to describe this fill,
	// ie 2 in the case of a bitmap fill.
	virtual INT32 GetNumberOfControlPoints() { return 2; }

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);

protected:
	virtual void ValidateAttributeValue();
	BOOL CopyNodeContents( AttrBitmapFill* NodeCopy );
};

/***********************************************************************************************

>	class AttrBitmapColourFill : public AttrBitmapFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Bitmap Graduated Colour Fill Attribute class.
	SeeAlso:	BitmapFillAttribute

***********************************************************************************************/

class AttrBitmapColourFill : public AttrBitmapFill
{
	CC_DECLARE_DYNCREATE(AttrBitmapColourFill)
	
public:
	AttrBitmapColourFill() : AttrBitmapFill() {}
	AttrBitmapColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrBitmapFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsColourMeshVisible(); }

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual BOOL ChangeControlColour(AttrColourChange* NewCol);
	virtual BOOL ChangeControlColour(DocColour& Col, FillControl Cntrl, AttrColourDrop * pColDrop = NULL );

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }
	virtual BOOL IsABitmapFill() const { return TRUE; }
	virtual BOOL IsABitmapColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

protected:
	virtual void RenderControl(FillControl, BOOL);	

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		KernelBitmap* pBmp = GetBitmap();
		return (pBmp == NULL || pBmp->GetBPP() != 24);
	}

protected:
	BitmapFillAttribute Value;
};

/***********************************************************************************************

>	class AttrBitmapTranspFill : public AttrBitmapFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Bitmap Graduated Colour Fill Attribute class.
	SeeAlso:	BitmapFillAttribute

***********************************************************************************************/

class AttrBitmapTranspFill : public AttrBitmapFill
{
	CC_DECLARE_DYNCREATE(AttrBitmapTranspFill)
	
public:
	AttrBitmapTranspFill() : AttrBitmapFill() {}
	AttrBitmapTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrBitmapFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;
	
	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual BOOL ChangeControlTransp(AttrTranspChange* NewTransp) 
			{ return AttrFillGeometry::ChangeControlTransp(NewTransp); }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 
	virtual BOOL IsABitmapTranspFill() const { return TRUE; }
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

protected:
	virtual void RenderControl(FillControl, BOOL);	

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return (GetTesselation() == RT_Simple ||
				(GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}

protected:
	BitmapTranspFillAttribute Value;
};



/***********************************************************************************************

>	class AttrFractalFill : public AttrBitmapFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	Fractal Fill Attribute class.
	SeeAlso:	FractalFillAttribute

***********************************************************************************************/

class AttrFractalFill : public AttrBitmapFill
{
	CC_DECLARE_DYNCREATE(AttrFractalFill)
	
	public:
		AttrFractalFill() : AttrBitmapFill() {}
		AttrFractalFill(Node* ContextNode,  
					 AttachNodeDirection Direction,    
					 BOOL Locked=FALSE, 
					 BOOL Mangled=FALSE,  
					 BOOL Marked=FALSE, 
					 BOOL Selected=FALSE): 
			AttrBitmapFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

		virtual void Transform( TransformBase& );
		virtual BOOL CanTransform();

		virtual BOOL HidingNode();
		virtual BOOL ShowingNode();

		virtual BOOL IsAFractalFill() const { return TRUE; } 

		virtual void SetAspectRatio(double Ratio);

		// This routine returns any secondary attribute that needs to be changed,
		// when this attribute is changed.
		virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);

	protected:
		virtual void ValidateAttributeValue();
		BOOL CopyNodeContents( AttrFractalFill* NodeCopy );
};



/***********************************************************************************************

>	class AttrTextureColourFill : public AttrFractalFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com> (Mike)
	Created:	22/8/94
	Purpose:	Texture fill attribute class. Acts as a base class for common code used
				by texture fills such as FractalColourFill and NoiseColourFill
				DO NOT INSTATIATE AN OBJECT OF THIS TYPE!
	SeeAlso:	AttrFractalFill

***********************************************************************************************/

class AttrTextureColourFill : public AttrFractalFill
{
	CC_DECLARE_DYNCREATE(AttrTextureColourFill)

	public:
		AttrTextureColourFill();
		AttrTextureColourFill(Node* ContextNode,  
					 AttachNodeDirection Direction,    
					 BOOL Locked=FALSE, 
					 BOOL Mangled=FALSE,  
					 BOOL Marked=FALSE, 
					 BOOL Selected=FALSE): 
			AttrFractalFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

		~AttrTextureColourFill();

		virtual void Render( RenderRegion* pRender );
		
		virtual Node* SimpleCopy();							// Copies a node
		
		virtual void  GetDebugDetails(StringBase* Str);
		virtual UINT32  GetAttrNameID(void); 

		virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
		virtual void RenderFillBlobs(RenderRegion* pRender);

		virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );
		virtual FillControl CheckForControlHit(DocCoord &);	

		virtual BOOL IsVisible() { return IsColourMeshVisible(); }

		virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }

		virtual BOOL ChangeControlColour(AttrColourChange* NewCol) 
				{ return AttrFillGeometry::ChangeControlColour(NewCol); }

		AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

		virtual BOOL IsAColourFill() const { return TRUE; }
		virtual AttributeValue* GetAttributeValue() { return NULL; }

	protected:
		virtual void RenderControl(FillControl, BOOL);	

	// Karim 13/09/2000
	// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
	public:
		virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs)
		{
			return (GetStartColour() != NULL	&&	GetStartColour()->IsTransparent() &&
					GetEndColour() != NULL		&&	GetEndColour()->IsTransparent());

		return false;
		}
};





/***********************************************************************************************

>	class AttrFractalColourFill : public AttrFractalFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Fractal Colour Fill Attribute class.
	SeeAlso:	AttrFractalFill

***********************************************************************************************/

class AttrFractalColourFill : public AttrTextureColourFill
{
	CC_DECLARE_DYNCREATE(AttrFractalColourFill)

	public:
		AttrFractalColourFill();
		AttrFractalColourFill(
					 Node* ContextNode,  
					 AttachNodeDirection Direction,    
					 BOOL Locked=FALSE, 
					 BOOL Mangled=FALSE,  
					 BOOL Marked=FALSE, 
					 BOOL Selected=FALSE): 
			AttrTextureColourFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

		~AttrFractalColourFill();

		virtual Node* SimpleCopy();							// Copies a node
		virtual UINT32 GetAttrNameID (void);
		virtual void  GetDebugDetails(StringBase* Str);
		virtual UINT32 GetNodeSize() const; 					// Returns size of node in bytes
		virtual AttributeValue* GetAttributeValue() { return &Value; }
		
		// Version 2 file format functions
		virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
		virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
		virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

	protected:
		FractalFillAttribute Value;
};





/***********************************************************************************************

>	class AttrNoiseColourFill : public AttrTextureColourFill

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Purpose:	Noisy colour Fill Attribute class.
	SeeAlso:	AttrTextureFractalFill

***********************************************************************************************/

class AttrNoiseColourFill : public AttrTextureColourFill
{
	CC_DECLARE_DYNCREATE(AttrNoiseColourFill)
	
	public:
		AttrNoiseColourFill();
		AttrNoiseColourFill(Node* ContextNode,  
					 AttachNodeDirection Direction,    
					 BOOL Locked=FALSE, 
					 BOOL Mangled=FALSE,  
					 BOOL Marked=FALSE, 
					 BOOL Selected=FALSE): 
			AttrTextureColourFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

		~AttrNoiseColourFill();

		virtual Node* SimpleCopy();							// Copies a node
		virtual UINT32 GetAttrNameID (void);
		virtual void  GetDebugDetails(StringBase* Str);
		virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes
		virtual AttributeValue* GetAttributeValue() { return &Value; }

		// Version 2 file format functions
		virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
		virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
		virtual BOOL WriteColourDefinitions (BaseCamelotFilter* pFilter);

	protected:
		NoiseFillAttribute Value;
};






/***********************************************************************************************

>	class AttrFractalColourFill : public AttrFractalFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Fractal Colour Fill Attribute class.
	SeeAlso:	AttrFractalFill

***********************************************************************************************/
/*
class AttrFractalColourFill : public AttrFractalFill
{
	CC_DECLARE_DYNCREATE(AttrFractalColourFill)
	
public:
	AttrFractalColourFill();
	AttrFractalColourFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFractalFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	~AttrFractalColourFill();

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl, ClickModifiers );
	virtual FillControl CheckForControlHit(DocCoord &);	

	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsColourMeshVisible(); }

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual BOOL ChangeControlColour(AttrColourChange* NewCol) 
			{ return AttrFillGeometry::ChangeControlColour(NewCol); }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeColour(pValue); }

	virtual BOOL IsAColourFill() const { return TRUE; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

protected:
	virtual void RenderControl(FillControl, BOOL);	

protected:
	FractalFillAttribute Value;
};
*/

/***********************************************************************************************

>	class AttrTextureTranspFill : public AttrFractalFill

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Purpose:	Transparency base class for textures
	SeeAlso:	AttrFractalFill

***********************************************************************************************/

class AttrTextureTranspFill : public AttrFractalFill
{
	CC_DECLARE_DYNCREATE(AttrTextureTranspFill)
	
public:
	AttrTextureTranspFill();
	AttrTextureTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFractalFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	~AttrTextureTranspFill();

	virtual void Render( RenderRegion* pRender );
	virtual Node* SimpleCopy();							// Copies a node
	virtual void GetDebugDetails(StringBase* Str);
	virtual UINT32 GetAttrNameID(void); 

	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl&, ClickModifiers );
	virtual FillControl CheckForControlHit(DocCoord &);	

	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return NULL; }

	virtual BOOL ChangeControlTransp(AttrTranspChange* NewTransp) 
			{ return AttrFillGeometry::ChangeControlTransp(NewTransp); }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 

protected:
	virtual void RenderControl(FillControl, BOOL);	

// Karim 13/09/2000
// Virtual override of NodeRenderable::IsSeeThrough() - see base declaration for details.
public:
	virtual BOOL IsSeeThrough(BOOL CheckIndirectAttrs) {
		return ((GetStartTransp() != NULL	&&	*GetStartTransp() != 0)	||
				(GetEndTransp() != NULL		&&	*GetEndTransp() != 0));
	}
};




/***********************************************************************************************

>	class AttrFractalTranspFill : public AttrTextureTranspFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Fractal Transparency Fill Attribute class.
	SeeAlso:	AttrTextureTranspFill

***********************************************************************************************/

class AttrFractalTranspFill : public AttrTextureTranspFill
{
	CC_DECLARE_DYNCREATE(AttrFractalTranspFill)

	public:
		AttrFractalTranspFill();
		AttrFractalTranspFill(
					 Node* ContextNode,  
					 AttachNodeDirection Direction,    
					 BOOL Locked=FALSE, 
					 BOOL Mangled=FALSE,  
					 BOOL Marked=FALSE, 
					 BOOL Selected=FALSE): 
			AttrTextureTranspFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

		~AttrFractalTranspFill();

		virtual Node* SimpleCopy();							// Copies a node
		virtual UINT32 GetAttrNameID (void);
		virtual UINT32 GetNodeSize() const; 					// Returns size of node in bytes
		virtual AttributeValue* GetAttributeValue() { return &Value; }
		virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);
		
		// Version 2 file format functions
		virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
		virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	protected:
		FractalTranspFillAttribute Value;
};





/***********************************************************************************************

>	class AttrNoiseTranspFill : public AttrTextureTranspFill

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Purpose:	Noisy transparent Fill Attribute class.
	SeeAlso:	AttrTextureTranspFill

***********************************************************************************************/

class AttrNoiseTranspFill : public AttrTextureTranspFill
{
	CC_DECLARE_DYNCREATE(AttrNoiseTranspFill)
	
	public:
		AttrNoiseTranspFill();
		AttrNoiseTranspFill(Node* ContextNode,  
					 AttachNodeDirection Direction,    
					 BOOL Locked=FALSE, 
					 BOOL Mangled=FALSE,  
					 BOOL Marked=FALSE, 
					 BOOL Selected=FALSE): 
			AttrTextureTranspFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

		~AttrNoiseTranspFill();

		virtual Node* SimpleCopy();							// Copies a node
		virtual UINT32 GetAttrNameID (void);
		virtual UINT32 GetNodeSize() const; 					// Returns size of node in bytes
		virtual AttributeValue* GetAttributeValue() { return &Value; }
		virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

		// Version 2 file format functions
		virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
		virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

	protected:
		NoiseTranspFillAttribute Value;
};







/***********************************************************************************************

>	class AttrFractalTranspFill : public AttrFractalFill

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/94
	Purpose:	Fractal Transparency Fill Attribute class.
	SeeAlso:	AttrFractalFill

***********************************************************************************************/
/*
class AttrFractalTranspFill : public AttrFractalFill
{
	CC_DECLARE_DYNCREATE(AttrFractalTranspFill)
	
public:
	AttrFractalTranspFill();
	AttrFractalTranspFill(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFractalFill(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	~AttrFractalTranspFill();

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
	void GetDebugDetails(StringBase* Str);
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual void RenderFillMesh(RenderRegion*, DocCoord* , BOOL*, INT32 NumControlPoints = 0);
	virtual void OnControlDrag( DocCoord, FillControl, ClickModifiers );
	virtual FillControl CheckForControlHit(DocCoord &);	

	virtual void RenderFillBlobs(RenderRegion* pRender);
	virtual BOOL IsVisible() { return IsTranspMeshVisible(); }
	virtual BOOL NeedsTransparency() const;

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillGeometry); }
	virtual AttributeValue* GetAttributeValue() { return &Value; }

	virtual BOOL ChangeControlTransp(AttrTranspChange* NewTransp) 
			{ return AttrFillGeometry::ChangeControlTransp(NewTransp); }

	AttrFillGeometry* ChangeAttrValue(AttrValueChange* pValue) { return ChangeTransp(pValue); }

	virtual BOOL IsATranspFill() const { return TRUE; } 

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

protected:
	virtual void RenderControl(FillControl, BOOL);	

protected:
	FractalTranspFillAttribute Value;
};
*/


/***********************************************************************************************

>	class AttrFillMapping : public NodeAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Fill mapping Attribute class.
	SeeAlso:	FillMappingAttribute

***********************************************************************************************/

class AttrFillMapping : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrFillMapping)
	
public:
	AttrFillMapping() {}
	AttrFillMapping(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		NodeAttribute (ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual INT32 operator==(const NodeAttribute& NodeAttrib);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillMapping); }

	virtual void SetRepeat(INT32 Rep) { ((FillMappingAttribute*)GetAttributeValue())->Repeat = Rep; }
	virtual INT32 GetRepeat() { return ((FillMappingAttribute*)GetAttributeValue())->Repeat; }

//Mark Howitt, 8/10/97. Include repeat value in debug tree
#ifdef _DEBUG
	virtual void GetDebugDetails(StringBase* Str);
#endif

	
	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);
};

/***********************************************************************************************

>	class AttrFillMappingLinear : public AttrFillMapping

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Linear fill mapping Attribute class.
	SeeAlso:	FillMappingLinearAttribute

***********************************************************************************************/

class AttrFillMappingLinear : public AttrFillMapping
{
	CC_DECLARE_DYNCREATE(AttrFillMappingLinear)
	
public:
	AttrFillMappingLinear() : AttrFillMapping() {}
	AttrFillMappingLinear(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillMapping(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	BOOL CopyNodeContents( AttrFillMappingLinear* NodeCopy );

public:
	FillMappingLinearAttribute Value;
};


/***********************************************************************************************

>	class AttrFillMappingSin : public AttrFillMapping

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Sinusoidal fill mapping class Attribute class.
	SeeAlso:	FillMappingSinAttribute

***********************************************************************************************/

class AttrFillMappingSin : public AttrFillMapping
{
	CC_DECLARE_DYNCREATE(AttrFillMappingSin)
	
public:
	AttrFillMappingSin() : AttrFillMapping() {}
	AttrFillMappingSin(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillMapping(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	BOOL CopyNodeContents( AttrFillMappingSin* NodeCopy );

public:
	FillMappingSinAttribute Value;
};


/***********************************************************************************************

>	class AttrFillEffect : public NodeAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Fill effect Attribute class.
	SeeAlso:	FillEffectAttribute

***********************************************************************************************/

class AttrFillEffect : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrFillEffect)
	
public:
	AttrFillEffect() {}
	AttrFillEffect(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		NodeAttribute (ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual INT32 operator==(const NodeAttribute& NodeAttrib);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrFillEffect); }
};

/***********************************************************************************************

>	class AttrFillEffectFade : public AttrFillEffect

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	RGB Fade fill effect Attribute class.
	SeeAlso:	FillEffectFadeAttribute

***********************************************************************************************/

class AttrFillEffectFade : public AttrFillEffect
{
	CC_DECLARE_DYNCREATE(AttrFillEffectFade)
	
public:
	AttrFillEffectFade() {}
	AttrFillEffectFade(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	BOOL CopyNodeContents( AttrFillEffectFade* NodeCopy );

public:
	FillEffectFadeAttribute Value;
};

/***********************************************************************************************

>	class AttrFillEffectRainbow : public AttrFillEffect

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Rainbow (HSV) fill effect Attribute class.
	SeeAlso:	FillEffectRainbowAttribute

***********************************************************************************************/

class AttrFillEffectRainbow : public AttrFillEffect
{
	CC_DECLARE_DYNCREATE(AttrFillEffectRainbow)
	
public:
	AttrFillEffectRainbow() {}
	AttrFillEffectRainbow(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	BOOL CopyNodeContents( AttrFillEffectRainbow* NodeCopy );

public:
	FillEffectRainbowAttribute Value;
};

/***********************************************************************************************

>	class AttrFillEffectAltRainbow : public AttrFillEffect

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Alternate Rainbow (HSV) fill effect Attribute class.
	SeeAlso:	FillEffectAltRainbowAttribute

***********************************************************************************************/

class AttrFillEffectAltRainbow : public AttrFillEffect
{
	CC_DECLARE_DYNCREATE(AttrFillEffectAltRainbow)
	
public:
	AttrFillEffectAltRainbow() {}
	AttrFillEffectAltRainbow(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrFillEffect(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	BOOL CopyNodeContents( AttrFillEffectAltRainbow* NodeCopy );

public:
	FillEffectAltRainbowAttribute Value;
};


/***********************************************************************************************

>	class AttrTranspFillMapping : public NodeAttribute

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Fill mapping Attribute class.
	SeeAlso:	FillMappingAttribute

***********************************************************************************************/

class AttrTranspFillMapping : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrTranspFillMapping)
	
public:
	AttrTranspFillMapping() {}
	AttrTranspFillMapping(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		NodeAttribute (ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual INT32 operator==(const NodeAttribute& NodeAttrib);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrTranspFillMapping); }

	virtual void SetRepeat(INT32 Rep) { ((TranspFillMappingAttribute*)GetAttributeValue())->Repeat = Rep; }
	virtual INT32 GetRepeat() { return ((TranspFillMappingAttribute*)GetAttributeValue())->Repeat; }

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);
};

/***********************************************************************************************

>	class AttrTranspFillMappingLinear : public AttrTranspFillMapping

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Linear fill mapping Attribute class.
	SeeAlso:	TranspFillMappingLinearAttribute

***********************************************************************************************/

class AttrTranspFillMappingLinear : public AttrTranspFillMapping
{
	CC_DECLARE_DYNCREATE(AttrTranspFillMappingLinear)
	
public:
	AttrTranspFillMappingLinear() : AttrTranspFillMapping() {}
	AttrTranspFillMappingLinear(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrTranspFillMapping(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);
	virtual BOOL HasEquivalentDefaultValue(BOOL bAppearance = FALSE);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	BOOL CopyNodeContents( AttrTranspFillMappingLinear* NodeCopy );

public:
	TranspFillMappingLinearAttribute Value;
};


/***********************************************************************************************

>	class AttrTranspFillMappingSin : public AttrTranspFillMapping

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/07/94
	Purpose:	Sinusoidal fill mapping class Attribute class.
	SeeAlso:	TranspFillMappingSinAttribute

***********************************************************************************************/

class AttrTranspFillMappingSin : public AttrTranspFillMapping
{
	CC_DECLARE_DYNCREATE(AttrTranspFillMappingSin)
	
public:
	AttrTranspFillMappingSin() : AttrTranspFillMapping() {}
	AttrTranspFillMappingSin(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		AttrTranspFillMapping(ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 

    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

	// Version 2 file format functions
	virtual BOOL WritePreChildrenWeb(BaseCamelotFilter* pFilter);
	virtual BOOL WritePreChildrenNative(BaseCamelotFilter* pFilter);

private:
	BOOL CopyNodeContents( AttrTranspFillMappingSin* NodeCopy );

public:
	TranspFillMappingSinAttribute Value;
};



/***********************************************************************************************

>	class AttrMould : public NodeAttribute

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/03/95
	Purpose:	Mould Attribute class.
	SeeAlso:	MouldAttribute

***********************************************************************************************/

class AttrMould : public NodeAttribute
{
	CC_DECLARE_DYNCREATE(AttrMould)
	
public:
	AttrMould() {}
	AttrMould(NodeMould* pMould, MouldGeometry* pMouldShape);
	AttrMould(Node* ContextNode,  
				 AttachNodeDirection Direction,    
				 BOOL Locked=FALSE, 
				 BOOL Mangled=FALSE,  
				 BOOL Marked=FALSE, 
				 BOOL Selected=FALSE): 
		NodeAttribute (ContextNode, Direction, Locked, Mangled, Marked, Selected) {}

	virtual INT32 operator==(const NodeAttribute& NodeAttrib);

	virtual CCRuntimeClass* GetAttributeType() { return CC_RUNTIME_CLASS(AttrMould); }

	void Render( RenderRegion* pRender );
	Node* SimpleCopy();							// Copies a node
	virtual UINT32 GetAttrNameID(void); 
    virtual UINT32 GetNodeSize() const; 			// Returns size of node in bytes

	MouldGeometry* GetMouldShape()	{ return Value.GetMouldShape(); }
	void SetMouldShape(MouldGeometry* pMould) { Value.SetMouldShape(pMould); }

	NodeMould* GetParentMould()	{ return Value.GetParentMould(); }
	void SetParentMould(NodeMould* pMould) { Value.SetParentMould(pMould); }

	virtual void PolyCopyNodeContents(NodeRenderable* pNodeCopy);

private:
	BOOL CopyNodeContents( AttrMould* NodeCopy );

public:
	MouldAttribute Value;
};


/********************************************************************************************

>	class AttrChangedMsg: public Msg

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Purpose:	This message is sent whenever an attribute changes in some way.
				It is used to update the Grad Fill Info Bar.
	SeeAlso:	-

********************************************************************************************/

class AttrChangedMsg: public Msg
{
	CC_DECLARE_DYNAMIC(AttrChangedMsg);
public:

	AttrFillGeometry* pChangedFill;
	AttrChangedMsg(AttrFillGeometry* pFill): pChangedFill(pFill) { } 
};

/********************************************************************************************

>	class FillAttrBlobState : public ListItem

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/96
	Purpose:	Used to record the fill blob selection state to determine wether apply ops
				should be merged or not.
	SeeAlso:	-

********************************************************************************************/

class FillAttrBlobState : public ListItem
{
	public:
		 FillAttrBlobState();
		~FillAttrBlobState();

		BOOL operator==(FillAttrBlobState& BlobState);

	public:
		AttrFillGeometry* pFillAttr;
		UINT32  count;
		BOOL* SelectionState;
};

/********************************************************************************************

>	class FillBlobSelectionState

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/96
	Purpose:	Used to record the fill blob selection state to determine wether apply ops
				should be merged or not.
	SeeAlso:	-

********************************************************************************************/

class FillBlobSelectionState : public CC_CLASS_MEMDUMP
{
	CC_DECLARE_MEMDUMP(FillBlobSelectionState);

public:  
	FillBlobSelectionState(); 
	~FillBlobSelectionState(); 	 						// Deletes lists 
	BOOL Record(); 	     								// Records the current selected state
	BOOL operator==(FillBlobSelectionState& SelState); 	// Compares this SelectionState with another to determine
														// if they are the same. 
	void DeleteAll() { BlobStateList.DeleteAll(); }

private:	

	CCRuntimeClass* FillType;			// Visible fill type
	UINT32	BlobCount;					// Number of blobs selected
	List	BlobStateList;				// Details of the selected blobs
}; 


#endif  // INC_FILLATTR
