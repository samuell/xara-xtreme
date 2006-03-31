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
// Implements the combine shape ops, like add, subtract, intersect and slice

/*
*/

#include "camtypes.h"
#include "combshps.h"
//#include "markn.h"
//#include "resource.h"
#include "app.h"
#include "nodepath.h"
#include "gclip.h"
#include "gclips.h"
#include "nodeattr.h"
#include "progress.h"
#include "grndrgn.h"
//#include "combbuts.h"
#include "gdraw.h"
#include "docview.h"
#include "document.h"
#include "group.h"
#include "attrmap.h"
#include "opbevel.h"
#include "nodecomp.h"
#include "nodetxts.h"
#include "brshattr.h"
#include "strkattr.h"
#include "lineattr.h"
#include "ppstroke.h"
#include "ophist.h"
//#include "opliveeffects.h"

DECLARE_SOURCE("$Revision$");


CC_IMPLEMENT_DYNCREATE(OpCombineShapes,		SelOperation)
CC_IMPLEMENT_DYNCREATE(OpAddShapes,			OpCombineShapes)
CC_IMPLEMENT_DYNCREATE(OpSubtractShapes,	OpCombineShapes)
CC_IMPLEMENT_DYNCREATE(OpIntersectShapes,	OpCombineShapes)
CC_IMPLEMENT_DYNCREATE(OpSliceShapes,		OpCombineShapes)
CC_IMPLEMENT_DYNCREATE(SelObjPathList,		ListItem)
CC_IMPLEMENT_DYNCREATE(SelObjPathListItem,	ListItem)

CC_IMPLEMENT_MEMDUMP(CombineBecomeA,		BecomeA);

#define new CAM_DEBUG_NEW

static INT32	DefaultTolerance  		= 1;
static INT32	DefaultFlatness			= 100;
static INT32	DefaultStrokeWidth		= 100;
static INT32	AddPathGranularity		= 1024;
static BOOL	CombineEffectBitmaps	= FALSE;


