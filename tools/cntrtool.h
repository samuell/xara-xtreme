// $Id: cntrtool.h 671 2006-03-15 14:30:01Z alex $
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

#ifndef INC_CONTOURTOOL
#define INC_CONTOURTOOL

#include "tool.h"
#include "doccoord.h"
#include "bars.h"
#include "blendatt.h"
#include "selop.h"
#include "biasgdgt.h"
#include "iprofile.h"
#include "blndtool.h"
#include "becomea.h"
#include "bevtrap.h"
#include "dragtool.h"

class ContourTool;
class ContourInfoBarOp;
class AttrJoinType;

#define OPTOKEN_CONTOURNODES _T("ContourNode")

//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------
//-----------------------------------------------------------------

/********************************************************************************************

>	class OpContourNodes : public SelOperation

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/8/99
	Purpose:	Copy of the blend nodes operation in blndtool.h

********************************************************************************************/

class OpContourNodes : public SelOperation
{
CC_DECLARE_DYNCREATE(OpContourNodes);

public:
	// Construction/Destruction
	OpContourNodes();                     
	~OpContourNodes();

	// The all important Do functions
	BOOL DoDrag(ContourTool * pTool, ContourInfoBarOp * pOp, DocCoord & PointerPos, BOOL bDragOnBlob,
		DocRect * pBlobRect = NULL);
	virtual BOOL SnappingDrag() { return FALSE; }
	
	// Virtual functions needed for the dragging operations to work
	virtual void DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag);
	virtual void DragFinished(	DocCoord PointerPos, 
								ClickModifiers ClickMods, Spread*, 
								BOOL Success, BOOL bSolidDrag);

	// sets up the dragging information necessary to render the drag blobs
	BOOL SetupDragInfo();

	// Some Render functions to will draw the EORed drag box
	void RenderDragBlobs(INT32 Width,Spread* pSpread,BOOL bKeepDirections=FALSE);

	// Some Render functions to will draw the EORed drag box
	void RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag);
		
	// These functions required for the OpDescriptor class
	static BOOL Declare();
	static OpState GetState(String_256* Description, OpDescriptor*);

	void GetOpName(String_256* OpName);

	
private:
	
	ContourTool * m_pTool;
	ContourInfoBarOp * m_pBarOp;

	BOOL m_bHasDragged;
	DocCoord m_LastPointerPos;
	DocCoord m_ThisPointerPos;
	
	// the rect of the blob which has been clicked on
	DocRect m_BlobRect;

	// the selection's inside bounding rect to be contoured
	DocRect m_SelRect;

	// the summed path of all nodes
	Path * m_pPathList;
	UINT32 m_NumPaths;

	// whether each path is inner or outer
	BOOL * m_pPathOuterList;
	AttrJoinType ** m_pPathJoinTypeList;
	JointType m_JoinType;

	// the start indexes of paths which are to be summed together
	UINT32 * m_pSetList;
	UINT32 m_NumSets;

	BOOL m_bDragStartedOnBlob;

	// the flatness to use - calculated in the setup depending on the active view
	INT32 m_Flatness;

	// Karim 10/05/2000
	// Tests whether a specific node should have its outline taken into account when
	// the user drags a contour.
	BOOL NodeCanBeDragContoured(Node* pSubNode, Node* pNode, BOOL bContourNodesExist);

	// alters pPoint to account for the fact that blobs always have the same value
	void AlterPointerPosToAccountForBlob(DocCoord * pPoint);

	// in dragging, we have the starting width and also the calculated width
	// from the pointers starting position. Thus, when dragging we calculate the
	// offset of the pointer from the original starting drag width, and apply
	// this offset to the original width to give the width to render the drag blobs
	// to.
	
	// the starting width to use
	MILLIPOINT m_OriginalWidth;

	// the starting drag width to use when calculating the offset of the pointer
	MILLIPOINT m_StartDragWidth;

};

/********************************************************************************************

>	class ContourTool : public Tool_v1

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	4/10/94
	Purpose:	The Contour tool class
	Notes:		Copied from the blend tool

********************************************************************************************/

class CCAPI ContourTool : public DragTool
{
// Give tool name in memory dumps
CC_DECLARE_MEMDUMP(ContourTool);

public:
	ContourTool();
	~ContourTool();

	// Standard calls your tool must provide
	BOOL Init();
	void Describe(void* InfoPtr);
	UINT32 GetID() { return TOOLID_CONTOURTOOL; };
	
