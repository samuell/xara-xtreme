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
// Header for the blend tool implementation

#ifndef INC_BLENDTOOL
#define INC_BLENDTOOL

//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "doccoord.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blendatt.h"
//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "biasgdgt.h"
//#include "iprofile.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "biasgain.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodershp.h"
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "matrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "selstate.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "dragtool.h"

class Cursor;
class Spread;
class BlendInfoBarOp;
class Node;
class NodeBlend;
class NodeBlender;
class BlendPath;
class CompoundNodeTreeFactory;

#define OPTOKEN_BLENDNODES  	 	_T("BlendNodes")
#define OPTOKEN_REMOVEBLEND 	 	_T("RemoveBlend")
#define OPTOKEN_CHANGEBLENDSTEPS 	_T("ChangeBlendSteps")
#define OPTOKEN_CHANGEBLEND	 	 	_T("ChangeBlendFlag")
#define OPTOKEN_BLENDONETOONE		_T("BlendOneToOne")
#define OPTOKEN_BLENDANTIALIAS		_T("BlendAntialias")
#define OPTOKEN_BLENDTANGENTIAL		_T("BlendTangential")
#define OPTOKEN_ADDBLENDPATH		_T("AddBlendPath")
#define OPTOKEN_DETACHBLENDPATH		_T("DetachBlendPath")
#define OPTOKEN_CHANGEBLENDPROFILE	_T("ChangeBlendProfile")
#define OPTOKEN_EDITBLENDENDOBJECT  _T("EditBlendEndobject")
#define OPTOKEN_CHANGEBLENDDISTANCE  _T("EditDistanceBetweenSteps")



/********************************************************************************************

>	class CompoundNodeTreeFactoryListItem : public ListItem

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Purpose:	Item in the CompoundNodeClassFactoryList 

********************************************************************************************/
class CompoundNodeTreeFactoryListItem : public ListItem
{
public:
	CompoundNodeTreeFactory * pFactory;
} ;

/********************************************************************************************

>	class CompoundNodeTreeFactoryList : public List

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Purpose:	A list of the compound node factories necessary to regenerate the shadow/
				bevels/other compound nodes after bevelling

********************************************************************************************/
class CompoundNodeTreeFactoryList : public List
{
public:
	~CompoundNodeTreeFactoryList();

	void AddItem(CompoundNodeTreeFactory *pItem);
	CompoundNodeTreeFactory * GetItem(INT32 index);
} ;

/********************************************************************************************

>	class BlenderInfoItem : public ListItem

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/94
	Purpose:	Holds info on a given blender.
				This is used when blending a blend to a blend, or a blend to an object, as
				all the NodeBlenders in a given NodeBlend have to be temporarily deinitialised while
				the objects are contatinated.
	SeeAlso:	ListItem

********************************************************************************************/

class BlenderInfoItem: public ListItem
{
	CC_DECLARE_MEMDUMP(BlenderInfoItem);
public:
	BlenderInfoItem() { pNodeBlender = NULL; pNodeStart = pNodeEnd = NULL; }

	NodeBlender*		pNodeBlender;
	NodeRenderableInk* 	pNodeStart;
	NodeRenderableInk* 	pNodeEnd;

	BlenderInfoItem* SimpleCopy() 	{	BlenderInfoItem *pItem = new BlenderInfoItem;
										if (pItem != NULL)
										{
											pItem->pNodeBlender = pNodeBlender;
											pItem->pNodeStart	= pNodeStart;
											pItem->pNodeEnd		= pNodeEnd;
										}
										return pItem;
								  	}
};





/********************************************************************************************

>	class BlendToolRef : public CC_CLASS_MEMDUMP

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	A class that represents a start or end of a blend.
				The start ref is used when a click occurs.
				The end ref is used at the end of a drag.

********************************************************************************************/

class BlendToolRef : public CC_CLASS_MEMDUMP
{
CC_DECLARE_MEMDUMP(BlendToolRef)

public:
	BlendToolRef() { Reset(); }
	void Reset() { 	pNode 		= NULL;
					pNodeBlend 	= NULL;
					pBlendPath	= NULL;
					pSpread 	= NULL;
					RemapRef 	= 0;
					Index 		= -1;
				 }