/********************************************************************************************

>	OpCombineShapes::OpCombineShapes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpCombineShapes::OpCombineShapes()
{
	Reason 		= COMBINE_NONE;
	pSelRange 	= NULL;
	NumPaths	= 0;
	JobCount	= 0;

	pContextNode = NULL;
	pListOfResults = NULL;
}

/********************************************************************************************

>	OpCombineShapes::~OpCombineShapes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor - does nothing
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpCombineShapes::~OpCombineShapes()
{
	ListOfSelObjPathLists.DeleteAll();
	if (pListOfResults)
	{
		delete pListOfResults;
		pListOfResults = NULL;
	};
}

/********************************************************************************************

>	static BOOL OpCombineShapes::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Initialises all the ops that implement the combine shape ops:
					OpAddShapes
					OpSubtractShapes
					OpIntersectShapes
					OpSliceShapes
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

// This macro gives a short-hand way of registering all the combine opdescriptors
// It may have to be updated if we every want to put combine ops on buttons

#define REG_COMBINE_OP(NAME,Name)\
	{\
		BOOL Blobby = RegisterOpDescriptor(\
						0,\
						_R(IDS_COMBINE_ ## NAME ), /*NORESOURCEFIX*/ \
						CC_RUNTIME_CLASS(Op ## Name ## Shapes),\
						OPTOKEN_ ## NAME ## SHAPES,\
						OpCombineShapes::GetState,\
						0,\
						_R(IDBBL_COMBINE_ ## NAME), /*NORESOURCEFIX*/ \
						_R(IDD_COMBINEBUTTONS),\
						_R(IDC_BTN_COMBINE ## NAME),  /*NORESOURCEFIX*/ \
						SYSTEMBAR_ARRANGE,\
						TRUE,\
						FALSE,\
						FALSE,\
 					  	0,\
 					  	GREY_WHEN_NO_SELECTION | DONT_GREY_WHEN_SELECT_INSIDE\
 					  );\
		ERROR1IF(!Blobby, FALSE, _R(IDS_OUT_OF_MEMORY));\
 	}\

BOOL OpCombineShapes::Init()
{
	REG_COMBINE_OP(ADD,			Add);
	REG_COMBINE_OP(SUBTRACT,	Subtract);
	REG_COMBINE_OP(INTERSECT,	Intersect);
	REG_COMBINE_OP(SLICE,		Slice);

	if (Camelot.DeclareSection(_T("CombineShapes"),5))
	{
		Camelot.DeclarePref(_T("CombineShapes"), _T("DefaultTolerance"), 	&DefaultTolerance,	1,1000);
		Camelot.DeclarePref(_T("CombineShapes"), _T("DefaultFlatness"), 	&DefaultFlatness,	10,2000);
		Camelot.DeclarePref(_T("CombineShapes"), _T("DefaultStrokeWidth"), 	&DefaultStrokeWidth,10,72000);
		Camelot.DeclarePref(_T("CombineShapes"), _T("AddPathGranularity"), 	&AddPathGranularity,1,1024*1024);
		Camelot.DeclarePref(_T("CombineShapes"), _T("CombineEffectsBitmaps"), &CombineEffectBitmaps, FALSE, TRUE);
	}

	return TRUE;
}

/********************************************************************************************

>	static OpState OpCombineShapes::GetState(String_256* pStr, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		pStr = string to place messages in
	Outputs:	-
	Returns:	The op state, i.e. the ticked and greyed state of the op
	Purpose:	Gives the op an oppertunity to say whether the op is executable or not.

				A combine op is only available if there are more than one selected nodes that are
				or can become paths.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState OpCombineShapes::GetState(String_256* pStr, OpDescriptor* pOpDesc)
{
	OpState OpSt;
	UINT32 Count = 0;
	UINT32 MinCount = 2;

	String Str(OPTOKEN_ADDSHAPES);
	if (pOpDesc->Token == Str)
		MinCount = 1;

	// Get the selected range
	SelRange* pSelRange = GetApplication()->FindSelection();
	BOOL bRestorePromote = pSelRange->SetPromoteToParent(TRUE);

	BecomeA BecomePath(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath));
	BecomePath.SetInsertComplexBlendStepsAsPaths (TRUE);		// compound nodes MUST be inserted as paths!

	// Count the number of selected nodes that are or can become NodePaths
	if (pSelRange != NULL)
	{
		Node* pNode = pSelRange->FindFirst();

		while (pNode != NULL)
		{
			if (pNode->CanBecomeA(&BecomePath))
				Count++;

			pNode = pSelRange->FindNext(pNode);
		}
	}

	pSelRange->SetPromoteToParent(bRestorePromote);

	// if at least 'MinCount' objects of the selection can be combined, ungrey the op
	if (Count >= MinCount)
		OpSt.Greyed = FALSE;
	else
	{
		// The op can't be done, so put an explanation in the string provided
		OpSt.Greyed = TRUE;

		String_256 Str(_R(IDS_COMBINE_SEL_ERROR));
		*pStr = Str;
	}

	return OpSt;
}

/********************************************************************************************

>	virtual void OpCombineShapes::GetOpName(String_256* OpName) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		OpName = ptr to str to place op name in
	Outputs:	The undo string for the operation
	Returns:	
	Purpose:	The GetOpName fn is overridden so that we return back a description 
				appropriate to the type of attribute that the operation applies. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpCombineShapes::GetOpName(String_256* OpName) 
{ 
	UINT32 IDS = 0;
	switch (Reason)
	{
		case COMBINE_ADD: 		IDS = _R(IDBBL_COMBINE_ADD); 		break;
		case COMBINE_SUBTRACT:	IDS = _R(IDBBL_COMBINE_SUBTRACT); 	break;
		case COMBINE_INTERSECT:	IDS = _R(IDBBL_COMBINE_INTERSECT);	break;
		case COMBINE_SLICE:		IDS = _R(IDBBL_COMBINE_SLICE); 		break;

		default:
			ERROR3_PF(("Unknown combine reason : %d",Reason));
			break;
	}

	if (IDS > 0)
		*OpName = String_256(IDS);
}  

/********************************************************************************************

>	BOOL OpCombineShapes::BeginSlowJob()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK
	Purpose:	Wrapper around ::BeginSlowJob() that sets up the hour glass counter correctly
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::BeginSlowJob()
{
	NumPaths = 0;
	JobCount = 0;
	SelObjPathList* pList = GetFirstList();
	while (pList != NULL)
	{
		NumPaths += pList->GetCount();
		pList = GetNextList(pList);
	}	

	INT32 SlowJobCount = NumPaths;
	if (Reason == COMBINE_SLICE)
		SlowJobCount *= 2;
	String_64 Str(_R(IDS_COMBINE_SLOWJOB));
	return (::BeginSlowJob(SlowJobCount,FALSE,&Str));
}
	

/********************************************************************************************

>	virtual void OpCombineShapes::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	This is it! The routine that gets all the hard work underway.

				It acts upon the value in member var 'Reason', performing the shape-combining op
				specified by that value
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpCombineShapes::Do(OpDescriptor*)
{
	BeginSlowJob();
	DoStartSelOp(TRUE, FALSE);
	
	BOOL ok = TRUE;
	NodeInsertCount = 0;
	pListOfResults = new ListRange();
	BOOL bCombineEffectBitmaps = CombineEffectBitmaps;

	// If shift key is pressed toggle the effect handling method
	ClickModifiers ClickMods = ClickModifiers::GetClickModifiers();
	if (ClickMods.Adjust)
		bCombineEffectBitmaps = !bCombineEffectBitmaps;

	// ---------------------------------------------------------
	// Phil's new version: Scan the selection and call DoBecomeA once only!
	//
	// Convert all shapes in the selection in-place into paths.
	// NOTE! this allows all compound nodes to become groups in place
	// (which doesn't happen with the passback method). This is important
	// for retaining the tree structure that the paths are found in so that
	// paths remain in place inside groups and effects subtrees.
	//
	pSelRange = GetApplication()->FindSelection();
	Range OldRange(*pSelRange);
	RangeControl rg = OldRange.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	OldRange.SetRangeControl(rg);

	::BeginSlowJob();

	switch (Reason)
	{
		// act upon the reason
		case COMBINE_ADD:
			ok = ConvertToShapes(&OldRange, bCombineEffectBitmaps);
			if (ok) ok = DoAddShapes();
			if (ok && NodeInsertCount==0) {ok=FALSE; InformWarning(_R(IDS_COMBINE_NORESULT));}
			if (ok) ok = DoHideListedNodes(bCombineEffectBitmaps);
//			if (ok) ok = DoSelectResultNodes(bCombineEffectBitmaps);
			break;

		case COMBINE_SUBTRACT:
			ok = ConvertToShapes(&OldRange, bCombineEffectBitmaps);
			if (ok) ok = DoCombineShapes(CLIP_STYLE_SUBTRACT, bCombineEffectBitmaps);
			if (ok && NodeInsertCount==0) {ok=FALSE; InformWarning(_R(IDS_COMBINE_NORESULT));}
			if (ok) ok = DoHideListedNodes(bCombineEffectBitmaps);
//			if (ok) ok = DoSelectResultNodes(bCombineEffectBitmaps);
			break;

		case COMBINE_INTERSECT:
			ok = ConvertToShapes(&OldRange, bCombineEffectBitmaps);
			if (ok) ok = DoCombineShapes(CLIP_STYLE_INTERSECT, bCombineEffectBitmaps);
			if (ok && NodeInsertCount==0) {ok=FALSE; InformWarning(_R(IDS_COMBINE_NORESULT));}
			if (ok) ok = DoHideListedNodes(bCombineEffectBitmaps);
//			if (ok) ok = DoSelectResultNodes(bCombineEffectBitmaps);
			break;

		case COMBINE_SLICE:
			{
				// Slice = Subtract + Intersect
				// Make a copy of the selection before we do anything else
				Range* pNewRange = CopyRange(&OldRange);
				OriginalBoundingRect = pSelRange->GetBoundingRect(TRUE);		// Use PromoteToParent to get TRUE bounds!

				// Subtract on the original selection
				ok = ConvertToShapes(&OldRange, bCombineEffectBitmaps);
				if (ok) ok = DoCombineShapes(CLIP_STYLE_SUBTRACT, bCombineEffectBitmaps);
				if (ok) ok = DoHideListedNodes(bCombineEffectBitmaps);

				// Have to delete lists because they are global! Argh!
				ListOfSelObjPathLists.DeleteAll();
				pListOfResults->Clear();

				// Intersect on the copy
				if (ok) ok = ConvertToShapes(pNewRange, bCombineEffectBitmaps);
				if (ok) ok = DoCombineShapes(CLIP_STYLE_INTERSECT, bCombineEffectBitmaps);
				if (ok) ok = DoHideListedNodes(bCombineEffectBitmaps);

				if (ok && NodeInsertCount==0) {ok=FALSE; InformWarning(_R(IDS_COMBINE_SLICE_NORESULT));}

//				if (ok) ok = DoSelectResultNodes(bCombineEffectBitmaps);

				delete pNewRange;
				pNewRange = NULL;
			}
			break;

		default:
			ERROR3_PF(("Unknown combine reason : %d",Reason));
			ok = FALSE;
			break;
	}

	::EndSlowJob();

	if (!ok)
	{
		FailAndExecute();

		// Get ptr to selected doc
		Document* pDoc = Document::GetSelected();
		Spread*  pSpread  = Document::GetSelectedSpread();

		// If there's a selected docview, redraw the selection
		if (pDoc != NULL && pSelRange != NULL && pSpread != NULL)
			pDoc->ForceRedraw(pSpread, pSelRange->GetBlobBoundingRect());
	}
	else
		pSelRange->Update(TRUE);

	// Destroy our lists of selected object paths
	ListOfSelObjPathLists.DeleteAll();
	if (pListOfResults)
	{
		delete pListOfResults;
		pListOfResults = NULL;
	};

	GetApplication()->UpdateSelection();

	// ::EndSlowJob();

	End();
}

//---------------------------------------------------

/********************************************************************************************

> BOOL OpCombineShapes::DoAddShapes()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This adds all the paths together to form a combined silhouette path of the selection.

				This can be thought of as a logical OR of the paths
	Errors:		-
	SeeAlso:	OpCombineShapes::Do()

********************************************************************************************/

BOOL OpCombineShapes::DoAddShapes()
{
	// We'll need a new NodePath to put in the tree
	NodePath* pNodePath = new NodePath;

	// If we have trouble setting up the NodePath, return FALSE
	if (pNodePath == NULL) 		 return FALSE;
	if (!pNodePath->SetUpPath()) return FALSE;

//	if (!SubtractAllShapesFromAllLines()) return FALSE;

	// Get the list of paths produced by the first selected object
	SelObjPathList* pSelObjPathList = GetFirstList();

	// This algorithm was very inefficient for large numbers of nodes 
	// as it adds each node to the output path in turn.  As the output path 
	// grows it takes longer and longer to add each successive object.
	// To avoid this we add each path to a temporary path and when 
	// it grows too large we add the temporary path into the output path 
	// and clear the temporary one.

	NodePath* pTempPath = new NodePath;
	if (pTempPath == NULL) 		 return FALSE;
	if (!pTempPath->SetUpPath()) return FALSE;

	while (pSelObjPathList != NULL)
	{
		// Add all the paths in this list to the path inside pNodePath
		if (!AddPathsFromList(&(pTempPath->InkPath),pSelObjPathList))
			return FALSE;

		if (pTempPath->InkPath.GetNumCoords() > AddPathGranularity)
		{
			// Combine the TempPath with the output path
			if (!AddOrMergePaths(&(pNodePath->InkPath), &(pTempPath->InkPath)))
				return(FALSE);

			// And clear out the TempPath
			if (!pTempPath->InkPath.ClearPath(FALSE))
				return(FALSE);
		}

		// Get the next list of paths generated by the next selected object
		pSelObjPathList = GetNextList(pSelObjPathList);
	}

	// If the TempPath has anything in
	if (pTempPath->InkPath.GetNumCoords() > 0)
	{
		// Combine the TempPath with the output path
		if (!AddOrMergePaths(&(pNodePath->InkPath), &(pTempPath->InkPath)))
			return(FALSE);
	}

	delete pTempPath;

	// We have successfully added all the paths together to form the union inside pNodePath

	// Find the last path item in the last list (i.e. the top-most path in the selection)
	pSelObjPathList 	= GetLastList();
	SelObjPathListItem* pSelObjPathListItem = pSelObjPathList->GetLastItem();
	NodeRenderableInk* 	pCreatedByNode 	= pSelObjPathListItem->GetCreatedByNode();

	// Get ptr to last selected object because that is where we're going to put the new node path
//	pContextNode = pSelRange->FindLast();
	pContextNode = pSelObjPathListItem->GetNodePath();
	if (pContextNode==NULL)
		pContextNode = pCreatedByNode;
	AttachDir = NEXT;
	if (!DoInsertNewNode(pNodePath))
		return FALSE;

	// Set up a set of default path flags to enable the bezier tool to edit this node (Naughty gavin!)
	pNodePath->InkPath.InitialiseFlags();

	// Select the new node path (and don't redraw - this will be done due to DoInsertNewNode())
	pNodePath->Select(FALSE);

	// Apply the attrs of the node that created the last path to the new NodePath
	CCAttrMap* 			pAttrMap 		= pSelObjPathListItem->GetAttrMap();
	if (!ApplyAttributes(pCreatedByNode,pNodePath,pAttrMap))
		return FALSE;

	return TRUE;
}

//---------------------------------------------------

/********************************************************************************************

> BOOL OpCombineShapes::AddPathsFromList(Path* pPath,SelObjPathList* pSelObjPathList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		pPath			= ptr to path to place the result
				pSelObjPathList	= ptr to list of paths
	Outputs:	pPath contains the union of all the paths in pSelObjPathList
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This adds all the paths in the list together to form a combined silhouette path of the selection.

				This can be thought of as a logical OR of the paths
	Errors:		-
	SeeAlso:	OpCombineShapes::Do()

********************************************************************************************/

BOOL OpCombineShapes::AddPathsFromList(Path* pPath,SelObjPathList* pSelObjPathList)
{
	// Check our input params
	ERROR3IF(pPath == NULL,"pPath == NULL");
	ERROR3IF(pSelObjPathList == NULL,"pSelObjPathList== NULL");
	if (pPath == NULL || pSelObjPathList == NULL) return FALSE;

	NodePath* pTempPath = new NodePath;
	if (pTempPath == NULL) 		 return FALSE;
	if (!pTempPath->SetUpPath()) return FALSE;

	// Get the first path in this list
	SelObjPathListItem* pSelObjPathListItem = pSelObjPathList->GetFirstItem();

	BOOL ok = TRUE;

	while (ok && pSelObjPathListItem != NULL)
	{
		// Get a pointer to the actual node path in this list item
		Path* pSrcPath = pSelObjPathListItem->GetPath();

		if (pSrcPath != NULL)
		{
			// Add this path into the TempPath
			ok = AddOrMergePaths(&(pTempPath->InkPath), pSrcPath);

			// If the TempPath is now too large
			if (ok && pTempPath->InkPath.GetNumCoords() > AddPathGranularity)
			{
				// Combine the TempPath with the output path
				ok = AddOrMergePaths(pPath, &(pTempPath->InkPath));
				// And clear the TempPath
				if (ok) ok = pTempPath->InkPath.ClearPath(FALSE);
			}
			if (ok) pSelObjPathList->SetProducedPaths(TRUE);
		}
		else
		{
			// What? No Src path? Surely not!
			ERROR3("pSrcPath == NULL");
			ok = FALSE;
		}

		if (ok)
			ok = ::ContinueSlowJob(++JobCount);

		// Get the next item in the selected object path list.
		pSelObjPathListItem = pSelObjPathList->GetNextItem(pSelObjPathListItem);
	}

	// If the TempPath has anything in
	if (ok && pTempPath->InkPath.GetNumCoords() > 0)
	{
		// Combine the TempPath with the output path
		ok = AddOrMergePaths(pPath, &(pTempPath->InkPath));
	}

	delete pTempPath;

	return (ok);
}



/****************************************************************************

>	BOOL OpCombineShapes::AddOrMergePaths(Path* pPath1, Path* pPath2)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/02/2005

	Inputs:		pPath1		- pointer to a Path
				pPath2		- pointer to a Path
	Returns:	TRUE if ok, FALSE if bother
	Purpose:	Helper function for DoAddShapes and AddPathsFromList

****************************************************************************/

BOOL OpCombineShapes::AddOrMergePaths(Path* pPath1, Path* pPath2)
{
	BOOL ok = TRUE;
	// if num coords == 0, the path is effectively empty, so just copy this node path's path
	if (pPath1->GetNumCoords() == 0)
		ok = pPath1->MergeTwoPaths(*pPath2);
	else
	{
		// Otherwise actually 'add' the paths together, using ClipPathToPath
		ok = AddPaths(pPath1,pPath2);

		// If this fails, we can just merge the two paths together as this achieves the desired
		// silhouette effect
		if (!ok) ok = pPath1->MergeTwoPaths(*pPath2);

	}
	return(ok);
}


/********************************************************************************************

> BOOL OpCombineShapes::AddPaths(Path* pPath1,Path* pPath2)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		pPath1 = ptr to first path, and also the place to store resultant path
				pPath2 = ptr to second path, i.e. the path to add to pPath1
	Outputs:	*pPath1 contains the result of adding the two paths (if TRUE returned).
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This adds the two paths together using ClipPathToPath, storing the result in pPath1
	Errors:		-
	SeeAlso:	OpCombineShapes::AddPathsFromList()

********************************************************************************************/

BOOL OpCombineShapes::AddPaths(Path* pPath1,Path* pPath2)
{
	ERROR2IF(pPath1 == NULL,FALSE,"pPath1 == NULL");
	ERROR2IF(pPath2 == NULL,FALSE,"pPath2 == NULL");

	// check for a zero-bounding box path
	DocRect dr = pPath2->GetBoundingRect();

	if (dr.Width() <= 1 && dr.Height() <= 1)
	{
		ERROR3("We have a zero bounding rect path");
		return FALSE;
	}

	Path StrokedPath2;
	if (!StrokedPath2.Initialise()) return FALSE;

	// is path 2 a line?
	if (pPath2->GetPathType() == PATHTYPE_LINE)
	{
		if (!StrokePathToPath(pPath2,&StrokedPath2))
			return FALSE;
	}

	INT32 len = CombinePaths(CLIP_STYLE_ADD,pPath2,&StrokedPath2,pPath1,pPath1);

	return (len > 0);
}

//-----------------------------------------------

/********************************************************************************************

> BOOL OpCombineShapes::DoCombineShapes(ClipStyle Style, BOOL bCombineEffectBitmaps)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		Style = High level style of clipping
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	Firstly, this adds all the paths together in the last list to form the clip path.
				Then all the other paths in the other lists are combined with the clip path, using
				the method specified in 'Flags'
	Errors:		-
	SeeAlso:	OpCombineShapes::Do()

********************************************************************************************/

BOOL OpCombineShapes::DoCombineShapes(ClipStyle Style, BOOL bCombineEffectBitmaps)
{
	Path ClipPath;
	Path StrokedClipPath;
	if (!ClipPath.Initialise()) 		return FALSE;
	if (!StrokedClipPath.Initialise()) 	return FALSE;

	// Get the list of paths produced by the top selected object
	SelObjPathList* pLastSelObjPathList = GetLastList();

	// Add all the paths in the last list to form the clip path to combine with
	if (!AddPathsFromList(&ClipPath,pLastSelObjPathList))
		return FALSE;

	// is the clip path is a line?
	if (ClipPath.GetPathType() == PATHTYPE_LINE)
	{
		if (!StrokePathToPath(&ClipPath,&StrokedClipPath))
			return FALSE;
	}

	// Get the list of paths produced by the first selected object
	SelObjPathList* pSelObjPathList = GetFirstList();

	ERROR2IF(pSelObjPathList == NULL,FALSE,"ptr to First list is NULL");
	ERROR2IF(pSelObjPathList->GetSelNode() == NULL,FALSE,"First list has no sel node");

	if (bCombineEffectBitmaps)
	{
		//-----------------------------------------------
		// Effects stacks will not be retained - the topmost effect will turn itself into
		// a bitmap
		//
		// All paths the op will produce will be placed into a group for convenience.
		// We don't know how many paths will be produced until after the op has done its stuff, so we
		// stick everything in a group, and if only one path is produced, we take it out and hide the group.

		// So, firstly create a group to put the paths in.
		pContextNode = new NodeGroup;
		if (pContextNode == NULL)
			return FALSE;

		// Insert the group into the tree
		if (!UndoableOperation::DoInsertNewNode((NodeGroup*)pContextNode,pSelObjPathList->GetSelNode(),NEXT,FALSE,FALSE,FALSE,FALSE))
			return FALSE;

		// We want all new paths to be inserted as children of the group
		AttachDir = LASTCHILD;

		// Make a note of how many paths have been inserted into the tree so far, so we can tell after the main
		// loop how many paths have been produced
		INT32 PrevInsertCount = NodeInsertCount;

		// This will point to a node to select, if indeed we want to select the node (see later)
		NodeRenderable* pSelectNode = NULL;

		// Main loop.  This produces all the paths and places them into the group
		while (pSelObjPathList != NULL && pSelObjPathList != pLastSelObjPathList)
		{
			// Combine all the paths in this list with the ClipPath
			// pContextNode is static! Argh!
			if (!CombinePathsFromList(Style,&ClipPath,&StrokedClipPath,pSelObjPathList))
				return FALSE;

			// Get the next list of paths generated by the next selected object
			pSelObjPathList = GetNextList(pSelObjPathList);
		}

		// We now need to decide what to do based on the number of paths produced.
		if (NodeInsertCount == (PrevInsertCount+1))
		{
			// Only one path was produced, so localise the attrs (just in case), move the path out of the
			// group, and hide the unneeded group
			if (!DoLocaliseCommonAttributes((NodeGroup*)pContextNode))
				return FALSE;

			Node* pNode = pContextNode->FindFirstChild();
			while (pNode != NULL && !pNode->IsAnObject())
				pNode = pNode->FindNext();

			if (pNode != NULL)
			{
				if (!DoMoveNode(pNode,pContextNode,NEXT))
					return FALSE;
			}

			if (!DoHideNode(pContextNode,TRUE))
				return FALSE;

			pSelectNode = (NodeRenderable*)pNode;
			pListOfResults->AddNode(pSelectNode);
		}
		else if (NodeInsertCount > (PrevInsertCount+1))
		{
			// More than one path was produced, so keep the group and factor out any common attributes.
			if (!DoFactorOutCommonChildAttributes((NodeGroup*)pContextNode))
				return FALSE;

			pSelectNode = (NodeGroup*)pContextNode;
			pListOfResults->AddNode(pSelectNode);
		}
		else
		{
			// The op produced not paths so just hide the empty group
			if (!DoHideNode(pContextNode,TRUE))
				return FALSE;
		}

		// Select the new node (and don't redraw - this will be done due to DoInsertNewNode())
		//
		// When slicing, only select the objects produced on the intersection stage
//		if (pSelectNode != NULL && (Reason != COMBINE_SLICE || Style != CLIP_STYLE_SUBTRACT))
//			pSelectNode->Select(FALSE);
	}
	else
	{
		//-----------------------------------------------
		// Effects stacks will be retained
		// So all shape combination must be done within the effects stack structure
		//
		// Make a note of how many paths have been inserted into the tree so far, so we can tell after the main
		// loop how many paths have been produced
//		INT32 PrevInsertCount = NodeInsertCount;

		// Main loop.  This produces all the paths
		while (pSelObjPathList != NULL && pSelObjPathList != pLastSelObjPathList)
		{
			// Combine all the paths in this list with the ClipPath
			// pContextNode is static! Argh!
			if (!CombinePathsFromList(Style, &ClipPath, &StrokedClipPath, pSelObjPathList, bCombineEffectBitmaps))
				return FALSE;

			// Factor out any common attributes.
			Node* pSelNode = pSelObjPathList->GetSelNode();
			if (pSelNode && pSelNode->IsCompoundClass())
				if (!DoFactorOutCommonChildAttributes((NodeCompound*)pSelNode, TRUE))
					return FALSE;

			// Get the next list of paths generated by the next selected object
			pSelObjPathList = GetNextList(pSelObjPathList);
		}

	}

	// We have successfully combined all the paths
	return TRUE;
}




/********************************************************************************************

> BOOL OpCombineShapes::CombinePathsFromList(	ClipStyle Style,
												Path* pClipPath,
												Path* pStrokedClipPath,
												SelObjPathList* pSelObjPathList,
												BOOL bCombineEffectBitmaps = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		Style 			= High level style of clipping
				pClipPath		= ptr to path to combine with the paths in pSelObjPathList
				pStrokedClipPath= ptr to stroked version of pClipPath ONLY if pClipPath is a line
				pSelObjPathList	= ptr to list of paths
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This combines pClipPath with all the paths in pSelObjPathList.

				For each path it tries to combine it with pClipPath.  A new NodePath is generated and
				placed in the tree at the place the selected object lives.
	Errors:		-
	SeeAlso:	OpCombineShapes::DoCombineShapes()

********************************************************************************************/

BOOL OpCombineShapes::CombinePathsFromList(	ClipStyle Style,
											Path* pClipPath,
											Path* pStrokedClipPath,
											SelObjPathList* pSelObjPathList,
											BOOL bCombineEffectBitmaps)
{
	// Check our input params
	ERROR3IF(pClipPath == NULL,"pClipPath == NULL");
	ERROR3IF(pSelObjPathList == NULL,"pSelObjPathList == NULL");
	if (pClipPath == NULL || pSelObjPathList == NULL) return FALSE;

	// Get the first path in this list
	SelObjPathListItem* pFirstItem = pSelObjPathList->GetFirstItem();
	SelObjPathListItem* pSelObjPathListItem = pFirstItem;
	BOOL ok = TRUE;

	while (ok && pSelObjPathListItem != NULL)
	{
		// Get a pointer to the actual node path in this list item
		NodePath* pNodePath = pSelObjPathListItem->GetNodePath();
		NodePath* pNewNodePath = new NodePath;
		NodeRenderableInk* pCreatedByNode = pSelObjPathListItem->GetCreatedByNode();

		if (pNodePath != NULL && pNewNodePath != NULL && pNewNodePath->SetUpPath())
		{
			//pNewNodePath->InkPath.Initialise(pNodePath->InkPath.GetNumCoords(), 64);
			//if 	(pNewNodePath->InkPath.CopyPathDataFrom(&(pNodePath->InkPath)))
			if (CombinePaths(Style, pClipPath, pStrokedClipPath, &(pNodePath->InkPath), &(pNewNodePath->InkPath)))
			{
				if (bCombineEffectBitmaps)
					ok = DoInsertNewNode(pNewNodePath);
				else
				{
					if (pSelObjPathListItem==pFirstItem)
					{
						// The first path produced can live where it's originating node lived
						// inside whatever Effect stack was in place
						ok = UndoableOperation::DoInsertNewNode(pNewNodePath, pNodePath, NEXT, TRUE, FALSE, FALSE, TRUE);
					}
					else
					{
						// If more than one path is produced under an Effect node
						// Then we must make a new Effect Stack for it
						//
						// BUT NOTE! I don't think any multiple set of nodes are produced outside
						// a group at the moment - so the effect applciation code found here
						// has never yet been tested!
						Node* pInsertNode = pNodePath;
						ListRange* pStack = EffectsStack::GetEffectsStackFromNode(pNodePath, FALSE, TRUE, TRUE);	// Include locked effects
						if (pStack)
							pInsertNode = pStack->FindLast();
						ok = UndoableOperation::DoInsertNewNode(pNewNodePath, pInsertNode, NEXT, TRUE, FALSE, FALSE, TRUE);
						pInsertNode = pNewNodePath;
						delete pStack;

						// Now copy the effects stack for this new child...
						if (ok) ok = ApplyEffects(pSelObjPathListItem, pNewNodePath, NULL);

					}

					// Select the resulting path now
					if (ok && pCreatedByNode->IsSelected())
						pNewNodePath->Select(FALSE);

					if (ok)
					{
						pListOfResults->AddNode(pNewNodePath);
						NodeInsertCount++;
					}
				}

				// Find out who created the path, and apply its attrs to the new NodePath
				CCAttrMap* 			pAttrMap 		= pSelObjPathListItem->GetAttrMap();
				
				if (ok) ok = ApplyAttributes(pCreatedByNode, pNewNodePath, pAttrMap, pSelObjPathListItem);

				// Set up a set of default path flags to enable the bezier tool to edit this node
				if (ok) pNewNodePath->InkPath.InitialiseFlags();

				if (ok) pSelObjPathList->SetProducedPaths(TRUE);
			}
			else
			{
				if (pNewNodePath!=NULL)	{ delete pNewNodePath; pNewNodePath = NULL;	}

				// Don't worry if the actual combine fails.  We'll just not put anything in the tree
				// because, hmmm, I wonder....
			}
		}
		else
		{
			// What? Error? Surely not!
			ERROR3("pNodePath == NULL || pNewNodePath == NULL || !pNewNodePath->SetUpPath())");
			ok = FALSE;

			if (pNewNodePath!=NULL)	{ delete pNewNodePath; pNewNodePath = NULL;	}
		}

		if (ok)
			ok = ::ContinueSlowJob(++JobCount);

		// Get the next item in the selected object path list.
		pSelObjPathListItem = pSelObjPathList->GetNextItem(pSelObjPathListItem);
	}

	return (ok);
}

/********************************************************************************************

> BOOL OpCombineShapes::CombinePaths(ClipStyle Style,Path* pClipPath,Path* pSrcPath,Path* pDestPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		Style = High level style of clipping
				pClipPath			= ptr to the path to combine with pSrcPath
				pStrokedClipPath	= ptr to stroked version of pClipPath ONLY if pClipPath is a line
				pSrcPath			= ptr to path that will be combined with pClipPath
				pDestPath			= ptr to path to place the result in
	Outputs:	*pDestPath contains the result of combining pCliPath with pSrcPath(if TRUE returned).
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This combines pClipPath from pSrcPath using ClipPathToPath, storing the result in pDestPath
	Errors:		-
	SeeAlso:	OpCombineShapes::CombinePathsFromList()

********************************************************************************************/

BOOL OpCombineShapes::CombinePaths(ClipStyle Style,Path* pClipPath,Path* pStrokedClipPath,Path* pSrcPath,Path* pDestPath)
{
	// Check those entry params
	ERROR2IF(pClipPath 			== NULL,FALSE,"pClipPath == NULL");
	ERROR2IF(pStrokedClipPath 	== NULL,FALSE,"pStrokedClipPath == NULL");
	ERROR2IF(pSrcPath  			== NULL,FALSE,"pSrcPath == NULL");
	ERROR2IF(pDestPath 			== NULL,FALSE,"pDestPath == NULL");

	UINT32 Flags = 0;	// Flags passed to ClipPathToPath()
	BOOL Clip = TRUE;	// If this is TRUE, ClipPathToPath() is called at the end of this func
	INT32 len = -1;		// This is the length of the path produced by ClipPathToPath

	BOOL SrcIsLine 	= (pSrcPath->GetPathType() 	== PATHTYPE_LINE);	// TRUE if src path is a line
	BOOL ClipIsLine = (pClipPath->GetPathType() == PATHTYPE_LINE);	// TRUE if clip path is a line

	Path DestPath;		// Used when using a line to slice the source path up

	// If the clip path is a line and the op is slice, try and make a closed path using pClipPath
	// and the bounding rect of pSrcPath
	if (ClipIsLine && Reason == COMBINE_SLICE)
	{
		if (DestPath.Initialise() && ConvertClipLineToShape(pClipPath,pSrcPath,&DestPath))
		{
			// If successfully created a closed path, then pretend func was entered with the new path
			// by pointing pClipPath to it. Also the clip path is no longer a line
			pClipPath = &DestPath;
			ClipIsLine = FALSE;
		}
	}
	
	switch (Style)
	{
		case CLIP_STYLE_SUBTRACT:
			Flags = 1;	// 1 is Source AND NOT Clip
			if (SrcIsLine) 	Flags |= CLIPPING_IS_STROKED;	// If source is a line, set the stroked flag

			if (ClipIsLine)
			{
				// If slicing with a line, don't bother doing the subtraction stage 
				if (Reason == COMBINE_SLICE)
					Clip = FALSE;
				else
				// Otherwise subtract with a stroked version of the line.
				// If the user's mad enough to try it, then we'd better try our best
				{ pClipPath = pStrokedClipPath; Flags |= CLIPPING_CLIP_WINDING; }
			}
			break;

		case CLIP_STYLE_INTERSECT:
			// If src is a line, Flags = 0 | stroked flag, else Flags = Source AND Clip
			if (SrcIsLine) Flags = CLIPPING_IS_STROKED; else Flags = 2; 
			if (ClipIsLine)
			{
				// If slicing with a line, don't bother doing the intersection stage 
				if (Reason == COMBINE_SLICE)
					Clip = FALSE;
				else
				// Otherwise intersect with a stroked version of the line.
				// If the user's mad enough to try it, then we'd better try our best
				{ pClipPath = pStrokedClipPath; Flags |= CLIPPING_CLIP_WINDING; }
			}
			break;

		case CLIP_STYLE_ADD:
			Flags = 7;	// 7 is Source OR Clip
			// convert lines into paths by stroking them
			// Also we must use a Non-zero winding rule to avoid cross segment lines taking effect
			if (SrcIsLine)  { StrokePathToPath(pSrcPath,pSrcPath);   Flags |= CLIPPING_SOURCE_WINDING; }
			if (ClipIsLine) { pClipPath = pStrokedClipPath; Flags |= CLIPPING_CLIP_WINDING; }
			break;
	}

	if (Clip)
	{
		// Clip those paths together
		// Mark Howitt 31/10/00
		// I`ve removed the ClipPathToPathWithAutoFlatness function and replaced it with
		// the function that returns you a flatness value to use with the ClipPath function
		double ClippingFlatness = pClipPath->CalculateFlatnessValueFromPath(750.0, 2.0, 375.0);
		double SourceFlatness = pSrcPath->CalculateFlatnessValueFromPath(750.0, 2.0, 375.0);

		len  = pClipPath->ClipPathToPath(*pSrcPath, pDestPath, Flags, DefaultTolerance, ClippingFlatness, SourceFlatness);
/*
		// show debugging info about the path we have clipped
		INT32 NumCoords = pDestPath->GetNumCoords();
		PathVerb * pVerbs = pDestPath->GetVerbArray();
		DocCoord * pCoord = pDestPath->GetCoordArray();

		for (INT32 i = 0; i < NumCoords; i++)
		{
			if (pVerbs[i] == PT_MOVETO)
				TRACE( _T("MOVE TO "));
			else
			if (pVerbs[i] == PT_BEZIERTO)
				TRACE( _T("BEZIER TO "));
			else
			if (pVerbs[i] == PT_LINETO)
				TRACE( _T("LINE TO "));

			TRACE( _T("%d, %d\n"), pCoord[i].x, pCoord[i].y);
		}
		TRACE( _T("end line\n"));
*/
	}

	return (len > 0);
}


/********************************************************************************************

> BOOL OpCombineShapes::ConvertClipLineToShape(Path* pClipPath,Path* pSrcPath,Path* pDestPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		pClipPath	= ptr to the path to convert
				pSrcPath	= ptr to path that will be combined with pClipPath
				pDestPath	= ptr to path to place the result in
	Outputs:	*pDestPath contains the result of converting pCliPath with regards to pSrcPath (if TRUE returned).
	Returns:	TRUE if OK, FALSE if failed

	Purpose:	Only call this if pClipPath is a line.
	
				This creates a shape out of pClipPath such that the path from the start point of pClipPath to
				the end point of pClipPath does NOT intersect with pSrcPath.
				It can only do this if the start and end point of pClipPath are outside the coord bounds of pSrcPath.
				If either pClipPath ends are inside pSrcPath's bounds, FALSE is returned.

				The shape is created by tacking on parts of the bounding rectangle to pClipPath, then closing
				the resultant path.
				Note: pDestPath must not point to pClipPath OR pSrcPath (i.e. you'll need three paths)
	Errors:		-
	SeeAlso:	OpCombineShapes::CombinePaths()

********************************************************************************************/

BOOL OpCombineShapes::ConvertClipLineToShape(Path* pClipPath,Path* pSrcPath,Path* pDestPath)
{
	// Check for stupid entry parama
	ERROR2IF(pClipPath == NULL,FALSE,"pClipPath == NULL");
	ERROR2IF(pSrcPath  == NULL,FALSE,"pSrcPath == NULL");
	ERROR2IF(pDestPath == NULL,FALSE,"pClipPath == NULL");

	// Get the coord bounds of the source path.  Inflate it slightly so that we don't run into tolerance
	// problems when calling ClipPathToPath()
	//DocRect Rect = GetBoundingRect(pSrcPath);

	// we must be consistant as to the rect we use to get a consistant shape
	// otherwise some items will all get cut along the line but it is luck
	// which side of the cut the shapes appear in (sjk 15/11/00)
//	DocRect Rect = GetApplication()->FindSelection()->GetBoundingRect(); // use a consistant rect for all the shapes
	DocRect Rect = OriginalBoundingRect; // use a consistant rect for all the shapes

	Rect.Inflate((DefaultTolerance*2)+10);
	
	INT32 		NumClipCoords = pClipPath->GetNumCoords();
	DocCoord*	pClipCoords   = pClipPath->GetCoordArray();
	DocCoord	ClipStart	  = pClipCoords[0];					// Start point of pClipPath
	DocCoord	ClipEnd		  = pClipCoords[NumClipCoords-1];	// End   point of pClipPath
	DocCoord	ExtraClipStart= pClipCoords[0];					// Start point of pClipPath
	DocCoord	ExtraClipEnd  = pClipCoords[NumClipCoords-1];	// End   point of pClipPath
	BOOL		NeedExtraClipStart	= FALSE;
	BOOL		NeedExtraClipEnd	= FALSE;

	Path		FlatPath;
//	BOOL		SetFlatPath = FALSE;
//	double		FLATTNESS =  1024;	// arbitry figure of how flattened the curve will be
									// only used for test for extra cuts

	// We have to choose a corner point of the bounding rect for both the start and end points
	// of pClipPath.

	DocCoord ClipCornerStart,ClipCornerEnd;

	DocCoord mid = Rect.Centre();

	if (ClipStart.x < mid.x)
		ClipCornerStart.x = Rect.lo.x;
	else
		ClipCornerStart.x = Rect.hi.x;

	if (ClipStart.y < mid.y)
		ClipCornerStart.y = Rect.lo.y;
	else
		ClipCornerStart.y = Rect.hi.y;

	if (ClipEnd.x < mid.x)
		ClipCornerEnd.x = Rect.lo.x;
	else
		ClipCornerEnd.x = Rect.hi.x;

	if (ClipEnd.y < mid.y)
		ClipCornerEnd.y = Rect.lo.y;
	else
		ClipCornerEnd.y = Rect.hi.y;

	// start clip point in the rect?
	// move it to the nearest edge
	if (Rect.ContainsCoord(ClipStart))
	{
		NeedExtraClipStart = TRUE;
		DocCoord v = ClipStart - pClipCoords[1];
		INT32 giveup = 250; // number of times to try

		// extrapperlate the last part of the line until it breaks out of the bounding rect
		// give up if it fails to break out after an arbitry number of goes
		while (Rect.ContainsCoord(ExtraClipStart) && giveup > 0)
		{
			ExtraClipStart.x += v.x * 10;
			ExtraClipStart.y += v.y * 10;
			giveup--;
		}

		// failled to leave the shape
		if (giveup == 0) 
			return FALSE;

		// moving the start may change which is the nearest corner to go for
		DocCoord mid = Rect.Centre();

		if (ExtraClipStart.x < mid.x)
			ClipCornerStart.x = Rect.lo.x;
		else
			ClipCornerStart.x = Rect.hi.x;

		if (ExtraClipStart.y < mid.y)
			ClipCornerStart.y = Rect.lo.y;
		else
			ClipCornerStart.y = Rect.hi.y;
	}

	// perform all the same calcualtions for the other end of the clip line
	if (Rect.ContainsCoord(ClipEnd))
	{
		NeedExtraClipEnd = TRUE;
		DocCoord v = ClipEnd - pClipCoords[NumClipCoords-2];
		INT32 giveup = 250; // number of times to try

		// extrapperlate the last part of the line until it breaks out of the bounding rect
		// give up if it fails to break out after an arbitry number of goes
		while (Rect.ContainsCoord(ExtraClipEnd) && giveup > 0)
		{
			ExtraClipEnd.x += v.x * 10;
			ExtraClipEnd.y += v.y * 10;
			giveup--;
		}

		// failled to leave the shape
		if (giveup == 0) 
			return FALSE;

		// moving the end may change which is the nearest corner to go for
		DocCoord mid = Rect.Centre();

		if (ExtraClipEnd.x < mid.x)
			ClipCornerEnd.x = Rect.lo.x;
		else
			ClipCornerEnd.x = Rect.hi.x;

		if (ExtraClipEnd.y < mid.y)
			ClipCornerEnd.y = Rect.lo.y;
		else
			ClipCornerEnd.y = Rect.hi.y;			
	}

	// We now have two sensible corners of Rect to connect pClipPath's end points to
	// and if required also optimal points on this Rect boundry to that are
	// in direct line with the ends of the cutting line

	// Firstly, copy pClipPath to pDestPath
	pDestPath->ClearPath(FALSE);
	if (!pDestPath->MergeTwoPaths(*pClipPath)) return FALSE;

	// add on an extra end part to make this end of the clip path outside the bounds of the selection
	if (NeedExtraClipEnd)
	{
		if (!pDestPath->AddLineTo(ExtraClipEnd)) return FALSE;
	}

	// Connect the end of the line to the end corner point
	if (!pDestPath->AddLineTo(ClipCornerEnd)) return FALSE;

	// add in the corner nearest the end
	// If end corner x != start corner x, we need to connect these up
	if (ClipCornerEnd.x != ClipCornerStart.x)
	{
		ClipCornerEnd.x = ClipCornerStart.x;
		if (!pDestPath->AddLineTo(ClipCornerEnd)) return FALSE;
	}

	// if the start and end corners are opposing we need to use an intermediate corner to avoid the selection
	// If end corner y != start corner y, we need to connect these up
	if (ClipCornerEnd.y != ClipCornerStart.y)
	{
		ClipCornerEnd.y = ClipCornerStart.y;
		if (!pDestPath->AddLineTo(ClipCornerEnd)) return FALSE;
	}

	// add in the corner nearest the start
	// add on an extra end part to make this end of the clip path outside the bounds of the selection
	if (NeedExtraClipStart)
	{
		if (!pDestPath->AddLineTo(ExtraClipStart)) return FALSE;
	}
	
	// Finally, close the path up by connecting the start corner point to the start of the line
	// and setting PT_CLOSEFIGURE in the last verb
	if (!pDestPath->AddLineTo(ClipStart)) return FALSE;

	INT32 n = pDestPath->GetNumCoords();
	PathVerb* pVerbs = pDestPath->GetVerbArray();
	pVerbs[n-1] |= PT_CLOSEFIGURE;

	return TRUE;
}

/********************************************************************************************

> DocRect OpCombineShapes::GetBoundingRect(Path* pPath)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/2/95
	Inputs:		pPath = ptr to path
	Outputs:	-
	Returns:	The stroked bounding rect of this path, disregarding attributes
	Purpose:	This gets the exact pure path bounds, using GDraw_CalcStrokeBBox.
	Errors:		-
	SeeAlso:	OpCombineShapes::DoCombineShapes()

********************************************************************************************/


DocRect OpCombineShapes::GetBoundingRect(Path* pPath)
{
	DocRect Rect;

	if (pPath != NULL)
	{
		#if REAL_GDRAW
		// Get GDraw to get the stroked bounds for this path
		GDrawContext *GD = GRenderRegion::GetStaticDrawContext();
		
		if (GD != NULL)
		{
			GD->CalcStrokeBBox((POINT*)pPath->GetCoordArray(),pPath->GetVerbArray(),pPath->GetNumCoords(),
								(tagRECT*)(&Rect),pPath->IsFilled, 0, CAPS_ROUND, JOIN_ROUND, NULL);
		}
		#else
		Rect = pPath->GetBoundingRect();
		#endif
	}

	if (!Rect.IsValid())
		Rect.MakeEmpty();

	return Rect;
}

//---------------------------------------------

/********************************************************************************************

> BOOL OpCombineShapes::SubtractAllShapesFromAllLines()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This subtracts all the shapes in all the lists from all the lines in all the lists
	Errors:		-
	SeeAlso:	OpCombineShapes::DoCombineShapes()

********************************************************************************************/

/*BOOL OpCombineShapes::SubtractAllShapesFromAllLines()
{
	// Get the list of paths produced by the first selected object
	SelObjPathList* pSelObjPathList = GetFirstList();

	while (pSelObjPathList != NULL)
	{
		SelObjPathListItem* pSelObjPathListItem = pSelObjPathList->GetFirstItem();

		while (pSelObjPathListItem != NULL)
		{
			Path* pLine = pSelObjPathListItem->GetPath();
			ERROR3IF(pLine == NULL,"SelObjPathListItem pLine == NULL");

			if (pLine != NULL && pLine->GetPathType() == PATHTYPE_LINE)
			{
				if (!SubtractAllShapesFromLine(pLine))
					return FALSE;
			}
				
			pSelObjPathListItem = pSelObjPathList->GetNextItem(pSelObjPathListItem);
		}

		// Get the next list of paths generated by the next selected object
		pSelObjPathList = GetNextList(pSelObjPathList);
	}

	return TRUE;
}
*/
/********************************************************************************************

> BOOL OpCombineShapes::SubtractAllShapesFromLine(Path* pLine)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		pLine = ptr to a line
	Outputs:	pLine is changed
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This subtracts all the shapes in all the lists from pLine
	Errors:		-
	SeeAlso:	OpCombineShapes::DoCombineShapes()

********************************************************************************************/

/*BOOL OpCombineShapes::SubtractAllShapesFromLine(Path* pLine)
{
	// Get the list of paths produced by the first selected object
	SelObjPathList* pSelObjPathList = GetFirstList();

	while (pSelObjPathList != NULL)
	{
		// Combine all the paths in this list with the ClipPath
		if (!SubtractShapesFromLine(pLine,pSelObjPathList))
			return FALSE;

		// Get the next list of paths generated by the next selected object
		pSelObjPathList = GetNextList(pSelObjPathList);
	}

	return TRUE;
}
*/
/********************************************************************************************

> BOOL OpCombineShapes::SubtractShapesFromLine(Path* pLine,SelObjPathList* pSelObjPathList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		pLine			= ptr to a line
				pSelObjPathList	= ptr to list of paths
	Outputs:	pLine is changed
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This subtracts all the shapes in pSelObjPathList from pLine
	Errors:		-
	SeeAlso:	OpCombineShapes::DoCombineShapes()

********************************************************************************************/
/*
BOOL OpCombineShapes::SubtractShapesFromLine(Path* pLine,SelObjPathList* pSelObjPathList)
{
	// Get the first path in this list
	SelObjPathListItem* pSelObjPathListItem = pSelObjPathList->GetFirstItem();

	while (pSelObjPathListItem != NULL)
	{
		Path* pClipPath = pSelObjPathListItem->GetPath();
		ERROR3IF(pClipPath == NULL,"SelObjPathListItem pClipPath == NULL");

		if (pClipPath != NULL && pClipPath != pLine && pClipPath->GetPathType() == PATHTYPE_SHAPE)
		{
			if (!CombinePaths(CLIP_STYLE_SUBTRACT,pClipPath,pLine,pLine))
				return FALSE;
		}

		pSelObjPathListItem = pSelObjPathList->GetNextItem(pSelObjPathListItem);
	}

	return TRUE;
}
*/
//---------------------------------------------------

/********************************************************************************************

> BOOL OpCombineShapes::ConvertToShapes(Range* pRange, BOOL bCombineEffectBitmaps)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/04/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This generates the data structure that holds all the paths generated by the selected objects.
				NOTE! Pointers to paths in the tree are stored in the lists (unlike CreateSelObjPathLists)

				This consists of a top level list than contains a series of secondary lists of paths.
				There is one instance of a secondary list per selected object.

				List Of Selected Object Path lists
					List of paths made by first selected object
						SelObjPathListItem
						SelObjPathListItem
						SelObjPathListItem
						...
					List of paths made by second selected object
						...
	Errors:		-
	SeeAlso:	CreateSelObjPathList()

********************************************************************************************/

BOOL OpCombineShapes::ConvertToShapes(Range* pRange, BOOL bCombineEffectBitmaps)
{
	CombineBecomeA BecomePath(BECOMEA_REPLACE, this, NULL, NULL, FALSE);
	BecomePath.SetInsertComplexBlendStepsAsPaths (TRUE);		// compound nodes MUST be inserted as paths!
	BecomePath.SetResultsStayInPlace(!bCombineEffectBitmaps);	// Only stay in place if notcaptruing effect bitmaps

	Node* pNode = pRange->FindFirst(FALSE);
	Node* pNextNode = NULL;
	BOOL ok = TRUE;

	while (pNode!=NULL && ok)
	{
		pNextNode = pRange->FindNext(pNode, FALSE);

		// Make a path list for this selected object if it can become a NodePath
		SelObjPathList* pSelObjPathList = new SelObjPathList(pNode);
		ok = (pSelObjPathList!=NULL);
		if (ok)
		{
			ListOfSelObjPathLists.AddTail(pSelObjPathList);

			// --- This code ripped from CreateSelObjPathList - looks suspect! ---
			// Check to see if this is the Top most visible object. This is used by the
			// Cached Compound Nodes to help determine whether or not to use the Cached nodes or BMP!
			BOOL IsTopObject = (pNextNode == NULL);
			// --- ! ---

			if (pNode->CanBecomeA(&BecomePath))
				ok = CreateSelObjPathList(pNode, pSelObjPathList, IsTopObject, bCombineEffectBitmaps, BECOMEA_REPLACE);

			// Tidy up if things have gone wrong
			if (!ok)
			{
				ListOfSelObjPathLists.RemoveTail();
				delete pSelObjPathList;
			}

			if (pNode->IsAnObject()) ((NodeRenderableInk*)pNode)->ReleaseCached();
		}

		pNode = pNextNode;

		if (ok)
			ok = ::ContinueSlowJob(++JobCount);
	}

	return ok;
}

/********************************************************************************************

> BOOL OpCombineShapes::CreateSelObjPathLists(BOOL bCombineEffectBitmaps);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This generates the data structure that holds all the paths generated by the selected objects.

				This consists of a top level list than contains a series of secondary lists of paths.
				There is one instance of a secondary list per selected object.

				List Of Selected Object Path lists
					List of paths made by first selected object
						SelObjPathListItem
						SelObjPathListItem
						SelObjPathListItem
						...
					List of paths made by second selected object
						...
	Errors:		-
	SeeAlso:	CreateSelObjPathList()

********************************************************************************************/

BOOL OpCombineShapes::CreateSelObjPathLists(BOOL bCombineEffectBitmaps)
{
	ERROR2IF(pSelRange == NULL,FALSE,"pSelRange == NULL");

	Node* pSelNode = pSelRange->FindFirst();
	UINT32 Count = 0;	// Used for a check made at end of routine
	BOOL ok = TRUE;
	BOOL IsTopObject = TRUE;

	::BeginSlowJob();

	BecomeA BecomePath(BECOMEA_REPLACE, CC_RUNTIME_CLASS(NodePath));
	BecomePath.SetResultsStayInPlace(!bCombineEffectBitmaps);

	while (ok && pSelNode != NULL)
	{
		if (pSelNode->CanBecomeA(&BecomePath))
		{
			Count++;	// Count num objs that can become a NodePath

			// Make a path list for this selected object if it can become a NodePath
			SelObjPathList* pSelObjPathList = new SelObjPathList(pSelNode);

			if (pSelObjPathList != NULL)
			{
				// Check to see if this is the Top most visable object. This is used by the
				// Cached Compound Nodes to help determine wether or not to use the Cached nodes or BMP!
				IsTopObject = (pSelRange->FindNext(pSelNode) == NULL);

				// Get the selected object to create the actual NodePath objects
				// and if successful, stuff the list on our list of sel obj path lists
				if (CreateSelObjPathList(pSelNode, pSelObjPathList, IsTopObject, bCombineEffectBitmaps))
				{
					ListOfSelObjPathLists.AddTail(pSelObjPathList);
				}
				else
				{
					ok = FALSE;
				}
			}
			else
				ok = FALSE;
		}

		if (ok)
			ok = ::ContinueSlowJob(++JobCount);

		// Get the next selected object
		pSelNode = pSelRange->FindNext(pSelNode);
	}

	::EndSlowJob();

	if (ok)
	{
		UINT32 MinCount = 2;
		if (Reason == COMBINE_ADD)
			MinCount = 1;

		if (Count < MinCount)
		{
			ERROR3_PF(("Less than %d of the selected objects could become paths",MinCount));
			return FALSE;
		}
	}

	return ok;
}

/********************************************************************************************

> BOOL OpCombineShapes::CreateSelObjPathList(Node* pNode,SelObjPathList* pSelObjPathList,BOOL IsFirstNode, BOOL bCombineEffectBitmaps, BecomeAReason reason = BECOMEA_REPLACE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		pNode			= ptr to the node that will generate one or more NodePaths
				pSelObjPathList	= the list to add the paths to using SelObjPathListItems
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This generates the data structure that holds all the paths generated by a given selected object.

				It uses a derived BecomeA class to receive the paths and generate SelObjPathListItems
				to add to pSelObjPathList
	Errors:		-
	SeeAlso:	CreateSelObjPathLists()

********************************************************************************************/

BOOL OpCombineShapes::CreateSelObjPathList(Node* pNode,SelObjPathList* pSelObjPathList,BOOL IsFirstNode, BOOL bCombineEffectBitmaps, BecomeAReason reason)
{
	ERROR2IF(pNode == NULL || pSelObjPathList == NULL,FALSE,"One or more params are NULL");

	// Set up a BecomeA derived object, so that we can receive all the paths generated by pNode.
	CombineBecomeA ParamBecomeA(reason, this,pSelObjPathList,pNode,IsFirstNode);
	ParamBecomeA.SetInsertComplexBlendStepsAsPaths(TRUE);		// compound nodes MUST be inserted as paths!
	ParamBecomeA.SetResultsStayInPlace(!bCombineEffectBitmaps);

	// if we have a brush attribute we have to do it independently (ugh)
	AttrBrushType* pBrush = NULL;
	((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), (NodeAttribute **)&pBrush);
	BOOL gotAValidBrush = FALSE;
	if (pBrush != NULL && pBrush->GetBrushHandle() != BrushHandle_NoBrush)
	{
		gotAValidBrush = TRUE;

		BOOL useBrush = FALSE;			// CGS:  decide whether to use the brush for DoBecomeA or pNode ....
		
		if (Reason == COMBINE_ADD)
		{
			useBrush = TRUE;
		}
		/*else					// put this back in if you want the slicing of lines to be sliced accurately in two.
								// if you want two new lines with brushes applied, leave like this
		{
			if (IS_A (pNode, NodePath))
			{
				NodePath* pPath = (NodePath*) pNode;
				if (!pPath->InkPath.IsClosed ())
				{
					useBrush = TRUE;				// its a nodepath thats filled
				}
			}
		}*/

		if (useBrush)
		{
			return (pBrush->DoBecomeA(&ParamBecomeA, (NodeRenderableInk*)pNode));
		}
	}

	// if we haven't got a brush then see if we have a stroke
	BOOL ok = TRUE;
	AttrStrokeType* pStroke = NULL;
	((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrStrokeType), (NodeAttribute**)&pStroke);
	AttrVariableWidth* pVarWidth = NULL;
	((NodeRenderableInk*)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth), (NodeAttribute**)&pVarWidth);
	
	if (pStroke && pStroke->HasPathProcessor() && pVarWidth && pVarWidth->HasActiveValueFunction())
	{
		if (!gotAValidBrush)
		{
			// Convert stroke to paths first, allow it to place any nodes it creates
			// after the original but without deleting the original
			if (pStroke)
			{
				ParamBecomeA.SetSecondary();					// Tell the pathprocessor that it doesn't own the node
				ok = pStroke->DoBecomeA(&ParamBecomeA, pNode);
				ParamBecomeA.SetSecondary(FALSE);
			}

			// Now convert the node to paths, placing nodes after the original (before stroke paths)
			// and deleting the original
			if (ok)
				ok = pNode->DoBecomeA(&ParamBecomeA);		// Attributes will be shuffled

			return ok;
		}
		else
		{
			// We've got a brush and a stroke on the same node!
			// The brush already knows about the stroke, so let that handle it ....
			//
			// Convert stroke to paths first, allow it to place any nodes it creates
			// after the original but without deleting the original
			if (pBrush)
			{
				ParamBecomeA.SetSecondary();					// Tell the pathprocessor that it doesn't own the node
				ok = pBrush->DoBecomeA(&ParamBecomeA, (NodeRenderableInk*)pNode);
				ParamBecomeA.SetSecondary(FALSE);
			}

			// Now convert the node to paths, placing nodes after the original (before stroke paths)
			// and deleting the original
			if (ok)
				ok = pNode->DoBecomeA(&ParamBecomeA);		// Attributes will be shuffled

			return ok;
		}
	}

	// Get pNode to generate its paths for us
	return (pNode->DoBecomeA(&ParamBecomeA));
}

