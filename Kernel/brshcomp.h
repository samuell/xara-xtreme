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

// Brush Component and Definition classes

#ifndef BRSHCOMP
#define BRSHCOMP

#include "linecomp.h"
//#include "camfiltr.h" - in camtypes.h [AUTOMATICALLY REMOVED]

class BrushRef;
class BaseCamelotFilter;
class CXaraFileRecord;
class CamelotRecordHandler;
class Node;
class PathProcessorBrush;
class AttrBrushType;
class BlendedBrushDefinition;
class BrushData;
class Range;

enum ListUpdateType
{
	UPDATE_NONE,
	UPDATE_LIST,
	UPDATE_PROPORTION,
	UPDATE_POSITION,
	UPDATE_LISTANDPOSITION
};


// indicates what type of offset from the path each object will have, e.g. all left of the path,
// all right of the path, left then right etc.
enum PathOffset {OFFSET_NONE,
				 OFFSET_ALTERNATE,
				 OFFSET_LEFT,
				 OFFSET_RIGHT,
				 OFFSET_RANDOM,
				 };

// for brushes made from multiple objects this indicates the order in which the objects appear
enum SequenceType { SEQ_FORWARD,
					SEQ_BACKWARD,
					SEQ_MIRROR,
					SEQ_RANDOM};

// various minimums and maximums that are used throughout the brush files
const MILLIPOINT MIN_BRUSH_SPACING = 1000;
const MILLIPOINT MAX_BRUSH_SPACING = 432000;
const MILLIPOINT MAX_BRUSH_OFFSET  = 216000;
const MILLIPOINT DEFAULT_BRUSH_SPACING = 10000;
const double MIN_BRUSH_SCALE = 0.005;
const double MAX_BRUSH_SCALE = 20;
const UINT32 MAX_BRUSH_OBJECTS = 50;
const UINT32 MIN_BRUSH_RAND = 0;
const UINT32 MAX_BRUSH_RAND = 100;
const UINT32 OFFSETTYPE_LEFT = 1;
const UINT32 OFFSETTYPE_RIGHT = 2;
const UINT32 OFFSETTYPE_CENTRE = 3;
const double MAX_TIMESTAMP = 5000;
const double MIN_TIMESTAMP = 2;
#define MAX_ANGLE 2*PI;
#define MIN_ANGLE -2*PI;
const double MAX_ANGLE_DEG = 360;
const double MIN_ANGLE_DEG = -360;
const UINT32   MAX_PRESSURE_EFFECT = 100;
const UINT32   MIN_PRESSURE_EFFECT = 0;
const MILLIPOINT MAX_BRUSH_SIZE = 432000;
const UINT32 BRUSHFILE_NONE = 9999;

/********************************************************************************************

>	typedef LineHandle BrushHandle;

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	A BrushHandle is a unique reference to a vector Brush in the
				BrushComponent. All references to Brushs are done via BrushHandles,
				which allows us to delete & relocate Brushs without worrying about
				leaving lots of pointers to them lying about and causing havoc.

				The special handle BrushHandle_NoBrush is used for illegal handles
				and returning error states.

	SeeAlso:	BrushComponent; BrushDefinition

********************************************************************************************/

typedef UINT32 BrushHandle;

const BrushHandle BrushHandle_NoBrush = 0xffffffff;


/********************************************************************************************

>	class BrushDefinition : public CCObject

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/99

	Purpose:	Stores a vector Brush definition
				A static list of these items is held in the BrushComponent.

	SeeAlso:	BrushComponent

********************************************************************************************/

class BrushDefinition : public LineDefinition
{
CC_DECLARE_DYNAMIC(BrushDefinition)

public:
	BrushDefinition();
	BrushDefinition(Node *pBrushTree);

	~BrushDefinition();

	// generates the brush objects from m_pTree
	BOOL		GenerateBrush();
	
	BOOL		RegenerateBrush(); 

	BOOL		IsActivated();  // has this definition been activated ok?
	void		SetActivated(BOOL Value);

	// call these functions before you render with this brush and when you finish,
	// as they generate and destroy the paths and attributes that we will use for
	// rendering with
	BOOL		StartRender();
	void		StopRender();
public:
	BOOL		IsDifferent(LineDefinition *pOther);
	BOOL		AreBrushParametersIdentical(BrushDefinition* pOther);

