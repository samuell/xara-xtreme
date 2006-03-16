// $Id: brushdlg.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef BRUSHDLG_H
#define BRUSHDLG_H

#define OPTOKEN_BRUSHEDIT_DLG	_T("CBrushEditDlg")
#define OPTOKEN_OPBRUSHEDIT		_T("OpBrushEdit")

#include "brshgdgt.h"
#include "brshdata.h"
#include "opdrbrsh.h"

enum RenderPreviewType { RT_LINE, RT_S, RT_LAST };
typedef enum RectSideInfo {WIDTH, HEIGHT, LONGEST};

class CBrushEditDlg : public DialogTabOp//DialogOp
{
	CC_DECLARE_DYNCREATE( CBrushEditDlg )
	
	private:
	// ptr to the owning gadget
		CBrushGadget*		pOwningGadget_m;
		
	// identifier for this particular instance
		CGadgetID			Id_m;

	// our private brush data
		BrushData*			OriginalData;
		BrushData*			EditData;
			
		BrushData			m_LastData;
		BrushHandle         m_Handle;           // if we are editing a definition, this is its handle

		AttrBrushType*      pAttrBrush_m;		// pointer to the node we are altering
		AttrBrushType*      pOrigAttrBrush_m;	// pointer to a copy of the node we are altering

		ChangeBrushType     ChangeType_m;		// the change that we are going to commit
		ChangeBrushOpParam  ChangeParam_m;

		CWindowID			PreviewWindow;		// handle to our preview window

		BOOL				FirstTime;			// is this the first time weve
												// shown the dialog (with the
												// brush preview)
	
		BOOL				EditingBrushDef;	// are we editing the brush definition?
		BOOL				m_bDisableOps;      // prevents us from launching operations
		RenderPreviewType	RenderType;			// what style we render previews in

		DocUnitList* pDocUnitList;				// ptr to document units
		UnitType 	 CurrentUserUnitType;		// the current unit type (for output in edit boxes)

		BOOL				m_bDisableGadgets;

		BOOL				m_bPreventUpdateSet;
		BOOL				m_SliderStarted;   // set when a slider movement begins

		static BOOL			m_bIsOpen;          // as we only ever want one instance of this so keep track of when we open it

		void HideShuffleIrrelevantButtons ();
		void ResizeDialogAndCreateBrushPreview ();
		void RenderBrushPreview (ReDrawInfoType* RedrawInfo);
		void CycleToNextPreviewStyle ();
		
		void InitBrushSpacing (BOOL OnlyForUnitChange = FALSE);
		void InitBrushOffset (BOOL OnlyForUnitChange = FALSE);
		void InitBrushScaling (BOOL OnlyForUnitChange = FALSE);
		void InitBrushEffects (BOOL OnlyForUnitChange = FALSE);
		void InitBrushSequence (BOOL OnlyForUnitChange = FALSE);
		void InitBrushFill (BOOL OnlyForUnitChange = FALSE);

		void ReInitBrushSpacing ();
		void ReInitBrushOffset ();
		void ReInitBrushScaling ();
		void ReInitBrushEffects ();
		void ReInitBrushSequence ();
		void ReInitBrushFill();
		void ReInitUnitDependantFields (CDlgResID ThePage);

		void InitComboPathOffsetType ();
		void InitSequenceType ();

		void HandleCommit (DialogMsg* Msg);
		void HandleSelectionChange (DialogMsg* Msg);
		void HandleButtonDown (DialogMsg* Msg);
		void HandleSliderPosChanging (DialogMsg* Msg);
		void HandleSliderPosSet (DialogMsg* Msg);

		void ReadButtons ( DialogMsg* Msg);
		void ReadRadios (DialogMsg* Msg);
		void ReadEditBox (DialogMsg* Msg);
		void ReadComboBox (DialogMsg* Msg);
		void ReadCheckBox (DialogMsg* Msg);
		void ReadSlider (DialogMsg* Msg);
		void WriteSliders (DialogMsg* Msg);

		BOOL UsingRandomSpacing;
		BOOL UsingRandomOffset;
		BOOL UsingRandomScaling;

		CGadgetID EditGadgetLostFocus;

		void UpdateEditBox (CGadgetID GadgetToUpdate);

		void LaunchOp();
		UINT32 GetNewRandomNumber(UINT32 OldNumber);
		void ReInitPage(CDlgResID PageID);
	
		BOOL GetAttributeNodeFromDefinition();

		void HandleDocSelectionChange();
		void RefreshActivePage();

		void GetSpacingRange(double*, double*);
		void SetSpacingSlider();
		void SetSpacingIncrSlider();
	
		void SetOffsetValSlider();
		void GetOffsetRange(double*, double*);

		MILLIPOINT GetSideOfBoundingBox(RectSideInfo RectInfo);
		double GetDistanceAsPercentageOfBBox(MILLIPOINT Distance, RectSideInfo RectInfo);
		MILLIPOINT GetPercentageAsDistance(double Percent, RectSideInfo RectInfo);
		void ChangeButtonText();

		void SetRandomRangeText(CGadgetID SliderID);
		String_32 GetRandomRangeText(INT32 SliderVal);
		String_32 GetRotationRandomText(INT32 SliderVal);
		String_32 GetRandomText(INT32 Value, INT32 MaxValue);

		// we need to subclass our edit boxes, so we'll use the helper
		virtual BOOL AddControlsToHelper();
		virtual void RemoveControlsFromHelper();

	public:
		// dialog constructor
		CBrushEditDlg ();
		// dialog destructor
		~CBrushEditDlg ();

		// dialog details ----------------------------------------
		
		static const		UINT32	IDD;
		static const		CDlgMode	Mode;

		// usual interface ----------------------------------------
		static	BOOL		Init ();
		static	OpState		GetState (String_256*,  OpDescriptor*);

		void				Do (OpDescriptor*);
	
		void				DoWithParam ( OpDescriptor*,  OpParam* OwningGadgetParams );
		virtual	MsgResult	Message( Msg* );

		void				CreateNewBrush();

		void HandleBrushEditSpacingMsg (DialogMsg* Msg);
		void HandleBrushEditOffsetMsg (DialogMsg* Msg);
		void HandleBrushEditScalingMsg (DialogMsg* Msg);
		void HandleBrushEditEffectMsg (DialogMsg* Msg);
		void HandleBrushEditSequenceMsg (DialogMsg* Msg);
		void HandleBrushFillMsg (DialogMsg* Msg);

		void InvokeVia (CBrushGadget& pInvokeWith, BrushData* pInvokeOn, 
						AttrBrushType* pBrush, BOOL& bMany);
		void InvokeVia (CBrushGadget& pInvokeWith, BrushData* pInvokeOn, 
						AttrBrushType* pBrush, AttrBrushType* pOrigBrush, BOOL& bMany);
		void InvokeVia (CBrushGadget& pInvokeWith, BrushHandle Handle);

		virtual BOOL RegisterYourPagesInOrderPlease ();	// Add in tab pages
		void ReInitialiseDialog (BrushData* pData);	
	
		void SetDialogTitle(String_256 Title);
		static AttrBrushType* GetSingleSelectedBrushAttribute();
		
};

#endif
