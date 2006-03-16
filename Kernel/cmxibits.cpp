// $Id: cmxibits.cpp 662 2006-03-14 21:31:49Z alex $
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
// bits for CMX import...

#include "camtypes.h"
#include "layer.h"
#include "group.h"
#include "colmodel.h"
#include "combshps.h"
#include "app.h"
#include "fontman.h"

#include "cmxform.h"
#include "cmxifltr.h"
#include "cmxidata.h"
#include "cmxibits.h"
#include "cmxistut.h"

CC_IMPLEMENT_DYNAMIC(CMXImportLayer, ListItem);
CC_IMPLEMENT_DYNAMIC(CMXImportStack, List);
CC_IMPLEMENT_DYNAMIC(CMXImportStackableItem, ListItem);
CC_IMPLEMENT_DYNAMIC(CMXImportTransform, CMXImportStackableItem);
CC_IMPLEMENT_DYNAMIC(CMXImportClipper, CMXImportStackableItem);
CC_IMPLEMENT_DYNAMIC(CMXImportColourList, List);
CC_IMPLEMENT_DYNAMIC(CMXImportColourListItem, ListItem);

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	CMXImportLayer::CMXImportLayer()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		none
	Returns:	none
	Purpose:	constructor

********************************************************************************************/

CMXImportLayer::CMXImportLayer()
{
	// set up things to a nice initial state
	pSubTree = NULL;
	pLastNodeAdded = NULL;
	pGroup = NULL;

	GroupLevel = 0;

	Used = FALSE;
}


/********************************************************************************************

>	CMXImportLayer::~CMXImportLayer()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		none
	Returns:	none
	Purpose:	destructor -- will delete all objects enclosed by it if they aren't marked
				as used.

********************************************************************************************/

CMXImportLayer::~CMXImportLayer()
{
	if(!Used)
	{
		// stuff wasn't used... delete it
		Node *This = pSubTree, *Next;

		while(This != 0)
		{
			Next = This->FindNext();

			This->CascadeDelete();

			delete This;

			This = Next;
		}
	}
}


/********************************************************************************************

>	BOOL CMXImportLayer::AddNode(Node *pNode)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		node to add
	Returns:	success flag thingy
	Purpose:	adds a node to the subtree for the layer

********************************************************************************************/

