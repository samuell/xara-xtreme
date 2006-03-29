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

// Layer and LayerStatus classes  (removed from paper.cpp 15/4/94)


#include "camtypes.h"
#include "layer.h"
//#include "simon.h"
#include "ensure.h"
#include "spread.h"
#include "page.h"
//#include "mario.h"
#include "document.h"
#include "nodedoc.h"
#include "blobs.h"
#include "app.h"
#include "aw_eps.h"
#include "ccdc.h"
#include "rndrgn.h"
//#include "nativeps.h"		// The old style EPS native filter, used in v1.1
//#include "ben.h"
//#include "prdlgctl.h"
#include "printctl.h"
#include "docview.h"
//#include "jason.h"
//#include "sglayer.h"
//#include "cameleps.h"
#include "colourix.h"
#include "colmsg.h"
#include "cmxrendr.h"
//#include "ai_epsrr.h"
//-#include "cmxexdc.h"
//#include "tim.h"

// WEBSTER - markn 15/1/97
// Extra headers needed for Webster
//#include "markn.h"
#include "qualattr.h"

#include "camfiltr.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "cxfrec.h"

#include "layermsg.h"	// layer messages
//#include "prevwres.h"	// _R(IDS_FRAMENAMEUNIQUEFORM) _R(IDS_TAG_LAYER_FRAMEPROPS) ...
#include "slicehelper.h"
#include "nbevcont.h"
#include "ncntrcnt.h"
#include "nodecont.h"
#include "ndclpcnt.h"
#include "lineattr.h"

CC_IMPLEMENT_DYNAMIC(Layer, NodeRenderablePaper)          
CC_IMPLEMENT_DYNAMIC(LayerMsgHandler,MessageHandler) 

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

#define COL_MAX 255
INT32 Layer::GuideLayerRed	= COL_MAX;
INT32 Layer::GuideLayerGreen	= 0;
INT32 Layer::GuideLayerBlue	= 0;

LayerMsgHandler* Layer::pLayerMsgHandler = NULL;

/*********************************************************************************************

	Preference:	ActiveLayerVisibleAndEditable
	Section:	Layers
	Range:		0 - 1
	Purpose:	Dictates whether the active layer is automatically made visible and editable
				or whether it is left in its last state which might include it being invisible.
				Defaults to the old system (FALSE).
				
				It is controlled by the "Current layer always visible & editable" switch in
				the General options tab. 

	SeeAlso:	NodeRenderableBounded::Init();

**********************************************************************************************/ 

BOOL Layer::ActiveLayerVisibleAndEditable = FALSE;

/*********************************************************************************************

	Preference:	EnableLayerCacheing
	Section:	Layers
	Range:		0 - 2
	Purpose:	Control whether layers are cached or not. 

	SeeAlso:	NodeRenderableBounded::Init();

**********************************************************************************************/ 

INT32 Layer::EnableLayerCacheing = 1;

/*********************************************************************************************

>    static BOOL Layer::Init()

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   10/10/95
     Inputs:    -
     Outputs:   -
     Returns:   -
     Purpose: 	Does any layer initialisation necessary.
	 			Declares some layer INI settings
				Creates a layer message handler
     Errors:    

**********************************************************************************************/
 
BOOL Layer::Init()
{
	BOOL ok = TRUE;

#if !defined(EXCLUDE_FROM_RALPH)
 	// Declare the section that the preferences for layers will live in
	if (ok) ok= Camelot.DeclareSection(TEXT("Layers"), 3);

	// declare the guideline colour vars
	if (ok) ok= Camelot.DeclarePref(TEXT("Layers"), TEXT("GuideLayerRed"),  &Layer::GuideLayerRed,   0, COL_MAX);
	if (ok) ok= Camelot.DeclarePref(TEXT("Layers"), TEXT("GuideLayerGreen"),&Layer::GuideLayerGreen, 0, COL_MAX);
	if (ok) ok= Camelot.DeclarePref(TEXT("Layers"), TEXT("GuideLayerBlue"), &Layer::GuideLayerBlue,  0, COL_MAX);
#endif

	if (ok)
	{
		pLayerMsgHandler = new LayerMsgHandler;
		ok = (pLayerMsgHandler != NULL);
	}

	return ok;
}

/*********************************************************************************************

>    static void Layer::Deinit()

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   10/10/95
     Inputs:    -
     Outputs:   -
     Returns:   -
     Purpose: 	Deinits the layer system.
	 			This deletes the layer message handler
     Errors:    

**********************************************************************************************/

void Layer::Deinit()
{
	if (pLayerMsgHandler != NULL)
	{
		delete pLayerMsgHandler;
		pLayerMsgHandler = NULL;
	}
} 

/*********************************************************************************************

>    Layer::Layer() 

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    -
     Outputs:   -
     Returns:   -
			  
     Purpose: This constructor creates a Layer linked to no other nodes, with all status
		 	  flags false, and NULL bounding and pasteboard rectangles.            
			
     Errors:    

**********************************************************************************************/
 

Layer::Layer(): NodeRenderablePaper()
{ 
	// Set all layer flags to their defaults 
	Visible 	= TRUE; 
	Locked 		= FALSE; 
	Printable 	= TRUE; 
	Background 	= FALSE; 
	Outline 	= FALSE;
	Guide  		= FALSE;
    Active 		= FALSE; 

	pGuideColour= NULL;
	pIndexedGuideColour = NULL;

	m_PageBackground = FALSE;

	// Frame related items
	m_Solid		 = FALSE;
	m_Overlay	 = FALSE;
	m_Edited	 = FALSE;
	m_Frame		 = FALSE;
	m_HiddenFrame	= FALSE;
	m_FrameDelay = 10;

	m_pReferencedBitmap = NULL; // DO NOT DELETE as it is a reference
}                    
 
/*********************************************************************************************

>    Layer::~Layer() 

     Author:    Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
     Created:   10/10/95
     Purpose: 	Default destructor
     Errors:    

**********************************************************************************************/

Layer::~Layer()
{ 
	if (pGuideColour != NULL)
		delete pGuideColour;

	if (pIndexedGuideColour != NULL)
		delete pIndexedGuideColour;

	m_pReferencedBitmap = NULL; // DO NOT DELETE as it is a reference
}                    
 
/***********************************************************************************************

>   Layer::Layer
	(
		Node* ContextNode,  
		AttachNodeDirection Direction,  
		String_256& LayerID, 
		BOOL Locked = FALSE, 
		BOOL Mangled = FALSE,  
		BOOL Marked = FALSE, 
		BOOL Selected = FALSE, 
		BOOL Renderable = FALSE
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
					      
			LayerID: String which uniquely identifies layer within the spread   
								  
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

Layer::Layer(Node* ContextNode,  
					AttachNodeDirection Direction,
					const String_256 &LayerID,  
					BOOL NodeLocked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected 
		   		   ): NodeRenderablePaper(ContextNode, Direction, NodeLocked, Mangled, 
									Marked, Selected)
{ 
	// Set all layer flags to their defaults 
	Visible 	= TRUE; 
	Locked 		= FALSE; 
	Printable 	= TRUE; 
	Background 	= FALSE; 
	Outline 	= FALSE;
	Guide  		= FALSE;
	Active 		= FALSE; 

	m_PageBackground = FALSE;

	// Frame related flags
	m_Overlay		= FALSE;
	m_Solid			= FALSE;
	m_Edited		= FALSE;
	m_Frame			= FALSE;
	m_HiddenFrame	= FALSE;
	m_FrameDelay	= 10;

	m_pReferencedBitmap = NULL; // DO NOT DELETE as it is a reference

	pGuideColour	= NULL;
	pIndexedGuideColour = NULL;

	LayerSt.StringLayerID = LayerID; 
}     

/********************************************************************************************

>	BOOL Layer::NeedsToRender(RenderRegion *pRender)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/02/94
	Inputs:		pRender - A pointer to the current render region (NULL if none)
	Returns:	TRUE  => This node should be rendered,
				FALSE => This node does not need to be rendered.
	Purpose:	Virtual function - this version decides whether or not a layer should be
				rendered according to the ExcludeLockedLayers flags and the state of the
				layer itself.
	SeeAlso:	Node::NeedsToRender

********************************************************************************************/

SubtreeRenderState Layer::RenderSubtree(RenderRegion *pRender, Node** ppNextNode, BOOL bClip)
{
	if (pRender==NULL)
		return SUBTREE_ROOTANDCHILDREN;

	// ignore hit detection render regions when the layer is locked.
	if (pRender->IsHitDetect() && IsLocked())
		return SUBTREE_NORENDER;

	PORTNOTE("printing","Layer::RenderSubtree - removed printing code and CamelotEPSRenderRegion reference");
#ifndef EXCLUDE_FROM_XARALX
	// If we are printing then we do special things
	if (pRender->IsPrinting())
	{
		// Ignore non-printable layers (i.e. don't ever print background layers)
		if (!IsPrintable())
			return SUBTREE_NORENDER;

		// Added this since we want to print what we see - might need looking at ?
		return IsVisible() ? SUBTREE_ROOTANDCHILDREN : SUBTREE_NORENDER;

		// Otherwise, get the print control
		
		CCPrintInfo *pInfo = CCPrintInfo::GetCurrent();
//		ERROR2IF(pInfo == NULL, FALSE, "No print info for printing render region!");
		PrintControl *pPrCtrl = NULL;
		if (pInfo != NULL)
			pPrCtrl = pInfo->GetPrintControl();
//		ERROR2IF(pPrCtrl == NULL, FALSE, "No print control for printing render region!");

		// Ok, work out what whether we should print this layer...
		if (pPrCtrl == NULL || pPrCtrl->GetPrintLayers() == PRINTLAYERS_VISIBLEFOREGROUND)
		{
			// Print only visible foreground layers.
			return IsVisible() ? SUBTREE_ROOTANDCHILDREN : SUBTREE_NORENDER;
		}
		else
		{
			// Print all foreground layers, regardless of visibility.
			return SUBTREE_ROOTANDCHILDREN;
		}

	}
	else if (IS_A(pRender,CamelotEPSRenderRegion))
		return (IsVisible() && IsPrintable()) ? SUBTREE_ROOTANDCHILDREN : SUBTREE_NORENDER;
	else
#endif
	{
		// Normal rendering
		//
		// Ask Helper function to set up cacheing for me...
		//
		// If we can find a cached bitmap for this node and render it
		// Then tell the renderer to move on without doing any more...
		// Else we might have set up a capture, in which case render as normal
		// and let RenderAfterSubtree catch the capture at the other end
		switch (Layer::EnableLayerCacheing)
		{
		// Value 1 means enable all layer cacheing
		case 1:
			if (IsVisible() && !IsGuide() && RenderCached(pRender))
			{
				return SUBTREE_NORENDER;
			}
		break;

		// Value 2 means enable layer cacheing for all but the active layer
		case 2:
			if (IsVisible() && !IsGuide() && !IsActive() && RenderCached(pRender))
			{
				return SUBTREE_NORENDER;
			}
		break;

		}

		// Simple visibility check...
		return IsVisible() ? SUBTREE_ROOTANDCHILDREN : SUBTREE_NORENDER;
	}

	return SUBTREE_NORENDER;
}



