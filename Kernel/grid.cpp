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
// NodeGrid implementation, and associated classes

/*
*/

#include "camtypes.h"

#include <math.h>

//#include "ensure.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rndrgn.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "osrndrgn.h"
#include "paper.h"
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "markn.h"
//#include "convert.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "grid.h"
//#include "units.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "chapter.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]

//#include "cxfrec.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "cxftags.h"
//#include "cxfdefs.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "camfiltr.h"	// BaseCamelot filter - in camtypes.h [AUTOMATICALLY REMOVED]
#include "unitcomp.h"	// UnitListComponent

const UINT32 MIN_PIX_RENDER_COUNT 	= 10;	// Min distance between rendered grid points, in pixels
const UINT32 PIX_CROSSHAIR_SIZE	 	= 2; 	// Size of grid crosshair points, in pixels
const INT32 GRID_REND_FACTOR			= 128;	// Fixed FP for rendering grid

static double 		mod(double n, double d)	{ double temp; return (modf(n/d,&temp)*d);}
static double 		sgn(double n) 			{ if (n<0) return (-1); else return (1); }

static UnitType GetDominantUnit(UnitType ThisUnit);
static double GetUnitMultiple(UnitType Units);

#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

UINT32 		NodeGrid::NumSelected 			= 0;
UINT32 		NodeGrid::NumNonDefaultSelected = 0;
INT32 		NodeGrid::LastXDir 			    = 0;
INT32 		NodeGrid::LastYDir 			    = 0;

UnitType 	NodeGrid::DefaultUnits 			= INCHES;
double		NodeGrid::DefaultDivisions 		= 1.0;
UINT32		NodeGrid::DefaultSubdivisions 	= 2;
GridType	NodeGrid::DefaultGridType		= RECTANGULAR;
GridType	NodeGrid::TypeForDefaultGrid	= RECTANGULAR;
BOOL		NodeGrid::DefaultGridDisabled	= FALSE;

//--------------------------------------------------------------------------------------------
// NodeGrid methods         

CC_IMPLEMENT_DYNAMIC(NodeGrid, NodeRenderablePaper)          

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/*********************************************************************************************

>    NodeGrid::NodeGrid() 

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   8/2/94
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeGrid linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

NodeGrid::NodeGrid(): NodeRenderablePaper()
{
	SetOrigin(0,0);   
	GridColour	= DocColour(COLOUR_GRID);

	GridFlags.Selected 			= FALSE;
	GridFlags.RelativeOrigin 	= FALSE;
	GridFlags.DefaultGrid 		= FALSE;
	GridFlags.Disabled			= FALSE;

	GridBoundingRect = DocRect(0,0,0,0);
}



/***********************************************************************************************

> Node* NodeGrid::SimpleCopy() // Private method  

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/2/94
	
	Inputs:         - 
    Outputs:    
    Returns:    A copy of the node, or NULL if memory runs out
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
    Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
 
        
	Scope:      protected       
**********************************************************************************************/

Node* NodeGrid::SimpleCopy()
{
	NodeGrid* NodeCopy = new NodeGrid();   
	if (NodeCopy != NULL)
		CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}          

/***********************************************************************************************
>   void NodeGrid::CopyNodeContents(NodeGrid* NodeCopy)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/2/94
	
	Inputs:		- 
	Outputs:    A copy of this node
 
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/


void NodeGrid::CopyNodeContents(NodeGrid* NodeCopy)
{
	ENSURE(NodeCopy != NULL,"Trying to copy a NodeGridRect's contents to a NULL node");  
	
	NodeRenderablePaper::CopyNodeContents(NodeCopy); 

	NodeCopy->XOrigin    = this->XOrigin;
	NodeCopy->YOrigin    = this->YOrigin;
	NodeCopy->GridColour = this->GridColour;
	NodeCopy->GridFlags  = this->GridFlags;

	for (INT32 i=0;i<NumGridBlobTypes;i++)
		NodeCopy->BlobCoords[i] = this->BlobCoords[i];
}


/***********************************************************************************************
>   void NodeGrid::PolyCopyNodeContents(NodeGrid* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeGrid::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeGrid), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeGrid))
		CopyNodeContents((NodeGrid*)pNodeCopy);
}



/********************************************************************************************

>	void* NodeGrid::GetDebugDetails(StringBase* Str) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void NodeGrid::GetDebugDetails(StringBase* Str) 
{          
	NodeRenderablePaper::GetDebugDetails(Str); 
}

/********************************************************************************************

>	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
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
              
String NodeGrid::Describe(BOOL /*Plural*/, BOOL /*Verbose*/) 
{     
	ENSURE (FALSE,"The illegal function NodeGrid::Describe was called\n"); 
	
	return _T(""); // Just to keep the compiler happy
}; 

/***********************************************************************************************

>	virtual void NodeGrid::Render(void)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		RendRegion: Render region to render into
	Outputs:	-
	Returns:	-   
	Purpose:    For rendering a node
		
***********************************************************************************************/

void NodeGrid::Render( RenderRegion* /*pRender*/ )
{
}

/********************************************************************************************

>	BOOL NodeGrid::NeedsToRender(RenderRegion *pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		pRender - A pointer to the current render region (null if none)
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	This function will indicate to the caller whether or not
				we want to render the given node, according to the information passed in.
	SeeAlso:	NodeRenderableBounded::NeedsToRender

********************************************************************************************/

SubtreeRenderState NodeGrid::RenderSubtree(RenderRegion *pRender, Node** ppNextNode, BOOL bClip)
{
	// If no cliprect supplied, assume we do need to render
	if (pRender==NULL || !bClip)
		return SUBTREE_ROOTANDCHILDREN;

	DocRect ClipRect = pRender->GetClipRect();

	return ClipRect.IsIntersectedWith(GridBoundingRect) ? SUBTREE_ROOTANDCHILDREN : SUBTREE_NORENDER;
}

/***********************************************************************************************

> void NodeGrid::CalcGridRenderSteps(MILLIPOINT PixelWidth,double* pStep,double DomStep,UnitType Units)
    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    10/2/94
	Inputs:		PixelWidth	= width of a pixel in millipoints
				pStep		= reference to resultant render step (IMPORTANT: holds initial step on entry)
				DomStep		= the number of millipoints between each dominant division
				Units		= Units used to define the render step
    Outputs:
    Returns:	-
    Purpose:    Works out what the step value should be used to render the grid with
				the give width of a pixel and the given units of the grid.
    Errors:     
	Scope:		private

**********************************************************************************************/
/* 	Technical note
	
	The following rules have been applied :

	1. 	If the grid points are further apart than MIN_PIX_RENDER_COUNT pixels, then return these values,
		i.e. this value will look OK with the given pixel width
	2.	Otherwise find a grid step that is a multiple of the initial grid step, a factor of the
		dominant grid step, and is wider than MIN_PIX_RENDER_COUNT pixels.

*/

void NodeGrid::CalcGridRenderSteps(	MILLIPOINT PixelWidth,
									double*    pStep,	// entry = initial step, exit = modified step
									double     DomStep,	// Distance between dominant steps
									UnitType   Units)	// Units of the grid
{
	if (Units == NOTYPE) return;  // The get-out clause of the recursive contract

	double MinRenderWidth = (double)(PixelWidth*MIN_PIX_RENDER_COUNT);	// min distance between rendered grid points

	if ((DomStep < MinRenderWidth))
	{
		double ExtraDomStep = DomStep*::GetUnitMultiple(Units);
		CalcGridRenderSteps(PixelWidth,&DomStep,ExtraDomStep,::GetDominantUnit(Units));
	}

	double InitialStep = *pStep;
	double RenderStep  = *pStep;

	// Apply the grid step rules to RenderXStep

	if (RenderStep < MinRenderWidth)
	{
		while (RenderStep < MinRenderWidth)
		{
			do
			{
				RenderStep += InitialStep;
			} while ((mod(DomStep,RenderStep) > (InitialStep/2)) && (RenderStep < DomStep));
		}
	}

	*pStep = RenderStep;
}



/***********************************************************************************************

>	virtual BOOL NodeGrid::Snap(DocCoord* pDocCoord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		A DocCoord in Spread coords
	Outputs:	-
	Returns:	FALSE - ALWAYS!
	Purpose:    Place holder for derived classes
	SeeAlso:	NodeGridRect::Snap
		
***********************************************************************************************/

BOOL NodeGrid::Snap(DocCoord* /*pDocCoord*/) 
{
	return FALSE;
}

/***********************************************************************************************

>	virtual BOOL NodeGrid::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		A DocRect, and two coords
	Outputs:	-
	Returns:	FALSE - ALWAYS!
	Purpose:    Place holder for derived classes
	SeeAlso:	NodeGridRect::Snap
		
***********************************************************************************************/

BOOL NodeGrid::Snap(DocRect* /*pDocRect*/,const DocCoord& /*PrevCoord*/,const DocCoord& /*CurCoord*/)
{
	return (FALSE);
}

/***********************************************************************************************

> MILLIPOINT NodeGrid::SnapOrdinate(MILLIPOINT ord,double Step,MILLIPOINT origin)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/3/94
	Inputs:		ord		= the ordinate to snap 
				Step 	= distance between each snap point on the line
				origin	= origin of first snap point of the line
	Outputs:	
	Returns:	Snapped version of ord
	Purpose:	General linear floating point ordinate snapping routine
	Errors:		BANG! if step = 0
	Scope:		protected
								   	

***********************************************************************************************/

MILLIPOINT NodeGrid::SnapOrdinate(MILLIPOINT ord,double Step,MILLIPOINT origin)
{
	double fp_ord 	= ord;

	fp_ord -= origin;  						// Translate to 0

	double sign 	= sgn(fp_ord);			// sign = 1 when fp_ord is >=0, sign = -1 when fp_ord<0

	fp_ord *= sign;							// make +ve if it is -ve

	double dist	= mod(fp_ord,Step); 
	if (dist > (Step/2)) dist -= Step;		// if closest to min adjacent snap point, snap to it
	fp_ord -= dist;

	fp_ord += 0.5;							// Round up before convertion to MILLIPOINTS, and while still +ve
	fp_ord *= sign;							// make -ve again if it was -ve on entry
	fp_ord += origin;						// Translate back to the origin

	return (MILLIPOINT)fp_ord;
}

/***********************************************************************************************

> void NodeGrid::UpdateBlobsData()

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/2/94
	
	Inputs:
    Outputs:    
    Returns:
    Purpose:    Updates the internel select blobs data for the grid.
				This involves putting the coords of all the select blobs into the BlobCoords
				array of the grid

**********************************************************************************************/

void NodeGrid::UpdateBlobsData()
{
	DocRect& BB = GridBoundingRect;

	BlobCoords[BottomLeft]  = DocCoord(BB.lo.x,BB.lo.y);
	BlobCoords[TopLeft]		= DocCoord(BB.lo.x,BB.hi.y);
	BlobCoords[TopRight]	= DocCoord(BB.hi.x,BB.hi.y);
	BlobCoords[BottomRight]	= DocCoord(BB.hi.x,BB.lo.y);

	MILLIPOINT midX = BB.lo.x+((BB.hi.x-BB.lo.x)/2);
	MILLIPOINT midY = BB.lo.y+((BB.hi.y-BB.lo.y)/2);

	BlobCoords[LeftMiddle]		= DocCoord(BB.lo.x,midY);
	BlobCoords[RightMiddle]		= DocCoord(BB.hi.x,midY);
	BlobCoords[TopMiddle]		= DocCoord(midX,BB.hi.y);
	BlobCoords[BottomMiddle]	= DocCoord(midX,BB.lo.y);
}

