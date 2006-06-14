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

/*
//*/

#include "camtypes.h"
//#include "app.h"
#include "nbevcont.h"

#ifdef BUILDSHADOWS

// code headers
//#include "docview.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "objchge.h"
//#include "osrndrgn.h"
#include "nodebev.h"

// resource headers
//#include "simon.h"
//#include "mario.h"
#include "attrappl.h"
#include "opgrad.h"
#include "transop.h"
#include "opbevel.h"
//#include "mrhbits.h"

// text class includes
#include "nodetxts.h"
//#include "txtattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "textops.h"
//#include "arrows.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bevinfo.h"

// Save/load
//#include "cxfrec.h"		// CXaraFileRecord - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrbev.h"
#include "bevtool.h"
#include "blndtool.h"
#include "blobs.h"
//#include "tool.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "bevres.h"
//#include "attrmgr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "contmenu.h"
//#include "becomea.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "lineattr.h"
//#include "txtattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "attrbev.h"
#include "nodecont.h"
#include "csrstack.h"
//#include "moldtool.h"
#include "attrmap.h"
#include "opcntr.h"
#include "nodeblnd.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "extender.h"
#include "ngcore.h"		// NameGallery, for stretching functionality
#include "blendatt.h"
#include "blndhelp.h"
#include "nodebldr.h"
#include "pathops.h"
#include "brshattr.h"
#include "pathndge.h"
//#include "mrhbits.h"
#include "offscrn.h"
#include "saveeps.h"
//#include "pmaskrgn.h"       
#include "slicehelper.h"       
#include "scanrr.h"
//#include "cmxrendr.h"
//#include "cmxexdc.h"
//#include "ai_epsrr.h"
#include "opnudge.h"
#include "ophist.h"
#include "cutop.h"

CC_IMPLEMENT_DYNAMIC(NodeBevelController, NodeGroup )  
CC_IMPLEMENT_DYNAMIC(BevelNodeTreeFactory, CompoundNodeTreeFactory )  

// Give this file in memory dumps
// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW

/*********************************************************************************************

>    NodeBevelController::NodeBevelController() 

     Author:    Olivier_Gascoin (Xara Group Ltd) <camelotdev@xara.com>
     Created:   20/11/96
     Inputs:    -
     Outputs:   
     Returns:   -
			  
     Purpose: This constructor creates a NodeBevelController 
			
     Errors:    -

**********************************************************************************************/
 
NodeBevelController::NodeBevelController() : NodeGroup()
{
	// Initialise member vars
	m_Contrast = 85;
	m_bDrag = FALSE;
	m_bIncludeBlobsInBoundingRect = FALSE;
	
	m_bBoundingRectIsForTool = FALSE;
	IsBoundingRectValid = FALSE;
	m_LastZoom = FIXED16(0);
	m_PerformedExtend = FALSE;
	m_LastColoursTransparency = FALSE;
	m_HaveDoneApplyTest = FALSE;
	m_PathToClipVeiw.Initialise();
	m_pBevel = NULL;
	m_pClipViewAttribute = NULL;

#ifdef _DEBUG
	myBevelID = -1;
	myBevelBecomeAID = -1;
#endif
}

NodeBevelController::NodeBevelController(Node* ContextNode,  
				AttachNodeDirection Direction,  
				BOOL Locked, 
				BOOL Mangled,  
				BOOL Marked, 
				BOOL Selected
				) : NodeGroup(ContextNode, Direction, Locked, Mangled, Marked, Selected)
{
	m_Contrast = 85;
	m_bDrag = FALSE;
	m_bIncludeBlobsInBoundingRect = FALSE;
	m_bBoundingRectIsForTool = FALSE;
	IsBoundingRectValid = FALSE;
	m_LastZoom = FIXED16(0);
	m_LastColoursTransparency = FALSE;
	m_HaveDoneApplyTest = FALSE;
	m_PathToClipVeiw.Initialise();
	m_pBevel = NULL;
	m_pClipViewAttribute = NULL;

#ifdef _DEBUG
	myBevelID = -1;
	myBevelBecomeAID = -1;
#endif
}
	
NodeBevelController::~NodeBevelController()
{	
#ifdef _DEBUG

	if (myBevelID > -1)
	{
		TCHAR			strId[100];
		camSnprintf( strId, 100, _T("Popping NodeBevelController ID:  %i\n"), myBevelID );
			
		TRACEUSER( "ChrisS", strId );
	}

#endif
}

NodeBevel * NodeBevelController::GetBevelNode()
{
	NodeBevel * pBob = (NodeBevel *)FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
	// ERROR3IF(pBob == NULL, "NodeBevelContoller has no child node");

	return pBob;
}

/********************************************************************************************

>	virtual NodeRenderableInk* NodeBevelController::GetInkNodeFromController() const

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/10/2000
	Inputs:		-
	Outputs:	-
	Returns:	The ink node that is used by bevels, shadows and contours
	Purpose:	To return the node that we bevel

	Notes:		Karim 23/10/2000
				Rewritten to be as fast as poss, so people feel like using this
				method rather than writing their own bevelled node search.
	SeeAlso:	

********************************************************************************************/
NodeRenderableInk* NodeBevelController::GetInkNodeFromController() const
{
	// We get our NodeBevel and look for the first ink node left or right from it,
	// depending on whether it is an inner or an outer bevel.
	Node* pKid = NULL;
	if (m_pBevel != NULL)
	{
		if (m_pBevel->m_bOuter)
		{
			pKid = m_pBevel->FindNext();
			while (pKid != NULL && !pKid->IsAnObject())
				pKid = pKid->FindNext();

			// so if our optimised version fails we'll go the other way
			if (pKid == NULL)
			{
				pKid = m_pBevel->FindPrevious();
				while (pKid != NULL && !pKid->IsAnObject())
					pKid = pKid->FindPrevious();
			}
		}
		else
		{
			pKid = m_pBevel->FindPrevious();
			while (pKid != NULL && !pKid->IsAnObject())
				pKid = pKid->FindPrevious();

			// so if our optimised version fails we'll go the other way
			if (pKid == NULL)
			{
				pKid = m_pBevel->FindNext();
				while (pKid != NULL && !pKid->IsAnObject())
					pKid = pKid->FindNext();
			}
		}
	}

//	ERROR3IF(pKid == NULL, "NodeBevelController::GetInkNodeFromController; Can't find bevelled node!");

	return (NodeRenderableInk*)pKid;
}



/*********************************************************************************************

>    static BOOL NodeBevelController::SelectionHasBevelNode()

     Author:    David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/10/98
     Inputs:    -
     Outputs:   Returns TRUE if the current selection has a bevel node in it
     Returns:   -
			  
     Purpose:	Runs through the current selection and returns TRUE if it has a bevel node in it
			
     Errors:    -

**********************************************************************************************/