/********************************************************************************************

>	void Layer::RenderAfterSubtree(RenderRegion* pRender)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/08/2004
	Inputs:		pRender - The region to render into
	Purpose:	Capture the group as a cached bitmap

********************************************************************************************/

void Layer::RenderAfterSubtree(RenderRegion* pRegion)
{
	// Call Helper function to run all my cacheing functionality for me...
	if (IsVisible())
		CaptureCached(pRegion);
}



/********************************************************************************************

>	BOOL Layer::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly = FALSE,
							  BOOL CheckSelected = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/03/94
	Inputs:		pRender - A pointer to the current export region (NULL if none)
				VisibleLayersOnly - TRUE => remove nodes which are on invisible layers
									   - FALSE => export everything
				CheckSelected - TRUE => we check if object selected and only export selected bjects
							  - FALSE => we don't bother checking for selection or not
	Returns:	TRUE => export this node.
	Purpose:	Indicate that we want to export layers.
				Now takes a parameter to say whether we want to take notice of the layers
				visible status when deciding about exporting it or not.
	SeeAlso:	Layer::NeedsToRender

********************************************************************************************/

BOOL Layer::NeedsToExport(RenderRegion *pRender, BOOL VisibleLayersOnly, BOOL CheckSelected)
{
#ifdef DO_EXPORT
	// If this is export as a ART file, we want to save all the layers
	if (pRender->IS_KIND_OF(NativeRenderRegion))
		return TRUE;

	// If there's a filter running
	// and it's trying to export just one layer
	// then return FALSE if this layer is NOT the layer in question
	// else fall through to the normal logic...
	Filter* pFilter = pRender->GetFilter();
	if (pFilter)
	{
		Layer* pFilterLayer = pFilter->GetSoleLayer();
		if (pFilterLayer!=NULL && pFilterLayer!=this)
			return FALSE;
	}

	// if this is an export as a CMX file, we don't want invisible layers
	// as CMX doesn't support non-visible stuff so we can't save it and
	// get the file looking the same
	if(!IsVisible() && pRender->IS_KIND_OF(CMXRenderRegion))
		return FALSE;

	// Guide layers only export to Native render regions
	if (Guide)
		return FALSE;

	// Changed so that instead of always returning True there is a parameter which
	// defaults to the old state whereby it always returns True but can be set so
	// that we return the visible state of the layer and hence this dictates whether
	// only visible layers will be exported.
	if (VisibleLayersOnly)
		return (IsVisible());
	else
		return(TRUE);
#else
	return FALSE;
#endif
}




/********************************************************************************************

>	BOOL Layer::IsLayer() const

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/11/94
	Returns:	TRUE => This node is a layer
	Purpose:	Indicate that we are a layer (overrides function in Node).
	SeeAlso:	Node::IsLayer

********************************************************************************************/

BOOL Layer::IsLayer() const
{
	return TRUE;
}




/********************************************************************************************

>	virtual String Layer::Describe(BOOL Plural, BOOL Verbose = TRUE)

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
              
String Layer::Describe(BOOL Plural, BOOL Verbose) 
{     
	if (Plural)
		return(String(_R(IDS_LAYER_DESCRP)));  
	else
		return(String(_R(IDS_LAYER_DESCRS))); 
}; 
 

/*********************************************************************************************

>    DocRect Layer::GetPasteboardRectangle(BOOL Pixelise = TRUE, View *pView = NULL) const

     Author:    Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
     Created:   13/5/93
     Inputs:    Pixelise - whether or not to pixelise the pasteboard rectangle before
						   returning it.
				pView - the view to pixelise to (not used if Pixelise is FALSE).
     Outputs:   The objects pasteboard rectangle
     Returns:   -
			  
     Purpose:   For obtaining the layer's pasteboard rectangle which is actually the 
     			pasteboard rectangle of its parent spread node.  
			
     Errors:    

**********************************************************************************************/

DocRect Layer::GetPasteboardRect(BOOL Pixelise, View *pView) const
{
	// Ensure that the parent of the Layer object is a spread
	ERROR2IF(!Parent->IsKindOf(CC_RUNTIME_CLASS(Spread)),
			DocRect(0,0,0,0),
			"The parent of a layer node was not a spread node");

	return ( ((Spread*)Parent)->GetPasteboardRect(Pixelise, pView) );
}       
	           	
	
/***********************************************************************************************
> Node* Layer::SimpleCopy()  

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

Node* Layer::SimpleCopy()
{
	Layer* NodeCopy; 
	NodeCopy = new Layer();  
	//ERRORIF(NodeCopy == NULL, _R(IDE_NOMORE_MEMORY), NULL); 
	if (NodeCopy != NULL)
		CopyNodeContents(NodeCopy);   
	return (NodeCopy);
}           
  
#ifdef _DEBUG  

void Layer::ShowDebugTreeDetails() const
{                                 
	TRACE( _T("Layer ")); 
	Node::ShowDebugTreeDetails(); 
}            

#endif 	  
 

/***********************************************************************************************
>   void Layer::CopyNodeContents(Layer* NodeCopy)

    Author:     Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
    Created:    10/8/94
	Inputs:     NodeCopy = ptr to the layer node that needs to become a copy of this layer
    Outputs:    The layer at *NodeCopy is a identical copy of this layer
    Returns:    -
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void Layer::CopyNodeContents(Layer* NodeCopy)
{
	// Ensure the pointer's not null
	ENSURE(NodeCopy != NULL,"Trying to copy a node's contents into a NULL node");  

	// Get immediate base class to copy its bits
	NodeRenderablePaper::CopyNodeContents(NodeCopy); 

	// Copy layer-specific stuff now.
	NodeCopy->LayerSt = LayerSt;

	NodeCopy->Active = Active;
	NodeCopy->Visible = Visible;
	NodeCopy->Locked = Locked;
	NodeCopy->Printable = Printable;
	NodeCopy->Background = Background;
	NodeCopy->Outline = Outline;
	NodeCopy->Guide = Guide;

	NodeCopy->Active = Active;
	NodeCopy->Visible = Visible;
	NodeCopy->Locked = Locked;
	NodeCopy->Printable = Printable;
	NodeCopy->Background = Background;
	NodeCopy->Outline = Outline;
	NodeCopy->Guide = Guide;

	// Copy across the useful frame related information
	NodeCopy->m_FrameDelay		= m_FrameDelay;
	NodeCopy->m_Overlay			= m_Overlay;
	NodeCopy->m_Solid			= m_Solid;
	NodeCopy->m_Frame			= m_Frame;
	NodeCopy->m_HiddenFrame		= m_HiddenFrame;
	NodeCopy->m_Edited			= m_Edited;
	NodeCopy->m_PageBackground	= m_PageBackground;
}       
   
                                             
/***********************************************************************************************
>   void Layer::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void Layer::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, Layer), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, Layer))
		CopyNodeContents((Layer*)pNodeCopy);
}



/***********************************************************************************************

>	Layer* Layer::FindNextLayer(BOOL ExcludeInvisible = FALSE, BOOL ExcludeLocked = FALSE)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/01/94
	Inputs:		ExcludeInvisible	Do not consider any invisible layer.
				ExcludeLocked		Do not consider any locked layer.
	Outputs:	-
	Returns:	The next sibling layer of this node, or NULL if there are no more layers 
	Purpose:    To find the next sibling layer

	Notes:		Karim 08/08/2000
				Extended this method so we can also exclude locked layers from the search.

	Errors:		-
	SeeAlso:	-

***********************************************************************************************/


Layer* Layer::FindNextLayer(BOOL ExcludeInvisible, BOOL ExcludeLocked)
{
	Node* CurrentNode = FindNext();
	while (CurrentNode != NULL)
	{
		if (CurrentNode->IsLayer())
		{
			Layer* l = (Layer*) CurrentNode;
			if (ExcludeInvisible && !l->IsVisible())
			{
				// we're ignoring invisible layers and this layer is invisible.
			}
			else if (ExcludeLocked && l->IsLocked())
			{
				// we're ignoring locked layers and this layer is locked.
			}
			else
			{
				// this one's ok - return it.
				return l;
			}
		}

		CurrentNode = CurrentNode->FindNext();
	}

	// no layer found.
	return NULL;
}


/***********************************************************************************************

>	Layer* Layer::FindPrevLayer(BOOL ExcludeInvisible = FALSE, BOOL ExcludeLocked = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/94
	Inputs:		ExcludeInvisible	Do not consider any invisible layer.
				ExcludeLocked		Do not consider any locked layer.
	Outputs:	-
	Returns:	The prev sibling layer of this node, or NULL if there are no more layers 
	Purpose:    To find the prev sibling layer

	Notes:		Karim 08/08/2000
				Extended this method so we can also exclude locked layers from the search.

	Errors:		-
	SeeAlso:	-

***********************************************************************************************/


Layer* Layer::FindPrevLayer(BOOL ExcludeInvisible, BOOL ExcludeLocked)
{
	Node* CurrentNode = FindPrevious();
	while (CurrentNode != NULL)
	{
		if (CurrentNode->IsLayer())
		{
			Layer* l = (Layer*) CurrentNode;
			if (ExcludeInvisible && !l->IsVisible())
			{
				// we're ignoring invisible layers and this layer is invisible.
			}
			else if (ExcludeLocked && l->IsLocked())
			{
				// we're ignoring locked layers and this layer is locked.
			}
			else
			{
				// this one's ok - return it.
				return l;
			}
		}

		CurrentNode = CurrentNode->FindPrevious(); 
	}

	// no layer found.
	return (NULL);
}

/***********************************************************************************************

>	Layer* Layer::FindNextFrameLayer()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/97
	Returns:	The next sibling frame layer of this node, or NULL if there are no more layers 
	Purpose:    To find the next sibling frame layer
	Errors:		-
	SeeAlso:	FindPrevFrameLayer; FindPrevLayer;

***********************************************************************************************/
  
Layer* Layer::FindNextFrameLayer()
{
	Layer* pCurrentLayer = FindNextLayer(); 
	while (pCurrentLayer != NULL)
	{
		// Frame layers cannot be guide layers or background layers
		if (pCurrentLayer->IsFrame() && !pCurrentLayer->IsPageBackground() && !pCurrentLayer->IsGuide())
		{ 
			return pCurrentLayer;	  
		}

		pCurrentLayer = pCurrentLayer->FindNextLayer(); 
	}
	
	// No next frame layer found
	return NULL;
}                                                               
  