	NodeRenderableInk*		pNode;
	NodeBlend*				pNodeBlend;
	BlendPath*				pBlendPath;
	Spread*					pSpread;
	UINT32					RemapRef;
	INT32					Index;
	BOOL					AStartNode;
	DocCoord				PointerPos;
};


/********************************************************************************************

>	class BlendTool : public Tool_v1

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Purpose:	The blend tool class

********************************************************************************************/

class CCAPI BlendTool : public DragTool
{
// Give tool name in memory dumps
CC_DECLARE_MEMDUMP(BlendTool);

public:
	BlendTool();
	~BlendTool();

	// Standard calls your tool must provide
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_BLEND; };
	
	// Some very common event handlers
	void SelectChange(BOOL isSelected);
	void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	void OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods);
	void RenderToolBlobs(Spread* pSpread,DocRect* pDocRect);
	BOOL GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);
	BOOL OnKeyPress(KeyPress* pKeyPress);

	// Some useful static functions
	static BlendInfoBarOp* 	GetBlendInfoBarOp()	{ return pBlendInfoBarOp; }
	static BOOL 			IsCurrent();
	static void 			DisplayStatusBarHelp(UINT32 StatusID);

	static BlendToolRef*	GetPtrRefStart() 	{ return pRefStart; }
	static BlendToolRef*	GetPtrRefEnd() 	  	{ return pRefEnd; }

	static void UpdateRef(BlendToolRef* pRef,Spread* pSpread, DocCoord PointerPos,BOOL CheckNodeUnderPoint = TRUE);
	static void UpdateCursorAndStatus();
	static void CheckNodeRemapping(BlendToolRef* pRefStart, BlendToolRef* pRefEnd);

	// determines whether the pointer is in the right place to edit the 
	// end object of a blend on a path
	BOOL EditBlendEndAndUpdateCursor(Spread* pSpread, DocCoord PointerPos);
	// lightweight version doesn't update cursor
	BOOL EditBlendEnd(Spread* pSpread, DocCoord PointerPos);

	BOOL SelectedBlendIsOnCurve();
	void UpdateInfobar();
private:

	// Private blend tool functions
	static NodeRenderableInk* 	FindObject(Spread* pSpread, DocCoord ClickPos);
	static BOOL 				IsPointOverPathBlob(DocCoord* pPointerPos,BlendToolRef* pRef);
	static BOOL 				IsPointOverBlendBlob(DocCoord* pPointerPos,BlendToolRef* pRef);

	BOOL CreateCursors();						// Create your tool's cursors in this func
	void DestroyCursors();						// Destroys all blend tool cursors

	
	// added by Diccon 3/9/99
	ShapeClickEffect DetermineClickEffect(DocCoord PointerPos, Spread* pSpread, NodeRegularShape** ReturnShape);
	BOOL RenderSelectInsideBlobs(RenderRegion* pRender, NodeRenderableInk* pNode);


	static BlendInfoBarOp*	pBlendInfoBarOp;	// Ptr to your tool's infobar
	
	BOOL m_EditEndObject;						// flag to say whether we wish to edit the end objects when dragging

	// Cursors
	static Cursor*	pcNormalCursor;						// Your standard cursor to use when your tool becomes active
	static Cursor*	pcBlendableCursor;					// Cursor when mouse is over object that can be blended
	static Cursor*	pcBlendableBlobCursor;				// Cursor when mouse is over the blob of a selected path
	static Cursor*	pcBlendableRemapCursor;				// Cursor when mouse is over remappable blobs of a blend			
														
	static Cursor*	pcCurrentCursor;					// The cursor your tool is currently displaying
	static INT32		CurrentCursorID;					// The ID of our current cursor on the stack

	// Status line text ID
	static UINT32	StatusID;

	// Data gathered during clicking and dragging
	static BlendToolRef	*pRefStart;
	static BlendToolRef	*pRefEnd;

	// Standard tool static vars
	static 	TCHAR* FamilyName;				// The Tools Family Name
	static 	TCHAR* ToolName;					// The Tool Name
	static 	TCHAR* Purpose;					// What the tool is for
	static 	TCHAR* Author;					// Who wrote it
};


