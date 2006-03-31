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
// Implementation of cut/copy and paste operations 

/*
//*/

#include "camtypes.h" 
#include "cutop.h"

#include "app.h"
#include "clipint.h"
#include "docrect.h"
#include "document.h"
#include "docview.h"
#include "ink.h"
#include "layer.h" 
#include "spread.h"
#include "trans2d.h"
#include "wrkrect.h"
#include "objchge.h"

//#include "clikdrag.h"

//#include "jason.h"
//#include "peter.h"
//#include "simon.h"
#include "tool.h"
#include "attrmgr.h"

#include "textops.h"
#include "nodetxts.h"		// For temporary disabling of text subselection copying/cutting
//#include "barsdlgs.h"		// button controls

#include "fillattr.h"
#include "ndoptmz.h"
#include "progress.h"
#include "transop.h"
//#include "bubbleid.h"

#include "camdoc.h"			// for CCamDoc::GetKernelPathName & CCamDoc::SetPathName

#include "fillramp.h"
#include "opgrad.h"
#include "blobs.h"

#include "cxfdefs.h"		// for TemplateAttribute
#include "cxftags.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "attrval.h"
#include "userattr.h"
#include "tmpltatr.h"

#include "ngsentry.h"		// for NodeSetProperty etc
#include "slicehelper.h"	// for the MeshLayers fn

//#include "will2.h"			// for _R(IDS_FILLTOOL_FILLINFOBARNAME)
#include "filltool.h"		// for GradInfoBarOp
#include "ngitem.h"			// for SGNameItem
#include "units.h"
#include "effects_stack.h"
#include "ophist.h"

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNCREATE(OpCut, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpCopy, UndoableOperation)  
CC_IMPLEMENT_DYNCREATE(OpPaste, SelOperation)  
CC_IMPLEMENT_DYNCREATE(OpDelete, SelOperation)
CC_IMPLEMENT_DYNCREATE(CarbonCopyOp, SelOperation)
CC_IMPLEMENT_DYNCREATE(OpDuplicate, CarbonCopyOp)
CC_IMPLEMENT_DYNCREATE(OpClone, CarbonCopyOp)
CC_IMPLEMENT_DYNCREATE(OpCopyAndTransform, CarbonCopyOp)
CC_IMPLEMENT_DYNCREATE(OpPasteAttributes, SelOperation)
CC_IMPLEMENT_DYNAMIC(OpParamPasteAtPosition,OpParam)


/********************************************************************************************

>	static void SetCutOpText(String_256 *UIDescription, UINT32 PrefixStringID)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95
	Inputs:		UIDescription - points to the UOIDescription for a GetState method
				PrefixStringID - the ID of a prefix to use (e.g. "Cut ")

	Outputs:	UIDescription will either be left unchanged (no special description
				available) or will be filled in (e.g. "Cut lines")

	Purpose:	Shared code used by almost all the GetState methods in kernel cutop.cpp
				Given a prefix string ("Copy "), it scans the selection for a description
				and generates an op description string ("Cut shape")

********************************************************************************************/

static void SetCutOpText(String_256 *UIDescription, UINT32 PrefixStringID)
{
	String_256 Description = GetApplication()->FindSelection()->Describe(MENU);

	if (!Description.IsEmpty())
	{
		*UIDescription = String_256(PrefixStringID);
		*UIDescription += Description;
	}
	// else Leave the UIDescription as it stands (probably a simple "Cut" etc)
}



// ------------------------------------------------------------------------------------------
// OpCut methods
            
/********************************************************************************************

>	OpCut::OpCut() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpCut constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpCut::OpCut(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpCut::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpCut initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpCut::Init()
{
	return (RegisterOpDescriptor(0,
 							_R(IDS_CUTOP),
							CC_RUNTIME_CLASS(OpCut),
 							OPTOKEN_CUT,
 							OpCut::GetState,
 							0,							// help ID 
 							_R(IDBBL_CUT),
 							0,
							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							0 //(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
	));
}               
    
/********************************************************************************************

>	OpState	OpCut::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpCut
	Purpose:	For finding OpCut's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpCut::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	// BODGE Temporarily disable copying TextStory sub-selections
/*
	Range Sel(*(GetApplication()->FindSelection()));
	Node* Current = Sel.FindFirst();
	while(Current != NULL)
	{
		if (Current->IS_KIND_OF(BaseTextClass))
		{
			if (!(IS_A(Current, TextStory)))
			{
				OpSt.Greyed = TRUE;
				return(OpSt);
			}
		}
		Current = Sel.FindNext(Current, TRUE); 
	}
*/

	SelRange* pSelRange = GetApplication()->FindSelection();	// get the selected range

	// Set up the ObjChangeParam so we can ask the selected nodes if they minds being cut
	ObjChangeFlags cFlags(TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	// Will one or more selected nodes allow this op?
	if (!pSelRange->AllowOp(&ObjChange,FALSE))
	{
		// None of the nodes can be deleted
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		UINT32 IDS = ObjChange.GetReasonForDenial();
		if (IDS == 0) IDS = _R(IDS_NO_OBJECTS_SELECTED);	// if 0 (i.e. not been set), then assume there's no selection
	   	*UIDescription = String_256(IDS);				// Resolve the string ID
	}
	else
		SetCutOpText(UIDescription, _R(IDS_CLIPBOARD_PRECUT));

	return(OpSt);
}

/********************************************************************************************

>	void OpCut::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Cut operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpCut::Do(OpDescriptor*)
{  
	ObjectSet CompoundSet;

	BeginSlowJob(-1, FALSE);		// Make sure we have a simple hourglass, without a delay

	// Check with the clipboard if it is OK to go ahead
	InternalClipboard *Clipboard = InternalClipboard::Instance();
	if (Clipboard == NULL || !Clipboard->PrepareForCopy())
	{
		FailAndExecute();

		EndSlowJob();
		End();
		return;
	}

	// Obtain the current selection
	Range Sel(*(GetApplication()->FindSelection()));
	
	// DMc change to range control
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);


	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags(TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	if (!DoStartSelOp(TRUE,FALSE))  // Try to record the selection state
		goto EndOperation;  
	
	// Mark nodes that will allow this to happen, and error if no nodes will let it happen
	if (!Sel.AllowOp(&ObjChange))
	{
		ERROR3("AllowOp() returned FALSE, i.e. op should have been greyed out");
		goto EndOperation;
	}

	
	// We need to invalidate the region
	if (!DoInvalidateNodesRegions(Sel, TRUE))
		goto EndOperation; 

	// Prior to hiding anything lets get ourselves a set of compound nodes which 
	// we will need to factor out attributes on. We cannot use the SelRange because
	// this will be empty after the deletion has taken place. 

	if (!(Sel.GetCompoundObjectSet(&CompoundSet, 
									TRUE )))
	{
		goto EndOperation; 
	}
	
	// Removed 24/07/95 (We now leave it up to the text story deletion code to handle its own attributes)

	// Because of complex deletion we must also add the TextStory objects 
	// of selected chars. This code will hopefuly be removed in future. It is
	// inefficient cos we will need to factor out more than once for each line !.
	
	//if (!(Sel.AddTextStoryCompoundsForDel(&CompoundSet)))
	//{
	//	goto EndOperation;
	//} 

	// We now must localise the attributes on every item marked for deletion
	if (!DoLocaliseForAttrChange(&Sel, (AttrTypeSet*)NULL, TRUE ))
	{
		goto EndOperation;
	}


   	// Copy the selection to the internal clipboard
	if (!DoCopyNodesToClipboard(Sel))
		goto EndOperation; 

	// Go through one pass, asking nodes to hide themselves
	if (!DoHideComplexRange(Sel))
		goto EndOperation;
	{
		SelRange* pRange = GetApplication()->FindSelection();
		if (pRange)
		{
			// freshen the selection range by counting the currently selected objects
			pRange->Update(FALSE,NULL);
//			INT32 num = pRange->Count();
			pRange->Count();	// side-effect?
		
			// Obtain the current selection (should be up-to-date now!)
			Range NewSel(*pRange);

			// Now go through a second pass snip the remainder out of the tree
			if (!DoHideNodes(NewSel,TRUE))	 // Hide the nodes (IncludeSubtreeSize)
				goto EndOperation;
		}
	}

	// Finally try to factor out the attributes on all compounds. Note that if a particular compound
	// is hidden then it will  be it's parent that has its attributes factored out !
	if (!DoFactorOutAfterAttrChange(&CompoundSet,(AttrTypeSet*)NULL))
	{
		goto EndOperation; 
	}
	
	// We need to inform the DocComponents of the clipboard that the copy is complete

EndOperation:

	CompoundSet.DeleteAll(); 	// Delete all items in the CompoundSet
		
	// Update all the changed nodes, i.e. tell all the parents of the children that have been effected
	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	// Invalidate sel range as it may have been cached in the middle of the op while the tree is
	// in an invalid state (i.e. some selected nodes may have had the PERMISSION_DENIED state set during caching).
	{
		SelRange* pRange = GetApplication()->FindSelection();
		if (pRange)
			pRange->Update(FALSE,NULL);
	}

	// Copy the doc's file-path etc, so that the clipboard doc knows if it is possible to
	// OLE link to the doc.
	Document* pSourceDoc = Document::GetSelected();
	if (pSourceDoc && !pSourceDoc->GetOilDoc()->GetKernelPathName().IsEmpty())
	{
		// Don't add to the MRU list!
		String_256 str = pSourceDoc->GetOilDoc()->GetKernelPathName();
		InternalClipboard::Instance()->GetOilDoc()->SetPathName(str, FALSE);
		TRACEUSER( "JustinF", _T("Copied file-path %s to clipboard document\n"),
					(LPCTSTR) InternalClipboard::Instance()->GetOilDoc()->GetKernelPathName());
	}
	else
	{
		// Set the path-name to be empty (what a delicious potential bug!)
		InternalClipboard::Instance()->GetOilDoc()->SetPathNameEmpty();
		TRACEUSER( "JustinF", _T("Setting clipboard file-path to nothing\n"));
	}

	// And finally, tell the clipboard we've finished copying to it
	// (This is the point where it is made available to other applications, etc)
	if (!Clipboard->CopyCompleted())
		FailAndExecute();

	EndSlowJob();
	End();
}			


// ------------------------------------------------------------------------------------------
// OpCopy methods
            
/********************************************************************************************

>	OpCopy::OpCopy() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpCopy constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpCopy::OpCopy(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpCopy::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpCopy initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpCopy::Init()
{
	TRACE( _T("Warning - OpCut::Init called\n") );
	return (RegisterOpDescriptor(0,
 							_R(IDS_COPYOP),
							CC_RUNTIME_CLASS(OpCopy),
 							OPTOKEN_COPY,
 							OpCopy::GetState,
 							0,							// help ID
 							_R(IDBBL_COPY),
 							0,							// bitmap ID
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							(DONT_GREY_WHEN_SELECT_INSIDE | GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
							));
}               
    
/********************************************************************************************

>	OpState	OpCopy::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpCopy
	Purpose:	For finding OpCopy's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpCopy::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	// BODGE Temporarily disable copying TextStory sub-selections
	Range Sel(*(GetApplication()->FindSelection()));
	/*Node* Current =*/ Sel.FindFirst();

	// Set up the ObjChangeParam so we can ask the selected nodes if they minds being copied
	ObjChangeFlags cFlags;
	cFlags.CopyNode=TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	// Will one or more selected nodes allow this op?
	if (!Sel.AllowOp(&ObjChange,FALSE))
	{
		// None of the nodes can be deleted
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		UINT32 IDS = ObjChange.GetReasonForDenial();
		if (IDS == 0) IDS = _R(IDS_NO_OBJECTS_SELECTED);	// if 0 (i.e. not been set), then assume there's no selection
	   	*UIDescription = String_256(IDS);				// Resolve the string ID
	}
	else
		SetCutOpText(UIDescription, _R(IDS_CLIPBOARD_PRECOPY));

	return(OpSt);   
}

