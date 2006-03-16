// $Id: page.cpp 662 2006-03-14 21:31:49Z alex $
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

// The Page class


#include "camtypes.h"
#include "page.h"
//#include "simon.h"
//#include "mario.h"
#include "ensure.h"
#include "exceptio.h"		// For BlowUpOnCrashMe() method      
#include "rndrgn.h"
#include "snap.h"
#include "nodedoc.h"
#include "document.h"
#include "ralphint.h"
#include "spread.h"
#include "layer.h"

//static INT32 	abs(INT32 n) 		{ if (n<0) return (0-n); else return (n);}

DocColour Page::PageColour(COLOUR_WHITE);	// Initial colour of a page (white)
											// Note - will NOT colour-separate

#ifdef RALPH
#define NO_PAGE 1
#endif

CC_IMPLEMENT_DYNAMIC(Page, NodeRenderablePaper)          

/*********************************************************************************************

>    Page::Page() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a Page linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

Page::Page(): NodeRenderablePaper()
{   
}                    
    
  
/***********************************************************************************************
>   Page::Page
    (
    	Node* ContextNode,  
		AttachNodeDirection Direction,
		const DocRect& PasteRect, 
		MILLIPOINT PageWidth, 
		MILLIPOINT PageHeight,  
		BOOL Locked=FALSE, 
		BOOL Mangled=FALSE,  
		BOOL Marked=FALSE, 
		BOOL Selected=FALSE 
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
    
    		PasteRect : Page's pasteboard rectangle
			PageWidth : The width of the page
			PageHeight: The height of the page
					      
			The remaining inputs specify the status of the node: 
			
			Locked:     Is node locked ?
			Mangled:    Is node mangled ?
			Marked:     Is node marked ?
			Selected:   Is node selected ?
			
    Outputs:   -
    Returns:   - 
    Purpose: This method initialises the page node and links it to ContextNode in the
			 direction specified by Direction. The sizes of the pasteboard rectangles of all 
			 paper nodes above this page are recalculated, and the document extents changed.  
			 			 
    Errors:  An assertion error will occur if ContextNode is NULL

***********************************************************************************************/

Page::Page
(
   	Node* ContextNode,  
	AttachNodeDirection Direction,
	const DocRect& NewPageRect, 
	BOOL Locked, 
	BOOL Mangled,  
	BOOL Marked, 
	BOOL Selected
):NodeRenderablePaper(ContextNode, Direction, Locked, Mangled, 
									Marked, Selected)     
{
	PageRect = NewPageRect;
}                                                      

