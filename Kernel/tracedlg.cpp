// $Id: tracedlg.cpp 662 2006-03-14 21:31:49Z alex $
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
// A sdialog for testing trace things

/*
*/

#include "camtypes.h"
#include "tracedlg.h"

#include "tracectl.h"

//#include "mario.h"
#include "reshlpid.h"
//#include "tracerc.h"
//#include "rik.h"
//#include "andy.h"
#include "menuops.h"

#include "oilprog.h"

#include "ccdc.h"		// specific #includes needed for kernel-rendered dialogues
#include "dlgcol.h"
#include "fillval.h"
#include "grnddib.h"

#include "app.h"
#include "range.h"
#include "nodebmp.h"
#include "bitmpinf.h"
#include "chapter.h"
#include "spread.h"
#include "page.h"
#include "noderend.h"
#include "fillattr.h"

#include "docview.h"
#include "trans2d.h"
#include "monotime.h"
#include "progress.h"
//#include "resource.h"
//#include "barsdlgs.h"			// for the tracer dialog bar control
#include "bubbleid.h"

//#include "will2.h"
#include "bmpcomp.h"
#include "wrkrect.h"	

#include "mrhbits.h"

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision: 662 $");

// An implement to match the Declare in the .h file.
// If you have many classes, it is recommended to place them all together, here at the start of the file
CC_IMPLEMENT_DYNAMIC(TraceMsg, Msg)
CC_IMPLEMENT_DYNCREATE(TraceDlg, DialogOp)
CC_IMPLEMENT_DYNCREATE(TraceOp, SelOperation)   
CC_IMPLEMENT_MEMDUMP(TraceOpParam, OpParam)

// This will get Camelot to display the filename and linenumber of any memory allocations
// that are not released at program exit
#define new CAM_DEBUG_NEW


// Set the static vars of the render dialog
const CDlgMode TraceDlg::Mode = MODELESS;
const UINT32 TraceDlg::IDD = _R(IDD_TRACEDLG);

// Order is
//	Passes
//	RemoveNoise
//	Min area
//	ICT
//	FCT
//	Accuracy
//	Smooth

#define TRACER_FACTORY_SETTINGS {\
			{1, 50, 25, 50, 50, 80, 15},/*mono*/ \
			{1, 50, 25, 50, 50, 50, 50},/*GS*/ \
			{1, 50, 25, 65, 65, 50, 50},/*256col*/ \
			{1, 50, 25, 25, 25, 50, 50} /*true col*/ \
			}

INT32 TraceDlg::DefaultMode=(INT32)TRACEMETHOD_TRUECOL;
INT32 TraceDlg::DefaultSetting[NUM_TRACEMETHOD][NUM_TRACESETTING] = TRACER_FACTORY_SETTINGS;
INT32 TraceDlg::FactorySetting[NUM_TRACEMETHOD][NUM_TRACESETTING] = TRACER_FACTORY_SETTINGS;

/*******************************************************************************************

>	BOOL TraceDlg::OriginalChanged()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Invalidates area for original bitmap, marks no destination as yet
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL TraceDlg::OriginalChanged()
{
	if (pPrevOriginal==pOriginal) return TRUE;
	pPrevOriginal=pOriginal;
	if (pTraceControl) pTraceControl->InitBitmap(pOriginal);
	InvalidateGadget(_R(IDC_TRACER_ORIGREDRAW));
	return FixOriginalComboBox() && SetStatus() && DestinationChanged();
}

/*******************************************************************************************

>	BOOL TraceDlg::DestinationChanged()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Invalidates area for destination bitmap, marks no destination as yet
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL TraceDlg::DestinationChanged()
{
	InvalidateGadget(_R(IDC_TRACER_DESTREDRAW));
	return TRUE;
}


/*******************************************************************************************

>	TraceDlg::TraceDlg() : DialogOp(RenderDlg::IDD, RenderDlg::Mode) 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Constructs the trace dialog Dialog

*******************************************************************************************/

TraceDlg::TraceDlg() : DialogOp(TraceDlg::IDD, TraceDlg::Mode) 
{
	// Set the 2 colours that we can toggle between
	pOriginal = NULL;
	pPrevOriginal=NULL;
	pTraceControl = new TraceControl; // NULL means tracer won't work...
}       



/*******************************************************************************************

>	TraceDlg::~TraceDlg()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Destructor, does nothing

*******************************************************************************************/

TraceDlg::~TraceDlg()
{
	pOriginal = NULL;
	
	if(!m_p24BitCopyNames.IsEmpty())
	{
		Remove24BitTempCopy();
	}

	if (pTraceControl)
	{
		pTraceControl->InitBitmap(pOriginal);
		delete pTraceControl;
		pTraceControl = NULL;
	}
} 

void TraceDlg::Remove24BitTempCopy()
{
	// Get the selected Document
	Document * pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc)
	{
		Bitmaps = pDoc->GetBitmapList();
	}

	// Get the first bitmap out of the bitmap list
	KernelBitmap * pBmp = (KernelBitmap *)(Bitmaps->GetHead());
	KernelBitmap * pTemp = NULL;

	POSITION Pos = NULL;
	BOOL DeleteThisBMP = FALSE;

	// If we`ve got a valid BMP then do some testing!
	while (pBmp != NULL)
	{
		// Get the first Copy Name from our list
		Pos = m_p24BitCopyNames.GetHeadPosition();

		// While the Position and the BMP are both valid do the check!
		while(Pos != NULL && pBmp != NULL)
		{
			// Check to see if the BMP`s name matches the one held in the copy list
			if (pBmp->GetName().CompareTo(m_p24BitCopyNames.GetAt(Pos)) == 0)
			{
				// Mark this BMP For deletion!
				DeleteThisBMP = TRUE;

				// Now break out of this while loop!
				break;
			}

			// Get the next name from the copy name list
			m_p24BitCopyNames.GetNext(Pos);
		}

		// If this isn`t the one then get the next in the list!
		pTemp = (KernelBitmap *) Bitmaps->GetNext(pBmp);

		// If the flag has been set then delete it!
		if(DeleteThisBMP)
		{
			// Remove it, Detach it, delete it and then just for good measure NULL it!!!
			Bitmaps->RemoveItem(pBmp);
			pBmp->Detach();
			delete pBmp;
			pBmp = NULL;

			// Reset the Delete flag
			DeleteThisBMP = FALSE;
		}

		// Set the Current BMP with the Temp
		pBmp = pTemp;
	}

	// Finished with everything so make sure the list is totally empty!
	while(!m_p24BitCopyNames.IsEmpty())
	{
		m_p24BitCopyNames.RemoveHead();
	}
}

/*******************************************************************************************

>	BOOL TraceDlg::CheckOriginalBitmap()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	pOriginal set
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Ensures pOriginal points to a valid bitmap in this document
	Errors:		None at present
	SeeAlso:	-


*******************************************************************************************/

