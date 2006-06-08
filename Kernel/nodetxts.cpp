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
*/

#include "camtypes.h"
#include "nodetxts.h"


// Code headers
#include "app.h"
#include "becomea.h"
#include "blobs.h"							
//#include "cameleps.h"
#include "contmenu.h"
#include "docview.h"
#include "fillattr.h"
#include "group.h"
#include "lineattr.h"
#include "nodetext.h"
#include "nodetxtl.h"
#include "objchge.h"
//#include "opbreak.h"
#include "ops.h"
#include "pathproc.h"
#include "rndrgn.h"
#include "trans2d.h"
#include "spread.h"
#include "textops.h"
#include "tool.h"
#include "txtattr.h"
#include "textfuns.h"
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "ai_epsrr.h"
#include "progress.h"
#include "camfiltr.h"
#include "cxftext.h"
#include "cxfrec.h"
#include "impstr.h"
#include "unicdman.h"
#include "extender.h"	// for ExtendParams
#include "ngcore.h"		// NameGallery, for stretching functionality
#include "nodepath.h"

// Resource headers
//#include "mario.h"
//#include "peter.h"
//#include "simon.h"
#include "textinfo.h"

#ifdef RALPH
#include "ralphcri.h" // For RalphCriticalSection
#endif

//#include "opbevel.h"	// For determining if a gradient fill is used in a TextStory.

DECLARE_SOURCE("$Revision$")				

CC_IMPLEMENT_DYNAMIC(BaseTextClass, NodeRenderableInk)
CC_IMPLEMENT_MEMDUMP(TextStoryInfo, CC_CLASS_MEMDUMP );
CC_IMPLEMENT_DYNAMIC(TextStory, BaseTextClass)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

// statics ...
TextStory* TextStory::pFocusStory = NULL;


/////////////////////////////////////////////////////////////////////////////////////////////
// BaseTextClass methods

/********************************************************************************************
>	void BaseTextClass::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	init function called by various constructors
********************************************************************************************/

void BaseTextClass::Init()
{
	mModifiedByOpFlag           = TRUE;		// ensures node will have metrics recached
	mDescendantModifiedByOpFlag = TRUE;		// ensures any children will have metrics cached
	mAffectedFlag               = TRUE;		// ensures node will be includes in redraw rect
	mDescendantAffectedFlag     = TRUE;		// ensures any children will be included in redraw
	mAlreadyWritten				= FALSE;	// ensures the char will be written to v2 files
}


/********************************************************************************************
>	BaseTextClass::BaseTextClass()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple BaseTextClass constructor
********************************************************************************************/

BaseTextClass::BaseTextClass(): NodeRenderableInk()	// Call the base class
{
	Init();
}


/********************************************************************************************
>	BaseTextClass::~BaseTextClass()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95
	Purpose:	BaseTextClass destructor
********************************************************************************************/

BaseTextClass::~BaseTextClass()
{
}


/********************************************************************************************
>	BaseTextClass::BaseTextClass(Node* ContextNode, AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.
		
				Direction:
			
				Specifies the direction in which the node is to be attached to the
				ContextNode. The values this variable can take are as follows:
								
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node

	Purpose:	The main BaseTextClass constructor
********************************************************************************************/

BaseTextClass::BaseTextClass(Node* ContextNode,
		 			 AttachNodeDirection Direction):NodeRenderableInk(ContextNode, Direction)
{
	Init();
}


/***********************************************************************************************
>	void BaseTextClass::CopyNodeContents(BaseTextClass* NodeCopy)

    Author:     Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
    Created:    3/5/95
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Assumption:	Only called when creating new copies and the constructor sets these flags correctly
***********************************************************************************************/

void BaseTextClass::CopyNodeContents(BaseTextClass* NodeCopy)
{
	// Ask the base class to do its bit
	NodeRenderableInk::CopyNodeContents(NodeCopy);
}


/***********************************************************************************************
>   void BaseTextClass::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void BaseTextClass::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(pNodeCopy->IsKindOf(CC_RUNTIME_CLASS(BaseTextClass)), "PolyCopyNodeContents given wrong dest node type");

	if (pNodeCopy->IsKindOf(CC_RUNTIME_CLASS(BaseTextClass)))
		CopyNodeContents((BaseTextClass*)pNodeCopy);
}



/********************************************************************************************
>	BOOL BaseTextClass::ReCacheNodeAndDescendantsMetrics()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Returns:	FALSE if fails
	Purpose:	Recache metrics in (this node and any text descendants) which have been ModifiedByOp
********************************************************************************************/

BOOL BaseTextClass::ReCacheNodeAndDescendantsMetrics(FormatRegion* pFormatRegion)
{
       PORTNOTE("text","BaseTextClass::ReCacheNodeAndDescendantsMetrics - do nothing");
#ifndef DISABLE_TEXT_RENDERING
	BOOL ok=TRUE;
	if (NodeOrDescendantModifiedByOp())
	{
		pFormatRegion->SaveContext();

		// scan immediate children rendering any text attrs, and recaching metrics on any text node
		Node* pNode = FindFirstChild();
		while (pNode!=NULL)
		{
			if (pNode->IsABaseTextClass())
				ok = ok && ((BaseTextClass*)pNode)->ReCacheNodeAndDescendantsMetrics(pFormatRegion);
			else if (pNode->IsKindOfTextAttribute())
				pNode->Render(pFormatRegion);
			pNode = pNode->FindNext();
		}

		// now all child text attrs rendered, recache this node's metrics
		if (this->ModifiedByOp())
			ok = ok && ReCacheMetrics(pFormatRegion);

		pFormatRegion->RestoreContext();
	}

	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	virtual BOOL BaseTextClass::ReCacheMetrics(FormatRegion* pFormatRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Inputs:		pFormatRegion -
	Returns:	FALSE if fails
	Purpose:	Recache metrics in text node - does nothing by default
********************************************************************************************/

BOOL BaseTextClass::ReCacheMetrics(FormatRegion* pFormatRegion)
{
	return TRUE;
}


/********************************************************************************************
>	void BaseTextClass::ClearFlags()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	clear all flags in the node
********************************************************************************************/

void BaseTextClass::ClearFlags()
{
	mModifiedByOpFlag           = FALSE;
	mDescendantModifiedByOpFlag = FALSE;
	mAffectedFlag               = FALSE;
	mDescendantAffectedFlag     = FALSE;
	mAlreadyWritten				= FALSE;
}


/********************************************************************************************
>	void BaseTextClass::ClearNodeAndDescendantsFlags()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Clear flags in node and all text descendants
	Assumption:	if any flag is set the AffectedFlag is set,
				and if AffectedFlag is set, its parents have DescendantAffectedFlag set
********************************************************************************************/

void BaseTextClass::ClearNodeAndDescendantsFlags()
{
	if (IS_A(this, CaretNode))
		SetOpPermission(PERMISSION_UNDEFINED);

	if (NodeOrDescendantAffected())
	{
		Node* pNode = FindFirstChild();
		while (pNode!=NULL)
		{
			if (pNode->IsABaseTextClass())
				((BaseTextClass*)pNode)->ClearNodeAndDescendantsFlags();
			pNode = pNode->FindNext();
		}
		ClearFlags();
	}
}


/********************************************************************************************
>	void BaseTextClass::FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Flag node and all text descendants that they have been directly modified by an op
				and flag all text parents that they have a text descendant directly modified by an op

********************************************************************************************/

void BaseTextClass::FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp()
{
	FlagModifiedByOp();
	FlagDescendantsModifiedByOp();
	FlagParentsHaveDescendantModifiedByOp();
}


/********************************************************************************************
>	void BaseTextClass::FlagParentsHaveDescendantModifiedByOp()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Flag all text parents that they have a text descendant directly modified by an op
********************************************************************************************/

void BaseTextClass::FlagParentsHaveDescendantModifiedByOp()
{
	// get parent
	Node* pNode = this->FindParent();
	if (pNode==NULL)
	{
// Can be called when text block is being inserted and does not yet have a parent???
//		ERROR3("BaseTextClass::FlagParentsHaveDescendantModifiedByOp() - node has no parent")
		return;
	}

	// if parent is a text node, set it's flag and flag it's parents
	if (pNode->IsABaseTextClass())
	{
		// if 'DescendantModifiedByOp' assume all it's parents already have this flag set
		BaseTextClass* pBTC = (BaseTextClass*)pNode;
		if (pBTC->DescendantModifiedByOp()==FALSE)
		{
			pBTC->FlagDescendantModifiedByOp();
			pBTC->FlagParentsHaveDescendantModifiedByOp();
		}
	}
}


/********************************************************************************************
>	void BaseTextClass::FlagDescendantsModifiedByOp()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Flag all text descendants as being directly modified by an op
********************************************************************************************/

void BaseTextClass::FlagDescendantsModifiedByOp()
{
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsABaseTextClass())
		{
			BaseTextClass* pBTC = (BaseTextClass*)pNode;
			pBTC->FlagModifiedByOp();
			pBTC->FlagDescendantsModifiedByOp();
		}
		pNode = pNode->FindNext();
	}
}


/********************************************************************************************
>	void BaseTextClass::FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Flag node and all text descendants that they have been 'affected'
				and flag all text parents that they have a text descendant 'affected'
********************************************************************************************/

void BaseTextClass::FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected()
{
	FlagAffected();
	FlagDescendantsAffected();
	FlagParentsHaveDescendantAffected();
}


/********************************************************************************************
>	void BaseTextClass::FlagParentsHaveDescendantAffected()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Flag all text parents that they have a text descendant 'affected'
********************************************************************************************/

void BaseTextClass::FlagParentsHaveDescendantAffected()
{
	// get parent
	Node* pNode = this->FindParent();
	if (pNode==NULL)
	{
		ERROR3("BaseTextClass::FlagParentsHaveDescendantAffected() - node has no parent");
		return;
	}

	// if parent is a text node, set it's flag and flag it's parents
	if (pNode->IsABaseTextClass())
	{
		// if 'DescendantAffected' assume all it's parents already have this flag set
		BaseTextClass* pBTC = (BaseTextClass*)pNode;
		if (pBTC->DescendantAffected()==FALSE)
		{
			pBTC->FlagDescendantAffected();
			pBTC->FlagParentsHaveDescendantAffected();
		}
	}
}


/********************************************************************************************
>	void BaseTextClass::FlagDescendantsAffected()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Purpose:	Flag all text descendants as being 'affected'
********************************************************************************************/

void BaseTextClass::FlagDescendantsAffected()
{
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsABaseTextClass())
		{
			BaseTextClass* pBTC = (BaseTextClass*)pNode;
			pBTC->FlagAffected();
			pBTC->FlagDescendantsAffected();
		}
		pNode = pNode->FindNext();
	}
}

/********************************************************************************************

>	void BaseTextClass::
		FlagPrevTextCharAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp()


	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/2000
	Purpose:	Flags the previous text node (on the current line only) as modified by op
				(this is useful for kerning where the previous char to the one that is inserted
				needs to be updated so it can find out if it is the left half of a kern pair)

********************************************************************************************/
void BaseTextClass::FlagPrevTextCharAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp()
{
	if (!this->IsAVisibleTextNode())
		return;

	VisibleTextNode* pVTN = static_cast<VisibleTextNode*>(this);
	TextChar* pTC = pVTN->FindPrevTextCharInStory();

	BaseTextClass* pBTC = pTC;
	if (pBTC)
	{
		pBTC->FlagModifiedByOp();
		pBTC->FlagDescendantsModifiedByOp();
		pBTC->FlagParentsHaveDescendantModifiedByOp();
	}
}

/********************************************************************************************
>	void BaseTextClass::UnionNodeAndDescendantsBounds(DocRect* pBounds, BOOL ParentModifiedByOp=FALSE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Inputs:		ParentModified - indicates if parent is ModifiedByOp
	Output:		pBounds        - updated to include bounds of all affected nodes
	Purpose:	Union bounds of the node and any text descendants 'affected' with the given bounds
				Any text nodes which are also 'ModifiedByOp' also have their BlobBounds included
				as they were selected at the start of the op.

	NOTES:		All 'affected' text descendants must have their bounds invalidated
				before the parent can have its bounds invalidated and recached.

				But (w.r.t. blob bounds) since a selected node can have no selected descendants
				AND BlobBounds are not dependent on descendants, if a node's parent is ModifiedByOp
				this node's blob bounds do not need to be included

				Should not be including BlobBounds in undo actions as they don't scale
				except if they're text blobs! - what about line & story blobs?
********************************************************************************************/

void BaseTextClass::UnionNodeAndDescendantsOldAndNewBounds(DocRect* pBounds, BOOL ParentModifiedByOp)
{
	if (NodeOrDescendantAffected())
	{
		Node* pNode = FindFirstChild();
		while (pNode!=NULL)
		{
			if (pNode->IsABaseTextClass())
				((BaseTextClass*)pNode)->UnionNodeAndDescendantsOldAndNewBounds(pBounds,ModifiedByOp());
			pNode = pNode->FindNext();
		}
	}

	if (Affected())
	{
		*pBounds = pBounds->Union(GetBoundingRect());
		if (ModifiedByOp() && !ParentModifiedByOp)
			*pBounds = pBounds->Union(GetBlobBoundingRect());

		InvalidateBoundingRect();

		*pBounds = pBounds->Union(GetBoundingRect());
		if (ModifiedByOp() && !ParentModifiedByOp)
			*pBounds = pBounds->Union(GetBlobBoundingRect());
	}
}



/********************************************************************************************
>	virtual BOOL BaseTextClass::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
																BOOL DoPreTriggerEdit = TRUE)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>; Karim 19/01/2000
	Created:	8/5/95
	Inputs:		pParam - pointer to the change parameter object
				SetOpPermissionState - TRUE to set the Nodes permission flags
				DoPreTriggerEdit	 = if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.
	Returns:	TRUE if the operation can proceede, FALSE to stop it
	Purpose:	Generic AllowOp() for all text nodes except TextStories
********************************************************************************************/

