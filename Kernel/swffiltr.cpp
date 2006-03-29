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
/* $Header: /Camelot/kernel/swffiltr.cpp 12    1/12/04 17:20 Phil $ */

#include "camtypes.h"	// Camelot data types.
#include "page.h"		// For Page.
#include "ops.h"		// For Operation.
#include "selop.h"		// For SelOperation.
#include "ccfile.h"		// For CCLexFile.
#include "document.h"	// For Document, DocCoord.
#include "pathname.h"	// For PathName.
#include "spread.h"		// For Spread.
#include "doccolor.h"	// For colours.
#include "progress.h"	// For progress functions.
//#include "nev.h"		// Resources for error messages.
#include "nodeshad.h"	// For NodeShadow.
#include "userattr.h"	// Needed for TemplateAttribute.
#include "cxfrech.h"	// Needed for TemplateAttribute.
#include "tmpltatr.h"	// For TemplateAttribute.

#include "oilfltrs.h"	// For FlashOILFilter.
#include "fillattr.h"	// Filter attributes.
//#include "filtrres.h"	// Resources for this class.
#include "bitfilt.h"	// For static drawing size function.
//#include "simon.h"		// Button name resources.

#include "swfrndr.h"	// Definition of FlashRenderRegion.
#include "swfexpdc.h"	// Declaration of FlashExportDC.
#include "swftext.h"	// Definition of FlashFormatText class.
#include "swfplace.h"	// For FlashPlaceObject.

#include "swffiltr.h"	// Definition of this class.

CC_IMPLEMENT_DYNAMIC ( FlashFilter, VectorFilter )

#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL FlashFilter::FlashFilter()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Returns:	Void.
	Purpose:	Filter constructor, and initialises the default settings.

********************************************************************************************/

FlashFilter::FlashFilter()
{
	// Set up filter descriptions.
	FilterName.Load(_R(IDT_FLASH_FILTERNAME));
	FilterInfo.Load(_R(IDT_FLASH_FILTERINFO));
	FilterID = FILTERID_FLASH;

	ExportMsgID = _R(IDT_EXPORTMSG_FLASH);

	Flags.CanImport = FALSE;
	Flags.CanExport = TRUE;

	mpBackgroundNode = NULL;
	mpBackgroundCol = NULL;

#if 0
	// No attributes yet
	DefLineWidthAttr = NULL;
	DefLineColAttr = NULL;
	DefFillColAttr = NULL;
#endif
}

/********************************************************************************************

>	BOOL FlashFilter::Init()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Returns:	TRUE if Filter initialised, otherwise FALSE.
	Purpose:	Initialise the filter (attaches a FlashOILFilter object).
	SeeAlso:	FlashOILFilter

********************************************************************************************/

