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

#ifndef INC_FEATHEROPS
#define INC_FEATHEROPS

//#define OPTOKEN_FEATHER			"Feather"
//#define OPTOKEN_UNFEATHER		"UnFeather"
#define OPTOKEN_FEATHERSIZE		_T("FeatherSize")
#define OPTOKEN_FEATHERPROFILE	_T("FeatherProfile")

//#include "selop.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "biasgdgt.h"		// for CBiasGainGadget
//#include "node.h"			// NodeHidden - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opliveeffects.h"
#include "effects_stack.h"

class AttrFeather;
class OpChangeFeatherSize;
class NodeFeatherEffect;
class FeatherAttrValue;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Changing feather size
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************
>	class ChangeFeatherSizeSliderOpDesc : public UndoableOpDescriptor
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/99
	Purpose:	This class handles the UI for _R(IDC_FEATHERSIZE_CUSTOMEDIT). It invokes OpChangeFeatherSize
				on the selected FeatherAttrValue(s)
	SeeAlso:	OpChangeFeatherSize, OpChangeLineWidthOpDesc
 ********************************************************************************************/

class ChangeFeatherSizeSliderOpDesc : public UndoableOpDescriptor
{
	CC_DECLARE_DYNAMIC( ChangeFeatherSizeSliderOpDesc )

public:
	ChangeFeatherSizeSliderOpDesc(
						 	UINT32 toolID,                    // Tool (Module) Identifier
						 	UINT32 txID,                      // String Resource ID
						 	CCRuntimeClass* Op,				// pointer to the Op's runtime class object
						 	TCHAR* tok,						// pointer to the token string
						 	pfnGetState gs,					// pointer to the GetState function
						 	UINT32 helpId = 0,				// help identifier 
						 	UINT32 bubbleID = 0,				// string resource for bubble help
						 	UINT32 resourceID = 0,			// resource ID
						 	UINT32 controlID = 0,				// control ID within resource
						 	BOOL ReceiveMessages = TRUE, 		
						 	BOOL Smart = FALSE, 
						 	BOOL Clean = FALSE,
				 			UINT32 AutoStateFlags = 0
						   );

	// GetState function for the OpDescriptor. Implemented in the Operation
//	static OpState GetState(String_256* Description, OpDescriptor*);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//									Message handling functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Function to initialise the feather controls on creation. 
	virtual void OnControlCreate(OpDescControlCreateMsg* CreateMsg);	
	// Function to handle final slider change
	virtual void OnSliderSet(OpDescControlMsg* SelChangedMsg);
	// Function to handle interactive slider changes
	virtual void OnSliderChanging(OpDescControlMsg* SliderChangingMsg);
	// Function to handle a selection from the edit text
	virtual void OnSelectionChange(OpDescControlMsg* SelChangedMsg, List* GadgetList);
	// Function to handle slider cancel
	virtual void OnSliderCancelled(OpDescControlMsg* SliderChangingMsg);

protected:
	// Function to handle changes of various messages
	virtual MsgResult Message(Msg* Msg);
	virtual BOOL OnSelChangingMsg(SelChangingMsg::SelectionState State);
	virtual BOOL OnCommonAttrsChangedMsg();
	virtual BOOL OnOptionsChangingMsg(OptionsChangingMsg* Msg);

private:
	BOOL SetFeatherSizeForCurrentSel();
	BOOL UpdateAllFeatherControls(String_256* Str, MILLIPOINT InverseSliderVal);
	void ConvertSizeToUnits(String_256& StrSize, MILLIPOINT size);

	INT32 GetSliderValue(DialogOp* pDlg, CGadgetID SliderID);
	void SetSliderValue(INT32 SliderVal, DialogOp* pDlg, CGadgetID SliderID);

	static const MILLIPOINT MinSlider;
	static const MILLIPOINT MaxSlider;

	// denotes the power to use for the exponential slider values.
	static const double		SliderChangeRate;

private:
	OpChangeFeatherSize* m_pOpChangeFeatherSize;
	INT32 LastOnscreenPixelSize;
};


/********************************************************************************************
>	class OpChangeFeatherSize : public UndoableOperation
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/12/99
	Purpose:	This class represents the operation of changing the size of the feather
				region (ie the penumbra width of of the shadow transparency). It is
				tied to the _R(IDC_FEATHERSIZE_CUSTOMEDIT) slider / edit control
				which appear in the Feather bar (and also on the Control Bar Palette).
	SeeAlso:	OpShadowPenumbra, ShadowPenumbra
********************************************************************************************/

class CCAPI OpChangeFeatherSize : public UndoableOperation
{
	CC_DECLARE_DYNCREATE( OpChangeFeatherSize )

public:
	OpChangeFeatherSize() : UndoableOperation() {};
	static OpState		GetState(String_256* Description, OpDescriptor*);
	static BOOL			Init();
	static BOOL			DeInit();
	static void			SetEditContext(INT32 iStackPos = STACKPOS_INVALID, ListRange* pEditRange = NULL);
	static ListRange*	GetEditContext();
	static void			ReleaseFeatherCache(NodeRenderableInk* pNode, DocRect& drRegion);