BOOL BaseTextClass::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState,
													BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"BaseTextClass::AllowOp() - pParam==NULL");
	ERROR2IF(Parent==NULL,FALSE,"BaseTextClass::AllowOp() - Parent==NULL");		// must have at least a TextStory

	// if a parent AllowOp() calls the TextStory AllowOp() this is as far down the tree as we
	// need to go to ensure the story is maintained correctly, hence the TextStory AllowOp()
	// does not call any child AllowOp()s
	// also, for similar reasons, any node in the story need only call it's parents AllowOp()s
	// and never its children
	// so we should never be called by a parent AllowOp() ...
	ERROR3IF(pParam->GetDirection()==OBJCHANGE_CALLEDBYPARENT,"BaseTextClass::AllowOp() - called by parent AllowOp()!");

	// decide if we allow it ...
	BOOL allowed=TRUE;
	ObjChangeFlags Flags=pParam->GetChangeFlags();
	if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD)
	{
		// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
		pParam->SetCallingChild(NULL);

		if (Flags.ReplaceNode || Flags.MultiReplaceNode || Flags.MoveNode)
			allowed=FALSE;
	}

	// and special case for caret (don't allow it to be deleted or copied)
	if (IS_A(this,CaretNode) && pParam->GetDirection()==OBJCHANGE_CALLEDBYOP)
		if (Flags.DeleteNode || Flags.CopyNode)
			allowed=FALSE;

	// if we didn't allow it set a reason
	if (allowed==FALSE)
		pParam->SetReasonForDenial(_R(IDE_TEXT_USEDBYSTORY));
		

	// if we allowed it, see if our parent allows it
	if (allowed)
	{
		// indicate we're calling from a child AllowOp()
		ObjChangeDirection OldDirection=pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed=Parent->AllowOp(pParam,SetOpPermissionState,DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if setting permisions ...
	if (SetOpPermissionState)
	{
		// (assume if parent allowed it, it set permission correctly on itself)
		// (and if called by a child, it did all the pre-op processing required)
		// (note we should never be called by our parent)
		if (allowed)
			allowed=PreOpProcessing(pParam);
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}


/********************************************************************************************
>	BOOL BaseTextClass::PreOpProcessing(ObjChangeParam* pParam)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/95
	Inputs:		pParam -
	Returns:	FALSE if fails
	Purpose:	Perform all preprocessing required before an op is performed on a text node
********************************************************************************************/

BOOL BaseTextClass::PreOpProcessing(ObjChangeParam* pParam)
{
	// if called by an AllowOp() which itself was NOT called by a child's AllowOp(), do our stuff
	// (assume if it WAS called by an AllowOp() which itself was called by a child AllowOp()
	// (this routine would have already been called by a child's AllowOp() which itself was
	// (called by an AlowOp() called directly by an Op!
	if (pParam->GetDirection()!=OBJCHANGE_CALLEDBYCHILD ||
		(pParam->GetCallingChild() != NULL &&
		pParam->GetCallingChild()->IsNodePath()) )
	{
		// if the TextStory has already been met by the op,
		// just union the affected node's bounds with the current redraw rect
		// else (we've been called for the first time in this op for this story) so,
		// flag the story has been met, reset the redraw rect, insert a single reformat action for undo
		TextStory* pTextStory = FindParentStory();
		if (pTextStory->NodeOrDescendantModifiedByOp())
			pTextStory->UpdateRedrawRect(GetBoundingRect());
		else
		{
			pTextStory->Validate();
			pTextStory->SetRedrawRect(GetBoundingRect());
			UndoableOperation* pOp=pParam->GetOpPointer();
			if (pOp!=NULL)
				if (PrePostTextAction::Init(pOp,pOp->GetUndoActionList(),pTextStory,TRUE)==AC_FAIL)
					return FALSE;
		}
		// also need to include blob bounds as obj must be selected to get here!
		pTextStory->UpdateRedrawRect(GetBlobBoundingRect());

		// flag the node is directly modified by the op
		FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();

		// flag prev char to allow for kerning
		FlagPrevTextCharAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();
	}
	return TRUE;
}


/********************************************************************************************
>	virtual CCRuntimeClass* BaseTextClass::GetCurrentAttribGroup()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/95
	Returns:	The current attribute group for all text nodes
	Purpose:	Every Ink object has an associated current attribute group, If an attribute
				is applied to the object, and it needs to become current then the attribute
				group specified will get updated.
********************************************************************************************/

CCRuntimeClass* BaseTextClass::GetCurrentAttribGroup()
{
	return (CC_RUNTIME_CLASS(BaseTextClass));
}


/********************************************************************************************
>	TextStory* BaseTextClass::FindParentStory()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/95
	Returns:	pointer to the (parent) text story (else NULL if error)
	Purpose:	get a pointer to the text story associated with this text node
				NB if 'this' is a text story, return 'this'
********************************************************************************************/

TextStory* BaseTextClass::FindParentStory()
{
	Node* pNode=this;
	while (IS_A(pNode,TextStory)==FALSE)
	{
		pNode = pNode->FindParent();
		if (pNode==NULL || pNode->IsABaseTextClass()==FALSE)
			ERROR2(NULL,"BaseTextClass::FindParentStory() - could not find associated TextStory");
	}
	return (TextStory*)pNode;
}


/********************************************************************************************
>	BOOL BaseTextClass::DoHideNode(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pUndoOp	- undoable operation (may be NULL if not undoable)
	Returns:	FALSE if fails
	Purpose:	As UndoableOperation::DoHideNode()
				BUT can be called non-undoably!!!
********************************************************************************************/

BOOL BaseTextClass::DoHideNode(UndoableOperation* pUndoOp)
{
	// deselect any node to hide to ensure 'parent of selected' flag correct
	if (IsSelected())
		SetSelected(FALSE);

	if (pUndoOp!=NULL)
		return pUndoOp->DoHideNode(this,FALSE,NULL,TRUE);

	// delete the node
	// (no need to update selection as non-undoable code only called for clipboard/import)
	// (no need to invalidate bounding box as text has it's own sytem to handle this)
	CascadeDelete();
	delete this;
	return TRUE;
}


/********************************************************************************************
>	BOOL BaseTextClass::DoInsertNewNode(UndoableOperation* pUndoOp,
					 					Node* pContextNode,
					 					AttachNodeDirection Direction)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pUndoOp				- undoable operation (may be NULL if not undoable)
				pContextNode		- see UndoableOperation::DoInsertNewNode()
				Direction			- see UndoableOperation::DoInsertNewNode()
	Returns:	FALSE if fails
	Purpose:	As UndoableOperation::DoInsertNewNode()
				BUT can be called non-undoably!!!
********************************************************************************************/

BOOL BaseTextClass::DoInsertNewNode(UndoableOperation* pUndoOp,
					 				Node* pContextNode,
					 				AttachNodeDirection Direction)
{
	BOOL ok = TRUE;
	if (pUndoOp!=NULL)
		ok = pUndoOp->DoInsertNewNode(this, pContextNode, Direction, FALSE,FALSE,FALSE,FALSE);
	else
	{
		// Insert the NewNode into the tree
		// (no need to invalidate bounding box as text has it's own sytem to handle this)
		// (assume layer is not locked and is visible)
		// (do not select the object, normalise it's attrs as this is done later)
		AttachNode(pContextNode, Direction);
	}

	if (ok) this->FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();

	// flag prev char to allow for kerning
	FlagPrevTextCharAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();

	return ok;
}


/********************************************************************************************
>	BOOL BaseTextClass::DoLocaliseCommonAttributes(	UndoableOperation* pUndoOp,
													BOOL CheckForDuplicates,
													BOOL Global,
													AttrTypeSet* pAffectedAttrTypes)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pUndoOp				- undoable operation (may be NULL if not undoable)
				CheckForDuplicated	- see UndoableOperation::DoLocaliseCommonAttributes()
				Global				- see UndoableOperation::DoLocaliseCommonAttributes()
				pAffectedAttrTypes	- see UndoableOperation::DoLocaliseCommonAttributes()
	Returns:	FALSE if fails
	Purpose:	As UndoableOperation::DoLocaliseCommonAttributes()
				BUT can be called non-undoably!!!
********************************************************************************************/

BOOL BaseTextClass::DoLocaliseCommonAttributes(	UndoableOperation* pUndoOp,
												BOOL CheckForDuplicates,
												BOOL Global,
												AttrTypeSet* pAffectedAttrTypes)
{
	if (pUndoOp!=NULL)
		return pUndoOp->DoLocaliseCommonAttributes(this,CheckForDuplicates,Global,pAffectedAttrTypes);

	return LocaliseCommonAttributes(CheckForDuplicates,Global,pAffectedAttrTypes);
}


/********************************************************************************************
>	BOOL BaseTextClass::DoFactorOutCommonChildAttributes(UndoableOperation* pUndoOp,
														 BOOL Global,
														 AttrTypeSet* pAffectedAttrTypes)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pUndoOp				- undoable operation (may be NULL if not undoable)
				Global				- see UndoableOperation::DoFactorOutCommonChildAttributes()
				pAffectedAttrTypes	- see UndoableOperation::DoFactorOutCommonChildAttributes()
	Returns:	FALSE if fails
	Purpose:	As UndoableOperation::DoFactorOutCommonChildAttributes()
				BUT can be called non-undoably!!!
********************************************************************************************/

BOOL BaseTextClass::DoFactorOutCommonChildAttributes(UndoableOperation* pUndoOp,
													 BOOL Global,
													 AttrTypeSet* pAffectedAttrTypes)
{
	if (pUndoOp!=NULL)
		return pUndoOp->DoFactorOutCommonChildAttributes(this,Global,pAffectedAttrTypes);

	return FactorOutCommonChildAttributes(Global,pAffectedAttrTypes);
}


/********************************************************************************************
>	void BaseTextClass::GetAttachNodeAndDirectionToAttachFirstChildObject(Node** ppNode,
																AttachNodeDirection* pDir)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Outputs:	ppNode - context node
				pDir   - attach direction
********************************************************************************************/

void BaseTextClass::GetAttachNodeAndDirectionToAttachFirstChildObject(Node** ppNode,
															AttachNodeDirection* pDir)
{
	*ppNode = this->FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
	*pDir   = PREV;

	// if no object to attach before, try next to last non-object node
	if (*ppNode==NULL)
	{
		*ppNode = this->FindLastChild();
		*pDir   = NEXT;
	}

	// if no child nodes, set first child of this
	if (*ppNode==NULL)
	{
		*ppNode = this;
		*pDir   = FIRSTCHILD;
	}
}


/********************************************************************************************
>	BOOL BaseTextClass::AddNonLineLevelDescendantAttrsToSet(AttrTypeSet* pAttrTypeSet)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/96
	Inputs:		pAttrTypeSet -
	Outputs:	pAttrTypeSet -
	Returns:	FALSE if fails
********************************************************************************************/

BOOL BaseTextClass::AddNonLineLevelDescendantAttrsToSet(AttrTypeSet* pAttrTypeSet)
{
	ERROR2IF(pAttrTypeSet==NULL,FALSE,"BaseTextClass::AddNonLineLevelDescendantAttrsToSet() - pAttrTypeSet==NULL");

	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		if ( pNode->IsAnAttribute() && !((NodeAttribute*)pNode)->IsALineLevelAttrib() )
		{
			NodeAttribute* pAttr = (NodeAttribute*)pNode;
			if (!pAttrTypeSet->AddToSet(pAttr->GetAttributeType()))
				return FALSE;
		}
		else if (pNode->IsABaseTextClass())
			if (!((BaseTextClass*)pNode)->AddNonLineLevelDescendantAttrsToSet(pAttrTypeSet))
				return FALSE;
		pNode = pNode->FindNext();
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// TextStoryInfo methods

/********************************************************************************************
>	TextStoryInfo::TextStoryInfo()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/96
	Purpose:	default constructor!
********************************************************************************************/

TextStoryInfo::TextStoryInfo()
{
	pUndoOp              = NULL;
	WordWrap             = TRUE;
	StoryWidth           = 0;		// assume text at a point
	WordWrapping         = FALSE;
	pPath                = NULL;
	PathLength           = 0;
	PathClosed           = FALSE;
	UnitDirectionVectorX = 0;
	UnitDirectionVectorY = 1;
	LeftPathIndent       = 0;
	RightPathIndent      = 0;
	DescentLine          = 0;
	DescentLineValid     = FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// TextStory methods

/********************************************************************************************
>	TextStory::TextStory()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple TextStory constructor, it is required so that SimpleCopy will work.
********************************************************************************************/

TextStory::TextStory(): BaseTextClass()	// Call the base class
{
	Init();
}


/********************************************************************************************
>	TextStory::~TextStory()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95
	Purpose:	The destructor currently just checks to see if the TextStory being deleted
				still has the input focus, and clears it if it has.  One situation where this
				occurs is when deleting a document.  This dosen't clear the selection
********************************************************************************************/

TextStory::~TextStory()
{
	if (GetFocusStory() == this)
		SetFocusStory(NULL);

	if (pImportedStringList != NULL)
	{
		pImportedStringList->DeleteAll();
		delete pImportedStringList;
		pImportedStringList = NULL;
	}
}


/********************************************************************************************
>	TextStory::TextStory(Node* ContextNode, AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.
		
				Direction:
			
				Specifies the direction in which the node is to be attached to the
				ContextNode. The values this variable can take are as follows:
								
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node

	Purpose:	The main TextStory constructor
********************************************************************************************/

TextStory::TextStory(Node* ContextNode,
		 			 AttachNodeDirection Direction):BaseTextClass(ContextNode, Direction)
{
	Init();
}


/********************************************************************************************
>	void TextStory::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/95
	Outputs:	Set member variables to default values.
	Purpose:	Initialies the member variables of the TextStory
********************************************************************************************/

void TextStory::Init()
{
	StoryMatrix = Matrix();
	RedrawRect  = DocRect(0,0,0,0);

	StoryWidth      = 0;
	mLeftIndent		= 0;
	mRightIndent	= 0;
	CachedCaret     = NULL;

	TextOnPathReversed   = FALSE;
	TextOnPathTangential = TRUE;
	PrintAsShapes        = FALSE;
	WordWrapping         = FALSE;
	BeingCopied          = FALSE;

	ImportFormatWidth = 0;
	ImportBaseShift   = AlignBaseline;

	CharsScale    = 1;
	CharsAspect   = 1;
	CharsRotation = 0;
	CharsShear    = 0;

	pImportedStringList	 = NULL;	// Used when importing strings in the v2 file format

	AutoKern = true;
}


/********************************************************************************************
>	Node* TextStory::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	A copy of the node, or NULL if we are out of memory
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL.
				The function is virtual, and must be defined for all derived classes of Node
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL.
********************************************************************************************/

Node* TextStory::SimpleCopy()
{
	// Make a new TextStory and then copy things into it
	TextStory* NodeCopy = new TextStory();

    ERROR1IF(NodeCopy==NULL, NULL, _R(IDE_NOMORE_MEMORY));

    if (NodeCopy)
    	CopyNodeContents(NodeCopy);

	return NodeCopy;
}


/***********************************************************************************************
>   void TextStory::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void TextStory::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, TextStory), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, TextStory))
		CopyNodeContents((TextStory*)pNodeCopy);
}



/***********************************************************************************************
>	void TextStory::CopyNodeContents(TextStory* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
***********************************************************************************************/

void TextStory::CopyNodeContents(TextStory* NodeCopy)
{
	// Ask the base class to do its bit
	BaseTextClass::CopyNodeContents(NodeCopy);

	// Copy the rest of the data (which should not be that set by the default constructor?)
	NodeCopy->StoryMatrix			= StoryMatrix;

	NodeCopy->StoryWidth			= StoryWidth;

	NodeCopy->TextOnPathReversed	= TextOnPathReversed;
	NodeCopy->TextOnPathTangential	= TextOnPathTangential;
	NodeCopy->PrintAsShapes			= PrintAsShapes;
	NodeCopy->WordWrapping			= WordWrapping;

	NodeCopy->ImportFormatWidth		= ImportFormatWidth;
	NodeCopy->ImportBaseShift		= ImportBaseShift;

	NodeCopy->CharsScale			= CharsScale;
	NodeCopy->CharsAspect			= CharsAspect;
	NodeCopy->CharsRotation			= CharsRotation;
	NodeCopy->CharsShear			= CharsShear;
	
	NodeCopy->mLeftIndent			= mLeftIndent;
	NodeCopy->mRightIndent			= mRightIndent;

	NodeCopy->AutoKern				= AutoKern;
}


/********************************************************************************************
>	String TextStory::Describe(BOOL Plural, BOOL Verbose = TRUE)  		

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		Plural:	Singular or plural description
	Returns:	A string describing the node
	Purpose:	Gives a description of the TextStory node for the status line etc
********************************************************************************************/

String TextStory::Describe(BOOL Plural, BOOL Verbose)  		
{
	if (Plural)
		return(String(_R(IDS_DESCRIBE_TEXTSTORYP)));
	else
		return(String(_R(IDS_DESCRIBE_TEXTSTORYS)));
}


/********************************************************************************************
>	UINT32 TextStory::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node
********************************************************************************************/

UINT32 TextStory::GetNodeSize() const
{
	return (sizeof(TextStory));
}


/********************************************************************************************
>	static TextStory* TextStory::CreateTextObject(DocCoord Anchor)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/95
	Inputs:		Anchor - The position the text story should be created at.
	Returns:	A pointer to the root of the text object subtree if successful, else NULL
	Purpose:	Creates a Text object.
	Errors:		An ERROR will be set if we run out of memory
********************************************************************************************/

TextStory* TextStory::CreateTextObject(DocCoord Anchor)
{
	Matrix StoryMat(Anchor.x, Anchor.y);
	return CreateTextObject(StoryMat);
}


/********************************************************************************************
>	static TextStory* TextStory::CreateTextObject(Matrix TheMatrix)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/03/95
	Inputs:		TheMatrix - the matrix to initialise the story with (includes inital position)
	Returns:	A pointer to the root of the text object subtree if successful, else NULL
	Purpose:	Creates a Text object
	Errors:		An ERROR will be set if we run out of memory
********************************************************************************************/

TextStory* TextStory::CreateTextObject(Matrix TheMatrix)
{
	// Pointers to objects we will create to construct a TextStory
	TextStory* pTextStory = NULL;
	TextLine* pTextLine = NULL;
//	CaretNode* pCaret = NULL;
//	EOLNode* pEOLN = NULL;
	BOOL ok = TRUE;

	// Try to create the root TextStory node
	pTextStory = new TextStory();
	ok = (pTextStory != NULL);

	// Set the matrix so that the origin is translated to the start point
	if (ok)
		pTextStory->SetStoryMatrix(TheMatrix);

	// Create a line for the story
	if (ok)
	{
		pTextLine = new TextLine(pTextStory, LASTCHILD);
		ok = (pTextLine != NULL);
	}

	// Add an EOLNode to the line
	if (ok)
		ok = (new EOLNode(pTextLine, FIRSTCHILD) != NULL);

	// Create the TextStory's Caret node
	if (ok)
		ok = (new CaretNode(pTextLine, FIRSTCHILD) != NULL);

	// Clean up if failure
	if (!ok && (pTextStory != NULL))
	{
		pTextStory->CascadeDelete();
		delete pTextStory;
		pTextStory = NULL;
	}

	return pTextStory;
}


/********************************************************************************************
>	void TextStory::GetDebugDetails(StringBase* Str)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node.
				This fn can be deleted before we ship
********************************************************************************************/

void TextStory::GetDebugDetails(StringBase* Str)
{
#if DEBUG_TREE

	BaseTextClass::GetDebugDetails(Str);

 	String_256			TempStr;
	String_256			TempStr2;
	TCHAR				floatStr[20];
		
	(*Str) += TEXT( "\r\nText story Data Dump\r\n" );

	fixed16	abcd[4];
	INT32	ef[2];
	StoryMatrix.GetComponents(abcd, ef);

	TempStr._MakeMsg( TEXT("\r\nMatrix\r\n"));	
	(*Str) += TempStr;
	camSnprintf( floatStr, 20, _T("%f,%f"), abcd[0].MakeDouble(), abcd[1].MakeDouble() );
	TempStr._MakeMsg( TEXT("a, b :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	camSnprintf( floatStr, 20, _T("%f,%f"), abcd[2].MakeDouble(), abcd[3].MakeDouble() );
	TempStr._MakeMsg( TEXT("c, d :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("e, f :\t#1%ld,\t#2%ld\r\n"), ef[0], ef[1]);
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("\r\nIndents"));	
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("\tLeft\t#1%ld\r\n"), GetLeftIndent());
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("\tRight\t#1%ld\r\n"), GetRightIndent());
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("\tStory Width\t#1%ld\r\n"), GetStoryWidth());
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("\r\nFlags"));	
	(*Str) += TempStr;
	if (IsTextOnPathReversed())
		TempStr._MakeMsg( TEXT("\tText on path is reversed\r\n"));	
	else
		TempStr._MakeMsg( TEXT("\tText on path is NOT reversed\r\n"));	
	(*Str) += TempStr;
	if (IsTextOnPathTangential())
		TempStr._MakeMsg( TEXT("\tText on path is tangential\r\n"));	
	else
		TempStr._MakeMsg( TEXT("\tText on path is NOT tangential\r\n"));	
	(*Str) += TempStr;
	if (IsPrintingAsShapes())
		TempStr._MakeMsg( TEXT("\tText is printing as shapes\r\n"));	
	else
		TempStr._MakeMsg( TEXT("\tText is NOT printing as shapes\r\n"));	
	(*Str) += TempStr;
	if (IsWordWrapping())
		TempStr._MakeMsg( TEXT("\tText is word wrapping\r\n"));
	else
		TempStr._MakeMsg( TEXT("\tText is NOT word wrapping\r\n"));
	(*Str) += TempStr;
	if (IsAutoKerning())
		TempStr._MakeMsg( TEXT("\tText is auto kerned\r\n"));
	else
		TempStr._MakeMsg( TEXT("\tText is NOT auto kerned\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("\tCharsScale = #1%ld\r\n"), CharsScale.GetRawLong() );
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("\tCharsAspect = #1%ld\r\n"), CharsAspect.GetRawLong() );
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("\tCharsRotation = #1%ld\r\n"), CharsRotation.GetRawLong() );
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("\tCharsShear = #1%ld\r\n"), CharsShear.GetRawLong() );
	(*Str) += TempStr;
	

#endif
}


/********************************************************************************************
>	CaretNode* TextStory::GetCaret()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/2/95
	Returns:	A pointer to the TextStory's caret node, or NULL
	Purpose:	Returns a pointer to the caret node of this TextStory, NULL will be returned
				if there is no Caret.
********************************************************************************************/

CaretNode* TextStory::GetCaret()
{
	// if there's a caret pointer in the cache then use it
	if (CachedCaret)
		return CachedCaret;

	// otherwise find the thing inside the story
	Node* pNode=FindFirstDepthFirst();
	while (pNode != NULL)
	{
		if (IS_A(pNode,CaretNode))
			break;
	 	pNode=pNode->FindNextDepthFirst(this);
	}
	
	// if we can't find the thing its trouble!
	ERROR2IF(pNode==NULL,NULL,"TextStory::GetCaret() - could not find caret!");

	CachedCaret = (CaretNode*)pNode;	
	return CachedCaret;
}



/********************************************************************************************
>	virtual BOOL TextStory::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
															BOOL DoPreTriggerEdit = TRUE)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>; Karim 19/01/2000
	Created:	6/5/95
	Inputs:		pParam - pointer to the change parameter object
				SetOpPermissionState - TRUE to set the Nodes permission flags
				DoPreTriggerEdit	 - if TRUE then calls NameGallery::PreTriggerEdit.
									   *Must* be TRUE if the calling Op may make any nodes
									   change their bounds, eg move, line width, cut.
									   Use TRUE if unsure.
	Returns:	TRUE if the operation can proceede, FALSE to stop it
	Purpose:	Allows the TextStory to abort the following operations under the following
				circumstances
********************************************************************************************/
BOOL TextStory::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState,
												BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL,FALSE,"TextStory::AllowOp() - pParam==NULL");

	// decide if we allow it ...

	// Karim 14/11/2000
	//	Added the !=OBJCHANGE_CALLEDBYCHILD clause.
	//	TextStories don't usually pass AllowOp down to their children.
	//	This is an exception - see TextStory::AllowOp_AccountForCompound for more info.
	BOOL allowed=TRUE;
	ObjChangeFlags Flags=pParam->GetChangeFlags();
	if (pParam->GetDirection() != OBJCHANGE_CALLEDBYCHILD)
		AllowOp_AccountForCompound(pParam, SetOpPermissionState, DoPreTriggerEdit);

	else
	{
		// we disallow changes to our text chars, but any child NodePath is fair game.
		if (pParam->GetCallingChild() == NULL || !pParam->GetCallingChild()->IsNodePath())
		{
			if (Flags.ReplaceNode || Flags.MultiReplaceNode)
			{
				pParam->SetReasonForDenial(_R(IDE_TEXT_USEDBYSTORY));
				allowed=FALSE;
			}
		}
	}

	// if we allow it, (and our parents weren't calling us) see if our parents do ...
	if (allowed && pParam->GetDirection()!=OBJCHANGE_CALLEDBYPARENT && Parent!=NULL)
	{
		// pass a temporary copy of the ObjChangeParam to our parent indicating call direction
		// and if all our children are deleted we know we don't delete ourselves so clear delete flag
		ObjChangeFlags NewFlags=pParam->GetChangeFlags();
		if (pParam->GetDirection()==OBJCHANGE_CALLEDBYCHILD)
			NewFlags.DeleteNode=FALSE;
		ObjChangeParam ObjParam(pParam->GetChangeType(), NewFlags, pParam->GetChangeObj(),
								pParam->GetOpPointer(),  OBJCHANGE_CALLEDBYCHILD);
		ObjParam.SetCallingChild(this);
		allowed=Parent->AllowOp(&ObjParam,SetOpPermissionState,DoPreTriggerEdit);
	}

	// if setting permisions ...
	if (SetOpPermissionState)
	{
		if (allowed)
		{
			if (Parent!=NULL)
				Parent->SetOpPermission(PERMISSION_ALLOWED);

			// if post process required,
			// ensure our OnChildChange is called for post processing (by setting permission on ourself),
			// and do all pre-op processing we need to do for text
			if (pParam->GetDirection()!=OBJCHANGE_CALLEDBYPARENT || Flags.Attribute || Flags.TransformNode)
			{
				SetOpPermission(PERMISSION_ALLOWED);
				allowed=PreOpProcessing(pParam);
			}
		}
		else
			SetOpPermission(PERMISSION_DENIED,TRUE);
	}

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}

	// Ilan 8/5/00
	// Inform geom linked attrs of the change.
	if(allowed)
	{
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();
		if(InformGeomLinkedAttrs)
		{
			NodeAttribute* pNA = FindFirstGeometryLinkedAttr();
			while(pNA)
			{
				pNA->LinkedNodeGeometryHasChanged(pChangeOp);
				pNA = pNA->FindNextGeometryLinkedAttr();
			}
		}
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}



/********************************************************************************************

>	virtual BOOL TextStory::AllowOp_AccountForCompound(	ObjChangeParam* pParam,
														BOOL SetOpPermissionState,
														BOOL DoPreTriggerEdit	)
	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/2000

	Purpose:	Req'd so that the curve bit of text-on-a-curve gets AllowOp messages, thus
				allowing feathered curve bits to update correctly.

				This is a very much cut-down version of Node::AllowOp_AccountForCompound,
				tailored to the way TextStories behave - DON'T COPY IT!

	See also:	Node::AllowOp_AccountForCompound().

********************************************************************************************/
BOOL TextStory::AllowOp_AccountForCompound(ObjChangeParam* pParam, BOOL SetOpPermissionState,
															  BOOL DoPreTriggerEdit)
{
	// we only do the biz if an attr changes, or if we're transformin' or regeneratin'.
	ObjChangeFlags Flags = pParam->GetChangeFlags();
	if (Flags.Attribute || Flags.TransformNode || Flags.RegenerateNode)
	{
		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetDirection(OBJCHANGE_CALLEDBYPARENT);

		for (Node*	pNode =  FindFirstChild();
					pNode != NULL;
					pNode =  pNode->FindNext())
		{
			if (pNode->IsNodePath())
				pNode->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
		}

		pParam->SetDirection(OldDirection);
	}

	return TRUE;
}



