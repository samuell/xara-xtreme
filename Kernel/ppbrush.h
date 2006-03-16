// $Id: ppbrush.h 662 2006-03-14 21:31:49Z alex $
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

// PathProcessorBrush header

#ifndef PPBRUSH
#define	PPBRUSH

#include "pathpcs.h"
#include "brshcomp.h"
#include "ndbldpth.h"
#include "brshattr.h"
#include "brpress.h"

class AttrBrushType;
class BrushAttrValue;
class Pathshape;
class BlendPathOffset;
class BaseCamelotFilter;
class MTRand;
class BrushData;
class HandleBrushBecomeA;
class AttrFlatTranspFill;
class CNamedColourReplacer;
class CHSVChanger;
class CTransparencyReplacer;
class CDistanceSampler;
class ValueFunction;
class CSampleItem;

typedef std::vector< UINT32 >	UINTArray;
const INT32 DEFAULT_ARRAY_SIZE = 500;
typedef std::list< CPathPointInfo > CPathPointList;

#define USE_MTRNG  // uncomment when we resolve the problems with the MTRand object


/********************************************************************************************

>	class PathProcessorBrush : public PathProcessor

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/12/96

	Purpose:	This path processor renders the new brush objects in place of the paths that 
				are passed to it.  It contains many variant members which can affect the placing,
				scaling and other aspects of the objects that are rendered.  The ink objects that 
				are actually rendered come from a BrushDefinition object. 
				The path processor maintains a pointer to an AttrBrushType which is the attribute
				node that 'owns' the processor.

	SeeAlso:	RenderRegion::DrawPath; PathProcessor::ProcessPath

********************************************************************************************/

class PathProcessorBrush : public PathProcessor
{
#ifdef NEWFASTBRUSHES
private:
	BrushAttrValue* pLinkedAttr;
public:
	void SetLinkedAttribute (BrushAttrValue* attr) { pLinkedAttr = attr; }
#endif

CC_DECLARE_DYNAMIC(PathProcessorBrush);

public:
	PathProcessorBrush();
	~PathProcessorBrush(); 

public:			// RenderRegion path-processing interface
	virtual void ProcessPath(Path *pPath, RenderRegion *pRender,
							PathShape ShapePath = PATHSHAPE_PATH);
	virtual BOOL WillChangeFillAndBrushSeparately(void);

	virtual BOOL IsAPathProcessorBrush();
public:			// Specialised interface
	virtual BOOL IsDifferent(PathProcessorBrush *pOther);
			// Returns TRUE if the pOther is considered to be a different processor type

	virtual PathProcessorBrush *Clone(void);
			// Creates a copy of this object. Must be overridden by derived classes!

	virtual BOOL NeedsTransparency(void) const {return FALSE;};
			// Returns TRUE if this path processor makes use of transparency

	virtual BOOL IsBlended() { return FALSE;}

	virtual BOOL DoBecomeA(BecomeA* pBecomeA, Path* pPath, Node* pParent);

	virtual BOOL DoesActuallyDoAnything(RenderRegion* pRender); 
			// returns TRUE if the PP actually alters the path to be rendered

public: // write processor details to a record
	 BOOL WriteNative(BaseCamelotFilter *pFilter);


public: // calculates the bounding box of this brush applied to the given path
	DocRect CalculateBoundingBox(Path* pPath, NodeRenderableInk* pParent);
	DocRect	TimeStampBoundingBox();

	void    SetCalculateBoundingBox(BOOL Set);  // tells the PPB to calculate the bounding rect on its next render
	DocRect GetCachedRect();    // returns the cached bounding rect

public: // assorted helpful public funs.
	INT32 GetObjectCountToDistance(MILLIPOINT Distance);
	MILLIPOINT GetDistanceToObject(UINT32 ObjectNum);
	
	// gets distance to the next object on the given path after Distance
	MILLIPOINT GetSpacingAtDistance(MILLIPOINT Distance);

