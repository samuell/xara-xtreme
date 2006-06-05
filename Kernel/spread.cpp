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

// Spread class implementation


#include "camtypes.h"
#include "spread.h"
//#include "simon.h"
#include "ensure.h"
//#include "rndrgn.h"
#include "page.h"
#include "exceptio.h"		// For BlowUpOnCrashMe() method      
#include "chapter.h"
//#include "mario.h"			// amounst other things _R(IDE_NOMORE_MEMORY)
#include "layer.h"
#include "convert.h"
#include "view.h"
//#include "sglayer.h"
#include "usercord.h"
//#include "tim.h"

#include "cxfrec.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "camfiltr.h"
#include "viewcomp.h"
#include "unitcomp.h"

#include "nodedoc.h"
#include "backgrnd.h"	// OpBackground
#include "animparams.h"
#include "progress.h"
#include "docview.h"
#include "optsmsgs.h"
#include "oilbitmap.h"	// CWxBitmap

//#include "prevwres.h"	// _R(IDS_TAG_LAYER_FRAMEPROPS)
//#include "frameops.h" // for OpGrabAllFrames
  
CC_IMPLEMENT_DYNAMIC(Spread, NodeRenderablePaper)          

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW



#if NEW_PASTEBOARD
// See the spread.h header for the definition of NEW_PASTEBOARD
// Note that I've set it to fail to compile until you've been made aware of the problem!
#error ("New pasteboard code is enabled. Import/Export is probably broken")
#endif


// Maximum pasteboard size - beyond this size we start to get problems at high zoom or something
// SeeAlso: kernel\optspage.cpp
const MILLIPOINT MaxPasteboardSize = 9 * 72000 * 12; 	// maximum = 108in = 9 ft = 275cm


#ifdef RALPH
#define NO_SPREAD 1
#endif
 
/*********************************************************************************************

>    Spread::Spread() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a Spread linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

Spread::Spread(): NodeRenderablePaper()
{   
	UserOrigin				= DocCoord(0, 0);
	SpreadOrigin			= DocCoord(0, 0);
   	BleedOffset				= 36000;
	ShowDropShadow			= TRUE;
	RalphDontShowPaper		= FALSE;
	//AnimPropertiesParam	= constructed
	//SpreadDimScale		= constructed
}                    
 

/***********************************************************************************************

>   Spread::Spread
	(
		Node* ContextNode,  
		AttachNodeDirection Direction, 
		MILLIPOINT BleedOffset = 0,  
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
					      
			BoundingRect: Bounding rectangle  
								  
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

Spread::Spread(Node* ContextNode,  
					AttachNodeDirection Direction,   
		 		    const DocRect& PasteRect, 
					MILLIPOINT Bleed, 
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected 
		   		   ): NodeRenderablePaper(ContextNode, Direction, Locked, Mangled, 
										Marked, Selected)
{   
	BleedOffset = Bleed;

	// Set the default spread coord origin
	SpreadOrigin	= PasteRect.lo;

	// Pasteboard is used to indicate the width of the document (for scroll bars etc)
	SetInitialPasteboardRect(PasteRect);

	// Default to showing the drop shadow on the page
	ShowDropShadow = TRUE;
	
	// we only render paper in some ralph document modes
	RalphDontShowPaper = FALSE;

	UserOrigin      = DocCoord(0,0);
}  



/********************************************************************************************

>	virtual String Spread::Describe(BOOL Plural, BOOL Verbose = TRUE)

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
              
String Spread::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_SPREAD_DESCRP)));  
	else
		return(String(_R(IDS_SPREAD_DESCRS))); 
}; 




/********************************************************************************************

>	DocRect	Spread::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/9/94
	Inputs:		DontUseAttrs - If this is TRUE the node should not try to use the
				attributes associated with the node. It will also leave the flag
				IsBoundingRectValid FALSE. This var defaults to FALSE
				HitTest - TRUE if being called during HitTest
	Returns:	The nodes bounding rectangle
	Purpose:	returns the nodes bounding rectangle. If the rectangle is valid the
				rectangle is simply returned, but is IsBoundingRectValid is FALSE (cos
				someone called InvaldateBoundingRect() on this node, or one of its
				children) then the bounding rect is first calculated and the flag reset,
				before it is returned. The Spreads bounds rect is measured in Document
				Coords and this conversion is made in here. ie. All objects below the spread
				measure their coords etc relative to the spread, but the spread itself is
				measured relative to the document.

********************************************************************************************/

DocRect	Spread::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// if the bounding rect of this node is not valid then fill in something
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		// just set it to be an empty rectangle
		DocRect BoundRect(0,0,0,0);
		
		Node* pNode = FindFirstChild();
		while (pNode!=NULL)
		{
			// Add in the bounding rect of this node with all the others
			if (pNode->IsBounded())
				BoundRect = BoundRect.Union(((NodeRenderableBounded*)pNode)->GetBoundingRect(DontUseAttrs));

			// And find the next node
			pNode = pNode->FindNext();
		}

		// Convert the bounding rect obtained from spread coords to document coords
		SpreadCoordToDocCoord(&BoundRect);

		if (DontUseAttrs)
			return BoundRect;

		// Copy the unions into the nodes bounding rect param
		BoundingRectangle = BoundRect;

		// mark the bounding rect as valid
		IsBoundingRectValid = TRUE;
	}

	// and return the bounding rect
	return BoundingRectangle;
}




/********************************************************************************************

>	DocRect	Spread::GetDrawingSize () const

	Author:		Colin_Barfoot (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/09/96
	Returns:	Size of the rectangle encompassing the drawing.
	Purpose:	Works out the size of the rectangle encompassing the drawing.
				Code also used to work out the pixel size in the bitmap export options
				dialog box.
				Graeme (13/7/00) - Changed the code so that the background layer is ignored.

********************************************************************************************/

DocRect	Spread::GetDrawingSize () const
{
	// Start out with an empty clip rectangle.
	DocRect SpreadRect;
	SpreadRect.MakeEmpty ();

	// Find the first layer in the spread.
	Layer* pLayer = FindFirstLayer ();

	// Loop through the layers, and union the bounding boxes of all visible, non-background
	// layers.
	while ( pLayer != NULL )
	{
		// Let's see if it is visible, and not a background layer.
		if ( pLayer->IsVisible() && !pLayer->IsBackground () )
		{
			// This one is visible, so union its bounding box in.
			SpreadRect = SpreadRect.Union(pLayer->GetBoundingRect () );
		}

		// Find the next layer.
		pLayer = pLayer->FindNextLayer ();
	}

	// Return the found rectangle to the caller. 	
	return SpreadRect;
}


/********************************************************************************************

>	BOOL Spread::IsSpread() const

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/02/95
	Returns:	TRUE  - this IS a spread node.
	Purpose:	Tell the caller that this is a spread node.

********************************************************************************************/

BOOL Spread::IsSpread() const
{
	// Yes, this is a spread!
	return TRUE;
}



/***********************************************************************************************

>	void Spread::RenderPasteboard(void) const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/5/93
	Inputs:		-
	Outputs:	-
	Returns:	-  
	Purpose:    To render the pasteboard and off-paste area
	Errors:		-     
	Scope:		Private
	SeeAlso:	-

***********************************************************************************************/

void Spread::RenderPasteboard( RenderRegion* pRender )
{                     
//#pragma message( __LOCMSG__ "Spread::RenderPasteboard- do nothing" )
//	TRACE( _T("Warning - Spread::RenderPasteboard called\n") );
	// Ensure we don't dither the filled areas
	pRender->SetSolidColours(TRUE);

	// Ensure that the top coord of this pasteboard is exactly the same as the bottom of 
	// the upper pasteboard.    
	              
#ifdef _DEBUG
	Spread* PrevSpread = FindPreviousSpread(); 
	if (PrevSpread != NULL)
		ENSURE((PasteboardRect.HighCorner().y == 
			   PrevSpread->GetPasteboardRect(FALSE,pRender->GetRenderView()).LowCorner().y),
			   "Spreads are not properly vertically aligned");	
#endif

	// Find out the Clip rect of the render region to help decide which rectangles to try to draw
	DocRect SpreadPasteboard = GetWidePasteboard(pRender->GetRenderView());

	DocCoordToSpreadCoord(&SpreadPasteboard);

	Page* CurrentPage = FindFirstPageInSpread();
	
	// --- Divide the pasteboard up into filled rectangles and render them ---
	
	DocRect CurrentPageRect = CurrentPage->GetPageRect();
	                                                   
	// Ensure that we have a transparent line colour throughout
	pRender->SetLineColour(COLOUR_TRANS); 

	// Set up a pasteboard colour
	DocColour COLOUR_PASTEBOARD(192L, 192L, 192L);
#ifdef NO_SPREAD
	COLOUR_PASTEBOARD = DocColour(Page::PageColour);
#endif
	// colour the pasteboard white to hide it if we're in ralph DRAWING_VIEW mode
	// ( this flag is set in Render() )
	if (RalphDontShowPaper)	
		COLOUR_PASTEBOARD = DocColour(255L, 255L, 255L);
	
	COLOUR_PASTEBOARD.SetSeparable(FALSE);		// Don't colour-separate the pasteboard

	// We can now have a zero sized top pasteboard and due to pixelization of the
	// SpreadPasteboard DocRect this may give rise to errors if Top is just
	// constructed. So we must check it before hand.
	// Effectively doing the same as the ENSURE((Low.x <= High.x) && (Low.y <= High.y)
	// in the DocRect constructor.
	if (SpreadPasteboard.HighCorner().y > CurrentPageRect.HighCorner().y)
	{
		// Render the top pasteboard rectangle                                                    
		DocRect Top(DocCoord(SpreadPasteboard.LowCorner().x, 
							CurrentPageRect.HighCorner().y),
					DocCoord(SpreadPasteboard.HighCorner().x,
			   				SpreadPasteboard.HighCorner().y));  
  				
		pRender->SetFillColour(COLOUR_PASTEBOARD);
		pRender->DrawRect(&Top);
	}

	// For each row of pages in a spread, render a rectangle to the left of the left-most 
	// page reaching the left of the pasteboard, and a rectangle to the right of the right-most 
	// page reaching the right of the pasteboard. Also if any off-paste area needs to be rendered
	// then render a rectangle to the right of the right pasteboard rectangle.  
		
	do 
	{
		// Render rectangle to the left of the left-most page but first check 
		// that there is enough of a left section to worry about
		if (CurrentPageRect.LowCorner().x > SpreadPasteboard.LowCorner().x)
		{
			// Render rectangle to the left of the left-most page
			DocRect Left(DocCoord(SpreadPasteboard.LowCorner().x,
								  CurrentPageRect.LowCorner().y),
						 DocCoord(CurrentPageRect.LowCorner().x,
					 			  CurrentPageRect.HighCorner().y));

			pRender->SetFillColour(COLOUR_PASTEBOARD);
			pRender->DrawRect(&Left);
		}

		// Find the right-most page in the current row
		while (CurrentPage->FindRightPage())
			CurrentPage = CurrentPage->FindNextPage();
			                         
		CurrentPageRect = CurrentPage->GetPageRect();
		 
		// Render rectangle to the right of the right-most page but first check 
		// that there is enough of a right section to worry about
		if (SpreadPasteboard.HighCorner().x > CurrentPageRect.HighCorner().x)
		{
			// Render a rectangle to the right of the right-most page
			DocRect Right(DocCoord(CurrentPageRect.HighCorner().x, 
								   CurrentPageRect.LowCorner().y), 
						  DocCoord(SpreadPasteboard.HighCorner().x, 
					  			   CurrentPageRect.HighCorner().y)); 
					  			   
			pRender->SetFillColour(COLOUR_PASTEBOARD); // added 24/3/97
			pRender->DrawRect(&Right);
		}

		CurrentPage = CurrentPage->FindNextPage(); // Move to the first page in the next row
	    if (CurrentPage != NULL)
		{
	    	CurrentPageRect = CurrentPage->GetPageRect();
		}
	} while (CurrentPage != NULL); // While there are more rows 
    
	// Check that there is enough of a bottom section to worry about
	if (CurrentPageRect.LowCorner().y > SpreadPasteboard.LowCorner().y)
	{
		// Render a rectangle below the lowest page
		DocRect Bottom(DocCoord(SpreadPasteboard.LowCorner().x, 
				   				SpreadPasteboard.LowCorner().y), 
					   DocCoord(SpreadPasteboard.HighCorner().x,
				   				CurrentPageRect.LowCorner().y));
				   				
		pRender->SetFillColour(COLOUR_PASTEBOARD);
		pRender->DrawRect(&Bottom); 	 
	}

	// And revert to normal dithered plotting
	pRender->SetSolidColours(FALSE);
}



/***********************************************************************************************

>	void Spread::RenderDropShadows(void) const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Purpose:    To render right-hand and bottom drop shadows
	Scope:		private

***********************************************************************************************/

void Spread::RenderDropShadows(RenderRegion* pRender) 
{             
//#pragma message( __LOCMSG__ "Spread::RenderDropShadows- do nothing" )
//	TRACE( _T("Warning - Spread::RenderDropShadows called\n") );
	// If we have the new page background layer present then we needn't bother redrawing the
	// page as we might get a flash of white as we redraw
	// Therefore, we don't need to add a pixel on the right hand side page border
	BOOL PageBackgroundPresent = FALSE;
	// Is there a page background layer on the spread?
	Layer* pLayer = FindFirstPageBackgroundLayer();
	// Yes, the layer is present but is it visible?
	if (pLayer && pLayer->IsVisible())
		PageBackgroundPresent = TRUE;

	// Ensure filled areas are not dithered
	pRender->SetSolidColours(TRUE);

    // Setup attributes                                                         
	pRender->SetLineWidth(0); // means single pixel lines	
	pRender->SetLineColour(COLOUR_TRANS); 

	DocColour COLOUR_SHADOW(COLOUR_MIDGREY);
	pRender->SetFillColour(COLOUR_SHADOW);

	// Get the scaled pixel size for the view
	FIXED16 ScaledPixelWidth;
	FIXED16 ScaledPixelHeight;
	pRender->GetRenderView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);

	// Find out how many millipoints 4 pixels is
	MILLIPOINT DropWidth = 4 * (min(ScaledPixelWidth.MakeLong(), ScaledPixelHeight.MakeLong() ));
	
	MILLIPOINT ExtraPixel = ScaledPixelWidth.MakeLong();
	if (PageBackgroundPresent)
		ExtraPixel = 0;
	
	// The page we are currently drawing drop shadows for
	Page* CurrentPage = FindFirstPageInSpread(); 
	  
	//Render drop shadows for each page in turn  
	while (CurrentPage != NULL)   
	{                                  
		// Will indicate if page has a left or right drop shadow
		BOOL PageHasADropShadow = FALSE; 
		DocRect PageRect = CurrentPage->GetPageRect();
		
		// If the current page has no page to its right then we should render a right-hand
		// drop shadow 
		if (!(CurrentPage->FindRightPage())) 
		{                                         
			// Take width of page outline into account by rendering drop shadow one pixel beyond
			// edge of page! This makes drop shadow same width as its height below the page.
			DocRect RightHandDropShadow(PageRect.HighCorner().x + ExtraPixel, 
	                                  PageRect.LowCorner().y - DropWidth,
	                                  PageRect.HighCorner().x + DropWidth + ExtraPixel, 
	                                  PageRect.HighCorner().y - DropWidth 
									 );   
									 
			pRender->DrawRect(&RightHandDropShadow);

/*			// Attempt to render fuzzy shadow edge using bitmap...
			{
				DocRect r = RightHandDropShadow;
				CWxBitmap*			pBitmap = new CWxBitmap();
				pBitmap->LoadBitmap(_R(IDB_SPREAD_RIGHTEDGE));
				KernelBitmap*		pkBitmap = new KernelBitmap(pBitmap, TRUE);

				pRender->RenderBits(pkBitmap, (DocCoord*)&r, 2, TRUE, NULL);	// Unclean!
	
				if (pBitmap)
				{
					pBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
				}
				if (pkBitmap)
				{
					delete pkBitmap;
					pkBitmap = NULL;
				}
			}
*/
			PageHasADropShadow = TRUE;
									 
		}                                
		
		// If the current page has no page vertically joined beneath it then render a 
		// bottom drop shadow. 
		if (!(CurrentPage->FindBottomPage()))
		{
			//Render bottom drop shadow    
			DocRect BottomDropShadow(PageRect.LowCorner().x + DropWidth, 
								   PageRect.LowCorner().y - DropWidth, 
								   PageRect.HighCorner().x + DropWidth, 
								   PageRect.LowCorner().y  
								  );         
	        
			pRender->DrawRect(&BottomDropShadow);

/*			// Attempt to render fuzzy shadow edge using bitmap...
			{
				DocRect r = BottomDropShadow;
				CWxBitmap*			pBitmap = new CWxBitmap();
				pBitmap->LoadBitmap(_R(IDB_SPREAD_BOTTOMEDGE));
				KernelBitmap*		pkBitmap = new KernelBitmap(pBitmap, TRUE);

				pRender->RenderBits(pkBitmap, (DocCoord*)&r, 2, TRUE, NULL);	// Unclean!
	
				if (pBitmap)
				{
					pBitmap->BMBytes = ((CWxBitmap*)OILBitmap::Default)->BMBytes;
				}
				if (pkBitmap)
				{
					delete pkBitmap;
					pkBitmap = NULL;
				}
			}
*/
			PageHasADropShadow = TRUE; 
		}       
		
		// This bit is not very beautiful. If a page has a page to its right and a page beneath
		// it, and the page to its right does not have have a page beneath it, then there will
		// be a little cube of drop shadow left to draw at the bottom right hand corner of the 
		// page. 
		
		if (!PageHasADropShadow)
		{
			// The page has a page to its right and a page beneath it   
			ENSURE(	CurrentPage->FindRightPage(), 
					"Spread::RenderDropShadows: Cannot find right page");               
			if (CurrentPage->FindRightPage()->FindBottomPage() == NULL) 
			{
				DocRect MissingCube(PageRect.HighCorner().x, 
									PageRect.LowCorner().y - DropWidth,
									PageRect.HighCorner().x + DropWidth, 
									PageRect.LowCorner().y); 
				
				pRender->DrawRect(&MissingCube);  
			}
		}
		                 
		// Move to next page
		CurrentPage = CurrentPage->FindNextPage();  
	}

	// And return to normal dithered rendering
	pRender->SetSolidColours(FALSE);
}  


