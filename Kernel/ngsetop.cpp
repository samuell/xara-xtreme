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
/*
	$Header: /Camelot/kernel/ngsetop.cpp 16    19/12/00 9:37 Matt $
	Operations to manipulate sets and names of objects.
*/

#include "camtypes.h"

#include "ngcore.h"
#include "ngdialog.h"
#include "ngitem.h"
#include "ngprop.h"
#include "ngscan.h"
#include "ngsentry.h"
#include "ngsetop.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "nodeattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "keypress.h"
//#include "nodetext.h"		// text tool bodges :(

//#include "cxfrech.h"		// Wix ObjectName attributes - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "attrval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "userattr.h"
#include "tmpltatr.h"

#include "impexpop.h"
#include "bitfilt.h"

#include "ophist.h"

//#include "mario.h"
//#include "justin3.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(OpSelectSet,				Operation);
CC_IMPLEMENT_DYNCREATE(OpSelectUnionSets,		Operation);
CC_IMPLEMENT_DYNCREATE(OpSelectIntersectSets,	Operation);
CC_IMPLEMENT_DYNCREATE(OpApplyNamesToSel,		SelOperation);
CC_IMPLEMENT_DYNCREATE(OpApplyNamesToOne,		UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpApplyNameToNone,		UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpDeleteNamesFromAll,	UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpRenameAll,				UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpRemoveNamesFromSel,	SelOperation);
CC_IMPLEMENT_DYNCREATE(OpRedefineNamesAsSel,	SelOperation);
CC_IMPLEMENT_DYNCREATE(OpExportSets,			Operation);
CC_IMPLEMENT_DYNCREATE(OpChangeSetProperty,		UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpChangeBarProperty,		UndoableOperation);
CC_IMPLEMENT_DYNCREATE(OpChangeBarExtends,		UndoableOperation);

// This definition must appear after the above CC_IMPLEMENT_ definitions.
#define new CAM_DEBUG_NEW


/********************************************************************************************
>	virtual void OpSelectSet::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Inputs:		pParam->Param1	---	 the (SGNameItem*) set to change the selection of
				pParam->Param2	---  the SelectScan::Change code (TOGGLE, DESELECT,
									 SELECT or SELECT_EXCLUSIVE).
	Purpose:	Performs the OPTOKEN_SELECT_SET operation.  
	SeeAlso:	SelectScan; Operation
********************************************************************************************/

void OpSelectSet::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	SelectScan((SGNameItem*)(void *) pParam->Param1, (SelectScan::Change) (INT32) pParam->Param2).Scan();
	End();
}



/********************************************************************************************
>	static OpState OpSelectUnionSets::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Returns the UI state of this operation.
********************************************************************************************/

OpState OpSelectUnionSets::GetState(String_256*, OpDescriptor*)
{
	INT32 nTotal;
	NameGallery::Instance()->GetHighlightCount(&nTotal, 0);
	return OpState(FALSE, nTotal < 1);
}



/********************************************************************************************
>	virtual void OpSelectUnionSets::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Performs the OPTOKEN_SELECT_UNION_SETS operation.  
	SeeAlso:	SelectUnionScan; OpSelectSet; OpSelectIntersectSets; Operation
********************************************************************************************/

void OpSelectUnionSets::Do(OpDescriptor*)
{
	SelectUnionScan::Change option = SelectUnionScan::SELECT_EXCLUSIVE;
	if( KeyPress::IsGalleryCtrlPressed() )
		option = SelectUnionScan::DESELECT;
	SelectUnionScan( option ).Scan();
	End();
}



/********************************************************************************************
>	static OpState OpSelectIntersectSets::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Returns the UI state of this operation.
********************************************************************************************/

OpState OpSelectIntersectSets::GetState(String_256*, OpDescriptor*)
{
	INT32 nTotal;
	NameGallery::Instance()->GetHighlightCount(&nTotal, 0);
	return OpState(FALSE, nTotal < 2);
}



/********************************************************************************************
>	virtual void OpSelectIntersectSets::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Performs the OPTOKEN_SELECT_INTERSECT_SETS operation.  
	SeeAlso:	SelectIntersectScan; OpSelect; OpSelectUnionSets; Operation
********************************************************************************************/

void OpSelectIntersectSets::Do(OpDescriptor*)
{
	SelectUnionScan option = SelectUnionScan::SELECT_EXCLUSIVE;
	if( KeyPress::IsGalleryCtrlPressed() )
		option = SelectUnionScan::DESELECT;
	SelectUnionScan( option ).Scan();
	End();
}



/********************************************************************************************
>	static OpState OpApplyNamesToSel::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Returns the UI state of OPTOKEN_APPLY_NAMES_TO_SEL,
				OPTOKEN_REMOVE_NAMES_FROM_SEL and OPTOKEN_REDEFINE_NAMES_AS_SEL.
********************************************************************************************/

OpState OpApplyNamesToSel::GetState(String_256*, OpDescriptor*)
{
	INT32 nTotal, nNames;
	NameGallery::Instance()->GetHighlightCount(&nTotal, &nNames);
	return OpState(FALSE, GetApplication()->FindSelection()->IsEmpty()
					   || nNames < 1 || nTotal > nNames);
}



/********************************************************************************************
>	virtual void OpApplyNamesToSel::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Performs the OPTOKEN_APPLY_NAMES_TO_SEL operation.
********************************************************************************************/

