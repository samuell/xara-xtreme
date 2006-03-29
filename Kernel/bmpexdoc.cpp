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

#include "camtypes.h"

#include "bmpexdoc.h"
#include "layer.h"
#include "spread.h"
//#include "will2.h"		// For layer name
#include "nodebmp.h"
#include "bitmpinf.h"
#include "fillval.h"

#include "lineattr.h"	// AttrStrokeColour
#include "fillattr.h"	// AttrFillGeometry

DECLARE_SOURCE("$Revision: 662 $");

CC_IMPLEMENT_DYNAMIC(BitmapExportDocument, Document)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	BitmapExportDocument::BitmapExportDocument(BOOL fHide = TRUE)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		fHide - whether this document is invisible or not (default is invisible)
	Purpose:	BitmapExportDocument constructor.

********************************************************************************************/

BitmapExportDocument::BitmapExportDocument(BOOL fHide)
  : Document(fHide),
	pBitmapNode(0)
{
	// Empty.
}



/********************************************************************************************

>	BOOL BitmapExportDocument::Init(KernelBitmap* pBitmap, const DocRect& RectToExport)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/6/96
	Inputs:		pBitmap - pointer to the kernel bitmap to be inserted into the document
							(can't be const 'cos KernelBitmapRef::SetBitmap isn't)
				RectToExport - DocRect reference of the rectangle to export	
	Returns:	FALSE if it failed
	Purpose:	This method initialises the document
				It calls down to Document::Init, then adds an insertion Layer node to the
				bottom of the basic doc tree.
				It then adds a totally transparent NodeRectangle and a NodeBitmap with the
				default NodeBitmap attributes referencing the specified KernelBitmap
	SeeAlso:	Document::Init; Document::InitTree

********************************************************************************************/

BOOL BitmapExportDocument::Init(KernelBitmap* pBitmap, const DocRect& RectToExport)
{
	// Don't attach any CCamDoc.
	if (!Document::Init(0)) return(FALSE);

	Node *pSpread = FindFirstSpread();
	ERROR3IF(pSpread == NULL, "No Spread in document!");

	Node *pLayer = pSpread->FindFirstChild();
	ERROR3IF(pLayer == NULL, "No Spread-child in document!");

	// Store away the rectangle
	ExportRect = RectToExport;

	// Now scan the children of the first spread until we find a layer, or run out of nodes
	while (pLayer != NULL && !pLayer->IsLayer())
		pLayer = pLayer->FindNext();

	if (pLayer == NULL)		// No Layer, so we'd better add one for ourselves
	{
		String_256 LayerID = String_256(_R(IDS_K_CLIPINT_LAYERNAME));
		pLayer = new Layer(pSpread, LASTCHILD, LayerID);
		if (pLayer == NULL)
			return(InitFailed());
	}

	// Create a new NodeRect
	NodeRect* pRectNode = new NodeRect(pLayer, FIRSTCHILD);

	// Failed so cleanup and exit
	if (pRectNode == NULL)
		return(InitFailed());

	// Initilaise the node
	if (!pRectNode->SetUpPath(6,6))
		return(InitFailed());

	// Create the rectangle
	pRectNode->CreateShape(ExportRect);

	// Give the rectangle a line colour
#if 0
	// This memory leaks a StrokeColourAttribute
	StrokeColourAttribute* pAttrValue = new StrokeColourAttribute(DocColour(COLOUR_TRANS));
	if (pAttrValue == NULL)
		return(InitFailed());
	
	NodeAttribute* pAttr = pAttrValue->MakeNode();
	if (pAttr == NULL)
		return(InitFailed());

	// Attach the attribute to the rectangle
	pAttr->AttachNode(pRectNode, FIRSTCHILD);
#else
	// Do what ApplyDefaultBitmapAttrs does
	Node* pLineColAttr = new AttrStrokeColour();
	if (pLineColAttr == NULL)
		return(InitFailed());

	((AttrFillGeometry*)pLineColAttr)->SetStartColour(&DocColour(COLOUR_NONE));
	pLineColAttr->AttachNode(pRectNode, FIRSTCHILD);
#endif

	// Create a NodeBitmap (don't attach it to the tree straight away 'cos we
	// have to put the attributes on it first)

	pBitmapNode = new NodeBitmap();
	if (pBitmapNode == NULL)
		return(InitFailed());

	if (!pBitmapNode->SetUpPath(6,6))
		return(InitFailed());

	pBitmapNode->CreateShape(ExportRect);

	if (!SetBitmap(pBitmap))
		return(InitFailed());

	// Set the bitmap's attributes
	// This must be done before the NodeBitmap is inserted into the tree
	if (!pBitmapNode->ApplyDefaultBitmapAttrs(NULL))
		return(InitFailed());

	// Attach it to the tree as the next sibling of the rectangle
	pBitmapNode->AttachNode(pRectNode, NEXT);

	// Success...
	return(TRUE);
}



