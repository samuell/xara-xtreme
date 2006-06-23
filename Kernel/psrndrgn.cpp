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

// Render region for rendering to PostScript devices.

/*
*/

#include "camtypes.h"

#include "psrndrgn.h"
#include "psdc.h"
#include "page.h"
//#include "spread.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "document.h" - in camtypes.h [AUTOMATICALLY REMOVED]
//#include "fillval.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "textfuns.h"
#include "devcolor.h"
#include "camelot.h"
#include "colplate.h"
//#include "isetres.h"
//#include "tim.h"
#include "oilfiles.h"
#include "unicdman.h"  //For MBCS support
//#include "xaracms.h"
//#include "app.h" - in camtypes.h [AUTOMATICALLY REMOVED]
#include "printctl.h"
//#include "view.h" - in camtypes.h [AUTOMATICALLY REMOVED]

DECLARE_SOURCE("$Revision$");

CC_IMPLEMENT_DYNAMIC(PrintPSRenderRegion, CamelotEPSRenderRegion)

#define new CAM_DEBUG_NEW


/********************************************************************************************

>	PrintPSRenderRegion(DocRect ClipRect, Matrix ConvertMatrix, FIXED16 ViewScale)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Purpose:	Initialise a render region for printing PostScript.
	SeeAlso:	CamelotEPSRenderRegion::CamelotEPSRenderRegion

********************************************************************************************/

PrintPSRenderRegion::PrintPSRenderRegion(DocRect ClipRect,
										 Matrix ConvertMatrix, 
										 FIXED16 ViewScale) 
	: CamelotEPSRenderRegion(ClipRect, ConvertMatrix, ViewScale)
{
	// We're a printing render region.
	RenderFlags.Printing = TRUE;
}

/********************************************************************************************

>	PrintPSRenderRegion::~PrintPSRenderRegion()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	15/04/94
	Purpose:	Delete the region, flushing the PostScript output DC beforehand.
	SeeAlso:	PSPrintDC::Flush

********************************************************************************************/

PrintPSRenderRegion::~PrintPSRenderRegion()
{
	PSPrintDC *pPSPrintDC = (PSPrintDC *) CCDC::ConvertFromNativeDC(RenderDC);

	// Restore OS context - just pretend we want to do some OS output
	pPSPrintDC->StartOSOutput();

	// Flush the DC
	pPSPrintDC->FlushDC();

	pPSPrintDC->StartOSOutput();

	// Karim 06/06/2000 - free the memory! (hope this doesn't blow up something else...)
	// AB: Don't do this. RenderRegion's destructor deletes RenderDC
	//	delete pPSPrintDC;
	//	pPSPrintDC = NULL;
}



/********************************************************************************************

>	BOOL PrintPSRenderRegion::InitDevice()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	27/05/94
	Returns:	TRUE if the device context is initialised ok;
				FALSE if not.
	Purpose:	Initialise the device specific mechanisms for this render region.
				For a PrintPSRenderRegion we need to change the DC slightly  - we use the
				existing RenderDC to make a PSPrintDC, which allows us to output PostScript
				directly to the device.
	Errors:		Same as base class.
	SeeAlso:	RenderRegion::InitDevice

********************************************************************************************/

BOOL PrintPSRenderRegion::InitDevice()
{
	// Ensure the current DC is suitable for postscript rendering

	CCDC *pCCDC = CCDC::ConvertFromNativeDC( RenderDC );
	ERROR2IF(!pCCDC || !pCCDC->IsKindOf(CC_RUNTIME_CLASS(PSPrintDC)), FALSE, "Trying to InitDevice on a non-Postscript CCDC");

	PSPrintDC *pPSPrintDC = (PSPrintDC *) pCCDC;

	pPSPrintDC->SetDCTransforms(RenderMatrix, RenderView);

	// Call base class - note that unlike other InitDevice() implementations, we don't
	// call this as the very first thing in the function - this is because we need to switch
	// the DC beforehand, as seen above.
	if (!RenderRegion::InitDevice())
		return FALSE;

	// Find out what this region can do
	GetRenderRegionCaps(&Caps);

	// All ok
	return TRUE;
}

/********************************************************************************************

>	BOOL PrintPSRenderRegion::StartRender()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/03/94
	Returns:	TRUE if worked, FALSE if failed.
	Purpose:	Prepare the render region for rendering (exporting).
	SeeAlso:	EPSRenderRegion::Initialise; EPSRenderRegion::StopRender

********************************************************************************************/

