// $Id: opbarcreation.h 662 2006-03-14 21:31:49Z alex $
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
#ifndef OPTOKEN_BARCREATIONOP

#include "cutop.h"
#include "opdupbar.h"

#define OPTOKEN_BARCREATIONOP _T("BarCreationOp")
// modes of state replacement that the bar creation dlg can be in
#define NEW_BAR				0
#define ONLY_BAR			1
#define REPLACE_SOME_STATES	2

class NodeBarProperty;


/********************************************************************************************

>	class OpBarCreation : public CarbonCopyOp

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/8/99
	Purpose:	This class makes several copies of the selection, putting them onto
				particular named layers. Each copy is named button1, button2, etc..
				Each layer is created and named Default, Mouse, Clicked or Selected.
				The copy can be 'mutated' to appear slightly different on each layer.
				The selection is deleted

********************************************************************************************/

class CCAPI OpBarCreation: public CarbonCopyOp
//class CCAPI OpBarCreation: public OpReAlignBar
{         
	CC_DECLARE_DYNCREATE( OpBarCreation )    

public:
	// standard stuff
	OpBarCreation();								
	static BOOL	Init();				
	static OpState	GetState(String_256*, OpDescriptor*);		
	virtual	void	Do(OpDescriptor* token);
    void			DoWithParam(OpDescriptor* token, OpParam* pOpParam);

	// functions that are useful here
	Layer * AddLayer(String_256 LayerName, INT32 NewLayerNo);
	BOOL ApplyNameAttr(String_256 * pNameAttr, Node * pNode, BOOL UseBarName = TRUE);
	void ShiftColourValue(DocColour* pdoccol, INT32 PercentColourChange);
	void FindAttribColours(Node * pAttrNode, INT32 PercentColourChange, INT32 State);
	BOOL CleanSelection (Range * pSel, List * pSelectionList, List * pDeletionList, INT32 LayerNumToCreate);

	// main function that does the business
	BOOL CreateOrEditBar (OpParam* pOpParam);
	BOOL CreateFromDefaultState (OpParam* pOpParam);
	BOOL CreateBackBarFromSelection (OpParam* pOpParam, BOOL Extend = FALSE);
	BOOL CreateBarAndBackBar (OpParam* pOpParam);

	void ShowState(INT32 ShowLayer);
	INT32 GetBarNumber() {return SliceHelper::GetBarNumberFromBarName(m_BarName);}



private:
	String_256	m_RolloverName[5];
	String_256	m_BarName;

	DocRect m_BarSelectionRect;
	DocRect m_FinishedBarRect;

};    


class OpParamBarCreation : public OpParam
{
//CC_DECLARE_DYNAMIC(OpParamBarCreation)

public:
	OpParamBarCreation()
			{	
				m_WantMouse = TRUE;
				m_WantClicked = FALSE;
				m_WantSelected = TRUE;
				m_IsVertical = TRUE;
				m_SuggestDesign = FALSE;
				m_NoOfButtons = 4;
				m_WantDefault = TRUE;
				m_StateReuse = REPLACE_SOME_STATES;
				m_BarName = _T("Bar1");
				m_ContinuousBar = FALSE;
				m_ButtonsAllSameSize = FALSE;
				m_Spacing = 0;
				m_FromSelection = TRUE;
				m_WantBackBar = FALSE;
				m_ExtendFlags = 3;
				m_DelExistingState = FALSE;
			}

	OpParamBarCreation(BOOL WantMouse, BOOL WantClicked, BOOL WantSelected, BOOL WantBackBar, BOOL IsVertical, BOOL SuggestDesign, INT32 NoOfButtons, BOOL WantDefault = TRUE, INT32 StateReuse = REPLACE_SOME_STATES,
						INT32 Spacing = 0,
						BOOL ContinuousBar = FALSE, BOOL ButtonsAllSameSize = FALSE, String_256 BarName = "Bar1", BOOL FromSelection = TRUE, BYTE ExtendFlags = 3,
						BOOL DelExistingState = FALSE)
			{	
				m_WantMouse = WantMouse;
				m_WantClicked = WantClicked;
				m_WantSelected = WantSelected;
				m_IsVertical = IsVertical;
				m_SuggestDesign = SuggestDesign;
				m_NoOfButtons = NoOfButtons;
				m_WantDefault = WantDefault;
				m_StateReuse = StateReuse;
				m_Spacing = Spacing;
				m_BarName = BarName;
				m_ContinuousBar = ContinuousBar;
				m_ButtonsAllSameSize = ButtonsAllSameSize;
				m_FromSelection = FromSelection;
				m_WantBackBar = WantBackBar;
				m_ExtendFlags = ExtendFlags;
				m_DelExistingState = DelExistingState;
			}

	BOOL m_WantMouse;
	BOOL m_WantClicked;
	BOOL m_WantSelected;
	BOOL m_IsVertical;
	BOOL m_SuggestDesign;
	BOOL m_ContinuousBar;
	BOOL m_ButtonsAllSameSize;
	INT32 m_NoOfButtons;
	BOOL m_WantDefault;
	INT32 m_StateReuse;
	String_256 m_BarName;
	INT32 m_Spacing;
	BOOL m_FromSelection;
	BOOL m_WantBackBar;
	BYTE m_ExtendFlags;
	BOOL m_DelExistingState;
};



/********************************************************************************************
>	struct BarDataType

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/00
	Purpose:	Attributes of bars.
	SeeAlso:	NodeBarProperty
********************************************************************************************/

struct BarDataType
{

	BYTE SameSize;
	unsigned IsLive			 : 1;
	unsigned IsHorizontal	 : 1;
	unsigned RequiresShuffle : 1;
	unsigned ButtonsExtend	 : 1;
	unsigned ButtonsScale	 : 1;
	unsigned GroupsStretch	 : 1;
	INT32 Spacing;

	BarDataType()
	  :	SameSize(0), IsLive(1), IsHorizontal(1), RequiresShuffle(1),
		ButtonsExtend(1), ButtonsScale(0), GroupsStretch(1), Spacing(0)
			{ }
};


// Accesses 'current' document's array of BarDataTypes.
NodeBarProperty* GetBarData();

#endif // have defined this class before