	BrushDefinition* Copy();  // makes a copy of this definition

	BOOL		CopyInkTreeToClipboard();
	
	BOOL		UsesPressure();

	static BOOL ObjectCanCreateBrush(NodeRenderableInk* pObject);


public:  // access functions
	// spacing fns
	MILLIPOINT	GetSpacing();
	BOOL		SetSpacing(MILLIPOINT Spacing);
	BOOL		SetSpacingIncrProp(double Incr);
	double		GetSpacingIncrProp();
	BOOL		SetSpacingIncrConst(MILLIPOINT Incr);
	MILLIPOINT  GetSpacingIncrConst();
	UINT32		GetSpacingMaxRand();
	BOOL		SetSpacingMaxRand(UINT32 Value);
	UINT32		GetSpacingRandSeed();
	void		SetSpacingRandSeed(UINT32 Seed);

public: // offset fns
	PathOffset	GetPathOffsetType();
	MILLIPOINT	GetPathOffsetValue();
	void		SetPathOffsetType(PathOffset Offset);
	void		SetPathOffsetValue(MILLIPOINT Value);
	BOOL		SetPathOffsetIncrProp(double Incr);
	double		GetPathOffsetIncrProp();
	BOOL		SetPathOffsetIncrConst(MILLIPOINT Incr);
	MILLIPOINT  GetPathOffsetIncrConst();
	UINT32		GetOffsetTypeRandSeed();
	void		SetOffsetTypeRandSeed(UINT32 Seed);
	UINT32		GetOffsetValueMaxRand();
	BOOL		SetOffsetValueMaxRand(UINT32 Seed);
	UINT32		GetOffsetValueRandSeed();
	void		SetOffsetValueRandSeed(UINT32 Seed);

public: // tiling fns
	void		SetTiling(BOOL Value);
	BOOL		IsTiled();
public: // rotation fns
	BOOL		SetRotationAngle(double Angle);
	double		GetRotationAngle();
	double      GetRotationIncrConst();
	BOOL		SetRotationIncrConst(double Value);
	double		GetRotationIncrProp();
	BOOL		SetRotationIncrProp(double Value);
	void		SetRotated(BOOL Value);
	BOOL		IsRotated();
	UINT32		GetRotationMaxRand();
	BOOL		SetRotationMaxRand(UINT32 Value);
	UINT32		GetRotationRandSeed();
	void		SetRotationRandSeed(UINT32 Seed);
	UINT32		GetRotationMaxPressure();
	BOOL		SetRotationMaxPressure(UINT32 Pressure);

public: // scaling fns
	double		GetBrushScaling() ;
	BOOL		SetBrushScaling(double Scale);
	double		GetBrushScalingIncr();
	BOOL	    SetBrushScalingIncr(double Incr);
	double		GetBrushScalingIncrConst();
	BOOL		SetBrushScalingIncrConst(double Incr);
	UINT32		GetScalingMaxRand();
	BOOL		SetScalingMaxRand(UINT32 Value);
	UINT32		GetScalingRandSeed();
	void		SetScalingRandSeed(UINT32 Seed);
	UINT32		GetScalingMaxPressure();
	BOOL		SetScalingMaxPressure(UINT32 Pressure);

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

public: // timestamping fns.
	double		GetTimeStampingPeriod();
	BOOL		SetTimeStampingPeriod(double Period);

public: // transparency access
	INT32		GetBrushTransparency();
	BOOL		SetBrushTransparency(INT32 Value);
	UINT32		GetTransparencyPressure();
	BOOL		SetTransparencyPressure(UINT32 Value);

public: // assorted other access fns 
	UINT32		GetNumBrushObjects() const;
	
	DocRect		GetLargestBoundingBox();  // gets the bbox of our largest object
	DocRect		GetLargestPossibleRect(BOOL AdjustForLineWidth = FALSE); // as above, plus all possible variations in scale

	MILLIPOINT	GetDefaultLineWidth(BOOL IgnorePressure = FALSE);
	BrushRef*	GetFirstBrushRef();
	BrushRef*	GetNextBrushRef();
	BrushRef*	GetBrushRef(UINT32 Index);

	void CalculateMaxScaling();
	double GetMaxScaling();

	// copies all the data to a path processor
	void	   CopyDataToProcessor(PathProcessorBrush* pPathProc);

	BOOL		BrushContainsGroup();  // was this brush created from an object that included a group node?

