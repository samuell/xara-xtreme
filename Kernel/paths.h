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
// Header file for the Path Classes


#ifndef INC_PATHS
#define INC_PATHS

#include "doccoord.h"
#include "docrect.h"
#include "handles.h"
#include "pathtype.h"
#include "attr.h"
#if !defined(EXCLUDE_FROM_XARLIB)
#include "gconsts.h"
#endif

class RenderRegion;
class NodePath;
class Operation;
class ProcessFlags;
class CCAttrMap;

// The path can store a number of channels of extra information, eg Pressure.
// These extra channels are defined here ...
const INT32 NUMEXTRACHANNELS = 1;	// Number of Extra Channels Possible

// Possibly temporary constant, the max differencing rate when walking a curve
const UINT32 MAXPATHDIFFRATE = 64;


typedef enum
{
	CI_PRESSURE
} ChannelIndex;

typedef UINT32 PathExtraElement;
typedef UINT32 PathWidth;

const PathExtraElement EXTRAVALUEMAX = 65535;

/********************************************************************************************

>	class PathExtraInfo

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/06/94
	Purpose:	Creation, manipulation and use of extra information on paths, eg. Pressure.
				A path will have a pointer to an instance of this class if it has some extra
				information associated with it.
				The extra info is stored as a number of channels, each of which is an array
				of 'PathExtraElements' (currently UINTs).  These channels can then be mapped
				to one of the known 'provider data' types, eg, Line Width, Colour etc.

********************************************************************************************/

class PathExtraInfo
{
friend class Path;
public:
	// construction etc
	PathExtraInfo();
	~PathExtraInfo();

	// Functions to add and access Extra Info
	BOOL Init(ChannelIndex, INT32);
	BOOL Add(ChannelIndex, INT32);
	void Sync(INT32);
	void Next() { CurrentExtraPos++; }

	// Functions to determine what extra info is availble
	BOOL HasWidthInfo();

	// Functions to alter the mapping of extra info
	void MapWidthInfo(ChannelIndex);

	// Functions to access the mapped path data
	PathWidth GetWidthInfo();
	PathWidth* GetWidthArray();

private:
	// Functions for messing about with the Extra Info Blocks
	BOOL IncreaseExtraBlocks(INT32);
	BOOL DecreaseExtraBlocks(INT32);
	void ShiftUpExtraInfo(INT32, INT32, INT32);
	void ShiftDownExtraInfo(INT32, INT32, INT32);
	void CopyExtraInfo(PathExtraInfo*);

private:
	// If the path has Extra Info in it then the Memory handle will be
	// in the ExtraInfoHandles array.  See ChannelIndex definition above.
	MHANDLE ExtraInfoHandles[NUMEXTRACHANNELS];

	FIXED16 Scaling[NUMEXTRACHANNELS];

	INT32 CurrentExtraPos;

	// Here we define the Variables used to map the channels
	ChannelIndex WidthChannel;		// The channel to use for Width Information
};
	
typedef enum
{
	JOIN_START,		// The Join is at the Start of the path
	JOIN_END,		// The Join is at the End of the path
	JOIN_MIDDLE,	// The Join is at the Start or End of an internal Sub Path
	JOIN_NONE		// There is no Join
} JoinType;


struct JoinInfo
{
	JoinType	JoinedAt;		// Where the join is
	INT32			JoinPos;		// The coord number of the join this is only really useful
								// when JoinedAt is JOIN_MIDDLE
};



enum PathTypeEnum {PATHTYPE_NONE, PATHTYPE_SHAPE, PATHTYPE_LINE };

/********************************************************************************************

<	POINTFLAG
<	POINTFLAG_ENDPOINTS
<	POINTFLAG_CONTROLPOINTS
<	POINTFLAG_ENDSFIRST

These are the flags that get passed to the routine FindNearestPoint to allow me to filter out
control points and that sort of thing. They can be ORed together.

POINTFLAG_ENDPOINTS is set if the caller wants endpoints returned (in this context, an endpoint
is any of a moveto, lineto or the third point on a curveto. Sometimes referred to as Anchor points)

POINTFLAG_CONTROLPOINTS is set if the routine should return bezier curve control points.

POINTFLAG_ENDSFIRST is set if the routine should look at all endpoints first, before it
looks at control points. If clear, all points in the path are checked in order.

*********************************************************************************************/