/***********************************************************************************************

>	Layer* Layer::FindPrevFrameLayer()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/04/97
	Returns:	The previous sibling frame layer of this node, or NULL if there are no more layers 
	Purpose:    To find the prev sibling frame layer
	Errors:		-
	SeeAlso:	FindNextFrameLayer; FindNextLayer;

***********************************************************************************************/
  
Layer* Layer::FindPrevFrameLayer()
{
	Layer* pCurrentLayer = FindPrevLayer(); 
	while (pCurrentLayer != NULL)
	{
		// Frame layers cannot be guide layers or background layers
		if (pCurrentLayer->IsFrame() && !pCurrentLayer->IsPageBackground() && !pCurrentLayer->IsGuide())
		{ 
			return pCurrentLayer;	  
		}

		pCurrentLayer = pCurrentLayer->FindPrevLayer(); 
	}

	// No next frame layer found
	return NULL;
}                                                               

/***********************************************************************************************

>    virtual BOOL Layer::HidingNode()

     Author:    Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
     Created:   17/6/97
	 Returns:   TRUE if all was ok. FALSE if an error occured.
     Purpose:   This virtual function is called whenever the node is hidden.
	 			It allows the node do things like 'optimise' itself to use less memory or
	 			send a message to let others know it is being hidden etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL Layer::HidingNode()
{
	PORTNOTETRACE("other","Layer::HidingNode - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Call the base class first
	if (!Node::HidingNode())
		return FALSE;

	// Check if we are the active layer, if we are then try and make
	// another layer active
	if (IsActive())
	{
		// Hiding an active layer
		TRACEUSER( "Neville", _T("++++++ Hiding the active layer\n"));

// Included in Camelot2 for frame/layer integration - RanbirR - 06/11/97
//#ifdef WEBSTER
		if (!IsFrame())
		{
			// Choose the next/prev layer to be the new active one
			Layer* pNewActiveLayer = FindPrevLayer();
			if (pNewActiveLayer == NULL)
				pNewActiveLayer = FindNextLayer();
			if (pNewActiveLayer != NULL)
				LayerSGallery::MakeActiveLayer(pNewActiveLayer);
		}
		else
		{
			// Choose the next/prev frame layer to be the new active one
			Layer* pNewActiveLayer = FindPrevFrameLayer();
			if (pNewActiveLayer == NULL)
				pNewActiveLayer = FindNextFrameLayer();
			if (pNewActiveLayer != NULL)
				FrameSGallery::MakeActiveLayer(pNewActiveLayer);
		}
	}

	// If we are hiding the page background layer then we must mark all frame layers
	// as edited as they must be regenerated.
	// Use frame layers as only these will have been captured
	if (IsPageBackground())
	{
		Node * pParentNode = FindParent();
		Spread * pSpread = NULL;
		if (pParentNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
			pSpread = (Spread*)pParentNode;
		if (pSpread)
		{
			Layer* pLayer = pSpread->FindFirstFrameLayer();
			while (pLayer != NULL)
			{
				// This is just as a double check
				if (pLayer->IsPseudoFrame())
				{
					// Set the edited flag on the layer
					pLayer->SetEdited(TRUE);
#ifdef _DEBUG
					// Tell the frame gallery to update its display of the frame
					BROADCAST_TO_ALL(LayerMsg(pLayer, LayerMsg::LayerReason::REDRAW_LAYER));
#endif
				}

				// Move to the next frame layer in the animation
				pLayer = pLayer->FindNextFrameLayer();
			}
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	LayerStatus::LayerStatus() 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerStatus constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

 
LayerStatus::LayerStatus() 
{
}

/********************************************************************************************

>	LayerStatus::LayerStatus(LayerStatus& ls) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerStatus copy constructor 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/



LayerStatus::LayerStatus(LayerStatus& ls) 
{
//	Flags.Visible = ls.Flags.Visible; 
//	Flags.Locked = ls.Flags.Locked;
//	Flags.Printable = ls.Flags.Printable;
//	Flags.Background = ls.Flags.Background;
//	Flags.Outline = ls.Flags.Outline;

	StringLayerID = ls.StringLayerID; 
}

/********************************************************************************************

>	LayerStatus::operator==(LayerStatus Status)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	LayerStatus == operator
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


INT32 LayerStatus::operator==(LayerStatus Status)
{
	return ( 
//			 (Flags.Visible    == Status.Flags.Visible) && 
//			 (Flags.Locked     == Status.Flags.Locked) && 
//			 (Flags.Printable  == Status.Flags.Printable) && 
//			 (Flags.Background == Status.Flags.Background) && 
//			 (Flags.Outline	   == Status.Flags.Outline) &&
		  	 (StringLayerID    == Status.StringLayerID)
		   ); 
}


// Query Layer status functions 
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	Document* Layer::FindDocument()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/94
	Inputs:		-
	Outputs:	-
	Returns:	Finds the Document object that contains the tree that contains this layer, or NULL (God forbit)
	Purpose:	This should be able to return the document associated with this layer.
				If not, then this program is a worthless piece of junk and way beyond repair
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

Document* Layer::FindDocument()
{
	Node* pThisNodeDoc = FindParent(CC_RUNTIME_CLASS(NodeDocument));

	if (pThisNodeDoc != NULL)
	{
		List* pList = &GetApplication()->Documents;

		if (pList != NULL)
		{
			Document* pDoc = (Document*) pList->GetHead();

			while (pDoc != NULL)
			{
				Node* pNodeDoc = pDoc->GetFirstNode();
				if (pNodeDoc != NULL)
					pNodeDoc = pNodeDoc->FindNext(CC_RUNTIME_CLASS(NodeDocument));

				if (pNodeDoc == pThisNodeDoc)
					return (pDoc);

				pDoc = (Document*) pList->GetNext(pDoc);
			}
		}
	}

	return NULL;
}

/********************************************************************************************

>	BOOL Layer::BelongsToDocument(Document* pDoc)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		pDoc = ptr to the document
	Outputs:	-
	Returns:	TRUE if the layer belongs to the current document
	Purpose:	Checks to see if this layer belongs to the document, i.e. the node is part
				of the tree that is attached to the given document.	
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
// A quickie for Alpha release on 15/8/94                                                             
// THIS FUNC IS IN THE WRONG PLACE!!!
// It should be a member of Document and have a prototype something like this -
//
//	BOOL Document::ContainsNode(Node* pNode)
//
// Didn't have time to wait for a rebuild after changing document.h

BOOL Layer::BelongsToDocument(Document* pDoc)
{
	ENSURE(pDoc != NULL,"Get a NULL pDoc");
	if (pDoc == NULL) return FALSE;

	Node* pTreeStart = pDoc->GetFirstNode();

	ENSURE(pTreeStart != NULL,"This doc doesn't have a tree start node");

	Node* pNodeDoc	   = pTreeStart->FindNext(CC_RUNTIME_CLASS(NodeDocument));
	Node* pThisNodeDoc = FindParent(CC_RUNTIME_CLASS(NodeDocument));

	ENSURE(pNodeDoc     != NULL,"This doc has no NodeDocument node");
	ENSURE(pThisNodeDoc != NULL,"This layer has no NodeDocument parent node");

	BOOL ok = ((pNodeDoc != NULL) && (pThisNodeDoc != NULL) && (pNodeDoc == pThisNodeDoc));

	if( !ok )
	{
		TRACEUSER( "MarkN", _T("-------\n"));
		TRACEUSER( "MarkN", _T("pDoc            = %ld 0x%p\n"),pDoc,pDoc);
		TRACEUSER( "MarkN", _T("pLayer          = %ld 0x%p\n"),this,this);
		TRACEUSER( "MarkN", _T("Layer's NodeDoc = %ld 0x%p\n"),pThisNodeDoc,pThisNodeDoc);
		TRACEUSER( "MarkN", _T("pDoc's  NodeDoc = %ld 0x%p\n"),pNodeDoc,pNodeDoc);
	}

	return (ok);
}

/********************************************************************************************

>	BOOL Layer::IsVisible(DocView* pView = NULL)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (modified by Markn)
	Created:	6/1/94
	Inputs:		pDocView = ptr to a docview (only used if the layer is a guide layer)
	Outputs:	-
	Returns:	TRUE if the layer is visible, otherwise FALSE. 
	Purpose:	For finding the layers visible status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
/*	Implementation notes:

	A layer is visible if the following is true - 
		The layer's visible flag is TRUE 
	OR	The layer is active and ActiveLayerVisibleAndEditable is TRUE
	OR	The AllVisible flag in the parent document is TRUE

	Guide layers are a special case:

	If the layer is a guide layer, then the visibility is determined by the associated DocView's GuideShow flag.
	The flag in the given DocView is used.
	If no DocView is given, the current DocView is used.
*/

BOOL Layer::IsVisible(DocView* pDocView)
{
	if (Guide)
	{
		// WEBSTER - markn 29/1/97
		// Guide layers are always invisible
		#ifndef WEBSTER
			if (pDocView == NULL)
				pDocView = DocView::GetCurrent();
			if (pDocView != NULL)
				 return (pDocView->GetShowGuidesState());
			else
				return FALSE;
		#else
			return FALSE;
		#endif // WEBSTER
	}

	// If it is visible, fast track the return
	if (Visible)
		return (TRUE);

	// If the "Active layer is editable and visible" flag is TRUE,
	// then return TRUE (layer is visible) if this layer is Active
	if (Active && ActiveLayerVisibleAndEditable)
		return (TRUE);

	// This layer is NOT visible. However, if the document to which this layer belongs
	// has the AllVisible flag set, then return TRUE
	// I.e. return the doc's AllVisible state
	Document *pDoc = FindDocument();
	if (pDoc == NULL)
		return(TRUE);

	return(pDoc->IsAllVisible());
}
  
/********************************************************************************************

>	BOOL Layer::GetVisibleFlagState()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer's Visible is TRUE, FALSE otherwise
	Purpose:	Returns the raw state of the layer's visible flag (specialist use only)	
	Errors:		-
	SeeAlso:	Layer::IsVisible()

********************************************************************************************/

BOOL Layer::GetVisibleFlagState()
{	
	if (Guide)
	{
		DocView* pDocView = DocView::GetCurrent();
		if (pDocView != NULL)
			 return (pDocView->GetShowGuidesState());
	}

	return (Visible);
}
  
