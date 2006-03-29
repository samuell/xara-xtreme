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

// This file contains the implementation of all Text object Nodes
// A TextObject is composed of the following nodes
// TextStory ( The root of the text object subtree )
// 		NodeAttributes (Many or none)
//		NodePath 
//		TextLine (Many or one)
//			NodeAttributes (Many or none)
//			TextChar
// See TEXTTLDS.DOC	for a description of the structure of the text object data structure


#include "camtypes.h"
#include "nodetext.h"

// Code headers
#include "app.h"
#include "becomea.h"
#include "blobs.h"
//#include "cameleps.h"
#include "cliptype.h"
#include "docview.h"
#include "gdraw.h"
#include "grndrgn.h"
#include "lineattr.h"
#include "nodetxtl.h"
#include "nodetxts.h"
#include "snap.h"
#include "spread.h"
#include "texttool.h"
#include "textops.h"
#include "attrmap.h"
#include "printctl.h"
#include "cxftext.h"
//#include "swfrndr.h"

// Resource headers
//#include "mario.h"
//#include "peter.h"
//#include "simon.h"

#include "webattr.h"			//For the GetClickableRectangle functions
#include "brshattr.h"

//#include "ai_epsrr.h"

DECLARE_SOURCE("$Revision: 662 $")

CC_IMPLEMENT_DYNAMIC(VisibleTextNode, BaseTextClass)
CC_IMPLEMENT_DYNAMIC(CaretNode, VisibleTextNode)
CC_IMPLEMENT_DYNAMIC(AbstractTextChar, VisibleTextNode)
CC_IMPLEMENT_DYNAMIC(EOLNode,  AbstractTextChar)
CC_IMPLEMENT_DYNAMIC(TextChar, AbstractTextChar)
CC_IMPLEMENT_DYNAMIC(KernCode, AbstractTextChar)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


/********************************************************************************************
>	VisibleTextNode::VisibleTextNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple VisibleTextNode constructor, it is required so that SimpleCopy will work.
********************************************************************************************/

VisibleTextNode::VisibleTextNode(): BaseTextClass() // Call the base class
{
	Init();
}

 
/********************************************************************************************
>	VisibleTextNode::VisibleTextNode(Node* ContextNode, AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.     
		
				Direction: 
			
				Specifies the direction in which the node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node  
				
	Purpose:	The main VisibleTextNode constructor
********************************************************************************************/

VisibleTextNode::VisibleTextNode(Node* ContextNode,  
		 			 			 AttachNodeDirection Direction): BaseTextClass(ContextNode, Direction)
{
	Init();
}


/********************************************************************************************
>	void VisibleTextNode::Init()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/3/95
	Purpose:	Initialise the members of a VisibleTextNode.  Called by all constructors.
********************************************************************************************/

void VisibleTextNode::Init()
{
	CharMatrix          = Matrix(0,0);
	PosInLine           = 0;
	IsBoundingRectValid = FALSE;
}


/********************************************************************************************
>	VisibleTextNode* VisibleTextNode::FindNextVTNInStory()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/95
	Returns:	A pointer to the next VisibleTextNode in the TextStory
	Purpose:	Finds the next VisibleTextNode in the TextStory. The routine searches
				across TextLines.   
	Errors:		An ERROR3 will be caused if the node is not on a line which is the child
				of a TextStory.
********************************************************************************************/

VisibleTextNode* VisibleTextNode::FindNextVTNInStory()
{
	Node* Line = FindParent(); 

	// Be cautious
	ERROR3IF(Line == NULL, "VisibleTextNode has no parent"); 
	ERROR3IF(!(IS_A(Line, TextLine)), "Parent of VisibleTextNode is not a TextLine"); 

	Node* Story = Line->FindParent(); 

	ERROR3IF(Story == NULL, "TextLine has no parent"); 
	ERROR3IF(!(IS_A(Story, TextStory)), "Parent of TextLine is not a TextStory"); 

	Node* n = this; 

	// Loop until we have found a next VisibleTextNode
	// This code is an optimised depth first routine
	do
	{
		if (n->FindNext())	// Has the current node got a next sibling
		{
			n = n->FindNext(); 	// Let's visit it 

			if (IS_A(n,TextLine))
			{
				// We have skipped to the next line, let's go to its first child if
				// it has one
				if (n->FindFirstChild())
				{
					n = n->FindFirstChild(); 
				}
			}

			if (n->IsAVisibleTextNode())
			{
				// Found one !
				return (VisibleTextNode*)n; 
			}

		}
		else
		{
			// The only way is up 
			n = n->FindParent(); 
		}	
		ERROR3IF(n == NULL, "We haven't found the parent TextStory"); 
	}
	while (n != Story);

	return NULL; // No more  
} 


/********************************************************************************************
>	VisibleTextNode* VisibleTextNode::FindPrevVTNInStory()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/95
	Returns:	A pointer to the previous VisibleTextNode in the TextStory
	Purpose:	Finds the previous VisibleTextNode in the TextStory. The routine searches
				across TextLines.   
	Errors:		An ERROR3 will be caused if the node is not on a line which is the child
				of a TextStory.
********************************************************************************************/

VisibleTextNode* VisibleTextNode::FindPrevVTNInStory()
{
	Node* Line = FindParent(); 

	// Be cautious
	ERROR3IF(Line == NULL, "VisibleTextNode has no parent"); 
	ERROR3IF(!(IS_A(Line, TextLine)), "Parent of VisibleTextNode is not a TextLine"); 

	Node* Story = Line->FindParent(); 

	ERROR3IF(Story == NULL, "TextLine has no parent"); 
	ERROR3IF(!(IS_A(Story, TextStory)), "Parent of TextLine is not a TextStory"); 

	Node* n = this; 

	// Loop until we have found a previous VisibleTextNode
	// This code is an optimised prev depth first routine
	do
	{
		if (n->FindPrevious())		// Has the current node got a previous sibling
		{
			n = n->FindPrevious(); 	// Let's visit it 

			if (IS_A(n, TextLine))
			{
				// Next node to check is the last child of this TextLine
				Node* LastChild = n->FindLastChild(); 
				if (LastChild != NULL)
				{
					n = LastChild;
				} 
			}								  

			if (n->IsAVisibleTextNode())
			{
				// Found one !
				return (VisibleTextNode*)n; 
			}

		}
		else
		{
			// The only way is up 
			n = n->FindParent(); 
		}	
		ERROR3IF(n == NULL, "We haven't found the parent TextStory"); 
	}
	while (n != Story);

	return NULL; // No more  
} 


/********************************************************************************************
>	AbstractTextChar* VisibleTextNode::FindNextAbstractTextCharInStory()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Returns:	A pointer to the next AbstractTextChar in the TextStory
********************************************************************************************/

AbstractTextChar* VisibleTextNode::FindNextAbstractTextCharInStory()
{
	VisibleTextNode* pChar = FindNextVTNInStory();
	while (pChar!=NULL && !pChar->IsAnAbstractTextChar())
		pChar = pChar->FindNextVTNInStory();

	return (AbstractTextChar*)pChar;
} 


/********************************************************************************************
>	AbstractTextChar* VisibleTextNode::FindPrevAbstractTextCharInStory()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Returns:	A pointer to the previous AbstractTextChar in the TextStory
********************************************************************************************/

AbstractTextChar* VisibleTextNode::FindPrevAbstractTextCharInStory()
{
	VisibleTextNode* pChar = FindPrevVTNInStory();
	while (pChar!=NULL && !pChar->IsAnAbstractTextChar())
		pChar = pChar->FindPrevVTNInStory();

	return (AbstractTextChar*)pChar;
} 


/********************************************************************************************
>	TextChar* VisibleTextNode::FindNextTextCharInStory()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Returns:	A pointer to the next TextChar in the TextStory
********************************************************************************************/

TextChar* VisibleTextNode::FindNextTextCharInStory()
{
	VisibleTextNode* pChar = FindNextVTNInStory();
	while (pChar!=NULL && !IS_A(pChar,TextChar))
		pChar = pChar->FindNextVTNInStory();

	return (TextChar*)pChar;
} 


/********************************************************************************************
>	TextChar* VisibleTextNode::FindPrevTextCharInStory()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/95
	Returns:	A pointer to the previous TextChar in the TextStory
********************************************************************************************/

TextChar* VisibleTextNode::FindPrevTextCharInStory()
{
	VisibleTextNode* pChar = FindPrevVTNInStory();
	while (pChar!=NULL && !IS_A(pChar,TextChar))
		pChar = pChar->FindPrevVTNInStory();

	return (TextChar*)pChar;
} 


/********************************************************************************************
>	VisibleTextNode* VisibleTextNode::FindNextVTNInLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Returns:	A pointer to the next VTN in the line
********************************************************************************************/

VisibleTextNode* VisibleTextNode::FindNextVTNInLine()
{
	Node* pNode = FindNext();
	while (pNode!=NULL && pNode->IsAVisibleTextNode()==FALSE)
		pNode = pNode->FindNext();

	return (VisibleTextNode*)pNode;
} 

/********************************************************************************************
>	AbstractTextChar* VisibleTextNode::FindNextAbstractTextCharInLine()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Returns:	A pointer to the next AbstractTextChar in the line
********************************************************************************************/

AbstractTextChar* VisibleTextNode::FindNextAbstractTextCharInLine()
{
	Node* pNode = FindNext();
	while (pNode!=NULL && pNode->IsAnAbstractTextChar()==FALSE)
		pNode = pNode->FindNext();

	return (AbstractTextChar*)pNode;
} 



/********************************************************************************************
>	VisibleTextNode* VisibleTextNode::FindPrevVTNInLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/1/96
	Returns:	A pointer to the previous VTN in the line
********************************************************************************************/

VisibleTextNode* VisibleTextNode::FindPrevVTNInLine()
{
	Node* pNode = FindPrevious();
	while (pNode!=NULL && pNode->IsAVisibleTextNode()==FALSE)
		pNode = pNode->FindPrevious();

	return (VisibleTextNode*)pNode;
} 

/********************************************************************************************
>	AbstractTextChar* VisibleTextNode::FindPrevAbstractTextCharInLine()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Returns:	A pointer to the prev AbstractTextChar in the line
********************************************************************************************/

AbstractTextChar* VisibleTextNode::FindPrevAbstractTextCharInLine()
{
	Node* pNode = FindPrevious();
	while (pNode!=NULL && pNode->IsAnAbstractTextChar()==FALSE)
		pNode = pNode->FindPrevious();

	return (AbstractTextChar*)pNode;
} 

/********************************************************************************************
>	TextLine* VisibleTextNode::FindParentLine()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/96
	Returns:	A pointer to the parent TextLine
********************************************************************************************/

TextLine* VisibleTextNode::FindParentLine()
{
	return (TextLine*)(this->FindParent(CC_RUNTIME_CLASS(TextLine)));
} 


/********************************************************************************************
>	virtual CopyType VisibleTextNode::GetCopyType()

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

CopyType VisibleTextNode::GetCopyType()
{
	return COMPLEXCOPY;
}


/********************************************************************************************
>	Node* VisibleTextNode::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	NULL (See below)
    Purpose:    It is illegal to copy a VisibleTextNode (it's abstract), therefore this 
    			function will generate an error if called.	       
********************************************************************************************/

Node* VisibleTextNode::SimpleCopy()
{
	ERROR2(NULL, "Trying to copy abstract VisibleTextNode !"); 
}


/********************************************************************************************
>	virtual INT32 VisibleTextNode::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)

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
								from COPYOBJECT inserted into the destination tree.
	Outputs:	pOutput

	Returns:	-1	= The routine failed to make a copy.
				 0	= No copy has been made.
				+1	= pOutput points to the copy.

    Purpose:    Create a complex copy of this node. This node has overridden GetCopyType()
				to return COMPLEXCOPY which means, when copying to the clipboard it must
				make a copy of itself and whatever else it needs. The resulting tree will
				be attached as the last child of the clipboard root.
				The routine will be called twice during a copy. Once when Stage==COPYOBJECT
				and once when Stage==COPYFINISHED. It is only necessary to return a copy
				during COPYOBJECT. COPYFINISHED can be used to tidy up and flags the node
				may set during COPYOBJECT.

				Note:
				Nodes may well require the use of flags to create a sensible complex copy.
				Every selected node will be called to copy itself during a CopyToClipboard
				whether they are SIMPLECOPY or COMPLEXCOPY types. Hence if the first call
				to a node creates a complete tree of objects including nodes which will
				likely be called by the clipboard copy routine further into its range ie
				they are selected as well, the node should keep a flag indicating it has
				already copied them and return 0 from the routine on subsequent calls.
				A concrete example of this is the text object. Any number of characters
				can be selected and copied to the clipboard. A character class specifies
				it is a COMPLEXCOPY node. Each selected character will then get called
				to make a complex copy of itself. To make a copy the character will ask
				the parent text story to make a copy of all selected nodes, including
				itself and lines. It needs to flag itself at this point as further chars
				in the text story will be asked to do the same.	This will prevent multiple
				text stories from being created, one for every selected character.
********************************************************************************************/

INT32 VisibleTextNode::ComplexCopy(CopyStage Stage, Range& RangeToCopy, Node** pOutput)
{
	TextStory* pTextStory = FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"VisibleTextNode::ComplexCopy() - pTextStory==NULL");
	return pTextStory->BaseComplexCopy(Stage, RangeToCopy, pOutput);
}


