// $Id: barcreationdlg.h 662 2006-03-14 21:31:49Z alex $
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

#ifndef INC_BARCREATIONDLG
#define INC_BARCREATIONDLG

#include "dialogop.h"

#define OPTOKEN_BARCREATIONDLG _T("BarCreationDlg")
#define OPTOKEN_BARSTATESDLG _T("BarStatesDlg")
#define OPTOKEN_BARREDEFINESTATEDLG _T("BarRedefineStateDlg")



/********************************************************************************************

>	class BarCreationDlg : public DialogOp

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/99
	Purpose:	Dialog to make building button bar rollovers easier

********************************************************************************************/

class BarCreationDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(BarCreationDlg);

	//Constructors, destructors and initialiser
public:
	BarCreationDlg();
	~BarCreationDlg();

	static	BOOL Init();

	// Creating the dialog
public:
	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);

protected:
	BOOL Create();
	BOOL SetUpControls( BOOL DoScan = TRUE);
	BOOL SetUpDynamicControls();
	BOOL CreateBar();
	void ReadBarProperties();
	void WriteBarProperties();
	INT32 GetBarNumber();
	void EnableAllMyControls(BOOL enable);

	// Operations (functions that other objects call)
public:
	virtual	MsgResult Message(Msg* Message);
	static	OpState GetState(String_256*, OpDescriptor*);

	//Member variables
protected:
	static BarCreationDlg *TheDialog;	// Points to the only Web Address dialog object

	BOOL m_IsEditing;
	BOOL m_FirstIsEditing;
	String_256 m_BarName;
	String_256 m_FirstBarName;
	BOOL m_AlreadyInThisFunction;
	BOOL m_HasBackBar;
	BOOL m_ValidBar;
	
public:
	static const UINT32 IDD;
	static const CDlgMode Mode;

public:
	static BOOL ms_WantMouse;
	static BOOL ms_WantClicked;
	static BOOL ms_WantSelected;
	static BOOL ms_WantDefault;
	static BOOL ms_IsVertical;
	static BOOL ms_SuggestDesign;
	static INT32 ms_NoOfButtons;
	static INT32 ms_Mode;
	//static BOOL ms_SameSize;
	static INT32 ms_SizeJustification;
	static BOOL ms_Continuous;
	static INT32 ms_Spacing;
	static INT32 ms_Stretching;
	static BOOL ms_FromSelection;
	static INT32 ms_StateToCreate;
	static BOOL ms_GroupsStretch;
};



class OpParamBarCreationDlg : public OpParam
{
//CC_DECLARE_DYNAMIC(OpParamBarCreation)

public:
	OpParamBarCreationDlg()
			{	
				m_IsEditing = FALSE;
				m_BarName = "Bar1";
				m_ButtonsToEdit = 0;
			}

	OpParamBarCreationDlg(BOOL IsEditing, String_256 BarName = "Bar1", INT32 ButtonsToEdit = 0)
			{	
				m_IsEditing = IsEditing;
				m_BarName = BarName;
				m_ButtonsToEdit = ButtonsToEdit;
			}

	BOOL m_IsEditing;
	String_256 m_BarName;
	INT32 m_ButtonsToEdit;
};

/********************************************************************************************

>	class BarStatesDlg : public DialogOp

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/00
	Purpose:	Dialog to make building button bar states

********************************************************************************************/

class BarStatesDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(BarStatesDlg);

	//Constructors, destructors and initialiser
public:
	BarStatesDlg();
	~BarStatesDlg();

	static	BOOL Init();

	// Creating the dialog
public:
	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);

protected:
	BOOL Create();
	void SetUpControls();
	BOOL ScanForBarDetails();
	BOOL CreateBarStates(BOOL ReCreate = FALSE);
	BOOL OnSelectionChanged();

	// Operations (functions that other objects call)
public:
	virtual	MsgResult Message(Msg* Message);
	static	OpState GetState(String_256*, OpDescriptor*);

	//Member variables
protected:
	static BarStatesDlg *TheDialog;	// Points to the only dialog object
	String_256 m_BarName;
	static INT32 m_State;
	static INT32 ms_SuggestDesign;
	BOOL m_StateExists[5];
	BOOL m_ValidBar;

public:
	static const UINT32 IDD;
	static const CDlgMode Mode;

};



class OpParamBarStatesDlg : public OpParam
{

public:
	OpParamBarStatesDlg()
			{	
				m_BarName = "Bar1";
			}

	OpParamBarStatesDlg(String_256 BarName)
			{	
				m_BarName = BarName;
			}

	String_256 m_BarName;
};






class BarRedefineStateDlg : public DialogOp
{
	CC_DECLARE_DYNCREATE(BarRedefineStateDlg);

	//Constructors, destructors and initialiser
public:
	BarRedefineStateDlg();
	~BarRedefineStateDlg();

	static	BOOL Init();

	// Creating the dialog
public:
	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);
	BOOL IsAttribInSelected(Node * pNode);

protected:
	BOOL Create();
	BOOL SetUpControls();
	BOOL RedefineState();

	// Operations (functions that other objects call)
public:
	virtual	MsgResult Message(Msg* Message);
	static	OpState GetState(String_256*, OpDescriptor*);

	//Member variables
protected:
	static BarRedefineStateDlg *TheDialog;	// Points to the only dialog object
	String_256 m_BarName;
	INT32 m_State;

public:
	static const UINT32 IDD;
	static const CDlgMode Mode;

};

class OpParamBarRedefineStateDlg : public OpParam
{

public:
	OpParamBarRedefineStateDlg()
			{	
				m_BarName = "Bar1";
				m_State	= 0;
			}

	OpParamBarRedefineStateDlg(String_256 BarName, INT32 State)
			{	
				m_BarName = BarName;
				m_State = State;
			}

	String_256 m_BarName;
	INT32 m_State;
};





#endif //INC_BARCREATIONDLG

