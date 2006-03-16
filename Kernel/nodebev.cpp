// $Id: nodebev.cpp 662 2006-03-14 21:31:49Z alex $
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

#include "camtypes.h"

#include <math.h>
//#include "pathedit.h"
#include "nodebev.h"
#include "nbevcont.h"
#include "noderect.h"
#include "ppbevel.h"

// Code headers
#include "attrmgr.h"
#include "fillval.h"
#include "fillattr.h"
#include "gclips.h"
#include "paths.h"
#include "pathtrap.h"
//#include "rndrgn.h"
#include "grndrgn.h"
#include "grndbmp.h"
#include "nodebmp.h"
#include "becomea.h"
#include "bmpcomp.h"
#include "attrbev.h"
//#include "xadrwold.h"
#include "spread.h"
#include "blobs.h"
//#include "bevres.h"
#include "lineattr.h"
//#include "bevtool.h"
#include "csrstack.h"
#include "prntview.h"
//#include "moldtool.h"
//#include "opcntr.h"
#include "nodecntr.h"	// for the 'becomeA' stuff
#include "ndbldpth.h"
#include "ncntrcnt.h"	// ContourNodePathProcessor
#include "nodetxts.h"
//#include "mario.h"		// _R(IDE_NOMORE_MEMORY)
#include "extender.h"	// for Extender code
//#include "swfrndr.h"	// For the FlashRenderRegion custom export code.
#include "cmxrendr.h"	// For the CMXRenderRegion custom export code.
//#include "ai_epsrr.h"	// For the AIEPSRenderRegion custom export code.
#include "rsmooth.h"
//#include "contmenu.h"
#include "blndhelp.h"
#include "offscrn.h"
#include "scanrr.h"
#include "opgrad.h"
//#include "princomp.h"
#include "strkattr.h"
//#include "ngcore.h"		// NameGallery, for stretching functionality
#include "slicehelper.h"
#include "brshattr.h"
#include "fthrattr.h"	// for AttrFeather
//#include "filtirr.h"
//#include "pmaskrgn.h"
#include "ophist.h"
#include "objchge.h"

CC_IMPLEMENT_DYNCREATE(NodeBevel, NodeRenderableInk)
CC_IMPLEMENT_DYNCREATE(BevelRecordHandler, CamelotRecordHandler)
CC_IMPLEMENT_DYNCREATE(NodeBevelBegin, NodeRenderableBounded)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


#ifdef GENERATETIMINGS
INT32 Timing1 = 0;
#endif

/**********************************************************************************************/

const UINT32 BevelClipPathToPath_Tolerance   =  30 ;
const UINT32 BevelClipPathToPath_Flatness    = 200 ;
const UINT32 BevelStrokePathToPath_Flatness  = 200 ;

const double BevelMaxBitmapWidth		    = 1600.0;
const double BevelMaxBitmapHeight			= 1200.0;


/***********************************************************************************************

>	NodeBevel::NodeBevel(Node* 	ContextNode,
						AttachNodeDirection Direction,
						const DocRect& 		BoundingRect,
						BOOL				Locked = FALSE,
						BOOL 				Mangled = FALSE,
						BOOL 				Marked = FALSE,
						BOOL 				Selected = FALSE,
						BOOL 				Renderable = FALSE
						)

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		ContextNode: Pointer to a node which this node is to be attached to.
				MonoOn Direction: MonoOff
				Specifies the direction in which the node is to be attached to the
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

	Purpose:	This constructor initialises the nodes flags and links it to ContextNode in the
				direction specified by Direction. All neccesary tree links are updated.
	Note:		SetUpShape() must be called before the NodeRegularShape is in a state in which
				it can be used.
    SeeAlso:	NodeRegularShape::SetUpShape
    Errors:		An ENSURE will occur if ContextNode is NULL

***********************************************************************************************/
NodeBevel::NodeBevel(Node* ContextNode,  
					AttachNodeDirection Direction,  
					BOOL Locked, 
					BOOL Mangled,  
					BOOL Marked, 
					BOOL Selected    
			  ) : NodeRenderableInk(ContextNode, Direction, Locked, Mangled, Marked, Selected )
{                         
	m_Indent = 0;
	m_pBevelBitmap = NULL;
	m_Contrast = 100;
	m_bLock = FALSE;
	m_bRenderBitmaps = TRUE;
	m_bZeroLineWidth = TRUE;
	m_LastPixelSize = 0;
	m_bOuter = FALSE;
	m_BitmapWidth = 0;
	m_BitmapHeight = 0;
	InkPath.Initialise(0,12);
	m_ShrunkPath.Initialise();
	m_bStopRender = FALSE;
	m_pBMPFill = NULL;
	m_BevelType = 0;
	m_LightAngle = 0;
	m_ViewScale = 0;
	m_OuterBevelPath.Initialise();
	m_Path.Initialise();
	m_pBeveler = NULL;
	m_Tilt = 32.0;
	m_bPathIsClosed = FALSE;
	m_bCached = FALSE;
	m_DoingRotation = FALSE;
	m_SubPixelOffset.x = 0;
	m_SubPixelOffset.y = 0;
	m_IsFlatFilledOnly = TRUE;
	m_HasTransparencyApplied = FALSE;
	m_pTranspFill = NULL;
	m_pCombiBitmap = NULL;
	m_LastZoomFactor = 1.0;
	m_LastCachedPrintBMPDPI = 0.0;
	m_IsABlendStepBevel = FALSE;
	m_DoRender = TRUE;
	m_IsAnAlphaBMP = FALSE;
	m_AmConvertingToShapes = FALSE;
	m_LastExportedDPI = 96.0;
}

/*********************************************************************************************

>	NodeBevel::NodeBevel() 

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/98
	Purpose:	Default constructor
	Note:		
	SeeAlso:	

**********************************************************************************************/
NodeBevel::NodeBevel()
{
	m_bStopRender = FALSE;
	m_Indent = 0;
	m_LightAngle = 300;
	m_bLock = FALSE;
	m_BevelType = 0;
	m_bZeroLineWidth = TRUE;
	m_pBevelBitmap = NULL;
	m_bOuter = FALSE;
	m_Contrast = 100;
	m_ShrunkPath.Initialise();
	m_ViewScale = 0;
	m_pBMPFill = NULL;
	m_bRenderBitmaps = TRUE;
	m_OuterBevelPath.Initialise();
	m_Path.Initialise();
	m_BevelType = 0;
	m_LightAngle = 0;
	m_JointType = MitreJoin;
	InkPath.Initialise(0,12);
	m_pBeveler = NULL;
	m_Tilt = 32.0;
	m_bPathIsClosed = FALSE;
	m_bCached = FALSE;
	m_DoingRotation = FALSE;
	m_SubPixelOffset.x = 0;
	m_SubPixelOffset.y = 0;
	m_IsFlatFilledOnly = TRUE;
	m_HasTransparencyApplied = FALSE;
	m_pTranspFill = NULL;
	m_pCombiBitmap = NULL;
	m_LastZoomFactor = 1.0;
	m_LastCachedPrintBMPDPI = 0.0;
	m_LastPixelSize = 0;
	m_IsABlendStepBevel = FALSE;
	m_DoRender = TRUE;
	m_IsAnAlphaBMP = FALSE;
	m_AmConvertingToShapes = FALSE;
	m_LastExportedDPI = 96.0;
}

/*********************************************************************************************

>	NodeBevel::~NodeBevel() 

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/98
	Purpose:	Default destructor
	Note:		
	SeeAlso:	

**********************************************************************************************/

NodeBevel::~NodeBevel()
{
	if (m_pBMPFill)
	{
		delete m_pBMPFill;
		m_pBMPFill = NULL;
	}

	if (m_pTranspFill)
	{
		delete m_pTranspFill;
		m_pTranspFill = NULL;
	}

	if (m_pBevelBitmap)
	{
		delete m_pBevelBitmap;
		m_pBevelBitmap = NULL;
	}

	if(m_pCombiBitmap)
	{
		delete m_pCombiBitmap;
		m_pCombiBitmap = NULL;
	}

	DeleteCache();
}

/*********************************************************************************************

>	String NodeBevel::Describe(BOOL Plural, BOOL Verbose) 

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/98
	Purpose:	Default constructor
	Note:		
	SeeAlso:	

**********************************************************************************************/

String NodeBevel::Describe(BOOL Plural, BOOL Verbose) 
{     
	String Name;
	Name.Load(_R(IDS_BEVEL_NODE_NAME));
	
	return Name; // Just to keep the compiler happy
}; 

/*********************************************************************************************

>	BOOL NodeBeveler::GenerateBevel();

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/98
	Purpose:	Creates the beveller class, and the beveller bitmap
	Note:		
	SeeAlso:	ContourBecomeA

**********************************************************************************************/
BOOL NodeBevel::GenerateBevel()
{
	NodeBevelController* pControl = (NodeBevelController *)FindParent();
	if ( !pControl )
		return FALSE ;

	// are we safe to render ?
	if ( pControl->GetInsideBoundingRect().IsEmpty() )
		return FALSE;
	
	BOOL ok = TRUE;
	
	// Check the current bitmap pointers and delete them if still valid
	if (m_pBMPFill)
	{
		delete m_pBMPFill;
		m_pBMPFill = NULL;
	}

	if (m_pTranspFill)
	{
		delete m_pTranspFill;
		m_pTranspFill = NULL;
	}

	if (m_pCombiBitmap)
	{
		delete m_pCombiBitmap;
		m_pCombiBitmap = NULL;
	}

	// Create a new pointer for the bitmap and Transp Bitmap fill Attributes
	m_pBMPFill = new BitmapFillAttribute;
	ERROR2IF(m_pBMPFill == NULL,FALSE,"Failed to create a new BMP Fill Attrib!");

	m_pTranspFill = new BitmapTranspFillAttribute;
	ERROR2IF(m_pTranspFill == NULL,FALSE,"Failed to create a new TranspBMP Fill Attrib!");

	UINT32 startTrans	= 0x00;
	UINT32 endTrans	= 0xff;
	m_pTranspFill->SetTranspType(TT_BEVEL);
	m_pTranspFill->SetStartTransp(&startTrans);
	m_pTranspFill->SetEndTransp(&endTrans);
	m_pTranspFill->SetTesselation(RT_RepeatInverted);

	m_pBMPFill->SetTesselation(RT_RepeatInverted);

	// set up the variables on myself & my parent by finding the appropriate attributes
	// create all the paths for the bevel
	// no need to do this if we're printing - they should already be set up
	if (!SetupVariables() || !CreateBevelPaths())
		ok = FALSE;

	return ok;
}

/*********************************************************************************************

>	BOOL NodeBevel::GenerateBevelForBlendStep(List * pList, CCAttrMap * pMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/2/2000
	Purpose:	Creates the bevel data from the given list of SumAllPathsElem
				
	Note:		Takes the variables from FindAppliedAttribute therefore the attributes
				should be applied to this node before calling this function
	SeeAlso:	ContourBecomeA

**********************************************************************************************/
BOOL NodeBevel::GenerateBevelForBlendStep(SumAllPathsElem* pElem, CCAttrMap * pAttrMap)
{
	// set up the variables on myself
	if (!SetupVariables(pAttrMap))
		return FALSE;	

	m_ViewScale = 0;

	BOOL bOuter = m_bOuter;
	INT32 Width = m_Indent;

	m_OuterBevelPath.ClearPath();
	m_Path.ClearPath();

	CProfileBiasGain Profile;

	// CGS - we need to generate the bevel with respect to the JoinType applied to the path
	// that we are generating it with; BUT if the original bevel had a jointype; then we need
	// to generate it with respect to this!
	
	JointType generateJoinType=MitreJoin;

	AttrJoinType * pJoinType = NULL;
	AttrJoinType * pBevelJoinType = NULL;

	pElem->GetAttrMap ()->Lookup( CC_RUNTIME_CLASS(AttrJoinType), (void *&)pJoinType);
	pAttrMap->Lookup( CC_RUNTIME_CLASS(AttrJoinType), (void *&)pBevelJoinType);

	if (pJoinType && pBevelJoinType)
	{
		if (*pJoinType == *pBevelJoinType)
			generateJoinType = pJoinType->Value.JoinType;
		else
			generateJoinType = pBevelJoinType->Value.JoinType;
	}

	// use the contour becomeA to generate all the paths
	ContourBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
		NULL, FALSE, &m_OuterBevelPath, &m_Path, Width, 1, bOuter, Profile,generateJoinType,
		BevelClipPathToPath_Flatness,TRUE);

	Path BlankPath;
	BlankPath.Initialise();

	NodePath* pPathNode = new NodePath;
	ERRORIF(pPathNode == NULL, _R(IDE_NOMORE_MEMORY), FALSE);

	pPathNode->InkPath.Initialise();

	pElem->GetPath()->ClipPathToPath(BlankPath, &(pPathNode->InkPath), 6);

	NodePath* pStrokedPathNode = pPathNode->MakeNodePathFromAttributes(30, pElem->GetAttrMap());
	
	if (pStrokedPathNode)
	{	
		pStrokedPathNode->DoBecomeA(&MyBecomeA);
		delete pStrokedPathNode;
	}

	delete pPathNode;
	pPathNode = NULL;

	if(m_Path.GetUsedSlots() <= 0)
		return TRUE;
	m_IsABlendStepBevel = TRUE;

	// set all the filled flags on the path list
	m_OuterBevelPath.IsFilled = TRUE;
	m_OuterBevelPath.IsStroked = FALSE;

	// reverse the original path so it will create a hole in the bevel path
	Path ReversedPath;
	ReversedPath.Initialise();
	ReversedPath.CloneFrom(m_Path);

	// Check to see what the zoom factor is so we can take it into account for the flatness values!
	DocView* pDocView = Document::GetCurrent()->GetFirstDocView();
	double CurrentZoom = 1.0;

	if(pDocView)
		CurrentZoom = pDocView->GetZoomFactor();

	ReverseAndShrinkBevelPathIntoInkPath(&ReversedPath, CurrentZoom, bOuter);

	// now the path has been created, create the bevel bitmap
	if (!CreateBevelBitmap())
		return FALSE;

	m_bCached = TRUE;

	return TRUE;
}



/*********************************************************************************************

>	BOOL NodeBevel::CreateBevelBitmap()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/10/99
	Purpose:	Creates the bitmap used in generating the highlights for bevels
	Note:		
	SeeAlso:	

**********************************************************************************************/
BOOL NodeBevel::CreateBevelBitmap()
{
	// Find the parent node and make sure it`s not null and it`s a bevel controller node
	Node* pNode = FindParent();
	if (!pNode && !pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
	{
		ERROR3("Bevel node has a parent which isn't a bevel controller node !");
		return FALSE;
	}

	TRACEUSER( "MarkH", _T("Creating a new Bevel Bitmap!\n"));

	// Clear the current shrunken path
	m_ShrunkPath.ClearPath();
	
	// delete the existing pointers
	if (m_pBevelBitmap)
	{
		delete m_pBevelBitmap;
		m_pBevelBitmap = NULL;
	}

	if (m_pBeveler)
	{
		delete m_pBeveler;
		m_pBeveler = NULL;
	}

	if(m_pCombiBitmap)
	{
		delete m_pCombiBitmap;
		m_pCombiBitmap = NULL;
	}

	// get the path to use from the processor
	if (m_Path.GetNumCoords() == 0)
		return FALSE;

	// Now create a new CBeveler Pointer
	m_pBeveler = new CBeveler(&m_Path);
	ERROR2IF(!m_pBeveler,FALSE,"Failed to create a CBeveler pointer!");

	// the flatness needs to be linked to the height of this
	MILLIPOINT Flatness = BEVEL_FLATNESS;

	// alter the flatness for printing
	NodeBevelController* pBevelController = (NodeBevelController *) pNode;
	if (pBevelController && pBevelController->IsPrinting())
		Flatness = (MILLIPOINT)(((double)BEVEL_FLATNESS) * (96.0 / pBevelController->GetDPI()));

	// set up the bevel type
	m_pBeveler->SetBevelType(m_BevelType);
	m_pBeveler->SetBevelAngle(m_LightAngle);
	m_pBeveler->SetBevelTilt(m_Tilt);

	// Find the current Join type
	AttrJoinType * pJoinType = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)(&pJoinType));
	JointType JT = RoundJoin;
	if (pJoinType)
		JT = pJoinType->Value.JoinType;

	// Calculate the angle using the lightangle variable
	double angle = 3.1415926536*m_LightAngle/180 ;
	NormCoord nc( cos(angle),sin(angle) );

	// Set up the beveler with all the new variables!
	BOOL ok = m_pBeveler->SetUpBeveler(m_Indent, JT, &nc, m_bOuter, pBevelController->m_Contrast, NULL, BevelClipPathToPath_Flatness);

	// Check to see if the setup went ok
	if(!ok)
		return FALSE;

	// Do another recalculation to setup the fill handles!
	CalculateBitmapSize(&m_BitmapWidth, &m_BitmapHeight);

	// Check to see if the size is valid
	if (m_BitmapWidth <= 0 || m_BitmapHeight <= 0)
		return TRUE;

	// Now tell the beveler how large to make the bevels.
	m_pBeveler->SetBevelerSize(m_BitmapWidth,m_BitmapHeight);
	
	// make the bounding box the same as the selection bounds 
	BoundingRectangle = m_PixelAllignedRect;
	IsBoundingRectValid = TRUE;

	m_BMPCentre.x = m_PixelAllignedRect.lo.x;
	m_BMPCentre.y = m_PixelAllignedRect.lo.y;
	m_BMPPoint1.x = m_PixelAllignedRect.hi.x;
	m_BMPPoint1.y = m_PixelAllignedRect.lo.y;
	m_BMPPoint2.x = m_PixelAllignedRect.lo.x;
	m_BMPPoint2.y = m_PixelAllignedRect.hi.y;

	NodeAttribute* pFillAttrib = NULL;
	DocColour FillColour = COLOUR_BLACK;

	// Test to see if theres a FillGeom Attribute applied to the bevel
	if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttrib))
	{
		// If the pointer is valid and It`s a flat fill then get the colour and set the IsFlatFill Flag
		if(pFillAttrib != NULL && pFillAttrib->IsAFlatFill())
		{
			FillColour = *((AttrFlatColourFill*)pFillAttrib)->GetStartColour();
			m_IsFlatFilledOnly = TRUE;
		}
		else
		{
			// it`s not a plain flat fill so set the flag to false
			m_IsFlatFilledOnly = FALSE;

			// Make sure we know if it`s an Alpha Channel Bitmap as this needs special treatment!
			if(pFillAttrib->IsABitmapFill())
			{
				BitmapFillAttribute* pFill = (BitmapFillAttribute*)pFillAttrib->GetAttributeValue();
				m_IsAnAlphaBMP = (pFill->GetBitmap()->GetBitmapInfoHeader()->biBitCount == 32);
			}
			else
				m_IsAnAlphaBMP = FALSE;
		}
	}

	// make sure the palette is setup ready for the BMP
	SetUpBitmapPalette((RenderRegion *)NULL,FillColour);

	// Everythings cool, return TRUE
	return TRUE;
}