	UINT32		GetBrushFileID();
	void		SetBrushFileID(UINT32 ID);

public:  // writes brush definition to a filter
	BOOL		ExportBrush(BaseCamelotFilter *pFilter, const BrushHandle Handle);


public:  // makes a brush data object out of the various members
	void InitialiseBrushData(BrushData* pData);
	BrushData* GetBrushData();

	void SetMembersFromData(BrushData Data);
protected:  // internal helper functions
	BOOL		PreExportSubTree();  // must be called just prior to exporting the subtree
	BOOL		PostExportSubTree(); // etc.

	// generates a brushref from an inknode and adds it to the array
	BOOL		AddNewObject(NodeRenderableInk* pNode);  
	
	// if we are trying to generate a brush from a brushed object, then replace the brush with a group
	BOOL		ReplaceBrushWithGroup(NodeRenderableInk* pInk, NodeGroup** ppGroup);

	void		ResetMembers();  //  sets everything to default

	void		InitialiseBrushArray(UINT32 NumObjects); // allocates the brushref array

	UINT32		GetFirstRandomNumber(UINT32 Seed = 0);
	UINT32		GetNextRandomNumber();

protected: 
	// brush specific data - these members affect the appearance of the brush and
	// are editable by the user via the BrushEdit dialog

	MILLIPOINT			m_BrushSpacing;  // the basic spacing along the path
	double				m_BrushSpacingIncrProp;  // the proportional spacing increment
	MILLIPOINT			m_BrushSpacingIncrConst; // constant spacing increment
	UINT32				m_BrushSpacingMaxRand; // the upper bound for spacing randomness    
	UINT32				m_BrushSpacingRandSeed; // the random seed for spacing
	UINT32				m_SpacingMaxPressure;

	BOOL				m_bTile;  // do we tile the fill attribute?
	BOOL			    m_bRotate; //do we rotate along the path, should really be called m_bTangent I guess
	double				m_RotateAngle;
	double				m_RotAngleIncrProp;
	double				m_RotAngleIncrConst;
	UINT32				m_RotationMaxRand;
	UINT32				m_RotationRandSeed;
	UINT32				m_RotationMaxPressure;

	PathOffset			m_PathOffsetType;  // the type of offset pattern (see enum above)
	MILLIPOINT			m_PathOffsetValue; // size of the offset
	double				m_PathOffsetIncrProp; // proportional offset increment
	MILLIPOINT			m_PathOffsetIncrConst; // Constant offset increment
	UINT32				m_OffsetTypeRandSeed;
	UINT32				m_OffsetValueMaxRand; // maximum random offset value
	UINT32				m_OffsetValueRandSeed;
	UINT32				m_OffsetMaxPressure;

	double				m_BrushScaling;    // the scale factor of the original objects
	double				m_BrushScalingIncr;  // the proportional scaling increment
	double				m_BrushScalingIncrConst; //the constant increment as a percentage of the original scaling
	UINT32				m_BrushScalingMaxRand; // the upper bound for scaling randomness    
	UINT32				m_BrushScalingRandSeed; // the random seed for scaling
	UINT32				m_ScalingMaxPressure;   // the extent to which pressure affects scaling

		
	double				m_BrushHueIncrement;  // the amount that the hue increases/decreases each time
	UINT32				m_BrushHueMaxRand;    // the maximum random amount of hue
	UINT32				m_BrushHueRandSeed;
	UINT32				m_HueMaxPressure;

	double				m_BrushSatIncrement;  // as above but for saturation
	UINT32				m_BrushSatMaxRand;
	UINT32				m_BrushSatRandSeed;
	UINT32				m_SatMaxPressure;

	INT32				m_BrushTransparency;    // the new transparency control
	UINT32				m_TranspMaxPressure;

	SequenceType		m_SequenceType;     // the type of sequence (if we have > 1 ink object)
	UINT32				m_SequenceRandSeed; // if we have a random sequence this is the seed

	double				m_TimeStampPeriod;  // the time stamping period in milliseconds (or zero if no timestamping)
	UINT32				m_TimeStampMaxPressure;


protected:  // helper member variables that are used internally
	std::vector< BrushRef * > m_BrushRefPtrArray;  // holds multiple blendrefs used to create the brush objects 
	UINT32				m_NumBrushObjects;   
	