BOOL TraceDlg::CheckOriginalBitmap()
{
	Document * pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
	{
		pOriginal = NULL;
		return OriginalChanged();
	}
	
	KernelBitmap * pBmp = NULL;

	pBmp = (KernelBitmap *)(Bitmaps->GetHead());

	while (pBmp != NULL)
	{
		if (pBmp == pOriginal) return OriginalChanged(); // OK we've found it.
		pBmp = (KernelBitmap *) Bitmaps->GetNext(pBmp);
	}

	pOriginal = (KernelBitmap *)(Bitmaps->GetHead()); // possibly NULL but certainly on the list
	return OriginalChanged();
}

/*******************************************************************************************

>	BOOL TraceDlg::FixOriginalComboBox()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Makes the combo box reflect the bitmaps within the document
	Errors:		None at present
	SeeAlso:	-

This routine is heavilly copied from one within the fill tool

*******************************************************************************************/

BOOL TraceDlg::FixOriginalComboBox()
{
	// Find the doccomponent with the list of bitmaps in it
	Document * pDoc = Document::GetSelected();

	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return TRUE;

	String_256 Str;

	DeleteAllValues(_R(IDC_TRACER_BITMAP));

	BOOL DoneMain = FALSE;
	INT32 SelIndex = 0;

	BOOL IsMono = FALSE;
	if ((pOriginal) && (pOriginal->GetBPP()==1)) IsMono=TRUE;

	EnableGadget(_R(IDC_TRACER_BLURNUM), !IsMono);
	EnableGadget(_R(IDC_TRACER_BLURSLIDER), !IsMono);
	EnableGadget(_R(IDC_TRACER_BLURTXT), !IsMono);

	if (Bitmaps->GetCount() > 0)
	{
		INT32 Index = 0;

		ListItem* pBmp = Bitmaps->GetHead();

		while (pBmp != NULL)
		{
			Str = ((KernelBitmap*)pBmp)->ActualBitmap->GetName();
			if (((KernelBitmap*)(pBmp)) == pOriginal)
			{
				SetStringGadgetValue(_R(IDC_TRACER_BITMAP),&Str, FALSE, -1);
				DoneMain = TRUE;
				SelIndex=Index;
			}
			SetStringGadgetValue(_R(IDC_TRACER_BITMAP),&Str,FALSE, Index++);
			pBmp = Bitmaps->GetNext(pBmp);
		}
	}
	else
	{
		Str.Load(_R(IDS_K_BFXDLG_DEFAULT));
		SetStringGadgetValue(_R(IDC_TRACER_BITMAP),&Str,TRUE, 0);
	}

	Str.Load(_R(IDS_K_BFXDLG_DEFAULT));
	SetComboListLength(_R(IDC_TRACER_BITMAP));
	if (!DoneMain)
	{
		SetStringGadgetValue(_R(IDC_TRACER_BITMAP),&Str, FALSE, -1);
		SetSelectedValueIndex(_R(IDC_TRACER_BITMAP), 0);
	}
	else
	{
		SetSelectedValueIndex(_R(IDC_TRACER_BITMAP), SelIndex);
	}
	return TRUE;
}


/*******************************************************************************************

>	BOOL TraceDlg::ReadOriginalComboBox()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Chooses a different bitmap from the combo box
	Errors:		None at present
	SeeAlso:	-

This routine is heavilly copied from one within the fill tool

*******************************************************************************************/

BOOL TraceDlg::ReadOriginalComboBox()
{
	Document * pDoc = Document::GetSelected();
	
	// Get the bitmap list
	BitmapList* Bitmaps = NULL;
	if (pDoc) Bitmaps = pDoc->GetBitmapList();

	if (Bitmaps == NULL)
		return TRUE;

	ListItem* pBmp = Bitmaps->GetHead();

	WORD Index;
	GetValueIndex(_R(IDC_TRACER_BITMAP),&Index); 

	for (INT32 i = 0; i < Index; i++)
	{
		if (pBmp == NULL)
			break;

		pBmp = Bitmaps->GetNext(pBmp);
	}

	pOriginal = (KernelBitmap*)pBmp; // Might be NULL (conceivably)
	return CheckOriginalBitmap();
}



/*******************************************************************************************

>	BOOL TraceDlg::SetStatus(BOOL Tracing=FALSE)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		Tracing = TRUE if tracing
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Makes status line reflect current status
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/

BOOL TraceDlg::SetStatus(BOOL Tracing)
{
	UINT32 theID = _R(IDS_TRACER_PROGRESSSTRING_BLANK);
	INT32 NumPaths=0;
	INT32 NumPoints=0;
	INT32 Percent=0;
	BOOL Done=FALSE;
	String_256 Temp;
	if (pTraceControl && pTraceControl->GetProgress(&Percent, &NumPaths, NULL, &NumPoints, &Done) && Percent)
	{
		if (Done)
		{
			theID = _R(IDS_TRACER_PROGRESSSTRING_DONE);
			Tracing=FALSE;
		}
		else
		{
			if (Tracing)
				theID = _R(IDS_TRACER_PROGRESSSTRING_TRACING);
			else
				theID = _R(IDS_TRACER_PROGRESSSTRING_ABORTED);
		}
	}
	Temp.MakeMsg(theID, NumPaths, NumPoints);
	SetStringGadgetValue(_R(IDC_TRACER_STATUSTEXT), &Temp);
	EnableGadget(IDOK, Done && (!Tracing));
	EnableGadget(IDCANCEL, !Tracing);
	EnableGadget(_R(IDC_TRACER_TRACEBTN), !Tracing);
	EnableGadget(_R(IDC_TRACER_HELP), !Tracing);
	return TRUE;
}

/*******************************************************************************************

>	BOOL TraceDlg::LoadSettings(INT32 * pSettings)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		*pSettings points to area to load from
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Sets up the sliders to a settings bank
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/
BOOL TraceDlg::LoadSettings(INT32 * pSettings)
{
	SetLongGadgetValue(_R(IDC_TRACER_PASSESNUM), 		pSettings[0]);
	SetLongGadgetValue(_R(IDC_TRACER_BLURSLIDER), 		100-pSettings[1]); 
	SetLongGadgetValue(_R(IDC_TRACER_MINAREASLIDER),	50- pSettings[2]); 
	SetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER), 		100-pSettings[3]); 
	SetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER), 		100-pSettings[4]); 
	SetLongGadgetValue(_R(IDC_TRACER_ACCURACYSLIDER), 	100-pSettings[5]); 
	SetLongGadgetValue(_R(IDC_TRACER_SMOOTHSLIDER), 	100-pSettings[6]); 
	return ReadParams();
}

/*******************************************************************************************

>	BOOL TraceDlg::SaveSettings(INT32 * pSettings)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		*pSettings points to area to save to
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Sets up the sliders to a settings bank
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/
BOOL TraceDlg::SaveSettings(INT32 * pSettings)
{
	pSettings[0]=    (INT32)GetLongGadgetValue(_R(IDC_TRACER_PASSESNUM) 		,0, 100, 0, NULL);
	pSettings[1]=100-(INT32)GetLongGadgetValue(_R(IDC_TRACER_BLURSLIDER) 		,0, 100, 0, NULL); 
	pSettings[2]=50- (INT32)GetLongGadgetValue(_R(IDC_TRACER_MINAREASLIDER)	,0, 100, 0, NULL); 
	pSettings[3]=100-(INT32)GetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER) 		,0, 100, 0, NULL); 
	pSettings[4]=100-(INT32)GetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER) 		,0, 100, 0, NULL); 
	pSettings[5]=100-(INT32)GetLongGadgetValue(_R(IDC_TRACER_ACCURACYSLIDER) 	,0, 100, 0, NULL); 
	pSettings[6]=100-(INT32)GetLongGadgetValue(_R(IDC_TRACER_SMOOTHSLIDER) 	,0, 100, 0, NULL); 
	return TRUE;
}

/*******************************************************************************************

>	TraceMethod TraceDlg::GetTraceMethod()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	Currently selected trace method
	Purpose:	Sets up the sliders to a settings bank
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/
	
TraceMethod TraceDlg::GetTraceMethod()
{
	TraceMethod Method;
	switch (GetSelectedValueIndex(_R(IDC_TRACER_METHOD)))
	{
		case 0:
			Method=TRACEMETHOD_MONO;
			break;
		case 1:
			Method=TRACEMETHOD_GREYSCALE;
			break;
		case 2:
			Method=TRACEMETHOD_256COL;
			break;
		case 3:
		default:
			Method=TRACEMETHOD_TRUECOL;
			break;
	}
	return Method;
}

/*******************************************************************************************

>	BOOL TraceDlg::SetTraceMethod(TraceMethod Method)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	None
	Returns:	TRUE on success else false for failure
	Purpose:	Sets the trace method
	Errors:		None at present
	SeeAlso:	-

*******************************************************************************************/
	