BOOL PrintPSRenderRegion::StartRender()
{
	// If we are being restarted, reinstate our dictionary on the stack
//	BOOL Restarted = RenderFlags.ValidDevice;

	// Call base class first
	if (!EPSRenderRegion::StartRender())
		return FALSE;

	// Attach the DC to this render region.
	PSPrintDC *pPSPrintDC = (PSPrintDC *) CCDC::ConvertFromNativeDC(RenderDC);
	pPSPrintDC->SetDCTransforms(RenderMatrix, RenderView);
	pPSPrintDC->AttachRenderRegion(this);

	Initialise();

	return TRUE;
}



/********************************************************************************************

>	BOOL PrintPSRenderRegion::StopRender()

	Author:		Will_Cowling (Xara Group Ltd) <camelotdev@xara.com>
	Created:	11/5/93
	Inputs:		RenderState is a Node* pointing at the current rendering node in the tree,
				or NULL if all objects rendered.
	Purpose:	Stops the rendering of a OSRenderRegion, saving it's current renderstate so
				that rendering can continue where it left off, later on.  If the RenderState
				passed is NULL then the RenderRegion will be unlinked from the list and will
				then delete itself.
	SeeAlso:	OSRenderRegion::StartRender()

********************************************************************************************/

BOOL PrintPSRenderRegion::StopRender()
{
	// Call base class
	BOOL bHaveRendered = EPSRenderRegion::StopRender();

	// Flush our buffered PostScript device context.
	PSPrintDC *pPSPrintDC = (PSPrintDC *) CCDC::ConvertFromNativeDC(RenderDC);
	pPSPrintDC->FlushDC();

	// Detach the DC from this render region.
	pPSPrintDC->DetachRenderRegion();

	return bHaveRendered;
}

/********************************************************************************************

>	BOOL PrintPSRenderRegion::CloseDown()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/09/95
	Returns:	TRUE (always)
	Purpose:	Close down a PostScript render region - in fact we do nothing as the
				printer driver is responsible for tidying up, and we will already have
				cleared our dictionary from the dict stack in StopRender()
	SeeAlso:	CamelotEPSRenderRegion::StopRender

********************************************************************************************/

BOOL PrintPSRenderRegion::CloseDown()
{
	// Do nothing - the printer driver should do it all for us...
	return TRUE;
}

/********************************************************************************************

>	virtual void PrintPSRenderRegion::ConditionalSuicide ( void )

	Author:		Graeme_Sutherland (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/2/00
	Inputs:		-
	Returns:	-
	Purpose:	Causes the object to commit suicide. This is to get around using a few
				if IS_A calls elsewhere in Camelot.

********************************************************************************************/

void PrintPSRenderRegion::ConditionalSuicide ( void )
{
	// Delete the object.
	delete this;
}

/********************************************************************************************

>	BOOL PrintPSRenderRegion::WriteProlog(KernelDC *pDC)

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

BOOL PrintPSRenderRegion::WriteProlog(KernelDC *pDC)
{
	// Call base class to output our dictionary
	if (!CamelotEPSRenderRegion::WriteProlog(pDC))
		// Error
		return FALSE;

	// All done
	return TRUE;
}

/********************************************************************************************

>	BOOL PrintPSRenderRegion::WriteSetup(KernelDC *pDC)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/04/95
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output any PostScript setup for this render region.  For EPS and printing,
				this means output of our PostScript code to initialise the context for
				rendering; for Native files we do nothing.
	SeeAlso:	EPSRenderRegion::WriteSetup

********************************************************************************************/

BOOL PrintPSRenderRegion::WriteSetup(KernelDC *pDC)
{
	// Call base class to output our setup code
	if (!CamelotEPSRenderRegion::WriteSetup(pDC))
		// Error
		return FALSE;

	// All ok
	return TRUE;
}


/********************************************************************************************

>	virtual BOOL PrintPSRenderRegion::WriteSepTables(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the current set of device printer profiles as Postscript hex
				arrays. Our Postscript prolog functions will use these arrays when
				creating separations. (if we are not separating, we do nothing)
	SeeAlso:	

********************************************************************************************/

