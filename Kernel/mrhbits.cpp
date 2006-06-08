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
// 

#include "camtypes.h"
#include "mrhbits.h"		// TODO
#include "view.h"
//#include "bitmap.h"
#include "fillattr.h"
#include "grndrgn.h"
#include "grndbmp.h"
//#include "docview.h"
//#include "grptrans.h"
//#include "ndcchbmp.h"

// Place any IMPLEMENT type statements here
CC_IMPLEMENT_MEMDUMP(CBMPBits, CC_CLASS_MEMDUMP)

// We want better memory tracking
#define new CAM_DEBUG_NEW

/********************************************************************************************

>	BOOL CBMPBits::RenderOILBMPToTransparentWhiteRect(BitmapFillAttribute* pRef, BOOL Transparent)
					
	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/99
	Inputs:		Pointer to an OILBitmap, Flag to say wether or not the Rectangle is to be transparent!
	Outputs:	None
	Returns:	TRUE if Everything ok!
	Purpose:	Front end function for RenderBMPFillAttrToTransparentWhiteRect(BitmapFillAttribute* pRef)
				used if the calling function does not contain a bitmapfillattribute!

********************************************************************************************/
OILBitmap* CBMPBits::RenderOILBMPToTransparentWhiteRect(OILBitmap* pConvBMP, BOOL Transparent, INT32 OutputBPP)
{
	// Check to see if we`ve been given a null bitmap pointer!
	ERROR2IF(pConvBMP == NULL,FALSE,"We`ve got a NULL Bitmap Pointer!! CBMPBits!");

	// Setup All the pointers that are going to be used
	KernelBitmap* pKernel = NULL;
	OILBitmap* pNewBMP = NULL;

	// Create a new KernelBitmap from the OILBitmap so we can create a new BitmapFillAttribute!
	pKernel = new KernelBitmap(pConvBMP);
	
	if(pKernel != NULL)
	{
		pNewBMP = RenderKernelBMPToTransparentWhiteRect(pKernel, Transparent, OutputBPP);

		delete pKernel;
		pKernel = NULL;
	}

	// Return the new Bitmap
	return pNewBMP;
}

/********************************************************************************************

>	BOOL CBMPBits::RenderKernelBMPToTransparentWhiteRect(BitmapFillAttribute* pRef)
					
	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/99
	Inputs:		Pointer to an KernelBitmap, Flag to say wether or not the Rectangle is to be transparent!
	Outputs:	None
	Returns:	TRUE if Everything ok!
	Purpose:	Front end function for RenderBMPFillAttrToTransparentWhiteRect(BitmapFillAttribute* pRef)
				used if the calling function does not contain a bitmapfillattribute!

********************************************************************************************/
OILBitmap* CBMPBits::RenderKernelBMPToTransparentWhiteRect(KernelBitmap* pConvBMP, BOOL Transparent, INT32 OutputBPP)
{
	// Check to see if we`ve been given a null bitmap pointer!
	ERROR2IF(pConvBMP == NULL,FALSE,"We`ve got a NULL Bitmap Pointer!! CBMPBits!");

	// Setup All the pointers that are going to be used
	OILBitmap* pNewBMP = NULL;
	BitmapFillAttribute* pRef = NULL;

	// Create the new BitmapFill Attribute
	pRef = new BitmapFillAttribute;

	if(pRef != NULL)
	{
		// make it reference the Passed in OILBitmap!
		pRef->AttachBitmap(pConvBMP);

		// Get the Width and Height of the Bitmap
		UINT32 Width = pConvBMP->GetWidth();
		UINT32 Height = pConvBMP->GetHeight();

		// Set the Fill Coordinates from the width and height variables
		DocCoord Start(0, 0);
		DocCoord End(Width * 750, 0);
		DocCoord End2(0, Height * 750);

		pRef->SetStartPoint(&Start);
		pRef->SetEndPoint(&End);
		pRef->SetEndPoint2(&End2);

		// Set the tessalation to repeating
		pRef->SetTesselation(2);

		// Now call the RenderBMPFillAttrToTransparentWhiteRect function with our new
		// BitmapFillAttribute!
		pNewBMP = RenderBMPFillAttrToTransparentWhiteRect(pRef,Transparent,OutputBPP);
	}

	// Delete everything that we created!
	if(pRef != NULL)
	{
		pRef->DetachBitmap();
		delete pRef;
		pRef = NULL;
	}

	// Return the new Bitmap
	return pNewBMP;
}

