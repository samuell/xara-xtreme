// $Id: princomp.cpp 662 2006-03-14 21:31:49Z alex $
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
// Implementation of the print document component classes

/*
*/

#include "camtypes.h"

#include "princomp.h"

#include "app.h"
#include "camfiltr.h"
//#include "colourix.h"
#include "cxfrec.h"
#include "cxfrech.h"
#include "cxftags.h"
#include "epsfiltr.h"
//#include "printctl.h"
#include "saveeps.h"

DECLARE_SOURCE("$Revision: 662 $");


CC_IMPLEMENT_DYNAMIC(PrintComponentClass,	DocComponentClass)
CC_IMPLEMENT_DYNAMIC(PrintComponent,		DocComponent)

// Declare smart memory handling in Debug builds
#define new CAM_DEBUG_NEW


//---------------------------------------------------------------------------------------

/********************************************************************************************

>	BOOL PrintComponentClass::Init()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Returns:	TRUE if all went well;
				FALSE if not.
	Purpose:	Register the print document component with the main application.
	Errors:		Out of memory.
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintComponentClass::Init()
{
	// Instantiate a component class to register with the application.
	PrintComponentClass *pClass = new PrintComponentClass;
	if (pClass == NULL)
		return FALSE;

	// Register it
	GetApplication()->RegisterDocComponent(pClass);

PORTNOTE("print","Removed PrintControl usage")
#ifndef EXCLUDE_FROM_XARALX
	if (Camelot.DeclareSection("Printing",1))
		Camelot.DeclarePref("Printing", "AppPrintMethod", 	(INT32*)(&PrintControl::AppPrintMethod)); */
#endif
	// All ok
	return TRUE;
}


/********************************************************************************************

>	BOOL PrintComponentClass::AddComponent(BaseDocument *pDocument)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pDocument - the document to add the print to.
	Returns:	TRUE if the print was added ok;
				FALSE if not.
	Purpose:	Add a print component to the specified document.
	Errors:		Out of memory
	SeeAlso:	PrintComponentClass

********************************************************************************************/