/********************************************************************************************
>	virtual ChangeCode TextStory::OnChildChange(ObjChangeParam* pParam)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/03/95
	Inputs:		pParam	= 	pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	This function is called at the end of any operation which has affected a TextStory
				in order to (undoably) reformat the story and redraw affected bits
********************************************************************************************/
ChangeCode TextStory::OnChildChange(ObjChangeParam* pParam)
{
	ERROR2IF(pParam==NULL,CC_FAIL,"TextStory::OnChildChange() - pParam==NULL");

	// ignore all but OBJCHANGE_FINISHED codes
	if (pParam->GetChangeType()!=OBJCHANGE_FINISHED)
		return CC_OK;

	// The AllowOp system will have called the CaretNode and it will have set
	// PERMISSION_DENIED on itself. We must ensure that that is cleared at the
	// end of the operation.
	if (IS_A(this, CaretNode))
		SetOpPermission(PERMISSION_UNDEFINED);

	// if we have an op pointer format and redraw the story undoably else just do it
	// (ie consecutive attribute applies only do the first one undoably)
	BOOL				ok = TRUE;
	UndoableOperation* pOp=pParam->GetOpPointer();
	if (pOp!=NULL)
	{
		if (ok)
			ok = PrePostTextAction::DoFormatStory(pOp,this);
		if (ok)
		{
			DocRect temprect = RedrawRect;
			// Don't call ReleaseCached here without very good reason
			// Some Ops which call this function have carefully controlled
			// their cacheing so that unwanted re-renders are avoided.
			// Calling ReleaseCached here blows all that control away
			if (!pParam->GetRetainCachedData())
			{
				ReleaseCached(TRUE, FALSE);
				BOOL bFoundEffects = FALSE;
				DocRect effectrect = GetEffectStackBounds(&bFoundEffects);
				if (bFoundEffects) temprect = temprect.Union(effectrect);
			}

			ok = pOp->DoInvalidateRegion(FindParentSpread(), temprect);
		}
	}
	else
	{
		if (ok)
			ok = FormatAndChildren(pOp,TRUE);

		BaseDocument* pOwnerDoc = FindOwnerDoc();
		ERROR2IF(pOwnerDoc==NULL,CC_FAIL,"TextStory::OnChildChange() - pDoc==NULL");
		if (ok && IS_A(pOwnerDoc, Document))
		{
			DocRect temprect = RedrawRect;
			if (!pParam->GetRetainCachedData())
			{
				ReleaseCached(TRUE, FALSE);
				BOOL bFoundEffects = FALSE;
				DocRect effectrect = GetEffectStackBounds(&bFoundEffects);
				if (bFoundEffects) temprect = temprect.Union(effectrect);
			}
			((Document*)pOwnerDoc)->ForceRedraw(FindParentSpread(), temprect, TRUE, this, FALSE);
		}
	}

	if (ok)
		return CC_OK;
 	else
 		return CC_FAIL;
}


/********************************************************************************************
>	virtual BOOL TextStory::CanBecomeA(BecomeA* pBecomeA)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/02/94
	Inputs:		InkClass: The class	of object to turn into
				pNumObjects = ptr to place number of objects of type pClass that will be created (Note: can be NULL).
							  *pNumObects in undefined on entry
	Returns:	TRUE if the node, or any of its children can transmogrify themselves to become
				an InkClass object
	Purpose:	This function is used by the convert to shapes operation. It determines if
				the node or any of its children can convert themselves into an InkClass object.

				The number you put into pNumObjects (if it's not NULL) should exactly equal the total number
				of pClass objects you create.  It should NOT contain any additional objects you may produce
				such as group objects for containing the pClass object, or attributes.

				Also, the entry value of *pNumObjects cannot be assumed to be 0.
********************************************************************************************/

BOOL TextStory::CanBecomeA(BecomeA* pBecomeA)
{
	// A TextStory can become a NodePath
	if (pBecomeA->BAPath())
	{
		if (pBecomeA->IsCounting())
		{
			// Sum the number of paths our immediate children will create
			Node* pNode = FindFirstDepthFirst();
			while (pNode!=NULL && pNode!=this)
			{
				// Call child to update BecomeA count
				pNode->CanBecomeA(pBecomeA);

				// Text chars don't have their own CanBecomeA, so count them here.
				if (IS_A(pNode, TextChar) && !((TextChar*)pNode)->IsAVisibleSpace())
					pBecomeA->AddCount(1);

				pNode = pNode->FindNextDepthFirst(this);
			}
		}

		return TRUE;
	}

	return FALSE;
}


/********************************************************************************************
>	virtual BOOL TextStory::DoBecomeA(BecomeA* pBecomeA)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Inputs:		pBecomeA - ptr to class containing info needed to become a new type of node.
	Returns:	FALSE if fails
	Purpose:	Transforms the object into another type of object.
********************************************************************************************/

BOOL TextStory::DoBecomeA(BecomeA* pBecomeA)
{

// BODGE TEXT - pass back should delete nodes if it fails - though MarkN claimed not when I wrote this!

	ERROR2IF(pBecomeA==NULL,FALSE,"TextStory::DoBecomeA() - pBecomeA==NULL");

	// here to overcome scope problem
	Node* pNode=NULL;

	// create a format region to keep an attribute stack
	FormatRegion FRegion;
	FormatRegion* pFormatRegion=&FRegion;
	if (pFormatRegion->Init(this)==FALSE)
		return FALSE;
	pFormatRegion->SaveContext();

	// if not passing back, create a NodeGroup to encompass the story
	// BODGE - should use ALLOC_WITH_FAIL
	NodeGroup* pStoryNodeGroup=NULL;
	if (pBecomeA->GetReason()!=BECOMEA_PASSBACK)
	{
		pStoryNodeGroup=new NodeGroup;
		if (pStoryNodeGroup==NULL)
			goto Fail;

		// render story level attrs and copy non-text attrs
		pNode=FindFirstChild();
		while (pNode)
		{
			if (pNode->IsAnAttribute())
			{
				pNode->Render(pFormatRegion);	// render attributes
				if (!pNode->IsKindOfTextAttribute())
					if (pNode->CopyNode(pStoryNodeGroup, LASTCHILD)==FALSE)
						goto Fail;
			}
			pNode=pNode->FindNext();
		}
	}

	// convert all the story's childs either attaching to the StoryNodeGroup or PassingBack
	pNode=FindFirstChild();
	while (pNode)
	{
		// convert all TextLines to groups (inc attrs and chars)
		if (IS_A(pNode,TextLine))
		{
			TextLine* pTextLine=(TextLine*)pNode;
			NodeGroup* pLineNodeGroup=NULL;
			if (pTextLine->CreateNodeGroup(&pLineNodeGroup,pFormatRegion,pBecomeA)==FALSE)
				goto Fail;
			if (pBecomeA->GetReason()!=BECOMEA_PASSBACK)
				pLineNodeGroup->AttachNode(pStoryNodeGroup,LASTCHILD);
		}
		// copy any NodePath with it's children attrs
		if (pNode->IsNodePath())
		{
			NodePath* pNodePath=(NodePath*)pNode;
			NodePath* pNodePathCopy=(NodePath*)pNodePath->SimpleCopy();
			if (pNodePathCopy==NULL)
				goto Fail;
			if (pNodePath->CopyChildrenTo(pNodePathCopy)==FALSE)
				goto Fail;
			if (pBecomeA->GetReason()!=BECOMEA_PASSBACK)
			{
				pNodePathCopy->AttachNode(pStoryNodeGroup,LASTCHILD);
				pBecomeA->PassBack(pNodePathCopy, pNodePath);
			}
			else
				if (pBecomeA->PassBack(pNodePathCopy,pNodePath)==FALSE)
					goto Fail;
		}
		pNode=pNode->FindNext();
	}

	// Must do this here or else the current attributes in the region
	// will have been deleted
	pFormatRegion->RestoreContext();

	// now either insert the replace the story or pass it back
	switch (pBecomeA->GetReason())
	{
		case BECOMEA_REPLACE:
		{
			UndoableOperation* pUndoOp = pBecomeA->GetUndoOp();

			// Deselect the node
			SetSelected(TRUE);		// ensures all children are deselected
			SetSelected(FALSE);

			// are we undoable or not ...
			if (pUndoOp!=NULL)
			{
				// hide the text story
				NodeHidden* pNodeHidden;
				if (pUndoOp->DoHideNode(this,FALSE,&pNodeHidden)==FALSE)
					goto FailNoRestore;
				// Insert the NodeGroup next to the old hidden TextStory
				if (!pUndoOp->DoInsertNewNode(pStoryNodeGroup,pNodeHidden,NEXT,FALSE,FALSE,FALSE,FALSE))
					goto FailNoRestore;

			}
			else
			{
				// attach the NodeGroup next to the text story, then delete 'this' TextStory - eek!
				pStoryNodeGroup->AttachNode(this,NEXT);
				CascadeDelete();
				delete this;
			}

			// Select the group, but only if the caller wants us too (moulds don't, for example)
			if (pBecomeA->Reselect() && pUndoOp!=NULL)
				if (!pUndoOp->DoSelectNode(pStoryNodeGroup,FALSE))
					return FALSE;

			break;
		}

		case BECOMEA_PASSBACK:
			// all pass back done by children!
			break;

		default:
			ERROR2_PF(FALSE,("Unknown BecomeA reason %d",pBecomeA->GetReason()));
			break;
	}

	return TRUE;


Fail:
	pFormatRegion->RestoreContext();

FailNoRestore:
	if (pStoryNodeGroup)
	{
		pStoryNodeGroup->CascadeDelete();
		delete pStoryNodeGroup;
	}

	return TRUE;
}


/********************************************************************************************
>	static TextStory* TextStory::GetFocusStory()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/02/95
	Returns:	Pointer to the TextStory with the active caret (can be NULL).
	Purpose:	This function should be used to get a pointer to the TextStory with the input
				focus.  There can be only one such story with an active caret at any time,
				although there does not have to aways be one (in this case NULL is returned)
********************************************************************************************/

TextStory* TextStory::GetFocusStory()
{
	return pFocusStory;
}


/********************************************************************************************
>	static void TextStory::SetFocusStory(TextStory* pNewStory)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/2/95
	Inputs:		pNewStory - pointer to a TextStory to giv the input focus to
							NULL to give no TextStory the input focus
	Returns:	FALSE if an error occured
	Purpose:	This function should be used to set the pointer with the input focus.
				Setting the pointer to NULL clears the into focus.
	Errors:		Attempts to set the intput focus to a non TextStory object.	  (ERROR3)	
********************************************************************************************/

void TextStory::SetFocusStory(TextStory* pNewStory)
{
	if ((pNewStory != NULL) && (!IS_A(pNewStory,TextStory)) )
	{
		ERROR3("Attempted to set FocusTextStory to a non-TextStory object");
		pNewStory = NULL;
	}

	pFocusStory = pNewStory;
}


/********************************************************************************************
>	NodePath TextStory::GetTextPath()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/02/94
	Returns:	A pointer to the path or NULL if none
	Purpose:	This function should be used to get a pointer to the path the text is on.
				NULL is returned if there is no text path.
********************************************************************************************/
NodePath* TextStory::GetTextPath() const
{
	return (NodePath*)FindFirstChild(CC_RUNTIME_CLASS(NodePath));
}


/********************************************************************************************
>	TextLine* TextStory::FindFirstLine()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/03/95
	Returns:	A pointer to the TextStory's first line
	Purpose:	This function should be used to obtain a pointer to the TextStory's
				first TextLine node.
********************************************************************************************/
TextLine* TextStory::FindFirstLine() const
{
	return (TextLine*)FindFirstChild(CC_RUNTIME_CLASS(TextLine));
}


/********************************************************************************************
>	TextLine* TextStory::FindLastLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	ptr to last TextLine in story
********************************************************************************************/
TextLine* TextStory::FindLastLine() const
{
	return (TextLine*)FindLastChild(CC_RUNTIME_CLASS(TextLine));
}


/********************************************************************************************
>	 void TextStory::RenderObjectBlobs(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		pRenderRegion -
	Purpose:	Render the text story's object blobs
********************************************************************************************/

void TextStory::RenderObjectBlobs(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pRenderRegion != NULL)
	{
   		pRenderRegion->SetLineColour(COLOUR_NONE);
		pRenderRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);

		pRenderRegion->DrawBlob(GetBlobPosAndSize(), BT_UNSELECTED);
	}
	else
		ERROR3("TextStory::RenderTinyBlobs() - pRenderRegion==NULL");
#endif
}


/********************************************************************************************
>	 void TextStory::RenderTinyBlobs(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Inputs:		pRenderRegion -
	Purpose:	Render the text story's tiny blobs
********************************************************************************************/

void TextStory::RenderTinyBlobs(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pRenderRegion != NULL)
	{
   		pRenderRegion->SetLineColour(COLOUR_NONE);
		pRenderRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRenderRegion->DrawBlob(GetBlobPosAndSize(), BT_UNSELECTED);
	}
	else
		ERROR3("TextStory::RenderTinyBlobs() - pRenderRegion==NULL");
#endif
}


/********************************************************************************************
>	DocRect TextStory::GetBlobBoundingRect();

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Returns:	The rectangle of the document covered by the blobs of this text story
********************************************************************************************/

DocRect TextStory::GetBlobBoundingRect()
{
	// get the bounds of the  test story's blob
	INT32     BlobSize=0;
	DocCoord BlobPos=GetBlobPosAndSize(&BlobSize);
	DocRect  BlobBounds(BlobPos,BlobPos);
	
	BlobBounds.Inflate(BlobSize/2);

	IncludeChildrensBoundingRects(&BlobBounds);
	return BlobBounds;
}


/********************************************************************************************
>	DocCoord TextStory::GetBlobPosAndSize(INT32* pSize=NULL)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Outputs:	pSize - size of a blob if required
	Returns:	coord to position blob
********************************************************************************************/

DocCoord TextStory::GetBlobPosAndSize(INT32* pSize)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the bounds of the text story
	DocRect StoryBounds=GetBoundingRect();

	// get the size of a blob
	INT32 BlobSize=0;
	BlobManager* pBlobMgr=GetApplication()->GetBlobManager();
	if (pBlobMgr)
		BlobSize=pBlobMgr->GetBlobSize();
	else
		ERROR3("TextStory::GetObjectBlobPos() - GetBlobManager() returned NULL");

	if (pSize) *pSize=BlobSize;
	return DocCoord( StoryBounds.lo.x-BlobSize/2, StoryBounds.hi.y+BlobSize/2 );
#else
	return DocCoord(0,0);
#endif
}		


/********************************************************************************************
>	virtual BOOL TextStory::HidingNode()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/95
	Outputs:	Can update pFocusStory if it points to this TextStory
	Returns:	The base class result
	Purpose:	Called when this TextStory is hidden - Clears the input focus from this
				TextStory if it currently has it, then calls the parent class.
********************************************************************************************/

BOOL TextStory::HidingNode()
{
	// Clear the focus
	if (GetFocusStory() == this)
	{
		SetFocusStory(NULL);
	}

	// Call the parent for it's processing
	return BaseTextClass::HidingNode();
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretToCharacter(VisibleTextNode* pChar, AttachNodeDirection Dir)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Inputs:		pChar - points to the character to position the caret by	
				Dir - direction relative to character to position
	Returns:	TRUE if the caret was successfuly moved
				FALSE if an error occured
	Purpose:	This routine moves the caret to the left or right of a specified char
********************************************************************************************/

BOOL TextStory::MoveCaretToCharacter(VisibleTextNode* pChar, AttachNodeDirection Dir)
{
	ERROR2IF(Dir!=PREV && Dir!=NEXT,FALSE,"Dir must be PREV or NEXT");
	ERROR2IF(pChar==NULL,FALSE,"Attempted to move the caret to NULL");
	CaretNode* pCaret=GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"TextStory::MoveCaretToCharacter() - TextStory has no caret");

	// ensure not inserted after EOL
	AttachNodeDirection AttachDir = Dir;
	if (pChar->IsAnEOLNode() && AttachDir==NEXT)
		AttachDir = PREV;

	pCaret->MoveNode(pChar, AttachDir);
	pCaret->HasMoved();
	return AttachCaretAttributes();
}


/********************************************************************************************
>	VisibleTextNode* TextStory::GetPrevWordChar(VisibleTextNode* pStartChar)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pStartChar - char to move from (usually the caret)
	Returns:	ptr to first char in previous word, or char after EOL or EOL
********************************************************************************************/

VisibleTextNode* TextStory::GetPrevWordChar(VisibleTextNode* pStartChar)
{
	ERROR2IF(pStartChar==NULL,NULL,"TextStory::GetPrevWordChar() - pStartChar==NULL");

	// skips chars until non-space found, then return ptr to last non-space before space
	// also return ptr to char immediately after EOL and EOL
	BOOL NonSpaceFound = FALSE;
	VisibleTextNode* pVTN = pStartChar;
	while (1)
	{
		VisibleTextNode* pPrevVTN = pVTN->FindPrevVTNInStory();

		// if at start of story, return ptr to first VTN
		if (pPrevVTN==NULL)
			return pVTN;

		// if EOL found, if we've moved, return ptr to char after EOL, else return EOL
		if (pPrevVTN->IsAnEOLNode())
		{
			if (pVTN!=pStartChar)
				return pVTN;
			else
				return pPrevVTN;
		}

		// if prev char not a space, flag this,
		// else prev char a space and if non-space found, return ptr to this char
		if (!pPrevVTN->IsAVisibleSpace())
			NonSpaceFound = TRUE;
		else if (NonSpaceFound)
			return pVTN;

		pVTN = pPrevVTN;
	}
}


/********************************************************************************************
>	VisibleTextNode* TextStory::GetNextWordChar(VisibleTextNode* pStartChar)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pStartChar - char to move from (usually the caret)
	Returns:	ptr to first char in next word, or EOL or char after EOL
********************************************************************************************/