/********************************************************************************************

>	BOOL CBMPBits::RenderBMPFillAttrToTransparentWhiteRect(BitmapFillAttribute* pRef)
					
	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/7/99
	Inputs:		A Pointer to a Bitmap Reference, Flag to say wether or not the Rectangle is to be transparent!
	Outputs:	None
	Returns:	TRUE if Everything ok!
	Purpose:	Renders a Any Bitmap onto a 32bit Transparent White Rectangle of size
				equal to bitmap referenced. This is mainly used just before a 32bit alpha channeled
				bitmap is rendered into a lower dpi region. It stops the effect of rendering 
				solid shapes on a black background.

********************************************************************************************/
OILBitmap* CBMPBits::RenderBMPFillAttrToTransparentWhiteRect(BitmapFillAttribute* pRef, BOOL Transparent, INT32 OutputBPP)
{
	ERROR2IF(pRef == NULL, FALSE, "Where`s the Bitmap Reference Pointer? {DIBUtil}");

	// Ok, we`ve got a 32Bit BMP so lets setup a render region!
	View*	pView = View::GetCurrent();
	Matrix	Mat(1,0,0,1,0,0);
	FIXED16 Scale = 1;
	double	DPI = 96;
	DocRect RenderRect;
	OILBitmap* pNewBMP = NULL;

	// Get the info from the bitmap we`re trying to display in the gallery!
//	LPBITMAPINFOHEADER pInfo = (pRef->GetBitmap()->ActualBitmap)->GetBitmapInfoHeader();

	// Setup the DocRect with the Start and End points of the bitmap fill
	RenderRect.lo.x = pRef->GetStartPoint()->x;
	RenderRect.lo.y = pRef->GetStartPoint()->y;
	RenderRect.hi.x = pRef->GetEndPoint()->x;
	RenderRect.hi.y = pRef->GetEndPoint2()->y;

	// Create a new GRenderBitmap region
	GRenderBitmap* pGBMP = new GRenderBitmap(RenderRect, Mat, Scale, OutputBPP, DPI, FALSE, 0, NULL, TRUE);

	if(pGBMP != NULL && pView != NULL)
	{
		// Set the flag to say that we want to use the BiCompression Field for transparencies!
		pGBMP->m_DoCompression = TRUE;
		
		// Create a new path which is the same size as the bitmap which will fill it!
		Path BMPPath;
		BMPPath.Initialise();
		BMPPath.CreatePathFromDocRect(&RenderRect);

		// Create a new FlatFillAttr and set it to white
		FlatFillAttribute* pWhiteFill = new FlatFillAttribute;
		DocColour White(COLOUR_WHITE);
		pWhiteFill->SetStartColour(&White);

		// Create a new StrokeColourAttr and set that also to white
		StrokeColourAttribute* pStrokeColour = new StrokeColourAttribute;
		pStrokeColour->SetStartColour(&White);

		// Create a new stroketransattr and set it to fully transparent
		StrokeTranspAttribute* pStrokeTrans = NULL;
		FlatTranspFillAttribute* pTransFill = NULL;

		if(Transparent)
		{
			pStrokeTrans = new StrokeTranspAttribute(0xFF);
			pTransFill = new FlatTranspFillAttribute(0xFF);
		}
		else
		{
			pStrokeTrans = new StrokeTranspAttribute(0x00);
			pTransFill = new FlatTranspFillAttribute(0x00);
		}

		// Make sure everything got setup alright and then proceed!
		if( pWhiteFill != NULL && pStrokeColour != NULL &&
			pStrokeTrans != NULL && pTransFill != NULL)
		{
			if (!pGBMP->AttachDevice(pView, NULL, NULL))
			{
				ERROR3("Cannot attatch devices");
			}

			// Initialize the Render region
			pGBMP->InitDevice();

			// Save the current context
			pGBMP->SaveContext();

			// Start the render process
			pGBMP->StartRender();

			pGBMP->SetLineColour(COLOUR_NONE);
			pGBMP->SetLineWidth(0);

			// Set all the attributes we`ve just created
			pTransFill->Render(pGBMP);
			pStrokeTrans->Render(pGBMP);
			pWhiteFill->Render(pGBMP);
			pStrokeColour->Render(pGBMP);

			// Render a rectangle with the current attributes
			pGBMP->DrawRect(&RenderRect);

			// Now Render the Bitmap using no Flat Transparency, else it all comes out blank!			
			UINT32 NoTrans = 0x00;
			pTransFill->SetStartTransp(&NoTrans);
			pTransFill->Render(pGBMP);

			// Render the bitmap onto the white rectangle
			pGBMP->RenderBitmapFill(&BMPPath,pRef);

			// Finished so stop rendering
			pGBMP->StopRender();

			// Restore the old context
			pGBMP->RestoreContext();

			// WinBM Now needs to be setup with the newly created bitmap
			pNewBMP = OILBitmap::Attach(pGBMP->ExtractBitmapCopy(),TRUE);
		}
		else
		{
			ERROR3("Can not display 32bit Alpha channel bitmap!!!");
		}

		// Now delete everything!
		if(pGBMP != NULL)
		{
			delete pGBMP;
			pGBMP = NULL;
		}

		if(pStrokeColour != NULL)
		{
			delete pStrokeColour;
			pStrokeColour = NULL;
		}

		if(pStrokeTrans != NULL)
		{
			delete pStrokeTrans;
			pStrokeTrans = NULL;
		}

		if(pWhiteFill != NULL)
		{
			delete pWhiteFill;
			pWhiteFill = NULL;
		}

		if(pTransFill != NULL)
		{
			delete pTransFill;
			pTransFill = NULL;
		}
	}

	// Return the new Bitmap!
	return pNewBMP;
}