/***********************************************************************************************

>	void Spread::RenderBleedArea( RenderRegion* pRender )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/5/93
	Purpose:	Renders the Bleed Area rectangle if the BleedOffset is not 0
	Scope:      private

***********************************************************************************************/

void Spread::RenderBleedArea( RenderRegion* pRender) 
{                                
//#pragma message( __LOCMSG__ "Spread::RenderBleedArea - do nothing" )
//	TRACE( _T("Warning - Spread::RenderBleedArea called\n") );
    if  (BleedOffset != 0)
    {                                 
		// Draw a rectangle BleedOffset outside the common bounding rectangle of all pages
		// in the spread. 
    
    	// Calculate the common bounding rectangle of all the spread's pages
    	DocRect BleedRectangle; 
    	
    	Page* CurrentPage = FindFirstPageInSpread(); 
    	while(CurrentPage != NULL)
    	{
			DocRect PageRect = CurrentPage->GetPageRect();
    		BleedRectangle = BleedRectangle.Union(PageRect); 
    		CurrentPage = CurrentPage->FindNextPage(); 
    	}                 
    	BleedRectangle.Inflate(BleedOffset);                     

		// Ensure plotting does not use dithered colours
		pRender->SetSolidColours(TRUE);

		// Draw a rectangle covering the page 
		// Set up attributes for drawing page rectangle
		pRender->SetLineWidth(0); 					// Means single-pixel lines
		pRender->SetLineColour(COLOUR_TRANS); 

		DocColour COLOUR_BLEEDAREA(COLOUR_RED);
		pRender->SetFillColour(COLOUR_BLEEDAREA);
  		pRender->DrawPixelRect(&BleedRectangle);

		// And return to normal dithered rendering
		pRender->SetSolidColours(FALSE);
    }
}    



/***********************************************************************************************

>	void Spread::Render( RenderRegion* pRender )

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/5/93
	Inputs:		pRender - The render region to draw into
	Purpose:    Renders spread items:
					Pasteboard and off-paste area
					Main fold line
					Spread dividers
					Bleed Area
					Drop-shadows
					Spread outline
					Print margin

***********************************************************************************************/

void Spread::Render( RenderRegion* pRender )
{
	// If we should blow up, then blow up - don't worry, this is a very quick inline check
	OpException::BlowUpOnCrashMe();

// ralph only renders paper/pasteboard in some view modes  	
#ifdef RALPH
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
	// set RalphDontShowPaper - it will be tested in RenderPasteboard	
	if(pDoc)
		RalphDontShowPaper =((Document*)pDoc)->RalphDontShowPaper();
#endif

    // --- Render pasteboard ---
    RenderPasteboard(pRender);

	if(!RalphDontShowPaper)
	{

		// --- Render main fold line ---
		Chapter* pChapter = FindParentChapter();
		ERROR3IF(pChapter == NULL,"Spread::Render: Could not find parent chapter");

		if (pChapter != NULL && pChapter->ShouldShowFoldLine())
		{
    		MILLIPOINT MainFoldLineXCoord = pChapter->GetFoldLineXCoord();

			DocRect PBRect = GetPasteboardRect(TRUE, pRender->GetRenderView());

			pRender->SetLineWidth(0); 					// Means single-pixel lines
			pRender->SetLineColour(COLOUR_TRANS);

			DocColour COLOUR_FOLDLINE	= DocColour(150L, 150L, 150L);
			COLOUR_FOLDLINE.SetSeparable(FALSE);		// Don't colour-separate the fold line

			pRender->SetFillColour(COLOUR_FOLDLINE);
    		pRender->DrawPixelLine(DocCoord(MainFoldLineXCoord, PBRect.lo.y),
								   DocCoord(MainFoldLineXCoord, PBRect.hi.y));
		}

		// --- Render spread dividers ---

		// Set the colour for the spread divide (mid grey)
/*		DocColour COLOUR_SPREADDIVIDE = DocColour(64L, 32L, 32L); //DocColour(127L, 127L, 127L);
		COLOUR_SPREADDIVIDE.SetSeparable(FALSE);		// Don't colour-separate the spread divider

		pRender->SetLineColour(COLOUR_SPREADDIVIDE);

		// Set the line width (Wider for the first spread in the chapter)
		if (FindPrevious()==NULL)
		{
			MILLIPOINT LineWidth = 4*PixelHeight; 
			pRender->SetLineWidth(LineWidth);
		}
		else
			pRender->SetLineWidth(0);

		// Draw line along the top of the pasteboard 
		View *pView = pRender->GetRenderView();
		MILLIPOINT Width = GetWidePasteboard(pView).Width();
		MILLIPOINT Height = GetWidePasteboard(pView).Height();
		pRender->DrawLine(DocCoord(-Width, Height), DocCoord(Width, Height));
*/

		if (FindPreviousSpread()!=NULL)
		{
			DocColour COLOUR_SPREADDIVIDE = DocColour(64L, 32L, 32L); //DocColour(127L, 127L, 127L);
			COLOUR_SPREADDIVIDE.SetSeparable(FALSE);		// Don't colour-separate the spread divider
			pRender->SetFillColour(COLOUR_SPREADDIVIDE);

			View *pView = pRender->GetRenderView();
			DocRect sd = GetWidePasteboard(pView);
			DocCoordToSpreadCoord(&sd);

			FIXED16 ScaledPixelWidth;
			FIXED16 ScaledPixelHeight;
			pRender->GetRenderView()->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);
			sd.lo.y = sd.hi.y - ScaledPixelWidth.MakeLong();
			pRender->DrawRect(&sd);
		}
		
		// --- Render drop shadows ---
		if (ShowDropShadow)
			RenderDropShadows( pRender );      
		
		// --- Render bleed area, if not 0 size ---
		// Render after page shadow as otherwise page shadow may overdraw the bleed margin
		// and the bleed margin will disappear.
		RenderBleedArea(pRender);
	}
}  


	
/***********************************************************************************************

> Node* Spread::SimpleCopy()   

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

Node* Spread::SimpleCopy()
{
	Spread* NodeCopy; 
	NodeCopy = new Spread();
	ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}  



/***********************************************************************************************

>   void Spread::CopyNodeContents(Spread* NodeCopy)

    Author:     Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
    Created:    28/4/93
	
	Inputs:           
    Outputs:    A copy of this node
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/

void Spread::CopyNodeContents(Spread* NodeCopy)
{                         
	ERROR3IF(NodeCopy == NULL,"Trying to copy node contents to\n"
							"a node pointed to by a NULL pointer"); 
	NodeRenderablePaper::CopyNodeContents(NodeCopy); 
	
	NodeCopy->BleedOffset = BleedOffset;
	NodeCopy->ShowDropShadow = ShowDropShadow;
	NodeCopy->SpreadOrigin = SpreadOrigin;
	NodeCopy->UserOrigin = UserOrigin;
	NodeCopy->m_AnimPropertiesParam = m_AnimPropertiesParam;
	NodeCopy->SpreadDimScale = SpreadDimScale;
	NodeCopy->RalphDontShowPaper = RalphDontShowPaper;
}

  	      
/***********************************************************************************************
>   void Spread::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void Spread::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, Spread), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, Spread))
		CopyNodeContents((Spread*)pNodeCopy);
}



/***********************************************************************************************

>	Page* Spread::FindFirstPageInSpread() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		-
	Outputs:	-
	Returns:	The first page in the spread, or NULL if the spread has no pages
	Purpose:    To find the first page in a spread
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Page* Spread::FindFirstPageInSpread() const
{                     
	Node* CurrentNode = FindFirstChild(); 
	while(CurrentNode != NULL)   
	{
		if (CurrentNode->IsKindOf(CC_RUNTIME_CLASS(Page)))
			return ((Page*)CurrentNode); 	 
		CurrentNode = CurrentNode->FindNext(); 
	}
	return (NULL); // No pages were found
}


  
/***********************************************************************************************

>	Spread* Spread::FindNextSpread(void)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		-
	Outputs:	-
	Returns:	The next sibling spread of this node, or NULL if there are no more spreads. 
	Purpose:    To find the next sibling spread
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Spread* Spread::FindNextSpread(void)
{
	Node* CurrentNode = FindNext(); 
	while (CurrentNode != 0)
	{
		if (CurrentNode->IsSpread()) return (Spread*) CurrentNode;	  
		CurrentNode = CurrentNode->FindNext(); 
	}
	return 0; 							// No spread found
}                                                               



/***********************************************************************************************

>	Spread* Spread::FindPreviousSpread(void)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		-
	Outputs:	-
	Returns:	The previous sibling spread of this node, or NULL if there are none.    
	Purpose:    To find the current spreads previous sibling spread
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

Spread* Spread::FindPreviousSpread(void)
{
	Node* CurrentNode = FindPrevious(); 
	while (CurrentNode != 0)
	{
		if (CurrentNode->IsSpread()) return (Spread*) CurrentNode;
		CurrentNode = CurrentNode->FindPrevious(); 
	}
	return 0; 							// No spread found
}       



/********************************************************************************************

>	Layer* Spread::FindActiveLayer(void)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		-
	Outputs:	-
	Returns:	The active layer for the spread, or NULL if there is no active layer 
				Note: a spread should always have one active layer, so in a DEBUG build an 
				ENSURE failure will occur if no active layer exists. 

	Purpose:	For finding the spreads active layer. 
	Errors:		An ENSURE failure will occur if the spread has no active layer, or if the spread
				has more than one active layer. 
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindActiveLayer(void)
{
	// Changed by MarkN 11/8/94 to use Spread::FindFirstLayer()and Layer::FindNextLayer()

	Layer* pActiveLayer = NULL; 
	Layer* pLayer = FindFirstLayer(); 	 // Get first child 
	BOOL MultipleActiveLayers = FALSE;
	while (pLayer != NULL)
	{
		if (pLayer->IsActive())			// we have found an active layer 
		{
			// There should only ever be one active layer 
			ERROR3IF(pActiveLayer != NULL, "Spread has more than one active layer");
			if (pActiveLayer != NULL)
				MultipleActiveLayers = TRUE;
			pActiveLayer = pLayer; 
		}

		pLayer = pLayer->FindNextLayer(); // Find next layer
	} 

	// This line added by MarkN 22/3/95, to ensure that this function cannot fail
	// as long as there is at least one layer in this spread.
	// The MultipleActiveLayers added by Neville 12/6/96 so that multiple active layers
	// are fixed, if found.
	PORTNOTETRACE("other","Spread::CopyNodeContents - not yet calling EnsureActiveLayerIntegrity");
#ifndef EXCLUDE_FROM_XARALX
	if (pActiveLayer == NULL || MultipleActiveLayers)
		LayerSGallery::EnsureActiveLayerIntegrity(this,&pActiveLayer);
#endif
	// Fail if the spread has no active layer 
	return (pActiveLayer);
} 



/********************************************************************************************

>	Layer* Spread::FindFirstLayer() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/94
	Inputs:		-
	Outputs:	-
	Returns:	The first layer on the spread, or NULL if the spread has no layers.  
				
	Purpose:	To find the spread's first layer. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindFirstLayer() const
{
	Node* Current = FindFirstChild(); 
	while (Current != NULL)
	{
		if (Current->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
		{
			return (Layer*)Current; 
		}	
		Current = Current->FindNext(); 	
	}

	return (NULL); 
}
 
                    
/********************************************************************************************

>	Layer* Spread::FindLastLayer() const 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The last layer on the spread, or NULL if the spread has no layers.  
				
	Purpose:	To find the spread's last layer. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindLastLayer() const
{
	Node* pNode = FindFirstChild(); 
	Layer* pLastLayer = NULL;

	while (pNode != NULL)
	{
		if (pNode->GetRuntimeClass() == CC_RUNTIME_CLASS(Layer))
			pLastLayer = (Layer*)pNode; 

		pNode = pNode->FindNext(); 	
	}

	return (pLastLayer); 
}
 


/********************************************************************************************

>	Layer* Spread::FindFirstGuideLayer() const

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	The first guide layer on the spread, or NULL if the spread has no layers.  
				
	Purpose:	To find the spread's first guide layer. 

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindFirstGuideLayer() const
{
	Layer* pLayer = FindFirstLayer();
	while (pLayer != NULL)
	{
		if (pLayer->IsGuide())
			return pLayer;

		pLayer = pLayer->FindNextLayer();
	}

	return NULL;
}

/********************************************************************************************

>	Layer* Spread::FindFirstPageBackgroundLayer() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The first page background layer on the spread, or NULL if the spread has no layers.  
	Purpose:	To find the spread's first page background layer. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindFirstPageBackgroundLayer() const
{
	// Search through the children of this Spread, looking for Layer nodes.
	// Use the special flag of the page background layer as its identifier.
	Layer* pLayer = FindFirstLayer(); 
	while (pLayer != NULL)
	{
		// See if this is a page background layer
		if (pLayer->IsPageBackground())
		{
			// Cor blimey, we've found one, so return this layer to the caller
			return pLayer;
		}
		
		pLayer = pLayer->FindNextLayer(); 	
	}

	// Nothing found
	return NULL;
}

/********************************************************************************************

>	Layer* Spread::FindFirstFrameLayer() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The first frame layer on the spread, or NULL if the spread has no layers.  
	Purpose:	To find the spread's first frame layer. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindFirstFrameLayer() const
{
	// Search through the children of this Spread, looking for Layer nodes.
	// Use the special flag for frame layers as its identifier.
	Layer* pLayer = FindFirstLayer(); 
	while (pLayer != NULL)
	{
		// See if this is a frame layer
		if (pLayer->IsFrame())
		{
			// Cor blimey, we've found one, so return this layer to the caller
			return pLayer;
		}
		
		pLayer = pLayer->FindNextLayer(); 	
	}

	// Nothing found
	return NULL;
}

/********************************************************************************************

>	Layer* Spread::FindLastFrameLayer() const 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/5/97
	Inputs:		-
	Outputs:	-
	Returns:	The last frame layer on the spread, or NULL if the spread has no frame layers.  
	Purpose:	To find the spread's last framelayer. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Layer* Spread::FindLastFrameLayer() const
{
	// Search through the children of this Spread, looking for Layer nodes.
	// Use the special flag for frame layers as its identifier.
	Layer* pLayer = FindFirstLayer(); 
	Layer* pLastLayer = NULL;
	while (pLayer != NULL)
	{
		// See if this is a frame layer
		if (pLayer->IsFrame())
		{
			// Cor blimey, we've found one, so remember this as a potential last frame layer
			pLastLayer = pLayer;
		}
		
		pLayer = pLayer->FindNextLayer(); 	
	}

	// Return what we found as the last layer
	return pLastLayer;
}

/********************************************************************************************

>	Page* Spread::FindLastPageInSpread() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/97
	Inputs:		-
	Outputs:	-
	Returns:	The last page on the spread, or NULL if the spread has no pages.  
	Purpose:	To find the spread's last page.
				Layers and other renderable nodes should always be inserted after the last page.
				Otherwise, there is likely to be redraw problems.
	Errors:		-
	SeeAlso:	FindFirstPageInSpread;

********************************************************************************************/