/*********************************************************************************************

>	BOOL NodeBevel::SetupVariables(CCAttrMap * pMap)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Purpose:	Sets up all the attributes on myself and my parent by finding the
				appropriate attributes
	Inputs:		The attribute map to use, NULL to find applied attributes
	Note:		
	SeeAlso:	

**********************************************************************************************/
BOOL NodeBevel::SetupVariables(CCAttrMap * pMap)
{
	// set the attributes up (on our parent !)
	AttrBevelIndent * pIndent = NULL;
	AttrBevelLightAngle * pLightAngle = NULL;
	AttrBevelContrast * pContrast = NULL;
	AttrBevelType * pType = NULL;
	AttrBevelLightTilt * pTilt = NULL;

	if(pMap == NULL)
	{
		this->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelIndent), (NodeAttribute **)(&pIndent));
		this->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightAngle), (NodeAttribute **)(&pLightAngle));
		this->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelContrast), (NodeAttribute **)(&pContrast));
		this->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelType), (NodeAttribute **)(&pType));
		this->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelLightTilt), (NodeAttribute **)(&pTilt));
	}
	else
	{
		pMap->Lookup( CC_RUNTIME_CLASS(AttrBevelIndent), (void *&)pIndent);
		pMap->Lookup( CC_RUNTIME_CLASS(AttrBevelLightAngle), (void *&)pLightAngle);
		pMap->Lookup( CC_RUNTIME_CLASS(AttrBevelContrast), (void *&)pContrast);
		pMap->Lookup( CC_RUNTIME_CLASS(AttrBevelType), (void *&)pType);
		pMap->Lookup( CC_RUNTIME_CLASS(AttrBevelLightTilt), (void *&)pTilt);
	}

	NodeBevelController * pBevelController = (NodeBevelController *)FindParent();
	if ( !pBevelController )
		return FALSE ;

	// Now make sure we`ve got valid pointers to all the varibale
	if (pIndent && pLightAngle && pContrast && pType && pTilt)
	{
		if (pIndent->Value.m_Indent < 0)
		{
			pBevelController->m_Indent = -pIndent->Value.m_Indent;
			pBevelController->m_bOuter = TRUE;
		}
		else
		{
			pBevelController->m_Indent = pIndent->Value.m_Indent;
			pBevelController->m_bOuter = FALSE;
		}

		m_Indent = pBevelController->m_Indent;
		m_bOuter = pBevelController->m_bOuter;

		m_LightAngle = pBevelController->m_LightAngle = pLightAngle->Value.m_LightAngle;
		m_Contrast = pBevelController->m_Contrast = pContrast->Value.m_Contrast;
		m_BevelType = pBevelController->m_BevelType = pType->Value.m_Type;
		m_Tilt = pBevelController->m_Tilt = pTilt->Value.m_LightTilt;

		return TRUE;
	}

	return FALSE;
}

/*********************************************************************************************

>	BOOL NodeBevel::CreateBevelPaths()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/10/99
	Purpose:	Sets up all the attributes on myself and my parent by finding the
				appropriate attributes
	Note:		
	SeeAlso:	ContourBecomeA (in kernel/nodecntr.h), NodeContour::GenerateContour

**********************************************************************************************/
BOOL NodeBevel::CreateBevelPaths()
{
	// Delete the current Cache
	DeleteCache();

	// build up the source path
	Node * pNode = FindParent();
	NodeBevelController * pControl = (NodeBevelController *)FindParent();

	if (!pNode)
		return FALSE;

	pNode = pNode->FindFirstChild();

	if (!pNode || pNode == this)
		return FALSE;

	double CurrentFlatness = 200.0;

	// Mark Howitt 31/10/00
	// This function call now replaces the passed in value when the BecomeA function is created.
	// This has been done to elliminate zoom dependancies which change the flatness values
	// depending on the current zoom value. See the function header for more details
	Node* pNodeToBevel = pNode->FindNextInk();

	while(pNodeToBevel == this && pNodeToBevel)
		pNodeToBevel = pNodeToBevel->FindNextInk();

	if(pNodeToBevel)
	{
		DocRect DR = ((NodeRenderableInk*)pNodeToBevel)->GetBoundingRect();

		// Get the smallest dimesion, Width or Height.
		double Smallest = DR.Height() < DR.Width() ? (double)DR.Height() : (double)DR.Width();

		// now make the flatness value equal to the smallest dimesion divided by 375
		CurrentFlatness = Smallest / 375.0;

		// Check to see if we`re within the specified limits
		if(1.0 > CurrentFlatness) CurrentFlatness = 1.0;
		if(375.0 < CurrentFlatness) CurrentFlatness = 375.0;
	}

	// Check to see what the zoom factor is so we can take it into account for the flatness values!
	DocView* pDocView = Document::GetCurrent()->GetFirstDocView();
	double CurrentZoom = 1.0;

	if(pDocView)
		CurrentZoom = pDocView->GetZoomFactor();

	m_LastZoomFactor = CurrentZoom;

	m_Path.ClearPath();
	InkPath.ClearPath();
	m_OuterBevelPath.ClearPath();

	INT32 Width  = pControl->m_Indent;
	BOOL bOuter = pControl->m_bOuter;

	// bog standard profile
	CProfileBiasGain Profile;
	AttrJoinType * pJoinType = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)(&pJoinType));

	JointType JT = RoundJoin;
	if (pJoinType)
		JT = pJoinType->Value.JoinType;

	// use the BecomeA passback mechanism to contour everything
/*#ifdef NEWBEVEL
	ContourBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
		NULL, FALSE, &m_OuterBevelPath, &m_Path, 0, 1, bOuter, Profile, JT,
		CurrentFlatness,TRUE);
#else*/
	ContourBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
		NULL, FALSE, &m_OuterBevelPath, &m_Path, Width, 1, bOuter, Profile, JT,
		CurrentFlatness,TRUE);
//#endif

	m_bPathIsClosed = TRUE;

	Path CopyPath; CopyPath.Initialise();
	Path TempPath; TempPath.Initialise();
//	UINT32 NumObjects = 0;
//	AttrVariableWidth* pStroked = NULL;

//	TextStory* pTextStory = NULL;

	// run through all nodes, contouring them
	while (pNode)
	{
		if(pNode != this)
		{
			// When we conditionally call CanBecomeA below, we want to find the number of paths
			// pNode will produce. So we must clear the counter now before the condition starts
			MyBecomeA.ResetCount();

			// is the node a blend path ?
			if (!pNode->IS_KIND_OF(NodeBlendPath))
			{	
				// is the node a path ?
				if (pNode->IsNodePath())
				{
					// turn the path into a closed shape
					if (((NodePath *)pNode)->InkPath.IsClosed())
						m_bPathIsClosed = FALSE;
/*
					FindAppliedAttribute(CC_RUNTIME_CLASS(AttrVariableWidth),(NodeAttribute**)(&pStroked));
					if(pStroked && ((VariableWidthAttrValue*)pStroked->GetAttributeValue())->GetWidthFunction() != NULL)
					{
						((NodePath *)pNode)->DoBecomeA(&MyBecomeA);
					}
					else
					{
*/						NodePath* pPathNode = ((NodePath *)pNode)->MakeNodePathFromAttributes(CurrentFlatness);

						if (pPathNode)
						{
							pPathNode->DoBecomeA(&MyBecomeA);
							delete pPathNode;
						}
//					}
				}
				else if (pNode->CanBecomeA(&MyBecomeA))
				{
					// If the NumObjects > 0 then we`ve got a compound node of some sort!
					if (MyBecomeA.GetCount() > 0)
					{
						// is this a text node ?
						if (pNode->IS_KIND_OF(TextStory))
						{
							// reformat it first
							((TextStory *)pNode)->FormatAndChildren();
						}
						else
						{
							// See if we`ve got any text in our compound, and if so format them!
							Node* pLid = pNode;
							Node* pNext = pNode;

							while(pNext)
							{
								pNext = SliceHelper::FindNextOfClass(pNext,pLid,CC_RUNTIME_CLASS(TextStory));
								
								if(pNext)
									((TextStory*)pNext)->FormatAndChildren();
							}
						}

						if (!pNode->DoBecomeA(&MyBecomeA))
							break;
					}

					if (MyBecomeA.GetCount() > 1)
					{
						TRACEUSER( "MarkH", _T("Clipping group to tidy things up!\n"));

						// Need to clip!
						CopyPath.ClearPath();
						CopyPath.CloneFrom(m_Path);
						m_Path.ClearPath();

						if(TempPath.ClipPathToPath(CopyPath,&m_Path,7|CLIPPING_SOURCE_WINDING,BevelClipPathToPath_Tolerance,CurrentFlatness,CurrentFlatness) < 0)
						{
							TRACEUSER( "MarkH", _T("ClipFailed!!! NodeBev.cpp\n"));
						}
					}
				}
			}
		}

		pNode = pNode->FindNext();
	}

	// set all the filled flags on the path list
	m_OuterBevelPath.IsFilled = TRUE;
	m_OuterBevelPath.IsStroked = FALSE;

	// reverse the original path so it will create a hole in the bevel path
	Path ReversedPath;
	ReversedPath.Initialise();
	ReversedPath.CloneFrom(m_Path);

	return ReverseAndShrinkBevelPathIntoInkPath(&ReversedPath, CurrentZoom, bOuter);
}

BOOL NodeBevel::ReverseAndShrinkBevelPathIntoInkPath(Path* pBevelPath, double ZoomValue, BOOL IsAnOuter)
{
	// Reverses the bevel path and adds it to the original to create the new bevel path
	// Also shrinks/Expands the inner/outer path edge by a pixel to get rid of antialiasing
	// effects when overlapping the object

	if(pBevelPath == NULL)
		return FALSE;

	double CurrentFlatness = 200.0 * ZoomValue;

	Path CopyPath;
	CopyPath.Initialise();
	CopyPath.CloneFrom(*pBevelPath);
	pBevelPath->ClearPath();
	
	UINT32 Modifier = 750;
	
	if(!m_bOuter)
		Modifier = 0;

	if(ZoomValue < 1.0)
		Modifier = UINT32( double(Modifier) * ZoomValue );

	CurrentFlatness = CopyPath.CalculateFlatnessValueFromPath(375.0);
	CopyPath.InitializeContourValues(Modifier,JOIN_ROUND,!IsAnOuter,CurrentFlatness);
	
	if(CopyPath.GetContourForStep(pBevelPath) < 0)
		pBevelPath->CloneFrom(CopyPath);

	if(pBevelPath->GetUsedSlots() > 2)
		pBevelPath->Reverse();

	if (IsAnOuter)
	{	
		// copy the outer path into the inking path for the bevel, and add the
		// original path to create a hole in it
		InkPath.CloneFrom(m_OuterBevelPath);
		InkPath.MergeTwoPaths(*pBevelPath);
	}
	else 
	{
		// create the path list for inner contours
		// copy the outer path into the inking path for the bevel, and add the
		// original path to create a hole in it
		if(m_OuterBevelPath.ClipPathToPath(
			*pBevelPath, &InkPath, 1 | CLIPPING_CLIP_WINDING,
			BevelClipPathToPath_Tolerance,
			CurrentFlatness,CurrentFlatness) < 0)
		{
			TRACEUSER( "MarkH", _T("ClipFailed so copying path! PATHS\n"));
		}
	}

	InkPath.IsFilled = TRUE;
	InkPath.IsStroked = FALSE;

	return TRUE;
}