//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------


/********************************************************************************************

>	class BlendInfoBarOp : public InformationBarOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94   changed 16/9/99 Diccon Yamanaka
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class BlendInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( BlendInfoBarOp )  
	//friend class BlendTool;
public:
	BlendInfoBarOp() { m_DetachBlendPath = FALSE; m_EditBlendSteps = TRUE;  m_BlendedOnCurve = FALSE;
						m_BlendAntiAlias = TRUE, m_OneToOne = FALSE; m_Tangential = FALSE;
						DlgResID = _R(IDD_BLENDTOOLBAR);
					 };

	virtual void UpdateState();		// Overrides the empty function in InformationBarOp
	void UpdateInfoBarState();      // does the actual updating
//	void ApplyOneToOne(BOOL State); // Sets the one-to-one state of all the selected blends
//	void ApplyAntialias(BOOL State);// Sets the antialias blend steps state of all the selected blends

	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here
	
	// DY 9/99 
	NodeBlend* GetBlendOnCurve(); // if the selection is a blend on a path this returns the blend
	static BOOL NonBlendsSelected();
	void EnableBlendSelectedGadgets(BOOL Enable);
	void LoadStringsIntoEffectCombo();
	void ShowEffectComboString(ColourBlendType Type);
private:

	void ChangeBitmapButtonState(CGadgetID GadgetID, BOOL* CurrentState); //DY 13/9/99 function to change the state of a bitmap button, 
	
	void ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID);
	void ChangeProfileOnIdle(CProfileBiasGain* Profile, CGadgetID GadgetID);
	
	// DY, 25/9/99 to set the text in the edit field 
	void SetBlendStepsEditText(INT32 NumSteps);
	void SetBlendDistanceEditText(INT32 NumSteps);

	BOOL GetDistanceEntered( double* Distance);  // returns the value set in the edit field
	BOOL IsStepDistanceValid (UINT32 MinValue, UINT32 MaxValue, double Distance);  // sets the DistanceEntered member of all selected blends

	NodeBlender* GetBlender();  // returns the first nodeblender of the selected blend
	NodeBlend* GetNodeBlend();  // returns the first nodeblend of the selection
	BOOL GetNumSteps( UINT32 MinValue, UINT32 MaxValue, INT32* NumSteps);		
  	BOOL GetNumStepsFromDistance(UINT32 MinValue, UINT32 MaxValue, INT32* NumSteps);			
	
	
	BOOL GetMeanBlendDistance(double* Distance);  // returns the length of the currently selected blend
	BOOL AllBlendsHaveSameNumSteps();    // returns whether or not all selected blends have the same number of steps
	BOOL AllBlendsHaveSameDistance();    

	void SetBlendEditState(EditState State);  // sets the edit state of all selected blends

	BOOL CheckSelectionAndSet(); // restores the selection if we have been editing a blend on a path

	CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType);
	NodeBlend* GetCurrentNodeBlend();
	INT32 AttributeIndexModifier(INT32 CurrentIndex);

	BOOL m_DetachBlendPath;
	BOOL m_EditBlendSteps;   //DY 13/9/99 this is really ugly but I can't get 
	                         // get any sense out of interrogating the 
	                          // gadgets directly so keep flags to remember where we are
	BOOL m_BlendAntiAlias;
	BOOL m_OneToOne;
	BOOL m_Tangential;
	BOOL m_BlendedOnCurve;

	public:

	CBiasGainGadget  m_BiasGainGadgetPosition;  // the profile gadget added 16/9/99
	CBiasGainGadget  m_BiasGainGadgetAttribute;

};

/********************************************************************************************

>	class BlendInfoBarOpCreate : public BarCreate

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Purpose:	Class for creating BlendInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class BlendInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create() { return (new BlendInfoBarOp); }
};

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

/********************************************************************************************

>	class OpBlendNodes : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/94
	Purpose:	Operation for blending two arbitrary objects together

********************************************************************************************/

