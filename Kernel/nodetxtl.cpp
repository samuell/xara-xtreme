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
#include "nodetxtl.h"

//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "blobs.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "group.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "paths.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "mario.h"
//#include "nodeattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodepath.h"
#include "nodetext.h"
#include "nodetxts.h"
//#include "simon.h"
//#include "textinfo.h"
//#include "matrix.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "cameleps.h"
#include "pathproc.h"
#include "fontman.h"
#include "attrmap.h"
#include "cxftext.h"

DECLARE_SOURCE("$Revision$")				 

CC_IMPLEMENT_MEMDUMP(TextLineInfo,      CC_CLASS_MEMDUMP)
CC_IMPLEMENT_DYNAMIC(TextLine, BaseTextClass)
CC_IMPLEMENT_DYNAMIC(FormatRegion, RenderRegion)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////////////////////
// TextLineInfo Methods

/********************************************************************************************
>	TextLineInfo::TextLineInfo()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/96
	Purpose:	default constructor
********************************************************************************************/

TextLineInfo::TextLineInfo()
{
	SumCharAdvances = 0;
	justification   = JLEFT;
	LeftMargin      = 0;
	RightMargin     = 0;
	ParaLeftMargin = 0;
	ParaRightMargin = 0;
	Ruler = NULL;
	WordWrapping    = FALSE;
	NumChars        = 0;
	NumSpaces       = 0;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// TextLine Methods

/********************************************************************************************
>	void TextLine::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/96
	Purpose:	common init function for constructors
********************************************************************************************/

void TextLine::Init()
{
	mLineAscent  = 0;
	mLineDescent = 0;
	mLineSize    = 0;

	mJustification  = JLEFT;
	mLineSpacing    = 0;
	mLineSpaceRatio = 1;
	mpRuler = new TxtRuler;

	mPosInStory  = 0;
}


/********************************************************************************************
>	TextLine::TextLine()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple TextLine constructor, it is required so that SimpleCopy will work.

********************************************************************************************/

TextLine::TextLine(): BaseTextClass()	// Call the base class
{
	Init();
}

/********************************************************************************************
>	TextLine::~TextLine()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	20/07/06
	Purpose:	Destructor

********************************************************************************************/

TextLine::~TextLine()
{
	delete mpRuler;
}
 
/********************************************************************************************
>	TextLine::TextLine(Node* ContextNode, AttachNodeDirection Direction)

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

	Purpose:	The main TextLine constructor
********************************************************************************************/

TextLine::TextLine(Node* ContextNode,  
		 			 AttachNodeDirection Direction):BaseTextClass(ContextNode, Direction)
{
	Init();
}


/********************************************************************************************
>	virtual CopyType TextLine::GetCopyType()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Returns:	A copy type describing how to copy this object
    Purpose:    This function returns a type describing how this object is to be copied.
				The fuction is called from the low level copy operation CopyObjects.
				There are two options at present, these being SIMPLECOPY and COMPLEXCOPY.
				SIMPLECOPY indicates that the node can be copied by a call to its virtual
				function SimpleCopy(). 
				COMPLEXCOPY however indicates that the node needs to do its own thing when
				copying and must be called via the ComplexCopy() virtual function. This
				virtual will likely return a tree of copied objects rather than just a
				copy of itself.
********************************************************************************************/

CopyType TextLine::GetCopyType()
{
	return COMPLEXCOPY;
}


/********************************************************************************************
>	Node* TextLine::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	A copy of the node, or NULL if we are out of memory 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes of Node  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
********************************************************************************************/

Node* TextLine::SimpleCopy()
{
	// Make a new TextLine and then copy things into it
	TextLine* NodeCopy = new TextLine();

    ERROR1IF(NodeCopy == NULL, NULL, _R(IDE_NOMORE_MEMORY)); 

    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}


/********************************************************************************************
>	virtual INT32 TextLine::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		Stage		- COPYOBJECT   if we should make a copy
							- COPYFINISHED once the entire copy operation has completed
				RangeToCopy - Describes the range which is currently being copied.
				pOutput		- a pointer to a pointer to a node. Set this pointer to point
							  at the tree copy
	Returns:	-1	= The routine failed to make a copy.
				 0	= No copy has been made.
				+1	= pOutput points to the copy.
    Purpose:    If the copystage is COPYOBJECT,
       			The node has been called to copy itself and do what ever it needs to to
				make a sensible copy of other items such as attributes. The caller
				(CopyObjects) will not deep copy this node (as this is a complex copy and
				it expects the handler to know what its doing). In this case the TextLine
				object cannot exist on its own. It needs a TextStory as a parent and
				however many text characters that live inside it. Hence this is what is
				returned.
	SeeAlso		Node::ComplexCopy(), CopyObjects()
********************************************************************************************/

INT32 TextLine::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)
{
	TextStory* pTextStory = FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"TextLine::ComplexCopy() - pTextStory==NULL");
	return pTextStory->BaseComplexCopy(Stage, RangeToCopy, pOutput);
}


/********************************************************************************************
>	virtual INT32 TextLine::ComplexHide(UndoableOperation* pOp, Node* pNextNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		pOp			- a pointer to an undoable operation
				pNextNode	- a pointer to the next node in range.
	Returns:	-1	= The routine failed to hide this node.
				 0	= Ignored, this object does not support complex hide operations, so
				      carry on and hide the node as normal.
				+1	= The node and possibly various others have been hidden correctly.
    Purpose: Override the node level virtual function ComplexHide. This gives us a chance to
			 get in and hide the various selected members of a text story sensibly. We
			 hide all necessary nodes when the last member of the text story is called to
			 complex hide itself, otherwise we may corrupt the range being scanned
********************************************************************************************/

INT32 TextLine::ComplexHide(UndoableOperation* pOp, Node* pNextNode)
{
	// if there is no next node in the range then we need to hide all nodes
	BOOL CallComplexHide = TRUE;
	TextStory* pThisStory = FindParentStory();
	ERROR2IF(pThisStory==NULL,FALSE,"VisibleTextNode::ComplexHide() - pThisStory==NULL");

	if (pNextNode)
	{
		if ( (pNextNode->IsAVisibleTextNode()) ||
		     (IS_A(pNextNode,TextLine))
		   )
		{
			TextStory* pTextStory = FindParentStory();
			ERROR2IF(pTextStory==NULL,FALSE,"VisibleTextNode::ComplexHide() - pTextStory==NULL");

			if (pThisStory==pTextStory)
				CallComplexHide=FALSE;
		}
	}

	if (CallComplexHide)
		return pThisStory->BaseComplexHide(pOp);
	else
		return 1;
}


/***********************************************************************************************
>	void TextLine::CopyNodeContents(TextLine* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
***********************************************************************************************/

void TextLine::CopyNodeContents(TextLine* NodeCopy)
{
	// Ask the base class to do its bit
	NodeRenderableBounded::CopyNodeContents(NodeCopy);

	// Copy specifics
	NodeCopy->mLineAscent  = mLineAscent;
	NodeCopy->mLineDescent = mLineDescent;
	NodeCopy->mLineSize    = mLineSize;

	NodeCopy->mJustification  = mJustification;
	NodeCopy->mLineSpacing    = mLineSpacing;
	NodeCopy->mLineSpaceRatio = mLineSpaceRatio;
	NodeCopy->mLeftMargin = mLeftMargin;
	NodeCopy->mFirstIndent = mFirstIndent;
	NodeCopy->mRightMargin = mRightMargin;
	*NodeCopy->mpRuler = *mpRuler;

	NodeCopy->mPosInStory  = mPosInStory;
}


/***********************************************************************************************
>   void TextLine::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void TextLine::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, TextLine), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, TextLine))
		CopyNodeContents((TextLine*)pNodeCopy);
}



/********************************************************************************************
>	BOOL TextLine::CreateNodeGroup(NodeGroup** ppNodeGroup,
									FormatRegion* pFormatRegion, BecomeA* pBecomeA)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Inputs:		pFormatRegion - format region with current attr stack
				pBecomeA      - ptr to class containing info needed to become a new type of node
	Outputs:	ppNodeGroup   - pointer to new node group (if not passing back)
	Returns:	FALSE if fails
	Purpose:	Creates a NodeGroup containing all chars converted to NodePaths
********************************************************************************************/

