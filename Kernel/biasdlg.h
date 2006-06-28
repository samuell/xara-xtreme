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


#ifndef  Included_CBiasGainDlg
#define  Included_CBiasGainDlg




#include "rnddlgs.h"
#include "transop.h"//"selop.h"
#include "biasgain.h"
#include "biasgdgt.h"
#include "bitmapdropdown.h"



#define  OPTOKEN_BIASGAIN_DLG  _T("CBiasGainDlg")
#define  OPTOKEN_OPBIASGAIN    _T("OpBiasGain")




//-------------------------------------------------------------------------------------------------
//
//	class  CBiasGainDlg
//
//	Author:     Harrison Ainsworth
//	Date:		05/99
//	Purpose:	be a dialog for inputing a bias/gain choice. to be owned and called from a 
//				CBiasGainGadget
//	Implemen-	DoWithParam() is supplied with a pointer to the owning gadget (in the OpParam), this 
//	tation:		is mainly used to tell the gadget when the dlg is running, which the gadget uses to 
//				ensure that only one (of its) dlgs is running at a time. So: you can have multiple of
//				these biasgain dialogs running, but each is used for a different purpose and is owned 
//				by a separate gadget.
//
//-------------------------------------------------------------------------------------------------

class  CBiasGainDlg  :  public  DialogOp
{         
	CC_DECLARE_DYNCREATE( CBiasGainDlg )

public: //-----------------------------------------------------------------------------------------

	// standard object services -------------------------------
	
						CBiasGainDlg ();
	
						~CBiasGainDlg ();
	//					CBiasGainDlg ( const CBiasGainDlg& );
	//CBiasGainDlg&		operator= ( const CBiasGainDlg& );

	// --------------------------------------------------------


	// constants ----------------------------------------------
	static const UINT32	IDD;
	static const CDlgMode	Mode;


	// usual interface ----------------------------------------
	static	BOOL		Init ();
	static	OpState		GetState ( String_256*,  OpDescriptor* );

	void				Do ( OpDescriptor* );
						// OwningGadgetParams: param1 is a CBiasGainGadget*, and param2 is a CProfileBiasGain*
	void				DoWithParam ( OpDescriptor*,  OpParam* OwningGadgetParams );
	virtual	MsgResult	Message( Msg* );

	
	// DMc
	// Sets the profile for the dialog, and updates all the controls
	//void SetProfile(CProfileBiasGain const &Profile);

	// CGS
	void InvokeVia (CBiasGainGadget& pInvokeWith, CProfileBiasGain* pInvokeOn, BOOL bMany);
	void ReInitialiseDialog (CProfileBiasGain* ReInitOn, BOOL bMany);
	void DisableAllControls ();
	void EnableAllControls ();
	void ToggleFillProfile () { useFillProfile = TRUE; }
	void ToggleTranspFillProfile () { useTranspFillProfile = TRUE; }
	// we need direct access the dialogs 'edited' biasgain within elans feathering bar ....
	CProfileBiasGain GetCurrentDialogProfile ();

protected: //--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--

	virtual	void		DrawCurve ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );
	virtual	void		DrawCurveInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );
	virtual	void		DrawFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );
	virtual	void		DrawTranspFillInterpolate ( const CProfileBiasGain& CurveShape,  ReDrawInfoType* ExtraInfo );


	// sliders interface --------------------------------------
	virtual	void		InitSliders  ( CProfileBiasGain const& );
	virtual	void		WriteSliders ( CProfileBiasGain const& );
	virtual	void		ReadSliders  ( CProfileBiasGain& );
	virtual void		InitiProfileCombobox();
			void		InitOneSlider( CGadgetID GadgetID );
			void		WriteOneSlider ( AFp ValueMinus1ToPlus1, CGadgetID GadgetID );
			AFp			ReadOneSlider ( CGadgetID GadgetID );
			void		InitBiasGainGadget ( CProfileBiasGain const& );

	static const INT32	kSliderLength_s;
	static const INT32	kSliderHalfLength_s;
	static const INT32	kSliderMin_s;
	static const INT32	kSliderMax_s;


	// edit boxes interface -----------------------------------
	virtual	void		InitEditBoxes  ( CProfileBiasGain const& );
	virtual	void		WriteEditBoxes ( CProfileBiasGain const& );
	virtual	void		ReadEditBoxes  ( CProfileBiasGain& );

private: /////////////////////////////////////////////////////////////

	// functions ----------------------------------------------
	//
	// message handlers
	virtual	void		HandleSliderPosChanging ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleSliderPosSet      ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleCommit            ( CDlgMessage const&,  CProfileBiasGain& );
	virtual	void		HandleInput             ( CDlgMessage const&,  CProfileBiasGain& );

	// communicate bias gain values to outside
	virtual void		BroadcastMessage ( CDlgMessage const&,  CProfileBiasGain const& )		const;


	// members ------------------------------------------------
	// 
	// ptr to the owning gadget
	CBiasGainGadget*	pOwningGadget_m;

	// the 'contained' profile gadget		(CGS - 10/1/99)
	CBiasGainGadget BiasGainGadget;

	// identifier for this particular instance
	CGadgetID			Id_m;

	CBitmapDropDown* m_pobddStandardProfile;

	// the curve representation
	CProfileBiasGain	BiasGain_m;

	// whether we disabled all controls or not (i.e.  whether we were asked to display MANY
	// profiles
	BOOL manyDisabledControls;

	// whether we should render an object profile or a fill profile internally
	// (by default we render using an object profile)
	BOOL useFillProfile;

	// whether we should render an object profile or a transparency fill profile internally
	// (by default we render using an object profile)
	BOOL useTranspFillProfile;


private:
	// Copying is disallowed
						CBiasGainDlg ( const CBiasGainDlg& );
	CBiasGainDlg&		operator= ( const CBiasGainDlg& );


}; 



#endif//Included_CBiasGainDlg