VisibleTextNode* TextStory::GetNextWordChar(VisibleTextNode* pStartChar)
{
	ERROR2IF(pStartChar==NULL,NULL,"TextStory::GetNextWordChar() - pStartChar==NULL");
	VisibleTextNode* pVTN = pStartChar;

	// skip the caret so we always start on a char
	if (pStartChar->IsACaret())
	{
		pVTN = pVTN->FindNextVTNInStory();
		ERROR2IF(pVTN==NULL,NULL,"TextStory::GetNextWordChar() - no VTN after caret");
	}

	// skips chars until space found, then return pointer to first non-space
	// also return ptr to EOL and char immediately after EOL
	BOOL SpaceFound = FALSE;
	while (1)
	{
		VisibleTextNode* pNextVTN = pVTN->FindNextVTNInStory();

		// if this is an EOL return pointer to next char if not at end of story
		if (pVTN->IsAnEOLNode())
		{
			if (pNextVTN!=NULL)
				return pNextVTN;
			else
				return pVTN;
		}

		// if next char is an EOL return pointer to it
		ERROR2IF(pNextVTN==NULL,NULL,"TextStory::GetNextWordChar() - story deos not end in EOL");
		if (pNextVTN->IsAnEOLNode())
			return pNextVTN;

		// if char a space, flag it,
		// else non-space and if space found, return ptr to non-space
		if (pVTN->IsAVisibleSpace())
			SpaceFound = TRUE;
		else if (SpaceFound)
			return pVTN;

		pVTN = pNextVTN;
	}
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretLeftAChar()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96
	Returns:	FALSE if fails
	Purpose:	This routine moves the caret to the left by one position
********************************************************************************************/

BOOL TextStory::MoveCaretLeftAChar()
{
	CaretNode* pCaret = GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"TextStory::MoveCaretLeftAChar() - Story has no charet");

	VisibleTextNode* pNewCaretPos = pCaret->FindPrevVTNInStory();
	if (pNewCaretPos==NULL)
		return TRUE;	// do nothing if at start of story

	return MoveCaretToCharacter(pNewCaretPos, PREV);
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretRightAChar()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/8/96
	Returns:	FALSE if fails
	Purpose:	This routine moves the caret to the right by one position
********************************************************************************************/

BOOL TextStory::MoveCaretRightAChar()
{
	CaretNode* pCaret = GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"TextStory::MoveCaretRightAChar() - Story has no caret");

	VisibleTextNode* pNewCaretPos = pCaret->FindNextVTNInStory();
	ERROR2IF(pNewCaretPos==NULL,FALSE,"TextStory::MoveCaretRightAChar() - no VTN after caret in story!");
	pNewCaretPos = pNewCaretPos->FindNextVTNInStory();
	if (pNewCaretPos==NULL)
		return TRUE;	// if at end of story, do nothing
		
	return MoveCaretToCharacter(pNewCaretPos, PREV);
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretLeftAWord()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Returns:	TRUE/FALSE for success/failure
	Purpose:	This routine moves the caret to the left by a word.
				If the caret is in a word it is moved to the start of that word, else
				the caret is positioned at the start of the previous word
********************************************************************************************/

BOOL TextStory::MoveCaretLeftAWord()
{
	CaretNode* pCaret = GetCaret(); 	// Obtain the caret
	ERROR2IF(pCaret == NULL, FALSE, "Story had no caret");

	// Get the new caret position
	VisibleTextNode* pStartOfPrev = GetPrevWordChar(pCaret);
	ERROR2IF(pStartOfPrev == NULL, FALSE, "Didn't find prev word start");

	// Move the caret
	if (pStartOfPrev != pCaret)
		return MoveCaretToCharacter(pStartOfPrev, PREV);
	else
		return TRUE;
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretRightAWord()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Returns:	TRUE if the caret was moved a word to the right.
				FALSE if the caret is already positioned at the end of the TextLine
	Purpose:	This routine moves the caret to the right by a word.  It caret is positioned
				to the left of the first character encountered after a space.
********************************************************************************************/

BOOL TextStory::MoveCaretRightAWord()
{
	CaretNode* pCaret = GetCaret(); 	// Obtain the caret
	ERROR2IF(pCaret == NULL, FALSE, "Story had no caret");

	// Get the new caret position
	VisibleTextNode* pStartOfNext = GetNextWordChar(pCaret);
	ERROR2IF(pStartOfNext == NULL, FALSE, "Didn't find next word start");

	// Move the caret
	if (pStartOfNext != pCaret)
		return MoveCaretToCharacter(pStartOfNext, PREV);
	else
		return TRUE;
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretToStartOfLine()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/8/95
	Returns:	TRUE for success, FALSE if there was an error
	Purpose:	This routine moves the caret to the start of the TextLine that it is on
********************************************************************************************/

BOOL TextStory::MoveCaretToStartOfLine()
{
	CaretNode* pCaret = GetCaret(); 	// Obtain the caret
	ERROR2IF(pCaret == NULL, FALSE, "Story had no caret");

	// Get its parent (which should be a TextLine)
	TextLine* pParent = (TextLine*)pCaret->FindParent();
	ERROR2IF(!IS_A(pParent,TextLine), FALSE, "Parent of Caret was not a TextLine!");

	// Get the first child of the TextLine
	VisibleTextNode* pFirstChar = pParent->FindFirstVTN();

	// Only move the caret if we need to
	if (pFirstChar != pCaret)
		return MoveCaretToCharacter(pFirstChar, PREV);
	else
		return TRUE;
}


/********************************************************************************************
>	BOOL TextStory::MoveCaretToEndOfLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Returns:	FALSE if fails
	Purpose:	Move caret to end of TextLine which it is on
********************************************************************************************/

BOOL TextStory::MoveCaretToEndOfLine()
{
	CaretNode* pCaret = GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"TextStory::MoveCaretToEndOfLine() - story has no caret");
	TextLine* pCaretLine = (TextLine*)pCaret->FindParent();
	ERROR2IF(pCaretLine==NULL || !IS_A(pCaretLine,TextLine), FALSE, "Parent of Caret was not a TextLine!");

	VisibleTextNode* pLastChar = pCaretLine->FindLastVTN();
	if (pLastChar->IsAnEOLNode())
	{
		pLastChar = pLastChar->FindPrevVTNInLine();
		ERROR2IF(pLastChar==NULL,FALSE,"TextStory::MoveCaretToEndOfLine() - should be at least 1 other VTN on line - the caret!");
	}

	if (pLastChar!=pCaret)
		return MoveCaretToCharacter(pLastChar, NEXT);

	return TRUE;
}										


/********************************************************************************************
>	BOOL TextStory::AttachCaretAttributes()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/95
	Returns:	TRUE if the attributes were successfully applied to the caret
				FALSE if an error occured
	Purpose:	This function should be called after the caret has been moved to its new
				location.  It deletes all its previous children, and copies the children of
				the character to the left of it.  If there isn't anything to the left then
				the attributes to the right are used; if there isn't anything to the right
				then no attributes are attached so the defaults are used.
				This function broacasts an attribute changed message
	Errors:		Returns TRUE if it worked, FALSE if the copy failed
********************************************************************************************/

BOOL TextStory::AttachCaretAttributes()
{
	CaretNode* pCaret = GetCaret();
	BOOL Success = TRUE;
	BOOL TellPeople = FALSE;
	ERROR2IF(pCaret==NULL,FALSE,"Text story didn't have caret");

	// Identify where were are going to get the new attributes from
	VisibleTextNode* pAttributeSource = pCaret->FindPrevVTNInLine();
	if (pAttributeSource == NULL)
		pAttributeSource = pCaret->FindNextVTNInLine();

	// See if the attributes are the same as the current ones
/*	if (pAttributeSource != NULL)
	{
		Node* pCaretChild = pCaret->FindFirstChild();
		Node* pSourceChild = pAttributeSource->FindFirstChild();

		if ( ( (pCaretChild == NULL) && (pSourceChild != NULL) ) ||
		     ( (pCaretChild != NULL) && (pSourceChild == NULL) ) )
		{
			TellPeople = TRUE;
		}

		if ( (pCaretChild != NULL) && (pSourceChild != NULL) )
		{
			while ((pCaretChild != NULL) && (pSourceChild != NULL))
			{
				if ( (pCaretChild->IsAnAttribute()) && (pSourceChild->IsAnAttribute()) )
				{
					if (! ( *((NodeAttribute*)pCaretChild) == *((NodeAttribute*)pSourceChild) ))
						TellPeople = TRUE;
				}
				else
					TellPeople = TRUE;
				pCaretChild = pCaretChild->FindNext();
				pSourceChild = pSourceChild->FindNext();
			}
		}
	}
	else */
		TellPeople = TRUE;

	if (pAttributeSource != NULL)
	{
		// Delete the children of the caret
		pCaret->DeleteChildren(pCaret->FindFirstChild());

		// Attempt to copy the attributes of the previous character across
		Node* pToCopy = pAttributeSource->FindFirstChild(CC_RUNTIME_CLASS(NodeAttribute));
		while (Success && (pToCopy != NULL))
		{
			Success = pToCopy->CopyNode(pCaret, LASTCHILD);
			pToCopy = pToCopy->FindNext(CC_RUNTIME_CLASS(NodeAttribute));
		}
	}

	// Update other people
	if (TellPeople)
		GetApplication()->FindSelection()->Update();

	return Success;
}


/********************************************************************************************
>	virtual void TextStory::Transform(TransformBase& transform)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/95
	Inputs:		transform - some form of transform
	Purpose:	transform a text story and any children capable of being transformed
********************************************************************************************/

void TextStory::Transform(TransformBase& transform)
{
	BOOL ok=TRUE;
	if (IS_A(&transform, Trans2DMatrix))
	{
		// apply transform to story matrix
		StoryMatrix *= ((Trans2DMatrix&)transform).GetMatrix();

		// decompose new story matrix
		FIXED16  Scale=1;
		FIXED16  Aspect=1;
		ANGLE    Rotation=0;
		ANGLE    Shear=0;
		DocCoord Translation(0,0);
		ok=StoryMatrix.Decompose(&Scale,&Aspect,&Rotation,&Shear,&Translation);
//TRACEUSER( "Ed", _T("\nNew        Story Matrix - Scale=%8.5f, Aspect=%8.5f, Rotation=%9.4f, Shear=%8.5f, Translation=%d,%d\n"),Scale.MakeDouble(),Aspect.MakeDouble(),Rotation.MakeDouble()*180/PI,Shear.MakeDouble()*180/PI,Translation.x,Translation.y);
		if (Scale<0)
		{
			Scale=-Scale;
			Shear=-Shear;
		}

		// if the scale or aspect in the matrix deviate from unit by more than a specified amount normalise them
		const FIXED16 NormaliseLimit=0.001;
		FIXED16 AbsScaleError  = Scale-1;
		FIXED16 AbsAspectError = Aspect-1;
		AbsScaleError  =  AbsScaleError<0 ? -AbsScaleError  : AbsScaleError;
		AbsAspectError = AbsAspectError<0 ? -AbsAspectError : AbsAspectError;
		if (ok && (AbsScaleError>NormaliseLimit || AbsAspectError>NormaliseLimit) )
		{
			// determine if we can handle aspect ratio transforms in the attributes
			BOOL TransformAspect=(GetTextPath()==NULL);
			if (TransformAspect==FALSE)
				Aspect=1;	

			// any change in width applied to attributes must also be applied to the story width
			FIXED16 AbsAspect = Aspect<0 ? -Aspect : Aspect;
			if (ok)
				SetStoryWidth( XLONG(GetStoryWidth()) * Scale * AbsAspect );
			if (ok)
				SetLeftIndent( XLONG(GetLeftIndent()) * Scale * AbsAspect );
			if (ok)
				SetRightIndent( XLONG(GetRightIndent()) * Scale * AbsAspect );

			// create a set containing attributes which will be transformed
			AttrTypeSet AffectedAttrs;
			if (ok) ok=AffectedAttrs.AddToSet(CC_RUNTIME_CLASS(AttrTxtFontSize));
			if (ok) ok=AffectedAttrs.AddToSet(CC_RUNTIME_CLASS(AttrTxtLineSpace));
			if (ok) ok=AffectedAttrs.AddToSet(CC_RUNTIME_CLASS(AttrTxtBaseLine));
			if (ok && TransformAspect) ok=AffectedAttrs.AddToSet(CC_RUNTIME_CLASS(AttrTxtAspectRatio));

			// We add all attributes, including the defaults to the TextStory
			if (ok) ok=MakeAttributeComplete(NULL, TRUE, &AffectedAttrs, TRUE);
			// Now localise the stories attributes, globally, checking for duplicates
			if (ok) ok=LocaliseCommonAttributes(TRUE,TRUE,&AffectedAttrs);

			// Before we transform the TextStory we must add all non-common defaults to the
			// children of the TextLines.
			if (ok)
			{
				// Scan all lines, localising and factoring out, to add defaults
				TextLine* pLine = this->FindFirstLine();
				while (pLine!=NULL)
				{
					ok = pLine->LocaliseCommonAttributes(TRUE,FALSE,&AffectedAttrs);
					if (ok) ok = pLine->FactorOutCommonChildAttributes(FALSE,&AffectedAttrs);
					pLine = pLine->FindNextLine();
				}
			}
			// At this stage there should be no invalidly placed defaults in the tree
				
			// apply scale/aspect to attribtes
			if (ok)
			{
				// for the entire TextStory sub-tree, transform and of the text attributes
				Node* pNode=FindFirstDepthFirst();
				while (pNode!=NULL)
				{
					if (pNode->IsAnAttribute() && pNode->IsKindOfTextAttribute())
						((AttrTxtBase*)pNode)->BaseLineRelativeTransform(Scale,Aspect);
					pNode=pNode->FindNextDepthFirst(this);
				}

				// normalise the attributes which may have been affected
				if (ok) ok=FactorOutCommonChildAttributes(FALSE,&AffectedAttrs);

				// even if factor out failed, remove effect of attribute scale/(aspect) from story matrix
				Matrix AttrChanges, AttrSpace, StoryChanges;
				AttrChanges.Compose(Scale,Aspect);					// effect of scale/aspect on attrs in attr space
				AttrSpace.Compose(1,1,Rotation,Shear,Translation);	// story to attr space transfom
				StoryChanges=AttrSpace.Inverse();					// transform into attr space
				StoryChanges*=AttrChanges.Inverse();				// remove effect of attr scale/aspect
				StoryChanges*=AttrSpace;							// transform back into doc space
				StoryMatrix*=StoryChanges;							// effect of scale/aspect on attrs in story space
			}

			if (ok)
			{
				// Finally factor out the attributes globally
				ok = FactorOutCommonChildAttributes(TRUE,&AffectedAttrs);
			}

			AffectedAttrs.DeleteAll();
		}

		if (!ok)
		{
			InformError();
			return;
		}
	}
	else
		ERROR3("TextStory::Transform() - can't handle non-Trans2DMatrix transforms!");

#if 0
{
	FIXED16  Scale=1;
	FIXED16  Aspect=1;
	ANGLE    Rotation=0;
	ANGLE    Shear=0;
	DocCoord Translation(0,0);
	StoryMatrix.Decompose(&Scale,&Aspect,&Rotation,&Shear,&Translation);
	TRACEUSER( "Ed", _T("Normalised Story Matrix - Scale=%8.5f, Aspect=%8.5f, Rotation=%9.4f, Shear=%8.5f, Translation=%d,%d\n"),Scale.MakeDouble(),Aspect.MakeDouble(),Rotation.MakeDouble()*180/PI,Shear.MakeDouble()*180/PI,Translation.x,Translation.y);
}
#endif

	InvalidateBoundingRect();
	TransformChildren(transform);

	if (transform.bSolidDrag)
		FormatAndChildren();
}


/********************************************************************************************
>	VisibleTextNode* TextStory::GetSelectionEnd(BOOL* pDirection = NULL)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/03/95
	Inputs:		pDirection - Pointer to a BOOL that is filled in with the direction of the selection
	Outputs:	FALSE if the the selection end if to the left of the Anchor.
				TRUE if the the selection end if to the right of the Anchor.
	Returns:	Pointer to the last selected node in the text selection (at the other end from
				the caret)
	Purpose:	This function should be called to find what character nodes are selected.
				The return value is a pointer to the last selected node, at the other end of
				the selected characters from the anchor (which is the caret).
				NULL will be returned if there is no selection.
********************************************************************************************/

VisibleTextNode* TextStory::GetSelectionEnd(BOOL* pDirection)
{
	CaretNode* pCaret = GetCaret();
	ERROR2IF(pCaret==NULL, NULL, "Story has no caret");
	VisibleTextNode* pNextChar = NULL;

	// Get the direction of the selection
	BOOL Forward = TRUE;
	pNextChar = pCaret->FindPrevVTNInStory();
	if ((pNextChar != NULL) && pNextChar->IsSelected())
		Forward = FALSE;
	if (pDirection != NULL)
		*pDirection = Forward;

	// Loop to the end of the selection
	VisibleTextNode* pResult = NULL;
	if (Forward)
		pNextChar = pCaret->FindNextVTNInStory();
	else
		pNextChar = pCaret->FindPrevVTNInStory();
	while ((pNextChar != NULL) && pNextChar->IsSelected())
	{
		pResult = pNextChar;
		if (Forward)
			pNextChar = pNextChar->FindNextVTNInStory();
		else
			pNextChar = pNextChar->FindPrevVTNInStory();
	}

	if (pResult == pCaret)
		pResult = NULL;

	return pResult;
}


/********************************************************************************************
>	virtual BOOL TextStory::FormatAndChildren(UndoableOperation* pUndoOp=NULL,
											  BOOL UseNodeFlags=FALSE, BOOL WordWrap=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Inputs:		pUndoOp      - pointer to undoable op (NULL if not undoable)
				UseNodeFlags - TRUE  => use each node's ModifiedByOpflags to determine what has changed
							   FALSE => assume the whole story has changed
				WordWrap     - indicates if wordwrapping should be undertaken
							   (FALSE for undo/redo as history will restore previous wrap state
							   or stories on clipboard, where they should be unwrapped
							   or when non-undoably affecting the tree in the doc)
	Returns:	FALSE if fails
	Purpose:	Reformat a text story
********************************************************************************************/

BOOL TextStory::FormatAndChildren(UndoableOperation* pUndoOp, BOOL UseNodeFlags, BOOL WordWrap)
{
       PORTNOTE("text","TextStory::FormatAndChildren - do nothing");
#ifndef DISABLE_TEXT_RENDERING
	// if whole story is affected, just flag all children as 'ModifiedByOp'
	if (UseNodeFlags==FALSE)
		FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();

	// if DOing the format (as opposed to undo/redo)
	// ensure all lines in paragraphs have same line level attrs as first line
	// flagging any changed lines as modified, before metrics are recached
	if (WordWrap)
	{
		Validate(FALSE);
		TextLine* pLine = this->FindFirstLine();
		while (pLine!=NULL)
		{
			if (pLine->FindEOLNode()==NULL)
				if (!pLine->EnsureNextLineOfParagraphHasSameLineLevelAttrs(pUndoOp))
					return FALSE;
			pLine = pLine->FindNextLine();
		}
	}

	// recache metrics in any nodes in story 'ModifiedByOp'
	// Karim 02/03/2000 - braces added because this FormatRegion needs to go out of scope
	// *before* people start localising attributes in one of the fn's called later on.
	{
		FormatRegion FormattingRegion;
		if (FormattingRegion.Init(this)==FALSE)
			return FALSE;
		ReCacheNodeAndDescendantsMetrics(&FormattingRegion);
	}

	// if text on path, get untransfomed (reversed) copy and associated information
	TextStoryInfo StoryInfo;
	StoryInfo.pUndoOp  = pUndoOp;
	StoryInfo.WordWrap = WordWrap;
	if (GetTextPath()!=NULL)
		if (CreateUntransformedPath(&StoryInfo)==FALSE)
			return FALSE;

	// determine the story's width, and if it is word wrapping or not
	if (StoryInfo.pPath==NULL)
		StoryInfo.StoryWidth = GetStoryWidth();		// 0 if text at point
	else
		StoryInfo.StoryWidth = StoryInfo.PathLength;
	StoryInfo.WordWrapping = IsWordWrapping();

	// format each line in story
	TextLine* pLine = this->FindFirstLine();
	while (pLine!=NULL)
	{
		if (pLine->Format(&StoryInfo)==FALSE)
			return FALSE;
		pLine = pLine->FindNextLine();
	}

	// delete the untransformed copy of the path if there was one
	if (StoryInfo.pPath!=NULL)
		delete StoryInfo.pPath;

	// ensure the caret is in the correct place
	CaretNode* pCaret=GetCaret();
	ERROR2IF(pCaret==NULL,FALSE,"TextStory::Format() - pCaret==NULL");
	if (pCaret->HasMoved()==FALSE)
		return FALSE;

	// update the redraw rect to encompass all the affected nodes
	UnionNodeAndDescendantsOldAndNewBounds(&RedrawRect);

	// clear all the flags in the story
	ClearNodeAndDescendantsFlags();

	Validate();
#endif
	return TRUE;
}


/*******************************************************************************************
	void TextStory::PreExportRender(RenderRegion* pRegion)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	2/2/95
 	Inputs:		pRegion - points to the export render region
 	Purpose:	This function is called when the render function passes through this node
 				It outputs the	Text Object start and end tokens
********************************************************************************************/

void TextStory::PreExportRender(RenderRegion* pRegion)
{
#if EXPORT_TEXT
PORTNOTE("epsfilter", "Removed use of EPSFilter")
#ifndef EXCLUDE_FROM_XARALX
	BOOL exportingAsShapes = FALSE;

	// Determine whether or not to export AI text as shapes (i.e. if there are any 
	//	gradient fills in the text.
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(AIEPSRenderRegion)))
	{
		exportingAsShapes = IsGradientFilled ();
	}

	// if there are gradient fills, then don't export text info, just set the RenderRegion up
	//	so that it's ready for converting text to shapes.
	if (exportingAsShapes)
	{
		((AIEPSRenderRegion*) pRegion)->SetTextAsShapes (exportingAsShapes);
	}
	// if there are no gradient fills, or we're not doing AI export, then carry on as normal.
	else
	{
		if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
		{
			// Output "start text object" token
			EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
			EPSRenderRegion* pEPSRegion = (EPSRenderRegion*)pRegion;

			NodePath* pPath = GetTextPath();

			// Text Story type
			if (pPath)
				pDC->OutputValue((INT32)2);
			else
				pDC->OutputValue((INT32)0);

			// text object token
 			pDC->OutputToken("To");
 			pDC->OutputNewLine();

			// output the story matrix. We need to make this relative to the spread the
			// story is on.
			Matrix TempMatrix;
			TempMatrix = StoryMatrix;

			// if we're exporting to illustrator, make the text position relative to
			// the page.
			if (pRegion->IsKindOf(CC_RUNTIME_CLASS(AIEPSRenderRegion)))
			{
				DocCoord pos;
				TempMatrix.GetTranslation(pos);

				Spread* pCurSpread = FindParentSpread();
				if (pCurSpread)
				{
					DocCoord result;
					pCurSpread->SpreadCoordToPagesCoord(&result, pos);
					TempMatrix.SetTranslation(result);
				}
			}

			pDC->OutputMatrix(&TempMatrix);
			// null start point
			pDC->OutputValue((INT32)0);
			// start path
			pDC->OutputToken("Tp");
			pDC->OutputNewLine();

			// export the path
			if (pPath)
			{
				// if it's an AI EPS region, we need to save the text attributes after the TP token, or 
				//	Photoshop screws up.
				if (pRegion->IsKindOf (CC_RUNTIME_CLASS (AIEPSRenderRegion)) == FALSE)
				{
					// Need to call this for any text attributes that may have
					// changed
					pEPSRegion->GetValidTextAttributes();
				}

				// Save the attribute context
				pRegion->SaveContext();

				// Save the attributes associated with this path.
				Node* pNode = pPath->FindPrevious();
				while (pNode)
				{
					if (pNode->IsAnAttribute())
					{
						((NodeAttribute*)pNode)->Render(pEPSRegion);
					}
					pNode=pNode->FindPrevious();
				}

				pNode = pPath->FindFirstChild();
				while (pNode)
				{
					if (pNode->IsAnAttribute())
					{
						((NodeAttribute*)pNode)->Render(pEPSRegion);
					}
					pNode=pNode->FindNext();
				}

		/*	
				CMapPtrToPtr* pAttribMap = new CMapPtrToPtr(30);
				if (pAttribMap == NULL)
					return;

				// Now find any attributes that are applied to this node.
				BOOL FoundAttrs = pPath->FindAppliedAttributes(pAttribMap);

				if (FoundAttrs)
				{
					// iterating all (key, value) pairs
					for (POSITION Pos = pAttribMap->GetStartPosition(); Pos != NULL;)
					{
						void *pType,*pVal;
						pAttribMap->GetNextAssoc(Pos,pType,pVal);

						NodeAttribute* pAttr = (NodeAttribute*)pVal;
						pAttr->Render(pEPSRegion);
					}
				}
		*/
				if (IsTextOnPathReversed())
					(pPath->InkPath).Reverse();

				pEPSRegion->ExportPath(&(pPath->InkPath),FALSE);

				if (IsTextOnPathReversed())
					(pPath->InkPath).Reverse();
				
				// restore the old context.
				pRegion->RestoreContext();

			}

			// end path
			pDC->OutputToken("TP");
			pDC->OutputNewLine();

			if (pRegion->IsKindOf(CC_RUNTIME_CLASS(AIEPSRenderRegion)))
			{
				// Need to call this for any text attributes that may have
				// changed.
				//
				// (ChrisG - 8/11/00). Removed, as this should be done by the
				//	individual characters themselves, and can screw up the export
				//	if it isn't, (AI doesn't like lots of font and render changes
				//	without actually drawing any text).
		//			pEPSRegion->GetValidTextAttributes();

				// (ChrisG - 8/11/00) Added support for automatic kerning.
				if (AutoKern)
					pDC->OutputValue (1l);
				else
					pDC->OutputValue (0l);

				pDC->OutputToken ("TA");				   // automatic kerning TA
				pDC->OutputNewLine();
    			pDC->OutputToken("0 0 0 TC");			   // Character spacing TC
				pDC->OutputNewLine();
    			pDC->OutputToken("100 100 100 TW");		   // Word spacing TW
				pDC->OutputNewLine();
    			pDC->OutputToken("0 0 0 Ti");			   // Line indentation Ti
				pDC->OutputNewLine();

				// (ChrisG - 3/11/00)
				// Export the overflow text if the text is on a path
				if (pPath)
					((AIEPSRenderRegion *) pRegion)->OverflowTextStart ();
			}	
		}

		if (pRegion->IsKindOf(CC_RUNTIME_CLASS(NativeRenderRegion)))
		{
			// Output "extras" token for text
			EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
			EPSRenderRegion* pEPSRegion = (EPSRenderRegion*)pRegion;

			INT32 WordWrapping = (IsWordWrapping() ? 1 : 0);
			pDC->OutputValue(WordWrapping);
			pDC->OutputValue(GetStoryWidth());
			double CScale = CharsScale.MakeDouble();
			pDC->OutputReal(CScale);
			double CAspect = CharsAspect.MakeDouble();
			pDC->OutputReal(CAspect);
			double CRotation = CharsRotation.MakeDouble();
			pDC->OutputReal(CRotation);
			double CShear = CharsShear.MakeDouble();
			pDC->OutputReal(CShear);
			INT32 AsShapes = ((PrintAsShapes) ? (1) : (0));
			pDC->OutputValue(AsShapes);

			// Output version zero and token
			pDC->OutputValue((INT32)0);
			pDC->OutputToken("ctex");
			pDC->OutputNewLine();

			// Version one stuff
		/*		pDC->OutputValue(GetLeftMargin());
			pDC->OutputValue(GetRightMargin());

			// Output version and token
			pDC->OutputValue((INT32)1);
			pDC->OutputToken("ctex");
			pDC->OutputNewLine();*/
		}
	}
#endif
#endif
}


