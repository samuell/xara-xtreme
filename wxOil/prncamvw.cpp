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

// PrintCamView - subclass of CamView used for printing.

/*
*/

#include "camtypes.h"

#include "prncamvw.h"

#include "camdoc.h"
#include "scrcamvw.h"
#include "document.h"
#include "prntview.h"
#include "docview.h"
#include "spread.h"
//#include "printdlg.h"
#include "prdlgctl.h"
#include "princomp.h"
#include "printctl.h"
#include "psrndrgn.h"
//#include "tim.h"
#include "camelot.h"
//#include "resource.h"
#include "app.h"
#include "prnmks.h"
#include "opbevel.h"

DECLARE_SOURCE("$Revision: 662 $");

#define new CAM_DEBUG_NEW
#define USERNAME "Mike"

BOOL PrintMonitor::StartPrintJob(HDC PrintJobDC)
{
	// Make sure we don't already have a print job going.
	if (CurrentPrintJob != NULL)
	{
		// Print job going already - must have been terminated early if the
		// user has initiated another print job, so clean it up first.
		if (::AbortDoc(CurrentPrintJob) <= 0)
		{
			// Could not abort print job.
			CurrentPrintJob = NULL;
			ERROR1(FALSE, _R(IDE_ALREADY_PRINTING));
		}
	}

	// Ok, starting a print job - keep a record of it.
	CurrentPrintJob = PrintJobDC;
	return TRUE;
}

void PrintMonitor::EndPrintJob()
{
	// Ok, throw away our reference to the print job.
	CurrentPrintJob = NULL;
}

BOOL PrintMonitor::IsPrintStatusOK()
{
	if (CCamApp::SeriousErrorCount == MySeriousErrorCount)
	{
		// We haven't had a serious error so any current print job will be
		// proceeding normally.
		return TRUE;
	}

	// Update our error count so we get back in sync.
	MySeriousErrorCount = CCamApp::SeriousErrorCount;

	BOOL Result = TRUE;

	// An error has occured - delete the print job if necessary.
	if (CurrentPrintJob != NULL)
	{
		// Use a semaphore structure here so we don't do this more than once.
		HDC PrintDC = CurrentPrintJob;
		CurrentPrintJob = NULL;
		::AbortDoc(PrintDC);

		// Something's gone wrong - tell caller to remove all printing render regions.
		Result = FALSE;
	}
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
	CCPrintInfo *pPrintInfo = CCPrintInfo::GetCurrent();
	if (pPrintInfo != NULL)
	{
		// Close down and delete this print info object (this will delete the print progress 
		// dialog etc.)
		pPrintInfo->EndPrinting();
		delete pPrintInfo;

		// Something's gone wrong - tell caller to remove all printing render regions.
		Result = FALSE;
	}
#endif //webster
	// Re-enable print Operation, and return result to caller.
	SetPrintingActive(FALSE);
	return TRUE;
}

void PrintMonitor::SetPrintingActive(BOOL Flag)
{
	PrintingIsActive = Flag;
}

BOOL PrintMonitor::IsPrintingActive()
{
	return (PrintingIsActive);
}


/********************************************************************************************

>	BOOL PrintMonitor::IsPrintingNow()

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/95
	Returns:	TRUE if we are activly printing NOW, FALSE if not
	Purpose:	Allows you to determine if there is a currently active print happening. This
				is used to disable screen redrawing, only during the print itself.

********************************************************************************************/

BOOL PrintMonitor::IsPrintingNow()
{
	// if there is no print job, then we are not printing
	if (CurrentPrintJob==NULL)
		return FALSE;

	// otherwise we are printing
	return TRUE;
}



/********************************************************************************************

>	void PrintMonitor::WantFullRedraw(BOOL WantRedraw)

	Author:		Rik_Heywood (Xara Group Ltd) <camelotdev@xara.com>
	Created:	3/8/95
	Inputs:		WantRedraw - TRUE if we want to force a redraw, FALSE if not
	Purpose:	Use this function to tell the PrintMonitor that a Full Redraw will be needed
				at the end of the print job. You can also use this function to tell it
				that the Full Redraw has been done.

********************************************************************************************/

void PrintMonitor::WantFullRedraw(BOOL WantRedraw)
{
	FullRedrawNeeded = WantRedraw;
}


BOOL PrintMonitor::InitPrefs()
{
	// Declare and load the .INI file settings.
	BOOL ok = Camelot.DeclareSection(TEXT("Printing"), 10) &&
		   	  Camelot.DeclarePref(TEXT("Printing"), TEXT("PrintMaskType"),
		   					   	  (INT32 *) &PrintMonitor::PrintMaskType, MASK_SIMPLE, MASK_OPTIMAL) &&
		   	  Camelot.DeclarePref(TEXT("Printing"), TEXT("PrintWithDriverBands"),
		   					   	  &PrintMonitor::PrintWithDriverBands, FALSE, TRUE);
	return ok;
}


// No current print job.
HDC PrintMonitor::CurrentPrintJob = NULL;
BOOL PrintMonitor::PrintingIsActive = FALSE;
BOOL PrintMonitor::FullRedrawNeeded = FALSE;

// No errors as yet.
INT32 PrintMonitor::MySeriousErrorCount = 0;

// Default to printing without mask, but with bands.
BOOL PrintMonitor::PrintWithDriverBands = TRUE;
BOOL PrintMonitor::PrintWithMask = TRUE;
PrintMonitor::MaskType PrintMonitor::PrintMaskType = MASK_OPTIMAL;


static RECT DriverBand;
static RECT TestBand;