void OpApplyNamesToSel::Do(OpDescriptor*)
{
	AllowOpScan aosSel(this, &theSelectedObjects),
				aosSentinel(this, &theSetSentinel);

	if (!aosSel.Scan() ||
		!aosSentinel.Scan() ||
		!DoStartSelOp(TRUE, TRUE) ||
		!ApplyScan(this, &theSelectedObjects).Scan() ||
		!ApplyScan(this, &theSetSentinel).Scan() ||
		!aosSel.Update() ||
		!aosSentinel.Update())
	{
		FailAndExecute();
	}

	End();
} 



/********************************************************************************************
>	virtual void OpApplyNamesToSel::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Inputs:		pParam->Param1		---		StringBase* to the name to apply to the
											selected objects
	Purpose:	Performs OPTOKEN_APPLY_NAME_TO_SEL for a single parameterised name.
	Returns:	TRUE if successful.
********************************************************************************************/

void OpApplyNamesToSel::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	ERROR3IF(pParam->Param1 == 0, "OpApplyNamesToSel::DoWithParam: null argument");
	const StringBase& strName = *((const StringBase*)(void *) pParam->Param1);

	AllowOpScan aosSel(this, &theSelectedObjects),
				aosSentinel(this, &theSetSentinel);

	if (!aosSel.Scan() ||
		!aosSentinel.Scan() ||
		!DoStartSelOp(TRUE, TRUE) ||
		!ApplySingleScan(this, &theSelectedObjects, strName).Scan() ||
		!ApplySingleScan(this, &theSetSentinel, strName).Scan() ||
		!ApplyPropScan(this, strName).Scan() ||
		!aosSel.Update() ||
		!aosSentinel.Update())
	{
		FailAndExecute();
	}

	End();
}



/********************************************************************************************
>	static OpState OpApplyNamesToOne::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Returns the UI state of OPTOKEN_APPLY_NAMES_TO_ONE,
				OPTOKEN_DELETE_NAMES_FROM_ALL, and OPTOKEN_RENAME_ALL.
********************************************************************************************/

OpState OpApplyNamesToOne::GetState(String_256*, OpDescriptor*)
{
	INT32 nTotal, nNames;
	NameGallery::Instance()->GetHighlightCount(&nTotal, &nNames);
	return OpState(FALSE, nNames < 1 || nTotal > nNames);
}



/********************************************************************************************
>	void OpApplyNamesToOne::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Inputs:		pParam->Param1	---	pointer to the Node to apply to, cast to a INT32
	Purpose:	Performs the OPTOKEN_APPLY_NAMES_TO_ONE operation which applies the
				highlighted names to the given object.  
	SeeAlso:	GalleryNameDragInfo
********************************************************************************************/

void OpApplyNamesToOne::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	ERROR3IF(pParam->Param1 == 0, "OpApplyNamesToOne::DoWithParam: null argument");
	Node* pNode = (Node*)(void *) pParam->Param1;
	
	SingleNodeSource theObject(pNode);
	AllowOpScan aosObj(this, &theObject),
				aosSentinel(this, &theSetSentinel);

	if (!aosObj.Scan() ||
		!aosSentinel.Scan() ||
		!ApplyScan(this, &theObject).Scan() ||
		!ApplyScan(this, &theSetSentinel).Scan() ||
		!aosObj.Update() ||
		!aosSentinel.Update())
	{
		FailAndExecute();
	}

	End();
}



/********************************************************************************************
>	void OpApplyNameToNone::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Inputs:		pParam->Param1		---		StringBase* to the name to apply to the
											selected objects
	Purpose:	Performs the OPTOKEN_APPLY_NAME_TO_NONE operation which applies the
				given name to no objects (ie. just to the NodeSetSentinel object).
	SeeAlso:	NameObjectsDlg::DoCommit
********************************************************************************************/

void OpApplyNameToNone::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	ERROR3IF(pParam->Param1 == 0, "OpApplyNameToNone::DoWithParam: null argument");
	const StringBase& strName = *((const StringBase*)(void *) pParam->Param1);

	AllowOpScan aosSentinel(this, &theSetSentinel);

	if (!aosSentinel.Scan() ||
		!ApplySingleScan(this, &theSetSentinel, strName).Scan() ||
		!ApplyPropScan(this, strName).Scan() ||
		!aosSentinel.Update())
	{
		FailAndExecute();
	}

	End();
}



/********************************************************************************************
>	virtual void OpDeleteNamesFromAll::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Performs the OPTOKEN_DELETE_NAMES_FROM_ALL operation.  Removes all the
				Wix ObjectName attributes that are members of the given Document's
				named sets.
********************************************************************************************/

void OpDeleteNamesFromAll::Do(OpDescriptor*)
{
	AllowOpScan aosDoc(this, &theSelectedDocument);

	if (!aosDoc.Scan() ||
		!HidePropScan(this).Scan() ||
		!HideScan(this, &theSelectedDocument).Scan() ||
		!aosDoc.Update())
	{
		FailAndExecute();
	}

	End();
} 