BOOL CMXImportLayer::AddNode(Node *pNode)
{
	// pNode can be NULL, just in case it's the result of a clipping operation
	// which actually resulted in nothing
	if(pNode == NULL)
		return TRUE;

	// right then, where do we want to put this thing?
	if(pGroup != 0)
	{
		// bung it on the end of this group
		pNode->AttachNode(pGroup, LASTCHILD, FALSE);		// don't check for transparency
	}
	else
	{
		// have we added something before?
		if(pSubTree == NULL)
		{
			// nope
			pSubTree = pNode;
		}
		else
		{
			// bung it on the end of the subtree list
			Node *pTarget = pSubTree;
			while(pTarget->FindNext() != 0)
				pTarget = pTarget->FindNext();

			// pTarget is the last node in the top level of the subtree
			pNode->AttachNode(pTarget, NEXT, FALSE);		// don't check transparency
		}
	}

	pLastNodeAdded = pNode;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportLayer::StartGroup(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		none
	Returns:	success flag thingy
	Purpose:	starts a group in the layer

********************************************************************************************/

BOOL CMXImportLayer::StartGroup(void)
{
	// create a new group node
	NodeGroup *pNewGroup = new NodeGroup;
	if(pNewGroup == NULL)
		return FALSE;

	// add it to the tree (but don't let it modify the last node thingy)
	Node *pTheLastNode = pLastNodeAdded;
	AddNode(pNewGroup);
	pLastNodeAdded = pTheLastNode;

	// put group level up a bit
	GroupLevel++;

	// point the thingy to this group node
	pGroup = pNewGroup;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportLayer::EndGroup(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		none
	Returns:	success flag thingy
	Purpose:	ends a group in the layer

********************************************************************************************/

BOOL CMXImportLayer::EndGroup(void)
{
	if(GroupLevel < 1)		// no start group...
		CMXFORMATERROR(FALSE)

	// now, we need to wander up a level
	if(GroupLevel == 1)
	{
		// this is a top level node -- just set the group pointer to zero and we're done
		pGroup = NULL;
	}
	else
	{
		// OK, this isn't a top level one -- we need to find it's parent group... easy!
		Node *pParent = pGroup->FindParent();

		// check it's what we expect
		ERROR2IF(!IS_A(pParent, NodeGroup), FALSE, "Parent of the group isn't a group, and it jolly well should be. <sulk>");
		
		// set the new group pointer as the new group
		pGroup = pParent;
	}

	// make a note of our nice group level thing
	GroupLevel--;

	return TRUE;
}


/********************************************************************************************

>	Layer *CMXImportLayer::UseSubTreeAsLayer(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/96
	Inputs:		none
	Returns:	layer node or NULL
	Purpose:	creates a layer node, bungs the subtree under it, and returns it,
				marking the subtree as used

********************************************************************************************/

Layer *CMXImportLayer::UseSubTreeAsLayer(void)
{
	Layer *pLayer = new Layer;
	if(pLayer == NULL)
		return NULL;

	// set up some stuff
	pLayer->SetLayerID(LayerName);
	pLayer->SetVisible(TRUE);
	pLayer->SetLocked(FALSE);
	pLayer->SetActive(FALSE);
	pLayer->SetPrintable(TRUE);
	pLayer->SetBackground(FALSE);
	pLayer->SetOutline(FALSE);

	// stick the objects under it, if we have any (blank layers are allowed)
	if(pSubTree != 0)
		pSubTree->InsertChainSimple(pLayer, FIRSTCHILD);

	// mark as used -- we don't want to go deleting things which are
	// in the tree, do we?
	Used = TRUE;

	return pLayer;
}



/********************************************************************************************

>	CMXImportStack::~CMXImportStack()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	destructor for the stack

********************************************************************************************/

CMXImportStack::~CMXImportStack()
{
#ifdef _DEBUG
	if(GetCount() != 0)
		TRACEUSER( "Ben", _T("\n\n****************** cmx stack not empty when deleted\n\n\n"));
#endif

	// get rid of anything left on the stack
	DeleteAll();
}



/********************************************************************************************

>	BOOL CMXImportStack::Push(CMXImportStackableItem *pItem)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	push an item onto the stack. It will be activated. It must be the
				right kind of item for this stack, by the way.

********************************************************************************************/

BOOL CMXImportStack::Push(CMXImportStackableItem *pItem)
{
	ERROR2IF(pItem == NULL, FALSE, "no item");

	// get the last item on at the moment
	CMXImportStackableItem *pPreviousTail = (CMXImportStackableItem *)GetTail();

	// put the item on the stack
	AddTail(pItem);

	// tell the item it's been added
	if(!pItem->PlacedOnStack(this))
		return FALSE;

	// deactivate the previous item, if we have one
	if(pPreviousTail != 0)
		pPreviousTail->Deactivate(this, FALSE);

	// activate this item
	return pItem->Activate(this);
}


/********************************************************************************************

>	CMXImportStackableItem *CMXImportStack::Pop(void)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	pop an item from the stack. It will be deactivated, and the previous one
				activated.

********************************************************************************************/

CMXImportStackableItem *CMXImportStack::Pop(void)
{
	// check we've got something
	if(GetCount() == 0)
		return 0;

	// right then, we need to get the last item by removing from the stack
	CMXImportStackableItem *pPopped = (CMXImportStackableItem *)RemoveTail();
	ERROR2IF(pPopped == NULL, 0, "oh no, not item here when the list said there was");

	// deactivate it
	pPopped->Deactivate(this, (GetCount() != 0)?FALSE:TRUE);

	// activate anything which is left
	CMXImportStackableItem *pLast = (CMXImportStackableItem *)GetTail();
	if(pLast != 0)
		pLast->Activate(this);

	// return the popped item for the caller to use or dispose of
	return pPopped;
}



/********************************************************************************************

>	BOOL CMXImportTransform::PlacedOnStack(CMXImportStack *pStack)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	called when the transform is placed on the stack. It will wander through
				the stack creating it's transform matrices and generally set itself up niecly.

********************************************************************************************/

BOOL CMXImportTransform::PlacedOnStack(CMXImportStack *pStack)
{
	CMXImportFilter *pFilter = pStack->GetFilter();
	ERROR2IF(pFilter == NULL, FALSE, "no filter");

	CMXImportTransform *pTrans = (CMXImportTransform *)pStack->GetHead();
	Matrix Sarah;		// identity matrix

	// run through the previous entries doing fancy stuff
	// note that we're on the stack at this point so we include
	// ourselves automagically
	while(pTrans != 0)
	{
		// what we do with this thing depends on whether the thingy is
		// concatenated to the previous ones or not
		if(pTrans->Concatenate)
		{
			// transform our compound thing by the one in the object
			Sarah *= pTrans->TheMatrix;
		}
		else
		{
			// just make our current compound one the one in the object
			Sarah = pTrans->TheMatrix;
		}

		// get the next item
		pTrans = (CMXImportTransform *)pStack->GetNext(pTrans);
	}

	// transform it by the filter's base matrix
	Matrix *pBase = pFilter->GetBaseMatrix();
	ERROR2IF(pBase == NULL, FALSE, "no base matrix");
	Sarah *= (*pBase);

	// set the compound matrix here ready for activation
	CompoundMatrix = Sarah;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportTransform::Activate(CMXImportStack *pStack)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	called to set this transform as the current transform

********************************************************************************************/

BOOL CMXImportTransform::Activate(CMXImportStack *pStack)
{
	CMXImportFilter *pFilter = pStack->GetFilter();
	ERROR2IF(pFilter == NULL, FALSE, "no filter");

	// set the matrix in the file to the compound of the previous transforms
	// and the base matrix
	pFilter->SetMatrix(&CompoundMatrix);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportTransform::Deactivate(CMXImportStack *pStack, BOOL IsLastItemOnStack)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	called to stop this being the active transform. It sets the matrix back
				to being the base transform thingy.

********************************************************************************************/

BOOL CMXImportTransform::Deactivate(CMXImportStack *pStack, BOOL IsLastItemOnStack)
{
	CMXImportFilter *pFilter = pStack->GetFilter();
	ERROR2IF(pFilter == NULL, FALSE, "no filter");

	// set the matrix to the base matrix -- this will either be replaced by the
	// previous matrix in the stack, or stay as the base matrix which is just
	// what we wanted anyway.
	pFilter->SetMatrix(pFilter->GetBaseMatrix());

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportClipper::PlacedOnStack(CMXImportStack *pStack)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		none
	Returns:	none
	Purpose:	called when the clipper is placed on the stack. It will intersect itself
				with the previous item if it's required to do so.

********************************************************************************************/

BOOL CMXImportClipper::PlacedOnStack(CMXImportStack *pStack)
{
	ERROR2IF(pClipPath == NULL, FALSE, "no clip path");
	CMXImportFilter *pFilter = pStack->GetFilter();
	ERROR2IF(pFilter == NULL, FALSE, "no filter");

	// first, transform the happy chappy with the currently active matrix
	Matrix *pSarah = pFilter->GetCurrentMatrix();
	ERROR2IF(pSarah == NULL, FALSE, "no matrix to transform with");
	pSarah->transform(pClipPath->GetCoordArray(), pClipPath->GetNumCoords());

	if(Intersect)
	{
		// find previous item
		CMXImportClipper *pLastClip = (CMXImportClipper *)pStack->GetPrev(this);

		if(pLastClip != 0)		// there was actually a previous path to clip
		{
			// right then, we need to create one of those things
			Path *pClippedClipPath = new Path;
			if(pClippedClipPath == NULL || !pClippedClipPath->Initialise())
				return FALSE;	// something went wrong getting a pathything

			// clip this new path to the previous item
			ERROR2IF(pLastClip->pClipPath == NULL, FALSE, "no clip path in previous thing");
			if(!pLastClip->pClipPath->ClipPathToPath(*pClipPath, pClippedClipPath, CLIP_STYLE_INTERSECT))
				return FALSE;

			// set the clipped path to this new one, deleteing the old one
			delete pClipPath;
			pClipPath = pClippedClipPath;
		}
	}

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportClipper::Activate(CMXImportStack *pStack)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		none
	Returns:	none
	Purpose:	called to set this clipper as the current transform

********************************************************************************************/

BOOL CMXImportClipper::Activate(CMXImportStack *pStack)
{
	CMXImportFilter *pFilter = pStack->GetFilter();
	ERROR2IF(pFilter == NULL, FALSE, "no filter");

	// set the clipping path in the thingy
	pFilter->SetClippingPath(pClipPath);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportClipper::Deactivate(CMXImportStack *pStack, BOOL IsLastItemOnStack)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		none
	Returns:	none
	Purpose:	called to stop this being the active clipper. It sets the clipping
				path to nothing

********************************************************************************************/

BOOL CMXImportClipper::Deactivate(CMXImportStack *pStack, BOOL IsLastItemOnStack)
{
	CMXImportFilter *pFilter = pStack->GetFilter();
	ERROR2IF(pFilter == NULL, FALSE, "no filter");

	// set to no clipping
	pFilter->SetClippingPath();

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportColour::Convert(cmxiColour *pCol)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts a colour from the data supplied by the file to a nice indexed
				colour we can use in our document

********************************************************************************************/

#define CVAL(n, max) (FIXED24)(((double)n) / ((double)max))

BOOL CMXImportColour::Convert(/*cmxiColour*/ void *pvCol)
{
	// to get around compiler difficulties (see header file)
	cmxiColour *pCol = (cmxiColour *)pvCol;

	// OK, get some stuff...
	ColourModel Model;
	ColourGeneric ColDef;
	ColourCMYK *pCMYK = (ColourCMYK *)&ColDef;
	ColourRGBT *pRGBT = (ColourRGBT *)&ColDef;
	ColourHSVT *pHSVT = (ColourHSVT *)&ColDef;
	ColourGreyT *pGreyT = (ColourGreyT *)&ColDef;

	switch(pCol->ColourModel)
	{
	case cmxCOLMODEL_CMYK:
		Model = COLOURMODEL_CMYK;
		pCMYK->Cyan =		CVAL(pCol->Spec.CMYK.Cyan, 100);
		pCMYK->Magenta =	CVAL(pCol->Spec.CMYK.Magenta, 100);
		pCMYK->Yellow =		CVAL(pCol->Spec.CMYK.Yellow, 100);
		pCMYK->Key =		CVAL(pCol->Spec.CMYK.Key, 100);
		break;

	case cmxCOLMODEL_CMYK255:
		Model = COLOURMODEL_CMYK;
		pCMYK->Cyan =		CVAL(pCol->Spec.CMYK.Cyan, 255);
		pCMYK->Magenta =	CVAL(pCol->Spec.CMYK.Magenta, 255);
		pCMYK->Yellow =		CVAL(pCol->Spec.CMYK.Yellow, 255);
		pCMYK->Key =		CVAL(pCol->Spec.CMYK.Key, 255);
		break;

	case cmxCOLMODEL_CMY:
		Model = COLOURMODEL_CMYK;
		pCMYK->Cyan =		CVAL(pCol->Spec.CMYK.Cyan, 100);
		pCMYK->Magenta =	CVAL(pCol->Spec.CMYK.Magenta, 100);
		pCMYK->Yellow =		CVAL(pCol->Spec.CMYK.Yellow, 100);
		pCMYK->Key =		0;
		break;

	case cmxCOLMODEL_RGB:
		Model = COLOURMODEL_RGBT;
		pRGBT->Red =		CVAL(pCol->Spec.RGB.Red, 255);
		pRGBT->Green =		CVAL(pCol->Spec.RGB.Green, 255);
		pRGBT->Blue =		CVAL(pCol->Spec.RGB.Blue, 255);
		pRGBT->Transparent = 0;
		break;

	case cmxCOLMODEL_HSB:
	case cmxCOLMODEL_HLS:
		Model = COLOURMODEL_HSVT;
		pHSVT->Hue =		CVAL(pCol->Spec.HSV.Hue, 360);
		pHSVT->Saturation =	CVAL(pCol->Spec.HSV.Saturation, 255);
		pHSVT->Value =		CVAL(pCol->Spec.HSV.Value, 255);
		pHSVT->Transparent = 0;
		break;

	case cmxCOLMODEL_GREY:
		Model = COLOURMODEL_GREYT;
		pGreyT->Intensity = CVAL(pCol->Spec.Grey.Grey, 255);
		pGreyT->Reserved1 = 0;
		pGreyT->Reserved2 = 0;
		pGreyT->Transparent = 0;
		break;

	default:
		TRACEUSER( "Ben", _T("unknown colour type encountered"));
		// don't know about this -- make up black and use that instead
		Model = COLOURMODEL_CMYK;
		pCMYK->Cyan = 1;
		pCMYK->Magenta = 1;
		pCMYK->Yellow = 1;
		pCMYK->Key = 1;
		break;
	}

	// make an index colour, and a DocColour which refers to it
	pIndexedColour = new IndexedColour(Model, &ColDef);
	pColour = new DocColour;
	if(pIndexedColour == NULL || pColour == NULL)
		return FALSE;

	pColour->MakeRefToIndexedColour(pIndexedColour);

	return TRUE;
}



/********************************************************************************************

>	BOOL CMXImportOutline::Convert(cmxiOutline *pOut, CMXImportFilterDataSet *Data)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts an outline description to a thingy we can use

********************************************************************************************/

BOOL CMXImportOutline::Convert(/*cmxiOutline*/ void *pvOut, CMXImportFilterDataSet *Data)
{
	// to get around compiler difficulties (see header file)
	cmxiOutline *pOut = (cmxiOutline *)pvOut;
	
	// do the colour ref
	ColourReference = pOut->ColourRef;

	// get the line width
	if(pOut->PenRef <= 0 || pOut->PenRef > Data->NumPens)
		CMXFORMATERROR(FALSE)			// pen ref out of range

	Width = Data->Pens[pOut->PenRef - 1].Width;

	// get the line style
	if(pOut->LineStyleRef <= 0 || pOut->LineStyleRef > Data->NumLineStyles)
		CMXFORMATERROR(FALSE)			// line style ref out of range

	Join = Data->LineStyles[pOut->LineStyleRef - 1].Join;
	Cap = Data->LineStyles[pOut->LineStyleRef - 1].Cap;
	NoStroke = Data->LineStyles[pOut->LineStyleRef - 1].NoStroke;

	// think about dot dash patterns for a while.
	//
	//
	//
	// nope, they're not very interesting, are they?
	//
	// anyway, I suppose we better see if this outline style has one
	// of the silly things attached to it.
	pDots = NULL;
	if(Data->LineStyles[pOut->LineStyleRef - 1].DotDash)
	{
		// right then, a dot dash pattern is applied. Test this theory.
		if(pOut->DotDashRef > 0 && pOut->DotDashRef <= Data->NumDotDashes)
		{
			// ref valid. Check it out.
			if(Data->DotDashes[pOut->DotDashRef - 1].NDots >= 1)
				pDots = &Data->DotDashes[pOut->DotDashRef - 1];
		}
	}

	// what about arrowheads...?
	pArrowheads = NULL;
	if(pOut->ArrowheadsRef > 0 && pOut->ArrowheadsRef <= Data->NumArrowheads)
	{
		pArrowheads = &Data->Arrowheads[pOut->ArrowheadsRef - 1];
	}

	// that'll do for now

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportPen::Convert(cmxiPen *pPen)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts a pen description

********************************************************************************************/

BOOL CMXImportPen::Convert(/*cmxiPen*/ void *pvPen)
{
	// to get around compiler difficulties (see header file)
	cmxiPen *pPen = (cmxiPen *)pvPen;
	
	// store the width -- that's all we can use
	Width = pPen->Width;
	if(Width < 0) Width = 0 - Width;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportFont::Convert( void *pvFont)

	Author:		Claude_Quemerais (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts a font description

********************************************************************************************/

BOOL CMXImportFont::Convert(/*cmxiFont*/ void *pvFont)
{
	// to get around compiler difficulties (see header file)
	cmxiFontRecord *pFont = (cmxiFontRecord *)pvFont;

	mIsItalic	=	FALSE;
	mIsBold		=	FALSE;
	mFont		=	pFont->FontName;
	String_64 name((TCHAR*)*mFont);
	
	(GetApplication()->GetFontManager())->CacheNamedFont(&name, FC_TRUETYPE);

	if((pFont->FontStyle  == cmxTEXTATTR_THIN_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_EXTRA_LIGHT_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_LIGHT_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_NORMAL_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_MEDIUM_ITALIC)	||
		(pFont->FontStyle == cmxTEXTATTR_SEMI_BOLD_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_BOLD_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_EXTRA_BOLD_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_BLACK_ITALIC))
			mIsItalic = TRUE;
	
	if((pFont->FontStyle  == cmxTEXTATTR_BOLD) ||
		(pFont->FontStyle == cmxTEXTATTR_EXTRA_BOLD) ||
		(pFont->FontStyle == cmxTEXTATTR_SEMI_BOLD) ||
		(pFont->FontStyle == cmxTEXTATTR_SEMI_BOLD_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_BOLD_ITALIC) ||
		(pFont->FontStyle == cmxTEXTATTR_EXTRA_BOLD_ITALIC))
			mIsBold = TRUE;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportPen::Convert(cmxiPen *pPen)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts a line style description

********************************************************************************************/

BOOL CMXImportLineStyle::Convert(/*cmxiLineStyle*/ void *pvStyle)
{
	// to get around compiler difficulties (see header file)
	cmxiLineStyle *pStyle = (cmxiLineStyle *)pvStyle;

	// do spec thingy
	if((pStyle->Spec & cmxLINESPEC_DOTDASH) != 0)
		DotDash = TRUE;
	else
		DotDash = FALSE;

	// do cap and join types
	switch(pStyle->CapAndJoin & cmxCAPJOIN_JOINMASK)
	{
	case cmxCAPJOIN_ROUNDJOIN: Join = RoundJoin; break;
	case cmxCAPJOIN_BEVELJOIN: Join = BevelledJoin; break;
	default: Join = MitreJoin; break;
	}
	switch(pStyle->CapAndJoin & cmxCAPJOIN_CAPMASK)
	{
	case cmxCAPJOIN_ROUNDCAP: Cap = LineCapRound; break;
	case cmxCAPJOIN_SQUARECAP: Cap = LineCapSquare; break;
	default: Cap = LineCapButt; break;
	}

	NoStroke = FALSE;
	if((pStyle->Spec & cmxLINESPEC_NONE) != 0)
		NoStroke = TRUE;


	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportDotDash::Convert(cmxiDotDash *pvDotDash)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts a dot dash description

********************************************************************************************/

BOOL CMXImportDotDash::Convert(/*cmxiDotDash*/ void *pvDotDash)
{
	// to get around compiler difficulties (see header file)
	cmxiDotDash *pDotDash = (cmxiDotDash *)pvDotDash;

	// copy in the dots INT32 array
	NDots = pDotDash->Dots.NumElements;
	pElements = (DashElement *)pDotDash->Dots.pElements;

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportArrowShape::Convert(cmxiArrowShape *pvArrowShape, BOOL Is32Bit)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts an arrow shape description

********************************************************************************************/

BOOL CMXImportArrowShape::Convert(/*cmxiArrowShape*/ void *pvArrowShape, BOOL Is32Bit)
{
	// to get around compiler difficulties (see header file)
	cmxiArrowShape *pArrowShape = (cmxiArrowShape *)pvArrowShape;

	// set up the arrow rec thingy
	Arrow.ArrowShape = pArrowShape->pShape;
	Arrow.LineWidth = (Is32Bit)?(360000):(1400);
	Arrow.IsNull = FALSE;

	// record the line offset
	LineOffset = pArrowShape->LineOffset;
	TRACEUSER( "Ben", _T("LineOffset = %d\n"), LineOffset);

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportArrowShape::Convert(cmxiArrowheads *pvArrowShape, CMXImportFilterDataSet *Data)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/08/96
	Inputs:		pointer to the raw data structure returned by the read data routine
	Returns:	error flag
	Purpose:	converts an arrow shape description

********************************************************************************************/

BOOL CMXImportArrowheads::Convert(/*cmxiArrowheads*/ void *pvArrowheads, CMXImportFilterDataSet *Data)
{
	// to get around compiler difficulties (see header file)
	cmxiArrowheads *pArrowheads = (cmxiArrowheads *)pvArrowheads;

	// record the start and end thingsy
	if(pArrowheads->StartRef > 0 && pArrowheads->StartRef <= Data->NumArrowShapes)
		pStart = &Data->ArrowShapes[pArrowheads->StartRef - 1];

	if(pArrowheads->EndRef > 0 && pArrowheads->EndRef <= Data->NumArrowShapes)
		pEnd = &Data->ArrowShapes[pArrowheads->EndRef - 1];

	return TRUE;
}


/********************************************************************************************

>	BOOL CMXImportColourList::AddColour(INT32 Reference, INT32 Position)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		reference of colour, position of colour
	Returns:	error flag
	Purpose:	adds a colour to the list

********************************************************************************************/

BOOL CMXImportColourList::AddColour(INT32 Reference, INT32 Position)
{
	// get new item
	CMXImportColourListItem *pNewItem = new CMXImportColourListItem;
	if(pNewItem == NULL)
		return FALSE;

	// set the data in the item
	pNewItem->Reference = Reference;
	pNewItem->Position = Position;

	// add it to the list
	AddTail(pNewItem);

	return TRUE;
}



/********************************************************************************************

>	INT32 CMXImportColourList::GetColourRefClosestToPosition(INT32 Position)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/08/96
	Inputs:		Position
	Returns:	reference of colour closest to this position
	Purpose:	finds the colour closest to a given position

********************************************************************************************/

INT32 CMXImportColourList::GetColourRefClosestToPosition(INT32 Position)
{
	// variable things
	INT32 Ref = 1;
	INT32 Distance = 10000;

	// run through the list
	CMXImportColourListItem *pEn = (CMXImportColourListItem *)GetHead();
	while(pEn != 0)
	{
		// see if this is closer
		INT32 d = ABS(pEn->Position - Position);
		if(d < Distance)
		{
			Ref = pEn->Reference;
			Distance = d;
		}

		// next!
		pEn = (CMXImportColourListItem *)GetNext(pEn);
	}

	return Ref;
}



/********************************************************************************************

>	BOOL CMXImportProcedure::Read(cmxiProcedureIndexEntry *pEn);

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/08/96
	Inputs:		entry of index
	Returns:	error flag
	Purpose:	converts an entry type thing

********************************************************************************************/

BOOL CMXImportProcedure::Read(/*cmxiProcedureIndexEntry*/ void *pvEn)
{
	cmxiProcedureIndexEntry *pEn = (cmxiProcedureIndexEntry *)pvEn;

	Position = pEn->Position;

	return TRUE;
}