Page* Spread::FindLastPageInSpread() const
{
	// Get the first page and then continually get the next page until one is not found
	// Each time we get a real page we note this as a potential last page.
	Page *pLastPage = NULL;
	Page *pPage = FindFirstPageInSpread();
	while (pPage != NULL)
	{
		pLastPage = pPage; // note this page as a potential last page
		pPage = pPage->FindNextPage();
	}

	// return what we think is the last page
	return pLastPage;
}

/********************************************************************************************

>	DocRect Spread::GetWidePasteboard(View *pView)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/11/94
	Inputs:		pView - the view to pixelise to.
	Purpose:	Return the pixelised boundary of the "wide pasteboard"

********************************************************************************************/

DocRect Spread::GetWidePasteboard(View *pView)
{
	ERROR2IF(this==NULL,DocRect(0,0,0,0),"GetWidePasteboard called on NULL pointer");

	// A large size - roughly the width/height of 30 A4 pages. We use this to extend
	// the WPB to an effective "infinity" in certain directions.
	const MILLIPOINT LargeNumber = 30 * (10 * 72000);

	DocRect WPB(PasteboardRect);

	// Inflate the left/right sides out to effective "infinity"
	WPB.Inflate(LargeNumber, 0);

	// If this is the very last spread in the chapter, expand its WPB down by "infinity"
	if (FindNextSpread() == NULL)
		WPB.lo.y -= LargeNumber;

	// Pixelise and return the result
	WPB.lo.Pixelise(pView);
	WPB.hi.Pixelise(pView);

	return(WPB);
}




#ifdef _DEBUG    
    
void Spread::ShowDebugTreeDetails() const
{                                 
	TRACE( _T("Spread ")); 
	Node::ShowDebugTreeDetails(); 
}

#endif  

/********************************************************************************************

>	void* Spread::GetDebugDetails(StringBase* Str) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/93
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Spread::GetDebugDetails(StringBase* Str)
{
	NodeRenderablePaper::GetDebugDetails(Str);
	String_256 TempStr;

	TempStr._MakeMsg(TEXT("\r\nBleed Offset = #1%ld"), BleedOffset);
	*Str += TempStr;

	TempStr._MakeMsg(TEXT("\r\nSpreadCoord Origin = #1%ld, #2%ld"),
					 SpreadOrigin.x, SpreadOrigin.y); 
	*Str += TempStr;
}



/********************************************************************************************

>	virtual UINT32 Spread::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 Spread::GetNodeSize() const 
{     
	return (sizeof(Spread)); 
}  



/********************************************************************************************

>	DocRect Spread::GetPageBounds() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	A document rectangle which is the rectangle bounding all Pages in
				this Spread.  This really is a document rectangle, not a spread
				rectangle hiding as a DocRect, ie. it is relative to the bottom-left
				of the document, not the spread.
	Purpose:	Finds the bounding rectangle of this Spread, defined as the smallest
				rectangle enclosing all the Pages the Spread contains.
	Errors:		-
	SeeAlso:	Page::GetPageRect

********************************************************************************************/

DocRect Spread::GetPageBounds() const
{
	// Start with an empty bounding rectangle.
	DocRect drBound;
	drBound.MakeEmpty();

	// Search through the children of this Spread, looking for Page nodes.
	Node* Current = FindFirstChild(); 
	while (Current != NULL)
	{
		if (Current->GetRuntimeClass() == CC_RUNTIME_CLASS(Page))
		{
			// Found a page node.  Take the union of its bounding rectangle and
			// the accumulated bounds.
			drBound = drBound.Union(((Page*) Current)->GetPageRect());
		}	
		Current = Current->FindNext(); 	
	}

	// Convert the value from spread coordinates to document coords
	SpreadCoordToDocCoord(&drBound);

	// Return the accumulated bounds.
	return drBound;
}



/********************************************************************************************

>	DocRect Spread::GetPageVisibleBounds() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/5/94
	Inputs:		-
	Outputs:	-
	Returns:	A document rectangle which is the rectangle bounding all Pages in
				this Spread.  This really is a document rectangle, not a spread
				rectangle hiding as a DocRect, ie. it is relative to the bottom-left
				of the document, not the spread.
	
	Purpose:	Finds the bounding rectangle of this Spread, defined as the smallest
				rectangle enclosing all the Pages the Spread contains.

				The visible bounds exclude all non-visible layers

	Errors:		-
	SeeAlso:	Page::GetPageRect

********************************************************************************************/

DocRect Spread::GetPageVisibleBounds() const
{
	// Start with an empty bounding rectangle.
	DocRect drBound;
	drBound.MakeEmpty();

	// Search through the children of this Spread, looking for Layer nodes.
	Layer* pCurrent = ((Spread*)this)->FindFirstLayer(); 
	while (pCurrent != NULL)
	{
		// Only add the bounding box of the layer in if it is visible and if it is not
		// either a guide layer or the page background layer.
		if (pCurrent->IncludeLayerInBoundingCalcs())
		{
			// Found a visible layer node.  Take the union of its bounding rectangle and
			// the accumulated bounds.
			drBound = drBound.Union(pCurrent->GetBoundingRect());
		}	
		
		pCurrent = pCurrent->FindNextLayer(); 	
	}

	// Convert the value from spread coordinates to document coords
	SpreadCoordToDocCoord(&drBound);

	// Return the accumulated bounds.
	return drBound;
}



/********************************************************************************************

>	DimScale* Spread::GetPtrDimScale()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/6/94
	Inputs:		-
	Outputs:	-
	Returns:	A ptr to the dimension scale object for all objects in this Spread
	Purpose:	For getting a ptr to the Spread's dimension scale object
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

DimScale* Spread::GetPtrDimScale()
{
	return (&SpreadDimScale);
} 



/********************************************************************************************

>	MILLIPOINT Spread::GetBleedOffset()	const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current size in millipoints of this spreads bleed are.
	Purpose:	For getting the Spread's bleed area size. 
	Errors:		-
	SeeAlso:	SetBleedOffset();

********************************************************************************************/

MILLIPOINT Spread::GetBleedOffset() const
{
	return BleedOffset;
} 



/********************************************************************************************

>	BOOL Spread::SetBleedOffset(MILLIPOINT Bleed)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current size in millipoints of this spreads bleed are.
	Purpose:	For setting a new value for the Spread's bleed area size. 
	Errors:		-
	SeeAlso:	GetBleedOffset();

********************************************************************************************/

BOOL Spread::SetBleedOffset(MILLIPOINT Bleed)
{
	BleedOffset = Bleed;	// set up new bleed value
	
	return TRUE;
} 



/********************************************************************************************

>	BOOL Spread::GetShowDropShadow() const

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		-
	Outputs:	-
	Returns:	The current drop shadow display state as either True or False.
	Purpose:	Finds out the current drop shadow display state.
	Errors:		-
	SeeAlso:	SetShowDropShadow();

********************************************************************************************/

BOOL Spread::GetShowDropShadow() const
{
	return ShowDropShadow;
} 



/********************************************************************************************

>	BOOL Spread::SetShowDropShadow(BOOL NewState)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		New drop shadow display state.
	Outputs:	-
	Returns:	The old drop shadow display state as either True or False.
	Purpose:	Allows the setting of a new drop shadow display state.
	Errors:		-
	SeeAlso:	GetShowDropShadow();

********************************************************************************************/

BOOL Spread::SetShowDropShadow(BOOL NewState)
{
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	BOOL OldState = ShowDropShadow;	// note current old state
	ShowDropShadow = NewState;		// Set up the required new state
	return OldState;				// return old state to the caller

#else //webster

	BOOL OldState = FALSE;	
    ShowDropShadow = FALSE ;
	return OldState;				// return old state to the caller
#endif //Webster

}

#if NEW_PASTEBOARD
// New pasteboard code by Jason. This keeps the bottom left corner of the page where
// it was, and rejigs the page size, other pages, bleed, and pasteboard to fit tidily
// around the outside. This results in a much tidier spread than the old code, but
// due to flaws in our V1 save file format, completely scrags save/load!
// Hopefully for V2, we can fix the save/load code to allow us to use more sensible layouts

/********************************************************************************************

>	BOOL Spread::GetPageSize(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Margin,
							 MILLIPOINT *Bleed, BOOL *Dps, BOOL *ShowDropShadow)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		-
	Outputs:	Width			returned width of the page in millipoints
				Height			returned height of the page in millipoints
				Margin			returned size of the margin area in millipoints (0 = none)
				Bleed			returned size of the bleed area in millipoints (0 = none)
				Dps				returned whether another page (or pages) is present or not.
				ShowDropShadow	returned new drop shadow display state.
	Returns:	Whether we completed the scanning operation ok or not as True or False.
	Purpose:	Allows the current page size specified for a spread to be determined.
				Assumes that all pages in one spread are the same size and hence returns
				this size.

				Note: Any of the input params can be NULL (Markn 4/4/95)

	Errors:		Will error if pages are not consistent.
	SeeAlso:	Spread::SetPageSize

********************************************************************************************/

BOOL Spread::GetPageSize(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Margin,
						 MILLIPOINT *Bleed, BOOL *Dps, BOOL *ShowDropShadow)
{
	ERROR2IF(this==NULL,FALSE,"Spread::GetPageSize called on NULL pointer");

	// Set up defaults in case of dodgy exit
	if (Width  	!= NULL) *Width  = 0;
	if (Height 	!= NULL) *Height = 0;
	if (Margin 	!= NULL) *Margin = 0;
	if (Bleed	!= NULL) *Bleed  = 0;
	if (Dps		!= NULL) *Dps = FALSE;
	if (ShowDropShadow != NULL) *ShowDropShadow = TRUE;

	Page *pPage = FindFirstPageInSpread();
	ERROR2IF(pPage == NULL,FALSE,"Spread::GetPageSize(): Could not find first Page");
	
	// Measured in millipoints
	DocRect PageRect = pPage->GetPageRect();

	MILLIPOINT PageWidth = PageRect.Width();
	MILLIPOINT PageHeight = PageRect.Height();	

	// Set up the return height and width according to this first page
	if (Width  != NULL) *Width  = PageWidth;
	if (Height != NULL) *Height = PageHeight;

	// Calculate the margin value
	if (Margin != NULL)
	{
		// The margin is the border between the edge of the page and the pasteboard
		// We assume that this is the same on all sides, which is the default whenever
		// the page size changes - If the user makes it lopsided, we'll use the maximum
		// value so that the page options dialogue shows a sensible new value.
		DocRect PasteRect = GetPasteboardRect();
		DocCoordToSpreadCoord(&PasteRect);

		INT32 MaxMargin = ABS(PageRect.lo.x - PasteRect.lo.x);

		INT32 temp = ABS(PageRect.lo.y - PasteRect.lo.y);
		if (temp > MaxMargin) MaxMargin = temp;

		temp = ABS(PageRect.hi.x - PasteRect.hi.x);
		if (temp > MaxMargin) MaxMargin = temp;

		temp = ABS(PageRect.hi.y - PasteRect.hi.y);
		if (temp > MaxMargin) MaxMargin = temp;

		*Margin = MaxMargin;
	}

	// Find out if dps by finding the next page in the spread and see if measurements are
	// the same as at present. Will only work in the simple case.
	pPage = pPage->FindNextPage();

	if (pPage != NULL)
	{
		// Measured in millipoints
		DocRect Page2Rect = pPage->GetPageRect();

		MILLIPOINT Page2Width = PageRect.Width();
		MILLIPOINT Page2Height = PageRect.Height();	

		// lox,loy,hix,hiy
		if (Page2Rect.lo.x == PageRect.hi.x && Page2Rect.lo.y == PageRect.lo.y &&
			Page2Width == PageWidth && Page2Height == PageHeight)
		{
			if (Dps != NULL)
				*Dps = TRUE;
		}
		else
		{
			// unrecognised page structure found
			ERROR2(FALSE,"bad page structure found in Spread::GetPageSize");
		}
	}
	else
	{
		if (Dps != NULL)
			*Dps = FALSE;
	}

	// Now get the current bleed size for the spread 
	if (Bleed != NULL)
		*Bleed = GetBleedOffset();

	if (ShowDropShadow != NULL)
		*ShowDropShadow = GetShowDropShadow();

	return TRUE;
}



/********************************************************************************************

>	BOOL Spread::SetPageSize(const MILLIPOINT Width, const MILLIPOINT Height, const MILLIPOINT Margin,
							 const MILLIPOINT Bleed, const BOOL Dps, const BOOL ShowDropShadow)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Hacked about a lot by Jason, 13/3/96)
	Created:	14/12/94
	Inputs:		Width			New width of pages in millipoints
				Height			New height of pages in millipoints
				Margin			New size of the margin area in millipoints (0 = none)
				Bleed			New size of the bleed area in millipoints (0 = none)
				Dps				(Double page spread) FALSE = 1 page; TRUE = 2 pages
				ShowDropShadow	New drop shadow display state.
	Outputs:	-

	Returns:	TRUE if we were successful

	Purpose:	Allows the setting of a new page size by height and width for all pages
				in this spread.	Also allows changing of other spread attributes such as:-
					margin around pages in spread (gap between pasteboard edge and pages),
					bleed size
					single/double page spread
					whether a drop shadow is displayed for pages on this spread.

	Notes:		The pages will be resized such that the bottom left of the bottom-left-most
				page stays in the same spot.

				Assumes there is only one default grid per spread

	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Spread::SetPageSize(const MILLIPOINT Width, const MILLIPOINT Height, const MILLIPOINT Margin,
						 const MILLIPOINT Bleed, const BOOL Dps, const BOOL ShowDropShadow)
{
	ERROR2IF(this == NULL,FALSE,"Spread::SetPageSize called on NULL pointer");

	// Include the bleed area into the margin we were given
	MILLIPOINT PageMargin = Margin + Bleed;

	// --- Sort out the default grid (* Assumes there is only one default grid *)
	// Find the position of the grid relative to the pages so this can be maintained
	// (see the end of this function)
	NodeGrid* pGrid = FindFirstDefaultGridInSpread();
	DocCoord PageRelGridOrigin;

	if (pGrid != NULL)
	{
		DocCoord GridOrigin;

		pGrid->GetOrigin(&GridOrigin.x, &GridOrigin.y);
		SpreadCoordToPagesCoord(&PageRelGridOrigin, GridOrigin);
	}

	// --- Reposition/resize/add/remove pages as necessary
	if (!AdjustAllPages(Width, Height, Margin, Dps))
		return FALSE;


	// --- Create and set a tidy new pasteboard rectangle
	{
		// Determine the new combined width of all the pages
		MILLIPOINT WidthOfPages = Width;			// 1 page (single page spread)
		if (Dps)
			WidthOfPages = 2 * Width;				// 2 pages (double page spread)

		// First, make it the right size, centered about (0,0)
		MILLIPOINT x = (WidthOfPages + 2*PageMargin) / 2;
		MILLIPOINT y = (Height + 2*PageMargin) / 2;

		DocRect NewPasteRect = DocRect(-x, -y, x, y);

		// Translate it to the center of the new page spread, so it is centered on the pages
		// Note that the Rect is now in the correct place in _Document_ coords.
		DocRect PagesRect = GetPageBounds();
		x = (PagesRect.lo.x / 2) + (PagesRect.hi.x / 2);	// NOTE: Maths done carefully
		y = (PagesRect.lo.y / 2) + (PagesRect.hi.y / 2);	// to avoid integer overflows!
		NewPasteRect.Translate(x, y);

		// And expand it as necessary to include all the objects in the spread

/*	Strangely, the BoundingRect seems to include the entire pasteboard area if the document is blank-
	This means we always make the pasteboard 1 inch bigger than requested!


		DocRect ObjectBounds = GetBoundingRect();
		if (ObjectBounds.IsValid())
		{
			// Inflate to get a 1inch margin around the objects, to ensure you can
			// see all their blobs and have a bit of a margin near the edge of the 
			// scroll extent.
			ObjectBounds.Inflate(72000);
			NewPasteRect = NewPasteRect.Union(ObjectBounds);
		}
*/
		// Convert it from spread coordinates into document coords, and set it
		ChangePasteboardRect(NewPasteRect);

		// Finally, ensure all pasteboards butt up to each other tidily
		AdjustPasteboards();
	}

	// --- Set fold line position
	// Find the chapter in which we belong
	Chapter* pChapter = FindParentChapter();
	ERROR3IF(pChapter == NULL,"Spread::SetPageSize: Could not find parent chapter");
	if (pChapter != NULL)
	{
		// Set the main fold line x coordinate. Initialise to non-showing value
		MILLIPOINT FoldLineX = 0;
		BOOL ShowFoldLine = FALSE;

		// Check if there is another spread present or not
		// If not and only a single page then do not show the fold line
		Spread* pNextSpread = FindNextSpread();
		if (pNextSpread != NULL || Dps)
		{
			// We have more than one spread and/or a double page spread so
			// set the fold line to the join between the two pages or the right hand
			// side of the left page 
			FoldLineX = PageMargin + Width;
			ShowFoldLine = TRUE;
		}

		// Set the fold line position, and enable/disable display of it as appropriate
		pChapter->SetFoldLineXCoord(FoldLineX, ShowFoldLine);
		pChapter->InvalidateBoundingRect();
	}

	// --- Set the new bleed size for this spread
	if (!SetBleedOffset(Bleed))
		return FALSE;

	// --- Set the new dropshadow state
	SetShowDropShadow(ShowDropShadow);

	// --- Adjust the default grid origin to a sensible place relative to the page
	if (pGrid != NULL)
	{
		DocCoord GridOrigin;
		PagesCoordToSpreadCoord(&GridOrigin, PageRelGridOrigin);
		pGrid->SetOrigin(GridOrigin.x, GridOrigin.y);
	}

	return TRUE;
}



