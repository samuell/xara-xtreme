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

// RenderRegion derived classes to export Camelot documents in EPS format.

/*
*/

#include "camtypes.h"
#include "saveeps.h"

#include <afxdlgs.h>
#include <ctype.h>
#include <time.h>

#include "pathname.h"
#include "document.h"
#include "errors.h"
#include "ensure.h"
#include "paper.h"
#include "nodepath.h"
#include "paths.h"
#include "lineattr.h"
#include "fillattr.h"
//#include "tim.h"
#include "kerneldc.h"
#include "spread.h"
#include "userenv.h"
#include "doccomp.h"
#include "colourix.h"
#include "attrmgr.h"
#include "cameleps.h"
#include "view.h"
#include "txtattr.h"
#include "nodetext.h"
#include "psrndrgn.h"
#include "psdc.h"
#include "nativeps.h"	// The old style EPS native filter, used in v1.1
#include "fixmem.h"		// for CCFree()
#include "release.h"
#include "app.h"
#include "fontman.h"
#include "colplate.h"
#include "clipattr.h"	// for ClipRegionAttribute.

CC_IMPLEMENT_DYNAMIC(EPSRenderRegion, VectorFileRenderRegion)

/********************************************************************************************

>	EPSRenderRegion::EPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, 
									 FIXED16 ViewScale) :
										RenderRegion(ClipRect, ConvertMatrix, ViewScale)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Purpose:	Constructor for an EPSOutputFilter object.  The object should be initialised
				before use.
	SeeAlso:	EPSOutputFilter::Init

********************************************************************************************/

EPSRenderRegion::EPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale) :
	VectorFileRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
	ValidPen = FALSE;
	CreatorString = "Adobe Illustrator 3.0 (exported by Camelot)";

	ExportFile = NULL;
	m_bValidPathAttrs = TRUE;
	m_bValidTextAttrs = TRUE;
}


/********************************************************************************************

>	BOOL EPSRenderRegion::Init()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/10/93
	Returns:	TRUE if the filter was initialised ok, FALSE otherwise.
	Purpose:	Initialise an EPSOutputFilter object.
	Errors:		Will fail if not enough memory to initialise the EPS stack.
	SeeAlso:	EPSStack

********************************************************************************************/