/********************************************************************************************

>	void OpCopy::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Copy operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpCopy::Do(OpDescriptor*)
{   
	BeginSlowJob(-1, FALSE);		// Make sure we have a simple hourglass, without a delay

	// Check with the clipboard if it is OK to go ahead
	InternalClipboard *Clipboard = InternalClipboard::Instance();
	if (Clipboard == NULL || !Clipboard->PrepareForCopy())
	{
		FailAndExecute();

		EndSlowJob();
		End();
		return;
	}

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));

	// DMc change to range control
	
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);
	
	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags;
	cFlags.CopyNode=TRUE;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst(); 
	
	ENSURE(FirstSelectedNode != NULL, "Called copy operation with no nodes selected"); 
	
	// In the retail build it is best to do nothing if we find there are no selected nodes 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{   
		if (!DoStartSelOp(TRUE,TRUE))  // Try to record the selection state
		{
			goto EndOperation;  
		}

		// Mark nodes that will allow this to happen, and error if no nodes will let it happen
		if (!Sel.AllowOp(&ObjChange))
		{
			ERROR3("AllowOp() returned FALSE, i.e. op should have been greyed out");
			goto EndOperation;
		}
		
	   	// Copy the selection to the internal clipboard
		if (!DoCopyNodesToClipboard(Sel))
		{
			goto EndOperation; 
		}
	}


EndOperation:
	// Update all the changed nodes, i.e. tell all the parents of the children that have been effected
	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	// Copy the doc's file-path etc, so that the clipboard doc knows if it is possible to
	// OLE link to the doc.
	Document* pSourceDoc = Document::GetSelected();
	if (pSourceDoc && !pSourceDoc->GetOilDoc()->GetKernelPathName().IsEmpty())
	{
		// Don't add to the MRU list!
		String_256 str = pSourceDoc->GetOilDoc()->GetKernelPathName();
		InternalClipboard::Instance()->GetOilDoc()->SetPathName(str, FALSE);
		TRACEUSER( "JustinF", _T("Copied file-path %s to clipboard document\n"),
					(LPCTSTR) InternalClipboard::Instance()->GetOilDoc()->GetKernelPathName());
	}
	else
	{
		// Set the path-name to be empty (what a delicious potential bug!)
		InternalClipboard::Instance()->GetOilDoc()->SetPathNameEmpty();
		TRACEUSER( "JustinF", _T("Setting clipboard file-path to nothing\n"));
	}

	// And finally, tell the clipboard we've finished copying to it
	// (This is the point where it is made available to other applications, etc)
	if (!Clipboard->CopyCompleted())
		FailAndExecute();

	EndSlowJob();
 	End();
}			


// ------------------------------------------------------------------------------------------
// OpPaste methods
            
/********************************************************************************************

>	OpPaste::OpPaste() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpPaste constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpPaste::OpPaste(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpPaste::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPaste initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPaste::Init()
{
	BOOL ok =  (RegisterOpDescriptor(0,
 							_R(IDS_PASTEOP),
							CC_RUNTIME_CLASS(OpPaste),
 							OPTOKEN_PASTE,
 							OpPaste::GetState,
 							0,							// help ID
 							_R(IDBBL_PASTE),
 							0,							// bitmap ID 
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
							));
	if (ok)
	{
		// Register a second OpDescriptor, for paste at same position. This OpDescriptor
		// will probably never appear directly on a bar/menu so it uses the same strings
		// as the above.
		ok =  (RegisterOpDescriptor(0,
 							_R(IDS_PASTEATSAMEPOSOP),
							CC_RUNTIME_CLASS(OpPaste),
 							OPTOKEN_PASTEATSAMEPOS,
 							OpPaste::GetState,
 							0,							// help ID
 							_R(IDBBL_PASTEATSAMEPOS),
							_R(IDD_BARCONTROLSTORE),		// resource ID
							_R(IDC_PASTEATSAMEPOS),			// control ID
							SYSTEMBAR_EDIT,				// Bar ID
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
							));
	}

	return ok;
}               
    
/********************************************************************************************

>	OpState	OpPaste::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPaste
	Purpose:	For finding OpPaste's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpPaste::GetState(String_256* UIDescription, OpDescriptor* pOpDesc)
{
	OpState OpSt;

	if (InternalClipboard::IsEmpty())
	{
		// There is no data to paste, so grey out, and return a reason for greying
		OpSt.Greyed = TRUE;
	   	*UIDescription = String_256(_R(IDS_CLIPBOARD_EMPTY));
	}
	else
	{
		// Otherwise, determine a string to display - always show "Paste", but if possible
		// add a description, e.g. "Paste quickshape", "Paste bitmap"

		String_64 Type;
		InternalClipboard::DescribeContents(&Type);
		if (!Type.IsEmpty() && pOpDesc->Token==String_256(OPTOKEN_PASTE))
		{
			*UIDescription = String_256(_R(IDS_CLIPBOARD_PREPASTE));
			*UIDescription += Type;
		}
	}
	return(OpSt);
}



/********************************************************************************************

>	void OpPaste::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Paste operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpPaste::Do(OpDescriptor* WhichOp)
{
	DoPaste(WhichOp,NULL,NULL);
}			

/********************************************************************************************

>	void OpPaste::DoWithParam(OpDescriptor* WhichOp, OpParam* pOpParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/96
	Inputs:		WhichOp = ptr to op descriptor
				pOpParam = ptr to the params (must be an OpParamPasteAtPosition object)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Paste operation using the give param object.  This mechanism
				is used by the "paste at position" op, utilised by the OLE drag & drop system.

				pOpParam should be a ptr to an OpParamPasteAtPosition, so that the spread 
				and centre coord of the paste can be passed in.

				If pOpParam is not an OpParamPasteAtPosition, the op fails
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpPaste::DoWithParam(OpDescriptor* WhichOp, OpParam* pOpParam)
{
	if (pOpParam != NULL && IS_A(pOpParam,OpParamPasteAtPosition))
	{
		OpParamPasteAtPosition* pPasteParam = (OpParamPasteAtPosition*)pOpParam;

		Spread* pSpread = pPasteParam->GetSpread();
		DocCoord Centre = pPasteParam->GetCentre();

		DoPaste(WhichOp,pSpread,&Centre);
	}
	else
	{
		FailAndExecute();
		End();
	}
}

/********************************************************************************************

>	void OpPaste::DoPaste(OpDescriptor* WhichOp,Spread* pSpread, DocCoord* pCentre)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/96
	Inputs:		WhichOp = ptr to op descriptor
				pSpread = ptr to spread to paste objects into (can be NULL)
				pCentre = coord of centre point of paste destination (can be NULL if pSpread is NULL)
	Outputs:	-
	Returns:	-
	Purpose:	Helper function for Do() & DoWithParam(), which in turn interfaces with
				DoPasteStandard() (don't look at me - I didn't design this stuff)
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpPaste::DoPaste(OpDescriptor* WhichOp,Spread* pSpread, DocCoord* pCentre)
{
	BeginSlowJob(-1, FALSE);		// Make sure we have a simple hourglass, without a delay

	// Check with the clipboard if it is OK to go ahead. If the clipboard contents are
	// currently external, this means it'll have to import them into the InternalClipboard
	// before we can actually copy them into our document.
	InternalClipboard *Clipboard = InternalClipboard::Instance();
	BOOL ExternalObjects;
	BOOL ok = (Clipboard!=NULL && Clipboard->PrepareForPaste(&ExternalObjects));

	if (ok)
	{
		// We paste at the same position if the operation was invoked via the PASTEATSAMEPOS OpDescriptor
		// and the stuff we are pasting has not come from an external source.
		BOOL PasteAtSamePos = (((WhichOp->Token) == String(OPTOKEN_PASTEATSAMEPOS)) && (!ExternalObjects));
		ok = DoPasteStandard(PasteAtSamePos, ExternalObjects, pSpread, pCentre);
	}

	if (ok)
		ok = Clipboard->PasteCompleted();

	if (!ok)
		FailAndExecute();

	EndSlowJob();
	End();
}			

/********************************************************************************************

>	BOOL OpPaste::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Inputs:		-
	Outputs:	Spread:  The spread to place the clipboard objects on
				Position:The centre of the view (Spread coords)

	Returns:	FALSE if it failed. In this case, Spread is returned containing a NULL, and
				Position is returned as DocCoord(0,0)

	Purpose:	Finds the centre insertion position for clipboard objects
	Errors:		-
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL OpPaste::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)
{
	// Let's chuck in a smattering of defensive programming
	ERROR3IF(Spread == NULL || Position == NULL, "Illegal NULL params");
	*Spread = NULL;
	*Position = DocCoord(0,0);	// A default value if we fail

	// ---------------------------------------------------------------------------------
	// Find out which spread is in the centre of the view 
	// this is the spread that the pasted objects will be placed on

	// Obtain the current DocView
	DocView* CurDocView = DocView::GetCurrent();

	ENSURE(CurDocView != NULL, "The current DocView is NULL"); 
	if (CurDocView == NULL)
	{
		return FALSE; // No DocView
	}

	// Get the view rect
	WorkRect WrkViewRect = CurDocView->GetViewRect();

	if (WrkViewRect.IsEmpty() || (!WrkViewRect.IsValid()) )
	{
		return FALSE; // Defensive
	}
	
	// Determine the centre of the view
	WorkCoord WrkCentreOfView; 
	WrkCentreOfView.x = WrkViewRect.lo.x	+ (WrkViewRect.Width()/2); 
	WrkCentreOfView.y = WrkViewRect.lo.y	+ (WrkViewRect.Height()/2);
	
	// FindEnclosing spread requires an OilCoord
	OilCoord OilCentreOfView = WrkCentreOfView.ToOil(CurDocView->GetScrollOffsets()); 

	// Find out which spread to insert the pasteboard objects onto
	(*Spread) = CurDocView->FindEnclosingSpread(OilCentreOfView);
	if ((*Spread) == NULL)
	{
		// There is no spread
		return FALSE; 
	}

	// Phew
	// ---------------------------------------------------------------------------------
	// Now lets find the spread coordinate of the centre of the view
	DocRect DocViewRect = CurDocView->GetDocViewRect(*Spread);
	
	if ( DocViewRect.IsEmpty() || (!DocViewRect.IsValid()) )
	{
		ERROR3("DocViewRect is invalid");
		return FALSE; // Defensive
	}

	// Find the centre of the DocViewRect
   	DocCoord DocCentreOfView; 
	DocCentreOfView.x = DocViewRect.lo.x	+ (DocViewRect.Width()/2); 
	DocCentreOfView.y = DocViewRect.lo.y	+ (DocViewRect.Height()/2);

	// --------------------------------------------------------------------------------
	// Now convert from DocCoords to spread coords
	(*Spread)->DocCoordToSpreadCoord(&DocCentreOfView);

	// Finally, copy the result into the output parameter
	*Position = DocCentreOfView;
	
	return TRUE;  
}


/********************************************************************************************

>	BOOL OpPaste::DoPasteStandard(BOOL PasteAtSamePos,  BOOL ExternalData, Spread* pSpread = NULL, DocCoord* pCentre = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> / Mike
	Created:	28/7/94
	Inputs:		PasteAtSamePos: TRUE if we should paste the objects at their existing positions.
								FALSE if they should be pasted in the centre of the view

				ExternalData:	This flag indicates that the data has come from the external
								clipboard. We transform all objects in the clipboard to the
								centre of the view whenever this flag is set. This is so that
								subsequent PasteAtSamePos operations position the objects somewhere
								sensible. 

				pSpread:		ptr to insertion spread.  If NULL, spread in the centre
								of the current view is used AND pCentre is ignored.
								
				pCentre:		ptr to coord defining the centre of the paste.
								if PasteAtSamePos is TRUE, this coord is not used (I think! - Markn)
								
	Outputs:	-
	Returns:	-
	Purpose:	Pastes the contents of the clipboard into the current document.
	Errors:		-
	Scope:		protected
	SeeAlso:	-

	Changes:	1/6/95  - Fixed the error recovery mechanisms so they don't leave the undo
						  system in a state.
						  Made this function more accessible to derived classes.

********************************************************************************************/