BOOL FlashFilter::Init ( void )
{
	// Get the OILFilter object
	pOILFilter = new FlashOILFilter(this);
	if (pOILFilter == NULL)
		return FALSE;

	mOnlySelected = FALSE;

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL FlashFilter::DoImport ( SelOperation *Op,
								 CCLexFile* pFile, 
							 	 Document *DestDoc,
								 BOOL AutoChosen,
								 ImportPosition *Pos,
								 KernelBitmap** ppImportedBitmap,
								 DocCoord* pPosTranslate,
								 String_256* URL )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Returns:	FALSE.
	Purpose:	Place holder function for Flash import. Since the filter only exports, a
				false value is always returned.
	SeeAlso:	DoExport

********************************************************************************************/

BOOL FlashFilter::DoImport ( SelOperation *Op,
							 CCLexFile* pFile, 
						 	 Document *DestDoc,
							 BOOL AutoChosen,
							 ImportPosition *Pos,
							 KernelBitmap** ppImportedBitmap,
							 DocCoord* pPosTranslate,
							 String_256* URL )
{
	// Import operations aren't supported. Therefore return FALSE.
	return FALSE;
}

/********************************************************************************************

>	BOOL FlashFilter::DoExport ( Operation* pOp,
								 CCLexFile* pFile,
								 PathName* pPath, 
								 Document* TheDocument,
								 BOOL ShowOptions)

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/8/99
	Returns:	TRUE if export succeeds, otherwise FALSE.
	Purpose:	Place holder function for Flash export until real function is done.
	SeeAlso:	DoImport

********************************************************************************************/

BOOL FlashFilter::DoExport ( Operation* pOp,
							 CCLexFile* pFile,
							 PathName* pPath, 
							 Document* pDoc,
							 BOOL ShowOptions )
{
	Spread				*pSpread	= GetFirstSpread ( pDoc );			// First spread.
	FlashRenderRegion	SWFRegion;										// Render region.
	FlashExportDC		ExDC		( this );							// Export DC.
	View				*pView		= (View*) pDoc->GetFirstDocView ();	// Use current view.

	// Reset the static variables used to maintain the count of the number of Flash objects
	// exported of the various types. Without this, Camelot gets a bit confused, and the
	// export doesn't quite work.
	FlashPlaceObject::ResetStatics ();
	
	// Get a reference to the selected nodes.
	Range				Selection	( * ( GetApplication ()->FindSelection () ) );

	// If a selection has been made, only export the selected nodes. This if statement
	// sets up the local and global variables for either of the options.
	if ( Selection.FindFirst () != NULL )
	{
		// A selection has been made, so don't render everything in the drawing.
		mOnlySelected = TRUE;
		mPageRect	= Selection.GetBoundingRect ();
	}
	else
	{
		// Export all nodes.
		mOnlySelected = FALSE;

		// Use a static method in the bitmap filter to extract the size of the drawing to
		// render to the file.
		mPageRect = BaseBitmapFilter::GetSizeOfDrawing ( pSpread );
	}

	// Get the background colour.

	DocColour BackgroundCol = GetPageColour ( pSpread, &mpBackgroundNode );
	mpBackgroundCol = &BackgroundCol;

	// Set up the origin of the bounding rectangle according to the Flash file specification.
	DocCoord			Origin		( mPageRect.lox, mPageRect.hiy );		// Flash's lox, loy.

	// Initialise the render region.
	SWFRegion.Init ();
	SWFRegion.SetBackgroundColour (BackgroundCol);

	// Create an ExportDC within the render region.
	SWFRegion.AttachDevice ( pView, &ExDC, pSpread );

	// Get the position of the first page, and set as origin, and convert to SWF co-ords.
	ExDC.SetOrigin ( Origin );

	// Ensure file is open:	- If closed, attempt to open it.
	//						- If unopenable, return error.
	if ( !pFile->isOpen () )
	{
		if ( pFile->IsKindOf ( CC_RUNTIME_CLASS ( CCDiskFile ) ) )
		{
			if ( !OpenExportFile ( ( CCDiskFile* ) pFile, pPath ) )
				return FALSE;
		}
		else
		{
			TRACEUSER ( "Graeme",
						"Tried to open non-CCDiskFile in FlashFilter::DoExport\n" );
			return FALSE;
		}
	}

	// Initialise the export DC with the file.
	ExDC.Init ( pFile );

	// Export Render the tree using the base Filter class method.
	ExportRender ( &SWFRegion );

	// Wrap up the file. Tasks to be done are:
	// (1) Write end of frame.
	// (2) End the file.
	// (3) Go back and plug holes in the header struct. (Size of file.)
	ExDC.EndFile ();

	return TRUE;
}

void FlashFilter::CleanUpAfterExport( BOOL Success )
{
	mpBackgroundNode = NULL;
	mpBackgroundCol = NULL;
}


/********************************************************************************************

>	DocColour FlashFilter::GetPageColour ( Spread *pSpread )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/08/99
	Inputs:		pSpread - Pointer to the current spread.
	Returns:	DocColour - the colour of the page background.
	Purpose:	Used to get the page colour.
	Errors:		-
	SeeAlso:	Page::GetPageColour()

********************************************************************************************/

DocColour FlashFilter::GetPageColour ( Spread* pSpread,
									   Node** pBackground )
{
	// Calculate the background colour
	DocColour dcol ( COLOUR_WHITE );		// Default to white.
	*pBackground = NULL;

	Layer *pLayer = pSpread->FindFirstPageBackgroundLayer();
	if (pLayer)
	{
		Node * pNode = pLayer->FindFirstChild();
		while (pNode && !pNode->IsLayer())
		{
			if (!pNode->IsNodeHidden())
			{
				if (pNode->IsARegularShape())
				{
					Node *pAttrNode = pNode->FindFirstChild();
					if ( pAttrNode && pAttrNode->IsAnAttribute()
						 && pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrFlatFill)))
					{
						dcol = *(((AttrFlatFill *)pAttrNode)->GetStartColour());
						*pBackground = pNode;
					}
				}
			}
			pNode = pNode->FindNext();
		}
	}

	return dcol;
}