BOOL EPSRenderRegion::Init()
{
	// Initialise all member variables.
	ValidPen		= FALSE;
	LastOutputAttrs	= NULL;
	ExportFile		= NULL;
	CreatorString	= NULL;

	m_bValidPathAttrs = TRUE;
	m_bValidTextAttrs = TRUE;

	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSRenderRegion::AttachDevice(View* pView, CDC* pDC, Spread* pSpread)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		pView - the view to attach this region to - ignored (must be NULL).
				pDC - the device context to attach the render region to.
				pSpread - the spread to attach the render region to.
	Purpose:	Attach the EPS render region to a device context and a spread to export.
	SeeAlso:	RenderRegion::AttachDevice; EPSExportDC

********************************************************************************************/

BOOL EPSRenderRegion::AttachDevice(View* pView, CDC* pDC, Spread* pSpread)
{
	// Sanity checks
//	ENSURE(pView == NULL, "Bad window pointer in EPSRenderRegion::AttachDevice");

	// Call the base class first
	if (!RenderRegion::AttachDevice(pView, pDC, pSpread))
		return FALSE;
	
	// Save the first node we need to export.
//	CurrentRenderState = pSpread->FindFirstForExport(this, FALSE);

	// Get a pointer to the CCDiskFile object
	EPSExportDC* pEPSDC = (EPSExportDC*) pDC;
	ExportFile = pEPSDC->ExportFile;

	// All okey dokey
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSRenderRegion::InitDevice ()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	09/05/95
	Returns:	TRUE if the device context is initialised ok;
				FALSE if not.
	Purpose:	Initialise the device specific mechanisms for this render region.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/

BOOL EPSRenderRegion::InitDevice ()
{
	// Call base class
	if (!RenderRegion::InitDevice())
		return FALSE;

	// Set the render caps up
	GetRenderRegionCaps(&Caps);

	// Buffer used to build up the %%For and %%CreationDate comments.
	char buf[300];

	// Find out which document we're using
	ENSURE(RenderView->GetDoc() != NULL, "View's document is NULL!");
	Document *TheDocument = RenderView->GetDoc();

	KernelDC *pDC = (KernelDC *) RenderDC;

	// Output the standard EPS header affair...
	WriteEPSVersion ();

	// Name of program that created this file.
	pDC->OutputToken	( "%%Creator:" );
	pDC->OutputToken	( CreatorString );
	pDC->OutputNewLine	();
	
	// File version saving. In anything but Native format does nothing
	WriteFileVersion ( pDC );

	// Output the %%For comment
	_stprintf(buf, "%%%%For: (%s) (%s)", ReleaseInfo::GetLicensee(), ReleaseInfo::GetCompany());
	pDC->OutputToken(buf);
	pDC->OutputNewLine();

	// The title of the picture
   	String_256 DocumentTitle = TheDocument->GetTitle();
	_stprintf(buf, "%%%%Title: (%s)", (TCHAR *) DocumentTitle);
	pDC->OutputToken(buf);
	pDC->OutputNewLine();

	// Date this file was created.
	time_t Now;
	time(&Now);
	struct tm *pNow = localtime(&Now);
	_tcsftime(buf, 100, "%%%%CreationDate: (%d/%m/%y) (%I:%M %p)", pNow);
	pDC->OutputToken(buf);
	pDC->OutputNewLine();

	// Bounding box.
	WriteEPSBoundingBox ();

	// Get a handle to our filter
	EPSExportDC *pExportDC = (EPSExportDC *) pDC;
	EPSFilter *pFilter = (EPSFilter *) pExportDC->GetParentFilter();

	// Write out the document's process colours. (Only for AIEPS.)
	WriteEPSProcessColours ();

	// Write out the Documents fonts
	TheDocument->WriteEPSFonts ( pFilter );

	// Write out the Documents resources
	WriteEPSResources ( pFilter, TheDocument );

	// Write out the Documents comments
	TheDocument->WriteEPSComments ( pFilter );
	
	// We can say compress this file if native.
	// We will leave the document comments in so that things like the clipart gallery
	// can still easily access this useful information.  
	//
	// Compressed saving. In anything but Native format does nothing
	WriteCompressionState ( pDC );

	// Get all the document components to output their header comments (if any)
	DocComponent *pComponent = TheDocument->EnumerateDocComponents ( NULL );

	while (pComponent != NULL)
	{
		// Inform this document component that we are about to start an EPS export.
		pComponent->WriteEPSComments(pFilter);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// No more comments
	pDC->OutputToken	( "%%EndComments" );
	pDC->OutputNewLine	();

	// Do the prolog...
	pDC->OutputToken	( "%%BeginProlog" );
	pDC->OutputNewLine	();

	// Do the render region specific prolog.
	WriteProlog ( pDC );

	// Write out the document,s Prolog script.
	WriteEPSProlog ( pFilter, TheDocument );

	// and the doc components prolog
	pComponent = TheDocument->EnumerateDocComponents(NULL);
	while (pComponent != NULL)
	{
		// Get this document component to write its EPS prolog out to the file.
		pComponent->WriteEPSProlog(pFilter);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	pDC->OutputToken("%%EndProlog");
	pDC->OutputNewLine();

	// Do the setup...
	pDC->OutputToken("%%BeginSetup");
	pDC->OutputNewLine();

	// Do the render region specific setup.
	WriteSetup ( pDC );

	// Write out the font definition.
	WriteDocumentSetup ( TheDocument, pFilter );

	// and the components setup
	pComponent = TheDocument->EnumerateDocComponents(NULL);
	while (pComponent != NULL)
	{
		// Get this document component to write its EPS setup out to the file.
		pComponent->WriteEPSSetup(pFilter);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// Parse through the tree, locate the gradient fills, and write them out
	// to the file.
	WriteGradientFills ( TheDocument );

	// Wrap up the header section
	pDC->OutputToken("%%EndSetup");
	pDC->OutputNewLine();

	// We are into the main script of the EPS file here, so give the
	// doc components one last chance to write something out
	pComponent = TheDocument->EnumerateDocComponents(NULL);

	while (pComponent != NULL)
	{
		// Get this document component to write its EPS Script data to the file
		pComponent->WriteScript(pFilter);

		// Look for next doc component
		pComponent = TheDocument->EnumerateDocComponents(pComponent);
	}

	// Set up render region
	InitClipping ();

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::CloseDown()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/09/95
	Returns:	TRUE if region was shut down ok;
				FALSE if not.
	Purpose:	Tells the EPS render region that all rendering is finished and the region
				is about to be shut down, so it should output any trailer etc that is
				required.

********************************************************************************************/

BOOL EPSRenderRegion::CloseDown()
{
	// Close down any active layers.
	EndLayer ();

	// Write out the trailer.
	WriteEPSTrailerComments ();

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::SaveAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/04/95
	Inputs:		Index - Unique attribute ID (See AttributeManager::RegisterDefaultAttribute).
				pAttr - the new attribute to use as the current one.
				Temp - TRUE if pAttr points to a temporary object, FALSE if not.
	Returns:	TRUE if the operation completed successfully, FALSE if not.
	Purpose:	Saves the current attribute on the attribute context stack, and installs
				the given attribute as the new attribute.
				If the attribute object is temporary, then it is deleted when the attribute
				is popped off the stack. See RenderRegion::RestoreAttribute

	Errors:		Out of memory.
	SeeAlso:	AttributeManager::RegisterDefaultAttribute

********************************************************************************************/

BOOL EPSRenderRegion::SaveAttribute(UINT32 Index, AttributeValue *pAttr, BOOL Temp)
{
	// Push the current attribute onto the stack, and install the new attribute as
	// the 'current' one.
	if (TheStack.Push(CurrentAttrs[Index].pAttr, CurrentAttrs[Index].Temp))
	{
		CurrentAttrs[Index].pAttr = pAttr; 
		CurrentAttrs[Index].Temp = Temp;

		// Everything worked ok.
		return TRUE;
	}

	// Failed
	return FALSE;
}

/********************************************************************************************
>	BOOL EPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/3/95
	Inputs:		ch      - unicode value of char
				pMatrix - matrix specifying transforms to place char correctly in document
	Returns:	FALSE if fails
	Purpose:	
********************************************************************************************/

BOOL EPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
	// If we are not drawing complex shapes and this shape is, then return
	if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
		return TRUE;

	ERROR2IF(   this==NULL,FALSE,"RenderRegion::RenderChar() - this==NULL");
	ERROR2IF(pMatrix==NULL,FALSE,"RenderRegion::RenderChar() - pMatrix==NULL");


#if EXPORT_TEXT
	// Check for changed attributes
	GetValidPathAttributes();
	GetValidTextAttributes();

// BODGE TEXT - need to account for unicode!
	KernelDC *pDC = (KernelDC *) RenderDC;

	char Buf[64];

	wsprintf(Buf,"%c",ch);

	pDC->OutputString(Buf);
	pDC->OutputToken("Tx");
	pDC->OutputNewLine();

#else
	// just do what RenderRegion::RenderChar() would do!

	// create the char's path
	Path* pCharPath=CreateCharPath(ch,pMatrix);
	if (pCharPath==NULL)
		return FALSE;

	// draw path using current attibutes in render region
 	if (pCharPath->GetNumCoords()!=0)
		DrawPath(pCharPath);

	// clean up
	delete pCharPath;
#endif

	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteNewLine ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/4/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- Failure.
	Purpose:	Outputs a new line tag to the file.

********************************************************************************************/

BOOL EPSRenderRegion::WriteNewLine ( void )
{
	EPSExportDC	*pDC	= static_cast<EPSExportDC*> ( RenderDC );

	// Check for changed attributes.
	GetValidPathAttributes ();
	GetValidTextAttributes ();

	// Output the tokens
	pDC->OutputToken	( "(\\r)" );
	pDC->OutputToken	( "TX" );
	pDC->OutputNewLine	();

	return TRUE;
}

/********************************************************************************************

>	void EPSRenderRegion::OutputFontName()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Purpose:	Output an Illustrator format fontname and pointsize token. The format is	
				/_fontname[-Bold|-Italic|-BoldItalic] pointsize Tf.
				EncodeFontName will generate this format from an internal fontname.
				DocodeFontName will return an internal format fontname and flags where nec.

********************************************************************************************/

void EPSRenderRegion::OutputFontName()
{
   	KernelDC *pDC = (KernelDC *) RenderDC;

	String_64 FontName;
	String_64 EncodedFontName;
	String_64 Append("/_");

	// get information about the current font
	FONTMANAGER->GetFontName(RR_TXTFONTTYPEFACE(), FontName);

	// Graeme (31-3-00) - I've lifted this piece of code from the CamelotEPSRenderRegion. It
	// appears to map an existing, encoded font name onto its Postscript counterpart.
	FONTMANAGER->EncodeAndMapFontName(FontName, EncodedFontName, GetFontStyle());
	
	EncodedFontName.Insert(&Append,0);
	pDC->OutputToken((TCHAR *)EncodedFontName);

	// Output the fontsize next
	double PointSize = ((double)RR_TXTFONTSIZE())/1000;
	pDC->OutputFloat(PointSize,3);
	
	// finally do output the font token
	pDC->OutputToken("Tf");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	INT32 EPSRenderRegion::GetFontStyle()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	18/4/95
	Purpose:	Compile the current style attributes.

********************************************************************************************/

INT32 EPSRenderRegion::GetFontStyle()
{
	INT32 Style = 0;
	if (RR_TXTBOLD()) Style+=1;
	if (RR_TXTITALIC()) Style+=2;
	return Style;
}


//
// These are the functions that actually do the exporting...
/********************************************************************************************

>	void EPSRenderRegion::GetValidTextAttributes()

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Purpose:	Text  equivalent of GetValidPathAttributes().

********************************************************************************************/

void EPSRenderRegion::GetValidTextAttributes()
{
	// Illustrator does not specify a token for changing 
	//  -fontsize
	//  -bold
	//  -italic
	// We need to use the standard Tf token. (see OutputFontName)
	
	BOOL FontOut = FALSE;
   	if (SetLastOutputAttribute(ATTR_TXTFONTTYPEFACE))
   	{
		FontOut=TRUE;
   	}

	if (SetLastOutputAttribute(ATTR_TXTFONTSIZE))
   	{
		FontOut=TRUE;
   	}		 

	if (SetLastOutputAttribute(ATTR_TXTBOLD))
   	{
		FontOut=TRUE;
   	}		

	if (SetLastOutputAttribute(ATTR_TXTITALIC))
   	{
		FontOut=TRUE;
   	}

	if (!m_bValidTextAttrs)
	{
		FontOut = TRUE;
	}

	// ok check whether a font change needs generating
	if (FontOut)
		OutputFontName();


	BOOL FillNLine = FALSE;
 	if (SetLastOutputAttribute(ATTR_STROKECOLOUR))
	{
		FillNLine = TRUE;
	}

	if (SetLastOutputAttribute(ATTR_FILLGEOMETRY))
 	{
		FillNLine = TRUE;
	}

	if (!m_bValidTextAttrs)
	{
		FillNLine = TRUE;
	}

	// Text Render mode needs changing as Text has gained or lost fill and/or 
	//	line colours
	if (FillNLine)
	{	
		OutputTextRenderMode ();
	}

	if (SetLastOutputAttribute(ATTR_TXTASPECTRATIO) || !m_bValidTextAttrs)
	{
		OutputTextAspectRatio ();
	}		

   	if (SetLastOutputAttribute(ATTR_TXTTRACKING) || !m_bValidTextAttrs)
   	{
		OutputTextTracking ();
   	}		 

	if (SetLastOutputAttribute(ATTR_TXTJUSTIFICATION) || !m_bValidTextAttrs)
   	{
		OutputTextJustification ();
   	}

// BODGE TEXT - should this be output before any lines rather than on a line
// 				by line basis?
   	if (SetLastOutputAttribute(ATTR_TXTLINESPACE) || !m_bValidTextAttrs)
   	{
		OutputTextLineSpacing ();
	}

	if (SetLastOutputAttribute(ATTR_TXTBASELINE) || !m_bValidTextAttrs)
	{
		OutputTextBaselineShift ();
	}

	if (SetLastOutputAttribute(ATTR_TXTSCRIPT) || !m_bValidTextAttrs)
	{
		OutputTextSubSuperScript ();
	}

	// The text attributes are now Definitely valid (as we've just written all of them)
	m_bValidTextAttrs = TRUE;
}



/********************************************************************************************

>	void EPSRenderRegion::OutputTextRenderMode()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the text's Render Mode (Tr) - this happens when the 
				text's fill or line colour changes to or from blank, see-through colour

********************************************************************************************/

void EPSRenderRegion::OutputTextRenderMode ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	INT32 Style=0;

	// Is there a currently active fill colour?
	if (! (RR_FILLCOLOUR().IsTransparent()) )
		Style+=1;

	// Is there a currently active line colour?
	if (! (RR_STROKECOLOUR().IsTransparent()) )
		Style+=2;

	switch (Style)
	{
		case 0: pDC->OutputToken("3 Tr");		// Invisible
				pDC->OutputNewLine();
				break;
		case 1: pDC->OutputToken("0 Tr");		// filled only
				pDC->OutputNewLine();
				break;
		case 2: pDC->OutputToken("1 Tr");		// stroked only
				pDC->OutputNewLine();
				break;
		case 3: pDC->OutputToken("2 Tr");		// filled and stroked
				pDC->OutputNewLine();
				break;
	}
}



/********************************************************************************************

>	void EPSRenderRegion::OutputTextAspectRatio()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the text's aspect ratio (Tx)

********************************************************************************************/

void EPSRenderRegion::OutputTextAspectRatio ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

   	pDC->OutputReal(RR_TXTASPECTRATIO().MakeDouble()*100.0);	// convert from ratio to %
   	pDC->OutputToken("Tz");
	pDC->OutputNewLine();
}



/********************************************************************************************

>	void EPSRenderRegion::OutputTextTracking()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the tracking (Tt)

********************************************************************************************/

void EPSRenderRegion::OutputTextTracking ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Must be output in 1/1000 of an em.
	// 1 em = point size of font.
	// Tracking internally =millipoints.
   	pDC->OutputValue(RR_TXTTRACKING());
   	pDC->OutputToken("Tt");
	pDC->OutputNewLine();
}



/********************************************************************************************

>	void EPSRenderRegion::OutputTextJustification()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the text justification (Ta)

********************************************************************************************/

void EPSRenderRegion::OutputTextJustification ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Output aspect ratio in %, 100 is default
   	switch (RR_TXTJUSTIFICATION())
   	{
   	case JLEFT: 
   		pDC->OutputValue((INT32)0);
		break;
	case JRIGHT: 
   		pDC->OutputValue((INT32)2);
		break;
	case JCENTRE: 
   		pDC->OutputValue((INT32)1);
		break;
	case JFULL: 
   		pDC->OutputValue((INT32)3);
		break;
	}
   	pDC->OutputToken("Ta");
	pDC->OutputNewLine();
}

/********************************************************************************************

>	void EPSRenderRegion::OutputTextLineSpacing()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the line spacing (Tl)

********************************************************************************************/

void EPSRenderRegion::OutputTextLineSpacing ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

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

	pDC->OutputReal(ptLineSpace);
	pDC->OutputReal(ptParaSpace);
	pDC->OutputToken("Tl");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputTextBaselineShift()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in Baseline shift (Ts)

********************************************************************************************/

void EPSRenderRegion::OutputTextBaselineShift ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Output baseline shift in points
	// format = rise Ts
	double BaseLine = ((double)RR_TXTBASELINE())/1000;
	pDC->OutputFloat(BaseLine,3);
	pDC->OutputToken("Ts");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputTextSubSuperScript()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/11/00
	Purpose:	Output the change in the Sub/Super script attribute (Ts)

********************************************************************************************/

void EPSRenderRegion::OutputTextSubSuperScript ()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Output sub/superscript
	double FontSize = ((double)RR_TXTFONTSIZE())/1000;			    	// in millipoints	 12pt = 12000 mp

	TxtScriptAttribute* pScript = RR_TXTSCRIPT();
	double offset = (pScript->Offset).MakeDouble();
	double size = (pScript->Size).MakeDouble();

	pDC->OutputToken("%%XSScript");
	pDC->OutputNewLine();

	double rise = FontSize*offset;

	pDC->OutputFloat(rise,3);
	pDC->OutputToken("Ts");
	pDC->OutputNewLine();

	double ptsize = FontSize*size;

	String_64 MappedFont;
	String_64 Append("/_");

	String_64 FontName;
	FONTMANAGER->GetFontName(RR_TXTFONTTYPEFACE(), FontName);

	// Graeme (31-3-00) - Map the encoded name onto the PS font name.
	FONTMANAGER->EncodeAndMapFontName(FontName,MappedFont,GetFontStyle());

	// Graeme (14-6-00) - I should add ascent and descent values, but Camelot doesn't
	// seem to store them anywhere.
	MappedFont.Insert(&Append,0);
	pDC->OutputToken((TCHAR *)MappedFont);
	pDC->OutputFloat(ptsize,3);
	// Output the ascent.
	// Output the descent.
	pDC->OutputToken("Tf");
	pDC->OutputNewLine();
}

/********************************************************************************************

>	void EPSRenderRegion::GetValidPathAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> (Will)
	Created:	30/03/94
	Purpose:	Ensure the EPS context has the correct attributes for drawing paths.
				This required because when the path attributes change, we don't output
				the EPS commands to reflect this straight away - we set a flag to indicate
				that the attributes have changed, and then when this function is called,
				it notices that the attributes have changed, and outputs the EPS commands
				to select these attributes.  This avoids redundant and copious attribute
				code in our EPS files.
				(The flags used are ValidPen and FillFlags.ValidGeometry).

********************************************************************************************/

void EPSRenderRegion::GetValidPathAttributes()
{
	// Path stroking. Do nothing if the stroke is transparent.
	// Note, if this changes you MUST investigate and change GetValidTextAttributes
	// cause this needs to output transparent stroke colours.
	if (SetLastOutputAttribute(ATTR_WINDINGRULE) || !m_bValidPathAttrs)
	{
		OutputWindingRule ();
	}

	if (!RR_STROKECOLOUR().IsTransparent())
	{
		if (SetLastOutputAttribute(ATTR_STROKECOLOUR) || !m_bValidPathAttrs)
			OutputStrokeColour();
	}

	if (SetLastOutputAttribute(ATTR_LINEWIDTH) || !m_bValidPathAttrs)
		OutputLineWidth();

	if (SetLastOutputAttribute(ATTR_JOINTYPE) || !m_bValidPathAttrs)
		OutputJoinType();

	// Set up the dash pattern first.
	OutputDashPatternInit();
	if (SetLastOutputAttribute(ATTR_DASHPATTERN) || !m_bValidPathAttrs)
		OutputDashPattern();

	if (SetLastOutputAttribute(ATTR_STARTCAP) || !m_bValidPathAttrs)
		OutputStartCap();

	if (SetLastOutputAttribute(ATTR_MITRELIMIT) || !m_bValidPathAttrs)
		OutputMitreLimit();

	// Path filling

	if (!RR_FILLCOLOUR().IsTransparent())
	{
		if (SetLastOutputAttribute(ATTR_FILLGEOMETRY) || !m_bValidPathAttrs)
			OutputFillColour();
	}

	// The path attributes must be valid (as we've just written them).
	m_bValidPathAttrs = TRUE;

}



/********************************************************************************************

>	void EPSRenderRegion::OutputWindingRule()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/12/00
	Purpose:	Output winding rule token to the EPS file

				This doesn't actually do anything, it's just here so that derived classes can
				override it.

********************************************************************************************/

void EPSRenderRegion::OutputWindingRule ()
{

}




/********************************************************************************************

>	void EPSRenderRegion::OutputLineWidth()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Output the standard line width token and measure for EPS

********************************************************************************************/

void EPSRenderRegion::OutputLineWidth()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Set line width
	pDC->OutputUserSpaceValue(RR_LINEWIDTH());
	pDC->OutputToken("w");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputJoinType()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Output the standard EPS join type

********************************************************************************************/

void EPSRenderRegion::OutputJoinType()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Set line Join Type
	pDC->OutputValue((UINT32)RR_JOINTYPE());
	pDC->OutputToken("j");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputDashPatternInit()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Sets up the next dash pattern for output.
	SeeAlso:	OutputDashPattern()

	Notes:		Special code for dash patterns, as they are affected by another
				attribute, which is line width, so we need to watch out for this by
				checking the base class 'StrokeFlags' variable (a change of line
				width automatically causes the dash pattern flag to be invalidated).

********************************************************************************************/

void EPSRenderRegion::OutputDashPatternInit()
{
	if (!StrokeFlags.ValidDashPattern && (RR_DASHPATTERN().Elements > 0))
	{
		// Delete the attribute if necessary
		if (LastOutputAttrs[ATTR_DASHPATTERN].Temp)
			delete LastOutputAttrs[ATTR_DASHPATTERN].pAttr;

		// Set to NULL to force it to be output by SetLastOutputAttribute()
		LastOutputAttrs[ATTR_DASHPATTERN].pAttr = NULL;
	}

	// Don't do this again until we need to.
	StrokeFlags.ValidDashPattern = TRUE;
}


/********************************************************************************************

>	void EPSRenderRegion::OutputDashPattern()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Output the current dash pattern settings. Watch for interaction with line
				widths, a change of line width causes the dash pattern to become invalid.
				To output a dash pattern correctly, use the following code

				OutputDashPatternInit();
				if (SetLastOutputAttribute(ATTR_DASHPATTERN))
					OutputDashPattern();

	SeeAlso:	OutputDashPatternInit();

********************************************************************************************/

void EPSRenderRegion::OutputDashPattern()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Set dash pattern
	INT32 NumEls = RR_DASHPATTERN().Elements;
	INT32* TempArray = NULL;

	// If the dash patterns need scaling, then we need to scale them
	// here, as Illustrator format uses absolute sizes for the dash
	// patterns, rather than the 'relative to line width' that we use
	INT32 DashLineWidth 	= RR_DASHPATTERN().LineWidth;
	INT32 LineWidth 		= RR_LINEWIDTH();

	BOOL DoScale = RR_DASHPATTERN().ScaleWithLineWidth;
	FIXED16 Scale = DoScale ? (double(LineWidth) / double(RR_DASHPATTERN().LineWidth)) : 1;

	// Don't bother with zero width lines
	if (LineWidth == 0)
		NumEls = 0;

	// But we needn't bother if there are no elements
	if (NumEls > 0)
	{
		INT32* DashArray = RR_DASHPATTERN().ElementData;
		TempArray = new INT32[NumEls];

		if (TempArray)
		{
			for (INT32 el=0; el<NumEls; el++)
			{
				TempArray[el] = LongMulFixed16(DashArray[el], Scale);

				if (TempArray[el] == 0)
				{
					// Found a zero dash element - this line is obviously too small
					// to bother with dash patterns.
					NumEls = 0;
					break;
				}
			}
		}
	}
	
	// Output the (possibly scaled) array of On-Off distances
	pDC->OutputArray(TempArray, NumEls);

	// Delete the temp buffer if necessary
	delete TempArray;

	// Also scale the Dash Start Offset if need be
	INT32 Offset = LongMulFixed16(RR_DASHPATTERN().DashStart, Scale);
	pDC->OutputUserSpaceValue(Offset);

	pDC->OutputToken("d");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputStartCap()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Output the start cap setting using the usual 'J' EPS token

********************************************************************************************/

void EPSRenderRegion::OutputStartCap()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	// Set line cap style
	pDC->OutputValue((UINT32)RR_STARTCAP());
	pDC->OutputToken("J");
	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputMitreLimit()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Output the current mitre limit value using the usual 'M' EPS token

********************************************************************************************/

void EPSRenderRegion::OutputMitreLimit()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

		// Set mitre limit

// We have no way of altering Mitre Limits, so there's not
// much point in saving them.

//		pDC->OutputUserSpaceValue(RR_MITRELIMIT());
//		pDC->OutputToken("M");
//		pDC->OutputNewLine();
}




/********************************************************************************************

>	void EPSRenderRegion::OutputStrokeColour()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	Output a stroke colour. This calls either OutputStrokeRGBcolour or
				OutputStrokeCMYKColour depending on the colour model.

********************************************************************************************/

void EPSRenderRegion::OutputStrokeColour()
{
	if (RR_STROKECOLOUR().GetColourModel() == COLOURMODEL_RGBT ||
		RR_STROKECOLOUR().GetColourModel() == COLOURMODEL_HSVT)
	{
		// Use new RGB fill colours if we're using an RGB colour model
		OutputStrokeRGBColour ();
	}
	else
	{
		// Use old CMYK version if we're not.
		OutputStrokeCMYKColour ();
	}
}


/********************************************************************************************

>	void EPSRenderRegion::OutputStrokeCMYKColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/00
	Purpose:	Output the current stroke colour. This function will use one of the 
				following EPS tokens
				K - if not separating and colour is unnamed
				X - if not separating and colour is named
				G - if separating

********************************************************************************************/

void EPSRenderRegion::OutputStrokeCMYKColour()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	PColourCMYK CMYK;

	// Assume no local context at present
	ColourContext* pContext;
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

	if (pSeparation==NULL)
	{
		// Get the current line colour in CMYK values, unless it's transparent
		// Print out colour component values
		RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
		pDC->OutputColour(&CMYK);

		if (RR_STROKECOLOUR().FindParentIndexedColour() == NULL)
		{
			// Unnamed colour - just add 'K' token
			pDC->OutputToken("K");
		}
		else
		{
			// Named colour - add Name, tint value, and 'X' token
			pDC->OutputColourName(&(RR_STROKECOLOUR()));
			pDC->OutputValue(0l);
			pDC->OutputToken("X");
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
			pDC->OutputColourValue(c);
			pDC->OutputToken("G");
		}
		else
		{
			RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
			pDC->OutputColour(&CMYK);
			pDC->OutputToken("X");
		}
	}

	pDC->OutputNewLine();
}



/********************************************************************************************

>	void EPSRenderRegion::OutputStrokeRGBColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/00
	Purpose:	Output the current stroke colour as RGB. This function will use one of the 
				following EPS tokens
				XA - if not separating and colour is unnamed
				XX - if not separating and colour is named
				G  - if separating

********************************************************************************************/

void EPSRenderRegion::OutputStrokeRGBColour()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

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
		pDC->OutputColourValue (red);
		pDC->OutputColourValue (green);
		pDC->OutputColourValue (blue);

		if (RR_STROKECOLOUR().FindParentIndexedColour() == NULL)
		{
			// Unnamed colour - just add 'XA' token
			pDC->OutputToken ("XA");
		}
		else
		{
			// Named colour - add Name, tint value, RGB flag and 'XX' token
			pDC->OutputColourName (&(RR_STROKECOLOUR()));
			pDC->OutputValue(0l);
			pDC->OutputValue(1l);
			pDC->OutputToken("XX");
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
			pDC->OutputColourValue(c);
			pDC->OutputToken("G");
		}
		else
		{
			RR_STROKECOLOUR().GetCMYKValue(pContext, &CMYK);
			pDC->OutputColour(&CMYK);
			pDC->OutputToken("X");
		}
	}

	pDC->OutputNewLine();
}