/********************************************************************************************

>	virtual String Page::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/6/93
	Inputs:		Plural: Flag indicating if the string description should be plural or
						singular. 
	Outputs:	-
	Retuns:		Description of the object 
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
              
String Page::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_PAGE_DESCRP)));  
	else
		return(String(_R(IDS_PAGE_DESCRS))); 
}; 
 

/***********************************************************************************************

>	void Page::Render()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/93
	Inputs:		-
	Outputs:	-
	Returns:	- 
	Purpose:    Renders page items: White page rectangle, page divider, print margin, and
									grid.  
				
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

void Page::Render( RenderRegion* pRender )
{      
//#pragma message( __LOCMSG__ "Page::Render - do nothing" )
//	TRACE( _T("Warning - Page::Render called\n") );
	// If we should blow up, then blow up - don't worry, this is a very quick inline check
//	OpException::BlowUpOnCrashMe();

	// Sanity check
	ERROR3IF(pRender == NULL,"Page::Render null render region!");
	if (pRender == NULL)
		return;

	// If we have the new page background layer present then we needn't bother redrawing the
	// page as we might get a flash of white as we redraw
	Node * pCurrent = FindParent();
	if (pCurrent->GetRuntimeClass() == CC_RUNTIME_CLASS(Spread))
	{
		// We have found the spread
		Spread * pSpread = (Spread *)pCurrent;
		// Is there a page background layer on the spread?
		Layer* pLayer = pSpread->FindFirstPageBackgroundLayer();
		// Yes, the layer is present but is it visible and does the quality level 
		// dictate that we should show it?
		if (
			pLayer && pLayer->IsVisible() &&
			(pRender->RRQuality.GetFillQuality() >= Quality::NoFill)
			)
			return;
	}

// This conditional replaces the correct code (directly below in the IF clause) in favour
// of a bodged version that renders the single pixel page outline using filled rectangles
// (the code in the ELSE clause). This is required because although GDI and GDraw fills
// can be made to match up, their outlines can't (at the time of writing).
//
// By the way, Gavin says, it's faster to plot four upright rects than to stroke a path
// to become the outline of the page!!!
//
#if 0
	// Set up attributes for drawing page rectangle
	pRender->SetLineWidth(0); 					// Means single-pixel lines
	pRender->SetLineColour(DocColour(BLACK)); 
	pRender->SetFillColour(PageColour);   

	// Draw a rectangle covering the page 
	pRender->DrawRect(&PageRect);

#else
	// Draw a rectangle covering the page 
	// Set up attributes for drawing page rectangle
	pRender->SetLineWidth(0); 					// Means single-pixel lines
	pRender->SetLineColour(COLOUR_TRANS); 

// randomly change page colour to see which bit is redrawing
#if 0
	static COLORREF pc = 0x00000000;
	switch (pc)
	{
		case 0x007FFFFF: pc = 0x00FF7FFF; break;
		case 0x00FF7FFF: pc = 0x00FFFF7F; break;
		case 0x00FFFF7F: pc = 0x007FFFFF; break;
		default: pc = 0x007FFFFF;
	}
	pRender->SetFillColour(DocColour((pc>>16)&0xFF,(pc>>8)&0xFF,pc&0xFF));
#else
	pRender->SetFillColour(PageColour);
#endif
	pRender->DrawRect(&PageRect);


#ifndef NO_PAGE
	// This is the default non-Ralph behaviour

	pRender->SetFillColour(COLOUR_BLACK);
	pRender->DrawPixelRect(&PageRect);
#else
	// Ralph Docs only render paper in some modes
	
	//find the parent NodeDocument 
	Node* pCurrentNode = FindParent(); 
	while(pCurrentNode != NULL)   
	{
		if (pCurrentNode->IsNodeDocument())
			break; 	 
		pCurrentNode = pCurrentNode->FindParent(); 
	}
	BaseDocument * pDoc =NULL;
	// get a Document *
	if(pCurrentNode)
		pDoc = ((NodeDocument*)(pCurrentNode))->GetParentDoc(); 
	// should we render paper ?
	if(((Document*)pDoc)->RalphDontShowPaper()==FALSE)
	{
		pRender->SetFillColour(COLOUR_BLACK);
		pRender->DrawPixelRect(&PageRect);
	}
#endif // NO_PAGE

#endif // simple redraw or redraw lines and page as filled rects
	
	//Draw print margin
	//if Document::PrintSpreads     *****
	//DocColour Red(0.87,0,0)
	// DocColour Transparent(TRANS);  
	
	//obtain the printer margin from the PrintView
	// pCurrentRenderRegion->DrawRect(&PrintMarginRect)
	
	// The rendering of the grid will have to wait until the grid exists.
}  

/***********************************************************************************************
> Node* Page::SimpleCopy() // Private method  

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

Node* Page::SimpleCopy()
{
	Page* NodeCopy; 
	NodeCopy = new Page();   
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}          

/***********************************************************************************************
>   void Page::CopyNodeContents(Page* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:		- 
	Outputs:    A copy of this node
 
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/


void Page::CopyNodeContents(Page* NodeCopy)
{
	ENSURE(NodeCopy != NULL,"Trying to copy a page's contents to a NULL node");  
	NodeRenderablePaper::CopyNodeContents(NodeCopy); 
 	NodeCopy->PageRect = PageRect;
 	NodeCopy->PageColour = PageColour;           
}


/***********************************************************************************************
>   void Page::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void Page::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, Page), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, Page))
		CopyNodeContents((Page*)pNodeCopy);
}



/*********************************************************************************************

>    const DocRect& Page::GetPageRect() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   The pages rectangle
     Returns:   -
			                 
     Purpose:   For obtaining the page rectangle   
			
     Errors:    

**********************************************************************************************/
 

