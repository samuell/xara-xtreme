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
// The Graduated Fill Tool

#ifndef INC_GRADFILL
#define INC_GRADFILL

//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "transop.h"
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fillattr2.h"
#include "biasgdgt.h"
//#include "selop.h"
#include "brshattr.h"
//#include "list.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "opbevel.h"
#include "dragtool.h"

class AttrTranspFillMapping;
class Cursor;
class GradInfoBarOp;
class TranspInfoBarOp;
class AttrFillGeometry;
class AttrFillMapping;
class AttrFillEffect;
class SelRange;
class OpApplyAttrInteractive;
class AttrStrokeTransp;

enum FillGeometry
{
	FGMENU_FLAT,
	FGMENU_LINEAR,
	FGMENU_CIRCULAR,
	FGMENU_RADIAL,
	FGMENU_CONICAL,
	FGMENU_SQUARE,
	FGMENU_THREECOL,
	FGMENU_FOURCOL,
	FGMENU_BITMAP,
	FGMENU_FRACTAL,
	FGMENU_NOISE
};

#define FGMENU_MANY -1			// cannot be included in above since user cannot select

enum TransparencyGeometry
{
	FGMENU_NOTRANSP,
	FGMENU_FLATTRANSP,
	FGMENU_LINEARTRANSP,
	FGMENU_CIRCULARTRANSP,
	FGMENU_RADIALTRANSP,
	FGMENU_CONICALTRANSP,
	FGMENU_SQUARETRANSP,
	FGMENU_THREECOLTRANSP,
	FGMENU_FOURCOLTRANSP,
	FGMENU_BITMAPTRANSP,
	FGMENU_FRACTALTRANSP,
	FGMENU_NOISETRANSP
};

#define FGMENU_MANYTRANSP -1	// cannot be included in above since user cannot select

enum
{
	FTMENU_REFLECT,
	FTMENU_SUB,
	FTMENU_ADD,
	FTMENU_CON,
	FTMENU_SAT,
	FTMENU_DARK,
	FTMENU_LIGHT,
	FTMENU_BRI,
	FTMENU_LUM,
	FTMENU_HUE,
// <-------------Include your next menu item here!!!!
	FTMENU_MAX
};

enum
{
	FMMENU_SIMPLE,
	FMMENU_REPEATING
};

enum
{
	FEMENU_FADE,
	FEMENU_RAINBOW,
	FEMENU_ALTRAINBOW
};

enum
{
	FTMENU_SIMPLE,
	FTMENU_REPEAT,
	FTMENU_REPEATINV
};

typedef enum
{
	FLATFILL,
	GRADFILL,
	GRADREPEATFILL,
	BITMAPFILL,
	FRACTALFILL,
	NOISEFILL,
	MANYFILLS,
	NOFILL
} FillMode;	

double GetDoubleGrain(INT32);
INT32 GetGrainPos(double);
INT32 GetGrainPosMax();

double GetDoubleScale(INT32);
INT32 GetScalePos(double);
INT32 GetScalePosMax();

/********************************************************************************************

>	class FillTools

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com> (based upon DMc BevelTools class)
	Created:	9/2/2000
	Purpose:	Provides functionality for generating a list of AttrFillGeometry nodes.
				This class is used by the profiling function GetProfileFromSelection
				(virtual overide).

				And I'm fed up with explaining why this class exists, so here is a short
				and blunt explanation:

				"we have to do a couple of extra things, so its much better to have the
				functionality here than it is to go mucking up things as far as bevel nodes
				etc. are concerned.

********************************************************************************************/

class FillTools
{
public:
	// builds a list of all selected nodes of given class
//	static BOOL BuildListOfSelectedNodes(List *pList,
//								const CCRuntimeClass * pClass,
//								BOOL bPromoteToParents = TRUE);
//	static BOOL BuildListOfSelectedAttrs(List *pList,
//								const CCRuntimeClass * pClass,
//								BOOL bPromoteToParents = TRUE);
//
//	// recursion down to find nodes
//	static BOOL GetAllNodesUnderNode(const Node * pNode, List * pList,
//		const CCRuntimeClass * pClass, INT32 depth = 0);
//
//	// finds the parent of the given node, the one which is a child of
//	// the spread. Used to get the root of a hierarchy.