BOOL TextLine::CreateNodeGroup(NodeGroup** ppNodeGroup, FormatRegion* pFormatRegion, BecomeA* pBecomeA)
{
#ifndef DISABLE_TEXT_RENDERING
	ERROR2IF(pFormatRegion==NULL,FALSE,"TextChar::CreateNodeGroup() - pFormatRegion==NULL");
	ERROR2IF(     pBecomeA==NULL,FALSE,"TextChar::CreateNodeGroup() - pBecomeA==NULL");
	ERROR2IF(  ppNodeGroup==NULL,FALSE,"TextChar::CreateNodeGroup() - ppNodeGroup==NULL");

	pFormatRegion->SaveContext();

	// here to overcome scope problem
	Node* pNode=NULL;
	TextChar* pTextChar=NULL;

	// if not passing back, create a NodeGroup to encompass the line, and copy non-text line attributes
	// BODGE - should use ALLOC_WITH_FAIL
	NodeGroup* pLineNodeGroup=NULL;
	if (pBecomeA->GetReason()!=BECOMEA_PASSBACK)
	{
		pLineNodeGroup=new NodeGroup;
		if (pLineNodeGroup==NULL)
			goto Fail;
	}

	pNode=FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsAnAttribute())
		{
			pNode->Render(pFormatRegion);	// render attributes
			if (pBecomeA->GetReason()!=BECOMEA_PASSBACK)
				if (!pNode->IsKindOfTextAttribute())
					if (pNode->CopyNode(pLineNodeGroup, LASTCHILD)==FALSE)
						goto Fail;
		}
		pNode=pNode->FindNext();
	}

	// For each child TextChar convert it to a NodePath 
	// then EITHER pass it back OR attach it to the line group with non-text char attributes
	pTextChar=(TextChar*)FindFirstChild(CC_RUNTIME_CLASS(TextChar));
	while (pTextChar)
	{
		pFormatRegion->SaveContext();
		pTextChar->RenderChildAttrs(pFormatRegion);
		NodePath* pCharNodePath=NULL;
		if (pTextChar->CreateNodePath(&pCharNodePath,pFormatRegion)==FALSE)
			goto Fail;
		if (pCharNodePath)
		{
			if (pBecomeA->GetReason()==BECOMEA_PASSBACK)
			{
				if (pBecomeA->PassBack(pCharNodePath,pTextChar)==FALSE)
					goto Fail;
			}
			else
			{
				pCharNodePath->AttachNode(pLineNodeGroup,LASTCHILD);
				Node* pNode=pTextChar->FindFirstChild();
				while (pNode)
				{
					if (pNode->IsAnAttribute() && !pNode->IsKindOfTextAttribute())
						if (pNode->CopyNode(pCharNodePath, LASTCHILD)==FALSE)
							goto Fail;
					pNode=pNode->FindNext();
				}

				pBecomeA->PassBack(pCharNodePath, pTextChar);
			}
		}
		pFormatRegion->RestoreContext();
		pTextChar=(TextChar*)(pTextChar->FindNext(CC_RUNTIME_CLASS(TextChar)));
	}

	*ppNodeGroup=pLineNodeGroup;
	pFormatRegion->RestoreContext();
	return TRUE;

Fail:
	if (pLineNodeGroup)
	{
		pLineNodeGroup->CascadeDelete();
		delete pLineNodeGroup;
	}
	pFormatRegion->RestoreContext();
#endif
	return FALSE;
}


/********************************************************************************************
>	static TextLine* TextLine::CreateEmptyTextLine(Node* pContextNode=NULL,
												   AttachNodeDirection Direction=FIRSTCHILD)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/5/95
	Inputs:		pContextNode - node to attach it to (or NULL if not to be attached)
				Direction    - direction in which to attach
	Returns:	pointer to new TextLine (or NULL if fails)
	Purpose:	Create a TextLine with EOL and attach it to another node if required
********************************************************************************************/

TextLine* TextLine::CreateEmptyTextLine(Node* pContextNode, AttachNodeDirection Direction)
{
	TextLine* pTextLine = new TextLine();
	if (pTextLine==NULL)
		return NULL;

	EOLNode* pEOL = new EOLNode(pTextLine,FIRSTCHILD);
	if (pEOL==NULL)
	{
		pTextLine->CascadeDelete();
		delete pTextLine;
		return NULL;
	}

	if (pContextNode!=NULL)
		pTextLine->AttachNode(pContextNode,Direction);

	return pTextLine;
}


/********************************************************************************************
>	String TextLine::Describe(BOOL Plural, BOOL Verbose = TRUE)  		   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		Plural:	Singular or plural description
	Returns:	A string describing the node
	Purpose:	Gives a description of the TextLine node for the status line etc
********************************************************************************************/

String TextLine::Describe(BOOL Plural, BOOL Verbose)  		   
{
	if (Plural)
		return(String(_R(IDS_DESCRIBE_TEXTLINEP)));  
	else
		return(String(_R(IDS_DESCRIBE_TEXTLINES))); 
}


/*******************************************************************************************
	void TextLine::GetBlobBoundingRect()

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	10/4/95
 	Returns:	The rectangle of the document affected by the blobs on this TextLine
 	Purpose:	This function returns the blob bounding rectangle of this TextLine.  There 
				is a tiny blob on the top left of the first character on the line.
********************************************************************************************/

DocRect TextLine::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord BlobPos=GetTinyBlobPos();
	DocRect BlobBounds;
	BlobManager* pBlobMgr=GetApplication()->GetBlobManager();
	if (pBlobMgr)
		pBlobMgr->GetBlobRect(BlobPos,&BlobBounds);
	else
		ERROR3("TextLine::GetBlobBoundingRect() - Couldn't find the BlobManager");

	IncludeChildrensBoundingRects(&BlobBounds);

	return BlobBounds;
#else
	return DocRect(0,0,0,0);
#endif
}


/*******************************************************************************************
	DocCoord TextLine::GetTinyBlobPos()

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	10/4/95
 	Returns:	The position of the tiny bob on this TextLine
 	Purpose:	This function returns the location of the tiny blob on this TextLine.  It is
				either on the top-left of the first character on the line, or if there isn't
				a char, on the top-left of the line bounding rect.
********************************************************************************************/

DocCoord TextLine::GetTinyBlobPos()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// set a fall back position - the top left of the line bounds
	DocRect TempRect=GetBoundingRect();
	DocCoord BlobPos=DocCoord(TempRect.lo.x,TempRect.hi.y);

	// but really we want the top left of the first char's metrics rect
	AbstractTextChar* pFirstChar=(AbstractTextChar*)FindFirstChild(CC_RUNTIME_CLASS(AbstractTextChar));
	if (pFirstChar)
	{
		if (pFirstChar->GetMetricsRect(&TempRect))
		{
			BlobPos=DocCoord(TempRect.lo.x,TempRect.hi.y);
			Matrix matrix;
			if (pFirstChar->GetStoryAndCharMatrix(&matrix))
				matrix.transform(&BlobPos);
			else	
				ERROR3("TextLine::GetTinyBlobPos() - GetStoryAndCharMatrix() failed");
		}
		else
			ERROR3("TextLine::GetTinyBlobPos() - GetMetricsRect() failed");
	}

	return BlobPos;
#else
	return DocCoord(0,0);
#endif
}


/*******************************************************************************************
	void TextLine::RenderTinyBlobs(RenderRegion* pRRegion)

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	10/4/95
 	Inputs:		pRegion - points to the RenderRegion to render into
 	Purpose:	Renders the TextLines tiny blobs
********************************************************************************************/

void TextLine::RenderTinyBlobs(RenderRegion* pRRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
   	pRRegion->SetLineColour(COLOUR_NONE);
	pRRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRRegion->DrawBlob(GetTinyBlobPos(), BT_UNSELECTED);
#endif
}


/*******************************************************************************************
	void TextLine::RenderObjectBlobs(RenderRegion* pRRegion)

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	10/4/95
 	Inputs:		pRegion - points to the RenderRegion to render into
 	Purpose:	Renders the TextLines object blobs
********************************************************************************************/

void TextLine::RenderObjectBlobs(RenderRegion* pRRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	RenderTinyBlobs(pRRegion);
#endif
}


/********************************************************************************************
>	UINT32 TextLine::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize
********************************************************************************************/

UINT32 TextLine::GetNodeSize() const 
{
	return (sizeof(TextLine)); 
}


/********************************************************************************************
>	void TextLine::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void TextLine::GetDebugDetails(StringBase* Str)
{
	NodeRenderableBounded::GetDebugDetails(Str); 
}


/********************************************************************************************
>	virtual BOOL TextLine::ReCacheMetrics(FormatRegion* pFormatRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Inputs:		pFormatRegion - 
	Returns:	FALSE if fails
	Purpose:	Recache metrics in text line
********************************************************************************************/

BOOL TextLine::ReCacheMetrics(FormatRegion* pFormatRegion)
{
	TRACEUSER("wuerthne", _T("TextLine::ReCacheMetrics"));
	SetJustification( pFormatRegion->GetJustification());
	SetLineSpacing(   pFormatRegion->GetLineSpacing());
	SetLineSpaceRatio(pFormatRegion->GetLineSpaceRatio());

	SetParaLeftMargin(pFormatRegion->GetLeftMargin());
	SetParaFirstIndent(pFormatRegion->GetFirstIndent());
	SetParaRightMargin(pFormatRegion->GetRightMargin());
	SetRuler(pFormatRegion->GetRuler());
#if defined(_DEBUG) && 0
	String_256 TempStr;
	String Str(_T(" "));
	for (TxtTabStopIterator It = mpRuler->begin(); It != mpRuler->end(); ++It)
	{
		switch((*It).GetType())
		{
			case LeftTab:
				TempStr._MakeMsg( TEXT("L(#1%ld)"), (*It).GetPosition());
				Str += TempStr;
				break;
			case RightTab:
				TempStr._MakeMsg( TEXT("R(#1%ld)"), (*It).GetPosition());
				Str += TempStr;
				break;
			case CentreTab:
				TempStr._MakeMsg( TEXT("C(#1%ld)"), (*It).GetPosition());
				Str += TempStr;
				break;
			case DecimalTab:
				TempStr._MakeMsg( TEXT("D(#1%ld)"), (*It).GetPosition());
				Str += TempStr;
				break;
		}
	}
	TRACEUSER("wuerthne", _T("ruler at %08x:%s"), mpRuler, (TCHAR*)Str);
#endif
	return TRUE;
}