/***********************************************************************************************

>	void NodeBevel::Render(RenderRegion* pRender)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/07/00
	Inputs:		Pointer to a render region
	Purpose:	Will build a path for the shape and draw it to the given render region.

***********************************************************************************************/
void NodeBevel::Render(RenderRegion* pRender)
{
//	TRACEUSER( "Gerry", _T("NodeBevel::Render region 0x%08x\n"),pRender);

	// set up variables from parent
	NodeBevelController * pBevelController = (NodeBevelController *)GetParentController();
	DocView* pDocView = Document::GetCurrent()->GetFirstDocView();
	
	// Check to see if we`ve got valid pointer!
	if(pBevelController == NULL || pRender == NULL || !m_DoRender || !pDocView)
		return;

	// Get the bevel variables from the Bevel Controller node
	m_Indent = pBevelController->m_Indent;
	m_Contrast = pBevelController->m_Contrast;
	m_LightAngle	= pBevelController->m_LightAngle;
	m_BevelType	= pBevelController->m_BevelType;
	m_bOuter = pBevelController->m_bOuter;

	// if we have a zero indent, then there`s no bevel! just return
	if (m_Indent==0)
		return;

	// if we`re printing then we need to check that our bounding render intersects with
	// the current render regions clipping rect
	if (pBevelController->IsPrinting())
	{
		// make sure we're intersected with the render regions clipping rectangle
		if (!(pRender->GetClipRect().IsIntersectedWith(BoundingRectangle)))
		{
			DeleteCachedBitmap();
			return ;
		}
	}

	// first, check for re-entry of code
	if (m_bLock)
		return;

	m_bStopRender = FALSE;

	// check if we have anything to draw
	// do this after we've removed all the path processors
	if (pBevelController->GetInsideBoundingRect().IsEmpty() || InkPath.GetNumCoords() == 0)
	{
		m_bStopRender = TRUE;
		return;
	}

	BOOL DoReRender = FALSE;

	double CurrentZoom = pDocView->GetZoomFactor();

	if(m_LastZoomFactor != CurrentZoom)
	{
		if(m_pCombiBitmap != NULL)
		{
			delete m_pCombiBitmap;
			m_pCombiBitmap = NULL;
		}

		Path ReversedPath;
		ReversedPath.Initialise();
		ReversedPath.CloneFrom(m_Path);

		if(!ReverseAndShrinkBevelPathIntoInkPath(&ReversedPath, CurrentZoom, m_bOuter))
		{
			m_bStopRender = TRUE;
			return;
		}

		m_LastZoomFactor = CurrentZoom;
	}

	pRender->SaveContext();

	NodeAttribute* pFillAttrib = NULL;
	DocColour FillColour = COLOUR_BLACK;

	// Store the last state of the IsFlatFilledOnly flag
	BOOL LastFillWasFlat = m_IsFlatFilledOnly;

	// Test to see if theres a FillGeom Attribute applied to the bevel
	if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttrib))
	{
		// If the pointer is valid and It`s a flat fill then get the colour and set the IsFlatFill Flag
		if(pFillAttrib != NULL && pFillAttrib->IsAFlatFill())
		{
			FillColour = *((AttrFlatColourFill*)pFillAttrib)->GetStartColour();
			m_IsFlatFilledOnly = (FillColour != COLOUR_NONE);
		}
		else
		{
			// it`s not a plain flat fill so set the flag to false
			m_IsFlatFilledOnly = FALSE;
			if(pFillAttrib && m_pBMPFill)
				m_pBMPFill->SetTesselation(((ColourFillAttribute*)pFillAttrib->GetAttributeValue())->GetTesselation());
		}
	}

	// Test to see if there`s a transparency applied to the bevel
	if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry),&pFillAttrib))
	{
		if(pFillAttrib != NULL && pFillAttrib->IsATranspFill())
		{
			// Check the Transparency Type. If it`s a flat fill check to see if the start value
			// is zero. if it`s not zero then it`s got transparency!
			if(pFillAttrib->IsAFlatFill())
			{
				// Only set as Transparent if it`s not NONE!
				m_HasTransparencyApplied = *((AttrFlatTranspFill*)pFillAttrib)->GetStartTransp() > 0;
			}
			else
				m_HasTransparencyApplied = TRUE;

			if (m_HasTransparencyApplied && pFillAttrib!=NULL && m_pTranspFill!=NULL)
				m_pTranspFill->SetTesselation(((ColourFillAttribute*)pFillAttrib->GetAttributeValue())->GetTesselation());
		}
		else
		{
			// Set the flag to false if it doesn`t 
			m_HasTransparencyApplied = FALSE;
		}
	}

	// Only re-render the bitmap if we`re not rotating the light angle, as this has already been
	// done for us, or if we`re doing hit testing!
	if((!m_DoingRotation && !pRender->IsHitDetect() && pRender->RRQuality.GetQuality() > 10 && !pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)))
		|| !m_pBevelBitmap)
	{
		// check to see if we have changed from a gradfill to a flat fill with transparency as the bitmap will need to be
		// updated!
		DoReRender |= (m_IsFlatFilledOnly && !LastFillWasFlat && m_HasTransparencyApplied);

		if(pRender->IsPrinting())
		{
//			TRACEUSER( "Gerry", _T("Printing Render\n"));
			if(!pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)) && pRender->RRQuality.GetQuality() > 100)
			{
				if(m_LastCachedPrintBMPDPI == 0.0)
				{
//					TRACEUSER( "Gerry", _T("Printing Render Updating\n"));
					m_LastCachedPrintBMPDPI = pBevelController->GetDPI();
					DoReRender = TRUE;
				}
			}
		}
		else
			m_LastCachedPrintBMPDPI = 0.0;

		if(m_pBevelBitmap == NULL)
			DoReRender = TRUE;

		// re-render the bitmap
		if(((MILLIPOINT)m_LastPixelSize != pRender->GetScaledPixelWidth() && !pBevelController->IsPrinting()) || DoReRender)
		{
			if(pRender->GetScaledPixelWidth() > 2)
			{
				m_LastPixelSize = pRender->GetScaledPixelWidth();
				if (!RenderBitmapForDisplay(pRender))
				{
					m_bStopRender = TRUE;
					pRender->RestoreContext();
					return;
				}
			}
	PORTNOTETRACE("other","NodeBevel::Render - removed Flash support");
#ifndef EXCLUDE_FROM_XARALX
			else if(pRender->IsKindOf(CC_RUNTIME_CLASS(FlashRenderRegion)))
			{
				INT32 NewDPI = 96;
				Camelot.GetPrefValue(TEXT("Displays"),TEXT("CompCToEDPI"),&NewDPI);
				pBevelController->SetDPI(NewDPI);

				BOOL OldFlag = m_AmConvertingToShapes;
				m_AmConvertingToShapes = TRUE;
				CreateBevelBitmap();
				RenderBitmapForDisplay(pRender);
				m_AmConvertingToShapes = OldFlag;
			}
#endif
		}

		// Now set up the palette (depending on contrast)
		SetUpBitmapPalette((RenderRegion *)NULL,FillColour);
	}

	// (ChrisG - 14/2/2001) Bitmaps should only be drawn if they have height or width 
	//	(otherwise GDraw has a fit and crashes).
	if (m_BitmapWidth > 0 && m_BitmapHeight > 0)
	{
		// now, render the bitmap to the display
		if(!pRender->IsHitDetect() && pRender->RRQuality.GetQuality() > 30)
		{
			RenderBitmap(pRender);
		}
		else
		{
			// DY 23/10/2000 if we have a brush attribute applied to this bevel then setting line width to 50
			// will cause it to scale to a really small value and lock up the program.  Therefore we 
			// ask the render region to disable all brush path processors on the stack
			
			pRender->EnableAllBrushPathProcessors(FALSE);

			pRender->SetLineColour(COLOUR_BLACK);
			pRender->SetFillColour(FillColour);
			pRender->SetLineWidth(50);

			InkPath.IsStroked = TRUE;
			pRender->DrawPath(&InkPath);
			InkPath.IsStroked = FALSE;

			// re-enable the path processors
			pRender->EnableAllBrushPathProcessors(TRUE);
		}
	}

	m_bStopRender = TRUE;

	// Reset the Lock Flag
	m_bLock = FALSE;

	pRender->RestoreContext();

//	TRACEUSER( "Gerry", _T("Finished rendering into region 0x%08x\n"),pRender);
}  


/********************************************************************************************
>	BOOL NodeBevel::RenderBitmapForDisplay(RenderRegion * pRegion)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/07/00
	Inputs:		The render region to be rendered into!
	Returns:	TRUE for success, FALSE for Failure!
	Purpose:	Creates the Brigtness bitmap for the bevel.

********************************************************************************************/
BOOL NodeBevel::RenderBitmapForDisplay(RenderRegion * pRegion)
{
	TRACEUSER( "Gerry", _T("NodeBevel::RenderBitmapForDisplay\n"));

	// Get the Bevel Controler
	NodeBevelController * pControl = (NodeBevelController *)GetParentController();
	
	// Make sure we`ve got some valid pointers!
	if (pControl == NULL)
		return FALSE;

	// (ChrisG - 14/2/2001) If we've got a zero sized (or less) bitmap, get rid of
	//	the old beveller and let it try to recreate the bitmap (hopefully at a 
	//	friendlier zoom factor).
	if (m_pBeveler != NULL)
	{
		if (m_BitmapHeight < 1 || m_BitmapWidth < 1)
		{
			delete m_pBeveler;
			m_pBeveler = NULL;
		}
	}

	if (m_pBeveler == NULL)
	{
		if (!CreateBevelBitmap())
			return(FALSE);
	}

	if (m_pBevelBitmap)
	{
		delete m_pBevelBitmap;
		m_pBevelBitmap = NULL;
	}

	// Save the current Pixel scale
	if(pRegion)
		m_LastPixelSize = pRegion->GetScaledPixelWidth();

	PORTNOTETRACE("other","NodeBevel::RenderBitmapForDisplay - removed printing support");
#ifndef EXCLUDE_FROM_XARALX
	if (pRegion && pRegion->IS_KIND_OF(PrintingMaskedRenderRegion))
		CalculateBitmapSize(&m_BitmapWidth, &m_BitmapHeight, pRegion);
	else
#endif
		CalculateBitmapSize(&m_BitmapWidth, &m_BitmapHeight, NULL);

	// Get the pixel size of the current clipping rect! SHOULD be pixel alligned.
	INT32 Width = m_BitmapWidth;
	INT32 Height = m_BitmapHeight;

	if (Width == 0 || Height == 0)
		return FALSE;

//	TRACEUSER( "Gerry", _T("Creating bevel bitmap (%d, %d)\n"), Width, Height);

	// Create a new bitmap of the current width and height!
	m_pBevelBitmap = new KernelBitmap(Width, Height, 8, 96, TRUE);
	ERROR2IF(m_pBevelBitmap == NULL,FALSE,"Failed to create a new bevel bitmap!");

	// Now render the bevel into the new Bevel Bitmap!
	m_pBeveler->RenderToBitmap(m_pBevelBitmap, TRUE, &m_SubPixelOffset);

	// If we're going to change the transp fill then the combi
	// bitmap will become out of date so uncache it here.
	if (m_pCombiBitmap)
	{
		delete m_pCombiBitmap;
		m_pCombiBitmap = NULL;
	}

	// Now set the Bitmap Ref to the Bevel Bitmap!
	m_pBMPFill->BitmapRef.SetBitmap(m_pBevelBitmap);
	m_pTranspFill->BitmapRef.SetBitmap(m_pBevelBitmap);
	// Everythings cool, return TRUE!
	return TRUE;
}

/********************************************************************************************

>	void NodeBevel::RenderBitmap(RenderRegion* pRender)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/00
	Inputs:		pRender	- The render region to use for rendering
	Purpose:	Renders the bevel using the bitmap bevelling system

	Function Outline :-
	
	1. Pass in render region.
	2. Check pointers for Region and Bitmap - return if failed.
	3. Save Region Context.
	4. Get a pointer to the current stroke colour attrib.
	5. Get the pointer for the parent bevel controller node.
	6. Check pointers for attrib and node - restore contrext and return if failed.
	7. Get the DocColour from the stroke attrib.
	8. Get the current pixel width from the render region.
	9. Get the pointer to the current view.
	10. If valid do 11, else goto 26. !!!
		11. Test that the current Pixel width is not equal to m_ViewScale,
			Test that m_ShrunkPath is not empty,
			Test the printing flag in the render region.
		12. If any one is true, do 13. else goto 26. !!!
			13. Check to see if the bevel is outer or inner.
			14. If bevel is inner then do 15, else do 17.
				15. Clear the m_ShrunkPath.
				16. Contour the InkPath out by 1/2 pixel into m_ShrunkPath.
			17. Else
				18. Clone m_OuterBevelPath into m_ShrunkPath.
				19. Contour m_Path in by 1/2 pixel into InnerMergePath.
				20. Reverse InnerMergePath and Merge with m_ShrunkPath.
				21. Clone m_ShrunkPath into InkPath.
			22. Close m_ShrunkPath.
			23. Set Filled and !Stroked
			24. If m_bZeroLineWidth is true set the line width to 50.
			25. m_VeiwScale = Pixel Width.
	26. if m_bZeroLineWidth do 27 else do 28.
		27. Set LineColour to NONE.
		
		28. Set FillColour to BLACK.
	29. Render m_ShrunkPath.
	30. If Bevel is an inner do 31 else do 35.
		31. Create a new winding rule attribute.
		32. if pointer is NULL do 33. else do 34.
			33. Restore Context and return.
		34. Render the new Winding Attrib.
	35. create a new AttrFillMappingLinear and set to repeat inverted.
	36. Render the new attrib.
	37. if m_bZeroLineWidth do 38. else do 40.
		38. Set the LineColour to COLOUR_NONE.
		39. Set the LineWidth to 50;
	40. Check the BMPFill pointer and do 41 if valid else do 44.
		41. Set up the fill handles.
		42. Set the repeat type to norepeattype.
		43. Render the fill into the region.
	44. Create a new TransparencyFillAttrib
	45. Check the pointer, if valid do 46 else do 50.
		46. Set the start transparency to 0.
		47. Set the end transparency to 0xff.
		48. Set the transparency type to 25 (Brightness)
		49. Render the TransparencyFillAttrib into the region.
		
		50. Restore the Context and return.
	51. Render the InkPath.
	52. Restore the context and tidy up the pointers
	53. return.

********************************************************************************************/
void NodeBevel::RenderBitmap(RenderRegion* pRender)
{
//	TRACEUSER( "Gerry", _T("NodeBevel::RenderBitmap\n"));

	// check for valid pointers
	if(!pRender)
		return;

	// We should always do this so that 0% flat trans bevelled objects get treated
	// as transparent by the printing system
	// Otherwise we end up trying to generate a 72000 dpi bevel bitmap thanks
	// to the ScaledPixelWidth value of PrintPSRenderRegions
	if(pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)))
	{
		// with blends, we don't have a bitmap because this is generated on the fly
		// but a bitmap is NOT generated for a scanning render region ....
		ScanningRenderRegion* pScanner = (ScanningRenderRegion*)pRender;
		// we MUST force this node to be included
		pScanner->ComplexShapeFoundWrapper();
		return;
	}

	// Check to see if we`ve got a bitmap for rendering!
	if(!m_pBevelBitmap)
		return;

	// Save the render regions context so we can mess about and restore all at the end
	pRender->SaveContext();

	// Get the current Stroke colour and also a pointer to the bevel controller
	NodeBevelController* pControl = (NodeBevelController *)GetParentController();
//	NodeAttribute* pFillAttrib = NULL;
	FillMappingAttribute FillMapAttrib;
	TranspFillMappingAttribute TranspFillMapAttrib;

	// Check for valid pointers
	if (pControl == NULL)
	{
		pRender->RestoreContext();
		return;
	}

	// first, render the inking path with the current settings no lines needed!
	// reduce/extend the ink path slightly to account for antialiasing glitches
	// only do this if the view scale has changed or we've re-rendered the bevel
	double PixelWidth  = pRender->GetScaledPixelWidth();

	// Get the current View pointer
	View* pView = View::GetCurrent();

	// if the view is valid then proceed
	if (pView)
	{
		// If the view scale is diffrent to the Pixel Width OR
		// the Shrunk Path is empty OR we are currently printing, then do the following
		if (m_ViewScale != PixelWidth || m_ShrunkPath.GetNumCoords() == 0 || pControl->IsPrinting())
		{
			// If this flag is set then set the line width to 50
			if (m_bZeroLineWidth)
				pRender->SetLineWidth(50);
			
			// Make the viewscale variable equal to the PixelWidth
			m_ViewScale = (UINT32)PixelWidth;
		}
	}

	// if we`re doing an inner bevel then we need to render a Winding Rule Attrib
	if (!m_bOuter)
	{
		// Create a new Winding Rule Attrib
//		pRender->SetWindingRule(PositiveWinding);
	}

	// if set, make sure we set the render region to have a line colour of NONE
	// and a line width of 50.
	if (m_bZeroLineWidth)
	{
		pRender->SetLineColour(COLOUR_NONE);
		pRender->SetLineWidth(50);
	}

	if(m_IsFlatFilledOnly) // Flat filled
	{
//		TRACEUSER( "Gerry", _T("Flat filled bevel\n"));
		// If we`ve got a valid pointer to the bitmap then setup the handles and render into
		// the render region
		FillMapAttrib.Repeat = 0;
		FillMapAttrib.Render(pRender);

		if (m_pBMPFill)
		{
//			TRACEUSER( "Gerry", _T("Rendering Bitmap Fill!\n"));
			m_pBMPFill->SetStartPoint(&m_BMPCentre);
			m_pBMPFill->SetEndPoint  (&m_BMPPoint1);
			m_pBMPFill->SetEndPoint2 (&m_BMPPoint2);
			m_pBMPFill->Render(pRender);
		}
		else
		{
			pRender->RestoreContext();
			ERROR1RAW(_R(IDE_NOMORE_MEMORY));
		}
	}
	else if(!m_IsFlatFilledOnly && !m_HasTransparencyApplied) // Bitmap or Gradient Filled
	{
//		TRACEUSER( "Gerry", _T("Non-flat non-transp bevel\n"));
		// The pointer is valid then do the following
		if (m_pTranspFill)
		{
			TranspFillMapAttrib.Repeat = 0;
			TranspFillMapAttrib.Render(pRender);

//			TRACEUSER( "Gerry", _T("Rendering Transparent Fill\n"));
			m_pTranspFill->SetStartPoint(&m_BMPCentre);
			m_pTranspFill->SetEndPoint  (&m_BMPPoint1);
			m_pTranspFill->SetEndPoint2 (&m_BMPPoint2);
			m_pTranspFill->Render(pRender);
		}
		else
		{
			pRender->RestoreContext();
			ERROR1RAW(_R(IDE_NOMORE_MEMORY));
		}
	}
	else if(!m_IsFlatFilledOnly && m_HasTransparencyApplied)
	{
//		TRACEUSER( "Gerry", _T("Non-flat transp bevel\n"));
		// Check for the special case of the bevel having a bitmapfill AND a transparency applied
		// This is special as we need to use the transparency attribute for the brightness bitmap, therefore
		// we need to render the current fill and transparency into a 32Bit bitmap and use this as the bitmap fill
		// leaving the transparency attribute free for our brightness bitmap!!!

		// Make sure we have valid pointers before doing anything to it!
		if(m_pBMPFill)
		{
			if(!pRender->IsKindOf(CC_RUNTIME_CLASS(ScanningRenderRegion)))
			{
				if(!m_pCombiBitmap)
				{
					// This involves the new technique of creating a new render region whilst still
					// in the rendering loop! Therefore we need to make sure we save the current GDraw Context.
//					TRACEUSER( "Gerry", _T("Creating a new OffScreen Bitmap!\n"));
					
					// Call the new OffSreen Bitmap Function to get the new 32Bit BMP Fill!
					// Check to see if it actually worked or failed!
					m_pCombiBitmap = CreateCombinedBMPAndTranspFill(pRender);
				}

				if(m_pCombiBitmap == NULL)
				{
					ERROR3("Failed to create a new BitmapTransparency Bitmap!");
					pRender->RestoreContext();
					return;
				}

				// Now setup the new Bitmap Fill Attribute and render it into the render region!
				m_pBMPFill->BitmapRef.SetBitmap(m_pCombiBitmap);

				m_pBMPFill->SetStartPoint(&m_BMPCentre);
				m_pBMPFill->SetEndPoint  (&m_BMPPoint1);
				m_pBMPFill->SetEndPoint2 (&m_BMPPoint2);
				m_pBMPFill->Render(pRender);
			}
			else
			{
//				TRACEUSER( "Gerry", _T("Scanning RR\n"));
			}

			FillMapAttrib.Repeat = 0;
			FillMapAttrib.Render(pRender);
		}
		else
		{
//			TRACEUSER( "Gerry", _T("No m_pBMPFill\n"));
			pRender->RestoreContext();
			ERROR1RAW(_R(IDE_NOMORE_MEMORY));
		}
	}

	// We need to get a bitmap for the fill
	if(InkPath.GetNumCoords() > 2)
		pRender->DrawPath(&InkPath);

	pRender->RestoreContext();