/********************************************************************************************

>	BOOL Spread::AdjustAllPages(const MILLIPOINT Width, const MILLIPOINT Height,
								   const MILLIPOINT Margin, const BOOL Dps)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> (Hacked about a lot by Jason, 13/3/96)
	Created:	16/1/95

	Inputs:		Width			New width of the page in millipoints
				Height			New height of the page in millipoints
				Margin			New size of the margin area in millipoints (0 = none)
				Dps				Whether another page is required or not.

	Outputs:	-
	Returns:	TRUE if we were successful

	Purpose:	Configures the pages in a spread.
				This simple function only allows:
					* All pages must be the same size
					* Spreads can only be Single or Double page spreads

				This will:
					* Make all pages the given size
					* Move all pages so they butt up against each other
					* Ensure that the pasteboard includes the margin around the outside
					  of the pages
					* Create/delete page(s) as necessary to make a single/double page spread

********************************************************************************************/

BOOL Spread::AdjustAllPages(const MILLIPOINT Width, const MILLIPOINT Height,
							   const MILLIPOINT Margin, const BOOL Dps)
{
	// Find the first page in the spread
	Page *pFirstPage = FindFirstPageInSpread();
	ERROR2IF(pFirstPage == NULL, FALSE, "This spread has no pages?!");

	// Get the size and position of the first page
	DocRect OldPageRect = pFirstPage->GetPageRect();

	// And place the new page with its bottom left corner in the same spot
	DocRect PageRect(0, 0, Width, Height);
	PageRect.Translate(OldPageRect.lo.x, OldPageRect.lo.y);

	if (!pFirstPage->SetPageRect(PageRect))
		return FALSE;

	// Find out if there is a 2nd page in the spread. This will be created/deleted/adjusted
	// as necessary to achieve the desired set of pages
	Page *pPage = pFirstPage->FindNextPage();
	if (pPage != NULL)
	{
		// --- We have 2 (or more) pages...
		if (Dps)
		{
			// ... and we want 2 pages, so just translate the current page to butt up
			// to the first page, and also set it to the new size
			PageRect.Translate(Width, 0);
			if (!pPage->SetPageRect(PageRect))
				return FALSE;

			// And advance to the next page. This should be NULL, but if it's not,
			// we'll drop out and delete all surplus pages anyway
			pPage = pPage->FindNextPage();
			ERROR3IF(pPage != NULL, "Surplus page(s) found in double page spread - I've deleted them");
		}

		// Finally, delete any surplus pages
		Page *pNext;
		while (pPage != NULL)
		{
			pNext = pPage->FindNextPage();
			pPage->CascadeDelete();
			// Of course, Cascade delete does not actually delete the page, so now do that
			delete pPage;
			pPage = pNext;
		}
	}
	else
	{
		// --- There is only one page
		// If we need 2 pages, then we'll have to create a new one
		if (Dps)
		{
			// Position the new one just to the right of the first page
			PageRect.Translate(Width, 0);

			Page *Page2 = new Page(this, LASTCHILD, PageRect);
			if (Page2 == NULL)
			{
				// We ran out of memory
				InformError(_R(IDE_NOMORE_MEMORY));
				return FALSE;
			}
		}
	}

	return TRUE;
}

#else

/********************************************************************************************

>	BOOL Spread::GetPageSize(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Margin,
							 MILLIPOINT *Bleed, BOOL *Dps, BOOL *ShowDropShadow)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/1/95
	Inputs:		-
	Outputs:	Width			returned width of the page in millipoints
				Height			returned height of the page in millipoints
				Margin			returned size of the margin area in millipoints (0 = none)
				Bleed			returned size of the bleed area in millipoints (0 = none)
				Dps				returned whether another page (or pages) is present or not.
				ShowDropShadow	returned new drop shadow display state.
	Returns:	Whether we completed the scanning operation ok or not as True or False.
	Purpose:	Allows the current page size specified for a spread to be determined.
				Assumes that all pages in one spread are the same size and hence returns
				this size.

				Note: Any of the input params can be NULL (Markn 4/4/95)

	Errors:		Will error if pages are not consistent.
	SeeAlso:	SetPageSize;

********************************************************************************************/
BOOL Spread::GetPageSize(MILLIPOINT *Width, MILLIPOINT *Height, MILLIPOINT *Margin,
						 MILLIPOINT *Bleed, BOOL *Dps, BOOL *ShowDropShadow)
{
	ERROR2IF(this==NULL,FALSE,"Spread::GetPageSize called on NULL pointer");

	// Set up defaults in case of dodgy exit
	if (Width  	!= NULL) *Width  = 0;
	if (Height 	!= NULL) *Height = 0;
	if (Margin 	!= NULL) *Margin = 0;
	if (Bleed	!= NULL) *Bleed  = 0;
	if (Dps		!= NULL) *Dps = FALSE;
//	WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
	if (ShowDropShadow != NULL) *ShowDropShadow = TRUE;
#endif //webster
	Page *pPage = FindFirstPageInSpread();
	ERROR2IF(pPage == NULL,FALSE,"Spread::GetPageSize(): Could not find first Page");
	
	// Measured in millipoints
	DocRect PageRect = pPage->GetPageRect();

	MILLIPOINT PageWidth = PageRect.Width();
	MILLIPOINT PageHeight = PageRect.Height();	

	// Set up the return height and width according to this first page
	if (Width  != NULL) *Width  = PageWidth;
	if (Height != NULL) *Height = PageHeight;

	// Get present low corner position, we will use this to calculate the gap around the pages
	DocCoord Low = PageRect.LowCorner();
	// Assume: Margin = the position of the bottom left hand corner of the old page.
	// Use y as the initial defualt page is shifted across giving large x and correct y.
	if (Margin != NULL) *Margin = Low.y;

	// Find out if dps by finding the next page in the spread and see if measurements are
	// the same as at present. Will only work in the simple case.
	pPage = pPage->FindNextPage();

	if (pPage != NULL)
	{
		// Measured in millipoints
		DocRect Page2Rect = pPage->GetPageRect();

		MILLIPOINT Page2Width = PageRect.Width();
		MILLIPOINT Page2Height = PageRect.Height();	

		// lox,loy,hix,hiy
		if( Page2Rect.lo.x == PageRect.hi.x && 
			Page2Rect.lo.y == PageRect.lo.y &&
			Page2Width == PageWidth && 
			Page2Height == PageHeight)
		{
			if (Dps != NULL) *Dps = TRUE;	
		}
		else
		{
			// unrecognised page structure found
			ERROR2(FALSE,"bad page structure found in Spread::GetPageSize");
		}
	}
	else
	{
		if (Dps != NULL) *Dps = FALSE;
	}

	// Now get the current bleed size for the spread 
	if (Bleed != NULL) *Bleed = GetBleedOffset();

	if (ShowDropShadow != NULL) *ShowDropShadow = GetShowDropShadow();

	return TRUE;
}

