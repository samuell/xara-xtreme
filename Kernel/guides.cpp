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
// NodeGuideline implementation

/*
*/

#include "camtypes.h"

// WEBSTER - markn 14/1/97
// Removed all code not needed in Webster
// This is the same as the code removed from RALPH
#ifdef WEBSTER
#define EXCLUDE_FROM_RALPH
#endif

#include "guides.h"
#include "snap.h"
#include "stockcol.h"
//#include "rndrgn.h"
#include "attr.h"
#include "view.h"
#include "docview.h"
#include "spread.h"
#include "layer.h"
#include "sprdmsg.h"
#include "document.h"
#include "qualattr.h"
#include "camview.h"
//#include "markn.h"
//#include "ed.h"
//#include "tim.h"
#include "contmenu.h"
//#include "lyrprop.h"
//#include "resource.h"
#include "usercord.h"
#include "layermsg.h"
//#include "scrcamvw.h"
//#include "grndrgn.h"
//#include "osrndrgn.h"
//#include "cameleps.h"
#include "saveeps.h"
#include "nativeps.h"		// The old style EPS native filter, used in v1.1
#include "keypress.h"
#include "prpsgds.h"
#include "snapops.h"
//#include "viewrc.h"
#include "csrstack.h"
//#include "justin.h"
#include "app.h"
//#include "statline.h"
#include "toollist.h"

#include "cxfrec.h"
#include "cxfrech.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "camfiltr.h"
#include "page.h"

#if !defined(EXCLUDE_FROM_RALPH)
#include "optsgrid.h"
#include "statline.h"
#endif


static MILLIPOINT 	abs_milli(MILLIPOINT n) 		{ if (n<0) return (0-n); else return (n);}

#define Swap(a,b)       { (a)^=(b), (b)^=(a), (a)^=(b); }

#define MAX_INT (0x01000000)
#define MIN_INT (0xf0000000)

#define GUIDE_CURSORID_UNSET -1

CC_IMPLEMENT_DYNAMIC(NodeGuideline, 	 		NodeRenderableInk)
#if !defined(EXCLUDE_FROM_RALPH)
CC_IMPLEMENT_DYNCREATE(OpGuideline, 	 		UndoableOperation)
CC_IMPLEMENT_DYNCREATE(OpSpreadOrigin, 	 		UndoableOperation)
CC_IMPLEMENT_DYNCREATE(GuidelinePropDlg, 		DialogOp)
CC_IMPLEMENT_MEMDUMP(OpGuidelineParam,	 		OpParam)
CC_IMPLEMENT_DYNAMIC(LayerMsg,			 		Msg)
CC_IMPLEMENT_DYNCREATE(OpDeleteGuideline, 		OpGuideline)
CC_IMPLEMENT_DYNCREATE(OpNewGuideline,	 		OpGuideline)
CC_IMPLEMENT_DYNCREATE(OpDeleteAllGuidelines, 	OpGuideline)
CC_IMPLEMENT_DYNCREATE(OpResetSpreadOrigin, 	OpSpreadOrigin)
#else
CC_IMPLEMENT_DYNAMIC(LayerMsg,			 		Msg)
#endif

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#if !defined(EXCLUDE_FROM_RALPH)
//-------------------------------------

NodeGuideline*   OpDeleteGuideline::pGuideline = NULL;
OpGuidelineParam OpNewGuideline::NewGuidelineParam;

//-------------------------------------

const CDlgMode 			GuidelinePropDlg::Mode 	= MODAL;
const INT32				GuidelinePropDlg::IDD	= _R(IDD_GUIDELINE_PROPERTIES);
GuidelinePropDlgParams 	GuidelinePropDlg::Params;

//-------------------------------------
#endif

// WEBSTER - markn 14/1/97
// Helper func for NodeGuideline::Render() - still in it's experimental stage
#ifndef WEBSTER
static void PullOntoStep(INT32* pMin,INT32* pMax,INT32 Dist)
{
	if (Dist != 0)
	{
		*pMin -= *pMin % Dist;
		*pMax += Dist-(*pMax % Dist);
	}
}
#endif // WEBSTER

/*********************************************************************************************

>    NodeGuideline::NodeGuideline() 

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   5/9/95
     Inputs:    -
     Outputs:   
     Returns:   -
     Purpose: This constructor creates a NodeGuideline linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.
     Errors:    

**********************************************************************************************/
 

NodeGuideline::NodeGuideline(): NodeRenderableInk()
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	Type 	 = GUIDELINE_HORZ;
	Ordinate = 72000*9;
#endif // WEBSTER
}                    
    
  

/***********************************************************************************************

> Node* NodeGuideline::SimpleCopy() // Private method  

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    5/9/95
	Inputs:     - 
    Outputs:    -
    Returns:    A copy of the node, or NULL if memory runs out
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
    Errors:     If memory runs out when trying to copy, then ERROR is called with an out of memory
				error and the function returns NULL. 
	Scope:      protected       

**********************************************************************************************/

Node* NodeGuideline::SimpleCopy()
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	NodeGuideline* NodeCopy = new NodeGuideline();   
	if (NodeCopy != NULL)
		CopyNodeContents(NodeCopy);   
	return (NodeCopy);
#else
	return NULL;
#endif // WEBSTER
}          

/***********************************************************************************************

>   void NodeGuideline::CopyNodeContents(NodeGuideline* NodeCopy)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    5/9/95
	Inputs:		- 
	Outputs:    A copy of this node
    Returns:    -
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/


void NodeGuideline::CopyNodeContents(NodeGuideline* NodeCopy)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	ENSURE(NodeCopy != NULL,"Trying to copy a NodeGuideline's contents to a NULL node");  
	
	NodeRenderableInk::CopyNodeContents(NodeCopy); 

	NodeCopy->Type		= this->Type;
	NodeCopy->Ordinate	= this->Ordinate;
#endif // WEBSTER
}


/***********************************************************************************************
>   void NodeGuideline::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeGuideline::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeGuideline), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeGuideline))
		CopyNodeContents((NodeGuideline*)pNodeCopy);
}



/********************************************************************************************

>	void* NodeGuideline::GetDebugDetails(StringBase* Str) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		-
	Outputs:	Str: String giving debug info about the node
	Returns:	-
	Purpose:	For obtaining debug information about the Node
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

     
void NodeGuideline::GetDebugDetails(StringBase* Str) 
{          
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	NodeRenderableInk::GetDebugDetails(Str); 
#endif // WEBSTER
}

/********************************************************************************************

>	virtual String Describe(BOOL Plural, BOOL Verbose = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
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
              
String NodeGuideline::Describe(BOOL /*Plural*/, BOOL /*Verbose*/) 
{     
//	ENSURE (FALSE,"The illegal function NodeGuideline::Describe was called\n"); 
	
	return( _T("") ); // Just to keep the compiler happy
}; 

/***********************************************************************************************

>	static MILLIPOINT NodeGuideline::GetScaledPixelWidth(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pRender: Render region to render into (can be NULL)
	Outputs:	-
	Returns:	-   
	Purpose:    Gets the scaled pixel width.
				if pRender != NULL, the render view associated with it is used, otherwise
				the selected doc view is used.

				It uses the all View::GetScaledPixelSize() function.
		
***********************************************************************************************/

MILLIPOINT NodeGuideline::GetScaledPixelWidth(RenderRegion* pRender)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	MILLIPOINT Width = 0;
	View* pView = NULL;

	if (pRender != NULL)
		pView = pRender->GetRenderView();
	else
		pView = DocView::GetCurrent();

	if (pView != NULL)
	{
		// Get the scaled pixel size for the view
		FIXED16 ScaledPixelWidth,
				ScaledPixelHeight; // Not used
		pView->GetScaledPixelSize(&ScaledPixelWidth, &ScaledPixelHeight);
		Width = (MILLIPOINT)ScaledPixelWidth.MakeLong();
	}

	return Width;
#else
	return 0;
#endif // WEBSTER
	return 0;
}

/***********************************************************************************************

>	virtual void NodeGuideline::Transform( TransformBase& Trans )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		Trans = the transformation to apply
	Outputs:	-
	Returns:	-   
	Purpose:    Transforms the guideline using Trans
		
***********************************************************************************************/

void NodeGuideline::Transform( TransformBase& Trans )
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord Coord(Ordinate,Ordinate);

	Trans.Transform(&Coord,1);

	if (Type == GUIDELINE_HORZ)
		Ordinate = Coord.y;
	else
		Ordinate = Coord.x;
#endif
}

/***********************************************************************************************

>	virtual void NodeGuideline::PreExportRender(RenderRegion* pRegion)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pRegion = the render region to export to
	Outputs:	-
	Returns:	-   
	Purpose:    Called before any child nodes are asked to export
		
***********************************************************************************************/

void NodeGuideline::PreExportRender(RenderRegion* pRegion)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
#ifdef DO_EXPORT
PORTNOTE("EPSRenderRegion", "Removed use fo EPSRenderRegion")
#if !defined(EXCLUDE_FROM_XARALX)
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

//		pDC->OutputValue(INT32(TAG_GUIDELINE));	// Output start extended object tag and token
//		pDC->OutputToken("cso");
//		pDC->OutputNewLine();

		pDC->OutputValue(Ordinate);
		pDC->OutputValue(INT32(Type));

		pDC->OutputToken(_T("glne"));			// Camelot "guideline" token
		pDC->OutputNewLine();
	}
#endif
#endif
#endif // WEBSTER
}

/***********************************************************************************************

>	virtual BOOL NodeGuideline::ExportRender(RenderRegion* pRegion)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pRegion = the render region to export to
	Outputs:	-
	Returns:	TRUE if exported completely, FALSE to indicate Render() needs to be called
	Purpose:    Called after child nodes are asked to export
		
***********************************************************************************************/

BOOL NodeGuideline::ExportRender(RenderRegion* pRegion)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
#ifdef DO_EXPORT
PORTNOTE("EPSRenderRegion", "Removed use fo EPSRenderRegion")
#if !defined(EXCLUDE_FROM_XARALX)
	if (pRegion->IS_KIND_OF(NativeRenderRegion))
	{
		/*EPSExportDC *pDC = */ (EPSExportDC *) pRegion->GetRenderDC();

//		pDC->OutputToken(_T("ceo"));			// Output end extended object token
//		pDC->OutputNewLine();
	}
#endif
#endif
#endif // WEBSTER
	return TRUE;
}