//	TRACEUSER( "Gerry", _T("End of NodeBevel::RenderBitmap\n"));
}

/********************************************************************************************

>	KernelBitmap* CreateCombinedBMPAndTranspFill(RenderRegion* pRegion = NULL)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/07/00
	Inputs:		pRegion	- The render region to use for rendering
	return:		An OIL Bitmap pointer of the new Bitmap!
	Purpose:	Creates a new 32Bit Bitmap to hold the combined Transp & Bitmap Fill Attributes.

***********************************************************************************************/
KernelBitmap* NodeBevel::CreateCombinedBMPAndTranspFill(RenderRegion* pRegion)
{
	// Get the current view pointer
	View* pView = View::GetCurrent();

	// check the passed in pointer and view pointer
	if(pRegion == NULL || pView == NULL)
		return NULL;

	// Create a local OILBitmap pointer ready to hold the new BMP
	// Now we want a 32Bit BMP so lets setup a render region!
	KernelBitmap* pNewBMP = NULL;

	// Get the current pixel width as a scale value.
	double GScale = 1.0;
	double SPW = pRegion->GetScaledPixelWidth();
//	TRACEUSER( "Gerry", _T("ScaledPixelWidth = %g\n"), SPW);

	// Do a quick check to make sure we don`t get any funny values!
	// 0 will obviously crash the dive later on!
	// Anything above 3750 will result in a scale factor of less than 0.2 and as this only happens
	// at zoom factors of 50% or less, we might as well leave the quality at a higher value as the 
	// bitmaps will be very small anyway!
	if(SPW <= 0.0 || SPW > 3750.0)
		SPW = 3750.0;

//	TRACEUSER( "Gerry", _T("Using ScaledPixelWidth = %g\n"), SPW);

	DocRect RRect(m_BMPPoint2.x,m_BMPPoint1.y,m_BMPPoint1.x,m_BMPPoint2.y);// = m_PixelAllignedRect;

	// Get the Pixel Width and Height using the scaled pixel width.
	double ScaledWidth = (double)RRect.Width() / SPW;
	double ScaledHeight = (double)RRect.Height() / SPW;

//	TRACEUSER( "Gerry", _T("scaled size (%g, %g)\n"), ScaledWidth, ScaledHeight);

	// Check to see if we`ve gone over the bounds!
	if((ScaledWidth > BevelMaxBitmapWidth) || (ScaledHeight > BevelMaxBitmapHeight))
	{
		// See which is the greater error
		if((BevelMaxBitmapWidth / ScaledWidth) >= (BevelMaxBitmapHeight / ScaledHeight))
		{
			// WIDTH
			SPW = 750.0 / (BevelMaxBitmapWidth / ScaledWidth);
			GScale = BevelMaxBitmapWidth / ScaledWidth;
		}
		else
		{
			// HEIGHT
			SPW = 750.0 / (BevelMaxBitmapHeight / ScaledHeight);
			GScale = BevelMaxBitmapHeight / ScaledHeight;
		}

//		TRACEUSER( "Gerry", _T("Using changed ScaledPixelWidth = %g\n"), SPW);
	}

// This GScale stuff causes problems and this routine should always create
// a bitmap of the same size as the source bitmaps so it will always use 1.0
// Gerry 22/06/2004
#if FALSE
	// Get a pointer to the controller node!
	NodeBevelController * pControl = (NodeBevelController *)FindParent();
	if(pControl && pRegion->IsPrinting())
	{
		GScale = 96;

		Document* pDoc = pView->GetDoc();

		PrintComponent *pPrintComponent = (PrintComponent *)pDoc->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent));
		if(pPrintComponent)
		{
			PrintControl *pPrintControl = pPrintComponent->GetPrintControl();
			if(pPrintControl)
				GScale = 1.0 * pPrintControl->GetDotsPerInch() / 96.0;
		}
	}
	else
	{
		GScale = 750.0 / SPW;

		if(GScale > 1.0)
			GScale = 1.0;
	}
#endif

//	TRACEUSER( "Gerry", _T("GScale = %g\n"), GScale);

	// Now Expand the Path by 1 Pixel!
	Path InflatePath;
	InflatePath.Initialise();
	InflatePath.CreatePathFromDocRect(&RRect);

	// Get the Current Applied Fill from the bevel.
	NodeAttribute * pFillAttr = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttr);

	// Get the Current Applied MappingFill from the bevel.
	NodeAttribute * pFillMapAttr = NULL;
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillMapping),&pFillMapAttr);

	BOOL bOldForce = pView->SetForceDefaultColourContexts();

	// We need to save the current GDraw Context before we try to setup a new region!
	ConcurrentRenderer* pGOffScreen = new ConcurrentRenderer;
	if (pGOffScreen == NULL)
	{
		pView->SetForceDefaultColourContexts(bOldForce);
		ERROR2(NULL, "Failed to create a new Borrower Class!");
	}

	// (ChrisG 8/5/2001) Try to initialise the context. If this fails, then we won't 
	//	have a bitmap to render things into, so we can't do a lot else.
	if (pGOffScreen->Initialise((GRenderRegion*)pRegion, GScale, RRect, 32, TRUE))
	{		
		GRenderRegion* pGBMP = pGOffScreen->GetRenderRegion();

		// Now make sure the pointer is valid!
		if(pGBMP)
		{
			pGBMP->GetDrawContext()->SetAntialiasFlag(0);

			// If found, render the current Fill Attribute
			if(pFillAttr)
				pFillAttr->Render(pGBMP);

			if(pFillMapAttr)
				pFillMapAttr->Render(pGBMP);

			pGBMP->SetLineColour(COLOUR_NONE);
			pGBMP->SetLineWidth(50);

			if (m_pTranspFill)
			{
				TRACEUSER( "MarkH", _T("Rendering Transparent Fill!\n"));
				m_pTranspFill->SetStartPoint(&m_BMPCentre);
				m_pTranspFill->SetEndPoint  (&m_BMPPoint1);
				m_pTranspFill->SetEndPoint2 (&m_BMPPoint2);
				m_pTranspFill->Render(pGBMP);
			}

			// We need to render the bevel object with the current attributes into the new region!
			pGBMP->DrawPath(&InflatePath);

			// NewBMP Now needs to be setup with the newly created bitmap
			pNewBMP = pGOffScreen->GetKernelBitmap();

//			TRACEUSER( "Gerry", _T("Created combined bitmap (%d, %d)\n"), pNewBMP->GetWidth(), pNewBMP->GetHeight());
		}
		else
		{
			ERROR3("Failed to create a new GRenderBitmap!");
		}

		// Now restore the original render region!
		pGOffScreen->RestorePreviousRendererState();
	}
	else
	{
		// Initialise failed and we don't have a bitmap to give back, so clean up and give 
		//	nothing back.
		delete pGOffScreen;
		pNewBMP = NULL;
	}

	pView->SetForceDefaultColourContexts(bOldForce);

	// Return the new Pointer
	return pNewBMP;
}

/***********************************************************************************************

>	void NodeBevel::CalculateBitmapSize(INT32 * retnWid,  INT32 * retnHei, RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/98
	Inputs:		Return pointers for witdth & height
	Purpose:	Calculates the width and the height for the bitmap depending on what's in m_Path

***********************************************************************************************/
void NodeBevel::CalculateBitmapSize(INT32 * retnWid,  INT32 * retnHei, RenderRegion * pRegion)
{
	// Get a pointer to the controller node!
	NodeBevelController * pControl = (NodeBevelController *)FindParent();

	// Check that all the pointers are valid!
	if(!retnWid || !retnHei || !pControl)
		return;

	// Get the bounding rect of the bevel!
	DocRect dr(0,0,0,0);

	// If it`s an outer then get the bounds that gavin creates when he generates his trap list!
	if(m_bOuter)
	{
		if(m_pBeveler)
			dr = m_pBeveler->GetSelectionRect();
		else
		{
			InkPath.GetTrueBoundingRect(&dr);
			dr.Inflate(m_Indent);
		}
	}
	else
	{
		// If it`s an inner then just get the bounds of the object and not the bevel path
		m_Path.GetTrueBoundingRect(&dr);
	}

	// Make sure we check to see if the returned rect is not empty!
	if (dr.IsEmpty())
	{
		*retnWid = 0;
		*retnHei = 0;
		return;
	}

	MILLIPOINT PixSize = 750;
	
	if(pRegion)
		PixSize = pRegion->GetScaledPixelWidth();

	PORTNOTETRACE("other","NodeBevel::CalculateBitmapSize - removed printing support");
	if(Document::GetCurrent() && Document::GetCurrent()->GetFirstDocView()
#ifndef EXCLUDE_FROM_XARALX
			 && !(pRegion && pRegion->IS_KIND_OF(PrintingMaskedRenderRegion))
#endif
		)
		PixSize = (MILLIPOINT)((double)PixSize / (Document::GetCurrent()->GetFirstDocView())->GetViewScale().MakeDouble());
	
	if(PixSize == 0)
		PixSize = 1;

	m_PixelAllignedRect = dr;
	double lWidth  = dr.Width() / PixSize;
	double lHeight = dr.Height() / PixSize;
	
	// Check to see if we`re not printing and then Scale up the values!
	PORTNOTETRACE("other","NodeBevel::CalculateBitmapSize - removed printing support");
#ifndef EXCLUDE_FROM_XARALX
	if ((!pControl->IsPrinting() && !m_AmConvertingToShapes) ||
		(pRegion && pRegion->IS_KIND_OF(PrintingMaskedRenderRegion)))
#else
	if (!m_AmConvertingToShapes)
#endif
	{
		// If the current render regions scale value is greater than 0 do the following
		if (pRegion && PixSize > 0 )
		{
			lWidth = (lWidth * 750) / PixSize;
			lHeight = (lHeight * 750) / PixSize;
		}
		
		// Make sure the values are under the maximum values!
		while ( lWidth  > BevelMaxBitmapWidth || lHeight > BevelMaxBitmapHeight )
		{
			// Divide them by a half!
			lWidth *= 0.5;
			lHeight *= 0.5;
		}
	}
	else
	{
		double DPI = pControl->GetDPI();
		lWidth  = dr.Width () * DPI / 72000 ;
		lHeight = dr.Height() * DPI / 72000 ;
	}

	// Now place the new values into the passed in variables
	*retnWid = (INT32)ceil(lWidth);
	*retnHei = (INT32)ceil(lHeight);
}

/***********************************************************************************************

>	void NodeBevel::SetUpBitmapPalette(RenderRegion * pRegion)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com> - Rewrite of David Mc code
	Created:	19/07/00
	Inputs:		Currently a pointer to a render region, But is not used.
	Purpose:	Sets up the bitmap palette depending on contrast setting

***********************************************************************************************/
void NodeBevel::SetUpBitmapPalette(RenderRegion * pRegion, DocColour FlatColour)
{
	// Check the Bevel Bitmap Pointer
	if (!m_pBevelBitmap)
		return;
	
	// Check that the Bitmap is an 8Bit BMP
	if (m_pBevelBitmap->GetBPP() != 8)
		return;

	// Setup a few local variables
	LPRGBQUAD pPalette = m_pBevelBitmap->GetPaletteForBitmap();
	INT32 Red = 0;
	INT32 Green = 0;
	INT32 Blue = 0;

	// Get the pointer to the parent controller and check it
	NodeBevelController* pControl = (NodeBevelController *)FindParent();
	ERROR3IF(pControl == NULL, "No parent");

	// Get the contrst value from the controller
	double Contrast = pControl->m_Contrast;

	// The following if statment never gets called as the pointer is always NULL
	if ( pRegion )
	{
		// find out the contrast first
		BevelAttributeValueContrast *pVal = (BevelAttributeValueContrast *)pRegion->GetCurrentAttribute(ATTR_BEVELCONTRAST);
		if (!pVal)
			return;
		Contrast = pVal->m_Contrast;
	}

	GDrawContext* pGD = GRenderRegion::GetStaticDrawContext();
	pGD->SetBevelLightness(UINT32(         Contrast*2.5599900)) ;
	pGD->SetBevelDarkness (UINT32(Contrast*Contrast*0.0255999)) ;

	// Get the component parts of the FillColour
	FlatColour.GetRGBValue(&Red,&Green,&Blue);

	// Now setup the palette of 255 grey scales OR if the bevel is flat filled and NOT transparent
	// then the palette is translated using the flat fill colour
	for ( INT32 i=0 ; i<256 ; i++ )
	{
		if(!m_IsFlatFilledOnly)
		{
			// Normal Brightness grey scales
			pPalette[i].rgbRed   =
			pPalette[i].rgbBlue  =
			pPalette[i].rgbGreen = i ;
		}
		else
		{
			// Translated colour table values used ONLY for Non Transparent Flat filled Bevels
	PORTNOTETRACE("other","NodeBevel::SetUpBitmapPalette - palette bodge");
#if defined(__WXGTK__)
			pPalette[i].rgbRed   = pGD->TranslateBevelValue(i,Blue ) ;
			pPalette[i].rgbGreen = pGD->TranslateBevelValue(i,Green) ;
			pPalette[i].rgbBlue  = pGD->TranslateBevelValue(i,Red  ) ;
#else
			pPalette[i].rgbRed   = pGD->TranslateBevelValue(i,Red  ) ;
			pPalette[i].rgbGreen = pGD->TranslateBevelValue(i,Green) ;
			pPalette[i].rgbBlue  = pGD->TranslateBevelValue(i,Blue ) ;
#endif
		}
		pPalette[i].rgbReserved = 0 ;
	}
}