/********************************************************************************************

>	BOOL Spread::SetPageSize(const MILLIPOINT Width, const MILLIPOINT Height, const MILLIPOINT Margin,
							 const MILLIPOINT Bleed, const BOOL Dps, const BOOL ShowDropShadow)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		Width			New width of the page in millipoints
				Height			New height of the page in millipoints
				Margin			New size of the margin area in millipoints (0 = none)
				Bleed			New size of the bleed area in millipoints (0 = none)
				Dps				Whether another page is required or not.
				ShowDropShadow	New drop shadow display state.
	Outputs:	-
	Returns:	Whether we completed the operation ok or not as True or False.
	Purpose:	Allows the setting of a new page size by height and width for all pages
				in this spread.	Also allows changing of other spread attributes such as:-
					margin around pages in spread,
					bleed size
					single/double page spread
					whether a drop shadow is displayed for this spread.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Spread::SetPageSize(const MILLIPOINT Width, const MILLIPOINT Height, const MILLIPOINT Margin,
						 const MILLIPOINT Bleed, const BOOL Dps, const BOOL ShowDropShadow)
{
	ERROR2IF(this==NULL,FALSE,"Spread::SetPageSize called on NULL pointer");

	// Resize all the pages in the spread to be of the new size specified.
	
	// To set page size for all pages in this spread we will...
	// Find the layout of the pages in the spread (could be X or T shaped!) and calculate
	// the overall bounds plus the bounds of the objects on the page.
	// Expand pasteboard rect to give same offsets around new pages
	// (We won't allow pasteboard to shrink smaller than outermost objects)
	// Translate any spread after this one so there's no gap or overlap with this one
	// Set each page to the new page size
	// Translate pages so they butt up exactly to each other
	// (in their original layout of course)
	// (We won't try to translate any objects on this spread.)

	BOOL ok = TRUE;
	
	// Note the present pages rect
	DocRect OldPagesRect;
	GetPagesRect(&OldPagesRect);

	// find the position of the grid relative to the pages so this can be maintained
	NodeGrid* pGrid=FindFirstDefaultGridInSpread();
	ERROR2IF(pGrid==NULL,FALSE,"Spread::SetPageSize() no current default grid");
	DocCoord GridOrigin;
	pGrid->GetOrigin(&GridOrigin.x,&GridOrigin.y);
	DocCoord PageRelGridOrigin;
	SpreadCoordToPagesCoord(&PageRelGridOrigin,GridOrigin);
	// Check if Margin < Bleed, if so then add the margin to the bleed
	MILLIPOINT PageMargin = 0;
	if (Margin < Bleed)
		PageMargin = Margin + Bleed;
	else
		PageMargin = Margin;

	// Set up a new pasteboard rectangle to be of the correct size according to the new spec
	// for the page or pages for this spread.
	// First set up the width of the pages according to whether we require one or two pages. 
	MILLIPOINT WidthOfPages = 0;
	if (Dps)
		WidthOfPages = PageMargin + 2 * Width;
	else
		WidthOfPages = Width;

	// Set up the class variables for page height/width to echo the new values so any
	// pasteboard calculations use them.
// REMOVED - No other code actually references these variables, so I've removed 'em!
//	PageWidth = Width;
//	PageHeight = Height;

	// Get current pasteboard rectangle 
	//DocRect OldPasteRect = GetPasteboardRect(FALSE);
	
	// Resize this by the required amount i.e. difference between old and new
	DocRect PasteRect(MinDocCoord + 0x10000,
		                  (MaxDocCoord - 0x10000) - (PageMargin + Height + PageMargin),
		                  (MinDocCoord + 0x10000) + (PageMargin + WidthOfPages + PageMargin),
		                  MaxDocCoord - 0x10000);

	// Align all the pasteboards for all spreads in this chapter
	ok = AlignPasteboards(PasteRect, PageMargin);	
	if (!ok)
		return FALSE;

	// Find the chapter which we belong to
	Chapter* pChapter = FindParentChapter();
	ERROR3IF(pChapter == NULL,"Spread::SetPageSize: Could not find parent chapter");
	if (pChapter != NULL)
	{
		// Set the main fold line x coordinate. Initialise to non-showing value
		MILLIPOINT FoldLineX = -1000;
		// Check if there is another spread present or not
		// If not and only a single page then do not show the fold line
		Spread* pNextSpread = FindNextSpread(); 
		if (pNextSpread != NULL || Dps == TRUE)
		{
			// We have more than one spread and/or a double page spread so
			// set the fold line to the join between the two pages or the right hand
			// side of the left page 
			FoldLineX = PageMargin + Width;
		}
		pChapter->SetFoldLineXCoord(FoldLineX);
		pChapter->InvalidateBoundingRect();
	}

	// Now change the position of all pages in the spread.		
	// Calling routine has checked if the page size has changed or not and not called us
	// if nothing has changed.
	// Should have checked everything including going from dps to sps or sps to dps and
	// deleting/adding pages accordingly and setting the new bleed and shadow state.
	ok = SetSizeOfAllPages(Width, Height, PageMargin, Dps);
	if (!ok)
		return FALSE;

	// Now set the current bleed size for this spread 
	ok = ok && SetBleedOffset(Bleed);
	if (!ok)
		return FALSE;

	SetShowDropShadow(ShowDropShadow);

	// Adjust the default grid size to fit the new page layout
	DocRect  Rect = GetPasteboardRect(FALSE);
	Rect.Translate(-Rect.lo.x, -Rect.lo.y);
 
	pGrid->SetBoundingRect(Rect);
	pGrid->InvalidateBoundingRect();
	PagesCoordToSpreadCoord(&GridOrigin,PageRelGridOrigin);
	pGrid->SetOrigin(GridOrigin.x,GridOrigin.y);

	// Now, we must ensure that the special page background layer, if present
	// gets resized to fit around the new page structure
	Layer * pLayer = FindFirstPageBackgroundLayer();
	if (pLayer)
	{
		// Call the static function in the background op to go and fix up
		// the covering rectangle
		// First, get that size of the rectangle required to fill the present
		// pages.
		DocRect NewPagesRect;
		GetPagesRect(&NewPagesRect);
		// Translate the old pages from the old origin to the new
		OldPagesRect.Translate(-OldPagesRect.lo.x, -OldPagesRect.lo.y);
		OldPagesRect.Translate(NewPagesRect.lo.x, NewPagesRect.lo.y);
		
PORTNOTE("other","Removed OpBackground usage")
#ifndef EXCLUDE_FROM_XARALX
		OpBackground::FixBackgroundLayer(pLayer, NewPagesRect, OldPagesRect);
#endif
	}
	
	return TRUE;
}

/********************************************************************************************

>	BOOL Spread::AlignPasteboards(const DocRect PasteRect, const INT32 Margin)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		New size of the paste board rectangle for this spread.
	Outputs:	-
	Returns:	Whether we completed the operation ok or not as True or False.
	Purpose:	Tries to align all the pasteboards of the spreads in this chapter to the new
				size of this spread.
				Checks to see if the objects on the spread are bigger than the newly specified 
				pasteboard rectangle and if so will put a margin around the object rectangle
				and then combine this with the specified pasteboard height. This means we will
				not loose any objects off the spread.
				As it is changing the size and position of the spread, it will invalidate the
				spreads bounding rectangle so that hopefully all bounding boxes will be 
				recalculated rather than using the old and more than likely wrong cached one. 
				Assumes at present that it is only called on the first spread in the chapter.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Spread::AlignPasteboards(const DocRect NewPasteRect, const MILLIPOINT Margin)
{
	ERROR2IF(this==NULL,FALSE,"Spread::AlignPasteboards called on NULL pointer");

	// Work out the overall bounds of the pages and objects on the page in the spread
	// This will be in document coordinates
	DocRect ObjectsBounds = GetBoundingRect(FALSE);

	// Take copy of entry pasteboard rectangle size
	DocRect PasteRect = NewPasteRect;

	// Note:: The tops of the old paste board rectangle and the new are aligned rather
	// than the bottoms. Hence the 0,0s are different. This means that direct comparisons
	// between new size, old size and bounding rectangles are not possible.
	// First, get the current paste board rectangle for this spread.
	DocRect SpreadRect = GetPasteboardRect();

	// Work out the maximum height and width of pasteboard required to encompass all the
	// current objects on the page. Include a margin around the outside. 
	MILLIPOINT HeightReqd = ObjectsBounds.hi.y - SpreadRect.lo.y + Margin;
	MILLIPOINT WidthReqd = ObjectsBounds.hi.x - SpreadRect.lo.x + Margin;
	// Above calculations will not include objects off the bottom and/or left hand side of
	// the page. We would need to shift the bottom left hand corner of the page plus all
	// objects if we were going to move these objects back onto the page.
	
	// If bounding rectangle of objects on page is bigger than the size of this new pasteboard
	// rectangle then resize the pasteboard to include the objects bounding box.
	if (PasteRect.Width() < WidthReqd ||
		PasteRect.Height() < HeightReqd )
	{
		// Work out what the new required width and height of the pasteboard rectangle is
		// including a margin size border around the outside of the objects bounding rectangle
		// so that all the handles are accessable. 
		MILLIPOINT SpreadHeight = 0;
		MILLIPOINT SpreadWidth = 0;
		if (HeightReqd < PasteRect.Height() )
			SpreadHeight = PasteRect.Height();
		else
			SpreadHeight = HeightReqd;

		if (WidthReqd < PasteRect.Width() )
			SpreadWidth = PasteRect.Width();
		else
			SpreadWidth = WidthReqd;
			  
		// Contruct a new PasteRect to take this object bounding rectangle into account.
		PasteRect = DocRect(MinDocCoord + 0x10000,
		                   (MaxDocCoord - 0x10000) - (SpreadHeight),
		                   (MinDocCoord + 0x10000) + (SpreadWidth),
		                   MaxDocCoord - 0x10000);
	}

	// Now we have the required size and position, go and set it
	ChangePasteboardRect(PasteRect); 

	// Make the wide pasteboard align to the new one
	// REMOVED - WPB now calculated on the fly rather than cached
//	WidePasteboard.lo.y = PasteRect.lo.y;
//	WidePasteboard.hi.y = PasteRect.hi.y;

	// Make sure bounding rectangles are invalidated so that they are recalculated
	InvalidateBoundingRect();

	// Now ensure all following spreads are properly attached to this one 
	// Assumes that we are altering the first spread in the document
	// Note this spread as a possible last spread in the document
	Spread* pLastSpread = this;
	Spread* pThisSpread = this;
	Spread* pNextSpread = FindNextSpread(); 
	while (pNextSpread != NULL)
	{
		// Next spread present, so adjust the pasteboard on the first/previous spread to
		// be correct assuming that it is not the last one present.
// REMOVED- AdjustPasteboardHeight used to set the wide pasteboard - this is no longer necessary
// as the Wide Pasteboard is calculated on the fly rather than cached now.
//		pThisSpread->AdjustPasteboardHeight(FALSE);

		DocRect FirstPasteRect = pThisSpread->GetPasteboardRect(FALSE);
		DocRect SecondPasteRect = pNextSpread->GetPasteboardRect(FALSE);
		// Construct a new Pasteboard rectangle of the correct size and initial position
		// and then move it down by the height of the first spread (Down = -ve). 
		DocRect TempPasteRect(MinDocCoord + 0x10000,
	                  (MaxDocCoord - 0x10000) - SecondPasteRect.Height(),
	                  (MinDocCoord + 0x10000) + SecondPasteRect.Width(),
	                  MaxDocCoord - 0x10000);

		TempPasteRect.Translate(0, -( FirstPasteRect.Height() )); 
		// Now we have the required size and position, go and set it
		pNextSpread->ChangePasteboardRect(TempPasteRect); 

		// Make the wide pasteboard align to the new one
//		REMOVED - WPB now calculated on the fly rather than cached
//		pNextSpread->WidePasteboard.lo.y = TempPasteRect.lo.y;
//		pNextSpread->WidePasteboard.hi.y = TempPasteRect.hi.y;

#ifdef _DEBUG
		// On Debug versions check that the spreads are aligned correctly
		FirstPasteRect = pThisSpread->GetPasteboardRect(FALSE);
		SecondPasteRect = pNextSpread->GetPasteboardRect(FALSE);
TRACEUSER( "Neville", _T("First spread low corner y = %d\n"),FirstPasteRect.LowCorner().y);
TRACEUSER( "Neville", _T("Second spread high corner y = %d\n"),SecondPasteRect.HighCorner().y);
		ENSURE((SecondPasteRect.HighCorner().y == FirstPasteRect.LowCorner().y),
		   "Spread::SetPageSize() Spreads are not properly vertically aligned");
#endif	
		
		// Make sure bounding rectangles are invalidated so that they are recalculated
		pNextSpread->InvalidateBoundingRect();

		// Note this next spread as a possible last spread and this spread
		pLastSpread = pNextSpread;
		pThisSpread = pNextSpread;

		// Now, see whether there is a next spread or not.
		pNextSpread = pNextSpread->FindNextSpread(); 
	}

	// Last spread in document/chapter so adjust the bottom pasteboard to be correct
// REMOVED - this method just adjusts the Wide pasteboard - no longer necessary as the WPB
// is calculated on the fly rather than cached
//	if (pLastSpread != NULL)
//		pLastSpread->AdjustPasteboardHeight(TRUE);

	return TRUE;
}

/********************************************************************************************

>	BOOL Spread::SetSizeOfAllPages(const MILLIPOINT Width, const MILLIPOINT Height,
								   const MILLIPOINT Margin, const BOOL Dps)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/1/95
	Inputs:		Width			New width of the page in millipoints
				Height			New height of the page in millipoints
				Margin			New size of the margin area in millipoints (0 = none)
				Dps				Whether another page is required or not.

	Outputs:	-
	Returns:	Whether we completed the operation ok or not as True or False.
	Purpose:	Sets the sizes of all pages in this spread to conform to the newly specified
				width and height. It will try and align all the pages. It will also try and
				create and or delete any pages which are necessary to conform to the specified
				single/double page spread flag. 
				Assumes all the pages in a spread are the same size.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Spread::SetSizeOfAllPages(const MILLIPOINT Width, const MILLIPOINT Height,
							   const MILLIPOINT Margin, const BOOL Dps)
{
	ERROR2IF(this==NULL,FALSE,"Spread::SetSizeOfAllPages called on NULL pointer");

	BOOL ok = TRUE;

	// Find the first page in the spread
	Page *pPage = FindFirstPageInSpread();
	ERROR2IF(pPage == NULL,FALSE,"Spread::SetSizeOfAllPages(): Could not find first Page");
		
	// Take note of where this first page is
//	Page* pFirstPage = pPage;
	
	// Get the size and position of the first page
	DocRect OldPageRect = pPage->GetPageRect();
	
	// Get present low corner position, we will use this to calculate the gap around the pages
	DocCoord Low = OldPageRect.LowCorner();

	// Set up the new starting position of the page
		Low.x = Margin;
	Low.y = Margin;

	// Construct a new position and size with the required params
	DocRect PageRect(Low, Width, Height);
	ok = pPage->SetPageRect(PageRect);	
	if (!ok)
		return FALSE;

	// Find out if there is a next page in the spread. If there is, then we need to
	// change that as well, if we are in dps mode, or delete it if dps is off 
	Page* pCurrentPage = pPage;
	pPage = pPage->FindNextPage();
	if (pPage != NULL)
	{
		if (Dps)
		{
			// Move the current page across by one page worths and set this as the new
			// size/position for the next page 
			PageRect.Translate(Width, 0);	
			ok = pPage->SetPageRect(PageRect);
			if (!ok)
				return FALSE;
			
			// Now check to see if any more pages exist. If so then delete them.
			pPage = pPage->FindNextPage();
			while (pPage != NULL)
			{
				ERROR3("Spread::SetSizeOfAllPages extra page found in double page spread");
			}
		}
		else
		{
			// Delete any pages over the initial one.
			do
			{
				pPage->CascadeDelete();
				// Of course, Cascade delete does not actually delete the page, so now do that
				delete pPage;
				// Check to see if any pages remain after the first required one 
				pPage = pCurrentPage;
				pPage = pPage->FindNextPage();
			}
			while (pPage != NULL);
		}
	}
	else
	{
		// If not double page spread then everything is correct
		// otherwise we will need to create a page of the correct size and position
		if (Dps)
		{
			// move first page across by one page worths
			PageRect.Translate(Width, 0);
			// Create a new last page based on this size/position 
			Page* Page2 = new Page(this, LASTCHILD, PageRect);
			if (Page2 == NULL)
			{
				// We ran out of memory or something
				InformError(_R(IDE_NOMORE_MEMORY));
				return FALSE;
			}
		}
	}

	return TRUE;
}
#endif

/********************************************************************************************

>	Document* Spread::FindParentDocument()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/97
	Returns:	A pointer to the parent document of this spread.
	Purpose:	Finds the parent document for this spread. Should always be one present.
				Defined as an explicit function so that caller just has to check for NULL
				returns, rather than also checking that the parent is a chapter. Also, makes
				it more encapsulated and friendly.  

********************************************************************************************/

Document* Spread::FindParentDocument()
{
	BaseDocument *pBaseDoc = FindOwnerDoc();

	// Make sure we have a real document
	if (!pBaseDoc || !pBaseDoc->IS_KIND_OF(Document))
	{
		if (pBaseDoc == NULL)
		{
			ERROR3("Eh up, the spread has no parent document");
		}
		else
		{
			ERROR3("Document is not a real Document - it's a BaseDocument!");
		}

		return NULL;
	}

	Document *pDoc = (Document *) pBaseDoc;
	return pDoc;
}

/********************************************************************************************

>	Chapter* Spread::FindParentChapter()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the parent chapter of this spread. Will be NULL if no parent
				chapter is found. This of course should never happen.
	Purpose:	Finds the parent chapter for this spread. Should always be one present.
				Defined as an explicit function so that caller just has to check for NULL
				returns, rather than also checking that the parent is a chapter. Also, makes
				it more encapsulated and friendly.  
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Chapter* Spread::FindParentChapter()
{
	ERROR2IF(this==NULL,NULL,"Spread::FindParentChapter called on NULL pointer");

	Node* pCurrentNode = FindParent(); 
	while(pCurrentNode != NULL)   
	{
		if (pCurrentNode->IsKindOf(CC_RUNTIME_CLASS(Chapter)))
			return ((Chapter*)pCurrentNode); 	 
		pCurrentNode = pCurrentNode->FindParent(); 
	}
	return NULL;
}

/********************************************************************************************

>	NodeGrid* Spread::FindFirstDefaultGridInSpread()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/1/95
	Inputs:		-
	Outputs:	-
	Returns:	A pointer to the grid node. Will be NULL if no default grid is found.
	Purpose:	Finds the first default grid that has been defined in this spread. Should be
				the only one present.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

NodeGrid* Spread::FindFirstDefaultGridInSpread()
{
	ERROR2IF(this==NULL,NULL,"Spread::FindFirstDefaultGridInSpread called on NULL pointer");

	Node* pCurrentNode = FindFirstChild(); 
	while(pCurrentNode != NULL)   
	{
		if (pCurrentNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			// Found a grid type node. Check if its a default type.
			NodeGrid* pCurrentGrid = (NodeGrid*)pCurrentNode;
			if (pCurrentGrid->IsDefault())
				return (pCurrentGrid); 	 
		}

		pCurrentNode = pCurrentNode->FindNext(); 
	}
	return (NULL); // No default grids were found
}


/********************************************************************************************

>	BOOL Spread::CreateDefaultPageAndGrid(BOOL CreateGrid = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/8/96
	Inputs:		CreateGrid = TRUE if you want a grid & page
						     FALSE if you only want a grid
	Outputs:	-
	Returns:	TRUE if ok, FALSE if it fails
	Purpose:	This creates a default page and a default grid and places them as children of the spread.

				It doesn't check to see the spread has already got a page(s) or grid(s), so only call
				this when creating a spread for the first time.

				(I appoligise for the disgusting default parameter - it is due to circumstances beyond my control)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

MILLIPOINT				PageWidth  = ( 8 * 72000);
MILLIPOINT				PageHeight = (11 * 72000);

// Default page size/position.
MILLIPOINT				Gap = 72000L;

BOOL Spread::CreateDefaultPageAndGrid(BOOL CreateGrid)
{
	// Work out where to put the page and create it
	DocRect PageRect(Gap, Gap, Gap+PageWidth, Gap+(1*PageHeight));
	Page *pPage = new Page(this, FIRSTCHILD, PageRect);
	if (pPage == NULL)
		return(FALSE);

	// Create a default blanket grid in the given spread
	if (CreateGrid)
		NodeGrid::MakeDefaultGrid(this);

	return TRUE;
}

/********************************************************************************************
>	DocCoord Spread::GetUserOrigin()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/95
	Returns:	Offset from SpreadCoords to UserCoords
********************************************************************************************/

DocCoord Spread::GetUserOrigin()
{
	return UserOrigin;
}


/********************************************************************************************
>	BOOL Spread::GetPagesRect(DocRect* pPagesRect)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Outputs:	pPagesRect
	Returns:	FALSE if fails (PagesRect unchanged)
	Purpose:	Get the union rect of all pages on the spread (in SpreadCoords)
	Errors:		pPagesRect==NULL
				no pages in spread
********************************************************************************************/

BOOL Spread::GetPagesRect(DocRect* pPagesRect)
{
	ERROR2IF(pPagesRect==NULL,FALSE,"Spread::GetPagesRect() - pPagesRect==NULL");

	Page* pPage=FindFirstPageInSpread();
	ERROR2IF(pPage==NULL,FALSE,"Spread::GetPagesRect() - no pages in spread!");

	*pPagesRect=pPage->GetPageRect();
	pPage=pPage->FindNextPage();
	while (pPage)
	{
		DocRect PageRect=pPage->GetPageRect();
		*pPagesRect=pPagesRect->Union(PageRect);
		pPage=pPage->FindNextPage();
	}

	return TRUE;
}


/********************************************************************************************
>	BOOL Spread::SpreadCoordToPagesCoord(DocCoord* pPagesCoord, DocCoord SpreadCoord)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		SpreadCoord
	Outputs:	pPagesCoord
	Returns:	FALSE if fails (PagesCoord unchanged)
	Purpose:	Convert SpreadCoord to PagesCoord (relative to union of pages in spread)
********************************************************************************************/

BOOL Spread::SpreadCoordToPagesCoord(DocCoord* pPagesCoord, DocCoord SpreadCoord)
{
	ERROR2IF(pPagesCoord==NULL,FALSE,"Spread::SpreadCoordToPagesCoord() - pPagesCoord==NULL");
	
	DocRect PagesRect;
	if (GetPagesRect(&PagesRect)==FALSE)
		return FALSE;

	*pPagesCoord=SpreadCoord-PagesRect.lo;
	return TRUE;
}


/********************************************************************************************
>	BOOL Spread::PagesCoordToSpreadCoord(DocCoord* pSpreadCoord, DocCoord PagesCoord)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		PagesCoord
	Outputs:	pSpreadCoord
	Returns:	FALSE if fails (SpreadCoord unchanged)
	Purpose:	Convert PagesCoord (relative to union of pages in spread) to SpreadCoord 
********************************************************************************************/

