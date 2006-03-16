// $Id: ngsetop.h 662 2006-03-14 21:31:49Z alex $
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
/*
	$Header: /Camelot/kernel/ngsetop.h 6     28/07/00 11:51 Simonk $
	Operations to manipulate sets and names of objects.
*/

#ifndef NGSETOP_H
#define NGSETOP_H

#include "selop.h"
#include "ngiter.h"

class SGNameItem;
class NodeBarProperty;
struct BarDataType;


/********************************************************************************************
>	class CCAPI OpSelectSet : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Changes the selection state of the given set of objects (SGNameItem*).
********************************************************************************************/

#define OPTOKEN_SELECT_SET						_T("SelectSetOfObjects")

class CCAPI OpSelectSet : public Operation
{
	CC_DECLARE_DYNCREATE(OpSelectSet);
	virtual void DoWithParam(OpDescriptor*, OpParam*);
	// GetState is GREY_WHEN_NO_CURRENT_DOC
};



/********************************************************************************************
>	class CCAPI OpSelectUnionSets : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Selects the union of all objects with highlighted names in the
				Attribute gallery.
********************************************************************************************/

#define OPTOKEN_SELECT_UNION_SETS				_T("SelectUnionOfSets")

class CCAPI OpSelectUnionSets : public Operation
{
	CC_DECLARE_DYNCREATE(OpSelectUnionSets);
	virtual void Do(OpDescriptor*);
public:
	static OpState GetState(String_256*, OpDescriptor*);		
};



/********************************************************************************************
>	class CCAPI OpSelectIntersectSets : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Selects the intersection of all objects with highlighted names in the
				Attribute gallery.
********************************************************************************************/

#define OPTOKEN_SELECT_INTERSECT_SETS			_T("SelectIntersectOfSets")

class CCAPI OpSelectIntersectSets : public Operation
{
	CC_DECLARE_DYNCREATE(OpSelectIntersectSets);
	virtual void Do(OpDescriptor*);
public:
	static OpState GetState(String_256*, OpDescriptor*);		
};



/********************************************************************************************
>	class CCAPI OpApplyNamesToSel : public SelOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Applies names highlighted in the Attribute gallery, or a name
				passed as a string parameter, to the selected objects.
	Notes:		There are two OpDescriptors for this operation, 
				OPTOKEN_APPLY_NAMES_TO_SEL (invoking Do) which applies the highlighted
				names in the Attribute gallery to the selected objects,	and
				OPTOKEN_APPLY_NAME_TO_SEL (invoking DoWithParam), which applies as a
				name a character string argument (a StringBase* cast into OpParam::Param1)
				to the selected objects.
********************************************************************************************/

#define OPTOKEN_APPLY_NAMES_TO_SEL			_T("ApplyNamesToSelectedObjects")
#define OPTOKEN_APPLY_NAME_TO_SEL			_T("ApplyNameToSelectedObjects")

class CCAPI OpApplyNamesToSel : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpApplyNamesToSel);
	virtual void Do(OpDescriptor*);
	virtual void DoWithParam(OpDescriptor*, OpParam*);
public:
	static OpState GetState(String_256*, OpDescriptor*);	// for multiple version
};



/********************************************************************************************
>	class CCAPI OpApplyNamesToOne : public UndoableOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Applies names highlighted in the Attribute gallery to a given
				object.  Executed by the Attribute gallery's drag'n'drop system.
********************************************************************************************/

#define OPTOKEN_APPLY_NAMES_TO_ONE			_T("ApplyNamesToOneObject")

class CCAPI OpApplyNamesToOne : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpApplyNamesToOne);
	virtual void DoWithParam(OpDescriptor*, OpParam* pOpParam);

public:
	static OpState GetState(String_256*, OpDescriptor*);

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const { return TRUE; }
};



/********************************************************************************************
>	class CCAPI OpApplyNameToNone : public UndoableOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Applies the given name to no objects (ie. just to the NodeSetSentinel
				object).
********************************************************************************************/

#define OPTOKEN_APPLY_NAME_TO_NONE			_T("ApplyNameToNoObjects")

class CCAPI OpApplyNameToNone : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpApplyNameToNone);
	virtual void DoWithParam(OpDescriptor*, OpParam* pOpParam);
	// GetState is GREY_WHEN_NO_CURRENT_DOC
};



/********************************************************************************************
>	class CCAPI OpRemoveNamesFromSel : public UndoableOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Removes (hides) highlighted names in the Attribute gallery from the
				selected objects.
********************************************************************************************/

#define OPTOKEN_REMOVE_NAMES_FROM_SEL		_T("RemoveNamesFromSelectedObjects")

class CCAPI OpRemoveNamesFromSel : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpRemoveNamesFromSel);
	virtual void Do(OpDescriptor*);
	// GetState same as OPTOKEN_APPLY_NAMES_TO_SEL
};



/********************************************************************************************
>	class CCAPI OpDeleteNamesFromAll : public UndoableOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Deletes (hides) names highlighted in the Attribute gallery from all
				objects.
********************************************************************************************/

#define OPTOKEN_DELETE_NAMES_FROM_ALL		_T("DeleteNamesFromAllObjects")