/********************************************************************************************

> BOOL OpCombineShapes::DoHideListedNodes(BOOL bCombineEffectBitmaps)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This hides all the nodes that were selected when the op was invoked.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::DoHideListedNodes(BOOL bCombineEffectBitmaps)
{
	// Get the list of paths produced by the first selected object
	SelObjPathList* pSelObjPathList = GetFirstList();
	BOOL ok = TRUE;

	while (pSelObjPathList != NULL)
	{
		if (bCombineEffectBitmaps)
		{
			Node* pSelNode = pSelObjPathList->GetSelNode();
			ERROR3IF(pSelNode == NULL,"pSelNode == NULL");
			if (pSelNode == NULL) return FALSE;

  			if (pSelNode->IsAnObject())
  			{
				// Invalidate the region
	  			if (!DoInvalidateNodeRegion((NodeRenderableInk*)pSelNode,TRUE,FALSE))
					return FALSE;

				if (pSelObjPathList->HasProducedPaths())
				{
					// Hide the node that generated the list

						if (!DoHideNode(pSelNode,TRUE))	return FALSE;
				}
				else
					pSelNode->SetSelected(FALSE);		// If it didn't contribute, just deselect it
			}
		}
		else
		{
			// Hide the individual path-producing nodes, leaving the
			// tree structure around them intact
			//
			// Get the first path in this list
			SelObjPathListItem* pFirstItem = pSelObjPathList->GetFirstItem();
			SelObjPathListItem* pListItem = pFirstItem;

			while (ok && pListItem != NULL)
			{
				// Always hide the creator node
				NodeRenderableInk* pCreator = pListItem->GetCreatedByNode();
				Node* pParent = pCreator->FindParent();
				if (pParent)
				{
					if (ok) ok = DoInvalidateNodeRegion(pCreator, TRUE, FALSE);
					if (ok) ok = DoHideNode(pCreator, TRUE);
				}

				// Hide the created path node if its in the tree
				NodePath* pPathNode = pListItem->GetNodePath();
				if (pPathNode->FindParent())
				{
					if (pParent==NULL)
						pParent = pPathNode->FindParent();
					if (ok) ok = DoInvalidateNodeRegion(pPathNode, TRUE, FALSE);
					if (ok) ok = DoHideNode(pPathNode, TRUE);
				}

				// If this hide has resulted in our parent becoming empty
				// Then hide that too...
				while (pParent && pParent->IsCompoundClass() && !((NodeCompound*)pParent)->HasVisibleChildren())
				{
					Node* pNextParent = pParent->FindParent();

					if (ok) ok = DoInvalidateNodeRegion((NodeCompound*)pParent, TRUE, FALSE);
					if (ok) ok = DoHideNode((NodeCompound*)pParent, TRUE);

					pParent = pNextParent;
				}

				// Get the next item in the selected object path list.
				pListItem = pSelObjPathList->GetNextItem(pListItem);
			}
		}

		// Get the next list of paths generated by the next selected object
		pSelObjPathList = GetNextList(pSelObjPathList);
	}

	// it all went well
	return ok;
}




/********************************************************************************************

> BOOL OpCombineShapes::DoSelectResultNodes(BOOL bCombineEffectBitmaps)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/04/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This selects all the nodes in the results list.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::DoSelectResultNodes(BOOL bCombineEffectBitmaps)
{
	// Get the list of paths produced by the first selected object
	BOOL ok = TRUE;

	NodeRenderableInk::DeselectAll(FALSE, FALSE);

	Node* pNode = pListOfResults->FindFirst();
	while (pNode)
	{
		if (pNode->IsNodeRenderableClass())
			((NodeRenderable*)pNode)->Select(FALSE);

		pNode = pListOfResults->FindNext(pNode);
	}

	// it all went well
	return ok;
}




/********************************************************************************************

> BOOL OpCombineShapes::ApplyAttributes(NodeRenderableInk* pSrcNode,NodeRenderableInk* pDestNode,CCAttrMap* pAttrMap, SelObjPathListItem* pListItem = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		pSrcNode	= ptr to node to get attrs from
				pDestNode	= ptr to node to apply pSrcNode's attrs to
				pAttrMap	= ptr to attr map (NULL means get attrs applied to pSrcNode)
				pListItem	= so that the can control processing based upon the values of this
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This applies attrs pDestNode.
				If pAttrMap == NULL, the attrs applied to pSrcNode are applied to pDestNode, else
				the attrs in pAttrMap are applied.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::ApplyAttributes(NodeRenderableInk* pSrcNode,NodeRenderableInk* pDestNode,CCAttrMap* pAttrMap, SelObjPathListItem* pListItem /*= NULL*/)
{
	ERROR3IF(pSrcNode  == NULL,"pSrcNode == NULL");
	ERROR3IF(pDestNode == NULL,"pDestNode == NULL");
	if (pSrcNode == NULL || pDestNode == NULL) return FALSE;

	BOOL ok = TRUE;				// Set to TRUE if we successfully apply attrs to pDestNode
	BOOL DeleteAttrs = FALSE;	// TRUE if we need to delete attr map before returning


	if (pAttrMap == NULL)
	{
		// Find the currently applied attributes and make a local copy of the map (like most
		// passback functions do)
		CCAttrMap AppliedMap(30);
		ok = pSrcNode->FindAppliedAttributes(&AppliedMap);
		pAttrMap = AppliedMap.Copy();
		DeleteAttrs = TRUE;
	}

	// if we have a stroked object we need to do some attribute juggling
	if (AdjustAttributesForStroke(pAttrMap, pListItem) == 2)
		return FALSE;

	// If all's OK, apply attrs to pDestNode
	ok = pDestNode->ApplyAttributes(pAttrMap,TRUE);

	// Delete the copied attr map if we have to
	if (DeleteAttrs)
	{
		pAttrMap->DeleteAttributes();
		delete pAttrMap;
	}

	return ok;
}