BOOL OpPaste::DoPasteStandard(BOOL PasteAtSamePos, BOOL ExternalData, Spread* pSpread, DocCoord* pCentre)
{
	// Assume failure until told otherwise
	BOOL HasFailed=TRUE;

	Spread*  InsertionSpread = pSpread;
	DocCoord PastePosition;

	if (InsertionSpread == NULL)
	{
		if (!FindCentreInsertionPosition(&InsertionSpread, &PastePosition))
			return FALSE;
	}
	else
	{
		ERROR3IF(pCentre == NULL,"You've supplied a spread, but no centre coord.  Are you mad?");

		if (pCentre != NULL)
			PastePosition = *pCentre;
	}

	ERROR3IF(InsertionSpread == NULL,"Spread is NULL");

	// -----------------------------------
	// This is an horrendous bodge!
	// What's the point passing in a centre of the routine insists on calculating it for itself?
	// Ho hum...
	// If the data has come from the outside world it's original position is unknown
	// So now that the user has committed themselves to pasting at a certain position
	// record that as being the data's "original position".
	if (ExternalData)
	{
		InternalClipboard::SetOriginalCentrePoint(PastePosition);
	}

	// If trying to paste data at its original position
	// Then retrieve that info from the clipboard and use it as the paste position
	if (PasteAtSamePos)
	{
		PastePosition = InternalClipboard::GetOriginalCentrePoint();
	}
	// -----------------------------------

	if (InsertionSpread != NULL)
	{
		Node* Current;

		InternalClipboard* IntClip = InternalClipboard::Instance();
		if (IntClip==NULL)
		{
			ERROR3("Could not find internal clipboard");
			goto EndOperation;
		}

		BOOL FirstObjectCopied = TRUE; 
		DocRect ObjectsBounds;
		DocCoord CentreOfObjectsBounds;	

		// Create the transform to be applied to all pasted objects
		ObjectsBounds = IntClip->GetObjectsBounds(); 

		CentreOfObjectsBounds.x = ObjectsBounds.lo.x	+ (ObjectsBounds.Width()/2); 
		CentreOfObjectsBounds.y = ObjectsBounds.lo.y	+ (ObjectsBounds.Height()/2);

		Trans2DMatrix ObjectTrans  (PastePosition.x - CentreOfObjectsBounds.x,
					   				PastePosition.y - CentreOfObjectsBounds.y); 

		// Set the selected spread (assume it is in the Selected Doc and DocView - an
		// ENSURE will occur if this turns out not to be the case!)
		// NOTE selected spread should change due to a change in the sel range!
		Document::SetSelectedViewAndSpread(NULL, NULL, InsertionSpread);

		// Get the current tool
		Tool* pTool = Tool::GetCurrent();
		Spread* pSelSpread = Document::GetSelectedSpread();

		// Get the tool to remove all its blobs before we deselect the nodes.
		// Only do this if the current tool dosent update itself on sel changed messages
		if (pSelSpread!=NULL && pTool!=NULL && !pTool->AreToolBlobsRenderedOnSelection())
			pTool->RenderToolBlobs(pSelSpread,NULL);

		if (!DoStartSelOp(FALSE, TRUE))  // Try to record the selection state
			goto EndOperation;  

		// After the operation ends we will need to inform all DocComponents in the 
		// destination document of the outcome.
		InformDocComponentsOfOperationsOutcome(GetWorkingDoc()); 

		// Inform all DocComponents in the destination doc that a copy is about to take place
		BOOL ok;
		CALL_WITH_FAIL((GetWorkingDoc()->StartComponentCopy()),this, ok)
		
		if (!ok)
		{
			// Start Component copy has failed so abort operation
			// Note that AbortComponentCopy will have been called
			goto EndOperation;
		} 


		Current = IntClip->GetInsertionLayer()->FindFirstChild();

		// Create a range of all clipboard objects which are about to be copied
		// Selected + Unselected + don't cross layers

		// DMc - also need to set the PromoteToParent flag
		Range ClipRange(Current,NULL,RangeControl(TRUE,TRUE,FALSE, FALSE, FALSE, FALSE, FALSE, FALSE));
			  
		Node* FirstCopiedObj = NULL;
		Node* Copy; 

		// Now we try and copy all clipboard nodes
		while (Current != NULL)
		{
			// Make a copy of Current
			CALL_WITH_FAIL(Current->NodeCopy(&Copy), this, ok);

			if (!ok)
				goto EndOperation; 

			// Sanity check, will vapourise in retails
			ENSURE(Copy->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded)), 
				"Object being pasted is not a NodeRenderableBounded");
			
			// Remember the first copy we make
			if (FirstCopiedObj == NULL) 
				FirstCopiedObj = Copy; 

			NodeRenderableBounded* pBoundCopy = (NodeRenderableBounded*)Copy;
		 
			// Insert object
			if (!DoInsertNewNode(pBoundCopy,
								 (Spread*)NULL, // Selected spread
								 FALSE,			// Dont Invalidate region
								 FirstObjectCopied	// Only clear the selection if this is the
								 					// first object being copied
								 ))
			{
				// Tidyup
				Copy->CascadeDelete(); 
				delete Copy;
				goto EndOperation; 
			}

			// Now translate the object into the correct position.
			// Note: This has to be done even when PasteInPlace is operational because
			// the clipboard may have moved the objects ready for OLE external use.
			pBoundCopy->Transform(ObjectTrans);

			// Now call PostDuplicate on the copied node aand all it's children
			BOOL ok = TRUE;
			Node* pCurrent = pBoundCopy->FindFirstDepthFirst();
			while (pCurrent!=NULL && ok)
			{
				ok = pCurrent->PostDuplicate(this);

				pCurrent = pCurrent->FindNextDepthFirst(pBoundCopy);
			}
		
			// We also need to invalidate the region of the node now that it has been transformed
			if (!ok || !DoInvalidateNodeRegion(pBoundCopy, TRUE, FALSE))
			{
				// Tidyup
				Copy->CascadeDelete(); 
				delete Copy; 
				goto EndOperation; 
			} 

			// Normalise the attributes
			((NodeRenderableInk*) pBoundCopy)->NormaliseAttributes();

			// remove any name attribs from the clipboard pasted object (sjk 15/8/00)
			Node * pAttrib = SliceHelper::FindNextNameNode(pBoundCopy,pBoundCopy);
			while (pAttrib)
			{
				BOOL leave = RemoveNamesAlreadyUsedInStretching(pAttrib, this);

				Node * pDelMe = pAttrib;
				pAttrib = SliceHelper::FindNextNameNode(pAttrib,pBoundCopy);
				if (!leave)
				{
					pDelMe->UnlinkNodeFromTree();
					delete pDelMe;
				}
			}

			// Get the next object from the clipboard to copy
			Current = Current->FindNext();	
			FirstObjectCopied = FALSE; 
		}

		ENSURE(FirstCopiedObj != NULL, "No nodes were copied"); 
		if (FirstCopiedObj == NULL)
			goto EndOperation;

		
		// We now need to ask all copied nodes to copy their component data to the 
		// current document
		Range CopiedRange(FirstCopiedObj, Copy, RangeControl(TRUE,TRUE,FALSE)); 

		CALL_WITH_FAIL((CopiedRange.CopyComponentDataToDoc(IntClip, pOurDoc)), this, ok);
		if (!ok)
			goto EndOperation; 

		// THIS MUST BE LAST!
		HasFailed = FALSE;
	}