BOOL NodeBevelController::SelectionHasBevelNode()
{
	if (GetSelectionBevelNode())
	{
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************************************

>    static BOOL NodeBevelController::SelectionHasBevelInkNode()

     Author:    David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/10/98
     Inputs:    -
     Outputs:   Returns TRUE if the current selection has a bevel node in it
     Returns:   -
			  
     Purpose:	Runs through the current selection and returns TRUE if it has a bevel node in it
			
     Errors:    -

**********************************************************************************************/

BOOL NodeBevelController::SelectionHasBevelInkNode()
{
	if (GetSelectionBevelInkNode())
	{
		return TRUE;
	}

	return FALSE;
}


/*********************************************************************************************

>    static NodeBevel * NodeBevelController::GetSelectionBevelNode(INT32 num);

     Author:    David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/10/98
     Inputs:    -
     Outputs:   Gets the n'th bevel node in the selection
     Returns:   -
			  
     Purpose:	Runs through the current selection and returns TRUE if it has a bevel node in it
			
     Errors:    -

**********************************************************************************************/

NodeBevelController * NodeBevelController::GetSelectionBevelNode(INT32 num /* = 0 */)
{
	// build the list
	List NodeList;

	if (!BevelTools::BuildListOfSelectedNodes(&NodeList,
								CC_RUNTIME_CLASS(NodeBevelController)))
	{
		return NULL;
	}

	if (NodeList.GetCount() <= (UINT32)num)
	{
		return NULL;
	}

	INT32 i = 0;
	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();

	NodeBevelController *pBevC = NULL;

	while (pItem)
	{
		if (pItem->pNode)
		{
			if (i == num)
			{
				pBevC = (NodeBevelController *)pItem->pNode;
				NodeList.DeleteAll();
				return pBevC;
			}
		}

		pItem = (NodeListItem *)NodeList.GetNext(pItem);
		i++;
	}
	return NULL;
}

/*********************************************************************************************

>    static NodeBevelController * NodeBevelController::GetSelectionBevelInkNode(INT32 num);

     Author:    David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
     Created:   12/10/98
     Inputs:    -
     Outputs:   Gets the n'th bevel ink node in the selection
     Returns:   -
			  
     Purpose:	Runs through the current selection and returns TRUE if it has a bevel node in it
			
     Errors:    -

**********************************************************************************************/

NodeBevel * NodeBevelController::GetSelectionBevelInkNode(INT32 num /* = 0 */)
{
	// build the list
	List NodeList;

	if (!BevelTools::BuildListOfSelectedNodes(&NodeList,
								CC_RUNTIME_CLASS(NodeBevel)))
	{
		return NULL;
	}

	if (NodeList.GetCount() <= (UINT32)num)
	{
		return NULL;
	}

	INT32 i = 0;
	NodeListItem * pItem = (NodeListItem *)NodeList.GetHead();
	NodeBevel * pBev = NULL;

	while (pItem)
	{
		if (pItem->pNode)
		{
			if (i == num)
			{
				pBev = (NodeBevel *)pItem->pNode;
				NodeList.DeleteAll();
				return pBev;
			}
		}

		pItem = (NodeListItem *)NodeList.GetNext(pItem);
		i++;
	} 
	return NULL;
}

/********************************************************************************************
>	BOOL NodeBevelController::WritePreChildrenNative(BaseCamelotFilter* pFilter)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	FALSE for failure
	Purpose:	record saving stuff from xar files
********************************************************************************************/

BOOL NodeBevelController::WritePreChildrenNative(BaseCamelotFilter* pFilter)
{
	UINT32 Tag = TAG_BEVEL;

	NodeBevel * pBevel = GetBevelNode();

	// do the writing
	INT32 RecordSize = 24; // bytes
	
	CamelotFileRecord Rec(pFilter,Tag,RecordSize);
	Rec.Init();

	// ok, do the writing of the bevel data 
	Rec.WriteINT32((INT32)pBevel->m_BevelType);		// 4 bytes
	Rec.WriteINT32((INT32)pBevel->m_Indent);			// 4 bytes
	Rec.WriteINT32((INT32)pBevel->m_LightAngle);			// 4 bytes
	Rec.WriteINT32((INT32)pBevel->m_bOuter);			// 4 bytes
	Rec.WriteINT32((INT32)pBevel->m_Contrast);					// 4 bytes
	Rec.WriteINT32((INT32)pBevel->m_Tilt);			// another 4 bytes

	// export to file
	pFilter->Write(&Rec);

	return TRUE;
}	

BOOL NodeBevelController::WritePostChildrenNative(BaseCamelotFilter* pFilter)
{
	// move the attribute node back to where it should be
	return TRUE;
}


BOOL NodeBevelController::WritePreChildrenWeb(BaseCamelotFilter * pFilter)
{
	return WritePreChildrenNative(pFilter);
}

BOOL NodeBevelController::WritePostChildrenWeb(BaseCamelotFilter * pFilter)
{
	return WritePostChildrenNative(pFilter);
}

/********************************************************************************************
>	BOOL NodeBevelController::LoadBevelFromRecord(CXaraFileRecord* pCXaraFileRecord)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	FALSE for failure
	Purpose:	record loading stuff from xar files
********************************************************************************************/

BOOL NodeBevelController::LoadBevelFromRecord(CXaraFileRecord* pCXaraFileRecord)
{
	INT32 BevelType		= 0;
	INT32 Indent			= 0;
	INT32 LightAngle		= 0;
	INT32 bOuter			= 0;
	INT32 Contrast		= 0;
	INT32 Tilt			= 45;
//	double dpi			= 96.0;


	if (!pCXaraFileRecord->ReadINT32(&BevelType))
		return FALSE;

	if (!pCXaraFileRecord->ReadINT32(&Indent))
		return FALSE;

	if (!pCXaraFileRecord->ReadINT32(&LightAngle))
		return FALSE;
	
	if (!pCXaraFileRecord->ReadINT32(&bOuter))
		return FALSE;

	if (!pCXaraFileRecord->ReadINT32(&Contrast))
		return FALSE;

	// check for tilt being defined
	if (pCXaraFileRecord->GetSize() == 24)
		if (!pCXaraFileRecord->ReadINT32(&Tilt))
			return FALSE;

	m_BevelType		= BevelType;
	m_Indent		= Indent;
	m_LightAngle	= LightAngle;
	m_bOuter		= bOuter;
	m_LType			= LineCapButt;
	m_Contrast		= Contrast;
	m_Tilt			= Tilt;

	// that'll be it !
	
	return TRUE;
}

/********************************************************************************************
>	void NodeBevelController::ReRenderBevelBitmap(double LightAngle)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	FALSE for failure
	Purpose:	Renders the bevel bitmap - JUST the bitmap, nothing else !
********************************************************************************************/
void NodeBevelController::ReRenderBevelBitmap(double LightAngle)
{
	// find my bevel node
	NodeBevel * pBevel = GetBevelNode();

	double OldLightAngle = pBevel->m_LightAngle;

	if (pBevel)
	{
		pBevel->m_LightAngle = LightAngle;
		pBevel->ReRenderBevelBitmap(FALSE);
		pBevel->m_LightAngle = OldLightAngle;
	}
}

/********************************************************************************************
>	BOOL NodeBevelController::PostImport()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	FALSE for failure
	Purpose:	set up my nodes after importing
********************************************************************************************/

BOOL NodeBevelController::PostImport()
{
	// get the bevel node under me to set up its values
	NodeBevel* pBevel = GetBevelNode();
	ERROR3IF(!pBevel, "NodeBevelController - No bevel ink node\n");

//	BOOL ok = TRUE;

	// set up the bevel node
	pBevel->m_Indent		= m_Indent;
	pBevel->m_BevelType		= m_BevelType;
	pBevel->m_LightAngle	= m_LightAngle;
	pBevel->m_bOuter		= m_bOuter;
	pBevel->m_BMPCentre		= m_BMPCentre;
	pBevel->m_BMPPoint1		= m_BMPPoint1;
	pBevel->m_BMPPoint2		= m_BMPPoint2;
	pBevel->m_Contrast		= m_Contrast;
	pBevel->m_Tilt			= m_Tilt;

	// create the bevel start node in its correct place
	Node * pFirstChild = FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));
	ERROR2IF(pFirstChild == NULL, FALSE, "No renderable children under bevel controller node");
	NodeBevelBegin * pBevelBegin = new NodeBevelBegin(pFirstChild, PREV);
	ERROR2IF(pBevelBegin == NULL, FALSE, "Can't create bevel begin node");

	// don't ask me why ! It just works this way
	Node * pBaseNode = SliceHelper::FindNextOfClass(this,this,CC_RUNTIME_CLASS(BaseTextClass));
	Node * pNextAttrNode = NULL;
	Node * pCurrentAttrNode = NULL;
//	Node * pTextAttrNode = NULL;
	BOOL IsLastToFind = FALSE;

	// What this next bit does is to copy the Text Attributes on this node to all the TextStories and then
	// Unlincks them from this! This is basically the Do Localize Attributes Function!!!
	while(pBaseNode)
	{
		IsLastToFind = (SliceHelper::FindNextOfClass(pBaseNode,this,CC_RUNTIME_CLASS(BaseTextClass)) == NULL);
		
		pCurrentAttrNode = FindFirstChild(CC_RUNTIME_CLASS(AttrTxtBase));
		
		while(pCurrentAttrNode)
		{
			pNextAttrNode = pCurrentAttrNode->FindNext(CC_RUNTIME_CLASS(AttrTxtBase));
			pCurrentAttrNode->CopyNode(pBaseNode, FIRSTCHILD);
			
			if(IsLastToFind)
			{
				pCurrentAttrNode->UnlinkNodeFromTree();
				delete pCurrentAttrNode;
				pCurrentAttrNode = NULL;
			}

			pCurrentAttrNode = pNextAttrNode;
		}

		pBaseNode = SliceHelper::FindNextOfClass(pBaseNode,this,CC_RUNTIME_CLASS(BaseTextClass));
	}

	// set up my attributes
	AttrBevelIndent * pIndent = new AttrBevelIndent(this, FIRSTCHILD);
	ERRORIF(pIndent == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
	if(m_bOuter)
		pIndent->Value.m_Indent = -m_Indent;
	else
		pIndent->Value.m_Indent = m_Indent;

	AttrBevelLightAngle * pLightAngle = new AttrBevelLightAngle(this, FIRSTCHILD);
	ERRORIF(pLightAngle == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
	pLightAngle->Value.m_LightAngle = (INT32)m_LightAngle;

	AttrBevelContrast * pContrast = new AttrBevelContrast(this, FIRSTCHILD);
	ERRORIF(pContrast == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
	pContrast->Value.m_Contrast = m_Contrast;

	AttrBevelType * pType = new AttrBevelType(this, FIRSTCHILD);
	ERRORIF(pType == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
	pType->Value.m_Type = m_BevelType;

	AttrBevelLightTilt * pTilt = new AttrBevelLightTilt(this, FIRSTCHILD);
	ERRORIF(pTilt == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
	pTilt->Value.m_LightTilt = static_cast<INT32>(m_Tilt);

	// We need to move the bevel node to the correct position in the tree if it`s an outer
	// which should be directly after the BevelBegin Node!
	if(m_bOuter)
		pBevel->MoveNode(pBevelBegin,NEXT);
	else
		pBevel->MoveNode(this,LASTCHILD);

	// finally, normalise all of the attributes
	OptimiseAttributes();

	// should be it! regen bevel
	CreateBevel();

	return TRUE;
}

/********************************************************************************************
>	String NodeBevelController::Describe(BOOL Plural, BOOL Verbose)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Returns my description string
********************************************************************************************/

String NodeBevelController::Describe(BOOL Plural, BOOL Verbose)
{
	String Name;
	Name.Load(_R(IDS_BEVEL_NODE_NAME));

	if (Plural)
	{
		Name += _T("s");
	}

	return Name;
}

/********************************************************************************************
>	void NodeBevelController::Transform( TransformBase& Trans )

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	
	Purpose:	Transforms me
********************************************************************************************/
void NodeBevelController::Transform( TransformBase& Trans )
{
	NodeBevel * pBob = (NodeBevel *)FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
	
	if(!pBob)
	{
		ERROR3("Failed to find the bevel node!");
		return;
	}

	// See GroupCanTransformCached()
	pBob->m_MustRegenOnChildChange = FALSE;
	NodeGroup::Transform(Trans);
	pBob->m_MustRegenOnChildChange = TRUE;

	pBob->Transform(Trans);

	InvalidateBoundingRect();
	GetBoundingRect(TRUE, FALSE);
}

/********************************************************************************************
>	DocRect NodeBevelController::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/11/00
	Inputs:		DontUseAttrs - TRUE if the attrs associated with the node should be
				ignored. defaults to FALSE
				HitTest      - TRUE if being called during HitTest
	Returns:	The bevels bounding rect
	Purpose:	If the bevels bounding rect is known then it is returned. If not, it is
				re-calculated.
********************************************************************************************/
DocRect NodeBevelController::GetBoundingRect(BOOL DontUseAttrs, BOOL HitTest)
{
	if (!IsBoundingRectValid || DontUseAttrs)
	{
		DocRect NewRect(0,0,0,0);

		// If we`ve got a bevel Node get it`s bounding Rect
		if (GetBevelNode())
			NewRect = GetBevelNode()->GetBoundingRect(DontUseAttrs, HitTest);
		else
		{
			NewRect = GetInsideBoundingRect();
			
			if (!NewRect.IsEmpty() && m_bOuter)
			{
				AttrBevelIndent * pIndent = NULL;
				if (FindAppliedAttribute(CC_RUNTIME_CLASS(AttrBevelIndent), (NodeAttribute **)&pIndent) && pIndent)
					NewRect.Inflate(pIndent->Value.m_Indent);
			}
		}

		if (DontUseAttrs)
			return NewRect; // just return this rectangle as it is not the nodes true bounding rect

		// copy the new docrect into the node bounding rect
		BoundingRectangle = NewRect;

		// Mark the bounding rect as valid
		IsBoundingRectValid = TRUE;
	}

	// Return the resulting bounding rect
	return BoundingRectangle;
}

/********************************************************************************************
>	DocRect NodeBevelController::GetBlobBoundingRect()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	-
	Purpose:	Gets the blob bounding rect
********************************************************************************************/
DocRect NodeBevelController::GetBlobBoundingRect()
{
	// now, find out if the blob manager's interest is in artistic blobs or not
	BlobManager* BlobMgr = GetApplication()->GetBlobManager();
	BlobStyle BS = BlobMgr->GetCurrentInterest(TRUE);

	DocRect br ;
	
	br = BoundingRectangle;
	
	// is the bevel tool active ?
	if (Tool::GetCurrentID() != TOOLID_BEVELTOOL ||
		!BevelTool::AmActive())
	{
		return NodeGroup::GetBlobBoundingRect();
	}

	DocRect BlobRect;

	// calculate the position of the light angle blob
	DocCoord blob1((br.hi.x + br.lo.x) / 2, 
		(br.hi.y + br.lo.y) / 2);
	DocCoord blob2;

	View * pView = DocView::GetCurrent();

	if (!pView)
	{
		return br;
	}

	double len = pView->GetScaledPixelWidth().MakeDouble() * 40;
	
	INT32 iLen = (INT32)(len);

	// get from the blob manager the size of blobs
	if (pView)
	{
		iLen += (INT32)(pView->GetScaledPixelWidth().MakeDouble() * 15);
		iLen += GetApplication()->GetBlobManager()->GetBlobSize();
	}

	DocRect dr;

	// calculate the rect for the blob
	dr.hi.x = blob1.x + iLen;
	dr.hi.y = blob1.y + iLen;

	dr.lo.x = blob1.x - iLen;
	dr.lo.y = blob1.y - iLen;

	BlobRect = br;

	// finally, union this with the standard group's bounding rect to give
	// the true blob bounding rect
	BlobRect = BlobRect.Union(dr);

	return BlobRect;
}



/********************************************************************************************

>	virtual void NodeBevelController::SelectInRect(const DocRect& Rect, SelStateAction st)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 April 2000
	Inputs:		Rect	const reference to the bounding rect to use.
				st		the selection state, ie SET/TOGGLE/CLEAR.
	Outputs:	This node may have its selection status changed, depending on the inputs.

	Purpose:	Helper method for the static fn SelectAllInRect() used by marquee select.
				This method changes the selection state of this node according to the given
				bounding rectangle and sel-state action. Most nodes will want to use the
				default behaviour, which is to select themselves if their bounds lie within
				the given bounding rectangle. If you want to do something special with the
				marquee select, then override this method.

	Notes:		This method is meant to be called solely from SelectAllInRect() - if you wish
				to call it from elsewhere, it *may* work as you expect, but it is a good idea
				that you check your assumptions on what preconditions are necessary.

	Errors:		ERROR3 in DEBUG if st holds an invalid state.
	See also:	SelectAllInRect().

********************************************************************************************/
void NodeBevelController::SelectInRect(const DocRect& Rect, SelStateAction st)
{
	// try to select each of our NodeRenderableInk children.
	NodeRenderableInk* pInkChild = NULL;
	Node* pChild = FindFirstChild();
	while (pChild != NULL)
	{
		if (pChild->IsAnObject())
		{
			pInkChild = (NodeRenderableInk*)pChild;
			if (Rect.ContainsRect(pInkChild->GetBoundingRect()))
			{
				switch (st)
				{
				case CLEAR:
					if (pInkChild->MarqueeSelectNode())
					{
						pInkChild->DeSelect(TRUE);
					}
					break;

				case SET:
					if (pInkChild->MarqueeSelectNode())
					{
						pInkChild->Select(TRUE);
					}
					break;

				case TOGGLE:
					if (pInkChild->MarqueeSelectNode())
					{
						if (pInkChild->IsSelected())
							pInkChild->DeSelect(TRUE);
						else
							pInkChild->Select(TRUE);
					}
					break;

				default:
					ERROR3("NodeBevelController::SelectInRect; Unknown SelStateAction!\n");
					return;
				}
			}
		}
		pChild = pChild->FindNext();
	}
}



/********************************************************************************************
>	void NodeBevelController::DeleteCache()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/10/98
	Inputs:		-
	Returns:	-
	Purpose:	Deletes my memory allocation
********************************************************************************************/
void NodeBevelController::DeleteCache()
{
	// tells the bevel node to delete its cache
	if (GetBevelNode())
	{
		GetBevelNode()->DeleteCache();
		GetBevelNode()->InvalidateMe();
	}
}

/********************************************************************************************
>	BOOL NodeBevelController::PromoteAttributeApplicationToMe(CCRuntimeClass *pAttrClass)

 	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/09/00
	Inputs:		Attribute to test
	Returns:	true for yes and false for no!
	Purpose:	Deals attribute promotions.
********************************************************************************************/
BOOL NodeBevelController::PromoteAttributeApplicationToMe(CCRuntimeClass *pAttrClass) const
{
	NodeBevel * pBevel = (NodeBevel *)FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));

	// If the Attribute is a bevel attribute then we must always place it on the controller node!
	if(pAttrClass == CC_RUNTIME_CLASS(AttrBevel) ||
	   pAttrClass == CC_RUNTIME_CLASS(AttrStartArrow) ||
	   pAttrClass == CC_RUNTIME_CLASS(AttrEndArrow) ||
	   pAttrClass == CC_RUNTIME_CLASS(AttrStartCap) ||
	   pAttrClass == CC_RUNTIME_CLASS(AttrDashPattern) ||
	   pBevel->IsSelected() )
	{
		return TRUE;
	}

	BOOL IsTransparent = (pAttrClass->IsKindOf(CC_RUNTIME_CLASS(AttrTranspFillGeometry)) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrTranspTypeChange) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrTranspChange) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrBitmapTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrCircularTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrConicalTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrFlatTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrFourColTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrThreeColTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrFractalTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrLinearTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrNoiseTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrRadialTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrSquareTranspFill) ||
						  pAttrClass == CC_RUNTIME_CLASS(AttrTextureTranspFill) ||
					      pAttrClass == CC_RUNTIME_CLASS(AttrTranspFillRampChange));

	if(!(pAttrClass == CC_RUNTIME_CLASS(AttrColourChange) ||
		pAttrClass == CC_RUNTIME_CLASS(AttrColourDrop) ||
		pAttrClass == CC_RUNTIME_CLASS(AttrBitmapChange) ||
		pAttrClass == CC_RUNTIME_CLASS(AttrBitmapTessChange) ||
		pAttrClass == CC_RUNTIME_CLASS(AttrColFillRampChange) ||
		pAttrClass->IsKindOf(CC_RUNTIME_CLASS(AttrFillGeometry)) ||
		IsTransparent))
	{
		return FALSE;
	}

	// Ok! Check to see if this node has the applied attribute. If it does then it means that
	// the children all have the same attribute (including the bevel).
	// first, get all the children
	BOOL DoApplyToMe = FALSE;
	NodeAttribute* pBevAttr = NULL;
	NodeAttribute* pObjAttr = NULL;
	List ChildList;
	NodeListItem * pItem = NULL;
	NodeRenderableInk* pCurrentNode = NULL;

	BevelTools::GetAllNodesUnderNode(this, &ChildList, CC_RUNTIME_CLASS(NodeRenderableInk));
	pItem = (NodeListItem *)ChildList.GetHead();

	INT32 Check = ChildList.GetCount();

	if(IsTransparent)
	{
		if(!pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry),&pBevAttr))
			return FALSE;
	}
	else
	{
		if(!pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pBevAttr))
			return FALSE;
	}

	while (pItem)
	{
		pCurrentNode = (NodeRenderableInk*)pItem->pNode;

		// is it selected implying that it is being transformed
		if (pCurrentNode && pCurrentNode->IsAnObject() && !pCurrentNode->IsABevel())
		{
			if(IsTransparent)
			{
				if(pCurrentNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrTranspFillGeometry),&pObjAttr))
				{
					if(pObjAttr->IsDifferent(pBevAttr))
						Check--;
				}
			}
			else
			{
				if(pCurrentNode->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrFillGeometry),&pObjAttr))
				{
					if(pObjAttr->IsDifferent(pBevAttr))
						Check--;
				}
			}
		}

		pItem = (NodeListItem *)ChildList.GetNext(pItem);
	}
	
	DoApplyToMe = ((ChildList.GetCount() - Check) <= 1);

	ChildList.DeleteAll();
	return DoApplyToMe;
}