/********************************************************************************************

>	virtual BOOL FlashFilter::WriteNodes ( RenderRegion *pRegion,
										   ExportDC *pDC,
										   BOOL VisibleLayersOnly,
										   BOOL CheckSelected,
										   BOOL ShowProgress )
	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/9/99
	Inputs:		pRegion 				- The render region to export to.
				pDc						- Device context to use, can be NULL.
				VisibleLayersOnly		- Use visible layers or not
				CheckSelected			- Only export selected nodes.
				ShowProgress			- TRUE then start up a progress bar or FALSE assume
										  the caller has done it.
	Returns:	TRUE if the export process completed successfully, FALSE if an error occured.
	Purpose:	This is a modified version of the base Filter class' ExportRenderNodes, and
				as a result contains Flash specific code. The only real modification has been
				to jump past any nodes in the background layer, which Flash doesn't render.
	SeeAlso:	Filter::ExportRenderNodes

********************************************************************************************/

BOOL FlashFilter::WriteNodes ( RenderRegion *pRegion,
							   ExportDC *pDC,
							   BOOL VisibleLayersOnly,
							   BOOL CheckSelected,
							   BOOL ShowProgress )
{
	// Loop through the tree as usual.
	// while ( Nodes Left to Export )

	// if ( Node is a TextStory )
	// Create a text record.
	// Create a font record if that font hasn't been defined yet.
	// Create a place object tag.

	// else
	// Create a path record.
	// If the path is identical to a previous path, don't store it.
	// If the fill is a bitmap fill, make a bitmap record.
	// Write a place object tag out.

#ifdef DO_EXPORT

	// Export the file, but catch any file errors.
	TRY
	{
		// This function parses through the Camelot tree in common with the other export
		// loops. Unlike the other loops, it builds up four linked lists as dictionaries
		// of shapes, fonts, and bitmaps, which are then rendered into a file by the
		// ExportAll () call at the end of the loop.

		// Future work: Add functions to compare strings and paths to ensure that repeated
		// objects or strings are stored once, and invoked several times, rather than
		// writing multiple instances to the file.

		// Create a FlashRenderRegion and FlashExportDC pointers by casting the stock ones.
		FlashExportDC		*pFDC	= ( FlashExportDC* ) pRegion->GetRenderDC ();
		FlashRenderRegion	*pFRR	= ( FlashRenderRegion* ) pRegion;

		// Find the first node that we should export from this spread
		Spread *pSpread = pRegion->GetRenderSpread ();
		Node *pLayer = ( Node * ) pSpread->FindFirstLayer ();
/*
		Node *pNode = pSpread->FindFirstForExport( pRegion, TRUE, VisibleLayersOnly, CheckSelected );

		if ( mpBackground != NULL && pNode == mpBackground )
		{
			// Move onto the next node.
			pNode = pNode->FindNextForExport( pRegion, TRUE, VisibleLayersOnly, CheckSelected );
		}

		// Create file header, and write it.
		pFDC->CreateHeader ( mBackground, mPageRect );

		while ( pNode != NULL )
		{
			// Declare variables.
			LayerState	ThisLayer	= GetButtonState ( pNode );
			String_256	ButtonName;

			// Limit the progress display to legal values.
			if ( pNode->IsNodeRenderableClass() )
			{
				CurrentProgressLimit = NumNodes + ( ( NodeRenderable * ) pNode )
										-> GetSizeOfExport ( this );
			}

			// Set the layer state.
			pFRR->SetLayerState ( ThisLayer );

			// Get the layer ID, so that we can tell if it's a button or not.
			if ( ThisLayer != NO_BUTTON && GetButtonName ( pNode, ButtonName ) )
			{
				// The name has been successfully extracted, and is passed into the render
				// region.
				pFRR->SetButtonName ( ( TCHAR* ) ButtonName );
			}
			else
			{
				// A NULL pointer in this function instructs the RenderRegion to treat the
				// next node as an ordinary shape, rather than a button.
				pFRR->SetButtonName ( NULL );
			}

			// Attempt to render the node using custom export code.
			if ( !pNode->ExportRender ( pRegion ) )
			{
				// No special export rendering - just use the normal rendering
				pNode->Render ( pRegion );		
			}

			NumNodes = CurrentProgressLimit;		// Update progress counter.

			if ( NumNodes > ( LastExportProgressUpdate + UpdateEvery ) )
			{
				LastExportProgressUpdate = NumNodes;

				// If we are rendering in strips then we need to add an offset to
				// the position of the progress bar.
				if ( !ContinueSlowJob ( ProgressOffset + NumNodes ) )
				{
					// Error; close down and exit nicely.
					// Must set an error otherwise we will get the dreaded reporting error
					// when none set message.
					// If no error message ID set in this filter then use default
					if ( StopExportMsgID == 0 )
						Error::SetError ( _R(IDW_CANCELEXPORT) );
					else
						Error::SetError ( StopExportMsgID );
					pRegion->StopRender ();
					if ( ShowProgress )
						EndSlowJob ();
					return FALSE;
				}
			}

			// Find another one to export
			pNode = pNode->FindNextForExport ( pRegion, TRUE, VisibleLayersOnly,
											   CheckSelected );

			if ( mpBackground != NULL && pNode == mpBackground )
			{
				// Move onto the next node.
				pNode = pNode->FindNextForExport ( pRegion, TRUE, VisibleLayersOnly,
												   CheckSelected );
			}
		}
*/
		// Create file header, and write it.
		pFDC->CreateHeader(*mpBackgroundCol, mPageRect);

		FlashRenderCallback MyCallback(this, pFRR, VisibleLayersOnly, CheckSelected, mpBackgroundNode);
		pRegion->RenderTree(pSpread, FALSE, FALSE, &MyCallback);

		// Call the main export function.
		pFRR->ExportAll ();
	}

	// Handle any file errors, or other exceptions.
	CATCH ( CFileException, e )
	{
		// Didn't work - report failure to caller.
		if ( pDC )
			pDC->ExportFile->SetThrowExceptions ( FALSE );
		pRegion->StopRender ();
		if ( ShowProgress )
			EndSlowJob ();
		return FALSE;
	}
	END_CATCH

	// All ok
	return TRUE;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL FlashRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/2004
	Inputs:		pNode - pointer to node about to be rendered by RenderTree
				pRender - pointer to RenderRegion running RenderTree loop
	Outputs:	-
	Returns:	TRUE if node can be rendered
				FALSE if not
	Purpose:	Handle the callback from RenderTree that has been requested by Filter

********************************************************************************************/

BOOL FlashRenderCallback::BeforeNode(RenderRegion* pRegion, Node* pNode)
{
	// If this is the background node, ignore it
	if (m_pBackgroundNode != NULL && pNode == m_pBackgroundNode)
		return FALSE;

	// Declare variables.
	LayerState	ThisLayer = m_pFilter->GetButtonState(pNode);
	String_256	ButtonName;

	// Set the layer state.
	m_pFRR->SetLayerState(ThisLayer);

	// Get the layer ID, so that we can tell if it's a button or not.
	if (ThisLayer != NO_BUTTON && m_pFilter->GetButtonName(pNode, ButtonName))
	{
		// The name has been successfully extracted, and is passed into the render
		// region.
		m_pFRR->SetButtonName((TCHAR*)ButtonName);
	}
	else
	{
		// A NULL pointer in this function instructs the RenderRegion to treat the
		// next node as an ordinary shape, rather than a button.
		m_pFRR->SetButtonName(NULL);
	}

	// Update the progress display
	if (pNode->IsNodeRenderableClass())
		m_pFilter->UpdateFilterProgress(((NodeRenderable*)pNode)->GetSizeOfExport(m_pFilter));
	else
		m_pFilter->UpdateFilterProgress(0);				// For back compatibility with previous version of this code

	// Render (export) the node that we have, using special export rendering if it exists.
	BOOL bExportRendered = pNode->ExportRender(pRegion);

	return (!bExportRendered);						// If not exported let RenderTree do it, return TRUE
}


/********************************************************************************************

>	BOOL FlashRenderCallback::BeforeSubtree(RenderRegion* pRegion,
											Node* pNode,
											Node** ppNextNode,
											BOOL bClip,
											SubtreeRenderState* pState)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/11/2004
	Inputs:		pNode - pointer to node about to be rendered by RenderTree
				pRender - pointer to RenderRegion running RenderTree loop
	Outputs:	pNextNode - pointer to node where navigation should continue
				State - Navigation state for RenderTree state machine
	Returns:	TRUE if we've returned a valid SubRenderState
				FALSE if not
	Purpose:	Handle the callback from RenderTree that has been requested by Filter

********************************************************************************************/

BOOL FlashRenderCallback::BeforeSubtree(RenderRegion* pRegion,
										 Node* pNode,
										 Node** ppNextNode,
										 BOOL bClip,
										 SubtreeRenderState* pState)
{
	// We're not counting so let normal, smart navigation take place
	if (pNode->NeedsToExport(pRegion, m_bVisibleLayersOnly, m_bSelectedOnly))
	{
		// This node does need to be exported
		// Tell the node at the root of the subtree that we're about to export it and it's children
		pNode->PreExportRender(pRegion);
		return FALSE;						// Tell RenderTree we want normal navigation to take place
	}
	else
	{
		*pState = SUBTREE_NORENDER;			// Tell RenderTree we want to forcibly skip this node
		return TRUE;
	}
}


/********************************************************************************************

>	virtual BOOL FlashFilter::ExportSelectionOnly()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/12/99
	Returns:	True if this filter wants check if an object is selected or not	and so export
				only the selected items Or False if want to export everything. (The value of
				mOnlySelected.)
	Purpose:	Determines if the filter wants to export only selected items.
	SeeAlso:	FlashFilter::ExportRender;

********************************************************************************************/

BOOL FlashFilter::ExportSelectionOnly(BOOL MaskedRender)
{
	// Flash defaults to attempt to export a selection first.
	return mOnlySelected;
}

/********************************************************************************************

>	virtual BOOL FlashFilter::ExportVisibleLayersOnly()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/00
	Inputs:		-
	Returns:	TRUE	- Only export visible layers.
				FALSE	- Export everything.
	Purpose:	Determines if the filter wants to export only visible layers.
				NB. This base class version does the default action of making the filter
				export everything including hidden layers.
	SeeAlso:	Filter::ExportRender;
				Filter::VisibleLayersOnly

********************************************************************************************/

BOOL FlashFilter::ExportVisibleLayersOnly()
{
	// The Flash filter should ignore hidden layers. Unfortunately the button system uses
	// hidden layers for its state, so export them in any case.
	return FALSE;
}	

/********************************************************************************************

>	LayerState FlashFilter::GetButtonState ( Node *pNode )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Returns:	LayerState - The layer on which this node resides, or NO_BUTTON if it's on a
				regular layer.
	Purpose:	Tests to see whether the node is part of a roll-over button or not.
	SeeAlso:	FlashFilter::ExportRenderNodes

********************************************************************************************/

LayerState FlashFilter::GetButtonState ( Node *pNode )
{
	// This function catches button codes, which are inconveniently scattered
	// throughout the tree, and uses this information to rebuild the list of nodes to
	// export into the correct form for buttons.
	Node		*pParent	= pNode->FindParent ();
	LayerState	ThisLayer	= NO_BUTTON;

	// Browse up the tree until we either find a layer node, or run out of tree.
	while ( pParent != NULL )
	{
		if ( pParent->IsLayer () )
		{
			// Extract the layer's name...
			String_256	LayerName	= ( ( Layer* ) pParent )->GetLayerID ();

			// ... set up comparison strings...
			String_256	Clicked ( _R(IDS_ROLLOVER_CLICKED) );		// = "Clicked"
			String_256	Default ( _R(IDS_ROLLOVER_DEFAULT) );		// = "Default"
			String_256	Mouse ( _R(IDS_ROLLOVER_MOUSE) );			// = "Mouse"
			String_256	Selected ( _R(IDS_ROLLOVER_SELECTED) );		// = "Selected"

			// ... and use these to identify whether it's a button layer, and what part
			// of the button it forms.
			if ( LayerName.IsIdentical ( Clicked ) )
			{
				ThisLayer = CLICKED;
			}
			else if ( LayerName.IsIdentical ( Default ) )
			{
				ThisLayer = DEFAULT;
			}
			else if ( LayerName.IsIdentical ( Mouse ) )
			{
				ThisLayer = MOUSE;
			}
			else if ( LayerName.IsIdentical ( Selected ) )
			{
				ThisLayer = SELECTED;
			}

			// Break out of the while loop.
			break;
		}

		// If it's not a layer, grab the next node and continue.
		pParent = pParent->FindParent ();
	}

	return ThisLayer;
}

/********************************************************************************************

>	BOOL FlashFilter::GetButtonName ( Node *pNode,
									  String_256 &ButtonName )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/1/00
	Returns:	True if a button has been found, otherwise false.
	Purpose:	If the node has a template attribute containing the name, this function loads
				it into the ButtonName string, and returns TRUE indicating that it's been
				found. Basically it extracts an ID name from a template.
	SeeAlso:	FlashFilter::ExportRenderNodes (), FlashFilter::GetButtonState ()

********************************************************************************************/

BOOL FlashFilter::GetButtonName ( Node *pNode,
								  String_256 &ButtonName )
{
	// We have a potential button candidate. Now check to see whether or not
	// it has a Templater Attribute node. If it does, get the name of the
	// button, and export it using the export DC.
	Node		*pChild		= pNode->FindFirstChild ();
	BOOL		FoundButton	= FALSE;

	// Ensure that the string is empty.
	ButtonName.Empty ();

	// Try to find a TemplateAttribte amongst the node's children.
	while ( pChild != NULL && !FoundButton )
	{
		// If one is found, the object is a button.
		if ( pChild->IsKindOf ( CC_RUNTIME_CLASS ( TemplateAttribute ) ) )
		{
			// It's a button! First extract the button's name.
			ButtonName = ( ( TemplateAttribute * ) pChild )->GetParam ();

			// Break out of the loop.
			FoundButton = TRUE;
		}
		else
		{
			// Otherwise parse the next record.
			pChild = pChild->FindNext ();
		}
	}

	// Return whether a button has been found.
	return FoundButton;
}