	// Some very common event handlers
	void SelectChange(BOOL isSelected);
	void OnClick( DocCoord, ClickType, ClickModifiers, Spread* );
	void OnMouseMove(DocCoord PointerPos,Spread* pSpread,ClickModifiers ClickMods);
	void RenderToolBlobs(Spread* pSpread,DocRect* pDocRect);
	BOOL GetStatusLineText(String_256* ptext, Spread*, DocCoord, ClickModifiers);
	BOOL OnKeyPress(KeyPress* pKeyPress);

	// Some useful static functions
	static ContourInfoBarOp* 	GetContourInfoBarOp()	{ return pContourInfoBarOp; }
	static BOOL 			IsCurrent();
	static void 			DisplayStatusBarHelp(UINT32 StatusID);

	static BlendToolRef*	GetPtrRefStart() 	{ return pRefStart; }
	static BlendToolRef*	GetPtrRefEnd() 	  	{ return pRefEnd; }

	static void UpdateRef(BlendToolRef* pRef,Spread* pSpread, DocCoord PointerPos,BOOL CheckNodeUnderPoint = TRUE);
	static void UpdateCursorAndStatus();
	static void CheckNodeRemapping(BlendToolRef* pRefStart, BlendToolRef* pRefEnd);

	static INT32 CalculateContourWidth(DocRect &br, DocCoord &PointerPos);
	static INT32 CalculateContourWidth(Node * pNode, DocCoord &PointerPos);

	BOOL AreToolBlobsRenderedOnSelection() { return TRUE; }

	void SetupToolBlobs();

	// TRUE for enable, FALSE for disable
	void SetBlobRendering(BOOL b) 
	{ 
		if (b) 
			m_bDisableBlobRenderingFlag = FALSE;
		else
			m_bDisableBlobRenderingFlag = TRUE;
	}

	// static drag operation
	// public so the info bar can use it to do drag rendering
	static OpContourNodes *m_pOpContourNodes;

private:
	// Private blend tool functions
	static NodeRenderableInk* 	FindObject(Spread* pSpread, DocCoord ClickPos);
	static BOOL 				IsPointOverPathBlob(DocCoord* pPointerPos,BlendToolRef* pRef);
	static BOOL 				IsPointOverBlendBlob(DocCoord* pPointerPos,BlendToolRef* pRef);

	BOOL CreateCursors();						// Create your tool's cursors in this func
	void DestroyCursors();						// Destroys all blend tool cursors

	static ContourInfoBarOp*	pContourInfoBarOp;	// Ptr to your tool's infobar

	// Cursors
	static Cursor*	pcNormalCursor;						// Your standard cursor to use when your tool becomes active
	static Cursor*	pcOverBlob;							// cursor when I'm over a blob
	
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

	// the blobs' coordinates for dragging
	DocCoord m_BlobPoints[8];
	
	// returns TRUE for whether a point is over a blob
	BOOL IsPointOverBlob(DocCoord &Point, DocRect *pBlobRect = NULL);

	BOOL m_bDisableBlobRenderingFlag;
	void InformWarningBevelExistsInSelection();
};


//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------
//-----------------------------------------------


/********************************************************************************************

>	class ContourInfoBarOp : public InformationBarOp

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94   changed 16/9/99 Diccon Yamanaka
	Purpose:	Class for handling the tool's information bar.

********************************************************************************************/

class ContourInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( ContourInfoBarOp )  
public:
	ContourInfoBarOp() 
	{	
		DlgResID = _R(IDD_CONTOURINFOBAR);
		m_DetachBlendPath = FALSE; 
		m_EditBlendSteps = TRUE; 
		m_BlendAntiAlias = TRUE; 
		m_OneToOne = FALSE; 
		m_Tangential = FALSE; 
		m_BlendedOnCurve = FALSE;
		m_bUpdateState = TRUE; 
		m_pTool = NULL;
		m_bDragging = FALSE;
		m_LastDragWidth = 0;
		m_bInsetPath = FALSE;
		m_bOuterIsSelected = TRUE;
	}

	virtual void UpdateState();		// Overrides the empty function in InformationBarOp

	MsgResult 	Message(Msg* Msg);	// All messages to the info bar come through here

	// sets the slider & the edit control values
	void SetSliderValue(UINT32 Width, BOOL bMany);

	// enables or diables the update state message
	void SetUpdateState(BOOL bUpdateState) { m_bUpdateState = bUpdateState; }

	ContourTool * m_pTool;

	BOOL m_BetweenViews; // added sjk 22/11/00 to stop extra msgs between view changes

private:

	void ChangeBitmapButtonState(CGadgetID GadgetID, BOOL* CurrentState); //DY 13/9/99 function to change the state of a bitmap button, 
	void InitialiseObjectGainGadget(CGadgetID GadgetID, UINT32 BubbleID, UINT32 StatusID)
		{
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
			 m_BiasGainObjectGadget.Init(this, GadgetID, BubbleID, StatusID); }  ;   // DY 16/9/99					