/********************************************************************************************
>	MILLIPOINT TextLine::GetEffectiveLeftMargin()

	Author:		Martin Wuerthner <xara@mw-software.com>
	Created:	18/07/06
	Inputs:		-
	Returns:	the left margin value to use
	Purpose:	Select the left margin or first indent value depending on whether this is
				the first line in the paragraph
********************************************************************************************/

MILLIPOINT TextLine::GetEffectiveLeftMargin()
{
	// we need to find out whether this is the first line in the paragraph, so we know
	// whether we need to use FirstLineIndent or LeftMargin

	TextLine* pPrevLine = FindPrevLine();
	if (pPrevLine==NULL || pPrevLine->FindEOLNode() != NULL)
	{
		// first line in paragraph
		return mFirstIndent;
	}
	else
	{
		return mLeftMargin;
	}
}

/********************************************************************************************
>	BOOL TextLine::Format(TextStoryInfo* pStoryInfo)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		pStoryInfo - current story level info
	Returns:	FALSE if fails
	Purpose:	perform line level formatting
	Note:		on entry pStoryInfo->DescentLine hold the DescentLine of the last line
				(pStoryInfo->DescentLineValid indicating if there was none (eg first line)
				and on exit, it holds the DescentLine for this line and DescentLineValid is TRUE
********************************************************************************************/

BOOL TextLine::Format(TextStoryInfo* pStoryInfo)
{
	ERROR2IF(pStoryInfo==NULL,FALSE,"TextLine::Format() - pStoryInfo == NULL");

	TRACEUSER("wuerthne", _T("TextLine::Format"));
	MILLIPOINT PhysicalLeftMargin   = pStoryInfo->LeftPathIndent;	// 0 if not on path
	MILLIPOINT PhysicalRightMargin  = pStoryInfo->StoryWidth - pStoryInfo->RightPathIndent;
	BOOL       WordWrapping = pStoryInfo->WordWrapping;
	MILLIPOINT RightMargin = PhysicalRightMargin - mRightMargin;
	MILLIPOINT LeftIndent = GetEffectiveLeftMargin();

	// if word wrapping, and not text at a point, and undoably 'do'ing op, word wrap the line
	MILLIPOINT WrapWidth = 0;
	if (WordWrapping)
		WrapWidth = RightMargin - PhysicalLeftMargin;	// will be 0 if text at a point
	if (WrapWidth!=0 && pStoryInfo->WordWrap)
	{
		if (!this->Wrap(pStoryInfo->pUndoOp, WrapWidth, LeftIndent))
			return FALSE;
	}
	else if (WrapWidth != 0)
	{
		// when called during undo (i.e., WordWrap = FALSE) and the story is word wrapping,
		// we do not want to wrap, but we still need to make sure that each tab gets its width
		// set correctly, otherwise PositionCharsInLine will not do the right thing
		FindBreakChar(WrapWidth, FALSE, LeftIndent);
	}

	// if line affected in any way, reposition chars in line
	if (NodeOrDescendantAffected())
	{
		TextLineInfo LineInfo;
		if (ReCalcLineInfo(&LineInfo)==FALSE)
			return FALSE;

		LineInfo.LeftMargin    = PhysicalLeftMargin;
		LineInfo.RightMargin   = PhysicalRightMargin;
		LineInfo.WordWrapping  = WordWrapping;
		if (PositionCharsInLine(&LineInfo)==FALSE)
			return FALSE;
	}

	// calculate this line's BaseLine, and keep the DescentLine up to date
	MILLIPOINT BaseLine    = 0;
	MILLIPOINT DescentLine = 0;
	if (CalcBaseAndDescentLine(&BaseLine,&DescentLine,pStoryInfo->DescentLine,pStoryInfo->DescentLineValid)==FALSE)
		return FALSE;
	pStoryInfo->DescentLine      = DescentLine;
	pStoryInfo->DescentLineValid = TRUE;

	// if the line has moved, set its' new position and flag it is 'affected'
	if (BaseLine!=GetPosInStory())
	{
		SetPosInStory(BaseLine);
		FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected();
	}

	// if line or descendent 'Affected' recalc matrices for affected children
	BOOL ok=TRUE;
	if (NodeOrDescendantAffected())
	{
		if (pStoryInfo->pPath==NULL)
			ok = SetCharMatrices(GetPosInStory());
		else
			ok = FitTextToPath(pStoryInfo,GetPosInStory());
	}

	return ok;
}


/********************************************************************************************
>	BOOL TextLine::EnsureNextLineOfParagraphHasSameLineLevelAttrs(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/7/96
	Inputs:		pUndoOp - 
	Returns:	FALSE if fails
	Purpose:	Ensure the next line of a paragraph has the same line level attibutes
********************************************************************************************/

BOOL TextLine::EnsureNextLineOfParagraphHasSameLineLevelAttrs(UndoableOperation* pUndoOp)
{
	TRACEUSER("wuerthne", _T("EnsureNextLineOfParagraphHasSameLineLevelAttrs"));

	ERROR2IF(FindEOLNode()!=NULL,FALSE,"TextLine::EnsureNextOfParagraphLineHasSameLineLevelAttrs() - there is no next line in the paragraph!");
	TextStory* pStory = this->FindParentStory();
	ERROR2IF(pStory==NULL,FALSE,"TextLine::EnsureNextOfParagraphLineHasSameLineLevelAttrs() - line has no parent story!");
	TextLine* pNextLine = this->FindNextLine();
	ERROR2IF(pNextLine==NULL,FALSE,"TextLine::EnsureNextOfParagraphLineHasSameLineLevelAttrs() - last line of story has no EOL!");

	// flag story not localised, and get the set of line level attrs to localise
	BOOL StoryLocalised = FALSE;
	AttrTypeSet LLASet;
	BOOL    ok = this->AddChildLineLevelAttrsToSet(&LLASet);
	if (ok) ok = pNextLine->AddChildLineLevelAttrsToSet(&LLASet);
	if (!ok) return FALSE;

	// hide any Line Level Attributes found on the next line
	// which are different to those on this line or do not exist on this line
	Node* pNextLineNode = pNextLine->FindFirstChild();
	while (pNextLineNode!=NULL)
	{
		if (pNextLineNode->IsAnAttribute() && ((NodeAttribute*)pNextLineNode)->IsALineLevelAttrib())
		{
			NodeAttribute* pNextLineLLA = (NodeAttribute*)pNextLineNode;
			NodeAttribute* pThisLineLLA = (NodeAttribute*)(this->FindFirstChild(pNextLineLLA->GetAttributeType()));
			if (pThisLineLLA==NULL || !((*pNextLineLLA)==(*pThisLineLLA)) )
			{
				if (!StoryLocalised)
					if (!pStory->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,&LLASet))
						return FALSE;
				StoryLocalised = TRUE;
				if (pUndoOp!=NULL)
				{
					if (!pUndoOp->DoHideNode(pNextLineLLA,FALSE))
						return FALSE;
				}
				else
				{
					pNextLineLLA->CascadeDelete();
					delete pNextLineLLA;
				}
			}
		}
		pNextLineNode = pNextLineNode->FindNext();
	}

	// hide any Line Level Attributes on next line which are different to those found on this line
	// then copy any Line Level Attributes from this line to the next
	// which don't exist (or have just been hiden) on the next line
	Node* pThisLineNode = this->FindFirstChild();
	while (pThisLineNode!=NULL)
	{
		if (pThisLineNode->IsAnAttribute() && ((NodeAttribute*)pThisLineNode)->IsALineLevelAttrib())
		{
			NodeAttribute* pThisLineLLA = (NodeAttribute*)pThisLineNode;
			NodeAttribute* pNextLineLLA = (NodeAttribute*)(pNextLine->FindFirstChild(pThisLineLLA->GetAttributeType()));

			if (pNextLineLLA!=NULL && !((*pNextLineLLA)==(*pThisLineLLA)) )
			{
				if (!StoryLocalised)
					if (!pStory->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,&LLASet))
						return FALSE;
				StoryLocalised = TRUE;
				if (pUndoOp!=NULL)
				{
					if (!pUndoOp->DoHideNode(pNextLineLLA,FALSE))
						return FALSE;
				}
				else
				{
					pNextLineLLA->CascadeDelete();
					delete pNextLineLLA;
				}
				pNextLineLLA = NULL;	// flag its been hidden
			}

			if (pNextLineLLA==NULL)
			{
				if (!StoryLocalised)
					if (!pStory->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,&LLASet))
						return FALSE;
				StoryLocalised = TRUE;
				TRACEUSER("wuerthne", _T("calling SimpleCopy for att %d %d"), IS_A(pThisLineLLA, AttrTxtLineSpace), IS_A(pThisLineLLA, AttrTxtRuler));
				NodeAttribute* pNewLLA = (NodeAttribute*)pThisLineLLA->SimpleCopy();
				TRACEUSER("wuerthne", _T("node copied %d %d"), IS_A(pNewLLA, AttrTxtLineSpace), IS_A(pNewLLA, AttrTxtRuler));
				if (pNewLLA==NULL)
					return FALSE;
				pNewLLA->AttachNode(pNextLine,FIRSTCHILD,TRUE,FALSE); 
				if (pUndoOp!=NULL)
				{
					Action* UndoHideAttrAction;     
					if (HideNodeAction::Init(pUndoOp,pUndoOp->GetUndoActionList(),pNewLLA,TRUE,&UndoHideAttrAction)==AC_FAIL)
						return FALSE; 
				}
			}
		}
		pThisLineNode = pThisLineNode->FindNext();
	}

	// if attrs localised on next line, factor out (globally) and flag line affected
	if (StoryLocalised)
	{
		if (!pStory->DoFactorOutCommonChildAttributes(pUndoOp,TRUE,&LLASet))
			return FALSE;
		pNextLine->FlagNodeAndDescendantsModifiedByOpAndParentsHaveDescendantModifiedByOp();
	}

	return TRUE;
}