	BOOL GetSpacingAndScalingAtDistance(MILLIPOINT Distance, MILLIPOINT* pSpacing,
										double* pScaling, UINT32* pPressure = NULL);

	// advances this brush to the given distance
	BOOL AdvanceBrushToDistance(MILLIPOINT Distance);
	UINT32 GetLastObject(); // retrieves the last object along the path

	// is there a PPB with the same handle on the renderstack?
	BOOL		IsThisPathProcessorOnTheStack(RenderRegion* pRender);

	void		AdjustSpacingForClosedPath(NodeRenderableInk* pInk);
	void		AdjustSpacingForClosedPath(Path* pPath, double dPathLength);

public:  // access functions
	// set/retrieve the brush definition handle for this PPB
	void		SetBrushDefinition(BrushHandle Handle);
	BrushHandle GetBrushDefinitionHandle(void);
	// retrieve the actual definition itself
	virtual BrushDefinition* GetOurBrushDefinition();
public:
	// spacing access fns
	MILLIPOINT	GetSpacing(void);
	BOOL	    SetSpacing(MILLIPOINT Spacing);
	BOOL		SetSpacingIncrProp(double Incr);
	double		GetSpacingIncrProp();
	BOOL		SetSpacingIncrConst(MILLIPOINT Incr);
	MILLIPOINT  GetSpacingIncrConst();
	UINT32		GetSpacingMaxRand();
	BOOL		SetSpacingMaxRand(UINT32 Value);
	UINT32		GetSpacingRandSeed();
	BOOL		SetSpacingRandSeed(UINT32 Value);
	void		SetSpacingRandCache(UINTArray* pArray);
	BOOL		SetSpacingMaxPressure(UINT32 NewMax);
	UINT32		GetSpacingMaxPressure();
	MILLIPOINT  GetLastSpacing();

public:
	// offset access fns
	PathOffset  GetPathOffsetType(void);
	MILLIPOINT  GetPathOffsetValue(void);
	void	    SetPathOffsetType(PathOffset Offset);
	void	    SetPathOffsetValue(MILLIPOINT Value);
	BOOL		SetPathOffsetIncrProp(double Incr);
	double		GetPathOffsetIncrProp();
	BOOL		SetPathOffsetIncrConst(MILLIPOINT Incr);
	MILLIPOINT  GetPathOffsetIncrConst();

	void		SetOffsetTypeRandCache(UINTArray* pArray);
	UINT32		GetOffsetTypeRandSeed();
	BOOL		SetOffsetTypeRandSeed(UINT32 Seed);
	UINT32		GetOffsetValueMaxRand();
	BOOL		SetOffsetValueMaxRand(UINT32 Value);
	UINT32		GetOffsetValueRandSeed();
	BOOL		SetOffsetValueRandSeed(UINT32 Seed);
	void		SetOffsetValueRandCache(UINTArray* pArray);
public:
	// tiling functions
	void	    SetTiling(BOOL Value);
	BOOL	    IsTiled();

public: // rotation access
	BOOL	    SetRotationAngle(double Angle);
	double	    GetRotationAngle();
	double      GetRotationIncrConst();
	BOOL		SetRotationIncrConst(double Value);
	double		GetRotationIncrProp();
	BOOL		SetRotationIncrProp(double Value);
	void	    SetRotated(BOOL Value);
	BOOL	    IsRotated();
	UINT32		GetRotationMaxRand();
	BOOL		SetRotationMaxRand(UINT32 Value);
	UINT32		GetRotationRandSeed();
	void		SetRotationRandSeed(UINT32 Seed);
	UINT32		GetRotationMaxPressure();
	BOOL		SetRotationMaxPressure(UINT32 Value);

public: // scaling access
	double		GetBrushScaling();
	BOOL		SetBrushScaling(double Scale);
	double		GetBrushScalingIncr();
	BOOL	    SetBrushScalingIncr(double Incr);
	double		GetBrushScalingIncrConst();
	BOOL		SetBrushScalingIncrConst(double Value);	
	UINT32		GetScalingMaxRand();
	BOOL		SetScalingMaxRand(UINT32 Value);
	UINT32		GetScalingRandSeed();
	BOOL		SetScalingRandSeed(UINT32 Value);
	//void		SetScalingRandCache(UINTArray* pArray);
	UINT32		GetScalingMaxPressure();
	BOOL		SetScalingMaxPressure(UINT32 Max);
	double		GetLastScaling();