/***********************************************************************************************

> DocCoord NodeGrid::GetBlobCoord(GridBlobType GridBlob)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/2/94
	
	Inputs:		The blob type whos coord is required
    Outputs:    
    Returns:	The coord of that blob
    Purpose:    Returns the coord of the required blob of the grid

**********************************************************************************************/

DocCoord NodeGrid::GetBlobCoord(GridBlobType GridBlob)
{
	return BlobCoords[GridBlob];
}

/***********************************************************************************************

> void NodeGrid::SetBoundingRect(const DocRect& Rect)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/2/94
	Inputs:		The bounding rect for grid
    Outputs:    
    Returns:
    Purpose:    Sets the bounding rect of the grid, plus any other data that is needed.
				Should call this after the Node's bounds has been set up using something like
				SetInitialBounds.

**********************************************************************************************/

void NodeGrid::SetBoundingRect(const DocRect& Rect)
{
	GridBoundingRect = Rect;
	UpdateBlobsData();
}

/***********************************************************************************************

> DocRect NodeGrid::GetBoundingRect()

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    23/2/94
	Inputs:		
    Outputs:    
    Returns:	The bounding rect for grid
    Purpose:    Returns the bounding rect of the grid.

**********************************************************************************************/

DocRect NodeGrid::GetBoundingRect()
{
	return GridBoundingRect;
}

/***********************************************************************************************

> DocRect NodeGrid::GetBlobBoundingRect()

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/3/94
	Inputs:		
    Outputs:    
    Returns:	The blob bounding rect for grid
    Purpose:    Returns the blob bounding rect of the grid.

**********************************************************************************************/

DocRect NodeGrid::GetBlobBoundingRect()
{
	// Find the paths bounding rectangle
	DocRect Rect = GridBoundingRect;

	// And if we can find the selected view, add on the size of a selection blob
	DocView* pDocView = DocView::GetSelected();
	if ( pDocView != NULL )
	{
		DocRect BlobSize;
		
		// Add the size of a blob to the top corner of the bounding rectangle
		OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Rect.HighCorner(), 
									BT_SELECTEDLARGEST, &BlobSize);
		Rect.IncludePoint( BlobSize.HighCorner() );
		
		// add the size of a blob to the bottom corner of the bounding rectangle
		OSRenderRegion::GetBlobRect(pDocView->GetViewScale(), Rect.LowCorner(), 
									BT_SELECTEDLARGEST, &BlobSize);
		Rect.IncludePoint( BlobSize.LowCorner() );
	}

	return Rect;
}

/***********************************************************************************************

> void NodeGrid::SetOrigin(MILLIPOINT X,MILLIPOINT Y)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    1/3/94
	Inputs:		The new (X,Y) origin of the grid
    Outputs:    
    Returns:	-
    Purpose:    Used for setting the origin of the grid.

**********************************************************************************************/

void NodeGrid::SetOrigin(MILLIPOINT X,MILLIPOINT Y)
{
	XOrigin = X;
	YOrigin = Y;

	// a bit of a bodge, but for v1.1 the user origin will be permenently linked to the grid origin
	// (during import the grid may not yet be in the tree and has no parent spread - eek!)
	Node* pNode = FindParent();
	while (pNode!=NULL && pNode->IsSpread() == FALSE)
		pNode = pNode->FindParent();
	if (pNode!=NULL)
		((Spread*)pNode)->SetUserOrigin(DocCoord(X,Y));
}


/***********************************************************************************************

> void NodeGrid::GetOrigin(MILLIPOINT* pX,MILLIPOINT* pY)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    1/3/94
	Inputs:		Pointers to MILLIPOINT vars.
    Outputs:    *pX = X Origin of grid
				*pY = Y Origin of grid
    Returns:	-
    Purpose:    Used for retrieving the origin of the grid.

**********************************************************************************************/

void NodeGrid::GetOrigin(MILLIPOINT* pX,MILLIPOINT* pY)
{
	*pX = XOrigin;
	*pY = YOrigin;
}

/***********************************************************************************************

> BOOL NodeGrid::IsGridSelected() const

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    14/5/94
	Inputs:		-
    Outputs:    -
    Returns:	TRUE if grid is selected
    Purpose:    Find out if the grid is selected

**********************************************************************************************/

BOOL NodeGrid::IsGridSelected() const
{ 
	// Grids have their own special selected flag.
	return GridFlags.Selected;
}

/***********************************************************************************************

> void NodeGrid::SetGridSelected(BOOL selected)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    14/5/94
	Inputs:		selected - if TRUE, the grid selected flag is set, else it is cleared
    Outputs:    -
    Returns:	-
    Purpose:    Change the grid's selected flag

**********************************************************************************************/

void NodeGrid::SetGridSelected(BOOL selected)
{
	BOOL OldState = GridFlags.Selected;
	GridFlags.Selected = selected;

	if (OldState != selected)
	{
		if (selected)
			NodeGrid::NumSelected++;
		else
			NodeGrid::NumSelected--;
	}
}

/***********************************************************************************************

> BOOL NodeGrid::IsDefault() const

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    14/5/94
	Inputs:		-
    Outputs:    -
    Returns:	TRUE if grid is a Default grid
    Purpose:    Find out if the grid is a Default grid, i.e. one that covers a spread
				completely.  These behave slightly differently in the selection model

**********************************************************************************************/
BOOL NodeGrid::IsDefault() const
{ 
	return GridFlags.DefaultGrid; 		
}

/***********************************************************************************************

> void NodeGrid::SetDefault(BOOL Default)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    14/5/94
	Inputs:		Default - if TRUE, the grid Default flag is set, else it is cleared
    Outputs:    -
    Returns:	-
    Purpose:    Change the grid's Default flag

**********************************************************************************************/

void NodeGrid::SetDefault(BOOL Default)
{ 
	GridFlags.DefaultGrid = Default;
}

/***********************************************************************************************

> BOOL NodeGrid::IsDisabled() const

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    14/5/94
	Inputs:		-
    Outputs:    -
    Returns:	TRUE if grid is disabled
    Purpose:    Find out if the grid is disabled.
				A disabled grid does not render and cannot be snapped to.  
				This mechanism is used to disable Default grids without deleting 
				them from the tree.

**********************************************************************************************/
BOOL NodeGrid::IsDisabled() const
{ 
	return GridFlags.Disabled;
}

/***********************************************************************************************

> void NodeGrid::SetDisabled(BOOL disabled)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    14/5/94
	Inputs:		disabled - if TRUE, the grid disabled flag is set, else it is cleared
    Outputs:    -
    Returns:	-
    Purpose:    Change the grid's disabled flag.
				Also, if the grid is disabled, its selected flag is cleared as a disabled
				grid cannot be selected.

**********************************************************************************************/

void NodeGrid::SetDisabled(BOOL disabled)
{ 
	GridFlags.Disabled   = disabled; 		
	if (disabled) SetGridSelected(FALSE);
}

/***********************************************************************************************

> static void NodeGrid::RecalcNumSelectedGrids(Spread* pSpread)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    17/5/94
	Inputs:		pSpread = ptr to spread that can contain selected grids.
    Outputs:    -
    Returns:	-
    Purpose:    Recalculates the number of selected grids there are in the given spread.

**********************************************************************************************/

void NodeGrid::RecalcNumSelectedGrids(Spread* pSpread)
{
	if (pSpread == NULL) return;

	NodeGrid::NumSelected = 0;
	NodeGrid::NumNonDefaultSelected = 0;

	// Start from the last node in the spread
	Node* pNode = pSpread->FindLastChild();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
		{
			NodeGrid* pGrid = (NodeGrid*)pNode;
			
			if (pGrid->IsGridSelected() && !pGrid->IsDisabled())
			{
				NodeGrid::NumSelected++;

				if (!pGrid->IsDefault())
					NodeGrid::NumNonDefaultSelected++;
			}
		}
		// Scan back along the sibling list
		pNode = pNode->FindPrevious();
	}
}

//--------------------------------------------------------------------------------------------
                                       
#ifdef _DEBUG
 
void NodeGrid::ShowDebugTreeDetails() const
{                                 
	NodeRenderablePaper::ShowDebugTreeDetails(); 
}  
#endif

void NodeGrid::DumpRect(DocRect& R,TCHAR* S)
{
#ifdef _DEBUG
	TRACEUSER( "MarkN", _T("%s.lo.x = %ld\n"),  S,R.lo.x );
	TRACEUSER( "MarkN", _T("%s.lo.y = %ld\n"),  S,R.lo.y);
	TRACEUSER( "MarkN", _T("%s.hi.x = %ld\n"),  S,R.hi.x);
	TRACEUSER( "MarkN", _T("%s.hi.y = %ld\n\n"),S,R.hi.y);
#endif
}


/********************************************************************************************
>	virtual double NodeGrid::CalcDivisions(BOOL Scale)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/95
	Inputs:		Scale - account for scaling (only FALSE when saving for doc compatibility)
	Returns:	Number of Units between each main grid point
	Purpose:	Used to get the number of Units between each main grid point.
	Note:		No longer uses 'Divisions' as this is a cached value which is not updated when
				unit scaling is changed. Dynamically caclulating this overcomes this problem
				Divisons has not been removed from the class as it also entails removing all
				code setting the value. I have proposed the whole system needs revamping!
********************************************************************************************/