// renames all of the attribute nodes in the tree (& sentinel) from Str to NewStr (sjk 17/8/00)
void RenameSetInstance(const String_256 & str, const String_256 & NewStr, UndoableOperation *pOp, Spread * pSpread)
{
	// cant convert to the same thing its pointless
	if (str.CompareTo(NewStr) == 0)
		return;

	if (pOp == NULL || pSpread == NULL)
		return;

	SliceHelper::PurgeUseOfSetName(str, pOp, &NewStr);
	Node * pTop = (Node *) pSpread; // search from the spread first
	BOOL again = TRUE;
	Node * pNodeSetSentinel = pOp->GetWorkingDoc()->GetSetSentinel(); // the sentinel 

	// scan the tree to replace these nodes with the new node name
	Node * pNode = SliceHelper::FindNextNameNode(pTop, pTop);
	do
	{
		while (pNode)
		{
			if (str.CompareTo(((TemplateAttribute *)pNode)->GetParam()) == 0)
			{
				// is the new attrib already on this parent?
				// test siblings for the new attrib if it is already there don't add another
				BOOL FoundLikeSibling = FALSE;
				Node * pSib;
				for (	pSib = pNode->FindParent()->FindFirstChild(CC_RUNTIME_CLASS(TemplateAttribute));
						pSib != NULL;
						pSib = pSib->FindNext(CC_RUNTIME_CLASS(TemplateAttribute)) )
						{
							if (NewStr.CompareTo(((TemplateAttribute *)pSib)->GetParam()) == 0)
								FoundLikeSibling = TRUE;
						}

				// add the new attrib where the old one was
				TemplateAttribute* pta = NULL;
				if (!FoundLikeSibling)
				{
					ALLOC_WITH_FAIL(pta, (new TemplateAttribute(TA_NAME, ((TemplateAttribute *)pNode)->GetQuestion(), NewStr)), pOp);
					if (pta)
					{
						pta->AttachNode(pNode, PREV);
						// Create an action to hide the new attribute when we Undo.
						HideNodeAction* pHideAct;
						if (AC_FAIL == HideNodeAction::Init(pOp, pOp->GetUndoActions(),
															pta, TRUE, (Action**) &pHideAct))
						{
							delete pta;
							pta = NULL;
						}				
					}
				}

				Node * pDelMe = pNode;
				pNode = SliceHelper::FindNextNameNode(pNode, pTop);

				// hide this node - the old attrib
				pOp->DoHideNode(pDelMe, FALSE);
			}
			else
				pNode = SliceHelper::FindNextNameNode(pNode, pTop);
		}

		again = pTop != pNodeSetSentinel;
		pTop = pNodeSetSentinel;
		pNode = SliceHelper::FindNextNameNode(pTop, pTop);
	} while (again);



	// Matt 18/12/2000
	// Nearly done renaming - don't forget that we need to hide the old NodeSetProperty !!!
	BOOL propertyrenamed = FALSE;
	NodeSetProperty * pNodeSetProperty = (NodeSetProperty *) pNodeSetSentinel->FindFirstChild(CC_RUNTIME_CLASS(NodeSetProperty));
	while (pNodeSetProperty && !propertyrenamed)
	{
		if ((pNodeSetProperty->GetName() == str) && (!pNodeSetProperty->IsNodeHidden()))
		{
			propertyrenamed = TRUE;
			pOp->DoHideNode(pNodeSetProperty, FALSE);
		}
		pNodeSetProperty = (NodeSetProperty *) pNodeSetProperty->FindNext(CC_RUNTIME_CLASS(NodeSetProperty));
	}
}







/********************************************************************************************
>	virtual void OpRenameAll::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Inputs:		pParam->Param1	 ---	StringBase* to the new name
	Purpose:	Performs the OPTOKEN_RENAME_ALL operation.  
********************************************************************************************/
void OpRenameAll::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	ERROR3IF(pParam->Param1 == 0, "OpRenameAll::DoWithParam: null argument");
	const StringBase& strNewName = *((const StringBase*) (void *) pParam->Param1);
	Spread * pSpread = Document::GetSelectedSpread();
	if (pSpread == NULL)
	{
		FailAndExecute();
		End();
		return;
	}

	// If we are only renaming one item then preserve it's properties, otherwise
	// if we are renaming lots to be the same name then make default properties.
	NodeSetProperty* pCopyPropNode = 0;
	NamedExportProp* pNewExport = 0;
//	Node * pNodeSetSentinel = GetWorkingDoc()->GetSetSentinel(); // the sentinel 

	INT32 nNames;
	NameGallery::Instance()->GetHighlightCount(0, &nNames);
	if (nNames == 1)
	{
		// Find the NodeSetProperty associated with the first highlighted item
		// in the 'Used Names' display group.
		String_256 str;
		NameGallery::Instance()->GetUsedNames()->FindNextSelectedItem()->GetNameText(&str);
		pCopyPropNode = GetWorkingDoc()->GetSetSentinel()->FindPropertyNode(str);
		if (pCopyPropNode != 0)
		{
			// Try to create an export property for the new name now so we can compare
			// it with that of the old name and work out what can be preserved.
			ALLOC_WITH_FAIL(pNewExport,
							(NamedExportProp*) SGNameProp::CreateDefault(strNewName,
																NamedExportProp::nIndex),
							this);
			if (pNewExport == 0)
			{
				FailAndExecute();
				End();
				return;
			}

			// Look up the old properties.
			NamedExportProp* pOldExport =
					(NamedExportProp*) pCopyPropNode->GetProperty(NamedExportProp::nIndex);
			ERROR3IF(pOldExport == 0, "OpRenameAll::DoWithParam: no export property");

			// Preserve the filter, options and location of the old property.
			pNewExport->SetFilter(pOldExport->GetFilter());
			pNewExport->SetOptions(pOldExport->GetOptions());

			String_256 strPath(pOldExport->GetPath().GetLocation(TRUE));
			strPath += pNewExport->GetPath().GetFileName(TRUE);
			pNewExport->SetPath(PathName(strPath));
		}		
	}

	SGNameItem* pNameGalleryItem = (SGNameItem*) NameGallery::Instance()->GetUsedNames()->GetChild();