	void		SetScaleToLineWidth(BOOL Value);  
	BOOL		IsScalingToLineWidth() { return m_bScaleToLineWidth;}

	void		ScaleToValue(MILLIPOINT Width, BOOL IgnorePressure = FALSE);
	double		GetScaleValue(MILLIPOINT Width);

public: // colour fns.
	double      GetHueIncrement();
	BOOL		SetHueIncrement(double Incr);
	UINT32		GetHueMaxRand();
	BOOL		SetHueMaxRand(UINT32 Max);
	UINT32		GetHueRandSeed();
	BOOL		SetHueRandSeed(UINT32 Seed);

	double		GetSatIncrement();
	BOOL		SetSatIncrement(double Incr);
	UINT32		GetSatMaxRand();
	BOOL		SetSatMaxRand(UINT32 Max);
	UINT32		GetSatRandSeed();
	BOOL		SetSatRandSeed(UINT32 Seed);


public :	// sequence fns
	SequenceType GetSequenceType();
	void	     SetSequenceType(SequenceType Type);
	UINT32		 GetSequenceSeed();
	void		 SetSequenceSeed(UINT32 Seed);

public:  // use local attribute fns.
	BOOL		GetUseLocalFillColour();
	void		SetUseLocalFillColour(BOOL UseLocal);

	BOOL		GetUseLocalTransp();
	void		SetUseLocalTransp(BOOL);

	BOOL		GetUseNamedColours();
	void		SetUseNamedColours(BOOL Value);

public: // transparency access
	INT32		GetBrushTransparency();
	BOOL		SetBrushTransparency(INT32 Value);
	UINT32		GetTransparencyPressure();
	BOOL		SetTransparencyPressure(UINT32 Value);

public: // other access fns.
	// number of ink objects in this brush
	UINT32		GetNumBrushObjects();	

	// set/retrieve parent attribute pointer
	void		SetParentAttribute(AttrBrushType* pAttr);
	AttrBrushType* GetParentAttribute();
	
	void		SetMaxScaling(double Scale);

	void		SetCurrentPressure(UINT32 Pressure);
	UINT32		GetCurrentPressure();

	void		CopyBrushData(BrushData* pData);
	void		CopyDataFromObject(BrushData* pData);

	MILLIPOINT  GetBrushSize(BOOL IgnorePressure = FALSE); //gets the size of the brush, to use as equivalent linewidth

	void SetStrokeColour(DocColour Col); //tells the PP what stroke colour to use

public: 
	// draws a single brush object at the given point, needs to be public as it is called by OpDrawBrush
	void RenderBrushAtPoint(DocCoord Point, double Angle, 
							UINT32 BrushObjectIndex, UINT32 Counter, RenderRegion* pRender, 
							HandleBrushBecomeA* pBecomeA = NULL, BOOL RealtimeBrush = FALSE,
							BrushDefinition* pBrushDefinition = NULL);
	
	// resets the RNG's to the start of their sequences
	BOOL ResetRandomNumberGenerators();

	CDistanceSampler* GetPressureCacheAndInitialise();
	CDistanceSampler* GetPressureCacheAndInitialise(RenderRegion* pRegion, ValueFunction** ppValFunc);
	CDistanceSampler* GetPressureCacheAndInitialise(NodeRenderableInk* pInkNode, ValueFunction** ppValFunc);

public: // these functions reset member variables to the state they should be in at the start
	// of a rendering pass.  They also need to be called by OpDrawBrush hence public status
	void SwitchAlternateValue();
	void Reset();