/***********************************************************************************************

>	virtual void NodeGuideline::Render(RenderRegion* pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		RendRegion: Render region to render into
	Outputs:	-
	Returns:	-   
	Purpose:    For rendering a node
		
***********************************************************************************************/

void NodeGuideline::Render( RenderRegion* pRender )
{
#if !defined(EXCLUDE_FROM_RALPH)
	pRender->SaveContext();
	DocRect Rect = pRender->GetClipRect();

	DocCoord StartCoord,EndCoord;

//	INT32 DashUnit = (72000/4)*4;

	if (Type == GUIDELINE_HORZ)
	{
		StartCoord.x = Rect.lo.x;
		EndCoord.x   = Rect.hi.x;
		PullOntoStep(&StartCoord.x,&EndCoord.x,pRender->GetScaledPixelWidth()*4);

		StartCoord.y = EndCoord.y = Ordinate;
	}
	else
	{
		StartCoord.y = Rect.lo.y;
		EndCoord.y   = Rect.hi.y;
		PullOntoStep(&StartCoord.y,&EndCoord.y,pRender->GetScaledPixelWidth()*4);

		StartCoord.x = EndCoord.x = Ordinate;
	}

	pRender->DrawLine(StartCoord,EndCoord);
		
	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	BOOL NodeGuideline::NeedsToRender(RenderRegion *pRender)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		pRender - A pointer to the current render region (null if none)
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	This function will indicate to the caller whether or not
				we want to render the given node, according to the information passed in.
	SeeAlso:	NodeRenderableBounded::NeedsToRender

********************************************************************************************/

SubtreeRenderState NodeGuideline::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// If no cliprect supplied, assume we do need to render
	if (pRender==NULL)	
		return SUBTREE_ROOTANDCHILDREN;

	if (pRender->IsPrinting())
		return SUBTREE_NORENDER;

PORTNOTE("epsrr", "Removed use of EPSRenderRegion")
#if !defined(EXCLUDE_FROM_XARALX)
	if (pRender->IS_KIND_OF(EPSRenderRegion))
		return SUBTREE_NORENDER;
#endif

	return SUBTREE_ROOTANDCHILDREN;
#else
	return SUBTREE_NORENDER;
#endif
}

/********************************************************************************************

>	BOOL NodeGuideline::NeedsToExport(RenderRegion *pRender,BOOL VisibleLayersOnly = FALSE, BOOL CheckSelected = FALSE)
 
	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/5/94
	Inputs:		pRender - A pointer to the current render region (null if none)
				ExcludeInvisibleLayers - TRUE => remove nodes which are on invisible layers
									   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE  => This node should be exported,
				FALSE => This node does not need to be exported.
	Purpose:	This function will indicate to the caller whether or not
				we want to export the given node, according to the information passed in.

				NodeGuidelines only export to the native file format.

	SeeAlso:	NodeGuideline::NeedsToRender

********************************************************************************************/

BOOL NodeGuideline::NeedsToExport(RenderRegion *pRender,BOOL VisibleLayersOnly, BOOL CheckSelected)
{
// WEBSTER - markn 14/1/97
#ifdef DO_EXPORT
#ifndef WEBSTER
PORTNOTE("NativeRenderRegion","Removed NativeRenderRegion usage")
#ifndef EXCLUDE_FROM_XARALX
	return (pRender->IS_KIND_OF(NativeRenderRegion));
#endif
#endif // WEBSTER
#endif
	return FALSE;
}

/***********************************************************************************************

>	DocRect NodeGuideline::GetRenderRect(BOOL HitTest= FALSE, RenderRegion* pRender = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pRenderRect = ptr to a render region (if NULL current DocView is used)
				HitTest     - TRUE if being called during HitTest
	Outputs:	-
	Returns:	Bounding rect for this guideline
	Purpose:    see NodeGuideline::GetRenderRect(Ordinate,Type,HitTest,pRender)
	SeeAlso:	CSnap
		
***********************************************************************************************/

DocRect NodeGuideline::GetRenderRect(BOOL HitTest, RenderRegion* pRender)
{
	return NodeGuideline::GetRenderRect(Ordinate,Type,HitTest,pRender);
}

/***********************************************************************************************

>	static DocRect NodeGuideline::GetRenderRect(MILLIPOINT Ordinate,GuidelineType Type,BOOL HitTest= FALSE, RenderRegion* pRender = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		Ordinate    = ordinate value
				Type		= guideline type
				pRenderRect = ptr to a render region (if NULL current DocView is used)
				HitTest     - TRUE if being called during HitTest
	Outputs:	-
	Returns:	Rect to render
	Purpose:    Main routine for calculating the bounding rect that represents a guideline
				This is mainly used for invalidating areas on screen for redraw purposes.
				If HitTest is TRUE, the rect is inflated by the size of the magnetic snap distance
				so that clicking near a guideline in order to drag it will work.
	SeeAlso:	NodeGuideline::GetScaledPixelWidth()
		
***********************************************************************************************/

DocRect NodeGuideline::GetRenderRect(MILLIPOINT Ordinate,GuidelineType Type,BOOL HitTest, RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect Rect = DocRect(Ordinate,Ordinate,Ordinate,Ordinate);

	MILLIPOINT PixelWidth = GetScaledPixelWidth(pRender);

	if (HitTest)
		PixelWidth = (CSnap::GetSnapDist()*2);
	else
		PixelWidth *= 2;

	if (Type == GUIDELINE_HORZ)
	{
		Rect.lo.x = MIN_INT;
		Rect.hi.x = MAX_INT;
		Rect.lo.y -= PixelWidth/2;
		Rect.hi.y += PixelWidth/2;
	}
	else
	{
		Rect.lo.x -= PixelWidth/2;
		Rect.hi.x += PixelWidth/2;
		Rect.lo.y = MIN_INT;
		Rect.hi.y = MAX_INT;
	}

	return Rect;
#else
	return DocRect(0,0,0,0);
#endif
}

/***********************************************************************************************

>	virtual DocRect	NodeGuideline::GetBoundingRect(BOOL DontUseAttrs=FALSE, BOOL HitTest=FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		DontUseAttrs - TRUE if you want to ignore all the nodes attributes
				FALSE by default.
				HitTest      - TRUE if being called during HitTest
	Outputs:	-
	Returns:	Bounding rect for the guideline
	Purpose:    Calcs the bounds of the guideline.  This varies with the current view.

				NOTE: 	Bounds are only given for hit testing.  This is so the bounds of guidelines
						aren't incorporated into the bounds of its parent (i.e. the layer).
						If you don't do this, doing a 'zoom to objects' will try and zoom to guidelines
						which is very wrong
	SeeAlso:	CSnap
		
***********************************************************************************************/

DocRect	NodeGuideline::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	if (HitTest)
		return GetRenderRect(HitTest);
	else
#endif // WEBSTER
		return DocRect(0,0,0,0);
}

/********************************************************************************************

>	virtual BOOL NodeGuideline::OnClick( DocCoord PointerPos, ClickType Click, 
									 ClickModifiers ClickMods, Spread *pSpread )

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/95
	Inputs:		PointerPos 	- The Location of the mouse pointer at the time of the click
				Click 		- The type of click received (single, double, drag etc)
				ClickMods 	- The modifiers to the click (eg shift, control etc )
				pSpread		- The spread the click occured in
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the Node to respond to clicks.

				Guidelines will return TRUE for ALL clicks.

				They only acually do something on CLICKTYPE_SINGLE clicks.  With this click they
				initiate a drag on the guideline to allow the user to move them about the page.

***********************************************************************************************/

BOOL NodeGuideline::OnClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods, Spread *pSpread)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (ClickMods.Menu)
		return FALSE;

	if (Click == CLICKTYPE_SINGLE)
	{
		OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GUIDELINE);
		ERROR3IF(pOpDesc == NULL,"FindOpDescriptor(OPTOKEN_GUIDELINE) failed");

		if (pOpDesc != NULL)
		{
			OpGuidelineParam GuidelineParam;

			GuidelineParam.Method 		= GUIDELINEOPMETHOD_MOVE_DRAG;
			GuidelineParam.pGuideline	= this;

			pOpDesc->Invoke(&GuidelineParam);
		}
	}
#endif
	return TRUE;
}


/********************************************************************************************
>	virtual BOOL NodeGuideline::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/5/95
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the guideline to respond to pop up menu clicks on itself.
********************************************************************************************/

BOOL NodeGuideline::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
//	WEBSTER-ranbirr-01/12/96	
#ifndef WEBSTER
	BOOL ok = TRUE;

#if !defined(EXCLUDE_FROM_RALPH)
	// set up data required for ops which could be invoked
	GuidelinePropDlg::SetEditGuidelineParams(this);
	OpDeleteGuideline::SetGuideline(this);

	ok = ok && pMenu->BuildCommand(OPTOKEN_DELETEGUIDELINE);
	ok = ok && pMenu->BuildCommand(OPTOKEN_SNAPTOGUIDES,TRUE);
	ok = ok && pMenu->BuildCommand(OPTOKEN_EDITGUIDELINEPROPDLG);
	ok = ok && pMenu->BuildCommand(OPTOKEN_GUIDEPROPERTIESDLG);
#endif

	return ok;
#else //webster
	return FALSE;
#endif //webster
}

/********************************************************************************************

>	virtual void NodeGuideline::SetSelected(BOOL Status)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		Status = selection state (ignored)
	Returns:	-
	Purpose:	Overrides the base class selection function
	
				'Status' is ignored and the node is always forced to be unselected.

***********************************************************************************************/

void NodeGuideline::SetSelected(BOOL Status)
{
	Flags.Selected = FALSE;
}

/***********************************************************************************************

>	MILLIPOINT NodeGuideline::ExtractOrdinate(DocCoord* pDocCoord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pDocCoord = A DocCoord in Spread coords
	Outputs:	-
	Returns:	Either the X or the Y ordinate, dependant on the guideline type
	Purpose:    Central place for ordinate extraction
	SeeAlso:	CSnap
		
***********************************************************************************************/

MILLIPOINT NodeGuideline::ExtractOrdinate(DocCoord* pDocCoord)
{
#ifndef WEBSTER
	// WEBSTER - markn 14/1/97
	ERROR3IF(pDocCoord == NULL,"pDocCoord is NULL");

	if (Type == GUIDELINE_HORZ)
		return pDocCoord->y;
	else
		return pDocCoord->x;
#else
	return 0;
#endif // WEBSTER
}