/********************************************************************************************
>	ChangeCode 	NodeBevelController::OnChildChange(ObjChangeParam* pParam)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/98
	Inputs:		-
	Returns:	-
	Purpose:	Deals with child changes
********************************************************************************************/
ChangeCode 	NodeBevelController::OnChildChange(ObjChangeParam* pParam)
{
	// get the op out of the parameter
	if (!pParam)
		return NodeGroup::OnChildChange(pParam);

	if (!pParam->GetOpPointer())
	{
		Document * pDoc = Document::GetCurrent();

		if (pDoc)
		{
			if (pParam->GetChangeFlags ().RegenerateNode)
				RegenerateNode(NULL, FALSE, FALSE);		// CGS:  it is now legitimate for us to
														// do this
		
//			Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));
		}

		return CC_OK;		// NodeController::OnChildChange() now regenerates the node when
							// a non-undoable op is passed in (and we don't want this to happen
							// with bevels!).  (Actually NodeCompound::OnChildChange())
	}

	UndoableOperation * pUndoOp = NULL;

	if (pParam->GetOpPointer()->IsKindOf(CC_RUNTIME_CLASS(UndoableOperation)))
		pUndoOp = pParam->GetOpPointer();

	if (!pUndoOp)
	{
		return CC_OK;		// NodeController::OnChildChange() now regenerates the node when
							// a non-undoable op is passed in (and we don't want this to happen
							// with bevels!).  (Actually NodeCompound::OnChildChange())
	}

	if (pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpCreateBevel)))
		return CC_OK;

	// first, invalidate my region
//	if (pParam->GetChangeType() == OBJCHANGE_FINISHED)
//		pUndoOp->DoInvalidateNodeRegion(this, TRUE);

	ChangeCode cd = CC_OK;

	BOOL bCallBase = TRUE;

	Node * pParent = NULL;
	List ChildList;
	BOOL bRegen = FALSE;
	NodeListItem * pItem = NULL;
	NodeAttribute * pAttr = NULL;

	if (m_PerformedExtend)
	{
		// this means that we have done an extend
		// which added the regen action into the action list
		// so no need to regen the node
		// just reset the flag and leave (sjk 4/8/2000)
		m_PerformedExtend = FALSE;
	}
	else
	// in bevels, no need to regenerate for tranparency changes, or for fill changes
	if (pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpReplaceAttributes)))
	{
		pAttr = ((OpReplaceAttributes *)pUndoOp)->GetAttribute();
		
		if (pAttr)
		{
			if (pAttr->IsAFillAttr() || pAttr->IsATranspFill())
				bCallBase = FALSE;
		}
	}	
	else if (pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpEditFill)))
	{
		bCallBase = FALSE;
	}
	else if (pUndoOp->IS_KIND_OF (OpPathNudge))
	{
		RegenerateNode(pUndoOp, FALSE, FALSE);
	}

	// Karim 20/12/2000		added to fix an update bug - shadow & bevel some text,
	//						enter some more & shadow doesn't get correct bounds from bevel.
	else if (pUndoOp->IS_KIND_OF (OpTextUndoable))
	{
		RegenerateNode(pUndoOp, FALSE, FALSE);
	}

	else if (pUndoOp->IsKindOf(CC_RUNTIME_CLASS(TransOperation)) &&
		!pUndoOp->IS_KIND_OF(OpMovePathPoint))
	{
		// a trans operation has occured - do nothing & leave it up to
		// the transform function
		// first, invalidate my region
//		pUndoOp->DoInvalidateNodeRegion(this, TRUE);

		// find out if a select-inside drag has occurred (e.g. when a group is shadowed
		// and an object inside the group is selected and dragged then a regeneration 
		// should occur. This isn't taken care of by the transform method as this method
		// isn't called by the children.

		// first, get all the children
		BevelTools::GetAllNodesUnderNode(this, &ChildList, CC_RUNTIME_CLASS(NodeRenderableInk));

		pItem = (NodeListItem *)ChildList.GetHead();

		bRegen = FALSE;
		BOOL MustRegenerate = FALSE;

		while (pItem)
		{
			// is it selected implying that it is being transformed
//			if (pItem->pNode->IsAnObject())
//			{
//				((NodeRenderableInk *)pItem->pNode)->InvalidateBoundingRect();
//				pUndoOp->DoInvalidateNodeRegion((NodeRenderableBounded *)pItem->pNode, TRUE);
//			}
//
			if (pItem->pNode->IsSelected()) // Are you SURE you can use selected here? - left only because it is a shortcut out
			{
				// get & test its parent
				pParent = pItem->pNode->FindParent();
				
				while (pParent)
				{
					// does the parent transform with children ?
					if (pParent->ShouldITransformWithChildren())
					{
						// is the parent this object ?
						if (pParent == this)
						{
							bRegen = FALSE; // don't regenerate
							pParent = NULL; // break out of the loop
						}
						else
						{
							// go on to the next parent 
							pParent = pParent->FindParent();
						}
					}
					else
					{
						bRegen = TRUE; // yep, regenerate ! 
						pParent = NULL; // break the loop
					}
				}
			}

			pItem = (NodeListItem *)ChildList.GetNext(pItem);
		}

		ChildList.DeleteAll();

		if (bRegen || MustRegenerate)
		{
			pUndoOp->DoInvalidateNodeRegion(this, TRUE);
			RegenerateNode(pUndoOp, FALSE, FALSE);
		}

		return CC_OK;
	}
	else if (pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpApplyAttrib)))
	{
		CCRuntimeClass * pClass = ((OpApplyAttrib *)pUndoOp)->GetValueChangeType();

		if (pClass)
		{
			if(pClass == CC_RUNTIME_CLASS(AttrBevel))
			{
				pUndoOp->DoInvalidateNodeRegion(this, TRUE);
				RegenerateNode(pUndoOp, FALSE, FALSE);
				return CC_OK;
			}
			else if(pClass == CC_RUNTIME_CLASS(AttrStrokeColourChange))
			{
				// If we are changing the outline colour then we need to check to see if it`s
				// been made transparent. If so then we need to regenerate the bevel!
				bCallBase = FALSE;
				OpApplyAttrib* pOpApplyNode = (OpApplyAttrib*)pParam->GetOpPointer();

				if(pOpApplyNode)
				{
					// We need to compare the current applied stroke colour with the one that`s being applied.
					pAttr = (AttrStrokeColourChange*)pOpApplyNode->GetAttributeToApply();
					
					if(pAttr)
					{
						DocColour* pStrokeColour = ((AttrStrokeColour*)pAttr)->GetStartColour();

						if(pStrokeColour)
						{
							bCallBase = (pStrokeColour->IsTransparent() != m_LastColoursTransparency);
							m_LastColoursTransparency = pStrokeColour->IsTransparent();
						}
					}
				}
			}
			else if(pClass == CC_RUNTIME_CLASS(AttrLineWidth))
			{
				// Always update the bevel paths when we change the outline of the beveled objects
				bCallBase = TRUE;
				m_LastColoursTransparency = FALSE;
			}
		}
	}			
	
	if (bCallBase)
		cd = NodeGroup::OnChildChange(pParam);

	// If we delete an object then the bevel is nolonger true, therefore regenerate to get back the true form!
	if (pParam->GetChangeType() == OBJCHANGE_FINISHED && (
			   pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpDelete))
			|| pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpPaste))
			|| pUndoOp->IsKindOf(CC_RUNTIME_CLASS(OpCut))
			))
	{
		pUndoOp->DoInvalidateNodeRegion(this, TRUE);
		RegenerateNode(pUndoOp, FALSE, FALSE);
	}
	
	// return the change code
	return cd;
}


/********************************************************************************************
>	Node* NodeBevelController::SimpleCopy()

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/98
	Inputs:		-
	Returns:	-
	Purpose:	Makes a copy of the node
********************************************************************************************/
Node* NodeBevelController::SimpleCopy()
{
	NodeBevelController *pNewNode = new NodeBevelController();
	ERROR3IF(!pNewNode, "Error - Couldn't create new bevel controller node");

	CopyNodeContents(pNewNode);  
	
	return pNewNode;
}


/********************************************************************************************
>	void NodeBevelController::CopyNodeContents(NodeBevelController* pNewNode)

 	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/01/2004
	Inputs:		-
	Returns:	
	Purpose:	Sort of backwards copy constructor

********************************************************************************************/
void NodeBevelController::CopyNodeContents(NodeBevelController* pNewNode)
{
	pNewNode->m_LType = m_LType;

	pNewNode->m_Tilt = m_Tilt;
	
	// boolean which tells GetBoundingRect to include the blobs
	pNewNode->m_bIncludeBlobsInBoundingRect = m_bIncludeBlobsInBoundingRect;

	// points determining the fill control points
	pNewNode->m_BMPCentre = m_BMPCentre;
	pNewNode->m_BMPPoint1 = m_BMPPoint1;
	pNewNode->m_BMPPoint2 = m_BMPPoint2;

	// the rect of the (original) selection
	pNewNode->m_SelectedRect = m_SelectedRect;

	pNewNode->m_BlobDocRect = m_BlobDocRect;
	pNewNode->m_BlobCentre = m_BlobCentre;
	
	// indicates that a drag is in progress
	pNewNode->m_bDrag = m_bDrag;

	pNewNode->m_HaveDoneApplyTest = m_HaveDoneApplyTest;

	// indicates that the last bounding rect passed back was for the tool
	// used to see if we need to re-generate the bounding rect 
	pNewNode->m_bBoundingRectIsForTool = m_bBoundingRectIsForTool;

	pNewNode->m_LastZoom = m_LastZoom;

	pNewNode->m_PerformedExtend = m_PerformedExtend;

	pNewNode->m_LastColoursTransparency = m_LastColoursTransparency;

	// New ClipView Attribute for rendering Inner bevels
//	if (pNewNode->m_pClipViewAttribute)
//		delete pNewNode->m_pClipViewAttribute;
//	pNewNode->m_pClipViewAttribute = NULL;
//	if (m_pClipViewAttribute)
//	{
//		pNewNode->m_pClipViewAttribute = (AttrClipView*)m_pClipViewAttribute->SimpleCopy();
//	}
	
	// Remember to copy or blank cached path data without leaking memory...
	pNewNode->m_PathToClipVeiw.Initialise(m_PathToClipVeiw.GetNumCoords());
	pNewNode->m_PathToClipVeiw.CopyPathDataFrom(&m_PathToClipVeiw);

	pNewNode->m_pBevel = NULL;

	pNewNode->m_LastClipZoom = m_LastClipZoom;

	pNewNode->m_BevelType	=	m_BevelType;
	pNewNode->m_Indent		=   m_Indent;
	pNewNode->m_LightAngle	=   m_LightAngle;
	pNewNode->m_bOuter		=   m_bOuter;						// NodeController?
	pNewNode->m_Contrast	=   m_Contrast;
	pNewNode->m_bBlendStartNode = m_bBlendStartNode;			// NodeController?
	pNewNode->m_bBlendEndNode   = m_bBlendEndNode;				// NodeController?
	pNewNode->m_pBlendCreatedByNode = m_pBlendCreatedByNode;	// NodeController?

	NodeGroup::CopyNodeContents(pNewNode);
}




/***********************************************************************************************
>   void NodeBevelController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)

    Author:     Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
    Created:    18/12/2003
    Outputs:    -
    Purpose:    Polymorphically copies the contents of this node to another
	Errors:     An assertion failure will occur if NodeCopy is NULL
    Scope:      protected
								     
***********************************************************************************************/

void NodeBevelController::PolyCopyNodeContents(NodeRenderable* pNodeCopy)
{
	ENSURE(pNodeCopy, "Trying to copy a node's contents into a NULL node");
	ENSURE(IS_A(pNodeCopy, NodeBevelController), "PolyCopyNodeContents given wrong dest node type");

	if (IS_A(pNodeCopy, NodeBevelController))
		CopyNodeContents((NodeBevelController*)pNodeCopy);
}