	// returns maximum and minimum limits on our random range, called by CBrushEditDlg hence public
	static void GetRandomLimits(UINT32 Base, UINT32 MaxRand, UINT32* pLower, UINT32* pUpper);
	static void GetRotationRandomLimits(UINT32 Base, UINT32 MaxRand, UINT32* pLower, UINT32* pUpper);

protected:  // helper functions
	
	// renders the attributes into the render region
	void RenderAttributes(RenderRegion* pRender, CCAttrMap* pAttrMap, 
						  CHSVChanger* pReplacer, CTransparencyReplacer* pTranspRep);

	// makes the variations on fill attributes but without rendering
	void AdjustFillAttributes(CCAttrMap* pAttrMap, CHSVChanger* pReplacer, CTransparencyReplacer* pTransRep);

	// adjusts the position of the point to be rendered according to the offset
	DocCoord AdjustPositionForOffset(DocCoord OriginalPosition, double Angle, double Scaling, 
									MILLIPOINT, UINT32 OffsetType);
							 
	// if we have multiple blendpaths (ie a group) this takes care of the offset of each 
	// blendpath from the centre of the group
	DocCoord AdjustPositionForBlendPathOffset(DocCoord OriginalPosition, BlendPathOffset* pOffset, 
											double Scaling, double Angle);

	// determine which points on the line are to be rendered
	void AddPointsInRectToList(DocRect Rect, Path* pPath, CPathPointList * pList);
	void AddAllPointsToList(Path* pPath, CPathPointList* pList);

	// retrieves the point at the given distance along thepath
	BOOL GetPointAtDistance(Path* pPath, MILLIPOINT Distance, DocCoord* pPoint, 
												double* pTangent);
	
	void RenderTimeStampPoints(RenderRegion* pRender);

	// works out how many objects we can fit in the specified distance
	INT32		HowManyObjectsInDistance(MILLIPOINT Distance);

	UINT32		GetCurrentPressureEffect(UINT32 MaxEffect, BOOL FixedMax = FALSE);  // works out as a % the effect of the current pressure

	// uses the rand function to get a random sequence, largely superceded by the use of the MTRand objects
/*	UINT32		GetFirstRandomNumber(UINT32 Seed = 0);
	UINT32		GetNextRandomNumber();
	UINT32		GetNextRandomNumberScaled(UINT32 MaxValue, UINT32 MinValue = 0);
*/
	// does our transparency adjustments and renders into the region
	void		AdjustAndRenderTransparency(RenderRegion* pRender, AttrFillGeometry* pAttrTransp, 
											CTransparencyReplacer* pRep);			

protected: // functions to determine transformations to use when rendering the next 
			// brush object.
	double		GetNextAngle();
	MILLIPOINT	GetNextOffset();
	double		GetNextSaturationMultiplier();
	double		GetNextHueMultiplier();
	double		GetNextTransparencyAdjust();
	double		GetNextPressureTranspAdjust();

	// tests to see if a brush object is within the given rect
	BOOL		ObjectIsWithinRect(DocRect Rect, DocCoord Centre, MILLIPOINT HalfWidth,
							MILLIPOINT HalfHeight, double Scaling, double Angle = 0);

	CSampleItem GetNextSampleItem(CDistanceSampler* pSampler, ValueFunction* pValFunc, double PathProportion = 0);

public: // helper fns. but they need to be public as they are used by OpDrawBrush
	double GetNextScaling(UINT32 Index);
	MILLIPOINT GetNextSpacing();
	UINT32   GetNextInkObject(UINT32 LastObject, UINT32 NumObjects);
	UINT32   GetNextOffsetType();
	void   SetNextOffsetType();
	
	// sets up arrays etc. to record our cached transformation data
	BOOL		PrepareForRenderingLoop(UINT32 NumBrushObjects);
	void		CleanUpAfterRender();

