// $Id$
// Implementation of Adobe Illustrator EPS filter.
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

#include "ai_epsrr.h"

#include <stdio.h>
#include <math.h>

#include "vectrndr.h"
#include "ccdc.h"
//#include "colmodel.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillattr.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "opbevel.h"
#include "swfexpdc.h"
#include "swfrndr.h"
#include "slicehelper.h"
#include "fillramp.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "nodebmp.h"
//#include "bitmap.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "range.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "rendsel.h"
//#include "dibutil.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "fontman.h"		// for FontManager - for writing font changes to overflow text
#include "colplate.h"		// for ColourPlate - in overflow text functions
#include "colourix.h"		// for Indexed Colour - for writing colour changes to overflow
#include "nodebev.h"
#include "layer.h"
#include "clipattr.h"

CC_IMPLEMENT_DYNAMIC(AIEPSRenderRegion, EPSRenderRegion)

/********************************************************************************************

>	AIEPSRenderRegion::AIEPSRenderRegion ( DocRect	ClipRect,
										   Matrix	ConvertMatrix,
										   FIXED16	ViewScale )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/00
	Purpose:	Create and initialise a render region for exporting AI EPS. Sets up the
				string to put in the %%Creator comment.
	SeeAlso:	EPSRenderRegion::EPSRenderRegion ()

********************************************************************************************/

AIEPSRenderRegion::AIEPSRenderRegion ( DocRect	ClipRect,
									   Matrix	ConvertMatrix,
									   FIXED16	ViewScale )
	: EPSRenderRegion ( ClipRect, ConvertMatrix, ViewScale )
{
	// Set up member variables.
	m_a				= 1;
	m_b				= 0;
	m_c				= 0;
	m_d				= 1;
	m_T				= DocCoord ( 0, 0 );
	m_ActiveLayer	= FALSE;
	m_LayerColour	= 0;

	m_fpOverflowText	= NULL;

	m_bInTextGap		= FALSE;
	m_bTextAsShapes		= FALSE;
	
	m_pLinearGradList	= NULL;
	m_pRadialGradList	= NULL;

	// Initialise the creator string to show that it's an AI file.
	CreatorString = _T("Adobe Illustrator(TM) 7.0 by Xara.");
}

