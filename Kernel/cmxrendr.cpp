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
// CMX RenderRegion, for exporting

#include "camtypes.h"
#include "cmxrendr.h"
#include "cmxexdc.h"
#include "view.h"
#include "spread.h"
#include "attrmgr.h"
#include "rrcaps.h"
#include "fixmem.h"
#include "range.h"
//#include "rendsel.h"
#include "nbevcont.h"

CC_IMPLEMENT_DYNAMIC(CMXRenderRegion, VectorFileRenderRegion)

/********************************************************************************************

>	CMXRenderRegion::CMXRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, 
									 FIXED16 ViewScale) :
										RenderRegion(ClipRect, ConvertMatrix, ViewScale)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Purpose:	Constructor for an CMXOutputFilter object.  The object should be initialised
				before use.
	SeeAlso:	CMXOutputFilter::Init

********************************************************************************************/

CMXRenderRegion::CMXRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale) :
	VectorFileRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
}

/********************************************************************************************

>	BOOL CMXRenderRegion::AttachDevice(View* pView, CDC* pDC, Spread* pSpread)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Inputs:		pView - the view to attach this region to - ignored (must be NULL).
				pDC - the device context to attach the render region to.
				pSpread - the spread to attach the render region to.
	Purpose:	Attach the CMX render region to a device context and a spread to export.
	SeeAlso:	RenderRegion::AttachDevice; CMXExportDC

********************************************************************************************/

BOOL CMXRenderRegion::AttachDevice(View* pView, CDC* pDC, Spread* pSpread)
{
	// Call the base class first
	if (!RenderRegion::AttachDevice(pView, pDC, pSpread))
		return FALSE;
	
	// Save the first node we need to export.
//	CurrentRenderState = pSpread->FindFirstForExport(this, FALSE);

	// Get a pointer to the CCDiskFile object
	CMXExportDC *pCMXDC = (CMXExportDC *) pDC;
	ExportFile = pCMXDC->ExportFile;

	// let it our lovely matrix
	pCMXDC->SetTransMatrix(&RenderMatrix);

	// All okey dokey
	return TRUE;
}


/********************************************************************************************

>	BOOL CMXRenderRegion::InitDevice()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Returns:	TRUE if the device context is initialised ok;
				FALSE if not.
	Purpose:	Initialise the device specific mechanisms for this render region.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/

BOOL CMXRenderRegion::InitDevice()
{
	// Call base class
	if (!RenderRegion::InitDevice())
		return FALSE;

	// Set the render caps up
	GetRenderRegionCaps(&Caps);

	// Find out which document we're using
	ENSURE(RenderView->GetDoc() != NULL, "View's document is NULL!");
	Document *TheDocument = RenderView->GetDoc();

	KernelDC *pDC = (KernelDC *) RenderDC;
	cmxDC = (CMXExportDC *)RenderDC;

	// Set up render region
	InitClipping();

	// All ok
	return TRUE;
}

// 
// These are the functions that are not used for exporting files...

/********************************************************************************************

>	void CMXRenderRegion::DrawPathToOutputDevice(Path *DrawPath, BOOL DataOnly)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Inputs:		DrawPath - the path to render.
	Purpose:	Output all the commands required to render this path to the CMX file.
	SeeAlso:	RenderRegion::DrawPath; CMXRenderRegion::GetValidPathAttributes
				CMXRenderRegion::ExportPath

********************************************************************************************/

void CMXRenderRegion::DrawPathToOutputDevice(Path *DrawPath, PathShape)
{
	ExportPath(DrawPath, FALSE);
}

/********************************************************************************************

>	BOOL CMXRenderRegion::ExportPath(Path *DrawPath, BOOL DataOnly)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Inputs:		DrawPath - the path to render.
				DataOnly - if TRUE, only output the line/curve/moveto data, and not the stroke
							or fill operators.
						   if FALSE, just output path normally.  NB. see Purpose
	Purpose:	Output all the commands required to render this path to the CMX file.
				If this is a compound path, the stroke/fill operators will always be output,
				regardless of the value of the DataOnly parameter.
	SeeAlso:	RenderRegion::DrawPath; CMXRenderRegion::GetValidPathAttributes

********************************************************************************************/

