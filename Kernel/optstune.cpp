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

// Implementation of the Tuneups tab of the options dialog box

/*

*/

#include "camtypes.h"
//WEBSTER-ranbirr-13/11/96
#ifndef WEBSTER
#include "appprefs.h"
//#include "prefsdlg.h"	// dialog/gadget ids
//#include "optsres.h"	// error strings
#include "optstune.h"
#include "app.h"		// Camelot object
#include "units.h"		// units
#include "convert.h"	// BytesToString and StringToBytes
#include "thumb.h"
#include "tunemem.h"
//#include "resource.h"	// Needs _R(IDS_OK) for InformWarning
//#include "justin2.h"
#include "ophist.h"

CC_IMPLEMENT_DYNAMIC(TuneTab, OptionsTabs)   

// This is not compulsory, but you may as well put it in so that the correct version
// of your file can be registered in the .exe
DECLARE_SOURCE("$Revision$");

/* The range of the values allowed in the TuneUps Temporary Workspace field */
#define MAX_TUNEUPS_MEMORY	0x7fff*1024
#define MIN_TUNEUPS_MEMORY  0
			  
/********************************************************************************************

>	TuneTab::TuneTab() 


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-	
	Outputs:	-
	Returns:	-
	Purpose:	TuneTab constructor. Creates a non-undoable operation
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

TuneTab::TuneTab()
{   
	// does nothing at present
}   	     

/********************************************************************************************

>	TuneTab::~TuneTab()


	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	TuneTab destructor
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
TuneTab::~TuneTab()
{   
}        

/********************************************************************************************

>	BOOL TuneTab::Init()


	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	True if initialised ok, False if failed.
	Purpose:	TuneTab initialisation routine
	Errors:		-
	SeeAlso:	-

********************************************************************************************/
																				
BOOL TuneTab::Init()
{   
	CurrentTempMemValue=0;
	return TRUE;
}        

/******************************************************************************************

>	CDlgResID TuneTab::GetPageID()()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	The dialog ID of this tab section.	  
	Purpose:	Allows the options dialog code to determine the dialog ID of this section.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

CDlgResID TuneTab::GetPageID()
{
	return _R(IDD_OPTSTAB_TUNE);
}

/******************************************************************************************

>	BOOL TuneTab::IsDocumentOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	Sound
	Returns:	Returns True if this tab is a document option.	  
	Purpose:	Allows the document options dialog code to determine if this tab belongs
				to the group of document options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL TuneTab::IsDocumentOption()
{
	return FALSE;	// This tab is not a document option 
}

/******************************************************************************************

>	BOOL TuneTab::IsProgramOption()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	Returns True if this tab is a program option.	  
	Purpose:	Allows the program options dialog code to determine if this tab belongs
				to the group of program options.
	Errors:		-
	SeeAlso:	IsProgramOption();

******************************************************************************************/

BOOL TuneTab::IsProgramOption()
{
	return TRUE;	// This tab is a program option 
}