enum OpBlendOpType {	BLENDOPTYPE_NONE, 			// No type - used on op construction
						BLENDOPTYPE_NEW, 			// A newly created blend object
						BLENDOPTYPE_REMAP, 			// A remapping of points
						BLENDOPTYPE_BLENDANDOBJECT,	// Blending a blend with an object
						BLENDOPTYPE_BLENDANDBLEND,	// Blending a blend with another blend
						BLENDOPTYPE_BLENDBLENDONPATH, //Blending a blend on a path
					};

class OpBlendNodes : public SelOperation
{
CC_DECLARE_DYNCREATE(OpBlendNodes);

public:
	// Construction/Destruction
	OpBlendNodes();                     
	~OpBlendNodes();

	// The all important Do functions
	BOOL DoDrag(BlendTool* pBlendTool);
	virtual BOOL SnappingDrag() { return FALSE; }
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, Spread*, 
								BOOL Success, BOOL bSolidDrag);

	// Some Render functions to will draw the EORed drag box
	void RenderMyDragBlobs();
	void RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	void GetOpName(String_256* OpName);

	// Undo and redo calls
	virtual BOOL Undo();
	virtual BOOL Redo();

	// Public support funcs
	BOOL RecordBlenderInfo(List& BlenderList,NodeBlend* pNodeBlend);
	void DeinitBlenders(List& BlenderList);
	BOOL ReinitBlenders(List& BlenderList);
//	BOOL DeinitAndReinitBlend(NodeBlend* pThisNodeBlend);

	// Diccon 9/99 when blending from one blend to another, this retrieves the child
	// of the blend that is closest to the drag point, for either the start or the end blend
	BOOL GetNodeClosestToPoint(NodeRenderableInk** ppInk, BOOL StartBlend);

private:
	// The hard working Do functions
	BOOL DoBlendObjects();
	BOOL DoCreateBlender(	NodeRenderableInk* pNodeStart,INT32 IndexStart,
							NodeRenderableInk* pNodeEnd,INT32 IndexEnd,
							Node* pContextNode, AttachNodeDirection AttachDir);
	BOOL DoRemapBlend();
	BOOL DoBlendBlendAndObject();
	BOOL DoBlendBlendAndBlend();
	BOOL DoDeinitBlenders(List& BlenderList);
	BOOL DoReinitBlenders(List& BlenderList);

	BOOL DeterminBlendObjectsProcessorHit ();

	// Member vars of the operation 
	BlendToolRef*	pRefStart;
	BlendToolRef*	pRefEnd;

	OpBlendOpType	OpType;

	// Data stored that can be used during Undo/Redo
	NodeBlend* pNodeBlend;
	NodeBlend* pNodeBlendStart;
	NodeBlend* pNodeBlendEnd;

	BlendTool* m_pBlendTool;

	// DMc - removes all shadows & bevels from the blend nodes
	BOOL RemoveCompoundNodes(CompoundNodeTreeFactoryList * pList);

	// helper function for the above (see cpp file)
	BOOL RemoveCompoundNodesFromNode(Node * pNode, Node ** pRetnNode,
		CompoundNodeTreeFactoryList * pList);

	// creates a contour node
	BOOL DoContourNode(Node * pNode, DocCoord PointerPos, UINT32 Steps);
	
};

/********************************************************************************************

>	class OpAddBlendPath : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/99
	Purpose:	Adds a blend path to the selected blends

********************************************************************************************/

class OpAddBlendPath : public SelOperation
{
CC_DECLARE_DYNCREATE(OpAddBlendPath);

public:
	// Construction/Destruction
	OpAddBlendPath()  {}
	~OpAddBlendPath() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void Do(OpDescriptor*);
	virtual void GetOpName(String_256* OpName);