/***********************************************************************************************
>	void VisibleTextNode::CopyNodeContents(VisibleTextNode* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An ERROR3 will occur if NodeCopy is NULL
***********************************************************************************************/

void VisibleTextNode::CopyNodeContents(VisibleTextNode* NodeCopy)
{
	// Ask the base class to do its bit
	Node::CopyNodeContents(NodeCopy);

	// copy specifics
	NodeCopy->CharMatrix = CharMatrix;
	NodeCopy->PosInLine  = PosInLine;
}


/********************************************************************************************
>	virtual INT32 VisibleTextNode::ComplexHide(UndoableOperation* pOp, Node* pNextNode)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/5/95
	Inputs:		pOp			- a pointer to an undoable operation
				pNextNode	- a pointer to the next node in range.
	Returns:	-1	= The routine failed to hide this node.
				 0	= Ignored, this object does not support complex hide operations, so
				      carry on and hide the node as normal.
				+1	= The node and possibly various others have been hidden correctly.
    Purpose:	Override the node level virtual function ComplexHide. This gives us a chance to
				get in and hide the various selected members of a text story sensibly. We
				hide all necessary nodes when the last member of the text story is called to
				complex hide itself, otherwise we may corrupt the range being scanned
********************************************************************************************/

INT32 VisibleTextNode::ComplexHide(UndoableOperation* pOp, Node* pNextNode)
{
	// if there is no next node in the range then we need to hide all nodes
	BOOL CallComplexHide = TRUE;
	TextStory* pThisStory = this->FindParentStory();
	ERROR2IF(pThisStory==NULL,FALSE,"VisibleTextNode::ComplexHide() - pThisStory==NULL");

	if (pNextNode)
	{
		if ( pNextNode->IsAVisibleTextNode() || IS_A(pNextNode,TextLine) )
		{
			TextStory* pTextStory = (TextStory*)(pNextNode->FindParent(CC_RUNTIME_CLASS(TextStory)));
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


/********************************************************************************************
>	UINT32 VisibleTextNode::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the VisibleTextNode. 
********************************************************************************************/

UINT32 VisibleTextNode::GetNodeSize() const 
{
	ERROR3("Tring to find the size of abstract VisibleTextNode"); 
	return (sizeof(VisibleTextNode)); 
}


/********************************************************************************************
>	void VisibleTextNode::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void VisibleTextNode::GetDebugDetails(StringBase* Str)
{
#if DEBUG_TREE
	BaseTextClass::GetDebugDetails(Str);

 	String_256 TempStr;
	String_256 TempStr2;
	TCHAR		floatStr[20];

	// dump the character matrix
	fixed16 abcd[4];
	INT32	ef[2];
	CharMatrix.GetComponents(abcd, ef);

	TempStr._MakeMsg( TEXT("\r\nMatrix\r\n"));	 
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f,%f"), abcd[0].MakeDouble(), abcd[1].MakeDouble());
	TempStr._MakeMsg( TEXT("a, b :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	tsprintf( floatStr, 20, _T("%f,%f"), abcd[2].MakeDouble(), abcd[3].MakeDouble());
	TempStr._MakeMsg( TEXT("c, d :\t#1%s\r\n"), floatStr);
	(*Str) += TempStr;
	TempStr._MakeMsg( TEXT("e, f :\t#1%ld,\t#2%ld\r\n"), ef[0], ef[1]);
	(*Str) += TempStr;
#endif
}


/********************************************************************************************
>	BOOL VisibleTextNode::GetStoryAndCharMatrix(Matrix* pMatrix)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Outputs:	pMatrix - output matrix
	Reutrns:	FALSE if fails
	Purpose:	set the given matrix to be the concatenation of the TextStory and VTN matrices
********************************************************************************************/

BOOL VisibleTextNode::GetStoryAndCharMatrix(Matrix* pMatrix)
{
	ERROR2IF(pMatrix==NULL,FALSE,"VisibleTextNode::GetStoryAndCharMatrix() - pMatrix==NULL");

	// get the VTN matrix, then concatenate the TextStory matrix
	*pMatrix=CharMatrix;
	TextStory* pTextStory = this->FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"VisibleTextNode::GetStoryAndCharMatrix() - pTextStory==NULL");
	*pMatrix*=*(pTextStory->GetpStoryMatrix());

	return TRUE;
 }


/********************************************************************************************
>	BOOL VisibleTextNode::RenderChildAttrs(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/95
	Inputs:		pRenderRegion - 
	Returns:	FALSE if fails
	Purpose:	Render any first level children attributes into the region
********************************************************************************************/

BOOL VisibleTextNode::RenderChildAttrs(RenderRegion* pRenderRegion)
{
	ERROR2IF(pRenderRegion==NULL,FALSE,"VisibleTextNode::RenderChildAttrs() - pRenderRegion==NULL");

	Node* pChildNode=FindFirstChild();
	while (pChildNode)
	{
		if (pChildNode->IsAnAttribute())
			pChildNode->Render(pRenderRegion);
		pChildNode=pChildNode->FindNext();
	}

	return TRUE;
}


/********************************************************************************************
>	MILLIPOINT VisibleTextNode::CalcCharDistAlongLine(BOOL IncludeThisChar)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		IncludeThisChar - TRUE to include this character in the distance,
								  FALSE to leave it out
	Returns:	The distance along the TextLine that this character is at
	Purpose:	Finds the distance along the text line of this character.
				You can optionally include the width of this character in the sum.
********************************************************************************************/

MILLIPOINT VisibleTextNode::CalcCharDistAlongLine(BOOL IncludeThisChar)
{
	if (IncludeThisChar)
		return GetPosInLine()+GetCharAdvance();
	else
		return GetPosInLine();
}


/********************************************************************************************
>	BOOL VisibleTextNode::WrapRestOfLineForward(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pUndoOp   - pointer to undoable op
	Returns:	FALSE if fails
	Purpose:	Wrap all nodes from this VTN (inc) to last VTN forward to start of next line
				If the line containing this VTN is hard (ends in an EOL)
				create a new line to wrap the text onto
********************************************************************************************/

BOOL VisibleTextNode::WrapRestOfLineForward(UndoableOperation* pUndoOp)
{
	// get info about the source line
	TextLine* pSrcLine = this->FindParentLine();
	ERROR2IF(pSrcLine==NULL,FALSE,"VisibleTextNode::WrapRestOfLineForward() - pSrcLine==NULL");
	VisibleTextNode* pLastVTNToMove = pSrcLine->FindLastVTN();
	ERROR2IF(pLastVTNToMove==NULL,FALSE,"VisibleTextNode::WrapRestOfLineForward() - no VTN's on soft line");

	// if SrcLine soft, find next line (localising non-LLA attrs on it) else create a new line
	AttrTypeSet* pAttrSet = NULL;	// assume creating new line so all attrs need localising
	TextLine*    pDstLine = NULL;
	BOOL         ok       = TRUE;
	if (pLastVTNToMove->IsAnEOLNode())
	{
		pDstLine = new TextLine;
		if (pDstLine==NULL)
			return FALSE;
		pSrcLine->CopyNodeContents(pDstLine);	// copy cached line level attr info
		if (ok) ok = pDstLine->DoInsertNewNode(pUndoOp,pSrcLine,NEXT);
	}
	else
	{
		pDstLine = pSrcLine->FindNextLine();
		ERROR2IF(pDstLine==NULL,FALSE,"VisibleTextNode::WrapRestOfLineForward() - no next line after soft line");

		pAttrSet = new AttrTypeSet();
		if (ok) ok = pSrcLine->AddNonLineLevelDescendantAttrsToSet(pAttrSet);
		if (ok) ok = pDstLine->AddNonLineLevelDescendantAttrsToSet(pAttrSet);

		if (ok) ok = pDstLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,pAttrSet);
	}

	// get context node and attach direction for attaching text as first objects in dest line
	Node* pAttachNode = pDstLine;
	AttachNodeDirection AttachDir = FIRSTCHILD;
	pDstLine->GetAttachNodeAndDirectionToAttachFirstChildObject(&pAttachNode,&AttachDir);

	// localise on SrcLine, move text to DstLine, then factor out on both
	// Localise/FactorOut not global as attrs on story should never change
	if (ok) ok = pSrcLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,FALSE,pAttrSet);
	if (ok) ok = this->DoMoveNodes(pUndoOp, pLastVTNToMove, pAttachNode, AttachDir);
	if (ok) ok = pSrcLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,pAttrSet);
	if (ok) ok = pDstLine->DoFactorOutCommonChildAttributes(pUndoOp,FALSE,pAttrSet);

	if (pAttrSet!=NULL)
	{
		pAttrSet->DeleteAll();
		delete pAttrSet;
	}

	return ok;
}


/********************************************************************************************
>	BOOL VisibleTextNode::WrapFromStartOfLineBack(UndoableOperation* pUndoOp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/7/96
	Inputs:		pUndoOp - pointer to undoable operation
	Returns:	FALSE if fails
	Purpose:	Wrap all chars upto this VTN (inc) back to the end of the previous line
				If this line is then empty, hide it
********************************************************************************************/

BOOL VisibleTextNode::WrapFromStartOfLineBack(UndoableOperation* pUndoOp)
{
	// get info about this and previous line
	TextLine* pThisLine = this->FindParentLine();
	ERROR2IF(pThisLine==NULL,FALSE,"VisibleTextNode::WrapFromStartOfLineBack() - pThisLine==NULL");
	VisibleTextNode* pFirstVTN = pThisLine->FindFirstVTN();
	ERROR2IF(pFirstVTN==NULL,FALSE,"VisibleTextNode::WrapFromStartOfLineBack() - no VTN on this line!");
	TextLine* pPrevLine = pThisLine->FindPrevLine();
	ERROR2IF(pPrevLine==NULL,FALSE,"VisibleTextNode::WrapFromStartOfLineBack() - pPrevLine==NULL");
	ERROR2IF(pPrevLine->FindEOLNode()!=NULL,FALSE,"VisibleTextNode::WrapFromStartOfLineBack() - trying to wrap back onto a hard line!");

	// get a set of non line level attrs, which are to be localised to char level
	AttrTypeSet NonLineLevelAttrs;
	BOOL    ok = pThisLine->AddNonLineLevelDescendantAttrsToSet(&NonLineLevelAttrs);
	if (ok) ok = pPrevLine->AddNonLineLevelDescendantAttrsToSet(&NonLineLevelAttrs);

	// move ALL the nodes from first VTN on line to this VTN back to end of prev line
	// if wrapping whole line back, hide the empty line
	// since previous line is soft assumes this and prev lines have same line level attrs
	// Localise/FactorOut global as Line Level attrs on story may change change
	BOOL WholeLine = (this->FindNextVTNInLine()==NULL);
	if (ok) ok = pThisLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,&NonLineLevelAttrs);
	if (ok) ok = pPrevLine->DoLocaliseCommonAttributes(pUndoOp,FALSE,TRUE,&NonLineLevelAttrs);
	if (ok) ok = pFirstVTN->DoMoveNodes(pUndoOp, this, pPrevLine, LASTCHILD);
	if (ok &&  WholeLine) ok = pThisLine->DoHideNode(pUndoOp);
	if (ok && !WholeLine) ok = pThisLine->DoFactorOutCommonChildAttributes(pUndoOp,TRUE,&NonLineLevelAttrs);
	if (ok) ok = pPrevLine->DoFactorOutCommonChildAttributes(pUndoOp,TRUE,&NonLineLevelAttrs);

	return ok;
}


/********************************************************************************************
>	BOOL VisibleTextNode::DoMoveNodes(UndoableOperation*  pUndoOp,
									  VisibleTextNode*    pLastVTN,
									  Node*               pDestNode,
									  AttachNodeDirection Direction)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		pUndoOp   - pointer to undoable operation (NULL if not undoable)
				pLastVTN  - pointer to VTN after last node to move (must be on the same line as this)
				pDestNode - pointer to node to attach to
				Direction - direction to attach in
	Returns:	FALSE if fails
	Purpose:	Move ALL nodes from this VTN to LastVTN to pDestNode in the specified direction
	Assumes:	First and last VTNs are on the same line
	Note:		only VTN in range are moved so hidden nodes don't cause undo bloat!
********************************************************************************************/