	virtual void		DoWithParam(OpDescriptor* pOp, OpParam* pParam);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL		MayChangeNodeBounds() const { return FALSE; }

	// Karim 23/08/2000 - New Feather slider UI.

	virtual void		DoSlide(INT32 SlideValue);
	virtual void		SlideFinished(BOOL SuccessfulFinish = TRUE);

private:
#ifdef FEATHER_EFFECT
	static NodeFeatherEffect* DoReplaceFeatherEffectNode(UndoableOperation* pOp,
											  NodeFeatherEffect* pNode,
											  MILLIPOINT FeatherSize,
											  CProfileBiasGain Profile);
#endif

private:
	List				m_NewFeatherAttrs;

	static INT32			s_iEditStackPos;
	static ListRange*	s_pEditRange;
};




/********************************************************************************************
>	class OpChangeFeatherSizeParam : public OpParam
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/05
	Purpose:	Control OpChangeFeatherSize
********************************************************************************************/

class OpChangeFeatherSizeParam : public OpParam
{
	CC_DECLARE_DYNAMIC(OpChangeFeatherSizeParam)

public:
	OpChangeFeatherSizeParam(	MILLIPOINT p,
								BOOL bAddNew,
								ListRange* pRange )
	{
		size = p;
		bAddNewFeathers = bAddNew;
		pLevelRange = pRange;
	};
	OpChangeFeatherSizeParam(	MILLIPOINT p,
								ListRange* pRange )
	{
		size = p;
		pLevelRange = pRange;
		bAddNewFeathers = (pRange==NULL);
	};
	~OpChangeFeatherSizeParam() {};

public:
	MILLIPOINT	size;
	BOOL		bAddNewFeathers;
	ListRange*	pLevelRange;
};




/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										Changing feather profile
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



/********************************************************************************************
>	class ChangeFeatherProfileOpDesc : public UndoableOpDescriptor
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/99
	Purpose:	This class handles the UI for _R(IDCB_FEATHERSIZE). It invokes OpChangeFeatherSize
				on the selected FeatherAttrValue(s)
	SeeAlso:	OpChangeFeatherSize, OpChangeLineWidthOpDesc
 ********************************************************************************************/

class ChangeFeatherProfileOpDesc : public UndoableOpDescriptor
{
	CC_DECLARE_DYNAMIC( ChangeFeatherProfileOpDesc )

public:
	ChangeFeatherProfileOpDesc(
						 	UINT32 toolID,                    // Tool (Module) Identifier
						 	UINT32 txID,                      // String Resource ID
						 	CCRuntimeClass* Op,				// pointer to the Op's runtime class object
						 	TCHAR* tok,						// pointer to the token string
						 	pfnGetState gs,					// pointer to the GetState function
						 	UINT32 helpId = 0,				// help identifier 
						 	UINT32 bubbleID = 0,				// string resource for bubble help
						 	UINT32 resourceID = 0,			// resource ID
						 	UINT32 controlID = 0,				// control ID within resource
						 	BOOL ReceiveMessages = TRUE, 		
						 	BOOL Smart = FALSE, 
						 	BOOL Clean = FALSE,
				 			UINT32 AutoStateFlags = 0
						   );

	// GetState function for the OpDescriptor. Implemented in the Operation
//	static OpState GetState(String_256* Description, OpDescriptor*);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//									Message handling functions
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// Function to initialise the profile gadget 
	virtual void OnControlCreate(OpDescControlCreateMsg* CreateMsg);	
	// Function to handle a selection from the profile gadgets dropdown/edit boxes
	virtual MsgResult Message(Msg* Msg);

	
	// Function to handle profile changed message (e.g. when sliders in BiasDialog are set or a profile in
	// combobox selected)
	virtual void OnProfileChanged(OpDescControlMsg* SelChangedMsg);
	// Function to handle profile changing (e.g. when slider is moving).
	virtual void OnProfileChanging(OpDescControlMsg* SliderChangingMsg);

	static CBiasGainGadget  m_BiasGainGadget;

	static void SetBoolGadgetSelected (CGadgetID Gadget, BOOL IsSelected);

public:
	BOOL m_bProfileChanging;	// Indicates whether the profile is changing (e.g. when user drags a slider in
								// the BiasGain dialog).
	BOOL m_bProfileInitialChange;	// set to true when the op should record itself in the ophist list
									// so that its actions are undoable (by OpUndo)
};

/********************************************************************************************
>	class OpChangeFeatherProfile : public UndoableOperation
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/12/99
	Purpose:	This class represents the operation of changing the profile of the feather
				region. It is tied to the _R(IDC_FEATHERPROFILE_GDGT) bias gain profile control
	SeeAlso:	OpChangeShadowProfile
 ********************************************************************************************/