/********************************************************************************************

> UINT32 OpCombineShapes::AdjustAttributesForStroke(CCAttrMap* pAttrMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/2000
	Inputs:		pAttrMap - the attribute map that we are about to apply to our combined node
	Outputs:	swaps a few things around in the map
	Returns:	0 - if we do not have an active stroke attribute (therefore do nothing)
				1 - if we do have an active stroke and everything went well
				2 - if we have an active stroke and something went wrong (e.g. out of memory)
	Purpose:	If we are doing combine shapes on a stroked shape then we need to make the following
				changes:
				1) Do not keep the AttrStrokeType and VariableWidth around, this is because their
				effects have been achieved via the DoBecomeA so if we keep them then effectively
				we are stroking our final output twice.
				2) Set line width to zero.  Once again the line width effect has been extracted 
				from the DoBecomeA
				3) Use the current stroke colour for the new fill colour.
				4) Ditch the stroke colour?

				We must actually make new nodes for these attributes as changing the existing
				ones will mess up our undo.  These attributes must be deleted after they are 
				applied by calling DeleteExtraAttributesForStroke()
	Errors:		-
	SeeAlso:	-
	Notes:		Phil, 16/11/2005
				We know that we own the attributes referred to by the Atrtibute Map so we
				can party on them without worrying about affecting other data structures.
				This allows us to remove attributes from the map completely, leaving it in
				a consistent state.

********************************************************************************************/