BOOL VisibleTextNode::DoMoveNodes(UndoableOperation*  pUndoOp,
								  VisibleTextNode*    pLastVTN,
								  Node*               pDestNode,
								  AttachNodeDirection Direction)
{
	ERROR2IF( pLastVTN==NULL,FALSE,"VisibleTextNode::DoMoveNodes() - pLastVTN==NULL");
	ERROR2IF(pDestNode==NULL,FALSE,"VisibleTextNode::DoMoveNodes() - pDestNode==NULL");

	// move all VTN in the range to the specified location
	// however, can only move the caret once there are are some cahrs on the new line
	Node*            pPrevDestNode = pDestNode;
	VisibleTextNode* pSrcVTN       = this;
	while (pPrevDestNode!=pLastVTN)
	{
		ERROR2IF(pSrcVTN==NULL,FALSE,"VisibleTextNode::DoMoveNodes() - did not find pLastVTN");
		VisibleTextNode* pNextSrcVTN = pSrcVTN->FindNextVTNInLine();
		if (pSrcVTN->DoMoveNode(pUndoOp, pPrevDestNode, Direction)==FALSE)
			return FALSE;
		Direction     = NEXT;		// all subsequent VTN moved after previous
		pPrevDestNode = pSrcVTN;
		pSrcVTN = pNextSrcVTN;
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL VisibleTextNode::DoMoveNode(UndoableOperation*  pUndoOp,
									 Node*               pDestNode,
									 AttachNodeDirection Direction)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/96
	Inputs:		pUndoOp   - pointer to undoable operation (NULL if not undoable)
				pDestNode - pointer to node to attach to
				Direction - direction to attach in
	Returns:	FALSE if fails
	Purpose:	Move this VTN to the specified location, flagging it 'Affected' in
				the old and new position
********************************************************************************************/

BOOL VisibleTextNode::DoMoveNode(UndoableOperation*  pUndoOp,
								 Node*               pDestNode,
								 AttachNodeDirection Direction)
{
	ERROR2IF(pDestNode==NULL,FALSE,"VisibleTextNode::DoMoveNode() - pDestNode==NULL");
	TextStory* pTextStory = this->FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"VisibleTextNode::DoMoveNode() - caret has no parent story!");

	// flag node affected in old position (and parents)
	FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected();
	
	// if selected, deselect to maintain 'parent of selected' flag
	BOOL selected = IsSelected();
	if (selected)
		SetSelected(FALSE);

	// if the caret (remember its position if undoable) always move it non-undoably
	// else just move the char, non-undoably if required
	// Note:must remember caret's old pos as if it is moved onto a new line,
	//		when it is undone, the caret will be a child of the hidden line
	//		and PrePostTextAction cannot cope with restoring the caret once hidden
	if (this->IsACaret())
	{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
		if (pUndoOp!=NULL)
			if (!PositionCaretAction::DoStoreCaretPosition(pUndoOp,pTextStory))
				return FALSE;
#endif
		MoveNode(pDestNode,Direction);
	}
	else
	{
		if (pUndoOp==NULL)
			MoveNode(pDestNode,Direction);
		else
			if (pUndoOp->DoMoveNode(this, pDestNode, Direction)==FALSE)
				return FALSE;
	}

	// if was selected, reselect to maintain 'parent of selected' flag
	if (selected)
		SetSelected(TRUE);

	// flag node affected in new position (so parents flagged correctly)
	FlagNodeAndDescendantsAffectedAndParentsHaveDescendantAffected();

	return TRUE;
}


/********************************************************************************************
>	void VisibleTextNode::ScrollToShow()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/4/95
	Purpose:	If this VTN is not fully on the screen then the screen is scrolled so the
				VTN is more visible.
********************************************************************************************/

void VisibleTextNode::ScrollToShow()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	DocView* pDocView = DocView::GetSelected();
	Spread* pSpread = FindParentSpread();		

	if (pDocView!=NULL && pSpread!=NULL)
	{
		DocRect ScreenArea = GetBlobBoundingRect();

		// Get the extent of the current view and convert to spread coordinates.
		DocRect WorkArea = pDocView->GetDocViewRect(pSpread);
		pSpread->DocCoordToSpreadCoord(&WorkArea);

		BOOL Scroll = FALSE;
		DocCoord WindowTopLeft;
		DocCoord Offset(0,0);		// Amount to offset current TopLeft by
	
		if (!WorkArea.ContainsRect(ScreenArea))
		{
			if (WorkArea.Height() > ScreenArea.Height() && WorkArea.Width() > ScreenArea.Width())
			{
				const double Shift = 0.05;

				if (ScreenArea.hi.y > WorkArea.hi.y)
					Offset.y = ScreenArea.hi.y-WorkArea.hi.y + (INT32)(WorkArea.Height()*Shift);

				if (ScreenArea.lo.y < WorkArea.lo.y)
					Offset.y = ScreenArea.lo.y-WorkArea.lo.y - (INT32)(WorkArea.Height()*Shift);

				if (ScreenArea.hi.x > WorkArea.hi.x)
					Offset.x = ScreenArea.hi.x-WorkArea.hi.x + (INT32)(WorkArea.Width()*Shift);

				if (ScreenArea.lo.x < WorkArea.lo.x)
					Offset.x = ScreenArea.lo.x-WorkArea.lo.x - (INT32)(WorkArea.Width()*Shift);

				if (Offset != DocCoord(0,0))
				{
					Scroll = TRUE;
					WindowTopLeft = DocCoord(WorkArea.lo.x, WorkArea.hi.y) + Offset;
				}
			}
			else
			{
				// Cannot contain object on screen.  Centre it.
				Scroll = TRUE;
				DocCoord CentreOfObject((ScreenArea.lo.x+ScreenArea.hi.x)/2, (ScreenArea.lo.y+ScreenArea.hi.y)/2);

				WindowTopLeft = DocCoord(CentreOfObject.x-WorkArea.Width()/2,CentreOfObject.y+WorkArea.Height()/2);
			}
		}

		// Now scroll to the new position
		if (Scroll)
		{
			// First, ensure that the spread's pasteboard is large enough to include this node
			// This is pretty quick if it is big enough.
			pSpread->ExpandPasteboardToInclude(GetBoundingRect());

			// Convert the window coordinate from Spread to Document coords...
			pSpread->SpreadCoordToDocCoord(&WindowTopLeft);

			// ...and thence to Logical coordinates
			WorkCoord WTL = WindowTopLeft.ToWork(pSpread, pDocView);
			pDocView->SetScrollOffsets(WTL, TRUE);

			// New stuff added by Will 29/6/95
			// Now we have scrolled the View, we need to invalidate the newly visible areas,
			// so that any blob rendering knows that the area has a pending redraw.
			// If we wait for the Paint message from the OS, then it will be too late.

			// First we get the new extent of the view we just scrolled and convert to spread coordinates.
			WorkArea = pDocView->GetDocViewRect(pSpread);
			pSpread->DocCoordToSpreadCoord(&WorkArea);

			DocRect	Invalid;

			if (Offset.x != 0)
			{
				// Get the invalid horizontal scroll rect, by subtracting the x scroll offset
				// from the left or right edge of the screen (depending on which way we scrolled)
				Invalid.lo.x = Offset.x > 0 ? WorkArea.hi.x - Offset.x : WorkArea.lo.x;
				Invalid.lo.y = WorkArea.lo.y;
				Invalid.hi.x = Offset.x > 0 ? WorkArea.hi.x : WorkArea.lo.x - Offset.x;
				Invalid.hi.y = WorkArea.hi.y;

				// and invalidate it
				pDocView->ForceRedraw(pSpread, Invalid);
			}

			if (Offset.y != 0)
			{
				// Get the invalid vertical scroll rect, by subtracting the y scroll offset
				// from the top or bottom edge of the screen (depending on which way we scrolled)
				Invalid.lo.x = WorkArea.lo.x;
				Invalid.lo.y = Offset.y > 0 ? WorkArea.hi.y - Offset.y : WorkArea.lo.y;
				Invalid.hi.x = WorkArea.hi.x;
				Invalid.hi.y = Offset.y > 0 ? WorkArea.hi.y : WorkArea.lo.y - Offset.y;

				// and invalidate it
				pDocView->ForceRedraw(pSpread, Invalid);
			}
		}
	}
#endif
}


/********************************************************************************************
>	virtual BOOL VisibleTextNode::CanAttrBeAppliedToMe(CCRuntimeClass* AttrType)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/6/95
	Inputs:		AttrType: The type of the attribute. It is a type, rather than an 
				instance because this function needs to be called from the Localise/Factor out 
				routines which operate on attribute types.

	Returns:	TRUE if an attribute of AttrType can be applied to this object, 
				FALSE otherwise. 

	Purpose:	This fn is not to be confused with RequiresAttrib, it determines if an 
				attribute of type AttrType can be directly applied to this object. The
				object is assumed to Require the attribute. 

				This function is called by the GetObjectToApplyTo virtual function.
				
				The function returns FALSE if we are trying to apply a line level 
				attribute to a VisibleTextNode. As a result attributes will get applied
				to the parent TextLine object.
********************************************************************************************/ 

BOOL VisibleTextNode::CanAttrBeAppliedToMe(CCRuntimeClass* AttrType)
{
	return !TextLine::IsAttrTypeLineLevel(AttrType);
}


/********************************************************************************************
>	virtual BOOL VisibleTextNode::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/10/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the VisibleTextNode to respond to pop up menu clicks on itself.
********************************************************************************************/

BOOL VisibleTextNode::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
	TextStory* pStory = this->FindParentStory();

	if (pStory!=NULL && pStory->IsSelected())
		return pStory->OnNodePopUp(pSpread, PointerPos, pMenu);
	else
		return FALSE;
}


/////////////////////////////////////////////////////////////////////////////////////////////
// AbstractTextChar methods

/********************************************************************************************
>	void AbstractTextChar::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/2/96
	Purpose:	common init function for constructors
********************************************************************************************/

void AbstractTextChar::Init()
{
	mCharWidth       = 0;
	mCharAdvance     = 0;
	mFontAscent      = 0;
	mFontDescent     = 0;
	mFontSize        = 0;
	mBaseLineShift   = 0;
	mAttrdCharBounds = DocRect(0,0,0,0);
}


/********************************************************************************************
>	AbstractTextChar::AbstractTextChar()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple AbstractTextChar constructor, it is required so that SimpleCopy will work.
********************************************************************************************/

AbstractTextChar::AbstractTextChar(): VisibleTextNode()	// Call the base class
{
	Init();
}


/********************************************************************************************
>	AbstractTextChar::AbstractTextChar(Node* ContextNode, AttachNodeDirection Direction, 
					 				   WCHAR ChCode)

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
				
				ChCode: The UNICODE character to store in the TextChar node
				                
	Purpose:	The main TextChar constructor
********************************************************************************************/

AbstractTextChar::AbstractTextChar(Node* ContextNode, AttachNodeDirection Direction): 
	VisibleTextNode(ContextNode, Direction)	// call the base class
{
	Init();
}


/********************************************************************************************
>	Node* AbstractTextChar::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	
    Purpose:    Because this is an abstract class, the SimpleCopy method simply causes
				an ERROR3 to go off.
********************************************************************************************/

Node* AbstractTextChar::SimpleCopy()
{
	ERROR3("Calling abstract classes SimpleCopy");

	// Well I suppose we should do the copy if the call gets into the retail 
	AbstractTextChar* NodeCopy = new AbstractTextChar();

    ERROR1IF(NodeCopy==NULL, NULL, _R(IDE_NOMORE_MEMORY)); 

    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}



/***********************************************************************************************
>	void AbstractTextChar::CopyNodeContents(AbstractTextChar* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
***********************************************************************************************/

void AbstractTextChar::CopyNodeContents(AbstractTextChar* NodeCopy)
{
	// Ask the base class to do its bit
	VisibleTextNode::CopyNodeContents( NodeCopy );

	// copy specifics
	NodeCopy->mCharWidth       = mCharWidth;
	NodeCopy->mCharAdvance     = mCharAdvance;
	NodeCopy->mFontAscent      = mFontAscent;
	NodeCopy->mFontDescent     = mFontDescent;
	NodeCopy->mFontSize        = mFontSize;
	NodeCopy->mBaseLineShift   = mBaseLineShift;
	NodeCopy->mAttrdCharBounds = mAttrdCharBounds;
}


/***********************************************************************************************
>   void AbstractTextChar::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AbstractTextChar::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AbstractTextChar), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AbstractTextChar))
		CopyNodeContents((AbstractTextChar*)pNodeCopy);
}



/********************************************************************************************
>	String AbstractTextChar::Describe(BOOL Plural, BOOL Verbose = TRUE)  		   

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		Plural:	Singular or plural description
	Returns:	A string describing the node
	Purpose:	Gives a description of all character nodes
********************************************************************************************/

String AbstractTextChar::Describe(BOOL Plural, BOOL Verbose)  		   
{
	if (Plural)
		return(String(_R(IDS_DESCRIBE_TEXTCHARP)));  
	else
		return(String(_R(IDS_DESCRIBE_TEXTCHARS))); 
}


/********************************************************************************************
>	UINT32 AbstractTextChar::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
********************************************************************************************/

UINT32 AbstractTextChar::GetNodeSize() const 
{
	ERROR3("Trying to find the size of an AbstractTextChar ?"); 
	return (sizeof(AbstractTextChar)); 
}


/********************************************************************************************
>	void AbstractTextChar::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void AbstractTextChar::GetDebugDetails(StringBase* Str)
{
	VisibleTextNode::GetDebugDetails(Str);
}


/********************************************************************************************
>	virtual DocRect AbstractTextChar::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/3/94
	Inputs:		DontUseAttrs - indicates that bounds of attributes applied to node should be ignored
				Defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The characters bounding rect
	Purpose:	Gets the bounds of the char
				if the cached bounds are invalid, they are recalculated
********************************************************************************************/

