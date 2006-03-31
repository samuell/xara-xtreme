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

// This file holds all the AttributeValue and NodeAttribute classes to do with path
// filling attributes.

/*
*/

#include "camtypes.h"

#include "fillattr.h"
#include "colormgr.h"
//#include "lineattr.h"
//#include "ink.h"
//#include "rndrgn.h"
//#include "grndrgn.h"
//#include "osrndrgn.h"
//#include "mario.h"
//#include "simon.h"
//#include "tim.h"
#include "tranform.h"
#include "app.h"
#include "blobs.h"
#include "opgrad.h"
#include "docview.h"
#include "fixmem.h"
#include "toollist.h"
#include "bitmpinf.h"
//#include "filltool.h"
#include "attrmgr.h"
#include "paths.h"
#include "fracfill.h"
#include "dibutil.h"
#include "bitmap.h"
#include "oilbitmap.h"
#include "spread.h"
//#include "will.h"
#include "ndoptmz.h"
#include "attrmap.h"
#include "camfiltr.h"
#include "cxftags.h"
#include "cxfdefs.h"
#include "rechattr.h"	// Record handling classes for attributes
#include "cxfrec.h"
#include "cxfcols.h"	// REF_DEFAULTCOLOUR_TRANSPARENT ...

#include "blendatt.h"
#include "lineattr.h"

//#include "jason.h" 		// For _R(IDS_BLACK)

#include "nodebmp.h"
#include "nodemold.h"
#include "ophist.h"

//#include "bfxalu.h"		// For BfxALU::IsGreyscaleBitmap

//#include "gerry.h"		// for new fill types
#include "fillramp.h"	// for new fill ramps, oh luvely

#include "bevtrap.h"	// for the calculate intersection stuff

#include "extender.h"	// for Extender functionality
//#include "ndcchbmp.h"	// so we know what a NodeCacheBitmap is.
#include "colcomp.h"	// for ColourListComponent
#include "cutop.h"
#include "effects_stack.h"


DECLARE_SOURCE("$Revision$");

//
// NodeAttribute classes
//
CC_IMPLEMENT_DYNAMIC(AttrFillGeometry,		  		NodeAttribute)
CC_IMPLEMENT_DYNAMIC(AttrTranspFillGeometry,	 	AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrFlatFill,					AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrLinearFill, 				AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrRadialFill, 				AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrConicalFill, 				AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrSquareFill, 				AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrBitmapFill, 				AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrFractalFill, 				AttrBitmapFill)
CC_IMPLEMENT_DYNAMIC(AttrThreeColFill, 				AttrFillGeometry)
CC_IMPLEMENT_DYNAMIC(AttrFourColFill,				AttrThreeColFill)

CC_IMPLEMENT_DYNAMIC(AttrValueChange,				AttrFillGeometry)
CC_IMPLEMENT_DYNCREATE(AttrColourChange,			AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrColourDrop,				AttrColourChange)
CC_IMPLEMENT_DYNCREATE(AttrStrokeColourChange,		AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrStrokeTranspChange,		AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrStrokeTranspTypeChange,	AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrTranspChange,			AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrTranspTypeChange,		AttrTranspChange)
CC_IMPLEMENT_DYNCREATE(AttrBitmapChange,			AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrBitmapTessChange,		AttrBitmapChange)
CC_IMPLEMENT_DYNCREATE(AttrBitmapDpiChange,			AttrBitmapChange)
CC_IMPLEMENT_DYNCREATE(AttrFractalChange,			AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrFractalGrainChange,		AttrFractalChange)
CC_IMPLEMENT_DYNCREATE(AttrFractalTileableChange,	AttrFractalChange)
CC_IMPLEMENT_DYNCREATE(AttrNoiseScaleChange,		AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrColFillRampChange,		AttrValueChange)
CC_IMPLEMENT_DYNCREATE(AttrTranspFillRampChange,	AttrValueChange)

CC_IMPLEMENT_DYNCREATE(AttrFlatColourFill, 		 	AttrFlatFill)
CC_IMPLEMENT_DYNCREATE(AttrLinearColourFill, 		AttrLinearFill)
CC_IMPLEMENT_DYNCREATE(AttrRadialColourFill, 		AttrRadialFill)
CC_IMPLEMENT_DYNCREATE(AttrConicalColourFill, 		AttrConicalFill)
CC_IMPLEMENT_DYNCREATE(AttrSquareColourFill, 		AttrSquareFill)
CC_IMPLEMENT_DYNCREATE(AttrBitmapColourFill, 		AttrBitmapFill)
CC_IMPLEMENT_DYNCREATE(AttrTextureColourFill, 		AttrFractalFill)
CC_IMPLEMENT_DYNCREATE(AttrFractalColourFill, 		AttrTextureColourFill)
CC_IMPLEMENT_DYNCREATE(AttrNoiseColourFill, 		AttrTextureColourFill)
CC_IMPLEMENT_DYNCREATE(AttrThreeColColourFill, 		AttrThreeColFill)
CC_IMPLEMENT_DYNCREATE(AttrFourColColourFill,		AttrFourColFill)

CC_IMPLEMENT_DYNCREATE(AttrFlatTranspFill, 		 	AttrFlatFill)
CC_IMPLEMENT_DYNCREATE(AttrLinearTranspFill, 		AttrLinearFill)
CC_IMPLEMENT_DYNCREATE(AttrRadialTranspFill, 		AttrRadialFill)
CC_IMPLEMENT_DYNCREATE(AttrConicalTranspFill, 	 	AttrConicalFill)
CC_IMPLEMENT_DYNCREATE(AttrSquareTranspFill, 	 	AttrSquareFill)
CC_IMPLEMENT_DYNCREATE(AttrBitmapTranspFill, 		AttrBitmapFill)
CC_IMPLEMENT_DYNCREATE(AttrTextureTranspFill, 		AttrFractalFill)
CC_IMPLEMENT_DYNCREATE(AttrFractalTranspFill, 		AttrTextureTranspFill)
CC_IMPLEMENT_DYNCREATE(AttrNoiseTranspFill,			AttrTextureTranspFill)

CC_IMPLEMENT_DYNCREATE(AttrThreeColTranspFill, 	 	AttrThreeColFill)
CC_IMPLEMENT_DYNCREATE(AttrFourColTranspFill, 	 	AttrFourColFill)

CC_IMPLEMENT_DYNAMIC(AttrCircularColourFill,		AttrRadialColourFill)
CC_IMPLEMENT_DYNAMIC(AttrCircularTranspFill,		AttrRadialTranspFill)

CC_IMPLEMENT_DYNCREATE(AttrFillMapping, 				NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrFillMappingLinear, 			AttrFillMapping)
CC_IMPLEMENT_DYNCREATE(AttrFillMappingSin, 				AttrFillMapping)

CC_IMPLEMENT_DYNCREATE(AttrFillEffect, 					NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrFillEffectFade, 				AttrFillEffect)
CC_IMPLEMENT_DYNCREATE(AttrFillEffectRainbow, 			AttrFillEffect)
CC_IMPLEMENT_DYNCREATE(AttrFillEffectAltRainbow, 		AttrFillEffect)

CC_IMPLEMENT_DYNCREATE(AttrTranspFillMapping, 			NodeAttribute)
CC_IMPLEMENT_DYNCREATE(AttrTranspFillMappingLinear, 	AttrTranspFillMapping)
CC_IMPLEMENT_DYNCREATE(AttrTranspFillMappingSin, 		AttrTranspFillMapping)

CC_IMPLEMENT_DYNCREATE(AttrMould, NodeAttribute)

// Message sent when Attributes are changed
CC_IMPLEMENT_DYNAMIC(AttrChangedMsg, Msg) 

CC_IMPLEMENT_MEMDUMP(FillBlobSelectionState, CC_CLASS_MEMDUMP)

// v2 file format fill record handler
CC_IMPLEMENT_DYNAMIC(FillAttrRecordHandler, CamelotRecordHandler)


// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW  

// Static pointers used when Creating or Editing Fill Attribues
AttrFillGeometry* AttrFillGeometry::EditedFill = NULL;
AttrFillGeometry* AttrFillGeometry::DraggedFill = NULL;

// Static variables	used to stop fill meshes from overwriting one-another
BOOL AttrFillGeometry::DoFillMeshCheck = TRUE;
FillGeometryAttribute* AttrFillGeometry::LastRenderedMesh = NULL;
DocCoord AttrFillGeometry::LastRenderedStartBlob = DocCoord(0,0);
DocCoord AttrFillGeometry::LastRenderedEndBlob = DocCoord(0,0);
DocCoord AttrFillGeometry::LastRenderedEnd2Blob = DocCoord(0,0);
DocCoord AttrFillGeometry::LastRenderedEnd3Blob = DocCoord(0,0);

BOOL AttrFillGeometry::TranspMeshesVisible = FALSE;

// The number of selected fill control points
UINT32 AttrFillGeometry::SelectionCount = 0;

// Some Static variables used for scanning attributes in the Tree.
// See AttrFillGeometry::FindFirstSelectedAttr()
SelectedAttrs AttrFillGeometry::s_SelAttrs;
CCAttrMap AttrFillGeometry::AttribMap;

FillControl AttrFillGeometry::ControlHit;
AttrFillGeometry* AttrFillGeometry::FillHit;

List AttrFillGeometry::HitList;

INT32 AttrFillGeometry::FractalSeed = 63933;
double AttrFillGeometry::FractalGraininess = 5;
double AttrFillGeometry::FractalGravity = 0;

INT32 AttrFillGeometry::FractalDPI = 96;
INT32 AttrFillGeometry::MaxFractalSize = 256;
BOOL AttrFillGeometry::DoCheckOnFillRampMesh = TRUE;
BOOL AttrFillGeometry::s_bGroupTransparency = TRUE;


/////////////////////////////////////////////////////////////////////////////////////////////
//
//								NodeAttribute classes
//
/////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFillGeometry
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	static BOOL AttrFillGeometry::Init()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/94
	Purpose:	Intialise the fill geometry editing operations

********************************************************************************************/

BOOL AttrFillGeometry::Init()
{
#ifdef STANDALONE
	BOOL ok = TRUE;
#else
	// Initialise our fill Creation and Editing Operations
	BOOL ok = ( OpCreateFill::Init() && OpEditFill::Init() );
#endif

#if !defined(EXCLUDE_FROM_RALPH)
	// Declare out 'Bodge' preference that enables or disables the checking for
	// duplicate fill mesh rendering
	if (Camelot.DeclareSection(_T("DebugFlags"), 1))
	{
		Camelot.DeclarePref( NULL, _T("DoFillMeshCheck"), 
						&DoFillMeshCheck, FALSE, TRUE );
	}

	if (Camelot.DeclareSection(_T("Attributes"), 6))
	{
		Camelot.DeclarePref(NULL, _T("FractalDPI"), 		(UINT32*)&FractalDPI);
		Camelot.DeclarePref(NULL, _T("MaxFractalSize"),	(UINT32*)&MaxFractalSize);
		Camelot.DeclarePref(NULL, _T("FractalSeed"), 		(UINT32*)&FractalSeed);
		Camelot.DeclarePref(NULL, _T("FractalGraininess"),	&FractalGraininess);
		Camelot.DeclarePref(NULL, _T("FractalGravity"),   	&FractalGravity);
		Camelot.DeclarePref(NULL, _T("GroupTransparency"), &s_bGroupTransparency);
	}
#endif

#ifndef STANDALONE
	if (Camelot.DeclareSection( _T("Dragging"), 3))
	{
		Camelot.DeclarePref(NULL, _T("InteractiveFillEditing"), &OpEditFill::InteractiveDragUpdate);
		Camelot.DeclarePref(NULL, _T("ContinuousFillControlUpdate"), &OpEditFill::ContinuousEOR);
		Camelot.DeclarePref(NULL, _T("DelayBeforeFillUpdate"), (UINT32*)&OpEditFill::IdleFillDelay);
	}
#endif

	return ok;
}

/********************************************************************************************

	Some Static functions for other people to check for control point hits
	or pass clicks to attributes etc.

********************************************************************************************/

/********************************************************************************************

>	BOOL AttrFillGeometry::CheckAttrClick( DocCoord PointerPos, ClickType Click, ClickModifiers ClickMods,
									Spread* pSpread)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/94
	Inputs:		PointerPos - The Coords (in DocCoords) of the point where the mouse button
				was clicked
				Click -	Describes the type of click that was detected. 
				ClickMods -	Indicates which buttons caused the click and which modifers were
				pressed at the same time
	Returns:	TRUE if it handled the Click, FALSE otherwise
	Purpose:	Check to see if an Attribute wants to do something with a click.
	SeeAlso:	AttrFillGeometry::CheckForFillControlHit

********************************************************************************************/

BOOL AttrFillGeometry::CheckAttrClick(DocCoord PointerPos,
									  ClickType Click,
									  ClickModifiers ClickMods,
									  Spread* pSpread)
{
#ifdef STANDALONE
	return FALSE;
#else

	if ( !(((Camelot.GetBlobManager())->GetCurrentInterest()).Fill) )
		return FALSE;	// Don't bother if fills blobs aren't displayed at the mo.

	// UINT32 SelCount = AttrFillGeometry::SelectionCount;
	LastRenderedStartBlob = DocCoord(0,0);
	LastRenderedEndBlob = DocCoord(0,0);
	LastRenderedEnd2Blob = DocCoord(0,0);
	LastRenderedEnd3Blob = DocCoord(0,0);

	if (Click == CLICKTYPE_UP)
	{
		if (!HitList.IsEmpty())
		{
			FillHit = NULL;
			ControlHit = FILLCONTROL_NULL;
			HitList.DeleteAll();	// Make sure the control hit list is empty
			return TRUE;
		}
	}

	if (Click == CLICKTYPE_SINGLE)
	{
		FillHit = NULL;
		ControlHit = FILLCONTROL_NULL;
		HitList.DeleteAll();	// Make sure the control hit list is empty

		// Find a fill attribute in the selection
		CCRuntimeClass* FillType = IsColourMeshVisible() ? CC_RUNTIME_CLASS(AttrFillGeometry)
														 : CC_RUNTIME_CLASS(AttrTranspFillGeometry);
		
		AttrFillGeometry* pAttr = FindFirstSelectedAttr(FillType);
	
		while (pAttr != NULL)
		{
			// Ask the Attribute if it wan't to do anything with the click
			if ( pAttr->OnClick(PointerPos, Click, ClickMods, pSpread) )
			{
				if (FillHit == NULL)
				{
					FillHit = pAttr;
				}

				BOOL InList = FALSE;

				// We may have already hit this attribute if the selection is
				// inside a parent and have common attributes, so we need to
				// check and make sure this attribute is NOT in the list
				// already.

				if (!HitList.IsEmpty())
				{
					ListItem* pItem = HitList.GetHead();

					while (pItem)
					{
						NodeAttributePtrItem* NodePtr = (NodeAttributePtrItem*)pItem;

						if (NodePtr->NodeAttribPtr == pAttr)
						{
							// Ignore this one, we've hit it already
						 	InList = TRUE;
							break;
						}

						pItem = HitList.GetNext(pItem);
					}
				}

				if ( !InList &&
					 IsMeshSame((FillGeometryAttribute*)FillHit->GetAttributeValue(),
								 (FillGeometryAttribute*)pAttr->GetAttributeValue()) ) 
				{
					NodeAttributePtrItem* NodePtr = new NodeAttributePtrItem;

					if (NodePtr != NULL)
					{
						// Add the Attr to the Hit list
						NodePtr->NodeAttribPtr = pAttr;
						HitList.AddTail(NodePtr);
					}
				}

			}

			// Move onto the next.
			pAttr = FindNextSelectedAttr(FillType);
		}                              

		// Check to see if anyone has deselected their control points
		// (The click may not have hit anyone at all, but they still may
		// have deselected their points).
//		if (!ClickMods.Adjust && HitList.IsEmpty())
//		{
			BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED)); 
//		}

//		if (!HitList.IsEmpty())
//			return TRUE;
	}

	if (Click == CLICKTYPE_DOUBLE)			// check for multi-stage fill blob insertion ....
	{
		FillHit = NULL;
		ControlHit = FILLCONTROL_NULL;
		HitList.DeleteAll();	// Make sure the control hit list is empty

		// Find a fill attribute in the selection
		CCRuntimeClass* FillType = /*IsColourMeshVisible() ?*/ CC_RUNTIME_CLASS(AttrFillGeometry);
														 //: CC_RUNTIME_CLASS(AttrTranspFillGeometry);
		
		// Use our own selected attrs iterator because OnClick uses the legacy global iterator
		SelectedAttrs selattrs;
		AttrFillGeometry* pAttr = (AttrFillGeometry*)selattrs.FindFirst(FillType);

		BOOL RetVal = FALSE;

		// walk the selection list ....
	
		while (pAttr != NULL)
		{	
			// Ask the Attribute if it wan't to do anything with the click
			if ( pAttr->OnClick(PointerPos, Click, ClickMods, pSpread) )
			{
				// the actual work has been done in the above function call ....
				// BUT we do need to select the new blob ....
				// which by camelots marvelousness only seems to work from within the
				// fill tool ....  (take a look at GradFillTool::OnClick ())
				RetVal = TRUE;
			}
			
			// Move onto the next.
			pAttr = (AttrFillGeometry*)selattrs.FindNext(FillType);
		}

		BROADCAST_TO_ALL(SelChangingMsg(SelChangingMsg::COLOURATTCHANGED));
		return (RetVal);
	}

	if (Click == CLICKTYPE_SINGLE && !HitList.IsEmpty())
	{
		// We hit one or more fills, and are about to drag them ...

		// Is there a Drag Operation already happening ?
		if (Operation::GetCurrentDragOp() == NULL)
		{
			NodeAttributePtrItem* HitFillPtr = 
				(NodeAttributePtrItem*)AttrFillGeometry::HitList.GetHead();

			AttrFillGeometry* HitFill = (AttrFillGeometry*)HitFillPtr->NodeAttribPtr;
			
			// Need to do a drag on the selected points,
			// so we had better start an operation
			OpEditFill* pOpEditFill = new OpEditFill;
			if (pOpEditFill == NULL)
			{
				// Failed to get the memory to do the job
				TRACEUSER( "Mike", _T("The Graduated Fill Edit Blob Operation failed to start\n") );

				// Inform the user that we are out of memory
				// The error message will be set by new
				InformError();
			}
			else
			{
				// check for a click on a fill ramp control point
				if (ISA_RAMPINDEX(ControlHit))
				{
					// if so, select the point in the fill
					if (HitFill->GetColourRamp())
					{
						HitFill->GetColourRamp()->DeselectAll();
						HitFill->GetColourRamp()->SetSelState(ControlHit, 1);
					}
				}
				
				// Go drag that Control Point !!
				pOpEditFill->DoDrag(PointerPos, pSpread, HitFill, ControlHit);
			}
		}
	
		return TRUE;
	}

	// Don't claim the click
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL AttrFillGeometry::OnClick( DocCoord PointerPos, ClickType Click, 
									 		ClickModifiers ClickMods )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/7/94
	Inputs:		PointerPos - The Location of the mouse pointer at the time of the click
				Click - The type of click received (single, double, drag etc)
				ClickMods - The modifiers to the click (eg shift, control etc )
	Returns:	BOOL - TRUE if the fill claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the fill to respond to clicks by selecting its blobs or starting
				drags etc.
				
********************************************************************************************/

BOOL AttrFillGeometry::OnClick( DocCoord PointerPos, ClickType Click, 
								ClickModifiers ClickMods, Spread *pSpread )
{
#ifndef STANDALONE
	DocView *pDocView = DocView::GetCurrent();

	// Should always be able to get current view
	ERROR3IF(pDocView == NULL, "AttrFillGeometry::OnClick: Could not get current DocView");
	if (pDocView==NULL)
		return FALSE;

	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill ||
		!IsVisible())
		return FALSE;

	// Is it just a single click ?
	if ( Click == CLICKTYPE_SINGLE )
	{
		// Did the click hit any of our control points ?
		FillControl Hit = CheckForControlHit(PointerPos);
		if ( Hit != FILLCONTROL_NULL )
		{
			// It hit one of our Controls.

			// Was it an Adjust Click ?
			if (ClickMods.Adjust && !ClickMods.Constrain)
			{
				if (!(ISA_RAMPINDEX (Hit)))				// but not for multi-stage fill blobs!
				{
					DeselectAllBut(Hit);
					// Just toggle the Selection state
					ToggleSelection(Hit);
				}
				else
				{
					SelectBlob(Hit);
					DeselectAllBut(Hit);
				}
    		}
    		else
			{
				// It was a Select Click, so we need
				// to select this Blob, and deselect
				// all the others
				SelectBlob(Hit);
				DeselectAllBut(Hit);

				if (ClickMods.Menu)				// was it the right-hand mouse button?
				{
					if (ISA_RAMPINDEX (Hit))	// if so and ramp, then delete the blob ....
					{
						OpDescriptor* pOpDelete = OpDescriptor::FindOpDescriptor (CC_RUNTIME_CLASS (OpDelete));

						if (pOpDelete)
						{
							pOpDelete->Invoke ();
						}
					}
				}
			}

			if (ControlHit == FILLCONTROL_NULL)
				ControlHit = Hit;

			if (Hit != ControlHit)
				return FALSE;

			// Claim the Click, 'cus we did something
			return TRUE;
		}
		else
		{
	 		// If it was a select click then we need to
			// deselect all out control points
			if (!ClickMods.Adjust)
			{
				DeselectAll();
			}

			// Don't claim the click
			return FALSE;	
		}
	}

	// Is it just a double click ?			// check for insertion of multi-stage fill blob ....
	if (Click == CLICKTYPE_DOUBLE)
	{
		// Did the click hit any of our control points ?
		FillControl Hit = CheckForControlHit (PointerPos);

		if (Hit == FILLCONTROL_RAMPPOINT)
		{
			DocCoord StartPoint = *(GetStartPoint());
			DocCoord EndPoint   = *(GetEndPoint());
			
			// where on the ramp point?
			
			/*double d =*/ FindRampPoint(PointerPos, StartPoint, EndPoint);
			
			Spread* pSpread = NULL;
			NodeRenderableInk* pNode = NULL;
			Pixel32bpp Pix;

			Document* TheDoc = Document::GetCurrent();
			pSpread = TheDoc->FindFirstSpread ();

			if (TheDoc != NULL)
			{
				// read the colour at that point ....
				///*pNode = */NodeRenderableInk::FindSimpleAtPointForColourPicker (pSpread, PointerPos, Pix, NULL, NULL);

				pNode = (NodeRenderableInk *) this->FindParent ();

				NodeRenderableInk::FindColourForNodeRenderableAtPoint (pSpread, PointerPos, Pix, pNode, this);

				if (pNode != NULL)
				{
					// convert and apply the colour as a new fill point ....
					
					const double conversion = 1.0/255.0;
							
					FIXED24 rValF24 = Pix.Red * conversion;
					FIXED24 gValF24 = Pix.Green * conversion;
					FIXED24 bValF24 = Pix.Blue * conversion;
					FIXED24 tValF24 = Pix.Alpha * conversion;
							
					ColourRGBT theColourRGBT;
					theColourRGBT.Red = rValF24;
					theColourRGBT.Green = gValF24;
					theColourRGBT.Blue = bValF24;
					theColourRGBT.Transparent = tValF24;
					DocColour* ColourToApply = new DOCCOLOUR_RGBT(&theColourRGBT);

					if (ColourToApply == NULL)
					{
						return (FALSE);
					}

					DocRect pSimpleBounds;
					pSimpleBounds = pNode->GetBoundingRect();
							
					AttrColourDrop* dummyDrop = new AttrColourDrop (PointerPos, pSimpleBounds, *ColourToApply);

					if (dummyDrop == NULL)
					{
						delete (ColourToApply);
						return (FALSE);
					}

					//AttrFillGeometry::DoColourDropTest(dummyDrop, pNode, this);

					dummyDrop->SetObjectDroppedOn (pNode);

					AttributeManager::ApplyAttribToNode(pNode, dummyDrop);

					delete (ColourToApply);

					return (TRUE);
				}
			}		
			return (FALSE);
		}
		return FALSE;
	}

#endif
	return FALSE;
}

/********************************************************************************************

>	BOOL AttrFillGeometry::IsFillBeingEdited()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/94
	Returns:	TRUE if this fill is currently being edited, FALSE otherwise
	Purpose:	Check to see if a fill is being edited, so we can decide whether
				to render the fill mesh or not.

********************************************************************************************/

BOOL AttrFillGeometry::IsFillBeingEdited()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (EditedFill == this)
	{
		return TRUE;
	}

	if (EditedFill != NULL && OpEditFill::CreateFill)
	{
		return TRUE;
	}

	if (EditedFill == NULL || HitList.IsEmpty())
	{
		return FALSE;
	}

	NodeAttributePtrItem* HitFillPtr = (NodeAttributePtrItem*)AttrFillGeometry::HitList.GetHead();

	while (HitFillPtr != NULL)
	{
		if (HitFillPtr->NodeAttribPtr == this)
		{
			return TRUE;
		}

		HitFillPtr = (NodeAttributePtrItem*)AttrFillGeometry::HitList.GetNext(HitFillPtr);
	}
#endif
	return FALSE;
}

/********************************************************************************************

>	static BOOL AttrFillGeometry::CheckForFillControlHit(DocCoord Pos, UINT32* Status)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/94
	Inputs:		Pointer to a Status ID to update if we hit a control point
	Purpose:	Check to see if the mouse moves over a fill control point.

********************************************************************************************/

BOOL AttrFillGeometry::CheckForFillControlHit(DocCoord Pos, UINT32* Status)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ( !(((Camelot.GetBlobManager())->GetCurrentInterest()).Fill) )
		return FALSE;	// Don't bother if fills blobs aren't displayed at the mo.

	BOOL HitAControl = FALSE;

	// Find the first attribute to test
	CCRuntimeClass* FillType = IsColourMeshVisible() ? CC_RUNTIME_CLASS(AttrFillGeometry)
													 : CC_RUNTIME_CLASS(AttrTranspFillGeometry);

	AttrFillGeometry* pAttr = FindFirstSelectedAttr(FillType);
	while (pAttr != NULL)
	{
		// See if the current pos is over one of this fills controls
		FillControl Hit = pAttr->CheckForControlHit(Pos);

		if (Hit != FILLCONTROL_NULL)
		{
			// We'ew over one of this fills controls ...
			// So update the Status according to the controls selection state
			// and take into account whether other control are selected

			if ( AttrFillGeometry::SelectionCount == 0)
			{
				// No Control points selected
				*Status = _R(IDS_FS_CNTRLPNT_SO);
			}

			if ( AttrFillGeometry::SelectionCount == 1)
			{
				// Just One Control point selected ....
				if ( pAttr->IsSelected(Hit) )
				{
					if (!ISA_RAMPINDEX(Hit))
					{
						*Status = _R(IDS_FS_CNTRLPNT_DO);	// and it must be this one
					}
					else
					{
						*Status = _R(IDS_FS_CNTRLPNTMS_DO);	// and it must be this one (multi-stage)
					}
				}
				else
					*Status = _R(IDS_FS_CNTRLPNT_SM);   // and it's not this one
			}

			if ( AttrFillGeometry::SelectionCount >= 2)
			{
				// Many Control points selected ...
				if ( pAttr->IsSelected(Hit) )
					*Status = _R(IDS_FS_CNTRLPNT_DM); 	// and this is one of them
				else
					*Status = _R(IDS_FS_CNTRLPNT_SM);   // and this isn't on of them
			}

			// Signal that we hit something
			HitAControl = TRUE;
   		    break;
		}
		// Move onto the next attr
		pAttr = FindNextSelectedAttr(FillType);
	}                              

	// Tell the caller whether we hit anything or not
	return HitAControl;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL AttrFillGeometry::CheckPreviousFillMesh()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	BODGE to stop fill meshes eoring each other out.
				This can be disabled by setting 'DoFillMeshCheck' to '0' in the 'ini' file.

********************************************************************************************/

BOOL AttrFillGeometry::CheckPreviousFillMesh()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (AllowRampRedraw)	// don't want to do this in certian cases!
	{
		// Don't bother if disabled by 'ini' file preference
		if (!DoFillMeshCheck)
			return FALSE;

		// Check to see if the Last Rendered Fill Mesh is the same as this one
		if (LastRenderedMesh != NULL && IsMeshSame(LastRenderedMesh, (FillGeometryAttribute*)GetAttributeValue()) )
		{
			// Yep, they're the same
			return TRUE;
		}
		else
		{
			// Not the same, so update the 'Last Rendered AttrValue' pointer
			LastRenderedMesh = (FillGeometryAttribute*)GetAttributeValue();
 		}
		// They're not the same
	}
#endif
	
	return FALSE;
}

BOOL AttrFillGeometry::IsMeshSame(FillGeometryAttribute* Fill1, FillGeometryAttribute* Fill2)
{
#if !defined(EXCLUDE_FROM_RALPH)
	BOOL PointsAreSame = FALSE;

	if (Fill1->GetStartPoint() != NULL && Fill2->GetStartPoint() != NULL)
	{
		if (*Fill1->GetStartPoint() != *Fill2->GetStartPoint())
			return FALSE;

		PointsAreSame = TRUE;
	}

	if (Fill1->GetEndPoint() != NULL && Fill2->GetEndPoint() != NULL)
	{
		if (*Fill1->GetEndPoint() != *Fill2->GetEndPoint())
			return FALSE;

		PointsAreSame = TRUE;
	}

	if (Fill1->GetEndPoint2() != NULL && Fill2->GetEndPoint2() != NULL)
	{
		if (*Fill1->GetEndPoint2() != *Fill2->GetEndPoint2())
			return FALSE;

		PointsAreSame = TRUE;
	}

	return PointsAreSame;
#else
	return FALSE;
#endif
}

UINT32 AttrFillGeometry::FillSelectionCount()
{
	return SelectionCount;
}

void AttrFillGeometry::SetSelectionCount(UINT32 Count)
{
	SelectionCount = Count;
}

void AttrFillGeometry::SetTranspMeshesVisible(BOOL Visible)
{
	TranspMeshesVisible = Visible;
}

BOOL AttrFillGeometry::IsTranspMeshVisible()
{
//	if (FindParent() != NULL && !(FindParent()->IsSelected()))
//		return FALSE;

	return TranspMeshesVisible;
}

BOOL AttrFillGeometry::IsColourMeshVisible()
{
//	if (FindParent() != NULL && !(FindParent()->IsSelected()))
//		return FALSE;

	return !TranspMeshesVisible;
}

void AttrFillGeometry::SetLastRenderedBlob(FillControl Control)
{
#if !defined(EXCLUDE_FROM_RALPH)
	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:
			if (GetStartPoint() != NULL)
				LastRenderedStartBlob = *GetStartPoint();
			break;

		case FILLCONTROL_ENDPOINT:
			if (GetEndPoint() != NULL)
				LastRenderedEndBlob = *GetEndPoint();
			break;

		case FILLCONTROL_SECONDARYPOINT:
		case FILLCONTROL_ENDPOINT2:
			if (GetEndPoint2() != NULL)
				LastRenderedEnd2Blob = *GetEndPoint2();
			break;

		case FILLCONTROL_ENDPOINT3:
			if ((GetStartPoint() != NULL) &&
				(GetEndPoint() != NULL) &&
				(GetEndPoint2() != NULL))
			{
				LastRenderedEnd3Blob = *GetEndPoint2() + *GetEndPoint() - *GetStartPoint();
			}
			break;
	}
#endif
}

BOOL AttrFillGeometry::IsBlobSame(FillControl Control)
{
#if !defined(EXCLUDE_FROM_RALPH)
	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:
			if (GetStartPoint() != NULL)
				return (LastRenderedStartBlob == *GetStartPoint());
			break;

		case FILLCONTROL_ENDPOINT:
			if (GetEndPoint() != NULL)
				return (LastRenderedEndBlob == *GetEndPoint());
			break;

		case FILLCONTROL_SECONDARYPOINT:
		case FILLCONTROL_ENDPOINT2:
			if (GetEndPoint2() != NULL)
				return (LastRenderedEnd2Blob == *GetEndPoint2());
			break;

		case FILLCONTROL_ENDPOINT3:
			if ((GetStartPoint() != NULL) &&
				(GetEndPoint() != NULL) &&
				(GetEndPoint2() != NULL))
			{
				DocCoord EndPoint3 = *GetEndPoint2() + *GetEndPoint() - *GetStartPoint();
				return (LastRenderedEnd3Blob == EndPoint3);
			}
	}
#endif
	return FALSE;	
}

/********************************************************************************************

>	double AttrFillGeometry::FindRampPoint(DocCoord &dc, 
					DocCoord &StartPoint, DocCoord &EndPoint)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/10/99
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode

********************************************************************************************/
double AttrFillGeometry::FindRampPoint(DocCoord &dc, DocCoord &StartPoint, DocCoord &EndPoint)
{
	DocCoord ClickPos = dc;

	DocRect dr(StartPoint.x, StartPoint.y, StartPoint.x, StartPoint.y);

	if (dr.lo.x > EndPoint.x)
	{
		dr.lo.x = EndPoint.x;
	}
			
	if (dr.lo.y > EndPoint.y)
	{
		dr.lo.y = EndPoint.y;
	}

	if (dr.hi.x < EndPoint.x)
	{
			dr.hi.x = EndPoint.x;
	}
	
	if (dr.hi.y < EndPoint.y)
	{
		dr.hi.y = EndPoint.y;
	}

	// get the blob size
	double dBlobSize = (double)(Camelot.GetBlobManager())->GetBlobSize();

	if (dr.Width() < (INT32)(dBlobSize * 2))
	{
		// add half a point to the hit test area
		dr.hi.x += (INT32)(dBlobSize * 2);
		dr.lo.x -= (INT32)(dBlobSize * 2);
	}

	if (dr.Height() < (INT32)(dBlobSize * 2))
	{
		dr.hi.y += (INT32)(dBlobSize * 2);
		dr.lo.y -= (INT32)(dBlobSize * 2);
	}
	
	if (dr.ContainsCoord(ClickPos))
	{
		// find out if the pointer is on the line (or near it) by
		// calculating the perpendicular distance between the
		// point and the line
		double p = 0;
		
		double dPosX = ClickPos.x;
		double dPosY = ClickPos.y;
		
		double dX1 = StartPoint.x;
		double dY1 = StartPoint.y;
		
		double dX2 = EndPoint.x;
		double dY2 = EndPoint.y;
		
		double dTestPosY = 0;
		double dTestPosX = 0;
		
		if (fabs(dX2 - dX1) > fabs(dY2 - dY1))
		{
			if (dX2 != dX1)
			{
				p = (dPosX - dX1) / (dX2 - dX1);
			
				dTestPosY = ((dY2 - dY1) * p) + dY1;
			
				// do the hit test on the line
				if (fabs(dTestPosY - dPosY) < dBlobSize / 2)
				{
					return p;
				}
			}
		}
		else if (dY2 != dY1)
		{
			p = (dPosY - dY1) / (dY2 - dY1);
			
			dTestPosX = ((dX2 - dX1) * p) + dX1;
			
			// do the hit test on the line
			if (fabs(dTestPosX - dPosX) < dBlobSize / 2)
			{
				return p;			
			}
		}
	}

	return -1;
}

/********************************************************************************************

>	virtual BOOL AttrFillGeometry::NeedsTransparency() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode

********************************************************************************************/

BOOL AttrFillGeometry::NeedsTransparency() const
{
	AttrFillGeometry* pNonConst = (AttrFillGeometry*) this;

	if (!pNonConst->IsAFractalFill() && pNonConst->GetBitmap() != NULL && pNonConst->GetBitmap()->GetBPP() <= 8)
	{
	 	INT32 TranspIndex;

		// If the bitmap has a transparency index then we'll need to force transparency on
		if (pNonConst->GetBitmap()->GetTransparencyIndex(&TranspIndex))
			return TRUE;
	}

	return FALSE;
}

/********************************************************************************************

>	AttrFillGeometry::AttrFillGeometry()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/94
	Purpose:	Default constructor for a Fill Attribute.
				Sets default Selection State and Attribute Bounds.

********************************************************************************************/

AttrFillGeometry::AttrFillGeometry() 
{
	// Make all the control points deselected
	SelectionState[FILLCONTROL_STARTPOINT] = FALSE;
	SelectionState[FILLCONTROL_ENDPOINT] = FALSE;
	SelectionState[FILLCONTROL_ENDPOINT2] = FALSE;
	SelectionState[FILLCONTROL_ENDPOINT3] = FALSE;
	SelectionState[FILLCONTROL_SECONDARYPOINT] = FALSE;

	// Make the attribute bounds empty.
	// Attribute bounds are only used for 'Current Attributes' that do not have
	// a parent, so they can be scaled when applied to a new object.
	AttrBounds = DocRect(0,0,0,0);

	AllowRampRedraw = TRUE;
	AllowBoundsRedraw = TRUE;
}

/********************************************************************************************

>	AttrFillGeometry::~AttrFillGeometry()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/94
	Purpose:	Destroys a Fill.
				Currently this function does nothing.

********************************************************************************************/

AttrFillGeometry::~AttrFillGeometry()
{
	LastRenderedMesh = NULL;		// this ptr may be pointing to us!
}

/********************************************************************************************

>	INT32 AttrFillGeometry::operator=(const AttrFillGeometry& FillAttrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ERROR3IF failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrFillGeometry::operator=(AttrFillGeometry& FillAttrib)
{
	ERROR3IF(!FillAttrib.IsAFillAttr(), 
				"Trying to assign two objects with different types"); 

	// Copy the Attribute bounds
	AttrBounds = FillAttrib.AttrBounds;

	// Copy the selection state
	for (INT32 i=0; i < FILLCONTROL_SECONDARYPOINT; i++)
	{
		SetBlobState(i, FillAttrib.IsSelected(i));
	}

	// Copy the Attribute Value.  ie. The Colours and Control Points
	*((FillGeometryAttribute*)GetAttributeValue()) = *((FillGeometryAttribute*)FillAttrib.GetAttributeValue());

	// Ensure the control points are valid for this kind of fill
	if (GetRuntimeClass() != FillAttrib.GetRuntimeClass())
		ValidateAttributeValue();

	return TRUE;
}

/********************************************************************************************

>	BOOL AttrFillGeometry::CopyNodeContents( AttrFillGeometry* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
    Outputs:    NodeCopy - A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFillGeometry::CopyNodeContents( AttrFillGeometry* NodeCopy)
{
	// First call the base class copy function
	NodeAttribute::CopyNodeContents( NodeCopy );

	// Copy contents specific to derived class here
	NodeCopy->AttrBounds = AttrBounds;

	// Copy the selection state
	for (INT32 i=0; i < FILLCONTROL_SECONDARYPOINT; i++)
	{
		NodeCopy->SetBlobState(i, IsSelected(i));
	}

	// and finally, copy the Colours and Control Points
	NodeCopy->GetAttributeValue()->SimpleCopy(GetAttributeValue());

	return TRUE;
} 



/***********************************************************************************************
>   void AttrFillGeometry::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFillGeometry::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(pNodeCopy->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)), "PolyCopyNodeContents given wrong dest node type");

	if (pNodeCopy->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)))
		CopyNodeContents((AttrFillGeometry*)pNodeCopy);
}



/********************************************************************************************

>	INT32 AttrFillGeometry::operator==(const NodeAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillGeometry
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ERROR3IF failure will occur if Attrib does not have a AttrFlatGeometry 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrFillGeometry::operator==(const NodeAttribute& Attrib)
{
	// First check they are of the same type
	if (((NodeAttribute*)&Attrib)->GetAttributeType() != GetAttributeType())
		return FALSE;

	// Make a more sensible pointer
	AttrFillGeometry* Attr = (AttrFillGeometry*)&Attrib;

	// Are the attributes of the same type and are their value the same ?
	return (*((FillGeometryAttribute*)Attr->GetAttributeValue()) == *((FillGeometryAttribute*)GetAttributeValue()) );
}

/********************************************************************************************
	
// Some static functions, which scan the current Selection for Fill Attributes
// returning each one as it finds them.

********************************************************************************************/

/********************************************************************************************

>	AttrFillGeometry* AttrFillGeometry::FindFirstSelectedAttr(CCRuntimeClass* AttrType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Returns:	The first AttrFillGeometry it finds in the selection.
				or NULL if they're aren't any.
	Purpose:	Scans the selection for fill attributes.
	SeeAlso:	AttrFillGeometry::FindNextSelectedAttr

********************************************************************************************/

AttrFillGeometry* AttrFillGeometry::FindFirstSelectedAttr(CCRuntimeClass* AttrType)
{
	NodeAttribute* pAttr = s_SelAttrs.FindFirst(AttrType);
	if (pAttr && pAttr->IsAFillAttr())
		return (AttrFillGeometry*) pAttr;
	return NULL;
}


/********************************************************************************************

>	AttrFillGeometry* AttrFillGeometry::FindNextSelectedAttr(CCRuntimeClass* AttrType)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/8/94
	Returns:	The next AttrFillGeometry it finds in the selection.
	Purpose:	Scans the selection for fill attributes.
	SeeAlso:	AttrFillGeometry::FindFirstSelectedAttr

********************************************************************************************/

AttrFillGeometry* AttrFillGeometry::FindNextSelectedAttr(CCRuntimeClass* AttrType)
{
	NodeAttribute* pAttr = s_SelAttrs.FindNext(AttrType);
	if (pAttr && pAttr->IsAFillAttr())
		return (AttrFillGeometry*) pAttr;
	return NULL;
}


/********************************************************************************************

>	NodeAttribute* SelectedAttrs::FindFirst(CCRuntimeClass* AttrType)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/2005
	Returns:	The first AttrFillGeometry it finds in the selection.
				or NULL if they're aren't any.
	Purpose:	Scans the selection for attributes.
				Replaces AttrFillGeometry::FindFirstSelectedAttr without relying on global
				variables!
	SeeAlso:	AttrFillGeometry::FindNextSelectedAttr

********************************************************************************************/

NodeAttribute* SelectedAttrs::FindFirst(CCRuntimeClass* AttrType)
{
	// Get the current selection
	// and initialise all statics to initial conditions
	m_pAttrSelection = Camelot.FindSelection();
	m_pLastSelectedAttr = NULL;
	m_pSelNode = NULL;

	// If previous use of FindFirst/NextSelectedAttr was stopped before FindNext had reached the
	// end, AttrSelectionLevel may still be left lying around...
	if (m_pAttrSelectionLevel)
	{
		delete m_pAttrSelectionLevel;
		m_pAttrSelectionLevel = NULL;
	}

	m_pAttrSelectionLevel = EffectsStack::GetNewLevelRange(NULL, FALSE);	// Don't escape old controllers, apply attr to base nodes

	if (m_pAttrSelectionLevel)
	{
		// Find the first Object
		m_pSelNode = m_pAttrSelectionLevel->FindFirst();
		if (m_pSelNode)
		{
			// Now find any attributes that are applied to this node.
			Node* pAttrNode = m_pSelNode;
			if (m_pSelNode->IsAnObject())
				pAttrNode = ((NodeRenderableInk*)m_pSelNode)->GetObjectToApplyTo(AttrType);
			ENSURE(pAttrNode->IsAnObject(), "Found unexpected non-ink node in FindFirstSelectedAttr");
			m_pLastSelectedAttr = ((NodeRenderableInk*)pAttrNode)->FindAppliedAttribute(AttrType);
			if (m_pLastSelectedAttr)
				return (NodeAttribute*)m_pLastSelectedAttr;
		}

		// If nothing found on first object or level range is empty
		// fall into FindNext so that we don't have to
		// duplicate its loop and its tidy up features...
		return FindNext(AttrType);
	}

	ENSURE(m_pAttrSelectionLevel==NULL, "Possible Range Leak");
	m_pAttrSelection = NULL;
	m_pSelNode = NULL;
	m_pLastSelectedAttr = NULL;
	return NULL;
}


/********************************************************************************************

>	NodeAttribute* SelectedAttrs::FindNext(CCRuntimeClass* AttrType)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/4/2005
	Returns:	The next AttrFillGeometry it finds in the selection.
	Purpose:	Scans the selection for fill attributes.
	SeeAlso:	AttrFillGeometry::FindFirstSelectedAttr

********************************************************************************************/

NodeAttribute* SelectedAttrs::FindNext(CCRuntimeClass* AttrType)
{
	// Better make sure nobodys being Silly
	ERROR2IF((m_pAttrSelection==NULL), NULL, "AttrSelection NULL in FindNextSelectedAttr");
	ERROR2IF((m_pAttrSelectionLevel==NULL), NULL, "AttrSelectionLevel NULL in FindNextSelectedAttr");

	// Loop through the selection range until found another match or end of range
	while (m_pSelNode)
	{
		m_pSelNode = m_pAttrSelectionLevel->FindNext(m_pSelNode);

		if (m_pSelNode)
		{
			// Now find any attributes that are applied to this node.
			Node* pAttrNode = m_pSelNode;
			if (m_pSelNode->IsAnObject())
				pAttrNode = ((NodeRenderableInk*)m_pSelNode)->GetObjectToApplyTo(AttrType);
			ENSURE(pAttrNode->IsAnObject(), "Found unexpected non-ink node in FindFirstSelectedAttr");
			m_pLastSelectedAttr = ((NodeRenderableInk*)pAttrNode)->FindAppliedAttribute(AttrType);
			if (m_pLastSelectedAttr)
			{
				return (NodeAttribute*)m_pLastSelectedAttr;
			}
		}
	}

	// End of iteration so clear out our statics
	if (m_pAttrSelectionLevel)
	{
		delete m_pAttrSelectionLevel;
		m_pAttrSelectionLevel = NULL;
	}
	m_pAttrSelection = NULL;
	m_pSelNode = NULL;
	m_pLastSelectedAttr = NULL;

	return NULL;
}


/********************************************************************************************

>	void AttrFillGeometry::RenderFillBlobs()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/9/94
	Purpose:	Force Renders the fill mesh off the screen.

********************************************************************************************/

void AttrFillGeometry::RenderFillBlobs()
{
#if !defined(EXCLUDE_FROM_RALPH)
	Node *pNode = this;
	while ((pNode != NULL) && !pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
		pNode = pNode->FindParent();

	if (pNode == NULL)
		return;			// We're not really in the tree

	Spread* pSpread = (Spread*)pNode;

	DocRect Bounds = GetBlobBoundingRect();

	// EOR on in all the render regions that are still rendering,
	// so that the Blob rendering when the region is finished, 
	// will take them off
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	
	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	
		
		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread &&
				(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
		
				if (ClipRect.IsIntersectedWith(Bounds))
				{
					ClipRect = ClipRect.Intersection(Bounds);
					RenderRegion* pRegion = DocView::RenderOnTop(&ClipRect, pSpread, UnclippedEOR);
					while (pRegion)
					{
						RenderFillBlobs(pRegion);
						LastRenderedMesh = NULL;

						// Get the Next render region
						pRegion = DocView::GetNextOnTop(&ClipRect);
					}
				}
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}

	RenderRegion* pRegion = DocView::RenderOnTop(&Bounds, pSpread, UnclippedEOR);
	while (pRegion)
	{
		RenderFillBlobs(pRegion);
		LastRenderedMesh = NULL;

		// Get the Next render region
		pRegion = DocView::GetNextOnTop(&Bounds);
	}

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// Since we are removing blobs then force all blobs to be deselected
//	DeselectAllNoRedraw();
#endif
}

/********************************************************************************************
	
// Fill Control Point Selection state functions

********************************************************************************************/

/********************************************************************************************

>	BOOL* AttrFillGeometry::GetSelectionState()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		SelState - Array of BOOLs to represent the Selection of Fill Control Points.
				PointCount - The number of control points you are interested in.
							 Must be <= NUMCONTROLPOINTS
	Purpose:	Get the current Selection state of the Fill Control Points.
	SeeAlso:	AttrFillGeometry::IsSelected; AttrFillGeometry::SetSelectionState; 
				AttrFillGeometry::SetBlobState; 
				AttrFillGeometry::CountSelectionControlPoints; 
				AttrFillGeometry::GetSelectionCount; 
				FillControl

********************************************************************************************/

UINT32 AttrFillGeometry::GetSelectionState(BOOL **pOutputState)
{
	// check the output pointer
	if (pOutputState==NULL)
		return 0;

	// assign an output parameter
	(*pOutputState=NULL);

	// get the total number if blobs in this fill
    UINT32 Count = GetBlobCount();
	if (Count==0)
		return 0;

	// allocate ourselves an array to hold the data.
	BOOL* pArray = (BOOL*)malloc(Count*sizeof(BOOL));
	if (pArray==NULL)
		return 0;

	// Obtain the selection state of each control point
	INT32					i;
	for( i = 0; i<5; i++)
	{
		pArray[i] = IsSelected(i);
	}

	// obtain the selection state of each fill ramp blob
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		pRamp->GetSelectionState(&pArray[i]);

	// assign the output
	(*pOutputState = pArray);
	return Count;
}


/********************************************************************************************

>	UINT32 AttrFillGeometry::GetBlobCount(BOOL IncludeRamps=TRUE)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		-
	Returns:	the number of blobs in this fill geometry (including colour ramps)
	Purpose:	Return the number of editable points in this fill geometry.

********************************************************************************************/

UINT32 AttrFillGeometry::GetBlobCount(BOOL IncludeRamps)
{
	UINT32 count=5;
	if (IncludeRamps)
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
			count+=pRamp->GetCount();
	}
	return count;
}

/*
void AttrFillGeometry::GetSelectionState(BOOL* SelState, INT32 PointCount)
{
    if (PointCount > NUMCONTROLPOINTS)
		PointCount = NUMCONTROLPOINTS;

	// Obtain the selection state of each control point
	for (INT32 i=0; i < PointCount; i++)
	{
		SelState[i] = IsSelected(i);
	}
}
*/



/********************************************************************************************

>	FillRamp* AttrFillGeometry::GetFillRamp() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		-
	Returns:	A fill ramp pointer
	Purpose:	Return the fill ramp associated with this fill geometry. Currently this
				ramp can be either a colour ramp or a transparency ramp. Other ramps might
				well be added at a later time

********************************************************************************************/

FillRamp* AttrFillGeometry::GetFillRamp()
{
	FillRamp*	pRamp = GetColourRamp();
	if (!pRamp) pRamp = GetTranspRamp();
	return pRamp;
}


/********************************************************************************************

>	DocColour* AttrFillGeometry::GetFirstSelectedColour()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		-
	Returns:	
	Purpose:	Get the colour associated with the first selected endpoint. This function
				tends to be used with GetSelectionCount once you've worked out there's only
				one end point selected. At least I use it mostly that way.

********************************************************************************************/

DocColour* AttrFillGeometry::GetFirstSelectedColour()
{
	// Don't bother checking for FILLCONTROL_SECONDARYPOINT as this can never
	// be selected.
	if (IsSelected(FILLCONTROL_STARTPOINT))	return GetStartColour();
	if (IsSelected(FILLCONTROL_ENDPOINT))	return GetEndColour();
	if (IsSelected(FILLCONTROL_ENDPOINT2))	return GetEndColour2();
	if (IsSelected(FILLCONTROL_ENDPOINT3)) 	return GetEndColour3();

	// ok is there a colour ramp in this fill? If so check it too.
	ColourRamp *pRamp = GetColourRamp();
	if (pRamp)
		return pRamp->GetFirstSelectedColour();

	// nothing selected here
	return NULL;
}


/********************************************************************************************

>	UINT32* AttrFillGeometry::GetFirstSelectedTransp()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		-
	Returns:	A pointer to the first selected transparency value. (This can be null as
				defined by WillC, so we need to return a pointer rather than the actual
				value.
	Purpose:	Get the transparency associated with the first selected endpoint. This function
				tends to be used with GetSelectionCount once you've worked out there's only
				one end point selected. At least I use it mostly that way.

********************************************************************************************/

UINT32* AttrFillGeometry::GetFirstSelectedTransp()
{
	if (IsSelected(FILLCONTROL_STARTPOINT))	return GetStartTransp();
	if (IsSelected(FILLCONTROL_ENDPOINT))	return GetEndTransp();
	if (IsSelected(FILLCONTROL_ENDPOINT2))	return GetEndTransp2();
	if (IsSelected(FILLCONTROL_ENDPOINT3)) 	return GetEndTransp3();

	// ok is there a colour ramp in this fill? If so check it too.
	TransparencyRamp *pRamp = GetTranspRamp();
	if (pRamp)
		return pRamp->GetFirstSelectedTransp();

	// nothing selected here
	return NULL;
}



/********************************************************************************************

>	INT32 AttrFillGeometry::GetFirstSelectedIndex()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/3/97
	Inputs:		-
	Returns:	The index of the first selected end blob. [0..n-1]
				-1 if there isn't one
	Purpose:	Find the index of the first selected endpoint. If nothing is selected
				-1 will be returned otherwise a number 0..n-1.

********************************************************************************************/

INT32 AttrFillGeometry::GetFirstSelectedIndex()
{
	// is there a selected blob internal to the geometry?
	for (INT32 i=0; i<FILLCONTROL_SECONDARYPOINT; i++)
	{
		if (IsSelected(i))
			return i;
	}

	// note, all fill geometries either contain a colour ramp
	// or a transparency ramp, but not both!
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		return pRamp->GetSelectedIndex();
	
	// nothing selected
	return -1;
}




/********************************************************************************************

>	void AttrFillGeometry::SetSelectionState(BOOL* SelState, INT32 PointCount)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		SelState - Array of BOOLs to represent the Selection of Fill Control Points.
				PointCount - The number of control points you want to set.
							 Must be <= NUMCONTROLPOINTS
	Purpose:	Set the Selection state of the Fill Control Points.
	SeeAlso:	AttrFillGeometry::IsSelected; AttrFillGeometry::GetSelectionState; 
				AttrFillGeometry::SetBlobState; 
				AttrFillGeometry::CountSelectionControlPoints; 
				AttrFillGeometry::GetSelectionCount; 
				FillControl

********************************************************************************************/
/*
void AttrFillGeometry::SetSelectionState(BOOL* SelState, INT32 PointCount)
{
	if (PointCount > NUMCONTROLPOINTS)
		PointCount = NUMCONTROLPOINTS;

	// Set the selection state of each control point
	for (INT32 i=0; i < PointCount; i++)
	{
		SetBlobState(i, SelState[i]);
	}
}
*/

/********************************************************************************************

>	BOOL AttrFillGeometry::IsSelected(FillControl Control)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/94
	Purpose:	Determines if a Fill Control Point is selected or not.
	SeeAlso:	AttrFillGeometry::SetSelectionState; AttrFillGeometry::GetSelectionState; 
				AttrFillGeometry::SetBlobState; 
				AttrFillGeometry::CountSelectionControlPoints; 
				AttrFillGeometry::GetSelectionCount; 
				FillControl

********************************************************************************************/

BOOL AttrFillGeometry::IsSelected(UINT32 Control)
{
#if !defined(EXCLUDE_FROM_RALPH)
	
	// check for a fill ramp index first.
	if (ISA_RAMPINDEX(Control))
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
			return pRamp->IsSelected(Control);
	}
	
	ERROR2IF(Control > NUMCONTROLPOINTS, FALSE, "Invalid control point in IsSelected()");
	
	// Secondary points can never be selected
	if (Control == FILLCONTROL_SECONDARYPOINT)
		return FALSE;

	return SelectionState[Control];
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	UINT32 AttrFillGeometry::CountSelectionControlPoints()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/94
	Returns:	UINT32, the number of selected fill control points in the selection.
	Purpose:	Get the number of Selected Fill Control Points within the selected objects.
	SeeAlso:	AttrFillGeometry::IsSelected; 
				AttrFillGeometry::SetSelectionState; AttrFillGeometry::GetSelectionState; 
				AttrFillGeometry::SetBlobState; 
				AttrFillGeometry::GetSelectionCount; 
				FillControl

********************************************************************************************/

UINT32 AttrFillGeometry::CountSelectionControlPoints()
{
	UINT32 Count = 0;

#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill)
		return 0;

	CCRuntimeClass* FillType = IsColourMeshVisible() ? CC_RUNTIME_CLASS(AttrFillGeometry)
													 : CC_RUNTIME_CLASS(AttrTranspFillGeometry);

	// Find the first Fill Attribute in the current Selection
	AttrFillGeometry* pAttr = FindFirstSelectedAttr(FillType);

	while (pAttr != NULL)
	{
		// Count this fill selected control points
		// and keep a running total
		if (pAttr->IsVisible())
			Count += pAttr->GetSelectionCount();

		// Move on to the next Fill
		pAttr = FindNextSelectedAttr(FillType);
	}

#endif
	// Count = Total number of fill control points
	// selected within the current Selection
	return Count;
}

/********************************************************************************************

>	UINT32 AttrFillGeometry::GetSelectionCount()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/94
	Returns:	UINT32, the number of selected control points in this fill.
	Purpose:	Get the number of Selected Fill Control Points.
	SeeAlso:	AttrFillGeometry::CountSelectionControlPoints;
				AttrFillGeometry::IsSelected; 
				AttrFillGeometry::SetSelectionState; AttrFillGeometry::GetSelectionState; 
				AttrFillGeometry::SetBlobState; 
				FillControl

********************************************************************************************/

UINT32 AttrFillGeometry::GetSelectionCount()
{
	UINT32 Count = 0;
#if !defined(EXCLUDE_FROM_RALPH)
	// Go though each control point, and increament
	// a count if the point is selected.
	for (INT32 i=0; i < FILLCONTROL_SECONDARYPOINT; i++)
	{
		if (IsSelected(i)) Count++;
	}

	// add in selected blobs on any colour ramp or transparency ramp
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		Count += pRamp->CountSelBlobs();

#endif
	// Number of Selected Fill Control Points
	return Count;
}

/********************************************************************************************

>	void AttrFillGeometry::SetBlobState(FillControl HitControl, BOOL Selected)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		HitControl, the FillControl to set the state of.
				Selected, a BOOL indicating whether to select or deslect this Point.
	Purpose:	Sets the selection state of a particular Control Point. 
	SeeAlso:	AttrFillGeometry::IsSelected; 
				AttrFillGeometry::SetSelectionState; AttrFillGeometry::GetSelectionState; 
				AttrFillGeometry::CountSelectionControlPoints;
				AttrFillGeometry::GetSelectionCount; 
				FillControl

********************************************************************************************/

void AttrFillGeometry::SetBlobState(FillControl HitControl, BOOL NewState)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ChangeBlobState(HitControl, (NewState ? BLOBSTATE_ON : BLOBSTATE_OFF));
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::SelectBlob(FillControl HitControl)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		HitControl, the FillControl that is to be selected.
	Purpose:	Selects a Fill Control Point. 
	SeeAlso:	AttrFillGeometry::DeselectBlob;
				AttrFillGeometry::ToggleSelection;
				FillControl;

********************************************************************************************/

void AttrFillGeometry::SelectBlob(FillControl HitControl)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ChangeBlobState(HitControl, BLOBSTATE_ON);
#endif
}


/********************************************************************************************

>	void AttrFillGeometry::DeselectBlob(FillControl HitControl)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		HitControl, the FillControl that is to be Deselected.
	Purpose:	Deselects a particular Fill Control Point. 
	SeeAlso:	AttrFillGeometry::SelectBlob;
				AttrFillGeometry::ToggleSelection;
				FillControl;

********************************************************************************************/

void AttrFillGeometry::DeselectBlob(FillControl HitControl)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ChangeBlobState(HitControl, BLOBSTATE_OFF);
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::ToggleSelection(FillControl HitControl)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		HitControl, the FillControl who's state is to be toggled.
	Purpose:	Toggle the selection of a Fill Control Point. 
	SeeAlso:	AttrFillGeometry::SelectBlob;
				AttrFillGeometry::DeselectBlob;
				FillControl;

********************************************************************************************/

void AttrFillGeometry::ToggleSelection(FillControl HitControl)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ChangeBlobState(HitControl, BLOBSTATE_TOGGLE);
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::ChangeBlobState(FillControl HitControl, INT32 state)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		HitControl, the FillControl that is to be selected.
				state = 0 = deselect blob
						1 = select blob
						2 = toggle blob selection
	Purpose:	Sets the state of a fill blob

********************************************************************************************/

void AttrFillGeometry::ChangeBlobState(FillControl HitControl, INT32 state)
{
#if !defined(EXCLUDE_FROM_RALPH)

	// check for a ramp index first - Added by Mike 10/3/97
	if (ISA_RAMPINDEX(HitControl))
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
		{
			RenderControl(HitControl,FALSE);
			pRamp->SetSelState(HitControl,state);
			RenderControl(HitControl,state);	
			// TODO: NEED TO DO SOMETHING ABOUT - SetLastRenderedBlob(HitControl);
			//SetLastRenderedBlob(HitControl);
		}
		return;
	}

	// Can't Select DragPoints
	if (HitControl >= FILLCONTROL_DRAGPOINT)
		return;

	// Secondary Point selection state is a mirror of End Point selection
	if (HitControl == FILLCONTROL_SECONDARYPOINT)
		HitControl = FILLCONTROL_ENDPOINT;

	// Save the current state of the blob
	BOOL oldstate = SelectionState[HitControl];
	BOOL newstate = oldstate;

	// now alter to the new state
	switch (state)
	{
		case BLOBSTATE_OFF:	
			newstate = FALSE; 
		break;
		case BLOBSTATE_ON: 
			newstate = TRUE;	
		break;
		case BLOBSTATE_TOGGLE: 
			newstate = !oldstate;
		break;
	}

	// has the state changed?
	if (oldstate != newstate)
	{
		// Undraw the old blob
		RenderControl(HitControl, FALSE);
		SelectionState[HitControl] = newstate;
		// Redraw the new blob
		RenderControl(HitControl, TRUE);
		SetLastRenderedBlob(HitControl);
	}
#endif
}



/********************************************************************************************

>	virtual void AttrFillGeometry::CycleSelection(BOOL Reverse)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/2000
	Purpose:	Cycles the selection state of the controls

********************************************************************************************/

void AttrFillGeometry::CycleSelection(BOOL Reverse)
{
	// have we got some sort of colour ramp?
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
	{
		// ok, we have a ramp involved, so lets cycle

		BOOL StartPointSelected, EndPointSelected;
		FillControl Result, OldSelectedBlob; //NewSelectedBlob;

		if (Reverse)
		{
			StartPointSelected  = IsSelected (FILLCONTROL_STARTPOINT);
			EndPointSelected = IsSelected (FILLCONTROL_ENDPOINT);

			Result = pRamp->RotateSelLeft (StartPointSelected, EndPointSelected, OldSelectedBlob);

			switch (Result)
			{
				case FILLCONTROL_STARTPOINT:
					if (OldSelectedBlob != FILLCONTROL_NULL)
					{
						SetBlobState (OldSelectedBlob, TRUE);
						pRamp->SetSelState (OldSelectedBlob, FALSE);
					}
					
					SetBlobState (Result, TRUE);
				break;
				case FILLCONTROL_ENDPOINT:
					ToggleSelection(FILLCONTROL_STARTPOINT);
					SetBlobState (Result, TRUE);
				break;
				default:
					if (ISA_RAMPINDEX (Result))
					{
						if (StartPointSelected) { SetBlobState (FILLCONTROL_STARTPOINT, FALSE); }
						if (EndPointSelected) { SetBlobState (FILLCONTROL_ENDPOINT, FALSE); }

						if (OldSelectedBlob != FILLCONTROL_NULL)
						{
							SetBlobState (OldSelectedBlob, TRUE);
							pRamp->SetSelState (OldSelectedBlob, FALSE);
						}
						SetBlobState (Result, TRUE);
					}
					
					// do nothing - since handled in RotateSelRight
					return;
			}
		}
		else
		{
			StartPointSelected  = IsSelected (FILLCONTROL_STARTPOINT);
			EndPointSelected = IsSelected (FILLCONTROL_ENDPOINT);

			Result = pRamp->RotateSelRight (StartPointSelected, EndPointSelected, OldSelectedBlob);

			switch (Result)
			{
				case FILLCONTROL_STARTPOINT:
					ToggleSelection(FILLCONTROL_ENDPOINT);
					SetBlobState (Result, TRUE);
				break;
				case FILLCONTROL_ENDPOINT:
					if (OldSelectedBlob != FILLCONTROL_NULL)
					{
						SetBlobState (OldSelectedBlob, TRUE);
						pRamp->SetSelState (OldSelectedBlob, FALSE);
					}
					
					SetBlobState (Result, TRUE);
				break;
				default:
					if (ISA_RAMPINDEX (Result))
					{
						if (StartPointSelected) { SetBlobState (FILLCONTROL_STARTPOINT, FALSE); }
						if (EndPointSelected) { SetBlobState (FILLCONTROL_ENDPOINT, FALSE); }

						if (OldSelectedBlob != FILLCONTROL_NULL)
						{
							SetBlobState (OldSelectedBlob, TRUE);
							pRamp->SetSelState (OldSelectedBlob, FALSE);
						}
						SetBlobState (Result, TRUE);
					}
					
					// do nothing - since handled in RotateSelRight
					return;
			}
		}		
		return;
	}
	else
	{
		if (GetSelectionCount() == 1)
		{
			ToggleSelection(FILLCONTROL_STARTPOINT);
			ToggleSelection(FILLCONTROL_ENDPOINT);
		}
	}
}


/********************************************************************************************

>	void AttrFillGeometry::DeselectAllNoRedraw()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Purpose:	Deselects all of the Fill Control Points for this fill,
				without redawing anything

********************************************************************************************/

void AttrFillGeometry::DeselectAllNoRedraw()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Force all our control points to be deselected
	SelectionState[FILLCONTROL_STARTPOINT]		= FALSE;
	SelectionState[FILLCONTROL_ENDPOINT]		= FALSE;
	SelectionState[FILLCONTROL_ENDPOINT2]		= FALSE;
	SelectionState[FILLCONTROL_ENDPOINT3]		= FALSE;
	SelectionState[FILLCONTROL_SECONDARYPOINT]	= FALSE;
	// And all our colour ramp control points to.
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		pRamp->DeselectAll();
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::DeselectAll()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Purpose:	Deselects all of the Fill Control Points for this fill. 

********************************************************************************************/

void AttrFillGeometry::DeselectAll()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Force all our control points to be deselected
	ChangeBlobState(FILLCONTROL_STARTPOINT,BLOBSTATE_OFF);
	ChangeBlobState(FILLCONTROL_ENDPOINT  ,BLOBSTATE_OFF);
	ChangeBlobState(FILLCONTROL_ENDPOINT2 ,BLOBSTATE_OFF);
	ChangeBlobState(FILLCONTROL_ENDPOINT3 ,BLOBSTATE_OFF);

	// And all our colour/transparency ramp control points to.
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
	{
		UINT32 first,last;
		if (pRamp->GetIndexRange(&first,&last))
		{
			for (UINT32 i=first; i<=last; i++)
				ChangeBlobState(i ,BLOBSTATE_OFF);
		}
	}
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::DeselectAllBut(FillControl HitControl)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/07/94
	Inputs:		HitControl, the FillControl that should NOT be deselected.
	Purpose:	Deselects all but one of the Fill Control Points. 

********************************************************************************************/

void AttrFillGeometry::DeselectAllBut(FillControl HitControl)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Secondary Point selection state is a mirror of End Point selection
	if (HitControl == FILLCONTROL_SECONDARYPOINT)
		HitControl = FILLCONTROL_ENDPOINT;
	
	for (INT32 i=0; i<FILLCONTROL_SECONDARYPOINT; i++)
	{
		if ((HitControl!=i) && SelectionState[i])
			ChangeBlobState(i,BLOBSTATE_OFF);
	}

	// And all our colour/transparency ramp control points to.
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
	{
		UINT32 first,last;
		UINT32 hc = (UINT32)HitControl;
		if (pRamp->GetIndexRange(&first,&last))
		{
			for (UINT32 i=first; i<=last; i++)
				if (hc!=i)	
					ChangeBlobState(i ,BLOBSTATE_OFF);
		}
	}
	
#endif
}

/********************************************************************************************

>	FillControl AttrFillGeometry::CheckForControlHit(DocCoord &ClickPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrFillGeometry::CheckForControlHit(DocCoord &ClickPos)
{
	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
	DocRect BlobRect;

	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	// Ok see if we've hit a fill ramp blob.
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
	{
		// have we hit a blob on the fill ramp?
		HitControl = pRamp->HitBlob(ATTRVALUE(), ClickPos);
		if (!ISA_RAMPINDEX(HitControl))
		{
			HitControl=FILLCONTROL_NULL;
		}
		else
		{
			// if we hit a fill ramp blob, then we need to stop others points from grabbing the drag!
			return (HitControl);
		}
	}

	// otherwise try the geometry points.
	if (GetStartPoint() != NULL)
	{
		// Get the rectangle around the Start Control Point
		(Camelot.GetBlobManager())->GetBlobRect(*GetStartPoint(), &BlobRect);
		// See if the Click Position is within the rectangle
		if ( BlobRect.ContainsCoord(ClickPos) )
			HitControl = FILLCONTROL_STARTPOINT;
	}

	if (GetEndPoint() != NULL)
	{
		// Get the rectangle around the end control point
		(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint(), &BlobRect);
		// See if the Click Position is within the rectangle
		if ( BlobRect.ContainsCoord(ClickPos) )
			HitControl = FILLCONTROL_ENDPOINT;
	}

	if (GetEndPoint2() != NULL)
	{
		// Get the rectangle around the secondary control point
		(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint2(), &BlobRect);
		// See if the Click Position is within the rectangle
		if ( BlobRect.ContainsCoord(ClickPos) )
			HitControl = FILLCONTROL_SECONDARYPOINT;
	}

	if (DoCheckOnFillRampMesh)
	{
		// DMc
		if (HitControl == FILLCONTROL_NULL)
		{
			// check for a hit along the linear fill line
			// first, get the line
			if (GetStartPoint() != NULL && GetEndPoint() != NULL)
			{
				// check the bounding rect first
				DocCoord StartPoint = *(GetStartPoint());
				DocCoord EndPoint   = *(GetEndPoint());

				double d = FindRampPoint(ClickPos, StartPoint, EndPoint);

				if ( d > 0 && d < 1.0)
				{
					HitControl = FILLCONTROL_RAMPPOINT;
				}		
			}
		}
	}

	return HitControl;
}

/********************************************************************************************

>	void AttrFillGeometry::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		Control, the FillControl to redraw
				RenderOn = TRUE, if we're putting the control on. FALSE if taking it off.
	Purpose:	Redraws the Fill Control blobs when the selection changes.
	SeeAlso:	FillControl			

********************************************************************************************/

void AttrFillGeometry::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();
	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	if (ISA_RAMPINDEX(Control))
	{
		FillRamp* pRamp = GetFillRamp();
		if (pRamp)
		{
			DocCoord point = pRamp->GetGeometryCoord(ATTRVALUE(), Control);
			(Camelot.GetBlobManager())->GetBlobRect(point, &ControlRect, TRUE);
			RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
		}
		return;
	}	
	
	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:
			if (GetStartPoint() != NULL)
			{
				// Redraw the Start Point Blob
				(Camelot.GetBlobManager())->GetBlobRect(*GetStartPoint(), &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT:
		case FILLCONTROL_SECONDARYPOINT:
		{
			if (GetEndPoint() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint(), &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint2(), &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
		}
		break;
	}
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::RenderFillControl(BOOL RenderOn, 
									 DocRect* ControlRect, 
									 Spread* pSpread, 
									 NodeRenderable* pParent)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	Renders a fill control

********************************************************************************************/

void AttrFillGeometry::RenderFillControl(BOOL RenderOn, 
									 DocRect* ControlRect, 
									 Spread* pSpread, 
									 NodeRenderable* pParent)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!RenderOn)
	{
		RenderControlIntoPendingRegions(ControlRect, pSpread, pParent);
	}

	(Camelot.GetBlobManager())->RenderMyBlobs(ControlRect, pSpread, pParent);
	AttrFillGeometry::LastRenderedMesh = NULL;

	if (RenderOn)
	{
		RenderControlIntoPendingRegions(ControlRect, pSpread, pParent);
	}
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::RenderControlIntoPendingRegions( DocRect* ControlRect, 
															Spread* pSpread, 
															NodeRenderable* pParent)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/95
	Purpose:	Renders the fill control into all pending render regions.

********************************************************************************************/

void AttrFillGeometry::RenderControlIntoPendingRegions( DocRect* ControlRect, 
														Spread* pSpread, 
														NodeRenderable* pParent)
{
#if !defined(EXCLUDE_FROM_RALPH)
	RenderRegionList* pRegionList = GetApplication()->GetRegionList();
	
	if (!pRegionList->IsEmpty())
	{
		RenderRegion* pRegion = (RenderRegion*)pRegionList->GetHead();	
		
		while (pRegion)
		{
			// Check the RenderRegion is for the same spread.
			if (pRegion->GetRenderSpread() == pSpread &&
				(pRegion->IsInkRenderStarted || pRegion->NeedsOSPaper))
			{
				// Render the blobs 'clipped' to this Render Region.
				DocRect ClipRect = pRegion->GetRegionRect();
		
				if (ClipRect.IsIntersectedWith(*ControlRect))
				{
					ClipRect = ClipRect.Intersection(*ControlRect);
					(Camelot.GetBlobManager())->RenderMyBlobs(&ClipRect, pSpread, pParent);
				}
			}

			// Get the Next render region
			pRegion = (RenderRegion*)pRegionList->GetNext(pRegion);
		}
	}

	AttrFillGeometry::LastRenderedMesh = NULL;
#endif
}

/********************************************************************************************

>	virtual void AttrFillGeometry::OnControlDrag( DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a fill control point.

********************************************************************************************/

void AttrFillGeometry::OnControlDrag( DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)

//	Document * pDoc = Document::GetCurrent();

	if (ISA_RAMPINDEX(DragControl))
	{
		// Aha, we're dragging a point on the fill ramp. We need to do all sorts of
		// gubbins here. The point being dragged will need to be snapped to the geometry
		// of the fill.
		// Algorithm is simply
		//   (1) FindClosestPointInGeometrySpace(InCoord, OutParam)
		//	 (2) pRamp->SetPosition(DragControl, OutParam);
	
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
		{
			float t = ATTRVALUE()->GetGeometryParam(Pos);
	
			FillControl NewDragControl = pRamp->SetPosition(DragControl, t);

			if (NewDragControl != DragControl)
			{
				DragControl = NewDragControl;
			}
	//		pRamp->SortRamp (TRUE);
		}
		return;
	}

	// Is the Control Point Valid ?
	ERROR3IF(DragControl > NUMCONTROLPOINTS, "Invalid control point in OnControlDrag()");

	// Which control is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:

			if (GetStartPoint() != NULL)
			{
				// They're dragging the Start Point
				if (ClickMods.Constrain)
				{
					// If the Constrain key is down then constrain the Angle of the
					// point, relative to the other end.
					DocView::ConstrainToAngle(*GetEndPoint(), &Pos);
				}

				SetStartPoint(&Pos);
				DocCoord temp = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
				SetEndPoint2(&temp);
			}
			break;

		case FILLCONTROL_ENDPOINT:

			if (GetEndPoint() != NULL)
			{
				// They're dragging the End Point
				if (ClickMods.Constrain)
				{
					// If the Constrain key is down then constrain the Angle of the
					// point, relative to the other end.
					DocView::ConstrainToAngle(*GetStartPoint(), &Pos);
				}

				SetEndPoint(&Pos);
				DocCoord temp = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
				SetEndPoint2(&temp);
			}
			break;

		case FILLCONTROL_SECONDARYPOINT:

			if (GetEndPoint2() != NULL)
			{
				// They're dragging the Secondary Point
				if (ClickMods.Constrain)
				{
					// If the Constrain key is down then constrain the Angle of the
					// point, relative to the other end.
					DocView::ConstrainToAngle(*GetStartPoint(), &Pos);
				}

				SetEndPoint2(&Pos);
			}
			break;
	}
#endif
}

void AttrFillGeometry::SetAspectRatio(double Ratio)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord Start 	= *GetStartPoint();
	DocCoord End 	= *GetEndPoint();

	DocCoord End2 = MakeLineAtAngle(Start, 
									End, 
									90, 
									INT32(Start.Distance(End) * Ratio)
									);

	SetEndPoint2(&End2);
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::GetControlPoints(DocCoord* PointArray, INT32 NumControls)	

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:     PointArray, an array of DocCoords to update with control positions
				NumControl, the number of Control points to get.
	Outputs:	PointArray is updated with the positions of the Fill Control Points.
	Purpose:	Get the positions of fill control points.

********************************************************************************************/

void AttrFillGeometry::GetControlPoints(DocCoord* PointArray, INT32 NumControls)	
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Make sure nobodys being silly
	if (NumControls > NUMCONTROLPOINTS)
		NumControls = NUMCONTROLPOINTS;

	// Make a tempory copy of the points positions
	DocCoord Controls[NUMCONTROLPOINTS];
	
	if (GetStartPoint() != NULL)
		Controls[FILLCONTROL_STARTPOINT] = *GetStartPoint();
	if (GetEndPoint() != NULL)
		Controls[FILLCONTROL_ENDPOINT]   = *GetEndPoint();
	if (GetEndPoint2() != NULL)
		Controls[FILLCONTROL_SECONDARYPOINT] = *GetEndPoint2();
	if (GetEndPoint2() != NULL)
		Controls[FILLCONTROL_ENDPOINT2]   = *GetEndPoint2();
	if (GetEndPoint3() != NULL)
		Controls[FILLCONTROL_ENDPOINT3]   = *GetEndPoint3();

	// Copy the positions into the callers array
	for (INT32 i=0; i < NumControls; i++)
	{
		PointArray[i] = Controls [i];
	}
#endif
}

/********************************************************************************************

>	void AttrFillGeometry::SetBoundingRect(DocRect &NewBounds)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Returns:	DocRect - The rectangle specifying the new bounds.
	Purpose:	Set the Attributes bounding rectangle, which is used for current attributes
				and scaling when applying to new ink nodes.
				This rect is only used for 'Current' Attributes which don't have any parent
				bounds. It allows us to scale current attributes when they are applied.

********************************************************************************************/

void AttrFillGeometry::SetBoundingRect(DocRect &NewBounds)
{ 
	// Er.. Update the Bounding Rectangle thingy.
	// This rect is only used for 'Current' Attributes which don't have any parent bounds
	// It allows us to scale current attributes when they are applied.
	AttrBounds = NewBounds;
}

/********************************************************************************************

>	BOOL AttrFillGeometry::WriteBoundingRect(BaseCamelotFilter* pFilter)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/03/2004
	Returns:	DocRect - The rectangle specifying the new bounds.
	Purpose:	Write the Attributes bounding rectangle.

********************************************************************************************/

BOOL AttrFillGeometry::WriteBoundingRect(BaseCamelotFilter* pFilter)
{
	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter, TAG_CURRENTATTRIBUTEBOUNDS, TAG_CURRENTATTRIBUTEBOUNDS_SIZE);

	ok = Rec.Init();

	if (ok) ok = Rec.WriteCoord(AttrBounds.lo);	// write out low corner details
	if (ok) ok = Rec.WriteCoord(AttrBounds.hi);	// write out high corner details

	// Finally, write the record out to file
	// In the process get the record number that this was written out as
	INT32 RecordNumber = 0L;
	if (ok) RecordNumber = pFilter->Write(&Rec);
	
	// If we have had a problem at any of the stages then return that to the caller
	return (ok && RecordNumber > 0);
}

/********************************************************************************************

>	void AttrFillGeometry::TransformSelectedControlPoints( TransformBase& Trans)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the selected control points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrFillGeometry::TransformSelectedControlPoints( TransformBase& Trans, BOOL* isARampBlob /*= NULL*/)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ( Trans.TransFills )
	{
		ClickModifiers ClickMods; /* = ClickModifiers::GetClickModifiers();*/
		ClickMods.Adjust = TRUE;

		if (GetStartPoint() && IsSelected(FILLCONTROL_STARTPOINT))
		{
			DocCoord Pos = *GetStartPoint();
			Trans.Transform( &Pos, 1);

			FillControl Start = FILLCONTROL_STARTPOINT;

			OnControlDrag(Pos, Start, ClickMods);
		}

		if (GetEndPoint() && IsSelected(FILLCONTROL_ENDPOINT))
		{
			DocCoord Pos = *GetEndPoint();
			Trans.Transform( &Pos, 1);

			FillControl End = FILLCONTROL_ENDPOINT;

			OnControlDrag(Pos, End, ClickMods);
		}

		// CGS:  wo - check the fill ramp as well ....

		TransformSelectedFillRampControlPoints (Trans, ClickMods, isARampBlob);
	}
#endif
}



/********************************************************************************************

>	void AttrFillGeometry::TransformSelectedFillRampControlPoints (TransformBase& Trans, ClickModifiers ClickMods)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/2000
	Inputs:		Trans - the transform object to apply to this attribute.
				ClickMods - click modifiers
	Purpose:	Transform a grad fill attributes fill ramp by moving the selected control points.
	SeeAlso:	AttrFillGeometry::TransformSelectedControlPoints

********************************************************************************************/
void AttrFillGeometry::TransformSelectedFillRampControlPoints (TransformBase& Trans, ClickModifiers ClickMods, BOOL* isARampBlob)
{
	if (GetFillRamp ())
	{
		// there is a fill ramp ....

		FillRamp* pRamp = GetFillRamp ();

		// NOTE:  the following code is written with the assumption
		// that there is ONLY EVER one blob selected.  If this restriction
		// is altered one-day, then the following code will need to be
		// revised ....

		if (pRamp->CountSelBlobs () > 0)
		{
			ERROR3IF (pRamp->CountSelBlobs () > 1, "More than one blob is selected! (extras will be ignored)");

			if (isARampBlob)
			{
				*isARampBlob = TRUE;		// cause it is
			}

			// get the index of the first ....

			INT32 sel = pRamp->GetSelectedIndex ();

			DocCoord Pos = pRamp->GetGeometryCoord ((FillGeometryAttribute*) GetAttributeValue (), sel);
			Trans.Transform( &Pos, 1);

			FillControl control = sel;

			//DisableBoundsRedraw ();
			OnControlDrag(Pos, control, ClickMods);
			//EnableBoundsRedraw ();
		}
	}
}



/********************************************************************************************

>	virtual void AttrFillGeometry::TransformToNewBounds(DocRect& NewBounds) 

	Author:		Simon_Maneggio (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/9/95
	Inputs:		The attributes new bounds. Usually the bounds of the object/s that the 
				attribute will be applied to.
	Purpose:	Moves and scales the attribute's bounds to fit NewBounds. This function
				is used when applying an attribute which has been stored (eg. as a Current
				attribute) to a new object.

********************************************************************************************/

void AttrFillGeometry::TransformToNewBounds(DocRect& NewBounds)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// We are a funny fill thingy, so we better do some transforming
	DocRect AttrBounds = GetBoundingRect();

	// If we have valid Node and Attribute Bounds then make a transform
	// that is the scale and offset of the two rectangles.
	if (!NewBounds.IsEmpty() && !AttrBounds.IsEmpty())
	{
		// Get the Width and Height of the two rectangles
		INT32 NodeWidth  = NewBounds.Width();
		INT32 NodeHeight = NewBounds.Height();
		INT32 AttrWidth  = AttrBounds.Width();
		INT32 AttrHeight = AttrBounds.Height();

		// Find the Centre of each rectangle
		NewBounds.Translate(NodeWidth/2, NodeHeight/2);
		AttrBounds.Translate(AttrWidth/2, AttrHeight/2);
		DocCoord NodeCentre = NewBounds.lo;
		DocCoord AttrCentre = AttrBounds.lo;

		// And now make a transform for the attribute
		// First move the old attribute position to the origin
		Matrix AttrTrans = Matrix(-AttrCentre.x, -AttrCentre.y);

		// Don't scale for bitmap/fractal fills/transparency.
		if (!IS_KIND_OF(AttrBitmapFill))
		{
			// Calculate the difference in size between the two rectangles & scale accordingly.
			AttrTrans *= Matrix(FIXED16(double(NodeWidth) / double(AttrWidth)),
								FIXED16(double(NodeHeight) / double(AttrHeight)));
		}

		// And finally move it to the new position
		AttrTrans *= Matrix(NodeCentre.x, NodeCentre.y);
		Trans2DMatrix Trans = Trans2DMatrix(AttrTrans);

		// Go transform that Attribute !!
		Transform(Trans);
	}
#endif
}



/********************************************************************************************

>	BOOL AttrFillGeometry::CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/9/2000
	Inputs:		SrcDoc:   The document from where this node was copied
				NodesDoc: The document where this node lives 
	Outputs:	-
	Returns:	FALSE if unable to copy data
	Purpose:	If the attribute contains any DocColours which are indexed then a copy
				of the indexed colour is made and added to the NodesDoc
				ColourListComponent.
	Errors:		-
	SeeAlso:	NodeRenderable::CopyComponentData, NodeAttribute::CopyComponentData

	NOTE:		The implementation of this function with NodeAttribute is very poor.
				The code does NOT provide an oppurtunity for the colour ramp to be copied.
				This overidden function fixes this problem, and provides a more robust implementatiom.

********************************************************************************************/

BOOL AttrFillGeometry::CopyComponentData(BaseDocument* SrcDoc, BaseDocument* NodesDoc)
{
	// Ask the base class to copy its data
	if (!NodeRenderable::CopyComponentData(SrcDoc, NodesDoc))
	{
		return FALSE; // Failed
	}
	// Get the colour list component
	ColourListComponent *pComponent = 
		(ColourListComponent *) NodesDoc->GetDocComponent(CC_RUNTIME_CLASS(ColourListComponent));

	ENSURE (pComponent != NULL, "Could not find ColourListComponent");

	// Copy across all DocColours
	
	DocColour* pDocCol = GetStartColour ();
	if (pDocCol) if (pComponent->CopyColourAcross(pDocCol) == CCCOPY_FAILED) { return (FALSE); }
	pDocCol = GetEndColour ();
	if (pDocCol) if (pComponent->CopyColourAcross(pDocCol) == CCCOPY_FAILED) { return (FALSE); }		
	pDocCol = GetEndColour2 ();
	if (pDocCol) if (pComponent->CopyColourAcross(pDocCol) == CCCOPY_FAILED) { return (FALSE); }
	pDocCol = GetEndColour3 ();
	if (pDocCol) if (pComponent->CopyColourAcross(pDocCol) == CCCOPY_FAILED) { return (FALSE); }

	ColourRamp *pRamp = GetColourRamp();

	if (pRamp)
	{
		return (pRamp->CopyComponentData (SrcDoc, NodesDoc));
	}

	return (TRUE);
}

/********************************************************************************************

>	virtual DocRect AttrFillGeometry::GetBlobBoundingRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Returns:	DocRect - The rectangle that contains all the attrs selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrFillGeometry::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	DocCoord StartPoint = DocCoord(0,0);
	DocCoord EndPoint = DocCoord(0,0);

	// Get the Start and End Points
	if (GetStartPoint() != NULL)
		StartPoint = *GetStartPoint();
	if (GetEndPoint() != NULL)
		EndPoint = *GetEndPoint();

	DocRect StartBlobRect;
	DocRect EndBlobRect;
	// Make a dummy bounds from just the Start Point
	DocRect BoundingRect(StartPoint, StartPoint);

	// Get the Bounding rect of Blobs on each of the ends
	(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &StartBlobRect);
	(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &EndBlobRect);

	// Now include the Bottom Left and Top Right of each blob in the Bounds.
	// We have to do it like this to make sure that the DocRect's coords
	// are valid.  ie. The Hi's are Higher than the Lo's.
	BoundingRect.IncludePoint(StartBlobRect.lo);
	BoundingRect.IncludePoint(StartBlobRect.hi);
	BoundingRect.IncludePoint(EndBlobRect.lo);
	BoundingRect.IncludePoint(EndBlobRect.hi);

	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0, 0,0);
#endif
}

/********************************************************************************************

>	AttrFillGeometry::AttributeChanged()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/94
	Purpose:	Called by someone who has changed something about us that it thinks
				we should know about.  It used by the Attribute manager to make a
				current attribute validate itself after it has been added to a new
				object.
				This functions just calls 'ValidateAttributeValue'.
	SeeAlso:	AttrFillGeometry::ValidateAttributeValue

********************************************************************************************/

void AttrFillGeometry::AttributeChanged()
{
	ValidateAttributeValue();
}

/********************************************************************************************

>	AttrFillGeometry* AttrFillGeometry::Mutate(AttrFillGeometry* Mutator, BOOL bOptimise = TRUE)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		Mutator, The AttrFillGeometry Mutator that will change this fill in some way.
	Returns:	A pointer to a new AttrFillGeometry, that is a Mutated version of this Fill. 
	Purpose:	Changes a fill in some way.

				If the Mutator is an 'AttrValueChange' then the fill will attempt to change
				any selected control points to be the colour of the Mutator.  If sucessful
				(ie. points were selected) then it returns a Pointer to a copy of itself
				with the mutators colours. If no points are selected anywhere in either
				this fill or any others in the selection then the Fill will return a pointer 
				to an 'AttrFlatColourFill' with the colours set to the Mutants.

				If the Mutator is NOT an 'AttrValueChange' then the fill will return a pointer
				to a fill of the same type as the Mutator, but with the fills own control
				points and colours.

	SeeAlso:	AttributeManager::AttributeSelected

********************************************************************************************/

AttrFillGeometry* AttrFillGeometry::Mutate(AttrFillGeometry* Mutator, BOOL bOptimise)
{
	AttrFillGeometry* NewFill = NULL;
#if !defined(EXCLUDE_FROM_RALPH)

//	SelectionCount = CountSelectionControlPoints();

	// Is it a Value Change mutator ?
	// and if so does the type match this attribute ?
	if ( Mutator->IsAValueChange() &&
		 Mutator->GetAttributeType() == GetAttributeType() )
	{
		// We need to change this attribute in some way, depending on the
		// mutator.  It knows what it wants to do, so we will ask it to
		// mutate 'this', and it will return a mutated copy
		NewFill = ((AttrValueChange*)Mutator)->MutateFill(this);
		if (NewFill == NULL)
			return NULL;
	}
	else
	{
		// It must be a 'Geometry change'
		// We will change this fill into another different type of fill,
		// but retain our control points and colours etc.

		if (IsPerspective() && 
		   (Mutator->GetRuntimeClass() != GetRuntimeClass()))
			return NULL;		// Can't mutate to a different fill when perspectivised

		// Make new fill with the same type as the Mutant
		CCRuntimeClass* ObjectType = Mutator->GetRuntimeClass();
		NewFill = (AttrFillGeometry*)ObjectType->CreateObject();
		if (NewFill == NULL)
			return NULL;

		// Copy the Attribute Bounds (used for Current Attributes)
		NewFill->AttrBounds = AttrBounds;

		// and copy our Values into it into the new fill
		*((FillGeometryAttribute*)NewFill->GetAttributeValue()) = *((FillGeometryAttribute*)this->GetAttributeValue());

		if ((!NewFill->IsABitmapFill() || NewFill->IsAFractalFill()) &&
			FindParent() != NULL && FindParent()->IsNodeDocument())
		{
			// If it's the default Fill attr, then we need to use Blank instead
			// of the default fill colour of 'None'
			DocColour DefFillCol;
			AttributeManager::FindDefaultColour(ColourManager::GetCurrentColourList(),
												_R(IDS_BLACKNAME), &DefFillCol);
			if (NewFill->IsAFlatFill())
				NewFill->SetStartColour(&DefFillCol);
			else
				NewFill->SetEndColour(&DefFillCol);
		}

		// Ensure any aspect lock is taken from the mutator
		((FillGeometryAttribute*)NewFill->GetAttributeValue())->
			SetAspectLock(((FillGeometryAttribute*)Mutator->GetAttributeValue())->IsAspectLocked());

		// Ensure our profile type is retained (i.e.  NOT taken from the mutator)
		((FillGeometryAttribute*)NewFill->GetAttributeValue())->
			SetProfile(((FillGeometryAttribute*)this->GetAttributeValue())->GetProfile());

		// Ensure our transparency type is retained (ie. NOT taken from the mutator)
		((FillGeometryAttribute*)NewFill->GetAttributeValue())->
			SetTranspType(((FillGeometryAttribute*)this->GetAttributeValue())->GetTranspType());

		// Check if we're mutating a fractal into a something
		if (this->IsAFractalFill() && Mutator->IsAFractalFill())
		{
			// if so then retain its graininess
			((FillGeometryAttribute*)NewFill->GetAttributeValue())->
				SetGraininess(((FillGeometryAttribute*)this->GetAttributeValue())->GetGraininess());
		}

		if (!OpEditFill::CreateFill)
		{
			if (Mutator->GetBitmap() == NULL && !this->IsAFractalFill())
			{
				// No bitmap has been specified in the mutator, 
				// so lets make a default one
				NewFill->CopyBitmap(KernelBitmap::MakeKernelBitmap());
			}
			else
			{
				// Copy the bitmap from the mutator into the new fill
				NewFill->CopyBitmap(Mutator->GetBitmap());
			}

			// Get the fractal seed from the Mutator, so we create a new random fractal
			((FillGeometryAttribute*)NewFill->GetAttributeValue())->
				SetSeed(((FillGeometryAttribute*)Mutator->GetAttributeValue())->GetSeed());

			NewFill->RecalcFractal();
		}
		else
		{
			NewFill->SetStartPoint(Mutator->GetStartPoint());
			NewFill->SetEndPoint(Mutator->GetEndPoint());
			NewFill->SetEndPoint2(Mutator->GetEndPoint2());
		}

/*
		if (Mutator->GetBitmap() &&
			!BfxALU::IsGreyscaleBitmap(Mutator->GetBitmap()))
		{
			// The new bitmap is not Greyscale, so make sure we
			// lose any contone colours
			
			NewFill->SetStartColour(NULL);
			NewFill->SetEndColour(NULL);
		}
*/

		NewFill->SetBlobState(FILLCONTROL_ENDPOINT, TRUE);

		// Update the mutator's tileable flag, so it can set the
		// fill mapping correctly, when the secondary attribute
		// is obtained
		Mutator->SetTileable(NewFill->GetTileable());
	}

	// If the resulting attribute in the tree is the same as the mutator
	// Then don't return anything so that the caller knows not to write undo
	// info
	// BUT: If we're applying an attribute in the knowledge that it will later
	// get re-applied many times then we should really apply it, ignoring this
	// optimisation
	if (NewFill != NULL && *NewFill == *this && bOptimise)
	{
		// If the mutated fill is the same, then forget about it.
		delete NewFill;
		return NULL;
	}

#endif
	// Return the Mutated Fill
	return NewFill;
}

/********************************************************************************************

>	AttrFillGeometry* AttrFillGeometry::ChangeColour(AttrValueChange* Mutator)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		Mutator, The AttrFillGeometry Mutator that will change this fill in some way.
	Returns:	A pointer to a new AttrFillGeometry, that is a Mutated version of this Fill. 
	Purpose:	Changes a fill in some way.
	SeeAlso:	AttributeManager::AttributeSelected

********************************************************************************************/

AttrFillGeometry* AttrFillGeometry::ChangeColour(AttrValueChange* Mutator)
{
	AttrFillGeometry* NewFill = NULL;

#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill and change the copys' control point colours
	NewFill	= (AttrFillGeometry*)this->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	if (Mutator->MutateFill(NewFill))
		return NewFill;

	BOOL Changed = NewFill->ChangeControlColour( (AttrColourChange*)Mutator );

	// Did we change any colours ?
	if (!Changed)
	{
		// Only make it a flat fill if there aren't any other fill controls
		// selected anywhere else
		if (IsVisible() && SelectionCount > 0)
			return NewFill;

		// No Control points selected, so make a flat fill instead
		delete NewFill;
        NewFill = new AttrFlatColourFill;
		if (NewFill == NULL)
			return NULL;

		// and use the mutants colour
		*(FillGeometryAttribute*)(NewFill->GetAttributeValue()) = *(FillGeometryAttribute*)(Mutator->GetAttributeValue());

		if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && IsVisible())
			RenderFillBlobs();
	}
#endif

	// Return the Mutated Fill
	return NewFill;
}


/********************************************************************************************

>	AttrFillGeometry* AttrFillGeometry::ChangeTransp(AttrValueChange* Mutator)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Inputs:		Mutator, The AttrFillGeometry Mutator that will change this fill in some way.
	Returns:	A pointer to a new AttrFillGeometry, that is a Mutated version of this Fill. 
	Purpose:	Changes a fill in some way.
	SeeAlso:	AttributeManager::AttributeSelected

********************************************************************************************/

AttrFillGeometry* AttrFillGeometry::ChangeTransp(AttrValueChange* Mutator)
{
	AttrFillGeometry* NewFill = NULL;

#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill and change the copys' control point colours
	NewFill	= (AttrFillGeometry*)this->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	if (Mutator->MutateFill(NewFill))
		return NewFill;

	BOOL Changed = NewFill->ChangeControlTransp( (AttrTranspChange*)Mutator );

	// Did we change any colours ?
	if (!Changed)
	{
		// Only make it a flat fill if there aren't any other fill controls
		// selected anywhere else
		if (IsVisible() && SelectionCount > 0)
			return NewFill;

		// No Control points selected, so make a flat fill instead
		delete NewFill;
        NewFill = new AttrFlatTranspFill;
		if (NewFill == NULL)
			return NULL;

		// and use the mutants colour
		*(FillGeometryAttribute*)(NewFill->GetAttributeValue()) = *(FillGeometryAttribute*)(Mutator->GetAttributeValue());

		NewFill->SetTranspType(GetTranspType());

		if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && IsVisible())
			RenderFillBlobs();
	}
#endif

	// Return the Mutated Fill
	return NewFill;
}

/********************************************************************************************

>	BOOL AttrFillGeometry::ChangeControlColour( AttrColourChange* ColourAttr )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/94
	Inputs:		NewColour, the colour that selected control points should change to.
	Returns:	TRUE if a control point was selected, and the colour applied.
	Purpose:	Applies a colour to selected Fill Control Points.
	SeeAlso:	AttrFillGeometry::Mutate

********************************************************************************************/

BOOL AttrFillGeometry::ChangeControlColour( AttrColourChange* ColourAttr )
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FALSE;

	BOOL ColChanged = FALSE;

	DocColour NewColour = *ColourAttr->GetStartColour();

	// Is the Start Point Selected ?
	if (SelectionState[FILLCONTROL_STARTPOINT] /*&& GetStartColour() != NULL*/)
	{
		// Set the Start Colour
		SetStartColour(&NewColour);
		ColChanged = TRUE;
	}

	// Is the End Point Selected ?
	if (SelectionState[FILLCONTROL_ENDPOINT] /*&& GetEndColour() != NULL*/)
	{
		// Set the End Colour
		SetEndColour(&NewColour);
		ColChanged = TRUE;
	}

	// Is the End Point Selected ?
	if (SelectionState[FILLCONTROL_ENDPOINT2] /*&& GetEndColour2() != NULL*/)
	{
		// Set the End Colour
		SetEndColour2(&NewColour);
		ColChanged = TRUE;
	}

	// Is End Point 3 Selected ?
	if (SelectionState[FILLCONTROL_ENDPOINT3] /*&& GetEndColour3() != NULL*/)
	{
		// Set the Colour
		SetEndColour3(&NewColour);
		ColChanged = TRUE;
	}

	// set any selected colours in the colour ramp?
	ColourRamp *pRamp = GetColourRamp();
	if (pRamp)
	{
		// we have a ramp here so set its colours
		if (pRamp->SetSelectedColours(&NewColour)>0)
			ColChanged=TRUE;
	}

	// Tell the caller whether we changed anything or not
	return (ColChanged);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	BOOL AttrFillGeometry::ChangeControlTransp( AttrTranspChange* TranspAttr )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/9/94
	Inputs:		NewColour, the colour that selected control points should change to.
	Returns:	TRUE if a control point was selected, and the colour applied.
	Purpose:	Applies a colour to selected Fill Control Points.
	SeeAlso:	AttrFillGeometry::Mutate

********************************************************************************************/

BOOL AttrFillGeometry::ChangeControlTransp( AttrTranspChange* TranspAttr )
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FALSE;

	if (GetStartTransp() == NULL)
		return FALSE;

	BOOL TranspChanged = FALSE;

	// Get the required new transparency
	UINT32 NewTransp = *TranspAttr->GetStartTransp();

	// Is the Start Point Selected ?
	if (SelectionState[FILLCONTROL_STARTPOINT])
	{
		// Set the Start Transp
		SetStartTransp(&NewTransp);
		TranspChanged = TRUE;
	}

	// Is the End Point Selected ?
	if (SelectionState[FILLCONTROL_ENDPOINT])
	{
		// Set the End Transp
		SetEndTransp(&NewTransp);
		TranspChanged = TRUE;
	}

	// Is the End Point Selected ?
	if (SelectionState[FILLCONTROL_ENDPOINT2])
	{
		// Set the End Transp
		SetEndTransp2(&NewTransp);
		TranspChanged = TRUE;
	}

	// Is the End Point Selected ?
	if (SelectionState[FILLCONTROL_ENDPOINT3])
	{
		// Set the End Transp
		SetEndTransp3(&NewTransp);
		TranspChanged = TRUE;
	}

	// set any selected colours in the colour ramp?
	TransparencyRamp *pRamp = GetTranspRamp();
	if (pRamp)
	{
		// we have a ramp here so set its colours
		if (pRamp->SetSelectedTransp(NewTransp)>0)
			TranspChanged=TRUE;
	}

	// Tell the caller whether we changed anything or not
	return TranspChanged;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	virtual BOOL AttrFillGeometry::EditColourFillRamp( AttrColFillRampChange *pRampAttr )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/97
	Inputs:		pRampAttr = a pointer to the ramp change attribute
	Returns:	TRUE if the change has been made, FALSE if not
	Purpose:	Applies a ramp change to this fill. See the class AttrColFillRampChange for
				all the documentation on this

********************************************************************************************/

BOOL AttrFillGeometry::EditColourFillRamp( AttrColFillRampChange *pRampAttr )
{
#if !defined(EXCLUDE_FROM_RALPH)
	// ok we've been called to alter the fill ramp in this attribute
	// there are three things we can do
	if (pRampAttr==NULL)
		return FALSE;
		
	BOOL Changed = FALSE;
	ColourRamp *pCRamp;

	switch (pRampAttr->GetAction())
	{
		case ACT_DONOTHING:
			// well lets do nothing then. Aw go on we've gotta do something!
			ERROR3("Someone's forgotten to set the edit action of the RampChange attr");
			break;

		case ACT_CREATENEWCOL:
		{
			pCRamp = GetColourRamp();
			if (!pCRamp)
			{
				FillGeometryAttribute *pAttrValue = ATTRVALUE();
				if (pAttrValue->SupportsFillRamps())
				{
					if (pAttrValue->IS_KIND_OF(GradFillAttribute))
					{
						pCRamp = ((GradFillAttribute*)pAttrValue)->MakeNewColourRamp();
					}
				}
			}
			
			if (pCRamp)
			{
				ColRampItem *pItem;
				pItem = pCRamp->AddEntry(pRampAttr->GetPosition(), pRampAttr->GetStartColour());
				Changed=(pItem!=NULL);
			}
		}
		break;

		case ACT_EDITCOLOFITEM:
			pCRamp = GetColourRamp();
			if (pCRamp)
			{
				Changed = pCRamp->SetItemColour(pRampAttr->GetIndex(), pRampAttr->GetStartColour());
			}
			break;

		case ACT_EDITCOLOFSELECTED:
			pCRamp = GetColourRamp();
			if (pCRamp)
			{
				Changed = (pCRamp->SetSelectedColours(pRampAttr->GetStartColour()))>0;
				Changed=TRUE;
			}
			break;

		default:
			ERROR3("Unknown action found during AttrFillGeometry::EditColFillRamp");
			break;
	}

	return Changed;
#else
	return FALSE;
#endif
}



/********************************************************************************************

>	BOOL AttrFillGeometry::EditTranspFillRamp( AttrTranspFillRampChange *pRampAttr )

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/3/97
	Inputs:		pRampAttr = a pointer to the ramp change attribute
	Returns:	TRUE if the change has been made, FALSE if not
	Purpose:	Applies a ramp change to this fill. See the class AttrTranspFillRampChange for
				all the documentation on this

********************************************************************************************/

BOOL AttrFillGeometry::EditTranspFillRamp( AttrTranspFillRampChange *pRampAttr )
{
#if !defined(EXCLUDE_FROM_RALPH)
	// ok we've been called to alter the fill ramp in this attribute
	// there are three things we can do
	if (pRampAttr==NULL)
		return FALSE;
		
	BOOL Changed = FALSE;
	TransparencyRamp *pTRamp;

	UINT32 t, *pt = pRampAttr->GetStartTransp();
	t = ((pt!=NULL) ? (*pt) : 128);
	
	switch (pRampAttr->GetAction())
	{
		case ACT_DONOTHING:
			// well lets do nothing then. Aw go on we've gotta do something!
			ERROR3("Someone's forgotten to set the edit action of the RampChange attr");
			break;

		case ACT_CREATENEWTRANSP:
		{
			pTRamp = GetTranspRamp();
			if (!pTRamp)
			{
				FillGeometryAttribute *pAttrValue = ATTRVALUE();
				if (pAttrValue->SupportsFillRamps())
				{
					if (pAttrValue->IS_KIND_OF(GradTranspFillAttribute))
					{
						pTRamp = ((GradTranspFillAttribute*)pAttrValue)->MakeNewTranspRamp();
					}
				}
			}
			if (pTRamp)
			{
				TranspRampItem *pItem;
				pItem = pTRamp->AddEntry(pRampAttr->GetPosition(), t);
				Changed=(pItem!=NULL);
			}
		}
		break;

		case ACT_EDITTRANSPOFITEM:
			pTRamp = GetTranspRamp();
			if (pTRamp)
			{
				Changed = pTRamp->SetItemTransp(pRampAttr->GetIndex(), t);
			}
			break;

		case ACT_EDITTRANSPOFSELECTED:
			pTRamp = GetTranspRamp();
			if (pTRamp)
			{
				Changed = (pTRamp->SetSelectedTransp(t))>0;
			}
			break;
		
		default:
			ERROR3("Unknown action found during AttrFillGeometry::EditTranspFillRamp");
			break;
	}

	return Changed;
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	DocColour *AttrFillGeometry::EnumerateColourFields(UINT32 Context)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>    (fixed by Chris Snook to make this thing do what it aught to)
	Created:	23/8/94	(CGS:  27/4/2001)
	Purpose:	See base class.
	SeeAlso:	NodeAttribute::EnumerateColourFields

********************************************************************************************/

DocColour *AttrFillGeometry::EnumerateColourFields(UINT32 Context)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Work out which colour to return.

	DocColour * pColour = NULL;
			
	switch (Context)
	{
		case 0:
			pColour = GetStartColour();
		break;

		case 1:
			pColour = GetEndColour();
		break;

		case 2:
			pColour = GetEndColour2();
		break;

		case 3:
			pColour = GetEndColour3();
		break;
	}

	if (pColour)
	{
		// was a valid index - just return the pointer to the colour
		return (pColour);
	}
	else
	{
		// it was out of range, but we still need to check the fillramp ....

		ColourRamp *pRamp = GetColourRamp();

		INT32 adjuster = 0;		// we need to adjust the offset into the fillramp

		// the following four lines of code always get the correct (zero-based) index ....
		if (GetStartColour()) { adjuster++; }
		if (GetEndColour())	  { adjuster++; }
		if (GetEndColour2())  { adjuster++; }
		if (GetEndColour3())  { adjuster++; }

		if (pRamp)
		{
			// nab it from the ramp ....
			return (pRamp->EnumerateColourFields(Context-adjuster));
		}
		// um, drop through and return NULL
	}

#endif
	// No more colours.
	return NULL;
}




void AttrFillGeometry::MakePerspective(UndoableOperation* pUndoOp) 
{ 
	((FillGeometryAttribute*)GetAttributeValue())->MakePerspective(); 
}

void AttrFillGeometry::RemovePerspective(UndoableOperation* pUndoOp) 
{ 
	if (IsPerspective() && pUndoOp != NULL)
	{
		Node* pParent = FindParent();
		if (pParent == NULL)
			return;

		AttrFillGeometry* AttribClone;

      	ALLOC_WITH_FAIL(AttribClone ,(AttrFillGeometry*)this->SimpleCopy(), pUndoOp)
		if (AttribClone == NULL)
			return;
		
		AttribClone->RemovePerspective();

		HideNodeAction* UndoHideNodeAction; 
		NodeHidden* pNodeHidden; 
		if(!pUndoOp->DoHideNode(this, FALSE, &pNodeHidden,FALSE))
			return;                

		AttribClone->AttachNode(pParent, FIRSTCHILD); 

	   	// Create an action to hide the attribute when we undo 
		if ( HideNodeAction::Init(pUndoOp, 
			 					  pUndoOp->GetUndoActionList(),
								  AttribClone, 
								  TRUE,				// When the attribute gets hidden we
								  					// must include its size 
			 					 (Action**)(&UndoHideNodeAction))
			 == AC_FAIL)  
		{   
			AttribClone->CascadeDelete();       
			delete (AttribClone); 
			return;  
		};
	}
	else
	{
		((FillGeometryAttribute*)GetAttributeValue())->RemovePerspective(); 
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrValueChange
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrValueChange::AttrValueChange()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Purpose:	Constructor for an attribute value change.

********************************************************************************************/

AttrValueChange::AttrValueChange()
{
	Colour = TRUE;
	Transp = !Colour;
}

/********************************************************************************************

>	CCRuntimeClass* AttrValueChange::GetAttributeType()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Returns:	A CC_RUNTIME_CLASS depending on whether this should change colour
				or transparency attribute.
	Purpose:	Determines the scope of the value change.

********************************************************************************************/

CCRuntimeClass* AttrValueChange::GetAttributeType()
{ 
	if (Colour)
		return CC_RUNTIME_CLASS(AttrFillGeometry); 
	else
		return CC_RUNTIME_CLASS(AttrTranspFillGeometry); 
}

/********************************************************************************************

>	void AttrValueChange::MutateColourFills(BOOL Change) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Purpose:	Changes the effect of this value change on colour fills.

********************************************************************************************/

void AttrValueChange::MutateColourFills(BOOL Change) 
{ 
	Colour = Change;
	Transp = !Colour;
}

/********************************************************************************************

>	void AttrValueChange::MutateColourFills(BOOL Change) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Purpose:	Changes the effect of this value change on Transparency fills.

********************************************************************************************/

void AttrValueChange::MutateTranspFills(BOOL Change)
{ 
	Transp = Change;
	Colour = !Transp;
}

/********************************************************************************************

>	AttrTranspChange::AttrTranspChange()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Purpose:	Constructor for an attribute value change.

********************************************************************************************/

AttrTranspChange::AttrTranspChange()
{
	Transp = TRUE;
	Colour = !Transp;
}

/********************************************************************************************

>	virtual NodeAttribute* AttrTranspChange::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrTranspChange::GetOtherAttrToApply(BOOL* IsMutate)
{
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// A transparency change also needs to set the Stroke Transparency

	if (AttrFillGeometry::FillSelectionCount() > 0)
		return NULL; 	// Only set line transparency if no control
						// points are selected

	NodeAttribute* OtherAttr = new AttrStrokeTranspChange;
	if (OtherAttr == NULL)
		return NULL;

	UINT32 Transp = *GetStartTransp();

	((AttrStrokeTranspChange *)OtherAttr)->SetStartTransp(&Transp);

	*IsMutate = TRUE;

	return OtherAttr;
}

/********************************************************************************************

>	AttrFillGeometry* AttrColourChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the colour of a fill.

********************************************************************************************/

AttrFillGeometry* AttrColourChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	if (FillToMutate->IsVisible() && 
		FillToMutate->GetSelectionCount() == 0 &&
		SelectionCount > 0)
		return NULL;

	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	BOOL Changed = NewFill->ChangeControlColour(this);

	// Did we change any colours ?
	if (!Changed)
	{
		if (NewFill->IsABitmapFill() &&
			NewFill->GetSelectionCount() > 0)
		{
			if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && IsVisible())
			{
				// We must have failed when trying to set a contone colour,
				// so we'll do nothing

				delete NewFill;
				return NULL;
			}
		}

		// No Control points selected, so make a flat fill instead
		delete NewFill;

        NewFill = new AttrFlatColourFill;
		if (NewFill == NULL)
			return NULL;

		// and use the mutants colour
		*(FillGeometryAttribute*)(NewFill->GetAttributeValue()) = *(FillGeometryAttribute*)(this->GetAttributeValue());

		if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && 
			FillToMutate->IsVisible() &&
			FillToMutate->FindParent() &&
			FillToMutate->FindParent()->IsSelected())
		{
			FillToMutate->RenderFillBlobs();
		}
	}
	// Return the Mutated Fill
	return NewFill;
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrTranspChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the colour of a fill.

********************************************************************************************/

AttrFillGeometry* AttrTranspChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	if (FillToMutate->IsVisible() && 
		FillToMutate->GetSelectionCount() == 0 &&
		SelectionCount > 0)
		return NULL;

	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	BOOL Changed = NewFill->ChangeControlTransp(this);

	// Did we change any colours ?
	if (!Changed)
	{
		// No Control points selected, so make a flat fill instead
		delete NewFill;

        NewFill = new AttrFlatTranspFill;
		if (NewFill == NULL)
			return NULL;

		// and use the mutants colour
		*(FillGeometryAttribute*)(NewFill->GetAttributeValue()) = *(FillGeometryAttribute*)(this->GetAttributeValue());

		NewFill->SetTranspType(FillToMutate->GetTranspType());

		if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && 
			FillToMutate->IsVisible() &&
			FillToMutate->FindParent() &&
			FillToMutate->FindParent()->IsSelected())
		{
			FillToMutate->RenderFillBlobs();
		}
	}

	// Return the Mutated Fill
	return NewFill;
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrBitmapChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the bitmap for a fill.

********************************************************************************************/

AttrFillGeometry* AttrBitmapChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

/*
	INT32 OldBPP;
	
	if (NewFill->GetBitmap())
		OldBPP = NewFill->GetBitmap()->GetBPP();
	else
		OldBPP = 24;

	INT32 NewBPP = this->GetBitmap()->GetBPP();

	if (OldBPP <= 8 && NewBPP > 8)
	{
		// Remove the end point blobs if necessary
		FillToMutate->DrawEndBlobs();
	}
*/

	NewFill->CopyBitmap(GetBitmap());

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrBitmapTessChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the bitmap tesselation of a fill.

********************************************************************************************/

AttrFillGeometry* AttrBitmapTessChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetTesselation(GetTesselation());
	NewFill->RecalcFractal();

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrBitmapDpiChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the bitmap dpi of a fill.

********************************************************************************************/

AttrFillGeometry* AttrBitmapDpiChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetDPI(Dpi);

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrFractalGrainChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the fractal graininess of a fill.

********************************************************************************************/

AttrFillGeometry* AttrFractalGrainChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetGraininess(GetGraininess());
	NewFill->RecalcFractal();

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrFractalTileableChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the Tileable-ness of a fractal fill.

********************************************************************************************/

AttrFillGeometry* AttrFractalTileableChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetTileable(GetTileable());
	NewFill->RecalcFractal();

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrTranspTypeChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the transparency type of a fill.

********************************************************************************************/

AttrFillGeometry* AttrTranspTypeChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	NewFill->SetTranspType(GetTranspType());

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrNoiseScaleChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the noise scale of a fill.

********************************************************************************************/

AttrFillGeometry* AttrNoiseScaleChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	// get the value from us.
	NewFill->SetGraininess(GetGraininess());
	NewFill->RecalcFractal();

	return NewFill; 
#else
	return NULL;
#endif
}

/********************************************************************************************

>	virtual NodeAttribute* AttrTranspTypeChange::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrTranspTypeChange::GetOtherAttrToApply(BOOL* IsMutate)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// A transparency type change also needs to set the Stroke Transparency	type

	NodeAttribute* OtherAttr = new AttrStrokeTranspTypeChange;
	if (OtherAttr == NULL)
		return NULL;

	UINT32 TranspType = GetTranspType();

	((AttrStrokeTranspTypeChange *)OtherAttr)->SetTranspType(TranspType);

	*IsMutate = TRUE;

	return OtherAttr;
#else
	return NULL;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrValueChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrValueChange::GetAttrNameID(void)  
{
	return (_R(IDS_VALUE_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrColourChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrColourChange::GetAttrNameID(void)  
{
	return (_R(IDS_COLOUR_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrBitmapChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrBitmapChange::GetAttrNameID(void)  
{
	return (_R(IDS_BITMAP_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrBitmapTessChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrBitmapTessChange::GetAttrNameID(void)  
{
	return (_R(IDS_BITMAPTESS_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrBitmapDpiChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrBitmapDpiChange::GetAttrNameID(void)  
{
	return (_R(IDS_BITMAPDPI_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrFractalChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFractalChange::GetAttrNameID(void)  
{
	return (_R(IDS_FRACTAL_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrFractalGrainChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFractalGrainChange::GetAttrNameID(void)  
{
	return (_R(IDS_FRACTALGRAIN_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrFractalTileableChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFractalTileableChange::GetAttrNameID(void)  
{
	return (_R(IDS_FRACTAL_CHANGE));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrTranspChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTranspChange::GetAttrNameID(void)  
{
	return (_R(IDS_TRANSP_CHANGE));		//_R(IDS_FLATTRANSPFILL));
}                                  

/********************************************************************************************

>	virtual UINT32 AttrTranspTypeChange::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTranspTypeChange::GetAttrNameID(void)  
{
	return (_R(IDS_TRANSPTYPE_CHANGE));
}

/********************************************************************************************

>	virtual UINT32 AttrNoiseScaleChange::GetAttrNameID(void)  

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/01/97
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrNoiseScaleChange::GetAttrNameID(void)  
{
	return (_R(IDS_NOISESCALE_CHANGE));
}                                  




/********************************************************************************************

>	AttrColFillRampChange::AttrColFillRampChange()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	A class to edit colour ramps

********************************************************************************************/

AttrColFillRampChange::AttrColFillRampChange()
{
	action   = ACT_DONOTHING;
	index    = FILLRAMP_ILLEGALINDEX;
	position = 0.0f;
}

/********************************************************************************************

>	void InitForColourAdd(DocColour* pColour, float pos);
	void InitForTranspAdd(UINT32 t, float pos);
	void InitForColourEdit(UINT32 i, DocColour* pColour);
	void InitForTranspEdit(UINT32 i, UINT32 t);
	void InitForSelEdit(DocColour* pColour);
	void InitForSelEdit(UINT32 transparency);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	All the init functions to set this AttrColFillRampChange object

********************************************************************************************/

void AttrColFillRampChange::InitForColourAdd(DocColour *pColour, float pos)
{
	action   = ACT_CREATENEWCOL;
	position = pos;
	Value.SetStartColour(pColour);
}

void AttrColFillRampChange::InitForColourEdit(UINT32 i, DocColour *pColour)
{
	action = ACT_EDITCOLOFITEM;
	index  = i;
	Value.SetStartColour(pColour);
}

void AttrColFillRampChange::InitForSelEdit(DocColour *pColour)
{
	action = ACT_EDITCOLOFSELECTED;
	index  = FILLRAMP_ILLEGALINDEX;
	Value.SetStartColour(pColour);
}


/********************************************************************************************

>	UINT32 AttrColFillRampChange::GetAttrNameID()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrColFillRampChange::GetAttrNameID()
{
	return (_R(IDS_FILLRAMP_COLOURCHANGE));
}


/********************************************************************************************

>	AttrFillGeometry* AttrColFillRampChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Inputs:		FillToMutate = a pointer to a fill attribute to alter.
	Returns:	A pointer to a copy which contains the alteration.
	Purpose:	Changes the colours in a fill ramp. FillToMutate contains all the info
				necessary to do the edit.

********************************************************************************************/

AttrFillGeometry* AttrColFillRampChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	if (FillToMutate->IsVisible() && 
		FillToMutate->GetSelectionCount() == 0 &&
		SelectionCount > 0)
		return NULL;

	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	// do the actual editing here
	BOOL Changed = NewFill->EditColourFillRamp(this);

	// Did we change any colours ?
	if (!Changed)
	{
		delete NewFill;
		return NULL;
	}
	// Return the Mutated Fill
	return NewFill;
#else
	return NULL;
#endif
}

/********************************************************************************************

>	AttrTranspFillRampChange::AttrTranspFillRampChange()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	A class to edit colour ramps

********************************************************************************************/

AttrTranspFillRampChange::AttrTranspFillRampChange()
{
	action   = ACT_DONOTHING;
	index    = FILLRAMP_ILLEGALINDEX;
	position = 0.0f;
}

/********************************************************************************************

>	void InitForTranspAdd(UINT32 t, float pos);
	void InitForTranspEdit(UINT32 i, UINT32 t);
	void InitForSelEdit(UINT32 transparency);

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Purpose:	All the init functions to set this AttrTranspFillRampChange object

********************************************************************************************/

void AttrTranspFillRampChange::InitForTranspAdd(UINT32 t, float pos)
{
	action   = ACT_CREATENEWTRANSP;
	position = pos;
	Value.SetStartTransp(&t);
}

void AttrTranspFillRampChange::InitForTranspEdit(UINT32 i, UINT32 t)
{
	action = ACT_EDITTRANSPOFITEM;
	index  = i;
	Value.SetStartTransp(&t);
}

void AttrTranspFillRampChange::InitForSelEdit(UINT32 t)
{
	action = ACT_EDITTRANSPOFSELECTED;
	index  = FILLRAMP_ILLEGALINDEX;
	Value.SetStartTransp(&t);
}

/********************************************************************************************

>	UINT32 AttrTranspFillRampChange::GetAttrNameID()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTranspFillRampChange::GetAttrNameID()
{
	return (_R(IDS_FILLRAMP_TRANSPCHANGE));
}


/********************************************************************************************

>	AttrFillGeometry* AttrTranspFillRampChange::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/03/97
	Inputs:		FillToMutate = a pointer to a fill attribute to alter.
	Returns:	A pointer to a copy which contains the alteration.
	Purpose:	Changes the colours in a fill ramp. FillToMutate contains all the info
				necessary to do the edit.

********************************************************************************************/

AttrFillGeometry* AttrTranspFillRampChange::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	if (FillToMutate->IsVisible() && 
		FillToMutate->GetSelectionCount() == 0 &&
		SelectionCount > 0)
		return NULL;

	// Make a copy of this Fill to change
	AttrFillGeometry* NewFill = (AttrFillGeometry*)FillToMutate->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	// do the actual editing here
	BOOL Changed = NewFill->EditTranspFillRamp(this);

	// Did we change any colours ?
	if (!Changed)
	{
		delete NewFill;
		return NULL;
	}

	// Return the Mutated Fill
	return NewFill;
#else
	return NULL;
#endif
}


/********************************************************************************************

>	AttrColourDrop::AttrColourDrop(DocCoord &DropPoint, DocRect &ObjectBounds, 
									DocColour NewColour)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Purpose:	Constructor	for Colour Drop Attribute

********************************************************************************************/

AttrColourDrop::AttrColourDrop(DocCoord &DropPoint, DocRect &ObjectBounds, 
								DocColour NewColour)
{
#if !defined(EXCLUDE_FROM_RALPH)
	SetDropPoint(DropPoint);
	SetObjectBounds(ObjectBounds);
	SetStartColour(&NewColour);

	ObjectHit = NULL;
	FillHit = NULL;
	ControlHit = FILLCONTROL_NULL;

	ObjectIsSelected = FALSE;
#endif
}

/********************************************************************************************

>	void AttrColourDrop::SetDropPoint(DocCoord &DropPoint)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Purpose:	Set the drop point for this colour drop

********************************************************************************************/

void AttrColourDrop::SetDropPoint(DocCoord &DropPoint)
{
	DropPos = DropPoint;
}

/********************************************************************************************

>	void AttrColourDrop::SetObjectBounds(DocRect &ObjectBounds)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Purpose:	Set the object bounds for this colour drop

********************************************************************************************/

void AttrColourDrop::SetObjectBounds(DocRect &ObjectBounds)
{
	Bounds = ObjectBounds;
}

/********************************************************************************************

>	void AttrColourDrop::SetObjectDroppedOn(NodeRenderableInk* pObject)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Purpose:	Set the object that the colour was dropped on.

********************************************************************************************/

void AttrColourDrop::SetObjectDroppedOn(NodeRenderableInk* pObject)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (pObject)
	{
		ObjectHit = pObject;
		ObjectIsSelected = pObject->IsSelected();
	}
#endif
}

/********************************************************************************************

>	NodeRenderableInk* AttrColourDrop::GetObjectDroppedOn()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Purpose:	Get the object that the colour was dropped on.

********************************************************************************************/

NodeRenderableInk* AttrColourDrop::GetObjectDroppedOn()
{
	return ObjectHit;
}

/********************************************************************************************

>	BOOL AttrColourDrop::IsObjectSelected()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/95
	Purpose:	Get the selection state of the object that the colour was dropped on.

********************************************************************************************/

BOOL AttrColourDrop::IsObjectSelected()
{
	return (ObjectHit && ObjectIsSelected);
}

/********************************************************************************************

>	virtual UINT32 AttrColourDrop::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/01/95
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrColourDrop::GetAttrNameID(void)  
{
	return (_R(IDS_COLOUR_CHANGE));
}                                  

/********************************************************************************************

>	AttrFillGeometry* AttrColourDrop::MutateFill(AttrFillGeometry* FillToMutate) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the colour of a fill.

********************************************************************************************/

AttrFillGeometry* AttrColourDrop::MutateFill(AttrFillGeometry* FillToMutate) 
{ 
	return FillToMutate->DoColourDrop(this);	
}

/********************************************************************************************

>	AttrFillGeometry* AttrColourDrop::EffectsParentBounds() 

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/2000
	Inputs:		-
	Purpose:	When creating a new multi-stage fill blob that is outside the shapes
				geometry, we need to invalidate not only the shape; but its bounds as well;
				otherwise we encounter redraw problems.

********************************************************************************************/

BOOL AttrColourDrop::EffectsParentBounds ()
{
	return (TRUE);
}

/********************************************************************************************

>	FillControl AttrFillGeometry::DoColourDropTest(AttrColourDrop* ColDrop, 
													NodeRenderableInk** Object, 
													AttrFillGeometry** Fill)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrFillGeometry::DoColourDropTest(AttrColourDrop* ColDrop, 
												NodeRenderableInk** Object, 
												AttrFillGeometry** FillAttr)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR2IF(ColDrop == NULL, FILLCONTROL_NULL, "Null ColourDrop passed to DoColourDropTest");

	DocCoord DropPoint = ColDrop->GetDropPoint();

	FillControl ControlHit = FILLCONTROL_NULL;

	if ( (((Camelot.GetBlobManager())->GetCurrentInterest()).Fill) )
	{
		BOOL HitAControl = FALSE;
		HitList.DeleteAll();

		AttrFillGeometry* pAttrFound = NULL;

		// Find the first attribute to test
		AttrFillGeometry* pAttr = FindFirstSelectedAttr();
		while (pAttr != NULL)
		{
			// We're only interested in colour fills
			if (pAttr->GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry))
			{
				// See if the current pos is over one of this fills controls
				FillControl Hit = pAttr->CheckForControlHit(DropPoint);

				if (Hit != FILLCONTROL_NULL)
				{
					// Signal that we hit something
					HitAControl = TRUE;

					if (pAttrFound == NULL)
					{
						pAttrFound = pAttr;
						ControlHit = Hit;
					}

					BOOL InList = FALSE;

					// We may have already hit this attribute if the selection is
					// inside a parent and have common attributes, so we need to
					// check and make sure this attribute is NOT in the list
					// already.

					if (!HitList.IsEmpty())
					{
						ListItem* pItem = HitList.GetHead();

						while (pItem)
						{
							NodeAttributePtrItem* NodePtr = (NodeAttributePtrItem*)pItem;

							if (NodePtr->NodeAttribPtr == pAttr)
							{
								// Ignore this one, we've hit it already
							 	InList = TRUE;
								break;
							}

							pItem = HitList.GetNext(pItem);
						}
					}

					if (!InList)
					{
						NodeAttributePtrItem* NodePtr = new NodeAttributePtrItem;

						if (NodePtr != NULL)
						{
							// Add the Attr to the Hit list
							NodePtr->NodeAttribPtr = pAttr;
							HitList.AddTail(NodePtr);
						}
					}
				}
			}
			
			// Move onto the next attr
			pAttr = FindNextSelectedAttr();
		}                              

		if (HitAControl)
		{
			// We've hit something !!
			if (FillAttr != NULL)
				*FillAttr = pAttrFound;
			
			if (Object != NULL)
				*Object = (NodeRenderableInk*)pAttrFound->FindParent();

			return ControlHit;
		}
	}

	if (*Object == NULL)
	{
		return FILLCONTROL_NULL;
	}

	// An object has been specified 
	Node* pParent = (Node*)*Object;
	ColDrop->SetObjectDroppedOn(*Object);

	DocRect ParentBounds = ((NodeRenderableBounded*)*Object)->GetBoundingRect();

	AttribMap.RemoveAll();
	BOOL FoundAttrs = ((NodeRenderableInk*)pParent)->FindAppliedAttributes(&AttribMap);
	if (FoundAttrs)
	{
		void		   *pAttr;
		if( AttribMap.Lookup( CC_RUNTIME_CLASS(AttrFillGeometry), pAttr ) )
		{
			// Wahay !! We've got one !!
			ControlHit = ((AttrFillGeometry*)pAttr)->TestColourDrop(ColDrop);
			return ControlHit;
		}
	}
#endif
	return FILLCONTROL_NULL;
}

/********************************************************************************************

>	AttrFillGeometry* AttrFillGeometry::DoColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the colour of a fill.

********************************************************************************************/

AttrFillGeometry* AttrFillGeometry::DoColourDrop(AttrColourDrop* ColDrop) 
{ 
	AttrFillGeometry* NewFill;
	DocColour DroppedColour = *ColDrop->GetStartColour();

	// First test to see which colour we should change
	FillControl ControlHit = TestColourDrop(ColDrop);

	if (ControlHit == FILLCONTROL_NULL)
	{
		// No Control points hit, so make a flat fill instead
        NewFill = new AttrFlatColourFill;
		if (NewFill == NULL)
			return NULL;

		// and use the mutants colour
		NewFill->SetStartColour(&DroppedColour);

		BOOL InSelection = ColDrop->IsObjectSelected();

		if (InSelection &&
			GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && 
			IsVisible()
			)
		{
			BOOL UniqueFill = TRUE;

			AttrFillGeometry* pAttr = FindFirstSelectedAttr();
			while (pAttr != NULL)
			{
 				if (pAttr != this && GetAttributeType() == pAttr->GetAttributeType())
				{
					if ( IsMeshSame((FillGeometryAttribute*)GetAttributeValue(),
									(FillGeometryAttribute*)pAttr->GetAttributeValue()) )
					{
						UniqueFill = FALSE;
						break;
					}
				}

				// Move onto the next attr
				pAttr = FindNextSelectedAttr();
			}

			/*
			if (UniqueFill)
				RenderFillBlobs();
				*/
		}
		
		return NewFill;
	}

	if (FindParent() && FindParent()->IsCompound())
	{
		if (ColDrop->GetObjectDroppedOn() && 
			!(ColDrop->GetObjectDroppedOn()->IsCompound()) && 
			ColDrop->IsObjectSelected())
		{
			// If we are dropping inside a group, then we'll need to remove
			// the blobs if this fill is applied to the whole group
			// RenderFillBlobs();
		}
	}

	// Make a copy of this Fill to change
	NewFill = (AttrFillGeometry*)this->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	if (!NewFill->ChangeControlColour(DroppedColour, ControlHit, ColDrop))
	{
		delete NewFill;
		return NULL;
	}

	// Return the Mutated Fill
	return NewFill;
}

/********************************************************************************************

>	BOOL AttrFillGeometry::ChangeControlColour(DocColour& Col, FillControl Cntrl,
						AttrColourDrop * pColDrop)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Applies a colour to a specific Fill Control Point.

********************************************************************************************/

BOOL AttrFillGeometry::ChangeControlColour(DocColour& Col, FillControl Cntrl,
										   AttrColourDrop * pColDrop)
{
// DMc for drag n drops onto fill ramp control points
	if (ISA_RAMPINDEX(Cntrl))
	{
		// we have a drag and drop onto a ramp control point
		if (GetColourRamp())
		{
			ColourRamp * pRamp = GetColourRamp();

			return pRamp->SetItemColour(Cntrl, &Col);
		}

		// treat as a normal ramp point
		Cntrl = FILLCONTROL_RAMPPOINT;
	}

	switch (Cntrl)
	{
		case FILLCONTROL_STARTPOINT:
			SetStartColour(&Col);
			break;

		case FILLCONTROL_ENDPOINT:
			SetEndColour(&Col);
			break;

		case FILLCONTROL_SECONDARYPOINT:
			SetEndColour(&Col);
			break;

		case FILLCONTROL_ENDPOINT2:
			SetEndColour2(&Col);
			break;

		case FILLCONTROL_ENDPOINT3:
			SetEndColour3(&Col);
			break;
		case FILLCONTROL_RAMPPOINT:
			if (pColDrop)
			{
				CProfileBiasGain DefaultBiasGain;

				if (!(GetProfile () == DefaultBiasGain))
				{
					// Load and build the question text.
					String_256 QueryString(_R(IDS_ASKMULTIREPLACEPROFILE));
					
					// The only way of bringing up a box with a string in it
					Error::SetError(0, QueryString, 0);
					INT32 DlgResult = InformMessage(0, _R(IDS_YES), _R(IDS_NO));
					Error::ClearError();

					switch (DlgResult)
					{
						case 1:		// YES
							SetProfile (DefaultBiasGain);		// we MUST revert to
																// the default profile
						break;
						case 2:		// NO
						return (FALSE);			// break out of this stuff!
					}
				}

				DocCoord DropPoint = pColDrop->GetDropPoint();

				double dRampPoint = FindRampPoint(DropPoint, *GetStartPoint(),
					*GetEndPoint());

				// make a new colour ramp (if necessary)
				if (GetColourRamp() == NULL)
				{
					FillGeometryAttribute *pAttrValue = ATTRVALUE();
					if (pAttrValue->SupportsFillRamps())
					{
						if (pAttrValue->IS_KIND_OF(GradFillAttribute))
						{
							ColourRamp * pRamp = ((GradFillAttribute*)pAttrValue)->MakeNewColourRamp();
							pRamp->AddEntry((float)dRampPoint, &Col);
							pRamp->Dump();
						}
					}
				}
				else
				{
					// find out if the drag hit a ramp point
					FillRamp * pRamp = GetColourRamp();

					ColRampItem * pItem = (ColRampItem *)pRamp->GetHead();

					while (pItem)
					{
						if (pItem->GetPosition() == dRampPoint)
						{
							// set the colour for the entry
							pItem->SetColour(&Col);
							break;
						}

						pItem = (ColRampItem *)pRamp->GetNext(pItem);
					}

					// it hasn't hit a ramp point so add an entry
					if (!pItem)
					{
						GetColourRamp()->AddEntry((float)dRampPoint, &Col);
						GetColourRamp()->Dump();
					}
				}
			}
			break;		
		default:
			SetStartColour(&Col);
			break;
	}

	return TRUE;}

/********************************************************************************************

>	BOOL AttrBitmapColourFill::ChangeControlColour(DocColour& Col, FillControl Cntrl)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Applies a colour to a specific Fill Control Point.

********************************************************************************************/

BOOL AttrBitmapColourFill::ChangeControlColour(DocColour& Col, FillControl Cntrl,
										   AttrColourDrop * pColDrop)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Do contone stuff here ....
	KernelBitmap* pBitmap = GetBitmap();
	KernelBitmap* pGreyBmp = NodeBitmap::CheckGreyscaleBitmap(pBitmap, _R(IDS_MAKEGREY), _R(IDS_DOGREY));

	if (pGreyBmp == NULL)
		return FALSE;		// Failed in some way (or the user cancelled)

	return AttrFillGeometry::ChangeControlColour(Col, Cntrl); 
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual FillControl AttrFillGeometry::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrFillGeometry::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	return CheckForControlHit(DropPoint);
#else
	return FILLCONTROL_NULL;
#endif
}

/********************************************************************************************

>	virtual FillControl AttrFlatFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrFlatFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
	return FILLCONTROL_NULL;
}

/********************************************************************************************

>	virtual FillControl AttrLinearFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrLinearFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	FillControl ControlHit = CheckForControlHit(DropPoint);
	
	// If it hit one of our control points, then use that
	if (ControlHit != FILLCONTROL_NULL)
		return ControlHit;

	// It didn't hit any of our control points, so if the drop is over
	// the object then we'll make a guess as to which control point
	// the user would like to change, depending on which area of the
	// object the pointer is over.

	// First make sure we're actually over an object
	NodeRenderableInk* pParentObject = ColDrop->GetObjectDroppedOn();
	if (pParentObject == NULL)
		return FILLCONTROL_NULL;	// We're not over any object 

	// Make sure this fill type has some Control Points
	if (GetStartPoint() == NULL || GetEndPoint() == NULL)
		return FILLCONTROL_NULL;

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint 	= *GetEndPoint();
		
	// Find the MidPoint of the line joining the Start and End Points
	DocCoord MidPoint = DocCoord(StartPoint.x + (EndPoint.x - StartPoint.x)/2,
								StartPoint.y + (EndPoint.y - StartPoint.y)/2);

	TRACEUSER( "Mike", _T("DropPoint (before)	= %d,%d\n"),DropPoint.x, DropPoint.y);

	TRACEUSER( "Mike", _T("StartPoint 			= %d,%d\n"),StartPoint.x, StartPoint.y);
	TRACEUSER( "Mike", _T("MidPoint 			= %d,%d\n"),MidPoint.x, MidPoint.y);
	TRACEUSER( "Mike", _T("EndPoint 			= %d,%d\n"),EndPoint.x, EndPoint.y);

	// Now find the angle of the line relative to the Horizontal
	ANGLE LineAngle = CalcLineAngle(MidPoint, EndPoint) - 90;
	Matrix Rotate 	= Matrix(-LineAngle);

	// Rotate the Drop Point around the MidPoint of the line
	DropPoint.translate(-MidPoint.x, -MidPoint.y);
	Rotate.transform(&DropPoint);
	DropPoint.translate(MidPoint.x, MidPoint.y);

	TRACEUSER( "Mike", _T("DropPoint (after)	= %d,%d\n"),DropPoint.x, DropPoint.y);

	// Now the Coords have be transformed as if the Line was Horizontal,
	// so all we need to do is check to see if the Drop Point is to the
	// left or right of the MidPoint.

	if (DropPoint.x <= MidPoint.x)
		ControlHit = FILLCONTROL_STARTPOINT;		
	else
		ControlHit = FILLCONTROL_ENDPOINT;		

	return ControlHit;
#else
	return FILLCONTROL_NULL;
#endif
}

/********************************************************************************************

>	virtual FillControl AttrRadialFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrRadialFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	return AttrFillGeometry::TestColourDrop(ColDrop);

	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	FillControl ControlHit = CheckForControlHit(DropPoint);
	
	// If it hit one of our control points, then use that
	if (ControlHit != FILLCONTROL_NULL)
		return ControlHit;

	// It didn't hit any of our control points, so if the drop is over
	// the object then we'll make a guess as to which control point
	// the user would like to change, depending on which area of the
	// object the pointer is over.

	// First make sure we're actually over an object
	NodeRenderableInk* pParentObject = ColDrop->GetObjectDroppedOn();
	if (pParentObject == NULL)
		return FILLCONTROL_NULL;	// We're not over any object 

	// Make sure this fill type has some Control Points
	if (GetStartPoint() == NULL || GetEndPoint() == NULL)
		return FILLCONTROL_NULL;

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint 	= *GetEndPoint();
	DocCoord EndPoint2 	= *GetEndPoint2();

	double MajRadius = EndPoint.Distance(StartPoint);
	double MinRadius = EndPoint2.Distance(StartPoint);

	ANGLE MajAngle  = CalcLineAngle(StartPoint, EndPoint)  + 180;
	ANGLE DropAngle = CalcLineAngle(StartPoint, DropPoint) + 180;

	ANGLE Diff = DropAngle - MajAngle;

	if (Diff >= 180)	Diff -= 360;
	if (Diff <= -180)	Diff += 360;
	
	Diff = ABS(Diff);
	if (Diff > 90)		Diff = 180 - Diff;

	TRACEUSER( "Mike", _T("MajAngle		= %f\n"),MajAngle.MakeDouble());
	TRACEUSER( "Mike", _T("DropAngle	= %f\n"),DropAngle.MakeDouble());
	TRACEUSER( "Mike", _T("Diff			= %f\n"),Diff.MakeDouble());

	double Radius = MajRadius + ((MinRadius - MajRadius) * (Diff.MakeDouble()/90));
	double Dist   = DropPoint.Distance(StartPoint);

	TRACEUSER( "Mike", _T("Radius		= %f\n"),Radius);
	TRACEUSER( "Mike", _T("Dist			= %f\n"),Dist);
		
	// Is the distance from the centre, more than half the radius ?

	if (Dist <= Radius/2)
		ControlHit = FILLCONTROL_STARTPOINT;		
	else
		ControlHit = FILLCONTROL_ENDPOINT;		

	return ControlHit;
#else
	return FILLCONTROL_NULL;
#endif
}

/********************************************************************************************

>	virtual FillControl AttrConicalFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrConicalFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	FillControl ControlHit = CheckForControlHit(DropPoint);
	
	// If it hit one of our control points, then use that
	if (ControlHit != FILLCONTROL_NULL)
		return ControlHit;

	// It didn't hit any of our control points, so if the drop is over
	// the object then we'll make a guess as to which control point
	// the user would like to change, depending on which area of the
	// object the pointer is over.

	// First make sure we're actually over an object
	NodeRenderableInk* pParentObject = ColDrop->GetObjectDroppedOn();
	if (pParentObject == NULL)
		return FILLCONTROL_NULL;	// We're not over any object 

	// Make sure this fill type has some Control Points
	if (GetStartPoint() == NULL || GetEndPoint() == NULL)
		return FILLCONTROL_NULL;

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint 	= *GetEndPoint();

	// Get the angle of lines joining the Start and End points,
	// and also the Start and Drop points. (And make them 0 to 360).
	ANGLE LineAngle = CalcLineAngle(StartPoint, EndPoint)  + 180;
	ANGLE DropAngle = CalcLineAngle(StartPoint, DropPoint) + 180;

	// Get the difference between the two angles
	ANGLE Diff = DropAngle - LineAngle;

	// and ensure we always get use the shortest path
	if (Diff >= 180)
		Diff -= 360;
					
	if (Diff <= -180)
		Diff += 360;

	TRACEUSER( "Mike", _T("LineAngle	= %f\n"),LineAngle.MakeDouble());
	TRACEUSER( "Mike", _T("DropAngle	= %f\n"),DropAngle.MakeDouble());
	TRACEUSER( "Mike", _T("Diff			= %f\n"),Diff.MakeDouble());

	// check for a colour drop on the line
	double d = FindRampPoint(DropPoint, *GetStartPoint(), *GetEndPoint());

	if (d >= 0 && d <= 1.0)
	{
		return FILLCONTROL_RAMPPOINT;
	}

	// If the difference is greater the +-90, then set the start colour
	if (Diff <= 90 && Diff > -90)
		ControlHit = FILLCONTROL_ENDPOINT;		
	else
		ControlHit = FILLCONTROL_STARTPOINT;

	return ControlHit;
}



/********************************************************************************************

>	virtual FillControl AttrBitmapFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrBitmapFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
	return AttrFillGeometry::TestColourDrop(ColDrop);
}

/********************************************************************************************

>	KernelBitmap *AttrBitmapFill::EnumerateBitmaps(UINT32 Count)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/17/95
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

KernelBitmap *AttrBitmapFill::EnumerateBitmaps(UINT32 Count)
{
	if (IsAFractalFill())
		return NULL;

	if (Count == 0) 
		return GetBitmap();

	return NULL;
}

/********************************************************************************************

>	virtual NodeAttribute* AttrBitmapFill::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/12/99
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrBitmapFill::GetOtherAttrToApply(BOOL* IsMutate)
{
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// A bitmap fill change also needs to set the mapping to repeating ....

	NodeAttribute* OtherAttr = NULL;
	
	if (GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry))
	{
		OtherAttr = new AttrFillMappingLinear;
	}
	else
	{
		OtherAttr = new AttrTranspFillMappingLinear;
	}

	if (OtherAttr == NULL)
	{
		return (NULL);
	}

	((AttrFillMappingLinear*) OtherAttr)->SetRepeat(RT_Repeating);

	*IsMutate = FALSE;

	return OtherAttr;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFlatFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrFlatFill::AttrFlatFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/07/94
	Purpose:	Constructs a Flat Fill.

********************************************************************************************/

AttrFlatFill::AttrFlatFill() 
{
}

/********************************************************************************************

>	virtual DocRect AttrFlatFill::GetBlobBoundingRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the nodes blobs.This should always 
				be calculated on the fly as the view scale can change without the node 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrFlatFill::GetBlobBoundingRect()
{
	// Flat fills have bo bounds
	DocRect BoundingRect(0,0,0,0);

	// and return it
	return BoundingRect;
}

/********************************************************************************************

>	void AttrFlatFill::ValidateAttributeValue()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrFlatFill::ValidateAttributeValue()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFlatColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrFlatColourFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Flat Fill Colour attribute.

********************************************************************************************/

void AttrFlatColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrFlatColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFlatColourFill::SimpleCopy()
{
	AttrFlatColourFill* NodeCopy = new AttrFlatColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFlatColourFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFlatColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_FILL_COLOUR));
}                                  

/********************************************************************************************

>	void AttrFlatColourFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrFlatColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

//	TempStr._MakeMsg( TEXT("\r\nFill") );
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrFlatColourFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFlatColourFill::GetNodeSize() const 
{     
	return sizeof(AttrFlatColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrFlatColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the flat fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFlatColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the colour first
	INT32 ColRef = pFilter->WriteRecord(&Value.Colour);

	// Is the colour reference ok?
	BOOL ok = (ColRef != 0);
		
	if (ok)
	{
		BOOL ColourRequired = TRUE;
		INT32 Tag = TAG_FLATFILL;
		INT32 Size = TAG_FLATFILL_SIZE;
		switch (ColRef)
		{
			case REF_DEFAULTCOLOUR_TRANSPARENT:
				Tag = TAG_FLATFILL_NONE;
				Size = TAG_FLATFILL_NONE_SIZE;
				ColourRequired = FALSE;
				break;
			case REF_DEFAULTCOLOUR_BLACK:
				Tag = TAG_FLATFILL_BLACK;
				Size = TAG_FLATFILL_BLACK_SIZE;
				ColourRequired = FALSE;
				break;
			case REF_DEFAULTCOLOUR_WHITE:
				Tag = TAG_FLATFILL_WHITE;
				Size = TAG_FLATFILL_WHITE_SIZE;
				ColourRequired = FALSE;
				break;
		}

		CamelotFileRecord Rec(pFilter,Tag,Size);

		if (ok) ok = Rec.Init();
		if (ok && ColourRequired) ok = Rec.WriteReference(ColRef);
		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFlatColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFlatColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrFlatColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrFlatColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	INT32 ColRef = pFilter->WriteRecord(&Value.Colour);

	// Is the colour reference ok?
	
	return (ColRef != 0);
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFlatTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrFlatTranspFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Flat Fill Colour attribute.

********************************************************************************************/

void AttrFlatTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrFlatTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFlatTranspFill::SimpleCopy()
{
	AttrFlatTranspFill* NodeCopy = new AttrFlatTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFlatTranspFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFlatTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_FILL_COLOUR));
}                                  

/********************************************************************************************

>	void AttrFlatTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrFlatTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nFill") );
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nFill value = #1%u"), *GetStartTransp() );	
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nFill type = #1%u"), GetTranspType() );	
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrFlatTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFlatTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrFlatTranspFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrFlatTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the flat transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFlatTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Transparency level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_FLATTRANSPARENTFILL,TAG_FLATTRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFlatTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFlatTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL AttrFlatTranspFill::HasEquivalentDefaultValue(BOOL bAppearance = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrFlatTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0));
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrLinearFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrLinearFill::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrLinearFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);

		if (IsPerspective())
			Trans.Transform( GetEndPoint3(), 1);
	}
}

/********************************************************************************************

>	BOOL AttrLinearFill::CanTransform()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	NodeRenderable::CanTransform

********************************************************************************************/

BOOL AttrLinearFill::CanTransform()
{
	return TRUE;
}

/********************************************************************************************

>	FillControl AttrLinearFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/10/99
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrLinearFill::CheckForControlHit(DocCoord &ClickPos)
{
	return AttrFillGeometry::CheckForControlHit(ClickPos);
	
}

/********************************************************************************************

>	AttrFillGeometry* AttrLinearFill::DoColourDrop(AttrColourDrop* ColDrop) 

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/12/94
	Inputs:		The fill that is to be changed.
	Purpose:	Changes the colour of a fill + does the colour ramps too

********************************************************************************************/

AttrFillGeometry* AttrLinearFill::DoColourDrop(AttrColourDrop* ColDrop) 
{ 
	return AttrFillGeometry::DoColourDrop(ColDrop);
}

/********************************************************************************************

>	BOOL AttrLinearFill::ChangeControlColour(DocColour& Col, FillControl Cntrl,
						AttrColourDrop * pColDrop)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/2/95
	Purpose:	Applies a colour to a specific Fill Control Point.

********************************************************************************************/

BOOL AttrLinearFill::ChangeControlColour(DocColour& Col, FillControl Cntrl,
										   AttrColourDrop * pColDrop)
{
	return AttrFillGeometry::ChangeControlColour(Col, Cntrl, pColDrop);
	
}




/********************************************************************************************

>	void AttrLinearFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrLinearFill::RenderFillBlobs

********************************************************************************************/

void AttrLinearFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (AllowRampRedraw == FALSE)
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
		{
			pRamp->RenderSelectedBlob (ATTRVALUE(), pRender);
		}
		
		return;
	}
	
	DocCoord Start = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord End   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (Start == End)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);
	// This will have also calculated a point for us to draw
	// the line to, so we don't try and draw though the arrow head.

	// Set the line colours etc as we need them
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw a Line from the Start Point to the End point,
	// minus a bit to allow for the Arrow head on the end.
	pRender->SetLineWidth(BlobSize/4);
   	pRender->DrawLine(Start, LineEnd);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// Now Render the blobs on the path

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(Start, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(Start, BT_UNSELECTED);
	}

	// Draw a blob at the end point
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blob
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End, BT_UNSELECTED);
	}

	// now render any ramp blobs
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
	{
		pRamp->RenderRampBlobs(ATTRVALUE(), pRender, NULL);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}


/********************************************************************************************

>	void AttrFillGeometry::DrawEndBlobs()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Purpose:	Draws the blobs on the ends of the fill arrows.

********************************************************************************************/

void AttrFillGeometry::DrawEndBlobs()
{
}

/********************************************************************************************

>	void AttrLinearColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrLinearColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[2];
	ControlPoints[0] = (*GetStartPoint());
	ControlPoints[1] = (*GetEndPoint());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 2);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/********************************************************************************************

>	void AttrLinearTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/8/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrLinearTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're Not in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[2];
	ControlPoints[0] = (*GetStartPoint());
	ControlPoints[1] = (*GetEndPoint());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 2);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/********************************************************************************************

>	void AttrLinearFill::ValidateAttributeValue()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrLinearFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0))
		return;

	// Make some defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	// If the StartPoint is 'NULL' then make all points sensible
	if ((*GetStartPoint()) == DocCoord(0,0))
	{
		// Start in the middle
		DocCoord temp = CentreOf(AttrBounds); 
		SetStartPoint(&temp);

		// End on the Middle Right
		temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
	}

	// If the EndPoint is 'NULL' then make end points sensible
	if ((*GetEndPoint()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
	}

	if ((*GetEndPoint2()) == DocCoord(0,0))
	{
		DocCoord temp = MakeLineAtAngle(*GetStartPoint(), *GetEndPoint(), 90);
		SetEndPoint2(&temp);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrLinearColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

	void AttrLinearColourFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Linear Fill Colour attribute.

********************************************************************************************/

void AttrLinearColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrLinearColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrLinearColourFill::SimpleCopy()
{
	AttrLinearColourFill* NodeCopy = new AttrLinearColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrLinearColourFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrLinearColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_LINEARGRADFILL)); 
}                                  

/********************************************************************************************

>	void AttrLinearColourFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrLinearColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nLinear Graduated Fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart point =(#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point =(#1%ld, #2%ld), "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	for (INT32 i=0; i<5; i++)
	{
		TempStr._MakeMsg(TEXT("\r\nControl #1%ld = #2%ld"),
						 i, SelectionState[i]);
		(*Str) += TempStr;
	}
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrLinearColourFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrLinearColourFill::GetNodeSize() const 
{     
	return sizeof(AttrLinearColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrLinearColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the linear fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrLinearColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	// am I a multistage fill ??
	BOOL bMultistage = FALSE;

	ColourRamp * pRamp = GetColourRamp();
	INT32 * pRampColRefs = NULL;
	double * pPositions = NULL;
	ColRampItem * pItem = NULL;
	UINT32 NumRampItems = 0;

	if (pRamp)
	{
		// write out all the colour references
		if (pRamp->GetCount() > 0)
		{
			bMultistage = TRUE;
			NumRampItems = pRamp->GetCount();

			pRampColRefs = new INT32[NumRampItems];
			pPositions   = new double[NumRampItems];

			pItem = (ColRampItem *)pRamp->GetHead();

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (pItem)
				{
					pPositions[i]   = pItem->GetPosition();
					pRampColRefs[i] = pFilter->WriteRecord(&pItem->GetColour());
				}

				pItem = (ColRampItem *)pRamp->GetNext(pItem);
			}
		}
	}

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));

	// If the linear fill points are not perpendicular then we must write
	// a new 3-point linear fill record...
	BOOL b3PointLinear = (!AreLinesPerpendicular(GetStartPoint(), GetEndPoint(), GetEndPoint2()));

	if (ok)
	{
		if (!bMultistage)
		{
			INT32 tag = TAG_LINEARFILL;
			UINT32 size = TAG_LINEARFILL_SIZE;
			if (b3PointLinear)
			{
				tag = TAG_LINEARFILL3POINT;
				size = TAG_LINEARFILL3POINT_SIZE;
			}

			CamelotFileRecord Rec(pFilter, tag, size);
			
			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			if (ok && b3PointLinear) ok = Rec.WriteCoord(Value.EndPoint2);
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
			if (ok) ok = pFilter->Write(&Rec);
		}
		else
		{
			INT32 tag = TAG_LINEARFILLMULTISTAGE;
			UINT32 size = TAG_LINEARFILLMULTISTAGE_SIZE;
			if (b3PointLinear)
			{
				tag = TAG_LINEARFILLMULTISTAGE3POINT;
				size = TAG_LINEARFILLMULTISTAGE3POINT_SIZE;
			}

			CamelotFileRecord Rec(pFilter, tag, size);
			
			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			if (ok && b3PointLinear) ok = Rec.WriteCoord(Value.EndPoint2);
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);

			// now, write out all the colour ramp items
			if (ok) ok = Rec.WriteUINT32(NumRampItems);

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (ok) ok = Rec.WriteDOUBLE(pPositions[i]);
				if (ok) ok = Rec.WriteReference(pRampColRefs[i]);
			}

			if (ok) ok = pFilter->Write(&Rec);
		}
	}

	if (pRampColRefs)
		delete [] pRampColRefs;

	if (pPositions)
		delete [] pPositions;

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrDashPatternAttrLinearColourFillWritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrLinearColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrLinearColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrLinearColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));

	if (ok)
	{
		ColourRamp * pRamp = GetColourRamp();
		if (pRamp)
		if (pRamp->GetCount() > 0)
		{
			ok = pRamp->WriteColourDefinitions (pFilter);
		}
	}
	
	return (ok);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrLinearTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrLinearTranspFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Linear Fill Colour attribute.

********************************************************************************************/

void AttrLinearTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrLinearTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrLinearTranspFill::SimpleCopy()
{
	AttrLinearTranspFill* NodeCopy = new AttrLinearTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrLinearTranspFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrLinearTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_LINEARTRANSPFILL)); 
}                                  

/********************************************************************************************

>	void AttrLinearTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrLinearTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nLinear Graduated Transp:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = #1%u"), *GetStartTransp() );
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = #1%u"), *GetEndTransp() );
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart point =(#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point =(#1%ld, #2%ld), "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrLinearTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrLinearTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrLinearTranspFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrLinearTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the linear transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrLinearTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Start transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp  > 255,"End transparency level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	// If the linear fill points are not perpendicular then we must write
	// a new 3-point linear fill record...
	BOOL b3PointLinear = (!AreLinesPerpendicular(GetStartPoint(), GetEndPoint(), GetEndPoint2()));

	INT32 tag = TAG_LINEARTRANSPARENTFILL;
	UINT32 size = TAG_LINEARTRANSPARENTFILL_SIZE;
	if (b3PointLinear)
	{
		tag = TAG_LINEARTRANSPARENTFILL3POINT;
		size = TAG_LINEARTRANSPARENTFILL3POINT_SIZE;
	}

	CamelotFileRecord Rec(pFilter, tag, size);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok && b3PointLinear) ok = Rec.WriteCoord(Value.EndPoint2);
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrDashPatternAttrLinearTranspFillWritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrLinearTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL AttrLinearTranspFill::HasEquivalentDefaultValue(BOOL bAppearance = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrLinearTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrRadialFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrRadialFill::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrRadialFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);
		
		if (IsPerspective())
			Trans.Transform( GetEndPoint3(), 1);

		// Ensure the transformed Points are sensible
		if (IsCircular() &&
			(!AreLinesPerpendicular(GetStartPoint(), GetEndPoint(), GetEndPoint2()) ||
			( ABS( (*GetStartPoint()).Distance(*GetEndPoint()) 
								- (*GetStartPoint()).Distance(*GetEndPoint2()) ) ) > 5 ))
		{
			// Oh dear we're not circular any more !!
			MakeElliptical();
		}
	}
}

/********************************************************************************************

>	BOOL AttrRadialFill::CanTransform()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	NodeRenderable::CanTransform

********************************************************************************************/

BOOL AttrRadialFill::CanTransform()
{
	return TRUE;
}


/*********************************************************************************************

>    virtual BOOL AttrRadialFill::NeedsToRenderAtEachBrushStroke()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/11/99
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if this attribute should be rendered at every step of a brush stroke
     Purpose:   So that don't have to keep re-rendering attributes whilst drawing a brush, this
				identifies whether or not the attribute need to be rendered at each step, 
				e.g. radial fills.
     Errors:    
	 See Also;  Brush code (ndbrshmk.cpp)

**********************************************************************************************/
       
BOOL AttrRadialFill::NeedsToRenderAtEachBrushStroke() const
{
	return TRUE;
}

/********************************************************************************************

>	virtual void AttrRadialFill::OnControlDrag( DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a control point.

********************************************************************************************/

void AttrRadialFill::OnControlDrag( DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	INT32 dx, dy;

	// Which control point is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:
		
        	// Someone is dragging the Centre of the Fill
			dx = StartPoint.x - Pos.x;
			dy = StartPoint.y - Pos.y;
			// Move the other points relative
			EndPoint.translate(-dx, -dy);
			EndPoint2.translate(-dx, -dy);

			SetEndPoint(&EndPoint);
			SetEndPoint2(&EndPoint2);
			SetStartPoint(&Pos);
			break;

		case FILLCONTROL_ENDPOINT:
			
			// Someone is dragging the first End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The Aspect ratio can be locked either by it being circular
			// or by the Shift key		
			if (IsCircular() || ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point based on the aspect ratio
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, 90, INT32(StartPoint.Distance(EndPoint2) * Ratio));
				SetEndPoint2(&temp);
			}
			else
			{
				// Aspect ratio is not locked, so maintain the old line length
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, 90, INT32(StartPoint.Distance(EndPoint2)));
				SetEndPoint2(&temp);
			}

			SetEndPoint(&Pos);
			break;

		case FILLCONTROL_SECONDARYPOINT:

			// Someone is dragging the second End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The Aspect ratio can be locked either by it being circular
			// or by the Shift key		
			if (IsCircular() || ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint2);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point based on the aspect ratio
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, -90, INT32(StartPoint.Distance(EndPoint) * Ratio));
				SetEndPoint(&temp);
			}
			else
			{
				// Aspect ratio is not locked, so maintain the old line length
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, -90, INT32(StartPoint.Distance(EndPoint)));
				SetEndPoint(&temp);
			}

			SetEndPoint2(&Pos);
			break;
		default:
			// check for dragging a ramp point
			if (ISA_RAMPINDEX(DragControl))
			{
				AttrFillGeometry::OnControlDrag( Pos, DragControl, ClickMods);
			}
			break;
	}
#endif
}

/********************************************************************************************

>	void AttrRadialFill::ValidateAttributeValue()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrRadialFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0))
	{
		// If the EndPoint2 is 'NULL' then make it sensible
		if (*GetEndPoint2()	== DocCoord(0,0))
		{
			DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90);
			SetEndPoint2(&temp);
			return;
		}
		
//		if (IsElliptical())
//		{
//			SetEndPoint2(&MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90, 
//		 						INT32((*GetStartPoint()).Distance((*GetEndPoint2())))));
//		}
//		else
		if (IsCircular())
		{
			// This must be a Circle, so make sure EndPoint2 is on the same
			// radius as the other endpoint
			DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90);
			SetEndPoint2(&temp);
		}

		return;
	}

	// Make up some sensible defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	// If the StartPoint is 'NULL' then make all points sensible
	if ((*GetStartPoint()) == DocCoord(0,0))
	{
		// Start in the centre of the bounds
		DocCoord temp = CentreOf(AttrBounds);
		SetStartPoint(&temp);
		// and set End Points to Middle Right, and Middle Top
		temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
		temp = DocCoord((*GetStartPoint()).x, (*GetStartPoint()).y + (Height/2));
		SetEndPoint2(&temp);
	}

	// If the EndPoint is 'NULL' then make end points sensible
	if ((*GetEndPoint()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
		temp = DocCoord((*GetStartPoint()).x, (*GetStartPoint()).y + (Height/2));
		SetEndPoint2(&temp);
	}

	if ((*GetEndPoint2()) == DocCoord(0,0))
	{
		DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90, Height/2);
		SetEndPoint2(&temp);
	}

	if (IsCircular())
	{
		DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()));
		SetEndPoint2(&temp);
	}
#endif
}

/********************************************************************************************

>	virtual DocRect AttrRadialFill::GetBlobBoundingRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrRadialFill::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	// Get the Start and End Points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint   = *GetEndPoint();
	DocCoord EndPoint2  = *GetEndPoint2();

	// Make a dummy bounds from just the Start Point
	DocRect BoundingRect(StartPoint, StartPoint);

	if (DraggedFill == this)
	{
		// This fill is being dragged, so we have to include the Ellipse bounding
		// rect as well
		DocRect StartBlobRect;
		DocRect EndBlobRect;
		
		DocRect DragRect = GetMeshEllipseBounds(StartPoint, EndPoint, EndPoint2);

		// Get the Bounding rect of Blobs on each of the ends
		(Camelot.GetBlobManager())->GetBlobRect(DragRect.lo, &StartBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(DragRect.hi, &EndBlobRect);

	 	// Now include the Bottom Left and Top Right of each blob in the Bounds.
		// We have to do it like this to make sure that the DocRect's coords
		// are valid.  ie. The Hi's are Higher than the Lo's.
		BoundingRect.IncludePoint(StartBlobRect.lo);
		BoundingRect.IncludePoint(StartBlobRect.hi);
		BoundingRect.IncludePoint(EndBlobRect.lo);
		BoundingRect.IncludePoint(EndBlobRect.hi);
	}
	else
	{
		// We're not being dragged, so just calc the bounds of the Start and End Blobs
		DocRect StartBlobRect;
		DocRect EndBlobRect;
		DocRect End2BlobRect;

		// Get the Bounding rect of the Fill Line, including the Blobs on the ends
		(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &StartBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &EndBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &End2BlobRect);

	 	// Now include the Bottom Left and Top Right of each blob in the Bounds.
		// We have to do it like this to make sure that the DocRect's coords
		// are valid.  ie. The Hi's are Higher than the Lo's.
		BoundingRect.IncludePoint(StartBlobRect.lo);
		BoundingRect.IncludePoint(StartBlobRect.hi);
		BoundingRect.IncludePoint(EndBlobRect.lo);
		BoundingRect.IncludePoint(EndBlobRect.hi);
		BoundingRect.IncludePoint(End2BlobRect.lo);
		BoundingRect.IncludePoint(End2BlobRect.hi);
	}

	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint);
	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint2);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0, 0,0);
#endif
}

/********************************************************************************************

>	FillControl AttrRadialFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrRadialFill::CheckForControlHit(DocCoord &ClickPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	return AttrFillGeometry::CheckForControlHit(ClickPos);

	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
	DocRect BlobRect;

	// Get the current control positions
	DocCoord CentrePoint  = *GetStartPoint();
	DocCoord EndColBlob   = *GetEndPoint();
	DocCoord End2ColBlob  = *GetEndPoint2();

	// Get the rectangle around the Centre Control Point
	(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the End Control Point
	(Camelot.GetBlobManager())->GetBlobRect(EndColBlob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	if (IsElliptical())
	{
		// Only elliptical fills have a second end point
		
		// Get the rectangle around the Senond End Control Point
		(Camelot.GetBlobManager())->GetBlobRect(End2ColBlob, &BlobRect);
		// See if the Click Position is within the rectangle
		if ( BlobRect.ContainsCoord(ClickPos) )
			HitControl = FILLCONTROL_SECONDARYPOINT;
	}

	return HitControl;
#else
	return FILLCONTROL_NULL;
#endif
}

/********************************************************************************************

>	void AttrRadialFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrRadialFill::RenderFillBlobs

********************************************************************************************/

void AttrRadialFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (AllowRampRedraw == FALSE)
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
		{
			pRamp->RenderSelectedBlob (ATTRVALUE(), pRender);
		}
		
		return;
	}
	
	DocCoord Start = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord End   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (Start == End)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord End2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);
	// This will have also calculated a point for us to draw
	// the line to, so we don't try and draw though the arrow head.

	Path ArrowPath2;
	DocCoord LineEnd2;

	if (IsElliptical())
	{
		// Calculate the Arrow on the End of the Line2
		ArrowPath2.Initialise();
		MakeMeshArrow(&ArrowPath2, Start, End2, &LineEnd2);
	}

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(Start, LineEnd);

	// Draw the secondary line if needed
	if (IsElliptical())
		pRender->DrawLine(Start, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// Render arrow on the end of line2 if its there
	if (IsElliptical())
		pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		if (AllowBoundsRedraw)
		{
			// This fill is being dragged, so draw an Ellipse to show the
			// shape of the fill
			pRender->SetLineColour(COLOUR_SELECTEDBLOB);
			pRender->SetFillColour(COLOUR_NONE);
		
			Path EllipsePath;
			EllipsePath.Initialise();
			// Make an Elliptical path
			MakeMeshEllipse(&EllipsePath, Start, End, End2);

			pRender->DrawPath(&EllipsePath);
		}
	}

	// Now Render the blobs on the path

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(Start, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(Start, BT_UNSELECTED);
	}

	// Draw a blob at the end point
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End, BT_SELECTED);
		if (IsElliptical())
			pRender->DrawBlob(End2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End, BT_UNSELECTED);
		if (IsElliptical())
			pRender->DrawBlob(End2,BT_UNSELECTED);
	}

	// now render any ramp blobs
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		pRamp->RenderRampBlobs(ATTRVALUE(), pRender, NULL);

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	void AttrRadialColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrRadialColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/********************************************************************************************

>	void AttrRadialTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrRadialTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're Not in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/********************************************************************************************

>	void AttrRadialFill::MakeCircular()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Makes this fill into a circular one.
	SeeAlso:	AttrRadialFill::MakeElliptical;
				AttrRadialFill::IsElliptical;
				AttrRadialFill::IsCircular

********************************************************************************************/

void AttrRadialFill::MakeCircular()
{
	((FillGeometryAttribute*)GetAttributeValue())->SetAspectLock(TRUE);
}

/********************************************************************************************

>	void AttrRadialFill::MakeElliptical()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/94
	Purpose:	Makes this fill into an elliptical one.
	SeeAlso:	AttrRadialFill::MakeCircular;
				AttrRadialFill::IsElliptical;
				AttrRadialFill::IsCircular

********************************************************************************************/

void AttrRadialFill::MakeElliptical()
{
	((FillGeometryAttribute*)GetAttributeValue())->SetAspectLock(FALSE);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrRadialColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrRadialColourFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Radial Fill Colour attribute.

********************************************************************************************/

void AttrRadialColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrRadialColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrRadialColourFill::SimpleCopy()
{
	AttrRadialColourFill* NodeCopy = new AttrRadialColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrRadialColourFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrRadialColourFill::GetAttrNameID(void)  
{
	if (IsElliptical ())
	{
		return (_R(IDS_ELLIPITICALGRADFILL));
	}
	else
	{
		return (_R(IDS_CIRCULARGRADFILL));
	}
}                                  



/********************************************************************************************

>	void AttrRadialColourFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrRadialColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nRadial Graduated Fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart point =(#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point =(#1%ld, #2%ld), "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point2 = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrRadialColourFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrRadialColourFill::GetNodeSize() const 
{     
	return sizeof(AttrRadialColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrRadialColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the radial fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrRadialColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	UINT32 Tag = TAG_CIRCULARFILL, TagSize = TAG_CIRCULARFILL_SIZE;

	if (IsElliptical())
	{
		Tag		= TAG_ELLIPTICALFILL;
		TagSize = TAG_ELLIPTICALFILL_SIZE;
	}

	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	// am I a multistage fill ??
	BOOL bMultistage = FALSE;

	// write out the colours for the ramp
	ColourRamp * pRamp = GetColourRamp();
	INT32 * pRampColRefs = NULL;
	double * pPositions = NULL;
	ColRampItem * pItem = NULL;
	UINT32 NumRampItems = 0;

	if (pRamp)
	{
		// write out all the colour references
		if (pRamp->GetCount() > 0)
		{
			bMultistage = TRUE;
			NumRampItems = pRamp->GetCount();

			pRampColRefs = new INT32[NumRampItems];
			pPositions   = new double[NumRampItems];

			pItem = (ColRampItem *)pRamp->GetHead();

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (pItem)
				{
					pPositions[i]   = pItem->GetPosition();
					pRampColRefs[i] = pFilter->WriteRecord(&pItem->GetColour());
				}

				pItem = (ColRampItem *)pRamp->GetNext(pItem);
			}
		}
	}

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));
		
	if (!bMultistage)
	{
		if (ok)
		{
			CamelotFileRecord Rec(pFilter,Tag,TagSize);
			
			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			
			if (ok && IsElliptical())						// Write out extra control point in the ellipse case
				ok = Rec.WriteCoord(Value.EndPoint2);
			
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);

			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());

			if (ok) ok = pFilter->Write(&Rec);
		}
	}
	else
	{
		if (IsElliptical())
		{
			Tag = TAG_ELLIPTICALFILLMULTISTAGE;
			TagSize = TAG_ELLIPTICALFILLMULTISTAGE_SIZE;
		}
		else
		{
			Tag = TAG_CIRCULARFILLMULTISTAGE;
			TagSize = TAG_CIRCULARFILLMULTISTAGE_SIZE;
		}

		if (ok)
		{
			CamelotFileRecord Rec(pFilter,Tag,TagSize);
			
			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);

			if (ok && IsElliptical())						// Write out extra control point in the ellipse case
				ok = Rec.WriteCoord(Value.EndPoint2);

			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);

			// now, write out all the colour ramp items
			if (ok) ok = Rec.WriteUINT32(NumRampItems);

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (ok) ok = Rec.WriteDOUBLE(pPositions[i]);
				if (ok) ok = Rec.WriteReference(pRampColRefs[i]);
			}

			if (ok) ok = pFilter->Write(&Rec);
		}
	}

	if (pRampColRefs)
		delete [] pRampColRefs;

	if (pPositions)
		delete [] pPositions;


	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrRadialColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrRadialColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrRadialColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrRadialColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));

	if (ok)
	{
		ColourRamp * pRamp = GetColourRamp();
		if (pRamp)
		if (pRamp->GetCount() > 0)
		{
			ok = pRamp->WriteColourDefinitions (pFilter);
		}
	}
	
	return (ok);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrRadialTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrRadialTranspFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Radial Fill Colour attribute.

********************************************************************************************/

void AttrRadialTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrRadialTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrRadialTranspFill::SimpleCopy()
{
	AttrRadialTranspFill* NodeCopy = new AttrRadialTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrRadialTranspFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrRadialTranspFill::GetAttrNameID(void)  
{
	if (!IsATranspFill ())
	{
		if (IsElliptical ())
		{
			return (_R(IDS_LINEARGRADFILL)); 
		}
		else
		{
			return (_R(IDS_CIRCULARGRADFILL));
		}
	}
	else
	{
		if (IsElliptical ())
		{
			return (_R(IDS_ELLIPITICALTRANSPFILL)); 
		}
		else
		{
			return (_R(IDS_CIRCULARTRANSPFILL));
		}
	}
}                                  

/********************************************************************************************

>	void AttrRadialTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrRadialTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nRadial Graduated Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart point =(#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point =(#1%ld, #2%ld), "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point2 = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrRadialTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrRadialTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrRadialTranspFill);
}  


/*********************************************************************************************

>    virtual BOOL AttrRadialTranspFill::NeedsToRenderAtEachBrushStroke()

     Author:    Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
     Created:   29/11/99
     Inputs:    -
     Outputs:   -
     Returns:   TRUE if this attribute should be rendered at every step of a brush stroke
     Purpose:   So that don't have to keep re-rendering attributes whilst drawing a brush, this
				identifies whether or not the attribute need to be rendered at each step, 
				e.g. radial fills.
     Errors:    
	 See Also;  Brush code (ndbrshmk.cpp)

**********************************************************************************************/
       
BOOL AttrRadialTranspFill::NeedsToRenderAtEachBrushStroke() const
{
	return TRUE;
}

/********************************************************************************************

  >	virtual BOOL AttrRadialTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the radial transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrRadialTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Start transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp  > 255,"End transparency level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	UINT32 Tag = TAG_CIRCULARTRANSPARENTFILL, TagSize = TAG_CIRCULARTRANSPARENTFILL_SIZE;

	if (IsElliptical())
	{
		Tag		= TAG_ELLIPTICALTRANSPARENTFILL;
		TagSize = TAG_ELLIPTICALTRANSPARENTFILL_SIZE;
	}

	CamelotFileRecord Rec(pFilter,Tag,TagSize);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);

	if (ok && IsElliptical())
		if (ok) ok = Rec.WriteCoord(Value.EndPoint2);

	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

/********************************************************************************************
>	BOOL AttrRadialTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrRadialTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




//--------------------------------------------------------------
// See AttrDashPatternAttrRadialTranspFillWritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrRadialTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrConicalFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrConicalFill::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrConicalFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);

//		ValidateAttributeValue();
	}
}

/********************************************************************************************

>	void AttrConicalFill::TransformSelectedControlPoints( TransformBase& Trans)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the selected control points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrConicalFill::TransformSelectedControlPoints( TransformBase& Trans, BOOL* isARampBlob /*= NULL*/)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ( Trans.TransFills )
	{
		ClickModifiers ClickMods; /* = ClickModifiers::GetClickModifiers();*/
		ClickMods.Adjust = TRUE;

		if (GetStartPoint() && IsSelected(FILLCONTROL_STARTPOINT))
		{
			DocCoord Pos = *GetStartPoint();
			Trans.Transform( &Pos, 1);

			FillControl Pt = FILLCONTROL_DRAGPOINT;
			
			OnControlDrag(Pos, Pt, ClickMods);
		}

		if (GetEndPoint() && IsSelected(FILLCONTROL_ENDPOINT))
		{
			DocCoord Pos = *GetEndPoint();
			Trans.Transform( &Pos, 1);

			FillControl End = FILLCONTROL_ENDPOINT;

			OnControlDrag(Pos, End, ClickMods);
		}

		// CGS - wo, lets check the fill ramp as well ....

		TransformSelectedFillRampControlPoints (Trans, ClickMods, isARampBlob);
	}
#endif
}

/********************************************************************************************

>	BOOL AttrConicalFill::CanTransform()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	NodeRenderable::CanTransform

********************************************************************************************/

BOOL AttrConicalFill::CanTransform()
{
	return TRUE;
}


/********************************************************************************************

>	FillControl AttrConicalFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrConicalFill::CheckForControlHit(DocCoord &ClickPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
	DocRect BlobRect;

	// Get the current control positions
	DocCoord CentrePoint  = *GetStartPoint();
	DocCoord EndColBlob   = *GetEndPoint();
	DocCoord StartColBlob = MakeLineAtAngle(CentrePoint, EndColBlob, 180);

	// Get the rectangle around the Centre Control Point
	(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_DRAGPOINT;

	// Get the rectangle around the Start Colour Control Point
	(Camelot.GetBlobManager())->GetBlobRect(StartColBlob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the End Colour Control Point
	(Camelot.GetBlobManager())->GetBlobRect(EndColBlob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	// check the fill ramp blobs
	if (HitControl == FILLCONTROL_NULL)
	{
		// do we have a fill ramp ?
		if (GetColourRamp())
		{
//			ColourRamp * pRamp = GetColourRamp();

			// check for a hit
			return AttrFillGeometry::CheckForControlHit(ClickPos);
		}
		else
		{
			// check the position on the fill ramp
			double d = FindRampPoint(ClickPos, *GetStartPoint(), *GetEndPoint());

			if (d > 0 && d < 1)
			{
				return FILLCONTROL_RAMPPOINT;
			}
		}			
	}

	return HitControl;
#else
	return FILLCONTROL_NULL;
#endif
}

/********************************************************************************************

>	AttrFillGeometry* AttrConicalFill::DoColourDrop(AttrColourDrop*)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/99
	Purpose:	Does colour drop onto conical fills

********************************************************************************************/
AttrFillGeometry* AttrConicalFill::DoColourDrop(AttrColourDrop* ColDrop)
{
	AttrFillGeometry* NewFill;
	DocColour DroppedColour = *ColDrop->GetStartColour();

	// First test to see which colour we should change
	FillControl ControlHit = TestColourDrop(ColDrop);

	if (ControlHit == FILLCONTROL_NULL)
	{
		// No Control points hit, so make a flat fill instead
        NewFill = new AttrFlatColourFill;
		if (NewFill == NULL)
			return NULL;

		// and use the mutants colour
		NewFill->SetStartColour(&DroppedColour);

		BOOL InSelection = ColDrop->IsObjectSelected();

		if (InSelection &&
			GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && 
			IsVisible()
			)
		{
			BOOL UniqueFill = TRUE;

			AttrFillGeometry* pAttr = FindFirstSelectedAttr();
			while (pAttr != NULL)
			{
 				if (pAttr != this && GetAttributeType() == pAttr->GetAttributeType())
				{
					if ( IsMeshSame((FillGeometryAttribute*)GetAttributeValue(),
									(FillGeometryAttribute*)pAttr->GetAttributeValue()) )
					{
						UniqueFill = FALSE;
						break;
					}
				}

				// Move onto the next attr
				pAttr = FindNextSelectedAttr();
			}

			/*
			if (UniqueFill)
				RenderFillBlobs();
				*/
		}
		
		return NewFill;
	}

	if (FindParent() && FindParent()->IsCompound())
	{
		if (ColDrop->GetObjectDroppedOn() && 
			!(ColDrop->GetObjectDroppedOn()->IsCompound()) && 
			ColDrop->IsObjectSelected())
		{
			// If we are dropping inside a group, then we'll need to remove
			// the blobs if this fill is applied to the whole group
			// RenderFillBlobs();
		}
	}

	// Make a copy of this Fill to change
	NewFill = (AttrFillGeometry*)this->SimpleCopy();
	if (NewFill == NULL)
		return NULL;

	if (!NewFill->ChangeControlColour(DroppedColour, ControlHit, ColDrop))
	{
		delete NewFill;
		return NULL;
	}

	// invalidate the region in the document & render the fill blobs
	Document * pDoc = Document::GetCurrent();

	if (pDoc)
		pDoc->ForceRedraw(FindParentSpread(), GetBlobBoundingRect(), FALSE, this->FindParent());

	// Return the Mutated Fill
	return NewFill;
}




/********************************************************************************************

>	void AttrConicalFill::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		Control, the FillControl to redraw
	Purpose:	Redraws the Fill Control blobs when the selection changes.
	SeeAlso:	FillControl			

********************************************************************************************/

void AttrConicalFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint   = *GetEndPoint();
	DocCoord EndPoint2  = 
		MakeLineAtAngle(StartPoint, EndPoint, 180);

	switch (Control)
	{
		case FILLCONTROL_DRAGPOINT:
			// Redraw the Centre Point Blob
			(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &ControlRect);
			break;

		case FILLCONTROL_STARTPOINT:
			// Redraw the Start Colour Blob
			(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &ControlRect);
			break;

		case FILLCONTROL_ENDPOINT:
			// Redraw the End Colour Blob
			(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &ControlRect);
			break;
	}

	if (ISA_RAMPINDEX(Control))
	{
		FillRamp* pRamp = GetFillRamp();
		if (pRamp)
		{
			DocCoord point = pRamp->GetGeometryCoord(ATTRVALUE(), Control);
			(Camelot.GetBlobManager())->GetBlobRect(point, &ControlRect, TRUE);
		}
	}	

	// Redraw the rectangle we calculated above
	RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
#endif
}

/********************************************************************************************

>	virtual void AttrConicalFill::OnControlDrag( DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a fill control point.

********************************************************************************************/

void AttrConicalFill::OnControlDrag( DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
	// Get the current Control Point Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();

	INT32 dx, dy;

	switch (DragControl)
	{
		case FILLCONTROL_DRAGPOINT:
			// They're dragging the Centre Point
			dx = StartPoint.x - Pos.x;
			dy = StartPoint.y - Pos.y;
			// Move the other points relative
			EndPoint.translate(-dx, -dy);
			SetEndPoint(&EndPoint);
			SetStartPoint(&Pos);
			break;

		case FILLCONTROL_STARTPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			{
				DocCoord coord( MakeLineAtAngle( StartPoint, Pos, 180 ) );
				SetEndPoint( &coord );
			}
			break;

		case FILLCONTROL_ENDPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			SetEndPoint(&Pos);
			break;
		default:
			// is the hit point a ramp point ?
			if (ISA_RAMPINDEX(DragControl))
			{
				FillRamp *pRamp = GetFillRamp();
				
				if (pRamp)
				{
					float t = (float)FindRampPoint(Pos, *GetStartPoint(), *GetEndPoint());

					if (t > 0 && t < 1.0)
					{	
						FillControl NewDragControl = pRamp->SetPosition(DragControl, t);

						if (NewDragControl != DragControl)
						{
							DragControl = NewDragControl;
						}

						Document * pDoc = Document::GetCurrent();
						
						if (pDoc)
						{
							Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));
							
							if (pSpread)
							{
								if (AllowRampRedraw)
								{
									pDoc->ForceRedraw(pSpread, GetBlobBoundingRect(), FALSE, this->FindParent());
								}
							}
							else
							{
								pSpread = Document::GetSelectedSpread();
								
								if (pSpread)
								{
									if (AllowRampRedraw)
									{
										pDoc->ForceRedraw(pSpread, GetBlobBoundingRect(), FALSE, this->FindParent());
									}
									
								}
							}
						}
						
					}
				}
			}
			break;
	}
}

/********************************************************************************************

>	double AttrConicalFill::FindRampPoint(DocCoord &dc, DocCoord &StartPoint, DocCoord &EndPoint);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/10/99
	Returns:	-
	Purpose:	Gives the value (0 - 1) of the given dc - bearing in mind the start point is
				the centre of the conical fill and end point is the start point on the conical
				curve

********************************************************************************************/


double AttrConicalFill::FindRampPoint(DocCoord &dc, DocCoord &StartPoint, DocCoord &EndPoint)
{
	//get the dot product
	DocCoord Centre((StartPoint.x + EndPoint.x) / 2, (StartPoint.y + EndPoint.y) / 2);

	double dStartX = StartPoint.x;
	double dStartY = StartPoint.y;

	double dEndX = EndPoint.x;
	double dEndY = EndPoint.y;

	double dVecX = dEndX - dStartX;
	double dVecY = dEndY - dStartY;

	// calculate the distance between the point and the centre point
	double ddX = dc.x - StartPoint.x;
	double ddY = dc.y - StartPoint.y;

	double DistToPoint = sqrt((ddX * ddX) + (ddY * ddY));
	
	// normalise the vector

	double DistToEndPoint = sqrt((dVecX * dVecX) + (dVecY * dVecY));

	double dRMod = 1/DistToEndPoint;
	dVecX *= dRMod;
	dVecY *= dRMod;

	double dToPointX = ((double)dc.x) - dStartX;
	double dToPointY = ((double)dc.y) - dStartY;

	dRMod = 1/sqrt((dToPointX * dToPointX) + (dToPointY * dToPointY));

	dToPointX *= dRMod;
	dToPointY *= dRMod;

	double dot = (dToPointX * dVecX) + (dToPointY * dVecY);

	// do the 90 degree dot product as well since we need to know
	// whether the point is over the blob itself, or over its mirror
	// image
	double dot90 = (-dToPointY * dVecX) + (dToPointX * dVecY);

	if (dot > 1.0)
		dot = 1.0;

	if (dot < -1.0)
		dot = -1.0;

	// get the angle
	double Angle = acos(dot) * 180.0 / 3.142;

	double Retn = (180.0 - Angle) / 180.0;

	double dBlobSize = (double)(Camelot.GetBlobManager())->GetBlobSize();

	// is the point in range ?
	if (fabs(DistToPoint - DistToEndPoint) < dBlobSize && dot90 < 0)
	{
		return Retn;
	}

	return -1;
}

/********************************************************************************************

>	void AttrConicalFill::ValidateAttributeValue()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Returns:	-
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrConicalFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0))
		return;

	// Make some defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	// If the StartPoint is 'NULL' then make all points sensible
	if ((*GetStartPoint()) == DocCoord(0,0))
	{
		// Start in the middle
		DocCoord temp = CentreOf(AttrBounds);
		SetStartPoint(&temp);

		// End on the Middle Right
		temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
	}

	// If the EndPoint is 'NULL' then make end points sensible
	if ((*GetEndPoint()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
	}
#endif
}

/********************************************************************************************

>	virtual DocRect AttrConicalFill::GetBlobBoundingRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/94
	Returns:	DocRect - The rectangle that contains all the attrs selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrConicalFill::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	// Get the Start and End Points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint   = *GetEndPoint();
	DocCoord EndPoint2  = MakeLineAtAngle(StartPoint, EndPoint, 180);

	DocRect StartBlobRect;
	DocRect EndBlobRect;
	// Get the bounds of the funny semi-circle mesh thingy
	DocRect BoundingRect = GetMeshEllipseBounds(StartPoint, EndPoint);

	// Get the Bounding rect of the Fill Line, including the Blobs on the ends
	(Camelot.GetBlobManager())->GetBlobRect(BoundingRect.lo, &StartBlobRect);
	(Camelot.GetBlobManager())->GetBlobRect(BoundingRect.hi, &EndBlobRect);

	// Now include the Bottom Left and Top Right of each blob in the Bounds.
	// We have to do it like this to make sure that the DocRect's coords
	// are valid.  ie. The Hi's are Higher than the Lo's.
	BoundingRect.IncludePoint(StartBlobRect.lo);
	BoundingRect.IncludePoint(StartBlobRect.hi);
	BoundingRect.IncludePoint(EndBlobRect.lo);
	BoundingRect.IncludePoint(EndBlobRect.hi);

	DocCoord Temp  = MakeLineAtAngle(EndPoint, StartPoint, -90);
	IncludeArrowHead(&BoundingRect, Temp, EndPoint);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0, 0,0);
#endif
}

/********************************************************************************************

>	void AttrConicalFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrConicalFill::RenderFillBlobs

********************************************************************************************/

void AttrConicalFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (AllowRampRedraw == FALSE)
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
		{
			pRamp->RenderSelectedBlob (ATTRVALUE(), pRender);
		}
		
		return;
	}
	
	DocCoord Start = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord End   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (Start == End)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord End2  = MakeLineAtAngle(Start, End, 180);

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	DocCoord Temp  = MakeLineAtAngle(End, Start, -90);
	MakeMeshArrow(&ArrowPath, Temp, End, &LineEnd);
	// This will have also calculated a point for us to draw
	// the line to, so we don't try and draw though the arrow head.

	Path EllipsePath;
	EllipsePath.Initialise();
	MakeMeshSemiCircle(&EllipsePath, Start, End, LineEnd);

	// Set the line colours etc as we need them
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetFillColour(COLOUR_NONE);

	if (DraggedFill == this)
	{
		if (AllowBoundsRedraw)
		{
			// If we are being dragged, then draw some lines to show the
			// fill angle
			pRender->DrawLine(Start, End);
			pRender->DrawLine(Start, End2);
		}
	}

	// Render a Circle
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawPath(&EllipsePath);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRender->DrawPath(&ArrowPath);

	// Now Render the blobs on the path
//	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
	pRender->DrawBlob(Start, BT_UNSELECTED);

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End, BT_UNSELECTED);
	}

	// Draw a blob at the end point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End2, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End2, BT_UNSELECTED);
	}

	// now render any ramp blobs
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		pRamp->RenderRampBlobs(ATTRVALUE(), pRender, NULL);

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	void AttrConicalColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrConicalColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[2];
	ControlPoints[0] = (*GetStartPoint());
	ControlPoints[1] = (*GetEndPoint());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 2);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/********************************************************************************************

>	void AttrConicalTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrConicalTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're Not in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[2];
	ControlPoints[0] = (*GetStartPoint());
	ControlPoints[1] = (*GetEndPoint());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 2);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrConicalColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrConicalColourFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Conical Fill Colour attribute.

********************************************************************************************/

void AttrConicalColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrConicalColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrConicalColourFill::SimpleCopy()
{
	AttrConicalColourFill* NodeCopy = new AttrConicalColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrConicalColourFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrConicalColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_CONICALGRADFILL)); 
}                                  

/********************************************************************************************

>	void AttrConicalColourFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrConicalColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nConical Graduated Fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart point =(#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point =(#1%ld, #2%ld), "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrConicalColourFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrConicalColourFill::GetNodeSize() const 
{     
	return sizeof(AttrConicalColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrConicalColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the conical fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrConicalColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	// am I a multistage fill ??
	BOOL bMultistage = FALSE;

	ColourRamp * pRamp = GetColourRamp();
	INT32 * pRampColRefs = NULL;
	double * pPositions = NULL;
	ColRampItem * pItem = NULL;
	UINT32 NumRampItems = 0;

	if (pRamp)
	{
		// write out all the colour references
		if (pRamp->GetCount() > 0)
		{
			bMultistage = TRUE;
			NumRampItems = pRamp->GetCount();

			pRampColRefs = new INT32[NumRampItems];
			pPositions   = new double[NumRampItems];

			pItem = (ColRampItem *)pRamp->GetHead();

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (pItem)
				{
					pPositions[i]   = pItem->GetPosition();
					pRampColRefs[i] = pFilter->WriteRecord(&pItem->GetColour());
				}

				pItem = (ColRampItem *)pRamp->GetNext(pItem);
			}
		}
	}

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));
		
	if (ok)
	{
		if (!bMultistage)
		{
			CamelotFileRecord Rec(pFilter,TAG_CONICALFILL,TAG_CONICALFILL_SIZE);

			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
			if (ok) ok = pFilter->Write(&Rec);
		}
		else
		{
			CamelotFileRecord Rec(pFilter,TAG_CONICALFILLMULTISTAGE,
				TAG_CONICALFILLMULTISTAGE_SIZE);

			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);

			// now, write out all the colour ramp items
			if (ok) ok = Rec.WriteUINT32(NumRampItems);

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (ok) ok = Rec.WriteDOUBLE(pPositions[i]);
				if (ok) ok = Rec.WriteReference(pRampColRefs[i]);
			}

			if (ok) ok = pFilter->Write(&Rec);
		}
	}

	if (pPositions)
		delete [] pPositions;

	if (pRampColRefs)
		delete [] pRampColRefs;

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrConicalColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrConicalColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrConicalColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrConicalColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));

	if (ok)
	{
		ColourRamp * pRamp = GetColourRamp();
		if (pRamp)
		if (pRamp->GetCount() > 0)
		{
			ok = pRamp->WriteColourDefinitions (pFilter);
		}
	}
	
	return (ok);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrConicalTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrConicalTranspFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Conical Fill Colour attribute.

********************************************************************************************/

void AttrConicalTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrConicalTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrConicalTranspFill::SimpleCopy()
{
	AttrConicalTranspFill* NodeCopy = new AttrConicalTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrConicalTranspFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrConicalTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_CONICALTRANSPFILL)); 
}                                  

/********************************************************************************************

>	void AttrConicalTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrConicalTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nConical Graduated Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart point =(#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd point =(#1%ld, #2%ld), "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrConicalTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrConicalTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrConicalTranspFill);
}  




/********************************************************************************************

  >	virtual BOOL AttrConicalTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the conical transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrConicalTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Start transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp  > 255,"End transparency level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_CONICALTRANSPARENTFILL,TAG_CONICALTRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrConicalTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrConicalTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}



/********************************************************************************************
>	BOOL AttrConicalTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrConicalTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrSquareFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	virtual FillControl AttrSquareFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		ColDrop - pointer to the 
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrSquareFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	FillControl ControlHit = CheckForControlHit(DropPoint);
	
	// If it hit one of our control points, then use that
	if (ControlHit != FILLCONTROL_NULL)
		return ControlHit;

	// It didn't hit any of our control points, so if the drop is over
	// the object then we'll make a guess as to which control point
	// the user would like to change, depending on which area of the
	// object the pointer is over.

	// First make sure we're actually over an object
	NodeRenderableInk* pParentObject = ColDrop->GetObjectDroppedOn();
	if (pParentObject == NULL)
		return FILLCONTROL_NULL;	// We're not over any object 

	// Make sure this fill type has some Control Points
	if (GetStartPoint() == NULL || GetEndPoint() == NULL || GetEndPoint2() == NULL)
		return FILLCONTROL_NULL;

	// This next bit effectively transforms the drop point into the parallelogram coord space
	// t & u specify the coords (i.e. EndPoint is t = 1 and u = 0 and EndPoint2 is t = 0 and u = 1
	// If the absolute value of either coord is less than 0.5 then we are closer to the start point
	
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint 	= *GetEndPoint();
	DocCoord EndPoint2 	= *GetEndPoint2();

	DocCoord P = EndPoint - StartPoint;
	DocCoord Q = EndPoint2 - StartPoint;
	DocCoord N = DropPoint - StartPoint;

	double px = P.x;
	double py = P.y;
	double qx = Q.x;
	double qy = Q.y;
	double nx = N.x;
	double ny = N.y;
		
	double den = (px * qy) - (py * qx);

	// Default to end point
	ControlHit = FILLCONTROL_ENDPOINT;

	// Guard against divide by zero
	// If this is zero it indicates that the fill is collapsed in some way
	// We always hit the end point because that is what is displayed

	if (den != 0.0)
	{
		// Calculate u
		double u = ((px * ny) - (py * nx)) / den;

		// If this is smaller than 0.5 then we need to calculate t
		if (fabs(u) < 0.5)
		{
			double t = 1.0;

			if (px != 0.0)
				t = (nx - (u * qx)) / px;
			else
				t = (ny - (u * qy)) / py;

			if (fabs(t) < 0.5)
				ControlHit = FILLCONTROL_STARTPOINT;
		}
	}

	return ControlHit;

#else
	return FILLCONTROL_NULL;
#endif
}



/********************************************************************************************

>	void AttrSquareFill::Transform( TransformBase& Trans )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	Copied from AttrLinearFill

********************************************************************************************/

void AttrSquareFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);

		if (IsPerspective())
			Trans.Transform( GetEndPoint3(), 1);
	}
}



/********************************************************************************************

>	BOOL AttrSquareFill::CanTransform()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	Copied from AttrLinearFill

********************************************************************************************/

BOOL AttrSquareFill::CanTransform()
{
	return TRUE;
}



/********************************************************************************************

>	void AttrSquareFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrSquareFill::RenderFillBlobs

********************************************************************************************/

void AttrSquareFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (AllowRampRedraw == FALSE)
	{
		FillRamp *pRamp = GetFillRamp();
		if (pRamp)
		{
			pRamp->RenderSelectedBlob (ATTRVALUE(), pRender);
		}
		
		return;
	}
	
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, StartPoint, EndPoint, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, StartPoint, EndPoint2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(StartPoint, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(StartPoint, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		if (AllowBoundsRedraw)
		{
			// If we are being dragged then draw a Parallelgram to
			// show the shape of the bitmap
			pRender->SetLineColour(COLOUR_SELECTEDBLOB);
			pRender->SetFillColour(COLOUR_NONE);
		
			Path ParaPath;
			ParaPath.Initialise();
			MakeMeshParallelagram(&ParaPath, StartPoint, EndPoint, EndPoint2);

			// Draw the parallelagram
			pRender->DrawPath(&ParaPath);
		}
	}

	// Now Render the blobs on the path
	// Set the line colour to none
	pRender->SetLineColour(COLOUR_NONE);

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(StartPoint, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(StartPoint, BT_UNSELECTED);
	}

	// Draw the blobs at the end points
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blobs
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(EndPoint, BT_SELECTED);
		pRender->DrawBlob(EndPoint2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blobs
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(EndPoint, BT_UNSELECTED);
		pRender->DrawBlob(EndPoint2,BT_UNSELECTED);
	}

	// now render any ramp blobs
	FillRamp *pRamp = GetFillRamp();
	if (pRamp)
		pRamp->RenderRampBlobs(ATTRVALUE(), pRender, NULL);

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}



/********************************************************************************************

>	virtual void AttrSquareFill::OnControlDrag(DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a control point.

********************************************************************************************/

void AttrSquareFill::OnControlDrag(DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	INT32 dx, dy;

	// Which control point is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:
		
        	// Someone is dragging the Centre of the Fill
			dx = StartPoint.x - Pos.x;
			dy = StartPoint.y - Pos.y;
			// Move the other points relative
			EndPoint.translate(-dx, -dy);
			EndPoint2.translate(-dx, -dy);

			SetEndPoint(&EndPoint);
			SetEndPoint2(&EndPoint2);
			SetStartPoint(&Pos);
			break;

		case FILLCONTROL_ENDPOINT:
			
			// Someone is dragging the first End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The Aspect ratio can be locked either by it being circular
			// or by the Shift key		
			if (ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point based on the aspect ratio
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, 90, INT32(StartPoint.Distance(EndPoint2) * Ratio));
				SetEndPoint2(&temp);
			}

			SetEndPoint(&Pos);
			break;

		case FILLCONTROL_SECONDARYPOINT:

			// Someone is dragging the second End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The Aspect ratio can be locked either by it being circular
			// or by the Shift key		
			if (ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint2);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point based on the aspect ratio
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, -90, INT32(StartPoint.Distance(EndPoint) * Ratio));
				SetEndPoint(&temp);
			}

			SetEndPoint2(&Pos);
			break;

			default:
			// check for dragging a ramp point
			if (ISA_RAMPINDEX(DragControl))
			{
				AttrFillGeometry::OnControlDrag( Pos, DragControl, ClickMods);
			}
			break;
	}
#endif
}



/********************************************************************************************

>	virtual DocRect AttrSquareFill::GetBlobBoundingRect()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrSquareFill::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	// Get the Start and End Points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint   = *GetEndPoint();
	DocCoord EndPoint2  = *GetEndPoint2();

	// Make a dummy bounds from just the Start Point
	DocRect BoundingRect(StartPoint, StartPoint);

	if (DraggedFill == this)
	{
		// This fill is being dragged, so we have to include the Parallelagram 
		// bounding rect as well
		DocRect StartBlobRect;
		DocRect EndBlobRect;
		DocRect DragRect = GetParallelagramBounds(StartPoint, EndPoint, EndPoint2);

		// Get the Bounding rect of Blobs on each of the ends
		(Camelot.GetBlobManager())->GetBlobRect(DragRect.lo, &StartBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(DragRect.hi, &EndBlobRect);

	 	// Now include the Bottom Left and Top Right of each blob in the Bounds.
		// We have to do it like this to make sure that the DocRect's coords
		// are valid.  ie. The Hi's are Higher than the Lo's.
		BoundingRect.IncludePoint(StartBlobRect.lo);
		BoundingRect.IncludePoint(StartBlobRect.hi);
		BoundingRect.IncludePoint(EndBlobRect.lo);
		BoundingRect.IncludePoint(EndBlobRect.hi);
	}
	else
	{
		// We're not being dragged, so just calc the bounds of the Start and End Blobs
		DocRect StartBlobRect;
		DocRect EndBlobRect;
		DocRect End2BlobRect;

		// Get the Bounding rect of the Fill Line, including the Blobs on the ends
		(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &StartBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &EndBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &End2BlobRect);

	 	// Now include the Bottom Left and Top Right of each blob in the Bounds.
		// We have to do it like this to make sure that the DocRect's coords
		// are valid.  ie. The Hi's are Higher than the Lo's.
		BoundingRect.IncludePoint(StartBlobRect.lo);
		BoundingRect.IncludePoint(StartBlobRect.hi);
		BoundingRect.IncludePoint(EndBlobRect.lo);
		BoundingRect.IncludePoint(EndBlobRect.hi);
		BoundingRect.IncludePoint(End2BlobRect.lo);
		BoundingRect.IncludePoint(End2BlobRect.hi);
	}

	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint);
	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint2);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0,0,0);
#endif
}



/********************************************************************************************

>	void AttrSquareFill::ValidateAttributeValue()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrSquareFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0))
	{
		// If the EndPoint2 is 'NULL' then make it sensible
		if (*GetEndPoint2()	== DocCoord(0,0))
		{
			DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90);
			SetEndPoint2(&temp);
		}
		
		return;
	}

	// Make up some sensible defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	// If the StartPoint is 'NULL' then make all points sensible
	if ((*GetStartPoint()) == DocCoord(0,0))
	{
		// Start in the centre of the bounds
		DocCoord temp = CentreOf(AttrBounds);
		SetStartPoint(&temp);

		// and set End Points to Middle Right, and Middle Top
		temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
		temp = DocCoord((*GetStartPoint()).x, (*GetStartPoint()).y + (Height/2));
		SetEndPoint2(&temp);
	}

	// If the EndPoint is 'NULL' then make end points sensible
	if ((*GetEndPoint()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord((*GetStartPoint()).x + (Width/2), (*GetStartPoint()).y);
		SetEndPoint(&temp);
		temp = DocCoord((*GetStartPoint()).x, (*GetStartPoint()).y + (Height/2));
		SetEndPoint2(&temp);
	}

	if ((*GetEndPoint2()) == DocCoord(0,0))
	{
		DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90, Height/2);
		SetEndPoint2(&temp);
	}
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrSquareColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrSquareColourFill::Render( RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	'Renders' a Square Fill Colour attribute.

********************************************************************************************/

void AttrSquareColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}



/********************************************************************************************

> Node* AttrSquareColourFill::SimpleCopy() 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrSquareColourFill::SimpleCopy()
{
	AttrSquareColourFill* NodeCopy = new AttrSquareColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  



/********************************************************************************************

>	void AttrSquareColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrSquareColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrSquareColourFill::GetAttrNameID(void)  

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrSquareColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_SQUAREGRADFILL)); 
}                                  



/********************************************************************************************

>	void AttrSquareColourFill::GetDebugDetails(StringBase* Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrSquareColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nDiamond Graduated Fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrSquareColourFill::GetNodeSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrSquareColourFill::GetNodeSize() const 
{     
	return sizeof(AttrSquareColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrSquareColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the Square fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrSquareColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	// am I a multistage fill ??
	BOOL bMultistage = FALSE;

	ColourRamp * pRamp = GetColourRamp();
	INT32 * pRampColRefs = NULL;
	double * pPositions = NULL;
	ColRampItem * pItem = NULL;
	UINT32 NumRampItems = 0;

	if (pRamp)
	{
		// write out all the colour references
		if (pRamp->GetCount() > 0)
		{
			bMultistage = TRUE;
			NumRampItems = pRamp->GetCount();

			pRampColRefs = new INT32[NumRampItems];
			pPositions   = new double[NumRampItems];

			pItem = (ColRampItem *)pRamp->GetHead();

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (pItem)
				{
					pPositions[i]   = pItem->GetPosition();
					pRampColRefs[i] = pFilter->WriteRecord(&pItem->GetColour());
				}

				pItem = (ColRampItem *)pRamp->GetNext(pItem);
			}
		}
	}

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));
		
	if (ok)
	{
		if (!bMultistage)
		{
			CamelotFileRecord Rec(pFilter,TAG_SQUAREFILL,TAG_SQUAREFILL_SIZE);

			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
			if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
			if (ok) ok = pFilter->Write(&Rec);
		}
		else
		{
			CamelotFileRecord Rec(pFilter,TAG_SQUAREFILLMULTISTAGE,
				TAG_SQUAREFILLMULTISTAGE_SIZE);

			if (ok) ok = Rec.Init();
			if (ok) ok = Rec.WriteCoord(Value.StartPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint);
			if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
			if (ok) ok = Rec.WriteReference(StartColRef);
			if (ok) ok = Rec.WriteReference(EndColRef);

			// now, write out all the colour ramp items
			if (ok) ok = Rec.WriteUINT32(NumRampItems);

			for (UINT32 i = 0 ; i < NumRampItems; i++)
			{
				if (ok) ok = Rec.WriteDOUBLE(pPositions[i]);
				if (ok) ok = Rec.WriteReference(pRampColRefs[i]);
			}

			if (ok) ok = pFilter->Write(&Rec);
		}
	}

	if (pPositions)
		delete [] pPositions;

	if (pRampColRefs)
		delete [] pRampColRefs;

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrSquareColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the Square fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrSquareColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrSquareColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrSquareColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);

	BOOL ok = ((StartColRef != 0) && (EndColRef != 0));

	if (ok)
	{
		ColourRamp * pRamp = GetColourRamp();
		if (pRamp)
		if (pRamp->GetCount() > 0)
		{
			ok = pRamp->WriteColourDefinitions (pFilter);
		}
	}
	
	return (ok);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrSquareTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrSquareTranspFill::Render( RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	'Renders' a Square Fill Colour attribute.

********************************************************************************************/

void AttrSquareTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}



/********************************************************************************************
>	virtual BOOL AttrSquareTranspFill::NeedsTransparency() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called to determine whether transparency is needed to render properly.
	Errors:		-

********************************************************************************************/

BOOL AttrSquareTranspFill::NeedsTransparency() const
{
	AttrSquareTranspFill* pNonConst = (AttrSquareTranspFill*) this;
	return ( 	pNonConst->GetTranspType() 		!= TT_Mix ||
				*(pNonConst->GetStartTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp()) 	!= 0 );
}



/********************************************************************************************

> Node* AttrSquareTranspFill::SimpleCopy() 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrSquareTranspFill::SimpleCopy()
{
	AttrSquareTranspFill* NodeCopy = new AttrSquareTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  



/********************************************************************************************

>	void AttrSquareTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrSquareTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrSquareTranspFill::GetAttrNameID(void)  

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrSquareTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_SQUARETRANSPFILL));
}                                  



/********************************************************************************************

>	void AttrSquareTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrSquareTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nSquare Graduated Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrSquareTranspFill::GetNodeSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrSquareTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrSquareTranspFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrSquareTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the Square transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrSquareTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Start transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp  > 255,"End transparency level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_SQUARETRANSPARENTFILL,TAG_SQUARETRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrSquareTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the Square transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrSquareTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}





/********************************************************************************************
>	BOOL AttrSquareTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrSquareTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrThreeColFill
//
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	void AttrThreeColFill::TransformSelectedControlPoints( TransformBase& Trans)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the selected control points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrThreeColFill::TransformSelectedControlPoints( TransformBase& Trans, BOOL* isARampBlob /*= NULL*/)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ( Trans.TransFills )
	{
		ClickModifiers ClickMods;
		ClickMods.Adjust = FALSE;		// Force adjust off

		// If the start point is selected then move the whole mesh
		if (IsSelected(FILLCONTROL_STARTPOINT))
		{
			DocCoord Pos = *GetStartPoint();
			Trans.Transform( &Pos, 1);

			FillControl Start = FILLCONTROL_STARTPOINT;

			OnControlDrag(Pos, Start, ClickMods);
		}
		else
		{
			// Otherwise move the fill points
			if (IsSelected(FILLCONTROL_ENDPOINT))
			{
				DocCoord Pos = *GetEndPoint();
				Trans.Transform( &Pos, 1);

				FillControl End = FILLCONTROL_ENDPOINT;

				OnControlDrag(Pos, End, ClickMods);
			}

			if (IsSelected(FILLCONTROL_ENDPOINT2))
			{
				DocCoord Pos = *GetEndPoint2();
				Trans.Transform( &Pos, 1);

				FillControl End2 = FILLCONTROL_ENDPOINT2;

				OnControlDrag(Pos, End2, ClickMods);
			}
		}
	}
#endif
}


/********************************************************************************************

>	virtual FillControl AttrThreeColFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		ColDrop - pointer to the 
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrThreeColFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	FillControl ControlHit = CheckForControlHit(DropPoint);
	
	// If it hit one of our control points, then use that
	if (ControlHit != FILLCONTROL_NULL)
		return ControlHit;

	// It didn't hit any of our control points, so if the drop is over
	// the object then we'll make a guess as to which control point
	// the user would like to change, depending on which area of the
	// object the pointer is over.

	// First make sure we're actually over an object
	NodeRenderableInk* pParentObject = ColDrop->GetObjectDroppedOn();
	if (pParentObject == NULL)
		return FILLCONTROL_NULL;	// We're not over any object 

	// Make sure this fill type has some Control Points
	if (GetStartPoint() == NULL || GetEndPoint() == NULL || GetEndPoint2() == NULL)
		return FILLCONTROL_NULL;

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint 	= *GetEndPoint();
	DocCoord EndPoint2 	= *GetEndPoint2();

	double StartDist = DropPoint.Distance(StartPoint);
	double EndDist = DropPoint.Distance(EndPoint);
	double End2Dist = DropPoint.Distance(EndPoint2);
	
	// FInd which point is closest
	
	if (StartDist < End2Dist)
	{
		if (StartDist < EndDist)
			ControlHit = FILLCONTROL_STARTPOINT;
		else
			ControlHit = FILLCONTROL_ENDPOINT;
	}
	else
	{
		if (EndDist < End2Dist)
			ControlHit = FILLCONTROL_ENDPOINT;
		else
			ControlHit = FILLCONTROL_ENDPOINT2;
	}

	return ControlHit;
#else
	return FILLCONTROL_NULL;
#endif
}



/********************************************************************************************

>	void AttrThreeColFill::Transform( TransformBase& Trans )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	Copied from AttrLinearFill

********************************************************************************************/

void AttrThreeColFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);

		if (IsPerspective())
			Trans.Transform( GetEndPoint3(), 1);
	}
}



/********************************************************************************************

>	BOOL AttrThreeColFill::CanTransform()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	Copied from AttrLinearFill

********************************************************************************************/

BOOL AttrThreeColFill::CanTransform()
{
	return TRUE;
}



/********************************************************************************************

>	void AttrThreeColFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrThreeColFill::RenderFillBlobs

********************************************************************************************/

void AttrThreeColFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];
	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_ENDPOINT2];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, StartPoint, EndPoint, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, StartPoint, EndPoint2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(StartPoint, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(StartPoint, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		// If we are being dragged then draw a line accros the end of the triangle
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_NONE);
		pRender->DrawLine(EndPoint, EndPoint2);
	}

	// Now Render the blobs on the path
	// Set the line colour to none
	pRender->SetLineColour(COLOUR_NONE);

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(StartPoint, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(StartPoint, BT_UNSELECTED);
	}

	// Draw the blobs at the end points
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blobs
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(EndPoint, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blobs
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(EndPoint, BT_UNSELECTED);
	}

	// Draw the blobs at the second end point
	if (SelState[FILLCONTROL_ENDPOINT2])
	{
		// Draw Selected Blobs
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(EndPoint2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blobs
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(EndPoint2,BT_UNSELECTED);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}



/********************************************************************************************

>	virtual void AttrThreeColFill::OnControlDrag(DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a control point.

********************************************************************************************/

void AttrThreeColFill::OnControlDrag(DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	INT32 dx, dy;

	// Which control point is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:
		
        	// Someone is dragging the Centre of the Fill
			dx = StartPoint.x - Pos.x;
			dy = StartPoint.y - Pos.y;
			// Move the other points relative
			EndPoint.translate(-dx, -dy);
			EndPoint2.translate(-dx, -dy);

			SetEndPoint(&EndPoint);
			SetEndPoint2(&EndPoint2);
			SetStartPoint(&Pos);
			break;

		case FILLCONTROL_ENDPOINT:
			
			// Someone is dragging the first End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The mesh can be locked as an equilateral triangle by the Shift key
			if (ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, 60, INT32(StartPoint.Distance(EndPoint2) * Ratio));
				SetEndPoint2(&temp);
			}

			SetEndPoint(&Pos);
			break;

		case FILLCONTROL_ENDPOINT2:

			// Someone is dragging the second End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The mesh can be locked as an equilateral triangle by the Shift key
			if (ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint2);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, -60, INT32(StartPoint.Distance(EndPoint) * Ratio));
				SetEndPoint(&temp);
			}

			SetEndPoint2(&Pos);
			break;
	}
#endif
}



/********************************************************************************************

>	virtual DocRect AttrThreeColFill::GetBlobBoundingRect()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrThreeColFill::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	// Get the Start and End Points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	// Make a dummy bounds from just the Start Point
	DocRect BoundingRect(StartPoint, StartPoint);

	// We're not being dragged, so just calc the bounds of the Start and End Blobs
	DocRect BlobRect;

	// Get the Bounding rect of the Blob and include the Bottom Left and
	// Top Right of each blob in the Bounds.
	// We have to do it like this to make sure that the DocRect's coords
	// are valid.  ie. The Hi's are Higher than the Lo's.
	(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint);
	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint2);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0,0,0);
#endif
}


/********************************************************************************************

>	virtual void AttrThreeColFill::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		Control, the control to render
				RenderOn, TRUE if rendering blob on FALSE if off
	Purpose:	Renders a specified control
	SeeAlso:	FillControl

********************************************************************************************/

void AttrThreeColFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:
			if (GetStartPoint() != NULL)
			{
				// Redraw the Start Point Blob
				(Camelot.GetBlobManager())->GetBlobRect(*GetStartPoint(), &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT:
			if (GetEndPoint() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint(), &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT2:
			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint2(), &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;
	}
#endif
}

	
/********************************************************************************************

>	virtual void AttrThreeColFill::CycleSelection(BOOL Reverse)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Purpose:	Cycles the selection state of the controls

********************************************************************************************/

void AttrThreeColFill::CycleSelection(BOOL Reverse)
{
	if (GetSelectionCount() == 1)
	{
		if (SelectionState[FILLCONTROL_STARTPOINT])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_ENDPOINT2);
			else
				SelectBlob(FILLCONTROL_ENDPOINT);
		}
		else if (SelectionState[FILLCONTROL_ENDPOINT])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_STARTPOINT);
			else
				SelectBlob(FILLCONTROL_ENDPOINT2);
		}
		else if (SelectionState[FILLCONTROL_ENDPOINT2])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_ENDPOINT);
			else
				SelectBlob(FILLCONTROL_STARTPOINT);
		}
	}
}


/********************************************************************************************

>	FillControl AttrThreeColFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrThreeColFill::CheckForControlHit(DocCoord &ClickPos)
{
	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect BlobRect;

	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;


	// Get the rectangle around the Start Control Point
	(Camelot.GetBlobManager())->GetBlobRect(*GetStartPoint(), &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the End Control Point
	(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint(), &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	// Get the rectangle around the Secondary Control Point
	(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint2(), &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT2;
#endif
	return HitControl;
}




/********************************************************************************************

>	void AttrThreeColFill::ValidateAttributeValue()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrThreeColFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0))
	{
		// If the EndPoint2 is 'NULL' then make it sensible
		if (*GetEndPoint2()	== DocCoord(0,0))
		{
			DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90);
			SetEndPoint2(&temp);
		}
		
		return;
	}

	// Make up some sensible defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	// If the StartPoint is 'NULL' then make all points sensible
	if ((*GetStartPoint()) == DocCoord(0,0))
	{
		// Start in the centre of the bounds
		SetStartPoint(&AttrBounds.lo);

		// and set End Points to Middle Right, and Middle Top
		DocCoord temp = DocCoord(AttrBounds.hi.x, AttrBounds.lo.y);
		SetEndPoint(&temp);
		temp = DocCoord(AttrBounds.lo.x, AttrBounds.hi.y);
		SetEndPoint2(&temp);
	}

	// If the EndPoint is 'NULL' then make it sensible
	if ((*GetEndPoint()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord(AttrBounds.hi.x, AttrBounds.lo.y);
		SetEndPoint(&temp);
	}

	if ((*GetEndPoint2()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord(AttrBounds.lo.x, AttrBounds.hi.y);
		SetEndPoint2(&temp);
	}
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrThreeColColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrThreeColColourFill::Render( RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	'Renders' a ThreeCol Fill Colour attribute.

********************************************************************************************/

void AttrThreeColColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}



/********************************************************************************************

> Node* AttrThreeColColourFill::SimpleCopy() 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrThreeColColourFill::SimpleCopy()
{
	AttrThreeColColourFill* NodeCopy = new AttrThreeColColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  



/********************************************************************************************

>	void AttrThreeColColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrThreeColColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[3];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_ENDPOINT2] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 3);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrThreeColColourFill::GetAttrNameID(void)  

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrThreeColColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_THREECOLGRADFILL)); 
}                                  



/********************************************************************************************

>	void AttrThreeColColourFill::GetDebugDetails(StringBase* Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrThreeColColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nDiamond Graduated Fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrThreeColColourFill::GetNodeSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrThreeColColourFill::GetNodeSize() const 
{     
	return sizeof(AttrThreeColColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrThreeColColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the ThreeCol fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrThreeColColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);
	INT32 EndCol2Ref	 = pFilter->WriteRecord(&Value.EndColour2);

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0) && (EndCol2Ref != 0));
		
	if (ok)
	{
		CamelotFileRecord Rec(pFilter,TAG_THREECOLFILL,TAG_THREECOLFILL_SIZE);

		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Value.StartPoint);
		if (ok) ok = Rec.WriteCoord(Value.EndPoint);
		if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
		if (ok) ok = Rec.WriteReference(StartColRef);
		if (ok) ok = Rec.WriteReference(EndColRef);
		if (ok) ok = Rec.WriteReference(EndCol2Ref);
		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrThreeColColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the ThreeCol fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrThreeColColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrThreeColColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrThreeColColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);
	INT32 EndCol2Ref	 = pFilter->WriteRecord(&Value.EndColour2);

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0) && (EndCol2Ref != 0));
	
	return (ok);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrThreeColTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrThreeColTranspFill::Render( RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	'Renders' a ThreeCol Fill Colour attribute.

********************************************************************************************/

void AttrThreeColTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}



/********************************************************************************************
>	virtual BOOL AttrThreeColTranspFill::NeedsTransparency() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called to determine whether transparency is needed to render properly.
	Errors:		-

********************************************************************************************/

BOOL AttrThreeColTranspFill::NeedsTransparency() const
{
	AttrThreeColTranspFill* pNonConst = (AttrThreeColTranspFill*) this;
	return ( 	pNonConst->GetTranspType() 		!= TT_Mix ||
				*(pNonConst->GetStartTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp2()) 	!= 0 );
}



/********************************************************************************************

> Node* AttrThreeColTranspFill::SimpleCopy() 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrThreeColTranspFill::SimpleCopy()
{
	AttrThreeColTranspFill* NodeCopy = new AttrThreeColTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  



/********************************************************************************************

>	void AttrThreeColTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrThreeColTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[3];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_ENDPOINT2] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 3);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrThreeColTranspFill::GetAttrNameID(void)  

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrThreeColTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_THREECOLTRANSPFILL)); 
}                                  



/********************************************************************************************

>	void AttrThreeColTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrThreeColTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nThreeCol Graduated Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrThreeColTranspFill::GetNodeSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrThreeColTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrThreeColTranspFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrThreeColTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the ThreeCol transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrThreeColTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Start transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp  > 255,"End transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp2 > 255,"End transparency2 level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_THREECOLTRANSPARENTFILL,TAG_THREECOLTRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp2));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrThreeColTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the ThreeCol transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrThreeColTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}




/********************************************************************************************
>	BOOL AttrThreeColTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrThreeColTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0 && Value.EndTransp2==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFourColFill
//
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	void AttrFourColFill::TransformSelectedControlPoints( TransformBase& Trans)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/8/96
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the selected control points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrFourColFill::TransformSelectedControlPoints( TransformBase& Trans, BOOL* isARampBlob /*= NULL*/)
{
#if !defined(EXCLUDE_FROM_RALPH)
	TRACE( _T("TransformSelected\n"));
	
	if ( Trans.TransFills )
	{
		ClickModifiers ClickMods;
		ClickMods.Adjust = FALSE;		// Force adjust off

		// If the start point is selected then just move the fill
		if (IsSelected(FILLCONTROL_STARTPOINT))
		{
			DocCoord Pos = *GetStartPoint();
			Trans.Transform( &Pos, 1);

			FillControl Start = FILLCONTROL_STARTPOINT;

			OnControlDrag(Pos, Start, ClickMods);
		}
		else
		{
			if (IsSelected(FILLCONTROL_ENDPOINT3))
			{
				DocCoord Pos = *GetEndPoint2() + *GetEndPoint() - *GetStartPoint();
				Trans.Transform( &Pos, 1);

				FillControl End3 = FILLCONTROL_ENDPOINT3;

				OnControlDrag(Pos, End3, ClickMods);
			}
			else
			{
				if (IsSelected(FILLCONTROL_ENDPOINT))
				{
					DocCoord Pos = *GetEndPoint();
					Trans.Transform( &Pos, 1);

					FillControl End = FILLCONTROL_ENDPOINT;

					OnControlDrag(Pos, End, ClickMods);
				}

				if (IsSelected(FILLCONTROL_ENDPOINT2))
				{
					DocCoord Pos = *GetEndPoint2();
					Trans.Transform( &Pos, 1);

					FillControl End2 = FILLCONTROL_ENDPOINT2;

					OnControlDrag(Pos, End2, ClickMods);
				}
			}	
		}
	}
#endif
}


/********************************************************************************************

>	virtual FillControl AttrFourColFill::TestColourDrop(AttrColourDrop* ColDrop) 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		ColDrop - pointer to the 
	Purpose:	Check to see which colour will be changed if dropped at this point

********************************************************************************************/

FillControl AttrFourColFill::TestColourDrop(AttrColourDrop* ColDrop) 
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// So, where was it dropped (or where will it be dropped)
	DocCoord DropPoint = ColDrop->GetDropPoint();

	// Look to see if the DropPoint is over any of the Fill Control Points
	FillControl ControlHit = CheckForControlHit(DropPoint);
	
	// If it hit one of our control points, then use that
	if (ControlHit != FILLCONTROL_NULL)
		return ControlHit;

	// It didn't hit any of our control points, so if the drop is over
	// the object then we'll make a guess as to which control point
	// the user would like to change, depending on which area of the
	// object the pointer is over.

	// First make sure we're actually over an object
	NodeRenderableInk* pParentObject = ColDrop->GetObjectDroppedOn();
	if (pParentObject == NULL)
		return FILLCONTROL_NULL;	// We're not over any object 

	// Make sure this fill type has some Control Points
	if (GetStartPoint() == NULL || GetEndPoint() == NULL || GetEndPoint2() == NULL)
		return FILLCONTROL_NULL;

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint 	= *GetEndPoint();
	DocCoord EndPoint2 	= *GetEndPoint2();
	DocCoord EndPoint3 = EndPoint2 + EndPoint - StartPoint;

	double StartDist = DropPoint.Distance(StartPoint);
	double EndDist = DropPoint.Distance(EndPoint);
	double End2Dist = DropPoint.Distance(EndPoint2);
	double End3Dist = DropPoint.Distance(EndPoint3);
	
	// FInd which point is closest
	
	if (StartDist < End3Dist)
	{
		// Start is smallest so far
		if (StartDist < End2Dist)
		{
			// Start is still smallest
			if (StartDist < EndDist)
				ControlHit = FILLCONTROL_STARTPOINT;
			else
				ControlHit = FILLCONTROL_ENDPOINT;
		}
		else
		{
			// End2 is smallest so far
			if (EndDist < End2Dist)
				ControlHit = FILLCONTROL_ENDPOINT;
			else
				ControlHit = FILLCONTROL_ENDPOINT2;
		}
	}
	else
	{
		// End3 is smallest so far
		if (End3Dist < End2Dist)
		{
			// End3 is still smallest
			if (End3Dist < EndDist)
				ControlHit = FILLCONTROL_ENDPOINT3;
			else
				ControlHit = FILLCONTROL_ENDPOINT;
		}
		else
		{
			// End2 is smallest so far
			if (End2Dist < EndDist)
				ControlHit = FILLCONTROL_ENDPOINT2;
			else
				ControlHit = FILLCONTROL_ENDPOINT;
		}
	}

	return ControlHit;
#else
	return FILLCONTROL_NULL;
#endif
}



/********************************************************************************************

>	void AttrFourColFill::Transform( TransformBase& Trans )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	Copied from AttrLinearFill

********************************************************************************************/

void AttrFourColFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);

		if (IsPerspective())
			Trans.Transform( GetEndPoint3(), 1);
	}
}



/********************************************************************************************

>	BOOL AttrFourColFill::CanTransform()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	Copied from AttrLinearFill

********************************************************************************************/

BOOL AttrFourColFill::CanTransform()
{
	return TRUE;
}



/********************************************************************************************

>	void AttrFourColFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrFourColFill::RenderFillBlobs

********************************************************************************************/

void AttrFourColFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];
	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_ENDPOINT2];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i = 0; i <= FILLCONTROL_ENDPOINT3; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint3 = EndPoint2 + EndPoint - StartPoint;

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, StartPoint, EndPoint, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, StartPoint, EndPoint2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(StartPoint, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(StartPoint, LineEnd2);

		// Draw a line from EndPoint to EndPoint3
	pRender->DrawLine(EndPoint, EndPoint3);

	// Draw a line from EndPoint2 to EndPoint3
	pRender->DrawLine(EndPoint2, EndPoint3);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);
	
	// Now Render the blobs on the path
	// Set the line colour to none
	pRender->SetLineColour(COLOUR_NONE);

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(StartPoint, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(StartPoint, BT_UNSELECTED);
	}

	// Draw the blobs at the end points
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blobs
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(EndPoint, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blobs
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(EndPoint, BT_UNSELECTED);
	}

	// Draw the blobs at the end points
	if (SelState[FILLCONTROL_ENDPOINT2])
	{
		// Draw Selected Blobs
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(EndPoint2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blobs
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(EndPoint2,BT_UNSELECTED);
	}

	// Draw the blobs at the end points
	if (SelState[FILLCONTROL_ENDPOINT3])
	{
		// Draw Selected Blobs
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(EndPoint3,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blobs
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(EndPoint3,BT_UNSELECTED);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}



/********************************************************************************************

>	virtual void AttrFourColFill::OnControlDrag(DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a control point.

********************************************************************************************/

void AttrFourColFill::OnControlDrag(DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	INT32 dx, dy;

	// Which control point is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:
		
        	// Someone is dragging the Centre of the Fill
			dx = StartPoint.x - Pos.x;
			dy = StartPoint.y - Pos.y;
			// Move the other points relative
			EndPoint.translate(-dx, -dy);
			EndPoint2.translate(-dx, -dy);

			SetEndPoint(&EndPoint);
			SetEndPoint2(&EndPoint2);
			SetStartPoint(&Pos);
			break;

		case FILLCONTROL_ENDPOINT:
			
			// Someone is dragging the first End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The Aspect ratio can be locked either by it being circular
			// or by the Shift key		
			if (ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point based on the aspect ratio
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, 90, INT32(StartPoint.Distance(EndPoint2) * Ratio));
				SetEndPoint2(&temp);
			}

			SetEndPoint(&Pos);
			break;

		case FILLCONTROL_ENDPOINT2:

			// Someone is dragging the second End Point
			
			// Constrain the angle if necessary
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(StartPoint, &Pos);

			// The Aspect ratio can be locked either by it being circular
			// or by the Shift key		
			if (ClickMods.Adjust)
			{
				double OldLen = StartPoint.Distance(EndPoint2);
				double NewLen = StartPoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new end point based on the aspect ratio
				DocCoord temp = MakeLineAtAngle(StartPoint, Pos, -90, INT32(StartPoint.Distance(EndPoint) * Ratio));
				SetEndPoint(&temp);
			}

			SetEndPoint2(&Pos);
			break;

		case FILLCONTROL_ENDPOINT3:
			DocCoord P = EndPoint - StartPoint;
			DocCoord Q = EndPoint2 - StartPoint;
			DocCoord N = Pos - StartPoint;
			DocCoord NewEndPoint = EndPoint;
			DocCoord NewEndPoint2 = EndPoint2;

			// If shift is down or both endpoints are the same
						
			if ((Pos.Distance(StartPoint) > 512) &&
				(Pos.Distance(EndPoint) > 512) &&
				(Pos.Distance(EndPoint2) > 512) )
			{
				if (ClickMods.Adjust || (P == Q))
				{
					NewEndPoint = DocCoord(Pos.x, StartPoint.y);
					NewEndPoint2 = DocCoord(StartPoint.x, Pos.y);
				}
				else
				{
					// We are now going to move the two endpoints such that the gradient
					// of all the edges remains constant
					
					double px = P.x;
					double py = P.y;
					double qx = Q.x;
					double qy = Q.y;
					double nx = N.x;
					double ny = N.y;
					
					double den = (px * qy) - (py * qx);

					ERROR3IF(floor(den) != den, "Non integer denominator");

					if (den != 0.0)
					{
						double u = ((px * ny) - (py * nx)) / den;

						NewEndPoint.x = Pos.x - (INT32) (u * qx);
						NewEndPoint.y = Pos.y - (INT32) (u * qy);
						NewEndPoint2.x = StartPoint.x + (INT32) (u * qx);
						NewEndPoint2.y = StartPoint.y + (INT32) (u * qy);
					}
					else
					{
						NewEndPoint = DocCoord(Pos.x, StartPoint.y);
						NewEndPoint2 = DocCoord(StartPoint.x, Pos.y);
					}
				}

				// Don't let the points get too close
				if ((StartPoint.Distance(NewEndPoint) > 512) &&
					(StartPoint.Distance(NewEndPoint2) > 512) )
				{
					SetEndPoint(&NewEndPoint);
					SetEndPoint2(&NewEndPoint2);
				}
			}
			
			break;
	}
#endif
}



/********************************************************************************************

>	virtual DocRect AttrFourColFill::GetBlobBoundingRect()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrFourColFill::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	// Get the control points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();
	DocCoord EndPoint3 = EndPoint2 + EndPoint - StartPoint;

	// Make a dummy bounds from just the Start Point
	DocRect BoundingRect(StartPoint, StartPoint);

	// We're not being dragged, so just calc the bounds of the Start and End Blobs
	DocRect BlobRect;

	// Get the Bounding rect of the Blob and include the Bottom Left and
	// Top Right of each blob in the Bounds.
	// We have to do it like this to make sure that the DocRect's coords
	// are valid.  ie. The Hi's are Higher than the Lo's.
	(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	(Camelot.GetBlobManager())->GetBlobRect(EndPoint3, &BlobRect);
	BoundingRect.IncludePoint(BlobRect.lo);
	BoundingRect.IncludePoint(BlobRect.hi);

	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint);
	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint2);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0,0,0);
#endif
}


/********************************************************************************************

>	virtual void AttrFourColFill::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		Control, the control to render
				RenderOn, TRUE if rendering blob on FALSE if off
	Purpose:	Renders a specified control
	SeeAlso:	FillControl

********************************************************************************************/

void AttrFourColFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:
			// Redraw the Start Point Blob
			(Camelot.GetBlobManager())->GetBlobRect(*GetStartPoint(), &ControlRect);
			RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			break;

		case FILLCONTROL_ENDPOINT:
			(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint(), &ControlRect);
			RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			break;

		case FILLCONTROL_ENDPOINT2:
			(Camelot.GetBlobManager())->GetBlobRect(*GetEndPoint2(), &ControlRect);
			RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			break;

		case FILLCONTROL_ENDPOINT3:
			DocCoord EndPoint3 = *GetEndPoint2() + *GetEndPoint() - *GetStartPoint();
 			(Camelot.GetBlobManager())->GetBlobRect(EndPoint3, &ControlRect);
			RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			break;
	}
#endif
}

	
/********************************************************************************************

>	virtual void AttrFourColFill::CycleSelection(BOOL Reverse)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Purpose:	Cycles the selection state of the controls
	SeeAlso:	FillControl

********************************************************************************************/

void AttrFourColFill::CycleSelection(BOOL Reverse)
{
	if (GetSelectionCount() == 1)
	{
		if (SelectionState[FILLCONTROL_STARTPOINT])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_ENDPOINT3);
			else
				SelectBlob(FILLCONTROL_ENDPOINT);
		}
		else if (SelectionState[FILLCONTROL_ENDPOINT])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_STARTPOINT);
			else
				SelectBlob(FILLCONTROL_ENDPOINT2);
		}
		else if (SelectionState[FILLCONTROL_ENDPOINT2])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_ENDPOINT);
			else
				SelectBlob(FILLCONTROL_ENDPOINT3);
		}
		else if (SelectionState[FILLCONTROL_ENDPOINT3])
		{
			DeselectAll();
			if (Reverse)
				SelectBlob(FILLCONTROL_ENDPOINT2);
			else
				SelectBlob(FILLCONTROL_STARTPOINT);
		}
	}
}


/********************************************************************************************

>	FillControl AttrFourColFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrFourColFill::CheckForControlHit(DocCoord &ClickPos)
{
	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect BlobRect;

	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	// Get the control points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();
	DocCoord EndPoint3 = EndPoint2 + EndPoint - StartPoint;

	// Get the rectangle around the Start Control Point
	(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the End Control Point
	(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	// Get the rectangle around the Secondary Control Point
	(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT2;

	// Get the rectangle around the Secondary Control Point
	(Camelot.GetBlobManager())->GetBlobRect(EndPoint3, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT3;
#endif
	return HitControl;
}


/********************************************************************************************

>	void AttrFourColFill::ValidateAttributeValue()

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	Makes sure the Coords of the Fill are sensible.

********************************************************************************************/

void AttrFourColFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0))
	{
		// If the EndPoint2 is 'NULL' then make it sensible
		if (*GetEndPoint2()	== DocCoord(0,0))
		{
			DocCoord temp = MakeLineAtAngle((*GetStartPoint()), (*GetEndPoint()), 90);
			SetEndPoint2(&temp);
		}
		
		return;
	}

	// Make up some sensible defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	// If the StartPoint is 'NULL' then make all points sensible
	if ((*GetStartPoint()) == DocCoord(0,0))
	{
		// Start in the centre of the bounds
		SetStartPoint(&AttrBounds.lo);

		// and set End Points to Middle Right, and Middle Top
		DocCoord temp = DocCoord(AttrBounds.hi.x, AttrBounds.lo.y);
		SetEndPoint(&temp);
		temp = DocCoord(AttrBounds.lo.x, AttrBounds.hi.y);
		SetEndPoint2(&temp);
	}

	// If EndPoint is 'NULL' then make it sensible
	if ((*GetEndPoint()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord(AttrBounds.hi.x, AttrBounds.lo.y);
		SetEndPoint(&temp);
	}

	// If EndPoint2 is 'NULL' then make it sensible
	if ((*GetEndPoint2()) == DocCoord(0,0))
	{
		DocCoord temp = DocCoord(AttrBounds.lo.x, AttrBounds.hi.y);
		SetEndPoint2(&temp);
	}
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFourColColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrFourColColourFill::Render( RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	'Renders' a FourCol Fill Colour attribute.

********************************************************************************************/

void AttrFourColColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}



/********************************************************************************************

> Node* AttrFourColColourFill::SimpleCopy() 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFourColColourFill::SimpleCopy()
{
	AttrFourColColourFill* NodeCopy = new AttrFourColColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  



/********************************************************************************************

>	void AttrFourColColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrFourColColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	// Get the control points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();
	DocCoord EndPoint3 = EndPoint2 + EndPoint - StartPoint;

	DocCoord ControlPoints[4];
	ControlPoints[FILLCONTROL_STARTPOINT] = StartPoint;
	ControlPoints[FILLCONTROL_ENDPOINT] = EndPoint;
	ControlPoints[FILLCONTROL_ENDPOINT2] = EndPoint2;
	ControlPoints[FILLCONTROL_ENDPOINT3] = EndPoint3;

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 4);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrFourColColourFill::GetAttrNameID(void)  

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFourColColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_FOURCOLGRADFILL)); 
}                                  



/********************************************************************************************

>	void AttrFourColColourFill::GetDebugDetails(StringBase* Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrFourColColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nFour colour graduated fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd2"));
//	(*GetEndColour2()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd3"));
//	(*GetEndColour3()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrFourColColourFill::GetNodeSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFourColColourFill::GetNodeSize() const 
{     
	return sizeof(AttrFourColColourFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrFourColColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the FourCol fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFourColColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);
	INT32 EndCol2Ref	 = pFilter->WriteRecord(&Value.EndColour2);
	INT32 EndCol3Ref	 = pFilter->WriteRecord(&Value.EndColour3);

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0) && (EndCol2Ref != 0) && (EndCol3Ref != 0));
		
	if (ok)
	{
		CamelotFileRecord Rec(pFilter,TAG_FOURCOLFILL,TAG_FOURCOLFILL_SIZE);

		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Value.StartPoint);
		if (ok) ok = Rec.WriteCoord(Value.EndPoint);
		if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
		if (ok) ok = Rec.WriteReference(StartColRef);
		if (ok) ok = Rec.WriteReference(EndColRef);
		if (ok) ok = Rec.WriteReference(EndCol2Ref);
		if (ok) ok = Rec.WriteReference(EndCol3Ref);
		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrFourColColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the FourCol fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFourColColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrFourColColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrFourColColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Must write out the colours first
	INT32 StartColRef = pFilter->WriteRecord(&Value.Colour);
	INT32 EndColRef	 = pFilter->WriteRecord(&Value.EndColour);
	INT32 EndCol2Ref	 = pFilter->WriteRecord(&Value.EndColour2);
	INT32 EndCol3Ref	 = pFilter->WriteRecord(&Value.EndColour3);

	// Are the colour references ok?
	BOOL ok = ((StartColRef != 0) && (EndColRef != 0) && (EndCol2Ref != 0) && (EndCol3Ref != 0));
	
	return (ok);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFourColTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrFourColTranspFill::Render( RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Purpose:	'Renders' a FourCol Fill Colour attribute.

********************************************************************************************/

void AttrFourColTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}



/********************************************************************************************
>	virtual BOOL AttrFourColTranspFill::NeedsTransparency() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called to determine whether transparency is needed to render properly.
	Errors:		-

********************************************************************************************/

BOOL AttrFourColTranspFill::NeedsTransparency() const
{
	AttrFourColTranspFill* pNonConst = (AttrFourColTranspFill*) this;
	return ( 	pNonConst->GetTranspType() 		!= TT_Mix || 
				*(pNonConst->GetStartTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp2()) 	!= 0 ||
				*(pNonConst->GetEndTransp3()) 	!= 0 );
}



/********************************************************************************************

> Node* AttrFourColTranspFill::SimpleCopy() 

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFourColTranspFill::SimpleCopy()
{
	AttrFourColTranspFill* NodeCopy = new AttrFourColTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  



/********************************************************************************************

>	void AttrFourColTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrFourColTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	// Get the control points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();
	DocCoord EndPoint3 = EndPoint2 + EndPoint - StartPoint;

	DocCoord ControlPoints[4];
	ControlPoints[FILLCONTROL_STARTPOINT] = StartPoint;
	ControlPoints[FILLCONTROL_ENDPOINT] = EndPoint;
	ControlPoints[FILLCONTROL_ENDPOINT2] = EndPoint2;
	ControlPoints[FILLCONTROL_ENDPOINT3] = EndPoint3;

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 4);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrFourColTranspFill::GetAttrNameID(void)  

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFourColTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_FOURCOLTRANSPFILL)); 
}                                  



/********************************************************************************************

>	void AttrFourColTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrFourColTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nFourCol Graduated Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	virtual UINT32 AttrFourColTranspFill::GetNodeSize() const

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFourColTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrFourColTranspFill);
}  



/********************************************************************************************

  >	virtual BOOL AttrFourColTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the FourCol transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFourColTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");
	ERROR3IF(Value.Transp     > 255,"Start transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp  > 255,"End transparency level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp2 > 255,"End transparency2 level is too high to be stored as a byte");
	ERROR3IF(Value.EndTransp3 > 255,"End transparency3 level is too high to be stored as a byte");
	ERROR3IF(Value.TranspType > 255,"Transparency type is too high to be stored as a byte");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_FOURCOLTRANSPARENTFILL,TAG_FOURCOLTRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp2));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp3));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrFourColTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pFilter - ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the FourCol transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFourColTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}




/********************************************************************************************
>	BOOL AttrFourColTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrFourColTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0 && Value.EndTransp2==0 && Value.EndTransp3==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrBitmapFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************************************

>    virtual BOOL AttrBitmapFill::HidingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   11/1/94
	 Returns:   TRUE if all was ok. FALSE if an error occured.
     Purpose:   This virtual function is called whenever the node is hidden.
	 			It allows the node do things like 'optimise' itself to use less memory or
	 			send a message to let others know it is being hidden etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL AttrBitmapFill::HidingNode()
{
	// Call the base class first
	if (!AttrFillGeometry::HidingNode())
		return FALSE;

	GetBitmapRef()->RemoveFromTree();

	return TRUE;
}

/***********************************************************************************************

>    virtual BOOL AttrBitmapFill::ShowingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   11/1/94
	 Returns:   TRUE if all was ok. FALSE if an error occured (eg Out of memory).
     Purpose:   This virtual function is called whenever the node is re-shown after being 
     			Hidden.
	 			It allows the node to reconstruct itself if it was optimised or
	 			send a message to let others know it is back etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL AttrBitmapFill::ShowingNode()
{
	// Call the base class first
	if (!AttrFillGeometry::ShowingNode())
		return FALSE;

	GetBitmapRef()->AddtoTree();

	return TRUE;
}

/********************************************************************************************

>	BOOL AttrBitmapFill::SetDPI(UINT32 NewDpi)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/11/94
	Inputs:		NewDpi - the new Dpi for this bitmap fill.
	Purpose:	Changes the Dpi of this bitmap by scaling the fill control points.

********************************************************************************************/
/*
BOOL AttrBitmapFill::SetDPI(UINT32 NewDpi)
{
	if (NewDpi == 0)
		return FALSE;

	if (GetBitmap() == NULL)
		return FALSE;

	OILBitmap *OilBM = GetBitmap()->ActualBitmap;

	if (IsAFractalFill())
	{
		((FillGeometryAttribute*)GetAttributeValue())->SetDPI(NewDpi);
	}

	if (OilBM == NULL)
		return FALSE;

	BitmapInfo Info;
	OilBM->GetInfo(&Info);

	INT32 PixWidth  = Info.PixelWidth;
	INT32 PixHeight = Info.PixelHeight;

	DocCoord Start = *GetStartPoint();
	DocCoord End   = *GetEndPoint();
	DocCoord End2  = *GetEndPoint2();

	INT32 Width  = INT32(Start.Distance(End));
	INT32 Height = INT32(Start.Distance(End2));

	INT32 HDpi = (PixWidth*72000)/Width;
	INT32 VDpi = (PixHeight*72000)/Height;

	INT32 OldDpi = HDpi;
	if (VDpi < OldDpi)
		OldDpi = VDpi;

	TRACEUSER( "Mike", _T("Bitmap Dpi is currently %d\n"),OldDpi);
	TRACEUSER( "Mike", _T("Setting Bitmap Dpi to %d\n"),NewDpi);

	FIXED16 Ratio 	= FIXED16(double(OldDpi)/double(NewDpi));
	Matrix Scale  	= Matrix(Ratio, Ratio);

	GetBitmapVirtualPoints(Start, End, End2,
							&Start, &End, &End2);

	DocCoord Centre = Start;

	Start.translate(-Centre.x, -Centre.y);	
	Scale.transform(&Start);
	Start.translate(Centre.x, Centre.y);	

	End.translate(-Centre.x, -Centre.y);	
	Scale.transform(&End);
	End.translate(Centre.x, Centre.y);	

	End2.translate(-Centre.x, -Centre.y);	
	Scale.transform(&End2);
	End2.translate(Centre.x, Centre.y);	

	GetBitmapRealPoints(Start, End, End2,
						&Start, &End, &End2);

	SetStartPoint(&Start);
	SetEndPoint(&End);
	SetEndPoint2(&End2);

	return TRUE;
}
*/

/********************************************************************************************

>	void AttrBitmapFill::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBitmapFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);
		Trans.Transform( GetEndPoint3(), 1);

//		ValidateAttributeValue();
	}
}

/********************************************************************************************

>	void AttrBitmapFill::TransformSelectedControlPoints( TransformBase& Trans)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the selected control points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrBitmapFill::TransformSelectedControlPoints( TransformBase& Trans, BOOL* isARampBlob /*= NULL*/)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ( Trans.TransFills )
	{
		ClickModifiers ClickMods; /* = ClickModifiers::GetClickModifiers();*/
		ClickMods.Adjust = TRUE;

		DocCoord StartPoint  = *GetStartPoint();
		DocCoord EndPoint    = *GetEndPoint();
		DocCoord EndPoint2   = *GetEndPoint2();

		DocCoord CentrePoint;
		DocCoord Drag2Blob;
		DocCoord Drag3Blob;

		GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
								&CentrePoint, &Drag2Blob, &Drag3Blob);

		if (GetStartPoint() && IsSelected(FILLCONTROL_STARTPOINT))
		{
			Trans.Transform( &CentrePoint, 1);

			FillControl Start = FILLCONTROL_STARTPOINT;
			
			OnControlDrag(CentrePoint, Start, ClickMods);
		}

		if (GetEndPoint() && IsSelected(FILLCONTROL_ENDPOINT))
		{
			Trans.Transform( &Drag2Blob, 1);

			FillControl End = FILLCONTROL_ENDPOINT;

			OnControlDrag(Drag2Blob, End, ClickMods);
		}
	}
#endif
}

/********************************************************************************************

>	BOOL AttrBitmapFill::CanTransform()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	NodeRenderable::CanTransform

********************************************************************************************/

BOOL AttrBitmapFill::CanTransform()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL AttrBitmapFill::CopyNodeContents( AttrBitmapFill* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrBitmapFill::CopyNodeContents( AttrBitmapFill* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrBitmapFill::CopyNodeContents!");

	AttrFillGeometry::CopyNodeContents( NodeCopy );

	// Copy contents specific to derived class here
	return TRUE;
} 



/********************************************************************************************

>	FillControl AttrBitmapFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrBitmapFill::CheckForControlHit(DocCoord &ClickPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
	DocRect BlobRect;

	DocCoord StartPoint  = *GetStartPoint();
	DocCoord EndPoint    = *GetEndPoint();
	DocCoord EndPoint2   = *GetEndPoint2();

	// Bodge Alert !!
	
	// Bitmap fills have a complicated control point system.
	// The actual control points needed for rendering are the Bottom Left,
	// Top Left, and Bottom Right points of the bitmap.  But on screen the 
	// points seen and moved are the Centre, Middle Right, and Middle Top.
	// So we need to convert the click points into vitual points before
	// we do the hit test.

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	// Get the rectangle around the Centre Control Point
	(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the First End Point
	(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	// Get the rectangle around the Second End Point
	(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_SECONDARYPOINT;

	return HitControl;
#else
	return FILLCONTROL_NULL;
#endif
}

/********************************************************************************************

>	virtual void AttrBitmapFill::OnControlDrag( DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a fill control point.

********************************************************************************************/

void AttrBitmapFill::OnControlDrag( DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Point Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	// Bodge Alert !!
	
	// Bitmap fills have a complicated control point system.
	// The actual control points needed for rendering are the Bottom Left,
	// Top Left, and Bottom Right points of the bitmap.  But on screen the 
	// points seen and moved are the Centre, Middle Right, and Middle Top.
	// So we need to convert the click points into vitual points before
	// we start dragging them about.

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	INT32 dx, dy;

	// Which control is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:

			// They're dragging the Centre Point
			dx = CentrePoint.x - Pos.x;
			dy = CentrePoint.y - Pos.y;
			// Move the other points relative
			Drag2Blob.translate(-dx, -dy);
			Drag3Blob.translate(-dx, -dy);
			CentrePoint = Pos;
			break;

		case FILLCONTROL_ENDPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre position.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(CentrePoint, &Pos);

			if (ClickMods.Adjust)
			{
				// Shift is pressed, so lock the aspect ratio of the bitmap mesh
				double OldLen = CentrePoint.Distance(Drag2Blob);
				double NewLen = CentrePoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new point based on the aspect ratio
				Drag3Blob = 
						MakeLineAtAngle(CentrePoint, Pos, 90, INT32(CentrePoint.Distance(Drag3Blob) * Ratio));
			}

			Drag2Blob   = Pos;
			break;

		case FILLCONTROL_SECONDARYPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre position.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(CentrePoint, &Pos);

			if (ClickMods.Adjust)
			{
				// Shift is pressed, so lock the aspect ratio of the bitmap mesh
				double OldLen = CentrePoint.Distance(Drag3Blob);
				double NewLen = CentrePoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new point based on the aspect ratio
				Drag2Blob = 
						MakeLineAtAngle(CentrePoint, Pos, -90, INT32(CentrePoint.Distance(Drag2Blob) * Ratio));
			}

			Drag3Blob  = Pos;
			break;
	}

	// Now we have to convert the Virtual Coords back into real
	// bitmap control points.
	TRACEUSER( "Mike", _T("Centre at %d,%d\n"), CentrePoint.x, CentrePoint.y);

	GetBitmapRealPoints(CentrePoint, Drag2Blob, Drag3Blob,
							&StartPoint, &EndPoint, &EndPoint2);

	// Store the new points back in the Fill
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);
#endif
}

void AttrBitmapFill::SetAspectRatio(double Ratio)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Point Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	Drag3Blob = MakeLineAtAngle(CentrePoint, 
								Drag2Blob, 
								90, 
								INT32(CentrePoint.Distance(Drag2Blob) * Ratio)
								);

	GetBitmapRealPoints(CentrePoint, Drag2Blob, Drag3Blob,
							&StartPoint, &EndPoint, &EndPoint2);

	// Store the new points back in the Fill
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);
#endif
}

/********************************************************************************************

>	void AttrBitmapFill::ValidateAttributeValue()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Makes sure the Coords of the Fill are sensible. Defaults to natural size
				of the bitmap

********************************************************************************************/

void AttrBitmapFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0) &&
		(*GetEndPoint2()) != DocCoord(0,0))
		return;

	// Make some defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = AttrBounds.Height();
	}

	BitmapInfo Info;
	BOOL GotBitmap = FALSE;

	// if possible we base our default size on the bitmaps preferred size
	KernelBitmap* KerBmp = ((FillGeometryAttribute*)GetAttributeValue())->GetBitmap();
	
	if (KerBmp && KerBmp->ActualBitmap)
		GotBitmap = KerBmp->ActualBitmap->GetInfo( &Info );

	// Use the recommended Width and Height if possible
	INT32 BitmapWidth  = (GotBitmap ? Info.RecommendedWidth  : Width );
	INT32 BitmapHeight = (GotBitmap ? Info.RecommendedHeight : Height );

	// Find the middle of the Attributes Bounds
	DocCoord Centre = CentreOf(AttrBounds);

	// Get the current control positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	// If the StartPoint is 'NULL' then make all points sensible
	if (StartPoint == DocCoord(0,0))
	{
		// Start in the middle
		StartPoint = DocCoord(Centre.x - BitmapWidth/2, Centre.y - BitmapHeight/2);
		// End on the Middle Right
		EndPoint = DocCoord(StartPoint.x + BitmapWidth, StartPoint.y);
		// and Middle Top
		EndPoint2 = DocCoord(StartPoint.x, StartPoint.y + BitmapHeight);
	}

	// If the EndPoint is 'NULL' then make end points sensible
	if (EndPoint == DocCoord(0,0))
	{
		EndPoint = DocCoord(StartPoint.x + BitmapWidth, StartPoint.y);
		EndPoint2 = DocCoord(StartPoint.x, StartPoint.y + BitmapHeight);
	}

	// If the EndPoint2 is 'NULL' then make it sensible
	if (EndPoint2 == DocCoord(0,0))
	{
		EndPoint2 = DocCoord(StartPoint.x, StartPoint.y + BitmapHeight);
	}

	// Store the validated positions back
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);
#endif
}

/********************************************************************************************

>	void AttrBitmapFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrBitmapFill::RenderFillBlobs

********************************************************************************************/

void AttrBitmapFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	DocCoord Start;
	DocCoord End;
	DocCoord End2; 

	// Work out the Virtual positions of the Bitmap Controls
	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&Start, &End, &End2);

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, Start, End2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(Start, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(Start, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		// If we are being dragged then draw a Parallelgram to
		// show the shape of the bitmap
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_NONE);
	
		Path ParaPath;
		ParaPath.Initialise();
		MakeMeshParallelagram(&ParaPath, Start, End, End2);

		// Draw the parallelagram
		pRender->DrawPath(&ParaPath);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	virtual DocRect AttrBitmapFill::GetBlobBoundingRect()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/94
	Returns:	DocRect - The rectangle that contains all the nodes selection blobs.
	Purpose:	Calculates the bounding rectangle of the attrs blobs.This should always 
				be calculated on the fly as the view scale can change without the attr 
				knowing, giving an incorrect result.

********************************************************************************************/

DocRect AttrBitmapFill::GetBlobBoundingRect()
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Optimisation.  If there is currently no interest in Fill Blobs
	// and this fill is not being Dragged (Fill blobs are turned off during
	// a fill drag), then we needn't bother doing anything. 
	if ( (!GetApplication()->GetBlobManager()->GetCurrentInterest(TRUE).Fill || !IsVisible()) && DraggedFill != this )
		return DocRect(0,0, 0,0);

	// Get the Start and End Points
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint   = *GetEndPoint();
	DocCoord EndPoint2  = *GetEndPoint2();

	// Work out the Virtual Coords
	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&StartPoint, &EndPoint, &EndPoint2);

	// Make a dummy bounds from just the Start Point
	DocRect BoundingRect(StartPoint, StartPoint);

	if (DraggedFill == this)
	{
		// This fill is being dragged, so we have to include the Parallelagram 
		// bounding rect as well
		DocRect StartBlobRect;
		DocRect EndBlobRect;
		DocRect DragRect = GetParallelagramBounds(StartPoint, EndPoint, EndPoint2);

		// Get the Bounding rect of Blobs on each of the ends
		(Camelot.GetBlobManager())->GetBlobRect(DragRect.lo, &StartBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(DragRect.hi, &EndBlobRect);

	 	// Now include the Bottom Left and Top Right of each blob in the Bounds.
		// We have to do it like this to make sure that the DocRect's coords
		// are valid.  ie. The Hi's are Higher than the Lo's.
		BoundingRect.IncludePoint(StartBlobRect.lo);
		BoundingRect.IncludePoint(StartBlobRect.hi);
		BoundingRect.IncludePoint(EndBlobRect.lo);
		BoundingRect.IncludePoint(EndBlobRect.hi);
	}
	else
	{
		// We're not being dragged, so just calc the bounds of the Start and End Blobs
		DocRect StartBlobRect;
		DocRect EndBlobRect;
		DocRect End2BlobRect;

		// Get the Bounding rect of the Fill Line, including the Blobs on the ends
		(Camelot.GetBlobManager())->GetBlobRect(StartPoint, &StartBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(EndPoint, &EndBlobRect);
		(Camelot.GetBlobManager())->GetBlobRect(EndPoint2, &End2BlobRect);

	 	// Now include the Bottom Left and Top Right of each blob in the Bounds.
		// We have to do it like this to make sure that the DocRect's coords
		// are valid.  ie. The Hi's are Higher than the Lo's.
		BoundingRect.IncludePoint(StartBlobRect.lo);
		BoundingRect.IncludePoint(StartBlobRect.hi);
		BoundingRect.IncludePoint(EndBlobRect.lo);
		BoundingRect.IncludePoint(EndBlobRect.hi);
		BoundingRect.IncludePoint(End2BlobRect.lo);
		BoundingRect.IncludePoint(End2BlobRect.hi);
	}

	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint);
	IncludeArrowHead(&BoundingRect, StartPoint, EndPoint2);

	// and return it
	return BoundingRect;
#else
	return DocRect(0,0,0,0);
#endif
}

/********************************************************************************************

>	void AttrBitmapFill::DrawEndBlobs()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/1/95
	Purpose:	Draws the blobs on the ends of the fill arrows.

********************************************************************************************/

void AttrBitmapFill::DrawEndBlobs()
{
#if !defined(EXCLUDE_FROM_RALPH)
	Node *pNode = this;
	while ((pNode != NULL) && !pNode->IsKindOf(CC_RUNTIME_CLASS(Spread)))
		pNode = pNode->FindParent();

	if (pNode == NULL)
		return;			// We're not really in the tree

	Spread* pSpread = (Spread*)pNode;

	DocRect Bounds = GetBlobBoundingRect();

	DocCoord Start = *GetStartPoint();
	DocCoord End   = *GetEndPoint();
	DocCoord End2  = *GetEndPoint2();

	GetBitmapVirtualPoints(Start, End, End2,
							&Start, &End, &End2);

	RenderRegion* pRender = DocView::RenderOnTop(&Bounds, pSpread, UnclippedEOR);
	while (pRender)
	{
		// Draw a blob at the start point
		if (IsSelected(FILLCONTROL_STARTPOINT))
		{
			// Draw Selected Blob
//			pRender->SetLineColour(COLOUR_SELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_SELECTEDBLOB);
			pRender->DrawBlob(Start, BT_SELECTED);
		}
		else
		{
			// Draw Unselected Blob
//			pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
			pRender->DrawBlob(Start, BT_UNSELECTED);
		}

		// Draw a blob at the end point
		if (IsSelected(FILLCONTROL_ENDPOINT))
		{
			// Draw Selected Blob
//			pRender->SetLineColour(COLOUR_SELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_SELECTEDBLOB);
			pRender->DrawBlob(End, BT_SELECTED);
			pRender->DrawBlob(End2, BT_SELECTED);
		}
		else
		{
			// Draw Unselected Blob
//			pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
			pRender->DrawBlob(End, BT_UNSELECTED);
			pRender->DrawBlob(End2, BT_UNSELECTED);
		}

		// Get the Next render region
		pRender = DocView::GetNextOnTop(&Bounds);
	}

	// Bodge to stop fill meshes EOR each other out.
	AttrFillGeometry::LastRenderedMesh = NULL;
#endif
}


//===========================================================================================
//
//	Karim MacDonald 08/12/1999
//	Extending Bits
//
//===========================================================================================

/********************************************************************************************

>	DocRect AttrBitmapFill::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE	if this bitmap fill may be reversible extended.
				FALSE	otherwise.
	Purpose:	Determine whether applying an extend to this Node is a reversible process.
	Errors:		
	See also:	class Extender; IsTypeExtendible(); Extend()

********************************************************************************************/
DocRect AttrBitmapFill::ValidateExtend(const ExtendParams& ExtParams)
{
	// a bitmap fill is treated a bit like text - it should maintain its aspect ratio and
	// translate as a whole. therefore, we only validate its extend reference-centre.
	// it also has no children to test.
	DocCoord doccArray[1] = { FindExtendCentre() };
	return Extender::ValidateControlPoints(1, doccArray, ExtParams);
//	return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
}



/********************************************************************************************

>	void AttrBitmapFill::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	08/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	This fill node may have its dimensions altered by the extension.
	Returns:	
	Purpose:	Perform an extend operation on this fill node.
	Errors:		
	See also:	class Extender; IsTypeExtendible(); Extend()

********************************************************************************************/
void AttrBitmapFill::Extend(const ExtendParams& ExtParams)
{
	// a bitmap fill's behaviour depends upon what it is applied to:
	//	non-path	with	EXTEND	::	do nothing - its parent will transform it correctly.
	//	path node	with	EXTEND	::	translate itself; remember that its parent has moved
	//									its centre by ExtParams.doccOffset already.
	//	any node	with	STRETCH	::	we need to unstretch ourself...
	// it is assumed here, though this may not always be true, that the fill's parent node
	// is the node to which it is applied.
	Node* pParent = FindParent();
	if (pParent != NULL)
		if (pParent->IsNodePath() || pParent->IsCompound())
			TransformTranslateObject(ExtParams);

	// if we are asked to stretch, we must first undo any stretch which was done to us by the
	// node we are applied to. then we must translate ourself, so that we maintain our place,
	// relative to the centre of the button.


	// we do all of this because this is what would be expected for a texture on a button.
	// the texture should _not_ stretch if the button stretches, but _should_ move around
	// with the button.
	// if the button stretches and moves, we arbitrarily decided that the fill should remain
	// in the same position relative to the scale-reference coord of the button - see
	// definition of ExtendParams for details.
	ExtendParams eps = ExtParams;
	eps.xscale = 1 / eps.xscale;					//
	eps.yscale = 1 / eps.yscale;					//	ie,	'undo the stretch, but leave
	eps.doccScaleStart = ExtParams.doccScaleEnd;	//		us where the stretch put us."
	eps.doccScaleEnd = ExtParams.doccScaleEnd;		//
	TransformStretchObject(eps);
}



/********************************************************************************************

>	virtual void AttrBitmapFill::TransformTranslateObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/12/1999
	Inputs:		ExtParams		description of how this bitmap fill should translate.
	Outputs:	This fill may be translated, depending on the flags held within ExtParams.
	Purpose:	Translate this bitmap fill, according to the offsets and flags defined in ExtParams,
				and a reference centre, defined by NodeRenderable::FindExtendCentre().
				This function does nothing unless ExtParams uses X_EXTEND or Y_EXTEND.

				This function is identical to NodeRenderable::TransformObject, except that
				in all cases except when our parent is a group, it will have already applied
				ExtParams' doccOffset to us, so we can usually omit this step.

	See also:	The Extender class; FindExtendCentre().

********************************************************************************************/
void AttrBitmapFill::TransformTranslateObject(const ExtendParams& ExtParams)
{
	DocCoord doccCentre = FindExtendCentre();

	// x-extension - we move the whole Node; behaviour determined by its centre.
	if (ExtParams.fExtendFlags & X_EXTEND)
	{
		// if our parent is a group, then it won't have applied any offset parameter to us,
		// so we need to do this ourself.
		Node* pParent = FindParent();
		if (pParent != NULL && pParent->IsCompound())
		{
			Trans2DMatrix baseXoffset(ExtParams.doccOffset.x, 0);
			Transform(baseXoffset);
		}

		// we only move anything if we're outside a buffer-zone around the centre
		// (necessary as objects can be misaligned by a millipoint or two).
		if (doccCentre.x > ExtParams.doccEndCentre.x + ExtParams.xincExtendBuffer)
		{
			Trans2DMatrix translateX(ExtParams.xinc, 0);
			Transform(translateX);
		}
		else if (doccCentre.x < ExtParams.doccEndCentre.x - ExtParams.xdecExtendBuffer)
		{
			Trans2DMatrix translateX(-ExtParams.xdec, 0);
			Transform(translateX);
		}
	}

	// y-extension - we move the whole Node; behaviour determined by its centre.
	if (ExtParams.fExtendFlags & Y_EXTEND)
	{
		// if our parent is a group, then it won't have applied any offset parameter to us,
		// so we need to do this ourself.
		Node* pParent = FindParent();
		if (pParent != NULL && pParent->IsCompound())
		{
			Trans2DMatrix baseYoffset(0, ExtParams.doccOffset.y);
			Transform(baseYoffset);
		}

		// only extend if we're outside the centre's buffer-zone.
		if (doccCentre.y > ExtParams.doccEndCentre.y + ExtParams.yincExtendBuffer)
		{
			Trans2DMatrix translateY(0, ExtParams.yinc);
			Transform(translateY);
		}
		else if (doccCentre.y < ExtParams.doccEndCentre.y - ExtParams.ydecExtendBuffer)
		{
			Trans2DMatrix translateY(0, -ExtParams.ydec);
			Transform(translateY);
		}
	}
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrBitmapColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrBitmapColourFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Bitmap Fill Colour attribute.

********************************************************************************************/

void AttrBitmapColourFill::Render(RenderRegion* pRender)
{
	pRender->SetFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrBitmapColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrBitmapColourFill::SimpleCopy()
{
	AttrBitmapColourFill* NodeCopy = new AttrBitmapColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrBitmapColourFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrBitmapColourFill::GetAttrNameID(void)  
{
	return (_R(IDS_BITMAPFILL)); 
}                                  

/********************************************************************************************

>	void AttrBitmapColourFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrBitmapColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nBitmap Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;

	BitmapInfo Info;
	KernelBitmap* KerBmp = ((BitmapFillAttribute*)GetAttributeValue())->GetBitmap();

	if (KerBmp && KerBmp->ActualBitmap && KerBmp->ActualBitmap->GetInfo( &Info ) )
	{
		TempStr._MakeMsg( _T("\r\nBits = (#1%ux#2%ux#3%u) "), Info.PixelWidth,
										Info.PixelHeight,
										Info.PixelDepth
						);
		(*Str) += TempStr;
	}
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrBitmapColourFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBitmapColourFill::GetNodeSize() const 
{     
	return sizeof(AttrBitmapColourFill);
}  

/********************************************************************************************

>	void AttrBitmapColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/6/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrBitmapColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

/*
	TRACEUSER( "Mike", _T("Start=%d,%d; End=%d,%d; End2=%d,%d\n"),ControlPoints[0].x, ControlPoints[0].y,
															 ControlPoints[1].x, ControlPoints[1].y,
															 ControlPoints[2].x, ControlPoints[2].y);

	DocRect ClipRect = pRender->GetClipRect();

	if (DraggedFill == NULL)
	{
		DocColour RandCol = DocColour(ColourValue(rand()>>7), ColourValue(rand()>>7), ColourValue(rand()>>7));

		pRender->SetFillColour(RandCol);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->DrawRect(&ClipRect);
	}

	TRACEUSER( "Mike", _T("ClipRect=%d,%d, %d,%d\n"),ClipRect.lox,ClipRect.loy,ClipRect.hix,ClipRect.hiy);
*/

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

/********************************************************************************************

>	void AttrBitmapColourFill::RenderFillMesh(	RenderRegion* pRender, 
												DocCoord* ControlPoints, BOOL* SelState,
												INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrBitmapFill::RenderFillBlobs

********************************************************************************************/

void AttrBitmapColourFill::RenderFillMesh(	RenderRegion* pRender, 
											DocCoord* ControlPoints, BOOL* SelState,
											INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	DocCoord Start;
	DocCoord End;
	DocCoord End2; 

	// Work out the Virtual positions of the Bitmap Controls
	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&Start, &End, &End2);

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, Start, End2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(Start, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(Start, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		// If we are being dragged then draw a Parallelgram to
		// show the shape of the bitmap
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_NONE);
	
		Path ParaPath;
		ParaPath.Initialise();
		MakeMeshParallelagram(&ParaPath, Start, End, End2);

		// Draw the parallelagram
		pRender->DrawPath(&ParaPath);
	}

	// Now Render the blobs on the path
//	if (GetBitmap()->GetBPP() <= 8)
//	{
		// Draw a blob at the start point
		if (SelState[FILLCONTROL_STARTPOINT])
		{
			// Draw Selected Blob
//			pRender->SetLineColour(COLOUR_SELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_SELECTEDBLOB);
			pRender->DrawBlob(Start, BT_SELECTED);
		}
		else
		{
			// Draw Unselected Blob
//			pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
			pRender->DrawBlob(Start, BT_UNSELECTED);
		}

		// Draw a blob at the end point
		if (SelState[FILLCONTROL_ENDPOINT])
		{
			// Draw Selected Blob
//			pRender->SetLineColour(COLOUR_SELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_SELECTEDBLOB);
			pRender->DrawBlob(End, BT_SELECTED);
			pRender->DrawBlob(End2,BT_SELECTED);
		}
		else
		{
			// Draw Unselected Blob
//			pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
			pRender->SetLineColour(COLOUR_NONE);
			pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
			pRender->DrawBlob(End, BT_UNSELECTED);
			pRender->DrawBlob(End2,BT_UNSELECTED);
		}
//	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	void AttrBitmapColourFill::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		Control, the FillControl to redraw
	Purpose:	Redraws the Fill Control blobs when the selection changes.
	SeeAlso:	FillControl			

********************************************************************************************/

void AttrBitmapColourFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:

			if (GetStartPoint() != NULL)
			{
				// Redraw the Start Point Blob
				(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT:

			if (GetEndPoint() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_SECONDARYPOINT:

			if (GetEndPoint2() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;
	}
#endif
}

/********************************************************************************************

>	BOOL AttrBitmapColourFill::ChangeControlColour(AttrColourChange* NewCol)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/95
	Inputs:		-
	Purpose:	Alter the colour of a selected fill handle
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBitmapColourFill::ChangeControlColour(AttrColourChange* NewCol)
{ 
#if !defined(EXCLUDE_FROM_RALPH)
	// Do contone stuff here ....
	if (SelectionState[FILLCONTROL_STARTPOINT] ||
		SelectionState[FILLCONTROL_ENDPOINT])
	{
		if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill && IsVisible())
		{
			KernelBitmap* pBitmap = GetBitmap();
			KernelBitmap* pGreyBmp = NodeBitmap::CheckGreyscaleBitmap(pBitmap, _R(IDS_MAKEGREY), _R(IDS_DOGREY));

			if (pGreyBmp == NULL)
				return FALSE;	// Failed in some way (or the user cancelled)
		}
	}

	return AttrFillGeometry::ChangeControlColour(NewCol); 
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrBitmapColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the bitmap fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBitmapColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	// Find the kernel bmp
	KernelBitmap *pBmp = Value.GetBitmap();
	ERROR2IF(pBmp == NULL,FALSE,"CXaraFileNodeBitmap::WriteNodeBitmap pBmp is NULL");

	// Must write out the bitmap first
	INT32 BitmapRecordRef = pFilter->WriteRecord(pBmp);
	ERROR2IF(BitmapRecordRef == 0,FALSE,"CXaraFileNodeBitmap::WriteNodeBitmap BitmapRecordRef is 0");

	UINT32 Tag     = TAG_BITMAPFILL;
	UINT32 TagSize = TAG_BITMAPFILL_SIZE;

	// Contone?
	DocColour* pStartCol= Value.GetStartColour();
	DocColour* pEndCol	= Value.GetEndColour();
	INT32 StartColRef, EndColRef;

	if (pStartCol != NULL && pEndCol != NULL)
	{
		// It's a contone bmp if it has both a start & an end colour.
		StartColRef = pFilter->WriteRecord(pStartCol);
		EndColRef   = pFilter->WriteRecord(pEndCol);

		Tag     = TAG_CONTONEBITMAPFILL;
		TagSize = TAG_CONTONEBITMAPFILL_SIZE;

		// Are the colour refs ok?
		ok = (StartColRef != 0) && (EndColRef != 0);
	}

	// Is the bmp reference ok?
	if (ok) ok = (BitmapRecordRef != 0);
		
	CamelotFileRecord Rec(pFilter,Tag,TagSize);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);

	if (Tag == TAG_CONTONEBITMAPFILL)
	{
		if (ok) ok = Rec.WriteReference(StartColRef);
		if (ok) ok = Rec.WriteReference(EndColRef);
	}

	if (ok) ok = Rec.WriteReference(BitmapRecordRef);
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrBitmapColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBitmapColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrBitmapColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrBitmapColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Contone?
	DocColour* pStartCol= Value.GetStartColour();
	DocColour* pEndCol	= Value.GetEndColour();
	INT32 StartColRef, EndColRef;

	if (pStartCol != NULL && pEndCol != NULL)
	{
		// It's a contone bmp if it has both a start & an end colour.
		StartColRef = pFilter->WriteRecord(pStartCol);
		EndColRef   = pFilter->WriteRecord(pEndCol);

		// Are the colour references ok?
		BOOL ok = ((StartColRef != 0) && (EndColRef != 0));

		return (ok);
	}
	
	return (TRUE);
}


/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrBitmapTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrBitmapTranspFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Bitmap Fill Transp attribute.

********************************************************************************************/

void AttrBitmapTranspFill::Render(RenderRegion* pRender)
{
	pRender->SetTranspFillGeometry(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrBitmapTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrBitmapTranspFill::SimpleCopy()
{
	AttrBitmapTranspFill* NodeCopy = new AttrBitmapTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrBitmapTranspFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrBitmapTranspFill::GetAttrNameID(void)  
{
	return (_R(IDS_BITMAPTRANSPFILL)); 
}                                  

/********************************************************************************************

>	void AttrBitmapTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrBitmapTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nBitmap Fill:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;

	BitmapInfo Info;
	KernelBitmap* KerBmp = ((BitmapTranspFillAttribute*)GetAttributeValue())->GetBitmap();

	if (KerBmp && KerBmp->ActualBitmap && KerBmp->ActualBitmap->GetInfo( &Info ) )
	{
		TempStr._MakeMsg( _T("\r\nBits = (#1%ux#2%ux#3%u) "), Info.PixelWidth,
										Info.PixelHeight,
										Info.PixelDepth
						);
		(*Str) += TempStr;
	}
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrBitmapTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrBitmapTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrBitmapTranspFill);
}  

/********************************************************************************************

>	void AttrBitmapTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrBitmapTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're Not in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}

void AttrBitmapTranspFill::RenderFillMesh(	RenderRegion* pRender, 
											DocCoord* ControlPoints, BOOL* SelState,
											INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	DocCoord Start;
	DocCoord End;
	DocCoord End2; 

	// Work out the Virtual positions of the Bitmap Controls
	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&Start, &End, &End2);

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, Start, End2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(Start, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(Start, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		// If we are being dragged then draw a Parallelgram to
		// show the shape of the bitmap
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_NONE);
	
		Path ParaPath;
		ParaPath.Initialise();
		MakeMeshParallelagram(&ParaPath, Start, End, End2);

		// Draw the parallelagram
		pRender->DrawPath(&ParaPath);
	}

	// Now Render the blobs on the path

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(Start, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(Start, BT_UNSELECTED);
	}

	// Draw a blob at the end point
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End, BT_SELECTED);
		pRender->DrawBlob(End2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End, BT_UNSELECTED);
		pRender->DrawBlob(End2,BT_UNSELECTED);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}

void AttrBitmapTranspFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:

			if (GetStartPoint() != NULL)
			{
				// Redraw the Start Point Blob
				(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT:

			if (GetEndPoint() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_SECONDARYPOINT:

			if (GetEndPoint2() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;
	}
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrBitmapTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the bitmap transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrBitmapTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	// Find the kernel bmp
	KernelBitmap *pBmp = Value.GetBitmap();
	ERROR2IF(pBmp == NULL,FALSE,"CXaraFileNodeBitmap::WriteNodeBitmap pBmp is NULL");

	// Must write out the bitmap first
	INT32 BitmapRecordRef = pFilter->WriteRecord(pBmp);
	ERROR2IF(BitmapRecordRef == 0,FALSE,"CXaraFileNodeBitmap::WriteNodeBitmap BitmapRecordRef is 0");

	// Is the bmp reference ok?
	if (ok) ok = (BitmapRecordRef != 0);
		
	CamelotFileRecord Rec(pFilter,TAG_BITMAPTRANSPARENTFILL,TAG_BITMAPTRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));
	if (ok) ok = Rec.WriteReference(BitmapRecordRef);
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());
	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrBitmapTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrBitmapTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}




/********************************************************************************************
>	BOOL AttrBitmapTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrBitmapTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFractalFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	void AttrFractalFill::Transform( TransformBase& Trans )

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Trans - the transform object to apply to this attribute.
	Purpose:	Transform a grad fill attribute by moving the start and end points.
	SeeAlso:	NodeRenderable::Transform

********************************************************************************************/

void AttrFractalFill::Transform( TransformBase& Trans )
{
	if ( Trans.TransFills )
	{
		Trans.Transform( GetStartPoint(), 1);
		Trans.Transform( GetEndPoint(), 1);
		Trans.Transform( GetEndPoint2(), 1);
		Trans.Transform( GetEndPoint3(), 1);

		// Ensure the transformed Points are sensible
//		ValidateAttributeValue();
		SetFractalDPI(GetDPI());
	}
}

/********************************************************************************************

>	BOOL AttrFractalFill::CanTransform()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	TRUE => transform this attribute.
	Purpose:	Indicate that this attribute can be transformed.
	SeeAlso:	NodeRenderable::CanTransform

********************************************************************************************/

BOOL AttrFractalFill::CanTransform()
{
	return TRUE;
}

/********************************************************************************************

>	BOOL AttrFractalFill::CopyNodeContents( AttrFractalFill* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFractalFill::CopyNodeContents( AttrFractalFill* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrFractalFill::CopyNodeContents!");

	AttrFillGeometry::CopyNodeContents( NodeCopy );

	// Copy contents specific to derived class here
	return TRUE;
} 

/***********************************************************************************************

>    virtual BOOL AttrFractalFill::HidingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   9/11/94
	 Returns:   TRUE if all was ok. FALSE if an error occured.
     Purpose:   This virtual function is called whenever the node is hidden.
	 			It allows the node do things like 'optimise' itself to use less memory or
	 			send a message to let others know it is being hidden etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL AttrFractalFill::HidingNode()
{
	// Call the base class first
	if (!AttrFillGeometry::HidingNode())
		return FALSE;

	((FillGeometryAttribute*)GetAttributeValue())->DetachBitmap();

	return TRUE;
}

/***********************************************************************************************

>    virtual BOOL AttrFractalFill::ShowingNode()

     Author:    Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
     Created:   9/11/94
	 Returns:   TRUE if all was ok. FALSE if an error occured (eg Out of memory).
     Purpose:   This virtual function is called whenever the node is re-shown after being 
     			Hidden.
	 			It allows the node to reconstruct itself if it was optimised or
	 			send a message to let others know it is back etc.

				ALWAYS CALL THE BASE CLASS' FUNCTION FROM YOUR OVERRIDDEN FUNCTION.

***********************************************************************************************/

BOOL AttrFractalFill::ShowingNode()
{
	// Call the base class first
	if (!AttrFillGeometry::ShowingNode())
		return FALSE;

	RecalcFractal();

	return TRUE;
}

/********************************************************************************************

>	void AttrFractalFill::ValidateAttributeValue()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/8/94
	Purpose:	Makes sure the Coords of the Fill are sensible. Defaults to natural size
				of the bitmap

********************************************************************************************/

void AttrFractalFill::ValidateAttributeValue()
{
#if !defined(EXCLUDE_FROM_RALPH)
	if ((*GetStartPoint()) != DocCoord(0,0) && (*GetEndPoint()) != DocCoord(0,0) &&
		(*GetEndPoint2()) != DocCoord(0,0))
		return;

	// Make some defaults
	DocRect AttrBounds = DocRect(0,0,0,0);

	INT32 Width  = DEFAULT_FILLWIDTH;
	INT32 Height = DEFAULT_FILLHEIGHT;

	// Are we an Orphan ?
 	if (FindParent() != NULL)
	{
		// Nope, so we can use Daddies Bounding Box
		SelRange* Selected = GetApplication()->FindSelection();
				 
		if (Selected == NULL || Selected->Count() <= 1)
			AttrBounds = ((NodeRenderableBounded*)FindParent())->GetBoundingRect(TRUE);
		else
			AttrBounds = Selected->GetBoundingRect();

		Width  = AttrBounds.Width();
		Height = Width;
	}

	// Find the middle of the Attributes Bounds
	DocCoord Centre = CentreOf(AttrBounds);

	// Get the current control positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	BOOL recalc = FALSE;

	// If the StartPoint is 'NULL' then make all points sensible
	if (StartPoint == DocCoord(0,0))
	{
		// Start in the middle
		StartPoint = DocCoord(Centre.x - Width/2, Centre.y - Height/2);
		// End on the Middle Right
		EndPoint = DocCoord(StartPoint.x + Width, StartPoint.y);
		// and Middle Top
		EndPoint2 = DocCoord(StartPoint.x, StartPoint.y + Height);

		recalc = TRUE;
	}

	// If the EndPoint is 'NULL' then make end points sensible
	if (EndPoint == DocCoord(0,0))
	{
		EndPoint = DocCoord(StartPoint.x + Width, StartPoint.y);
		EndPoint2 = DocCoord(StartPoint.x, StartPoint.y + Height);

		recalc = TRUE;
	}

	// If the EndPoint2 is 'NULL' then make it sensible
	if (EndPoint2 == DocCoord(0,0))
	{
		EndPoint2 = DocCoord(StartPoint.x, StartPoint.y + Height);

		recalc = TRUE;
	}

	// Store the validated positions back
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);

	if (recalc)
		((FillGeometryAttribute*)GetAttributeValue())->RecalcFractal();
#endif
}

/********************************************************************************************

>	virtual NodeAttribute* AttrFractalFill::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrFractalFill::GetOtherAttrToApply(BOOL* IsMutate)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// A Fractal fill change also needs to set the Fill Mapping

	NodeAttribute* OtherAttr = NULL;

	BOOL Tileable = GetTileable();

	if (GetAttributeType() == CC_RUNTIME_CLASS(AttrFillGeometry))
	{
		OtherAttr = new AttrFillMappingLinear;
	}
	else
	{
		OtherAttr = new AttrTranspFillMappingLinear;
	}

	if (OtherAttr == NULL)
		return NULL;

	if (Tileable)
		((AttrFillMappingLinear*)OtherAttr)->SetRepeat(RT_Repeating);
	else
		((AttrFillMappingLinear*)OtherAttr)->SetRepeat(RT_RepeatInverted);

	*IsMutate = FALSE;

	return OtherAttr;
#else
	return NULL;
#endif
}

void AttrFractalFill::SetAspectRatio(double Ratio)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Point Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	Drag3Blob = MakeLineAtAngle(CentrePoint, 
								Drag2Blob, 
								90, 
								INT32(CentrePoint.Distance(Drag2Blob) * Ratio)
								);

	GetBitmapRealPoints(CentrePoint, Drag2Blob, Drag3Blob,
							&StartPoint, &EndPoint, &EndPoint2);

	// Store the new points back in the Fill
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrTextureColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrTextureColourFill::AttrTextureColourFill()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Constructor for a texture fill.

********************************************************************************************/

AttrTextureColourFill::AttrTextureColourFill()
{
}

/********************************************************************************************

>	AttrTextureColourFill::~AttrTextureColourFill()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Destructor for a texture fill.

********************************************************************************************/

AttrTextureColourFill::~AttrTextureColourFill()
{
}



/********************************************************************************************

> Node* AttrTextureColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrTextureColourFill::SimpleCopy()
{
	TRACEUSER( "Mike", _T("AttrTextureColourFill::SimpleCopy() called. This should NOT happen."));

	AttrTextureColourFill* NodeCopy = new AttrTextureColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  


/********************************************************************************************

>	virtual UINT32 AttrTextureColourFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTextureColourFill::GetAttrNameID (void)
{	
	return (_R(IDS_BITMAPFILL));
}

// I can't be bothered to enter any comments for these ....

UINT32 AttrFractalColourFill::GetAttrNameID (void)
{
	return (_R(IDS_FRACTALCLOUDSGRADFILL));
}

UINT32 AttrNoiseColourFill::GetAttrNameID (void)
{
	return (_R(IDS_FRACTALPLASMAGRADFILL));
}



/********************************************************************************************

>	void AttrTextureColourFill::GetDebugDetails(StringBase* Str)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrTextureColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nTexture Fill:\r\n"));
	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nStart"));
//	(*GetStartColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

//	TempStr._MakeMsg(TEXT("\r\nEnd"));
//	(*GetEndColour()).GetDebugDetails(&TempStr);
//	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) \n"), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;

#endif
}


/********************************************************************************************

>	void AttrTextureColourFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Fractal Fill colour attribute.

********************************************************************************************/

void AttrTextureColourFill::Render(RenderRegion* pRender)
{
	if (GetBitmap() == NULL || GetBitmap()->ActualBitmap == NULL)
	{
		if (!RecalcFractal())
			return;
	}

	pRender->SetFillGeometry((ColourFillAttribute*)GetAttributeValue(), FALSE);
}



/********************************************************************************************

>	void AttrTextureColourFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrTextureColourFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're Not in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}



/********************************************************************************************

>	FillControl AttrTextureColourFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrTextureColourFill::CheckForControlHit(DocCoord &ClickPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
	DocRect BlobRect;

	DocCoord StartPoint  = *GetStartPoint();
	DocCoord EndPoint    = *GetEndPoint();
	DocCoord EndPoint2   = *GetEndPoint2();

	// Bodge Alert !!
	
	// Bitmap fills have a complicated control point system.
	// The actual control points needed for rendering are the Bottom Left,
	// Top Left, and Bottom Right points of the bitmap.  But on screen the 
	// points seen and moved are the Centre, Middle Right, and Middle Top.
	// So we need to convert the click points into vitual points before
	// we do the hit test.

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	// Get the rectangle around the Centre Control Point
	(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the First End Point
	(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	// Get the rectangle around the Second End Point
	(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_SECONDARYPOINT;

	return HitControl;
#else
	return FILLCONTROL_NULL;
#endif
}


/********************************************************************************************

>	virtual void AttrTextureColourFill::OnControlDrag( DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a fill control point.

********************************************************************************************/

void AttrTextureColourFill::OnControlDrag( DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Point Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	// Bodge Alert !!
	
	// Bitmap fills have a complicated control point system.
	// The actual control points needed for rendering are the Bottom Left,
	// Top Left, and Bottom Right points of the bitmap.  But on screen the 
	// points seen and moved are the Centre, Middle Right, and Middle Top.
	// So we need to convert the click points into vitual points before
	// we start dragging them about.

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	INT32 dx, dy;

	// Which control is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:

			// They're dragging the Centre Point
			dx = CentrePoint.x - Pos.x;
			dy = CentrePoint.y - Pos.y;
			// Move the other points relative
			Drag2Blob.translate(-dx, -dy);
			Drag3Blob.translate(-dx, -dy);
			CentrePoint = Pos;
			break;

		case FILLCONTROL_ENDPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre position.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(CentrePoint, &Pos);

			if (ClickMods.Adjust)
			{
				// Shift is pressed, so lock the aspect ratio of the bitmap mesh
				double OldLen = CentrePoint.Distance(Drag2Blob);
				double NewLen = CentrePoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new point based on the aspect ratio
				Drag3Blob = 
						MakeLineAtAngle(CentrePoint, Pos, 90, INT32(CentrePoint.Distance(Drag3Blob) * Ratio));
			}

			Drag2Blob   = Pos;
			break;

		case FILLCONTROL_SECONDARYPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre position.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(CentrePoint, &Pos);

			if (ClickMods.Adjust)
			{
				// Shift is pressed, so lock the aspect ratio of the bitmap mesh
				double OldLen = CentrePoint.Distance(Drag3Blob);
				double NewLen = CentrePoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new point based on the aspect ratio
				Drag2Blob = 
						MakeLineAtAngle(CentrePoint, Pos, -90, INT32(CentrePoint.Distance(Drag2Blob) * Ratio));
			}

			Drag3Blob  = Pos;
			break;
	}

	// Now we have to convert the Vitual Coords back into real
	// bitmap control points.

	GetBitmapRealPoints(CentrePoint, Drag2Blob, Drag3Blob,
							&StartPoint, &EndPoint, &EndPoint2);

	// Store the new points back in the Fill
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);
#endif
}


/********************************************************************************************

>	void AttrTextureColourFill::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		Control, the FillControl to redraw
	Purpose:	Redraws the Fill Control blobs when the selection changes.
	SeeAlso:	FillControl			

********************************************************************************************/

void AttrTextureColourFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:

			if (GetStartPoint() != NULL)
			{
				// Redraw the Start Point Blob
				(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT:

			if (GetEndPoint() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_SECONDARYPOINT:

			if (GetEndPoint2() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;
	}
#endif
}



/********************************************************************************************

>	void AttrTextureColourFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrBitmapFill::RenderFillBlobs

********************************************************************************************/

void AttrTextureColourFill::RenderFillMesh(RenderRegion* pRender, 
											DocCoord* ControlPoints, BOOL* SelState,
											INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	DocCoord Start;
	DocCoord End;
	DocCoord End2; 

	// Work out the Virtual positions of the Bitmap Controls
	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&Start, &End, &End2);

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, Start, End2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(Start, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(Start, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		// If we are being dragged then draw a Parallelgram to
		// show the shape of the bitmap
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_NONE);
	
		Path ParaPath;
		ParaPath.Initialise();
		MakeMeshParallelagram(&ParaPath, Start, End, End2);

		// Draw the parallelagram
		pRender->DrawPath(&ParaPath);
	}

	// Now Render the blobs on the path

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(Start, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(Start, BT_UNSELECTED);
	}

	// Draw a blob at the end point
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End, BT_SELECTED);
		pRender->DrawBlob(End2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End, BT_UNSELECTED);
		pRender->DrawBlob(End2,BT_UNSELECTED);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}

















/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFractalColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////


/********************************************************************************************

>	AttrFractalColourFill::AttrFractalColourFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Constructor for a fractal fill.

********************************************************************************************/

AttrFractalColourFill::AttrFractalColourFill()
{
	SetFractalDPI(AttrFillGeometry::FractalDPI);
}

/********************************************************************************************

>	AttrFractalColourFill::~AttrFractalColourFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Destructor for a fractal fill.

********************************************************************************************/

AttrFractalColourFill::~AttrFractalColourFill()
{
}



/********************************************************************************************

>	void AttrFractalColourFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrFractalColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	AttrTextureColourFill::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nFractal Fill:\r\n"));
	(*Str) += TempStr;

	INT32 Seed 			= ((FractalFillAttribute*)GetAttributeValue())->Seed;
//	double Gravity 		= ((FractalFillAttribute*)GetAttributeValue())->Gravity.MakeDouble();
//	double Graininess 	= ((FractalFillAttribute*)GetAttributeValue())->Graininess.MakeDouble();
//	double Squash 		= ((FractalFillAttribute*)GetAttributeValue())->Squash.MakeDouble();

	TempStr._MakeMsg(TEXT("\r\n\nSeed = #1%ld "), 
					 (Seed));
	(*Str) += TempStr;
#endif
}


/********************************************************************************************

>	virtual UINT32 AttrFractalColourFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFractalColourFill::GetNodeSize() const 
{     
	return sizeof(AttrFractalColourFill);
}  


/********************************************************************************************

>	Node* AttrFractalColourFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFractalColourFill::SimpleCopy()
{
	AttrFractalColourFill* NodeCopy = new AttrFractalColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  


/********************************************************************************************

  >	virtual BOOL AttrFractalColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the fractal fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFractalColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	// Make sure the start & end colours are written out first.
	INT32 StartColRef = pFilter->WriteRecord(Value.GetStartColour());
	INT32 EndColRef   = pFilter->WriteRecord(Value.GetEndColour());

	// Are the colour refs ok?
	ok = (StartColRef != 0) && (EndColRef != 0);

	if (ok)
	{
		CamelotFileRecord Rec(pFilter,TAG_FRACTALFILL,TAG_FRACTALFILL_SIZE);

		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Value.StartPoint);
		if (ok) ok = Rec.WriteCoord(Value.EndPoint);
		if (ok) ok = Rec.WriteCoord(Value.EndPoint2);
		if (ok) ok = Rec.WriteReference(StartColRef);
		if (ok) ok = Rec.WriteReference(EndColRef);

		if (ok) ok = Rec.WriteINT32(Value.Seed);
		if (ok) ok = Rec.WriteFIXED16(Value.Graininess);
		if (ok) ok = Rec.WriteFIXED16(Value.Gravity);
		if (ok) ok = Rec.WriteFIXED16(Value.Squash);
		if (ok) ok = Rec.WriteINT32(Value.Dpi);
		if (ok) ok = Rec.WriteBYTE(Value.Tileable);
		if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
		if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());

		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFractalColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFractalColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

  >	virtual BOOL AttrFractalColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrFractalColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Make sure the start & end colours are written out first.
	INT32 StartColRef = pFilter->WriteRecord(Value.GetStartColour());
	INT32 EndColRef   = pFilter->WriteRecord(Value.GetEndColour());

	// Are the colour refs ok?
	BOOL ok = (StartColRef != 0) && (EndColRef != 0);
	
	return (ok);
}








/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrNoiseColourFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrNoiseColourFill::AttrNoiseColourFill()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Purpose:	Constructor for a texture fill.

********************************************************************************************/

AttrNoiseColourFill::AttrNoiseColourFill()
{
}

/********************************************************************************************

>	AttrNoiseColourFill::~AttrNoiseColourFill()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Purpose:	Destructor for a texture fill.

********************************************************************************************/

AttrNoiseColourFill::~AttrNoiseColourFill()
{
}


/********************************************************************************************

>	void AttrNoiseColourFill::GetDebugDetails(StringBase* Str)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrNoiseColourFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	AttrTextureColourFill::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nNoise Fill:\r\n"));
	(*Str) += TempStr;

	double Scale = ((NoiseFillAttribute*)GetAttributeValue())->GetGraininess().MakeDouble();

	TempStr._MakeMsg(TEXT("\r\n\nScale = #1%f "), 
					 (Scale));
	(*Str) += TempStr;
#endif
}

/********************************************************************************************

>	virtual UINT32 AttrNoiseColourFill::GetNodeSize() const

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrNoiseColourFill::GetNodeSize() const 
{     
	return sizeof(AttrNoiseColourFill);
}  


/********************************************************************************************

>	Node* AttrNoiseColourFill::SimpleCopy() 

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/01/97
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrNoiseColourFill::SimpleCopy()
{
	AttrNoiseColourFill* NodeCopy = new AttrNoiseColourFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  


/********************************************************************************************

  >	virtual BOOL AttrNoiseColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the fractal fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrNoiseColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	// Make sure the start & end colours are written out first.
	INT32 StartColRef = pFilter->WriteRecord(Value.GetStartColour());
	INT32 EndColRef   = pFilter->WriteRecord(Value.GetEndColour());

	// Are the colour refs ok?
	ok = (StartColRef != 0) && (EndColRef != 0);

	if (ok)
	{
		CamelotFileRecord Rec(pFilter,TAG_NOISEFILL,TAG_NOISEFILL_SIZE);

		if (ok) ok = Rec.Init();
		if (ok) ok = Rec.WriteCoord(Value.StartPoint);			// INT32,INT32	4,4
		if (ok) ok = Rec.WriteCoord(Value.EndPoint);			//				4,4
		if (ok) ok = Rec.WriteCoord(Value.EndPoint2);			//				4,4
		if (ok) ok = Rec.WriteReference(StartColRef);			//				4
		if (ok) ok = Rec.WriteReference(EndColRef);				//				4
		if (ok) ok = Rec.WriteFIXED16(Value.GetGraininess());	//				4
		if (ok) ok = Rec.WriteINT32(Value.GetSeed());			//				4
		if (ok) ok = Rec.WriteINT32(Value.GetFractalDPI());		//				4
		if (ok) ok = Rec.WriteBYTE(Value.GetTileable());		//				1		= 45
		if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());
		if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());

		if (ok) ok = pFilter->Write(&Rec);
	}

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrNoiseColourFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrNoiseColourFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}



/********************************************************************************************

  >	virtual BOOL AttrNoiseColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/9/2000
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes out colour definitions for this fill.
	SeeAlso:	BaseCamelotFilter::WriteRemainingAtomicTagDefinitions ()
				Layer::WriteAtomicNodesColourRefs ()

********************************************************************************************/

BOOL AttrNoiseColourFill::WriteColourDefinitions (BaseCamelotFilter* pFilter)
{
	// Make sure the start & end colours are written out first.
	INT32 StartColRef = pFilter->WriteRecord(Value.GetStartColour());
	INT32 EndColRef   = pFilter->WriteRecord(Value.GetEndColour());

	// Are the colour refs ok?
	BOOL ok = (StartColRef != 0) && (EndColRef != 0);
	
	return (ok);
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrTextureTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrTextureTranspFill::AttrTextureTranspFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Constructor for a fractal fill.

********************************************************************************************/

AttrTextureTranspFill::AttrTextureTranspFill()
{
}

/********************************************************************************************

>	AttrTextureTranspFill::~AttrTextureTranspFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Destructor for a fractal fill.

********************************************************************************************/

AttrTextureTranspFill::~AttrTextureTranspFill()
{
}

/********************************************************************************************

>	virtual BOOL AttrTextureTranspFill::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode

********************************************************************************************/

BOOL AttrTextureTranspFill::NeedsTransparency() const
{
	return TRUE;
}

/********************************************************************************************

> Node* AttrTextureTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrTextureTranspFill::SimpleCopy()
{
	AttrTextureTranspFill* NodeCopy = new AttrTextureTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrTextureTranspFill::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTextureTranspFill::GetAttrNameID(void)  
{
	
	return (_R(IDS_BITMAPTRANSPFILL));
}

UINT32 AttrFractalTranspFill::GetAttrNameID(void)  
{	
	return (_R(IDS_FRACTALCLOUDSTRANSPFILL));
}

UINT32 AttrNoiseTranspFill::GetAttrNameID(void)  
{	
	return (_R(IDS_FRACTALPLASMATRANSPFILL));
}

/********************************************************************************************

>	void AttrTextureTranspFill::GetDebugDetails(StringBase* Str)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/94
	Outputs:	Str - the string containing details of the attribute.
	Purpose:	Output details of this attribute to the Camelot debug tree dialog.

********************************************************************************************/

void AttrTextureTranspFill::GetDebugDetails(StringBase* Str)
{
#ifdef _DEBUG
	NodeAttribute::GetDebugDetails( Str );

	String_256 TempStr;

	TempStr._MakeMsg( TEXT("\r\nTexture transparency:\r\n"));
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nStart = (#1%ld, #2%ld)"), 
					 (*GetStartPoint()).x, (*GetStartPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\nEnd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint()).x, (*GetEndPoint()).y);
	(*Str) += TempStr;

	TempStr._MakeMsg(TEXT("\r\n3rd = (#1%ld, #2%ld) "), 
					 (*GetEndPoint2()).x, (*GetEndPoint2()).y);
	(*Str) += TempStr;
#endif
}



/********************************************************************************************

>	void AttrTextureTranspFill::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Fractal Fill Transp attribute.

********************************************************************************************/

void AttrTextureTranspFill::Render(RenderRegion* pRender)
{
	if (GetBitmap() == NULL || GetBitmap()->ActualBitmap == NULL)
	{
		if (!RecalcFractal())
		 	return;
	}

	pRender->SetTranspFillGeometry((TranspFillAttribute*)GetAttributeValue(), FALSE);
}


/********************************************************************************************

>	void AttrTextureTranspFill::RenderFillBlobs(RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/9/94
	Inputs:		pRender - The region to render the blobs to.
	Purpose:	Renders the grad fills arrow blob when requested to by its selected parent.

********************************************************************************************/

void AttrTextureTranspFill::RenderFillBlobs(RenderRegion* pRender)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!IsVisible())
		return; 	// We're Not in Fill Transparency Mode

	// Don't bother if this fill is being edited as a copy of it
	// we be rendered thoughout the drag op
	if (IsFillBeingEdited())
		return;

	// Ignore this if the mesh is the same as the last one rendered.
	if (CheckPreviousFillMesh())
		return;

	DocCoord ControlPoints[5];
	ControlPoints[FILLCONTROL_STARTPOINT] = (*GetStartPoint());
	ControlPoints[FILLCONTROL_ENDPOINT] = (*GetEndPoint());
	ControlPoints[FILLCONTROL_SECONDARYPOINT] = (*GetEndPoint2());

	// Render a nice pretty Fill Mesh thingy
	RenderFillMesh(pRender, ControlPoints, SelectionState, 5);

	// This call was removed by Gerry (2/9/96) as it causes blob problems
	// If we are removing blobs then force all blobs to be deselected
//	if ((Camelot.GetBlobManager())->IsRemovingBlobs())
//		DeselectAllNoRedraw();
#endif
}





/********************************************************************************************

>	FillControl AttrTextureTranspFill::CheckForControlHit(DocCoord &ClickPos)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/07/94
	Inputs:		ClickPos, The DocCoord position to check.
	Returns:	A FillControl, indicating the Fill Control Point Hit,
				or FILLCONTROL_NULL, if no points hit.
	Purpose:	Check to see if a click was on a Fill Control Point. 
	SeeAlso:	FillControl

********************************************************************************************/

FillControl AttrTextureTranspFill::CheckForControlHit(DocCoord &ClickPos)
{
#if !defined(EXCLUDE_FROM_RALPH)
	if (!GetApplication()->GetBlobManager()->GetCurrentInterest().Fill || !IsVisible())
		return FILLCONTROL_NULL;

	// Set up a default, that indicates not control points hit
	FillControl HitControl = FILLCONTROL_NULL;
	DocRect BlobRect;

	DocCoord StartPoint  = *GetStartPoint();
	DocCoord EndPoint    = *GetEndPoint();
	DocCoord EndPoint2   = *GetEndPoint2();

	// Bodge Alert !!
	
	// Bitmap fills have a complicated control point system.
	// The actual control points needed for rendering are the Bottom Left,
	// Top Left, and Bottom Right points of the bitmap.  But on screen the 
	// points seen and moved are the Centre, Middle Right, and Middle Top.
	// So we need to convert the click points into vitual points before
	// we do the hit test.

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	// Get the rectangle around the Centre Control Point
	(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_STARTPOINT;

	// Get the rectangle around the First End Point
	(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_ENDPOINT;

	// Get the rectangle around the Second End Point
	(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &BlobRect);
	// See if the Click Position is within the rectangle
	if ( BlobRect.ContainsCoord(ClickPos) )
		HitControl = FILLCONTROL_SECONDARYPOINT;

	return HitControl;
#else
	return FILLCONTROL_NULL;
#endif
}



/********************************************************************************************

>	virtual void AttrTextureTranspFill::OnControlDrag( DocCoord Pos, FillControl DragControl, 
													ClickModifiers ClickMods)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/8/94
	Inputs:		Pos - The Location of the mouse pointer at the time of the call
				DragControl - The FillControl that is being dragged.
				ClickMods - The modifiers to the click (eg shift, control etc )
	Purpose:	Called when an edit operation is dragging a fill control point.

********************************************************************************************/

void AttrTextureTranspFill::OnControlDrag( DocCoord Pos, FillControl& DragControl, ClickModifiers ClickMods)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Get the current Control Point Positions
	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	// Bodge Alert !!
	
	// Bitmap fills have a complicated control point system.
	// The actual control points needed for rendering are the Bottom Left,
	// Top Left, and Bottom Right points of the bitmap.  But on screen the 
	// points seen and moved are the Centre, Middle Right, and Middle Top.
	// So we need to convert the click points into vitual points before
	// we start dragging them about.

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	INT32 dx, dy;

	// Which control is being dragged ?
	switch (DragControl)
	{
		case FILLCONTROL_STARTPOINT:

			// They're dragging the Centre Point
			dx = CentrePoint.x - Pos.x;
			dy = CentrePoint.y - Pos.y;
			// Move the other points relative
			Drag2Blob.translate(-dx, -dy);
			Drag3Blob.translate(-dx, -dy);
			CentrePoint = Pos;
			break;

		case FILLCONTROL_ENDPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre position.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(CentrePoint, &Pos);

			if (ClickMods.Adjust)
			{
				// Shift is pressed, so lock the aspect ratio of the bitmap mesh
				double OldLen = CentrePoint.Distance(Drag2Blob);
				double NewLen = CentrePoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new point based on the aspect ratio
				Drag3Blob = 
						MakeLineAtAngle(CentrePoint, Pos, 90, INT32(CentrePoint.Distance(Drag3Blob) * Ratio));
			}

			Drag2Blob   = Pos;
			break;

		case FILLCONTROL_SECONDARYPOINT:

			// If the Constrain key is down then constrain the Angle of the
			// point, relative to the centre position.
			if (ClickMods.Constrain)
				DocView::ConstrainToAngle(CentrePoint, &Pos);

			if (ClickMods.Adjust)
			{
				// Shift is pressed, so lock the aspect ratio of the bitmap mesh
				double OldLen = CentrePoint.Distance(Drag3Blob);
				double NewLen = CentrePoint.Distance(Pos);
				double Ratio = 1.0;

				if (OldLen == 0)
					Ratio = 0;
				else
					Ratio = NewLen/OldLen;

				// Calculate the new point based on the aspect ratio
				Drag2Blob = 
						MakeLineAtAngle(CentrePoint, Pos, -90, INT32(CentrePoint.Distance(Drag2Blob) * Ratio));
			}

			Drag3Blob  = Pos;
			break;
	}

	// Now we have to convert the Vitual Coords back into real
	// bitmap control points.

	GetBitmapRealPoints(CentrePoint, Drag2Blob, Drag3Blob,
							&StartPoint, &EndPoint, &EndPoint2);

	// Store the new points back in the Fill
	SetStartPoint(&StartPoint);
	SetEndPoint(&EndPoint);
	SetEndPoint2(&EndPoint2);
#endif
}

/********************************************************************************************

>	void AttrTextureColourFill::RenderControl(FillControl Control, BOOL RenderOn)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/7/94
	Inputs:		Control, the FillControl to redraw
	Purpose:	Redraws the Fill Control blobs when the selection changes.
	SeeAlso:	FillControl			

********************************************************************************************/

void AttrTextureTranspFill::RenderControl(FillControl Control, BOOL RenderOn)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocRect ControlRect;

	// Ignore if we're not in the tree yet
	// We may be a tempory clone, or something
	NodeRenderable* pParent = (NodeRenderable*)FindParent();

	if (pParent == NULL)
		return;

	if (IsBlobSame(Control))
		return;			// Ignore if same as the last blob rendered

	Spread* pSpread = this->FindParentSpread();

	DocCoord StartPoint = *GetStartPoint();
	DocCoord EndPoint = *GetEndPoint();
	DocCoord EndPoint2 = *GetEndPoint2();

	DocCoord CentrePoint;
	DocCoord Drag2Blob;
	DocCoord Drag3Blob;

	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&CentrePoint, &Drag2Blob, &Drag3Blob);

	switch (Control)
	{
		case FILLCONTROL_STARTPOINT:

			if (GetStartPoint() != NULL)
			{
				// Redraw the Start Point Blob
				(Camelot.GetBlobManager())->GetBlobRect(CentrePoint, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_ENDPOINT:

			if (GetEndPoint() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;

		case FILLCONTROL_SECONDARYPOINT:

			if (GetEndPoint2() != NULL)
			{
				// Redraw BOTH End Point Blobs
				(Camelot.GetBlobManager())->GetBlobRect(Drag2Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}

			if (GetEndPoint2() != NULL)
			{
				(Camelot.GetBlobManager())->GetBlobRect(Drag3Blob, &ControlRect);
				RenderFillControl(RenderOn, &ControlRect, pSpread, pParent);
			}
			break;
	}
#endif
}



/********************************************************************************************

>	void AttrTextureTranspFill::RenderFillMesh(RenderRegion* pRender, 
									DocCoord* ControlPoints, BOOL* SelState,
									INT32 NumControlPoints)
	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/7/94
	Inputs:		pRender - The region to render the blobs to.
				ControlPoints - The positions of all the control points
				SelState - The selection state of the control points
				NumControlPoints - The Number of control points.
	Purpose:	Renders the grad fills mesh during a drag op.
				Don't call this, call RenderFillBlobs().
	SeeAlso:	AttrBitmapFill::RenderFillBlobs

********************************************************************************************/

void AttrTextureTranspFill::RenderFillMesh(RenderRegion* pRender, 
											DocCoord* ControlPoints, BOOL* SelState,
											INT32 NumControlPoints)
{
#if !defined(EXCLUDE_FROM_RALPH)
	DocCoord StartPoint = ControlPoints[FILLCONTROL_STARTPOINT];
	DocCoord EndPoint   = ControlPoints[FILLCONTROL_ENDPOINT];

	if (StartPoint == EndPoint)
		return;

	if (SelState == NULL)
	{
		// If no selection state passed in, then assume
		// all the points are deselected
		BOOL Selected[NUMCONTROLPOINTS];
		for (INT32 i=0; i< NumControlPoints; i++)
		{
			Selected[i] = FALSE;
		}
		SelState = Selected;
	}

	DocCoord EndPoint2  = ControlPoints[FILLCONTROL_SECONDARYPOINT];

	DocCoord Start;
	DocCoord End;
	DocCoord End2; 

	// Work out the Virtual positions of the Bitmap Controls
	GetBitmapVirtualPoints(StartPoint, EndPoint, EndPoint2,
							&Start, &End, &End2);

	// Remember what attributes were here before
	pRender->SaveContext();

	// Get the current blob size in Doc Units
	INT32 BlobSize = (Camelot.GetBlobManager())->GetBlobSize();

	// Calculate the Arrow on the End of the Line
	Path ArrowPath;
	ArrowPath.Initialise();
	DocCoord LineEnd;
	MakeMeshArrow(&ArrowPath, Start, End, &LineEnd);

	// Calculate the Arrow on the End of the Line2
	Path ArrowPath2;
	ArrowPath2.Initialise();
	DocCoord LineEnd2;
	MakeMeshArrow(&ArrowPath2, Start, End2, &LineEnd2);

	// Set the line colours etc as we need them
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// First Draw the Line
	pRender->SetLineWidth(BlobSize/4);
	pRender->DrawLine(Start, LineEnd);

	// Draw the secondary line
	pRender->DrawLine(Start, LineEnd2);

	// Render an Arrow at the end of the line
	pRender->SetLineWidth(0);
	pRender->SetLineColour(COLOUR_NONE);
	pRender->DrawPath(&ArrowPath);

	// and on the end of the secondary line
	pRender->DrawPath(&ArrowPath2);

	if (DraggedFill == this)
	{
		// If we are being dragged then draw a Parallelgram to
		// show the shape of the bitmap
		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetFillColour(COLOUR_NONE);
	
		Path ParaPath;
		ParaPath.Initialise();
		MakeMeshParallelagram(&ParaPath, Start, End, End2);

		// Draw the parallelagram
		pRender->DrawPath(&ParaPath);
	}

	// Now Render the blobs on the path

	// Draw a blob at the start point
	if (SelState[FILLCONTROL_STARTPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(Start, BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(Start, BT_UNSELECTED);
	}

	// Draw a blob at the end point
	if (SelState[FILLCONTROL_ENDPOINT])
	{
		// Draw Selected Blob
//		pRender->SetLineColour(COLOUR_SELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_SELECTEDBLOB);
		pRender->DrawBlob(End, BT_SELECTED);
		pRender->DrawBlob(End2,BT_SELECTED);
	}
	else
	{
		// Draw Unselected Blob
//		pRender->SetLineColour(COLOUR_UNSELECTEDBLOB);
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);
		pRender->DrawBlob(End, BT_UNSELECTED);
		pRender->DrawBlob(End2,BT_UNSELECTED);
	}

	// Put all the old attributes back
	pRender->RestoreContext();
#endif
}



/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFractalTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrFractalTranspFill::AttrFractalTranspFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Constructor for a transparency fill.

********************************************************************************************/

AttrFractalTranspFill::AttrFractalTranspFill()
{
	SetFractalDPI(AttrFillGeometry::FractalDPI);
}

/********************************************************************************************

>	AttrFractalColourFill::~AttrFractalColourFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Destructor for a transparency fill.

********************************************************************************************/

AttrFractalTranspFill::~AttrFractalTranspFill()
{
}

/********************************************************************************************

>	virtual UINT32 AttrFractalTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFractalTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrFractalTranspFill);
}  

/********************************************************************************************

> Node* AttrFractalTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFractalTranspFill::SimpleCopy()
{
	AttrFractalTranspFill* NodeCopy = new AttrFractalTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

  >	virtual BOOL AttrFractalTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the fractal transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFractalTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_FRACTALTRANSPARENTFILL,TAG_FRACTALTRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);			// INT32,INT32  4,4
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);			//			  4,4
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);			//			  4,4

	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));			//				1
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));		//				1
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));		//				1

	if (ok) ok = Rec.WriteINT32(Value.Seed);					//				4
	if (ok) ok = Rec.WriteFIXED16(Value.Graininess);		//				4
	if (ok) ok = Rec.WriteFIXED16(Value.Gravity);			//				4
	if (ok) ok = Rec.WriteFIXED16(Value.Squash);			//				4
	if (ok) ok = Rec.WriteINT32(Value.Dpi);					//				4
	if (ok) ok = Rec.WriteBYTE(Value.Tileable);				//				1	= 48
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());		// 8
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());		// 8

	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFractalTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFractalTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL AttrFractalTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrFractalTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrNoiseTranspFill
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	AttrNoiseTranspFill::AttrNoiseTranspFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Constructor for a transparency fill.

********************************************************************************************/

AttrNoiseTranspFill::AttrNoiseTranspFill()
{
	SetFractalDPI(AttrFillGeometry::FractalDPI);
}

/********************************************************************************************

>	AttrFractalColourFill::~AttrFractalColourFill()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/9/94
	Purpose:	Destructor for a transparency fill.

********************************************************************************************/

AttrNoiseTranspFill::~AttrNoiseTranspFill()
{
}

/********************************************************************************************

>	virtual UINT32 AttrNoiseTranspFill::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node.
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrNoiseTranspFill::GetNodeSize() const 
{     
	return sizeof(AttrNoiseTranspFill);
}  

/********************************************************************************************

> Node* AttrNoiseTranspFill::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrNoiseTranspFill::SimpleCopy()
{
	AttrNoiseTranspFill* NodeCopy = new AttrNoiseTranspFill();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

  >	virtual BOOL AttrNoiseTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the fractal transparent fill record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrNoiseTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	BOOL ok = TRUE;

	CamelotFileRecord Rec(pFilter,TAG_NOISETRANSPARENTFILL,TAG_NOISETRANSPARENTFILL_SIZE);

	if (ok) ok = Rec.Init();
	if (ok) ok = Rec.WriteCoord(Value.StartPoint);				// INT32, INT32 = 4,4
	if (ok) ok = Rec.WriteCoord(Value.EndPoint);				//				4,4
	if (ok) ok = Rec.WriteCoord(Value.EndPoint2);				//				4,4

	if (ok) ok = Rec.WriteBYTE(BYTE(Value.Transp));				//				1
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.EndTransp));			//				1
	if (ok) ok = Rec.WriteBYTE(BYTE(Value.TranspType));			//				1

	if (ok) ok = Rec.WriteFIXED16(Value.GetGraininess());		//				4
	if (ok) ok = Rec.WriteINT32(Value.GetSeed());				//				4
	if (ok) ok = Rec.WriteINT32(Value.GetFractalDPI());			//				4
	if (ok) ok = Rec.WriteBYTE(Value.GetTileable());			//				1		= 40
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetBias ());		// 8
	if (ok) ok = Rec.WriteDOUBLE ((double) GetProfile ().GetGain ());		// 8

	if (ok) ok = pFilter->Write(&Rec);

	if (!ok)
		pFilter->GotError(_R(IDE_FILE_WRITE_ERROR));

	return ok;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrNoiseTranspFill::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrNoiseTranspFill::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}




/********************************************************************************************
>	BOOL AttrNoiseTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/05/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrNoiseTranspFill::HasEquivalentDefaultValue(BOOL bAppearance)
{
	// Slight bodge - we will assume that the default transparency is fully opaque
	if (bAppearance)
		return (Value.TranspType==TT_NoTranspType || (Value.TranspType==TT_Mix && Value.Transp == 0 && Value.EndTransp==0));
	else
		return FALSE;
}




/////////////////////////////////////////////////////////////////////////////////////////////
//
//								AttrFillMapping
//
/////////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************************************

>	INT32 AttrFillMapping::operator==(const NodeAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrFillMappingLinear 
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ERROR3IF failure will occur if Attrib does not have a AttrFillMappingLinear 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrFillMapping::operator==(const NodeAttribute& Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrFillMapping)), 
				"Trying to compare two objects with different types"); 

	return (GetRepeat() == ((AttrFillMapping*)&Attrib)->GetRepeat());
}

/********************************************************************************************

>	virtual NodeAttribute* AttrFillMapping::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrFillMapping::GetOtherAttrToApply(BOOL* IsMutate)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// If the fill mapping is changing, then we must make sure that
	// any fractal tileable flags are updated
	
	NodeAttribute* OtherAttr = new AttrFractalTileableChange;
	if (OtherAttr == NULL)
		return NULL;

	((AttrValueChange*)OtherAttr)->MutateColourFills(TRUE);

	INT32 Repeat = GetRepeat();
	BOOL Tile;

	switch (Repeat)
	{
		case RT_Simple:
			Tile = FALSE;
			break;

		case RT_Repeating:
			Tile = TRUE;
			break;

		case RT_RepeatInverted:
			Tile = FALSE;
			break;

		default:
			Tile = TRUE;
			break;
	}

	((AttrValueChange*)OtherAttr)->SetTileable(Tile);

	*IsMutate = TRUE;

	return OtherAttr;
#else
	return NULL;
#endif
}

//Mark Howitt, 7/10/97. Display Repeat Value info in the camelot debug tree
#ifdef _DEBUG
void AttrFillMapping::GetDebugDetails(StringBase* Str)
{
	NodeAttribute::GetDebugDetails(Str);
	(*Str) += TEXT("\r\nRepeating Value = ");
	switch(GetRepeat())
	{
		case 0 :
			(*Str) += TEXT("Flat.");
			break;
		case 1 :
			(*Str) += TEXT("Simple.");
			break;
		case 2 :
			(*Str) += TEXT("Repeating.");
			break;
		case 3 :
			(*Str) += TEXT("InvRepeating.");
			break;
		case 4 :
			(*Str) += TEXT("Repeating Extra.");
			break;
		default :
			(*Str) += TEXT("Not Reconized!");
			break;
	}
	(*Str) += TEXT("\r\n");
}
#endif
//

/********************************************************************************************

>	void AttrFillMappingLinear::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a linear graduation fill mapping attribute.

********************************************************************************************/

void AttrFillMappingLinear::Render( RenderRegion* pRender)
{
	pRender->SetFillMapping(&Value, FALSE);
}


/********************************************************************************************

> Node* AttrFillMappingLinear::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFillMappingLinear::SimpleCopy()
{
	AttrFillMappingLinear* NodeCopy = new AttrFillMappingLinear;
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFillMappingLinear::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFillMappingLinear::GetAttrNameID(void)  
{
	return (_R(IDS_FILLMAPPINGLINEAR)); 
}                                  


/********************************************************************************************

>	BOOL AttrFillMappingLinear::CopyNodeContents(AttrFillMappingLinear* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFillMappingLinear::CopyNodeContents(AttrFillMappingLinear* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrFillMappingLinear::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrFillMappingLinear::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFillMappingLinear::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrFillMappingLinear), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrFillMappingLinear))
		CopyNodeContents((AttrFillMappingLinear*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrFillMappingLinear::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFillMappingLinear::GetNodeSize() const 
{     
	return (sizeof(AttrFillMappingLinear)); 
}  




/********************************************************************************************

  >	virtual BOOL AttrFillMappingLinear::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the linear fill mapping record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFillMappingLinear::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	UINT32 Tag = TAG_UNDEFINED, TagSize = 1;
	switch (Value.Repeat)
	{
		case 0 :
		case 1 :	Tag		= TAG_FILL_NONREPEATING;
					TagSize = TAG_FILL_NONREPEATING_SIZE;
					break;
		case 2 :	Tag		= TAG_FILL_REPEATING;
					TagSize = TAG_FILL_REPEATING_SIZE;
					break;
		case 3 :	Tag		= TAG_FILL_REPEATINGINVERTED;
					TagSize = TAG_FILL_REPEATINGINVERTED_SIZE;
					break;
//Mark Howitt. 8/10/97. Make repeating grad fills a special case
#ifdef NEW_FEATURES
		case 4 :	Tag		= TAG_FILL_REPEATING_EXTRA;
					TagSize = TAG_FILL_REPEATING_EXTRA_SIZE;
					break;
#endif
		default:
			ERROR3("Unknown repeat type in this fill mapping attr");
			return FALSE;
			break;
	}

	CamelotFileRecord Rec(pFilter,Tag,TagSize);
	return pFilter->Write(&Rec);
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFillMappingLinear::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFillMappingLinear::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	void AttrFillMappingSin::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a sinusoidal graduation fill mapping attribute.

********************************************************************************************/

void AttrFillMappingSin::Render( RenderRegion* pRender)
{
	pRender->SetFillMapping(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrFillMappingSin::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFillMappingSin::SimpleCopy()
{
	AttrFillMappingSin* NodeCopy = new AttrFillMappingSin();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFillMappingSin::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFillMappingSin::GetAttrNameID(void)  
{
	return (_R(IDS_FILLMAPPINGSIN)); 
}                                  


/********************************************************************************************

>	BOOL AttrFillMappingSin::CopyNodeContents(AttrFillMappingSin* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFillMappingSin::CopyNodeContents(AttrFillMappingSin* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrFillMappingSin::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrFillMappingSin::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFillMappingSin::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrFillMappingSin), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrFillMappingSin))
		CopyNodeContents((AttrFillMappingSin*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrFillMappingSin::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFillMappingSin::GetNodeSize() const 
{     
	return (sizeof(AttrFillMappingSin)); 
}  


/********************************************************************************************

  >	virtual BOOL AttrFillMappingSin::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the sine fill mappine record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFillMappingSin::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	ERROR3("v2 file format does not support this attribute");
	return FALSE;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFillMappingSin::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFillMappingSin::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	INT32 AttrFillEffect::operator==(const NodeAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib  - the attribute to compare, which must be an AttrFillEffectFade 
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ERROR3IF failure will occur if Attrib does not have a AttrFillEffectFade 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrFillEffect::operator==(const NodeAttribute& Attrib)
{
	// Always equal
	return TRUE;
}

/********************************************************************************************

>	void AttrFillEffectFade::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a RGB fade fill effect attribute.

********************************************************************************************/

void AttrFillEffectFade::Render( RenderRegion* pRender)
{
	pRender->SetFillEffect(&Value, FALSE);
}


/********************************************************************************************

> Node* AttrFillEffectFade::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFillEffectFade::SimpleCopy()
{
	AttrFillEffectFade* NodeCopy = new AttrFillEffectFade();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFillEffectFade::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFillEffectFade::GetAttrNameID(void)  
{
	return (_R(IDS_FILLEFFECTFADE)); 
}                                  


/********************************************************************************************

>	BOOL AttrFillEffectFade::CopyNodeContents(AttrFillEffectFade* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFillEffectFade::CopyNodeContents(AttrFillEffectFade* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrFillEffectFade::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrFillEffectFade::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFillEffectFade::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrFillEffectFade), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrFillEffectFade))
		CopyNodeContents((AttrFillEffectFade*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrFillEffectFade::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFillEffectFade::GetNodeSize() const 
{     
	return (sizeof(AttrFillEffectFade)); 
}  


/********************************************************************************************

  >	virtual BOOL AttrFillEffectFade::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the fade fill effect record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFillEffectFade::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_FILLEFFECT_FADE,TAG_FILLEFFECT_FADE_SIZE);
	return pFilter->Write(&Rec);
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFillEffectFade::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFillEffectFade::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	void AttrFillEffectRainbow::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a rainbow fill effect attribute.

********************************************************************************************/

void AttrFillEffectRainbow::Render( RenderRegion* pRender)
{
	pRender->SetFillEffect(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrFillEffectRainbow::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFillEffectRainbow::SimpleCopy()
{
	AttrFillEffectRainbow* NodeCopy = new AttrFillEffectRainbow();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFillEffectRainbow::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFillEffectRainbow::GetAttrNameID(void)  
{
	return (_R(IDS_FILLEFFECTRAINBOW)); 
}                                  


/********************************************************************************************

>	BOOL AttrFillEffectRainbow::CopyNodeContents(AttrFillEffectRainbow* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFillEffectRainbow::CopyNodeContents(AttrFillEffectRainbow* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrFillEffectRainbow::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrFillEffectRainbow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFillEffectRainbow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrFillEffectRainbow), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrFillEffectRainbow))
		CopyNodeContents((AttrFillEffectRainbow*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrFillEffectRainbow::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFillEffectRainbow::GetNodeSize() const 
{     
	return (sizeof(AttrFillEffectRainbow)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrFillEffectRainbow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the rainbow fill effect record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFillEffectRainbow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_FILLEFFECT_RAINBOW,TAG_FILLEFFECT_RAINBOW_SIZE);
	return pFilter->Write(&Rec);
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFillEffectRainbow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFillEffectRainbow::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	void AttrFillEffectAltRainbow::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' an alternate rainbow fill effect attribute.

********************************************************************************************/

void AttrFillEffectAltRainbow::Render( RenderRegion* pRender)
{
	pRender->SetFillEffect(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrFillEffectAltRainbow::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrFillEffectAltRainbow::SimpleCopy()
{
	AttrFillEffectAltRainbow* NodeCopy = new AttrFillEffectAltRainbow();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrFillEffectAltRainbow::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrFillEffectAltRainbow::GetAttrNameID(void)  
{
	return (_R(IDS_FILLEFFECTALTRAINBOW)); 
}                                  


/********************************************************************************************

>	BOOL AttrFillEffectAltRainbow::CopyNodeContents(AttrFillEffectAltRainbow* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrFillEffectAltRainbow::CopyNodeContents(AttrFillEffectAltRainbow* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrFillEffectAltRainbow::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrFillEffectAltRainbow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrFillEffectAltRainbow::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrFillEffectAltRainbow), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrFillEffectAltRainbow))
		CopyNodeContents((AttrFillEffectAltRainbow*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrFillEffectAltRainbow::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrFillEffectAltRainbow::GetNodeSize() const 
{     
	return (sizeof(AttrFillEffectAltRainbow)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrFillEffectAltRainbow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the alt rainbow fill effect record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrFillEffectAltRainbow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	CamelotFileRecord Rec(pFilter,TAG_FILLEFFECT_ALTRAINBOW,TAG_FILLEFFECT_ALTRAINBOW_SIZE);
	return pFilter->Write(&Rec);
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrFillEffectAltRainbow::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrFillEffectAltRainbow::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	INT32 AttrTranspFillMapping::operator==(const NodeAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrTranspFillMappingLinear 
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ERROR3IF failure will occur if Attrib does not have a AttrTranspFillMappingLinear 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrTranspFillMapping::operator==(const NodeAttribute& Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrTranspFillMapping)), 
				"Trying to compare two objects with different types"); 

	return (GetRepeat() == ((AttrTranspFillMapping*)&Attrib)->GetRepeat());
}

/********************************************************************************************

>	virtual NodeAttribute* AttrTranspFillMapping::GetOtherAttrToApply(BOOL* IsMutate)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/8/95
	Returns:	The secondary attribute to apply, or NULL if none to apply
	Purpose:	Some attributes require a secondary atribute to be changed when they are
				changed.  This routine obtains a pointer to the secondary attribute to
				apply.

********************************************************************************************/

NodeAttribute* AttrTranspFillMapping::GetOtherAttrToApply(BOOL* IsMutate)
{
#if !defined(EXCLUDE_FROM_RALPH)
	ERROR3IF(IsMutate == NULL, "NULL flag pointer passed to GetOtherAttrToApply");

	// If the fill mapping is changing, then we must make sure that
	// any fractal tileable flags are updated
	
	NodeAttribute* OtherAttr = new AttrFractalTileableChange;
	if (OtherAttr == NULL)
		return NULL;

	((AttrValueChange*)OtherAttr)->MutateTranspFills(TRUE);

	INT32 Repeat = GetRepeat();
	BOOL Tile;

	switch (Repeat)
	{
		case RT_Simple:
			Tile = FALSE;
			break;

		case RT_Repeating:
			Tile = TRUE;
			break;

		case RT_RepeatInverted:
			Tile = FALSE;
			break;

		default:
			Tile = TRUE;
			break;
	}

	((AttrValueChange*)OtherAttr)->SetTileable(Tile);

	*IsMutate = TRUE;

	return OtherAttr;
#else
	return NULL;
#endif
}

/********************************************************************************************

>	void AttrTranspFillMappingLinear::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a linear graduation FillTransp mapping attribute.

********************************************************************************************/

void AttrTranspFillMappingLinear::Render( RenderRegion* pRender)
{
	pRender->SetTranspFillMapping(&Value, FALSE);
}


/********************************************************************************************

> Node* AttrTranspFillMappingLinear::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrTranspFillMappingLinear::SimpleCopy()
{
	AttrTranspFillMappingLinear* NodeCopy = new AttrTranspFillMappingLinear;
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrTranspFillMappingLinear::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTranspFillMappingLinear::GetAttrNameID(void)  
{
	return (_R(IDS_FILLMAPPINGLINEAR)); 
}                                  


/********************************************************************************************

>	BOOL AttrTranspFillMappingLinear::CopyNodeContents(AttrTranspFillMappingLinear* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrTranspFillMappingLinear::CopyNodeContents(AttrTranspFillMappingLinear* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrTranspFillMappingLinear::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrTranspFillMappingLinear::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTranspFillMappingLinear::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTranspFillMappingLinear), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTranspFillMappingLinear))
		CopyNodeContents((AttrTranspFillMappingLinear*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTranspFillMappingLinear::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTranspFillMappingLinear::GetNodeSize() const 
{     
	return (sizeof(AttrTranspFillMappingLinear)); 
}  



/********************************************************************************************

  >	virtual BOOL AttrTranspFillMappingLinear::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the linear fill mapping record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrTranspFillMappingLinear::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	UINT32 Tag = TAG_UNDEFINED, TagSize = 1;
	switch (Value.Repeat)
	{
		case 0 :
		case 1 :	Tag		= TAG_TRANSPARENTFILL_NONREPEATING;
					TagSize = TAG_TRANSPARENTFILL_NONREPEATING_SIZE;
					break;
		case 2 :	Tag		= TAG_TRANSPARENTFILL_REPEATING;
					TagSize = TAG_TRANSPARENTFILL_REPEATING_SIZE;
					break;
		case 3 :	Tag		= TAG_TRANSPARENTFILL_REPEATINGINVERTED;
					TagSize = TAG_TRANSPARENTFILL_REPEATINGINVERTED_SIZE;
					break;
	//Mark Howitt. 8/10/97. (well actually Chris Snook - 8/12/99); cause we need to do it here
	//as well - Make repeating grad fills a special case
#ifdef NEW_FEATURES
		case 4 :	Tag		= TAG_TRANSPARENTFILL_REPEATING_EXTRA;
					TagSize = TAG_TRANSPARENTFILL_REPEATING_EXTRA_SIZE;
					break;
#endif

		default:
			ERROR3("Unknown repeat type in this fill mapping attr");
			return FALSE;
			break;
	}

	CamelotFileRecord Rec(pFilter,Tag,TagSize);
	return pFilter->Write(&Rec);
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrTranspFillMappingLinear::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrTranspFillMappingLinear::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************
>	BOOL AttrTranspFillMappingLinear::HasEquivalentDefaultValue(BOOL bAppearance = FALSE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/09/2005
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node has a value equivalent to the relevant 
				FALSE otherwise
	Purpose:	Determine whether this attribute has the default value or not
	Errors:		-
	SeeAlso:	-
********************************************************************************************/

BOOL AttrTranspFillMappingLinear::HasEquivalentDefaultValue(BOOL bAppearance)
{
	return (Value.Repeat==2);
}




/********************************************************************************************

>	void AttrTranspFillMappingSin::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a sinusoidal graduation FillTransp mapping attribute.

********************************************************************************************/

void AttrTranspFillMappingSin::Render( RenderRegion* pRender)
{
	pRender->SetTranspFillMapping(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrTranspFillMappingSin::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrTranspFillMappingSin::SimpleCopy()
{
	AttrTranspFillMappingSin* NodeCopy = new AttrTranspFillMappingSin();
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrTranspFillMappingSin::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrTranspFillMappingSin::GetAttrNameID(void)  
{
	return (_R(IDS_FILLMAPPINGSIN)); 
}                                  


/********************************************************************************************

>	BOOL AttrTranspFillMappingSin::CopyNodeContents(AttrTranspFillMappingSin* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrTranspFillMappingSin::CopyNodeContents(AttrTranspFillMappingSin* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrTranspFillMappingSin::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 


/***********************************************************************************************
>   void AttrTranspFillMappingSin::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrTranspFillMappingSin::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrTranspFillMappingSin), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrTranspFillMappingSin))
		CopyNodeContents((AttrTranspFillMappingSin*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrTranspFillMappingSin::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrTranspFillMappingSin::GetNodeSize() const 
{     
	return (sizeof(AttrTranspFillMappingSin)); 
}  






/********************************************************************************************

  >	virtual BOOL AttrTranspFillMappingSin::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pFilter = ptr to the filter
	Returns:	TRUE if record is written, FALSE if not
	Purpose:	Writes the transparent sine fill mapping record to the filter
	SeeAlso:	-

********************************************************************************************/

BOOL AttrTranspFillMappingSin::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	ERROR2IF(pFilter == NULL,FALSE,"NULL filter param");

	ERROR3("v2 file format does not support this attribute");
	return FALSE;
#else
	return FALSE;
#endif
}

//--------------------------------------------------------------
// See AttrTranspFillMappingSin::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
//
BOOL AttrTranspFillMappingSin::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
#ifdef DO_EXPORT
	return WritePreChildrenWeb(pFilter);
#else
	return FALSE;
#endif
}


/********************************************************************************************

>	AttrMould::AttrMould(NodeMould* pMould, MouldGeometry* pMouldShape)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/02/94
	Inputs:		pMould - the Mould to associate with this attribute
	Purpose:	Constructor for Mould attribute.

********************************************************************************************/

AttrMould::AttrMould(NodeMould* pMould, MouldGeometry* pMouldShape)
{
	SetParentMould(pMould);
	SetMouldShape(pMouldShape);
}

/********************************************************************************************

>	INT32 AttrMould::operator==(const NodeAttribute& Attrib)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		Attrib - the attribute to compare, which must be an AttrMouldLinear 
	Returns:	Usual semantics for equality.
	Purpose:	A virtual comparison operator. See NodeAttribute::operator== for 
				a description of why it's required. 
	Errors:		An ERROR3IF failure will occur if Attrib does not have a AttrMouldLinear 
				runtime class.
	SeeAlso:	NodeAttribute::operator==

********************************************************************************************/

INT32 AttrMould::operator==(const NodeAttribute& Attrib)
{
	ERROR3IF(!Attrib.IsKindOf(CC_RUNTIME_CLASS(AttrMould)), 
				"Trying to compare two objects with different types"); 

	return (GetParentMould() == ((AttrMould*)&Attrib)->GetParentMould());
}

/********************************************************************************************

>	void AttrMould::Render( RenderRegion* pRender)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Purpose:	'Renders' a Mould attribute.

********************************************************************************************/

void AttrMould::Render( RenderRegion* pRender)
{
//	pRender->SetMould(&Value, FALSE);
}

/********************************************************************************************

> Node* AttrMould::SimpleCopy() 

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
    Returns:    A copy of the node, or NULL if memory runs out 
    Purpose:    This method returns a shallow copy of the node with all Node pointers NULL. 
				The function is virtual, and must be defined for all derived classes.  
 	Errors:     If memory runs out when trying to copy, then ERROR is called with an out of 
 				memory error and the function returns NULL. 
	Scope:      protected       

********************************************************************************************/
     
Node* AttrMould::SimpleCopy()
{
	AttrMould* NodeCopy = new AttrMould;
	if (NodeCopy == NULL)
		return NULL;

	CopyNodeContents(NodeCopy);
    
	return NodeCopy;
}  

/********************************************************************************************

>	virtual UINT32 AttrMould::GetAttrNameID(void)  

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	Attribute description ID
	Purpose:	Returns a string resource ID describing the attribute

********************************************************************************************/

UINT32 AttrMould::GetAttrNameID(void)  
{
	return (_R(IDS_ATTRMOULD)); 
}                                  

/********************************************************************************************

>	BOOL AttrMould::CopyNodeContents(AttrMould* NodeCopy)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Inputs:		NodeCopy - the node to copy.
    Outputs:    A copy of this node
    Purpose:    This method copies the node's contents to the node pointed to by NodeCopy.
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
********************************************************************************************/

BOOL AttrMould::CopyNodeContents(AttrMould* NodeCopy)
{
	ERROR2IF(NodeCopy == NULL, FALSE, "NULL pointer in AttrMould::CopyNodeContents!");

	NodeAttribute::CopyNodeContents(NodeCopy);

	// Copy contents specific to derived class here
	NodeCopy->Value.SimpleCopy(&Value);
	return TRUE;
} 

/***********************************************************************************************
>   void AttrMould::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void AttrMould::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, AttrMould), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, AttrMould))
		CopyNodeContents((AttrMould*)pNodeCopy);
}



/********************************************************************************************

>	virtual UINT32 AttrMould::GetNodeSize() const

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/8/94
	Returns:	The size of the node in bytes
	Purpose:	For finding the size of the node 
	SeeAlso:	Node::GetSubtreeSize

********************************************************************************************/

UINT32 AttrMould::GetNodeSize() const 
{     
	return (sizeof(AttrMould)); 
}  




/********************************************************************************************

			Blending Bits

********************************************************************************************/

BOOL AttrFillGeometry::Blend(BlendAttrParam* pBlendParam)
{
	// Check entry param isn't NULL
	ERROR3IF(pBlendParam == NULL,"NULL entry param");
	if (pBlendParam == NULL) return FALSE;

	// Get the Value member to blend to the Value member of the other NodeAttribute.
	// If it succeeds, ask the blended Value to make a NodeAttribute out of itself.

	if (GetAttributeValue()->Blend(pBlendParam))
	{
		// Get the blended attr val. After this call, the ptr is our reponsibility
		// so we have to delete it if it's no longer needed
		AttributeValue* pBlendedAttrVal = pBlendParam->GetBlendedAttrVal();

		if (pBlendedAttrVal != NULL)
		{
			// We have a blended attr val, so ask it to make a NodeAttribute out of itself
			// and set the pBlendParam's blended NodeAttribute ptr to it
			NodeAttribute* pBlendedAttr = pBlendedAttrVal->MakeNode();
			pBlendParam->SetBlendedAttr(pBlendedAttr);

			if (pBlendedAttr != NULL)
			{
				// We were able to make a blended NodeAttribute
				// so delete the blended attr val, and return TRUE
				delete pBlendedAttrVal;
				return TRUE;
			}
			else
			{
				// Couldn't make a blended NodeAttribute, so give the blended attr val back
				// and return FALSE
				pBlendParam->SetBlendedAttrVal(pBlendedAttrVal);
				return FALSE;
			}
		}
	}

	return FALSE;
}



//===========================================================================================
//
//	Karim MacDonald 08/12/1999
//	Extending Bits
//
//===========================================================================================

/********************************************************************************************

>	DocRect AttrFillGeometry::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE	if this fill is unextendible (it is not graduated), or if it may be
						reversible extended.
				FALSE	otherwise.
	Purpose:	Determine whether applying an extend to this Node is a reversible process.
	Errors:		
	See also:	class Extender; IsTypeExtendible(); Extend()

********************************************************************************************/
DocRect AttrFillGeometry::ValidateExtend(const ExtendParams& ExtParams)
{
	// quit immediately with all-clear if we are not a graduated fill.
	if (!IsAGradFill())
		return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);

	// construct an array of the fill's handles.
	INT32 i = 0;
	DocCoord doccHandles[3];
	DocCoord* pDocc = GetStartPoint();
	if (pDocc != NULL)
		doccHandles[i ++] = *pDocc;

	pDocc = GetEndPoint();
	if (pDocc != NULL)
		doccHandles[i ++] = *pDocc;

	pDocc = GetEndPoint2();
	if (pDocc != NULL)
		doccHandles[i ++] = *pDocc;

	// validate our array of fill handles.
	// don't bother checking children - attributes don't have any.
	INT32 numPoints = GetNumberOfControlPoints();
	return Extender::ValidateControlPoints(numPoints, doccHandles, ExtParams);
}



/********************************************************************************************

>	void AttrFillGeometry::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	03/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	This fill node may have its dimensions altered by the extension.
	Returns:	
	Purpose:	Perform an extend operation on this fill node.
	Errors:		
	See also:	class Extender; IsTypeExtendible(); Extend()

********************************************************************************************/
void AttrFillGeometry::Extend(const ExtendParams& ExtParams)
{
	// quit immediately if we are not a graduated fill.
	if (!IsAGradFill())
		return;

	// we only need to do the extend operation, as whatever we're applied to
	// takes care of translating/stretching us.
	Node* pParent = FindParent();
	if (pParent != NULL)
		if (pParent->IsNodePath() || pParent->IsCompound())
			TransformTranslateObject(ExtParams);
}



/********************************************************************************************

>	virtual void AttrFillGeometry::TransformTranslateObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	01/12/1999
	Inputs:		ExtParams		description of how this path should extend.
	Outputs:	This path's control points may be extended, depending on ExtParams' flags.
	Purpose:	Do an extend (as opposed to stretch) operation on this fill, using ExtParams
				as the source of the extend data, together with the extend-centre of this
				path, defined in NodeRenderable::FindExtendCentre() and overridden here also.

				Because whatever we're applied to has already translated us by ExtParams'
				doccOffset parameter, we usually skip that particular step in the process,
				with the exception of our parent being a group, in which case it won't have
				applied any transformation directly to us.

				This function does nothing unless ExtParams uses X_EXTEND or Y_EXTEND.
	See also:	The Extender class; FindExtendCentre().

********************************************************************************************/
void AttrFillGeometry::TransformTranslateObject(const ExtendParams& ExtParams)
{
	// construct an array of the fill's handles.
	INT32 pointIndex = 0;
	DocCoord* doccpHandles[3];
	DocCoord* pDocc = GetStartPoint();
	if (pDocc != NULL)
		doccpHandles[pointIndex ++] = pDocc;

	pDocc = GetEndPoint();
	if (pDocc != NULL)
		doccpHandles[pointIndex ++] = pDocc;

	pDocc = GetEndPoint2();
	if (pDocc != NULL)
		doccpHandles[pointIndex ++] = pDocc;

	INT32 numPoints = GetNumberOfControlPoints();

	// x-extension behaviour.
	if (ExtParams.fExtendFlags & X_EXTEND)
	{
		// if our parent is a group, then it won't have applied any offset parameter to us,
		// so we need to do this ourself.
		Node* pParent = FindParent();
		if (pParent != NULL && pParent->IsCompound())
		{
			Trans2DMatrix baseXoffset(ExtParams.doccOffset.x, 0);
			Transform(baseXoffset);
		}

		for (INT32 i = 0; i < numPoints; i ++)
		{
			if (doccpHandles[i]->x > ExtParams.doccEndCentre.x + ExtParams.xincExtendBuffer)
				doccpHandles[i]->x += ExtParams.xinc;
			else if (doccpHandles[i]->x < ExtParams.doccEndCentre.x - ExtParams.xdecExtendBuffer)
				doccpHandles[i]->x -= ExtParams.xdec;
		}
	}

	// y-extension behaviour.
	if (ExtParams.fExtendFlags & Y_EXTEND)
	{
		// if our parent is a group, then it won't have applied any offset parameter to us,
		// so we need to do this ourself.
		Node* pParent = FindParent();
		if (pParent != NULL && pParent->IsCompound())
		{
			Trans2DMatrix baseYoffset(0, ExtParams.doccOffset.y);
			Transform(baseYoffset);
		}

		for (INT32 i = 0; i < numPoints; i ++)
		{
			if (doccpHandles[i]->y > ExtParams.doccEndCentre.y + ExtParams.yincExtendBuffer)
				doccpHandles[i]->y += ExtParams.yinc;
			else if (doccpHandles[i]->y < ExtParams.doccEndCentre.y - ExtParams.ydecExtendBuffer)
				doccpHandles[i]->y -= ExtParams.ydec;
		}
	}

	// doing this should cause the fourth fill point to be reset to a sensible value.
	SetEndPoint3(NULL);
}



/********************************************************************************************

>	void AttrLinearFill::TransformTranslateObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/2000

	Purpose:	Override of AttrFillGeometry::TransformTranslateObject().
				See that method for more info.

	See also:	AttrFillGeometry::TransformTranslateObject()

********************************************************************************************/
void AttrLinearFill::TransformTranslateObject(const ExtendParams& ExtParams)
{
	// call our base class to do the major extending stuff.
	AttrFillGeometry::TransformTranslateObject(ExtParams);

	// ok, after the extend, our third control point is a bit skew-wiff,
	// so we need to wack it back in line - done by this (deceptively) simple call.
	SetEndPoint2(NULL);
}



/********************************************************************************************

>	void AttrRadialFill::TransformTranslateObject(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/11/2000

	Purpose:	Override of AttrFillGeometry::TransformTranslateObject().
				See that method for more info.

	See also:	AttrFillGeometry::TransformTranslateObject()

********************************************************************************************/
void AttrRadialFill::TransformTranslateObject(const ExtendParams& ExtParams)
{
	// call our base class to do the major extending stuff.
	AttrFillGeometry::TransformTranslateObject(ExtParams);

	// if we're a circle, then the extend has left our third control point
	// hanging in the air, so we need to pull it back down to earth...
	if (IsCircular())
	{
		DocCoord Start	= *GetStartPoint();
		DocCoord End1	= *GetEndPoint();
		DocCoord End2	= MakeLineAtAngle(Start, End1, 90, (INT32)Start.Distance(End1));
		SetEndPoint2(&End2);
	}
}



/*
	Transparency mode-change stuff.
*/

/********************************************************************************************
>	virtual BOOL AttrFlatTranspFill::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode
********************************************************************************************/

BOOL AttrFlatTranspFill::NeedsTransparency() const
{
	AttrFlatTranspFill* pNonConst = (AttrFlatTranspFill*) this;
	return pNonConst->GetTranspType() != TT_Mix || *(pNonConst->GetStartTransp()) != 0;
}



/********************************************************************************************
>	virtual BOOL AttrLinearTranspFill::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode
********************************************************************************************/

BOOL AttrLinearTranspFill::NeedsTransparency() const
{
	AttrLinearTranspFill* pNonConst = (AttrLinearTranspFill*) this;
	return ( 	pNonConst->GetTranspType() 		!= TT_Mix ||
				*(pNonConst->GetStartTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp()) 	!= 0 );
}



/********************************************************************************************
>	virtual BOOL AttrRadialTranspFill::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode
********************************************************************************************/

BOOL AttrRadialTranspFill::NeedsTransparency() const
{
	AttrRadialTranspFill* pNonConst = (AttrRadialTranspFill*) this;
	return ( 	pNonConst->GetTranspType() 		!= TT_Mix ||
				*(pNonConst->GetStartTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp()) 	!= 0 );
}



/********************************************************************************************
>	virtual BOOL AttrConicalTranspFill::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode
********************************************************************************************/

BOOL AttrConicalTranspFill::NeedsTransparency() const
{
	AttrConicalTranspFill* pNonConst = (AttrConicalTranspFill*) this;
	return ( 	pNonConst->GetTranspType() 		!= TT_Mix ||
				*(pNonConst->GetStartTransp()) 	!= 0 ||
				*(pNonConst->GetEndTransp()) 	!= 0 );
}



/********************************************************************************************
>	virtual BOOL AttrBitmapTranspFill::NeedsTransparency() const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if this node requires transparency mode to render properly.
	Purpose:	Called 
	Errors:		-
	SeeAlso:	Node::AttachNode
********************************************************************************************/

BOOL AttrBitmapTranspFill::NeedsTransparency() const
{
	return TRUE;
}



/********************************************************************************************

	FillAttrBlobState::FillAttrBlobState()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Purpose:	Constructor

********************************************************************************************/

FillAttrBlobState::FillAttrBlobState()
{
	count=0;
	pFillAttr=NULL;
	SelectionState=NULL;
}

/********************************************************************************************

	FillAttrBlobState::~FillAttrBlobState()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Purpose:	Destruct this object. If the state pointer is not null delete it.

********************************************************************************************/

FillAttrBlobState::~FillAttrBlobState()
{
	if (SelectionState!=NULL)
	{
		free(SelectionState);
		SelectionState=NULL;
	}
}


/********************************************************************************************

>	BOOL FillAttrBlobState::operator==(FillAttrBlobState& BlobState)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Returns:	TRUE if the blobs states are the same
	Purpose:	Compares two blob states to see if they are the same

********************************************************************************************/

BOOL FillAttrBlobState::operator==(FillAttrBlobState& BlobState)
{
	if (pFillAttr != BlobState.pFillAttr)
		return FALSE;

	if (count != BlobState.count)
		return FALSE;

	if (pFillAttr != NULL)
	{
		if (SelectionState==NULL || BlobState.SelectionState==NULL)
			return FALSE;

		for (UINT32 i=0; i<count; i++)
		{
			if (SelectionState[i] != BlobState.SelectionState[i])
				return FALSE;
		}
	}
	
	return TRUE;
}

/********************************************************************************************

>	FillBlobSelectionState::FillBlobSelectionState()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Purpose:	Constructor for the blob selection state record class

********************************************************************************************/

FillBlobSelectionState::FillBlobSelectionState()
{
	BlobCount = 0;
	FillType = NULL;
}

/********************************************************************************************

>	FillBlobSelectionState::~FillBlobSelectionState()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Purpose:	Constructor for the blob selection state record class.
				Deletes all the blob selection states in the list.

********************************************************************************************/

FillBlobSelectionState::~FillBlobSelectionState()
{
	BlobStateList.DeleteAll();
}

/********************************************************************************************

>	BOOL FillBlobSelectionState::Record()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Returns:	TRUE if all ok
	Purpose:	Makes a record of the current fill blob selection state so it can be
				compared at some later date.

********************************************************************************************/

BOOL FillBlobSelectionState::Record()
{
#if !defined(EXCLUDE_FROM_RALPH)
	BlobStateList.DeleteAll();
	FillType = NULL;

	if (GetApplication()->GetBlobManager()->GetCurrentInterest().Fill)
	{
		FillType = AttrFillGeometry::IsColourMeshVisible() ? CC_RUNTIME_CLASS(AttrFillGeometry)
														   : CC_RUNTIME_CLASS(AttrTranspFillGeometry);

		// Find the first Fill Attribute in the current Selection
		AttrFillGeometry* pAttr = AttrFillGeometry::FindFirstSelectedAttr(FillType);

		while (pAttr != NULL)
		{
			// Count this fill selected control points
			// and keep a running total
			if (pAttr->IsVisible())
			{
				if (pAttr->GetSelectionCount() > 0)
				{
					FillAttrBlobState* pBlobState = new FillAttrBlobState();
					if (pBlobState != NULL)
					{
						pBlobState->pFillAttr = pAttr;
						pBlobState->count = pAttr->GetSelectionState(&(pBlobState->SelectionState));
						BlobStateList.AddTail(pBlobState);
					}
				}
			}

			// Move on to the next Fill
			pAttr = AttrFillGeometry::FindNextSelectedAttr(FillType);
		}
	}
#endif
	return TRUE;
}

/********************************************************************************************

>	BOOL FillBlobSelectionState::operator==(FillBlobSelectionState& SelState)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/3/96
	Returns:	TRUE if the blobs states are the same
	Purpose:	Compares two blob states to see if they are the same

********************************************************************************************/

BOOL FillBlobSelectionState::operator==(FillBlobSelectionState& SelState)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// Were the same type of fills visible ?
	if (FillType != SelState.FillType)
		return FALSE;

	// Were the same number of blobs selected ?
	if (BlobCount != SelState.BlobCount)
		return FALSE;

	// Were the same fills selected ?
	if (BlobStateList.GetCount() != SelState.BlobStateList.GetCount())
		return FALSE;

	// So are they *really*	identical ?
	if (BlobStateList.GetCount() > 0)
	{
		FillAttrBlobState* pBlobState = (FillAttrBlobState*)BlobStateList.GetHead();
		FillAttrBlobState* pOtherBlobState = (FillAttrBlobState*)SelState.BlobStateList.GetHead();

		while (pBlobState != NULL && pOtherBlobState != NULL)
		{
			if ( !(*pBlobState == *pOtherBlobState) )
				return FALSE;

			pBlobState = (FillAttrBlobState*)BlobStateList.GetNext(pBlobState);
			pOtherBlobState = (FillAttrBlobState*)SelState.BlobStateList.GetNext(pOtherBlobState);
		}
	}
#endif
	return TRUE;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------


/********************************************************************************************

>	virtual UINT32* FillAttrRecordHandler::GetTagList()

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		-
	Returns:	Ptr to a list of tag values, terminated by CXFRH_TAG_LIST_END
	Purpose:	Provides the record handler system with a list of records handled by this
				handler
	SeeAlso:	-

********************************************************************************************/

UINT32* FillAttrRecordHandler::GetTagList()
{
	static UINT32 TagList[] = {	TAG_FLATFILL,
								TAG_FLATFILL_NONE,
								TAG_FLATFILL_BLACK,
								TAG_FLATFILL_WHITE,
								TAG_LINEARFILL,
								TAG_LINEARFILL3POINT,
								TAG_ELLIPTICALFILL,
								TAG_CIRCULARFILL,
								TAG_CONICALFILL,
								TAG_SQUAREFILL,
								TAG_THREECOLFILL,
								TAG_FOURCOLFILL,
								TAG_BITMAPFILL,
								TAG_CONTONEBITMAPFILL,
								TAG_FRACTALFILL,
								TAG_NOISEFILL,

								TAG_FLATTRANSPARENTFILL,
								TAG_LINEARTRANSPARENTFILL,
								TAG_LINEARTRANSPARENTFILL3POINT,
								TAG_ELLIPTICALTRANSPARENTFILL,
								TAG_CIRCULARTRANSPARENTFILL,
								TAG_CONICALTRANSPARENTFILL,
								TAG_SQUARETRANSPARENTFILL,
								TAG_THREECOLTRANSPARENTFILL,
								TAG_FOURCOLTRANSPARENTFILL,
								TAG_BITMAPTRANSPARENTFILL,
								TAG_FRACTALTRANSPARENTFILL,
								TAG_NOISETRANSPARENTFILL,

								TAG_FILL_REPEATING,
								TAG_FILL_NONREPEATING,
								TAG_FILL_REPEATINGINVERTED,
//Mark Howitt, 8/10/97. Include repeating grad fills
#ifdef NEW_FEATURES
								TAG_FILL_REPEATING_EXTRA,
								TAG_TRANSPARENTFILL_REPEATING_EXTRA,	// Chris Snook (8/12/99)
#endif
								TAG_TRANSPARENTFILL_REPEATING,
								TAG_TRANSPARENTFILL_NONREPEATING,
								TAG_TRANSPARENTFILL_REPEATINGINVERTED,

								TAG_FILLEFFECT_ALTRAINBOW,
								TAG_FILLEFFECT_RAINBOW,
								TAG_FILLEFFECT_FADE,
								TAG_LINEARFILLMULTISTAGE,
								TAG_LINEARFILLMULTISTAGE3POINT,
								TAG_CIRCULARFILLMULTISTAGE,
								TAG_ELLIPTICALFILLMULTISTAGE,
								TAG_CONICALFILLMULTISTAGE,
								TAG_SQUAREFILLMULTISTAGE,

								CXFRH_TAG_LIST_END};

	return (UINT32*)&TagList;
}

/********************************************************************************************

>	virtual BOOL FillAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	BOOL ok = TRUE;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_FLATFILL:
		case TAG_FLATFILL_NONE:
		case TAG_FLATFILL_BLACK:
		case TAG_FLATFILL_WHITE:			ok = HandleFlatFillRecord(pCXaraFileRecord);					break;
		
		case TAG_LINEARFILL3POINT:
		case TAG_LINEARFILL:				ok = HandleLinearFillRecord(pCXaraFileRecord);					break;
		case TAG_LINEARFILLMULTISTAGE3POINT:
		case TAG_LINEARFILLMULTISTAGE:		ok = HandleMultiStageLinearFillRecord(pCXaraFileRecord);		break;
		case TAG_ELLIPTICALFILL:		
		case TAG_CIRCULARFILL:				ok = HandleEllipticalFillRecord(pCXaraFileRecord);				break;
		case TAG_ELLIPTICALFILLMULTISTAGE:
		case TAG_CIRCULARFILLMULTISTAGE:	ok = HandleMultiStageCircularFillRecord(pCXaraFileRecord);		break;
		case TAG_CONICALFILL:				ok = HandleConicalFillRecord(pCXaraFileRecord);					break;
		case TAG_CONICALFILLMULTISTAGE:		ok = HandleMultiStageConicalFillRecord(pCXaraFileRecord);		break;
		case TAG_SQUAREFILL:				ok = HandleSquareFillRecord(pCXaraFileRecord);					break;
		case TAG_SQUAREFILLMULTISTAGE:		ok = HandleMultiStageSquareFillRecord(pCXaraFileRecord);		break;
		case TAG_THREECOLFILL:				ok = HandleThreeColFillRecord(pCXaraFileRecord);				break;
		case TAG_FOURCOLFILL:				ok = HandleFourColFillRecord(pCXaraFileRecord);					break;
		case TAG_BITMAPFILL:			
		case TAG_CONTONEBITMAPFILL:			ok = HandleBitmapFillRecord(pCXaraFileRecord);					break;
		case TAG_FRACTALFILL:				ok = HandleFractalFillRecord(pCXaraFileRecord);					break;
		case TAG_NOISEFILL:					ok = HandleNoiseFillRecord(pCXaraFileRecord);					break;

		case TAG_FLATTRANSPARENTFILL:		ok = HandleFlatTransparentFillRecord(pCXaraFileRecord);			break;
		case TAG_LINEARTRANSPARENTFILL3POINT:
		case TAG_LINEARTRANSPARENTFILL:		ok = HandleLinearTransparentFillRecord(pCXaraFileRecord);		break;
		case TAG_ELLIPTICALTRANSPARENTFILL:
		case TAG_CIRCULARTRANSPARENTFILL:	ok = HandleEllipticalTransparentFillRecord(pCXaraFileRecord);	break;
		case TAG_CONICALTRANSPARENTFILL:	ok = HandleConicalTransparentFillRecord(pCXaraFileRecord);		break;
		case TAG_SQUARETRANSPARENTFILL:		ok = HandleSquareTransparentFillRecord(pCXaraFileRecord);		break;
		case TAG_THREECOLTRANSPARENTFILL:	ok = HandleThreeColTransparentFillRecord(pCXaraFileRecord);		break;
		case TAG_FOURCOLTRANSPARENTFILL:	ok = HandleFourColTransparentFillRecord(pCXaraFileRecord);		break;

		case TAG_BITMAPTRANSPARENTFILL:		ok = HandleBitmapTransparentFillRecord(pCXaraFileRecord);		break;
		case TAG_FRACTALTRANSPARENTFILL:	ok = HandleFractalTransparentFillRecord(pCXaraFileRecord);		break;
		case TAG_NOISETRANSPARENTFILL:		ok = HandleNoiseTransparentFillRecord(pCXaraFileRecord);		break;

		case TAG_FILL_REPEATING:
//Mark Howitt, 7/10/97. using new fill modes
#ifdef NEW_FEATURES
		case TAG_FILL_REPEATING_EXTRA:
#endif
		case TAG_FILL_NONREPEATING:
		case TAG_FILL_REPEATINGINVERTED:			ok = HandleFillRepeatRecord(pCXaraFileRecord);				break;
//Chris Snook, 8/12/99. using new fill modes
#ifdef NEW_FEATURES		
		case TAG_TRANSPARENTFILL_REPEATING_EXTRA:
#endif
		case TAG_TRANSPARENTFILL_REPEATING:
		case TAG_TRANSPARENTFILL_NONREPEATING:
		case TAG_TRANSPARENTFILL_REPEATINGINVERTED:	ok = HandleTransparentFillRepeatRecord(pCXaraFileRecord);	break;

		case TAG_FILLEFFECT_ALTRAINBOW:
		case TAG_FILLEFFECT_RAINBOW: 
		case TAG_FILLEFFECT_FADE:		ok = HandleFillEffectRecord(pCXaraFileRecord);	break;

		default:
			ok = FALSE;
			ERROR3_PF(("I don't handle records with the tag (%d)\n",pCXaraFileRecord->GetTag()));
			break;
	}

	return ok;
}


/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFlatFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/5/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a flat fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFlatFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	INT32 ColourRef;
	INT32 Tag = pCXaraFileRecord->GetTag();
	BOOL ok = TRUE;
	// If it is the plain flat fill record then read the colour
	// If it is one of the flat fill default colour records then this is not required
	switch (Tag)
	{
		case TAG_FLATFILL:
			ok = pCXaraFileRecord->ReadINT32(&ColourRef);
			break;
		case TAG_FLATFILL_NONE:
			ColourRef = REF_DEFAULTCOLOUR_TRANSPARENT;
			break;
		case TAG_FLATFILL_BLACK:
			ColourRef = REF_DEFAULTCOLOUR_BLACK;
			break;
		case TAG_FLATFILL_WHITE:
			ColourRef = REF_DEFAULTCOLOUR_WHITE;
			break;
		default:
			ERROR2(FALSE,"I don't handle this tag type");
	}

	if (ok)
	{
		AttrFlatColourFill* pAttr = new AttrFlatColourFill;
		if (pAttr != NULL)
		{
			FlatFillAttribute* pValue = (FlatFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				if (ok) ok = GetDocColour(ColourRef,&(pValue->Colour));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleLinearFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a linear fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleLinearFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 tag = pCXaraFileRecord->GetTag();
	ERROR2IF(pCXaraFileRecord == NULL, FALSE, "pCXaraFileRecord is NULL");
	ERROR2IF(tag!=TAG_LINEARFILL && tag!=TAG_LINEARFILL3POINT, FALSE, "I don't handle this tag type");

	BOOL ok = TRUE;
	BOOL b3PointLinear = (tag==TAG_LINEARFILL3POINT);

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColourRef,EndColourRef;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the linear fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok && b3PointLinear) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrLinearColourFill* pAttr = new AttrLinearColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			LinearFillAttribute* pValue = (LinearFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				if (b3PointLinear)
					pValue->SetEndPoint2(&EndPoint2);
				else
					pValue->SetEndPoint2(NULL);
				
				// Store the profile
				pValue->SetProfile (Profile);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleMultiStageLinearFillRecord(
					CXaraFileRecord* pCXaraFileRecord)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a multi stage linear fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleMultiStageLinearFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 tag = pCXaraFileRecord->GetTag();
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(tag!=TAG_LINEARFILLMULTISTAGE && tag!=TAG_LINEARFILLMULTISTAGE3POINT, FALSE, "I don't handle this tag type");

	BOOL ok = TRUE;
	BOOL b3PointLinear = (tag==TAG_LINEARFILLMULTISTAGE3POINT);

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColourRef,EndColourRef;

	// Read in the linear fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok && b3PointLinear) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);

	// now, read all the multi stage records
	INT32 * pColRecs = NULL;
	UINT32 NumRecords = 0;

	if (ok) ok = pCXaraFileRecord->ReadUINT32(&NumRecords);

	pColRecs = new INT32[NumRecords];

	double *pPositions = new double[NumRecords];

	UINT32				i;
	for( i = 0 ; i < NumRecords; i++)
	{
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&(pPositions[i]));
		if (ok) ok = pCXaraFileRecord->ReadINT32(&(pColRecs[i]));
	}

	DocColour Colour;

	if (ok)
	{
		AttrLinearColourFill* pAttr = new AttrLinearColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			LinearFillAttribute* pValue = (LinearFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				if (b3PointLinear)
					pValue->SetEndPoint2(&EndPoint2);
				else
					pValue->SetEndPoint2(NULL);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));

				// create the colour ramp
				ColourRamp * pRamp = pValue->MakeNewColourRamp();			

				for (i = 0 ; i < NumRecords; i++)
				{
					// add a new ramp item
					if (ok) ok = GetDocColour(pColRecs[i], &Colour);
					
					if (ok) pRamp->AddEntry((float)pPositions[i], &Colour);
				}				

				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	if (pPositions)
		delete [] pPositions;

	if (pColRecs)
		delete [] pColRecs;

	return ok;
}


/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleEllipticalFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a elliptical or circular fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleEllipticalFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_ELLIPTICALFILL && Tag != TAG_CIRCULARFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColourRef,EndColourRef;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the elliptical/circular fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);

	if (ok && Tag == TAG_ELLIPTICALFILL)				// Ellipses have an extra control point
		ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrRadialColourFill* pAttr = new AttrRadialColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			RadialFillAttribute* pValue = (RadialFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);

				if (Tag == TAG_ELLIPTICALFILL)
				{
					pValue->SetEndPoint2(&EndPoint2);
					pValue->MakeElliptical();
				}
				else
					pValue->MakeCircular();

				// Store the profile
				pValue->SetProfile (Profile);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleMultiStageCircularFillRecord(
			CXaraFileRecord* pCXaraFileRecord)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/10/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a multistage circular fill
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleMultiStageCircularFillRecord(
			CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_CIRCULARFILLMULTISTAGE &&
		Tag != TAG_ELLIPTICALFILLMULTISTAGE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColourRef,EndColourRef;

	// Read in the elliptical/circular fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);

	if (ok && Tag == TAG_ELLIPTICALFILLMULTISTAGE)	// Ellipses have an extra control point
		ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);

	// now, read all the multi stage records
	INT32 * pColRecs = NULL;
	UINT32 NumRecords = 0;

	if (ok) ok = pCXaraFileRecord->ReadUINT32(&NumRecords);

	pColRecs = new INT32[NumRecords];

	double *pPositions = new double[NumRecords];

	for (UINT32 i = 0 ; i < NumRecords; i++)
	{
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&(pPositions[i]));
		if (ok) ok = pCXaraFileRecord->ReadINT32(&(pColRecs[i]));
	}

	DocColour Colour;

	if (ok)
	{
		AttrRadialColourFill* pAttr = new AttrRadialColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			RadialFillAttribute* pValue = (RadialFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);

				if (Tag == TAG_ELLIPTICALFILLMULTISTAGE)
				{
					pValue->SetEndPoint2(&EndPoint2);
					pValue->MakeElliptical();
				}
				else						
					pValue->MakeCircular();

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));

				// create the colour ramp
				ColourRamp * pRamp = pValue->MakeNewColourRamp();			

				for (UINT32 i = 0 ; i < NumRecords; i++)
				{
					// add a new ramp item
					if (ok) ok = GetDocColour(pColRecs[i], &Colour);
					
					if (ok) pRamp->AddEntry((float)pPositions[i], &Colour);
				}				

				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	delete [] pPositions;
	delete [] pColRecs;

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleConicalFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a conical fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleConicalFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_CONICALFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint;
	INT32 StartColourRef,EndColourRef;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the conical fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrConicalColourFill* pAttr = new AttrConicalColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			ConicalFillAttribute* pValue = (ConicalFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(NULL);

				// Store the profile
				pValue->SetProfile (Profile);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleMultiStageConicalFillRecord(
				CXaraFileRecord* pCXaraFileRecord)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/10/99
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a multi stage conical fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleMultiStageConicalFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_CONICALFILLMULTISTAGE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint;
	INT32 StartColourRef,EndColourRef;

	// Read in the conical fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);

		// now, read all the multi stage records
	INT32 * pColRecs = NULL;
	UINT32 NumRecords = 0;

	if (ok) ok = pCXaraFileRecord->ReadUINT32(&NumRecords);

	pColRecs = new INT32[NumRecords];

	double *pPositions = new double[NumRecords];

	UINT32				i;
	for( i = 0 ; i < NumRecords; i++)
	{
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&(pPositions[i]));
		if (ok) ok = pCXaraFileRecord->ReadINT32(&(pColRecs[i]));
	}

	DocColour Colour;

	if (ok)
	{
		AttrConicalColourFill* pAttr = new AttrConicalColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			ConicalFillAttribute* pValue = (ConicalFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(NULL);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));

				// create the colour ramp
				ColourRamp * pRamp = pValue->MakeNewColourRamp();			

				for (i = 0 ; i < NumRecords; i++)
				{
					// add a new ramp item
					if (ok) ok = GetDocColour(pColRecs[i], &Colour);
					
					if (ok) pRamp->AddEntry((float)pPositions[i], &Colour);
				}				

				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	if (pPositions)
		delete [] pPositions;

	if (pColRecs)
		delete [] pColRecs;

	return ok;
}


/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleSquareFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a square fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleSquareFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_SQUAREFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColourRef,EndColourRef;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the elliptical/circular fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrSquareColourFill* pAttr = new AttrSquareColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			SquareFillAttribute* pValue = (SquareFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				// Store the profile
				pValue->SetProfile (Profile);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleMultiStageSquareFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/3/2000
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a multi-stage square fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleMultiStageSquareFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_SQUAREFILLMULTISTAGE,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColourRef,EndColourRef;
//	double Bias = 0, Gain = 0;
//	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the elliptical/circular fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
//	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
//	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);
	
	// now, read all the multi stage records
	INT32 * pColRecs = NULL;
	UINT32 NumRecords = 0;

	if (ok) ok = pCXaraFileRecord->ReadUINT32(&NumRecords);

	pColRecs = new INT32[NumRecords];

	double *pPositions = new double[NumRecords];

	UINT32				i;
	for( i = 0 ; i < NumRecords; i++)
	{
		if (ok) ok = pCXaraFileRecord->ReadDOUBLE(&(pPositions[i]));
		if (ok) ok = pCXaraFileRecord->ReadINT32(&(pColRecs[i]));
	}

	DocColour Colour;

	if (ok)
	{
		AttrSquareColourFill* pAttr = new AttrSquareColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			SquareFillAttribute* pValue = (SquareFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));

				// create the colour ramp
				ColourRamp * pRamp = pValue->MakeNewColourRamp();			

				for (i = 0 ; i < NumRecords; i++)
				{
					// add a new ramp item
					if (ok) ok = GetDocColour(pColRecs[i], &Colour);
					
					if (ok) pRamp->AddEntry((float)pPositions[i], &Colour);
				}				

				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	if (pPositions)
		delete [] pPositions;

	if (pColRecs)
		delete [] pColRecs;

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleThreeColFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a ThreeCol fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleThreeColFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_THREECOLFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint;
	DocCoord EndPoint;
	DocCoord EndPoint2;
	INT32 StartColourRef;
	INT32 EndColourRef;
	INT32 EndColour2Ref;

	// Read in the elliptical/circular fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColour2Ref);

	if (ok)
	{
		AttrThreeColColourFill* pAttr = new AttrThreeColColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			ThreeColFillAttribute* pValue = (ThreeColFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));
				if (ok) ok = GetDocColour(EndColour2Ref,&(pValue->EndColour2));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFourColFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a FourCol fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFourColFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_FOURCOLFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint;
	DocCoord EndPoint;
	DocCoord EndPoint2;
	INT32 StartColourRef;
	INT32 EndColourRef;
	INT32 EndColour2Ref;
	INT32 EndColour3Ref;

	// Read in the elliptical/circular fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColourRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColour2Ref);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColour3Ref);

	if (ok)
	{
		AttrFourColColourFill* pAttr = new AttrFourColColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			FourColFillAttribute* pValue = (FourColFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				// Convert the colour references into doc colours, and insert the attr into the tree
				if (ok) ok = GetDocColour(StartColourRef,&(pValue->Colour));
				if (ok) ok = GetDocColour(EndColourRef,&(pValue->EndColour));
				if (ok) ok = GetDocColour(EndColour2Ref,&(pValue->EndColour2));
				if (ok) ok = GetDocColour(EndColour3Ref,&(pValue->EndColour3));
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleBitmapFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a bitmap or contone bitmap fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleBitmapFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_BITMAPFILL && Tag != TAG_CONTONEBITMAPFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 BitmapRef,StartColRef,EndColRef;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the bitmap fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (Tag == TAG_CONTONEBITMAPFILL)
	{
		if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColRef);
		if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColRef);
	}

	if (ok) ok = pCXaraFileRecord->ReadINT32(&BitmapRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrBitmapColourFill* pAttr = new AttrBitmapColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			BitmapFillAttribute* pValue = (BitmapFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				if (Tag == TAG_CONTONEBITMAPFILL)
				{
					DocColour StartCol,EndCol;

					// Convert the colour references into doc colours
					if (ok) ok = GetDocColour(StartColRef,&StartCol);
					if (ok) ok = GetDocColour(EndColRef,&EndCol);

					if (ok)
					{
						pValue->SetStartColour(&StartCol);
						pValue->SetEndColour(&EndCol);
					}
				}

				// Store the profile
				pValue->SetProfile (Profile);

				// Convert the bmp reference into a kernel bmp, and insert the attr into the tree
				KernelBitmap * pBitmap = NULL;
				pBitmap = GetReadBitmapReference(BitmapRef);

				ok = (pBitmap != NULL);
				if (ok) ok = pValue->AttachBitmap(pBitmap);
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFractalFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record must be a fractal fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFractalFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_FRACTALFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColRef,EndColRef, Seed, DPI;
	FIXED16 Graininess, Gravity, Squash;
	BYTE Tileable;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the fractal fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColRef);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&Seed);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Graininess);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Gravity);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Squash);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&DPI);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Tileable);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrFractalColourFill* pAttr = new AttrFractalColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			FractalFillAttribute* pValue = (FractalFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);
									   
				// Store the fractal data
				pValue->SetSeed(Seed);
				pValue->SetGraininess(Graininess);
				pValue->SetGravity(Gravity);
				pValue->SetSquash(Squash);
				pValue->SetFractalDPI(DPI);
				pValue->SetTileable(Tileable);

				DocColour StartCol,EndCol;

				// Convert the colour references into doc colours
				if (ok) ok = GetDocColour(StartColRef,&StartCol);
				if (ok) ok = GetDocColour(EndColRef,&EndCol);
				if (ok)
				{
					pValue->SetStartColour(&StartCol);
					pValue->SetEndColour(&EndCol);
				}

				// Store the profile
				pValue->SetProfile (Profile);

				// Recreate the fractal, and insert the attr into the tree
				if (ok) ok = pValue->RecalcFractal();
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}


/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleNoiseFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/97
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record must be a noise fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleNoiseFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_NOISEFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 StartColRef,EndColRef, DPI;
	FIXED16 Grain;
	INT32 Seed;
	BYTE Tileable;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the fractal fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&StartColRef);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&EndColRef);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Grain);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Seed);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&DPI);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Tileable);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrNoiseColourFill* pAttr = new AttrNoiseColourFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			NoiseFillAttribute* pValue = (NoiseFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);
									   
				// Store the noise data
				pValue->SetGraininess(Grain);
				pValue->SetSeed(Seed);
				pValue->SetFractalDPI(DPI);
				pValue->SetTileable(Tileable);

				DocColour StartCol,EndCol;

				// Convert the colour references into doc colours
				if (ok) ok = GetDocColour(StartColRef,&StartCol);
				if (ok) ok = GetDocColour(EndColRef,&EndCol);
				if (ok)
				{
					pValue->SetStartColour(&StartCol);
					pValue->SetEndColour(&EndCol);
				}

				// Store the profile
				pValue->SetProfile (Profile);

				// Recreate the fractal, and insert the attr into the tree
				if (ok) ok = pValue->RecalcFractal();
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFlatTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a flat transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFlatTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_FLATTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	BYTE Transp,TranspType;

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);

	if (ok)
	{
		AttrFlatTranspFill* pAttr = new AttrFlatTranspFill;
		if (pAttr != NULL)
		{
			FlatTranspFillAttribute* pValue = (FlatTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->Transp		= UINT32(Transp);
				pValue->TranspType	= UINT32(TranspType);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleLinearTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Linear transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleLinearTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 tag = pCXaraFileRecord->GetTag();
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(tag!=TAG_LINEARTRANSPARENTFILL && tag!=TAG_LINEARTRANSPARENTFILL3POINT, FALSE, "I don't handle this tag type");

	BOOL ok = TRUE;
	BOOL b3PointLinear = (tag==TAG_LINEARTRANSPARENTFILL3POINT);

	DocCoord StartPoint, EndPoint, EndPoint2;
	BYTE Transp,EndTransp,TranspType;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok && b3PointLinear) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrLinearTranspFill* pAttr = new AttrLinearTranspFill;
		if (pAttr != NULL)
		{
			LinearTranspFillAttribute* pValue = (LinearTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				if (b3PointLinear)
					pValue->SetEndPoint2(&EndPoint2);
				else
					pValue->SetEndPoint2(NULL);
				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
				pValue->SetProfile (Profile);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleEllipticalTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be an Elliptical or circular transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleEllipticalTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_ELLIPTICALTRANSPARENTFILL && Tag != TAG_CIRCULARTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	BYTE Transp,EndTransp,TranspType;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);

	if (ok && Tag == TAG_ELLIPTICALTRANSPARENTFILL)
		ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrRadialTranspFill* pAttr = new AttrRadialTranspFill;
		if (pAttr != NULL)
		{
			RadialTranspFillAttribute* pValue = (RadialTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);

				if (Tag == TAG_ELLIPTICALTRANSPARENTFILL)
				{
					pValue->SetEndPoint2(&EndPoint2);
					pValue->MakeElliptical();
				}
				else
					pValue->MakeCircular();

				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
				pValue->SetProfile (Profile);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleConicalTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Conical transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleConicalTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_CONICALTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint;
	BYTE Transp,EndTransp,TranspType;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrConicalTranspFill* pAttr = new AttrConicalTranspFill;
		if (pAttr != NULL)
		{
			ConicalTranspFillAttribute* pValue = (ConicalTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
				pValue->SetProfile (Profile);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleSquareTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Square transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleSquareTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_SQUARETRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	BYTE Transp,EndTransp,TranspType;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrSquareTranspFill* pAttr = new AttrSquareTranspFill;
		if (pAttr != NULL)
		{
			SquareTranspFillAttribute* pValue = (SquareTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
				pValue->SetProfile (Profile);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleThreeColTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a ThreeCol transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleThreeColTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_THREECOLTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint;
	DocCoord EndPoint;
	DocCoord EndPoint2;
	BYTE Transp;
	BYTE EndTransp;
	BYTE EndTransp2;
	BYTE TranspType;

	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp2);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);

	if (ok)
	{
		AttrThreeColTranspFill* pAttr = new AttrThreeColTranspFill;
		if (pAttr != NULL)
		{
			ThreeColTranspFillAttribute* pValue = (ThreeColTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->EndTransp2	= UINT32(EndTransp2);
				pValue->TranspType	= UINT32(TranspType);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFourColTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a FourCol transparent fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFourColTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	UINT32 Tag = pCXaraFileRecord->GetTag();

	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(Tag != TAG_FOURCOLTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint;
	DocCoord EndPoint;
	DocCoord EndPoint2;
	BYTE Transp;
	BYTE EndTransp;
	BYTE EndTransp2;
	BYTE EndTransp3;
	BYTE TranspType;

	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);

	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp2);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp3);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);

	if (ok)
	{
		AttrFourColTranspFill* pAttr = new AttrFourColTranspFill;
		if (pAttr != NULL)
		{
			FourColTranspFillAttribute* pValue = (FourColTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);

				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->EndTransp2	= UINT32(EndTransp2);
				pValue->EndTransp3	= UINT32(EndTransp3);
				pValue->TranspType	= UINT32(TranspType);
				ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleBitmapTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a Transparent bitmap fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleBitmapTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_BITMAPTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	INT32 BitmapRef;
	BYTE Transp,EndTransp,TranspType;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the bitmap fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&BitmapRef);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrBitmapTranspFill* pAttr = new AttrBitmapTranspFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			BitmapTranspFillAttribute* pValue = (BitmapTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);
				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
				pValue->SetProfile (Profile);

				// Convert the bmp reference into a kernel bmp, and insert the attr into the tree
				KernelBitmap * pBitmap = NULL;
				pBitmap = GetReadBitmapReference(BitmapRef);

				ok = (pBitmap != NULL);
				if (ok) ok = pValue->AttachBitmap(pBitmap);
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFractalTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record must be a transparent fractal fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFractalTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_FRACTALTRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	BYTE Transp,EndTransp,TranspType;
	INT32 Seed, DPI;
	FIXED16 Graininess, Gravity, Squash;
	BYTE Tileable;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the fractal fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);

	if (ok) ok = pCXaraFileRecord->ReadINT32(&Seed);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Graininess);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Gravity);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Squash);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&DPI);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Tileable);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrFractalTranspFill* pAttr = new AttrFractalTranspFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			FractalTranspFillAttribute* pValue = (FractalTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);
				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
									   
				// Store the fractal data
				pValue->SetSeed(Seed);
				pValue->SetGraininess(Graininess);
				pValue->SetGravity(Gravity);
				pValue->SetSquash(Squash);
				pValue->SetFractalDPI(DPI);
				pValue->SetTileable(Tileable);
				pValue->SetProfile (Profile);

				// Recreate the fractal, and insert the attr into the tree
				if (ok) ok = pValue->RecalcFractal();
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}



/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleNoiseTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/1/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record must be a transparent noise fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleNoiseTransparentFillRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");
	ERROR2IF(pCXaraFileRecord->GetTag() != TAG_NOISETRANSPARENTFILL,FALSE,"I don't handle this tag type");

	BOOL ok = TRUE;

	DocCoord StartPoint, EndPoint, EndPoint2;
	BYTE Transp,EndTransp,TranspType;
	INT32 DPI;
	FIXED16 Grain;
	INT32 Seed;
	BYTE Tileable;
	double Bias = 0, Gain = 0;
	double* ptrBias = &Bias, *ptrGain = &Gain;

	// Read in the fractal fill data
	if (ok) ok = pCXaraFileRecord->ReadCoord(&StartPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint);
	if (ok) ok = pCXaraFileRecord->ReadCoord(&EndPoint2);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Transp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&EndTransp);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&TranspType);
	if (ok) ok = pCXaraFileRecord->ReadFIXED16(&Grain);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&Seed);
	if (ok) ok = pCXaraFileRecord->ReadINT32(&DPI);
	if (ok) ok = pCXaraFileRecord->ReadBYTE(&Tileable);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrBias);
	if (ok) ok = pCXaraFileRecord->ReadDOUBLEnoError (ptrGain);

	CProfileBiasGain Profile;

	if ((ptrBias != NULL) && (ptrGain != NULL))
	{
		Profile.SetBias((AFp) Bias);
		Profile.SetGain((AFp) Gain);
	}

	if (ok)
	{
		AttrNoiseTranspFill* pAttr = new AttrNoiseTranspFill;
		if (pAttr != NULL)
		{
			// Get a ptr to the attr value object
			NoiseTranspFillAttribute* pValue = (NoiseTranspFillAttribute*)pAttr->GetAttributeValue();

			if (pValue != NULL)
			{
				// Store the start and end points
				pValue->SetStartPoint(&StartPoint);
				pValue->SetEndPoint(&EndPoint);
				pValue->SetEndPoint2(&EndPoint2);
				pValue->Transp		= UINT32(Transp);
				pValue->EndTransp	= UINT32(EndTransp);
				pValue->TranspType	= UINT32(TranspType);
									   
				// Store the fractal data
				pValue->SetGraininess(Grain);
				pValue->SetSeed(Seed);
				pValue->SetFractalDPI(DPI);
				pValue->SetTileable(Tileable);
				pValue->SetProfile (Profile);

				// Recreate the fractal, and insert the attr into the tree
				if (ok) ok = pValue->RecalcFractal();
				if (ok) ok = InsertNode(pAttr);
			}
			else
				ok = FALSE;
		}
		else
			ok = FALSE;
	}

	return ok;
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFillRepeatRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a fill repeat record

				Social comment:
				England lost against Germany last night in the Euro96 semi-final, 6-5 on penalties (1-1 after extra time).
				I'm devastated.  I can't believe England could be so unlucky.  I no longer have the will to work.
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFillRepeatRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	INT32 Repeat;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_FILL_NONREPEATING:		Repeat = 1; break;
		case TAG_FILL_REPEATING:		Repeat = 2; break;
		case TAG_FILL_REPEATINGINVERTED:Repeat = 3; break;
//Mark Howitt, 8/10/97. Make grad fills a special case for backwards compatability
#ifdef NEW_FEATURES
		case TAG_FILL_REPEATING_EXTRA:	Repeat = 4; break;
#endif
		default:
			ERROR3("Do what? I only do fill repeat records mate");
			return TRUE;
			break;
	}

	AttrFillMappingLinear* pAttr = new AttrFillMappingLinear;
	if (pAttr != NULL)
	{
		pAttr->Value.Repeat = Repeat;
		return InsertNode(pAttr);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleTransparentFillRepeatRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a transparent fill repeat record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleTransparentFillRepeatRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	INT32 Repeat;

	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_TRANSPARENTFILL_NONREPEATING:		Repeat = 1; break;
		case TAG_TRANSPARENTFILL_REPEATING:			Repeat = 2; break;
		case TAG_TRANSPARENTFILL_REPEATINGINVERTED:	Repeat = 3; break;
	//Mark Howitt. 8/10/97. (well actually Chris Snook - 8/12/99); cause we need to do it here
	//as well - Make repeating grad fills a special case
#ifdef NEW_FEATURES
		case TAG_TRANSPARENTFILL_REPEATING_EXTRA:				Repeat = 4; break;
#endif

		default:
			ERROR3("Do what? I only do trans fill repeat records mate");
			return TRUE;
			break;
	}

	AttrTranspFillMappingLinear* pAttr = new AttrTranspFillMappingLinear;
	if (pAttr != NULL)
	{
		pAttr->Value.Repeat = Repeat;
		return InsertNode(pAttr);
	}

	return FALSE;
}

/********************************************************************************************

>	BOOL FillAttrRecordHandler::HandleFillEffectRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	25/6/96
	Inputs:		pCXaraFileRecord = ptr to record to handle
	Returns:	TRUE if handled successfuly
				FALSE otherwise
	Purpose:	Handles the given record.
				The record has to be a linear fill record
	SeeAlso:	-

********************************************************************************************/

BOOL FillAttrRecordHandler::HandleFillEffectRecord(CXaraFileRecord* pCXaraFileRecord)
{
	ERROR2IF(pCXaraFileRecord == NULL,FALSE,"pCXaraFileRecord is NULL");

	AttrFillEffect* pAttr = NULL;
	switch (pCXaraFileRecord->GetTag())
	{
		case TAG_FILLEFFECT_ALTRAINBOW:	pAttr = new AttrFillEffectAltRainbow;	break;
		case TAG_FILLEFFECT_RAINBOW:	pAttr = new AttrFillEffectRainbow;		break;
		case TAG_FILLEFFECT_FADE:		pAttr = new AttrFillEffectFade;			break;

		default:
			ERROR3("Do what? I only do fill effect records mate");
			return TRUE;
			break;
	}

	if (pAttr != NULL)
		return InsertNode(pAttr);

	return FALSE;
}

/********************************************************************************************

>	virtual void FillAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/6/96
	Inputs:		pRecord = ptr to a record
				pStr = ptr to string to update
	Returns:	-
	Purpose:	This provides descriptions for the fill attribute record.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

#if XAR_TREE_DIALOG
void FillAttrRecordHandler::GetRecordDescriptionText(CXaraFileRecord* pRecord,StringBase* pStr)
{
	if (pStr == NULL || pRecord == NULL)
		return;

	// Call base class first
	// This outputs the tag and size
	CamelotRecordHandler::GetRecordDescriptionText(pRecord,pStr);

	char s[256];

	UINT32 Tag = pRecord->GetTag();
	switch (Tag)
	{
		case TAG_FLATFILL :
		{
			// Read the colour reference number
			INT32 ColourRef;
			BOOL ok = pRecord->ReadINT32(&ColourRef);
			_stprintf(s,"Colour reference = %d\r\n",ColourRef);	(*pStr) += s;
		}
		break;

		case TAG_LINEARFILL :
		case TAG_LINEARFILL3POINT :
		case TAG_ELLIPTICALFILL :
		case TAG_CIRCULARFILL :
		case TAG_CONICALFILL :
		case TAG_SQUAREFILL :
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			INT32 StartColourRef,EndColourRef;

			// Read in the linear fill data
			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			if (Tag == TAG_ELLIPTICALFILL || Tag == TAG_SQUAREFILL || Tag==TAG_LINEARFILL3POINT)
				pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadINT32(&StartColourRef);
			pRecord->ReadINT32(&EndColourRef);

			_stprintf(s,"Start Colour ref\t= %d\r\n",StartColourRef);				(*pStr) += s;
			_stprintf(s,"End Colour ref\t= %d\r\n",EndColourRef);	   				(*pStr) += s;
			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);	(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);		(*pStr) += s;
			if (Tag == TAG_ELLIPTICALFILL || Tag == TAG_SQUAREFILL || Tag==TAG_LINEARFILL3POINT)
			{
				_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);	(*pStr) += s;
			}
		}
		break;

		case TAG_THREECOLFILL :
		case TAG_FOURCOLFILL :
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			INT32 StartColourRef,EndColourRef, EndColourRef2, EndColourRef3;

			// Read in the linear fill data
			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadINT32(&StartColourRef);
			pRecord->ReadINT32(&EndColourRef);
			pRecord->ReadINT32(&EndColourRef2);
			if (Tag == TAG_FOURCOLFILL)
				pRecord->ReadINT32(&EndColourRef3);

			_stprintf(s,"Start Colour ref\t= %d\r\n",StartColourRef);				(*pStr) += s;
			_stprintf(s,"End Colour ref\t= %d\r\n",EndColourRef);	   				(*pStr) += s;
			_stprintf(s,"End Colour2 ref\t= %d\r\n",EndColourRef2);	   				(*pStr) += s;
			if (Tag == TAG_FOURCOLFILL)
			{
				_stprintf(s,"End Colour3 ref\t= %d\r\n",EndColourRef3);	   			(*pStr) += s;
			}
			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);		(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);			(*pStr) += s;
			_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);		(*pStr) += s;
		}
		break;

		case TAG_BITMAPFILL :
		case TAG_CONTONEBITMAPFILL :
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			INT32 BitmapRef;
			INT32 StartColourRef,EndColourRef;

			// Read in the linear fill data
			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			pRecord->ReadCoord(&EndPoint2);
			if (Tag == TAG_CONTONEBITMAPFILL)
			{
				pRecord->ReadINT32(&StartColourRef);
				pRecord->ReadINT32(&EndColourRef);
			}
			pRecord->ReadINT32(&BitmapRef);

			_stprintf(s,"Bitmap reference\t= %d\r\n",BitmapRef);					(*pStr) += s;
			if (Tag == TAG_CONTONEBITMAPFILL)
			{
				_stprintf(s,"Start Colour ref\t= %d\r\n",StartColourRef);			(*pStr) += s;
				_stprintf(s,"End Colour ref\t= %d\r\n",EndColourRef);	   			(*pStr) += s;
			}
			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);	(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);		(*pStr) += s;
			_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);		(*pStr) += s;
		}
		break;

		case TAG_FRACTALFILL:
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			INT32 StartColRef,EndColRef, Seed, DPI;
			FIXED16 Graininess, Gravity, Squash;
			BYTE Tileable;
			double d = 1.0;

			// Read in the fractal fill data
			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadINT32(&StartColRef);
			pRecord->ReadINT32(&EndColRef);

			pRecord->ReadINT32(&Seed);
			pRecord->ReadFIXED16(&Graininess);
			pRecord->ReadFIXED16(&Gravity);
			pRecord->ReadFIXED16(&Squash);
			pRecord->ReadINT32(&DPI);
			pRecord->ReadBYTE(&Tileable);

			_stprintf(s,"Start Colour ref\t= %d\r\n",StartColRef);				(*pStr) += s;
			_stprintf(s,"End Colour ref\t= %d\r\n",EndColRef);					(*pStr) += s;

			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);	(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);  		(*pStr) += s;
			_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);		(*pStr) += s;

			_stprintf(s,"Seed\t\t= %d\r\n",Seed);									(*pStr) += s;

			d = Graininess.MakeDouble();_stprintf(s,"Graininess\t= %g\r\n",d);	(*pStr) += s;
			d = Gravity.MakeDouble();	_stprintf(s,"Gravity\t\t= %g\r\n",d);		(*pStr) += s;
			d = Squash.MakeDouble();	_stprintf(s,"Squash\t\t= %g\r\n",d);		(*pStr) += s;

			_stprintf(s,"DPI\t\t= %d\r\n",DPI);									(*pStr) += s;
			_stprintf(s,"Tileable\t\t= %d\r\n",INT32(Tileable));					(*pStr) += s;
		}
		break;

		case TAG_FLATTRANSPARENTFILL:
		{
			BYTE Transp,TranspType;
			pRecord->ReadBYTE(&Transp);
			pRecord->ReadBYTE(&TranspType);

			_stprintf(s,"Transparency\t\t= %d\r\n",INT32(Transp));				(*pStr) += s;
			_stprintf(s,"Transparency Type\t\t= %d\r\n",INT32(TranspType));	(*pStr) += s;
		}
		break;

		case TAG_LINEARTRANSPARENTFILL:
		case TAG_LINEARTRANSPARENTFILL3POINT:
		case TAG_ELLIPTICALTRANSPARENTFILL:
		case TAG_CIRCULARTRANSPARENTFILL:
		case TAG_CONICALTRANSPARENTFILL:
		case TAG_SQUARETRANSPARENTFILL:
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			BYTE Transp,EndTransp,TranspType;

			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			if (Tag == TAG_ELLIPTICALTRANSPARENTFILL || Tag == TAG_SQUARETRANSPARENTFILL || Tag==TAG_LINEARFILL3POINT)
				pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadBYTE(&Transp);
			pRecord->ReadBYTE(&EndTransp);
			pRecord->ReadBYTE(&TranspType);

			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);	(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);		(*pStr) += s;
			if (Tag == TAG_ELLIPTICALTRANSPARENTFILL || Tag == TAG_SQUARETRANSPARENTFILL || Tag==TAG_LINEARFILL3POINT)
			{
				_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);	(*pStr) += s;
			}
			_stprintf(s,"Start Transparency\t= %d\r\n",INT32(Transp));				(*pStr) += s;
			_stprintf(s,"End Transparency\t= %d\r\n",INT32(EndTransp));			(*pStr) += s;
			_stprintf(s,"Transparency Type\t= %d\r\n",INT32(TranspType));			(*pStr) += s;
		}
		break;

		case TAG_THREECOLTRANSPARENTFILL:
		case TAG_FOURCOLTRANSPARENTFILL:
		{
			DocCoord StartPoint;
			DocCoord EndPoint;
			DocCoord EndPoint2;
			BYTE Transp;
			BYTE EndTransp;
			BYTE EndTransp2;
			BYTE EndTransp3;
			BYTE TranspType;

			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadBYTE(&Transp);
			pRecord->ReadBYTE(&EndTransp);
			pRecord->ReadBYTE(&EndTransp2);
			if (Tag == TAG_FOURCOLTRANSPARENTFILL)
				pRecord->ReadBYTE(&EndTransp3);
			pRecord->ReadBYTE(&TranspType);

			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);		(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);			(*pStr) += s;
			_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);		(*pStr) += s;
			_stprintf(s,"Start Transparency\t= %d\r\n",INT32(Transp));				(*pStr) += s;
			_stprintf(s,"End Transparency\t= %d\r\n",INT32(EndTransp));				(*pStr) += s;
			_stprintf(s,"End Transparency2\t= %d\r\n",INT32(EndTransp2));			(*pStr) += s;

			if (Tag == TAG_FOURCOLTRANSPARENTFILL)
			{
				_stprintf(s,"End Transparency3\t= %d\r\n",INT32(EndTransp3));		(*pStr) += s;
			}

			_stprintf(s,"Transparency Type\t= %d\r\n",INT32(TranspType));			(*pStr) += s;
		}
		break;

		case TAG_BITMAPTRANSPARENTFILL:
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			INT32 BitmapRef;
			BYTE Transp,EndTransp,TranspType;

			// Read in the linear fill data
			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadBYTE(&Transp);
			pRecord->ReadBYTE(&EndTransp);
			pRecord->ReadBYTE(&TranspType);
			pRecord->ReadINT32(&BitmapRef);

			_stprintf(s,"Bitmap reference\t= %d\r\n",BitmapRef);					(*pStr) += s;

			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);	(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);		(*pStr) += s;
			_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);		(*pStr) += s;

			_stprintf(s,"Start Transparency\t= %d\r\n",INT32(Transp));				(*pStr) += s;
			_stprintf(s,"End Transparency\t= %d\r\n",INT32(EndTransp));			(*pStr) += s;
			_stprintf(s,"Transparency Type\t= %d\r\n",INT32(TranspType));			(*pStr) += s;

		}
		break;

		case TAG_FRACTALTRANSPARENTFILL:
		{
			DocCoord StartPoint, EndPoint, EndPoint2;
			BYTE Transp,EndTransp,TranspType;
			INT32 Seed, DPI;
			FIXED16 Graininess, Gravity, Squash;
			BYTE Tileable;
			double d = 1.0;

			// Read in the fractal fill data
			pRecord->ReadCoord(&StartPoint);
			pRecord->ReadCoord(&EndPoint);
			pRecord->ReadCoord(&EndPoint2);
			pRecord->ReadBYTE(&Transp);
			pRecord->ReadBYTE(&EndTransp);
			pRecord->ReadBYTE(&TranspType);

			pRecord->ReadINT32(&Seed);
			pRecord->ReadFIXED16(&Graininess);
			pRecord->ReadFIXED16(&Gravity);
			pRecord->ReadFIXED16(&Squash);
			pRecord->ReadINT32(&DPI);
			pRecord->ReadBYTE(&Tileable);

			_stprintf(s,"Coord 1\t\t= (%d,%d)\r\n",StartPoint.x,StartPoint.y);	(*pStr) += s;
			_stprintf(s,"Coord 2\t\t= (%d,%d)\r\n",EndPoint.x,EndPoint.y);  		(*pStr) += s;
			_stprintf(s,"Coord 3\t\t= (%d,%d)\r\n",EndPoint2.x,EndPoint2.y);		(*pStr) += s;

			_stprintf(s,"Start Transparency\t= %d\r\n",INT32(Transp));				(*pStr) += s;
			_stprintf(s,"End Transparency\t= %d\r\n",INT32(EndTransp));			(*pStr) += s;
			_stprintf(s,"Transparency Type\t= %d\r\n",INT32(TranspType));			(*pStr) += s;

			_stprintf(s,"Seed\t\t= %d\r\n",Seed);									(*pStr) += s;

			d = Graininess.MakeDouble();_stprintf(s,"Graininess\t= %g\r\n",d);	(*pStr) += s;
			d = Gravity.MakeDouble();	_stprintf(s,"Gravity\t\t= %g\r\n",d);		(*pStr) += s;
			d = Squash.MakeDouble();	_stprintf(s,"Squash\t\t= %g\r\n",d);		(*pStr) += s;

			_stprintf(s,"DPI\t\t= %d\r\n",DPI);									(*pStr) += s;
			_stprintf(s,"Tileable\t\t= %d\r\n",INT32(Tileable));					(*pStr) += s;
		}
		break;
	}
}
#endif // _DEBUG