BOOL TraceDlg::SetTraceMethod(TraceMethod Method)
{
	switch (Method)
	{
		case TRACEMETHOD_MONO:
			SetSelectedValueIndex(_R(IDC_TRACER_METHOD), 0);
			break;
		case TRACEMETHOD_GREYSCALE:
			SetSelectedValueIndex(_R(IDC_TRACER_METHOD), 1);
			break;
		case TRACEMETHOD_256COL:
			SetSelectedValueIndex(_R(IDC_TRACER_METHOD), 2);
			break;
		case TRACEMETHOD_TRUECOL:
		default:
			SetSelectedValueIndex(_R(IDC_TRACER_METHOD), 3);
			break;
	}
	return TRUE;
}


/*******************************************************************************************

>	BOOL TraceDlg::ReadParams()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	This is a temporary routine to put the values in the TraceCtl into the dialog

*******************************************************************************************/

BOOL TraceDlg::ReadParams()
{
	SetLongGadgetValue(_R(IDC_TRACER_BLURNUM), 100-GetLongGadgetValue(_R(IDC_TRACER_BLURSLIDER), 0, 100, 0, NULL));
	SetLongGadgetValue(_R(IDC_TRACER_MINAREANUM), 50-GetLongGadgetValue(_R(IDC_TRACER_MINAREASLIDER), 0, 100, 0, NULL));
	SetLongGadgetValue(_R(IDC_TRACER_ACCURACYNUM), 100-GetLongGadgetValue(_R(IDC_TRACER_ACCURACYSLIDER), 0, 100, 0, NULL));
	SetLongGadgetValue(_R(IDC_TRACER_ICTNUM), 100-GetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER), 0, 100, 0, NULL));
	SetLongGadgetValue(_R(IDC_TRACER_FCTNUM), 100-GetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER), 0, 100, 0, NULL));
	SetLongGadgetValue(_R(IDC_TRACER_SMOOTHNUM), 100-GetLongGadgetValue(_R(IDC_TRACER_SMOOTHSLIDER), 0, 100, 0, NULL));
	TraceMethod Method=GetTraceMethod();
	BOOL Mono=(Method == TRACEMETHOD_MONO);
	BOOL Limited=(Method == TRACEMETHOD_GREYSCALE) || (Method==TRACEMETHOD_256COL);
	BOOL TrueCol=(Method == TRACEMETHOD_TRUECOL);
	INT32 Passes = GetLongGadgetValue  (_R(IDC_TRACER_PASSESNUM),	0, 100, 0, NULL);
	if (Method != TRACEMETHOD_TRUECOL) Passes=1;
	
	EnableGadget(_R(IDC_TRACER_FCTNUM), !Mono);
	EnableGadget(_R(IDC_TRACER_FCTSLIDER), !Mono);
	EnableGadget(_R(IDC_TRACER_FCTTXT), !Mono);
	
	EnableGadget(_R(IDC_TRACER_ICTNUM), Passes>1);
	EnableGadget(_R(IDC_TRACER_ICTSLIDER), Passes>1);
	EnableGadget(_R(IDC_TRACER_ICTTXT), Passes>1);
	
	EnableGadget(_R(IDC_TRACER_PASSESNUM), TrueCol);
	EnableGadget(_R(IDC_TRACER_PASSESTXT), TrueCol);

	return TRUE;
}


/*******************************************************************************************

>	void TraceDlg::WriteParams()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	This is a temporary routine to put the values in the dialog into the TraceCtl

*******************************************************************************************/