/********************************************************************************************
>	TextLine* TextLine::FindFirstLineOfParagraph()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/96
	Returns:	ptr to first line of paragraph in which this line is to be found
				(or NULL if error)
********************************************************************************************/

TextLine* TextLine::FindFirstLineOfParagraph()
{
	TextLine* pLine = this;
	while (1)
	{
		TextLine* pPrevLine = pLine->FindPrevLine();
		if (pPrevLine==NULL || pPrevLine->FindEOLNode()!=NULL)
			break;
		pLine = pPrevLine;
	}
	return pLine;
}


/********************************************************************************************
>	static BOOL TextLine::IsAttrTypeLineLevel(CCRuntimeClass* pAttrType)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/7/96
	Inputs:		pAttrType - 
	Returns:	TRUE if attr type is a line level attribute
********************************************************************************************/

BOOL TextLine::IsAttrTypeLineLevel(CCRuntimeClass* pAttrType)
{
	return pAttrType==CC_RUNTIME_CLASS(AttrTxtJustification)
		|| pAttrType==CC_RUNTIME_CLASS(AttrTxtLineSpace)
		|| pAttrType==CC_RUNTIME_CLASS(AttrTxtLeftMargin)
		|| pAttrType==CC_RUNTIME_CLASS(AttrTxtRightMargin)
		|| pAttrType==CC_RUNTIME_CLASS(AttrTxtFirstIndent)
		|| pAttrType==CC_RUNTIME_CLASS(AttrTxtRuler);
}


/********************************************************************************************
>	BOOL TextLine::AddChildLineLevelAttrsToSet(AttrTypeSet* pAttrTypeSet)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/96
	Inputs:		pAttrTypeSet - 
	Outputs:	pAttrTypeSet - 
	Returns:	FALSE if fails
********************************************************************************************/

BOOL TextLine::AddChildLineLevelAttrsToSet(AttrTypeSet* pAttrTypeSet)
{
	Node* pNode = FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsAnAttribute() && ((NodeAttribute*)pNode)->IsALineLevelAttrib())
			if (!pAttrTypeSet->AddToSet(((NodeAttribute*)pNode)->GetAttributeType()))
				return FALSE;
		pNode = pNode->FindNext();
	}
	return TRUE;
}

/********************************************************************************************
>	BOOL FormatState::FinishTabSection(VisibleTextNode* pLastFormattedNode, BOOL IsLastTabSection)

	Author:		Martin Wuerthner <xara@mw-software.com>			
	Created:	23/06/06
	Inputs:		pLastFormattedNode - the most recently fitted node
				IsLastTabSection - TRUE if this section has been terminated by EOL or a
				line break (i.e., not by a Tab), which means we should add
				ExtraSpaceOnChars/Space here)
	Returns:	ptr to char to break at (or NULL if ERROR)
	Purpose:	This routine is called when a tab or the end of the line is encountered,
				so the text after the previous tab (or the start of the line) can be
				finally formatted.
				The Width/Advance of the Tab is always set and if SetCharPositions is TRUE,
				then all the character positions are set.
********************************************************************************************/

BOOL FormatState::FinishTabSection(VisibleTextNode* pLastFormattedNode, BOOL IsLastTabSection)
{
	// first, set the Tab character's width - this is only necessary for non-Left tabs
	// (for left tabs we have already set it when encountering the tab)
	if (ActiveTabType != LeftTab)
	{
		// pLastTabVTN can only be undefined for ActiveTabType == LeftTab
		((HorizontalTab*)pLastTabVTN)->SetCharWidth(RemainingSpace);
		((HorizontalTab*)pLastTabVTN)->SetCharAdvance(RemainingSpace);
	}
	if (!SetCharPositions) return TRUE;

	// full formatting desired
	VisibleTextNode* pVTN = pLastTabVTN ? pLastTabVTN : pFirstVTN;
	MILLIPOINT Pos = AnchorPos;
	do
	{
		ERROR2IF(pVTN == NULL, FALSE, "FinishTabSection - unexpected end of line");
		if (Pos + CharPosOffset != pVTN->GetPosInLine())
		{
			pVTN->SetPosInLine(Pos + CharPosOffset);
			pVTN->FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected();	
		}
		Pos += pVTN->GetCharAdvance();
		
		if (IsLastTabSection && IS_A(pVTN,TextChar))
			Pos += ExtraOnChars;
		if (IsLastTabSection && pVTN->IsASpace())
			Pos += ExtraOnSpaces;
		if (pVTN == pLastFormattedNode) break;
		pVTN = pVTN->FindNextVTNInLine();
	} while(1);
	return TRUE;
}

/********************************************************************************************
>	void FormatState::AdvanceBy(MILLIPOINT CharWidth)

	Author:		Martin Wuerthner <xara@mw-software.com>			
	Created:	23/06/06
	Inputs:		CharWidth - the amount to advance by
				IsADecimalPoint - TRUE if the currently active tab is a decimal tab and the
								  character to be formatted is its decimal point character
	Purpose:	Account for a char with width CharWidth to be fitted into the current line.
	See also:   TextLine::FindBreakChar for a description of the formatting algorithm used
********************************************************************************************/

void FormatState::AdvanceBy(MILLIPOINT CharWidth, BOOL IsADecimalPoint)
{
	// Note: This routine may only update Width and RemainingSpace, otherwise
	// the backtracking in IsAvailable() does not work as expected.
	if (ActiveTabType == CentreTab && RemainingSpace > 0)
	{
		MILLIPOINT SpaceToLeftUsed = 0;
		// half the width is distributed into RemainingSpace, if it is big enough
		if (RemainingSpace >= CharWidth / 2)
		{
			// half the width fully fits into RemainingSpace
			SpaceToLeftUsed = CharWidth / 2;
		}
		else
		{
			// does not fit, so use up all of RemainingSpace
			SpaceToLeftUsed = RemainingSpace;
		}
		// distribute some of the character to the left
		RemainingSpace -= SpaceToLeftUsed;
		// and the remainder to the right
		Width += CharWidth - SpaceToLeftUsed;
	}
	else if ((ActiveTabType == RightTab || (ActiveTabType == DecimalTab && !DecimalPointFound
											&& !IsADecimalPoint))
			 && RemainingSpace > 0)
	{
		if (RemainingSpace >= CharWidth) RemainingSpace -= CharWidth;
		else
		{
			Width += CharWidth - RemainingSpace;
			RemainingSpace = 0;
		}
	}
	else
	{
		// We end up here if:
		// - the last tab was left tab, or
		// - the last tab was a decimal tab and we have already found the decimal point, or
		// - the last tab was a decimal tab and this is a decimal point character (NB. - the
		//   decimal point is always formatted to the right of the tab position), or, finally,
		// - this is any kind of tab and we have no more remaining space to the left of it
		// In all these cases, we simply format the character to the right of the current position,
		// which is the easiest thing to do anyway
		Width += CharWidth;
	}
}

/********************************************************************************************
>	BOOL FormatState::IsAvailable(MILLIPOINT CharWidth, BOOL IsATab)

	Author:		Martin Wuerthner <xara@mw-software.com>			
	Created:	23/06/06
	Inputs:		CharWidth - the amount to advance by
				IsATab - TRUE if the character to be fitted is a tab
				IsADecimalPoint - TRUE if the currently active tab is a decimal tab and the
								  character to be formatted is its decimal point character
	Purpose:	Test whether a char with width CharWidth can be fitted into the current line.
********************************************************************************************/

BOOL FormatState::IsAvailable(MILLIPOINT CharWidth, BOOL IsATab, BOOL IsADecimalPoint)
{
	// If the full CharWidth still fits, then we can return TRUE straight away
	// without having to look at the active tab type
	if (Width + CharWidth < FitWidth) return TRUE;
	// If the above did not succeed there is no hope in the standard case (last
	// tab was left tab or the beginning of the line) - the same is true for a
	// tab character because it will start a new tab section, so none of its width
	// can go to the left of the current formatting position
	if (ActiveTabType == LeftTab || IsATab) return FALSE;

	// For other tab types, things are a bit more complicated because half or
	// all of the character can go to the left of the tab stop. Rather than
	// duplicating all the AdvanceBy code, we simply advance and backtrack.
	MILLIPOINT OldWidth = Width;
	MILLIPOINT OldRemainingSpace = RemainingSpace;
	AdvanceBy(CharWidth, IsADecimalPoint);
	MILLIPOINT NewWidth = Width;
	Width = OldWidth;
	RemainingSpace = OldRemainingSpace;
	return NewWidth < FitWidth;
}