/********************************************************************************************

>	BOOL BitmapExportDocument::SetBitmap(KernelBitmap* pBitmap)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		pBitmap - pointer to the kernel bitmap to be inserted into the document
							(can't be const 'cos KernelBitmapRef::SetBitmap isn't)
	Returns:	FALSE if it failed
	Purpose:	This function sets the bitmap method initialises the document
				It is an internal method, called by Initialise()
				It calls down to Document::Init, then adds an insertion Layer node to the
				bottom of the basic doc tree.
				It then adds a NodeBitmap referencing the specified KernelBitmap

********************************************************************************************/

BOOL BitmapExportDocument::SetBitmap(KernelBitmap* pBitmap)
{
	TRACEUSER( "Gerry", _T("BitmapExportDocument::SetBitmap()\n"));

	ERROR2IF(pBitmap == NULL, FALSE, "NULL bitmap passed to SetBitmap()\n");
	ERROR2IF(pBitmapNode == NULL, FALSE, "pBitmapNode is NULL in SetBitmap()\n");

	// Attach the bitmap to our node
	//
	// Note: Neville 18/9/97 
	// If the bitmap is a lone bitmap i.e. not attached to any document, then we
	// must attach it properly to this document so that it gets cleaned out
	// rather than memory leaking. If not, then we are just temporarily using
	// the bitmap and so everything should be ok. We need to work in the following
	// situations:-
	// - Selecting multiple bitmaps in the bitmap gallery and saving as animated GIF.
	//	 Here we are passed a list of bitmaps which are already in use in the source
	//   document.
	// - Making a new clipart index for bitmaps. Here we are passed a brand spanking
	//   new bitmap which is not on any lists. This used to memory leak.
	BitmapList* pBmpList = pBitmap->GetParentBitmapList();
	if (pBmpList != NULL)
	{
		// This will memory leak a kernel bitmap each time if it is not on a parent
		// document list. You will also end up with a KernelBitmap which contains
		// a deleted ActualBitmap pointer, which is very bad!
		pBitmapNode->GetBitmapRef()->SetBitmap(pBitmap);
	}
	else
	{
		// This is the same technique that the BaseBitmapFilter uses and so must be correct!
		pBitmapNode->GetBitmapRef()->Attach(pBitmap, this);
		if (pBitmapNode->GetBitmap() != pBitmap)
		{
			// It didn't use the bitmap we gave it, so we can delete it
			delete pBitmap;
		}
	}
	
	BitmapInfo Info;
	pBitmapNode->GetBitmap()->ActualBitmap->GetInfo(&Info);

	// Calculate the rectangle for this bitmap
	// We make it as large as possible (within the ExportRect)
	// without changing the aspect ration

	INT32 RectWidth = ExportRect.Width();
	INT32 RectHeight = ExportRect.Height();

	double xScale = (double) RectWidth / (double) Info.RecommendedWidth;
	double yScale = (double) RectHeight / (double) Info.RecommendedHeight;

	DocRect BitmapRect;

	BitmapRect.lo.x = 0;
	BitmapRect.lo.y = 0;

	if (xScale < yScale)
	{
		// Bitmap will be full width
		BitmapRect.hi.x = RectWidth;
		BitmapRect.hi.y = (INT32) ((double) Info.RecommendedHeight * xScale);

		// Center the bitmap vertically in the rectangle
		BitmapRect.Translate(0, (RectHeight - BitmapRect.Height()) / 2);
	}
	else
	{
		// Bitmap will be full height
		BitmapRect.hi.x = (INT32) ((double) Info.RecommendedWidth * yScale);
		BitmapRect.hi.y = RectHeight;

		// Center the bitmap horizontally in the rectangle
		BitmapRect.Translate((RectWidth - BitmapRect.Width()) / 2, 0);
	}

	// Delete the node's path
	// Because CreateShape inserts a rectangle at the beginning of the path
	pBitmapNode->InkPath.DeleteFromElement(0);

	// And create the shape
	pBitmapNode->CreateShape(BitmapRect);

	return(TRUE);
}



/********************************************************************************************

>	BOOL BitmapExportDocument::InitFailed(void)

	Author:		Gerry_Iles (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/6/96
	Inputs:		-
	Returns:	See Document::InitFailed()
	Purpose:	This function deletes the NodeBitmap and then calls Document::InitFailed()
	SeeAlso:	Document::InitFailed

********************************************************************************************/

BOOL BitmapExportDocument::InitFailed(void)
{
	if (pBitmapNode)
	{
		delete pBitmapNode;
		pBitmapNode = NULL;
	}

	return(Document::InitFailed());
}