/********************************************************************************************

>	AIEPSRenderRegion::~AIEPSRenderRegion 
	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001
	Purpose:	Destructor for an AI render region. All this really does is dispose of the 
				gradient fill cache (if there is one)
	SeeAlso:	EPSRenderRegion::EPSRenderRegion ()

********************************************************************************************/
AIEPSRenderRegion::~AIEPSRenderRegion ()
{
	ClearGradientCache ();
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::ExportBitmap ( NodeBitmap	*pNodeBMP )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/00
	Inputs:		pNodeBMP	- A pointer to the NodeBitmap being exported.
	Returns:	TRUE		- If successful.
				FALSE		- If an error is encountered.
	Purpose:	Kicks off the NodeBitmap export process..
	SeeAlso:	NodeBitmap::ExportRender ()

********************************************************************************************/

BOOL AIEPSRenderRegion::ExportBitmap ( NodeBitmap	*pNodeBMP )
{
	// Set up the local variables.
	KernelBitmap	*pBitmap	= pNodeBMP->GetBitmap ();

	// Set up the bitmap's transformation matrix.
	LoadBitmapMatrix ( pNodeBMP );

	// Write out the bitmap record.
	WriteBitmapRecord ( pBitmap->GetActualBitmap () );

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::ExportBevel ( NodeBevel	*pBevel )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/4/00
	Inputs:		pBevel	- A pointer to a bevel node.
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	Renders a bevel into a DIB, and exports this as a bitmap to the AI file.
	SeeAlso:	NodeBevel::ExportRender

********************************************************************************************/

BOOL AIEPSRenderRegion::ExportBevel ( NodeBevel	*pBevel )
{
	// Set up the local variables.
	KernelDC				*pDC			= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );
	RangeControl			ControlFlags	( TRUE, TRUE );
	Range					ToRender		( pBevel, pBevel, ControlFlags );
	KernelBitmap			*pBitmap		= NULL;
	Path					*pSourcePath	= &( pBevel->InkPath );
	DocRect					Bounds			= pBevel->GetBoundingRect ();
	DocCoord				Position		( Bounds.lo.x, Bounds.hi.y );
	double					Width			= static_cast<double> ( Bounds.Width () );
	double					Height			= static_cast<double> ( Bounds.Height () );

	// Create and render the bitmap.
	pBitmap = pBevel->CreateBitmapCopy(-1.0,FALSE);

	if(!pBitmap)
		return FALSE;

	// Get the width and height of the bitmap.
	double					BMPWidth		= static_cast<double> ( pBitmap->GetWidth () );
	double					BMPHeight		= static_cast<double> ( pBitmap->GetHeight () );

	// Write the path. (ChrisG 16/01/01) If we have one.
	if (pSourcePath->GetNumCoords () != 0)
	{
		WriteMask ( pSourcePath, TRUE );
	}

	// Set up the bitmap translation matrix.
	LoadTranslationMatrix ( Position );

	// Scale width and height to be valid value for AI co-ordinates.
	Width	/= 1000;
	Height	/= 1000;
	
	// And calcuate the scale values for placing the bitmap.
	m_a	= Width  / BMPWidth;
	m_d	= Height / BMPHeight;

	// Write the bitmap record.
	WriteBitmapRecord ( pBitmap->ActualBitmap );

	// Write the end of mask operator. (ChrisG 16/01/01) If we've written a mask.
	if (pSourcePath->GetNumCoords () != 0)
	{
		pDC->OutputToken	( _T("Q") );
		pDC->OutputNewLine	();
	}

	if(pBitmap)
	{
		pBitmap->DestroyGreyscaleVersion();
		delete pBitmap;
	}

	// Success!
	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::ExportShadow ( OILBitmap	*pBitmap,
										   UINT32			Darkness,
										   DocRect		&Bounds )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/00
	Inputs:		pShadow	- A pointer to a shadow node.
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	Renders a shadow into a DIB, and exports this as a bitmap to the AI file.
	SeeAlso:	NodeBevel::ExportRender

********************************************************************************************/

BOOL AIEPSRenderRegion::ExportShadow ( OILBitmap	*pBitmap,
									   UINT32			Darkness,
									   DocRect		&Bounds )
{
	// Set up the local variables.
//	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	DocCoord	Position		( Bounds.lo.x, Bounds.hi.y );
	double		Width			= static_cast<double> ( Bounds.Width () ) / 1000;
	double		Height			= static_cast<double> ( Bounds.Height () ) / 1000;
	double		BMPWidth		= static_cast<double> ( pBitmap->GetWidth () );
	double		BMPHeight		= static_cast<double> ( pBitmap->GetHeight () );
	DocColour	ShadowColour	= *( static_cast<FillGeometryAttribute*>
									 ( GetCurrentAttribute ( ATTR_FILLGEOMETRY ) )
									 ->GetStartColour () );
	DocColour	BlendedShadow	= AlphaBlend ( ShadowColour, mBackgroundColour, Darkness );

	// And use the shadow colour to build a contone bitmap palette.
	pBitmap->BuildContonePalette ( BlendedShadow, mBackgroundColour, EFFECT_RGB,
								   RenderView );

	// Set up the bitmap translation matrix.
	LoadTranslationMatrix ( Position );
	
	// And calcuate the scale values for placing the bitmap.
	m_a	= Width  / BMPWidth;
	m_d	= Height / BMPHeight;

	// Write out the bitmap header.
	WriteBitmapHeader ( (INT32)BMPWidth, (INT32)BMPHeight );

	// Export the bitmap.
	WriteContoneBody ( pBitmap );

	// Finish off the bitmap record.
	WriteBitmapTail ();

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::ExportLayer ( Layer	*pLayer )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/00
	Inputs:		pLayer	- A pointer to a layer object.
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	If there is an existing layer, it writes the end of layer tags, before
				creating a new layer record.
	SeeAlso:	AIEPSRenderRegion::EndLayer ()

********************************************************************************************/

BOOL AIEPSRenderRegion::ExportLayer ( Layer	*pLayer )
{
	// Tidy up the existing layer, if it exists.
	EndLayer ();

	// And write out the next layer.
	return StartLayer ( static_cast<Layer*> ( pLayer->FindNext
											  ( CC_RUNTIME_CLASS ( Layer ) ) ) );
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::StartLayer ( Layer	*pLayer )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/00
	Inputs:		pLayer	- A pointer to a layer object.
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	Writes a layer record to the file.
	SeeAlso:	AIEPSRenderRegion::EndLayer ()

********************************************************************************************/

BOOL AIEPSRenderRegion::StartLayer ( Layer	*pLayer )
{
	// Catch null pointers to layers.
	if ( pLayer != NULL )
	{
		KernelDC	*pDC			= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );
		TCHAR		LayerName [258];

		// Set up the layer's name.
		camSprintf ( LayerName, _T("(%s)"), ( TCHAR* ) pLayer->GetLayerID () );

		// Start a layer definition.
		pDC->OutputToken	( _T("%AI5_BeginLayer") );
		pDC->OutputNewLine	();

		// Set the layer's properties.
		pDC->OutputValue	( ( UINT32 ) pLayer->IsVisible () );	// Is the layer visible?
		pDC->OutputValue	( ( UINT32 ) 1 );					// Is the layer previewed?
		pDC->OutputValue	( ( UINT32 ) !pLayer->IsLocked () );	// Is the layer enabled?
		pDC->OutputValue	( ( UINT32 ) pLayer->IsPrintable () );//Is the layer printable?
		pDC->OutputValue	( ( UINT32 ) 0 );					// Is the layer dimmed?
		pDC->OutputValue	( ( UINT32 ) 0 );					// No multilayer masks.

		// The ID colour.
		pDC->OutputValue	( ( UINT32 ) m_LayerColour % 27 );	// Set the layer's ID colour.
		m_LayerColour ++;										// Increment colour ID.

		// Colour values - I don't know what they're used for.
		pDC->OutputValue	( ( UINT32 ) 0 );					// Red intensity.
		pDC->OutputValue	( ( UINT32 ) 0 );					// Blue intensity.
		pDC->OutputValue	( ( UINT32 ) 0 );					// Green intensity.

		// Write out the Lb tag.
		pDC->OutputToken	( _T("Lb") );
		pDC->OutputNewLine	();

		// Set the layer's name.
		pDC->OutputToken	( LayerName );
		pDC->OutputToken	( _T("Ln") );
		pDC->OutputNewLine	();

		// Tell Camelot that there is an active layer.
		m_ActiveLayer = TRUE;
	}

	// Success!
	return TRUE;
}

/********************************************************************************************

>	void AIEPSRenderRegion::RenderChar (WCHAR ch, Matrix * pMatrix)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Overrides the default behaviour for rendering a single character. Used to 
				store additional information on text blocks needed for AI text. i.e. the
				overflow text block ((xxxxxxxxx) TX) which occurs after the unjustified 
				((x) Tx\n (x) Tx\n) block. Both of these are needed for correct Illustrator
				export of text.
	SeeAlso:	EPSRenderRegion::RenderChar

********************************************************************************************/

BOOL AIEPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
	TCHAR buf[20];
	BOOL result = FALSE;		// assume failure if nothing given.

	if (m_bTextAsShapes)
	{
		result = RenderCharAsShape (ch, pMatrix);
	}
	else
	{
		// Output matrix (a b c d e f Tm\n) - a,b,c and d are in the range 1.0 to -1.0
		// Only do this for text on paths
		if (ExportingOnPath ())
		{
			KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

			pDC->OutputMatrix(pMatrix);
			pDC->OutputToken(_T("Tm"));
			pDC->OutputNewLine();
		}

		// call the base class first, so that the attributes are exported BEFORE the character
		result = EPSRenderRegion::RenderChar (ch, pMatrix);;
		
			// store any extra information, closing an attribute gap if one is open.
		if (ExportingOnPath ())
		{
			OverflowTextFinishGap ();
			camSprintf (buf,_T("%c"),ch);
			OverflowTextWrite (buf);
		}
	}
		// Call the base class' function to do the standard export.
	return result;
}


/********************************************************************************************

>	void AIEPSRenderRegion::RenderCharAsShape (WCHAR ch, Matrix * pMatrix)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Allows text to be drawn as shapes, not characters, so that AI can cope
				with gradient filled text.
	SeeAlso:	AIEPSRenderRegion::RenderChar, EPSRenderRegion::RenderChar

********************************************************************************************/

BOOL AIEPSRenderRegion::RenderCharAsShape (WCHAR ch, Matrix* pMatrix)
{
	BOOL result = FALSE;

	// create the char's path
	Path* pCharPath=CreateCharPath(ch,pMatrix);

	// if the path got created correctly, then we done good...
	if (pCharPath!=NULL)
	{
		// if at least one point, draw path using current attibutes in render region
 		if (pCharPath->GetNumCoords()!=0)
			DrawPath(pCharPath);

		// clean up
		delete pCharPath;

		result = TRUE;
	}

	return result;
}

/********************************************************************************************

>	void AIEPSRenderRegion::Initialise()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/00
	Purpose:	Sets up render region - it outputs the AIEPS file header comments, and
				intialises the rendering attributes.
	SeeAlso:	EPSRenderRegion::Initialise, EPSRenderRegion::InitAttributes

********************************************************************************************/

void AIEPSRenderRegion::Initialise()
{
	// Set up member variables.
	m_a				= 1;
	m_b				= 0;
	m_c				= 0;
	m_d				= 1;
	m_T				= DocCoord ( 0, 0 );
	m_ActiveLayer	= FALSE;
	m_LayerColour	= 0;

	ClearGradientCache ();

	// Call the EPSRenderRegion method to re-enter the export loop.
	EPSRenderRegion::InitAttributes ();
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteEPSVersion ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/000
	Inputs:		pDC - The device context to output to.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes the relevant EPS version declaration.
	SeeAlso:	EPSRenderRegion::WriteEPSVersion

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteEPSVersion ( void )
{
	// Cast the pointer to an appropriate DC.
	KernelDC *pDC = static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );

	// Output the special AIEPS header start token.
	pDC->OutputToken	( _T("%!PS-Adobe-3.0") );
	pDC->OutputNewLine	();

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteEPSProcessColours ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/000
	Inputs:		pDC - The device context to output to.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes the colour type used to disk.
	SeeAlso:	EPSRenderRegion::WriteEPSProcessColours

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteEPSProcessColours ( void )
{
	// Cast a pointer to the appropriate DC.
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);

	// Output the process colours
	pDC->OutputToken	( _T("%%DocumentProcessColors: Cyan Magenta Yellow Black") );
	pDC->OutputNewLine	();

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteEPSResources ( EPSFilter	*pFilter,
												Document	*pDocument )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/00
	Inputs:		pDC - The device context to output to.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	This function writes out the resource inclusion string required for the AI
				file.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteEPSResources ( EPSFilter	*pFilter,
											Document	*pDocument )
{
	// Cast a pointer to the appropriate DC.
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	
	// Call the export method in the document.
	pDocument->WriteEPSResources ( pFilter );

	// Add a few things necessary for the AI file format.
	pDC->OutputToken	( _T("%AI3_ColorUsage: Color") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("%AI5_FileFormat 2.0") );
	pDC->OutputNewLine	();

	// All done.
	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteEPSProlog ( EPSFilter	*pFilter,
											 Document	*pDocument )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/00
	Inputs:		pDC - The device context to output to.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	This method writes out the initialisation code for the included resources.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteEPSProlog ( EPSFilter	*pFilter,
										 Document	*pDocument )
{
	// Call the export method in the document.
	pDocument->WriteEPSProlog ( pFilter );

	// All done.
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::WriteGradientFills ( Document *pDocument )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/00
	Inputs:		pDocument - The document to be written.
	Outputs:	-
	Returns:	TRUE if successful.
				FALSE if error (e.g. file/disk error)
	Purpose:	Parses through the tree, identifies any gradient fills, and writes them to
				the export DC's file.

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteGradientFills ( Document *pDocument )
{
	// Get the spread from the document.
	Spread		*pSpread		= pDocument->FindFirstSpread ();

	// Build the gradient cache up from all the fills under the spread, after cleaning any 
	//	junk out of it (there shouldn't be any there, but that ain't the point)
	ClearGradientCache ();
	BuildGradientCache (pSpread);

	// Write out the number of fills and their definition.
	WriteGradientCount ();
	WriteGradientDefinitions ();

	// The fill has been successfully saved, so return TRUE.
	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteLinearFill ( FillGeometryAttribute	*pFill, INT32 id )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/00
	Inputs:		pFill	- A pointer to the fill attribute record.
				id		- the id number for the fill
	Returns:	TRUE	- If successful.
				FALSE	- The was an error (e.g. file/disk error).
	Purpose:	Writes a linear fill to the disk file.

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteLinearFill ( FillGeometryAttribute * pFill, EFFECTTYPE effect, INT32 id )
{
	// Extract the fill attribute - this is necessary for processing the fill type.
	KernelDC				*pDC				= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );
	INT32						Colours				= 2;
	TCHAR					GradientName [32];
	DocColour				StartColour			= *( pFill->GetStartColour () );
	DocColour				EndColour;
	ColourRamp				*pRamp				= pFill->GetColourRamp ();

	// The fill is, or can be approximated by, a linear fill.
	camSprintf ( GradientName, _T("(%s %d)"), LinearGradient, id );

	// (ChrisG 16/01/01) Four colour fill MUST come first, as it's derived from Three
	//	colour fill, and returns valid for IsThreeColFill().
	if ( pFill->IsAFourColFill () )
	{
		// I'm going to use a version of the below here, but for now I'll just
		// set up the end colour as the most distant end colour.
		EndColour	= *( pFill->GetEndColour3 () );
	}
	else if ( pFill->IsAThreeColFill () )
	{
		// AI doesn't support three colour fills, so I'm going to fake it. This
		// code merges the two end colours to approximate the fill with a two
		// colour linear fill.
		EndColour.Mix ( pFill->GetEndColour (), pFill->GetEndColour2(),
						0.5f, NULL, FALSE, NULL );
	}
	else
	{
		// Set up the end colour value appropriately.
		EndColour	= *( pFill->GetEndColour () );
	}

	// Count the number of colours.
	if ( pRamp != NULL )
	{
		UINT32	First	= 0;
		UINT32	Last	= 0;

		// Extract the indices from the colour ramp.
		pRamp->GetIndexRange ( &First, &Last );

		// And add their difference to the colour count. Note, you need to add one
		// to the result so that you don't register having no colours. There really
		// should be a count number of elements method in the ramp code, though.
		Colours += 1 + Last - First;
	}

	// (ChrisG 15/01/01) If this is a four colour fill, since we will be adding a mid-colour
	//	below, then we need to include it in the definition
	if (pFill->IsAFourColFill ())
	{
		Colours ++;
	}

	// (ChrisG 4/4/2001) Add in the extra colours given from the HSV fill effect emulation
	if (effect == EFFECT_HSV_SHORT || effect == EFFECT_HSV_LONG)
	{
		Colours += (Colours - 1) * NumFillEmulationSteps;
	}

	// Write the gradient's header.
	pDC->OutputToken	( _T("%AI5_BeginGradient:") );
	pDC->OutputToken	( GradientName );
	pDC->OutputNewLine	();

	// Then the type definition.
	pDC->OutputToken	( GradientName );
	pDC->OutputValue	( static_cast<INT32> ( LinearFill ) );
	pDC->OutputValue	( static_cast<INT32> ( Colours ) );
	pDC->OutputToken	( _T("Bd") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("[") );
	pDC->OutputNewLine	();

	// (ChrisG 3/4/2001) Get bias and convert to 0-100 range (from -1 to +1)
	//	This will be used later on to offset the positions
	CProfileBiasGain profile = pFill->GetProfile ();
	INT32 bias = (INT32) ((1 - (double) profile.GetBias ()) * 50);

	// Write out the last colour.
	WriteGradientEntry ( &EndColour, 100, 50 );

	// set up the last colour and position (for the fill effect emulation.
	DocColour * pLastColour = &EndColour;
	INT32 lastPos = 100;

	// Mid colour for four colour fills - this needs to be stored longer so that it can be
	//	re-used in the case of HSV fill effects.
	DocColour	MidColour;

	// If the fill is a four colour fill, add a mid-colour that's a blend of the
	// second and third colours.
	if ( pFill->IsAFourColFill () )
	{

		MidColour.Mix ( pFill->GetEndColour (), pFill->GetEndColour2 (),
						0.5f, NULL, FALSE, NULL );

		// Write out the extra fill steps, if there are any.
		if (effect != EFFECT_RGB)
		{
			WriteFillEffectSteps (pLastColour, lastPos, &MidColour, 50, bias, effect);
			pLastColour = &MidColour;
		}

		// And set the value.
		WriteGradientEntry ( &MidColour, 50, bias);
	}

	// Otherwise, process the colour ramp.
	else if ( pRamp != NULL )
	{
		ColRampItem *pItem = pRamp->GetLastCol ();

		// Write in the ramp's colour values.
		while ( pItem != NULL )
		{
			// Get the position of the colour in the fill.
			INT32		Ratio	= static_cast <INT32> ( 100 * pItem->GetPosition () );
			DocColour	*pCol	= pItem->GetColourAddr ();

			// Write out the extra fill steps, if there are any.
			if (effect != EFFECT_RGB)
			{
				WriteFillEffectSteps (pLastColour, lastPos, pCol, Ratio, bias, effect);
				pLastColour = pCol;
				lastPos = Ratio;
			}

			// And set the value.
			WriteGradientEntry ( pCol, Ratio, 50 );

			// Increment the pointer onto the next item.
			pItem = pRamp->GetPrevCol ( pItem );
		}
	}

	// Write out the extra fill steps, if there are any.
	if (effect != EFFECT_RGB)
	{
		WriteFillEffectSteps (pLastColour, lastPos, &StartColour, 0, bias, effect);
	}

	// Write out the start colour.
	WriteGradientEntry ( &StartColour, 0, bias );

	// Write out the end of gradient tokens.
	pDC->OutputToken	( _T("BD") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("%AI5_EndGradient") );
	pDC->OutputNewLine	();

	// It worked!
	return TRUE;
}


/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteRadialFill ( AttrFillGeometry	*pFill, INT32 id )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/00
	Inputs:		pFill	- A pointer to the fill attribute record.
				id		- The fill's id number
	Returns:	TRUE	- If successful.
				FALSE	- The was an error (e.g. file/disk error).
	Purpose:	Writes a Radial fill to the disk file.

********************************************************************************************/
BOOL AIEPSRenderRegion::WriteRadialFill (FillGeometryAttribute * pFill, EFFECTTYPE effect, INT32 id)
{
	// Extract the fill attribute - this is necessary for processing the fill type.
	KernelDC				*pDC				= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );
	INT32						Colours				= 2;
	TCHAR					GradientName [32];
	DocColour				StartColour			= *( pFill->GetStartColour () );
	DocColour				EndColour			= *( pFill->GetEndColour () );
	ColourRamp				*pRamp				= pFill->GetColourRamp ();

	// The fill is, or can be approximated by, a Radial fill.
	camSprintf ( GradientName, _T("(%s %d)"), RadialGradient, id );

	// Count the number of colours.
	if ( pRamp != NULL )
	{
		UINT32	First	= 0;
		UINT32	Last	= 0;

		// Extract the indices from the colour ramp.
		pRamp->GetIndexRange ( &First, &Last );

		// And add their difference to the colour count. Note, you need to add one
		// to the result so that you don't register having no colours. There really
		// should be a count number of elements method in the ramp code, though.
		Colours += 1 + Last - First;

		if (effect != EFFECT_RGB)
		{
			// Add in the extra steps for the fill effect (Five extra steps between each 
			//	pair of colours).
			Colours += (Colours - 1) * NumFillEmulationSteps;
		}
	}

	// Write the gradient's header.
	pDC->OutputToken	( _T("%AI5_BeginGradient:") );
	pDC->OutputToken	( GradientName );
	pDC->OutputNewLine	();

	// Then the type definition.
	pDC->OutputToken	( GradientName );
	pDC->OutputValue	( static_cast<INT32> ( RadialFill ) );
	pDC->OutputValue	( static_cast<INT32> ( Colours ) );
	pDC->OutputToken	( _T("Bd") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("[") );
	pDC->OutputNewLine	();

	// (ChrisG 3/4/2001) Get bias and convert to 0-100 range (from -1 to +1)
	CProfileBiasGain profile = pFill->GetProfile ();
	INT32 bias = (INT32) ((1 - (double) profile.GetBias ()) * 50);
		
	// Write out the start colour.
	WriteGradientEntry ( &StartColour, 0, bias );

	DocColour * pLastCol = &StartColour;
	INT32 lastPos = 0;

	// Process the colour ramp.
	if ( pRamp != NULL )
	{
		ColRampItem *pItem = pRamp->GetFirstCol ();

		// Write in the ramp's colour values.
		while ( pItem != NULL )
		{
			// Get the position of the colour in the fill.
			INT32		Ratio	= static_cast <INT32> ( 100 * pItem->GetPosition () );
			DocColour	*pCol	= pItem->GetColourAddr ();

			// Write out the intermediate fill steps (if there are any).
			if (effect != EFFECT_RGB)
			{
				WriteFillEffectSteps (pLastCol, lastPos, pCol, Ratio, bias, effect);
				pLastCol = pCol;
				lastPos = Ratio;
			}

			// And set the value.
			WriteGradientEntry ( pCol, Ratio, 50 );

			// Increment the pointer onto the next item.
			pItem = pRamp->GetNextCol ( pItem );
		}
	}

	// Write out the intermediate fill steps (if there are any).
	if (effect != EFFECT_RGB)
	{
		WriteFillEffectSteps (pLastCol, lastPos, &EndColour, 100, bias, effect);
	}

	// Write out the last colour.
	WriteGradientEntry ( &EndColour, 100, 50 );

	// Write out the end of gradient tokens.
	pDC->OutputToken	( _T("BD") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("%AI5_EndGradient") );
	pDC->OutputNewLine	();

	// It worked!
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::WriteGradientEntry ( DocColour	*pColour,
														 INT32		Position,
														 INT32		Midpoint )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/00
	Inputs:		pColour		- A pointer to a Camelot colour.
				Position	- The position of the colour within the fill.
				Midpoint	- The midpoint of the fill.
	Returns:	TRUE if successful.
				FALSE if error (e.g. file/disk error)
	Purpose:	Writes a line into the definition of a gradient fill.

********************************************************************************************/
BOOL AIEPSRenderRegion::WriteGradientEntry ( DocColour	*pColour,
											 INT32		Position,
											 INT32		Midpoint )
{
	INT32 red;											//
	INT32 green;											// RGB colour values
	INT32 blue;											//
	PColourCMYK	CMYKColour;								// The colour to be stored.

	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	TCHAR		Line [80];								// Contains the string to be stored.

	// (ChrisG 3/4/2001) 
	if (Midpoint < 13)
		Midpoint = 13;
	else if (Midpoint > 87)
		Midpoint = 87;

	pColour->GetCMYKValue ( &CMYKColour );				// Extract the CMYK value.

	if (pColour->GetColourModel () == COLOURMODEL_RGBT ||
		pColour->GetColourModel () == COLOURMODEL_HSVT)
	{
		// Use RGB syntax.
		pColour->GetRGBValue ( &red, &green, &blue );

		camSprintf ( Line, _T("2 %d %d%%_Bs"), Midpoint, Position );
		pDC->OutputColour		( &CMYKColour );
		pDC->OutputColourValue	( red );
		pDC->OutputColourValue	( green );
		pDC->OutputColourValue	( blue );
		pDC->OutputToken		( Line );
		pDC->OutputNewLine		();
	}
	else
	{

		// It is necessary to build the output line like this, so that there isn't a space
		// between the position of the colour, and the %_Bs operator.
		camSprintf ( Line, _T("1 %d %d%%_Bs"), Midpoint, Position );	// Build the output string.

		pDC->OutputColour	( &CMYKColour );				// Write the colour to the file.
		pDC->OutputToken	( Line );						// Write the output string out.
		pDC->OutputNewLine	();								// And write a new-line tag.
	}

	return TRUE;										// All done successfully.
}

/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::WriteFillEffectSteps (	DocColour * pColour1,
															INT32		pos1,
															DocColour * pColour2,
															INT32		pos2,
															INT32		bias,
															EFFECTTYPE	effect)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/2001
	Inputs:		pColour1 	- The colour to start from.
				pos1		- The position of the start colour within the fill (0-100%).
				pColour2	- The colour to end at.
				pos2		- The position of the end colour within the entire fill (0-100%)
				bias		- The fill's bias part of any profile (0-100%).
				effect		- The type of fill effect (EFFECT_HSV_SHORT or EFFECT_HSV_LONG)
	Returns:	TRUE if successful.
				FALSE if error (e.g. file/disk error)
	Purpose:	Writes the additional entries required for a special fill effect (rainbow or
				alt-rainbow) to the gradient definition, as a series of steps.
	See Also:	AIEPSRenderRegion::WriteGradientEntry
				AIEPSRenderRegion::WriteLinearFill
				AIEPSRenderRegion::WriteRadialFill

********************************************************************************************/
BOOL AIEPSRenderRegion::WriteFillEffectSteps (	DocColour *pColour1,
												INT32 pos1, 
												DocColour *pColour2, 
												INT32 pos2, 
												INT32 bias, 
												EFFECTTYPE effect)
{
	ERROR3IF (effect == EFFECT_RGB, "AIEPSRenderRegion::WriteFillEffectSteps, an attempt was made to write HSV fill effect steps on an RGB fade fill");

	// RGB fades are the 'standard' fill, so we don't want to do any special processing on them
	if (effect != EFFECT_RGB)
	{
		// Start and end colour values
		INT32 h1 = 0;
		INT32 h2 = 0;
		INT32 s1 = 0;
		INT32 s2 = 0;
		INT32 v1 = 0;
		INT32 v2 = 0;

		// intermediate colour values / increments
		DocColour midColour;
		double hAdd = 0;
		double sAdd = 0;
		double vAdd = 0;
		double posAdd = 0;

		// First get the start and end colour components
		pColour1->GetHSVValue (&h1, &s1, &v1);
		pColour2->GetHSVValue (&h2, &s2, &v2);

		// Calculate the increments  
		//
		//	NOTE: if the hue changes by more than 180 deg. in a short HSV (rainbow) fill
		//	(or not more than 180 deg. in a long HSV (alt-rainbow) fill), then the direction
		//	must be reversed. This is done by changing the hue increment value.
		hAdd = h2-h1;
		if (effect == EFFECT_HSV_SHORT)
		{
			if (hAdd > 180)
				hAdd -= 360;
			else if (hAdd < -180)
				hAdd += 360;
		}
		else
		{
			if ((hAdd > 0) && (hAdd <= 180))
				hAdd -= 360;
			else if ((hAdd <= 0) && (hAdd >= -180))
				hAdd += 360;
		}
		hAdd = hAdd / (NumFillEmulationSteps + 1);
		sAdd = (s2-s1) / (NumFillEmulationSteps + 1);
		vAdd = (v2-v1) / (NumFillEmulationSteps + 1);
		posAdd = ((double) pos2-pos1) / (NumFillEmulationSteps + 1);

		// Cycle through the stops calcuating each stop's colour and position, before 
		//	writing them out.
		for (INT32 i=1; i<NumFillEmulationSteps + 1; i++)
		{
			// increment each colour value and the position
			h2 = (INT32)(h1 + (i * hAdd));
			s2 = (INT32)(s1 + (i * sAdd));
			v2 = (INT32)(v1 + (i * vAdd));
			pos2 = (INT32)(pos1 + (i * posAdd));

			// wrap the hue around if it's gone past 360 deg or 0 deg.
			if (h2>=360)
				h2 -= 360;
			else if (h2<0)
				h2 += 360;

			// create the colour and export.
			midColour.SetHSVValue (h2, s2, v2);
			WriteGradientEntry (&midColour, pos2, 50);
		}
	}	// End of is fill effect not an RGB fade.

	return TRUE;
}

/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::WriteDocumentSetup ( Document	*pDocument,
														 EPSFilter	*pFilter )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/00
	Inputs:		pDocument	- The document being outputted.
				pFilter		- A pointer to the export filter.
	Outputs:	-
	Returns:	TRUE if successful.
				FALSE if error (e.g. file/disk error)
	Purpose:	Browse through the tree, and if a text story is found invoke the document
				setup function. By only exporting the font setup when necessary, the file
				size produced is smaller (which makes it easier for me to interpret :) ),
				and it should hopefully make the filter run a bit faster too.

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteDocumentSetup ( Document	*pDocument,
											 EPSFilter	*pFilter )
{
	// Get the spread from the document.
	Spread		*pSpread	= pDocument->FindFirstSpread ();
	Node		*pTextStory	= NULL;

	// Try to find a single instance of the TextStory node.
	pTextStory = SliceHelper::FindNextOfClass ( pSpread, pSpread,
												CC_RUNTIME_CLASS ( TextStory ) );

	// If something was found...
	if ( pTextStory != NULL )
	{
		// ... write out the font set-up.
		pDocument->WriteEPSSetup ( pFilter );
	}
	else
	{
		// Otherwise just get the relevant bits.
		pDocument->AIExportExtras ( pFilter->GetExportDC () );
	}

	// It worked!
	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::EndLayer ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	If there is an existing layer, it writes the end of layer tags, before
				creating a new layer record.
	SeeAlso:	AIEPSRenderRegion::ExportLayer ()

********************************************************************************************/

BOOL AIEPSRenderRegion::EndLayer ()
{
	// Only act if there's an active layer.
	if ( m_ActiveLayer )
	{
		// Cast the RenderDC pointer into a useful form.
		KernelDC	*pDC	= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );

		// Write out the end of layer tag.
		pDC->OutputToken	( _T("LB") );
		pDC->OutputNewLine	();
		pDC->OutputToken	( _T("%AI5_EndLayer") );
		pDC->OutputNewLine	();

		// Set m_ActiveLayer to be FALSE.
		m_ActiveLayer = FALSE;
	}

	return TRUE;
}

/********************************************************************************************

>	void AIEPSRenderRegion::WriteGradientFillInstance ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9/03/00
	Purpose:	This function identifies whether or not the current fill is a gradient fill,
				and writes in the appropriate values if it is. I've already created the
				records of the gradients themselves (see above), so this is just a case of
				matching the gradient with its ID, and Adobe Illustrator can then put the
				records together.

				(ChrisG 18/12/00) - all this does now is increment the counters used for 
				determining which gradient fill is currently being used.

	SeeAlso:	EPSRenderRegion::WriteGradientFill, WriteGradientFillUsage

********************************************************************************************/

void AIEPSRenderRegion::WriteGradientFillInstance ()
{
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	FillGeometryAttribute	*pFillAttr	= ( FillGeometryAttribute* )
											CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	DocCoord				StartPoint	= *( pFillAttr->GetStartPoint () );
	DocCoord				EndPoint	= *( pFillAttr->GetEndPoint () );
	DocCoord				Delta		= EndPoint - StartPoint;
	double					Angle		= 90.0f;
	double					Length		= FlashExportDC::CalculateLength ( Delta );
	TCHAR					Name [80];
	BOOL					IsRadial	= pFillAttr->IsARadialFill () ||
										  pFillAttr->IsASquareFill ();
	INT32					id			= 0;

	if (IsGradientFillValidForExport (pFillAttr))
	{
		id = FindGradientInCache (pFillAttr, GetFillEffect ());

		// If the fill is, or can be approximated by, a radial fill, set the the definition to
		// indicate this.
		if ( IsRadial )
		{
			// Set up the name to show that it's a radial gradient.
			camSprintf ( Name, _T("(%s %d)"), RadialGradient, id );
		}
		// Otherwise it's represented by a linear fill.
		else
		{
			// Set up the name to show that it's a linear gradient.
			camSprintf ( Name, _T("(%s %d)"), LinearGradient, id );
		
			// Reset the end point value if we're dealing with a three colour fill.
			if ( pFillAttr->IsAThreeColFill () )
			{
				DocCoord	EndPoint2	= *( pFillAttr->GetEndPoint2 () );

				// Complete the parallellogram of vectors.
				EndPoint += EndPoint2 - StartPoint;
			}
		}

		// (ChrisG 16/01/01) Since all the endpoint calculations have now been done,
		//	we can determine Delta, length and angle. (NOTE: these cannot be done 
		//	before here, as we wouldn't be using the correct endpoint values.)
		Delta	= EndPoint - StartPoint;
		Length	= FlashExportDC::CalculateLength ( Delta );

		// The tangent of an angle is undefined for PI/2 radians. (i.e. when Delta.x
		// is zero.)
		if ( Delta.x != 0 )
		{
			Angle = atan ( ( double ) Delta.y / ( double ) Delta.x );

			// AI uses degrees, whereas the standard maths library uses radians, so
			// effect a conversion.
			Angle *= ( 180.0 / PI );	// Pi radians = 180 degrees.

			// Tan repeats itself every Pi radians, so add 180 degrees to the angle
			// if it's got a negative value of Delta.y. Special case: If Delta.x is
			// negative, and Delta.y is 0, the angle is 180 degrees.
			if ( Delta.x < 0 )
			{
				// I want the angle to be in the interval ]-180, 180], and so I've
				// got special case handling, depending on which quadrant it lies
				// in initially. Without the if statement here, the interval would
				// be ]-90, 270].
				if ( Angle >= 0.0 )
				{
					Angle -= 180.0;
				}
				else
				{
					Angle += 180.0;
				}
			}
		}

		// More special case stuff. If Delta.x is 0, then the angle must be either
		// +90 or -90 degrees. And we can get the sign of the angle from the sign
		// of Delta.y
		else if ( Delta.y < 0 )
		{
			Angle = -90.0;
		}

		// AI uses points as a co-ordinate system - we use millipoints. Dividing
		// the length of the fill by 1,000 will thus return it's value in points.
		Length /= 1000;

		// Write out the appropriate fill record.
		pDC->OutputToken	( _T("1") );				// It is a clipped gradient.
		pDC->OutputToken	( Name );				// The gradient's name.
		pDC->OutputCoord	( StartPoint );			// Start point for the gradient fill.
		pDC->OutputReal		( Angle );				// The angle of the fill from the X-Axis.
		pDC->OutputReal		( Length );				// The length of the matrix.
		pDC->OutputToken	( _T("1 0 0 1 0 0 Bg") );	// Matrix manipulating the fill. (Unused.)
		pDC->OutputNewLine	();

	}	// End of IsGradientFillValidForExport.
}

/********************************************************************************************

>	void AIEPSRenderRegion::OutputFillColour ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/3/00
	Purpose:	If the fill is a gradient fill, this method will invoke the
				WriteGradientFillInstance method, otherwise it will call the base
				EPSRenderRegion OutputFillColour method. This allows Camelot to support
				gradient fills in the AI file format.

				(ChrisG 30/3/2001) - Gradient fill instances should be exported when they 
				are used on an object, not when a fill attribute is found. This way, fills 
				can be applied to multiple objects.

	SeeAlso:	AIEPSRenderRegion::OutputStrokeColour

********************************************************************************************/

void AIEPSRenderRegion::OutputFillColour ()
{
	// Output the actual fill colour.
	EPSRenderRegion::OutputFillColour ();
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::LoadBitmapMatrix ( NodeBitmap	*pNodeBMP )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/4/00
	Inputs:		pNodeBitmap	- A pointer to the bitmap node being exported.
	Returns:	TRUE		- Successful.
				FALSE		- An error has occured.
	Purpose:	This code generates a matrix to position and manipulate a bitmap within
				an AI file. Roughly, it calculates the angle of rotation, followed by the
				skew angle, and creates a matrix using these values.
	See Also:	FlashExportDC::WriteBitmapFill ()

********************************************************************************************/

BOOL AIEPSRenderRegion::LoadBitmapMatrix ( NodeBitmap	*pNodeBMP )
{
	// Step 1:	Extract the co-ordinates of three corners from the bitmap record, and store
	//			them as named co-ordinates within this function. The names are a historical
	//			reference to the original names within the FlashExportDC, and refer to the
	//			positions of a bitmap fill.
	double			Angle		= pNodeBMP->GetRotationAngle ();

	double			SinRotate	= sin ( Angle );
	double			CosRotate	= cos ( Angle );
//	double			Skew		= 0;

	DocCoord		LowLeft		= pNodeBMP->Parallel [3];	// Low corner of the bitmap.
	DocCoord		TopLeft		= pNodeBMP->Parallel [0];	// Max Y corner.
	DocCoord		LowRight	= pNodeBMP->Parallel [2];	// Max X corner.

	// Step 2:	Calculate the normalised dot product of the two vectors, and this forms
	//			the cosine of their internal angle.
	double			WidthX		= static_cast<double> ( LowLeft.x - LowRight.x ) / 1000;
	double			WidthY		= static_cast<double> ( LowLeft.y - LowRight.y ) / 1000;
	double			HeightX		= static_cast<double> ( TopLeft.x - LowLeft.x  ) / 1000;
	double			HeightY		= static_cast<double> ( TopLeft.y - LowLeft.y  ) / 1000;
	
	// Get the lengths of these vectors to normalise the dot-product. (Necessary for the
	// trignometric function calculations.)
	double			Width		= sqrt ( ( WidthX * WidthX ) + ( WidthY * WidthY ) );
	double			Height		= sqrt ( ( HeightX * HeightX ) + ( HeightY * HeightY ) );
/*
	// The dot product calculation. By normalising the result, the cosine of the angle is
	// calculated.
	double		CosSkew	= ( ( WidthX * HeightX ) + ( HeightY * WidthY ) ) /
							  ( Height * Width );
	double		SinSkew	= sqrt ( 1 - ( CosSkew * CosSkew ) );

	// Multiply the width of the shape by the sine derived from the dot product. This will
	// give the actual width, and not the one distorted by skewing.
	Width *= SinSkew;

	// If the DotSine value is not 0, calculate the tan value that forms the skew angle.
	// Since tan is undefined for 0, I'm ignoring this state.
	if ( SinSkew != 0 )
	{
		Skew = - CosSkew / SinSkew;
	}
*/
	// Grab a reference to the bitmap, and pull it's height and width from it.
	KernelBitmap	*pBitmap	= pNodeBMP->GetBitmap ();	// A reference to the bitmap.

	double			BMPWidth	= static_cast<double> ( pBitmap->GetWidth () );
	double			BMPHeight	= static_cast<double> ( pBitmap->GetHeight () );

	// Catch BMPWidth or BMPHeight set to zero. This shouldn't happen...
	if ( BMPWidth == 0 )
		BMPWidth = 1;

	if ( BMPHeight == 0 )
		BMPHeight = 1;

	// Step 4:	Use the above values to calculate the values with which to populate the
	//			transformation matrix.
	double			ScaleX		= Width  / BMPWidth;
	double			ScaleY		= Height / BMPHeight;

	// Step 5:	Use these to build the matrix. The variables a, b, c, d refer to the AI
	//			matrix entries. The form of the matrix is:
	//
	//			| SinRotate		CosRotate | | ScaleX		0		|
	//			| -CosRotate	SinRotate | | Skew * ScaleY	ScaleY	|

	m_a	= ( SinRotate * ScaleX );	// + ( CosRotate * Skew * ScaleY );
	m_b	= ( - CosRotate * ScaleX );	// + ( SinRotate * Skew * ScaleY );
	m_c	= CosRotate * ScaleY;
	m_d	= SinRotate * ScaleY;

	// There's a weird bug with the AI export, in that a small (>0.0001) value is misread
	// by Adobe Illustrator as a monstrous value. To get around this, I'm going to set all
	// near zero values to zero.
	if ( Absol ( m_a ) < 1e-4 )
	{
		m_a = 0;
	}

	if ( Absol ( m_b ) < 1e-4 )
	{
		m_b = 0;
	}

	if ( Absol ( m_c ) < 1e-4 )
	{
		m_c = 0;
	}

	if ( Absol ( m_d ) < 1e-4 )
	{
		m_d = 0;
	}

	// Step 6:	Calculate the translation part of the matrix.
	m_T = TopLeft;

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::LoadTranslationMatrix ( DocCoord &Translation )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	Loads the matrix with:

				| 1 0 Translation.x |
				| 0 1 Translation.y |

				Which can then be written with WriteLoadedMatrix.
	SeeAlso:	AIESPRenderRegion::LoadBitmapMatrix, AIEPSRenderRegion::WriteLoadedMatrix

********************************************************************************************/

BOOL AIEPSRenderRegion::LoadTranslationMatrix ( DocCoord &Translation )
{
	// Set the transformation part of the matrix to be the identity matrix.
	m_a	= 1;
	m_b = 0;
	m_c = 0;
	m_d = 1;

	// And load up the translation value.
	m_T	= Translation;

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteLoadedMatrix ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	Writes a pre-loaded matrix to the file. I've added a couple of functions to
				create a matrix first, since it'll be more efficient to store the data, and
				re-write it as required, than to recalculate it twice.
	SeeAlso:	AIESPRenderRegion::LoadBitmapMatrix, AIEPSRenderRegion::LoadTranslationMatrix

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteLoadedMatrix ( void )
{
	// Cast the DC pointer to be a KernelDC pointer. This allows it to access useful member
	// functions.
	KernelDC	*pDC	= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );

	// Write the matrix.
	pDC->OutputToken	( _T("[") );
	pDC->OutputReal		( m_a );
	pDC->OutputReal		( m_b );
	pDC->OutputReal		( m_c );
	pDC->OutputReal		( m_d );
	pDC->OutputCoord	( m_T );
	pDC->OutputToken	( _T("]") );

	return TRUE;
}

/********************************************************************************************

>	void AIEPSRenderRegion::WriteMask ( Path *MaskPath, BOOL OutputGroupToken = FALSE )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/4/00
	Inputs:		DrawPath - The path to render.
				OutputGroupToken	whether or not we output a 'q' clipping group token.
	Purpose:	Creates a path that defines a masking pattern. All successive paths (until
				the end of mask operator) will be clipped to this shape.

				I've repeated a lot of code from EPSRenderRegion::ExportPath because I'm
				scared of changing what is a fairly fundamental class, and then finding
				that there are several hundred resultant problems. A lot of the filters
				are pretty shakey at the moment in any case.
	SeeAlso:	RenderRegion::ExportPath

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteMask ( Path *MaskPath, BOOL OutputGroupToken )
{
	KernelDC	*pDC		= static_cast<KernelDC*> ( CCDC::ConvertFromNativeDC(RenderDC) );	// DC for export file.
	DocCoord	*Coords		= MaskPath->GetCoordArray();
	PathVerb	*Verbs		= MaskPath->GetVerbArray();
	INT32			NumCoords	= MaskPath->GetNumCoords();
	INT32			ReadPos		= 1;									// Reading data from.
	BOOL		IsCompound	= FALSE;

	// (ChrisG 16/01/01) Only write masks/clips if there is a path to clip to.
	if (NumCoords != 0)
	{
		if (OutputGroupToken)
		{
			// Start the mask.
			pDC->OutputToken	( _T("q") );
			pDC->OutputNewLine	();
		}

		// Check to see if this is a compound path. If it is, there will be more than one
		// moveto command.
		while ( ReadPos < NumCoords )
		{
			// Find out the type of element that we are over, after the close flag has been
			// removed.
			if ( ( ( Verbs [ReadPos] ) & ( ~PT_CLOSEFIGURE ) ) == PT_MOVETO )
			{
				// This is a compound path - stop searching for moveto's.
				IsCompound = TRUE;

				// Write the tag to indicate that it's a compound path.
				pDC->OutputToken	( _T("*u") );
				pDC->OutputNewLine	();
				break;
			}
			else
			{
				// Try next point
				ReadPos++;
			}
		}

		// Reset to start of path
		ReadPos   = 0;

		// loop through the whole path
		while(ReadPos < NumCoords)
		{
			// Find out the type of element that we are over (after the close flag has been removed)
//			Coord P[4];
			switch ( (Verbs[ReadPos]) & (~PT_CLOSEFIGURE) )
			{
				case PT_MOVETO:
					// If this is in the middle of the path, specify how this sub-path
					// should be rendered - this is needed so that we re-import all the flags
					// correctly on complex paths (e.g. PT_CLOSEFIGURE)
					if (ReadPos > 0)
					{
						// Write out the mask definition.
						WriteMaskTags ();
					}

					// Output the moveto command
					pDC->OutputCoord(Coords[ReadPos]);
					pDC->OutputToken(TEXT("m"));
					pDC->OutputNewLine();
					ReadPos++;
					break;


				case PT_LINETO:
					// Output the lineto command
					pDC->OutputCoord(Coords[ReadPos]);
					pDC->OutputToken(TEXT("l"));
					pDC->OutputNewLine();
					ReadPos++;
					break;


				case PT_BEZIERTO:
					// If this point is a bezier, then the next 2 points should be beziers to
					ENSURE((Verbs[ReadPos+1]) & (~PT_CLOSEFIGURE), "Bezier found with 1 point");
					ENSURE((Verbs[ReadPos+2]) & (~PT_CLOSEFIGURE), "Bezier found with 2 points");
					
					// Make sure that this is not at the start of the path
					ENSURE(ReadPos > 0, "Broken path found while exporting EPS" );

					// Output the moveto command
					pDC->OutputCoord(Coords[ReadPos]);
					pDC->OutputCoord(Coords[ReadPos+1]);
					pDC->OutputCoord(Coords[ReadPos+2]);
					pDC->OutputToken(TEXT("c"));
					pDC->OutputNewLine();
					ReadPos += 3;
					break;

				default:
					ENSURE( FALSE, "We found a Path Element that does not exist!" );
					break;
			}
		}

		// Do the final (or possibly only) render command
		WriteMaskTags ();

		if ( IsCompound )
		{

			// End compound path
			pDC->OutputToken(_T("*U"));
			pDC->OutputNewLine();
		}

		// Wrap up the mask.
		pDC->OutputToken	( _T("0 O") );
		pDC->OutputNewLine	();

	}	// End of if there is a clip path.

	return TRUE;
}

/********************************************************************************************

>	void AIEPSRenderRegion::WriteMaskTags ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/00
	Returns:	TRUE	- Success.
				FALSE	- Error.
	Purpose:	Writes out the tags at the end of each block of a mask's definition.
	SeeAlso:	RenderRegion::ExportPath

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteMaskTags ()
{
	KernelDC	*pDC	= static_cast<KernelDC*> ( CCDC::ConvertFromNativeDC(RenderDC) );	// DC for export file.

	// Write out the mask definition tags.
	pDC->OutputToken	( _T("h") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("W") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("n") );
	pDC->OutputNewLine	();

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteBitmapRecord ( OILBitmap	*pBitmap )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/00
	Inputs:		pBitmap	- A pointer to the OIL bitmap object being rendered.
	Returns:	TRUE	- If successful.
				FALSE	- If an error is encountered.
	Purpose:	Creates a kernel bitmap instance within the AI file.
	SeeAlso:	NodeBitmap::ExportRender ()

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteBitmapRecord ( OILBitmap	*pBitmap )
{
	// Set up the local variables.
//	KernelDC	*pDC		= static_cast<KernelDC*> ( CCDC::ConvertFromNativeDC(RenderDC) );
	INT32		Width		= static_cast<INT32> ( pBitmap->GetWidth () );
	INT32		Height		= static_cast<INT32> ( pBitmap->GetHeight () );

	// Write out the bitmap header.
	WriteBitmapHeader ( Width, Height );

	// Write out the body.
	WriteBitmapBody ( pBitmap, Width, Height );

	// Finish off the bitmap record.
	WriteBitmapTail ();

	// It worked!
	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteBitmapHeader ( UINT32 Width,
												UINT32 Height )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/00
	Inputs:		Width	- The width (in pixels) of the bitmap image.
				Height	- The height (in pixels) of the bitmap image.
	Returns:	TRUE	- If successful.
				FALSE	- If an error is encountered.
	Purpose:	Writes the AI header for a bitmap record.
	SeeAlso:	NodeBitmap::ExportRender ()

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteBitmapHeader ( INT32 Width,
											INT32 Height )
{
	// Set up kernel DC up in an appropriate form.
	KernelDC	*pDC	= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );

	// Declare the bitmap record.
	pDC->OutputToken	( _T("%AI5_File:") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("%AI5_BeginRaster") );
	pDC->OutputNewLine	();

	// (ChrisG 23/11/00) The Xh operator has been removed, as it wasn't correct (it didn't 
	//	have the third, unknown, value), it isn't in the Illustrator File Format Manual, 
	//	and all the information in it seems to be duplicated in the standard XI operator, 
	//	which is written out immediately after the Xh one was.

	// The matrix.
//	WriteLoadedMatrix ();

	// Height and width. (and an extra value - as there's no mention of this token in the
	//	Illustrator file format manual, I have no idea what it signifies, but Photoshop 
	//	needs it, or else it gives MPS Parser Errors).
//	pDC->OutputValue	( Width );
//	pDC->OutputValue	( Height );
//	pDC->OutputValue	( 0l );
//	pDC->OutputToken	( _T("Xh") );
//	pDC->OutputNewLine	();


	// Write out the XI operator.

	// The matrix.
	WriteLoadedMatrix ();

	// The bounds of the bitmap.
	pDC->OutputToken	( _T("0 0") );
	pDC->OutputValue	( Width );
	pDC->OutputValue	( Height );

	// Height and width. Note: The AI documentation puts their order the other
	// way around, but the AI export from Fireworks uses this order.
	pDC->OutputValue	( Width );
	pDC->OutputValue	( Height );

	// These values are always constant, and so I don't need to worry about
	// them changing.
	pDC->OutputToken	( _T("8 3 0 0 0 0") );
	pDC->OutputNewLine	();

	// Write out the file size.
	pDC->OutputToken	( _T("%%BeginData:") );
	pDC->OutputValue	( static_cast<INT32> ( Height * Width * 3 ) );
	pDC->OutputNewLine	();

	// Wrap out the XI operator.
	pDC->OutputToken	( _T("XI") );
	pDC->OutputNewLine	();
 
	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteBitmapBody ( OILBitmap	*pBitmap,
											  INT32		Width,
											  INT32		Height )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/00
	Inputs:		pBitmap	- A pointer to the OIL bitmap object being rendered.
				Width	- The width of the bitmap in pixels.
				Height	- The height of the bitmap in pixels.
	Returns:	TRUE	- If successful.
				FALSE	- If an error is encountered.
	Purpose:	Writes the bitmap's data to the file.
	SeeAlso:	NodeBitmap::ExportRender ()

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteBitmapBody ( OILBitmap	*pBitmap,
										  INT32		Width,
										  INT32		Height )
{
	// Set up the local variables.
	KernelDC	*pDC	= static_cast<KernelDC*> ( CCDC::ConvertFromNativeDC(RenderDC) );
	INT32			Count	= 0;

	// Write the initial %.
	pDC->OutputToken	( _T("%") );

	// Write out the bitmap data. It's necessary to go backwards through y
	// otherwise the bitmap is rendered upside down.
	for ( INT32 y = Height - 1; y >= 0; y-- )
	{
		for ( INT32 x = 0; x < Width; x++ )
		{
			Pixel32bpp	BMPPixel	= pBitmap->ReadPixel32bpp ( x, y );
			TCHAR		Output [7];

			// Wrap the line to keep the file tidy.
			if ( Count >= 10 )
			{
				pDC->OutputNewLine	();
				pDC->OutputToken	( _T("%") );
				Count = 0;
			}

			// Translate the RGB values into an ASCII string.
			camSprintf ( Output, _T("%.2X%.2X%.2X"), BMPPixel.Red, BMPPixel.Green, BMPPixel.Blue );

			// And output them. I'm writing directly to the file because the OutputToken ()
			// function introduces spaces between the values, which is not a good thing for
			// the AI file format. This way, it's much happier. :)
			if ( ExportFile->write ( Output, 6 ).fail () )
			{
				// There's been a problem.
				return FALSE;
			}

			Count ++;
		}
	}

	// Write a new line after the bitmap record, to keep things tidy.
	pDC->OutputNewLine	();

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteContoneBody ( OILBitmap	*pBitmap )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/00
	Inputs:		pBitmap	- A pointer to the OIL bitmap object being rendered.
	Returns:	TRUE	- If successful.
				FALSE	- If an error is encountered.
	Purpose:	Writes a contone bitmap to the file as a 32bpp image.
	SeeAlso:	NodeBitmap::ExportRender ()

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteContoneBody ( OILBitmap	*pBitmap )
{
	// Set up the local variables.
	KernelDC	*pDC	= static_cast<KernelDC*> ( CCDC::ConvertFromNativeDC(RenderDC) );
	INT32		Width	= static_cast<INT32> ( pBitmap->GetWidth () );
	INT32		Height	= static_cast<INT32> ( pBitmap->GetHeight () );
	INT32			Count	= 0;

	// Write the initial %.
	pDC->OutputToken	( _T("%") );

	// Write out the bitmap data. It's necessary to go backwards through y
	// otherwise the bitmap is rendered upside down.
	for ( INT32 y = Height - 1; y >= 0; y-- )
	{
		for ( INT32 x = 0; x < Width; x++ )
		{
			UINT32			Index		= pBitmap->ReadPixelGreyscale ( x, y );
			DocColour		PixelColour	= pBitmap->GetContonePaletteEntry ( Index );
			TCHAR			Output [7];
			INT32			lRed;
			INT32			lGreen;
			INT32			lBlue;

			// Extract the values from the DocColour.
			PixelColour.GetRGBValue ( &lRed, &lGreen, &lBlue );
			
			// Wrap the line to keep the file tidy.
			if ( Count >= 10 )
			{
				pDC->OutputNewLine	();
				pDC->OutputToken	( _T("%") );
				Count = 0;
			}

			// Translate the RGB values into an ASCII string.
			camSprintf ( Output, _T("%.2X%.2X%.2X"), lRed, lGreen, lBlue );

			// And output them. I'm writing directly to the file because the OutputToken ()
			// function introduces spaces between the values, which is not a good thing for
			// the AI file format. This way, it's much happier. :)
			if ( ExportFile->write ( Output, 6 ).fail () )
			{
				// There's been a problem.
				return FALSE;
			}

			Count ++;
		}
	}

	// Write a new line after the bitmap record, to keep things tidy.
	pDC->OutputNewLine	();

	return TRUE;
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::WriteBitmapTail ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13/4/00
	Inputs:		-
	Returns:	TRUE	- If successful.
				FALSE	- If an error is encountered.
	Purpose:	Wraps up the bitmap instance.
	SeeAlso:	NodeBitmap::ExportRender ()

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteBitmapTail ( void )
{
	// Set up kernel DC up in an appropriate form.
	KernelDC	*pDC	= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );

	// Wrap up the bitmap record.
	pDC->OutputToken	( _T("%%EndData") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("XH") );
	pDC->OutputNewLine	();
	pDC->OutputToken	( _T("%AI5_EndRaster") );
	pDC->OutputNewLine	();

	// ChrisG (27/10/00).
	// This token was changed from 'N' close unfilled path, to 'F' close filled path. The  
	//	latter preserves information about the current line width, join type, fill colour, 
	//	etc... Whereas the 'N' token clears all this info, so any text objects or paths which
	//	follow the bitmap were picking up colour and line info for an unfilled/unstroked path,
	//	rather than the current attributes.
	pDC->OutputToken	( _T("F") );
	pDC->OutputNewLine	();

	// The colours are no longer valid, as the bitmap definition has invalidated them.
	m_bValidPathAttrs = FALSE;
	m_bValidTextAttrs = FALSE;

	return TRUE;
}


/********************************************************************************************

>	void AIEPSRenderRegion::OutputWindingRule ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/00
	Purpose:	Output the winding rule used to AI EPS

********************************************************************************************/

void AIEPSRenderRegion::OutputWindingRule ()
{
	// Set up kernel DC up in an appropriate form.
	KernelDC	*pDC	= static_cast<KernelDC *> ( CCDC::ConvertFromNativeDC(RenderDC) );

	if (RR_WINDINGRULE() == EvenOddWinding)
	{
		// Even-Odd winding (Camelot's default)
		pDC->OutputValue ((INT32)1);
	}
	else if (RR_WINDINGRULE() == NonZeroWinding)
	{
		// Non-Zero winding (EPS's default)
		pDC->OutputValue ((INT32)0);
	}
	else
	{
		// Positive or negative winding (only Even-Odd and Non-Zero are supported in AI7),
		//	so assume that we're using camelot's default rule.
		pDC->OutputValue ((INT32)1);
	}

	pDC->OutputToken (_T("XR"));
	pDC->OutputNewLine ();
}



/********************************************************************************************

>	void AIEPSRenderRegion::OutputLineWidth()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the standard line width token and measure for AI EPS

********************************************************************************************/

void AIEPSRenderRegion::OutputLineWidth()
{
	TCHAR buf [64];

	// Set line width
	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();
		camSprintf (buf, _T("%.2f w\n"), ((double) RR_LINEWIDTH())/1000);
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputLineWidth ();
}


/********************************************************************************************

>	void AIEPSRenderRegion::OutputJoinType()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the standard AI EPS join type

********************************************************************************************/

void AIEPSRenderRegion::OutputJoinType()
{
	TCHAR buf [64];

	// Set line Join Type
	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();
		camSprintf (buf, _T("%d j\n"), (UINT32)RR_JOINTYPE());
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputJoinType ();
}



/********************************************************************************************

>	void AIEPSRenderRegion::OutputDashPattern()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Outputs EPS dash pattern values. This function currently only exists so that
				extra handling can be easily achieved for Illustrator EPS export of dash 
				patterns in the future (if need be).
	SeeAlso:	EPSRenderRegion::OutputDashPattern ();
				EPSRenderRegion::OutputDashPatternInit();

********************************************************************************************/

void AIEPSRenderRegion::OutputDashPattern()
{
	EPSRenderRegion::OutputDashPattern ();
}


/********************************************************************************************

>	void AIEPSRenderRegion::OutputStartCap()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the start cap setting using the usual 'J' EPS token

********************************************************************************************/

void AIEPSRenderRegion::OutputStartCap()
{
	TCHAR buf [64];

	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();
		camSprintf (buf, _T("%d J\n"), (UINT32)RR_STARTCAP());
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputStartCap ();
}


/********************************************************************************************

>	void AIEPSRenderRegion::OutputMitreLimit()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the current mitre limit value using the usual 'M' EPS token.
				Again, this is only here so that the AI export can be easily expanded or
				modified.

********************************************************************************************/

void AIEPSRenderRegion::OutputMitreLimit()
{
	EPSRenderRegion::OutputMitreLimit ();
}





/********************************************************************************************

>	void AIEPSRenderRegion::OutputStrokeColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the current stroke colour. This function will use one of the 
				following EPS tokens
				K - if not separating and colour is unnamed
				X - if not separating and colour is named
				G - if separating
	SeeAlso:	AIEPSRenderRegion::OutputFillColour

********************************************************************************************/

void AIEPSRenderRegion::OutputStrokeColour()
{
	// Write out the stroke colour
	EPSRenderRegion::OutputStrokeColour ();
}








/********************************************************************************************

>	void AIEPSRenderRegion::OutputFontName()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output an Illustrator format fontname and pointsize token. The format is	
				/_fontname[-Bold|-Italic|-BoldItalic] pointsize Tf.
				EncodeFontName will generate this format from an internal fontname.
				DocodeFontName will return an internal format fontname and flags where nec.

********************************************************************************************/

void AIEPSRenderRegion::OutputFontName()
{
	if (ExportingOnPath ())
	{
		TCHAR buffer [64];

		OverflowTextStartGap ();

		String_64 FontName;
		String_64 EncodedFontName;
		String_64 Append(_T("/_"));

		// get information about the current font
		FONTMANAGER->GetFontName(RR_TXTFONTTYPEFACE(), FontName);

		// Graeme (31-3-00) - I've lifted this piece of code from the CamelotEPSRenderRegion. It
		// appears to map an existing, encoded font name onto its Postscript counterpart.
		FONTMANAGER->EncodeAndMapFontName(FontName, EncodedFontName, GetFontStyle());
		
		EncodedFontName.Insert(Append,0);

		// Output the fontsize next
		double PointSize = ((double)RR_TXTFONTSIZE())/1000;
		
		// finally do output the font token
		camSprintf (buffer, _T("%s %.1f Tf\n"), (TCHAR *)EncodedFontName, PointSize);
		OverflowTextWrite (buffer);
	}
	EPSRenderRegion::OutputFontName ();
}



/********************************************************************************************

>	void AIEPSRenderRegion::OutputTextRenderMode()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the text's Render Mode (Tr) - this happens when the 
				text's fill or line colour changes to or from blank, see-through colour

********************************************************************************************/

void AIEPSRenderRegion::OutputTextRenderMode ()
{
	INT32 Style=0;

	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();

		// Is there a currently active fill colour?
		if (! (RR_FILLCOLOUR().IsTransparent()) )
			Style+=1;

		// Is there a currently active line colour?
		if (! (RR_STROKECOLOUR().IsTransparent()) )
			Style+=2;

		switch (Style)
		{
			case 0: OverflowTextWrite (_T("3 Tr\n"));	// Invisible
					break;
			case 1: OverflowTextWrite (_T("0 Tr\n"));	// filled only
					break;
			case 2: OverflowTextWrite (_T("1 Tr\n"));	// stroked only
					break;
			case 3: OverflowTextWrite (_T("2 Tr"));	// filled and stroked
					break;
		}
	}

	EPSRenderRegion::OutputTextRenderMode ();
}



/********************************************************************************************

>	void AIEPSRenderRegion::OutputTextAspectRatio()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the text's aspect ratio (Tz)

********************************************************************************************/

void AIEPSRenderRegion::OutputTextAspectRatio ()
{
	TCHAR buf [64];

	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();
		camSprintf (buf, _T("%.0f Tz\n"), (RR_TXTASPECTRATIO().MakeDouble()*100.0));	// convert from ratio to %
   		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputTextAspectRatio ();
}



/********************************************************************************************

>	void AIEPSRenderRegion::OutputTextTracking()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the tracking (Tt)

********************************************************************************************/

void AIEPSRenderRegion::OutputTextTracking ()
{
	// Must be output in 1/1000 of an em.
	// 1 em = point size of font.
	// Tracking internally =millipoints.
	TCHAR buf [64];

	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();
		camSprintf (buf, _T("%d Tt\n"), RR_TXTTRACKING());
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputTextTracking ();
}



/********************************************************************************************

>	void EPSRenderRegion::OutputTextJustification()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the text justification (Ta)

********************************************************************************************/

void AIEPSRenderRegion::OutputTextJustification ()
{
	TCHAR buf [64];

	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();

		switch (RR_TXTJUSTIFICATION())
   		{
   		case JLEFT: 
   			camSprintf (buf, _T("%d Ta\n"), ((INT32)0));
			break;
		case JRIGHT: 
   			camSprintf (buf, _T("%d Ta\n"), ((INT32)2));
			break;
		case JCENTRE: 
   			camSprintf (buf, _T("%d Ta\n"), ((INT32)1));
			break;
		case JFULL: 
   			camSprintf (buf, _T("%d Ta\n"), ((INT32)3));
			break;
		}
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputTextJustification ();
}

/********************************************************************************************

>	void AIEPSRenderRegion::OutputTextLineSpacing()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the line spacing (Tl)

********************************************************************************************/

void AIEPSRenderRegion::OutputTextLineSpacing ()
{

	if (ExportingOnPath ())
	{
		TCHAR buf [64];

		OverflowTextStartGap ();

		// Output line spacing in points.
		// format - paraspace linespace Tl
		double ptLineSpace;
		double ptParaSpace=0;

		TxtLineSpaceAttribute* pLineSpace = (TxtLineSpaceAttribute*)(CurrentAttrs[ATTR_TXTLINESPACE].pAttr);

		// There are some rules for reading the linespacing value which I shall divulge
		// If IsARation is true then use the proportinal linespacing value.
		// else use the absolute linespacing
		// However if the absolute linespacing is zero, then we MUST use the proportional
		// linespacing. Eeek!
		// ie it is an error if (absolute==0 && !IsARatio()) which we will check for here

		double FontSize = (double)RR_TXTFONTSIZE();			    	// in millipoints	 12pt = 12000 mp
		double absLineSpace = (double)pLineSpace->Value;
		double proLineSpace = (pLineSpace->Ratio).MakeDouble();

		BOOL Proportional = pLineSpace->IsARatio();

		if (!Proportional && absLineSpace==0)
		{
			ERROR3("Absolute line spacing is zero yet IsARatio() is FALSE, in GetValidTextAttributes()");
			// Make sure we use the proportional value no matter what.
			Proportional=TRUE;
		}

		if (Proportional)
			ptLineSpace = FontSize*proLineSpace/1000;
		else
			ptLineSpace = absLineSpace/1000;

		camSprintf (buf, _T("%.0f %.0f Tl\n"), ptLineSpace, ptParaSpace);
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputTextLineSpacing ();
}


/********************************************************************************************

>	void AIEPSRenderRegion::OutputTextBaselineShift()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in Baseline shift (Ts)

********************************************************************************************/

void AIEPSRenderRegion::OutputTextBaselineShift ()
{
	TCHAR buf [64];

	// Output baseline shift in points
	// format = rise Ts
	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();
		double BaseLine = ((double)RR_TXTBASELINE())/1000;
		camSprintf (buf, _T("%.1f Ts\n"), BaseLine);
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputTextBaselineShift ();
}


/********************************************************************************************

>	void AIEPSRenderRegion::OutputTextSubSuperScript()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the Sub/Super script attribute (Ts)

********************************************************************************************/

void AIEPSRenderRegion::OutputTextSubSuperScript ()
{
	TCHAR buf [64];
	
	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();

		// Output sub/superscript
		// in millipoints	 12pt = 12000 mp
		double FontSize = ((double)RR_TXTFONTSIZE())/1000;

		TxtScriptAttribute* pScript = RR_TXTSCRIPT();
		double offset = (pScript->Offset).MakeDouble();
		double size = (pScript->Size).MakeDouble();

		OverflowTextWrite (_T("%%XSScript\n"));

		double rise = FontSize*offset;

		camSprintf (buf, _T("%.1f Ts\n"), rise);
		OverflowTextWrite (buf);

		double ptsize = FontSize*size;

		String_64 MappedFont;
		String_64 Append(_T("/_"));

		String_64 FontName;
		FONTMANAGER->GetFontName(RR_TXTFONTTYPEFACE(), FontName);

		// Graeme (31-3-00) - Map the encoded name onto the PS font name.
		FONTMANAGER->EncodeAndMapFontName(FontName,MappedFont,GetFontStyle());

		// Graeme (14-6-00) - I should add ascent and descent values, but Camelot doesn't
		// seem to store them anywhere.
		MappedFont.Insert(Append,0);

		camSprintf (buf, _T("%s %.1f Tf\n"),(TCHAR *)MappedFont, ptsize);
		// Output the ascent.
		// Output the descent.
		OverflowTextWrite (buf);
	}

	EPSRenderRegion::OutputTextSubSuperScript ();
}

/*------------*/



//----------------------------------------------------------------
//
//	The following functions are all overflow text functions,
//	which write an additional text line, terminated with the 
//	token (TX), to the EPS file, the Output functions declared
//	earlier should call these functions to add any attribute 
//	changes, so that the two text blocks remain consistent.
//
//----------------------------------------------------------------


/********************************************************************************************

>	void AIEPSRenderRegion::ExportingOnPath()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/00
	Returns:	TRUE - yes, we are exporting on a path
				FALSE - no, we aren't exporting on a path
	Purpose:	To determine whether this text is being exported on a path.
	SeeAlso:	

********************************************************************************************/

BOOL AIEPSRenderRegion::ExportingOnPath ()
{
	if (m_fpOverflowText)
		return TRUE;
	else
		return FALSE;
}


/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextStart()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Start the overflow text block.
	SeeAlso:	

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextStart ()
{
		// open overflow file
	m_fpOverflowText = fopen ("OverFlow.txt", "w");

	// write out start info - NOTE: we're currently in a gap from writing the initial 
	//	attributes, so we're going to have to close it.
	m_bInTextGap = TRUE;
}


/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextFinish()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Finish the overflow text block and write both blocks to the actual EPS file.
	SeeAlso:	

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextFinish ()
{
	ASSERT (m_fpOverflowText);

		// write out final information.
	OverflowTextStartGap ();

		// close files.
	fclose (m_fpOverflowText);
	m_fpOverflowText = NULL;

	// copy info into actual EPS file.
	char cbuffer [1025];
	TCHAR buffer [1025];
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	FILE * fp;

		// Write out the Extra info.
	fp = fopen ("OverFlow.txt", "r+t");

	while (fgets (cbuffer, 1024, fp) != NULL)
	{
		char c;
		INT32 i=0;
		cbuffer[1024]=0;
		do
		{
			c=cbuffer[i];
			buffer[i]=c; // 1:1 CHAR->TCHAR conversion
			i++;
		} while (c);
			
		// deal with newlines.
		pDC->OutputToken (buffer);
		pDC->OutputNewLine ();
	}
	fclose (fp);


		// clean up temp. files
	remove ("OverFlow.txt");
}

/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextStartGap()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/00
	Purpose:	Starts a new gap in the Overflow text, so that attributes can be written into
				the overflow blocks (the TX ones)
	SeeAlso:	AIEPSRenderRegion::OverflowTextFinishGap ()

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextStartGap ()
{
	ASSERT (ExportingOnPath ());

	// if we're not in a gap, finish writing the old TX block and start a new gap, 
	if (m_bInTextGap == FALSE)
	{
		OverflowTextWrite (_T(") TX\n"));
		m_bInTextGap = TRUE;
	}
}

/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextFinishGap()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/11/00
	Purpose:	Closes a gap for writing attributes in the overflow text, so that a new line 
				of overflow text can be started.
	SeeAlso:	AIEPSRenderRegion::OverflowTextStartGap ();

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextFinishGap ()
{
	ASSERT (ExportingOnPath ());

	// if we're in a text gap (for changing the attributes), start writing a new TX block.
	if (m_bInTextGap == TRUE)
	{
		OverflowTextWrite (_T("("));
		m_bInTextGap = FALSE;
	}
}


/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextWrite()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Write some text to the overflow body file. This will store all the text and
				modified attributes for the TX text block.
	SeeAlso:	

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextWrite (TCHAR * text)
{
	ASSERT (ExportingOnPath ());
	while (*text)
	{
		fprintf (m_fpOverflowText, "%c", *text++);
	}
}


/********************************************************************************************

>	BOOL AIEPSRenderRegion::OverflowTextWriteSingleColour (UINT32 n)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/00
	Inputs:		n - the colour value (range 0-255) to write to the EPS file.
	Purpose:	Output a colour value to the overflow text file.  A 'single colour' is a value
				as used in the Camelot 'Colour' class, i.e. in the range 0 to 255.
				This range is converted to the range 0.0 to 1.0, and output to the file.
	SeeAlso:	KernelDC::OutputColourValue

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextWriteSingleColour (UINT32 n)
{
	// Output to string
	TCHAR buf[20];

	// Convert to points, getting integer and fractional parts
	camSprintf (buf, _T("%.2f "), ((double) n) / 255);
	OverflowTextWrite (buf);
}

/********************************************************************************************

>	BOOL AIEPSRenderRegion::OverflowTextWriteColour(PColourCMYK *pCol)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/11/00
	Inputs:		Col - the CMYK colour values to be output.
	Purpose:	Output a colour, as expressed by the Camelot 'Colour' class.  This function
				takes the CMYK variation, is this is most appropriate to EPS files.
				Camelot colour values are converted from the 0-255 range to the 0.0-1.0
				range before being output.

				e.g.
				MonoOn
					PColourCMYK Col = { 255, 255, 128, 0 };
					pDC->OutputColour(&Col);
				MonoOff
				will give the following output:
				MonoOn
					1.0 1.0 0.5 0.0
				MonoOff
	SeeAlso:	KernelDC::OutputColourValue
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextWriteColour (PColourCMYK *pCol)
{
	// Write each of the colour values.
	OverflowTextWriteSingleColour (pCol->Cyan);
	OverflowTextWriteSingleColour (pCol->Magenta);
	OverflowTextWriteSingleColour (pCol->Yellow);
	OverflowTextWriteSingleColour (pCol->Key);
}

/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextWriteNamedColour	(DocColour *pCol, 
															ColourContext* pContext = NULL)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/08/94
	Inputs:		pCol - the colour to output.
				pContext - the context to use to convert the colour before output.
						   if this is NULL the default CMYK context will be used.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Similar to OutputColour(), except it outputs the colour name and tint
				of the colour as well.  If pCol does not reference an indexed colour,
				then the name "NoName" is used.
	SeeAlso:	KernelDC::OutputColour
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextWriteNamedColour(DocColour *pCol, ColourContext* pContext)
{
	// Get CMYK version of this colour.
	PColourCMYK CMYK;
	pCol->GetCMYKValue(pContext, &CMYK);

	// Output CMYK version
	OverflowTextWriteColour (&CMYK);
		
	// Get the indexed colour from the DocColour.
	IndexedColour *pIndCol = pCol->FindParentIndexedColour();

	// Cope with the unexpected!
//	ENSURE(pIndCol != NULL, "Named colour has no index colour!");

	if (pIndCol == NULL)
	{
		if (pCol->IsTransparent())
		{
			// This is a 'no colour' type colour, so output a zero-length colour name,
			// as this is the only way we can handle this at the moment.
			return;
		}
		else
		{
			// Otherwise make up a colour name (see epsfiltr.h).
			OverflowTextWrite (_T("("));
			OverflowTextWrite (ImmediateColourFudgeyBodgeName);
			OverflowTextWrite (_T(")"));
		}
	}
	else
	{
		// Got an indexed colour - output its name
		// (Pass in TRUE to get a unique-identifier for local colours rather than "Local colour")
		String_64 *ColName = pIndCol->GetName(TRUE);
		OverflowTextWrite (_T("("));
		OverflowTextWrite ((TCHAR *) (*ColName));
		OverflowTextWrite (_T(")"));
	}

	// Always tint 0
	OverflowTextWrite (_T(" 0 "));
}

/********************************************************************************************

>	void AIEPSRenderRegion::OverflowTextWriteColourName (DocColour *pCol)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/00
	Inputs:		pCol - the colour to output.
	Purpose:	Outputs a named colour's name as a string. If pCol does not reference an 
				indexed colour, then the name "NoName" is used. This was seperated from 
				OutputNamedColour so that the DeviceContext will not need to worry about
				which ColourModel to use (CMYK, RGB, etc...), as this will be determined 
				by the RenderRegion.
	SeeAlso:	KernelDC::OutputNamedColour
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

void AIEPSRenderRegion::OverflowTextWriteColourName (DocColour *pCol)
{
	// Get the indexed colour from the DocColour.
	IndexedColour *pIndCol = pCol->FindParentIndexedColour();

	if (pIndCol == NULL)
	{
		if (pCol->IsTransparent())
		{
			// This is a 'no colour' type colour, so output a zero-length colour name,
			// as this is the only way we can handle this at the moment.
			OverflowTextWrite (_T("()"));
		}
		else
		{
			// Otherwise make up a colour name (see epsfiltr.h).
			OverflowTextWrite (_T("("));
			OverflowTextWrite (ImmediateColourFudgeyBodgeName);
			OverflowTextWrite (_T(")"));
		}
	}
	else
	{
		// Got an indexed colour - output its name
		// (Pass in TRUE to get a unique-identifier for local colours rather than "Local colour")
		String_64 *ColName = pIndCol->GetName(TRUE);
		OverflowTextWrite (_T("("));
		OverflowTextWrite ((TCHAR *) (*ColName));
		OverflowTextWrite (_T(")"));
	}
}





/********************************************************************************************

>	virtual void AIEPSRenderRegion::SetClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8th November 2000
	Inputs:		pClipAttr	ptr to the new ClipRegionAttribute to set as current in our RR.
				Temp		whether the attr is temporary or not.

	Outputs:	Postscript's context is saved and the commands are output to intersect its
				current clipping path with that defined by the ClipAttr.
				Also, the RR's current attr context is updated to use this new ClipAttr.

	Purpose:	The Adobe Illustrator version of Karim's EPSRenderRegion::SetClipRegion,
				This was needed as Illustrator uses non-standard EPS commands
				
				Shrink the RR's clipping region to the region defined by pClipAttr.
				We do this by:
					1.	Recording a save-state command.
					2.	Exporting a description of the clipping path in pClipAttr.
					3.	Recording a 'clip-to-last-path' command.
					4.	Recording a 'start-new-path' command, as we don't want our clip-path
						to get stroked or rendered in any way.

	Notes:		We mustn't record clipping commands when rendering the default attribute.
				Therefore this method does *nothing whatsoever* unless pClipAttr's ptr to
				its clipping-path is non-NULL. This ptr is NULL in the default attr.

	See also:	RenderRegion::SetClipRegion(), EPSRenderRegion::SetClipRegion()

********************************************************************************************/
void AIEPSRenderRegion::SetClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)
{
	if (pClipAttr->GetClipPath() != NULL)
	{
		// Update the RR's table of current attributes.
		RenderRegion::SetClipRegion (pClipAttr, Temp);

		// Write out the clipping path as a mask.
		WriteMask(pClipAttr->GetClipPath(), pClipAttr->IsResponsibleForGrouping ());
	}
}



/********************************************************************************************

>	virtual void AIEPSRenderRegion::RestoreClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8th November 2000
	Inputs:		pClipAttr	ptr to the ClipRegionAttribute to restore over the current one.
				Temp		whether the attr is temporary or not.

	Outputs:	Postscript is told to restore its last saved context.

	Purpose:	Adobe Illustrator version of Karim's EPSRenderRegion::RestoreClipRegion.
				See SetClipRegion for why this was necessary.
	
				Restore the RR's clipping region.
				We do this by recording a restore-state command.

	Errors:		ERROR3 if the current ClipRegionAttribute holds a NULL clip-path ptr.
				This state of affairs should not happen. You can't set a ClipRegionAttribute
				with a NULL clip-path ptr, so the only such attr is the default one, and you
				should never be restoring over _that_.

	See also:	RenderRegion::RestoreClipRegion(), EPSRenderRegion::RestoreClipRegion()

********************************************************************************************/

void AIEPSRenderRegion::RestoreClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)
{
	// we should never be entered when the current clipattr has a NULL path, as what this
	// means is that either somebody somehow did a SetClipRegion() with such an attr, or
	// somebody is trying to Restore() over the default ClipRegionAttribute (which holds a
	// NULL path ptr). Neither of these actions should occur.

	if (RR_CLIPREGION()->GetClipPath() == NULL)
	{
		ERROR3("EPSRenderRegion::RestoreClipRegion; Current ClipRegionAttribute has a NULL path");
	}
	else
	{
		// Update the RR's table of current attributes.
		RenderRegion::RestoreClipRegion(pClipAttr, Temp);
	}
}


/********************************************************************************************

>	virtual void AIEPSRenderRegion::SetTextAsShapes (BOOL shapes)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8th November 2000
	Inputs:		shapes - whether to export text as shapes (TRUE) or characters (FALSE)

	Purpose:	Sets the export TextAsShapes flag

	See also:	AIEPSRenderRegion::RenderChar(), TextStory::PreExportRender, 
				TextStory::ExportRender

********************************************************************************************/

void AIEPSRenderRegion::SetTextAsShapes (BOOL shapes)
{
	m_bTextAsShapes = shapes;
}


/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::GetTextAsShapes ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8th November 2000
	Returns:	BOOL - whether text is exported as shapes (TRUE) or characters (FALSE)

	Purpose:	Sets the export TextAsShapes flag

	See also:	AIEPSRenderRegion::GetTextAsShapes

********************************************************************************************/

BOOL AIEPSRenderRegion::GetTextAsShapes ()
{
	return m_bTextAsShapes;
}


/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::WriteNewLine ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	9th November 2000
	Returns:	BOOL - whether the a new line was written successfully.

	Purpose:	Writes a new line to the output of text if text is exported as characters, 
				does nothing if it's exported as shapes.

	See also:	EPSRenderRegion::WriteNewLine

********************************************************************************************/

BOOL AIEPSRenderRegion::WriteNewLine ( void )
{
	BOOL success;

	// don't write any newlines if we're exporting text as shapes (cos' it could stuff up 
	//	the eps).
	if (GetTextAsShapes () == TRUE)
	{
		success = TRUE;
	}
	else
	{
		success = EPSRenderRegion::WriteNewLine ();
	}

	return success;
}



/********************************************************************************************

>	virtual void AIEPSRenderRegion::OutputFillRGBColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/00
	Purpose:	Writes additional info to the file for RGB fill colours 
	See also:	EPSRenderRegion::OutputFillRGBColour

********************************************************************************************/

void AIEPSRenderRegion::OutputFillRGBColour ()
{
	// NOTE: We need to export any changes to the overflow text here, 
	// as it's the last place we can catch the colour change.
	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();

		// Colour values.
		INT32 red;
		INT32 green;
		INT32 blue;
		PColourCMYK CMYK;

		// Assume no local context at present - used as a check to make sure that we 
		//	aren't exporting as seperations.
		ColourContext* pContext;
		ColourPlate* pSeparation;
		GetOutputColourPlate (COLOURMODEL_CMYK, &pContext, &pSeparation);

		if (pSeparation==NULL)
		{
			// As expected, we ain't using seperations, so we can output as RGB.

			// Get the current line colour in RGB values and print them out.
			RR_FILLCOLOUR().GetRGBValue (&red, &green, &blue);
			OverflowTextWriteSingleColour (red);
			OverflowTextWriteSingleColour (green);
			OverflowTextWriteSingleColour (blue);

			if (RR_FILLCOLOUR().FindParentIndexedColour() == NULL)
			{
				// Unnamed colour - just add 'Xa' token
				OverflowTextWrite (_T("Xa\n"));
			}
			else
			{
				// Named colour - add Name, tint value, RGB flag and 'Xx' token
				OverflowTextWriteColourName (&(RR_FILLCOLOUR()));
				OverflowTextWrite (_T(" 0 1 Xx\n"));
			}
		}
		else
		{
			// Since we are seperating the colours, then use CMYK mode.

			// Assume unnamed colour as 'g' grey fill operator does
			// not allow for custom greys.
			if (pSeparation->IsMonochrome())
			{
				RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
				BYTE c = 0xFF - CMYK.Key;
				OverflowTextWriteSingleColour (c);
				OverflowTextWrite (_T("g\n"));
			}
			else
			{
				RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
				OverflowTextWriteColour (&CMYK);
				OverflowTextWrite (_T("x\n"));
			}
		}
	}

	// Write out the usual EPS colour tokens.
	EPSRenderRegion::OutputFillRGBColour ();
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::OutputFillCMYKColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/00
	Purpose:	Writes additional info to the file for CMYK fill colours 
	See also:	EPSRenderRegion::OutputFillCMYKColour

********************************************************************************************/

void AIEPSRenderRegion::OutputFillCMYKColour ()
{
	// NOTE: We need to export any changes to the overflow text here, 
	// as it's the last place we can catch the colour change.
	if (ExportingOnPath ())
	{
		PColourCMYK CMYK;

		OverflowTextStartGap ();

		// Assume no local context at present
		ColourContext* pContext;
		ColourPlate* pSeparation;
		GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

		if (pSeparation==NULL)
		{
			// Get the current line colour in CMYK values, unless it's transparent
			// Print out colour component values.
			if (RR_FILLCOLOUR().FindParentIndexedColour() == NULL)
			{
				// Unnamed colour
				// retrieve the colour value
				RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
				OverflowTextWriteColour (&CMYK);
				OverflowTextWrite (_T("k\n"));
			}
			else
			{
				// Named colour
				OverflowTextWriteNamedColour (&(RR_FILLCOLOUR()), pContext);
				OverflowTextWrite (_T("x\n"));
			}
		}
		else
		{
			// Assume unnamed colour as 'g' grey fill operator does
			// not allow for custom greys.
			if (pSeparation->IsMonochrome())
			{
				RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
				BYTE c = 0xFF - CMYK.Key;
				OverflowTextWriteSingleColour (c);
				OverflowTextWrite (_T("g\n"));
			}
			else
			{
				RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
				OverflowTextWriteColour (&CMYK);
				OverflowTextWrite (_T("x\n"));
			}
		}
	}

	// Call the base class to write the ordinary info out
	EPSRenderRegion::OutputFillCMYKColour ();
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::OutputStrokeRGBColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/00
	Purpose:	Writes additional info to the file for RGB stroke colours 
	See also:	EPSRenderRegion::OutputStrokeRGBColour

********************************************************************************************/

void AIEPSRenderRegion::OutputStrokeRGBColour ()
{	
	// NOTE: We need to export any changes to the overflow text here, 
	// as it's the last place we can catch the colour change.
	if (ExportingOnPath ())
	{
		OverflowTextStartGap ();

		// Colour values.
		INT32 red;
		INT32 green;
		INT32 blue;
		PColourCMYK CMYK;

		// Assume no local context at present - used as a check to make sure that we 
		//	aren't exporting as seperations.
		ColourContext* pContext;
		ColourPlate* pSeparation;
		GetOutputColourPlate (COLOURMODEL_CMYK, &pContext, &pSeparation);

		if (pSeparation==NULL)
		{
			// As expected, we ain't using seperations, so we can output as RGB.

			// Get the current line colour in RGB values and print them out.
			RR_STROKECOLOUR().GetRGBValue (&red, &green, &blue);
			OverflowTextWriteSingleColour (red);
			OverflowTextWriteSingleColour (green);
			OverflowTextWriteSingleColour (blue);

			if (RR_STROKECOLOUR().FindParentIndexedColour() == NULL)
			{
				// Unnamed colour - just add 'XA' token
				OverflowTextWrite (_T("XA\n"));
			}
			else
			{
				// Named colour - add Name, tint value, RGB flag and 'XX' token
				OverflowTextWriteColourName (&(RR_STROKECOLOUR()));
				OverflowTextWrite (_T(" 0 1 XX\n"));
			}
		}
		else
		{
			// Since we are seperating the colours, then use CMYK mode.

			// Assume unnamed colour as 'g' grey fill operator does
			// not allow for custom greys.
			if (pSeparation->IsMonochrome())
			{
				RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
				BYTE c = 0xFF - CMYK.Key;
				OverflowTextWriteSingleColour (c);
				OverflowTextWrite (_T("G\n"));
			}
			else
			{
				RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
				OverflowTextWriteColour (&CMYK);
				OverflowTextWrite (_T("X\n"));
			}
		}
	}

	// Call the base class to process the colour normally (i.e. to the EPS file itself)
	EPSRenderRegion::OutputStrokeRGBColour ();
}


/********************************************************************************************

>	virtual void AIEPSRenderRegion::OutputStrokeCMYKColour ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/12/00
	Purpose:	Writes additional info to the file for CMYK fill colours 
	See also:	EPSRenderRegion::OutputFillCMYKColour

********************************************************************************************/

void AIEPSRenderRegion::OutputStrokeCMYKColour ()
{
	// Write out the colour to the overflow file if we're doing text on a path.
	if (ExportingOnPath ())
	{
		PColourCMYK CMYK;
		OverflowTextStartGap ();

		// Assume no local context at present
		ColourContext* pContext;
		ColourPlate* pSeparation;
		GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

		if (pSeparation==NULL)
		{
			// Get the current line colour in CMYK values, unless it's transparent
			// Print out colour component values.
			if (RR_STROKECOLOUR().FindParentIndexedColour() == NULL)
			{
				// Unnamed colour
				// retrieve the colour value
				RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
				OverflowTextWriteColour (&CMYK);
				OverflowTextWrite (_T("K\n"));
			}
			else
			{
				// Named colour
				OverflowTextWriteNamedColour (&(RR_STROKECOLOUR()), pContext);
				OverflowTextWrite (_T("X\n"));
			}
		}
		else
		{
			// Assume unnamed colour as 'g' grey fill operator does
			// not allow for custom greys.
			if (pSeparation->IsMonochrome())
			{
				RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
				BYTE c = 0xFF - CMYK.Key;
				OverflowTextWriteSingleColour (c);
				OverflowTextWrite (_T("G\n"));
			}
			else
			{
				RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
				OverflowTextWriteColour (&CMYK);
				OverflowTextWrite (_T("X\n"));
			}
		}
	}

	// Call the base class to write out the ordinary information
	EPSRenderRegion::OutputStrokeCMYKColour ();
}


/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::IsGradientFillValidForExport ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/01/01
	Purpose:	Determines whether a particular fill can be used through the AI export
	See also:	AIEPSRenderRegion::WriteGradientFills, AIEPSRenderRegion::WriteGradientUsage

********************************************************************************************/
BOOL AIEPSRenderRegion::IsGradientFillValidForExport (FillGeometryAttribute * pFill)
{
	BOOL acceptable = FALSE;

	if ( pFill->IsAColourFill () &&			// Is it a colour fill?
		 pFill->IsAGradFill () &&			// Is it a graduated fill?
		 ! (pFill->IsAFractalFill ()) &&	// Is it not a fractal (clouds) fill?
		 ! (pFill->IsANoiseFill ()) &&		// Is it not a fractal (plasma) fill?
		 ! (pFill->IsABitmapFill ()) )		// Is it not a bitmap fill?
	{
		acceptable = TRUE;
	}
	else
	{
		acceptable = FALSE;
	}

	return acceptable;
}


/********************************************************************************************

>	virtual BOOL AIEPSRenderRegion::IncludeGradientFill ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/2001
	Purpose:	Stores information about a fill so that the same fill isn't declared multiple
				times.
	See also:	AIEPSRenderRegion::WriteGradientFills, AIEPSRenderRegion::WriteGradientUsage,
				AIEPSRenderRegion::FindGradientInCache

********************************************************************************************/
void AIEPSRenderRegion::IncludeGradientFill (FillGeometryAttribute * pFill, EFFECTTYPE effect)
{
	ERROR3IF ((pFill == NULL), "AIEPSRenderRegion::IncludeGradientFill - Attempted to include NULL fill");

	ListFill * pNewList = NULL;
	FillGeometryAttribute * pNewFill = NULL;

	// don't try and store the fill if it's invalid.
	if (pFill != NULL)
	{
		// Create a new FillGeometryAttribute, of the same type as the one passed in.

		//	NOTE: Four colour fills are derived from three colour fills, so they return
		//		TRUE for IsAThreeColFill. This means that we need to process four colour
		//		fills before we process three colour fills, otherwise all four colour fills
		//		will become represented by three colour ones and the copy won't match the
		//		original.

		if (pFill->IsALinearFill ())
			pNewFill = new LinearFillAttribute;
		else if (pFill->IsARadialFill ())
			pNewFill = new RadialFillAttribute;
		else if (pFill->IsAConicalFill ())
			pNewFill = new ConicalFillAttribute;
		else if (pFill->IsASquareFill ())
			pNewFill = new SquareFillAttribute;
		else if (pFill->IsAFourColFill ())
			pNewFill = new FourColFillAttribute;
		else if (pFill->IsAThreeColFill ())
			pNewFill = new ThreeColFillAttribute;
		else if (pFill->IsAFractalFill ())
			pNewFill = new FractalFillAttribute;
		else if (pFill->IsANoiseFill ())
			pNewFill = new NoiseFillAttribute;
		else if (pFill->IsABitmapFill ())
			pNewFill = new BitmapFillAttribute;
		else
		{
			pNewFill = NULL;
			ERROR3 ("AIEPSRenderRegion::IncludeGradientFill - Unrecognised fill included");
		}

		// Copy fill data if the the fill is recognised. There's no point in doing this if
		//	we didn't recognise the fill, as the copy is bound to be different to the original.
		if (pNewFill != NULL)
		{
			// Copy all the relevant info.
			*(pNewFill) = *pFill;
			ERROR3IF ( !(*pFill == *pNewFill), "AIEPSRenderRegion::IncludeGradientFill - Copied fill doesn't match original");

			// New Fill is built, so add it into either the radial or linear list.
			if (pFill->IsARadialFill () || pFill->IsASquareFill ())
			{
				pNewList = new ListFill (pNewFill, effect, m_pRadialGradList);
				m_pRadialGradList = pNewList;
			}
			else
			{
				pNewList = new ListFill (pNewFill, effect, m_pLinearGradList);
				m_pLinearGradList = pNewList;
			}
		}
	}
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::ClearGradientCache ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001

	Purpose:	Deletes the list of stored gradient fills, that was built up using 
				IncludeGradientFill

	See also:	AIEPSRenderRegion::IncludeGradientFill, 
				AIEPSRenderRegion::FindGradientInCache

********************************************************************************************/
void AIEPSRenderRegion::ClearGradientCache ()
{
	ListFill * pList = NULL;

	// only clear it if there is a list to clear
	if (m_pLinearGradList)
	{
		// Cycle through the linear list deleting each item in turn.
		while (m_pLinearGradList != NULL)
		{
			pList = m_pLinearGradList->pNext;
			delete m_pLinearGradList;
			m_pLinearGradList = pList;
		}
	}

	if (m_pRadialGradList)
	{
		// Cycle through the radial list deleting each item in turn.
		while (m_pRadialGradList != NULL)
		{
			pList = m_pRadialGradList->pNext;
			delete m_pRadialGradList;
			m_pRadialGradList = pList;
		}
	}
}


/********************************************************************************************

>	virtual INT32 AIEPSRenderRegion::FindGradientInCache ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001
	Returns		the identifier of the found gradient fill, or 0 if it wasn't found

	Purpose:	Deletes the list of stored gradient fills, that was built up using 
				IncludeGradientFill

	See also:	AIEPSRenderRegion::IncludeGradientFill, 

********************************************************************************************/
INT32 AIEPSRenderRegion::FindGradientInCache (FillGeometryAttribute * pFill, EFFECTTYPE effect)
{
	INT32 id	= 0;
	ListFill * pList = NULL;
	FillGeometryAttribute * pStoredFill = NULL;

	double fillBias = 0;
	CProfileBiasGain fillProfile = pFill->GetProfile ();
	fillBias = (double) fillProfile.GetBias ();

	// work out which list we want to search in.
	if (pFill->IsARadialFill () || pFill->IsASquareFill ())
		pList = m_pRadialGradList;
	else
		pList = m_pLinearGradList;

	// Cycle through all the fills until we find the one we want, or until we run out of fills
	while ((pList != NULL) && (id == 0))
	{
		pStoredFill = pList->pFill;

		// Because the start and end points for an Illustrator fill are not included in the
		//	cache, we don't need to worry about them being the same.
		//
		// So, we set all the start and end points to be the same (since they would cause two
		//	identical fills in different places, or of different lengths to be considered as
		//	different otherwise).
		pStoredFill->SetStartPoint (pFill->GetStartPoint ());
		pStoredFill->SetEndPoint (pFill->GetEndPoint ());
		pStoredFill->SetEndPoint2 (pFill->GetEndPoint2 ());
		pStoredFill->SetEndPoint3 (pFill->GetEndPoint3 ());

		// Now that the fill coords are the same, test to see whether they're 
		//	the same fill, or not.
		if (*pStoredFill == *pFill)
		{
			double storeBias = 0;
			CProfileBiasGain storeProfile = pStoredFill->GetProfile ();
			storeBias = (double) storeProfile.GetBias ();

			if ((storeBias == fillBias) && (pList->effect == effect))
				id = pList->id;
		}

		// Check the next fill
		pList = pList->pNext;
	}

	return id;
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::BuildGradientCache (Node * pStartNode)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2001
	Inputs:		pStartNode - the node to start looking from.
	Purpose:	Builds up a 'cache' of gradient fills.

	See also:	AIEPSRenderRegion::WriteGradientFills, AIEPSRenderRegion::WriteGradientDefinitions

********************************************************************************************/
void AIEPSRenderRegion::BuildGradientCache (Node * pStartNode)
{
//	Node * pChild	= NULL;

	// Set up the gradient searching system
	AIEPSGradientScanRenderRegion scanRR (this);
	scanRR.AttachDevice (DocView::GetSelected (), RenderDC, RenderView->GetDoc()->FindFirstSpread ());
	scanRR.InitDevice ();

	// Although the counting system does return the number of fills found, it can be got from
	//	the scanner just as easily.
	BuildGradientCacheUsingScanner (pStartNode, &scanRR);
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::BuildGradientCacheUsingScanner (Node * pNode,
																	AIEPSGradientScanRenderRegion * pScan)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2001
	Inputs:		pNode - the node to start looking from.
				pScan - the scanner to use
				.
	Purpose:	Builds up the gradient cache by scanning through each node under pNode for new 
				fills

	See also:	AIEPSRenderRegion::WriteGradientFills, AIEPSRenderRegion::WriteGradientDefinitions

********************************************************************************************/
void AIEPSRenderRegion::BuildGradientCacheUsingScanner (Node * pNode, AIEPSGradientScanRenderRegion * pScan)
{
	// Children are rendered first, so that attributes will be applied to the right objects.
	Node * pChild = pNode->FindFirstChild ();
	while (pChild!=NULL)
	{
		BuildGradientCacheUsingScanner (pChild, pScan);
		pChild = pChild->FindNext ();
	}

	// "Render" the node into the scanner. Currently, rendering is the only way to access
	//	the intermediate steps (and their attributes) in blends, contours and some others, 
	//	so we need to use a special Render Region to catch the gradient (fill) information.
	//
	// NOTE: Since this uses the same system as the main render loop, it shouldn't be possible
	//	for the gradient cache and the main file to become out of sync. Also, bevels and 
	//	shadows are excluded, as they are rendered as bitmaps in AI export.
	if (!pNode->IsABevel () && !pNode->IsAShadow ())
		pNode->Render (pScan);
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::WriteGradientCount ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/3/2001
	Purpose:	Writes out the count of all the gradient fills in the Cache

	See also:	AIEPSRenderRegion::WriteGradientDefinitions, 
				AIEPSRenderRegion::BuildGradientCache

********************************************************************************************/
void AIEPSRenderRegion::WriteGradientCount ()
{
	KernelDC *pDC = (KernelDC*)CCDC::ConvertFromNativeDC(RenderDC);
	ListFill * pList	= m_pLinearGradList;
	INT32 numFills		= 0;

	// Cycle through the gradient cache writing out fills as we go.
	//
	//	First count the linear fills
	while (pList != NULL)
	{
		numFills ++;
		pList = pList->pNext;
	}

	// Then count the radial fills.
	pList = m_pRadialGradList;
	while (pList != NULL)
	{
		numFills ++;
		pList = pList->pNext;
	}

	// If there are any fills, then write the count out.
	if (numFills)
	{
		pDC->OutputValue (numFills);
		pDC->OutputToken (_T("Bn"));
		pDC->OutputNewLine ();
	}
}

/********************************************************************************************

>	virtual void AIEPSRenderRegion::WriteGradientDefinitions ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/2001
	Purpose:	Writes out the definitions of all the  gradient fills in the Cache

	See also:	AIEPSRenderRegion::WriteGradientCount, 
				AIEPSRenderRegion::BuildGradientCache

********************************************************************************************/
void AIEPSRenderRegion::WriteGradientDefinitions ()
{
	ListFill *				pList = NULL;
	FillGeometryAttribute * pFill = NULL;

	// Cycle through the gradient cache writing out fills as we go.
	//
	//	First do the linear list.
	pList = m_pLinearGradList;
	while (pList != NULL)
	{
		pFill = pList->pFill;
		WriteLinearFill ( pFill, pList->effect, pList->id );
		pList = pList->pNext;
	}

	// Then do the radial list.
	pList = m_pRadialGradList;
	while (pList != NULL)
	{
		pFill = pList->pFill;
		WriteRadialFill ( pFill, pList->effect, pList->id );
		pList = pList->pNext;
	}
}



CC_IMPLEMENT_DYNAMIC(AIEPSGradientScanRenderRegion, VectorFileRenderRegion)


/********************************************************************************************

>	virtual void AIEPSGradientScanRenderRegion::AIEPSGradientScanRenderRegion (RenderRegion * pControllingRegion)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/2001
	Inputs:		pControllingRegion - the AIEPSRenderRegion that created this..
	Purpose:	Used to scan the tree for gradient fills when exporting to AI EPS format. The 
				objects are rendered into this region, which then stores the fills in a gradient
				cache. This is necessary, as objects such as blends only allow access to the 
				intermediate steps when they are being rendered, and any gradient fills need 
				to be cached in the AI file before proper rendering starts. 

********************************************************************************************/
AIEPSGradientScanRenderRegion::AIEPSGradientScanRenderRegion (RenderRegion * pControllingRegion)
 : VectorFileRenderRegion ()
{
	SetControllingRegion (pControllingRegion);

	DocRect clipRect = pControllingRegion->GetClipRect ();
	SetClipRect (clipRect);
}

/********************************************************************************************

>	virtual void AIEPSGradientScanRenderRegion::DrawPathToOutputDevice (Path *pPath, 
														  PathProcessor *pCaller = NULL, 
														  PathShape ShapePath = PATHSHAPE_PATH)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/2001
	Inputs:		see RenderRegion::DrawPathToOutputDevice
	Purpose:	Used to catch gradient fills on objects (mainly in blends, contours, brushes, 
				and other objects which won't show up in the tree) so that they can be included 
				properly in the caching system.

	See also:	RenderRegion::DrawPath

********************************************************************************************/
void AIEPSGradientScanRenderRegion::DrawPathToOutputDevice (Path *DrawPath, PathShape shapePath /*= PATHSHAPE_PATH*/)
{
	FillGeometryAttribute * pFill = (FillGeometryAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	AIEPSRenderRegion * pControl = (AIEPSRenderRegion *) GetControllingRegion ();

	EFFECTTYPE effect = GetFillEffect ();

	// Write and store the fill info it's got a gradient fill.
	if (pControl->IsGradientFillValidForExport (pFill) && 
		!pControl->FindGradientInCache (pFill, effect))
	{
		// Store this fill - function takes a copy of the whole fill (not just the pointer), 
		//	so this is safe. 
		pControl->IncludeGradientFill (pFill, effect);
	}

	// Reset the fill to RGB. This is temp, so it will be deleted when it is no longer used.
	SetFillEffect (new FillEffectFadeAttribute, TRUE);
}

/********************************************************************************************

>	virtual void AIEPSGradientScanRenderRegion::SetControllingRegion (RenderRegion * pControllingRegion)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/2/2001
	Inputs:		the AIEPSRenderRegion that we need to send all the information to once we've 
				scanned it in.
	Purpose:	sets the controlling AIEPSRenderRegion. I.e. the one that needs to cache the 
				gradient fills.

********************************************************************************************/
void AIEPSGradientScanRenderRegion::SetControllingRegion (RenderRegion * pControllingRegion)
{
	ERROR3IF (!pControllingRegion->IsKindOf (CC_RUNTIME_CLASS(AIEPSRenderRegion)), "AIEPSGradientScanRenderRegion::SetControllingRegion - controlling region is not an AIEPSRegion");

	m_pController = pControllingRegion;
}



CC_IMPLEMENT_DYNAMIC (ListFill, CCObject);

/********************************************************************************************

>	ListFill::ListFill ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001
	Purpose:	Constructor for a gradient fill holder, so it can be stored in a linked list
				cache. All variables (esp. the id) should be set before use. It is better to 
				use the constructor below, as this will allocate id numbers uniquely.

********************************************************************************************/
ListFill::ListFill ()
{
	pFill = NULL;
	pNext = NULL;
	id = 1;
	effect = EFFECT_RGB;
}

/********************************************************************************************

>	ListFill::ListFill (FillGeometry * pNewFill, ListFill * pNewNext)

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001
	Inputs:		pNewFill	- The fill to be stored.
				pNewNext	- The next item in the list.
	Purpose:	Constructor for a gradient fill holder, so it can be stored in a linked list
				cache.

********************************************************************************************/
ListFill::ListFill (FillGeometryAttribute * pNewFill, EFFECTTYPE eff, ListFill * pNewNext) 
{
	pFill = pNewFill;
	effect = eff;

	pNext = pNewNext;
	if(pNewNext)
	{
		id = pNewNext->id + 1;
	}
	else
	{
		id = 1;
	}
}

/********************************************************************************************

>	ListFill::~ListFill ()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/2001
	Purpose:	Destructor for a ListFill item. This also cleans up the fill it contains (if
				any).

********************************************************************************************/
ListFill::~ListFill ()
{
	if (pFill != NULL) 
		delete pFill;
}