/********************************************************************************************

>	KernelBitmap* CreateBitmapCopy(double DPI = -1.0, BOOL CheckFillFlags = TRUE);

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/12/00
	Inputs:		DPI to render with, if this is -1.0 the code will find the options value.
				CheckFillFlags when TRUE will use the member variable flags to decide what
				to render. FALSE render it anyway.
	Purpose:	Creates a BitmapCopy of the Bevel Bitmap including the current fills and
				transparency settings generated at the given DPI!

********************************************************************************************/
KernelBitmap* NodeBevel::CreateBitmapCopy(double DPI, BOOL CheckFillFlags)
{
	m_AmConvertingToShapes = TRUE;

	NodeBevelController* pControl = (NodeBevelController *)FindParent();
	
	INT32 NewDPI = 96;
	if(pControl && DPI == -1.0)
	{
		View* pView = View::GetCurrent();
		if (pView)
			NewDPI = (INT32)pView->GetConvertToEditableShapesDPI();
	}
	else
		NewDPI = (INT32)DPI;

	if(pControl)
		pControl->SetDPI(NewDPI);

	// The bitmap has not been rendered at this stage so we need to call the
	// appropriate render function!
	ERROR2IF(!RenderBitmapForDisplay(NULL),FALSE,"Failed to create the bevel bitmap!");

	NodeAttribute* pFillAttrib = NULL;
	NodeAttribute* pFillMapAttr = NULL;
	NodeAttribute* pFillTranspMapAttr = NULL;
	NodeAttribute* pFillEffectAttr = NULL;
	DocColour FillColour = COLOUR_BLACK;

	// Get the Current Applied MappingFill from the bevel.
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillMapping),&pFillMapAttr);
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillMapping),&pFillTranspMapAttr);
	FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillEffect),&pFillEffectAttr);

	// Test to see if theres a FillGeom Attribute applied to the bevel
	if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttrib))
	{
		// If the pointer is valid and It`s a flat fill then get the colour and set the IsFlatFill Flag
		if(pFillAttrib != NULL && pFillAttrib->IsAFlatFill())
			FillColour = *((AttrFlatColourFill*)pFillAttrib)->GetStartColour();
	}

	// make sure the palette is setup ready for the BMP
	SetUpBitmapPalette((RenderRegion *)NULL,FillColour);

	KernelBitmap* pConvertedBMP = NULL;
	
	// We now need to check to see if we`re a Fourth state bevel, in which case we need to
	// Create a new fill bitmap using the current Bevel Bitmap!
	if(!m_IsFlatFilledOnly && (m_HasTransparencyApplied || !CheckFillFlags))
	{
		// Ok, Lets setup a few variables need to create the new BMP
		View*	pView = View::GetCurrent();
		Matrix	Mat(1,0,0,1,0,0);
		FIXED16 Scale = 1;
		DocRect RenderRect(m_BMPPoint2.x,m_BMPPoint1.y,m_BMPPoint1.x,m_BMPPoint2.y);
		Path InflatePath;
		InflatePath.Initialise();
		InflatePath.CreatePathFromDocRect(&RenderRect);
		
		// Check to see if we`ve got an empty rect if so then return NULL!
		ERROR2IF(RenderRect.IsEmpty(),NULL,"We`ve got an empty DocRect in the selection! (CBMPBits)");

		// Create a new GRenderBitmap region
		GRenderBitmap* pGBMP = new GRenderBitmap(RenderRect, Mat, Scale, 32, NewDPI, FALSE, 0, NULL, TRUE);

		if(pGBMP)
		{
			pGBMP->m_DoCompression = TRUE;
			
			if(!pGBMP->AttachDevice(pView, NULL, NULL))
			{
				delete pGBMP;
				pGBMP = NULL;
			}
			else
			{
				// Get the Current Applied Fill from the bevel.
				NodeAttribute * pFillAttr = NULL;
				FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttr);

				pGBMP->InitDevice();
				pGBMP->InitAttributes();
				pGBMP->RRQuality.SetQuality(QUALITY_MAX);
				pGBMP->SetQualityLevel();
				pGBMP->SetLineAttributes();
				pGBMP->SetFillAttributes();

				pGBMP->StartRender();

				pGBMP->GetDrawContext()->SetAntialiasFlag(0);

				// If found, render the current Fill Attribute
				if(pFillAttr)
					pFillAttr->Render(pGBMP);

				if(pFillMapAttr)
					pFillMapAttr->Render(pGBMP);

				if(pFillEffectAttr)
					pFillEffectAttr->Render(pGBMP);

				pGBMP->SetLineColour(COLOUR_NONE);
				pGBMP->SetLineWidth(50);

				if (m_pTranspFill)
				{
					TRACEUSER( "MarkH", _T("Rendering Transparent Fill!\n"));
					m_pTranspFill->SetStartPoint(&m_BMPCentre);
					m_pTranspFill->SetEndPoint  (&m_BMPPoint1);
					m_pTranspFill->SetEndPoint2 (&m_BMPPoint2);
					m_pTranspFill->Render(pGBMP);
				}

				// We need to render the bevel object with the current attributes into the new region!
				pGBMP->DrawPath(&InflatePath);
			
				pGBMP->StopRender();

				pConvertedBMP = new KernelBitmap(pGBMP->ExtractBitmapCopy());
				delete pGBMP;
				pGBMP = NULL;
			}
		}
	}
	else
	{
		// Now make a copy of the bevel bitmap!
		pConvertedBMP = new KernelBitmap(m_pBevelBitmap->ActualBitmap->MakeCopy());
	}

	return pConvertedBMP;
}

/********************************************************************************************

>	BOOL NodeBevel::DoBecomeA(BecomeA* pBecomeA);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		BecomeA struct
	Purpose:	Converts me to a path
	SeeAlso:	

********************************************************************************************/
BOOL NodeBevel::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param							   
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	if (!pBecomeA->BAPath())
		return FALSE;

	// This lump checks that the Reason is one that we understand
	// It also makes sure that we don't have a NULL UndoOp ptr
	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	NodePath				* pNewNodePath		= NULL;
	Node					* pBevelStartNode	= NULL;
	NodeRenderableInk		* pParent			= NULL;
	NodeHidden				* pHiddenNode		= NULL;
	NodeAttribute			* pAttr				= NULL;
//	NodeAttribute			* pAttrT			= NULL;
//	AttrStrokeTransp	    * pStrokeColour1	= NULL;
//	BitmapFillAttribute		* pBitmapFill		= NULL;
//	AttrFillMappingLinear	* pFillMapping		= NULL;
	AttrStrokeColour		* pStrokeColour		= NULL;
	AttrLineWidth			* pLineWidth		= NULL;
	
	NodeAttribute* pFillMapAttr = NULL;
	NodeAttribute* pFillTranspMapAttr = NULL;
	NodeAttribute* pFillEffectAttr = NULL;
	
//	UINT32 startTrans = 0x00;
//	UINT32 endTrans	= 0xff;

	DocColour dcNone(COLOUR_NONE);

	switch (pBecomeA->GetReason())
	{
	case BECOMEA_REPLACE:
		{
			// Mark Howitt 15/08/00 - Rewrite of david code!!!!
			// What we need to do here is be quite warry of the current set up of our bevels!! There are four types we can
			// have which are as follows...
			//
			// 1. Flat filled bevel NO transparency. This type is as with all the others is made up of just one
			// shape that just has a bitmap fill of the bevel.
			// 2. Flat filled WITH transparency. As type 1 but with the transparency applied as normal.
			// 3. NON Flat fill NO transparency. This type has the bevel bitmap as a transparency bitmap fill with applied
			// NON flat fill as normal.
			// 4. NON Flat filled WITH Transparency. NOTE! These are a special case as both the transparent and normal fill
			// attributes are already taken! In this case there is an off screen Bitmap render of the normal fill and the bevel bitmap
			// as a tranparency ( Not the applied transparency ) which replaces the normal fill. This means the bevel bitmap
			// is merged into the original fill to make a new one( 32Bit Alpha Channeled!!!) The Original transparency is rendered
			// as normal!
			// 
			// As you can see we`ve got 4 cases that are different and will need to be converted in different ways!
			// The main variables which decide the the current state are...
			// m_IsFlatFilledOnly - which tells you wether you`ve got just a flat fill attribute.
			// m_HasTransparencyApplied - which indicates wether there is any transparency applied.
			// therefore the states match up as follows...
			//
			// 1. m_IsFlatFilledOnly && !m_HasTransparencyApplied
			// 2. m_IsFlatFilledOnly && m_HasTransparencyApplied
			// 3. !m_IsFlatFilledOnly && !m_HasTransparencyApplied
			// 4. !m_IsFlatFilledOnly && m_HasTransparencyApplied

			// CGS:  actually this is more complex - because we can have ilans wonderful feathers knocking
			// around in here !!!!  I have decided that the best way to account for this is to check for
			// the feather attribute directly within each of marks four cases ....

			// First things first, lets make a new NodePath to hold all the bevel info!
			// Get a pointer to the undo op and create a new path undoably!
			UndoableOperation* pOp = pBecomeA->GetUndoOp();
			ALLOC_WITH_FAIL(pNewNodePath, new NodePath, pOp); 
/*
			NodeAttribute* pFillAttrib = NULL;
			DocColour FillColour = COLOUR_BLACK;

			// Get the Current Applied MappingFill from the bevel.
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillMapping),&pFillMapAttr);
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillMapping),&pFillTranspMapAttr);
			FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillEffect),&pFillEffectAttr);

			// Test to see if theres a FillGeom Attribute applied to the bevel
			if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttrib))
			{
				// If the pointer is valid and It`s a flat fill then get the colour and set the IsFlatFill Flag
				if(pFillAttrib != NULL && pFillAttrib->IsAFlatFill())
					FillColour = *((AttrFlatColourFill*)pFillAttrib)->GetStartColour();
			}

			// make sure the palette is setup ready for the BMP
			SetUpBitmapPalette((RenderRegion *)NULL,FillColour);

			KernelBitmap* pConvertedBMP = NULL;
			
			// We now need to check to see if we`re a Fourth state bevel, in which case we need to
			// Create a new fill bitmap using the current Bevel Bitmap!
			if(!m_IsFlatFilledOnly && m_HasTransparencyApplied)
			{
				// Ok, Lets setup a few variables need to create the new BMP
				View*	pView = View::GetCurrent();
				Matrix	Mat(1,0,0,1,0,0);
				FIXED16 Scale = 1;
				DocRect RenderRect(m_BMPPoint2.x,m_BMPPoint1.y,m_BMPPoint1.x,m_BMPPoint2.y);
				Path InflatePath;
				InflatePath.Initialise();
				InflatePath.CreatePathFromDocRect(&RenderRect);
				
				// Check to see if we`ve got an empty rect if so then return NULL!
				ERROR2IF(RenderRect.IsEmpty(),NULL,"We`ve got an empty DocRect in the selection! (CBMPBits)");

				// Create a new GRenderBitmap region
				GRenderBitmap* pGBMP = new GRenderBitmap(RenderRect, Mat, Scale, 32, DPI, FALSE, 0, NULL, TRUE);

				if(pGBMP)
				{
					pGBMP->m_DoCompression = TRUE;
					
					if(!pGBMP->AttachDevice(pView, NULL, NULL))
					{
						delete pGBMP;
						pGBMP = NULL;
					}
					else
					{
						// Get the Current Applied Fill from the bevel.
						NodeAttribute * pFillAttr = NULL;
						FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pFillAttr);

						pGBMP->InitDevice();
						pGBMP->InitAttributes();
						pGBMP->RRQuality.SetQuality(QUALITY_MAX);
						pGBMP->SetQualityLevel();
						pGBMP->SetLineAttributes();
						pGBMP->SetFillAttributes();

						pGBMP->StartRender();

						pGBMP->GetDrawContext()->SetAntialiasFlag(0);

						// If found, render the current Fill Attribute
						if(pFillAttr)
							pFillAttr->Render(pGBMP);

						if(pFillMapAttr)
							pFillMapAttr->Render(pGBMP);

						if(pFillEffectAttr)
							pFillEffectAttr->Render(pGBMP);

						pGBMP->SetLineColour(COLOUR_NONE);
						pGBMP->SetLineWidth(50);

						if (m_pTranspFill)
						{
							TRACEUSER( "MarkH", _T("Rendering Transparent Fill!\n"));
							m_pTranspFill->SetStartPoint(&m_BMPCentre);
							m_pTranspFill->SetEndPoint  (&m_BMPPoint1);
							m_pTranspFill->SetEndPoint2 (&m_BMPPoint2);
							m_pTranspFill->Render(pGBMP);
						}

						// We need to render the bevel object with the current attributes into the new region!
						pGBMP->DrawPath(&InflatePath);
					
						pGBMP->StopRender();

						pConvertedBMP = new KernelBitmap(pGBMP->ExtractBitmapCopy());
						delete pGBMP;
						pGBMP = NULL;
					}
				}
			}
			else
			{
				// Now make a copy of the bevel bitmap!
				pConvertedBMP = new KernelBitmap(m_pBevelBitmap->ActualBitmap->MakeCopy());
			}
*/
			// If the path to be bevelled has no points then don't insert anything to replace this node
			BOOL bCreateNew = (m_Path.GetNumCoords() != 0);

			KernelBitmap* pConvertedBMP = NULL;

			if (bCreateNew)
			{
				pConvertedBMP = CreateBitmapCopy();
				
				if(!pConvertedBMP)
				{
					ERROR3("Failed To Create Bevel Bitmap Copy!");
				}
				else
				{
					NodeCompound* pControl = GetParentController();
					if (pControl != NULL)
					{
						LPBITMAPINFOHEADER pBevelHeader = pConvertedBMP->GetBitmapInfoHeader();
						const double PixPerMetre = pControl->GetDPI() * INCHES_PER_METRE;
						pBevelHeader->biXPelsPerMeter = pBevelHeader->biYPelsPerMeter = (INT32)(PixPerMetre + 0.5);
					}

					pConvertedBMP->Attach(Document::GetCurrent()->GetBitmapList());
				}

				// Now delete the bevel bitmap as when undoing it will automatically be recreated!
				if(m_pBevelBitmap)
				{
					delete m_pBevelBitmap;
					m_pBevelBitmap = NULL;
				}

				// Make sure that our fill attributes reference the new bitmap!
				m_pBMPFill->BitmapRef.SetBitmap(pConvertedBMP);
				m_pTranspFill->BitmapRef.SetBitmap(pConvertedBMP);

				if (m_pCombiBitmap)
				{
					delete m_pCombiBitmap;
					m_pCombiBitmap = NULL;
				}

				m_pBMPFill->SetStartPoint(&m_BMPCentre);
				m_pBMPFill->SetEndPoint(&m_BMPPoint1);
				m_pBMPFill->SetEndPoint2(&m_BMPPoint2);

				m_pTranspFill->SetStartPoint(&m_BMPCentre);
				m_pTranspFill->SetEndPoint(&m_BMPPoint1);
				m_pTranspFill->SetEndPoint2(&m_BMPPoint2);

				// Copy the current bevel path into the new path with flattening
				pNewNodePath->InkPath.Initialise();
				pNewNodePath->InkPath.CloneFrom(InkPath);
				pNewNodePath->InkPath.IsFilled = TRUE;
				pNewNodePath->InkPath.TryToClose();
				pNewNodePath->InkPath.InitialiseFlags();

				// Make sure that we copy across the Feather attribute if there`s one there!
				if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFeather),&pAttr))
					pAttr->CopyNode(pNewNodePath, FIRSTCHILD);

				// First state check
				if(m_IsFlatFilledOnly && !m_HasTransparencyApplied)
				{
					// STATE 1
					if(m_pBMPFill)
					{
						// Make sure the Fill attributes point to the new bitmap!
						pAttr = m_pBMPFill->MakeNode();
						pAttr->AttachNode(pNewNodePath,FIRSTCHILD);
					}
				}
				else if((m_IsFlatFilledOnly && m_HasTransparencyApplied) || (!m_IsFlatFilledOnly && m_HasTransparencyApplied))
				{	
					// STATE 2 OR STATE 4
					if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry),&pAttr))
						pAttr->CopyNode(pNewNodePath, FIRSTCHILD);

					if(m_pBMPFill)
					{
						// Make sure the Fill attributes point to the new bitmap!
						pAttr = m_pBMPFill->MakeNode();
						pAttr->AttachNode(pNewNodePath,FIRSTCHILD);
					}
				}
				else
				{
					// STATE 3
					if(FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pAttr))
						pAttr->CopyNode(pNewNodePath, FIRSTCHILD);

					if(m_pTranspFill)
					{
						// Make sure the Fill attributes point to the new bitmap!
						pAttr = m_pTranspFill->MakeNode();
						pAttr->AttachNode(pNewNodePath,FIRSTCHILD);
					}
				}

				// Make sure the line width is none as they shouldn`t have outlines!
				ALLOC_WITH_FAIL(pLineWidth, new AttrLineWidth, pOp);
				pLineWidth->Value.LineWidth = 50;
				pLineWidth->AttachNode(pNewNodePath, FIRSTCHILD);

				// Also make the line colour NONE
				ALLOC_WITH_FAIL(pStrokeColour, new AttrStrokeColour, pOp);
				pStrokeColour->SetStartColour(&dcNone);
				pStrokeColour->AttachNode(pNewNodePath, FIRSTCHILD);

				if(pFillMapAttr)
					pFillMapAttr->CopyNode(pNewNodePath,FIRSTCHILD);

				if(pFillTranspMapAttr)
					pFillTranspMapAttr->CopyNode(pNewNodePath,FIRSTCHILD);

				if(pFillEffectAttr)
					pFillEffectAttr->CopyNode(pNewNodePath,FIRSTCHILD);

			}

			m_AmConvertingToShapes = FALSE;

			// hide the bevel start node & me and
			// insert the ink path node
			pParent = (NodeRenderableInk *)FindParent();
			
			if (pParent)
			{
				// first, remove all existing line widths
				// and also all bevel attributes

// TODOG: This must all work with a NULL pOp pointer!!!
				
				pBevelStartNode = pParent->FindFirstChild(CC_RUNTIME_CLASS(NodeBevelBegin));

				if (pOp)
				{
					pOp->DoRemoveAttrTypeFromSubtree(pParent, CC_RUNTIME_CLASS(AttrBevelLightAngle));
					pOp->DoRemoveAttrTypeFromSubtree(pParent, CC_RUNTIME_CLASS(AttrBevelType));
					pOp->DoRemoveAttrTypeFromSubtree(pParent, CC_RUNTIME_CLASS(AttrBevelContrast));
					pOp->DoRemoveAttrTypeFromSubtree(pParent, CC_RUNTIME_CLASS(AttrBevelIndent));

					if (bCreateNew)
					{
						// insert all the nodes
						if(!m_bOuter)
							pOp->DoInsertNewNode(pNewNodePath, pParent, LASTCHILD, TRUE, FALSE, FALSE, TRUE);
						else
							pOp->DoInsertNewNode(pNewNodePath, pBevelStartNode, NEXT, TRUE, FALSE, FALSE, TRUE);
					}

					if (pBevelStartNode)
						pOp->DoHideNode(pBevelStartNode, TRUE, &pHiddenNode, FALSE);
				}
				else
				{
					pParent->RemoveAttrTypeFromSubtree(CC_RUNTIME_CLASS(AttrBevelLightAngle));
					pParent->RemoveAttrTypeFromSubtree(CC_RUNTIME_CLASS(AttrBevelType));
					pParent->RemoveAttrTypeFromSubtree(CC_RUNTIME_CLASS(AttrBevelContrast));
					pParent->RemoveAttrTypeFromSubtree(CC_RUNTIME_CLASS(AttrBevelIndent));

					if (bCreateNew)
					{
						// insert all the nodes
						if(!m_bOuter)
							pNewNodePath->AttachNode(pParent, LASTCHILD, FALSE, FALSE);
						else
							pNewNodePath->AttachNode(pBevelStartNode, NEXT, FALSE, FALSE);
					}

					if (pBevelStartNode)
					{
						pBevelStartNode->CascadeDelete();
						delete pBevelStartNode;
					}
				}
			}
						
			if (bCreateNew)
			{
				CCAttrMap ThisMap;
				if (!pNewNodePath->FindAppliedAttributes(&ThisMap))
					return FALSE;

				CCAttrMap* pThisMap = ThisMap.Copy();
				if (pThisMap)
				{
					if (pThisMap->Lookup(CC_RUNTIME_CLASS(AttrStrokeColour), (void *&)(pStrokeColour)))
					{
						if (pStrokeColour && pStrokeColour->GetStartColour())
							pStrokeColour->SetStartColour(&dcNone);
					}
				}
				pBecomeA->PassBack(pNewNodePath, this, pThisMap);
			}

			// now hide me too
			if (pOp)
				pOp->DoHideNode(this, TRUE, &pHiddenNode, FALSE);
			else
			{
				CascadeDelete();
				delete this;
			}
		}
		break;
	case BECOMEA_PASSBACK:
		{
			// we need to pass back 2 node paths, with appropriate attributes maps
			pNewNodePath = new NodePath;
			ERRORIF(pNewNodePath == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
			
			// clone the ink path from me !
			pNewNodePath->InkPath.Initialise();
			pNewNodePath->InkPath.CloneFrom(InkPath);
			pNewNodePath->InkPath.IsFilled = TRUE;
			pNewNodePath->InkPath.TryToClose();
			pNewNodePath->InkPath.InitialiseFlags();
			
			// if we're passing back, just do the first path
			CCAttrMap ThisMap;
			if (!FindAppliedAttributes(&ThisMap))
				return FALSE;

			// make a copy of this attribute map
			CCAttrMap * pNewAttrMap = ThisMap.Copy();
			// get the stroke colour attribute out of this map and set it to COLOUR_NONE
			if( pNewAttrMap->Lookup( CC_RUNTIME_CLASS(AttrStrokeColour), (void *&)(pStrokeColour)))
			{
				if (pStrokeColour && pStrokeColour->GetStartColour())
					pStrokeColour->SetStartColour(&dcNone);
			}
			// ok, now lets set up the attribute maps
			pBecomeA->PassBack(pNewNodePath, this, pNewAttrMap);
		}
		break;
		default:
			break;
	}
	return TRUE;
}