/******************************************************************************************

>	BOOL TuneTab::CommitRenderingSection()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Tuneups section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL TuneTab::CommitRenderingSection()
{
TRACEUSER( "Neville", _T("TuneTab::CommitRenderingSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::CommitRenderingSection called with no dialog pointer");

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=FALSE;			// Flag for validity of value
//	BOOL SetOk=TRUE;			// Preference value set ok
//	BOOL ReadOk=TRUE;			// Value read ok from gadget

	// Now the maximum temporary memory available to Gdraw for rendering purposes
	UINT32 TempWorkMem = 0;		// Working memory value in bytes

	// Now read the state of the use maximum memory switch, if on then set TempWorkMemory to 0
	BOOL UseMaximumMemory = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_USEMAXMEMORY), 0, 1, 0, &Valid);

	// If we actually got a value out of it, set the flag
	if (Valid)
		TunedMemory::SetMemSizeInfinte(UseMaximumMemory);


	if (Valid && !UseMaximumMemory)
	{
		// Use the specified maximum memory value so get the string from the editable field
		TempWorkMem = pPrefsDlg->GetMemoryGadgetValue(_R(IDC_OPTS_TEMPWORKMEM),
													  MIN_TUNEUPS_MEMORY, MAX_TUNEUPS_MEMORY,
													  _R(IDE_OPTS_BADTEMPWORKMEM), &Valid);	

		// If it is valid, then set the Limited memory manager up
		if (Valid)
		{
	   		// Set the actual GDraw memory values
			INT32 ActualTempWorkMem = TunedMemory::SetLimitedMemSize((INT32)TempWorkMem);

			// If returned value is not the same as the value we gave then there is a
			// problem wiht it, we should warn the user and show them this new value as
			// a good one ot be using.
			if (ActualTempWorkMem != (INT32)TempWorkMem)
			{
				// Put this new value in as a hint to the user.
				pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_TEMPWORKMEM), ActualTempWorkMem);
				// Scold the user and tell them about the value we have placed there.
				String_256 WarnMsg(_R(IDE_OPTS_BADTEMPWORKMEM));	// Invalid value
				WarnMsg += String_32(_R(IDE_OPTS_MINIMUMALLOWED));	// The minimum allowed is 
				String_256 MinAmount;  							// Make a string of the min value
				Convert::BytesToString(&MinAmount, ActualTempWorkMem);
				WarnMsg += MinAmount;
				Error::SetError(0, WarnMsg, 0);
				InformWarning(0, _R(IDS_OK));
				// Make sure we stop the box being oked.
				Valid = FALSE;
			}
		}
	}

	// Something is not as it should be
	if (!Valid)
		return FALSE;

	// return happy
	return TRUE;
}
			   
/******************************************************************************************

>	BOOL UndoTab::CommitUndoSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Undo section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL TuneTab::CommitUndoSection()
{
TRACEUSER( "Neville", _T("TuneTab::CommitUndoSection\n"));
	ERROR3IF(pPrefsDlg == NULL, "TuneTab::CommitUndoSection called with no dialog pointer");

	// Ok has been pressed so take the values from this section of the dialog box
	BOOL Valid=TRUE;		// Flag for validity of value
//	BOOL State=FALSE;		// Flag for state of button/switch
	BOOL SetOk=TRUE;		// Preference value set ok

	// Section = Undo settings

	// Get the currently selected document
	Document* pDocument = Document::GetSelected();
	if (pDocument != NULL)
	{
		// Getthe current undo size
		UINT32 CurrentSize = pDocument->GetOpHistory().GetSize();
		UINT32 CurrentMaxSize = pDocument->GetOpHistory().GetMaxSize();

		// Now read the state of the use maximum memory switch, if on then set TempWorkMemory to 0
		BOOL Unlimited = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_UNLIMITEDUNDO), 0, 1, 0, &Valid);

		UINT32 NewMaxUndoSize;
		if (Unlimited)
		{
			// Use the maximum possible value
			NewMaxUndoSize = UINT32_MAX;
		}
		else
		{
			// Read the value from the editable field limiting the values in the process
			NewMaxUndoSize = pPrefsDlg->GetMemoryGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE),
															 1*1024, UINT32_MAX-1,
													 		 _R(IDE_OPTS_INVALIDUNDOSIZE), &Valid);
			if (!Valid)
				return FALSE; 			// Undo size incorrect, user has been warned already
		}
TRACEUSER( "Neville", _T("Current undo size =%d\n"),CurrentSize);
TRACEUSER( "Neville", _T("Current maximum undo size =%d\n"),CurrentMaxSize);
TRACEUSER( "Neville", _T("New maximum undo size =%d\n"),NewMaxUndoSize);
		
		// If a new size has been entered then try to set this as the new maximum
		if (NewMaxUndoSize != CurrentMaxSize)
		{
			// If the resize means that data will be lost then warn the user and ask for confirmation
			// otherwise just set the new limit.
			BOOL OkToResize = TRUE;
			if (NewMaxUndoSize < CurrentSize)
			{
				// Ask user to confirm loss of undo data
				// Warn the user that there is no selection = large export
				ErrorInfo Info;
				Info.ErrorMsg = _R(IDT_OPTS_LOSSOFUNDO);
				Info.Button[0] = _R(IDB_UNDOQUERY_RESIZE);
				Info.Button[1] = _R(IDB_UNDOQUERY_DONTRESIZE);
				if ((ResourceID)AskQuestion(&Info) == _R(IDB_UNDOQUERY_DONTRESIZE))
				{
					// If the user has cancelled just display the current value again
					OkToResize = FALSE;
				}
			}
		
			// If everything is ok then try the resize
			if (OkToResize)
			{
				SetOk = pDocument->GetOpHistory().SetNewMaxSize(NewMaxUndoSize);
				if (!SetOk)
				{
					// Out of ID's at present so use wrong error message 
					InformError(_R(IDE_OPTS_BADUNDORESIZE));
				
					// Force the update to the old max size value
					OkToResize = FALSE;
				}
				else
				{
					// Mark the document as modified as we have resized it ok
					pDocument->SetModified(TRUE);					
				}
			}

			// If for any reason OkToResize has been set ot FALSE then update the undo size
			// field and exit FALSE
			if (!OkToResize)
			{
				// Restore the MaxSize in dialog field
			  	//String_256	String;
				// Get current size
				UINT32 MaxUndoSize = pDocument->GetOpHistory().GetMaxSize();
				// Show that value on the dialog box as a memory size
				pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE), MaxUndoSize);

				return FALSE;
			}
		}
	}
	//else
	//	ERROR2(FALSE,_R(IDE_OPTS_SETPREF_TUNE));

	return TRUE;
}

/******************************************************************************************

>	BOOL TuneTab::CommitSection()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/1/95
	Inputs:		-
	Outputs:	-
	Returns:	True if values in dialog box ok, False otherwise.	  
	Purpose:	Takes the values in the Tuneups section tab of the options dialog box and
				sets the associated	preference values accordingly
				Called when ok is pressed on the dialog box.
	Errors:		-
	SeeAlso:	-

******************************************************************************************/