UINT32 OpCombineShapes::AdjustAttributesForStroke(CCAttrMap* pAttrMap, SelObjPathListItem* pListItem /*= NULL*/)
{
	if (pAttrMap == NULL)
		return 0;

	// first see if we have an active stroke, if not we can just quit already
	AttrStrokeType* pStroke = NULL;
	pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrStrokeType), (void*&)pStroke);
	
	if (pStroke == NULL || !pStroke->HasPathProcessor())
		return 0;

	AttrVariableWidth* pVarWidth = NULL;
	pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrVariableWidth), (void*&)pVarWidth);
	
	// CGS:  we should only really continue IF we have an active value function!  Otherwise, we seem to run
	// into problems when we have give new objects most recent attributes turned on ....
	if ((pVarWidth == NULL) || !pVarWidth->HasActiveValueFunction ())
		return (0);

	// right so we want to set the line width to zero
	AttrLineWidth* pLineWidth = new AttrLineWidth;
	if (pLineWidth != NULL)
	{
		pAttrMap->RemoveAttribute(CC_RUNTIME_CLASS(AttrLineWidth));
		pLineWidth->Value.LineWidth = 0;
		pAttrMap->SetAt(CC_RUNTIME_CLASS(AttrLineWidth), (void*)pLineWidth);
	}

	// now get the stroke colour and use it to make a fill colour
	AttrStrokeColour * pColour = NULL;

	// look up the stroke colour attribute			
	pAttrMap->Lookup(CC_RUNTIME_CLASS(AttrStrokeColour),
						(void *&)pColour);
	

	// make a new flat fill attribute and apply this to the node
	if (pColour)
	{
		if (!pListItem || pListItem->GetStrokeCreatedPassbackPath ())
		{
			StrokeColourAttribute * pAttrVal = (StrokeColourAttribute *)pColour->GetAttributeValue();

			if (pAttrVal)
			{
				AttrFlatColourFill* pFill = new AttrFlatColourFill;
				
				if (pFill)
				{
					DocColour Col = pAttrVal->Colour;
					pFill->SetStartColour(&Col);
					
					// set back in the map
					pAttrMap->RemoveAttribute(CC_RUNTIME_CLASS(AttrFillGeometry));
					pAttrMap->SetAt(CC_RUNTIME_CLASS(AttrFillGeometry), (void*)pFill);
				}
				else
					return 2;
			}
		}
		// now make a new stroke colour and set it transparent 		
		AttrStrokeColour* pNewStrokeCol = new AttrStrokeColour;
		if (pNewStrokeCol)
		{
			pNewStrokeCol->Value.Colour = DocColour(COLOUR_NONE);
			pAttrMap->RemoveAttribute(CC_RUNTIME_CLASS(AttrStrokeColour));
			pAttrMap->SetAt(CC_RUNTIME_CLASS(AttrStrokeColour), (void*)pNewStrokeCol);
		}
	}

	// now remove the stroke and var width attributes, and any brushes whilst we're here
	pAttrMap->RemoveAttribute(CC_RUNTIME_CLASS(AttrVariableWidth));
	pAttrMap->RemoveAttribute(CC_RUNTIME_CLASS(AttrStrokeType));
	pAttrMap->RemoveAttribute(CC_RUNTIME_CLASS(AttrBrushType));

	return 1;
}