/********************************************************************************************

>	BOOL NodeBevel::CanBecomeA(BecomeA* pBecomeA);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		Ink class to test
	Purpose:	Can become a path !
	SeeAlso:	

********************************************************************************************/

BOOL NodeBevel::CanBecomeA(BecomeA* pBecomeA)
{
	if (pBecomeA->BAPath())
	{
		pBecomeA->AddCount(2);

		return TRUE;
	}

	return FALSE;
}

/********************************************************************************************
>	void NodeBevel::RenderTinyBlobs(RenderRegion* pRender)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/98
	Inputs:		-
	Returns:	See base class
	Purpose:	
********************************************************************************************/
void NodeBevel::RenderTinyBlobs(RenderRegion* pRender)
{
	pRender->SaveContext();
	pRender->SetLineColour(COLOUR_NONE);
	pRender->SetFillColour(COLOUR_UNSELECTEDBLOB);

	// Render the blobs on the path
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();

	DocRect dr(0,0,0,0);
	InkPath.GetTrueBoundingRect(&dr);

	DocCoord Coord(dr.lo.x + (BlobMgr->GetBlobSize() / 2), 
		dr.hi.y - (BlobMgr->GetBlobSize() / 2));
	pRender->DrawBlob(Coord, BT_UNSELECTED);
	pRender->RestoreContext();
}

/********************************************************************************************

>	BitmapFillAttribute* NodeBevel::GetBitmapFillAttribute ( void )

 	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/12/99
	Inputs:		-
	Returns:	A constant pointer to m_pBMPFill.
	Purpose:	Returns a pointer to the bitmap fill contained within NoveBevel.

********************************************************************************************/
BitmapFillAttribute* NodeBevel::GetBitmapFillAttribute ( void )
{
	return m_pBMPFill;
}

/********************************************************************************************
>	DocRect	NodeBevel::GetBoundingRect(BOOL DontUseAttrs = FALSE, BOOL HitTest = FALSE)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/10/98
	Inputs:		-
	Returns:	The bounding rect for the bevel
	Purpose:	
********************************************************************************************/

DocRect NodeBevel::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if ( IsBoundingRectValid && !DontUseAttrs )
		return BoundingRectangle;
	
	DocRect dr(0,0,0,0);

	// run through all previous nodes calculating their bounding rects
	if (InkPath.GetNumCoords() == 0)
	{
		Node* pNode = NULL;

		if(m_bOuter)
			pNode = FindNext(CC_RUNTIME_CLASS(NodeRenderableBounded));
		else
			pNode = FindPrevious(CC_RUNTIME_CLASS(NodeRenderableBounded));

		if (!pNode)			
			return DocRect(0,0,0,0);
		
		while (pNode)
		{
			if (pNode->IsRenderable() && !pNode->IsAnAttribute() &&
				!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelBegin)) &&
				!pNode->IsNodeHidden())
			{
				if (pNode->IsNodePath())
				{
					// rough guess of how big the path bounding rect is
					NodePath * pNodePath = ((NodePath *)pNode)->MakeNodePathFromAttributes(BevelClipPathToPath_Flatness);

					DocRect dr2(0,0,0,0);
					pNodePath->InkPath.GetTrueBoundingRect(&dr2);
					dr = dr.Union(dr2);
					
					delete pNodePath;
				}
				else
					dr = dr.Union(((NodeRenderableBounded *) pNode)->GetBoundingRect(DontUseAttrs, HitTest));
			}
			
			if(m_bOuter)
				pNode = pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableBounded));
			else
				pNode = pNode->FindPrevious(CC_RUNTIME_CLASS(NodeRenderableBounded));
		}
		
		BoundingRectangle = dr;
		
		if (m_bOuter)
			BoundingRectangle.Inflate(m_Indent * 2);
	}
	else
		InkPath.GetTrueBoundingRect(&BoundingRectangle);
	
	IsBoundingRectValid = TRUE;

	return BoundingRectangle;
}


DocRect NodeBevel::GetBlobBoundingRect()
{
	// now, find out if the blob manager's interest is in artistic blobs or not
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	BlobStyle BS = BlobMgr->GetCurrentInterest(TRUE);

	INT32 BlobSize = BlobMgr->GetBlobSize();
	DocRect br = GetBoundingRect();
	br.lo.x -= BlobSize;
	br.lo.y -= BlobSize;
	br.hi.x += BlobSize;
	br.hi.y += BlobSize;

	IncludeChildrensBoundingRects(&br);
	
PORTNOTE("other","Removed BevelTool from NodeBevel::GetBlobBoundingRect");
#ifndef EXCLUDE_FROM_XARALX
	// is the bevel tool active ?
	if (Tool::GetCurrentID() != TOOLID_BEVELTOOL || !BevelTool::AmActive())
		return br;
#endif

	DocRect BlobRect;

	// calculate the position of the light angle blob
	DocCoord blob( (br.hi.x+br.lo.x)>>1, 
				   (br.hi.y+br.lo.y)>>1 );

	View * pView = View::GetCurrent();
	if (!pView)
		return br;

	INT32 iLen = (INT32)(pView->GetScaledPixelWidth().MakeDouble()*40) ;

	// get from the blob manager the size of blobs
	if (pView)
	{
		iLen += (INT32)(pView->GetScaledPixelWidth().MakeDouble()*15);
		iLen += GetApplication()->GetBlobManager()->GetBlobSize();
	}

	DocRect dr;
	// calculate the rect for the blob
	dr.lo.x = blob.x - iLen;
	dr.lo.y = blob.y - iLen;
	dr.hi.x = blob.x + iLen;
	dr.hi.y = blob.y + iLen;

	BlobRect = br;
	// finally, union this with the standard group's bounding rect to give
	// the true blob bounding rect
	BlobRect = BlobRect.Union(dr);

	return BlobRect;
}

/********************************************************************************************

>	virtual void NodeBevel::Transform( TransformBase& Trans )

	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/11/94
	Inputs:		Trans - The transform Object
	Purpose:	Transforms the shape.
	SeeAlso:	NodePath::Transform()

********************************************************************************************/
void NodeBevel::Transform( TransformBase& tb )
{
	if(!m_MustRegenOnChildChange)
		return;

	// get the scale factor for the transformation
	FIXED16 ScaleFactor = tb.GetScalar();

	TransformChildren(tb);
	Trans2DMatrix * pMat2D = NULL;
	if (tb.IsInvertable())
		pMat2D = (Trans2DMatrix*)(&tb);

	// And scale the line width
	m_Indent= labs( INT32(ScaleFactor.MakeDouble() * ((double)m_Indent) ));

	NodeBevelController * pControl = (NodeBevelController *)FindParent();
	if (pControl)
		pControl->m_Indent = m_Indent;

	AttrBevelIndent * pApplied = NULL;
	AttrBevelIndent * pIndent = NULL;

	if ( pMat2D &&
		 !pMat2D->GetMatrix().IsTranslation() &&
		 FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelIndent),(NodeAttribute **)&pApplied) &&
		 pApplied->IsADefaultAttr() )
	{
		// we need to insert an attribute onto me
		pIndent = new AttrBevelIndent(FindParent(), FIRSTCHILD);
		if (pIndent)
		{
			pIndent->Value.m_Indent = m_Indent;
			if (m_bOuter)
				pIndent->Value.m_Indent = -m_Indent;
			((NodeRenderableInk *)FindParent())->NormaliseAttributes();
		}
	}
			
	// transform the paths
	if (m_Path.GetNumCoords() > 0)
		tb.Transform(m_Path.GetCoordArray(), m_Path.GetNumCoords());

	if (m_OuterBevelPath.GetNumCoords() > 0)
		tb.Transform(m_OuterBevelPath.GetCoordArray(), m_OuterBevelPath.GetNumCoords());

	if (InkPath.GetNumCoords() > 0)
		tb.Transform(InkPath.GetCoordArray(), InkPath.GetNumCoords());

	if (m_ShrunkPath.GetNumCoords() > 0)
		tb.Transform(m_ShrunkPath.GetCoordArray(), m_ShrunkPath.GetNumCoords());

	// and regenerate the bitmap (if necessary)
	if (pMat2D)
	{
		if (!pMat2D->GetMatrix().IsTranslation())
		{
			GenerateBevel();
		}
		else
		{
			DocCoord TransCoords(0,0);
			pMat2D->GetMatrix().GetTranslation(TransCoords);

//			if(TransCoords.x % 750 != 0 || TransCoords.y % 750 != 0)
//				GenerateBevel();
//			else
			{
				// transform the source points of the bitmap fill
				tb.Transform(&m_BMPCentre, 1);
				tb.Transform(&m_BMPPoint1, 1);
				tb.Transform(&m_BMPPoint2, 1);
				tb.Transform(&BoundingRectangle.lo, 1);
				tb.Transform(&BoundingRectangle.hi, 1);
			}
		}
	}

	InvalidateBoundingRect();
	GetBoundingRect(TRUE, FALSE);
}

/********************************************************************************************
>	BOOL NodeBevel::NeedsTransparency()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/98
	Inputs:		-
	Returns:	TRUE
	Purpose:	We sure do need transparency !
********************************************************************************************/
BOOL NodeBevel::NeedsTransparency() const
{
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL Bevel::RequiresAttrib(CCRuntimeClass* pAttribClass,
											BOOL Search = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/2000
	Inputs:		pAttribClass	the attr type to test.
				Search			not a clue - look at base class implementation for info.

	Returns:	FALSE if pAttribClass is a brush attr,
				calls base implementation otherwise.

	Purpose:	NodeBevel does not require brush attrs.

	Errors:		returns FALSE if pAttribClass is NULL.

********************************************************************************************/
BOOL NodeBevel::RequiresAttrib(CCRuntimeClass* pAttribClass, BOOL Search)
{
	if (pAttribClass == NULL || pAttribClass == CC_RUNTIME_CLASS(AttrBrushType))
		return FALSE;
	else
		return NodeRenderableInk::RequiresAttrib(pAttribClass, Search);
}



/********************************************************************************************

>	virtual BOOL NodeBevel::RequiresAttrib(NodeAttribute* pAttr,
											BOOL Search = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/2000
	Inputs:		pAttr		the attr to test.
				Search		not a clue - look at base class implementation for info.

	Returns:	FALSE if pAttr is a brush attr,
				calls base implementation otherwise.

	Purpose:	NodeBevel does not require brush attrs.

	Errors:		returns FALSE if pAttr is NULL.

********************************************************************************************/
BOOL NodeBevel::RequiresAttrib(NodeAttribute* pAttr, BOOL Search)
{
	if (pAttr == NULL || pAttr->IS_KIND_OF(AttrBrushType))
		return FALSE;
	else
		return NodeRenderableInk::RequiresAttrib(pAttr, Search);
}



/********************************************************************************************

>	virtual BOOL NodeBevel::CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/2000
	Inputs:		pAttribClass	the attr type to test.
	Returns:	FALSE if pAttribClass is a brush attr,
				TRUE otherwise.

	Purpose:	Determine whether the given attr type can be applied directly to me.
				We don't like brushed bevels, so we don't accept brush attrs.

	Errors:		returns FALSE if pAttribClass is NULL.

********************************************************************************************/
BOOL NodeBevel::CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass)
{
	if (pAttribClass == NULL || pAttribClass == CC_RUNTIME_CLASS(AttrBrushType))
		return FALSE;
	else
		return TRUE;
}




/********************************************************************************************
>	virtual NodeCompound* NodeBevel::GetParentController() const

 	Author:		Peter_Arnold (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/12/96
	Inputs:		-
	Returns:	Pointer to parent Bevel controller (NULL if not found!)
	Purpose:	Returns a type correct pointer to the parent Bevel controller
********************************************************************************************/
NodeCompound* NodeBevel::GetParentController() const
{
	NodeBevelController* pBob = (NodeBevelController*)FindParent();
	if (pBob != NULL && !IS_A(pBob, NodeBevelController))
	{
		ERROR3("Parent of Bevel was not a NodeBevelController");
		pBob = NULL;
	}

	return pBob;
}

/********************************************************************************************
>	void NodeBevel::DeleteCachedBitmap()

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/98
	Inputs:		-
	Returns:	
	Purpose:	Deletes the current Bevel Bitmap. Mainly used when printing to reduce
				memory usage.
********************************************************************************************/
void NodeBevel::DeleteCachedBitmap()
{
	if(m_pBevelBitmap)
	{
		m_pBevelBitmap->Detach();
		m_pBevelBitmap->DestroyGreyscaleVersion();
		delete m_pBevelBitmap;
		m_pBevelBitmap = NULL;
	}

	if(m_pCombiBitmap)
	{
		delete m_pCombiBitmap;
		m_pCombiBitmap = NULL;
	}
}

/********************************************************************************************
>	void NodeBevel::DeleteCache()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/98
	Inputs:		-
	Returns:	
	Purpose:	Deletes all data held in this class
********************************************************************************************/
void NodeBevel::DeleteCache()
{
	m_bCached = FALSE;
	
	if (m_pBevelBitmap)
	{
		m_pBevelBitmap->Detach();
		delete m_pBevelBitmap;
		m_pBevelBitmap = NULL;
	}

	if (m_pBeveler)
	{
		delete m_pBeveler;
		m_pBeveler = NULL;
	}

	InkPath.ClearPath();
	m_OuterBevelPath.ClearPath();
	m_ShrunkPath.ClearPath();
	m_Path.ClearPath();
	InvalidateMe();
}

/********************************************************************************************
>	BOOL NodeBevel::ReRenderBevelBitmap(BOOL bAntialias)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/98
	Inputs:		-
	Returns:	
	Purpose:	Re-renders the bevel bitmap using the beveler as it's already set up
				with the currently applied attributes
********************************************************************************************/
BOOL NodeBevel::ReRenderBevelBitmap(BOOL bAntialias)
{
	BOOL ok = TRUE;
	
	if (m_pBeveler && m_pBevelBitmap)
	{
		// if the beveler is present, and we have a bitmap then re-render the bitmap
		m_pBeveler->SetBevelAngle(m_LightAngle);
		m_pBeveler->SetBevelType(m_BevelType);
		m_pBeveler->SetBevelTilt(m_Tilt);

		if(m_pCombiBitmap)
		{
			delete m_pCombiBitmap;
			m_pCombiBitmap = NULL;
		}

		// if we're antialiasing, then do the wait cursor
		if (bAntialias)
			m_DoingRotation = FALSE;
		else
			m_DoingRotation = TRUE;

		m_pBevelBitmap->DestroyGreyscaleVersion();
		ok = m_pBeveler->RenderToBitmap(m_pBevelBitmap, bAntialias, &m_SubPixelOffset);
	}

	return ok;
}


/********************************************************************************************
>	BOOL NodeBevel::IsNodePath() const

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/98
	Inputs:		-
	Returns:	FALSE always since I'm not a path - just am pretending to be one
	Purpose:	
********************************************************************************************/
BOOL NodeBevel::IsNodePath() const
{
	return FALSE;
}

/********************************************************************************************
>	BOOL NodeBevel::WritePreChildrenWeb(BaseCamelotFilter* pFilter)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	FALSE for failure
	Purpose:	Writes out a dummy record, for my attribute to be attached to
********************************************************************************************/
BOOL NodeBevel::WritePreChildrenWeb(BaseCamelotFilter* pFilter)
{
	UINT32 Tag = TAG_BEVELINK;

	CamelotFileRecord Rec(pFilter,Tag,0);

	pFilter->Write(&Rec);

	return TRUE;
}

BOOL NodeBevel::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	return WritePreChildrenWeb(pFilter);	
}