	static BOOL GetSelectedAttrList(List* pList,
									const CCRuntimeClass* pClass,
									BOOL bGradFill,
									BOOL bTranspFill,
									EffectsStack* pUseThisStack = NULL);

private:
	static BOOL IsNodeInList(List * pList, const Node * pNode);
	static BOOL IsNodeTrueFill(const Node * pNode);
	static BOOL bScanForTransparency;
};

/********************************************************************************************

>	class CCAPI GradFillTool : public Tool_v1

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	The fill tool.

********************************************************************************************/

class CCAPI GradFillTool : public DragTool
{
// Give my name in memory dumps
CC_DECLARE_MEMDUMP(GradFillTool);

public:
	GradFillTool();
	~GradFillTool();
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_GRADFILL; };
	
	// Event Handlers
	virtual void SelectChange(BOOL);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual void OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods);
	virtual BOOL OnKeyPress(KeyPress* pKeyPress);

	virtual BOOL GetStatusLineText(String_256*, Spread*, DocCoord, ClickModifiers);
	void GetCursorAndStatus(DocCoord, Spread*, Cursor**, String_256*);

	static BOOL IsCurrentTool() { return CurrentTool; }

	void 	ToggleControlPoints(BOOL Reverse);

	static void	EnableFillNudge(); 
	static void	DisableFillNudge();
	static BOOL IsFillNudgeEnabled();

public:
	static	BOOL AllowFillNudges;

private:
	static 	TCHAR* FamilyName;	// The Tools Family Name
	static 	TCHAR* ToolName;		// The Tool Name
	static 	TCHAR* Purpose;		// What the tool is for
	static 	TCHAR* Author;		// Who wrote it

	static void DisplayStatusBarHelp(UINT32 StatusID);

	static UINT32 LastStatusID;

	static BOOL CurrentTool;
	
	// info about clicks as they arrive
	DocCoord ClickStart;		// Where the first click was clicked
	DocCoord StartPos;			
	Spread*  StartSpread;		// The spread where we received the first click

	// The bounding box of the selection
	DocRect  SelectionBox;

	// A count of the number of selected object when the tool was selected
	BOOL	 IsSelection;

	// The tools cursors
	Cursor*  pCurrentCursor;
	Cursor*  pGradFillCursor;
	Cursor*  pGradPointCursor;

	INT32 CurrentCursorID;

	static GradInfoBarOp*	pGradInfoBarOp;
	static TranspInfoBarOp*	pTranspInfoBarOp;

	AttrFillGeometry* EditingFill;
	BOOL DoubleClicked;
	BOOL OpBusy;

	static BOOL NudgeFills;
};

/********************************************************************************************

>	class CCAPI TranspTool : public Tool_v1

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/94
	Purpose:	The transparency tool.

********************************************************************************************/

class CCAPI TranspTool : public DragTool
{
// Give my name in memory dumps
CC_DECLARE_MEMDUMP(TranspTool);

public:
	TranspTool();
	~TranspTool();
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_TRANSP; };
	
	// Event Handlers
	virtual void SelectChange(BOOL);
	virtual void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	virtual void OnMouseMove(DocCoord Pos, Spread* pSpread, ClickModifiers ClickMods);
	virtual BOOL OnKeyPress(KeyPress* pKeyPress);

	virtual BOOL GetStatusLineText(String_256*, Spread*, DocCoord, ClickModifiers);
	void GetCursorAndStatus(DocCoord, Spread*, Cursor**, String_256*);

	static BOOL IsCurrentTool() { return CurrentTool; }

	void 	ToggleControlPoints(BOOL Reverse);

	static void	EnableFillNudge();
	static void	DisableFillNudge();
	static BOOL IsFillNudgeEnabled();