BOOL PrintComponentClass::AddComponent(BaseDocument *pDocument)
{
	PORTNOTETRACE("print","PrintComponentClass::AddComponent - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	// Check to see if this document already has a print component; if so, leave it alone.
	if (pDocument->GetDocComponent(CC_RUNTIME_CLASS(PrintComponent)) != NULL)
		return TRUE;

	// No print component- try to create a new one for this document.

	// Firstly, create a new PrintControl object for the print component
	PrintControl *pPrCtrl = new PrintControl();
	if(pPrCtrl == NULL)
		return FALSE;

	// Ok - create the print component using this print control object.
	PrintComponent *pComponent = new PrintComponent(pPrCtrl);
	if (pComponent == NULL)
	{
		// Out of memory...
		delete pPrCtrl;
		return FALSE;
	}

	// All ok - add the component to the document.
	pDocument->AddDocComponent(pComponent);
#endif
	return TRUE;
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

//----------------------------

/********************************************************************************************

>	PrintComponent::PrintComponent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Purpose:	Construct a print component. DO NOT use this constructor. It gives
				and ERROR3 (and sets up this in a semi-safe default state)
	SeeAlso:	PrintComponent

********************************************************************************************/

PrintComponent::PrintComponent()
{
	ERROR3("PrintComponent constructed with default constructor!?\n");
	pPrCtrl 	= NULL;
	pExportDC 	= NULL;
}



/********************************************************************************************

>	PrintComponent::PrintComponent(PrintControl *pThisPrCtrl)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pThisPrCtrl - the print control to use for this print componet.
	Purpose:	Construct a print component using the given print control object
	Errors:		ENSURE if pPrCtrl is NULL.
	SeeAlso:	PrintComponent

********************************************************************************************/

PrintComponent::PrintComponent(PrintControl *pThisPrCtrl)
{
	ERROR3IF(pThisPrCtrl == NULL, "NULL print control object in print component constructor!");

	// Install this Bitmap list.
	pPrCtrl 	= pThisPrCtrl;
	pExportDC 	= NULL;
}

/********************************************************************************************

>	PrintComponent::~PrintComponent()

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Purpose:	Clean up a print component's data structures - deletes the print control object
	Errors:		-
	SeeAlso:	PrintComponent

********************************************************************************************/

PrintComponent::~PrintComponent()
{
	// Delete our print control object
	if (pPrCtrl != NULL)
	{
	PORTNOTETRACE("print","PrintComponent::~PrintComponent - pPrCtrl NOT deleted");
#ifndef EXCLUDE_FROM_XARALX
		delete pPrCtrl;
#endif
		pPrCtrl = NULL;
	}
}


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#define PC_SECTION_VERSION 1

static TCHAR *PCTokenStr[] =
{
	_T("%%PrintControl"),
	_T("%%+WholeSpread"),
	_T("%%+Scale"),
	_T("%%+Orientation"),
	_T("%%+FitType"),
	_T("%%+TopMargin"),
	_T("%%+LeftMargin"),
	_T("%%+Width"),
	_T("%%+Height"),
	_T("%%+Rows"),
	_T("%%+Columns"),
	_T("%%+Gutter"),
	_T("%%+Layers"),
	_T("%%+PSLevel"),
	_T("%%+PrintMethod"),
	_T("%%+BitmapResMethod"),
	_T("%%+DotsPerInch"),
	_T("%%+Collated"),
	_T("%%+NumCopies"),
	_T("%%+PrintToFile"),
	_T("%%+ObjPrintRange"),
	_T("%%+DPSPrintRange"),
	_T("%%+AllTextAsShapes")
};

/********************************************************************************************

>	PCToken PrintComponent::GetToken(const char* pComment)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Input:		pComment = ptr to an EPS comment
	Returns:	The print component token value 
	Purpose:	Outputs a print component token with the given value
	SeeAlso:	PrintComponent

********************************************************************************************/

PCToken PrintComponent::GetToken(const char* pComment)
{
	UINT32 i;

	for (i=0;!_istspace(pComment[i]) && i < PC_BUFFERSIZE;i++)
		Buffer[i] = pComment[i];

	if (i >= PC_BUFFERSIZE)
		i=0;

	Buffer[i] = '\0';

	for (i=0;i < PCTOKEN_UNKNOWN;i++)
	{
		if (_tcscmp(PCTokenStr[i],Buffer) == 0)
			return (PCToken)i;
	}

	return PCTOKEN_UNKNOWN;
}

/********************************************************************************************

>	void PrintComponent::ExtractTokenValStr(const char* pComment)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Input:		pComment = ptr to an EPS comment
	Returns:	-
	Purpose:	The string representation of the token value is stored in Buffer.
				If no value is found, or the buffer overflows, Buffer will contain an empty string
	SeeAlso:	PrintComponent

********************************************************************************************/

void PrintComponent::ExtractTokenValStr(const char* pComment)
{
	Buffer[0] = '\0';

	if (pComment != NULL)
	{
		UINT32 i=0;

		// Skip past token str, up to first space char or string terminating char
		while (!_istspace(pComment[i]) && !_istcntrl(pComment[i]))
			i++;

		// Find next non-space char
		while (_istspace(pComment[i]))
			i++;

		// Extract all the remaining printable characters
		UINT32 j=0;
		while (_istprint(pComment[i]) && j < PC_BUFFERSIZE)
			Buffer[j++] = pComment[i++];

		if (j >= PC_BUFFERSIZE)
			j=0;

		Buffer[j] = '\0';
	}
}

/********************************************************************************************

>	INT32 PrintComponent::GetTokenValINT32(const char* pComment)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Input:		pComment = ptr to an EPS comment
	Returns:	-
	Purpose:	This gets the value after the token part of the comment and returns it as a INT32
	SeeAlso:	PrintComponent

********************************************************************************************/

INT32 PrintComponent::GetTokenValINT32(const char* pComment)
{
	ExtractTokenValStr(pComment);

	INT32 n = 0;
	_stscanf( Buffer, _T("%ld"), &n );

	return n;
}


/********************************************************************************************

>	FIXED16 PrintComponent::GetTokenValFIXED16(const char* pComment)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/4/95
	Input:		pComment = ptr to an EPS comment
	Returns:	-
	Purpose:	This gets the value after the token part of the comment and returns it as a FIXED16
	SeeAlso:	PrintComponent

********************************************************************************************/

FIXED16 PrintComponent::GetTokenValFIXED16(const char* pComment)
{
	ExtractTokenValStr(pComment);

	double n = 0.0;
	_stscanf( Buffer, _T("%le"), &n );

	if (n > double(0x7fff))
		n = double(0x7fff);

	if (n < double(-0x8000))
		n = double(-0x8000);

	return FIXED16(n);
}


/********************************************************************************************

>	BOOL PrintComponent::OutputValue(UINT32 Token,UINT32 Value)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Input:		Token = the print component token to output
				Value = the value to output after the token
	Returns:	TRUE if successful, FALSE if failed
	Purpose:	Outputs a print component token with the given value
	SeeAlso:	PrintComponent

********************************************************************************************/

// Signed INT32 version
BOOL PrintComponent::OutputValue(UINT32 Token,INT32 Value)
{
	ERROR2IF(pExportDC == NULL,FALSE,"NULL export DC in PrintComponent::OutputValue()");
	ERROR2IF(Token >= PCTOKEN_UNKNOWN,FALSE,"Token out of range");

	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintComponentClass::OutputValue - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (ok) ok = pExportDC->OutputToken(PCTokenStr[Token]);
	if (ok) ok = pExportDC->OutputValue(Value);
	if (ok) ok = pExportDC->OutputNewLine();
#endif

	return ok;
}

// FIXED16 version
BOOL PrintComponent::OutputValue(UINT32 Token,FIXED16 Value)
{
	ERROR2IF(pExportDC == NULL,FALSE,"NULL export DC in PrintComponent::OutputValue()");
	ERROR2IF(Token >= PCTOKEN_UNKNOWN,FALSE,"Token out of range");

	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintComponentClass::OutputValue - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (ok) ok = pExportDC->OutputToken(PCTokenStr[Token]);
	if (ok) ok = pExportDC->OutputReal(Value.MakeDouble());
	if (ok) ok = pExportDC->OutputNewLine();
#endif
	return ok;
}

/********************************************************************************************

>	BOOL PrintComponent::WriteEPSComments(EPSFilter *pFilter)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pFilter - the filter being used to import the EPS file.
	Returns:	TRUE if the print control object was written successfully;
				FALSE if not.
	Purpose:	Writes out the print control object in the form of EPS comments in the 'header'
				comments of the EPS file.
	Errors:		Out of disk space, and similar disk errors.
	SeeAlso:	DocComponent::WriteEPSComments;
				DocComponent

********************************************************************************************/

BOOL PrintComponent::WriteEPSComments(EPSFilter *pFilter)
{
	// Graeme (23-5-00) - Neither AI nor ArtWorks files need the print control entries, so
	// just return TRUE to avoid problems.
	if ( !pFilter->NeedsPrintComponents () )
	{
		return TRUE;
	}

	ERROR2IF ( pPrCtrl == NULL, FALSE,
			  "No print control to output in PrintComponent::WriteEPSComments()" );


	// Set up the export DC ptr
	pExportDC = pFilter->GetExportDC();

	ERROR2IF(pExportDC == NULL,FALSE,"Export DC is NULL in PrintComponent::WriteEPSComments()");

	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintComponentClass::WriteEPSComments - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	if (ok) ok = OutputValue(PCTOKEN_SECTIONNAME,		PC_SECTION_VERSION);

	if (ok) ok = OutputValue(PCTOKEN_WHOLESPREAD,		pPrCtrl->IsWholeSpread());
	if (ok) ok = OutputValue(PCTOKEN_SCALE,				pPrCtrl->GetScale());
	if (ok) ok = OutputValue(PCTOKEN_ORIENTATION,		pPrCtrl->GetPrintOrient());
	if (ok) ok = OutputValue(PCTOKEN_FITTYPE,			pPrCtrl->GetFitType());
	if (ok) ok = OutputValue(PCTOKEN_TOPMARGIN,			pPrCtrl->GetTopMargin());
	if (ok) ok = OutputValue(PCTOKEN_LEFTMARGIN,		pPrCtrl->GetLeftMargin());
	if (ok) ok = OutputValue(PCTOKEN_WIDTH,				pPrCtrl->GetWidth());
	if (ok) ok = OutputValue(PCTOKEN_HEIGHT,			pPrCtrl->GetHeight());
	if (ok) ok = OutputValue(PCTOKEN_ROWS,				pPrCtrl->GetRows());
	if (ok) ok = OutputValue(PCTOKEN_COLUMNS,			pPrCtrl->GetColumns());
	if (ok) ok = OutputValue(PCTOKEN_GUTTER,			pPrCtrl->GetGutter());
	if (ok) ok = OutputValue(PCTOKEN_LAYERS,			pPrCtrl->GetPrintLayers());
	if (ok) ok = OutputValue(PCTOKEN_PSLEVEL,			pPrCtrl->GetPSLevel());
	if (ok) ok = OutputValue(PCTOKEN_BITMAPRESMETHOD,	pPrCtrl->GetBitmapResMethod());

	// We only need to output the bitmap DPI if the user has selected 'manual' bitmap DPI
	// Auto DPI will calculated the DPI automatically ( surprise, surprise - Cilla 'ere!)
	// This also means that the printer's DPI is not needed before saving, hence avoiding any unnecessary
	// delays.
	if (ok)
	{
		if (pPrCtrl->GetBitmapResMethod() == BITMAPRES_MANUAL)
			ok = OutputValue(PCTOKEN_DOTSPERINCH, pPrCtrl->GetDotsPerInch());
		else
			ok = OutputValue(PCTOKEN_DOTSPERINCH, 150);
	}

	if (ok) ok = OutputValue(PCTOKEN_COLLATED,			pPrCtrl->IsCollated());
	if (ok) ok = OutputValue(PCTOKEN_NUMCOPIES,			pPrCtrl->GetNumCopies());
	if (ok) ok = OutputValue(PCTOKEN_PRINTTOFILE,		pPrCtrl->GetPrintToFile());
	if (ok) ok = OutputValue(PCTOKEN_OBJPRINTRANGE,		pPrCtrl->GetObjPrintRange());
	if (ok) ok = OutputValue(PCTOKEN_DPSPRINTRANGE,		pPrCtrl->GetDPSPrintRange());
	if (ok) ok = OutputValue(PCTOKEN_ALLTEXTASSHAPES,	pPrCtrl->GetTextOptions());
//	if (ok) ok = OutputValue(PCTOKEN_PRINTMETHOD,		pPrCtrl->GetPrintMethod());

	// reset the DC ptr to NULL (ready for next time)
	pExportDC = NULL;
#endif
	return ok;
}


/********************************************************************************************

>	ProcessEPSResult PrintComponent::ProcessEPSComment(EPSFilter *pFilter, 
														    const char *pComment)

	Author:		Mark_Neves (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/3/95
	Inputs:		pFilter - the EPS filter that is being used to import a file.
				pComment - pointer to the comment to process.
	Returns:	EPSCommentUnknown	 	- This EPS comment is not recognised by the document
									      component.
				EPSCommentSyntaxError	- This EPS comment was recognised by this document
									   	  component, but it contained an error.
				EPSCommentSystemError 	- This EPS comment was recognised by this document
									      component, but an error occured that was not caused
										  directly by the comment, e.g. out of memory.
				EPSCommentOK		 	- This EPS comment was recognised as a legal comment by 
									      this document component, and was processed
									      successfully.
	Purpose:	Process an EPS comment - if this is a print component comment, this component
				will claim it and try to decode it.
	Errors:		Badly formed EPS comment.
	SeeAlso:	DocComponent::ProcessEPSComment

********************************************************************************************/

ProcessEPSResult PrintComponent::ProcessEPSComment(EPSFilter *pFilter, 
														const char *pComment)
{
	ERROR2IF(pPrCtrl == NULL,EPSCommentUnknown,"No print control to output in PrintComponent::ProcessEPSComment()");

	ProcessEPSResult Result = EPSCommentUnknown;

	PORTNOTETRACE("print","PrintComponentClass::ProcessEPSComment - do nothing");
#ifndef EXCLUDE_FROM_XARALX
	PCToken Token = GetToken(pComment);

	if (Token != PCTOKEN_UNKNOWN)
	{
		// Let the print control object know we are importing the data rather than changing it via UI
		pPrCtrl->StartImport();

		Result = EPSCommentOK;

		INT32 	n 	  =	GetTokenValINT32(pComment);
		FIXED16 Scale = GetTokenValFIXED16(pComment);

		switch (Token)
		{
			case PCTOKEN_SECTIONNAME:
				// 'n' is the version number for this comment block
				if (n != PC_SECTION_VERSION)
					Result = EPSCommentSyntaxError;
				break;

			case PCTOKEN_WHOLESPREAD:		pPrCtrl->SetWholeSpread(n);							break;
			case PCTOKEN_SCALE:				pPrCtrl->SetScale(Scale);							break;
			case PCTOKEN_ORIENTATION:		pPrCtrl->SetPrintOrient(PrintOrient(n));			break;
			case PCTOKEN_FITTYPE:			pPrCtrl->SetFitType(PrintFitType(n));				break;
			case PCTOKEN_TOPMARGIN:			pPrCtrl->SetTopMargin(n);							break;
			case PCTOKEN_LEFTMARGIN:		pPrCtrl->SetLeftMargin(n);							break;
			case PCTOKEN_WIDTH:				pPrCtrl->SetWidth(n);								break;
			case PCTOKEN_HEIGHT:			pPrCtrl->SetHeight(n);								break;
			case PCTOKEN_ROWS:				pPrCtrl->SetRows(n);								break;
			case PCTOKEN_COLUMNS:			pPrCtrl->SetColumns(n);								break;
			case PCTOKEN_GUTTER:			pPrCtrl->SetGutter(n);								break;
			case PCTOKEN_LAYERS:			pPrCtrl->SetPrintLayers(PrintLayers(n));			break;
// Dirty bodge to ensure that PSLevel is always 2
// Job 10463: remove PS Level bits - default to Level 2
//			case PCTOKEN_PSLEVEL:			pPrCtrl->SetPSLevel(PSLevel(n));					break;
			case PCTOKEN_PSLEVEL:			pPrCtrl->SetPSLevel(PSLEVEL_2);						break;
			case PCTOKEN_BITMAPRESMETHOD:	pPrCtrl->SetBitmapResMethod(BitmapResMethod(n));	break;
			case PCTOKEN_DOTSPERINCH:		pPrCtrl->SetDotsPerInch(n);							break;
			case PCTOKEN_COLLATED:			pPrCtrl->SetCollated(n);							break;
			case PCTOKEN_NUMCOPIES:			pPrCtrl->SetNumCopies(n);							break;
			case PCTOKEN_PRINTTOFILE:		pPrCtrl->SetPrintToFile(n);							break;
			case PCTOKEN_OBJPRINTRANGE:		pPrCtrl->SetObjPrintRange(PrintRangeObj(n));		break;
			case PCTOKEN_DPSPRINTRANGE:		pPrCtrl->SetDPSPrintRange(PrintRangeDPS(n));		break;
			case PCTOKEN_ALLTEXTASSHAPES:	pPrCtrl->SetTextOptions((PrintTextOptions) n);		break;
//			case PCTOKEN_PRINTMETHOD:		pPrCtrl->SetPrintMethod(PrintMethodType(n));		break;
		}

		// Let the print control object know we have finished importing (for the moment at least)
		pPrCtrl->EndImport();
	}
#endif
	return Result;
}






/********************************************************************************************

>	BOOL PrintComponent::EndExport(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
				Success - True if everything went swimmingly, False if just a clean up is required.
	Purpose:	Inform the colour list document component that a Web or Native export has
				just finished.
	Returns:	TRUE if the component was able to end the importing;
				FALSE if not (e.g. out of memory)
	SeeAlso:	DocComponent

********************************************************************************************/

BOOL PrintComponent::EndExport(BaseCamelotFilter *pFilter, BOOL Success)
{
	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintComponent::EndExport - do nothing");
// WEBSTER - markn 14/2/97
// No print records needed in Webster
#ifndef WEBSTER
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	if (pFilter == NULL)
	{
		ERROR3("PrintComponent::EndExport filter is null!");
		return(ok);
	}

	// If we're not exporting native data, or if export has failed, we do nothing here
	if (pFilter->IsWebFilter() || !Success)
		return(ok);

	// Export our 3 record types (1 print info, 1 imagesetting info, 'n' colour plates)
	ExportPrintSettings(pFilter);
	ExportImagesetting(pFilter);

	TypesetInfo *TInfo = TypesetInfo::FindTypesetInfoForDoc(pFilter->GetDocument());
	if (TInfo != NULL && TInfo->GetNumPlates() > 0)
	{
		ColourPlate *pPlate = TInfo->GetFirstPlate();
		while (pPlate != NULL)
		{
			ExportColourPlate(pFilter, pPlate);
			pPlate = TInfo->GetNextPlate(pPlate);
		}	
	}

#endif		
#endif // WEBSTER
	return(ok);
}



/********************************************************************************************

>	BOOL PrintComponent::ExportPrintSettings(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Returns:	TRUE for success

	Purpose:	Export a TAG_PRINTERSETTINGS native file record

	SeeAlso:	PrintingRecordHandler

********************************************************************************************/

BOOL PrintComponent::ExportPrintSettings(BaseCamelotFilter *pFilter)
{
	BOOL ok = TRUE;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

	// --- Write the record
	CXaraFileRecord Rec(TAG_PRINTERSETTINGS, TAG_PRINTERSETTINGS_SIZE);
	if (ok) ok = Rec.Init();

	INT32 Value;		// Temp variable used by macros etc

	// --- Macros. These are undefined immediately at the end of this function
	// --- Helper macro for safely exporting an enum as a BYTE value: 1, 2. Unknown enums are output as a 1.
	#define OutputEnum2(FuncName,A,B)			\
		if (ok)									\
		{										\
			switch(pPrCtrl->Get##FuncName())	\
			{									\
				case A: Value=1; break;			\
				case B: Value=2; break;			\
				default: Value=1; ERROR3("Bad enum"); break; \
			}									\
			ok = Rec.WriteBYTE((BYTE)Value);	\
		}

	// --- Helper macro for safely exporting an enum as a BYTE value: 1, 2, 3. Unknown enums are output as a 1.
	#define OutputEnum3(FuncName,A,B,C)			\
		if (ok)									\
		{										\
			switch(pPrCtrl->Get##FuncName())	\
			{									\
				case A: Value=1; break;			\
				case B: Value=2; break;			\
				case C: Value=3; break;			\
				default: Value=1; ERROR3("Bad enum"); break; \
			}									\
			ok = Rec.WriteBYTE((BYTE)Value);	\
		}

	// --- Helper macro for safely exporting an enum as a BYTE value: 1, 2, 3. Unknown enums are output as a 1.
	#define OutputEnum4(FuncName,A,B,C,D)			\
		if (ok)									\
		{										\
			switch(pPrCtrl->Get##FuncName())	\
			{									\
				case A: Value=1; break;			\
				case B: Value=2; break;			\
				case C: Value=3; break;			\
				case D: Value=4; break;			\
				default: Value=1; ERROR3("Bad enum"); break; \
			}									\
			ok = Rec.WriteBYTE((BYTE)Value);	\
		}

	// --- Helper macro to output any 32-bit UNsigned value
	#define OutputValueU(FuncName)				\
		if (ok)  ok = Rec.WriteUINT32((UINT32)pPrCtrl->Get##FuncName());

	// --- Helper macro to output any 32-bit Signed value
	#define OutputValueS(FuncName)				\
		if (ok)  ok = Rec.WriteINT32((INT32)pPrCtrl->Get##FuncName());

	// --- Helper macro to output any 16-bit UNsigned value
	#define OutputValue16(FuncName)				\
		if (ok)  ok = Rec.WriteUINT16((UINT16)pPrCtrl->Get##FuncName());


																									// Size	Total
	OutputValueU(NumCopies);																		// 4	4
	if (ok) ok = Rec.WriteFIXED16(pPrCtrl->GetScale());												// 4	8
	OutputValueS(TopMargin);																		// 4	12
	OutputValueS(LeftMargin);																		// 4	16
	OutputValueS(Width);																			// 4	20
	OutputValueS(Height);																			// 4	24
	OutputValue16(Rows);																			// 2	26
	OutputValue16(Columns);																			// 2	28
	OutputValueS(Gutter);																			// 4	32
	OutputEnum3(PrintMethod,		PRINTMETHOD_NORMAL, PRINTMETHOD_BITMAP, PRINTMETHOD_AABITMAP);	// 1	33
	OutputEnum2(ObjPrintRange,		PRINTRANGEOBJ_ALL, PRINTRANGEOBJ_SELECTED);						// 1	34
	OutputEnum3(DPSPrintRange,		PRINTRANGEDPS_BOTH, PRINTRANGEDPS_LEFTPAGES, PRINTRANGEDPS_RIGHTPAGES);
																									// 1	35
	OutputEnum2(PrintOrient,		PRINTORIENTATION_UPRIGHT, PRINTORIENTATION_SIDEWAYS);			// 1	36
	OutputEnum4(FitType,			PRINTFIT_BEST, PRINTFIT_CUSTOM, PRINTFIT_MULTIPLE, PRINTFIT_BESTPAPER);	// 1	37
	OutputEnum2(PrintLayers,		PRINTLAYERS_ALLFOREGROUND, PRINTLAYERS_VISIBLEFOREGROUND);		// 1	38
	OutputEnum3(PSLevel,			PSLEVEL_AUTO, PSLEVEL_1, PSLEVEL_2);							// 1	39
	OutputEnum2(BitmapResMethod,	BITMAPRES_AUTO, BITMAPRES_MANUAL);								// 1	40
																									//
	if (ok)																							//
	{																								//
		// If bitmap resolution is automatic, then just output a default of 150dpi for DotsPerInch	//
		if (pPrCtrl->GetBitmapResMethod() == BITMAPRES_MANUAL)										//
			Value = pPrCtrl->GetDotsPerInch();														//
		else																						//
			Value = 150;																			//
																									//
		ok = Rec.WriteUINT32((UINT32) Value);															// 4	44
	}																								//
																									//
	if (ok)																							//
	{																								//
		Value = 0x00;																				//
		if (pPrCtrl->IsCollated())			Value |= 0x01;											//
		if (pPrCtrl->IsWholeSpread())		Value |= 0x02;											//
		if (pPrCtrl->GetPrintToFile())		Value |= 0x04;											//
		if (pPrCtrl->GetTextOptions() == PRINTTEXTOPTIONS_ALLTEXTASSHAPES)							//
			Value |= 0x08;																			//
																									//
		ok = Rec.WriteBYTE((BYTE)Value);															// 1	45 TOTAL
	}

	// --- Finally, output the record
	if (ok)
		pFilter->Write(&Rec);


	// Undefine the macros we were using
	#undef OutputEnum2
	#undef OutputEnum3
	#undef OutputValueU
	#undef OutputValueS
	#undef OutputValue16
#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX

	return(ok);
}



/********************************************************************************************

>	BOOL PrintComponent::ExportImagesetting(BaseCamelotFilter *pFilter)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
	Returns:	TRUE for success

	Purpose:	Export a TAG_IMAGESETTING native file record

	SeeAlso:	PrintingRecordHandler

********************************************************************************************/

BOOL PrintComponent::ExportImagesetting(BaseCamelotFilter *pFilter)
{
	BOOL ok = TRUE;

#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

	TypesetInfo *TInfo = GetPrintControl()->GetTypesetInfo();
	if (TInfo == NULL)
		return(TRUE);		// If we can't find imagesetting info, don't sweat it

	CXaraFileRecord Rec(TAG_IMAGESETTING, TAG_IMAGESETTING_SIZE);
	if (ok)  ok = Rec.Init();

	if (ok)  ok = Rec.WriteINT32(TInfo->GetPrintResolution());
	if (ok)  ok = Rec.WriteDOUBLE(TInfo->GetDefaultScreenFrequency());

	if (ok)
	{
		UINT16 Func = 0;
		switch(TInfo->GetScreenFunction())
		{
			case SCRTYPE_SPOT1:			Func = 1;	break;
			case SCRTYPE_SPOT2:			Func = 2;	break;
			case SCRTYPE_TRIPLESPOT1:	Func = 3;	break;
			case SCRTYPE_TRIPLESPOT2:	Func = 4;	break;
			case SCRTYPE_ELLIPTICAL:	Func = 5;	break;
			case SCRTYPE_LINE:			Func = 6;	break;
			case SCRTYPE_CROSSHATCH:	Func = 7;	break;
			case SCRTYPE_MEZZOTINT:		Func = 8;	break;
			case SCRTYPE_SQUARE:		Func = 9;	break;
			case SCRTYPE_DITHER:		Func = 10;	break;
		}
		ok = Rec.WriteUINT16(Func);
	}

	if (ok)
	{
	  	BYTE Flags = 0x00;
		if (TInfo->AreSeparating())			Flags |= 0x01;
		if (TInfo->AreScreening())			Flags |= 0x02;
		if (TInfo->PrintEmulsionDown())		Flags |= 0x04;
		if (TInfo->PrintPhotoNegative())	Flags |= 0x08;
		if (TInfo->AlwaysOverprintBlack())	Flags |= 0x10;

		ok = Rec.WriteBYTE(Flags);
	}

	if (ok)
		pFilter->Write(&Rec);

#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX
	return(ok);
}



/********************************************************************************************

>	BOOL PrintComponent::ExportColourPlate(BaseCamelotFilter *pFilter, ColourPlate *pPlate)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96

	Inputs:		pFilter - the BaseCamelotFilter filter that is being used to export a file.
				pPlate  - the plate to be exported

	Returns:	TRUE for success

	Purpose:	Export a TAG_COLOURPLATE native file record

	SeeAlso:	PrintingRecordHandler

********************************************************************************************/

BOOL PrintComponent::ExportColourPlate(BaseCamelotFilter *pFilter, ColourPlate *pPlate)
{
	ERROR3IF(pPlate == NULL, "Illegal NULL params");

	BOOL ok = TRUE;
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)

	// --- Save any referenced spot colour
	IndexedColour *pCol = pPlate->GetSpotColour();
	INT32 SpotRecordNumber = 0;
	if (pCol != NULL)
	{
		DocColour Fred;
		Fred.MakeRefToIndexedColour(pCol);
		SpotRecordNumber = pFilter->WriteRecord(&Fred);
		ok = (SpotRecordNumber > 0);
	}

	// -- Now save the ColourPlate
	CXaraFileRecord Rec(TAG_COLOURPLATE, TAG_COLOURPLATE_SIZE);
	if (ok)  ok = Rec.Init();

	BYTE Type = 0;
	switch(pPlate->GetType())
	{
		case COLOURPLATE_CYAN:		Type = 1; break;
		case COLOURPLATE_MAGENTA:	Type = 2; break;
		case COLOURPLATE_YELLOW:	Type = 3; break;
		case COLOURPLATE_KEY:		Type = 4; break;
		case COLOURPLATE_SPOT:		Type = 5; break;
	}
																	//	Size	Total
	if (ok)  ok = Rec.WriteBYTE(Type);								//	1		1
	if (ok)  ok = Rec.WriteReference(SpotRecordNumber);				//	4		5

	double temp = pPlate->GetScreenAngle();
	if (ok)  ok = Rec.WriteDOUBLE(temp);							//	8		13
	temp = pPlate->GetScreenFrequency();
	if (ok)  ok = Rec.WriteDOUBLE(temp);							//	8		21
																	//
	BYTE Flags = 0x00;												//	1		22
	if (!pPlate->IsDisabled())		Flags |= 0x01;
	if (pPlate->Overprints())		Flags |= 0x02;
	if (ok)  ok = Rec.WriteBYTE(Flags);

	if (ok)
		pFilter->Write(&Rec);

#endif // EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX
	return(ok);
}



/********************************************************************************************

>	void PrintComponent::ImportPrintSettings(CXaraFileRecord* Rec)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		pCXaraFileRecord - the record to be imported

	Purpose:	Load a TAG_PRINTERSETTINGS native file record

	SeeAlso:	PrintingRecordHandler

********************************************************************************************/

void PrintComponent::ImportPrintSettings(CXaraFileRecord* Rec)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintComponent::ImportPrintSettings - do nothing");
	// Let the print control object know we are importing the data rather than changing it via UI
	// This makes sure that setting some values does not auto-update other values!
	pPrCtrl->StartImport();

	// --- Macros. These are undefined immediately at the end of this function
	// --- Helper macro for safely importing an enum stored as a BYTE value: 1, 2.
	//	   Unknown enums are ignored - i.e. they retain the default program setting
	#define InputEnum2(FuncName,A,B)							\
		if (ok)													\
		{														\
			BYTE Value;											\
			ok = Rec->ReadBYTE(&Value);							\
			if (ok)												\
			{													\
				switch(Value)									\
				{												\
					case 1: pPrCtrl->Set##FuncName(A); break;	\
					case 2: pPrCtrl->Set##FuncName(B); break;	\
					default: ERROR3("Bad enum"); break;			\
				}												\
			}													\
		}

	// --- Helper macro for safely exporting an enum as a BYTE value: 1, 2, 3. Unknown enums are Input as a 1.
	#define InputEnum3(FuncName,A,B,C)							\
		if (ok)													\
		{														\
			BYTE Value;											\
			ok = Rec->ReadBYTE(&Value);							\
			if (ok)												\
			{													\
				switch(Value)									\
				{												\
					case 1: pPrCtrl->Set##FuncName(A); break;	\
					case 2: pPrCtrl->Set##FuncName(B); break;	\
					case 3: pPrCtrl->Set##FuncName(C); break;	\
					default: ERROR3("Bad enum"); break;			\
				}												\
			}													\
		}

	// --- Helper macro for safely exporting an enum as a BYTE value: 1, 2, 3. Unknown enums are Input as a 1.
	#define InputEnum4(FuncName,A,B,C,D)						\
		if (ok)													\
		{														\
			BYTE Value;											\
			ok = Rec->ReadBYTE(&Value);							\
			if (ok)												\
			{													\
				switch(Value)									\
				{												\
					case 1: pPrCtrl->Set##FuncName(A); break;	\
					case 2: pPrCtrl->Set##FuncName(B); break;	\
					case 3: pPrCtrl->Set##FuncName(C); break;	\
					case 4: pPrCtrl->Set##FuncName(D); break;	\
					default: ERROR3("Bad enum"); break;			\
				}												\
			}													\
		}

	// --- Helper macro to Input any 32-bit UNsigned value
	#define InputValueU(FuncName)								\
		if (ok)													\
		{														\
			UINT32 Value;										\
			ok = Rec->ReadUINT32(&Value);						\
			if (ok)	 pPrCtrl->Set##FuncName(Value);				\
		}														\

	// --- Helper macro to Input any 32-bit Signed value
	#define InputValueS(FuncName)								\
		if (ok)													\
		{														\
			INT32 Value;											\
			ok = Rec->ReadINT32(&Value);							\
			if (ok)	 pPrCtrl->Set##FuncName(Value);				\
		}														\

	// --- Helper macro to Input any 16-bit UNsigned value (NOTE: Casts the result to a INT32)
	#define InputValue16(FuncName)								\
		if (ok)													\
		{														\
			UINT16 Value;										\
			ok = Rec->ReadUINT16(&Value);						\
			if (ok)	 pPrCtrl->Set##FuncName((INT32)Value);		\
		}														\
	

	// --- Read the record
	InputValueU(NumCopies);
	if (ok)
	{
		FIXED16 Value;
		ok = Rec->ReadFIXED16(&Value);
		if (ok)  pPrCtrl->SetScale(Value);
	}
	InputValueS(TopMargin);
	InputValueS(LeftMargin);
	InputValueS(Width);	
	InputValueS(Height);
	InputValue16(Rows);
	InputValue16(Columns);
	InputValueS(Gutter);
	InputEnum3(PrintMethod,		PRINTMETHOD_NORMAL, PRINTMETHOD_BITMAP, PRINTMETHOD_AABITMAP);
	InputEnum2(ObjPrintRange,	PRINTRANGEOBJ_ALL, PRINTRANGEOBJ_SELECTED);
	InputEnum3(DPSPrintRange,	PRINTRANGEDPS_BOTH, PRINTRANGEDPS_LEFTPAGES, PRINTRANGEDPS_RIGHTPAGES);
	InputEnum2(PrintOrient,		PRINTORIENTATION_UPRIGHT, PRINTORIENTATION_SIDEWAYS);
	InputEnum4(FitType,			PRINTFIT_BEST, PRINTFIT_CUSTOM, PRINTFIT_MULTIPLE, PRINTFIT_BESTPAPER);
	InputEnum2(PrintLayers,		PRINTLAYERS_ALLFOREGROUND, PRINTLAYERS_VISIBLEFOREGROUND);
// Dirty bodge to ensure that all loaded XAR files use PSLEVEL_2
// Job 10463: remove PS Level bits - default to Level 2
//	InputEnum3(PSLevel,			PSLEVEL_AUTO, PSLEVEL_1, PSLEVEL_2);
	InputEnum3(PSLevel,			PSLEVEL_2, PSLEVEL_2, PSLEVEL_2);
	InputEnum2(BitmapResMethod,	BITMAPRES_AUTO, BITMAPRES_MANUAL);

	if (ok)
	{
		UINT32 Value;
		ok = Rec->ReadUINT32(&Value);
		if (ok)  pPrCtrl->SetDotsPerInch(Value);
	}

	if (ok)
	{
		BYTE Value;
		ok = Rec->ReadBYTE(&Value);

		if (ok)
		{
			pPrCtrl->SetCollated(	(Value & 0x01) != 0);	// Bit 1 indicates collated
			pPrCtrl->SetWholeSpread((Value & 0x02) != 0);	// Bit 2 indicates whole spread
			pPrCtrl->SetPrintToFile((Value & 0x04) != 0);	// Bit 3 indicates print-to-file

			pPrCtrl->SetTextOptions(((Value & 0x08) != 0) ? PRINTTEXTOPTIONS_ALLTEXTASSHAPES : PRINTTEXTOPTIONS_NORMAL);
															// Bit 4 indicates print-text-as-shapes
		}
	}

	// Let the print control object know we have finished importing (for the moment at least)
	pPrCtrl->EndImport();

	// Undefine the macros we were using
	#undef InputEnum2
	#undef InputEnum3
	#undef InputValueU
	#undef InputValueS
	#undef InputValue16

#endif
}



/********************************************************************************************

>	void PrintComponent::ImportImagesetting(CXaraFileRecord* Rec)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		Rec - the record to be imported

	Purpose:	Load a TAG_IMAGESETTING native file record

	SeeAlso:	PrintingRecordHandler

********************************************************************************************/

void PrintComponent::ImportImagesetting(CXaraFileRecord* Rec)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	PORTNOTETRACE("print","PrintComponent::ImportImagesetting - do nothing");
	TypesetInfo *TInfo = GetPrintControl()->GetTypesetInfo();
	if (TInfo == NULL)
		return;

	// Vape any colour plates already resident in the typeset info
	TInfo->DestroyPlateList();

	BOOL ok = TRUE;

	if (ok)
	{
		INT32 Res;
		ok = Rec->ReadINT32(&Res);
		if (ok)  TInfo->SetPrintResolution(Res);
	}

	if (ok)
	{
		double Freq;
		ok = Rec->ReadDOUBLE(&Freq);
		if (ok)  TInfo->SetDefaultScreenFrequency(Freq);
	}

	if (ok)
	{
		UINT16 Func;
		ok = Rec->ReadUINT16(&Func);

		if (ok)
		{
			ScreenType Screen = SCRTYPE_NONE;
			switch(Func)
			{
				case 1: Screen = SCRTYPE_SPOT1;			break;
				case 2: Screen = SCRTYPE_SPOT2;			break;
				case 3: Screen = SCRTYPE_TRIPLESPOT1;	break;
				case 4: Screen = SCRTYPE_TRIPLESPOT2;	break;
				case 5: Screen = SCRTYPE_ELLIPTICAL;	break;
				case 6: Screen = SCRTYPE_LINE;			break;
				case 7: Screen = SCRTYPE_CROSSHATCH;	break;
				case 8: Screen = SCRTYPE_MEZZOTINT;		break;
				case 9: Screen = SCRTYPE_SQUARE;		break;
				case 0: Screen = SCRTYPE_DITHER;		break;
			}
			TInfo->SetScreenFunction(Screen, TRUE);
		}
	}

	if (ok)
	{
	  	BYTE Flags;
		ok = Rec->ReadBYTE(&Flags);

		TInfo->SetSeparations(	(Flags & 0x01) != 0);
		TInfo->SetOutputPrintersMarks(TInfo->AreSeparating());	// Slave Printers Marks off the Seps flag

		TInfo->SetScreening(	(Flags & 0x02) != 0);
		TInfo->SetEmulsionDown(	(Flags & 0x04) != 0);
		TInfo->SetPhotoNegative((Flags & 0x08) != 0);
		TInfo->SetOverprintBlack((Flags & 0x10) != 0);
	} 
#endif
}



/********************************************************************************************

>	void PrintComponent::ImportColourPlate(CXaraFileRecord* Rec, CamelotRecordHandler *pHandler)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/8/96
	Inputs:		Rec - the record to be imported
				pHandler - The 'andler wot's callin' me, so's I can find me spot culler.

	Purpose:	Load a TAG_COLOURPLATE native file record

	Notes:		To correctly set up all plates, the TypesetInfo should have been
				correctly set up (by loading a TAG_IMAGESETTING record). This is
				because some ColourPlate settings are regenerated from entries
				saved in the TAG_IMAGESETTING record.

	SeeAlso:	PrintingRecordHandler

********************************************************************************************/

void PrintComponent::ImportColourPlate(CXaraFileRecord* Rec, CamelotRecordHandler *pHandler)
{
#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)
	BOOL ok = TRUE;

	PORTNOTETRACE("print","PrintComponent::ImportColourPlate - do nothing");
	TypesetInfo *TInfo = GetPrintControl()->GetTypesetInfo();
	if (TInfo == NULL)
		return;
	
	ColourPlate *NewPlate = TInfo->CreateColourPlate();
	if (NewPlate == NULL)
		return;

	BYTE Type;
	if (ok)  ok = Rec->ReadBYTE(&Type);

	ColourPlateType PlateType = COLOURPLATE_NONE;
	if (ok)
	{
		switch(Type)
		{
			case 1: PlateType = COLOURPLATE_CYAN;		break;
			case 2: PlateType = COLOURPLATE_MAGENTA;	break;
			case 3: PlateType = COLOURPLATE_YELLOW;		break;
			case 4: PlateType = COLOURPLATE_KEY;		break;
			case 5: PlateType = COLOURPLATE_SPOT;		break;
		}
	}

	IndexedColour *pCol = NULL;
	if (ok)
	{
		INT32 ColRecordNum;
		ok = Rec->ReadINT32(&ColRecordNum);
		if (ok && PlateType == COLOURPLATE_SPOT)
		{
			DocColour SpotCol;
			ok = pHandler->GetDocColour(ColRecordNum, &SpotCol);
			if (ok)
				pCol = SpotCol.FindParentIndexedColour();
		}
	}

	// If this is a spot plate, it must reference a valid spot colour
	if (PlateType == COLOURPLATE_SPOT &&
		(pCol == NULL || pCol->GetType() != COLOURTYPE_SPOT || pCol->IsDeleted()) )
	{
		// Poohs. No point in keeping this plate around
		delete NewPlate;
		return;
	}

	NewPlate->SetType(NULL, PlateType, pCol);

	if (ok)
	{
		double Angle = 0.0;
		double Frequency = 60.0;

		ok = Rec->ReadDOUBLE(&Angle);
		if (ok)  ok = Rec->ReadDOUBLE(&Frequency);
		if (ok)  NewPlate->SetScreenInfo(Angle, Frequency);
	}

	BYTE Flags;
	if (ok)  ok = Rec->ReadBYTE(&Flags);

	if (ok)
	{
		NewPlate->SetDisabled( (Flags & 0x01) == 0);		// NOTE Flag is ENabled state!
		NewPlate->SetOverprint((Flags & 0x02) != 0);
	}


	if (ok)
	{
		// Now, it'll be quite handy if we add the new plate to the plate list!
		TInfo->AddPlate(NewPlate);
	}
#endif
}