/********************************************************************************************

>	void ScreenView::OnFilePrint()

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	02/09/95
	Purpose:	The main printing control function.  This is a modified version of the MFC
				one because MFC does not allow the concept of having a mapping between
				pages and physical pieces of paper other than 1 to 1.  i.e. MFC can't handle
				two-up printing, fit lots, pamphlet printing etc.
	Errors:		-
	SeeAlso:	CView::OnFilePrint(); CCamView::OnFilePrintPreview()

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
void ScreenView::OnFilePrint()
{
	// Set Current states...
	Document* KernelDoc = GetDocument()->GetKernelDoc();
	ENSURE(KernelDoc != NULL, "Oh no!!");
	KernelDoc->SetCurrent();
	pDocView->SetCurrent();

	List CompoundNodeList;
	NodeCompound * pCompound = NULL;
	NodeListItem * pItem ;
	double dpi = 96.0;

	Spread * pSpread = Document::GetSelectedSpread();

	TRACEUSER( USERNAME, _T("OnFilePrint()\n"));

	// get default print info
	CCPrintInfo *pPrintInfo;
	TRY
	{
		pPrintInfo = new CCPrintInfo(KernelDoc,this);
	}
	CATCH(CException, e)
	{
		// Out of memory - report error and exit.
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		InformError();
		return;
	}
	END_CATCH

	ASSERT(pPrintInfo->m_pPD != NULL);    // must be set
	BOOL StartedPrinting = FALSE;

	// Make sure the user can't get 2 print dialog boxes up
	PrintMonitor::SetPrintingActive(TRUE);

	if (!OnPreparePrinting(pPrintInfo))
	{
		TRACEUSER( USERNAME, _T("OnPreparePrinting() returned FALSE\n"));
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	CString strTemp;

	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("Entering print loop\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));

	ERROR3IF(pPrintInfo->m_pPD->m_pd.hDC == NULL, "NULL DC when trying to print.");
	// must be set (did you remember to call DoPreparePrinting?)

	// Let's see if we can start printing...
	if ((pPrintInfo->m_pPD->m_pd.hDC == NULL) || 
		!PrintMonitor::StartPrintJob(pPrintInfo->m_pPD->m_pd.hDC))
	{
		TRACEUSER( USERNAME, _T("StartPrintJob() screwed up\n"));

		// No - something went wrong.
		if (pPrintInfo->m_pPD->m_pd.hDC == NULL)
		{
			// No DC - set a generic "we can't print" error.
			Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
		}

		// Tell user and exit.
		InformError();
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	// Setup the printing DC
	CDC dcPrint;

	// Attach printer dc
	dcPrint.Attach(pPrintInfo->m_pPD->m_pd.hDC);
	dcPrint.m_bPrinting = TRUE;
	OnBeginPrinting(&dcPrint, pPrintInfo);

	// Get access to our print control object
	PrintControl *pPrintControl = pPrintView->GetPrintControl();

	// disable main window while printing & init printing status dialog
	AfxGetMainWnd()->EnableWindow(FALSE);

	// set up document info and start the document printing process
	DOCINFO* pDocInfo = NULL;

	// start document printing process
	if  ((pPrintInfo->SetAbortProc(&dcPrint) < 0) ||
		 !pPrintInfo->SetUpDocInfo(&pDocInfo) 	  ||		// SetUpDocInfo() allocs a DOCINFO for pDocInfo
		 pDocInfo == NULL 						  ||
		 (dcPrint.StartDoc(pDocInfo) == SP_ERROR))
	{
		TRACEUSER( USERNAME, _T("Unable to start document\n"));

		// enable main window before proceeding
		AfxGetMainWnd()->EnableWindow(TRUE);

		// cleanup and show error message
		OnEndPrinting(&dcPrint, pPrintInfo);

		BOOL PrintToFileUserAborted = pPrintInfo->GetPrintControl ()->GetPrintToFile ();

		delete pPrintInfo;
		pPrintInfo = NULL;
		PrintMonitor::EndPrintJob();

		// Lose our PrintView
		delete pPrintView;
		pPrintView = NULL;

		// Delete a the docinfo object
		if (pDocInfo != NULL)
		{
			delete pDocInfo;
			pDocInfo = NULL;
		}

		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
		if (!PrintToFileUserAborted)
		{
			AfxMessageBox(_R(AFX_IDP_FAILED_TO_START_PRINT));
		}
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	// CGS:  any blended compound nodes MUST be tagged as having been generated for printing
	NodeGroup::SetIsForPrinting (TRUE);

	// begin page printing loop
	BOOL bError = TRUE;

	// Set up the print layout system.
	// This allows GetNextPaper() and ReprintPaper() functions
	// to be called.
	if (!pPrintInfo->StartPrinting())
	{
		TRACEUSER( USERNAME, _T("StartPrinting() failed\n"));
		// Error in starting printing.
		Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
		goto ExitPrint;
	}

	// Must remember to tell print info object when we are done.
	StartedPrinting = TRUE;

	// Work out if we should do our own PostScript
	BOOL DoPostScript;
	if ((pPrintControl->GetPrintMethod() == PRINTMETHOD_NORMAL) &&
		(CCDC::GetType(&dcPrint, TRUE) == RENDERTYPE_PRINTER_PS))
	{
		// Not printing as a bitmap and it is a PostScript printer so mark it as such.
		DoPostScript = TRUE;
	}
	else
	{
		// Either it's not a PostScript printer or we are printing as a bitmap,
		// so don't do weird PostScript stuff
		DoPostScript = FALSE;
	}

	// If we are printing as bitmap, then set the "Printing blah.art" message in the dialog.
	// (This is done by RenderSimpleView() when we are printing normally).
	if ((pPrintControl->GetPrintMethod() != PRINTMETHOD_NORMAL) && (pPrintInfo != NULL))
		// We going to print the document now
		pPrintInfo->SetPrinting();

	// Ask the printer if it supports banding. We don't actually need the result of this call,
	// but asking the printer about banding has the side effect that the printer driver then
	// knows that we support banding! If we don't do this, then some printing (e.g. to an HP DeskJet
	// on Win3.1) simply chucks out blank pages, because the driver thinks we can't band, and there
	// isn't enough memory/disc space (or something) to do the page in one go.
	//
	// BLOCK
	{
		BOOL BandInfoSupported = FALSE;
		WORD wEscape = BANDINFO;
		if (!DoPostScript && 
			PrintMonitor::PrintWithDriverBands &&
			dcPrint.Escape(QUERYESCSUPPORT, sizeof(WORD), (LPCSTR) &wEscape, NULL) > 0)
		{
			// The BANDINFO Escape is supported  -  but we don't actually care!
			BandInfoSupported = TRUE;
		}
	}

	// DMc 22-7-99
	// MRH 11/9/00 - Major rewrite of the below david code. Fixed major logic problem and
	// cut out the regeneratenodes function as this is not required!
	BevelTools::GetAllNodesUnderNode(pSpread, &CompoundNodeList, CC_RUNTIME_CLASS(NodeCompound));
	
	dpi = dcPrint.GetDeviceCaps(LOGPIXELSX);

	if (pPrintControl)
		dpi = (double)pPrintControl->GetDotsPerInch();
	
	pItem = (NodeListItem *)CompoundNodeList.GetHead();
	
	while (pItem)
	{
		pCompound = (NodeCompound *)pItem->pNode;
		
		if (pCompound)
		{
			if (pCompound->RegenerateForPrinting())
			{
				pCompound->SetDPI(dpi);
				pCompound->SetPrinting(TRUE);
			}
		}
		
		pItem = (NodeListItem *)CompoundNodeList.GetNext(pItem);
	}
	
	TRACEUSER( "DavidM", _T("Beginning print dpi %d\n"), dpi);

	// Loop around for each physical page ("paper") we have to print. Note that now we may print out
	// multiple physical pieces of paper for each "paper" (page) as we may do C, M, Y, K plates etc.

	while (pPrintInfo->MorePaper())
	{
		// Start off the print marks system bits for this paper
		// We lock progress window updates during this call so that the progress window will
		// ignore all attempts to update the display.
		pPrintInfo->LockProgressUpdate(TRUE);
			EnumeratePagePatches(pPrintInfo);
		pPrintInfo->LockProgressUpdate(FALSE);

		// Initialise the plate printing system for any plates of this page
		// It also sets up GetNextPlate() and ReprintPlate functions
		UINT32 plateerr;
		if (!pPrintInfo->StartPlatePrinting(pPrintView, &plateerr))
		{
			TRACEUSER( USERNAME, _T("StartPlatePrinting() failed\n"));
			// Error in startplateprinting! the error id may have been returned
			// We ignore the id if zero, ie StartPlatePrinting reported it.
			if (plateerr>0)
			{
				String_64 reason;
				String_256 ErrorMsg;
				BOOL ok = ( reason.Load(plateerr) );
				ok = ok && ( ErrorMsg.MakeMsg( _R(IDE_PRINT_ERROR), (LPCTSTR) reason) > 0);
				if (ok)
					Error::SetError(_R(IDE_PRINT_ERROR), (TCHAR *) ErrorMsg, 0);
				else
					Error::SetError(_R(IDE_PRINT_USERABORT));
			}
			goto ExitPrint;
		}
		
		// Loop around for each logical plate we have to print - this generates a piece of physical
		// paper out of the printer on each pass.
		while (pPrintInfo->MorePlates())
		{
			// Get the first "paper" (plate or page) to print
			pPrintInfo->GetNextPaper();

			// and set up the plate printing system ready to print the next plate on the next pass
			if (!pPrintInfo->SetNextPlate(pPrintView))
			{
				TRACEUSER( USERNAME, _T("SetNextPlate() failed!\n"));
				// Error in starting printing.
				Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}

			// Now, prepare this paper (plate/page) for printing
			TRACEUSER( USERNAME, _T("Starting physical page\n"));			
			OnPrepareDC(&dcPrint, pPrintInfo);

			// check for end of print
			if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
			{
				TRACEUSER( USERNAME, _T("Print abort 1\n"));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}

			// attempt to start the current paper
			if (dcPrint.StartPage() < 0)
			{
				TRACEUSER( USERNAME, _T("Unable to StartPage()\n"));
				Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}				  

			// Is it a PostScript printer?
			if (DoPostScript)
			{
				// Yes, so before we start the page, we send our PostScript procset to the device.
				if (!PrintPSRenderRegion::InitPSDevice(&dcPrint, pPrintView))
				{
					TRACEUSER( USERNAME, _T("Unable to Init PS device\n"));
					Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
					pPrintInfo->EndPlatePrinting(pPrintView);
					goto ExitPrint;
				}
			}

			// If banding, find the first band to print
			// Only use driver banding if the preference says so, and never band
			// to PS devices (yes, the Win95 PS driver supports banding, except
			// not really and it just screws up).
			if (PrintMonitor::PrintWithDriverBands && !DoPostScript)
			{
				// Do the driver banding
				dcPrint.Escape(NEXTBAND, 0, NULL, (LPSTR) &DriverBand);

				TRACEUSER( USERNAME, _T("INITIALBAND: (%d,%d), (%d, %d)\n"),
						 DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
			}

			TRACEUSER( USERNAME, _T("Starting PrintPaper() loop...\n"));

			// Now render the paper into each band.
			BOOL MoreBands = TRUE;
			while (MoreBands)
			{
				TRACEUSER( USERNAME, _T("Calling PrintPaper()...\n"));
				// Print this piece of paper (to the current band)
				if (!PrintPaper(&dcPrint, pPrintInfo, pPrintControl))
				{
					// Something went wrong - quit and abort print job.
					TRACEUSER( USERNAME, _T("PrintPaper() failed\n"));
					Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
					pPrintInfo->EndPlatePrinting(pPrintView);
					goto ExitPrint;
				}

				// Only use driver banding if the preference says so, and never band to PS devices.
				MoreBands = FALSE;		// Assume no more bands until told otherwise
				if (PrintMonitor::PrintWithDriverBands && !DoPostScript)
				{
					// We've printed this band - see if printer driver has any bands left,
					// NB we kludge this as some drivers keep returning the same band...fabby eh?
					dcPrint.Escape(NEXTBAND, 0, NULL, (LPSTR) &DriverBand);
					TRACEUSER( USERNAME, _T("Driver returned valid band: (%d,%d), (%d, %d)\n"),
							  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);

					// If it does, then we need to loop around to print this physical piece of paper
					// into the next band...
					if (!IsRectEmpty(&DriverBand))
					{
						TRACEUSER( USERNAME, _T("Forcing a reprint of paper [for band: (%d,%d), (%d, %d)]\n"),
								  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
						MoreBands = TRUE;
					}
					else
					{
						TRACEUSER( USERNAME, _T("End of bands for this page\n"));
					}
				}

				// If we're going to print more bands, then set up the "paper" for the next band
				if (MoreBands)
				{
					// Tell the print control that we want the same "paper" again, rather than moving
					// on to the next one.
					pPrintControl->ReprintPaper();

					// And reset all the PrintInfo data for this "paper" ready for the next band
					pPrintInfo->GetNextPaper();
				}
			}

			TRACEUSER( USERNAME, _T("Ending physical page\n"));
			
			// End of page clean up.
			// Note that we don't check for an error from EndPage() because due to
			// a bug (in Windows/GDI) it returns -1 when using banding.
			// The MS workaround is to not check the return value!
			dcPrint.EndPage();

			if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
			{
				TRACEUSER( USERNAME, _T("Print abort 2\n"));
				pPrintInfo->EndPlatePrinting(pPrintView);
				goto ExitPrint;
			}
		
			// Any more plates to print?, if so we need to print the plate as a complete new page.
			// We set ReprintPaper so that the next pass tries to print the same "paper" (document page)
			// again, but we've moved it on to the next plate (in SetNextPlate, above), so we'll actually
			// be rendering a different plate the next time around.
			if (pPrintInfo->MorePlates())
				pPrintControl->ReprintPaper();
		}	

		// End the plate printing function
		pPrintInfo->EndPlatePrinting(pPrintView);
	}

	// Set the flag to indicate that we're exiting happily
	bError = FALSE;


ExitPrint:
	// cleanup document printing process
	PrintMonitor::EndPrintJob();
	PrintMonitor::SetPrintingActive(FALSE);

	if (!bError)
		dcPrint.EndDoc();
	else
	{
		TRACEUSER( USERNAME, _T("ExitPrint: an error occured()\n"));
		// An error occured - abort the job.
		dcPrint.AbortDoc();

		// If the user did not cancel the job, report what went wrong.
		if (!pPrintInfo->Abort())
			InformError();
	}

	AfxGetMainWnd()->EnableWindow();    // enable main window

	OnEndPrinting(&dcPrint, pPrintInfo);    // clean up after printing

	// Clean up kernel printing code
	if (StartedPrinting)
		pPrintInfo->EndPrinting();

	dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor

	delete pPrintInfo;
	pPrintInfo = NULL;

	// Lose our PrintView (if it exists) - not needed anymore.
	delete pPrintView;
	pPrintView = NULL;

	// Delete a the docinfo object
	if (pDocInfo != NULL)
	{
		delete pDocInfo;
		pDocInfo = NULL;
	}

	// If we need to redraw everything then do it.
	if (PrintMonitor::FullRedrawNeeded)
	{
		// iterate through all the documents and get them to redraw themselves
		Document* pDoc = (Document*) Camelot.Documents.GetHead();

		while (pDoc!=NULL)
		{
			// Get it to redraw itself
			pDoc->ForceRedraw();

			// get the next document in the list
			pDoc = (Document*) Camelot.Documents.GetNext(pDoc);
		}
	}

	// mark the PrintMonitor as not needing a redraw
	PrintMonitor::WantFullRedraw(FALSE);

	// DMc restore the compound nodes dpi's
	pItem = (NodeListItem *)CompoundNodeList.GetHead();
	
	while (pItem)
	{
		pCompound = (NodeCompound *)pItem->pNode;
		
		if (pCompound)
		{
			if (pCompound->RegenerateForPrinting())
			{
				pCompound->SetDPI(96.0);
				pCompound->SetPrinting(FALSE);
			
				pCompound->RegenerateNode(NULL, FALSE);
			}
		}
		
		pItem = (NodeListItem *)CompoundNodeList.GetNext(pItem);
	}	

	CompoundNodeList.DeleteAll();

	NodeGroup::KillAllBlendBecomeAConsLists (FALSE, TRUE);
	NodeGroup::SetIsForPrinting (FALSE);

	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("Exiting print loop\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
	TRACEUSER( "ChrisS", _T("------------------------------------------------------------------------\n"));
}

#endif //webster

/*
void ScreenView::OnFilePrint()
{
	// Set Current states...
	Document* KernelDoc = GetDocument()->GetKernelDoc();
	ENSURE(KernelDoc != NULL, "Oh no!!");
	KernelDoc->SetCurrent();
	pDocView->SetCurrent();

	TRACEUSER( "Tim", _T("OnFilePrint()\n"));

	// get default print info
	CCPrintInfo *pPrintInfo;
	TRY
	{
		pPrintInfo = new CCPrintInfo(KernelDoc,this);
	}
	CATCH(CException, e)
	{
		// Out of memory - report error and exit.
		Error::SetError(_R(IDS_OUT_OF_MEMORY));
		InformError();
		return;
	}
	END_CATCH

	ASSERT(pPrintInfo->m_pPD != NULL);    // must be set
	BOOL StartedPrinting = FALSE;

	// Make sure the user can't get 2 print dialog boxes up
	PrintMonitor::SetPrintingActive(TRUE);

	if (!OnPreparePrinting(pPrintInfo))
	{
		TRACEUSER( "Tim", _T("OnPreparePrinting() returned FALSE\n"));
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	CString strTemp;

	ERROR3IF(pPrintInfo->m_pPD->m_pd.hDC == NULL, "NULL DC when trying to print.");
	// must be set (did you remember to call DoPreparePrinting?)

	// Let's see if we can start printing...
	if ((pPrintInfo->m_pPD->m_pd.hDC == NULL) || 
		!PrintMonitor::StartPrintJob(pPrintInfo->m_pPD->m_pd.hDC))
	{
		TRACEUSER( "Tim", _T("StartPrintJob() screwed up\n"));

		// No - something went wrong.
		if (pPrintInfo->m_pPD->m_pd.hDC == NULL)
		{
			// No DC - set a generic "we can't print" error.
			Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
		}

		// Tell user and exit.
		InformError();
		delete pPrintInfo;
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	// Setup the printing DC
	CDC dcPrint;

	// Attach printer dc
	dcPrint.Attach(pPrintInfo->m_pPD->m_pd.hDC);
	dcPrint.m_bPrinting = TRUE;
	OnBeginPrinting(&dcPrint, pPrintInfo);

	// Get access to our print control object
	PrintControl *pPrintControl = pPrintView->GetPrintControl();

	// disable main window while printing & init printing status dialog
	AfxGetMainWnd()->EnableWindow(FALSE);

	// set up document info and start the document printing process
	DOCINFO* pDocInfo = NULL;

	// start document printing process
	if  ((pPrintInfo->SetAbortProc(&dcPrint) < 0) ||
		 !pPrintInfo->SetUpDocInfo(&pDocInfo) 	  ||		// SetUpDocInfo() allocs a DOCINFO for pDocInfo
		 pDocInfo == NULL 						  ||
		 (dcPrint.StartDoc(pDocInfo) == SP_ERROR))
	{
		TRACEUSER( "Tim", _T("Unable to start document\n"));

		// enable main window before proceeding
		AfxGetMainWnd()->EnableWindow(TRUE);

		// cleanup and show error message
		OnEndPrinting(&dcPrint, pPrintInfo);
		delete pPrintInfo;
		pPrintInfo = NULL;
		PrintMonitor::EndPrintJob();

		// Lose our PrintView
		delete pPrintView;
		pPrintView = NULL;

		// Delete a the docinfo object
		if (pDocInfo != NULL)
		{
			delete pDocInfo;
			pDocInfo = NULL;
		}

		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
		AfxMessageBox(_R(AFX_IDP_FAILED_TO_START_PRINT));
		PrintMonitor::SetPrintingActive(FALSE);
		return;
	}

	// begin page printing loop
	BOOL bError = TRUE;

	// Set up the print layout system.
	if (!pPrintInfo->StartPrinting())
	{
		TRACEUSER( "Tim", _T("StartPrinting() failed\n"));
		// Error in starting printing.
		Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
		goto ExitPrint;
	}

	// Must remember to tell print info object when we are done.
	StartedPrinting = TRUE;

	// Work out if we should do our own PostScript
	BOOL DoPostScript;
	if ((pPrintControl->GetPrintMethod() == PRINTMETHOD_NORMAL) &&
		(CCDC::GetType(&dcPrint, TRUE) == RENDERTYPE_PRINTER_PS))
	{
		// Not printing as a bitmap and it is a PostScript printer so mark it as such.
		DoPostScript = TRUE;
	}
	else
	{
		// Either it's not a PostScript printer or we are printing as a bitmap,
		// so don't do weird PostScript stuff
		DoPostScript = FALSE;
	}

	// If we are printing as bitmap, then set the "Printing blah.art" message in the dialog.
	// (This is done by RenderSimpleView() when we are printing normally).
	if ((pPrintControl->GetPrintMethod() != PRINTMETHOD_NORMAL) && (pPrintInfo != NULL))
		// We going to print the document now
		pPrintInfo->SetPrinting();

	BOOL ReprintingPaper;
	ReprintingPaper = FALSE;

	// Do we support the BANDINFO Escape?
	BOOL BandInfoSupported;
	BandInfoSupported = FALSE;
	WORD wEscape;
	wEscape = BANDINFO;

	if (!DoPostScript && 
		PrintMonitor::PrintWithDriverBands &&
		dcPrint.Escape(QUERYESCSUPPORT, sizeof(WORD), (LPCSTR) &wEscape, NULL) > 0)
	{
		// The BANDINFO Escape is supported
		BandInfoSupported = TRUE;
	}


	// Loop around for each physical piece of paper we have to print.
	while (pPrintInfo->GetNextPaper())
	{
		// If this is the first time we've seen this page, set it up
		if (!ReprintingPaper)
		{
			TRACEUSER( "Tim", _T("Starting physical page\n"));
			
			OnPrepareDC(&dcPrint, pPrintInfo);

			// check for end of print
			if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
			{
				TRACEUSER( "Tim", _T("Print abort 1\n"));
				goto ExitPrint;
			}

			// attempt to start the current page
			if (dcPrint.StartPage() < 0)
			{
				TRACEUSER( "Tim", _T("Unable to StartPage()\n"));
				Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
				goto ExitPrint;
			}

			// Is it a PostScript printer?
			if (DoPostScript)
			{
				// Yes, so before we start the page, we then send our 
				// PostScript procset to the device.
				if (!PrintPSRenderRegion::InitPSDevice(&dcPrint, pPrintView))
				{
					TRACEUSER( "Tim", _T("Unable to Init PS device\n"));
					Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
					goto ExitPrint;
				}
			}

			// Only use driver banding if the preference says so, and never band
			// to PS devices (yes, the Win95 PS driver supports banding, except
			// not really and it just screws up).
			if (PrintMonitor::PrintWithDriverBands && !DoPostScript)
			{
				// Do the driver banding
				dcPrint.Escape(NEXTBAND, 0, NULL, (LPSTR) &DriverBand);
#if 0
				if (BandInfoSupported)
				{
					BANDINFOSTRUCT BandInfo;
					dcPrint.Escape(BANDINFO, sizeof(, NULL, (LPSTR) &DriverBand);
				}
#endif
				TRACEUSER( "Tim", _T("INITIALBAND: (%d,%d), (%d, %d)\n"),
						 DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
			}
		}

		TRACEUSER( "Tim", _T("Starting PrintPaper() loop...\n"));

		TRACEUSER( "Tim", _T("Calling PrintPaper()...\n"));

		// Print this piece of paper.
		if (!PrintPaper(&dcPrint, pPrintInfo, pPrintControl))
		{
			// Something went wrong - quit and abort print job.
			TRACEUSER( "Tim", _T("PrintPaper() failed\n"));
			Error::SetError(_R(IDE_PRINT_ERROR_SYSTEM));
			goto ExitPrint;
		}

		// Only use driver banding if the preference says so, and never band
		// to PS devices.
		if (PrintMonitor::PrintWithDriverBands && !DoPostScript)
		{
			// Finished all the patches - see if printer driver has any bands left?
			// NB we kludge this as some drivers keep returning the same band...fabby eh?
			TestBand = DriverBand;
			dcPrint.Escape(NEXTBAND, 0, NULL, (LPSTR) &TestBand);

			TRACEUSER( "Tim", _T("NEXTBAND: (%d,%d), (%d, %d)\n"),
					  TestBand.left, TestBand.top, TestBand.right, TestBand.bottom);

			if (0) //EqualRect(&TestBand, &DriverBand))
			{
				// Same band back again - no more bands!
				SetRectEmpty(&DriverBand);
				TRACEUSER( "Tim", _T("Driver returned the same band again\n"));
			}
			else
			{
				// Use this band
				DriverBand = TestBand;
				TRACEUSER( "Tim", _T("Driver returned valid band: (%d,%d), (%d, %d)\n"),
						  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
			}

			// If it does, then we need to restart the printing of this physical
			// piece of paper...
			if (!IsRectEmpty(&DriverBand))
			{
				TRACEUSER( "Tim", _T("Forcing a reprint of paper [for band: (%d,%d), (%d, %d)]\n"),
						  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);
				pPrintControl->ReprintPaper();
				ReprintingPaper = TRUE;
			}
			else
			{
				// Not going to reprint this page
				TRACEUSER( "Tim", _T("End of bands for this page\n"));
				ReprintingPaper = FALSE;
			}
		}

		if (!ReprintingPaper)
		{
			TRACEUSER( "Tim", _T("Ending physical page\n"));
			
			// End of page clean up.

			// Note that we don't check for an error from EndPage() because due to
			// a bug (in Windows/GDI) it returns -1 when using banding.
			// The MS workaround is to not check the return value!
			dcPrint.EndPage();

			if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
			{
				TRACEUSER( "Tim", _T("Print abort 2\n"));
				goto ExitPrint;
			}
		}
	}

	bError = FALSE;

ExitPrint:
	// cleanup document printing process

	PrintMonitor::EndPrintJob();
	PrintMonitor::SetPrintingActive(FALSE);

	if (!bError)
		dcPrint.EndDoc();
	else
	{
		TRACEUSER( "Tim", _T("ExitPrint: an error occured()\n"));
		// An error occured - abort the job.
		dcPrint.AbortDoc();

		// If the user did not cancel the job, report what went wrong.
		if (!pPrintInfo->Abort())
			InformError();
	}

	AfxGetMainWnd()->EnableWindow();    // enable main window

	OnEndPrinting(&dcPrint, pPrintInfo);    // clean up after printing

	// Clean up kernel printing code
	if (StartedPrinting)
		pPrintInfo->EndPrinting();

	dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor

	delete pPrintInfo;
	pPrintInfo = NULL;

	// Lose our PrintView (if it exists) - not needed anymore.
	delete pPrintView;
	pPrintView = NULL;

	// Delete a the docinfo object
	if (pDocInfo != NULL)
	{
		delete pDocInfo;
		pDocInfo = NULL;
	}

	// If we need to redraw everything then do it.
	if (PrintMonitor::FullRedrawNeeded)
	{
		// iterate through all the documents and get them to redraw themselves
		Document* pDoc = (Document*) Camelot.Documents.GetHead();

		while (pDoc!=NULL)
		{
			// Get it to redraw itself
			pDoc->ForceRedraw();

			// get the next document in the list
			pDoc = (Document*) Camelot.Documents.GetNext(pDoc);
		}
	}

	// mark the PrintMonitor as not needing a redraw
	PrintMonitor::WantFullRedraw(FALSE);
}
*/