BOOL PrintPSRenderRegion::WriteSepTables(KernelDC *pDC)
{
	ColourContext* pContext;
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, &pContext, &pSeparation);

	// Note, we return true to all but the token output
	// functions. We will terminate the output only if
	// we receive a disc failure.
	
	// do nothing if we are not separating
	if (pSeparation==NULL)
		return TRUE;

	// check that we've got a colour context attached to us.
	if (pContext==NULL)
	{
		ERROR3("There is no current colour context in this render region!");
		return TRUE;
	}

	// Whip out the physical tables we will use to separate.
	BYTE Table[5*256];
	if (!pContext->GetProfileTables(Table))
		return TRUE;

	// output cyan
	BOOL ok = pDC->OutputToken(_T("/ccurve ["));
	ok = ok && WriteSepTablesHelper(pDC, Table);
	ok = ok && pDC->OutputToken(_T("] def"));
	ok = ok && pDC->OutputNewLine();

	// output magenta
	ok = ok && pDC->OutputToken(_T("/mcurve ["));
	ok = ok && WriteSepTablesHelper(pDC, Table+256);
	ok = ok && pDC->OutputToken(_T("] def"));
	ok = ok && pDC->OutputNewLine();

	// output yellow
	ok = ok && pDC->OutputToken(_T("/ycurve ["));
	ok = ok && WriteSepTablesHelper(pDC, Table+512);
	ok = ok && pDC->OutputToken(_T("] def"));
	ok = ok && pDC->OutputNewLine();

	// output ucr
	ok = ok && pDC->OutputToken(_T("/ucurve ["));
	ok = ok && WriteSepTablesHelper(pDC, Table+768);
	ok = ok && pDC->OutputToken(_T("] def"));
	ok = ok && pDC->OutputNewLine();

	// output black generation
	ok = ok && pDC->OutputToken(_T("/bcurve ["));
	ok = ok && WriteSepTablesHelper(pDC, Table+1024);
	ok = ok && pDC->OutputToken(_T("] def"));
	ok = ok && pDC->OutputNewLine();

	// Enable separations
	ok = ok && pDC->OutputToken(_T("1 setseps"));
	ok = ok && pDC->OutputNewLine();

	// Is this a mono plate?
	BOOL mono = pSeparation->IsMonochrome();

	// find what type of separation plate we are using
	ColourPlateType Type = pSeparation->GetType();
	switch (Type)
	{
		case COLOURPLATE_CYAN:
			ok = ok && pDC->OutputToken(_T("v_cpcy setplate"));
			ok = ok && pDC->OutputNewLine();
		break;
		case COLOURPLATE_MAGENTA:
			ok = ok && pDC->OutputToken(_T("v_cpmg setplate"));
			ok = ok && pDC->OutputNewLine();
		break;
		case COLOURPLATE_YELLOW:
			ok = ok && pDC->OutputToken(_T("v_cpyl setplate"));
			ok = ok && pDC->OutputNewLine();
		break;
		case COLOURPLATE_KEY:
			ok = ok && pDC->OutputToken(_T("v_cpky setplate"));
			ok = ok && pDC->OutputNewLine();
		break;
		case COLOURPLATE_SPOT:
			// Force mono to be true here, all spot colour will
			// be going out as weights of key, and hence are
			// really the same as mono c,m,y,k
			mono=TRUE;
		break;
		default:
			ERROR3("What kind of a plate is that?");
			break;

	}

	// Output the mono setting. This determins whether our postscript
	// colour fill and stroke functions will set colour using the
	// setgray function or setcmyk
	if (mono)
		ok = ok && pDC->OutputToken(_T("1 setmono"));
	else
		ok = ok && pDC->OutputToken(_T("0 setmono"));
	ok = ok && pDC->OutputNewLine();

	// All ok
	return ok;
}


/********************************************************************************************

>	BOOL PrintPSRenderRegion::WriteSepTablesHelper(KernelDC *pDC, BYTE* Table)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/06/96
	Inputs:		pDC - the device context to output to.
				Table = a pointer to a 256 byte entry table
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the table of 256 values to the output stream

********************************************************************************************/

BOOL PrintPSRenderRegion::WriteSepTablesHelper(KernelDC *pDC, BYTE* Table)
{
	BOOL ok;
	UINT32 val;
	for (INT32 i=0; i<256; i++)
	{
		val = (UINT32) (Table[i]);
		ok = pDC->OutputValue(val);
		if (!ok)
			return FALSE;
	}
	return TRUE;
}