EndOperation:
	return (!HasFailed);
}



 /********************************************************************************************

>	static void OpPaste::RemoveNamesAlreadyUsedInStretching(Node * pAttrib)

	Author:		Simon_Knight (Xara Group Ltd) <camelotdev@xara.com> (put in a function of its own by Matt)
	Created:	22/01/2001
	Inputs:		-
	Returns:	-	False if the set info should not be pasted
	Purpose:	Remove set information from pasted attributes if that name
					would interfere with stretching if added...
	Notes:		Made static so that it can also be used by the PasteAttributes operation

********************************************************************************************/

BOOL OpPaste::RemoveNamesAlreadyUsedInStretching(Node * pAttrib, UndoableOperation* pOp)
{
	// try only removing names that has anything to do with extending
	String_256 Temp = SliceHelper::GetBarName((TemplateAttribute *)pAttrib);
	// is it a bar element?
	BOOL ok = Temp.IsEmpty();
	SGNameItem* pGalleryItem = NULL;
	if (ok)
	{
		Temp = ((TemplateAttribute *)pAttrib)->GetParam();
		pGalleryItem = SliceHelper::LookupNameGalleryItem(Temp);
		if (pGalleryItem)
		{
			// is it a trigger?
			if (pGalleryItem->m_IsATrigger == TRUE) // use == TRUE since it is often undefined
				ok = FALSE;
			else
			{
PORTNOTE("other","Removed SGNameItem::GetProperty usage")
#ifndef EXCLUDE_FROM_XARALX
				// is it a target?
				NamedStretchProp* pProp = (NamedStretchProp*)pGalleryItem->GetProperty(NamedStretchProp::nIndex);
				if (pProp && !pProp->GetTriggers().empty())
					ok = FALSE;
#endif
			}
		}
		else
		{
			// create a default property for a new set
			SliceHelper::CreatePropertiesForSet(Temp,
						TEXT(""),
						TRUE, FALSE, FALSE, 0,
						TEXT(""), TRUE, NULL, NULL, pOp);
		}
	}

	return ok;
}

// ------------------------------------------------------------------------------------------
// OpDelete methods
            
/********************************************************************************************

>	OpDelete::OpDelete() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpDelete constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpDelete::OpDelete(): SelOperation()								
{                              
}

 /********************************************************************************************

>	BOOL OpDelete::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDelete initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDelete::Init()
{
	return (RegisterOpDescriptor(0,
 							_R(IDS_DELETEOP),
							CC_RUNTIME_CLASS(OpDelete),
 							OPTOKEN_DELETE,
 							OpDelete::GetState,
 							0,							// help ID
 							_R(IDBBL_DELETE),
 							0,
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							0 // (GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
	       ));

	return true;
}               
    
/********************************************************************************************

>	OpState	OpDelete::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDelete
	Purpose:	For finding OpDelete's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDelete::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;

	SelRange *pSelRange = GetApplication()->FindSelection(); 

	// Set up the ObjChangeParam so we can ask the selected nodes if they mind being deleted
	ObjChangeFlags cFlags(TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,NULL);

	// Will one or more selected nodes allow this op?
	if (!pSelRange->AllowOp(&ObjChange,FALSE))
	{
		// No nodes can be deleted
		OpSt.Greyed = TRUE;

		// Load reason why operation is disabled
		UINT32 IDS = ObjChange.GetReasonForDenial();
		if (IDS == 0) IDS = _R(IDS_NO_OBJECTS_SELECTED);	// if 0 (i.e. not been set), then assume there's no selection
	   	*UIDescription = String_256(IDS);				// Resolve the string ID
	}
	else
		SetCutOpText(UIDescription, _R(IDS_CLIPBOARD_PREDELETE));

	return(OpSt);   
}

/********************************************************************************************

>	void OpDelete::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Delete operation. 
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
	
void OpDelete::Do(OpDescriptor*)
{   
	ObjectSet CompoundSet;

	// DMc
	// first, try deleting selected fill ramp blobs
	// check the fill interest first
	BlobManager * pBlobMgr = GetApplication()->GetBlobManager();

	if (pBlobMgr)
	{
		if (pBlobMgr->GetCurrentInterest().Fill)
		{
			if (DeleteFillRampBlobs())
			{
				// we've deleted fill ramp blobs, then drop out - job done
				return;
			}	
		}
	}

	// Obtain the current selections 
	Range Sel(*(GetApplication()->FindSelection()));
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	// Prepare an ObjChangeParam so we can mark which nodes will allow this op to happen to them
	ObjChangeFlags cFlags(TRUE);
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	if (!DoStartSelOp(TRUE,FALSE))  // Try to record the selection state
		goto EndOperation;

	// Mark nodes that will allow this to happen, and error if no nodes will let it happen
	if (!Sel.AllowOp(&ObjChange))
	{
		ERROR3("AllowOp() returned FALSE, i.e. op should have been greyed out");
		goto EndOperation;
	}


	// We need to invalidate the region
	if (!DoInvalidateNodesRegions(Sel, TRUE))
		goto EndOperation; 

	// Prior to hiding anything lets get ourselves a set of compound nodes which 
	// we will need to factor out attributes on. We cannot use the SelRange because
	// this will be empty after the deletion has taken place. 

	if (!(Sel.GetCompoundObjectSet(&CompoundSet, TRUE /* Exclude TextObjects*/)))
	{
		goto EndOperation; 
	}
	
	// Because of complex deletion we must also add the TextStory objects 
	// of selected chars. This code will hopefuly be removed in future. It is
	// inefficient cos we will need to factor out more than once for each line !.
	
	//if (!(Sel.AddTextStoryCompoundsForDel(&CompoundSet)))
	//{
	//	goto EndOperation;
	//} 

	// We know must localise the attributes on every item marked for deletion
	if (!DoLocaliseForAttrChange(&Sel, (AttrTypeSet*)NULL, TRUE /* Exclude TextStory objects */))
	{
		goto EndOperation;
	}

	// Go through one pass, asking nodes to hide themselves
	if (!DoHideComplexRange(Sel))
		goto EndOperation;

	{
		SelRange* pRange = GetApplication()->FindSelection();

		rg = Sel.GetRangeControlFlags();
		rg.PromoteToParent = TRUE;
		
		if (pRange)
		{
			// freshen the selection range by counting the currently selected objects
			pRange->Update(FALSE,NULL);
			pRange->Range::SetRangeControl(rg);

			/*INT32 num =*/ pRange->Count();
		
			// Obtain the current selection (should be up-to-date now!)
			Range NewSel(*pRange);

			NewSel.Range::SetRangeControl(rg);

			// Now go through a second pass snip the remainder out of the tree
			if (!DoHideNodes(NewSel,TRUE))	 // Hide the nodes (IncludeSubtreeSize)
				goto EndOperation;
		}
	}

	// Finally try to factor out the attributes on all compounds. Note that if a particular compound
	// is hidden then it will  be it's parent that has its attributes factored out !
	if (!DoFactorOutAfterAttrChange(&CompoundSet,(AttrTypeSet*)NULL))
	{
		goto EndOperation; 
	}


EndOperation:
	// Delete all items in the CompoundSet
	CompoundSet.DeleteAll();

	// Update all the changed nodes, i.e tell all the parents of the children that have been effected
	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	if (!UpdateChangedNodes(&ObjChange))
		FailAndExecute();

	// Invalidate sel range as it may have been cached in the middle of the op while the tree is
	// in an invalid state (i.e. some selected nodes may have had the PERMISSION_DENIED state set during caching).
	{
		SelRange* pRange = GetApplication()->FindSelection();
		if (pRange)
			pRange->Update(FALSE,NULL);

		rg.PromoteToParent = FALSE;
		pRange->Range::SetRangeControl(rg);
	}

 	End(); 
}			