/********************************************************************************************

>	BOOL Layer::IsLocked()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com> (modified by Markn) (modified to not use CurrentDoc by Jason)
	Created:	6/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is locked, otherwise FALSE. 
	Purpose:	For finding the layers locked status 
				A layer is locked if the following is true - 
					The layer's locked flag is TRUE 
				AND	The layer is NOT active (clause not used in Alpha release 15/8/94)
				AND	The Multilayer flag in the parent document is FALSE
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Layer::IsLocked()
{
/*	// WEBSTER - markn 29/1/97
	// All layers are editable in Webster
	#ifndef WEBSTER */
		// Disabled as we now have frame layers and some layers MUST be locked

		// Fast track it if the layer is not locked (the majority of cases)
		if (!Locked)
			return (FALSE);

		// If we are the page background layer then ALWAYS return locked
		if (IsPageBackground())
			return TRUE;

		// If the "Active layer is editable and visible" flag is TRUE,
		// then return FALSE (layer is editable) if this layer is Active
		if (Active && ActiveLayerVisibleAndEditable)
			return (FALSE);

		// This layer is locked. However, if the document to which this layer belongs
		// has the Multilayer flag set (i.e. all layers editable), the return FALSE
		// I.e. return the inverse of the doc's Multilayer state
		Document *pDoc = FindDocument();
		if (pDoc == NULL)
			return (FALSE);

		return (!pDoc->IsMultilayer());
/*	#else
		// Fast track it if the layer is not locked (the majority of cases)
		// Required in Webster so that when the BackgroundOp is inserting objects and
		// it has turned the locked status off, then we do not get the warning. In other
		// cases where the user is trying to change the background layer and the locked
		// flag is set to its default true state, we should fall through to the PageBackground
		// test and fail.
		if (!Locked)
			return (FALSE);

		// If we are the page background layer then ALWAYS return locked
		if (IsPageBackground())
			return TRUE;
		
		// always return FALSE i.e. editable
		return FALSE;
	#endif // WEBSTER */
}

/********************************************************************************************

>	BOOL Layer::GetLockedFlagState()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer's Locked is TRUE, FALSE otherwise
	Purpose:	Returns the raw state of the layer's Locked flag (specialist use only)	
	Errors:		-
	SeeAlso:	Layer::IsLocked()

********************************************************************************************/

BOOL Layer::GetLockedFlagState()
{	
	return (Locked);
}
  
/********************************************************************************************

>	BOOL Layer::IsActive()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is active, otherwise FALSE. 
	Purpose:	For finding the layers active status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL Layer::IsActive()
{
	return (Active); 
} 

/********************************************************************************************

>	BOOL Layer::IsPrintable()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is printable, otherwise FALSE. 
	Purpose:	For finding the layers printable status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL Layer::IsPrintable()
{
	return (Printable && !IsGuide()); 
} 

/********************************************************************************************

>	BOOL Layer::IsBackground()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is a background layer, otherwise FALSE. 
	Purpose:	For finding the layers background status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL Layer::IsBackground()
{
	return (Background); 
} 

/********************************************************************************************

>	BOOL Layer::IsOutline()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is to render its objects in outline mode
	Purpose:	For finding the layers outline status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL Layer::IsOutline()
{
	return (Outline); 
} 

/********************************************************************************************

>	BOOL Layer::IsGuide()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is one that can contain guidelines
	Purpose:	For finding the layers guide status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


BOOL Layer::IsGuide()
{
	return (Guide); 
} 

/********************************************************************************************

>	BOOL Layer::IsPageBackground()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is one that can contains the page background
	Purpose:	For finding the layer's page background status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL Layer::IsPageBackground()
{
	return (m_PageBackground); 
} 


/********************************************************************************************

>	BOOL Layer::IsOverlay()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Returns:	TRUE if the frame/layer is one that has its Overlay flag set
	Purpose:	For finding the frame's/layer's Overlay status 

********************************************************************************************/

BOOL Layer::IsOverlay()
{
	return m_Overlay;
}

/********************************************************************************************

>	BOOL Layer::IsSolid()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Returns:	TRUE if the frame/layer is one that has its Solid flag set
	Purpose:	For finding the frame's/layer's Solid status 

********************************************************************************************/

BOOL Layer::IsSolid()
{
	return m_Solid;
}

/********************************************************************************************

>	BOOL Layer::IsEdited()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Returns:	TRUE if the frame/layer is one that has its Edited flag set
	Purpose:	For finding the frame's/layer's Edited status 

********************************************************************************************/

BOOL Layer::IsEdited()
{
	return m_Edited;
}

/********************************************************************************************

>	BOOL Layer::IsFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Returns:	TRUE if the frame/layer is one that has its Frame flag set
	Purpose:	For finding the frame's/layer's Frame status 

********************************************************************************************/

BOOL Layer::IsFrame()
{
	return m_Frame;
}

/********************************************************************************************

>	BOOL Layer::IsHiddenFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Returns:	TRUE if the frame/layer is one that has its Hidden flag set
	Purpose:	For finding the frame's/layer's Hidden status
				This marks this frame as a hidden frame layer which will not be saved into
				the GIF animation but may take part in the rendering process of other frames.

********************************************************************************************/

BOOL Layer::IsHiddenFrame()
{
	return m_HiddenFrame;
}

/********************************************************************************************

>	BOOL Layer::IsPseudoFrame()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/4/97
	Returns:	TRUE if the frame/layer is one that might be a frame layer
	Purpose:	For finding out whether this might be regarded as a frame layer without
				especially having its frame flag set.

********************************************************************************************/

BOOL Layer::IsPseudoFrame()
{
	// Only true if the layer is not a background layer or a guide layer or a page background
	// layer
	return (!IsBackground() && !IsGuide() && !IsPageBackground());
}

/********************************************************************************************
>	UINT32 Layer::GetFrameDelay()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/97
	Returns:	The delay value for each animation frame layer.
	
********************************************************************************************/

DWORD Layer::GetFrameDelay()
{
	return m_FrameDelay;
}

/********************************************************************************************
>	UINT32 Layer::SetFrameDelay()

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/05/97
	Returns:	-
	purpose:	To set the delay value for each animation frame layer.
	
********************************************************************************************/
void Layer::SetFrameDelay(const DWORD& Delay)
{
	m_FrameDelay = Delay;
}

/********************************************************************************************

>	BOOL Layer::IncludeLayerInBoundingCalcs()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/4/97
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the layer is one that can contains the page background
	Purpose:	For finding out whether to include the layer in bounding box calcalutions.
				This may not be a simple IsVisible test as usually you want to exclude the
				two special layers:- guides and page background.
				This function should be used when:-
					Calculating drawing sizes for zooming
					Calculating drawing sizes for export
	Errors:		-
	SeeAlso:	Spread::GetPageVisibleBounds(); BaseBitmapFilter::GetSizeOfDrawing();

********************************************************************************************/

BOOL Layer::IncludeLayerInBoundingCalcs()
{
	// Only add the bounding box of the layer in if it is visible and if it is not
	// either a guide layer or the page background layer.
	return (!IsPageBackground() && !IsGuide() && IsVisible());
} 



/********************************************************************************************

>	String_256& Layer::GetLayerID()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		-
	Outputs:	-
	Returns:	The layers ID
	Purpose:	For finding the layers identifier 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

String_256& Layer::GetLayerID()
{
	return (LayerSt.StringLayerID); 
} 

/********************************************************************************************

>	LayerStatus& GetLayerStatus()

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/1/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Returns the layers status 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

LayerStatus& Layer::GetLayerStatus()
{
	return (LayerSt); 
} 

// Set Layers status functions 
// -----------------------------------------------------------------------------------------

/********************************************************************************************

>	void Layer::SetVisible(BOOL Status) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		Status: Status of the Layer's visible flag 
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's visible flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Layer::SetVisible(BOOL Status) 
{
	if (Guide)
	{
		DocView* pDocView = DocView::GetCurrent();
		if (pDocView != NULL)
			 pDocView->SetShowGuidesState(Status);
	}
	else
		Visible = Status;
}

/********************************************************************************************

>	void Layer::SetLocked(BOOL Status) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		Status: Status of the Layer's locked flag 
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's locked flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Layer::SetLocked(BOOL Status) 
{
	Locked = Status; 
}

/********************************************************************************************

>	void Layer::SetActive(BOOL Status) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		Status: Status of the Layer's active flag 
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's active flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Layer::SetActive(BOOL Status) 
{
	Active = Status; 
}

/********************************************************************************************

>	void Layer::SetPrintable(BOOL Status) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		Status: Status of the Layer's printable flag 
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's printable flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Layer::SetPrintable(BOOL Status) 
{
	Printable = Status; 
}

/********************************************************************************************

>	void Layer::SetBackground(BOOL Status) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		Status: Status of the Layer's background flag 
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's background flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Layer::SetBackground(BOOL Status) 
{
	Background = Status; 
}


/********************************************************************************************

>	void Layer::SetOutline(BOOL Status) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		Status: Status of the Layer's outline flag
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's outline flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Layer::SetOutline(BOOL Status) 
{
	Outline = Status; 
}

/********************************************************************************************

>	void Layer::SetGuide(BOOL Status) 

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/9/95
	Inputs:		Status: Status of the Layer's Guide flag
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's Guide flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/


void Layer::SetGuide(BOOL Status) 
{
	Guide = Status; 
}

/********************************************************************************************

>	void Layer::SetPageBackground(BOOL Status) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/97
	Inputs:		Status: Status of the Layer's PageBackground flag
	Outputs:	-
	Returns:	-
	Purpose:	To set the Layer's PageBackground flag status (TRUE/FALSE)
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Layer::SetPageBackground(BOOL Status) 
{
	m_PageBackground = Status; 
}

/********************************************************************************************

>	void Layer::SetSolid(BOOL Status) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		Status: Status of the Frame/Layer's Solid flag
	Purpose:	To set the Frame/Layer's Solid flag status (TRUE/FALSE)

********************************************************************************************/

void Layer::SetSolid(BOOL Status)
{
	m_Solid = Status; 
}

/********************************************************************************************

>	void Layer::SetOverlay(BOOL Status) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		Status: Status of the Frame/Layer's Overlay flag
	Purpose:	To set the Frame/Layer's Overlay flag status (TRUE/FALSE)

********************************************************************************************/

void Layer::SetOverlay(BOOL Status)
{
	m_Overlay = Status; 
}

/********************************************************************************************

>	void Layer::SetEdited(BOOL Status) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		Status: Status of the Frame/Layer's Edited flag
	Purpose:	To set the Frame/Layer's Edited flag status (TRUE/FALSE)

********************************************************************************************/

void Layer::SetEdited(BOOL Status)
{
	m_Edited = Status; 
}

/********************************************************************************************

>	void Layer::SetFrame(BOOL Status) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/4/97
	Inputs:		Status: Status of the Frame/Layer's Frame flag
	Purpose:	To set the Frame/Layer's Frame flag status (TRUE/FALSE)

********************************************************************************************/

void Layer::SetFrame(BOOL Status)
{
	m_Frame = Status; 
}

/********************************************************************************************

>	void Layer::SetHiddenFrame(BOOL Status) 

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/7/97
	Inputs:		Status: Status of the Frame/Layer's Hidden flag
	Purpose:	To set the Frame/Layer's Hidden flag status (TRUE/FALSE)

********************************************************************************************/