/********************************************************************************************

>	void EPSRenderRegion::OutputFillColour()

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/7/96
	Purpose:	This now delegates the output to either OutputFillRGBColour or 
				OutputFillCMYKColour, depending on the colour model used.

********************************************************************************************/

void EPSRenderRegion::OutputFillColour()
{
	if (RR_FILLCOLOUR().GetColourModel() == COLOURMODEL_RGBT ||
		RR_FILLCOLOUR().GetColourModel() == COLOURMODEL_HSVT)
	{
		// Use new RGB fill colours if we're using an RGB colour model
		OutputFillRGBColour ();
	}
	else
	{
		// Use old CMYK version if we're not.
		OutputFillCMYKColour ();
	}
}

/********************************************************************************************

>	void EPSRenderRegion::OutputFillCMYKColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7/12/00
	Purpose:	Output the current fill colour in CMYK mode. This function will use one of 
				the following EPS tokens
				k - if not separating and colour is unnamed
				x - if not separating and colour is named
				g - if separating

********************************************************************************************/

void EPSRenderRegion::OutputFillCMYKColour()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	PColourCMYK CMYK;

	// retrieve the output plate if there is one
	ColourContext* pContext;
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

	// It seems that in composite mode we always output c,m,y,k colour
	// and don't have a monochromatic composite mode. So we don't need
	// to check this.
	if (pSeparation==NULL)
	{
		// We're in colour output mode, so output the colour values.
		RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
		pDC->OutputColour(&CMYK);

		if (RR_FILLCOLOUR().FindParentIndexedColour() == NULL)
		{
			// Unnamed colour - add 'k' token
			pDC->OutputToken("k");
		}
		else
		{
			// Named colour - add Name, tint and 'x' token
			pDC->OutputColourName(&(RR_FILLCOLOUR()));
			pDC->OutputValue(0l);
			pDC->OutputToken("x");
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
			pDC->OutputColourValue(c);
			pDC->OutputToken("g");
		}
		else
		{
			RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
			pDC->OutputColour(&CMYK);
			pDC->OutputToken("k");
		}
	}

	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::OutputFillRGBColour()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/11/00
	Purpose:	Output the current fill colour as RGB. This function will use one of the 
				following EPS tokens
				Xa - if not separating and colour is unnamed
				Xx - if not separating and colour is named
				g  - if separating

