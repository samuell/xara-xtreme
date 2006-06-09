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
// Interface of the ExtendSetsDlg class.

#ifndef INC_EXTENDSETSDLG
#define INC_EXTENDSETSDLG

#include "chcklist.h"	// for CTextCheckListBox
#include "extender.h"	// for bitwise extend-type flags
//#include "dialogop.h"	// for DialogOp - in camtypes.h [AUTOMATICALLY REMOVED]

class SGNameItem;


//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************
>	class ExtendSetsDlg : public DialogOp

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/1999
	Purpose:	Dialog to allow the user to choose which named sets are extended
				by which other named sets.
********************************************************************************************/

#define OPTOKEN_EXTENDSETSDLG _T("ExtendSetsDlg")

class ExtendSetsDlg : public DialogOp
{
	// runtime class creation stuff.
	CC_DECLARE_DYNCREATE(ExtendSetsDlg);

/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Member functions.
//

public:
	// constructor, destructor & initialisation.
	ExtendSetsDlg();
	~ExtendSetsDlg();
	static BOOL Init();

	// dialog creation.
	void Do(OpDescriptor*);
	void DoWithParam(OpDescriptor* pOp, OpParam* pParam);

	// Operations (functions that other objects call)
	virtual	MsgResult Message(Msg* Message);
	static	OpState GetState(String_256*, OpDescriptor*);

	// Ask this dialog for a list of named sets ticked in the checklist.
//	List* GetNamedSets();

protected:
	// dialog creation.
	BOOL Create();
	BOOL SetUpControls();

	// list-of-named-set manipulation.
	BOOL FillCheckListBox();
	void GenerateListOfSets();



/////////////////////////////////////////////////////////////////////////////////////////////
//
//	Member variables.
//

public:
	// Dialog ID and type (modal/modeless).
	static const UINT32 IDD;
	static const CDlgMode Mode;

protected:
	// pointer to the one and only instance of this dialog.
	// should this be used or dropped? not sure yet.
	static ExtendSetsDlg* TheDialog;

	// a pointer to the named set which defines the extension.
	BYTE*			m_bArrayExtendFlags;	// array of bytes describing which sets stretch.
	SGNameItem*		m_pDefineExtendSet;		// pointer to named set which defines extension.
	BOOL*			m_pbCommit;				// pointer we can use to flag OK or Cancel.
	BYTE*			m_pfExtendFlags;		// ptr to flags we can set for extend behaviour.

	// listbox of sets which the user can choose from.
	CTextCheckListBox m_tclbExtendingSets;

	// constants used as indices into the extend behaviour combo-boxes.
	static const INT32 EXTEND_BEHAVIOUR_NONE;
	static const INT32 EXTEND_BEHAVIOUR_EXTEND;
	static const INT32 EXTEND_BEHAVIOUR_STRETCH;
};



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



/********************************************************************************************
>	class OpParamExtendSetsDlg : public OpParam

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/11/1999
	Purpose:	A parameter class for creating the ExtendSetsDlg with.
	See also:	ExtendSetsDlg.
	Errors:		ERROR2RAW if the default constructor is called.
********************************************************************************************/

class OpParamExtendSetsDlg : public OpParam
{
public:
	OpParamExtendSetsDlg()
	{
		ERROR2RAW("OpParamExtendSetsDlg default constructor should not be called!");
		m_bArrayExtendFlags	= NULL;
		m_pDefineExtendSet	= NULL;
		m_pbCommit			= NULL;
		m_pfExtendFlags		= NULL;
	}

	OpParamExtendSetsDlg(	BYTE* bArrayExtendFlags, SGNameItem* pDefineExtendSet,
							BOOL* pbCommit, BYTE* pfExtendFlags )
	{
		m_bArrayExtendFlags	= bArrayExtendFlags;
		m_pDefineExtendSet	= pDefineExtendSet;
		m_pbCommit			= pbCommit;
		m_pfExtendFlags		= pfExtendFlags;
	}

	BYTE* m_bArrayExtendFlags;		// an array of bytes, one for each named set - whether it
									// should be ticked in the dialog, (visible? enabled?).
	SGNameItem* m_pDefineExtendSet;	// the named set which is being extended.
	BOOL* m_pbCommit;				// ptr to a BOOL denoting whether OK or Cancel pressed.
	BYTE* m_pfExtendFlags;			// ptr to a bitwise set of flags for extend behaviour.
};



//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------



#endif	// !INC_EXTENDSETSDLG