	BOOL		SetLastRenderedData(UINT32 BrushObject, double Scaling, double Tangent, double Rotation);
	void		DecideWhetherToUseActualScaling(UINT32 BrushObject);
	

protected:  //DATA  
	BrushHandle m_BrushHandle; // identifies which brush definition to use

	// Helper members

	// These members are used to store the last brush effect value that were rendered with,
	// so that we can work out subsequent values
	double		m_LastScalingRendered;

	BOOL*		m_pObjectRendered;  // array to hold flags which are set after we render the first object
	BOOL		m_UseActualScaling;  // set when we should use our absolute scaling value rather
									 // than a relative one (see GetNextScaling)
	
	double*		m_pLastScalingRendered; // holds the last scaling value for each object

	double*		m_pLastTangent;  // stores the last tangent for each object
	double*		m_pLastRotation; //stores the last rotation for each object

	DocColour	m_CurrentStrokeCol;  // the current stroke colour in our render region (used for overriding colours)

	BOOL		m_DisableMe;     // flag set when we don't want to process anything

	// brush specific data, used to determine the appearance of the brush. Most
	// of these can be set by the user one way or another

/***************Spacing values***************/
	MILLIPOINT			m_BrushSpacing;  // the basic spacing along the path
	double				m_BrushSpacingIncrProp; // the proportional spacing increment - NOT CURRENTLY IN USE!
	MILLIPOINT			m_BrushSpacingIncrConst; // the constant spacing increment - NOT CURRENTLY IN USE!
	UINT32				m_BrushSpacingMaxRand; // the upper bound for spacing randomness    
	UINT32				m_BrushSpacingRandSeed; // the random seed for spacing
	UINT32				m_SpacingMaxPressure;
	MILLIPOINT			m_LastSpacing;  //records the last brush spacing

	BOOL				m_bTile;  // do we tile the fill attribute?
	BOOL			    m_bRotate; //do we rotate tangentially along the path
	
/****************Rotation values************/	
	double				m_RotateAngle; // sets a constant rotation angle (in degrees)
	double				m_RotAngleIncrProp; // - NOT CURRENTLY IN USE!
	double				m_RotAngleIncrConst;
	double				m_LastAngle;
	UINT32				m_RotationMaxRand; 
	UINT32				m_RotationRandSeed; 
	UINT32				m_RotationMaxPressure;

/****************Offset values****************/
	PathOffset			m_PathOffsetType;  // the type of offset pattern (see enum in brshcomp.h)
	MILLIPOINT			m_PathOffsetValue; // size of the offset
	double				m_PathOffsetIncrProp; // proportional offset increment - NOT CURRENTLY IN USE!
	MILLIPOINT			m_PathOffsetIncrConst; // Constant offset increment - NOT CURRENTLY IN USE!
	UINT32				m_OffsetMaxPressure;
	MILLIPOINT			m_LastOffset;
	UINT32				m_LastOffsetType;
	UINT32				m_OffsetTypeRandSeed;
	UINT32				m_OffsetValueMaxRand; // maximum random offset value
	UINT32				m_OffsetValueRandSeed;
	
/********************Scaling values******************/
	double				m_BrushScaling;    // how much to scale the objects by
	double				m_BrushScalingIncr; // the scaling increment - NOT CURRENTLY IN USE!
	double				m_BrushScalingIncrConst; //the constant increment as a percentage of the original scaling - NOT CURRENTLY IN USE!
	double				m_LastScaling;		// the last scaling
	double				m_CurrentScaling;
	UINT32				m_BrushScalingMaxRand; // the upper bound for scaling randomness    
	UINT32				m_BrushScalingRandSeed; // the random seed for scaling
	UINT32				m_ScalingMaxPressure;   // the extent to which pressure affects scaling

/********************Hue and saturation values********/
	double				m_BrushHueIncrement;  // the amount that the hue increases/decreases each time - NOT CURRENTLY IN USE!
	UINT32				m_BrushHueMaxRand;    // the maximum random amount of hue
	UINT32				m_BrushHueRandSeed;
	UINT32				m_HueMaxPressure;
	double				m_LastHueMultiplier;      //UNUSED
	double				m_BrushSatIncrement;  // as above but for saturation - NOT CURRENTLY IN USE!
	UINT32				m_BrushSatMaxRand;     
	UINT32				m_BrushSatRandSeed;
	UINT32				m_SatMaxPressure;
	double				m_LastSatMultiplier;   //UNUSED

/*********************Sequence values********************/
	SequenceType		m_SequenceType;     // the type of sequence (if we have > 1 ink object)
	UINT32				m_SequenceRandSeed; // if we have a random sequence this is the seed
	BOOL				m_MirrorSeqAscend; // if we have a mirror sequence are we ascending?


