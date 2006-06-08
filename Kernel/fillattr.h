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

// fillattr.h has been split into two. This contains essentially the implementation or
// specific classes. Not many files should need to include this. This MUST NOT include
// fillval.h - see fillattr2.h for the rest

#ifndef INC_FILLATTR
#define INC_FILLATTR

#include "attr.h"
#include "nodeattr.h"
#include "msg.h"
#include "bitmap.h"
#include "list.h"
#include "binds.h"
#include "clikmods.h"


class TransparencyRamp;
class ColourRamp;
class CProfileBiasGain;
class FillGeometryAttribute;
class AttrValueChange;
class AttrColourChange;
class AttrTranspChange;
class AttrColourDrop;
class FillRamp;
class AttrColFillRampChange;
class AttrTranspFillRampChange;
class UndoableOperation;
class SelectedAttrs;
class MouldGeometry;
class NodeMould;
 
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

	virtual DocColour* GetStartColour();
	virtual DocColour* GetEndColour();
	virtual DocColour* GetEndColour2();
	virtual DocColour* GetEndColour3();
	virtual UINT32*	   GetStartTransp();
	virtual UINT32*	   GetEndTransp();
	virtual UINT32*	   GetEndTransp2();
	virtual UINT32*	   GetEndTransp3();
	virtual DocCoord*  GetStartPoint();
	virtual DocCoord*  GetEndPoint();
	virtual DocCoord*  GetEndPoint2();
	virtual DocCoord*  GetEndPoint3();
	virtual CProfileBiasGain& GetProfile ();

	virtual void SetStartColour(DocColour* NewCol);
	virtual void SetEndColour(DocColour* NewCol);
	virtual void SetEndColour2(DocColour* NewCol);
	virtual void SetEndColour3(DocColour* NewCol);
	virtual void SetStartTransp(UINT32* NewTransp);
	virtual void SetEndTransp(UINT32* NewTransp);
	virtual void SetEndTransp2(UINT32* NewTransp);
	virtual void SetEndTransp3(UINT32* NewTransp);
	virtual void SetStartPoint(DocCoord* Pos);
	virtual void SetEndPoint(DocCoord* Pos);
	virtual void SetEndPoint2(DocCoord* Pos);
	virtual void SetEndPoint3(DocCoord* Pos);
	virtual void SetProfile (CProfileBiasGain& SetWith);

	virtual UINT32	GetTranspType();
	virtual UINT32	GetDPI();
	virtual UINT32	GetFractalDPI();
	virtual UINT32	GetFractalDim();
	virtual INT32    GetSeed();
	virtual FIXED16 GetGraininess();
	virtual FIXED16 GetGravity();
	virtual FIXED16 GetSquash();
	virtual BOOL    GetTileable();
	virtual double	GetNoiseScale();

	virtual void	SetTranspType(UINT32 NewType);
	virtual BOOL	SetDPI(UINT32 NewDpi);
	virtual BOOL	SetFractalDPI(UINT32 NewDpi);
	virtual void	SetFractalDim(UINT32 NewDim);
	virtual BOOL    SetSeed(INT32 NewSeed);
	virtual BOOL    SetGraininess(FIXED16 NewGrain);
	virtual BOOL    SetGravity(FIXED16 NewGrav);
	virtual BOOL    SetSquash(FIXED16 NewSquash);
	virtual BOOL    SetTileable(BOOL NewTile);
	virtual BOOL	SetNoiseScale(double ns);

	virtual BOOL RecalcFractal();
	virtual BOOL Randomise();

	virtual BOOL AttachBitmap(KernelBitmap* NewBitmap);
	virtual BOOL DetachBitmap();
	virtual KernelBitmap* GetBitmap();
	virtual BOOL CopyBitmap(KernelBitmap* CopyBitmap);

	virtual KernelBitmapRef* GetBitmapRef();

	virtual INT32 GetTesselation();
	virtual void SetTesselation(INT32 NewTess);

	virtual BOOL IsPerspective();
	virtual ColourRamp*		  GetColourRamp();
	virtual TransparencyRamp* GetTranspRamp();
	virtual void		  SetColourRamp(ColourRamp* New);
	virtual void		  SetTranspRamp(TransparencyRamp* New);

	virtual void MakePerspective(UndoableOperation* pUndoOp = NULL);
	virtual void RemovePerspective(UndoableOperation* pUndoOp = NULL);

	virtual DocColour *EnumerateColourFields(UINT32 Context);

	virtual AttributeValue* GetAttributeValue() = 0;
	virtual DocRect GetBlobBoundingRect();
	virtual ANGLE GetSecondaryAngle() { return 0; }

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