/********************************************************************************************

>	BOOL PrintPSRenderRegion::WritePlateName(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the current plate name for this print separation, if there is one.

********************************************************************************************/

BOOL PrintPSRenderRegion::WritePlateName(KernelDC *pDC)
{
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, NULL, &pSeparation);
	
	// do nothing if we are not separating
	if (pSeparation==NULL)
		return TRUE;

	// Get and output the plate name, cheers
	String_64 platename;
	pSeparation->GetDescription(&platename);
	TCHAR* pPlate = (TCHAR*)platename;
	// ok output the textural name
	BOOL ok = pDC->OutputToken(_T("%%PlateColor :"));
	ok = ok && pDC->OutputToken(pPlate);
	ok = ok && pDC->OutputNewLine();

	return ok;
}




/********************************************************************************************

>	BOOL PrintPSRenderRegion::WritePlateScreen(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/06/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the setscreen function for this plate.

********************************************************************************************/

BOOL PrintPSRenderRegion::WritePlateScreen(KernelDC *pDC)
{
	PrintControl *pPrintCtl=NULL;
	View *pView = GetRenderView();
	if (pView) pPrintCtl = pView->GetPrintControl();
	if (!pPrintCtl)
		return TRUE;

	// Get a pointer to the typeset info structure
	TypesetInfo *pInfo = pPrintCtl->GetTypesetInfo();
	
	double ang,freq;
	String_256 ScreenName;
	ScreenType scrtype;

	// If separating then interogate the current plate
	if (pInfo->AreSeparating())
	{
		ColourPlate* pSeparation;
		GetOutputColourPlate(COLOURMODEL_CMYK, NULL, &pSeparation);
		
		// do nothing if we are not separating
		if (pSeparation==NULL)
			return TRUE;

		// Make sure screening is on in this plate
		if (!pSeparation->ActiveScreening())
			return TRUE;

		// Get the screen type if enabled.
		scrtype = pSeparation->GetScreenFunction();
		if (scrtype==SCRTYPE_NONE)
			return TRUE;

		// ok we can get the angle and frequency
		ang = pSeparation->GetScreenAngle();
		freq = pSeparation->GetScreenFrequency();
	}
	else
	{
		// Is screening off?
		if (!pInfo->AreScreening())
			return TRUE;

		scrtype = pInfo->GetScreenFunction();
		if (scrtype==SCRTYPE_NONE)
			return TRUE;

		ang = 45.0;
		freq = pInfo->GetDefaultScreenFrequency();
	}

	// read the name of this screen
	pInfo->GetScreenName(scrtype, &ScreenName);
		
	String_256 fred;
	fred += String_8(_T("{"));
	fred += ScreenName;
	fred += String_8(_T("}"));

	// ok output 'freq ang screenfunc setscreen'
	BOOL ok = pDC->OutputFloat(freq, 4);
	ok = ok && pDC->OutputFloat(ang, 4);
	ok = ok && pDC->OutputToken(fred);
	ok = ok && pDC->OutputToken(_T("setscreen"));
	ok = ok && pDC->OutputNewLine();

	return ok;
}



/********************************************************************************************

>	BOOL PrintPSRenderRegion::WriteSepFunctions(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	26/06/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Output the setscreen functions file for this separation.
	SeeAlso:	WriteProlog

********************************************************************************************/

BOOL PrintPSRenderRegion::WriteSepFunctions(KernelDC *pDC)
{
	PrintControl *pPrintCtl=NULL;
	View *pView = GetRenderView();
	if (pView) pPrintCtl = pView->GetPrintControl();
	if (!pPrintCtl)
		return TRUE;

	// Get a pointer to the typeset info structure
	TypesetInfo *pInfo = pPrintCtl->GetTypesetInfo();
	// Is screening off?
	if (!pInfo->AreScreening())
		return TRUE;

	// Get hold of our PostScript prolog resource...
	CCResTextFile ScreenFile;

	// Open the file
	if (!ScreenFile.open(_R(IDM_PS_SPOTFUNCS), _R(IDT_PS_RES)))
	{
		// Failed to open the file...
		ERROR2(FALSE, "Could not get at PostScript resource!");
	} 

	// Read each line from the file and output it to the DC.
	String_256 LineBuf;
	TCHAR *pBuf = (TCHAR *) LineBuf;
	
	while (!ScreenFile.eof())
	{
		// Copy this line to output.
		ScreenFile.read(&LineBuf);
		pDC->OutputTCHARAsChar(pBuf, LineBuf.Length());
		pDC->OutputNewLine();
	}
	
	// All done
	ScreenFile.close();

	return TRUE;
}