********************************************************************************************/

void EPSRenderRegion::OutputFillRGBColour()
{
	KernelDC *pDC = (KernelDC *) RenderDC;

	INT32 red;
	INT32 green;
	INT32 blue;;
	PColourCMYK CMYK;

	// retrieve the output plate if there is one
	ColourContext* pContext;
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

	if (pSeparation==NULL)
	{
		// We're in colour output mode, so output the colour values (as RGB).
		RR_FILLCOLOUR().GetRGBValue (&red, &green, &blue);
		pDC->OutputColourValue (red);
		pDC->OutputColourValue (green);
		pDC->OutputColourValue (blue);

		if (RR_FILLCOLOUR().FindParentIndexedColour() == NULL)
		{
			// Unnamed colour - add 'Xa' token
			pDC->OutputToken("Xa");
		}
		else
		{
			// Named colour - add Name, tint, RGB flag and 'Xx' token
			pDC->OutputColourName(&(RR_FILLCOLOUR()));
			pDC->OutputValue(0l);
			pDC->OutputValue(1l);
			pDC->OutputToken("Xx");
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
			pDC->OutputColourValue(c);
			pDC->OutputToken("g");
		}
		else
		{
			RR_FILLCOLOUR().GetCMYKValue(pContext, &CMYK);
			pDC->OutputColour(&CMYK);
			pDC->OutputToken("k");
		}
	}

	pDC->OutputNewLine();
}