BOOL TuneTab::CommitSection()
{
TRACEUSER( "Neville", _T("TuneTab::CommitSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::CommitSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_TUNE));	// The tune up page identifier
	if (!ok)
		return TRUE;			// Talk to page failed to return now

	ok = CommitRenderingSection();
	if (ok != TRUE)
		return ok;				// Something went wrong so return this to caller.
	
#ifndef EXCLUDE_GALS
	// Section = Gallery cache size

	BOOL Valid = TRUE; 
	INT32 CacheSize = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_CACHESIZE), 1, 99, _R(IDE_OPTS_INVALIDCACHESIZE), &Valid);
	if (Valid)
	{
TRACEUSER( "Neville", _T("commit gallery cache size='%d'\n"), CacheSize);
		BOOL SetOk = Camelot.SetPrefValue(TEXT("Libraries"), TEXT("Thumbnails"), &CacheSize);
		ERROR2IF(!SetOk,2,_R(IDE_OPTS_SETPREF_TUNE));
		// Tell the galleries and in particular the thumbnail one about the change.		
		SGThumbs::ResizeAllThumbCaches(CacheSize);
	}
	else
		return FALSE; 			// Cache size incorrect, user has been warned already
#endif

	ok = CommitUndoSection();
	if (ok != TRUE)
		return ok;				// Something went wrong so return this to caller.

	// ---------------------------------------------------------------------------------
	// Cacheing preferences
	{
		BOOL SetOk = FALSE;
		INT32 State = 0;
		BOOL Valid = FALSE;

		State = pPrefsDlg->GetLongGadgetValue(_R(IDC_CHECK_CACHE_ENABLED), 0, 1, 0, &Valid);
		if (Valid)
			SetOk = Camelot.SetPrefValue(TEXT("Rendering"), TEXT("Cacheing"), &State);

	//	State = pPrefsDlg->GetLongGadgetValue(_R(IDC_EDIT_CACHE_SIZE), 1, 100, 0, &Valid);
		String_256 strCachePercent = pPrefsDlg->GetStringGadgetValue(_R(IDC_EDIT_CACHE_SIZE), &Valid);
		double dValue = 0;
		INT32 Pos = 0;
		INT32 iValue = 0;
		Convert::ReadNumber(strCachePercent, &Pos, &dValue);	// Allow trailing chars
		if (Valid && dValue>=1 && dValue<=100)
			iValue = (INT32)dValue;
			SetOk = Camelot.SetPrefValue(TEXT("Cache"), TEXT("CacheRAMPercent"), &iValue);

		// Reset the cache size...
		CBitmapCache* pCache = Camelot.GetBitmapCache();
		if (pCache)
		{
			pCache->SetMaximumDataSize(pCache->CalcRecommendedMaximumDataSize());
			// If the new size if smaller than the old size, things will get
			// thrown out of the cache the next time any attempt is made to
			// store anything...
		}
	}

	return TRUE;
}