/********************************************************************************************
>	VisibleTextNode* TextLine::FindBreakChar(MILLIPOINT FitWidth, BOOL SetCharPositions,
										 MILLIPOINT Indent, MILLIPOINT CharPosOffset,
										 MILLIPOINT ExtraOnChars, MILLIPOINT ExtraOnSpaces)

	Author:		Martin Wuerthner <xara@mw-software.com>
				(based on routine by Ed_Cornes <camelotdev@xara.com> created 15/7/96)
	Created:	23/06/06
	Inputs:		FitWidth - the overall width available
				SetCharPositions - TRUE if character positions should be set
				Indent = starting position of first character (logical, counts as far as tab
						 stop positions are concerned)
                CharPosOffset = positional offset to be added to each character, transparent
						 as far as tab stop positions are concerned
				ExtraOnChars, ExtraOnSpaces - see PositionCharsInLine
				The last four are only used if SetCharPositions = TRUE
	Returns:	ptr to char to break at (or NULL if ERROR)
	Purpose:	Find words to fit given width, absorbing spaces, optionally formats line
********************************************************************************************/

VisibleTextNode* TextLine::FindBreakChar(MILLIPOINT FitWidth, BOOL SetCharPositions,
										 MILLIPOINT Indent, MILLIPOINT CharPosOffset,
										 MILLIPOINT ExtraOnChars, MILLIPOINT ExtraOnSpaces)
{
	// This routine does not just find the break char (SetCharPositions = FALSE), it
	// also formats lines by setting the character positions (SetCharPositions = TRUE).
	// With the introduction of Tabs, formatting has become more complex, so it makes
	// a lot of sense to have 
	// depending on whether 
	// Finding the break char is straightforward when no tabs are involved.
	// You just add up the widths and remember the last space character and
	// return that when the available width has been exceeded.
	//
	// With tabs, things become a bit more complex. Left tabs are easy: A left tab
	// is treated like a character with the width of the remaining space up to the
	// tab stop. When dealing with centre or right tabs we need to remember how much
	// space we had left to the left of the tab stop. Half (for centre tabs) or all
	// (for right tabs) of the width of all subsequent text goes to the left until
	// that space is filled up, all remaining space goes to the right only.
	// Decimal tabs behave like centre tabs except that distributing half the width
	// to the left stops as soon as the decimal point has been seen.
	//
	// The formatting algorithm below divides the line into sections delimited by
	// tabs or end of line. When a section is finished (i.e., when seeing a tab or
	// end of line), then the complete section is formatted (using FinishTabSection()).

	TRACEUSER("wuerthne", _T("FindBreakChar, SetCharPositions = %d"), SetCharPositions);
	VisibleTextNode* pBreakChar = NULL;
	BOOL             CharOnLine = FALSE;
	VisibleTextNode* pPrevVTN   = NULL;
	VisibleTextNode* pVTN       = this->FindFirstVTN();

	// create a FormatState object that takes care of the formatting
	FormatState State(FitWidth, SetCharPositions, pVTN, Indent, CharPosOffset, ExtraOnChars, ExtraOnSpaces);

	while(pVTN!=NULL)
	{
		BOOL IsATab = IS_A(pVTN, HorizontalTab);
		BOOL IsADecimalPoint = FALSE;
		if (State.ActiveTabType == DecimalTab && pVTN->IsATextChar())
		{
			// if the currently active tab is a decimal tab check whether we have hit a decimal point
			// NB. - the decimal point character is stored in the tab stop
			IsADecimalPoint = (((TextChar*)pVTN)->GetUnicodeValue() == State.DecimalPointChar);
		}
		if (IsATab)
		{
			TxtTabType ThisTabType;
			MILLIPOINT ThisTabPos;
			MILLIPOINT TabWidth;

			// finish the previous tab
			State.FinishTabSection(pVTN, FALSE);

			// find the next tab stop (always returns usable values - if there are no more
			// tab stops on the ruler the routine assumes left tabs at equidistant positions)
			WCHAR DecimalPointChar;
			TxtRulerAttribute::FindTabStopInRuler(mpRuler, State.Width, &ThisTabType, &ThisTabPos,
												  &DecimalPointChar);
			TabWidth = ThisTabPos - State.Width;
			((HorizontalTab*)pVTN)->SetCharAdvance(TabWidth);
			((HorizontalTab*)pVTN)->SetCharWidth(TabWidth);
			State.AnchorPos = State.Width;
			if (ThisTabType != LeftTab)
			{
				// Space between the tab character and the stop it aligns to
				State.RemainingSpace = TabWidth;
			}
			if (ThisTabType == DecimalTab)
			{
				State.DecimalPointChar = DecimalPointChar;
				State.DecimalPointFound = FALSE;
			}
			State.pLastTabVTN = pVTN;
			State.ActiveTabType = ThisTabType;
			State.ActiveTabPos  = ThisTabPos;
			// we allow line breaks at tabs
			pBreakChar  = pPrevVTN;
		}
		if (!pVTN->IsACaret())
		{
			if (pVTN->IsAnEOLNode())
			{
				State.FinishTabSection(pVTN, TRUE);
				return pVTN;
			}
			else if (pVTN->IsASpace())
			{
				State.AdvanceBy(pVTN->GetCharAdvance(), IsADecimalPoint);
				pBreakChar  = pVTN;
			}
			else if (!CharOnLine || State.IsAvailable(pVTN->GetCharWidth(), IsATab, IsADecimalPoint))
			{
				if (IsATab) State.Width += pVTN->GetCharAdvance();
				else State.AdvanceBy(pVTN->GetCharAdvance(), IsADecimalPoint);
				if (pVTN->IsAHyphen())
					pBreakChar = pVTN;
				if (State.ActiveTabType == DecimalTab && !State.DecimalPointFound && IsADecimalPoint)
					State.DecimalPointFound = TRUE;
			}
			else
			{
				// did not fit
				if (pBreakChar==NULL)
					pBreakChar = pPrevVTN;
				if (pBreakChar) State.FinishTabSection(pBreakChar, TRUE);
				return pBreakChar;
			}
			CharOnLine = TRUE;
		}
		pPrevVTN = pVTN;
		if (SetCharPositions)
		{
			// formatting run - we stay within our line and finish when we run past the end
			pVTN = pVTN->FindNextVTNInLine();
			if (!pVTN)
			{
				State.FinishTabSection(pPrevVTN, TRUE);
				return NULL;
			}
		}
		else
		{
			pVTN = pVTN->FindNextVTNInStory();
		}
	}
	ERROR2(FALSE,"FindBreakChar() - story has no final EOL!");
}


/********************************************************************************************
>	BOOL TextLine::Wrap(UndoableOperation* pUndoOp, MILLIPOINT WrapWidth)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/7/96
	Inputs:		pUndoOp   - 
				WrapWidth - width into which to format
				LeftIndent - the left indent
	Returns:	FALSE if fails
	Purpose:	Word wrap the line
********************************************************************************************/

