// $Id: opdrbrsh.h 662 2006-03-14 21:31:49Z alex $
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

// class OpDrawBrush

#ifndef INC_OPDRBR
#define INC_OPDRBR

#include "opfree.h"
#include "brshcomp.h"
#include "ndbldpth.h"
#include "brshattr.h"

class AttrBrushType;
class BlendRef;
class NodeBrush;
class NodeBrushPath;
class NodeBrushMaker;
class CPathPointInfo;
class DocCoord;
class GRenderBrush;
class ProcessPathDistance;
class FreeHandTool;
class BrushPointInfo;
class CDistanceSampler;
class CSampleData;

//typedef CMap<MILLIPOINT,MILLIPOINT&,CPathPointInfo,CPathPointInfo&> PointsMap;

#define OPTOKEN_DRAWBRUSH _T("DrawBrush")
#define OPTOKEN_CHANGEBRUSH _T("ChangeBrush")

#define OPBRUSHPOINTSCACHE


/********************************************************************************************

>	class OpDrawBrush : public OpFreeHand

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/10/99
	Purpose:	Draws a freehand stroke, smooths it and applies a nice brush effect
********************************************************************************************/

class OpDrawBrush : public OpFreeHand
{
	CC_DECLARE_DYNCREATE(OpDrawBrush);

public:
	// Construction/Destruction
	OpDrawBrush();
	OpDrawBrush(FreeHandTool* pTool);
	~OpDrawBrush();

	// The all important Do functions
	void DoDrag(DocCoord Anchor, Spread*, INT32, BrushHandle Handle, FreeHandJoinInfo* pJoinInfo, Path* ToolPath, 
				GRenderBrush* pGRndRgn, ClickModifiers ClickMods);
//	virtual BOOL SnappingDrag() { return FALSE;  }
	
	// We want to know about key presses so we can see when the modifers change if the
	// mouse is not moving
//	virtual BOOL DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag);

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragPointerIdle(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, 
								Spread*, 
								BOOL Success, BOOL bSolidDrag);

	// don't render any blobs whilsts dragging - v.important or it will fall over when you
	// try and render the view into the temporary renderregion
	void RenderDragBlobs( DocRect, Spread*, BOOL bSolidDrag) {}
	
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	virtual BOOL IsBrushOp() { return TRUE;}
public : //access
	BOOL SetTimeStamp(double TimeStamp); // sets the period for our timestamp (if we're using it)

	static CDistanceSampler* GeneratePressureData(AttrBrushType* pAttrBrush, MILLIPOINT StartDistance, 
												  MILLIPOINT EndDistance, MILLIPOINT NewPathLength);

	static MILLIPOINT GetCurrentLineWidthIfNotDefault(); //gets the current default line width
protected: // helper functions
	
	void ResetMembers();

	// adds points to our internal path
	void AddPointsToPath(DocCoord Pos, Spread* pSpread);
	
	virtual void SetCursorAndStatus(CursorType CurType);

	// applies attributes to the node we create
	virtual BOOL ApplyAttributes(NodePath* NewPath, Document *pDocument);
	
	// does the work of getting our brush objects drawn and onto the screen
	void RenderStepsForTime(double TimeSlice, Spread* pSpread);
	
	

	// Initialisation helper functions
	BOOL InitialisePathProcessorBrush(BrushHandle Handle, MILLIPOINT Distance = 0);
	BOOL InitialiseProcessPathDistance();
	BOOL InitialisePressureCache();

	AttrBrushType* CreateNewAttribute();

	virtual BOOL EditBrushLists(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements);
			
	// updates the timestamping list of a brush which was created using timestamping and then edited
	BOOL EditTimeStampList(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements);
	
	// update the pressure list
	BOOL EditPressureList(NodePath* pNewPath, INT32 FirstChangedIndex, INT32 NumElements);

	virtual BOOL SimpleJoinBrush(NodePath* pNewNodePath, Path* pNewPath);
	virtual BOOL ReverseBrushPressure();


protected:  // Timestamping functions
	BOOL InitialiseTimeStampList();
	void RenderTimeStamp();

	BOOL InitialisePressureSampler();
	// smoothing functions
	BOOL ApplyRetroSmoother(NodePath* pNodePath, double SmoothAmount);