DocRect AbstractTextChar::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// if not hit testing, attrs should be included and cache is valid, return cached value
	if (HitTest==FALSE && DontUseAttrs==FALSE && IsBoundingRectValid)
		return BoundingRectangle;

	BOOL    ok=TRUE;
	DocRect TempBounds;
	if (HitTest)
		ok=GetMetricsRectBounds(&TempBounds);
	else
	{
		// get the matrix to transform the char to the correct point in the doc
		Matrix matrix;
		ok=GetStoryAndCharMatrix(&matrix);

		// get attr'd-char bounds from node, and transform them
		TempBounds=GetAttrdCharBounds();											  
		if (ok) ok=matrix.TransformBounds(&TempBounds);

		// if including attrs, account for line width and recache bounds
		NodeAttribute* pAttr;
		if (ok && DontUseAttrs==FALSE && FindAppliedAttribute(CC_RUNTIME_CLASS(AttrLineWidth), &pAttr))
		{
			// get width of line and inflate bounds
			LineWidthAttribute* pLineWidthAttr=(LineWidthAttribute*)(pAttr->GetAttributeValue());
			TempBounds.Inflate( (MILLIPOINT)(pLineWidthAttr->LineWidth)/2 );

			// update value in cache and flag valid
			BoundingRectangle   = TempBounds;
			IsBoundingRectValid = TRUE;
		}
		// now we must also account for brush attributes
		NodeAttribute* pBrushAttr = NULL;
		if (ok && DontUseAttrs == FALSE && FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBrushType), &pBrushAttr))
		{
			DocRect BrushRect = ((AttrBrushType*)pBrushAttr)->GetAttrBoundingRect(this);
			BoundingRectangle = BoundingRectangle.Union(BrushRect);
			IsBoundingRectValid = TRUE;
		}
	}

	// report any errors which occurred, and return the bounding box
	if (!ok) InformError();
	return TempBounds;
}


/********************************************************************************************
>	DocRect AbstractTextChar::GetBlobBoundingRect()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95
	Returns:	DocRect - Returns the bounding rect of the char and its blobs
	Purpose:	Calculates the blob bounding box of the char based on its metrics
				Irrespective of their visibility
********************************************************************************************/

DocRect AbstractTextChar::GetBlobBoundingRect()
{
	DocRect Bounds(0,0,0,0);
	if (GetMetricsRectBounds(&Bounds)==FALSE)
		return GetBoundingRect();

	IncludeChildrensBoundingRects(&Bounds);
	return Bounds;
}

/********************************************************************************************
>	virtual DocRect AbstractTextChar::GetImagemapClickableRectangle()

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		-
	Returns:	The bounding rectangle of this character and all adjacent characters
				in the same line with the same Web Address attribute
	Purpose:	Used to get the clickable area of this character for an imagemap.

				The clickable area of this character is defined as the bounding
				rectangle of all adjacent characters with the same Web Address attribute.
				This is so that sequences of characters with the same Web Address attribute
				present a single clickable area, rather than a lot of separate areas.

				The way it does this is by calling our helper function
				GetImagemapRectForAdjacentChars twice: once to get the
				clickable rectangle of the characters before this, once
				to get the clickable rectangle of the characters after this.

********************************************************************************************/

DocRect AbstractTextChar::GetImagemapClickableRectangle()
{
	DocRect rectPrevious=GetImagemapRectForAdjacentChars(FALSE);
	DocRect rectNext=GetImagemapRectForAdjacentChars(TRUE);

	rectPrevious=rectPrevious.Union(rectNext);

	return rectPrevious;
}

/********************************************************************************************
>	virtual DocRect AbstractTextChar::GetImagemapRectForAdjacentChars(BOOL fForwards)

	Author:		Graham_Walmsley (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/3/97
	Inputs:		fForwards	TRUE for the characters after this one
							FALSE for the character before this one
	Returns:	The bounding rectangle of this character and all characters
				(before/after) this one in this line with the same
				Web Address attribute.

	Purpose:	Called by GetImagemapClickableRectangle to get the clickable rectangle
				for this character and all the adjacent characters with the same
				WebAddress attribute.

	SeeAlso:	AbstractTextChar::GetImagemapClickableRectangle()

********************************************************************************************/

DocRect AbstractTextChar::GetImagemapRectForAdjacentChars(BOOL fForwards)
{
	//First set up a pointer to the node we're currently looking at.
	//It will start at this text character
	//and scan either forwards or backwards
	AbstractTextChar* patcLookNode=this;

	//This flag tells us whether we are pointing at a character with
	//the same Web Address attribute as this one
	BOOL fSameAttribute=TRUE;

	//And this is the DocRect we will return
	DocRect rectToReturn=GetBoundingRect(FALSE, TRUE);

	//And let's find the Web Address attribute applied to this character
	AttrWebAddress* pwaThisAttribute=NULL;

	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrWebAddress), (NodeAttribute**) &pwaThisAttribute);
	
	//While the node we're looking at exists, and has the same
	//Web Address attribute applied to it as this one
	while (patcLookNode && fSameAttribute)
	{
		//Then get the bounding box of that node
		DocRect rectLookNode=patcLookNode->GetBoundingRect(FALSE,TRUE);

		//Expand our rectangle to return to include that bounding box
		rectToReturn=rectToReturn.Union(rectLookNode);

		//Then move the pointer either forwards or backwards
		if (fForwards)
			patcLookNode=patcLookNode->FindNextAbstractTextCharInLine();
		else
			patcLookNode=patcLookNode->FindPrevAbstractTextCharInLine();

		//Now, if we are still looking at a node
		if (patcLookNode)
		{
			//Then find the attribute applied to that node
			AttrWebAddress* pwaLookAttribute=NULL;

			patcLookNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrWebAddress), (NodeAttribute**) &pwaLookAttribute);
			ERROR2IF(pwaLookAttribute==NULL, DocRect(0,0,0,0), "AbstractTextChar::GetImagemapRectForAdjacentChars got no applied attribute!");
		
			//And test whether it's the same as the attribute applied to this node
			fSameAttribute=(*pwaLookAttribute==*pwaThisAttribute);
		}
	}

	//Then return the rectangle we've created
	return rectToReturn;
}


/********************************************************************************************
>	void AbstractTextChar::RenderObjectBlobs(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/4/95
	Inputs:		pRenderRegion - region to render the blobs into
	Purpose:	Renders the selection blobs
********************************************************************************************/

void AbstractTextChar::RenderObjectBlobs(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	Path* pPath=CreateMetricsRectPath();
	if (pPath!=NULL && pRenderRegion!=NULL)
	{
		DocColour Trans(COLOUR_TRANS);
		pRenderRegion->SetLineWidth(0);
		pRenderRegion->SetLineColour(Trans);
		pRenderRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRenderRegion->DrawPath(pPath);
		delete pPath;
	}
#endif
}


/********************************************************************************************
>	void AbstractTextChar::RenderTinyBlobs(RenderRegion* pRenderRegion)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Inputs:		pRenderRegion - region to render the blobs into
	Purpose:	Renders the tiny blob, which is an outline selection box
********************************************************************************************/

void AbstractTextChar::RenderTinyBlobs(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	Path* pPath=CreateMetricsRectPath();
	if (pPath!=NULL && pRenderRegion!=NULL)
	{
		DocColour Trans(COLOUR_TRANS);
		pRenderRegion->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRenderRegion->SetFillColour(Trans);
		pRenderRegion->DrawPath(pPath);
		delete pPath;
	}
#endif
}


/********************************************************************************************
>	BOOL AbstractTextChar::GetMetricsRect(DocRect* pRect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Outputs		pRect - 
	Returns:	FALSE if fails
	Purpose:	return the metrics rect of a char
********************************************************************************************/

BOOL AbstractTextChar::GetMetricsRect(DocRect* pRect)
{
	ERROR2IF(pRect==NULL,FALSE,"AbstractTextChar::GetMetricsRect() - pRect==NULL");

	TextLine* pTextLine=(TextLine*)FindParent();
	ERROR2IF(pTextLine==NULL,FALSE,"AbstractTextChar::GetMetricsRect - pTextLine==NULL");

	// get a rect (relative to the TextStory) from the bottom left to top right of the char
	pRect->lo.y = pTextLine->GetLineDescent();
	pRect->hi.y = pTextLine->GetLineAscent();
	pRect->lo.x = 0;
	pRect->hi.x = 0;
	MILLIPOINT advance=GetVisibleAdvance();
	if (advance>=0)
		pRect->hi.x = advance;
	else
		pRect->lo.x = advance;

	return TRUE;
}


/********************************************************************************************
>	BOOL AbstractTextChar::GetMetricsRectInStory(DocRect* pRect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Outputs		pRect - 
	Returns:	FALSE if fails
	Purpose:	return the metrics rect of a char relative to the TextStory
	Note:		Not valid for text on a path!!
********************************************************************************************/

BOOL AbstractTextChar::GetMetricsRectInStory(DocRect* pRect)
{
	ERROR2IF(pRect==NULL,FALSE,"AbstractTextChar::GetMetricsRectInStory() - pRect==NULL");

	if (GetMetricsRect(pRect)==FALSE)
		return FALSE;

	// make it relative to the story!
	GetpMatrix()->transform((Coord*)&(pRect->lo),2);

	return TRUE;
}


/********************************************************************************************
>	BOOL AbstractTextChar::GetMetricsRectBounds(DocRect* pRect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/4/95
	Outputs		pRect - 
	Returns:	FALSE if fails
	Purpose:	return the bounds in the doc of the char's metrics rect
********************************************************************************************/

BOOL AbstractTextChar::GetMetricsRectBounds(DocRect* pRect)
{
	ERROR2IF(pRect==NULL,FALSE,"AbstractTextChar::GetMetricsRectInStory() - pRect==NULL");

	Matrix matrix;
	if (GetStoryAndCharMatrix(&matrix)==FALSE)
		return FALSE;

	if (GetMetricsRect(pRect)==FALSE)
		return FALSE;

	if (matrix.TransformBounds(pRect)==FALSE)
		return FALSE;

	return TRUE;
}


/********************************************************************************************
>	Path* AbstractTextChar::CreateMetricsRectPath()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Returns:	pointer to path (or NULL if fails)
	Purpose:	creates a rectangular path the advance width of	a char and
				the height of a line transformed to the correct point in the doc
********************************************************************************************/

Path* AbstractTextChar::CreateMetricsRectPath()
{
	DocRect rect(0,0,0,0);
	if (GetMetricsRect(&rect)==FALSE)
		return FALSE;

	// create the path
	Path* pBoundsPath=new Path(); 
	BOOL ok=(pBoundsPath!=NULL);
	if (ok) ok=pBoundsPath->Initialise(6,12);
	if (ok) ok=pBoundsPath->AddMoveTo(rect.lo);
	if (ok) ok=pBoundsPath->AddLineTo(DocCoord(rect.hi.x,rect.lo.y));	// bottom line
	if (ok)	ok=pBoundsPath->AddLineTo(rect.hi);						// right side
	if (ok) ok=pBoundsPath->AddLineTo(DocCoord(rect.lo.x,rect.hi.y));	// top line
	if (ok)	ok=pBoundsPath->AddLineTo(rect.lo);						// left side
	if (ok) ok=pBoundsPath->CloseSubPath();
	if (ok) pBoundsPath->IsFilled=TRUE;

	// transform path to correct point in doc
	Matrix matrix;
	if (ok) ok=GetStoryAndCharMatrix(&matrix);
	DocCoord* pPathCoords=NULL;
	if (ok) pPathCoords=pBoundsPath->GetCoordArray();							     
	if (ok) ok=(pPathCoords!=NULL);
	if (ok)	matrix.transform((Coord*)pPathCoords, pBoundsPath->GetNumCoords());

	// if not OK, delete path and set return path pointer to NULL
	if (!ok)
	{
		delete pBoundsPath;
		pBoundsPath=NULL;
	}

	return pBoundsPath;
}

/********************************************************************************************
>	BOOL AbstractTextChar::ReCacheMetrics(FormatRegion* pFormatRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/96
	Inputs:		pFormatRegion - 
	Returns:	FALSE if fails
	Purpose:	Recache metrics (and AttrdCharBound) in AbstractTextChars
********************************************************************************************/