	/* DY 16/9 when blending to a curve we need to know if path to 
	blend to is to be rendered on top or not */
	BOOL IsBlendPathOnTopOfBlend(SelRange* pSel);

};

/********************************************************************************************

>	class OpDetachBlendPath : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/99
	Purpose:	Detached the blend path from each of the selected blends

********************************************************************************************/

class OpDetachBlendPath : public SelOperation
{
CC_DECLARE_DYNCREATE(OpDetachBlendPath);

public:
	// Construction/Destruction
	OpDetachBlendPath()  {}
	~OpDetachBlendPath() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void Do(OpDescriptor*);
	virtual void GetOpName(String_256* OpName);
};

/********************************************************************************************

>	class OpRemoveBlend : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Purpose:	This removes the selected blend objects, leaving the original objects in the tree

********************************************************************************************/

class OpRemoveBlend : public SelOperation
{
CC_DECLARE_DYNCREATE(OpRemoveBlend);

public:
	// Construction/Destruction
	OpRemoveBlend()  {}
	~OpRemoveBlend() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void Do(OpDescriptor*);
	virtual void GetOpName(String_256* OpName);
};

/********************************************************************************************

>	class OpChangeBlendSteps : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Purpose:	This changes the number of steps of the selected blends

********************************************************************************************/

class OpChangeBlendSteps : public SelOperation
{
CC_DECLARE_DYNCREATE(OpChangeBlendSteps);

public:
	// Construction/Destruction
	OpChangeBlendSteps()  {}
	~OpChangeBlendSteps() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void DoWithParam(OpDescriptor*,OpParam* pOpParam);
	virtual void GetOpName(String_256* OpName);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }

	BOOL DeterminBlendObjectsProcessorHit (OpParam* pOpParam, List* nodeList);
};



/********************************************************************************************

>	class ChangeBlendStepsAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/11/94
	Purpose:	An action which undoes the modification of a path's filled bit (or redoes it)
				It can also be used to undo the IsStroked flag as well.
	SeeAlso:	-

********************************************************************************************/

class ChangeBlendStepsAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeBlendStepsAction)

public:
	ChangeBlendStepsAction();
	~ChangeBlendStepsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							Node* pThisNodeBlend,
							UINT32 NumSteps,
							double DistanceEntered,
							ChangeBlendStepsAction** NewAction);
protected:
	Node*	 	pNodeBlend;
	UINT32		OldNumSteps;
	double      OldDistanceEntered;
};


/********************************************************************************************

>	class RemapBlendAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Purpose:	An action which undoes the remapping of a blend
	SeeAlso:	-

********************************************************************************************/

class RemapBlendAction : public Action
{

CC_DECLARE_DYNCREATE(RemapBlendAction)

public:
	RemapBlendAction();
	~RemapBlendAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeBlend* pNodeBlend,
							UINT32 RemapRef,
							DocCoord PosStart,
							DocCoord PosEnd,
							RemapBlendAction** NewAction);
protected:
	NodeBlend* 			pNodeBlend;
	UINT32				RemapRef;
	DocCoord			InvPosStart;
	DocCoord			InvPosEnd;
};


/********************************************************************************************

>	class InitBlendersAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Purpose:	An action which deinits or reinits a blend
	SeeAlso:	-

********************************************************************************************/

class InitBlendersAction : public Action
{

CC_DECLARE_DYNCREATE(InitBlendersAction)

public:
	InitBlendersAction();
	~InitBlendersAction();
	virtual ActionCode Execute();
	static ActionCode Init( OpBlendNodes* pOp,
							ActionList* pActionList,
							List* pBlenderInfoList,
							BOOL DeinitState,
							InitBlendersAction** NewAction);
protected:
	List				BlenderInfoList;
	BOOL				Deinit;
};

//-------------------------------------------------------
//-------------------------------------------------------
//-------------------------------------------------------