/********************************************************************************************

>	BOOL CBMPBits::RenderSelectionToBMP(SelRange* pSel, INT32 OutputBPP, BOOL Transparent)
					
	Author:		Mark_Howitt (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/8/99
	Inputs:		Flag to say wether or not the Rectangle is to be transparent and the required
				output depth. Pointer to the current selection of nodes.
	Returns:	The newly rendered BMP or NULL if something went bang!
	Purpose:	Renders any selection of nodes into a BMP copy.

********************************************************************************************/
OILBitmap* CBMPBits::RenderSelectionToBMP(SelRange* pSel, INT32 OutputBPP, BOOL Transparent, DocRect *pClip, double DPI)
{
	// Check to see if we have a valid selection pointer!
	ERROR2IF(pSel == NULL,NULL,"Recieved a NULL Selection Range Pointer in CBMPBits!");

	// Ok, Lets setup a few variables need to create the new BMP
	View*	pView = View::GetCurrent();
	Matrix	Mat(1,0,0,1,0,0);
	FIXED16 Scale = 1;
//	double	DPI = 96;
	OILBitmap* pNewBMP = NULL;

	// Get the current DocRect of the selection!
	// but use the passed in clip region if it is already calculated
	DocRect RenderRect = pClip ? *pClip : pSel->GetBoundingRect();

	// Check to see if we`ve got an empty rect if so then return NULL!
	ERROR2IF(RenderRect.IsEmpty(),NULL,"We`ve got an empty DocRect in the selection! (CBMPBits)");

	// Create a new GRenderBitmap region
	GRenderBitmap* pGBMP = new GRenderBitmap(RenderRect, Mat, Scale, OutputBPP, DPI, FALSE, 0, NULL, TRUE);

	// Check to see if the Render Region is valid!
	ERROR2IF(pGBMP == NULL,NULL,"Failed to create a GRenderBitmap in CBMPBits!");

	if (Transparent)
	{
		// Set the flag to say that we want to use the BiCompression Field for transparencies!
		pGBMP->m_DoCompression = TRUE;
	}

	// Start to render into region!
	if(!pGBMP->AttachDevice(pView, NULL, NULL))
	{
		ERROR3("Cannot attatch devices");
		
		delete pGBMP;
		pGBMP = NULL;
		
		return NULL;
	}

	pGBMP->InitDevice();
	pGBMP->InitAttributes();
	pGBMP->RRQuality.SetQuality(QUALITY_MAX);
	pGBMP->SetQualityLevel();
	pGBMP->SetLineAttributes();
	pGBMP->SetFillAttributes();

	pGBMP->StartRender();

	Node* pCurrent = pSel->FindFirst(FALSE);

	while(pCurrent != NULL)
	{
		// Render all it's child nodes using the following function
		pGBMP->RenderTreeNoCache(pCurrent);

		pCurrent = pSel->FindNext(pCurrent);
	}

	// Rendered everything so stop the render process.
	pGBMP->StopRender();

	// Now Extract the bitmap!
	pNewBMP = OILBitmap::Attach(pGBMP->ExtractBitmapCopy(),TRUE);

	// Delete all the new`s that have happened!
	if(pGBMP != NULL)
	{
		delete pGBMP;
		pGBMP = NULL;
	}
	
	// If everything went ok then return the pointer to the BMP!
	return pNewBMP;
}