/***********************************************************************************************

>	void NodeGuideline::ReplaceOrdinate(DocCoord* pDocCoord,MILLIPOINT Ordinate)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pDocCoord = A DocCoord in Spread coords
				Ordinate  = ordinate value
	Outputs:	Either the X or the Y ordinate of pDocCoord set to Ordinate, dependant on the guideline type
	Returns:	-
	Purpose:    Central place for ordinate setting
	SeeAlso:	CSnap
		
***********************************************************************************************/

void NodeGuideline::ReplaceOrdinate(DocCoord* pDocCoord,MILLIPOINT Ordinate)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	ERROR3IF(pDocCoord == NULL,"pDocCoord is NULL");

	if (Type == GUIDELINE_HORZ)
		pDocCoord->y = Ordinate;
	else
		pDocCoord->x = Ordinate;
#endif // WEBSTER
}


/***********************************************************************************************

>	static MILLIPOINT NodeGuideline::ToSpreadOrdinate(Spread* pSpread,MILLIPOINT UserOrdinate,GuidelineType Type)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		pSpread		= ptr to a spread
				UserOrdinate= ordinate value in UserCoords
				Type 		= the type of guideline
	Outputs:	-
	Returns:	Spread ordinate version of UserOrdinate
	Purpose:    Converts a given ordinate within UserCoord space, into a Spread-related ordinate
	SeeAlso:	
		
***********************************************************************************************/

MILLIPOINT NodeGuideline::ToSpreadOrdinate(Spread* pSpread,MILLIPOINT UserOrdinate,GuidelineType Type)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	ERROR2IF(pSpread == NULL,0,"pSpread is NULL");

	UserCoord 	UCoord(UserOrdinate,UserOrdinate);
	DocCoord 	DCoord = UCoord.ToSpread(pSpread);

	if (Type == GUIDELINE_HORZ)
		return DCoord.y;
	else
		return DCoord.x;
#else
	return 0;
#endif // WEBSTER
}

/***********************************************************************************************

>	static MILLIPOINT NodeGuideline::ToUserOrdinate(Spread* pSpread,MILLIPOINT SpreadOrdinate,GuidelineType Type)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/10/95
	Inputs:		pSpread			= ptr to a spread
				SpreadOrdinate	= ordinate value in UserCoords
				Type 			= the type of guideline
	Outputs:	-
	Returns:	User ordinate version of SpreadOrdinate
	Purpose:    Converts a given ordinate within SpreadCoord space, into a User-related ordinate
	SeeAlso:	
		
***********************************************************************************************/

MILLIPOINT NodeGuideline::ToUserOrdinate(Spread* pSpread,MILLIPOINT SpreadOrdinate,GuidelineType Type)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	ERROR2IF(pSpread == NULL,0,"pSpread is NULL");

	DocCoord 	DCoord(SpreadOrdinate,SpreadOrdinate);
	UserCoord 	UCoord = DCoord.ToUser(pSpread);

	if (Type == GUIDELINE_HORZ)
		return UCoord.y;
	else
		return UCoord.x;
#else
	return 0;
#endif // WEBSTER
}


/***********************************************************************************************

>	void NodeGuideline::TranslateRect(DocRect* pDocRect,MILLIPOINT Delta)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pDocRect = ptr to a DocRect
				Delta 	 = amount to translate rect by
	Outputs:	Either the X or the Y ordinates of pDocRec are translated by Delta, dependant on the guideline type
	Returns:	-
	Purpose:    Central place for doc rect translation
	SeeAlso:	CSnap
		
***********************************************************************************************/

void NodeGuideline::TranslateRect(DocRect* pDocRect,MILLIPOINT Delta)
{
// WEBSTER - markn 14/1/97
#ifndef WEBSTER
	ERROR3IF(pDocRect == NULL,"pDocRect is NULL");

	if (Type == GUIDELINE_HORZ)
		pDocRect->Translate(0,Delta);
	else
		pDocRect->Translate(Delta,0);
#endif // WEBSTER
}

/***********************************************************************************************

>	virtual BOOL NodeGuideline::Snap(DocCoord* pDocCoord)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		pDocCoord = A DocCoord in Spread coords
	Outputs:	-
	Returns:	TRUE if pDocCoord has snapped to this guideline - FALSE otherwise
	Purpose:    The snap function for DocCoords
	SeeAlso:	CSnap
		
***********************************************************************************************/

BOOL NodeGuideline::Snap(DocCoord* pDocCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	MILLIPOINT SnapDist 	= CSnap::GetSnapDist();
	MILLIPOINT SrcOrdinate  = ExtractOrdinate(pDocCoord);

	MILLIPOINT Dist = abs_milli(Ordinate-SrcOrdinate);

	if (Dist <= SnapDist)
	{
		ReplaceOrdinate(pDocCoord,Ordinate);
		return TRUE;
	}
#endif
	return FALSE;
}

/***********************************************************************************************

>	virtual BOOL NodeGuideline::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord);

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/9/95
	Inputs:		A DocRect, and two coords
	Outputs:	-
	Returns:	TRUE if pDocRect has snapped to this guideline - FALSE otherwise
	Purpose:    Place holder for derived classes
	SeeAlso:	CSnap
		
***********************************************************************************************/

BOOL NodeGuideline::Snap(DocRect* pDocRect,const DocCoord& PrevCoord,const DocCoord& CurCoord)
{
#if !defined(EXCLUDE_FROM_RALPH)
	MILLIPOINT SnapDist = CSnap::GetSnapDist();

	MILLIPOINT SrcOrdinate  = ExtractOrdinate(&pDocRect->lo);
	MILLIPOINT Dist = Ordinate-SrcOrdinate;
	if (abs_milli(Dist) <= SnapDist)
	{
		TranslateRect(pDocRect,Dist);
		return TRUE;
	}

	SrcOrdinate  = ExtractOrdinate(&pDocRect->hi);
	Dist = Ordinate-SrcOrdinate;
	if (abs_milli(Dist) <= SnapDist)
	{
		TranslateRect(pDocRect,Dist);
		return TRUE;
	}
#endif
	return FALSE;
}

/***********************************************************************************************

>	virtual BOOL NodeGuideline::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pFilter = ptr to filter to write to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:    Web format doesn't export guidelines
	SeeAlso:	
		
***********************************************************************************************/

BOOL NodeGuideline::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	return FALSE;
}


/***********************************************************************************************

>	virtual BOOL NodeGuideline::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/96
	Inputs:		pFilter = ptr to filter to write to
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise
	Purpose:    Saves out the guideline in the v2 native file format
	SeeAlso:	
		
***********************************************************************************************/

BOOL NodeGuideline::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(UINT32(Type) > 255,"Can't store the guideline type in a byte");

	CamelotFileRecord Rec(pFilter,TAG_GUIDELINE,TAG_GUIDELINE_SIZE);

	BOOL	ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(Type);

	if (ok && Type == GUIDELINE_HORZ)
		ok = Rec.WriteYOrd(Ordinate);
	else
		ok = Rec.WriteXOrd(Ordinate);

	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------------------------------------
                                       
#ifdef _DEBUG
 
void NodeGuideline::ShowDebugTreeDetails() const
{                                 
	NodeRenderableInk::ShowDebugTreeDetails(); 
}  
#endif


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

#if !defined(EXCLUDE_FROM_RALPH)

/***********************************************************************************************

>	OpGuideline::OpGuideline()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default constructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpGuideline::OpGuideline()
{
	pDraggedGuideline = NULL;
	pBroadcastLayer   = NULL;
	UndoIDS 		  = _R(IDS_MARKN_EMPTY);
	LeaveCopy		  = FALSE;
	pCursor			  = NULL;
	CurrentStatusHelp = _R(IDS_MARKN_EMPTY);
	CursorStackID 	  = GUIDE_CURSORID_UNSET;
	RenderOn		  = TRUE;
}


/***********************************************************************************************

>	OpGuideline::~OpGuideline()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default destructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpGuideline::~OpGuideline()
{
	if (pCursor != NULL)
		delete pCursor;
}

/***********************************************************************************************

>	void OpGuideline::BroadcastGuidelineChanges(NodeGuideline* pGuideline)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		pGuideline = ptr to guideline
	Outputs:	-
	Returns:	-
	Purpose:    Broadcasts a LayerMsg::LayerReason::GUIDELINES_CHANGED for the parent layer
				of pGuideline
	SeeAlso:	DoWithParams()
		
***********************************************************************************************/

void OpGuideline::BroadcastGuidelineChanges(NodeGuideline* pGuideline)
{
	if (pGuideline != NULL)
		BroadcastGuidelineChanges((Layer*)pGuideline->FindParent(CC_RUNTIME_CLASS(Layer)));
}

/***********************************************************************************************

>	void OpGuideline::BroadcastGuidelineChanges(Layer* pLayer)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		pLayer = ptr to layer
	Outputs:	-
	Returns:	-
	Purpose:    Broadcasts a LayerMsg::LayerReason::GUIDELINES_CHANGED for the given layer
	SeeAlso:	DoWithParams()
		
***********************************************************************************************/

void OpGuideline::BroadcastGuidelineChanges(Layer* pLayer)
{
	if (pLayer != NULL)
	{
		BROADCAST_TO_ALL(LayerMsg(pLayer,LayerMsg::GUIDELINES_CHANGED));
	}

	pBroadcastLayer = pLayer;
}

/***********************************************************************************************

>	void OpGuideline::DoWithParam(OpDescriptor* pOpDesc,OpParam* pParam)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/9/95
	Inputs:		pOpDesc = ptr op desc
				pOpParam = param block to use
	Outputs:	-
	Returns:	-
	Purpose:    This function is used to create a guideline in the selected spread.
				It calls DoAddNewGuideline() with pParam->Param1 specifying the guideline type.
	SeeAlso:	DoAddNewGuideline(), OilRuler::OnLButtonDown()
		
***********************************************************************************************/