	BOOL				m_bUseLocalFillColour;  //flag to say whether or not to ignore our cached fill colour and use the local line
												// colour 
	BOOL				m_bUseLocalTransp;      //as above but for transparencies

	BOOL				m_bUseNamedColours;     // same again for named colours

/**********************Transparency values**************/
	INT32				m_BrushTransparency;    // the new transparency control
	UINT32				m_TranspMaxPressure;

	UINT32				m_TimeStampMaxPressure; // not sure if we will ever use this 

protected: // helper member variables, these are not visible to the user

	UINT32				m_AltValue;    // keeps track of alternating offsets

	MILLIPOINT          m_LineWidth;   // the line width of the path we are processing
	BOOL				m_bScaleToLineWidth; // do we scale to the line width?
	
	BOOL				m_bRenderAllAttributes; // flag to say whether or not to render all applied attributes 
	                                            // (for simple brushes we can get away with rendering them only once)
	AttrBrushType*      m_pParentAttr;     // pointer to the attribute node that we belong to

	PressureItem		m_CurrentPressure;  // records the current pressure value for the 
											// object currently being rendered (-1 means don't use pressure)

	DocRect             m_CachedRect;       // stores our cached bounding rect
	BOOL				m_bCalculateRect;   // flag indicating whether or not we are currently calculating the bounding rect

	BOOL				m_bValidPressure;   // set when we get a proper pressure cache

	AttrFlatTranspFill*	m_pDefaultTransp;   // render this to wipe out previous transparencies

	MILLIPOINT			m_LastSpacingNoRandom; // records the amount of randomness added to spacing by the random effect
	MILLIPOINT			m_LastOffsetNoRandom; 
	double				m_LastScalingNoRandom;
	double				m_LastRotationNoRandom;

	double				m_MaxScaling;			// the scaling that will keep the size of objects to MAX_BRUSH_SIZE

	UINT32				m_LastObject;          // for multiple object brushes, keeps track of the last object in the sequence

#ifdef USE_MTRNG // the random number generators
	MTRand*             m_pSpacingRNG;   
	MTRand*				m_pScalingRNG;
	MTRand*				m_pOffsetTypeRNG;
	MTRand*				m_pOffsetValueRNG;
	MTRand*				m_pRotationRNG;
	MTRand*				m_pSaturationRNG;
	MTRand*				m_pHueRNG;
	MTRand*				m_pSequenceRNG;
#endif
	
};



/***********************************************************************************************

>	class BrushSubRenderContext: public SubRenderContext

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/8/2000
	Purpose:	Used during rendering to store the render context of a brush node if it has
				taken too long to render so far.

***********************************************************************************************/

PORTNOTE("other","SubRenderContext removed")
#ifndef EXCLUDE_FROM_XARALX
class BrushSubRenderContext: public SubRenderContext
{
CC_DECLARE_DYNCREATE(BrushSubRenderContext);
public:
	BrushSubRenderContext() { m_Distance = 0; }

	MILLIPOINT m_Distance;
};
#endif

#endif