	// applies retro smoother to a brush
	BOOL RetroSmoothBrush(NodePath* pNewNodePath);

protected:	// data
	MILLIPOINT m_BrushSpacing;            // the base brush spacing
	MILLIPOINT m_LastSpacing;             // record the previous spacing (it is not always constant)
	MILLIPOINT m_DistanceSinceLastObject;  // the distance along the path since the last object rendered
	MILLIPOINT m_LastDistanceRendered;  // the distance along the path of the last object rendered
	DocCoord m_LastPoint;           // the location of the last brush object (prior to rendering)
	DocCoord m_StartPoint;          // the first point
	DocCoord m_LastPointRendered;  // location of the last point rendered
	INT32 m_NumBrushObjects;  // the number of brush objects drawn so far
	
	UINT32 m_NumInkObjects;	// the number of different ink objects that this brush contains
	UINT32 m_LastInkObjectRendered; // records which ink object was last rendered

	double m_BrushScaling;       // our base scaling

	GRenderBrush*       m_pGRenderBrush;      // the render region that renders and displays what we draw
	PathProcessorBrush*	m_pPathProcBrush;     // the path processor that controls what is drawn

//#ifdef NEWFASTBRUSHES
	BrushDefinition* m_pBrushDefinition;	// its wasteful to keep getting hold of this inside RenderBrushAtPoint
											// lets grab it in this class
	BOOL mustClearStatusBar;				// helps us to avoid stupidly updating the status bar
	Application* pApp;						// so we don't have to keep getting old of it
//#endif

	ProcessPathDistance* m_pProcPathDistance; // calculates where we are along the path
	FreeHandTool*		m_pTool;              // our parent tool

	BOOL m_bFirstPointRendered;				 // indicates whether or not the first point has been rendered
	
	MILLIPOINT m_FirstSpacing;				// the spacing to use for the first object

	INT32	m_LastPathIndexRendered;    // when we are EOR rendering the path, keep track of the last one we did

#ifdef OPBRUSHPOINTSCACHE
	PointsMap*          m_pPointsCache;		// caches the coordinates of our brush objects
#endif

protected: // Time stamping data

	double				m_TimeStamp;  // if we are timestamping this records how often we put down an object
	double				m_LastTimeStamp; // the time of the last object
	TimeStampList*      m_pTimeStampList; // stores the timestamped points
	MonotonicTime		m_Timer;

protected: 
	// pressure data

	PressureList*		m_pPressureList;

	// the new sample data
	CDistanceSampler*		m_pPressureSampler;

	UINT32 m_NumPressureVals;
private:
	
	
};

enum ChangeBrushType  { CHANGEBRUSH_NONE,
						CHANGEBRUSH_SPACING,
						CHANGEBRUSH_SPACING_MAXRAND,
						CHANGEBRUSH_SPACING_RANDSEED,
						CHANGEBRUSH_SPACING_INCRPROP,
						CHANGEBRUSH_SPACING_INCRCONST,
						CHANGEBRUSH_SCALING,
						CHANGEBRUSH_SCALING_INCR,
						CHANGEBRUSH_SCALING_INCRCONST,
						CHANGEBRUSH_SCALING_MAXRAND,
						CHANGEBRUSH_SCALING_RANDSEED,
						CHANGEBRUSH_SCALING_PRESSURE,
						CHANGEBRUSH_OFFSET_TYPE,
						CHANGEBRUSH_OFFSET_TYPE_SEED,
						CHANGEBRUSH_OFFSET_VAL,
						CHANGEBRUSH_OFFSET_INCRPROP,
						CHANGEBRUSH_OFFSET_INCRCONST,
						CHANGEBRUSH_OFFSET_MAXRAND,
						CHANGEBRUSH_OFFSET_VALSEED,
						CHANGEBRUSH_OFFSET_SEEDS,
						CHANGEBRUSH_TILED,
						CHANGEBRUSH_TANGENTIAL,
						CHANGEBRUSH_ROTATE_ANGLE,
						CHANGEBRUSH_ROTATE_INCRPROP,
						CHANGEBRUSH_ROTATE_INCRCONST,
						CHANGEBRUSH_ROTATE_MAXRAND,
						CHANGEBRUSH_ROTATE_RANDSEED,
						CHANGEBRUSH_ROTATE_PRESSURE,
						CHANGEBRUSH_NAME,
						CHANGEBRUSH_USELOCALFILLCOL,
						CHANGEBRUSH_USELOCALTRANSP,
						CHANGEBRUSH_USENAMEDCOL,
						CHANGEBRUSH_REGEN,
						CHANGEBRUSH_SEQUENCE,
						CHANGEBRUSH_SEQUENCE_RANDSEED,
						CHANGEBRUSH_TIMESTAMP,
						CHANGEBRUSH_HUE_INCR,
						CHANGEBRUSH_HUE_MAXRAND,
						CHANGEBRUSH_FILL_SEEDS,
						CHANGEBRUSH_SAT_MAXRAND,
						CHANGEBRUSH_SAT_RANDSEED,
						CHANGEBRUSH_TRANSP,
						CHANGEBRUSH_TRANSP_PRESSURE,
						CHANGEBRUSH_ALL
										};