	UINT32				m_LastBrushRefRetrieved;  // keeps track of which brushref was asked for last

	DocRect             m_LargestBoundingBox;   //BBox of the largest brush object in the definition
	BOOL				m_bInitOk;              // flag is set to TRUE if initialisation went ok

	double				m_MaxScaling;			// the scaling that will keep the size of objects to MAX_BRUSH_SIZE

	UINT32 m_DefaultFileID;  // if this brush was loaded in from a default file on start-up then it
							// will be assigned an ID indicating which file

	BOOL				m_bActivated; // flag to indicate whether the definition has been deactivated
};



/********************************************************************************************

>	class BrushComponentClass : public DocComponentClass

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97
	Purpose:	The class that gets a Brush doc component going on start up
				We need to register our class as a document component and this is where
				we do it all.
	SeeAlso:	DocComponentClass

********************************************************************************************/

class BrushComponentClass : public DocComponentClass
{
CC_DECLARE_DYNAMIC(BrushComponentClass)

public:
	static BOOL Init();
	static void DeInit();
	static BOOL LoadDefaultBrushes();

public:
	BOOL AddComponent(BaseDocument *pDoc);
};



/********************************************************************************************

>	class BrushComponent : public DocComponent

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/2/97

	Purpose:	A document component for handling Brush definitions.

				BrushComponents share a global cache of available Brush types.
				Individual instances serve as managers for the list, mainly providing
				import/export facilities for Brush definitions.

	SeeAlso:	DocComponent

********************************************************************************************/

class BrushComponent : public LineComponent
{

CC_DECLARE_DYNAMIC(BrushComponent)

public:
	BrushComponent();
	virtual ~BrushComponent();

public:
	// if you are loading brushes at start-up these must be called
	static BOOL BeginLoadingDefaultFiles();
	static BOOL LoadNextDefaultFile();
	static void EndLoadingDefaults();

public: // assorted access and such
	AttrBrushType* CreateAttributeNode(BrushHandle Handle);
	static AttrBrushType* CreateNode(BrushHandle Handle);
 
	virtual LineHandle AddNewItem(LineDefinition *pLine, BOOL AskName = FALSE);
						// adds a new brush to the component

	static UINT32 GetNumberOfBrushes() {return m_Used;}
	static BrushDefinition *FindBrushDefinition(UINT32 Handle, BOOL IncludeDeactivated = FALSE);
					// Finds the definition of a given Line

	BOOL CopyInkObjectsToClipboard(BrushHandle Handle); 
					// copies the ink objects of a brush definition to the clipboard


	virtual BOOL		NameIsUnique(const String_32& NameString);
			// checks the definitions to see if this name is unique

	virtual String_32 GetUniqueName(String_32* pName = NULL);
			// Searches to see if the name provided is already in use by a brush	

	static void CancelNewBrush();
	static void SetNewName(String_32 NewName);
public:			// Import/Export fns.
	
	virtual BOOL StartExport(BaseCamelotFilter *pFilter);
	
	 BOOL StartImportBrush(CamelotRecordHandler *pHandler, CXaraFileRecord *pRecord, UINT32 ImportHandle);

	 BOOL EndImportBrush(CamelotRecordHandler *pHandler);
				// Starts and completes import of the Atomic Brush definition

	 BOOL ExportLine(BaseCamelotFilter *pFilter, BrushHandle Handle);
				// Exports a Brush definition (if necessary)

	 BrushHandle FindImportedBrush(UINT32 ImportedHandle);
				// Finds the internal handle of the stroke definition with the given imported handle

	BrushDefinition* GetImportBrushDefinition();
	void             CleanUpAfterImport();
						// deletes the objects we create during an import
	
private:			// Global Brush list & management functions
	virtual BOOL ExpandArray(void);



	BrushDefinition* m_pImportBrushDef;  // used to temporarily store data when importing a brush

	MILLIPOINT m_StoreSpacing;
	BOOL m_StoreTile;
	BOOL m_StoreRotate;
	double m_StoreRotateAngle;
	PathOffset m_StorePathOffsetType;
	MILLIPOINT m_StoreOffsetValue;
	
	static String_32 m_NewName;  // the name of our next brush (set by the naming dialog)
	static BOOL m_bCancelNewBrush;
	static UINT32 m_CurrentDefaultFile; // keeps track of which default file we are loading in on startup

};



#endif