BOOL CMXRenderRegion::ExportPath ( Path *DrawPath, BOOL DataOnly )
{
	// Get the device context for our export file.
	KernelDC	*pDC		= ( KernelDC * ) RenderDC;

	DocCoord*	Coords		= DrawPath->GetCoordArray();
	PathVerb*	Verbs		= DrawPath->GetVerbArray();
	INT32			NumCoords	= DrawPath->GetNumCoords();

	// No need to export a zero length path.
	if ( NumCoords != 0 )
	{
		// There were some points on the path, so attempt to export it.
		return cmxDC->WritePath ( Coords, Verbs, NumCoords, DrawPath->IsFilled );
	}

	else
	{
		// Pretend that it worked to prevent any errors from being thrown.
		return TRUE;
	}
}

/********************************************************************************************

>	BOOL CMXRenderRegion::ExportBevel ( NodeBevel *pBevel )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Inputs:		pBevel	- A pointer to a bevel node.
	Returns:	TRUE if the node was successfully exported, FALSE if it wasn't.
	Purpose:	Contains the custom export code for the bevel node.

********************************************************************************************/

BOOL CMXRenderRegion::ExportBevel ( NodeBevel *pBevel )
{
	ColourFillAttribute		*pFill			= ( ColourFillAttribute * ) GetCurrentAttribute
											  ( ATTR_FILLGEOMETRY );
	StrokeColourAttribute	*pLine			= ( StrokeColourAttribute * ) GetCurrentAttribute
											  ( ATTR_STROKECOLOUR );
	OILBitmap				*pBMP			= NULL;
	DocColour				OldColour		( pLine->Colour );
	BitmapFillAttribute		BevFill;
	BOOL					Result			= FALSE;
	DocRect					Bounds			= pBevel->GetBoundingRect ();
	RangeControl			ControlFlags	( TRUE, TRUE );
	Range					ToRender		( pBevel, pBevel, ControlFlags );
	DocColour				Transparent		( COLOUR_TRANS );
	KernelBitmap* pBevelBMP = NULL;

	NodeBevelController* pControl = (NodeBevelController*)pBevel->GetParentController();

	if(pControl)
		pBevelBMP = pBevel->CreateBitmapCopy(pControl->GetDPI());
	else
	{
		ERROR2(FALSE,"Failed to find a bevel controller node!");
	}

	ERROR2IF(!pBevelBMP,FALSE,"Failed to create a new Export BevelBitmap!");

	// Copy the information over.
	BevFill.SimpleCopy ( pBevel->GetBitmapFillAttribute () );

	// Set the line colour to be transparent.
	pLine->Colour = Transparent;

	// For some reason, the pBevFill attribute isn't correctly instansiated at this point.
	// Thus it's necessary to load up the points describing its position before export is
	// attempted. Note: In the Flash export filter, these points are pre-loaded, so there
	// must be some weird bug somewhere.
	BevFill.SetStartPoint	( &Bounds.lo );
	BevFill.SetEndPoint		( &DocCoord ( Bounds.hix, Bounds.loy ) );
	BevFill.SetEndPoint2	( &DocCoord ( Bounds.lox, Bounds.hiy ) );
	
	// Attach the new bitmap to the bevel fill attribute.
	BevFill.AttachBitmap ( pBevelBMP );

	// Set up the bevel fill as a standard bitmap fill.
	SetFillGeometry ( &BevFill, FALSE );

	// Export the bevel.
	Result = ExportPath ( &( pBevel->InkPath ), FALSE );

	// Pop the attribute off the stack.
	RestoreFillGeometry ( pFill, FALSE );

	// Restore the line colour.
	pLine->Colour = OldColour;

	// Now delete the Copy Bitmap!
	delete pBevelBMP;
	pBevelBMP = NULL;

	return Result;
}