BOOL AbstractTextChar::ReCacheMetrics(FormatRegion* pFormatRegion)
{
	PORTNOTETRACE("text","AbstractTextChar::ReCacheMetrics - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// get metrics for this char with current attribute stack
	CharMetrics metrics;
	if (pFormatRegion->GetCharMetrics(&metrics,GetUnicodeValue())==FALSE)
		return FALSE;

	// set font ascent, descent and size
	SetFontAscent( metrics.FontAscent);
	SetFontDescent(metrics.FontDescent);
	SetFontSize(pFormatRegion->GetFontSize());
	SetBaseLineShift(pFormatRegion->GetBaseLineShift());

	// set char width and advance width inc tracking ...
	if (IS_A(this,KernCode))
	{
		INT32       KernInEmBy1000 = ((KernCode*)this)->GetValue().x;
		MILLIPOINT KernInMP       = MulDiv(KernInEmBy1000, metrics.FontEmWidth, 1000);
		SetCharWidth(KernInMP);
		SetCharAdvance(GetCharWidth());
	}
	else if (this->IsAnEOLNode())
	{
		SetCharWidth(0);
		SetCharAdvance(0);
	}
	else
	{
		SetCharWidth(metrics.CharWidth);
		MILLIPOINT TrackingInMP       = 0;
		MILLIPOINT TrackingInEmBy1000 = pFormatRegion->GetTracking();
		if (TrackingInEmBy1000 !=0 )
			TrackingInMP = MulDiv(TrackingInEmBy1000, metrics.FontEmWidth, 1000);

		TextStory *story = FindParentStory();
		if (story)
		{
			if (story->IsAutoKerning())
				SetCharAdvance(GetCharWidth()+TrackingInMP+GetAutoKernSize(pFormatRegion));
			else
				SetCharAdvance(GetCharWidth()+TrackingInMP);
		}
		else
			ERROR3("Can't find base story");
	}

	// set AttrdCharBounds (if zero size (or kern code), set to sensible size for hit test)
	DocRect AttrdCharBounds(0,0,0,0);
	if (!IS_A(this,KernCode))
		if (pFormatRegion->GetAttrdCharBounds(&AttrdCharBounds,GetUnicodeValue())==FALSE)
			return FALSE;
	if (AttrdCharBounds.IsEmpty())
		AttrdCharBounds.hi = DocCoord(GetCharAdvance(),pFormatRegion->GetFontSize()/3);
	SetAttrdCharBounds(AttrdCharBounds);
#endif
	return TRUE;
}


/********************************************************************************************
>	MILLIPOINT AbstractTextChar::GetVisibleAdvance()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/95
	Returns:	pointer to path (or NULL if fails)
	Purpose:	get the visible advance with of the char
				ie advance width less last char tracking if at end of line
********************************************************************************************/

MILLIPOINT AbstractTextChar::GetVisibleAdvance()
{
	if (IS_A(this,TextChar) && FindNext(CC_RUNTIME_CLASS(AbstractTextChar))==NULL)
		 return GetCharWidth();

	return GetCharAdvance();
}


/////////////////////////////////////////////////////////////////////////////////////////////
// TextChar methods

/********************************************************************************************
>	TextChar::TextChar()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple TextChar constructor, it is required so that SimpleCopy will work.
				You should not normally call this constructor as it does not initialise
				the object.
********************************************************************************************/

TextChar::TextChar(): AbstractTextChar()
{
	Ch = _T('\0'); 
}

 
/********************************************************************************************
>	TextChar::TextChar(Node* ContextNode, AttachNodeDirection Direction)

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

	Purpose:	The main TextChar constructor
********************************************************************************************/

TextChar::TextChar(Node* ContextNode, AttachNodeDirection Direction, 
		 		   WCHAR ChCode): AbstractTextChar(ContextNode, Direction)
{
	Ch = ChCode; 
}


/********************************************************************************************
>	Node* TextChar::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	A copy of the node, or NULL if we are out of memory 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes of Node  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
********************************************************************************************/

Node* TextChar::SimpleCopy()
{
	// Make a new BigChar and then copy things into it
	TextChar* NodeCopy = new TextChar();

    ERROR1IF(NodeCopy==NULL, NULL, _R(IDE_NOMORE_MEMORY)); 

    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}


/***********************************************************************************************
>   void TextChar::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void TextChar::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, TextChar), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, TextChar))
		CopyNodeContents((TextChar*)pNodeCopy);
}



/***********************************************************************************************
>	void TextChar::CopyNodeContents(TextChar* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
***********************************************************************************************/

void TextChar::CopyNodeContents(TextChar* NodeCopy)
{
	// Ask the base class to do its bit
	AbstractTextChar::CopyNodeContents( NodeCopy );
	// Copy the rest of the data in here
	NodeCopy->Ch        = Ch;
}


/********************************************************************************************
>	UINT32 TextChar::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
********************************************************************************************/

UINT32 TextChar::GetNodeSize() const 
{
	return (sizeof(TextChar)); 
}


/********************************************************************************************
>	void TextChar::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void TextChar::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	AbstractTextChar::GetDebugDetails(Str);
	String_256 TempStr;
		
	TempStr._MakeMsg( TEXT( "\r\nUnicode Ch code = #1%u ('#2%c')"),Ch, (Ch<256) ? Ch : ' ' );
	(*Str) += TempStr;
#endif
}


/********************************************************************************************
>	virtual void TextChar::Render(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		pRenderRegion - RenderRegion with current attributes stack
	Purpose:	render a TextChar with the current attributes specified in the RenderRegion
********************************************************************************************/

void TextChar::Render(RenderRegion* pRenderRegion)
{
	if (RenderCore(pRenderRegion)==FALSE)
	{
		// Mike 6/10/95: removed this call as it causes big problems if redraw fails
		// InformError();
	}
}


/********************************************************************************************
>	BOOL TextChar::RenderCore(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		pRenderRegion - RenderRegion with current attributes stack
	Returns:	FALSE if fails
	Purpose:	render a TextChar with the current attributes specified in the RenderRegion
********************************************************************************************/

BOOL TextChar::RenderCore(RenderRegion* pRenderRegion)
{
	PORTNOTETRACE("text","TextChar::RenderCore - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pRenderRegion == NULL, FALSE, "TextChar::RenderCore() - pRenderRegion == NULL");

	// If the render region is being used for hit detection then render bounds
	// BODGE TEXT - should be in the render region RenderChar function
	if (pRenderRegion->IsHitDetect() &&
		pRenderRegion->RRQuality.GetLineQuality() != Quality::BlackLine &&
		Tool::GetCurrentID() == TOOLID_TEXT)
	{
		Path* pPath = CreateMetricsRectPath();
	   	ERROR2IF(pPath == NULL, FALSE, "TextChar::RenderCore() - CreateMetricsRectPath() failed");
		pRenderRegion->SaveContext();
		pRenderRegion->SetFillColour(COLOUR_BLACK);
		pRenderRegion->DrawPath(pPath);
		pRenderRegion->RestoreContext();
		delete pPath;
		return TRUE;
	}

	Matrix matrix;
	if (GetStoryAndCharMatrix(&matrix) == FALSE) return FALSE;

	// render the character through the matrix with current attributes in RenderRegion
	// If the render region is a printing region then maybe print as shapes.
	if (pRenderRegion->IsPrinting())
	{
		PrintControl* pCtrl = pRenderRegion->GetRenderView()->GetPrintControl();
		TextStory* pStory = this->FindParentStory();
		if ((pCtrl != NULL && pCtrl->GetTextOptions() == PRINTTEXTOPTIONS_ALLTEXTASSHAPES) ||
			(pStory != NULL && pStory->IsPrintingAsShapes()))
		{
			return pRenderRegion->RenderRegion::RenderChar(GetUnicodeValue(), &matrix);
		}
	}

		// if we're exporting to illustrator, make the text position relative to
		// the page.
	if (pRenderRegion->IsKindOf (CC_RUNTIME_CLASS(AIEPSRenderRegion)))
	{
		// (ChrisG 14/12/00) - Only transform if we're not exporting text as shapes, as this
		// will be done in CreateCharPath - in AIEPSRenderRegion::RenderChar
		if (((AIEPSRenderRegion *) pRenderRegion)->GetTextAsShapes () == FALSE)
		{
			DocCoord pos;
			matrix.GetTranslation(pos);

			Spread* pCurSpread = FindParentSpread();
			if (pCurSpread)
			{
				DocCoord result;
				pCurSpread->SpreadCoordToPagesCoord(&result, pos);
				matrix.SetTranslation(result);
			}
		}
	}

	// Render into the given region region as characters
	BOOL Result =  pRenderRegion->RenderChar(GetUnicodeValue(), &matrix);

#if EXPORT_TEXT

	// Graeme (27/4/00) - This piece of code is here to fix a problem with the export of
	// paragraph text to an EPS varient file. Basically, because only the last line in a
	// paragraph is terminated by a line feed, the export filter isn't recognising that
	// there is a break, and so all the text appears on one line. This new piece of code
	// fixes the problem.

	// This goes horribly wrong if it's printing to distiller, which picks up the paragraph 
	// width anyway, making this only relevant for exporting eps files...
	if (pRenderRegion->IsPrinting () == FALSE)
	{
		// Determine whether this is the last node in a text line.
		if ( FindNextVTNInLine () == NULL )
		{
			// Manually output a newline token.
			Result = pRenderRegion->WriteNewLine ();
		}
	}		// if not printing (i.e. doing export)

#endif	// EXPORT_TEXT

	return Result;
#endif
	return false;
}


/********************************************************************************************
>	void TextChar::RenderEorDrag(RenderRegion* pRenderRegion)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/03/95
	Inputs:		pRenderRegion - region to render the blobs into
	Purpose:	Renders the selection blobs for this TextChar
********************************************************************************************/

void TextChar::RenderEorDrag(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// Before rendering the character we need to render it's applied *TEXT* attributes
	// into the render region as EOR drag render regions dosen't maintain an attribute stack
	CCAttrMap* pAttribMap = new CCAttrMap(30);
	BOOL FoundAttributes = FindAppliedAttributes(pAttribMap);
	
	if (pAttribMap!=NULL)
	{
		pRenderRegion->SaveContext();

		// Render the text attributes
		CCAttrMap::iterator pos = pAttribMap->GetStartPosition();
		while (pos!=NULL)
		{
			CCRuntimeClass *pKey;
			void		   *pVal;
			pAttribMap->GetNextAssoc(pos, pKey, pVal);

			if (((NodeAttribute*)pVal)->IsKindOfTextAttribute())
				((NodeAttribute*)pVal)->Render(pRenderRegion);
		}

		// Render the character
		if (RenderCore(pRenderRegion)==FALSE)
			InformError();

		pRenderRegion->RestoreContext();
	}

	delete pAttribMap;
#endif
}

/********************************************************************************************

>	virtual BOOL TextChar::ExportRender ( RenderRegion *pRegion )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/10/99
	Inputs:		pRegion - A pointer to a RenderRegion.
	Outputs:	-
	Returns:	TRUE if the RenderRegion is a FlashRenderRegion, otherwise FALSE.
	Purpose:	If pRenderRegion points to a FlashRenderRegion, the character will be
				processed as a Flash character, and ultimately stored in the export file
				in the appropriate manner. (As a set of three records: One referring to
				the paths, one mapping the paths onto characters, and one detailling the
				text string itself.

********************************************************************************************/

BOOL TextChar::ExportRender ( RenderRegion *pRegion )
{
	PORTNOTETRACE("text","TextChar::ExportRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL bResult = FALSE;

	// If the RenderRegion is a FlashRenderRegion, use custom Flash export code.
	if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( FlashRenderRegion ) ) )
	{
		// Step 1:	Cast the pRegion pointer to be a FlashRenderRegion pointer, so that
		//			it can access the custom Flash export methods.
		FlashRenderRegion *pFlash = ( FlashRenderRegion* ) pRegion;

		// Step 1a:	If the character supports fill or line types that are unsupported by
		//			Flash, return FALSE, and export them as paths.
		// (Option, may add later.)

		// Step 2:	Pass the character into the appropriate method to export the character.
		//			This is where all the hard work is done.
		bResult = pFlash->ExportCharacter ( this );
	}

	// Step 3:	The export has been done successfully. Return TRUE to prevent Camelot
	//			from rendering the character out as paths as well as text. (If there's
	//			been an error, bResult will have been set to FALSE.)
	return bResult;
#endif
	return false;
}

/********************************************************************************************
>	BOOL TextChar::CreateNodePath(NodePath** ppNodePath, FormatRegion* pFormatRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/95
	Inputs:		pFormatRegion - format region with current attr stack
	Outputs:	ppNodePath    -  pointer to NodePath (may be NULL if none - eg a space!)
	Returns:	FALSE if fails
	Purpose:	Transforms the TextChar into a NodePath
********************************************************************************************/

BOOL TextChar::CreateNodePath(NodePath** ppNodePath, FormatRegion* pFormatRegion)
{
	PORTNOTETRACE("text","TextChar::CreateNodePath - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	ERROR2IF(pFormatRegion==NULL,FALSE,"TextChar::CreateNodePath() - pFormatRegion==NULL");
	ERROR2IF(   ppNodePath==NULL,FALSE,"TextChar::CreateNodePath() - ppNodePath==NULL");

	// get the char's path transformed to the correct place inthe doc
	Matrix matrix;
	if (GetStoryAndCharMatrix(&matrix)==FALSE)
		return FALSE;
	Path* pPath=pFormatRegion->CreateCharPath(GetUnicodeValue(), &matrix);
	if (pPath==NULL)
		return FALSE;
	pPath->InitialiseFlags();

	// Allocate a new NodePath node and copy path data into it
	BOOL ok=TRUE;
	NodePath* pNodePath=NULL;
	if (pPath->GetNumCoords()!=0)
	{
		pNodePath=new NodePath;
		ok=(pNodePath!=NULL);
		if (ok) ok=pNodePath->InkPath.Initialise(pPath->GetNumCoords(),1);
		if (ok) ok=pNodePath->InkPath.CopyPathDataFrom(pPath);
		if (!ok)
		{
			delete pNodePath;
			pNodePath=NULL;
		}
	}

	delete pPath;

	*ppNodePath=pNodePath;
	return ok;
#else
	return FALSE;
#endif
}



/***********************************************************************************************
> 	virtual BOOL TextChar::Snap(DocCoord* pDocCoord)

    Author:     Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
    Created:    19/10/95
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    If the point is snapped then pDocCoord will contain the point of attraction.
    Returns:    TRUE 	- the DocCoord has been snapped to the character's path.
    			FALSE	- the DocCoord has not been processed.
    Purpose:    Snaps to given coord to the nearest point on the characters render path.  If
    			it is not appropriate to snap the coord to the shape (at the moment this means
    			the	coord is too far awawy), then FALSE is returned.
**********************************************************************************************/

BOOL TextChar::Snap(DocCoord* pDocCoord)
{
	BOOL Snapped = FALSE;

	PORTNOTETRACE("text","TextChar::Snap - do nothing");
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	MILLIPOINT SnapDist    = CSnap::GetSnapDist();
	MILLIPOINT SqrSnapDist = SnapDist*SnapDist;

	// create a format region with an attribute stack
	FormatRegion FRegion;
	FormatRegion* pFormatRegion=&FRegion;
	BOOL ok = pFormatRegion->Init(this);
	pFormatRegion->SaveContext();

	// Get the path for this character
	NodePath* pPath = NULL;
	if (ok)
		ok = CreateNodePath(&pPath, pFormatRegion);

	if (ok && (pPath!= NULL) && (pPath->InkPath.GetNumCoords() > 0))
	{
		Snapped = CSnap::SnapCoordToPath(pDocCoord, &(pPath->InkPath));
		delete pPath;
	}

	pFormatRegion->RestoreContext();
#endif
	return Snapped;
}


/********************************************************************************************
>	virtual BOOL TextChar::SupportsClipboardFormat(InternalClipboardFormat *Format) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/4/95
	Returns:	TRUE  = This node supports the given format
				FALSE = This node does not support the format (or the format is unknown)

	Purpose:	Determine if a node supports a given internal data type. This is used
				by the clipboard when exporting to other applications in order to
				determine if certain data types can be supplied.

				e.g. The basic formats include:
					Vector		- this is ALWAYS assumed to be available (Vector format
								  includes every Node, e.g. export in Camelot .art format)

					Text		- As well as paths, some objects can provide text chars

					Bitmap		- Bitmap fills can render a filled object or supply the
								  bitmap used for filling with.

				See InternalClipboardFormat (kernel\cliptype.h) for more details

	Notes:		TextChars can be exported as either "vector" or "text" data
	SeeAlso:	InternalClipboardFormat; Node::SupportsClipboardFormat
********************************************************************************************/

BOOL TextChar::SupportsClipboardFormat(InternalClipboardFormat *Format) const
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	// TextChars can be exported as either "vector" or "text" data
	return(Format->IsSameFormat(InternalClipboardFormat(CLIPTYPE_TEXT)));
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL TextChar::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
	BOOL TextChar::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes a text char record to the new file format filter

********************************************************************************************/

BOOL TextChar::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtChar::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL TextChar::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtChar::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL TextChar::WritePostChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Tells the text node that all it's children have been exported (or skipped if
				CanWriteChildrenWeb() returned FALSE)

				Chars use this to reset their AlreadyWritten flag, because this can only be reset
				after it's children have been skipped

********************************************************************************************/

BOOL TextChar::WritePostChildrenWeb(BaseCamelotFilter* pFilter)
{
	SetAlreadyWritten(FALSE);
	return TRUE;
}

//------------------------------------------------------------------------------
// See TextChar::WritePostChildrenWeb(BaseCamelotFilter *pFilter) for more details

BOOL TextChar::WritePostChildrenNative(BaseCamelotFilter* pFilter)
{
	SetAlreadyWritten(FALSE);
	return TRUE;
}

/********************************************************************************************

>	BOOL TextChar::CanWriteChildrenWeb(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Asks if it's alright to write out the char's children

				Chars will only say no if they've already been written out (checks the AlreayWritten flag)

********************************************************************************************/

BOOL TextChar::CanWriteChildrenWeb(BaseCamelotFilter *pFilter)
{
	return !AlreadyWritten();
}

//------------------------------------------------------------------------------
// See TextChar::CanWriteChildrenWeb(BaseCamelotFilter *pFilter) for more details

BOOL TextChar::CanWriteChildrenNative(BaseCamelotFilter *pFilter)
{
	return !AlreadyWritten();
}

/********************************************************************************************

>	MILLIPOINT TextChar::GetAutoKernSize(FormatRegion* pFormatRegion)

	Author:		Jonathan_Payne (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/2000
	Inputs:		FormatRegion
	Returns:	Kern between char and next char
	Purpose:	Find auto kern between two TextChar and next TextChar in the story

********************************************************************************************/
MILLIPOINT TextChar::GetAutoKernSize(FormatRegion* pFormatRegion)
{
	PORTNOTETRACE("text","TextChar::GetAutoKernSize - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	TextChar* pNextTC = FindNextTextCharInStory();

	if (pNextTC)
	{
		// If we get here, we know there is a TextChar (this) and a following textchar (pNextTC)
		// that might have a kern between them.  Before asking for that kerning value, we have
		// to check that both 'this' and 'pNextTC' are of the same font and have the same bold/
		// italic attribs.  If we did not do this, GetCharsKerning would use the font currently
		// selected into pFormatRegion which is just the font the TextChar 'this'.

		// The basic method I am using to do this is to look at the child attribs of 'this'
		// (don't care about attribs higher up the tree as they will apply to both 'this' and
		// 'pNextTC') and check that pNextTC also has child attribs of the same type.

		// Well it should work...

		// Hopefully...

		Node *Child;			// used to point to child attribs of 'this'
		Node *CompareChild;		// used to point to child attribs of 'pNextTC'

		Child = FindFirstChild();
		while (Child)
		{
			if (IS_A(Child,AttrTxtFontTypeface)	||
				IS_A(Child,AttrTxtItalic)		||
				IS_A(Child,AttrTxtBold)			||
				IS_A(Child,AttrTxtFontSize)		||
				IS_A(Child,AttrTxtScript))
			{
				// 'this' has a child node that could mean it has a difference between
				// it 'pNextTC' that could be important to us.  Now we have to find out firstly
				// 'pNextTC' has this child node and if so if it has the same value.
				CompareChild = pNextTC->FindFirstChild();
				while (CompareChild && Child->GetRuntimeClass() != CompareChild->GetRuntimeClass())
					CompareChild = CompareChild->FindNext();

				if (CompareChild)
				{
					// If we get here we have found an attrib that is shared by 'this' and 'pNextTC'
					// with 'Child' pointing to the attrib of 'this' and 'CompareChild' pointing to the
					// attrib of 'pNextTC'.
					if (Child->IsDifferent(CompareChild))
						return 0; // No kern
				}
				else
				{
					// We found an attrib on 'this' but not on 'pNextTC' so they must be different.
					// That means that we don't kern.
					return 0; // No kern
				}
			}
			
			Child = Child->FindNext();
		}

		// If we get here, we know that 'this' has no attribs that are different from 'pNextTC'.
		// What we don't know is if 'pNextTC' has an attrib that 'this' does not have.  This case
		// is made a little simpler as the mere existence of an attrib should be enough to make the
		// two different so there is no need to compare the attrib values.
		CompareChild = pNextTC->FindFirstChild();
		while (CompareChild)
		{
			if (IS_A(CompareChild,AttrTxtFontTypeface)	||
				IS_A(CompareChild,AttrTxtItalic)		||
				IS_A(CompareChild,AttrTxtBold)			||
				IS_A(CompareChild,AttrTxtFontSize)		||
				IS_A(CompareChild,AttrTxtScript))
			{
				// Found an attrib of 'pNextTC'.  Just need to check that 'this' has this attrib as well
				// (if it does have that attrib, we will have checked they are the same in the IsDifferent()
				// call, above).
				Child = FindFirstChild();
				while (Child && Child->GetRuntimeClass() != CompareChild->GetRuntimeClass())
					Child = Child->FindNext();

				if (!Child)
					return 0; // No kern
			}

			CompareChild = CompareChild->FindNext();
		}

		// Wow, we got through all the tests so I guess we might as well check for a kern now
		// (at last).  Possible optimisation - we could do this at the top of the function and
		// then do all the all the checking afterwards - not sure if this would be faster or not.
		return pFormatRegion->GetCharsKerning(GetUnicodeValue(), pNextTC->GetUnicodeValue());
	}
	else
		return 0; // No kern
#else
	return 0;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
// CaretNode methods

/********************************************************************************************
>	CaretNode::CaretNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple CaretNode constructor, it is required so that SimpleCopy will work.
********************************************************************************************/

CaretNode::CaretNode(): VisibleTextNode() // Call the base class
{
}
 

/********************************************************************************************
>	CaretNode::CaretNode(Node* ContextNode, AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.     
		
				Direction: 
			
				Specifies the direction in which the node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node  
				
	Purpose:	The main CaretNode constructor
********************************************************************************************/

CaretNode::CaretNode(Node* ContextNode,  
		 			 	AttachNodeDirection Direction): VisibleTextNode(ContextNode, Direction)
{
}

/********************************************************************************************
>	Node* CaretNode::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	NULL (See below)
    Purpose:    It is illegal to copy a CaretNode, therefore this function will generate an error
				if called.	
				But blends, moulds etc. call this function so I (Peter) have implemented it
				Fix after the show
********************************************************************************************/

Node* CaretNode::SimpleCopy()
{
	// Make a new CaretNode and then copy things into it
	CaretNode* NodeCopy = new CaretNode();

    ERROR1IF(NodeCopy == NULL, NULL, _R(IDE_NOMORE_MEMORY)); 

    if (NodeCopy != NULL)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}


/********************************************************************************************
>	UINT32 CaretNode::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the CaretNode. 
********************************************************************************************/

UINT32 CaretNode::GetNodeSize() const 
{
	return (sizeof(CaretNode)); 
}


/********************************************************************************************
>	void CaretNode::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void CaretNode::GetDebugDetails(StringBase* Str)
{
#if DEBUG_TREE
	VisibleTextNode::GetDebugDetails(Str);
#endif
}


/********************************************************************************************
>	Path* CaretNode::CreatePath(Matrix* pMatrix)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/395
	Inputs:		pMatrix - transform to be applied to the caret
	Returns:	pointer to new path (or NULL if fails)
	Purpose:	Create a caret path
********************************************************************************************/
								
Path* CaretNode::CreatePath(Matrix* pMatrix)
{	
	ERROR2IF(pMatrix==NULL,NULL,"CaretNode::CreatePath() - pMatrix==NULL");

	TextLine* pTextLine=(TextLine*)FindParent();
	ERROR2IF(pTextLine==NULL,FALSE,"CaretNode::CreatePath() - pTextLine==NULL");

	// find italic status on caret
	NodeAttribute* pAttr;
	BOOL Italic=FALSE;
	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTxtItalic), &pAttr))
		Italic=((TxtItalicAttribute*)(pAttr->GetAttributeValue()))->ItalicOn;

	// find script offest
	FIXED16 ScriptOffset=0;
	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTxtScript), &pAttr))
		ScriptOffset=((TxtScriptAttribute*)(pAttr->GetAttributeValue()))->Offset;

	// find caret height
	MILLIPOINT LineSize  = pTextLine->GetLineSize();
	MILLIPOINT CaretSize = LineSize;
	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTxtFontSize), &pAttr))
		CaretSize=((TxtFontSizeAttribute*)(pAttr->GetAttributeValue()))->FontSize;

	// find baseline shift