/********************************************************************************************
>	BOOL NodeBevelController::PostDuplicate(UndoableOperation* pOp)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/6/99
	Inputs:		-
	Returns:	-
	Purpose:	Regens the node after a duplication
********************************************************************************************/
BOOL NodeBevelController::PostDuplicate(UndoableOperation* pOp)
{
	CreateBevel();
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NodeBevelController::RequiresAttrib(CCRuntimeClass* pAttribClass,
											BOOL Search = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/2000
	Inputs:		pAttribClass	the attr type to test.
				Search			not a clue - look at base class implementation for info.

	Returns:	FALSE if pAttribClass is a brush attr,
				calls base implementation otherwise.

	Purpose:	NodeBevelController does not require brush attrs.

	Errors:		returns FALSE if pAttribClass is NULL.

********************************************************************************************/
BOOL NodeBevelController::RequiresAttrib(CCRuntimeClass* pAttribClass, BOOL Search)
{
	if (pAttribClass == NULL || pAttribClass == CC_RUNTIME_CLASS(AttrBrushType))
		return FALSE;
	else
		return NodeRenderableInk::RequiresAttrib(pAttribClass, Search);
}



/********************************************************************************************

>	virtual BOOL NodeBevelController::RequiresAttrib(NodeAttribute* pAttr,
											BOOL Search = FALSE)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/08/2000
	Inputs:		pAttr		the attr to test.
				Search		not a clue - look at base class implementation for info.

	Returns:	FALSE if pAttr is a brush attr,
				calls base implementation otherwise.

	Purpose:	NodeBevelController does not require brush attrs.

	Errors:		returns FALSE if pAttr is NULL.

********************************************************************************************/
BOOL NodeBevelController::RequiresAttrib(NodeAttribute* pAttr, BOOL Search)
{
	if (pAttr == NULL || pAttr->IS_KIND_OF(AttrBrushType))
		return FALSE;
	else
		return NodeRenderableInk::RequiresAttrib(pAttr, Search);
}



/********************************************************************************************

>	virtual BOOL NodeBevelController::CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass)

	Author:		Diccon_Yamanaka (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/2000
	Inputs:		pAttribClass	the attr type to test.
	Returns:	FALSE if pAttribClass is a brush attr,
				TRUE otherwise.

	Purpose:	Determine whether the given attr type can be applied directly to me.
				We don't like brushed bevels, so we don't accept brush attrs.

	Errors:		returns FALSE if pAttribClass is NULL.

********************************************************************************************/
BOOL NodeBevelController::CanAttrBeAppliedToMe(CCRuntimeClass* pAttribClass)
{
	if (pAttribClass == NULL || pAttribClass == CC_RUNTIME_CLASS(AttrBrushType))
		return FALSE;
	else
		return TRUE;
}


/***********************************************************************************************

>	INT32 NodeBevelController::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		See base class
	Purpose:	Hides this node

***********************************************************************************************/
INT32 NodeBevelController::ComplexHide(UndoableOperation* pOp, Node* pNextInRange)
{
	if(m_pClipViewAttribute)
	{
		m_pClipViewAttribute->UnlinkNodeFromTree();
		delete m_pClipViewAttribute;
		m_pClipViewAttribute = NULL;
	}

	NodeHidden * pHidden = NULL;
	BOOL ok = pOp->DoHideNode(this, TRUE, &pHidden, TRUE);

	if (ok)
		return 1;

	return -1;
}

BOOL NodeBevelController::HidingNode()
{
	if(m_pClipViewAttribute)
	{
		m_pClipViewAttribute->UnlinkNodeFromTree();
		delete m_pClipViewAttribute;
		m_pClipViewAttribute = NULL;
	}
	return TRUE;
}

BOOL NodeBevelController::ShowingNode()
{
	return TRUE;
}

/********************************************************************************************
>	void NodeBevelController::RenderMyBlobs(RenderRegion* pRender)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/98
	Inputs:		-
	Returns:	-
	Purpose:	Renders the bevel blobs (bevel controller node cannot have tiny blobs !!)
********************************************************************************************/

void NodeBevelController::RenderMyBlobs(RenderRegion* pRegion)
{
}

/********************************************************************************************
>	BOOL NodeBevelController::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, 
						ContextMenu* pMenu)

 	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/12/98
	Inputs:		-
	Returns:	-
	Purpose:	Does the pop-up menu
********************************************************************************************/
BOOL NodeBevelController::OnNodePopUp(Spread* pSpread, DocCoord PointerPos, ContextMenu* pMenu)
{
	return TRUE;
	// return pMenu->BuildCommand(OPTOKEN_SELECTBEVEL, TRUE);
}

/********************************************************************************************

>	BOOL NodeBevelController::DoBecomeA(BecomeA* pBecomeA);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		BecomeA struct
	Purpose:	Converts me to a path
	SeeAlso:	

********************************************************************************************/

BOOL NodeBevelController::DoBecomeA(BecomeA* pBecomeA)
{
	// Check for a NULL entry param							   
	ERROR2IF_PF(pBecomeA == NULL,FALSE,("pBecomeA is NULL"));

	if (!pBecomeA->BAPath())
	{
		return FALSE;
	}

	BOOL ValidReason = (pBecomeA->GetReason() == BECOMEA_REPLACE || pBecomeA->GetReason() == BECOMEA_PASSBACK);
	ERROR2IF_PF(!ValidReason,FALSE,("Unkown BecomeA reason %d",pBecomeA->GetReason()));

	UndoableOperation* pOp = pBecomeA->GetUndoOp();

//	NodeGroup			* pGroup		= NULL;
	Node				* pNode			= NULL;
//	Node				* pNextNode		= NULL;
//	NodeHidden			* pHidden		= NULL;
//	AttrStrokeColour	* pStrokeColour	= NULL;

	DocColour NoColour(COLOUR_NONE);

//	UINT32 end = 255;

//	UINT32 NumObjs = 0;

	// If there`s a clipview, remove it!
	if(m_pClipViewAttribute)
	{
		m_pClipViewAttribute->UnlinkNodeFromTree();
		delete m_pClipViewAttribute;
		m_pClipViewAttribute = NULL;
	}

	switch (pBecomeA->GetReason())
	{
	case BECOMEA_REPLACE:
	{
		// if any of our children are selected, then we'll have to remember to select
		// the group created in our place.
		BOOL fSelectBecomeAGroup = IsParentOfSelected();

		// call NodeGroup's implementation first
		if (!NodeGroup::DoBecomeA(pBecomeA))
			return FALSE;

		// now turn into a group.
		NodeGroup* pGroup = BecomeAGroup(pOp);

		// select the group but don't bother redrawing blobs an' suchlike.
		if (fSelectBecomeAGroup && pGroup != NULL)
			pGroup->Select(FALSE);
	}
	break;

	case BECOMEA_PASSBACK :
		if (pBecomeA->IsBlendBecomeA())
		{
			CompoundNodeBlendBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
				NULL, FALSE, this, pBecomeA);
			
			// now that the become a is set up call all my sub nodes with this
			Node * pChild = FindFirstChild();
			
			while (pChild)
			{
				if (pChild->IsAnObject() && !pChild->NeedsParent(NULL))
				{
					MyBecomeA.ResetCount();
					
					if (pChild->CanBecomeA(&MyBecomeA))
					{
						// tell the "become a" that we're starting a block
						MyBecomeA.SetNumPathNodes(MyBecomeA.GetCount());
						MyBecomeA.ResetCount();
						
						pChild->DoBecomeA(&MyBecomeA);
					}
				}
				
				pChild = pChild->FindNext();
			}
		}
		else if (pBecomeA->IsCompoundBlendBecomeA ())			// test code ....
		{
			// CGS:  another compound node (of which we are a child) is requesting us to
			// BECOMEA_PASSBACK so get on and do it ....
			
			// this code is only called when blending bevels that have been shadowed
			// and when converting to editable shapes (in the forementioned case)

			CompoundNodeBlendBecomeA MyBecomeA(BECOMEA_PASSBACK, CC_RUNTIME_CLASS(NodePath),
				NULL, FALSE, this, pBecomeA);
			
			Node * pChild = FindFirstChild();
			
			while (pChild)
			{
				if (pChild->IsAnObject() && !pChild->NeedsParent(NULL))
				{
					MyBecomeA.ResetCount();
					
					if (pChild->CanBecomeA(&MyBecomeA))
					{
						// tell the become a that we're starting a block
						MyBecomeA.SetNumPathNodes(MyBecomeA.GetCount());
						MyBecomeA.ResetCount();
						
						pChild->DoBecomeA(&MyBecomeA);
					}
				}
				
				pChild = pChild->FindNext();
			}
		}
		else
		{
			// Sequentially ask the children of the blend to DoBecomeA
			// This is all that's required because the child objects are only passing back
			// the new node type, and NOT replacing themselves in the tree
			// This also ensures the receipient gets the list of paths in render order
			pNode = FindFirstChild();
			while (pNode != NULL)
			{
				if (!pNode->DoBecomeA(pBecomeA))
					return FALSE;

				pNode = pNode->FindNext();
			}
		}

		break;
		default: break;
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL NodeBevelController::CanBecomeA(BecomeA* pBecomeA);

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/2/99
	Inputs:		Ink class to test
	Purpose:	Can become a path !
	SeeAlso:	

********************************************************************************************/

BOOL NodeBevelController::CanBecomeA(BecomeA* pBecomeA)
{
	// Can only convert to paths
	if (!pBecomeA->BAPath())
		return FALSE;

	NodeRenderableInk * pNode = (NodeRenderableInk *)FindFirstChild(CC_RUNTIME_CLASS(NodeRenderableInk));

	while (pNode)
	{
		if (!pNode->NeedsParent(NULL))
		{
			// Update number of children that can become a path
			pNode->CanBecomeA(pBecomeA);
		}
		pNode = (NodeRenderableInk *)pNode->FindNext(CC_RUNTIME_CLASS(NodeRenderableInk));
	}
	
	return TRUE;
}

/********************************************************************************************

>	void NodeBevelController::CreateBevel()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/99
	Inputs:		
	Purpose:	Creates the bevel bitmaps immediately, not waiting for the next render-click
	SeeAlso:	

********************************************************************************************/

void NodeBevelController::CreateBevel()
{
	NodeBevel * pBevel = GetBevelNode();

	if (pBevel)
	{
		if(pBevel->GenerateBevel())
			m_pBevel = pBevel;
		else
			m_pBevel = NULL;
	}
	else
		ERROR3("Failed to create a new bevel node!");

	return;
}

/********************************************************************************************

>	BOOL NodeBevelController::PreApplyClipViewNode(RenderRegion* pRender)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/12/00
	Inputs:		A Render region to render into
	returns:	TRUE to say that we inserted a new ClipViewAttribute, FALSE to say we didn`t!
	Purpose:	Inserts a ClipViewAttribute on the Beveled object so that we can illiminate
				Aliasing effects when the bevel is rendered on top.

********************************************************************************************/
BOOL NodeBevelController::PreApplyClipViewNode(RenderRegion* pRender)
{
	// Setup a return Variable
	BOOL Ok = FALSE;

	// First check the pointers and make sure we`re an inner bevel
	if(pRender && m_pBevel && !m_pBevel->m_bOuter)
	{
		// Get the current join type on the bevel!
		AttrJoinType * pJoinType = NULL;
		m_pBevel->FindAppliedAttribute(CC_RUNTIME_CLASS(AttrJoinType), (NodeAttribute **)(&pJoinType));
		
		JointType JT = RoundJoin;
		if (pJoinType)
			JT = pJoinType->Value.JoinType;

		JoinStyles JoinStyle = JOIN_ROUND;
		JoinStyle = (JT == MitreJoin) ? JOIN_MITER : (JT == RoundJoin) ? JOIN_ROUND : JOIN_BEVEL;

		double Width = 750.0;
		if(pRender->GetScaledPixelWidth() > 0)
			Width = 2.0 * pRender->GetScaledPixelWidth();

		// Get the current Path from the Bevel node as this is the path that the
		// clipping view will be using.
		if(!m_pBevel->GetBevelPath())
			m_pBevel->CreateBevelPaths();

		Path* pTemp = m_pBevel->GetBevelPath();
		Path BevelPath;
		BevelPath.Initialise();
		BevelPath.CloneFrom(*pTemp);

		m_PathToClipVeiw.ClearPath();
		BevelPath.InitializeContourValues((UINT32)Width,JoinStyle,FALSE);

		// Make sure that the path we want to be doing the clipping is valid
		if(BevelPath.GetContourForStep(&m_PathToClipVeiw) > 2)
		{
			if(!m_pClipViewAttribute)
			{
				// Now find the first Renderable Ink Node and try to apply the new Attribute!
				NodeRenderableInk* pFoundNode = FindFirstChildInk();
				while(pFoundNode)
				{
					// Only apply the Attribute to NON bevel objects
					if(!pFoundNode->IsABevel())
					{
						// First do a quick check to see if we`ve already got a clipview on this node!
						Node* pChild = pFoundNode->FindFirstChild();
						while(pChild)
						{
							if(pChild->IsAClipViewAttr())
							{
								m_pClipViewAttribute = (AttrClipView*)pChild;
								break;
							}

							pChild = pChild->FindNext();
						}

						// If we found one then use it, else create a new one!
						if(!m_pClipViewAttribute)
							m_pClipViewAttribute = new AttrClipView(pFoundNode,FIRSTCHILD);

						if(m_pClipViewAttribute)
						{
							m_pClipViewAttribute->Value.SetClipPath(&m_PathToClipVeiw);
							Ok = TRUE;
						}
						break;
					}
					else
						pFoundNode = pFoundNode->FindNextInk();
				}
			}
			else
			{
				if(m_pClipViewAttribute)
					m_pClipViewAttribute->Value.SetClipPath(&m_PathToClipVeiw);
				Ok = TRUE;
			}
		}
		else
		{
			TRACEUSER( "MarkH", _T("Clip Path Came to Less Than 2 Points!"));
			Ok = TRUE;
		}
	}

	// return Ok!
	ERROR3IF(!Ok,"Didn`t Apply a ClipViewAttribute for inner bevels!");
	return Ok;
}

/********************************************************************************************

>	SubtreeRenderState NodeBevelController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip = TRUE)

	Author:		Phil_Martin (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/00
	Purpose:	Inserts a ClipViewAttribute on the Beveled object so that we can illiminate
				Aliasing effects when the bevel is rendered on top.
	See Also:	PreApplyClipViewNode();

********************************************************************************************/
SubtreeRenderState NodeBevelController::RenderSubtree(RenderRegion* pRender, Node** ppNextNode, BOOL bClip)
{
	if (pRender && pRender->IsPrinting())
	{
		TRACEUSER( "MarkH", _T("Rendering PrePrintChildren NODEBEVEL! <------------------------\n"));

		// Make sure we`ve got valid pointers and we`re not hittesting!
		if(pRender && !pRender->IsHitDetect() && m_pBevel && !m_pBevel->m_bOuter)
			PreApplyClipViewNode(pRender);

		return NodeGroup::RenderSubtree(pRender, ppNextNode, bClip);
	}
	else
	{
		TRACEUSER( "MarkH", _T("Rendering PreChildren NODEBEVEL! <------------------------\n"));

	#ifdef _DEBUG
	//	if(pRender)
	//		pRender->m_TraceOutContextLevels = TRUE;
	#endif

		if(m_pClipViewAttribute)
			m_pClipViewAttribute->EnableRendering();

		if(!m_pBevel)
			m_pBevel = GetBevelNode();

		// Make sure we`ve got valid pointers and we`re not hittesting!
		if(pRender && !pRender->IsHitDetect() && m_pBevel && !pRender->IsPrinting())
		{
			if(!m_pBevel->m_bOuter)
				PreApplyClipViewNode(pRender);
			else if(m_pBevel->m_bOuter && m_pClipViewAttribute)
			{
				m_pClipViewAttribute->UnlinkNodeFromTree();
				delete m_pClipViewAttribute;
				m_pClipViewAttribute = NULL;
			}
		}

		return NodeGroup::RenderSubtree(pRender, ppNextNode, bClip);
	}
	return SUBTREE_ROOTANDCHILDREN;
}

/********************************************************************************************

>	void NodeBevelController::PreExportRender( RenderRegion* pRender )

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/00
	Inputs:		A render region to render into
	Purpose:	Inserts a ClipViewAttribute on the Beveled object so that we can illiminate
				Aliasing effects when the bevel is rendered on top.
	See Also:	PreApplyClipViewNode();

********************************************************************************************/
void NodeBevelController::PreExportRender( RenderRegion* pRender )
{
	TRACEUSER( "MarkH", _T("PreExport Rendering NODEBEVEL! <------------------------\n"));

#ifdef DO_EXPORT
	if (pRender->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "start group" token
		EPSExportDC *pDC = (EPSExportDC *) pRender->GetRenderDC();

		// (ChrisG 16/01/01) don't export the ClipViewAttribute if it's an inner bevel,
		//	doing so can cause the internal object to be clipped to a path that is smaller
		//	than the hole left in the bitmap (especially if it is exported at a low zoom
		//	factor, e.g. 10%).
		if(m_pBevel)
		{
			if (!m_pBevel->m_bOuter && m_pClipViewAttribute)
			{
				m_pClipViewAttribute->EnableRendering(FALSE);
			}
			else if (m_pBevel->m_bOuter)
			{
				pDC->OutputToken(_T("u"));
				pDC->OutputNewLine();
			}
		}
	}
PORTNOTE("cmx", "Removed use of CMXRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	else if(pRender->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRender->GetRenderDC();
		DocRect BBox = GetBoundingRect(FALSE,FALSE);
		pDC->StartGroup(&BBox);
	}
#endif
#endif
}

BOOL NodeBevelController::ExportRender(RenderRegion* pRegion) 
{
#ifdef DO_EXPORT
	if (pRegion->IsKindOf(CC_RUNTIME_CLASS(EPSRenderRegion)))
	{
		// Output "end group" token
		EPSExportDC *pDC = (EPSExportDC *) pRegion->GetRenderDC();

		if(m_pBevel && m_pBevel->m_bOuter)
			pDC->OutputToken(_T("U"));

		pDC->OutputNewLine();
		
		// Tell caller we rendered ourselves ok
		return TRUE;
	}
PORTNOTE("cmx", "Removed use of CMXRenderRegion")
#ifndef EXCLUDE_FROM_XARALX
	else if(pRegion->IsKindOf(CC_RUNTIME_CLASS(CMXRenderRegion)))
	{
		// mark start of a group...
		CMXExportDC *pDC = (CMXExportDC *) pRegion->GetRenderDC();
		pDC->EndGroup();

		return TRUE;
	}
#endif
	if(m_pBevel)
		m_pBevel->SetConvertingFlag();

#endif
	// Render this node in the normal way
	return FALSE;
}

/********************************************************************************************

>	void NodeBevelController::Render(RenderRegion* pRender)

	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/9/00
	Inputs:		A render region to render into
	Purpose:	Main purpose is to remove any clipping attributes from the objects if
				we are doing inner bevels!

********************************************************************************************/
void NodeBevelController::Render(RenderRegion* pRender)
{
	TRACEUSER( "MarkH", _T("Finished Rendering NODEBEVEL! <------------------------\n"));

	if(m_pClipViewAttribute && !m_pClipViewAttribute->GetRenderStatus())
		m_pClipViewAttribute->EnableRendering();
#ifdef _DEBUG
//	if(pRender)
//		pRender->m_TraceOutContextLevels = FALSE;
#endif
}

/********************************************************************************************

>	BOOL NodeBevelController::RegenerateNode(UndoableOperation * pOp)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/99
	Inputs:		
	Returns:	TRUE to indicate that the node was changed, FALSE if nothing changed
	Purpose:	Forces a regeneration of the node in an undoable way
	SeeAlso:	

********************************************************************************************/
BOOL NodeBevelController::RegenerateNode(UndoableOperation * pOp, BOOL bCacheRender,
										 BOOL bInformParents)
{
	// we don't regenerate for printing (yet ! - we actually regenerate when rendered)
	NodeBevel * pBevel = GetBevelNode();

	if (!pBevel)
	{
		ERROR3("Cannot find bevel node (RegenerateNode)");
		return FALSE;
	}		

	List BevList;
	NodeListItem * pItem = NULL;
	RegenerateBevelAction* pRegenAction = NULL;

	Spread * pSpread = (Spread *)FindParent(CC_RUNTIME_CLASS(Spread));

	if (!pSpread)
		pSpread = Document::GetSelectedSpread();

//	Document * pDoc = Document::GetCurrent();

	ERROR2IF(pSpread == NULL, FALSE, "No spread found for regeneration of node");
	
	if (pOp)
	{
		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Couldn't create list item");
			return FALSE;
		}

		pItem->pNode = pBevel;

		BevList.AddTail(pItem);

		Document * pDoc		= Document::GetCurrent();

		if (!pDoc)
			return FALSE;

		Spread * pSpread	= this->FindParentSpread();

		if (!pSpread)
			return FALSE;

		// invalidate my region first
		ReleaseCached();
		pOp->DoInvalidateRegion(pSpread, BoundingRectangle);

		// set up the action to regenerate the bevel on this undo operation
		if (RegenerateBevelAction::Init(pOp, pOp->GetUndoActionList(),&BevList,&pRegenAction,
			bCacheRender)  == AC_FAIL)
		{
			ERROR3("RegenerateBevelAction::Init failed !\n");
			return FALSE;
		}

		BevList.DeleteAll();
	}
	else
	{
		// inform parents of change before
		if (bInformParents)
		{
			PreInformParentsOfRegenerate();
		}

		CreateBevel();
		InvalidateBoundingRect(TRUE);

		// inform parents of change
		if (bInformParents)
		{
			PostInformParentsOfRegenerate();
		}
	}

	// make sure we kick the sel range
	GetApplication()->UpdateSelection();

	return TRUE;
}

/********************************************************************************************

>	BOOL NodeBevelController::RegenerateBevelBitmap(UndoableOperation * pOp, 
				BOOL bInformParents = TRUE);


	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/5/99
	Inputs:		
	Purpose:	Forces a regeneration of the bevel, but doesn't regenerate the path
	SeeAlso:	

********************************************************************************************/
BOOL NodeBevelController::RegenerateBevelBitmap(UndoableOperation * pOp, 
				BOOL bInformParents)
{
	NodeBevel * pBevel = GetBevelNode();

	if (!pBevel)
	{
		ERROR3("Cannot find bevel node (RegenerateNode)");
		return TRUE;
	}		

	List BevList;
	NodeListItem * pItem = NULL;
	RegenerateBevelBitmapAction* pRegenAction = NULL;
	
	if (pOp)
	{
		// invalidate the bevel node
		pBevel->InvalidateBoundingRect();
		pOp->DoInvalidateNodeRegion(pBevel, TRUE, TRUE);

		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Couldn't create list item");
			return FALSE;
		}

		pItem->pNode = pBevel;
		BevList.AddTail(pItem);
		
		Document * pDoc		= Document::GetCurrent();

		if (!pDoc)
			return FALSE;

		Spread * pSpread	= this->FindParentSpread();

		if (!pSpread)
			return FALSE;

		// invalidate my region first
		ReleaseCached();
		pOp->DoInvalidateRegion(pSpread, BoundingRectangle);

		// set up the action to regenerate the bevel on this undo operation
		if (RegenerateBevelBitmapAction::Init(pOp, pOp->GetUndoActionList(),&BevList,&pRegenAction)  
			!= AC_OK)
		{
			ERROR3("RegenerateBevelAction::Init failed !\n");
			return FALSE;
		}

		BevList.DeleteAll();
	}
	else
	{
		// inform parents of change before
		if (bInformParents)
		{
			PreInformParentsOfRegenerate();
		}

		if (pBevel)
		{
			if (!pBevel->SetupVariables())
				return FALSE;
			
			if (!pBevel->ReRenderBevelBitmap(TRUE))
				return FALSE;
		}

		// inform parents of change
		if (bInformParents)
		{
			PostInformParentsOfRegenerate();
		}
	}

	// make sure we kick the sel range
	GetApplication()->UpdateSelection();

	return TRUE;
}


/********************************************************************************************

>	virtual BOOL NodeBevelController::AllowOp(	ObjChangeParam *pParam,
												BOOL SetOpPermissionState,
												BOOL DoPreTriggerEdit = TRUE )

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>; Karim 20/01/2000
	Created:	21/6/99
	Inputs:		pParam					describes the way an op wants to change the node
				SetOpPermissionState	if TRUE the OpPermission of nodes should be set
				DoPreTriggerEdit		if TRUE then NameGallery::PreTriggerEdit is called.
										*Must* be TRUE if the calling Op may make any nodes
										change their bounds, eg move, line width, cut.
										Use TRUE if unsure.
	Purpose:	
	SeeAlso:	

********************************************************************************************/
BOOL NodeBevelController::AllowOp(ObjChangeParam *pParam, BOOL SetOpPermissionState,
														  BOOL DoPreTriggerEdit)
{
	ERROR2IF(pParam == NULL, FALSE, "NodeBevelController::AllowOp; NULL pParam");

	// Set up a flag to see if any of the child objects get changed
	BOOL allowed = TRUE;

	UndoableOperation* pOp = pParam->GetOpPointer();

	if (pOp)
	{
		// can't contour a bevelled object
		if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpCreateContour)) ||
			pOp->IsKindOf(CC_RUNTIME_CLASS(OpChangeContourWidth)))
		{
			allowed = FALSE;
		}
		else if (pOp->IsKindOf(CC_RUNTIME_CLASS(OpRemoveBlend)))
		{
			allowed = FALSE;
			pParam->SetReasonForDenial(_R(IDS_CANT_REMOVE_BLEND_WHEN_BEVELLED));
		}
	}

	if (pParam->GetDirection() != OBJCHANGE_CALLEDBYCHILD ||
		pParam->GetCallingChild() != NULL)
	{
		BOOL AnyAllowed = AllowOp_AccountForCompound( pParam,
													  SetOpPermissionState,
													  DoPreTriggerEdit );

		// if called by a parent, just pass this result back.
		if (pParam->GetDirection() == OBJCHANGE_CALLEDBYPARENT)
			return AnyAllowed;
	}

	// if we allowed it, see if our parents do ...
	if (allowed && Parent != NULL && pParam->GetDirection() != OBJCHANGE_CALLEDBYPARENT)
	{
		ObjChangeDirection OldDirection = pParam->GetDirection();
		pParam->SetCallingChild(this);
		pParam->SetDirection(OBJCHANGE_CALLEDBYCHILD);
		allowed = Parent->AllowOp(pParam, SetOpPermissionState, DoPreTriggerEdit);
		pParam->SetDirection(OldDirection);
	}

	// if necessary, set permissions for OnChildChange.
	if (SetOpPermissionState)
		SetOpPermission(allowed ? PERMISSION_ALLOWED : PERMISSION_DENIED, TRUE);

	// if we're ok so far and were asked to do a PreTriggerEdit, then
	// determine whether the Op may change the bounds of some nodes.
	// If it may, then call NameGallery::PreTriggerEdit.
	if (allowed && DoPreTriggerEdit)
	{
		// if the Op is non-NULL then query its MayChangeNodeBounds() method.
		UndoableOperation* pChangeOp = pParam->GetOpPointer();
		if (pChangeOp != NULL && pChangeOp->MayChangeNodeBounds() && NameGallery::Instance())
		{
			NameGallery::Instance()->PreTriggerEdit(pChangeOp, pParam, this);
		}
	}

	return allowed;
}