/*******************************************************************************************
>	BOOL TextStory::ExportRender(RenderRegion* pRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/4/95
	Inputs:		pRegion - points to the export render region
	Returns:	TRUE if rendered OK (FALSE=>use normal rendering)
	Purpose:	This function is called when the render function passes through this node
				It outputs the	Text Object start and end tokens
********************************************************************************************/

BOOL TextStory::ExportRender(RenderRegion* pRegion)
{
#if EXPORT_TEXT
PORTNOTE("epsfilter", "Removed use of EPSFilter")
#ifndef EXCLUDE_FROM_XARALX
	// (ChrisG 8/11/00) - Reset the 'exporting text as shapes' flag, so that it's valid for 
	//	the next text story - also don't export the 'end of text object' data if we aren't 
	//	exporting a text object.
	BOOL endTextObject = TRUE;

	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(AIEPSRenderRegion)))
	{
		if (((AIEPSRenderRegion*) pRegion)->GetTextAsShapes () == TRUE)
		{
			endTextObject = FALSE;
		}
		((AIEPSRenderRegion*) pRegion)->SetTextAsShapes (FALSE);
	}
	
	// don't export the 'end text object' if we shouldn't
 	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)) && (endTextObject))
	{
		// Output "End text object" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();
		pDC->OutputToken("TO");
		pDC->OutputNewLine();

		// output token if text is wrapped
		if (pRegion->IsKindOf(CC_RUNTIME_CLASS(NativeRenderRegion)))
		{
			if (WillStoryWrapOnPath())
			{
				pDC->OutputValue((INT32)TAG_TEXTWRAPPED);
				pDC->OutputToken("cso");
				pDC->OutputNewLine();
				pDC->OutputToken("ceo");
				pDC->OutputNewLine();
			}
		}
		return TRUE;
	}
#endif
#endif
	return FALSE;	
}


/********************************************************************************************

  >	BOOL TextStory::EnsureStoryHasCaret()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	28/8/96
  	Returns:	FALSE if fails
 	Purpose:	Makes sure the story has a caret node

********************************************************************************************/

BOOL TextStory::EnsureStoryHasCaret()
{
	TextLine* pFirstLine = FindFirstLine();
	TextLine* pLine = pFirstLine;

	if (pFirstLine != NULL)
	{
		// Scan all the lines looking for a caret node
		while (pLine != NULL)
		{
			// Does the line have a caret?
			if (pLine->FindCaret() != NULL)
				return TRUE;

			// Get next line
			pLine = pLine->FindNextLine();
		}

		// No caret in the text story, so lets stick one in the first line
		// just before the first visible text node
		VisibleTextNode* pVTN = pFirstLine->FindFirstVTN();
		if (pVTN != NULL)
		{
			CaretNode* pCaret = new CaretNode(pVTN,PREV);
			if (pCaret != NULL)
				return TRUE;
		}
	}

	return FALSE;
}



/********************************************************************************************
 >	BOOL TextStory::PostImport()

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	05/04/95
  	Returns:	FALSE if fails
 	Purpose:	This functions is called following the complete import of a document.  It
 				allows nodes to complete their post import initialisation once they are in
 				complete tree.
				The TextStory and its children are formatted following loading.
********************************************************************************************/

BOOL TextStory::PostImport()
{
#ifdef RALPH
	// We don't want to be rendering at the same time as doing this, because we get
	// blobby text problems - fun, but bad !
	RalphCriticalSection RalphCS;
#endif

	// Expand any strings that might have been imported from the v2 file format
	if (!ExpandImportedStrings())
		return FALSE;

	// Make sure it has a caret
	if (!EnsureStoryHasCaret())
		return FALSE;

	// if not newly inserted, ignore!
	if (!ModifiedByOp())
		return TRUE;

	// format to a width if one is specified
	if (GetImportFormatWidth()!=0)
	{
		SetWordWrapping(TRUE);
		SetStoryWidth(GetImportFormatWidth());
		ImportFormatWidth=0;				// prevent it happening on subsequent imports (as all stories are called!)
	}

	// account for any corel baseline shift style
	BOOL ok = TRUE;
	if (ok && ImportBaseShift!=AlignBaseline)
	{
		ok=CorelStyleBaselineShift(ImportBaseShift);
		ImportBaseShift=AlignBaseline;		// prevent it happening on subsequent imports (as all stories are called!)
	}

	// format and redraw
	if (ok) ok=FormatAndChildren();
	if (ok)
	{
		BaseDocument* pDoc = FindOwnerDoc();
		if (pDoc!=NULL && IS_A(pDoc, Document))
		{
			DocRect temprect = RedrawRect;
			ReleaseCached(TRUE, FALSE);
			BOOL bFoundEffects = FALSE;
			DocRect effectrect = GetEffectStackBounds(&bFoundEffects);
			if (bFoundEffects) temprect = temprect.Union(effectrect);
			((Document*)pDoc)->ForceRedraw(FindParentSpread(), RedrawRect, TRUE, this);
		}
	}

	return ok;
}



/********************************************************************************************
>	BOOL TextStory::PostDuplicate(UndoableOperation* pOp)

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	7/10/96
	Inputs:		pOp - pointer to operation
	Outputs:	-
  	Returns:	TRUE/FALSE for success/failure
 	Purpose:	This function is called following the (simple)copy of a text story.
				The TextStory and its children are re-formatted.
********************************************************************************************/
BOOL TextStory::PostDuplicate(UndoableOperation* pOp)
{
	ERROR2IF(pOp==NULL, FALSE, "Operation pointer was NULL");
	
	return PrePostTextAction::DoFormatStory(pOp, this);
}



/********************************************************************************************
>	BOOL TextStory::CorelStyleBaselineShift(BaseShiftEnum BaseShift);

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	5/7/95
	Inputs:		BaseShift -
  	Returns:	FALSE if fails
 	Purpose:	
********************************************************************************************/

BOOL TextStory::CorelStyleBaselineShift(BaseShiftEnum BaseShift)
{
	TextLine* pTextLine = this->FindFirstLine();
	ERROR2IF(pTextLine==NULL,FALSE,"TextStory::CorelStyleBaselineShift() - couldn't find text line in story");
	MILLIPOINT BaselineShift=0;
	MILLIPOINT LineAscent=(pTextLine->GetLineAscent()*4)/5;
	switch (BaseShift)
	{
		case AlignAscenders:  BaselineShift=-LineAscent; break;
		case AlignDescenders: BaselineShift=-pTextLine->GetLineDescent(); break;
		case AlignCentres:    BaselineShift=(-LineAscent-pTextLine->GetLineDescent())/2; break;
		default: break;
	}
	TxtBaseLineAttribute BaselineAttr(BaselineShift);
	BOOL ok = ((AttributeValue*)&BaselineAttr)->MakeNode(this,FIRSTCHILD) != NULL;
	
	return ok;
}


/********************************************************************************************
>	BOOL TextStory::ImportFix_MoveScaleToAttrs();

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	4/6/95
  	Returns:	FALSE if fails
 	Purpose:	Move the scale from the story's matrix down to its attributes
 				(ie fix up docs which were created with partly developed by code!)
********************************************************************************************/

BOOL TextStory::ImportFix_MoveScaleToAttrs()
{
	BOOL ok=TRUE;
	// remove scale from story matrix
	// Localise
	// for attrs on story/line/chars if txtattr and fontsize/linespace/baseline, scale
	// FactorOut
	return ok;
}


/*******************************************************************************************
>	BOOL TextStory::CreateUntransformedPath(TextStoryInfo* pStoryInfo)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	17/4/95
	Outputs:	pStoryInfo - info required about the path
	Returns:	FALSE if fails
 	Purpose:	Create a copy of the story's path removing the Story matrix transform
				Also, reverse path if text is reversed on the path
	Note:		This should really be broken up into member functions of Path
********************************************************************************************/

BOOL TextStory::CreateUntransformedPath(TextStoryInfo* pStoryInfo)
{
	ERROR2IF(pStoryInfo==NULL,FALSE,"TextStory::CreateUntransformedPath() - pStoryInfo==NULL");

	// get pointers to info in text story's NodePath
	NodePath* pNodePath = GetTextPath();
	ERROR2IF(pNodePath==NULL,FALSE,"TextStory::CreateUntransformedPath() - the TextStory has no path!!");
	INT32      NumCoords       = pNodePath->InkPath.GetNumCoords();
	DocCoord* pNodePathCoords = pNodePath->InkPath.GetCoordArray();
	PathVerb* pNodePathVerbs  = pNodePath->InkPath.GetVerbArray();
	BOOL ok = (NumCoords>=2 && pNodePathCoords!=NULL && pNodePathVerbs!=NULL);
	ERROR2IF(!ok,FALSE,"TextStory::CreateUntransformedPath() - problem with NodePath in story");

	// create a copy of the transformed path
	Path* pPath = new Path;
	ERROR2IF(pPath==NULL,FALSE,"TextStory::CreateUntransformedPath() - failed to crete path");
	if (ok) ok=pPath->Initialise(NumCoords, 12);
	if (ok) ok=pPath->CopyPathDataFrom(pNodePathCoords, pNodePathVerbs, NumCoords, TRUE);
	DocCoord* pPathCoords = NULL;
	PathVerb* pPathVerbs  = NULL;
	if (ok)
	{
		pPathCoords = pPath->GetCoordArray();
		pPathVerbs  = pPath->GetVerbArray();
		ok = (pPathCoords!=NULL && pPathVerbs!=NULL);
		if (!ok) ERROR2RAW("TextStory::CreateUntransformedPath() - Problem copying path");
	}

	// if text reversed on path, reverse path
	if (ok && IsTextOnPathReversed())
		pPath->Reverse();

	// remove the story transform from the copy of the path
	if (ok)
	{
		Matrix matrix = GetStoryMatrix();
		matrix = matrix.Inverse();
		matrix.transform((Coord*)pPathCoords, NumCoords);
	}

	// find the untransformed path length
	if (ok)
	{
		double fPathLength = 0;
		ProcessLength PathLengthProcess(64);
		ok = PathLengthProcess.PathLength(pPath, &fPathLength);
		pStoryInfo->PathLength = (MILLIPOINT)(fPathLength+0.5);
	}

	// determine if the path is closed
	if (ok)
		pStoryInfo->PathClosed = ( (pPathVerbs[NumCoords-1] & PT_CLOSEFIGURE) != 0);

	// determine direction of perpendicular to first point (downwards)
	if (ok)
	{
		// get the x,y offsets ratios for the line position at a perpendicular to the first point on the path ...
		double dx = pPathCoords[1].x-pPathCoords[0].x;
		double dy = pPathCoords[1].y-pPathCoords[0].y;
		double LineLength = sqrt(dx*dx+dy*dy);
		pStoryInfo->UnitDirectionVectorX = -dy/LineLength;
		pStoryInfo->UnitDirectionVectorY =  dx/LineLength;
	}

	pStoryInfo->LeftPathIndent  = GetLeftIndent();
	pStoryInfo->RightPathIndent = GetRightIndent();
	
	// if not ok and we created a path, delete it
	if (!ok && pPath!=NULL)
	{
		delete pPath;
		pPath=NULL;
	}
	
	// set output
	pStoryInfo->pPath = pPath;

	return ok;
}


/*******************************************************************************************
>	void TextStory::MatrixFitToPath()

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	17/4/95
 	Purpose:	modify TextStory matrix appropriately when fitting text to a path
********************************************************************************************/

void TextStory::MatrixFitToPath()
{
	DocCoord Translation(0,0);
	BOOL ok=GetpStoryMatrix()->Decompose(&CharsScale,&CharsAspect,&CharsRotation,&CharsShear,&Translation);
	ERROR2IF(!ok,void(0),"TextStory::MatrixFitToPath() - Matrix::Decompose() failed");

	// if text rotated to 2nd/3rd quadrant, reverse text on curve
	if (CharsRotation<(ANGLE)(-PI/2) || CharsRotation>(ANGLE)(PI/2))
	{
		ReverseTextOnPath();
		CharsRotation = (CharsRotation>0) ? CharsRotation-PI : CharsRotation+PI;
	}

	StoryMatrix=Matrix(Translation.x,Translation.y);		// keep a record of translation
}


/*******************************************************************************************
>	void TextStory::MatrixRemoveFromPath()

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	17/4/95
 	Purpose:	modify TextStory matrix appropriately when removing text from a path
********************************************************************************************/

void TextStory::MatrixRemoveFromPath()
{
	if (IsTextOnPathReversed())
	{
		CharsRotation = (CharsRotation>0) ? CharsRotation-PI : CharsRotation+PI;
		ReverseTextOnPath();
	}
	Matrix CharsMatrix;
	CharsMatrix.Compose(CharsScale,CharsAspect,CharsRotation,CharsShear);
	CharsMatrix*=StoryMatrix;
	SetStoryMatrix(CharsMatrix);
}


/********************************************************************************************
>	VisibleTextNode* TextStory::FindFirstVTN()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Returns:	A pointer to the first VisibleTextNode in the story.  NULL if error
********************************************************************************************/
VisibleTextNode* TextStory::FindFirstVTN() const
{
	TextLine* pFirstLine = this->FindFirstLine();
	ERROR2IF(pFirstLine==NULL,NULL,"TextStory::FindFirstVisibleTextNode() - No TextLine in the TextStory");
	return pFirstLine->FindFirstVTN();
}


/********************************************************************************************
>	VisibleTextNode* TextStory::FindLastVTN()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Returns:	A pointer to the last VisibleTextNode in the story.  NULL if error
********************************************************************************************/
VisibleTextNode* TextStory::FindLastVTN() const
{
	TextLine* pLastLine = this->FindLastLine();
	ERROR2IF(pLastLine==NULL,NULL,"TextStory::FindLastVTN() - story has no lines!");
	return pLastLine->FindLastVTN();
}


/********************************************************************************************
>	virtual BOOL TextStory::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/04/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the TextStory to respond to pop up menu clicks on itself.
********************************************************************************************/

BOOL TextStory::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL ok = TRUE;
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_TEXT, TRUE);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_APPLYLEFTJUSTIFY);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_APPLYCENTREJUSTIFY);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_APPLYRIGHTJUSTIFY);
//	ok = ok && pMenu->BuildCommand(OPTOKEN_APPLYFULLJUSTIFY, TRUE);
//	WEBSTER-ranbirr-13/11/96

	ok = ok && pMenu->BuildCommand(OPTOKEN_REVERSESTORYPATH);
#ifndef WEBSTER
	ok = ok && pMenu->BuildCommand(OPTOKEN_TOGGLEPRINTASSHAPES, TRUE);
#endif //webster
	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL TextStory::DeleteSelectedText(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/96
	Inputs:		pUndoOp -
	Returns:	FALSE if fails
	Purpose:	Delete all selected text objects in story, deleteing whole lines where possible
********************************************************************************************/

BOOL TextStory::DeleteSelectedText(UndoableOperation* pUndoOp)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pUndoOp==NULL,FALSE,"TextStory::DeleteSelectedText() - pUndoOp==NULL");
	BOOL    ok = DeleteSelectedTextLines(pUndoOp);
	if (ok)	ok = DeleteSelectedTextCharacters(pUndoOp);
	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL TextStory::DeleteSelectedTextLines(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Inputs:		pUndoOp -
	Returns:	FALSE if fails
	Purpose:	Called by DeleteSelectedText to delete all selected TextLines
				(including TextLines will all children selected)
********************************************************************************************/

BOOL TextStory::DeleteSelectedTextLines(UndoableOperation* pUndoOp)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pUndoOp==NULL,FALSE,"TextStory::DeleteSelectedTextLines() - pUndoOp==NULL");

	// Delete all selected lines except the last one
	BOOL AttrsLocalisedOnStory = FALSE;
	TextLine* pLine = this->FindFirstLine();
	ERROR2IF(pLine==NULL,FALSE,"TextStory::DeleteSelectedTextLines() - story has no lines");
	while (pLine!=NULL)
	{
		TextLine* pNextLine = pLine->FindNextLine();
		if (pLine->WholeLineSelected())
		{
			// If caret on line to be deleted, move it to next line
			CaretNode* pCaret = pLine->FindCaret();
			if (pCaret!=NULL)
			{
				BaseTextClass*      pAttachNode = NULL;
				AttachNodeDirection AttachDir   = PREV;
				TextLine* pPrevLine = pLine->FindPrevLine();
				if (pNextLine!=NULL)
				{
					pAttachNode = pNextLine->FindFirstVTN();
					ERROR2IF(pAttachNode==NULL,FALSE,"TextStory::DeleteSelectedTextLines() - text line has no VTN");
					AttachDir = PREV;
				}
				else if (pPrevLine!=NULL)
				{
					pAttachNode = pPrevLine->FindLastVTN();
					ERROR2IF(pAttachNode==NULL,FALSE,"TextStory::DeleteSelectedTextLines() - text line has no VTN");
					if (IS_A(pAttachNode,EOLNode))
						AttachDir = PREV;
					else
						AttachDir = NEXT;
				}
				else // deleting only line in story, so create a new one
				{
					pAttachNode = TextLine::CreateEmptyTextLine();
					if (pAttachNode==NULL)
						return FALSE;
					if (!pUndoOp->DoInsertNewNode(pAttachNode,pLine,NEXT, FALSE,FALSE,FALSE,FALSE))
						return FALSE;
					AttachDir = FIRSTCHILD;
				}

				if (!PositionCaretAction::DoStoreCaretPosition(pUndoOp,this))
					return FALSE;
				pCaret->MakeAttributeComplete();
				pCaret->MoveNode(pAttachNode,AttachDir);	// caret should not be selected
				pCaret->NormaliseAttributes();
			}

			// if not already localised, loaclise attrs on story, then hide line
			if (!AttrsLocalisedOnStory)
				if (!pUndoOp->DoLocaliseCommonAttributes(this,FALSE,TRUE))
					return FALSE;
			AttrsLocalisedOnStory = TRUE;
			if (!pLine->DoHideNode(pUndoOp))
				return FALSE;
		}
		pLine = pNextLine;
	}

	// if attrs were loaclised on story, factor out
	if (AttrsLocalisedOnStory)
		if (!pUndoOp->DoFactorOutCommonChildAttributes(this, TRUE))
			return FALSE;
#endif
	return TRUE;
}


/********************************************************************************************
>	BOOL TextStory::DeleteSelectedTextCharacters(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/96
	Inputs:		pUndoOp -
	Returns:	FALSE if fails
	Purpose:	delete all (remaining) selected VTN in story,
				joining lines if EOL deleted and story not word wrapping
********************************************************************************************/