/********************************************************************************************

>	BOOL OpDelete::DeleteFillRampBlobs()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com> Mc
	Created:	24/10/99
	Inputs:		
	Outputs:	-
	Returns:	-
	Purpose:	Deletes the selected fill ramp blobs. If any exist, we shouldn't delete
				any objects as this could be confusing. Only delete objects i.e. return FALSE
				if (a) the blob interest is for fill blobs and (b) if no fill ramp blobs
				are selected
				
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
BOOL OpDelete::DeleteFillRampBlobs()
{
	// run through the selection seeing if any fill ramp blobs are selected
	Range * pSel = GetApplication()->FindSelection();

	if (!pSel)
		return FALSE;

	Node * pNode = pSel->FindFirst();
	AttrFillGeometry * pFill = NULL;
//	AttrFillGeometry * pNewFill = NULL;

	Document * pDoc = Document::GetCurrent();

	BOOL bHasBegun = FALSE;

	BOOL bFoundBlobs = FALSE;
//	NodeHidden * pHidden = NULL;

	while (pNode)
	{
		if (pNode->IsAnObject())
		{
			// find out the fill attribute associated with this node
			if (((NodeRenderableInk *)pNode)->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),
				(NodeAttribute **)(&pFill)))
			{
				// we've found a fill
				if (pFill)
				{
					// now, does this fill have a colour ramp ?
					if (pFill->GetColourRamp())
					{
						// if it does have a colour ramp, see if any of the ramp's nodes are
						// selected
						FillRamp * pRamp = pFill->GetColourRamp();

						if (pRamp->CountSelBlobs() != 0)
						{
							// ok, we have more than one so remove the selected items
							bFoundBlobs = TRUE;

							if (!bHasBegun)
							{
								if (!DoStartSelOp(TRUE, TRUE))
								{
									goto ErrorEnd;
								}

								bHasBegun = TRUE;
							}

							// invalidate the node region
							((NodeRenderableInk *)pNode)->ReleaseCached();
							if (!DoInvalidateRegion(pNode->FindParentSpread(), 
								((NodeRenderableInk *)pNode)->GetBoundingRect()))
//							if (!DoInvalidateNodeRegion((NodeRenderableBounded*)pItem->pNode))
								goto ErrorEnd;

							RestoreFillRampAction * pAction = NULL;
							
							if (RestoreFillRampAction::Init(this, this->GetUndoActionList(), pFill,
								&pAction) != AC_OK)
							{
								goto ErrorEnd;
							}

							pRamp = pFill->GetColourRamp();

							// now, remove all selected nodes from the colour ramp
							RampItem * pThisItem = (RampItem *)pRamp->GetHead();
							RampItem * pNextItem = NULL;

							while (pThisItem)
							{
								if (pThisItem->IsSelected())
								{
									// remove it !
									pNextItem = (RampItem *)pRamp->GetNext(pThisItem);
									delete pRamp->RemoveItem(pThisItem);
									pThisItem = pNextItem;

									if (pRamp->IsEmpty ())
									{
										//delete (pRamp);
										
										// CGS
										// we cannot do the following line - since deleting the fillramp leads
										// us into trouble with the undo/redo system!
										((GradFillAttribute*) (pFill->GetAttributeValue ()))->DeleteColourRamp ();

										// CGS
										// we also need to re-toggle the status of the profile gadget ....

										//Tool* CurrentTool = Tool::GetCurrent();
										//CurrentTool->		// possibly test this as well ....

PORTNOTE("other", "Removed use of DialogBarOp in OpDelete::DeleteFillRampBlobs")
#ifndef EXCLUDE_FROM_XARALX
										String_32 str = String_32(_R(IDS_FILLTOOL_FILLINFOBARNAME));
										GradInfoBarOp* pDialogBarOp = (GradInfoBarOp*) DialogBarOp::FindDialogBarOp(str);
										
										if (pDialogBarOp)
										{
											if (pDialogBarOp->IsVisible ())
											{
												pDialogBarOp->EnableControls ();
											}
										}
#endif

										pThisItem = NULL;
									}
								}
								else
								{
									pThisItem = (RampItem *)pRamp->GetNext(pThisItem);
								}
							}

							((NodeRenderableInk *)pNode)->ReleaseCached();
							if (!DoInvalidateRegion(pNode->FindParentSpread(), 
								((NodeRenderableInk *)pNode)->GetBoundingRect()))
//							if (!DoInvalidateNodeRegion((NodeRenderableBounded*)pItem->pNode))
								goto ErrorEnd;

							if (pDoc)
								pDoc->ForceRedraw(pNode->FindParentSpread(),
									((NodeRenderableInk *)pNode)->GetBoundingRect(), FALSE, pNode);

						}
					}
				}
			}
		}

		pNode = pSel->FindNext(pNode);
	}

	if (bHasBegun)
		End();

	return bFoundBlobs;

ErrorEnd:
	if (bHasBegun)
	{
		FailAndExecute();
		End();
	}
	return TRUE;
}

// ------------------------------------------------------------------------------------------
// CarbonCopyOp methods
             
/********************************************************************************************

>	CarbonCopyOp::CarbonCopyOp() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	CarbonCopyOp constructor - does nothing itself, just calls the parent constructor
	Errors:		-
	SeeAlso:	OpDuplicate, OpClone

********************************************************************************************/
            
CarbonCopyOp::CarbonCopyOp(): SelOperation()								
{                              
}

    
/********************************************************************************************

>	OpState	CarbonCopyOp::GetState(String_256*, OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		-
	Outputs:	-
	Returns:	The state of the CarbonCopyOp or any operations derived from it that dosen't
				overide this function.
	Purpose:	For finding the operations state.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	CarbonCopyOp::GetState(String_256* UIDescription, OpDescriptor *Bob)
{
	OpState OpSt;

	return(OpSt);   
}


/********************************************************************************************

>	BOOL CarbonCopyOp::DoProcessing(BOOL Translate, INT32 XOffset = 0, INT32 YOffset = 0)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com> (from Simon M code). Altered by Graham 25/6/96.
	Created:	18/11/94
	Inputs:		A Trans2DMatrix (Graham 25/6/96) giving details of how the copy
				should be transformed/translated.

				SelectCopy: TRUE if the original is to be deselected and the copy selected
							(as in Clone and Duplicate)

							FALSE if the original is to remain selected and the copy
							unselected (as in dropping copies with CopyAndTransform)
	Outputs:	-
	Returns:	TRUE if the operation completed OK, FALSE otherwise
	Purpose:	Performs the common processing of the Clone, Duplicate and CopyAndTransform
				operations.  Clones	are not offset from the original, whilst Duplicated objects
				are offset by the amount in the parameter matrix. CopyAndTransform creates
				a copy transformed by the specified matrix.

	Errors:		ERROR2IF's to check there is no selection, Selection is not an ink object, or
				Selection is not bounded.
	SeeAlso:	-

********************************************************************************************/