//	MILLIPOINT BaseLineShift=0;
//	if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTxtBaseLine), &pAttr))
//		BaseLineShift=((TxtBaseLineAttribute*)(pAttr->GetAttributeValue()))->Value;

	// calc caret top and bottom points accounting for italic and baseline shift
	FIXED16				CaretRatio=(LineSize!=0) ? Div32By32(CaretSize,LineSize) : 1;
	DocCoord			CaretTop(0, XLONG(pTextLine->GetLineDescent()) * CaretRatio );
	DocCoord			CaretBot(0, XLONG(pTextLine->GetLineAscent()) * CaretRatio );
	if (Italic && ScriptOffset==0)
	{
		const double SlantRatio = tan(15.0*PI/180);
		CaretTop.x = (INT32) ((double)CaretTop.y * SlantRatio);
		CaretBot.x = (INT32) ((double)CaretBot.y * SlantRatio);
	}

	// get caret path
	Path* pCaretPath=new Path(); 
	BOOL ok=(pCaretPath!=NULL);
	if (ok) ok=pCaretPath->Initialise(2,12);
	if (ok) ok=pCaretPath->AddMoveTo(CaretTop);
	if (ok) ok=pCaretPath->AddLineTo(CaretBot);

	// apply transform to caret path
	DocCoord* pPathCoords=NULL;
	if (ok) pPathCoords=pCaretPath->GetCoordArray();							     
	if (ok) ok=(pPathCoords!=NULL);
	if (ok)	pMatrix->transform((Coord*)pPathCoords, pCaretPath->GetNumCoords());

	// if not OK, delete caret path and set return path pointer to NULL
	if (!ok)
	{
		delete pCaretPath;
		pCaretPath=NULL;
	}

	return pCaretPath;
} 


/********************************************************************************************
>	void CaretNode::RenderObjectBlobs(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/95
	Inputs:		pRender - The region to render into
	Returns:	FALSE if fails
	Purpose:	Renders the caret's path into the pRender render region
********************************************************************************************/

void CaretNode::RenderObjectBlobs(RenderRegion* pRenderRegion)
{
	if (RenderObjectBlobsCore(pRenderRegion)==FALSE)
		InformError();
}


/********************************************************************************************
>	BOOL CaretNode::RenderObjectBlobsCore(RenderRegion* pRenderRegion)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/2/95
	Inputs:		pRender: The region to render into
	Returns:	FALSE if fails
	Purpose:	Renders the caret's path into the pRender render region
********************************************************************************************/