private:
	static 	TCHAR* FamilyName;	// The Tools Family Name
	static 	TCHAR* ToolName;		// The Tool Name
	static 	TCHAR* Purpose;		// What the tool is for
	static 	TCHAR* Author;		// Who wrote it

	static void DisplayStatusBarHelp(UINT32 StatusID);

	static UINT32 LastStatusID;

	static BOOL CurrentTool;
	
	// info about clicks as they arrive
	DocCoord ClickStart;		// Where the first click was clicked
	DocCoord StartPos;			
	Spread*  StartSpread;		// The spread where we received the first click

	// The bounding box of the selection
	DocRect  SelectionBox;

	// A count of the number of selected object when the tool was selected
	BOOL	 IsSelection;

	// The tools cursors
	Cursor*  pCurrentCursor;
	Cursor*  pTranspCursor;
	Cursor*  pTranspPointCursor;

	INT32 CurrentCursorID;

	static TranspInfoBarOp*	pTranspInfoBarOp;

	AttrFillGeometry* EditingFill;
	BOOL DoubleClicked;
	BOOL OpBusy;

	static BOOL NudgeFills;
};

/********************************************************************************************

>	class GradInfoBarOp : public InformationBarOp

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Class for handling the gradfill tool's information bar.

********************************************************************************************/

class GradInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( GradInfoBarOp )  
public:
	GradInfoBarOp(ResourceID r=_R(IDD_GRADTOOLBAR)) {DlgResID=r;}				// Dummy default constructor for DYNCREATE

	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here

	void		InitControls();		// Initialises the controls in the info bar
	void		EnableControls();	// Enables/disables the controls in the info bar

	void		ShowInfo();

	// A set of routines for displaying information in the info bar
	void		ShowFlatInfo();
	void		ShowGraduatedInfo();
//	void		ShowRepeatGraduatedInfo();
	void		ShowBitmapInfo();
	void		ShowFractalInfo();
	void		ShowNoiseInfo();

	void 		ShowCommonType(BOOL ShowDataAndNotSet = TRUE);
	void 		ShowCommonMapping(BOOL ShowDataAndNotSet = TRUE);
	void 		ShowCommonEffect(BOOL ShowDataAndNotSet = TRUE);
	void 		ShowCommonTesselate();
	void 		ShowCommonBitmapName();
	void 		ShowCommonBitmapDpi();
	void 		ShowControlPointInfo();
	void 		ShowCommonFractalGrain();
	void		ShowCommonNoiseScale();

private:
	void ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID);
	CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType);
	
	void 		ChangeFillType();
	void 		ChangeFillMapping();
	void 		ChangeFillEffect();

	void 		ChangeDPI();
	void 		ChangeFractalGrain();
	void		ChangeNoiseScale();
	void 		ChangeBitmapName();

	void		SetGadgetWritable(INT32, BOOL);
	BOOL		GetGadgetWritable (INT32);

	INT32 		FindCommonTesselate();
	String_256 	FindCommonBitmapName();
	INT32 		FindCommonBitmapDpi();
	double 		FindCommonFractalGrain();
	double		FindCommonNoiseScale();

	String_64 	GetSelectedPoint(AttrFillGeometry*);
	String_64 	GetSelectedColour(AttrFillGeometry*);
	String_64 	GetColourName(DocColour&);
	void 		ScanSelectionForControlPoints(String_64* ,String_64*);

	AttrFillGeometry* GetCurrentGeometry();
	AttrFillMapping*  GetCurrentMapping();

	AttrFillGeometry* MakeFillMutator();

	BOOL SetGadgetString(CGadgetID Gadget, StringBase* StrValue);

	void InitEffect();
	void InitMapping();
	void InitBitmapName();
	void InitTesselate();