BOOL CarbonCopyOp::DoProcessing(Trans2DMatrix Transformer, BOOL SelectCopy = TRUE)
{   
	//Help the user with their selection...
	Range* Selection = GetApplication()->FindSelection();
	RangeControl TransFlags = Selection->GetRangeControlFlags();
	TransFlags.IgnoreNoneRenderable=TRUE;
	TransFlags.IgnoreInvisibleLayers = TRUE;
	Selection->SetRangeControl(TransFlags);
	SliceHelper::ModifySelectionToContainWholeButtonElements();

	// Obtain the current selections 
	NodeListItem * pItem = NULL;

	// DMc - build a list of the original selection, so we can reselect them later
	Range Sel(*(GetApplication()->FindSelection()));

	List * pSelList = Sel.MakeListOfNodes(FALSE);
	
	RangeControl rg = Sel.GetRangeControlFlags();
	rg.PromoteToParent = TRUE;
	Sel.Range::SetRangeControl(rg);

	ObjChangeFlags cFlags;
	ObjChangeParam ObjChange(OBJCHANGE_STARTING,cFlags,NULL,this);

	if (!Sel.AllowOp(&ObjChange))
	{
		SliceHelper::RestoreSelection();
		return FALSE;
	}


	// Find the first node which is selected 
	Node* FirstSelectedNode = Sel.FindFirst();

	Node* Current; 

//		RangeControl LyrCntrl = { TRUE, FALSE, FALSE };  // Selected + don't cross layers  
	Range LayerRange; 

   	Node* FirstNodeToCopyOnLyr;
	Node* LastNodeToCopyOnLyr;
	Node* Tail; 

	ERROR2IF(FirstSelectedNode == NULL, FALSE, "Called CarbonCopy operation with no nodes selected"); 

	// init the vars needed for the mesh layers bit later on
	Node * pFirstNodeOfImportedLayer[5];
	String_256	StateLayerNames[5];
	Node * pLayer = NULL;

	StateLayerNames[0].Load(_R(IDS_ROLLOVER_DEFAULT)); // = "Default";
	StateLayerNames[1].Load(_R(IDS_ROLLOVER_MOUSE)); // = "Mouse";
	StateLayerNames[2].Load(_R(IDS_ROLLOVER_CLICKED)); // = "Clicked";
	StateLayerNames[3].Load(_R(IDS_ROLLOVER_SELECTED)); // = "Selected";
	StateLayerNames[4].Load(_R(IDS_BACK_BAR)); // = "BackBar";

	INT32 i;
	for (i = 0; i < 5; i++)
		pFirstNodeOfImportedLayer[i] = NULL;

	//Graham 30/9/96: Tell the current drag operation to clear its list
	//of nodes to select at the end of the drag.
	TransOperation* pTransOperation=(TransOperation*) GetCurrentDragOp();

	if(pTransOperation)
		pTransOperation->ClearNodesToSelect();
	
	if (!DoStartSelOp(FALSE,TRUE))  // Try to record the selection state
	{
		goto EndOperation;
	}

// The following is error checking to make sure the copy doesn't go off the edge of the
// pasteboard.

	// Is the transform matrix going to move or transform the copy?
	if (Transformer.GetMatrix().Type!=TRANS_IDENTITY)
	{
		//Yes. So check the copy will not spill off the pasteboard.

		// Find the bounding rectangle of the selection
		DocRect CurrentBounds = Sel.GetBoundingRect();
		
		//And the bounding rectangle of the selection once transformed
		DocRect NewBounds=CurrentBounds;
		Transformer.GetMatrix().TransformBounds(&NewBounds);
		
		//And the current spread if there is one
		Spread* pSpread = pOurDoc->GetSelectedSpread();
		
		//Is there a spread currently selected?
		if (pSpread!=NULL)
		{
			//Yes. Will its pasteboard expand to include the bounds of the transformed selection?
			//(Note: ExpandPasteboardToInclude doesn't actually expand the pasteboard
			//currently. It just checks whether NewBounds fits within the pasteboard area.)
			if(!pSpread->ExpandPasteboardToInclude(NewBounds))
			{
				//No, the transformed copy will not fit in the pasteboard.
				//Is the transform a translation?
				if(Transformer.GetMatrix().IsTranslation())
				{
					//Yes. So restrict the translation so it fits within the
					//pasteboard.
					
					//First get the translation in "INT32" form
					INT32 XOffset;
					INT32 YOffset;

					Transformer.GetMatrix().GetTranslation(XOffset, YOffset);

					DocRect PasteRect = pSpread->GetPasteboardRect();
					pSpread->DocCoordToSpreadCoord(&PasteRect);
					
					// Find out how far we can move
					INT32 MaxX = 0;
					INT32 MaxY = 0;

					// First the X direction
					if (XOffset<0)
					{
						// The offset is negative, so work out the maximum negative offset allowed
						MaxX = PasteRect.lo.x - CurrentBounds.lo.x;
						if (XOffset<MaxX) XOffset = MaxX;
					}
					else
					{
						MaxX = PasteRect.hi.x - CurrentBounds.hi.x;
						if (XOffset>MaxX)
						XOffset = MaxX;
					}

					// and then the Y direction
					if (YOffset<0)
					{	
						// The offset is negative, so work out the maximum negative offset allowed
						MaxY = PasteRect.lo.y - CurrentBounds.lo.y;
						if (YOffset<MaxY) YOffset = MaxY;
					}
					else
					{
						MaxY = PasteRect.hi.y - CurrentBounds.hi.y;
						if (YOffset>MaxY)
						YOffset = MaxY;
					}
					
					//And put the translations back into the Transformer matrix
					Matrix TranslateMatrix=Transformer.GetMatrix();
					TranslateMatrix.SetTranslation(XOffset, YOffset);

					Transformer=Trans2DMatrix(TranslateMatrix);
				}
				else
				{
					//No, this is not a translation.
					//End the operation without copying the selection.
					//This should not be confusing to the user, because the user can see
					//he has dragged the selection over the edge of the pasteboard.
					//So he should not be surprised that the operation does nothing.

					goto EndOperation;
				}
			}
		}
		else
		{
		//No, there is no spread selected.
		//We do not want to risk creating a copy outside the current bounds
		//of the object. So we set the Transformer matrix to the identity, which simply
		//clones the object
		Transformer=Trans2DMatrix();
		}
	}
		
	// Now start the copying procedure, which we do one layer at a time

		//The first node to copy on the first layer is the first selected node
		FirstNodeToCopyOnLyr = FirstSelectedNode;

		// When there are no more objects to copy, FirstNodeToCopyOnLyr will be NULL
		while (FirstNodeToCopyOnLyr != NULL)
		{

			// Tail is the position to insert the new Duplicate nodes		                                                
			Tail = FirstNodeToCopyOnLyr->FindParent()->FindLastChild(TRUE); // Excludes InsertionNode

			// Create a range of selected nodes on this layer
//			LayerRange = Range(FirstNodeToCopyOnLyr, NULL, RangeControl(TRUE,FALSE,FALSE,FALSE,
//				FALSE,FALSE,FALSE,TRUE) );
			Range temp(FirstNodeToCopyOnLyr, NULL, RangeControl(TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE));
			LayerRange = temp;

			//When we enter the loop, Current should signify the last copied node on this layer.
			//Set to null to signify we need to copy the first node on this layer.
			Current=NULL;
			LastNodeToCopyOnLyr = LayerRange.FindLast();

			Node * pFirstNewNodeInLayer = NULL;

			// Loop for each node on this layer
			do
			{
				//Get the next node in the layer to copy. If Current is NULL, this
				//is the first node in the layer to be copied.

				if(Current==NULL)
					Current=FirstNodeToCopyOnLyr;
				else
					Current = LayerRange.FindNext(Current);

				// Make a copy of the current node
				Node* TheCopy;
				BOOL CopiedOK; 

				CALL_WITH_FAIL(Current->NodeCopy(&TheCopy), this, CopiedOK);
				if (!CopiedOK)
				{
					goto EndOperation;
				}

				// store this to tidy up later in meshing layers
				if (!pFirstNewNodeInLayer)
					pFirstNewNodeInLayer = TheCopy;

				// make sure that it is bounded
				ERROR2IF(!TheCopy->IsBounded(), FALSE, "Object being pasted is not a NodeRenderableBounded"); 
				NodeRenderableBounded* BoundCopy = (NodeRenderableBounded*)TheCopy;

				// Insert the copied node at the tail position
				if (!DoInsertNewNode(BoundCopy, Tail, NEXT,
									 FALSE, 	// Don't Invalidate region 
									 FALSE)) 	// Don't Clear the selections
				{
					// Failed, so tidy up before returning
					TheCopy->CascadeDelete(); 
					delete TheCopy; 
					goto EndOperation;
				} 

				// DMc - to fix bug with r-hand clicks on drags
				BoundCopy->SetSelected(TRUE);
				BoundCopy->SetRender(TRUE, TRUE);

				//If we need to Transform the copy, do so
				if (Transformer.GetMatrix().Type!=TRANS_IDENTITY)
				{
					BoundCopy->Transform(Transformer);
				}

				ERROR2IF(!Current->IsAnObject(), FALSE, "Current should be an ink node");

				// Call PostDuplicate on the copied node and all it's children
				BOOL ok = TRUE;
				Node* pCurrent = BoundCopy->FindFirstDepthFirst();
				while (pCurrent!=NULL && ok)
				{
					ok = pCurrent->PostDuplicate(this);
					pCurrent = pCurrent->FindNextDepthFirst(BoundCopy);
				}
				if (!ok) goto EndOperation; 

				
				//Now we need to select the copy and deselect the original.

				//This code isn't too elegant, because it's four days until deadline
				//and I want to get this "bug" fixed. So...

				//Is there a transform drag going on?
				Operation* pDragOp=GetCurrentDragOp();
				TransOperation* pTransDragOp=(TransOperation*) pDragOp;

				if (!pTransDragOp)
				{
					//No. So we can deselect the original now.
					//((NodeRenderableInk*)Current)->DeSelect(TRUE); 
				}
				else
				{
					//Yes, there's a transform drag going on.
					//Transform drags start acting strangely if you
					//change the selection halfway through.
					//So first we need to deselect the copy, to ensure
					//we haven't changed the selection.
					//((NodeRenderableInk*)BoundCopy)->DeSelect(TRUE);

					//And tell the drag to select the copy after the
					//drag has finished.
					pTransDragOp->SelectNodeAfterDrag(BoundCopy);
				}
			
				// We also need to invalidate the region of the node now that it has been transformed
				BoundCopy->ReleaseCached(TRUE, FALSE, FALSE, FALSE);				// Release Parents because of change
				if (!DoInvalidateNodeRegion(BoundCopy, TRUE, FALSE, FALSE, FALSE))	// Don't recache everything
				{
					// Tidyup
					TheCopy->CascadeDelete(); 
					delete TheCopy; 
					goto EndOperation;
				}

				// We also need to invalidate the region of the node now that it has been transformed
				BoundCopy->ReleaseCached(TRUE, FALSE, FALSE, FALSE);				// Release Parents because of change
				DoInvalidateNodeRegion((NodeRenderableBounded*) Current, TRUE, FALSE, FALSE, FALSE);	// Don't recache everything
				
	
				//New insertion position is after the last copied node
				Tail = BoundCopy;

			} while (Current != LastNodeToCopyOnLyr);


			// is this a rollover layer?
			pLayer = pFirstNewNodeInLayer->FindParent();
			if (pLayer->IsLayer())
				for (i = 0; i < 5; i++)
				{
					if (pFirstNodeOfImportedLayer[i] == NULL 
						&& StateLayerNames[i].CompareTo(((Layer *)pLayer)->GetLayerID() ) == 0)
						pFirstNodeOfImportedLayer[i] = pFirstNewNodeInLayer;
				}


			//And get the first node on the next layer to copy
			FirstNodeToCopyOnLyr = Sel.FindNext(Tail);
		}
	
	// Mesh the buttons duplicated onto the layers
	// if we have any of these special layers
	if (pFirstNodeOfImportedLayer[0] || pFirstNodeOfImportedLayer[1] || pFirstNodeOfImportedLayer[2] || pFirstNodeOfImportedLayer[3])
		SliceHelper::MeshImportedLayersWithExistingButtonBars(pFirstNodeOfImportedLayer, this, FALSE);

	// DMc - deselect the original selection
	if (pSelList)
	{
		pItem = (NodeListItem *)pSelList->GetHead();
		
		while (pItem)
		{
			if (pItem->pNode)
			{
				pItem->pNode->SetSelected(FALSE);
			}
			
			pItem = (NodeListItem *)pSelList->GetNext(pItem);
		}
		
		pSelList->DeleteAll();
		delete pSelList;
	}
		
	//Just incase we helped the user with their selection - restore it back again
	TRACEUSER( "Matt", _T("Changing Selection in CarbonCopyOp::DoProcessing()\n"));
	SliceHelper::RestoreSelection();

	ObjChange.Define(OBJCHANGE_FINISHED,cFlags,NULL,this);
	UpdateChangedNodes(&ObjChange);

	End();

	return TRUE;

 	EndOperation:

	if (pSelList)
	{
		pSelList->DeleteAll();
		delete pSelList;
	}

	//Just incase we helped the user with their selection - restore it back again
	TRACEUSER( "Matt", _T("Restoring Selection in CarbonCopyOp::DoProcessing()\n"));
	SliceHelper::RestoreSelection();

	End();
	return FALSE;
}