/********************************************************************************************

>	BOOL TuneTab::GreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has closed all documents 
	Errors:		-
	SeeAlso:	TuneTab::UngreySection;

********************************************************************************************/

BOOL TuneTab::GreySection()
{
TRACEUSER( "Neville", _T("GreySection in TuneTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::GreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_TUNE));	// The TuneTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_UNDO_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_UNDOINFO), DocumentName);

	// Only update if we are not already grey 
	if (GreyStatus == TRUE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(FALSE);
	
	GreyStatus = TRUE;

	return ok;
} 

/********************************************************************************************

>	BOOL TuneTab::UngreySection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called when the user has selected a new document 
	Errors:		-
	SeeAlso:	TuneTab::UngreySection;

********************************************************************************************/

BOOL TuneTab::UngreySection()
{
TRACEUSER( "Neville", _T("UngreySection in TuneTab section\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::UngreySection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_TUNE));	// The TuneTab identifier
	if (!ok)
		return TRUE;	// Talk to page failed to return now

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_UNDO_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_UNDOINFO), DocumentName);

	// Only update if we are not already ungrey 
	if (GreyStatus == FALSE)
		return TRUE;

	// Call our central greying/ungreying function
	ok = ChangeControlStatus(TRUE);
	
	GreyStatus = FALSE;

	return ok;
}

/********************************************************************************************

>	BOOL TuneTab::ChangeControlStatus(const BOOL Status)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Called to grey/ungrey all controls on this tab. 
	Errors:		-
	SeeAlso:	TuneTab::GreySection;	TuneTab::UngreySection;

********************************************************************************************/

BOOL TuneTab::ChangeControlStatus(const BOOL Status)
{
	// Grey/ungrey the info section
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_TUNEUPSINFO), Status);

	// GDraw rendering memory section
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_TUNEGROUP), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MEMREQD), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_FULLPAGEMEM), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_USEMAXMEMORY), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_USELIMITMEM), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MEMLIMITTXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_TEMPWORKMEM), Status);

//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_GALLERYGRP), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_CACHETXT), Status);
//	pPrefsDlg->EnableGadget(_R(IDC_OPTS_CACHESIZE), Status);

	// Undo section
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOINFO), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOSIZETXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOSIZE), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNLIMITEDUNDO), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_LIMITEDUNDO), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOLIMITTXT), Status);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), Status);

	return TRUE;
}			