/********************************************************************************************
>	Node* NodeBevel::SimpleCopy()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Returns a copy of me
********************************************************************************************/
Node* NodeBevel::SimpleCopy()
{
	NodeBevel * pNewNode = new NodeBevel;

	ERRORIF(pNewNode == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	CopyNodeContents(pNewNode);

	return pNewNode;
}




/********************************************************************************************
>	void NodeBevel::CopyNodeContents(NodeBevel* pNewNode)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2004
	Inputs:		-
	Returns:	
	Purpose:	Sort of backwards copy constructor

********************************************************************************************/
void NodeBevel::CopyNodeContents(NodeBevel* pNewNode)
{
	pNewNode->m_Indent     = m_Indent;
	pNewNode->m_LightAngle = m_LightAngle;
	pNewNode->m_Contrast   = m_Contrast;
	pNewNode->m_BevelType  = m_BevelType;
	pNewNode->m_bOuter	   = m_bOuter;

	pNewNode->InkPath.Initialise(InkPath.GetNumCoords());
	pNewNode->InkPath.CopyPathDataFrom(&InkPath);

	pNewNode->m_MustRegenOnChildChange = m_MustRegenOnChildChange;

	pNewNode->m_Tilt = m_Tilt;

	pNewNode->m_IsABlendStepBevel = m_IsABlendStepBevel;

	// points determining the fill control points
	pNewNode->m_BMPCentre = m_BMPCentre;
	pNewNode->m_BMPPoint1 = m_BMPPoint1;
	pNewNode->m_BMPPoint2 = m_BMPPoint2;

	// the rect of the (original) selection
	pNewNode->m_SelectedRect = m_SelectedRect;
	pNewNode->m_JointType = m_JointType;

	pNewNode->m_Path.Initialise(m_Path.GetNumCoords());
	pNewNode->m_Path.CopyPathDataFrom(&m_Path);

	pNewNode->m_DoRender = TRUE;

	pNewNode->DeleteCachedBitmap();

	if (m_pBevelBitmap==NULL && m_pBMPFill!=NULL)
		m_pBMPFill->BitmapRef.SetBitmap(m_pBevelBitmap);
	if (m_pBevelBitmap==NULL && m_pTranspFill!=NULL)
		m_pTranspFill->BitmapRef.SetBitmap(m_pBevelBitmap);

	if (pNewNode->m_pBMPFill)
		delete pNewNode->m_pBMPFill;
	pNewNode->m_pBMPFill = NULL;
	if (m_pBMPFill)
	{
		pNewNode->m_pBMPFill = new BitmapFillAttribute;
		pNewNode->m_pBMPFill->SimpleCopy(m_pBMPFill);
	}

	if (pNewNode->m_pTranspFill)
		delete pNewNode->m_pTranspFill;
	pNewNode->m_pTranspFill = NULL;
	if (m_pTranspFill)
	{
		pNewNode->m_pTranspFill = new BitmapTranspFillAttribute;
		pNewNode->m_pTranspFill->SimpleCopy(m_pTranspFill);
	}

	pNewNode->m_BitmapWidth = m_BitmapWidth;
	pNewNode->m_BitmapHeight = m_BitmapHeight;

	pNewNode->m_SubPixelOffset = m_SubPixelOffset;
	pNewNode->m_PixelAllignedRect = m_PixelAllignedRect;
	pNewNode->m_IsFlatFilledOnly = m_IsFlatFilledOnly;
	pNewNode->m_IsAnAlphaBMP = m_IsAnAlphaBMP;
	pNewNode->m_HasTransparencyApplied = m_HasTransparencyApplied;
	pNewNode->m_AmConvertingToShapes = m_AmConvertingToShapes;

	pNewNode->m_LastCachedPrintBMPDPI = m_LastCachedPrintBMPDPI;
	pNewNode->m_LastExportedDPI = m_LastExportedDPI;

	// lock on rendering
	pNewNode->m_bStopRender = m_bStopRender;
	pNewNode->m_DoingRotation = m_DoingRotation;

	// this is the bevel path generated by the path processor for outer bevels (without
	// the hole in it)
	pNewNode->m_OuterBevelPath.Initialise(m_OuterBevelPath.GetNumCoords());
	pNewNode->m_OuterBevelPath.CopyPathDataFrom(&m_OuterBevelPath);

	// the pixel sizes used to create the bevel bitmap
	pNewNode->m_ViewScale = m_ViewScale;

	// the shrunk path used in rendering
	pNewNode->m_ShrunkPath.Initialise(m_ShrunkPath.GetNumCoords());
	pNewNode->m_ShrunkPath.CopyPathDataFrom(&m_ShrunkPath);

	pNewNode->m_bLock = m_bLock;
	pNewNode->m_bRenderBitmaps = m_bRenderBitmaps;

	// indicates we should zero the line width of the path, or not (as the case might be)
	pNewNode->m_bZeroLineWidth = m_bZeroLineWidth;

	// Make sure the new node doesn't have an old CBeveler lying around.
	if (pNewNode->m_pBeveler)
	{
		delete pNewNode->m_pBeveler;
		pNewNode->m_pBeveler = NULL;
	}

	// whether the original path is closed or not
	// if not, don't do any path shrinking
	pNewNode->m_bPathIsClosed = m_bPathIsClosed;

	// flag to indicate that when this node is inside of a blend there's
	// no need to regenerate it - it's already been cached
	pNewNode->m_bCached = m_bCached;

	// Reverses the bevel path and adds it to the original to create the new bevel path
	// Also shrinks/Expands the inner/outer path edge by a pixel to get rid of antialiasing
	// effects when overlapping the object
	pNewNode->m_LastZoomFactor = m_LastZoomFactor;

	// the last pixel size for the bitmap
	pNewNode->m_LastPixelSize = m_LastPixelSize;

	NodeRenderableInk::CopyNodeContents(pNewNode);
}




/***********************************************************************************************
>   void NodeBevel::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBevel::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBevel), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBevel))
		CopyNodeContents((NodeBevel*)pNodeCopy);
}



/********************************************************************************************
>	BOOL NodeBevel::PostDuplicate(UndoableOperation* pOp)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		-
	Returns:	-
	Purpose:	Regens the node after a duplication
********************************************************************************************/
BOOL NodeBevel::PostDuplicate(UndoableOperation* pOp)
{
	// set my variables from the parent
	return TRUE;
}


//////////////////////////////////////////////////////////////////////
// class to handle loading in of bevel records


/********************************************************************************************
>	BevelRecordHandler::BevelRecordHandler()

 	Author:		Luke_Hart (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/07/05
	Inputs:		-
	Returns:	
	Purpose:	Constructor
********************************************************************************************/
BevelRecordHandler::BevelRecordHandler()
{
}

/********************************************************************************************
>	BevelRecordHandler::~BevelRecordHandler()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Destructor
********************************************************************************************/
BevelRecordHandler::~BevelRecordHandler()
{
}

/********************************************************************************************
>	UINT32*	BevelRecordHandler::GetTagList()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Gets the tag list for this handler to handle 
********************************************************************************************/
UINT32*	BevelRecordHandler::GetTagList()
{
	static const UINT32 TagList[] = {TAG_BEVEL,
									TAG_BEVATTR_INDENT,
									TAG_BEVATTR_LIGHTANGLE,
									TAG_BEVATTR_CONTRAST,
									TAG_BEVATTR_TYPE,
									TAG_BEVELINK, 
									CXFRH_TAG_LIST_END};
	return (UINT32*)&TagList;
}

/********************************************************************************************
>	BOOL BevelRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Handles the bevel record - basically creates a BevelController node &
				creates all the attributes
********************************************************************************************/
BOOL BevelRecordHandler::HandleRecord(CXaraFileRecord* pCXaraFileRecord)
{
	// load in the bevel information and the source path for the node
	// create the bevel node and include in the tree

//	AttrBevelIndent *	  pBevIndent = NULL;
//	AttrBevelLightAngle * pBevLightAngle = NULL;
//	AttrBevelContrast *   pBevContrast = NULL;
//	AttrBevelType *       pBevType = NULL;

	INT32 value = 0;

	if (pCXaraFileRecord->GetTag() == TAG_BEVEL)
	{
		m_pBevelController = new NodeBevelController();

		if (!m_pBevelController)
		{
			ERROR(_R(IDE_NOMORE_MEMORY), FALSE);
		}

		BOOL ok = m_pBevelController->LoadBevelFromRecord(pCXaraFileRecord);
		if (!ok)
		{
			delete m_pBevelController ;
			m_pBevelController = NULL;
			return FALSE;
		}
	
		// insert it into the tree
		InsertNode(m_pBevelController);
	}
	else if (pCXaraFileRecord->GetTag() == TAG_BEVELINK)
	{
		// create the bevel node an insert it
		m_pBevelInk = new NodeBevel;

		if (!m_pBevelInk)
		{
			ERROR(_R(IDE_NOMORE_MEMORY), FALSE);			
		}

		InsertNode(m_pBevelInk);
		m_pBevelInk->InvalidateMe();
	}
	else if (pCXaraFileRecord->GetTag() == TAG_BEVATTR_INDENT)
	{
		// read the record
		if (!pCXaraFileRecord->ReadINT32(&value))
			return FALSE;
	}
	else if (pCXaraFileRecord->GetTag() == TAG_BEVATTR_LIGHTANGLE)
	{
		// read the record
		if (!pCXaraFileRecord->ReadINT32(&value))
			return FALSE;
	}
	else if (pCXaraFileRecord->GetTag() == TAG_BEVATTR_CONTRAST)
	{
		// read the record
		if (!pCXaraFileRecord->ReadINT32(&value))
			return FALSE;
	}
	else if (pCXaraFileRecord->GetTag() == TAG_BEVATTR_TYPE)
	{
		// read the record
		if (!pCXaraFileRecord->ReadINT32(&value))
			return FALSE;
	}
	
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL NodeBevel::NeedsParent(Node* pNode) const


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/99
	Inputs:		A node to do the class checking on
	Returns:	
	Purpose:	Do we need a parent of the same class as the given node ???
	See also:	NodeCompound::OnChildChange

********************************************************************************************/
BOOL NodeBevel::NeedsParent(Node* pNode) const
{
	if (!pNode)
		return TRUE;
	
	if (pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
		return TRUE;

	return FALSE;
}

/********************************************************************************************

>	DocRect NodeBevel::GetInsideBoundingRect()


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/7/99
	Inputs:		
	Returns:	A bounding rect
	Purpose:	Gets the bounding rect of the original (non-bevelled) objects
	See also:	NodeCompound::OnChildChange

********************************************************************************************/
DocRect NodeBevel::GetInsideBoundingRect()
{
	Node * pNode = FindPrevious();

	DocRect dr(0,0,0,0);

	while (pNode)
	{
		if (pNode->IsAnObject() && !pNode->IsNodeHidden() && pNode->IsBounded() &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelBegin)))
		{
			dr = dr.Union(((NodeRenderableBounded *)pNode)->GetBoundingRect());
		}
		pNode = pNode->FindPrevious();
	}

	return dr;
}

/********************************************************************************************

>	void NodeBevel::DrawNodesForPrinting(RenderRegion * pRender)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/99
	Inputs:		
	Returns:	
	Purpose:	Renders all original nodes again
	Notes:		Used because in printing, problems occur with the ShrinkPath stuff, causing
				holes to appear in the printed image. Thus, we take a different tack of rendering
				the bevelled path without the original being taken away from it, and then rendering
				the original nodes again so no holes occur
	See also:	NodeBevel::RenderBitmap

********************************************************************************************/
void NodeBevel::RenderNodesForPrinting(RenderRegion * pRender)
{
	PORTNOTETRACE("other","NodeBevel::RenderNodesForPrinting - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// first, save the context
	pRender->SaveContext();

	// next get the first child node of the controller
	Node* pController = (Node*)GetParentController();

	Node* pNode = pController->FindFirstChild();

	while (pNode)
	{
		if (!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelBegin)) &&
			!pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)) &&
			!pNode->IsNodeHidden() &&
			pNode->IsNodeRenderableClass())
		{
			RenderNodeRecursively(pNode, pRender);
		}
		pNode = pNode->FindNext();
	}

	pRender->RestoreContext();
#endif
}

/********************************************************************************************

>	void NodeBevel::RenderNodeRecursively(Node * pNode, RenderRegion * pRender)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/99
	Inputs:		
	Returns:	
	Purpose:	Renders all children of this node first, then renders the node itself
	Notes:		
	See also:	NodeBevel::DrawNodesForPrinting

********************************************************************************************/
void NodeBevel::RenderNodeRecursively(Node * pNode, RenderRegion * pRender)
{
	BOOL bHasChildren = FALSE;

	// get the first child node
	Node * pChildNode = pNode->FindFirstChild();

	if (pChildNode)
	{
		// save the context if this node has children
		pRender->SaveContext();
		bHasChildren = TRUE;
	}	

	while (pChildNode)
	{
		if (!pChildNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelBegin)) &&
			!pChildNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)) &&
			!pChildNode->IsNodeHidden() &&
			pChildNode->IsNodeRenderableClass())
		{
			RenderNodeRecursively(pChildNode, pRender);
		}
		pChildNode = pChildNode->FindNext();
	}

	// now, render the node itself
	pRender->SaveContext();
	pRender->SetLineColour(COLOUR_NONE);
	pNode->Render(pRender);
	pRender->RestoreContext();
	
	// restore the context
	if (bHasChildren)
		pRender->RestoreContext();
}