/********************************************************************************************

>	void EPSRenderRegion::GetOutputColourPlate(ColourModel ColModel, ColourContext** pContext, ColourPlate** pPlate)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/06/96
	Inputs:		The colour model of the active output context to use.
				A pointer to a pointer to a colour context (can be NULL)
				A pointer to a pointer to a colour plate   (again can be NULL)
	Outputs:	pContext points to the current context in the view attached to this render region
				pPlate points to the plate whose model was required.
	Returns:	-
	Purpose:	This render region is associated with a document view. This function
				retrieves the colour context from this view and returns it along with
				a plate (if available). ColourModel is used to determin which plate to
				return from the context.
				If there is no view pContext and pPlate will be NULL.
				If there is no context in the view both return values will be NULL
				If there is a context but no plate attached to it, one can assume the
				current render action is not separated.

********************************************************************************************/

void EPSRenderRegion::GetOutputColourPlate(ColourModel ColModel, ColourContext** pContext, ColourPlate** pPlate)
{
	ColourContext *pCurContext=NULL;
	ColourPlate   *pCurPlate=NULL;

	// Get the render region view and find its local CMYK colour context
	View* pCurrView = GetRenderView();
	if (pCurrView!=NULL)
		pCurContext = pCurrView->GetColourContext(ColModel, TRUE);

	// If we have a context, check for mono output.
	if (pCurContext!=NULL)
		 pCurPlate = pCurContext->GetColourPlate();
	
	// Set return vars
	if (pContext!=NULL)
		(*pContext) = pCurContext;
	if (pPlate!=NULL)
		(*pPlate) = pCurPlate;
}


/********************************************************************************************

>	void EPSRenderRegion::DrawPathToOutputDevice(Path *DrawPath, BOOL DataOnly)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		DrawPath - the path to render.
	Purpose:	Output all the commands required to render this path to the EPS file.
	SeeAlso:	RenderRegion::DrawPath; EPSRenderRegion::GetValidPathAttributes
				EPSRenderRegion::ExportPath

********************************************************************************************/

void EPSRenderRegion::DrawPathToOutputDevice(Path *DrawPath, PathShape)
{
	// ChrisG - 26/10/00 Only export paths if they have any co-ordinates - Exporting
	//	paths with no co-ords causes no end of hassle in the ExportPath function below,
	//	where they are used as controls for while loops, positions in arrays (but with
	//	one subtracted), etc...
	if (DrawPath->GetNumCoords ())
	{
			// If we are not drawing complex shapes and this shape is, then return
		if ((!RenderComplexShapes) && (TestForComplexShape(&Caps)))
			return;

		ExportPath(DrawPath, FALSE);

		// Now do the arrow heads if the render region can't do them directly
		// -- But only draw arrow heads if the path is "stroked"
		if (DrawPath->IsStroked && !Caps.ArrowHeads)
		{
			// Doesn't support arrow heads directly so we render them as paths.
			DrawPathArrowHeads(DrawPath->GetCoordArray(),
							   DrawPath->GetVerbArray(),
							   DrawPath->GetNumCoords());
		}
	}
}

/********************************************************************************************

>	void EPSRenderRegion::ExportPath(Path *DrawPath, BOOL DataOnly, BOOL PureDataOnly = FALSE)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Inputs:		DrawPath - the path to render.
				DataOnly - if TRUE, only output the line/curve/moveto data, and not the stroke
							or fill operators.
						   if FALSE, just output path normally.  NB. see Purpose
				PureDataOnly	if TRUE, will do what the DataOnly flag does, but will really
								do it, instead of bottling out on compound paths!

	Purpose:	Output all the commands required to render this path to the EPS file.
				If this is a compound path, the stroke/fill operators will always be output,
				regardless of the value of the DataOnly parameter.

				Hello, Karim  here. The above is is *no longer the case* if the PureDataOnly
				flag is set. I *don't want any* stroke or fill operators on compound clipping
				paths, thankyou very much!

	SeeAlso:	RenderRegion::DrawPath; EPSRenderRegion::GetValidPathAttributes

********************************************************************************************/