/********************************************************************************************

>	BOOL TuneTab::UpdateSection(String_256 *DocumentName)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/2/95
	Inputs:		Name of document
	Outputs:	-
	Returns:	-
	Purpose:	Called when we have switched to a new document and need to update all the 
				controls on this tab. Should only init the tab if the page is present.  
				The document name allows the info field on the tab to be filled in correctly.
	Errors:		-
	SeeAlso:	TuneTab::GreySection; TuneTab::UngreySection; TuneTab::InitSection;

********************************************************************************************/

BOOL TuneTab::UpdateSection(String_256 *DocumentName)
{
TRACEUSER( "Neville", _T("TuneTab::UpdateSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::UpdateSection called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_TUNE));	// The TuneTab identifier
	if (!ok)
		return TRUE;		// page not present

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocName(_R(IDT_OPTS_UNDO_INFO)); 
	DocName +=	*DocumentName;
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_UNDOINFO), DocName);

	// Remove any lists that we have created.
	//pPrefsDlg->DeleteAllValues(_R(IDC_OPTS_SIZELIST));

	// Call the normal init section code but only the undo is document specific
	return InitUndoSection();
}

/********************************************************************************************

>	BOOL TuneTab::HandleMsg(DialogMsg* Msg)

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Handles all the Tuneup options tabs messages 
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

BOOL TuneTab::HandleMsg(DialogMsg* Msg)
{
TRACEUSER( "Neville", _T("HandleTuneUpMsg\n"));
	ERROR2IF(Msg == NULL,FALSE,"TuneTab::Message null message received");
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::HandleMsg called with no dialog pointer");

	BOOL ok = pPrefsDlg->TalkToPage(_R(IDD_OPTSTAB_TUNE));	// The tune page identifier
	if (!ok)
		return TRUE;		// no page present = handled message
	
	switch(Msg->DlgMsg)
	{
		case DIM_CREATE:	// Initialise controls
			GreyStatus = FALSE;	// we are ungreyed by default
			ok = InitSection();
			if (!ok)
				InformError();
			break;
		case DIM_LFT_BN_CLICKED:
			OptionsTabs::SetApplyNowState(TRUE);
			if ((Msg->GadgetID == _R(IDC_OPTS_USEMAXMEMORY)) || (Msg->GadgetID == _R(IDC_OPTS_USELIMITMEM)))
			{
				BOOL Valid;
				BOOL UseMaximumMemory = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_USEMAXMEMORY), 0, 1, 0, &Valid);
				// If set then grey the editable field and its name text otherwise ungrey
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_MEMLIMITTXT), !UseMaximumMemory);
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_TEMPWORKMEM), !UseMaximumMemory);
			}
			else if ((Msg->GadgetID ==  _R(IDC_OPTS_UNLIMITEDUNDO)) || (Msg->GadgetID == _R(IDC_OPTS_LIMITEDUNDO)))
			{
				BOOL Valid;
				BOOL Unlimited = pPrefsDlg->GetLongGadgetValue(_R(IDC_OPTS_UNLIMITEDUNDO), 0, 1, 0, &Valid);
				// If set then grey the editable field and its name text otherwise ungrey
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOLIMITTXT), !Unlimited);
				pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), !Unlimited);
			}
		case DIM_SELECTION_CHANGED:
		case DIM_TEXT_CHANGED:
			OptionsTabs::SetApplyNowState(TRUE);
			break;
		default:
			break;
	}
	return TRUE;
}  

/******************************************************************************************

>	BOOL TuneTab::InitRenderingSection()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the GDraw rendering section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL TuneTab::InitRenderingSection()
{
	TRACEUSER( "Neville", _T("TuneTab::InitRenderingSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::InitRenderingSection called with no dialog pointer");

	BOOL ReadOk = TRUE; 	// Flag to say whether the preference value was read ok 
	BOOL ok	= TRUE;			// Flag for whether value set up ok 

	// The Maximum temporary workspace available to Gdraw
	UINT32 TempWorkMem = TunedMemory::GetLimitedMemSize();
//	UINT32 TempWorkMemK = TempWorkMem / 1024;
	BOOL UseMaximumMemory = TunedMemory::IsAutomaticMemory();

	// Limit the values											   
	if (TempWorkMem > MAX_TUNEUPS_MEMORY)
	{
		ReadOk=FALSE;
		TRACEUSER( "Neville", _T("value too large, must be < 32M\n"));
		// Error - Value too large
		ERROR2(2,_R(IDE_OPTS_READPREF_TUNE));
	}

	if (ReadOk && (TempWorkMem < MIN_TUNEUPS_MEMORY) && (TempWorkMem != 0) )
	{
		ReadOk=FALSE;
		TRACEUSER( "Neville", _T("value too small - must be 1024 bytes or greater\n"));
		// Error - Value too small
		ERROR2(2,_R(IDE_OPTS_READPREF_TUNE));
	}
	 
	// Find out how much ram is needed for a full screen transparency blit
	INT32 FullPageMem = TunedMemory::GetScreenMemSize();
 	ok = pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_FULLPAGEMEM), FullPageMem);

	// Format a string to relate the amount of memory put aside
	// Display the chosen value in the editable field
	pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_TEMPWORKMEM), TempWorkMem);

	// Set up the use maximum/limited memory radio buttons		  
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_USEMAXMEMORY), UseMaximumMemory);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_USELIMITMEM), !UseMaximumMemory);

	// If set then grey the editable field and its name text
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_MEMLIMITTXT), !UseMaximumMemory);
	pPrefsDlg->EnableGadget(_R(IDC_OPTS_TEMPWORKMEM), !UseMaximumMemory);
		
 	return TRUE;
}

/******************************************************************************************

>	BOOL TuneTab::InitUndoSection()

	Author:		Neville_Humphrys (Xara Group Ltd) <camelotdev@xara.com>
	Created:	20/1/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Undo section of the options dialog box.
				This section includes the:-
				- Unlimited undo memory size switch
				- OR maximum memory allowed for undo system 
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL TuneTab::InitUndoSection()
{
TRACEUSER( "Neville", _T("TuneTab::InitUndoSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::InitUndoSection called with no dialog pointer");

//	BOOL ReadOk = FALSE; 	// Flag to say whether the preference value was read ok 

	// Section = Undo settings

	// Set up the display for the current size of the undo buffer.
	Document* pDocument = Document::GetSelected();
	if (pDocument != NULL)
	{
		// Get the current undo size
		UINT32 UndoSize = pDocument->GetOpHistory().GetSize();
		
		// Put that size in a string formatted as a memory size	value
		// and show that on the dialog box
		pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_UNDOSIZE), UndoSize);

		// Now get the currently set maximum size
		UINT32 MaxUndoSize = pDocument->GetOpHistory().GetMaxSize();

		OldMaxUndoSize = MaxUndoSize;

		// If set at the maximum size possible then assume unlimited
		BOOL Unlimited = FALSE;
		if (MaxUndoSize == UINT32_MAX)
		{
			// If at maximum then we are in the unlimited state		
			Unlimited = TRUE;
			// Force in a much more sensible value
			MaxUndoSize = 256 * 1024;			
		}
		else
			Unlimited = FALSE;

		// Put that size in a string formatted as a memory size	value
		// and show that on the dialog box
		pPrefsDlg->SetMemoryGadgetValue(_R(IDC_OPTS_MAXUNDOSIZE), MaxUndoSize);

		// Set the unlimited/limited radio button states
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_UNLIMITEDUNDO), Unlimited);
		pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_LIMITEDUNDO), !Unlimited);
		
		// Grey or ungrey the editable field and its name text accordingly
		pPrefsDlg->EnableGadget(_R(IDC_OPTS_UNDOLIMITTXT), !Unlimited);
		pPrefsDlg->EnableGadget(_R(IDC_OPTS_MAXUNDOSIZE), !Unlimited);

TRACEUSER( "Neville", _T("undo size ='%d'\n"),UndoSize);
TRACEUSER( "Neville", _T("max undo size ='%d'\n"),MaxUndoSize);
	}
	else
	{
		// Ensure our section is greyed
		GreySection();
	}

	return TRUE;
}

/******************************************************************************************

>	BOOL TuneTab::InitSection()

	Author:		Richard_Millican (Xara Group Ltd) <camelotdev@xara.com>
	Created:	19/01/95
	Inputs:		-
	Outputs:	-
	Returns:	TRUE if successful, else FALSE
	Purpose:	Sets initial values for the Tuneups section of the options dialog box.
	Errors:		-
	SeeAlso:	AppPrefsDlg; OptionsTabs;

******************************************************************************************/