//	SGNameItem* pRenameMe = NULL;
	while (pNameGalleryItem)
	{
		if (pNameGalleryItem->IsSelected())
		{
			String_256 str;
			pNameGalleryItem->GetNameText(&str);
			// remove any references to str (the old set name)
			// and replace them (undoably of corse) with strNewName (sjk)
			RenameSetInstance(str, *((const String_256 *) (void *) pParam->Param1), this, pSpread);
		}

		pNameGalleryItem = (SGNameItem *) pNameGalleryItem->GetNext();
	}
	
	
	// Now perform the actual renaming.
	AllowOpScan aosDoc(this, &theSelectedDocument);

	BOOL ok = aosDoc.Scan();

	if (ok)
		ok = ApplyPropScan(this, strNewName, pCopyPropNode).Scan();

	if (ok)
		ok = HidePropScan(this).Scan();

//	if (ok)
//		ok = HideScan(this, &theSelectedDocument).Scan();

	// update the locations of everything now as we dont want any extends going on
	NameGallery * pNameGallery = NameGallery::Instance();
	pNameGallery->FastUpdateNamedSetSizes(); // make sure we are using the most up-to-date data

	if (ok)
		ok = aosDoc.Update();

	if (!ok)
	{
		FailAndExecute();
		End();
		return;
	}

	// If we preserved an export property then reset it to the default for its new name.
	if (pNewExport != 0)
	{
		// Change to the new property (without undo) and deallocate the old.
		pCopyPropNode = GetWorkingDoc()->GetSetSentinel()->FindPropertyNode(strNewName);
		if (pCopyPropNode != 0) delete pCopyPropNode->SetProperty(pNewExport);
	}
	
	End();
}



/********************************************************************************************
>	virtual void OpRemoveNamesFromSel::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Inputs:		pOpDesc		---		See base class
	Purpose:	Performs the OPTOKEN_REMOVE_NAMES_FROM_SEL operation.
********************************************************************************************/

void OpRemoveNamesFromSel::Do(OpDescriptor*)
{
	AllowOpScan aosSel(this, &theSelectedObjects);

	if (!aosSel.Scan() ||
		!DoStartSelOp(TRUE, TRUE) ||
		!HideScan(this, &theSelectedObjects).Scan() ||
		!aosSel.Update())
	{
		FailAndExecute();
	}

	End();
} 



/********************************************************************************************
>	virtual void OpRedefineNamesAsSel::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Performs the OpRedefineNamesAsSel operation.  Hides all the Wix
				ObjectName attributes that are members of the given Document's
				given attribute set and applies them only to the selected objects.
	Inputs:		pOpDesc		---		See base class
	SeeAlso:	class OpRedefineNamesAsSel::Param; OpApplyAttribToSelected::DoWithParam;
				OpRedefineNamesAsSel::DoApply
********************************************************************************************/

void OpRedefineNamesAsSel::Do(OpDescriptor*)
{
	AllowOpScan aosSel(this, &theSelectedObjects),
				aosUnsel(this, &theUnselectedObjects),
				aosSentinel(this, &theSetSentinel);

	if (!aosSel.Scan() ||
		!aosUnsel.Scan() ||
		!aosSentinel.Scan() ||
		!DoStartSelOp(TRUE, TRUE) ||
		!HideScan(this, &theSelectedDocument).Scan() ||
		!ApplyScan(this, &theSelectedObjects).Scan() ||
		!ApplyScan(this, &theSetSentinel).Scan() ||
		!aosSel.Update() ||
		!aosUnsel.Update() ||
		!aosSentinel.Update())
	{
		FailAndExecute();
	}

	End();
}	



/********************************************************************************************
>	static OpState OpExportSets::GetState(String_256*, OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Returns the UI state of this operation.
********************************************************************************************/

OpState OpExportSets::GetState(String_256*, OpDescriptor*)
{
	INT32 nTotal, nNames;
	NameGallery::Instance()->GetHighlightCount(&nTotal, &nNames);
	return OpState(FALSE, nNames < 1);
}



/********************************************************************************************
>	virtual void OpExportSets::Do(OpDescriptor*)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Performs the OPTOKEN_EXPORT_SETS operation.  
	SeeAlso:	OpSelectIntersectSets; Operation
********************************************************************************************/

void OpExportSets::Do(OpDescriptor*)
{
	// Iterate over all highlighted set items in the gallery, re-exporting the
	// export targets.
	if (ExportHighlightedIter(this).ForEach() != 0) InformError();
	End();
}



/***********************************************************************************************
>   OpExportSets::ExportHighlightedIter::ExportHighlightedIter(OpExportSets* pOp)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:		pOp		---		required context for the exporting.
	Purpose:    Constructs an ExportHighlightedIter.
	SeeAlso:	OpExportSets; NameItemIter
***********************************************************************************************/

OpExportSets::ExportHighlightedIter::ExportHighlightedIter(OpExportSets* pOp)
  :	NameItemIter(NameItemIter::HIGHLIGHTED_NAMES),
	m_pOp(pOp)
{
	// Save the selection.
	OpMenuExport::SaveSelection();
}



/***********************************************************************************************
>   virtual OpExportSets::ExportHighlightedIter::~ExportHighlightedIter()

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Destroys an ExportHighlightedIter.
***********************************************************************************************/