double NodeGrid::CalcDivisions(BOOL Scale)
{
	double   Spacing = GetMainStep();
	UnitType Units   = GetUnits(Scale);
	BOOL ok = TRUE;
	String_256 Str;

	if (Scale==FALSE)
		ok = Convert::MillipointsToString(Spacing,Units,&Str,10);
	else
	{
		DimScale* pDimScale = DimScale::GetPtrDimScale(this);
		ERROR2IF(pDimScale==NULL,FALSE,"NodeGridRect::CalcDivisions() - pDimScale==NULL");
		ok = pDimScale->ConvertToUnits(Spacing,&Str,TRUE,10,Units);
	}

	double   Divs;
	UnitType DummyUnits;
	if (ok) ok=Convert::StringToComponents(Str, &Divs, &DummyUnits);
	if (!ok) Divs=1;

	return Divs;
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

// NodeGridRect methods         

CC_IMPLEMENT_DYNAMIC(NodeGridRect, NodeGrid)          

/*********************************************************************************************

>    NodeGridRect::NodeGridRect() 

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   8/2/94
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeGridRect linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

NodeGridRect::NodeGridRect(): NodeGrid()
{   
  	Units     	= INCHES;
	Divisions   = 1.0;
  	Subdivisions= 2;

	MainXStep = 72000.0;
  	SubXStep  = MainXStep/Subdivisions;
	MainYStep = MainXStep;
  	SubYStep  = SubXStep;
}                    
    
  
/********************************************************************************************

>	String NodeGridRect::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
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
              
String NodeGridRect::Describe(BOOL Plural, BOOL Verbose) 
{   

	if (Plural)
		return (String(_R(IDS_GRIDRECT_DESCRP)));
	else
		return (String(_R(IDS_GRIDRECT_DESCRS)));
}; 
 

/***********************************************************************************************

>	void NodeGridRect::Render(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		pRender = a render region
	Outputs:	-
	Returns:	- 
	Purpose:    Renders the rectangular grid item
				
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

void NodeGridRect::Render( RenderRegion* pRender )
{      
#if !defined(EXCLUDE_FROM_RALPH)
//	static INT32 bbb=0;
//	if (IsUserName("MarkN")) TRACE( _T("\n-------------------------------------------------------------------\n"));
//	if (IsUserName("MarkN")) TRACE( _T("NodeGridRect::Render - %ld\n\n"),bbb++);

	// See if the render region likes grids to be drawn....
	if (!pRender->WantsGrids())
		return;

	// If the grid is disabled, don't bother either
	if (IsDisabled())
		return;

	View* pView = pRender->GetRenderView();
	if ((pView == NULL) || (!IS_A(pView, DocView)))
		// No docview to render to.
		return;

	DocView *pDocView = (DocView *) pView;

	if (!pDocView->GetShowGridState() && Tool::GetCurrentID() != TOOLID_GRID) return;

	// Set up attributes for drawing grid points
	pRender->SaveContext();
// Don't set line attributes because grid points are now plotted used filled rects
//	pRender->SetLineWidth(0); 								// Means single-pixel lines
//	pRender->SetLineColour(GridColour);						// Grid blobs are in this colour

	// Set fill attributes for grid point rectangle plotting...
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_TRANS); 
	pRender->SetFillColour(GridColour);
		
	DocRect ClipRect = pRender->GetClipRect();				// Get the current clip rect
	DocRect RendRect;

	RendRect.lo.x = max(GridBoundingRect.lo.x,ClipRect.lo.x);	// hi and lo renderable grid points
	RendRect.lo.y = max(GridBoundingRect.lo.y,ClipRect.lo.y);	// derived from the clip rect and the 
	RendRect.hi.x = min(GridBoundingRect.hi.x,ClipRect.hi.x);	// node's bounding rect
	RendRect.hi.y = min(GridBoundingRect.hi.y,ClipRect.hi.y);
	
	if (RendRect.IsValid())
	{
		MILLIPOINT PixelWidth = pRender->GetScaledPixelWidth();	// the width of a single pixel 

		RenderMainPoints(pRender,RendRect,PixelWidth);			// Render the main grid points (crosshairs)
		RenderSubPoints(pRender,RendRect,PixelWidth);			// Render the sub grid points (dots)
	}

	pRender->RestoreContext();
#endif
}  

/***********************************************************************************************

> void NodeGridRect::RenderMainPoints(RenderRegion* pRender,DocRect RendRect,MILLIPOINT PixelWidth)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/2/94
	
	Inputs:		pRender 	= region to render to
				RendRect	= Rectangle defining the renderable portion of the grid
				PixelWidth	= width of a pixel in millipoints
    Outputs:    
    Returns:
    Purpose:    Renders the main grid points within the given renderable rectangle
    Errors:     
	Scope:		private

***********************************************************************************************/

void NodeGridRect::RenderMainPoints(RenderRegion* pRender,DocRect RendRect,MILLIPOINT PixelWidth)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Set the fp render step values to be the underlying main grid step value
	double fp_RenderXStep = MainXStep;
	double fp_RenderYStep = MainYStep;

	// Calculate the step for rendering the grid in relation to the width of a pixel and the
	// factors of the dominant unit
	CalcGridRenderSteps(PixelWidth,&fp_RenderXStep,fp_RenderXStep*::GetUnitMultiple(Units),Units);
	CalcGridRenderSteps(PixelWidth,&fp_RenderYStep,fp_RenderYStep*::GetUnitMultiple(Units),Units);

	// Snap the render rect to these adjusted steps so we render all points within the renderable
	// portion of the grid
	Snap(&RendRect.lo,fp_RenderXStep,fp_RenderYStep);
	Snap(&RendRect.hi,fp_RenderXStep,fp_RenderYStep);

	ENSURE(RendRect.IsValid(),"RendRect is invalid in NodeGridRect::RenderMainPoints");
		
	// Get fixed point versions of the fp values for speedy grid rendering
	MILLIPOINT RenderXStep = (MILLIPOINT)((fp_RenderXStep * GRID_REND_FACTOR)+0.5);
	MILLIPOINT RenderYStep = (MILLIPOINT)((fp_RenderYStep * GRID_REND_FACTOR)+0.5);

//	DumpRect(RendRect,"RendRect");

	// Convert RendRect to fixed point values
	RendRect.lo.x *= GRID_REND_FACTOR;
	RendRect.lo.y *= GRID_REND_FACTOR;
	RendRect.hi.x *= GRID_REND_FACTOR;
	RendRect.hi.y *= GRID_REND_FACTOR;

	DocRect ScaledBB = GridBoundingRect;
	ScaledBB.lo.x *= GRID_REND_FACTOR;
	ScaledBB.lo.y *= GRID_REND_FACTOR;
	ScaledBB.hi.x *= GRID_REND_FACTOR;
	ScaledBB.hi.y *= GRID_REND_FACTOR;

	// Make sure the render rectangle region doesn't lie outside the bounding rect
	while (RendRect.lo.x <  ScaledBB.lo.x) RendRect.lo.x += RenderXStep;
	while (RendRect.lo.y <  ScaledBB.lo.y) RendRect.lo.y += RenderYStep;
	while (RendRect.hi.x >= ScaledBB.hi.x) RendRect.hi.x -= RenderXStep;
	while (RendRect.hi.y >= ScaledBB.hi.y) RendRect.hi.y -= RenderYStep;

	RendRect.hi.x += GRID_REND_FACTOR; // compensate for errors (i.e. add the bodge factor)
	RendRect.hi.y += GRID_REND_FACTOR; // compensate for errors

	// Render those grid points boy!

	DocCoord GridPoint,TrueGridPoint;

	const MILLIPOINT Pix = pRender->GetScaledPixelWidth();
//	const MILLIPOINT PixBy2 = Pix/2;
	const MILLIPOINT Length = PIX_CROSSHAIR_SIZE * Pix;

   	for (GridPoint.x=RendRect.lo.x;GridPoint.x<=RendRect.hi.x;GridPoint.x+=RenderXStep)
	{
		for (GridPoint.y=RendRect.lo.y;GridPoint.y<=RendRect.hi.y;GridPoint.y+=RenderYStep)
	  	{
			// Scale the fixed point grid coord down to get the true coord of the grid point
			TrueGridPoint.x = (GridPoint.x+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
			TrueGridPoint.y = (GridPoint.y+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;

//    		pRender->DrawCross(TrueGridPoint, PIX_CROSSHAIR_SIZE);
			// Phil says: Don't use DrawPixel because it can't be trusted to plot the
			// same pixel under GDI as it does under GDraw!
			// Instead draw two rectangles centered around the required coord.
			DocRect Horz(TrueGridPoint.x-Length,TrueGridPoint.y,TrueGridPoint.x+Length+Pix,TrueGridPoint.y+Pix);
			pRender->DrawRect(&Horz);

			DocRect Vert(TrueGridPoint.x,TrueGridPoint.y-Length,TrueGridPoint.x+Pix,TrueGridPoint.y+Length+Pix);
			pRender->DrawRect(&Vert);

	    }
	}
#endif
}


/***********************************************************************************************

> void NodeGridRect::RenderSubPoints(RenderRegion* pRender,DocRect RendRect,MILLIPOINT PixelWidth)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/2/94
	
	Inputs:		pRender 	= region to render to
				RendRect	= Rectangle defining the renderable portion of the grid
				PixelWidth	= width of a pixel in millipoints
    Outputs:    
    Returns:
    Purpose:    Renders the sub grid points within the given renderable rectangle
    Errors:     
	Scope:		private

***********************************************************************************************/

void NodeGridRect::RenderSubPoints(RenderRegion* pRender,DocRect RendRect,MILLIPOINT PixelWidth)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Set the fp render step values to be the underlying sub grid step value
	double fp_RenderXStep = SubXStep;
	double fp_RenderYStep = SubYStep;

	// Calculate the step for rendering the grid in relation to the width of a pixel and the
	// factors of the dominant unit
	CalcGridRenderSteps(PixelWidth,&fp_RenderXStep,MainXStep,Units);
	CalcGridRenderSteps(PixelWidth,&fp_RenderYStep,MainYStep,Units);

	// If the render step is greater than the main step then don't render the sub grid
	if (fp_RenderXStep >= MainXStep) return;
	if (fp_RenderYStep >= MainYStep) return;

	// Snap the render rect to the render steps so we render all points within the renderable
	// portion of the grid

	Snap(&RendRect.lo,fp_RenderXStep,fp_RenderYStep);
	Snap(&RendRect.hi,fp_RenderXStep,fp_RenderYStep);

	ENSURE(RendRect.IsValid(),"RendRect is invalid in NodeGridRect::RenderSubPoints");

	// Get fixed point versions of the fp values for speedy grid rendering
	MILLIPOINT RenderXStep = (MILLIPOINT)((fp_RenderXStep * GRID_REND_FACTOR)+0.5);
	MILLIPOINT RenderYStep = (MILLIPOINT)((fp_RenderYStep * GRID_REND_FACTOR)+0.5);

	// Convert RendRect to fixed point values
	RendRect.lo.x *= GRID_REND_FACTOR;
	RendRect.lo.y *= GRID_REND_FACTOR;
	RendRect.hi.x *= GRID_REND_FACTOR;
	RendRect.hi.y *= GRID_REND_FACTOR;

	DocRect ScaledBB = GridBoundingRect;
	ScaledBB.lo.x *= GRID_REND_FACTOR;
	ScaledBB.lo.y *= GRID_REND_FACTOR;
	ScaledBB.hi.x *= GRID_REND_FACTOR;
	ScaledBB.hi.y *= GRID_REND_FACTOR;

	// Make sure the render rectangle region doesn't lie outside the bounding rect
	while (RendRect.lo.x <  ScaledBB.lo.x) RendRect.lo.x += RenderXStep;
	while (RendRect.lo.y <  ScaledBB.lo.y) RendRect.lo.y += RenderYStep;
	while (RendRect.hi.x >= ScaledBB.hi.x) RendRect.hi.x -= RenderXStep;
	while (RendRect.hi.y >= ScaledBB.hi.y) RendRect.hi.y -= RenderYStep;

	RendRect.hi.x += GRID_REND_FACTOR; // compensate for errors (i.e. add the bodge factor)
	RendRect.hi.y += GRID_REND_FACTOR; // compensate for errors

	// Go ahead punk, render those points												 	

	DocCoord GridPoint,TrueGridPoint;

	const MILLIPOINT Pix = pRender->GetScaledPixelWidth();
//	const MILLIPOINT PixBy2 = Pix/2;

	for (GridPoint.x=RendRect.lo.x;GridPoint.x<=RendRect.hi.x;GridPoint.x+=RenderXStep)
	{
		for (GridPoint.y=RendRect.lo.y;GridPoint.y<=RendRect.hi.y;GridPoint.y+=RenderYStep)
	  	{
			// Scale the fixed point grid coord down to get the true coord of the grid point
			TrueGridPoint.x = (GridPoint.x+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
			TrueGridPoint.y = (GridPoint.y+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;

//			pRender->DrawPixel(TrueGridPoint);
			// Phil says: Don't use DrawPixel because it can't be trusted to plot the
			// same pixel under GDI as it does under GDraw!
			// Instead draw a rectangle 1*1 pixels around the required coord.
			DocRect Pixel(	TrueGridPoint.x,
							TrueGridPoint.y,
							TrueGridPoint.x+Pix,
							TrueGridPoint.y+Pix);
			pRender->DrawRect(&Pixel);
    	}
	}
#endif
}

/***********************************************************************************************

> BOOL NodeGridRect::Snap(DocCoord* pDocCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/2/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped to the grid.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest grid point.  If it is not appropriate to snap
				the coord to the grid (at the moment this means the coord lies outside the grid object's
				bounding box), then FALSE is returned.
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeGridRect::Snap(DocCoord* pDocCoord)
{
	if (IsDisabled()) return FALSE;

	ENSURE(SubXStep !=0,"SubXStep ==0 in NodeGridRect::Snap");
	ENSURE(SubYStep !=0,"SubYStep ==0 in NodeGridRect::Snap");

	if (!GridBoundingRect.ContainsCoord(DocCoord(pDocCoord->x,pDocCoord->y)))
		return (FALSE);

	MILLIPOINT x = SnapOrdinate(pDocCoord->x,SubXStep,XOrigin);
	MILLIPOINT y = SnapOrdinate(pDocCoord->y,SubYStep,YOrigin);

	if (!GridBoundingRect.ContainsCoord(DocCoord(x,y)))
		return (FALSE);

	pDocCoord->x = x;
	pDocCoord->y = y;

	return (TRUE);
}

/***********************************************************************************************

> BOOL NodeGridRect::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/2/94
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped to the grid.
    			FALSE	- the DocRect has not been processed.
    			                                  		 
    Purpose:    Snaps the given rect to the nearest position on the grid, preserving its width
    			and height.
    			The coords of the rect used for the snapping are determined by the PrevCoord and
				CurCoord coords supplied.  This is done to allow the user to control how a
				selection rectangle is snapped to the grid by the direction of his/her last mouse 
				movement.
				To force the bottom left hand corner of the rect to be snapped, 
				supply PrevCoord=(0,0) and CurCoord(-1,-1).
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeGridRect::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	if (IsDisabled()) return FALSE;

	ENSURE(SubXStep !=0,"SubXStep ==0 in NodeGridRect::Snap");
	ENSURE(SubYStep !=0,"SubYStep ==0 in NodeGridRect::Snap");
	
	INT32 xdir = CurCoord.x - PrevCoord.x;
	INT32 ydir = CurCoord.y - PrevCoord.y;
	INT32 x,y,prevx, prevy;
	
	if (LastXDir == 0)	LastXDir = xdir;
	if (xdir == 0)		xdir = LastXDir;
	LastXDir = xdir;

	if (xdir <= 0) x = pDocRect->lo.x; else x = pDocRect->hi.x;

	if (LastYDir == 0)	LastYDir = ydir;
	if (ydir == 0)		ydir = LastYDir;
	LastYDir = ydir;

	if (ydir <= 0) y = pDocRect->lo.y; else y = pDocRect->hi.y;

	if (!GridBoundingRect.ContainsCoord(DocCoord(x,y)))
		return (FALSE);

	prevx 	= x;
	prevy 	= y;
	x 		= SnapOrdinate(x,SubXStep,XOrigin);
	y 		= SnapOrdinate(y,SubYStep,YOrigin);

	if (!GridBoundingRect.ContainsCoord(DocCoord(x,y)))
		return (FALSE);

	pDocRect->Translate(x-prevx,y-prevy);

	return (TRUE);
}
/***********************************************************************************************

> BOOL NodeGridRect::Snap(DocCoord* pDocCoord,double XStep,double YStep)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/2/94
	Inputs:     pDocCoord	- the coord to snap
				XStep		- the x step to use when snapping the point
				YStep		- the y step to use when snapping the point
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped to the grid.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Same as Snap(pDocCoord), except the coord is snapped to grid points at the given intervals
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeGridRect::Snap(DocCoord* pDocCoord,double XStep,double YStep)
{
	if (IsDisabled()) return FALSE;

	ENSURE(XStep!=0,"XStep==0 in NodeGridRect::Snap(pDocCoord,XStep,YStep");
	ENSURE(YStep!=0,"YStep==0 in NodeGridRect::Snap(pDocCoord,XStep,YStep"); 

	pDocCoord->x = SnapOrdinate(pDocCoord->x,XStep,XOrigin);
	pDocCoord->y = SnapOrdinate(pDocCoord->y,YStep,YOrigin);

	return (TRUE);
}


/***********************************************************************************************

> Node* NodeGridRect::SimpleCopy() // Private method  

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/2/94
	
	Inputs:         - 
    Outputs:    
    Returns:    A copy of the node, or NULL if memory runs out
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
    Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
 
        
	Scope:      protected       
**********************************************************************************************/

Node* NodeGridRect::SimpleCopy()
{
	NodeGridRect* NodeCopy = new NodeGridRect();   
	if (NodeCopy != NULL)
		CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}          

/***********************************************************************************************
>   void NodeGridRect::CopyNodeContents(NodeGridRect* NodeCopy)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    8/2/94
	
	Inputs:		- 
	Outputs:    A copy of this node
 
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/


void NodeGridRect::CopyNodeContents(NodeGridRect* NodeCopy)
{
	ENSURE(NodeCopy != NULL,"Trying to copy a NodeGridRect's contents to a NULL node");  
	NodeGrid::CopyNodeContents(NodeCopy); 

	NodeCopy->MainXStep 	= this->MainXStep;
	NodeCopy->MainYStep 	= this->MainYStep;
	NodeCopy->SubXStep  	= this->SubXStep;	
	NodeCopy->SubYStep  	= this->SubYStep;	

	NodeCopy->Units   		= this->Units;
	NodeCopy->Divisions		= this->Divisions;
	NodeCopy->Subdivisions	= this->Subdivisions;
}


/***********************************************************************************************
>   void NodeGridRect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeGridRect::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeGridRect), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeGridRect))
		CopyNodeContents((NodeGridRect*)pNodeCopy);
}



/********************************************************************************************

>	void* NodeGridRect::GetDebugDetails(StringBase* Str) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/94
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void NodeGridRect::GetDebugDetails(StringBase* Str) 
{          
	NodeGrid::GetDebugDetails(Str); 
}
 
	      
//--------------------------------------------------------------------------------------------
                                       
#ifdef _DEBUG
 
void NodeGridRect::ShowDebugTreeDetails() const
{                                 
	TRACE( _T("NodeGridRect::ShowDebugTreeDetails() ")); 
	Node::ShowDebugTreeDetails(); 
}  

#endif
 
//--------------------------------------------------------------------------------------------

/********************************************************************************************
>	BOOL NodeGridRect::SetGridParams(double Div,UINT32 Subdiv,UnitType NewUnits, BOOL Scale=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Div 	 - Number of Units between each main grid point
				Subdiv 	 - Number of subdivisions between each main grid point
				NewUnits - The units the grid is defined in
				Scale    - account for unit scaling (only FALSE when loading for compatibility with old docs)
	Returns:	FALSE if fails
	Purpose:	Defines the grid.
********************************************************************************************/

BOOL NodeGridRect::SetGridParams(double Div, UINT32 Subdiv, UnitType NewUnits, BOOL Scale)
{
	if (Div < 0)
	{
		ERROR3("NodeGridRect::SetGridParams() - Div < 0!");
		Div = 0.0;
	}
	if (Subdiv < 1)
	{
		ERROR3("NodeGridRect::SetGridParams() - SubDiv < 1!");
		Subdiv = 1;
	}

	DimScale* pDimScale = DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,FALSE,"NodeGridRect::SetGridParams() - pDimScale==NULL");

	// set the raw values in the grid - but only set units if scaling turned off
	// (except if scaling is not to be accounted for  - ie during loading for compatibility with old docs - yuk!)
	Divisions	 = Div;
	Subdivisions = Subdiv;
	if (Scale==FALSE || pDimScale->GetScaleUnits()==NOTYPE)
		Units = NewUnits;

	// convert the divisions and units into a millipoint grid spacing (accounting for unit scaling) - yuk!
	double GridSpacing=0;
	if (Scale)
	{
		BOOL ok = pDimScale->ComponentsToMillipoint(&GridSpacing, Divisions, GetUnits());
		if (!ok) return FALSE;
	}
	else
	{
		DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		ERROR2IF(pDocUnitList==NULL,FALSE,"DimScale::ComponentsToMillipoint() - pDocUnitList==NULL");
		Unit* pUnit = pDocUnitList->FindUnit(Units);
		ERROR2IF(pUnit==NULL,FALSE,"DimScale::ComponentsToMillipoint() - pUnit==NULL");
		GridSpacing = Div * pUnit->GetMillipoints();
	}

	if (GridSpacing<1)
	{
		ERROR3("NodeGridRect::SetGridParams() - GridSpacing < 1!");
		GridSpacing=1;
	}

	MainXStep = GridSpacing;
	SubXStep  = MainXStep/Subdivisions;
	if (SubXStep<1)
	{
		ERROR3("NodeGridRect::SetGridParams() - SubXStep < 1!");
		SubXStep=1;
	}

	MainYStep = MainXStep;
	SubYStep  = SubXStep;
    
	return TRUE;
}


/********************************************************************************************
>	double NodeGridRect::GetDivisions(BOOL Scale=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/95
	Inputs:		Scale - account for scaling (only FALSE when saving for doc compatibility)
	Returns:	Number of Units between each main grid point
	Purpose:	Used to get the number of Units between each main grid point.
********************************************************************************************/

double NodeGridRect::GetDivisions(BOOL Scale)
{
	return NodeGrid::CalcDivisions(Scale);
}


/********************************************************************************************

>	UINT32 NodeGridRect::GetSubdivisions()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Num subdivisions
	Purpose:	Used to get the number of subdivisions between each main grid point.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeGridRect::GetSubdivisions()
{
	return Subdivisions;
}


/********************************************************************************************
>	UnitType NodeGridRect::GetUnits(BOOL Scale=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/95
	Inputs:		Scale - account for unit scaling (only FALSE when saving for compatibility with old docs)
	Returns:	Units used by the grid
	Purpose:	Gets the units used by the grid
********************************************************************************************/

UnitType NodeGridRect::GetUnits(BOOL Scale)
{
	DimScale* pDimScale = DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,NOTYPE,"NodeGridRect::GetUnits() - pDimScale==NULL");

	// if scaling turned on return scale units
	// (except if scaling is not to be accounted for  - ie during saving for compatibility with old docs)
	UnitType ScaleUnits = pDimScale->GetScaleUnits();
	if (Scale && ScaleUnits!=NOTYPE)
		return ScaleUnits;

	return Units;
}


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

// NodeGridIso methods         

CC_IMPLEMENT_DYNAMIC(NodeGridIso, NodeGrid)          

/*********************************************************************************************

>    NodeGridRect::NodeGridIso() 

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   15/2/94
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeGridIso linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

NodeGridIso::NodeGridIso(): NodeGrid()
{   
  	Units 		 = INCHES;
	Divisions	 = 1.0;
  	Subdivisions = 4;

  	MainStep = 72000.0;
  	SubStep = MainStep/Subdivisions;
}                    
    
  
/********************************************************************************************

>	String NodeGridIso::Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
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
              
String NodeGridIso::Describe(BOOL Plural, BOOL Verbose) 
{   
    if (Plural)
		return (String(_R(IDS_GRIDISO_DESCRP)));
	else
		return (String(_R(IDS_GRIDISO_DESCRS)));
}; 
 

/***********************************************************************************************

>	void NodeGridIso::Render(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Inputs:		pRender = a render region
	Outputs:	-
	Returns:	- 
	Purpose:    Renders the isometric grid item
				
	Errors:		-
	SeeAlso:	-

***********************************************************************************************/

void NodeGridIso::Render( RenderRegion* pRender )
{      
#if !defined(EXCLUDE_FROM_RALPH)
	if (IsDisabled()) return;

	View* pView = pRender->GetRenderView();
	if ((pView == NULL) || (!IS_A(pView, DocView)))
		// No docview to render to.
		return;

	DocView *pDocView = (DocView *) pView;

 	if (!pDocView->GetShowGridState() && Tool::GetCurrentID() != TOOLID_GRID) return;

	// Set up attributes for drawing grid points
	pRender->SaveContext();
// Don't setup for line rendering because grid points now rendered as filled rects...
//	pRender->SetLineWidth(0); 								// Means single-pixel lines
//	pRender->SetLineColour(GridColour);						// Grid blobs are in this colour

	// Setup for rendering grid point rects...
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_TRANS); 
	pRender->SetFillColour(GridColour);
		
	DocRect ClipRect = pRender->GetClipRect();				// Get the current clip rect
	DocRect RendRect;

	RendRect.lo.x = max(GridBoundingRect.lo.x,ClipRect.lo.x);	// hi and lo renderable grid points
	RendRect.lo.y = max(GridBoundingRect.lo.y,ClipRect.lo.y);	// derived from the clip rect and the 
	RendRect.hi.x = min(GridBoundingRect.hi.x,ClipRect.hi.x);	// node's bounding rect
	RendRect.hi.y = min(GridBoundingRect.hi.y,ClipRect.hi.y);

	if (RendRect.IsValid())
	{
		MILLIPOINT PixelWidth = pRender->GetScaledPixelWidth();	// the width of a single pixel 

		RenderPoints(pRender,RendRect,PixelWidth);			// Render the grid points (crosshairs and sub grid dots)
	}

	pRender->RestoreContext();
#endif
}  