class CCAPI OpDeleteNamesFromAll : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpDeleteNamesFromAll);
	virtual void Do(OpDescriptor*);
	// GetState same as OPTOKEN_SELECT_UNION_SETS
};



/********************************************************************************************
>	class CCAPI OpRenameAll : public UndoableOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Renames the objects with highlighted names in the Attribute gallery
				to the new name in a string parameter.
********************************************************************************************/

#define OPTOKEN_RENAME_ALL					_T("RenameAllObjects")

class CCAPI OpRenameAll : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpRenameAll);
	virtual void DoWithParam(OpDescriptor*, OpParam*);
	// GetState same as OPTOKEN_APPLY_NAMES_TO_ONE

	// Karim 21/01/2000 - see UndoableOperation base implementation.
	virtual BOOL MayChangeNodeBounds() const	{ return TRUE; }
};



/********************************************************************************************
>	class CCAPI OpRedefineNamesAsSel : public SelOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Redefines (reapplies) the names highlighted in the Attribute gallery
				as the selected objects.
********************************************************************************************/

#define OPTOKEN_REDEFINE_NAMES_AS_SEL		_T("RedefineNamesAsSelectedObjects")

class CCAPI OpRedefineNamesAsSel : public SelOperation
{
	CC_DECLARE_DYNCREATE(OpRedefineNamesAsSel);
	virtual void Do(OpDescriptor*);
	// GetState same as OPTOKEN_APPLY_NAMES_TO_SEL
};




/********************************************************************************************
>	class CCAPI OpExportSets : public Operation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Exports sets of objects highlighted in the Attribute gallery.
********************************************************************************************/

#define OPTOKEN_EXPORT_SETS					_T("ExportSets")

class CCAPI OpExportSets : public Operation
{
	CC_DECLARE_DYNCREATE(OpExportSets);
	virtual void Do(OpDescriptor*);
	static OpState GetState(String_256*, OpDescriptor*);


	/********************************************************************************************
	>	class CCAPI OpExpotrSets::ExportHighlightedIter : public NameItemIter

		Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
		Created:	23/11/1999
		Purpose:	Iterates over highlighted set items, re-exporting any that are
					export targets.
	********************************************************************************************/

	class CCAPI ExportHighlightedIter : public NameItemIter
	{
	public:
		ExportHighlightedIter(OpExportSets* pOp);
		virtual ~ExportHighlightedIter();

	protected:
		virtual BOOL Do(SGNameItem*);

		OpExportSets* m_pOp;
		String_256 m_strExtErrors;
	};
};



/********************************************************************************************
>	class CCAPI OpChangeSetProperty : public UndoableOperation

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Changes the set property associated with the SGNameProp* passed
				in OpParam::Param1.
********************************************************************************************/

#define OPTOKEN_CHANGE_SET_PROPERTY			_T("ChangeSetProperty")

class CCAPI OpChangeSetProperty : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpChangeSetProperty);
	virtual void DoWithParam(OpDescriptor*, OpParam* pParam);
	// GetState is GREY_WHEN_NO_CURRENT_DOC
};



/********************************************************************************************
>	class CCAPI OpChangeBarProperty : public UndoableOperation
	class CCAPI OpChangeBarPropParam : public OpParam

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Changes the bar property passed in OpChangeBarPropParam.
********************************************************************************************/

#define OPTOKEN_CHANGE_BAR_PROPERTY			_T("ChangeBarProperty")

class CCAPI OpChangeBarProperty : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpChangeBarProperty);
	virtual void DoWithParam(OpDescriptor*, OpParam* pParam);
	// GetState is GREY_WHEN_NO_CURRENT_DOC

	virtual BOOL MayChangeNodeBounds() const		{ return TRUE; }
	INT32 m_BarIndex; // used by the undo and redo
	BOOL m_MakingLive; // Does this op turn the live stretching back on? Used in extending (ngcore.cpp)

	static BOOL DoChangeBarProperty(UndoableOperation * pOp, INT32 Index, NodeBarProperty* pProp, const BarDataType * pBarData, BOOL * pMadeLive = NULL);
};


struct CCAPI OpChangeBarPropParam : public OpParam
{
	NodeBarProperty* m_Prop;
	INT32 m_nIndex;
	const BarDataType* m_pbdtInfo;
};



/********************************************************************************************
>	class CCAPI OpChangeBarExtends : public UndoableOperation
	class CCAPI OpChangeBarPropParam : public OpParam

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Purpose:	Changes the extending method (ie the flag) for every named set that has
				a matching m_BarNumber to that passed in.
********************************************************************************************/

#define OPTOKEN_CHANGE_BAR_EXTENDS				_T("ChangeBarExtends")

class CCAPI OpChangeBarExtends : public UndoableOperation
{
	CC_DECLARE_DYNCREATE(OpChangeBarExtends);
	virtual void DoWithParam(OpDescriptor*, OpParam* pParam);
	// GetState is GREY_WHEN_NO_CURRENT_DOC
};



// This function is responsible for initialising all other components of the Attribute
// gallery.
BOOL InitNamingSystem();

#endif	/* !NGSETOP_H */