public:
	static INT32 CurrentGeometryIndex;
	static INT32 CurrentMappingIndex;
	static INT32 CurrentEffectIndex;

	static INT32 CurrentTesselateIndex;
	static INT32 CurrentBitmapIndex;

	SelRange *Selection;		// Can't be changed in this tool
	SelRange::CommonAttribResult CommonAttr;
	SelRange::CommonAttribResult CommonAttrEffect;
	SelRange::CommonAttribResult CommonAttrMap;

private:
	FillMode Mode;
	FillMode OldMode;

	BOOL BitmapListChanged;

	BOOL MappingDisabled;
	BOOL EffectDisabled;
	BOOL AllowForceToSimpleMapping;

	AttrFillGeometry* CommonGeometry;
	AttrFillMapping* CommonMapping;
	AttrFillEffect* CommonEffect;

	INT32 SelGeometryIndex;
	INT32 SelMappingIndex;
	INT32 SelEffectIndex;

	INT32 SelPointIndex;
	INT32 SliderMax;

public:
PORTNOTE("other","filltool.cpp - removed m_BiasGainGadget use")
#ifndef EXCLUDE_FROM_XARALX
	CBiasGainGadget  m_BiasGainGadget;
#endif
};
	

/********************************************************************************************

>	class GradInfoBarOpCreate : public BarCreate

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Class for creating GradInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class GradInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new GradInfoBarOp); }
};

/********************************************************************************************

>	class TranspInfoBarOp : public InformationBarOp

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Class for handling the gradfill tool's information bar.

********************************************************************************************/

class TranspInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( TranspInfoBarOp )  
public:
	TranspInfoBarOp(ResourceID r=_R(IDD_ZOOMTOOLBAR)) {DlgResID=r; m_pAttrApplyOp = NULL; m_pDragTransp = NULL;}

	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here

	void		InitControls();		// Initialises the controls in the info bar
	void		EnableControls();	// Enables/disables the controls in the info bar

	void		ShowInfo();

	// A set of routines for displaying information in the info bar
	void		ShowFlatInfo();
	void		ShowGraduatedInfo();
//	void		ShowRepeatGraduatedInfo();
	void		ShowBitmapInfo();
	void		ShowFractalInfo();
	void		ShowNoiseInfo();

	void 		ShowCommonType(BOOL ShowDataAndNotSet = TRUE);
	void 		ShowCommonMapping(BOOL ShowDataAndNotSet = TRUE);
	void		ShowCommonTranspType (BOOL ShowDataAndNotSet = TRUE);
	void 		ShowCommonTesselate();
	void 		ShowCommonBitmapName();
	void 		ShowCommonBitmapDpi();
	void 		ShowControlPointInfo();
	void 		ShowCommonFractalGrain();
	void		ShowCommonNoiseScale();

	UINT32		GetTranspType() { return CurrentTransTypeIndex + 1; }

private:
	void ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID);
	CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType);
	
	void 		ChangeFillType();
	void 		ChangeFillMapping();
	BOOL		ChangeBitmapName();
	void 		ChangeTranspType();

	void 		ChangeDPI ();
	void		ChangeTransparencyValue ();
	void 		ChangeFractalGrain (BOOL useEditbox = FALSE);
	void		ChangeNoiseScale (BOOL useEditbox = FALSE);

	void		SetGadgetWritable(INT32, BOOL);
	BOOL		GetGadgetWritable (INT32);

	String_64 	GetSelectedPoint(AttrFillGeometry*);
	UINT32 		GetSelectedTransp(AttrFillGeometry*);
	void 		ScanSelectionForControlPoints(String_64* ,INT32*);

	INT32		FindCommonTranspType();
	INT32 		FindCommonTesselate();
	String_64 	FindCommonBitmapName();
	INT32 		FindCommonBitmapDpi();
	double 		FindCommonFractalGrain();
	double		FindCommonNoiseScale();

	void		SetTranspValue(UINT32, BOOL bIntermediateStep = FALSE, BOOL bFirstStep = FALSE);

	AttrFillGeometry* 		GetCurrentGeometry();