/********************************************************************************************

>	class OpChangeBrush : public SelOperation

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/11/99
	Purpose:	base class for operations to change various features of the brush
	SeeAlso:	-

********************************************************************************************/

class OpChangeBrush : public SelOperation
{
CC_DECLARE_DYNCREATE(OpChangeBrush);

public:
	// Construction/Destruction
	OpChangeBrush();  					

	~OpChangeBrush() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void DoWithParam(OpDescriptor*,OpParam* pOpParam);
	virtual void GetOpName(String_256* OpName);

protected:
	BOOL ChangeLineWidth(const double &PropChange, NodeRenderableInk* pInk);
	// changes the line width applied to pInk by the proportion provided

private:
	ChangeBrushType ChangeType;
};

/********************************************************************************************

>	class ChangeBrushOpParam : public OpParam

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/99
	Purpose:	The param passed to OpChangeBrush::DoWithParam
	SeeAlso:	-

********************************************************************************************/

class ChangeBrushOpParam : public OpParam
{

CC_DECLARE_DYNAMIC(ChangeBrushOpParam)

public:
	ChangeBrushOpParam();	
	ChangeBrushOpParam(const ChangeBrushOpParam& Other);
	~ChangeBrushOpParam()	{}

	void InitMembers();
	BOOL ChangeLineWidth();

public: // overridables
	virtual void SetOldValues(AttrBrushType* pBrushedNode);
	virtual void SetNewValues(AttrBrushType* pBrushedNode);
	virtual ChangeBrushOpParam operator=(const ChangeBrushOpParam& Other);

	void SwapOldAndNew();
	void SwapDouble(double &First, double &Second);
	void SwapUINT(UINT32 &First, UINT32 &Second);
	void SwapINT32(INT32 &First, INT32 &Second);
	void SwapBOOL(BOOL &First, BOOL &Second);
	void SetValues(BrushData Data);

	ChangeBrushType ChangeType;