BOOL Spread::PagesCoordToSpreadCoord(DocCoord* pSpreadCoord, DocCoord PagesCoord)
{
	ERROR2IF(pSpreadCoord==NULL,FALSE,"Spread::PagesCoordToSpreadCoord() - pSpreadCoord==NULL");
	
	DocRect PagesRect;
	if (GetPagesRect(&PagesRect)==FALSE)
		return FALSE;

	*pSpreadCoord=PagesCoord+PagesRect.lo;
	return TRUE;
}


/********************************************************************************************
>	BOOL Spread::TextToSpreadCoord(DocCoord* pSpreadCoord, StringBase* pxText, StringBase* pyText)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		pxText - x UserCoord in text form
				pyText - y UserCoord in text form
	Outputs:	pSpreadCoord
	Returns:	FALSE if fails (SpreadCoord unchanged)
	Purpose:	Convert Coord in text form (relative to union of pages in spread) into SpreadCoord
********************************************************************************************/

BOOL Spread::TextToSpreadCoord(DocCoord* pSpreadCoord, StringBase* pxText, StringBase* pyText)
{
	ERROR2IF(pSpreadCoord==NULL,FALSE,"Spread::TextToSpreadCoord() - pSpreadCoord==NULL");
	ERROR2IF(      pxText==NULL,FALSE,"Spread::TextToSpreadCoord() - pxText==NULL");
	ERROR2IF(      pyText==NULL,FALSE,"Spread::TextToSpreadCoord() - pyText==NULL");
	
	DimScale* pDimScale=DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,FALSE,"Spread::TextToSpreadCoord() - pDimScale==NULL");

	UserCoord UserPos(0,0);
	if (pDimScale->ConvertToMillipoints(*pxText, &(UserPos.x))==FALSE)
		return FALSE;
	if (pDimScale->ConvertToMillipoints(*pyText, &(UserPos.y))==FALSE)
		return FALSE;

	*pSpreadCoord = UserPos.ToSpread(this);
	return TRUE;
}


/********************************************************************************************
>	BOOL Spread::SpreadCoordToText(StringBase* pxText, StringBase* pyText, DocCoord SpreadCoord,
					BOOL xUnitSpecifier=TRUE, BOOL yUnitSpecifier=TRUE, INT32 dp=-1)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/1/95
	Inputs:		SpreadCoord
				xUnitSpecifier (default TRUE)  - append unit specifier to x text
				yUnitSpecifier (default TRUE)  - append unit specifier to y text
				dp             (default 2)     - decimal places (-1 = use system default)
	Outputs:	pxText - x UserCoord in text form
				pyText - y UserCoord in text form
	Returns:	FALSE if fails (xText,yText unchanged)
	Purpose:	Convert SpreadCoord into Coord in text form (relative to union of pages in spread) 
	Note:		If only Y specifier required and units are prefix
				then the unit specifier is actually placed before the x value
********************************************************************************************/

BOOL Spread::SpreadCoordToText(String_256* pxText, String_256* pyText, DocCoord SpreadCoord,
								BOOL xUnitSpecifier, BOOL yUnitSpecifier, INT32 dp)
{
	ERROR2IF(pxText==NULL,FALSE,"Spread::SpreadCoordToText() - pxText==NULL");
	ERROR2IF(pyText==NULL,FALSE,"Spread::SpreadCoordToText() - pyText==NULL");
	ERROR2IF(    dp<-1   ,FALSE,"Spread::SpreadCoordToText() - dp<-1");
	
	DimScale* pDimScale=DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,FALSE,"Spread::SpreadCoordToText() - pDimScale==NULL");

 	// if only Y unit specified, and units are prefix, force X unit specifier only
 	if (xUnitSpecifier==FALSE && yUnitSpecifier==TRUE)
 	{
 		// Get the current unit list from the document and the the current scale units
 		DocUnitList* pDocUnitList=DocUnitList::GetCurrentDocUnitList();
 		ERROR2IF(pDocUnitList==NULL,FALSE,"Spread::SpreadCoordToText() - DocUnitList::GetCurrentDocUnitList() returned NULL");
 		Unit* pUnit=pDocUnitList->FindUnit(pDimScale->GetUnits());
 		ERROR2IF(pUnit==NULL,FALSE,"Spread::SpreadCoordToText() - pDocUnitList->FindUnit() returned NULL");
 		if (pUnit->IsPrefix())
 		{
 			xUnitSpecifier=TRUE;
 			yUnitSpecifier=FALSE;
 		}
 	}
 
	UserCoord UserPos=SpreadCoord.ToUser(this);
	pDimScale->ConvertToUnits(UserPos.x, pxText, xUnitSpecifier, dp);
	pDimScale->ConvertToUnits(UserPos.y, pyText, yUnitSpecifier, dp);

	return TRUE;
}


/********************************************************************************************
>	BOOL Spread::GetDecimalPlacesForPixelResolution(View *pView, INT32* dp)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/95
	Returns:	FALSE if fails
	Outputs:	dp - number of dp (-ve values limited to 0)
	Purpose:	Find the number of dp to resolve a pixel in units and scale associated with spread
				(calulates the both x and y and returns max)
********************************************************************************************/

BOOL Spread::GetDecimalPlacesForPixelResolution(View *pView, INT32* dp)
{
	DimScale* pDimScale=DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,FALSE,"Spread::GetDecimalPlacesForPixelResolution() - pDimScale==NULL");

	INT32 xdp=2;
	INT32 ydp=2;
	double MPPerUnit=0.0;
	if( pDimScale->ConvertToDouble( String_8( _T("1") ), &MPPerUnit ) == FALSE )
		return FALSE;

	FIXED16 PixelWidth,
			PixelHeight;
			
	pView->GetScaledPixelSize(&PixelWidth, &PixelHeight);

	if (PixelWidth==0 || PixelHeight==0)
		return FALSE;

	ERROR2IF(MPPerUnit<1,FALSE,"Spread::GetDecimalPlacesForPixelResolution() - Millipoints per Unit < 1!");
	xdp=(INT32)log10(MPPerUnit/PixelWidth.MakeDouble() )+1;
	ydp=(INT32)log10(MPPerUnit/PixelHeight.MakeDouble())+1;

	if (xdp<0) xdp=0;
	if (ydp<0) ydp=0;
	*dp = xdp>ydp ? xdp : ydp;
	return TRUE;
}



/********************************************************************************************

>	DocCoord Spread::GetSpreadCoordOrigin(BOOL Pixelise, View *pView) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/96

	Inputs:		Pixelise - TRUE if you want the coordinate pixelised
				pView - NULL, or the View to use for pixelisation purposes.
				(These inputs are exactly the same as those in GetPasteboardRect)

	Returns:	The spread coordinate origin, in document coordinates

	Purpose:	To determine the spread coordinate origin
				This is used to convert between document and spread coordinates:
					DocCoord	= SpreadCoord + Origin
					SpreadCoord	= DocCoord - Origin

	Notes:		In the past, spread coords shared their origin with the bottom left corner
				of a spread's pasteboard. Obviously, if we tried to move this corner, we got
				into deep strife, so I've separated out the 2 points. By default the origin
				will happen to sit on the bottom left corner of the pasteboard, but this
				will change if the pasteboard is ever resized. Generally the SCO does not
				move (unless you have multiple spreads and a whole spread has to shift
				down to make room).

				WARNING: Because the SCO is not associated with the pasteboard corner any
				more, it is easy to resize the pasteboard so that the SCO lies *outside*
				the pasteboard. This can cause ERROR3's to go off if you try to call
				FindEnclosingChapter on the SCO (because no chapter encloses it!).
				Note that FindEnclosingChapter is called by the (document form) of
				DocCoord::ToWork. I have changed the ERROR3 message to point at this warning.

	SeeAlso:	NodePaper::GetPasteBoardRect; Spread::SetSpreadCoordOrigin

********************************************************************************************/

DocCoord Spread::GetSpreadCoordOrigin(BOOL Pixelise, View *pView) const
{
#if NEW_PASTEBOARD
// New functionality: Spread Origin is separate from pasteboard bottom left corner
	if (Pixelise)
	{
		DocCoord temp(SpreadOrigin);

		if (pView != NULL)
			temp.Pixelise(pView);
		else
			temp.Pixelise();
	
		return(temp);
	}

	return(SpreadOrigin);
#else
	// To keep the old functionality, the spread origin currently always sits on the
	// pasteboard bottom left corner.
	if (Pixelise)
	{
		DocCoord temp(PasteboardRect.lo);

		if (pView != NULL)
			temp.Pixelise(pView);
		else
			temp.Pixelise();
	
		return(temp);
	}

	return(PasteboardRect.lo);
#endif
}



/********************************************************************************************

>	void Spread::SetSpreadCoordOrigin(DocCoord NewOrigin)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/3/96

	Inputs:		NewOrigin - the new origin point for the spread coordinates within
				this spread

	Purpose:	To set the spread coordinate origin
				This is used to convert between document and spread coordinates:
					DocCoord	= SpreadCoord + Origin
					SpreadCoord	= DocCoord - Origin

	Notes:		Moving the spread coord origin effectively moves all nodes on the
				spread (pages, layers, objects, etc) in the opposite direction. It 
				may therefore result in objects and even pages moving off the pasteboard.

				Note also that you'll probably have to redraw some stuff!

				Use carefully - Talk to Jason about it first!

	SeeAlso:	Spread::GetSpreadCoordOrigin

********************************************************************************************/

void Spread::SetSpreadCoordOrigin(DocCoord NewOrigin)
{
#if !NEW_PASTEBOARD
	ERROR3("You can't set the spread coord origin (unimplemented) - Talk to Jason/Phil");
#endif

	SpreadOrigin = NewOrigin;
}



/********************************************************************************************

>	void Spread::SpreadCoordToDocCoord(DocCoord *pSpreadCoord) const
>	void Spread::SpreadCoordToDocCoord(DocRect *pSpreadRect) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/96

	Inputs:		} pSpreadCoord/pSpreadRect - The coordinate/rectangle to be converted
	Outputs:	}

	Purpose:	To convert between Document and Spread coordinate spaces

	SeeAlso:	Spread::GetSpreadCoordOrigin

********************************************************************************************/

void Spread::SpreadCoordToDocCoord(DocCoord *pSpreadCoord) const
{
	ERROR3IF(pSpreadCoord == NULL, "Illegal NULL params");

	DocCoord Origin = GetSpreadCoordOrigin();

	pSpreadCoord->x += Origin.x;
	pSpreadCoord->y += Origin.y;
}



void Spread::SpreadCoordToDocCoord(DocRect *pSpreadRect) const
{
	ERROR3IF(pSpreadRect == NULL, "Illegal NULL params");

	SpreadCoordToDocCoord(&pSpreadRect->lo);
	SpreadCoordToDocCoord(&pSpreadRect->hi);
}



/********************************************************************************************

>	void Spread::DocCoordToSpreadCoord(DocCoord *pDocCoord) const
>	void Spread::DocCoordToSpreadCoord(DocCoord *pDocRect) const

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/3/96

	Inputs:		} pDocCoord/pDocRect - The coordinate/rectangle to be converted
	Outputs:	}

	Purpose:	To convert between Document and Spread coordinate spaces

	SeeAlso:	Spread::GetSpreadCoordOrigin

********************************************************************************************/

void Spread::DocCoordToSpreadCoord(DocCoord *pDocCoord) const
{
	ERROR3IF(pDocCoord == NULL, "Illegal NULL params");

	DocCoord Origin = GetSpreadCoordOrigin();

	pDocCoord->x -= Origin.x;
	pDocCoord->y -= Origin.y;
}



void Spread::DocCoordToSpreadCoord(DocRect *pDocRect) const
{
	ERROR3IF(pDocRect == NULL, "Illegal NULL params");

	DocCoordToSpreadCoord(&pDocRect->lo);
	DocCoordToSpreadCoord(&pDocRect->hi);
}



/********************************************************************************************

>	DocCoord Spread::GetMaxPasteboardSize(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/96

	Returns:	The maximum pasteboard size (x,y)

	Purpose:	To determine the maximum size that the pasteboard will allow itself
				to be expanded to.

	Notes:		In document coordinates, the bottom left of the pasteboard is fixed.
				However, by moving all objects inside the pasteboard (all spread coords)
				we can effectively expand the pasteboard in any direction. Thus, if the
				pasteboard width/height is smaller than the returned maximum, you can
				still expand the pasteboard in any direction. Thus, the return value from
				this method gives the maximum size rather than the max position rectangle.

	SeeAlso:	Spread::ExpandPasteboardToInclude

********************************************************************************************/

DocCoord Spread::GetMaxPasteboardSize(void)
{
#if !NEW_PASTEBOARD
	ERROR3("Warning: GetMaxPasteboardSize is unsupported at present! See Jason/Phil");
#endif

	DocCoord MPB(MaxPasteboardSize, MaxPasteboardSize);
	return(MPB);
}



/********************************************************************************************

>	BOOL Spread::ExpandPasteboardToInclude(DocRect IncludeRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/2/96

	Inputs:		IncludeRect - The rectangle (in SPREAD coordinates, i.e. standard renderable
				object bounds) which you wish to include into the spread's pasteboard range.

	Returns:	TRUE if the pasteboard was successfully expanded.
				FALSE if the pasteboard has reached its maximum size and cannot be expanded
				further (note: the pasteboard may still expand during this call, but the
				return code indicates that it has now reached maximum extent). The given
				rectangle lies outside the available maximum bounds.

	Purpose:	To expand the pasteboard area to include the given rectangle. Used when
				objects are moved outside the current pasteboard area to keep them in an
				area which the user can see. The pasteboard will never expand beyond
				a maximum size (see Spread::GetMaxPasteboardSize)

	Notes:		**** This has only been tested for single-spread documents ****

				The pasteboard expands equally in all directions in order to keep all
				page "margins" equal. This is necessary so that window scroll positions
				are not messed when we re-load a file

	SeeAlso:	Spread::GetMaxPasteboardSize; NodeRenderablePaper::ChangePasteboardRect

********************************************************************************************/

BOOL Spread::ExpandPasteboardToInclude(DocRect IncludeRect)
{
#if NEW_PASTEBOARD
#error ("This code has been commented out for safety. Please uncomment and try again");

	Progress Hourglass;		// Ensure an hourglass is running while we're busy

	// Convert the Spread coordinates "IncludeRect" into Document coordinates "IncludeDocRect"
	DocRect IncludeDocRect(IncludeRect);
	SpreadCoordToDocCoord(&IncludeDocRect);

	// If the pasteboard is already big enough, return immediately (success)
	if (PasteboardRect.ContainsRect(IncludeDocRect))
		return(TRUE);

	// Remember where the view was looking before we moved things about
	DocView *pView = DocView::GetSelected();
	DocRect ViewRect;
	if (pView != NULL)
	{
		ViewRect = pView->GetDocViewRect(this);
		DocCoordToSpreadCoord(&ViewRect);
	}

	// Inflate the IncludeRect in order to add a margin outside it. 72000mpt => 1 inch
	IncludeDocRect.Inflate(72000);

	// And include the rectangle into the new pasteboard rectangle
	DocRect NewBounds(PasteboardRect);
	NewBounds = NewBounds.Union(IncludeDocRect);

	// And now find the amount by which the "margin" should change, and inflate the pasteboard
	// rect in all directions, to keep the margin equal around the pages.
	INT32 InflateBy = 0;
	{
		InflateBy = ABS(NewBounds.lo.x - PasteboardRect.lo.x);

		INT32 temp = ABS(NewBounds.lo.y - PasteboardRect.lo.y);
		if (temp > InflateBy) InflateBy = temp;

		temp = ABS(NewBounds.hi.x - PasteboardRect.hi.x);
		if (temp > InflateBy) InflateBy = temp;

		temp = ABS(NewBounds.hi.y - PasteboardRect.hi.y);
		if (temp > InflateBy) InflateBy = temp;


		// Make sure we don't expand beyond the maximum size
		DocCoord MaxSize = GetMaxPasteboardSize();
		if (PasteboardRect.Width() + 2*InflateBy > MaxSize.x)
			InflateBy = (MaxSize.x - PasteboardRect.Width()) / 2;

		if (PasteboardRect.Height() + 2*InflateBy > MaxSize.y)
			InflateBy = (MaxSize.y - PasteboardRect.Height()) / 2;

		// And inflate the original pasteboard
		NewBounds = PasteboardRect;
		NewBounds.Inflate(InflateBy, InflateBy);
	}

	// Now we have the required size and position, go and set it
	ChangePasteboardRect(NewBounds);

	// Make sure bounding rectangles are invalidated so that they are recalculated
	InvalidateBoundingRect();

	// Now run through all spreads, lining up their pasteboards so they don't overlap,
	// and making sure that their default grids fully cover the pasteboards.
	// Note that this also ensures that the document is redrawn
	AdjustPasteboards();

	Document *pParentDoc = (Document *) FindOwnerDoc();
	if (pParentDoc != NULL)
	{
		// Pretend that page size has changed to cause related UI to change (rulers mainly)
		BROADCAST_TO_ALL(OptionsChangingMsg(pParentDoc,
							OptionsChangingMsg::NEWPAGESIZE));
	}

	// Try to keep the scroll offsets sensible
	if (pView != NULL && pView->GetDoc() == pParentDoc)
	{
		// First, scroll to the previously visible view rectangle - this will adjust for the
		// new size of the pasteboard (i.e. new size of the view window extent) to make the
		// view look like it has not moved at all.
		pView->ScrollToShow(&ViewRect);

		// Now, make sure that the IncludeRect is visible on screen
		pView->ScrollToShow(&IncludeRect);
	}

	return(TRUE);			// return - successful

#else
	// Pasteboard can never expand. Sniffle, Sob!
	// We return TRUE if the pasteboard already includes the specified rectangle, and
	// FALSE if we would have to expand to include it.

	// Convert the Spread coordinates "IncludeRect" into Document coordinates "IncludeDocRect"
	DocRect IncludeDocRect(IncludeRect);
	SpreadCoordToDocCoord(&IncludeDocRect);

	// If the pasteboard is already big enough, return immediately (success)
	if (PasteboardRect.ContainsRect(IncludeDocRect))
		return(TRUE);

	return(FALSE);
#endif
}