void OpGuideline::DoWithParam(OpDescriptor* pOpDesc,OpParam* pParam)
{
	ERROR3IF(pParam == NULL,"pParam is NULL");

	if (pParam != NULL)
	{
		OpGuidelineParam* pGuidelineParam = (OpGuidelineParam*)pParam;
		NodeGuideline* pGuideline = pGuidelineParam->pGuideline;

		OpMethod = pGuidelineParam->Method;

		switch (OpMethod)
		{
			//-----------------------------------
			case GUIDELINEOPMETHOD_MOVE_DRAG:
			{
				ERROR3IF(pGuideline == NULL,"pGuideline is NULL");

				if (pGuideline != NULL)
				{
					DocCoord PointerPos(pGuideline->GetOrdinate(),pGuideline->GetOrdinate());
					DoMoveGuideline(NULL,PointerPos,pGuideline);
				}
			}
			UndoIDS = _R(IDS_OPMOVEGUIDELINE);
			break;

			//-----------------------------------
			case GUIDELINEOPMETHOD_MOVE_IMMEDIATE:
			{
				ERROR3IF(pGuideline == NULL,"pGuideline is NULL");

				pSpread = Document::GetSelectedSpread();

				if (pGuideline != NULL && !DoTranslateGuideline(pGuideline,pGuidelineParam->NewOrdinate))
					FailAndExecute();
				else
					BroadcastGuidelineChanges(pSpread->FindFirstGuideLayer());

				End();
			}
			UndoIDS = _R(IDS_OPMOVEGUIDELINE);
			break;

			//-----------------------------------
			case GUIDELINEOPMETHOD_NEW_DRAG:
			{
				DocCoord PointerPos(-5000,-5000);
				DoAddNewGuideline(NULL,PointerPos,pGuidelineParam->Type);
			}
			UndoIDS = _R(IDS_OPNEWGUIDELINE);
			break;

			//-----------------------------------
			case GUIDELINEOPMETHOD_NEW_IMMEDIATE:
			{
				pSpread = Document::GetSelectedSpread();

				if (!DoNewGuideline(NULL,NEXT,pGuidelineParam->Type,pGuidelineParam->NewOrdinate))
					FailAndExecute();
				else
					BroadcastGuidelineChanges(pSpread->FindFirstGuideLayer());

				End();
			}
			UndoIDS = _R(IDS_OPNEWGUIDELINE);
			break;

			//-----------------------------------
			case GUIDELINEOPMETHOD_CREATE_GUIDE_LAYER:
			{
				pSpread = Document::GetSelectedSpread();
				Layer* pGuideLayer = DoCreateGuideLayer();

				if (pGuideLayer == NULL)
					FailAndExecute();
				else
					BroadcastGuidelineChanges(pGuideLayer);

				End();
			}
			UndoIDS = _R(IDS_OPCREATEGUIDELAYER);
			break;

			//-----------------------------------
			case GUIDELINEOPMETHOD_DELETE:
			{
				pSpread = Document::GetSelectedSpread();

				if (!DoDeleteListOfGuidelines(pGuidelineParam->pGuidelineList))
					FailAndExecute();
				else
					BroadcastGuidelineChanges(pSpread->FindFirstGuideLayer());

				End();

			}
			UndoIDS = _R(IDS_OPDELETESELECTEDGUIDELINES);		// ********* NEW RESOURCE STRING!!!!!!
			break;
			//-----------------------------------
			default:
				ERROR3_PF(("Unknown OpGuideline method : %d",pGuidelineParam->Method));
				break;
		}
	}
}

/***********************************************************************************************

>	void OpGuideline::DoAddNewGuideline(Spread* pSpread,DocCoord PointerPos,GuidelineType TheType)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		pThisSpread 	  = ptr to spread drag started in
				PointerPos	 	  = coord of point clicked
				TheType			  = The type of guideline
	Outputs:	-
	Returns:	-
	Purpose:    Starts a drag to add a new guideline to the document
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpGuideline::DoAddNewGuideline(Spread* pSpread,DocCoord PointerPos,GuidelineType TheType)
{
	Type = TheType;

	DoDrag(pSpread,PointerPos);
}

/***********************************************************************************************

>	void OpGuideline::DoMoveGuideline(Spread* pSpread,DocCoord PointerPos,NodeGuideline* pGuideline)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		pThisSpread 	  = ptr to spread drag started in
				PointerPos	 	  = coord of point clicked
				pGuideline 		  = the guideline to move
	Outputs:	-
	Returns:	-
	Purpose:    Starts a drag to add a new guideline to the document
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpGuideline::DoMoveGuideline(Spread* pSpread,DocCoord PointerPos,NodeGuideline* pGuideline)
{
	pDraggedGuideline = pGuideline;
	Type 			  = pDraggedGuideline->GetType();

	DoDrag(pSpread,PointerPos);
}


/***********************************************************************************************

>	void OpGuideline::DoDrag(Spread* pThisSpread,DocCoord PointerPos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		pThisSpread 	  = ptr to spread drag started in (NULL means get selected spread)
				PointerPos	 	  = coord of point clicked
	Outputs:	-
	Returns:	-
	Purpose:    Starts a drag for a new or existing guideline
	SeeAlso:	DoMoveGuideline(), DoAddNewGuideline()
		
***********************************************************************************************/

void OpGuideline::DoDrag(Spread* pThisSpread,DocCoord PointerPos)
{
	pSpread = pThisSpread;

	if (pSpread == NULL)
		pSpread = Document::GetSelectedSpread();

	ERROR3IF(pSpread == NULL,"pSpread == NULL");
	if (pSpread == NULL)
	{
		End();
		return;
	}

	SetOrdinate(PointerPos);

	RenderMyDragBlobs();

PORTNOTE("keypress", "Use of KeyPress class bypassed in TransOperation::DragStarted");
#ifndef EXCLUDE_FROM_XARALX
	LeaveCopy = KeyPress::IsKeyPressed(CAMKEY(ADD));
#else
	LeaveCopy = FALSE;
#endif

	if (pCursor == NULL)
	{
		ToolListItem* pItem = Tool::Find(TOOLID_SELECTOR);

		if (pItem != NULL)
		{
			Tool* pTool = pItem->m_pTool;

			if (pTool != NULL)
			{
				if (Type == GUIDELINE_HORZ)
					pCursor	= new Cursor(pTool,_R(IDCSR_SEL_HGUIDE));
				else
					pCursor	= new Cursor(pTool,_R(IDCSR_SEL_VGUIDE));

				if (pCursor != NULL)
					CursorStackID = CursorStack::GPush(pCursor);
			}
		}
	}

	UpdateStatusLineAndPointer();

	// Tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_DEFERSCROLL );
}
	
/***********************************************************************************************

>	virtual void OpGuideline::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		PointerPos = coord of the pointer
				ClickMods  = info on the click modifiers
				pSpread    = ptr to spread 
	Outputs:	-
	Returns:	-
	Purpose:    Responds to a mouse move during a drag of a guideline
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpGuideline::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag)
{
	// First Rub out the old box
	RenderMyDragBlobs();

	DocView::SnapCurrent(pSpread,&PointerPos);

	SetOrdinate(PointerPos);

	// Render the new drag box
	RenderMyDragBlobs();

	UpdateStatusLineAndPointer();
}


/***********************************************************************************************

>	virtual void OpGuideline::DragFinished(	DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success , BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		PointerPos = coord of the pointer
				ClickMods  = info on the click modifiers
				pSpread    = ptr to spread (not used)
				Success	   = TRUE if drag ended successfully, FALSE if drag terminated (usu. by pressing Escape)
	Outputs:	-
	Returns:	-
	Purpose:    Responds to the drag of a guideline ending
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpGuideline::DragFinished(	DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag)
{
	// First Rub out the old boxes
	RenderMyDragBlobs();

	if (Success)
	{
		DocView::SnapCurrent(pSpread,&PointerPos);
		SetOrdinate(PointerPos);

		if (pDraggedGuideline != NULL)
		{
			if (IsMouseOverRuler())
			{
				UndoIDS = _R(IDS_OPDELETEGUIDELINE);
				Success = DoDeleteGuideline(pDraggedGuideline);
			}
			else
				Success = DoTranslateGuideline(pDraggedGuideline,Ordinate);
		}
		else
			Success = !IsMouseOverRuler() && DoNewGuideline(NULL,NEXT,Type,Ordinate);
	}

	// End the Drag
	EndDrag();

	// Restore cursor
	if (CursorStackID != GUIDE_CURSORID_UNSET)
	{
		CursorStack::GPop(CursorStackID);
		CursorStackID = GUIDE_CURSORID_UNSET;
	}

	if (pCursor != NULL)
	{
		delete pCursor;
		pCursor = NULL;
	}

	if (!Success)
		FailAndExecute();
	else
		BroadcastGuidelineChanges(pSpread->FindFirstGuideLayer());
	// End the op
	End();
}

/***********************************************************************************************

>	void OpGuideline::UpdateStatusLineAndPointer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Makes sure the status line AND pointer are correct during a drag
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpGuideline::UpdateStatusLineAndPointer()
{
//	if (pCursor != NULL && CursorStackID != GUIDE_CURSORID_UNSET)
//		CursorStack::GSetTop(pCursor,CursorStackID);

	// Which status help should we display?
	UINT32 IDS = _R(IDS_DRAGDELETEGUIDE);

	if (!IsMouseOverRuler())
	{
		if (CanLeaveCopy())
		{
			if (Type == GUIDELINE_HORZ)
				IDS = _R(IDS_DRAGCOPYHORZGUIDE);
			else
				IDS = _R(IDS_DRAGCOPYVERTGUIDE);
		}
		else
		{
			if (Type == GUIDELINE_HORZ)
				IDS = _R(IDS_SELHORZGUIDE);
			else
				IDS = _R(IDS_SELVERTGUIDE);
		}
	}

	if (IDS != CurrentStatusHelp)
	{
		// put up some status line help

		CurrentStatusHelp = IDS;
		String_256 Str(IDS);

		StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
		if (pStatusLine!=NULL)
			pStatusLine->UpdateText(&Str,STATUSLINE_SELDESC_STATBAR);
	}
}

/***********************************************************************************************

>	virtual BOOL OpGuideline::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pKeyPress = ptr to keypress object
	Outputs:	-
	Returns:	-
	Purpose:    Responds to a keypress during the drag
	SeeAlso:	DragFinished
		
***********************************************************************************************/

BOOL OpGuideline::DragKeyPress(KeyPress* pKeyPress, BOOL bSolidDrag)
{
	BOOL Processed = FALSE;
PORTNOTE("keypress", "Use of KeyPress class bypassed in TransOperation::DragStarted");
#ifndef EXCLUDE_FROM_XARALX
	switch (pKeyPress->GetVirtKey())
	{
		case CAMKEY(ADD):
			if (!pKeyPress->IsModified() && pKeyPress->IsPress() && !pKeyPress->IsRepeat())
			{
				LeaveCopy = !LeaveCopy;
				UpdateStatusLineAndPointer();
				Processed = TRUE;
			}
			break;

		case CAMKEY(CONTROL):
			UpdateStatusLineAndPointer();
			Processed = TRUE;
			break;
	}
#endif
	return Processed;
}