OpExportSets::ExportHighlightedIter::~ExportHighlightedIter()
{
	// Restore the selection.
	OpMenuExport::RestoreSelection();

	// Warn about any unexportable names.
	if (!m_strExtErrors.IsEmpty())
	{
		// Some implied extensions weren't recognised so warn about them.
		String_256 str;
		m_strExtErrors.toUpper();
		str.MakeMsg(_R(IDS_NAMEGAL_EXT_ERROR), &m_strExtErrors);
		Error::SetError(0, str, 0);
		InformWarning();
	}
}



/***********************************************************************************************
>   virtual BOOL OpExportSets::ExportHighlightedIter::Do(SGNameItem* pItem)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Purpose:    Called for each SGNameItem that is highlighted in the Name Gallery.
***********************************************************************************************/

BOOL OpExportSets::ExportHighlightedIter::Do(SGNameItem* pItem)
{
	// Skip if the set contains no objects.
	if (pItem->IsEmpty()) return TRUE;
	
	// Skip if the export property is not (yet) set up.
	NamedExportProp* pProp = (NamedExportProp*) pItem->GetProperty(NamedExportProp::nIndex);
	ERROR3IF(pProp == 0, "ExportHighlightedIter::Do: no export property");

	// if the user has not set the options themselves yet prompt them for a location
	if (pProp->IsVirgin())
	{
		if ( !pProp->ExportPropertiesToLocationDlg(pItem) )
			return FALSE;

		// pItem may have been altered in the above call
		// and we need to refer to the new state
		pProp = (NamedExportProp*) pItem->GetProperty(NamedExportProp::nIndex);
	}

	if (pProp->GetFilter() == 0 || !pProp->GetPath().IsValid())
	{
		// Build up a warning message over the course of the iteration.
		String_256 str;
		pItem->GetNameText(&str);
		PathName pth(str);

		if (!m_strExtErrors.IsEmpty()) m_strExtErrors += String(_R(IDS_NAMEGAL_EXT_COMMA));
		m_strExtErrors += pth.GetType();
		return TRUE;
	}

	// Clear out the selection and just select the given set.
	if (!SelectScan(pItem, SelectScan::SELECT_EXCLUSIVE, TRUE).Scan()) return FALSE;

	// Fix-up the bevel gubbins etc.
	OpMenuExport::NormaliseSelection();

	// Try to open a file.
	CCDiskFile theFile;
	if (!theFile.open(pProp->GetPath().GetPath(), ios::in | ios::out | ios::binary))
		return FALSE;

	// TODO: express-export the selection using the stored options.
	BOOL fOk = FALSE;
	if (pProp->GetFilter()->IS_KIND_OF(BaseBitmapFilter))
	{
		// never assume that the colours are the same so invalidate the palette
		// or indeed the thing may have changed size so flush the lot of cached data
		pProp->GetOptions()->BitmapSourceHasChanged();
		// do the export dance
		fOk = ((BaseBitmapFilter*) pProp->GetFilter())->DoExportWithOptions(
											m_pOp,
											&theFile,
											&pProp->GetPath(),
											m_pOp->GetWorkingDoc(),
											pProp->GetOptions());
	}
	
	// Tidy up and continue iterating if successful.
	theFile.close();
	return fOk;
}



/***********************************************************************************************
>   virtual void OpChangeSetProperty::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     pParam->Param1		---		the name of the set the property appertains to
				pParam->Param2		---		the SGNameProp* to set in the tree
	Purpose:    Runs the OPTOKEN_CHANGE_SET_PROPERTY operation.
	SeeAlso:	SGNameProp::Change
***********************************************************************************************/

void OpChangeSetProperty::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	// Extract the parameters.
	ERROR3IF(pParam == 0 || pParam->Param1 == 0, "OpChangeSetProperty::DoWithParam: no input");
	const StringBase& strName = *(((StringBase**) (void *) pParam->Param1)[0]);
	StringBase* pstrNewName = ((StringBase**) (void *) pParam->Param1)[1];
	SGNameProp* pProp = (SGNameProp*) (void *) pParam->Param2;

	// Find the equivalent old property, if any, and hide it.
	NodeSetSentinel* pSentry = GetWorkingDoc()->GetSetSentinel();
	NodeSetProperty* pOldSetNode = pSentry->FindPropertyNode(strName);
	ERROR3IF(pOldSetNode == 0, "OpChangeSetProperty::DoWithParam: can't find NodeSetProperty");
	if (pOldSetNode != 0 && !DoHideNode(pOldSetNode, TRUE, 0, TRUE))
	{
		FailAndExecute();
		End();
		return;
	}

	// Try to create a duplicate property holder node with the given (re)name.
	NodeSetProperty* pNewSetNode;
	ALLOC_WITH_FAIL(pNewSetNode,
					(new NodeSetProperty(pstrNewName != 0 ? *pstrNewName : strName)),
					this);
	if (pNewSetNode == 0)
	{
		FailAndExecute();
		End();
		return;
	}

	// Try to copy the old properties into it.
	NodeSetProperty* fOk=NULL; // we use this as a boolean because ALLOC_WITH_FAIL expects an allocation routine and thus pointers
	// so we return a dummy pointer (pNewSetNode) if we succeed as a flag
	ALLOC_WITH_FAIL(fOk, ((pNewSetNode->CopyProperties(pOldSetNode))?pNewSetNode:NULL), this);
	if (!fOk)
	{
		FailAndExecute();
		End();
		return;
	}

	// Set the new property within the duplicate.
	SGNameProp* pOldProp = pNewSetNode->SetProperty(pProp);
	if (pOldProp != 0) delete pOldProp;

	// Mark the new property as no longer "virgin".
	pProp->SetDirty();

	// Undoably insert the duplicate into the tree.
	ERROR3IF(pSentry->FindPropertyNode(pNewSetNode->GetName()) != 0,
				"OpChangeSetProperty::DoWithParam: property node already exists");
	pNewSetNode->AttachNode(pSentry, LASTCHILD);
	HideNodeAction* pHideAct;
	if (AC_FAIL == HideNodeAction::Init(this, GetUndoActions(), pNewSetNode,
										TRUE, (Action**) &pHideAct))
	{
		delete pNewSetNode;
		FailAndExecute();
		End();
		return;
	}

	// Finally, if requested to then rename the strName set to be *pstrNewName.
	// Changing the type of an export property, for example, can mean renaming.
	if (pstrNewName != 0)
	{
		RenameSetInstance(strName, *pstrNewName, this, Document::GetSelectedSpread());
	}

	End();
}