BOOL TextLine::Wrap(UndoableOperation* pUndoOp, MILLIPOINT WrapWidth, MILLIPOINT Indent)
{
	VisibleTextNode* pBreakChar = this->FindBreakChar(WrapWidth, FALSE, Indent);
	if (pBreakChar==NULL)
		return FALSE;

	// if break char on this line, wrap the rest of the line forward
	// else wrap all lines and chars upto the break char back on this line
	TextLine* pBreakLine = pBreakChar->FindParentLine();
	ERROR2IF(pBreakLine==NULL,FALSE,"TextLine::Wrap() - break char has no parent line");
	if (pBreakLine==this)
	{
		VisibleTextNode* pAfterBreak = pBreakChar->FindNextVTNInLine();
		if (pAfterBreak!=NULL)
			if (!pAfterBreak->WrapRestOfLineForward(pUndoOp))
				return FALSE;
	}
	else
	{
		TextLine* pNextLine = this->FindNextLine();
		while (pNextLine!=pBreakLine)
		{
			ERROR2IF(pNextLine==NULL,FALSE,"TextLine::Wrap() - did not find line with break char!");
			VisibleTextNode* pLastVTN = pNextLine->FindLastVTN();
			ERROR2IF(pLastVTN==NULL,FALSE,"TextLine::Wrap() - no VTN on line!");
			if (!pLastVTN->WrapFromStartOfLineBack(pUndoOp))
				return FALSE;
			pNextLine = this->FindNextLine();
		}
		if (!pBreakChar->WrapFromStartOfLineBack(pUndoOp))
			return FALSE;
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL TextLine::ReCalcLineInfo(TextLineInfo* pLineInfo)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/96
	Outputs:	pLineInfo - line level info
	Returns:	FALSE if fails
	Purpose:	Recalculate line level info (absorbing space at end of line)
	Note:		Also updates line level info in node itself
********************************************************************************************/

BOOL TextLine::ReCalcLineInfo(TextLineInfo* pLineInfo)
{
	ERROR2IF(pLineInfo==NULL,FALSE,"TextLine::ReCalcLineInfo() - pLineInfo==NULL");

	TRACEUSER("wuerthne", _T("RecalcLineInfo"));
	// reset info in node to be calculated
	SetLineAscent(0);
	SetLineDescent(0);
	SetLineSize(0);

	// init local info to be calculated
	MILLIPOINT SumCharAdvances = 0;
	INT32       NumChars        = 0;
	INT32       NumSpaces       = 0;
	MILLIPOINT SumCharAdvancesToLastNonSpace = 0;
	INT32       NumCharsToLastNonSpace        = 0;
	INT32       NumSpacesToLastNonSpace       = 0;

	// for each AbstractTextChar on line ...
	BOOL AbstractTextCharFound = FALSE;
	Node* pNode = FindFirstChild();
	ERROR3IF(pNode==NULL,"TextLine::ReCalcLineInfo() - line has no children!");
	while (pNode!=NULL)
	{
		if (pNode->IsAnAbstractTextChar())
		{
			AbstractTextChar* pATC= (AbstractTextChar*)pNode;

			// update info in TextLine (but only use EOL if no chars on line)
			if (!pATC->IsAnEOLNode() || !AbstractTextCharFound)
			{
				UpdateLineAscent( pATC->GetFontAscent());
				UpdateLineDescent(pATC->GetFontDescent());
				UpdateLineSize(   pATC->GetFontSize());
			}

			// update local info
			if (IS_A(pATC,TextChar))
				NumChars  += 1;
			if (IS_A(pATC, HorizontalTab))
			{
				// only spaces/chars after the last tab can be stretched for full justification
				TRACEUSER("wuerthne", _T("tab at %08x width=%d"), pATC, pATC->GetCharAdvance());
				NumSpaces = 0;
				NumChars = 0;
			}
			if (pATC->IsASpace())
				NumSpaces += 1;
			else if (!pATC->IsAnEOLNode())
			{
				NumCharsToLastNonSpace        = NumChars;
				NumSpacesToLastNonSpace       = NumSpaces;
				SumCharAdvancesToLastNonSpace = SumCharAdvances + pATC->GetCharWidth();	// exc tracking of this char
			}
			SumCharAdvances += pATC->GetCharAdvance();

			AbstractTextCharFound = TRUE;
		}
		pNode = pNode->FindNext();
	}
	ERROR2IF(!AbstractTextCharFound,FALSE,"TextLine::ReCalcLineInfo() - line has no AbstractTextChars");

	// set outputs
	pLineInfo->SumCharAdvances = SumCharAdvancesToLastNonSpace;
	pLineInfo->NumChars        = NumCharsToLastNonSpace;
	pLineInfo->NumSpaces       = NumSpacesToLastNonSpace;
	pLineInfo->justification   = GetJustification();
	pLineInfo->ParaLeftMargin  = GetEffectiveLeftMargin();
	pLineInfo->ParaRightMargin = GetParaRightMargin();
	pLineInfo->Ruler           = GetRuler();
	return TRUE;
}


/********************************************************************************************
>	BOOL TextLine::PositionCharsInLine(TextLineInfo* pLineInfo)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		pLineInfo - required info about the line
	Returns:	FALSE if fails
	Purpose:	set the position of each char on the line accounting for justification
				ALSO, flag any chars that have moved as 'affected'
********************************************************************************************/

BOOL TextLine::PositionCharsInLine(TextLineInfo* pLineInfo)
{
	ERROR2IF(pLineInfo==NULL,FALSE,"TextLine::PositionCharsInLine() - pLineInfo==NULL");
	// we have two different left/right margins: margin properties of the complete story
	// (only for text on path) and margins applied as attributes to paragraphs
	MILLIPOINT PhysicalLeftMargin = pLineInfo->LeftMargin;        // story property
	MILLIPOINT ParagraphLeftMargin = pLineInfo->ParaLeftMargin;   // paragraph attribute
	MILLIPOINT LeftMargin      = PhysicalLeftMargin + ParagraphLeftMargin;

	MILLIPOINT PhysicalRightMargin = pLineInfo->RightMargin;
	MILLIPOINT ParagraphRightMargin = pLineInfo->ParaRightMargin;
	MILLIPOINT RightMargin = PhysicalRightMargin - ParagraphRightMargin;

	MILLIPOINT SumCharAdvances = pLineInfo->SumCharAdvances;
	BOOL       WordWrapping    = pLineInfo->WordWrapping;
	MILLIPOINT JustifyWidth    = RightMargin - LeftMargin;

	TRACEUSER("wuerthne", _T("PositionCharsInLine, Sum=%d, JWidth=%d"), SumCharAdvances, JustifyWidth);

	// if word wrapping, fully justified, text does not fill line and end of paragraph
	// left justified
	Justification justification = pLineInfo->justification;
	if (WordWrapping && justification==JFULL && SumCharAdvances < JustifyWidth && FindEOLNode()!=NULL)
		justification = JLEFT;

	// calc amount by which chars need to be adjusted (default to left justify)
	MILLIPOINT CharPosOffset = PhysicalLeftMargin;
	MILLIPOINT ExtraOnSpaces = 0;
	MILLIPOINT ExtraOnChars  = 0;
	switch (justification)
	{
		// for both right and centre justification we subtract ParagraphLeftMargin because this
		// will be added in again later
		case JRIGHT:
			CharPosOffset = RightMargin - SumCharAdvances - ParagraphLeftMargin;
			break;
		case JCENTRE:
			CharPosOffset = (LeftMargin + RightMargin - SumCharAdvances)/2 - ParagraphLeftMargin;
			break;
		case JFULL:
			if (JustifyWidth!=0)
			{
				// if text does not fill line, expand spaces so it does,
				// but if no spaces, and word wrapping, increase inter-char spacing so word fills line
				// if text overflows line, reduce inter-char spacing so words fit line
				INT32 gap       = JustifyWidth - SumCharAdvances;
				INT32 NumChars  = pLineInfo->NumChars;
				INT32 NumSpaces = pLineInfo->NumSpaces;
				if (gap>0)
				{
					if (NumSpaces>0)
						ExtraOnSpaces = gap/NumSpaces;
					else
						if (WordWrapping && NumChars>1)
							ExtraOnChars = gap/(NumChars-1);
				}
				else
					if (pLineInfo->NumChars>1)
						ExtraOnChars = gap/(NumChars-1);
			}
			break;
		default:
			break;
	}

	// run through line setting all char's 'PosInLine' (flagging those which moved)
	TRACEUSER("wuerthne", _T("calling FindBreakChar, Indent=%d, Offset=%d, ExtraC=%d, ExtraS=%d"),
			  ParagraphLeftMargin, CharPosOffset, ExtraOnChars, ExtraOnSpaces);
	FindBreakChar(INT32_MAX, TRUE, ParagraphLeftMargin, CharPosOffset, ExtraOnChars, ExtraOnSpaces);
	return TRUE;
}


/********************************************************************************************
>	BOOL TextLine::CalcBaseAndDescentLine(MILLIPOINT* pBaseLine, MILLIPOINT* pDescentLine,
										MILLIPOINT LastDescentLine, BOOL LastDescentLineValid)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/96
	Inputs:		LastDescentLine      - DescentLine of last line
				LastDescentLineValid - FALSE if first line in story
	Outputs:	pBaseLine    - base line of this line
				pDescentLine - base line of this line
	Returns:	FALSE if fails
	Purpose:	Calculate BaseLine and DescentLine for this line given the last line's DescentLine
				if this is the first line in the story,
				the baseline is 0 and the DescentLine has to be back calculated
********************************************************************************************/

BOOL TextLine::CalcBaseAndDescentLine(MILLIPOINT* pBaseLine, MILLIPOINT* pDescentLine,
									MILLIPOINT LastDescentLine, BOOL LastDescentLineValid)
{
	// get some info about the line ...
	MILLIPOINT LineAscent  = GetLineAscent();
	MILLIPOINT LineDescent = GetLineDescent();
	MILLIPOINT LineHeight  = LineAscent-LineDescent;
	MILLIPOINT LineSpacing = GetLineSpacing();

	MILLIPOINT BaseLine    = 0;
	MILLIPOINT DescentLine = 0;
	if (LineSpacing!=0)
	{
		MILLIPOINT OffsetFromDescentLineToBaseLine = MulDiv(LineSpacing, -LineDescent, LineHeight);
		if (LastDescentLineValid)
		{
			DescentLine = LastDescentLine - LineSpacing;
			BaseLine    = DescentLine + OffsetFromDescentLineToBaseLine;
		}
		else
			DescentLine = BaseLine - OffsetFromDescentLineToBaseLine;
	}
	else
	{
		FIXED16    LineSpaceRatio     = GetLineSpaceRatio();
		MILLIPOINT AbsolutLineSpacing = MILLIPOINT( XLONG(LineHeight) * LineSpaceRatio );
		MILLIPOINT OffsetFromLastDescentLineToBaseLine = LineAscent;
		if (LineSpaceRatio<1)
			OffsetFromLastDescentLineToBaseLine = XLONG(LineAscent) * LineSpaceRatio;

		if (LastDescentLineValid)
		{
			BaseLine    = LastDescentLine - OffsetFromLastDescentLineToBaseLine;
			DescentLine = LastDescentLine - AbsolutLineSpacing;
		}
		else
			DescentLine = BaseLine + OffsetFromLastDescentLineToBaseLine - AbsolutLineSpacing;
	}

	// set outputs
	*pDescentLine = DescentLine;
	*pBaseLine    = BaseLine;

	return TRUE;
}


/********************************************************************************************
>	BOOL TextLine::SetCharMatrices(MILLIPOINT LinePos)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		LinePos - pos of line in story
	Returns:	FALSE if fails
	Purpose:	set matrices in all VTN on a line
********************************************************************************************/

BOOL TextLine::SetCharMatrices(MILLIPOINT LinePos)
{
	Node* pNode=FindFirstChild();
	while (pNode!=NULL)
	{
		if (pNode->IsAVisibleTextNode())
		{
			VisibleTextNode* pVTN=(VisibleTextNode*)pNode;
			if (pVTN->Affected())
				pVTN->SetMatrix( Matrix( pVTN->GetPosInLine(), LinePos+pVTN->GetBaseLineShift() ) );
		}
		pNode=pNode->FindNext();
	}
	return TRUE;
}


/********************************************************************************************
>	BOOL TextLine::FitTextToPath(TextStoryInfo* pStoryInfo, MILLIPOINT LinePos)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Inputs:		pStoryInfo - cached info associated with path
				LinePos   - pos of line in story
	Returns:	FALSE if fails
	Purpose:	set matrices in all VTN on a line so as to fit text to the path, accounting
				for any transform which was applied before the text was fitted to the path
********************************************************************************************/

BOOL TextLine::FitTextToPath(TextStoryInfo* pStoryInfo, MILLIPOINT LinePos)
{
	ERROR2IF(pStoryInfo==NULL,FALSE,"TextLine::FitTextToPath() - pStoryInfo==NULL");
	TextStory* pTextStory = FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"TextLine::FitTextToPath() - pTextStory==NULL");

	// get values required to account for transforms applied to text before it was fitted to the path
	FIXED16 scale    = pTextStory->GetCharsScale();
	FIXED16 xscale   = (scale<0 ? -scale : scale) * pTextStory->GetCharsAspect();
	FIXED16 shear    = scale * tan(pTextStory->GetCharsShear().MakeDouble());
//	double  rotation = pTextStory->GetCharsRotation().MakeDouble();

	// scale the line width offset
	LinePos = XLONG(LinePos) * scale;
	MILLIPOINT LinePosX = (MILLIPOINT)(LinePos * pStoryInfo->UnitDirectionVectorX + 0.5);
	MILLIPOINT LinePosY = (MILLIPOINT)(LinePos * pStoryInfo->UnitDirectionVectorY + 0.5);

	// if chars tangential, get a pointer to a double to read the angle into
	// else set the angle to 0 and make the pointer NULL (ie don't actually ask for the angle)
	double  TangentAtDist=0;
	double* pTangentAtDist=NULL;
	if (pTextStory->IsTextOnPathTangential())
		pTangentAtDist=&TangentAtDist;

	// some vars used repeatedly while scanning line ...
	BOOL                Found=FALSE;
	DocCoord            CoordAtDist(0,0);
	Matrix              VTNMatrix(0,0);
	ProcessPathDistance PathDistanceProcess(64);

	Node* pNode=FindFirstChild();
	while (pNode)
	{
		if (pNode->IsAVisibleTextNode())
		{
			VisibleTextNode* pVTN=(VisibleTextNode*)pNode;
			if (pVTN->Affected())
			{
				// get distance to centre of char accounting for pre-transform
				MILLIPOINT CharWidthBy2 = XLONG(pVTN->GetCharWidth()/2) * xscale;
				MILLIPOINT Dist=XLONG(pVTN->GetPosInLine()) * xscale + CharWidthBy2;

				if (pStoryInfo->PathClosed)
				{
					Dist = Dist % pStoryInfo->PathLength;
					if (Dist<0)
						Dist += pStoryInfo->PathLength;
				}

				// and thence the coord (and tangent) this distance along the path
				if (PathDistanceProcess.GetCoordAndTangent(&CoordAtDist,pTangentAtDist,&Found,Dist,pStoryInfo->pPath)==FALSE)
					return FALSE;

				// set the char matrix to account for pre-transform and baseline shift,
				VTNMatrix=Matrix( xscale, 0, shear, scale, 0, XLONG(pVTN->GetBaseLineShift()) * scale );
				// then rotate appropriately,
//				VTNMatrix*=Matrix(DocCoord(CharWidthBy2,0),(TangentAtDist+rotation)/PI*180);
				VTNMatrix*=Matrix(DocCoord(CharWidthBy2,0),TangentAtDist/PI*180);
				// then translate to the correct point on the path and account for line spacing
				VTNMatrix.translate(CoordAtDist.x-CharWidthBy2+LinePosX,CoordAtDist.y+LinePosY);
				pVTN->SetMatrix(VTNMatrix);
			}
		}
		pNode=pNode->FindNext();
	}

	return TRUE;
}


/*******************************************************************************************
>	VisibleTextNode* TextLine::FindCharAtDistAlongLine(MILLIPOINT Distance, BOOL* LeftHandSide)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	12/5/95
 	Inputs:		Distance - distance along the line to look for
				LeftHandSide - pointer to a BOOL
	Outputs:	LeftHandSide - TRUE if the point supplied is in the left half of the char
							   FALSE if the point supplied is in the right half of the char
	Returns:	A pointer to the character found (NULL if error)
 	Purpose:	Call this function to get the character at a certian distance along this 
				TextLine.  If the distance is off the right end of the line then a pointer to
				the EOLNode is returned.
********************************************************************************************/

VisibleTextNode* TextLine::FindCharAtDistAlongLine(MILLIPOINT Distance, BOOL* LeftHandSide)
{
	ERROR2IF(LeftHandSide==NULL,NULL,"TextLine::FindCharAtDistAlongLine() - NULL side pointer");

	VisibleTextNode* pVTN = FindFirstVTN();
	ERROR2IF(pVTN==NULL,NULL,"TextLine::FindCharAtDistAlongLine() - has no VTN");
	while (1)
	{
		const MILLIPOINT Left   = pVTN->GetPosInLine();
		const MILLIPOINT Centre = Left+pVTN->GetCharWidth()/2;
		const MILLIPOINT Next   = Left+pVTN->GetCharAdvance();

		// if to left of centre of this char (and to right of previous if any)
		// or at end of line, return char and 'to left'
		if (Distance<=Centre || pVTN->IsAnEOLNode())
		{
			*LeftHandSide = TRUE;
			return pVTN;
		}

		// if to left of next char (and right of centre of this char),
		// return char and 'to right'
		if (Distance<=Next)
		{
			*LeftHandSide = FALSE;
			return pVTN;
		}

		// if no next VTN, return this char and 'to right'
		VisibleTextNode* pNextVTN = pVTN->FindNextVTNInLine();
		if (pNextVTN==NULL)
		{
			*LeftHandSide = FALSE;
			return pVTN;
		}
		pVTN = pNextVTN;
	}
}


/********************************************************************************************
>	BOOL TextLine::LineContainsCaret(CaretNode** pCaret)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Returns:	ptr to caret if line contains caret else NULL
********************************************************************************************/
CaretNode* TextLine::FindCaret() const
{
	return (CaretNode*)FindFirstChild(CC_RUNTIME_CLASS(CaretNode));
}


/********************************************************************************************
>	BOOL TextLine::WholeLineSelected()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Returns:	TRUE if the line itself or all the chars on the line are selected
				(including hard EOL but excluding soft EOL and caret)
********************************************************************************************/

BOOL TextLine::WholeLineSelected()
{
	if (IsSelected()==FALSE)
	{
		VisibleTextNode* pVTN = FindFirstVTN();
		ERROR2IF(pVTN==NULL,FALSE,"TextLine::WholeLineSelected() - line has no VTN!");
		while (pVTN!=NULL)
		{
			if (!pVTN->IsSelected() && !pVTN->IsACaret())
				return FALSE;
			pVTN = pVTN->FindNextVTNInLine();
		}
	}
	return TRUE;
}



/********************************************************************************************
>	TextLine* TextLine::FindNextLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	pointer to next TextLine
********************************************************************************************/
TextLine* TextLine::FindNextLine()  const
{
	return (TextLine*)FindNext(CC_RUNTIME_CLASS(TextLine));
}


/********************************************************************************************
>	TextLine* TextLine::FindPrevLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	pointer to previous TextLine
********************************************************************************************/
TextLine* TextLine::FindPrevLine() const
{
	return (TextLine*)FindPrevious(CC_RUNTIME_CLASS(TextLine));
}


/********************************************************************************************
>	VisibleTextNode* TextLine::FindFirstVTN()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95
	Returns:	A pointer to the first VTN in this line; NULL if there isn't one
	Purpose:	For getting a pointer to the first Visible Text Node in this TextLine
********************************************************************************************/
VisibleTextNode* TextLine::FindFirstVTN() const
{
	return (VisibleTextNode*)FindFirstChild(CC_RUNTIME_CLASS(VisibleTextNode));
}


/********************************************************************************************
>	VisibleTextNode* TextLine::FindLastVTN()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/95
	Returns:	A pointer to the last VTN in this line; NULL if there isn't one
	Purpose:	For getting a pointer to the last Visible Text Node in this TextLine
********************************************************************************************/
VisibleTextNode* TextLine::FindLastVTN() const
{
	return (VisibleTextNode*)FindLastChild(CC_RUNTIME_CLASS(VisibleTextNode));
}


/********************************************************************************************
>	EOLNode* TextLine::FindEOLNode()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/96
	Returns:	ptr to EOL (or NULL if none - error set)
********************************************************************************************/
EOLNode* TextLine::FindEOLNode() const
{
	EOLNode* pEOL = (EOLNode*)FindFirstChild(CC_RUNTIME_CLASS(EOLNode));
	ERROR3IF(pEOL!=NULL && pEOL->FindNext(CC_RUNTIME_CLASS(EOLNode))!=NULL,
		"TextLine::FindEOLNode() - more than one EOLNode on a line!");
	return pEOL;
}


/********************************************************************************************
>	MILLIPOINT TextLine::GetLastCharTracking()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/1/95
	Returns:	Returns size of tracking on last char on line
********************************************************************************************/

MILLIPOINT TextLine::GetLastCharTracking()
{
	AbstractTextChar* pATC = (AbstractTextChar*)FindFirstChild(CC_RUNTIME_CLASS(AbstractTextChar));
	if (pATC->IsAnEOLNode())
		pATC = (AbstractTextChar*)(pATC->FindPrevious(CC_RUNTIME_CLASS(AbstractTextChar)));

	if (pATC==NULL)
		return 0;

	return pATC->GetCharAdvance() - pATC->GetCharWidth();
}


/********************************************************************************************
>	BOOL TextLine::WillLineWrapOnPath(FIXED16 xscale, MILLIPOINT Length)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/95
	Inputs:		pPath   - path to fit text to
				ModLen  - length of the path
	Returns:	TRUE if the text line will wrap on this path
				FALSE if not
	Purpose:	Calculates whether the formatter will wrap this line of text around the path
				It calculates this by checking the position of the last character (or VTN in
				the line). If this wraps it is assumed the whole line will wrap
********************************************************************************************/

BOOL TextLine::WillLineWrapOnPath(FIXED16 xscale, MILLIPOINT PLength)
{
	TextStory* pTextStory = FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"TextLine::IsLineWrappedOnPath() - pTextStory==NULL");

	BOOL found = FALSE;
	Node* pNode=FindLastChild();
	while (pNode && !found)
	{
		if (pNode->IsAVisibleTextNode())
			found = TRUE;
		else
			pNode=pNode->FindPrevious();
	}

	if (pNode)
	{
		VisibleTextNode* pVTN=(VisibleTextNode*)pNode;
		MILLIPOINT Dist=XLONG(pVTN->GetPosInLine()) * xscale;
		if (Dist>PLength)
			return TRUE;
	}
	return FALSE;
}