/********************************************************************************************

>	void NodeBevelController::RenderEorDrag(RenderRegion * pRegion)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/7/99
	Inputs:		
	Purpose:	Renders the Eor drag stuff
	SeeAlso:	

********************************************************************************************/
void NodeBevelController::RenderEorDrag(RenderRegion * pRegion)
{
	BOOL bChildrenSelected = FALSE;

	Node * pNode = FindFirstChild();
	Node * pInsideNode = NULL;

	// check for a select inside drag
	while (pNode)
	{
		if (pNode->IsSelected())
		{
			bChildrenSelected = TRUE;
			break;
		}

		pNode = pNode->FindNext();
	}

	// check whether my parents are selected
	// if one of my parents are selected then this can't be a select inside !
	if (!bChildrenSelected)
	{
		Node * pSelParent = FindParent();
	
		while (pSelParent && pSelParent->IsAnObject())
		{
			if (pSelParent->IsSelected())
			{
				bChildrenSelected = TRUE;
				break;
			}

			pSelParent = pSelParent->FindParent();
		}
	}

	if (bChildrenSelected || IsSelected())
	{
		NodeBevel * pBevel = GetBevelNode();

		if (pBevel)
		{
			pBevel->RenderEorDrag(pRegion);
		}	
	}
	else
	{
		// render all selected nodes eor drags
		pInsideNode = FindFirstDepthFirst();

		while (pInsideNode != this && pInsideNode != NULL)
		{
			if (pInsideNode->IsSelected() && pInsideNode->IsAnObject())
			{
				((NodeRenderableInk *)pInsideNode)->RenderEorDrag(pRegion);
			}

			pInsideNode = pInsideNode->FindNextDepthFirst(this);
		}
	}
}