/********************************************************************************************

>	BOOL PrintPSRenderRegion::PushClipRegion(KernelDC *pDC, const DocRect& Rect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/96
	Inputs:		pDC - the device context to output to.
				Rect - the clipping rect to create.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Stack the PostScript clipping rectangle. The clipping rectangle will be
				active until our context is reset.
	SeeAlso:	PopClipRegion

********************************************************************************************/

BOOL PrintPSRenderRegion::PushClipRegion(KernelDC *pDC, const DocRect& Rect)
{
	BOOL ok = pDC->OutputToken(_T("gs"));
	ok = ok && pDC->OutputNewLine();
	ok = ok && WriteClipRegion(pDC,Rect);
	return ok;
}


/********************************************************************************************

>	BOOL PrintPSRenderRegion::PopClipRegion(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Unstack the current PostScript clip region. We simply pop the graphics state
				here to return to the previous clipping region.
	SeeAlso:	PushClipRegion

********************************************************************************************/

BOOL PrintPSRenderRegion::PopClipRegion(KernelDC *pDC)
{
	BOOL ok = pDC->OutputToken(_T("gr"));
	ok = ok && pDC->OutputNewLine();
	return ok;
}




/********************************************************************************************

>	BOOL PrintPSRenderRegion::WriteClipRegion(KernelDC *pDC, const DocRect& Rect)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/96
	Inputs:		pDC - the device context to output to.
				Rect - the clipping rect to create.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Set up a PostScript clipping rectangle. The clipping rectangle will be
				active until our context is reset.

********************************************************************************************/

BOOL PrintPSRenderRegion::WriteClipRegion(KernelDC *pDC, const DocRect& Rect)
{
	if (!Rect.IsValid() || Rect.IsEmpty())
		return TRUE;

	DocCoord c0,c1;
	c0=Rect.lo;
	c1=Rect.hi;
	BOOL ok = pDC->OutputCoord(c0);
	ok = ok && pDC->OutputCoord(c1);
	ok = ok && pDC->OutputToken(TEXT("Cp"));
	ok = ok && pDC->OutputNewLine();

	return ok;
}


/********************************************************************************************

>	BOOL PrintPSRenderRegion::WritePhotoNegative(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Start photo negative rendering. All rendering will be photo-negated.

********************************************************************************************/

BOOL PrintPSRenderRegion::WritePhotoNegative(KernelDC *pDC)
{
/*
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, NULL, &pSeparation);

	// do nothing if we are not separating
	if (pSeparation==NULL)
		return TRUE;

	// if this plate says negative lets do it
	if (pSeparation->IsEPSNegative())
		return WriteSetTransfer(pDC);
*/
	// Find the print control structure.
	PrintControl *pControl = GetRenderView()->GetPrintControl();
	if (pControl)
	{
		TypesetInfo *pTypeset = pControl->GetTypesetInfo();
		if (pTypeset && pTypeset->PrintPhotoNegative())
			return WriteSetTransfer(pDC);
	}

	return TRUE;
}

BOOL PrintPSRenderRegion::WriteSetTransfer(KernelDC *pDC)
{
	BOOL ok = pDC->OutputToken(_T("{1 exch sub} settransfer"));
	     ok = ok && pDC->OutputNewLine();
	return ok;
}


/********************************************************************************************

>	BOOL PrintPSRenderRegion::WriteRenderPaper(KernelDC *pDC)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	29/08/96
	Inputs:		pDC - the device context to output to.
	Outputs:	-
	Returns:	TRUE if ok;
				FALSE if error (e.g. file/disk error or printer driver error)
	Purpose:	Fill the entire renderable area with white

********************************************************************************************/

BOOL PrintPSRenderRegion::WriteRenderPaper(KernelDC *pDC)
{
/*
	ColourPlate* pSeparation;
	GetOutputColourPlate(COLOURMODEL_CMYK, NULL, &pSeparation);

	// do nothing if we are not separating
	if (pSeparation==NULL)
		return TRUE;

	// if this plate says negative lets do it
	if (pSeparation->IsEPSNegative())
		return WriteFillPaper(pDC);
*/
	// Find the print control structure.
	PrintControl *pControl = GetRenderView()->GetPrintControl();
	if (pControl)
	{
		TypesetInfo *pTypeset = pControl->GetTypesetInfo();
		if (pTypeset && pTypeset->PrintPhotoNegative())
			return WriteFillPaper(pDC);
	}

	return TRUE;
}