const DocRect& Page::GetPageRect(void) const
{
	return (PageRect); 
}                              

/*********************************************************************************************

>    BOOL Page::SetPageRect(DocRect)

     Author:    Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
     Created:   14/12/93
     Inputs:    The pages rectangle
     Outputs:   -
     Returns:   -
			                 
     Purpose:   For setting up a new page rectangle   
			
     Errors:    

**********************************************************************************************/
 

BOOL Page::SetPageRect(DocRect NewPageRect)
{
	PageRect = NewPageRect;

	return TRUE; 
}                              



/**********************************************************************************************

>	Page* Page::FindNextPage(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		-
	Outputs:	-
	Returns:	The next sibling page of this node, or NULL if there are none.   
	Purpose:    To find the next sibling page
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Page* Page::FindNextPage(void) 
{
	Node* CurrentNode = FindNext(); 
	while (CurrentNode != NULL)
	{
		if(CurrentNode->IsKindOf(CC_RUNTIME_CLASS(Page)))
			return ((Page*) CurrentNode);	  
		CurrentNode = CurrentNode->FindNext(); 
	}
	return (NULL); 							// No page found
}     

/***********************************************************************************************

>	Page* Page::FindRightPage(void) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/93
	Inputs:		-
	Outputs:	-
	Returns:	If this page has another page joined to its right hand edge, then a 
				pointer to the right page is returned, else NULL is returned. 
				
	Purpose:	To find the page joined to the right of this page. 
	Errors:		-
	SeeAlso:	Page::BottomPage
***********************************************************************************************/

Page* Page::FindRightPage(void) 
{  
	Page* SiblingPage = FindNextPage();      
	 
	if (SiblingPage != NULL) 
	{   
		const DocRect& SiblingPageRect = SiblingPage->GetPageRect();  
			
		//For two pages joined together horizontally the hi.x of the left-hand page must 
		//be equal to the lo.x of the right hand page, also the two pages will have 
		//identical y coordinates (i.e. they have the same height and are in the same place
		//vertically. 
		
		// This can be optimised **
		if (((PageRect.HighCorner().x) == (SiblingPageRect.LowCorner().x)) &&
	    	((PageRect.LowCorner().y ) == (SiblingPageRect.LowCorner().y)) &&
		    ((PageRect.HighCorner().y) == (SiblingPageRect.HighCorner().y))
		   )	 
			return (SiblingPage); 
		else
			return (NULL); 	 			 
	}        
	return (NULL);
}                  

/***********************************************************************************************

>	Page* Page::FindBottomPage(void)  

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/93
	Inputs:		-
	Outputs:	-
	Returns:	If this page has another page joined beneath it, then a pointer to the 
				bottom page is returned, else NULL is returned. 
				
	Purpose:    To find the page joined to the bottom of this page.   
	Errors:		-
	SeeAlso:	Page::RightPage

***********************************************************************************************/

Page* Page::FindBottomPage(void) 
{
	Page* SiblingPage = FindNextPage(); 
	while (SiblingPage != NULL)                       
	{         
		DocRect SiblingPageRect = SiblingPage->GetPageRect();   
		// For two pages joined together vertically the hi.y of the lower page must be 
		// equal to lo.y of the upper page, also the two pages will have identical x
		// coordinates (i.e. they have the same width and are in the same place horizontally).       
    	// This condition could be optimised   
    	if ( ((SiblingPageRect.HighCorner().y) == (PageRect.LowCorner().y)) &&
    		 ((SiblingPageRect.HighCorner().x) == (PageRect.HighCorner().x)) &&
    		 ((SiblingPageRect.LowCorner().x) == (PageRect.LowCorner().x))
    	   )                  
    		return(SiblingPage); 
    	SiblingPage = SiblingPage->FindNextPage();
    }   
    return (NULL); // No page was found to be joined beneath this page
}  