BOOL TraceDlg::WriteParams()
{
	ERROR2IF((!pTraceControl), FALSE, "No TraceControl in WriteParams()");
	
	TraceMethod Method = TRACEMETHOD_TRUECOL;
	INT32 MinimumArea = 0;
	double MaximumInitialAreaErrorThreshold=0.0;
	double MinimumInitialAreaErrorThreshold=0.0;
	double InitialAreaErrorRatio=0.0;
	double MinGradFillError=0.0;
	double GradFillErrorRatio=0.0;
	double MinPixelError=0.0;
	double MaxPixelError=0.0;
	double PixelErrorRatio=0.0;
	double QuantColours=0.0;
	double Blur = 0.0;
	double Smooth=0.0;

	pTraceControl->GetParams(	&Method,
								&MinimumArea,
								&MaximumInitialAreaErrorThreshold,
								&MinimumInitialAreaErrorThreshold,
								&InitialAreaErrorRatio,
								&MinGradFillError,
								&GradFillErrorRatio,
								&MinPixelError,
								&MaxPixelError,
								&PixelErrorRatio,
								&QuantColours,
								&Blur,
								&Smooth
								);


	MinimumArea 						= 50-GetLongGadgetValue  (_R(IDC_TRACER_MINAREASLIDER),	0, 100, 0, NULL);	
	MaximumInitialAreaErrorThreshold 	= 0.01+0.003*(double)(100-GetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER),		0, 100, 0, NULL));
	MinimumInitialAreaErrorThreshold 	= 0.01+0.003*(double)(100-GetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER),		0, 100, 0, NULL));
	MaxPixelError 						= 0.45+0.16*(double)(/*100-*/GetLongGadgetValue(_R(IDC_TRACER_ACCURACYSLIDER),    	0, 100, 0, NULL));
	MinPixelError = MaxPixelError;
	Blur 								= (double)(100-GetLongGadgetValue(_R(IDC_TRACER_BLURSLIDER),    	0, 100, 0, NULL));
	Smooth 								= (double)(100-GetLongGadgetValue(_R(IDC_TRACER_SMOOTHSLIDER),    	0, 100, 0, NULL));
	QuantColours					 	= 0.01*(double)(/*100-*/GetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER),	0, 100, 0, NULL));
	
	if (QuantColours<0) QuantColours=0;
	QuantColours=QuantColours*QuantColours; // spread it out a little at the low end (i.e. high end on the slider)

	INT32 Passes = GetLongGadgetValue  (_R(IDC_TRACER_PASSESNUM),	0, 100, 0, NULL);
	Method = GetTraceMethod();
	if (Method != TRACEMETHOD_TRUECOL) Passes=1;

	if (Passes<1) Passes=1;
	if (Passes>5) Passes=5;
	if (MaximumInitialAreaErrorThreshold<MinimumInitialAreaErrorThreshold*1.05)
		MaximumInitialAreaErrorThreshold=MinimumInitialAreaErrorThreshold*1.05;
	if (Passes==1)
	{
		MaximumInitialAreaErrorThreshold=MinimumInitialAreaErrorThreshold;
		InitialAreaErrorRatio = 0.5;
	}
	else
	{
		InitialAreaErrorRatio = exp ((1.0/(double)(Passes-1))*(log(MinimumInitialAreaErrorThreshold)-log(MaximumInitialAreaErrorThreshold)));
	}
	MinimumInitialAreaErrorThreshold *= 1.000000001; // For rounding


	TRACEUSER( "Alex", _T("MinimumArea=%d, MaxIE=%f, MinIE=%f, IEAR=%f, MaxPE=%f, Passes=%d\n"),
				MinimumArea,
				MaximumInitialAreaErrorThreshold,
				MinimumInitialAreaErrorThreshold,
				InitialAreaErrorRatio,
				MaxPixelError,
				Passes);

	pTraceControl->SetParams(	&Method,
								&MinimumArea,
								&MaximumInitialAreaErrorThreshold,
								&MinimumInitialAreaErrorThreshold,
								&InitialAreaErrorRatio,
								&MinGradFillError,
								&GradFillErrorRatio,
								&MinPixelError,
								&MaxPixelError,
								&PixelErrorRatio,
								&QuantColours,
								&Blur,
								&Smooth
								);

	return TRUE;
}



/*******************************************************************************************

>	BOOL TraceDlg::FindBitmap()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Inputs:		None
	Outputs:	pOriginal set
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Set pOriginal to point at a bitmap
	Errors:		None at present
	SeeAlso:	-

One ought to call CheckOriginal after this call

*******************************************************************************************/

BOOL TraceDlg::FindBitmap()
{
//	pOriginal = NULL;

	Range Sel(*(GetApplication()->FindSelection()));

	Node* FirstSelectedNode = Sel.FindFirst(); 
	if (FirstSelectedNode != NULL) // No nodes selected so End
	{
	    Node* CurrentNode = FirstSelectedNode;       
	    Node* NextCurrent; 
	    
	    // Do all bitmaps. OK this should pick up the fill as well. Never mind
		while (CurrentNode != NULL)
		{
			NextCurrent = Sel.FindNext(CurrentNode);
	       	if  ( (CurrentNode->IsSelected()) && (CurrentNode->GetRuntimeClass() == CC_RUNTIME_CLASS(NodeBitmap)) ) 
	       	{         
				pOriginal = ((NodeBitmap *)(CurrentNode))->GetBitmap();
				return CheckOriginalBitmap();
	       	}
			CurrentNode = NextCurrent; 
		}

	} 

	// Find the first Fill Attribute in the selection
	AttrFillGeometry* pAttrNode = AttrFillGeometry::FindFirstSelectedAttr();

	while (pAttrNode != NULL)
	{
		if (pAttrNode->IsKindOf(CC_RUNTIME_CLASS(AttrBitmapColourFill)))
		{
			pOriginal = pAttrNode->GetBitmap();
			return CheckOriginalBitmap();
		}

		// Check the next fill
		pAttrNode = AttrFillGeometry::FindNextSelectedAttr();
	}

	return CheckOriginalBitmap();
}

/*******************************************************************************************

>	BOOL TraceDlg::DoTrace()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	31/1/95
	Purpose:	Temporary tracing routine

*******************************************************************************************/

BOOL TraceDlg::DoTrace(BOOL SingleStep)
{
	if (pTraceControl && pOriginal)
	{
		WriteParams();
		KernelBitmap * pDestination;
		KernelBitmap * pTemp;
		pTraceControl->GetBitmaps(NULL, &pDestination, &pTemp);
		if ((!SingleStep) || (!pDestination) || (!pTemp))
			pTraceControl->InitBitmap(pOriginal);

		MonotonicTime MT;

		BOOL Done = FALSE;
		BOOL Abort = FALSE;

		String_64 ProcessMessage(_R(IDS_TRACEDLG_PREPROCESSING));
		BeginSlowJob(100, FALSE, &ProcessMessage);
		ContinueSlowJob();

		while ((!Done) && (!Abort))
		{
			Abort = !(pTraceControl->Trace(&Done));
			// invalidate the gadget with the picture in it

			if (!Abort)
			{
				if (Abort = (BreakKeyHit() ==TRUE) /*Assignment*/)
				{
					ERROR1RAW(_R(IDE_TRACER_ABORT));
				}
			}

			if (Done || Abort || SingleStep || MT.Elapsed(1000, TRUE))
			{
				InvalidateGadget(_R(IDC_TRACER_DESTREDRAW));
				PaintGadgetNow(_R(IDC_TRACER_DESTREDRAW));

				INT32 Percent;
				INT32 NumPaths;
				INT32 NumCusps;
				INT32 NumPoints;

				if ((pTraceControl->GetProgress(&Percent, &NumPaths, &NumCusps, &NumPoints))&& !Abort)
				{
					Abort = !ContinueSlowJob(Percent);
					if (Abort) ERROR1RAW(_R(IDE_TRACER_ABORT));
				}
				SetStatus(TRUE);
				//if (Done) TRACEUSER( "Alex", _T("**Done - %d Paths, %d Cusps, %d Points\n"), NumPaths, NumCusps, NumPoints);

			}

			if (SingleStep)
			{
				EndSlowJob();
				SetStatus();
				return !Abort;
			}

		}
		EndSlowJob();
		SetStatus();
		return (!Abort);
	}
	return SetStatus();
}