void Layer::SetHiddenFrame(BOOL Status)
{
	m_HiddenFrame = Status; 
}

/********************************************************************************************
// Generated Bitmap related functions
********************************************************************************************/

/********************************************************************************************

>	virtual ChangeCode Layer::OnChildChange(ObjChangeParam* pParam)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/05/97
	Inputs:		pParam	= pointer to a object change parameter class
	Returns:	CC_OK		if we have successfully processed the change.
				CC_FAIL		if we cannot handle this particular change and must prevent the
							child from continuing
	Purpose:	We override the function to mark layers as edited.
				This should do the same as the baseclass i.e. return ok but should else set
				the edited flag to true.
				Composite objects can use this function to respond to one of their children
				undergoing a change. They should return CC_FAIL whenever they are unable to
				cope with the change.
	SeeAlso:	WarnParentOfChange();

********************************************************************************************/

ChangeCode Layer::OnChildChange(ObjChangeParam* pParam)
{
	// Note that this layer has been edited
	SetEdited(TRUE);

#ifdef _DEBUG
	// Tell the frame gallery to update its display of the frame
	BROADCAST_TO_ALL(LayerMsg(this, LayerMsg::REDRAW_LAYER));
#endif

	return CC_OK;
}

/********************************************************************************************
// Generated Bitmap related functions
********************************************************************************************/

/********************************************************************************************

>	KernelBitmap * Layer::GetGeneratedBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/97
	Returns:	NULL or the kernel bitmap that we generated to represent this layer
	Purpose:	To recover the bitmap that has been generated to represent this layer, if any.

********************************************************************************************/

KernelBitmap * Layer::GetGeneratedBitmap()
{
	// Just ask the embedded bitmap ref to return its current status
	return m_GeneratedBitmap.GetBitmap();
}
		
/********************************************************************************************

>	BOOL Layer::SetGeneratedBitmap(KernelBitmap * pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/97
	Inputs:		The kernel bitmap that we wish to represent this layer
	Purpose:	To set the bitmap that has been generated to represent this layer.

********************************************************************************************/

BOOL Layer::SetGeneratedBitmap(KernelBitmap * pBitmap)
{
	//ERROR2IF(pBitmap == NULL,FALSE,"SetGeneratedBitmap Bad params");

	Document *pParentDoc = (Document *) FindOwnerDoc();
	ERROR2IF(pParentDoc == NULL,FALSE,"SetGeneratedBitmap bad parent doc!");

	// delete the old one from the system, if it is there and it is not used
	// elsewhere in the document
	KernelBitmap * pOldBitmap = m_GeneratedBitmap.GetBitmap();
	if (pOldBitmap)
	{
		// Disclude ourselves from the count
		m_GeneratedBitmap.Detach();
		
		// Should we use HasBeenDeleted???
		// If the bitmap is not used in this document then delete it
		if (!pOldBitmap->IsUsedInDocument(pParentDoc))
			delete pOldBitmap;
		//m_GeneratedBitmap.DeleteBmp();
	}

	// Only do this if we are passed in a non-null bitmap
	if (pBitmap != NULL)
	{
		// Make our bitmap ref use the new specified one
		// We could use the nice and simple:-
		//m_GeneratedBitmap.Attach(pBitmap, pParentDoc);
		// but this makes a new kernel bitmap, which is bad!
		// So do this instead:-
		// Try and add it to the list of bitmaps already present in this document
		BitmapList * pBmpList = pParentDoc->GetBitmapList();
		if (pBmpList == NULL)
		{
			ERROR3("SetGeneratedBitmap No document bitmap ref found!");
			return FALSE; 
		}

		// and then attach the bitmap
		pBitmap->Attach(pBmpList);
		// and tell everybody about the change
		BROADCAST_TO_ALL(BitmapListChangedMsg(pBmpList, pBitmap)); 

		m_GeneratedBitmap.SetBitmap(pBitmap);

		// If we have a generated bitmap then we cannot have a referenced bitmap
		// so reset the referenced bitmap
		SetReferencedBitmap(NULL);
	}

	return TRUE;
}

/********************************************************************************************

>	KernelBitmap *Layer::EnumerateBitmaps(UINT32 Count)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> based on Will code
	Created:	7/5/97
	Inputs:		Count - the bitmap to get (see Purpose).
	Returns:	The KernelBitmap in use by the node, or NULL if no more are used.
	Purpose:	Find out what bitmaps, if any, are used by this node.

				The base class returns NULL always, so you over-ride this in any node classes
				that use bitmaps.

				This function supports nodes that use more than one bitmap - you call this
				function repeatedly and keep incrementing the Count parameter that you pass
				in each time by 1.  You should stop calling it when it returns NULL, as this
				indicates that no more bitmaps are used by this node.
				Count should start off as 0 for the first call.  Note that this function
				can (and often will) return NULL for the first call, as many nodes don't
				use bitmaps, obviously.

	SeeAlso:	KernelBitmap

********************************************************************************************/

KernelBitmap *Layer::EnumerateBitmaps(UINT32 Count)
{
	if (Count == 0) 
		return GetGeneratedBitmap();

	return NULL;
}

/********************************************************************************************
// End of Generated Bitmap related functions
********************************************************************************************/

/********************************************************************************************
// Referenced Bitmap related functions
********************************************************************************************/

/********************************************************************************************

>	KernelBitmap * Layer::GetReferencedBitmap()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/97
	Returns:	NULL or the kernel bitmap that we reference to represent this layer
	Purpose:	To recover the bitmap that is present on the layer and is of the correct sort
				so that it can be used to represent this layer.

********************************************************************************************/

KernelBitmap * Layer::GetReferencedBitmap()
{
	// We have a direct pointer rather than a ref as otherwise we will see the bitmap twice
	// in the bitmap gallery for this document.
	return m_pReferencedBitmap;
}
		
/********************************************************************************************

>	BOOL Layer::SetReferencedBitmap(KernelBitmap * pBitmap)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/7/97
	Inputs:		The kernel bitmap that we wish to represent this layer
	Purpose:	To set the bitmap that has been found which is a single bitmap on this layer
				which is of the correct colour depth and scaling which can be used to directly
				represent this layer. Usually, the bitmap will be part of an animation that
				has been loaded in.

********************************************************************************************/

BOOL Layer::SetReferencedBitmap(KernelBitmap * pBitmap)
{
	// We have a direct pointer rather than a ref as otherwise we will see the bitmap twice
	// in the bitmap gallery for this document.
	m_pReferencedBitmap = pBitmap;

	return TRUE;
}

/********************************************************************************************
// End of Referenced Bitmap related functions
********************************************************************************************/

/***********************************************************************************************

>	BOOL Layer::HasLayerGotRenderableChildren(Layer* pLayer)

  Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com> from MarkN version in LayerSGallery::HasLayerGotChildren()
	Created:	21/7/97
	Returns:	TRUE  - The layer has children the user may be interested in - i.e. children
						that actually render something.
				FALSE - The layer has no rendering children
	Purpose:	Scans the layer for children that render something on the screen.
				It ignores attributes, hidden nodes, etc - nodes that don't render anything themselves.
				
***********************************************************************************************/

BOOL Layer::HasLayerGotRenderableChildren()
{
	Node* pNode = FindFirstChild();
	BOOL ChildFound = FALSE;

	while (pNode != NULL && !ChildFound)
	{
		ChildFound = pNode->IsBounded();
		pNode = pNode->FindNext();
	}

	return (ChildFound);
}

/********************************************************************************************

>	void Layer::SetLayerID((String_256& LayerID)

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/94
	Inputs:		LayerID: The new layer identifier
	Outputs:	-
	Returns:	- 
	Purpose:	For setting the layer's identifier 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void Layer::SetLayerID(String_256& LayerID)
{
	LayerSt.StringLayerID = LayerID; 
} 


/********************************************************************************************

>	void Layer::SetLayerStatus(LayerStatus& Status); 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/1/94
	Inputs:		Status: The new status for the layer 
	Outputs:	-
	Returns:	-
	Purpose:	To set the layers status
	Errors:		-
	SeeAlso:	Layer::SetVisible 
	SeeAlso:	Layer::SetLocked
	SeeAlso:	Layer::SetActive
	SeeAlso:	Layer::SetPrintable
	SeeAlso:	Layer::SetBackground
	SeeAlso:	LAyer::SetGlobalQualityFlg

********************************************************************************************/
 
void Layer::SetLayerStatus(LayerStatus& Status) 
{
	LayerSt = Status; 	
}

void Layer::GetDebugDetails(StringBase* Str) 
{          
#ifdef _DEBUG
	NodeRenderablePaper::GetDebugDetails(Str); 
	(*Str)+= TEXT("\r\nLayer Status("); 
	(*Str)+=TEXT("\r\n   VISIBLE = "); 
	if (Visible)
		(*Str)+=TEXT("TRUE"); 
	else
		(*Str)+=TEXT("FALSE"); 
	
	(*Str)+=TEXT("\r\n   LOCKED = "); 
	if (Locked)
		(*Str)+=TEXT("TRUE"); 
	else
		(*Str)+=TEXT("FALSE"); 

	(*Str)+=TEXT("\r\n   ACTIVE = "); 
	if (Active)
		(*Str)+=TEXT("TRUE"); 
	else
		(*Str)+=TEXT("FALSE"); 
            
	(*Str)+=TEXT("\r\n   PRINTABLE = "); 
	if (Printable)
		(*Str)+=TEXT("TRUE"); 
	else
		(*Str)+=TEXT("FALSE"); 

	(*Str)+=TEXT("\r\n   BACKGROUND = "); 
	if (Background)
		(*Str)+=TEXT("TRUE"); 
	else
		(*Str)+=TEXT("FALSE"); 

	(*Str)+=TEXT("\r\n");
	
	String_256 Temp; 

	Temp._MakeMsg(TEXT("Outline = #1%ld\r\n"),Outline);   
	(*Str)+=Temp; 
	Temp._MakeMsg(TEXT("Guide   = #1%ld\r\n"),Guide);   
	(*Str)+=Temp; 
	Temp._MakeMsg(TEXT("PageBackground   = #1%ld\r\n"),m_PageBackground);   
	(*Str)+=Temp; 
	Temp._MakeMsg(TEXT("Frame   = #1%ld\r\n"),m_Frame);   
	(*Str)+=Temp; 
	Temp._MakeMsg(TEXT("Edited  = #1%ld\r\n"),m_Edited);   
	(*Str)+=Temp; 
	Temp._MakeMsg(TEXT("Hidden frame = #1%ld\r\n"),m_HiddenFrame);   
	(*Str)+=Temp; 

	(*Str)+=TEXT("Layer ID = "); 
	(*Str)+=LayerSt.StringLayerID; 
	(*Str)+=TEXT("\r\n");
#endif
}	