/***********************************************************************************************

>	void Page::CopyIntoSpread(Page* ContextPage, LinkDirection ld)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/93
	Inputs:		ContextPage: Page to which the copied page is to be attached    
						 ld: The direction in which the copied page is to be attached to the 
							 ContextPage
	Outputs:	-
	Returns:	-
	Purpose:    To make a copy of the current page and attach it to ContextPage
				in the direction specified by ld.    
				
				(********* This fn is not finished yet it will be quite complex)
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

void Page::CopyIntoSpread(Page* ContextPage, LinkDirection ld)
{                  
}

#ifdef _DEBUG   
                
void Page::ShowDebugTreeDetails() const
{                                 
	TRACE( _T("Page ")); 
	Node::ShowDebugTreeDetails(); 
}   

#endif
      
/********************************************************************************************

>	void Page::SetPageColour(DocColour &NewColour)


	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/7/93
	Inputs:		NewColour: The new page colour
	Outputs:	-
	Returns:	-
	Purpose:	Sets the page colour to NewColour 
	Errors:		-
	SeeAlso:	Page::GetPageColour

********************************************************************************************/

void Page::SetPageColour(DocColour &NewColour)
{
	PageColour = NewColour;
}   

/********************************************************************************************

>	DocColour &Page::GetPageColour(void) const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/7/93
	Inputs:		-
	Outputs:	-
	Returns:	The colour of a camelot page
	Purpose:	For finding the colour of a page
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DocColour &Page::GetPageColour(void) 
{
	return (PageColour);     
}  

/********************************************************************************************

>	void* Page::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void Page::GetDebugDetails(StringBase* Str) 
{          
#ifdef _DEBUG
	NodeRenderablePaper::GetDebugDetails(Str); 
    String_256 TempStr; 
    if (!PageRect.IsValid())
	{ 
		TempStr = TEXT("\r\nPage Rectangle = *INVALID*\r\n"); 
	}
	else
    TempStr._MakeMsg(TEXT("\r\nPage Rectangle\r\n   Low(#1%ld, #2%ld)\r\n   High(#3%ld, #4%ld)\r\n"),    
						  PageRect.LowCorner().x,  
						  PageRect.LowCorner().y,
						  PageRect.HighCorner().x,  
						  PageRect.HighCorner().y); 
    (*Str)+=TempStr; 
#endif
}
 
/********************************************************************************************

>	virtual UINT32 Page::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 Page::GetNodeSize() const 
{     
	return (sizeof(Page)); 
}  	      

/********************************************************************************************

>	INT32 Page::SnapOrdinate(INT32 Lo, INT32 Hi,INT32 Src,INT32 SnapDist)

    Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	12/12/95
	Inputs:     Lo = low ordinate
				Hi = high ordinate
				Src = Source ordinate
				SnapDist = snap distance threshold
    Outputs:    
    Returns:    The snapped version of Src.
    			                                  		 
    Purpose:    This function will return either Lo, Hi, or Src depending on the following:

				if Src is closest to Lo, and the distance is <= SnapDist, Hi is returned
				if Src is closest to Hi, and the distance is <= SnapDist, Hi is returned

				Otherwise Src is returned back.
    Errors:        
	Scope:      public
	       
********************************************************************************************/

INT32 Page::SnapOrdinate(INT32 Lo, INT32 Hi,INT32 Src,INT32 SnapDist)
{
	INT32 DistLo = abs(Lo-Src);
	INT32 DistHi = abs(Hi-Src);

	if (DistLo < DistHi)
	{
		if (DistLo <= SnapDist)
			Src = Lo;
	}
	else
	{
		if (DistHi <= SnapDist)
			Src = Hi;
	}

	return Src;
}