BOOL PrintPSRenderRegion::WriteFillPaper(KernelDC *pDC)
{
	BOOL ok = pDC->OutputToken(_T("gsave clippath 1 setgray fill grestore"));
		 ok = ok && pDC->OutputNewLine();
	return ok;
}







/********************************************************************************************

>	BOOL PrintPSRenderRegion::OutputPSHeader()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com> & JustinF
	Created:	04/29/95
	Returns:	TRUE if ok;
				FALSE if file/printer error occured.
	Purpose:	Output the PostScript header that Camelot needs in order for its PostScript
				code to work.  NB. the original version of this function has been commented
				out and moved to the end of this source file.

********************************************************************************************/

BOOL PrintPSRenderRegion::OutputPSHeader()
{
PORTNOTE("printing", "Don't output strange rectangles")
#ifndef EXCLUDE_FROM_XARALX
	// If running under Win95 or Win31, but not any flavour of WinNT, then begin with some
	// fake GDI output to force the buggy Postscript driver to flush its output buffer.
	if (IsWin32s())
	{
		// Justin says: please *don't* change this to *anything* else until you've
		// spoken to me or Phil.
		CBrush br;
		br.CreateStockObject(WHITE_BRUSH);
		CBrush* pOldBr = RenderDC->SelectObject(&br);

		CPen pn;
		pn.CreateStockObject(WHITE_PEN);
		CPen* pOldPn = RenderDC->SelectObject(&pn);

		// Draw a tiny white rectangle with a white outline.
		TRACEUSER( "JustinF", _T("Postscript pixel fix in action ...\n"));
		RenderDC->Rectangle(&CRect(0, 0, 1, 1));

		RenderDC->SelectObject(pOldPn);
		RenderDC->SelectObject(pOldBr);
	}
#endif

	// Use the current DC to make a new one that is suitable for rendering
	// PostScript to directly.
	PSPrintDC* pPSPrintDC = new PSPrintDC(RenderDC);
	if (pPSPrintDC == NULL) return FALSE;

	// Set up this - don't need it but the DC might get upset if it has no view.
	RenderDC = pPSPrintDC->GetDC();
	pPSPrintDC->SetDCTransforms(RenderMatrix, RenderView);

	// Tell the DC (i) we want to do OS output - this prevents it from outputting a
	// reference to our dictionary before we have defined it (because it's trying to
	// set up the PostScript C ready for Camelot EPS commands); (ii) we have finished
	// doing our 'OS' output. :-)
	return pPSPrintDC->StartOSOutput() && pPSPrintDC->EndOSOutput();
}



/********************************************************************************************

>	void PrintPSRenderRegion::Initialise()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/29/95
	Purpose:	Over-rides the EPS header output code - we don't want to do this everytime
				we start a PostScript render region, only every time we start a print job.
	SeeAlso:	PrintPSRenderRegion::OutputPSHeader

********************************************************************************************/

void PrintPSRenderRegion::Initialise()
{
	// Set up render region
	InitClipping();
	InitAttributes();
}

/********************************************************************************************

>	void PrintPSRenderRegion::DeInitialise()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	04/29/95
	Purpose:	Over-rides the EPS trailer output code - we don't want to do this everytime
				we delete a PostScript render region - the printer driver should do it for 
				us.
	SeeAlso:	PrintPSRenderRegion::OutputPSHeader

********************************************************************************************/

void PrintPSRenderRegion::DeInitialise()
{
	// Clear up
	DeInitAttributes();
}


BOOL PrintPSRenderRegion::InitPSDevice(CNativeDC *pDC, PrintView *pPrintView)
{
	// Make a new PSRenderRegion, and use it to output out PostScript header.
	DocRect DummyRect;
	Matrix DummyMatrix;
	FIXED16 DummyScale(0);
	PrintPSRenderRegion *pRegion = new PrintPSRenderRegion(DummyRect, DummyMatrix, DummyScale);
	if (pRegion == NULL)
		return FALSE;

	// Attach new region to the DC.
	pRegion->AttachDevice((View *) pPrintView, pDC, Document::GetSelectedSpread());

	// Output the header
	if (!pRegion->OutputPSHeader())
		return FALSE;

	delete pRegion;

	return TRUE;
}