BOOL TextStory::DeleteSelectedTextCharacters(UndoableOperation* pUndoOp)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(pUndoOp==NULL,FALSE,"TextStory::DeleteSelectedTextCharacters() - pUndoOp==NULL");

	// get a set of non line level attrs on this story, which are to be localised to char level
	AttrTypeSet NonLineLevelAttrs;
	BOOL ok = this->AddNonLineLevelDescendantAttrsToSet(&NonLineLevelAttrs);

	// delete all the selected chars (except caret) in the story, joining line if EOL deleted
	TextLine* pLine = this->FindFirstLine();
	ERROR2IF(pLine==NULL,FALSE,"TextStory::DeleteSelectedTextCharacters() - story has no lines");
	while (ok && pLine!=NULL)
	{
		BOOL AttrsLocalisedOnLine = FALSE;
		VisibleTextNode* pVTN = pLine->FindFirstVTN();
		ERROR2IF(pVTN==NULL,FALSE,"TextStory::DeleteSelectedTextCharacters() - line has no VTN");
		while (ok && pVTN!=NULL)
		{
			// ensure last EOL in story not selected (so not deleted)
			if (pVTN->IsAnEOLNode() && pVTN->IsSelected() && pLine->FindNextLine()==NULL)
			{
				ERROR3("TextStory::DeleteSelectedTextCharacters() - last EOL of the story selected");
				pVTN->DeSelect(FALSE);
			}

			// if char not to be deleted, skip to next char, else ...
			if (!pVTN->IsSelected() || pVTN->IsACaret())
				pVTN = pVTN->FindNextVTNInLine();
			else
			{
				// if attrs not already loaclised on this line, do so
				if (!AttrsLocalisedOnLine)
					ok = pUndoOp->DoLocaliseCommonAttributes(pLine,FALSE,TRUE,&NonLineLevelAttrs);
				AttrsLocalisedOnLine = TRUE;

				// if deleting EOL move all chars from next line to end of this and hide next line
				if (pVTN->IsAnEOLNode())
				{
					TextLine* pNextLine = pLine->FindNextLine();
					ERROR2IF(pNextLine==NULL,FALSE,"TextStory::DeleteSelectedTextCharacters() - trying to delete last EOL in story!");
					VisibleTextNode* pFirstVTN = pNextLine->FindFirstVTN();
					VisibleTextNode* pLastVTN  = pNextLine->FindLastVTN();
					ERROR2IF(pFirstVTN==NULL || pLastVTN==NULL,FALSE,"TextStory::DeleteSelectedTextCharacters() - no VTN on next line");

					if (ok) ok = pUndoOp->DoLocaliseCommonAttributes(pNextLine,FALSE,TRUE,&NonLineLevelAttrs);
					if (ok) ok = pFirstVTN->DoMoveNodes(pUndoOp,pLastVTN,pLine,LASTCHILD);
					if (ok) ok = pNextLine->DoHideNode(pUndoOp);
				}

				// hide VTN, getting pointer to next
				VisibleTextNode* pNextVTN = pVTN->FindNextVTNInLine();
				if (ok) ok = pVTN->DoHideNode(pUndoOp);
				pVTN = pNextVTN;
			}
		}
		// if ok and attrs were localised on the line, factor out
		if (ok && AttrsLocalisedOnLine)
			ok = pUndoOp->DoFactorOutCommonChildAttributes(pLine,TRUE,&NonLineLevelAttrs);

		pLine = pLine->FindNextLine();
	}

	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	INT32 TextStory::BaseComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		Stage		- either COPYOBJECT
									 COPYFINISHED
				RangeToCopy - Describes the range which is currently being copied.
				pOutput		- Depends on the Stage parameter
							  if Stage=
							  COPYOBJECT
							    Then the node pointer pOutput points at NULL. It should be
								set on exit to point at the copied object or tree of objects
							  COPYFINISHED
							  	Then the node pointer pOutput points at the resulting copy
								from COPYOBJECT as it has been inserted into the destination
								tree.
	Returns:	-1	= The routine failed to make a copy.
				 0	= No copy has been made.
				+1	= pOutput points to the copy.
    Purpose:
********************************************************************************************/

INT32 TextStory::BaseComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)
{
	ERROR2IF(pOutput==NULL,FALSE,"TextStory::RootComplexCopy() called with NULL output pointer");

	switch (Stage)
	{
		case COPYOBJECT:
			if (!BeingCopied)
			{
				BeingCopied = TRUE;
				if (Copy(RangeToCopy, pOutput))
					return 1;
				else
					return -1;
			}
			return 0;
			break;

		case COPYFINISHED:
			if (BeingCopied)
			{
				BeingCopied = FALSE;
				Node* pCopy = (*pOutput);
				if (pCopy && (IS_A(pCopy,TextStory)))
					((TextStory*)pCopy)->FormatAndChildren(NULL,FALSE,FALSE);
			}
			return 0;
			break;

		default:
			return -1;
			break;
	}
}


/********************************************************************************************
>	BOOL TextStory::Copy(Range& RangeToCopy, Node** pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		RangeToCopy - Describes the range which is currently being copied.
				pOutput		- a pointer to a node. Set this pointer to point at the tree copy
   	Returns:	FALSE if fails
    Purpose:    Make a copy of the text story and all children which lie inside RangeToCopy
********************************************************************************************/

BOOL TextStory::Copy(Range& RangeToCopy, Node** pOutput)
{
	ERROR2IF(pOutput==NULL,FALSE,"TextStory::Copy() called with NULL output pointer");

	/* Notes: ok, at this stage, each object in the copy range has been called with
	   MakeAttributesComplete() apart from those that state they are complex
	   copy nodes. Here I expect simply to make attributes complete on the
	   text story and assume the rest of the subtree is correct. Until I changed this
	   OpCopy used to MakeAttributesComplete on the whole selected range. This was a
	   problem, as we are actually copying more than the selected range. We are in
	   effect grouping the selected range under a parent. On pasting, the paste code
	   only (sensibly) normalises on all first level siblings and hence misses our
	   'grouped' children, the result being all selected characters having thousands of
	   atts applied to them which cannot be factored out.
	*/

	// copy the story
	TextStory* pStoryCopy = (TextStory*)SimpleCopy();
	BOOL ok = (pStoryCopy!=NULL);

	// Make the attrs complete so we can copy the story correctly
	if (ok) ok = MakeAttributeComplete();
	if (ok)
	{
		// Mark all nodes in this current range						// why is this not part of SetStoryMarkers?!
		Node* pNode = RangeToCopy.FindFirst(TRUE);
		while (pNode)
		{
			pNode->SetMarker(TRUE);
			pNode=RangeToCopy.FindNext(pNode,TRUE);
		}

		// specifically set the flags to copy the correct nodes
		SetStoryMarkers(RangeToCopy);
		ok = CopyChildrenTo(pStoryCopy, ccMARKED);

		// Now that EOLNodes take attributes we have all sorts of bother with the last EOL.
		// The user can't put attributes onto it.  We must copy
		// the attributes applied to the ATC prior to the last EOL onto the EOL.
		if (ok)
		{
			VisibleTextNode* pLastNode = pStoryCopy->FindLastVTN();
			AbstractTextChar* pPrevLast = NULL;
			if (pLastNode!=NULL)
				pPrevLast = (AbstractTextChar*)pLastNode->FindPrevious(CC_RUNTIME_CLASS(AbstractTextChar));
			if (pLastNode!=NULL && pPrevLast!=NULL)
			{
				// Clear out any attributes off the last EOLNode
				Node* pLastChild = pLastNode->FindFirstChild();
				if (pLastChild!=NULL)
					pLastNode->DeleteChildren(pLastChild);
			
				// Copy the correct ones onto it.
				if (pPrevLast->FindFirstChild() != NULL)
					ok = pPrevLast->CopyChildrenTo(pLastNode);
			}
		}

		// Mark all nodes in this current range						// is this not made redundant by ClearMarks()?!
		pNode = RangeToCopy.FindFirst(TRUE);
		while (pNode)
		{
			pNode->SetMarker(FALSE);
			pNode=RangeToCopy.FindNext(pNode,TRUE);
		}

		ClearMarks();				// clear flags on source story
		pStoryCopy->ClearMarks();	// clear flags on destination story
		ResetEOLs(FALSE);			// make EOLs physical again (ie not virtual!)	// why does this take a param?!
		NormaliseAttributes();		// on source story								// why not dest?
	}

	// unwrap the story on the clipboard
	if (ok) ok = pStoryCopy->UnWrapStory(NULL);

	// insert a caret into the copied story
	CaretNode*       pCaret      = NULL;
	VisibleTextNode* pAttachNode = NULL;
	if (ok) pAttachNode = pStoryCopy->FindFirstVTN();
	ok = (pAttachNode!=NULL);
	if (ok) pCaret = new CaretNode;
	ok = (pCaret!=NULL);	// should an error be set if caret not created?
	if (ok) pCaret->AttachNode(pAttachNode, PREV);

	// if we failed but a story copy was created, delete it
	if (!ok && pStoryCopy!=NULL)
	{
		pStoryCopy->CascadeDelete();
		delete pStoryCopy;
		pStoryCopy = NULL;
	}

	*pOutput = pStoryCopy;
	return ok;
}


/********************************************************************************************
>	BOOL TextStory::UnWrapStory(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/96
	Inputs:		pUndoOp - undoable operation (NULL if not undo-able)
    Purpose:    Join consecutive soft lines into single hard lines
				and append a final (virtual) EOL to the story if none
********************************************************************************************/

BOOL TextStory::UnWrapStory(UndoableOperation* pUndoOp)
{
	TextLine* pThisLine = FindFirstLine();
	ERROR2IF(pThisLine==NULL,FALSE,"TextStory::UnWrapStory() - story has no text lines!");
	while (pThisLine!=NULL)
	{
		TextLine* pNextLine = pThisLine->FindNextLine();
		if (pThisLine->FindEOLNode()==NULL)
		{
			if (pNextLine!=NULL)
			{
				// wrap next line back to end of this
				VisibleTextNode* pLastVTN = pNextLine->FindLastVTN();
				ERROR2IF(pLastVTN==NULL,FALSE,"TextStory::UnWrapStory() - no VTN on soft line");
				pLastVTN->WrapFromStartOfLineBack(pUndoOp);
			}
			else
			{
				// add a virtual EOL to keep Mike's stuff from falling over
				EOLNode* pEOL = new EOLNode;
				if (pEOL==NULL)
					return FALSE;
				pEOL->SetVirtual(TRUE);
				pEOL->AttachNode(pThisLine,LASTCHILD);
			}
		}
		else
			pThisLine = pNextLine;	// move onto start of next paragraph
	}

	// ensure op flags not set in story on clipboard
	ClearNodeAndDescendantsFlags();

	Validate();

	return TRUE;
}


/********************************************************************************************
>	void TextStory::SetStoryMarkers(Range& RangeToCopy)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		RangeToCopy - Describes the range which is currently being copied.
    Purpose:    Scan through the text story setting the necessary copyme bits to make
    			sure the text story is copied correctly. The range is used to determin
				what items are actually being copied. If characters are being copied,
				we need to copy a line and text story as well.
********************************************************************************************/

void TextStory::SetStoryMarkers(Range& RangeToCopy)
{
	// find the first object in the story
	Node* pNode = FindFirstChild();
	while (pNode)
	{
		if (pNode->IsAnAttribute())
		{
			pNode->MarkNodeAndChildren();
		}
		else
		{		
			// ok check if its a line and if so do all the character level stuff
			if (IS_A(pNode,TextLine))
			{
				// find the first child of this particular line
				Node* cNode = NULL;
				Node* qNode = pNode->FindFirstChild();
				while (qNode)
				{
					// Mark in range characters, kern codes and other abstract characters
					// Only copy the char if it is in the search range.
					if (qNode->IsMarked())
					{
						qNode->MarkNodeAndChildren();
						// Save a copy of this chap for later
						cNode=qNode;
					}
					qNode=qNode->FindNext();
				}

				// we've found an item inrange in this line.....
				if (cNode)
				{
					// Make sure we mark the line too
					qNode = cNode->FindParent();
					qNode->SetMarker(TRUE);

					// take all the line level attributes with us too
					qNode = qNode->FindFirstChild();
					while (qNode)
					{
						if (qNode->IsAnAttribute())
						 	qNode->MarkNodeAndChildren();
						qNode=qNode->FindNext();
					}

					// Now we need to make sure we take the end of line node with us.
					qNode=cNode->FindParent();
					qNode=qNode->FindFirstChild(CC_RUNTIME_CLASS(EOLNode));

					ERROR3IF((qNode) && (!IS_A(qNode,EOLNode)),"Where has the lines EOLnode gone? - (TextStory::SetStoryMarkers())");

					// We assume the last node is an EOLnode.
					if (qNode && IS_A(qNode,EOLNode))
					{
						if (!qNode->IsMarked())
						{
							// It wasn't already marked ie it can't have been 'inrange'
							qNode->MarkNodeAndChildren();
							// We need to mark the eolnode as transient
							// ie so we know the difference between unselected
							// and selected nodes.
							((EOLNode*)qNode)->SetVirtual(TRUE);
						}
					}
				}
			}
			else
			{
				// otherwise its not a line and its not an attribute so is it in the range?
				// if its in range, take it and its children with us
				if (pNode->IsMarked())
					pNode->MarkNodeAndChildren();
			}
		}
	
		// Scan for the next object
		pNode=pNode->FindNext();
	}

	/* I suppose we need to take the caret too,
	   erh no we need to wait and insert a caret later, it could be in a different line
	   to the selection if select inside has occured! and hence wont be copied properly

	   CaretNode* pCaret = GetCaret();
	   if (pCaret)
	       pCaret->MarkNodeAndChildren(); */

}


/********************************************************************************************
>	void TextStory::ResetEOLs(BOOL Status)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/95
	Inputs:		Status - the status to set for each EOL node.
    Purpose:	Scan through all end of line nodes in this text story and set the status
    			of their virtual flag
********************************************************************************************/

void TextStory::ResetEOLs(BOOL Status)
{
	// find the first line in this story
	TextLine* pLine = this->FindFirstLine();
 	while (pLine)
	{
		// for each line, set its EOL nodes status
		EOLNode* pEOL = pLine->FindEOLNode();
		if (pEOL!=NULL)
			pEOL->SetVirtual(Status);
		
		pLine=pLine->FindNextLine();
	}
}


/********************************************************************************************
>	INT32 TextStory::BaseComplexHide(UndoableOperation *pOp)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		pOp	- a pointer to an undoable operation
	Returns:	-1	= The routine failed to hide this node.
				 0	= Ignored, this object does not support complex hide operations
				+1	= The node and possibly various others have been hidden correctly.
    Purpose: This function handles the text story being hidden during operations such as
			 OpDelete / OpCut / etc. The function is called indirectly via it child nodes.
			 It should never be called directly. Its purpose is to cope with various parts
			 of the text story being hidden. It is not possible to DoHideNodes() on a
			 selection of the text stories children as this may leave the story in an invalid
			 state. Hence all children ask their parent text story to sort out the problem.
			 The function is usually called from a derived version of Node::ComplexHide()
********************************************************************************************/

INT32 TextStory::BaseComplexHide(UndoableOperation *pOp)
{
	ERROR2IF(pOp==NULL, -1, "Operation pointer is NULL in TextStory::BaseComplexHide()");

	if (!DeleteSelectedText(pOp))
		return -1;

#if !defined(EXCLUDE_FROM_RALPH)
	// try to regain the caret on completion
	TextInfoBarOp::RegainCaretOnOpEnd();
#endif

	return 1;
}


/********************************************************************************************
>	static TextStory* TextStory::CreateFromChars(DocCoord Pos, char* pChars, WCHAR* pWChars, Document* pCreateDoc,
												 LOGFONT* pLogFont=NULL, BOOL ControlCodes=FALSE
												 DocColour* pColour=NULL)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/95
	Inputs:		Pos          - coord in doc at which to position TextStory
				pChars       - pointer to array of chars  ('\0' terminated)
				pWChars      - pointer to array of WCHARs ('\0' terminated)
				pCreateDoc	 - pointer to document story is to be created in
				pLogFont     - optional pointer to a LogFont from which to derive attributes
				ControlCodes - TRUE if control codes should also be processed
				pColour      - text colour
	Returns:	Pointer to TextStory, or NULL if fails
	Purpose:	Create a TextStory from an array of chars OR WCHARs
				and optionally account for control codes
				and a LogFont (ie size/aspect/bold/italic/typeface/rotation)

	Notes:		Calls ContinueSlowJob periodically to allow progress displays to be updated

********************************************************************************************/

TextStory* TextStory::CreateFromChars(DocCoord Pos, char* pChars, WCHAR* pWChars, Document* pCreateDoc,
									  LOGFONT* pLogFont, BOOL ControlCodes, DocColour* pColour)
{
	ERROR2IF(pChars==NULL && pWChars==NULL || pChars!=NULL && pWChars!=NULL, NULL,
			 "TextStory::CreateFromCharArray() - must specify one and only one char array");
	ERROR2IF(pCreateDoc==NULL, NULL, "No creation document");

	// create an empty text story, getting pointers to TextLine and Caret
	TextStory* pTextStory=TextStory::CreateTextObject(Pos);
	TextLine*  pTextLine =NULL;
	CaretNode* pCaretNode=NULL;
	BOOL ok=(pTextStory!=NULL);
	if (ok)
	{
		pTextLine = pTextStory->FindFirstLine();
		if (pTextLine!=NULL)
			pCaretNode=pTextLine->FindCaret();
		if (pCaretNode==NULL)
		{
			ok=FALSE;
			ERROR2RAW("TextStory::CreateFromChars() - failed to find TextLine or Caret");
		}
	}

	// Put the current attributes onto the story
	if (ok)
		ok = pCreateDoc->GetAttributeMgr().ApplyCurrentAttribsToNode(pTextStory);

	// Before forcing on attributes we need to delete the current instances first
	if (ok)
		ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrFillGeometry));
//	if (ok)
//		ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrStrokeTransp));
	if (ok)
		ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrLineWidth));
	if (ok)
		ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrStrokeColour));
	
	// add black fill colour, no/transparent line colour, 0.25pt line width attributes
	if (ok)
	{
		FlatFillAttribute     FlatFillAttr(pColour!=NULL ? *pColour : AttributeManager::DefaultBlack);
		StrokeTranspAttribute StrokeTranspAttr(255);
		LineWidthAttribute    LineWidthAttr(250);
		if (ok) ok = ((AttributeValue*)    &FlatFillAttr)->MakeNode(pTextLine,PREV) != NULL;
//		if (ok) ok = ((AttributeValue*)&StrokeTranspAttr)->MakeNode(pTextLine,PREV) != NULL;
		if (ok) ok = ((AttributeValue*)   &LineWidthAttr)->MakeNode(pTextLine,PREV) != NULL;
		// BODGE TEXT - this is the only way this attribute will work!!!!
		AttributeValue* pAttr=NULL;
		DocColour trans(COLOUR_TRANS);
		if (ok) ok = NULL!=(pAttr=new StrokeColourAttribute(trans));
		if (ok) ok = NULL!=pAttr->MakeNode(pTextLine,PREV);
		if (pAttr!=NULL) delete pAttr;
	}

	// if we have a LogFont, set story rotation, add typeface, size, aspect, bold, italic attributes
	if (ok && pLogFont!=NULL)
	{
		// Delete the current instances of the attribute first
		if (ok)
			ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrTxtFontTypeface));
		if (ok)
			ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrTxtFontSize));
		if (ok)
			ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrTxtAspectRatio));
		if (ok)
			ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrTxtBold));
		if (ok)
			ok = DeleteChildAttribute(pTextStory, CC_RUNTIME_CLASS(AttrTxtItalic));

		if (ok)
		{
			FontInfo info;
			ok=TextManager::GetInfoFromLogFont(&info,pLogFont);
			if (ok)
			{
				Matrix StoryMatrix(info.Rotation);
				StoryMatrix.translate(Pos.x,Pos.y);
				pTextStory->SetStoryMatrix(StoryMatrix);

				TxtFontTypefaceAttribute FontAttr(info.Handle);
				TxtFontSizeAttribute     SizeAttr(info.Size);
				TxtAspectRatioAttribute  AspectAttr(info.Aspect);
				TxtBoldAttribute         BoldAttr(info.Bold);
				TxtItalicAttribute       ItalicAttr(info.Italic);
				if (ok) ok = ((AttributeValue*)  &FontAttr)->MakeNode(pTextLine,PREV) != NULL;
	 			if (ok) ok = ((AttributeValue*)  &SizeAttr)->MakeNode(pTextLine,PREV) != NULL;
	 			if (ok) ok = ((AttributeValue*)&AspectAttr)->MakeNode(pTextLine,PREV) != NULL;
	 			if (ok) ok = ((AttributeValue*)  &BoldAttr)->MakeNode(pTextLine,PREV) != NULL;
	 			if (ok) ok = ((AttributeValue*)&ItalicAttr)->MakeNode(pTextLine,PREV) != NULL;
			}
		}
	}

	// insert each char before the caret
	INT32  index=0;
	WCHAR LastChar=0;
	while (ok)
	{
		// get a char from the appropriate array, increase index, exitting if none left
		WCHAR ch = 0;
		if (pChars==NULL)
			ch = pWChars[index++] ;
		else
		{
			// Read the next character from the array
			if (UnicodeManager::IsDBCSOS() && UnicodeManager::IsDBCSLeadByte(pChars[index]))
			{
				ch = UnicodeManager::ComposeMultiBytes(pChars[index], pChars[index+1]);
				index += 2;
			}
			else
			{
				ch = (unsigned char)(pChars[index]);
				index += 1;
			}

			// Convert into Unicode if it's not a control char
			if ((pChars != NULL) && (ch>=32 && ch!=127))
				ch = UnicodeManager::MultiByteToUnicode(ch);
		}

		if (ch=='\0')
			break;

		// if not a control code, just insert the char to the left of the caret
		// else, if handling control codes, do what is required
		if (ch>=32 && ch!=127)
		{
			TextChar* pTextChar = new TextChar(pCaretNode, PREV, ch);
			ok=(pTextChar!=NULL);
		}
		else if (ControlCodes)
		{
			switch (ch)
			{
				case '\n':
				{
					// Give the progress system a chance to update its displays
					ContinueSlowJob();

					// if part of '\r\n' pair ignore it as already insereted new line,
					// else continue into '\r' case and insert a new line
					if (LastChar=='\r')
						break;
				}
				case '\r':	// insert a new line
				{
					pTextLine = TextLine::CreateEmptyTextLine(pTextLine, NEXT);
					ok=(pTextLine!=NULL);
					if (ok) pCaretNode->MoveNode(pTextLine,FIRSTCHILD);
					break;
				}
				case '\t':	// insert a kern to mimic a tab
				{
					DocCoord tabkern(4000,0); // Nothing like a hardcoded bodge
					KernCode* pKernCode= new KernCode(pCaretNode, PREV, tabkern);
					ok=(pKernCode!=NULL);
					break;
				}
				default:
					TRACE( _T("Control Code %d, not yet supported\n"),ch);
			}
		}
		LastChar=ch;
	}

	if (ok)
	{
		EOLNode* pLastEOL=(EOLNode*)(pCaretNode->FindNext());
		ok=(pLastEOL!=NULL);
		if (ok)
			pLastEOL->SetVirtual(TRUE);
		else
			ERROR2RAW("TextStory::CreateFromChars() - couldn't EOL after caret!");
	}

	// if we failed to finish the job, tidy up
	if (!ok)
	{
		pTextStory->CascadeDelete();
		delete pTextStory;
		pTextStory=NULL;
	}

	return pTextStory;
}