enum ChangeBlendType {	CHANGEBLEND_NONE, 
						CHANGEBLEND_ONETOONE, 
						CHANGEBLEND_ANTIALIAS, 
						CHANGEBLEND_COLOURBLENDTYPE,
						CHANGEBLEND_TANGENTIAL, 
						CHANGEBLEND_OBJECTPROFILE,
						CHANGEBLEND_ATTRPROFILE, 
						CHANGEBLEND_BLENDONPATH,
						CHANGEBLEND_EDITEND };

/********************************************************************************************

>	class OpChangeBlend : public SelOperation

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/12/94
	Purpose:	This changes aspects of all selected blends

********************************************************************************************/

class OpChangeBlend : public SelOperation
{
CC_DECLARE_DYNCREATE(OpChangeBlend);

public:
	// Construction/Destruction
	OpChangeBlend()  					{ ChangeType = CHANGEBLEND_NONE; }
//	OpChangeBlend(ChangeBlendType Type) { ChangeType = Type; }
	~OpChangeBlend() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void DoWithParam(OpDescriptor*,OpParam* pOpParam);
	virtual void GetOpName(String_256* OpName);

	ChangeBlendType GetChangeType () { return (ChangeType); }	// required for undo/redo

private:
	ChangeBlendType ChangeType;
};

/********************************************************************************************

>	class OpBlendOneToOne : public OpChangeBlend

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/94
	Purpose:	This changes the one-to-one flag of all selected blends

********************************************************************************************/

class OpBlendOneToOne : public OpChangeBlend
{
CC_DECLARE_DYNCREATE(OpBlendOneToOne);

public:
	// Construction/Destruction
	OpBlendOneToOne()  {}
	~OpBlendOneToOne() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void Do(OpDescriptor*);
};

/********************************************************************************************

>	class OpBlendAntialias : public OpChangeBlend

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/94
	Purpose:	This changes the one-to-one flag of all selected blends

********************************************************************************************/

class OpBlendAntialias : public OpChangeBlend
{
CC_DECLARE_DYNCREATE(OpBlendAntialias);

public:
	// Construction/Destruction
	OpBlendAntialias()  {}
	~OpBlendAntialias() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void Do(OpDescriptor*);
};

/********************************************************************************************

>	class OpBlendTangential : public OpChangeBlend

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/99
	Purpose:	This changes the Tangential flag of all selected blends

********************************************************************************************/

class OpBlendTangential : public OpChangeBlend
{
CC_DECLARE_DYNCREATE(OpBlendTangential);

public:
	// Construction/Destruction
	OpBlendTangential()  {}
	~OpBlendTangential() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void Do(OpDescriptor*);
};


/********************************************************************************************

>	class OpEditBlendEndObject : public OpChangeBlend

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com> yamanaka
	Created:	6/9/99
	Purpose:	The operation to edit the position of the end objects of a blend on a path
********************************************************************************************/

class OpEditBlendEndObject : public SelOperation
{
CC_DECLARE_DYNCREATE(OpEditBlendEndObject);

public:
	OpEditBlendEndObject() {}
	OpEditBlendEndObject(BlendTool* pBlendTool);
	~OpEditBlendEndObject();

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);


	virtual void GetOpName(String_256* OpName);

	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, Spread*, 
								BOOL Success, BOOL bSolidDrag);
	virtual void DragPointerIdle(DocCoord, ClickModifiers, Spread*, BOOL bSolidDrag);
	// Some Render functions to will draw the EORed drag box
//	void RenderMyDragBlobs();
	void RenderDragBlobs(Spread* pSpread, DocCoord CentrePosition, BOOL On);
	void RenderSelectedObjectBlobs(Spread* pSpread);

	BOOL DoDrag(DocCoord PointerPos, Spread* pSpread);
	NodeBlendPath* GetNodeBlendPath();