/********************************************************************************************

>	virtual DocRect NodeBevelController::ValidateExtend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	
	Returns:	TRUE if extending this Node and its children is a reversible operation,
				FALSE otherwise.
	Purpose:	Tests the reversibility of an Extend operation applied to this node.

				In the case of a NodeBevelController, this function is identical to Node's
				implementation, except that this controller's own NodeBevel child is ignored.
				This allows the NodeBevel to tell its parent to extend without fear of
				infinite recursion.
	Errors:		
	See also:	IsTypeExtendible(), Extend().

********************************************************************************************/
DocRect NodeBevelController::ValidateExtend(const ExtendParams& ExtParams)
{
	Node* pBob = GetBevelNode();
	DocRect drMinExtend(INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX), drThisMinExtend;
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		if (pChildNode == pBob)
			continue;

		drThisMinExtend = pChildNode->ValidateExtend(ExtParams);
		if (drMinExtend.lo.x > drThisMinExtend.lo.x) drMinExtend.lo.x = drThisMinExtend.lo.x;
		if (drMinExtend.lo.y > drThisMinExtend.lo.y) drMinExtend.lo.y = drThisMinExtend.lo.y;
		if (drMinExtend.hi.x > drThisMinExtend.hi.x) drMinExtend.hi.x = drThisMinExtend.hi.x;
		if (drMinExtend.hi.y > drThisMinExtend.hi.y) drMinExtend.hi.y = drThisMinExtend.hi.y;
	}
	return drMinExtend;
}



/********************************************************************************************

>	virtual void NodeBevelController::Extend(const ExtendParams& ExtParams)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	06/12/1999
	Inputs:		ExtParams		description parameters for the extension.
	Outputs:	Some of this node's children may have their dimensions altered.
	Returns:	
	Purpose:	Perform an Extend operation on this Node, and its children if appropriate.

				In the case of a NodeBevelController, this function is identical to Node's
				implementation, except that this controller's own NodeBevel child is ignored.
				This allows the NodeBevel to tell its parent to extend without fear of
				infinite recursion.
	Errors:		
	See also:	

********************************************************************************************/
void NodeBevelController::Extend(const ExtendParams& ExtParams)
{
	Node* pBob = GetBevelNode();
	for (	Node* pChildNode = FindFirstChild();
			pChildNode != NULL;
			pChildNode = pChildNode->FindNext()	)
	{
		if (pChildNode == pBob)
			continue;

		pChildNode->Extend(ExtParams);
	}

	// Add an action to regen the bevel here
	// and instead of regening the bev when the m_PerformedExtend occurs in the child change
	// cut out and let the action do the stuff
	m_PerformedExtend = TRUE; 

	RegenerateNode(ExtParams.pOp, FALSE, FALSE);
}