/********************************************************************************************

>	BOOL PrintPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	30/4/95
	Inputs:		ch      - unicode value of char
				pMatrix - matrix specifying transforms to place char correctly in document
	Returns:	FALSE if fails
	Purpose:	Render a character,
				using the specified transform and current attributes in the render region.

********************************************************************************************/

BOOL PrintPSRenderRegion::RenderChar(WCHAR ch, Matrix* pMatrix)
{
PORTNOTE("printing", "Disabled PS text rendering")
#ifndef EXCLUDE_FROM_XARALX
	// If it is stroked or not simple flat fill, or not a standard ASCII character then we 
	//	must do this as paths.
	BOOL FlatFill = IS_A(CurrentAttrs[ATTR_FILLGEOMETRY].pAttr, FlatFillAttribute);

	if (!FlatFill || 
		!RR_STROKECOLOUR().IsTransparent() ||
		(FlatFill & RR_FILLCOLOUR().IsTransparent()) ||
		(ch > 255))
		return RenderRegion::RenderChar(ch, pMatrix);

	// Check for emulsion down printing
	PrintControl *pPrintCtl;
	View *pView = GetRenderView();
	if (pView && (pPrintCtl=pView->GetPrintControl())) 
	{ 
		if (pPrintCtl->GetTypesetInfo()->PrintEmulsionDown())
		return RenderRegion::RenderChar(ch, pMatrix);
	}

	// get overall matrix - attribute matrix concatenated with given matrix if supplied
	Matrix matrix;
	if (GetCharAttributeMatrix(&matrix)==FALSE)
		return FALSE;
	if (pMatrix)
		matrix*=*pMatrix;

	// Can we do this using a GDI font?
	// We can if the matrix only specifies scaling and translation
	FIXED16 abcd[4];
	INT32	ef[2];
	matrix.GetComponents(abcd, ef);

	// GDI can't do y-axis flips, so we do it as shapes if this is detected (and x-axis
	// flips, for consistency).
	if ((abcd[0] < FIXED16(0)) || (abcd[3] < FIXED16(0)))
	{
		// Flipped in one or both axes - render as a path.
		return RenderRegion::RenderChar(ch, pMatrix);
	}

	// Work out how complex the transformation is.
	FIXED16 ScaleX, ScaleY;
	ANGLE 	Rotation, Shear;

	if ((abcd[1] == FIXED16(0)) && (abcd[2] == FIXED16(0)))
	{
		// Simple scaling transformation.
		ScaleX 	 = abcd[0];
		ScaleY   = abcd[3];
		Rotation = FIXED16(0);
		Shear 	 = FIXED16(0);
	}
	else
	{
		// Decompose the matrix to find out how complex it is.
		// Pass in NULL for translation as we already know it is in 'ef'.
		FIXED16 Aspect;
		BOOL Result = matrix.Decompose(&ScaleY, &Aspect, &Rotation, &Shear, NULL);

		if (!Result || (Shear != FIXED16(0)))
			// Either there was a problem, or the character is sheared, in which case
			// we can't do it with GDI.
			return RenderRegion::RenderChar(ch, pMatrix);

		// Set up the ScaleX based on the aspect ratio
		ScaleX = ScaleY * Aspect;
	}

	// Check for sideways printing - if the render matrix has rotation, then we are
	// printing at 270 degrees rotation, so adjust the rotation accordingly.
#if 1
	FIXED16 RenderABCD[4];
	INT32	RenderEF[2];
	RenderMatrix.GetComponents(RenderABCD, RenderEF);
	if ((RenderABCD[1] != FIXED16(0)) || (RenderABCD[2] != FIXED16(0)))
		// Rotate by 270 degrees (angle is in radians)
		Rotation += FIXED16(1.5 * PI);
#endif
	// Simple transformation - we can do this with a GDI font.
	PSPrintDC *pPSDC = (PSPrintDC *) CCDC::ConvertFromNativeDC(RenderDC);
	if (!pPSDC->StartOSOutput())
		return FALSE;


	// Work out required width and height of the font
	MILLIPOINT ReferenceSize = TextManager::GetDefaultHeight();
	MILLIPOINT Width  = ReferenceSize * ScaleX;
	MILLIPOINT Height = ReferenceSize * ScaleY;

	if (!pPSDC->SelectNewFont(RR_TXTFONTTYPEFACE(), RR_TXTBOLD(), RR_TXTITALIC(),
					   Width, Height, Rotation))
	{
		// Could not select font (maybe because device can't rotate fonts)
		pPSDC->EndOSOutput();
		return RenderRegion::RenderChar(ch, pMatrix);
	}

	// Ok, so the OS might screw up our attributes/graphics state, so we force
	// attributes to be output specifically the next time we do some Camelot output.
	ResetOutputAttributes();

	// First, set up the text attributes that are not encoded in the font.
	UINT32 OldTextAlign = RenderDC->SetTextAlign(TA_BASELINE);
	INT32 OldBKMode = RenderDC->SetBkMode(TRANSPARENT);
	COLORREF TextColour = ConvertColourToScreenWord(CurrentColContext, &RR_FILLCOLOUR());

	// check for completely black text
	if (TextColour==0 && IsWindowsNT())
	{
		// All components are 0!
		// BODGE TEXT - The driver decides its a good idea not to ouput
		//              0 g when black is (it thinks) already set.
		//				Hence we get the colour previously set. So, we
		//				output a dummy black here, which it doesn't ignore.
		//				Done by Mike (04/07/96). It would be nice to find
		//				out why the driver is ignoring us but time is fleating
		
		TextColour = (DWORD)(0x00010101);
		
		/*
		This really is majorly dangerous! (Read comments in OutputDirect for why)
		We could use the following but we will destroy the graphics state of
		gdi for colour. ie if GDI does a 'save char col fill restore' we will 
		end up doing this
		col save char col fill restore
		and will have mucked up the graph state.

		String_32 LineBuf("0 g");
		LineBuf += '\n';
		TCHAR *pBuf = (TCHAR *) LineBuf;
		pPSDC->OutputTCHARAsChar(pBuf, LineBuf.Length());
		*/
	}

	COLORREF OldTextColour = RenderDC->SetTextColor(TextColour);

	// Render the character in the specified position
	DocCoord DocPos(ef[0], ef[1]);

	// Convert DocCoord to WinCoord
	OilCoord OilPoint;
	RenderMatrix.GetComponents(abcd, ef);
	OilPoint.x = MatrixCalc(abcd[0], DocPos.x, abcd[2], DocPos.y) + ef[0];
	OilPoint.y = MatrixCalc(abcd[1], DocPos.x, abcd[3], DocPos.y) + ef[1];
	WinCoord WinPos = OilPoint.ToWin(RenderView);

	// Graham 5/8/96: "ch" is presently in UNICODE or ASCII
	// We need to convert it over to MBCS to deal with Japanese strings
	//So convert ch, which is of form WCHAR, over to a MBCS UINT32 character index

	UINT32 uiCharNumber = UnicodeManager::UnicodeToMultiByte(ch);

	//Now we want to put that UINT32 value into an array of char ready to pass to
	//RenderDC->TextOut. We do this using UnicodeManager::DecomposeMultiBytes

	BYTE bCharArray[2];

	UnicodeManager::DecomposeMultiBytes(uiCharNumber, &bCharArray[0], &bCharArray[1]);

	//Now, is the character in bCharArray one or two bytes long?
	//If it is one byte long, the first byte in bCharArray will be zero.
	if (bCharArray[0]==0)
		//It's a standard ASCII character, one byte long
		//So pass that character (bCharArray[1]) to the TextOut function.
		//The last parameter in text out is the number of bytes - in this case 1.
		RenderDC->TextOut(WinPos.x, WinPos.y, (CHAR*) &bCharArray[1], 1);
	else
		//The character is two bytes long (that is, it's a foreign character)
		//So we pass bCharArray[0] to TextOut and tell TextOut that it should
		//use two bytes from that address. We do this by setting the last
		//parameter to 2.
		RenderDC->TextOut(WinPos.x, WinPos.y, (CHAR*) &bCharArray[0], 2);

	// Clean up text attributes
	RenderDC->SetTextAlign(OldTextAlign);
	RenderDC->SetBkMode(OldBKMode);
	RenderDC->SetTextColor(OldTextColour);

	// Finished doing GDI output
	if (!pPSDC->EndOSOutput())
		return FALSE;
#endif
	return TRUE;
}