#endif
		}
	void InitialiseAttrGainGadget(CGadgetID GadgetID, UINT32 BubbleID, UINT32 StatusID)
		{
PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
			m_BiasGainAttrGadget.Init(this, GadgetID, BubbleID, StatusID);
#endif
		}  ;   // DY 16/9/99					
	
	void ChangeProfile(CProfileBiasGain* Profile, CGadgetID GadgetID);
	void ChangeProfileOnIdle(CProfileBiasGain* Profile, CGadgetID GadgetID);

	BOOL m_DetachBlendPath;
	BOOL m_EditBlendSteps;   //DY 13/9/99 this is really ugly but I can't get 
	                         // get any sense out of interrogating the 
	                          // gadgets directly
	BOOL m_BlendAntiAlias;
	BOOL m_OneToOne;
	BOOL m_Tangential;
	BOOL m_BlendedOnCurve;

	Spread* m_BlobsSpread;
public:

PORTNOTE("other", "Removed CBiasGainGadget")
#ifndef EXCLUDE_FROM_XARALX	
	CBiasGainGadget  m_BiasGainObjectGadget;  // the profile gadget added 16/9/99
	CBiasGainGadget  m_BiasGainAttrGadget;  // the profile gadget added 16/9/99
#endif

	BOOL GetInsetPathFlag() { return m_bInsetPath; }

private:
	
	// whether the inner, outer or neither of the two buttons are selected
	BOOL m_bOuterIsSelected;
	BOOL m_bInnerIsSelected;

	void DealWithContourOuterClick();
	void DealWithContourInnerClick();
	void DealWithWidthChange(UINT32 Width);
	void DealWithInsetPathClick();

	BOOL ConvertValueToString(String_256 &In, const INT32 value, UnitType type=NOTYPE);
	BOOL ConvertStringToValue(CGadgetID ID,INT32 &value);

	/////////////////////////////////////////////////////////////////////////////////////////
	// Karim 11/09/2000
	// New methods to support the new join-type infobar buttons.

	void HandleJoinTypeMitreClicked();
	void HandleJoinTypeRoundClicked();
	void HandleJoinTypeBevelClicked();
	void ChangeJoinType(JointType jt);
	void UpdateJoinTypeControls();
	void EnableDisableJoinTypeControls();

	/////////////////////////////////////////////////////////////////////////////////////////

	CProfileBiasGain* GetProfileFromSelection (CGadgetID ProfileGadgetID, BOOL* bMany, BOOL* bAllSameType);

	// enables the update state message
	BOOL m_bUpdateState;

	// whether we are dragging or not
	BOOL m_bDragging;

	// the last width which the blobs were rendered with so we can hence remove them
	MILLIPOINT m_LastDragWidth;

	// functions to get & retrieve the value in the logaritmic slider (for widths)
	MILLIPOINT GetSliderLogValue();
	BOOL SetSliderLogValue(MILLIPOINT Width);

	void UpdateGadgetHelp();

	BOOL m_bInsetPath;

	BOOL CheckForBevels();
};

/***********************************************************************************************

>	class ContourDragBecomeA : public BecomeA

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	2/12/99
	Purpose:	This is used to turn all nodes which are to be contoured into a single path for
				dragging

***********************************************************************************************/
class ContourDragBecomeA : public BecomeA
{
public:
	// constructor
	ContourDragBecomeA(BecomeAReason Reason,
					CCRuntimeClass* pClass, 
					UndoableOperation* pOp, 
					BOOL Sel,
					Path * pPathList,
					UINT32 NumPaths,
					BOOL IncludeLineWidths = FALSE) : BecomeA(Reason, pClass, pOp, Sel)
	{
		m_pPathList = pPathList;
		m_NumPaths = NumPaths;
		m_Count = 0;
		m_IncludeLineWidths = IncludeLineWidths;

		// we're only interested in the silhouette.
		fSilhouette = TRUE;
	}

	// passback function
	virtual BOOL PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,
		CCAttrMap* pAttrMap);

//	UINT32 GetCount() { return m_Count; }

private:

	// path list
	Path * m_pPathList;

	// number of paths
	UINT32 m_NumPaths;
	
	// count variable to keep track of which path we're currently addressing
//	UINT32 m_Count;

	BOOL m_IncludeLineWidths;
} ;



	

/********************************************************************************************

>	class ContourInfoBarOpCreate : public BarCreate

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/94
	Purpose:	Class for creating ContourInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class ContourInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp* Create() { return (new ContourInfoBarOp); }
};



#endif // INC_CONTOURTOOL