/***********************************************************************************************
>   virtual void OpChangeBarProperty::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     pParam->Param2		---		pointer to the OpChangeBarPropParam
	Purpose:    Runs the OPTOKEN_CHANGE_BAR_PROPERTY operation.
	SeeAlso:	NodeBarProperty::MakeChange; OpChangeBarPropParam
***********************************************************************************************/

void OpChangeBarProperty::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	// Extract the parameter.
	ERROR3IF(pParam == 0, "OpChangeBarProperty::DoWithParam: no parameter");
	OpChangeBarPropParam* pInput = (OpChangeBarPropParam*) (pParam);
	NodeBarProperty* pProp = pInput->m_Prop; //(NodeBarProperty*) (pParam->Param1);
	ERROR3IF(pInput == 0 || pProp == 0, "OpChangeBarProperty::DoWithParam: no property");

	// Set this to a special value to indicate it's as yet unused.
	m_BarIndex = -1;
	m_MakingLive = FALSE; // is the live stretch being turned back on?

	// Try to duplicate the old bar properties.
	if (!DoChangeBarProperty(this, pInput->m_nIndex, pProp, pInput->m_pbdtInfo, &m_MakingLive))
	{
		FailAndExecute();
		End();
		return;
	}

	// does this to record what the op was that triggered the extending stuff
	// most ops don't do it like this. Instead they go through AllowOp
	// but his doesn't need to go through AllowOp and doesn't call that
	// so it calls this instead. sjk
	NameGallery* pNameGallery = NameGallery::Instance();
	if (pNameGallery != 0)
	{
		// set up for a bar wiggling
		pNameGallery->PreTriggerEdit(this, 0, (Node*) 0);

		// make sure this bar does know it has been touched
		pNameGallery->m_TouchedBar = pInput->m_nIndex;
		m_BarIndex = pInput->m_nIndex;
	}

	// Success.
	End();
}

BOOL OpChangeBarProperty::DoChangeBarProperty(UndoableOperation * pOp, INT32 Index, NodeBarProperty* pProp, const BarDataType * pBarData, BOOL * pMadeLive)
{
	if (pMadeLive)
		*pMadeLive = FALSE;

	if (!pOp || !pProp || !pBarData)
		return FALSE;

	// Try to duplicate the old bar properties.
	NodeBarProperty* pCopy;
	ALLOC_WITH_FAIL(pCopy, ((NodeBarProperty*) pProp->SimpleCopy()), pOp);
	if (pCopy == 0)
	{
		return FALSE;
	}

	// Try to attach the duplicate bar properties in the right place.
	pCopy->AttachNode(pOp->GetWorkingDoc()->GetSetSentinel(), LASTCHILD);
	HideNodeAction* pHideAct;
	if (AC_FAIL == HideNodeAction::Init(pOp, pOp->GetUndoActions(), pCopy,
										TRUE, (Action**) &pHideAct))
	{
		delete pCopy;
		return FALSE;
	}

	// Write the new data into the duplicate.
	if (Index >= (INT32)pCopy->HowMany())
		pCopy->Add(*pBarData);
	else
	{
		BOOL MakingLive = !pCopy->Bar(Index).IsLive && pBarData->IsLive;
		BarDataType* pCopyData = &pCopy->Bar(Index);
		*pCopyData = *pBarData;
		if (pMadeLive)
			*pMadeLive = MakingLive;
	}

	// Try to hide the old bar properties.
	if (!pOp->DoHideNode(pProp, TRUE, 0, TRUE))
	{
		return FALSE;
	}
	
	return TRUE;
}



/***********************************************************************************************
>   virtual void OpChangeBarExtends::DoWithParam(OpDescriptor*, OpParam* pParam)

	Author:     Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:    27/8/99
	Inputs:     pParam->Param1		---		(INT32) ID of the bars to change
				pParam->Param2		---		(BYTE) the new stretching mode to set
	Purpose:    Runs the OPTOKEN_CHANGE_BAR_EXTENDS operation.
	SeeAlso:	NodeBarProperty::MakeChange
***********************************************************************************************/