	MILLIPOINT m_OldSpacing;
	MILLIPOINT m_NewSpacing;
	double	   m_NewSpacingIncrProp;
	double	   m_OldSpacingIncrProp;
	MILLIPOINT m_NewSpacingIncrConst;
	MILLIPOINT m_OldSpacingIncrConst;
	PressureArray* m_pPressure;
	UINT32	   m_OldSpacingMaxRand;
	UINT32	   m_NewSpacingMaxRand;
	UINT32	   m_NewSpacingRandSeed;
	UINT32	   m_OldSpacingRandSeed;
	double	   m_OldScaling;
	double     m_NewScaling;
	double	   m_NewScalingIncr;
	double	   m_OldScalingIncr;
	double	   m_NewScalingIncrConst;
	double	   m_OldScalingIncrConst;
	UINT32	   m_NewScalingMaxRand;
	UINT32	   m_OldScalingMaxRand;
	UINT32	   m_NewScalingRandSeed;
	UINT32	   m_OldScalingRandSeed;
	UINT32	   m_OldScalingMaxPressure;
	UINT32	   m_NewScalingMaxPressure;
	PathOffset m_OldPathOffsetType;
	PathOffset m_NewPathOffsetType;
	UINT32	   m_NewOffsetTypeRandSeed;
	UINT32	   m_OldOffsetTypeRandSeed;
	MILLIPOINT m_OldPathOffsetVal;
	MILLIPOINT m_NewPathOffsetVal;
	double     m_NewOffsetIncrProp;
	double     m_OldOffsetIncrProp;
	MILLIPOINT m_NewOffsetIncrConst;
	MILLIPOINT m_OldOffsetIncrConst;
	UINT32	   m_NewOffsetValMaxRand;
	UINT32	   m_OldOffsetValMaxRand;
	UINT32	   m_NewOffsetValRandSeed;
	UINT32	   m_OldOffsetValRandSeed;
	BOOL	   m_bOldTiled;
	BOOL	   m_bNewTiled;
	BOOL       m_bOldRotated;
	BOOL       m_bNewRotated;
	double     m_OldRotateAngle;
	double     m_NewRotateAngle;
	double	   m_OldRotationIncrProp;
	double	   m_NewRotationIncrProp;
	double     m_OldRotationIncrConst;
	double     m_NewRotationIncrConst;
	UINT32	   m_NewRotateMaxRand;
	UINT32	   m_OldRotateMaxRand;
	UINT32	   m_NewRotateRandSeed;
	UINT32	   m_OldRotateRandSeed;
	UINT32	   m_NewRotationMaxPressure;
	UINT32	   m_OldRotationMaxPressure;
	String_32  m_OldBrushName;
	String_32  m_NewBrushName;
	BOOL	   m_bOldUseLocalFillColour;
	BOOL	   m_bNewUseLocalFillColour;
	BOOL	   m_bNewUseLocalTransp;
	BOOL	   m_bOldUseLocalTransp;
	BOOL	   m_bOldUseNamed;
	BOOL	   m_bNewUseNamed;
	SequenceType m_OldSequenceType;
	SequenceType m_NewSequenceType;
	UINT32		m_NewSequenceRandSeed;
	UINT32		m_OldSequenceRandSeed;
	double		m_NewHueIncrement;
	double		m_OldHueIncrement;
	UINT32		m_NewHueMaxRand;
	UINT32		m_OldHueMaxRand;
	UINT32		m_NewHueRandSeed;
	UINT32		m_OldHueRandSeed;
	UINT32		m_NewSatMaxRand;
	UINT32		m_OldSatMaxRand;
	UINT32		m_NewSatRandSeed;
	UINT32		m_OldSatRandSeed;
	INT32		m_NewTransp;
	INT32		m_OldTransp;
	UINT32		m_NewTranspPressure;
	UINT32		m_OldTranspPressure;

	double	m_OldTimeStampValue;
	double	m_NewTimeStampValue;
};



/********************************************************************************************

>	class ChangeBrushAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Purpose:	An action which changes one of the members of a brush attribute
	SeeAlso:	-

********************************************************************************************/

class ChangeBrushAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeBrushAction)

public:
	ChangeBrushAction();
	~ChangeBrushAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRenderableInk* pBrushedNode,
							ChangeBrushOpParam* pChangeParam,
							ChangeBrushAction** NewAction);

	BOOL ChangeSelectionNoUndo(ChangeBrushOpParam* pParam);