/***********************************************************************************************

>	static BOOL OpGuideline::IsMouseOverRuler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if mouse is over a ruler, FALSE otherwise
	Purpose:    Test to see where the mouse pointer is.
				It will return TRUE if the mouse is over either ruler, or the origin gadget.

	SeeAlso:	OpGuideline::DragFinished()
		
***********************************************************************************************/

BOOL OpGuideline::IsMouseOverRuler()
{
	DocView* pDocView = DocView::GetSelected();
	if (pDocView != NULL)
	{
		CCamView* pCCamView = pDocView->GetConnectionToOilView();
		return (pCCamView->IsMouseOverRuler() != OVER_NO_RULERS);
	}

	return FALSE;
}

/***********************************************************************************************

>	BOOL OpGuideline::DoTranslateGuideline(NodeGuideline* pGuideline,MILLIPOINT Ordinate,NodeGuideline** ppNewGuideline = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pGuideline = ptr to an existing guideline
				Ordinate   = Absolute position to translate to
				ppNewGuideline 	= place to put ptr of translated guideline (Can be NULL)

	Outputs:	*ppNewGuideline = ptr to translated guideline (NULL if function fails)
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:    Translates the given guideline to line up with Ordinate, undoably
	SeeAlso:	OpGuideline::DragFinished()
		
***********************************************************************************************/

BOOL OpGuideline::DoTranslateGuideline(NodeGuideline* pGuideline,MILLIPOINT Ordinate,NodeGuideline** ppNewGuideline)
{
	ERROR2IF(pGuideline == NULL,FALSE,"pGuideline is NULL");

	Type = pGuideline->GetType();

	BOOL ok = TRUE;

	if (ok) ok = DoNewGuideline(pGuideline,NEXT,Type,Ordinate,ppNewGuideline);
	if (ok) ok = DoDeleteGuideline(pGuideline,TRUE);

	return ok;
}

/***********************************************************************************************

>	BOOL OpGuideline::DoNewGuideline(Node* pContext,AttachNodeDirection AttachDir,GuidelineType Type,MILLIPOINT Ordinate,NodeGuideline** ppNewGuideline = NULL)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pContext   		= ptr to context node (Can be NULL)
				AttachDir  		= how the new guideline should be attached to the context node
				Type			= type of guideline to create
				Ordinate		= position of new guideline
				ppNewGuideline 	= place to put ptr of new guideline (Can be NULL)

	Outputs:	*ppNewGuideline = ptr to new guideline (NULL if function fails)
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:    Creates a new guideline to line up with Ordinate, undoably

	SeeAlso:	OpGuideline::DragFinished()
		
***********************************************************************************************/

BOOL OpGuideline::DoNewGuideline(Node* pContext,AttachNodeDirection AttachDir,GuidelineType Type,MILLIPOINT Ordinate,NodeGuideline** ppNewGuideline)
{
	if (pContext == NULL)
	{
		pContext = DoCreateGuideLayer();
		AttachDir = LASTCHILD;
	}

	ERROR2IF(pContext == NULL,FALSE,"pContext is NULL");

	BOOL ok = TRUE;

	NodeGuideline* pNewGuideline = new NodeGuideline;
	ok = (pNewGuideline != NULL);
	if (ok)
	{
		pNewGuideline->SetType(Type);
		pNewGuideline->SetOrdinate(Ordinate);

		DocRect Rect = pNewGuideline->GetRenderRect(FALSE);

		if (ok) ok = DoInsertNewNode(pNewGuideline,pContext,AttachDir,FALSE,FALSE,FALSE,FALSE);
		if (ok) ok = DoInvalidateRegion(pSpread,Rect);
	}

	if (!ok && pNewGuideline != NULL)
	{
		delete pNewGuideline;
		pNewGuideline = NULL;
	}

	if (ppNewGuideline != NULL)
		*ppNewGuideline = pNewGuideline;

	return ok;
}

/***********************************************************************************************

>	BOOL OpGuideline::DoDeleteGuideline(NodeGuideline* pGuideline,BOOL TryToLeaveCopy = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pGuideline = ptr to an existing guideline
				TryToLeaveCopy = TRUE to try and leave copy, FALSE means force delete
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:    Deletes the given guideline, undoably
	SeeAlso:	OpGuideline::DragFinished()
		
***********************************************************************************************/

BOOL OpGuideline::DoDeleteGuideline(NodeGuideline* pGuideline,BOOL TryToLeaveCopy)
{
	BOOL ok = TRUE;

	if (TryToLeaveCopy && CanLeaveCopy())
	{
		// User is trying to copy the guideline, so just set up the correct undo string
		UndoIDS = _R(IDS_OPCOPYGUIDELINE);
	}
	else
	{
		ERROR2IF(pGuideline == NULL,FALSE,"pGuideline is NULL");

		pSpread = pGuideline->FindParentSpread();
		ERROR2IF_PF(pSpread == NULL,FALSE,("pGuideline (0x%x) has no parent spread",pGuideline));

		DocRect Rect = pGuideline->GetRenderRect(FALSE);

		if (ok) ok = DoInvalidateRegion(pSpread,Rect);
		if (ok)	ok = DoHideNode(pGuideline,TRUE);
	}

	return ok;
}


/***********************************************************************************************

>	BOOL OpGuideline::CanLeaveCopy()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if user wants to leave a copy behind, FALSE otherwise
	Purpose:    Sees if the user wants to leave a copy behind

				TRUE is returned if the ctrl is pressed, or Numpad + has been pressed an odd number
				of times during the drag

	SeeAlso:	OpGuideline::DragFinished()
		
***********************************************************************************************/

BOOL OpGuideline::CanLeaveCopy()
{
	return (LeaveCopy || KeyPress::IsConstrainPressed());
}

/***********************************************************************************************

>	BOOL OpGuideline::DoDeleteListOfGuidelines(NodeGuideline** pGuidelineList)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		pGuidelineList = ptr to a list of existing guidelines
	Outputs:	-
	Returns:	TRUE if successful, FALSE otherwise
	Purpose:    Deletes the guidelines in the given list, undoably
				The list should be terminated by a NULL value
	SeeAlso:	OpGuideline::DragFinished()
		
***********************************************************************************************/

BOOL OpGuideline::DoDeleteListOfGuidelines(NodeGuideline** pGuidelineList)
{
	ERROR2IF(pGuidelineList == NULL,FALSE,"pGuidelineList is NULL");

	BOOL ok= TRUE;

	while (*pGuidelineList != NULL && ok)
	{
		ok = DoDeleteGuideline(*pGuidelineList);
		pGuidelineList++;
	}

	return ok;
}

/***********************************************************************************************

>	Layer* OpGuideline::DoCreateGuideLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to the guide layer, or NULL one couldn't be created.
	Purpose:    This returns a ptr to the guide layer.

				If the guide layer already exists, a ptr to it is returned.

				If there isn't a guide layer, it is created and inserted into the doc tree.
				If this fails the func returns NULL

	SeeAlso:	OpGuideline::DoNewGuideline()
		
***********************************************************************************************/

Layer* OpGuideline::DoCreateGuideLayer()
{
	Layer* pLayer = pSpread->FindFirstGuideLayer();
	if (pLayer != NULL)
		return pLayer;

	Layer* pNewLayer = Layer::CreateGuideLayer();
	if (pNewLayer != NULL)
	{
		// If we insert the new layer as the FIRSTCHILD then this will be behind the page.
		// If we insert as the last child then we will be at the front of
		// the stacking order. We need to insert ourselves as the node after the last page node
		// This used to take this approach before 4/4/97.
//		Page *pLastPage = pSpread->FindLastPageInSpread();
		Layer *pLastLayer = pSpread->FindLastLayer();
		// If there is the sepcial background layer present then ensure that we insert after that
//		Layer * pLayer = pSpread->FindFirstPageBackgroundLayer();
//		Node * pInsertionPoint = pLastPage;
		Node * pInsertionPoint = pLastLayer;
//		if (pLayer)
//			pInsertionPoint = pLayer;

		// Insert the new layer as the next node after the last page.
		if (pLastLayer && DoInsertNewNode(pNewLayer,pInsertionPoint,NEXT,FALSE,FALSE,FALSE,FALSE))
		{
			pNewLayer->EnsureUniqueLayerID();
			BROADCAST_TO_ALL(SpreadMsg(pSpread,SpreadMsg::LAYERCHANGES));
		}
		else
		{
			delete pNewLayer;
			pNewLayer = NULL;
		}
	}

	return pNewLayer;
}



/***********************************************************************************************

>	static Layer* OpGuideline::CreateGuideLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to a guide layer that's ready to be inserted into the tree
	Purpose:    This does everything required in creating a layer fit to be called the guide layer
	SeeAlso:	-
		
***********************************************************************************************/

// WEBSTER - markn 15/1/97
// Moved this static function into the Layer class, where it should be
/*
Layer* OpGuideline::CreateGuideLayer()
{
	Layer* pNewLayer = new Layer;
	if (pNewLayer != NULL)
	{
		String_256 LayerName(_R(IDS_GUIDESLAYERNAME));

		pNewLayer->SetVisible(TRUE); 
		pNewLayer->SetLocked(FALSE); 
		pNewLayer->SetPrintable(FALSE); 
		pNewLayer->SetBackground(TRUE); 
		pNewLayer->SetOutline(FALSE);
		pNewLayer->SetGuide(TRUE);
		pNewLayer->SetLayerID(LayerName); 

		Quality Qual;
		Qual.SetQuality(-1);
		QualityAttribute QualAttr(Qual);
		NodeAttribute* pNodeAttr = QualAttr.MakeNode();
		pNodeAttr->AttachNode(pNewLayer,FIRSTCHILD);
	}

	return pNewLayer;
}
*/

/***********************************************************************************************

>	void OpGuideline::SetOrdinate(DocCoord& PointerPos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/9/95
	Inputs:		PointerPos = pos of mouse pointer
	Outputs:	-
	Returns:	-
	Purpose:    Sets the Ordinate member var to either the X or the Y value of PointerPos
				depending on the guideline type 
	SeeAlso:	-
		
***********************************************************************************************/

void OpGuideline::SetOrdinate(DocCoord& PointerPos)
{
	if (Type == GUIDELINE_HORZ)
		Ordinate = PointerPos.y;
	else
		Ordinate = PointerPos.x;
}

/***********************************************************************************************

>	void OpGuideline::RenderMyDragBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Renders the blobs associated with the current drag
	SeeAlso:	RenderDragBlobs()
		
***********************************************************************************************/