BOOL TuneTab::InitSection()
{
TRACEUSER( "Neville", _T("TuneTab::InitSection\n"));
	ERROR2IF(pPrefsDlg == NULL,FALSE,"TuneTab::InitSection called with no dialog pointer");

	// Make sure the information field displaying the name of the current document
	// is correct.
	String_256	DocumentName(_R(IDT_OPTS_UNDO_INFO)); 
	DocumentName +=	*GetDocumentName();
	pPrefsDlg->SetStringGadgetValue(_R(IDC_OPTS_UNDOINFO), DocumentName);

	InitRenderingSection();

	// Section = Gallery Cache size

	INT32 CacheSize = 1;
	BOOL ReadOk = Camelot.GetPrefValue(TEXT("Libraries"), TEXT("Thumbnails"), &CacheSize);
	pPrefsDlg->SetLongGadgetValue(_R(IDC_OPTS_CACHESIZE), CacheSize);
	ERROR2IF(!ReadOk,FALSE,_R(IDE_OPTS_READPREF_TUNE));

	InitUndoSection();
	
	// --------------------------------------------------------------------------
	// Bitmap cacheing
	///
	BOOL bCacheingEnabled = FALSE;
	Camelot.GetPrefValue(TEXT("Rendering"), TEXT("Cacheing"), &bCacheingEnabled);
	pPrefsDlg->SetBoolGadgetSelected(_R(IDC_CHECK_CACHE_ENABLED), bCacheingEnabled);

	INT32 iCacheRAMPercent = 0;
	Camelot.GetPrefValue(TEXT("Cache"), TEXT("CacheRAMPercent"), &iCacheRAMPercent);
	TCHAR Str[32];
	String_32 temp(_R(IDS_PERCENT_FORMAT));
	camSnprintf(Str, 32, temp, iCacheRAMPercent);
	String_32 PercentStr(Str);
	pPrefsDlg->SetStringGadgetValue(_R(IDC_EDIT_CACHE_SIZE), PercentStr);

	CBitmapCache* pCache = GetApplication()->GetBitmapCache();
	if (pCache)
	{
		// This value should really be rounded!
		INT32 iCacheUsagePercent = ((UINT64)pCache->GetCurrentDataSize()*100)/pCache->GetMaximumDataSize();

		TCHAR Str[32];
		String_32 temp(_R(IDS_PERCENT_OF_FORMAT));
		String_32 strCacheSize;
		Convert::BytesToString(&strCacheSize, (UINT32)pCache->GetMaximumDataSize());
		camSnprintf(Str, 32, temp, iCacheUsagePercent, (TCHAR*)strCacheSize);
		String_32 PercentStr(Str);
		pPrefsDlg->SetStringGadgetValue(_R(IDC_EDIT_CURRENTCACHE), PercentStr);

//		pPrefsDlg->SetLongGadgetValue(_R(IDC_PROGRESS_CACHEUSAGE), iCacheUsagePercent);		// TODO: Should be percentage
	}

	return TRUE;
}

#endif //webster