protected:	
	static AttrBrushType* GetAttrBrush(NodeRenderableInk* pInk);
	static BOOL ChangeBrush(ChangeBrushOpParam* pOpParam, AttrBrushType* pAttrBrush);

	// functions that set the values in the atttribute
	static BOOL ChangeBrushSpacing(MILLIPOINT NewSpacing, PressureArray* pArray, AttrBrushType* pAttrBrush);
	static BOOL ChangeSpacingIncrProp(double NewIncr, PressureArray* pArray, AttrBrushType* pAttrBrush);
	static BOOL ChangeSpacingIncrConst(MILLIPOINT NewIncr, PressureArray* pArray, AttrBrushType* pAttrBrush);
	static BOOL ChangeBrushScaling(double NewScaling, AttrBrushType* pAttrBrush);
	static BOOL ChangeScalingRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeScalingIncr(double NewIncr, AttrBrushType* pAttrBrush);
	static BOOL ChangeScalingIncrConst(double NewIncr, AttrBrushType* pAttrBrush);
	static BOOL ChangeScalingMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush);
	static BOOL ChangeScalingPressure(UINT32 Pressure, AttrBrushType* pAttrBrush);
	static BOOL ChangeTangential(BOOL bRotate, AttrBrushType* pAttrBrush);
	static BOOL ChangeRotation(double NewAngle, AttrBrushType* pAttrBrush);
	static BOOL ChangeRotationIncrProp(double Incr, AttrBrushType* pAttrBrush);
	static BOOL ChangeRotationIncrConst(double Incr, AttrBrushType* pAttrBrush);
	static BOOL ChangeRotationRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeRotationMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush);
	static BOOL ChangeRotationMaxPressure(UINT32 Pressure, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetValue(MILLIPOINT NewOffset, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetType(PathOffset NewOffset, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetTypeSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetIncrProp(double NewIncrProp, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetIncrConst(MILLIPOINT NewIncrConst, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetValMaxRand(UINT32 NewMax, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetValRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeOffsetSeeds(UINT32 NewTypeSeed, UINT32 NewOffsetSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeBrushTile(BOOL bTile, AttrBrushType* pAttrBrush);
	static BOOL ChangeSpacingMaxRand(UINT32 NewMaxRand, AttrBrushType* pAttrBrush);
	static BOOL ChangeSpacingRandSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeUseLocalFillColour(BOOL bUseLocal, AttrBrushType* pAttrBrush);
	static BOOL ChangeUseLocalTransp(BOOL bUseLocal, AttrBrushType* pAttrBrush);
	static BOOL ChangeUseNamedColour(BOOL bUse, AttrBrushType* pAttr);
	static BOOL ChangeSequenceType(SequenceType SeqType, AttrBrushType* pAttrBrush);
	static BOOL ChangeSequenceSeed(UINT32 NewSeed, AttrBrushType* pAttrBrush);
	static BOOL ChangeAll(ChangeBrushOpParam* pParam, AttrBrushType* pAttrBrush);
	static BOOL ChangeHueIncrement(double NewIncr, AttrBrushType* pAttrBrush);
	static BOOL ChangeHueMaxRand (UINT32 Rand, AttrBrushType* pAttrBrush);
	static BOOL ChangeFillSeeds (UINT32 HueSeed, UINT32 SatSeed, AttrBrushType* AttrBrush);
	static BOOL ChangeSatMaxRand (UINT32 Rand, AttrBrushType* pAttrBrush);
	static BOOL ChangeSatRandSeed (UINT32 Seed, AttrBrushType* pAttrBrush);
	static BOOL ChangeTransparency(INT32 Transp, AttrBrushType* pAttrBrush);
	static BOOL ChangeTranspPressure(UINT32 Press, AttrBrushType* pAttrBrush);

	BOOL ChangeLineWidthChildNoUndo(double PropChange, NodeRenderableInk* pInk);
		// changes the line width child of an ink node by the given proportion 
protected: // data
	NodeRenderableInk* 	m_pBrushedNode;
	ChangeBrushOpParam 	m_ChangeParam;
};


/********************************************************************************************

>	class RemoveTimeStampPointsAction : public Action

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/2000
	Purpose:	An action which removes points from the timestamping list of a BrushAttValue
	SeeAlso:	-

********************************************************************************************/

class RemoveTimeStampPointsAction : public Action
{
CC_DECLARE_DYNCREATE(RemoveTimeStampPointsAction)

public:
	RemoveTimeStampPointsAction();
	~RemoveTimeStampPointsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrBrushType* pAttrBrush,
							MILLIPOINT StartDistance,
							MILLIPOINT EndDistance,	
							TimeStampList* pPointsList,
							RemoveTimeStampPointsAction** NewAction);

public: // data
	TimeStampList* m_pTimeStampList;
	AttrBrushType* m_pAttrBrush;
	MILLIPOINT	   m_StartDistance;
	MILLIPOINT     m_EndDistance;

};


/********************************************************************************************

>	class AddTimeStampPointsAction : public Action

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/2000
	Purpose:	An action which adds points to the timestamping list of a BrushAttValue
	SeeAlso:	-

********************************************************************************************/

class AddTimeStampPointsAction : public Action
{
CC_DECLARE_DYNCREATE(AddTimeStampPointsAction)

public:
	AddTimeStampPointsAction();
	~AddTimeStampPointsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrBrushType* pAttrBrush,
							TimeStampList* pNewPoints,
							MILLIPOINT StartDistance,
							MILLIPOINT EndDistance,
							size_t NumPoints,
							AddTimeStampPointsAction** NewAction);
public:
	AttrBrushType* m_pAttrBrush;
	TimeStampList* m_pTimeStampList;
	MILLIPOINT m_StartDistance;
	MILLIPOINT m_EndDistance;
	size_t	   m_NumPoints;
};



/********************************************************************************************

>	class UpdateBrushAction : public Action

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/2000
	Purpose:	An action which signals to a brush attribute that its ink node hsa changed.
				It is appropriate to call this action when serious adjustments have been made
				to the timestamping points list of the attrbrushtype, for instance adding or
				removing subsets of the list.  It is not appropriate to call it when the coordinates
				of the path have changed but you do not wish to add or remove any points from the list.
				

	SeeAlso:	-

********************************************************************************************/

class UpdateBrushAction : public Action
{
CC_DECLARE_DYNCREATE(UpdateBrushAction)

public:
	UpdateBrushAction();
	~UpdateBrushAction();

	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodePath* pNodePath,
							UpdateBrushAction** NewAction);

public: //data
	NodePath* m_pNodePath;


};


/********************************************************************************************

>	class SetRepositionTimeStampPointsAction : public Action

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/2000
	Purpose:	The purpose of this action is to set a flag that determines what happens when
				the brushed nodepath calls its timestamping brush attribute to update itself.
				Setting this flag to TRUE indicates that our path has changed so that our 
				timestamping list coordinates are no longer on the line.  Therefore we will 
				reposition these points on the new line.  Setting it to FALSE indicates that 
				we have added or removed points from the timestamping list and should sort 
				and reorder it to make sure it is ok.
				

	SeeAlso:	-

********************************************************************************************/


class SetTimeStampUpdateTypeAction : public Action
{
CC_DECLARE_DYNCREATE(SetTimeStampUpdateTypeAction)

public:
	SetTimeStampUpdateTypeAction();
	~SetTimeStampUpdateTypeAction();

	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrBrushType* pAttrBrush,
							NodePath* pNodePath,
							ListUpdateType TypeToSet,
							SetTimeStampUpdateTypeAction** NewAction);

public: //data
	NodePath* m_pNodePath;
	AttrBrushType* m_pAttrBrush;
	ListUpdateType m_TypeToSet;

};


/********************************************************************************************

>	class RemovePressurePointsAction : public Action

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/2000
	Purpose:	An action which removes points from the pressure data list of a BrushAttValue
	SeeAlso:	-

********************************************************************************************/

class RemovePressurePointsAction : public Action
{
CC_DECLARE_DYNCREATE(RemovePressurePointsAction)

public:
	RemovePressurePointsAction();
	~RemovePressurePointsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrBrushType* pAttrBrush,
							UINT32 StartIndex,
							UINT32 NumPoints,	
							CSampleData* pSampler,
							RemovePressurePointsAction** NewAction);

public: // data
	CSampleData* m_pPressureSampler;
	AttrBrushType* m_pAttrBrush;
	MILLIPOINT	   m_StartIndex;
	MILLIPOINT     m_NumPoints;

};


/********************************************************************************************

>	class AddPressurePointsAction : public Action

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/2000
	Purpose:	An action which adds points to the Pressure list of a BrushAttValue
	SeeAlso:	-

********************************************************************************************/

class AddPressurePointsAction : public Action
{
CC_DECLARE_DYNCREATE(AddPressurePointsAction)

public:
	AddPressurePointsAction();
	~AddPressurePointsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrBrushType* pAttrBrush,
							CSampleData* pNewPoints,
							UINT32 StartIndex,
							size_t NumPoints,
							AddPressurePointsAction** NewAction);
public:
	AttrBrushType*  m_pAttrBrush;
	CSampleData*    m_pPressureSampler;
	MILLIPOINT		m_StartIndex;
	size_t			m_NumPoints;
};

#endif