// ------------------------------------------------------------------------------------------
// OpDuplicate methods
 
// Duplicate preference variables (They have the same name as the preference)
INT32 OpDuplicate::DuplicatePlacementX = INT32(DEFAULT_DUPLICATE_PLACEMENT_X);
INT32 OpDuplicate::DuplicatePlacementY = INT32(DEFAULT_DUPLICATE_PLACEMENT_Y);
            
/********************************************************************************************

>	OpDuplicate::OpDuplicate() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpDuplicate constructor
	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/
            
            
OpDuplicate::OpDuplicate(): CarbonCopyOp()								
{                              
}

 /********************************************************************************************

>	BOOL OpDuplicate::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/93
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpDuplicate initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpDuplicate::Init()
{
	BOOL ok =  Camelot.DeclareSection(TEXT("Duplicate"), 10) &&
		   	   Camelot.DeclarePref(TEXT("Duplicate"), TEXT("DuplicatePlacementX"),
		   					   		&OpDuplicate::DuplicatePlacementX, INT_MIN, INT_MAX) &&
		   	   Camelot.DeclarePref(TEXT("Duplicate"), TEXT("DuplicatePlacementY"),
		   					   		&OpDuplicate::DuplicatePlacementY,INT_MIN, INT_MAX) &&
	 
				(RegisterOpDescriptor(0,
 							_R(IDS_DUPLICATEOP),
							CC_RUNTIME_CLASS(OpDuplicate),
 							OPTOKEN_DUPLICATE,
 							OpDuplicate::GetState,
 							0,							// help ID
 							_R(IDBBL_DUPLICATE),
 							0,							// bitmap ID
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION)
));
 	return ok; 
	return true;
}               


/********************************************************************************************

>	OpState	OpDuplicate::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpDuplicate
	Purpose:	For finding the operations state.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpDuplicate::GetState(String_256* UIDescription, OpDescriptor *Bob)
{
	OpState OpSt;

	SetCutOpText(UIDescription, _R(IDS_CLIPBOARD_PREDUPLICATE));

	return(OpSt);   
}



/********************************************************************************************

>	void OpDuplicate::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/93
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Duplicate operation, duplicates are placed as Last children of
				their layer. 
	Errors:		-
	SeeAlso:	CarbonCopyOp::DoProcessing

********************************************************************************************/
	
void OpDuplicate::Do(OpDescriptor*)
{   
	//Graham 25/6/96: Now you pass a matrix instead of individual parameters.

	INT32 dx = DuplicatePlacementX;
	INT32 dy = DuplicatePlacementY;
	Document* pDoc = Document::GetCurrent();
	if (pDoc)
	{
		DocCoord offset = pDoc->GetDuplicationOffset();
		dx = offset.x;
		dy = offset.y;
	}

	Trans2DMatrix MatrixToPass(dx, dy);

	DoProcessing(MatrixToPass);
}	



// ------------------------------------------------------------------------------------------
// OpClone methods
 
/********************************************************************************************

>	OpClone::OpClone() 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpClone constructor
	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/
            
OpClone::OpClone(): CarbonCopyOp()								
{                              
}



/********************************************************************************************

>	BOOL OpClone::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpClone initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpClone::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_CLONEOP),
								CC_RUNTIME_CLASS(OpClone),
 								OPTOKEN_CLONE,
 								OpClone::GetState,
 								0,	/* help ID */
 								_R(IDBBL_CLONE),
								_R(IDD_BARCONTROLSTORE),		// resource ID
								_R(IDC_CLONE),
								SYSTEMBAR_EDIT,				// Bar ID
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION) ));
}



/********************************************************************************************

>	OpState	OpClone::GetState(String_256*, OpDescriptor*)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpClone
	Purpose:	For finding the operations state.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpClone::GetState(String_256* UIDescription, OpDescriptor *Bob)
{
	OpState OpSt;

	SetCutOpText(UIDescription, _R(IDS_CLIPBOARD_PRECLONE));

	return(OpSt);   
}


/********************************************************************************************

>	void OpClone::Do(OpDescriptor*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/11/94
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the Clone operation, duplicates are placed as Last children of
				their layer. 
	Errors:		-
	SeeAlso:	CarbonCopyOp::DoProcessing

********************************************************************************************/
	
void OpClone::Do(OpDescriptor*)
{   
	//Graham 25/6/96: Now you pass a matrix to transform by instead of parameters
	//to translate by. We don't want to transform our copy at all - so create
	//an Identity matrix to pass
	
	Trans2DMatrix MatrixToPass; 
	
	DoProcessing(MatrixToPass);
}	

// ------------------------------------------------------------------------------------------
// OpCopyAndTransform methods. All heavily copied from Peter's code above.
 
/********************************************************************************************

>	OpCopyAndTransform::OpCopyAndTransform() 

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpCopyAndTransform default constructor. The CopyTransform matrix
				will be constructed, by default, as an Identity matrix. Therefore,
				using this constructor essentially creates a Clone operation.
	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/
            
OpCopyAndTransform::OpCopyAndTransform(): CarbonCopyOp()								
{                   
}

/********************************************************************************************

>	OpCopyAndTransform::OpCopyAndTransform(Trans2DMatrix Transform, BOOL SelectCopy=FALSE) 

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		Transform: The matrix by which to transform the copy

				PassSelectCopy: TRUE to select the copy and deselect the original
								FALSE (default) to leave the original selected and
								the copy deselected.
	Outputs:	-
	Returns:	-
	Purpose:	OpCopyAndTransform constructor.

				This creates a CopyAndTransform operation that transforms the copy
				by the Transform matrix.

	Errors:		-
	SeeAlso:	CarbonCopyOp

********************************************************************************************/
            
OpCopyAndTransform::OpCopyAndTransform(Trans2DMatrix Transform, BOOL PassSelectCopy):CarbonCopyOp()								
{                              
	CopyTransform=Transform;
	SelectCopy=PassSelectCopy;
}


/********************************************************************************************

>	BOOL OpCopyAndTransform::Init()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
	Purpose:	OpCopyAndTransform initialiser method

				Note that CopyAndTransform never appears on the menu or toolbar at
				the moment, but I have created menu strings just in case anyone
				wants it to. There is no button icon defined for this operation, however.

	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpCopyAndTransform::Init()
{
	return	(RegisterOpDescriptor(0,
	 							_R(IDS_COPYANDTRANSFORMOP),
								CC_RUNTIME_CLASS(OpCopyAndTransform),
 								OPTOKEN_COPYANDTRANSFORM,
 								OpCopyAndTransform::GetState,
 								0,	/* help ID */
 								_R(IDBBL_COPYANDTRANSFORM),
								0,		// resource ID
								0,
								SYSTEMBAR_ILLEGAL,				// Bar ID
								TRUE, 						// Receive messages
								FALSE,
								FALSE,
								0,
								(GREY_WHEN_NO_CURRENT_DOC | GREY_WHEN_NO_SELECTION) ));
}



/********************************************************************************************

>	OpState	OpCopyAndTransform::GetState(String_256*, OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpCopyAndTransform
	Purpose:	For finding the operations state.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpCopyAndTransform::GetState(String_256* UIDescription, OpDescriptor *Bob)
{
	OpState OpSt;

	SetCutOpText(UIDescription, _R(IDS_CLIPBOARD_PRECOPYANDTRANSFORM));

	return(OpSt);   
}


/********************************************************************************************

>	void OpCopyAndTransform::Do(OpDescriptor*)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the CopyAndTransform operation with the current CopyTransform matrix
				and the current SelectCopy status.

				This creates a copy of the current selection, transformed by the
				CopyTransform matrix.

				It keeps the original selection selected rather than selecting the copy.

	Errors:		-
	SeeAlso:	CarbonCopyOp::DoProcessing

********************************************************************************************/
	
void OpCopyAndTransform::Do(OpDescriptor*)
{   	
	DoProcessing(CopyTransform, SelectCopy);
}	



// ------------------------------------------------------------------------------------------
// OpPasteAttributes methods
            