protected:
	BOOL RecalculateBlend(DocCoord PointerPos); // does the work when the drag has finished

	BOOL GetClosestPointOnPath(DocCoord PointerPos, DocCoord* ClosestPoint);  // retrieves the closest point on the path to the coord passed
	NodeBlender* GetBlenderAndPosition(Node* pNode, BOOL* First);  

	// two versions of this function depending on whether you wish to keep 
	// the number of blend steps constant or the distance between steps.
	BOOL InsertChangeEndActions(double NewPathProportion, DocCoord NewPosition, 
								Node* pNodeToEdit);

	BOOL InsertChangeEndActions(double NewPathProportion, DocCoord NewPosition,
								 double StepDistance, Node* pNodeToEdit);

	BOOL CalculateNewNumStepsAndPosition(UINT32 OldNumSteps, double BlendDistance, 
										double StepDistance, double* FixedDistance, 
										double* MoveableDistance, UINT32* NewNumSteps); 
										
	BOOL InsertChangeProportion(double NewProp, BOOL FirstNode, NodeBlender* pEditBlender);


	Matrix* m_pTransMatrix;         
	DocCoord m_StartCoord;
	DocCoord m_LastCoord;
	Range*  m_pRange;
	Range*  m_pCopyRange;
	NodeBlend* m_pNodeBlend;
	SelectionState* m_pSelState;
	NodeBlendPath* m_pNodeBlendPath;
	BlendTool* m_pBlendTool;
};


/********************************************************************************************

>	class ChangeBlendOpParam : public OpParam

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/95
	Purpose:	The param passed to OpChangeBlend::DoWithParam
	SeeAlso:	-

********************************************************************************************/

class ChangeBlendOpParam : public OpParam
{

CC_DECLARE_MEMDUMP(ChangeBlendOpParam)

public:
	ChangeBlendOpParam()	{ ChangeType = CHANGEBLEND_NONE; }
	~ChangeBlendOpParam()	{}

	void SetOldValues(NodeRenderableInk* pNodeBlend);
	void SetNewValues(NodeRenderableInk* pNodeBlend);
	void SwapOldAndNew();
	void SwapProfiles(CProfileBiasGain* pOldProfile, CProfileBiasGain* pNewProfile);

	ChangeBlendType ChangeType;

	BOOL			NewAntiAlias;
	BOOL			OldAntiAlias;

	BOOL			NewOneToOne;
	BOOL			OldOneToOne;

	ColourBlendType	NewColBlendType;
	ColourBlendType	OldColBlendType;

	BOOL			NewTangential;
	BOOL			OldTangential;

	// Diccon Yamanaka 9/99 added code
	BOOL			OldBlendedOnCurve;
	BOOL			NewBlendedOnCurve;

	CProfileBiasGain OldObjectProfile;
	CProfileBiasGain NewObjectProfile;

	CProfileBiasGain OldAttrProfile;
	CProfileBiasGain NewAttrProfile;

	EndObject		OldEndObject;
	EndObject		NewEndObject;
};


/********************************************************************************************

>	class ChangeBlendAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Purpose:	An action which changes the one-to-one flag of a blend
	SeeAlso:	-

********************************************************************************************/

class ChangeBlendAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeBlendAction)

public:
	ChangeBlendAction();
	~ChangeBlendAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRenderableInk* pThisNodeBlend,
							ChangeBlendOpParam* pChangeParam,
							ChangeBlendAction** NewAction);

	static void ChangeObjectProfileWithNoUndo (CProfileBiasGain &Profile, BOOL regenerateParents = FALSE);
	static void ChangeAttributeProfileWithNoUndo (CProfileBiasGain &Profile);
protected:	
	NodeRenderableInk* 	pNodeBlend;
	ChangeBlendOpParam 	ChangeParam;
};

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

enum ChangeBlenderType { CHANGEBLENDER_NONE, 
						 CHANGEBLENDER_ANGLESTART, 
						 CHANGEBLENDER_ANGLEEND,
						 CHANGEBLENDER_PATHSTART,
						 CHANGEBLENDER_PATHEND, 
						 CHANGEBLENDER_REGEN,
						 CHANGEBLENDER_NBPINDEX,
						 CHANGEBLENDER_BLENDONCURVE,
						 CHANGEBLENDER_SWAPENDS
												};

