// $Id: brshname.h 662 2006-03-14 21:31:49Z alex $
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

// brush name dialog implementation
#ifndef BRSHNAME
#define BRSHNAME

#include "dialogop.h"
#include "brshcomp.h"

/********************************************************************************************
>	class CCAPI CBaseBrushNameDlg : public DialogOp
	class CCAPI CBrushNameDlg : public CBaseBrushNameDlg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	Base class for the dialog that is used to name or rename brushes
	SeeAlso:	DialogOp
********************************************************************************************/

class CBaseBrushNameDlg : public DialogOp
{         
CC_DECLARE_DYNAMIC(CBaseBrushNameDlg)
static BOOL m_bModeless;
protected:
	// Creation & destruction.
	CBaseBrushNameDlg(CDlgResID idRes, CDlgMode nMode);

	// Description of what is to be (re)named.
	virtual StringBase* GetDescription(StringBase* pstrDesc) = 0;

	// A suggestion for the (re)name.
	virtual StringBase* GetSuggestion(StringBase* pstrDesc) = 0;

	// Returns zero if valid, the ID of an explanative error string if not.
	virtual UINT32 IsValid(const StringBase& strName);

	// This is called when the user enters a valid name and clicks the OK button.
	virtual BOOL DoCommit(const StringBase& strName) = 0;

	virtual void HandleCancel() {};

	// Event handlers.
	virtual void Do(OpDescriptor*);
	virtual void DoWithParam(OpDescriptor*,  OpParam* pParam);
	virtual MsgResult Message(Msg* pMessage);

	// Helper that sets the text in the static and edit controls, the initial focus etc.
	virtual void InitGadgetText(String_32* pString = NULL, BOOL resizeDialog = TRUE) = 0;

protected: //data
	BrushHandle m_BrushHandle;

	// Data.
	String_32 m_strSuggest;

	
};


/********************************************************************************************
	class CBrushNameDlg : public CBaseBrushNameDlg
	class  CRenameBrushDlg : public CBaseBrushNameDlg

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	dialog that is used to name brushes
	SeeAlso:	DialogOp
********************************************************************************************/

#define OPTOKEN_NAME_BRUSH_DLG			_T("NameBrushDlg")

class CNameBrushDlg  : public CBaseBrushNameDlg
{	
	CC_DECLARE_DYNCREATE(CNameBrushDlg);

public:
	static BOOL Init();
	static	OpState		GetState (String_256*,  OpDescriptor*);
protected:
	CNameBrushDlg();

	virtual StringBase* GetDescription(StringBase* pstrDesc);
	virtual StringBase* GetSuggestion(StringBase* pstrDesc);
	virtual UINT32 IsValid(const StringBase& strName);
	virtual BOOL DoCommit(const StringBase& strName);
	virtual void InitGadgetText(String_32* pString = NULL, BOOL resizeDialog = TRUE);
};



/********************************************************************************************
	class CInitBrushNameDlg : public CNameBrushDlg


	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/6/2000
	Purpose:	this is the initial dialog that is brought up to name the brush upon creation
				it differs from the above class in that this naming is not undoable
	SeeAlso:	DialogOp
********************************************************************************************/

#define OPTOKEN_INITNAME_BRUSH_DLG			_T("InitNameBrushDlg")

class CInitBrushNameDlg : public CNameBrushDlg
{
	CC_DECLARE_DYNCREATE(CInitBrushNameDlg)
	
public:
	static BOOL Init();
	static	OpState		GetState (String_256*,  OpDescriptor*);

protected:
	CInitBrushNameDlg();
	virtual BOOL DoCommit(const StringBase& strName);
	virtual void HandleCancel();
	virtual void InitGadgetText(String_32* pString = NULL, BOOL resizeDialog = TRUE);
};
#endif