/********************************************************************************************

>	virtual UINT32 Layer::GetNodeSize() const

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/93
	Inputs:		-
	Outputs:	-
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
				
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 Layer::GetNodeSize() const 
{     
	return (sizeof(Layer)); 
}  
 
/********************************************************************************************

>	virtual DocRect Layer::GetBlobBoundingRect()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Inputs:		-
	Outputs:	-
	Returns:	The layer's blob bounds
	Purpose:	Returns the layer's bounding rect inflated by the size of a blob
	SeeAlso:	NodeRenderable::GetBlobBoundingRect()

********************************************************************************************/

DocRect Layer::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	ENSURE(BlobMgr != NULL,"No blob manager - HELP!");

	DocRect rect 	 = GetBoundingRect();
	INT32	BlobSize = BlobMgr->GetBlobSize();

	rect.Inflate(BlobSize);

	return (rect);
#else
	return DocRect(0,0,0,0);
#endif
}

/********************************************************************************************

> void Layer::PreExportRender(RenderRegion* pRegion)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/94
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Outputs data to mark the beginning of a layer
	SeeAlso:	-

********************************************************************************************/

void Layer::PreExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	if(pRegion->IsKindOf(CC_RUNTIME_CLASS(ArtWorksEPSRenderRegion)))
	{
		// export layers in ArtWorks-type EPS render regions

		// Can only export guide layers in Native format
		if (Guide && !pRegion->IS_KIND_OF(NativeRenderRegion))
			return;

		// Get ptr to the export DC
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		// Guide layers are extended objects so mark them as such
		if (Guide)
		{
			pDC->OutputValue(INT32(TAG_GUIDELAYER));	// Output start extended object tag and token
			pDC->OutputToken(_T("cso"));
			pDC->OutputNewLine();
		}

		// Output the layer name
		pDC->OutputString(LayerSt.StringLayerID);

		// Layer type - foreground or background
		if (IsBackground())
			pDC->OutputToken(_T("0"));
		else
			pDC->OutputToken(_T("1"));

		// Visible flag
		if (Visible)
			pDC->OutputToken(_T("1"));
		else
			pDC->OutputToken(_T("0"));

		// Printable flag
		if (IsPrintable())
			pDC->OutputToken(_T("1"));
		else
			pDC->OutputToken(_T("0"));

		// Locked flag
		if (Locked)
			pDC->OutputToken(_T("1"));
		else
			pDC->OutputToken(_T("0"));

		// Layer token itself

		if (!Guide)
			pDC->OutputToken(_T("alyr"));
		else
		{
			// Output guide layer stuff

			if (pGuideColour == NULL)
				SetGuideColour(NULL);

			ERROR3IF(pGuideColour == NULL,"Can't export guide layer without a colour");

			if (pGuideColour != NULL)
				pDC->OutputNamedColour(pGuideColour);

			pDC->OutputToken(_T("glyr"));
		}

		pDC->OutputNewLine();
	}
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		pDC->StartLayer(GetLayerID());
	}
#endif
}

/***********************************************************************************************

>	virtual BOOL Layer::ExportRender(RenderRegion* pRegion)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/10/95
	Inputs:		pRegion = the render region to export to
	Outputs:	-
	Returns:	-   
	Purpose:    Called after all child nodes are asked to export
		
***********************************************************************************************/

BOOL Layer::ExportRender(RenderRegion* pRegion)
{
#ifdef DO_EXPORT
	// Can only export guide layers in Native format
	if (Guide && pRegion->IS_KIND_OF(NativeRenderRegion))
	{
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		pDC->OutputToken(_T("ceo"));
		pDC->OutputNewLine();
	}
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		pDC->EndLayer();

		return TRUE;
	}

	// Graeme (11-4-00) - Added call to AIEPSRenderRegion to export layers in that format.
	else if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( AIEPSRenderRegion ) ) )
	{
		// Cast the render region to get a reference to an AIEPSRenderRegion.
		AIEPSRenderRegion	*pAIEPS	= static_cast<AIEPSRenderRegion*> ( pRegion );

		// Call the export method.
		pAIEPS->ExportLayer ( this );

		// Return TRUE to avoid calling the standard exporter code.
		return TRUE;
	}

#endif

	return FALSE;
}


/********************************************************************************************

> 	virtual BOOL Layer::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState = TRUE,
														BOOL DoPreTriggerEdit = TRUE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com> 20/01/2000
	Created:	6/02/95
	Inputs:		pParam				 = describes the way an op wants to change the node
				SetOpPermissionState = if TRUE the Op permission state of this node will be set according to
										the outcome of the call
				DoPreTriggerEdit	 = layers and the nodes above them are not part of the stretching
									   mechanism, and hence do not use this flag at all.
	Outputs:	-
	Returns:	TRUE always!!
	Purpose:	Layers will allow any op to happen to them, or to their children.

				The main purpose for layers overriding this func is to ensure that layers become the
				top level node in the tree for the op permission mechanism, i.e. parents of layers (spreads, etc)
				shouldn't play a part.

				This func will always return TRUE, and if 'SetOpPermissionState' is TRUE, the layer's op permission
				state will be set to PERMISSION_ALLOWED.

	SeeAlso:	Node::AllowOp(),Node::GetOpPermission(),Node::SetOpPermission(),Layer::SetOpPermission()

********************************************************************************************/

BOOL Layer::AllowOp(ObjChangeParam* pParam, BOOL SetOpPermissionState, BOOL DoPreTriggerEdit)
{
	if (SetOpPermissionState)
		SetOpPermission(PERMISSION_ALLOWED);

	return TRUE;
}

/********************************************************************************************

> 	virtual void Layer::SetOpPermission(OpPermissionState NewState,BOOL AndParents = FALSE)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/02/95
	Inputs:		NewState 	= the state to set the permission to
				AndParents 	= Ignored (see later)
	Outputs:	-
	Returns:	-
	Purpose:	Calls Node::SetOpPermission(NewState,FALSE), i.e. you can set the layer's op permission
				state, but the 'AndParent' param is ignored.

				The purpose of this func is to make layers the top level node that has it's op permission
				state changed.

	SeeAlso:	Node::SetOpPermission();

********************************************************************************************/

void Layer::SetOpPermission(OpPermissionState NewState,BOOL AndParents)
{
	Node::SetOpPermission(NewState,FALSE);
}


/********************************************************************************************

> 	virtual void Layer::EnsureUniqueLayerID(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/04/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Ensures that the layers ID is unique amoung it's siblings. Call after the
				node is linked into the document tree.
	SeeAlso:	Layer::SetLayerID

********************************************************************************************/

void Layer::EnsureUniqueLayerID(void)
{
	// first, find the first sibling of this node
	Layer *pFirstSibling = this;
	Layer *pPrevious = NULL;

	while ((pPrevious = pFirstSibling->FindPrevLayer()) != NULL)
		pFirstSibling = pPrevious;

	// then run though checking to see if the name is unqiue.
	String_256 ID	= GetLayerID();
	INT32 Count		= 1;			// the count of the extra number to give the layer
	Layer *pExamine = NULL;
	BOOL Unique		= FALSE;
	BOOL Changed	= FALSE;		// whether the ID got changed

	do
	{
		// run through the siblings to find out if their ID match this one
		pExamine = pFirstSibling;
		Unique = TRUE;
		
		while (pExamine != NULL)
		{
			// don't fiddle with this layer
			if (pExamine != this)
			{
				// OK, does it match?
				Layer *pLayer = pExamine;
				if (pLayer->GetLayerID() == ID)
				{
					// nope
					Unique = FALSE;
					break;
				}
			}

			// next thing to look at...
			pExamine = pExamine->FindNextLayer();
		}

		// right then, is it unique?
		if (Unique == FALSE)
		{
			// munge up a new layer name
			Count++;

// there is a problem here..
// a) ralph has no string resources
// b) MakeMsg has been bodged so that it never fails
// the result is that ralph sits in this loop forever
// ralph doesn't give a fig about layer names so just get out of here 
#ifdef RALPH
			return;
#endif
			UINT32 format_id = _R(IDT_LAYERNAMEUNIQUEFORM);	// "#1%s (#2%d)" 
			// If its a frame then use a different form of unique formatter without the brackets
			if (IsFrame())
				format_id = _R(IDS_FRAMENAMEUNIQUEFORM);			// "#1%s #2%d"
			
			if (ID.MakeMsg(format_id, (TCHAR *)GetLayerID(), Count) == 0)
				return;		// error getting a string -- give up! (doesn't really matter as the only problem will be the user seeing two layers with the same name)

			Changed = TRUE;

			// and then try again...
		}
	
	} while (Unique == FALSE);
	
	// and if it needs to be changed, change it
	if (Changed)
		SetLayerID(ID);
}


/********************************************************************************************

>	UINT32 Layer::CountNumWritableWebLayers(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	The number of layers that will be written in the web file
	Purpose:	Counts how many layers are writable, as determined by Layer::CanWriteChildrenWeb()

				The count looks at all layers, including this one, in the spread to which this layer belongs

	SeeAlso:	CanWriteChildrenWeb()

********************************************************************************************/

UINT32 Layer::CountNumWritableWebLayers(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
/*****************************************************
See note in BOOL Layer::WritePreChildrenWeb(BaseCamelotFilter* pFilter) for 
a reason why this code is commented out
(markn 12/8/96)

	UINT32 WritableLayerCount = 0;
	Spread* pSpread = (Spread*)FindParent(CC_RUNTIME_CLASS(Spread));
	if (pSpread != NULL)
	{
		Layer* pLayer = pSpread->FindFirstLayer();
		while (pLayer != NULL)
		{
			if (pLayer->CanWriteChildrenWeb(pFilter))
				WritableLayerCount++;
			pLayer = pLayer->FindNextLayer();
		}
	}

	return WritableLayerCount;
******************************************************/

	return 0;
#else
	return 0;
#endif
}

/********************************************************************************************

> 	BOOL Layer::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Writes out the layer data suitable for the web format

				Web files only need to write out the basic layer record - no other info is needed.

				NOTE:	See Layer::CanWriteChildrenWeb() header for the rules that determine
						whether a layer is actually written or not

	SeeAlso:	CanWriteChildrenWeb()

********************************************************************************************/

BOOL Layer::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// If the layer doesn't want the children to be written out, then don't write out the layer record
	if (!CanWriteChildrenWeb(pFilter))
		return FALSE;