// Size of bands to use when printing as bitmap.
#define BAND_HEIGHT (128)

/********************************************************************************************

>	BOOL ScreenCamView::PrintPaper(CDC *pPrintDC, 
								   CCPrintInfo *pPrintInfo,
								   PrintControl *pPrintControl)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/11/95
	Inputs:		pPrintDC - the (printer) device context to print to.
				pPrintInfo - the object to use for layout control.
	Returns:	TRUE if the paper was printed ok;
				FALSE if not.
	Purpose:	Prints all the ink onto a physical piece of paper.  This includes coping
				with patches (multiple copies on one piece of paper, and banding bitmap
				printing to improve performance.
	Errors:		pPrintDC has a bad DC handle => ERROR3
				Problem with setting up clipping region => ERROR2
	SeeAlso:	ScreenCamView::OnFilePrint; ScreenCamView::GetPrintClipRect

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
BOOL ScreenView::PrintPaper(CDC *pPrintDC, 
							CCPrintInfo *pPrintInfo, 
							PrintControl *pPrintControl)
{
	TRACEUSER( "Tim", _T("PrintPaper() started\n"));

	TRACEUSER( "Tim", _T("PrintPaper() printing band=(%d,%d),(%d,%d)\n"),
			  DriverBand.left, DriverBand.top, DriverBand.right, DriverBand.bottom);


	// Page successfully started, so now render the page by doing each 'patch'
	// in turn.  Note that we store the patch information in the print view,
	// which the view will use when setting up to render
	// (see PrintView::ConstructRenderingMatrix())
	while (pPrintInfo->GetNextPatch(&pPrintView->PatchInfo))
	{
		// Tell the view what scale this is.
		pPrintView->SetViewScale(pPrintView->PatchInfo.Scale / FIXED16(100));

		// Set up drawing rect to entire page (in logical coordinates)
		pPrintInfo->m_rectDraw.SetRect(0, 0, 
									   pPrintDC->GetDeviceCaps(HORZRES),
									   pPrintDC->GetDeviceCaps(VERTRES));
		pPrintDC->DPtoLP(&pPrintInfo->m_rectDraw);

		// Let's band this to make performance acceptable.

		// Bodge banding - many printer drivers don't do banding so performance
		// is a dog (i.e. not enough memory to print on my 32Mb machine!)
		const INT32 BandHeight = BAND_HEIGHT;
		INT32 BottomOfPage = pPrintInfo->m_rectDraw.bottom;
		const INT32 PageHeight = pPrintInfo->m_rectDraw.bottom - pPrintInfo->m_rectDraw.top;

		// Find print control object for this document, to see if we are
		// printing via bitmap, and so if we need to band the output.
		PrintMethodType PrintType;
		PrintType = pPrintControl->GetPrintMethod();

		// Work out whether or not to use banding for printing.
		// (i.e. are we printing with a bitmap?)
		if ((PrintType == PRINTMETHOD_AABITMAP) ||
			(PrintType == PRINTMETHOD_BITMAP))
		{
			// Banding is required - set up first band.
			pPrintInfo->SetSliderSubRangeMax(pPrintInfo->m_rectDraw.Height() / BAND_HEIGHT);
			pPrintInfo->m_rectDraw.bottom = pPrintInfo->m_rectDraw.top + BandHeight;
		}

		INT32 CurrentBand = 0;

		TRACEUSER( "Tim", _T("Rectangle: (%d,%d), (%d,%d) BottomOfPage=%d\n"),
				 pPrintInfo->m_rectDraw.left,pPrintInfo->m_rectDraw.top,
				 pPrintInfo->m_rectDraw.right,pPrintInfo->m_rectDraw.bottom,
				 BottomOfPage);

		while (pPrintInfo->m_rectDraw.bottom <= BottomOfPage)
	    {
			// Update slider
			if ((PrintType == PRINTMETHOD_AABITMAP) ||
				(PrintType == PRINTMETHOD_BITMAP))
			{
				// Doing our own banding, so update progress slider.
				pPrintInfo->SetSliderSubRangePos(CurrentBand);
			}

			//
			// Strange stuff to work out what area to print in this band.
			//
			WinRect OILClip = GetPrintClipRect(pPrintDC, pPrintInfo);

			// Check for no intersection.
			if (!OILClip.IsRectEmpty())
			{
				// Put this clip region into print info structure.
				pPrintInfo->m_rectDraw = OILClip;

				// Make this the clip region
				HRGN hBandRgn;
				hBandRgn = ::CreateRectRgnIndirect(&pPrintInfo->m_rectDraw);
				ERROR2IF(hBandRgn == NULL, FALSE, "CreateRectRgnIndirect() failed!");
				ERROR3IF(pPrintDC->GetSafeHdc() == NULL, "Bad HDC in dcPrint!");
				ERROR2IF(::SelectClipRgn(pPrintDC->GetSafeHdc(), hBandRgn) == 0,
						 FALSE, "SelectClipRgn() failed!");

				// Delete the region
				ERROR2IF(!::DeleteObject(hBandRgn), FALSE, "DeleteObject() failed!");

				// Print the band.
				pPrintView->SetCurrent();
				OnPrint(pPrintDC, pPrintInfo);

				// Restore clip region.
				ERROR2IF(::SelectClipRgn(pPrintDC->GetSafeHdc(), NULL) == 0,
						 FALSE, "SelectClipRgn(NULL) failed!");

				if (!pPrintInfo->m_bContinuePrinting || pPrintInfo->Abort())
					// Printing has finished.
					return FALSE;
			}

			// Is this the last band?
			if (pPrintInfo->m_rectDraw.bottom < BottomOfPage)
			{
				// No - get the next band.
				pPrintInfo->m_rectDraw.top = pPrintInfo->m_rectDraw.bottom;
				pPrintInfo->m_rectDraw.bottom = pPrintInfo->m_rectDraw.top + BandHeight;

				// Limit to bottom of page if necessary.
				if (pPrintInfo->m_rectDraw.bottom > BottomOfPage)
					pPrintInfo->m_rectDraw.bottom = BottomOfPage;
			}
			else
			{
				// No bands left - so we're done here.
				TRACEUSER( "Tim", _T("No bespoke banding\n"));
				break;
			}

			CurrentBand++;
		}
	}

	TRACEUSER( "Tim", _T("PrintPaper() finished\n"));

	// All worked ok
	return TRUE;
}
#endif //webster

/********************************************************************************************

>	WinRect ScreenView::GetPrintClipRect(CDC *pPrintDC, CCPrintInfo *pPrintInfo)

	Author:		Tim_Browse (Xara Group Ltd) <camelotdev@xara.com>
	Created:	05/11/95
	Inputs:		pPrintDC - the device context we are printing to - we need this because
						   otherwise the PrintView object can't compute the rendering
						   matrix, which we need in order to find out the position of the
						   patch on the DC.
				pPrintInfo - the print info object, which specifies the window onto the
							 page/DC that we can print into.
	Returns:	The intersection of the area of the current patch (as held in 
				ScreenCamView::pPrintView->PatchInfo) and the printable area of the DC,
				in WinCoords.
	Purpose:	Works out which area of the paper to print on, given the current patch
				(as held in the PrintView object), and the printing area specified by
				the CCPrintInfo object.
	SeeAlso:	ScreenCamView::PrintPaper; PrintView::ConstructRenderingMatrix

********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
WinRect ScreenView::GetPrintClipRect(CDC *pPrintDC, CCPrintInfo *pPrintInfo)
{
	// We need to attach the PrintView while we are doing this so it knows how big
	// the pixels are and so on.
	pPrintView->AttachToDC(pPrintDC);

	// Get the rendering matrix for this patch
	Spread *pSpread = Document::GetSelectedSpread(); // Should be PatchInfo.pSpread
	Matrix TheMatrix = pPrintView->ConstructRenderingMatrix(pSpread);

	// Use matrix to transform clip rect of patch to Oil coords.
	DocRect ClipRect = pPrintView->PatchInfo.GetClipRect(FALSE,FALSE);

	// turn this clip rect into an OilRect
	OilRect PatchClipRect(OilCoord(ClipRect.lo.x, ClipRect.lo.y),
					      OilCoord(ClipRect.hi.x, ClipRect.hi.y));

	TheMatrix.transform(&PatchClipRect.lo);
	TheMatrix.transform(&PatchClipRect.hi);

	// Rendering transform may involve a rotation so correct if necesary.
	pPrintView->CorrectRotatedRectangle((Rect *) &PatchClipRect);

	// We need to inflate the clipping rectangle as this is used to determin the
	// bitmap size when rendering to AABITMAP output (and BITMAP).
	// Ok, There is a problem here.... We need to set the clip rectangle to cover
	// the crop area too. Otherwise, when rendering text as text, GDI will bin the
	// crop mark text we render. We do this after the transform to avoid the transform
	// scaling this unscalable value.
	pPrintView->PatchInfo.InflateRectBy(&PatchClipRect, TRUE, TRUE);

	// Convert the OIL cliprect to logical GDI units
	WinRect WinClipRect;
	WinClipRect = PatchClipRect.ToWin(pPrintView);

	// Find intersection with clip rect.
	WinRect OILClip;
	OILClip.IntersectRect(&pPrintInfo->m_rectDraw, WinClipRect);

	// Finished messing with the PrintView for the mo so we can detach the DC.
	// It is eventually re-attached (in ScreenCamView::OnDraw()) when we call OnPrint().
	pPrintView->AttachToDC(NULL);

	return OILClip;
}
#endif //webster

/********************************************************************************************

>	void ScreenView::EnumeratePagePatches(CCPrintInfo *pPrintInfo)

	Author:		Mike_Kenny (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14/09/96
	Inputs:		pPrintInfo - the print info object, which specifies the window onto the
							 page/DC that we can print into.
	Returns:	
	Purpose:	In order to locate crop marks / registration marks etc correctly, we need
				to work out the bounds of all patches printed on a single sheet of paper.
				This routine does just that.
				
********************************************************************************************/
//	WEBSTER-ranbirr-12/11/96
#ifndef WEBSTER
void ScreenView::EnumeratePagePatches(CCPrintInfo *pPrintInfo)
{
#ifndef EXCLUDE_FROM_RALPH
#ifndef STANDALONE

	DocRect Bounds;
	DocRect cliprect;
	INT32 bleed;
	BOOL emuldwn;

	// get a pointer to the print marks manager
	PrintMarksMan *pMarksMan = GetApplication()->GetMarksManager();
	if (pMarksMan==NULL)
		return;

	// Destroy what we may already have from a previous piece of paper
	pMarksMan->ResetPageRects();

	PrintPatchInfo* pPatchInfo = &pPrintView->PatchInfo;
	if (pPrintInfo->GetNextPaper())
	{
		// scan through each patch on the first paper region
		while (pPrintInfo->GetNextPatch(pPatchInfo))
		{
			// Get raw patch clip rectangle
			cliprect = pPatchInfo->GetClipRect(FALSE,FALSE);
			bleed = pPatchInfo->GetBleed();
			emuldwn = pPatchInfo->EmulsionDown;
			// dont inflate by the bleed, this is passed in and used by the print marks manager
			//cliprect.Inflate(pPatchInfo->Bleed);
			// Build a Spread=>OS conversion matrix for this patch
			Matrix TheMatrix = pPrintView->ConstructRenderingMatrix(pPatchInfo->pSpread);
			// transform the patch
			TheMatrix.transform(&cliprect.lo);
			TheMatrix.transform(&cliprect.hi);
			// Rendering matrix may involve a rotation when printing so correct the rectangle
			// for this if necessary.
			pPrintView->CorrectRotatedRectangle((Rect *)&cliprect);
			// tell the marks manager about these bounds.
			pMarksMan->AddPageRect(cliprect);
		}
	}
	
	// Get the marks manager to update its imagesetting rect
	pMarksMan->SetImagesettingRect();
	pMarksMan->SetBleed(bleed);
	pMarksMan->SetEmulsionDown(emuldwn);
	
	// reprint this piece of paper so we dont upset the print system
	pPrintInfo->ReprintPaper();

#endif
#endif
}

#endif //webster