void OpGuideline::RenderMyDragBlobs()
{
	DocRect Rect;
	RenderDragBlobs(Rect,pSpread, FALSE);
	RenderOn = !RenderOn;
}
	
/***********************************************************************************************

>	void OpGuideline::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		Rect 	= region of doc being updated (NOT USED!)
				pSpread = ptr to the spread being redrawn
	Outputs:	-
	Returns:	-
	Purpose:    Main routine for rendering the blobs associated with the current drag.
				This can be called from anywhere, e.g. when the view scrolls
	SeeAlso:	-
		
***********************************************************************************************/

void OpGuideline::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
	static DocRect RenderRect;

	RenderRegion* pRender = DocView::RenderOnTop( NULL, pSpread, UnclippedEOR );

	while (pRender != NULL)
	{
		// Set line colour for dashed line
		pRender->SetLineColour(COLOUR_BLACK);

		DocCoord StartPoint(Ordinate,Ordinate);
		DocCoord EndPoint(Ordinate,Ordinate);

		if (RenderOn)
		{
			View* pView = pRender->GetRenderView();
			if (pView != NULL)
			{
				RenderRect = pView->GetDocViewRect(pSpread);
				RenderRect = RenderRect.ToSpread(pSpread,pView);
			}
			RenderRect.Inflate(pRender->GetScaledPixelWidth());
		}

		if (Type == GUIDELINE_HORZ)
		{
			StartPoint.x = RenderRect.lo.x;
			EndPoint.x   = RenderRect.hi.x;
		}
		else
		{
			StartPoint.y = RenderRect.lo.y;
			EndPoint.y   = RenderRect.hi.y;
		}

		pRender->DrawDashLine(StartPoint,EndPoint);

		// Get the Next render region
		pRender = DocView::GetNextOnTop(NULL);
	}	
}
		
		
/***********************************************************************************************

>	virtual BOOL OpGuideline::Undo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successfull, FALSE otherwise
	Purpose:    Called when the op is undone.
				This broadcasts a 'Guidelines changed' layer message
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpGuideline::Undo()
{
	if (UndoableOperation::Undo())
	{
		BroadcastGuidelineChanges(pBroadcastLayer);
		return TRUE;
	}

	return FALSE;
}

/***********************************************************************************************

>	virtual BOOL OpGuideline::Redo()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successfull, FALSE otherwise
	Purpose:    Called when the op is redone.
				This broadcasts a 'Guidelines changed' layer message
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpGuideline::Redo()
{
	if (UndoableOperation::Redo())
	{
		BroadcastGuidelineChanges(pBroadcastLayer);
		return TRUE;
	}

	return FALSE;
}

/***********************************************************************************************

>	static BOOL OpGuideline::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if op successfully declared, FALSE otherwise
	Purpose:    Registers an op desc for this op
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpGuideline::Init()
{
	return (GuidelinePropDlg::Init() 	&& 
			OpNewGuideline::Init() 		&&
			OpSpreadOrigin::Init() 		&& 
			OpResetSpreadOrigin::Init() &&
#ifndef STANDALONE
			OpChangeLayerColour::Init() &&
#endif
			RegisterOpDescriptor(
								0, 
								0,
								CC_RUNTIME_CLASS(OpGuideline), 
								OPTOKEN_GUIDELINE ,
								OpGuideline::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								) &&

			RegisterOpDescriptor(
								0, 
								_R(IDS_DELETEALLGUIDELINES),
								CC_RUNTIME_CLASS(OpDeleteAllGuidelines), 
								OPTOKEN_DELETEALLGUIDELINES ,
								OpDeleteAllGuidelines::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								) &&

			RegisterOpDescriptor(
								0, 
								_R(IDS_DELETEGUIDELINE),
								CC_RUNTIME_CLASS(OpDeleteGuideline), 
								OPTOKEN_DELETEGUIDELINE ,
								OpDeleteGuideline::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								)								
			);
}

/***********************************************************************************************

>	static OpState OpGuideline::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/

OpState OpGuideline::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	return State;
}

/***********************************************************************************************

>	static void OpGuideline::GetOpName(String_256* OpName)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/9/95
	Inputs:		OpName = ptr to string
	Outputs:	OpName contains a description of the op
	Returns:	-
	Purpose:    Returns a desc of the op.  Mainly used to fill in the menu item for the undo/redo ops. 
	SeeAlso:	-
		
***********************************************************************************************/