/********************************************************************************************

>	BOOL CMXRenderRegion::ExportBevelBegin ( NodeBevelBegin *pBevel )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	21/2/00
	Input:		pBevel - A pointer to a NodeBevelBegin object.
	Returns:	TRUE if success.
	Purpose:	Stub function to prevent NodeBevelBegin from rendering.
	SeeAlso:	-

********************************************************************************************/

BOOL CMXRenderRegion::ExportBevelBegin ( NodeBevelBegin *pBevel )
{
	return TRUE;
}

/********************************************************************************************

>	BOOL CMXRenderRegion::ExportShadow ( OILBitmap	*pBitmap,
										 UINT32		Darkness,
										 DocRect	&Bounds )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/2/00
	Inputs:		pShadow	- A pointer to a Shadow node.
	Returns:	TRUE if the node was successfully exported, FALSE if it wasn't.
	Purpose:	Contains the custom export code for the shadow node.

********************************************************************************************/

BOOL CMXRenderRegion::ExportShadow ( OILBitmap	*pBitmap,
									 UINT32		Darkness,
									 DocRect	&Bounds )
{
	// The bitmap export code for CMX in Camelot is a mess, due to it being done at the last
	// minute when the last version was shipped. Because of this, the ExportShadow function
	// repeats a few lines of code from NodeBitmap::ExportRender.
	CMXExportDC				*pDC			= static_cast<CMXExportDC *> ( RenderDC );
	ColourFillAttribute		*pFill			= ( ColourFillAttribute * ) GetCurrentAttribute
											  ( ATTR_FILLGEOMETRY );
	StrokeColourAttribute	*pLine			= ( StrokeColourAttribute * ) GetCurrentAttribute
											  ( ATTR_STROKECOLOUR );
	ADDR					BitmapAddress	= pBitmap->GetBitmapBits ();
	DocColour				ShadowColour	= *( static_cast<FillGeometryAttribute*>
												( GetCurrentAttribute ( ATTR_FILLGEOMETRY ) )
												->GetStartColour () );
	DocColour				Background		( COLOUR_WHITE );	// I don't do backgrounds yet.
	DocColour				BlendedShadow	= AlphaBlend ( ShadowColour, Background,
														   Darkness );
	DocCoord				Parallel [4];
	StrokeColourAttribute	StartColAttr	( BlendedShadow );
	FlatFillAttribute		EndColAttr;

	// I need to set the line colour (temporarily) to be the start colour, and the fill
	// colour to be the end colour. This sets the attributes up for a contone bitmap
	// fill.
	EndColAttr.SetStartColour ( &Background );

	SetLineColour	( &StartColAttr, FALSE );
	SetFillGeometry	( &EndColAttr, FALSE );

	// Get the rectangle describing the shadow's path.
	Parallel [0] = DocCoord ( Bounds.lox, Bounds.hiy );
	Parallel [1] = DocCoord ( Bounds.hix, Bounds.hiy );
	Parallel [2] = DocCoord ( Bounds.hix, Bounds.loy );
	Parallel [3] = DocCoord ( Bounds.lox, Bounds.loy );

	// Quickly knock up a kernel bitmap.
	KernelBitmap Bitmap ( pBitmap );

	// And export it.
	pDC->WriteBitmap ( &Bitmap, Parallel, CMXExportDC::CMXBITMAPCOLOURSOURCE_LINEANDFILL );

	// Restore the old settings.
	RestoreFillGeometry	( pFill, FALSE );
	RestoreLineColour	( pLine, FALSE );

	// It all worked - so return TRUE.
	return TRUE;
}

/********************************************************************************************

>	virtual void CMXRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void CMXRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	// This forces line attributes - dash patterns and arrow heads to be output
	// properly for CMX, rather than using Camelot 'shorthand' tokens.
	pCaps->CanDoNothing();
	pCaps->ArrowHeads = TRUE;
	pCaps->DashPatterns = TRUE;
}

/********************************************************************************************

>	virtual BOOL CMXRenderRegion::WantsGrids()

	Author:		Ben_Summers (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/06/96
	Returns: 	FALSE
	Purpose:	very little

********************************************************************************************/

BOOL CMXRenderRegion::WantsGrids()
{
	return FALSE;
}