/***************************************
Originally, .web files would not write out a layer node if there was only one layer
i.e. the layer could be implied, hence saving space.

However, writing the layer node out has benefits when importing .web files.  It means we can easily
make sure undo import works, plus we can cope with redraw problems easier.

So single-layer docs saved out in the new format will contain a layer node.
(markn 12/8/96)

-	UINT32 WritableLayerCount = CountNumWritableWebLayers(pFilter);
-	ERROR3IF(WritableLayerCount == 0,"WritableLayerCount should be at least 1");
-
-	// If there aren't 2 or more writable layers, then don't bother 
-	if (WritableLayerCount < 2)
-		return FALSE;
*****************************************/

	CamelotFileRecord Rec(pFilter,TAG_LAYER,TAG_LAYER_SIZE);
	return (pFilter->Write(&Rec) != 0);

#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL Layer::WritePreChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Writes out the layer data suitable for the native format

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Layer::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// If the layer doesn't want the children to be written out, then don't write out the layer record
	if (!CanWriteChildrenNative(pFilter))
		return FALSE;

	// Always write out the layer record in the native format
	CamelotFileRecord Rec(pFilter,TAG_LAYER,TAG_LAYER_SIZE);
	return (pFilter->Write(&Rec) != 0);

#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL Layer::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE if it's ok to write out the layer's children
				FALSE otherwise
	Purpose:	Determines if the layer should write out its contents in the web format

				Web files will not write out the following types of layer:
					Invisible layers
					Layers than contain no renderable objects
					Guide layers

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Layer::CanWriteChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	// If not visible and not a animated GIF frame layer, don't bother
	if (!IsVisible() && !IsFrame())
		return FALSE;

	// If a guide layer, don't bother
	if (IsGuide())
		return FALSE;

	// If no renderable objects on the layer and not a animated GIF frame layer, don't bother
	if (!IsFrame())
	{
		DocRect Rect = GetBoundingRect();
		if (Rect.IsEmpty())
			return FALSE;
	}

	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL Layer::CanWriteChildrenNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/7/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE if it's ok to write out the layer's children
				FALSE otherwise
	Purpose:	Determines if the layer should write out its contents in the native format

				Native files will not write out layers when:
					It's actually the cack compact format AND
					The user has selected "Remove invisible layers" AND
					The layer is invisible
					The layer is NOT a frame layer

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Layer::CanWriteChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT

	// if we are saving in the compact format, and
	// the user has selected to remove invisible layers & the layer is invisible,
	// then don't let the children go out
	if (pFilter->IsCompactNativeFilter())
	{
		// If user doesn't want visible layers, and it's not visible and it's not a animated GIF frame layer, don't bother
		if (pFilter->GetRemoveInvisibleLayers() && !IsVisible() && !IsFrame())
			return FALSE;
	}

	return Node::CanWriteChildrenNative(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL Layer::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Begins the child record sequence for layers in the native format

				The native format writes out all layers and includes the layer details record 
				as the layer's first child record

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Layer::WriteBeginChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// First thing to do is write out the Down record
	BOOL ok = (pFilter->WriteZeroSizedRecord(TAG_DOWN));

	// The colour ref used in the guide layer info record
	INT32 ColRef = 0;

	// Firstly, if it's a guide layer write out the colour definition
	if (ok && IsGuide())
	{
		DocColour* pColour = GetGuideColour();

		if (pColour != NULL)
			ColRef = pFilter->WriteRecord(pColour);

		if (ColRef == 0)
		{
			pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));
			ok = FALSE;
		}
	}

	if (ok)
	{
		// Assume it's a normal layer (i.e. not a guide layer)
		UINT32 Tag  = TAG_LAYERDETAILS;
		INT32  Size = TAG_LAYERDETAILS_SIZE;

		// If it's a guide layer, change the tag & size values
		if (IsGuide())
		{
			Tag  = TAG_GUIDELAYERDETAILS;
			Size = TAG_GUIDELAYERDETAILS_SIZE;
		}

		// Build the flags byte
		BYTE Flags = 0;
		if (GetVisibleFlagState())	Flags |= TAG_LAYER_FLAG_VISIBLE;
		if (GetLockedFlagState())	Flags |= TAG_LAYER_FLAG_LOCKED;
		if (IsPrintable())			Flags |= TAG_LAYER_FLAG_PRINTABLE;
		if (IsActive())				Flags |= TAG_LAYER_FLAG_ACTIVE;
		// new 4/4/97
		if (IsPageBackground())		Flags |= TAG_LAYER_FLAG_PAGEBACKGROUND;
		if (IsBackground())			Flags |= TAG_LAYER_FLAG_BACKGROUND;

		// Create a record and write to it
		CamelotFileRecord Rec(pFilter,Tag,Size);
		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteBYTE(Flags);
		if (ok) ok = Rec.WriteUnicode(GetLayerID());

		// Make sure the guide layer details include the colour ref
		if (ok && Tag == TAG_GUIDELAYERDETAILS)
			ok = Rec.WriteReference(ColRef);

		// Write the details record to the file.
		ok = (pFilter->Write(&Rec) != 0);

		// Only write out the frame properties if it is a frame layer
		if (ok && IsFrame()) ok = WriteFrameLayerProperties(pFilter);

		ok = WriteAtomicNodesColourRefs (this, pFilter);
	}

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************

> 	BOOL Layer::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/8/96
	Inputs:		pFilter = ptr to the filter to write to
	Outputs:	-
	Returns:	TRUE if the node has written out a record to the filter
				FALSE otherwise
	Purpose:	Ends the child record sequence for layers in the native format

				The native format writes out all layers.  This func just writes out the Up record

	SeeAlso:	WritePreChildrenNative()

********************************************************************************************/

BOOL Layer::WriteEndChildRecordsNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	return pFilter->WriteZeroSizedRecord(TAG_UP);
#else
	return FALSE;
#endif
}


//-------------------------------------------------------------------
// These stubs have been put in so that layer.h doesn't have to get touched if they need
// implementing.

BOOL Layer::WritePostChildrenWeb(BaseCamelotFilter* pFilter)
{
	return Node::WritePostChildrenWeb(pFilter);
}

BOOL Layer::WritePostChildrenNative(BaseCamelotFilter* pFilter)
{
	return Node::WritePostChildrenNative(pFilter);
}

BOOL Layer::WriteBeginChildRecordsWeb(BaseCamelotFilter* pFilter)
{
/*****************************************************
See note in BOOL Layer::WritePreChildrenWeb(BaseCamelotFilter* pFilter) for 
a reason why this code is commented out
(markn 12/8/96)

-	UINT32 WritableLayerCount = CountNumWritableWebLayers(pFilter);
-
-	if (WritableLayerCount < 2)
-		return TRUE;
******************************************************/

	return Node::WriteBeginChildRecordsWeb(pFilter);
}

BOOL Layer::WriteEndChildRecordsWeb(BaseCamelotFilter* pFilter)
{
/*****************************************************
See note in BOOL Layer::WritePreChildrenWeb(BaseCamelotFilter* pFilter) for 
a reason why this code is commented out
(markn 12/8/96)

-	UINT32 WritableLayerCount = CountNumWritableWebLayers(pFilter);
-
-	if (WritableLayerCount < 2)
-		return TRUE;
******************************************************/

	return Node::WriteEndChildRecordsWeb(pFilter);
}

BOOL Layer::ReadPostChildrenWeb(BaseCamelotFilter* pFilter)
{
	if (pFilter != NULL)
		pFilter->InvalidateRegion(this);

	return Node::ReadPostChildrenWeb(pFilter);
}

BOOL Layer::ReadPostChildrenNative(BaseCamelotFilter* pFilter)
{
	if (pFilter != NULL)
		pFilter->InvalidateRegion(this);

	return Node::ReadPostChildrenNative(pFilter);
}


/********************************************************************************************

> 	DocColour* Layer::GetGuideColour()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		-
	Outputs:	-
	Returns:	ptr to the DocColour attached to this layer
	Purpose:	Returns the DocColour that specifies the colour to render the guide objects
				in this layer with.
	SeeAlso:	SetGuideColour()

********************************************************************************************/

DocColour* Layer::GetGuideColour()
{
	if (pGuideColour == NULL)
		SetGuideColour(NULL);

	return (pGuideColour);
}

/********************************************************************************************

>	void Layer::SetGuideColour(IndexedColour* pIndexedColour)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pIndexedColour = ptr to IndexedColour (NULL means set to default)
	Outputs:	-
	Returns:	-
	Purpose:	Assigns the given indexed colour as the colour to render guide objects in this layer in.
	SeeAlso:	GetGuideColour()

********************************************************************************************/

void Layer::SetGuideColour(IndexedColour* pIndexedColour)
{
//#pragma message( __LOCMSG__ "Layer::SetGuideColour - do nothing" )
//	TRACE( _T("Warning - Layer::SetGuideColour called\n") );
	if (pGuideColour == NULL)
		pGuideColour = new DocColour();

	if (pGuideColour != NULL)
	{
		if (pIndexedColour == NULL)
		{
			ColourValue r = ColourValue(double(GuideLayerRed)   / double(COL_MAX));
			ColourValue g = ColourValue(double(GuideLayerGreen) / double(COL_MAX));
			ColourValue b = ColourValue(double(GuideLayerBlue)  / double(COL_MAX));
//#pragma message( __LOCMSG__ "Removed IndexedColour usage" )
			pIndexedColour = new IndexedColour(r,g,b);
			pIndexedGuideColour = pIndexedColour;
		}

		if (pIndexedColour != NULL)
			pGuideColour->MakeRefToIndexedColour(pIndexedColour);
	}
}

/********************************************************************************************

>	void Layer::MakeGuideColourDefault()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Makes the current colour the default for all guide layers
	SeeAlso:	GetGuideColour(),SetGuideColour()

********************************************************************************************/

void Layer::MakeGuideColourDefault()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pGuideColour != NULL)
	{
		INT32 Red,Green,Blue;
		pGuideColour->GetRGBValue(&Red,&Green,&Blue);
		GuideLayerRed   = INT32(Red);
		GuideLayerGreen = INT32(Green);
		GuideLayerBlue  = INT32(Blue);
	}
#endif
}

/********************************************************************************************

>	DocColour Layer::GetDefaultGuideColour()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		-
	Outputs:	-
	Returns:	The DocColour that represents the default colour settings
	Purpose:	Makes the current colour the default for all guide layers
	SeeAlso:	GetGuideColour(),SetGuideColour()

********************************************************************************************/

DocColour Layer::GetDefaultGuideColour()
{
#if !defined(EXCLUDE_FROM_RALPH)
	ColourValue r = ColourValue(double(GuideLayerRed)   / double(COL_MAX));
	ColourValue g = ColourValue(double(GuideLayerGreen) / double(COL_MAX));
	ColourValue b = ColourValue(double(GuideLayerBlue)  / double(COL_MAX));

	DocColour Col(r,g,b);

	return Col;
#else
	return DocColour(0,0,0);
#endif
}

/********************************************************************************************

>	void Layer::ColourChanged(IndexedColour* pChangedColour)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		pChangedColour = ptr to the colour that's just changed
	Outputs:	-
	Returns:	-
	Purpose:	Called when a colour has changed.
	SeeAlso:	ColourDeleted()

********************************************************************************************/