void OpGuideline::GetOpName(String_256* OpName)
{
	*OpName = String_256(UndoIDS);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------

/***********************************************************************************************

>	OpDeleteGuideline::OpDeleteGuideline()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default constructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpDeleteGuideline::OpDeleteGuideline()
{
	UndoIDS = _R(IDS_OPDELETEGUIDELINE);
}

/***********************************************************************************************

>	OpDeleteGuideline::~OpDeleteGuideline()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default destructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpDeleteGuideline::~OpDeleteGuideline()
{
}

/********************************************************************************************

>	void OpDeleteGuideline::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeleteGuideline::Do(OpDescriptor*)
{
	BOOL ok = FALSE;

	Spread* pSpread = Document::GetSelectedSpread();

	if (pGuideline != NULL)
		ok = DoDeleteGuideline(pGuideline);

	if (!ok)
		FailAndExecute();
	else
		BroadcastGuidelineChanges(pSpread->FindFirstGuideLayer());

	pGuideline = NULL;

	End();
}

/***********************************************************************************************

>	static OpState OpDeleteGuideline::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/

OpState OpDeleteGuideline::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;

	State.Greyed = (pGuideline == NULL);

	return State;
}

///////////////////////////////////////////////////////////////////////////////
// OpNewGuideline

/******************************************************************************
>	OpNewGuideline::OpNewGuideline()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Purpose:    Default constructor
******************************************************************************/

OpNewGuideline::OpNewGuideline()
{
	UndoIDS = _R(IDS_OPNEWGUIDELINE2UNDO);
}


/******************************************************************************
>	OpNewGuideline::~OpNewGuideline()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Purpose:    Default destructor
******************************************************************************/

OpNewGuideline::~OpNewGuideline()
{
}


/******************************************************************************
>	BOOL OpResetSpreadOrigin::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Returns:	FALSE if fails
	Purpose:    Initilise OpNewGuideline
******************************************************************************/

BOOL OpNewGuideline::Init()
{
	return  RegisterOpDescriptor(
								0, 
								_R(IDS_OPNEWGUIDELINE2),
								CC_RUNTIME_CLASS(OpNewGuideline), 
								OPTOKEN_NEWGUIDELINE2 ,
								OpNewGuideline::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								);
}


/*****************************************************************************
>	void OpNewGuideline::Do(OpDescriptor* pOpDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pOpDesc - 
	Purpose:	sets up param block and invokes OpGuideline::DoWithParams() 
******************************************************************************/

void OpNewGuideline::Do(OpDescriptor* pOpDesc)
{
	Spread* pSpread = Document::GetSelectedSpread();
	if (pOpDesc!=NULL && pSpread!=NULL && NewGuidelineParam.Method==GUIDELINEOPMETHOD_NEW_IMMEDIATE)
	{
//		GuidelineType Type     = NewGuidelineParam.Type;
		DocCoord      GuidePos = DocCoord(NewGuidelineParam.NewOrdinate, NewGuidelineParam.NewOrdinate);
		DoWithParam(pOpDesc,&NewGuidelineParam);

		NewGuidelineParam.Method = GUIDELINEOPMETHOD_INVALID;	// reset static param block
	}
	else
	{
		ERROR3("OpNewGuideline::Do() - invalid Method or pOpDesc==NULL or no selected spread");
		FailAndExecute();
		End();
	}
}


/******************************************************************************
>	static void OpNewGuideline::SetNewGuidelineParam(GuidelineType Type, MILLIPOINT pos)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		Type - type of guide (hoiz/vert)
				Pos  - positin of guide
******************************************************************************/

void OpNewGuideline::SetNewGuidelineParam(GuidelineType Type, MILLIPOINT pos)
{
	NewGuidelineParam.Method      = GUIDELINEOPMETHOD_NEW_IMMEDIATE;
	NewGuidelineParam.Type        = Type;
	NewGuidelineParam.NewOrdinate = pos;
}


/******************************************************************************
>	static OpState OpNewGuideline::GetState(String_256* pReasonGreyed, OpDescriptor* pOpDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pOpDesc       - 
	Outputs:	pReasonGreyed - 
	Returns:	An OpState object
******************************************************************************/

OpState OpNewGuideline::GetState(String_256* pReasonGreyed, OpDescriptor* pOpDesc)
{
	return OpState();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------

/***********************************************************************************************

>	OpDeleteAllGuidelines::OpDeleteAllGuidelines()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default constructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpDeleteAllGuidelines::OpDeleteAllGuidelines()
{
	UndoIDS = _R(IDS_OPDELETEALLGUIDELINES);
}

/***********************************************************************************************

>	OpDeleteAllGuidelines::~OpDeleteAllGuidelines()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Default destructor
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

OpDeleteAllGuidelines::~OpDeleteAllGuidelines()
{
}

/********************************************************************************************

>	void OpDeleteAllGuidelines::Do(OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Creates then opens the dialog 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void OpDeleteAllGuidelines::Do(OpDescriptor*)
{
	BOOL ok = FALSE;

	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
	{
		Layer* pLayer = pSpread->FindFirstGuideLayer();
		if (pLayer != NULL)
		{
			ok = TRUE;
			Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
			while (pNode != NULL)
			{
				Node* pNextNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeGuideline));
				ok = ok && DoDeleteGuideline((NodeGuideline*)pNode);
				pNode = pNextNode;
			}
		}
	}

	if (!ok)
		FailAndExecute();
	else
		BroadcastGuidelineChanges(pSpread->FindFirstGuideLayer());

	End();
}

/***********************************************************************************************

>	static OpState OpDeleteAllGuidelines::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/

OpState OpDeleteAllGuidelines::GetState(String_256* Description, OpDescriptor*)
{
	OpState State(FALSE,TRUE);		// Greyed out by default

	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread != NULL)
	{
		Layer* pLayer = pSpread->FindFirstGuideLayer();
		if (pLayer != NULL)
		{
			Node* pNode = pLayer->FindFirstChild(CC_RUNTIME_CLASS(NodeGuideline));
			State.Greyed = (pNode == NULL);		// Ungrey if there are one or more guidelines
		}
	}

	if (State.Greyed)
		Description->Load(_R(IDS_OPDELETEALLGUIDESGREY));

	return State;
}

///////////////////////////////////////////////////////////////////////////////
// GuidelinePropDlg - or should that be OpGuidelinePropDlg!

/********************************************************************************************
>	GuidelinePropDlg::GuidelinePropDlg(): DialogOp(GuidelinePropDlg::IDD, GuidelinePropDlg::Mode) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Purpose:	GuidelinePropDlg constructor. Creates a non-undoable operation
********************************************************************************************/

GuidelinePropDlg::GuidelinePropDlg(): DialogOp(GuidelinePropDlg::IDD, GuidelinePropDlg::Mode) 
{   
}        


/********************************************************************************************
>	MsgResult GuidelinePropDlg::Message(Msg* pMsg)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Inputs:		pMsg -
	Returns:	a message result
	Purpose:	Handles all the scale dialog's messages 
********************************************************************************************/

MsgResult GuidelinePropDlg::Message(Msg* pMsg)
{
	if (IS_OUR_DIALOG_MSG(pMsg))
	{
		DialogMsg* Msg = (DialogMsg*)pMsg;
		BOOL EndDialog = FALSE;

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
				ShowDetails();
				SetKeyboardFocus (_R(IDC_GUIDELINEPROP_POSITION)) ;
				HighlightText    (_R(IDC_GUIDELINEPROP_POSITION)) ;
				break;
				
			case DIM_COMMIT:
				EndDialog = CommitValues();
				if (!EndDialog)
					Msg->DlgMsg = DIM_NONE;  // stop default handler from closing dialog
				break;

			case DIM_SOFT_COMMIT:
				break; 

			case DIM_CANCEL:
				EndDialog = TRUE;
				break;

		    case DIM_LFT_BN_CLICKED:
				break; 

			case DIM_TEXT_CHANGED:
				break;

			default:
				break;
		}

		if (EndDialog)	// Dialog communication over 
		{			   // Isn't this pretty?
			Close();  // Close the dialog 
			End(); 	 // Destroy dialog 
			return OK;
	   	}
	}

	// Pass everything on to the base class . . .
	return DialogOp::Message(pMsg);
}  


/********************************************************************************************
>	OpState	GuidelinePropDlg::GetState(String_256* pReasonGreyed, OpDescriptor* pOpDesc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Inputs:		pOpDesc       -
	Outputs:	pReasonGreyed - 
	Returns:	op state
	Purpose:	GuidelinePropDlg GetState method
********************************************************************************************/

OpState	GuidelinePropDlg::GetState(String_256* pReasonGreyed, OpDescriptor* pOpDesc)
{    
	OpState OpSt;
	return OpSt;
}

		 
/********************************************************************************************
>	BOOL GuidelinePropDlg::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Returns:	FALSE if fails
	Purpose:	GuidelinePropDlg Init method
********************************************************************************************/

BOOL GuidelinePropDlg::Init()
{  
	return RegisterOpDescriptor(0,
								_R(IDS_OPEDITGUIDELINEPROPDLG),
								CC_RUNTIME_CLASS(GuidelinePropDlg),
								OPTOKEN_EDITGUIDELINEPROPDLG,
								GuidelinePropDlg::GetState,
								0,	// help ID
								0,  // bubble ID
								0	// bitmap ID
								)
		&& RegisterOpDescriptor(0,
								_R(IDS_OPNEWGUIDELINEPROPDLG),
								CC_RUNTIME_CLASS(GuidelinePropDlg),
								OPTOKEN_NEWGUIDELINEPROPDLG,
								GuidelinePropDlg::GetState,
								0,	// help ID
								0,  // bubble ID
								0	// bitmap ID
								);
}   

 
/********************************************************************************************
>	void GuidelinePropDlg::Do(OpDescriptor* pOpDesc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Inputs:		pOpDesc - 
	Purpose:	Creates then opens the dialog 
********************************************************************************************/

void GuidelinePropDlg::Do(OpDescriptor* pOpDesc)
{
	if (Create()==FALSE)
	{
		InformError(0,_R(IDS_OK));
  		End();
	}
}


/********************************************************************************************
>	void GuidelinePropDlg::ShowDetails()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Purpose:	Shows guideline details in the dialog
********************************************************************************************/

void GuidelinePropDlg::ShowDetails()
{
	MILLIPOINT    Ordinate;
	GuidelineType Type;
	Spread*       pSpread;
	String_256    TitleStr("");
	if (Params.pGuideline!=NULL)
	{
		pSpread  = Params.pGuideline->FindParentSpread();
		Ordinate = Params.pGuideline->GetOrdinate();
		Type 	 = Params.pGuideline->GetType();
		TitleStr = String_256(_R(IDS_OPEDITGUIDELINEPROPDLGTITLE));
	}
	else
	{
		pSpread  = Document::GetSelectedSpread();
		Ordinate = Params.Ordinate;
		Type 	 = Params.Type;
		TitleStr = String_256(_R(IDS_OPNEWGUIDELINEPROPDLGTITLE));
	}

	SetTitlebarName(&TitleStr);

	Ordinate = NodeGuideline::ToUserOrdinate(pSpread,Ordinate,Type);
	SetDimensionGadgetValue(_R(IDC_GUIDELINEPROP_POSITION),Ordinate,pSpread);

	UINT32 IDS = (Type==GUIDELINE_HORZ) ? _R(IDS_HORIZONTAL) : _R(IDS_VERTICAL);
	String_256 Str(IDS);
	SetStringGadgetValue(_R(IDC_GUIDELINEPROP_TYPE),IDS);
}


/********************************************************************************************
>	BOOL GuidelinePropDlg::CommitValues()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/95
	Returns:	TRUE if values excepted, FALSE otherwise
	Purpose:	Called when the OK button is selected
********************************************************************************************/

BOOL GuidelinePropDlg::CommitValues()
{
	OpGuidelineParam GuidelineParam;

	BOOL ok;
	MILLIPOINT Ordinate = GetDimensionGadgetValue(_R(IDC_GUIDELINEPROP_POSITION),Document::GetSelectedSpread(),&ok);

	if (!ok)
		return FALSE;

	if (Params.pGuideline != NULL)
	{
		GuidelineParam.Method      = GUIDELINEOPMETHOD_MOVE_IMMEDIATE;
		GuidelineParam.pGuideline  = Params.pGuideline;
		GuidelineParam.NewOrdinate = NodeGuideline::ToSpreadOrdinate(Params.pGuideline->FindParentSpread(),Ordinate,Params.pGuideline->GetType());
	}
	else
	{
		GuidelineParam.Method      = GUIDELINEOPMETHOD_NEW_IMMEDIATE;
		GuidelineParam.Type        = Params.Type;
		GuidelineParam.NewOrdinate = NodeGuideline::ToSpreadOrdinate(Document::GetSelectedSpread(),Ordinate,Params.Type);
	}

	OpDescriptor* pOpDesc = OpDescriptor::FindOpDescriptor(OPTOKEN_GUIDELINE);
	ERROR3IF(pOpDesc == NULL,"FindOpDescriptor(OPTOKEN_GUIDELINE) failed");

	if (pOpDesc != NULL)
		pOpDesc->Invoke(&GuidelineParam);

	return TRUE;
}


/******************************************************************************
>	static void GuidelinePropDlg::SetNewGuidelineParams(GuidelineType type, MILLIPOINT pos)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		Type - type of guide (hoiz/vert)
				pos  - position of guide
	Purpose:	Set up param block for guideline property dialog
******************************************************************************/

void GuidelinePropDlg::SetNewGuidelineParams(GuidelineType type, MILLIPOINT pos)
{
	Params.pGuideline = NULL;
	Params.Type       = type;
	Params.Ordinate   = pos;
}


/******************************************************************************
>	static void GuidelinePropDlg::SetEditGuidelineParams(NodeGuideline* pGuide)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/95
	Inputs:		pGuide - guide to edit
	Purpose:	Set up param block for guideline property dialog
******************************************************************************/

void GuidelinePropDlg::SetEditGuidelineParams(NodeGuideline* pGuide)
{
	ERROR3IF(pGuide==NULL,"GuidelinePropDlg::SetEditGuidelineParam() - pGuide==NULL");
	Params.pGuideline = pGuide;
	Params.Type       = GUIDELINE_NOTYPE;
	Params.Ordinate   = 0;
}


///////////////////////////////////////////////////////////////////////////////
// OpSpreadOrigin

/********************************************************************************************

>	OpSpreadOrigin::OpSpreadOrigin()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Default constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpSpreadOrigin::OpSpreadOrigin()
{
	RenderOn = TRUE;
}

/********************************************************************************************

>	OpSpreadOrigin::~OpSpreadOrigin()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		- 
	Outputs:	-
	Returns:	-
	Purpose:	Default destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpSpreadOrigin::~OpSpreadOrigin()
{
}

/***********************************************************************************************

>	void OpSpreadOrigin::Do(OpDescriptor* pOpDesc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		pOpDesc = ptr op desc
	Outputs:	-
	Returns:	-
	Purpose:    This func starts a drag that will end up setting the spread origin
	SeeAlso:	DoAddNewGuideline(), OilRuler::OnLButtonDown()
		
***********************************************************************************************/

void OpSpreadOrigin::Do(OpDescriptor* pOpDesc)
{
	DocCoord StartPos(-5000,-5000);

	DoDrag(NULL,StartPos);
}

/***********************************************************************************************

>	void OpSpreadOrigin::DoDrag(Spread* pThisSpread,DocCoord PointerPos)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		pThisSpread 	  = ptr to spread drag started in (NULL means get selected spread)
				PointerPos	 	  = coord of point clicked
	Outputs:	-
	Returns:	-
	Purpose:    Starts a drag for the origin
	SeeAlso:	-
		
***********************************************************************************************/

void OpSpreadOrigin::DoDrag(Spread* pThisSpread,DocCoord PointerPos)
{
	pSpread = pThisSpread;

	if (pSpread == NULL)
		pSpread = Document::GetSelectedSpread();

	ERROR3IF(pSpread == NULL,"pSpread == NULL");
	if (pSpread == NULL)
	{
		End();
		return;
	}

	CurrentOrigin = PointerPos;

	RenderMyDragBlobs();

	// put up some status line help
	StatusLine* pStatusLine=GetApplication()->GetpStatusLine();
	if (pStatusLine!=NULL)
	{
		String_256 temp(_R(IDS_OPSPREADORIGINDRAG));
		pStatusLine->UpdateText(&temp,STATUSLINE_SELDESC_STATBAR);
	}
	
	// Tell the Dragging system that we need drags to happen
	StartDrag( DRAGTYPE_DEFERSCROLL );
}

/***********************************************************************************************

>	virtual void OpSpreadOrigin::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		PointerPos = coord of the pointer
				ClickMods  = info on the click modifiers
				pSpread    = ptr to spread 
	Outputs:	-
	Returns:	-
	Purpose:    Responds to a mouse move during a drag of the origin
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpSpreadOrigin::DragPointerMove( DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL bSolidDrag)
{
	// First Rub out the old box
	RenderMyDragBlobs();

	DocView::SnapCurrent(pSpread,&PointerPos);

	CurrentOrigin = PointerPos;

	// Render the new drag box
	RenderMyDragBlobs();
}


/***********************************************************************************************

>	virtual void OpSpreadOrigin::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		PointerPos = coord of the pointer
				ClickMods  = info on the click modifiers
				pSpread    = ptr to spread (not used)
				Success	   = TRUE if drag ended successfully, FALSE if drag terminated (usu. by pressing Escape)
	Outputs:	-
	Returns:	-
	Purpose:    Responds to the drag of a guideline ending
	SeeAlso:	UndoableOperation
		
***********************************************************************************************/

void OpSpreadOrigin::DragFinished(DocCoord PointerPos, ClickModifiers ClickMods, Spread*, BOOL Success, BOOL bSolidDrag)
{
	// First Rub out the old boxes
	RenderMyDragBlobs();

	Success = (Success && !OpGuideline::IsMouseOverRuler());

	if (Success)
	{
		DocView::SnapCurrent(pSpread,&PointerPos);
		CurrentOrigin = PointerPos;

		Success = DoChangeOrigin(pSpread,CurrentOrigin);
	}

	if (!Success)
		FailAndExecute();

	// End the op & Drag
	End();
	EndDrag();
}

/***********************************************************************************************
>	BOOL OpSpreadOrigin::DoChangeOrigin(Spread* pSpread, DocCoord NewOrigin)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Inputs:		pSpread    = ptr to spread
				NewOrigin  = new origin for the spread
	Returns:	FALSE if fails
	Purpose:    Undoably set a new grid/user origin
***********************************************************************************************/

BOOL OpSpreadOrigin::DoChangeOrigin(Spread* pSpread, DocCoord NewOrigin)
{
	ERROR2IF(pSpread==NULL,FALSE,"OpSpreadOrigin::DoChangeOrigin() - pSpread==NULL");

	NodeGrid* pDefaultGrid = pSpread->FindFirstDefaultGridInSpread();
	ERROR2IF(pDefaultGrid==NULL,FALSE,"OpSpreadOrigin::DoChangeOrigin() - FindFirstDefaultGrid() failed");

	// if no change in origin, just return without inserting an op
	MILLIPOINT x,y;
	pDefaultGrid->GetOrigin(&x,&y);		// call me stupid but couldn't this return a DocCoord!
	if (DocCoord(x,y)==NewOrigin)
		return TRUE;

	// cunningly (NOT) OpGridResize expects the origin to be page relative!
	DocCoord NewPageRelativeOrigin;
	pSpread->SpreadCoordToPagesCoord(&NewPageRelativeOrigin,NewOrigin);

	// Force the origin to snap to a pixel boundary.
	NewPageRelativeOrigin.x = ((NewPageRelativeOrigin.x + 375) / 750) * 750;
	NewPageRelativeOrigin.y = ((NewPageRelativeOrigin.y + 375) / 750) * 750;

	// set up a complete set of grid params with new origin
	// (would be nice to have them all default to 'no change' and only specify the new ones but that would be far to obvious!)
	GridResizeInfo GridParams;
	GridParams.pSpread          = pSpread;
	GridParams.OriginX          = NewPageRelativeOrigin.x;
	GridParams.OriginY          = NewPageRelativeOrigin.y;
	GridParams.GridDivisions    = pDefaultGrid->GetDivisions();
	GridParams.GridSubDivisions = pDefaultGrid->GetSubdivisions();
	GridParams.GridUnits        = pDefaultGrid->GetUnits();
 	GridParams.TypeOfGrid       = pDefaultGrid->GetGridType();
//	GridParams.GridSpacing	// this seems to be 'MainStep' which is also determined from Divisions and Units

	return DoResizeGrid(&GridParams);
}


/***********************************************************************************************

>	void OpSpreadOrigin::RenderMyDragBlobs()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:    Renders the blobs associated with the current drag
	SeeAlso:	RenderDragBlobs()
		
***********************************************************************************************/

void OpSpreadOrigin::RenderMyDragBlobs()
{
	DocRect Rect;
	RenderDragBlobs(Rect,pSpread, FALSE);
	RenderOn = !RenderOn;
}
	
/***********************************************************************************************

>	void OpSpreadOrigin::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		Rect 	= region of doc being updated (NOT USED!)
				pSpread = ptr to the spread being redrawn
	Outputs:	-
	Returns:	-
	Purpose:    Main routine for rendering the blobs associated with the current drag.
				This can be called from anywhere, e.g. when the view scrolls
	SeeAlso:	-
		
***********************************************************************************************/

void OpSpreadOrigin::RenderDragBlobs(DocRect Rect,Spread* pSpread, BOOL bSolidDrag)
{
	static DocRect RenderRect;

	RenderRegion* pRender = DocView::RenderOnTop( NULL, pSpread, UnclippedEOR );

	DocCoord Start;
	DocCoord End;

	while (pRender != NULL)
	{
		pRender->SetLineColour(COLOUR_BLACK);
		pRender->SetLineWidth(pRender->GetScaledPixelWidth());

		if (RenderOn)
		{
			View* pView = pRender->GetRenderView();
			if (pView != NULL)
			{
				RenderRect = pView->GetDocViewRect(pSpread);
				RenderRect = RenderRect.ToSpread(pSpread,pView);
			}
			RenderRect.Inflate(pRender->GetScaledPixelWidth());
		}

		Start = DocCoord(CurrentOrigin.x,RenderRect.lo.y);
		End   = DocCoord(CurrentOrigin.x,RenderRect.hi.y);
		pRender->DrawDashLine(Start,End);

		Start = DocCoord(RenderRect.lo.x,CurrentOrigin.y);
		End   = DocCoord(RenderRect.hi.x,CurrentOrigin.y);
		pRender->DrawDashLine(Start,End);

		// Get the Next render region
		pRender = DocView::GetNextOnTop(NULL);
	}	
}

/***********************************************************************************************

>	static BOOL OpSpreadOrigin::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if op successfully declared, FALSE otherwise
	Purpose:    Registers an op desc for this op
	SeeAlso:	-
		
***********************************************************************************************/

BOOL OpSpreadOrigin::Init()
{
	return RegisterOpDescriptor(0, 
								_R(IDS_OPSPREADORIGIN),
								CC_RUNTIME_CLASS(OpSpreadOrigin), 
								OPTOKEN_SPREADORIGIN ,
								OpSpreadOrigin::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								);
}


/***********************************************************************************************

>	static OpState OpSpreadOrigin::GetState(String_256* Description, OpDescriptor*)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/95
	Inputs:		Description = ptr to place description of why this op can't happen
				pOpDesc     = ptr to the Op Desc associated with this op
	Outputs:	-
	Returns:	An OpState object
	Purpose:    Func for determining the usability of this op
	SeeAlso:	-
		
***********************************************************************************************/

OpState OpSpreadOrigin::GetState(String_256* Description, OpDescriptor*)
{
	OpState State;
	return State;
}


///////////////////////////////////////////////////////////////////////////////
// OpResetSpreadOrigin

/******************************************************************************
>	OpResetSpreadOrigin::OpResetSpreadOrigin()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Purpose:    Default constructor
******************************************************************************/

OpResetSpreadOrigin::OpResetSpreadOrigin()
{
}


/******************************************************************************
>	BOOL OpResetSpreadOrigin::Init()

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Returns:	FALSE if fails
	Purpose:    Initilise OpResetSpreadOrigin
******************************************************************************/

BOOL OpResetSpreadOrigin::Init()
{
	return RegisterOpDescriptor(0, 
								_R(IDS_OPRESETSPREADORIGIN),
								CC_RUNTIME_CLASS(OpResetSpreadOrigin), 
								OPTOKEN_RESETSPREADORIGIN ,
								OpResetSpreadOrigin::GetState,
								0,  /* help ID */
								0,	/* bubble ID */
								0	/* bitmap ID */
								);
}


/*****************************************************************************
>	void OpResetSpreadOrigin::Do(OpDescriptor* pOpDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pOpDesc - not used
	Purpose:	Sets up params to set origin to page origin
				and invokes OpSpreadOrigin::DoChangeOrigin()
******************************************************************************/

void OpResetSpreadOrigin::Do(OpDescriptor* pOpDesc)
{
	Spread* pSpread = Document::GetSelectedSpread();
	ERROR3IF(pSpread==NULL,"OpResetSpreadOrigin::Do() - no seleted spread");
	DocCoord PageOrigin(0,0);
	BOOL ok = (pSpread!=NULL);
	if (ok) ok = pSpread->PagesCoordToSpreadCoord(&PageOrigin,DocCoord(0,0));
	if (ok) ok = DoChangeOrigin(pSpread,PageOrigin);

	if (!ok)
		FailAndExecute();
	End();
}


/******************************************************************************
>	static OpState OpResetSpreadOrigin::GetState(String_256* pReasonGreyed, OpDescriptor* pOpDesc)

	Author:		Ed_Cornes (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pOpDesc       - 
	Outputs:	pReasonGreyed - 
	Returns:	An OpState object
******************************************************************************/

OpState OpResetSpreadOrigin::GetState(String_256* pReasonGreyed, OpDescriptor* pOpDesc)
{
	OpState state(FALSE,TRUE);	// assume it will be greyed - for returning errors

	Spread* pSpread = Document::GetSelectedSpread();
	if (pSpread!=NULL)
	{
		DocCoord UserOrigin = UserCoord(0,0).ToSpread(pSpread);
		DocCoord PageOrigin(0,0);
		if (pSpread->PagesCoordToSpreadCoord(&PageOrigin,DocCoord(0,0))!=FALSE)
			state.Greyed = (UserOrigin==PageOrigin);
	}
	else
		ERROR3("OpResetSpreadOrigin::GetState() - no seleted spread");

	if (state.Greyed && pReasonGreyed!=NULL)
		pReasonGreyed->Load(_R(IDS_OPRESETSPREADORIGINGREY));

	return state;
}
#endif