void EPSRenderRegion::ExportPath(Path *DrawPath, BOOL DataOnly, BOOL PureDataOnly)
{
	// Only change data if we have actually have data to change
	if (!PureDataOnly)
	{
		// Make sure we draw in the right colours
		GetValidPathAttributes();
	}

	// Get the device context for our export file.
	KernelDC *pDC = (KernelDC *) RenderDC;

	DocCoord* Coords = DrawPath->GetCoordArray();
	PathVerb* Verbs  = DrawPath->GetVerbArray();
	INT32 NumCoords = DrawPath->GetNumCoords();

	// Position we are reading points from
	INT32 ReadPos = 1;

	// Check to see if this is a compound path. If it is, there will be more than one
	// moveto command.
	BOOL CompoundPath = FALSE;

	while (ReadPos < NumCoords)
	{
		// Find out the type of element that we are over (after the close flag has been removed)
		if (((Verbs[ReadPos]) & (~PT_CLOSEFIGURE) ) == PT_MOVETO)
		{
			// This is a compound path - stop searching for moveto's.
			CompoundPath = TRUE;
			break;
		}
		// Try next point
		ReadPos++;
	}

	// Karim 24/05/2000 - don't want this bracket data if the PureDataOnly flag is set.
	if (CompoundPath && !PureDataOnly)
	{
		// Bracket compound paths./
		pDC->OutputToken("*u");
		pDC->OutputNewLine();
	}

	// Work out how to render the path (stroke, fill, both, none, etc.)
	char PathTypeOpen[2] = "N";
	char PathTypeClosed[2];

	// Work out if the path is filled.
	BOOL IsFilled = TRUE;

	ColourFillAttribute *pFillAttr = 
		(ColourFillAttribute *) CurrentAttrs[ATTR_FILLGEOMETRY].pAttr;
	
	if (pFillAttr->IsKindOf(CC_RUNTIME_CLASS(FlatFillAttribute)) &&
		pFillAttr->Colour.IsTransparent())
	{
		// Flat fill with transparent colour => no fill
		IsFilled = FALSE;
	}

	if (!DrawPath->IsFilled)
		IsFilled = FALSE;

	// Work out what should be done to the path
	if (RR_STROKECOLOUR().IsTransparent() || !DrawPath->IsStroked)
	{
		if (IsFilled)
			// Fill path
			PathTypeOpen[0] = 'F';
	}
	else
	{
		if (IsFilled)
			// Stroke and fill path
			PathTypeOpen[0] = 'B';
		else
			// Stroke path, leaving it unfilled
			PathTypeOpen[0] = 'S';
	}

	// make closed varity of PathType
	PathTypeClosed[0] = _totlower(PathTypeOpen[0]);
	PathTypeClosed[1] = '\0';

	// Reset to start of path
	ReadPos   = 0;

	// loop through the whole path
	while(ReadPos < NumCoords)
	{
		// Find out the type of element that we are over (after the close flag has been removed)
		Coord P[4];
		switch ( (Verbs[ReadPos]) & (~PT_CLOSEFIGURE) )
		{
			case PT_MOVETO:
				// If this is in the middle of the path, specify how this sub-path
				// should be rendered - this is needed so that we re-import all the flags
				// correctly on complex paths (e.g. PT_CLOSEFIGURE)
				// Karim 25/05/2000 - changed so we don't output this stroking token if PureDataOnly.
				if (ReadPos > 0 && !PureDataOnly)
				{
					pDC->OutputToken(((Verbs[ReadPos - 1] & PT_CLOSEFIGURE) != 0)?PathTypeClosed:PathTypeOpen);
					pDC->OutputNewLine();
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

	// Karim 24/05/2000 - if PureDataOnly is specified, then we *really* don't want to render here!
	if (!PureDataOnly && (!DataOnly || CompoundPath))
	{
		// Write out any additional info it's a gradient fill.
		if (pFillAttr->IsAGradFill ())
		{
			WriteGradientFillInstance ();
		}

			// Do the final (or possibly only) render command
		pDC->OutputToken(((Verbs[NumCoords - 1] & PT_CLOSEFIGURE) != 0)?PathTypeClosed:PathTypeOpen);
		pDC->OutputNewLine();
	}

	// Karim 24/05/2000 - don't want this bracket data if the PureDataOnly flag is set.
	if (CompoundPath && !PureDataOnly)
	{
		// End compound path
		pDC->OutputToken("*U");
		pDC->OutputNewLine();
	}
}

/********************************************************************************************

>	BOOL EPSRenderRegion::SetLastOutputAttribute(INT32 AttrID)

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/5/95
	Inputs:		AttrID, is the AttrIndex (see kernel\attrmgr.h) of the attribute to set
	Returns:	TRUE,  if the attribute is different, and SHOULD be output
				FALSE, if the attribute is the same as the last one output
	Purpose:	Check to see if the current attribute is the same as the last one that
				was output in EPS.

********************************************************************************************/

BOOL EPSRenderRegion::SetLastOutputAttribute(INT32 AttrID)
{
	ERROR2IF(LastOutputAttrs == NULL, FALSE, "No 'LastOutputAttrs' array in EPSRenderRegion::SetLastOutputAttribute");
	ERROR2IF(CurrentAttrs == NULL, FALSE, "No 'CurrentAttrs' array in EPSRenderRegion::SetLastOutputAttribute");
	ERROR2IF(AttrID > NumCurrentAttrs, FALSE, "Bad AttrID passed to EPSRenderRegion::SetLastOutputAttribute");

	// Find the Current Attr of the same Type
	AttributeValue* pAttr = CurrentAttrs[AttrID].pAttr;

	// Removed by Will. Bitmap output now sets these to NULL, so force certain attributes
	// to be output again.
	//ERROR2IF(pAttr == NULL, FALSE, "Couldn't find Current Attr in EPSRenderRegion::SetLastOutputAttribute");

	// If no attribute of this type has been output yet,
	// or the current attribute is different to the last
	// attribute we output, then set the Last Output
	// attribute to the current one

	if (LastOutputAttrs[AttrID].pAttr == NULL ||
		LastOutputAttrs[AttrID].pAttr->IsDifferent(pAttr))
	{
		if (LastOutputAttrs[AttrID].Temp &&
			LastOutputAttrs[AttrID].pAttr != NULL)
		{
			// Get rid of this copy, as we are about to over write it
			delete LastOutputAttrs[AttrID].pAttr;
		}

		// We always take a copy because blends create attributes that are temporary
		// but are not marked as such (for complex reasons).
		if (TRUE) // (CurrentAttrs[AttrID].Temp)
		{
			// If the current attribute is a temporary attribute,
			// then we need to make a copy of it, otherwise it may
			// have been deleted when we try and look at it later on
			CCRuntimeClass* ObjectType = pAttr->GetRuntimeClass();
			AttributeValue* AttrClone = (AttributeValue*)ObjectType->CreateObject();
			AttrClone->SimpleCopy(pAttr);

			LastOutputAttrs[AttrID].pAttr = AttrClone;	
			LastOutputAttrs[AttrID].Temp = TRUE;	
		}
		else
		{
			// This must be an attribute in the tree, so we know
			// it won't get deleted
			LastOutputAttrs[AttrID].pAttr = pAttr;	
			LastOutputAttrs[AttrID].Temp = FALSE;	
		}

		return TRUE;
	}
	else
	{
		// The current attribute is the same as the last one
		// that was output
		return FALSE;
	}
}

/********************************************************************************************

>	void EPSRenderRegion::ResetOutputAttributes()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/06/95
	Purpose:	Resets the attribute array so that all attributes are considered to
				be invalidated - i.e. the next time an attribute is needed, it will
				always be output, regardless of whether the last attribute output
				was the same as it.
				This is mainly used for PostScript printing when we use the OS to do
				text, because we cannot be sure of how the OS will leave the graphics
				state after doing text output, so we assume nothing, and reset all
				required attributes after doing text output via the OS.

********************************************************************************************/

void EPSRenderRegion::ResetOutputAttributes()
{
	// Delete any temp attributes and mark all attrs as NULL.
	if (LastOutputAttrs != NULL)
	{
		for (INT32 i = 0; i < NumCurrentAttrs; i++)
		{
			if (LastOutputAttrs[i].Temp)
				delete LastOutputAttrs[i].pAttr;

			LastOutputAttrs[i].pAttr = NULL;
			LastOutputAttrs[i].Temp  = FALSE;
		}
	}
}

/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteFileVersion(KernelDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/05/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error)
	Purpose:	This allows the filter to save out a comment line giving file version
				information. At present, in anything but Native format, it does nothing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteFileVersion(KernelDC *pDC)
{
	// No file version required here...
	return TRUE;
}
	
/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteCompressionState(KernelDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/05/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error)
	Purpose:	This starts the compression process. Called just after we output the document
				information so that this is not compressed and the galleries can access it
				easily.
				In this baseclass version we do nothing as compression would be a very bad
				thing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteCompressionState(KernelDC *pDC)
{
	// No compression required here...
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteEndCompressionState(KernelDC *pDC)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/05/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error)
	Purpose:	This is called once almost everything has been output, just before we output
				the EOF and we fix the EPS header.
				In this baseclass version we do nothing as compression would be a very bad
				thing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteEndCompressionState(KernelDC*)
{
	// No compression required here...
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteGradientFills ( Document *pDocument )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	1/3/00
	Inputs:		Document - The document being outputted.
	Outputs:	-
	Returns:	TRUE if successful.
				FALSE if error (e.g. file/disk error)
	Purpose:	This function is a stub function, which is overwritten in the AI render
				region.

********************************************************************************************/

BOOL EPSRenderRegion::WriteGradientFills ( Document *pDocument )
{
	// We don't do anything, so just return TRUE.
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteDocumentSetup ( Document		*pDocument,
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

BOOL EPSRenderRegion::WriteDocumentSetup ( Document		*pDocument,
										   EPSFilter	*pFilter )
{
	// Do nothing for normal EPS, only AIEPS files.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::EndLayer ()

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/4/00
	Inputs:		-
	Returns:	TRUE	- Success.
				FALSE	- An error occured.
	Purpose:	If there is an existing layer, it writes the end of layer tags, before
				creating a new layer record.
	SeeAlso:	AIEPSRenderRegion::EndLayer ()

********************************************************************************************/

BOOL EPSRenderRegion::EndLayer ()
{
	// Don't do anything.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteEPSVersion ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/3/000
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes the EPS version used by this particular file type.
	SeeAlso:	EPSRenderRegion::InitDevice

********************************************************************************************/

BOOL EPSRenderRegion::WriteEPSVersion ( void )
{
	// Cast a pointer to the appropriate DC.
	KernelDC *pDC = static_cast<KernelDC*> ( RenderDC );

	// Output the standard EPS header start token.
	pDC->OutputToken	( "%!PS-Adobe-2.0 EPSF-1.2" );
	pDC->OutputNewLine	();

	// Success.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteEPSBoundingBox ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/5/000
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes an EPS bounding box.
	SeeAlso:	EPSRenderRegion::InitDevice

********************************************************************************************/

BOOL EPSRenderRegion::WriteEPSBoundingBox ( void )
{
	// Cast a pointer to the appropriate DC.
	KernelDC	*pDC	= static_cast<KernelDC*> ( RenderDC );
	DocRect		BBox	= RenderSpread->GetBoundingRect ();

	// Bounding box type stuff - get the spread's bounding box and convert from DocCoords
	// to spread coords.
	RenderSpread->DocCoordToSpreadCoord(&BBox);

	// Write the bounding box to the file.
	pDC->OutputToken	( "%%BoundingBox:" );
	pDC->OutputCoord	( BBox.lo, ACCURACY_ROUNDDOWN );
	pDC->OutputCoord	( BBox.hi, ACCURACY_ROUNDUP );
	pDC->OutputNewLine	();

	// Success.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteEPSProcessColours ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/000
	Inputs:		pDC - The device context to output to.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Does nothing - process colours are only relevant in AI files.
	SeeAlso:	EPSRenderRegion::InitDevice

********************************************************************************************/

BOOL EPSRenderRegion::WriteEPSProcessColours ( void )
{
	// It always works! :)
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteEPSResources ( EPSFilter	*pFilter,
											  Document	*pDocument )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/000
	Inputs:		pDC - The device context to output to.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	This is a stub function, that's over-loaded in the AIEPS render region. It's
				used to write out the necessary resources for an AI file.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteEPSResources ( EPSFilter	*pFilter,
										  Document	*pDocument )
{
	// We don't need to do anything yet.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteEPSProlog ( EPSFilter	*pFilter,
										   Document		*pDocument )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/3/000
	Inputs:		pFilter		- The output filter being used.
				pDocument	- The document being exported.
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	This is a stub function, that's over-loaded in the AIEPS render region. It's
				used to write out the necessary Prolog script for an AI file.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteEPSProlog ( EPSFilter	*pFilter,
									   Document		*pDocument )
{
	// We don't need to do anything yet.
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteEPSTrailerComments ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	22/5/000
	Inputs:		-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Writes the comments out at the end of an EPS file.
	SeeAlso:	EPSRenderRegion::CloseDown

********************************************************************************************/

BOOL EPSRenderRegion::WriteEPSTrailerComments ( void )
{
	// Get a pointer to the kernel DC.
	KernelDC	*pDC		= static_cast<KernelDC*>	( RenderDC );
	EPSExportDC	*pExportDC	= static_cast<EPSExportDC*>	( pDC );
	EPSFilter	*pFilter	= static_cast<EPSFilter *>	( pExportDC->GetParentFilter () );
	Document	*pDocument	= RenderView->GetDoc ();
	// Find out which document we're using.
	ENSURE ( pDocument != NULL, "View's document is NULL!" );

	// Write out the page trailer.
	pDC->OutputToken	( "%%PageTrailer" );
	pDC->OutputNewLine	();
	pDC->OutputToken	( "showpage" );
	pDC->OutputNewLine	();
	pDC->OutputToken	( "%%Trailer" );
	pDC->OutputNewLine	();

	// Write out the Documents trailer comments
	pDocument->WriteEPSTrailer ( pFilter );

	// Close down the compression system before we write out the fixed form of the header.
	WriteEndCompressionState ( pDC );

	// End of file token.
	pDC->OutputToken	( "%%EOF" );
	pDC->OutputNewLine	();

	// Success!
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteProlog(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output any PostScript prolog for this render region.  For EPS and printing,
				this means output of our PostScript rendering procedures; for Native
				files we do nothing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteProlog(KernelDC *pDC)
{
	// No PostScript procedure defns here...
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSRenderRegion::WriteSetup(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output any PostScript setup for this render region.  For EPS and printing,
				this means output of our PostScript code to initialise the context for
				rendering; for Native files we do nothing.
	SeeAlso:	PrintPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL EPSRenderRegion::WriteSetup(KernelDC *pDC)
{
	// No PostScript setup code here...
	return TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteSepTables(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the current set of device printer profiles as Postscript data
				arrays. Our Postscript prolog functions will use these arrays when
				creating separations. (if we are not separating, we do nothing)
	SeeAlso:	

********************************************************************************************/

BOOL EPSRenderRegion::WriteSepTables(KernelDC *pDC)
{
	// No Postscript separation tables here
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WriteSepTables(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/06/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the current set of separation functions for this EPS file if
				any. These will usually be present in printable postscript versions of
				EPS render regions.
	SeeAlso:	WriteProlog

********************************************************************************************/

BOOL EPSRenderRegion::WriteSepFunctions(KernelDC *pDC)
{
	// No Postscript separation functions here
	return TRUE;
}


/********************************************************************************************

>	virtual void EPSRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	10/4/95
	Outputs:	pCaps - The details about what types of thing this render region can render
	Purpose:	This function allows render regions to admit to what they can and can not
				render. This allows other areas of the program to come in and help render
				regions out in some situations, if they are unable to render everything.
				eg. an OSRenderRegion can not render transparancy.

********************************************************************************************/

void EPSRenderRegion::GetRenderRegionCaps(RRCaps* pCaps)
{
	// This forces line attributes - dash patterns and arrow heads to be output
	// properly for EPS, rather than using Camelot 'shorthand' tokens.
	pCaps->CanDoNothing();
	pCaps->ClippedOutput = TRUE;
}

/********************************************************************************************

>	virtual BOOL EPSRenderRegion::WantsGrids()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	12/12/95
	Returns: 	FALSE
	Purpose:	This function is designed to help with the rendering of grids, as they are
				not always wanted (eg xara eps). EPS classes generally do not want to render
				grids. The ones that do (NativeEPS) should replace this with a version that
				returns TRUE.

********************************************************************************************/

BOOL EPSRenderRegion::WantsGrids()
{
	return FALSE;
}



/********************************************************************************************

>	virtual void EPSRenderRegion::SetClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 May 2000
	Inputs:		pClipAttr	ptr to the new ClipRegionAttribute to set as current in our RR.
				Temp		whether the attr is temporary or not.

	Outputs:	Postscript's context is saved and the commands are output to intersect its
				current clipping path with that defined by the ClipAttr.
				Also, the RR's current attr context is updated to use this new ClipAttr.

	Purpose:	Shrink the RR's clipping region to the region defined by pClipAttr.
				We do this by:
					1.	Recording a save-state command.
					2.	Exporting a description of the clipping path in pClipAttr.
					3.	Recording a 'clip-to-last-path' command.
					4.	Recording a 'start-new-path' command, as we don't want our clip-path
						to get stroked or rendered in any way.

	Notes:		We mustn't record clipping commands when rendering the default attribute.
				Therefore this method does *nothing whatsoever* unless pClipAttr's ptr to
				its clipping-path is non-NULL. This ptr is NULL in the default attr.

	See also:	RenderRegion::SetClipRegion()

********************************************************************************************/
void EPSRenderRegion::SetClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)
{
	if (pClipAttr->GetClipPath() != NULL)
	{
		// Update the RR's table of current attributes.
		RenderRegion::SetClipRegion(pClipAttr, Temp);

		// Get the device context for our export file.
		KernelDC *pDC = (KernelDC *) RenderDC;

		// export a 'gsave' command.
		pDC->OutputToken(TEXT("gsave"));
		pDC->OutputNewLine();

		// export a description of the clipping path - data only, please.
		ExportPath(pClipAttr->GetClipPath(), TRUE, TRUE);

		// export a 'clip' command. note that if our current winding rule is for even-odd
		// winding, we must export an 'eoclip' command instead.
		if (RR_WINDINGRULE() == EvenOddWinding)
			pDC->OutputToken(TEXT("eoclip"));
		else
			pDC->OutputToken(TEXT("clip"));
		pDC->OutputNewLine();

		// export a 'newpath' command.
		pDC->OutputToken(TEXT("newpath"));
		pDC->OutputNewLine();
	}
}



/********************************************************************************************

>	virtual void EPSRenderRegion::RestoreClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)

	Author:		Karim_MacDonald (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23 May 2000
	Inputs:		pClipAttr	ptr to the ClipRegionAttribute to restore over the current one.
				Temp		whether the attr is temporary or not.

	Outputs:	Postscript is told to restore its last saved context.

	Purpose:	Restore the RR's clipping region.
				We do this by recording a restore-state command.

	Errors:		ERROR3 if the current ClipRegionAttribute holds a NULL clip-path ptr.
				This state of affairs should not happen. You can't set a ClipRegionAttribute
				with a NULL clip-path ptr, so the only such attr is the default one, and you
				should never be restoring over _that_.

	See also:	RenderRegion::RestoreClipRegion()

********************************************************************************************/

void EPSRenderRegion::RestoreClipRegion(ClipRegionAttribute* pClipAttr, BOOL Temp)
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

		// Get the device context for our export file.
		KernelDC *pDC = (KernelDC *) RenderDC;

		// export a 'grestore' command.
		pDC->OutputToken(TEXT("grestore"));
		pDC->OutputNewLine();
	}
}

/********************************************************************************************

>	void EPSRenderRegion::WriteGradientFillInstance()

	Author:		Chris_Gallimore (Xara Group Ltd) <camelotdev@xara.com> 
	Created:	18/12/00
	Purpose:	This function allows extra work to be done on gradient fills, when the fill
				and/or stroke operators are written. i.e. at the last possible moment before
				rendering.

				It is currently only used by the AI export, as this requires gradient fills
				to be written at this position, and re-written for each new object.

	SeeAlso:	AIEPSRenderRegion::WriteGradientFillUsage

********************************************************************************************/

void EPSRenderRegion::WriteGradientFillInstance ()
{
	// do absolutely nothing, unless it's AI (which should be picked up in AIEPSRenderRegion)
}

//
// EPSExportDC class
//


/********************************************************************************************

>	EPSExportDC::EPSExportDC(Filter *Parent)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		The filter object associated with this export DC.
	Purpose:	Initialise a DC for exporting EPS.

********************************************************************************************/

EPSExportDC::EPSExportDC(Filter *Parent) : ExportDC(Parent)
{
}



/********************************************************************************************

>	BOOL EPSExport::Init(CCLexFile* pFile)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	2/2/95
	Inputs:		pFile - The file to attach to the DC
	Returns:	TRUE if all went well, FALSE if not
	Purpose:	Calls the base class Init function and then sets up the DC to throw exceptions
				when it gets errors as that is how the EPS stuff expects errors to be reported.
	SeeAlso:	ExportDC::Init

********************************************************************************************/

BOOL EPSExportDC::Init(CCLexFile* pFile)
{
	// First get the base class to do its thing
	if (!ExportDC::Init(pFile)) return FALSE;

	// now do what I want done.
	// We want export files to throw exceptions, and not report errors.
	ExportFile->SetThrowExceptions(TRUE);
	ExportFile->SetReportErrors(FALSE);
	return TRUE;
}



/********************************************************************************************

>	BOOL EPSExportDC::OutputNewLine()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Causes a new line to be started in the EPS output file.   This is used
				to give a pleasant appearance to the EPS file - most EPS commands (as
				opposed to operands) are followed by a new line.
	SeeAlso:	EPSExportDC; EPSExportDC::OutputToken
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL EPSExportDC::OutputNewLine()
{
	// Graeme (22-2-00) - Windows uses \r\n as the newline code in its files.
	static char NewLine[] = "\r\n";
	if (ExportFile->write(NewLine, 2).fail())
		// Error occured
		return FALSE;

	LineWidth = 0;

	// Success
	return TRUE;
}

/********************************************************************************************

>	BOOL EPSExportDC::OutputToken(TCHAR *Str)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	28/03/94
	Inputs:		Str - the character string to write to the file.
	Returns:	TRUE if the data was written ok;
				FALSE if not => ERROR1
	Purpose:	Outputs a string token to the EPS file.
				This is the central routine through which the other high-level routines
				eventually come. The other routines convert their output to a string,
				which theythen pass on to this routine.
				A record is kept of the current line width - if it is over 70 characters
				wide before the token is output, then a new line is output to keep
				the lines in the EPS file reasonably short.
				For this reason, it is important not to output strings that contain
				newline characters, because this routine will not notice, and hence
				the LineWidth count will be wrong.
				This routine also ensures that tokens are separated by either a space
				or a newline, so it is not necessary to output spaces directly to keep the 
				tokens separate - it happens automatically.
	SeeAlso:	EPSExportDC
	Errors:		Disk/file error => ERROR1

********************************************************************************************/

BOOL EPSExportDC::OutputToken(TCHAR *Str)
{
	// Special tokens
	static char Space = ' ';

	if (LineWidth > 100)
	{
		// (ChrisG 8/12/00) We now have only one way of writing a newline out. The old method
		//	used "/n/r" to write a newline, which doesn't work, while OutputNewLine used "/r/n",
		//	which does, Also the max line width has been expanded to 100 characters.
		if (OutputNewLine () == FALSE)
			// Error
			return FALSE;
	}

	// Pad with a space (unless at the beginning of the line)
	if (LineWidth > 0)
	{
		if (ExportFile->write(&Space, 1).fail())
			// Error
			return FALSE;
		LineWidth++;
	}

	// Write the token out to the file
	INT32 Len = cc_strlenBytes(Str);
	if (ExportFile->write(Str, Len).fail())
		// Error
		return FALSE;

	LineWidth += Len;

	// Success
	return TRUE;
}


/********************************************************************************************

>	BOOL EPSExportDC::OutputDirect(BYTE *Buf, INT32 nBytes)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/23/95
	Inputs:		Buf - the bytes to send to the stream.
				nBytes - the number of bytes to send to the stream.
	Returns:	TRUE if all the bytes were sent ok;
				FALSE if not.
	Purpose:	Send bytes directly to the PostScript stream with no alteration or padding.
				Used for sending binary/hex data to stream.
	SeeAlso:	KernelDC::OutputNewLine; KernelDC::OutputToken

********************************************************************************************/

BOOL EPSExportDC::OutputDirect(BYTE *Buf, INT32 nBytes)
{
	if (ExportFile->write(Buf, nBytes).fail())
	{
		// Error
		return FALSE;
	}

	// All ok
	return TRUE;
}