BOOL CaretNode::RenderObjectBlobsCore(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ERROR2IF(pRenderRegion==NULL,FALSE,"CaretNode::RenderObjectBlobsCore() - pRenderRegion==NULL");

	TextStory* pTextStory = this->FindParentStory();
	ERROR2IF(pTextStory==NULL,FALSE,"CaretNode::RenderObjectBlobsCore() - pTextStory==NULL");

	// get the caret path transformed to the correct place in the doc
	Matrix matrix;
	if (GetStoryAndCharMatrix(&matrix)==FALSE)
		return FALSE;
	Path* pCaretPath=CreatePath(&matrix);
	BOOL ok=(pCaretPath!=NULL);

	// draw the caret path in red (line width?)
	if (ok)
	{
		pRenderRegion->SaveContext();
		pRenderRegion->SetLineColour(RED);
	 	pRenderRegion->DrawPath(pCaretPath);
		pRenderRegion->RestoreContext();
	}

	// delete the caret path
	delete pCaretPath;

	return ok;
#else
	return TRUE;
#endif
}


/*******************************************************************************************
>	virtual BOOL CaretNode::ExportRender(RenderRegion* pRegion)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	14/4/95
 	Inputs:		pRegion - points to the export render region
	Returns:	TRUE if rendered OK (FALSE=>use normal rendering)
 	Purpose:	This function is called when the render function passes through this node
 				It outputs the	Text Object start and end tokens
********************************************************************************************/

BOOL CaretNode::ExportRender(RenderRegion* pRegion)
{
#if EXPORT_TEXT
// BODGE TEXT - need to export caret in a comment!
 	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		EPSExportDC *pDC=(EPSExportDC*)pRegion->GetRenderDC();
// 		pDC->OutputToken("ctk");
//		pDC->OutputNewLine();
		return TRUE;
	}
#endif
	return FALSE;
}


/********************************************************************************************
>	virtual void CaretNode::Describe(BOOL Plural, BOOL Verbose = TRUE) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/95
	Inputs:		Plural - FALSE to describe a single caret
						 TRUE to describe many
	Returns:	String describing what the node is
	Purpose:	For obtaining a desciption about the node to use in the status line
********************************************************************************************/

String CaretNode::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_CARETSDESCRIPTION)));  
	else
		return(String(_R(IDS_CARETDESCRIPTION))); 
}


/********************************************************************************************
>	virtual DocRect CaretNode::GetBlobBoundingRect()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Returns:	the caret's bounds
	Purpose:	get the caret's blob bounds
	Note:		Abuse of bounding box cache should really be for renderable bounds
				but the caret has none!
********************************************************************************************/

DocRect CaretNode::GetBlobBoundingRect()
{
	if (IsBoundingRectValid==FALSE)
		if (ValidateBoundingRect()==FALSE)
			InformError();

	return BlobBounds;
}


/********************************************************************************************
>	virtual DocRect CaretNode::GetBoundingRect(BOOL DontUseAttr=FALSE, BOOL HitTest=FALSE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		DontUseAttr - not used
				HitTest     - not used
	Returns:	the caret's renderable bounds
********************************************************************************************/

DocRect CaretNode::GetBoundingRect(BOOL DontUseAttr, BOOL HitTest)
{
	if (IsBoundingRectValid==FALSE)
		if (ValidateBoundingRect()==FALSE)
			InformError();

	return BoundingRectangle;
}


/********************************************************************************************
>	BOOL CaretNode::ValidateBoundingRect(FormatRegion* pFormatRegion=NULL)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pFormatRegion - not used
	Returns:	FALSE if fails
	Purpose:	recache the caret's renderable and blob bounding rects
********************************************************************************************/

BOOL CaretNode::ValidateBoundingRect(FormatRegion* pFormatRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	BOOL ok=TRUE;

	// get the overall matrix to apply to the caret
	Matrix matrix;
	if (GetStoryAndCharMatrix(&matrix)==FALSE)
		return FALSE;

	// get a zero size rect at the correct position in the doc
	// for the renderable bounds of the caret (uses by ScrollToShow)
	DocCoord temp(0,0);
	matrix.transform((Coord*)&temp,1);
	DocRect RendRect(temp,temp);

	// get the bounds of the caret path for the blob bounds
	Path* pCaretPath=CreatePath(&matrix);
	DocRect BlobRect(0,0,0,0);
	ok=(pCaretPath!=NULL);
	if (ok)
	{
		DocCoord* pCoords=pCaretPath->GetCoordArray();
		PathVerb* pVerbs=pCaretPath->GetVerbArray();
		ok=(pVerbs!=NULL && pCoords!=NULL);
		if (ok)
		{
			GDrawContext *GD = GRenderRegion::GetStaticDrawContext();

			if (GD != NULL)
			{
				ok=!(GD->CalcStrokeBBox((POINT*)pCoords, pVerbs, pCaretPath->GetNumCoords(),
									 (LPRECT)(&BlobRect), 0, 2, CAPS_ROUND, JOIN_ROUND, NULL));
			}

			if (!ok) ERROR2RAW("CaretNode::ValidateBoundingRect() - GDraw_CalcStrokeBBox() failed");
		}
		delete pCaretPath;
	}

	// Inflate the blob bounds, so that the Caret is at least a pixel wide
	if (View::GetCurrent() != NULL)
	{
		INT32 PixWidth = (View::GetCurrent()->GetScaledPixelWidth()).MakeLong();
		BlobRect.Inflate(PixWidth/2);
	}

	// update the cached values
	BoundingRectangle   = RendRect;
	BlobBounds          = BlobRect;
	IsBoundingRectValid = TRUE;
	return ok;
#else
	return TRUE;
#endif
}


/********************************************************************************************
>	BOOL CaretNode::HasMoved();

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/3/95
	Returns:	FALSE if fails
	Purpose:	reposition the caret after it has moved
				If possible to the right of the previous char as the caret takes its attributes
				from this char so the caret's position reflects it's baseline shift correctly
********************************************************************************************/

BOOL CaretNode::HasMoved()
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	AbstractTextChar* pPrevATC=(AbstractTextChar*)(this->FindPrevious(CC_RUNTIME_CLASS(AbstractTextChar)));
	BOOL AfterPrevChar = (pPrevATC!=NULL);

	// if positioning the caret after the previous char and its a kern code
	// and its on a path and a silly size, position the caret before the next char
	if (AfterPrevChar && IS_A(pPrevATC,KernCode))
	{
		TextStory* pTextStory = this->FindParentStory();
		ERROR2IF(pTextStory==NULL,FALSE,"CaretNode::HasMoved() - could not find caret's parent story!");
		if ( pTextStory->GetTextPath()!=NULL && ((KernCode*)pPrevATC)->GetValue().x>1000 )
			AfterPrevChar = FALSE;
	}

	// if positioning caret after last char, use prev char's info plus advance
	// else just use next char's info
	Matrix matrix;
	MILLIPOINT pos=0;
	if (AfterPrevChar)
	{
		const MILLIPOINT advance=pPrevATC->GetVisibleAdvance();
		matrix  = Matrix(advance,0);
		matrix *= pPrevATC->GetMatrix();
		pos     = pPrevATC->GetPosInLine()+advance;
	}
	else
	{
		AbstractTextChar* pNextATC=(AbstractTextChar*)(this->FindNext(CC_RUNTIME_CLASS(AbstractTextChar)));
		ERROR2IF(pNextATC==NULL,FALSE,"CaretNode::HasMoved() - no AbstractTextChar before or after caret!");
		matrix = pNextATC->GetMatrix();
		pos    = pNextATC->GetPosInLine();
	}

	SetMatrix(matrix);
	SetPosInLine(pos);
	InvalidateBoundingRect();
#endif
	return TRUE;
}


/********************************************************************************************
>	virtual BOOL CaretNode::DiscardsAttributeChildren()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/5/95
	Returns:	TRUE
	Purpose:	When attributes are applied to the caret, we cannot generate undo info
				as children of the CaretNode are thrown away in a non-undoable way.
********************************************************************************************/

BOOL CaretNode::DiscardsAttributeChildren() const
{
// TEMP TEST! Allow caret to respond normally to attribute application! [Phil, 14/08/2005]
return FALSE;
//	return TRUE;
}


/********************************************************************************************
>	BOOL CaretNode::DoApplyAttrsTo(UndoableOperation* pUndoOp, VisibleTextNode* pDstVTN, BOOL bDeleteDups)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/7/96
	Inputs:		pDstVTN - VisibleTextNode to apply caret attrs to
	Returns:	FALSE if fails
	Purpose:	Apply attrs on caret to given VTN (on same line) ensuring tree left optimal
********************************************************************************************/

BOOL CaretNode::DoApplyAttrsTo(UndoableOperation* pUndoOp, VisibleTextNode* pDstVTN, BOOL bDeleteDups)
{
	ERROR2IF(pUndoOp==NULL,FALSE,"CaretNode::DoApplyAttrsTo() - pUndoOp==NULL");
	ERROR2IF(pDstVTN==NULL,FALSE,"CaretNode::DoApplyAttrsTo() - pDstVTN==NULL");

	TextLine* pCaretLine = this->FindParentLine();
	ERROR2IF(pCaretLine==NULL,FALSE,"CaretNode::DoApplyAttrsTo() - caret has no TextLine parent");
	TextLine* pDstVTNLine = pDstVTN->FindParentLine();
	ERROR2IF(pDstVTNLine==NULL,FALSE,"CaretNode::DoApplyAttrsTo() - dest VTN has no TextLine parent");
if (bDeleteDups)
{
	ERROR2IF(pDstVTNLine!=pCaretLine,FALSE,"CaretNode::DoApplyAttrsTo() - caret and pDstVTN not on same line");
}

	// determine if attr types on caret are already localised
	AbstractTextChar* pATC = (AbstractTextChar*)(pCaretLine->FindFirstChild(CC_RUNTIME_CLASS(AbstractTextChar)));
	ERROR2IF(pATC==NULL,FALSE,"OpTextFormat::DoInsertCharHelper() - caret line has no AbstaractTextChars");
	BOOL CommonAttrsAffected = !pATC->HasAttrTypesOn(this);

	// if attr types on the caret are not localised, localise them
	AttrTypeSet CaretsAttrTypes;
 	this->AddChildAttrTypesToSet(&CaretsAttrTypes);
	if (CommonAttrsAffected)
		if (!pUndoOp->DoLocaliseForAttrChange(pDstVTN,&CaretsAttrTypes,NULL))
			return FALSE;

// BODGE WORDWRAP - causes dup attrs if applying defalut attr on caret to new char in line of non-default

	// now copy carets attrs to new char, deleting any duplicates
	NodeAttribute* pCaretAttr = (NodeAttribute*)(this->FindFirstChild(CC_RUNTIME_CLASS(NodeAttribute)));
	while (pCaretAttr!=NULL)
	{
		NodeAttribute* pDupAttr = pDstVTN->GetChildAttrOfType(pCaretAttr->GetAttributeType());

		// If we have found a duplicate and we are allowed to delete it, do so
		if (pDupAttr!=NULL && bDeleteDups)
		{
			pDupAttr->CascadeDelete();
			delete pDupAttr;
		}

		// If we didn't find a duplicate or we did and we deleted it then
		// copy the caret's attribute
		if (pDupAttr==NULL || bDeleteDups)
		{
			Node* pAttrCopy; 
			if (!pCaretAttr->NodeCopy(&pAttrCopy))
				return FALSE;
			pAttrCopy->AttachNode(pDstVTN, LASTCHILD);
		}

		pCaretAttr = (NodeAttribute*)(pCaretAttr->FindNext(CC_RUNTIME_CLASS(NodeAttribute)));
	}

	// if attr types on caret were localised, factor them out
 	if (CommonAttrsAffected)
		if (!pUndoOp->DoFactorOutAfterAttrChange(pDstVTN, &CaretsAttrTypes))
			return FALSE;

	return TRUE;
}


/********************************************************************************************
>	virtual void CaretNode::SetSelected(BOOL Status)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		Status - TRUE to select the caret
						 FALSE to deselect it
	Purpose:	Overides the normal SetSelected call so we can ensure that the caret is not
				selected outside the TextTool.
********************************************************************************************/

void CaretNode::SetSelected(BOOL Status)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (!TextTool::IsCurrentTool())
	{
		TextStory* pStory = this->FindParentStory();
		if (pStory != NULL)
			pStory->SetSelected(Status);
	}
	else
		BaseTextClass::SetSelected(Status);
#endif
}


/********************************************************************************************
>	BOOL CaretNode::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
	BOOL CaretNode::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes a caret node record to the new file format filter
********************************************************************************************/

BOOL CaretNode::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
//#ifdef DO_EXPORT
//	return CXaraFileTxtCaret::WritePreChildrenWeb(pFilter, this);
//#else
	return FALSE;
//#endif
}

BOOL CaretNode::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
//#ifdef DO_EXPORT
//	return CXaraFileTxtCaret::WritePreChildrenNative(pFilter, this);
//#else
	return FALSE;
//#endif
}

BOOL CaretNode::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

BOOL CaretNode::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// EOLNode methods

/********************************************************************************************
>	EOLNode::EOLNode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple EOLNode constructor, it is required so that SimpleCopy will work.
********************************************************************************************/

EOLNode::EOLNode(): AbstractTextChar() // Call the base class
{
	Virtual   = FALSE;
}

 
/********************************************************************************************
>	EOLNode::EOLNode(Node* ContextNode, AttachNodeDirection Direction)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/95
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.     
		
				Direction: 
			
				Specifies the direction in which the node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node  
				
	Purpose:	The main EOLNode constructor
********************************************************************************************/