/********************************************************************************************

>	BOOL TextLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
	BOOL TextLine::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter - filter to save text line node to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Saves a text line record to the new file format filter

********************************************************************************************/

BOOL TextLine::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtLine::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextLine::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtLine::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

//--------------------------------

BOOL TextLine::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtLine::WriteBeginChildRecordsWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextLine::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtLine::WriteBeginChildRecordsNative(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextLine::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtLine::WriteEndChildRecordsWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextLine::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtLine::WriteEndChildRecordsNative(pFilter, this);
#else
	return FALSE;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
// FormatRegion


/********************************************************************************************
>	FormatRegion::FormatRegion()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Purpose:	Initialise member variables of the RenderRegion to keep it happy
********************************************************************************************/

FormatRegion::FormatRegion() : RenderRegion()
{
	CurrentClipRect = DocRect(0,0,0,0);
	ScaledPixelWidth = 1;
}


/********************************************************************************************
>	FormatRegion::~FormatRegion()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Purpose:	Destructor for FormatRegion
********************************************************************************************/

FormatRegion::~FormatRegion()
{
}


/********************************************************************************************
>	BOOL FormatRegion::Init(NodeRenderableInk* pFirstNode)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/03/95
	Inputs:		pFirstNode - pointer to a node whose applied attributes are used to initialise 
							 the FormatRegion's attribute stack
	Returns:	FALSE if fails
	Purpose:	Initialise a FormatRegion
				setting the attribute stack to those attributes applied to the specified node
********************************************************************************************/

BOOL FormatRegion::Init(NodeRenderableInk* pFirstNode)
{
	m_pFormatDC = std::auto_ptr<wxDC>( new wxMemoryDC );
	if( NULL == m_pFormatDC.get() )
		ERROR2(FALSE,"FormatRegion::Init() - CreateCompatibleDC() failed");

	RenderView = DocView::GetSelected();

	// If there is not a selected doc view (e.g. because it is a clipboard document),
	// try the current view item.
	// This should only be an issue when formatting text that's been imported into the clipboard, usually
	// as part of an OLE operation
	if (RenderView == NULL)
	{
		RenderView = View::GetCurrent();
		if (RenderView == NULL)
		{
			ERROR3("No view!! Can't format text without a view");
			return FALSE;
		}

	//	ERROR3IF(RenderView->GetDoc()->IsNotAClipboard(),"No selected doc view yet doc attached to current view is not a clipboard doc.  Is this right?");
	}

	// Initialise the render region
	StartRender();

	// Get the attributes
	if (pFirstNode==NULL)
		return TRUE;

	CCAttrMap		   *pAttribMap = new CCAttrMap(30);
	if (pAttribMap == NULL) return FALSE;

	if (!pFirstNode->FindAppliedAttributes(pAttribMap))
	{
		delete pAttribMap;
		return FALSE;
	}

	CCAttrMap::iterator	pos = pAttribMap->GetStartPosition();
	CCAttrMap::iterator	end = pAttribMap->GetEndPosition();
	while( pos != end )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		pAttribMap->GetNextAssoc(pos, pKey, pVal);

		((NodeAttribute*) pVal)->Render(this);
	}

	delete pAttribMap;

	return TRUE;
}