/********************************************************************************************

>	virtual BOOL NodeBevel::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
															BOOL DoPreTriggerEdit = TRUE)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	20/11/96
	Inputs:		pParam					describes the way an op wants to change the node
				SetOpPermissionState	if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Purpose:	
	SeeAlso:	

********************************************************************************************/
BOOL NodeBevel::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
												BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam==NULL, FALSE, "NodeBevel::AllowOp; NULL pParam");

	// Set up a flag to see if any of the child objects get changed
	BOOL allowed = TRUE;

	UndoableOperation* pOp = pParam->GetOpPointer();

	// clean out the calling-child ptr, so it doesn't get passed around unintentionally.
	pParam->SetCallingChild(NULL);

	// before going further, see whether our parents allow the Op.
	if (allowed && Parent != NULL && pParam->GetDirection()!= OBJCHANGE_CALLEDBYPARENT)
	{
		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed = Parent->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if the parent allows this, then test the op for ourself.
	if (allowed && pOp)
	{
		// disallow mould and contour Ops.
PORTNOTE("other","Removed opCreateNewMould, OpPasteEnvelope, OpPastePerspective, OpCreateContour from NodeBevel::GetBlobBoundingRect");
#ifndef EXCLUDE_FROM_XARALX
		if ( pOp->IsKindOf(CC_RUNTIME_CLASS(OpCreateNewMould)) ||
			 pOp->IsKindOf(CC_RUNTIME_CLASS(OpPasteEnvelope)) ||
			 pOp->IsKindOf(CC_RUNTIME_CLASS(OpPastePerspective)) ||
			 pOp->IsKindOf(CC_RUNTIME_CLASS(OpCreateContour))
			)
			allowed = FALSE;
#endif

		if( (pOp->IsKindOf(CC_RUNTIME_CLASS(OpEditFill)) ||
			 pOp->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttribToSelected))) &&
			 !m_IsFlatFilledOnly && m_HasTransparencyApplied && m_pCombiBitmap )
		{
			// Make sure the attributes aren't pointing at the bitmap we are about to delete
			if (m_pBMPFill->GetBitmap() == m_pCombiBitmap)
				m_pBMPFill->BitmapRef.SetBitmap(NULL);
			if (m_pTranspFill->GetBitmap() == m_pCombiBitmap)
				m_pTranspFill->BitmapRef.SetBitmap(NULL);
			
			delete m_pCombiBitmap;
			m_pCombiBitmap = NULL;
		}
	}

	// if necessary, set permissions for OnChildChange.
	if (SetOpPermissionState)
		SetOpPermission(allowed ? PERMISSION_ALLOWED : PERMISSION_DENIED, TRUE);

	if (allowed)
	{
		UndoableOperation* pChangeOp = pParam->GetOpPointer();

		// check for geometry linked attributes
		BOOL InformGeomLinkedAttrs = SetOpPermissionState && pChangeOp && pChangeOp->MayChangeNodeBounds();
		if (InformGeomLinkedAttrs)
		{
			NodeAttribute* pNA = FindFirstGeometryLinkedAttr();
			while(pNA)
			{
				pNA->LinkedNodeGeometryHasChanged(pChangeOp);
				pNA = pNA->FindNextGeometryLinkedAttr();
			}
		}		

		// if we're ok so far and were asked to do a PreTriggerEdit, then
		// determine whether the Op may change the bounds of some nodes.
		// If it may, then call NameGallery::PreTriggerEdit.
PORTNOTE("other","Removed NameGallery from NodeBevel::GetBlobBoundingRect");
#ifndef EXCLUDE_FROM_XARALX
		if (DoPreTriggerEdit && pChangeOp && pChangeOp->MayChangeNodeBounds())
			allowed = NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
#endif
	}

	// return result (directly, or indirectly via a child AllowOp()) to op
	return allowed;
}



/********************************************************************************************

>	void NodeBevel::RenderEorDrag(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/11/96
	Inputs:		
	Purpose:	
	SeeAlso:	

********************************************************************************************/
void NodeBevel::RenderEorDrag(RenderRegion * pRegion)
{
	pRegion->DrawPath(&InkPath);
}

/********************************************************************************************

>	virtual DocRect NodeBevel::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Nodewill be a reversible operation,
				FALSE otherwise.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				A NodeBevel cannot itself extend, so instead it asks its parent controller
				to extend for it. Infinite recursion does not occur, as the controller node
				ignores its child node, this NodeBevel.
	Errors:		In debug builds, ERROR3 if this Node has no NodeBevelController,
				in release, we return TRUE.
	See also:	IsTypeExtendible(), Extend().

********************************************************************************************/
DocRect NodeBevel::ValidateExtend(const ExtendParams& ExtParams)
{
	Node* pBob = GetParentController();
	if (pBob == NULL)
	{
		ERROR3("NodeBevel::ValidateExtend- no controller Node found!");
		return DocRect(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX);
	}
	else
	{
		return pBob->ValidateExtend(ExtParams);
	}
}



/********************************************************************************************

>	virtual void NodeBevel::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	Some of the child nodes of this BevelNode's NodeBevelController may have
				their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.

				A NodeBevel cannot itself extend, so instead it asks its parent controller
				to extend for it. Infinite recursion does not occur, as the controller node
				ignores its child node, this NodeBevel.
	Errors:		In debug builds, ERROR3 if this Node has no NodeBevelController,
				in release, we do nothing.
	See also:	

********************************************************************************************/
void NodeBevel::Extend(const ExtendParams& ExtParams)
{
	Node* pBob = GetParentController();
	if (pBob == NULL)
	{
		ERROR3("NodeBevel::ValidateExtend- no controller Node found!");
	}
	else
		pBob->Extend(ExtParams);

	return;
}



/********************************************************************************************

>	virtual INT32 NodeBevel::EstimateNodeComplexity (OpParam* details)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/09/2000

	Inputs:		details		any data that should be used for the calculation

	Outputs:	-

	Returns:	an estimate of the nodes complexity

	Purpose:	This function estimates a complexity value for the node.  The complexity
				value is based upon the total length of all paths in the node.

	See Also:	OpBlendNodes::DeterminBlendObjectsProcessorHit ()

********************************************************************************************/

INT32 NodeBevel::EstimateNodeComplexity (OpParam* details)
{
	return (InkPath.GetUsedSlots ());
}

void NodeBevel::PreExportRender ( RenderRegion *pRegion )
{
	if( pRegion 
PORTNOTE("other","Removed ImagemapRenderRegion usage")
#ifndef EXCLUDE_FROM_XARALX
		&& !pRegion->IsKindOf( CC_RUNTIME_CLASS(ImagemapRenderRegion) )
#endif
	 )
	{
		NodeBevelController* pControl = (NodeBevelController *)FindParent();
		if(pControl)
		{
			INT32 NewDPI = 96;
			Camelot.GetPrefValue(TEXT("Displays"),TEXT("CompCToEDPI"),&NewDPI);
			pControl->SetDPI(NewDPI);
			DeleteCachedBitmap();
			m_AmConvertingToShapes = TRUE;
/*
			double ThisDPI = 96.0;
			
			if(pRegion->GetScaledPixelWidth() > 75)
				ThisDPI = (750.0 / pRegion->GetScaledPixelWidth()) * 96.0;

			if(m_LastExportedDPI != ThisDPI)
			{
				m_LastExportedDPI = ThisDPI;
				pControl->SetDPI(ThisDPI);
				DeleteCachedBitmap();
				m_AmConvertingToShapes = TRUE;
			}
			else
				m_AmConvertingToShapes = FALSE;
*/
		}
	}
	else
		m_AmConvertingToShapes = FALSE;
}

/********************************************************************************************

>	virtual BOOL NodeBevel::ExportRender ( RenderRegion *pRegion )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/99
	Inputs:		pRegion - A pointer to a RenderRegion.
	Outputs:	-
	Returns:	TRUE if custom export code is invoked, otherwise FALSE.
	Purpose:	If pRenderRegion points to a FlashRenderRegion, the bevel will be
				processed as a bitmap filled shape, and stored as a pair of records
				(shape and bitmap) within the resulting Flash file.

				If pRenderRegion points to a CMXRenderRegion, a similar conversion function
				will be invoked.

********************************************************************************************/
BOOL NodeBevel::ExportRender( RenderRegion *pRegion )
{
	PORTNOTETRACE("other","NodeBevel::ExportRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL bResult = FALSE;

	NodeBevelController * pController = (NodeBevelController *)FindParent();

	// Step 1:	If the RenderRegion is a FlashRenderRegion, use custom Flash export code.
	if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( FlashRenderRegion ) ) )
	{
		// Step 1a:	Cast the pRegion pointer to be a FlashRenderRegion pointer, so that
		//			it can access the custom Flash export methods.
		FlashRenderRegion	*pFlash = ( FlashRenderRegion* ) pRegion;

		// Step 1b:	Pass the character into the appropriate method to export the character.
		//			This is where all the hard work is done.
		bResult = pFlash->ExportBevel ( this );
	}

	// Step 2:	Otherwise check whether it's a CMXRenderRegion.
	else if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( CMXRenderRegion ) ) )
	{
		// Step 2a:	Cast the pRegion pointer to be a CMXRenderRegion pointer.
		CMXRenderRegion		*pCMX	= ( CMXRenderRegion* ) pRegion;

		// Step 2b:	Pass the character into the appropriate method to export the character.
		//			This is where all the hard work is done.
		bResult = pCMX->ExportBevel ( this );
	}

	// Step 3:	Otherwise check whether it's an AIEPSRenderRegion.
	else if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( AIEPSRenderRegion ) ) )
	{
		// Step 3a:	Cast the pRegion pointer to be a CMXRenderRegion pointer.
		AIEPSRenderRegion	*pAIEPS	= ( AIEPSRenderRegion* ) pRegion;

		// Step 3b:	Pass the character into the appropriate method to export the character.
		//			This is where all the hard work is done.
		bResult = pAIEPS->ExportBevel ( this );
	}

	// Step 4:	The export has been done successfully. Return TRUE to prevent Camelot
	//			from rendering the character out as paths as well as text. (If there's
	//			been an error, bResult will have been set to FALSE.)
	return bResult;
#else
	return FALSE;
#endif
}

/********************************************************************************************

>	virtual BOOL NodeBevel::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/2/2000
	Inputs:		pSpread		The spread in which things are happening
				PointerPos	The Location of the mouse pointer at the time of the click
				pMenu		The menu to which items should be added
	Returns:	BOOL - TRUE if the node claims the click as its own and FALSE if it is
				not interested in the click
	Purpose:	Allows the shadow to respond to the click

********************************************************************************************/
BOOL NodeBevel::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
	BOOL ok = TRUE;

	PORTNOTETRACE("other","NodeBevel::ExportRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
#ifndef NO_ADVANCED_TOOLS	
	ok = ok && pMenu->BuildCommand(TOOL_OPTOKEN_BEVEL, TRUE);
#endif
#endif
	return ok;
}


/****************************************************************************

>	void NodeBevel::GetDebugDetails( StringBase* Str )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/2005

	Inputs:		Str			- pointer to a StringBase
	Purpose:	Builds debug info for camelot tree dialog

****************************************************************************/

void NodeBevel::GetDebugDetails( StringBase* Str )
{
#if DEBUG_TREE
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	String_256 TempStr;
	String_256 TempStr2;
		
	(*Str) += TEXT( "\r\nNodeBevel Data Dump\r\n" );

	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("Type :\t#1%d\r\n"), m_BevelType);
	(*Str) += TempStr;

#endif
}



//////////////////////////////////////////////////////////////////////
// The NodeBevelBegin class - used to set up the path processor to
// build the bevel path before rendering the 

/********************************************************************************************
>	NodeBevelBegin::NodeBevelBegin()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Constructor for the NodeBevelBegin class.
	Notes:		This class is purely used to set up the bevel path processor which builds
				the bevel path
********************************************************************************************/
NodeBevelBegin::NodeBevelBegin()
{
}

/********************************************************************************************
>	NodeBevelBegin::NodeBevelBegin(Node* ContextNode,  
		 AttachNodeDirection Direction, 
		 BOOL Locked, 
		 BOOL Mangled,  
		 BOOL Marked, 
		 BOOL Selected) 
			: NodePath(ContextNode, Direction, Locked, Mangled, Marked, Selected)


 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Constructor for the NodeBevelBegin class.
	Notes:		This class is purely used to set up the bevel path processor which builds
				the bevel path
********************************************************************************************/
NodeBevelBegin::NodeBevelBegin(Node* ContextNode,  
		 AttachNodeDirection Direction, 
		 BOOL Locked, 
		 BOOL Mangled,  
		 BOOL Marked, 
		 BOOL Selected) 
			: NodeRenderableBounded(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
	
}


/********************************************************************************************
>	void NodeBevelBegin::Render(RenderRegion * rr)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Pushes the bevel path processor onto the path processor stack of the given
				render region
	Notes:		
********************************************************************************************/
void NodeBevelBegin::Render(RenderRegion * rr)
{
	return;
	// is the ink path valid in the ink node ?
/*	Node * pNode = FindNext(CC_RUNTIME_CLASS(NodeBevel));

	NodeBevelController * pControl = (NodeBevelController *)FindParent();

	if (!pControl)
		return;

	// no inside bounding rect ? no render !
	if (pControl->GetInsideBoundingRect().IsEmpty())
		return;

	if (pControl->IsPrinting())
	{
		// make sure we're intersected with the render regions clipping rectangle
		if (!(rr->GetClipRect().IsIntersectedWith(BoundingRectangle)))
			return ;
	}

	// push the contour path processor to do the extension of the node
	// only if we're an outer bevel
	BevelAttributeValueIndent * pBevelIndent = (BevelAttributeValueIndent *)
		rr->GetCurrentAttribute(ATTR_BEVELINDENT); */
}

/********************************************************************************************
>	DocRect NodeBevelBegin::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Returns the bounding rect of the bevel controller node
	Notes:		
********************************************************************************************/
DocRect NodeBevelBegin::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	// same as first sibling
	NodeBevelController* pBevelController = (NodeBevelController*)FindParent();

	if (pBevelController->GetInsideBoundingRect().IsEmpty())
		return DocRect(0,0,0,0);

	if (pBevelController)
	{
		NodeBevel* pBevel = pBevelController->GetBevelNode();

		if (pBevel)
		{
			BoundingRectangle = pBevel->GetBoundingRect(DontUseAttrs, HitTest);
			return BoundingRectangle;
		}
	}
	
	Node * pNode = this->FindNext(CC_RUNTIME_CLASS(NodeRenderableBounded));
	if (!pNode || pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevel)))
	{
		// ERROR3("NodeBevelBegin - Can't find renderable node !\n");
		return BoundingRectangle;
	}

	BoundingRectangle = ((NodeRenderableBounded *)pNode)->GetBoundingRect(DontUseAttrs, HitTest);
	return BoundingRectangle;
}


DocRect NodeBevelBegin::GetBlobBoundingRect()
{
	return GetBoundingRect(TRUE, FALSE);
}


/********************************************************************************************
>	Node* NodeBevelBegin::SimpleCopy()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Returns a copy of me
********************************************************************************************/
Node* NodeBevelBegin::SimpleCopy()
{
	NodeBevelBegin * pNewNode = new NodeBevelBegin;

	ERRORIF(pNewNode == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	return pNewNode;
}

/********************************************************************************************

>	virtual BOOL NodeBevelBegin::NeedsParent(Node* pNode)


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/99
	Inputs:		A node to do the class checking on
	Returns:	
	Purpose:	Do we need a parent of the same class as the given node ???
	See also:	NodeCompound::OnChildChange

********************************************************************************************/
BOOL NodeBevelBegin::NeedsParent(Node* pNode) const
{
	if (!pNode || pNode->IsKindOf(CC_RUNTIME_CLASS(NodeBevelController)))
		return TRUE;

	return FALSE;
}

// a NodeBevelBegin can never be selected ....

void NodeBevelBegin::SetSelected(BOOL Status)
{
	Node::SetSelected(FALSE);
}


/********************************************************************************************

>	BOOL NodeBevelBegin::NeedsToRender(RenderRegion * pRender)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/5/99
	Inputs:		A Render region
	Returns:	
	Purpose:	Is the top path processor a bevel path processor ? if so, I don't need to
				render myself
	See also:	NodeBevelBegin::Render

********************************************************************************************/

SubtreeRenderState NodeBevelBegin::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	return SUBTREE_ROOTANDCHILDREN;
}


/********************************************************************************************

>	virtual BOOL NodeBevelBegin::ExportRender ( RenderRegion *pRegion )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/99
	Inputs:		pRegion - A pointer to a RenderRegion.
	Outputs:	-
	Returns:	TRUE if the RenderRegion is a FlashRenderRegion, otherwise FALSE.
	Purpose:	If pRenderRegion points to a FlashRenderRegion, the bevel will be
				processed as a bitmap filled shape, and stored as a pair of records
				(shape and bitmap) within the resulting Flash file.

********************************************************************************************/

BOOL NodeBevelBegin::ExportRender ( RenderRegion *pRegion )
{
	PORTNOTETRACE("other","NodeBevelBegin::ExportRender - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	BOOL bResult = FALSE;

	NodeBevelController * pController = (NodeBevelController *)FindParent();

	// Step 1:	Test whether the render region is a Flash one. If so, invoke it's custom
	//			export code.
	if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( FlashRenderRegion ) ) )
	{
		// Step 1a:	Cast the pRegion pointer to be a FlashRenderRegion pointer, so that
		//			it can access the custom Flash export methods.
		FlashRenderRegion *pFlash = ( FlashRenderRegion* ) pRegion;

		// Step 1b:	Pass the character into the appropriate method to export the character.
		//			This is where all the hard work is done.
		bResult = pFlash->ExportBevelBegin ( this );
	}
	// Step 2:	Test whether the render region is a CMX one. If so, invoke it's custom
	//			export code.
	else if ( pRegion->IsKindOf ( CC_RUNTIME_CLASS ( CMXRenderRegion ) ) )
	{
		// Step 2a:	Cast the pRegion pointer to be a CMXRenderRegion pointer, so that
		//			it can access the custom CMX export methods.
		CMXRenderRegion *pCMX = ( CMXRenderRegion* ) pRegion;

		// Step 2b:	Invoke the stub function within the CMX render region.
		bResult = pCMX->ExportBevelBegin ( this );
	}
	// render bitmap stuff
	else
	{
		// ok, im intersecting so render
		Render(pRegion);

		return TRUE;
	}

	// Step 3:	The export has been done successfully. Return TRUE to prevent Camelot
	//			from rendering the character out as paths as well as text. (If there's
	//			been an error, bResult will have been set to FALSE.)
	return bResult;
#else
	return FALSE;
#endif
}

//#endif