/********************************************************************************************
>	BOOL TextStory::WillStoryWrapOnPath()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/95
	Inputs:		-
	Returns:	TRUE if the text story will wrap on this path
				FALSE if not
	Purpose:	Calculates whether the formatter will wrap any line of text around the path
				It calculates this by checking the position of the last character (or VTN in
				each line). If this wraps it is assumed the whole line will wrap.
				Note text is assumed to only ever wrap on a path which is closed.
********************************************************************************************/

BOOL TextStory::WillStoryWrapOnPath()
{
// BODGE TEXT - horrible copy of code elsewhere

	// first find the text path.
	NodePath* pNodePath = GetTextPath();
	if (pNodePath==NULL)
		return FALSE;
	Path* pPath = &(pNodePath->InkPath);

	// if the path is open, text never wraps.
	INT32 NumPathCoords=pPath->GetNumCoords();
	if (NumPathCoords<2)
		return FALSE;

	BOOL PathClosed=FALSE;
	PathVerb* pPathVerbs=pPath->GetVerbArray();
	ERROR2IF(pPathVerbs==NULL,FALSE,"TextStory::IsStoryWrappedOnpath() - pPathVerbs==NULL");
	if (pPathVerbs[NumPathCoords-1] & PT_CLOSEFIGURE)
		PathClosed=TRUE;

	if (!PathClosed)
		return FALSE;
	
	// find the untransformed path length
	MILLIPOINT PathLength = 0;
	double fPathLength=0;
	ProcessLength PathLengthProcess(64);
	if (PathLengthProcess.PathLength(pPath,&fPathLength))
		PathLength=(MILLIPOINT)fPathLength;

	FIXED16 scale  = GetCharsScale();
	FIXED16 xscale = (scale<0 ? -scale : scale) * GetCharsAspect();

	BOOL Wrapped = FALSE;
	TextLine* pLine = FindFirstLine();
	while (pLine && !Wrapped)
	{
		Wrapped = pLine->WillLineWrapOnPath(xscale,PathLength);
		pLine = pLine->FindNextLine();
	}

	return Wrapped;
}


/********************************************************************************************
>	static DocCoord TextStory::GetPathBlobPos(MILLIPOINT IndentLength, NodePath* pPath)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/95
	Inputs:		IndentLength - the indent length
				pPath - pointer to the path
	Returns:	The position of a point along the path
	Purpose:	Gets the location of the of a point a certian distance along the path.
				This function is static so others, eg the drag op, can call it and it's
				scary math
********************************************************************************************/

DocCoord TextStory::GetPathBlobPos(MILLIPOINT IndentLength, NodePath* pPath)
{
	DocCoord Result(0,0);
	pPath->InkPath.GetPointAtDistance(IndentLength, &Result);
	return Result;
}


/********************************************************************************************
>	BOOL TextStory::GetCharacterAtPoint(BOOL InBounds, DocCoord Point, VisibleTextNode** pHitChar)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/1/96
	Inputs:		InBounds - if TRUE then Point must be within the bounds of the story to find
							a character
				Point - the location in question
	Outputs:	pHitChar - the character the point is near, NULL if there isn't a character
				ToLeft - true if the point is to the left of the character, FALSE if to the right
	Returns:	TRUE for success, FALSE if an error occured
	Purpose:	Given a location works out the nearest character to that location.
********************************************************************************************/

BOOL TextStory::GetCharacterAtPoint(BOOL InBounds, DocCoord Point, VisibleTextNode** pHitChar, BOOL* ToLeft)
{
	ERROR3IF(GetTextPath()!=NULL, "GetCharacterAtPoint : Results are unpredictable for stories on a path");
	
	// Get inital pointers, checking them
	ERROR2IF(pHitChar==NULL || ToLeft==NULL, FALSE, "NULL output param");
	CaretNode* pCaret = GetCaret();
	ERROR2IF(pCaret == NULL, FALSE, "Story has no caret");
	TextLine* pFirstLine = FindFirstLine();
	TextLine* pLastLine = FindLastLine();
	ERROR2IF((pFirstLine==NULL || pLastLine==NULL), FALSE, "Story has no text lines");

	// Set outputs to defaults
	*pHitChar = NULL;
	*ToLeft = FALSE;

	// Untransform the point via the story matrix
	DocCoord UTPoint = Point;
	Matrix StoryMat = GetStoryMatrix();
	StoryMat=StoryMat.Inverse();
	StoryMat.transform(&UTPoint);

	// Check to see wether the point is within the untransformed bounds of the story
	if (InBounds)
	{
		DocRect UTStoryBounds = GetUTStoryBounds();
		if (!UTStoryBounds.ContainsCoord(UTPoint))
		{
			*pHitChar = NULL;
			*ToLeft = FALSE;
			return TRUE;
		}
	}

	// See if the point is above the first line
	INT32 HighestPointFound = INT32_MIN;
	INT32 LowestPointFound = INT32_MAX;
	TextLine* HighestLine = NULL;
	TextLine* LowestLine = NULL;
	
	// Scan through the lines to see if one contains the point
	INT32 PrevHigh = pFirstLine->GetPosInStory() + pFirstLine->GetLineAscent();
	INT32 PrevLow = pFirstLine->GetPosInStory() + pFirstLine->GetLineDescent();
	TextLine* pLine = pFirstLine;
	while (pLine != NULL)
	{
		INT32 HighestPoint = pLine->GetPosInStory() + pLine->GetLineAscent();
		INT32 LowestPoint = pLine->GetPosInStory() + pLine->GetLineDescent();

		// Update the highest and lowest known points in the story
		if (HighestPointFound < HighestPoint)
		{
			HighestPointFound = HighestPoint;
			HighestLine = pLine;
		}
		if (LowestPointFound > LowestPoint)
		{
			LowestPointFound = LowestPoint;
			LowestLine = pLine;
		}

		// Get the bounds of the current and previous lines
		INT32 Highest = max( PrevHigh, max( PrevLow, max( HighestPoint, LowestPoint ) ) );
		INT32 Lowest  = min( PrevHigh, min( PrevLow, min( HighestPoint, LowestPoint ) ) );

		if ((UTPoint.y <= Highest) && (UTPoint.y >= Lowest))
			break;

		PrevHigh = HighestPoint;
		PrevLow = LowestPoint;
		pLine = pLine->FindNextLine();
	}

	// If pLine is NULL then the point is above/below the story
	if (pLine == NULL)
	{
		if (UTPoint.y >= HighestPointFound)
			pLine = HighestLine;
		if (UTPoint.y <= LowestPointFound)
			pLine = LowestLine;
	}
	ERROR2IF(pLine == NULL, FALSE, "Didn't find a line");

	// Find a character on the line
	BOOL ToLeftPosChar = TRUE;
	VisibleTextNode* pPointerChar = pLine->FindCharAtDistAlongLine(UTPoint.x, &ToLeftPosChar);
	if (pPointerChar->IsAnEOLNode())
		ToLeftPosChar = FALSE;
	if (pPointerChar->IsACaret())
	{
		if (pPointerChar->FindNextVTNInLine()!=NULL)
			pPointerChar = pPointerChar->FindNextVTNInLine();
		else
			pPointerChar = pPointerChar->FindPrevVTNInLine();
	}

   *pHitChar = pPointerChar;
   *ToLeft = ToLeftPosChar;

   return TRUE;
}


/********************************************************************************************
>	DocCoord TextStory::GetLeftIndentPos()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/96
	Returns:	The position of this stories path left indent
	Purpose:	Gets the position in the document of the left indent of this story.
********************************************************************************************/
DocCoord TextStory::GetLeftIndentPos() const
{
	ERROR3IF(GetTextPath() == NULL, "Story not on a path, LeftIndent unused");

	if (GetTextPath() != NULL)
	{
		if (IsTextOnPathReversed())
		{
			INT32 TotalPathLength = (INT32)GetTextPath()->InkPath.GetPathLength();
			return GetPathBlobPos(TotalPathLength-GetLeftIndent(), GetTextPath());
		}
		else
			return GetPathBlobPos(GetLeftIndent(), GetTextPath());
	}


	else
		return DocCoord(0,0);
}



/********************************************************************************************
>	DocCoord TextStory::GetRightIndentPos()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/8/96
	Returns:	The position of this stories right indent
	Purpose:	Gets the position in the document of the right indent of this story.
				Remember that the right inrent is relative to the RHS of the path
********************************************************************************************/
DocCoord TextStory::GetRightIndentPos() const
{
	ERROR3IF(GetTextPath() == NULL, "Story not on a path, RightIndent unused");

	if (GetTextPath() != NULL)
	{
		if (IsTextOnPathReversed())
		{
			return GetPathBlobPos(GetRightIndent(), GetTextPath());
		}
		else
		{
			INT32 TotalPathLength = (INT32)GetTextPath()->InkPath.GetPathLength();
			return GetPathBlobPos(TotalPathLength-GetRightIndent(), GetTextPath());
		}
	}
	else
		return DocCoord(0,0);
}



/********************************************************************************************

>	virtual BOOL TextStory::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
	virtual BOOL TextStory::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter = ptr to the filter to write out to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes out a records associated with this node

	We currently call a helper class to do the work for us. Its been written this way
	to (a) keep all the text exporting code in one place (cxftext.h, cxftext.cpp),
	(b) to stop this file and its header from becoming bloated (reducing dependencies
	and build times) and (c) because I feel its neater (ie I want to).			Ach

********************************************************************************************/

BOOL TextStory::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextStory::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextStory::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::CanWriteChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextStory::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::CanWriteChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL TextStory::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter);
	BOOL TextStory::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter);
	BOOL TextStory::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter);
	BOOL TextStory::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/08/96
	Inputs:		pFilter = ptr to the filter to write out to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes out child records associated with the text story

	It uses the CXaraFileTxtStory class, because this is where all the rest of the code for
	text story output is.

********************************************************************************************/

BOOL TextStory::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::WriteBeginChildRecordsWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextStory::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::WriteBeginChildRecordsNative(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextStory::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::WriteEndChildRecordsWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextStory::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtStory::WriteEndChildRecordsNative(pFilter, this);
#else
	return FALSE;
#endif
}
/********************************************************************************************

>	void TextStory::AddImportedString(ImportedString* pImportedString)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pImportedString = ptr to the imported string object
	Outputs:	-
	Returns:	-
	Purpose:	Adds the object to the list of imported strings

********************************************************************************************/

void TextStory::AddImportedString(ImportedString* pImportedString)
{
	if (pImportedStringList == NULL)
		pImportedStringList = new ImportedStringList;

	if (pImportedStringList != NULL)
		pImportedStringList->AddTail(pImportedString);
}


/********************************************************************************************

>	BOOL TextStory::ExpandImportedStrings()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pImportedString = ptr to the imported string object
	Outputs:	-
	Returns:	-
	Purpose:	Expands any strings that were imported into a series of text characters.

				When text strings are imported from the v2 file format, the first char in the
				string gets inserted into the tree, which marks the position of the begining of the
				string.

				The whole string is then saved (in an ImportedString object), along with the
				inserted text char node, until post-import.

				Post-import then inserts text chars for the rest of the string next to the first
				text char of the string, copying child attrs from the first char to each of the string's
				remaining chars, to ensure that they look correct.

				Why do this on post import?  Why not do it when you get a string record from the file?

				Well, at the point that the string record is read, we don't know what child attrs will
				be applied to it, because, if there are any child attrs, they will appear later in the file.

				There are a number of ways to do it, but they all involve doing something to the string
				*after* you know that the child attrs have been read in.

				Post-import was considered a good place because the necessary processing can be done
				before the story is formatted, it doesn't assume anything about the order of records
				in the file format, and it was a Tuesday.


********************************************************************************************/

BOOL TextStory::ExpandImportedStrings()
{
	// If the list is empty, get out of here, now!
	if (pImportedStringList == NULL)
		return TRUE;

	BOOL ok = TRUE;

	// Expand each string in the list
	ImportedString* pImportedString = pImportedStringList->GetHead();
	while (ok && pImportedString != NULL)
	{
		// get the char node, and a copy of the original record
		TextChar* pChar = pImportedString->GetFirstChar();
		CXaraFileRecord* pRecord = pImportedString->GetRecord();

		if (pChar != NULL && pRecord != NULL)
		{
			WCHAR wChar;

			// The first char is read and discarded because pChar contains the char.
			ok = pRecord->ReadWCHAR(&wChar);

			// Find out the size of the record (and hence the num chars in the string)
			// Num chars = Size/2 (Unicode - double byte chars)
			UINT32 Size = pRecord->GetSize();

			// We only iterate until Size <=2, because we have read and discarded the first
			// char in the string.
			for (;ok && Size > 2;Size-=2)
			{
				// Read the next char code
				ok = pRecord->ReadWCHAR(&wChar);

				TextChar* pNewChar = NULL;

				if (ok) pNewChar = new TextChar();				// Create a new TextChar
				if (ok) ok = (pNewChar != NULL);				//
				if (ok) pNewChar->SetUnicodeValue(wChar);		// Set the char's code
				if (ok) pNewChar->AttachNode(pChar,NEXT);		// Attach it next to the last inserted char
				if (ok) ok = pChar->CopyChildAttrs(pNewChar);	// Make sure it looks the same as the last char
				if (ok) pChar = pNewChar;						// pNewChar now becomes the last char inserted
			}
		}

		// Get the next imported string
		pImportedString = pImportedStringList->GetNext(pImportedString);
	}

	// After expanding the strings, throw them away, and throw the list away too.
	pImportedStringList->DeleteAll();
	delete pImportedStringList;
	pImportedStringList = NULL;

	return ok;
}



/********************************************************************************************
>	void TextStory::Validate(BOOL EnsureOpFlagsReset=TRUE)

  Author:	Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
  Created:	16/6/96
  Inputs:	EnsureOpFlagsReset -
  Purpose:	Perform various integrity checks on a text story in debug builds
********************************************************************************************/

#ifdef _DEBUG
#define VALIDATE
#define TRACEIF(a,b) { if (a) { TRACE b; } }
#endif

void TextStory::Validate(BOOL EnsureOpFlagsReset)
{
PORTNOTE("text","Removed 'Ed' validate from TextStory::Validate")
#ifndef EXCLUDE_FROM_XARALX
#ifdef VALIDATE
if (IsUserName("Ed"))
{
	// choose parent to check from
	Node* pNode = this;
	if (pNode==NULL)
	{
		ERROR3("TestStory::Validate() - could not find desired parent, using top parent");
		pNode = this;
	}

	static INT32 count=0;
	if ((count++ % 12)==0)
		CheckSubtree(pNode);

	ERROR3IF(CharsScale !=FIXED16(1), "TextStory::Validate() - CharsScale!=1");
	ERROR3IF(CharsAspect!=FIXED16(1), "TextStory::Validate() - CharsAspect!=1");
	ERROR3IF(CharsRotation       !=0, "TextStory::Validate() - CharsRoation!=0 - not yet supported");
	ERROR3IF(ImportFormatWidth   !=0, "TextStory::Validate() - ImportFormatWidth!=0");
	ERROR3IF(!TextOnPathTangential,   "TextStory::Validate() - TextOnPathTangential!=0 - not yet supported");
	ERROR3IF(ImportBaseShift!=AlignBaseline,"TextStory::Validate() - ImportBaseShift!=AlignBaseline");
	VisibleTextNode* pLastVTNInStory = this->FindLastVTN();
	ERROR3IF(pLastVTNInStory==NULL,"TextStory::Validate() - story has no VTN!");
	ERROR3IF(!pLastVTNInStory->IsAnEOLNode(),"TextStory::Validate() - last VTN in story is not an EOLNode");

	FIXED16  Scale    = 1;
	FIXED16  Aspect   = 1;
	ANGLE    Rotation = 0;
	ANGLE    Shear    = 0;
	DocCoord Translation(0,0);
	if (StoryMatrix.Decompose(&Scale,&Aspect,&Rotation,&Shear,&Translation))
	{
		const FIXED16 NormaliseLimit = 0.001;
		if (Scale<0)
		{
			Scale = -Scale;
			Shear = -Shear;
		}
		FIXED16 AbsScaleError  = Scale-1;
		FIXED16 AbsAspectError = Aspect-1;
		AbsScaleError  =  AbsScaleError<0 ? -AbsScaleError  : AbsScaleError;
		AbsAspectError = AbsAspectError<0 ? -AbsAspectError : AbsAspectError;
		if (AbsScaleError>NormaliseLimit || AbsAspectError>NormaliseLimit)
			ERROR3("TextStory::Validate() - StroyMatrix scale or aspect not normalised!");
	}
	else
		ERROR3("TextStory::Validate() - failed to decompose story matrix");

	BOOL LineFound  = FALSE;
	BOOL PathFound  = FALSE;
	BOOL CaretFound = FALSE;
	Node* pNodeInStory = this->FindFirstChild();
	while (pNodeInStory!=NULL)
	{
		if (EnsureOpFlagsReset && pNodeInStory->IsABaseTextClass())
		{
			BaseTextClass* pBTC = (BaseTextClass*)pNodeInStory;
			ERROR3IF(pBTC->Affected(),              "TextStory::Validate() - AffectedFlag Set!");
			ERROR3IF(pBTC->DescendantAffected(),    "TextStory::Validate() - DescendantAffectedFlag Set!");
			ERROR3IF(pBTC->ModifiedByOp(),          "TextStory::Validate() - ModifiedByOpFlag Set!");
			ERROR3IF(pBTC->DescendantModifiedByOp(),"TextStory::Validate() - DescendantModifiedByOpFlag Set!");
		}
		if (pNodeInStory->IsAnAttribute())
		{
			ERROR3IF(LineFound,"TextStory::Validate() - Attribute found after first line in story");
			ERROR3IF(PathFound,"TextStory::Validate() - Attribute found after path in story");
			NodeAttribute* pAttr = (NodeAttribute*)pNodeInStory;
			if (pAttr->IsALineLevelAttrib() && !TextLine::IsAttrTypeLineLevel(pAttr->GetAttributeType()))
				ERROR3("TextStory::Validate() - TextLine::IsAttrTypeLineLevel() out of date?");
		}
		else if (IS_A(pNodeInStory,NodePath))
		{
			ERROR3IF(LineFound,"TextStory::Validate() - Path is not first object in story");
			CheckLeaf(pNodeInStory);
			PathFound = TRUE;
		}
		else if (IS_A(pNodeInStory,TextLine))
		{
			BOOL VTNFound   = FALSE;
			BOOL EOLFound   = FALSE;
			Node* pNodeInLine = pNodeInStory->FindFirstChild();
			while (pNodeInLine!=NULL)
			{
				if (EnsureOpFlagsReset && pNodeInStory->IsABaseTextClass())
				{
					BaseTextClass* pBTC = (BaseTextClass*)pNodeInStory;
					ERROR3IF(pBTC->Affected(),              "TextStory::Validate() - AffectedFlag Set!");
					ERROR3IF(pBTC->DescendantAffected(),    "TextStory::Validate() - DescendantAffectedFlag Set!");
					ERROR3IF(pBTC->ModifiedByOp(),          "TextStory::Validate() - ModifiedByOpFlag Set!");
					ERROR3IF(pBTC->DescendantModifiedByOp(),"TextStory::Validate() - DescendantModifiedByOpFlag Set!");
				}
				if (pNodeInLine->IsAnAttribute())
				{
					ERROR3IF(VTNFound,"TextStory::Validate() - Attribute found after first VTN in line");
					NodeAttribute* pAttr = (NodeAttribute*)pNodeInLine;
					if (pAttr->IsALineLevelAttrib() && !TextLine::IsAttrTypeLineLevel(pAttr->GetAttributeType()))
						ERROR3("TextStory::Validate() - TextLine::IsAttrTypeLineLevel() out of date?");
				}
				else if (IS_A(pNodeInLine,CaretNode))
				{
					ERROR3IF(CaretFound,"TextStory::Validate() - story has more than 1 caret");
					CheckLeaf(pNodeInLine);
					VTNFound   = TRUE;
					CaretFound = TRUE;
				}
				else if (IS_A(pNodeInLine,EOLNode))
				{
					ERROR3IF(EOLFound,"TextStory::Validate() - line has more than 1 EOL");
					CheckLeaf(pNodeInLine);
					VTNFound = TRUE;
					EOLFound = TRUE;
				}
				else if (pNodeInLine->IsAVisibleTextNode())
				{
					ERROR3IF(EOLFound,"TextStory::Validate() - VTN found after EOL on line");
					CheckLeaf(pNodeInLine);
					VTNFound = TRUE;
				}
				else if (!pNodeInLine->IsNodeHidden())
					ERROR3("TextStory::Validate() - unknown node in line (ie not a NodeAttribute, VisibleTextNode or NodeHidden)");
				pNodeInLine = pNodeInLine->FindNext();
			}
			LineFound = TRUE;
		}
		else if (!pNodeInStory->IsNodeHidden())
			ERROR3("TextStory::Validate() - unknown node in story (ie not a NodeAttribute, NodePath, TextLine or NodeHidden)");
		pNodeInStory = pNodeInStory->FindNext();
	}
	ERROR3IF(!LineFound, "TextStory::Validate() - story has no lines!");
//	ERROR3IF(!CaretFound,"TextStory::Validate() - story has no caret!");
	TRACEIF(!CaretFound,("TextStory::Validate() - story has no caret!\n"));
}
#endif
#endif
}