//	AttrPaintingMode* GetCurrentPaintingMode ();
	AttrTranspFillMapping*  GetCurrentMapping();

	AttrFillGeometry* MakeFillMutator();

	BOOL SetGadgetString(CGadgetID Gadget, StringBase* StrValue);

	void InitMapping();
	void InitBitmapName();
	void InitTesselate();
	void InitTransType();

	void DisplayTransparencyPercentString (double value);

	// bodgey fn. to let brush attributes know when transparency is being applied
	void UpdateBrushAttributes();  

public:
	static INT32 CurrentGeometryIndex;
	static INT32 CurrentTransTypeIndex;
	static INT32 CurrentMappingIndex;
	static INT32 CurrentTypeIndex;

	static INT32 CurrentTesselateIndex;
	static INT32 CurrentBitmapIndex;

	SelRange *Selection;		// Can't be changed in this tool
	SelRange::CommonAttribResult CommonAttr;
	SelRange::CommonAttribResult CommonAttrMap;

private:
	FillMode Mode;
	FillMode OldMode;

	BOOL BitmapListChanged;

	BOOL MappingDisabled;
	BOOL TypeDisabled;
	BOOL AllowForceToSimpleMapping;

	AttrFillGeometry* CommonGeometry;
	AttrFillMapping* CommonMapping;

	INT32 SelGeometryIndex;
	INT32 SelMappingIndex;
	INT32 SelTypeIndex;

	BOOL SliderDragged;
	INT32 SliderMax;

	INT32 LastSliderPos;

	OpApplyAttrInteractive* m_pAttrApplyOp;
	AttrValueChange* m_pDragTransp;

public:
PORTNOTE("other","filltool.cpp - removed m_BiasGainGadget use")
#ifndef EXCLUDE_FROM_XARALX
	CBiasGainGadget  m_BiasGainGadget;
#endif
};

/********************************************************************************************

>	class TranspInfoBarOpCreate : public BarCreate

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/94
	Purpose:	Class for creating TranspInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class TranspInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new TranspInfoBarOp); }
};

/***********************************************************************************************

>	class AttrRemoveTransp : public AttrTranspChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Purpose:	Removes the transparency from objects.
				This Attribute never exists in the Tree.  It is used for changing the Transp
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrRemoveTransp : public AttrTranspChange
{
	CC_DECLARE_DYNCREATE(AttrRemoveTransp)
	
public:
	AttrRemoveTransp() {}

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);
};

/***********************************************************************************************

>	class AttrRemoveTransp : public AttrTranspChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Purpose:	Removes the transparency from objects.
				This Attribute never exists in the Tree.  It is used for changing the Transp
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrRemoveStrokeTransp : public AttrValueChange
{
	CC_DECLARE_DYNCREATE(AttrRemoveStrokeTransp)
	
public:
	AttrRemoveStrokeTransp() : AttrValueChange() {}

	virtual AttributeValue* GetAttributeValue() { return &Value; }
	virtual CCRuntimeClass* GetAttributeType();

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

protected:
	StrokeTranspAttribute Value;
};

/***********************************************************************************************

>	class AttrMakeFlatTransp : public AttrTranspChange

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/4/94
	Purpose:	Force object to be 50% transparent
				This Attribute never exists in the Tree.  It is used for changing the Transp
				of an existing Fill Geometry.
	SeeAlso:	AttrFillGeometry

***********************************************************************************************/

class AttrMakeFlatTransp : public AttrTranspChange
{
	CC_DECLARE_DYNCREATE(AttrMakeFlatTransp)
	
public:
	AttrMakeFlatTransp() {}

	virtual AttrFillGeometry* MutateFill(AttrFillGeometry* FillToMutate);