class CCAPI OpChangeFeatherProfile: public UndoableOperation
{         
	CC_DECLARE_DYNCREATE( OpChangeFeatherProfile )    

public:
	OpChangeFeatherProfile() : UndoableOperation() {}
	static OpState	GetState(String_256*, OpDescriptor*);
	static BOOL		Init();
	virtual void	Do (OpDescriptor* pOp);
	virtual void	DoWithParam(OpDescriptor* pOp, OpParam* pParam);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return FALSE; }

protected:
	// Function to handle changes of various messages
	virtual BOOL OnSelChangingMsg(SelChangingMsg::SelectionState State);

private:
	BOOL DoChangeFeatherProfile(FeatherAttrValue* pAttr, CProfileBiasGain &newProfile);
//	CProfileBiasGain* GetProfileFromSelection(CGadgetID GadgetID, BOOL* bMany, BOOL* bAllSameType);
	static SelRange::CommonAttribResult GetProfileFromSelection2(BOOL bFastScan,
																  CProfileBiasGain* pCommonProfile);

};

/********************************************************************************************
>	class FeatherProfileOpParam : public OpParam
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/00
	Purpose:	so we can can virtual DoWithParam functions with a CProfileBiasGain
 ********************************************************************************************/
class FeatherProfileOpParam : public OpParam
{
	CC_DECLARE_DYNCREATE(FeatherProfileOpParam)

public:
	FeatherProfileOpParam(const CProfileBiasGain p, ListRange* pRange) { P = p; pLevelRange = pRange; }
	~FeatherProfileOpParam() {;}

public:
	CProfileBiasGain P;
	ListRange* pLevelRange;
};

/********************************************************************************************
>	class ChangeFeatherProfileAction : public Action
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/12/99
	Purpose:	An action which modifies the profile of the feather region
	SeeAlso:	ChangePenumbraProfileAction
 ********************************************************************************************/

class ChangeFeatherProfileAction : public Action
{
CC_DECLARE_DYNCREATE(ChangeFeatherProfileAction)

public:
	ChangeFeatherProfileAction();
//	~ChangeFeatherProfileAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							FeatherAttrValue* pThisFeatherCompound,
							CProfileBiasGain Profile,
							ChangeFeatherProfileAction** NewAction);

protected:
	FeatherAttrValue* m_pFeather;
	CProfileBiasGain m_LastProfile;	
};


/********************************************************************************************
>	class ChangeFeatherEffectProfileAction : public Action
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/2005
	Purpose:	An action which modifies the profile of the feather effect
 ********************************************************************************************/

#ifdef FEATHER_EFFECT
class ChangeFeatherEffectProfileAction : public Action
{
	CC_DECLARE_DYNCREATE(ChangeFeatherEffectProfileAction)

public:
	ChangeFeatherEffectProfileAction() { m_pFeatherEffect = NULL;}
//	~ChangeFeatherProfileAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeFeatherEffect* pThisFeather,
							CProfileBiasGain Profile,
							ChangeFeatherEffectProfileAction** NewAction);

protected:
	NodeFeatherEffect*	m_pFeatherEffect;
	CProfileBiasGain	m_LastProfile;	
};


/********************************************************************************************
>	class ChangeFeatherEffectAction : public Action
	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/2005
	Purpose:	An action which modifies the profile of the feather effect
 ********************************************************************************************/

class ChangeFeatherEffectAction : public Action
{
	CC_DECLARE_DYNCREATE(ChangeFeatherEffectAction)

public:
	ChangeFeatherEffectAction() { m_pFeatherEffect = NULL; m_LastFeatherSize = 0; }
//	~ChangeFeatherEffectAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							NodeFeatherEffect* pThisFeather,
							MILLIPOINT FeatherSize,
							CProfileBiasGain Profile,
							ChangeFeatherEffectAction** NewAction);

protected:
	NodeFeatherEffect*	m_pFeatherEffect;
	MILLIPOINT			m_LastFeatherSize;
	CProfileBiasGain	m_LastProfile;	
};
#endif


/********************************************************************************************
>	class RegenerateFeatherContourAction : public Action
	Author:		Ilan_Copelyn (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/04/00
	Purpose:	An action which speeds facillitates the caching of the outer contour which
				feathers use to create the feather transparency mask
	SeeAlso:	
 ********************************************************************************************/
class RegenerateFeatherContourAction : public Action
{
CC_DECLARE_DYNCREATE(RegenerateFeatherContourAction)

public:
	RegenerateFeatherContourAction();
	~RegenerateFeatherContourAction();
	virtual ActionCode Execute();
	static ActionCode Init( Operation* pOp,
							ActionList* pActionList,
							FeatherAttrValue* pThisFeatherCompound,
							Path* OldContour,
							RegenerateFeatherContourAction** NewAction);

protected:
	FeatherAttrValue* m_pFeather;
	Path* m_pContour;
};

#endif
