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

// NodeRenderableChapter class


#include "camtypes.h"
#include "chapter.h"
//#include "simon.h"
//#include "mario.h"
//#include "tim.h"
#include "ensure.h"
#include "spread.h"

#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"
#include "camfiltr.h"

CC_IMPLEMENT_DYNAMIC(Chapter, NodeRenderablePaper)          

 
/*********************************************************************************************

>    Chapter::Chapter() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a Chapter linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

Chapter::Chapter(): NodeRenderablePaper()
{   
	FoldLineXCoord = 0;
	ShowFoldLine = FALSE;
}



/***********************************************************************************************

>   Chapter::Chapter
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,   
		MILLIPOINT FldLineXCoord, 
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
	)

    Author:  Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created: 26/4/93             
    
    Inputs: ContextNode: Pointer to a node which this node is to be attached to.     
    
			Direction: 
			
				Specifies the direction in which this node is to be attached to the 
				ContextNode. The values this variable can take are as follows: 
								  
				PREV      : Attach node as a previous sibling of the context node
				NEXT      : Attach node as a next sibling of the context node
				FIRSTCHILD: Attach node as the first child of the context node
				LASTCHILD : Attach node as a last child of the context node                               
					      
			FldLineXCoord: X Coordinate of the main fold line for the spreads in the chapter
								  
			The remaining inputs specify the status of the node: 
			
			Locked:     Is node locked ?
			Mangled:    Is node mangled ?
			Marked:     Is node marked ?
			Selected:   Is node selected ?
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the node and links it to ContextNode in the
			 direction specified by Direction. All neccesary tree links are
			 updated.     
			 
    Errors:  An assertion error will occur if ContextNode is NULL


***********************************************************************************************/

Chapter::Chapter(Node* ContextNode,  
					AttachNodeDirection Direction,   
					MILLIPOINT FldLineXCoord, 
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected 
		   		   ): NodeRenderablePaper(ContextNode, Direction, Locked, Mangled, 
									Marked, Selected)
{
	FoldLineXCoord = FldLineXCoord;    
	ShowFoldLine = TRUE;
}   


/********************************************************************************************

>	virtual String Chapter::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Returns:	Description of the object 
	Purpose:	To return a description of the Node object in either the singular or the 
				plural. This method is called by the DescribeRange method.
				
				The description will always begin with a lower case letter.   
				
	Errors:		A resource exception will be thrown if a problem occurs when loading the 
				string resource. 
	SeeAlso:	-

********************************************************************************************/
/*
	Technical Notes:
	
	The String resource identifiers should be of the form: ID_<Class>_DescriptionS for the 
	singular description and ID_<Class>_DescriptionP for the plural. 
*/              
              
String Chapter::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_CHAPTER_DESCRP)));  
	else
		return(String(_R(IDS_CHAPTER_DESCRS))); 
}; 
 

/***********************************************************************************************

>	Chapter* Chapter::FindPreviousChapter()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/93
	Inputs:		-
	Outputs:	-
	Returns:	The previous sibling chapter or NULL if there are no more chapters
	Purpose:	For finding the previous sibling chapter	
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Chapter* Chapter::FindPreviousChapter() 
{ 
	Node* CurrentNode = FindPrevious(); 
	while (CurrentNode != 0)
	{
		if (CurrentNode->IsChapter())
			return (Chapter*) CurrentNode;	  
		CurrentNode = CurrentNode->FindPrevious(); 
	}
	return 0; // No chapter found      
}                                              


/***********************************************************************************************

>	void Chapter::Render( RenderRegion* )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/5/93

***********************************************************************************************/

void Chapter::Render( RenderRegion* )
{
}   

/***********************************************************************************************

>	Spread* Chapter::FindFirstSpread()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Inputs:		-
	Outputs:	-
	Returns:	Returns the first spread in the chapter (or NULL if there are no spreads)
	Purpose:	Returns the first spread which is an immediate child of the chapter
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Spread* Chapter::FindFirstSpread()
{
	Node* pNode = FindFirstChild();
	while(pNode != 0 && !pNode->IsSpread())
		pNode = pNode->FindNext();

	return (Spread*) pNode;
}

/***********************************************************************************************

>	void Chapter::SetFoldLineXCoord(MILLIPOINT value, BOOL DisplayFoldLine = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93

	Inputs:		value: New value for the X coordinate of the chapters fold line
				DisplayFoldLine - TRUE if the fold line should be displayed
								  FALSE to disable display of the fold line altogether

	Purpose:    For setting the X coordinate of the chapters fold line, and enabling/disabling
				the display of the fold line (for single page spreads the fold line is disabled)

	Notes:		The fold line is rendered in spread.cpp

				After setting this value, you'll need to redraw the document appropriately

	SeeAlso:	Chapter::ShouldShowFoldLine; Chapter::GetFoldLineXCoord

***********************************************************************************************/

void Chapter::SetFoldLineXCoord(MILLIPOINT value, BOOL DisplayFoldLine)
{
	FoldLineXCoord = value;
	ShowFoldLine = DisplayFoldLine;
}



/***********************************************************************************************

>	BOOL Chapter::ShouldShowFoldLine(void) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/95

	Returns:	TRUE if the chapter fold line should be displayed
				FALSE if the chapter fold line should not be displayed

	Purpose:	To determine if the chapter fold line is intended to be displayed

	SeeAlso:	Chapter::SetFoldLineXCoord

***********************************************************************************************/

BOOL Chapter::ShouldShowFoldLine(void) const
{
	return(ShowFoldLine);
}