/********************************************************************************************

>	virtual NodeRenderableInk * 
			NodeBevelController::CreateTreeFromNodeToBlend(NodeRenderableInk * pNode)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/2/2000
	Inputs:		The node to wrap up
	Outputs:	NULL for failure
	Returns:	The root of the new tree
	Purpose:	Creates a copy of my tree, wrapping up the given node
	
********************************************************************************************/
NodeRenderableInk * 
			NodeBevelController::CreateTreeFromNodeToBlend(NodeRenderableInk * pNode, 
			CCAttrMap * pAttrMap)
{
	// remove all attributes from the node's subtree
	ERROR2IF(pNode->FindParent() != NULL, NULL, "Node shouldn't have parents");

	if (pNode->IsNodePath())
	{
		pNode->CascadeDelete();
	}
	
	// apply the attribute map to this node
	if (pNode->IsNodePath())
	{
		pNode->ApplyAttributes(pAttrMap, FALSE);
	}

	// now, lets make a copy of myself & the bevel node
	NodeBevelController * pControl = NULL;

	pControl = (NodeBevelController *)this->PublicCopy();
	ERRORIF(pControl == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	NodeBevel * pBevel = (NodeBevel *)FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
	ERROR2IF(pBevel == NULL, NULL, "Can't find bevel node");

	// apply the attribute map of the original bevel node to the bevel copy node
	NodeBevel * pCopyBevel = (NodeBevel *)pBevel->PublicCopy();
	ERRORIF(pCopyBevel == NULL, _R(IDE_NOMORE_MEMORY), NULL);
	
	CCAttrMap * pBevMap = CCAttrMap::MakeAppliedAttrMap(pBevel);
	ERRORIF(pBevMap == NULL, _R(IDE_NOMORE_MEMORY), NULL);

	pCopyBevel->ApplyAttributes(pBevMap, FALSE);

	delete pBevMap;

	NodeBevelBegin * pNewBevelBegin = new NodeBevelBegin;
	ERRORIF(pNewBevelBegin == NULL, _R(IDE_NOMORE_MEMORY), NULL);
	pNewBevelBegin->AttachNode(pControl, FIRSTCHILD);

	pNode->AttachNode(pNewBevelBegin, NEXT);
	
	if(m_bOuter)
		pCopyBevel->AttachNode(pNewBevelBegin, NEXT);
	else
		pCopyBevel->AttachNode(pControl, LASTCHILD);

	// Karim 24/11/2000
	// The returned node will be blended, which means its bounds will be incorrect.
	// Therefore, invalidate them here and hope that somebody doesn't ask for them
	// before the blend happens.
	pControl->InvalidateBoundingRect(TRUE);

	return pControl;
}

void HandleOneToNoneAttributeRelationship (NodeRenderableInk* pBevel, CCAttrMap* pMap)
{
	NodeAttribute* pAttr = NULL;
	
	pBevel->FindAppliedAttribute (CC_RUNTIME_CLASS (AttrTranspFillGeometry), &pAttr);
	NodeAttribute* pNew = (NodeAttribute*) pAttr->SimpleCopy ();
	delete (pMap->ReplaceAttribute (pNew));


	pBevel->FindAppliedAttribute (CC_RUNTIME_CLASS (AttrTranspFillMapping), &pAttr);
	pNew = (NodeAttribute*) pAttr->SimpleCopy ();
	delete (pMap->ReplaceAttribute (pNew));

	pBevel->FindAppliedAttribute (CC_RUNTIME_CLASS (AttrFillGeometry), &pAttr);
	pNew = (NodeAttribute*) pAttr->SimpleCopy ();
	delete (pMap->ReplaceAttribute (pNew));

	pBevel->FindAppliedAttribute (CC_RUNTIME_CLASS (AttrFillMapping), &pAttr);
	pNew = (NodeAttribute*) pAttr->SimpleCopy ();
	delete (pMap->ReplaceAttribute (pNew));
}



/********************************************************************************************

>	void ApplyRelevantAttributes (CCAttrMap* ptrBevelMap, CCAttrMap* oldPathMap,
								  CCAttrMap* newPathMap, CCAttrMap* pMap)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000
	Inputs:		ptrBevelMap - the attribute map for the intermediate NodeBevel
				oldPathMap - the attribute map for the node that were replacing
				newPathMap - the attribute map for the node that were replacing by
				pMap - the resulting attribute map for the for the intermediate NodeBevel
				pSingleMap - the resulting attribute map for the for the intermediate NodeBevel
				(when blending from one to none)
	Outputs:	-
	Returns:	-
	Purpose:	Determins which attributes need to be applied to the bevel, so that we use
				appropriate attributes.  For example, if bevel and old path both have the same
				AttrFlatColourFill; then apply the one in newPathMap to the NodeBevel (via pMap).
	
********************************************************************************************/
void NodeBevelController::ApplyRelevantAttributes (CCAttrMap* ptrBevelMap, CCAttrMap* oldPathMap,
												   CCAttrMap* newPathMap, CCAttrMap* pMap,
												   CCAttrMap* pSingleMap)
{
	ERROR3IF(!ptrBevelMap,	"Bogus bevel!");
	ERROR3IF(!oldPathMap,	"Bogus old path attributes!");
	ERROR3IF(!newPathMap,	"Bogus newpath!");
	ERROR3IF(!(pMap || pSingleMap), "Bogus result attribute maps!");

	NodeAttribute* bevAttr = NULL, *oldPAttr = NULL, *newPAttr = NULL, *newAttr = NULL;

	BOOL done = FALSE;
	INT32 i = 0;
	CCRuntimeClass* pClass = NULL;	

	while (!done)
	{
		switch (i)
		{
			case 0:		pClass = CC_RUNTIME_CLASS (AttrFillGeometry);			break;
			case 1:		pClass = CC_RUNTIME_CLASS (AttrFillMapping);			break;
			case 2:		pClass = CC_RUNTIME_CLASS (AttrTranspFillGeometry);		break;
			case 3:		pClass = CC_RUNTIME_CLASS (AttrTranspFillMapping);		break;
			default:	done = TRUE;
		}

		if (!done)
		{
			ptrBevelMap->Lookup( pClass, (void*&) bevAttr);
			oldPathMap->Lookup( pClass, (void*&) oldPAttr);

			if (bevAttr && oldPAttr)
			{
				if (*bevAttr == *oldPAttr)
				{
					// both the bevel and the original path have the same value for this attribute class
					// we MUST generate the bevel with respect to the attribute in newPathMap (for this attribute class)

					newPathMap->Lookup( pClass, (void*&) newPAttr);
					
					if (newPAttr)
					{
						newAttr = (NodeAttribute*) newPAttr->SimpleCopy ();
						if (pMap)
						{
							delete (pMap->ReplaceAttribute (newAttr));
						}
						else if (pSingleMap)
						{
							delete (pSingleMap->ReplaceAttribute (newAttr));
						}
					}
				}
			}

			i++;

			bevAttr = NULL, oldPAttr = NULL, newPAttr = NULL, newAttr = NULL;
		}
	}
}



/********************************************************************************************

>	BOOL NodeBevelController::EndBlendStep(BlendNodeParam * pParam)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/10/2000
	Inputs:		pParam	-	the blend node param (see NodeBlender) for the end of blending
	Outputs:	
	Returns:	TRUE for success, FALSE for failure
	Purpose:	Informs the node of the rendering of the blend step finished 	
	
********************************************************************************************/
BOOL NodeBevelController::EndBlendStep(BlendNodeParam * pParam)
{
	// CGS:  firstly, are we just rendering, or are we making shapes?

	if (!pParam->GetHandleBecomeA ())
	{
		// were just rendering ....

//		BOOL done = FALSE;

		BOOL okToRender = TRUE;
		
		// get hold of the current path processor ....
		if (pParam->GetPathProcessor ())
		{
			ERROR2IF(!pParam->GetPathProcessor ()->IS_KIND_OF(SumAllPathsPathProcessor),
					 FALSE, "First path processor isn't a sum all paths path processor");

			// grab the path processor
			SumAllPathsPathProcessor * pProc = (SumAllPathsPathProcessor *) pParam->GetPathProcessor ();

			pProc->SetEnabled(FALSE);		// disable it

			// we can now get on and generate intermediate blend steps.  NOTE:  this node contour
			// controller should not be touched at all - since it is held within the tree
			// so we make a copy that we can play with ....
				
			Node* copy = NULL;
			this->NodeCopy (&copy);

			NodeBevelController* ptrCopy = (NodeBevelController*) copy;

			if (!ptrCopy) { return (FALSE); }		// out of memory

			// find the contour
			NodeBevel * pBevelCopy = (NodeBevel *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
			ERROR2IF(pBevelCopy == NULL, FALSE, "Can't find the contour node");

#ifdef _DEBUG		// special debug info
//			INT32 id = ++NodeBevelController::contourLastID;
//			ptrCopy->myContourID = id;
//			char strId [100];
//			wsprintf (strId, "Pushing NodeBevelController ID:  %i\n", ptrCopy->myContourID);	
//			TRACEUSER ("ChrisS", strId);
#endif

	//////////////////////////////////////////////////////////////////////////////////////////////////

			// now the fun begins, lest blend attributes for this particular blend step ....
			
			CCAttrMap * pMap = NULL;						// if pEndControl is a contour as well
			CCAttrMap * pPrimaryBevelAttrMap = NULL;		// if pEndControl is not a contour

			// blend the contour's attributes
			NodeBevelController * pEndControl = (NodeBevelController *)FindAssociatedBlendNode(pParam);

			if (pEndControl)
			{
				// we are blending two bevels YIPEE!		
				
				NodeBevel * pEndBevel = (NodeBevel *)pEndControl->FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
				NodeRenderableInk * pOrigEndNode = pEndControl->GetBlendCreatedByNode();

				// somethings a bit wrong if either of these fire ....
				ERROR2IF(pOrigEndNode == NULL, FALSE, "Cant find the original contour node which this is based on");
				ERROR2IF(!pOrigEndNode->IS_KIND_OF(NodeBevelController), FALSE, "Original node isn't a NodeBevelController");

				pOrigEndNode = (NodeBevel *)pOrigEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));

				BlendPath MyStartPath;
				MyStartPath.Initialise((NodeRenderableInk *)pBevelCopy->PublicCopy(), -1, pBevelCopy, 1, 0, NULL);
				
				BlendPath MyEndPath;
				MyEndPath.Initialise((NodeRenderableInk *)pEndBevel->PublicCopy(), -1, pOrigEndNode, 1, 0, NULL);
				
				BlendNodeParam BNParam;
				BNParam.Init(pParam, &MyStartPath, &MyEndPath);
				
				pMap = new CCAttrMap(30);

				BlendHelpers * pHelp = new BlendHelpers;
				ERRORIF(pHelp == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// now, perform the blend of the attributes
				if (!pHelp->BlendAttributes(&BNParam, pMap))
				{
					pParam->GetRenderRegion()->PopPathProcessor();
					okToRender = FALSE;
				}
				
				delete pHelp;
			}
			else
			{
				// were blending a bevelled node to a non-bevelled node.
				// since we have not generated an attribute map, shadows that are generated
				// render as white ....

				// we need to make a attribute map for *this* NodeBevel, and apply it to
				// the copy ....

				// build an attribute map for the contour
				
				CCAttrMap* temp = CCAttrMap::MakeAppliedAttrMap(pBevelCopy);
				
				pPrimaryBevelAttrMap = temp->Copy ();		// MUST copy, cause those in temp point to attributes
															// that will soon be deleted

				if (temp) {	delete temp; temp = NULL; }
			}

			// scan for the path that original contour was generated with ....
			NodePath* currentPath = (NodePath *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodePath));
			
			// in-order that we can make things work correctly, we MUST obtain attribute maps for
			// both the bevel and the path; and take a copy of both of these (so that we can still
			// use their attributes without them referencing attributes in our subtree that will be deleted).
			
			CCAttrMap* currentPathAttrMap = CCAttrMap::MakeAppliedAttrMap (currentPath);		// path
			CCAttrMap* currentBevelAttrMap = CCAttrMap::MakeAppliedAttrMap (pBevelCopy);	// bevel
			CCAttrMap* currentPathAttrMapC = currentPathAttrMap->Copy ();			// we MUST copy the attrmap!
			CCAttrMap* currentBevelAttrMapC = currentBevelAttrMap->Copy ();			// we MUST copy the attrmap!

			// NOTE:  don;t call DeleteAttributes () on either of the following!  If you do, then you will
			// delete things that MUST be deleted correctly later
			if (currentPathAttrMap)		{	delete currentPathAttrMap;	currentPathAttrMap = NULL; }
			if (currentBevelAttrMap)	{	delete currentBevelAttrMap;	currentBevelAttrMap = NULL; }
			
			pBevelCopy->DeleteChildren (pBevelCopy->FindFirstChild ());	// our copy is unaffected by this

			// if we find one, we know get on and merge all the paths so that the contour can
			// be generated correctly.

			// we do this by:

			// replacing currentPath by a group that contains all paths on the path processors list
			// (i.e.  we replace it by the blended paths - who have their blended attribute maps
			// applied to them)

			if (currentPath)
			{
				currentPath->CascadeDelete ();
				delete (currentPath);
				currentPath = NULL;

				NodeGroup* newGroup = new NodeGroup ();

				ListItem* pItem = pProc->GetList ()->GetHead ();
				
				Path* blendPath = NULL;
				CCAttrMap* blendMap = NULL;

				while (pItem)
				{
					blendPath = ((SumAllPathsElem *) pItem)->GetPath ();
					blendMap = ((SumAllPathsElem *) pItem)->GetAttrMap ();
					
					NodePath* newPath = new NodePath ();
					INT32 numCoords = blendPath->GetNumCoords ();
					newPath->InkPath.ClearPath ();
					newPath->InkPath.Initialise (numCoords);
					newPath->InkPath.CopyPathDataFrom (blendPath);
					ApplyRelevantAttributes (currentBevelAttrMapC, currentPathAttrMapC, blendMap, pMap, pPrimaryBevelAttrMap);
					blendMap->ApplyAttributesToNode (newPath);
					newPath->AttachNode(newGroup, LASTCHILD);

					pItem = pProc->GetList ()->GetNext (pItem);
					delete ((SumAllPathsElem *) pProc->GetList()->RemoveHead ());
				}

				if (ptrCopy->m_bOuter)		// an outer contour
				{
					newGroup->AttachNode(pBevelCopy, NEXT);
				}
				else if (!ptrCopy->m_bOuter)
				{
					newGroup->AttachNode(pBevelCopy, PREV);
				}
			}

			// apply blended contour attributes ....

			if (pMap)
			{
				pMap->ApplyAttributesToNode (pBevelCopy);
			}

			if (pPrimaryBevelAttrMap)
			{
				pPrimaryBevelAttrMap->ApplyAttributesToNode (pBevelCopy);
			}

			// generate the contour (which will correctly be generated for merged paths if needed)
			
			if (!pBevelCopy->GenerateBevel ())
			{
				okToRender = FALSE;
			}

			// now render the contour controller as a complete node (just like the document renders)
			
			if (okToRender)
			{
				RenderRegion* pRegion = pParam->GetRenderRegion();
				if (pRegion)
					pRegion->RenderTreeNoCache(ptrCopy);
			}

			if (pMap)					{ pMap->DeleteAttributes ();					delete pMap; }
			if (pPrimaryBevelAttrMap)	{ pPrimaryBevelAttrMap->DeleteAttributes ();	delete pPrimaryBevelAttrMap; }
			if (currentPathAttrMapC)	{ currentPathAttrMapC->DeleteAttributes ();		delete currentPathAttrMapC; }
			if (currentBevelAttrMapC)	{ currentBevelAttrMapC->DeleteAttributes ();	delete currentBevelAttrMapC; }

			// if the contour is NOT shadowed, then we can safely delete the copy of it

			Node* parent = FindParent ();

			if (parent != NULL)
			{
				if (!(IS_A (parent, NodeShadowController)))
				{
					ptrCopy->CascadeDelete ();
					delete (ptrCopy);
				}
				else
				{
					SetShadowDeleteThisNode (ptrCopy);
				}
			}
			else
			{
				ptrCopy->CascadeDelete ();
				delete (ptrCopy);
			}
		}
	}
	else	// were doing a make shapes (i.e.  become a .....)
	{	
		// get hold of the current path processor ....
		if (pParam->GetPathProcessor ())
		{
			ERROR2IF(!pParam->GetPathProcessor ()->IS_KIND_OF(SumAllPathsPathProcessor),
					 FALSE, "First path processor isn't a sum all paths path processor");

			// grab the path processor
			SumAllPathsPathProcessor * pProc = (SumAllPathsPathProcessor *) pParam->GetPathProcessor ();

			pProc->SetEnabled(FALSE);		// disable it

			// we can now get on and generate intermediate blend steps.  NOTE:  this node contour
			// controller should not be touched at all - since it is held within the tree
			// so we make a copy that we can play with ....
				
			Node* copy = NULL;
			this->NodeCopy (&copy);

			NodeBevelController* ptrCopy = (NodeBevelController*) copy;

			if (!ptrCopy) { return (FALSE); }		// out of memory

			// find the contour
			NodeBevel * pBevelCopy = (NodeBevel *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
			ERROR2IF(pBevelCopy == NULL, FALSE, "Can't find the contour node");

	//////////////////////////////////////////////////////////////////////////////////////////////////

			// now the fun begins, lest blend attributes for this particular blend step ....
			
			CCAttrMap * pMap = NULL;						// if pEndControl is a contour as well
			CCAttrMap * pPrimaryBevelAttrMap = NULL;		// if pEndControl is not a contour

			// blend the contour's attributes
			NodeBevelController * pEndControl = (NodeBevelController *)FindAssociatedBlendNode(pParam);

			if (pEndControl)
			{
				// we are blending two bevels YIPEE!		
				
				NodeBevel * pEndBevel = (NodeBevel *)pEndControl->FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));
				NodeRenderableInk * pOrigEndNode = pEndControl->GetBlendCreatedByNode();

				// somethings a bit wrong if either of these fire ....
				ERROR2IF(pOrigEndNode == NULL, FALSE, "Cant find the original contour node which this is based on");
				ERROR2IF(!pOrigEndNode->IS_KIND_OF(NodeBevelController), FALSE, "Original node isn't a NodeBevelController");

				pOrigEndNode = (NodeBevel *)pOrigEndNode->FindFirstChild(CC_RUNTIME_CLASS(NodeBevel));

				BlendPath MyStartPath;
				MyStartPath.Initialise((NodeRenderableInk *)pBevelCopy->PublicCopy(), -1, pBevelCopy, 1, 0, NULL);
				
				BlendPath MyEndPath;
				MyEndPath.Initialise((NodeRenderableInk *)pEndBevel->PublicCopy(), -1, pOrigEndNode, 1, 0, NULL);
				
				BlendNodeParam BNParam;
				BNParam.Init(pParam, &MyStartPath, &MyEndPath);
				
				pMap = new CCAttrMap(30);

				BlendHelpers * pHelp = new BlendHelpers;
				ERRORIF(pHelp == NULL, _R(IDE_NOMORE_MEMORY), FALSE);
				
				// now, perform the blend of the attributes
				if (!pHelp->BlendAttributes(&BNParam, pMap))
				{
					delete (pMap);
					return (FALSE);
				}
				
				delete pHelp;
			}
			else
			{
				// were blending a bevelled node to a non-bevelled node.
				// since we have not generated an attribute map, shadows that are generated
				// render as white ....

				// we need to make a attribute map for *this* NodeBevel, and apply it to
				// the copy ....

				CCAttrMap* temp = CCAttrMap::MakeAppliedAttrMap(pBevelCopy);
				
				pPrimaryBevelAttrMap = temp->Copy ();		// MUST copy, cause those in temp point to attributes
															// that will soon be deleted

				if (temp) {	delete temp; temp = NULL; }
			}

			// scan for the path that original contour was generated with
			NodePath* currentPath = (NodePath *)ptrCopy->FindFirstChild(CC_RUNTIME_CLASS(NodePath));

			// in-order that we can make things work correctly, we MUST obtain attribute maps for
			// both the bevel and the path; and take a copy of both of these (so that we can still
			// use their attributes without them referencing attributes in our subtree that will be deleted).
			
			CCAttrMap* currentPathAttrMap = CCAttrMap::MakeAppliedAttrMap (currentPath);	// path
			CCAttrMap* currentBevelAttrMap = CCAttrMap::MakeAppliedAttrMap (pBevelCopy);	// bevel
			CCAttrMap* currentPathAttrMapC = currentPathAttrMap->Copy ();			// we MUST copy the attrmap!
			CCAttrMap* currentBevelAttrMapC = currentBevelAttrMap->Copy ();			// we MUST copy the attrmap!

			// NOTE:  don;t call DeleteAttributes () on either of the following!  If you do, then you will
			// delete things that MUST be deleted correctly later
			if (currentPathAttrMap)		{	delete currentPathAttrMap;	currentPathAttrMap = NULL; }
			if (currentBevelAttrMap)	{	delete currentBevelAttrMap;	currentBevelAttrMap = NULL; }
			
			pBevelCopy->DeleteChildren (pBevelCopy->FindFirstChild ());	// our copy is unaffected by this

			// if we find one, we know get on and merge all the paths so that the contour can
			// be generated correctly.

			// we do this by:

			// replacing currentPath by a group that contains all paths on the path processors list
			// (i.e.  we replace it by the blended paths - who have their blended attribute maps
			// applied to them)

			std::list<NodePath*> pathPtrs;				// a list of the new paths (so that we may call normaliseattributes)

			if (currentPath)
			{
				currentPath->CascadeDelete ();
				delete (currentPath);
				currentPath = NULL;

				NodeGroup* newGroup = new NodeGroup ();

				ListItem* pItem = pProc->GetList ()->GetHead ();
				
				Path* blendPath = NULL;
				CCAttrMap* blendMap = NULL;

				while (pItem)
				{
					blendPath = ((SumAllPathsElem *) pItem)->GetPath ();
					blendMap = ((SumAllPathsElem *) pItem)->GetAttrMap ();
					
					NodePath* newPath = new NodePath ();
					INT32 numCoords = blendPath->GetNumCoords ();
					newPath->InkPath.ClearPath ();
					newPath->InkPath.Initialise (numCoords);
					newPath->InkPath.CopyPathDataFrom (blendPath);
					ApplyRelevantAttributes (currentBevelAttrMapC, currentPathAttrMapC, blendMap, pMap, pPrimaryBevelAttrMap);
					blendMap->ApplyAttributesToNode (newPath);
					newPath->AttachNode(newGroup, LASTCHILD);

					pathPtrs.push_back (newPath);
					
					pItem = pProc->GetList ()->GetNext (pItem);
					delete ((SumAllPathsElem *) pProc->GetList()->RemoveHead ());
				}

				if (ptrCopy->m_bOuter)		// an outer contour
				{
					newGroup->AttachNode(pBevelCopy, NEXT);
				}
				else if (!ptrCopy->m_bOuter)
				{
					newGroup->AttachNode(pBevelCopy, PREV);
				}
			}

			// apply blended contour attributes ....

			if (pMap)
			{
				pMap->ApplyAttributesToNode (pBevelCopy);
			}

			if (pPrimaryBevelAttrMap)
			{
				pPrimaryBevelAttrMap->ApplyAttributesToNode (pBevelCopy);
			}

			// generate the contour (which will correctly be generated for merged paths if needed)
			
			pBevelCopy->GenerateBevel ();

			// now insert into the tree or passback ....

			BecomeA* accessPtr = pParam->GetHandleBecomeA ()->GetBecomeA ();
			UndoableOperation* pUndoOp = accessPtr->GetUndoOp ();

			Node* parent = NULL;
			NodeRenderableInk* pCreator = GetBlendCreatedByNode ();
			ERROR2IF(pCreator == NULL, FALSE, "Cant find the original bevel node which this is based on");
			
			parent = pCreator->FindParent (CC_RUNTIME_CLASS (NodeShadowController));

			// we MUST also scan to see if the blend was shadowed (and not the bevel) ....
			
			Node* pBlenderParent = NULL;
			Node* pBlender = pParam->GetNodeBlend ();
			ERROR2IF(pBlender == NULL, FALSE, "Cant find the original node blend which this node is based on!");

			pBlenderParent = pBlender->FindParent (CC_RUNTIME_CLASS (NodeShadowController));

			BOOL parentIsNodeShadow = FALSE;

			if (parent)
			{
				if (!pBlenderParent)	// if the blend was NOT shadowed
				{
					parentIsNodeShadow = TRUE;
				}
				// else the blend was shadowed we need to insert into the tree
			}

			// are we wanting to be inserted into the tree?
			if (accessPtr->GetReason () == BECOMEA_REPLACE)
			{
				if (!parentIsNodeShadow)
				{
					if (!pUndoOp->DoInsertNewNode (ptrCopy,pParam->GetHandleBecomeA ()->GetContextNode (),PREV,TRUE,FALSE,FALSE,TRUE))
					{
						return FALSE;
					}

					// now normalise all attributes ....
					
					pBevelCopy->NormaliseAttributes ();

					std::list<NodePath*>::iterator i;

					for (i = pathPtrs.begin (); i != pathPtrs.end (); i++)
					{
						(*i)->NormaliseAttributes ();
					}

					// if we are required to insert paths into the tree - THEN we MUST call DoBecomeA on
					// ptrCopy.  This call now also creates the bevel bitmap for us

					if (accessPtr->GetInsertComplexBlendStepsAsPaths ())
					{
						ptrCopy->DoBecomeA (accessPtr);
					}
				}
				else
				{
					SetShadowThisNode (ptrCopy);	// we need to set this so that NodeShadowController
													// can shadow ptrCopy

					// um, NodeShadowController now needs to be responsible for NormaliseAttributes for
					// the paths that are within our list.  Um I say, exactly how does one do this?
				}
			}
			// or do we just want our paths?
			else if (accessPtr->GetReason () == BECOMEA_PASSBACK)
			{
				if (parentIsNodeShadow)
				{
					SetShadowThisNode (ptrCopy);	// we need to set this so that NodeShadowController
													// can shadow ptrCopy
				}
				ptrCopy->DoBecomeA (accessPtr);
				ptrCopy->SetBlenderNode (pParam->GetNodeBlend ());

#ifdef _DEBUG
//				INT32 id = ++NodeBevelController::bevelLastBecomeAID;
//				ptrCopy->myBevelBecomeAID = id;
//				char strId [100];
//				wsprintf (strId, "Pushing NodeBevelController PASSBACK ID:  %i\n", ptrCopy->myBevelBecomeAID);			
//				TRACEUSER ("ChrisS", strId);
#endif

				// setup necessary stuff so that we can delete ptrCopy at the relevant time ....
				
				if (AllocatedBlendConsList (LT_BECOMEA_BEVELSLIST))
				{	
					BlendConsListInsert (LT_BECOMEA_BEVELSLIST, ptrCopy);
				}
				else
				{
					AllocBlendConsList (LT_BECOMEA_BEVELSLIST);
					BlendConsListInsert (LT_BECOMEA_BEVELSLIST, ptrCopy);
				}
			}	

			if (pMap)					{ pMap->DeleteAttributes ();					delete pMap; }
			if (pPrimaryBevelAttrMap)	{ pPrimaryBevelAttrMap->DeleteAttributes ();	delete pPrimaryBevelAttrMap; }
			if (currentPathAttrMapC)	{ currentPathAttrMapC->DeleteAttributes ();		delete currentPathAttrMapC; }
			if (currentBevelAttrMapC)	{ currentBevelAttrMapC->DeleteAttributes ();	delete currentBevelAttrMapC; }
		}
	}
	
	return TRUE;
}