#define POINTFLAG_ENDPOINTS		0x0001	// return endpoints
#define POINTFLAG_CONTROLPOINTS	0x0002	// return control points
#define POINTFLAG_ENDSFIRST		0x0004	// look at endpoints first


/********************************************************************************************

>	class Path

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/01/94
	Purpose:	Creation, manipulation and use of Paths. A path is an arbitary collection of
				MoveTo, LineTo and CurveTo segments
				Public Member variables :-
				BOOL IsFilled - TRUE if this path if filled, FALSE otherwise

********************************************************************************************/

class Path
{
public:
	// construction etc
	Path();
	~Path();
	
	// copy functions
	BOOL CopyPathDataFrom( Path* SrcPath );
	BOOL CopyPathDataFrom( DocCoord* Coords, PathVerb* Verbs, INT32 NumCoords, 
						   BOOL Filled = FALSE, BOOL Stroked = TRUE);

	// Create from DocRect function
	BOOL CreatePathFromDocRect(DocRect* pRect);

	// initialisation
	BOOL Initialise(INT32 InitialSize=24, INT32 BlockSize=12);

	// Searching Functions
	BOOL FindStartOfPath();
	void FindStartOfSubPath();
	void FindStartOfSubPath(INT32* Index) const;
	void FindEndOfSubPath();
	void FindEndOfSubPath(INT32* Index) const;
	void FindEndElOfSubPath(INT32* Index) const;
	void ExpandRange(INT32* start, INT32* end, INT32 num);

	BOOL FindNext();
	BOOL FindNext(INT32* Index) const;
	BOOL FindPrev();
	BOOL FindPrev(INT32* Index) const;
	BOOL FindPrevEndPoint(INT32* index) const;
	BOOL FindNextEndPoint(INT32* index) const;
	BOOL GetSubPathEnds(DocCoord* start, DocCoord* end);
	BOOL FindNearestPoint(DocCoord pos, UINT32 flags, INT32* position);
	BOOL PointCloseToLine(DocCoord pos, INT32* position);
	BOOL ClosestSelectedEndpoint(DocCoord pos, INT32* position, double* distance);
	BOOL FindNextSelected(INT32* Index);
	BOOL FindSelectionLimits(INT32 index, INT32* p, INT32* q);
	INT32 FindOppositeControlPoint(INT32 ThisIndex);
	INT32 FindPrevControlPoint(INT32 position);
	INT32 FindNextControlPoint(INT32 position);
	
	double SqrDistanceToPoint(const DocCoord pt, INT32* NearEl, double* mu);
	DocCoord ClosestPointTo(const double t, const INT32 index);
	BOOL IsPointCloseTo(const DocCoord ICoord, INT32 range, INT32* NearElement, double* mu );
	double SqrLength();
	double GetPathLength(double dFlatness = 64.0);
	double DistReparameterise(const double sqrdist, INT32* Element);
	DocCoord ConvSqrDistToCoord(const double sqrdist);
	INT32 BreakInTwo(Path* pChildPath);
	INT32 NumSplinters();

	BOOL DistanceTo(INT32 index, double t, double* distance, double dFlatness=64.0);
	BOOL DistanceTo(DocCoord Coord, double* dist, double dFlatness=64.0);
	BOOL SplitAtPoint(const DocCoord& SplitPoint, INT32* SplitAt, INT32* NewElements);
	BOOL GetPointAtDistance(MILLIPOINT Distance, DocCoord* pPoint,double* pTangent=NULL, UINT32* pPressure = NULL);
	BOOL GetDistanceToPoint(DocCoord Point, MILLIPOINT* Distance);

	void Scale(const DocCoord dcOrigin, const double dDPI=96.0);


	INT32 GetPathPosition();
	void SetPathPosition( INT32);

	// Function to check if we are still in the path
	BOOL IsInPath();

	// Information functions
	PathVerb GetVerb() const;
	DocCoord GetCoord() const;
	PathFlags GetFlags() const;
	PathWidth GetWidth() const;
	PathTypeEnum GetPathType() const;
	