/***********************************************************************************************

>	MILLIPOINT Chapter::GetFoldLineXCoord() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		-
	Outputs:	-
	Returns:	The value of the X coordinate of the chapters fold line   
	Purpose:    For finding the X coordinate of the chapters fold line
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

MILLIPOINT Chapter::GetFoldLineXCoord() const
{
	return (FoldLineXCoord);
}



/***********************************************************************************************
> Node* Chapter::SimpleCopy() 

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:         - 
    Outputs:    
    Returns:    A copy of the node, or NULL if memory runs out 
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
   	       
	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
 
        
    Scope:      protected  
**********************************************************************************************/

Node* Chapter::SimpleCopy()
{
	Chapter* NodeCopy; 
	NodeCopy = new Chapter();       
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}    

/***********************************************************************************************
>   void Chapter::PolyCopyNodeContents(Chapter* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void Chapter::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, Chapter), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, Chapter))
		CopyNodeContents((Chapter*)pNodeCopy);
}



/***********************************************************************************************
>   void Chapter::CopyNodeContents(Chapter* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void Chapter::CopyNodeContents(Chapter* NodeCopy)
{                         
	ENSURE(NodeCopy != NULL,"Trying to copy node contents to\n"
							"a node pointed to by a NULL pointer"); 
	NodeRenderablePaper::CopyNodeContents(NodeCopy); 
	NodeCopy->FoldLineXCoord = FoldLineXCoord; 
}              


/********************************************************************************************

>	XLONG Chapter::GetChapterDepth()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/93
	Returns:	Depth of the Chapter
	Purpose:	Find the depth of a chapter in logical coords.
				i.e. how far from the top of the document the top of the chapter's
				pasteboard is.

********************************************************************************************/

XLONG Chapter::GetChapterDepth()
{
	XLONG Depth	= 0;

	// Loop through document tree calculating the logical coordinate offset for the
	// current chapter 	

//	Chapter *pChapter = Node::FindFirstChapter(FindOwnerDoc());
	Node* pNode = FindParent();
	ERROR2IF(!(pNode->IsNodeDocument()), 0, "Parent of Chapter is not NodeDocument");
	Chapter *pChapter = (Chapter*)pNode->FindFirstChild(CC_RUNTIME_CLASS(Chapter));

	ENSURE(pChapter != NULL, "Couldn't find first chapter in Chapter::GetChapterDepth");
	
	while ((pChapter != NULL) && (pChapter != this))
	{		
                                         				
		ENSURE(pChapter->IsKindOf(CC_RUNTIME_CLASS(Chapter)), 
				"Chapter's sibling is not a Chapter");

		const DocRect ChapRect = pChapter->GetPasteboardRect();
            
        // Accumulate logical offset
		Depth += ChapRect.Height();						
			
		pChapter = (Chapter *) pChapter->FindNext();
	}

	return Depth;
}



#ifdef _DEBUG

void Chapter::ShowDebugTreeDetails() const
{                                 
	TRACE( _T("Chapter ") );
	Node::ShowDebugTreeDetails(); 
}            

#endif 

/********************************************************************************************

>	void* Chapter::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void Chapter::GetDebugDetails(StringBase* Str) 
{          
	NodeRenderablePaper::GetDebugDetails(Str); 
	String_256 TempStr; 
	TempStr._MakeMsg(TEXT("\r\nFoldLine X Coord = #1%ld\r\n"), 
					 FoldLineXCoord); 
	(*Str)+=TempStr;      
}
 				 
/********************************************************************************************

>	virtual UINT32 Chapter::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 Chapter::GetNodeSize() const 
{     
	return (sizeof(Chapter)); 
}  


/*********************************************************************************************

>    BOOL Chapter::IsChapter(void) const

     Author:    Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
     Created:   28/04/95
     Returns:   TRUE
     Purpose:   For finding if a node is a chapter node.

**********************************************************************************************/
       
BOOL Chapter::IsChapter() const
{ 
	return TRUE;
}

/********************************************************************************************

  >	virtual BOOL Chapter::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Web files don't write out a chapter records.

				This code assumes the document will only contain one chapter

	SeeAlso:	CanWriteChildrenWeb(), WritePostChildrenWeb()

********************************************************************************************/

BOOL Chapter::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

/********************************************************************************************

  >	virtual BOOL Chapter::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pFilter = ptr to filter to write to
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Writes out a chapter record.

	SeeAlso:	CanWriteChildrenWeb(), WritePostChildrenWeb()

********************************************************************************************/

BOOL Chapter::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	BOOL RecordWritten = FALSE;

	// Always write out the spread record in native files
	CXaraFileRecord Rec(TAG_CHAPTER,TAG_CHAPTER_SIZE);
	if (pFilter->Write(&Rec) != 0)
		RecordWritten = TRUE;
	else
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return RecordWritten;
#else
	return FALSE;
#endif
}


/********************************************************************************************

> 	BOOL Chapter::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Begins the child record sequence for chapter in the web format

				Web export doesn't write out chapter records, so this overrides the default
				behaviour in Node by ensuring the Down record does not get written

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Chapter::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

/********************************************************************************************

> 	BOOL Chapter::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Ends the child record sequence for chapter in the web format

				Web export doesn't write out chapter records, so this overrides the default
				behaviour in Node by ensuring the Up record does not get written

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Chapter::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

//-------------------------------------------------------------------------
// Stubs put here in case they need implementing
BOOL Chapter::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
	return Node::WriteBeginChildRecordsNative(pFilter);
}

BOOL Chapter::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
	return Node::WriteEndChildRecordsNative(pFilter);
}