/********************************************************************************************

>	BOOL TraceDlg::RenderBitmap(ReDrawInfoType* ExtraInfo, KernelBitmap * BitmapToUse)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		ExtraInfo - The structure that has the extra data we need to start rendering
				BitmapToUse - The bitmap to render
	Outputs:	None
	Returns:	TRUE if succeeded, FALSE & error set if not
	Purpose:	Fills a renderable control with a bitmap
	Errors:		None at present
	SeeAlso:	-

********************************************************************************************/

BOOL TraceDlg::RenderBitmap(ReDrawInfoType* ExtraInfo, KernelBitmap * BitmapToUse)
{
	// Go get a render region
	DocRect VirtualSize(0, 0, ExtraInfo->dx, ExtraInfo->dy);
	RenderRegion* pRender = CreateGRenderRegion(&VirtualSize, ExtraInfo);
	if (pRender!=NULL)
	{
		DialogColourInfo RedrawColours;		// Get a supplier for default dlg colours

		// A Grey colour [...hmmm, it's not a very grey grey any more... oragnge more like]
		DocColour Grey(255,200,0);

		// Render the attributes and the a rectangle
		pRender->SaveContext();
		pRender->SetLineColour(COLOUR_TRANS);

			// Draw a rectangle to fill in the background - Fill with Dialogue Background colour
		pRender->SetFillColour(RedrawColours.DialogBack());
		pRender->DrawRect(&VirtualSize);
	
		if (BitmapToUse)
		{

			NodeBitmap NB;
			NodeBitmap * pNodeBitmap = &NB;
		
			if (!((pNodeBitmap == NULL) || (!pNodeBitmap->SetUpPath(12,12))))
			{
				// Get a new bitmap object for this node.
				pNodeBitmap->GetBitmapRef()->SetBitmap(BitmapToUse);
						
				ENSURE(pNodeBitmap->GetBitmap()->ActualBitmap != NULL, "No bitmap object found!");
		
				DocRect BitmapSize(VirtualSize);
				BitmapInfo BMInfo;
				BitmapToUse->ActualBitmap->GetInfo(&BMInfo);
				if (BMInfo.PixelWidth==0) BMInfo.PixelWidth=1;
				if (BMInfo.PixelHeight==0) BMInfo.PixelHeight=1;

				double BMAspect = ((double)(BMInfo.PixelWidth))/((double)(BMInfo.PixelHeight));
				double RedrawAspect = ((double)(ExtraInfo->dx))/((double)(ExtraInfo->dy));

				if (BMAspect>RedrawAspect)
				{
					// grey at top and bottom
					INT32 NewHeight = (INT32)( 0.5 + ((double)(VirtualSize.Width()))/BMAspect);
					if (NewHeight<1) NewHeight = 1;
					BitmapSize.loy = (VirtualSize.Height()-NewHeight)/2;
					BitmapSize.hiy = BitmapSize.loy + NewHeight;
				}
				else
				{
				 	// grey on left and right
					INT32 NewWidth = (INT32)( 0.5 + ((double)(VirtualSize.Height()))*BMAspect);
					if (NewWidth<1) NewWidth = 1;
					BitmapSize.lox = (VirtualSize.Width()-NewWidth)/2;
					BitmapSize.hix = BitmapSize.lox + NewWidth;
				}

				// And set this in our bitmap node
				pNodeBitmap->CreateShape(BitmapSize);
					
				pNodeBitmap->Render(pRender);
			}
		}

		pRender->RestoreContext();

		// Get rid of the render region
		DestroyGRenderRegion(pRender);
	}
	return TRUE;
}


/********************************************************************************************
>	MsgResult TraceDlg::Message( Msg* Message)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		Message: The message 

	
	Returns:	OK	 		  Message handled ok (return this even if you don't need to 
							  respond to the message). 

				FAIL 		  Something terrible happened whilst processing the message
				 			  eg. we ran out of memory. You must set ERROR if you
				 			  are returning this value. 

				EAT_MSG       The Message was handled ok but don't send it to any
				 			  more MessageHandlers. 

	Purpose:	This is the message handler for the TraceDlg
	 
********************************************************************************************/