	// This routine returns any secondary attribute that needs to be changed,
	// when this attribute is changed.
	virtual NodeAttribute* GetOtherAttrToApply(BOOL* IsMutate);
};

/***********************************************************************************************

>	class FillProfileOpParam : public OpParam

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Purpose:	Serves as our param passing stuff for undo/redo of fill profile changing.
	SeeAlso:

***********************************************************************************************/

class FillProfileOpParam : public OpParam
{
	CC_DECLARE_DYNCREATE(FillProfileOpParam)

public:
	FillProfileOpParam() { };
	~FillProfileOpParam() { };

public:
	CProfileBiasGain Profile;
	
};

/********************************************************************************************

>	class OpChangeFillProfile : public SelOperation

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Purpose:	The operation class that is responsible for changing fill profiles.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_FILLPROFILE	_T("FillProfile")				// our all important optoken!

class OpChangeFillProfile : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpChangeFillProfile)

public:
	OpChangeFillProfile();
	~OpChangeFillProfile();

	virtual void DoWithParam(OpDescriptor* pOp, OpParam* pParam);
	static OpState GetState(String_256* Description, OpDescriptor*);
	static BOOL Declare();
};

/********************************************************************************************

>	class ChangeFillProfileAction : public Action

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Purpose:	Action for changing fill profiles
	SeeAlso:	-

********************************************************************************************/

class ChangeFillProfileAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeFillProfileAction)

public:
	ChangeFillProfileAction();
	~ChangeFillProfileAction();

	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrFillGeometry* pThisNodeFill,
							CProfileBiasGain &Profile,
							ChangeFillProfileAction** NewAction,
							BOOL bReverse = TRUE,
							BOOL bCache = FALSE);

	static void ChangeFillProfileWithNoUndo (CProfileBiasGain &Profile);
protected:
	AttrFillGeometry* m_pNodeFill;
	CProfileBiasGain m_LastProfile;	
};

/***********************************************************************************************

>	class TranspFillProfileOpParam : public OpParam

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Purpose:	Serves as our param passing stuff for undo/redo of transp fill profile changing.
	SeeAlso:

***********************************************************************************************/

class TranspFillProfileOpParam : public OpParam
{
	CC_DECLARE_DYNCREATE(TranspFillProfileOpParam)

public:
	TranspFillProfileOpParam() { };
	~TranspFillProfileOpParam() { };

public:
	CProfileBiasGain Profile;
	
};

/********************************************************************************************

>	class OpChangeTranspFillProfile : public SelOperation

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Purpose:	The operation class that is responsible for changing transp fill profiles.
	SeeAlso:	-

********************************************************************************************/

#define OPTOKEN_TRANSPFILLPROFILE	_T("TranspFillProfile")			// our all important optoken!

class OpChangeTranspFillProfile : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpChangeTranspFillProfile)

public:
	OpChangeTranspFillProfile();
	~OpChangeTranspFillProfile();

	virtual void DoWithParam(OpDescriptor* pOp, OpParam* pParam);
	static OpState GetState(String_256* Description, OpDescriptor*);
	static BOOL Declare();
};

/********************************************************************************************

>	class ChangeTranspFillProfileAction : public Action

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/2000
	Purpose:	Action for changing transp fill profiles
	SeeAlso:	-

********************************************************************************************/

class ChangeTranspFillProfileAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeTranspFillProfileAction)

public:
	ChangeTranspFillProfileAction();
	~ChangeTranspFillProfileAction();

	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							AttrFillGeometry* pThisNodeTranspFill,
							CProfileBiasGain &Profile,
							ChangeTranspFillProfileAction** NewAction,
							BOOL bReverse = TRUE,
							BOOL bCache = FALSE);

	static void ChangeTranspFillProfileWithNoUndo (CProfileBiasGain &Profile);
protected:
	AttrFillGeometry* m_pNodeTranspFill;
	CProfileBiasGain m_LastProfile;	
};


#endif