/********************************************************************************************

>	virtual BOOL Page::Snap(DocCoord* pDocCoord)

    Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	12/12/95
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped .
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest point on the page boundary
    Errors:        
	Scope:      public
	       
********************************************************************************************/

BOOL Page::Snap(DocCoord* pDocCoord)
{
	ERROR3IF(pDocCoord == NULL,"pDocCoord == NULL");
	if (pDocCoord == NULL) return FALSE;

	MILLIPOINT SnapDist = CSnap::GetSnapDist();

	DocRect BigRect   = PageRect;
	DocRect SmallRect = PageRect;

	BigRect.Inflate(SnapDist);
	SmallRect.Inflate(-SnapDist);

	if (BigRect.ContainsCoord(*pDocCoord) && !SmallRect.ContainsCoord(*pDocCoord))
	{
		pDocCoord->x = SnapOrdinate(PageRect.lo.x,PageRect.hi.x,pDocCoord->x,SnapDist);
		pDocCoord->y = SnapOrdinate(PageRect.lo.y,PageRect.hi.y,pDocCoord->y,SnapDist);

		return TRUE;
	}
	return FALSE;
}

/***********************************************************************************************

> BOOL Page::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:	12/12/95
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped.
    			FALSE	- the DocRect has not been processed.
    			                                  		 
    Purpose:    Snaps the given rect to the page, preserving its width and height.

				It will snap the side of the rect that corresponds to the direction the
				mouse is moving, e.g. if the mouse is moving left, the left hand side of the 
				rect is snapped.

				To force the bottom left hand corner of the rect to be snapped, 
				supply PrevCoord=(0,0) and CurCoord(-1,-1).

				ALWAYS RETURNS FALSE currently.

	Scope:      public
	       
**********************************************************************************************/

BOOL Page::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(pDocRect == NULL,"pDocRect == NULL");
	if (pDocRect == NULL) return FALSE;

	DocCoord OrigCoord;

	if (PrevCoord.x < CurCoord.x)
		OrigCoord.x = pDocRect->hi.x;
	else
		OrigCoord.x = pDocRect->lo.x;
		
	if (PrevCoord.y < CurCoord.y)
		OrigCoord.y = pDocRect->hi.y;
	else
		OrigCoord.y = pDocRect->lo.y;

	DocCoord SnapCoord = OrigCoord;

	if (Snap(&SnapCoord))
	{
		INT32 DeltaX = SnapCoord.x - OrigCoord.x;
		INT32 DeltaY = SnapCoord.y - OrigCoord.y;

		pDocRect->Translate(DeltaX,DeltaY);

		return TRUE;
	}
	else
#endif
		return FALSE;
}

//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

/********************************************************************************************

>	virtual BOOL Page::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/7/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the Page record to the filter (if it's required)
	SeeAlso:	Node::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

********************************************************************************************/

BOOL Page::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	// This is not required in the web format
	return TRUE;
}

//--------------------------------------------------------------
// See Page::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL Page::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#if 0 //NEW_NATIVE_FILTER	// New native filters, only available to those who need them at present
	ERROR2IF(pFilter == NULL,FALSE,"Page - NULL filter param");

	// Only write out in the native file format
	// Just write out the position of this page as other information such as
	// show bleed is spread related and written in the spread information record
	BOOL RecordWritten = TRUE;
	BOOL ok = TRUE;

	CXaraFileRecord Rec(TAG_PAGE,TAG_PAGE_SIZE);

	ok = Rec.Init();

	if (ok) ok = Rec.WriteCoord(PageRect.lo);	// write out low corner details
	if (ok) ok = Rec.WriteCoord(PageRect.hi);	// write out high corner details

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);
	
	// If we have had a problem at any of the stages then return that to the caller
	if (!ok || RecordNumber <= 0)
		RecordWritten = FALSE;

	return RecordWritten;
#else
	return TRUE;
#endif //NEW_NATIVE_FILTER
}