/********************************************************************************************

>	void NodeGroup::RenderTinyBlobs(RenderRegion* pRender)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/6/94
	Inputs:		pRender - The region to draw the blobs in
	Purpose:	Renders the tiny blobs for a group (A Single blob on the topmost child object
				in the group)

********************************************************************************************/

void NodeBevelController::RenderTinyBlobs(RenderRegion* pRegion)
{
#if !defined(EXCLUDE_FROM_RALPH)
	// get the topmost object in this group and tell it to render its tiny blobs.
	Node* pNode = FindLastChild();
	while (pNode != NULL && !pNode->IsAnObject())
		pNode = pNode->FindPrevious();

	// empty groups are not allowed!
	if (pNode == NULL)
	{
		ERROR3("NodeGroup::RenderTinyBlobs; This group is empty! Shouldn't be!");
		return;
	}
	else
	{
		((NodeRenderableInk*)pNode)->RenderTinyBlobs(pRegion);
	}

#endif
}



/****************************************************************************

>	void NodeBevelController::GetDebugDetails( StringBase* Str )

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	07/07/2005

	Inputs:		Str			- pointer to a StringBase
	Purpose:	Builds debug info for camelot tree dialog

****************************************************************************/

void NodeBevelController::GetDebugDetails( StringBase* Str )
{
#if DEBUG_TREE
	// Call base class
	NodeRenderableInk::GetDebugDetails( Str );
	
	String_256 TempStr;
	String_256 TempStr2;
		
	(*Str) += TEXT( "\r\nNodeBevelController Data Dump\r\n" );

	DocRect BlobRect = GetBlobBoundingRect();
	TempStr._MakeMsg( TEXT("Blob Bounding Rect :\r\n\t#1%ld,\t#2%ld\r\n\t#3%ld,\t#4%ld\r\n"),
					  BlobRect.lo.x, BlobRect.lo.y, BlobRect.hi.x, BlobRect.hi.y );
	(*Str) += TempStr;

	TempStr._MakeMsg( TEXT("Type :\t#1%d\r\n"), m_BevelType);
	(*Str) += TempStr;

#endif
}



//////////////////////////////////////////////////////////////////////////////////////////////
// BevelNodeTreeFactory implementation

/********************************************************************************************

>	BevelNodeTreeFactory::BevelNodeTreeFactory(NodeBevelController * pControl)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/99
	Inputs:		The controller node which to set up the values from
	Purpose:	Sets up the class factory so that it can recreate the bevel with the
				same parameters as the controller node passed in
	SeeAlso:	

********************************************************************************************/
BevelNodeTreeFactory::BevelNodeTreeFactory(NodeBevelController * pControl)
{
	// first, get the bevel node
	NodeBevel * pBevel = pControl->GetBevelNode();
	
	m_BevelType = pBevel->m_BevelType;
	m_BevelIndent = pBevel->m_Indent;
	m_BevelContrast = pBevel->m_Contrast;
	m_BevelLightAngle = (INT32)pBevel->m_LightAngle;

	if (pBevel->m_bOuter)
	{
		m_BevelIndent = -m_BevelIndent;
	}
	
	/*NodeBevelBegin * pBevelBegin = */
		(NodeBevelBegin *)pControl->FindFirstChild(CC_RUNTIME_CLASS(NodeBevelBegin));

	// now, get the attribute maps for the bevel controller node and the bevel node
	// first, set up the attribute map
	CCAttrMap *pMap = CCAttrMap::MakeAppliedAttrMap((NodeRenderableInk *)pControl);

	CCAttrMap::iterator	pos = pMap->GetStartPosition();
	CCAttrMap::iterator	end = pMap->GetEndPosition();

	NodeAttribute	   *pAttrNode = NULL;
	NodeListItem	   *pItem = NULL;
	
	// run through the map, creating copies of the attributes and adding them to the list
	while( pos != end )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		pMap->GetNextAssoc(pos, pKey, pVal);

		pAttrNode = ((NodeAttribute *)pVal);

		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Couldn't create list item");
			return;
		}

		pAttrNode->NodeCopy(&(pItem->pNode));

		m_ControllerAttributeList.AddTail(pItem);
	}

	delete pMap;
	pMap = NULL;

	// now, get the attribute maps for the bevel node
	pMap = CCAttrMap::MakeAppliedAttrMap((NodeRenderableInk *)pBevel);

	pos = pMap->GetStartPosition();

	pAttrNode = NULL;
	
	// run through the map, creating copies of the attributes and adding them to the list
	while( pos != end )
	{
		CCRuntimeClass *pKey;
		void		   *pVal;
		pMap->GetNextAssoc( pos, pKey, pVal );

		pAttrNode = ((NodeAttribute *)pVal);

		pItem = new NodeListItem;

		if (!pItem)
		{
			ERROR3("Couldn't create list item");
			return;
		}

		pAttrNode->NodeCopy(&(pItem->pNode));

		m_BevelNodeAttributeList.AddTail(pItem);
	}

	delete pMap;
	pMap = NULL;

}

/********************************************************************************************

>	BevelNodeTreeFactory::~BevelNodeTreeFactory()

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/8/99
	Inputs:		
	Purpose:	Destructor
	SeeAlso:	

********************************************************************************************/
BevelNodeTreeFactory::~BevelNodeTreeFactory()
{
	NodeListItem * pItem = (NodeListItem *)m_ControllerAttributeList.GetHead();

	while (pItem)
	{
		delete pItem->pNode;
		pItem->pNode = NULL;

		pItem = (NodeListItem *)m_ControllerAttributeList.GetNext(pItem);
	}

	m_ControllerAttributeList.DeleteAll();

	pItem = (NodeListItem *)m_BevelNodeAttributeList.GetHead();

	while (pItem)
	{
		delete pItem->pNode;
		pItem->pNode = NULL;

		pItem = (NodeListItem *)m_BevelNodeAttributeList.GetNext(pItem);
	}
}

/********************************************************************************************

>	NodeCompound *BevelNodeTreeFactory::CreateNode(List *pList, UndoableOperation * pOp = NULL)

	Author:		David_McClarnon (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/8/99
	Inputs:		The list of nodes to bevels, and an undoable operation (if necessary for 
				undo to work)
	Returns:	The compound node created (NULL for failure)
	Purpose:	Given the list of nodes, bevels them with my parameters

********************************************************************************************/
NodeCompound * BevelNodeTreeFactory::CreateNode(List *pList, UndoableOperation * pOp /* = NULL*/)
{
	if (!pList)
		return NULL;

	if (pList->IsEmpty())
		return NULL;

	if (pOp == NULL)
		return NULL;
	
	NodeListItem * pItem = (NodeListItem *)pList->GetHead();

	NodeListItem * pAttrItem = NULL;

	NodeBevelController * pControl = NULL;
	NodeBevel			* pBevel   = NULL;
	NodeBevelBegin		* pBevelBegin= NULL;

	BOOL ok = TRUE;

	if (pOp)
	{
		ALLOC_WITH_FAIL(pControl, new NodeBevelController, pOp);
		ALLOC_WITH_FAIL(pBevel, new NodeBevel, pOp);
		ALLOC_WITH_FAIL(pBevelBegin, new NodeBevelBegin, pOp);

		// assign all the attributes to this controller node
		pAttrItem = (NodeListItem *)m_ControllerAttributeList.GetHead();

		while (pAttrItem)
		{
			if (((NodeAttribute *)pAttrItem->pNode)->CanBeAppliedToObject())
			{
				pAttrItem->pNode->CopyNode(pControl, LASTCHILD);
			}

			pAttrItem = (NodeListItem *)m_ControllerAttributeList.GetNext(pAttrItem);
		}

		// insert the controller node first
		ok = pOp->DoInsertNewNode(pControl, pItem->pNode, NEXT, TRUE);

		// now, assign all the bevel node's attributes
		pAttrItem = (NodeListItem *)m_BevelNodeAttributeList.GetHead();

		while (pAttrItem)
		{
			if (((NodeAttribute *)pAttrItem->pNode)->CanBeAppliedToObject())
			{
				pAttrItem->pNode->CopyNode(pBevel, LASTCHILD);
			}

			pAttrItem = (NodeListItem *)m_BevelNodeAttributeList.GetNext(pAttrItem);
		}

		// insert the bevel begin node next
		if (ok)
			ok = pOp->DoInsertNewNode(pBevelBegin, pControl, LASTCHILD, TRUE);		

		// move all nodes to being children of this node
		while (pItem && ok)
		{
			ok = pOp->DoMoveNode(pItem->pNode, pControl, LASTCHILD);

			pItem = (NodeListItem *)pList->GetNext(pItem);
		}

		// now, insert the bevel begin & end nodes
		if (ok)
			ok = pOp->DoInsertNewNode(pBevel, pControl, LASTCHILD, TRUE);

		if (ok)
		{
			ok = pOp->DoInvalidateNodeRegion(pControl, TRUE);
		}

		// now, remove & re-apply all the bevel attributes
		if (ok)
			ok = pOp->DoRemoveAttrTypeFromSubtree(pControl, CC_RUNTIME_CLASS(AttrBevelIndent));

		if (ok)
			ok = pOp->DoRemoveAttrTypeFromSubtree(pControl, CC_RUNTIME_CLASS(AttrBevelLightAngle));

		if (ok)
			ok = pOp->DoRemoveAttrTypeFromSubtree(pControl, CC_RUNTIME_CLASS(AttrBevelContrast));

		if (ok)
			ok = pOp->DoRemoveAttrTypeFromSubtree(pControl, CC_RUNTIME_CLASS(AttrBevelType));

		pBevel->m_BevelType = m_BevelType;
		pBevel->m_LightAngle = m_BevelLightAngle;
		pBevel->m_Contrast = m_BevelContrast;
		pBevel->m_Indent   = abs(m_BevelIndent);

		pControl->m_BevelType = m_BevelType;
		pControl->m_LightAngle = m_BevelLightAngle;
		pControl->m_Contrast = m_BevelContrast;
		pControl->m_Indent   = abs(m_BevelIndent);

		if (m_BevelIndent < 0)
		{
			pBevel->m_bOuter = TRUE;
			pControl->m_bOuter = TRUE;
		}
		else
		{
			pBevel->m_bOuter = FALSE;
			pControl->m_bOuter = FALSE;
		}

		// create the new attribute nodes
		AttrBevelIndent * pIndent = NULL;
		ALLOC_WITH_FAIL(pIndent, new AttrBevelIndent, pOp);
		pIndent->Value.m_Indent = m_BevelIndent;

		AttrBevelLightAngle * pLightAngle = NULL;
		ALLOC_WITH_FAIL(pLightAngle, new AttrBevelLightAngle, pOp);
		pLightAngle->Value.m_LightAngle = m_BevelLightAngle;

		AttrBevelContrast * pContrast = NULL;
		ALLOC_WITH_FAIL(pContrast, new AttrBevelContrast, pOp);
		pContrast->Value.m_Contrast = m_BevelContrast;

		AttrBevelType * pType = NULL;
		ALLOC_WITH_FAIL(pType, new AttrBevelType, pOp);
		pType->Value.m_Type = m_BevelType;

		// attach all attribute nodes to the controller node
		pContrast->AttachNode(pControl, FIRSTCHILD);
		pType->AttachNode(pControl, FIRSTCHILD);
		pLightAngle->AttachNode(pControl, FIRSTCHILD);
		pIndent->AttachNode(pControl, FIRSTCHILD);

		if (ok)
			ok = pOp->DoFactorOutCommonChildAttributes(pControl);

		if (ok)
			pControl->RegenerateNode(NULL, FALSE);

		if (ok)
			ok = pOp->DoInvalidateNodeRegion(pControl, TRUE);

	}

	// that's it !
	if (!ok)
		return NULL;

	return pControl;
}

#endif