/********************************************************************************************
>	void TextStory::CheckLeaf(Node* pLeafNode)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		pLeafNode -
	Purpose:	Ensure no Linelevel attrs below line level
********************************************************************************************/
	
void TextStory::CheckLeaf(Node* pLeafNode)
{
#ifdef VALIDATE
	if (pLeafNode==NULL)
	{
		ERROR3("TextStory::CheckLeaf() - pNode==NULL");
		return;
	}

	Node* pNodeOnLeaf = pLeafNode->FindFirstChild();
	while (pNodeOnLeaf!=NULL)
	{
		if (pNodeOnLeaf->IsAnAttribute())
		{
			if ( ((NodeAttribute*)pNodeOnLeaf)->IsALineLevelAttrib() )
			{
				if (IS_A(pLeafNode,CaretNode))
					TRACE( _T("TextStory::CheckLeaf() - Line level attr applied to caret!\n"));
				else
					ERROR3("TextStory::CheckLeaf() - Line level attr applied below line level");
			}
		}
		pNodeOnLeaf = pNodeOnLeaf->FindNext();
	}
#endif
}


/********************************************************************************************
>	void TextStory::CheckSubtree(Node*        pNode,
								 AttrTypeSet* pParentAttrSet      = NULL,
								 BOOL*        pDescendantSelected = NULL)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/6/96
	Inputs:		pNode               -
				pParentAttrSet      -
	Outputs:	pDescendantSelected -
	Purpose:	Validate a subtree, checking for:
				- duplication of attribute on node or parent (except caret)
				- nodes with child objects must be flagged as compound
				- attributes   are not flagged as selected or parent of selected
				- hidden nodes are not flagged as selected,   parent of selected or compound
				- node is not both flagged as selected and parent of selected
				- node with    selected descendant is     flagged as parent of selected
				- node with no selected descendant is not flagged as parent of selected
				- no sibling attributes
********************************************************************************************/

#include "nodedoc.h"

void TextStory::CheckSubtree(Node* pNode, AttrTypeSet* pParentAttrSet, BOOL* pDescendantSelected)
{
#ifdef VALIDATE
	if (pNode==NULL)
	{
		ERROR3("CheckSubtree() - pNode==NULL");
		return;
	}

PORTNOTE("text","Removed AfxCheckMemory")
#ifndef EXCLUDE_FROM_XARALX
	ERROR3IF(!AfxCheckMemory(),"CheckSubtree() - memory corrupt!");
#endif

	// get ptr to doc (if not an isolated subtree) as children are default attrs
	NodeDocument* pDoc = NULL;
	if (IS_A(pNode,NodeDocument))
		pDoc = (NodeDocument*)pNode;
	else
		pDoc = (NodeDocument*)(pNode->FindParent(CC_RUNTIME_CLASS(NodeDocument)));

	// flag attr set not yet coied - only do so if we have to as it is SLOOOW
	AttrTypeSet* pAttrSet = NULL;

	BOOL ChildSelected    = FALSE;
	BOOL ChildParentOfSel = FALSE;
	BOOL ChildObjectFound = FALSE;
	Node* pChildNode = pNode->FindFirstChild();
	while (pChildNode)
	{
		TRACEIF( pChildNode->IsSelected() && pChildNode->IsParentOfSelected(), 
			( _T("CheckSubtree() - node (%s) both selected and parent of selected\n"), GetNodeInfo(pChildNode) ) );
		ChildSelected    |= pChildNode->IsSelected();
		ChildParentOfSel |= pChildNode->IsParentOfSelected();
		if (pChildNode->IsAnAttribute())
		{
			// if not already done, if no Attr set passed in, create one else copy the given one
			if (pAttrSet==NULL)
			{
				if (pParentAttrSet==NULL)
					pAttrSet = new AttrTypeSet;
				else
					pAttrSet = pParentAttrSet->CopySet();
			}

			TRACEIF( ChildObjectFound,		( _T("CheckSubtree() - attribute (%s) found after child object\n"), GetNodeInfo(pChildNode) ) );
			NodeAttribute* pAttr = (NodeAttribute*)pChildNode;
			TRACEIF(pAttr->IsSelected(),	( _T("CheckSubtree() - attribute (%s) selected\n"), GetNodeInfo(pAttr) ) );
			TRACEIF(pAttr->IsParentOfSelected(),( _T("CheckSubtree() - attribute (%s) parent of selected\n"), GetNodeInfo(pAttr) ) );
			if (!IS_A(pNode,CaretNode))
			{
				// if attrbute already in set it is a duplicate of one on a parent (exc defaults)
				CCRuntimeClass* pAttrType = pAttr->GetAttributeType();
				TRACEIF( pAttrSet->InSet(pAttrType), ( _T("CheckSubtree() - attribute (%s) duplicate of one on self (%s) or parent\n"),
					GetNodeInfo(pAttr),GetNodeInfo(pNode)));

				// if not an isolated subtree and not default attr, check for duplicated default attrs
				if (pDoc!=NULL && pNode!=pDoc)
				{
					NodeAttribute* pDefaultAttr = (NodeAttribute*)(pDoc->FindFirstChild(pAttrType));
					TRACEIF( pDefaultAttr==NULL, ( _T("CheckSubtree() - attribute (%s) found for which there is no default!\n"),
						GetNodeInfo(pAttr)));
					TRACEIF( (*pDefaultAttr)==(*pAttr), ( _T("CheckSubtree() - duplicate of default attr (%s) found!\n"),
						GetNodeInfo(pAttr)));
				}

				// if not default doc attrs, add to attr set
				if (pNode!=pDoc)
					if (pAttrSet->AddToSet(pAttrType)==FALSE)
						ERROR3("CheckSubtree() - AttrTypeSet::AddToSet() failed");
			}
		}
		else if (pChildNode->IsNodeHidden())
		{
			NodeHidden* pNodeHidden = (NodeHidden*)pChildNode;
			TRACEIF(pNodeHidden->IsSelected()        ,( _T("CheckSubtree() - hidden node (%s) selected\n"),
				GetNodeInfo(pChildNode)));
			TRACEIF(pNodeHidden->IsParentOfSelected(),( _T("CheckSubtree() - hidden node (%s) parent of selected\n"),
				GetNodeInfo(pChildNode)));
			TRACEIF(pNodeHidden->IsCompound(),        ( _T("CheckSubtree() - hidden node (%s) is compound\n"), 
				GetNodeInfo(pChildNode)));
		}
		else
		{
			Node* pNode = pChildNode;
			BOOL DescendantSelected = FALSE;
			// if not already done, if no Attr set passed in, create one else copy the given one
			if (pAttrSet==NULL)
			{
				if (pParentAttrSet==NULL)
					pAttrSet = new AttrTypeSet;
				else
					pAttrSet = pParentAttrSet->CopySet();
			}
			CheckSubtree(pNode, pAttrSet, &DescendantSelected);
			TRACEIF( DescendantSelected && !pChildNode->IsParentOfSelected(),
				( _T("CheckSubtree() - descendant selected but node (%s) not parent of selected\n"), GetNodeInfo(pNode)));
//			TRACEIF(!DescendantSelected &&       pNode->IsParentOfSelected(),("CheckSubtree() - node (%s) parent of selected but no descendant selected\n",GetNodeInfo(pNode)));
			if (!DescendantSelected && pNode->IsParentOfSelected())
				TRACE( _T("CheckSubtree() - node (%s) parent of selected but no descendant selected\n"),GetNodeInfo(pNode));
			ChildObjectFound = TRUE;
		}
		pChildNode = pChildNode->FindNext();
	}
	TRACEIF( ChildObjectFound && !pNode->IsCompound(), ( _T("CheckSubtree() - child object found on non-compound node (%s)\n"),
		GetNodeInfo(pNode)));

	if (pAttrSet!=NULL)
	{
		pAttrSet->DeleteAll();
		delete pAttrSet;
	}

	if (pDescendantSelected!=NULL)
		*pDescendantSelected = ChildSelected || ChildParentOfSel;
#endif
}


/********************************************************************************************
>	TCHAR* TextStory::GetNodeInfo(Node* pNode)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/7/96
	Inputs:		pNode -
	Purpose:	return string with CC_RUNTIME_CLASS name of node and address
********************************************************************************************/

TCHAR* TextStory::GetNodeInfo(Node* pNode)
{
	static TCHAR pDesc[256];
	if (pNode!=NULL)
		camSnprintf( pDesc, 256, _T("%s @ 0x%p\0"), pNode->GetRuntimeClass()->GetClassName(), pNode );
	else
		camSnprintf( pDesc, 256, _T("\0") );
	return pDesc;
}



/********************************************************************************************
>	void TextStory::DeleteChildAttribute(NodeRenderableInk* pParent, CCRuntimeClass* pReqdAttrib)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/96
	Inputs:		pParent - Parent node of the attribute
				pReqdAttrib - Attribute type to delete
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Finds and deletes a child attribute of the given type
	SeeAlso:	TextStory::CreateFromChars.
********************************************************************************************/
BOOL TextStory::DeleteChildAttribute(NodeRenderableInk* pParent, CCRuntimeClass* pReqdAttrib)
{
	ERROR2IF(pParent==NULL || pReqdAttrib==NULL, FALSE, "NULL parameter");

	NodeAttribute* pAppliedAttr = pParent->GetChildAttrOfType(pReqdAttrib);

	ERROR3IF(pAppliedAttr==NULL, "Attribute not found");
		
	if (pAppliedAttr != NULL)
	{
		pAppliedAttr->CascadeDelete();
		delete pAppliedAttr;
	}

	return TRUE;
}



/********************************************************************************************
>	BOOL TextStory::OKToExport()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		-
	Outputs:	-
	Returns:	FALSE if this story should not be exported (ie it's duff).  TRUE otherwise
	Purpose:	Determines wether or not this text story should be exported.  If there are no
				characters in it then we don't want to save it as it will cause bother when
				loaded back.
********************************************************************************************/
BOOL TextStory::OKToExport()
{
	// See if we have any visible characters inside us.
	VisibleTextNode* pVTN = FindFirstVTN();
	BOOL FoundCharacter = FALSE;
	while (pVTN!=NULL && !FoundCharacter)
	{
		if (pVTN->IsATextChar())
			FoundCharacter = !pVTN->IsAVisibleSpace();
		pVTN = pVTN->FindNextVTNInStory();
	}

	if (FoundCharacter)
		return TRUE;

	// If we get here then we are an empty story.  Is it safe to not export us?
	// Scan up the tree.  If we reach a layer without passing though compounds other than groups it's fine.
	Node* pParent = FindParent();
	while (pParent!=NULL && !pParent->IsLayer())
	{
		if (pParent->IsCompound() && !IS_A(pParent, NodeGroup))
			return TRUE;	// Must export the story as the compound may rely on its presence.

		pParent = pParent->FindParent();
	}

	// If we get here it's safe to not export this story.
	return FALSE;
}



/********************************************************************************************
>	DocRect TextStory::GetUTStoryBounds()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/96
	Inputs:		-
	Outputs:	-
	Returns:	The untransformed bounds of the story
	Purpose:	The bounding box of the story in it's untransformed space.
********************************************************************************************/
DocRect TextStory::GetUTStoryBounds()
{
	// Get pointers to first and last lines in story
	TextLine* pFirstLine = FindFirstLine();
	TextLine* pLastLine = FindLastLine();
	ERROR3IF((pFirstLine==NULL || pLastLine==NULL), "Story has no text lines");

	if (pFirstLine!=NULL && pLastLine!=NULL)
	{
		// Get the width of the widest line in the story
		MILLIPOINT WidestLineWidth = 0;
		TextLine* pLine = pFirstLine;
		while (pLine != NULL)
		{
			VisibleTextNode* pLastVTN = pLine->FindLastVTN();

			if (pLastVTN!=NULL)
			{
				const MILLIPOINT Dist = pLastVTN->CalcCharDistAlongLine(TRUE);

				if (Dist > WidestLineWidth)
					WidestLineWidth = Dist;
			}

			pLine = (TextLine*)pLine->FindNext(CC_RUNTIME_CLASS(TextLine));
		}

		INT32 LowY = pLastLine->GetPosInStory() + pLastLine->GetLineDescent();
		INT32 HighY = -pFirstLine->GetLineAscent();

		return DocRect( 0,	// Err, this only works for left justified stories!
						(LowY < HighY) ? LowY : HighY,
						WidestLineWidth,
						(LowY < HighY) ? HighY : LowY);
	}
	else
		return DocRect(0,0,0,0);
}



/********************************************************************************************

>	DocRect TextStory::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		parameters describing the extension.
	Returns:	TRUE if this text story can be validly extended,
				FALSE otherwise.
	Purpose:	Tests to see whether this text-story's extend-centre is positioned
				so as to make an extend operation irreversible.
	See also:	Extender class.

********************************************************************************************/
DocRect TextStory::ValidateExtend(const ExtendParams& ExtParams)
{
	// if we lie on a curve, we must test that curve and our attributes,
	// otherwise we must test ourself, then pass the test on to our children.
	DocRect drMinExtend(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
	NodePath* pnPath = GetTextPath();
	if (pnPath == NULL)
	{
		DocCoord doccArray[1] = { FindExtendCentre() };
		drMinExtend = Extender::ValidateControlPoints(1, doccArray, ExtParams);

		// if we didn't invalidate the extension, we must call the base class
		// implementation, which will validate our children.
		if (drMinExtend.lo.x == INT32_MAX &&
			drMinExtend.lo.y == INT32_MAX &&
			drMinExtend.hi.x == INT32_MAX &&
			drMinExtend.hi.y == INT32_MAX)
			drMinExtend = Node::ValidateExtend(ExtParams);
	}
	else
	{
		drMinExtend = pnPath->ValidateExtend(ExtParams);

		DocRect drThisMinExtend;
		for (	Node* pChildNode = FindFirstChild();
				pChildNode != NULL;
				pChildNode = pChildNode->FindNext()	)
		{
			if (!pChildNode->IsAnAttribute())
				continue;

			drThisMinExtend = pChildNode->ValidateExtend(ExtParams);
			if (drMinExtend.lo.x > drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
			if (drMinExtend.lo.y > drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
			if (drMinExtend.hi.x > drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
			if (drMinExtend.hi.y > drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
		}
	}

	return drMinExtend;
}



/********************************************************************************************

>	void TextStory::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		parameters describing the extension.
	Purpose:	Perform an Extend operation on this text-story.
	See also:	Extender class.

********************************************************************************************/
void TextStory::Extend(const ExtendParams& ExtParams)
{
	// if we lie on a curve, we must extend that curve, together with our attributes,
	// otherwise we must extend ourself, then extend our children.
	NodePath* pnPath = GetTextPath();
	if (pnPath == NULL)
	{
		// do the extension operations on ourself.
		// text stories never stretch, they just translate.
		TransformTranslateNoStretchObject(ExtParams);
		TransformTranslateObject(ExtParams);

		// do the base-class implementation to extend our children.
		Node::Extend(ExtParams);
	}
	else
	{
		// extend our path.
		pnPath->Extend(ExtParams);

		// extend our attributes.
		for (	Node* pChildNode = FindFirstChild();
				pChildNode != NULL;
				pChildNode = pChildNode->FindNext()	)
		{
			if (pChildNode->IsAnAttribute())
				pChildNode->Extend(ExtParams);
		}
	}
}



/********************************************************************************************

>	DocCoord TextStory::FindExtendCentre()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Returns:	The centre-point which the extender routines use to determine whether
				an extension is valid, and in which detection it should occur.
	Purpose:	Find the extend-centre reference point.
	See also:	Extender class.

********************************************************************************************/
DocCoord TextStory::FindExtendCentre()
{
	DocRect rectNode = GetBoundingRect();
	DocCoord doccCentre = rectNode.Centre();
	switch ( FindJustification() )
	{
	case JCENTRE:
	case JFULL:
		// do nothing - the centre of the rectangle is required.
		break;

	case JRIGHT:
		doccCentre.x = rectNode.hi.x;
		break;

	case JLEFT:
	default:
		doccCentre.x = rectNode.lo.x;
		break;
	}

	return doccCentre;
}



/********************************************************************************************

>	Justification TextStory::FindJustification()

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Returns:	The Justification of this text-story.
	Purpose:	Used in conjunction with Extend() and its sister functions to determine which
				way a text-story should extend. If you use this function for another purpose,
				read it and make sure it does what you think it should do first.
	Notes:		Be aware that this function uses IsKindOf() to
				test this node's children within the tree.
	See also:	Extender class.

********************************************************************************************/
Justification TextStory::FindJustification()
{
	// first, search for a text-justify attribute underneath this TextStory in the tree.
	Node* pNode = FindFirstDepthFirst();
	while (pNode != NULL && !pNode->IsKindOf(CC_RUNTIME_CLASS(AttrTxtJustification)))
		pNode = pNode->FindNextDepthFirst(this);

	// if we didn't find one, then find the attribute currently applied to pTextStory.
	BOOL bFoundAttr = FALSE;
	AttrTxtJustification* pAttrJustify = NULL;
	if (pNode != NULL)
	{
		pAttrJustify = (AttrTxtJustification*)pNode;
		bFoundAttr = TRUE;
	}
	else
		bFoundAttr = FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTxtJustification), (NodeAttribute**)&pAttrJustify);

	// if we retrieved an attribute, get its value.
	// if we have no luck, we'll just use JLEFT as the default.
	Justification justify = JLEFT;
	if (bFoundAttr)
	{
		TxtJustificationAttribute* textJustAttr = (TxtJustificationAttribute*)pAttrJustify->GetAttributeValue();
		if (textJustAttr != NULL)
			justify = textJustAttr->justification;
	}

	return justify;
}

String_256 TextStory::GetStoryAsString()
{
	String_256 Content = "";
	String_256 AddTemplate = "x";
	Node * pTextLine = FindFirstChild(CC_RUNTIME_CLASS(TextLine));

	INT32 StringMaxSize = 255;

	while (pTextLine && StringMaxSize > 0)
	{
		Node * pChar = pTextLine->FindFirstChild(CC_RUNTIME_CLASS(TextChar));

		while (pChar && StringMaxSize > 0)
		{
			*AddTemplate = (TCHAR) (((TextChar *)pChar)->GetUnicodeValue());
			Content += AddTemplate;

			StringMaxSize--;

			pChar = pChar->FindNext(CC_RUNTIME_CLASS(TextChar));
		}

		pTextLine = pTextLine->FindNext(CC_RUNTIME_CLASS(TextLine));

		if (pTextLine)
		{
			Content += _T("\n");
			StringMaxSize -=2;
		}

	}

	return Content;
}



/********************************************************************************************

>	BOOL TextStory::IsGradientFilled()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/11/00
	Returns:	Whether the text story is gradient filled (true), or not (false)
	Purpose:	Looks through the list of nodes under the text story node to see if there 
				are any gradient fills on this object.
	.
	See also:	TextStory::PreExportRender

********************************************************************************************/
BOOL TextStory::IsGradientFilled ()
{
	PORTNOTETRACE("text","TextStory::IsGradientFilled - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ListItem	*pItem			= NULL;
	List		GradientList;
	BOOL		filled			= FALSE;

	// Now get the list of nodes.
	BevelTools::GetAllNodesUnderNode ( this, &GradientList,
									   CC_RUNTIME_CLASS ( AttrFillGeometry ) );

	// (ChrisG 8/11/00) - Cycle through the gradient list finding if there are any
	//	gradient fills.
	pItem = GradientList.GetHead ();
	while ( (pItem != NULL) && (filled == FALSE) )
	{
		NodeListItem		*pNodeItem	= ( NodeListItem* ) pItem;
		AttrFillGeometry	*pFill		= ( AttrFillGeometry* ) pNodeItem->pNode;

		// if it is a graduated colour fill, then it is gradient filled
		if ( pFill->IsAColourFill () &&			// Is it a colour fill?
			 pFill->IsAGradFill ())				// Is it a graduated fill?
		{
			filled = TRUE;
		}

		// Get the next list item.
		pItem = GradientList.GetNext ( pItem );
	}

	GradientList.DeleteAll();

	return filled;
#else
	return true;
#endif
}