EOLNode::EOLNode(Node* ContextNode,  
 		 			AttachNodeDirection Direction): AbstractTextChar(ContextNode, Direction)
{
	Virtual   = FALSE;
}


/********************************************************************************************
>	Node* EOLNode::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	A copy of the node, or NULL if we are out of memory 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes of Node  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
********************************************************************************************/

Node* EOLNode::SimpleCopy()
{
	// Make a new TextChar and then copy things into it
	EOLNode* NodeCopy = new EOLNode();

    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;

}


/********************************************************************************************
>	virtual void EOLNode::Describe(BOOL Plural, BOOL Verbose = TRUE) 

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/03/95
	Inputs:		Plural - FALSE to describe a single EOLNode
						 TRUE to describe many
	Returns:	String describing what the node is
	Purpose:	For obtaining a desciption about the node to use in the status line
********************************************************************************************/

String EOLNode::Describe(BOOL Plural, BOOL Verbose)
{
	if (Plural)
		return(String(_R(IDS_EOLNODES)));  
	else
		return(String(_R(IDS_EOLNODE))); 
}


/***********************************************************************************************
>	void EOLNode::CopyNodeContents(EOLNode* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	Inputs:           
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
***********************************************************************************************/

void EOLNode::CopyNodeContents(EOLNode* NodeCopy)
{
	// Ask the base class to do its bit
	AbstractTextChar::CopyNodeContents( NodeCopy );

	// Copy the rest of the data in here
	NodeCopy->Virtual   = Virtual;
}


/***********************************************************************************************
>   void EOLNode::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void EOLNode::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, EOLNode), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, EOLNode))
		CopyNodeContents((EOLNode*)pNodeCopy);
}



/********************************************************************************************
>	UINT32 EOLNode::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
********************************************************************************************/

UINT32 EOLNode::GetNodeSize() const 
{
	return (sizeof(EOLNode)); 
}


/********************************************************************************************
>	void EOLNode::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void EOLNode::GetDebugDetails(StringBase* pStr)
{
#if DEBUG_TREE
	AbstractTextChar::GetDebugDetails(pStr);

	String_256 TempStr;

	switch (IsVirtual())
	{
		case TRUE:  TempStr._MakeMsg(TEXT("\r\nIs virtual"));  break;
		case FALSE: TempStr._MakeMsg(TEXT("\r\nIs physical")); break;
	}
	*pStr += TempStr;
#endif
}

	
/********************************************************************************************
>	virtual DocRect EOLNode::GetBoundingRect(BOOL DontUseAttr=FALSE, BOOL HitTest=FALSE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		DontUseAttr - not used (EOL is not rendered)
				HitTest     - TRUE if being called during HitTest
	Returns:	the EOL bounds (zero size but in correct position
	Purpose:	Get the EOL's bounds
				If the cached bounds are invalid, recalculate them
********************************************************************************************/

DocRect EOLNode::GetBoundingRect(BOOL DontUseAttr, BOOL HitTest)
{
	if (IsBoundingRectValid==FALSE)
		if (ValidateBoundingRect()==FALSE)
			InformError();

	return BoundingRectangle;
}


/********************************************************************************************
>	BOOL EOLNode::ValidateBoundingRect(FormatRegion* pFormatRegion=NULL)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/3/95
	Inputs:		pFormatRegion - not used
	Returns:	FALSE if fails
	Purpose:	calculate the EOL bounds in the document
********************************************************************************************/

BOOL EOLNode::ValidateBoundingRect(FormatRegion* pFormatRegion)
{
	Matrix matrix;
	if (GetStoryAndCharMatrix(&matrix)==FALSE)
		return FALSE;

	// get a zero size rect at correct place in the doc for the renderable bounds of the EOL
	DocCoord temp(0,0);
	matrix.transform(&temp);
	BoundingRectangle   = DocRect(temp,temp);
	IsBoundingRectValid = TRUE;
	return TRUE;
}


/********************************************************************************************
>	virtual DocRect EOLNode::GetBlobBoundingRect()

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Returns:	the EOLNode's blob bounds
	Purpose:	get the EOLNode's blob bounds
********************************************************************************************/

DocRect EOLNode::GetBlobBoundingRect()
{
	Path RenderPath;
	DocRect	Bounds(0,0,1,1);
	if (GetBlobPath(&RenderPath))
		Bounds = RenderPath.GetBoundingRect();

	return Bounds;
}


/********************************************************************************************
>	void EOLNode::RenderObjectBlobs(RenderRegion* pRenderRegion)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		pRenderRegion - region to render the blobs into
	Purpose:	Renders the selection blobs for an EOLNode
********************************************************************************************/

void EOLNode::RenderObjectBlobs(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	Path RenderPath;
	if (GetBlobPath(&RenderPath))
	{
		DocColour Trans(COLOUR_TRANS);
		pRenderRegion->SetLineWidth(0);
		pRenderRegion->SetLineColour(Trans);
		pRenderRegion->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRenderRegion->DrawPath(&RenderPath);
	}
	else
		InformError();
#endif
}


/********************************************************************************************
>	void EOLNode::RenderTinyBlobs(RenderRegion* pRenderRegion)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		pRenderRegion - region to render the blobs into
	Purpose:	Renders the selection blobs for an EOLNode
********************************************************************************************/

void EOLNode::RenderTinyBlobs(RenderRegion* pRenderRegion)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	RenderObjectBlobs(pRenderRegion);
#endif
}


/********************************************************************************************
>	BOOL EOLNode::GetBlobPath(Path*)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		Pointer to a path
	Returns:	TRUE/FALSE for success/failure
	Purpose:	Builds a path for rendering the blobs for an EOLNode
********************************************************************************************/

BOOL EOLNode::GetBlobPath(Path* pPath)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	ERROR2IF(pPath==NULL, FALSE, "Path pointer was NULL");
	TextLine* pParentLine = this->FindParentLine();
	ERROR2IF(pParentLine==NULL, FALSE, "EOLNode didn't have a parent TextLine");
	const MILLIPOINT LineAscent = pParentLine->GetLineAscent();
	const MILLIPOINT HalfLA     = LineAscent/2;
	const MILLIPOINT QuarterLA  = LineAscent/4;
	BOOL ok = TRUE;
	if (ok)	ok = pPath->Initialise(4,1);
	if (ok)	ok = pPath->AddMoveTo(DocCoord(QuarterLA,0));
	if (ok)	ok = pPath->AddLineTo(DocCoord(QuarterLA, HalfLA));
	if (ok)	ok = pPath->AddLineTo(DocCoord(HalfLA+QuarterLA, QuarterLA));
	if (ok)	ok = pPath->AddLineTo(DocCoord(QuarterLA,0));
	if (ok)
	{
		ok = pPath->CloseSubPath();
		pPath->IsFilled = TRUE;
	}

	// Transform the path
	if (ok)
	{
		Matrix matrix;
		if (ok)	ok = GetStoryAndCharMatrix(&matrix);
		if (ok)	matrix.transform((Coord*)(pPath->GetCoordArray()), pPath->GetNumCoords());
	}

	return ok;
#else
	return FALSE;
#endif
}
/********************************************************************************************

>	BOOL EOLNode::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
	BOOL EOLNode::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes an end of line record to the new file format filter

********************************************************************************************/

BOOL EOLNode::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtEOL::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL EOLNode::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtEOL::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL EOLNode::CanWriteChildrenWeb(BaseCamelotFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/8/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Asks if it's alright to write out the EOL's children

				EOLs always say yes.  EOL nodes can't be female then.

********************************************************************************************/

BOOL EOLNode::CanWriteChildrenWeb(BaseCamelotFilter *pFilter)
{
	return TRUE;
}

//------------------------------------------------------------------------------
// See EOLNode::CanWriteChildrenWeb(BaseCamelotFilter *pFilter) for more details

BOOL EOLNode::CanWriteChildrenNative(BaseCamelotFilter *pFilter)
{
	return TRUE;
}

/*******************************************************************************************
>	virtual BOOL EOLNode::ExportRender(RenderRegion* pRegion)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	14/4/95
 	Inputs:		pRegion - points to the export render region
	Returns:	TRUE if rendered OK (FALSE=>use normal rendering)
 	Purpose:	This function is called when the render function passes through this node
 				It outputs the Text Object start and end tokens
********************************************************************************************/

BOOL EOLNode::ExportRender(RenderRegion* pRegion)
{
#if EXPORT_TEXT

	// (ChrisG - 3/11/00)
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(AIEPSRenderRegion)))
	{
		// if we were exporting the overflow data (i.e. we were exporting a path), then 
		//	finish off exporting this
		if (((AIEPSRenderRegion *) pRegion)->ExportingOnPath ())
			((AIEPSRenderRegion *) pRegion)->OverflowTextFinish ();
	}

	return pRegion->WriteNewLine ();
#else
	return FALSE;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////
// KernCode methods

/********************************************************************************************
>	KernCode::KernCode()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Purpose:	Simple KernCode constructor, it is required so that SimpleCopy will work.
				You should not normally call this constructor as it does not initialise
				the object.
********************************************************************************************/

KernCode::KernCode(): AbstractTextChar()
{
}

/********************************************************************************************
>	KernCode::KernCode(DocCoord& KernValue)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Inputs:		KernValue: Horizontal/Vertical kern value                  
	Purpose:	KernCode constructor
********************************************************************************************/

KernCode::KernCode(DocCoord& KernValue): AbstractTextChar()
{
	Value = KernValue;	
}

 
/********************************************************************************************
>	KernCode::KernCode(Node* ContextNode, AttachNodeDirection Direction, DocCoord& KernValue)

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
				
				Value: Horizontal/Vertical kern value                  

	Purpose:	The main KernCode constructor
********************************************************************************************/

KernCode::KernCode(Node* ContextNode, AttachNodeDirection Direction, 
		 		   DocCoord& KernValue): AbstractTextChar(ContextNode, Direction)
{
	Value = KernValue;
}


/*******************************************************************************************
>	virtual BOOL KernCode::ExportRender(RenderRegion* pRegion)

 	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
 	Created:	14/4/95
 	Inputs:		pRegion - points to the export render region
	Returns:	TRUE if rendered OK (FALSE=>use normal rendering)
 	Purpose:	This function is called when the render function passes through this node
 				It outputs the	Text Object start and end tokens
********************************************************************************************/

BOOL KernCode::ExportRender(RenderRegion* pRegion)
{
#if EXPORT_TEXT
 	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output any valid text attributes necessary
		((EPSRenderRegion*)pRegion)->GetValidPathAttributes();
		((EPSRenderRegion*)pRegion)->GetValidTextAttributes();

		EPSExportDC *pDC=(EPSExportDC*)pRegion->GetRenderDC();

		// Use illustrator 3.0 compatible token.
		// Any vertical movement is done by BaselineShift
		INT32 autokern = 0;
		pDC->OutputValue(autokern);
		pDC->OutputValue(-(Value.x));
 		pDC->OutputToken("Tk");
		pDC->OutputNewLine();
		return TRUE;
	}
#endif
	return FALSE;
}


/********************************************************************************************
>	Node* KernCode::SimpleCopy()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	A copy of the node, or NULL if we are out of memory 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes of Node  
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
********************************************************************************************/

Node* KernCode::SimpleCopy()
{
	// Make a new BigChar and then copy things into it
	KernCode* NodeCopy = new KernCode();

    ERROR1IF(NodeCopy == NULL, NULL, _R(IDE_NOMORE_MEMORY)); 

    if (NodeCopy)
    	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}


/***********************************************************************************************
>	void KernCode::CopyNodeContents(KernCode* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
***********************************************************************************************/

void KernCode::CopyNodeContents(KernCode* NodeCopy)
{
	// Ask the base class to do its bit
	AbstractTextChar::CopyNodeContents( NodeCopy );
	// Copy the rest of the data in here
	NodeCopy->Value = Value;
}


/***********************************************************************************************
>   void KernCode::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void KernCode::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, KernCode), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, KernCode))
		CopyNodeContents((KernCode*)pNodeCopy);
}



/********************************************************************************************
>	UINT32 KernCode::GetNodeSize() const 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
********************************************************************************************/

UINT32 KernCode::GetNodeSize() const 
{
	return (sizeof(KernCode)); 
}


/********************************************************************************************
>	void KernCode::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/2/95
	Outputs:	Str: String giving debug info about the node
	Purpose:	For obtaining debug information about the Node. 
				This fn can be deleted before we ship 
********************************************************************************************/

void KernCode::GetDebugDetails(StringBase* Str)
{
#if DEBUG_TREE
	AbstractTextChar::GetDebugDetails(Str);
	String_256 TempStr;
		
	TempStr._MakeMsg( TEXT( "\r\nKernValue = (#1%u, #2%u)"), Value.x, Value.y);
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	BOOL KernCode::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
	BOOL KernCode::WritePreChildrenNative(BaseCamelotFilter *pFilter)

	Author:		Andy_Hayward (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/07/96
	Inputs:		pFilter - new file format filter to write record to
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:	Writes a kern code record to the new file format filter

********************************************************************************************/

BOOL KernCode::WritePreChildrenWeb(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtKern::WritePreChildrenWeb(pFilter, this);
#else
	return FALSE;
#endif
}

BOOL KernCode::WritePreChildrenNative(BaseCamelotFilter *pFilter)
{
#ifdef DO_EXPORT
	return CXaraFileTxtKern::WritePreChildrenNative(pFilter, this);
#else
	return FALSE;
#endif
}