/********************************************************************************************
>	BOOL FormatRegion::GetCharMetrics(CharMetrics* pCharMetrics, WCHAR ch)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/96
	Inputs:		ch           - the character (kern codes should ask for a FONTEMCHAR)
	Outputs:	pCharMetrics - metrics (CharWidth, FontAscent, FontDescent, FontEmWidth)
	Returns:	FALSE if fails
	Purpose:	Get metrics for a given char using the current attributes of the format region
********************************************************************************************/

BOOL FormatRegion::GetCharMetrics(CharMetrics* pCharMetrics, WCHAR ch)
{
	ERROR2IF(pCharMetrics==NULL, FALSE, "FormatRegion::GetCharMetrics() - pCharMetrics==NULL");

	// get the char metrics for the specified character
	wxDC			   *pDC = m_pFormatDC.get();
	CharDescription		FontDesc( FONTEMCHAR, RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC() );
	if( FONTMANAGER->GetCharMetrics( pDC, ch, FontDesc, pCharMetrics ) == FALSE )
		return FALSE;

	// get x/y scale factors and apply then to the default metrics found
	// Note: y scale factor does not include script ratio so does not affect ascent/descent
	//       ie a line of subscript text has the same line ascents/descents as a non-subscript line
	TxtScriptAttribute* pScriptAttr=RR_TXTSCRIPT();
	ERROR2IF(pScriptAttr==NULL,FALSE,"RenderRegion::GetCharAttributeMatrix() - pScriptAttr==NULL");
	double ScaleY = (double)RR_TXTFONTSIZE() / TextManager::GetDefaultHeight();
	double ScaleX = ScaleY * RR_TXTASPECTRATIO().MakeDouble() * pScriptAttr->Size.MakeDouble();
	pCharMetrics->Scale(ScaleX,ScaleY);

	return TRUE;
}


/********************************************************************************************
>	MILLIPOINT FormatRegion::GetCharsKerning(WCHAR chLeft, WCHAR chRight)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/10/2000
	Inputs:		chLeft		- the left char of a kern pair
				chRight		- the right char of a kern pair
	Returns:	Kern distance between chars or 0 on error or no kern
	Purpose:	Finds the kerning distance between two chars
********************************************************************************************/

MILLIPOINT FormatRegion::GetCharsKerning(WCHAR chLeft, WCHAR chRight)
{
	// get the char metrics for the specified character
	wxDC			   *pDC = m_pFormatDC.get();
	CharDescription		FontDesc(FONTEMCHAR, RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC());
	MILLIPOINT			kern = FONTMANAGER->GetCharsKerning( pDC, chLeft, chRight, FontDesc );

	// get x/y scale factors and apply then to the default metrics found
	// Note: y scale factor does not include script ratio so does not affect ascent/descent
	//       ie a line of subscript text has the same line ascents/descents as a non-subscript line
	TxtScriptAttribute* pScriptAttr=RR_TXTSCRIPT();
	ERROR2IF(pScriptAttr==NULL,FALSE,"RenderRegion::GetCharAttributeMatrix() - pScriptAttr==NULL");
	double ScaleY = (double)RR_TXTFONTSIZE() / TextManager::GetDefaultHeight();
	double ScaleX = ScaleY * RR_TXTASPECTRATIO().MakeDouble() * pScriptAttr->Size.MakeDouble();

	if (kern)
		kern = (MILLIPOINT)(kern * ScaleX + 0.5);

	return kern;
}