void OpChangeBarExtends::DoWithParam(OpDescriptor*, OpParam* pParam)
{
	// Extract the parameters.
	ERROR3IF(pParam == 0, "OpChangeBarExtends::DoWithParam: no parameter");
	INT32 idBars = (INT32) pParam->Param1;
	BYTE nMode = (BYTE) pParam->Param2;
	SGUsedNames* pNames = NameGallery::Instance()->GetUsedNames();
	ERROR3IF(pNames == 0, "OpChangeBarExtends::DoWithParam: no set names");
	NodeSetSentinel* pSentry = GetWorkingDoc()->GetSetSentinel();

	// Iterate over all SGUsedNames, looking for matching bar IDs.
	for (SGNameItem* pItem = (SGNameItem*) pNames->GetChild();
		 pItem != 0;
		 pItem = (SGNameItem*) pItem->GetNext())
			if (pItem->m_BarNumber == idBars)
			{
				// We've found a bar whose ID matches.  Undoably change its extending
				// mode to that passed in.
				String_256 strName;
				pItem->GetNameText(&strName);
				NodeSetProperty* pOldSetNode = pSentry->FindPropertyNode(strName);
				ERROR3IF(pOldSetNode == 0,
							"OpChangeBarExtends::DoWithParam: can't find NodeSetProperty");

				// Try to duplicate the old set properties.
				NodeSetProperty* pNewSetNode;
				ALLOC_WITH_FAIL(pNewSetNode,
								((NodeSetProperty*) pOldSetNode->SimpleCopy()),
								this);
				if (pNewSetNode == 0)
				{
					FailAndExecute();
					End();
					return;
				}

				// Write the stretching flag into the duplicate.
				NamedStretchProp* pProp =
						(NamedStretchProp*) pNewSetNode->GetProperty(NamedStretchProp::nIndex);
				ERROR3IF(pProp == 0, "OpChangeBarExtends::DoWithParam: no property");
				pProp->SetStretchType(nMode);

				// Mark the duplicate property as no longer "virgin".
				pProp->SetDirty();

				// Insert the new properties in their place.
				pNewSetNode->AttachNode(pSentry, LASTCHILD);
				HideNodeAction* pHideAct;
				if (AC_FAIL == HideNodeAction::Init(this, GetUndoActions(), pNewSetNode,
													TRUE, (Action**) &pHideAct))
				{
					delete pNewSetNode;
					FailAndExecute();
					End();
					return;
				}

				//  Finally, hide the old properties.
				if (!DoHideNode(pOldSetNode, TRUE, 0, TRUE))
				{
					FailAndExecute();
					End();
					return;
				}
			}

	// Success.
	End();
}



/********************************************************************************************
>	BOOL InitNamingSystem()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/7/99
	Purpose:	Initialises the various operations and other components of the
				Attribute gallery with the exception of the gallery and its embedded
				gagdets, which are initialised with the other galleries in sginit.cpp
	Returns:	TRUE if successful.
********************************************************************************************/