	INT32 GetPathByteLength() const;
	INT32 GetNumElements() const;
	INT32 GetNumCoords() const;
	INT32 GetNumEndPoints() const;
	INT32 GetAllOpenEnds(INT32 MaxElements, DocCoord* EndCoords) const;
	INT32 GetNumSelEndPoints() const;
	INT32 GetNumSubpaths() const;

	
	DocRect GetBoundingRect() const;
#if !defined(EXCLUDE_FROM_XARLIB)
	DocRect GetBlobRect() const;
	BOOL GetTrueBoundingRect(	DocRect* pRect,	MILLIPOINT LineWidth = 0,
												CCAttrMap* pAttrMap = NULL );
#endif

	DocCoord GetEndPoint();
	DocCoord GetControl1();
	DocCoord GetControl2();

	DocCoord*  GetCoordArray() const;
	PathVerb*  GetVerbArray()  const;
	PathFlags* GetFlagArray()  const;
	PathWidth* GetWidthArray();
	void GetPathArrays(PathVerb** pV, DocCoord** pC = NULL, PathFlags** pF = NULL);

	void GetFlags( PathFlags* NewFlags);
	void SetFlags( const PathFlags& NewFlags);
	void InitialiseFlags();
	void InitialiseFlags( const INT32 Startindex, const INT32 Len);
	void InitialiseFlags( PathVerb* pVerbs, PathFlags* pFlags, const INT32 Len);

	// Insertion Functions
	BOOL InsertMoveTo(DocCoord p1, PathFlags* NewFlags = NULL);
	BOOL InsertLineTo(DocCoord p1, PathFlags* NewFlags = NULL);
	BOOL InsertCurveTo(DocCoord p1, DocCoord p2, DocCoord p3, PathFlags* NewFlags = NULL);
	BOOL CloseSubPath();
	BOOL InsertSection(INT32 StartSlot, INT32 NumSlots);
	BOOL AddMoveTo(DocCoord p1, PathFlags* NewFlags = NULL);
	BOOL AddLineTo(DocCoord p1, PathFlags* NewFlags = NULL);
	BOOL AddCurveTo(DocCoord p1, DocCoord p2, DocCoord p3, PathFlags* NewFlags = NULL);
	BOOL AddCurveTo(DocCoord p1, PathFlags* NewFlags = NULL);

	// Deletion functions
	BOOL DeleteSection(INT32 StartSlot, INT32 NumSlots);
	BOOL DeleteElement();
	BOOL DeleteFromElement(INT32 ElementNum);
	BOOL ClearPath();
	BOOL ClearPath(BOOL compress);
	BOOL Compact();

	// Getting stuff from the flags
	BOOL IsSelected();
	BOOL IsSmooth();
	BOOL IsRotate();
	BOOL IsEndPoint();
	BOOL IsSubSelection();

	// Other Query functions
	BOOL IsNearOpenEnd(const DocRect& BlobRect, INT32* SlotNum);
	BOOL IsOpenEnd(const INT32 index);
	BOOL IsComplexPath();
	BOOL IsIsometric(const Path& OtherPath, Matrix* pTransform, const double& Tolerance = 1.0) const;
	INT32	CalcCRC();
	INT32 CalcRegionCRC(const INT32 StartIndex, const INT32 EndIndex);
	INT32 CalcSelectedCRC();
	INT32 CalcSelPointsCRC(const INT32 StartIndex, const INT32 EndIndex);
	XLONG CalcArea();

	// New Compare Function
	INT32 ComparePathToPath(Path* pComparePath, BOOL QuickCheck = TRUE);

	// Debug Functions
	void DumpPath();
	void GetDebugDetails(StringBase* Str);

#if !defined(EXCLUDE_FROM_XARLIB)
	// Rendering Functions (Blobs)
	void RenderPathBlobs(Spread* pSpread);
	void RenderPathBlobs(RenderRegion* pRender);
	void RenderPathControlBlobs(Spread* Spread, INT32 EndPtIndex);
	void RenderPathControlBlobs(RenderRegion* pRender,INT32 EndPtIndex);
	void RenderPathSelectedControlBlobs(Spread* pSpread, BOOL Removing = TRUE);
	void RenderPathSelectedControlBlobs(Spread* pSpread, RenderRegion* pRender);
	void RenderSelectedControlBlobsToPendingRegions(Spread* pSpread);
	void RenderPathPenBlobs(Spread* pSpread);
	void RenderPathPenBlobs(RenderRegion* pRegion);

