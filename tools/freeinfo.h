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
// FreeHand Tool Info bar operations


#ifndef FREEINFO_INC
#define FREEINFO_INC

//#include "bars.h" - in camtypes.h [AUTOMATICALLY REMOVED]
class FreeHandTool;

#include "brshgdgt.h"
#include "brshcomp.h"  // for brushhandle
#include "valfunc.h"  // for valuefunctionID

#include <list>

// list to contain brushhandles in order of most recent use
typedef list<BrushHandle> BrushList;

/********************************************************************************************

>	class FreeHandInfoBarOp : public InformationBarOp

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for handling the FreeHand tool's information bar.

********************************************************************************************/

class FreeHandInfoBarOp : public InformationBarOp
{
	CC_DECLARE_DYNCREATE( FreeHandInfoBarOp )  
public:
	// Dummy default constructor for DYNCREATE
	FreeHandInfoBarOp();
	~FreeHandInfoBarOp();

	// All messages to the info bar come through here
	MsgResult 	Message(Msg* Msg);

	// Functions to allow the tool to talk to the info bar
	void SetToolActiveState(BOOL IsToolActive);
	void SetRetroState(BOOL CanRetroFit);
	void SetRetroString();

	void InitBrushes ();
	void AddBrush();
	void AddBrush(BrushHandle Handle);

	virtual void UpdateState(); // Called during idle processing to disable controls

	BrushData GetBrushDataFromSelection(BOOL* pSelEmpty = NULL);
	void	  SetDefaultStrokeInCombo();

	void RemoveBrush(BrushHandle Handle);

PORTNOTE("other", "Disabled Brush editing")
#ifndef EXCLUDE_FROM_XARALX
	static CBrushGadget* GetBrushGadget() { return &BrushGadget;}
#endif

protected:
	BOOL InitBrushList();  // initialises the brush list
	BOOL ReInitBrushList(); // re-initialises but remembers the previous order
	BOOL SetBrushListToControl();
	BOOL SetStrokesToControl(); 
protected:  
	// in a vain attempt to clear up the message proc I'm farming out some of it here
	void	HandleBrushSelectChange();
	void	HandleCreate();
	void	HandleSliderPosChanging();
	void	HandleStrokeSelectChange();
	
	void	LaunchBrushEditDialog();
	void	SetHeadOfListSelected();
	void	SetDisplayBrush();
	void	SetBrushComboFromSelection();

	BrushHandle	GetBrushHandleFromSelection(BOOL* pSelEmpty = NULL);

	void	SetStrokeComboFromSelection();
	INT32		GetStrokeIndexFromValueFunctionID(ValueFunctionID ID);

protected: // helper functions dealing with the brush list
	BrushHandle	GetBrushHandleFromControlIndex(BrushHandle Index); 
	INT32 GetControlIndexFromBrushHandle(BrushHandle Handle);
	void		SetHeadOfList(BrushHandle Handle);		

public:	
	// Keep a pointer to the Freehand tool in here
	FreeHandTool*	pTool;
	BOOL 			CanRetroFit;
	
	
protected:
	// Since we get Selection Changing message before the controls are made
	// we need to keep a flag to indicate if the controls have been built
	BOOL 			ControlsExist;
	BOOL			DropDownInited;
	
/*	
	StrokeList*		m_pStrokeList;

	StrokeHandle	m_DisplayStroke;
*/
	INT32			m_LastSmoothness;

	BrushList*		m_pBrushList;

	BrushHandle		m_DisplayBrush;   // the currently displayed brush in the combo

PORTNOTE("other", "Disabled Brush editing")
#ifndef EXCLUDE_FROM_XARALX
	static CBrushGadget	BrushGadget; // we only ever want one version of the brush edit dialog
									 // hence the static gadget
#endif

	INT32			    m_SelectedStroke; // the last selected stroke
};
	

/********************************************************************************************

>	class FreeHandInfoBarOpCreate : public BarCreate

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/4/94
	Purpose:	Class for creating FreeHandInfoBarOps.
				Derived classes of BarCreate are used by DialogBarOp::ReadBarsFromFile()

********************************************************************************************/

class FreeHandInfoBarOpCreate : public BarCreate
{
public:
	DialogBarOp*	Create() { return (new FreeHandInfoBarOp); }
};



#endif	// FREEINFO_INC