/********************************************************************************************

>	void OpCombineShapes::DeleteExtraStrokeAttributes(CCAttrMap* pAttrMap)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/9/2000
	Inputs:		pAttrMap - the attrmap that we generated to apply to the new combined shapes
	Outputs:	some attributes are deleted from the map
	Returns:	-
	Purpose:	To delete the extra attributes we allocated in AdjustAttributesForStroke
	SeeAlso:	-

********************************************************************************************/

/*void OpCombineShapes::DeleteExtraStrokeAttributes(CCAttrMap* pAttrMap, SelObjPathListItem* pListItem)
{
	if (pAttrMap == NULL)
	{
		ERROR3("Attribute map is NULL in OpCombineShapes::DeleteExtraStrokeAttributes");
		return;
	}

	// Just look em up and wipe em out
	AttrLineWidth* pLineWidth = NULL;
	pAttrMap->Lookup((void*)CC_RUNTIME_CLASS(AttrLineWidth), (void*&)pLineWidth);
	if (pLineWidth != NULL)
	{
TRACEUSER( "Phil", _T("DESA Removing line width %x\n"), pLineWidth);
		delete pLineWidth;
		pLineWidth = NULL;

		pAttrMap->RemoveKey((void*)CC_RUNTIME_CLASS(AttrLineWidth));
	}

	AttrStrokeColour* pStrokeCol = NULL;
	pAttrMap->Lookup((void*)CC_RUNTIME_CLASS(AttrStrokeColour), (void*&)pStrokeCol);
	if (pStrokeCol != NULL)
	{
TRACEUSER( "Phil", _T("DESA Removing stroke colour %x\n"), pStrokeCol);
		delete pStrokeCol;
		pStrokeCol = NULL;

		pAttrMap->RemoveKey((void*)CC_RUNTIME_CLASS(AttrStrokeColour));
	}

	if (!pListItem || pListItem->GetStrokeCreatedPassbackPath ())
	{	
		AttrFillGeometry* pFill = NULL;
		pAttrMap->Lookup((void*)CC_RUNTIME_CLASS(AttrFillGeometry), (void*&)pFill);
		if (pFill)
		{
TRACEUSER( "Phil", _T("DESA Removing fill %x\n"), pFill);
			delete pFill;
			pFill = NULL;

			pAttrMap->RemoveKey((void*)CC_RUNTIME_CLASS(AttrFillGeometry));
		}
	}

	return;
}
*/