	// Functions to draw components of the selection blobs
	void DrawBlob(RenderRegion*, const DocCoord&, BOOL);
	void DrawControlBlob(RenderRegion*, const DocCoord&);
	void DrawControlLine(RenderRegion*, const DocCoord&, const DocCoord&);
#endif

	// Functions to help with sub-selections
	void ClearSubSelection();
	void SetAllSubSelection();
	void EnsureSelection(BOOL UseStarts);

	// Path Smoothing functions
	void SmoothCurve(BOOL SetRenderFlags = FALSE, BOOL SnapEnds = FALSE, INT32 SnapIndex = 0);
	void CalcDoubleCurve( DocCoord&, DocCoord&, DocCoord&, DocCoord*, DocCoord*);
	void CalcPointCurve( DocCoord&, DocCoord&, DocCoord&, DocCoord*);
	void CalcPointLine( DocCoord&, DocCoord&, DocCoord&, DocCoord*);
	void CalcPointEnd( DocCoord&, DocCoord&, DocCoord*);
	void CalcRotate( DocCoord&, DocCoord*, DocCoord*, DocCoord&);

	// Manipulation functions
	void Reverse();
	void ReverseSection(INT32 StartSlot, INT32 SubPathLen);
	void TryToClose();
	void RotateElementsLeft(const INT32 Start, const INT32 End, INT32 Rotate);
	BOOL MergeTwoPaths(const Path& Other);
	BOOL MergeTwoPaths(DocCoord* OtherCoords,PathVerb* OtherVerbs,PathFlags* OtherFlags,INT32 Length,BOOL Filled);
	BOOL SplitAtPoint(const DocCoord SplitPt, INT32* SplitElement, UINT32* NumElements, PathVerb* Verbs, DocCoord* Coords);
	BOOL ChangeStartElement(INT32 StartIndex);
	void Translate(const INT32 x, const INT32 y);
	void Translate(const DocCoord& coord);

#if !defined(EXCLUDE_FROM_XARLIB)
	INT32 ClipPathToPath(const Path& Src,Path* const pDest,UINT32 Flags,
						UINT32 Tolerance = 100,double SrcFlatness = 750.0,double ClipFlatness = 750.0);
	
	BOOL StrokePathToPath(	MILLIPOINT 	LineWidth 	= 250, 
							LineCapType LineCap	  	= LineCapButt,
							JointType 	JoinStyle 	= MitreJoin, 
							DashType*	pDash	  	= NULL,
							Path* 		pDest	  	= NULL,
							double 		Flatness	= 200.0,
							BOOL 		Close	  	= FALSE);

	// MRH 25/10/00 -	New Helper function which returns a suitable flatness value that
	//					can be used in conjunction with clipping, stroking and contouring!
	double CalculateFlatnessValueFromPath(double DividerValue = 375.0, double LowerLimit = 1.0, double UpperLimit = 375.0);

	// Contour Functions - MRH 23/05/00
	// The following functions are to do with contouring paths using Gavins new Contouring code!
	INT32 InitializeContourValues(UINT32 Width, JoinStyles JoinS = JOIN_ROUND, BOOL IsAnOuterContour = TRUE,
								 double Flatness = 200.0, BOOL ClosePath = TRUE, BOOL UseContourMode = TRUE,
								 CapStyles CapS = CAPS_ROUND, UINT32 MitreLimit = 751941);
	
	INT32 GetContourForStep(Path* pDest, double StepValue = 1.0);

#endif

	// functions for working with NeedToRender flags
	void ClearNeedToRender();

	// Copy functions
	BOOL CopySectionFrom(const Path& Other, INT32 StartIndex, INT32 NumToCopy);
	BOOL CopySectionTo(Path* Dest, INT32 StartIndex, INT32 NumToCopy);
	BOOL MergeSectionFrom(INT32 DestinPos, const Path& Source, INT32 SourcePos, INT32 Length);
	BOOL MergeSectionTo(INT32 SourcePos, INT32 Length, Path* Destin, INT32 DestinPos);
	BOOL MakePathFromSection(const INT32 Start, const INT32 Length, Path* pDestin);
	BOOL MakePathFromSubPath(const INT32 lSubpathIndex, Path* pDestin);
	BOOL CloneFrom(const Path& Other);