/********************************************************************************************

>	class ChangeBlenderOpParam : public OpParam

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/99
	Purpose:	Holds the info needed to change the blender in a certain way
	SeeAlso:	-

********************************************************************************************/

class ChangeBlenderOpParam : public OpParam
{

CC_DECLARE_MEMDUMP(ChangeBlenderOpParam)

public:
	ChangeBlenderOpParam()							{ m_ChangeType = CHANGEBLENDER_NONE; }
	ChangeBlenderOpParam(ChangeBlenderType Type)	{ m_ChangeType = Type; }
	~ChangeBlenderOpParam()	{}

	void SetOldValues(NodeBlender* pNodeBlender);
	void SetNewValues(NodeBlender* pNodeBlender);
	void SwapOldAndNew();

	ChangeBlenderType m_ChangeType;

	double	m_NewAngleStart;
	double	m_OldAngleStart;
	double	m_NewAngleEnd;
	double	m_OldAngleEnd;
	double  m_OldPathStart;
	double  m_NewPathStart;
	double  m_OldPathEnd;
	double  m_NewPathEnd;
	INT32	m_OldNodeBlendPathIndex;
	INT32	m_NewNodeBlendPathIndex;
	BOOL	m_OldBlendedOnCurve;
	BOOL	m_NewBlendedOnCurve;
	INT32	m_OldObjIndexStart;
	INT32    m_NewObjIndexStart;
	INT32	m_OldObjIndexEnd;
	INT32	m_NewObjIndexEnd;
};


/********************************************************************************************

>	class ChangeBlenderAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/99
	Purpose:	An action which changes the blender
	SeeAlso:	-

********************************************************************************************/

class ChangeBlenderAction : public Action
{

CC_DECLARE_DYNCREATE(ChangeBlenderAction)

public:
	ChangeBlenderAction();
	~ChangeBlenderAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeBlender* pNodeBlender,
							ChangeBlenderOpParam& ChangeParam);
protected:	
	NodeBlender* 	  		m_pNodeBlender;
	ChangeBlenderOpParam 	m_ChangeParam;
};


/********************************************************************************************

>	class InvalidateBoundsAction : public Action

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/99
	Purpose:	An action which invalidates the bounds of a node
	SeeAlso:	-

********************************************************************************************/

class InvalidateBoundsAction : public Action
{

CC_DECLARE_DYNCREATE(InvalidateBoundsAction)

public:
	InvalidateBoundsAction();
	~InvalidateBoundsAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeRenderableBounded* pNode,
							BOOL IncludeChildren);
protected:	
	NodeRenderableBounded* 	m_pNode;
	BOOL					m_IncludeChildren;
};



/********************************************************************************************

>	class OpChangeBlendDistance : public SelOperation

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/99
	Purpose:	This changes the distance between steps of the selected blends
				

********************************************************************************************/

class OpChangeBlendDistance : public SelOperation
{

CC_DECLARE_DYNCREATE(OpChangeBlendDistance)

public:
	OpChangeBlendDistance() {}
	~OpChangeBlendDistance() {}

	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	// The all-important Do() func
	virtual void DoWithParam(OpDescriptor*,OpParam* pOpParam);
	virtual void GetOpName(String_256* OpName);

	

protected:
	BOOL InsertChangeStepsAction(NodeBlend* pNodeBlend,  
								 double StepDistance, 
								 UINT32* NewNumSteps, double* NewDistances);

	BOOL InsertChangePathProportion (NodeBlend* pNodeBlend,  
									 double StartDistance,
									 double EndDistance);

	BOOL InsertChangeLinearBlendActions(NodeBlend* pNodeBlend, 
										Operation* pOp,
										double NewStepDistance);

	BOOL CalculateNewNumStepsAndPosition(UINT32 OldNumSteps, double BlendDistance, 
										double StepDistance, double* FixedDistance, 
										double* MoveableDistance, UINT32* NewNumSteps); 
										

	BOOL InsertTransformNodesAction(NodeBlend* pNodeBlend, double StartDistance, double EndDistance);
	
};




#endif 		// INC_BLENDTOOL