void Layer::ColourChanged(IndexedColour* pChangedColour)
{
// WEBSTER - markn 15/1/97
// Don't need this in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH)
	if (pChangedColour == NULL || pGuideColour == NULL)
		return;

	IndexedColour* pColour = pGuideColour->FindParentIndexedColour();

	if (pChangedColour == pColour)
	{
PORTNOTE("dialog","Removed LayerGallery usage");
#ifndef EXCLUDE_FROM_XARALX
		Document* pDoc = Document::GetCurrent();
		if (pDoc != NULL)
			LayerSGallery::ForceRedrawLayer(pDoc,this);
#endif
	}
#endif
#endif // WEBSTER
}

/********************************************************************************************

>	void Layer::ColourDeleted()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when a colour may have been deleted.
	SeeAlso:	ColourChanged()

********************************************************************************************/

void Layer::ColourDeleted()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pGuideColour != NULL)
	{
		IndexedColour* pIndexedColour = pGuideColour->FindParentIndexedColour();

		if (pIndexedColour != NULL && pIndexedColour->IsDeleted())
		{
			ColourGeneric ColGen;
			pIndexedColour->GetSourceColour(&ColGen);
			ColourModel Model = pIndexedColour->GetColourModel();

			IndexedColour* pNewColour = new IndexedColour(Model,&ColGen);

			SetGuideColour(pNewColour);
//			Document* pDoc = Document::GetCurrent();
//			if (pDoc != NULL)
//				LayerSGallery::ForceRedrawLayer(pDoc,this);
		}
	}
#endif
}

/***********************************************************************************************

>	static Layer* Layer::CreateGuideLayer()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	Ptr to a guide layer that's ready to be inserted into the tree
	Purpose:    This does everything required in creating a layer fit to be called the guide layer

				// WEBSTER - markn - 15/1/97
				// Moved from OpGuideLine for WEBSTER purposes, but it should be in this class anyway
	SeeAlso:	-
		
***********************************************************************************************/

Layer* Layer::CreateGuideLayer()
{
	Layer* pNewLayer = new Layer;
	if (pNewLayer != NULL)
	{
		String_256 LayerName(_R(IDS_GUIDESLAYERNAME));

		pNewLayer->SetVisible(TRUE); 
		pNewLayer->SetLocked(FALSE); 
		pNewLayer->SetPrintable(FALSE); 
//		pNewLayer->SetBackground(TRUE);
		pNewLayer->SetBackground(FALSE);	// make sure Guides are in the "normal" foreground section
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

/********************************************************************************************

>	BOOL Layer::WriteFrameLayerProperties(BaseCamelotFilter* pFilter)

	Author:		Ranbir_Rana (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/5/97
	Input:		pFilter = ptr to the filter to export to.
	Outputs:	-
	Returns:	TRUE if ok, FALSE otherwise.
	Purpose:	Exports the delay details for this frame layer to the filter.
		
********************************************************************************************/

BOOL Layer::WriteFrameLayerProperties(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL RecordWritten = FALSE;
	BOOL ok = TRUE;

	// Add a description of the TAG_LAYER_FRAMEPROPS record, for older importers that don't understand this record
	pFilter->AddTagDescription(TAG_LAYER_FRAMEPROPS, _R(IDS_TAG_LAYER_FRAMEPROPS));

	// Start a record to save our data in
	CXaraFileRecord Rec(TAG_LAYER_FRAMEPROPS, TAG_LAYER_FRAMEPROPS_SIZE);
	// Get the Frame delay inforamtion.
	DWORD Delay = GetFrameDelay();
	BYTE Flags = 0;

	if (IsSolid())			Flags |= TAG_LAYER_FLAG_SOLID;
	if (IsOverlay())		Flags |= TAG_LAYER_FLAG_OVERLAY;
	if (IsHiddenFrame())	Flags |= TAG_LAYER_FLAG_HIDDEN;
		
	ok = Rec.Init();

	if (ok)
		ok = Rec.WriteUINT32(Delay);	

	if (ok)
		ok = Rec.WriteBYTE(Flags);	

	// Write out the record.
	if (ok) ok = (pFilter->Write(&Rec) != 0);
	if (ok)
		RecordWritten = TRUE;
	else
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return RecordWritten;
#else
	return FALSE;	
#endif 

};

/********************************************************************************************

>	PathName Layer::MakeExportLayerName(const PathName& path)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/03/2004
	Input:		PathName - original pathname
	Outputs:	-
	Returns:	PathName - modified pathname
	Purpose:	Make a pathname that's specific to this layer
		
********************************************************************************************/

PathName Layer::MakeExportLayerName(const PathName& path)
{
	PathName pathLayer = path;

	String_256 filename = path.GetFileName(FALSE);
	filename += String( _T(" ") );
//	if (LayerSt.StringLayerID.SubWithoutCase("Layer")==-1)
//		filename += String("Layer ");
	filename += LayerSt.StringLayerID;
	pathLayer.SetFileName(filename);

	return pathLayer;
};

//-----------------------------------------------------
//-----------------------------------------------------
//-----------------------------------------------------

/********************************************************************************************

>	LayerMsgHandler::LayerMsgHandler()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Purpose:	Constructs a LayerMsgHandler, an object which handles messages for layers
	Errors:		-
	SeeAlso:	MessageHandler

********************************************************************************************/

LayerMsgHandler::LayerMsgHandler()
  : MessageHandler(CC_RUNTIME_CLASS(MessageHandler), TRUE)
{
}

/********************************************************************************************

>	virtual MsgResult LayerMsgHandler::Message(Msg* Message)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/10/95
	Inputs:		Message: The message

	Purpose:	Process messages on behalf of layers

********************************************************************************************/

MsgResult LayerMsgHandler::Message(Msg* Message)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (MESSAGE_IS_A(Message, ColourChangingMsg))
	{
		ColourChangingMsg *Msg = (ColourChangingMsg *) Message;
		switch ( Msg->State )
		{
			case ColourChangingMsg::LISTDELETED:
			case ColourChangingMsg::LISTDESELECTED:
			case ColourChangingMsg::LISTPAGED:
			case ColourChangingMsg::LISTUPDATED:
				if (Msg->ScopeDoc != NULL)
				{
					Spread* pSpread = Msg->ScopeDoc->FindFirstSpread();
					if (pSpread != NULL)
					{
						Layer* pGuideLayer = pSpread->FindFirstGuideLayer();
						if (pGuideLayer != NULL)
							pGuideLayer->ColourDeleted();
					}
				}				
				break;

			case ColourChangingMsg::COLOURUPDATED:
				if (Msg->ScopeDoc != NULL)
				{
					Spread* pSpread = Msg->ScopeDoc->FindFirstSpread();
					if (pSpread != NULL)
					{
						Layer* pGuideLayer = pSpread->FindFirstGuideLayer();
						if (pGuideLayer != NULL)
							pGuideLayer->ColourChanged(Msg->ChangedColour);
					}
				}				
				break;
			default:
				break;
		}
	}
#endif
	return OK; 
}



/********************************************************************************************

  >	virtual BOOL ShouldWriteColourDefinitions (Node* pNode)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pNode = ptr to node
	Returns:	TRUE if record should be written, FALSE if not
	Purpose:	Determins whether colour definitions should be written for this node
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL ShouldWriteColourDefinitions (Node* pNode)
{
	CCRuntimeClass * pClass = CC_RUNTIME_CLASS(NodeBevelController);
	BOOL done = FALSE;
	INT32 i = 0;

	while (!done)
	{
		Node* pParent = pNode->FindParent (pClass);

		if (pParent)
		{
			return (TRUE);
		}

		switch (i)
		{
			case 0:
				pClass = CC_RUNTIME_CLASS(NodeContourController);
			break;
			case 1:
				pClass = CC_RUNTIME_CLASS(NodeShadowController);
			break;
			case 2:
				pClass = CC_RUNTIME_CLASS(NodeClipViewController);
			break;
			default:
				done = TRUE;
		}

		i++;
	}
	return (FALSE);
}



/********************************************************************************************

>	BOOL Layer::WriteAtomicNodesColourRefs (Node* pLayer, BaseCamelotFilter* pFilter)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if written, FALSE otherwise
	Purpose:	All compound nodes (e.g.  bevels, contours, shadows, clipview) are now defined
				as being atomic.  This is so that they can be backwards compatible with CX2.
				This means that colour definitions that occur within these nodes are omitted
				when CX2 reads the files.  The cause access violations when another colour
				definition references the omitted.  Thus, compund atomic nodes MUST write
				colour definitions BEFORE the node is written.  The function scans the document
				for relevent nodes, and cause them to write out their colour definitions
				immediately after the layer node.  This is only done of compound atomic nodes,
				since doing it for all would slow down other peoples distributed stuff.
	Errors:		-
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()
				AttrFillGeometry::WriteColourDefinitions ()

********************************************************************************************/

BOOL Layer::WriteAtomicNodesColourRefs (Node* pLayer, BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;

	CCRuntimeClass * pClass = CC_RUNTIME_CLASS(AttrFlatColourFill);
	BOOL done = FALSE;
	INT32 i = 0;

	while (!done)
	{
		AttrFillGeometry* pNode = (AttrFillGeometry*) SliceHelper::FindNextOfClass(pLayer, pLayer, pClass);

		while (pNode)
		{
			if (ShouldWriteColourDefinitions (pNode))
			{
				ok = pNode->WriteColourDefinitions (pFilter);
			}
			pNode = (AttrFillGeometry*) SliceHelper::FindNextOfClass(pNode, pLayer, pClass);
		}
		if (!ok) done = TRUE;
		switch (i)
		{
			case 0:
				pClass = CC_RUNTIME_CLASS(AttrLinearColourFill);
			break;
			case 1:
				pClass = CC_RUNTIME_CLASS(AttrRadialColourFill);
			break;
			case 2:
				pClass = CC_RUNTIME_CLASS(AttrConicalColourFill);
			break;
			case 3:
				pClass = CC_RUNTIME_CLASS(AttrSquareColourFill);
			break;
			case 4:
				pClass = CC_RUNTIME_CLASS(AttrThreeColColourFill);
			break;
			case 5:
				pClass = CC_RUNTIME_CLASS(AttrFourColColourFill);
			break;
			case 6:
				pClass = CC_RUNTIME_CLASS(AttrBitmapColourFill);
			break;
			case 7:
				pClass = CC_RUNTIME_CLASS(AttrFractalColourFill);
			break;
			case 8:
				pClass = CC_RUNTIME_CLASS(AttrNoiseColourFill);
			break;
			case 9:
				pClass = CC_RUNTIME_CLASS(AttrStrokeColour);
			break;
			default:
				done = TRUE;
		}

		i++;
	}

	return (ok);
}