	// Joining Functions
	BOOL SimpleJoin(Path* Other, INT32*, BOOL*, BOOL *Reversed = NULL);
	BOOL ComplexJoin(Path* Other, INT32*, BOOL*);
	BOOL ComplexToSameComplexJoin(Path* Other, INT32*, BOOL*);
	BOOL FindJoinedSubPathInfo(const DocCoord& JoinCoord, INT32* Start, INT32* Length, BOOL* JoinAtStart);
	BOOL JoinToAnother(Path* OtherPath, INT32 MainIndex, INT32 OtherIndex);

	// Merging and replacement functions
	BOOL RetroReplaceSection(INT32 StartSlot, INT32 NumSlots, Path* NewPath, BOOL KeepStart);

	// smoothing and flattening functions
	BOOL SmoothRegions(const double error, BOOL smoothall, BOOL reselect);
	BOOL SmoothSection(INT32 index1, INT32* index2, const double error, INT32 selection);
	BOOL Smooth(const double error);
	BOOL Quantise(const double threshold, Path* pOutput);
	BOOL Flatten(const double flatness, Path* pOutput);
	BOOL CreateFlatPath(const ProcessFlags& PFlags, const double flatness, Path* pOutput);

	// Making room in a path for more points
	BOOL MakeSpaceInPath(INT32 NumSlots);
	BOOL MakeSpaceInPath(INT32 NumSlots, INT32 Position);
	BOOL MakeSpaceAtEnd(INT32 NumSlots);
	BOOL OpenGap(INT32 NumSlots);
	BOOL OpenGap(INT32 NumSlots, INT32 Position);
	BOOL EnsureVolume(INT32 NumSlots);

	// Functions for external array manipulation
	BOOL ExternalArraysAdded(const INT32 Added);
	BOOL ExternalArraysReplaced(const INT32 Size);

	// Functions to determine what extra info is available
	BOOL HasWidth();

	// Functions to alter the mapping of extra info
	void MapWidth(ChannelIndex);

	// Functions to add Extra Info
	BOOL InitExtraInfo(ChannelIndex);
	BOOL AddExtraInfo(ChannelIndex, INT32);
	void SyncExtraInfo();
	void NextExtraInfo() { ExtraInfo->Next(); }

	DocCoord SmoothControlPoint(INT32 Index,
								BOOL StartAndEndSnapped = FALSE,
								BOOL JoinedToAnother = FALSE,
								BOOL OtherIsCurve = TRUE,
								const DocCoord& OtherCoord = DocCoord(0,0) );

//	void Path::SmartSmoothCurve(BOOL StartAndEndSnapped = FALSE,
//								BOOL JoinedToAnother = FALSE,
//								Path* OtherPath = NULL,
//								BOOL JoinStartOfThis = FALSE,
//								BOOL JoinStartOfOther = FALSE,
//								BOOL RecordUndo = FALSE,
//								NodePath* UndoPath = NULL,
//								Operation* UndoOperation = NULL);

	BOOL	IsSubPathClosed(INT32 Index);
	BOOL	IsClosed();

#ifdef _DEBUG
	// Validation functions
	BOOL CheckPathValid();
#endif
	BOOL EnsureValid(BOOL* ChangesMade = NULL);

private:
	// Helper functions
	BOOL InsertSectionAtEnd(const Path* Other, INT32 StartPos, INT32 NumSlots);

#if !defined(EXCLUDE_FROM_XARLIB)
	// Contour Variables - MRH 23/05/00
	INT32 m_ContourWidth;
	JoinStyles m_ContourJoinS;
	CapStyles m_ContourCapS;
	double m_ContourFlatness;
	UINT32 m_ContourMitreLimit;
	UINT32 m_ContourLength;
	BOOL m_IsAnOuterContour;
	BOOL m_DoClosePath;
	BOOL m_UseContourMode;
#endif

public:
	// Flags that effect the entire path
	BOOL IsFilled;
	BOOL IsStroked;

	INT32 GetUsedSlots () { return (UsedSlots); }

private:
	// Memory Handles for the paths actual data
	MHANDLE	VerbHandle;
	MHANDLE CoordHandle;
	MHANDLE FlagsHandle;

	// The path may have other information, eg. Pressure
	PathExtraInfo* ExtraInfo;

	// Keep track of the available memory
	INT32 UnUsedSlots;
	INT32 UsedSlots;
	INT32 SlotAllocSize;
	INT32 SlotInitSize;

	// Keep track of the current path position
	INT32 CurrentPos;
};


#endif	// INC_PATH