/********************************************************************************************

>	OpPasteAttributesAttributes::OpPasteAttributes(): SelOperation

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	OpPasteAttributesAttributes constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
            
            
OpPasteAttributes::OpPasteAttributes(): SelOperation()
{                              
}

 /********************************************************************************************

>	BOOL OpPasteAttributes::Init()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	OpPasteAttributes initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL OpPasteAttributes::Init()
{
	BOOL ok =  RegisterOpDescriptor(0,
 							_R(IDS_PASTEATTRIBUTESOP),
							CC_RUNTIME_CLASS(OpPasteAttributes),
 							OPTOKEN_PASTEATTRIBUTES,
 							OpPasteAttributes::GetState,
 							0,							// help ID
 							_R(IDBBL_PASTEATTRIBUTES),
 							0,							// bitmap ID
 							0,
							SYSTEMBAR_ILLEGAL,			// For now !
							TRUE, 						// Receive messages
							FALSE,
							FALSE,
							0,
							(GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)
							);

	return ok;
}               
    
/********************************************************************************************

>	OpState	OpPasteAttributes::GetState(String_256*, OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the OpPasteAttributes
	Purpose:	For finding OpPasteAttributes's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	OpPasteAttributes::GetState(String_256* UIDescription, OpDescriptor*)
{
	OpState OpSt;
	// Greyed if the clipboard is empty
	if (InternalClipboard::IsEmpty(FALSE)) // Only look at internal clipboard
	{
		OpSt.Greyed = TRUE;
		// Determine if the External clipboard is empty
		if (InternalClipboard::IsEmpty(TRUE))
		{
			// There is no data on either the Internal or external clipboards.
			// tell the user that the clipboard is empty
	   		*UIDescription = String_256(_R(IDS_CLIPBOARD_EMPTY));
		}
		else
		{
			// There is data on the external clipboard.
			// We can't paste attributes until the user has done a paste
			*UIDescription = String_256(_R(IDS_EXTERNAL_CLIP_OBJECTS));
		}
	}
	else
	{
		// There is data on the internal clipboard
		// Are there any common attributes
		BOOL CommonAttrsExist = FALSE; 
		BOOL bCommonLEsExist = FALSE; 
		Range* pClipboardRange = InternalClipboard::GetClipboardRange();
		// Finds all common attributes. Remember common attributes are cached
		if (pClipboardRange) 
		{
			CommonAttrSet CommonAttrs;
			if( pClipboardRange->FindCommonAttributes(&CommonAttrs , TRUE ) )
			{
				if (CommonAttrs.IsAnyItemCommon())
					CommonAttrsExist = TRUE;	
			}
		}

		// Check for LiveEffects
		if (pClipboardRange)
		{
			Range* pTempRange = new Range(*pClipboardRange);
			ENSURE(pTempRange!=NULL, "OpPasteAttributes::Do couldn't allocate memory for Range\n");
			if (pTempRange)
			{
				RangeControl rg = pTempRange->GetRangeControlFlags();
				rg.PromoteToParent = FALSE;
				pTempRange->SetRangeControl(rg);

				EffectsStack* pEffectsStack = EffectsStack::GetEffectsStackFromSelection(pTempRange, TRUE, TRUE);	// Include locked effects in this stack
				bCommonLEsExist = (pEffectsStack!=NULL && pEffectsStack->LockableNonEmpty());
				if (pEffectsStack)
					delete pEffectsStack;
			}
			delete pTempRange;
		}

		if (!CommonAttrsExist && !bCommonLEsExist)
		{
			OpSt.Greyed = TRUE;
			// The objects on the clipboard share no common attributes. Report this to the user
			*UIDescription = String_256(_R(IDS_NO_COMMON_CLIP_ATTRS));
		}
	}
	return(OpSt);
}



/********************************************************************************************

>	void OpPasteAttributes::Do(OpDescriptor*)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/08/95
	Inputs:		OpDescriptor (unused)
	Purpose:	Performs the PasteAttributes operation. 
				
********************************************************************************************/
	
void OpPasteAttributes::Do(OpDescriptor* WhichOp)
{	
	BOOL ok = TRUE; // Until something goes wrong
	InternalClipboard* pSrcDoc = InternalClipboard::Instance();
	Document* pDestDoc = GetWorkingDoc();

	
	// Start the op in the usual way
	DoStartSelOp(FALSE, FALSE, TRUE, TRUE);
	
	// Obtain a set of common attribute values for all attribute types
	// remember these values are cached
	Range* pClipboardRange = InternalClipboard::GetClipboardRange();
	if (!pClipboardRange)
	{
		ok = FALSE; 
	}
	 
	CommonAttrSet CommonAttrs;

	if (ok)
	{
		if (!pClipboardRange->FindCommonAttributes(&CommonAttrs, TRUE ) )
		{
			// Our GetState fn will have greyed the op if there were no common attributes
			ok = FALSE;
		}
	}
	
	if (ok)
	{ 
		ERROR3IF(CommonAttrs.IsEmpty(), "Empty CommonAttribute set in OpPasteAttributes::Do"); 

		// We will require the bounds of all objects on the clipboard to scale AttrFillGeometry 
		// attributes.
		DocRect ClipBounds = pClipboardRange->GetBoundingRect();

		// Create a list of attributes to apply
		List AttribsToApply;
		NodeAttributePtrItem* pAttrToApplyItem;
		NodeAttribute* pAttr;
		CommonAttributeItem*  pAttrItem;
		// For each item in the common attribute set
		//TRACE( _T("Attributes being applied\n\n"));

		for (pAttrItem = (CommonAttributeItem*)(CommonAttrs.GetHead());
			 pAttrItem;
			 pAttrItem = (CommonAttributeItem*)(CommonAttrs.GetNext(pAttrItem)))
		{
			pAttr = pAttrItem->pAttr;
			if (pAttrItem->Status == Range::ATTR_COMMON && pAttr)
			{
				// Ask the attribute if it wants to be applied.
				if (pAttr->CanBeAppliedToObject())
				{
					// DEBUG
					//TRACE( _T("%s \n"), pAttrItem->AttrType->m_lpszClassName); 
					// This attribute is common. Add it to the list	of attributes to apply
					pAttrToApplyItem = new NodeAttributePtrItem;
					if (pAttrToApplyItem) 
					{ 	
						// We need to make a copy of the attribute because it's component data
						// will soon be in the document we are pasting into.
						NodeAttribute* pAttribClone = (NodeAttribute*)(pAttr->SimpleCopy());
						if (pAttribClone)
						{
							if (!pAttribClone->IsAnObjectName() || OpPaste::RemoveNamesAlreadyUsedInStretching(pAttribClone, (UndoableOperation *)this))
							{
								AttribsToApply.AddHead(pAttrToApplyItem); 

								pAttrToApplyItem->NodeAttribPtr = pAttribClone; 

								if (pAttribClone->IsAFillAttr())
								{
									// Set the bounds of the attribute. This is of course the bounds
									// of the clipboard objects. 
									((AttrFillGeometry*)pAttribClone)->SetBoundingRect(ClipBounds);
								}

								// I'm not sure if this	is neccessary but Will does it when making an attribute
								// current.
								if (pAttribClone->IsAFractalFill())
								{
									// The attr, should always use the default DPI for fractals.
							 		((AttrFillGeometry*)pAttribClone)->SetFractalDPI(AttrFillGeometry::FractalDPI);
								}
							}
						}
						else
							ok = FALSE; 
					}
					else
						ok = FALSE;
				}
			}
		}

		if (ok)
		{
			// Copy the component data from all attributes before we try to apply them.
			// (this is the safest way).
			// Inform all DocComponents in the destination doc that a copy is about to take place
			BOOL ok;
			CALL_WITH_FAIL((pDestDoc->StartComponentCopy()),this, ok)
	 		if (ok)
			{
				// Now ask all attributes to copy accross their component data
				NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)AttribsToApply.GetHead();
				while (pAttrItem && ok)
				{
					NodeAttribute* pAttr = pAttrItem->NodeAttribPtr;
					ERROR3IF(pAttr == NULL, "Where has our attribute gone ??"); 
					if (!pAttr->CopyComponentData(pSrcDoc, pDestDoc))
					{
						pDestDoc->AbortComponentCopy(); // Cancel all data which has been copied
						ok = FALSE; // stop what were doing
					} 
					pAttrItem = (NodeAttributePtrItem*)AttribsToApply.GetNext(pAttrItem);
				}
				if (ok)
					ok = pDestDoc->EndComponentCopy();
			}
		}

		if (ok)
		{
			// This fn will invoke an operation to apply the attributes to the selection. 

			// DMc - change the range to include parents
// This is inconsistent with all other apply operations
// So don't do it!
//			SelRange * pSel = GetApplication()->FindSelection();
//			RangeControl rg = pSel->GetRangeControlFlags();
//			rg.PromoteToParent = TRUE;
//			pSel->Range::SetRangeControl(rg);

			// Do the attribute application, adding actions to this op
//			AttributeManager::AttributesSelected(AttribsToApply, _R(IDS_PASTE_ATTRIBUTES)); 
			DoAttributesSelected(AttribsToApply, _R(IDS_PASTE_ATTRIBUTES), TRUE);

//			rg.PromoteToParent = FALSE;
//			pSel->Range::SetRangeControl(rg);
		}
		
		{
		// We don't need the list of attrs anymore
		NodeAttributePtrItem* pAttrItem = (NodeAttributePtrItem*)AttribsToApply.GetHead();
		while (pAttrItem)
		{
			delete (pAttrItem->NodeAttribPtr);
			pAttrItem->NodeAttribPtr = NULL;
			pAttrItem = (NodeAttributePtrItem*)AttribsToApply.GetNext(pAttrItem);
		}
		AttribsToApply.DeleteAll(); // tidyup	
		}
	}

	// ---------------------------------------------------------------------------
	// Now do the LiveEffects
	if (pClipboardRange && ok)
	{
		Range* pTempRange = new Range(*pClipboardRange);
		ENSURE(pTempRange!=NULL, "OpPasteAttributes::Do couldn't allocate memory for Range\n");
		if (pTempRange)
		{
			RangeControl rg = pTempRange->GetRangeControlFlags();
			rg.PromoteToParent = FALSE;
			pTempRange->SetRangeControl(rg);

			EffectsStack* pEffectsStack = EffectsStack::GetEffectsStackFromSelection(pTempRange, TRUE, TRUE);	// Include locked effects
			if (pEffectsStack!=NULL && !pEffectsStack->IsEmpty())
			{
PORTNOTE("effects", "Call to DoCopyPPStackToSelection removed from OpPasteAttributes")
#ifndef EXCLUDE_FROM_XARALX
				// Do the effect stack copy, adding actions to this op
				OpLiveEffect::DoCopyPPStackToSelection(this, pEffectsStack, pSrcDoc, pDestDoc);
#endif
			}
			if (pEffectsStack)
				delete pEffectsStack;

			delete pTempRange;
		}
	}

	End();
}			