/********************************************************************************************

>	void Spread::AdjustPasteboards(void)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96

	Purpose:	This runs through all spreads in this spread's parent chapter, doing
				the following:

				1) Find the minimum and maximum x values of all spread pasteboards in
				   the chapter, and set all pasteboards to have this same x extent
				   so that they all line up tidily in a nice clean column.

				2) Move the pasteboards (and contents) as necessary so that recently
				   expanded pasteboards do not overlap neighbouring spreads, and
				   they all butt up against each other nicely.
				   (Note: The whole pasteboard is moved, including shifting the
				   spread coord origin to keep it in the same relative position)

				3) Ensure that all default grids fully cover their pasteboard.

				Finally, it will cause a redraw of all views onto this document to
				ensure that they are correctly updated.

	Notes:		**** This has only been tested for single-spread Chapters ****
				(Although it has been written to hopefully handle multiple spreads)

	SeeAlso:	Spread::GetMaxPasteboardSize; NodeRenderablePaper::ChangePasteboardRect

********************************************************************************************/

void Spread::AdjustPasteboards(void)
{
//#if NEW_PASTEBOARD
	Chapter* ParentChapter = FindParentChapter();
	ERROR3IF(ParentChapter == NULL, "No parent Chapter?!");

	// First, find the minimum and maximum pasteboard x extents for the entire document.
	// Helpfully, our parent Chapter's pasteboard rect is a union of all of ours!
	DocRect ChapterBounds = ParentChapter->GetPasteboardRect();

	// Now we loop through all spreads from this one onwards, doing 2 things
	// 1) Make sure that each following spread is shifted to the correct document coordinates
	//    by ensuring its pasteboard is placed after this spread's one.
	// 2) Make sure that all default grids in all spreads fully cover the pasteboards.
	Spread* pThisSpread = ParentChapter->FindFirstSpread();

	// Force the x extent of the first pasteboard to match all others in this chapter
	DocRect TempPasteRect = pThisSpread->GetPasteboardRect();
	TempPasteRect.lo.x = ChapterBounds.lo.x;
	TempPasteRect.hi.x = ChapterBounds.hi.x;

/*if (pThisSpread->FindNextSpread())
{
	DocCoord OriginOffset(pThisSpread->SpreadOrigin.x - pThisSpread->PasteboardRect.lo.x,
							pThisSpread->SpreadOrigin.y - pThisSpread->PasteboardRect.lo.y);

	DocRect r;
	pThisSpread->GetPagesRect(&r);
	pThisSpread->SpreadCoordToDocCoord(&r);
	r.Inflate(18000);
	INT32 ydiff = r.lo.y - TempPasteRect.lo.y;
	TempPasteRect.lo.y = r.lo.y;

	pThisSpread->SpreadOrigin.x = TempPasteRect.lo.x + OriginOffset.x;
	pThisSpread->SpreadOrigin.y = TempPasteRect.lo.y + OriginOffset.y - ydiff;
}
*/

	pThisSpread->ChangePasteboardRect(TempPasteRect);
	pThisSpread->InvalidateBoundingRect();

	// For each spread...
	while (pThisSpread != NULL)
	{
		Progress::Update();		// Update any active hourglass

		Spread* pNextSpread = pThisSpread->FindNextSpread();

		// --- If there is a spread following this one, fix its pasteboard rect position
		if (pNextSpread != NULL)
		{
			DocRect FirstPasteRect  = pThisSpread->GetPasteboardRect(FALSE);
/*if (pThisSpread->FindNextSpread())
{
	DocRect r;
	pThisSpread->GetPagesRect(&r);
	pThisSpread->SpreadCoordToDocCoord(&r);
	r.Inflate(18000);
	FirstPasteRect.lo.y = r.lo.y;
}
if (pThisSpread->FindPreviousSpread())
{
	DocRect r;
	pThisSpread->GetPagesRect(&r);
	pThisSpread->SpreadCoordToDocCoord(&r);
	r.Inflate(18000);
	FirstPasteRect.hi.y = r.hi.y;
}
*/
			DocRect SecondPasteRect = pNextSpread->GetPasteboardRect(FALSE);
			DocCoord OriginOffset(pNextSpread->SpreadOrigin.x - pNextSpread->PasteboardRect.lo.x,
									pNextSpread->SpreadOrigin.y - pNextSpread->PasteboardRect.lo.y);

/*if (pNextSpread->FindNextSpread())
{
	DocRect r;
	pNextSpread->GetPagesRect(&r);
	pNextSpread->SpreadCoordToDocCoord(&r);
	r.Inflate(18000);
	INT32 ydiff = r.lo.y - SecondPasteRect.lo.y;
	SecondPasteRect.lo.y = r.lo.y;
	OriginOffset.y -= ydiff;
}
if (pNextSpread->FindPreviousSpread())
{
	DocRect r;
	pNextSpread->GetPagesRect(&r);
	pNextSpread->SpreadCoordToDocCoord(&r);
	r.Inflate(18000);
	SecondPasteRect.hi.y = r.hi.y;
}
*/

			// Construct a new Pasteboard rectangle of the correct size, and move it
			// to lie just below the previous spread's pasteboard area
			TempPasteRect = SecondPasteRect;

			TempPasteRect.Translate(-TempPasteRect.lo.x, -TempPasteRect.lo.y);
			TempPasteRect.Translate(FirstPasteRect.lo.x, FirstPasteRect.lo.y - TempPasteRect.Height()); 

			// And force the x extent of this pasteboard to match all others in this chapter
			TempPasteRect.lo.x = ChapterBounds.lo.x;
			TempPasteRect.hi.x = ChapterBounds.hi.x;

			// Move the spread coordinate origin so that it stays at the same relative offset
			// from the pasteboard bottom left corner, or else all objects inside the spread
			// will suddenly shift to a new place on (or off) the pasteboard!
			pNextSpread->SpreadOrigin.x = TempPasteRect.lo.x + OriginOffset.x;
			pNextSpread->SpreadOrigin.y = TempPasteRect.lo.y + OriginOffset.y;

			// Set the new paste rect
			pNextSpread->ChangePasteboardRect(TempPasteRect);
		
			// Make sure bounding rectangles are invalidated so that they are recalculated
			pNextSpread->InvalidateBoundingRect();
		}

		// --- Fix all the default grids to exactly cover their pasteboards
		Node *ptr = pThisSpread->FindFirstChild();
		while (ptr != NULL)
		{
			if (ptr->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			{
				// Found a grid type node. Check if its a default type.
				NodeGrid* pCurrentGrid = (NodeGrid*)ptr;

				if (pCurrentGrid->IsDefault())
				{
					// Default grids need to fill the spread's pasteboard rect
					DocRect Bounds = pThisSpread->GetPasteboardRect();
					DocCoordToSpreadCoord(&Bounds);					

					// And set the new bounding rect
					pCurrentGrid->SetBoundingRect(Bounds);
				}
			}

			ptr = ptr->FindNext(); 
		}

		// --- And move on to the next spread to fix
		pThisSpread = pNextSpread;
	}

	// And invalidate all displayed views on this document, in order to make sure they
	// correctly update on screen.
	Document *pDoc = (Document *)FindOwnerDoc();
	if (pDoc != NULL)
		pDoc->ForceRedraw();

//#else
//ERROR3("Spread::AdjustPasteboards is unimplemented - see Jason/Phil");
//#endif
}



/***********************************************************************************************

>	virtual void Spread::SetInitialPasteboardRect(const DocRect& PasteRect)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/96

	Inputs:		PasteRect: Rectangle representing the outer limits of the pasteboard 

	Purpose:	To set the initial pasteboard rectangle of this node, and then change the 
				pasteboard rectangles of all its parents.

	            The method also changes the document extents in the NodeDocument node at 
				the root of the tree.

	Notes:		Spread overrides this base class method in order to set its Spread Coordinate
				Origin suitably. It then calls the base class to do the normal things.

	SeeAlso:	NodeRenderablePaper::SetInitialPasteboardRect

***********************************************************************************************/

void Spread::SetInitialPasteboardRect(const DocRect& PasteRect)
{
	// Call the base class to do its stuff
	NodeRenderablePaper::SetInitialPasteboardRect(PasteRect);

	// Set the spread coordinate origin to be at the bottom left of the pasteboard by default
	SpreadOrigin = PasteboardRect.lo;
}



/********************************************************************************************

> 	virtual BOOL Spread::PostImport()

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/3/96

	Inputs:		-
	Outputs:	-
	Returns:	TRUE/FALSE for success/failure

	Purpose:	This function is called after a document is imported.  Nodes should override
				this function to do any post-import processing.

				The spread overrides this function in order to automatically resize its
				pasteboard to include all the newly imported nodes if possible.

	Notes:		Calls the base class PostImport after doing its own processing

	SeeAlso:	Node::PostImport

********************************************************************************************/

BOOL Spread::PostImport()
{
	// Get the bounding rect. This comes back in document coords, so we convert to spread
	// coords and then call ExpandPasteboardToInclude to include our bounds in the PB area.
	if (NodeRenderablePaper::PostImport())
	{
		DocRect Bounds = GetBoundingRect();
		DocCoordToSpreadCoord(&Bounds);
		ExpandPasteboardToInclude(Bounds);
		return(TRUE);
	}

	return(FALSE);
}


//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

/********************************************************************************************

>	virtual BOOL Spread::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Web files do not write out spreads.
				
				This code assumes that the document only has one spread

	SeeAlso:	Node::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

********************************************************************************************/

BOOL Spread::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}

/********************************************************************************************

>	virtual BOOL Spread::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the spread record to the filter

				Native files write out all spreads.

	SeeAlso:	Node::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

********************************************************************************************/

BOOL Spread::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	
	BOOL RecordWritten = FALSE;

	// Always write out the spread record in native files
	CXaraFileRecord Rec(TAG_SPREAD,TAG_SPREAD_SIZE);
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

> 	BOOL Spread::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Begins the child record sequence for spread in the native format

				The native format writes out all spreads and includes the spread details record 
				as the spread's first child record

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Spread::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL RecordWritten = FALSE;

	// First thing to do is write out the Down record
	RecordWritten = (pFilter->WriteZeroSizedRecord(TAG_DOWN));
	
	// Write out the page sizes etc for this spread
	if (RecordWritten) RecordWritten = WriteSpreadInformation(pFilter);
	if (RecordWritten) RecordWritten = WriteSpreadScaling(pFilter);
	
	// write out the  Animtion Record.
	if (RecordWritten) RecordWritten = WriteSpreadAnimProperties(pFilter);
	
	return RecordWritten;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

> 	BOOL Spread::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Ends the child record sequence for spreads in the native format

				The native format writes out all spreads.  This func just writes out the Up record

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Spread::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	return pFilter->WriteZeroSizedRecord(TAG_UP);
#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL Spread::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Begins the child record sequence for spread in the web format

				Web export doesn't write out spread records, so this overrides the default
				behaviour in Node by ensuring the Down record does not get written

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Spread::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

/********************************************************************************************

> 	BOOL Spread::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE ok, FALSE otherwise
	Purpose:	Ends the child record sequence for spread in the web format

				Web export doesn't write out spread records, so this overrides the default
				behaviour in Node by ensuring the Up record does not get written

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Spread::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
	return TRUE;
}

/********************************************************************************************

>	BOOL Spread::WriteSpreadAnimationProperties(BaseCamelotFilter* pFilter)
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/97
	Input:		pFilter = ptr to the filter to export to.
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise.
	Purpose:	Exports the Animation Properties details for this Spread to the filter.
	Notes:		Karim 07/12/2000 - modified to save out transparent GIF information.

*********************************************************************************************/

BOOL Spread::WriteSpreadAnimProperties(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL RecordWritten = FALSE;

	BOOL ok = TRUE;
		
	// Add a description of the TAG_SPREAD_ANIMPROPS record, for older importers that don't understand this record
	pFilter->AddTagDescription(TAG_SPREAD_ANIMPROPS, _R(IDS_TAG_SPREAD_ANIMPROPS));

	// Start a record to save our data in
	CXaraFileRecord Rec(TAG_SPREAD_ANIMPROPS, TAG_SPREAD_ANIMPROPS_SIZE);

	// Get our information...
	DWORD	Loop				=	m_AnimPropertiesParam.GetAnimLoop();
	DWORD	GlobalDelay			=   m_AnimPropertiesParam.GetGlobalAnimDelay();
	DWORD	Dither				=	m_AnimPropertiesParam.GetDither();
	DWORD	WebPalette			=	m_AnimPropertiesParam.GetPalette();
	DWORD	ColoursPalette		=	m_AnimPropertiesParam.GetPaletteCols();
	DWORD	NumColsInPalette	=	m_AnimPropertiesParam.GetNumColsInPalette();

	// We have a flags word so get the bits that affect it
	BOOL	SystemCols			=	m_AnimPropertiesParam.GetUseSystemCols();
	BOOL	BgTransparent		=	m_AnimPropertiesParam.GetIsBackGroundTransp();

	// And now fill in the bits in the flags word.
	// To be compatible with CX2, we set the flag if BgTransparent is FALSE,
	// as CX2 automatically did bg transparent GIFs all the time.
	BYTE	FlagsWord			=	(SystemCols		? 1 : 0);
			FlagsWord			|=	(BgTransparent	? 0 : 2);

	ok = Rec.Init();
	
	if (ok)
		ok = Rec.WriteUINT32(Loop);	
	if (ok)
		ok = Rec.WriteUINT32(GlobalDelay);
	if (ok)
		ok = Rec.WriteUINT32(Dither);
	if (ok)
		ok = Rec.WriteUINT32(WebPalette);
	if (ok)
		ok = Rec.WriteUINT32(ColoursPalette);
	if (ok)
		ok = Rec.WriteUINT32(NumColsInPalette);
	if (ok)
		ok = Rec.WriteUINT32(FlagsWord);

	// Write out the record.
	if (pFilter->Write(&Rec) != 0)
		RecordWritten = TRUE;
	else
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return RecordWritten;
#else
	return FALSE;	
#endif 
	}

/******************************************************************************************************

>	void Spread::SetAnimationLoop(const DWORD &Loop)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/97
	Input:		Loop :- Global loop value.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the loop value within the class data member m_AnimPropertiesParam.
				This value has been recieved from the Animation properties tab.
				
*******************************************************************************************************/
void Spread::SetAnimationLoop(const DWORD &Loop)
{
	m_AnimPropertiesParam.SetAnimLoop(Loop);
}