/********************************************************************************************

> BOOL OpCombineShapes::ApplyEffects(SelObjPathListItem* pListItem, NodeRenderableInk* pDestNode, NodeRenderableInk* pParentEffect)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/02/2005
	Inputs:		pListItem	= so that the can control processing based upon the values of this
				pDestNode	= ptr to node to apply pSrcNode's attrs to
	Outputs:	-
	Returns:	TRUE if OK, FALSE if failed
	Purpose:	This applies attrs pDestNode.
				If pAttrMap == NULL, the attrs applied to pSrcNode are applied to pDestNode, else
				the attrs in pAttrMap are applied.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::ApplyEffects(SelObjPathListItem* pListItem, NodeRenderableInk* pDestNode, NodeRenderableInk* pParentEffect)
{
	BOOL ok = TRUE;
	PORTNOTE("other", "Removed OpCombineShapes::ApplyEffects");
#ifndef EXCLUDE_FROM_XARALX

	ERROR3IF(pListItem  == NULL, "pListItem == NULL");
	ERROR3IF(pDestNode == NULL, "pDestNode == NULL");

	NodeRenderableInk* pSrcNode = pListItem->GetCreatedByNode();
	ERROR3IF(pSrcNode  == NULL, "pSrcNode == NULL");

	// Get the effect stack for the object which produced this path (pSrcNode)
	// If the source node had effects applied to it
	// And that stack is different than the stack above the context node
	// Then we must copy the effect stack onto the destination node
	ListRange* pStack = pListItem->GetEffectStack();
	Node* pEffect = NULL;
	if (pStack)
		pEffect = pStack->FindFirst();
	if (pEffect)
	{
		if (pParentEffect==NULL || pParentEffect!=pEffect)
		{
			// We need to copy the effect stack onto this object
			OpLiveEffect::DoCopyEffectsStack(this, pDestNode, pStack, GetWorkingDoc(), GetWorkingDoc());
		}
	}
#endif
	return ok;
}


/********************************************************************************************

>	static BOOL OpCombineShapes::GetSubpath(Path& Src,Path* const pDest,INT32* pStartIndex)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Inputs:		Src 		= source path
				pDest 		= ptr to place to put result
				pStartIndex = index to start looking for sub paths in Src (0 = first subpath)
	Outputs:	*pStartIndex is updated to be the start of the next subpath (-1 means there are no more)
	Returns:	TRUE if all went well, FALSE otherwise
	Purpose:	This picks out a subpath from Src and places it into pDest
	SeeAlso:	-

********************************************************************************************/
/*
BOOL OpCombineShapes::GetSubpath(Path& Src,Path* const pDest,INT32* pStartIndex)
{
	ERROR2IF(pDest == NULL,FALSE,"pDest == NULL");
	ERROR2IF(pStartIndex == NULL,FALSE,"pDest == NULL");

	INT32 StartIndex = *pStartIndex;
	INT32 NumCoords = Src.GetNumCoords();
	if (StartIndex < 0 || StartIndex >= NumCoords)
		return FALSE;

	// Get the start and end indexes for the current subpath
	INT32 EndIndex=StartIndex;
	Src.FindEndElOfSubPath(&EndIndex);
	ERROR2IF(EndIndex >= NumCoords,FALSE,"EndIndex gone beyond num coords in path");

	if (!pDest->MakeSpaceInPath((EndIndex-StartIndex)+1))
		return FALSE;

	// Copy the subpath into destination path
	if (!Src.CopySectionTo(pDest,StartIndex,(EndIndex-StartIndex)+1))
		return FALSE;

	// Make sure the filled and stroked flags are correct
	pDest->IsFilled  = Src.IsFilled;
	pDest->IsStroked = Src.IsStroked;

	INT32 NextStartIndex = EndIndex+1;
	if (NextStartIndex >= NumCoords)
		NextStartIndex = -1;

	*pStartIndex = NextStartIndex;

	return (TRUE);
}
*/