/***********************************************************************************************

> void NodeGridIso::RenderPoints(RenderRegion* pRender,DocRect RendRect,MILLIPOINT PixelWidth)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/2/94
	
	Inputs:		pRender 	= region to render to
				RendRect	= Rectangle defining the renderable portion of the grid
				PixelWidth	= width of a pixel in millipoints
    Outputs:    
    Returns:
    Purpose:    Renders the grid points within the given renderable rectangle
    Errors:     
	Scope:		private

***********************************************************************************************/

void NodeGridIso::RenderPoints(RenderRegion* pRender,DocRect RendRect,MILLIPOINT PixelWidth)
{
#if !defined(EXCLUDE_FROM_RALPH)
	TRACEUSER( "MarkN", _T("\n------------------\nNodeGridIso::RenderPoints\n\n"));

	// Set the fp render step value to be the underlying main grid step value
	double fp_MainRenderStep = MainStep;

	// Calculate the step for rendering the grid in relation to the width of a pixel and the
	// multiples of the dominant unit
	CalcGridRenderSteps(PixelWidth,&fp_MainRenderStep,fp_MainRenderStep*::GetUnitMultiple(Units),Units);

	// Snap the render rect to these adjusted steps so we render all points within the renderable
	// portion of the grid

	Snap(&RendRect.lo,fp_MainRenderStep);
	Snap(&RendRect.hi,fp_MainRenderStep);
	BOOL YOffAdjust = FALSE; // YOffAdjust = FALSE because we have snapped exactly to the main grid
	
	if (RendRect.lo.x > RendRect.hi.x) Swap(RendRect.lo.x,RendRect.hi.x);
	if (RendRect.lo.y > RendRect.hi.y) Swap(RendRect.lo.y,RendRect.hi.y);

	ENSURE(RendRect.IsValid(),"RendRect is invalid in NodeGridIso::RenderPoints");

	// Set the fp sub render step value to be the underlying sub grid step value
	double fp_SubRenderStep = SubStep;
	CalcGridRenderSteps(PixelWidth,&fp_SubRenderStep,fp_MainRenderStep,Units);
	if (fp_SubRenderStep >= fp_MainRenderStep)	fp_SubRenderStep = 0;

	// Get fixed point versions of the fp values for speedy grid rendering
	MILLIPOINT MainRenderStep = (MILLIPOINT)((fp_MainRenderStep * GRID_REND_FACTOR)+0.5);
	MILLIPOINT SubRenderStep  = (MILLIPOINT)((fp_SubRenderStep  * GRID_REND_FACTOR)+0.5);

	// Convert RendRect to fixed point values
	RendRect.lo.x *= GRID_REND_FACTOR;
	RendRect.lo.y *= GRID_REND_FACTOR;
	RendRect.hi.x *= GRID_REND_FACTOR;
	RendRect.hi.y *= GRID_REND_FACTOR;

	DocRect ScaledBB = GridBoundingRect;
	ScaledBB.lo.x *= GRID_REND_FACTOR;
	ScaledBB.lo.y *= GRID_REND_FACTOR;
	ScaledBB.hi.x *= GRID_REND_FACTOR;
	ScaledBB.hi.y *= GRID_REND_FACTOR;

	RendRect.Inflate(MainRenderStep); YOffAdjust = !YOffAdjust;

	// Make sure the render rectangle region doesn't lie outside the bounding rect
	while (RendRect.lo.x <  ScaledBB.lo.x) { RendRect.lo.x += MainRenderStep; YOffAdjust = !YOffAdjust; }
	while (RendRect.lo.y <  ScaledBB.lo.y)   RendRect.lo.y += MainRenderStep;
	while (RendRect.hi.x >  ScaledBB.hi.x)   RendRect.hi.x -= MainRenderStep;
	while (RendRect.hi.y >  ScaledBB.hi.y)   RendRect.hi.y -= MainRenderStep;

	RendRect.lo.x -= MainRenderStep;  YOffAdjust = !YOffAdjust; 	// Need to plot to left of BB for sub grid dots
	RendRect.lo.y -= MainRenderStep;  							// Need to plot underneath BB for sub grid dots

	RendRect.hi.x += GRID_REND_FACTOR; // compensate for errors (i.e. add the bodge factor)
	RendRect.hi.y += GRID_REND_FACTOR; // compensate for errors

	RendRect.hi.y += MainRenderStep;

	// Render those grid points boy!

	DocCoord 	GridPoint,TrueGridPoint;
	MILLIPOINT 	yoff;

	const MILLIPOINT Pix = pRender->GetScaledPixelWidth();
//	const MILLIPOINT PixBy2 = Pix/2;
	const MILLIPOINT Length = PIX_CROSSHAIR_SIZE * Pix;

   	for (GridPoint.x=RendRect.lo.x;GridPoint.x<=RendRect.hi.x;GridPoint.x+=MainRenderStep)
	{
		if (YOffAdjust) yoff = MainRenderStep/2; else yoff = 0;

		RendRect.lo.y += yoff;
		RendRect.hi.y += yoff;

		for (GridPoint.y=RendRect.lo.y;GridPoint.y<=RendRect.hi.y;GridPoint.y+=MainRenderStep)
	  	{
			if (ScaledBB.ContainsCoord(GridPoint))
			{
				// Scale the fixed point grid coord down to get the true coord of the grid point
				TrueGridPoint.x = (GridPoint.x+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
				TrueGridPoint.y = (GridPoint.y+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;

//				pRender->DrawCross(TrueGridPoint,PIX_CROSSHAIR_SIZE);
				// Phil says: Don't use DrawPixel because it can't be trusted to plot the
				// same pixel under GDI as it does under GDraw!
				// Instead draw two rectangles centered around the required coord.
				DocRect Horz(TrueGridPoint.x-Length, TrueGridPoint.y,TrueGridPoint.x+Length+Pix,TrueGridPoint.y+Pix);
				pRender->DrawRect(&Horz);

				DocRect Vert(TrueGridPoint.x, TrueGridPoint.y-Length,TrueGridPoint.x+Pix,TrueGridPoint.y+Length+Pix);
				pRender->DrawRect(&Vert);
			}

			if (SubRenderStep != 0) 
				RenderSubPoints(pRender,GridPoint,MainRenderStep,SubRenderStep,PixelWidth,ScaledBB);
	    }

		RendRect.lo.y -= yoff;
		RendRect.hi.y -= yoff;

		YOffAdjust = !YOffAdjust;
	}
#endif
}


/***********************************************************************************************

> void NodeGridIso::RenderSubPoints(RenderRegion* pRender,
									const DocCoord& MainGridPoint,
									MILLIPOINT MainRenderStep,
									MILLIPOINT SubRenderStep,
									MILLIPOINT PixelWidth,
									DocRect&   ScaledBB)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/2/94
	
	Inputs:		pRender 		= region to render to
				MainGridPoint	= coord of a main grid cross hair point   (Scaled up by GRID_REND_FACTOR)
				MainRenderStep	= step for rendering the main grid points (Scaled up by GRID_REND_FACTOR)
				SubRenderStep	= step for rendering the sub grid points  (Scaled up by GRID_REND_FACTOR)
				PixelWidth		= width of a single pixel
				ScaledBB		= The bounding rect of grid scaled up by GRID_REND_FACTOR
    Outputs:    
    Returns:
    Purpose:    Renders the sub grid points from the given main point
    Errors:     
	Scope:		private

***********************************************************************************************/

void NodeGridIso::RenderSubPoints(	RenderRegion* pRender,
									const DocCoord& MainGridPoint,
									MILLIPOINT MainRenderStep,
									MILLIPOINT SubRenderStep,
									MILLIPOINT /*PixelWidth*/,
									DocRect&   ScaledBB)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord SubGridPoint,TrueSubGridPoint;
	MILLIPOINT maxx,maxy;

	// Render the vertical sub grid dots from this point to the one above
	SubGridPoint.x = MainGridPoint.x;
	SubGridPoint.y = MainGridPoint.y+SubRenderStep;
	maxy = MainGridPoint.y+MainRenderStep;

	const MILLIPOINT Pix = pRender->GetScaledPixelWidth();
//	const MILLIPOINT PixBy2 = Pix/2;

	while (SubGridPoint.y < maxy)
	{
		if (ScaledBB.ContainsCoord(SubGridPoint))
		{
			// Scale the fixed point grid coord down to get the true coord of the grid point
			TrueSubGridPoint.x = (SubGridPoint.x+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
			TrueSubGridPoint.y = (SubGridPoint.y+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
//			pRender->DrawPixel(TrueSubGridPoint);
			// Phil says: Don't use DrawPixel because it can't be trusted to plot the
			// same pixel under GDI as it does under GDraw!
			// Instead draw a rectangle 1*1 pixels around the required coord.
			DocRect Pixel(	TrueSubGridPoint.x,
							TrueSubGridPoint.y,
							TrueSubGridPoint.x+Pix,
							TrueSubGridPoint.y+Pix);
			pRender->DrawRect(&Pixel);
		}
		SubGridPoint.y += SubRenderStep;
	}

	// Render the diagonal dots from this point to the one to the right and up
	SubGridPoint.x = MainGridPoint.x+SubRenderStep;
	SubGridPoint.y = MainGridPoint.y+(SubRenderStep/2);
	maxx = MainGridPoint.x+MainRenderStep;

	while (SubGridPoint.x < maxx)
	{
		if (ScaledBB.ContainsCoord(SubGridPoint))
		{
			// Scale the fixed point grid coord down to get the true coord of the grid point
			TrueSubGridPoint.x = (SubGridPoint.x+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
			TrueSubGridPoint.y = (SubGridPoint.y+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;

//			pRender->DrawPixel(TrueSubGridPoint);
			// Phil says: Don't use DrawPixel because it can't be trusted to plot the
			// same pixel under GDI as it does under GDraw!
			// Instead draw a rectangle 1*1 pixels around the required coord.
			DocRect Pixel(	TrueSubGridPoint.x,
							TrueSubGridPoint.y,
							TrueSubGridPoint.x+Pix,
							TrueSubGridPoint.y+Pix);
			pRender->DrawRect(&Pixel);
		}
		SubGridPoint.x += SubRenderStep;
		SubGridPoint.y += SubRenderStep/2;
	}

	// Render the diagonal dots from this point to the one to the right and down
	SubGridPoint.x = MainGridPoint.x+SubRenderStep;
	SubGridPoint.y = MainGridPoint.y-(SubRenderStep/2);
	maxx = MainGridPoint.x+MainRenderStep;

	while (SubGridPoint.x < maxx)
	{
		if (ScaledBB.ContainsCoord(SubGridPoint))
		{
			// Scale the fixed point grid coord down to get the true coord of the grid point
			TrueSubGridPoint.x = (SubGridPoint.x+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;
			TrueSubGridPoint.y = (SubGridPoint.y+(GRID_REND_FACTOR/2)) / GRID_REND_FACTOR;

//			pRender->DrawPixel(TrueSubGridPoint);
			// Phil says: Don't use DrawPixel because it can't be trusted to plot the
			// same pixel under GDI as it does under GDraw!
			// Instead draw a rectangle 1*1 pixels around the required coord.
			DocRect Pixel(	TrueSubGridPoint.x,
							TrueSubGridPoint.y,
							TrueSubGridPoint.x+Pix,
							TrueSubGridPoint.y+Pix);
			pRender->DrawRect(&Pixel);
		}
		SubGridPoint.x += SubRenderStep;
		SubGridPoint.y -= SubRenderStep/2;
	}
#endif
}


/***********************************************************************************************

> BOOL NodeGridIso::Snap(DocCoord* pDocCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/2/94
	Inputs:     pDocCoord	= a coord in Spread coords
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped to the grid.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Snaps to given coord to the nearest grid point.  If it is not appropriate to snap
				the coord to the grid (at the moment this means the coord lies outside the grid object's
				bounding box), then FALSE is returned.
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeGridIso::Snap(DocCoord* pDocCoord)
{
	if (IsDisabled()) return FALSE;

	ENSURE(MainStep!=0,"MainStep==0 in NodeGridIso::Snap");
	ENSURE(SubStep !=0,"SubStep ==0 in NodeGridIso::Snap");

	if (!GridBoundingRect.ContainsCoord(DocCoord(pDocCoord->x,pDocCoord->y)))
		return (FALSE);

	MILLIPOINT x = SnapOrdinate(pDocCoord->x,SubStep,XOrigin);
	MILLIPOINT y = SnapOrdinate(pDocCoord->y,SubStep,CalcYOrigin(pDocCoord->x,SubStep));

	if (!GridBoundingRect.ContainsCoord(DocCoord(x,y)))
		return (FALSE);

	pDocCoord->x = x;
	pDocCoord->y = y;

	return (TRUE);
}

/***********************************************************************************************

> BOOL NodeGridIso::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/2/94
	Inputs:     pDocCoord	- the rectangle to snap
				StartDrag	- Start coord of drag
				EndDrag		- End coord of drag
    Outputs:    
    Returns:    TRUE 	- the DocRect been snapped to the grid.
    			FALSE	- the DocRect has not been processed.

    Purpose:    Snaps the given rect to the nearest position on the grid, preserving its width
    			and height.
    			The coords of the rect used for the snapping are determined by the PrevCoord and
				CurCoord coords supplied.  This is done to allow the user to control how a
				selection rectangle is snapped to the grid by the direction of his/her last mouse 
				movement.
				To force the bottom left hand corner of the rect to be snapped, 
				supply PrevCoord=(0,0) and CurCoord(-1,-1).
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeGridIso::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
	if (IsDisabled()) return FALSE;

	ENSURE(MainStep!=0,"MainStep==0 in NodeGridIso::Snap");
	ENSURE(SubStep !=0,"SubStep ==0 in NodeGridIso::Snap");
	
	INT32 xdir = CurCoord.x - PrevCoord.x;
	INT32 ydir = CurCoord.y - PrevCoord.y;
	INT32 x,y,prevx, prevy;
	
	if (LastXDir == 0)	LastXDir = xdir;
	if (xdir == 0)		xdir = LastXDir;
	LastXDir = xdir;

	if (xdir <= 0) x = pDocRect->lo.x; else x = pDocRect->hi.x;

	if (LastYDir == 0)	LastYDir = ydir;
	if (ydir == 0)		ydir = LastYDir;
	LastYDir = ydir;

	if (ydir <= 0) y = pDocRect->lo.y; else y = pDocRect->hi.y;

	if (!GridBoundingRect.ContainsCoord(DocCoord(x,y)))
		return (FALSE);

	prevx 	= x;
	prevy 	= y;
	x 		= SnapOrdinate(x,SubStep,XOrigin);
	y 		= SnapOrdinate(y,SubStep,CalcYOrigin(x,SubStep));

	if (!GridBoundingRect.ContainsCoord(DocCoord(x,y)))
		return (FALSE);

	pDocRect->Translate(x-prevx,y-prevy);

	return (TRUE);
}

/***********************************************************************************************

> BOOL NodeGridIso::Snap(DocCoord* pDocCoord,MILLIPOINT Step)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    9/2/94
	Inputs:     pDocCoord	- the coord to snap
				XStep		- the step to use when snapping the point
    Outputs:    
    Returns:    TRUE 	- the DocCoord has been snapped to the grid.
    			FALSE	- the DocCoord has not been processed.
    			                                  		 
    Purpose:    Same as Snap(pDocCoord), except the coord is snapped to grid points at the given intervals
    Errors:        
	Scope:      public
	       
**********************************************************************************************/

BOOL NodeGridIso::Snap(DocCoord* pDocCoord,double Step)
{
	if (IsDisabled()) return FALSE;

	ENSURE(Step!=0,"Step==0 in NodeGridIso::Snap");

	pDocCoord->x = SnapOrdinate(pDocCoord->x,Step,XOrigin);
	pDocCoord->y = SnapOrdinate(pDocCoord->y,Step,CalcYOrigin(pDocCoord->x,Step));

	return (TRUE);
}


/***********************************************************************************************

> MILLIPOINT NodeGridIso::CalcYOrigin(MILLIPOINT x,double Step)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    16/2/94
	Inputs:		x    = Snapped x coord
				Step = step rate of underlying grid
    Outputs:    
    Returns:    The Y origin that should be used to snap Y coords to the grid
    Purpose:    The Y origin used for snapping Y coords alternates between YOrigin and YOrigin+(Step/2)
				depending on a point's snapped x coord. This function returns the correct one to use.
    Errors:
	Scope:		private

**********************************************************************************************/

MILLIPOINT NodeGridIso::CalcYOrigin(MILLIPOINT x,double Step)
{
	double xdist = mod((double)(x-XOrigin),Step*2); 
	if (xdist<0) xdist = 0-xdist;

	if ((xdist > (Step/2)) && (xdist < ((Step*2)-(Step/2))) )
		return YOrigin+((MILLIPOINT)((Step/2)+0.5));
	else
		return YOrigin;


/*
	MILLIPOINT xdist = 	abs((x-XOrigin) % (Step*2));

	if ((xdist > (Step/2)) && (xdist < ((Step*2)-(Step/2))) )
		return YOrigin+(Step/2);
	else
		return YOrigin;
*/
}

/***********************************************************************************************

> Node* NodeGridIso::SimpleCopy() // Private method  

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/2/94
	
	Inputs:         - 
    Outputs:    
    Returns:    A copy of the node, or NULL if memory runs out
		 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
	       
    Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
 
        
	Scope:      public
**********************************************************************************************/

Node* NodeGridIso::SimpleCopy()
{
	NodeGridIso* NodeCopy = new NodeGridIso();   
	if (NodeCopy != NULL)
		CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}          

/***********************************************************************************************
>   void NodeGridIso::CopyNodeContents(NodeGridIso* NodeCopy)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    15/2/94
	
	Inputs:		- 
	Outputs:    A copy of this node
 
    Returns:    -
		 
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
		      
	Errors:     An assertion failure will occur if NodeCopy is NULL
	
    Scope:      protected
								     
***********************************************************************************************/


void NodeGridIso::CopyNodeContents(NodeGridIso* NodeCopy)
{
	ENSURE(NodeCopy != NULL,"Trying to copy a NodeGridIso's contents to a NULL node");  
	NodeGrid::CopyNodeContents(NodeCopy); 

	NodeCopy->MainStep 		= this->MainStep;
	NodeCopy->SubStep  		= this->SubStep;	

	NodeCopy->Units   		= this->Units;
	NodeCopy->Divisions		= this->Divisions;
	NodeCopy->Subdivisions	= this->Subdivisions;
}


/***********************************************************************************************
>   void NodeGridIso::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeGridIso::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeGridIso), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeGridIso))
		CopyNodeContents((NodeGridIso*)pNodeCopy);
}



/********************************************************************************************

>	void* NodeGridIso::GetDebugDetails(StringBase* Str) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/2/94
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void NodeGridIso::GetDebugDetails(StringBase* Str) 
{          
	NodeGrid::GetDebugDetails(Str); 
}
 
	      
//--------------------------------------------------------------------------------------------
                                       
#ifdef _DEBUG
 
void NodeGridIso::ShowDebugTreeDetails() const
{                                 
	TRACE( _T("NodeGridIso::ShowDebugTreeDetails() ")); 
	Node::ShowDebugTreeDetails(); 
}  

#endif
 
//--------------------------------------------------------------------------------------------

/********************************************************************************************
>	BOOL NodeGridIso::SetGridParams(double Div,UINT32 Subdiv,UnitType NewUnits, BOOL Scale=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/95
	Inputs:		Div      - Number of Units between each main step of the grid
				Subdiv   - Number of subdivisions between each main grid point
				NewUnits - The units the grid is defined in
				Scale    - account for unit scaling (only FALSE when loading for compatibility with old docs)
	Returns:	FALSE if fails
	Purpose:	Defines the grid.
********************************************************************************************/

BOOL NodeGridIso::SetGridParams(double Div,UINT32 Subdiv,UnitType NewUnits, BOOL Scale)
{
	if (Div < 0)
	{
		ERROR3("NodeGridIso::SetGridParams() - Div < 0!");
		Div = 0.0;
	}
	if (Subdiv < 1)
	{
		ERROR3("NodeGridIso::SetGridParams() - SubDiv < 1!");
		Subdiv = 1;
	}

	DimScale* pDimScale = DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,FALSE,"NodeGridRect::SetGridParams() - pDimScale==NULL");

	// set the raw values in the grid - but only set units if scaling turned off
	// (except if scaling is not to be accounted for  - ie during loading for compatibility with old docs - yuk!)
	Divisions	 = Div;
	Subdivisions = Subdiv;
	if (Scale==FALSE || pDimScale->GetScaleUnits()==NOTYPE)
		Units = NewUnits;

	// convert the divisions and units into a millipoint grid spacing (accounting for unit scaling) - yuk!
	double GridSpacing=0;
	if (Scale)
	{
		BOOL ok = pDimScale->ComponentsToMillipoint(&GridSpacing, Divisions, GetUnits());
		if (!ok) return FALSE;
	}
	else
	{
		DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
		ERROR2IF(pDocUnitList==NULL,FALSE,"DimScale::ComponentsToMillipoint() - pDocUnitList==NULL");
		Unit* pUnit = pDocUnitList->FindUnit(Units);
		ERROR2IF(pUnit==NULL,FALSE,"DimScale::ComponentsToMillipoint() - pUnit==NULL");
		GridSpacing = Div * pUnit->GetMillipoints();
	}

	if (GridSpacing<1)
	{
		ERROR3("NodeGridIso::SetGridParams() - GridSpacing < 1!");
		GridSpacing=1;
	}

	MainStep = GridSpacing;
	SubStep  = MainStep/Subdivisions;
	if (SubStep<1)
	{
		ERROR3("NodeGridIso::SetGridParams() - SubXStep < 1!");
		SubStep=1;
	}

	return TRUE;
}


/********************************************************************************************
>	double NodeGridIso::GetDivisions(BOOL Scale=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/95
	Inputs:		Scale - account for scaling (only FALSE when saving for doc compatibility)
	Returns:	Number of Units between each main grid point
	Purpose:	Used to get the number of Units between each main grid point.
********************************************************************************************/

double NodeGridIso::GetDivisions(BOOL Scale)
{
	return NodeGrid::CalcDivisions(Scale);
}


/********************************************************************************************

>	UINT32 NodeGridIso::GetSubdivisions()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/94
	Inputs:		-
	Outputs:	-
	Returns:	Num subdivisions
	Purpose:	Used to get the number of subdivisions between each main grid point.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

UINT32 NodeGridIso::GetSubdivisions()
{
	return Subdivisions;
}

/********************************************************************************************
>	UnitType NodeGridIso::GetUnits(BOOL Scale=TRUE)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/95
	Inputs:		Scale - account for unit scaling (only FALSE when saving for compatibility with old docs)
	Returns:	Units used by the grid
	Purpose:	Gets the units used by the grid
********************************************************************************************/

UnitType NodeGridIso::GetUnits(BOOL Scale)
{
	DimScale* pDimScale = DimScale::GetPtrDimScale(this);
	ERROR2IF(pDimScale==NULL,NOTYPE,"NodeGridIso::GetUnits() - pDimScale==NULL");

	// if scaling turned on return scale units
	// (except if scaling is not to be accounted for  - ie during saving for compatibility with old docs)
	UnitType ScaleUnits = pDimScale->GetScaleUnits();
	if (Scale && ScaleUnits!=NOTYPE)
		return ScaleUnits;

	return Units;
}


//---------------------------------------------
//---------------------------------------------

/********************************************************************************************

>	static void NodeGrid::ProcessAllGrids(ProcessGrid* pProcessGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pProcessGrid = ptr to the object that will process each grid in the current doc
	Returns:	-
	Purpose:	Processes all the grids in the current doc using the grid processing object
				provided as a parameter
	SeeAlso:	-

********************************************************************************************/

void NodeGrid::ProcessAllGrids(ProcessGrid* pProcessGrid)
{
	ERROR3IF(Document::GetSelected() == NULL,"What, no doc? Surely some mistake!");

	pProcessGrid->pDoc = Document::GetSelected();
	if (pProcessGrid->pDoc == NULL) return;

	// Now we need to scan all spreads throughout the current doc

	pProcessGrid->pChapter = Node::FindFirstChapter(pProcessGrid->pDoc);
    while (pProcessGrid->pChapter != NULL)
	{
		// For each chapter in the doc...
		Node* pNode = pProcessGrid->pChapter->FindFirstChild();
		while (pNode != NULL)
		{
			// If the child node of the chapter is a spread...
			if (pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
				ProcessGridsInSpread(pProcessGrid,(Spread*)pNode);
	
			pNode = pNode->FindNext();
		}
		pProcessGrid->pChapter = pProcessGrid->pChapter->FindNextChapter();
	}
}    


/********************************************************************************************

>	static void NodeGrid::ProcessGridsInSpread(ProcessGrid* pProcessGrid,Spread* pSpread)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pProcessGrid = ptr to the object that will process each grid in the current doc
				pSpread  	 = spread containing grids to be processed
	Returns:	-
	Purpose:	Processes all grids in this spread using the given grid processor param
	SeeAlso:	-

********************************************************************************************/

void NodeGrid::ProcessGridsInSpread(ProcessGrid* pProcessGrid,Spread* pSpread)
{
	pProcessGrid->pSpread = pSpread;

	// scan the children of the spread for grids
	Node* pNode = pSpread->FindFirstChild();
	while (pNode != NULL)
	{
		if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeGrid)))
			// we now have a ptr to a grid, so process it
			pProcessGrid->Process((NodeGrid*)pNode);

		pNode = pNode->FindNext();
	}
}

//-------
//-------
//-------

/********************************************************************************************

>	void ProcessGridForceRedraw::Process(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pGrid = the grid to force a redraw on
	Returns:	-
	Purpose:	Forces a redraw on the given grid
	SeeAlso:	-

********************************************************************************************/

void ProcessGridForceRedraw::Process(NodeGrid* pGrid)
{
	if (!pGrid->IsDisabled())
	{
		// Get ptr to selected doc view
		DocView* pDocView = DocView::GetSelected();

		// If there's a selected docview, redraw!
		if (pDocView != NULL)
			pDocView->ForceRedraw(pSpread,pGrid->GetBlobBoundingRect());
	}
}

//--------
//--------
//--------

/********************************************************************************************

>	ProcessGridUnitMsg::ProcessGridUnitMsg(UnitMsg* pThisUnitMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pUnitMsg = ptr to the unit msg
	Returns:	-
	Purpose:	THE constructor for this class
				This assigns pThisUnitMsg to the member var pUnitMsg ready for its
				Process method to be called vai NodeGrid::ProcessAllGrids()
	SeeAlso:	-

********************************************************************************************/

ProcessGridUnitMsg::ProcessGridUnitMsg(UnitMsg* pThisUnitMsg)
{
	pUnitMsg = pThisUnitMsg;
}

/********************************************************************************************

>	void ProcessGridUnitMsg::Process(NodeGrid* pGrid)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/7/94
	Inputs:		pGrid = the grid to process
	Returns:	-
	Purpose:	Checks to see if the grid is using the unit specified in the UnitMsg
				member var, and if so changes it if the unit is about to be deleted
	SeeAlso:	-

********************************************************************************************/

void ProcessGridUnitMsg::Process(NodeGrid* pGrid)
{
	switch (pUnitMsg->MsgType)
	{
		case (UnitMsg::BEFOREDELETE) :
		{
			UnitType OldUnitType = pUnitMsg->ThisUnitType;

			if (pGrid->GetUnits() == OldUnitType)
			{
				double 	 Div    	 = pGrid->GetDivisions();
				UINT32	 SubDiv 	 = pGrid->GetSubdivisions();
				Unit*	 pThisUnit 	 = pUnitMsg->pDocUnitList->FindUnit(OldUnitType);
				UnitType NewUnitType = pThisUnit->GetBaseUnitType();

				ENSURE(NewUnitType != NOTYPE,"Er, deleting a unit that has no base type");

				Div = Convert::ConvertToNewUnits(Div,OldUnitType,NewUnitType);
			
				pGrid->SetGridParams(Div,SubDiv,NewUnitType);
			}
		}
		break;

		case (UnitMsg::CHANGED) :
		{
			UnitType ChangedUnitType = pUnitMsg->ThisUnitType;

			if (pGrid->GetUnits() == ChangedUnitType)
			{
				double 	 Div    	 = pGrid->GetDivisions();
				UINT32	 SubDiv 	 = pGrid->GetSubdivisions();

				pGrid->SetGridParams(Div,SubDiv,ChangedUnitType);

				if (!pGrid->IsDisabled())
				{
					// Get ptr to selected doc view
					DocView* pDocView = DocView::GetSelected();

					// If there's a selected docview, redraw!
					if (pDocView != NULL)
						pDocView->ForceRedraw(pGrid->FindParentSpread(),pGrid->GetBlobBoundingRect());
				}

				//GridTool::ForceRedraw(pGrid);
			}
		}
		break;
		default: break;
	}	
}

/********************************************************************************************

>	static void NodeGrid::ForceRedrawAllGrids()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/10/94
	Inputs:		-
	Returns:	-
	Purpose:	Forces a redraw on all the grids in the current view
	SeeAlso:	

********************************************************************************************/

void NodeGrid::ForceRedrawAllGrids()
{
	ProcessGridForceRedraw ProcGrid;
	NodeGrid::ProcessAllGrids(&ProcGrid);
}    

/********************************************************************************************
>	static void NodeGrid::MakeDefaultGrid(Spread* pSpread, BOOL Scale=TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/94
	Inputs:		pSpread = spread in which to make the Default grid
				Scale - account for unit scaling (only FALSE when loading for doc compatibility)
	Purpose:	Creates a grid covering the whole spread.  The grid is
				defined using the default parameters.
********************************************************************************************/

void NodeGrid::MakeDefaultGrid(Spread* pSpread, BOOL Scale)
{
	if (pSpread != NULL)
	{
		NodeGrid* pNewGrid = NULL;				// This will point to the new grid
		switch (GetDefaultGridType())			// Create the new grid of the right type
		{
			case RECTANGULAR 	: pNewGrid = new NodeGridRect(); break;
			case ISOMETRIC 		: pNewGrid = new NodeGridIso(); break;
			default: break; // we check it below
		}

		if (pNewGrid != NULL)
		{
			// Build the grid rectangle to completely cover the pasteboard
			DocRect Rect = pSpread->GetPasteboardRect(FALSE);
			pSpread->DocCoordToSpreadCoord(&Rect);

			pNewGrid->AttachNode(pSpread, LASTCHILD); 
//			pNewGrid->SetInitialBounds(Rect);
			pNewGrid->SetBoundingRect(Rect);			// Tell the NodeGrid of the new bounds
			pNewGrid->SetDefault(TRUE);
			pNewGrid->SetDisabled(GetDefaultGridDisabled());

			// Set the grid (and user coord) origin to the bottom left corner of the 1st page
//			DocRect PageRect;
//			if (pSpread->GetPagesRect(&PageRect))
//				pNewGrid->SetOrigin(PageRect.lo.x, PageRect.lo.y);

			// Set the grid params to the default grid settings
			// If the default unit type is NOTYPE, get the page units from the doc unit list
			// of the current document
			UnitType Units = GetDefaultUnits();
			if (Units == NOTYPE)
			{
				// Get the node's dic unit list
				DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();

				ENSURE(pDocUnitList != NULL,"No doc unit list attached to this doc yet");
				if (pDocUnitList != NULL)
					Units = pDocUnitList->GetPageUnits();  		// Set to the page units.
			}

			if (Units != NOTYPE)
				pNewGrid->SetGridParams(GetDefaultDivisions(),GetDefaultSubdivisions(),Units,Scale);
		}
	}
}

/********************************************************************************************

>	static void NodeGrid::ResetDocRectSnap()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/10/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Call this before starting a drag that will use the DocRect version of the snap function
	SeeAlso:	-

********************************************************************************************/

void NodeGrid::ResetDocRectSnap()
{ 
	LastXDir = LastYDir = 0; 
}

//----------------------------------------------
// Moved these from the class def in grid.h so that they are no longer in-line,
// hence allowing them to be called from DLLs

UINT32 		NodeGrid::GetNumSelectedGrids() 			{ return NumSelected; }
UINT32 		NodeGrid::GetNumNonDefaultSelectedGrids() 	{ return NumNonDefaultSelected; }

GridType	NodeGrid::GetTypeForDefaultGrid() 	{ return TypeForDefaultGrid; }
BOOL		NodeGrid::GetDefaultGridDisabled()	{ return DefaultGridDisabled; }
double		NodeGrid::GetDefaultDivisions()		{ return DefaultDivisions; }
UINT32		NodeGrid::GetDefaultSubdivisions()	{ return DefaultSubdivisions; }
UnitType	NodeGrid::GetDefaultUnits()			{ return DefaultUnits; }
GridType	NodeGrid::GetDefaultGridType() 		{ return DefaultGridType; }

void		NodeGrid::SetTypeForDefaultGrid(GridType GType) 	{ TypeForDefaultGrid 	= GType; }
void		NodeGrid::SetDefaultGridDisabled(BOOL state)		{ DefaultGridDisabled 	= state; }
void 		NodeGrid::SetDefaultDivisions(double Div)			{ DefaultDivisions 		= Div; }
void		NodeGrid::SetDefaultSubdivisions(UINT32 Sub)			{ DefaultSubdivisions 	= Sub; }
void 		NodeGrid::SetDefaultUnits(UnitType UType)			{ DefaultUnits 			= UType; }
void		NodeGrid::SetDefaultGridType(GridType	GType) 		{ DefaultGridType		= GType; }

//--------------------------------------------------------------------
//--------------------------------------------------------------------
// Support functions for unit processing
//--------------------------------------------------------------------
//--------------------------------------------------------------------

static UnitType GetDominantUnit(UnitType ThisUnit)
{
//	for (INT32 u=0;(u<NUM_DEFAULT_UNIT_TYPES) && (UnitDataList[u].Units != Units);u++) ;
//	if (u >= NUM_DEFAULT_UNIT_TYPES) return NOTYPE; else return UnitDataList[u].DominantUnit;

	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	Unit* pThisUnit = pDocUnitList->FindUnit(ThisUnit);
	double NumMills = pThisUnit->GetMillipoints();

	UnitType ThisUnitBaseType = pThisUnit->GetBaseUnitType();
	if (ThisUnitBaseType != NOTYPE)
	{
		Unit*  pBaseUnit = pDocUnitList->FindUnit(pThisUnit->GetBaseUnitType());

		if (NumMills < pBaseUnit->GetMillipoints())
			return (pBaseUnit->GetUnitType());
	}

	INT32 NumUnits = pDocUnitList->GetNumUnits();
	INT32 u;
	double   ResultMills = 1.7E308;
	UnitType ResultUnitType = NOTYPE;

	for (u=0;u < NumUnits;u++)
	{
		Unit* pUnit = pDocUnitList->FindUnit(u);
		UnitType BaseUnitType = pUnit->GetBaseUnitType();

		if (BaseUnitType != NOTYPE)
		{
			if (BaseUnitType == ThisUnit)
			{
				double m = pUnit->GetMillipoints();
				if ((m > NumMills) && (m < ResultMills))
				{
					ResultMills = m;
					ResultUnitType = pUnit->GetUnitType();
				}
			}
		}
	}

	return ResultUnitType;
}

static double GetUnitMultiple(UnitType Units)
{
//	for (INT32 u=0;(u<NUM_DEFAULT_UNIT_TYPES) && (UnitDataList[u].Units != Units);u++) ;
//	if (u >= NUM_DEFAULT_UNIT_TYPES) return 1; else return UnitDataList[u].Multiple;

	UnitType DomUnitType = ::GetDominantUnit(Units);

	if (DomUnitType == NOTYPE)
		return 2.0;

	DocUnitList* pDocUnitList = DocUnitList::GetCurrentDocUnitList();
	Unit* pUnit    = pDocUnitList->FindUnit(Units);
	Unit* pDomUnit = pDocUnitList->FindUnit(DomUnitType);
	double Multiple;
	
	if (pUnit->GetBaseUnitType() == pDomUnit->GetUnitType())
		Multiple = pUnit->GetBaseDenominator()  / pUnit->GetBaseNumerator();
	else
		Multiple = pDomUnit->GetBaseNumerator() / pDomUnit->GetBaseDenominator();

	if (Multiple <= 1.0)
		return 2.0;
	else
		return Multiple;
}

/*
static struct 
{
	UnitType  	Units;
	TCHAR* 		Suffix;
	UINT32		Multiple;
	UnitType	DominantUnit;
} UnitDataList[NUM_DEFAULT_UNIT_TYPES] = 
{
	{ MILLIMETRES,	"mm", 10,   CENTIMETRES	},
	{ CENTIMETRES,	"cm", 100,  METRES 		},
	{ METRES,		"m" , 1000, NOTYPE 		},
	{ INCHES,		"in", 12,   FEET 		},
	{ FEET,			"ft", 3,    YARDS 		},
	{ YARDS,		"yd", 1760, NOTYPE 		},
	{ COMP_POINTS,	"pt", 72,   PICAS 		},
	{ PICAS,		"pi", 6,    INCHES 		},
	{ MILLIPOINTS,	"mp", 1000, COMP_POINTS }
};
*/


//static TCHAR* GetUnitSuffix(UnitType Units)
//{
//	for (INT32 u=0;(u<NUM_DEFAULT_UNIT_TYPES) && (UnitDataList[u].Units != Units);u++) ;
//	if (u >= NUM_DEFAULT_UNIT_TYPES) return ""; else return UnitDataList[u].Suffix;

//	Unit* pUnit = pDocUnitList->FindUnit(DomUnitType);
//	String_32 Spec = pUnit->GetSpecifier();
//	return ((TCHAR*)Spec);
//}

//-------------
//---------------------------------------------------------------
//---------------------------------------------------------------
//---------------------------------------------------------------

/********************************************************************************************

>	virtual BOOL NodeGrid::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the grid record to the filter (if it's required)
	SeeAlso:	Node::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

********************************************************************************************/

BOOL NodeGrid::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	// do nothing in the web format
	return FALSE;
}

//--------------------------------------------------------------
// See NodeGrid::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL NodeGrid::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	BOOL RecordWritten = TRUE;

	// Write out the page sizes etc for this spread
	if (RecordWritten) RecordWritten = WriteGridAndRulerSettings(pFilter);
	if (RecordWritten) RecordWritten = WriteGirdAndRulerOrigin(pFilter);

	return RecordWritten;
#else
	return TRUE;
#endif //DO_EXPORT
}

/********************************************************************************************

>	BOOL NodeGrid::WriteGridAndRulerSettings(BaseCamelotFilter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports the settings for the grid and rulers to the filter.
				This is the default grid, as this acts as the settings for the grid,
				as you would expect, and the page rulers.
	SeeAlso:	NodeGrid::WritePreChildrenNative; NodeGrid::WritePreChildrenWeb;

********************************************************************************************/

BOOL NodeGrid::WriteGridAndRulerSettings(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = FALSE;
	
	// Only write out this set of details if it is the default grid
	if (IsDefault())
	{
		// Go and get the numbers out of the grid
		// BODGE - old builds (hence docs) save the grid spacing in divisions and units but don't
		// account for unit scaling, so as not to change doc format new docs do the same so we must
		// read the grid 'Divisions' with scaling turned off - yuk!
		// But surely, it is safer to save out the units unscaled!
		BOOL Scale = FALSE;
		double   Divisions    = GetDivisions(Scale);
		UnitType Unit         = GetUnits(Scale);
		UINT32     SubDivisions = GetSubdivisions();
		GridType TypeOfGrid   = GetGridType();
		
		// Convert the unittype into a reference
		UnitListComponent * pUnitsComponent = pFilter->GetUnitDocComponent();
		ERROR2IF(pUnitsComponent == NULL,FALSE,"WriteGridAndRulerSettings No units doc component present");

		INT32 UnitsRef = pUnitsComponent->GetWriteUnitReference(Unit, pFilter);

		CXaraFileRecord Rec(TAG_GRIDRULERSETTINGS, TAG_GRIDRULERSETTINGS_SIZE);

		ok = Rec.Init();

		if (ok) ok = Rec.WriteINT32(UnitsRef);	
		if (ok) ok = Rec.WriteDOUBLE(Divisions);	
		if (ok) ok = Rec.WriteUINT32(SubDivisions);	
		if (ok) ok = Rec.WriteBYTE((BYTE)TypeOfGrid);	

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

>	BOOL Spread::WriteGirdAndRulerOrigin(BaseCamelotFilter* pFilter)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/8/96
	Input:		pFilter = ptr to the filter to export to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:	Exports the origin for the grid and ruler to the filter.
				This is the default grid, as this acts as the settings for the grid,
				as you would expect, and the page rulers.
	SeeAlso:	NodeGrid::WritePreChildrenNative; NodeGrid::WritePreChildrenWeb;

********************************************************************************************/

BOOL NodeGrid::WriteGirdAndRulerOrigin(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = FALSE;
	
	// Only write out this set of details if it is the default grid
	if (IsDefault())
	{
		// Read the grid (user) origin
		DocCoord Origin(0,0);
		GetOrigin(&Origin.x, &Origin.y);
		
		// We will now translate this to be relative to the page origin, as it will be displayed
		// to the user
		Spread* pSpread = pFilter->GetSpread();
		DocCoord PageRelGridOrigin = Origin;
		if (pSpread)
			pSpread->SpreadCoordToPagesCoord(&PageRelGridOrigin, Origin);

		CXaraFileRecord Rec(TAG_GRIDRULERORIGIN, TAG_GRIDRULERORIGIN_SIZE);

		ok = Rec.Init();

		// Write out the origin
		if (ok) ok = Rec.WriteCoord(PageRelGridOrigin);	

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