/******************************************************************************************************

>	void Spread::SetAnimationDelay(const DWORD GlobalDelay)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/97
	Input:		Global Delay :- Global Delay value.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the the delay value within the class data member m_AnimPropertiesParam.
				This value has been recieved from the Animation properties tab.
				
*******************************************************************************************************/
void Spread::SetAnimationDelay(const DWORD GlobalDelay)
{
	m_AnimPropertiesParam.SetGlobalanimDelay(GlobalDelay);
}

/******************************************************************************************************

>	void SetAnimationColours(const DITHER& Dither, const WEB_PALETTE& WebPalette, 
							 const PALETTE_COLOURS& ColoursPalette, const DWORD& NumColsInPalette,
							 const BOOL& IsBackgroundTransparent);					
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/97
	Input:		Dither, WebPalette, ColoursPalette, NumColsInPalette,
				IsBackgroundTransparent:- Animation colour preferences.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the values for the animation colours within the class data member m_AnimPropertiesParam.
				These values have been recieved from the Animation properties tab.
	Notes:		Karim 07/12/2000 - added IsBackgroundTransparent parameter.
				
*******************************************************************************************************/
void Spread::SetAnimationColours(const DITHER& Dither, const WEB_PALETTE& WebPalette, 
								 const PALETTE_COLOURS& ColoursPalette, const DWORD& NumColsInPalette,
								 const BOOL& IsBackgroundTransparent)
{
	// test if we need to junk any cached bitmaps
	if (Dither != m_AnimPropertiesParam.GetDither() ||
		WebPalette != m_AnimPropertiesParam.GetPalette() ||
		ColoursPalette != m_AnimPropertiesParam.GetPaletteCols() ||
		NumColsInPalette != m_AnimPropertiesParam.GetNumColsInPalette() ||
		IsBackgroundTransparent != m_AnimPropertiesParam.GetIsBackGroundTransp())
	{
		// force a refresh (regrab) of all frames as
		// the palette has changed etc..
PORTNOTETRACE("OpGrabAllFrames","Spread::SetAnimationColours - Not setting OpGrabAllFrames force refresh flag");
#ifndef EXCLUDE_FROM_XARALX
		OpGrabAllFrames::ms_ForceRefreshOfAllFrames = TRUE;
#endif
	}

	m_AnimPropertiesParam.SetDither(Dither);
	m_AnimPropertiesParam.SetPalette(WebPalette);
	m_AnimPropertiesParam.SetPaletteCols(ColoursPalette);
	m_AnimPropertiesParam.SetNumColsInPalette(NumColsInPalette);
	m_AnimPropertiesParam.SetIsBackGroundTransp(IsBackgroundTransparent);
}

/******************************************************************************************************

>	void GetAnimationColours(DITHER * pDither, WEB_PALETTE * pWebPalette, 
							 PALETTE_COLOURS * pColoursPalette, DWORD * pNumColsInPalette,
							 BOOL * pIsBackgroundTransparent);
	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/97
	Outputs:	Dither, WebPalette, ColoursPalette, NumColsInPalette,
				IsBackgroundTransparent:- Animation colour preferences.
	Returns:	-
	Purpose:	Gets the values for the animation colours within the class data member m_AnimPropertiesParam.
	Notes:		Karim 07/12/2000 - added IsBackgroundTransparent parameter.
				
*******************************************************************************************************/

void Spread::GetAnimationColours(DITHER * pDither, WEB_PALETTE * pWebPalette, 
								 PALETTE_COLOURS * pColoursPalette, DWORD * pNumColsInPalette,
								 BOOL * pIsBackgroundTransparent)
{
	if (pDither != NULL)
		*pDither = m_AnimPropertiesParam.GetDither();
	if (pWebPalette != NULL)
		*pWebPalette = m_AnimPropertiesParam.GetPalette();
	if (pColoursPalette != NULL)
		*pColoursPalette = m_AnimPropertiesParam.GetPaletteCols();
	if (pNumColsInPalette != NULL)
		*pNumColsInPalette = m_AnimPropertiesParam.GetNumColsInPalette();
	if (pIsBackgroundTransparent != NULL)
		*pIsBackgroundTransparent = m_AnimPropertiesParam.GetIsBackGroundTransp(); 
}

/********************************************************************************************

>	BOOL Spread::WriteSpreadInformation(BaseCamelotFilter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports the page details, margins, bleed size and other spread related
				information for this spread	to the filter.
				We output an information record rather than the individual pages as:-
					- all the pages in the spread are defined to be the same size
					- this is the same way that the old format used (not a good argument I know!)
					- all the code to do it this way is present and proved
					- it is a logical packing of all spread related information into one record 
	SeeAlso:	Spread::WritePreChildrenNative; Spread::WritePreChildrenWeb;

********************************************************************************************/

BOOL Spread::WriteSpreadInformation(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

//	BOOL RecordWritten = FALSE;

	// Now save out the information record
	MILLIPOINT Width = 0;
	MILLIPOINT Height = 0;
	MILLIPOINT Margin = 0;
	MILLIPOINT Bleed = 0;
	BOOL Dps = TRUE;
	BOOL ShowDropShadow = TRUE;

	BOOL ok = TRUE;

	// and go and get all the page info
	if (ok) ok = GetPageSize(&Width, &Height, &Margin, &Bleed, &Dps, &ShowDropShadow);

	// If it worked, we had better try and output some info
	if (ok)
	{
		CXaraFileRecord Rec(TAG_SPREADINFORMATION, TAG_SPREADINFORMATION_SIZE);

		ok = Rec.Init();

		// Width, Height of page
		if (ok) ok = Rec.WriteINT32(Width);	
		if (ok) ok = Rec.WriteINT32(Height);
		// <Margin : MILLIPOINT>	The margin to add around all four sides of the pages in the spread to make up the pasteboard. 
		if (ok) ok = Rec.WriteINT32(Margin);	
		// <Bleed : MILLIPOINT>	Bleed margin to add around all pages in this spread. (0 means none)
		if (ok) ok = Rec.WriteINT32(Bleed);	
		// <SpreadFlags : BYTE>	Flags for the current spread.
		// SpreadFlags ::= DoublePageSpread | ShowDropShadow
		// ShowDropShadow flag to say whether we apply a page shadow behind the page
		// DoublePageSpread flag to say whether one or two pages are present 
		BYTE FlagsWord = (ShowDropShadow ? 2 : 0) | (Dps ? 1 : 0);
		if (ok) ok = Rec.WriteBYTE(FlagsWord);	

		// Finally, write the record out to file
		if (ok) ok = (pFilter->Write(&Rec) != 0);
	}
		
	return ok;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	BOOL Spread::WriteSpreadScaling(BaseCamelotFilter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports the scaling information for this spread to the filter
	SeeAlso:	Spread::WritePreChildrenNative; Spread::WritePreChildrenWeb;

********************************************************************************************/

BOOL Spread::WriteSpreadScaling(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = FALSE;
	
	DimScale* pDimScale = GetPtrDimScale();
	if (pDimScale != NULL)
	{
		// Find out if it is active
		BOOL Active = pDimScale->IsActive();

		// Find out about the scale factor strings
		String_32 DrawingScale = pDimScale->GetDrawingScaleStr();
		String_32 RealScale = pDimScale->GetRealScaleStr();

		// If we just save the strings directly, we will named references to the units being used.
		// This is bad as if somebody saves out '10cm' and then loads this onto a Japanese version,
		// they will not get what they expected. If we strip the scaling factor and units from the strings
		// and save this instead, the units as a units reference, then everything will be happy. The units
		// will get loaded in and converted into the correct locale format. (Apart from user units of course).
		UnitType Default = pDimScale->GetUnits();

		double 		DrawingScaleValue	= 1.0;
		UnitType 	DrawingUnits		= NOTYPE;
		ok = Convert::StringToComponents(DrawingScale, &DrawingScaleValue, &DrawingUnits);

		if (DrawingUnits == NOTYPE)
			DrawingUnits = Default;

		double 		RealScaleValue		= 1.0;
		UnitType 	RealUnits			= NOTYPE;
		ok = Convert::StringToComponents(RealScale, &RealScaleValue, &RealUnits);

		if (RealUnits == NOTYPE)
			RealUnits = Default;
		
		// Convert the unittype into a reference
		UnitListComponent * pUnitsComponent = pFilter->GetUnitDocComponent();
		ERROR2IF(pUnitsComponent == NULL,FALSE,"WriteGridAndRulerSettings No units doc component present");

		INT32 DrawingUnitsRef = pUnitsComponent->GetWriteUnitReference(DrawingUnits, pFilter);
		INT32 RealUnitsRef = pUnitsComponent->GetWriteUnitReference(RealUnits, pFilter);

		INT32 Tag = TAG_SPREADSCALING_INACTIVE;
		INT32 Size = TAG_SPREADSCALING_INACTIVE_SIZE;
		if (Active)
		{
			Tag = TAG_SPREADSCALING_ACTIVE;
			Size = TAG_SPREADSCALING_ACTIVE_SIZE;
		}

		CXaraFileRecord Rec(Tag, Size);

		ok = Rec.Init();

		if (ok) ok = Rec.WriteDOUBLE(DrawingScaleValue);	
		if (ok) ok = Rec.WriteINT32(DrawingUnitsRef);	
		if (ok) ok = Rec.WriteDOUBLE(RealScaleValue);	
		if (ok) ok = Rec.WriteINT32(RealUnitsRef);	

		// Finally, write the record out to file
		// In the process get the record number that this was written out as
		INT32 RecordNumber = 0L;
		if (ok) RecordNumber = pFilter->Write(&Rec);

		// If we have had a problem at any of the stages then return that to the caller
		if (!ok || RecordNumber <= 0)
			ok = FALSE;
	}

	return ok;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	BOOL Sperad::SetAnimPropertiesParam(AnimPropertiesParam* pParam)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/05/97
	Inputs:		pParam - Sets the Animation Properties details for this spread.
	Returns:	TRUE if ok, FALSE otherwise.
	
********************************************************************************************/
void Spread::SetSpreadAnimPropertiesParam(const AnimPropertiesParam& Param)
{
	m_AnimPropertiesParam = Param;
}

/********************************************************************************************

>	void Sperad::GetAnimPropertiesParam()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/05/97
	Purpose:	Returns the the Animation Properties details for this spread.
		
********************************************************************************************/
AnimPropertiesParam& Spread::GetSpreadAnimPropertiesParam()
{
	return m_AnimPropertiesParam;
}

/********************************************************************************************************************
>	BOOL Spread::SetSpreadAnimPropertiesParam(const DWORD &Loop, const DWORD &GlobalDelay, const DITHER &Dither, 
												const WEB_PALETTE &WebPalette, const PALETTE_COLOURS &ColoursPalette,
												const DWORD &NumColsInPalette, const BOOL& UseSystemColours,
												const BOOL& IsBackgroundTransparent)
	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/05/97
	Purpose:	Sets the Animation Properties details for this spread.
	Notes:		Karim 07/12/00 - added IsBackgroundTransparent parameter.
		
*********************************************************************************************************************/

BOOL Spread::SetSpreadAnimPropertiesParam(const DWORD &Loop, const DWORD &GlobalDelay, const DITHER &Dither, 
											const WEB_PALETTE &WebPalette, const PALETTE_COLOURS &ColoursPalette,
											const DWORD &NumColsInPalette, const BOOL& UseSystemColours,
											const BOOL& IsBackgroundTransparent)
{
	ERROR2IF(this==NULL,FALSE,"Spread::SetSpreadAnimProperties cslled on NULL pointer");

	m_AnimPropertiesParam.SetAnimLoop(Loop);
	m_AnimPropertiesParam.SetGlobalanimDelay(GlobalDelay);	
	m_AnimPropertiesParam.SetDither(Dither);
	m_AnimPropertiesParam.SetPalette(WebPalette);
	m_AnimPropertiesParam.SetPaletteCols(ColoursPalette);
	m_AnimPropertiesParam.SetNumColsInPalette(NumColsInPalette);
	m_AnimPropertiesParam.SetUseSystemCols(UseSystemColours);
	m_AnimPropertiesParam.SetIsBackGroundTransp(IsBackgroundTransparent);

	return TRUE;
}

/********************************************************************************************************************
BOOL Spread::GetSpreadAnimPropertiesParam(DWORD *Loop, DWORD *GlobalDelay, DITHER *Dither, 
											WEB_PALETTE *WebPalette, PALETTE_COLOURS* ColoursPalette,
											DWORD* NumColsInPalette, BOOL* UseSystemColours,
											BOOL* IsBgTransparent) 

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/04/97
	Purpose:	Allows access to the Animation Properties details for this spread.
	Notes:		Karim 07/12/00 - added IsBgTransparent parameter.
		
*********************************************************************************************************************/
BOOL Spread::GetSpreadAnimPropertiesParam(DWORD *Loop, DWORD *GlobalDelay, DITHER *Dither, 
											WEB_PALETTE *WebPalette, PALETTE_COLOURS* ColoursPalette,
											DWORD* NumColsInPalette, BOOL* UseSystemColours,
											BOOL* IsBgTransparent)
{
	ERROR2IF(this==NULL,FALSE,"Spread::GetSpreadAnimProperties cslled on NULL pointer");

	*Loop				=		 m_AnimPropertiesParam.GetAnimLoop();
	*GlobalDelay		=		 m_AnimPropertiesParam.GetGlobalAnimDelay();	
	*Dither				=		 m_AnimPropertiesParam.GetDither();
	*WebPalette			=		 m_AnimPropertiesParam.GetPalette();
	*ColoursPalette		=		 m_AnimPropertiesParam.GetPaletteCols();
	*NumColsInPalette	=	 	 m_AnimPropertiesParam.GetNumColsInPalette();
	*UseSystemColours	=		 m_AnimPropertiesParam.GetUseSystemCols();
	if (IsBgTransparent != NULL)
		*IsBgTransparent=		 m_AnimPropertiesParam.GetIsBackGroundTransp();

	return TRUE;
}

/********************************************************************************************

>	void Spread::SetAnimationBoundingRect(const DocRect& BoundingRect)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Inputs:		BoundingRect - the new bounding rect for this spread.
	Purpose:	Sets the new stored bounding rect for this spread.

********************************************************************************************/

void Spread::SetAnimationBoundingRect(const DocRect& BoundingRect)
{
	m_AnimPropertiesParam.SetBoundingRect(BoundingRect);
}

/********************************************************************************************

>	DocRect Spread::GetAnimationBoundingRect()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/06/97
	Returns:	the stored bounding rect for the animation
	Purpose:	Returns the stored animation bounding rect for this spread.
		
********************************************************************************************/

DocRect Spread::GetAnimationBoundingRect()
{
	return m_AnimPropertiesParam.GetBoundingRect();
}


/********************************************************************************************

>	void Spread::SetAnimationQuality(const Quality& NewQuality)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/97
	Inputs:		NewQuality - the new capture quality for this spread.
	Purpose:	Sets the new stored quality that the animation for this spread was captured at.
  
********************************************************************************************/

void Spread::SetAnimationQuality(const Quality& NewQuality)
{
	m_AnimPropertiesParam.SetAnimationQuality(NewQuality);
}


/********************************************************************************************

>	Quality Spread::GetAnimationQuality()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/97
	Returns:	the stored quality for the animation
	Purpose:	Returns the stored quality that the animation for this spread was captured at.
		
********************************************************************************************/

Quality Spread::GetAnimationQuality()
{
	return m_AnimPropertiesParam.GetAnimationQuality();
}


/********************************************************************************************

>	SubtreeRenderState Spread::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/07/2004
	Returns:	indicator as to whether rendering can proceed in this subtree
	Purpose:	-
		
********************************************************************************************/

SubtreeRenderState Spread::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	return SUBTREE_ROOTANDCHILDREN;
}


/********************************************************************************************

>	BOOL Spread::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly = FALSE,
											BOOL CheckSelected = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/2004
	Inputs:		pRender - A pointer to the current export region (null if none)
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
									   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	FALSE => we never want to export NodeRenderablePaper objects.
	Purpose:	Indicate that we don't want to export this class of nodes.
	SeeAlso:	NodeRenderablePaper::NeedsToRender

********************************************************************************************/

BOOL Spread::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
	return TRUE;
}

     