BOOL InitNamingSystem()
{
/*
	RegisterOpDescriptor's arguments:-

	toolID 								: Tool ID (from which the Module ID can be found)
	txID  								: String Resource ID
	RuntimeClass						: The operation's runtime class
	tok,								: pointer to the token string
	gs,	 								: pointer to the GetState function
	helpId = 0							: help identifier 
	bubbleID = 0						: string resource for bubble help
	resourceID = 0						: resource ID
	controlID = 0						: control ID within that resource within that tool
	GroupBarID = SYSTEMBAR_ILLEGAL		: The system bar group this op desc belongs to
	ReceiveMessages	= FALSE *!!*		: Does this respond to system messages?
										: NOTE ** MUST be TRUE for ops connected to buttons?
	Smart = FALSE						: A smart duplicate operation ?
	Clean = TRUE  *!!*					: Does the operation change the document
	pVertOpDesc	  *!!*					: this op desc's vertical alter ego, if any
	OneOpenInstID = 0					: When not 0 only one live instance
				 						  of the operation is permitted, and
										  OneOpenInstID is the string resource
										  ID describing why.This will be useful
				 						  for 1 open instance dialogs.
	AutoStateFlags = 0					: Flags which indicate conditions when
				 						  the operation should automatically be
										  greyed/ticked, GREY_WHEN_NO_CURRENT_DOC and
										  GREY_WHEN_NO_SELECTION, DONT_GREY_WHEN_SELECT_INSIDE
										  This cuts down the number calls to GetState

	NB. The Clean argument defaults to FALSE for UndoableOperation::RegisterOpDescriptor.
	Also sote that UndoableOperation::RegisterOpdescriptor doesn't take the pVertOpDesc
	argument.  Oh sweet Camelot!
*/
	return OpSelectSet::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_SELECT_SET),
				CC_RUNTIME_CLASS(OpSelectSet),
				OPTOKEN_SELECT_SET,
				0,
 				0,
 				_R(IDBBL_NAMEOP_SELECT_SET),
				0,
				0,
				SYSTEMBAR_ILLEGAL,
				FALSE,
				FALSE,
				TRUE,
				0,
				0,
				GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)

		&& OpSelectUnionSets::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_SELECT_UNION_SETS),
				CC_RUNTIME_CLASS(OpSelectUnionSets),
				OPTOKEN_SELECT_UNION_SETS,
				OpSelectUnionSets::GetState,
 				0,
 				_R(IDBBL_NAMEOP_SELECT_UNION_SETS))
		
		&& OpSelectIntersectSets::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_SELECT_INTERSECT_SETS),
				CC_RUNTIME_CLASS(OpSelectIntersectSets),
				OPTOKEN_SELECT_INTERSECT_SETS,
				OpSelectIntersectSets::GetState,
 				0,
 				_R(IDBBL_NAMEOP_SELECT_INTERSECT_SETS))
		
		&& OpApplyNamesToSel::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_APPLY_NAMES_TO_SEL),
				CC_RUNTIME_CLASS(OpApplyNamesToSel),
				OPTOKEN_APPLY_NAMES_TO_SEL,
				OpApplyNamesToSel::GetState,
 				0,
 				_R(IDBBL_NAMEOP_APPLY_NAMES_TO_SEL))
		
		&& OpApplyNamesToSel::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_APPLY_NAME_TO_SEL),
				CC_RUNTIME_CLASS(OpApplyNamesToSel),
				OPTOKEN_APPLY_NAME_TO_SEL,
				0,
 				_R(IDBBL_NAMEOP_APPLY_NAME_TO_SEL),
 				0,
 				0,
 				0,
				SYSTEMBAR_ILLEGAL,
				FALSE,
				FALSE,
				FALSE,
				0,
				GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE)

		&& OpApplyNamesToOne::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_APPLY_NAMES_TO_ONE),
				CC_RUNTIME_CLASS(OpApplyNamesToOne),
				OPTOKEN_APPLY_NAMES_TO_ONE,
				OpApplyNamesToOne::GetState,
 				0,
 				_R(IDBBL_NAMEOP_APPLY_NAMES_TO_ONE))

		&& OpApplyNameToNone::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_APPLY_NAME_TO_NONE),
				CC_RUNTIME_CLASS(OpApplyNameToNone),
				OPTOKEN_APPLY_NAME_TO_NONE,
				0,
 				_R(IDBBL_NAMEOP_APPLY_NAME_TO_NONE),
 				0,
 				0,
 				0,
				SYSTEMBAR_ILLEGAL,
				FALSE,
				FALSE,
				FALSE,
				0,
				GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
		
		&& OpDeleteNamesFromAll::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_DELETE_NAMES_FROM_ALL),
				CC_RUNTIME_CLASS(OpDeleteNamesFromAll),
				OPTOKEN_DELETE_NAMES_FROM_ALL,
				OpApplyNamesToOne::GetState,
 				0,
 				_R(IDBBL_NAMEOP_DELETE_NAMES_FROM_ALL))

		&& OpRenameAll::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_RENAME_ALL),
				CC_RUNTIME_CLASS(OpRenameAll),
				OPTOKEN_RENAME_ALL,
				OpApplyNamesToOne::GetState,
 				0,
 				_R(IDBBL_NAMEOP_RENAME_ALL))

		&& OpRemoveNamesFromSel::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_REMOVE_NAMES_FROM_SEL),
				CC_RUNTIME_CLASS(OpRemoveNamesFromSel),
				OPTOKEN_REMOVE_NAMES_FROM_SEL,
				OpApplyNamesToSel::GetState,
 				0,
 				_R(IDBBL_NAMEOP_REMOVE_NAMES_FROM_SEL))

		&& OpRedefineNamesAsSel::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_REDEFINE_NAMES_AS_SEL),
				CC_RUNTIME_CLASS(OpRedefineNamesAsSel),
				OPTOKEN_REDEFINE_NAMES_AS_SEL,
				OpApplyNamesToSel::GetState,
 				0,
 				_R(IDBBL_NAMEOP_REDEFINE_NAMES_AS_SEL))
		
		&& OpExportSets::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_EXPORT_SETS),
				CC_RUNTIME_CLASS(OpExportSets),
				OPTOKEN_EXPORT_SETS,
				OpExportSets::GetState,
 				0,
 				_R(IDBBL_NAMEOP_EXPORT_SETS))

		&& OpChangeSetProperty::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_CHANGE_SET_PROPERTY),
				CC_RUNTIME_CLASS(OpChangeSetProperty),
				OPTOKEN_CHANGE_SET_PROPERTY,
				0,
 				_R(IDBBL_NAMEOP_CHANGE_SET_PROPERTY),
 				0,
 				0,
 				0,
				SYSTEMBAR_ILLEGAL,
				FALSE,
				FALSE,
				FALSE,
				0,
				GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
		
		&& OpChangeBarProperty::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_CHANGE_BAR_PROPERTY),
				CC_RUNTIME_CLASS(OpChangeBarProperty),
				OPTOKEN_CHANGE_BAR_PROPERTY,
				0,
 				_R(IDBBL_NAMEOP_CHANGE_BAR_PROPERTY),
 				0,
 				0,
 				0,
				SYSTEMBAR_ILLEGAL,
				FALSE,
				FALSE,
				FALSE,
				0,
				GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
		
		&& OpChangeBarExtends::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_CHANGE_BAR_EXTENDS),
				CC_RUNTIME_CLASS(OpChangeBarExtends),
				OPTOKEN_CHANGE_BAR_EXTENDS,
				0,
 				_R(IDBBL_NAMEOP_CHANGE_BAR_EXTENDS),
 				0,
 				0,
 				0,
				SYSTEMBAR_ILLEGAL,
				FALSE,
				FALSE,
				FALSE,
				0,
				GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
		
		&& NameObjectsDlg::Init()

		&& NameObjectsDlg::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_NAME_OBJECTS_DLG),
				CC_RUNTIME_CLASS(NameObjectsDlg),
				OPTOKEN_NAME_OBJECTS_DLG,
				0,
				0,
				_R(IDBBL_NAMEOP_NAME_OBJECTS_DLG),
				0,
				0,
				SYSTEMBAR_ILLEGAL,
				TRUE,					// send kernel messages
				FALSE,
				TRUE,
				0,
				_R(IDS_NAMEDLG_ONE_INSTANCE),
				GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)

		&& RenameObjectsDlg::RegisterOpDescriptor(
				0,
				_R(IDS_NAMEOP_RENAME_OBJECTS_DLG),
				CC_RUNTIME_CLASS(RenameObjectsDlg),
				OPTOKEN_RENAME_OBJECTS_DLG,
				OpApplyNamesToOne::GetState,
				0,
				_R(IDBBL_NAMEOP_RENAME_OBJECTS_DLG));
}