/********************************************************************************************

>	BOOL OpCombineShapes::StrokePathToPath(Path* pPath,Path* pDest)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		pPath = ptr to path
				pDest = ptr to path to place result in
	Outputs:	-
	Returns:	TRUE if OK, FALSE if it goes bang
	Purpose:	Strokes pPath, and places result in pDest
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::StrokePathToPath(Path* pPath,Path* pDest)
{
	ERROR2IF(pPath == NULL,FALSE,"pPath == NULL");
	ERROR2IF(pDest == NULL,FALSE,"pDest == NULL");

	return pPath->StrokePathToPath(DefaultStrokeWidth,LineCapButt,MitreJoin,NULL,pDest,200);

 /*
	INT32 NumCoords = pPath->GetNumCoords();
	INT32 OutputSize = (NumCoords*40)+1000;
	Path OutputPath;
	if (!OutputPath.Initialise(OutputSize)) return FALSE;

	DocCoord*	IPoints = 			pPath->GetCoordArray();
	BYTE*		ITypes	= (BYTE*) 	pPath->GetVerbArray();
	DWORD		ILength = (DWORD)  	NumCoords;

	DocCoord* 	OPoints = 			OutputPath.GetCoordArray();
	BYTE*		OTypes	= (BYTE*) 	OutputPath.GetVerbArray();
	DWORD		OLength = (DWORD)  	OutputSize;

	DashType Dash;
	Dash.Length = 0;

	INT32 len = GRenderRegion::StrokePathToPath(IPoints,ITypes,ILength,
									OPoints,OTypes,OLength,
									FALSE,(TOLERANCE*5)/2,200,LineCapButt,MitreJoin,&Dash);

	if (!OutputPath.MergeTwoPaths(OutputPath.GetCoordArray(),OutputPath.GetVerbArray(),OutputPath.GetFlagArray(),
								len,TRUE)) return FALSE;

	if (len > 0)
	{
		pDest->ClearPath(FALSE);
		if (!pDest->MakeSpaceInPath(len)) return FALSE;
		if (!pDest->MergeTwoPaths(	OutputPath.GetCoordArray(),OutputPath.GetVerbArray(),OutputPath.GetFlagArray(),
									len,TRUE)) return FALSE;

		pDest->InitialiseFlags();
		RemoveBogusCloseFigures(pDest);
		INT32 n = pDest->GetNumCoords();
		PathVerb* pVerbs = pDest->GetVerbArray();
		pVerbs[n-1] |= PT_CLOSEFIGURE;
	}

	return TRUE;
*/
}

/********************************************************************************************

>	BOOL OpCombineShapes::DoInsertNewNode(NodeRenderableBounded* pNewNode)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/1/95
	Inputs:		pNewNode = ptr to the node to insert
	Outputs:	-
	Returns:	TRUE if OK, FALSE if it goes bang
	Purpose:	High-level interface to UndoableOperation::DoInsertNode().

				Every time this call is successful, it incs a counter so that the op can
				tell how many new nodes have been placed in the tree as a result of the shape combining
				operation.

				Uses member vars pContextNode & AttachDir

				If no new nodes are created, the op can then fail, rather than just giving the effect of deleting
				the selection
	SeeAlso:	-

********************************************************************************************/

BOOL OpCombineShapes::DoInsertNewNode(NodeRenderableBounded* pNewNode)
{
	ERROR2IF(pNewNode == NULL,FALSE,"Given NULL node ptr");
	ERROR2IF(pContextNode == NULL,FALSE,"Context node is NULL");

	if (UndoableOperation::DoInsertNewNode(pNewNode,pContextNode,AttachDir,TRUE,FALSE,FALSE,TRUE))
	{
		NodeInsertCount++;
		return TRUE;
	}
	else
		return FALSE;
}




/********************************************************************************************

>	ListRange* OpCombineShapes::CopyRange(Range* pRange)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/05/2005
	Inputs:		pRange = ptr to the range to copy
	Outputs:	-
	Returns:	ptr to ListRange if OK,
				NULL otherwise
	Purpose:	Copy the specified range of nodes
	SeeAlso:	-

********************************************************************************************/

ListRange* OpCombineShapes::CopyRange(Range* pRange)
{
	BOOL ok = TRUE;
	Node* pNode = pRange->FindFirst();
	ListRange* pNewRange = new ListRange();

	while (pNode && ok && pNewRange)
	{
		Node* pNext = pRange->FindNext(pNode);

		// Make a copy of the current node and all its children
		Node* TheCopy;
		BOOL CopiedOK; 
		CALL_WITH_FAIL(pNode->NodeCopy(&TheCopy), this, CopiedOK);
		if (CopiedOK)
		{
			// make sure that it is bounded
			ERROR2IF(!TheCopy->IsBounded(), FALSE, "Object being copied is not a NodeRenderableBounded"); 
			NodeRenderableBounded* BoundCopy = (NodeRenderableBounded*)TheCopy;

			// Insert the copied node next to it's originator
			ok = UndoableOperation::DoInsertNewNode(BoundCopy, pNode, NEXT,
													 FALSE, 	// Don't Invalidate region 
													 FALSE, 	// Don't Clear the selections
													 FALSE,		// Don't select new node
													 FALSE 		// Don't normalise attributes
													 );
			if (!ok)
			{
				// Failed, so tidy up before returning
				TheCopy->CascadeDelete(); 
				delete TheCopy; 
			} 
			else
			{
				// Record the copied node in the ListRange
				pNewRange->AddNode(TheCopy);

				// Call PostDuplicate on the copied node and all it's children
				Node* pCurrent = BoundCopy->FindFirstDepthFirst();
				while (pCurrent!=NULL && ok)
				{
					ok = pCurrent->PostDuplicate(this);
					pCurrent = pCurrent->FindNextDepthFirst(BoundCopy);
				}
			}
		}

		pNode = pNext;
	}

	if (!ok)
	{
		delete pNewRange;
		pNewRange = NULL;
	}

	return pNewRange;
}




//----------------------------------------------------
/********************************************************************************************

> BOOL CombineBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		pNewNode 		= ptr to new generated node (should be a NodePath)
				pCreatedByNode 	= ptr to the node that generated this NodePath
				pAttrMap		= ptr attr map (NULL means get attrs applied to pCreatedByNode)
	Outputs:	-
	Returns:	-
	Purpose:	This is the func that receives the NodePaths generated by the selection.
				The constructor ensures that we have a list to add SelObjPathListItems to.
	Errors:		-
	SeeAlso:	CreateSelObjPathList()

********************************************************************************************/

BOOL CombineBecomeA::PassBack(NodeRenderableInk* pNewNode,NodeRenderableInk* pCreatedByNode,CCAttrMap* pAttrMap)
{
	// Do a quick param check
	ERROR2IF(pNewNode == NULL,FALSE,"pNewNode == NULL");
	ERROR2IF(pSelObjPathList == NULL,FALSE,"pSelObjPathList is NULL");
	ERROR2IF(!pNewNode->IsNodePath(),FALSE,"pNewNode not a kind of NodePath");

	// We've received a NodePath from a selected object, so make a sel obj path list item
	// out of it and add it to the list

	SelObjPathListItem* pSelObjPathListItem = new SelObjPathListItem((NodePath*)pNewNode,pCreatedByNode,pAttrMap, strokeCreatedPassbackPath);

	if (pSelObjPathListItem != NULL)
	{
		pSelObjPathList->AddTailItem(pSelObjPathListItem);
		return TRUE;
	}
	else
		return FALSE;

/*
	*** code for splitting the new NodePath into separate subpaths ***

	// Get ptr to src path of new node
	Path* pSrcPath = &(((NodePath*)pNewNode)->InkPath);

	// We've received a NodePath from a selected object, so make sel obj path list items
	// for each of the sub paths contained within it

	Path DestPath;
	BOOL ok = DestPath.Initialise();

	INT32 StartIndex = 0;

	if (ok)
	{
		do
		{
			ok = OpCombineShapes::GetSubpath(*pSrcPath,&DestPath,&StartIndex);
			if (ok)
			{
				NodePath* pNodePath = new NodePath;

				ok = (pNodePath != NULL);

				if (ok)	ok = pNodePath->SetUpPath(DestPath.GetNumCoords());
				if (ok) ok = pNodePath->InkPath.CopyPathDataFrom(&DestPath);

				if (ok)
				{
					SelObjPathListItem* pSelObjPathListItem = new SelObjPathListItem(pNodePath,pCreatedByNode);
					ok = (pSelObjPathListItem != NULL);
					if (ok) pSelObjPathList->AddTailItem(pSelObjPathListItem);
				}

				if (!ok && pNodePath!=NULL)
					delete pNodePath;
			}

		} while (ok && StartIndex >=0);
	}

	if (pNewNode != NULL)
		delete pNewNode;

	return ok;
*/
}

//----------------------------------

/********************************************************************************************

> SelObjPathListItem::SelObjPathListItem(NodePath* pThisNodePath,NodeRenderableInk* pThisCreatedByNode,CCAttrMap* pThisAttrMap, BOOL createdByStroke)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		pThisNodePath 		= the NodePath this item refers to
				pThisCreatedByNode	= the node that created pThisNodePath
				pThisAttrMap		= ptr to attr map (NULL means get applied attrs from pThisCreatedByNode)
				createdByStroke		= was the item created by a stroke?
	Outputs:	-
	Returns:	-
	Purpose:	Main constructor.  Sets up the item correctly
	Errors:		-
	SeeAlso:	

********************************************************************************************/

SelObjPathListItem::SelObjPathListItem(NodePath* pThisNodePath,NodeRenderableInk* pThisCreatedByNode,CCAttrMap* pThisAttrMap, BOOL createdByStroke /*= FALSE*/)
{
	pNodePath 		= pThisNodePath;
	pCreatedByNode	= pThisCreatedByNode;
	pAttrMap		= pThisAttrMap;
	strokeCreatedPassbackPath = createdByStroke;
	pEffectStack	= NULL;
}

/********************************************************************************************

> SelObjPathListItem::~SelObjPathListItem()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/1/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor that deletes the member path pointer, and any attrs supplied
	Errors:		-
	SeeAlso:	

********************************************************************************************/

SelObjPathListItem::~SelObjPathListItem()
{
	// Must delete the NodePaths generated via the DoBecomeA() mechanism
	// Only delete if this node is not in the tree!
	if (pNodePath != NULL && pNodePath!=pCreatedByNode && pNodePath->FindParent()==NULL && pNodePath->GetHiddenCnt()==0)
	{
		pNodePath->CascadeDelete();
		delete pNodePath;
	}

	if (pAttrMap  != NULL)
	{
		// Delete all the attrs in the map
		CCRuntimeClass * pType;
		void* pVal;

		// iterating all (key, value) pairs
		for (CCAttrMap::iterator Pos = pAttrMap->GetStartPosition(); Pos != pAttrMap->GetEndPosition();)
		{
			// Get attr at position Pos
			pAttrMap->GetNextAssoc(Pos,pType,pVal);

			if (pVal != NULL)
			{
				pAttrMap->RemoveKey(pType);
				delete (NodeAttribute*)pVal;
			}
		}

		// delete the actual map
		delete pAttrMap;
	}

	if (pEffectStack)
	{
		delete pEffectStack;
		pEffectStack = NULL;
	}
}

/********************************************************************************************

>	Path* SelObjPathListItem::GetPath()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/1/95
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to the path, or NULL
	Purpose:	Get that Path ptr boy!
	SeeAlso:	-

********************************************************************************************/

Path* SelObjPathListItem::GetPath()
{
	if (pNodePath != NULL) 
		return &(pNodePath->InkPath); 
	else 
		return NULL; 
}