MsgResult TraceDlg::Message( Msg* Message)
{
	static INT32 OurMessage = 0;
	if (IS_OUR_DIALOG_MSG(Message))
	{
		DialogMsg* Msg = ((DialogMsg*) Message); 
		
		BOOL EndDialog = FALSE;		// TRUE if we should quit the dialog
		BOOL CommitValues = FALSE; 	// TRUE if we should commit the dialog values

		switch (Msg->DlgMsg)
		{
			case DIM_CREATE:
			{

				SetGadgetRange    (_R(IDC_TRACER_BLURSLIDER), 0, 100, 1);
				SetGadgetBitmaps  (_R(IDC_TRACER_BLURSLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
//				SetLongGadgetValue(_R(IDC_TRACER_BLURSLIDER), 100-50); 

				SetGadgetRange    (_R(IDC_TRACER_ACCURACYSLIDER), 0, 100, 1);
				SetGadgetBitmaps  (_R(IDC_TRACER_ACCURACYSLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
//				SetLongGadgetValue(_R(IDC_TRACER_ACCURACYSLIDER), 100-50); 

				SetGadgetRange    (_R(IDC_TRACER_MINAREASLIDER), 0, 50, 1);
				SetGadgetBitmaps  (_R(IDC_TRACER_MINAREASLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
//				SetLongGadgetValue(_R(IDC_TRACER_MINAREASLIDER), 50-25); 

				SetGadgetRange    (_R(IDC_TRACER_ICTSLIDER), 0, 100, 1);
				SetGadgetBitmaps  (_R(IDC_TRACER_ICTSLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
//				SetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER), 100-35); 

				SetGadgetRange    (_R(IDC_TRACER_FCTSLIDER), 0, 100, 1);
				SetGadgetBitmaps  (_R(IDC_TRACER_FCTSLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
//				SetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER), 100-25); 

				SetGadgetRange    (_R(IDC_TRACER_SMOOTHSLIDER), 0, 100, 1);
				SetGadgetBitmaps  (_R(IDC_TRACER_SMOOTHSLIDER), _R(IDB_QUALITYBASE), _R(IDB_QUALITYSLIDER));
//				SetLongGadgetValue(_R(IDC_TRACER_SMOOTHSLIDER), 50); 

//				SetLongGadgetValue(_R(IDC_TRACER_PASSESNUM), 1);

				pOriginal=NULL;
				pPrevOriginal=NULL;
				OurMessage=0;
				FindBitmap();

				DeleteAllValues(_R(IDC_TRACER_METHOD));

				String_256 Str0(_R(IDS_TRACER_MONOCHROME));
				String_256 Str1(_R(IDS_TRACER_GREYSCALE));
				String_256 Str2(_R(IDS_TRACER_256COL));
				String_256 Str3(_R(IDS_TRACER_TRUECOL));
		  	
				SetStringGadgetValue(_R(IDC_TRACER_METHOD),&Str0,FALSE, 0);
			  	SetStringGadgetValue(_R(IDC_TRACER_METHOD),&Str1,FALSE, 1);
			  	SetStringGadgetValue(_R(IDC_TRACER_METHOD),&Str2,FALSE, 2);
				SetStringGadgetValue(_R(IDC_TRACER_METHOD),&Str3,FALSE, 3);
			
//				SetStringGadgetValue(_R(IDC_TRACER_METHOD),&Str3, FALSE, -1);
				SetComboListLength(_R(IDC_TRACER_METHOD));
				SetTraceMethod((TraceMethod)DefaultMode);
				DefaultSetting[TRACEMETHOD_MONO][0]=1; // ensure this one pass
				DefaultSetting[TRACEMETHOD_256COL][0]=1; // ensure this one pass
				DefaultSetting[TRACEMETHOD_GREYSCALE][0]=1; // ensure this one pass
				LoadSettings(DefaultSetting[(INT32)DefaultMode]);

//				ReadParams();
				WriteParams();
//				SetStatus();
//				FixOriginalComboBox();
				break;
			}
			
			case DIM_SLIDER_POS_CHANGING:
			case DIM_SLIDER_POS_IDLE:
			case DIM_SLIDER_POS_SET:	
				{
					INT32 Passes = GetLongGadgetValue  (_R(IDC_TRACER_PASSESNUM),	0, 100, 0, NULL);
					if (GetTraceMethod() != TRACEMETHOD_TRUECOL) Passes=1;
					
					if (!OurMessage && (Msg->GadgetID == _R(IDC_TRACER_ICTSLIDER) || Msg->GadgetID == _R(IDC_TRACER_FCTSLIDER)))
					{
						// We must fix sliders
						INT32 CurrentInit = 100-GetLongGadgetValue  (_R(IDC_TRACER_ICTSLIDER),	0, 100, 0, NULL);
						INT32 CurrentFinal = 100-GetLongGadgetValue  (_R(IDC_TRACER_FCTSLIDER),	0, 100, 0, NULL);

						OurMessage++;

						if (Msg->GadgetID == _R(IDC_TRACER_FCTSLIDER))
						{
							if (Passes<=1)
							{
								SetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER), 100-CurrentFinal);
							}
							else if (CurrentInit<=CurrentFinal)
							{
								SetLongGadgetValue(_R(IDC_TRACER_ICTSLIDER), 100-((CurrentFinal>=100)?100:CurrentFinal+1));
							}
						}
						else
						{
							if (Passes<=1)
							{
								SetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER), 100-CurrentInit);
							}
							else if (CurrentInit<=CurrentFinal)
							{
								SetLongGadgetValue(_R(IDC_TRACER_FCTSLIDER), 100-((CurrentInit<=0)?0:CurrentInit-1));
							}
						}

						OurMessage--;
					}
					ReadParams();
				}
				break;

			case DIM_TEXT_CHANGED:	// Text changed 
				if (Msg->GadgetID == _R(IDC_TRACER_PASSESNUM)) ReadParams();
				break;

			case DIM_COMMIT:		// Want to commit and quit
				EndDialog = TRUE;
				CommitValues = TRUE; 
				break;

			case DIM_SOFT_COMMIT:	// Want to commit
				CommitValues = TRUE; 
				break; 

			case DIM_CANCEL:		// Want to quit
				EndDialog = TRUE;
				break;

			case DIM_LFT_BN_CLICKED :
			{
				TraceMethod Method = GetTraceMethod();
				// See which button was pressed
				switch (Msg->GadgetID)
				{
					case _R(IDC_TRACER_TRACEBTN) :
					{
						if (!DoTrace(FALSE)) InformError();
						break;
					}
					case _R(IDC_TRACER_RESET) :
					{
						LoadSettings(DefaultSetting[(INT32)Method]);
						break;
					}
					case _R(IDC_TRACER_SAVE) :
					{
						DefaultMode=Method;
						SaveSettings(DefaultSetting[(INT32)Method]);
						break;
					}
					case _R(IDC_TRACER_DEFAULT) :
					{
						LoadSettings(FactorySetting[(INT32)Method]);
						break;
					}
				}
				break;
			}

			case DIM_REDRAW :
				// This is where all the redrawing is done
				// Which control in the window is sending the redraw message (if there are many
				// grdraw controls you can tell which is which from the Gadget ID
				KernelBitmap * pDestination;
				KernelBitmap * pTemp;
				pTraceControl->GetBitmaps(NULL, &pDestination, &pTemp);
				switch (Msg->GadgetID)
				{
					// Draw the redraw_me control in here
					case _R(IDC_TRACER_ORIGREDRAW) :
					{
						RenderBitmap((ReDrawInfoType*) Msg->DlgMsgParam, pOriginal);
						break;
					}
					case _R(IDC_TRACER_DESTREDRAW) :
					{
						RenderBitmap((ReDrawInfoType*) Msg->DlgMsgParam, pDestination);
						break;
					}
				}
				break;
			
			case DIM_SELECTION_CHANGED:
				switch (Msg->GadgetID)
				{
					// Draw the redraw_me control in here
					case _R(IDC_TRACER_BITMAP) :
					{
						ReadOriginalComboBox();
						break;
					}
					case _R(IDC_TRACER_METHOD) :
					{
						LoadSettings(DefaultSetting[(INT32)GetTraceMethod()]);
						break;
					}
				}
				break;											       

		}

		// End dialog here
		if (EndDialog)	// Dialog communication over 
		{	
			if (pTraceControl)
			{
				BOOL Done;
				pTraceControl->GetProgress(NULL,NULL,NULL,NULL,&Done);
				if (CommitValues && pOriginal && Done)
				{
					TraceOpParam param;
					param.pTraceControl = pTraceControl;
		
					OpDescriptor* pOpDesc=OpDescriptor::FindOpDescriptor("TraceOp");
					ERROR3IF_PF(pOpDesc==NULL,("Couldn't find OPTOKEN_OPTRACEOP op descriptor"));
					pOpDesc->Invoke((OpParam*)&param);
				}

				pTraceControl->InitBitmap(NULL);
				pOriginal = NULL;
				pTraceControl->RemoveTree(TRUE /* delete it*/);
			}

			Close(); 		   
			TRACEUSER( "Alex", _T("Calling End()\n"));
			End(); 	 	// End of dialog 
	   	}
		//return (DLG_EAT_IF_HUNGRY(Msg));   // I return EAT_MSG unless the message needs to be sent to all dialogs 
	} else if (MESSAGE_IS_A(Message, SelChangingMsg))		// Selection changed - use new bitmap
	{
		SelChangingMsg *Msg = (SelChangingMsg *) Message;
		switch ( Msg->State )
		{
			case SelChangingMsg::SelectionState::SELECTIONCHANGED:
			case SelChangingMsg::SelectionState::NODECHANGED:
//				pOriginal=NULL;
				FindBitmap();
				break;
		}
	} else if (MESSAGE_IS_A(Message, BitmapListChangedMsg))		// Selection changed - use new bitmap
	{
		CheckOriginalBitmap();
		FixOriginalComboBox();
	} else if (MESSAGE_IS_A(Message, TraceMsg))
	{
		pOriginal=((TraceMsg*)(Message))->pBitmap;
		CheckOriginalBitmap();
	}

	return DialogOp::Message(Message);
} 

/*******************************************************************************************

>	OpState	TraceDlg::GetState(String_256*, OpDescriptor*)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Returns the OpState of the dialogue operation

*******************************************************************************************/

OpState	TraceDlg::GetState(String_256*, OpDescriptor*)
{
	return OpState(FALSE, FALSE);
}

		 

/*******************************************************************************************

>	BOOL TraceDlg::Init()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Returns:	FALSE if it fails (due to lack of memory)
	Purpose:	Creates an OpDescriptor for a Trace Dialog

*******************************************************************************************/
BOOL TraceDlg::Init()
{  
	INT32 Mode;
	INT32 Setting;
	GetApplication()->DeclareSection( "Tracer", 40);
	GetApplication()->DeclarePref( "Tracer", "DefaultMode", &DefaultMode, 0, ((INT32)TRACEMETHOD_ILLEGAL)-1);
	String_256 PrefName;
	String_256 PrefBase("Tracer mode #1%d setting #2%d"); // not resourced as .ini file setting
	
	for (Mode=0; Mode<(INT32)NUM_TRACEMETHOD; Mode++) for (Setting=0; Setting<(INT32)NUM_TRACESETTING; Setting++)
	{
		PrefName._MakeMsg(PrefBase, Mode, Setting);
		GetApplication()->DeclarePref( "Tracer", TEXT(PrefName), &(DefaultSetting[Mode][Setting]), 0, 100);
	}

	return (RegisterOpDescriptor(0,								// Tool ID
 								_R(IDS_TRACEDLG),	 				// String resouirce ID
								CC_RUNTIME_CLASS(TraceDlg),		// Runtime class
 								OPTOKEN_TRACEDLG,				// Token string
 								TraceDlg::GetState,				// GetState function
								0,								// Help ID
								_R(IDBBL_BITMAPTRACER),				// Bubble ID
								_R(IDD_BARCONTROLSTORE),			// Resource ID
								_R(IDC_BITMAPTRACER),				// Control ID
								SYSTEMBAR_UTILITIES,			// Bar ID
								TRUE,							// Recieve system messages
								FALSE,							// Smart duplicate operation
								TRUE,							// Clean operation
								0,								// No vertical counterpart
								_R(IDS_TRACER_ONE),
								GREY_WHEN_NO_CURRENT_DOC | DONT_GREY_WHEN_SELECT_INSIDE)	// String for one copy only
		&& TraceOp::Init()); // HORRIBLE! But means I don't have to get a lock on main.cpp
}   
 

         

/*******************************************************************************************

>	void TraceDlg::Do(OpDescriptor*)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/12/94
	Purpose:	Creates and shows a Render Demo dialog

*******************************************************************************************/
void TraceDlg::Do(OpDescriptor*)
{
	// if we can create the dialog, then open it
	if (Create())
		Open();
	else
		TRACE( _T("Failed to create Render Demo Dialog\n"));
}

/********************************************************************************************

>	void TraceDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the BfxOp for the first time
	Errors:		-
	SeeAlso:	-

The first parameter should be a kernel bitmap to use or null to just bring the dialog up

********************************************************************************************/

void TraceDlg::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{
	Do(pOp);
	KernelBitmap * pBitmap = NULL;
	if (pParam) pBitmap = (KernelBitmap *)(void *)(pParam->Param1);
	if (pBitmap && pBitmap->IsKindOf(CC_RUNTIME_CLASS(KernelBitmap)))
	{
		pOriginal=pBitmap;
		OriginalChanged();
	}
	return;
}


/********************************************************************************************

>	TraceOp::TraceOp() 

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	TraceOp constructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TraceOp::TraceOp(): SelOperation()  
{                                
}
   
/********************************************************************************************

>	BOOL TraceOp::Init()

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if the operation could be successfully initialised 
				FALSE if no more memory could be allocated 
				
	Purpose:	BlobyOp initialiser method
	Errors:		ERROR will be called if there was insufficient memory to allocate the 
				operation.
	SeeAlso:	-

********************************************************************************************/

BOOL TraceOp::Init()
{  
	return (SelOperation::RegisterOpDescriptor(
				 							0,
				 							_R(IDS_TRACEOP),
											CC_RUNTIME_CLASS(TraceOp),
				 							OPTOKEN_TRACEOP,
				 							TraceOp::GetState,
				 							0,	/* help ID */
				 							0,  /* bubble help ID */
				 							0	/* bitmap ID */)); 
}

/********************************************************************************************

>	OpState	TraceOp::GetState(String_256*, OpDescriptor*)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		-
	Outputs:	-
	Returns:	The state of the TraceOp
	Purpose:	For finding the TraceOp's state. 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

OpState	TraceOp::GetState(String_256*, OpDescriptor*)
{    
	OpState OpSt;
	return(OpSt);
}
		
/********************************************************************************************

>	void TraceOp::DoWithParam(OpDescriptor* pOp, OpParam* pParam)	   

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	23/2/95
	Inputs:		OpDescriptor (unused)
	Outputs:	-
	Returns:	-
	Purpose:	Performs the TraceOp for the first time
	Errors:		-
	SeeAlso:	-

Blatantly copied from BitFilt::DoImportBitmap

********************************************************************************************/

void TraceOp::DoWithParam(OpDescriptor* pOp, OpParam* pParam)
{               
	DocCoord DropPos;
	Spread * pSpread;
	
	if (!GetWorkingDoc()) return;
	
	if (!FindCentreInsertionPosition(&pSpread, &DropPos))
	{
		InformError();
		FailAndExecute();
		End();
		return;
	}
	
	// Put up a progress display/hourglass (FALSE => no delay, show it NOW!)
	BeginSlowJob(100, FALSE);
	TraceControl * pTraceControl = (((TraceOpParam *)(pParam))->pTraceControl);
	if (!pTraceControl) return; // we've done.

	KernelBitmap * pKB = NULL;
	Node * pXNode = pTraceControl->GetPaths();
	if (!pXNode || !pXNode->IsKindOf(CC_RUNTIME_CLASS(NodeRenderableBounded))) return; // nothing to trace
	NodeRenderableBounded *pNodeRenderableBounded = (NodeRenderableBounded *)pXNode;

	if (!pTraceControl->GetBitmaps(&pKB, NULL, NULL)) 
	{
		InformError();		
		FailAndExecute();
		End();
		return;
	}
	
	if (!pKB)
	{
		ERROR2RAW("Where did that DropPos go then?");
		InformError();		
		FailAndExecute();
		End();
		return;
	}

	// First, set the rectangle to the right size for the bitmap...
	BitmapInfo BMInfo;
	pKB->ActualBitmap->GetInfo(&BMInfo);

	DocRect BoundsRect;
	BoundsRect.lo.x = DropPos.x - (BMInfo.RecommendedWidth/2);
	BoundsRect.lo.y = DropPos.y - (BMInfo.RecommendedHeight/2);
	BoundsRect.hi.x = BoundsRect.lo.x + BMInfo.RecommendedWidth;
	BoundsRect.hi.y = BoundsRect.lo.y + BMInfo.RecommendedHeight;
	DocCoord Point1 = DocCoord(BoundsRect.hi.x, BoundsRect.lo.y);
	DocCoord Point2 = DocCoord(BoundsRect.lo.x, BoundsRect.hi.y);

	pTraceControl->RemoveTree(FALSE); // Don't delete it
	pTraceControl->InitBitmap(); // to get us some room back

	Matrix tMatrix(Div32By32(Point1.x-BoundsRect.lo.x,BMInfo.PixelWidth<<8),
						Div32By32(Point2.x-BoundsRect.lo.x,BMInfo.PixelWidth<<8),
				   Div32By32(Point1.y-BoundsRect.lo.y,BMInfo.PixelHeight<<8),
				   		Div32By32(Point2.y-BoundsRect.lo.y,BMInfo.PixelHeight<<8),
				   BoundsRect.lo.x,BoundsRect.lo.y);
	Trans2DMatrix Trans(tMatrix);

	pNodeRenderableBounded->Transform(Trans);
		
	// Insert the node, but don't invalidate its region
	if (!DoInsertNewNode(pNodeRenderableBounded, pSpread, TRUE))
	{
		Node * pFirstChild = pNodeRenderableBounded->FindFirstChild();
		if (pFirstChild) pNodeRenderableBounded->DeleteChildren(pFirstChild);
		delete pNodeRenderableBounded;

		EndSlowJob();
		FailAndExecute();
		InformError();
		End();
		return;
	}

	// All ok
	EndSlowJob();
	End();
	return;
	
}                                                      

/********************************************************************************************

>	BOOL TraceOp::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	24/3/95
	Inputs:		-
	Outputs:	Spread:  The spread to place the clipboard objects on
				Position:The centre of the view (Spread coords)
	Returns:	TRUE if succeeded else FALSE
	Purpose:	Finds the centre insertion position for clipboard objects
	Errors:		ERROR2 in various situations
	Scope:		private
	SeeAlso:	-

********************************************************************************************/

BOOL TraceOp::FindCentreInsertionPosition(Spread** Spread, DocCoord* Position)
{
	// ---------------------------------------------------------------------------------
	// Find out which spread is in the centre of the view 
	// this is the spread that the pasted objects will be placed on

	ERROR2IF((!Spread)||(!Position),FALSE,"Invalid call to FindCentreInsertionPoint()");
	// Obtain the current DocView
	DocView* CurDocView = GetWorkingDocView();

	ERROR2IF(!CurDocView, FALSE, "The current DocView is NULL"); 

	WorkRect WrkViewRect = CurDocView->GetViewRect();

	ERROR2IF((WrkViewRect.IsEmpty() || (!WrkViewRect.IsValid()) ), FALSE, "Invalid WorkRect");
	
	// Determine the centre of the view
	WorkCoord WrkCentreOfView; 
	WrkCentreOfView.x = WrkViewRect.lox	+ (WrkViewRect.Width()/2); 
	WrkCentreOfView.y = WrkViewRect.loy	+ (WrkViewRect.Height()/2);
	
	// FindEnclosing spread requires an OilCoord
	OilCoord OilCentreOfView = WrkCentreOfView.ToOil(CurDocView->GetScrollOffsets()); 

	// Find out which spread to insert the pasteboard objects onto
	(*Spread) = CurDocView->FindEnclosingSpread(OilCentreOfView);
	ERROR2IF(!(*Spread), FALSE, "No spread?!");

	// Phew
	// ---------------------------------------------------------------------------------
	// Now lets find the spread coordinate of the centre of the view
	DocRect DocViewRect = CurDocView->GetDocViewRect(*Spread);

	ERROR2IF( ( (DocViewRect.IsEmpty()) || !(DocViewRect.IsValid())), FALSE, "DocViewRect is invalid" );

	// Find the centre of the DocViewRect
   	DocCoord DocCentreOfView; 
	DocCentreOfView.x = DocViewRect.lox	+ (DocViewRect.Width()/2); 
	DocCentreOfView.y = DocViewRect.loy	+ (DocViewRect.Height()/2);

	// Now convert from DocCoords to spread coords
	(*Spread)->DocCoordToSpreadCoord(&DocCentreOfView);

	// Finally, fill in the return value
	*Position = DocCentreOfView;
	
	return TRUE;  
}


/********************************************************************************************

>	static void TraceMsg::OpenOrUse(KernelBitmap * pBitmap)

	Author:		Alex_Bligh (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/02/95
	Inputs:		pBitmap = bitmap to use
	Outputs:	-
	Returns:	-
	Purpose:	Open the tracer dialog if it's not already open, else just uses that bitmap
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

void TraceMsg::OpenOrUse(KernelBitmap * pBitmap)
{
	// Obtain a pointer to the op descriptor for the create operation 
	OpDescriptor* OpDesc = OpDescriptor::FindOpDescriptor(CC_RUNTIME_CLASS(TraceDlg));
	if (!OpDesc) return;

	// Only one live instance of the operation is allowed. It's probably a dialog 
	if (!MessageHandler::MessageHandlerExists(CC_RUNTIME_CLASS(TraceDlg)))
	{
		OpDesc->Invoke(&OpParam((INT32)pBitmap,(INT32)NULL));		 
	}
	else
	{
		BROADCAST_TO_CLASS(TraceMsg(pBitmap),DialogOp);	
	}	
	return;
}


